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


#include "Effect.hh"

class BezierCurve : public CurveIn3d
{
public:
  BezierCurve(PointCollection &p) : p(p) { }

  Point Index(float pos) const
  {
    //std::cout << "Bezier: " << pos << std::endl;
    int count = p.NumPoints();
    Vector val(0.0,0.0,0.0);
    for(int i=0;i<count;i++)
      { 
	val += bin(pos,i,count-1)*Vector(p.Points(i));
	//std::cout << "val: " << p.Points(i) << ":" << bin(pos,i,count-1) << ":" << val << std::endl;
      }
    return Point(val);
  }
  float bin(float pos, int i, int n) const
  { 
    return float(ni(n,i)) * pow(double(pos), double(i))*pow(double(1.0-pos), double(n-i));
  }
  int ni(int n, int i) const
  {
    if (i==0) { return 1; }
    if (n==i) { return 1; }
    return ni(n-1,i-1)+ni(n-1,i);
  }
  float Size() const
  {
    return 1.0;
  }
private:
  PointCollection &p;
};

class BezierPath : public MatrixCurve
{
public:
  BezierPath(PointCollection &coll) : bezier(coll),move(bezier) { }
  float Size() const { return move.Size(); }
  Matrix Index(float x) const { return move.Index(x); }
private:
  BezierCurve bezier;
  CurveMovement move;
};


class ConnectCurves : public CurveIn3d
{
public:
#if 0
  ConnectCurves(CurveIn3d &curve, CurveIn3d &curve2) : curve(curve), curve2(curve2), p0(curve.Index(curve.Size()*5/6)), p1(curve.Index(curve.Size())), p2(curve2.Index(0.0)), p3(curve2.Index(curve2.Size()*1/6)), vec(&p0, &p3+1), bezier(conv) { }
  float Size() const { return curve.Size()+bezier.Size()+curve2.Size(); }
  Point Index(float x) const
  {
    if (x<curve.Size())
      {
	return curve.Index(x);
      }
    else if (x < curve.Size()+bezier.Size())
      {
	return bezier.Index(1.0-(x-curve.Size()));
      }
    else
      {
	return curve2.Index(x-curve.Size()-bezier.Size());
      }
  }
private:
  CurveIn3d &curve, &curve2;
  Point p0,p1,p2,p3;
  VectorArray<Point> vec;
  //PointCollectionConvert conv;
  BezierCurve bezier;
#endif
};

#if 0
void LinkDist(const Point2d &p1, const Point2d &p2, Point2d &ptarget, Point2d &ptarget2, float dist1, float dist2);


class Circle
{
public:
  Circle(float r) : r(r) { }
  typedef float A;
  typedef std::pair<float,float> B;
  B f(A a) const { return make_pair(r*cos(a), r*sin(a)); }
private:
  float r;
};

template<class I1, class I2>
class Intersection
{
  Intersection(I1 i1, I2 i2) : i1(i1), i2(i2) { }
  typedef bool A;
  typedef std::pair<float,float> B;
  B f(A a) const { return i1.f(f1(a)); }
  B f2(A a) const { return i2.f(f2(a)); }
  virtual I1::A f1(A i)=0;
  virtual I2::A f2(A i)=0;
private:
  I1 i1;
  I2 i2;
};
#endif



