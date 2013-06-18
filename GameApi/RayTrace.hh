
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
};
