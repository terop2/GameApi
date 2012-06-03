#include "Effect.hh"

template<class X>
class X_Eq_Class : public Function<X, bool>
{
public:
  X_Eq_Class(X x) : x(x) { }
  bool Index(X y) { return x==y; }
private:
  X x;
};

class Eq_Class_Float : public Function<float, bool>
{
public:
  Eq_Class_Float(float val) : val(val) { }
  bool Index(float x) const
  {
    return fabs(x-val)<0.001;
  }
private:
  float val;
};

class Eq_Class_Point : public Function<Point, bool>
{
public:
  Eq_Class_Point(Point p) : p(p) { }
  bool Index(Point pp) const
  {
    return (pp-p).Dist()<0.001;
  }
private:
  Point p;
};

template<class T, class K>
class Intersection : public Function<std::pair<T,K>,bool>
{
public:
  Intersection(Function<T,bool> &f1, Function<K,bool> &f2) : f1(f1), f2(f2) { }
  bool Index(std::pair<T,K> t) const { return f1.Index(t.first) && f2.Index(t.second); }
private:
  Function<T,bool> &f1;
  Function<K,bool> &f2;
};

class Lattice : public Function<float,bool>
{
public:
  Lattice() { }
  bool Index(float x) const
  {
    int val = x;
    float d = x-float(val);
    return fabs(d)<0.001;
  }
};

class Lattice2d : public Function<Point2d, bool>
{
public:
  bool Index(Point2d p) const
  {
    bool xb = x.Index(p.first);
    bool yb = y.Index(p.second);
    return xb&&yb;
  }
private:
  Lattice x;
  Lattice y;
};

class Lattice3d : public Function<Point, bool>
{
public:
  bool Index(Point p) const
  {
    bool xb = x.Index(p.x);
    bool yb = y.Index(p.y);
    bool zb = z.Index(p.z);
    return xb&&yb&&zb;
  }
private:
  Lattice x;
  Lattice y;
  Lattice z;
};

template<class T>
class Subset : public Function<T,bool>
{
  virtual bool Index(T t) const=0; // characteristic function
};

template<class T>
class Power : public Function<subset<T>*, bool>
{
public:
  virtual std::vector<subset<T>*> Subsets() const=0;
  virtual bool Index(subset<T> *power) const=0;
  virtual subset<T> *Unique() const=0;
};

template<class I1, class I2, class C>
class PowerBitmap
{
public:
  virtual I1 SizeX() const=0;
  virtual I2 SizeY() const=0;
  virtual C Map(I1 i1, I2 i2) const=0;
};

template<class T, class K>
class PowerIntersection : public PowerBitmap<subset<T>*, subset<K>*, bool>
{
public:
  PowerIntersection(Power<T> *pw1, Power<K> *pw2) : pw1(pw1), pw2(pw2) { }
  virtual subset<T>* SizeX() const { return NULL; }
  virtual subset<K>* SizeY() const { return NULL; }
  virtual bool Map(subset<T>* i1, subset<K> *i2) const
  {
    reutrn pw1.Index(i1) && pw2.Index(i2);
  }
private:
  Power<T> *pw1;
  Power<K> *pw2;
};

template<class P, class T, class K>
class PowerSubsetP : public Power<subset<P> *>
{
public:
  PowerSubsetP(Power<T> *left_p, Power<K> *right_p) : intersect(left_p, right_p), l(left_p), r(right_p) { }
  virtual std::vector<subset<subset<P>*>*> Subsets() const=0;
  virtual bool Index(subset<subset<P>*> *power) const
  {
    return intersect.Map(left_p->Unique(), right_p->Unique());
  }
  virtual subset<subset<P>*> *Unique() const=0;
private:
  PowerIntersection intersect;
  Power<T> *left_p;
  Power<K> *right_p;
};

template<class T, class K, class P>
Power<subset<P> *> *Intersect(Power<T> *left_p, Power<K> *right_p);
{
  return new PowerSubsetP<P,T,K>(left_p, right_p);
}

template<class T>
class SubsetSingle : public Subset<T>
{
public:
  SubsetSingle(T t) : m_t(t) { }
  bool Map(T t) const { return t==m_t; }
private:
  T m_t;
};

template<class T>
class PSingle : public Power<T>
{
public:
  PSingle(SubsetSingle<T> *subset) : subset(subset) { }
  bool Inside(subset<T> *power) { return subset==power; }
  subset<T> *Unique() const { return subset; }
private:
  SubsetSingle<T> *subset;
};

template<class T, class K>
class PullBackArrow
{
public:
  virtual K Map(T t) const=0;
  virtual Power<T> *PMap(Power<K> *k) const=0; // contravariant powerset functor
};

template<class Top, class P, class A, class B, class C>
class PullBackDiagram
{
public:
  virtual PullBackArrow<Top,P> *Product()=0;
  virtual PullBackArrow<P,A> *LeftProjection()=0;
  virtual PullBackArrow<P,B> *RightProjection()=0;
  virtual PullBackArrow<A,C> *LeftEq()=0;
  virtual PullBackArrow<B,C> *RightEq()=0;
};

template<class Top, class Pr, class A, class B, class C>
subset<P> *PullBackLeft(PullBackDiagram<Top,P,A,B,C> &d, Top t)
{
  PullBackArrow<Top,P> *pr = d.Product();
  PullBackArrow<P,A> *lp = d.LeftProjection();
  PullBackArrow<P,B> *rp = d.RightProjection();
  PullBackArrow<A,C> *leq = d.LeftEq();
  PullBackArrow<B,C> *req = d.RightEq();
  
  P product = pr->Map(t);
  A a = lp->Map(product);
  C c = leq->Map(a);
  SubsetSingle<C> subset(c);
  PSingle<C> power(&subset);
  Power<B> *pb = req->PMap(&power);
  Power<P> *pp = rp->PMap(pb);
  return pp->Unique();
}

template<class Top, class Pr, class A, class B, class C>
subset<P> *PullBack(PullBackDiagram<Top,P,A,B,C> &d, Top t)
{
  PullBackArrow<Top,P> *pr = d.Product();
  PullBackArrow<P,A> *lp = d.LeftProjection();
  PullBackArrow<P,B> *rp = d.RightProjection();
  PullBackArrow<A,C> *leq = d.LeftEq();
  PullBackArrow<B,C> *req = d.RightEq();

  // find correct c : C
  P product = pr->Map(t);
  A a = lp->Map(product);
  C c = leq->Map(a);
  
  // apply powerset functor for the object
  SubsetSingle<C> subset(c);
  PSingle<C> power(&subset);
  
  // apply powerset functor for the pullback diagram arrows
  // both sides of the equation
  Power<A> *left_a = leq->PMap(&power);
  Power<B> *right_b = req->PMap(&power);
  Power<P> *left_p = lp->PMap(left_a);
  Power<P> *right_p = lp->PMap(right_b);

  // intersect
  Power<subset<P>*> *i = Intersect(left_p, right_p);

  // find unique subset
  return i->Unique();
}
