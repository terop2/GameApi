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
// You should have received a copy of the GNU Librry General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//


#include "VectorTools.hh"
#include <vector>
#include "Effect.hh"

namespace OC
{

struct Triangle
{
  Point p1,p2,p3;
};
struct Line
{
  Point p1,p2;
};
struct Quad
{
  Point p1, p2, p3, p4;
};
struct PointList
{
  std::vector<Point> vec;
};
struct TriList
{
  std::vector<Triangle> vec;
};
struct QuadList
{
  std::vector<Quad> vec;
};
struct LineList
{
  std::vector<Line> vec;
};
struct Sphere
{
  float radius;
  Point center;
};


class ObjectCreator
{
public:
  float SphereFunc(Sphere s, Point p);
  static Point Scale(Point p1, Point p2, float p);
  static Point Middle(Point p1, Point p2);
  static Vector Dir(Point p1, Point p2);
  static Point Dist(Point p, Vector v, float dist);
  static Point Dist(Point p, Point p2, float dist);
  static Point Plane2d(Point2d p, Plane pl);
  static Point2d Rot(float angle_in_rad, float dist);
  static Point Curve(const CurveIn3d &curve, float x);
  static Point Intersect(Line l, Sphere s);
  static Point Intersect(Line l, Function<Point, float> &func);
  template<class T>
  static Point Intersect(Line l, float (*func)(T t, Point p), T t);
  static Triangle Tri(Point p, Point p2, Point p3);
  static Triangle Tri(Point p, Vector v1, Vector v2);
  static Triangle Tri(Line l, Point p);
  static Line Ln(Point p, Point p2);
  static Line Ln(Point p, Vector v);
  static PointList PL(Triangle t);
  static PointList PL(Line l);
  static PointList PL(Point p);
  static void AppendP(PointList &p, Point p1);
  static void AppendL(LineList &l, Line l1);
  template<class It>
  static PointList PL(It beg, It end);
  static PointList MovePL(const PointList &p, Vector v);
  static LineList Lines(const PointList &p1, const PointList &p2);
  static QuadList Quads(const PointList &p1, const PointList &p2);
  static Quad Q(Point p1, Point p2, Point p3, Point p4);
};


template<class It>
PointList ObjectCreator::PL(It beg, It end)
{
  PointList p;
  p.vec = std::vector<Point>(beg,end);
  return p;
}
template<class T>
Point ObjectCreator::Intersect(Line l, float (*func)(T t, Point p), T t)
{
  NativeFunction2<T,Point,float> native(func); // (T,Point)->float
  Curry2a<T,Point,float> curry(native, t); // Point->float
  Point p = Intersect(l, curry);
  return p;
}


} // namespace OC
