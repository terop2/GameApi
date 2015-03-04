
#include "Graph.hh"
#include "VectorTools.hh"

Point2d SolveWithFailure(const Function<Point2d, float> &func, Point2d tl, Point2d br, bool &error);
Point SolveWithFailure(const Function<Point, float> &func, Point tl, Point br, bool &error);

class DerivateFunction : public Function<float, float>
{
public:
  DerivateFunction(Function<float,float> &f, float delta) : f(f),delta(delta) { }
  float Index(float x) const
  {
    float v1 = f.Index(x+delta);
    float v2 = f.Index(x);
    //std::cout << x << " " << v1 << " " << v2 << std::endl;
    return (v1-v2)/delta;
  }
private:
  Function<float,float> &f;
  float delta;
};


class PartialFunction2_1 : public Function<float,float>
{
public:
  PartialFunction2_1(Function<Point2d, float> &f, float y_) : f(f),y(y_) { }
  float Index(float x) const
  {
    Point2d p = { x,y };
    return f.Index(p);
  }
private:
  Function<Point2d, float> &f;
  float y;
};

class PartialFunction2_2 : public Function<float,float>
{
public:
  PartialFunction2_2(Function<Point2d, float> &f, float x_) : f(f),x(x_) { }
  float Index(float y) const
  {
    Point2d p = { x,y };
    return f.Index(p);
  }
private:
  Function<Point2d, float> &f;
  float x;
};

class PartialFunction3_1 : public Function<float,float>
{
public:
  PartialFunction3_1(Function<Point, float> &f, float y_, float z_) : f(f),y(y_), z(z_) { }
  float Index(float x) const
  {
    Point p(x,y,z);
    return f.Index(p);
  }
private:
  Function<Point, float> &f;
  float y;
  float z;
};

class PartialFunction3_2 : public Function<float,float>
{
public:
  PartialFunction3_2(Function<Point, float> &f, float x_, float z_) : f(f),x(x_), z(z_) { }
  float Index(float y) const
  {
    Point p(x,y,z);
    return f.Index(p);
  }
private:
  Function<Point, float> &f;
  float x;
  float z;
};

class PartialFunction3_3 : public Function<float,float>
{
public:
  PartialFunction3_3(Function<Point, float> &f, float x_, float y_) : f(f),x(x_), y(y_) { }
  float Index(float z) const
  {
    Point p(x,y,z);
    return f.Index(p);
  }
private:
  Function<Point, float> &f;
  float x;
  float y;
};




class Gradient : public Function<Point2d, Vector2d>
{
public:
  Gradient(Function<Point2d, float> &f) : f(f) { }
  Vector2d Index(Point2d p) const
  {
    PartialFunction2_1 p1(f, p.y);
    PartialFunction2_2 p2(f, p.x);
    DerivateFunction f1(p1,0.001);
    DerivateFunction f2(p2,0.001);
    Vector2d v;
    v.dx = f1.Index(p.x);
    v.dy = f2.Index(p.y);
    return v;
  }
private:
  Function<Point2d,float> &f;
};

class Gradient3d : public Function<Point, Vector>
{
public:
  Gradient3d(Function<Point, float> &f) : f(f) { }
  Vector Index(Point p) const
  {
    PartialFunction3_1 p1(f, p.y, p.z);
    PartialFunction3_2 p2(f, p.x, p.z);
    PartialFunction3_3 p3(f, p.x, p.y);
    DerivateFunction f1(p1,0.0001);
    DerivateFunction f2(p2,0.0001);
    DerivateFunction f3(p3,0.0001);
    Vector v;
    v.dx = f1.Index(p.x);
    v.dy = f2.Index(p.y);
    v.dz = f3.Index(p.z);
    //std::cout << "Vec: " << v << std::endl;
    return v;
  }
private:
  Function<Point,float> &f;
};


class Point2dFunction : public Function<Point2d, float>
{
public:
  Point2dFunction(Function<std::pair<float,float>, float> &f) : f(f) { }
  float Index(Point2d p) const
  {
    std::pair<float,float> pp = std::make_pair(p.x, p.y);
    return f.Index(pp);
  }
private:
  Function<std::pair<float,float>, float> &f;
};

class IntegrationFunction : public Function<float, float>
{
  // Integration is function (R->R)->R.
  // to get that from this code do this: 
  //  IntegrationFunction i(...); 
  //  float r = i.Index(x);
public:
  IntegrationFunction(const Function<float,float> &f, float delta, float startx, float endx, float C) : f(f), delta(delta), startx(startx), /*endx(endx),*/ C(C) { }
  float Index(float x) const
  {
    float val = C;
    float val2 = C;
    for(float xx = startx;xx<x;xx+=delta)
      {
	val += std::min(f.Index(xx)*delta, f.Index(xx+delta)*delta);
	val2 += std::max(f.Index(xx)*delta, f.Index(xx+delta)*delta);
      }
    val/=(x-startx);
    val2/=(x-startx);
    return (val+val2)/2.0;
  }
  float Delta(float x) const
  {
    float val = C;
    float val2 = C;
    for(float xx = startx;xx<x;xx+=delta)
      {
	val += std::min(f.Index(xx)*delta, f.Index(xx+delta)*delta);
	val2 += std::max(f.Index(xx)*delta, f.Index(xx+delta)*delta);
      }
    val /= (x-startx);
    val2 /= (x-startx);
    return fabs(val-val2)/2.0;
  }
private:
  const Function<float,float> &f;
  float delta, startx, /*endx,*/ C;
};

template<class T, class U, class K>
class InverseFunctionCompose : public InvertableFunction<T,K>
{
public:
  InverseFunctionCompose(InvertableFunction<T,U> &f, InvertableFunction<U,K> &g) : f(f), g(g) { }
  K Index(T t) const { return g.Index(f.Index(t)); }
  std::vector<T> Inverse(K k) const
  {
    std::vector<U> vec = g.Inverse(k);
    std::vector<T> vec2;
    int size = vec.size();
    for(int i=0;i<size;i++)
      {
	std::vector<T> vec3 = f.Inverse(vec[i]);
	vec2.insert(vec2.end(), vec3.begin(), vec3.end());
      }
    return vec2;
  }
private:
  InvertableFunction<T,U> &f;
  InvertableFunction<U,K> &g;
};
template<class T>
class Forall
{
public:
  virtual T Element() const=0;
};

template<class U, class V>
class Exists
{
public:
  Exists(Forall<U> &fa, Function<U,V> &f) : fa(fa), f(f) { }
  V Element() const { return f.Index(fa.Element()); }
private:
  Forall<U> &fa;
  Function<U,V> &f;
};


template<class T>
class PowerSet : public Function<T,bool>
{
public:
};

template<class T>
class Compare
{
public:
  virtual bool Comp(T t1, T t2) const=0;
};

template<class A, class U=A>
class SubImage : public Function<U,A>, public PowerSet<A>
{
public:
  SubImage(Function<A,bool> &obj, bool choose) : obj(obj), choose(choose) { }
  bool Index(A t) const
  {
    return obj.Index(t)==choose;
  }
  A Index(U t) const
  {
    return t;
  }
private:
  Function<A,bool> &obj;
  bool choose;
};

template<class X, class Y, class U=X, class V=Y>
class InverseImageK : public Function<Function<V,Y>*, Function<U,X>*>, private Function<U,X>
{
public:
  InverseImageK(Function<X,Y> &f) : f(f) { }
  Function<U,X> *Index(Function<V,Y>* vy) const
  {
    return *this;
  }
  X Index(U u) const
  {
    return u;
  }
private:
  Function<X,Y> &f;
};

template<class X, class Y, class U=X, class V=Y>
class InverseImageA : public Function<X,bool>
{
public:
  InverseImageA(Function<X,Y> &f, Function<Y,bool> &a) : f(f),a(a) { }
  bool Index(X x) const {
    return a.Index(f.Index(x));
  }
  X Index(U u) const {
    return u;
  }
private:
  Function<X,Y> &f;
  Function<Y,bool> &a;
};

template<class A, class B>
class InverseImage : public Function<PowerSet<B>*,PowerSet<A>*>, private PowerSet<A>
{
public:
  InverseImage(Function<A,B> &f) : f(f) { }
  PowerSet<A> *Index(PowerSet<B> *b) const
  {
    bb = b;
    return const_cast<InverseImage*>(this);
  }
private:
  bool Index(A a) const
  {
    return bb->Index(f.Index(a));
  }
private:
  Function<A,B> &f;
  mutable PowerSet<B> *bb;
};


template<class Omega, class A, class B>
class InverseImageF : public Function<Function<B,Omega>*, Function<A,Omega>*>, private Function<A,Omega>
{
public:
  InverseImageF(Function<A,B> &f) : f(f) { }
  Function<A,Omega> *Index(Function<B,Omega> *b) const
  {
    bb = b;
    return const_cast<InverseImageF*>(this);
  }
private:
  Omega Index(A a) const
  {
    return bb->Index(f.Index(a));
  }
private:
  Function<A,B> &f;
  Function<B,Omega> *bb;
};

struct Singleton
{
  static const Singleton &get() { static Singleton s; return s; }
};

template<class U, class V, class X, class Y>
class InverseImageSquare
{
public:
  class Req
  {
  public:
    virtual X i(U u) const=0;
    virtual Y f(X x) const=0;
    virtual Y j(V v) const=0;
  };
  class Res
  {
  public:
    virtual bool u(U u) const=0;
    virtual bool v(V v) const=0;
    virtual bool x(X x) const=0;
    virtual bool y(Y y) const=0;
  };
  class ResImpl : public Res
  {
  public:
    ResImpl(PowerSet<Y> &mem, Req &r) : mem(mem),r(r) { }
    bool u(U u) const { return mem.Index(r.f(r.i(u))); }
    bool v(V v) const { return mem.Index(r.j(v)); }
    bool x(X x) const { return mem.Index(r.f(x)); }
    bool y(Y y) const { return mem.Index(y); }
    
  private:
    PowerSet<Y> &mem;
    Req &r;
  };
  
};

template<class A>
class ElementSingleton : public Function<Singleton, A>
{
public:
  ElementSingleton(A a) : a(a) { }
  A Index(Singleton s) const { return a; }
private:
  A a;
};

template<class A>
class Fiber : public Function<PowerSet<A>*,bool>
{
public:
  Fiber(A a) : elem(a), image(elem) { }
  bool Index(PowerSet<A> *mem) const
  {
    PowerSet<Singleton> *mem2 = image.Index(mem);
    return mem2->Index(Singleton::get());
  }
private:
  ElementSingleton<A> elem;
  InverseImage<Singleton, A> image;
};



template<class U, class X, class Y>
class FiberSquare
{
public:
  typedef typename InverseImageSquare<U,X,Singleton,Y>::Req Req;
  typedef typename InverseImageSquare<U,X,Singleton,Y>::Res Res;
  typedef typename InverseImageSquare<U,X,Singleton,Y>::ResImpl ResImpl;
};

struct Spread
{
  Color center;
  float radius;
};
struct Pixel
{
public:
  int x,y;   // 0..sx, 0..sy
  int r,g,b; // 0..255
};
struct Env
{
  Pixel center;
  Pixel t,b,l,r;
};
class BlurFunction : public Function<Pixel, Env>
{
};


class ImageBools : public Function<Pixel, bool>
{
public:
  ImageBools(Bitmap<Color> &bm) : bm(bm) { }

  bool Index(Pixel p) const
  {
    Color c = bm.Map(p.x,p.y);
    return c.r == p.r && c.g == p.g && c.b == p.b;
  }
private:
  Bitmap<Color> &bm;
};


template<class A, class B, class Omega>
class InverseImage2 : public Function<Function<B,Omega>*,Function<A,Omega>*>, private Function<A,Omega>
{
public:
  InverseImage2(Function<A,B> &f) : f(f) { }
  Function<A,Omega> *Index(Function<B,Omega> *b) const
  {
    bb = b;
    return this;
  }
private:
  Omega Index(A a) const
  {
    return bb->Index(f.Index(a));
  }
private:
  Function<A,B> &f;
  mutable Function<B,Omega> *bb;
};


template<class A>
class Intersection : public PowerSet<A>
{
public:
  Intersection(PowerSet<A> &m, PowerSet<A> &m2) : m(m), m2(m2) { }
  bool Index(A a) const
  {
    return m.Index(a) && m2.Index(a);
  }
private:
  PowerSet<A> &m;
  PowerSet<A> &m2;
};


template<class T, class A, class C, class B>
class Intersection2 : public Function<PowerSet<B>*,PowerSet<T>*>
{
public:
  Intersection2(Function<A,B> &f1, Function<C,B> &f2, Function<T,A> &m1, Function<T,C> &m2) : i1(f1), i2(f2), im1(m1), im2(m2), mem(0) { }
  PowerSet<T> *Index(PowerSet<B> *b) const
  {
    PowerSet<A> *ia = i1.Index(b);
    PowerSet<C> *ic = i1.Index(b);
    PowerSet<T> *it1 = im1.Index(ia);
    PowerSet<T> *it2 = im2.Index(ic);
    delete mem;
    mem = new Intersection<T>(it1,it2);
    return mem;
  }

private:
  InverseImage<A,B> i1;
  InverseImage<C,B> i2;
  InverseImage<T,A> im1;
  InverseImage<T,C> im2;
  mutable Intersection<T> *mem;
};

template<class A>
class Union2 : public PowerSet<A>
{
public:
  Union2(PowerSet<A> &m, PowerSet<A> &m2) : m(m), m2(m2) { }
  bool Index(A a) const
  {
    return m.Index(a) || m2.Index(a);
  }
private:
  PowerSet<A> &m;
  PowerSet<A> &m2;
};


template<class A>
class IntersectionArray : public PowerSet<A>
{
public:
  IntersectionArray(Array<int,PowerSet<A>*> &arr) : arr(arr) { }
  bool Index(A a) const
  {
    bool bb = true;
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	PowerSet<A> *mem = arr.Index(i);
	bool b = mem->Index(a);
	bb &= b;
      }
    return bb;
  }
private:
  Array<int, PowerSet<A>*> &arr;
};

template<class A>
class UnionArray : public PowerSet<A>
{
public:
  UnionArray(Array<int,PowerSet<A>*> &arr) : arr(arr) { }
  bool Index(A a) const
  {
    bool bb = false;
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	PowerSet<A> *mem = arr.Index(i);
	bool b = mem->Index(a);
	bb |= b;
      }
    return bb;
  }
private:
  Array<int, PowerSet<A>*> &arr;
};


template<class A, class B>
class Equalizer : public PowerSet<A>
{
public:
  Equalizer(Compare<B> &comp, Function<A,B> &f1, Function<A,B> &f2) : comp(comp), f1(f1), f2(f2) { }
  bool Index(A a) const
  {
    return comp.Comp(f1.Index(a), f2.Index(a));
  }
private:
  Compare<B> &comp;
  Function<A,B> &f1;
  Function<A,B> &f2;
};

class DistanceFloat : public PowerSet<float>
{
public:
  DistanceFloat(float p, float delta) : p(p), delta(delta) { }
  bool Index(float x) const
  {
    return std::abs(x-p) < delta;
  }
private:
  float p, delta;
};

class DistancePoint2d : public PowerSet<Point2d>
{
public:
  DistancePoint2d(Point2d p, float r) : p(p), r(r) { }
  bool Index(Point2d x) const
  {
    return (x-p).Dist() < r;
  }
private:
  Point2d p;
  float r;
};

template<class E, class A, class B>
class Equalizer2 : public Function<PowerSet<B>*,PowerSet<E>*>
{
public:
  Equalizer2(Function<A,B> &f1, Function<A,B> &f2, Function<E,A> &e) : f1(f1), f2(f2), e(e), mem(0) { }
  PowerSet<E> *Index(PowerSet<B> *b) const
  {
    PowerSet<A> *f_1 = f1.Index(b);
    PowerSet<A> *f_2 = f2.Index(b);
    delete mem;
    mem = new Intersection<A>(*f_1,*f_2);
    PowerSet<E> *ee = e.Index(mem);
    return ee;
  }
private:
  InverseImage<A,B> f1;
  InverseImage<A,B> f2;
  InverseImage<E,A> e;
  mutable Intersection<A> *mem;
};
template<class E, class A, class B>
class Equalizer4 : public Function<E,B>
{
  Equalizer4(Function<A,B> &f1, Function<A,B> &f2, Function<E,A> &e, bool path_from_f2) : f1(f1), f2(f2), e(e), path_from_f2(path_from_f2) { }
  B Index(E e) const
  {
    if (path_from_f2) { return f2.Index(e.Index(e)); }
    return f1.Index(e.Index(e));
  }
private:
  Function<A,B> &f1;
  Function<A,B> &f2;
  Function<E,A> &e;
  bool path_from_f2;
};


template<class A, class B>
class InverseEqualizer : public Function<A, std::vector<A> >
{
public:
  InverseEqualizer(Function<A,B> &f, InvertableFunction<A,B> &g) : f(f), g(g) {}
  std::vector<A> Index(A a) const
  {
    B b = f.Index(a);
    return g.Inverse(b);
  }
private:
  Function<A,B> &f;
  InvertableFunction<A,B> &g;
};

template<class A, class B>
class CombineFunctions : public Function<A,std::pair<B,B> >
{
public:
  CombineFunctions(Function<A,B> &f, Function<A,B> &g) : f(f), g(g) { }
  std::pair<B,B> Index(A a) const
  {
    return std::make_pair(f.Index(a), g.Index(a));
  }
private:
  Function<A,B> &f;
  Function<A,B> &g;
};

template<class A, class B>
class RealGraph : public Function<A,std::pair<A,B> >
{
public:
  RealGraph(Function<A,B> &f) : f(f) { }
  std::pair<A,B> Index(A a) const
  {
    return std::make_pair(a,f.Index(a));
  }
private:
  Function<A,B> &f;
};
/*
class RealEqualizer
{
public:
  RealEqualizer(Function<A,B> &f1, Function<A,B> &f2) : gf1(f1), gf2(f2) { }
  
private:
  RealGraph<A,B> gf1;
  RealGraph<A,B> gf2;
};
*/
template<class A, class B, class C>
class InversePullback : public Function<A,std::vector<B> >
{
public:
  InversePullback(Function<A,C> &ac, InvertableFunction<B,C> &bc) : ac(ac), bc(bc) { }
  B Index(A a) const
  {
    C c = ac.Index(a);
    std::vector<B> b = bc.Inverse(c);
    return b;
  }
private:
  Function<A,C> &ac;
  InvertableFunction<B,C> &bc;
};

template<class T>
class ChooseElem : public PowerSet<T>
{
public:
  ChooseElem(T a) : a(a) { }
  bool Index(T a_) const { return a_==a; }
private:
  T a;
};

template<class A, class B>
class Equalizer3 : public PowerSet<std::pair<A,B> >
{
public:
  Equalizer3(Function<A,B> &f1, Function<A,B> &f2) : eq(f1,f2,id) { }
  bool Index(std::pair<A,B> &ab) const
  {
    ChooseElem<B> c(ab.second);
    PowerSet<A> *elem = eq.Index(&c);
    return elem->Index(ab.first);
  }
private:
  Equalizer2<A,A,B> eq;
  IdentityFunction<A> id;
};



struct ImgSelect
{
  int x;
  int y;
  int color_component;
};

class ImageFunction : public Function<ImgSelect, int>
{
public:
  ImageFunction(Bitmap<Color> &bm) : bm(bm) { }
  int Index(ImgSelect p) const
  {
    Color c = bm.Map(p.x,p.y);
    if (p.color_component == 0) return c.r;
    if (p.color_component == 1) return c.g;
    if (p.color_component == 2) return c.b;
    return 0;
  }
private:
  Bitmap<Color> &bm;
};
#if 0
class BlurFunction : public Function<ImgSelect, int>
{
public:
  BlurFunction(Bitmap<Color> &bm, float mult) : bm(bm) { }

  int Index(ImgSelect p) const
  {
    int x = p.x;
    int y = p.y;
    Color cx1 = bm.Map(x-1,y);
    Color cx2 = bm.Map(x+1,y);
    Color cy1 = bm.Map(x,y-1);
    Color cy2 = bm.Map(x,y+1);
    Color c = bm.Map(x,y);
    int val;
    if (p.color_component == 0)
      {
	val = ((cx1.r+cx2.r+cy1.r+cy2.r)*mult/4.0 + (1.0-mult)*c.r);
      }
    if (p.color_component == 1)
      {
	val = ((cx1.g+cx2.g+cy1.g+cy2.g)*mult/4.0 + (1.0-mult)*c.g);
      }
    if (p.color_component == 2)
      {
	val = ((cx1.b+cx2.b+cy1.b+cy2.b)*mult/4.0 + (1.0-mult)*c.b);
      }
    return val;
  }
private:
  Bitmap<Color> &bm;
};

#endif

class ConstantBitmap2 : public Bitmap<Color>
{
public:
  ConstantBitmap2(int sx, int sy, int r, int g, int b) : r(r), g(g), b(b) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    return Color(r,g,b);
  }
private:
  int sx,sy,r,g,b;
};

#if 0
class Sharpen : public Bitmap<Color>
{
public:
  Sharpen(const Bitmap<Color> &c) : c(c), img(c) { }
  int SizeX() const { return c.SizeX(); }
  int SizeY() const { return c.SizeY(); }
  Color Map(int x, int y) const
  {
    for(int r=0;r<255;r++)
      {
	ConstantBitmap2 b(r,0,0);
	BlurFunction blur(b, 0.5);
	Equalizer3<ImgSelect, int> eq(img, blur);
	
      }
  }
private:
  const Bitmap<Color> &c;
  IdentityFunction id;
  ImageFunction img;
};
#endif

template<class A, class B, class C>
class Pullback : private PowerSet<std::pair<A,B> >
{
public:
  Pullback(Compare<C> &comp, Function<A,C> &f1, Function<B,C> &f2) : comp(comp), f1(f1), f2(f2) { }

  bool Index(std::pair<A,B> p) const
  {
    return comp.Comp(f1.Index(p.first), f2.Index(p.second));
  }
private:
  Compare<C> &comp;
  Function<A,C> &f1;
  Function<B,C> &f2;
};

// P = std::pair<A,B>
template<class P, class A, class B, class C>
class Pullback2 : public Function<PowerSet<C>*,PowerSet<P>*>
{
public:
  Pullback2(Function<A,C> &f1, Function<B,C> &f2, Function<P,A> &p1, Function<P,B> &p2) : f1(f1), f2(f2), p1(p1), p2(p2), mem(0) { }
  PowerSet<P> *Index(PowerSet<C> *c) const
  {
    PowerSet<A> *f_1 = f1.Index(c);
    PowerSet<B> *f_2 = f2.Index(c);
    PowerSet<P> *p_1 = p1.Index(f_1);
    PowerSet<P> *p_2 = p2.Index(f_2);
    delete mem;
    mem = new Intersection<P>(p_1,p_2);
    return mem;
  }

private:
  InverseImage<A,C> f1;
  InverseImage<B,C> f2;
  InverseImage<P,A> p1;
  InverseImage<P,B> p2;
  mutable Intersection<P> *mem;
};
template<class P, class A, class B, class C>
class Pullback3 : public Function<P,C>
{
public:
  Pullback3(Function<A,C> &f1, Function<B,C> &f2, Function<P,A> &p1, Function<P,B> &p2, bool is_path_from_B) : is_path_from_B(is_path_from_B), f1(f1), f2(f2), p1(p1), p2(p2) { }
  C Index(P p) const
  {
    if (is_path_from_B) { return f2.Index(p2.Index(p)); }
    return f1.Index(p1.Index(p));
  }
private:
  bool is_path_from_B;
  Function<A,C> &f1;
  Function<B,C> &f2;
  Function<P,A> &p1;
  Function<P,B> &p2;
};

template<class P, class A, class B, class C>
class Pullback4I : public Function<P,std::pair<C,C> >
{
public:
  Pullback4I(Function<A,C> &f1, Function<B,C> &f2, Function<P,A> &p1, Function<P,B> &p2) : f1(f1), f2(f2),p1(p1), p2(p2) { }
  std::pair<C,C> Index(P p) const
  {
    A a = p1.Index(p);
    B b = p2.Index(p);
    //std::cout << "Pullback4Is:" << a.first << " " << a.second << " " << b << std::endl;
    C c1 = f1.Index(a);
    C c2 = f2.Index(b);
    //std::cout << "Pullback4I:" << c1 << " " << c2 << std::endl;
    return std::make_pair(c1,c2);
  }
private:
  Function<A,C> &f1;
  Function<B,C> &f2;
  Function<P,A> &p1;
  Function<P,B> &p2;
};
class PbMinus : public Function<std::pair<float,float>, float>
{
public:
  float Index(std::pair<float,float> p) const { return fabs(p.first - p.second); }
};

struct Float
{
  Float(float v) : value(v) { }
  float value;
  class FloatFunction : public Function<float,float>
  {
  public:
    FloatFunction(Function<Float,float> &f) : f(f) { }
    float Index(float x) const
    {
      Float a(x);
      return f.Index(a);
    }
  private:
    Function<Float,float> &f;
  };
  static Float SolveWithFailure(Function<Float,float> &f, Float t_0, Float t_1, bool &error) {
    FloatFunction ff(f);
    float x = ::SolveWithFailure(ff,t_0.value, t_1.value, error);
    Float fl(x);
    return fl;
  }
};

struct FloatPair
{
public:
  FloatPair(float xx, float yy) : x(xx), y(yy) { }
  float x,y;
  class FloatFunction : public Function<Point2d,float>
  {
  public:
    FloatFunction(Function<FloatPair,float> &f) : f(f) { }
    float Index(Point2d x) const
    {
      FloatPair a(x.x,x.y);
      return f.Index(a);
    }
  private:
    Function<FloatPair,float> &f;
  };
  static FloatPair SolveWithFailure(Function<FloatPair,float> &f, FloatPair t_0, FloatPair t_1, bool &error) {
    FloatFunction ff(f);
    Point2d t_0a = { t_0.x, t_0.y };
    Point2d t_1a = { t_1.x, t_1.y };
    Point2d x = ::SolveWithFailure(ff,t_0a, t_1a, error);
    FloatPair p(x.x,x.y);
    return p;
  }
};
struct FloatPoint
{
public:
  FloatPoint(Point p) : p(p) { }
  Point get() const { return p; }
  class FloatFunction : public Function<Point,float>
  {
  public:
    FloatFunction(const Function<FloatPoint,float> &f) : f(f) { }
    float Index(Point x) const
    {
      FloatPoint a(x);
      return f.Index(a);
    }
  private:
    const Function<FloatPoint,float> &f;
  };

  static FloatPoint SolveWithFailure(const Function<FloatPoint, float> &f, FloatPoint t_0, FloatPoint t_1, bool &error)
  {
    FloatFunction ff(f);
    Point t_0a(t_0.p.x, t_0.p.y, t_0.p.z);
    Point t_1a(t_1.p.x, t_1.p.y, t_1.p.z);
    Point p = ::SolveWithFailure(ff, t_0a, t_1a, error);
    FloatPoint pp(p);
    return pp;
  }
public:
  Point p;
};

// T = Float or T = FloatPair
// P = std::pair<A,B>
// A = anything
// B = anything
// C = anything
template<class T, class P, class A, class B, class C>
class FloatPullbackGeneric : public Element<T> // T=float, C = float
{
public:
  FloatPullbackGeneric(Function<A, C> &f1, 
		Function<B, C> &f2,
		Function<P, A> &p1,
		Function<P, B> &p2,
		Function<T, P> &t,
                Function<std::pair<C,C>, float> &pb_minus, 
		T t_0, 
		T t_1) : t(t), pb(f1,f2,p1,p2), pb1(t,pb), pb2(pb1, pb_minus), t_0(t_0), t_1(t_1), f1(f1), f2(f2), p1(p1), p2(p2) { }
  T Elem() const
  {
    T val = T::SolveWithFailure(pb2, t_0, t_1, b);
    return val;
  }
  bool Success() const { Elem(); return b; }
  P get_P() const { return t.Index(Elem()); }
  A get_A() const { return p1.Index(get_P()); }
  B get_B() const { return p2.Index(get_P()); }
  std::pair<C,C> get_pair() const 
  {
    return std::make_pair(f1.Index(get_A()), f2.Index(get_B()));
  }
private:
  Function<T, P> &t;
  Pullback4I<P,A,B,C> pb;
  //PbMinus m;

  FunctionCompose<T,P, std::pair<C,C> > pb1;
  FunctionCompose<T,std::pair<C,C>, float> pb2;
  T t_0, t_1;
  Function<A,C> &f1;
  Function<B,C> &f2;
  Function<P,A> &p1;
  Function<P,B> &p2;
  mutable bool b;
};
template<class P, class A, class B>
class FloatPullback : public FloatPullbackGeneric<Float,P,A,B, float>
{
public:
  FloatPullback(Function<A, float> &f1, 
		Function<B, float> &f2,
		Function<P, A> &p1,
		Function<P, B> &p2,
		Function<float, P> &t,
		float t_0, 
		float t_1) : t_0a(t_0), t_1a(t_1), FloatPullbackGeneric<Float,P,A,B,float>(f1,f2,p1,p2,t,minus,t_0a,t_1a) { }
private:
  PbMinus minus;
  Float t_0a;
  Float t_1a;
};
class PointPointDistance : public Function<std::pair<Point,Point>, float>
{
public:
  float Index(std::pair<Point,Point> p) const
  {
    //std::cout << p.first << " " << p.second << std::endl;
    Vector v = p.first - p.second;
    return sqrt(v.dx*v.dx+v.dy*v.dy+v.dz*v.dz);
  }
};

class Point2dPoint2dDistance : public Function<std::pair<Point2d,Point2d>, float>
{
public:
  float Index(std::pair<Point2d,Point2d> p) const
  {
    Vector2d v = p.first - p.second;
    return sqrt(v.dx*v.dx+v.dy*v.dy);
  }
};


template<class P, class A, class B>
class DistancePullback : public FloatPullbackGeneric<FloatPair, P,A,B, Point>
{
public:
  DistancePullback(Function<A, Point> &f1, 
		Function<B, Point> &f2,
		Function<P, A> &p1,
		Function<P, B> &p2,
		Function<FloatPair, P> &t,
                FloatPair t_0, 
		FloatPair t_1) : FloatPullbackGeneric<FloatPair, P,A,B,Point>(f1,f2,p1,p2,t,dist,t_0,t_1) { }
private:
  PointPointDistance dist;
};

template<class P, class A, class B>
class Distance2dPullback : public FloatPullbackGeneric<FloatPair, P,A,B, Point2d>
{
public:
  Distance2dPullback(Function<A, Point2d> &f1, 
		Function<B, Point2d> &f2,
		Function<P, A> &p1,
		Function<P, B> &p2,
		Function<FloatPair, P> &t,
                FloatPair t_0, 
		FloatPair t_1) : FloatPullbackGeneric<FloatPair, P,A,B,Point2d>(f1,f2,p1,p2,t,dist,t_0,t_1) { }
private:
  Point2dPoint2dDistance dist;
};

template<class P, class A, class B>
class Distance3dPullback : public FloatPullbackGeneric<FloatPoint, P,A,B, Point>
{
public:
  Distance3dPullback(Function<A, Point> &f1, 
		Function<B, Point> &f2,
		Function<P, A> &p1,
		Function<P, B> &p2,
		Function<FloatPoint, P> &t,
                FloatPoint t_0, 
		FloatPoint t_1) : FloatPullbackGeneric<FloatPoint, P,A,B,Point>(f1,f2,p1,p2,t,dist,t_0,t_1) { }
private:
  PointPointDistance dist;
};





template<class T, class K>
class Fst : public Function<std::pair<T,K>, T>
{
public:
  T Index(std::pair<T,K> p) const { return p.first; }
};

template<class T, class K>
class Snd : public Function<std::pair<T,K>, K>
{
public:
  K Index(std::pair<T,K> p) const { return p.second; }
};




template<class P>
class DistPullback3d : public FloatPullback<P, Point, Point>
{
  DistPullback3d(Function<Point, float> &f1, 
		  Function<Point, float> &f2,
		  Function<P, Point> &p1,
		  Function<P, Point> &p2,
		  Function<float, P> &t) : 
    FloatPullback<std::pair<Point,Point>, Point, Point>(f1,f2,p1,p2,t) { }
};



template<class P, class A, class B, class C>
class Pullback4 : public Function<P,InvertableFunction<C,C>*>, private InvertableFunction<C,C>
{
public:
  Pullback4(Function<A,C> &f1, Function<B,C> &f2, Function<P,A> &p1, Function<P,B> &p2, bool is_path_from_B) : is_path_from_B(is_path_from_B), f1(f1), f2(f2), p1(p1), p2(p2) { }
  InvertableFunction<C,C> *Index(P p) const
  {
    
  }
private:
  bool is_path_from_B;
  Function<A,C> &f1;
  Function<B,C> &f2;
  Function<P,A> &p1;
  Function<P,B> &p2;
};



template<class A, class C>
class EquivalenceRelation : public PowerSet<std::pair<A,A> >
{
public:
  EquivalenceRelation(Compare<C> &comp, Function<A,C> &f) : pb(comp, f, f) { }
  bool Index(std::pair<A,A> p) const
  {
    return pb.Index(p);
  }
private:
  Pullback<A,A,C> pb;
};

class FloatCompare : public Compare<float>
{
public:
  bool Comp(float t1, float t2) const
  {
    return fabs(t1-t2) < 0.001;
  }
};

class Distance2dA : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return sqrt(p.x*p.x+p.y*p.y);
  }
};

class Distance3d : public Function<Point, float>
{
public:
  float Index(Point p) const
  {
    return sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
  }
};

class PointCollectionDistance : public Function<Point, float>
{
public:
  PointCollectionDistance(PointCollection &coll) : coll(coll) { }
  float Index(Point p) const
  {
    float dist = 0.0;
    int s = coll.Size();
    for(int i=0;i<s;i++)
      {
	Point c = coll.Index(i);
	Vector v = p - c;
	dist += v.Dist();
      }
    return dist;
  }  
private:
  PointCollection &coll;
};

class PointCollectionNormal : public Function<Point, Vector>
{
public:
  PointCollectionNormal(PointCollection &coll, float dist1, float dist2) : coll(coll), dist1(dist1), dist2(dist2) { }
  Vector Index(Point p) const
  {
    Vector dist(0.0, 0.0, 0.0);
    int s = coll.Size();
    float d = 0.0;
    for(int i=0;i<s;i++)
      {
	Point c = coll.Index(i);
	Vector v = p - c;
	d += v.Dist();
	if (d >= dist1 && d <= dist2)
	  {
	    dist += v;
	  }
      }
    return dist;
  }
private:
  PointCollection &coll;
  float dist1, dist2;
};

class Range2 : public PowerSet<float>
{
public:
  Range2(float start, float end) : start(start), end(end) { }
  bool Index(float x) const
  {
    return x >= start && x <=end;
  }
private:
  float start,end;
};

class Step : public PowerSet<float>
{
public:
  Step(float x, bool flip) : x(x) { }
  bool Index(float xx) const
  {
    if (flip)
      {
	return xx > x;
      }
    else
      {
	return xx < x;
      }
  }
private:
  float x;
  bool flip;
};

class Angle : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return atan2(p.y,p.x);
  }
};

class XCoord2d : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return p.x;
  }
};

class YCoord2d : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return p.y;
  }
};


class XCoord3d : public Function<Point, float>
{
public:
  float Index(Point p) const
  {
    return p.x;
  }
};

class YCoord3d : public Function<Point, float>
{
public:
  float Index(Point p) const
  {
    return p.y;
  }
};

class ZCoord3d : public Function<Point, float>
{
public:
  float Index(Point p) const
  {
    return p.z;
  }
};

class Repeat2 : public PowerSet<float>
{
public:
  Repeat2(PowerSet<float> &mem, float sizex) : mem(mem), sizex(sizex) { }
  bool Index(float t) const
  {
    while(t>sizex)
      t-=sizex;
    while(t<0.0)
      t+=sizex;
    return mem.Index(t);
  }
  
private:
  PowerSet<float> &mem;
  float sizex;
};

template<class A, class T>
class FunctionCall : public Element<T>
{
public:
  FunctionCall(Function<A, T> &f, A a) : f(f), a(a) { }
  T Elem() const
  {
    return f.Index(a);
  }
private:
  Function<A,T> &f;
  A a;
};

class InverseImageSubset2d : public PowerSet<Point2d>
{
public:
  InverseImageSubset2d(Function<Point2d, float> &f, PowerSet<float> &m) : img(f), m(m) { }
  bool Index(Point2d p) const
  {
    const PowerSet<Point2d> *mem = img.Index(&m);
    return mem->Index(p);
  }
private:
  InverseImage<Point2d, float> img;
  PowerSet<float> &m;
};

class BitmapPowerSet : public PowerSet<Pos>
{
public:
  BitmapPowerSet(Bitmap<bool> &bm) : bm(bm) { }
  bool Index(Pos p) const
  {
    return bm.Map(p.x, p.y);
  }
private:
  Bitmap<bool> &bm;
};

template<class T>
class BitmapInverseImage : public Function<T,bool>
{
public:
  BitmapInverseImage(Bitmap<bool> &bm, Function<T,Pos> &f) : mem(bm), inverse(f) { }
  bool Index(T t) const
  {
    PowerSet<T> *m = inverse.Index(mem);
    return m->Index(t);
  }
private:
  BitmapPowerSet mem;
  InverseImage<T,Pos> inverse;
};

class ContinuousBitmapPowerSet : public PowerSet<Point2d>
{
public:
  ContinuousBitmapPowerSet(ContinuousBitmap<bool> &bm) : bm(bm) { }
  bool Index(Point2d p) const
  {
    return bm.Map(p.x, p.y);
  }
private:
  ContinuousBitmap<bool> &bm;
};

template<class T>
class ContinuousBitmapInverseImage : public Function<T,bool>
{
public:
  ContinuousBitmapInverseImage(ContinuousBitmap<bool> &bm, Function<T, Point2d> &f) : mem(bm), inverse(f) { }
  bool Index(T t) const
  {
    PowerSet<T> *m = inverse.Index(mem);
    return m->Index(t);
  }
private:
  ContinuousBitmapPowerSet mem;
  InverseImage<T,Point2d> inverse;
};


class LineIn2dScanLine : public Function<float, bool> // x -> bool
{
public:
  LineIn2dScanLine(Point2d p1, Point2d p2, float y) : /*p1(p1), p2(p2),*/ line(p1,p2), image(line),y(y) { }
  bool Index(float x) const
  {
    Step st(y,false /*(p1.y<p2.y&&p1.x<p2.x) || (p2.y<p1.y&&p2.x<p1.x)*/);
    PowerSet<float> *x_mem = image.Index(&st);
    return x_mem->Index(x);
  }
private:
  //Point2d p1,p2;
  //ConstantFloatFunction c;
  LineIn2d2 line;
  //IdentityFunction<float> id;
  InverseImage<float,float> image;
  //Equalizer2<float,float,float> eq;
  float y;
};

class PolygonLines : public Line2dCollection
{
public:
  PolygonLines(PlanePoints2d &points, int num) : points(points),num(num) { }
  int Size() const 
  { 
    int i2 = 0;
    int count=0;
    for(;i2<points.Size();i2++)
      {
	if (count == num) break;
	if (i2!=0 && points.Type(i2)==PlanePoints2d::PlanePointsType::EMove)
	  count++;
      }
    int i3 = 0;
    for(;i2<points.Size();i2++,i3++)
      {
	if (i2!=0 && points.Type(i2)==PlanePoints2d::PlanePointsType::EMove) { break; }
      }
    return i3-1;
  }
  Line2d Index(int i) const
  {
    int i2 = 0;
    int count = 0;
    for(;i2<points.Size();i2++)
      {
	if (count == num) break;
	if (i2!=0 && points.Type(i2)==PlanePoints2d::PlanePointsType::EMove)
	  count++;
      }
    int pos = i2+i;
    Point2d p1 = points.Map(pos);
    Point2d p2 = points.Map(pos+1);
    Line2d l;
    l.p1 = p1;
    l.p2 = p2;
    return l;
  }
private:
  PlanePoints2d &points;
  int num;
};

class PolygonFill : public ContinuousBitmap<bool>
{
public:
  PolygonFill(float sx, float sy, Line2dCollection &coll) : sx(sx), sy(sy), coll(coll), swapped(coll) { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  bool Map(float x, float y) const
  {
    //std::cout << "PolygonFill" << x << " " << y << std::endl;
    FilterLines filter(swapped, y);
    bool res = false;
    int size = coll.Size();
    for(int i=0;i<size;i++)
      {
	bool use = filter.Index(i);
	//std::cout << "Use: " << use << " ";
	if (use)
	  {
	    Line2d l = coll.Index(i);
	    //std::cout << "l: " << l.p1.x << "," << l.p1.y << " " << l.p2.x << "," << l.p2.y << " ";
	    LineIn2dScanLine line(l.p1,l.p2,y);
	    //std::cout << "line: " << line.Index(0) << " " << line.Index(20) << std::endl;
	    bool b = line.Index(x);
	    if (line.Index(0)==true && line.Index(sx-1)==false) b = !b;
	    //std::cout << "b: " << b << std::endl;
	    //res = res && b;
	    if (b)
	    {
	      	res = !res;
	    }
	  }
      }
    return res;
  }
private:
  float sx,sy;
  Line2dCollection &coll;
  mutable SwapLine2dCollection swapped;
};

class TextureWithHole : public Bitmap<Color>
{
public:
  TextureWithHole(Bitmap<Color> &orig, PointCollection2d &coll) : cont(orig, orig.SizeX(),orig.SizeY()), lines(coll, true), poly(orig.SizeX(), orig.SizeY(), lines), anot(cont, poly), sample(anot, orig.SizeX(), orig.SizeY()) { }
  int SizeX() const { return sample.SizeX(); }
  int SizeY() const { return sample.SizeY(); }
  Color Map(int x, int y) const { return sample.Map(x,y); }

private:
  ContinuousBitmapFromBitmap<Color> cont;
  ContinuousLines2d lines;
  PolygonFill poly;
  AndNotColorContinuousBitmap anot;
  SampleBitmap sample;
};

class GradientWithHole : public Bitmap<Color>
{
public:
  GradientWithHole(int sx, int sy, Point2d p1, Point2d p2, Color c1, Color c2,
		   PointCollection2d &points)
    : grad(float(sx), float(sy), p1, p2, c1,c2),
      sample(grad, sx, sy),
      hole(sample, points) { }
  int SizeX() const { return hole.SizeX(); }
  int SizeY() const { return hole.SizeY(); }
  Color Map(int x, int y) const { return hole.Map(x,y); }

private:
  GradientBitmap grad; // ContinuousBitmap<Color>
  SampleBitmap sample;
  TextureWithHole hole;
};

//
// Moving objects
//
class MovingObject
{
public:
  
};
template<class A>
class MinusFunction : public Function<A, float>
{
public:
  MinusFunction(Function<A,float> &f, Function<A,float> &g) : f(f), g(g) { }
  float Index(A x) const
  {
    return f.Index(x) - g.Index(x);
  }
private:
  Function<A,float> &f;
  Function<A,float> &g;
};


// 1) Screen surface Point2d
// 2) 3d world coordinates Point3d
// 3) 3d object coordinates Point3d
// 4) 2d texture coordinates Point2d
// 5) Texture pixel Color

//
// Polynomial equation solving exactly
//

class ConstantFunction : public Function<float, float>
{
public:
  ConstantFunction(float val) : val(val) { }
  float Index(float x) const { return val; }
private:
  float val;
};

class SquareFunction : public InvertableFunction<float,float>
{ // x*x
public:
  float Index(float x) const { return x*x; }
  std::vector<float> Inverse(float x) const
  {
    std::vector<float> v;
    float val = sqrt(x);
    v.push_back(val);
    v.push_back(-val);
    return v;
  }
};

class TimesConstantFunction : public InvertableFunction<float, float>
{ // *c
public:
  TimesConstantFunction(float c) : c(c) { }
  float Index(float x) const { return x*c; }
  std::vector<float> Inverse(float x) const
  {
    std::vector<float> v;
    x/=c;
    v.push_back(x);
    return v;
  }
private:
  float c;
};

template<class X, class Y>
class SumXFunctionPullback : public Function<Y, std::vector<X> > // y|->x
{ // f(x) + g(y) = h(y)
public:
  SumXFunctionPullback(InvertableFunction<X, float> &f, Function<Y, float> &g, Function<Y, float> &h) : minus(h,g), pb(minus, f) { }
  std::vector<X> Index(Y x) const
  {
    return pb.Index(x);
  }
private:
  MinusFunction<Y> minus; // Y->float
  InversePullback<Y, X, float> pb;
};

template<class X, class Y, class Z>
class SumXFunctionPullback2 : public Function<std::pair<Y,Z>, std::vector<X> >
{ // f(x) + g(y,z) = h(y,z)
public:
  SumXFunctionPullback2(InvertableFunction<X, float> &f, 
			Function< std::pair<Y,Z>, float> &g, 
                        Function< std::pair<Y,Z> , float > &h) 
    : pb(f,g,h) { }
  std::vector<X> Index(std::pair<Y,Z> p) const
  {
  return pb.Index(p);
  }
private:
SumXFunctionPullback<X, std::pair<Y,Z> > pb;
};

class TreFoilKnot : public Curve<Point>
{
public:
  float Size() const { return 2.0*3.14159; }
  Point Index(float t) const
  {
    Point p;
    p.x = (2.0*cos(3*t))*cos(2.0*t);
    p.y = (2.0+cos(3*t))*sin(2.0*t);
    p.z = sin(3.0*t);
    return p;
  }
};

class ChooseX2d : public Function<float, float>
{
public:
  ChooseX2d(Function<float, Point2d> &f) : f(f) { }
  float Index(float x) const { return f.Index(x).x; }
private:
  Function<float, Point2d> &f;
};

class ChooseY2d : public Function<float, float>
{
public:
  ChooseY2d(Function<float, Point2d> &f) : f(f) { }
  float Index(float x) const { return f.Index(x).y; }
private:
  Function<float, Point2d> &f;
};


inline float FindRoot(Function<float,float> &f, float x) // f: x->y, returns x
{
  DerivateFunction df(f, 0.01);
  for(int i=0;i<20;i++)
    {
      x = x - f.Index(x)/df.Index(x);
    }
  return x;
}
inline float FindMinMaxPoint(Function<float, float> &f, float x)
{
  DerivateFunction df(f, 0.01);
  DerivateFunction ddf(df, 0.01);
  for(int i=0;i<20;i++)
    {
      x = x - df.Index(x)/ddf.Index(x);
    }
  return x;  
}


//
// Equation solving using expressions
// 
enum ExprType
  {
    EX,
    EY,
    ENum,
    EPlus,
    EPow2,
    EPow3,
    EMul
  };

class Expr 
{
public:
  virtual int ChildCount() const=0;
  virtual Expr *Child(int i) const=0;
  virtual int Type() const=0;
};

class X : public Expr 
{
public:
  virtual int ChildCount() const { return 0; }
  virtual Expr *Child(int i) const { return 0; }
  virtual int Type() const { return EX; }
};
class Y : public Expr 
{
public:
  virtual int ChildCount() const { return 0; }
  virtual Expr *Child(int i) const { return 0; }
  virtual int Type() const { return EY; }
};
class Num : public Expr
{ 
public:
  Num(float c) : c(c) { } 
  virtual int ChildCount() const { return 0; }
  virtual Expr *Child(int i) const { return 0; }
  virtual int Type() const { return ENum; }

  float c; 
};

class Plus2 : public Expr { 
public:
  Plus2(Expr &t, Expr &k) : t(t), k(k) { } 
  virtual int ChildCount() const { return 2; }
  virtual Expr *Child(int i) const { if (i==0) return &t; else return &k; }
  virtual int Type() const { return EPlus; }

  Expr &t; 
  Expr &k; 
};

class Pow2 { 
public:
  Pow2(Expr &t) : t(t) { } 
  virtual int ChildCount() const { return 1; }
  virtual Expr *Child(int i) const { return &t; }
  virtual int Type() const { return EPow2; }
  Expr &t; 
};

class Pow3 { 
public:
  Pow3(Expr &t) : t(t) { } 
  virtual int ChildCount() const { return 1; }
  virtual Expr *Child(int i) const { return &t; }
  virtual int Type() const { return EPow3; }
  Expr &t; 
};

class Mul2 { 
public:
  Mul2(Expr &t, Expr &k) : t(t), k(k) { } 
  virtual int ChildCount() const { return 2; }
  virtual Expr *Child(int i) const { if (i==0) return &t; else return &k; }
  virtual int Type() const { return EPlus; }
  
  Expr &t; Expr &k; 
};

template<class T, class K>
class Eq
{
public:
  Eq(Expr &t, Expr &k) : t(t), k(k) { }
  Expr &t;
  Expr &k;
};

//
// Support functions for pullback and equalizer
// 
//
class FloatSpace : public Function<Point, float>
{
public:
  float Index(Point p) const { return Get(p); }
  virtual float Get(Point p) const=0;
};

class FloatSpaceFunction : public Function<Point,float>
{
public:
  FloatSpaceFunction(FloatSpace &sp) : sp(sp) { }
  float Index(Point p) const { return sp.Get(p); }
private:
  FloatSpace &sp;
};

class TimedFloatSpace
{
public:
  virtual float Get(Point p, float time) const=0;
};

class DistanceFloatSpace : public FloatSpace
{
public:
  DistanceFloatSpace(Point center) : center(center) { }
  virtual float Get(Point p) const
  {
    p-=center;
    return Vector(p).Dist();
  }
private:
  Point center;
};

class FloatSpaceY : public FloatSpace
{
public: // values are 0.0 -- 1.0
  FloatSpaceY(float start_y, float end_y) : start_y(start_y), end_y(end_y) { }
  virtual float Get(Point p) const 
  {
    if (p.y < start_y) return 0.0;
    if (p.y > end_y) return 1.0;
    return (p.y-start_y)/(end_y-start_y); 
  }
private:
  float start_y;
  float end_y;
};

class ConstantFloatSpace : public FloatSpace
{
public:
  ConstantFloatSpace(float val) : x(val) { }
  float Get(Point p) const { return x; }

private:
  float x;
};

class UpgradeFloatSpace : public FloatSpace
{
public:
  UpgradeFloatSpace(FloatSpace &orig, VolumeObject &v, float value) : v(v), value(value), orig(orig) { }
  float Get(Point p) const
  {
    float val = orig.Get(p);
    bool b = v.Inside(p);
    if (val < value && b)
      {
	return value;
      }
    return val;
  }
private:
  VolumeObject &v;
  float value;
  FloatSpace &orig;
};

class DowngradeFloatSpace : public FloatSpace
{
public:
  DowngradeFloatSpace(FloatSpace &orig, VolumeObject &v, float value) : v(v), value(value), orig(orig) { }
  float Get(Point p) const
  {
    float val = orig.Get(p);
    bool b = v.Inside(p);
    if (val > value && b)
      {
	return value;
      }
    return val;
  }
private:
  VolumeObject &v;
  float value;
  FloatSpace &orig;
};

class UpgradeTimedFloatSpace : public TimedFloatSpace
{
public:
  UpgradeTimedFloatSpace(TimedFloatSpace &orig, TimedVolumeObject &v, float value) : v(v), value(value), orig(orig) { }
  float Get(Point p, float time) const
  {
    float val = orig.Get(p, time);
    bool b = v.Inside(p, time);
    if (val < value && b)
      {
	return value;
      }
    return val;
  }
private:
  TimedVolumeObject &v;
  float value;
  TimedFloatSpace &orig;
};

#if 0
class DowngradeTimedFloatSpace : public FloatSpace
{
public:
  DowngradeTimedFloatSpace(TimedFloatSpace &orig, TimedVolumeObject &v, float value) : v(v), value(value), orig(orig) { }
  float Get(Point p, float time) const
  {
    float val = orig.Get(p, time);
    bool b = v.Inside(p, time);
    if (val > value && b)
      {
	return value;
      }
    return val;
  }
private:
  TimedVolumeObject &v;
  float value;
  TimedFloatSpace &orig;
};
#endif
class MinFloatSpace : public FloatSpace
{
public:
  MinFloatSpace(FloatSpace &s1, FloatSpace &s2) : s1(s1), s2(s2) { }
  float Get(Point p) const
  {
    return std::min(s1.Get(p), s2.Get(p));
  }
private:
  FloatSpace &s1;
  FloatSpace &s2;
};

class MaxFloatSpace : public FloatSpace
{
public:
  MaxFloatSpace(FloatSpace &s1, FloatSpace &s2) : s1(s1), s2(s2) { }
  float Get(Point p) const
  {
    return std::max(s1.Get(p), s2.Get(p));
  }
private:
  FloatSpace &s1;
  FloatSpace &s2;
};

class MinTimedFloatSpace : public TimedFloatSpace
{
public:
  MinTimedFloatSpace(TimedFloatSpace &s1, TimedFloatSpace &s2) : s1(s1), s2(s2) { }
  float Get(Point p, float time) const
  {
    return std::min(s1.Get(p, time), s2.Get(p, time));
  }
private:
  TimedFloatSpace &s1;
  TimedFloatSpace &s2;
};

class MaxTimedFloatSpace : public TimedFloatSpace
{
public:
  MaxTimedFloatSpace(TimedFloatSpace &s1, TimedFloatSpace &s2) : s1(s1), s2(s2) { }
  float Get(Point p, float time) const
  {
    return std::max(s1.Get(p, time), s2.Get(p, time));
  }
private:
  TimedFloatSpace &s1;
  TimedFloatSpace &s2;
};

class OneMinusFloatSpace : public FloatSpace
{
public:
  OneMinusFloatSpace(FloatSpace &s1) : s1(s1) { }
  float Get(Point p) const
  {
    return 1.0-s1.Get(p);
  }
private:
  FloatSpace &s1;
};

class OneMinusTimedFloatSpace : public TimedFloatSpace
{
public:
  OneMinusTimedFloatSpace(TimedFloatSpace &s1) : s1(s1) { }
  float Get(Point p, float time) const
  {
    return 1.0-s1.Get(p, time);
  }
private:
  TimedFloatSpace &s1;
};

class PointConfigurator
{
public:
  virtual void SetPoint(Point p)=0;
};

class SpherePointConfigurator : public PointConfigurator
{
public:
  
};


struct ObjectProperties
{
public:
  VolumeObject *volume;
  VolumeObject *surfacepoints;
  ContinuousVoxel<Vector> *normals;
  ContinuousVoxel<Color> *color;
  ContinuousVoxel<float> *refletivity; // the percentage which is used to determine which part of colour will be reflected and which part is colour.
  //ContinuousVoxel<int> *blurcount;
  //ContinuousVoxel<float> *blur_dist;
};

Color ColorCombine(Color c1, Color c2, float val);

struct SphereProperties
{
public:
  SphereProperties(Point center, float radius, float gap)
  {
    volume = new SphereVolume(center, radius);
    surfacepoints = new SphereSurfaceVolume(center, radius, gap);
    normals = new SphereNormalContinuousVoxel(center);
  }
  SphereVolume *volume; // VolumeObject
  SphereSurfaceVolume *surfacepoints; // VolumeObject
  SphereNormalContinuousVoxel *normals; // ContinuousVoxel<Vector>  
};
ObjectProperties ToObj(SphereProperties &sp);
Vector Reflect(Point ray_pos, Vector ray_dir, Point hit_pos, Vector normal);

template<class T, class K>
class Diagonal : public Function<T,std::pair<K,K> >
{
public:
  Diagonal(Function<T,K> &f) : f(f) { }
  std::pair<K,K> Index(T t) const
  {
    K k = f.Index(t);
    return std::make_pair(k,k);
  }
private:
  Function<T,K> &f;
};

#if 0
class RaySphereIntersection
{
public:
  RaySphereIntersection(Point center, float radius, 
			Function<float, Point> &ray, 
			float t_0, float t_1)
    : dist(center), dist_f(dist), surf(radius), surf_f(surf), 
      diag(ray), pb(dist_f, surf_f, fst, snd, diag, t_0, t_1) { }
  Point IntersectionPoint() const { return pb.get_A(); }
private:
  DistanceFloatSpace dist; // Point center needed
  FloatSpaceFunction dist_f;
  ConstantFloatSpace surf; // float needed
  FloatSpaceFunction surf_f;
  Fst<Point,Point> fst;
  Snd<Point,Point> snd;
  Diagonal<float, Point> diag;
  FloatPullback<std::pair<Point,Point>, Point, Point> pb;
};
#endif

class AFunction : public Function<float, float>
{
public:
  AFunction(Function2<float,float,float> &f, float y) : f(f), y(y) { }
  float Index(float x) const
  {
    return f.Index(x,y);
  }
private:
  Function2<float,float,float> &f;
  float y;
};

class BFunction : public Function<float, float>
{
public:
  BFunction(Function2<float,float,float> &f, float x) : f(f), x(x) { }
  float Index(float y) const
  {
    return f.Index(x,y);
  }
private:
  Function2<float,float,float> &f;
  float x;
};

class PXFunction : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return p.x;
  }
};

class PYFunction : public Function<Point2d, float>
{
public:
  float Index(Point2d p) const
  {
    return p.y;
  }
};

class XProdFunction : public Function<float, Point2d>
{
public:
  XProdFunction(float y) : y(y) { }
  Point2d Index(float x) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
private:
  float y;
};

class YProdFunction : public Function<float, Point2d>
{
public:
  YProdFunction(float x) : x(x) { }
  Point2d Index(float y) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
private:
  float x;
};


struct ExactResult
{
  float x,y; // only valid for border points
};

#if 0
class FunctionPlaneIntersection : public Function2<float, float, std::pair<Point2d, Point2d> >
{
public:
  FunctionPlaneIntersection(Function2<float, float, float> &f, // (x,z)->y
			    Range rx, Range rz) : fa(f, p.y), fb(f,p.x), rx(rx), rz(rz) { }
  std::pair<Point2d,Point2d> Index(float x, float y) const
  {
    XProdFunction xprod(y);
    YProdFunction yprod(x);
    FloatPullback xpb(fa,fb, pxf, pyf, xprod, rx.start, rx.end);
    FloatPullback ypb(fa,fb, pxf, pyf, yprod, rz.start, rz.end);
    Point2d r1;
    r1.x = xpb.Elem();
    r1.y = y;
    Point2d r2;
    r2.x = x;
    r2.y = ypb.Elem();
    return std::make_pair(r1,r2);
  }
  

private:
  AFunction fa;
  BFunction fb;
  PXFunction pxf;
  PYFunction pyf;
  Range rx,rz;
};
#endif

class Point2dDistance : public Function<std::pair<Point2d,Point2d>, float>
{
public:
  float Index(std::pair<Point2d,Point2d> p) const
  {
    Point2d d;
    d.x = p.first.x-p.second.x;
    d.y = p.first.y-p.second.y;
    return sqrt(d.x*d.x+d.y*d.y);
  }
};
#if 0
class IsSpecialPoint : public Function2<float,float, bool>
{
public:
  IsSpecialPoint(Function2<float,float,float> &f, Range rx,Range ry, float dist_required) : sect(f,rx,ry), dist_required(dist_required) { }
  bool Index(float x, float y) const
  {
    std::pair<Point2d, Point2d> p = sect.Index(x,y);
    float d = dist.Index(p);
    return d<dist_required;
  }
private:
  FunctionPlaneIntersection sect;
  Point2dDistance dist;
  float dist_required;
};
#endif

//
// Proper root finding requires
//

// 1) f(x,y,z)=0
// 2) graph. If f :: RxRxR->R, then we have 4d space (RxRxR)xR
// 3) 2nd part of graph, a function RxRxR->(RxRxRxR)-->2
// 4) (id_RxRxR,f)
// 5) x|->(x,x)
// 6) intersection of 4d spaces.

// Intersection is:
// T------>A
// T-->K-->A
// T-->S-->A

//
// 3d version works with a "loop". Put a loop to origo, and a circle in far away.
// Then run a root-finding in each position in the loop.
//
// 4d version works with sphere
// then run root-finding with tiny sphere and large sphere.

#if 0
template<class T, class K>
class GraphOfFunction : public Function<T, std::pair<T,K> >
{
public:
  GraphOfFunction(Function<T,K> &f) : f(f) { }
  std::pair<T,K> Index(T t) const
  {
    return std::make_pair(t,f.Index(t));
  }
private:
  Function<T,K> &f;
};

class Graph3d : public GraphOfFunction<Point,float>
{
};

template<class S, class K>
struct GraphPair
{
  GraphPair(Function<T,K> &f1, Function<T,K> &f2) : gf1(f1), gf2(f2) { }
  std::pair<T,K> f1(S t) const { return gf1.Index(t); }
  std::pair<T,K> f2(S t) const { return gf2.Index(t); }
private:
  GraphOfFunction gf1, gf2;
};

template<class T, class S>
class Intersection : public GraphPair<S,K>
{
public:
  virtual S get_1(T t) const=0;
  virtual S get_2(T t) const=0;

  std::pair<T,K> getA(T t) const { return f1(get_1(t)); }
  std::pair<T,K> getB(T t) const { return f2(get_2(t)); }

  // can be used to draw a picture.
  bool IsRoot(T t) const {
    return fabs(getA(t).second-getB(t).second)<0.001;
  }
};

class Sphere
{
public:
  Sphere(Point center, float radius) : center(center), radius(radius) { }
  Point get_p(float x, float y)
  {
  }
};

class EqSolver3d : public Function<std::pair<float,float>, float>
{
public:
  EqSolver3d(Function<Point, float> &f) : f(f) { }
  LineIn3d line(float x, float y) const
  {
    Point p1 = tiny.get_p(x,y);
    Point p2 = large.get_p(x,y);
    return l;
  }
  float Index(std::pair<float,float> p) const
  {
    float x = p.first;
    float y = p.second;
    Point p1 = tiny.get_p(x,y);
    Point p2 = large.get_p(x,y);
    LineIn3d ln(p1,p2);
    FunctionCompose<float,float,float> comp(ln, f);
    float s = ::Solve(comp, 0.0, 1.0);
    return s;
  }
public:
  Sphere tiny;
  Sphere large;
  Function<Point,float> &f;
};



template<class T>
class OpValue
{
public:
  virtual bool Value(T t) const=0;
};

template<class A, class B>
class OpFunction
{
public:
  virtual OpValue<A> *OpMap(OpValue<B> *value) const=0;
};

class OpArea // upper limit
{
};

class OpElem // lower limit
{
};

//
// Differential equations
//

// triangle dy/dx, tangent.
// expression e with x and y as variables with (x,y) : RxR
// dy/dx = e.
// SOLUTION:
// dy/dx = e/1.
// dy=e && dx=1.


class LineLineIntersection
{
public:
  LineLineIntersection(Point2d l1p1, Point2d l1p2,
		       Point2d l2p1, Point2d l2p2)
    : l1p1(l1p1), l1p2(l1p2), l2p1(l2p1), l2p2(l2p2) { }
  
  Point2d IntersectionPoint() const
  {
    float x1 = l1p1.x;
    float y1 = l1p1.y;
    float x2 = l1p2.x;
    float y2 = l1p2.y;
    float x3 = l2p1.x;
    float y3 = l2p1.y;
    float x4 = l2p2.x;
    float y4 = l2p2.y;
    // thanks to wikipedia line-line intersection for this stuff.
    float xupper =(x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4);
    float xlower = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    float xx = xupper/xlower;
    float yupper = (x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4);
    float ylower = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    float yy = yupper/ylower;
    Point2d p;
    p.x = xx;
    p.y = yy;
    return p;
  }

private:
  Point2d l1p1, l1p2;
  Point2d l2p1, l2p2;
};


class SquareWithTriangle
{
public:
  // x^2+y^2=z^2
  // sin x = y/z
  // cos x = x/z
  // tan x = x/y
  // multiplication: area = x*y
  // factoring: A => (X,Y). x*y |- (x,y)
  // Question: How is a*b |-> z link working? It is same as factoring composed with atan(x,y).
};


//
// FULL CATEGORY IMPLEMENTATION
//

template<template<> class Container, class A, class B>
class CATArrow
{
public:
  virtual B Func(A a) const=0;
  virtual Container<std::vector<A> > BackwardFunc(Container<B> cont) const=0;
};

template<template<> class Container1, template<> class Container2, class A, class B>
class CATTypeConv
{
public:
  virtual Container2<B> Conv2(Container1<A> cont);
  virtual Container1<A> Conv1(Container2<B> cont);
};

template<template<> class Container1, template<> class Container2, template<> class Container12, class A, class B, class C>
class CATCombinePair
{
public:
  virtual std::pair<Container1<A>, Container2<B> > Separate(Container12<C> c) const=0;
  virtual Container12<C> Combine(std::pair<Container1<A>, Container2<B> > p) const=0;
};

template<template<> class Container, class A>
class CATSingleElemPair
{
public:
  virtual Container<A> SingleElem(A a) const=0;
};

template<template<> class Container, class A, class I>
class CATChooseElem
{
public:
  virtual A ChooseElem(Container<A> cont, I elem) const=0;
};

template<template<> class Container, class T>
class CATCount
{
public:
  virtual void SetContainer(Container<T> &cont)=0;
  virtual int NumItems() const=0;
};


template<template<> class Container, class A>
class CATIdentity
{
public:
  virtual Container<A> Identity(Container<A> cont) const=0;
};

class TriangleComposition
{
public:
  CatArrow<C1, A,B> *Composition(CATArrow<C1, A,C> *c1, CATArrow<C1,C,B> *c2) const=0;
};

class SquareComposition
{
public:
  //std::pair<CATArrow<C1,A,B1>*, CATArrow<C1,B1,C>*> Composition(CATArrow<C1,A,K1> *c1, CATArrow<C1,K1,C> *c2);
  //std::pair<CATArrow<C1,A,K1>*, CATArrow<C1,K1,C>*> Composition2(CATArrow<C1,A,B1> *c1, CATArrow<C1,B1,C> *c2);
  CATArrow<C1,A,C> *Intersection(CATARrow<C1,A,B> *c1, CATArrow<C1,B,C> *c2,
				 CATArrow<C1,A,B2> *d1, CATArrow<C1,B2,C> *d2)
  {
    // implement as A->(B INTERSECT B2) -> C
  }
};

class CATIntersection
{
public:
  virtual CatArrow<C1,A,B> *Intersect(CATArrow<C1,A,B> *a1, CATArrow<C1,A,B> *a2) const=0;
};

class CATIntersectionArrow : public CATArrow<C1,A,B>
{
public: // 
  CATIntersectionArrow(CATArrow<C1,A,B> *a1, CATArrow<C1,A,B> *a2) : a1(a1), a2(a2) : a1(a1), a2(a2) 
  { 
  }
private:
  virtual B Func(A a) const
  {
    
  }
  virtual C1<std::vector<A> > BackwardFunc(C1<B> cont) const
  {
  }
private:
  CATArrow<C1,A,B> *a1;
  CATArrow<C1,A,B> *a2;

};

//
// IMPLEMENTATIONS
//
template<template<> class Container, class A, class B, class C>
class CATIntersection : public CATCombinePair<Container, Container, Container, A,B,C>, private CATArrow<Container, C,bool>
{
public:
  CATIntersection(bool (*fptrA)(A), bool (*fptrB)(B), 
		  CATArrow<Container, A, C> &fA, 
		  CATArrow<Container, B,C> &fB, 
		  CATSingleElemPair<Container, C> &singleelem, 
		  CATChooseElem<Container,C,int> &choose, 
		  CATCount<Container, bool> &count, 
		  CATChooseElem<Container,A,int> &chooseA, 
		  CATChooseElem<Container,B,int> &chooseB, 
		  CATCount<Container,A> &countA, 
		  CATCount<Container,B> &countB) 
  : 
    fptrA(fptrA), fptrB(fptrB), fA(fA), fB(fB), singleelem(singleelem), choose(choose), 
    count(count), chooseA(chooseA), chooseB(chooseB), countA(countA), countB(countB) 
  { } 
  virtual std::pair<Container<A>, Container<B> > Separate(Container<C> c) const=0;
  virtual Container<C> Combine(std::pair<Container<A>, Container<B> > p) const
  {
    Container<A> cA = p.first;
    countA.SetContainer(cA);
    int na = countA.NumItems();
    std::vector<C> vec;
    for(int i1=0;i1<na;i1++)
      {
	A a1 = chooseA.ChooseElem(cA, i1);
	C c = fA.Func(a1);
	if (Func(c))
	  vec.push_back(c);
      }


    Container<B> cB = p.second;
    countB.SetContainer(cB);
    int nb = countB.NumItems();
    for(int i2=0;i2<nb;i2++)
      {
	A a2 = chooseA.ChooseElem(cA, i2);
	C c = fB.Func(a2);
	if (Func(c))
	  vec.push_back(c);
      }

  }
  virtual bool Func(C c) const
  {
    Container<C> cc = singleelem.SingleElem(c);
    Container<A> a = fA.BackwardFunc(cc);
    Container<B> b = fB.BackwardFunc(cc);
    countA.SetContainer(a);
    int na = countA.NumItems();
    bool ba = false;
    for(int i1=0;i1<na;i1++)
      {
	A a1 = chooseA.ChooseElem(a,i1);
	bool aa = fptrA(a1);
	ba = ba || aa;
      }
    countB.SetContainer(b);
    int nb = countB.NumItems();
    bool bb2 = false;
    for(int i2=0;i2<nb;i2++)
      {
	B b1 = chooseB.ChooseElem(b,i2);
	bool bb = fptrB(b1);
	bb2 = bb2 || bb;
      }
    return ba && bb2;
  }
  virtual Container<std::vector<C> > BackwardFunc(Container<bool> cont) const
  {
    count.SetContainer(cont);
    int n = count.NumItems();
    for(int i=0;i<n;i++)
      {
	bool b = choose.ChooseElem(cont,i);
      }
  }

private:
  bool (*fptrA)(A); 
  bool (*fptrB)(B);
  //C (*fA)(A);
  //C (*fB)(B);
  CATArrow<Container, A,C> &fA;
  CATArrow<Container, B,C> &fB;
  CATSingleElemPair<Container,C> &singleelem;
  CATChooseElem<Container,C,int> &choose;
  CATCount<Container> &count;
  CATChooseElem<COntainer,A,int> &chooseA;
  CATChooseElem<Container,B,int> &chooseB;
  CATCount<Container> &countA;
  CATCount<Container> &countB;
};


template<class Container, class T>
class CATVector : public CATChooseElem<Container, T, int>, public CATCount<Container, T>
{
public:
  CATVector(std::vector<T> &vec) : vec(vec) { }
  virtual T ChooseElem(Container<T> cont, int elem) const
  {
    //m_cont = cont;
    //Refresh();
    return vec[elem];
  }
  virtual void SetContainer(Container<T> &cont) { /*m_cont = cont;*/ }
  virtual int NumItems() const { return vec.count(); }
  
private:
  //Container<T> m_cont;
  std::vector<T> &vec;
};

template<class Container, class T>
class CATVector2 : public Container<T>
{
public:
  CATVector2(std::vector<T> &vec) : vec(vec) { }
  
private:
  std::vector<T> &vec;
};


//
// Proper intersection
//

template<class T, class A, class B, class K>
class Intersection3 : public Function<T,std::pair<K,K> >
{
public:
  // functions must throw exception if not possible to do it
  Intersection3(Function<T,A> &ta,
		Function<T,B> &tb,
		Function<A,K> &ak,
		Function<B,K> &bk) : ta(ta), tb(tb), ak(ak), bk(bk) { }
  // throws if outside of the intersection
  K Index(T t) const
  {
    K k1;
    K k2;
    k1 = ak.Index(ta.Index(t));
    k2 = bk.Index(tb.Index(t));
    return std::make_pair(k1,k2);
  }
  
private:
  Function<T,A> &ta;
  Function<T,B> &tb;
  Function<A,K> &ak;
  Function<B,K> &bk;
};

template<class P, class A, class B, class K>
class pullback3 : public Intersection3<std::pair<P,K>, std::pair<P,K>, std::pair<P,K>, std::pair<P,K> >
{
};

class SphereIntersectionTexture : public Bitmap<bool>
{
public:
  SphereIntersectionTexture(Point center1, float radius1,
			    Point center2, float radius2,
			    int sx, int sy) : center1(center1), radius1(radius1),
					      center2(center2), radius2(radius2)
					      sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual bool Map(int x, int y) const
  {
    float xx = float(x)/float(sx);
    float yy = float(y)/float(sy);
    SphericalPoint point(center1);
    point.r = radius1;
    point.alfa = xx*2.0*3.14159;
    point.beta = yy*2.0*3.14159;
    Point pos = point.ToPoint();
    pos -= center2;
    float d = Vector(pos).Dist();
    return d < radius2;
  }

};

template<class A>
class Occupied : public Function<A, bool>
{
public:
  virtual B f(A a) const=0;
  virtual B g(A a) const=0;
  bool Index(A x) const { return f(x)==g(x); }
};


template<class T, class C>
class PullbackMinus : public Function<T,C>
{
public:
  PullbackMinus(Function<T,C> &c1, Function<T,C> &c2) : c1(c1),c2(c2)
  {
  }
  C Index(T t) const { return c1.Index(t)-c2.Index(t); }
private:
  Function<T,C> &c1;
  Function<T,C> &c2;
};


/* THIS IS WORKING PULLBACK with T=float, C=float! */
/* todo: CHECK If negative values work in SolveWithFailure() */
template<class T, class A, class B, class C>
class Pullback
{
public:
  Pullback(Function<A,C> &f1, Function<B,C> &f2,
	   Function<T,A> &p1, Function<T,B> &p2) : f1(f1), f2(f2),p1(p1),p2(p2), compose1(p1,f1), compose2(p2,f2), minus(compose1, compose2) { }
  T Solve(T t_0, T t_1, bool &success) const
  {
    success = false;
    T val = SolveWithFailure(minus, t_0, t_1, success);
    if (success) return val;
    return T();
  }

private:
  Function<A,C> &f1;
  Function<B,C> &f2;
  Function<T,A> &p1;
  Function<T,B> &p2;
  FunctionCompose<T,A,C> compose1;
  FunctionCompose<T,B,C> compose2;
  PullbackMinus<T,C> minus;
};

class DistanceFunction : public Function<Point,float>
{
public:
  DistanceFunction(Point center) : center(center) { }
  float Index(Point p) const { p-=center; return sqrt(p.x*p.x+p.y*p.y+p.z*p.z); }
private:
  Point center;
};
class ConstantFunction : public Function<void, float>
{
public:
  ConstantFunction(float val) : val(val) { }
  float Index(void) const { return val; }
};
class PointIdentity : public Function<float,Point>
{
public:
  PointIdentity(Point p1, Vector v) : p1(p1), v(v) { }
  Point Index(float p) const { return p1+v*p; }
private:
  Point p1;
  Vector v;
};
class PointExclam : public Function<Point, void>
{
public:
  void Index(Point p) const { }
};
class SpherePullback : public Pullback<float, Point, void, float>
{
public:
  SpherePullback(Point ray_p1, Vector ray_v, Point center, float val) : Pullback<float,Point,void,float>(dist, cx, id, exc), dist(center), id(ray_p1,ray_v), cx(val) { }
  float Solve(float p1, float p2, bool &success) const
  {
    return Pullback<float,Point,void,float>::Solve(p1,p2,success);
  }
private:
  DistanceFunction dist;
  ConstantFunction cx;
  PointIdentity id;
  PointExclam exc;
};

class BlobPullback : public Pullback<float, Point, Point, float>
{
public:
  
private:
  DistanceFunction dist1, dist2;
  PointIdentity ray1, ray2;
};


//
// Volumes and surfaces
//
template<class T>
class VolumeToSurface : public Function<Point2d, float>
{
public:
  VolumeToSurface(Function<Point2d, Point> &surface,
		  Function<Point, T> &object) : surface(surface), object(object)
  {
  }
  float Index(Point2d p) const
  {
    return object.Index(surface.Index(p));
  }
private:
  Function<Point2d, Point> &surface;
  Function<Point,T> &object;
};

class XYPlane : public Function<Point2d, Point>
{
public:
  XYPlane(float z) : z(z) { }
  Point Index(Point2d p) const { return Point(p.x,p.y,z); }
private:
  float z;
};

//
// Complex distance
//

struct RadiusPoints
{
  Point p1,p2;
  float r;
};

struct RadiusPointCalc
{
  Vector pos(Point p) const
  {
    Vector v1 = p-p1;
    Vector v2 = p-p2;
    Plane pl(p, v1,v2);
    Point2d p2d = { pl.CoordsX(p), pl.CoordsY(p) };
    Point2d p2d2 = { pl.CoordsX(p1), pl.CoordsY(p1) };
    Point2d p2d3 = { pl.CoordsX(p2), pl.CoordsY(p2) };
    
    Vector2d v2 = p2d2-p2d;
    Vector2d v3 = p2d3-p2d;
    Vector2d v = v2+v3;
    
    LineLineIntersection intersect(p2d2, p2d3, p2d, p2d+v);
    Point2d intersectpoint = intersect.IntersectionPoint();
    Point px = pl.Navigate(intersectpoint);
    
    
  }
};


class IntersectableCurve2d
{
public:
  virtual Point2d Curve(float val) const=0;
  virtual float Dist(Point2d p) const=0;
};
void Intersection(IntersectableCurve &c1, IntersectableCurve &c2)
{
  // STEP1 compose c1.Curve and c2.Dist.
  // STEP2 use float->float in root finding.
}

class ConstantMinus : public Function<float,float>
{
public:
  ConstantMinus(Function<float,float> &f, float c) : f(f), c(c) { }
  float Index(float x) const
  {
    return f.Index(x)-c;
  }
private:
  Function<float,float> &f;
  float c;
};


class Pullback_c 
{
public:
  Pullback_c(Function<float,float> &f1, Function<float,float> &f2, float b) : f(f), g(g), b(b), cons1(f,b), cons2(g,b) { }
  std::pair<float,float> solve(float s1, float s2) const
  {
    float val = Solve(cons1, s1, s2);
    float val2 = Solve(cons2, s1, s2);
    return std::make_pair(val, val2);
  }
private:
  Function<float,float> &f;
  Function<float,float> &g;
  float b;
  ConstantMinus cons1;
  ConstantMinus cons2;
};

template<class A, class B>
class Pullback_int_d : public Function<std::pair<A,B>, bool>
{
public:
  Pullback_d(Function<A,int> &f, Function<B,int> &g) : f(f), g(g) { }
  bool Index(std::pair<A,B> &p) const
  {
    int c = f.Index(p.first);
    int c2 = g.Index(p.second);
    return c==c2;
  }
private:
  Function<A,C> &f;
  Function<B,C> &g;
};

template<class A, class B, class C>
class Pullback_d : public Function<std::pair<A,B>, bool>
{
public:
  Pullback_d(Function<A,C> &f, Function<B,C> &g) : f(f), g(g) { }
  bool Index(std::pair<A,B> &p) const
  {
    C c = f.Index(p.first);
    C c2 = g.Index(p.second);
    return c<c2;
  }
private:
  Function<A,C> &f;
  Function<B,C> &g;
};

template<class T, class A, class B, class C>
class Pullback_full : public Function<T,std::pair<A,B> >, public Function<std::pair<A,B>, bool>
{
public:
  Pullback_full(Function<T,A> &ta,
		Function<T,B> &tb,
		Function<A,C> &ac,
		Function<B,C> &bc)
    : ta(ta),
      tb(tb),
      ac(ac),
      bc(bc) { }

  std::pair<A,B> Index(T t) const
  {
    A a = ta.Index(t);
    B b = tb.Index(t);
    return std::make_pair(a,b);
  }
  bool Index(std::pair<A,B> p) const
  {
    return ac.Index(p.first)==bc.Index(p.second);
  }
private:
  Function<T,A> &ta;
  Function<T,B> &tb;
  Function<A,C> &ac;
  Function<B,C> &bc;
};


template<class X, class Y>
class Exists : public Function<Function<std::pair<X,Y>,bool>*,Function<X,bool>* >, private Function<X,bool>
{
public:
  Exists(std::vector<Y> &vec) : vec(vec) { }
  Function<X,bool> *Index(Function<std::pair<X,Y>, bool> &f)
  {
    ff = &f;
    return const_cast<Exists*>(this);
  }
  bool Index(X x) const
  {
    int s = vec.size();
    bool bb = false;
    for(int i=0;i<s;i++)
      {
	bool b = ff->Index(std::make_pair(x, vec[i]));
	bb |= b;
      }
    return bb;
  }
private:
  Function<std::pair<X,Y>, bool> *ff;
  std::vector<Y> &vec;
}; 

template<class I, class A, class B>
class Arrow
{
public:
  virtual std::pair<A,B> FindMap(I i) const=0;
};

template<class I, class I1, class I2, class T, class A, class B, class V>
class InverseImageOnArrows : public Arrow<I,T,A>
{
public:
  InverseImage(Arrow<I1,V,B> &part, Arrow<I2,A,B> &f) : part(part), f(f) { }
  virtual std::pair<T,A> FindMap(I i) const
  {
    I1 i1 = i.first;
    I2 i2 = i.second;
    std::pair<V,B> p1 = part.FindMap(i1);
    std::pair<A,B> p2 = f.FindMap(i2);
    if (p1.second == p2.second)
      {
	A a = p2.first;
	T t = a;
	return std::make_pair(t,a);
      }
    else
      {
	return std::make_pair(T(),A());
      }
  }

private:
  Arrow<I1,V,B> &part;
  Arrow<I2,A,B> &f;
};


#endif

