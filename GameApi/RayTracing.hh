
#include "VectorTools.hh"
#include "Effect.hh"

class RaySphereIntersection
{
public:
  struct IntersectPoint
  {
    Point p;
  };
  IntersectPoint Intersect(Point center, float radius,
			   Point ray_p1, Point ray_p2);
		  
};



RaySphereIntersection::IntersectPoint RaySphereIntersection::Intersect(Point center, float radius,
			 Point ray_p1, Point ray_p2)
{
  RaySphereIntersection::IntersectPoint p;
  return p;
#if 0
  Plane pl(center, ray_p1-center, ray_p2-center);
  Point2d center_2d = { pl.CoordsX(center), pl.CoordsY(center) };
  Point2d ray_p1_2d = { pl.CoordsX(ray_p1), pl.CoordsY(ray_p1) };
  Point2d ray_p2_2d = { pl.CoordsX(ray_p2), pl.CoordsY(ray_p2) };
  // 2 UNKNOWN VARIABLES

  Point pos;
  pos -= center;
  bool b = pos.Dist() < 100.0;

  // use 2d plane based on center, ray_p1, ray_p2
  // find line_line intersection point between lines:
  //  1) center -> surface
  //  2) ray_p1 -> ray_p2
  // Project the coordinates to 2d first.


  // |-----|------|
  // check all points, determine if they're inside or outside of object
  // 2^3 different alternatives to check
  // Can be split to two parts:
  // |----|, 2^2 different alternatives to check:
  // 0    0 = split to |---|---|, split not in this section
  // 0    1 = split in this section
  // 1    0 = split in this section
  // 1    1 = split not in this section

  // END OF 2 UNKNOWN VARIABLES
  Point pos2 = pl.Navigate(res);
  IntersectionPoint ipoint;
  ipoint.p = pos2;
  return ipoint;
#endif
}

bool FindRange(bool b1, bool b2)
{
  if (b1 && !b2) return true;
  if (!b1 && b2) return true;
  return false;
}

std::pair<Range<Point>, Range<Point> > Split(Range<Point> p)
{
  Point p1 = p.start;
  Point p2 = p.end;
  Point middle = Point(0.5*Vector(p1+Vector(p2)));
  return std::make_pair(Range<Point>(p1,middle), Range<Point>(middle,p2));
}

Range<Point> FindSurface(Range<Point> range1, Range<Point> range2, VolumeObject &o, int level);
Range<Point> FindSurface(Range<Point> range, VolumeObject &o, int level)
{
  if (level==0) return range;
  std::pair<Range<Point>, Range<Point> > p = Split(range);
  return FindSurface(p.first, p.second, o, level);
}

Range<Point> FindSurface(Range<Point> range1, Range<Point> range2, VolumeObject &o, int level)
{
  bool b1 = o.Inside(range1.start);
  bool b2 = o.Inside(range1.end);
  bool b3 = o.Inside(range2.start);
  bool b4 = o.Inside(range2.end);
  bool in_this_section1 = FindRange(b1,b2);
  bool in_this_section2 = FindRange(b3,b4);
  std::vector<Range<Point> > res;
  if (in_this_section1)
    {
      res.push_back(Range<Point>(range1.start, range1.end));
    }
  if (in_this_section2)
    {
      res.push_back(Range<Point>(range2.start, range2.end));
    }
  int s = res.size();
  float d = 10000000.0;
  Range<Point> result;
  if (s!=0)
    {
      for(int i=0;i<s;i++)
	{
	  Range<Point> p = FindSurface(res[i],o,level-1);
	  if (Vector(p.start-range1.start).Dist()<d)
	    {
	      d = Vector(p.start-range1.start).Dist();
	      result = p;
	    }
	}
    }
  else
    {
      Range<Point> p1 = FindSurface(range1,o, level-1);
      if (Vector(p1.start-range1.start).Dist()<d)
	{
	  d = Vector(p1.start-range1.start).Dist();
	  result = p1;
	}
      Range<Point> p2 = FindSurface(range2,o, level-1);
      if (Vector(p2.start-range1.start).Dist()<d)
	{
	  d = Vector(p2.start-range1.start).Dist();
	  result = p2;
	}
    }
  return result;
}


class VolumeSpace
{
public:
  // returns negative if inside object
  // returns positive if outside of it
  virtual float DistanceFromSurface(Point p) const=0;
};

class QuadVolumeSpace : public VolumeSpace
{
public:
  QuadVolumeSpace(VolumeSpace &next, Point p1, Point p2, Point p3, Point p4,
		  Vector dir) 
    : /*next(next),*/ p1(p1), p2(p2), p3(p3), p4(p4), dir(dir) { }
  
private:
  //VolumeSpace &next;
  Point p1,p2,p3,p4;
  Vector dir;
  
};
