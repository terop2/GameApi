
#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "Effect.hh"
#include "Equalizer.hh"

struct PForce
{
public:
  virtual Vector Force(Point p) const=0;
};

class Gravity : public PForce
{
public:
  Gravity(float g) : g(g) { }
  Vector Force(Point p) const { return Vector(0.0,-g,0.0); }
private:
  float g;
};

struct PPoint
{
public:
  Point pos;
  Vector speed;
  float time;
  float mass;
  std::vector<PForce*> vec;
};

class XForces : public Function<float,float>
{
public:
  XForces(PPoint &pp) : pp(pp) { }
  float Index(float time) const
  {
    int s = pp.vec.size();
    float val = 0.0;
    for(int i=0;i<s;i++)
      {
	PForce *f = pp.vec[i];
	Vector v = f->Force(pp.pos);
	val += v.dx;
      }
    val /= pp.mass;
    return val;
  }
private:
  PPoint &pp;
};

class YForces : public Function<float,float>
{
public:
  YForces(PPoint &pp) : pp(pp) { }
  float Index(float time) const
  {
    int s = pp.vec.size();
    float val = 0.0;
    for(int i=0;i<s;i++)
      {
	PForce *f = pp.vec[i];
	Vector v = f->Force(pp.pos);
	val += v.dy;
      }
    val /= pp.mass;
    return val;
  }
private:
  PPoint &pp;
};

class ZForces : public Function<float,float>
{
public:
  ZForces(PPoint &pp) : pp(pp) { }
  float Index(float time) const
  {
    int s = pp.vec.size();
    float val = 0.0;
    for(int i=0;i<s;i++)
      {
	PForce *f = pp.vec[i];
	Vector v = f->Force(pp.pos);
	val += v.dz;
      }
    val /= pp.mass;
    return val;
  }
private:
  PPoint &pp;
};

class CombinePositions : public Function<float, Point>
{
public:
  CombinePositions(Function<float,float> &x,
		   Function<float,float> &y,
		   Function<float,float> &z) : x(x), y(y), z(z) { }
  Point Index(float time) const
  {
    float xx = x.Index(time);
    float yy = y.Index(time);
    float zz = z.Index(time);
    return Point(xx,yy,zz);
  }
private:
  Function<float,float> &x;
  Function<float,float> &y;
  Function<float,float> &z;
};


// This is function float->Point
class PointMovement : public CurveIn3d
{
public:
  PointMovement(PPoint &p, float time_step, float delta) 
    : p(p), xf(p), yf(p), zf(p), time_step(time_step), delta(delta)
  { }
  Point Index(float time) const
  {
    IntegrationFunction ifx1(xf, delta, p.time, time, p.speed.dx);
    IntegrationFunction ify1(yf, delta, p.time, time, p.speed.dy);
    IntegrationFunction ifz1(zf, delta, p.time, time, p.speed.dz);
    
    IntegrationFunction ifx2(ifx1, delta, p.time, time, p.pos.x);
    IntegrationFunction ify2(ify1, delta, p.time, time, p.pos.y);
    IntegrationFunction ifz2(ifz1, delta, p.time, time, p.pos.z);
    CombinePositions comb(ifx2, ify2, ifz2);
    return comb.Index(time);
  }
  Vector Speed(float time) const
  {
    IntegrationFunction ifx1(xf, delta, p.time, time, p.speed.dx);
    IntegrationFunction ify1(yf, delta, p.time, time, p.speed.dy);
    IntegrationFunction ifz1(zf, delta, p.time, time, p.speed.dz);
    CombinePositions comb(ifx1, ify1, ifz1);
    return Vector(comb.Index(time));
  }
  float Length() const { return 100000.0; }
private:
  PPoint &p;
  XForces xf;
  YForces yf;
  ZForces zf;

  float time_step;
  float delta;
};

Vector SplitToComponent(Vector vec, Vector base);

class CollisionDistanceToPlane : public Function<Point, float>
{
public:
  CollisionDistanceToPlane(Point u_p, Vector u_x, Vector u_y)
    : pl(u_p, u_x, u_y) { }
  float Index(Point p) const
  {
    return pl.Dist(p);
  }
private:
  Plane pl;
};

class Any : public Function<float, int>
{
public:
  int Index(float x) const { return 0; }
};

template<class T>
class Zero : public Function<T, float>
{
public:
  float Index(T t) const { return 0.0; }
};


class PairFunc : public Function<float, std::pair<Point,int> >
{
public:
  PairFunc(Function<float, Point> &curve) : curve(curve) { }
  std::pair<Point, int> Index(float time) const
  {
    return std::make_pair(curve.Index(time), 0);
  }
private:
  Function<float, Point> &curve;
};

template<class A, class B>
class ProjA : public Function<std::pair<A,B>, A>
{
public:
  A Index(std::pair<A,B> p) const { return p.first; }
};

template<class A, class B>
class ProjB : public Function<std::pair<A,B>, B>
{
public:
  B Index(std::pair<A,B> p) const { return p.second; }
};

template<class T, class A, class B>
class PairFunc2 : public Function<T, std::pair<A,B> >
{
public:
  PairFunc2(Function<T,A> &f, Function<T,B> &f2) : f(f), f2(f2) { }
  std::pair<A,B> Index(T p) const { return std::make_pair(f.Index(p), f2.Index(p)); }
private:
  Function<T,A> &f;
  Function<T,B> &f2;
};


#if 0
class CalculateCollisionPoint
{
public:
  CalculateCollisionPoint(Function<float, Point> &curve,
			  Function<Point, float> &space, float start_time, float end_time) 
    : pairfunc(curve), pb(space, zero, fst, snd, pairfunc, start_time, end_time)
  { }
  float get_time() const { return pb.Elem().value; }
  Point get_point() const { return pb.get_A(); }
private:
  PairFunc pairfunc;
  FloatPullback<std::pair<Point,int>, Point, int> pb;
  Any any;
  Zero<int> zero;
  Fst<Point,int> fst;
  Snd<Point,int> snd;
};
#endif

template<class P1, class P2>
class PosFunc : public Function<std::pair<float,float>, std::pair<P1,P2> >
{
public:
  PosFunc(Function<float, P1> &f1, Function<float,P2> &f2) : f1(f1),f2(f2) { }
  std::pair<P1,P2> Index(std::pair<float,float> p) const
  {
    return std::make_pair(f1.Index(p.first), f2.Index(p.second));
  }
private:
  Function<float,P1> &f1;
  Function<float,P2> &f2;
};


class SurfacePlaneIntersection
{
public:
  SurfacePlaneIntersection(Function<Point2d, float> &surface, float y) : surface(surface), y(y) { }
  bool Inside(Point2d p) const { return fabs(surface.Index(p)-y) < 0.001; }
  Point2d Step(Point2d p) const
  {
    Gradient g(surface);
    Vector2d v = g.Index(p);
    v/=20.0;
    p+=v;
    return p;
  }
  bool InsideBox(Point2d p, Point2d tl, Point2d br)
  {
    if (p.x<tl.x) return false;
    if (p.x>br.x) return false;
    if (p.y<tl.y) return false;
    if (p.y>br.y) return false;
    return true;
  }
  Point2d Loop(Point2d start_pos, Point2d tl, Point2d br, bool &succes)
  {
    Point2d p = start_pos;
    while(InsideBox(p, tl, br) && !Inside(p))
      {
	p = Step(p);
      }
    succes = InsideBox(p, tl, br);
    return p;
  }
private:
  Function<Point2d, float> &surface;
  float y;
};
Point2d SolveWithFailure(Function<Point2d, float> &func, Point2d tl, Point2d br, bool &b);
class WorldWorldIntersection
{
public:
  // RxRxR|->t
  WorldWorldIntersection(Function<Point,float> &f, float t) : f(f), t(t) { } 
  bool Inside(Point p) const 
  { 
    return fabs(f.Index(p)-t) <0.001; 
  }
  Point Step(Point p) const
  {
#if 0
    Gradient3d g(f);
    Vector v = g.Index(p);
    v/=20.0;
    p+=v;
    return p;
#endif
    float val = f.Index(p);
    Gradient3d g(f);
    Vector vv = g.Index(p);
    Vector v = val*(vv/vv.Dist());
    v/=20.0;
    //v*=-1;
    p+=v;
    //std::cout << "Point:" << p << std::endl;
    return p;
  }
  bool InsideBox(Point p, Point tl, Point br)
  {
    if (p.x<tl.x) return false;
    if (p.x>br.x) return false;
    if (p.y<tl.y) return false;
    if (p.y>br.y) return false;
    if (p.z<tl.z) return false;
    if (p.z>br.z) return false;
    return true;
  }
  Point Loop(Point start_pos, Point tl, Point br, bool &b)
  {
    Point p = start_pos;
    //int i=0;
    while(InsideBox(p, tl, br) && !Inside(p))
      {
	p = Step(p);
	//std::cout << i << " " << p << std::endl;
	//i++;
      }
    b = Inside(p); //InsideBox(p, tl, br);
    return p;
  }

private:
  Function<Point,float> &f;
  float t;
};

Point SolveWithFailure(const Function<Point, float> &func, Point tl, Point br, bool &error);



class FloatPairToStdPair : public Function<FloatPair, std::pair<float,float> >
{
public:
  std::pair<float,float> Index(FloatPair p) const
  {
    return std::make_pair(p.x,p.y);
  }
};
class FloatPointToStdPair : public Function<FloatPoint, std::pair<std::pair<float,float>, float> >
{
public:
  std::pair<std::pair<float,float>, float> Index(FloatPoint p) const
  {
    return std::make_pair(std::make_pair(p.p.x,p.p.y),p.p.z);
  }
};

#if 0
class LineLineIntersection
{
public:
  LineLineIntersection(Point2d p_1, Vector2d v_1, Point2d p_2, Vector2d v_2)
    : line1(p_1, p_1+v_1), line2(p_2,p_2+v_2), tl(-1000.0,-1000.0), br(1000.0,1000.0), pb(line1, line2, fst, snd, pair) { }
  Point2d Elem() const
  {
    return pb.get_A();
  }
private:
  LineIn2d line1, line2;
  Distance2dPullback<std::pair<float,float>, Point2d, Point2d> pb;
  FloatPairToStdPair pair;
  FloatPair tl, br;
};
#endif
class SurfaceFunction : public Function<std::pair<float,float>, Point>
{
public:
  SurfaceFunction(SurfaceIn3d &surf) : surf(surf) { }
  Point Index(std::pair<float,float> p) const
  {
    return surf.Index(p.first,p.second);
  }
private:
  SurfaceIn3d &surf;
};

// A pullback of (RxR)x(R).
class RenderingEquation
{
public:
  RenderingEquation(Point ray_p, Vector ray_dir, SurfaceIn3d &surf, Point tl_, Point br_)
    : r(ray_p, ray_dir),
      surf_func(surf), 
      tl(tl_), br(br_),
      pb(surf_func, r, proj_a, proj_b, comb, tl, br) 
  {}
  Point InternalPos() const { return pb.Elem().get(); }
  Point2d SurfacePos() const {
    std::pair<float,float> p = pb.get_A();
    Point2d pp;
    pp.x = p.first;
    pp.y = p.second;
    return pp;
  }
  float RayPos() const {
    float p = pb.get_B();
    return p;
  }
  bool Success() const { return pb.Success(); }
private:
  ProjA<std::pair<float,float>, float> proj_a;
  ProjB<std::pair<float,float>, float> proj_b;
  FloatPointToStdPair comb;		    
  Ray r; // float -> Point
  SurfaceFunction surf_func; // float x float -> Point
  FloatPoint tl,br;
  Distance3dPullback<std::pair<std::pair<float,float>, float>, std::pair<float,float>, float> pb; // (RxR)x(R) pullback
};

class RenderingEquationBitmap : public Bitmap<Point2d>
{
public:
  RenderingEquationBitmap(Point ray_p, Vector ray_x, Vector ray_y, Vector ray_z, SurfaceIn3d &surf, int sx, int sy) : ray_p(ray_p), ray_x(ray_x), ray_y(ray_y), ray_z(ray_z), surf(surf), tl(ray_p), br(ray_p+ray_x+ray_y+ray_z), sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Point2d Map(int x, int y) const
  {
    float xx = x / float(sx);
    float yy = y / float(sy);
    Vector u_x = ray_x * xx;
    Vector u_y = ray_y * yy;
    Vector u_z = ray_z;
    Point p = ray_p + u_x + u_y;
    RenderingEquation eq(p, u_z, surf, tl, br);
    if (eq.Success())
      {
	Point2d pp = eq.SurfacePos();
	//std::cout << "SurfacePos:" << pp.x << " " << pp.y << std::endl;
#if 1
	float ssx = surf.XSize();
	float ssy = surf.YSize();
	while (pp.x < 0.0)
	  {
	    pp.x += ssx;
	  }
	while (pp.x >= ssx)
	  {
	    pp.x -= ssx;
	  }
	while (pp.y < 0.0)
	  {
	    pp.y += ssy;
	  }
	while (pp.y >= ssy)
	  {
	    pp.y -= ssy;
	  }
#endif
	return pp;

      }
    Point2d p2 = { -1.0, -1.0 };
    return p2;
  }

private:
  Point ray_p;
  Vector ray_x, ray_y, ray_z;
  SurfaceIn3d &surf;
  Point tl;
  Point br;
  int sx,sy;
};
class TextureBitmap : public Bitmap<Color>
{
public:
  TextureBitmap(Bitmap<Point2d> &coords, SurfaceIn3d &surf) : coords(coords), surface(surf) { }
  virtual int SizeX() const { return coords.SizeX(); }
  virtual int SizeY() const { return coords.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Point2d p = coords.Map(x,y);
    unsigned int c = surface.Color(p.x, p.y);
    return Color(c);
  }

private:
  Bitmap<Point2d> &coords;
  SurfaceIn3d &surface;
};


#endif
