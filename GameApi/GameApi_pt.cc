
#include "GameApi_h.hh"

EXPORT GameApi::PT GameApi::PointApi::origo()
{
  return add_point(e, 0.0,0.0,0.0);
}
EXPORT GameApi::PT GameApi::PointApi::point(float x, float y, float z)
{
  return add_point(e, x,y,z);
}
EXPORT GameApi::PT GameApi::PointApi::move(PT p1, float dx, float dy, float dz)
{
  Point *p = find_point(e,p1);
  return add_point(e, p->x+dx,p->y+dy,p->z+dz);
}
EXPORT GameApi::PT GameApi::PointApi::move(PT p1, V vec)
{
  Point *p = find_point(e, p1);
  Vector *v = find_vector(e, vec);
  return add_point(e, p->x+v->dx, p->y+v->dy, p->z+v->dz);
}
EXPORT GameApi::PT GameApi::PointApi::mix(PT p1, PT p2, float val)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Point res = *pp1+val*Vector((*pp2)-(*pp1));
  return add_point(e,res.x,res.y,res.z);
}
EXPORT float GameApi::PointApi::pt_x(PT p)
{
  return find_point(e,p)->x;
}
EXPORT float GameApi::PointApi::pt_y(PT p)
{
  return find_point(e,p)->y;
}
EXPORT float GameApi::PointApi::pt_z(PT p)
{
  return find_point(e,p)->z;
}
EXPORT float GameApi::PointApi::dist3d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  float d = (*pp1-*pp2).Dist();
  return d;
}
EXPORT float GameApi::PointApi::dist2d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  Point2d pa = { pp1->x,pp1->y };
  Point2d pb = { pp2->x,pp2->y };
  Vector2d v = pb-pa;
  float d = v.Dist();
  return d;
}
EXPORT GameApi::V GameApi::PointApi::minus(PT p1, PT p2)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Vector v = *pp1 - *pp2;
  return add_vector(e,v.dx,v.dy,v.dz);
}

EXPORT GameApi::PointApi::PointApi(Env &e) : e(e) { }

EXPORT GameApi::PT GameApi::PointApi::from_angle(float radius, float angle)
{
  Point p = Point(0.0,0.0,0.0)+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}
EXPORT GameApi::PT GameApi::PointApi::from_angle(PT center, float radius, float angle)
{
  Point *cen = find_point(e,center);
  Point p = *cen+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}

EXPORT GameApi::PT GameApi::PointApi::spherical_coords(PT pos)
{
  Point *pt = find_point(e, pos);
  float x = pt->x;
  float y = pt->y;
  float z = pt->z;
  float r = sqrt(x*x+y*y+z*z);
  float alfa = acos(z/r);
  float beta = atan(y/x);
  //std::cout << "Spherical" << x << " " << y << " " << z << ":" << r << " " << alfa << " " << beta << std::endl; 
  return add_point(e, alfa, beta, r);
}
EXPORT GameApi::PT GameApi::PointApi::cartesian_coords(PT pos)
{
  Point *pt = find_point(e, pos);
  float x = pt->x;
  float y = pt->y;
  float z = pt->z;

  float alfa = x;
  float beta = y;
  float r = z;
  x = r*sin(alfa)*cos(beta);
  y = r*sin(alfa)*sin(beta);
  z = r*cos(alfa);
  return add_point(e, x,y,z);
}
