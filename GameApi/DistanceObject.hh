
#include "VectorTools.hh"

class DistanceObject
{
public:
  virtual int NumDistances() const=0;
  virtual float Distance(int idx, Point ps) const=0;
};

class MinDistance : public DistanceObject
{
public:
  MinDistance(DistanceObject &array) : array(array) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    int mn = 9999999;
    int s = array.NumDistances();
    for(int i=0;i<s;i++)
      {
	float d = array.Distance(i, ps);
	if (d<mn) mn=d;

      }
    return mn;
  }
private:
  DistanceObject &array;
};

class SphereDistance : public DistanceObject
{
public:
  SphereDistance(Point center) : center(center) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    ps-=center;
    return sqrt(ps.x*ps.x+ps.y*ps.y+ps.z*ps.z);
  }
private:
  Point center;
};

class PlaneDistance : public DistanceObject
{
public:
  PlaneDistance(Point p, Vector dir) : p(p), dir(dir) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    std::pair<Vector,Vector> split = Vector::SplitToComponents(ps-p, dir);
    return split.first.Dist();
  }
private:
  Point p;
  Vector dir;
};
