
#include "VectorTools.hh"

class RayTrace
{
public:
  virtual bool RayIntersect(Point p1, Vector dir) const=0;
  virtual Point RayIntersectPoint(Point p1, Vector dir) const=0;
};

class PointProperty
{
public:
  virtual Vector PointNormal(Point p) const=0;
  virtual unsigned int PointColor(Point p) const=0;
  virtual Point2d PointTexCoord(Point p) const=0;
  virtual float RaySplitPercentage(Point p) const=0;
};

class RaySpace
{
public:
  virtual Point2d Ray(Point p1, Vector dir) const=0;

};

class ObjectSpace
{
public:
  virtual Point2d Obj(Point p) const=0;
};

class SphereObjectSpace : public ObjectSpace
{
public:
  SphereObjectSpace(Point center) : center(center) { }
  virtual Point2d Obj(Point p) const
  {
    p-=center;
    float r = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    float alfa = acos(p.z/r);
    float beta = atan2(p.y,p.x);
    Point2d p = { alfa, beta };
    return p;
  }
  Point center;
};



class RayTraceSphereImpl : public RayTrace
{
public:
  RayTraceSphereImpl(Point center, float radius) : center(center), radius(radius) { }
  bool RayIntersect(Point p1, Vector dir) const
  {
    float a = Vector::DotProduct(dir, dir);
    float b = 2.0*Vector::DotProduct(dir, p1);
    float c = Vector::DotProduct(p1,p1) - (radius*radius);
    float disc = b*b - 4*a*c;
    if (disc < 0) { return false; }
    float distSqrt = sqrtf(disc);
    float q;
    if (b<0) { q = (-b - distSqrt)/2.0; }
    else { q = (-b + distSqrt)/2.0; }
    
    float t0 = q/a;
    float t1 = c/q;
    if (t0>t1) 
      {
	float tmp = t0;
	t0 = t1;
	t1=tmp;
      }
    if (t1<0.0) { return false; }
    if (t0<0.0) { t = t1; return true; }
    t = t0;
    return true;
  }
  virtual Point RayIntersectPoint(Point p1, Vector dir) const { 
    RayIntesect(p1,dir);
    return t; 
  }

private:
  Point t;
  Point center;
  float radius;
};

class SphereRaySpace : public RaySpace
{
public:
  SphereRaySpace(Point center, float radius) : center(center), radius(radius) { }
  virtual Point2d Ray(Point p1, Vector dir) const
  {
    Point p = impl.RayIntersectPoint(p1, dir);
    Point2d p2 = objspace.Obj(p);
    return p2;
  }

private:
  RayTraceSphereImpl impl;
  SphereObjectSpace objspace;

};
