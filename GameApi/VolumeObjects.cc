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


#include "VolumeObjects.hh"
#include <cmath>
#include <iostream>
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL/SDL_opengl.h>
#include "Shader.hh"
#include "Graph.hh"
#include "Triangle.hh"

SphereVolume::SphereVolume(const Point &origo_, float radius_)
  : radius(radius_), radius_x_radius(radius_*radius_), origo(origo_)
{
}

bool SphereVolume::Inside(Point v) const
{
  // x^2+y^2+z^2 < d^2
  v -= Vector(origo);
  return v.x*v.x+v.y*v.y+v.z*v.z < radius_x_radius;
}
Vector SphereVolume::Normal(Point v) const
{
  return v-origo;
}

Point SphereVolume::Part(const SphericalPoint &part)
{
  SphericalPoint p = part;
  p.r *= radius;
  return p.ToPoint() + Vector(origo);
}

float SphereVolume::Line(Point p, Point p2) const
{
  Vector origo1 = Vector(origo);
  float x1 = p.x;
  float x2 = p2.x;
  float y1 = p.y;
  float y2 = p2.y;
  float z1 = p.z;
  float z2 = p2.z;
  float x3 = origo1.dx;
  float y3 = origo1.dy;
  float z3 = origo1.dz;
  float a = (x2 - x1)*(x2-x1) + (y2 - y1)*(y2-y1) + (z2 - z1)*(z2-z1);
  float b = 2*( (x2 - x1)*(x1 - x3) + (y2 - y1)*(y1 - y3) + (z2 - z1)*(z1 - z3) );
  float c = x3*x3 + y3*y3 + z3*z3 + x1*x1 + y1*y1 + z1*z1 - 2.0*(x3*x1 + y3*y1 + z3*z1) - radius_x_radius; 
  if (b*b < 4*a*c) return 0.0;
  float sqrts = sqrt(b*b - 4*a*c);
  float res = -b + sqrts;
  float res2 = -b - sqrts;
  float res3 = res2 < res ? res2 : res;
  return res3;
}

ConeVolume::ConeVolume(const Point &pos, const Vector &line, float rad1, float rad2)
  : pos(pos), line(line), rad1(rad1), rad2(rad2)
{
}
bool ConeVolume::Inside(Point vv) const
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
Point ConeVolume::Part(const U &u)
{
  Matrix m = Matrix::RotateAroundAxis(line, u.alfa);
  float rad = u.r*(rad1*u.line + (1.0-u.line)*rad2);
  Point p(rad, 0.0, 0.0); // this is probably wrong.
  return pos + u.line*line + p*m;
}
Vector ConeVolume::Normal(Point vv) const
{
 Vector v = vv - pos;
  float div = Vector::DotProduct(v, line);
  float div2 = Vector::DotProduct(line, line);
  div /= div2;
  //float r = rad2*div + rad1*(1.0-div);
  Vector proj = line*div;
  return v - proj;
}
//#endif

#if 0
bool TorusVolume::Inside(Point p) const
{
  p = (-Vector(center)).ApplyPoint(p);
  Vector v = p-Vector::DotProduct(p,N);
  Vector w = r1 * v/v.Dist();
  return (p-w).Dist() < r2;
}
#endif
bool TorusVolume::Inside(Point p) const
{
  p = (-Vector(center)).ApplyPoint(p);
    float divs = 40.0;
    for(float angle1 = 0.0; angle1 < 2.0*3.14159; angle1+=2.0*3.14159/divs)
      {
	float x = cos(angle1);
	float y = sin(angle1);
	Vector u_x = plane.u_x*x;
	Vector u_y = plane.u_y*y;
	Vector u = u_x + u_y;
	u *= 1.0/u.Dist();
	u *= dist1;
	Point pp = u.ApplyPoint(Point::Origo());
	Vector v = p - pp;
	//std::cout << v.dx << " " << v.dy << " " << v.dz << ":" << v.Dist() << " == " << dist2 << std::endl;
	if (v.Dist() - dist2 < 0) return true;
      }
    return false;

}
VolumeEffect::VolumeEffect(Render *r)
  : FrameAnim(r)
{
  piece.push_back(&discard);
  piece.push_back(&gouraud);
}
VolumeEffect::~VolumeEffect()
{
  piece.unuse();
  glDisable(GL_ALPHA_TEST);
}
void VolumeEffect::Init()
{
  glEnable(GL_ALPHA_TEST);
  Triangle tri(Point(0.0,0.0,0.0),
	       Point(5.0,0.0,0.0),
	       Point(2.5,100.0,0.0));
  SphereVolume p(Point(100,100,100), 100);
  SphereVolume p2(Point(150,100,100), 100);
  OrVolume andnot(p,p2);
  RandomVolumePolys r(andnot, tri, 10000, Point(0.0,0.0,0.0), 200.0,200.0,200.0);
  MatrixElem rr(r, Matrix::Translate(-100.0, -100.0, -100.0)*Matrix::Scale(2.0,2.0,2.0));
  TrisToTris tr(rr);
  BatchGeneration batch(tr, 10240);
  SeparateArrayElements sep(batch);
  IteratorArray<int, const FaceCollectionTri*> batcharr(sep);
  //FaceCollectionTri *arr[] = { &tr, &tr };
  ObjectArray oa(batcharr.begin(), batcharr.end());
  VBO vbo(oa);

  DiscardAttributes discard_attr(AttrCenterX, AttrCenterY, AttrCenterZ);
  piece.set_attribs(discard_attr);
  std::vector<Attrib> attribs;
  discard_attr.Attr(attribs);
  
  UpdateVBO(vbo, vbostate, UpdateVertexNormalIndex, attribs);
}

bool VolumeEffect::Frame(float time)
{
  glAlphaFunc(GL_LESS, 0.5);

  DiscardParameters params(time*time);
  piece.set_params(params);
  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,180,0);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  piece.set_params(pp);
  piece.use();

  glColor4f(1.0,0.5,0.3,0.5);
  DrawVBO(vbostate, UpdateVertexNormalIndex);
  return false;
}

bool MandelBulb::Inside(Point C) const
{
  Point p(0.0,0.0,0.0);
  for(int i=0;i<iterations;i++)
    {
      p = Step(p, C, n);
      if (!sp.Inside(p))
	{
	  return false;
	}
    }
  return true;
}

Point MandelBulb::Step(Point p, Point C, float n)
{
  float x = p.x;
  float y = p.y;
  float z = p.z;
  float r = sqrt(x*x+y*y+z*z);
  float r_n = pow(r,n);
  float alpha = atan2(y,x);
  float beta = atan2(z, sqrt(x*x+y*y));
  float nx = cos(n*alpha)*cos(n*beta);
  float ny = sin(n*alpha)*cos(n*beta);
  float nz = sin(n*beta);
  Point z_n(r_n*nx,r_n*ny,r_n*nz);
  Point z_n_c = z_n + C;
  return z_n_c;
}

void FractalEffect::Init()
{
  MandelBulb vol(8.0,  // n
		 Point(0.0,0.0,0.0), // p
		 Point(0.0,0.0,0.0), // center
		 5.0, // radius
		 6); // iterations
  VolumeVoxel vox(vol,
		  Range<float>(-1.0,1.0),
		  Range<float>(-1.0,1.0),
		  Range<float>(-1.0,1.0));
  int div = 20;
  SampleVoxel<bool> samp(vox, div,div,div);

  //Cube3HandleValue faces; // HandleValue<Cube3>
  GenerateArray<Cube3> faces;
  Cube2Cube3Function f(30.0,30.0,30.0,
		       -30*(div/2), -30*(div/2), -30*(div/2));
  HandleValueFunction<Cube2, Cube3> cmap(faces, f);
  MarchingCubesVoxel gen(samp, cmap);
  gen.Gen();
  CubeElem ce;
  ce.SetBox(Matrix::Identity());
  MemoizeFaces ce2(ce);
  Triangulate cubetris(ce2);
  cubetris.Gen();
  Cube3 cube3;
  cube3.minx = -1.0;
  cube3.miny = -1.0;
  cube3.minz = -1.0;
  cube3.maxx = 1.0;
  cube3.maxy = 1.0;
  cube3.maxz = 1.0;
  BoxedTriangles boxed(faces, cubetris, cube3);
  FaceCollectionFromTriArray face(boxed);
  GetRender()->UpdateVBO(face, vbostate.GetState() /*, UpdateAll*/);
}
bool FractalEffect::Frame(float time)
{
  GetRender()->DrawVBO(vbostate.GetState());
  return false; 
}
