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



#ifndef WIDGETS_HH
#define WIDGETS_HH

#include "Effect.hh"
#include "Graph.hh"
#include <cassert>


class EventSurface
{
public:
  virtual void HandleKey(int key)=0;
  virtual void HandlePointer(int button)=0;
  virtual void HandlePointerMove(Point2d p)=0;
  virtual void HandleKeyDown(int key) { }
  virtual void HandleKeyUp(int key) { }
  virtual void HandleFrame(float time) { }
};
class EmptyEventSurface : public EventSurface
{
public:
  void HandleKey(int key) {std::cout << "Key: " << key << std::endl; }
  void HandlePointer(int button) { std::cout << "Pointer: " << button << std::endl; }
  void HandlePointerMove(Point2d p) { std::cout << "Move: " << p.x << "," << p.y << std::endl; }
};

struct Position2d
{
  Point2d p;
};

class MovePointEventSurface : public EventSurface
{
public:
  MovePointEventSurface(Position2d &p, Point2d tl, Point2d br, const float &speed) :p(p), tl(tl), br(br), speed(speed), moveleft(false), moveright(false), moveup(false), movedown(false)
  {
    p.p.x = (tl.x+br.x)/2;
    p.p.y = (tl.x+br.x)/2;
  }
  void HandleKeyDown(int key) 
  { 
    switch(key)
      {
      case 273: moveup = true; break;
      case 274: movedown = true; break;
      case 275: moveright = true; break;
      case 276: moveleft = true; break;
      }
    //std::cout << key << " DOWN" << std::endl; 
  }
  void HandleKeyUp(int key) 
  { 
    switch(key)
      {
      case 273: moveup = false; break;
      case 274: movedown = false; break;
      case 275: moveright = false; break;
      case 276: moveleft = false; break;
      }
    //std::cout << key << " Up" << std::endl; 
  }
  void HandlePointer(int button) { }
  void HandlePointerMove(Point2d p) { }
  void HandleKey(int key) { }
  void HandleFrame(float /*time*/)
  {
    float speed2 = speed;
    if (moveup && moveleft) { speed2 /= sqrt(2); }
    else if (moveup && moveright) { speed2 /= sqrt(2); }
    else if (movedown && moveleft) { speed2 /= sqrt(2); }
    else if (movedown && moveright) { speed2 /= sqrt(2); }
    if (moveup) { p.p.y -= speed2; }
    if (movedown) { p.p.y += speed2; }
    if (moveleft) { p.p.x -= speed2; }
    if (moveright) { p.p.x += speed2; }
    if (p.p.y < tl.y) p.p.y = tl.y;
    if (p.p.y > br.y) p.p.y = br.y;
    if (p.p.x < tl.x) p.p.x = tl.x;
    if (p.p.x > br.x) p.p.x = br.x;
    //std::cout << "FramePos: " << p.p.x << " " << p.p.y << std::endl;
  }
private:
  Position2d &p;
  Point2d tl,br;
  const float &speed;
  bool moveleft;
  bool moveright;
  bool moveup;
  bool movedown;
};

class EventSurface;
void Execute2(FrameAnim &f);
void Execute2(FrameAnim &f, EventSurface &surf);

class DrawableArea
{
public:
  
};


//template<class T, class D>
//class Tree
//{
//public:
//};

template<class T, class D, class DC, class DR>
class Grid
{
public:
};

class DataType
{
public:
  virtual float XCount() const=0;
  virtual float YCount() const=0;
  virtual bool get_val(float x, float y) const=0;
  virtual void set_val(float x, float y, bool b)=0;
  virtual const Region &InvalidRects() const=0;
  virtual void ClearInvalidRects()=0;
};
template<class T>
class ArrayDataType : public DataType
{
public:
  ArrayDataType(VectorArray<T> &arr, Range<T> &r) : arr(arr),r(r), convert(rects) 
  {
    Rect rr;
    rr.tl.x = 0;
    rr.br.x = arr.Size();
    rr.tl.y = float(r.start);
    rr.tl.y = float(r.end);
    rects.push_back(rr);
  }
  float XCount() const { return arr.Size(); }
  float YCount() const { return float(r.end) - float(r.start); }
  void ClearInvalidRects()
  {
    rects.clear();
  }
  const Region &InvalidRects() const
  {
    return convert;
  }
  bool get_val(float x, float y) const
  {
    assert(x>=0.0);
    assert(x<arr.Size());
    assert(y>=0.0);
    assert(y<float(r.end)-float(r.start));
    y+=r.start;
    bool b = arr.Index(x) < T(y);
    //std::cout << "get_val "<< x << "," << y << ":" << b << std::endl;
    return b;
  }
  void set_val(float x, float y, bool b)
  {
    if (b)
      {
	if (x >= 0.0 && x < arr.Size())
	  {
	    arr[x] = r.start + T(y);
	    float old_val = r.start;
	    float new_val = r.end;
	    float old_x = x;
	    float new_x = x+1;
	    Rect r;
	    r.tl.x = old_x;
	    r.br.x = new_x;
	    r.tl.y = old_val;
	    r.br.y = new_val;
	    //std::cout << "SetVal: " << old_x << " " << new_x << " " << old_val << " " << new_val << std::endl;
	    rects.push_back(r);
	  }
	else
	  std::cout << "ArrayDataType: Error, array index! " << std::endl;
      }
  }
private:
  VectorArray<T> &arr;
  Range<T> &r;
  VectorArray<Rect> rects;
  RegionConvert convert;
};

class Point2dArrayDataType : public DataType
{
public:
  Point2dArrayDataType(VectorArray<Point2d> &arr, Rect r) : arr(arr), r(r), convert(rects) 
  {
    rects.push_back(r);
  }
  float XCount() const { return r.br.x - r.tl.x; }
  float YCount() const { return r.br.y - r.tl.y; }
  virtual const Region &InvalidRects() const { return convert; }
  virtual void ClearInvalidRects() { rects.clear(); }
  bool get_val(float x, float y) const
  {
    x += r.tl.x;
    y += r.tl.y;
    for(int i=0;i<arr.Size();i++)
      {
	Point2d p = arr.Index(i);
	float dx = x - p.x;
	float dy = y - p.y;
	float dist = sqrt(dx*dx + dy*dy);
	if (dist<(r.br.x-r.tl.x)/140.0) return true;
      }
    return false;
  }
  void set_val(float x, float y, bool b)
  {
    x += r.tl.x;
    y += r.tl.y;
    if (b)
      {
	Point2d t;
	t.x = x;
	t.y = y;
	arr.push_back(t);
	Rect rr;
	rr.tl.x = x-(r.br.x-r.tl.x)/140.0;
	rr.tl.y = r.tl.y;
	rr.br.x = x+(r.br.x-r.tl.x)/140.0;
	rr.br.y = r.br.y;
	rects.push_back(rr);
      }
    else
      {
	// TODO, remove point from array
      }
  }
private:
  VectorArray<Point2d> &arr;
  Rect r;
  VectorArray<Rect> rects;
  RegionConvert convert;
};


class DataTypeContainer : public Array<int, DataType*>
{
};

class RectFrameAnim : public FrameAnim
{
public:
  RectFrameAnim(Rect r, FrameAnim &obj, Rect screen, Render *rr) : FrameAnim(rr), r(r), obj(obj), screenx(screen.br.x), screeny(screen.br.y) { }
  void Init() { obj.Init(); }
  void Cleanup() { obj.Cleanup(); }
  void PreFrame(float time) { obj.PreFrame(time); }
  bool Frame(float time);
  void PostFrame() { obj.PostFrame(); }
private:
  Rect r;
  FrameAnim &obj;
  float screenx, screeny;
};
class RotateFrameAnim : public FrameAnim
{
public:
  RotateFrameAnim(FrameAnim &next, Render *r) : FrameAnim(r), next(next) { }
  void Init() 
  {
    next.Init();
  }
  void Cleanup()
  {
    next.Cleanup();
  }  
  void PreFrame(float time)
  {
    next.PreFrame(time);
  }
  bool Frame(float time);
  void PostFrame() 
  {
    next.PostFrame();
  }
private:
  FrameAnim &next;
};

class VectorFrameAnim : public FrameAnim
{
public:
  template<class It>
  VectorFrameAnim(It beg, It end) : vec(beg, end) { }
  typedef std::vector<FrameAnim*>::iterator iterator;
  void Init() 
  {
    for(iterator i = vec.begin();i!=vec.end();i++)
      {
	(*i)->Init();
      }
  }
  void Cleanup()
  {
    for(iterator i = vec.begin();i!=vec.end();i++)
      {
	(*i)->Cleanup();
      }
  }
  void PreFrame(float time)
  {
    for(iterator i = vec.begin();i!=vec.end();i++)
      {
	(*i)->PreFrame(time);
      }
  }
  bool Frame(float time)
  {
    for(iterator i = vec.begin();i!=vec.end();i++)
      {
	(*i)->Frame(time);
      }
    return false;
  }
  void PostFrame() 
  {
    for(iterator i = vec.begin();i!=vec.end();i++)
      {
	(*i)->PostFrame();
      }
  }

private:
  std::vector<FrameAnim*> vec;
};

class DataTypeArray : public Array<std::pair<float, float>, bool>
{
public:
  DataTypeArray(DataType &dt) : dt(dt) { }
  std::pair<float,float> Size() const 
  { 
    //std::cout << "DataTypeArray: " << dt.XCount() << " " << dt.YCount() << std::endl;
    return std::make_pair(dt.XCount(), dt.YCount()); }
  bool Index(std::pair<float, float> p) const
  {
    return dt.get_val(p.first, p.second);
  }
private:
  DataType &dt;
};
class SamplePairArray : public Array<std::pair<int,int>, bool>
{
public:
  SamplePairArray(Array<std::pair<float,float>, bool> &func, int x, int y) : func(func), x(x), y(y) { }
  std::pair<int,int> Size() const { return std::make_pair(x,y); }
  bool Index(std::pair<int, int> p) const
  {
    std::pair<float,float> pp = func.Size();
    float xs = pp.first;
    float ys = pp.second;
    float xx = Sample(p.first, x, xs);
    float yy = Sample(p.second, y, ys);
    //std::cout << "SamplePairFunction: " << p.second << " " << y << " " << ys << std::endl;
    return func.Index(std::make_pair(xx,yy));
  }
  static float Sample(int x2, int intsize, float size)
  {
    return float(x2)/float(intsize)*size;
  }
private:
  Array<std::pair<float,float>, bool> &func;
  int x,y;
};


class DataTypeBitmap
{
public:
  DataTypeBitmap(DataType &dt, int x, int y) : func(dt), func2(func, x,y), bitmap(func2) { }
  void Generate(Rect r) { return bitmap.Generate(r); }
  BufferRef Ref() const { return bitmap.Ref(); }
private:
  DataTypeArray func;
  SamplePairArray func2;
  FunctionBitmap bitmap;
};

class Widget : public EventSurface, public FrameAnim
{
public:
  Widget(Render *r) : FrameAnim(r) { }
};

class PlaneRayIntersection : public Element<std::pair<Point2d, float> >
{
public:
  PlaneRayIntersection(Point p, Vector v, Plane pl) : p(p), v(v), pl(pl) { }
  std::pair<Point2d, float> Elem() const
  {
    Point l1 = p;
    Point l2 = p + v;
    Point p1 = pl.u_p;
    Point p2 = pl.u_p + pl.u_x;
    Point p3 = pl.u_p + pl.u_y;
    LinePlaneIntersection i = LinePlaneIntersectionFunc(l1,l2,p1,p2,p3);
    Point2d pp;
    pp.x = i.tuv.dy;
    pp.y = i.tuv.dz;
    return std::make_pair(pp, i.tuv.dx);
  }
private:
  Point p;
  Vector v;
  Plane pl;
};
class Texture
{
public:
  virtual BufferRef Bitmap() const=0;
  virtual void SetChanged(bool b)=0;
};

class TextureEvents
{
public:
  virtual void Pos(Point2d p)=0; // moved pointer to coordinates in the texture
  virtual void HandleKey(int key)=0;
  virtual void HandlePointer(int button) =0;
  virtual void HandleTextureChange(Texture &tex)=0;
};

class DataTypeTextureEvents : public TextureEvents
{
public:
  DataTypeTextureEvents(DataType &dt, int x, int y) : dt(dt), dtbitmap(dt, x,y), x(x), y(y) 
  {
    pos.x = 0.0;
    pos.y = 0.0;
  }
  void Pos(Point2d p) 
  {
    pos = p; 
    dt.set_val(pos.x/x*dt.XCount(), pos.y/y*dt.YCount(), true);
    changed = true;

  }
  void HandleKey(int key) { }
  void HandlePointer(int button)
  {
    if (button == 0)
      {
	dt.set_val(pos.x/x*dt.XCount(), pos.y/y*dt.YCount(), true);
	changed = true;
      }
    if (button == 1)
      {
	dt.set_val(pos.x/x*dt.XCount(), pos.y/y*dt.YCount(), false);
	changed = true;
      }
  }
  void HandleTextureChange(Texture &tex)
  {
    if (changed)
      {
	const Region &r = dt.InvalidRects();
	int size = r.Size();
	for(int i=0;i<size;i++)
	  {
	    Rect rr = r.Index(i);
	    rr.tl.x *= x/dt.XCount();
	    rr.tl.y *= 2.0*y/dt.YCount();
	    rr.br.x *= x/dt.XCount();
	    rr.br.y *= 2.0*y/dt.YCount();
	    dtbitmap.Generate(rr);
	    BufferRef source = dtbitmap.Ref();
	    BufferRef target = tex.Bitmap();
	    BufferRef::CopyBuffer(source, target, 0,0, rr.tl.x, rr.tl.y, rr.br.x, rr.br.y);
	  }
	dt.ClearInvalidRects();
      }
    tex.SetChanged(changed);
    changed = false;
  }
private:
  Point2d pos;
  DataType &dt;
  DataTypeBitmap dtbitmap;
  bool changed;
  int x,y;
};

class TextureWidget : public Widget, private Texture
{
public:
  TextureWidget(BufferRef buffer, TextureEvents &events, int screensizex, int screensizey, Render *r) : Widget(r), buffer(buffer), events(events), rectangle(screensizex, screensizey), quads_to_planes(rectangle), texture(buffer), sx(screensizex), sy(screensizey), rectangle2(rectangle, Matrix::Translate(0.0,0.0,0.1)), outline(rectangle2) { }
  float XRot() const { return 1.0; }
  float YRot() const { return 1.0; }
  float ZRot() const { return 0.0; }
  void HandleKey(int key) 
  {
    events.HandleKey(key);
    changed = true;
  }
  void HandlePointer(int button) 
  {
    events.HandlePointer(button);
    changed = true;
  }
  void HandlePointerMove(Point2d p);
  BufferRef Bitmap() const
  {
    return buffer;
  }
  void SetChanged(bool b)
  {
    changed2 = b;
  }
  bool Frame(float time);
private:
  BufferRef buffer;
  TextureEvents &events;
  RectangleElem rectangle;
  QuadsToPlanes quads_to_planes;

  TexturePlugin texture;
  bool changed, changed2;
  float sx,sy;
  MatrixElem rectangle2;
  OutlineFaces outline;
};
#if 0
template<class T>
class EditArray : public Array<int, T>, public Widget
{
public:
  EditArray(Array<int, T> &initial_val, Range<T> &r, int x, int y, Rect rect) : copy_array(initial_val), dt(copy_array, r), bitmap(dt, x,y), rect(rect) 
  {
    copy_array.Generate();
    bitmap.Generate();
  }
  void HandleKey(int key) { }
  void HandlePointer(int button) 
  {
    if (button == 0 && rect.InsideRect(mouse))
      {
	Point2d p = rect.RectInternalCoordinates(mouse);
	p.x /= rect.br.x - rect.tl.x;
	p.y /= rect.br.y - rect.tl.y;
	p.x *= dt.XCount();
	p.y *= dt.YCount();
	dt.set_val(p.x, p.y, true);
	// TODO DRAW.
	copy_array.Generate();
	bitmap.Generate();
      }
  }
  void HandlePointerMove(Point2d p) { mouse = p; }
  bool Frame(float time)
  {
    return false;
  }
private:
  CopyArray<T> copy_array;
  ArrayDataType<T> dt;
  DataTypeBitmap bitmap;
  Point2d mouse;
  Rect rect;
};
#endif
#endif
