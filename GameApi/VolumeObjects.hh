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
// You should have received a copy of the Library GNU General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef VOLUMEOBJECTS_HH
#define VOLUMEOBJECTS_HH

#include "VectorTools.hh"
#include "Buffer.hh"
#include "Effect.hh"
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include "Shader.hh"

#if 0
template<class T>
class Member
{
public:
  virtual bool Inside(T t) const=0;
};

template<class X, class Y>
class FunctionMember : public Member<X>
{
public:
  FunctionMember(Member<Y> &m, Function<X,Y> &f) : m(m), f(f) { }
  bool Inside(X x) const
  {
    Y y = f.Index(x);
    return m.Inside(y);
  }
private:
  Member<Y> &m;
  Function<X,Y> &f;
};

template<class X, class Y>
class Intersection : public Member<std::pair<X,Y> >
{
public:
  Intersection(Member<X> &m1, Member<Y> &m2) : m1(m1), m2(m2), f1(f1), f2(f2) { }
  bool Inside(std::pair<X,Y> xy) const
  {
    X x = xy.first;
    Y y = xy.second;
    return m1.Inside(x) && m2.Inside(y);
  }  
private:
  Member<X> &m1;
  Member<Y> &m2;
};

template<class A>
class Diagonal : public Function<A, std::pair<A,A> >, public Member<std::pair<A,A> >
{
public:
  std::pair<A,A> Index(A a) { return make_pair(a,a); }
  bool Inside(std::pair<A,A> &aa) { return aa.first == aa.second; }
};

template<class A, class B>
class GraphX : public Function<std::pair<A,A>, std::pair<A,B> >, public Member<std::pair<A,B> >
{
public:
  GraphX(Function<A,B> &ab) : ab(ab) { }
  std::pair<A,B> Index(std::pair<A,A> &aa)
  {
    return make_pair(aa.first, ab.Index(aa.second));
  }
  bool Inside(std::pair<A,B> p) const
  {
    //TODO 
  }
private:
  Function<A,B> &ab;
};

template<class X, class Y>
class Equalizer : public Member<X>
{
public:
  Equalizer(Function<X,Y> &f1, Function<X,Y> &f2) : g_f1(f1), g_f2(f2), i(g_f1, g_f2) { }
  bool Inside(X x) const
  {
    std::pair<X,X> p = d.Index(x);
    std::pair<X,Y> p_f = g_f1.Index(p);
    return i.Inside(p_f);
  }
private:
  Diagonal<X> d;
  Intersection<std::pair<X,Y>, std::pair<X,Y> > i;
  GraphX<X,Y> g_f1;
  Graphx<X,Y> g_f2;
};
template<class A, class B>
class First : public Function<std::pair<A,B>, A>
{
public:
  
};

#endif
struct PointPair
{
  Point p1;
  Point p2;
};

class VolumeObject
{
public:
  virtual bool Inside(Point v) const { return false; }
  bool EquRel(Point p1, Point p2) { return Inside(p1)==Inside(p2); }
  virtual float Line(Point v, Point v2) const { return 0.0; }
  virtual Color ColorValue(Point v) const { return Color(255,255,255); }
  virtual float Value(Point v) const { return 0.0; }
  virtual Vector Normal(Point v) const { return Vector(0.0,0.0,0.0); }
  virtual ~VolumeObject() { }
#if 0
  struct Triplet
  {
    O o;
    Point p;
  };
  virtual Triplet InsideTriplet(Point v) const=0;
#endif
};


class BitmapVolume; // see Graph.hh

class MatrixVolumeObject : public VolumeObject
{
public:
  MatrixVolumeObject(VolumeObject *obj, Matrix m) : obj(obj), m(m) { }
  virtual bool Inside(Point v) const { 
    Point p = v*m; 
    return obj->Inside(p);
  }
  virtual float Line(Point v, Point v2) const {
    Point p1 = v*m;
    Point p2 = v2*m;
    return obj->Line(p1,p2);
  }
  virtual Color ColorValue(Point v) const {
    Point p = v*m;
    return obj->ColorValue(p);
  }
  virtual float Value(Point v) const {
    Point p = v*m;
    return obj->Value(p);
  }
  virtual Vector Normal(Point v) const {
    Point p=v*m;
    return obj->Normal(p);
  }
private:
  VolumeObject *obj;
  Matrix m;
};

class FloatVolumeObject {
public:
  virtual float FloatValue(Point p) const=0;
  virtual ~FloatVolumeObject() { }
};

class MinDistFloatVolume : public FloatVolumeObject
{
public:
  MinDistFloatVolume(Curve<Point> &curve) : curve(curve) { }
  float FloatValue(Point p) const
  {
    ClosestDistanceFromCurve::DistRes res = closest.MinDistanceFromCurve(curve, p);
    return res.distance;
  }
private:
  Curve<Point> &curve;
  ClosestDistanceFromCurve closest;
};

class ColorVolumeObject {
public:
  virtual unsigned int ColorValue(Point p) const=0;
  virtual ~ColorVolumeObject() { }
};

class VectorVolumeObject {
public:
  virtual Vector VectorValue(Point p) const=0;
  virtual ~VectorVolumeObject() { }
};

class MinFloatVolumeObject : public FloatVolumeObject {
public:
  MinFloatVolumeObject(FloatVolumeObject *f1, FloatVolumeObject *f2)
    : f1(f1), f2(f2) { }
  virtual float FloatValue(Point p) const
  {
    return std::min(f1->FloatValue(p), f2->FloatValue(p));
  }
private:
  FloatVolumeObject *f1, *f2;
};


class PlusFloatVolumeObject : public FloatVolumeObject {
public:
  PlusFloatVolumeObject(FloatVolumeObject *f1, FloatVolumeObject *f2)
    : f1(f1), f2(f2) { }
  virtual float FloatValue(Point p) const
  {
    return f1->FloatValue(p)+f2->FloatValue(p);
  }
private:
  FloatVolumeObject *f1, *f2;
};


class MoveFloatVolumeObject : public FloatVolumeObject {
public:
  MoveFloatVolumeObject(FloatVolumeObject *next, 
			float dx, float dy, float dz) : next(next),
							dx(dx), dy(dy), dz(dz) { }
  virtual float FloatValue(Point p) const
  {
    p-=Vector(dx,dy,dz);
    return next->FloatValue(p);
  }
private:
  FloatVolumeObject *next;
  float dx,dy,dz;
};

class MaxFloatVolumeObject : public FloatVolumeObject {
public:
  MaxFloatVolumeObject(FloatVolumeObject *f1, FloatVolumeObject *f2)
    : f1(f1), f2(f2) { }
  virtual float FloatValue(Point p) const
  {
    return std::max(f1->FloatValue(p), f2->FloatValue(p));
  }
private:
  FloatVolumeObject *f1, *f2;
};

class SubVolume : public VolumeObject
{
public:
  SubVolume(FloatVolumeObject &obj, float start_range, float end_range) : obj(obj), start_range(start_range), end_range(end_range) { }
  virtual bool Inside(Point v) const {
    float val = obj.FloatValue(v);
    return val>=start_range && val<=end_range;
  }

private:
  FloatVolumeObject &obj; 
  float start_range; 
  float end_range;
};




class TimedVolumeObject
{
public:
  virtual bool Inside(Point p, float time) const=0;
};

class VolumeObjectFunction : public VolumeObject
{
public:
  VolumeObjectFunction(const VolumeObject &next, Function<Point,Point> &f) : next(next), f(f) { }
  virtual bool Inside(Point v) const 
  {
    return next.Inside(f.Index(v));
  }
  Color ColorValue(Point v) const
  {
    return next.ColorValue(f.Index(v));
  }
  
private:
  const VolumeObject &next;
  Function<Point, Point> &f;
};

template<class T>
class VolumeObjectFunction2 : public Function<T, bool>
{
public:
  VolumeObjectFunction2(Function<T, Point> &f, VolumeObject &v) : f(f), v(v) { }
  bool Index(T t) const
  {
    return v.Inside(f.Index(t));
  }
private:
  Function<T, Point> &f;
  VolumeObject &v;
};

class ColorCubeVolume : public VolumeObject
{
public:
  ColorCubeVolume(VolumeObject *next_) : next(next_) { }
  bool Inside(Point v) const { return next->Inside(v); }
  float Line(Point v, Point v2) const { return next->Line(v,v2); }
  virtual Vector Normal(Point v) const { return next->Normal(v); }
  Color ColorValue(Point v) const
  {
    Color c;
    c.r = int(v.x*255.0);
    c.g = int(v.y*255.0);
    c.b = int(v.z*255.0);
    return c;
  }
private:
  VolumeObject *next;
};

class SphereSurfaceVolume : public VolumeObject
{
public:
  SphereSurfaceVolume(Point origo, float radius, float gap) : origo(origo), radius(radius),gap(gap) { }
  bool Inside(Point v) const
  {
    v-=origo;
    float d = v.Dist();
    if (d>radius-gap && d <radius+gap)
      {
	return true;
      }
    return false;
  }
private:
  Point origo;
  float radius;
  float gap;
};

//#if 0
class SphereVolume : public VolumeObject
{
public:
  SphereVolume(Point origo, float radius);
  bool Inside(Point v) const ;
  float Line(Point v, Point v2) const;
  Vector Normal(Point v) const;

  Point Part(const SphericalPoint &part);

  struct R_p // equivalence relation from sets for mathematics
  {
    Point p1;
    Point p2;
    bool b;
  };
#if 0
  virtual Triplet InsideTriplet(Point v) const
  {
    Triplet t;
    t.o = o;
    t.p = v;
    return t;
  }
#endif
private:
  float radius;
  float radius_x_radius;
  Point origo;
#if 0
  O o;
#endif
};
//#endif
class ConeVolume : public VolumeObject
{
public:
  ConeVolume(const Point &pos, const Vector &line, float rad1, float rad2);
  bool Inside(Point v) const;
  Vector Normal(Point v) const;
  
  struct U
  {
    float line;
    float alfa;
    float r;
  };
  Point Part(const U &u);
private:
  Point pos;
  Vector line;
  float rad1;
  float rad2;
};

class CubeVolume : public VolumeObject
{
public:
  CubeVolume(float minx, float maxx,
	     float miny, float maxy,
	     float minz, float maxz) 
  : minx(minx), maxx(maxx),
    miny(miny), maxy(maxy),
    minz(minz), maxz(maxz) { }

  bool Inside(Point p) const
  {
    if (p.x<minx || p.x>maxx) return false;
    if (p.y<miny || p.y>maxy) return false;
    if (p.z<minz || p.z>maxz) return false;
    return true;
  }

private:
  float minx, maxx;
  float miny, maxy;
  float minz, maxz;  
};


class SphereTextureVolume : public VolumeObject
{
public:
  SphereTextureVolume(const VolumeObject &next, const BufferRef &texture, const Point &center, float multx, float multy) : centerreq(center), multx(multx), multy(multy), next(&next) { }
  float Value(Point v) const { return next->Value(v); }
  bool Inside(Point v) const { return next->Inside(v); }
  virtual Vector Normal(Point v) const { return next->Normal(v); }

  Color ColorValue(Point p) const
  {
    Point center = centerreq;
    float x=p.x-center.x, y=p.y -center.y, z=p.z-center.z;
    float r = sqrtf(x*x+y*y+z*z);
    float alfa = acosf(z/r);
    float beta = atan2f(y,x);
    //
    
    int tx = int(alfa/3.14159*(texture.width)*multx);
    int ty = int((3.14159+beta)/3.14159/2.0*(texture.height)*multy);
    tx %= texture.width;
    ty %= texture.height;
    //if (tx > texture->width) tx = 0;
    //if (ty > texture->height) ty = 0;
    //if (tx < 0) tx = 0;
    //if (ty < 0) ty = 0;
    unsigned int color = texture.buffer[tx+ty*texture.ydelta];
    return Color(color);

  }
private:
  mutable BufferRef texture;
  const Point &centerreq;
  float multx, multy;
  const VolumeObject *next;
};

class ColorMixVolume : public VolumeObject
{
public:
  ColorMixVolume(const VolumeObject &t, const VolumeObject &k) : t(&t), k(&k) { }
  bool Inside(Point v) const 
  {
    return t->Inside(v);
  }
  Color ColorValue(Point v) const
  {
    Color ct = t->ColorValue(v);
    Color ck = k->ColorValue(v);
    unsigned int comb = Color::PixelCombine(ct.Pixel(),ck.Pixel());
    return Color(comb);
  }
private:
  const VolumeObject *t;
  const VolumeObject *k;
};

class ColorSumVolume : public VolumeObject
{
public:
  ColorSumVolume(const VolumeObject &t, const VolumeObject &k) : t(&t), k(&k) { }
  bool Inside(Point v) const 
  {
    return t->Inside(v);
  }
  Color ColorValue(Point v) const
  {
    Color ct = t->ColorValue(v);
    Color ck = k->ColorValue(v);
    //unsigned int comb = Color::PixelCombine(ct.Pixel(),ck.Pixel());
    Color c;
    c.r = ct.r + ck.r;
    c.g = ct.g + ck.g;
    c.b = ct.b + ck.b;
    if (c.r > 255) c.r = 255;
    if (c.g > 255) c.g = 255;
    if (c.b > 255) c.b = 255;
    return c;
  }
private:
  const VolumeObject *t;
  const VolumeObject *k;
};


class AndVolume  : public VolumeObject
{
public:
  AndVolume(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
  bool Inside(Point v) const 
  {
    return tt.Inside(v) && kk.Inside(v);
  }
  Color ColorValue(Point v) const
  { 
    return tt.ColorValue(v);
  }

private:
  VolumeObject &tt;
  VolumeObject &kk;
};

class AndNotVolume  : public VolumeObject
{
public:
  AndNotVolume(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
  bool Inside(Point v) const 
  {
    return tt.Inside(v) && !kk.Inside(v);
  }
  Color ColorValue(Point v) const
  { 
    if (tt.Inside(v-Vector(0.0,0.0,0.01)))
      return kk.ColorValue(v);
    return tt.ColorValue(v);
  }
  Vector Normal(Point v) const 
  { 
    if (tt.Inside(v-Vector(0.0,0.0,0.01)))
      return -kk.Normal(v);
    return tt.Normal(v);
  }

private:
  VolumeObject &tt;
  VolumeObject &kk;
};


class NotVolume  : public VolumeObject
{
public:
  NotVolume(VolumeObject &t) : tt(t) { }
  bool Inside(Point v) const  
  {
    return !tt.Inside(v);
  }
  Color ColorValue(Point v) const
  { 
    return tt.ColorValue(v);
  }
private:
  VolumeObject &tt;
};


class ImplyVolume  : public VolumeObject
{
public:
  ImplyVolume(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
  bool Inside(Point v) const
  {
    bool b1 = tt.Inside(v);
    bool b2 = kk.Inside(v);
    if (b1 && b2) return true;
    if (b1 && !b2) return false;
    if (!b1 && b2) return true;
    if (!b1 && !b2) return true;
    return false;
  }
  Color ColorValue(Point v) const
  { 
    return tt.ColorValue(v);
  }
private:
  VolumeObject &tt;
  VolumeObject &kk;
};

class EquivalenceVolume  : public VolumeObject
{
public:
  EquivalenceVolume(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
  bool Inside(Point v) const
  {
    bool b1 = tt.Inside(v);
    bool b2 = kk.Inside(v);
    if (b1 && b2) return true;
    if (b1 && !b2) return false;
    if (!b1 && b2) return false;
    if (!b1 && !b2) return true;
    return false;
  }
  Color ColorValue(Point v) const
  { 
    return tt.ColorValue(v);
  }
private:
  VolumeObject &tt;
  VolumeObject &kk;
};



class OrVolume  : public VolumeObject
{
public:
  OrVolume(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
  bool Inside(Point v) const 
  {
    return tt.Inside(v) || kk.Inside(v);
  }
  virtual Vector Normal(Point v) const 
  { 
    if (tt.Inside(v))
      {
	return tt.Normal(v); 
      }
    else
      {
	return kk.Normal(v);
      }
  }
  float Line(Point p, Point p2) const
  {
    float f1 = tt.Line(p,p2);
    float f2 = kk.Line(p,p2);
    if (f1 < 0.0) f1 = 0.0002;
    if (f2 < 0.0) f2 = 0.0002;
    if (f1 < 0.0001) return f2;
    if (f2 < 0.0001) return f1;
    return f2 < f1 ? f2 : f1;
  }

  Color ColorValue(Point v) const
  { 
    if (tt.Inside(v) && kk.Inside(v))
      {
	Color tc = tt.ColorValue(v);
	Color kc = kk.ColorValue(v);
	tc.r += kc.r;
	tc.b += kc.b;
	tc.g += kc.g;
	tc.r /= 2;
	tc.g /= 2;
	tc.b /= 2;
	return tc;
      }
    else
    if (tt.Inside(v))
      {
	return tt.ColorValue(v);
      }
    else if (kk.Inside(v))
      {
	return kk.ColorValue(v);
      }
    else
      {
	return Color(int(128.0*v.x),int(128.0*v.y),int(128.0*v.z));
      }

  }
private:
  VolumeObject &tt;
  VolumeObject &kk;
};

#if 0
class OrVolumeArray : public VolumeObject
{
public:
  OrVolumeArray(VolumeObject *array, int size) : array(array), size(size) {}

private:
  VolumeObject *array;
  int size;
};
#endif

class ColorSpecVolume  : public VolumeObject
{
public:
  ColorSpecVolume(VolumeObject &val_, Color c_) : c(c_), val(val_) { }
  ColorSpecVolume(VolumeObject &val_, int r, int g, int b) : c(r,g,b), val(val_) { }
  Color ColorValue(Point /*v*/) const { return c; }
  float Value(Point v) const { return val.Value(v); }
  bool Inside(Point v) const { return val.Inside(v); }
  float Line(Point v, Point v2) const { return val.Line(v,v2); }
  Color LineColor(Point v, Point v2) const { return c; }
  virtual Vector Normal(Point v) const { return val.Normal(v); }

private:
  Color c;
  VolumeObject &val;
};
template<class T>
class MoveVolume  : public VolumeObject
{
public:
  MoveVolume() :  origo(Vector(0.0,0.0,0.0)), next(0) { }
  MoveVolume(T &next_, Vector origo_) : origo(origo_), next(&next_) { }
  MoveVolume(T &next_, float x, float y, float z) : origo(x,y,z), next(&next_) { }
  void SetT(T &next_) { next = &next_; }
  void SetOrigo(Vector v) { origo = v; }
  bool Inside(Point p) const 
  {
    //p = (-origo).ApplyPoint(p);
    p.x -= origo.dx;
    p.y -= origo.dy;
    p.z -= origo.dz;
    return next->Inside(p);
  }
  float Value(Point p) const
  {
    //p = (-origo).ApplyPoint(p);
    p.x -= origo.dx;
    p.y -= origo.dy;
    p.z -= origo.dz;
    return next->Value(p);
  }
  Color ColorValue(Point p) const
  {
    p.x -= origo.dx;
    p.y -= origo.dy;
    p.z -= origo.dz;
    return next->ColorValue(p);
  }  
private:
  Vector origo;
  T *next;
};

class ScaleVolume : public VolumeObject
{
public:
  ScaleVolume(VolumeObject *next_, float scale_x, float scale_y, float scale_z) : next(next_), scale_x(scale_x), scale_y(scale_y), scale_z(scale_z) { }
  bool Inside(Point p) const
  {
    p.x /= scale_x;
    p.y /= scale_y;
    p.z /= scale_z;
    return next->Inside(p);
  }
  float Value(Point p) const
  {
    p.x /= scale_x;
    p.y /= scale_y;
    p.z /= scale_z;
    return next->Value(p);
  }
  Color ColorValue(Point p) const
  {
    p.x /= scale_x;
    p.y /= scale_y;
    p.z /= scale_z;
    return next->ColorValue(p);    
  }

private:
  VolumeObject *next;
  float scale_x, scale_y, scale_z;
};


class BoxVolume  : public VolumeObject
{
public:
  BoxVolume(float x1, float y1, float z1,
      float x2, float y2, float z2) : tl(x1,y1,z1), br(x2,y2,z2) { }
  BoxVolume(Vector top_left, Vector bottom_right) : tl(top_left), br(bottom_right) { }
  bool Inside(Point p) const
  {
    if (p.x < tl.dx) return false;
    if (p.x > br.dx) return false;

    if (p.y < tl.dy) return false;
    if (p.y > br.dy) return false;

    if (p.z < tl.dz) return false;
    if (p.z > br.dz) return false;
    return true;
  }
  //float Integral(Range x, Range y, Range z)
  //{
    //Rectangle r;
    //Range z;
  //}

public:
  //Range x,y,z;
  Vector tl, br;
};


class TorusVolume : public VolumeObject
{
public:
  TorusVolume(Vector u_x, Vector u_y, float dist1_, float dist2_, Point center_) : plane(center_, u_x, u_y), dist1(dist1_), dist2(dist2_), center(center_) { }
  bool Inside(Point p) const;

private:
  Plane plane;
  float dist1; // defines dist of 1st circle from origo
  float dist2; // torus dist
  Point center;
};

class FaceCollectionVolume : public VolumeObject
{
public:
  FaceCollectionVolume(FaceCollection *coll, Point p) : coll(coll),pp(p) { }
  bool Inside(Point p) const;
private:
  FaceCollection *coll;
  Point pp;
};


class Random
{
public:
  Random() /*: seed1(555), seed2(999)*/ { }
  unsigned int next() { return rand(); /*seed1 += seed2; seed2 = seed1; return seed1 >> 16;*/ }
  unsigned int maximum() { return RAND_MAX; /*return 0xffff;*/ }
private:
  //unsigned int seed1, seed2;
};

class RandomPointsInBox : public PointCollection
{
public:
  RandomPointsInBox(int num, Point p, float x, float y, float z) : num(num),p(p), x(x), y(y), z(z) { }

  int Size() const { return num; }
  Point Index(int i) const
  {
    Random r;
    float xp = double(r.next())/r.maximum()*x + p.x;
    float yp = double(r.next())/r.maximum()*y + p.y;
    float zp = double(r.next())/r.maximum()*z + p.z;
    return Point(xp,yp,zp);
  }
private:
  int num;
  Point p;
  float x,y,z;
};

class RandomVolumePolys : public BoxableFaceCollection
{
public:
  RandomVolumePolys(VolumeObject &vol, PointCollection &coll, int numfaces, Point p, float x, float y, float z) : vol(vol), coll(coll), numfaces(numfaces), p(p), x(x),y(y),z(z), facem(-999) { }
  void SetBox(Matrix m) { }
  virtual int NumFaces() const { return numfaces; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    if (face==facem)
      {
	return pointm_ + (coll.Index(point)*Matrix::XRotation(xrot)*Matrix::YRotation(yrot)*Matrix::ZRotation(zrot));
      }

    while(1)
      {
	Random r;
	float xp = double(r.next())/r.maximum()*x + p.x;
	float yp = double(r.next())/r.maximum()*y + p.y;
	float zp = double(r.next())/r.maximum()*z + p.z;
	xrot = double(r.next())/r.maximum()*2.0*3.14159;
	yrot = double(r.next())/r.maximum()*2.0*3.14159;
	zrot = double(r.next())/r.maximum()*2.0*3.14159;
	pointm_ = Point(xp,yp,zp);
	if (vol.Inside(pointm_))
	  break;
      }
    facem = face;
    pointm = point;

    return pointm_ + (coll.Index(point)*Matrix::XRotation(xrot)*Matrix::YRotation(yrot)*Matrix::ZRotation(zrot));
  }
  virtual unsigned int Color(int face, int point) const
  {
    return Color::White().Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    FacePoint(face,point);
    Point pa = coll.Index(0);
    Point pb = coll.Index(1);
    Point pc = coll.Index(2);
    Vector v = pb-pa;
    Vector v2 = pc-pa;
    return Vector::CrossProduct(v,v2)*Matrix::XRotation(xrot)*Matrix::YRotation(yrot)*Matrix::ZRotation(zrot);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    FacePoint(face,point);
    Point p = pointm_;
    switch(id)
      {
      case AttrCenterX: return p.x;
      case AttrCenterY: return p.y;
      case AttrCenterZ: return p.z;
      }
    return 0.0;
  }
private:
  VolumeObject &vol;
  PointCollection &coll;
  int numfaces;
  Point p;
  float x,y,z;
  mutable int facem, pointm;
  mutable Point pointm_;
  mutable float xrot, yrot, zrot;
};

class MandelBulb : public VolumeObject
{
public:
  MandelBulb(float n, Point p, Point center, float radius, int iterations) : n(n), p(p), sp(center,radius), iterations(iterations)  { }
  bool Inside(Point C) const;
private:
  static Point Step(Point p, Point C, float n);
  float n;
  Point p;
  SphereVolume sp;
  int iterations;
  int current_iteration_count;
};

class VolumeEffect : public FrameAnim
{
public:
  VolumeEffect(Render *r);
  ~VolumeEffect();
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
  DiscardPiece discard;
  GouraudPiece gouraud;
  PieceShader piece;
};

class FractalEffect : public FrameAnim
{
public:
  FractalEffect(Render *r) : FrameAnim(r), vbostate(*r) { }
  void Init();
  bool Frame(float time);
  //VBOState vbostate;
  RenderState vbostate;
};

#endif
