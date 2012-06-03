
#include "VectorTools.hh"
#include "Plane.hh"

class CoordTransform
{
public:
  virtual Point Transform(Point p) const=0;
};

class LinPosTransform
{
public:
  virtual Matrix Transform(Point p) const=0;
};

class FloatMatrix
{
public:
  virtual Matrix Mat(float x) const=0;
};

class LinPosCoordTransform : public CoordTransform
{
public:
  LinPosCoordTransform(LinPosTransform &t) : t(t) { }
  Point Transform(Point p) const
  {
    Matrix m = t.Transform(p);
    return p*m;
  }
private:
  LinPosTransform &t;
};

class SinTransform : public CoordTransform
{
public:
  SinTransform(float dist_x, float speed_x,
	       float dist_y, float speed_y,
	       float dist_z, float speed_z) 
    : dist_x(dist_x),
      dist_y(dist_y),
      dist_z(dist_z),
     spx(speed_x),
     spy(speed_y),
     spz(speed_z) 
  { }
  Point Transform(Point p) const
  {
    Point pp;
    pp.x = p.x + dist_x * cos(p.x*spx);
    pp.y = p.y + dist_y * cos(p.y*spy);
    pp.z = p.z + dist_z * cos(p.z*spz);
    return pp;
  }
private:
  float dist_x, dist_y, dist_z;
  float spx, spy, spz;
};

class CoordTransform2d
{
public:
  virtual Point2d Transform(Point2d p) const=0;
};
class Translate2d : public CoordTransform2d
{
public:
  Translate2d(Point2d p) : p(p) { }
  Point2d Transform(Point2d pp) const
  {
    Point2d px;
    px.x = pp.x + p.x;
    px.y = pp.y + p.y;
    return px;
  }
  
private:
  Point2d p;
};


class FromPolarCoordinates : public CoordTransform2d
{
public:
  // p.x = radius
  // p.y = angle
  Point2d Transform(Point2d polar) const
  {
    Point2d pp;
    pp.x = polar.x * cos(polar.y);
    pp.y = polar.x * sin(polar.y);
    return pp;
  }
};
class ToPolarCoordinates : public CoordTransform2d
{
public:
  Point2d Transform(Point2d p) const
  {
    Point2d pp;
    pp.x = sqrt(p.x*p.x+p.y*p.y);
    if (p.x == 0 && p.y == 0)
      pp.y = 0;
    else if (p.x>=0)
      pp.y = asin(p.y/pp.x);
    else pp.y = -asin(p.y/pp.x)+3.14159;
    return pp;
  }
};

class ToSphericalCoordinates : public CoordTransform
{
public:
  Point Transform(Point p) const
  {
    SphericalPoint sp;
    sp.FromPoint(p);
    return Point(sp.r, sp.alfa, sp.beta);
    
  }
};

class FromSphericalCoordinates : public CoordTransform
{
public:
  Point Transform(Point p) const
  {
    SphericalPoint sp;
    sp.r = p.x;
    sp.alfa = p.y;
    sp.beta = p.z;
    return sp.ToPoint();
  }
};

class ToCylindarCoordinates : public CoordTransform
{
public:
  virtual Point Transform(Point p) const
  {
    Point2d pp;
    pp.x = p.x;
    pp.y = p.y;
    Point2d pp2 = c.Transform(pp);
    Point res;
    res.x = pp2.x;
    res.y = pp2.y;
    res.z = p.z;
    return res;
  }
private:
  ToPolarCoordinates c;
};


class FromCylindarCoordinates : public CoordTransform
{
public:
  virtual Point Transform(Point p) const
  {
    Point2d pp;
    pp.x = p.x;
    pp.y = p.y;
    Point2d pp2 = c.Transform(pp);
    Point res;
    res.x = pp2.x;
    res.y = pp2.y;
    res.z = p.z;
    return res;
  }
private:
  FromPolarCoordinates c;
};


class SurfaceTransform
{
public:
  virtual Point Transform(Point2d p) const=0;
};

class PlaneSurface : public SurfaceTransform
{
public:
  PlaneSurface(Plane pl) : pl(pl) { }
  Point Transform(Point2d p) const;
private:
  Plane pl;
};

class SphereSurface : public SurfaceTransform
{
public:
  SphereSurface(Point center, float radius) : center(center), radius(radius) { }
  Point Transform(Point2d p) const
  {
    Point pp;
    pp.x = radius*sin(p.x)*cos(p.y);
    pp.y = radius*sin(p.x)*sin(p.y);
    pp.z = radius*cos(p.x);
    pp += center;
    return pp;
  }
private:
  Point center;
  float radius;
};

class TorusSurface : public SurfaceTransform
{
public:
  TorusSurface(Point2d center, float dist1, float dist2, Plane pl) : cmc(dist1), circle(center, dist2), plane(pl), circle_plane(circle, plane), cmult(cmc, circle_plane) { }
  Point Transform(Point2d p) const
  {
    return cmult.Index(p.x, p.y);
  }

public:
  CircleMatrixCurve cmc;
  Circle circle;
  Plane plane;
  PlaneCurveIn3d circle_plane;
  CurveMultiply cmult;
};

class SurfaceValue
{
public:
  virtual float Value(Point2d p) const=0;
};

class HeightMapSurfaceTransform : public SurfaceTransform
{
public:
  HeightMapSurfaceTransform(SurfaceValue &val) : val(val) { }
  
  Point Transform(Point2d p) const
  {
    Point res;
    res.x = p.x;
    res.y = p.y;
    res.z = val.Value(p);
    return res;
  }
private:
  SurfaceValue &val;

};

class InterpolateSurfaceTransform : public SurfaceTransform
{
public:
  InterpolateSurfaceTransform(SurfaceTransform &s1, 
			      SurfaceTransform &s2,
			      float time) // 0.0 .. 1.0'
    : s1(s1),
      s2(s2),
      time(time) { }
  Point Transform(Point2d p) const
  {
    Point p1 = s1.Transform(p);
    Point p2 = s2.Transform(p);
    LineIn3d l(p1,p2);
    return l.Index(time);
  }
private:
  SurfaceTransform &s1;
  SurfaceTransform &s2;
  float time;
};

class GridTo3d : public GridIn3d
{
public:
  GridTo3d(GridIn2d &g, SurfaceTransform &transform) : g(g), transform(transform) { }
  Point Index(int x, int y) const { return transform.Transform(g.Index(x,y)); }
  virtual Point2d Texture(int x, int y) const { Point2d p; p.x = 0.0; p.y=0.0; return p; }
  virtual unsigned int Color(int x, int y) const
  {
    return Color::White().Pixel();
  }

  virtual Vector Normal(int x, int y) const
  {
    Point p = Index(x,y);
    Point px = Index(x+1,y);
    Point py = Index(x,y+1);
    Vector vx = px - p;
    Vector vy = py - p;
    return Vector::CrossProduct(vx, vy);
  }
  virtual float Attrib(int x, int y, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int x, int y, int id) const
  {
    return 0;
  }
  int XSize() const { return g.XSize(); }
  int YSize() const { return g.YSize(); }

private:
  GridIn2d &g;
  SurfaceTransform &transform;
};

class UnitPlane : public GridIn2d
{
public: // [0.0 .. 1.0]x[0.0 .. 1.0]
  UnitPlane(int x, int y) : x(x), y(y) { }
  virtual int XSize() const { return x; }
  virtual int YSize() const { return y; }
  virtual Point2d Index(int xx, int yy) const
  {
    Point2d p;
    p.x = float(xx)/float(x);
    p.y = float(yy)/float(y);
    return p;
  }
private:
  int x, y;
};

class TransformPlane : public GridIn2d
{
public:
  TransformPlane(GridIn2d &g, CoordTransform2d &coord) : g(g), coord(coord) { }
  virtual int XSize() const { return g.XSize(); }
  virtual int YSize() const { return g.YSize(); }
  virtual Point2d Index(int x, int y) const { return coord.Transform(g.Index(x,y)); }

private:
  GridIn2d &g;
  CoordTransform2d &coord;
};

class TransformEffect : public FrameAnim
{
public:
  TransformEffect(Render *r) : FrameAnim(r) { }
  bool Frame(float time);
};
