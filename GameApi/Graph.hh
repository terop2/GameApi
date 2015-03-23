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


#ifndef GRAPH_HH
#define GRAPH_HH

#include "Effect.hh"
#include "VolumeObjects.hh"
#include "Shader.hh"
#include "GraphI.hh"
#include <set>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

Quad QuadInterpolation(const Quad &q1, const Quad &q2, float val);


class QuadInterpolationFunction : public Function<std::pair<Quad,Quad>, Quad>
{ // val = [0..1]
public:
  QuadInterpolationFunction(float val) : val(val) { }
  Quad Index(std::pair<Quad,Quad> p) const
  {
    return QuadInterpolation(p.first,p.second, val);
  }
private:
  float val;
};

struct PointI
{
  int x,y,z;
};

class Rect
{
public:
  std::pair<Rect,Rect> SplitX() 
  {
    Rect r1;
    Rect r2;
    r1.tl.x = tl.x;
    r1.tl.y = tl.y;
    r1.br.x = (tl.x+br.x)/2;
    r1.br.y = br.y;
    r2.tl.x = (tl.x+br.x)/2;
    r2.tl.y = tl.y;
    r2.br.x = br.x;
    r2.br.y = br.y;
    return std::make_pair(r1,r2);
  }
  bool InsideRect(Point2d p)
  {
    if (p.x < tl.x) return false;
    if (p.y < tl.y) return false;
    if (p.x >= br.x) return false;
    if (p.y >= br.y) return false;
    return true;
  }
  Point2d RectInternalCoordinates(Point2d p)
  {
    Point2d p2;
    p2.x = p.x - tl.x;
    p2.y = p.y - tl.y;
    return p2;
  }
  Point2d tl;
  Point2d br;
};
class Region : public Array<int,Rect>
{
};
typedef ArrayConvert<Region, int, Rect> RegionConvert;



class WaveformBitmap : public Bitmap<Color>
{
public:
  WaveformBitmap(Waveform &wv, float start_x, float end_x, float start_y, float end_y, int sx, int sy, Color true_color, Color false_color) : wv(wv), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), sx(sx), sy(sy), true_color(true_color), false_color(false_color) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float pos = float(x)/float(sx)*(end_x-start_x)+start_x;
    float val = wv.Index(pos);
    val -= start_y;
    val /= end_y-start_y;
    val *= float(sy);
    float start_range = 0.0;
    start_range -= start_y;
    start_range /= end_y-start_y;
    start_range *= float(sy);
    float end_range = val;
    if (start_range > end_range) { std::swap(start_range, end_range); }
    if (y >= start_range && y <= end_range)
      {
	return true_color;
      }
    return false_color;
  }

private:
  Waveform &wv;
  float start_x, end_x, start_y, end_y;
  int sx,sy;
  Color true_color, false_color;
  //void *data;
};

class Object3dRender : public Bitmap<bool>
{
public:
  Object3dRender(VolumeObject &o, int sx, int sy, Point p, float min_z, float max_z) : o(o), sx(sx), sy(sy), p(p), min_z(min_z), max_z(max_z) 
  { 
    done = BufferRefMask::NewBuffer(sx,sy);
    render(p,p.z);
  }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  bool Map(int x, int y) const 
  {
    return done.operator[](y)[x] == 1;
  }
  Point2d projection(Point p) const {
    Point2d pp;
    pp.x = p.x;
    pp.y = p.y;
    return pp;
  }
  void render(Point p, float startz) {
    Point2d pp = projection(p);
    if (pp.y<0.0) return;
    if (pp.x<0.0) return;
    if (pp.x>=sx) return;
    if (pp.y>=sy) return;
    if (done.operator[]((int)pp.y)[(int)pp.x]==1) return;
    if (o.Inside(p)) {
      done.operator[]((int)pp.y)[(int)pp.x] = 1;
      render(p+Vector(1.0,0.0,0.0), startz);
      render(p+Vector(0.0,1.0,0.0), startz);
      render(p+Vector(-1.0,0.0,0.0), startz);
      render(p+Vector(0.0,-1.0,0.0), startz);
      return;
    }
    if (p.z>=startz && p.z < max_z)
      render(p+Vector(0.0,0.0,1.0), startz);
    if (p.z<=startz && p.z > min_z)
      render(p+Vector(0.0,0.0,-1.0), startz);
  }
private:
  BufferRefMask done;
  VolumeObject &o;
  int sx,sy;
  Point p;
  float min_z, max_z;
};

class FloatRangeBitmap : public Bitmap<bool>
{
public:
  FloatRangeBitmap(Bitmap<float> &fb, float rs, float re) : fb(fb), rs(rs), re(re) { }
  virtual int SizeX() const { return fb.SizeX(); }
  virtual int SizeY() const { return fb.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    float val = fb.Map(x,y);
    return val >= rs && val <= re;
  }
private:
  Bitmap<float> &fb;
  float rs;
  float re;
};
class SectionsBoolBitmap : public Bitmap<bool>
{
public:
  SectionsBoolBitmap(int sx, int sy, float x, float y, std::function<bool (float angle)> f) : sx(sx), sy(sy), x(x), y(y), f(f) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  
  bool Map(int ax, int ay) const
  {
    float xx = ax;
    float yy = ay;
    xx-=x;
    yy-=y;
    float angle_in_rad = atan2(yy,xx);
    float angle_in_deg = angle_in_rad*360.0/2.0/3.14159;
    bool b = f(angle_in_deg);
    return b;
  }

private:
  int sx,sy;
  float x,y;
  std::function<bool (float)> f;
};

class PartCircleBoolBitmap : public Bitmap<bool>
{
public:
  PartCircleBoolBitmap(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad) : sx(sx), sy(sy), x(x), y(y), sa(start_angle), ea(end_angle), sr(start_rad), er(end_rad) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  
  bool Map(int ax, int ay) const
  {
    ax-=x;
    ay-=y;
    float r = sqrtf(float(ax)*float(ax)+float(ay)*float(ay));
    float angle = atan2f(float(ay),float(ax));
    if (angle<sa || angle>ea) return false;
    if (r<sr || r>er) return false;
    return true;
  }
private:
  int sx,sy;
  float x,y;
  float sa,ea;
  float sr,er;
};

class BoolBitmapFromFloatBitmap2 : public Bitmap<bool>
{
public:
  BoolBitmapFromFloatBitmap2(Bitmap<float> &bm2, 
			     float range_start, 
			     float range_end) 
  : 
    bm(bm2), 
    range_start(range_start), 
    range_end(range_end) 
  { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    float val = bm.Map(x,y);
    return range_start<=val && val <= range_end; 
  }
  
private:
  Bitmap<float> &bm;
  float range_start, range_end;
};

class GrayScaleBitmapFromFloatBitmap : public Bitmap<Color>
{
public:
  GrayScaleBitmapFromFloatBitmap(Bitmap<float> &bm, Color c1, Color c2) : bm(bm), c1(c1), c2(c2) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    float val = bm.Map(x,y);
    return Color::Interpolate(c2,c1,val);
  }

private:
  Bitmap<float> &bm;
  Color c1, c2;
};

class FloatBitmapFromBoolBitmap : public Bitmap<float>
{
public:
  FloatBitmapFromBoolBitmap(Bitmap<bool> &bm, int csx, int csy) : bm(bm), csx(csx), csy(csy) { }
  virtual int SizeX() const { return bm.SizeX()/csx+1; }
  virtual int SizeY() const { return bm.SizeY()/csy+1; }
  virtual float Map(int x, int y) const
  {
    int sx = csx;
    int sy = csy;
    float val = 0.0;
    for(int xx=0;xx<sx;xx++)
      for(int yy=0;yy<sy;yy++)
	{
	  if (x*csx+xx < bm.SizeX() && y*csy+yy<bm.SizeY())
	    val+=bm.Map(x*csx+xx,y*csy+yy)?1.0:0.0;
	}
    val/=csx*csy;
    return val;
  }
  
private:
  Bitmap<bool> &bm;
  int csx,csy;
};

template<class T>
class ChooseTBitmap : public Bitmap<T>
{
public:
  ChooseTBitmap(Bitmap<bool> &bm, T false_t, T true_t) : bm(bm), false_t(false_t), true_t(true_t) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual T Map(int x, int y) const
  {
    if (bm.Map(x,y)) return true_t;
    return false_t;
  }
  
private:
  Bitmap<bool> &bm;
  T false_t, true_t;
};

template<class T>
class FontCharacterString : public Bitmap<T>
{
public:
  FontCharacterString(T def, int x_gap) : def(def), x_gap(x_gap) { }
  void push_back(Bitmap<T> *bm, int top) 
  { 
    vec.push_back(bm); 
    vec2.push_back(top);
  }
  virtual int SizeX() const 
  {
    int sz=vec.size();
    int pixels = 0;
    for(int i=0;i<sz;i++) { pixels+=vec[i]->SizeX()+x_gap; }
    return pixels;
  }
  virtual int SizeY() const 
  {
    return PositiveDelta()+Height();
  }
  virtual T Map(int x, int y) const
  {
    y-=PositiveDelta();
    int sz=vec.size();
    int pixels = 0;
    int oldpixels = 0;
    int i=0;
    for(;i<sz;i++) 
      { 
	pixels+=vec[i]->SizeX()+x_gap;
	if (pixels>x) break;
	oldpixels = pixels;
      }
    if (i==sz) { return def; }
    int delta = -vec2[i];
    if (y<delta) return def;
    if (y>=delta+vec[i]->SizeY()) return def;
    
    return vec[i]->Map(x-oldpixels,y-delta);
  }
  int PositiveDelta() const
  {
    int sz = vec2.size();
    int min_delta=0;
    for(int i=0;i<sz;i++)
      {
	int delta = (-vec2[i]);
	if (delta<min_delta) min_delta=delta;
      }
    return -min_delta;
  }
  int Height() const
  {
    int sz = vec2.size();
    int max_height=0;
    for(int i=0;i<sz;i++)
      {
	int height = (-vec2[i])+vec[i]->SizeY();
	if (height>max_height) max_height=height;
      }
    return max_height;
  }
private:
  std::vector<Bitmap<T>*> vec;
  std::vector<int> vec2;
  T def;
  int x_gap;
};

class NotBitmap : public Bitmap<bool>
{
public:
  NotBitmap(Bitmap<bool> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  bool Map(int x, int y) const { return !bm.Map(x,y); }
private:
  Bitmap<bool> &bm;
};

class OrBitmap : public Bitmap<bool>
{
public:
  OrBitmap(Bitmap<bool> &bm1, Bitmap<bool> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  bool Map(int x, int y) const { return bm1.Map(x,y)||bm2.Map(x,y); }
private:
  Bitmap<bool> &bm1;
  Bitmap<bool> &bm2;
};
class XorBitmap : public Bitmap<bool>
{
public:
  XorBitmap(Bitmap<bool> &bm1, Bitmap<bool> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  bool Map(int x, int y) const { return bm1.Map(x,y) ^ bm2.Map(x,y); }
private:
  Bitmap<bool> &bm1;
  Bitmap<bool> &bm2;
};

class AndNotBitmap : public Bitmap<bool>
{
public:
  AndNotBitmap(Bitmap<bool> &bm1, Bitmap<bool> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  bool Map(int x, int y) const { return bm1.Map(x,y) && !bm2.Map(x,y); }
private:
  Bitmap<bool> &bm1;
  Bitmap<bool> &bm2;
};

class EquivalenceClassIntBitmap : public Bitmap<bool>
{
public:
  EquivalenceClassIntBitmap(Bitmap<int> &bm, int val) : bm(bm), val(val) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    return bm.Map(x,y)==val;
  }
private:
  Bitmap<int> &bm;
  int val;
};

class EquivalenceClassColorBitmap : public Bitmap<bool>
{
public:
  EquivalenceClassColorBitmap(Bitmap<Color> &bm, Color val) : bm(bm), val(val) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    return c.r==val.r && c.g==val.g && c.b == val.b;
  }
  
private:
  Bitmap<Color> &bm;
  Color val;
};


template<class T, class K>
class CombineBitmaps : public Bitmap<std::pair<T,K> >
{
public:
  CombineBitmaps(Bitmap<T> &bm1, Bitmap<K> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  virtual std::pair<T,K> Map(int x, int y) const
  {
    return std::make_pair(bm1.Map(x,y), bm2.Map(x,y));
  }
  
private:
  Bitmap<T> &bm1;
  Bitmap<K> &bm2;
};

template<class T>
class ChooseBitmap : public Bitmap<T>
{
public:
  ChooseBitmap(Bitmap<T> &bm1, Bitmap<T> &bm2, Bitmap<bool> &which) : bm1(bm1), bm2(bm2), which(which) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  virtual T Map(int x, int y) const
  {
    if (which.Map(x,y)) return bm2.Map(x,y);
    return bm1.Map(x,y);
  }
  
private:
  Bitmap<T> &bm1;
  Bitmap<T> &bm2;
  Bitmap<bool> &which;
};


struct MaskedBitmap
{
  Bitmap<Color> *colors;
  Bitmap<unsigned char> *mask;
};

template<class T>
class InverseMapBitmap : public Bitmap<T>
{
public:
  InverseMapBitmap(Bitmap<T> &bm, std::pair<int,int> (*fptr)(std::pair<int,int>), int sx, int sy) : bm(bm), fptr(fptr),sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    std::pair<int,int> p = fptr(std::make_pair(x,y));
    return bm.Map(p.first,p.second);
  }
private:
  Bitmap<T> &bm;
  std::pair<int,int> (*fptr)(std::pair<int,int>);
  int sx,sy;
};




class PointNDim : public NDim<float, Point>
{
public:
  PointNDim(Point p) : p(p) { }
  virtual std::vector<float> Count() const { return std::vector<float>(); }
  virtual Point Map(float *array, int size) const
  {
    if (size != 0) { std::cout << "PointNDIm dimension error!" << std::endl; }
    return p;
  }
  virtual NDim<float,Point> *Interpolate(const NDim<float,Point>& b1, const NDim<float,Point> & b2, float val) const
  {
    const PointNDim *dim1 = dynamic_cast<const PointNDim*>(&b1);
    const PointNDim *dim2 = dynamic_cast<const PointNDim*>(&b2);
    Point p1 = dim1->p;
    Point p2 = dim2->p;
    return new PointNDim(Point::Interpolate(p1,p2,val));
  }
public:
  Point p;
};

template<class I, class B>
class NextNDim : public NDim<I, B>
{
public:
  NextNDim(NDim<I,B> &start, NDim<I,B> &end, I length) : start(start), end(end), length(length), middle_1(0), middle_2(0) { }  
  virtual std::vector<I> Count() const 
  {
    std::vector<I> vec = start.Count();
    vec.push_back(length);
    return vec;
  }
  virtual B Map(I *array, int size) const
  {
    NDim<I,B> *d_middle = start.Interpolate(start, end, array[size-1]);
    B pos = d_middle->Map(array, size-1);
    delete d_middle;
    return pos;
  }
  virtual NDim<I,B> *Interpolate(const NDim<I,B> &b1, const NDim<I,B> &b2, I val) const
  {
    const NextNDim *dim1 = dynamic_cast<const NextNDim<I,B>*>(&b1);
    const NextNDim *dim2 = dynamic_cast<const NextNDim<I,B>*>(&b2);
    delete middle_1;
    delete middle_2;
    middle_1 = dim1->start.Interpolate(dim1->start, dim1->end, val);
    middle_2 = dim2->start.Interpolate(dim2->start, dim2->end, val);
    return new NextNDim(*middle_1, *middle_2, length);
  }
  ~NextNDim() { delete middle_1; delete middle_2; }
private:
  NDim<I,B> &start;
  NDim<I,B> &end;
  I length;
  mutable NDim<I,B> *middle_1;
  mutable NDim<I,B> *middle_2;
};



template<class C>
class BitmapArray : public Array<int, C>
{
public:
  BitmapArray(Bitmap<C> &bm) : bm(bm) { }
  int Size() const { return bm.SizeX()*bm.SizeY(); }
  C Index(int i) const
  {
    int sx = bm.SizeX();
    int y = i / sx;
    int x = i - y*sx;
    return bm.Map(x,y);
  }
private:
  Bitmap<C> &bm;
};

class RectBitmap : public Bitmap<TRect>
{
};

class NormalRectBitmap : public RectBitmap
{
public:
  NormalRectBitmap(Point2d p1, Vector2d u_x, Vector2d u_y,
		   float width, float height, float x_gap, float y_gap,
		   int sx, int sy) : 
    p1(p1), 
    width(width), height(height), x_gap(x_gap), y_gap(y_gap),
    u_x(u_x), u_y(u_y),
    sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual TRect Map(int x, int y) const
  {
    float xx = p1.x + (width+x_gap)*u_x.dx*x+(height+y_gap)*u_y.dx*y;
    float yy = p1.y + (width+x_gap)*u_x.dy*x+(height+y_gap)*u_y.dy*y;
    TRect r;
    r.x = xx;
    r.y = yy;
    r.width = width;
    r.height = height;
    return r;
  }
private:
  Point2d p1;
  float width, height, x_gap, y_gap;
  Vector2d u_x, u_y;
  int sx,sy;
};
class RectCubeFunction : public Function<TRect, CubeCoords>
{
public:
  RectCubeFunction(Point p, Vector u_x, Vector u_y, Vector u_z, float z) : p(p), u_x(u_x), u_y(u_y), u_z(u_z),z(z) { }
  CubeCoords Index(TRect r) const
  {
    CubeCoords c;
    c.p111 = p;
    c.p112 = p+z*u_z;
    c.p121 = p+r.y*u_y;
    c.p122 = p+r.y*u_y+z*u_z;
    c.p211 = p + r.x*u_x;
    c.p212 = p+z*u_z+ r.x*u_x;
    c.p221 = p+r.y*u_y+ r.x*u_x;
    c.p222 = p+r.y*u_y+z*u_z+ r.x*u_x;
    return c;
  }
private:
  Point p;
  Vector u_x, u_y;
  Vector u_z;
  float z;
};


class LighterBitmap : public Bitmap<Color>
{
public:
  LighterBitmap(Bitmap<Color> &orig, Bitmap<float> &light, Point2d pos) : orig(orig), light(light),pos(pos) { }
  virtual int SizeX() const { return orig.SizeX(); }
  virtual int SizeY() const { return orig.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    if (x<pos.x) return orig.Map(x,y);
    if (y<pos.y) return orig.Map(x,y);
    if (x>=pos.x+light.SizeX()) return orig.Map(x,y);
    if (y>=pos.y+light.SizeY()) return orig.Map(x,y);
    return Color::Lighter(orig.Map(x,y), light.Map(x-pos.x, y-pos.y));
  }
private:
  Bitmap<Color> &orig;
  Bitmap<float> &light;
  Point2d pos;
};

class DarkerBitmap : public Bitmap<Color>
{
public:
  DarkerBitmap(Bitmap<Color> &orig, Bitmap<float> &dark, Point2d pos) : orig(orig), dark(dark),pos(pos) { }
  virtual int SizeX() const { return orig.SizeX(); }
  virtual int SizeY() const { return orig.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    if (x<pos.x) return orig.Map(x,y);
    if (y<pos.y) return orig.Map(x,y);
    if (x>=pos.x+dark.SizeX()) return orig.Map(x,y);
    if (y>=pos.y+dark.SizeY()) return orig.Map(x,y);
    return Color::Darker(orig.Map(x,y), dark.Map(x-pos.x, y-pos.y));
  }
private:
  Bitmap<Color> &orig;
  Bitmap<float> &dark;
  Point2d pos;
};


template<class T>
class DynBitmap : public Bitmap<T>
{
public:
  DynBitmap(int sx, int sy) : vec(sx*sy), sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    return vec[x+sx*y];
  }
  const T &At(int x, int y) const
  {
    return vec[x+sx*y];
  }
  T &At(int x, int y)
  {
    return vec[x+sx*y];
  }
  
private:
  std::vector<T> vec;
  int sx, sy;
};


class MemoizeBitmap : public Bitmap<Color>
{
public:
  MemoizeBitmap(Bitmap<Color> &bm) : bm(bm) 
  {
    int width = bm.SizeX();
    int height = bm.SizeY();
    buf = BufferRef::NewBuffer(width, height);
    for(int j=0;j<height;j++)
      for(int i=0;i<width;i++)
	{
	  buf.buffer[i+j*buf.ydelta] = 0xfefefefe;
	}
    
  }
  ~MemoizeBitmap() { BufferRef::FreeBuffer(buf); }
  void MemoizeAll()
  {
    int sx=SizeX();
    int sy=SizeY();
    for(int x=0;x<sx;x++)
      for(int y=0;y<sy;y++)
	{
	  Map(x,y);
	}
  }
  virtual int SizeX() const { return buf.width; }
  virtual int SizeY() const { return buf.height; }
  virtual Color Map(int x, int y) const
  {
    //std::cout << x << " " << y << std::endl;
    if (x<0 || x >= int(buf.width) || y <0 || y>=int(buf.height))
	return Color(0x00000000);
    if (buf.buffer[x+y*buf.ydelta] == 0xfefefefe)
      {
	Color c = bm.Map(x,y);
	unsigned int cc = c.Pixel();
	buf.buffer[x+y*buf.ydelta] = cc;
      }
    return Color(buf.buffer[x+y*buf.ydelta]);
  }

private:
  Bitmap<Color> &bm;
  BufferRef buf;
};

class InterpolateBitmap : public Bitmap<Color>
{
public:
  InterpolateBitmap(Bitmap<Color> &c1, Bitmap<Color> &c2, float xx) :c1(c1), c2(c2),xx(xx) { }
  virtual int SizeX() const { return std::min(c1.SizeX(),c2.SizeX()); }
  virtual int SizeY() const { return std::min(c1.SizeY(),c2.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    Color cc1 = c1.Map(x,y);
    Color cc2 = c2.Map(x,y);
    return Color::Interpolate(cc1,cc2,xx);
  }
  
private:
  Bitmap<Color> &c1;
  Bitmap<Color> &c2;
  float xx;
};

class BitmapNDim : public NDim<float, Color>
{
public:
  BitmapNDim(Bitmap<Color> &bm) : bm(bm), middle(0) { }
  virtual std::vector<float> Count() const { std::vector<float> vec; vec.push_back(float(bm.SizeX())); vec.push_back(float(bm.SizeY())); return vec; }
  virtual Color Map(float *array, int size) const 
  {
    if (size != 2) std::cout << "BitmapNDim error dimensions" << std::endl;
    return bm.Map(int(array[0]), int(array[1]));
  }
  virtual NDim<float,Color> *Interpolate(const NDim<float,Color>& b1, const NDim<float,Color> & b2, float val) const
  {
    const BitmapNDim *dim1 = dynamic_cast<const BitmapNDim*>(&b1);
    const BitmapNDim *dim2 = dynamic_cast<const BitmapNDim*>(&b2);
    Bitmap<Color> &bm1 = dim1->bm;
    Bitmap<Color> &bm2 = dim2->bm;
    delete middle;
    middle = new InterpolateBitmap(bm1, bm2, val);
    return new BitmapNDim(*middle);
  }
  ~BitmapNDim() { delete middle; }
private:
  Bitmap<Color> &bm;
  mutable Bitmap<Color> *middle;
};


class GraphBitmap : public Bitmap<Color>
{
public:
  GraphBitmap(Function<float,float> &f, Color top, Color middle, Color bottom, float up_delta, float down_delta, float start_x, float end_x, float start_y, float end_y, int size_x, int size_y) : f(f), top(top), middle(middle), bottom(bottom), up(up_delta), down(down_delta), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), size_x(size_x), size_y(size_y) { }
  virtual int SizeX() const { return size_x; }
  virtual int SizeY() const { return size_y; }

  Color Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx/=size_x;
    yy/=size_y;
    xx *= (end_x-start_x);
    yy *= (end_y-start_y);
    xx += start_x;
    yy += start_y;
    float val = f.Index(xx);
    if (yy < val-up) return top;
    if (yy < val+down) return middle;
    return bottom;
  }
private:
  Function<float,float> &f;
  Color top, middle, bottom;
  float up, down;
  float start_x, end_x;
  float start_y, end_y;
  int size_x, size_y;
};


class BorderBitmap : public Bitmap<Color>
{
public:
  BorderBitmap(Bitmap<Color> &next, 
	       Color left, Color right, Color top, Color bottom,
	       int left_border, int right_border, int top_border, int bottom_border) : next(next), left(left), right(right), top(top), bottom(bottom),
										       left_border(left_border), right_border(right_border),
										       top_border(top_border), bottom_border(bottom_border) { }
  virtual int SizeX() const { return left_border + next.SizeX() + right_border; }
  virtual int SizeY() const { return top_border + next.SizeY() + bottom_border; }
  virtual Color Map(int x, int y) const
  {
    if (x < left_border) return left;
    if (y < top_border) return top;
    if (x > SizeX()-right_border) return right;
    if (y > SizeY()-bottom_border) return bottom;
    return next.Map(x-left_border, y-top_border);
  }

private:
  Bitmap<Color> &next;
  Color left, right, top, bottom;
  int left_border, right_border, top_border, bottom_border;
};

template<class T>
class ModifyBitmap : public Bitmap<T>
{
public:
  ModifyBitmap(Bitmap<T> &bm, Bitmap<T> &bm2, int x, int y) : bm(bm), bm2(bm2), x(x), y(y) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual T Map(int xx, int yy) const
  {
    if (xx>=x && xx<x+bm2.SizeX())
      if (yy>=y && yy<y+bm2.SizeY())
	{
	  return bm2.Map(xx-x,yy-y);
	}
    return bm.Map(xx,yy);
  }
private:
  Bitmap<T> &bm;
  Bitmap<T> &bm2;
  int x,y;
};

class BlitBitmapClass : public Bitmap<Color>
{
public:
  BlitBitmapClass(Bitmap<Color> &bm, Bitmap<Color> &bm2, int x, int y) : bm(bm), bm2(bm2), x(x), y(y) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int xx, int yy) const
  {
    Color c1 = bm.Map(xx,yy);
    Color c2(0,0,0,0);
    if (xx>=x && xx<x+bm2.SizeX())
      if (yy>=y && yy<y+bm2.SizeY())
	{
	  //std::cout << "Test" << x <<" " << y << ":" << xx << " " << yy << std::endl;
	c2 = bm2.Map(xx-x, yy-y);
	}
    Color res = Color::Interpolate(c1,c2,c2.alpha/255.0);
    //std::cout << res.r << res.b << res.g << res.alpha << std::endl;
    return res;
  } 
private:
  Bitmap<Color> &bm;
  Bitmap<Color> &bm2;
  int x,y;
};

class BlitBitmapClassMasked : public Bitmap<Color>
{
public:
  BlitBitmapClassMasked(Bitmap<Color> &bm, Bitmap<Color> &bm2, int x, int y, Bitmap<float> &mask) : bm(bm), bm2(bm2), x(x), y(y), mask(mask) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int xx, int yy) const
  {
    Color c1 = bm.Map(xx,yy);
    Color c2(0,0,0,0);
    float val = 0.0;
    if (xx>=x && xx<x+bm2.SizeX())
      if (yy>=y && yy<y+bm2.SizeY())
	{
	c2 = bm2.Map(xx-x, yy-y);
	val = mask.Map(xx-x,yy-y);
	}
    return Color::Interpolate(c1,c2,val);
  }  
private:
  Bitmap<Color> &bm;
  Bitmap<Color> &bm2;
  int x,y;
  Bitmap<float> &mask;
};

class BlitBitmapClassMasked2 : public Bitmap<Color>
{
public:
  BlitBitmapClassMasked2(Bitmap<Color> &bm, Bitmap<Color> &bm2, int x, int y, Bitmap<bool> &mask) : bm(bm), bm2(bm2), x(x), y(y), mask(mask) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int xx, int yy) const
  {
    Color c1 = bm.Map(xx,yy);
    Color c2(0,0,0,0);
    bool val = false;
    if (xx>=x && xx<x+bm2.SizeX())
      if (yy>=y && yy<y+bm2.SizeY())
	{
	c2 = bm2.Map(xx-x, yy-y);
	val = mask.Map(xx-x,yy-y);
	}
    if (!val) {
      return c1;
    }
    else
      {
	return c2;
      }
  }  
private:
  Bitmap<Color> &bm;
  Bitmap<Color> &bm2;
  int x,y;
  Bitmap<bool> &mask;
};


void BlitBitmap(BufferRef ref, int x, int y, Bitmap<Color> &color);

template<class T>
class SubBitmap : public Bitmap<T>
{
public:
  SubBitmap(Bitmap<T> &bm, int x, int y, int width, int height) : bm(bm), x(x), y(y), width(width), height(height) { }
  int SizeX() const { return width; }
  int SizeY() const { return height; }
  T Map(int mx, int my) const
  {
    return bm.Map(x+mx, y+my);
  }
  
private:
  Bitmap<T> &bm;
  int x,y,width, height;
};
template<class T>
class GrowBitmap : public Bitmap<T>
{
public:
  GrowBitmap(Bitmap<T> &bm, int l, int t, int r, int b,
	     T bg_color) : bm(bm), l(l), t(t), r(r),b(b),bg_color(bg_color) 
  {
  }
  virtual int SizeX() const { return l+bm.SizeX()+r; }
  virtual int SizeY() const { return t+bm.SizeY()+b; }
  virtual T Map(int x, int y) const
  {
    if (x<l || y<t || x>=l+bm.SizeX()||y>=t+bm.SizeY())
      {
	return bg_color;
      }
      return bm.Map(x-l,y-t);
  }
  
private:
  Bitmap<T> &bm;
  int l,t,r,b;
  T bg_color;
};


template<class T>
class TileBitmap : public SubBitmap<T>
{
public:
  TileBitmap(Bitmap<T> &bm, int tile_sx, int tile_sy, int x, int y) : SubBitmap<T>(bm, tile_sx*x, tile_sy*y, tile_sx, tile_sy) { }
};

template<class T>
class TileBitmap2 : public Function<Pos, Bitmap<T>*>
{
public:
  TileBitmap2(Bitmap<T> &bm, int tile_sx, int tile_sy) : bm(bm), tile_sx(tile_sx), tile_sy(tile_sy) { }
  Bitmap<T> *Index(Pos p) const
  {
    Bitmap<T> *val = new TileBitmap<T>(bm, tile_sx, tile_sy, p.x, p.y);
    vec.push_back(val);
    return val;
  }
  ~TileBitmap2() 
  {
    DeleteAll();
  }
  void DeleteAll()
  {
    int s = vec.size();
    for (int i=0;i<s; i++)
      {
	delete vec[i];
      }
    vec.clear();
  }
private:
  mutable std::vector<Bitmap<T> *> vec;
  Bitmap<T> &bm;
  int tile_sx, tile_sy;
};

class QuadBitmapFromSphere : public Bitmap<Quad>
{
public:
  QuadBitmapFromSphere(Point center, float radius, int sx, int sy) 
    : sx(sx), sy(sy), sphere(sx,sy), mat(sphere, Matrix::Scale(radius,radius,radius)*Matrix::Translate(center.x,center.y,center.z)) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Quad Map(int x, int y) const
  {
    int face = x + y*sx;
    Quad q;
    q.p1 = mat.FacePoint(face, 0);
    q.p2 = mat.FacePoint(face, 1);
    q.p3 = mat.FacePoint(face, 2);
    q.p4 = mat.FacePoint(face, 3);
    return q;
  }

private:
  int sx,sy;
  SphereElem sphere;
  MatrixElem mat;
};

class QuadBitmapFromObjects : public Bitmap<Quad>
{
public:
  QuadBitmapFromObjects(FaceCollection &coll, Array<int,Point> &pos) : coll(coll), pos(pos) { }
  int SizeX() const { return pos.Size(); }
  int SizeY() const { return coll.NumFaces(); }
  Quad Map(int x, int y) const
  {
    Point p = pos.Index(x);
    Quad q;
    q.p1 = p + coll.FacePoint(y,0);
    q.p2 = p + coll.FacePoint(y,1);
    q.p3 = p + coll.FacePoint(y,2);
    q.p4 = p + coll.FacePoint(y,3);
    return q;
  }
private:
  FaceCollection &coll; 
  Array<int,Point> &pos;
};
template<class T>
class BitmapStore : public Bitmap<T>
{
public:
  virtual int SizeX() const { return width; }
  virtual int SizeY() const { return height; }
  virtual T Map(int x, int y) const { return array[x+y*height]; }

  void Set(Bitmap<T> &t)
  {
    width = t.SizeX();
    height = t.SizeY();
    delete array;
    array = new T[width*height];
    for(int y=0;y<height;y++)
      {
	for(int x=0;x<width;x++)
	  {
	    array[x+y*width] = t.Map(x,y);
	  }
      }
  }
  ~BitmapStore() { delete [] array; }
private:
  T *array;
  int width;
  int height;
};



typedef Bitmap<Size> SizeBitmap;

template<class T, class K>
class ConvertBitmap : public Bitmap<K>
{
public:
  ConvertBitmap(Bitmap<T> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual K Map(int x, int y) const
  {
    return bm.Map(x,y); // implicit convesion T->K
  }

private:
  Bitmap<T> &bm;
};
 
class PosBitmap : public Bitmap<Pos>
{
public:
  PosBitmap(int sx, int sy) : sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Pos Map(int x, int y) const
  {
    Pos p;
    p.x = x;
    p.y = y;
    return p;
  }
private:
  int sx,sy;
};

class SubPos : public Function<Pos,Pos>
{
public:
  SubPos(int x, int y) : x(x), y(y) { }
  Pos Index(Pos p) const
  {
    p.x+=x;
    p.y+=y;
    return p;
  }
private:
  int x,y;
};

template<class T>
class BitmapFunction : public Function<Pos,T>
{
public:
  BitmapFunction(Bitmap<T> &bm) : bm(bm) { }
  T Index(Pos p) const
  {
    return bm.Map(p.x,p.y);
  }
private:
  Bitmap<T> &bm;
};

template<class T, class K>
class StructBitmap : public Bitmap<std::pair<T,K> >
{
public:
  StructBitmap(Array<int, T> &a1, Array<int,K> &a2) : a1(a1), a2(a2) { }
  int SizeX() const { return a1.Size(); }
  int SizeY() const { return a2.Size(); }
  std::pair<T,K> Map(int x, int y) const { return std::make_pair(a1.Index(x),a2.Index(y)); }
private:
  Array<int,T> &a1;
  Array<int,K> &a2;
};

class ColumnRowBitmap : public Bitmap<Size>
{
public:
  ColumnRowBitmap(Array<int, int> &column_x, Array<int, int> &row_y) : x(column_x), y(row_y) { }
  int SizeX() const { return x.Size(); }
  int SizeY() const { return y.Size(); }
  Size Map(int xx, int yy) const { Size s; s.sx = x.Index(xx); s.sy = y.Index(yy); return s; }
private:
  Array<int,int> &x;
  Array<int,int> &y;
};

template<class T>
class MBitmap : public Bitmap<T>
{
public:
  MBitmap(int x, int y) : bm(new T[x*y]), sx(x), sy(y) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  T Map(int xx, int yy) const 
  {
    return bm[xx+yy*sx];
  }
  T &Mod(int xx, int yy) { return bm[xx+yy*sx]; }
  MBitmap<T> &operator=(const MBitmap<T> &t) 
  {
    bm=new T[t.sx*t.sy];
    std::copy(t.bm,t.bm+t.sx*t.sy, bm);
    sx=t.sx;
    sy=t.sy;
    return *this;
  }
  MBitmap(const MBitmap<T> &t) : bm(new T[t.sx*t.sy]), sx(t.sx), sy(t.sy)
  {
    std::copy(t.bm,t.bm+sx*sy,bm);
  }
private:
  T *bm;
  int sx,sy;
};

template<class T>
class MFunction : public Function<Size, MBitmap<T> >
{
public:
  MBitmap<T> Index(Size s) const
  {
    MBitmap<T> bm(s.sx,s.sy);
    return bm;
  }
};

template<class T1, class K, class T2 = T1>
class MapBitmap : public Bitmap<K>
{ // T1 => T2 conversion needs to exists
  // TODO: x and y are not passed to f().
public:
  MapBitmap(Bitmap<T1> &bm, Function<T2,K> &f) : bm(bm), f(f) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  K Map(int x, int y) const { return f.Index(bm.Map(x,y)); }
private:
  Bitmap<T1> &bm;
  Function<T2,K> &f;
};

class MapRectBitmap : public MapBitmap<TRect, CubeCoords>
{
public:
  MapRectBitmap(Bitmap<TRect> &bm, Point p, Vector u_x, Vector u_y, Vector u_z, float z) : MapBitmap<TRect, CubeCoords>(bm, func), func(p,u_x,u_y,u_z,z) { }
private:
  RectCubeFunction func;
};
class MapRectArray : public MapArray<int, TRect, CubeCoords>
{
public:
  MapRectArray(Array<int,TRect> &arr, Point p, Vector u_x, Vector u_y, Vector u_z, float z) : MapArray<int,TRect,CubeCoords>(arr,func), func(p,u_x,u_y,u_z,z) { }
  
private:
  RectCubeFunction func;
};



template<class C>
class ConstantBitmap : public Bitmap<C>
{
public:
  ConstantBitmap(C c, int x, int y) : c(c), x(x), y(y) { }
  int SizeX() const { return x; }
  int SizeY() const { return y; }
  C Map(int , int ) const
  {
    return c;
  }
private:
  C c;
  int x,y;
};

template<class C>
class BitmapRelation : public Bitmap<C>
{
public:
  BitmapRelation(Bitmap<C> &bm, C unknown_value, int xx, int yy) : bm(bm),xx(xx), yy(yy), val(bm.Map(xx,yy)), unknown_value(unknown_value) { }
  BitmapRelation(Bitmap<C> &bm, C unknown_value, C value) : bm(bm), val(value), unknown_value(unknown_value) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  C Map(int x, int y) const
  {
    C c = bm.Map(x,y);
    if (val==c)
      {
	return c;
      }
    else
      {
	return unknown_value;
      }
  }
private:
  Bitmap<C> &bm;
  C val;
  C unknown_value;
  int xx,yy;
};

class RandomBitmap : public Bitmap<float>
{
public:
  int SizeX() const { return x; }
  int SizeY() const { return y; }
  float Map(int xx, int yy) const { return bitmap[xx+x*yy]; }
  RandomBitmap(int x, int y, float min, float max) : x(x), y(y), min(min), max(max) 
  {
    bitmap = new float[x*y];
    for(int yy=0;yy<y;yy++)
      {
	for(int xx=0;xx<x;xx++)
	  {
	    bitmap[xx+x*yy] = Rand();
	  }
      }
  }
  ~RandomBitmap() { delete [] bitmap; }
  float Rand()
  {
    float rn = float(r.next());
    rn /= r.maximum();
    rn *= max-min;
    rn += min;
    return rn;
  }
private:
  float *bitmap;
  int x;
  int y;
  float min;
  float max;
  Random r;
};

template<class C>
class RepeatBitmap : public Bitmap<C>
{
public:
  RepeatBitmap(Bitmap<C> &bm, int times_x, int times_y) : bm(bm), times_x(times_x), times_y(times_y) { }
  int SizeX() const { return times_x*bm.SizeX(); }
  int SizeY() const { return times_y*bm.SizeY(); }
  C Map(int x, int y) const
  {
    int xx = x % bm.SizeX();
    int yy = y % bm.SizeY();
    return bm.Map(xx,yy);
  }
private:
  Bitmap<C> &bm;
  int times_x, times_y;
};

template<class C, class T>
class SubstituteBitmapPixels : public Bitmap<C>
{
public:
  // all bitmaps returned by func() must be same size
  SubstituteBitmapPixels(Bitmap<T> &bm, Function<T, Bitmap<C>*> &func, int sx, int sy) : bm(bm), func(func),sx(sx),sy(sy) { }
  virtual int SizeX() const { return bm.SizeX()*sx; }
  virtual int SizeY() const { return bm.SizeY()*sy; }
  virtual C Map(int x, int y) const
  {
    int tx = x/sx;
    int ty = y/sy;
    T pixel = bm.Index(tx,ty);
    int rx = x-tx*sx;
    int ry = y-ty*sy;
    C pixel2 = func->Index(pixel)->Map(rx,ry);
    return pixel2;
  }

private:
  Bitmap<T> &bm;
  Function<T,Bitmap<C>*> &func;
  int sx,sy;
};

template<class C>
class RotateZoomer : public Bitmap<C>
{
public:
  RotateZoomer(Bitmap<C> &bm, int sx, int sy, Point2d p1, Vector2d vx, Vector2d vy, C outside) : bm(bm), sx(sx),sy(sy), p1(p1), vx(vx), vy(vy),outside(outside) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual C Map(int x, int y) const
  {
    Point2d p = p1+vx*(float(x)/sx)+vy*(float(y)/sy);
    if (p.x < 0.0 || p.x >= bm.SizeX()) return outside;
    if (p.y < 0.0 || p.y >= bm.SizeY()) return outside;
    return bm.Map(int(p.x),int(p.y));
  }
  
private:
  Bitmap<C> &bm;
  int sx,sy;
  Point2d p1;
  Vector2d vx;
  Vector2d vy;
  C outside;
};


template<class C>
class CombineBitmapX : public Bitmap<C>
{
public:
  CombineBitmapX(Bitmap<C> &bm, Bitmap<C> &bm2) : bm(bm), bm2(bm2) { }
  int SizeX() const { return bm.SizeX()+bm2.SizeX(); }
  int SizeY() const { return std::min(bm.SizeY(), bm.SizeY()); }
  C Map(int x, int y) const
  {
    if (x<bm.SizeX())
      {
	return bm.Map(x,y);
      }
    else
      {
	return bm2.Map(x-bm.SizeX(), y);
      }
  }  
private:
  Bitmap<C> &bm;
  Bitmap<C> &bm2;
};

class FloatPointBitmap : public Bitmap<float>
{
public:
  FloatPointBitmap(Point p) : p(p) { }
  FloatPointBitmap(Vector v) : p(Point(v)) { }
  int SizeX() const { return 1; }
  int SizeY() const { return 3; }
  float Map(int x, int y) const
  {
    switch(y)
      {
      case 0: return p.x;
      case 1: return p.y;
      case 2: return p.z;
      }
    return 0.0;
  }
private:
  Point p;
};

class RowMultiply : public Bitmap<float>
{
public:
  RowMultiply(Bitmap<float> &bm, int row, float val) : bm(bm), row(row), val(val) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    if (y==row)
      {
	return val*bm.Map(x,y);
      }
    else
      {
	return bm.Map(x,y);
      }
  }

private:
  Bitmap<float> &bm;
  int row;
  float val;
};

class RowAddition : public Bitmap<float>
{
public:
  RowAddition(Bitmap<float> &bm, int row1, int row2, float val_for_row2) : bm(bm), row1(row1), row2(row2), val(val_for_row2) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    if (y==row1)
      {
	return bm.Map(x,row1) + val*bm.Map(x,row2);
      }
    else
      {
	return bm.Map(x,y);
      }
  }
private:
  Bitmap<float> &bm;
  int row1;
  int row2;
  float val;
};

class NodeToOne : public Bitmap<float>
{
public:
  NodeToOne(Bitmap<float> &bm, int x_and_y) : m(bm, x_and_y, 1.0/bm.Map(x_and_y, x_and_y)) { }
  virtual int SizeX() const { return m.SizeX(); }
  virtual int SizeY() const { return m.SizeY(); }
  virtual float Map(int x, int y) const
  {
    return m.Map(x,y);
  }  
private:
  RowMultiply m;
};
class NodeToZero : public Bitmap<float>
{ // assumes bm.Map(x,x) has diagonal as 1.0
public:
  NodeToZero(Bitmap<float> &bm, int x, int y) : add(bm, y, x, -bm.Map(x,y)) { }
  int SizeX() const { return add.SizeX(); }
  int SizeY() const { return add.SizeY(); }
  float Map(int x, int y) const { return add.Map(x,y); }
private:
  RowAddition add;
};
Line PlanePlaneIntersection(const Plane &p1, const Plane &p2);



struct Cube3
{
  float minx,miny,minz;
  float maxx,maxy,maxz;
};

class HeightMapCubesFromBitmap : public Bitmap<Cube3>
{
public:
  HeightMapCubesFromBitmap(Bitmap<float> &bm_min, Bitmap<float> &bm_max, float sx, float sy, float sz) : bm_min(bm_min), bm_max(bm_max) { }
  int SizeX() const { return bm_min.SizeX(); }
  int SizeY() const { return bm_min.SizeY(); }
  Cube3 Map(int x, int y) const
  {
    Cube3 c;
    c.minx = sx*x/SizeX();
    c.maxx = sx*(x+1)/SizeX();
    c.miny = sy*y/SizeY();
    c.maxy = sy*(y+1)/SizeY();
    c.minz = sz*bm_min.Map(x,y);
    c.maxz = sz*bm_max.Map(x,y);
    return c;
  }
private:
  Bitmap<float> &bm_min;
  Bitmap<float> &bm_max;
  float sx,sy,sz;
};

class Cube3Cube : public BoxableFaceCollection
{
public:
  Cube3Cube(Cube3 c) : mat(cube, Matrix::Identity()) { SetCube(c); }
  void SetCube(const Cube3 &c)
  {
    mat.SetMatrix(Matrix::Scale(c.maxx-c.minx, c.maxy-c.miny, c.maxz-c.minz)
		  *Matrix::Translate(c.minx, c.miny, c.minz));
  }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return mat.NumFaces(); }
  virtual int NumPoints(int face) const { return mat.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return mat.FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const
  {
    return mat.PointNormal(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return mat.Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return mat.TexCoord(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return mat.Attrib(face,point,id);
  }  
  virtual int AttribI(int face, int point, int id) const
  {
    return mat.AttribI(face,point,id);
  }  
private:
  CubeElem cube;
  MatrixElem mat;
};

class BitmapCubeArray : public FaceCollectionArray
{
public:
  BitmapCubeArray(Bitmap<Cube3> &cubes) : cubes(cubes),c(Cube3()) { }
  int Size() const { return cubes.SizeX()*cubes.SizeY(); }
  const FaceCollection *Index(int i) const
  {
    int x = i / cubes.SizeY();
    int y = i - x*cubes.SizeY();
    Cube3 cc = cubes.Map(x,y);
    const_cast<Cube3Cube&>(c).SetCube(cc);
    return &c;
  }
private:
  Bitmap<Cube3> &cubes;
  Cube3Cube c;
};

class CubePlane : public BoxableFaceCollection
{
public:
  CubePlane(int x, int y, float sx, float sy, float sz) : bit(x,y, 0.0, sz), bitzero(x,y,0.0,0.0), height(bitzero, bit, sx/x, sy/y, 1.0), cubearray(height), mat(cubearray) { }
  virtual void SetBox(Matrix b) { /*box = b;*/ }
  virtual int NumFaces() const { return mat.NumFaces(); }
  virtual int NumPoints(int face) const { return mat.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return mat.FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const
  {
    return mat.PointNormal(face,point);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return mat.Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return mat.TexCoord(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return mat.Attrib(face,point,id);
  }  
  virtual int AttribI(int face, int point, int id) const
  {
    return mat.AttribI(face,point,id);
  }  

private:
  RandomBitmap bit;
  RandomBitmap bitzero;
  HeightMapCubesFromBitmap height;
  BitmapCubeArray cubearray;
  CompressObject mat;
};


class LayeredBitmap : public Bitmap<int>
{
public:
  LayeredBitmap(Array<int, Bitmap<bool>*> &arr,
		Array<int, int> &arr2) : arr(arr), arr2(arr2) { }

  int SizeX() const { return arr.Index(0)->SizeX(); }
  int SizeY() const { return arr.Index(0)->SizeY(); }
  int Map(int x, int y) const
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	Bitmap<bool> *b = arr.Index(i);
	if (b->Map(x,y)) return arr2.Index(i);
      }
    return 0;
  }
private:
  Array<int, Bitmap<bool>*> &arr;
  Array<int, int> &arr2;
};

template<class K, class T>
class CombineElems
{
public:
  virtual T Exp(const Array<int, K> &arr) const=0;
};

template<class T>
class LayeredBitmap2 : public Bitmap<T>
{
public:
  LayeredBitmap2() { }
  void push_back(Bitmap<T> *t) { vec.push_back(t); }
  void finish_push_backs() 
  {
    pos.clear();
    int s = vec.Size();
    for(int i=0;i<s;i++)
      {
	Point2d p = { 0.0,0.0 };
	pos.push_back(p);
      }
  }
  void set_pos(int i, Point2d pos_p) { pos[i] = pos_p; } 
  void set_combiner(CombineElems<bool,T> *e) { m_e = e; }
  virtual int SizeX() const { return vec[0].SizeX(); }
  virtual int SizeY() const { return vec[0].SizeY(); }
  virtual T Map(int x, int y) const
  {
    VectorArray<T> vec;
    int s = vec.Size();
    for(int i=0;i<s;i++)
      {
	Bitmap<T> *bm = vec.Index(i);
	Point2d ps = pos.Index(i);
	int xx = x + ps.x;
	int yy = y + ps.y;
	vec.push_back(bm->Map(xx,yy));
      }
    return m_e->Exp(vec);
  }
private:
  VectorArray<Bitmap<T> *> vec;
  VectorArray<Point2d> pos;
  CombineElems<bool,T> *m_e;
};

class BitmapCircle : public Bitmap<bool>
{
public:
  BitmapCircle(Point2d center, float radius, int sx, int sy) : center(center), radius(radius), sx(sx), sy(sy) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  bool Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx -= center.x;
    yy -= center.y;
    return xx*xx+yy*yy < radius*radius;
  }
private:
  Point2d center;
  float radius;
  int sx, sy;
};


class PointGrid : public Bitmap<Point>
{
public:
  PointGrid(int x, int y, Point p, Vector u_x, Vector u_y, int sx, int sy) : x(x), y(y), p(p), u_x(u_x), u_y(u_y), sx(sx), sy(sy) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Point Map(int xx, int yy) const
  {
    int dx = xx-x;
    int dy = yy-y;
    return p + u_x*float(dx) + u_y*float(dy);
  }
private:
  int x; 
  int y; 
  Point p; 
  Vector u_x; 
  Vector u_y; 
  int sx; 
  int sy;
};
class UnsignedIntFromBitmap : public Bitmap<unsigned int>
{
public:
  UnsignedIntFromBitmap(Bitmap<Color> &bm) : bm(bm) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  unsigned int Map(int x, int y) const
  {
    return bm.Map(x,y).Pixel();
  }
private:
  Bitmap<Color> &bm;
};
class BitmapFromUnsignedInt : public Bitmap<Color>
{
public:
  BitmapFromUnsignedInt(Bitmap<unsigned int> &bm) : bm(bm) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  Color Map(int x, int y) const
  {
    return bm.Map(x,y);
  }
private:
  Bitmap<unsigned int> &bm;
};


template<class T>
class BitmapFromFile : public Bitmap<T>
{
public:
  BitmapFromFile(std::string filename, Function<char, T> &func, int sizex, int sizey) : sizex(sizex), sizey(sizey), func(func) 
  {
    std::cout << "Reading: " << filename << std::endl;
    std::ifstream file(filename.c_str());
    std::string s;
    while(std::getline(file, s))
      {
	  vec.push_back(s);
      }
  }
  virtual int SizeX() const { return sizex; }
  virtual int SizeY() const { return sizey; }
  T Map(int x, int y) const
  {
    return func.Index(vec[y][x]);
  }
private:
  std::vector<std::string> vec;
  int sizex, sizey;
  Function<char, T> &func;
};
class MapFunction : public Function<char, bool>
{
public:
  bool Index(char c) const
  {
    if (c=='a'||c=='b'||c=='c'||c=='d') return true;
    return false;
  }
};
class ColorMapFunction : public Function<char, unsigned int>
{
public:
  unsigned int Index(char c) const
  {
    switch(c)
      {
      case 'a': return Color(255,255,255).Pixel();
      case 'b': return Color(255,0,0).Pixel();
      case 'c': return Color(0,255,0).Pixel();
      case 'd': return Color(0,0,255).Pixel();
      };
    return Color::White().Pixel();
  }
};



class BitmapFromBuffer : public ColorBitmap
{
public:
  BitmapFromBuffer(BufferRef buf) : buf(buf) { }
  int SizeX() const { return buf.width; }
  int SizeY() const { return buf.height; }
  Color Map(int x, int y) const
  {
    if (x >= 0 && x <int(buf.width))
      if (y >= 0 && y < int(buf.height))
	return Color(buf.buffer[x+y*buf.ydelta]);
    return Color();
  }
  unsigned int &Map2(int x, int y) const
  {
    if (x >= 0 && x <int(buf.width))
      if (y >= 0 && y < int(buf.height))
	return buf.buffer[x+y*buf.ydelta];
    throw "error";
  }


public:
  BufferRef buf;
};

class BufferFromBitmap : public BufferRefReq
{
public:
  BufferFromBitmap(Bitmap<Color> &t) : t(t) 
  {
    buf = BufferRef::NewBuffer(1,1);
    //Gen();
  }
  ~BufferFromBitmap() { delete [] buf.buffer; }
  void Gen() const
  {
    BufferRef::FreeBuffer(buf);
    buf = BufferRef::NewBuffer(t.SizeX(), t.SizeY());
    for(int y=0;y<t.SizeY();y++)
      for(int x=0;x<t.SizeX();x++)
	{
	  int color = t.Map(x,y).Pixel();
	  buf.buffer[x+y*buf.ydelta] = color;
	}
  }
  BufferRef Buffer() const { return buf; }
private:
  Bitmap<Color> &t;
  mutable BufferRef buf;
};


#if 0
class PolygonBitmap : public ContinuousBitmap<float>
{ // this is for piecewise definition of shapes
public:
  PolygonBitmap(int sx, int sy, Point2d p1, Point2d p2, Vector2d v1, Vector2d v2, float val_0, float val_1) : sx(sx),sy(sy), p1(p1), p2(p2), v1(v1), v2(v2), val_0(val_0), val_1(val_1) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual float Map(float x, float y) const
  {
    
  }
private:
  Point2d p1,p2;
  Vector2d v1,v2;
  float val_0, val_1;
};

#endif
template<class C>
class RotateContinuousBitmap : public ContinuousBitmap<C>
{
public:
  RotateContinuousBitmap(ContinuousBitmap<C> *orig, float center_x, float center_y, float angle) : orig(orig), center_x(center_x), center_y(center_y), angle(angle) { }
  virtual float SizeX() const { return orig->SizeX(); }
  virtual float SizeY() const { return orig->SizeY(); }
  virtual C Map(float x, float y) const
  {
    x-=center_x;
    y-=center_y;
    float xx = cos(angle)*x + sin(angle)*y;
    float yy = -sin(angle)*x + cos(angle)*y;
    xx+=center_x;
    yy+=center_y;
    if (xx >= 0.0 && xx<orig->SizeX())
      if (yy >=0.0 && yy<orig->SizeY())
	return orig->Map(xx,yy);
    return C();
  }
private:
  ContinuousBitmap<C> *orig;
  float center_x, center_y;
  float angle;
};

typedef ContinuousBitmap<Color> ContinuousColorBitmap;

template<class T>
class InverseCreateContinuousBitmap : public ContinuousBitmap<T>
{
public:
  InverseCreateContinuousBitmap(Bitmap<T> &bm, std::pair<int,int> (*fptr)(float,float), float sx, float sy) : bm(bm), fptr(fptr),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual T Map(float x, float y) const
  {
    std::pair<int,int> p = fptr(x,y);
    return bm.Map(p.first, p.second);
  }

private:
  Bitmap<T> &bm;
  std::pair<int,int> (*fptr)(float,float);
  float sx,sy;
};

class RandomSamplingBitmap : public Bitmap<Color>
{
public:
  RandomSamplingBitmap(ContinuousBitmap<Color> &c, int count) : c(c), count(count) { }
  int SizeX() const { return c.SizeX()+1; }
  int SizeY() const { return c.SizeY()+1; }
  Color Map(int x, int y) const
  {
    Point2d tl = { float(x),float(y) };
    std::vector<Color> col;
    for(int i=0;i<count;i++)
      {
	float xval = const_cast<RandomSamplingBitmap*>(this)->Rand();
	float yval = const_cast<RandomSamplingBitmap*>(this)->Rand();
	Point2d p = { tl.x+xval, tl.y+yval };
	col.push_back(c.Map(p.x,p.y));
      }
    return Color::AverageArray(&col[0], count);
  }
  float Rand()
  {
    unsigned int val = rand.next();
    float fval = val;
    fval/= float(rand.maximum());
    return fval;
  }
private:
  ContinuousBitmap<Color> &c;
  int count;
  mutable Random rand;
};



template<class T>
class Function2Bitmap : public ContinuousBitmap<T>
{
public:
  Function2Bitmap(float sx, float sy, Function2<float,float,T> &f) : sx(sx), sy(sy), f(f) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual T Map(float x, float y) const { return f.Index(x,y); }
private:
  float sx,sy;
  Function2<float,float,T> &f;
};


template<class T>
class BorderBitmap2 : public ContinuousBitmap<bool>
{ // requires == operation
public:
  BorderBitmap2(float width, ContinuousBitmap<T> &c) : width(width), c(c) { }
  virtual float SizeX() const
  {
    return c.SizeX();
  }
  virtual float SizeY() const
  {
    return c.SizeY();
  }
  virtual bool Map(int x, int y) const
  {
    if (x<width || y<width ||x+width>=c.SizeX() ||y+width>=c.SizeY()) return false;
    T t0 = c.Map(x,y);
    T tx = c.Map(x+width,y);
    T ty = c.Map(x,y+width);
    T tmx = c.Map(x-width,y);
    T tmy = c.Map(x,y-width);
    return !(t0==tx && t0==ty && t0==tmx && t0==tmy);
  }
private:
  float width;
  ContinuousBitmap<T> &c;
};


template<class C>
class ConstantContinuousBitmap : public ContinuousBitmap<C>
{
public:
  ConstantContinuousBitmap(float sx, float sy, C c) : sx(sx), sy(sy), c(c) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual C Map(float x, float y) const { return c; }
private:
  float sx,sy;
  C c;
};

template<class T, class K>
class ContinuousStructBitmap : public ContinuousBitmap<std::pair<T,K> >
{
public:
  ContinuousStructBitmap(Array<float, T> &a1, Array<float,K> &a2) : a1(a1), a2(a2) { }
  float SizeX() const { return a1.Size(); }
  float SizeY() const { return a2.Size(); }
  std::pair<T,K> Map(float x, float y) const { return std::make_pair(a1.Index(x),a2.Index(y)); }
private:
  Array<float,T> &a1;
  Array<float,K> &a2;
};


template<class T1, class K, class T2 = T1>
class MapContinuousBitmap : public ContinuousBitmap<K>
{ // T1 => T2 conversion needs to exists
public:
  MapContinuousBitmap(ContinuousBitmap<T1> &bm, Function<T2,K> &f) : bm(bm), f(f) { }
  float SizeX() const { return bm.SizeX(); }
  float SizeY() const { return bm.SizeY(); }
  K Map(float x, float y) const { return f.Index(bm.Map(x,y)); }
private:
  ContinuousBitmap<T1> &bm;
  Function<T2,K> &f;
};


class SpiralContinuousBitmap : public ContinuousBitmap<Point2d>
{
public:
  SpiralContinuousBitmap(float sx, float sy, Point2d center,
			 Function<float, float> &mymap) : sx(sx), sy(sy), center(center), mymap(mymap) { }
  
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  Point2d Map(float x, float y) const
  {
    x-=center.x;
    y-=center.y;
    float dist = sqrt(x*x+y*y);
    float dist2 = mymap.Index(dist);
    Matrix m = Matrix::ZRotation(dist2);
    Point p(x,y,0.0);
    p = p * m;
    Point2d pp = { p.x, p.y };
    return pp;
  }
private:
  float sx,sy;
  Point2d center;
  Function<float,float> &mymap;
};

class AndNotColorContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  AndNotColorContinuousBitmap(ContinuousBitmap<Color> &bm,
			      ContinuousBitmap<bool> &cut_out) : bm(bm), cut_out(cut_out) { }
  float SizeX() const { return bm.SizeX(); }
  float SizeY() const { return bm.SizeY(); }
  Color Map(float x, float y) const
  {
    Color c1 = bm.Map(x,y);
    bool c2 = cut_out.Map(x,y);
    if (c2) { c1=Color(0,0,0,0); }
    return c1;
  }
  
private:
  ContinuousBitmap<Color> &bm;
  ContinuousBitmap<bool> &cut_out;
};

class AndNotContinuousBitmap : public ContinuousBitmap<bool>
{
public:
  AndNotContinuousBitmap(ContinuousBitmap<bool> &bm,
			 ContinuousBitmap<bool> &cut_out) : bm(bm), cut_out(cut_out) { }
  float SizeX() const { return bm.SizeX(); }
  float SizeY() const { return bm.SizeY(); }
  bool Map(float x, float y) const
  {
    bool c1 = bm.Map(x,y);
    bool c2 = cut_out.Map(x,y);
    if (c2) { c1=false; }
    return c1;
  }
  
private:
  ContinuousBitmap<bool> &bm;
  ContinuousBitmap<bool> &cut_out;
};


class OrContinuousBitmap : public ContinuousBitmap<bool>
{
public:
  template<class It>
  OrContinuousBitmap(It beg, It end) : vec(beg, end) { }
  float SizeX() const
  {
    float s = 0.0;
    int sz = vec.size();
    for(int i=0;i<sz;i++)
      {
	s = std::max(vec[i]->SizeX(), s);
      }
    return s;
  }
  float SizeY() const
  {
    float s = 0.0;
    int sz = vec.size();
    for(int i=0;i<sz;i++)
      {
	s = std::max(vec[i]->SizeY(), s);
      }
    return s;
  }
  bool Map(float x, float y) const
  {
    //std::cout << "OrContinuousBitmap" << x << " " << y << std::endl;
    bool b = false;
    int sz = vec.size();
    for(int i=0;i<sz;i++)
      {
	b |= vec[i]->Map(x,y);
      }
    return b;
  }
private:
  std::vector<ContinuousBitmap<bool> *> vec;
};

class TriangleCoordsContinuousBitmap : public ContinuousBitmap<float>
{
public:
  TriangleCoordsContinuousBitmap(float sx, float sy, const Point2d &p1, const Point2d &p2, const Point2d &p3, float val1, float val2, float val3) : sx(sx), sy(sy), p1(p1), p2(p2), p3(p3), val1(val1), val2(val2), val3(val3) { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  float Map(float x, float y) const
  {
    float p12 = ypos12(y);
    float p13 = ypos13(y);
    float p23 = ypos23(y);
    if (p12>=0.0 && p12<=1.0 && p13>=0.0 && p13<=1.0)
      { // p12 + p13
	float x12 = xpos12(p12);
	float x13 = xpos13(p13);
	float v12 = Line12(p12);
	float v13 = Line13(p13);
	x -= x12;
	x /= x13-x12;
	return v12*(1.0-x)+v13*x;
      }
    else if (p12>=0.0 && p12<=1.0 && p23>=0.0 && p23<=1.0)
      { // p12 + p23
	float x12 = xpos12(p12);
	float x23 = xpos23(p23);
	float v12 = Line12(p12);
	float v23 = Line23(p23);
	x -= x12;
	x /= x23-x12;
	return v12*(1.0-x)+v23*x;
      }
    else if (p13>=0.0 && p13<=1.0 && p23>=0.0 && p23<=1.0)
      { // p13 + p23
	float x13 = xpos13(p13);
	float x23 = xpos23(p23);
	float v13 = Line13(p13);
	float v23 = Line23(p23);
	x -= x13;
	x /= x23-x13;
	return v13*(1.0-x)+v23*x;
      }
    else
      { // outside triangle
	return 0.0;
      }

  }
  // TODO
  float xpos12(float p) const
  {
    return p1.x*(1.0-p)+p2.x*p;
  }
  float xpos13(float p) const
  {
    return p1.x*(1.0-p)+p3.x*p;
  }
  float xpos23(float p) const
  {
    return p2.x*(1.0-p)+p3.x*p;
  }


  float ypos12(float y) const
  {
    y-=p1.y;
    y/=p2.y-p1.y;
    return y;
  }
  float ypos13(float y) const
  {
    y-=p1.y;
    y/=p3.y-p1.y;
    return y;
  }
  float ypos23(float y) const
  {
    y-=p2.y;
    y/=p3.y-p2.y;
    return y;
  }

  
  float Line12(float x) const
  {
    return val1*(1.0-x) + val2*x;
  }
  float Line13(float x) const
  {
    return val1*(1.0-x) + val3*x;
  }
  float Line23(float x) const
  {
    return val2*(1.0-x) + val3*x;
  }
private:
  float sx,sy;
  const Point2d &p1, &p2, &p3;
  float val1, val2, val3;
};


class TriangleContinuousBitmap : public ContinuousBitmap<bool>
{
public:
  TriangleContinuousBitmap(float sx, float sy, const Point2d &p1, const Point2d &p2, const Point2d &p3) : sx(sx),sy(sy), p1(p1), p2(p2), p3(p3) { }
  float SizeX() const
  {
    return sx;
  }
  float SizeY() const
  {
    return sy;
  }
  
  bool Map(float x, float y) const
  {
    bool a1 = Line(p3.x,p3.y,p1,p2);
    bool a2 = Line(p2.x,p2.y,p1,p3);
    bool a3 = Line(p1.x,p1.y,p2,p3);
    bool b1 = Line(x,y,p1,p2);
    bool b2 = Line(x,y,p1,p3);
    bool b3 = Line(x,y,p2,p3);
    if (!a1) { b1 = !b1; }
    if (!a2) { b2 = !b2; }
    if (!a3) { b3 = !b3; }

    return b1&&b2&&b3;
  }

  bool Line(float x, float y, const Point2d &a, const Point2d &b) const
  {
    Point2d r = { x, y };
    float k = (r.x-a.x) / (b.x-a.x);
    return r.y < a.y + k*(b.y-a.y);
  }
  
private:
  float sx,sy;
  const Point2d &p1;
  const Point2d &p2;
  const Point2d &p3;
};

class QuadContinuousBitmap : public ContinuousBitmap<bool>
{
public:
  QuadContinuousBitmap(float sx, float sy, Point2d p1, Point2d p2, Point2d p3, Point2d p4) : tri1(sx,sy, p1,p2,p3), tri2(sx,sy,p1,p3,p4) { }
  virtual float SizeX() const { return tri1.SizeX(); }
  virtual float SizeY() const { return tri1.SizeY(); }
  virtual bool Map(float x, float y) const
  {
    return tri1.Map(x,y) || tri2.Map(x,y);
  }
private:
  TriangleContinuousBitmap tri1;
  TriangleContinuousBitmap tri2;
};

class ColorTriangle : public ContinuousBitmap<Color>
{
public:
  ColorTriangle(float sx, float sy, const Point2d &p1, const Point2d &p2, const Point2d &p3, const Color &c, const Color &c2) : tri(sx,sy,p1,p2,p3), c(c), c2(c2) { }
  float SizeX() const { return tri.SizeX(); }
  float SizeY() const { return tri.SizeY(); }
  Color Map(float x, float y) const
  {
    if (tri.Map(x,y)) { return c; }
    return c2;
  }
private:
  TriangleContinuousBitmap tri;
  const Color &c;
  const Color &c2;
};
class ColorTriangle2 : public ContinuousBitmap<Color>
{
public:
  ColorTriangle2(float sx, float sy, 
		 const Point2d &p1, const Point2d &p2, const Point2d &p3, 
		 const Color &c1, const Color &c2, const Color &c3, const Color &background) 
    : sx(sx), sy(sy),
      tri(sx,sy,p1,p2,p3),
      r(sx,sy,p1,p2,p3,c1.r,c2.r,c3.r),
      g(sx,sy,p1,p2,p3,c1.g,c2.g,c3.g),
      b(sx,sy,p1,p2,p3,c1.b,c2.b,c3.b),
      a(sx,sy,p1,p2,p3,c1.alpha,c2.alpha,c3.alpha),
      background(background)
  { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  Color Map(float x, float y) const
  {
    if (!tri.Map(x,y)) return background;
    float rr = r.Map(x,y);
    float gg = g.Map(x,y);
    float bb = b.Map(x,y);
    float aa = a.Map(x,y);
    return Color(rr,gg,bb,aa);
  }
private:
  float sx,sy;
  TriangleContinuousBitmap tri;
  TriangleCoordsContinuousBitmap r,g,b,a;
  const Color &background;
};

class GradientBitmap : public ContinuousBitmap<Color>
{
public:
  GradientBitmap(float sx, float sy, Point2d p1, Point2d p2, Color c1, Color c2) : sx(sx), sy(sy), p1(p1), p2(p2), c1(c1), c2(c2) { }

  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  Color Map(float x, float y) const;

private:
  float sx,sy;
  Point2d p1,p2;
  Color c1,c2;
};

class ContinuousBitmapArray : public ContinuousBitmap<Color>
{
public:
  ContinuousBitmapArray(float sx, float sy, Array<int, ContinuousBitmap<Color>*> &arr) : sx(sx), sy(sy), arr(arr) { }
  float SizeX() const 
  {
    return sx;
  }
  float SizeY() const
  {
    return sy;
  }
  Color Map(float x, float y) const
  {
    int size = arr.Size();
    Color curr(255,255,255,255);
    for(int i=0;i<size;i++)
      {
	Color c = arr.Index(i)->Map(x,y);
	curr = Color::Interpolate(curr,c,c.alpha);
      }
    return curr;
  }
private:
  float sx,sy;
  Array<int, ContinuousBitmap<Color> *> &arr;
};

class TexturedTriangle : public ContinuousBitmap<Color>
{
public:
  TexturedTriangle(float sx, float sy, const Point2d &p1, const Point2d &p2, const Point2d &p3,
		   ContinuousBitmap<Color> &texture,
		   const Point2d &t1, const Point2d &t2, const Point2d &t3)
    : sx(sx), sy(sy), tri(sx,sy, p1,p2,p3), tx(sx,sy,p1,p2,p3,t1.x,t2.x,t3.x),
      ty(sx,sy,p1,p2,p3,t1.y,t2.y,t3.y), texture(texture)
  {
  }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  Color Map(float x, float y) const
  {
    if (!tri.Map(x,y)) { Color c(0,0,0,0); return c; }
    float ttx = tx.Map(x,y);
    float tty = ty.Map(x,y);
    return texture.Map(ttx,tty);
  }

private:
  float sx,sy;
  TriangleContinuousBitmap tri;
  TriangleCoordsContinuousBitmap tx,ty;
  ContinuousBitmap<Color> &texture;
};

class CircleContinuousBitmap : public ContinuousBitmap<bool>
{
public:
  CircleContinuousBitmap(float size_x, float size_y, float center_x, float center_y, float radius) : size_x(size_x), size_y(size_y), center_x(center_x), center_y(center_y), radius(radius) { }
  float SizeX() const
  {
    return size_x;
  }
  float SizeY() const
  {
    return size_y;
  }
  bool Map(float x, float y) const
  {
    x-= center_x;
    y-= center_y;
    return x*x+y*y < radius*radius;
  }
private:
  float size_x, size_y;
  float center_x, center_y, radius;
};

class SampleBitmap : public Bitmap<Color>
{
public:
  SampleBitmap(ContinuousBitmap<Color> &c, int sx, int sy) : c(c),sx(sx), sy(sy) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    float area_x = c.SizeX()/sx;
    float area_y = c.SizeY()/sy;
    float left = x*area_x;
    float top = y*area_y;
    left += 0.5*area_x;
    top += 0.5*area_y;
    Color c_c = c.Map(left,top);
    Color c_l = c.Map(left-0.25*area_x, top);
    Color c_r = c.Map(left+0.25*area_x, top);
    Color c_t = c.Map(left, top-0.25*area_y);
    Color c_b = c.Map(left, top+0.25*area_y);
    c_c *= 0.5;
    c_l *= 0.25/2;
    c_r *= 0.25/2;
    c_t *= 0.25/2;
    c_b *= 0.25/2;
    Color cc = c_c + c_l + c_r + c_t + c_b;
    return cc;
  }
private:
  ContinuousBitmap<Color> &c;
  int sx,sy;
};

template<class T>
class ContinuousBitmapFromBitmap : public ContinuousBitmap<T>
{
public:
  ContinuousBitmapFromBitmap(Bitmap<T> &bitmap, float xsize, float ysize) : bitmap(bitmap), xsize(xsize), ysize(ysize) { }
  float SizeX() const { return xsize; }
  float SizeY() const { return ysize; }
  T Map(float x, float y) const
  {
    x/=xsize;
    y/=ysize;
    x*=bitmap.SizeX();
    y*=bitmap.SizeY();
    return bitmap.Map(x,y);
  }
private:
  Bitmap<T> &bitmap;
  float xsize;
  float ysize;
};
template<class T>
class BitmapFromContinuousBitmap : public Bitmap<T>
{
public:
  BitmapFromContinuousBitmap(ContinuousBitmap<T> &bitmap, int xsize, int ysize) : bitmap(bitmap), xsize(xsize), ysize(ysize) { }
  int SizeX() const { return xsize; }
  int SizeY() const { return ysize; }
  T Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx /= xsize;
    yy /= ysize;
    xx *= bitmap.SizeX();
    yy *= bitmap.SizeY();
    return bitmap.Map(xx,yy);
  }
private:
  ContinuousBitmap<T> &bitmap;
  int xsize;
  int ysize;
};

typedef Voxel<Quad> QuadVoxel;

template<class C>
class VoxelNDim : public NDim<int, C>
{
public:
  VoxelNDim(Voxel<C> &bm) : bm(bm) { }
  virtual std::vector<int> Count() const { std::vector<int> vec; vec.push_back(bm.SizeX()); vec.push_back(bm.SizeY()); vec.push_back(bm.SizeZ()); return vec; }
  virtual C Map(int *array, int size) const 
  {
    if (size != 3) std::cout << "VoxelNDim error dimensions" << std::endl;
    return bm.Map(array[0], array[1], array[2]);
  }
private:
  Voxel<C> &bm;
};




template<class T1, class K, class T2 = T1>
class MapVoxel : public Voxel<K>
{ // T1 => T2 conversion needs to exists
public:
  MapVoxel(Voxel<T1> &bm, Function<T2,K> &f) : bm(bm), f(f) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  int SizeZ() const { return bm.SizeZ(); }
  K Map(int x, int y, int z) const { return f.Index(bm.Map(x,y,z)); }
private:
  Voxel<T1> &bm;
  Function<T2,K> &f;
};


class QuadVoxelFromObjects : public Voxel<Quad>
{ // FaceCollection must use quads
public:
  QuadVoxelFromObjects(FaceCollection &coll, Bitmap<Point> &pos) : coll(coll), pos(pos) { }
  int SizeX() const { return pos.SizeX(); }
  int SizeY() const { return pos.SizeY(); }
  int SizeZ() const { return coll.NumFaces(); }
  Quad Map(int x, int y, int z) const
  {
    Quad q;
    Point p = pos.Map(x,y);
    q.p1 = p + coll.FacePoint(z, 0);
    q.p2 = p + coll.FacePoint(z, 1);
    q.p3 = p + coll.FacePoint(z, 2);
    q.p4 = p + coll.FacePoint(z, 3);
    return q;
  }
private:
  FaceCollection &coll;
  Bitmap<Point> &pos;
};
class QuadArrayElem : public BoxableFaceCollection
{
public:
  QuadArrayElem(Array<int, Quad> &array) : voxel(array) { }

  virtual int NumFaces() const { return voxel.Size(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    Quad q = get(face);
    if (point == 0) return q.p1;
    if (point == 1) return q.p2;
    if (point == 2) return q.p3;
    return q.p4;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Quad q = get(face);
    Point p1 = q.p1;
    Point p2 = q.p2;
    Point p3 = q.p3;
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return Color::White().Pixel(); }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Quad get(int face) const
  { 
    return voxel.Index(face);
  }
private:
  Array<int,Quad> &voxel;

};

class QuadBitmapElem : public BoxableFaceCollection
{
public:
  QuadBitmapElem(Bitmap<Quad> &voxel) : voxel(voxel) { }

  virtual int NumFaces() const { return voxel.SizeX()*voxel.SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    Quad q = get(face);
    if (point == 0) return q.p1;
    if (point == 1) return q.p2;
    if (point == 2) return q.p3;
    return q.p4;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Quad q = get(face);
    Point p1 = q.p1;
    Point p2 = q.p2;
    Point p3 = q.p3;
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return Color::White().Pixel(); }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Quad get(int face) const
  { 
    int x = face / (voxel.SizeX());
    int ddx = face - x*(voxel.SizeX());
    int y = ddx;
    Quad q = voxel.Map(x,y);
    return q;
  }
private:
  Bitmap<Quad> &voxel;
};


class QuadVoxelElem : public BoxableFaceCollection
{
public:
  QuadVoxelElem(Voxel<Quad> &voxel) : voxel(voxel) { }

  virtual int NumFaces() const { return voxel.SizeX()*voxel.SizeY()*voxel.SizeZ(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    Quad q = get(face);
    if (point == 0) return q.p1;
    if (point == 1) return q.p2;
    if (point == 2) return q.p3;
    return q.p4;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Quad q = get(face);
    Point p1 = q.p1;
    Point p2 = q.p2;
    Point p3 = q.p3;
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return Color::White().Pixel(); }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Quad get(int face) const 
  { 
    int z = face / (voxel.SizeX()*voxel.SizeY());
    int ddz = face - z*(voxel.SizeX()*voxel.SizeY());
    int y = ddz / voxel.SizeX();
    int ddy = ddz - y*voxel.SizeX();
    int x = ddy;
    Quad q = voxel.Map(x,y,z);
    return q;
  }
private:
  Voxel<Quad> &voxel;
};


template<class X, class Y>
class StructVoxel : public Voxel<std::pair<X,Y> >
{
public:
  StructVoxel(Bitmap<X> &bm, Array<int, Y> &a) : bm(bm), a(a) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual int SizeZ() const { return a.Size(); }
  virtual std::pair<X,Y> Map(int x, int y, int z) const
  {
    X xx = bm.Index(x,y);
    Y yy = a.Index(z);
    return std::make_pair(xx,yy);
  }
private:
  Bitmap<X> &bm;
  Array<int, Y> &a;
};

class CubeVoxel : public Voxel<Cube3>
{
public:
  CubeVoxel(Cube3 p, int x, int y, int z) : p(p), x(x), y(y), z(z) { }
  int SizeX() const { return x; }
  int SizeY() const { return y; }
  int SizeZ() const { return z; }
  Cube3 Map(int xx, int yy, int zz) const
  {
    float sx = p.maxx-p.minx;
    float sy = p.maxy-p.miny;
    float sz = p.maxz-p.minz;
    sx/=x;
    sy/=y;
    sz/=z;
    float px = xx*sx + p.minx;
    float py = yy*sy + p.miny;
    float pz = zz*sz + p.minz;
    Cube3 c;
    c.minx = px;
    c.miny = py;
    c.minz = pz;
    c.maxx = px+sx;
    c.maxy = py+sy;
    c.maxz = pz+sz;
    return c;
  }
private:
  Cube3 p;
  int x,y,z;
};

Cube3 CubeVoxelArea(Voxel<Cube3> &cubes, PointI p1, PointI p2);



class RayTracingFunction0 : public Function<float, Point>
{
public:
  RayTracingFunction0(Point p1, Vector v) : p1(p1), v(v) { }
  Point Index(float x) const
  {
    return p1+x*v;
  }
private:
  Point p1;
  Vector v;
};

class RayTracingFunction1 : public Function<float,float>
{
public:
  RayTracingFunction1(Function<float,Point> &f, ContinuousVoxel<float> &objs) : f(f), objs(objs) { }
  float Index(float x) const
  {
    Point p = f.Index(x);
    return objs.Map(p.x,p.y,p.z);
  }
private:
  Function<float, Point> &f;
  ContinuousVoxel<float> &objs;
};

class RayTracingFunction : public Function<Point2d, Point>
{
public:
  RayTracingFunction(ContinuousVoxel<float> &objs,
		     Vector v, float z) : objs(objs), v(v),z(z) { }
  Point Index(Point2d p) const
  {
    Point p1(p.x,p.y,z);
    Vector v(0.0,0.0,-2.0*z);
    return RayTrace(p1,v);
  }

  Point RayTrace(Point p, Vector v) const
  {
    RayTracingFunction0 func0(p,v);
    RayTracingFunction1 func1(func0, objs);
    float val = Solve(func1, 0.0, 1.0);
    Point pp = func0.Index(val);
    return pp;
  }

private:
  ContinuousVoxel<float> &objs;
  Vector v;
  float z;
};

class RayTrace
{
public:
  RayTrace(ContinuousVoxel<float> &objs) : objs(objs) { }
  Point RayTraceFunc(Point p, Vector v)
  {
    RayTracingFunction0 func0(p,v);
    RayTracingFunction1 func1(func0, objs);
    float val = Solve(func1, 0.0, 1.0);
    Point pp = func0.Index(val);
    return pp;
  }
private:
  ContinuousVoxel<float> &objs;
};

class RayTracingBitmap : public Bitmap<Color>
{
public:
  RayTracingBitmap(Function<Point2d,Point> &raytrace, // use RayTracingFunction
		   ContinuousVoxel<Color> &color,
		   int sx, int sy) : raytrace(raytrace), color(color), sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    Point2d p = { float(x), float(y) };
    Point pp = raytrace.Index(p);
    return color.Map(pp.x,pp.y,pp.z);
  }
private:
  Function<Point2d, Point> &raytrace;
  ContinuousVoxel<Color> &color;
  int sx,sy;
};


template<class T>
class Maybe
{
public:
  Maybe() : b(false), obj(NULL) { }
  Maybe(const T &t) : b(true) { obj = new T(t); }
  Maybe(const Maybe<T> &m) : b(m.b) { obj = new T(*m.obj); }
  void operator=(const Maybe<T> &m)
  {
    b = m.b;
    obj = new T(*m.obj);
  }
  ~Maybe() { delete obj; }
  bool b;
  T *obj;
};
template<class C>
class MaybeVoxel : public ContinuousVoxel<Maybe<C> >
{
public:
  MaybeVoxel(VolumeObject &obj, ContinuousVoxel<C> &data) : obj(obj), data(data) { }
  virtual float SizeX() const { return data.SizeX(); }
  virtual float SizeY() const { return data.SizeY(); }
  virtual float SizeZ() const { return data.SizeZ(); }
  virtual Maybe<C> Map(float x, float y, float z) const
  {
    if (obj.Inside(Point(x,y,z))) return data.Map(x,y,z);
    return Maybe<C>();
  }
private:
  VolumeObject &obj;
  ContinuousVoxel<C> &data;
};
template<class C>
class JustVoxel : public ContinuousVoxel<C>
{ 
public:
  JustVoxel(ContinuousVoxel<Maybe<C> > &vox, C def) : vox(vox), def(def) { }
  virtual float SizeX() const { return vox.SizeX(); }
  virtual float SizeY() const { return vox.SizeY(); }
  virtual float SizeZ() const { return vox.SizeZ(); }
  virtual C Map(float x, float y, float z) const
  {
    Maybe<C> val = vox.Map(x,y,z);
    if (val.b) return def;
    return *val.obj;
  }

private:
  ContinuousVoxel<Maybe<C > > &vox;
  C def;
};


class SphereNormalContinuousVoxel : public ContinuousVoxel<Vector>
{
public:
  SphereNormalContinuousVoxel(Point center) : center(center) { }
  virtual float SizeX() const { return 1.0; }
  virtual float SizeY() const { return 1.0; }
  virtual float SizeZ() const { return 1.0; }
  virtual Vector Map(float x, float y, float z) const
  {
    Point v(x,y,z);
    v-=center;
    return Vector(v);
  }  
private:
  Point center;
};

class ColorCube : public ContinuousVoxel<Color>
{
public:
  virtual float SizeX() const { return 1.0; }
  virtual float SizeY() const { return 1.0; }
  virtual float SizeZ() const { return 1.0; }
  Color Map(float x, float y, float z) const
  {
    Color c;
    c.r = x*255;
    c.g = y*255;
    c.b = z*255;
    return c;
  }
};


class PosContinuousVoxel : public ContinuousVoxel<Point>
{
public:
  PosContinuousVoxel(float sx, float sy, float sz) : sx(sx), sy(sy), sz(sz) { }

  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual float SizeZ() const { return sz; }
  virtual Point Map(float x, float y, float z) const
  {
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
  }
private:
  float sx,sy,sz;
};

template<class T>
class PlaneBitmap : public ContinuousBitmap<T>
{
public:
  PlaneBitmap(ContinuousVoxel<T> &voxel, Plane pl, float sx, float sy) : voxel(voxel), pl(pl), sx(sx), sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual T Map(float x, float y) const
  {
    Point2d p2d = { x,y };
    PlaneIn3d plane(p2d, pl);
    Point p3d = plane.Elem();
    return voxel.Map(p3d.x, p3d.y, p3d.z);
  }
private:
  ContinuousVoxel<T> &voxel;
  Plane pl;
  float sx,sy;
};

class MatrixContinuousVoxel : public ContinuousVoxel<Point>
{
public:
  MatrixContinuousVoxel(ContinuousVoxel<Point> &p, Matrix m) : m(m),p(p) { }
  virtual float SizeX() const { return p.SizeX(); }
  virtual float SizeY() const { return p.SizeY(); }
  virtual float SizeZ() const { return p.SizeZ(); }
  virtual Point Map(float x, float y, float z) const
  {
    Point pp = p.Map(x,y,z);
    return pp * m;
  }
private:
  Matrix m;
  ContinuousVoxel<Point> &p;
};

template<class T>
class SampleVoxel : public Voxel<T>
{
public:
  SampleVoxel(ContinuousVoxel<T> &v, int x, int y, int z) : v(v), x(x), y(y), z(z) { }
  int SizeX() const { return x; }
  int SizeY() const { return y; }
  int SizeZ() const { return z; }
  T Map(int xx, int yy, int zz) const
  {
    float xa = Sample(xx, x, v.SizeX());
    float ya = Sample(yy, y, v.SizeY());
    float za = Sample(zz, z, v.SizeZ());
    return v.Map(xa,ya,za);
  }
  float Sample(int xx, int xsize, float sizex) const
  {
    return xx*sizex/xsize;
  }
private:
  ContinuousVoxel<T> &v;
  int x,y,z;
};

template<class T>
class VoxelToContinuousVoxel : public ContinuousVoxel<T>
{
public:
  VoxelToContinuousVoxel(Voxel<T> &vx) : vx(vx) { }
  int SizeX() const { return vx.SizeX(); }
  int SizeY() const { return vx.SizeY(); }
  int SizeZ() const { return vx.SizeZ(); }
  T Map(int xx, int yy, int zz) const
  {
    return vx.Map(xx,yy,zz);
  }
  
private:
  Voxel<T> &vx;
};

class VolumeVoxel : public ContinuousVoxel<bool>
{
public:
  VolumeVoxel(VolumeObject &vol, Range<float> x, Range<float> y, Range<float> z) : vol(vol), x(x), y(y), z(z) { }
  float SizeX() const { return x.end-x.start; }
  float SizeY() const { return y.end-y.start; }
  float SizeZ() const { return z.end-z.start; }
  bool Map(float xx, float yy, float zz) const
  {
    xx+=x.start;
    yy+=y.start;
    zz+=z.start;
    return vol.Inside(Point(xx,yy,zz));
  }
private:
  VolumeObject &vol;
  Range<float> x;
  Range<float> y;
  Range<float> z;
};

template<class T>
class SphereCoords
{
public:
  // r = [0..radius]
  // alfa = [0..2*pi]
  // beta = [0..pi]
  virtual float radius() const=0;
  virtual T Map(float r, float alfa, float beta) const=0;
};

template<class T>
class VoxelToSphere : public SphereCoords<T>
{
public:
  VoxelToSphere(ContinuousVoxel<T> &sp, Point p, float radius) : sp(sp), p(p), m_r(radius) { }
  float radius() const
  {
    return m_r;
  }
  T Map(float r, float alfa, float beta) const
  {
    SphericalPoint spec(Point(0.0,0.0,0.0));
    spec.r = r;
    spec.alfa = alfa;
    spec.beta = beta;
    Point pp = spec.ToPoint();
    pp += p;
    return sp.Map(pp.x,pp.y,pp.z);
  }
private:
  ContinuousVoxel<T> &sp;
  Point p;
  float m_r;
};


template<class T, class K>
class GraphBool
{
public:
  static bool Compare(T t, K k)
  {
    return t == k;
  }
};

template<>
class GraphBool<float, float>
{
public:
  static bool Compare(float x, float y)
  {
    return x-y < 0.01;
  }
};
template<>
class GraphBool<std::pair<float, float>, std::pair<float,float> >
{
public:
  static bool Compare(std::pair<float, float> p1, std::pair<float,float> p2)
  {
    return p1.first - p2.first < 0.01 && p1.second - p2.second < 0.01;
  }
};


template<class T, class K>
class Graph2 : public Function<std::pair<T,K>, bool>
{
public:
  Graph2(const Function<T,K> &func) : func(func) { }
  bool Index(std::pair<T,K> i) const
  {
    return GraphBool<T,K>::Compare(func.Index(i.first), i.second);
  }
private:
  const Function<T,K> &func;
};

template<class T, class K>
class TwoGraphs : public Array<T,Range<K> >
{
public:
  TwoGraphs(Array<T,K> &f, Array<T,K> &f2) : pair(f,f2) { }
  T Size() const { return pair.Size(); }
  Range<K> Index(T t) const
  {
    std::pair<K,K> p = pair.Index(t);
    Range<K> r;
    r.start = p.first;
    r.end = p.second;
    return r;
  }
private:
  ArrayPair<T,K,K> pair;
};



template<class T>
class ColorizeBoolArray : public Array<T,Color>
{
public:
  ColorizeBoolArray(const Array<T,bool> &func) : func(func) { }
  T Size() const { return func.Size(); }
  Color Index(T t) const
  {
    bool b = func.Index(t);
    if (b) return Color(255,255,255,128);
    else return Color(0,0,0, 128);
  }
private:
  const Array<T,bool> &func;
};


class BitmapizeArray : public ColorBitmap
{
public:
  BitmapizeArray(Array<std::pair<int,int>,Color> &func) : func(func) { }
  int SizeX() const
  {
    return func.Size().first;
  }
  int SizeY() const
  {
    return func.Size().second;
  }
  Color Map(int x, int y) const
  {
    return func.Index(std::make_pair(x,y));
  }
private:
  Array<std::pair<int,int>, Color> &func;
};

class BitmapCache
{
public:
  BitmapCache(ColorBitmap &bitmap) : bitmap(bitmap) 
  {
    ref = BufferRef::NewBuffer(1,1);
  }
  void Generate(Rect r)
  {
    BufferRef::FreeBuffer(ref);
    int sizex = bitmap.SizeX();
    int sizey = bitmap.SizeY();
    ref = BufferRef::NewBuffer(sizex, sizey);
    if (r.tl.x > r.br.x) std::swap(r.tl.x,r.br.x);
    if (r.tl.y > r.br.y) std::swap(r.tl.y,r.br.y);
    if (r.br.x > sizex) r.br.x = sizex;
    if (r.br.y > sizey) r.br.y = sizey;
    if (r.tl.x < 0.0) r.tl.x = 0.0;
    if (r.tl.y < 0.0) r.tl.y = 0.0;
    //std::cout << r.tl.x << " " << r.br.x << " " << r.tl.y << " " << r.br.y << std::endl;
    for(int y=r.tl.y;y<r.br.y;y++)
      for(int x=r.tl.x;x<r.br.x;x++)
	{
	  Color c = bitmap.Map(x,y);
	  ref.buffer[x+ref.ydelta*y] = c.Pixel();
	}    
  }
  BufferRef Ref() const { return ref; }
  ~BitmapCache() { BufferRef::FreeBuffer(ref); }
private:
  BufferRef ref;
  ColorBitmap &bitmap;
};

class TriangleHandleValue : public HandleValue<Triangle>, public BoxableFaceCollection
{
public:
  void Handle(Triangle t)
  {
    vec.push_back(t);
  }
  void SetBox(Matrix m) { }
  int NumFaces() const { return vec.size(); }
  int NumPoints(int face) const { return 3; }
  Point FacePoint(int face, int point) const 
  { 
    return vec[face].Index(point);
  }
  unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0;
    p.y = 0;
    return p;
  }

  Vector PointNormal(int face, int point) const
  {
    Point p = FacePoint(face, 0);
    Point px = FacePoint(face, 1);
    Point py = FacePoint(face, 2);
    return -Vector::CrossProduct(px-p, py-p);
  }
  float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  int AttribI(int face, int point, int id) const
  {
    return 0.0;
  }
private:
  std::vector<Triangle> vec;
};


class Cube3HandleValue : public HandleValue<Cube3>, public Element<const BoxableFaceCollection*>
{
public:
  Cube3HandleValue() { }
  void Handle(Cube3 t)
  {
    CubeElem *cube = new CubeElem;
    cube->SetBox(Matrix::Identity());
    Matrix m = Matrix::Scale((t.maxx-t.minx)/2, (t.maxy-t.miny)/2, (t.maxz-t.minz)/2);
    Matrix m2 = m * Matrix::Translate(-t.minx, -t.miny, -t.minz);
    MatrixElem *matrix = new MatrixElem(*cube, m2);
    or_elem.push_back(matrix);
  }
  void update_faces_cache() { or_elem.update_faces_cache(); }
  const BoxableFaceCollection *Elem() const { return &or_elem; }
private:
  OrElem<BoxableFaceCollection> or_elem;
};
template<class T>
class EnumerateBitmap
{
public:
  EnumerateBitmap(Bitmap<T> &bm, HandleValue<T> &hv) : bm(bm), hv(hv) { }
  void Gen()
  {
    int sizey = bm.SizeY();
    int sizex = bm.SizeX();
    for(int y=0;y<sizey;y++)
      {
	for(int x=0;x<sizex;x++)
	  {
	    hv.Handle(bm.Map(x,y));
	  }
      }
  }
private:
  Bitmap<T> &bm;
  HandleValue<T> &hv;    
};

class GenerateSequence
{
public:
  GenerateSequence(HandleValue<int> &gen, int size) : gen(gen), size(size) { }
  void Gen()
  {
    for(int i=0;i<size;i++)
      {
	gen.Handle(i);
      }
  }
private:
  HandleValue<int> &gen;
  int size;
};

template<class T>
class GenerateArray : public HandleValue<T>, public VectorArray<T>
{
public:
  void Init() { VectorArray<T>::clear(); }
  void Handle(T t)
  {
    this->push_back(t);
  }
};

class GenerateGrid
{
public:
  GenerateGrid(HandleValue<std::pair<int,int> > &gen, int sizex, int sizey) : gen(gen), sizex(sizex), sizey(sizey) { }
  void Gen() 
  {
    for(int y=0;y<sizey;y++)
      for(int x=0;x<sizex;x++)
	gen.Handle(std::make_pair(x,y));
  }
private:
  HandleValue<std::pair<int,int> > &gen;
  int sizex, sizey;
};

struct Cube2
{
  int x;
  int y;
  int z;
};

class Cube2Cube3Function : public Function<Cube2, Cube3>
{
public:
  Cube2Cube3Function(float sx, float sy, float sz, float px, float py, float pz) : sx(sx), sy(sy), sz(sz), px(px), py(py), pz(pz) { }
  Cube3 Index(Cube2 c) const
  {
    float fx = c.x;
    float fy = c.y;
    float fz = c.z;
    fx *= sx;
    fy *= sy;
    fz *= sz;
    fx+=px;
    fy+=py;
    fz+=pz;
    Cube3 cc;
    cc.minx = fx - sx/2.0;
    cc.miny = fy - sy/2.0;
    cc.minz = fz - sz/2.0;
    cc.maxx = fx + sx/2.0;
    cc.maxy = fy + sy/2.0;
    cc.maxz = fz + sz/2.0;
    return cc;
  }
private:
  float sx,sy,sz;
  float px,py,pz;
};


class GenerateCube
{
public:
  GenerateCube(HandleValue<Cube2> &gen, int sizex, int sizey, int sizez) : gen(gen), sizex(sizex), sizey(sizey), sizez(sizez) { }
  void Gen()
  {
    for(int z=0;z<sizez;z++)
      for(int y=0;y<sizey;y++)
	for(int x=0;x<sizex;x++)
	  {
	    Cube2 c;
	    c.x = x;
	    c.y = y;
	    c.z = z;
	    gen.Handle(c);
	  }
  }
private:
  HandleValue<Cube2> &gen;
  int sizex, sizey, sizez;
};

template<class C>
class GenerateVoxel : private HandleValue<Cube2>
{
public:
  GenerateVoxel(Voxel<C> &voxel, HandleValue<std::pair<Cube2, C> > &hv) : voxel(voxel), gen(*this, voxel.SizeX(), voxel.SizeY(), voxel.SizeZ()),  hv(hv) { }
  void Gen() { gen.Gen(); }
private:
  void Handle(Cube2 c)
  {
    C cc = voxel.Map(c.x, c.y, c.z);
    hv.Handle(std::make_pair(c,cc));
  }
private:
  Voxel<C> &voxel;
  GenerateCube gen;
  HandleValue<std::pair<Cube2,C> > &hv;
};

template<class T>
class FilterHandleValue : public HandleValue<std::pair<T, bool> >
{
public:
  FilterHandleValue(HandleValue<T> &hv) : hv(hv) { }
  void Handle(std::pair<T,bool> p)
  {
    if (p.second)
      {
	hv.Handle(p.first);
      }
  }
private:
  HandleValue<T> &hv;
};

template<class T>
class HandleValueSource
{
public:
  HandleValueSource(Array<int, T> &arr, HandleValue<T> &hv) : arr(arr), hv(hv) { }
  void Gen()
  {
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	hv.Handle(arr.Index(i));
      }
  }
private:
  Array<int, T> &arr;
  HandleValue<T> &hv;
};

class CubeHandleValue : public HandleValue<Cube2>
{
public:
  CubeHandleValue(Point p, 
		  Vector ax, Vector ay, Vector az,
		  HandleValue<Vector> &hv) : pp(p),						     ax(ax),ay(ay),az(az), hv(hv) { }
  void Handle(Cube2 p)
  {
    Vector v = pp + float(p.x) * ax + float(p.y) * ay + float(p.z)*az;
    hv.Handle(v);
  }
private:
  Point pp;
  Vector ax, ay, az;
  HandleValue<Vector> &hv;
};

template<class T>
class ScaleTranslateHandleValue : public HandleValue<std::pair<Vector, T> >
{
public:
  ScaleTranslateHandleValue(HandleValue<std::pair<Matrix, T> > &hv, float sx, float sy, float sz) : hv(hv), sx(sx), sy(sy), sz(sz) { }
  void Handle(std::pair<Vector,T> v) 
  {
    hv.Handle(std::make_pair(Matrix::Scale(sx,sy,sz)*Matrix::Translate(v.first.dx, v.first.dy, v.first.dz), v.second));
  }
private:
  HandleValue<std::pair<Matrix, T> > &hv;
  float sx,sy,sz;
};

class ColorHandleValue : public HandleValue<Vector>
{
public:
  ColorHandleValue(HandleValue<std::pair<Vector,unsigned int> > &hv, float sizex, float sizey, float sizez) : hv(hv), sizex(sizex), sizey(sizey), sizez(sizez)
  { }
  void Handle(Vector v) 
  {
    Vector v2 = v;
    v2.dx +=sizex/2;
    v2.dy +=sizey/2;
    v2.dz +=sizez/2;
    unsigned int color = Color::ColorCube(v2.dx,v2.dy,v2.dz, sizex, sizey, sizez);
    hv.Handle(std::make_pair(v, color));
  }
private:
  HandleValue<std::pair<Vector, unsigned int> > &hv;
  float sizex, sizey, sizez;
};

class FaceCollectionHandleValueDynamic : public HandleValue<std::pair<Matrix, unsigned int > >
{
public:
  FaceCollectionHandleValueDynamic() { }
  void DrawVBO(VBOState &vbostate, VBOUpdate u);
  void Handle(std::pair<Matrix, unsigned int> mm)
  {
    Matrix m = mm.first;
    Node n = { 
      { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
	m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
	m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
	m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] },
      mm.second };
    vec.push_back(n);
  }
private:
  //BoxableFaceCollection &fc;
  struct Node
  {
    float mat[16];
    unsigned int color;
  };
  mutable VectorArray<Node> vec;
};

struct AnimPoint
{
public:
  Point pos;
  float time;
};
struct ObjectId
{
  int id;
};

class Dynamic : public HandleValue<std::pair<int, Matrix> >,
		public HandleValue<std::pair<int, AnimPoint> >,
		public HandleValue<std::pair<int, unsigned int> >,
		public HandleValue<std::pair<int, ObjectId > >
{
public:
  Dynamic(PieceShader &piece) : piece(piece) { }
  int count() const { return vec.size(); }
  int create_new() { vec.push_back(Node()); return vec.size()-1; }
  void unuse(int id) { vec[id].active=false; }
  void DrawVBO(VBOState &vbostate, VBOUpdate u, float time);
  void Handle(std::pair<int, Matrix> m); // original position
  void Handle(std::pair<int, AnimPoint> m); // movement
  void Handle(std::pair<int, unsigned int> m); // color
  void Handle(std::pair<int, ObjectId> m); // object id
private:
  struct Node
  {
    Matrix m;
    AnimPoint p;
    unsigned int color;
    int obj_id;
    bool active;
  };
private:
  std::vector<Node> vec;
  std::vector<std::pair<int, AnimPoint> > pending_changes;
  PieceShader &piece;
};


class FaceCollectionHandleValue : public HandleValue<std::pair<Matrix, unsigned int > >
{
public:
  FaceCollectionHandleValue(BoxableFaceCollection &fc) : vec(), fc(fc) { }
  void Handle(std::pair<Matrix,unsigned int> m)
  {
	MatrixElem *me = new MatrixElem(fc, m.first);
	ColorElem *ce = new ColorElem(*me, m.second);
	MemoizeFaces *memo = new MemoizeFaces(*ce);
	vec.push_back(memo);
	vec2.push_back(me);
	vec2.push_back(ce);
	vec2.push_back(memo);
	//RenderOpenGl(me);
  }
  void UpdateVBO(VBOState &vbostate, VBOUpdate u)
  {
    MemoizeArray<const FaceCollection*> memovec(vec.Size(), vec);
    FaceCollectionArrayConvert convert(memovec);
    CompressObject compress(convert);
    ::UpdateVBO(compress, vbostate, u);
  }
  ~FaceCollectionHandleValue()
  {
    int size = vec2.Size();
    for(int i=0;i<size;i++)
      {
	delete vec2.Index(i);
      }
  }
private:
  VectorArray<const FaceCollection*> vec;
  VectorArray<const FaceCollection*> vec2;
  BoxableFaceCollection &fc;
};

template<class T, class K>
class HandleValueFunction : public HandleValue<T>
{
public:
  HandleValueFunction(HandleValue<K> &hv, Function<T,K> &func) : hv(hv), func(func) { }
  void Handle(T t)
  {
    hv.Handle(func.Index(t));
  }
private:
  HandleValue<K> &hv;
  Function<T,K> &func;
};


template<class T, class K>
class SampledGraph : public Array<int, Point2d>
{
public:
  SampledGraph(Function<float,float> &func, float start, float end, int steps) : func(func), sample_func(start, (end-start)/steps), g(sample_hv, steps) {}
  Point2d Index(int i) const { return genarray.Index(i); }
  int Size() const { return genarray.Size(); }
private:
  Function<T,K> &func;

  GenerateArray<Point2d> genarray;
  
  
  HandleValueFunction<int, float> sample_hv;
  Sample sample_func;
  GenerateSequence g; // int
};


template<class A, class B, class C> // T = std::pair<A,B> 
class PullBack
{
public:
  PullBack(Array<int, A> &a_array, 
	   Array<int, B> &b_array,
	   Function<A,C> &f, 
	   Function<B,C> &g) : a_array(a_array), b_array(b_array), f(f), g(g) { }

  void Gen( HandleValue<std::pair<A,B> > &gen )
  {
    int asize = a_array.Size();
    int bsize = b_array.Size();
    for(int aa = 0;aa<asize;aa++)
      for(int bb = 0;bb<bsize;bb++)
	{
	  A a = a_array.Index(aa);
	  B b = b_array.Index(bb);
	  if (Eval(a,b))
	    gen.Handle(std::make_pair(a,b));
	}
  }
  bool Eval(A a, B b)
  {
    return GraphBool<C,C>::Compare(f.Index(a),g.Index(b));
  }

private:
  Array<int, A> &a_array;
  Array<int, B> &b_array;
  Function<A,C> &f;
  Function<B,C> &g;
};


class FunctionBitmap
{
public:
  FunctionBitmap(Array<std::pair<int, int>, bool> &func) : colorize(func), bitmapize(colorize), cache(bitmapize) 
  {
  }
  void Generate(Rect r) { cache.Generate(r); }
  BufferRef Ref() const { return cache.Ref(); }
private:
  ColorizeBoolArray<std::pair<int,int> > colorize;
  BitmapizeArray bitmapize;
  BitmapCache cache;
};




class VoxelEffect : public FrameAnimPlugins
{
public:
  VoxelEffect(Render *r);
  void Init();
  bool Frame(float time);
  ~VoxelEffect();
private:
  bool first;
  VBOState vbostate;
  FaceCollectionHandleValueDynamic hv;
  GenerateArray<std::pair<Vector, unsigned int> > hv1a;
  GenerateArray<std::pair<Vector, unsigned int> > hv2a;

  //int texture;
  GlobalTexturePiece globpiece;
  TexturePiece texturepiece;
  PieceShader shader;
  PhongPiece phongpiece;
  NormalPiece normalpiece;
};

class MarchingCubesVoxel
{
public:
  MarchingCubesVoxel(Voxel<bool> &vox, HandleValue<Cube2> &hv) : vox(vox), hv(hv) { }
  void Gen()
  {
    {
      std::set<Cube2, Compare> emptyset;
      visited_cubes.swap(emptyset);
    }
    Cube2 cc;
    Initial(cc);
    Id(cc);
  }
private:
  void Initial(Cube2 &c)
  {
    int sizex = vox.SizeX();
    int sizey = vox.SizeY();
    int sizez = vox.SizeZ();
    float x,y,z;
    do
      {
	x = float(random.next())/float(random.maximum());
	y = float(random.next())/float(random.maximum());
	z = float(random.next())/float(random.maximum());
	x*=sizex;
	y*=sizey;
	z*=sizez;
      }
    while(!vox.Map(int(x),int(y),int(z)));
    c.x = int(x);
    c.y = int(y);
    c.z = 0;
    bool b = Id2(c);
    while(b && c.z<sizez)
      {
	c.z++;
	b = Id2(c);
      }
  }
  bool Id2(const Cube2 &c)
  {
    bool a0 = vox.Map(c.x, c.y, c.z);
    bool x = vox.Map(c.x+1, c.y, c.z);
    bool y = vox.Map(c.x, c.y+1, c.z);
    bool z = vox.Map(c.x, c.y, c.z+1);
    bool yz = vox.Map(c.x, c.y+1, c.z+1);
    bool xz = vox.Map(c.x+1, c.y, c.z+1);
    bool xy = vox.Map(c.x+1, c.y+1, c.z);
    bool xyz = vox.Map(c.x+1, c.y+1, c.z+1);
    bool allsame = a0 == x && x == y && y==z && z == yz && yz == xz && xz == xy  && xy == xyz;
    return allsame;
  }
  void Id(const Cube2 &cc2)
  {
    std::vector<Cube2> nodes;
    nodes.push_back(cc2);
    Cube2 c;
    while(!nodes.empty())
      {
	c = nodes.back();
	nodes.pop_back();
	if (visited_cubes.find(c) != visited_cubes.end())
	  {
	    continue;
	  }
	visited_cubes.insert(c);
	hv.Handle(c);
	
	bool a0 = vox.Map(c.x, c.y, c.z);
	bool x = vox.Map(c.x+1, c.y, c.z);
	bool y = vox.Map(c.x, c.y+1, c.z);
	bool z = vox.Map(c.x, c.y, c.z+1);
	bool yz = vox.Map(c.x, c.y+1, c.z+1);
	bool xz = vox.Map(c.x+1, c.y, c.z+1);
	bool xy = vox.Map(c.x+1, c.y+1, c.z);
	bool xyz = vox.Map(c.x+1, c.y+1, c.z+1);
	
	bool my = (a0 == x) && (x == xz) && (z == a0); // -y
	bool px = (x == xy) && (xy == xyz) && (x == xz); // x
	bool mz = (a0 == x) && (x == xy) && (a0 == y); // -z
	bool pz = (z == xz) && (xz == xyz) && (z == yz); // z
	bool py = (y == xy) && (xy == xyz) && (y==yz); // y
	bool mx = (a0 == y) && (y == yz) && (a0 == z); // -x
	
	if (!mx)
	  {
	    Cube2 cc = c;
	    cc.x--;
	    nodes.push_back(cc);
	  }
	if (!px)
	  {
	    Cube2 cc = c;
	    cc.x++;
	    nodes.push_back(cc);
	  }
	
	if (!my)
	  {
	    Cube2 cc = c;
	    cc.y --;
	    nodes.push_back(cc);
	  }
	if (!py)
	  {
	    Cube2 cc = c;
	    cc.y ++;
	    nodes.push_back(cc);
	  }
	if (!mz)
	  {
	    Cube2 cc = c;
	    cc.z --;
	    nodes.push_back(cc);
	  }
	if (!pz)
	  {
	    Cube2 cc = c;
	    cc.z ++;
	    nodes.push_back(cc);
	  }
      }
  }
  class Compare
  {
  public:
    bool operator()(const Cube2 &c1, const Cube2 &c2)
    {
      if (c1.x != c2.x) return c1.x < c2.x;
      if (c1.y != c2.y) return c1.y < c2.y;
      if (c1.z != c2.z) return c1.z < c2.z;
      return false;
    }
  };
private:
  Voxel<bool> &vox;
  HandleValue<Cube2> &hv;
  std::set<Cube2, Compare> visited_cubes;
  mutable Random random;
};

class WaveFunction : public Function<Vector, Vector>
{
public:
  WaveFunction(float time, Waveform &F, Waveform &G, float c) : time(time), F(F), G(G), c(c) { }
  Vector Index(Vector v) const
  {
    float r = v.Dist();
    float oner = 1.0/r;
    float u = oner*(F.Index(r-c*time))+oner*(G.Index(r+c*time));
    return u*v;
  }
private:
  float time;
  Waveform &F;
  Waveform &G;
  float c;
};

void RenderVoxel(VolumeObject &o, int size, float wholesize, HandleValue<std::pair<Vector, unsigned int> > &hv, VBOState &vbostate);

void RenderVoxel(VolumeObject &orv, int size, float wholesize, HandleValue<std::pair<Vector, unsigned int> > &hv);

class WallsFromMap
{
public:
  WallsFromMap(Bitmap<bool> &mymap, Bitmap<unsigned int> &mycolor, HandleValue<std::pair<std::pair<int,int>, std::pair<int, unsigned int> > > &hv, int x, int y) : mymap(mymap), mycolor(mycolor), hv(hv), x(x), y(y) { }
  void Gen()
  {
    Recurse(x,y);
  }
private:
  // 0 = +x
  // 1 = -x
  // 2 = +y
  // 3 = -y
  // 4 = roof
  // 5 = floor
  void Recurse(int x, int y)
  {
    if (visited_nodes.find(std::make_pair(x,y))!=visited_nodes.end())
      {
	return;
      }
    if (!mymap.Map(x,y)) return;
    unsigned int color = mycolor.Map(x,y);
    visited_nodes.insert(std::make_pair(x,y));
    hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(4, color)));
    hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(5, color)));
    bool nx = mymap.Map(x+1,y);
    bool px = mymap.Map(x-1,y);
    bool ny = mymap.Map(x,y+1);
    bool py = mymap.Map(x,y-1);
    if (nx)
      {
	Recurse(x+1,y);
      }
    else
      {
	hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(0,color)));
      }
    if (px)
      {
	Recurse(x-1,y);
      }
    else
      {
	hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(1,color)));
      }
    if (ny)
      {
	Recurse(x,y+1);
      }
    else
      {
	hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(2,color)));
      }
    if (py)
      {
	Recurse(x,y-1);
      }
    else
      {
	hv.Handle(std::make_pair(std::make_pair(x,y), std::make_pair(3, color)));
      }    
  }
private:
  Bitmap<bool> &mymap;
  Bitmap<unsigned int> &mycolor;
  HandleValue<std::pair<std::pair<int,int>, std::pair<int, unsigned int> > > &hv;
  int x, y;
  mutable std::set<std::pair<int,int> > visited_nodes;
};
class WallMatrix : public HandleValue<std::pair<std::pair<int,int> , std::pair<int, unsigned int > > >
{
public:
  WallMatrix(HandleValue<std::pair<Matrix, unsigned int> > &hv, float size) : hv(hv), size(size) { }
  void Handle(std::pair<std::pair<int,int>, std::pair<int, unsigned int > > h)
  {
    int val = h.second.first;
    int x = h.first.first;
    int y = h.first.second;
    unsigned int color = h.second.second;
    Matrix m = Matrix::Identity();
    switch(val)
      {
      case 0: m = Matrix::YRotation(90*2.0*3.14159/360.0); break;
      case 1: m = Matrix::YRotation(-90*2.0*3.14459/360.0); break;
      case 3: m = Matrix::YRotation(180*2.0*3.14159/360.0); break;
      case 2: m = Matrix::Identity(); break; 
      case 4: m = Matrix::XRotation(-90*2.0*3.14159/360.0); break;
      case 5: m = Matrix::XRotation(90*2.0*3.14159/360.0); break;
     }
    hv.Handle(std::make_pair(Matrix::Translate(0.0,0.0,size/2)*m*Matrix::Translate(x*size, 0.0, y*size), color));
  }
private:
  HandleValue<std::pair<Matrix, unsigned int> > &hv;
  float size;
};

template<class P, class S>
class DivideAndConquer
{
public:
  virtual bool CanDivide(P p) const=0;
  virtual std::vector<P> Divide(P p) const=0;
  virtual S Solve(P p) const=0;
  virtual S Combine(std::vector<S> s) const=0;
};

template<class P, class S>
S Conquer(DivideAndConquer<P,S> &d, P p)
{
  if (d.CanDivide(p))
    {
      std::vector<P> pp = d.Divide(p);
      std::vector<S> ss;
      for(typename std::vector<P>::iterator i=pp.begin();i!=pp.end();i++)
	{
	  S s1 = Conquer(d, *i);
	  ss.push_back(s1);
	}
      S s = d.Combine(ss);
      return s;
    }
  else
    {
      S s = d.Solve(p);
      return s;
    }
}

class MapEffect : public FrameAnimPlugins
{
public:
  MapEffect(Render *r);
  ~MapEffect();
  void Init();
  bool Frame(float time);
  float XRot() const { return 0.0; }
  float YRot() const { return 1.0; }
  float ZRot() const { return 0.0; }
  float RotSpeed() const { return 0.0; }
public:
  MapFunction mapfunc1;
  ColorMapFunction colorfunc1;
  BitmapFromFile<bool> mapfunc;
  BitmapFromFile<unsigned int> colorfunc;

  FaceCollectionHandleValueDynamic hv;
  VBOState vbostate;
  bool first;

  unsigned int texture;
  TexturePiece texturepiece;
  PieceShader shader;
};

//
// Supershape
//
class SuperShape2d : public Function<float, float>
{
public:
  SuperShape2d(float a, float b, float m, float n1, float n2, float n3) : a(a), b(b), m(m), n1(n1), n2(n2), n3(n3) { }
  float Index(float alfa) const
  {
    //std::cout << alfa << " " << m << " " << a << " " << b << std::endl;
    float d1 = fabs(1.0/a*cos(m*alfa/4.0));
    float d2 = fabs(1.0/b*sin(m*alfa/4.0));
    float a1 = pow(d1,n2);
    float a2 = pow(d2,n3);
    float r2 = a1 + a2;
    float r = pow(double(r2), double(-1.0/n1));
    //std::cout << d1 << " " << d2 << " " << a1 << " " << a2 << " " << r2 << " " << r << std::endl;
    return r;
  }
private:
  float a,b, m, n1, n2, n3;
};

struct SuperParameters
{
  float a,b,m,n1,n2,n3;
};

class SuperShape3d : public SurfaceIn3d
{
public:
  SuperShape3d(SuperParameters &a1, SuperParameters &a2) : r1(a1.a, a1.b, a1.m, a1.n1, a1.n2, a1.n3),
							   r2(a2.a, a2.b, a2.m, a2.n1, a2.n2, a2.n3) { }
  SuperShape3d(float a, 
	       float b, 
	       float m, 
	       float n1, 
	       float n2, 
	       float n3,
	       float aa,
	       float bb,
	       float mm,
	       float nn1,
	       float nn2,
	       float nn3) 
    : r1(a ,b  ,m ,n1 ,n2 ,n3), 
      r2(aa,bb,mm,nn1,nn2,nn3) { }
  Point Index(float alfa, float beta) const
  {
    alfa-=3.14159/2.0;
    beta-=3.14159;

    float x = r1.Index(alfa)*cos(alfa)*r2.Index(beta)*cos(beta);
    float y = r1.Index(alfa)*sin(alfa)*r2.Index(beta)*cos(beta);
    float z = r2.Index(beta)*sin(beta);
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    //std::cout << p << std::endl;
    return p;
  }
  Point2d Texture(float x, float y) const { Point2d p; p.x = x; p.y = y; return p; }
  float Attrib(float x, float y, int id) const { return 0.0; }
  int AttribI(float x, float y, int id) const
  {
    return 0;
  }
  Vector Normal(float x, float y) const
  {
    Point p1 = Index(x,y);
    Point p2 = Index(x+0.01,y);
    Point p3 = Index(x,y+0.01);
    Vector u_x = p2-p1;
    Vector u_y = p3-p1;
    return Vector::CrossProduct(u_x, u_y);
  }
  unsigned int Color(float x, float y) const
  {
    return Color::White().Pixel();
  }
  float XSize() const
  {
    return 3.14159;
  }
  float YSize() const
  {
    return 2.0*3.14159;
  }

private:
  SuperShape2d r1;
  SuperShape2d r2;
};

template<class T>
class FunnyArray
{
public:
  virtual int Size() const=0;
  virtual int ASize(int x) const=0;
  virtual T Map(int x, int y) const=0;
};

template<class T>
class FunnyChoose : public Array<int, T>
{
public:
  FunnyChoose(FunnyArray<T> &arr, Array<int,int> &index) : arr(arr), index(index) { }
  int Size() const { return arr.Size(); }
  T Index(int i) const
  {
    int ii = index.Index(i);
    T t = arr.Map(i,ii);
    return t;
  }
private:
  FunnyArray<T> &arr;
  Array<int,int> &index;
};





struct VertexData3d
{
  Point center;
  float radius;
};
struct EdgeData3d
{
  Point dom_center;
  float dom_radius;

  Point cod_center;
  float cod_radius;
};

class GraphToLines : public LineCollection
{
public:
  GraphToLines(Graph &g, GraphData<VertexData3d, EdgeData3d> &data) : g(g), data(data) { }
  int NumLines() const
  {
    return g.NumEdges();
  }
  Point LinePoint(int line, int point) const
  {
    std::pair<int,int> e = g.EdgeVertex(line);
    EdgeData3d ed = data.Edge(line, e.first, e.second);
    Point p;
    if (point == 0) p = ed.dom_center;
    else p = ed.cod_center;
    return p;
  }
private:
  Graph &g;
  GraphData<VertexData3d, EdgeData3d> &data;
};

class FacesFromMap : public BoxableFaceCollection
{
public:
  FacesFromMap(Bitmap<bool> &mymap, Bitmap<Point> &pos) : mymap(mymap), pos(pos) { Refresh(); }

  void SetBox(Matrix m) { }
  virtual int NumFaces() const { return vec.size(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    const Face &face2 = vec[face];
    unsigned int p = point;
    if (p > face2.p.size()-1) p = face2.p.size()-1;
    if (face2.p.size()==0)
      return Point(0.0,0.0,0.0);
    return face2.p[p];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    const Face &face2 = vec[face];
    Point p = face2.p[0]; //pos.Map(0,0);
    Point px = face2.p[1]; //pos.Map(1,0);
    Point py = face2.p[2]; //pos.Map(0,1);
    return -Vector::CrossProduct(px-p, py-p);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }


  void Refresh()
  {
    vec = std::vector<Face>();
    int sizex = mymap.SizeX();
    int sizey = mymap.SizeY();
    for(int y=0;y<sizey-1;y++)
      for(int x=0;x<sizex-1;x++)
	{
	  Face f;
	  if (mymap.Map(x,y))
	    {
	      f.p.push_back(pos.Map(x,y));
	    }
	  if (mymap.Map(x+1,y))
	    {
	      f.p.push_back(pos.Map(x+1,y));
	    }
	  if (mymap.Map(x+1,y+1))
	    {
	      f.p.push_back(pos.Map(x+1,y+1));
	    }
	  if (mymap.Map(x,y+1))
	    {
	      f.p.push_back(pos.Map(x,y+1));
	    }
	  vec.push_back(f);
	}
  }

  struct Face
  {
    std::vector<Point> p;
  };
private:
  Bitmap<bool> &mymap;
  Bitmap<Point> &pos;
  std::vector<Face> vec;
};

class GraphFromMap : public Graph, public GraphData<VertexData3d, EdgeData3d>
{
public:
  GraphFromMap(Bitmap<bool> &mymap, Bitmap<Point> &pos, bool twoway, float radius) : mymap(mymap), twoway(twoway), pos(pos), radius(radius) 
  {
    Refresh();
  }
  VertexData3d Vertex(int v) const 
  {
    std::pair<int,int> p = positions[v];
    int x = p.first;
    int y = p.second;

    VertexData3d d;
    d.center = pos.Map(x,y);
    d.radius = radius;
    return d;
  }
  EdgeData3d Edge(int /*e*/, int v1, int v2) const
  {
    std::cout << v1 << " " << v2 << std::endl;
    VertexData3d d1 = Vertex(v1);
    VertexData3d d2 = Vertex(v2);
    EdgeData3d e;
    e.dom_center = d1.center;
    e.dom_radius = d1.radius;
    e.cod_center = d2.center;
    e.cod_radius = d2.radius;
    std::cout << e.dom_center << " " << e.cod_center << std::endl;
    return e;
  }
  int NumVertexes() const
  {
    return counter;
  }
  int NumEdges() const
  {
    return edgecount;
  }
  std::pair<int,int> EdgeVertex(int edge) const
  {
    return edges[edge];
  }
  void Refresh()
  {
    int xsize = mymap.SizeX();
    int ysize = mymap.SizeY();
    int count = 0;
    for(int y=0;y<ysize;y++)
      for(int x=0;x<xsize;x++)
	{
	  if (mymap.Map(x,y))
	    {
	      positions[count] = std::make_pair(x,y);
	      count ++;
	    }
	}

    counter = count;
    int count2 = 0;
    count = 0;
    for(int y=0;y<ysize;y++)
      for(int x=0;x<xsize;x++)
	{
	  if (mymap.Map(x,y))
	    {
	      if (mymap.Map(x+1,y)) { edges[count2]=std::make_pair(count, find(x+1,y)); count2++; }
	      if (mymap.Map(x,y+1)) { edges[count2]=std::make_pair(count, find(x,y+1)); count2++; }
	      if (twoway)
		{
		  if (mymap.Map(x-1,y)) { edges[count2]=std::make_pair(count, find(x-1,y)); count2++; }
		  if (mymap.Map(x,y-1)) { edges[count2]=std::make_pair(count, find(x,y-1)); count2++; }
		}
	      count ++;
	    }
	}
    edgecount = count2;
  }
  int find(int x, int y)
  {
    for(int i=0;i<counter;i++)
      {
	std::pair<int,int> p = positions[i];
	if (p.first == x && p.second == y)
	  {
	    return i;
	  }
      }
    return -1;
  }
private:
  mutable std::map<int, std::pair<int,int> > positions;
  mutable std::map<int, std::pair<int,int> > edges;
  int counter;
  int edgecount;
  Bitmap<bool> &mymap; 
  bool twoway;
  Bitmap<Point> &pos;
  float radius;
};


template<class T>
class DataRep
{
public:
  virtual BoxableFaceCollection *Data(const T &t) const=0;
};

class SphereDataRep : public DataRep<VertexData3d>
{
public:
  SphereDataRep(int x, int y) : x(x), y(y) { }
  virtual BoxableFaceCollection *Data(const VertexData3d &t) const
  {
    sphere = new SphereElem(x,y);
    sphere->SetBox(Matrix::Identity());
    mat = new MatrixElem(*sphere, Matrix::Scale(t.radius, t.radius, t.radius)*Matrix::Translate(t.center.x, t.center.y, t.center.z));
    return mat;
  }
private:
  mutable SphereElem *sphere;
  mutable MatrixElem *mat;
  int x, y;
};

class ConeDataRep : public DataRep<EdgeData3d>
{
public:
  ConeDataRep(int num) : num(num) { }
  virtual BoxableFaceCollection *Data(const EdgeData3d &t) const
  {
    elem = new ConeElem(num, t.dom_center, t.cod_center, t.dom_radius, t.cod_radius);
    return elem;
  }
private:
  mutable ConeElem *elem;
  int num;
};

template<class V, class E>
class RenderGraph : public BoxableFaceCollection
{
public:
  RenderGraph(Graph &g, 
	      GraphData<V,E> &data,
	      DataRep<V> &vertex,
	      DataRep<E> &edge) 
    : g(g), data(data), vertex(vertex), edge(edge), or_elem(0)
  { Refresh(); }
  void Refresh()
  {
    int v = g.NumVertexes();
    for(int a = 0; a<v; a++)
      {
	V vv = data.Vertex(a);
	BoxableFaceCollection *coll = vertex.Data(vv);
	vec.push_back(coll);
      }
    int e = g.NumEdges();
    for(int b = 0; b<e; b++)
      {
	std::pair<int,int> p = g.EdgeVertex(b);
	E ee = data.Edge(b, p.first, p.second);
	BoxableFaceCollection *coll = edge.Data(ee);
	vec.push_back(coll);
      }
    delete or_elem;
    or_elem = new OrElem<BoxableFaceCollection>(vec.begin(), vec.end());
    or_elem->update_faces_cache();
  }
  virtual int NumFaces() const { return or_elem->NumFaces(); }
  virtual int NumPoints(int face) const { return or_elem->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return or_elem->FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { return or_elem->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return or_elem->Attrib(face, point, id); }
  virtual int AttribI(int face, int point, int id) const { return or_elem->AttribI(face, point, id); }
  virtual unsigned int Color(int face, int point) const { return or_elem->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return or_elem->TexCoord(face,point); }
  virtual void SetBox(Matrix b) { return or_elem->SetBox(b); }
  virtual bool Inside(Point p) const { return or_elem->Inside(p); }
  
private:
  Graph &g;
  GraphData<V,E> &data;
  DataRep<V> &vertex;
  DataRep<E> &edge;
  std::vector<BoxableFaceCollection*> vec;
  OrElem<BoxableFaceCollection> *or_elem;
};



class NormalMapping1 : public Bitmap<Point>
{
public:
  NormalMapping1(SurfaceIn3d &surf, SurfaceIn3d &polys, int sizex, int sizey) : surf(surf), polys(polys), sizex(sizex), sizey(sizey) { }

  virtual int SizeX() const { return sizex; }
  virtual int SizeY() const { return sizey; }
  virtual Point Map(int x, int y) const
  {
    Point p1 = surf.Index(surf.XSize()*x/sizex, surf.YSize()*y/sizey);
    Point p2 = polys.Index(polys.XSize()*x/sizex, polys.YSize()*y/sizey);
    Vector v = p2-p1;
    return Point(v);
  }
private:
  SurfaceIn3d &surf;
  SurfaceIn3d &polys;
  int sizex;
  int sizey;
};


template<class C>
class ZoomingStep
{
public:
  virtual Bitmap<C>* Index(C c) const=0;
};
template<class C>
class ZoomArray : public Array<int, ZoomingStep<C>*>
{
};


class ChoosePointByMouseCursor
{
public:
  ChoosePointByMouseCursor(PointCollection &coll) : coll(coll) { }
  int Choose(Point2d pp) const
  {
    int m = 100000.0;
    int index;
    for(int i=0;i<coll.Size();i++)
      {
	Point p = coll.Index(i);
	Point2d pp2 = { p.x-pp.x, pp.y-pp.y };	
	float dist = sqrt(pp2.x*pp2.x+pp2.y*pp2.y);
	if (dist < m) { m=dist; index = i; }
      }
    return index;
  }
private:
  //Point2d pp;
  PointCollection &coll;
};
template<class T>
class Monoid
{
public:
  virtual T m(T t1, T t2) const=0;
  virtual T id() const=0;
};
template<class T>
class CompositionMonoid : public Monoid<Function<T,T> *>, private Function<T,T>
{
public:
  virtual Function<T,T> *m(Function<T,T> *t1, Function<T,T> *t2) const
  {
    m_t1 = t1;
    m_t2 = t2;
    return this;
  }
  T Index(T t) const
  {
    return m_t2->Index(m_t1->Index(t));
  }
  Function<T,T> *id() const
  {
    return &idf;
  }
private:
  Function<T,T> *m_t1;
  Function<T,T> *m_t2;
  IdentityFunction<T> idf;
};

template<class T>
class GroupObject
{
public:
  virtual T m(T t1, T t2) const=0;
  virtual T id() const=0;
  virtual T inv(T t) const=0;
};
template<class T>
class GroupMonoid : public Monoid<T>
{
public:
  GroupMonoid(GroupObject<T> &g) : g(g) { }
  T m(T t1, T t2) const { return g.m(t1,t2); }
  T id() const { return g.id(); }
private:
  GroupObject<T> &g;
};
class FloatGroup : public GroupObject<float>
{
public:
  float m(float t1, float t2) const { return t1*t2; }
  float id() const { return 1.0; }
  float inv(float a) const { return 1.0/a; }
};

class MatrixGroup : public GroupObject<Matrix>
{
public:
  Matrix m(Matrix t1, Matrix t2) const
  {
    return t1*t2;
  }
  Matrix id() const
  {
    return Matrix::Identity();
  }
  Matrix inv(Matrix t) const
  {
    return Matrix::Inverse(t);
  }

};

#if 0
template<class T, class U, class B>
class Part
{
public:
  virtual U Index_p(T t) const=0;
  virtual B Index_ch(U u) const=0;
};

template<class V, class X, class Y, class B>
class InverseImage : public Function<Part<V,Y,B>*, Part<std::pair<X,V>,X, B> *>, private Part<std::pair<X,V>,X, B>
{
public:
  InverseImage(Function<X,Y> &f) : f(f) { }
  Part<std::pair<X,V>,X, B> *Index(Part<V,Y, B> *p) const
  {
    pp = p;
    return this;
  }
  B Index_ch(X x) const
  {
    Y y = f.Index(x);
    return pp->Index_ch(y);
  }
  X Index_p(std::pair<X,V> u) const
  {
    return u.first;
  }
private:
  Function<X,Y> &f;
  Part<V,Y,B> *pp;
};

template<class X, class V, class Y, class B>
class Intersection : private InverseImage<V,X,Y,B>, private Function<X,Y>, private Part<std::pair<X,V>,Y,B>
{
public:
  Intersection(Part<V,Y,B> &j, Part<X,Y,B> &f) : InverseImage<V,X,Y,B>(*this), j(j), f(f) { }
  
  Part<std::pair<X,V>,X,B> *Part_i() const
  {
    return Index(&j);
  }
  Part<std::pair<X,V>,Y,B> *Part_m() const
  {
    return this;
  }
  Y Index_p(std::pair<X,V> p) const
  {
    Part<std::pair<X,V>,X,B> *i = Part_i();
    X x = i->Index_p(p);
    Y y = Index(x);
    return y;
  }
  B Index_ch(Y y) const
  {
    return j.Index_ch(y) && f.Index_ch(y);
  }

  Y Index(X x) const
  {
    return f.Index_p(x);
  }
private:
  Part<V,Y,B> &j;
  Part<X,Y,B> &f;
};

#endif

struct SubSpace
{
public:
  VolumeObject *obj;
};


class RegularPolygon : public PointCollection2d
{
public:
  RegularPolygon(Point2d center, float radius, int count, float angle) : circle(center, radius), sample(circle, count, angle) { }
  int Size() const { return sample.Size(); }
  Point2d Index(int i) const { return sample.Index(i); }
private:
  Circle circle;
  SampleCurveIn2d sample;
};

struct Line2d
{
  Point2d p1,p2;
};

Point2d IntersectLines(const Line2d &l1, const Line2d &l2);
float AngleLines(const Point2d &intersect, const Point2d &l1, const Point2d &l2);

class Line2dCollection : public Array<int,Line2d>
{
public:
};



class ContinuousLines2d : public Line2dCollection
{
public:
  ContinuousLines2d(PointCollection2d &p, bool loops) : p(p), loops(loops) { }
  int Size() const { return loops?p.Size():p.Size()-1; }
  Line2d Index(int line) const
  {
    //std::cout << "ContinuousLines2d" << line << std::endl;
    Line2d res;
    int point = 0;
    int num = line+point;
    num %= p.Size();
    res.p1 = p.Index(num);
    point = 1;
    num = line+point;
    num %= p.Size();
    res.p2 = p.Index(num);
    return res;
  }
private:
  PointCollection2d &p;
  bool loops;
};

class LineIn2d : public CurveIn2d
{
public:
  LineIn2d(Point2d p1, Point2d p2) : p1(p1), p2(p2) { }
  float Size() const { return 1.0; }
  Point2d Index(float x) const 
  {
    return Point2d::FromVector((1.0-x)*Vector2d::FromPoint(p1)+x*Vector2d::FromPoint(p2));
  }
public:
  Point2d p1,p2;
};

#if 0
template<class T>
class ReorientedFunction : public Bitmap<T>
{
public:
  ReorientedFunction(Line2d &line, ContinuousBitmap<T> &func, T default_, int sz, int sy, float height) : line(line), func(func), sx(sx), sy(sy), height(height), default_(default_) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  T Map(int x, int y) const
  {
    Point2d p = { float(x), float(y) };
    std::pair<Vector,Vector> pp = Vector2d::SplitToComponents(p-line.p1, line.p2-line.p1);
    Vector vx = pp.first;
    Vector vy = pp.second;
    vx/=(line.p2-line.p1).Dist();
    vy/=height;
    float px = line.p1.x+vx*func.SizeX();
    float py = line.p1.y+vy*func.SizeY();
    if (px<0||px>=func.SizeX()) return default_;
    if (py<0||py>=func.SizeY()) return default_;
    T t = func.Map(px,py);
    return t;
  }
private:
  Line2d &line;
  ContinuousBitmap<T> &func;
  int sx,sy;
  float height;
  T default_;
};
#endif

template<class T>
class ChooseBitmap2 : public Bitmap<Color>
{
public:
  ChooseBitmap2(Bitmap<Color> &bm1, Bitmap<Color> &bm2, Bitmap<float> &choose)
    : bm1(bm1), bm2(bm2), choose(choose) { }
  virtual int SizeX() const { return bm1.SizeX(); }
  virtual int SizeY() const { return bm1.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Color t1 = bm1.Map(x,y);
    Color t2 = bm2.Map(x,y);
    Color t = Color::Interpolate(t1,t2, choose.Map(x,y));
    return t;
  }

private:
  Bitmap<T> &bm1;
  Bitmap<T> &bm2;
  Bitmap<float> &choose;
};

template<class T>
class ContainerBitmap : public Bitmap<T>
{
public:
  ContainerBitmap(int sx, int sy, T def) : sx(sx), sy(sy), def(def) { }
  int push_back(TRect rect, Bitmap<T> *bm)
  {
    rects.push_back(rect);
    vec.push_back(bm);
    return vec.size()-1;
  }
  void move(int index, TRect newpos) { rects[index]=newpos; }
  void change(int index, Bitmap<T> *bm) { vec[index]=bm; }
  void resize(int new_sx, int new_sy) { sx = new_sx; sy = new_sy; }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    int sz = rects.size();
    for(int i=0;i<sz;i++)
      {
	TRect r = rects[i];
	if (x<r.x) continue;
	if (y<r.y) continue;
	if (x>=r.x+r.width) continue;
	if (y>=r.y+r.height) continue;
	return vec[i].Map(x-r.x,y-r.y);
      }
    return def;
  }
private:
  int sx,sy;
  std::vector<TRect> rects;
  std::vector<Bitmap<T>*> vec;
  T def;
};
template<class T>
class LayerBitmap : public Bitmap<T>
{
public:
  LayerBitmap(T (*fptr)(T *,int), int sx, int sy, T def) : fptr(fptr), sx(sx), sy(sy), def(def) { }
  void push_back(Bitmap<T> *bm)
  {
    vec.push_back(bm);
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    int sz = vec.size();
    if (sz==0) return def;
    std::vector<T> v;
    for(int i=0;i<sz;i++)
      {
	v.push_back(vec[i]->Map(x,y));
      }
    T val = fptr(&v[0], sz);
    return val;
  }
private:
  T (*fptr)(T*,int);
  int sx,sy;
  std::vector<Bitmap<T>*> vec;
  T def;
};

class LineIn2d2 : public Function<float,float> // x->y
{
public:
  LineIn2d2(Point2d p1, Point2d p2) : p1(p1), p2(p2) { }
  float Index(float x) const
  {
    float y = p1.y + (p2.y-p1.y)*(x-p1.x)/(p2.x-p1.x);
    return y;
  }
private:
  Point2d p1,p2;
};
class ConstantFloatFunction : public Function<float,float>
{
public:
  ConstantFloatFunction(float y) : y(y) { }
  float Index(float) const { return y; }
private:
  float y;
};

class SplitLines2d : public Line2dCollection
{
public:
  SplitLines2d(Line2dCollection &lines, Line2d split) : lines(lines), split(split) { }
  int Size() const { return vec.size(); }
  Line2d Index(int i) const
  {
    return vec[i];
  }

  void Execute()
  {
    int size = lines.Size();
    vec.clear();
    for(int i=0;i<size;i++)
      {
	Line2d line = lines.Index(i);
	LineIn2d2 l(line.p1, line.p2);
	LineIn2d2 sp(split.p1, split.p2);
	if ((l.Index(line.p1.x) > sp.Index(line.p1.x) &&
	     l.Index(line.p2.x) < sp.Index(line.p2.x))
	    ||
	    (l.Index(line.p1.x) < sp.Index(line.p1.x) &&
	     l.Index(line.p2.x) > sp.Index(line.p2.x))
	    )
	  {  // here we have the points in different side of the line
	    Point2d splitpoint = IntersectLines(line,split);
	    Line2d l1 = { line.p1, splitpoint };
	    Line2d l2 = { splitpoint, line.p2 };
	    vec.push_back(l1);
	    vec.push_back(l2);
	  }
	else
	  {
	    vec.push_back(line);
	  }
      }
  }

private:
  Line2dCollection &lines;
  Line2d split;
  std::vector<Line2d> vec;
};


class SwapLine2dCollection : public Line2dCollection
{ // sorts points by y-coordinate
public:
  SwapLine2dCollection(Line2dCollection &coll) : coll(coll) { }
  int Size() const { return coll.Size(); }
  Line2d Index(int i) const
  {
    Line2d l = coll.Index(i);
    if (l.p1.y > l.p2.y)
      {
	std::swap(l.p1.x,l.p2.x);
	std::swap(l.p1.y,l.p2.y);
      }
    return l;
  }
private:
  Line2dCollection &coll;
};

class FilterLines : public Array<int,bool>
{ // requires sorted lines
public:
  FilterLines(Line2dCollection &coll, float y) : coll(coll), y(y) { }
  int Size() const
  {
    return coll.Size();
  }
  bool Index(int i) const
  {
    Line2d l = coll.Index(i);
    if (l.p1.y <= y && y <= l.p2.y)
      {
	return true;
      }
    return false;
  }
private:
  Line2dCollection &coll;
  float y;
};

class Field : public Function<Point,float>
{
};

class FieldBitmap : public Bitmap<float>
{
public:
  FieldBitmap(Field &f, Bitmap<Point> &pos) : f(f), pos(pos) { }
  int SizeX() const { return pos.SizeX(); }
  int SizeY() const { return pos.SizeY(); }
  float Map(int x, int y) const { return f.Index(pos.Map(x,y)); }
private:
  Field &f;
  Bitmap<Point> &pos;
};


class DistanceField : public Field
{
public:
  DistanceField(Point center, float dist) : center(center), dist(dist) { }
  float Index(Point p) const
  {
    p -= center;
    float d = p.Dist();
    return std::abs(d-dist);
  }
private:
  Point center;
  float dist;
};

class FindPositionFromCurve : public Element<float>
{ // finds position from curve where field=0.
public:
  FindPositionFromCurve(Field &field, CurveIn3d &curve) : field(field), curve(curve) { }
  float Elem() const
  {
    SolvableCompose<Point> solved(curve,field);
    float val = Solve(solved, 0.0, curve.Size());
    return val;
  }
  
private:
  Field &field;
  CurveIn3d &curve;
};

class ChessBoard : public Bitmap<bool>
{
public:
  ChessBoard(int sx, int sy, bool b) : sx(sx), sy(sy), b(b) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual bool Map(int x, int y) const
  {
    bool bb = false;
    if (x & 1)
      {
	bb = !bb;
      }
    if (y & 1)
      {
	bb = !bb;
      }
    if (b)
      {
	bb = !bb;
      }
    return bb;
  }
private:
  int sx,sy;
  bool b;
};

template<class K, class T>
class CollectBitmapsBitmap : public Bitmap<Bitmap<T>*>
{
public:
  CollectBitmapsBitmap(const Bitmap<K> &bm, 
		       const Function<K,Bitmap<T>* > &func) 
    : bm(bm), func(func) { }
  int SizeX() const
  {
    return bm.SizeX();
  }
  int SizeY() const
  {
    return bm.SizeY();
  }
  Bitmap<T> *Map(int x, int y) const
  {
    return func.Index(bm.Map(x,y));
  }
private:
  const Bitmap<K> &bm;
  const Function<K, Bitmap<T>* > &func;
};

template<class T>
class FlattenBitmap : public Bitmap<T>
{
public:
  FlattenBitmap(Bitmap<Bitmap<T>* > &bm) : bm(bm) { }
  int SizeX() const
  {
    return StartX(bm.SizeX());
  }
  int SizeY() const
  {
    return StartY(bm.SizeY());
  }
  T Map(int x, int y) const
  {
    int sizex = bm.SizeX();
    int posx = 0;
    int dx = 0;
    int kx = 0;
    for (int i=0;i<sizex;i++)
      {
	int oldposx = posx;
	posx = StartX(i);
	if (x < posx)
	  {
	    kx = i;
	    dx = x - oldposx;
	    break;
	  }
      }

    int sizey = bm.SizeY();
    int posy = 0;
    int dy = 0;
    int ky = 0;
    for (int i=0;i<sizey;i++)
      {
	int oldposy = posy;
	posy = StartY(i);
	if (y < posy)
	  {
	    ky = i;
	    dy = y - oldposy;
	    break;
	  }
      }
    return bm.Map(kx,ky)->Map(dx,dy);
  }
  int StartX(int x) const
  {
    int sx = bm.SizeX();
    int sizex = 0;
    for(int i=0;i<sx;i++)
      {
	sizex += bm.Map(i,0)->SizeX();
	if (i == x) break;
      }
    return sizex;
  }
  int StartY(int y) const
  {
    int sy = bm.SizeY();
    int sizey = 0;
    for(int i=0;i<sy;i++)
      {
	sizey += bm.Map(0,i)->SizeY();
	if (i == y) break;
      }
    return sizey;
  }
private:
  Bitmap<Bitmap<T> *> &bm;
};
template<class K, class T>
class CombineBitmaps2 : public Bitmap<T>
{ // ideal for chessboard
public:
  CombineBitmaps2(const Bitmap<K> &bm, const Function<K, Bitmap<T>* > &func) : collect(bm, func), flatten(collect) { }
  int SizeX() const { return flatten.SizeX(); }
  int SizeY() const { return flatten.SizeY(); }
  T Map(int x, int y) const { return flatten.Map(x,y); }
private:
  CollectBitmapsBitmap<K,T> collect;
  FlattenBitmap<T> flatten;
};

class ChessBoardBitmap : public Bitmap<Color>, private Function<bool, Bitmap<Color>* >
{
public:
  ChessBoardBitmap(Color c1, Color c2, int sx, int sy, int ssx, int ssy) : white_bm(c1, sx,sy), black_bm(c2,sx,sy), bitmap_bool(ssx,ssy, true), combine(bitmap_bool, *this) { }
  Bitmap<Color> *Index(bool b) const
  {
    if (b) return &white_bm;
    return &black_bm;
  }
  int SizeX() const { return combine.SizeX(); }
  int SizeY() const { return combine.SizeY(); }
  Color Map(int x, int y) const { return combine.Map(x,y).second; }
private:
  mutable ConstantBitmap<Color> white_bm;
  mutable ConstantBitmap<Color> black_bm;
  ChessBoard bitmap_bool;
  CombineBitmaps<bool,Color> combine;
};


class SuperShapeEffect : public FrameAnim
{
public:
  SuperShapeEffect(Render *r) : FrameAnim(r) { }
  bool Frame(float time);
};

class RenderToTextureEffect : public FrameAnimPlugins
{
public:
  RenderToTextureEffect(Render *r);
  void Init();
  bool Frame(float time);
private:
  RectangleElem e;
  SimpleTexCoords tex;
  FaceRenderCmds render;
  TextureCommands tex2;
  TransparencyCmds trans;
  ColorCmds col;
  DisplayListCmds disp;

};

class InterpolateMovementEffect : public FrameAnim
{
public:
  InterpolateMovementEffect(Render *render) : FrameAnim(render) { }

  void Init();
  bool Frame(float time);
  ~InterpolateMovementEffect();
private:
  GenerateArray<Point> hv;
  GenerateArray<Point> hv2;
  VBOState vbostate;

  PieceShader shader;
  LinearInterpolationShader linshader;
  GouraudPiece phong;
};

class GraphEffect : public FrameAnim
{
public:
  GraphEffect(Render *render) : FrameAnim(render) { }

  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
  //VBOState vbostate2;
};

class ShaderEffect : public FrameAnim
{
public:
  ShaderEffect(ShaderFile &file, Render *r) : FrameAnim(r), seq(file) { }
  void Init();
  bool Frame(float time);
private:
  ShaderSeq seq;
  int s;
  VBOState vbostate;
};

class Effect2dEffect : public FrameAnim
{
public:
  Effect2dEffect(ShaderFile &file, Render *r) : FrameAnim(r), seq(file) { }
  void Init();
  bool Frame(float time);
private:
  ShaderSeq seq;
  int s;
  VBOState vbostate;
};
class Effect3dEffect : public FrameAnim
{
public:
  Effect3dEffect(ShaderFile &file, Render *r) : FrameAnim(r), seq(file) { }
  ~Effect3dEffect();
  void Init();
  bool Frame(float time);
private:
  ShaderSeq seq;
  int s;
  VBOState vbostate;
};

class IntersectAnim : public Anim
{
public:
  int NumFrames() const { return 5; }
  FaceCollection *Frame(int framenum) const
  {
    SphereElem *sphere1 = new SphereElem(10,10);
    SphereElem *sphere2 = new SphereElem(10,10);
    MatrixElem *s1 = new MatrixElem(*sphere1, Matrix::Scale(100,100,100)*Matrix::Translate(framenum,100.0,0.0));
    MatrixElem *s2 = new MatrixElem(*sphere2, Matrix::Scale(100,100,100));
    AndNotElem *anot = new AndNotElem(*s1,*s2);
    return anot;
  }
};

class SizeBitmapTo2 : public Bitmap<Color>
{
public:
  SizeBitmapTo2(Bitmap<Color> &bm) : bm(bm) { }
  int SizeX() const 
  {
    int a=1;
    int b=2;
    while(bm.SizeX() > b)
      {
	a <<= 1;
	b <<= 1;
      }
    return b;
  }
  int SizeY() const 
  { 
    int aa=1;
    int bb=2;
    while(bm.SizeY() > bb)
      {
	aa <<= 1;
	bb <<= 1;
      }
    return bb;
  }
  Color Map(int x, int y) const 
  {
    if (x>=bm.SizeX()) return Color(0.0,0.0,0.0);
    if (y>=bm.SizeY()) return Color(0.0,0.0,0.0);
    return bm.Map(x,y); 
  }
private:
  Bitmap<Color> &bm;
};


class MeshTexturesImpl : public MeshTextures
{
public:
  MeshTexturesImpl(Bitmap<Color> &bm) : /*bm(bm),*/ bm2(bm), next(empty), ref(0) { }
  MeshTexturesImpl(Bitmap<Color> &bm, MeshTextures &tex) : /*bm(bm),*/ bm2(bm), next(tex), ref(0) { }
  ~MeshTexturesImpl() { delete ref; }
  virtual int NumTextures() const { return 1+next.NumTextures(); }
  virtual void GenTexture(int num) 
  { 
    if (num == 0)
      {
	delete ref;
	ref = new BufferFromBitmap(bm2);
	ref->Gen();
      }
  }
  virtual BufferRef TextureBuf(int num) const { return ref->Buffer(); }

private:
  //Bitmap<Color> &bm;
  SizeBitmapTo2 bm2;
  MeshTextures &next;
  MeshTextures empty;
  BufferFromBitmap *ref;
};


class ScrollableView
{ // speed = in frames
public:
  ScrollableView(int viewsizex, int viewsizey,
		 int wholesizex, int wholesizey,
		 float speed) 
    : vsx(viewsizex), vsy(viewsizey),
      wsx(wholesizex), wsy(wholesizey),
      speed(speed), current_time(0.0), current_x(0.0), current_y(0.0), target_time(-1.0) { }

  float TargetRangeX() const { return wsx-vsx; }
  float TargetRangeY() const { return wsy-vsy; }
  void SetTargetPos(float x, float y) 
  {
    //if (x < 0.0 || x > TargetRangeX()) return;
    //if (y < 0.0 || y > TargetRangeY()) return;
    target_x=x; target_y=y;
    float tx = current_x - target_x;
    float ty = current_y - target_y;
    float dist = sqrt(tx*tx+ty*ty);
    float time = dist*speed;
    target_time = current_time + time;
  }
  void Frame()
  {
    if (current_time < target_time)
      {
	Vector2d v = { target_x-current_x, target_y-current_y };
	float dist = v.Dist();
	if (dist > speed)
	  {
	    v/= dist;
	    v*= speed;
	  }
	else
	  {
	    /* do nothing */
	  }
	current_x += v.dx;
	current_y += v.dy;
      }
    current_time += 1.0;
  }
  float PosX() const { return current_x; }
  float PosY() const { return current_y; }
  void SetSpeed(float s) 
  { 
    speed = s; 
    SetTargetPos(target_x, target_y);
  }
private:
  int vsx, vsy;
  int wsx, wsy;
  float speed;

  float current_time;
  float current_x, current_y;
  float target_x, target_y, target_time;
};

void PrepareGrid(Bitmap<Color> &bm, int cellsx, int cellsy, ArrayRender &rend);
void RenderGrid(Bitmap<Pos> &bm, float x, float y, int cellsx, int cellsy, ArrayRender &rend, int start_x, int start_y, int size_x, int size_y);


template<class T>
class Space
{
public:
  virtual T SpaceMap(Point p) const=0;
};
template<class T>
class Space2d
{
public:
  virtual T SpaceMap(Point2d p) const=0;
};

class MoveMap : public Space<Point>
{
public:
  MoveMap(Vector v) : v(v) { }
  Point SpaceMap(Point p) const { return p+v; }
private:
  Vector v;
};

class PhongSpace : public Space<float>
{ // vector from origo to normal
public:
  PhongSpace(Point viewer, Point light, int count) : viewer(viewer), light(light), count(count) { }
  float SpaceMap(Point normal) const
  {
    Point p = viewer + Vector(light);
    //p.Normalize();
    //normal.Normalize();
    float col = Vector::DotProduct(normal, p);
    for(int i=0;i<count;i++)
      col *= col;
    return col;
  }
private:
  Point viewer;
  Point light;
  int count;
};

template<class T>
class PairSpace
{
public:
  virtual T PairMap(Point p1, Point p2) const=0;
};
template<class T>
class PairSpace2d
{
public:
  virtual T PairMap(Point2d p1, Point2d p2) const=0;
};


class PointSplitSpace : public Space<std::pair<Point,Point> >
{
public:
  PointSplitSpace(Vector v1, Vector v2) : v1(v1), v2(v2) { }
  std::pair<Point,Point> SpaceMap(Point p) const
  {
    return std::make_pair(p+v1, p+v2);
  }
private:
  Vector v1,v2;
};

class PointPairSpace2d : public PairSpace2d<Point2d>
{
public:
  PointPairSpace2d(Vector2d v1_dir, Vector2d v2_dir) : v1(v1_dir), v2(v2_dir) { }
  Point2d PairMap(Point2d p1, Point2d p2) const
  {
    Point2d pp1 = { p1.x+v1.dx, p1.y+v1.dy };
    Line2d l1 = { p1, pp1 };
    Point2d pp2 = { p2.x+v2.dx, p2.y+v2.dy };
    Line2d l2 = { p1, pp2 };
    Point2d inter = IntersectLines(l1,l2);
    return inter;
  }
private:
  Vector2d v1,v2;
};


class TextureCoordBitmap : public Bitmap<Point2d>
{
};
class ZBitmap : public Bitmap<int>
{
};

template<class T>
class CylinderMap
{
public:
  // alfa = [0..360]
  virtual T AMap(float alfa, float s) const=0;
};

template<class T>
class SphereMap
{
public:
  // alfa = [0..2pi]
  // beta = [0..pi]
  virtual T SMap(float alfa, float beta) const=0;
};

class SphereDist : public Bitmap<float>
{ // distance from bitmap to sphere surface
public:
  SphereDist(Point center, float r, int sx, int sy, Point2d tl, Point2d br) : center(center), /*r(r),*/ sx(sx), sy(sy), tl(tl), br(br) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    xx*= br.x - tl.x;
    yy*= br.y - tl.y;
    xx+=tl.x;
    yy+=tl.y;
    return xx*xx+yy*yy;
 }
private:
  Point center;
  //float r;
  int sx, sy;
  Point2d tl,br;
};

class GridEffect2 : public FrameAnimPlugins
{
public:
  GridEffect2(Render *rend) : FrameAnimPlugins(rend), scr(1024, 768, 500*10, 500*11, 1.0) { }
  void Init();
  bool Frame(float time);
  virtual float RotSpeed() const { return 0.0; }
private:
  ArrayRender rend;
  ScrollableView scr;
};

class BitmapSprite : public Sprite
{
public:
  BitmapSprite(Bitmap<Color> &c, Point2d pos) : c(c), pos(pos) { }
  virtual int NumFrames() const { return 1; }
  virtual int XSize(int frame) const { return c.SizeX(); }
  virtual int YSize(int frame) const { return c.SizeY(); }
  virtual Point2d Pos(int frame) const { return pos; }
  virtual Color Pixel(int frame, int x, int y) const { return c.Map(x,y); }
private:
  Bitmap<Color> &c;
  Point2d pos;
};

class ArraySprite : public Sprite
{
public:
  ArraySprite(Sprite **array, int count) : array(array), count(count) { }
  void update_cache()
  {
    int count = 0;
    int sum = 0;
    int f = 0;
    int k = 0;
    int frames = count;
    for(int i=0;i<frames;i++,k++)
      {
	int oldsum = sum;
	int c = array[i]->NumFrames();
	sum += c;
	for(int s=0;s<c;s++)
	  {
	    if (f%10==0) {
	      std::cout << "ArraySprite: " << f << "/" << frames << std::endl;
	    }
	    cache.push_back(f-oldsum);
	    num.push_back(k);
	    f++;
	  }
      }
  }
  virtual int NumFrames() const 
  {
    int counter = 0;
    for(int i=0;i<count;i++)
      {
	counter += array[i]->NumFrames();
      }
    return counter;
  }
  virtual int XSize(int frame) const
  {
#if 1
    return array[num[frame]]->XSize(cache[frame]);
#endif
#if 0
    int counter = 0;
    for(int i=0;i<count;i++)
      {
	int oldcount = counter;
	counter += array[i]->NumFrames();
	if (counter > frame) { return array[i]->XSize(frame-oldcount); }
      }
    return 0;
#endif
  }
  virtual int YSize(int frame) const
  {
#if 1
    return array[num[frame]]->YSize(cache[frame]);
#endif
#if 0
    int counter = 0;
    for(int i=0;i<count;i++)
      {
	int oldcount = counter;
	counter += array[i]->NumFrames();
	if (counter > frame) { return array[i]->YSize(frame-oldcount); }
      }
    return counter;
#endif
  }
  virtual Point2d Pos(int frame) const
  {
#if 1
    return array[num[frame]]->Pos(cache[frame]);
#endif
#if 0
    int counter = 0;
    for(int i=0;i<count;i++)
      {
	int oldcount = counter;
	counter += array[i]->NumFrames();
	if (counter > frame) { return array[i]->Pos(frame-oldcount); }
      }
    Point2d p = { 0.0, 0.0 };
    return p;
#endif
  }
  virtual Color Pixel(int frame, int x, int y) const
  {
#if 1
    return array[num[frame]]->Pixel(cache[frame],x,y);
#endif

#if 0
    int counter = 0;
    for(int i=0;i<count;i++)
      {
	int oldcount = counter;
	counter += array[i]->NumFrames();
	if (counter > frame) { return array[i]->Pixel(frame-oldcount, x,y); }
      }
    return Color(0,0,0);
#endif
  }
private:
  Sprite **array;
  int count;
  std::vector<int> cache;
  std::vector<int> num;
};


class VertexArraySet;
void TexturePrepare(const Sprite &s, ArrayRender &rend);
void TextureEnable(ArrayRender &rend, int frame, bool enable);
void PrepareSprite(const Sprite &s, ArrayRender &rend);
void PrepareSpriteToVA(const Sprite &s, VertexArraySet &vas);

void RenderSprite(const Sprite &s, int frame, Point2d pos, float z, ArrayRender &rend, Program *prog);
void RenderSprite(const Sprite &s, int frame, Point2d pos, float z, ArrayRender &rend, float mult_x, float mult_y, Program *prog);
void RenderSprite(const Sprite &s, int frame, Point2d pos1, Point2d pos2, Point2d pos1_inside, Point2d pos2_inside, float z, ArrayRender &rend, Program *prog);

class SpriteMesh : public Mesh
{
public:
  SpriteMesh(const Sprite &s) : s(s) { }
  virtual int NumStates() const { return 0; }
  virtual int StateNum(int frame, int face) const { return 0; }


  virtual int NumFrames() const { return s.NumFrames(); }
  virtual int NumFaces(int framenum) const { return 1; }
  virtual int NumPoints() const { return 4; }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    int sx = s.XSize(framenum);
    int sy = s.YSize(framenum);
    switch(point)
      {
      case 0: return Point(0.0,0.0, 0.0);
      case 1: return Point(sx, 0.0, 0.0);
      case 2: return Point(sx, sy, 0.0);
      case 3: return Point(0.0, sy, 0.0);
      };
    return Point(0.0,0.0,0.0);
  }
private:
  const Sprite &s;
};

class SpriteTexCoords : public MeshTexCoords
{
public:
  SpriteTexCoords(const Sprite &s) : s(s) { }
  virtual int NumFrames() const { return s.NumFrames(); }
  virtual Point2d TexCoord(int framenum, int statenum, int face, int point) const 
  {
    int sx = 1.0; //s.XSize(framenum);
    int sy = 1.0; //s.YSize(framenum);
    //std::cout << "Tex: " << sx << " " << sy << std::endl;
    switch(point)
      {
      case 0: return Point2d::NewPoint(0.0,0.0);
      case 1: return Point2d::NewPoint(sx, 0.0);
      case 2: return Point2d::NewPoint(sx, sy);
      case 3: return Point2d::NewPoint(0.0, sy);
      };
    return Point2d::NewPoint(0.0,0.0);
  }
private:
  const Sprite &s;
};


class Data
{
public:
  void insert(int id, void *data) { m[id]=data; }
  void *fetch(int id) { return m[id]; }
private:
  std::map<int, void*> m;
};


void PrepareCube(const Bitmap<Range<float> > &bm_alternatives, 
		 Point pos, Vector u_x, Vector u_y, Vector u_z, 
		 ArrayRender &rend, Data &d, int id);
void RenderCube(const Bitmap<Range<int> > &bm,
		const Matrix &pos,
		ArrayRender &rend, Data &d, int id);


class Mandelbrot : public Bitmap<int>
{
public: // good values are x=[-2..1], y=[-1,1]
  Mandelbrot(bool julia, float start_x, float end_x, float start_y, float end_y, int sx, int sy, int count, float xx, float yy) : julia(julia), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y),sx(sx), sy(sy), count(count), xx(xx), yy(yy) { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  int Map(int x, int y) const
  {
    if (julia)
      {
	Point2d z;
	z.x = start_x + x*(end_x-start_x)/sx;
	z.y = start_y + y*(end_y-start_y)/sy;
	Point2d C;
	C.x = xx;
	C.y = yy;
	return RecRec(count, z,C);
      }
    else
      {
	Point2d z;
	z.x = xx;
	z.y = yy;
	Point2d C;
	C.x = start_x + x*(end_x-start_x)/sx;
	C.y = start_y + y*(end_y-start_y)/sy;
	return RecRec(count, z,C);
      }
  }
private:
  Point2d Rec(Point2d z, Point2d C) const
  {
    return Add(Mul(z,z), C);
  }
  int RecRec(int count, Point2d z, Point2d C) const
  {
    int i=0;
    for(;i<count;i++)
      {
	z = Rec(z,C);
	if (Dist2(z)>2.0*2.0)
	  return i;
      }
    return i;
  }
  float Dist2(Point2d p1) const
  {
    return p1.x*p1.x+p1.y*p1.y;
  }
  Point2d Add(Point2d p1, Point2d p2) const
  {
    Point2d r;
    r.x = p1.x+p2.x;
    r.y = p1.y+p2.y;
    return r;
  }
  Point2d Mul(Point2d p1, Point2d p2) const
  {
    Point2d r;
    r.x = p1.x*p2.x-p1.y*p2.y;
    r.y = p1.y*p2.x+p1.x*p2.y;
    return r;
  }

private:
  bool julia;
  float start_x, end_x;
  float start_y, end_y;
  int sx,sy;
  int count;
  float xx,yy;
};

class MapBitmapToColor : public Bitmap<Color>
{
public:
  MapBitmapToColor(int value1, int value2,
		   Color c1, Color c2,
		   Bitmap<int> &bm)
    : value1(value1), value2(value2),
      c1(c1), c2(c2), bm(bm)
  {
  }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  Color Map(int x, int y) const
  {
    int val = bm.Map(x,y);
    float f = float(val-value1)/(value2-value1);
    return Color::Interpolate(c1,c2, 1.0-f);
  }

private:
  int value1, value2;
  Color c1,c2;
  Bitmap<int> &bm;
};

class Heightmap2 : public Array<Pos, Quad>
{
public:
  Heightmap2(Point center, Vector u_x, Vector u_y, int sx, int sy) : center(center), u_x(u_x), u_y(u_y), sx(sx), sy(sy) { }
  Pos Size() const
  {
    Pos p;
    p.x = sx-1;
    p.y = sy-1;
    return p;
  }
  Quad Index(Pos p) const
  {
    int x1 = p.x;
    int y1 = p.y;
    int x2 = p.x + 1;
    int y2 = p.y + 1;
    Point p11 = center + x1*u_x + y1*u_y;
    Point p21 = center + x2*u_x + y1*u_y;
    Point p12 = center + x1*u_x + y2*u_y;
    Point p22 = center + x2*u_x + y2*u_y;
    Quad q;
    q.p1 = p11;
    q.p2 = p21;
    q.p3 = p22;
    q.p4 = p12;
    return q;
  }
private:
  Point center;
  Vector u_x, u_y, v;
  int sx,sy;
  //int maxval;
};

#if 0
class RectSplitter
{
public:
  virtual int Count() const=0;
  virtual void SetParent(TRect parent)=0;
  virtual TRect GetRect() const=0;
  virtual Color GetPixel(Point2d p) const=0;
  virtual RectSplitter *Splitter(int val) const=0;
protected:
  bool InsideRect(TRect r, Point2d p)
  {
    if (p.x < r.x) return false;
    if (p.y < r.y) return false;
    if (p.x > r.x+r.width) return false;
    if (p.y > t.y+r.height) return false;
    return true;
  }
};
class BitmapRectSplitter : public RectSplitter
{
public:
  BitmapRectSplitter(ContinuousBitmap<Color> &vals) : vals(vals) { }
  virtual int Count() const { return 1; }
  virtual Rect Rects(int val, Rect parent) const
  {
    return parent;
  }
  Color Pixel(int val, Rect pos, Point2d p) const
  {
    if (InsideRect(pos,p))
      {
	p.x -= pos.x;
	p.y -= pos.y;
	p.x/=pos.width;
	p.y/=pos.height;
	p.x*=vals.SizeX();
	p.y*=vals.SizeY();
	return vals.Map(p.x,p.y);
      }
    return Color(0,0,0,0);
  }
private:
  ContinuousBitmap<Color> &vals;
};
class PosRectSplitter : public RectSplitter
{
public:
  PosRectSplitter(RectDelta *array, int size) : array(array), size(size) { }
  
private:
  RectDelta *array;
  int size;
};
#endif


class BitmapTextureSurfaceIn3d : public SurfaceIn3dImpl
{
public:
  BitmapTextureSurfaceIn3d(SurfaceIn3d &surf, Bitmap<class Color> &tex) : SurfaceIn3dImpl(surf), tex(tex) { }
  virtual unsigned int Color(float x, float y) const 
  {
    if (x<0.0 || y<0.0) { return 0x00000000; }
    float sx = XSize();
    float sy = YSize();
    x/=sx;
    y/=sy;
    x*=tex.SizeX();
    y*=tex.SizeY();
    std::cout << "Color:" << x << " " << y << std::endl;
    return tex.Map(x,y).Pixel();
  }

private:
  Bitmap<class Color> &tex;
};

class BitmapSphere : public SurfaceIn3d
{
public:
  BitmapSphere(ContinuousBitmap<float> &bm, Point center, float radius0, float radius1) : bm(bm), center(center), radius0(radius0), radius1(radius1) { }
  Point Index(float x, float y) const
  {
    Matrix m = Matrix::XRotation(x)*Matrix::YRotation(y);
    float xx = x*bm.SizeX()/(2.0*3.14159);
    float yy = y*bm.SizeY()/3.14159;
    float value = bm.Map(xx,yy);
    Vector v = Vector(0.0,0.0,radius0*(1.0-value)+radius1*value);
    Vector pp = v*m;
    return center+pp;
  }
  virtual float XSize() const { return 2.0*3.14159; }
  virtual float YSize() const { return 3.14159; }
  virtual Vector Normal(float x, float y) const
  {
    Vector vx = Index(x+0.01,y)-Index(x,y);
    Vector vy = Index(x,y+0.01)-Index(x,y);
    return -Vector::CrossProduct(vx,vy);
  }
  virtual Point2d Texture(float x, float y) const { Point2d p = { 0,0 }; return p; }
  virtual float Attrib(float x, float y, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual unsigned int Color(float x, float y) const { return 0; }

private:
  ContinuousBitmap<float> &bm;
  Point center;
  float radius0, radius1;
};

class ContinuousColorBitmapToFloatBitmap : public ContinuousBitmap<float>
{
public:
  ContinuousColorBitmapToFloatBitmap(ContinuousBitmap<Color> &bm) : bm(bm) { }
  virtual float SizeX() const { return bm.SizeX(); }
  virtual float SizeY() const { return bm.SizeY(); }
  virtual float Map(float x, float y) const
  {
    Color c = bm.Map(x,y);
    float val = (c.r+c.b+c.g)/3/255.0;
    return val;
  }
private:
  ContinuousBitmap<Color> &bm;
};

class HeightMap3Data : public Bitmap<float>
{
public:
};
class HeightMap3DataImpl : public HeightMap3Data
{
public:
  HeightMap3DataImpl(Bitmap<Color> &c) : c(c) { }
  int SizeX() const { return c.SizeX(); }
  int SizeY() const { return c.SizeY(); }
  float Map(int x, int y) const
  {
    Color cc = c.Map(x,y);
    int val = (cc.r + cc.g + cc.b)/3;
    float vv = float(val)/255.0;
    return vv;
  }
private:
  Bitmap<Color> &c;
};

class HeightMap3 : public Mesh
{
public:
  HeightMap3(Bitmap<float> &bm, 
	     float min_val, float max_val,
	     Point p,
	     Vector u_x,
	     Vector u_y, // u_y is the function's value
	     Vector u_z) : bm(bm), /*min_val(min_val), max_val(max_val),*/
			   p(p), u_x(u_x), u_y(u_y), u_z(u_z), cached_face(-1) 
  {
    float start_x = 0.0;
    float start_y = 0.0;
    float start_z = 0.0;
    float end_x = 0.0;
    float end_y = 0.0;
    float end_z = 0.0;
    Point p111(start_x, start_y, start_z);
    Point p112(start_x, start_y, end_z);
    Point p121(start_x, end_y, start_z);
    Point p122(start_x, end_y, end_z);
    Point p211(end_x, start_y, start_z);
    Point p212(end_x, start_y, end_z);
    Point p221(end_x, end_y, start_z);
    Point p222(end_x, end_y, end_z);

    elem = new CubeElem(p111,p112,p121,p122,
			p211,p212,p221,p222);
  }
  int NumStates() const { return 1; }
  int StateNum(int,int) const { return 0; }
  virtual int NumFrames() const { return 1; }
  virtual int NumFaces(int framenum) const { return bm.SizeX()*bm.SizeY()*elem->NumFaces(); }
  virtual int NumPoints() const
  {
    return elem->NumPoints(0);
  }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    int z = face / (bm.SizeX()*bm.SizeY());
    if (cached_face != face)
      {
	int ddz = face - z*(bm.SizeX()*bm.SizeY());
	int y = ddz / bm.SizeX();
	int ddy = ddz - y*bm.SizeX();
	int x = ddy;
	float val = bm.Map(x,y);

	Point tl = Pos(x,val,y);
	//Point br = Pos(x+1,val,y+1);
    //Point tla = Pos(x,0,y);
	Point bra = Pos(x+1,0,y+1);
	float start_x = tl.x;
	float start_y = tl.y;
	float start_z = tl.z;
	float end_x = bra.x;
	float end_y = bra.y;
	float end_z = bra.z;
	
	Point p111(start_x, start_y, start_z);
	Point p112(start_x, start_y, end_z);
	Point p121(start_x, end_y, start_z);
	Point p122(start_x, end_y, end_z);
	Point p211(end_x, start_y, start_z);
	Point p212(end_x, start_y, end_z);
	Point p221(end_x, end_y, start_z);
	Point p222(end_x, end_y, end_z);

	delete elem;
	elem = new CubeElem(p111,p112,p121,p122,
			    p211,p212,p221,p222);
	cached_face = face;

      }
    return elem->FacePoint(z,point);
  }

  Point Pos(int x, float y, int z) const
  {
    Point pos = p + float(x)*u_x/bm.SizeX() + y*u_y + float(z)*u_z/bm.SizeY();
    return pos;
  }

private:
  Bitmap<float> &bm;
  mutable CubeElem *elem;
  //float min_val, max_val;
  Point p;
  Vector u_x, u_y, u_z;
  mutable int cached_face;
};

class Visitor;
struct Loop
{
  int inverse;
  VectorArray<Point2d> *points;
  void visit(Visitor &v);
  void init() { inverse=false; points=new VectorArray<Point2d>; }
};


class Visitor
{
public:
  virtual void visit(bool &b)=0;
  virtual void visit(int &a)=0; // 0
  virtual void visit(float &b)=0; // 1
  virtual void visit(Point2d &p)=0; // 2
  virtual void visit(Point &p)=0; //3
  virtual void visit(VectorArray<Loop> *array)=0; // 4
  virtual void visit(VectorArray<Point2d> *array)=0; // 5
};


class SizePosVisitor : public Visitor
{
public:
  SizePosVisitor() : off(0) { }
  virtual void visit(bool &b)
  {
    offset.push_back(off); size.push_back(sizeof(bool));
    off += sizeof(bool);
    type.push_back(6);
  }
  virtual void visit(int &a) 
  { offset.push_back(off); size.push_back(sizeof(int)); off += sizeof(int); type.push_back(0); }
  virtual void visit(float &b) 
  { offset.push_back(off); size.push_back(sizeof(float)); off+= sizeof(float); type.push_back(1); }
  virtual void visit(Point2d &p) 
  { offset.push_back(off); size.push_back(sizeof(Point2d)); off+=sizeof(Point2d);type.push_back(2); }
  virtual void visit(Point &p)
  { offset.push_back(off); size.push_back(sizeof(Point)); off+=sizeof(Point); type.push_back(3); }
  virtual void visit(VectorArray<Loop> *array) 
  {
    offset.push_back(off); size.push_back(sizeof(VectorArray<Loop>*));
    off+=sizeof(VectorArray<Loop>*);
    type.push_back(4);
  }
  virtual void visit(VectorArray<Point2d> *array) 
  { 
    std::cout << array << std::endl;
    std::cout << this << std::endl;
    offset.push_back(off); size.push_back(sizeof(VectorArray<Point2d>*));
    off+=sizeof(VectorArray<Point2d>*);
    type.push_back(5);
  }

public:
  int Offset(int i) const { return offset[i]; }
  int Size(int i) const { return size[i]; }
  int Type(int i) const { return type[i]; }
private:
  std::vector<int> offset;
  std::vector<int> size;
  std::vector<int> type;
  int off;
};

class WriteVisitor : public Visitor
{
public:
  WriteVisitor(char *currentptr, int size) : currentptr(currentptr) /*, size(size) */{ }
  virtual void visit(int &a) 
  { int *ptr = (int*)currentptr; a = *ptr; currentptr+=sizeof(int); }
  virtual void visit(float &b)
  { float *ptr = (float*)currentptr; b = *ptr; currentptr+=sizeof(float); }
  virtual void visit(Point2d &p)
  { Point2d *ptr = (Point2d*)currentptr; p = *ptr; currentptr+=sizeof(Point2d); }
  virtual void visit(Point &p)
  { Point *ptr = (Point*)currentptr; p = *ptr; currentptr+=sizeof(Point); }
  //virtual void visit(Array<int,int> &array) { }
  //{ int *ptr = (int*)currentptr; a = *ptr; currentptr+=sizeof(int); }
  //virtual void visit(Array<int,float> &array) { }
  //{ int *ptr = (int*)currentptr; a = *ptr; currentptr+=sizeof(int); }
private:
  char *currentptr;
  //int size;
};

struct Path3d
{
  std::vector<Point> vec;
};
struct Path2d
{
  std::vector<Point2d> vec;
};


class Parser2 {
public:
  Parser2() : currentptr(0), size(0) { }
  int ParseInt(std::string s, bool &success);
  int ParseEnum(std::string *strings, int size, std::string s, bool &success);
  bool ParseBool(std::string s, bool &success);
  float ParseFloat(std::string s, bool &success);
  Point2d ParsePoint2d(std::string s, bool &success);
  Point ParsePoint(std::string s, bool &success);
  template<class T>
  VectorArray<T> *ParseArray(std::string s, bool &success);
  template<class T>
  T ParseStruct(std::string s, bool &success);

  Loop ParseLoop(std::string s, bool &success)
  {
    Loop l = ParseStruct<Loop>(s,success);
    return l;
  }
  Path3d ParsePath3d(std::string s, bool &success);
  Path2d ParsePath2d(std::string s, bool &success);
public: // these parse without {}'s.
  template<class T>
  T ParseStruct2(std::string s, bool &success);
  template<class T>
  VectorArray<T> *ParseArray2(std::string s, bool &success);
private:
  template<class T>
  T *Alloc()
  {
    //delete[] currentptr;
    T *obj = new T;
    currentptr = (char*)obj;
    //currentptr = new char[sizeof(T)];
    //new(currentptr)T;
    size = sizeof(T);
    return obj;
  }
private:
  char *currentptr;
  int size;
};

template<class T>
class TypeTraits
{
};
template<>
class TypeTraits<int>
{
public:
  typedef int (Parser2::*fptrtype)(std::string, bool &);
  static fptrtype fptr;
};


template<>
class TypeTraits<bool>
{
public:
  typedef bool (Parser2::*fptrtype)(std::string, bool &);
  static fptrtype fptr;
};




template<>
class TypeTraits<float>
{
public:
  typedef float (Parser2::*fptrtype)(std::string, bool &);
  static fptrtype fptr;
  //float (Parser2::*fptr)(std::string, bool &) = &Parser2::ParseFloat;
};

template<>
class TypeTraits<Point2d>
{
public:
  typedef Point2d (Parser2::*fptrtype)(std::string, bool &);
  static fptrtype fptr;

  //Point2d (Parser2::*fptr)(std::string, bool &) = &Parser2::ParsePoint2d;
};

template<>
class TypeTraits<Loop>
{
public:
  typedef Loop (Parser2::*fptrtype)(std::string, bool &);
  static fptrtype fptr;

  //Point2d (Parser2::*fptr)(std::string, bool &) = &Parser2::ParsePoint2d;
};


#if 0
template<>
class TypeTraits<Point>
{
public:
  Point (Parser2::*fptr)(std::string, bool &) = &Parser2::ParsePoint;
};
template<>
class TypeTraits<Array<int,int> >
{
};
template<>
class TypeTraits<Array<int,float> >
{
};
#endif


class ParseInterface
{
public:
  virtual void* Parse(std::string s, bool &success)=0;
};
template<class T>
class ParseInterfaceImpl : public ParseInterface
{
public:
  ParseInterfaceImpl(T (Parser2::*fptr)(std::string, bool &)) : fptr(fptr) { }
  void *Parse(std::string s, bool &success)
  {
    t = fptr(s, success);
    return &t;
  }
private:
  T (Parser2::*fptr)(std::string, bool &);
  void *res;
  T t;
};


struct TestStruct
{
  float x,y,z,k;
  int a,b,c,d;
  void visit(Visitor &v)
  {
    v.visit(x);
    v.visit(y);
    v.visit(z);
    v.visit(k);
    v.visit(a);
    v.visit(b);
    v.visit(c);
    v.visit(d);
  }
};

void Write(Parser2 *p, char *c, int size, int type, std::string s, bool &success)
;
std::string WhiteSpace(std::string s);

int Find(std::string s, char c);

void DumpMem(const char *c, int size);

template<class T>
T Parser2::ParseStruct2(std::string s_, bool &success)
{
  std::cout << "ParseStruct2: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  SizePosVisitor sizepos;
  
  T *ptr = Alloc<T>();
  ptr->init();
  ptr->visit(sizepos);

  int pos = Find(s,',');
  if (pos==-1)
    {
      std::cout << "ParseStruct2 find fail" << std::endl;
      success = false;
      return T();
    }
  int i = 0;
  char *cp = currentptr;
  std::string snd;
  while(pos != -1)
    {
      std::string fst = s.substr(0,pos);
      snd = s.substr(pos+1, s.length()-pos-1);
      pos = Find(snd, ',');
      int type = sizepos.Type(i);
      std::cout << "Write type: " << type << std::endl;
      Write(this, cp, sizepos.Size(i), type, fst, success);
      if (!success) { std::cout << "ParseStruct2 Write fail" << std::endl; return T(); }
      cp+=sizepos.Size(i);
      std::cout << "Advance by:" << sizepos.Size(i) << std::endl;
      //TypeTraits<Type>::fptr
      i++;
      s = snd;
    }
  int type = sizepos.Type(i);
  std::cout << "Write type2: " << type << std::endl;
  Write(this, cp, sizepos.Size(i), type, snd, success);
  cp+=sizepos.Size(i);
  std::cout << "Advance by2:" << sizepos.Size(i) << std::endl;
  std::cout << "Type: " << typeid(T).name() << std::endl;
  if (cp != currentptr+sizeof(T))
    {
      //std::cout << std::hex << (void*)cp << " " << std::dec << sizeof(T) << std::hex << " " << (void*)currentptr << std::dec << std::endl;
      std::cout << "Struct Alignment problem" << std::endl;
    }
  DumpMem(currentptr, sizeof(T));
  return *(T*)currentptr;
}

template<class T>
T Parser2::ParseStruct(std::string s_, bool &success)
{
  std::cout << "ParseStruct: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  if (s.length()<3) { std::cout << "ParseStruct length<3 fail: '" << s << "'" << std::endl; success = false; return T(); }
  if (s[0]=='{' && s[s.length()-1]=='}')
    {
      std::string ss = s.substr(1, s.length()-2);
      T t = ParseStruct2<T>(ss, success);
      return t;
    }
  else
    {
      std::cout << "ParseStruct {} fail" << std::endl;
      success = false;
      return T();
    }
}
template<class T>
VectorArray<T> *Parser2::ParseArray(std::string s_, bool &success)
{
  std::cout << "ParseArray: '" << s_ << "'" << std::endl;
  std::string s = WhiteSpace(s_);

  if (s.length()<3) { success = false; return 0; }
  if (s[0]=='{' && s[s.length()-1]=='}')
    {
      std::string ss = s.substr(1, s.length()-2);
      VectorArray<T> *t = ParseArray2<T>(ss, success);
      return t;
    }
  else
    {
      std::cout << "ParseArray {} fail" << std::endl;
      success = false;
      return 0;
    }
}

template<class T>
VectorArray<T> *Parser2::ParseArray2(std::string s_, bool &success)
{
  std::cout << "ParseArray2: '" << s_ << "'" << std::endl;
  std::string s = WhiteSpace(s_);

  T (Parser2::*fptr)(std::string, bool &) = TypeTraits<T>::fptr;
  int pos = Find(s,',');
  if (pos == -1)
    {
      T t = (this->*fptr)(s, success);
      if (!success) { std::cout << "ParseArray2 fail" << std::endl; return 0; }
      VectorArray<T> *vec = Alloc<VectorArray<T> >();
      vec->push_back(t);
      return vec;
    }
  std::string sbegin = s.substr(0,pos);
  std::string send = s.substr(pos+1, s.length()-1-pos);
  T t = (this->*fptr)(sbegin, success);
  if (!success) { std::cout << "ParseArray2 fail2" << std::endl; success = false; return 0; }
  VectorArray<T> *lst = ParseArray2<T>(send, success);
  if (!success) { std::cout << "ParseArray2 fail3" << std::endl; success = false; return 0; }

  lst->push_back(t);
  success = true;
  return lst;  
}


void FontParseTestCases();

//
// New interpolation for timing stuff
//

class IWaveform : public Function<float,float>
{
public:
  virtual float Start() const=0;
  virtual float End() const=0;
  virtual float Min() const=0;
  virtual float Max() const=0;
  virtual float Index(float val) const=0;
};

class IWaveFormArray : public Array<int, IWaveform*>
{
public:
  virtual float Map(float x) const=0;
};
class IDynWaveArray : public IWaveFormArray
{
public:
  IDynWaveArray(float (*fptr)(float,float)) : fptr(fptr) { }
  void push_back(IWaveform *wave) { vec.push_back(wave); }
  
  IWaveform* Index(int val) const
  {
    return vec[val];
  }
  float Map(float x) const
  {
    int s=vec.size();
    std::vector<IWaveform*> waves;
    for(int i=0;i<s;i++)
      {
	IWaveform *w = vec[i];
	if (x < w->Start()) continue;
	if (x > w->End()) continue;
	waves.push_back(w);
      }
    int s2 = waves.size();
    float val = vec[0]->Index(x);
    for(int i=1;i<s2;i++)
      {
	IWaveform *w1 = vec[i];
	float f1 = w1->Index(x);
	val = fptr(val, f1);
      }
    return val;
  }
private:
  //float start, end, min,max;
  std::vector<IWaveform*> vec;
  float (*fptr)(float,float);
};

class IWaveArrayArray : public Array<int, IWaveFormArray*>
{
public:
};

class IDynWaveArrayArray : public IWaveArrayArray
{
public:
  void push_back(IWaveFormArray* wave) { vec.push_back(wave); }
  
  IWaveFormArray* Index(int val) const
  {
    return vec[val];
  }
  float Map(int y, float x) const
  {
    return vec[y]->Map(x);
  }
private:
  std::vector<IWaveFormArray*> vec;
};

// 2 -> ZxZ -> AxB -> Ax(Z)B
template<class A, class B, class Z>
class BitmapPullback : public Function<std::pair<A,B>, bool>
{
public:
  BitmapPullback(ContinuousBitmap2<Z,bool> &Zz,
		 Function<A,Z> &f,
		 Function<B,Z> &g) : Zz(Zz),f(f),g(g) { }
  bool Index(std::pair<A,B> p) const
  {
    A a = p.first;
    B b = p.second;
    Z aa = f.Index(a);
    Z bb = g.Index(b);
    return Zz.Map(aa,bb);
  }
private:
  ContinuousBitmap2<Z,bool> &Zz;
  Function<A, Z> &f;
  Function<B, Z> &g;
};


class IntDiagonalBitmap : public ContinuousBitmap2<int, bool>
{
public:
  IntDiagonalBitmap(int sx, int sy) : sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual bool Map(int x, int y) const
  {
    return x==y;
  }
private:
  int sx, sy;
};

class DiagonalBitmap : public ContinuousBitmap2<float, bool>
{
public:
  DiagonalBitmap(float sx, float sy, float acc=0.001) : sx(sx), sy(sy), acc(acc) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual bool Map(float x, float y) const
  {
    return fabs(x-y)<acc;
  }
private:
  float sx,sy,acc;
};

template<class A, class B>
class Subset
{
public:
  Subset(Array<int, A> &enumeration,
	 Function<A,B> &subsetfunc,
	 Function<B,bool> &characteristic) : enumeration(enumeration), subsetfunc(subsetfunc), characteristic(characteristic) { }
  void HeavyLoop() {
    int s = enumeration.Size();
    for(int i=0;i<s;i++)
      {
	A a = enumeration.Index(i);
	B b = subsetfunc.Index(a);
	bool bb = characteristic(b);
	if (bb) vec_true.push_back(a);
	else vec_false.push_back(a);
      }
  }
public:
  std::vector<A> vec_true;
  std::vector<A> vec_false;
  
private:
  Array<int,A> &enumeration;
  Function<A,B> &subsetfunc;
  Function<B,bool> &characteristic;
};
  
template<class A, class B, class AA, class BB>
class BitmapIntersection : public Bitmap<bool>
{
public:
  BitmapIntersection(Array<int,A> &enum1,
		     Array<int,B> &enum2,
		     Function<A,AA> &subset1,
		     Function<B,BB> &subset2,
		     Function<AA,bool> &char1,
		     Function<BB,bool> &char2) : enum1(enum1),
						 enum2(enum2),
						 subset1(subset1),
						 subset2(subset2),
						 char1(char1),
						 char2(char2) { }
public:
  int SizeX() const { return enum1.Size(); }
  int SizeY() const { return enum2.Size(); }
  bool Map(int x, int y) const
  {
    A a = enum1.Index(x);
    B b = enum2.Index(y);
    AA aa = subset1.Index(a);
    BB bb = subset2.Index(b);
    bool ba = char1.Index(aa);
    bool bm = char1.Index(bb);
    return ba&&bm;
  }
private:
  Array<int,A> &enum1;
  Array<int,B> &enum2;
  Function<A,AA> &subset1;
  Function<B,BB> &subset2;
  Function<AA,bool> &char1;
  Function<BB,bool> &char2;
};
  
template<class T>
class BitmapEnumeration : public Array<int,std::pair<int,int> >
{
public:
  BitmapEnumeration(Bitmap<T> &bm) : bm(bm) { }
  int Size() const
  {
    return bm.SizeX()*bm.SizeY();
  }
  std::pair<int,int> Index(int i) const
  {
    int sx = bm.SizeX();
    int iy = i/sx;
    int ix = i-(sx*iy);
    return std::make_pair(ix,iy);
  }
private:
  Bitmap<T> &bm;
};

template<class A>
class BitmapSubset : public Function<std::pair<int,int>, A>
{
public:
  BitmapSubset(Bitmap<A> &bm) : bm(bm) { }
  A Index(std::pair<int,int> p) const
  {
    return bm.Map(p.first,p.second);
  }
private:
  Bitmap<A> &bm;
};

class Sphere : public Function<Point,bool>
{
public:
  Sphere(Point center, float radius) : center(center), radius(radius) { }
  bool Index(Point p) const
  {
    p-=center;
    return p.x*p.x+p.y*p.y+p.z*p.z<radius*radius;
  }
  float Radius() const { return radius; }
  Point Center() const { return center; }
private:
  Point center;
  float radius;
};
#if 0
template<class T, class A, class B, class AB>
class Intersection
{
public:
  
};

#endif


class RenderPolyToBitmap : public Bitmap<Color>
{
public:
  RenderPolyToBitmap(Point2d center_in_bm, Point center, int sx, int sy, FaceCollection *coll, Bitmap<Color> &bg) : center(center), sx(sx), sy(sy), coll(coll),bg(bg) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    x -= center_in_bm.x;
    y -= center_in_bm.y;
    int faces = coll->NumFaces();
    for(int f=0;f<faces;f++)
      {
	int points = coll->NumPoints(f);
	DynPointCollection2d pointcol;
	for(int p=0;p<points;p++) 
	  {
	    Point pos = coll->FacePoint(f,p);
	    pos+=Vector(center);
	    Point2d p2 = Perspective(pos);
	    pointcol.push_back(p2);
	  }
	while(points<4) { pointcol.push_back(pointcol.Index(points-1)); points++; }
	TriangleContinuousBitmap bm(sx,sy, pointcol.Index(0), pointcol.Index(1), pointcol.Index(2));
	TriangleContinuousBitmap bm2(sx,sy,pointcol.Index(0), pointcol.Index(2), pointcol.Index(3));
	bool b = bm.Map(x,y);
	bool b2 = bm2.Map(x,y);
	if (!b && !b2) return bg.Map(x,y);
	int i1,i2,i3;
	if (b) {
	  i1=0;i2=1;i3=2;
	} else {
	  i1=0;i2=2;i3=3;
	}
	TriangleCoordsContinuousBitmap color_val_r(sx,sy, pointcol.Index(i1),pointcol.Index(i2),pointcol.Index(i3), Color(coll->Color(f,i1)).r, Color(coll->Color(f,i2)).r, Color(coll->Color(f,i3)).r);      
	TriangleCoordsContinuousBitmap color_val_g(sx,sy, pointcol.Index(i1),pointcol.Index(i2),pointcol.Index(i3), Color(coll->Color(f,i1)).g, Color(coll->Color(f,i2)).g, Color(coll->Color(f,i3)).g);      
	TriangleCoordsContinuousBitmap color_val_b(sx,sy, pointcol.Index(i1),pointcol.Index(i2),pointcol.Index(i3), Color(coll->Color(f,i1)).b, Color(coll->Color(f,i2)).b, Color(coll->Color(f,i3)).b);
	TriangleCoordsContinuousBitmap color_val_a(sx,sy, pointcol.Index(i1),pointcol.Index(i2),pointcol.Index(i3), Color(coll->Color(f,i1)).alpha, Color(coll->Color(f,i2)).alpha, Color(coll->Color(f,i3)).alpha);

	TriangleCoordsContinuousBitmap tex_val_tx(sx,sy, pointcol.Index(i1), pointcol.Index(i2), pointcol.Index(i3), coll->TexCoord(f,i1).x, coll->TexCoord(f,i2).x, coll->TexCoord(f,i3).x);
	TriangleCoordsContinuousBitmap tex_val_ty(sx,sy, pointcol.Index(i1), pointcol.Index(i2), pointcol.Index(i3), coll->TexCoord(f,i1).y, coll->TexCoord(f,i2).y, coll->TexCoord(f,i3).y);

	return TexMap(f, color_val_r.Map(x,y), color_val_g.Map(x,y),
		      color_val_b.Map(x,y), color_val_a.Map(x,y),
		      tex_val_tx.Map(x,y), tex_val_ty.Map(x,y));	
      }
    Color c;
    return c;
  }
  virtual Color TexMap(int face, float r, float g, float b, float a, float tx, float ty) const
  {
    int tex = coll->FaceTexture(face);
    if (tex==-1)
      return Color(r,g,b,a);
    else
      {
	BufferRef texture = coll->TextureBuf(tex);
	return Color::Interpolate(Color(r,g,b,a), Color(texture.buffer[(int)tx+((int)ty)*texture.ydelta]), 0.5);
      }
  }
  virtual Point2d Perspective(Point p) const
  {
    Point2d pp = { p.x, p.y };
    return pp;
  }
private:
  Point2d center_in_bm;
  Point center;
  int sx,sy;
  FaceCollection *coll;
  Bitmap<Color> &bg;
};

//
// Texture support
//
class TextureI
{
public:
  virtual int SizeX() const { return 0; }
  virtual int SizeY() const { return 0; }
  virtual Color Map(int x, int y) const { return Color::Transparent(); }
  virtual int AreaCount() const  { return 0; }
  virtual int Id(int i) const { return 0; }
  virtual Point2d AreaS(int i) const { Point2d p; p.x = 0.0;  return p; }
  virtual Point2d AreaE(int i) const { Point2d p; p.x = 0.0; return p; }
};

class TextureIBitmap : public Bitmap<Color>
{
public:
  TextureIBitmap(TextureI &ti) : ti(ti) { }
  virtual int SizeX() const { return ti.SizeX(); }
  virtual int SizeY() const { return ti.SizeY(); }
  virtual Color Map(int x, int y) const { return ti.Map(x,y); }  
private:
  TextureI &ti;
};

class TexPlane : public TextureI
{
public:
  TexPlane(int sx, int sy) : sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const { return Color::Transparent(); }
private:
  int sx,sy;
};

class TexBitmap : public TextureI
{
public:
  TexBitmap(Bitmap<Color> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const { return bm.Map(x,y); }
private:
  Bitmap<Color> &bm;
};

class TexAssign : public TextureI
{
public:
  TexAssign(TextureI &next, int id, int x, int y, Bitmap<Color> &bm)
    : next(next), id(id), x(x), y(y), bm(bm) { }
  virtual int SizeX() const { return next.SizeX(); }
  virtual int SizeY() const { return next.SizeY(); }
  virtual Color Map(int x, int y) const 
  {
    int s = AreaCount();
    for(int i=0;i<s;i++)
      {
	Point2d p1 = AreaS(i);
	Point2d p2 = AreaE(i);
	if (x<p1.x) continue;
	if (y<p1.y) continue;
	if (x>=p2.x) continue;
	if (y>=p2.y) continue;
	return bm.Map(x-p1.x,y-p1.y);
      }
    return next.Map(x,y); 
  }
  virtual int AreaCount() const  { return next.AreaCount()+1; }
  virtual int Id(int i) const 
  {
    if (i==0) { return id; }
    return next.Id(i-1);
  }
  virtual Point2d AreaS(int i) const 
  {
    if (i==0) {
      Point2d p;
      p.x = (float)x;
      p.y = (float)y;
      return p;
    }
    return next.AreaS(i-1);
  }
  virtual Point2d AreaE(int i) const { 
    if (i==0) {
      Point2d p;
      p.x = (float)(x+bm.SizeX());
      p.y = (float)(y+bm.SizeY());
      return p;
    }
    return next.AreaS(i-1);
  }
  
private:
  TextureI &next;
  int id;
  int x,y;
  Bitmap<Color> &bm;
};

class TextureITexCoord : public TextureI
{
public:
  TextureITexCoord(TextureI &next, int id, int x, int y, int width, int height)
    : next(next), id(id), x(x), y(y), width(width), height(height) { }
  virtual int SizeX() const { return next.SizeX(); }
  virtual int SizeY() const { return next.SizeY(); }
  virtual Color Map(int x, int y) const { return next.Map(x,y); }

  virtual int AreaCount() const  { return next.AreaCount()+1; }
  virtual int Id(int i) const 
  {
    if (i==0) { return id; }
    return next.Id(i-1);
  }
  virtual Point2d AreaS(int i) const 
  {
    if (i==0) {
      Point2d p;
      p.x = (float)x;
      p.y = (float)y;
      return p;
    }
    return next.AreaS(i-1);
  }
  virtual Point2d AreaE(int i) const { 
    if (i==0) {
      Point2d p;
      p.x = (float)(x+width);
      p.y = (float)(y+height);
      return p;
    }
    return next.AreaS(i-1);
  }
private:
  TextureI &next;
  int id; 
  int x; int y; 
  int width; int height;
};


//
// PLANE
//
class PlanePoints2d
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual int Size() const=0;
  virtual Point2d Map(int i) const=0;
  //virtual Point2d Map2(int i) const=0;
  enum PlanePointsType { EMove, ELineTo, ECubic,EConic };
  virtual PlanePointsType Type(int i) const { if (i==0) return EMove; else return ELineTo; }
};

#if 0
class FloodFillBorder : public PlanePoints2d
{
public:
  FloodFillBorder(Bitmap<bool> &bm, int x, int y) :bm(bm), x(x),y(y) { }
  void run() {
  }
  virtual float SizeX() const { return bm.SizeX(); }
  virtual float SizeY() const { return bm.SizeY(); }
  virtual int Size() const { return vec.size(); }
  virtual Point2d Map(int i) const { return vec[i]; }

private:
  Bitmap<bool> &bm;
  int x,y;
  std::vector<Point2d> vec;
};

#endif

//
// FBO
//

//
// FRAMEBUFFER OBJECTS
//
class FBO : public Bitmap<Color>
{
public:
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    if (x>=0&&x<sx)
      if (y>=0&&y<sy)
	return ref.buffer[x+y*ref.ydelta];
    return Color::Transparent();
  }
  FBO(int sx, int sy);
  ~FBO();
  void bind();
  void unbind();
  void update(); // swapbuffers
private:
  unsigned int ids[1];
  unsigned int rbo[1];
  BufferRef ref;
  int sx,sy;
};

class BitmapVolume : public VolumeObject
{
public:
  BitmapVolume(Bitmap<bool> *b, float dist) : b(b), dist(dist) { }
  virtual bool Inside(Point v) const {
    if (b->Map(v.x,v.y))
      {
	return true;
      }
    if (v.z>0.0 && v.z<dist) { return true; }
    return false;
  }
private:
  Bitmap<bool> *b;
  float dist;
};

class Layout
{
public:
  virtual int count() const=0;
  virtual TRect get(int i) const=0;
};

class SplitXYLayout : public Layout
{
public:
  SplitXYLayout(Layout &l, int id, int num_x, int num_y) : l(l), id(id), num_x(num_x), num_y(num_y) { }
  virtual int count() const { return num_x*num_y; }
  virtual TRect get(int i) const
  {
    int xx = i/num_y;
    int yy = i-xx*num_y;
    TRect r = l.get(id);
    r.width /= num_x;
    r.height /= num_y;
    r.x += r.width*xx;
    r.y += r.height*yy;
    return r;
  }
private:
  Layout &l;
  int id;
  int num_x, num_y;
};

class CenterLayout : public Layout
{
public:
  CenterLayout(Layout &l, int id, int cx, int cy) : l(l), id(id), cx(cx), cy(cy) { }
  virtual int count() const { return 1; }
  virtual TRect get(int i) const
  {
    TRect r = l.get(id);
    int ccx = r.x+r.width/2;
    int ccy = r.y+r.height/2;
    r.x = ccx-cx;
    r.y = ccy-cy;
    r.width = cx;
    r.height = cy;
    return r;
  }
  
private:
  Layout &l;
  int id;
  int cx,cy;
};

class ArrayLayout : public Layout
{
public:
  ArrayLayout(Layout **layout, int *id, int size) : layout(layout), id(id), size(size) { }
  virtual int count() const { return size; }
  virtual TRect get(int i) const
  {
    TRect r = layout[i]->get(id[i]);
    return r;
  }
private:
  Layout **layout;
  int *id;
  int size;
};

class MarginLayout : public Layout
{
public:
  MarginLayout(Layout &l, int id, int lx, int rx, int ty, int by) : l(l), id(id), lx(lx), rx(rx), ty(ty), by(by) { }
  virtual int count() const { return 1; }
  virtual TRect get(int i) const
  {
    TRect r = l.get(id);
    r.x+=lx;
    r.y+=ty;
    r.width-=rx+lx;
    r.height-=by+ty;
    return r;
  }
private:
  Layout &l;
  int id;
  int lx;
  int rx;
  int ty;
  int by;
};

class SplitLayoutY : public Layout
{
public:
  SplitLayoutY(Layout &l, int id, int num) : l(l), id(id), num(num) { }
  virtual int count() const { return num; }
  virtual TRect get(int i) const
  {
    TRect p = l.get(id);
    float k = p.height/num;
    p.y+=k*i;
    p.height = k;
    return p;
  }
private:
  Layout &l;
  int id;
  int num;
};

class SplitLayoutX : public Layout
{
public:
  SplitLayoutX(Layout &l, int id, int num) : l(l), id(id), num(num) { }
  virtual int count() const { return num; }
  virtual TRect get(int i) const
  {
    TRect p = l.get(id);
    float k = p.width/num;
    p.x+=k*i;
    p.width = k;
    return p;
  }
private:
  Layout &l;
  int id;
  int num;
};


class RootLayout : public Layout
{
public:
  RootLayout(int sx, int sy) : sx(sx),sy(sy) { }
  virtual int count() const { return 1; }
  virtual TRect get(int i) const
  {
    TRect r;
    r.x = 0;
    r.y = 0;
    r.width = sx;
    r.height= sy;
    return r;
  }
private:
  int sx,sy;
};


class Draw
{
public:
  virtual int type() const=0;
  virtual Point2d topleft() const=0;
  virtual Point2d bottomright() const=0;
  virtual int propcount() const=0;
  virtual std::string propname(int i) const=0;
  virtual std::string proptype(int i) const=0;
  virtual void* probvalue(int i) const=0;

  virtual int ArrayCount() const=0;
  virtual Draw* ArrayIndex(int i) const=0;
  
  virtual int AlArrayCount() const=0;
  virtual Draw* AlArrayIndex(int i) const=0;
};
#if 0

class DrawScroll : public Draw
{
public:
  DrawScroll(Draw &next, int delta_x, int delta_y) : next(next), delta_x(delta_xx), delta_y(delta_y) { }
  virtual int type() const { return next.type(); }
  virtual Point2d topleft() const { return next.topleft(); }
  virtual Point2d bottomright() const { return next.bottomright(); }
  virtual int propcount() const { return next.propcount(); }
  virtual std::string propname(int i) const { return next.propname(i); }
  virtual std::string proptype(int i) const { return next.proptype(i); }
  virtual void* propvalue(int i) const { return next.propvalue(i); }

  virtual int ArrayCount() const { return next.ArrayCount(); }
  virtual Draw* ArrayIndex(int i) const
  {
    return next.ArrayIndex(i);
  }
  
  virtual int AlArrayCount() const { return next.AlArrayCount(); }
  virtual Draw* AlArrayIndex(int i) const
  {
    reutrn next.AlArrayIndex(i);
  }
private:
  Draw &next;
  int delta_x, delta_y;
};
class DrawEmpty : public Draw
{
public:
  DrawEmpty(int type, Point2d tl, Point2d br) : type(type), tl(tl), br(br) { }
  virtual int type() const { return type; }
  virtual Point2d topleft() const { return tl; }
  virtual Point2d bottomright() const { return br; }
  virtual int propcount() const { return 0; }
  virtual std::string propname(int i) const { return ""; }
  virtual std::string proptype(int i) const { return ""; }
  virtual void* probvalue(int i) const { return 0; }

  virtual int ArrayCount() const { return 0; }
  virtual Draw* ArrayIndex(int i) const { return 0; }
  
  virtual int AlArrayCount() const { return 0; }
  virtual Draw* AlArrayIndex(int i) const { return 0; }
private:
  int type;
  Point2d tl,br;
};

template<class T>
class DrawPropTrait { };
template<>
class DrawPropTrait<float> { static const std::string s = "float"; };
template<>
class DrawPropTrait<int> { static const std::string s = "int"; };

template<class T>
class DrawProp : public Draw
{
public:
  DrawProp(Draw &next, std::string name, T t) : next(next), name(name), type(DrawPropTrait<T>::s), t(t) { }
  virtual int type() const { return next.type(); }
  virtual Point2d topleft() const { return next.topleft(); }
  virtual Point2d bottomright() const { return next.bottomright(); }

  virtual int propcount() const { return next.propcount()+1; }
  virtual std::string propname(int i) const
  {
    if (i==0) return name; else return next.propname(i-1);
  }
  virtual std::string proptype(int i) const
  {
    if (i==0) return type; else return next.proptype(i-1);
  }
  virtual void* probvalue(int i) const
  {
    if (i==0) return &t; else return next.propvalue(i-1);
  }
  virtual int ArrayCount() const { return next.ArrayCount(); }
  virtual Draw* ArrayIndex(int i) const { return next.ArrayIndex(i); }
  
  virtual int AlArrayCount() const { return next.AlArrayCount(); }
  virtual Draw* AlArrayIndex(int i) const { return next.AlArrayIndex(i); }

private:
  Draw &next;
  std::string name;
  std::string type;
  T t;
};

class DrawArray : public Draw
{
public:
  DrawArray(Draw &next, Draw *array, int size) : next(next) { }
  virtual int type() const { return next.type(); }
  virtual Point2d topleft() const { return next.topleft(); }
  virtual Point2d bottomright() const { return next.bottomright(); }
  virtual int propcount() const { return next.propcount(); }
  virtual std::string propname(int i) const { return next.propname(i); }
  virtual std::string proptype(int i) const { return next.proptype(i); }
  virtual void* probvalue(int i) const { return next.propvalue(i); }

  virtual int ArrayCount() const { return size; }
  virtual Draw* ArrayIndex(int i) const { return &array[i]; }
  
  virtual int AlArrayCount() const { return next.AlArrayCount(); }
  virtual Draw* AlArrayIndex(int i) const { return next.AlArrayIndex(i); }
private:
  Draw &next;
  Draw *array;
  int size;
};

class DrawAlArray : public Draw
{
public:
  DrawAlArray(Draw &next, Draw *array, int size) : next(next) { }
  virtual int type() const { return next.type(); }
  virtual Point2d topleft() const { return next.topleft(); }
  virtual Point2d bottomright() const { return next.bottomright(); }
  virtual int propcount() const { return next.propcount(); }
  virtual std::string propname(int i) const { return next.propname(i); }
  virtual std::string proptype(int i) const { return next.proptype(i); }
  virtual void* probvalue(int i) const { return next.propvalue(i); }

  virtual int ArrayCount() const { return next.ArrayCount(); }
  virtual Draw* ArrayIndex(int i) const { return next.ArrayIndex(i); }
  
  virtual int AlArrayCount() const { return size; }
  virtual Draw* AlArrayIndex(int i) const { return &array[i]; }
private:
  Draw &next;
  Draw *array;
  int size;
};
#endif


class DistanceRenderable
{
public:
  virtual float distance(Point p) const=0;
  virtual std::string shader() const=0;
  virtual int varnum() const=0;
};


class PointsApiPoints
{
public:
  virtual int NumPoints() const=0;
  virtual Point Pos(int i) const=0;
  virtual unsigned int Color(int i) const=0;
};

class SpacePoints : public PointsApiPoints
{
public:
  SpacePoints(VolumeObject &v, Point p, Vector u_x, Vector u_y, Vector u_z,
	      int sx, int sy, int sz) : v(v), p(p), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx), sy(sy), sz(sz) { Create(); }
  void Create() {
    for(int i=0;i<sx;i++)
      {
	Point pp = p + u_x*i/sx;
      for(int j=0;j<sy;j++)
	{
	  Vector pp2 = u_y*j/sy;
	for(int k=0;k<sz;k++)
	  {
	    Vector pp3 = u_z*k/sz;
	    Point p = pp + pp2 + pp3;
	    bool b = v.Inside(p);
	    if (b) { vec.push_back(p); }
	  }
	}
      }
  }
  virtual int NumPoints() const { return vec.size(); }
  virtual Point Pos(int i) const { return vec[i]; }
  virtual unsigned int Color(int i) const { return 0xffffffff; }
private:
  VolumeObject &v;
  Point p;
  Vector u_x, u_y, u_z;
  int sx, sy, sz;
  std::vector<Point> vec;
};



#endif
