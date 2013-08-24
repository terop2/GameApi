// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//

#include <iostream>
#include <algorithm>
#include "Monoid.hh"

class StringRef
{
public:
  StringRef() : ptr(0), length(0) { }
  StringRef(const std::string &s) : ptr(&s[0]), length(s.size()) { }
  StringRef(const StringRef &s, int start, int length) : ptr(&s[start]), length(length) { }
  //char &operator[](int index) { return ptr[index]; }
  const char &operator[](int index) const { return ptr[index]; }
  int size() const { return length; }
  StringRef substr(int start, int length) const { return StringRef(*this, start, length); }
  StringRef extend(const std::string &alphabet) const
  {
    for(int i=0;i<length;i++)
      {
	std::string::const_iterator ii = std::find(alphabet.begin(), alphabet.end(), ptr[i]);
	if (ii==alphabet.end())
	  {
	    return StringRef(*this, 0,i);
	  }
      }
    return StringRef(*this, 0, length);
  }
  static std::string intersect(const std::string &s, const std::string &s2)
  {
    std::string res;
    std::set_intersection(s.begin(), s.end(), s2.begin(), s2.end(), back_inserter(res));
    return res;
  }
  StringRef &operator=(const StringRef &ref)
  {
    ptr = ref.ptr;
    length = ref.length;
    return *this;
  }
  friend std::ostream &operator<<(std::ostream &o, const StringRef &ref)
  {
    for(int i=0;i<ref.length;i++)
      {
	o << ref.ptr[i];
      }
    return o;
  }
private:
  const char *ptr;
  int length;
};

//
// LANGUAGE
//

class Language
{
public:
  virtual ~Language() { }
  virtual std::string Alphabet() const=0;
  virtual bool Match(const StringRef &s) const=0;

  virtual int MatchResultCount() const=0;
  virtual StringRef MatchResult(int i) const=0;

};
class StringRefArray : public Array<int, char>
{
public:
  StringRefArray(const StringRef &s) : s(s) { }
  char Index(int i) const
  {
    return s[i];
  }
  int Size() const { return s.size(); }
private:
  const StringRef &s;
};

class MatchResultArray : public ArrayArray<int, int, char>
{
public:
  MatchResultArray(Language &l) : l(l), refarray(0) { }
  Array<int, char> *Index(int i) const
  {
    delete refarray;
    refarray = new StringRefArray(l.MatchResult(i));
    return refarray;
  }
  int Size() const
  {
    return l.MatchResultCount();
  }
  ~MatchResultArray() { delete refarray; }
private:
  Language &l;
  mutable StringRefArray *refarray;
};


class EmptyLanguage : public Language
{
public:
  virtual std::string Alphabet() const { return ""; }
  bool Match(const StringRef &s) const { return s.size()==0; }
  virtual int MatchResultCount() const { return 0; }
  virtual StringRef MatchResult(int i) const { return StringRef(""); }
};

class CharLanguage : public Language
{
public:
  CharLanguage(char c) : c(c) { }
  virtual std::string Alphabet() const { return std::string(c,1); }
  bool Match(const StringRef &s) const
  {
    if (s.size()!=1) return false;
    if (s[0]!=c) return false;
    return true;
  }
  virtual int MatchResultCount() const { return 0; }
  virtual StringRef MatchResult(int i) const { return StringRef(""); }
private:
  char c;
};


class ComposeLanguage : public Language
{
public:
  ComposeLanguage(Language &l1, Language &l2) : l1(l1), l2(l2) { }
  virtual std::string Alphabet() const 
  {
    std::string s;
    s+=l1.Alphabet();
    s+=l2.Alphabet();
    std::sort(s.begin(), s.end());
    std::unique(s.begin(), s.end());
    return s;
  }
  bool Match(const StringRef &s) const
  {
    const std::string &s1 = l1.Alphabet();
    const std::string &s2 = l2.Alphabet();
    std::string intersect = StringRef::intersect(s1,s2);

    if (intersect.size()==0)
      {
    StringRef str = s.extend(l1.Alphabet());
    int i = str.size();
    //for(int i=0;i<=s.size();i++)
    //  {
	StringRef start = s.substr(0,i);
	StringRef end = s.substr(i,s.size()-i);
	//std::cout << "Try Compose Split: " << start << ";" << end << std::endl;
	bool smatch = l1.Match(start);
	bool ematch = l2.Match(end);
	if (smatch&&ematch) 
	  {
	    num = i;
	    m1 = start;
	    m2 = end;
	    //std::cout << "Compose Split: " << start << ";" << end << std::endl;
	  return true;
	  }
      }
    else
      {
	for(int i=0;i<=s.size();i++)
	  {
	    StringRef start = s.substr(0,i);
	    StringRef end = s.substr(i,s.size()-i);
	    bool smatch = l1.Match(start);
	    bool ematch = l2.Match(end);
	    if (smatch&&ematch) 
	      {
		num = i;
		m1 = start;
		m2 = end;
		//std::cout << "Compose Split: " << start << ";" << end << std::endl;
		return true;
	      }
	  }
      }
    return false;
  }
  int MatchResultCount() const { return 2; }
  StringRef MatchResult(int i) const
  {
    if (i==0)
      {
	return m1;
      }
    else
      {
	return m2;
      }
  }

#if 0
  void Set(int ii, const StringRef &s) const
  {
    Match(s);
    StringRef start = s.substr(0,num);
    StringRef end = s.substr(num,s.size()-num);
    if (ii==0) { l1.Set(ii, start); }
    if (ii==1) { l2.Set(ii,end); }
    std::cout << "ComposeLanguage:: Set error!" << ii << " " << start << " " << end  << std::endl;
  }
#endif
private:
  mutable int num;
  Language &l1, &l2;
  mutable StringRef m1,m2;
};

class LanguageMonoid : public Monoid2<Language*>
{
public:
  Language* empty() const { return new EmptyLanguage; }
  Language* compose(Language* a, Language *b) const
  {
    return new ComposeLanguage(*a, *b);
  }
};


class ComposeArray : public Language
{
public:
  template<class It>
  ComposeArray(It beg, It end) : vec(beg,end) { }
  ComposeArray() { }
  void push_back(Language *lang) { vec.push_back(lang); }
  virtual std::string Alphabet() const 
  {
    std::string s;
    for(std::vector<Language*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	s+=(*i)->Alphabet();
      }
    std::sort(s.begin(), s.end());
    std::unique(s.begin(), s.end());
    return s;
  }

  bool Match(const StringRef &s) const
  {
    if (vec.size()==0)
      {
	EmptyLanguage l;
	return l.Match(s);
      }
    if (vec.size()==1)
      {
	return vec[0]->Match(s);
      }
    if ((vec.size()&1)==1)
      { // odd count
	ComposeArray arr(vec.begin(), vec.end()-1);
	ComposeLanguage l(arr, **(vec.end()-1));
	return l.Match(s);
      }
    else
      {
	std::vector<Language*> v;
	for(unsigned int i=0;i<vec.size();i+=2)
	  {
	    Language *comp = new ComposeLanguage(*(vec[i]), *(vec[i+1]));
	    v.push_back(comp);
	  }
	ComposeArray arr(v.begin(), v.end());
	bool b = arr.Match(s);
	for(unsigned int ii=0;ii<v.size();ii++)
	  {
	    delete v[ii];
	  }
	return b;
      }
    
  }
  virtual int MatchResultCount() const
  {
    return vec.size();
  }
  virtual StringRef MatchResult(int i) const
  {
    return StringRef(""); // TODO... NOT WORKING...
  }

private:
  std::vector<Language*> vec;
};

class OrLanguage2 : public Language
{
public:
  OrLanguage2(Language &l1, Language &l2) : l1(l1), l2(l2) { }
  bool First() const { return b1; }
  bool Second() const { return b2; }

  virtual std::string Alphabet() const 
  {
    std::string s;
    s+=l1.Alphabet();
    s+=l2.Alphabet();
    std::sort(s.begin(), s.end());
    std::unique(s.begin(), s.end());
    return s;
  }
  bool Match(const StringRef &s) const
  {
    b1 = false;
    b2 = false;
    if (l1.Match(s)){ b1=true; m=s; return true; }
    if (l2.Match(s)){ b2=true; m=s; return true; }
    return false;
  }
  virtual int MatchResultCount() const { return 1; }
  virtual StringRef MatchResult(int i) const { return m; }
private:
  Language &l1, &l2;
  mutable StringRef m;
  mutable bool b1,b2;
};
class OrLanguage : public Language
{
public:
  template<class It>
  OrLanguage(It beg, It end) : vec(beg,end) { }
  OrLanguage() { }
  void push_back(Language *lang) { vec.push_back(lang); }
  int Num() const { return num; }
  virtual std::string Alphabet() const 
  {
    std::string s;
    for(std::vector<Language*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	s+=(*i)->Alphabet();
      }
    std::sort(s.begin(), s.end());
    std::unique(s.begin(), s.end());
    return s;
  }

  bool Match(const StringRef &s) const
  {
    num = 0;
    int ii = 0;
    for(std::vector<Language*>::const_iterator i = vec.begin();i!=vec.end();i++, ii++)
      {
	if ((*i)->Match(s)) 
	  {
	    num = ii; 
	    m=s;
	    //std::cout << "Or matched " << s << " " << num << std::endl;	
	    return true; 
	  }
      }
    return false;
  }
  virtual int MatchResultCount() const { return 1; }
  virtual StringRef MatchResult(int i) const { return m; }  
private:
  std::vector<Language*> vec;
  mutable StringRef m;
  mutable int num;
};

class Digit : public Language
{
public:
  Digit() 
    : d0('0'), d1('1'), d2('2'), d3('3'), d4('4'), d5('5'), d6('6'), 
      d7('7'), d8('8'), d9('9')
  {
    or_lang.push_back(&d0);
    or_lang.push_back(&d1);
    or_lang.push_back(&d2);
    or_lang.push_back(&d3);
    or_lang.push_back(&d4);
    or_lang.push_back(&d5);
    or_lang.push_back(&d6);
    or_lang.push_back(&d7);
    or_lang.push_back(&d8);
    or_lang.push_back(&d9);
  }
  int Num() const { return or_lang.Num(); }
  bool Match(const StringRef &s) const
  {
    return or_lang.Match(s);
  }
  virtual int MatchResultCount() const { return or_lang.MatchResultCount(); }
  virtual StringRef MatchResult(int i) const { return or_lang.MatchResult(i); }
  virtual std::string Alphabet() const 
  {
    return "0123456789";
  }

private:
  CharLanguage d0,d1,d2,d3,d4,d5,d6,d7,d8,d9;
  OrLanguage or_lang;
};

class Repeat : public Language
{
public:
  Repeat(int i, Language &lang) : i(i), lang(lang), repeat_n(rep(i,lang)), comp(*repeat_n, lang) 
  {
  }
  ~Repeat() { if (i != 2) delete repeat_n; }
  Language *rep(int i, Language &lang) const
  {
    if (i==0)
      {
	return 0;
      }
    if (i==1)
      {
	return 0;
      }
    if (i==2)
      {
	return &lang;
      }
    else
      {
	return new Repeat(i-1, lang);
      }
  }
  virtual std::string Alphabet() const 
  {
    return lang.Alphabet();
  }
  bool Match(const StringRef &s) const
  {
    if (i==0) return el.Match(s);
    if (i==1) { m = s; return lang.Match(s); } 
    return comp.Match(s);
  } 
  virtual int MatchResultCount() const { return i; }
  virtual StringRef MatchResult(int ii) const 
  {
    if (i==1)
      {
	return m;
      }
    return repeat_n->MatchResult(ii);
  }
#if 0
  void Set(int ii, const StringRef &s) const
  {
    if (ii==0 || !repeat_n)
      {
	lang.Match(s);
      }
    else
      {
	return comp.Set(ii,s);
      }
  }
#endif  
private:
  int i;
  Language &lang;
  Language *repeat_n;
  ComposeLanguage comp;
  EmptyLanguage el;
  mutable StringRef m;
};

class StarLanguage : public Language
{
public:
  StarLanguage(Language &lang, int maxiterations) : lang(lang), maxiterations(maxiterations) { }
  int Num() const { return num; }
  virtual std::string Alphabet() const 
  {
    return lang.Alphabet();
  }

  bool Match(const StringRef &s) const
  {
    for(int i=0;i<maxiterations;i++) // magic number 20. Should be larger probably.
      {
	Repeat r(i, lang);
	if (r.Match(s)) 
	  {
	    m = s;
	    //std::cout << "Star match " << s << " " << i << std::endl;
	    num = i; return true; 
	  }
      }
    return false; // never will be done..
  }
  virtual int MatchResultCount() const
  {
    return 1;
  }
  virtual StringRef MatchResult(int i) const
  {
    return m;
  }

#if 0
  void Set(int i, const StringRef &s) const
  {
    Repeat r(i,lang);
    r.Set(i, s);
  }
#endif
private:
  Language &lang;
  mutable int num;
  int maxiterations;
  mutable StringRef m;
}; 

class Integer : public Language
{
public:
  Integer() : s(d,20) { }
  bool Match(const StringRef &ss) const { ref=&ss; return s.Match(ss); }
  int Num() const
  {
    StringRef mr = MatchResult(0);
    int val=0;
    std::cout << mr.size() << std::endl;
    for(int i=0;i<mr.size();i++)
      {
	val+=mr[i]-'0';
	val*=10;
      }
    val/=10;
    return val;
  }
  virtual int MatchResultCount() const { return s.MatchResultCount(); }
  virtual StringRef MatchResult(int i) const
  {
    return s.MatchResult(i); 
  }
  virtual std::string Alphabet() const 
  {
    return s.Alphabet();
  }
private:
  Digit d;
  StarLanguage s;
  mutable const StringRef *ref;
};

class ArrayLanguage : public Language
{
public:
  ArrayLanguage(Language &lang, Language &separator) : lang(lang), sep(separator), comp(lang, sep), star(comp, 20), comp2(star, lang)
  {
  }
  virtual std::string Alphabet() const 
  {
    std::string s;
    s+=lang.Alphabet();
    s+=sep.Alphabet();
    std::sort(s.begin(), s.end());
    std::unique(s.begin(), s.end());
    return s;
  }

  virtual bool Match(const StringRef &s) const
  {
    return comp2.Match(s);
  }

  virtual int MatchResultCount() const
  {
    return comp2.MatchResultCount();
  }
  virtual StringRef MatchResult(int i) const
  {
    return comp2.MatchResult(i);
  }
  
private:
  Language &lang;
  Language &sep;
  ComposeLanguage comp;
  StarLanguage star;
  ComposeLanguage comp2;
};

// state and event are enums.
template<class State, class Event>
class StateMachine
{
public:
  virtual int NumStates() const = 0;
  virtual State Initial() const = 0;
  virtual bool Valid(State state, Event event) const=0;
  virtual State Transition(State state, Event event) const = 0;
  virtual bool IsFinal(State state) const = 0;
};

class CharStateMachine : public StateMachine<int, char>
{
public:
  CharStateMachine(char c) : c(c) { }
  int NumStates() const { return 2; }
  int Initial() const { return 0; }
  bool Valid(int state, char e) const 
  {
    if (e==c && state == 0)
      return true;
    return false;
  }
  int Transition(int state, char e) const
  {
    if (e==c && state==0) return 1;
    return 0;
  }
  bool IsFinal(int state) const
  {
    if (state==1) return true;
    return false;
  }
private:
  char c;
};

template<class Event>
class ConcatStateMachine : public StateMachine<int, Event>
{
public:
  ConcatStateMachine(StateMachine<int, Event> &sm1,
		     StateMachine<int, Event> &sm2) 
    : sm1(sm1), sm2(sm2) { }
  int NumStates() const { return sm1.NumStates()+sm2.NumStates(); }
  int Initial() const { return sm1.Initial(); }
  bool Valid(int state, Event event) const
  {
    int i = sm1.NumStates();
    if (state < i)
      { // still in sm1
	if (sm1.IsFinal(state))
	  {
	    return sm2.Valid(sm2.Initial(), event);
	  }
	else
	  {
	    return sm1.Valid(state, event);
	  }
      }
    else
      {
	return sm2.Valid(state-i, event);
      }
  }
  int Transition(int state, Event event) const
  {
    int i = sm1.NumStates();
    if (state < i)
      { // still in sm1
	if (sm1.IsFinal(state))
	  { // move to sm2
	    return i+sm2.Transition(sm2.Initial(), event);
	  }
	else
	  {
	    return sm1.Transition(state, event);
	  }
      }
    else
      { // in sm2
	return i+sm2.Transition(state-i, event);
      }
  }
  bool IsFinal(int state) const
  {
    int i = sm1.NumStates();
    if (state < i) return false;
    if (sm2.IsFinal(state-i)) return true;
    return false;
  }
private:
  StateMachine<int, Event> &sm1;
  StateMachine<int, Event> &sm2;
};

template<class Event>
class UnionStateMachine : public StateMachine<int,Event>
{
public:
  UnionStateMachine(StateMachine<int, Event> &sm1,
		    StateMachine<int, Event> &sm2) 
    : sm1(sm1), sm2(sm2) { }
  int NumStates() const 
  {
    return sm1.NumStates()+sm2.NumStates()+1;
  }
  int Initial() const
  {
    return 0;
  }
  bool Valid(int state, Event event) const
  {
    if (state==0)
      {
	if (sm1.Valid(state, event)) return sm1.Valid(state,event);
	if (sm2.Valid(state, event)) return sm2.Valid(state,event);
	return false;
      }
    else
      {
	int i = sm1.NumStates();
	if (state < i+1)
	  {
	    return sm1.Valid(state-1, event);
	  }
	else
	  {
	    return sm2.Valid(state-i-1, event);
	  }
      }
  }
  int Transition(int state, Event event) const
  {
    if (state==0)
      {
	if (sm1.Valid(state,event)) return sm1.Transition(state,event);
	if (sm2.Valid(state,event)) return sm2.Transition(state,event);
	return 0;
      }
    else
      {
	int i = sm1.NumStates();
	if (state < i+1)
	  {
	    return 1+sm1.Transition(state-1,event);
	  }
	else
	  {
	    return i+1+sm2.Transition(state-i-1, event);
	  }
      }
  }
  bool IsFinal(int state) const
  {
    int i = sm1.NumStates();    
    if (state==0)
      {
	return false;
      }
    else if (state < i+1)
      {
	return sm1.IsFinal(state-1);
      }
    else
      {
	return sm2.IsFinal(state-i-1);
      }
  }
  
private:
  StateMachine<int, Event> &sm1;
  StateMachine<int, Event> &sm2;
};

template<class State, class Event>
class ModifiableStateMachine
{
public:
  virtual State AddState() = 0;
  virtual void AddTransition(State state_a, State state_b, Event event) = 0;
  virtual void MarkFinal(State state) = 0;
  virtual void ChangeInitial(State state) = 0;
};

template<class State, class Event>
class StateMachineState
{
public:
  StateMachineState(StateMachine<State,Event> &sm) :  current_state(sm.Initial()), sm(sm) {}
  void Reset() { current_state = sm.Initial(); }
  bool ValidEvent(Event event) const
  {
    return sm.Valid(current_state, event);
  }
  void SendEvent(Event event)
  {
    current_state = sm.Transition(current_state, event);
  }
  bool Final() const { return sm.IsFinal(current_state); }
private:
  State current_state;
  StateMachine<State,Event> &sm;
};

class StateMachineLanguage : public Language
{
public:
  StateMachineLanguage(StateMachine<int, char> &sm, const std::vector<char> &events) : sm(sm), events(events) { }
  virtual std::string Alphabet() const
  {
    return std::string(events.begin(), events.end());
  }
  virtual bool Match(const StringRef &s) const
  {
    StateMachineState<int,char> state(sm);
    for(int i=0;i<s.size();i++)
      {
	state.SendEvent(s[i]);
      }
    return state.Final();
  }

  virtual int MatchResultCount() const { return 0; }
  virtual StringRef MatchResult(int i) const { return StringRef(""); }

private:
  StateMachine<int,char> &sm;
  std::vector<char> events;
};


//
//
// PROPER STATE MACHINE
//
//

struct State
{
public:
  void AddLink(State *st) { links.push_back(st); link_events.push_back('@'); }
  void AddLink(char c, State *st) { links.push_back(st); link_events.push_back(c); }
  void SetFinal(bool f) { final = f; }
  void SetStartGroup(bool b) { startgroup = b; }
  void SetEndGroup(bool b) { endgroup = b; }
public:
  bool IsStartGroup(char /*cc*/) const
  {
    int size = link_events.size();
    for(int i=0;i<size;i++)
      {
	char c = link_events[i];
	if (c=='@') {
	  bool b = links[i]->IsStartGroup('*');
	  if (b) return true;
	}
	//if (c==cc) return startgroup;
      }
    return startgroup;    
  }
  bool IsEndGroup(char /*cc*/) const
  {
    int size = link_events.size();
    for(int i=0;i<size;i++)
      {
	char c = link_events[i];
	if (c=='@') {
	  bool b = links[i]->IsEndGroup('*');
	  if (b) return true;
	}
	//if (c==cc) return endgroup;
      }
    return endgroup; 
    
  }
  bool IsFinal() const 
  {
    int size = link_events.size();
    for(int i=0;i<size;i++)
      {
	char c = link_events[i];
	if (c=='@') {
	  bool b = links[i]->IsFinal();
	  if (b) return true;
	}
      }
    return final; 
  }
  std::vector<char> Lookup() const
  {
    std::vector<char> res;
    int size = link_events.size();
    for(int i=0;i<size;i++)
      {
	char c = link_events[i];
	if (c=='@') { 
	  std::vector<char> cc = links[i]->Lookup();
	  int size = cc.size();
	  for(int k=0;k<size;k++)
	    {
	      res.push_back(cc[k]);
	    }
	} else {
	  res.push_back(c);
	}	
      }
    return res;
  }
  State *Follow(char ca) const
  {
    int size = link_events.size();
    for(int i=0;i<size;i++)
      {
	char c = link_events[i];
	if (c=='@') {
	  State *st = links[i]->Follow(ca);
	  if (st) return st;
	}
	if (c==ca) return links[i];
      }
    return 0;
  }
private:
  std::vector<State*> links;
  std::vector<char> link_events;
  bool final;
  bool startgroup;
  bool endgroup;
};

typedef std::pair<State*, State*> SM;


SM CharacterMatch(char c)
{
  State *first = new State;
  State *second = new State;
  first->AddLink(c, second);
  return std::make_pair(first, second);
}

SM Concatenation(SM p1, SM p2)
{
  p1.second->AddLink('@', p2.first);
  return std::make_pair(p1.first, p2.second);
}

SM Repeat(SM p1)
{
  p1.second->AddLink('@', p1.first);
  return std::make_pair(p1.first, p1.second);
}

SM Or(SM p1, SM p2)
{
  State *first = new State;
  first->AddLink('@', p1.first);
  first->AddLink('@', p2.first);

  State *last = new State;
  p1.second->AddLink('@', last);
  p2.second->AddLink('@', last);
  return std::make_pair(first,last);
}

SM String(std::string s)
{
  if (s.size()==0)
    {
      State *first = new State;
      return std::make_pair(first,first);
    }
  char c = s[0];
  SM csm = CharacterMatch(c);
  int size = s.size();
  for(int i=1;i<size;i++)
    {
      SM a = CharacterMatch(s[i]);
      csm = Concatenation(csm,a);
    }
  return csm;
}

SM Grouping(SM s)
{
  State *startgroup = new State;
  startgroup->SetStartGroup(true);
  startgroup->AddLink('@', s.first);
  State *endgroup = new State;
  endgroup->SetEndGroup(true);
  s.second->AddLink('@', endgroup);
  return std::make_pair(startgroup, endgroup);
}

SM Any(std::vector<char> alphabet)
{
  if (alphabet.size()==0)
    {
      State *first = new State;
      return std::make_pair(first,first);
    }
  char c = alphabet[0];
  SM csm = CharacterMatch(c);
  int size = alphabet.size();
  for(int i=1;i<size;i++)
    {
      SM a = CharacterMatch(alphabet[i]);
      csm = Or(csm,a);
    }
  return csm;
}

int find(const std::vector<char> &c, char cc)
{
  int s = c.size();
  for(int i=0;i<s;i++)
    {
      if (c[i]==cc) return i;
    }
  return -1;
}

SM Not(std::vector<char> alphabet, std::vector<char> not_included)
{
  if (alphabet.size()==0)
    {
      State *first = new State;
      return std::make_pair(first,first);
    }
  char c = alphabet[0];
  SM csm = CharacterMatch(c);
  int size = alphabet.size();
  for(int i=1;i<size;i++)
    {
      if (find(not_included, alphabet[i]) != -1)
	{
	  continue;
	}	  
      SM a = CharacterMatch(alphabet[i]);
      csm = Or(csm,a);
    }
  return csm;  
}
SM Final(SM s)
{
  s.second->SetFinal(true);
  return s;
}

bool Match(SM sm, const std::string &ss, std::vector<std::string> &res)
{
  std::vector<std::string> stack;

  State *st = sm.first;
  if (!st) return false;

  int sz = ss.size();
  for(int i=0;i<sz;i++)
    {
      //std::cout << "Loop count: " << i << std::endl;
      if (st->IsStartGroup(ss[i]))
	{
	  //std::cout << "STARTGROUP" << std::endl;
	  stack.push_back("");
	}
      if (stack.size())
	{
	  //std::cout << "Inserting: " << ss[i] << std::endl;
	  stack[stack.size()-1]+=ss[i];
	}
      st = st->Follow(ss[i]);
      if (st->IsEndGroup(ss[i]))
	{
	  //std::cout << "ENDGROUP" << std::endl;
	  res.push_back(stack.back());
	  stack.pop_back();
	}
      if (!st) return false;

    }

  if (st->IsFinal()) return true;
  return false;
}

//
// Tests
//
void testlang()
{
  SM s = String("abcdef");
  SM st = Repeat(s);
  SM ss = String("ppppp");
  SM s2 = Grouping(st);
  SM ss2 = Grouping(ss);
  SM sc = Concatenation(s2,ss2);
  SM s3 = Final(sc);
  std::vector<std::string> res;
  bool b = Match(s3, "abcdefabcdefppppp", res);
  if (b)
    std::cout << "MATCH!" << std::endl;
  int size = res.size();
  for(int i=0;i<size;i++)
    {
      std::cout << "P " << res[i] << std::endl;
    }
}
