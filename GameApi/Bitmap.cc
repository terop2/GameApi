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

#include "Bitmap.hh"
//#include "BigObjects.hh"
#include <set>
#include <queue>
#include <unordered_set>
#include "Buffer.hh"
#include <cstdlib>

#if 0
class Random
{
public:
  Random() : seed1(555), seed2(999) { }
  unsigned int next() { return rand(); /*seed1 += seed2; seed2 = seed1; return s
eed1 >> 16;*/ }
  unsigned int maximum() { return RAND_MAX; /*return 0xffff;*/ }
private:
  unsigned int seed1, seed2;
};

class MarchingCubes
{
public:
  struct U
  {
    float u_x, u_y, u_z;
    float c_x, c_y, c_z;
  };


  struct Coord;
  void SetU(float ux, float uy, float uz) { u.u_x = ux; u.u_y = uy; u.u_z = uz; }
  void SetC(float x, float y, float z) { u.c_x = x; u.c_y = y; u.c_z = z; }
  unsigned int Color(float x, float y, float z)
  {
    z+=u.u_z;
    x/=400.0; y/=400.0; z/=400.0; 
    return obj->ColorValue(Point(x,y,z)).Pixel();
  }
  bool Object(float x, float y, float z) 
  {
    x/=400.0; y/=400.0; z/=400.0; 
    return obj->Inside(Point(x,y,z));
    //Point p(x/300.0,y/300.0,z/300.0);
    //float f = 1.0/p.Dist();
    //float g = 1.0/(p-Point(c_x,c_y,c_z)).Dist();
    //float h = 1.0/(p-Point(0.0,-0.7,0.0)).Dist();
    //return (f+g+h)-6.0 < 0.0; 

 
    //return x*x+y*y+z*z<10000.0; 
  }

  bool Id2(const Coord &c)
  {
    bool a0 = Object(c.x, c.y, c.z);
    bool x = Object(c.x+u.u_x, c.y, c.z);
    bool y = Object(c.x, c.y+u.u_y, c.z);
    bool z = Object(c.x, c.y, c.z+u.u_z);
    bool yz = Object(c.x, c.y+u.u_y, c.z+u.u_z);
    bool xz = Object(c.x+u.u_x, c.y, c.z+u.u_z);
    bool xy = Object(c.x+u.u_x, c.y+u.u_y, c.z);
    bool xyz = Object(c.x+u.u_x, c.y+u.u_y, c.z+u.u_z);

    bool allsame = a0 == x && x == y && y==z && z == yz && yz == xz && xz == xy 
&& xy == xyz;
    return allsame;
  }
  void Id(Coord cc2, BufferRef *buf, BufferRefF *vbuf)
  {
    std::vector<Coord> nodes;
    for(int i=0;i<8;i++) cc2.valid[i]=false;
    nodes.push_back(cc2);
    
    Coord c;
    while(!nodes.empty())
      {
	c = nodes.back();
	nodes.pop_back();

    if (visited_cubes.find(c)!=visited_cubes.end())
      {
	//std::cout << "out" << std::endl;
	continue; //return;
      }
    visited_cubes.insert(c);
#define A0 0
#define X 1
#define Y 2
#define Z 3
#define YZ 4
#define XZ 5
#define XY 6
#define XYZ 7

    bool a0 = c.valid[A0]?c.array[A0]:Object(c.x, c.y, c.z);
    bool x = c.valid[X]?c.array[X]:Object(c.x+u.u_x, c.y, c.z);
    bool y = c.valid[Y]?c.array[Y]:Object(c.x, c.y+u.u_y, c.z);
    bool z = c.valid[Z]?c.array[Z]:Object(c.x, c.y, c.z+u.u_z);
    bool yz = c.valid[YZ]?c.array[YZ]:Object(c.x, c.y+u.u_y, c.z+u.u_z);
    bool xz = c.valid[XZ]?c.array[XZ]:Object(c.x+u.u_x, c.y, c.z+u.u_z);
    bool xy = c.valid[XY]?c.array[XY]:Object(c.x+u.u_x, c.y+u.u_y, c.z);
    bool xyz = c.valid[XYZ]?c.array[XYZ]:Object(c.x+u.u_x, c.y+u.u_y, c.z+u.u_z);
    c.array[A0]=a0;
    c.array[X]=x;
    c.array[Y]=y;
    c.array[Z]=z;
    c.array[YZ]=yz;
    c.array[XZ]=xz;
    c.array[XY]=xy;
    c.array[XYZ]=xyz;

    int offset = (int(c.x)+buf->width/2)+(int(c.y)+buf->height/2)*buf->ydelta;
    if (!(offset < 0 || (unsigned int)offset > buf->ydelta*buf->height))
    if (vbuf->buffer[offset] > c.z)
      {
	vbuf->buffer[offset] = c.z;
	float xx=0.0,yy=0.0;
	if (xz || xyz) xx=u.u_x;
	if (yz || xyz) yy=u.u_y;

	unsigned int col = Color(c.x+xx, c.y+yy, c.z+u.u_z);
	for(int y=0;y<u.u_y;y++)
	  {
	  int offset2 = offset + y*buf->ydelta;
	  for(int x=0;x<u.u_x;x++)
	    buf->buffer[offset2+x] = col;
	  }
      }
    //std::cout << "Cubes" << c.x << " " << c.y << " " << c.z << std::endl;


    //std::cout << "Cube: " << a0 << " " << x << " " << y << " " << z << " " << 
    //yz << " " << xz << " " << xy << " " << xyz << std::endl;

    bool my = (a0 == x) && (x == xz) && (z == a0); // -y
    bool px = (x == xy) && (xy == xyz) && (x == xz); // x
    bool mz = (a0 == x) && (x == xy) && (a0 == y); // -z
    bool pz = (z == xz) && (xz == xyz) && (z == yz); // z
    bool py = (y == xy) && (xy == xyz) && (y==yz); // y
    bool mx = (a0 == y) && (y == yz) && (a0 == z); // -x

    //std::cout << "Faces: " << mx << " " << my << " " << mz << " " << px << " "
    // << py << " " << pz << std::endl;
    for(int i=0;i<8;i++)
      {
	c.valid[i]=true;
      }
    if (!mx)
      {
	Coord cc = c;
	cc.PrevX();
	cc.valid[A0]=false;
	cc.valid[Y]=false;
	cc.valid[Z]=false;
	cc.valid[YZ]=false;
	cc.array[X]=cc.array[A0];
	cc.array[XZ]=cc.array[Z];
	cc.array[XY]=cc.array[Y];
	cc.array[XYZ]=cc.array[YZ];

	nodes.push_back(cc);
      }
    if (!px)
      {
	Coord cc = c;
	cc.NextX();
	cc.valid[X]=false;
	cc.valid[XY]=false;
	cc.valid[XZ]=false;
	cc.valid[XYZ]=false;
	cc.array[A0]=cc.array[X];
	cc.array[Z]=cc.array[XZ];
	cc.array[Y]=cc.array[XY];
	cc.array[YZ]=cc.array[XYZ];

	nodes.push_back(cc);
      }
    if (!my)
      {
	Coord cc = c;
	cc.PrevY();
	cc.valid[A0]=false;
	cc.valid[X]=false;
	cc.valid[Z]=false;
	cc.valid[XZ]=false;
	cc.array[Y]=cc.array[A0];
	cc.array[YZ]=cc.array[Z];
	cc.array[XY]=cc.array[X];
	cc.array[XYZ]=cc.array[XZ];

	nodes.push_back(cc);
      }
    if (!py)
      {
	Coord cc = c;
	cc.NextY();
	cc.valid[Y]=false;
	cc.valid[XY]=false;
	cc.valid[YZ]=false;
	cc.valid[XYZ]=false;
	cc.array[A0]=cc.array[Y];
	cc.array[Z]=cc.array[YZ];
	cc.array[X]=cc.array[XY];
	cc.array[XZ]=cc.array[XYZ];

	nodes.push_back(cc);
      }
    if (!mz)
      {
	Coord cc = c;
	cc.PrevZ();
	cc.valid[A0]=false;
	cc.valid[X]=false;
	cc.valid[Y]=false;
	cc.valid[XY]=false;
	cc.array[Z]=cc.array[A0];
	cc.array[YZ]=cc.array[Y];
	cc.array[XZ]=cc.array[X];
	cc.array[XYZ]=cc.array[XY];

	nodes.push_back(cc);
      }
    if (!pz)
      {
	Coord cc = c;
	cc.NextZ();
	cc.valid[Z]=false;
	cc.valid[XZ]=false;
	cc.valid[YZ]=false;
	cc.valid[XYZ]=false;
	cc.array[A0]=cc.array[Z];
	cc.array[Y]=cc.array[YZ];
	cc.array[X]=cc.array[XZ];
	cc.array[XY]=cc.array[XYZ];

	nodes.push_back(cc);
      }
      }
  }
  void Initial(Coord &c)
  {
    Point p;
    do
      {
	p.x = float(random.next())/float(random.maximum())*2.0-1.0;
	p.y = float(random.next())/float(random.maximum())*2.0-1.0;
	p.z = float(random.next())/float(random.maximum())*2.0-1.0;
	p.x *= 400.0;
	p.y *= 400.0;
	p.z *= 400.0;
      }
    while(!Object(p.x, p.y, p.z));
    c.x = int(p.x)*u.u_x;
    c.y = int(p.y)*u.u_y;
    c.xx = int(p.x);
    c.yy = int(p.y);
    c.z = int(-400)*u.u_z;
    c.zz = -400;

    bool b = Id2(c);
    int count = 0;
    while(b && count<6000)
      {
	c.NextZ();
	b = Id2(c);
	count++;
      }
    std::cout << "Count: " << count << std::endl;
  }

public:
  struct Coord 
  { 
    float x,y,z;
    int xx, yy, zz;
    bool array[8];
    bool valid[8];
    U *u;
    
    void NextX() { x+=u->u_x; xx++; }
    void NextY() { y+=u->u_y; yy++; }
    void NextZ() { z+=u->u_z; zz++; }
    void PrevX() { x-=u->u_x; xx--; }
    void PrevY() { y-=u->u_y; yy--; }
    void PrevZ() { z-=u->u_z; zz--; }

    friend bool operator==(const Coord &c1, const Coord &c2)
    {
      return c1.xx==c2.xx && c1.yy==c2.yy && c1.zz == c2.zz;
    }
    friend bool operator<(const Coord &c1, const Coord &c2)
    {
      if (c1.xx != c2.xx) return c1.xx < c2.xx;
      if (c1.yy != c2.yy) return c1.yy < c2.yy;
      if (c1.zz != c2.zz) return c1.zz < c2.zz;
      return false;
    }
  };
  class MyHash : public std::unary_function<MarchingCubes::Coord, size_t>
  {
  public:
    std::size_t operator()(const MarchingCubes::Coord &val) const
    {
      return static_cast<size_t>(val.xx+(val.yy<<8)+(val.zz<<16));
    }
  };



  MarchingCubes(const VolumeObject *obj, float u_x, float u_y, float u_z,
		float c_x, float c_y, float c_z) 
    : visited_cubes(1000000), obj(obj) 
  { 
    u.u_x = u_x;
    u.u_y = u_y;
    u.u_z = u_z;
    u.c_x = c_x;
    u.c_y = c_y;
    u.c_z = c_z; 
  }
  
  std::tr1::unordered_set<Coord, MyHash> visited_cubes;
  mutable Random random;
  const VolumeObject *obj;
  U u;
};

void RenderToBitmap(const VolumeObject &v, BufferRef bitmap)
  {
    BufferRefF zbuffer = BufferRefF::NewBuffer(bitmap.width, bitmap.height);

    MarchingCubes c(&v, 1.0, 1.0, 1.0,
		    0.0, 0.3, 0.0);
    MarchingCubes::Coord cc;
    cc.x = 0.0;
    cc.y = 0.0;
    cc.z = 0.0;
    cc.xx = 0;
    cc.yy = 0;
    cc.zz = 0;
    cc.u = &c.u;
    //c.SetU(1.0,1.0,1.0);
    //c.SetC(0.0, 0.3, 0.0);
    c.Initial(cc);
    c.Id(cc, &bitmap, &zbuffer);
  }

#if 0
Cone::Cone(const Point &pos, const Vector &line, float rad1, float rad2)
  : pos(pos), line(line), rad1(rad1), rad2(rad2)
{
}
bool Cone::Inside(Point vv) const
{
  Vector v = vv - pos;
  float div = Vector::DotProduct(v, line);
  float div2 = Vector::DotProduct(line, line);
  div /= div2;
  if (div < 0.0) return false;
  if (div > 1.0) return false;
  float r = rad2*div + rad1*(1.0-div);
  Vector proj = line*div;
  float dist = (proj - v).Dist();
  return dist < r;
}
Point Cone::Part(const U &u)
{
  Matrix m = Matrix::RotateAroundAxis(line, u.alfa);
  float rad = u.r*(rad1*u.line + (1.0-u.line)*rad2);
  Point p(rad, 0.0, 0.0); // this is probably wrong.
  return pos + u.line*line + p*m;
}
Vector Cone::Normal(Point vv) const
{
 Vector v = vv - pos;
  float div = Vector::DotProduct(v, line);
  float div2 = Vector::DotProduct(line, line);
  div /= div2;
  //float r = rad2*div + rad1*(1.0-div);
  Vector proj = line*div;
  return v - proj;
}

#endif
#endif
