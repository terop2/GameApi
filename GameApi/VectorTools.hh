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

#ifndef VECTORTOOLS_HH
#define VECTORTOOLS_HH
#include <iostream>
#include <cmath>

//#pragma warning(disable:4244)
//#pragma warning(disable:4305)

inline float Min(float a, float b)
{
  return a < b ? a : b;
}

inline float Max(float a, float b)
{
  return a > b ? a : b;
}


class ComponentMultipliers
{
public:
  float xm, ym, zm;
};


class Vector;

class Point
{
public:
  Point() : x(0.0), y(0.0), z(0.0) { }
  Point(float xx, float yy, float zz) : x(xx), y(yy), z(zz) { }
  Point(const Vector &v);
public:
  float x,y,z;
public:
  float Dist() const;
  friend float Dist(const Point &a, const Point &b);
  //void Normalize() { x/=s; y/=s; z/=s; s=1.0; }
  static Point Origo() { Point p; p.x = 0; p.y = 0; p.z = 0;  return p; }
  static Point Interpolate(const Point &aVec, const Point &aVec2, float aVal)
  {
    Point v;
    v.x = aVec.x*aVal + aVec2.x*(1.0-aVal);
    v.y = aVec.y*aVal + aVec2.y*(1.0-aVal);
    v.z = aVec.z*aVal + aVec2.z*(1.0-aVal);
    //v.s = aVec.s*aVal + aVec2.s*(1.0-aVal);
    return v;
  }
  void operator+=(const Vector &v);
  void operator-=(const Vector &v);
  friend Point operator+(const Point &p, const Vector &v);
  friend Point operator-(const Point &p, const Vector &v);
  friend std::ostream &operator<<(std::ostream &o, const Point &p);
  friend std::istream &operator>>(std::istream &i, Point &p);
};

class SphericalPoint
{
public:
  SphericalPoint(Point center) : center(center) { }
  Point ToPoint();
  void FromPoint(Point v);
public:
  Point center;
  float r;
  float alfa;
  float beta;
};



class Vector // a function Point->Point
{
public:
  // Zero is identity elment
  Vector() : dx(0.0), dy(0.0), dz(0.0) { } 
  Vector(float ddx, float ddy, float ddz) : dx(ddx), dy(ddy), dz(ddz) { }
  Vector(const Vector &aVec) : dx(aVec.dx), dy(aVec.dy), dz(aVec.dz) { }
  Vector(const Point &aPoint) : dx(aPoint.x), dy(aPoint.y), dz(aPoint.z) { }
  Point operator()(Point p) const { return ApplyPoint(p); }
  //void Normalize() { dx/=ds; dy/=ds; dz/=ds; ds=1.0; }

  Point ApplyPoint(Point p) const
  {
    Point r;
    r.x = p.x + dx;
    r.y = p.y + dy;
    r.z = p.z + dz;
    return r;
  }
  void operator+=(const Vector &aVec)
  {
    dx+=aVec.dx;
    dy+=aVec.dy;
    dz+=aVec.dz;
  }
  // addition is associative and commutative
  friend Vector operator+(const Vector &aVec, const Vector &aVec2)
  {
    Vector v = aVec;
    v += aVec2;
    return v;
  }
  void operator*=(const float &aFloat)
  {
    dx*=aFloat;
    dy*=aFloat;
    dz*=aFloat;
  }
  void operator/=(const float &aFloat)
  {
    dx/=aFloat;
    dy/=aFloat;
    dz/=aFloat;
  }
  // scalar multiplication is associative (ab)v = a(bv)
  // with 1.0 as multiplicative identity.
  friend Vector operator*(const Vector &aVec, const float &aFloat)
  {
    Vector v = aVec;
    v *= aFloat;
    return v;
  }
  friend Vector operator*(const float &aFloat, const Vector &aVec)
  {
    return aVec * aFloat;
  }
  friend Vector operator/(const Vector &aVec, const float &aFloat)
  {
    return aVec * (1.0 / aFloat);
  }

  static float DotProduct(const Vector &aVec, const Vector &aVec2)
  {
    float r;
    r = aVec.dx * aVec2.dx;
    r += aVec.dy * aVec2.dy;
    r += aVec.dz * aVec2.dz;
    return r;
  }
  static Vector CrossProduct(const Vector &aVec, const Vector &aVec2)
  {
    Vector v;
    v.dx = aVec.dy*aVec2.dz - aVec.dz*aVec2.dy;
    v.dy = aVec.dz*aVec2.dx - aVec.dx*aVec2.dz;
    v.dz = aVec.dx*aVec2.dy - aVec.dy*aVec2.dx;
    return v;
  }
  static float FindProjectionLength(Vector u, Vector u_x)
  {
    float alfa = acos(DotProduct(u,u_x)/u.Dist()/u_x.Dist());
    float a_length = u_x.Dist()*cos(alfa);
    return a_length;
  }
  static std::pair<Vector,Vector> SplitToComponents(Vector u, Vector u_x)
  {
    float alfa = acos(DotProduct(u,u_x)/u.Dist()/u_x.Dist());
    float a_length = u_x.Dist()*cos(alfa);
    Vector a_vec = a_length * u_x;
    Vector b_vec = -a_vec + u;
    return std::make_pair(a_vec, b_vec);
  }

  static float Angle(const Vector &aVec, const Vector &aVec2);

  // -x has property that x + (-x) = 0.
  Vector operator-() const
  {
    Vector v;
    v.dx = -dx;
    v.dy = -dy;
    v.dz = -dz;
    return v;
  }
  void XMultiply(const ComponentMultipliers &aMult)
  {
    dx*=aMult.xm;
    dy*=aMult.xm;
    dz*=aMult.xm;
  }
  void YMultiply(const ComponentMultipliers &aMult)
  {
    dx*=aMult.ym;
    dy*=aMult.ym;
    dz*=aMult.ym;
  }
  void ZMultiply(const ComponentMultipliers &aMult)
  {
    dx*=aMult.zm;
    dy*=aMult.zm;
    dz*=aMult.zm;
  }
  float Dist() const;
  friend float Dist(const Vector &a, const Vector &b) { return (b-a).Dist(); }

  // r = u_x*x + u_y*y + u_z*z
  Vector UnitConvert(const Vector &u_x, const Vector &u_y, const Vector &u_z);

  static Vector UnitX();
  static Vector UnitY();
  static Vector UnitZ();

  friend Vector operator-(const Point &p1, const Point &p2)
  {
    Vector v;
    v.dx = p1.x-p2.x;
    v.dy = p1.y-p2.y;
    v.dz = p1.z-p2.z;
    return v;
  }
  friend std::ostream &operator<<(std::ostream &o, const Vector &p);
  friend std::istream &operator>>(std::istream &i, Vector &p);
public:
  float dx,dy,dz;
};

struct AxisAngle
{
  float angle;
  Vector axis;
};


class Matrix // a function point->point
{
public:
  friend Point operator*(const Point &p, const Matrix &m);
  friend Vector operator*(const Vector &v, const Matrix &m);
  friend Matrix operator*(const Matrix &m1, const Matrix &m2);
  
public:
  void operator*=(const Matrix &m) 
  {
    *this = *this * m;
  }
  static Matrix KeepRotation(const Matrix &m);
  static Matrix Transpose(const Matrix &m);
  static Matrix XRotation(float rot);
  static Matrix YRotation(float rot);
  static Matrix ZRotation(float rot);
  static Matrix Translate(float x, float y, float z);
  static Matrix Scale(float x, float y, float z);
  static Matrix ProjectionTrans(float z_min);
  static Matrix PerspectiveProjection(float dist);
  static Matrix Perspective(float fovy, float aspect, float near, float far);
  static Matrix Ortho(float left, float right, float bottom, float top, float near, float far);
  static Matrix SphericalToCartesian(SphericalPoint p);
  static Matrix CartesianToSpherical(Point p);
  static Matrix Identity();
  static Matrix MultiplyMatrix(const Matrix &m, int count);
  static Matrix RotateAroundAxis(const Vector &v, float angle);
  static Matrix RotateAroundAxisPoint(const Point &p,
				      const Vector &v,
				      float angle);
  static Matrix Inverse(const Matrix &m);
  static Matrix Interpolate(const Matrix &m1, const Matrix &m2, float t); // t = [0.0..1.0]
  static AxisAngle FindAxisAngle(const Matrix &m);
  friend std::ostream &operator<<(std::ostream &o, const Matrix &m);
public:
  float matrix[16];
  bool is_identity;
};

struct Point2d;

class Plane
{
public:
  Plane(Point u_p, Vector v1, Vector v2) 
    : u_p(u_p), u_x(v1), u_y(v2)
  {
    //u_x = v1; // * (1.0/v1.Dist());
    //u_y = v2;// * (1.0/v2.Dist());
  }
  Plane(const Plane &p) : u_p(p.u_p), u_x(p.u_x), u_y(p.u_y) { }
  Plane() : u_p(0.0,0.0,0.0), u_x(0.0,0.0,0.0), u_y(0.0,0.0,0.0) { }
  void Normalize() { u_x/=u_x.Dist(); u_y/=u_y.Dist(); }

  float Dist(Point p) const;
  float CoordsX(Point p) const;
  float CoordsY(Point p) const;
  bool WhichSideOfPlane(Point p) const;
  Vector Normal(float length) const; 
  void RotateAroundNormal(float angle_in_rad);
  Point Navigate(const Point2d &p);
  Vector PlaneNormal() { return Normal(1.0); }
  float Dist2(Point p);
  bool LineSegmentIntersection(Point p1, Point p2, Point2d &outP);
  bool TriangleIntersection(Point p1, Point p2, Point p3, Point2d &res1, Point2d &res2);
public:
  Point u_p;
  Vector u_x;
  Vector u_y;
};



class Color
{
public:
  // vector [(-1,-1,-1)..(1,1,1)]
  Color() : r(0), g(0), b(0), alpha(255) { }
  Color(Vector v);
  Color(int r, int g, int b);
  Color(int r, int g, int b, int alpha);
  Color(unsigned int color);
  void operator*=(float k) { r *= k; g *=k; b *=k; alpha*=k; }
  friend Color operator+(const Color &c1, const Color &c2)
  {
    Color res;
    res.r = c1.r+c2.r;
    res.g = c1.g+c2.g;
    res.b = c1.b+c2.b;
    res.alpha = c1.alpha+c2.alpha;
    return res;
  }
  unsigned int Pixel() { return (alpha<<24) + (b << 16) + (g << 8) + r; }
  friend Color operator*(Color c, float val)
  {
    c*=val;
    return c;
  }
  friend Color operator*(float val, Color c)
  {
    c*=val;
    return c;
  }
  static unsigned short PixelConvert(unsigned int pixel)
  {
    //unsigned int r = pixel &0xff;
    //unsigned int g = pixel &0xff00;
    //unsigned int b = pixel &0xff0000;
    return 0;
  }
  static unsigned int PixelCombine(unsigned int pixel1, unsigned int pixel2)
  {
    unsigned int r = pixel1 & 0xff;
    unsigned int g = pixel1 & 0xff00;
    unsigned int b = pixel1 & 0xff0000;
    unsigned int a = pixel1 & 0xff000000;
    unsigned int r2 = pixel2 & 0xff;
    unsigned int g2 = pixel2 & 0xff00;
    unsigned int b2 = pixel2 & 0xff0000;
    unsigned int a2 = pixel2 & 0xff000000;
    r+=r2;
    g+=g2;
    b+=b2;
    a/=2;
    a2/=2;
    a+=a2;
    r/=2;
    g/=2;
    b/=2;
    r&=0xff;
    g&=0xff00;
    b&=0xff0000;
    a&=0xff000000;
    return r + g + b + a;
  }
  static Color Transparent() { Color c; c.alpha=0; c.r = 0; c.b = 0; c.g = 0; return c; }
  static Color Black() { Color c; c.r=0; c.b=0; c.g = 0; c.alpha=255; return c; }
  static Color White() { Color c; c.r=255; c.b=255; c.g=255; c.alpha=255; return c;}
  static Color CubeCenter() {  Color c; c.r=128; c.b=128; c.g=128; c.alpha=255; return c;}
  static unsigned int ColorCube(float x, float y, float z, float sizex, float sizey, float sizez)
  {
    x/= sizex;
    y/= sizey;
    z/= sizez;
    // now 0..1
    x*=255.0;
    y*=255.0;
    z*=255.0;
    unsigned int r = int(x) &0xff;
    unsigned int g = (int(y)<<8) &0xff00;
    unsigned int b = (int(z)<<16) &0xff0000;
    unsigned int a = ((0xff)<<24) &0xff000000; 
    return r+g+b+a;
  }
  static unsigned int RangeChange(unsigned int color,
				  unsigned int range_upper,
				  unsigned int range_lower)
  {
    unsigned int r = color & 0xff;
    unsigned int g = color & 0xff00;
    unsigned int b = color & 0xff0000;
    unsigned int a = color & 0xff000000;

    unsigned int ru = range_upper & 0xff;
    unsigned int gu = range_upper & 0xff00;
    unsigned int bu = range_upper & 0xff0000;
    unsigned int au = range_upper & 0xff000000;

    unsigned int rl = range_lower & 0xff;
    unsigned int gl = range_lower & 0xff00;
    unsigned int bl = range_lower & 0xff0000;
    unsigned int al = range_lower & 0xff000000;

    float rf = float(r)/255.0;
    float gf = float(g>>8)/255.0;
    float bf = float(b>>16)/255.0;
    float af = float(a>>24)/255.0;

    unsigned int rc = rl + (unsigned int)(rf*(ru-rl));
    unsigned int gc = gl + (unsigned int)(gf*(gu-gl));
    unsigned int bc = bl + (unsigned int)(bf*(bu-bl));
    unsigned int ac = al + (unsigned int)(af*(au-al));
    rc = rc & 0xff;
    gc = gc & 0xff00;
    bc = bc & 0xff0000;
    ac = ac & 0xff000000;
    return rc+gc+bc+ac;

  }
  static unsigned int CubicInterpolate(unsigned int pixel1, unsigned int pixel2, float val)
  {
    unsigned int r = pixel1 & 0xff;
    unsigned int g = pixel1 & 0xff00;
    unsigned int b = pixel1 & 0xff0000;
    unsigned int a = pixel1 & 0xff000000;
    unsigned int r2 = pixel2 & 0xff;
    unsigned int g2 = pixel2 & 0xff00;
    unsigned int b2 = pixel2 & 0xff0000;
    unsigned int a2 = pixel2 & 0xff000000;

    float rf = r;
    float gf = g>>8;
    float bf = b>>16;
    float af = a>>24;

    float rf2 = r2;
    float gf2 = g2>>8;
    float bf2 = b2>>16;
    float af2 = a2>>24;
    
    rf /= 255.0;
    gf /= 255.0;
    bf /= 255.0;
    af /= 255.0;

    rf2 /= 255.0;
    gf2 /= 255.0;
    bf2 /= 255.0;
    af2 /= 255.0;

    rf = rf*rf;
    gf = gf*gf;
    bf = bf*bf;
    af = af*af;

    rf2 = rf2*rf2;
    gf2 = gf2*gf2;
    bf2 = bf2*bf2;
    af2 = af2*af2;
    
    float val1 = 1.0-val;
    float r3 = val1*rf + val*rf2;
    float g3 = val1*gf + val*gf2;
    float b3 = val1*bf + val*bf2;
    float a3 = val1*af + val*af2;
    
    r3 = sqrt(r3);
    g3 = sqrt(g3);
    b3 = sqrt(b3);
    a3 = sqrt(a3);

    r3 *= 255.0;
    g3 *= 255.0;
    b3 *= 255.0;
    a3 *= 255.0;
    
    unsigned int r4 = r3;
    unsigned int g4 = g3;
    unsigned int b4 = b3;
    unsigned int a4 = a3;

    g4 <<= 8;
    b4 <<= 16;
    a4 <<= 24;
    return r4+g4+b4+a4;
  }
  static unsigned int Interpolate(unsigned int pixel1, unsigned int pixel2, float val)
  {

    unsigned int r = pixel1 & 0xff;
    unsigned int g = pixel1 & 0xff00;
    unsigned int b = pixel1 & 0xff0000;
    unsigned int a = pixel1 & 0xff000000;
    unsigned int r2 = pixel2 & 0xff;
    unsigned int g2 = pixel2 & 0xff00;
    unsigned int b2 = pixel2 & 0xff0000;
    unsigned int a2 = pixel2 & 0xff000000;

    a >>= 4;
    a2 >>= 4;

    float val1 = 1.0-val;
    unsigned int r3 = ((unsigned int)(r*val1 + r2*val)) & 0xff;
    unsigned int g3 = ((unsigned int)(g*val1 + g2*val)) & 0xff00;
    unsigned int b3 = ((unsigned int)(b*val1 + b2*val)) & 0xff0000;
    unsigned int a3 = ((unsigned int)(a*val1 + a2*val)) & 0x0ff00000;
    a3 <<= 4;
    return r3+g3+b3+a3;
  }

  static Color Interpolate(const Color &aVec, const Color &aVec2, float aVal)
  {
    if (aVal < 0.0) aVal = 0.0;
    if (aVal > 1.0) aVal = 1.0;
    Color v;
    v.r = int(float(aVec.r)*(1.0-aVal) + float(aVec2.r)*aVal);
    v.g = int(float(aVec.g)*(1.0-aVal) + float(aVec2.g)*aVal);
    v.b = int(float(aVec.b)*(1.0-aVal) + float(aVec2.b)*aVal);
    v.alpha = int(float(aVec.alpha)*(1.0-aVal) + float(aVec2.alpha)*aVal);
    return v;
  }

  static Color Lighter(const Color &aVec, float aVal)
  {
    return Interpolate(aVec, Black(), aVal);
  }
  static Color Darker(const Color &aVec, float aVal)
  {
    return Interpolate(aVec, White(), aVal);
  }
  static Color AverageArray(Color *array, int size)
  {
    if (size==0) return Color(0,0,0,0);
    int rr=0;
    int gg=0;
    int bb=0;
    int aa=0;
    for (int i=0;i<size;i++)
      {
	rr+=array[i].r;
	gg+=array[i].g;
	bb+=array[i].b;
	aa+=array[i].alpha;
      }
    rr/=size;
    gg/=size;
    bb/=size;
    aa/=size;
    return Color(rr,gg,bb,aa);
  }

public:
  int r,g,b; // each component 0.255
  int alpha;
};

struct UnitVectors
{
  void OrthoNormalize();
  void Scale(float x);
  Vector u_x, u_y, u_z;
};
struct Coords
{
  void OrthoNormalize();
  void Scale(float x);
  void Reposition(Point p) { center = p; }
  Point FindInternalCoords(Point external_coords) const;
  Point FindExternalCoords(Point internal_coords) const;
  Point center;
  Vector u_x, u_y, u_z;
};
Point UnitCube(Point p, Point pos, Vector u_x, Vector u_y, Vector u_z);
Point UnitToCube(Point p, Point pos, Vector u_x, Vector u_y, Vector u_z);
Point UnitToFlex(Point p, 
		 Point bTL, Point bTR, Point bBL, Point bBR,
		 Point fTL, Point fTR, Point fBL, Point fBR);



struct Line
{
  Point p;
  Vector dir;
};


class Quarternion
{
public:
  float x,y,z,w;

  friend Quarternion operator+(const Quarternion &a, const Quarternion &b);
  friend Quarternion operator*(const Quarternion &a, float x);
  static Matrix QuarToMatrix(const Quarternion &q); 
  static Quarternion MatrixToQuar(const Matrix &m); // loses translation
  static Quarternion NLerp(const Quarternion &q, const Quarternion &q2, float t);
};

class LineProperties
{
public:
  LineProperties(Point p1, Point p2) : p1(p1), p2(p2) 
  {
  }
  // x=[0..1], angle=0..2pi
  Plane PlaneFromLine(float x, float angle, float dist);
  Point MiddlePoint(float x) const;
  Vector PerpendicularVector(float angle, float length);
  float Dist(Point p);
  bool TriangleIntersection(Point p1, Point p2, Point p3);
  bool QuadIntersection(Point p1, Point p2, Point p3, Point p4);
  float LineCoords(Point p) const;
private:
  Point p1, p2;
};

class RectProperties
{
public:
  RectProperties(Point tl, Point br) : tl(tl), br(br) { }
  bool IntersectRect(Point tl2, Point br2)
  {
    float start_x = std::max(tl.x,tl2.x);
    float end_x = std::min(br.x, br2.x);
    if (start_x>end_x) return false;
    float start_y = std::max(tl.y, tl2.y);
    float end_y = std::min(br.y, br2.y);
    if (start_y>end_y) return false;
    return true;
  }
private:
  Point tl,br;
};


struct Vector2d;

struct Point2d
{
  float x,y;
  static Point2d NewPoint(float x, float y) { Point2d p; p.x = x; p.y = y; return p; }
  static Point2d FromVector(const Vector2d &v);
  void operator+=(const Point2d &p)
  {
    x += p.x;
    y += p.y;
  }
  void operator+=(const Vector2d &p);
  void operator-=(const Point2d &p)
  {
    x -= p.x;
    y -= p.y;
  }

  void operator*=(float k)
  {
    x*=k;
    y*=k;
  }
  friend Point2d operator+(const Point2d &p, const Point2d &p2)
  {
    Point2d res;
    res.x = p.x + p2.x;
    res.y = p.y + p2.y;
    return res;
  }
  friend Point2d operator+(const Point2d &p, const Vector2d &p2);
  friend Point2d operator*(float x, const Point2d &p)
  {
    Point2d res;
    res.x = x*p.x;
    res.y = x*p.y;
    return res;
  }
  friend std::ostream &operator<<(std::ostream &o, const Point2d &p);
  friend std::istream &operator>>(std::istream &i, Point2d &p);
};
bool IsWithInBoundingBox(Point2d p, Point2d top_left, Point2d bottom_right);
bool LineLineIntersection_Parallel(Point2d l1_p1, Point2d l1_p2,
			     Point2d l2_p1, Point2d l2_p2);
Point2d LineLineIntersection(Point2d l1_p1, Point2d l1_p2,
			     Point2d l2_p1, Point2d l2_p2);

struct Vector2d
{
  float dx,dy;

  void operator*=(float x)
  {
    dx *= x;
    dy *= x;
  }
  void operator/=(float x)
  {
    dx /= x;
    dy /= x;
  }
  float Length() const;
  float Dist() const { return Length(); }
  static float DotProduct(const Vector2d &v, const Vector2d &v2)
  {
    return v.dx*v2.dx+v.dy*v2.dy;
  }
  static Vector2d FromPoint(const Point2d &p)
  {
    Vector2d pp;
    pp.dx = p.x;
    pp.dy = p.y;
    return pp;
  }
  Vector2d operator-() { Vector2d res; res.dx = -dx; res.dy = -dy; return res; }
  friend Vector2d operator+(const Vector2d &p1, const Vector2d &p2)
  {
    Vector2d res;
    res.dx = p1.dx + p2.dx;
    res.dy = p1.dy + p2.dy;
    return res;
  }
  friend Vector2d operator*(float x, const Vector2d &p)
  {
    Vector2d res;
    res.dx = x*p.dx;
    res.dy = x*p.dy;
    return res;
  }
  friend Vector2d operator*(const Vector2d &p, float x)
  {
    Vector2d res;
    res.dx = x*p.dx;
    res.dy = x*p.dy;
    return res;
  }

};
inline Vector2d operator-(const Point2d &p1, const Point2d &p2)
{
  Vector2d v;
  v.dx = p1.x-p2.x;
  v.dy = p1.y-p2.y;
  return v;
}

inline void Point2d::operator+=(const Vector2d &p)
{
  x += p.dx;
  y += p.dy;
}
inline Point2d Point2d::FromVector(const Vector2d &v)
{
  Point2d p;
  p.x = v.dx;
  p.y = v.dy;
  return p;
}
inline Point2d operator+(const Point2d &p, const Vector2d &p2)
{
  Point2d res;
  res.x = p.x + p2.dx;
  res.y = p.y + p2.dy;
  return res;
}

class CubePlanes
{
public:
  CubePlanes(Point p, Vector u_x, Vector u_y, Vector u_z) : p(p), u_x(u_x), u_y(u_y), u_z(u_z) { }
  Plane XY(float z, Point2d normalized_center) const 
  { 
    Point pp(p+normalized_center.x*u_x+normalized_center.y*u_y+z*u_z);
    return Plane(pp, u_x,u_y);
  }
  Plane YZ(float x, Point2d normalized_center) const
  {
    Point pp(p+x*u_x+normalized_center.x*u_y+normalized_center.y*u_z);
    return Plane(pp, u_y, u_z);
  }
  Plane XZ(float y, Point2d normalized_center) const
  {
    Point pp(p+normalized_center.x*u_x+y*u_y+normalized_center.y*u_z);
    return Plane(pp, u_x, u_z);
  }
private:
  Point p;
  Vector u_x;
  Vector u_y;
  Vector u_z;
};

class PlaneIntersection
{ // intersection of plane and plane
public:
  PlaneIntersection(const Plane &p1, const Plane &p2);
  Line Intersection() const;
  float ProjectP1(Point2d p_in_plane1);
  float ProjectP2(Point2d p_in_plane2);
private:
  const Plane &p1;
  const Plane &p2;
  Line l;
};
// defined in Graph.cc
Line PlanePlaneIntersection(const Plane &p1, const Plane &p2);

#if 0
class LineCircleIntersection
{
public:
  LineCircleIntersection(Point2d center, float radius,
			 Point2d line1, Point2d line2)
    : center(center), radius(radius), line1(line1), line2(line2) { }

private:
  //Point2d center;
  float radius;
  Point2d line1;
  Point2d line2;
};
#endif
class FlexiblePlane
{
public:
  Point line1_p1, line1_p2;
  Point line2_p1, line2_p2;
  Point line12_p1(float x) const { return line1_p1+Vector(line1_p2-line1_p1)*x; }
  Point line12_p2(float x) const { return line2_p1+Vector(line2_p2-line2_p1)*x; }

  static Point LinePoint(Point p1, Point p2, float x)
  {
    return p1+Vector(p2-p1)*x;
  }
};

struct FlexibleCube
{
  FlexiblePlane front_plane;
  FlexiblePlane back_plane;
  Point coords(float x, float y, float z)
  {
    Point p1 = front_plane.line12_p1(x);
    Point p2 = front_plane.line12_p2(x);
    Point pp = FlexiblePlane::LinePoint(p1,p2,y);

    Point r1 = back_plane.line12_p1(x);
    Point r2 = back_plane.line12_p2(x);
    Point rp = FlexiblePlane::LinePoint(r1,r2,y);
    
    return FlexiblePlane::LinePoint(pp,rp,z);
    
  }
};

FlexibleCube box(Point p1, float sx, float sy, float sz);

enum GcId
  {
    ERect,
    ELine,
    EBitmap
  };

class GC2d
{
public:
  virtual void Draw2dObject(int id, FlexiblePlane pl, void *data)=0;
};

class GC3d
{
public:
  virtual void Draw3dObject(int id, FlexibleCube c)=0;
};

class Widget3d
{
public:
  virtual void Draw(FlexiblePlane p, GC2d *gc) const=0;
};
class Cube3d
{
public:
  virtual void Draw(FlexibleCube c, GC3d *gc) const=0;
};



#endif
