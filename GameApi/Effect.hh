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


#ifndef EFFECT_HH
#define EFFECT_HH

#include "VectorTools.hh"
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include "Pieces.hh"
#include <SDL/SDL.h>
#include "Buffer.hh"
#include "Intersect.hh"
#include "Shader.hh"
#include <iterator>
#include <cassert>
#include <tr1/memory>

enum AttribId
  {
    AttrCenterX,
    AttrCenterY,
    AttrCenterZ,
    AttrInternalX, // objects internal coordinates
    AttrInternalY,
    AttrInternalZ,
    AttrInternalObjX,
    AttrInternalObjY, 
    AttrObjectId,
    AttrInternalNX_X,
    AttrInternalNX_Y,
    AttrInternalNX_Z,
    AttrInternalNY_X,
    AttrInternalNY_Y,
    AttrInternalNY_Z,
    AttrFace,
    AttrFrame
  };




class Effect
{
public:
  virtual ~Effect() { }
  virtual float Length() const=0;
  virtual void ChangeTime(float time)=0;
};

class Render;
class LineCollection;
class FaceCollection;
class FrameAnim
{
public:
  FrameAnim(Render *render) : render(render) { }
  virtual ~FrameAnim() { }
  virtual float Time() const { return 5.0; }
  virtual void Init() { }
  virtual void Cleanup() { }
  virtual void PreFrame(float time) { /*GetRender()->SetTime(time);*/ }
  virtual bool Frame(float time)=0;
  virtual void PostFrame() { }
  virtual float XRot() const { return 1.0; }
  virtual float YRot() const { return 1.0; }
  virtual float ZRot() const { return 0.0; }  
  virtual float RotSpeed() const { return 1.0; }
public:
  Render *GetRender() const { return render; }
public:
  void RenderOpenGl(const FaceCollection &col);
  void RenderOpenGlLines(const LineCollection &col);
  void RenderOpenGlPoly(const PointCollection &col);
private:
  Render *render;
};


class FramePlugin
{
public:
  virtual ~FramePlugin() { }
  virtual void Init() { }
  virtual void Cleanup() { }
  virtual void PreFrame(float time) { }
  virtual void PostFrame() { }
};


template<class A, class B>
class Union
{
public:
  Union(A a) : is_a(true) { either.a = a; }
  Union(B b) : is_a(false) { either.b = b; }
  Union(A a, B b, bool is_a) : is_a(is_a) { if (is_a) either.a = a; else either.b = b; }
  Union(const Union<A,B> &c) 
  { 
    if (c.is_a) either.a = c.either.a;
    else
      either.b = c.either.b;
    is_a = c.is_a;
  }
  bool is_a;
  union {
    A a;
    B b;
  } either;
};

template<class A, class B>
class Function 
{
public:
  typedef A Domain;
  typedef B CoDomain;

  virtual ~Function() { }
  virtual B Index(A a) const=0;
};

template<class A, class B>
class Arrow
{
public:
  virtual B Map(A a) const=0;
  virtual A InvMap(A a, int num) const=0;
};

class InterpolateFunction : public Function<float, float>
{
public:
  InterpolateFunction(Function<float,float> &f1,
		      Function<float,float> &f2, float pos) : f1(f1),f2(f2), pos(pos) { }

  float Index(float x) const
  {
    return f1.Index(x)*pos + f2.Index(x)*(1.0-pos);
  }
private:
  Function<float,float> &f1;
  Function<float,float> &f2;
  float pos;
};

template<class T, class K>
class InvertableFunction : public Function<T,K>
{
public:
  virtual K Index(T t) const=0;
  virtual std::vector<T> Inverse(K k) const=0;
};

class VectorInvertableFunction2d : public InvertableFunction<Point2d, Point2d>
{
public:
  VectorInvertableFunction2d(Vector2d vec) : vec(vec) { }
  Point2d Index(Point2d t) const 
  { 
    Point2d p = { t.x+vec.dx, t.y+vec.dy };
    return p; 
  }
  std::vector<Point2d> Inverse(Point2d k) const
  {
    std::vector<Point2d> p;
    Point2d pp = { k.x-vec.dx, k.y-vec.dy };
    p.push_back(pp);
    return p;
  }
private:
  Vector2d vec;
};

class VectorPointConversion : public Function<Vector, Point>
{
public:
  Point Index(Vector v) const 
  {
    Point p;
    p.x = v.dx;
    p.y = v.dy;
    p.z = v.dz;
    return p;
  }
};

class PointVectorConversion : public Function<Point, Vector>
{
public:
  Vector Index(Point v) const 
  {
    Vector p;
    p.dx = v.x;
    p.dy = v.y;
    p.dz = v.z;
    return p;
  }
};


template<class T, class A, class B>
class ForwardFunction : public Function<A,B>
{
public:
  ForwardFunction(T *obj, B (T::*fptr)(A) const) : obj(obj), fptr(fptr) { }
  B Index(A a) const { return (obj->*fptr)(a); }
private:
  T *obj;
  B (T::*fptr)(A) const;
};
template<class A, class B>
class NativeFunction : public Function<A,B>
{
public:
  NativeFunction(B (*fptr)(A)) : fptr(fptr) { }
  B Index(A a) const { return fptr(a); }
private:
  B (*fptr)(A);
};

template<class K, class Impl>
class FunctionImplT0 : public Function<void, K>
{
  FunctionImplT0() : c(0) { }
  K Index() const {
    delete c;
    c = new Impl();
    return c;
  }
  ~FunctionImplT0() { delete c; }
private:
  mutable Impl *c;
};

template<class K, class P1, class Impl>
class FunctionImplT1 : public Function<void, K>
{
  FunctionImplT1(P1 p1) : c(0), p1(p1) { }
  K Index() const {
    delete c;
    c = new Impl(p1);
    return c;
  }
  ~FunctionImplT1() { delete c; }

private:
  P1 p1;
  mutable Impl *c;
};


template<class K, class P1, class P2, class Impl>
class FunctionImplT2 : public Function<void, K>
{
  FunctionImplT2(P1 p1, P2 p2) : c(0), p1(p1), p2(p2) { }
  K Index() const {
    delete c;
    c = new Impl(p1,p2);
    return c;
  }
  ~FunctionImplT2() { delete c; }

private:
  P1 p1;
  P2 p2;
  mutable Impl *c;
};


template<class K, class P1, class P2, class P3, class Impl>
class FunctionImplT3 : public Function<void, K>
{
  FunctionImplT3(P1 p1, P2 p2, P3 p3) : c(0), p1(p1), p2(p2), p3(p3) { }
  K Index() const {
    delete c;
    c = new Impl(p1,p2, p3);
    return c;
  }
  ~FunctionImplT3() { delete c; }

private:
  P1 p1;
  P2 p2;
  P3 p3;
  mutable Impl *c;
};




template<class T, class K, class Impl>
class FunctionImpl0 : public Function<T,K>
{
public:
  FunctionImpl0() : c(0) { }
  K Index(T a) const
  {
    delete c;
    c = new Impl(a);
    return c;
  }
  ~FunctionImpl0() { delete c; }

private:
  mutable Impl *c;
};

template<class T, class K, class P, class Impl>
class FunctionImpl1 : public Function<T,K>
{
public:
  FunctionImpl1(P p) : p(p), c(0) { }
  K Index(T a) const
  {
    delete c;
    c = new Impl(a, p);
    return c;
  }
  ~FunctionImpl1() { delete c; }
private:
  P p;
  mutable Impl *c;
};

template<class T, class K, class P1, class P2, class Impl>
class FunctionImpl2 : public Function<T,K>
{
public:
  FunctionImpl2(P1 p1, P2 p2) : p1(p1), p2(p2), c(0) { }
  K Index(T a) const
  {
    delete c;
    c = new Impl(a, p1, p2);
    return c;
  }
  ~FunctionImpl2() { delete c; }

private:
  P1 p1;
  P2 p2;
  mutable Impl *c;
};

template<class T, class K, class P1, class P2, class P3, class Impl>
class FunctionImpl3 : public Function<T,K>
{
public:
  FunctionImpl3(P1 p1, P2 p2, P3 p3) : p1(p1), p2(p2), p3(p3), c(0) { }
  K Index(T a) const
  {
    delete c;
    c = new Impl(a, p1, p2, p3);
    return c;
  }
  ~FunctionImpl3() { delete c; }

private:
  P1 p1;
  P2 p2;
  P3 p3;
  mutable Impl *c;
};




template<class A, class B, class C>
class FunctionCompose : public Function<A,C>
{
public:
  FunctionCompose(Function<A,B> &f1, Function<B,C> &f2) : f1(f1), f2(f2) { }
  C Index(A a) const { return f2.Index(f1.Index(a)); }
private:
  Function<A,B> &f1;
  Function<B,C> &f2;
};

template<class C1, class C2, class D>
class EitherFunction : public Function<Union<C1,C2>, D>
{
public:
  EitherFunction(Function<C1,D> &f1, Function<C2,D> &f2) : f1(f1), f2(f2) { }
  D Index(Union<C1,C2> u) const
  {
    if (u.is_a)
      return f1.Index(u.either.a);
    else
      return f2.Index(u.either.b);
  }
private:
  Function<C1,D> &f1;
  Function<C2,D> &f2;
};

template<class A, class B, class C>
class Function2
{
public:
  typedef std::pair<A,B> Domain;
  typedef C CoDomain;
  C Index(const std::pair<A,B> &p) { return Index(p.first, p.second); }

  virtual ~Function2() { }
  virtual C Index(A a, B b) const=0;
};

template<class A>
class ForLoop : public Function<A,A>
{
public:
  ForLoop(int repeat, Function<A,A> &a) : repeat(repeat), f(&a), f2(0) { }
  ForLoop(int repeat, Function2<int, A,A> &a) : repeat(repeat), f(0), f2(&a) { }
  A Index(A a) const
  {
    if (f)
      {
	for(int i=0;i<repeat;i++)
	  {
	    a = f->Index(a);
	  }
      }
    else
      {
	for(int i=0;i<repeat;i++)
	  {
	    a = f2->Index(i,a);
	  }
      }
    return a;
  }
private:
  int repeat;
  Function<A,A> *f;
  Function2<int,A,A> *f2;
};


template<class A, class B, class C>
class NativeFunction2 : public Function2<A,B,C>
{
public:
  NativeFunction2(C (*fptr)(A,B)) : fptr(fptr) { }
  C Index(A a, B b) const { return fptr(a,b); }
private:
  C (*fptr)(A, B);
};

template<class T>
class Element
{
public:
  virtual ~Element() { }
  virtual T Elem() const=0;
};
template<class A, class B>
class Curry : public Element<B>
{
public:
  Curry(const Function<A,B> &func, A a) : func(func),a(a) { }
  B Elem() const { return func.Index(a); }
private:
  const Function<A,B> &func;
  A a;
};
template<class A, class B, class C>
class Curry2a : public Function<B,C>
{
public:
  Curry2a(const Function2<A,B,C> &func2, A a) : func2(func2), a(a) { }
  C Index(B b) const { return func2.Index(a,b); }
private:
  const Function2<A,B,C> &func2;
  A a;
};

template<class A, class B, class C>
class Curry2b : public Function<A,C>
{
public:
  Curry2b(const Function2<A,B,C> &func2, B b) : func2(func2), b(b) { }
  C Index(A a) const { return func2.Index(a,b); }
private:
  const Function2<A,B,C> &func2;
  B b;
};
template<class A, class B>
class RenamedFunction : public Function<A,B>
{
public:
  B Index(A a) const { return Map(a); }
  virtual B Map(A a) const=0;
};

template<class X, class Y, class B>
class Exponentiation : public Function<X,Function<B,Y>* >, private RenamedFunction<B,Y>
{
public:
  Exponentiation(Function<std::pair<X,B>, Y> &f) : f(f) { }
  Function<B,Y> *Index(X x) const
  {
    xx = x;
    return this;
  }
  Y Map(B b) const
  {
    return f.Index(std::make_pair(xx,b));
  }
private:
  Function<std::pair<X,B>, Y> &f;
  X xx;
};

template<class X, class Y, class B>
class ExponentiationI : public Function<std::pair<X,B>, Y>
{
public:
  ExponentiationI(Function<X, Function<B,Y> *> &func) : func(func) { }
  Y Index(std::pair<X,B> &p) const
  {
    Function<B,Y> *func2 = func.Index(p.first);
    return func2->Index(p.second);
  }
private:
  Function<X, Function<B,Y> *> &func;
};

// This is really a Function<std::pair<Function<B,Y>*,B>, Y> 
template<class B, class Y> 
class Evaluation : public ExponentiationI<Function<B,Y>*, Y, B>
{
public:
  Evaluation(Function<Function<B,Y>*, Function<B,Y>*> &id) : ExponentiationI<Function<B,Y>*,Y,B>(id) { }
};

template<class X>
class EvaluationCharacteristicFunction : public Function<std::pair<Function<X,bool>*, X>, bool>
{
public:
  EvaluationCharacteristicFunction(Function<Function<X,bool>*, Function<X,bool>* > &id) : eval(id) { }
  bool Index(std::pair<Function<X,bool>*, X> p) const
  {
    return eval.Index(p);
  }
private:
  Evaluation<X,bool> eval;
};
template<class A, class B, class C, class D>
class PairFunction : Function<std::pair<A,B>, std::pair<C,D> >
{
public:
  PairFunction(Function<A,C> &f1, Function<B,D> &f2) : f1(f1), f2(f2) { }
  std::pair<C,D> Index(std::pair<A,B> p) const
  {
    return make_pair(f1.Index(p.first), f2.Index(p.second));
  }  
private:
  Function<A,C> &f1;
  Function<B,D> &f2; 
};

template<class N>
class Successor : public Function<N,N>
{
public:
  N Index(N n) const
  {
    return n+1;
  }
};


template<class N>
class Iteration : public Function<int, Function<N,N>* >, private Function<N,N>
{
public:
  Iteration(Function<N,N> &succ) : succ(succ) { }
  Function<N,N>* Index(int n) const
  {
    count = n;
    return this;
  }
  N Index(N n) const
  {
    N nn = n;
    for(int i=0;i<count;i++)
      {
	nn = succ(nn);
      }
    return nn;
  }
private:
  Function<N,N> &succ;
  int count;
};

template<class N>
class Plus : public Function<std::pair<N,N>, N>
{
public:
  Plus(Function<N,N> &succ) : ite(succ), exp(ite) { }
  N Index(std::pair<N,N> &p) const
  {
    return exp.Index(p);
  }
private:
  Iteration<N> ite;
  ExponentiationI<N,N,N> exp;
};

template<class N, class A>
class Mul : public Function<std::pair<N,A>, A>, private Function<A, Function<int, A>*>
{
public:
  Mul(Function<std::pair<A,A>, A> &plus, A zero) : exp(plus), ptr(ptr), zero(zero), expi(this), ptr(0) { }
  A Index(std::pair<N,A> &p) const
  {
    return expi.Index(p);
  }
  class IteFunc : public Function<int, A>
  {
  public:
    IteFunc(Function<A,A> *func, A zero) : func(func), zero(zero) { }
    A Index(int a) const
    {
      Iteration<A> ite(func);
      Function<A,A> *f2 = ite.Index(a);
      A aa = f2->Index(zero);
      return aa;
    }
    Function<A,A> *func;
    A zero;
  };

  Function<int,A> *Index(A a) const
  {
    Function<A,A> *func = exp.Index(a);
    delete ptr;
    ptr = new IteFunc(func, zero);
    return ptr;
  }
  ~Mul() { delete ptr; }
private:
  Exponentiation<A,A,A> exp;
  Evaluation<A,A> eval;
  ExponentiationI<A, int, A> expi;
  IteFunc *ptr;
  A zero;
};

template<class T>
class ElementFunction : public Function<Element<T>*, T>
{
public:
  T Index(Element<T> *e) const
  {
    return e->Elem();
  }
};

template<class I, class T>
class Array : public Function<I,T>
{
public:
  virtual I Size() const=0;
};
template<class T>
void AllocToNativeArray(Array<int,T> &arr, T*** result);

template<class T>
class NativeArray : public Array<int, T>
{
public:
  NativeArray(T *array, int size) : array(array), size(size) { }
  int Size() const { return size; }
  T Index(int i) const { return array[i]; }
private:
  T *array;
  int size;
};

template<class I, class A, class B>
class MapArray : public Array<I, B>
{
public:
  MapArray(Array<I,A> &arr, Function<A,B> &func) : arr(arr), func(func) { }
  I Size() const { return arr.Size(); }
  B Index(I i) const { return func.Index(arr.Index(i)); }
private:
  Array<I,A> &arr;
  Function<A,B> &func;
};

class SingleInt : public Array<int,int>
{
public:
  SingleInt(int a) : a(a) { }
  int Size() const { return 1; }
  int Index(int i) const { return a; }
  int Sum() const { return a; }
private:
  int a;
};
class TwoInts : public Array<int,int>
{
public:
  TwoInts(int a, int b) : a(a), b(b) { }
  int Size() const { return 2; }
  int Index(int i) const { if (i==0) return a; else return b; }
  int Sum() const { return a+b; }
private:
  int a,b;
};

class ThreeInts : public Array<int,int>
{
public:
  ThreeInts(int a, int b, int c) : a(a), b(b), c(c) { }
  int Size() const { return 3; }
  int Index(int i) const { if (i==0) return a; else if (i==1) return b; else return c; }
  int Sum() const { return a+b+c; }
private:
  int a,b,c;
};

template<class T>
class ResizeArray : public Array<int, T>
{
public:
  ResizeArray(Array<int, T> &arr, T def, int size) : arr(arr), def(def), size(size) { }
  int Size() const { return size; }
  T Index(int i) const
  {
    if (i < arr.Size()) return arr.Index(i);
    return def;
  }
private:
  Array<int, T> &arr;
  T def;
  int size;
};


template<class I, class I2, class T>
class SwapIndexesArray : public Array<I2, Array<I, T>*>
{ // all arrays need to be same size.
public:
  SwapIndexesArray(Array<I, Array<I2, T>*> &arr) : arr(arr),i(0) { }
  I2 Size() const { return arr.Index(0)->Size(); }
  ~SwapIndexesArray() { delete i; }
  Array<I,T>* Index(I2 index) const
  {
    delete i;
    i = new InnerArray(arr, index);
    return i;
  }
  struct InnerArray : public Array<I,T>
  {
    InnerArray(Array<I, Array<I2,T>*> &arr, I2 index) : arr(arr), index(index) { }
    I Size() const { return arr.Size(); }
    T Index(I i) const
    {
      return arr.Index(i)->Index(index);
    }
    Array<I, Array<I2, T>*> &arr;
    I2 index;
  };
private:
  Array<I, Array<I2, T>*> &arr;
  mutable InnerArray *i;
};

template<class P, class T>
class ApplyParameterArray : public Array<int, T>
{
public:
  ApplyParameterArray(Array<int, Array<P, T>*> &arr, P p) : arr(arr), p(p) { }
  int Size() const { return arr.Size(); }
  T Index(int i) const
  {
    return arr.Index(i)->Index(p);
  }
private:
  Array<int, Array<P,T>*> &arr;
  P p;
};

template<class I, class T>
class FunctionArray : public Array<I,T>
{
public:
  FunctionArray(I size, Function<I,T> &func) : size(size), func(func) { }
  I Size() const { return size; }
  T Index(I i) const { return func.Index(i); }
private:
  I size;
  Function<I,T> &func;
};

template<class B, class I, class T>
class ArrayConvert : public B
{
public:
  ArrayConvert(const Array<I, T> &arr) : arr(arr) { }
  int Size() const { return arr.Size(); }
  T Index(int i) const { return arr.Index(i); }
private:
  const Array<I,T> &arr;
};

template<class T>
class CombineArrays : public Array<int,T>
{
public:
  CombineArrays(const Array<int,T> &a1, const Array<int,T> &a2) : a1(a1), a2(a2) { }
  int Size() const { return a1.Size()+ a2.Size(); }
  T Index(int i) const
  {
    int size = a1.Size();
    if (i < size)
      {
	return a1.Index(i);
      }
    else
      {
	return a2.Index(i-size);
      }    
  }
  bool ItemInFirstArray(int i) const
  {
    return i < a1.Size();
  }
  bool ItemInSecondArray(int i) const
  {
    return !ItemInFirstArray(i);
  }
  
private:
  const Array<int,T> &a1;
  const Array<int,T> &a2;
};

template<class T>
class FilterArray : public Array<int,T>
{
public:
  FilterArray(Array<int,T> &arr, Function<T, bool> &b) : arr(arr), b(b) { }
  int Size() const
  {
    int size = arr.Size();
    int count = 0;
    for(int i=0;i<size;i++)
      {
	if (b.Index(arr.Index(i)))
	  count++;
      }
    return count;
  }
  T Index(int ii) const
  {
    int size = arr.Size();
    int count = 0;
    for(int i=0;i<size;i++)
      {
	if (count == ii) return arr.Index(i);
	if (b.Index(arr.Index(i)))
	  count++;
      }
    return arr.Index(0);
  }
private:
  Array<int, T> &arr;
  Function<T, bool> &b;
};

template<class I, class T>
std::ostream &operator<<(std::ostream &o, const Array<I,T> &arr)
{
  int size = arr.Size();
  o << "[";
  for(int i=0;i<50 && i<size;i++)
    {
      o << arr.Index(i) << ",";
    }
  o << "...]";
  return o;
}	


template<class I1, class I2, class T>
class Array2 : public Function2<I1,I2,T>
{
public:
  virtual I1 SizeX() const=0;
  virtual I2 SizeY() const=0;
};

template<class A, class B, class C>
class ComposeArrays : public Array<A,C>
{
public:
  ComposeArrays(Array<A,B> &a1, Function<B,C> &a2) : a1(a1), a2(a2) { }
  A Size() const { return a1.Size(); }
  C Index(A a) const { return a2.Index(a1.Index(a)); }
private:
  Array<A,B> &a1;
  Function<B,C> &a2;
};

template<class A, class C>
class ComposeChain : public Array<A,C>
{
public:
  template<class T1>
  ComposeChain(Array<A, T1> &a1, Function<T1, C> &f1)
  {
    arr = new ComposeArrays<A,T1,C>(a1, f1);
  }
  template<class T1, class T2>
  ComposeChain(Array<A, T1> &a1, Function<T1,T2> &f1, Function<T2, C> &f2)
  {
    Array<A,T2> *arr1 = new ComposeArrays<A,T1,T2>(a1, f1);
    arr = new ComposeArrays<A,T2, C>(*arr1, f2);
  }
  template<class T1, class T2, class T3>
  ComposeChain(Array<A, T1> &a1, Function<T1,T2> &f1, Function<T2,T3> &f2, Function<T3, C> &f3)
  {
    Array<A,T2> *arr1 = new ComposeArrays<A,T1,T2>(a1, f1);
    Function<T2,C> *func2 = new FunctionCompose<T2,T3,C>(a1, f1);
    arr = new ComposeArrays<A,T2, C>(*arr1, *func2);
  }
  template<class T1, class T2, class T3, class T4>
  ComposeChain(Array<A,T1> &a1, Function<T1, T2> &f1, Function<T2,T3> &f2, Function<T3,T4> &f3, Function<T4,C> &f4)
  {
    Array<A,T2> *arr1 = new ComposeArrays<A,T1,T2>(a1, f1);
    Function<T2,T4> *func2 = new FunctionCompose<T2,T3,T4>(f2, f3);
    Function<T2,C> *func3 = new FunctionCompose<T2,T4,C>(*func2, f4);
    arr = new ComposeArrays<A,T2, C>(*arr1, *func3);    
  }
  template<class T1, class T2, class T3, class T4,class T5>
  ComposeChain(Array<A,T1> &a1, Function<T1, T2> &f1, Function<T2,T3> &f2, Function<T3,T4> &f3, Function<T4,T5> &f4, Function<T5,C> &f5)
  {
    ComposeChain<A,T5> *arr1 = new ComposeChain<A,T5>(a1,f1,f2,f3,f4);
    arr = new ComposeArrays<A,T5, C>(*arr1, f5);    
  }

  template<class T1, class T2, class T3, class T4,class T5, class T6>
  ComposeChain(Array<A,T1> &a1, Function<T1, T2> &f1, Function<T2,T3> &f2, Function<T3,T4> &f3, Function<T4,T5> &f4, Function<T5,T6> &f5, Function<T6,C> &f6)
  {
    ComposeChain<A,T6> *arr1 = new ComposeChain<A,T6>(a1,f1,f2,f3,f4,f5);
    arr = new ComposeArrays<A,T6, C>(*arr1, f6);    
  }



  A Size() const { return arr->Size(); }
  C Index(A a) const { return arr->Index(a); }
  

private:
  Array<A,C> *arr;
};

template<class T>
class SingletonArray : public Array<int,T>
{
public:
  SingletonArray(T t) : t(t) { }
  virtual int Size() const { return 1; }
  virtual T Index(int index) const { return t; }
private:
  T t;
};

template<class S, class T>
class DuplicateArray : public Array<S, T>
{
public:
  DuplicateArray(const T &t, const S &s) : t(t),s(s) { }
  S Size() const { return s; }
  T Index(S index) const { return t; }
private:
  T t;
  S s;
};

template<class T>
class DupArray : public Array<int, T>
{
public:
  DupArray(Array<int,T> &orig, int count) : orig(orig), count(count) { }
  int Size() const { return orig.Size()*count; }
  T Index(int i) const
  {
    int pos = i % orig.Size();
    return orig.Index(pos);
  }
private:
  Array<int,T> &orig;
  int count;
};

template<class T>
class VectorArray : public Array<int, T>
{
public:
  VectorArray() : vec() { }
  template<class It>
  VectorArray(It beg, It end) : vec(beg, end) { }
  virtual int Size() const { return vec.size(); }
  virtual T Index(int index) const { return vec.at(index); }

  T &operator[](int index) { return vec[index]; }
  void push_back(T t) { vec.push_back(t); }
  T pop_back() 
  {
    T t = vec.back();
    vec.pop_back(); 
    return t;
  }
  void clear() { vec.clear(); }
  const T *get_whole_array() const { return &vec[0]; }
private:
  std::vector<T> vec;
};

template<class T>
class CopyArray : public VectorArray<T>
{
public:
  CopyArray(Array<int, T> &arr) : arr(arr) 
  {
    Generate();
  }
  void Generate()
  {
    int size = VectorArray<T>::Size();
    for(int i=0;i<size;i++)
      VectorArray<T>::pop_back();
    // now VectorArray is empty
    int size2 = arr.Size();
    for(int ii=0;ii<size2;ii++)
      this->push_back(arr.Index(ii));
  }
private:
  Array<int, T> &arr;
};

template<class T, class K>
class IteratorArray
{
public:
  IteratorArray(const Array<T,K> &arr) : arr(arr) { }
  struct const_iterator
  {
    const_iterator(const Array<T,K> &arr, T index) : arr(arr), index(index) { }
    void operator++() { index++; }
    void operator--() { index--; }
    void operator+=(int i) { index+=i; }
    void operator-=(int i) { index-=i; }
    friend int operator-(const const_iterator &i, const const_iterator &i2)
    {
      return i.index - i2.index; 
    }
    typedef std::random_access_iterator_tag iterator_category;
    typedef int pointer;
    typedef int &reference;
    typedef K value_type;
    typedef int difference_type;
    K operator*() const { return arr.Index(index); }
    const Array<T,K> &arr;
    T index;
  };
  const_iterator begin() const { return const_iterator(arr, 0); }
  const_iterator end() const { return const_iterator(arr, arr.Size()); }
private:
  const Array<T,K> &arr;
};

template<class I, class T>
class ArrayDiagonal : public Array<I,T>
{
public:
  ArrayDiagonal(Array<std::pair<I,I>, T> &arr) : arr(arr) { }
  I Size() const 
  {
    std::pair<I,I> p = arr.Size();
    return std::min(p.first, p.second);
  }
  T Index(I i) const
  {
    return arr.Index(std::make_pair(i,i));
  }
private:
  Array<std::pair<I,I>, T> &arr;
};

template<class I1, class I2, class T1, class T2>
class ArrayPair2 : public Array<std::pair<I1,I2>, std::pair<T1,T2> >
{
public:
  ArrayPair2(const Array<I1,T1> &arr1, const Array<I2,T2> &arr2) : arr1(arr1), arr2(arr2) { }
  std::pair<I1,I2> Size() const 
  { 
    return std::make_pair(arr1.Size(), arr2.Size());
  }
  std::pair<T1,T2> Index(std::pair<I1,I2> i) const 
  { 
    T1 i2 = arr1.Index(i.first);
    T2 i3 = arr2.Index(i.second);
    //std::cout << "ArrayPair: " << i2 << " " << i3 << std::endl;
    return std::make_pair(i2, i3); }
private:
  const Array<I1,T1> &arr1;
  const Array<I2,T2> &arr2;
};

template<class I, class T1, class T2>
class ArrayPair : public Array<I,std::pair<T1,T2> >
{
public:
  ArrayPair(Array<I,T1> &a1, Array<I,T2> &a2) : pair(a1,a2), diag(pair) { }
  I Size() const { return diag.Size(); }
  std::pair<T1,T2> Index(I i) const
  {
    return diag.Index(i);
  }
private:
  ArrayPair2<I,I,T1,T2> pair;
  ArrayDiagonal<I,std::pair<T1,T2> > diag;
};

template<class I, class T1, class T2>
class FstArray : public Array<I, T1>
{
public:
  FstArray(Array<I, std::pair<T1,T2> > &arr) : arr(arr) { }
  I Size() const { return arr.Size(); }
  T1 Index(I i) const
  {
    return arr.Index(i).first;
  }
private:
  Array<I,std::pair<T1,T2> > &arr;
};

template<class I, class T1, class T2>
class SndArray : public Array<I, T2>
{
public:
  SndArray(Array<I, std::pair<T1,T2> > &arr) : arr(arr) { }
  I Size() const { return arr.Size(); }
  T2 Index(I i) const
  {
    return arr.Index(i).second;
  }
private:
  Array<I,std::pair<T1,T2> > &arr;
};


template<class I, class T>
class SubArray : public Array<I,T>
{
public:
  SubArray(const Array<I,T> &arr, int start_index, int length) : arr(arr), start_index(start_index), length(length) { }
  I Size() const { return length; }
  T Index(int i) const
  {
    return arr.Index(start_index+i);
  }
private:
  const Array<I,T> &arr;
  int start_index;
  int length;
};


template<class I, class J, class T>
class ArrayArray : public Array<I, Array<J,T>*>
{
public:
};

template<class T>
class Range
{
public:
  Range() : start(), end() { }
  Range(T s, T e) : start(s), end(e) { }
  T start;
  T end;
};

template<class T>
class SubArrayArray : public ArrayArray<int, int, T>
{
public:
  SubArrayArray(Array<int, Range<int> > &ranges, Array<int, T> &contents) : ranges(ranges), contents(contents) { }
  Array<int, T> *Index(int i) const
  {
    delete sub;
    Range<int> r = ranges.Index(i);
    sub = new SubArray<int,T>(contents, r.start, r.end-r.start);
    return sub;
  }
  ~SubArrayArray() { delete sub; }
private:
  SubArray<int, T> *sub;
  Array<int,Range<int> > &ranges;
  Array<int, T> &contents;
};

template<class I, class J, class T>
class ArrayArraySizes : public Array<I, J>
{
public:
  ArrayArraySizes(ArrayArray<I, J, T> &arr) : arr(arr) { }
  J Index(I i) const
  {
    return arr.Index(i)->Size();
  }
  I Size() const { return arr.Size(); }
private:
  ArrayArray<I, J, T> &arr;
};

class IndexMap : public Array<int,std::pair<int,int> >
{ // takes small array of sizes, and creates larger array which
  // is described by the sizes.
public:
  IndexMap(Array<int,int> &arr) : arr(arr) { }
  std::pair<int,int> Index(int i) const
  {
    int index = 0;
    int size = arr.Size();
    for(int ii = 0;ii<size;ii++)
      {
	int oldindex = index;
	index += arr.Index(ii);
	if (index >= i) return std::make_pair(ii, i - oldindex);
      }
    return std::make_pair(0,0);
  }
  int Size() const
  {
    int size = arr.Size();
    int count = 0;
    for(int ii=0;ii<size;ii++)
      {
	count += arr.Index(ii);
      }
    return count;
  }
private:
  Array<int,int> &arr;
};


template<class T>
class CompressArrayArray : public Array<int,T>
{
public:
  CompressArrayArray(ArrayArray<int, int, T> &arr) : arr(arr), sizes(arr), sizesmap(sizes) { }
  T Index(int i) const
  {
    std::pair<int,int> p = sizesmap.Index(i);
    return arr.Index(p.first)->Index(p.second);
  }
private:
  ArrayArray<int, int, T> &arr;

  ArrayArraySizes<int, int, T> sizes;
  IndexMap sizesmap;
};

template<class T>
class Curve : public Array<float, T>
{
public:
  //float Length() const { return static_cast<float>(Size()); }
};

typedef Curve<std::pair<Point,Vector> > CurveWithNormal;


template<class T>
class CurveConvert : public Curve<T>
{
public:
  CurveConvert(const Array<float,T> &arr) : arr(arr) { }
  int Size() const { return arr.Size(); }
  T Index(float i) const { return arr.Index(i); }

private:
  const Array<float, T> &arr;
};

template<class T>
class ScaleCurve : public Curve<T>
{
public:
  ScaleCurve(Curve<T> &curve, float target_length) : curve(curve), target_length(target_length) { }
  float Size() const { return target_length; }
  T Index(float x) const
  {
    return curve.Index(x/target_length*curve.Size());
  }
private:
  Curve<T> &curve;
  float target_length;
};

class CurveIn2d : public Curve<Point2d>
{
public:
  virtual ~CurveIn2d() { }
  //virtual Point2d Index(float pos) const=0;
  //virtual float Length() const=0;
};

class FunctionCurveIn2d : public CurveIn2d
{
public:
  FunctionCurveIn2d(Function<float, float> &f, float size, Point2d p, Vector2d u_x, Vector2d u_y) : f(f), size(size), p(p), u_x(u_x), u_y(u_y) { }
  float Size() const { return size; }
  Point2d Index(float x) const
  {
    Point2d res = p;
    res += x*u_x + f.Index(x)*u_y;
    return res;
  }
private:
  Function<float,float> &f;
  float size;
  Point2d p;
  Vector2d u_x, u_y;
};


class CurveIn2dConvert : public CurveIn2d
{
public:
  CurveIn2dConvert(const Array<float, Point2d> &c2) : c2(c2) { }
  float Size() const { return c2.Size();}
  Point2d Index(float i) const { return c2.Index(i); }
private:
  const Array<float, Point2d> &c2;
};

class CurveIn3d : public Curve<Point>
{
public:
  virtual ~CurveIn3d() { }
  //virtual Point Index(float pos) const=0;
  //virtual float Length() const=0;
};

class FunctionCurveIn3d : public CurveIn3d
{
public:
  FunctionCurveIn3d(Function<float, float> &f, float size, Point p, Vector u_x, Vector u_y) : f(f), size(size), p(p), u_x(u_x), u_y(u_y) { }
  float Size() const { return size; }
  Point Index(float x) const
  {
    Point res = p;
    res += u_x*x + u_y*f.Index(x);
    return res;
  }
private:
  Function<float,float> &f;
  float size;
  Point p;
  Vector u_x;
  Vector u_y;
};


class CurveArrayConvert : public Array<int, Array<float, Point>* >
{
public:
  CurveArrayConvert(Array<int, CurveIn3d*> &arr) : arr(arr) { }
  int Size() const { return arr.Size(); }
  Array<float, Point>* Index(int i) const
  {
    return arr.Index(i); // conversion CurveIn3d* => Array<int,Point>*
  }
private:
  Array<int, CurveIn3d*> &arr;
};

class CurveIn3dConvert : public CurveIn3d
{
public:
  CurveIn3dConvert(const Array<float, Point> &c2) : c2(c2) { }
  float Size() const { return c2.Size(); }
  Point Index(float i) const { return c2.Index(i); }
private:
  const Array<float, Point> &c2;
};

class RepositionCurve : public CurveIn3d
{
public:
  RepositionCurve(CurveIn3d &curve, Point new_start_point) : curve(curve), new_start_point(new_start_point) { }
  float Size() const { return curve.Size(); }
  Point Index(float i) const
  {
    Point p = curve.Index(i);
    Point pp = curve.Index(0.0);
    p -= pp;
    p += new_start_point;
    return p;
  }
private:
  CurveIn3d &curve;
  Point new_start_point;
};
class ConstantSpeedCurve : public CurveIn3d
{
public:
  ConstantSpeedCurve(CurveIn3d &curve, int divs) : curve(curve), divs(divs) { }

  float Size() const
  {
    return LengthAtPos(curve.Size());
  }
  Point Index(float len) const
  {
    float pos = PosAtLength(len);
    return curve.Index(pos);
  }
  float LengthAtPos(float pos2) const
  {
    float delta = curve.Size()/divs;
    float pos = 0.0;
    float len = 0.0;
    for(int i = 0;i<divs;i++)
      {
	Point p = curve.Index(pos);
	Point p2 = curve.Index(pos+delta);
	Vector v = p2-p;
	float dist = v.Dist();
	len+=dist;	
	pos+=delta;
	if (pos > pos2) return len;
      }
    return 0.0;
  }
  
  float PosAtLength(float length) const
  {
    float delta = curve.Size()/divs;
    float pos = 0.0;
    float len = 0.0;
    for(int i = 0;i<divs;i++)
      {
	Point p = curve.Index(pos);
	Point p2 = curve.Index(pos+delta);
	Vector v = p2-p;
	float dist = v.Dist();
	len+=dist;	
	if (len > length) return pos;
	pos+=delta;
      }
  }
private:
  CurveIn3d &curve;
  int divs;
};


class CurveCollection : public Array<int, CurveIn3d*>
{
public:
};
class CurveCollection2d : public Array<int, CurveIn2d*>
{
public:
};

class MatrixCurve : public Curve<Matrix>
{
public:
  virtual ~MatrixCurve() { }
  //virtual Matrix Index(float pos) const=0;
  //virtual float Length() const=0;
};

class XRotate : public MatrixCurve
{
public:
  XRotate(float angle) : angle(angle) { }
  float Size() const { return angle*2.0*3.14159/360.0; }
  Matrix Index(float x) const
  {
    return Matrix::XRotation(x);
  }
private:
  float angle;
};

class MatrixCurveSequencer : public MatrixCurve
{
public:
  // must be pushed in order.
  void push_back(float starttime, float len, MatrixCurve *c)
  {
    start_time.push_back(starttime);
    end_time.push_back(starttime+len);
    length.push_back(c->Size());
    curve.push_back(c);
  }
  float Size() const 
  {
    if (end_time.Size()==0) return 0.0;
    return end_time.Index(end_time.Size()-1); 
  }
  Matrix Index(float x) const
  {
    int s = start_time.Size();
    Matrix m = Matrix::Identity();
    for(int i=0;i<s;i++)
      {
	float st = start_time.Index(i);
	float et = end_time.Index(i);
	if (x < st) continue;
	if (x >= st && x < et)
	  {
	    MatrixCurve *c = curve.Index(i);
	    float l = length.Index(i);
	    m = m * c->Index((x-st)*l/(et-st));
	  }
	if (x >= et)
	  {
	    MatrixCurve *c = curve.Index(i);
	    float l = length.Index(i);
	    m = m * c->Index(l);
	  }
      }
    return m;
  }
private:
  VectorArray<float> start_time;
  VectorArray<float> end_time;
  VectorArray<float> length;
  VectorArray<MatrixCurve*> curve;
};


class TranslateElement : public Element<Matrix>
{
public:
  TranslateElement(Point p) : p(p) { }
  Matrix Elem() const { return Matrix::Translate(p.x,p.y,p.z); }
private:
  Point p;
};
typedef FunctionImpl0<Point, Element<Matrix>*, TranslateElement> TranslateElementFunction;

class TranslateToMatrixCurve : public MatrixCurve
{
public:
  TranslateToMatrixCurve(CurveIn3d &curve) : curve(curve) { }
  Matrix Index(float pos) const 
  { 
    Point p = curve.Index(pos);
    return Matrix::Translate(p.x,p.y,p.z);
  }
  float Size() const { return curve.Size(); }
private:
  CurveIn3d &curve;
};
typedef FunctionImpl0<CurveIn3d*, MatrixCurve*, TranslateToMatrixCurve> TranslateToMatrixCurveFunction;

class MultiplyMatrixCurve : public MatrixCurve
{
public:
  MultiplyMatrixCurve(MatrixCurve &c1, MatrixCurve &c2) : c1(c1), c2(c2) { }
  float Size() const { return std::min(c1.Size(), c2.Size()); }
  Matrix Index(float x) const { return c1.Index(x)*c2.Index(x); }
private:
  MatrixCurve &c1, &c2;
};

class SpiralMatrixCurve : public MatrixCurve
{
public:
  SpiralMatrixCurve(float rot) : rot(rot) { }
  float Size() const { return 2.0*3.14159; }
  Matrix Index(float x) const { return Matrix::ZRotation(x*rot); }
private:
  float rot;  
};
typedef FunctionImpl0<float, MatrixCurve*, SpiralMatrixCurve> SpiralMatrixCurveFunction;

class MatrixCurveConvert : public MatrixCurve
{
public:
  MatrixCurveConvert(const Array<float, Matrix> &c2) : c2(c2) { }
  float Size() const { return c2.Size(); }
  Matrix Index(float i) const { return c2.Index(i); }
private:
  const Array<float, Matrix> &c2;
};
typedef FunctionImpl0<Array<float, Matrix>*, MatrixCurve*, MatrixCurveConvert> MatrixCurveConvertFunction;

class IdentityMatrixCurve : public MatrixCurve
{
public:
  IdentityMatrixCurve(float length) : length(length) { }
  Matrix Index(float pos) const { return Matrix::Identity(); }
  float Size() const { return length; }
private:
  float length;
};

class CircleMatrixCurve : public MatrixCurve
{
public:
  CircleMatrixCurve(float x) : x(x) { }
  virtual Matrix Index(float pos) const
  {
    return Matrix::Translate(x, 0.0, 0.0)*Matrix::YRotation(pos);
  }
  virtual float Size() const { return 3.14159*2.0; }  
private:
  float x;
};

class CircleMatrixCurve2 : public MatrixCurve
{
public:
  CircleMatrixCurve2(float x, float time) : x(x), time(time), m(Matrix::Translate(x,0.0,0.0)) { }
  virtual Matrix Index(float pos) const
  {
    float y = 1.0+0.2*sin(pos*3.0+time); // [0.8..1.2]
    
    return Matrix::Scale(y,y,y)*m*Matrix::YRotation(pos);
  }
  virtual float Size() const { return 3.14159*2.0; }  
private:
  float x;
  float time;
  Matrix m;
};

class MatrixCurveToCurveIn3d : public CurveIn3d
{
public:
  MatrixCurveToCurveIn3d(const MatrixCurve &curve, Vector v) : curve(curve), v(v) { }
  Point Index(float pos) const
  {
    return v*curve.Index(pos);
  }
  float Size() const { return curve.Size(); }
private:
  const MatrixCurve &curve;
  Vector v;
};

typedef FunctionImpl1<MatrixCurve*, CurveIn3d*, Vector, MatrixCurveToCurveIn3d> MatrixCurveToCurveIn3dFunction;


template<class T>
class ComposeTrait
{
};

template<>
class ComposeTrait<Point>
{
public:
  static Point Identity() { return Point(0.0,0.0,0.0); }
  static Point Compose(Point p, Point p2) { return p+p2; }
};

template<>
class ComposeTrait<Point2d>
{
public:
  static Point2d Identity() { Point2d p; p.x=0.0; p.y=0.0; return p; }
  static Point2d Compose(Point2d p, Point2d p2) 
  {
    Point2d pp;
    pp.x = p.x+p2.x;
    pp.y = p.y+p2.y;
    return pp; 
  }
};

template<>
class ComposeTrait<Matrix>
{
public:
  static Matrix Identity() { return Matrix::Identity(); }
  static Matrix Compose(Matrix m, Matrix m2) { return m*m2; }
};

template<class T, class Base>
class ComposeCurve : public Base
{
public:
  template<class It>
  ComposeCurve(It beg, It end) : vec(beg, end) { }

  T Index(float pos) const 
  {
    typedef typename std::vector<Base*>::iterator iterator;
    int ii =0;
    T res = ComposeTrait<T>::Identity();
    for(iterator i=vec.begin();i!=vec.end();i++,ii++)
      {
	float p = Pos(pos, ii);
	Base *b = *i;
	T t = b->Index(p);
	res = ComposeTrait<T>::Compose(res,t);
      }
    return res;
  }
  float Length() const 
  {
    typedef typename std::vector<Base*>::iterator iterator;
    float length = 0.0;
    for(iterator i=vec.begin();i!=vec.end();i++)
      {
	length += (*i)->Length();
      }
    return length;
  }
  float Pos(float pos, int i)
  {
    typedef typename std::vector<Base*>::iterator iterator;
    int length=0.0;
    for(iterator i=vec.begin();i!=vec.end();i++)
      {
	float len = (*i)->Length();
	float oldlength = length;
	length += len;
	if (pos < length) return pos-oldlength;
      }
    return length;
  }
private:
  std::vector<Base*> vec;
};

// already exists in Graph.hh
#if 0
class LineIn2d : public CurveIn2d
{
public:
  LineIn2d(Point2d p1, Point2d p2) : p1(p1), p2(p2) { }
  virtual Point2d Index(float pos) const 
  {
    return Point2d::FromVector(Vector2d::FromPoint(p1)*pos+Vector2d::FromPoint(p2)*(1.0-pos));
  }
  virtual float Size() const { return 1.0; }

private:
  Point2d p1,p2;
};
#endif

class LineIn3d : public CurveIn3d
{
public:
  LineIn3d(Point p1, Point p2) : p1(p1), p2(p2) { }
  virtual Point Index(float pos) const 
  {
    return Vector(p1)*pos+Vector(p2)*(1.0-pos);
  }
  virtual float Size() const { return 1.0; }
private:
  Point p1,p2;
  
};

class SurfaceIn3d
{
public:
  virtual ~SurfaceIn3d() { }
  virtual Point Index(float x, float y) const = 0;
  virtual Point2d Texture(float x, float y) const = 0;
  virtual float Attrib(float x, float y, int id) const=0;
  virtual int AttribI(float x, float y, int id) const=0;
  virtual Vector Normal(float x, float y) const = 0;
  virtual unsigned int Color(float x, float y) const =0;
  virtual float XSize() const = 0;
  virtual float YSize() const = 0;
};



class SurfaceIn3dImpl : public SurfaceIn3d
{
public:
  SurfaceIn3dImpl(SurfaceIn3d &next) : next(next) { }
  virtual Point Index(float x, float y) const { return next.Index(x,y); }
  virtual Point2d Texture(float x, float y) const { return next.Texture(x,y); }
  virtual float Attrib(float x, float y, int id) const { return next.Attrib(x,y,id); }
  virtual int AttribI(float x, float y, int id) const { return next.AttribI(x,y,id); }
  virtual Vector Normal(float x, float y) const { return next.Normal(x,y); }
  virtual unsigned int Color(float x, float y) const { return next.Color(x,y); }
  virtual float XSize() const { return next.XSize(); }
  virtual float YSize() const { return next.YSize(); }
private:
  SurfaceIn3d &next;
};

class MoveSurfaceIn3d : public SurfaceIn3dImpl
{
public:
  MoveSurfaceIn3d(SurfaceIn3d &next, Vector v) : SurfaceIn3dImpl(next), next(next), v(v) { }
  virtual Point Index(float x, float y) const { 
    Point p = next.Index(x,y);
    Move(p);
    return p;
  }
  void Move(Point &p) const { p+=v; } 
private:
  SurfaceIn3d &next;
  Vector v;
};

class BetweenSurface : public SurfaceIn3dImpl
{
public:
  BetweenSurface(SurfaceIn3d &next, CurveIn3d &first, CurveIn3d &second) : SurfaceIn3dImpl(next), first(first), second(second) { }
  virtual Point Index(float x, float y) const { return Pos(x,y); }
  virtual float XSize() const { return std::min(first.Size(), second.Size()); }
  virtual float YSize() const { return 1.0; }
  
  Point Pos(float x, float y) const
  {
    Point p1 = first.Index(x);
    Point p2 = second.Index(x);
    LineIn3d ln(p1,p2);
    return ln.Index(y);
  }
private:
  CurveIn3d &first;
  CurveIn3d &second;
};

class SurfaceBetweenSurfaces : public SurfaceIn3dImpl
{
public:
  SurfaceBetweenSurfaces(SurfaceIn3d &next, SurfaceIn3d &plane1, SurfaceIn3d &plane2, CurveIn2d &curve): SurfaceIn3dImpl(next), plane1(plane1), plane2(plane2), curve(curve) { }
  virtual Point Index(float x, float y) const 
  { 
    Point2d p1 = curve.Index(x);
    Point pp1 = plane1.Index(p1.x, p1.y);
    Point pp2 = plane2.Index(p1.x, p1.y);
    LineIn3d ln(pp1,pp2);
    return ln.Index(y);
  }
  
private:
  SurfaceIn3d &plane1;
  SurfaceIn3d &plane2;
  CurveIn2d &curve;
};


// Note, coordinates.hh has other surfaces
class SphereSurfaceIn3d : public SurfaceIn3d
{
public:
  SphereSurfaceIn3d(Point center, float radius) : center(center), radius(radius) { }
  virtual Point Index(float x, float y) const
  {
    Matrix m = Matrix::XRotation(x)*Matrix::YRotation(y);
    Vector v = Vector(0.0,0.0,radius);
    Vector pp = v*m;
    Point ps = center+pp;
    //std::cout << "SphereSurface:" << x << " " << y << ":" << ps << std::endl;
    return ps;
  }
  virtual Point2d Texture(float x, float y) const { Point2d p = { 0,0 }; return p; }
  virtual float Attrib(float x, float y, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float x, float y) const { Vector v; v.dx = 0.0; v.dy=0.0; return v; }
  virtual unsigned int Color(float x, float y) const { return 0; }
  virtual float XSize() const { return 2.0*3.14159; }
  virtual float YSize() const { return 3.14159; }
private:
  Point center;
  float radius;
};

class CurveInSurface : public CurveIn3d
{
public:
  CurveInSurface(SurfaceIn3d &surf, CurveIn2d &curve) 
    : surf(surf), curve(curve) { }
  float Size() const 
  {
    return curve.Size();
  }
  Point Index(float x) const
  {
    Point2d p = curve.Index(x);
    return surf.Index(p.x,p.y);
  }
private:
  SurfaceIn3d &surf;
  CurveIn2d &curve;  
};

class GridIn3d
{
public:
  virtual ~GridIn3d() { }
  virtual Point Index(int x, int y) const = 0;
  virtual Point2d Texture(int x, int y) const = 0;
  virtual Vector Normal(int x, int y) const = 0;
  virtual unsigned int Color(int x, int y) const =0;
  virtual float Attrib(int x, int y, int id) const = 0;
  virtual int AttribI(int x, int y, int id) const = 0;
  virtual int XSize() const = 0;
  virtual int YSize() const = 0;
};



class FloatArray : public Array<int, float>
{
public:
  virtual ~FloatArray() {}
  int NumFloats() const { return Size(); }
  //virtual int NumFloats() const=0;
  //virtual float Index(int x) const=0;
};

class FloatArrayConvert : public FloatArray
{
public:
  FloatArrayConvert(const Array<int, float> &c2) : c2(c2) { }
  int Size() const { return c2.Size(); }
  float Index(int i) const { return c2.Index(i);}
private:
  const Array<int, float> &c2;
};
typedef FunctionImpl0<Array<int, float>*, FloatArray*, FloatArrayConvert> FloatArrayConvertFunction;


class Circle : public CurveIn2d
{
public:
  Circle() : r(1.0) { center.x = 0; center.y = 0; }
  Circle(Point2d center, float r) : center(center), r(r) { }
  Point2d Index(float angle) const 
  { 
    Point2d res; 
    res.x=r*cos(angle) + center.x; 
    res.y = r* sin(angle) + center.y; 
    return res; 
  }
  float Size() const { return 2.0*3.14159; }
private:
  Point2d center;
  float r;
};
typedef FunctionImpl1<Point2d, CurveIn2d*, float, Circle> CircleFunction;

#if 0
class CircleFrom2Points
{
public:
  CircleFrom2Points(Point2d p1, Point2d p2, float r, bool b) : p1(p1), p2(p2), r(r), b(b) { }
  Point2d Center() const
  {
    // (p1.x+x)*(p1.x+x) + (p1.y+y)*(p1.y+y) = r*r
    // (p2.x+x)*(p2.x+x) + (p2.y+y)*(p2.y+y) = r*r
    
    // p1.x*p1.x + 2*(p1.x*x) + x*x + p1.y*p1.y + 2*p1.y*y + y*y = r*r
    // p2.x*p2.x + 2*(p2.x*x) + x*x + p2.y*p2.y + 2*p2.y*y + y*y = r*r
    // (p1.x*p1.x - p2.x*p2.x) + (p1.x-p2.x)*2*x +
    // (p1.y*p1.y - p2.y*p2.y) + (p1.y-p2.y)*2*y = 0

    // (p1.x-p2.x)*2*x = (p2.x*p2.x - p1.x*p1.x) + (p2.y*p2.y-p1.y*p1.y) + (p2.y-p1.y)*2*y
    float px = (p2.x*p2.x-p1.x*p1.x) / (p1.x-p2.x)/2.0;
    float py = (p2.y*p2.y-p1.y*p1.y) / (p1.x-p2.x)/2.0;
    float m = (p2.y-p1.y)*2/(p1.x-p2.x)/2.0;
    // line eq: x = px+py + m*y;

    // (p1.x+px+py+m*y)*(p1.x+px+py+m*y) + (p1.y+y)*(p1.y+y) = r*r
    // 
    
    
  }

private:
  Point2d p1;
  Point2d p2;
  float r;
  bool b;
};
#endif
class Distance2d : public Function<Point2d, float>
{
public:
  Distance2d(Point2d center) : center(center) { }
  float Index(Point2d p) const { p.x-=center.x; p.y-=center.y; return sqrt(p.x*p.x+p.y*p.y); }
private:
  Point2d center;
};
typedef FunctionImpl0<Point2d, Function<Point2d, float>*, Distance2d> Distance2dFunction;


class FloatMove : public Function<float, float>
{
public:
  FloatMove(float val) : val(val) { }
  float Index(float p) const { return p+val; }
private:
  float val;
};
typedef FunctionImpl0<float, Function<float, float>*, FloatMove> FloatMoveFunction;

class ScaleCurveIn2d : public CurveIn2d
{
public:
  ScaleCurveIn2d(CurveIn2d &curve, float x, float y) : curve(curve), x(x),y(y) { }
  float Size() const { return curve.Size(); }
  Point2d Index(float angle) const 
  {
    Point2d p = curve.Index(angle);
    p.x*=x;
    p.y*=y;
    return p;
  }
private:
  CurveIn2d &curve;
  float x,y;
};
typedef FunctionImpl2<CurveIn2d*, CurveIn2d*, float, float, ScaleCurveIn2d> ScaleCurveIn2dFunction;

class AnimCircle : public CurveIn2d
{
public:
  AnimCircle(float time, float count) : time(time), count(count) { }
  Point2d Index(float angle) const 
  {
    float r = 1.5+0.2*sin((angle+time)*count);
    Point2d res;
    res.x=r*cos(angle);
    res.y=r*sin(angle);
    return res;
  }
  float Size() const { return 2.0*3.14159; }
private:
  float time;
  float count;
};
typedef FunctionImpl1<float, CurveIn2d*, float, AnimCircle> AnimCircleFunction;

class CurveMultiply : public SurfaceIn3d
{
public:
  CurveMultiply(MatrixCurve &curve, CurveIn3d &curve2) : curve(curve), curve2(curve2) { }
  Point Index(float x, float y) const 
  {
    Point p = curve2.Index(y);
    Matrix m = curve.Index(x);
    return p*m;
  }
  Point2d Texture(float x, float y) const { Point2d p; p.x=x; p.y=y; return p; }
  unsigned int Color(float x, float y) const
  {
    return Color::White().Pixel();
  }
  Vector Normal(float x, float y) const
  {
    Point p1 = Index(x,y);
    Point p2 = Index(x+0.01,y);
    Point p3 = Index(x,y+0.01);
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual int AttribI(float x, float y, int id) const
  {
    return 0;
  }
  virtual float Attrib(float x, float y, int id) const
  {
    switch(id)
      {
      case AttrInternalX: return x;
      case AttrInternalY: return y;
      };
    return 0.0;
  }

  float XSize() const { return curve.Size(); }
  float YSize() const { return curve2.Size(); }
private:
  MatrixCurve &curve;
  CurveIn3d &curve2;
};

class PlaneIn3d : public Element<Point>
{
public:
  PlaneIn3d(Point2d p, Plane pl) : p(p), pl(pl) { }
  Point Elem() const
  {
    return pl.u_p + p.x*pl.u_x + p.y*pl.u_y;
  }
private:
  Point2d p;
  Plane pl;  
};
class PlaneIn3d2 : public Function<Point2d, Point>
{
public:
  PlaneIn3d2(Plane pl) : pl(pl) { }
  Point Index(Point2d p) const
  {
    return pl.u_p + p.x*pl.u_x+p.y*pl.u_y;
  }
private:
  Plane pl;
};


typedef FunctionImpl1<Point2d, Element<Point>*, Plane, PlaneIn3d> PlaneIn3dFunction;

class PlaneCurveIn3d : public CurveIn3d
{
public:
  PlaneCurveIn3d(CurveIn2d &c, Plane p) : c(c), p(p) { }
  PlaneCurveIn3d(CurveIn2d *c, Plane p) : c(*c), p(p) { }
  float Size() const { return c.Size(); }
  Point Index(float angle) const
  {
    Point2d pp = c.Index(angle);
    Point pp2 = p.u_p + pp.x*p.u_x + pp.y*p.u_y;

    //std::cout << Point(p.u_x) << " " << Point(p.u_y) << " " << pp.x << " " << pp.y << " " << pp2 << std::endl;
    return pp2;
  }
private:
  CurveIn2d &c;
  Plane p;
};
typedef FunctionImpl1<CurveIn2d*, CurveIn3d*, Plane, PlaneCurveIn3d> PlaneCurveIn3dFunction;

class MultiplyCurves : public SurfaceIn3d
{
public:
  MultiplyCurves(const CurveIn3d &c1, const CurveIn3d &c2) : c1(c1), c2(c2) { }
  virtual Point Index(float x, float y) const 
  {
    return c1.Index(x)+c2.Index(y);
  }
  virtual Point2d Texture(float x, float y) const { Point2d p; p.x=x; p.y=y; return p; }
  virtual unsigned int Color(float x, float y) const
  {
    return Color::White().Pixel();
  }
  virtual Vector Normal(float x, float y) const {
    Point p1 = Index(x,y);
    Point p2 = Index(x+0.01,y);
    Point p3 = Index(x,y+0.01);
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);

    // Vector u_x = c1.Index(x+0.1)-c1.Index(x);
    //Vector u_y = c2.Index(y+0.1)-c2.Index(y);
    //return Vector::CrossProduct(u_x, u_y);
  }
  virtual int AttribI(float x, float y, int id) const
  {
    return 0;
  }
  virtual float Attrib(float x, float y, int id) const
  {
    switch(id)
      {
      case AttrInternalX: return x/XSize(); 
      case AttrInternalY: return y/YSize();
      }
    return 0.0;
  }
  virtual float XSize() const { return c1.Size(); }
  virtual float YSize() const { return c2.Size(); }

private:
  const CurveIn3d &c1;
  const CurveIn3d &c2;
};

class PlaneSurfaceIn3d : public SurfaceIn3d
{
public:
  PlaneSurfaceIn3d(Plane pl) 
    : line_x(pl.u_p, pl.u_p+pl.u_x),
      line_y(pl.u_p, pl.u_p+pl.u_y),
      mult(line_x, line_y)
  { }

  virtual Point Index(float x, float y) const { return mult.Index(x,y); }
  virtual Point2d Texture(float x, float y) const { return mult.Texture(x,y); }
  virtual float Attrib(float x, float y, int id) const { return mult.Attrib(x,y,id); }
  virtual int AttribI(float x, float y, int id) const { return mult.AttribI(x,y,id); }
  virtual Vector Normal(float x, float y) const { return mult.Normal(x,y); }
  virtual unsigned int Color(float x, float y) const { return mult.Color(x,y); }
  virtual float XSize() const 
  { return mult.XSize(); }
  virtual float YSize() const { return mult.YSize(); }

private:
  LineIn3d line_x;
  LineIn3d line_y;
  MultiplyCurves mult;
};


class AnimCurveIn3d : public CurveIn3d
{
public:
  AnimCurveIn3d(CurveIn3d &c1, CurveIn3d &c2, float time, float length) : c1(c1), c2(c2), time(time), length(length) { }
  virtual Point Index(float pos) const
  {
    float p = time/length;

    float pos1 = pos*c1.Size()/Size();
    float pos2 = pos*c2.Size()/Size();
    return Vector(c1.Index(pos1))*p+Vector(c2.Index(pos2))*(1.0-p);
  }
  virtual float Size() const
  {
    float p = time/length;
    return c1.Size()*p+c2.Size()*(1.0-p);
  }
private:
  CurveIn3d &c1, &c2;
  float time, length;
};




class ContinuousPlanes
{
public:
  virtual ~ContinuousPlanes() { }
  virtual Plane Index(float x)=0;
};

class PlanesFromCurve : public ContinuousPlanes
{
public:
  PlanesFromCurve(CurveIn3d &center, CurveIn3d &x, CurveIn3d &y) : center(center), x(x), y(y) { }

  Plane Index(float xx) { Plane p; p.u_p = center.Index(xx); p.u_x = x.Index(xx)-p.u_p; p.u_y = y.Index(xx)-p.u_p; return p; }
private:
  CurveIn3d &center, &x, &y;
};


class FrameAnimPlugins : public FrameAnim
{
public:
  FrameAnimPlugins(Render *render) : FrameAnim(render) { }
  void Init() { 
    for(std::vector<FramePlugin*>::iterator i=plugins.begin();i!=plugins.end();i++)
      (*i)->Init();
  }
  void Cleanup() {
    for(std::vector<FramePlugin*>::iterator i=plugins.begin();i!=plugins.end();i++)
      (*i)->Cleanup();
  }
  void PreFrame(float time)
  {
    for(std::vector<FramePlugin*>::iterator i=plugins.begin();i!=plugins.end();i++)
      (*i)->PreFrame(time);
  }
  void PostFrame()
  {
    for(std::vector<FramePlugin*>::iterator i=plugins.begin();i!=plugins.end();i++)
      (*i)->PostFrame();
  }
  virtual ~FrameAnimPlugins() 
  {
    for(std::vector<FramePlugin*>::iterator i=plugins.begin();i!=plugins.end();i++)
      delete *i;
  }
public:
  std::vector<FramePlugin*> plugins;
};

class Storage
{
public:
  virtual ~Storage() {}
  virtual int MatrixCount() const = 0;
  virtual Matrix &IndexMatrix(int i) const=0;
  virtual int PointCount() const = 0;
  virtual Point &IndexPoint(int i) const =0;
  virtual int VectorCount() const = 0;
  virtual Vector &IndexVector(int i) const =0;
  virtual int FloatCount() const = 0;
  virtual float &IndexFloat(int i) const =0;
};


class TexturePlugin : public FramePlugin
{
public:
  TexturePlugin(BufferRef buf) : buf(buf), texture(5566) { }

  void Init();

  void Start();
  void End();
  ~TexturePlugin();
  unsigned int Texture() const { return texture; }
public:
  BufferRef buf;
  unsigned int texture;
};

void Execute(FrameAnim &f, SDL_Surface *screen);
BufferRef LoadImage(std::string filename);

class PointCollection : public Array<int, Point>
{
public:
  virtual ~PointCollection() { }
  int NumPoints() const { return Size(); }
  Point Points(int index) const { return Index(index); }
  //virtual Point Points(int index) const=0;
};


class PointCollectionConvert : public PointCollection
{
public:
  PointCollectionConvert(const Array<int, Point> &c2) : c2(c2) { }
  int Size() const { return c2.Size();}
  Point Index(int i) const { return c2.Index(i); }
private:
  const Array<int, Point> &c2;
};
typedef FunctionImpl0<Array<int,Point>*, PointCollection*, PointCollectionConvert> PointCollectionConvertFunction;

class PointCollectionConvertArray : public Array<int, PointCollection*>
{
public:
  PointCollectionConvertArray(Array<int, Array<int, Point>*> &arr) : arr(arr), convert(0) { }
  int Size() const { return arr.Size(); }
  PointCollection *Index(int i) const
  {
    delete convert;
    convert = new PointCollectionConvert(*arr.Index(i));
    return convert;
  }
  ~PointCollectionConvertArray() { delete convert; }
private:
  Array<int, Array<int, Point>*> &arr;
  mutable PointCollectionConvert *convert;
};


class Triangle : public PointCollection
{
public:
  Triangle(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3) { }
  int Size() const { return 3; }
  Point Index(int i) const {
    switch(i)
      {
      case 0: return p1;
      case 1: return p2;
      case 2: return p3;
      }
    return Point(0.0,0.0,0.0);
  }
private:
  Point p1, p2, p3;
};

class InterpolateTriangle
{
public:
  InterpolateTriangle(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3)
  {
  }
  // (x,y) \in Triangle((0.0,0.0), (1.0,0.0), (0.0,1.0))
  Point p12_13(float x, float y) const
  {
    Vector v1(p2-p1);
    Vector v2(p3-p1);
    v1*=x;
    v2*=y;
    return p1+v1+v2;
  }
  Point p21_23(float x, float y) const
  {
    Vector v1(p1-p2);
    Vector v2(p3-p2);
    v1*=x;
    v2*=y;
    return p2+v1+v2;
  }
  Point p31_32(float x, float y) const
  {
    Vector v1(p1-p3);
    Vector v2(p2-p3);
    v1*=x;
    v2*=y;
    return p3+v1+v2;
  }

private:
  Point p1,p2,p3;
};

class PolygonEdges1 : public FloatArray
{
public:
  PolygonEdges1(PointCollection &coll) : coll(coll) { }
  int Size() const
  {
    return coll.Size()+1;
  }
  float Index(int ii) const
  {
    if (ii==0) return 0.0;
    int s = coll.Size();
    Point old_p = coll.Index(0);
    float dist = 0.0;
    for(int i=1;i<s;i++)
      {
	Point p = coll.Index(i);
	dist += Dist(old_p, p);
	if (i==ii) return dist;
	old_p = p;
      }
    dist+=Dist(old_p, coll.Index(0));
    return dist;
  }
private:
  PointCollection &coll;
};
class PolygonEdges2 : public CurveIn3d
{
public:
  PolygonEdges2(PointCollection &coll) : coll(coll) { }
  float Size() const
  {
    Point old_p = coll.Index(0);
    float dist = 0.0;
    int s = coll.Size();
    for(int i=1;i<s;i++)
      {
	Point p = coll.Index(i);
	dist += Dist(old_p, p);
	old_p = p;
      }
    dist+=Dist(old_p, coll.Index(0));
    return dist;
  }
  Point Index(float x) const
  {
    Point old_p = coll.Index(0);
    float dist = 0.0;
    int s = coll.Size();
    for(int i=1;i<s;i++)
      {
	float old_dist = dist;
	Point p = coll.Index(i);
	dist += Dist(old_p, p);
	if (dist>=x)
	  {
	    LineIn3d line(old_p, p);
	    return line.Index((x - old_dist)/(dist-old_dist));
	  }
	old_p = p;
      }
    LineIn3d line2(old_p, coll.Index(0));
    return line2.Index((x-dist)/Dist(old_p, coll.Index(0)));
  }
private:
  PointCollection &coll;
};

// finds nearest point which is not the current point
class FindNearestPoint : public PointCollection
{
public:
  FindNearestPoint(PointCollection &coll) : coll(coll) 
  {
    size = coll.Size();
    array = new Point[size];
    for(int i=0;i<size;i++)
      {
	array[i] = coll.Index(i);
      }
  }
  int Size() const { return size; }
  Point Index(int ii) const
  {
    int dist = 999999.0;
    Point res(0.0,0.0,0.0);
    Vector ref(array[ii]);
    for(int i=0;i<size;i++)
      {
	Point ai = array[i];
	float d = (ai-ref).Dist();
	if (d<dist)
	  {
	    // check if we're at same point.
	    if (i != ii)
	      {
		res = ai;
		dist = d;
	      }
	  }
      }
    return res;
  }
  ~FindNearestPoint() { delete [] array; }
private:
  Point *array;
  int size;
  PointCollection &coll;
};

class PointCollection2d : public Array<int, Point2d>
{
};

typedef ArrayConvert<PointCollection2d, int, Point2d> PointCollection2dConvert;

class TriPointCollection2d : public PointCollection2d
{
public:
  TriPointCollection2d(Point2d p1, Point2d p2, Point2d p3) : p1(p1), p2(p2), p3(p3) { }
  int Size() const { return 3; }
  Point2d Index(int i) const
  {
    switch(i)
      {
      case 0: return p1;
      case 1: return p2;
      case 2: return p3;
      };
    Point2d p = { 0.0,0.0 };
    return p;
  }
private:
  Point2d p1,p2,p3;
};
class QuadPointCollection2d : public PointCollection2d
{
public:
  QuadPointCollection2d(Point2d p1, Point2d p2, Point2d p3, Point2d p4) : p1(p1), p2(p2), p3(p3),p4(p4) { }
  int Size() const { return 4; }
  Point2d Index(int i) const
  {
    switch(i)
      {
      case 0: return p1;
      case 1: return p2;
      case 2: return p3;
      case 3: return p4;
      };
    Point2d p = { 0.0,0.0 };
    return p;
  }
private:
  Point2d p1,p2,p3,p4;
};

class DynPointCollection2d : public PointCollection2d
{
public:
  int Size() const { return vec.size(); }
  Point2d Index(int i) const
  {
    return vec[i];
  }
  void push_back(Point2d p) { vec.push_back(p); }
private:    
  std::vector<Point2d> vec;
};


class SampleCurveIn2d : public PointCollection2d
{
public:
  SampleCurveIn2d(const CurveIn2d &curve, int count, float length) : curve(curve), count(count), length(length) { }
  int Size() const
  {
    return count;
  }
  Point2d Index(int i) const
  {
    float x = i * length / count;
    return curve.Index(x);
  }
private:
  const CurveIn2d &curve;
  int count;
  float length;
};

class PolyFromCurve2d : public PointCollection2d
{
public:
  PolyFromCurve2d(Function<float,float> &curve, float y, float start_x, float end_x, int x_numpoints) : curve(curve), y(y), start_x(start_x), end_x(end_x), x_numpoints(x_numpoints) { }
  int Size() const
  {
    return x_numpoints+2;
  }
  Point2d Index(int i) const
  {
    if (i==0) 
      {
	Point2d corner = { start_x,y };
	return corner;
      }
    if (i>0 && i<x_numpoints+1)
      {
	i-=1;
	float x = start_x + i*(end_x-start_x)/x_numpoints;
	float yy = curve.Index(x);
	if (yy<y) yy=y;
	Point2d p = { x,yy };
	return p;
      }    
    if (i==x_numpoints+1)
      {
	Point2d corner = { end_x, y };
	return corner;
      }
  }
  
private:
  Function<float,float> &curve;
  float y;
  float start_x;
  float end_x;
  int x_numpoints;
};

class MaxCurve : public Function<float,float>
{
public:
  MaxCurve(Function<float,float> &f1, Function<float,float> &f2) : f1(f1), f2(f2) { }
  float Index(float x) const
  {
    return std::max(f1.Index(x),f2.Index(x));
  }
private:
  Function<float,float> &f1;
  Function<float,float> &f2;
};

class PolyFromTwoCurves2d : public PointCollection2d
{
public:
  PolyFromTwoCurves2d(Function<float,float> &curve1,
		      Function<float,float> &curve2,
		      float start_x, float end_x, int x_numpoints)
    : curve1(curve1), curve2(curve2), start_x(start_x), end_x(end_x), x_numpoints(x_numpoints) { }

  int Size() const
  {
    return x_numpoints*2;
  }
  Point2d Index(int i) const
  {
    Point2d p;
    return p;
  }
private:
  Function<float,float> &curve1;
  Function<float,float> &curve2;
  float start_x;
  float end_x;
  int x_numpoints;
};


class PlanePointsIn3d : public PointCollection
{
public:
  PlanePointsIn3d(PointCollection2d &pc, Plane p) : pc(pc), p(p) { }
  int Size() const { return pc.Size(); }
  Point Index(int num) const
  {
    Point2d pp = pc.Index(num);
    Point pp2 = p.u_p + pp.x*p.u_x + pp.y*p.u_y;
    return pp2;
  }
private:
  PointCollection2d &pc;
  Plane p;
};
typedef FunctionImpl1<PointCollection2d*, PointCollection*, Plane, PlanePointsIn3d> PlanePointsIn3dFunction;

class VectorCollection : public Array<int, Vector>
{
public:
  virtual ~VectorCollection() { }
  int NumVectors() const { return Size(); }
  Vector Vectors(int index) const { return Index(index); }
};

class VectorCollectionConvert : public VectorCollection
{
public:
  VectorCollectionConvert(const Array<int, Vector> &c2) : c2(c2) { }
  int Size() const { return c2.Size(); }
  Vector Index(int i) const { return c2.Index(i);}
private:
  const Array<int, Vector> &c2;
};
typedef FunctionImpl0<Array<int, Vector>*,VectorCollection*, VectorCollectionConvert> VectorCollectionConvertFunction;

class SimpleVectorCollection : public VectorCollection
{
public:
  SimpleVectorCollection(Vector v) : v(v) { }
  int Size() const { return 1; }
  Vector Index(int index)const { return v; }
private:
  Vector v;
};
typedef FunctionImpl0<Vector, VectorCollection*, SimpleVectorCollection> SimpleVectorCollectionFunction;

class VectorCurve : public Array<float, Vector>
{
};

class PointVectorCurve : public Array<float, std::pair<Point, Vector> >
{
};
class PointVectorCollection : public Array<int, std::pair<Point,Vector> >
{
};


class PointVectorCollectionConvert : public PointVectorCollection
{
public:
  PointVectorCollectionConvert(const Array<int, std::pair<Point,Vector> > &c2) : c2(c2) { }
  int Size() const { return c2.Size(); }
  std::pair<Point,Vector> Index(int i) const { return c2.Index(i); }
private:
  const Array<int, std::pair<Point,Vector> > &c2;
};

class Distance : public FloatArray
{
public:
  Distance(VectorCollection &coll) : coll(coll) { }
  int Size() const { return coll.NumVectors(); }
  float Index(int x) const 
  {
    Vector v = coll.Vectors(x);
    return sqrt(v.dx*v.dx+v.dy*v.dy+v.dz*v.dz);
  }
  float Sum() const {
    int size = Size();
    float d = 0.0;
    for(int i=0;i<size;i++)
      {
	d+=Index(i);
      }
    return d;
  }
private:
  VectorCollection &coll;
};
typedef FunctionImpl0<VectorCollection*, FloatArray*, Distance> DistanceFunction;

class WeightedSampling : public FloatArray
{
public:
  WeightedSampling(FloatArray &weights, float start) : weights(weights), start(start) 
  {
    //float s = start;
    //int size = Size();
    //for(int i=0;i<size;i++)
    //  {
    //  s+=weights.Index(i);
    //  vec.push_back(s);
    //  }    
  }
  int Size() const { return weights.NumFloats(); }
  float Index(int x) const
  {
    //return vec[x];
    float s = start;
    
    for(int i=0;i<x;i++)
      s+=weights.Index(i);
    return s;
  }
private:
  FloatArray &weights;
  float start;
};

class ConstantSampling : public FloatArray
{
public:
  ConstantSampling(int count, float dist) : count(count), dist(dist) { }
  int Size() const { return count; }
  float Index(int x) const 
  {
    float length = dist;
    length /= float(count);
    length *= float(x);
    return length;
  }
private:
  int count;
  float dist;
};

class PointCollectionFromGrid : public PointCollection
{
public:
  PointCollectionFromGrid(GridIn3d &grid) : grid(grid) { }
  int Size() const { return grid.XSize()*grid.YSize(); }

  Point Index(int index) const
  {
    int size = grid.XSize();
    int y = index / size;
    int x = index - y*size;
    Point p = grid.Index(x,y);
    return p;
  }
private:
  GridIn3d &grid;
};
typedef FunctionImpl0<GridIn3d*, PointCollection*, PointCollectionFromGrid> PointCollectionFromGridFunction;

class FaceId
{
public:
  int array[5];
  int elems_used;
};

class TextureId
{
public:
  int id;
};

class FaceCollection
{
public:
  virtual ~FaceCollection() { }
  virtual int NumFaces() const = 0;
  virtual int NumPoints(int face) const=0;
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual float Attrib(int face, int point, int id) const=0;
  virtual int AttribI(int face, int point, int id) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;

  virtual Point EndFacePoint(int face, int point) const { return FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return TexCoord(face,point); }

  virtual float Duration() const { return 1.0; }

  virtual int NumTextures() const { return 0; }
  virtual void GenTexture(int num) { }
  virtual BufferRef TextureBuf(int num) const { BufferRef ref; return ref; }
  virtual int FaceTexture(int face) const { return -1; }
  //virtual void Pullback() const=0; // should call P functions only
  //void P(FaceCollection &single);
  //void P(FaceCollection &coll1, FaceCollection &coll2);
  //void P(FaceCollection *coll, int size); // needed in or operation
  //virtual int RangeCount();
  //virtual Range<int> RangeArray(int count);
#if 0
  struct Triplet
  {
    P poly_id;
    int face;
    int point;
  };
  virtual Triplet FaceTriplet(int face, int point) const=0;
#endif
#if 0
  virtual Coords Coordinates() const=0;
#endif
};

class ForwardFaceCollection : public FaceCollection
{
public:
  ForwardFaceCollection(FaceCollection &coll) : coll(coll) { }
  virtual int NumFaces() const { return coll.NumFaces(); }
  virtual int NumPoints(int face) const { return coll.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return coll.FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { return coll.PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return coll.Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return coll.AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return coll.Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return coll.TexCoord(face,point); }
  virtual int NumTextures() const { return coll.NumTextures(); }
  virtual void GenTexture(int num) { coll.GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return coll.TextureBuf(num); }
  virtual int FaceTexture(int face) const { return coll.FaceTexture(face); }
private:
  FaceCollection &coll;
};
class RecalculateNormals : public ForwardFaceCollection
{
public:
  RecalculateNormals(FaceCollection &coll) : ForwardFaceCollection(coll) { }
  Vector PointNormal(int face, int point) const 
  { 
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    return -Vector::CrossProduct(p2-p1,p3-p1);
  }
};

class ChangePoints : public ForwardFaceCollection
{
public:
  ChangePoints(FaceCollection &coll, Function<Point,Point> &f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual Point FacePoint(int face, int point) const 
  { 
    Point p = ForwardFaceCollection::FacePoint(face,point); 
    return f.Index(p);
  }
private:
  Function<Point,Point> &f;  
};


// TODO: do MemoizeAnim to speed up precalc
class Anim
{
public:
  virtual int NumFrames() const=0;
  virtual FaceCollection *Frame(int framenum) const=0;
};

class AnimSet
{
public:
  virtual void Set(int index, bool b)=0;
};

class AnimSelectI
{
public:
  virtual int Size() const=0;
  virtual int *SelectArray() const=0;
};

class SubSelect : public AnimSet
{
public:
  SubSelect(AnimSet &s, int start, int size) : s(s), start(start), size(size) { }
  void Set(int index, bool b)
  {
    s.Set(index+start, b);
  }
private:
  AnimSet &s;
  int start;
  int size;
};

class AnimSelect : public AnimSelectI, public AnimSet
{
public:
  AnimSelect(int size) : size(size) 
  { 
    selectarray = new int[size]; 
    for(int i=0;i<size;i++)
      {
	selectarray[i] = 0;
      }
  }
  ~AnimSelect() { delete [] selectarray; }
  void Set(int index, bool b)
  {
    if (index<0 || index >=size) return;
    if (b) { selectarray[index] = 1; }
    else { selectarray[index] = 0; }
  }
  int *SelectArray() const { return selectarray; }
  int Size() const { return size; }
private:
  int *selectarray;
  int size;
};

class FrameSelect
{
public:
  FrameSelect(AnimSet &s) : s(s) { }
  void set_frame(int framenum) 
  {
    s.Set(frame, false);
    s.Set(framenum, true);
    frame = framenum;
  }
private:
  AnimSet &s;
  int frame;
};


class CoordSource
{
public:
  virtual Coords coord() const=0;
};

class RotateAroundEdge : public MatrixCurve
{
public: // m = position of object at time 0
  RotateAroundEdge(Matrix m, FaceCollection &obj, int face, int point1, int point2, float speed, float length) : m(m), obj(obj), speed(speed), face(face), point1(point1), point2(point2), length(length) { }
  float Size() const { return length; }
  Matrix Index(float time) const
  {
    Point p1 = obj.FacePoint(face, point1);
    Point p2 = obj.FacePoint(face, point2);
    Point p1m = p1 * m;
    Point p2m = p2 * m;
    Vector v = p2m - p1m;
    Matrix mm = Matrix::RotateAroundAxisPoint(p1m, v, time*speed);    
    return mm;
  }
private:
  Matrix m;
  FaceCollection &obj;
  float speed;
  int face, point1, point2;
  float length;
};



class FaceCollectionArray : public Array<int, const FaceCollection*>
{
};

class AnimFaceCollectionArray : public FaceCollectionArray
{
public:
  AnimFaceCollectionArray(const Anim &a) : a(a) { }
  int Size() const { return a.NumFrames(); }
  const FaceCollection *Index(int i) const
  {
    return a.Frame(i);
  }
private:
  const Anim &a;
};

typedef ArrayConvert<FaceCollectionArray, int, const FaceCollection*> FaceCollectionArrayConvert;

class IdentifyObjectsFromArray : public FaceCollectionArray, private FaceCollection
{
public:
  IdentifyObjectsFromArray(FaceCollectionArray &array) : arr(array) { }
  int Size() const { return arr.Size(); }
  const FaceCollection *Index(int object) const
  {
    object_id = object;
    return this;
  }
  virtual int NumFaces() const { return arr.Index(object_id)->NumFaces(); }
  virtual int NumPoints(int face) const { return arr.Index(object_id)->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return arr.Index(object_id)->FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { return arr.Index(object_id)->PointNormal(face,point); }
  virtual int AttribI(int face, int point, int id) const 
  {
    return arr.Index(object_id)->AttribI(face,point,id);

  }
  virtual float Attrib(int face, int point, int id) const 
  {
    if (id == AttrObjectId)
      {
	return object_id;
      }
    return arr.Index(object_id)->Attrib(face,point,id);
  }
private:
  FaceCollectionArray &arr;
  mutable int object_id;
};

class DuplicateObject : public FaceCollectionArray
{
public:
  DuplicateObject(const FaceCollection &coll, int count) : coll(coll), count(count) { }
  
  int Size() const { return count; }
  const FaceCollection *Index(int i) const
  {
    return &coll;
  }
private:
  const FaceCollection &coll;
  int count;
};

class CompressObject : public FaceCollection
{
public:
  CompressObject(FaceCollectionArray &arr) : arr(arr), oldcount(-1) { }
  virtual int NumFaces() const
  {
    if (oldcount != -1) return oldcount;
    int count = 0;
    int num = arr.Size();
    for(int i=0;i<num;i++)
      {
	count += arr.Index(i)->NumFaces();
      }
    oldcount = count;
    return count;
  }
  virtual int NumPoints(int face) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->NumPoints(face-oldcount); }
      }
    return 0;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->FacePoint(face-oldcount, point); }
      }
    std::cout << "CompressObject::Point" << std::endl;
    return Point(0.0,0.0,0.0);

  }

  virtual unsigned int Color(int face, int point) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->Color(face-oldcount, point); }
      }
    std::cout << "CompressObject::0" << std::endl;
    return 0;
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->TexCoord(face-oldcount, point); }
      }
    std::cout << "CompressObject::0" << std::endl;
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }


  virtual Vector PointNormal(int face, int point) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->PointNormal(face-oldcount, point); }
      }
    std::cout << "CompressObject::Vector" << std::endl;
    return Vector(0.0,0.0,0.0);

  }
  virtual float Attrib(int face, int point, int id) const
  {
    int count = 0;
    int num = arr.Size();
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->Attrib(face-oldcount, point, id); }
      }
    return 0.0;
  }

  virtual int AttribI(int face, int point, int id) const
  {
    int count = 0;
    int num = arr.Size();
    //std::cout << "AttribI: " << num << std::endl;
    for (int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (count > face) { return arr.Index(i)->AttribI(face-oldcount, point, id); }
      }
    return 0.0;
  }


private:
  FaceCollectionArray &arr;
  mutable int oldcount;
};

class FaceCollectionTri
{
public:
  virtual ~FaceCollectionTri() { }
  virtual int NumFaces() const=0;
  virtual int NumPoints() const=0;
  virtual Point FacePoint(int index) const=0;
  virtual Vector PointNormal(int index) const=0;
  virtual Point2d TexCoord(int index) const=0;
  virtual unsigned int Color(int index) const=0;
  virtual float Attrib(int index, int id) const=0;
  virtual int AttribI(int index, int id) const=0;
  virtual int Index(int face, int point) const=0;
  virtual int IndexCount() const=0;
  virtual int IndexToFace(int index) const=0;
};

class SeparateArrayElements : public Array<int,const FaceCollectionTri*>
{
public:
  SeparateArrayElements(const Array<int,const FaceCollectionTri*> &arr) : arr(arr) { }
  struct Arr : public FaceCollectionTri
  {
    Arr(const FaceCollectionTri &tri, const Array<int,const FaceCollectionTri*> &arr, int t) : tri(tri),arr(arr),t(t) { }
    virtual int NumFaces() const { arr.Index(t); return tri.NumFaces(); }
    virtual int NumPoints() const { arr.Index(t); return tri.NumPoints(); }
    virtual Point FacePoint(int index) const { arr.Index(t); return tri.FacePoint(index); }
    virtual Vector PointNormal(int index) const { arr.Index(t); return tri.PointNormal(index); }
    virtual unsigned int Color(int index) const { arr.Index(t); return tri.Color(index); }
    virtual Point2d TexCoord(int index) const { arr.Index(t); return tri.TexCoord(index); }

    virtual float Attrib(int index, int id) const { arr.Index(t); return tri.Attrib(index,id); }
    virtual int AttribI(int index, int id) const { arr.Index(t); return tri.AttribI(index,id); }
    virtual int Index(int face, int point) const { arr.Index(t); return tri.Index(face,point); }
    virtual int IndexCount() const { arr.Index(t); return tri.IndexCount(); }
    virtual int IndexToFace(int index) const { arr.Index(t); return tri.IndexToFace(index); }
  private:
    const FaceCollectionTri &tri;
    const Array<int,const FaceCollectionTri*> &arr;
    int t;
  };
  int Size() const { return arr.Size(); }
  const FaceCollectionTri *Index(int i) const
  {
    Arr *a = new Arr(*arr.Index(i), arr, i);
    owner.push_back(a);
    return a;
  }
  ~SeparateArrayElements() 
  {
    for(std::vector<Arr*>::iterator i = owner.begin();i!=owner.end();i++)
      {
	delete *i;
      }
  }
private:
  const Array<int,const FaceCollectionTri*> &arr;
  mutable std::vector<Arr*> owner;
};

class BatchGeneration : public Array<int, const FaceCollectionTri*>, private FaceCollectionTri
{
public:
  BatchGeneration(FaceCollectionTri &tri, int batchsize) : tri(tri), batchsize(batchsize) { }
public: // FaceCollectionTri
  virtual int NumFaces() const { return CurrentBatchSize(); }
  virtual int NumPoints() const 
  { 
    int i = LastIndex()-FirstIndex(); 
    //std::cout << "NumPoints() " << i << std::endl;
    return i;
  }
  virtual Point FacePoint(int index) const 
  {
    return tri.FacePoint(IndexMap(index));    
  }
  virtual Vector PointNormal(int index) const
  {
    return tri.PointNormal(IndexMap(index));
  }
  virtual unsigned int Color(int index) const
  {
    return tri.Color(IndexMap(index));
  }
  virtual Point2d TexCoord(int index) const
  {
    return tri.TexCoord(IndexMap(index));
  }
  virtual float Attrib(int index, int id) const
  {
    return tri.Attrib(IndexMap(index), id);
  }
  virtual int AttribI(int index, int id) const
  {
    return tri.AttribI(IndexMap(index), id);
  }
  virtual int Index(int face, int point) const
  { // input face=[0..currentbatchsize()]
    // output index = [0..(LastIndex()-FirstIndex()]
    return IndexMapI(tri.Index(FaceMap(face), point));
  }
  virtual int IndexCount() const { return LastIndex()-FirstIndex(); }
  virtual int IndexToFace(int index) const
  {
    return FaceMapI(tri.IndexToFace(IndexMap(index)));
  }
private:
  int FaceMap(int face) const
  {
    face += batchsize*currentbatch;
    return face;
  }
  int FaceMapI(int face) const
  {
    face -= batchsize*currentbatch;
    return face;
  }
  int IndexMap(int index) const
  {
    index += FirstIndex();
    return index;
  }
  int IndexMapI(int index) const
  {
    index -= FirstIndex();
    return index;
  }
  int BatchFace(int face) const
  {
    return face % batchsize;
  }
  int FirstIndex() const
  {
    return tri.Index(batchsize*currentbatch,0);
  }
  int LastIndex() const
  {
    return tri.Index(batchsize*currentbatch+CurrentBatchSize(),0);
  }
  int CurrentBatchSize() const
  {
    int numfaces = tri.NumFaces();
    numfaces -= currentbatch*batchsize;
    if (numfaces > batchsize) return batchsize;
    return numfaces;
  }
public: // Array<int, FaceCollectionTri*>
  int Size() const { return tri.NumFaces()/batchsize + 1; }
  const FaceCollectionTri *Index(int i) const
  {
    currentbatch = i;
    return this;
  }
private:
  FaceCollectionTri &tri;
  int batchsize;
  mutable int currentbatch;
};

class FaceCollectionTriWithObjects : public FaceCollectionTri
{
public:
  virtual int NumObjects() const=0;
  virtual int PointsStartRange(int obj) const=0;
  virtual int PointsEndRange(int obj) const=0;
  virtual int IndexStartRange(int obj) const=0;
  virtual int IndexEndRange(int obj) const=0;
};


class ObjectArray2 : public FaceCollectionTriWithObjects
{
public:
  ObjectArray2(Array<int, FaceCollectionTri*> &arr) : arr(arr) { }
  int NumObjects() const { return arr.Size(); }
  virtual int PointsStartRange(int obj) const
  {
    int count = 0;
    int ii = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumPoints();
	ii++;
	if (ii > obj) { count = oldcount; break; }
      }
    return count;
  }
  virtual int PointsEndRange(int obj) const
  {
    return PointsStartRange(obj) + arr.Index(obj)->NumPoints();
  }
  virtual int IndexStartRange(int obj) const
  {
    int count = 0;
    int ii = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	ii++;
	if (ii > obj) { count = oldcount; break; }
      }
    return count;
  }

  virtual int IndexEndRange(int obj) const
  {
    return IndexStartRange(obj) + arr.Index(obj)->IndexCount();
  }

  virtual int NumFaces() const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	count += arr.Index(i)->NumFaces();
      }
    return count;

  }
  virtual int NumPoints() const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	count += arr.Index(i)->NumPoints();
      }
    return count;
  }
  virtual Point FacePoint(int index) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) 
	  { 
	    Point p = arr.Index(i)->FacePoint(index-oldcount); 
	    //std::cout << index << " " << p << std::endl;
	    return p;
	  }
      }
    Point b;
    return b;
  }
  virtual unsigned int Color(int index) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) { return arr.Index(i)->Color(index-oldcount); }
      }
    std::cout << "ObjectArray2::0" << std::endl;
    return 0;
  }

  virtual Point2d TexCoord(int index) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) { return arr.Index(i)->TexCoord(index-oldcount); }
      }
    std::cout << "ObjectArray2::0" << std::endl;
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }


  virtual Vector PointNormal(int index) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) { return arr.Index(i)->PointNormal(index-oldcount); }
      }
    Vector b;
    return b;

  }
  virtual float Attrib(int index, int id) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) { return arr.Index(i)->Attrib(index-oldcount, id); }
      }
    float b=0.0;
    return b;

  }
  virtual int AttribI(int index, int id) const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->IndexCount();
	if (count > index) { return arr.Index(i)->AttribI(index-oldcount, id); }
      }
    float b=0.0;
    return b;

  }
  virtual int IndexCount() const
  {
    int count = 0;
    int num = arr.Size();
   for(int i=0;i<num;i++)
      {
	count += arr.Index(i)->IndexCount();
      }
    return count;
  }
  virtual int IndexToFace(int index) const
  {
    int count = 0;
    int facecount = 0;
    int oldfacecount = 0;
    int result = 0;
    int num = arr.Size();
    int i = 0;
    for(;i<num;i++)
      {
	int oldcount = count;
	oldfacecount = facecount;
	count += arr.Index(i)->IndexCount();
	facecount += arr.Index(i)->NumFaces();
	if (count > index) { result = index-oldcount; break; }
      }
    return oldfacecount + arr.Index(i)->IndexToFace(result);
  }
  virtual int Index(int face, int point) const
  {
    int result =0;
    int count = 0;
    int i = 0;
    int obj=0;
    int num = arr.Size();
    for(;i<num;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
		if (count > face) { result = face-oldcount; break; }
	obj++;
      }
    int ii = arr.Index(i)->Index(result, point);
    return IndexStartRange(obj) + ii;
  }
private:
  Array<int,FaceCollectionTri*> &arr;
};

class ObjectArray : public FaceCollectionTriWithObjects
{
public:
  // array of const FaceCollectionTri*
  template<class It>
  ObjectArray(It beg, It end) : vec(beg, end) {}

  virtual int NumObjects() const { return vec.size(); }
  virtual int PointsStartRange(int obj) const
  {
    int count = 0;
    int ii = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumPoints();
	ii++;
	if (ii > obj) { count = oldcount; break; }
      }
    return count;
  }
  virtual int PointsEndRange(int obj) const
  {
    return PointsStartRange(obj) + vec[obj]->NumPoints();
  }
  virtual int IndexStartRange(int obj) const
  {
    int count = 0;
    int ii = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	ii++;
	if (ii > obj) { count = oldcount; break; }
      }
    return count;
  }

  virtual int IndexEndRange(int obj) const
  {
    return IndexStartRange(obj) + vec[obj]->IndexCount();
  }

  virtual int NumFaces() const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	count += (*i)->NumFaces();
      }
    return count;

  }
  virtual int NumPoints() const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	count += (*i)->NumPoints();
      }
    return count;
  }
  virtual Point FacePoint(int index) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->FacePoint(index-oldcount); }
      }
    Point b;
    return b;
  }

  virtual unsigned int Color(int index) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->Color(index-oldcount); }
      }
    std::cout << "ObjectArray::0" << std::endl;

    return 0;
  }

  virtual Point2d TexCoord(int index) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->TexCoord(index-oldcount); }
      }
    std::cout << "ObjectArray::0" << std::endl;

    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }


  virtual Vector PointNormal(int index) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->PointNormal(index-oldcount); }
      }
    Vector b;
    return b;

  }
  virtual float Attrib(int index, int id) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->Attrib(index-oldcount, id); }
      }
    float b=0.0;
    return b;

  }
  virtual int AttribI(int index, int id) const
  {
    int count = 0;
    for(std::vector<const FaceCollectionTri*>::const_iterator i=vec.begin(); i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->IndexCount();
	if (count > index) { return (*i)->AttribI(index-oldcount, id); }
      }
    int b=0;
    return b;

  }

  virtual int IndexCount() const
  {
    return 0;
  }
  virtual int IndexToFace(int index) const
  {
    int count = 0;
    int facecount = 0;
    int oldfacecount = 0;
    int result = 0;
    std::vector<const FaceCollectionTri*>::const_iterator i = vec.begin();
    for(; i!=vec.end();i++)
      {
	int oldcount = count;
	oldfacecount = facecount;
	count += (*i)->IndexCount();
	facecount += (*i)->NumFaces();
	if (count > index) { result = index-oldcount; break; }
      }
    return oldfacecount + (*i)->IndexToFace(result);
  }
  virtual int Index(int face, int point) const
  {
    int result =0;
    int count = 0;
    std::vector<const FaceCollectionTri*>::const_iterator i = vec.begin();
    int obj=0;
    for(; i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
		if (count > face) { result = face-oldcount; break; }
	obj++;
      }
    int ii = (*i)->Index(result, point);
    return IndexStartRange(obj) + ii;
  }
private:
  std::vector<const FaceCollectionTri*> vec;
};

class TrisToTris : public FaceCollectionTri
{
public:
  TrisToTris(const FaceCollection &coll) : coll(coll) { }
  virtual int NumFaces() const
  {
    return coll.NumFaces();
  }
  virtual int NumPoints() const
  {
    return coll.NumFaces()*3;
  }
  virtual Point FacePoint(int index) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.FacePoint(face, point);
  }
  virtual unsigned int Color(int index) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.Color(face, point);
  }
  virtual Point2d TexCoord(int index) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.TexCoord(face, point);
  }

  virtual Vector PointNormal(int index) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.PointNormal(face, point);
  }
  virtual float Attrib(int index, int id) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.Attrib(face, point, id);
  }
  virtual int AttribI(int index, int id) const
  {
    int face = index / 3;
    int point = index - face*3;
    return coll.AttribI(face, point, id);
  }
  virtual int Index(int face, int point) const
  {
    return face*3+point;
  }
  virtual int IndexCount() const
  {
    return coll.NumFaces();
  }
  virtual int IndexToFace(int index) const
  {
    //return index;
    return index/3;
  }
private:
  const FaceCollection &coll;

};
typedef FunctionImpl0<FaceCollection*, FaceCollectionTri*, TrisToTris> TrisToTrisFunction;
class QuadsToPlanes : public Array<int, Plane>
{
public:
  QuadsToPlanes(const FaceCollection &coll) : coll(coll) { }
  Plane Index(int i) const
  {
    Point p1 = coll.FacePoint(i, 0);
    Point p2 = coll.FacePoint(i, 1);
    Point p3 = coll.FacePoint(i, 2);
    Point p4 = coll.FacePoint(i, 3);
    Point avg = Vector(p1+p2+p3+p4)/4;
    return Plane(avg, Vector(p2-p1)/2, Vector(p4-p1)/2);
  }
  int Size() const { return coll.NumFaces(); }
private:
  const FaceCollection &coll;
};

class QuadsToTris : public FaceCollectionTri
{ // assumes that NumPoints(face) is 4
public:
  QuadsToTris(const FaceCollection &coll) : coll(coll) { }
  virtual int NumFaces() const
  {
    return coll.NumFaces()*2;
  }
  virtual int NumPoints() const
  {
    return coll.NumFaces()*4;
  }
  virtual Point FacePoint(int index) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.FacePoint(face, point);
  }
  virtual Vector PointNormal(int index) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.PointNormal(face, point);
  }
  virtual unsigned int Color(int index) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.Color(face, point);
  }

  virtual Point2d TexCoord(int index) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.TexCoord(face, point);
  }

  virtual float Attrib(int index, int id) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.Attrib(face, point, id);
  }
  virtual int AttribI(int index, int id) const
  {
    int face = index >> 2;
    int point = index & 0x3;
    return coll.AttribI(face, point, id);
  }
  virtual int Index(int face, int point) const
  {
    int face1 = face >> 1;
    int face2 = face &0x1;
    int offset = face2*2 + face1*4;
    if (face2==1 && point==2)
      {
	int i = offset-face2*2;
	//std::cout << "Indexes2: " << face << " " << point << ":" << i << std::endl;
	return i;
      }
    int i = offset+point;
    //std::cout << "Indexes: " << face << " " << point << ":" << i << std::endl;
    return i;
  }
  virtual int IndexCount() const
  {
    //return 3*NumFaces();
    //return 6*coll.NumFaces();
    return 4*coll.NumFaces();
  }
  virtual int IndexToFace(int index) const
  {
    //return index/3;
    //return index;
    return index/2;
  }
private:
  const FaceCollection &coll;
};
typedef FunctionImpl0<FaceCollection*, FaceCollectionTri*, QuadsToTris> QuadsToTrisFunction;

class PointCollectionArray : public PointCollection
{
public:
  template<class It>
  PointCollectionArray(It beg, It end) : vec(beg,end) { }
  virtual int Size() const { return vec.size(); }
  virtual Point Index(int index) const { return vec[index]; }
private:
  std::vector<Point> vec;
};

class StarCurve : public PointCollection
{
public:
  StarCurve(float r1, float r2, int numpoints) : num(numpoints), r1(r1), r2(r2) {}
  int Size() const { return num*2; }
  Point Index(int i) const
  {
    int ii = i % 2;
    float r=0.0;
    if (ii==0) r=r1;
    if (ii==1) r=r2;
    
    float angle = 2.0*3.14159*float(i/2)/float(num);
    Point p;
    p.x = r*cos(angle);
    p.y = r*sin(angle);
    p.z = 0.0;
    return p;
  }
private:
  int num;
  float r1,r2;
};

class SampleCurve3d : public PointCollection
{
public:
  SampleCurve3d(const CurveIn3d &curve, int count) : curve(curve), arr(c), c(count, curve.Size()) { }
  SampleCurve3d(const CurveIn3d *curve, int count) : curve(*curve), arr(c), c(count, curve->Size()) { }
  SampleCurve3d(const CurveIn3d &curve, const FloatArray &arr) : curve(curve), arr(arr), c(1,0) { }
  int Size() const { return arr.NumFloats(); }
  Point Index(int index) const
  {
    //float length = curve.Length();
    //length /= float(count);
    //length *= float(index);
    float length = arr.Index(index);
    return curve.Index(length);
  }
private:
  const CurveIn3d &curve;
  //int count;
  const FloatArray &arr;
  ConstantSampling c;
};


typedef FunctionImpl1<CurveIn3d*, PointCollection*, int, SampleCurve3d> SampleCurve3dFunction;

class SampledLine : public PointCollection
{
public:
  SampledLine(Point p1, Point p2, int count) : line(p1,p2), sample(line, count) { }
  int Size() const { return sample.Size(); }
  Point Index(int i) const { return sample.Index(i); }
private:
  LineIn3d line;
  SampleCurve3d sample;
};


class Direction : public Element<Matrix>
{
public:
  Direction(const std::pair<Point, Point> &p) : p(p) { }
  Matrix Elem() const
  {
    Vector v = p.second - p.first;
    //std::cout << p.first << " " << p.second << " " << v << std::endl;
    Vector vx(1.0,0.0,0.0);
    Vector vy(0.0,1.0,0.0);
    Vector vz(0.0,0.0,1.0);
    float anglex = Vector::Angle(v,vx);
    float angley = Vector::Angle(v,vy);
    float anglez = Vector::Angle(v,vz);
    //std::cout << anglex << " " << angley << " " << anglez << std::endl;
    return Matrix::XRotation(anglex)*Matrix::YRotation(angley)*Matrix::ZRotation(anglez)*Matrix::Translate(p.first.x,p.first.y,p.first.z);
  }
private:
  std::pair<Point, Point> p; // need to take copy
};
typedef FunctionImpl0<std::pair<Point,Point>,Element<Matrix>*, Direction> DirectionFunction;

class CurveMovement : public MatrixCurve
{
public:
  CurveMovement(const CurveIn3d &curve) : curve(curve) { }
  float Size() const { return curve.Size(); }
  Matrix Index(float x) const
  {
    Point p = curve.Index(x);
    Point p2 = curve.Index(x+0.01);
    Direction d(std::make_pair(p,p2));
    return d.Elem();
  }
private:
  const CurveIn3d &curve;
};
typedef FunctionImpl0<CurveIn3d*, MatrixCurve*, CurveMovement> CurveMovementFunction;

class FlipNormals : public FaceCollection
{
public:
  FlipNormals(FaceCollection &coll) : coll(coll) { }
  int NumFaces() const { return coll.NumFaces(); }
  int NumPoints(int face) const { return coll.NumPoints(face); }
  Point FacePoint(int face, int point) const { return coll.FacePoint(face,point); }
  Vector PointNormal(int face, int point) const {
    return -coll.PointNormal(face,point);
  }
  float Attrib(int face, int point, int id) const { return coll.Attrib(face,point,id); }
  int AttribI(int face, int point, int id) const { return coll.AttribI(face,point,id); }
private:
  FaceCollection &coll;
};
typedef FunctionImpl0<FaceCollection*, FaceCollection*, FlipNormals> FlipNormalsFunction;

class LineCollection
{
public:
  virtual ~LineCollection() { }
  virtual int NumLines() const =0;
  virtual Point LinePoint(int line, int point) const = 0;
};


class ContinuousLines : public LineCollection
{
public:
  ContinuousLines(PointCollection &p, bool loops) : p(p), loops(loops) { }
  ContinuousLines(PointCollection *p, bool loops) : p(*p), loops(loops) { }
  int NumLines() const { return loops?p.NumPoints():p.NumPoints()-1; }
  Point LinePoint(int line, int point) const
  {
    int num = line+point;
    num %= p.NumPoints();
    return p.Points(num);
  }
private:
  PointCollection &p;
  bool loops;
};
typedef FunctionImpl1<PointCollection*, LineCollection*, bool, ContinuousLines> ContinuousLinesFunction;

class OutlineFaces : public LineCollection
{
public:
  OutlineFaces(FaceCollection &c) : c(c) { }
  int NumLines() const
  {
    int num = c.NumFaces();
    int count = 0;
    for(int i=0;i<num;i++)
      count += c.NumPoints(i)+1;
    return count;
  }
  Point LinePoint(int line, int point) const
  {
    int num = c.NumFaces();
    int count = 0;
    int i=0;
    for(;i<num;i++)
      {
      count += c.NumPoints(i)+1;
      if (count >= line) break;
      }
    int p = count - line + point;
    int pp = p % c.NumPoints(i);
    return c.FacePoint(i, pp);
    
  }
private:
  FaceCollection &c;

};
typedef FunctionImpl0<FaceCollection*, LineCollection*, OutlineFaces> OutlineFacesFunction;

class InverseOutlineFaces : public ForwardFaceCollection
{
public:
  InverseOutlineFaces(FaceCollection &previous, LineCollection &coll) : ForwardFaceCollection(previous), previous(previous), coll(coll) { }
  Point FacePoint(int face, int point) const
  {
    int num2 = 0;
    int faces = previous.NumFaces();
    for(int i=0;i<faces;i++)
      {
	if (i>=face) break;
	num2+=previous.NumPoints(i)+1;
      }
    return coll.LinePoint(num2+point, 0);
  }
private:
  FaceCollection &previous;
  LineCollection &coll;
};

class FaceCenterPoints : public PointCollection
{
public:
  FaceCenterPoints(FaceCollection &faces) : faces(faces) { }
  int Size() const { return faces.NumFaces(); }
  Point Index(int face) const { 
    int count = faces.NumPoints(face);
    Vector sum;
    for(int i=0;i<count;i++)
      {
	Vector p = faces.FacePoint(face, i);
	sum+=p;
      }
    sum*=1.0/float(count);
    //std::cout << face << ":" << sum.dx << " " << sum.dy << " " << sum.dz << std::endl;
    return sum;
  }
private:
  FaceCollection &faces;
};
typedef FunctionImpl0<FaceCollection*, PointCollection*, FaceCenterPoints> FaceCenterPointsFunction;

class FaceNormals : public VectorCollection
{
public:
  FaceNormals(FaceCollection &faces) : faces(faces) { }
  int Size() const { return faces.NumFaces(); }
  Vector Index(int face) const {
    int count = faces.NumPoints(face);
    Vector sum;
    for(int i=0;i<count;i++)
      {
	Vector p = faces.PointNormal(face,i);
	sum += p;
      }
    sum*=1.0/float(count);
    return sum;
  }
private:
  FaceCollection &faces;
};
class AdvanceVector : public Element<std::pair<Point, Vector> >
{
public:
  AdvanceVector(const std::pair<Point,Vector> &v, float val) : v(v), val(val) { }
  std::pair<Point,Vector> Elem() const
  {
    return std::make_pair(v.first + val*v.second, v.second);
  }
private:
  std::pair<Point,Vector> v;
  float val;
};
typedef FunctionImpl1<std::pair<Point,Vector>, Element<std::pair<Point,Vector> >*, float , AdvanceVector> AdvanceVectorFunction;

class VectorPoints : public Element<std::pair<Point,Point> >
{
public:
  VectorPoints(const std::pair<Point, Vector> &vec) : vec(vec) { }
  std::pair<Point, Point> Elem() const
  {
    //std::cout << "VectorPoints: " << vec.first << " " << vec.second << std::endl;
    return std::make_pair(vec.first, vec.first+vec.second);
  }
private:
  std::pair<Point, Vector> vec; // need to take copy of the values.
};
typedef FunctionImpl0<std::pair<Point,Vector>, Element<std::pair<Point,Point> >*, VectorPoints> VectorPointsFunction;

class Fur : public LineCollection
{
public:
  Fur(FaceCollection &face, int numlines, int numlines2, float length) : face(face), numlines(numlines), numlines2(numlines2), length(length) { }
  int NumLines() const { return face.NumFaces()*numlines*numlines2; }
  Point LinePoint(int line, int point) const
  {
    int zz = line / (numlines*numlines2);
    int ddz = line - zz * (numlines*numlines2);
    int yy = ddz / numlines;
    int ddy = ddz - yy*numlines;
    int xx = ddy;
    float yyy = float(yy)/numlines2;
    float xxx = float(xx)/numlines;
 
    //int face = line / numlines;
    //int line2 = line - face*numlines;
    Point p1 = face.FacePoint(zz, 0);
    Point p2 = face.FacePoint(zz, 1);
    Point p3 = face.FacePoint(zz, 2);
    Vector v1 = p2-p1;
    Vector v2 = p3-p2;
    
    if (point == 0)
      {
	return p1+v1*yyy+xxx*v2;
      }
    else
      {
	return p1+v1*yyy+v2*xxx+face.PointNormal(zz,0)*length;

      }
  }
private:
  FaceCollection &face;
  int numlines, numlines2;
  float length;
};

class TextureCoords
{
public:
  virtual ~TextureCoords() { }
  virtual int Texture(int face) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
};

class FaceCollectionTextureCoords : public TextureCoords
{
public:
  FaceCollectionTextureCoords(const FaceCollection &coll) : coll(coll) { }
  int Texture(int face) const
  {
    return coll.FaceTexture(face);
  }
  Point2d TexCoord(int face, int point) const { return coll.TexCoord(face,point); }
private:
  const FaceCollection &coll;
};

class SphericalTexCoords : public TextureCoords
{
public:
  SphericalTexCoords(FaceCollection &coll, int texture, Point center) : coll(coll), texture(texture), center(center) { }

  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    Point p = coll.FacePoint(face,point);
    Vector v = p-Vector(center);
    float alfa = atan2(sqrt(v.dx*v.dx+v.dy*v.dy),v.dz);
    float beta = atan2(v.dy,v.dx);
    Point2d res;
    res.x = alfa/3.14159/2.0;
    res.y = beta/3.14159/2.0;
    return res;
  }
private:
  FaceCollection &coll;
  int texture;
  Point center;
};
typedef FunctionImpl2<FaceCollection*, TextureCoords*, int, Point, SphericalTexCoords> SphericalTexCoordsFunction;

class PlaneTexCoords : public TextureCoords
{
public:
  PlaneTexCoords(FaceCollection &coll, int texture, Point center, Vector x, Vector y) : coll(coll), texture(texture), center(center), x(x), y(y) { }
  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    Point p = coll.FacePoint(face,point);
    Vector v = p-Vector(center);
    float projx = Vector::DotProduct(v,x)/x.Dist();
    float projy = Vector::DotProduct(v,y)/y.Dist();
    Point2d res;
    res.x = projx;
    res.y = projy;
    return res;
  }

private:
  FaceCollection &coll;
  int texture;
  Point center;
  Vector x,y;
};

class HorizTexCoords : public TextureCoords
{
public:
  HorizTexCoords(FaceCollection &coll, int texture) : coll(coll), texture(texture) { }
  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    int numfaces = coll.NumFaces();
    float dx = 1.0/numfaces;
    float x = face*dx;
    float xx=0.0,yy=0.0;
    float dy = 1.0;
    switch(point)
      {
      case 3: xx=0.0; yy=0.0; break;
      case 0: xx=0.0; yy=dy; break;
      case 1: xx=dx; yy=dy; break;
      case 2: xx=dx; yy=0.0; break;
      };
    Point2d p;
    p.x = x+xx;
    p.y = yy;
    return p;
  }
private:
  FaceCollection &coll;
  int texture;
};
typedef FunctionImpl1<FaceCollection*, TextureCoords*, int, HorizTexCoords> HorixTexCoordsFunction;

class SimpleTexCoords : public TextureCoords
{
public:
  SimpleTexCoords(FaceCollection &coll, int texture) : coll(coll), texture(texture) { }
  void SetTexture(int tex) { texture=tex; }
  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    int count = coll.NumPoints(face);
    float angle = 45.0 /*/2.0*/+point*360.0/count;
    Point2d d;
    d.x = 0.5+0.5*  cos(angle*3.14159*2.0/360.0);
    d.y = 0.5+0.5*  sin(angle*3.14159*2.0/360.0);
    return d;
  }
private:
  FaceCollection &coll;
  int texture;
};
typedef FunctionImpl1<FaceCollection*, TextureCoords*, int, SimpleTexCoords> SimpleTexCoordsFunction;

class SpaceTexCoords : public TextureCoords
{
public:
  SpaceTexCoords(FaceCollection &coll, int texture, Point p, Vector x, Vector y) : coll(coll), texture(texture), p(p), x(x), y(y) { }
  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    Point c = coll.FacePoint(face, point);
    Vector v = c-p;
    float d1 = Vector::DotProduct(v, x);
    float d2 = Vector::DotProduct(v, y);
    //d1/= v.Dist();
    d1/= x.Dist();
    d1/= x.Dist();
    //d2/= v.Dist();
    d2/= y.Dist();
    d2/= y.Dist();
    Point2d res;
    res.x = d1;
    res.y = d2;
    return res;
  }
private:
  FaceCollection &coll;
  int texture;
  Point p;
  Vector x,y;
};


class ElementCollection
{
public:
  virtual ~ElementCollection() { }
  virtual int NumElements() const = 0;
  virtual void SetElemId(int id)=0;
};

class ColorElement : public ElementCollection
{
public:
  template<class It> 
  ColorElement(It beg, It end, ElementCollection &ec) : cont(beg, end), ec(ec) { }
  
  int NumElements() const { return ec.NumElements(); }
  void SetElemId(int id);
private:
  std::vector<Color2> cont;
  ElementCollection &ec;
};
class Portal
{
public:
  virtual ~Portal(){ }
  virtual Matrix PortalMatrix() const=0;
  virtual float Width() const=0;
  virtual float Height() const=0;
};


/*
class Room
{
public:
  virtual ~Room() { }
  virtual Matrix RoomMatrix() const=0;
};
*/

class Waveform : public Function<float, float>
{
public:
  virtual ~Waveform() { }
  virtual float Length() const=0;
  virtual float Min() const = 0;
  virtual float Max() const = 0;
  virtual float Index(float val) const = 0;
};
class ZeroWaveform : public Waveform
{
public:
  ZeroWaveform(float length_x, float min_y, float max_y) : length(length_x), min(min_y), max(max_y) { }
  float Length() const { return length; }
  float Min() const { return min; }
  float Max() const { return max; }
  float Index(float val) const { return 0.0; }
private:
  float length, min, max;
};

class WaveformPart
{
public:
  virtual float Delta(float x) const=0;
};
class ConstantWaveformPart : public WaveformPart
{
public:
  ConstantWaveformPart(float start_x, float end_x, float value) : start_x(start_x), end_x(end_x), value(value) { }
  float Delta(float x) const
  {
    if (x < start_x) return 0.0;
    if (x >= end_x) return 0.0;
    return value;
  }
private:
  float start_x, end_x, value;
};

class LinWaveformPart : public WaveformPart
{
public:
  LinWaveformPart(float start_x, float end_x, float delta_y) : start_x(start_x), end_x(end_x), delta_y(delta_y) { }
  float Delta(float x) const
  {
    if (x < start_x) return 0.0;
    if (x > end_x) return delta_y;
    return (x-start_x)/(end_x-start_x)*delta_y;
  }
private:
  float start_x, end_x, delta_y;
};

class ArrayWaveform : public Array<int, WaveformPart*>, private WaveformPart
{
public:
  ArrayWaveform(float *array, int size, float samplelength) : array(array), size(size), samplelength(samplelength), part(0) { }
  int Size() const { return size; }
  WaveformPart *Index(int i) const
  {
    delete part;
    part = new ConstantWaveformPart(samplelength*i, samplelength*(i+1), array[i]);
    return part;
  }
  
private:
  float *array;
  int size;
  float samplelength;
  mutable ConstantWaveformPart *part;
};
class ArrayWaveform2 : public Waveform
{
public:
  ArrayWaveform2(float *array, int size, float samplelength, float m_min, float m_max) : array(array), size(size), samplelength(samplelength), m_min(m_min), m_max(m_max) { }
  virtual float Length() const { return size*samplelength; }
  virtual float Min() const { return m_min; }
  virtual float Max() const { return m_max; }
  virtual float Index(float val) const 
  {
    val /= samplelength;
    int val2 = val;
    if (val >= 0 && val < size)
      {
	return array[val2];
      }
    else
      {
	return 0.0;
      }
  }

private:
  float *array;
  int size;
  float samplelength;
  float m_min, m_max;
};


class PartWaveform : public Waveform
{
public:
  PartWaveform(Waveform &base, Array<int, WaveformPart*> &arr) : base(base), arr(arr) { }
  float Length() const { return base.Length(); }
  float Min() const { return base.Min(); }
  float Max() const { return base.Max(); }
  float Index(float val) const
  {
    float val2 = base.Index(val);
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	val+=arr.Index(i)->Delta(val2);
      }
    return val;
  }
private:
  Waveform &base;
  Array<int, WaveformPart*> &arr;
};

class IncreasingWaveform : public Waveform
{
public:
  float Length() const { return 1.0; }
  float Min() const { return 0.0; }
  float Max() const { return 1.0; }
  float Index(float val) const { return val; }
};

class SampleWaveform : public FloatArray
{
public:
  SampleWaveform(Waveform &wave, int count) : wave(wave), count(count) { }
  int Size() const { return count; }
  float Index(int i) const
  {
    return wave.Index(Sample(i));
  }
  float Sample(int i) const
  {
    return float(i)*wave.Length()/float(count);
  }
private:
  Waveform &wave;
  int count;
};
typedef FunctionImpl1<Waveform*, FloatArray*, int, SampleWaveform> SampleWaveformFunction;


class FloatArrayToPointCollection2d : public PointCollection2d
{
public:
  FloatArrayToPointCollection2d(FloatArray &array, Point2d p, Vector2d v_x, Vector2d v_y) : array(array), p(p), v_x(v_x), v_y(v_y) { }
  int Size() const { return array.Size(); }
  Point2d Index(int i) const
  {
    Point2d pp;
    pp.x = p.x + v_x.dx*i/Size() + v_y.dx*array.Index(i);
    pp.y = p.y + v_x.dy*i/Size() + v_y.dy*array.Index(i);
    return pp;
  }
private:
  FloatArray &array;
  Point2d p;
  Vector2d v_x, v_y;
};


class WaveformArray : public Array<int, Waveform*>
{
};
class WaveformCurve : public Curve<Waveform*>
{
};
class WaveformFunction : public Array2<float, float, float>
{
public:
  WaveformFunction(WaveformCurve &curve) : curve(curve) { }
  float Index(float x, float y)
  {
    Waveform *wave = curve.Index(x);
    float val = wave->Index(y);
    return val;
  }
  float SizeX() const { return curve.Size(); }
  float SizeY() const { return curve.Index(0.0)->Length(); }
private:
  WaveformCurve &curve;
};


class HeightMapSurface : public SurfaceIn3d
{
public:
  HeightMapSurface(Array2<float, float, float> &array, Point p, UnitVectors u) : array(array), p(p), u(u) { }
  virtual Point Index(float x, float y) const 
  {
    float val = array.Index(x,y);
    return p + x*u.u_x/array.SizeX() + y*u.u_y/array.SizeY() + val*u.u_z;
  }
  virtual Point2d Texture(float x, float y) const { Point2d p; p.x=x; p.y=y; return p; }
  virtual float Attrib(float x, float y, int id) const { return 0.0; }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float xx, float yy) const 
  {
    Point p = Index(xx,yy);
    Point p2 = Index(xx+0.01,yy);
    Point p3 = Index(xx,yy+0.01);
    return -Vector::CrossProduct(p2-p, p3-p);
  }
  virtual float XSize() const { return array.SizeX(); }
  virtual float YSize() const { return array.SizeY(); }
private:
  Array2<float, float,float> &array;
  Point p;
  UnitVectors u;
};

class RepeatWaveform : public Waveform
{
public:
  RepeatWaveform(Waveform &w, int times) : w(w), times(times) { }
  float Length() const { return w.Length()*times; }
  float Min() const { return w.Min(); }
  float Max() const { return w.Max(); }
  float Index(float val) const 
  { 
    while(val>=w.Length()) val-=w.Length(); 
    return w.Index(val);
  }

private:
  Waveform &w;
  int times;
};
typedef FunctionImpl1<Waveform*, Waveform*, int, RepeatWaveform> RepeatWaveformFunction;

class SinWaveform : public Waveform
{
public:
  float Length() const { return 2.0*3.14159; }
  float Min() const { return -1.0; }
  float Max() const { return 1.0; }
  float Index(float val) const { return sinf(val); }
};
typedef FunctionImplT0<Waveform*, SinWaveform> SinWaveformFunction;

class FitWaveform : public Waveform
{
public:
  FitWaveform(Waveform &v, float x, float ymin, float ymax)
    : x(x), ymin(ymin), ymax(ymax), v(v)
  {
  }
  float Min() const { return ymin; }
  float Max() const { return ymax; }
  float Length() const { return x; }
  float Index(float xx) const { return (v.Index(xx/x*v.Length())-v.Min())/(v.Max()-v.Min())*(ymax-ymin)+ymin; }
public:
  float x,ymin,ymax;
  Waveform &v;
};
typedef FunctionImpl3<Waveform*, Waveform*, float, float ,float, FitWaveform> FitWaveformFunction;

class ConstantWaveform : public Waveform
{
public:
  ConstantWaveform(float val, float length=1000.0) : val(val), length(length) { }
  float Min() const { return val; }
  float Max() const { return val; }
  float Length() const { return length; }
  float Index(float xx) const { return val; }
private:
  float val, length; 
};
typedef FunctionImplT2<Waveform*, float, float, ConstantWaveform> ConstantWaveformFunction;

class InterpolateWaveform : public Waveform
{
public:
  template<class It>
  InterpolateWaveform(It beg, It end, float length, float ymin, float ymax) : 
    length(length), ymin(ymin), ymax(ymax), cont(beg, end) 
  { }
  float Min() const { return ymin; }
  float Max() const { return ymax; }
  float Length() const { return length; }
  float Index(float time) const;
private:
  float length, ymin, ymax;
  std::vector<Point2d> cont;
};
class SawWaveform : public InterpolateWaveform
{
public:
  SawWaveform() : InterpolateWaveform(array, array+3, 1.0, 0.0, 1.0) { }
private:
  static Point2d array[3];
};
typedef FunctionImplT0<Waveform*, SawWaveform> SawWaveformFunction;

class Box
{
public:
  Point BoxPoint(float x, float y, float z) const;

  Point center;
  Vector u_x, u_y, u_z;
};

class BoxCollection : public Array<int, Matrix>
{
public:
  virtual ~BoxCollection() { }

  int NumBoxes() const { return Size(); }
  Matrix BoxIndex(int box) const { return Index(box); }
};
class BoxCollectionConvert : public BoxCollection
{
public:
  BoxCollectionConvert(const Array<int, Matrix> &c2) : c2(c2) { }
  int Size() const 
  {
    int size = c2.Size();
    return size; 
  }
  Matrix Index(int i) const { return c2.Index(i); }
private:
  const Array<int,Matrix> &c2;
};
typedef FunctionImpl0<Array<int,Matrix>, BoxCollection*, BoxCollectionConvert> BoxCollectionConvertFunction;

class FacesToBoxCollection : public BoxCollection
{
public:
  FacesToBoxCollection(FaceCollection &coll, float dist) : facepoints(coll), normals(coll), combined(facepoints, normals), advance(dist), ch(combined, advance, advance_elem, func, func_elem, dir_func, dir_func_elem), boxes(ch) { }
  int Size() const { return boxes.Size(); }
  Matrix Index(int i) const { return boxes.Index(i); }
  
private:
  FaceCenterPoints facepoints;
  FaceNormals normals;
  ArrayPair<int, Point, Vector> combined;
  AdvanceVectorFunction advance;
  ElementFunction<std::pair<Point, Vector> > advance_elem;
  VectorPointsFunction func;
  ElementFunction<std::pair<Point, Point> > func_elem;
  DirectionFunction dir_func;
  ElementFunction<Matrix> dir_func_elem;
  ComposeChain<int, Matrix> ch;
  BoxCollectionConvert boxes;
};
typedef FunctionImpl1<FaceCollection*, BoxCollection*, float, FacesToBoxCollection> FacesToBoxCollectionFunction;

class ObjectCollection
{
public:
  virtual ~ObjectCollection() { }
  virtual int NumObjects() const = 0;
  virtual int BoxObject(int obj) const=0;
};
class LinearObjectCollection : public ObjectCollection
{
public:
  LinearObjectCollection(int count) : count(count) { }
  int NumObjects() const { return count; }
  int BoxObject(int obj) const { return obj; }
private:
  int count;
};
class ConstantObjectCollection : public ObjectCollection
{
public:
  ConstantObjectCollection(int count, int object) : count(count), object(object) { }
  int NumObjects() const { return count; }
  int BoxObject(int /*obj*/) const { return object; }
private:
  int count;
  int object;
};

class BoxCollectionWithObject : public Array<int, std::pair<Matrix, int> >
{
public:
  virtual ~BoxCollectionWithObject() { }

  int Size() const { return NumBoxes(); }
  std::pair<Matrix, int> Index(int box) const { return std::make_pair(BoxIndex(box), BoxObject(box)); }

  virtual int NumBoxes() const = 0;
  virtual Matrix BoxIndex(int box) const=0;
  virtual int BoxObject(int box) const=0;
};

class ShaderParameterArray : public Array<int, ShaderParameters*>
{
};
class ShaderParameterArrayConvert : public ShaderParameterArray
{
public:
  ShaderParameterArrayConvert(const Array<int, ShaderParameters*> &c2) : c2(c2) { }
  int Size() const { return c2.Size(); }
  ShaderParameters* Index(int i) const { return c2.Index(i); }
private:
  const Array<int,ShaderParameters*> &c2;
};

class CombineParameterArrays : public ShaderParameterArray
{
public:
  CombineParameterArrays(ShaderParameterArray &arr, ShaderParameterArray &arr2) : arr(arr), arr2(arr2) { }
  int Size() const { return std::min(arr.Size(), arr2.Size()); }
  ShaderParameters* Index(int i) const
  {
    combine.SetValues(arr.Index(i), arr2.Index(i));
    return &combine;
  }
private:
  mutable CombineParameters combine;
  ShaderParameterArray &arr;
  ShaderParameterArray &arr2;
};

class BlobParameterArray : public ShaderParameterArray
{
public:
  BlobParameterArray(PointCollection &center_points, PointCollection &center2_points, FloatArray &radius) : center_points(center_points), center2_points(center2_points), p(0), r(radius) {}
  ~BlobParameterArray() { delete p; }
  int Size() const { return std::min(std::min(center_points.Size(), center2_points.Size()), r.Size()); }
  ShaderParameters *Index(int i) const
  {
    Point center = center_points.Index(i);
    Point center2 = center2_points.Index(i);
    float rr = r.Index(i);
    if (!p) { p = new BlobParameters(center, center2,rr); }
    p->set_points(center, center2,rr);
    return p;
  }
private:
  PointCollection &center_points;
  PointCollection &center2_points;
  mutable BlobParameters *p;
  FloatArray &r;
};

struct ObjectData
{
  Matrix matrix;
  int object;
  ShaderParameters *params;
  // to be extended if needed for RenderOpenGlObjects to use new data
  friend std::ostream &operator<<(std::ostream &o, const ObjectData &od);
};
class ObjectDataArray : public Array<int, ObjectData>
{
public:
};
class CombineObjectData : public ObjectDataArray
{
public:
  CombineObjectData(BoxCollection &box, ObjectCollection &obj, ShaderParameterArray &params) : box(box), obj(obj), params(params) { }
  int Size() const { return std::min(std::min(box.NumBoxes(), obj.NumObjects()), params.Size()); }
  ObjectData Index(int i) const
  {
    Matrix m = box.BoxIndex(i);
    int o = obj.BoxObject(i);
    ShaderParameters *p = params.Index(i);
    ObjectData d;
    d.matrix = m;
    d.object = o;
    d.params = p;
    return d;
  }
private:
  BoxCollection &box;
  ObjectCollection &obj;
  ShaderParameterArray &params;
};

class CombineBoxesAndObjects : public BoxCollectionWithObject
{
public:
  CombineBoxesAndObjects(BoxCollection &box, ObjectCollection &obj) : box(box), obj(obj) { }
  int NumBoxes() const { return std::min(box.NumBoxes(), obj.NumObjects()); }
  Matrix BoxIndex(int b) const { return box.BoxIndex(b); }
  int BoxObject(int b) const { return obj.BoxObject(b); }
  
private:
  BoxCollection &box;
  ObjectCollection &obj;
};

class PointsToBoxCollection : public BoxCollection
{
public:
  PointsToBoxCollection(PointCollection &p) : p(p) { }
  virtual int Size() const { return p.NumPoints(); }
  virtual Matrix Index(int box) const 
  { 
    Point pp = p.Points(box);
    return Matrix::Translate(pp.x, pp.y, pp.z);
  }		     
private:
  PointCollection &p;
};
typedef FunctionImpl0<PointCollection*, BoxCollection*, PointsToBoxCollection> PointsToBoxCollectionFunction;

class Origo : public BoxCollection
{
public:
  virtual int NumBoxes() { return 1; }
  virtual Matrix BoxIndex(int box) { return Matrix::Identity(); }
};


// morphism A->B
class PathCollection
{
public:
  virtual ~PathCollection() { }
  virtual float PathLength() const = 0;
  virtual int NumPaths(float posinpath) const = 0;
  virtual Matrix Index(float posinpath, int path) const = 0;
};

class PathVisibility
{
public:
  virtual bool Visible(float posinpath, int path)=0;
};

// diagram commutes
class EqualPaths : public PathCollection
{
public:
  EqualPaths(PathCollection &p1, PathCollection &p2) : p1(p1), p2(p2) 
  {
  }
  float PathLength() { return p1.PathLength(); }
  int NumPaths(float posinpath) const { return p1.NumPaths(posinpath) + p2.NumPaths(posinpath); }
  Matrix Index(float posinpath, int path) const
  {
    if (path < p1.NumPaths(posinpath))
      {
	return p1.Index(posinpath, path);
      }
    else
      {
	return p2.Index(posinpath, path-p1.NumPaths(posinpath));
      }
  }
private:
  PathCollection &p1;
  PathCollection &p2;
};

class BreakPaths : public PathCollection
{
public:
  BreakPaths(PathCollection &coll, PathVisibility &p) : coll(coll), p(p) { }
  float PathLength() const { return coll.PathLength(); }
  int NumPaths(float posinpath) const 
  {
    int c=coll.NumPaths(posinpath);
    int count = 0;
    for(int i=0;i<c;i++)
      {
	bool b = p.Visible(posinpath, i);
	if (b) count++;
      }
    return count;
  }
  Matrix Index(float posinpath, int path)
  {
    return coll.Index(posinpath, Mapping(posinpath, path));
  }
  int Mapping(float posinpath, int ii) const
  {
    int count = 0;
    int num = coll.NumPaths(posinpath);
    for(int i=0;i<num;i++)
      {
	bool b = p.Visible(posinpath,i);
	if (b)
	  {
	    if (count == ii) return i;
	    count++;
	  }
      }
    return 0;
  }
private:
  PathCollection &coll;
  PathVisibility &p;
};

class DiagonalPathCollection : public PathCollection
{
public:
  DiagonalPathCollection(float length, BoxCollection &box) : length(length), box(box) { }
  float PathLength() const { return length; }
  int NumPaths(float posinpath) const 
  {
    if (posinpath<length/2.0) return box.NumBoxes();
    return box.NumBoxes()*2;
  }
  Matrix Index(float /*posinpath*/, int path) const 
  {
    return box.BoxIndex(path);
  }
private:
  float length;
  BoxCollection &box;
};

class PathCollectionUser
{
public:
  PathCollectionUser(PathCollection &path) : path(path), timeproj(path), pathproj(path) { }
  BoxCollection *Cod() const { return ChooseTime(path.PathLength()); }
  BoxCollection *Dom() const { return ChooseTime(0.0); }
  BoxCollection *ChooseTime(float pos) const { timeproj.SetTime(pos); return &timeproj; }
  MatrixCurve *ChoosePath(int path) const { pathproj.SetPath(path); return &pathproj; }

  
  class TimeProjection : public BoxCollection
  {
  public:
    TimeProjection(PathCollection &path) : time(time), path(path) { }
    void SetTime(float time_) { time = time_; }
    int Size() const { return path.NumPaths(time); }
    Matrix Index(int box) const { return path.Index(time, box); }
  private:
    float time;
    PathCollection &path;
  };
  class PathProjection : public MatrixCurve
  {
  public:
    PathProjection(PathCollection &path) : path(path) { }
    Matrix Index(float pos) const { return path.Index(pos, pathpos); }
    float Size() const { return path.PathLength(); }
    void SetPath(int path_) { pathpos=path_; }
  private:
    int pathpos;
    PathCollection &path;
  };
  PathCollection &path;
  mutable TimeProjection timeproj;
  mutable PathProjection pathproj;
};

class SampleMatrixCurve : public BoxCollection
{
public:
  SampleMatrixCurve(const MatrixCurve &curve, int count) : curve(curve), count(count) { }
  int Size() const { return count; }
  Matrix Index(int box) const
  {
    float length = curve.Size();
    length /= float(count);
    length *= float(box);
    return curve.Index(length);
  }
private:
  const MatrixCurve &curve;
  int count;
};
typedef FunctionImpl1<MatrixCurve*, BoxCollection*, int, SampleMatrixCurve> SampleMatrixCurveFunction;

class Boxable
{
public:
  virtual ~Boxable() { }

  virtual void SetBox(Matrix b)=0;
};
class BooleanOps
{
public:
  virtual ~BooleanOps() { }
  virtual bool Inside(Point p) const { return false; }
};
class BoxableFaceCollection : public FaceCollection, public BooleanOps
{
};


class FaceStore
{
public:
  virtual void Add(const FaceCollection &coll)=0;
};

struct FacePointStruct
{
  Point vec_point;
  Vector normal;
  float attrib[10];
  int attrib_id[10];
  int attribI_id[10];
  unsigned int color;
  Point2d tex_coord;
};

struct FaceIndexStruct
{
  int face;
  int point;
  friend bool operator<(const FaceIndexStruct &i1, const FaceIndexStruct &i2)
  {
    if (i1.face < i2.face) return true;
    if (i1.face > i2.face) return false;
    if (i1.point < i2.point) return true;
    if (i1.point > i2.point) return false;
    return false;
  }
};

class FaceCollectionStore : public BoxableFaceCollection, public FaceStore
{
public:
  FaceCollectionStore() : facecount(0),texturecount(0), textures(false) { }
  void SetTextures(bool b) { textures = b; }
  
  void Add(const FaceCollection &coll)
  {
    Add(coll, textures);
  }
  void Add(const FaceCollection &coll, bool textures)
  {
    int oldtextures = texturecount;
    int numtextures = coll.NumTextures();
    if (textures)
      {
	for(int i=0;i<numtextures;i++)
	  {
	    const_cast<FaceCollection&>(coll).GenTexture(i);
	    buffers.push_back(coll.TextureBuf(i));
	  }
      }
    int faces = coll.NumFaces();
    int oldfaces = facecount;
    facecount += faces;
    texturecount += coll.NumTextures();
    for(int f=0;f<faces;f++)
      {
	int points = coll.NumPoints(f);
	points_vec.push_back(points);
	textureselection.push_back(coll.FaceTexture(f)+oldtextures);
	for(int p = 0;p<points;p++)
	  {
	    FaceIndexStruct fi;
	    FacePointStruct fp;
	    fi.face = f + oldfaces;
	    fi.point = p;
	    fp.vec_point = coll.FacePoint(f,p);
	    fp.normal = coll.PointNormal(f,p);
	    // TODO Attrib & AttribI
	    fp.color = coll.Color(f,p);
	    fp.tex_coord = coll.TexCoord(f,p);
	    map[fi] = fp;
	  }
      }
  }


  virtual int NumFaces() const { return facecount; }
  virtual int NumPoints(int face) const { return points_vec[face]; }
  virtual Point FacePoint(int face, int point) const 
  {
    FacePointStruct &fp = find(face,point);
    return fp.vec_point;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    FacePointStruct &fp = find(face,point);
    return fp.normal;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    FacePointStruct &fp = find(face,point);
    return fp.color;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    FacePointStruct &fp = find(face,point);
    return fp.tex_coord;
  }
  int NumTextures() const { return texturecount; }
  BufferRef TextureBuf(int num) const { return buffers[num]; }
  int FaceTexture(int face) const { return textureselection[face]; }

public:
  FacePointStruct &find(int face, int point) const
  {
    FaceIndexStruct ii = { face, point };
    FacePointStruct &fp = map[ii];
    return fp;
  }
private:
  int facecount;
  std::vector<int> points_vec;
  int texturecount;
  std::vector<BufferRef> buffers;
  std::vector<int> textureselection;
  mutable std::map<FaceIndexStruct, FacePointStruct> map;
  //std::vector<FacePoint> vec;
  bool textures;
};


class FrameFaceCollection : public BoxableFaceCollection
{
public:
  FrameFaceCollection(BoxableFaceCollection &coll, int frame) : coll(coll), frame(frame) { }
  virtual int NumFaces() const { return coll.NumFaces(); }
  virtual int NumPoints(int face) const { return coll.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return coll.FacePoint(face, point); }
  virtual Vector PointNormal(int face, int point) const { return coll.PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { 
    if (id==AttrFrame)
      {
	//std::cout << "AttribI " << face << " " << point << " " << frame << std::endl;
	return float(frame)+0.01;
      }
    return coll.Attrib(face, point, id); }
  virtual int AttribI(int face, int point, int id) const 
  { 
    if (id==AttrFrame)
      {
	//std::cout << "AttribI " << face << " " << point << " " << frame << std::endl;
	return frame;
      }
    return coll.AttribI(face, point, id); 
  }
  virtual unsigned int Color(int face, int point) const { return coll.Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return coll.TexCoord(face,point); }
  virtual bool Inside(Point p) const { return false; }
  virtual void SetBox(Matrix b) { }
private:
  const BoxableFaceCollection &coll;
  int frame;
};


class BoxableFaceCollectionConvert : public BoxableFaceCollection
{
public:
  BoxableFaceCollectionConvert(const FaceCollection &coll) : coll(coll) { }
  virtual int NumFaces() const { return coll.NumFaces(); }
  virtual int NumPoints(int face) const { return coll.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return coll.FacePoint(face, point); }
  virtual Vector PointNormal(int face, int point) const { return coll.PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return coll.Attrib(face, point, id); }
  virtual int AttribI(int face, int point, int id) const { return coll.AttribI(face, point, id); }
  virtual unsigned int Color(int face, int point) const { return coll.Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return coll.TexCoord(face,point); }
  virtual bool Inside(Point p) const { return false; }
  virtual void SetBox(Matrix b) { }

private:
  const FaceCollection &coll;
};

class FrameFaceCollectionArray : public FaceCollectionArray
{
public:
  FrameFaceCollectionArray(FaceCollectionArray &arr) : arr(arr), frames(0), convert(0) { }
  int Size() const { return arr.Size(); }
  const FaceCollection *Index(int i) const
  {
    delete frames;
    delete convert;
    convert = new BoxableFaceCollectionConvert(*arr.Index(i));
    frames = new FrameFaceCollection(*convert, i);
    return frames;
  }
private:
  FaceCollectionArray &arr;
  mutable FrameFaceCollection *frames;
  mutable BoxableFaceCollectionConvert *convert;
};

class EmptyBoxableFaceCollection : public BoxableFaceCollection
{
public:
  void SetBox(Matrix b) { }
  virtual int NumFaces() const { return 0; }
  virtual int NumPoints(int face) const { return 0; }
  virtual Point FacePoint(int face, int point) const { return Point(0.0,0.0,0.0); }
  virtual Vector PointNormal(int face, int point) const { return Vector(0.0,0.0,0.0); }
  virtual unsigned int Color(int face, int point) const 
  {
    std::cout << "EmptyBoxableFaceCollection::0" << std::endl;
    return 0; 
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    std::cout << "EmptyBoxableFaceCollection::1" << std::endl;
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }

};

void AttachBoxes(const BoxCollection &c, const std::vector<Boxable*> &v);

class ObjectSelection
{
public:
  virtual BoxableFaceCollection *Select(int index)=0;
};

class ConstantObjectSelection : public ObjectSelection
{
public:
  ConstantObjectSelection(BoxableFaceCollection *face) : face(face) { }
  BoxableFaceCollection *Select(int index)
  {
    return face;
  }
private:
  BoxableFaceCollection *face;
};

class BoxedElem : public BoxableFaceCollection
{
public:
  BoxedElem(BoxCollection &coll, BoxableFaceCollection &item) : coll(coll), sel(cont), cont(&item) { }
  BoxedElem(BoxCollection &coll, ObjectSelection &sel) : coll(coll), sel(sel), cont(0) { }
  void SetBox(Matrix m)
  {
    /*
    for(int i=0;i<coll.NumBoxes();i++)
      {
	BoxableFaceCollection *face = sel.Select(i);
	face->SetBox(m);
      }
    */
  }
  int NumFaces() const 
  {
    int count=0;
    for(int i=0;i<coll.NumBoxes();i++)
      {
	FaceCollection *face = sel.Select(i);
	count += face->NumFaces();
      }
    return count;
  }
  struct FaceObject
  {
    int object;
    int face;
  };
  FaceObject FindFace(int face) const
  {
    int count=0;
    for(int i=0;i<coll.NumBoxes();i++)
      {
	FaceCollection *fc = sel.Select(i);
	int prevcount = count;
	count += fc->NumFaces();
	if (count > face) 
	  { 
	    FaceObject o;
	    o.object = i;
	    o.face = face - prevcount;
	    return o;
	  }
      }
    FaceObject o;
    o.object = 0;
    o.face = 0;
    return o;
  }
  int NumPoints(int face) const 
  {
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->NumPoints(o.face);
  }
  Point FacePoint(int face, int point) const 
  { 
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->FacePoint(o.face, point)*coll.BoxIndex(o.object);   
  }
  unsigned int Color(int face, int point) const
  {
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->Color(o.face, point);
  }
  Point2d TexCoord(int face, int point) const
  {
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->TexCoord(o.face, point);
  }

  Vector PointNormal(int face, int point) const 
  {
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->PointNormal(o.face, point);  
  }
  float Attrib(int face, int point, int id) const 
  { 
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->Attrib(o.face,point,id); 
  }
  int AttribI(int face, int point, int id) const 
  { 
    FaceObject o = FindFace(face);
    FaceCollection *fc = sel.Select(o.object);
    return fc->AttribI(o.face,point,id); 
  }
private:
  BoxCollection &coll; 
  ObjectSelection &sel;
  ConstantObjectSelection cont;
};
typedef FunctionImpl1<BoxCollection*, BoxableFaceCollection*, BoxableFaceCollection &, BoxedElem> BoxedElemFunction;


class SampleSurfaceForGrid : public GridIn3d
{
public:
  SampleSurfaceForGrid(const SurfaceIn3d &surf, int x, int y) : surf(surf), x(x), y(y) {}
  virtual Point Index(int x1, int y1) const 
  {
    return surf.Index(Sample(x1,x,surf.XSize()),Sample(y1,y,surf.YSize()));
  }
  virtual Point2d Texture(int x1, int y1) const
  {
    return surf.Texture(Sample(x1,x,surf.XSize()),Sample(y1,y,surf.YSize()));
  }
  virtual Vector Normal(int x1, int y1) const
  {
    return surf.Normal(Sample(x1,x,surf.XSize()),Sample(y1,y,surf.YSize()));
  }
  virtual unsigned int Color(int x1, int y1) const
  {
    return surf.Color(Sample(x1,x, surf.XSize()), Sample(y1,y, surf.YSize()));
  }
  virtual float Attrib(int x1, int y1, int id) const
  {
    return surf.Attrib(Sample(x1,x,surf.XSize()),Sample(y1,y,surf.YSize()), id);
  }
  virtual int AttribI(int x1, int y1, int id) const
  {
    return surf.AttribI(Sample(x1,x,surf.XSize()),Sample(y1,y,surf.YSize()), id);
  }

  virtual int XSize() const { return x; }
  virtual int YSize() const { return y; }

  static float Sample(int val, int size, float sizef) { return float(val)*sizef/float(size); }
private:
  const SurfaceIn3d &surf;
  int x;
  int y;
};
typedef FunctionImpl2<SurfaceIn3d*, GridIn3d*, int, int, SampleSurfaceForGrid> SampleSurfaceForGridFunction;

class SampleSurfaceIn3d : public FaceCollection, public TextureCoords
{
public:
  SampleSurfaceIn3d(const SurfaceIn3d &surf, int texture, int x, int y) : surf(surf), texture(texture), x(x), y(y) { }
  virtual int Texture(int face) const { return texture; }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = Sample(face, point);
    Point2d pp = surf.Texture(p.x, p.y);
    return pp;
  }

  virtual int NumFaces() const { return x*y; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    Point2d p = Sample(face, point);
    Point pp = surf.Index(p.x, p.y);
    //std::cout << face << " " << point << " " << p.x <<" " << p.y << " " << pp << std::endl;
    return pp;
  }
  virtual unsigned int Color(int face, int point) const
  {
    Point2d p = Sample(face, point);
    unsigned int pp = surf.Color(p.x, p.y);
    return pp;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point2d p = Sample(face, point);
    Vector pp = surf.Normal(p.x, p.y);
    return pp;
  }
  float Attrib(int face, int point, int id) const 
  {
    Point2d p = Sample(face, point);
    float pp = surf.Attrib(p.x, p.y, id);
    //std::cout << p.x << " " << p.y << " " << id << " " << pp << std::endl;
    return pp;
  }
  int AttribI(int face, int point, int id) const 
  {
    Point2d p = Sample(face, point);
    int pp = surf.AttribI(p.x, p.y, id);
    //std::cout << p.x << " " << p.y << " " << id << " " << pp << std::endl;
    return pp;
  }
private:
  Point2d Sample(int face, int point) const
  {
    int yindex = face/x;
    int ddy = face - yindex*x;
    int xindex = ddy;

    if (point==1 || point==2) xindex++;
    if (point==2 || point==3) yindex++;
    //std::cout << face << " " << point << " " << xindex << " " << yindex;
    float xlength = surf.XSize();
    float ylength = surf.YSize();
    xlength /= float(x);
    ylength /= float(y);
    xlength *= float(xindex);
    ylength *= float(yindex);
    Point2d p;
    p.x = xlength;
    p.y = ylength;
    //std::cout << ":" << xlength << " " << ylength << std::endl;
    return p;
  }


private:
  const SurfaceIn3d &surf;
  int texture;
  int x,y;
};
typedef FunctionImpl3<SurfaceIn3d*, FaceCollection*, int,int,int, SampleSurfaceIn3d> SampleSurfaceIn3dFunction;
//
// Note: SurfaceIn3d methods needs to be called immediately after Index()
// is called. Otherwise other Index() calls might confuse state of this.
//
class QuadsToSurfaces : public Array<int, const SurfaceIn3d*>, private SurfaceIn3d
{
public:
  QuadsToSurfaces(FaceCollection &coll) : coll(coll) { }
  const SurfaceIn3d* Index(int i) const { last_i = i; return this; }
  int Size() const { return coll.NumFaces(); }
  
  virtual Point Index(float x, float y) const { return interpolate2(last_i, x, y); }
  virtual Point2d Texture(float x, float y) const { Point2d p; p.x = x; p.y = y; return p; }
  virtual unsigned int Color(float x, float y) const 
  {
    unsigned int v1 = coll.Color(last_i, 0);
    unsigned int v2 = coll.Color(last_i, 1);
    unsigned int v3 = coll.Color(last_i, 2);
    unsigned int v4 = coll.Color(last_i, 3);
    return interpolate(v1,v2,v3,v4,x,y);
  }
  virtual int AttribI(float x, float y, int id) const
  {
    return 0;
  } 
  virtual float Attrib(float x, float y, int id) const 
  {
    if (id==AttrInternalX) return x;
    if (id==AttrInternalY) return y;
    if (id==AttrInternalNX_X)
      {
	Point p = Index(x+0.01,y);
	return p.x;
      }
    if (id==AttrInternalNX_Y)
      {
	Point p = Index(x+0.01,y);
	return p.y;
      }
    if (id==AttrInternalNX_Z)
      {
	Point p = Index(x+0.01,y);
	return p.z;
      }
    if (id==AttrInternalNY_X)
      {
	Point p = Index(x,y+0.01);
	return p.x;
      }
    if (id==AttrInternalNY_Y)
      {
	Point p = Index(x,y+0.01);
	return p.y;
      }
    if (id==AttrInternalNY_Z)
      {
	Point p = Index(x,y+0.01);
	return p.z;
      }

    return 0.0; 
  }
  virtual Vector Normal(float x, float y) const
  {
    Vector v1 = coll.PointNormal(last_i, 0);
    Vector v2 = coll.PointNormal(last_i, 1);
    Vector v3 = coll.PointNormal(last_i, 2);
    Vector v4 = coll.PointNormal(last_i, 3);
    return interpolate(v1,v2,v3,v4,x,y);
  }
  virtual float XSize() const { return 1.0; }
  virtual float YSize() const { return 1.0; }

private:
  static Point interpolate(Point p0, Point p1, Point p2, Point p3, float x, float y)
  {
    Point pp0 = p0 + Vector(p3-p0)*x;
    Point pp1 = p1 + Vector(p2-p1)*x;
    return y*Vector(pp0) + (1.0-y)*Vector(pp1);
  }
  Point interpolate2(int face, float x, float y) const
  {
    Point p0 = coll.FacePoint(face, 0);
    Point p1 = coll.FacePoint(face, 1);
    Point p2 = coll.FacePoint(face, 2);
    Point p3 = coll.FacePoint(face, 3);
    return interpolate(p0,p1,p2,p3, x,y);
  }
  static unsigned int interpolate(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3, float x, float y)
  {
    unsigned int v1 = Color::Interpolate(p0,p1,x);
    unsigned int v2 = Color::Interpolate(p3,p2,x);

    unsigned int res = Color::Interpolate(v1,v2,y);
    return res;
  }
  static Vector interpolate(Vector p0, Vector p1, Vector p2, Vector p3, float x, float y)
  {
    Vector v1 = p0*x+p1*(1.0-x);
    Vector v2 = p3*x+p2*(1.0-x);

    Vector res = v1*y+v2*(1.0-y);
    return res;
  }


private:
  mutable int last_i;
  FaceCollection &coll;
};


class SplitQuads : public FaceCollection
{
public:
  SplitQuads(FaceCollection &faces, int x, int y) : surf(faces), faces(faces), x(x), y(y) { }
  virtual int NumFaces() const 
  { 
    SampleSurfaceIn3d sample(*surf.Index(0), 0, x,y);  
    return sample.NumFaces()*faces.NumFaces();
  }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.FacePoint(face1, point);
  }

  virtual unsigned int Color(int face, int point) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.Color(face1, point);
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.TexCoord(face1, point);
  }


  virtual Vector PointNormal(int face, int point) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.PointNormal(face1, point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.Attrib(face1, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    int face1 = face / faces.NumFaces();
    int face2 = face - face1*faces.NumFaces();
    const SurfaceIn3d *s = surf.Index(face2);
    SampleSurfaceIn3d sample(*s, 0, x, y);
    return sample.AttribI(face1, point, id);
  }
private:
  QuadsToSurfaces surf;
  FaceCollection &faces;
  int x,y;
};
typedef FunctionImpl2<FaceCollection*, FaceCollection*, int, int, SplitQuads> SplitQuadsFunction;



template<class T>
class OrElem : public T /*BoxableFaceCollection*/
{
public:
  OrElem() : vec() { }
  template<class It>
  OrElem(It beg, It end) : vec(beg, end) { }
  OrElem(T *o1, T *o2, T *o3, T *o4) 
  { 
    vec.push_back(o1);
    vec.push_back(o2);
    vec.push_back(o3);
    vec.push_back(o4);
  }
  void update_faces_cache() const
  {
    int count = 0;
    int sum = 0;
    int f = 0;
    int k = 0;
    int faces = NumFaces();
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++,k++)
      {
	int oldsum = sum;
	count = (*i)->NumFaces();
	sum += count;
	for(int i=0;i<count;i++)
	  {
	    if (f%1000==0) {
	      std::cout << "faces: " << f << "/" << faces << std::endl;
	    }

	    faces_cache.push_back(f-oldsum);
	    faces_num.push_back(k);
	    f++;
	  }
      }
#if 0
    int faces = NumFaces();
    for(int f=0;f<faces;f++)
      {
	if (f%1000==0) {
	  std::cout << "faces: " << f << "/" << faces << std::endl;
	}
	int val = 0;
	int k=0;
	int count = 0;
	for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++,k++)
	  {
	    int oldcount = count;
	    count += (*i)->NumFaces();
	    if (count > f) { val = f-oldcount; break; }
	  }
	faces_cache.push_back(val);
	faces_num.push_back(k);
      }
    
#endif
  }
  void push_back(T *ptr)
  {
    vec.push_back(ptr);
  }
  bool Inside(Point p) const 
  {
    bool b = false;
    for(typename std::vector<T*>::const_iterator i=vec.begin();i!=vec.end();i++)
      b |= (*i)->Inside(p);
    return b;
  }
  void SetBox(Matrix b) 
  {
    //for(typename std::vector<T*>::iterator i=vec.begin();i!=vec.end();i++)
    //  (*i)->SetBox(b);
  }
  virtual int NumFaces() const 
  { 
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      count += (*i)->NumFaces();
    return count; 
  }
  virtual int NumPoints(int face) const 
  { 
    return vec[faces_num[face]]->NumPoints(faces_cache[face]);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->NumPoints(face-oldcount);
      }
    return 0;
#endif
  }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[faces_num[face]]->FacePoint(faces_cache[face], point);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->FacePoint(face-oldcount, point);
      }
    return Point(0.0,0.0,0.0);
#endif
  }

  virtual unsigned int Color(int face, int point) const
  {
    return vec[faces_num[face]]->Color(faces_cache[face],point);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->Color(face-oldcount, point);
      }
    //std::cout << "OrElem::0" << std::endl;

    return 0;
#endif
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    return vec[faces_num[face]]->TexCoord(faces_cache[face],point);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->TexCoord(face-oldcount, point);
      }
    //std::cout << "OrElem::0" << std::endl;

    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
#endif
  }



  virtual Vector PointNormal(int face, int point) const
  {
    return vec[faces_num[face]]->PointNormal(faces_cache[face], point);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->PointNormal(face-oldcount, point);
      }
    return Vector(0.0,0.0,0.0);
#endif
  }
  float Attrib(int face, int point, int id) const 
  {
    return vec[faces_num[face]]->Attrib(faces_cache[face], point, id);
#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->Attrib(face-oldcount, point, id);
      }
    return 0.0;
#endif
  }
  int AttribI(int face, int point, int id) const 
  {
    return vec[faces_num[face]]->AttribI(faces_cache[face], point, id);

#if 0
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumFaces();
	if (count > face) return (*i)->AttribI(face-oldcount, point, id);
      }
    return 0.0;
#endif
  }

  virtual int NumTextures() const 
  { 
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += vec[i]->NumTextures();
      }
    return count; 
  }
  
  virtual void GenTexture(int num) 
  { 
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumTextures();
	if (count > num) { (*i)->GenTexture(num-oldcount); break; }
      }
  }
  virtual BufferRef TextureBuf(int num) const 
  { 
    int count = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumTextures();
	if (count > num) { return (*i)->TextureBuf(num-oldcount);}
      }
    BufferRef ref = BufferRef::NewBuffer(1,1);
    return ref;
  }
  virtual int FaceTexture(int face) const 
  { 
    int count = 0;
    int texcount = 0;
    for(typename std::vector<T /*BoxableFaceCollection*/ *>::const_iterator i=vec.begin();i!=vec.end();i++)
      {
	int oldcount = count;
	int oldtexcount = texcount;
	count += (*i)->NumFaces();
	texcount += (*i)->NumTextures();
	if (count > face) { return oldtexcount+(*i)->FaceTexture(face-oldcount);}
      }
    return 0;
  }

private:
  std::vector<T /*BoxableFaceCollection*/ *> vec;
  mutable std::vector<int> faces_cache;
  mutable std::vector<int> faces_num;
};

class QuadElem : public BoxableFaceCollection
{
public:
  QuadElem(Point p1, Point p2, Point p3, Point p4) : p1(p1), p2(p2), p3(p3), p4(p4) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return 1; }
  virtual int NumPoints(int face) const { return 4; }
  //virtual int Texture(int face) const { return face; }
  //virtual Point2d TexCoord(int face, int point) const;
  virtual Point FacePoint(int face, int point) const
  {
    if (point==0) return p1;
    if (point==1) return p2;
    if (point==2) return p3;
    return p4;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    switch(id)
      {
      case AttrCenterX: return 0.0; //box.matrix[3];
      case AttrCenterY: return 0.0; //box.matrix[7];
      case AttrCenterZ: return 0.0; //box.matrix[11];
      }
    return 0.0;
  }

private:
  Point p1,p2,p3,p4;
};

class TriElem : public BoxableFaceCollection
{
public:
  TriElem(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return 1; }
  virtual int NumPoints(int face) const { return 3; }
  //virtual int Texture(int face) const { return face; }
  //virtual Point2d TexCoord(int face, int point) const;
  virtual Point FacePoint(int face, int point) const
  {
    if (point==0) return p1;
    if (point==1) return p2;
    return p3;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    switch(id)
      {
      case AttrCenterX: return 0.0; //box.matrix[3];
      case AttrCenterY: return 0.0; //box.matrix[7];
      case AttrCenterZ: return 0.0; //box.matrix[11];
      }
    return 0.0;
  }

private:
  Point p1,p2,p3;
};

struct CubeCoords
{
  Point p111;
  Point p112;
  Point p121;
  Point p122;
  Point p211;
  Point p212;
  Point p221;
  Point p222;
};

class PolygonElem : public BoxableFaceCollection
{
public:
  PolygonElem() { }
  void push_back(Point p) { points.push_back(p); }
  virtual int NumFaces() const { return 1; }
  virtual int NumPoints(int face) const { return points.size(); }
  virtual Point FacePoint(int face, int point) const
  {
    return points[point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return Vector(0.0,0.0,0.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
  virtual int NumTextures() const { return 0; }
  virtual void GenTexture(int num) { }
  virtual BufferRef TextureBuf(int num) const { BufferRef ref; return ref; }
  virtual int FaceTexture(int face) const { return -1; }

private:
  std::vector<Point> points;
};

class CubeElem : public BoxableFaceCollection, public CoordSource
{
public:
  CubeElem() : p111(Point(-1.0,-1.0,-1.0)),
	       p112(Point(-1.0,-1.0,1.0)),
	       p121(Point(-1.0,1.0,-1.0)),
	       p122(Point(-1.0,1.0,1.0)),
	       p211(Point(1.0,-1.0,-1.0)),
	       p212(Point(1.0,-1.0,1.0)),
	       p221(Point(1.0,1.0,-1.0)),
	       p222(Point(1.0,1.0,1.0)) { }
  CubeElem(Point p111,
	   Point p112,
	   Point p121,
	   Point p122,
	   Point p211,
	   Point p212,
	   Point p221,
	   Point p222) : p111(p111), p112(p112), p121(p121),
			 p122(p122), p211(p211), p212(p212),
			 p221(p221), p222(p222) { }
  virtual Coords coord() const
  {
    Coords c;
    c.center = Point(0.0,0.0,0.0);
    c.u_x = Vector(1.0,0.0,0.0);
    c.u_y = Vector(0.0,1.0,0.0);
    c.u_z = Vector(0.0,0.0,1.0);
    return c;
  }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return 6; }
  virtual int NumPoints(int face) const { return 4; }
  //virtual int Texture(int face) const { return face; }
  //virtual Point2d TexCoord(int face, int point) const;
  virtual Point FacePoint(int face, int point) const;
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v = FacePoint(face, point);
    //Vector center = Vector(box.matrix[3], box.matrix[7], box.matrix[11]);
    //v.Normalize();
    return v; /*-center;*/
  }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    switch(id)
      {
      case AttrCenterX: return 0.0; //box.matrix[3];
      case AttrCenterY: return 0.0; //box.matrix[7];
      case AttrCenterZ: return 0.0; //box.matrix[11];
      }
    return 0.0;
  }

private:
  Point p111;
  Point p112;
  Point p121;
  Point p122;
  Point p211;
  Point p212;
  Point p221;
  Point p222;
  //Matrix box;
};
typedef FunctionImplT0<BoxableFaceCollection*, CubeElem> CubeElemFunction;

class CylinderElem : public BoxableFaceCollection, public CoordSource
{
public:
  virtual Coords coord() const
  {
    Coords c;
    c.center = Point(0.0,0.0,0.0);
    c.u_x = Vector(1.0,0.0,0.0);
    c.u_y = Vector(0.0,1.0,0.0);
    c.u_z = Vector(0.0,0.0,1.0);
    return c;
  }

  CylinderElem(int numfaces) : numfaces(numfaces) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return numfaces; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const;

  virtual Vector PointNormal(int face, int point) const
  {
    Vector v = FacePoint(face, point);
    //Vector center = Vector(box.matrix[3], box.matrix[7], box.matrix[11]);
    return v; /*-center;*/
  }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

private:
  //Matrix box;
  int numfaces;
};



typedef FunctionImplT0<BoxableFaceCollection*, CylinderElem> CylinderElemFunction;


class DiskElem : public BoxableFaceCollection
{
public:
  DiskElem(int numfaces, float disk_pos) :  numfaces(numfaces), disk_pos(disk_pos) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return numfaces; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const;

  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Vector v = FacePoint(face, point);
    //Vector center = Vector(box.matrix[3], box.matrix[7], box.matrix[11]);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

private:
  //Matrix box;
  int numfaces;
  float disk_pos;
};
typedef FunctionImplT2<BoxableFaceCollection*, int, int, DiskElem> DiskElemFunction;


template<class T>
class IdentityFunction : public Function<T,T>
{
public:
  virtual T Index(T a) const { return a; }
};

// interpolation morphism
class InterpolationPathCollection : public PathCollection
{
public:
  InterpolationPathCollection(float pathlength, BoxCollection &dom, BoxCollection &cod, Function<int,int> &mapping) : pathlength(pathlength), dom(dom), cod(cod), mapping(mapping) { }
  int NumPaths(float /*time*/) const { return dom.NumBoxes(); }
  float PathLength() const { return pathlength; }
  Matrix Index(float posinpath, int path) 
  {
    Matrix domm = dom.BoxIndex(path);
    Matrix codm = dom.BoxIndex(mapping.Index(path));
    Matrix m = Matrix::Interpolate(domm, codm, posinpath/pathlength);
    return m;
  }
private:
  float pathlength;
  BoxCollection &dom;
  BoxCollection &cod;
  Function<int,int> &mapping;
};


class Water : public Function2<float, float, float>
{
public:
  Water(float time) : time(time) 
  { 
    int starttime=300;
    for(int i =0;i<320;i++)
      {
	float ii = i;
	if (i>time-starttime&&i<time-starttime+50.0)
	  array[i]=0.5*0.1*sin(ii*2.0*3.14159/320.0);
	else
	  array[i]=0.05*0.1;
      }
  }
  float Index(float x, float y) const 
  {
    float m = 8;
    float n = 5;
    // waves
    float lx = cos(m*3.14159*x);
    float ly = cos(n*3.14159*y);
    float lt = cos(8.0*time);
    float wave = 0.02*lx*ly*lt;

    float m2 = 5;
    float n2 = 10;
    // waves
    float lx2 = cos(m2*3.14159*x);
    float ly2 = cos(n2*3.14159*y);
    float lt2 = cos(18.0*time);
    float wave2 = 0.02*lx2*ly2*lt2;
    
    // splash
    float xr = x-0.5;
    float yr = y-0.5;
    float r = sqrtf(xr*xr+yr*yr);
    //float s = 1.0/(r+0.05);
    int rr = int(320.0*r);
    float s=array[rr]; //sin(r);
    return wave+wave2+s*sin(r*90.0-time*2.0); 
  }
private:
  float time;
  float array[320];
};

class HeightMap : public BoxableFaceCollection
{
public:
  HeightMap(int x, int y, Function2<float, float, float> &func) : x(x), y(y), func(func) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return x*y; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int dx=0, dy=0;
    if (point==1) dx=1;
    else if (point==2) { dx=1; dy=1; }
    else if (point==3) { dy=1; }
    
    int xx = face / y;
    int yy = face - xx*y;
    xx += dx;
    yy += dy;
    return GetPoint(xx,yy);
  }
  unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Point GetPoint(float xx, float yy) const
  {
    float zz = func.Index(xx/x,yy/y);
    
    Vector vx=xx*Vector(1.0/x,0.0,0.0);
    Vector vy=yy*Vector(0.0,0.0,1.0/y);
    Vector vz=zz*Vector(0.0,1.0,0.0);
    return (vx+vy+vz);
  }

  virtual Vector PointNormal(int face, int point) const
  {
    int dx=0, dy=0;
    if (point==1) dx=1;
    else if (point==2) { dx=1; dy=1; }
    else if (point==3) { dy=1; }
    
    int xx = face / y;
    int yy = face - xx*y;
    xx += dx;
    yy += dy;
    Point p = GetPoint(xx,yy);
    Point p2 = GetPoint(xx+0.01,yy);
    Point p3 = GetPoint(xx,yy+0.01);
    return -Vector::CrossProduct(p2-p, p3-p);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

private:
  int x,y;
  //Matrix box;
  Function2<float, float, float> &func;
};
class ColorElem : public BoxableFaceCollection
{
public:
  ColorElem(const BoxableFaceCollection &next, unsigned int color) : next(next) , color(color) { }
  virtual void SetBox(Matrix b) { /*const_cast<BoxableFaceCollection&>(next).SetBox(b);*/ }
  virtual int NumFaces() const { return next.NumFaces(); }
  virtual int NumPoints(int face) const { return next.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return next.FacePoint(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return color;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    return next.PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next.Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next.AttribI(face,point,id);
  }

private:
  const BoxableFaceCollection &next;
  unsigned int color;
};

class ColorFaceElem : public BoxableFaceCollection
{
public:
  ColorFaceElem(const BoxableFaceCollection &next, 
		unsigned int color_1,
		unsigned int color_2,
		unsigned int color_3,
		unsigned int color_4
		) : next(next) , color_1(color_1), color_2(color_2), color_3(color_3), color_4(color_4) { }
  virtual void SetBox(Matrix b) { /*const_cast<BoxableFaceCollection&>(next).SetBox(b);*/ }
  virtual int NumFaces() const { return next.NumFaces(); }
  virtual int NumPoints(int face) const { return next.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return next.FacePoint(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    switch(point)
      {
      case 0: return color_1;
      case 1: return color_2;
      case 2: return color_3;
      case 3: return color_4;
      }
    return color_1;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return next.TexCoord(face,point);
    /*
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
    */
  }

  virtual Vector PointNormal(int face, int point) const
  {
    return next.PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next.Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next.AttribI(face,point,id);
  }

private:
  const BoxableFaceCollection &next;
  unsigned int color_1;
  unsigned int color_2;
  unsigned int color_3;
  unsigned int color_4;
};


class TextureElem : public BoxableFaceCollection
{
public:
  TextureElem(const BoxableFaceCollection &next, TextureCoords &coords) : next(next) , coords(coords) { }
  virtual void SetBox(Matrix b) { /*const_cast<BoxableFaceCollection&>(next).SetBox(b);*/ }
  virtual int NumFaces() const { return next.NumFaces(); }
  virtual int NumPoints(int face) const { return next.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return next.FacePoint(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return next.Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coords.TexCoord(face,point);
  }

  virtual Vector PointNormal(int face, int point) const
  {
    return next.PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next.Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next.AttribI(face,point,id);
  }

private:
  const BoxableFaceCollection &next;
  TextureCoords &coords;
};




class TextureElem2 : public BoxableFaceCollection
{
public:
  TextureElem2(const FaceCollection &next, BufferRefReq &ref, TextureCoords &coords) : next(next), ref(ref), coords(coords) { }
  virtual void SetBox(Matrix b) { /*const_cast<BoxableFaceCollection&>(next).SetBox(b);*/ }
  virtual int NumFaces() const { return next.NumFaces(); }
  virtual int NumPoints(int face) const { return next.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return next.FacePoint(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return next.Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coords.TexCoord(face,point);
  }

  virtual Vector PointNormal(int face, int point) const
  {
    return next.PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next.Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next.AttribI(face,point,id);
  }
  virtual int NumTextures() const { return 1+next.NumTextures(); }
  virtual void GenTexture(int num)
  {
    ref.Gen();
  }
  virtual BufferRef TextureBuf(int num) const 
  {
    if (num==0) 
      {
	return ref.Buffer();
      }
    else return next.TextureBuf(num-1);
  }
  virtual int FaceTexture(int face) const 
  {
    int i = coords.Texture(face);
    return i;
  }
  
private:
  const FaceCollection &next;
  BufferRefReq &ref;
  TextureCoords &coords;
};



class MatrixElem : public BoxableFaceCollection, public Boxable
{
public:
  MatrixElem(const BoxableFaceCollection &next, Matrix m) : next(&next), m(m), box(Matrix::Identity()) { inverse=false;  }
  virtual void SetBox(Matrix b) { box=b; }
  virtual int NumFaces() const { return next->NumFaces(); }
  virtual int NumPoints(int face) const { return next->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = next->FacePoint(face,point);
    Vector v = p;
    Vector vv = v*m*box;
    return vv;
  
  }
  virtual unsigned int Color(int face, int point) const
  {
    return next->Color(face, point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return next->TexCoord(face,point);
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Point p = FacePoint(face,point);
    Vector v = next->PointNormal(face,point);
    Point p2 = p+v;
    p = p * m * box;
    p2 = p2 * m *box;
    Vector vv = p2 - p;
    return vv;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next->Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next->AttribI(face,point,id);
  }

  virtual bool Inside(Point p) const {
    if (!inverse) { inv=Matrix::Inverse(m); inverse=true; }
    Point pp = p*inv;
    return next->Inside(pp);
  }
public:
  void SetMatrix(Matrix mm) { m = mm; }
  void SetNext(const BoxableFaceCollection *next_) { next=next_; }
private:
  const BoxableFaceCollection *next;
  Matrix m;
  Matrix box;
  mutable Matrix inv;
  mutable bool inverse;
};
typedef FunctionImpl1<BoxableFaceCollection*, BoxableFaceCollection*, Matrix, MatrixElem> MatrixElemFunction;

class Move : public BoxableFaceCollection
{
public:
  Move(BoxableFaceCollection &coll, Vector v) : coll(coll), v(v) { }
  Move(BoxableFaceCollection &coll, float x, float y, float z) : coll(coll), v(x,y,z) { }
  virtual void SetBox(Matrix b) { /*coll.SetBox(b);*/ }
  virtual int NumFaces() const { return coll.NumFaces(); }
  virtual int NumPoints(int face) const { return coll.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = coll.FacePoint(face,point);
    p += v;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {    
    return coll.PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll.Attrib(face, point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll.AttribI(face,point,id);
  }

  virtual bool Inside(Point p) const {
    p-=v;
    return coll.Inside(p);
  }
private:
  BoxableFaceCollection &coll;
  Vector v;
};
typedef FunctionImpl1<BoxableFaceCollection*, BoxableFaceCollection*, Vector, Move> MoveFunction;

class OrFunc : public Function<int, bool>
{
public:
  OrFunc(Function<int, bool> &f, Function<int, bool> &f2) : f(f), f2(f2) { }
  bool Index(int a) const { return f.Index(a)||f2.Index(a); }
private:
  Function<int,bool> &f;
  Function<int,bool> &f2;
};

class AndFunc : public Function<int, bool>
{
public:
  AndFunc(Function<int, bool> &f, Function<int, bool> &f2) : f(f), f2(f2) { }
  bool Index(int a) const { return f.Index(a)&&f2.Index(a); }
private:
  Function<int,bool> &f;
  Function<int,bool> &f2;
};

class NotFunc : public Function<int, bool>
{
public:
  NotFunc(Function<int, bool> &f) : f(f) { } 
  bool Index(int a) const { return !f.Index(a); }
private:
  Function<int,bool> &f;
};

enum FaceType
  {
    EDoNotDraw,
    EDraw,
    EIntersect
  };

template<class T>
class IsEqualFunc : public Function<T, bool>
{
public:
  IsEqualFunc(T t) : t(t) { }
  bool Index(T tt) const { return tt==t; }
private:
  T t;
};

template<class A, class B, class C>
class ComposeFunc : public Function<A,C>
{
public:
  ComposeFunc(Function<A,B> &a, Function<B,C> &b) : a(a), b(b) 
  {
  }
  C Index(A aa) const { return b.Index(a.Index(aa)); }
private:
  Function<A,B> &a;
  Function<B,C> &b;
};
template<class C>
class SampleFunction : public ComposeFunc<int, float, C>
{
public:
  SampleFunction(Function<int, float> &a, Function<float, C> &b) : ComposeFunc<int,float,C>(a,b) { }
};

class Sample : public Function<int, float>
{
public:
  Sample(float zero, float succ) : zero(zero), succ(succ) { }
  float Index(int i) const
  {
    return zero + i*succ;
  }
private:
  float zero, succ;
};

class RemovePolysFunc : public Boxable, public Function<int, FaceType>
{
public:
  RemovePolysFunc(BoxableFaceCollection &obj, BoxableFaceCollection &obj2) : obj(obj), obj2(obj2)  { }

  void SetBox(Matrix b) { /*obj.SetBox(b);*/ }
  FaceType Index(int face) const
  {
    //Point sum = center.Points(face);
    int count = obj.NumPoints(face);
    bool foundtrue = false;
    bool foundfalse = false;
    for(int i=0;i<count;i++)
      {
	Point p = obj.FacePoint(face,i);
	bool b = obj2.Inside(p);
	if (b) { foundtrue = true; }
	else { foundfalse = true; }
      }
    if (foundtrue && foundfalse)
      return EIntersect;
    if (foundtrue)
      return EDraw;
    if (foundfalse)
      return EDoNotDraw;
    return EIntersect;
  }
private:
  BoxableFaceCollection &obj;
  BoxableFaceCollection &obj2;
  Matrix invbox;
};

class IsChangingFace : public Function<int, bool>
{
public:
  IsChangingFace(const std::vector<FaceCollection*> &v) : v(v) { }
  bool Index(int id) const
  {
    int s = v.size();
    for(int f=0;f<v[0]->NumPoints(id);f++)
      {
	Point p = v[0]->FacePoint(id,0);
	for(int i=1;i<s;i++)
	  {
	    Point p2 = v[i]->FacePoint(id,f);
	    if ((p2-p).Dist()>0.002) return true;
	  }
      }
    return false;
  }
private:
  const std::vector<FaceCollection*> &v;
};

class FilterFaces : public FaceCollection
{
public:
  FilterFaces(FaceCollection &obj, Function<int, bool> &func) : obj(obj), func(func) { }
  int NumFaces() const
  {
    int c=obj.NumFaces();
    int count = 0;
    for(int i=0;i<c;i++)
      {
	bool b = func.Index(i);
	if (b) count++;
      }
    return count;
  }
  int NumPoints(int face) const
  {
    return obj.NumPoints(Mapping(face));
  }
  Point FacePoint(int face, int point) const
  {
    return obj.FacePoint(Mapping(face), point);
  }
  unsigned int Color(int face, int point) const
  {
    return obj.Color(Mapping(face), point);
  }
  Point2d TexCoord(int face, int point) const
  {
    return obj.TexCoord(Mapping(face), point);
  }

  Vector PointNormal(int face, int point) const
  {
    return obj.PointNormal(Mapping(face), point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return obj.Attrib(Mapping(face), point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return obj.AttribI(Mapping(face),point,id);
  }

private:
  int Mapping(int ii) const
  {
    int count=0;
    int num = obj.NumFaces();
    for(int i=0;i<num;i++)
      {
	bool b = func.Index(i);
	if (b)
	  {
	  if (count == ii) return i;
	  count++;
	  }
      }
    return 0;
  }

private:
  FaceCollection &obj;
  Function<int,bool> &func;
};

class SplitPolygons : public FaceCollection
{
public:
  SplitPolygons(FaceCollection &a1, FaceCollection &a2, BooleanOps &ops, bool b) : a1(a1), a2(a2), ops(ops), points1(a1), points2(a2), b(b) { }

  int NumFaces() const { return faces.size(); }
  int NumPoints(int face) const { return faces[face].poly.size(); }
  Point FacePoint(int face, int point) const { return faces[face].poly[point]; }
  unsigned int Color(int face, int point) const { return Color::White().Pixel(); }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Vector PointNormal(int face, int point) const
  { 
    return normals[face].poly[point];
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

  struct OnePolygon
  {
    std::vector<Point> poly;
  };
  struct OneNormal
  {
    std::vector<Vector> poly;
  };

  void DoSplitting();

private:
  FaceCollection &a1;
  FaceCollection &a2;
  BooleanOps &ops;
  FaceCenterPoints points1, points2;
  std::vector<OnePolygon> faces;
  std::vector<OneNormal> normals;
  bool b;
};

template<class A>
class MemoizeFunction : public Function<int,A>
{
public:
  MemoizeFunction(int count, Function<int,A> &func) : func(func), array(count), toggle(count) 
  {
    for(int i=0;i<count;i++) toggle[i]=false;
  }

  A Index(int val) const
  { 
    if (toggle[val]==false)
      {
	toggle[val]=true;
	array[val]=func.Index(val);
      }
    return array[val];
  }
private:
  Function<int, A> &func;
  mutable std::vector<A> array;
  mutable std::vector<char> toggle;
};

template<class A>
class MemoizeArray : public Array<int,A>
{
public:
  MemoizeArray(int count, Array<int,A> &func) : func(func), array(count), toggle(count), count(count) 
  {
    for(int i=0;i<count;i++) toggle[i]=false;
  }

  A Index(int val) const
  { 
    if (toggle[val]==false)
      {
	toggle[val]=true;
	array[val]=func.Index(val);
      }
    return array[val];
  }
  int Size() const { return count; }
private:
  Array<int, A> &func;
  mutable std::vector<A> array;
  mutable std::vector<char> toggle;
  int count;
};




class MemoizeFaces : public FaceCollection
{
public:
  MemoizeFaces(FaceCollection &c) : c(c), numfaces(-1) { }
  MemoizeFaces(const MemoizeFaces &c) : c(c.c), numfaces(c.numfaces), numpoints(c.numpoints), facepoint(c.facepoint), color(c.color), normal(c.normal), facetexture(c.facetexture)
  {
  }
  void operator=(const MemoizeFaces &c)
  {
    numfaces=c.numfaces;
    numpoints=c.numpoints;
    facepoint=c.facepoint;
    color = c.color;
    normal=c.normal;
    facetexture = c.facetexture;
  }
  void Reset()
  {
    numfaces=-1;
    numpoints = std::vector<int>(1);
    facepoint = std::map<FP,Point>();
    color = std::map<FP,unsigned int>();
    normal = std::map<FP,Vector>();
    facetexture = std::map<int,int>();
  }
  void MemoizeAll()
  {
    int faces = NumFaces();
    for(int i=0;i<faces;i++)
      {
	int points = NumPoints(i);
	for(int j=0;j<points;j++)
	  {
	    FacePoint(i,j);
	    Color(i,j);
	    TexCoord(i,j);
	    PointNormal(i,j);
	  }
      }
  }
  int NumFaces() const 
  {
    if (numfaces==-1) 
      {
	numfaces=c.NumFaces();
	if (numfaces > 0)
	  numpoints.resize(numfaces);
	return numfaces;
      }
    return numfaces;
  }
  int NumPoints(int face) const
  {
    int i = numpoints[face];
    if (i==0)
      {
	numpoints[face]=c.NumPoints(face);
	return numpoints[face];
      }
    return i;
  }
  Point FacePoint(int face, int point) const
  {
    FP fp;
    fp.face = face;
    fp.point = point;
    std::map<FP,Point>::iterator i = facepoint.find(fp);
    if (i==facepoint.end())
      {
	Point p;
	p=(facepoint[fp]=c.FacePoint(face, point));
	return p;
      }
    return i->second;
  }
  unsigned int Color(int face, int point) const
  {
    FP fp;
    fp.face = face;
    fp.point = point;
    std::map<FP,unsigned int>::iterator i = color.find(fp);
    if (i==color.end())
      {
	unsigned int p;
	p=(color[fp]=c.Color(face, point));
	return p;
      }
    return i->second;

  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return c.TexCoord(face,point);
  }

  Vector PointNormal(int face, int point) const
  {
    FP fp;
    fp.face = face;
    fp.point = point;
    std::map<FP,Vector>::iterator i = normal.find(fp);
    if (i==normal.end())
      {
	Vector p;
	p=(normal[fp]=c.FacePoint(face, point));
	return p;
      }
    return i->second;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return c.Attrib(face,point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return c.AttribI(face,point,id);
  }
  virtual int NumTextures() const { return c.NumTextures(); }
  virtual void GenTexture(int num) { c.GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return c.TextureBuf(num); }

  virtual int FaceTexture(int face) const
  {
    std::map<int,int>::iterator i = facetexture.find(face);
    if (i==facetexture.end())
      {
	int p;
	p = (facetexture[face]=c.FaceTexture(face));
	return p;
      }
    return i->second;
  }

private:
    struct FP 
    {
      int face;
      int point;
      friend bool operator<(const FP &a, const FP &b) 
      { 
	if (a.face<b.face) return true;
	else if (b.face < a.face) return false;
	
	if (a.point<b.point) return true;
	else if (b.point<a.point) return false;
	return false;
	
      }
    };
  FaceCollection &c;
  mutable int numfaces;
  mutable std::vector<int> numpoints;
  mutable std::map<FP, Point> facepoint;
  mutable std::map<FP, unsigned int> color;
  mutable std::map<FP, Vector> normal;  
  mutable std::map<int, int> facetexture;
};

class PreCalcThread : public FaceCollection, public Effect
{
public:
  int NumFaces() const
  {
    pthread_mutex_lock(&start_lock);
    pthread_mutex_unlock(&start_lock);

    pthread_mutex_lock(&mutex);
    int t = Map(time).NumFaces();
    pthread_mutex_unlock(&mutex);
    return t;
  }
  int NumPoints(int face) const
  {
    pthread_mutex_lock(&start_lock);
    pthread_mutex_unlock(&start_lock);

    pthread_mutex_lock(&mutex);
    int t = Map(time).NumPoints(face);
    pthread_mutex_unlock(&mutex);
    return t;
  }
  Point FacePoint(int face, int point) const
  {
    pthread_mutex_lock(&start_lock);
    pthread_mutex_unlock(&start_lock);

    pthread_mutex_lock(&mutex);
    Point p = Map(time).FacePoint(face,point);
    pthread_mutex_unlock(&mutex);
    return p;
  }
  Vector PointNormal(int face, int point) const
  {
    pthread_mutex_lock(&start_lock);
    pthread_mutex_unlock(&start_lock);

    pthread_mutex_lock(&mutex);
    Vector v = Map(time).PointNormal(face,point);
    pthread_mutex_unlock(&mutex);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

  const MemoizeFaces &Map(float time) const
  {
    SawWaveform saw;
    FitWaveform fit(saw, 150.0, 0.0, (float)framelist.size());
    RepeatWaveform rep(fit, 50);
    const MemoizeFaces &ref = framelist[(int)rep.Index(time)];
    return ref;
  }

  static void* precalc(void *arg)
  {
    PreCalcThread *t = (PreCalcThread*) arg;
    float time = 0.0;
    bool first = true;
    while(time <1000.0)
      {
	t->eff.ChangeTime(time);
	time+=100.0;
	MemoizeFaces m(t->fc);
	int num = m.NumFaces();
	for(int i=0;i<num;i++)
	  {
	    int p = m.NumPoints(i);
	    for(int ii=0;ii<p;ii++)
	      {
		m.FacePoint(i,ii);
		m.PointNormal(i,ii);
	      }
	  }
	pthread_mutex_lock(&t->mutex);
	t->framelist.push_back(m);
	pthread_mutex_unlock(&t->mutex);
	if (first) { pthread_mutex_unlock(&t->start_lock); first=false; }
      }
    return 0;
  }
  float Length() const { return eff.Length(); }
  void ChangeTime(float t) { time = t; }
  PreCalcThread(FaceCollection &fc, Effect &eff) : fc(fc), eff(eff) 
  {
    pthread_mutex_init(&mutex, 0);
    pthread_mutex_init(&start_lock, 0);
    pthread_mutex_lock(&start_lock);
    pthread_create(&thread, 0, &precalc, this);    
  }
  ~PreCalcThread()
  {
    pthread_mutex_destroy(&mutex);
    //pthread_destroy(&thread);
  }
private:
  FaceCollection &fc;
  Effect &eff;
  pthread_t thread;
  mutable pthread_mutex_t mutex;
  mutable pthread_mutex_t start_lock;
  std::vector<MemoizeFaces> framelist;
  float time;
};



class RemovePolys : public Function<int, bool>
{
public:
  RemovePolys(BoxableFaceCollection &obj, BoxableFaceCollection &obj2, FaceType test=EDoNotDraw) : func(obj,obj2), equal(test), compose(func, equal) { }
  bool Index(int face) const { return compose.Index(face); }
private:
  RemovePolysFunc func;
  IsEqualFunc<FaceType> equal;
  ComposeFunc<int, FaceType, bool> compose;
};
class IntersectFaces : public FaceCollection
{
public:
  IntersectFaces(BoxableFaceCollection &obj1, BoxableFaceCollection &obj2) : func(obj1, obj2), isequal(EIntersect), compose(func, isequal), filter(obj1, compose), memo(filter)  { }

  virtual int NumFaces() const { return memo.NumFaces(); }
  virtual int NumPoints(int face) const { return memo.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return memo.FacePoint(face,point); }
  virtual unsigned int Color(int face, int point) const { return memo.Color(face, point); }
  virtual Point2d TexCoord(int face, int point) const { return memo.TexCoord(face, point); }
  virtual Vector PointNormal(int face, int point) const { return memo.PointNormal(face, point); }
  virtual float Attrib(int face, int point, int id) const { return memo.Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const
  {
    return memo.AttribI(face,point,id);
  }

private:
  RemovePolysFunc func;
  IsEqualFunc<FaceType> isequal;
  ComposeFunc<int, FaceType, bool> compose;
  FilterFaces filter;
  MemoizeFaces memo;
};


class AndNotElem : public BoxableFaceCollection
{
public:
  AndNotElem(BoxableFaceCollection &o1, BoxableFaceCollection &o2) : o1(o1), o2(o2), rem(o1, o2), mem(o1.NumFaces(), rem), rem2(o2,o1, EDraw), mem2(o2.NumFaces(), rem2), f1(o1, mem), f2(o2, mem2), iface(o1,o2), iface2(o2,o1), split1(iface, iface2, o2, true), split2(iface2, iface, o1, false), or_elem(&f1, &f2, &split1, &split2)
  {
  }
  void DoSplitting()
  {
    split1.DoSplitting();
    split2.DoSplitting();
  }
  int NumFaces() const { return or_elem.NumFaces(); }
  int NumPoints(int face) const { return or_elem.NumPoints(face); }
  Point FacePoint(int face, int point) const { return or_elem.FacePoint(face,point); }
  unsigned int Color(int face, int point) const { return or_elem.Color(face,point); }
  Point2d TexCoord(int face, int point) const { return or_elem.TexCoord(face,point); }
  Vector PointNormal(int face, int point) const { return or_elem.PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return or_elem.Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const
  {
    return or_elem.AttribI(face,point,id);
  }

  void SetBox(Matrix b) { /*o1.SetBox(b); o2.SetBox(b);*/ }
  bool Inside(Point p) const { return o1.Inside(p) && !o2.Inside(p); }
private:
  BoxableFaceCollection &o1, &o2;
  RemovePolys rem;
  MemoizeFunction<bool> mem;
  RemovePolys rem2;
  MemoizeFunction<bool> mem2;
  FilterFaces f1;
  FilterFaces f2;
  IntersectFaces iface;
  IntersectFaces iface2;
  SplitPolygons split1;
  SplitPolygons split2;
  OrElem<FaceCollection> or_elem;
};

class RectangleElem : public BoxableFaceCollection
{
public:
  RectangleElem(float x, float y) : x(x/2.0), y(y/2.0) { }
  int NumFaces() const { return 1; }
  int NumPoints(int face) const { return 4; }
  Point FacePoint(int face, int point) const
  {
    switch(point)
      {
      case 1: return Point(-x,-y,0.0);
      case 2: return Point(-x,y,0.0);
      case 3: return Point(x,y,0.0);
      case 0: return Point(x,-y,0.0);
      };
    return Point(0.0,0.0,0.0);
  }
  unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Vector PointNormal(int face, int point) const
  { 
    return Vector(1.0,0.0,0.0);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  float Attrib(int /*face*/, int point, int id) const
  {
    switch(id)
      {
      case AttrInternalX:
	switch(point)
	  {
	  case 0: return -1.0;
	  case 1: return -1.0;
	  case 2: return 1.0;
	  case 3: return 1.0;
	  };
      case AttrInternalY:
	switch(point)
	  {
	  case 0: return -1.0; 
	  case 1: return 1.0;
	  case 2: return 1.0;
	  case 3: return -1.0;
	  };
      };
    return 0.0;
  }
  void SetBox(Matrix b) { }
  bool Inside(Point p) const { return false; }
private:
  float x,y;
};

class RenderBox : public BoxableFaceCollection
{
public:
  RenderBox(int sx, int sy, int sz, float sizex, float sizey, float sizez) : sx(sx), sy(sy), sz(sz), sizex(sizex), sizey(sizey), sizez(sizez) { }
  int NumFaces() const { return sx+sy+sz; }
  int NumPoints(int face) const { return 4; }
  Point FacePoint(int face, int point) const
  {
    if (face < sx)
      {
	int face2 = face - 0;
	float pos_x = (float(face2)/sx)*2.0-1.0;
	float p_y_0 = -1.0;
	float p_y_1 = 1.0;
	float p_z_0 = -1.0;
	float p_z_1 = 1.0;
	float p_y;
	float p_z;
	if (point == 0) { p_y = p_y_0; p_z = p_z_0; }
	if (point == 1) { p_y = p_y_1; p_z = p_z_0; }
	if (point == 2) { p_y = p_y_1; p_z = p_z_1; }
	if (point == 3) { p_y = p_y_0; p_z = p_z_1; }
	return InternalToVisible(Point(pos_x, p_y, p_z));
      }
    else if (face < sx+sy)
      {
	int face2 = face - sx;
	float p_x_0 = -1.0;
	float p_x_1 = 1.0;
	float pos_y = 2.0*float(face2)/sy-1.0;
	float p_z_0 = -1.0;
	float p_z_1 = 1.0;
	float p_x;
	float p_z;
	if (point == 0) { p_x = p_x_0; p_z = p_z_0; }
	if (point == 1) { p_x = p_x_1; p_z = p_z_0; }
	if (point == 2) { p_x = p_x_1; p_z = p_z_1; }
	if (point == 3) { p_x = p_x_0; p_z = p_z_1; }
	return InternalToVisible(Point(p_x, pos_y, p_z));
      }
    else 
      {
	int face2 = face - sy - sx;
	float p_x_0 = -1.0;
	float p_x_1 = 1.0;
	float p_y_0 = -1.0;
	float p_y_1 = 1.0;
	float pos_z = 2.0*float(face2)/sz-1.0;
	float p_x;
	float p_y;
	if (point == 0) { p_x = p_x_0; p_y = p_y_0; }
	if (point == 1) { p_x = p_x_1; p_y = p_y_0; }
	if (point == 2) { p_x = p_x_1; p_y = p_y_1; }
	if (point == 3) { p_x = p_x_0; p_y = p_y_1; }
	return InternalToVisible(Point(p_x, p_y, pos_z));
      }
  }
  Point InternalToVisible(Point p) const
  {
    p.x *= sizex;
    p.y *= sizey;
    p.z *= sizez;
    return p;
  }
  Point VisibleToInternal(Point p) const
  {
    p.x /= sizex;
    p.y /= sizey;
    p.z /= sizez;
    return p;
  }

  unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Vector PointNormal(int face, int point) const
  { 
    return Vector(1.0,0.0,0.0);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

  float Attrib(int face, int point, int id) const
  {
    Point p = FacePoint(face,point);
    Point p2 = VisibleToInternal(p);

    float val=0.0;
    switch(id)
      {
      case AttrInternalX: val = p2.x; break;
      case AttrInternalY: val = p2.y; break;
      case AttrInternalZ: val = p2.z; break;
      case AttrInternalObjX: 
	if (point == 0) { val = -1.0; }
	if (point == 1) { val = -1.0; }
	if (point == 2) { val = 1.0; }
	if (point == 3) { val = 1.0; }
	break;
      case AttrInternalObjY: 
	if (point == 0) { val = -1.0; }
	if (point == 1) { val = 1.0; }
	if (point == 2) { val = 1.0; }
	if (point == 3) { val = -1.0; }
	break;
      };

    return val;
  }
  void SetBox(Matrix b) { }
  bool Inside(Point p) const { return false; }
private:
  int sx,sy,sz;
  float sizex, sizey, sizez;
};

class SphereElem : public BoxableFaceCollection
{
public:
  SphereElem(int numfaces, int numfaces2) :  numfaces(numfaces), numfaces2(numfaces2), center(Point(0.0,0.0,0.0)), radius(1.0) { }
  SphereElem(Point center, float radius, int numfaces1, int numfaces2) : numfaces(numfaces1), numfaces2(numfaces2), center(center), radius(radius) { }
  virtual void SetBox(Matrix b) { /*box = b; inverted=false; */}
  virtual int NumFaces() const { return (numfaces*2)*numfaces2; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const;
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual bool Inside(Point p) const 
  {
    p-=center;
    Vector v = p;
    //if (!inverted) { boxinv=Matrix::Inverse(box); inverted=true; }
    Point pp = v;
    
    return pp.x*pp.x+pp.y*pp.y+pp.z*pp.z<radius*radius; 
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Vector v = FacePoint(face, point);
    Point p = v;
    p-=center;
    Vector vv = p;
    //Vector center = Vector(box.matrix[3], box.matrix[7], box.matrix[11]);
    return vv; /*-center*/;
  }
private:
  //Matrix box;
  //mutable Matrix boxinv;
  int numfaces;
  int numfaces2;
  Point center;
  float radius;
  //mutable bool inverted;
};
typedef FunctionImplT2<BoxableFaceCollection*, int, int, SphereElem> SphereElemFunction;

class ConeElem : public BoxableFaceCollection
{
public:
  ConeElem(int numfaces, Point p1, Point p2, float rad1, float rad2)
    : numfaces(numfaces), p1(p1), p2(p2), rad1(rad1), rad2(rad2), lp(p1,p2), pl1(lp.PlaneFromLine(0.0, 0.0, 1.0)), pl2(lp.PlaneFromLine(1.0,0.0,1.0)) { }
  ConeElem(int numfaces, Point p1, Point p2, float rad1, float rad2, Plane pl1, Plane pl2) :  numfaces(numfaces), p1(p1), p2(p2), rad1(rad1), rad2(rad2), lp(p1, p2), pl1(pl1), pl2(pl2) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return numfaces; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const;
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }

  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual bool Inside(Point p) const 
  {
    return false;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Point p;
    switch(point)
      {
      case 0: case 3: p=p1; break;
      case 1: case 2: p=p2; break;
      };
    Vector v = FacePoint(face, point);
    Vector center = p;
    return v-center;
  }
private:
  //Matrix box;
  int numfaces;
  Point p1, p2;
  float rad1, rad2;
  LineProperties lp;
  Plane pl1, pl2;
};
typedef FunctionImplT2<BoxableFaceCollection*, int, int, SphereElem> SphereElemFunction;



class TransformPoints : public PointCollection
{
public:
  TransformPoints(const PointCollection &coll, Matrix m) : coll(coll), m(m) { }
  int Size() const { return coll.Size(); }
  Point Index(int i) const
  {
    return coll.Index(i)*m;
  }
private:
  const PointCollection &coll;
  Matrix m;
};
typedef FunctionImpl1<PointCollection*, PointCollection*, Matrix, TransformPoints> TransformPointsFunction;

class PointsGrid : public GridIn3d
{
public:
  PointsGrid(const PointVectorCollection &points, const BoxCollection &boxes) : points(points), boxes(boxes) { }
  virtual Point Index(int x, int y) const
  {
    Point p = points.Index(x).first;
    Matrix m = boxes.Index(y);
    return p*m;
  }
  virtual Point2d Texture(int x, int y) const { Point2d p; p.x = 0.0; p.y=0.0; return p; }
  virtual unsigned int Color(int x, int y) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual Vector Normal(int x, int y) const
  {
    Vector v = points.Index(x).second;
    Matrix m = boxes.Index(y);
    return v*m;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual float Attrib(int x, int y, int id) const { return 0.0; }
  virtual int XSize() const { return points.Size(); }
  virtual int YSize() const { return boxes.Size(); }
private:
  const PointVectorCollection &points; // x
  const BoxCollection &boxes; // y
};

class Render
{
public:
  virtual void SetTime(float time)=0;
  virtual void SetMatrix(Matrix m)=0;
  virtual void RenderFaces(const FaceCollection &col)=0;
  virtual void RenderFaces(const FaceCollection &col, const TextureCoords &tex)=0;
  virtual void RenderFacesTexture(const FaceCollection &col)=0;

  virtual void RenderLines(const LineCollection &col)=0;
  virtual void RenderPoly(const PointCollection &col)=0;
  virtual void* CreateState()=0;
  virtual void UpdateVBO(const FaceCollection &faces, void *state)=0;
  virtual void DrawVBO(void *state)=0;
  virtual void DestroyState(void *state)=0;

  virtual void Show() { }
};

class SDLRender : public Render
{
public:
  void SetTime(float time);
  void SetMatrix(Matrix m);
  void RenderFaces(const FaceCollection &col);
  void RenderFaces(const FaceCollection &col, const TextureCoords &tex);
  void RenderFacesTexture(const FaceCollection &col) 
  {
      FaceCollectionTextureCoords coords(col);
      RenderFaces(col, coords); 
  }
  void RenderLines(const LineCollection &col);
  void RenderPoly(const PointCollection &col);
  void* CreateState();
  void UpdateVBO(const FaceCollection &faces, void *state);
  void DrawVBO(void *state);
  void DestroyState(void *state);
};

class RenderState
{
public:
  RenderState(Render &r) : r(r) { state = r.CreateState(); }
  void *GetState() const { return state; }
  ~RenderState() { r.DestroyState(state); }
private:
  Render &r;
  void *state;
};

void TextureGeneration(FaceCollection &coll, FrameAnimPlugins &plugins);
void RenderOpenGl(const FaceCollection &col);
void RenderOpenGlTextures(const FaceCollection &col);
void RenderOpenGl(const FaceCollection &col, Function<int, bool> &removepolys);
void RenderOpenGl(const FaceCollection &col, ElementCollection &nums);
void RenderOpenGl(const FaceCollection &col, const BoxCollection &b, Boxable &elem);

void RenderOpenGl(const FaceCollection &col, const TextureCoords &tex);
void RenderOpenGl(const FaceCollection &col, ElementCollection &nums, const TextureCoords &tex);
void RenderOpenGl(const FaceCollection &col, const BoxCollection &b, Boxable &elem, const TextureCoords &tex);

void RenderOpenGlLines(const LineCollection &col);
void RenderOpenGlLines(const LineCollection &col, ElementCollection &nums);
void RenderOpenGlLines(const LineCollection &col, const BoxCollection &b, Boxable &elem);

void RenderOpenGlPoly(const PointCollection &col);
void RenderOpenGl(const LineCollection &col);
void RenderOpenGl(const PointCollection &col);

template<class T>
void RenderOpenGl(const Array<int, T*> &array)
{
  int count = array.Size();
  for(int i=0;i<count;i++)
    {
      RenderOpenGl(*array.Index(i));
    }
}


class BoxCombine : public virtual ElementCollection
{
public:
  BoxCombine(const BoxCollection &b, Boxable &elem) : b(b), elem(elem) { }

  virtual int NumElements() const { return b.NumBoxes(); }
  virtual void SetElemId(int id) { elem.SetBox(b.BoxIndex(id)); }

private:
  const BoxCollection &b;
  Boxable &elem;
};

class AnimBox : public virtual BoxCollection, public virtual Effect
{
public:
  AnimBox(const BoxCollection &c, const BoxCollection &c2, float length) : c(c), c2(c2), length(length) { }
  int Size() const { return c.NumBoxes(); }
  float Length() const { return length; }
  void ChangeTime(float t) { time = t; }
  Matrix Index(int box) const;

private:
  const BoxCollection &c;
  const BoxCollection &c2;
  float time, length;
};

class CubeGrid : public virtual BoxCollection
{
public:
  CubeGrid(int x, int y, int z, float dx, float dy, float dz, Matrix b) 
    : x(x), y(y), z(z),
      dx(dx), dy(dy), dz(dz), b(b)
  { }

  virtual int Size() const { return x*y*z; }
  virtual Matrix Index(int box) const;


private:
  int x,y,z;
  float dx,dy,dz;
  Matrix b;
};
class MatrixCollection : public virtual BoxCollection
{
public:
  template<class It>
  MatrixCollection(It beg, It end) : cont(beg, end) { }
  virtual int Size() const
  {
    return cont.size();
  }
  virtual Matrix Index(int box) const {
    return cont[box];
  }
private:
  std::vector<Matrix> cont;

};

class CombineBoxCollection : public BoxCollection
{
public:
  CombineBoxCollection(BoxCollection &c, BoxCollection &c2) : c(c), c2(c2) { }
  int NumBoxes() const { return c.NumBoxes()*c2.NumBoxes(); }
  Matrix BoxIndex(int box) const 
  {
    int bb = box / c2.NumBoxes();
    int bb2 = box - (bb*c2.NumBoxes());
    //std::cout << bb << " " << bb2 << std::endl;
    return c.BoxIndex(bb)*c2.BoxIndex(bb2);
  }
private:
  BoxCollection &c, &c2;
};

class ArrayBox : public virtual BoxCollection
{
public:
  template<class It>
  ArrayBox(It beg, It end) : cont(beg, end) { }
  ArrayBox() { }
  void push_back(BoxCollection* c) { cont.push_back(c); }
  virtual int Size() const
  { 
    int count = 0;
    for(std::vector<BoxCollection*>::const_iterator i=cont.begin(); i!=cont.end();i++)
      count += (*i)->NumBoxes();
    return count;
  }
  virtual Matrix Index(int box) const {
    int count = 0;
    for(std::vector<BoxCollection*>::const_iterator i=cont.begin(); i!=cont.end();i++)
      {
	int oldcount = count;
	count += (*i)->NumBoxes();
	if (count > box) { return (*i)->BoxIndex(box-oldcount); }
      }
    Matrix b = Matrix::Identity();
    return b;
  }
  
private:
  std::vector<BoxCollection*> cont;
};

class MatrixBox : public virtual BoxCollection
{
public:
  MatrixBox(BoxCollection &c, Matrix m) : m(m), c(c) { }
  virtual int Size() const { return c.NumBoxes(); }
  virtual Matrix Index(int box) const
  {
    Matrix b = c.BoxIndex(box);
    return b*m; 
  }
private:
  Matrix m;
  BoxCollection &c;
};
typedef FunctionImpl1<BoxCollection*, BoxCollection*, Matrix, MatrixBox> MatrixBoxFunction;

class UnitBox : public virtual BoxCollection
{
public:
  UnitBox() 
  { 
    b=Matrix::Identity();
  }
  UnitBox(Matrix m) { b=m; }
  virtual int Size() const { return 1; }
  virtual Matrix Index(int box) const { return b; }
private:
  Matrix b;
};

class SampleGrid : public FaceCollection, public TextureCoords
{
public:
  SampleGrid(GridIn3d &grid, int texture) : grid(grid), texture(texture) { }
  int Texture(int face) const { return texture; }
  Point2d TexCoord(int face, int point) const
  {
    std::pair<int,int> p = Sample(face,point);
    Point2d pp = grid.Texture(p.first,p.second);
    return pp;
  }
  int NumFaces() const { return grid.XSize()*grid.YSize(); }
  int NumPoints(int face) const { return 4; }
  Point FacePoint(int face, int point) const
  {
    std::pair<int,int> p = Sample(face, point);
    Point pp = grid.Index(p.first, p.second);
    return pp;
  }
  unsigned int Color(int face, int point) const
  {
    std::pair<int,int> p = Sample(face, point);
    unsigned int color = grid.Color(p.first, p.second);
    return color;
  }
  Vector PointNormal(int face, int point) const
  {
    std::pair<int,int> p = Sample(face,point);
    Vector pp = grid.Normal(p.first, p.second);
    return pp;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    std::pair<int,int> p = Sample(face,point);
    int pp = grid.AttribI(p.first, p.second, id);
    return pp;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    std::pair<int,int> p = Sample(face,point);
    float pp = grid.Attrib(p.first, p.second, id);
    return pp;
  }
private:
  std::pair<int,int> Sample(int face, int point) const
  {
    int yindex = face/grid.XSize();
    int ddy = face - yindex*grid.XSize();
    int xindex = ddy;
    if (point==1 || point==2) xindex++;
    if (point==2 || point==3) yindex++;
    //if (xindex > grid.XSize()-1) xindex=grid.XSize()-1;
    //if (yindex > grid.YSize()-1) yindex=grid.YSize()-1;
    xindex = xindex % grid.XSize();
    yindex = yindex % grid.YSize();
    return std::make_pair(xindex, yindex);
  }
private:
  GridIn3d &grid;
  int texture;
};
typedef FunctionImpl1<GridIn3d*, FaceCollection*, int, SampleGrid> SampleGridFunction;

class DiffVectorCollection : public VectorCollection
{
public:
  DiffVectorCollection(PointCollection &points, bool loops) : points(points), loops(loops) { }
  int Size() const { if (loops) return points.Size()+1; return points.Size(); }
  Vector Index(int index) const 
  { 
    if (index==Size()-1 && loops) return points.Index(0)-points.Index(index-1); 
    if (index==0) return Vector(0.0,0.0,0.0); 
    return points.Index(index)-points.Index(index-1); 
  }
private:
  PointCollection &points;
  bool loops;
};

class PointVectorInterpolate : public PointCollection
{ // to be used with pointvectorrepeat
  // pos = [0..1]
public:
  PointVectorInterpolate(PointVectorCollection &coll, float pos) : coll(coll), pos(pos) { }
  int Size() const { return coll.Size(); }
  Point Index(int i) const
  {
    std::pair<Point,Vector> p = coll.Index(i);
    return p.first + p.second*pos;
  }
private:
  PointVectorCollection &coll;
  float pos;
};

class PointVectorRepeat : public PointVectorCollection
{
public:
  PointVectorRepeat(CurveIn2d &curve, Plane pl, int numobjects, bool loops) : curve3d(curve, pl), sample3d(curve3d, numobjects-1), diff3d(sample3d, loops) { }
  int Size() const
  {
    return sample3d.Size();
  }
  std::pair<Point,Vector> Index(int i) const
  {
    Point p = sample3d.Index(i);
    Vector v = diff3d.Index(i+1);
    return std::make_pair(p,v);
  }

private:
  PlaneCurveIn3d curve3d;
  SampleCurve3d sample3d;
  DiffVectorCollection diff3d;
};


class SpanObject : public GridIn3d
{
public:
  SpanObject(PointCollection &points, PointCollection &points2, MatrixCurve &contobject) : points(points), vectors(diff), d(vectors), s(d, 0.0), contobject(contobject), id(1.0), diff(points2, true), points2(points2), dist(d.Sum()) { }
  SpanObject(PointCollection &points, PointCollection &points2) : points(points), vectors(diff), d(vectors), s(d, 0.0), contobject(id), id(1.0), diff(points2, true), points2(points2), dist(d.Sum()) { }
  Point Index(int x, int y) const 
  {
    Point p = points.Points(x);
    //Matrix mm = Matrix::Identity();
    p += Vector(points2.Points(y)-points2.Points(0.0));
    //for(int yy=0;yy<y;yy++)
    //{
    //	p += vectors.Vectors(yy);
	//mm = contobject.Index(s.Index(yy))*mm;
    //  }
    Matrix m = contobject.Index(s.Index(y)/dist*contobject.Size());
    return p*m;
  }
  Point2d Texture(int x, int y) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  unsigned int Color(int x, int y) const
  {
    return Color::White().Pixel();
  }
  Vector Normal(int x, int y) const
  {
    Point p = Index(x,y);
    Point p2 = Index((x+1) % XSize(),y);
    Point p3 = Index(x,(y+1) % YSize());
    return -Vector::CrossProduct(p2-p, p3-p);
  }


  virtual float Attrib(int x, int y, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int x, int y, int id) const
  {
    return 0;
  }
  int XSize() const
  {
    return points.NumPoints();
  }
  int YSize() const
  {
    return vectors.NumVectors();
  }
private:
  PointCollection &points;
  VectorCollection &vectors;
  Distance d;
  WeightedSampling s;
  const MatrixCurve &contobject;
  IdentityMatrixCurve id;
  std::vector<Vector> v;
  DiffVectorCollection diff;
  PointCollection &points2;
  float dist;
};

#if 0
class MoveInPair : public Function<std::pair<Point,Point>, std::pair<Point,Point> >
{
public:
  MoveSecondInPair(Vector firstmove, Vector secondmove) : firstmove(firstmove), secondmove(secondmove) { }
  std::pair<Point,Point> Index(std::pair<Point,Point> p)
  {
    return std::make_pair(p.first + firstmove, p.second + secondmove);
  }
};
#endif

#if 0
class SpanObject2
{
private:
  SpanObject2(PointCollection &points, Vector v1, Vector v2) : distfunc(v1,v2), pointpair(points, points), distance(pointpair, distfunc) { }

  
  MoveInPair distfunc;
  ArrayPair<int, int, Point, Point> pointpair;
  ComposeArrays<int, std::pair<Point,Point>, std::pair<Point,Point> > distance;
};
#endif

class RingElem : public BoxableFaceCollection
{
public:
  RingElem(const PointVectorCollection &points, float x, int steps)
    : mcurve(x), sample(mcurve, steps), grid(points, sample), faces(grid, 0)
  { }
  void SetBox(Matrix m) { }
  virtual int NumFaces() const { return faces.NumFaces(); }
  virtual int NumPoints(int face) const { return faces.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return faces.FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { return faces.PointNormal(face,point); }
  virtual unsigned int Color(int face, int point) const { return faces.Color(face, point); }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual float Attrib(int face, int point, int id) const { return faces.Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const
  {
    return faces.AttribI(face,point,id);
  }

private:
  CircleMatrixCurve mcurve;
  SampleMatrixCurve sample;
  PointsGrid grid;
  SampleGrid faces;
};
typedef FunctionImpl2<PointVectorCollection*, BoxableFaceCollection*, float, int, RingElem> RingElemFunction;


class Blob
{
public:
  // x_0 = center of sphere1, x_1 = surface point of sphere1, x_2 = center of spehere2
  Point line(float t, Point x_0, Point x_1) { return x_0+Vector(x_1-x_0)*t; }
  float f(Point p, Point x_0, Point x_1, Point x_2) { return ((p.x-x_0.x)*(p.x-x_0.x)+(p.y-x_0.y)*(p.y-x_0.y)+(p.z-x_0.z)*(p.z-x_0.z)); }
  float g(Point p, Point x_0, Point x_1, Point x_2) { return ((p.x-x_2.x)*(p.x-x_2.x)+(p.y-x_2.y)*(p.y-x_2.y)+(p.z-x_2.z)*(p.z-x_2.z)); }
  
  float h(Point p, Point x_0, Point x_1, Point x_2, float r)
  {
    float fp = f(p, x_0, x_1, x_2);
    float gp = g(p, x_0, x_1, x_2);
    if (fp < 1.0) fp = 1.0;
    if (gp < 1.0) gp = 1.0;
    return 1.0/(fp)+1.0/(gp)-r;
  }
  
  //float h(Point p) { return 1.0*exp(-3.0*f(p))+1.0*exp(-3.0*g(p)) - r; }
  float H(float tt, Point x_0, Point x_1, Point x_2, float r) 
  { 
    return h(line(tt, x_0, x_1), x_0, x_1, x_2, r); 
  }
#if 0
double FalsiMethod(double s, double t, double e, int m)
{
    int n,side=0;
    double r=t,fr,fs = H(s),ft = H(t);
 
    for (n = 1; n <= m; n++)
    {
        r = (fs*t - ft*s) / (fs - ft);
        if (fabs(t-s) < e*fabs(t+s)) break;
        fr = H(r);
 
        if (fr * ft > 0)
        {
          t = r; ft = fr;
          if (side==-1) fs /= 2;
          side = -1;
        }
        else if (fs * fr > 0)
        {
          s = r;  fs = fr;
          if (side==+1) ft /= 2;
          side = +1;
        }
        else break;
    }
    return r;
}
double SecantMethod(double xn_1, double xn, double e, int m)
{
    int n;
    double d;
    for (n = 1; n <= m; n++)
    {
        d = (xn - xn_1) / (H(xn) - H(xn_1)) * H(xn);
        if (fabs(d) < e)
            return xn;
        xn_1 = xn;
        xn = xn - d;
    }
    return xn;
}
#endif

  float t(float t_0, float t_1, Point x_0, Point x_1, Point x_2, float r)
  {
    //return SecantMethod(t_0, t_1, 0.001, 100);
    //std::cout << "Start" << std::endl;
    float t;
    t=t_0;
    for(;t<t_1;)
      {
	float Ht = H(t, x_0, x_1, x_2, r);
	if (Ht<0.1) return t;
	t+=Ht/20.0;
      }
    //std::cout << "(val,t_0) =" << val  << "," << t_0 << std::endl;

    return t_0;
  }
  // this returns replacement for x_1
  Point adjustpoint(Point x_0, Point x_1, Point x_2, float r)
  {
    float tt = t(1.0,5.0, x_0, x_1, x_2, r);
    return line(tt, x_0, x_1);
  }
};

class Ray : public Function<float, Point>
{
public:
  Ray(Point p, Vector v) : p(p), v(v) { }
  Point Index(float x) const 
  {
    //std::cout << "Ray: " << p << " " << x << " " << v << std::endl;
    Point pp = p + x*v;
    //std::cout << "Ray: " << x << " " << pp << std::endl;
    return pp; 
  }
private:
  Point p;
  Vector v;
};
template<class T>
class SolvableCompose : public Function<float, float>
{
public:
  SolvableCompose(Function<float, T> &curve,
		  Function<T, float> &value)
    : compose(curve, value) { }
  float Index(float x) const { return compose.Index(x); }
private:
  FunctionCompose<float, T, float> compose;
};
typedef SolvableCompose<Point> SolvableComposePoint;

float Solve(const Function<float, float> &func, float t_0, float t_1);
float SolveWithFailure(const Function<float, float> &func, float t_0, float t_1, bool &success);

class Packing : public PointCollection2d
{
public:
  Packing(int num, float dist) : num(num), dist(dist) { }
  int Size() const { return num; }
  Point2d Index(int i) const
  {
    if (i==0) 
      {
	Point2d p;
	p.x=0.0;
	p.y=0.0;
	return p;
      }
    if (i==1)
      {
	return s(Index(0));
      }
    return k(Index(i-1), Index(i-2));
  }
  
  
  
public:
  Point2d k(Point2d p, Point2d p2) const
  {
    float dist = (p2.x-p.x)*(p2.x-p.x)+(p2.y-p.y)*(p2.y-p.y);
    Circle c(p, dist);
    Point2d pp = c.Index(60.0*3.14159*2.0/360.0);
    return pp;
#if 0
    float dist = (p2.x-p.x)*(p2.x-p.x)+(p2.y-p.y)*(p2.y-p.y);
    Circle c(p,dist);
    Distance2d d(p2);
    FloatMove m(dist);
    FunctionCompose<float, Point2d, float> comp1(c,d);
    FunctionCompose<float, float, float> comp2(comp1, m);
    float t = Solve(comp2, 0.0, 2.0*3.14159);
    Point2d p3 = c.Index(t);
    return p3;
#endif
  }
  Point2d s(Point2d p) const
  {
    p.x += dist;
    return p;
  }
private:
  int num;
  float dist;
};
typedef FunctionImplT2<PointCollection2d*, int, int, Packing> PackingFunction;

class PointMap : public Function<Point,Point>
{
public:
  virtual ~PointMap() { }
  Point Index(Point p) const { return Convert(p); }
  virtual Point Convert(Point p) const=0;
};
class MatrixPointMap : public PointMap
{
public:
  MatrixPointMap(Matrix m) : m(m) { }
  Point Convert(Point p) const
  {
    return p*m;
  }
private:
  Matrix m;
};

class MatrixFunction : public InvertableFunction<Point,Point>
{
public:
  MatrixFunction(Matrix m) : m(m) { }
  Point Index(Point p) const
  {
    return p*m;
  }
  std::vector<Point> Inverse(Point p) const
  {
    std::vector<Point> vec;
    vec.push_back(p*Matrix::Inverse(m));
    return vec;
  }
private:
  Matrix m;
};
class CX : public InvertableFunction<float, float>
{
public:
  CX(float c) : c(c) { }
  float Index(float x) const { return c*x; }
  std::vector<float> Inverse(float y) const
  {
    std::vector<float> v;
    v.push_back(y/c);
    return v;
  }
private:
  float c;
};

class XPA : public InvertableFunction<float, float>
{
public:
  XPA(float c) : c(c) { }
  float Index(float x) const { return x+c; }
  std::vector<float> Inverse(float y) const
  {
    std::vector<float> v;
    v.push_back(y-c);
    return v;
  }
private:
  float c;
};

class AMX : public InvertableFunction<float, float>
{
public:
  AMX(float c) : c(c) { }
  float Index(float x) const { return c-x; }
  std::vector<float> Inverse(float y) const
  {
    std::vector<float> v;
    v.push_back(c-y);
    return v;
  }
private:
  float c;
};



class BlobPointMap : public PointMap
{
public:
  BlobPointMap(Point center1, Point center2, float r) : center1(center1), center2(center2), r(r) { }
  Point Convert(Point p) const
  {
    Blob b;
    return b.adjustpoint(center1, p, center2,r);
  }
private:
  Point center1, center2;
  float r;
};

class ModifyObject : public BoxableFaceCollection
{
public:
  ModifyObject(BoxableFaceCollection &coll, PointMap &pm) : coll(coll), pm(pm) { }
  void SetBox(Matrix m) { /*coll.SetBox(m);*/ }
  int NumFaces() const { return coll.NumFaces(); }
  int NumPoints(int face) const { return coll.NumPoints(face); }
  Point FacePoint(int face, int point) const 
  { 
    return pm.Convert(coll.FacePoint(face,point));
  }
  unsigned int Color(int face, int point) const
  {
    return coll.Color(face, point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll.TexCoord(face,point);
  }

  Vector PointNormal(int face, int point) const
  {
    return coll.PointNormal(face,point);
  }
  float Attrib(int face, int point, int id) const
  {
    return coll.Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll.AttribI(face,point,id);
  }
private:
  BoxableFaceCollection &coll;
  PointMap &pm;
};
typedef FunctionImpl1<BoxableFaceCollection*, BoxableFaceCollection*, PointMap&, ModifyObject> ModifyObjectFunction;

class CubeEffect : public FrameAnim
{
public:
  CubeEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};
class CylinderEffect : public FrameAnimPlugins
{
public:
  CylinderEffect(Render *render);
  bool Frame(float time);
};
class SphereEffect : public FrameAnim
{
public:
  SphereEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};
class RoboEffect : public FrameAnim
{
public:
  RoboEffect(Render *render);
  ~RoboEffect();
  bool Frame(float time);
private:
  EmptyVertexPiece empty;
  GouraudPiece gouraud;
  PieceShader piece;
};
class MoleculeEffect : public FrameAnimPlugins
{
public:
  MoleculeEffect(Render *render);
  void Init();
  void Cleanup();
  void PreFrame(float time);
  bool Frame(float time);
private:
  unsigned int texture;
  GlobalTexturePiece globpiece;
  TexturePiece texturepiece;
  PhongPiece phongpiece;
  NormalPiece normalpiece;
  PieceShader shader;
};
class MoleculeInsideEffect : public FrameAnim
{
public:
  MoleculeInsideEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};
//class MoleculeInsideEffect2 : public Effect
//{
//public:
//  FaceCollection &Collection() const { return coll; }
//private:
//  FaceCollection &coll;
//};

class TitleEffect : public FrameAnim
{
public:
  TitleEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};
class TestEffect : public FrameAnim
{
public:
  TestEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
private:
  static bool first;
};
class TunnelEffect : public FrameAnim
{
public:
  TunnelEffect(Render *render) : FrameAnim(render) { }

  bool Frame(float time);
};

class HeightMapEffect : public FrameAnimPlugins
{
public:
  HeightMapEffect(Render *r);
  float XRot() const { return 0.0; }
  float YRot() const { return 1.0; }
  float ZRot() const { return 1.0*sin(40.0*3.14159*2.0/360.0); }
  bool Frame(float time);
private:
  unsigned int texture;
};

class PathEffect : public FrameAnim
{
public:
  PathEffect(Render *render) : FrameAnim(render) { }

  bool Frame(float time);
};


class VBORanges
{
public:
  virtual ~VBORanges() { }
  virtual int NumObjects()const =0;
  virtual int StartRange(int obj) const=0;
  virtual int EndRange(int obj) const=0;

  virtual int StartRangeIndex(int obj) const=0;
  virtual int EndRangeIndex(int obj) const=0;

};
class FaceCollectionVBO : public VBORanges
{
public:
  virtual int NumPoints() const = 0;
  //virtual int NumPoints(int face) const=0; // always returns 3.

  // (x,y,z)
  virtual void UpdateVertexList(int start, int end, float *array) const=0;
  // (nx, ny, nz)
  virtual void UpdateNormalList(int start, int end, float *array) const=0;
  // (tx, ty)
  virtual void UpdateTextureCoordList(int start, int end, float *array) const=0;
  // (r, g, b)
  virtual void UpdateColorList(int start, int end, char *array) const=0;
  virtual void UpdateAttrib(int start, int end, float *array, int id) const=0;
  virtual void UpdateAttribI(int start, int end, short *array, int id) const=0;

  // (i)
  virtual int IndexSize() const = 0;
  virtual void UpdateIndexList(int start, int end, int *array) const=0;
  //virtual Point FacePoint(int face, int point) const=0;
  //virtual Vector PointNormal(int face, int point) const=0;
  // returns face number
};
class VBO : public FaceCollectionVBO
{
public:
  VBO(FaceCollectionTriWithObjects &o) : o(o) { }
  virtual int NumPoints() const { return o.NumPoints(); }
  //virtual int NumPoints(int face) const=0; // always returns 3.

  // (x,y,z)
  virtual void UpdateVertexList(int start, int end, float *array) const
  {
    //std::cout << start << " " << end << std::endl;
    float *arr = array + start*3;
    for(int i=start;i<end;i++)
      {
	Point p = o.FacePoint(i);
	//std::cout << i << " " << p << std::endl;
	arr[0] = p.x;
	arr[1] = p.y;
	arr[2] = p.z;
	arr+=3;
      }
  }
  // (nx, ny, nz)
  virtual void UpdateNormalList(int start, int end, float *array) const
  {
    float *arr = array + start*3;
    for(int i=start;i<end;i++)
      {
	Vector v = o.PointNormal(i);
	arr[0] = v.dx;
	arr[1] = v.dy;
	arr[2] = v.dz;
	arr+=3;
      }
  }
  // (tx, ty)
  virtual void UpdateTextureCoordList(int start, int end, float *array) const
  {
    float *arr = array + start*2;
    for(int i=start;i<end;i++)
      {
	Point2d p = o.TexCoord(i);
	arr[0] = p.x;
	arr[1] = p.y;
	arr+=2;
      }
  }
  // (r, g, b)
  virtual void UpdateColorList(int start, int end, char *array) const
  {
    char *arr = array + start;
    for(int i=start;i<end;i++)
      {
	unsigned int v = o.Color(i);
	arr[0]=char(v); // r
	arr[1]=char(v>>8); // g
	arr[2]=char(v>>16); // b
	arr[3]=char(v>>24); // a
	arr+=4;
      }

  }
  virtual void UpdateAttrib(int start, int end, float *array, int id) const
  {
    float *arr = array + start;
    for(int i=start;i<end;i++)
      {
	float v = o.Attrib(i, id);
	//std::cout << i << " " << v << std::endl;
	arr[0]=v;
	arr+=1;
      }
  }
  virtual void UpdateAttribI(int start, int end, short *array, int id) const
  {
    short *arr = array + start;
    for(int i=start;i<end;i++)
      {
	int v = o.AttribI(i, id);
	//std::cout << i << " " << v << std::endl;
	arr[0]=v;
	arr+=1;
      }
  }
  // (i1,i2,i3)
  virtual int IndexSize() const { return o.NumFaces(); }
  virtual void UpdateIndexList(int start, int end, int *array) const
  {
    int *arr = array;
    for(int i=start;i<end;i++)
      {
	int i1 = o.Index(o.IndexToFace(i),0);
	int i2 = o.Index(o.IndexToFace(i),1);
	int i3 = o.Index(o.IndexToFace(i),2);
	//std::cout << "Index: " << i << " F" << o.IndexToFace(i) << ":" << i1 << " " << i2 << " " << i3 << std::endl;
	arr[0] = i1;
	arr[1] = i2;
	arr[2] = i3;
	arr+=3;
      }
  }
  //virtual Point FacePoint(int face, int point) const=0;
  //virtual Vector PointNormal(int face, int point) const=0;
  // returns face number
  virtual int NumObjects()const { return o.NumObjects(); }
  virtual int StartRange(int obj) const { return o.PointsStartRange(obj); }
  virtual int EndRange(int obj) const { return o.PointsEndRange(obj); }

  virtual int StartRangeIndex(int obj) const { return o.IndexStartRange(obj); }
  virtual int EndRangeIndex(int obj) const { return o.IndexEndRange(obj); }
private:
  FaceCollectionTriWithObjects &o;
};
typedef FunctionImpl0<FaceCollectionTriWithObjects*, FaceCollectionVBO*, VBO> VBOFunction;

enum VBOUpdate 
  { 
    UpdateVertex = 1, 
    UpdateNormal = 2, 
    UpdateTexture = 4, 
    UpdateColor = 8, 
    UpdateIndex = 16,
    UpdateAll = UpdateVertex|UpdateNormal|UpdateIndex,
    UpdateVertexNormal = UpdateVertex|UpdateNormal,
    UpdateVertexNormalIndex = UpdateVertex|UpdateNormal|UpdateIndex,
    UpdateVertexIndex = UpdateVertex | UpdateIndex,
    UpdateAllColor = UpdateAll | UpdateColor,
    UpdateAllTexture = UpdateAll|UpdateTexture
  };



class VBOStatePriv;
class VBOState
{
public:
  VBOState();
  ~VBOState();
private:
  VBOStatePriv *impl;
  friend void UpdateVBO(const FaceCollectionVBO &faces, int obj, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs);
  friend void UpdateVBO(const FaceCollectionVBO &faces, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs);
  friend void UpdateVBO(const FaceCollection &faces, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs);
  friend void DrawVBO(int obj, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs);
  friend void DrawVBO(VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs);
};


void UpdateVBO(const FaceCollectionVBO &faces, int obj, VBOState &state, VBOUpdate u=UpdateAll, const std::vector<Attrib> &attribs=std::vector<Attrib>());
void UpdateVBO(const FaceCollectionVBO &faces, VBOState &state, VBOUpdate u=UpdateAll, const std::vector<Attrib> &attribs=std::vector<Attrib>());
void UpdateVBO(const FaceCollection &faces, VBOState &state, VBOUpdate u=UpdateAll, const std::vector<Attrib> &attribs=std::vector<Attrib>());
void DrawVBO(int obj, VBOState &state, VBOUpdate u=UpdateAll, const std::vector<Attrib> &attribs=std::vector<Attrib>());
void DrawVBO(VBOState &state, VBOUpdate u=UpdateAll, const std::vector<Attrib> &attribs=std::vector<Attrib>());
void RenderOpenGlObjects(const BoxCollectionWithObject &coll, VBOState &vbostate, VBOUpdate u=UpdateVertexNormal);
void RenderOpenGlObjects(const ObjectDataArray &arr, VBOState &vbostat, VBOUpdate u, ShaderUse *use);



class ObjectDrawer
{
public:
  virtual int Count() const=0;
  virtual int ObjectId(int index) const=0;
  virtual Matrix ObjectPos(int index) const=0;
};

class Command
{
public:
  virtual void cmd()=0;
};
class DrawVBOCmd : public Command
{
public:
  DrawVBOCmd(FaceCollectionVBO &faces, int obj, VBOState &state, VBOUpdate u)
    : faces(faces), obj(obj), state(state), u(u) { }
  virtual void cmd()
  {
    DrawVBO(obj, state, u);
  }
private:
  FaceCollectionVBO &faces;
  int obj;
  VBOState &state;
  VBOUpdate u;
};
class MatrixVBOCmd : public Command
{
public:
  MatrixVBOCmd(Command &c, Matrix m) : c(c), m(m) { }
  virtual void cmd();

private:
  Command &c;
  Matrix m;
  
};

class LinkedObject
{
public:
  virtual int NumConnections() const =0;
  virtual Matrix Transform(int conn1, int conn2)=0;
};

class Camera
{
public:
  Camera(MatrixCurve &curve, float time);
  ~Camera();
};

class OpenGlCommands
{
public:
  virtual void PreCalc()=0;
  virtual void Execute()=0;
};
class FrameAnimCmds : public OpenGlCommands
{
public:
  FrameAnimCmds(FrameAnim &fa, float time) : fa(fa) { }
  void PreCalc() { fa.Init(); }
  void Execute() { fa.PreFrame(time); fa.Frame(time); }
private:
  FrameAnim &fa;
  float time;
};

class RenderToTexture : public OpenGlCommands
{
public:
  RenderToTexture(TexturePlugin *plugin, OpenGlCommands &cmds, bool in_precalc) : plugin(plugin), cmds(cmds), in_precalc(in_precalc) { }
  void PreCalc();
  void Execute();
private:
  TexturePlugin *plugin;
  OpenGlCommands &cmds;
  bool in_precalc;
};
class ColorCmds : public OpenGlCommands
{
public:
  ColorCmds(OpenGlCommands &cmds, Color c) : cmds(cmds), c(c) { }
  void PreCalc() { cmds.PreCalc(); }
  void Execute();
private:
  OpenGlCommands &cmds;
  Color c;
};
class TransparencyCmds : public OpenGlCommands
{
public:
  TransparencyCmds(OpenGlCommands &inner_cmds) : inner_cmds(inner_cmds) { }
  void PreCalc() { inner_cmds.PreCalc(); }
  void Execute();
private:
  OpenGlCommands &inner_cmds;
};
class FaceRenderCmds : public OpenGlCommands
{
public:
  FaceRenderCmds(FaceCollection &coll) : coll(coll), coords(0) { }
  FaceRenderCmds(FaceCollection &coll, TextureCoords &coord) : coll(coll), coords(&coord) { }
  void PreCalc() { }
  void Execute() 
  {
    if (coords)
      RenderOpenGl(coll, *coords);
    else
      RenderOpenGl(coll);
  }
private:
  FaceCollection &coll;
  TextureCoords *coords;
};

class RenderOpenGlCmds : public OpenGlCommands
{
public:
  RenderOpenGlCmds(const FaceCollection &col) : col(col) { }
  void PreCalc() { }
  void Execute();
private:
  const FaceCollection &col;
};

class TextureCommands : public OpenGlCommands
{
public:
  TextureCommands(OpenGlCommands &cmds) : cmds(cmds) { }
  void PreCalc() { cmds.PreCalc(); }
  void Execute();
private:
  OpenGlCommands &cmds;
};
class DisplayListCmds : public OpenGlCommands
{
public:
  DisplayListCmds(OpenGlCommands &cmds) : priv(0), cmds(cmds) { }
  void PreCalc();
  void Execute();
  ~DisplayListCmds();
private:
  void *priv;
  OpenGlCommands &cmds;
};
class ShaderCmds : public OpenGlCommands
{
public:
  ShaderCmds(OpenGlCommands &cmds, PieceShader &piece, ShaderPiece &shader, ShaderParameters *params, ShaderAttributes *attrs) : cmds(cmds), piece(piece), params(params), attrs(attrs) 
  {
    piece.push_back(&shader);
  }
  void SetParams(ShaderParameters *p) { params = p; }
  void SetAttribs(ShaderAttributes *a) { attrs = a; }
  void PreCalc() 
  {
    piece.config();
    cmds.PreCalc(); 
  }
  void Execute() 
  { 
    piece.use();
    if (params)
      piece.set_params(*params);
    if (attrs)
      piece.set_attribs(*attrs);
    cmds.Execute();
    piece.unuse();
  }
  ~ShaderCmds() {  }
private:
  OpenGlCommands &cmds;
  PieceShader &piece;
  ShaderParameters *params;
  ShaderAttributes *attrs;
};

void PrecalcCmds(OpenGlCommands &cmds);
void RenderCmds(OpenGlCommands &cmds);
void PrecalcRenderCmds(OpenGlCommands &cmds);

class ScopeMatrix
{
public:
  ScopeMatrix(Matrix m);
  ~ScopeMatrix();
};

class VBOEffect : public FrameAnim
{
public:
  VBOEffect(Render *r);
  ~VBOEffect();
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
  EmptyVertexPiece empty;
  //RandomPositionPiece empty;
  PhongPiece phong;
  BlobPiece blob;
  PieceShader piece;
};
class CircleEffect : public FrameAnim
{
public:
  CircleEffect(Render *render) : FrameAnim(render) { }

  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
};
class BlobEffect : public FrameAnimPlugins
{
public:
  BlobEffect(Render *r);
  ~BlobEffect();
  bool Frame(float time);
private:
  GlobalTexturePiece globpiece;
  TexturePiece texturepiece;
  PhongPiece phongpiece;
  NormalPiece normalpiece;
  PieceShader shader;
};

class ArrayFrameAnim : public FrameAnim
{
public:
  ArrayFrameAnim(Array<int,FrameAnim*> &arr, Render *r) : FrameAnim(r), arr(arr) { }
  void Init()
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	FrameAnim* fa = arr.Index(i);
	fa->Init();
      }
  }
  void Cleanup()
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	FrameAnim* fa = arr.Index(i);
	fa->Init();
      }
  }
  void PreFrame(float time)
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	FrameAnim* fa = arr.Index(i);
	fa->PreFrame(time);
      }
  }
  bool Frame(float time)
  {
    int size = arr.Size();
    //std::cout << "Frame: " << size << std::endl;
    for(int i=0;i<size;i++)
      {
	FrameAnim* fa = arr.Index(i);
	fa->Frame(time);
      }
    return false;
  }
  void PostFrame()
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	FrameAnim* fa = arr.Index(i);
	fa->PostFrame();
      }
  }
private:
  Array<int,FrameAnim*> &arr;
};
class DefaultFrameAnim : public FrameAnim
{
public:
  DefaultFrameAnim(const FaceCollection &faces, float time, Render *r) : FrameAnim(r), faces(faces), time(time) { }
  float Time() const { return time; }
  void Init()
  {
    //UpdateVBO(faces, vbostate);
  }
  bool Frame(float time)
  {
    //std::cout << "Frame" << std::endl;
    BoxableFaceCollectionConvert conv(faces);
    MatrixElem me(conv, Matrix::Scale(300.0,300.0,300.0));
    RenderOpenGl(me);
    //DrawVBO(vbostate);
    
    return false;
  }
private:
  const FaceCollection &faces;
  VBOState vbostate;
  float time;
};

class SequentialFrameAnim : public FrameAnim
{
public:
  SequentialFrameAnim(const Array<int, FrameAnim*> &arr, Render *r) : FrameAnim(r), arr(arr)
  {
  }
  float Time() const { 
    float time = 0.0;
    for(int i=0;i<arr.Size();i++)
      {
	time += arr.Index(i)->Time();
      }
    return time;
  }
  void Init()
  {
    for(int i=0;i<arr.Size();i++)
      {
	arr.Index(i)->Init();
      }
  }
  bool Frame(float time)
  {
    float totaltime = 0.0;
    for(int i=0;i<arr.Size();i++)
      {
	float oldtotal = totaltime;
	float t = arr.Index(i)->Time();
	totaltime += t;
	if (totaltime > time)
	  {
	    return arr.Index(i)->Frame(time-oldtotal);
	  }
      }
    return false;
  }
private:
  const Array<int, FrameAnim*> &arr;
};

class SurfaceEffect : public FrameAnim
{
public:
  SurfaceEffect(Render *r);
  ~SurfaceEffect();
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
  WaterPiece water;
  PhongPiece phong;
  PieceShader shader;
};
class RingEffect : public FrameAnim
{
public:
  RingEffect(Render *render) : FrameAnim(render) { }
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
};
class MovementEffect : public FrameAnim
{
public:
  MovementEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};
class SinusEffect : public FrameAnim
{
public: 
  SinusEffect(Render *render) : FrameAnim(render) { }
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
};

class TunnelEffect2 : public FrameAnim
{
public:
  TunnelEffect2(Render *render) : FrameAnim(render) { }
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
};

class WalkCubeEffect : public FrameAnim
{
public:
  WalkCubeEffect(Render *render) : FrameAnim(render) { }
  bool Frame(float time);
};

void InitFrameAnim(FrameAnim &f, SDL_Surface *screen);
void DisplayFrame(FrameAnim &f, SDL_Surface *screen, float time);
void CleanupFrameAnim(FrameAnim &f, SDL_Surface *surf);
SDL_Surface *InitSDL(int scr_x, int scr_y, bool vsync, bool antialias=false);


class CubeAnim : public Anim
{
public:
  CubeAnim() : coll(0), coll2(0) { }
  int NumFrames() const { return 50; }
  FaceCollection *Frame(int framenum) const
  {
    //std::cout << framenum << std::endl;
    delete coll;
    delete coll2;
    coll = new CubeElem;
    coll2 = new MatrixElem(*coll, Matrix::Scale(50+framenum,50+framenum,50+framenum));
    return coll2;
  }
private:
  mutable BoxableFaceCollection *coll, *coll2;
};

class Frames : public AnimSelectI
{
public:
  Frames(int frames) : as(frames), fs(as),frames(frames) { }
  void set_frame(int framenum) 
  {
    while(framenum > frames)
      framenum-=frames;
    fs.set_frame(framenum); 
  }
public:
  int Size() const { return as.Size(); }
  int *SelectArray() const { return as.SelectArray(); }
private:
  AnimSelect as;
  FrameSelect fs;
  int frames;
};

class AnimFrameAnim : public FrameAnim
{
public:
  AnimFrameAnim(Anim &a, ShaderFile &f, float length) : FrameAnim(&rend), a(a), fr(a.NumFrames()),s(f), length(length) { }
  void Init() 
  {
    UpdateAnim(a, s);
  }
  bool Frame(float time)
  {
    int framenum = time / length*a.NumFrames();
    //std::cout << time << " " << length << " " << a.NumFrames() << " " << framenum << std::endl;
    //fr.set_frame(framenum);
    while (framenum > a.NumFrames())
      framenum-=a.NumFrames();
    DrawAnim(framenum-0.5, framenum+0.5, s);
    return false;
 }
private:
  Anim &a;
  Frames fr;
  AnimState s;
  float length;
  SDLRender rend;
};

class ContinuousObjectMovementArrays : public Array<int, float>
{
public:
  ContinuousObjectMovementArrays(Array<int, float> &time_lengths, float current_time) : time_lengths(time_lengths), current_time(current_time) { }
  int Size() const { return time_lengths.Size(); }
  float Index(int i) const
  {
    float t = 0.0;
    int size = time_lengths.Size();
    for(int ii=0;ii<size;ii++)
      {
	t+=time_lengths.Index(ii);
	if (t > current_time)
	  {
	    if (i < ii) return 1.0;
	    if (i == ii) return (time_lengths.Index(ii) - t + current_time)/time_lengths.Index(ii); 
	    if (i > ii) return 0.0;
	  }
      }
    return 1.0;
  }
private:
  Array<int, float> &time_lengths;
  float current_time;
};

class ContinuousObjectMovement
{
public:
  ContinuousObjectMovement(Array<int, Function<float, Matrix>*> &arr,
			   Array<int, float> &arr2) : arr(arr), arr2(arr2) { }

  Matrix get_matrix() const
  {
    if (arr.Size() != arr2.Size())
      {
	std::cout << "ContinuousObjectMovement::array size problem" << std::endl;
	return Matrix::Identity();
      }
    int size = arr.Size();
    Matrix res = Matrix::Identity();
    for(int i=0;i<size;i++)
      {
	Function<float,Matrix> *func = arr.Index(i);
	float pos = arr2.Index(i);
	res *= func->Index(pos);
      }
    return res;
  }

private:
  Array<int, Function<float, Matrix>*> &arr;
  Array<int, float> &arr2;
};

BufferRef CopyFromSDLSurface(SDL_Surface *surf);

class Mesh
{
public:
  virtual int NumFrames() const=0;
  virtual int NumFaces(int framenum) const=0;
  virtual int NumPoints() const=0;
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const=0;

  virtual int NumStates() const=0;
  virtual int StateNum(int framenum, int face) const=0;
};

class FaceArrayMesh : public Mesh
{
public:
  FaceArrayMesh(FaceCollection **array, int size) : array(array), size(size) { }
  virtual int NumStates() const { return NumStates(size); }
  virtual int NumStates(int frames) const 
  {
    std::map<int,int>::iterator i = cache.find(frames);
    if (i!=cache.end())
      {
	return i->second;
      }
    int s = size;
    int currentstate = -1;
    int count = 0;
    for(int frame=0;frame<s&&frame<frames;frame++)
      {
	FaceCollection *framecoll = array[frame];
	int faces = framecoll->NumFaces();
	for(int face = 0;face<faces;face++)
	  {
	    int texture = framecoll->FaceTexture(face);
	    if (texture != currentstate)
	      {
		count++;
		currentstate = texture;
	      }
	  }
      }
    cache[frames]=count;
    return count; 
  }
  
  virtual int StateNum(int frame, int face2) const 
  { 
    int states_before = NumStates(frame);
    int count = states_before;
    FaceCollection *framecoll = array[frame];
    if (!framecoll->NumTextures()) return states_before;
    int faces = framecoll->NumFaces();
    int currentstate = -1;
    for(int face = 0;face<faces&&face<face2;face++)
      {
	int texture = framecoll->FaceTexture(face);
	if (texture != currentstate)
	  {
	    count++;
	    currentstate = texture;
	  }
      }
    return count; 
  }

  virtual int NumFrames() const { return size; }
  virtual int NumFaces(int framenum) const 
  {
    int val = array[framenum]->NumFaces(); 
    //std::cout << "NumFaces: " << framenum << ":" << val << std::endl;
    return val;
  }
  virtual int NumPoints() const
  {
    return array[0]->NumPoints(0);
  }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    Point p = array[framenum]->FacePoint(face,point);
    //std::cout << framenum << " " << face << " " << point << ":" << p << std::endl;
    return p;
  }
  
private:
  FaceCollection **array;
  int size;
  mutable std::map<int,int> cache;
};

class MeshNormals
{
public:
  virtual int NumFrames() const { return 0; }
  virtual Vector PointNormal(int framenum, int statenum, int face, int point) const { return Vector(0.0,0.0,0.0); }
};

class FaceArrayMeshNormals : public MeshNormals
{
public:
  FaceArrayMeshNormals(FaceCollection **array, int size) : array(array), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual Vector PointNormal(int framenum, int statenum, int face, int point) const 
  { return array[framenum]->PointNormal(face,point); }
private:
  FaceCollection **array;
  int size;
};

class MeshTexCoords
{
public:
  virtual int NumFrames() const { return 0; }
  virtual Point2d TexCoord(int framenum, int statenum, int face, int point) const { Point2d p = { 0.0, 0.0 }; return p;  }
};

class FaceArrayMeshTexCoords : public MeshTexCoords
{
public:
  FaceArrayMeshTexCoords(FaceCollection **array, int size) : array(array), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual Point2d TexCoord(int framenum, int statenum, int face, int point) const 
  { 
    return array[framenum]->TexCoord(face,point);
  }
private:
  FaceCollection **array;
  int size;
};

class MeshColors
{
public:
  virtual int NumFrames() const { return 0; }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point) const { return 0; }
};

class FaceArrayMeshColors : public MeshColors
{
public:
  FaceArrayMeshColors(FaceCollection **array, int size) : array(array), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point) const 
  { return array[framenum]->Color(face,point); }
private:
  FaceCollection **array;
  int size;
};

class MeshTextures
{
public:
  virtual int NumTextures() const { return 0; }
  virtual void GenTexture(int num) { }
  virtual BufferRef TextureBuf(int num) const { BufferRef ref; return ref; }
};

class FaceArrayMeshTextures : public MeshTextures, public Function<int,int>
{
public:
  FaceArrayMeshTextures(FaceCollection **array, int size, int frame) : array(array), size(size), frame(frame) { }
  virtual int NumTextures() const { return array[frame]->NumTextures(); }
  virtual void GenTexture(int num) 
  {
    array[frame]->GenTexture(num);
  }
  virtual BufferRef TextureBuf(int num) const { return array[frame]->TextureBuf(num); }
  virtual int Index(int texnum) const
  {
    int s = array[frame]->NumFaces();
    for(int i=0;i<s;i++)
      {
	int tex = array[frame]->FaceTexture(i);
	if (tex==texnum) return i;
      }
    return -1;
  }
private:
  FaceCollection **array;
  int size;
  int frame;
};

class MeshShaderParameters
{
public:
  virtual int NumFrames() const { return 0; }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point, int id) const { return 0; }
};



template<class T>
class Bitmap;


class ArrayRender
{
public:
  ArrayRender() : vertex_array(0), normal_array(0), color_array(0),
		  tex_coord_array(0), index_array(0), texture(0), texture_count(0) { }
  ~ArrayRender();
  int NumFaces(FaceCollection &coll);
  int NumVertices(FaceCollection &coll);
  void Alloc(int numfaces, int numvertices);
  void Alloc(int numfaces, int numvertices, int vertexframes, int normalframes, int colorframes, int texcoordframes);
  void AllocTexture(int count);
  std::pair<int,int> Insert(FaceCollection &coll);
  std::pair<int,int> InsertMesh(Mesh &mesh, int frame);
  std::vector<std::pair<int,int> > InsertAll(Mesh &mesh, MeshNormals &normal, MeshTexCoords &coord, MeshColors &color);
  void UpdatePos(FaceCollection &coll, std::pair<int,int> p);
  void UpdateNormals(Mesh &mesh, MeshNormals &normal, std::pair<int,int> p, int frame);
  void UpdateTexCoord(Mesh &mesh, MeshTexCoords &coord, std::pair<int,int> p, int frame);
  void UpdateColors(Mesh &mesh, MeshColors &color, std::pair<int,int> p, int frame);
  void UpdateAllTextures(MeshTextures &texture);
  void UpdateTexture(MeshTextures &texture, int num);
  void EnableTexture(int num);
  void DisableTexture();
  void Render(bool normal, bool color, bool texcoord) { Render(normal, color, texcoord, 0, used_vertex_count[0]); }
  void Render(bool normal, bool color, bool texcoord, int vertex_pos, int vertex_size);
  void Render(int vertexframe, int normalframe, int colorframe, int texcoordframe, int vertex_pos, int vertex_size);
  void Render(int vertexframe, int normalframe, int colorframe, int texcoordframe) { Render(vertexframe, normalframe, colorframe, texcoordframe, 0, used_vertex_count[vertexframe]); }
  
  int VertexArrayStep() const { return 3; }
  int NormalArrayStep() const { return 3; }
  int ColorArrayStep() const { return 4; }
  int TexCoordArrayStep() const { return 2; }

  int TextureCount() const { return texture_count; }
public:
  float *vertex_array; // 3 * float per vertex
  int vertex_array_size; 
  int *used_vertex_count;
  int *used_face_count;
  float *normal_array; // 3 * float per vertex
  unsigned char *color_array; // 4 * unsigned char per vertex
  float *tex_coord_array; // 2 * float per vertex
  bool quads;

  int *index_array; // DO NOT MODIFY, indexed by face number

  int m_vertex_frames;
  int m_normal_frames;
  int m_color_frames;
  int m_texcoord_frames;

  int *texture;
  int texture_count;
  int previous_frame;
  BufferRef **textures;
  int *tex_size_x;
  int *tex_size_y;
};

struct PrecalcStore
{
  std::vector<std::pair<int,int> > vec;
  int vertex_size;
  int framecount;
};
class Mesh;

void PreCalcMesh(Mesh &mesh, ArrayRender &r, PrecalcStore &prestore);
void PreCalcFaceStore(void (*fptr)(FaceStore &s, float time, float repeat_length), float repeat_duration, int repeat_framecount, ArrayRender &r, PrecalcStore &prestore);
void ExecuteFaceStore(ArrayRender &r, PrecalcStore &prestore, int frame);


class DynamicMesh : public Mesh
{
public:
  DynamicMesh() : numframes(0), numpoints(0) { }
  virtual int NumFrames() const { return numframes; }
  virtual int NumFaces(int framenum) const
  {
    //std::cout << "framenum: " << framenum << " " << numfaces[framenum] << std::endl;
    return numfaces[framenum];
  }
  virtual int NumPoints() const
  {
    return numpoints;
  }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    Key k = { framenum, face, point };
    return vec[k];
#if 0
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	const Key &k = vec[i];
	if (k.framenum == framenum && k.face==face && k.point==point)
	  {
	    return k.p;
	  }
      }
    return Point(0.0,0.0,0.0);
#endif
  }
  struct Key { int framenum; int face; int point; 
    friend bool operator<(const Key &k1, const Key &k2)
    {
      if (k1.framenum < k2.framenum) return true;
      if (k1.framenum > k2.framenum) return false;

      if (k1.face < k2.face) return true;
      if (k1.face > k2.face) return false;

      if (k1.point < k2.point) return true;
      if (k1.point > k2.point) return false;
      return false;
    }
  };

  void push_frame() { numframes++; }
  void push_back(const FaceCollection &coll)
  {
    std::cout << "numfaces insert" << std::endl;
    numfaces.push_back(coll.NumFaces());
    if (numpoints==0) numpoints = coll.NumPoints(0);
    int faces = coll.NumFaces();
    for(int i=0;i<faces;i++)
      {
	for(int j=0;j<numpoints;j++)
	  {
	    Key k = { numframes, i, j };
	    //vec.push_back(k);
	    vec[k] = coll.FacePoint(i,j);
	  }
      }
  }
  virtual int NumStates() const { return 0; }
  virtual int StateNum(int framenum, int face) const { return 0; }

private:
  int numframes;
  std::vector<int> numfaces;
  int numpoints;
  mutable std::map<Key,Point> vec;
  //std::vector<Key> vec;
};

class MeshFaceCollection : public FaceCollection
{
public:
  MeshFaceCollection(Mesh &mesh, int framenum) : mesh(mesh), framenum(framenum) { }
  virtual int NumFaces() const { return mesh.NumFaces(framenum); }
  virtual int NumPoints(int face) const { return mesh.NumPoints(); }
  virtual Point FacePoint(int face, int point) const
  {
    return mesh.FacePoint(framenum, 0, face, point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = mesh.FacePoint(framenum, 0, face, 0);
    Point p2 = mesh.FacePoint(framenum, 0, face, 1);
    Point p3 = mesh.FacePoint(framenum, 0, face, 2);
    return -Vector::CrossProduct(p2-p1, p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const
  { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

private:
  Mesh &mesh;
  int framenum;
};

class SDLArrayRender : public Render
{
public:
  void SetTime(float time);
  void SetMatrix(Matrix m);
  void RenderFaces(const FaceCollection &col);
  void RenderFaces(const FaceCollection &col, const TextureCoords &tex);
  void RenderFacesTexture(const FaceCollection &col);

  void RenderLines(const LineCollection &col);
  void RenderPoly(const PointCollection &col);
  void* CreateState();
  void UpdateVBO(const FaceCollection &faces, void *state);
  void DrawVBO(void *state);
  void DestroyState(void *state);
public:
  void Show();
private:
  ArrayRender rend;
  PrecalcStore s;
  DynamicMesh dyn;
  FaceCollectionStore store;
};
void PreCalcExecute(Render &rend, FrameAnim &f, float duration, int numframes);


class ArrayEffect : public FrameAnim
{
public:
  ArrayEffect(Render *render) : FrameAnim(render) { }
  float XRot() const { return 0.0; }
  float YRot() const { return 1.0; }
  float ZRot() const { return 0.0; }

  void Init();

  bool Frame(float time);
private:
  ArrayRender rend;
  PrecalcStore s;
};

class MeshToTrianglesCounter
{
public:
  int face_num(int face) const { return face/2; }
  int point_map(int face, int point) const { 
    int f = face/2;
    int ff = face-f*2;
    int p =0;
    if (ff)
      {
	if (point==0) p=0;
	if (point==1) p=1;
	if (point==2) p=2;
      }
    else
      {
	if (point==0) p=0;
	if (point==1) p=2;
	if (point==2) p=3;
      }
    return p;
  }
};

class MeshToTriangles : public Mesh
{
public:
  MeshToTriangles(Mesh &m) : m(m) { }
  virtual int NumStates() const { return 0; }
  virtual int StateNum(int frame, int face) const { return 0; }

  virtual int NumFrames() const
  {
    return m.NumFrames();
  }
  virtual int NumFaces(int framenum) const
  {
    //std::cout << "NumFaces " << m.NumFaces(framenum) << std::endl;
    if (m.NumPoints()==4) return m.NumFaces(framenum)*2;
    return m.NumFaces(framenum);
  }
  virtual int NumPoints() const
  {
    return 3;
  }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    if (m.NumPoints()==4)
      {
	int f = count.face_num(face);
	int p = count.point_map(face, point);
	Point pp = m.FacePoint(framenum, statenum, f, p);
	//std::cout << framenum << " " << statenum << ":" << face << " " << point << ":" << pp << std::endl;
	return pp;
      }
    return m.FacePoint(framenum, statenum, face, point);
  }
private:
  Mesh &m;
  MeshToTrianglesCounter count;
};
class MeshNormalsToTriangles : public MeshNormals
{
public:
  MeshNormalsToTriangles(MeshNormals &n, bool b) : n(n), b(b) { }
  virtual int NumFrames() const { return n.NumFrames(); }
  virtual Vector PointNormal(int framenum, int statenum, int face, int point) const 
  {
    if (b)
      {
	int f = count.face_num(face);
	int p = count.point_map(face, point);
	return n.PointNormal(framenum, statenum, f,p);
      }
    return n.PointNormal(framenum, statenum, face, point);
  }
private:
  MeshNormals &n;
  bool b;
  MeshToTrianglesCounter count;
};

class MeshTexCoordsToTriangles : public MeshTexCoords
{
public:
  MeshTexCoordsToTriangles(MeshTexCoords &n, bool b) : n(n), b(b) { }
  virtual int NumFrames() const { return n.NumFrames(); }
  virtual Point2d TexCoord(int framenum, int statenum, int face, int point) const 
  { 
    if (b)
      {
	int f = count.face_num(face);
	int p = count.point_map(face, point);
	return n.TexCoord(framenum, statenum, f, p);
      }
    return n.TexCoord(framenum, statenum, face, point);
  }

private:
  MeshTexCoords &n;
  bool b;
  MeshToTrianglesCounter count;
};

class MeshColorsToTriangles : public MeshColors
{
public:
  MeshColorsToTriangles(MeshColors &n, bool b) : n(n), b(b) { }
  virtual int NumFrames() const { return n.NumFrames(); }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point) const 
  { 
    if (b)
      {
	int f = count.face_num(face);
	int p = count.point_map(face, point);
	return n.VertexColor(framenum, statenum, f, p);
      }
    return n.VertexColor(framenum, statenum, face, point);
  }
private:
  MeshColors &n;
  bool b;
  MeshToTrianglesCounter count;
};

//
// VBO usage, try 2
//

struct VertexElem
{
  float x,y,z;
};
struct NormalElem
{
  float nx,ny,nz;
};
struct ColorElem2
{
  unsigned char r,g,b,a;
};
struct TexCoordElem
{
  float tx,ty;
};
struct DataBlock
{
  DataBlock() : type(0), orig_ptr(0), size_in_bytes(1), adjusted_buffer(0), adjusted_size(0), cleanup((char*)0) { }
  template<class T>
  DataBlock(int type, T *ptr,int count) : type(type), orig_ptr((char*)ptr), size_in_bytes(count*sizeof(T)), adjusted_buffer(0), adjusted_size(0), cleanup(ptr) { }
  DataBlock(const DataBlock &b)
  {
    adjusted_buffer = new char[b.adjusted_size];
    adjusted_size = b.adjusted_size;
    std::copy(b.adjusted_buffer, b.adjusted_buffer+adjusted_size, adjusted_buffer);
    orig_ptr = b.orig_ptr;
    type = b.type;
    size_in_bytes = b.size_in_bytes;
  }
  std::pair<char*, int> BufferPosPart() const { return std::make_pair(adjusted_buffer, adjusted_size); }
  int type;
  char *orig_ptr;
  int size_in_bytes;
  void OrigToAdjusted(int size_in_bytes);
  char *adjusted_buffer;
  int adjusted_size;
  ~DataBlock();
  std::shared_ptr<void> cleanup;
};

struct DataFrames
{
  std::vector<DataBlock> vec;

  void SetFrames(int frames) { vec.resize(frames); }
  int NumBytes(); // just one of them
  int Count();
  void OrigToAdjusted(int frame, int bytes) { vec[frame].OrigToAdjusted(bytes); }
  std::pair<char*, int> BufferPosPart(int frame) const
  {
    return vec[frame].BufferPosPart();
  }
};

struct DataTypes
{
  DataFrames vertex;
  DataFrames normal;
  DataFrames color;
  DataFrames texcoord;
  void SetFrames(int type, int frames);

  void OrigToAdjusted(bool has_normal, bool has_color, bool has_texcoord);
  std::pair<char*, int> BufferPosPart(int type, int frame)
  {
    switch(type) {
    case 0: return vertex.BufferPosPart(frame);
    case 1: return normal.BufferPosPart(frame);
    case 2: return color.BufferPosPart(frame);
    case 3: return texcoord.BufferPosPart(frame);
    };
    return std::make_pair((char*)0,0);
  }
};

struct VBOObjects
{
  VBOObjects();
  ~VBOObjects();
  void SetObjectCount(int count) { objs.resize(count); }
  void MeshToOrig(int objnum, Mesh &mesh);
  void MeshNormalsToOrig(int objnum, Mesh &mesh, MeshNormals &normals);
  void MeshColorsToOrig(int objnum, Mesh &mesh, MeshColors &colors);
  void MeshTexCoordsToOrig(int objnum, Mesh &mesh, MeshTexCoords &texcoords);
  void OrigToAdjusted(bool has_normal, bool has_color, bool has_texcoord);
  void AllocWholeBuffer(bool has_normal, bool has_color, bool has_texcoord);
  void CopyPartsToWholeBuffer(int vertex, int normal, int color, int texcoord);
  void BlitWholeBufferToGPU();
  void CopyPartToGPU(int obj_num, int vertex, int normal, int color, int texcoord);
  void BlitGPUToScreen();
public:
  void AllocTexture(int count);
  void UpdateTexture(MeshTextures &tex, int frame, int pos);
  void EnableTexture(int frame);
  void DisableTexture();


private:
  std::vector<DataTypes> objs;
  char *wholebuffer;
  int wholebuffer_vertex_size;
  int wholebuffer_normal_size;
  int wholebuffer_color_size;
  int wholebuffer_texcoord_size;
  bool m_has_normal;
  bool m_has_color;
  bool m_has_texcoord;
  bool m_quads;

  struct Priv;
  Priv *priv;
  friend class BlitWholeInterface;
  friend class BlitPartInterface;

  std::pair<char*, int> BufferPosPart(int type, int obj_num, int frame);
  std::pair<int,int> BufferPosWhole(int type, int obj_num);
  void SetFrameCount(int obj_num, int type, int num_frames);

  int *texture;
};


template<class T>
void AllocToNativeArray(Array<int,T> &arr, T** result)
{
  int sz = arr.Size();
  *result = new T[sz];
  for(int i=0;i<sz;i++)
    {
      (*result)[i] = arr.Index(i);
    }
}

class FaceCollectionBuffer
{
public:
  FaceCollectionBuffer(FaceCollection &coll,
		       bool vertex, bool normal, bool color, bool texcoord) : coll(&coll), m_vertex(vertex), m_normal(normal), m_color(color), m_texcoord(texcoord) { }
  void AddAttrib(int id) { attrib_ids.push_back(id); }
  void CalculateSizes();
  void CreateBuffers();
  const Buffer<float> &VertexBuffer() const { return *vertex_buffer; }
  const Buffer<float> &NormalBuffer() const { return *normal_buffer; }
  const Buffer<unsigned int> &ColorBuffer() const { return *color_buffer; }
  const Buffer<float> &TexCoordBuffer() const { return *texcoord_buffer; }
  const Buffer<float> &Attrib_buffer(int id) const
  {
    int s = attrib_ids.size();
    for(int i=0;i<s;i++)
      if (attrib_ids[i]==id) return *attrib_buffers[i];
    return *((Buffer<float>*)0);
  }
private:
  FaceCollection *coll;
  bool m_vertex;
  bool m_normal;
  bool m_color;
  bool m_texcoord;
  int vertex_size;
  int normal_size;
  int color_size;
  int texcoord_size;
  Buffer<float> *vertex_buffer;
  Buffer<float> *normal_buffer;
  Buffer<unsigned int> *color_buffer;
  Buffer<float> *texcoord_buffer;
  Buffer<float> **attrib_buffers;
  std::vector<int> attrib_ids;
};

class RenderBuffers
{
public:
  void SetupVertex(const Buffer<float> &buf, int pos);
  void SetupNormal(const Buffer<float> &buf, int pos);
  void SetupColor(const Buffer<unsigned int> &buf, int pos);
  void SetupTexCoord(const Buffer<float> &buf, int pos);
  void SetupAttrib(int id, const Buffer<float> &buf, int pos);
  void Render(bool quads, int start_elem, int numitems);
  //void RenderOne(int start_elem, int numitems);
  void DisableAll();
private:
  bool vertex,normal,color,texcoord;
  std::vector<int> attrib_ids;
};

// shader time has to reset on state change
// 1) Empty in vertex array solves problems
// 2) time has to be continuous
// 3) shaders describe path of single point to get continuous time
// 4) vertex arrays describe all _possible_ changes in the path
//      or jumps in the positions

#if 0
struct TimedPoint
{
  float start_time;
  float end_time;
  
};
#endif

class FaceCollectionReplacement
{
public:
  virtual Point FacePoint(int face, int point);
  virtual int SpecialTimesCount() const=0;
  virtual float SpecialTimes(int special_time)=0; // returns time
  // TODO: PATH SELECTION
  virtual int SpecialPathNum() const=0; // which path to choose
  virtual Point PathData(int special_time, int num) const=0;
};


class VertexArrayFaceCollection : public FaceCollection
{
public:
  VertexArrayFaceCollection(float *v_array, int v_size,
			    float *n_array, int n_size,
			    unsigned int *c_array, int c_size,
			    float *tex_array, int tex_size,
			    float **attrib_array, int a_size1, int a_size2)
    : v_array(v_array), v_size(v_size),
      n_array(n_array), n_size(n_size),
      c_array(c_array), c_size(c_size),
      tex_array(tex_array), tex_size(tex_size),
      attrib_array(attrib_array), attrib_size1(a_size1), attrib_size2(a_size2)
  {
  }
      
  virtual int NumFaces() const { return v_size/3/3; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    if (v_array)
      {
	x = v_array[face*3*3+point*3+0];
	y = v_array[face*3*3+point*3+1];
	z = v_array[face*3*3+point*3+2];
      }
    return Point(x,y,z);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    if (n_array)
      {
	x = n_array[face*3*3+point*3+0];
	y = n_array[face*3*3+point*3+1];
	z = n_array[face*3*3+point*3+2];    
      }
    return Vector(x,y,z);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (attrib_array)
      return attrib_array[id][face*3*1+point*1];
    else
      {
	return 0.0;
      }
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (c_array)
      return c_array[face*3+point];
    else
      return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    float x = 0.0;
    float y = 0.0;
    if (tex_array)
      {
	x = tex_array[face*3*2+point*2 +0];
	y = tex_array[face*3*2+point*2 +1];
      }
    Point2d p = { x,y };
    return p;
  }
  virtual int NumTextures() const { return 0; }
  virtual void GenTexture(int num) { }
  virtual BufferRef TextureBuf(int num) const { BufferRef ref; return ref; }
  virtual int FaceTexture(int face) const { return -1; }
private:
  float *v_array;
  int v_size;
  float *n_array;
  int n_size;
  unsigned int *c_array;
  int c_size;
  float *tex_array;
  int tex_size;
  float **attrib_array;
  int attrib_size1, attrib_size2;
};

class FaceCollectionVertexArray
{
public:
  FaceCollectionVertexArray(FaceCollection &coll) : coll(coll) { }
  void Generate_Vertex(float **vertex, int *size)
  {
  }
  void Generate_Normal(float **normal, int *size);
  void Generate_Color(unsigned int **color, int *size);
  void Generate_Tex(float **texture, int *size);
  void Generate_Attrib(int id, float *array, int *size);
  void Generate_Attrib_Array(float **attrib_array, int *asize1, int *asize2);
  
private:
  FaceCollection &coll;
};

#endif
