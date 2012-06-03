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



#ifndef BITMAP_HH
#define BITMAP_HH

#include "VectorTools.hh"
#include "Buffer.hh"

#if 0
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
};


class Phong : public VolumeObject
{
public:
  Phong(VolumeObject &t, Point viewer, Point light, Color lightcolor, Color spec_color) : tt(t), light(light), lightcolor(lightcolor), spec_color(spec_color)
  {
    Vector l_p_v = Vector(viewer) + Vector(light);
    halfway = l_p_v/l_p_v.Dist();
  }
  virtual bool Inside(Point v) const { return tt.Inside(v); }
  virtual Vector Normal(Point v) const { return tt.Normal(v); }
  virtual Color ColorValue(Point p) const
  {
    Vector normal = tt.Normal(p);
    normal = normal/normal.Dist();
    float col = Vector::DotProduct(normal, halfway);
    float col2 = Vector::DotProduct(normal, light);
    //col = 1.0 - col;
    col = col*col;
    //col = col*col;
    //col = col*col;
    //col = col*col;
    //col += col2*0.5;
    if (col < 0.0) col = 0.0;
    if (col > 1.0) col = 1.0;
    if (col2 < 0.0) col2 = 0.0;
    if (col2 > 1.0) col2 = 1.0;
    //std::cout << col << std::endl;
    //col = col*col;
    //col = col*col;
    //col = col*col;
    //col *=255.0;
    //col2 *= 255.0;
    Color c;
    c.r = int(col*lightcolor.r+col2*spec_color.r);
    c.g = int(col*lightcolor.g+col2*spec_color.g);
    c.b = int(col*lightcolor.b+col2*spec_color.b);
    if (c.r > 255) c.r = 255;
    if (c.g > 255) c.g = 255;
    if (c.b > 255) c.b = 255;
    return c;
  }

private:
  VolumeObject &tt;
  Vector halfway;
  Vector light;
  Color lightcolor;
  Color spec_color;
};

class Cone : public VolumeObject
{
public:
  Cone(const Point &pos, const Vector &line, float rad1, float rad2);
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
class AndNot  : public VolumeObject
{
public:
  AndNot(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
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

class Or  : public VolumeObject
{
public:
  Or(VolumeObject &t, VolumeObject &k) : tt(t), kk(k) { }
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

class ColorSpec  : public VolumeObject
{
public:
  ColorSpec(VolumeObject &val_, Color c_) : c(c_), val(val_) { }
  ColorSpec(VolumeObject &val_, int r, int g, int b) : c(r,g,b), val(val_) { }
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
class VolumeBox  : public VolumeObject
{
public:
  VolumeBox(float x1, float y1, float z1,
      float x2, float y2, float z2) : tl(x1,y1,z1), br(x2,y2,z2) { }
  VolumeBox(Vector top_left, Vector bottom_right) : tl(top_left), br(bottom_right) { }
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

void RenderToBitmap(const VolumeObject &v, BufferRef bitmap);



#endif
#endif
