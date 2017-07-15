
#include "GameApi_h.hh"
#include "Intersect.hh"

class PlaneCut : public Cutter
{
public:
  PlaneCut(Point pos, Vector u_x, Vector u_y) : pos(pos),u_x(u_x),u_y(u_y) 
  { 

  }
  std::vector<Point> cut(Point p1, Point p2) const
  {

    LinePlaneIntersection sect = LinePlaneIntersectionFunc(p1,p2,
							   pos, pos+u_x, pos+u_y);
    return std::vector<Point> { IntersectionPoint(sect, p1, p2) };
  }
private:
  Point pos;
		 Vector u_x, u_y; 
};

EXPORT GameApi::CT GameApi::CutterApi::plane_cut(PT pos, V u_x, V u_y)
{
  Point *pos_1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);
  return add_cutter(e, new PlaneCut(*pos_1, *u_x1, *u_y1));
}
#if 0
class SphereCut : public Cutter
{
public:
  SphereCut(Point center, float radius) : center(center), radius(radius) { }
  Point cut(Point p1, Point p2) const
  {
    
  }
private:
  Point center;
  float radius;
};
#endif

//EXPORT GameApi::CT GameApi::CutterApi::sphere_cut(PT center, float radius)
//{
//  Point *center_1 = find_point(e, center);
//  return add_cutter(e, new SphereCut(*center_1, radius));
//}
class DistanceCut : public Cutter
{
public:
  DistanceCut(DistanceRenderable *dr) : dr(dr) {}
  std::vector<Point> cut(Point p1, Point p2) const
  {
    float d1 = dr->distance(p1);
    float d2 = dr->distance(p2);
    if (d1<d2)
      std::swap(p1,p2);
    Point p = p1;
    Vector v = (p2-p1);
    float dd = v.Dist();
    v /= dd;
    std::vector<Point> vec;
    //int count = 0;
    float pos = 0.0;
    while(1) {
      float d = dr->distance(p);
      //std::cout << "Dist: " << d << std::endl;
      //std::cout << "Pos: " << pos << std::endl;
      if (pos>1.0) { vec.push_back(p2); break; }//std::vector<Point>{ p2 };
      if (fabs(d)<0.0001) { vec.push_back(p); break; }
      //if (count>30) return Point(0.0,0.0,0.0);
      // p+=v * d;
      pos+=fabs(d/dd);
      //if (pos>=0.0 && pos<=1.0)
	p = (1.0-pos)*Vector(p1)+pos*Vector(p2);
      //count++;
    }
    //if (pos<0.0 ||pos>1.0)
    //  std::cout << p1 << " " << p2 << ":" << p << std::endl;
    return vec;
  }
private:
  DistanceRenderable *dr;
};

EXPORT GameApi::CT GameApi::CutterApi::distance_cut(FD dist1)
{
  DistanceRenderable *dist = find_distance(e, dist1);
  return add_cutter(e, new DistanceCut(dist));
}

class CutterVolume : public VolumeObject
{
public:
  CutterVolume(Cutter *ct, Point pt, float dist) : ct(ct), pt(pt), dist(dist) { }
  bool Inside(Point p) const
  {
    Point p1 = pt;
    Vector u = p-p1;
    u /= u.Dist();
    u*=dist;
    Point p2 = p1 + u;
    std::vector<Point> cut_p = ct->cut(p1,p2);
    Vector v1 = p-p1;
    Vector v2 = cut_p[0]-p1;
    return v1.Dist() < v2.Dist();
  }
private:
  Cutter *ct;
  Point pt;
  float dist;
};
EXPORT GameApi::O GameApi::CutterApi::cutter_volume(CT c, PT point, float dist)
{
  Cutter *ct = find_cutter(e, c);
  Point *pt = find_point(e, point);
  return add_volume(e, new CutterVolume(ct, *pt, dist));
}
