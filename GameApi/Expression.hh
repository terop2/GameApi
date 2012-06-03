#include "Effect.hh"
#include <vector>


template<class T, class K>
class Duplicate
{
public:
  Duplicate(T t) : t(t) { }
  std::vector<K> Elem()
  {
    std::vector<K> vec;
    int s=funcs.size();
    for(int i=0;i<s;i++)
      {
	Function<T,K> *func = funcs[i];
	vec.push_back(func->Index(t));
      }
    return vec;
  }
  void push_back(Function<T,K> *func) { funcs.push_back(func); }
private:
  T t;
  std::vector<Function<T,K>*> funcs;
};

template<class T, class K>
class Duplicate2Array : public Array<int,K>
{
public:
  Duplicate2Array(T t, Array<int,Function<T,K>*> &arr) : t(t), arr(arr) { }
  int Size() const
  {
    return arr.Size();
  }
  K Index(int i) const
  {
    Function<T,K>* func = arr.Index(i);
    return func->Index(t);
  }
  
private:
  T t;
  Array<int,Function<T,K>*> &arr;
};

template<class T, class K>
class Duplicate2
{
public:
  Duplicate2(T t, Array<int, Function<T,K>*> &funcarr) : t(t), arr(t, funcarr) { }
  Array<int,K> *Elem()
  {
    return &arr;
  }
private:
  Duplicate2Array arr;
};

template<class T, class K>
class Duplicate3
{
public:
  Duplicate3(T t) : dup(t,vec) { }
  void push_back(Function<T,K> *func) { vec.push_back(func); }
  Array<int,K> *Elem()
  {
    return dup.Elem();
  }
priate:
  VectorArray vec;
  Duplicate2<T,K> dup;
};

class Duplicate4Array : public Array<int,T>
{
public:
  Duplicate4Array(int s, T t) : t(t), s(s) { }
  int Size() const { return s; }
  T Index(int i) const { return t; }
private:
  T t;
  int s;
};

template<class T>
class Duplicate4
{
public:
  Duplicate4(int s, T t) : arr(s,t) { }
  Array<int,T> *Elem() const
  {
    return &arr;
  }
private:
  Duplicate4Array arr;
};

//
//
// NEW EXPRESSION STUFF
//
template<class T>
class Value
{
public:
  virtual T get() const=0;
  virtual bool valid_get() const=0;
  virtual void set(T t)=0;
  virtual bool valid_set(T t) const=0;
};

template<class T, class K>
class X_choose_first : public Value<T>
{ // (x) |- (x,y)
public:
  X_choose_first(Value<std::pair<T,K> > &p) : p(p) { }
  virtual T get() const { return p.get().first; }
  bool valid_get() const { return p.valid_get(); }
  virtual void set(T t) 
  {
    p.set(std::make_pair(t, p.get().second));
  }
  virtual bool valid_set(T t) const 
  {
    return p.valid_set(std::make_pair(t, p.get().second));
  }

private:
  Value<std::pair<T,K> > &p;
};

template<class T, class K>
class X_choose_second : public Value<K>
{ // (y) |- (x,y)
public:
  X_choose_second(Value<std::pair<T,K> > &p) : p(p) { }
  virtual K get() const { return p.get().first; }
  bool valid_get() const { return p.valid_get(); }
  virtual void set(K k) 
  {
    p.set(std::make_pair(p.get().first, k));
  }
  virtual bool valid_set(K k) const 
  {
    return p.valid_set(std::make_pair(p.get().first,k));
  }

private:
  Value<std::pair<T,K> > &p;
};


class X_X_Value_Int : public Value<std::pair<int,int> >
{ // (x,x) : (int,int)
public:
  
  virtual std::pair<T,T> get() const { return p; }
  bool valid_get() const { return true; }
  virtual void set(std::pair<T,T> t) { p = t; }
  virtual bool valid_set(std::pair<T,T> t) const
  {
    return t.first == t.second;
  }
private:
  std::pair<T,T> p;
};


template<class T>
class X_Value : public Value<T>
{ // x |- (x)
public:
  X_Value() : m_t(0.0) { }
  X_Value(T t) : m_t(t) { }
  virtual T get() const { return m_t; } 
  bool valid_get() const { return true; }
  virtual void set(T t) { m_t = t; }
  virtual bool valid_set(T t) const { return true; }
private:
  T m_t;
};

template<class T>
class ConstantValue : public Value<T>
{
public:
  ConstantValue(T t) : t(t) { }
  virtual T get() const { return t; }
  bool valid_get() const { return true; }
  virtual void set(T t) { }
  virtual bool valid_set(T t) const { return true; }
private:
  T t;
};

template<class T>
class PlusValue : public Value<T>
{ // (x,y) |- (x+y)
public:
  PlusValue(Value<T> &t1, Value<T> &t2) : t1(t1), t2(t2) { }
  bool valid_get() const { return t1.valid_get() && t2.valid_get(); }
  virtual T get() const 
  {
    return t1.get() + t2.get();
  }  
  virtual void set(T t)
  {
    T tt = 0;
    for(T i=0;i<t;i++)
      {
	if (t1.valid_set(i) && t2.valid_set(t-i))
	  {
	    break;
	  }
      }
    t1.set(i); t2.set(t-i);
  }
  virtual bool valid_set(T t) const
  {
    T tt = 0;
    bool b = false;
    for(T i=0;i<t;i++)
      {
	if (t1.valid_set(i) && t2.valid_set(t-i))
	  {
	    b = true;
	    break;
	  }
      }
    return b;
  }
  
private:
  Value<T> &t1, &t2;
};

template<class T>
class Unit : public Value<T>
{ // u |- (u)
public:
  Unit(T &ref) : ref(ref) { }
  virtual T get() const { return ref; }  
  bool valid_get() const { return true; }
  virtual void set(T t) { }
  virtual bool valid_set(T t) const
  {
    return true;
  }
private:
  T &ref;
};

class T_Int_to_Float_conversion : public Value<float>
{ //(x) |- (y), F : Int->Float
public:
  T_Int_to_Float_conversion(Value<int> &val) : val(val) { }
  virtual float get() const { return (float)val.get(); }
  bool valid_get() const { return true; }

  virtual void set(float t)
  {
    val.set((int)t);
  }
  virtual bool valid_set(float t) const
  {
    int v = (int)t;
    float vf = (float) v;
    float delta = t - vf;
    return fabs(delta) < 0.001;
  }
private:
  Value<int> &val;
};

class T_Float_to_Int_conversion : public Value<int>
{ //(x) |- (y), F : Float->Int
public:
  T_Float_to_Int_conversion(Value<float> &val) : val(val) { }
  virtual int get() const { return (int)val.get(); }
  bool valid_get() const 
  { 
    int v = (int)val.get();
    float vf = (float) v;
    float delta = val.get() - vf;
    return fabs(delta) < 0.001;
  }

  virtual void set(int t)
  {
    val.set((float)t);
  }
  virtual bool valid_set(int t) const
  {
    return true;
  }
private:
  Value<float> &val;
};


class T_power_2_float : public Value<float>
{ // x |- (x^2)
public:
  T_power_2(Value<float> &val) : val(val) { }
  virtual float get() const { float t = val.get(); float t2 = t*t; return t2; }
  virtual void set(float t) { val.set(sqrt(t)); }
  virtual bool valid_set(float t) const
  {
    return val.valid_set(t);
  }
private:
  Value<float> &val;
};

template<class T>
class T_constant_multiplication : public Value<T>
{ // x |- (k*x)
public:
  T_constant_multiplication(Value<T> &x, Value<T> &k) : x(x), k(k) { }
  virtual T get() const { return k.get()*x.get(); }
  virtual void set(T t) { x.set(t/k.get()); }
  virtual bool valid_set(T t) const { return x.valid_set(t/k.get()); }
private:
  Value<T> &x;
  Value<T> &k;
};

void test_expr();
