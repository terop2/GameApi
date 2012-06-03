
#include "Effect.hh"

class IntersectionCurve : public Curve<Point>
{
public:
  IntersectionCurve(SphereSurfaceIn3d &sphere1, SphereSurfaceIn3d &sphere2) : sphere1(sphere1), sphere2(sphere2) { }

private:
  SphereSurfaceIn3d &sphere1;
  SphereSurfaceIn3d &sphere2;
};


//
// Proper intersections
//

class IntersectionPrimitive2d : public Function<float, Point2d>, public Function<Point2d, float>
{
public:
  Point2d Index(float val) const { return Curve(val); }
  float Index(Point2d p) const { return Distance(p); }

  virtual Point2d Curve(float val) const=0;
  virtual float Distance(Point2d p) const=0;
  virtual float StartRange() const =0;
  virtual float EndRange() const=0;
};

float Intersection1(IntersectionPrimitive2d &p1, IntersectionPrimitive2d &p2)
{
  SolvableCompose<Point2d> compose(p1,p2);
  float intersectionpoint = Solve(compose, p1.StartRange(), p1.EndRange());
  return intersectionpoint;
}
float Intersection2(IntersectionPrimitive2d &p1, IntersectionPrimitive2d &p2)
{
  SolvableCompose<Point2d> compose(p2,p1);
  float intersectionpoint = Solve(compose, p2.StartRange(), p2.EndRange());
  return intersectionpoint;
}

class IntersectionPrimitive3d : public Function<float, Point2d>, public Function<Point2d,Point>, public Function<Point, float>
{
public:
  void set_curve(CurveIn3d &curve) { m_curve = &curve; }
  Point2d Index(float val) const { return Retraction(*m_curve, val); }
  Point Index(Point2d val) const { return Surface(val); }
  float Index(Point p) const { return DistanceFromSurface(p); }
  virtual Point2d Retraction(const CurveIn3d &curve, float pos) const=0;
  virtual Point Surface(Point2d p) const=0;
  // distance is negative if inside, positive if outside
  virtual float DistanceFromSurface(Point p) const=0; 
  virtual float StartRange() const =0;
  virtual float EndRange() const=0;
private:
  CurveIn3d *m_curve;
};

class SpherePrimitive : public IntersectionPrimitive3d
{
public:
  SpherePrimitive(Point center, float radius) : center(center), radius(radius) { }
  virtual Point2d Retraction(const CurveIn3d &curve, float pos) const
  {
    
  }
  virtual Point Surface(Point2d p) const=0;
  virtual float DistanceFromSurface(Point p) const=0;
  virtual float StartRange() const =0;
  virtual float EndRange() const=0;
};

float Intersection1(IntersectionPrimitive3d &p1, IntersectionPrimitive3d &p2)
{
  FunctionCompose<float, Point2d, Point> compose1(p1,p1);
  FunctionCompose<float, Point, float> compose2(compose1, p2);
  float intersectionpoint = Solve(compose2, p1.StartRange(), p1.EndRange());
  return intersectionpoint;
}
float Intersection2(IntersectionPrimitive3d &p1, IntersectionPrimitive3d &p2)
{
  FunctionCompose<float, Point2d, Point> compose1(p2,p2);
  FunctionCompose<float, Point, float> compose2(compose1, p1);
  float intersectionpoint = Solve(compose2, p2.StartRange(), p2.EndRange());
  return intersectionpoint;
}


template<class T>
class ColorSpace
{
public:
  virtual Color MapColor(T t) const=0;
};
typedef ColorSpace<Point2d> ColorSpace2d;
typedef ColorSpace<Point> ColorSpace3d;
