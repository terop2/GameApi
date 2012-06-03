
#ifndef EDITOR_HH
#define EDITOR_HH

#include "Effect.hh"
#include <string>
#include <typeinfo>
#include <sstream>

template<class T>
void valuefy(std::string s, T &f);


class CreationObject
{
public:
  virtual std::string Name() const=0;
  virtual std::string Type() const=0;
  virtual void *GetValue() const=0;
  virtual void SetValue(void *val)=0;
};
template<class T>
class ValueCreationObject : public CreationObject
{
public:
  ValueCreationObject(std::string name, T t) : name(name), t(t) { }
  virtual std::string Name() const { return name; }
  virtual std::string Type() const { return typeid(T).name(); }
  virtual void *GetValue() const { return (void*)&t; }
  virtual void SetValue(void *val)
  {
    t = *(T*)val;
  }
  
private:
  std::string name;
  T t;
};


class CreationFunction
{
public:
  virtual int NumParams() const=0;
  virtual std::string Name() const=0;
  virtual std::string ReturnType() const=0;
  virtual std::string ParamType(int i) const=0;
  virtual void SetParam(int i, void *param)=0;
  virtual void Execute()=0;
  virtual void *GetReturn() const=0;

  virtual bool Appendable() const = 0;
  virtual void Append(void *param)=0;

  virtual bool Editable() const { return false; }
  virtual void Edit(std::string s) { }
};

template<class T>
class ValueCreationFunction : public CreationFunction
{
public:
  ValueCreationFunction(std::string name) : name(name) { }
  int NumParams() const { return 0; }
  std::string Name() const { return name; }
  std::string ReturnType() const { return typeid(T).name(); }
  std::string ParamType(int i) const { return ""; }
  void SetParam(int i, void *param) { }
  void Execute() { }
  void *GetReturn() const { return (void*)static_cast<const T*>(&value); }
  bool Appendable() const { return false; }
  void Append(void *param) { }
  bool Editable() const { return true; }
  void Edit(std::string s)
  {
    valuefy(s,value);
  }
private:
  T value;
  std::string name;
};

// T = P*
template<class T>
class CreationPointerArray : public CreationFunction
{
public:
  CreationPointerArray(std::string name) : name(name) { }
  int NumParams() const { return arr.Size()+1; }
  std::string Name() const { return name; }
  std::string ReturnType() const { return typeid(Array<int,T>).name(); }
  std::string ParamType(int i) const { return typeid(T).name(); }
  void SetParam(int i, void *param)
  {
    if (i==arr.Size())
      arr.push_back((T)param);
    else
      {
	arr[i] = (T)param;
      }
  }
  void Execute() { }
  void *GetReturn() const 
  {
    //const_cast<VectorArray<T>&>(arr).pop_back();
    return (void*)static_cast<Array<int,T>*>(&arr); 
  }
  bool Appendable() const { return true; }
  void Append(void *param) {
    if (param)
      arr.push_back((T)param);
    else
      arr.push_back(T());
  }
private:
  mutable VectorArray<T> arr;
  std::string name;
};

template<class T>
class CreationArray : public CreationFunction
{
public:
  CreationArray(std::string name) : name(name) { }
  int NumParams() const { return arr.Size()+1; }
  std::string Name() const { return name; }
  std::string ReturnType() const { return typeid(Array<int,T>).name(); }
  std::string ParamType(int i) const { return typeid(T*).name(); }
  void SetParam(int i, void *param)
  {
    if (i==arr.Size())
      arr.push_back(*(T*)param);
    else
      {
	arr[i] = *(T*)param;
      }
  }
  void Execute() { }
  void *GetReturn() const 
  {
    //const_cast<VectorArray<T>&>(arr).pop_back();
    return (void*)static_cast<Array<int,T>*>(&arr); 
  }
  bool Appendable() const { return true; }
  void Append(void *param) {
    if (param)
      arr.push_back(*(T*)param);
    else
      arr.push_back(T());
  }
private:
  mutable VectorArray<T> arr;
  std::string name;
};


template<class R>
class CreationFunction0 : public CreationFunction
{
public:
  CreationFunction0(R (*fptr)(), std::string name) : fptr(fptr), name(name) { }
  virtual bool Appendable() const { return false; }
  virtual void Append(void*) { }
  int NumParams() const { return 0; }
  std::string ParamType(int i) const { return ""; }
  virtual std::string Name() const { return name; }
  void SetParam(int i, void *param)
  {
  }
  void Execute() 
  { 
    retval = fptr(); 
  }
  void *GetReturn() const { return (void*)retval; }
  virtual std::string ReturnType() const { return typeid(R).name(); }
private:
  R (*fptr)();
  R retval;
  std::string name;
  std::string rettype;
};


template<class T, class R>
class CreationFunction1 : public CreationFunction
{
public:
  //CreationFunction1(R (*fptr)(T), std::string name) : fptr(fptr), name(name) { }
  CreationFunction1(R (*fptr2)(const T &), std::string name) : fptr2(fptr2), name(name) { }
  virtual bool Appendable() const { return false; }
  virtual void Append(void*) { }
  int NumParams() const { return 1; }
  std::string ParamType(int i) const { return typeid(T).name(); }
  virtual std::string Name() const { return name; }
  void SetParam(int i, void *param)
  {
    switch(i)
      {
      case 0: param0 = (T*)param; break;
      }
  }
  void Execute() 
  { 
    //if (fptr)
    //  retval = fptr(*param0);
    //else 
      if (fptr2)
      retval = fptr2(*param0); 
  }
  void *GetReturn() const { return (void*)retval; }
  virtual std::string ReturnType() const { return typeid(R).name(); }
private:
  //R (*fptr)(T);
  R (*fptr2)(const T&);
  T *param0;
  R retval;
  std::string name;
  std::string rettype;
};

template<class T1, class T2, class R>
class CreationFunction2 : public CreationFunction
{
public:
  //CreationFunction2(R (*fptr)(T1,T2), std::string name) : fptr(fptr), fptr2(NULL), name(name) { }
  CreationFunction2(R (*fptr2)(T1&,T2&), std::string name, int dummy) : fptr2(fptr2), name(name) { }
  CreationFunction2(R (*fptr3)(const T1&, const T2&), std::string name, float dummy) : fptr2(NULL), fptr3(fptr3), name(name) { }
  CreationFunction2(R (*fptr4)(const T1&, T2&), std::string name, float dummy, float dummy2) : fptr2(NULL), fptr3(NULL), fptr4(fptr4), name(name) { }
  //CreationFunction2(R (*fptr4)(T1, const T2 &), std::string name, const char *dummy) : fptr(NULL), fptr2(NULL), fptr3(NULL), fptr4(fptr4), name(name) { }
  virtual bool Appendable() const { return false; }
  virtual void Append(void*) { }

  int NumParams() const { return 2; }
  virtual std::string Name() const { return name; }
  virtual std::string ReturnType() const { return typeid(R).name(); }
  std::string ParamType(int i) const 
  { 
    switch(i)
      {
      case 0: return typeid(T1).name(); 
      case 1: return typeid(T2).name();
      };
    return "";
  }
  void SetParam(int i, void *param)
  {
    switch(i)
      {
      case 0: param0 = (T1*)param; break;
      case 1: param1 = (T2*)param; break;
      }
  }
  void Execute() 
  { 
    if (fptr2) { retval = fptr2(*param0, *param1); }
    else if (fptr3) { retval = fptr3(*param0, *param1); }
    else if (fptr4) { retval = fptr4(*param0, *param1); }
  }
  void *GetReturn() const { return (void*)retval; }
private:
  //R (*fptr)(T1,T2);
  R (*fptr2)(T1&,T2&);
  R (*fptr3)(const T1&,const T2&);
  R (*fptr4)(const T1&,T2&);
  //R (*fptr4)(T1,const T2 &);
  T1 *param0;
  T2 *param1;
  R retval;
  std::string name;
};

template<class T1, class T2, class T3, class R>
class CreationFunction3 : public CreationFunction
{
public:
  CreationFunction3(R (*fptr)(const T1&,const T2&,const T3&), std::string name) : fptr(fptr), name(name) { }
  CreationFunction3(R (*fptr)(const T1&,const T2&,T3&), std::string name) : fptr2(fptr2), name(name) { }

  virtual bool Appendable() const { return false; }
  virtual void Append(void*) { }

  int NumParams() const { return 3; }
  virtual std::string Name() const { return name; }
  virtual std::string ReturnType() const { return typeid(R).name(); }
  std::string ParamType(int i) const 
  { 
    switch(i)
      {
      case 0: return typeid(T1).name(); 
      case 1: return typeid(T2).name();
      case 2: return typeid(T3).name();
      };
    return "";
  }
  void SetParam(int i, void *param)
  {
    switch(i)
      {
      case 0: param0 = (T1*)param; break;
      case 1: param1 = (T2*)param; break;
      case 2: param2 = (T3*)param; break;
      }
  }
  void Execute() 
  { 
    if (fptr2)
      {
	retval = fptr2(*param0, *param1, *param2); 
      }
    if (fptr)
      {
	retval = fptr(*param0, *param1, *param2); 
      }
    //else if (fptr2)
    //  {
    //	retval = fptr2(*param0, *param1, *param2); 
    //  }
  }
  void *GetReturn() const { return (void*)retval; }
private:
  R (*fptr)(const T1&,const T2&,const T3&);
  R (*fptr2)(const T1&,const T2&, T3&);
  //R (*fptr2)(T1&,T2&,T3&);
  T1 *param0;
  T2 *param1;
  T3 *param2;
  R retval;
  std::string name;
};


class CreationFunctionArray : public Array<int, CreationFunction*>
{
};
class DynCreationFunctionArray : public CreationFunctionArray
{
public:
  int Size() const { return vec.Size(); }
  CreationFunction* Index(int i) const { return vec.Index(i); }

  template<class T>
  void push_value(std::string name)
  {
    vec.push_back(new ValueCreationFunction<T>(name));
  }

  template<class T>
  void push_back_array(std::string name)
  {
    vec.push_back(new CreationArray<T>(name));
  }
  template<class T>
  void push_back_ptr_array(std::string name)
  {
    vec.push_back(new CreationPointerArray<T>(name));
  }

  template<class R>
  void push_back(R (*fptr)(), std::string name)
  {
    vec.push_back(new CreationFunction0<R>(fptr, name));
  }
  template<class T, class R>
  void push_back_ref(R (*fptr)(T&), std::string name)
  {
    vec.push_back(new CreationFunction1<T,R>(fptr, name));
  }
  //template<class T, class R>
  //void push_back_val(R (*fptr)(T), std::string name)
  //{
  //  vec.push_back(new CreationFunction1<T,R>(fptr, name));
  //}


  template<class T, class R>
  void push_back_cref(R (*fptr)(const T&), std::string name)
  {
    vec.push_back(new CreationFunction1<T,R>(fptr, name));
  }


  template<class T1, class T2, class R>
  void push_back_ref_ref(R (*fptr)(T1&,T2&), std::string name)
  {
    vec.push_back(new CreationFunction2<T1,T2,R>(fptr, name, 666));
  }
  template<class T1, class T2, class R>
  void push_back_cref_ref(R (*fptr)(T1&,T2&), std::string name)
  {
    vec.push_back(new CreationFunction2<T1,T2,R>(fptr, name, 666,666));
  }

  template<class T1, class T2, class R>
  void push_back_ref_cref(R (*fptr)(T1&,const T2&), std::string name)
  {
    vec.push_back(new CreationFunction2<T1,T2,R>(fptr, name));
  }
  template<class T1, class T2, class R>
  void push_back_cref_cref(R (*fptr)(const T1&,const T2&), std::string name)
  {
    vec.push_back(new CreationFunction2<T1,T2,R>(fptr, name, 10.0));
  }
  //template<class T1, class T2, class R>
  //void push_back_val_cref(R (*fptr)(T1,const T2&), std::string name)
  //{
  //  vec.push_back(new CreationFunction2<T1,T2,R>(fptr, name, ""));
  //}


  template<class T1, class T2, class T3, class R>
  void push_back_cref_cref_cref(R (*fptr)(const T1&,const T2&,const T3&), std::string name)
  {
    vec.push_back(new CreationFunction3<T1,T2,T3,R>(fptr, name));
  }

  template<class T1, class T2, class T3, class R>
  void push_back_cref_cref_ref(R (*fptr)(const T1&,const T2&,T3&), std::string name)
  {
    vec.push_back(new CreationFunction3<T1,T2,T3,R>(fptr, name));
  }


  /*
  template<class T, class R>
  void push_back(R (*fptr)(T))
  {
    vec.push_back(new CreationFunction1<T,R>(fptr));
  }
  template<class T1, class T2, class R>
  void push_back(R (*fptr)(T1,T2))
  {
    vec.push_back(new CreationFunction2<T1,T2,R>(fptr));
  }
  */
private:
  VectorArray<CreationFunction*> vec;
};

typedef ArrayConvert<CreationFunctionArray, int, CreationFunction*> CreationFunctionArrayConvert;

CurveMultiply* curve_multiply_func(MatrixCurve &curve, CurveIn3d &curve2);

CreationFunctionArray *creation_array();

template<class T>
struct ExpressionTree
{
  T t;
  std::vector<ExpressionTree<T>*> children;
};

struct ExpressionNode
{
  CreationFunction *func;
};

ExpressionTree<ExpressionNode> *create_expr_tree_node(CreationFunction *func);

void ExecuteGtkmm(int argc, char **argv, ExpressionTree<ExpressionNode> *&tree);
void ExecuteEffect(ExpressionTree<ExpressionNode> *tree, SDL_Surface *surf);
FrameAnim *CreateFrameAnim(ExpressionTree<ExpressionNode> *tree, Render *r);

std::string stringify(float x);

std::vector<CreationFunction*> filter_creation_array(Array<int, CreationFunction*> &arr, std::string ret_type);



#endif
