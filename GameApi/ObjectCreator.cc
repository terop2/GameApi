// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//


#include "ObjectCreator.hh"
#include "Effect.hh"
#include "IntersectObject.hh"

namespace OC {

Point ObjectCreator::Scale(Point p1, Point p2, float p)
{
  return Vector(p1)*p+Vector(p2)*(1.0-p);
}
Point ObjectCreator::Middle(Point p1, Point p2)
{
  return Scale(p1,p2,0.5);
}
Vector ObjectCreator::Dir(Point p1, Point p2)
{
  return p2-p1;
}
Point ObjectCreator::Dist(Point p, Vector v, float dist)
{
  return p + dist*v/v.Dist();
}
Point ObjectCreator::Dist(Point p, Point p2, float dist)
{
  return Dist(p, Dir(p,p2), dist);
}
Point ObjectCreator::Plane2d(Point2d p, Plane pl)
{
  PlaneIn3d plane(p,pl);
  return plane.Elem();
}
Point2d ObjectCreator::Rot(float angle, float dist)
{
  Point2d p;
  p.x = dist*cos(angle);
  p.y = dist*sin(angle);
  return p;
}

Point ObjectCreator::Curve(const CurveIn3d &curve, float x)
{
  return curve.Index(x);
}

float ObjectCreator::SphereFunc(Sphere s, Point p)
{
  Vector v = p-s.center;
  v /= s.radius;
  return v.Dist();
}

Point ObjectCreator::Intersect(Line l, Function<Point,float> &func)
{
  Ray r(l.p1, Dir(l.p1,l.p2));
  SolvableCompose<Point> compose(r,func);
  float x = Solve(compose, 0.0, 1.0);
  return r.Index(x);
}

Point ObjectCreator::Intersect(Line l, Sphere s)
{
  SphereIntersection sphere(s.center, s.radius);
  return Intersect(l, sphere);
}

Triangle ObjectCreator::Tri(Point p, Point p2, Point p3)
{
  Triangle t;
  t.p1 = p;
  t.p2 = p2;
  t.p3 = p3;
  return t;
}
Triangle ObjectCreator::Tri(Point p, Vector v1, Vector v2)
{
  Triangle t;
  t.p1 = p;
  t.p2 = p + v1;
  t.p3 = p + v2;
  return t;
}
Triangle ObjectCreator::Tri(Line l, Point p)
{
  Triangle t;
  t.p1 = l.p1;
  t.p2 = l.p2;
  t.p3 = p;
  return t;
}
Line ObjectCreator::Ln(Point p, Point p2)
{
  Line l;
  l.p1 = p;
  l.p2 = p2;
  return l;
}
Line ObjectCreator::Ln(Point p, Vector v)
{
  Line l;
  l.p1 = p;
  l.p2 = p + v;
  return l;
}
PointList ObjectCreator::PL(Triangle t)
{
  PointList p;
  p.vec.push_back(t.p1);
  p.vec.push_back(t.p2);
  p.vec.push_back(t.p3);
  return p;
}
PointList ObjectCreator::PL(Line l)
{
  PointList p;
  p.vec.push_back(l.p1);
  p.vec.push_back(l.p2);
  return p;
}

PointList ObjectCreator::PL(Point pp)
{
  PointList p;
  p.vec.push_back(pp);
  return p;
}
PointList ObjectCreator::MovePL(const PointList &p, Vector v)
{
  PointList pp;
  for(std::vector<Point>::const_iterator i=p.vec.begin();i!=p.vec.end();i++)
    {
      pp.vec.push_back(*i + v);
    }

  return pp;
}
LineList ObjectCreator::Lines(const PointList &p1, const PointList &p2)
{
  if (p1.vec.size() != p2.vec.size())
    {
      std::cout << "ERROR: ObjectCreator::Lines() PointLists wrong size" << std::endl; 
    }
  LineList l;
  std::vector<Point>::const_iterator i1, i2;
  for(i1=p1.vec.begin(), i2=p2.vec.begin(); i1!=p1.vec.end();i1++,i2++)
    {
      l.vec.push_back(Ln(*i1, *i2));
    }
  return l;
}

Quad ObjectCreator::Q(Point p1, Point p2, Point p3, Point p4)
{
  Quad q;
  q.p1 = p1;
  q.p2 = p2;
  q.p3 = p3;
  q.p4 = p4;
  return q;
}

QuadList ObjectCreator::Quads(const PointList &p1, const PointList &p2)
{
  if (p1.vec.size() != p2.vec.size())
    {
      std::cout << "ERROR: ObjectCreator::Quads() PointLists wrong size" << std::endl; 
    }

  int size = p1.vec.size();
  std::vector<Point>::const_iterator i1 = p1.vec.begin();
  std::vector<Point>::const_iterator i2 = p2.vec.begin();
  QuadList ql;
  int i = 0;
  for(;i1!=p1.vec.end();i1++, i2++,i++)
    {
      ql.vec.push_back(Q(*i1,*i2,p2.vec[(i+1)%size], p1.vec[(i+1)%size]));
    }
  return ql;
}
void ObjectCreator::AppendP(PointList &p, Point p1)
{
  p.vec.push_back(p1);
}

void ObjectCreator::AppendL(LineList &l, Line l1)
{
  l.vec.push_back(l1);
}


} // namespace OC
