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


#define NO_SDL_GLEXT
#include "Category.hh"
#include "VectorTools.hh"
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

void RenderArrow::doit()
{
  glPushMatrix();
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  glMultMatrixf(&mat[0]);
  glColor4ub(color, color>>8, color>>16, color>>24);
  //std::cout << object << std::endl;
  ::DrawVBO(vbostate, UpdateAll); // missing object
  glPopMatrix();
}

void CategoryEffect::Init()
{
  //SphereElem cube(10,10);
  CubeElem cube;
  cube.SetBox(Matrix::Identity());
  UpdateVBO(cube, vox.vbostate, UpdateAll);
}
bool CategoryEffect::Frame(float time)
{

  int step = 20.0;
  vox.cube.x = 40;
  vox.cube.y = 40;
  vox.cube.z = 40;

  std::pair<VolumeObject*, Point> array[] =
    {
      std::make_pair(new SphereVolume(Point(0.0,0.0,0.0),1.0) , Point(-step*vox.cube.x/2,-step*vox.cube.y/2,-step*vox.cube.z/2)),
      std::make_pair(new SphereVolume(Point(0.0,0.0,0.0),1.0) , Point(-step*vox.cube.x/2,400.0-step*vox.cube.y/2,-step*vox.cube.z/2)),
      std::make_pair(new ConeVolume(Point(0.0,0.0,0.0), Vector(2.0,0.0,0.0), 1.0, 0.6), Point(400.0-step*vox.cube.x/2,-step*vox.cube.y/2,-step*vox.cube.z/2))
    };
  vox.VolumePosListObject::array = new VectorArray<std::pair<VolumeObject*, Point> >(array, array+3);

  vox.rx.start = -2.0;
  vox.rx.end = 2.0;
  vox.ry.start = -2.0;
  vox.ry.end = 2.0;
  vox.rz.start = -2.0;
  vox.rz.end = 2.0;
  //vox.volume = 
  vox.u_x = Vector(step, 0.0, 0.0);
  vox.u_y = Vector(0.0, step, 0.0);
  vox.u_z = Vector(0.0, 0.0, step);
  vox.ColorObject::color = 0xffffffff;
  vox.ObjectNumObject::num = 0;
  //vox.Point2Object::p = Point(-step*vox.cube.x/2,-step*vox.cube.y/2,-step*vox.cube.z/2);
  vox.ColorCubeArrow::sx = step*vox.cube.x;
  vox.ColorCubeArrow::sy = step*vox.cube.y;
  vox.ColorCubeArrow::sz = step*vox.cube.z;
  vox.ScaleTranslateArrow::sx = step/2;
  vox.ScaleTranslateArrow::sy = step/2;
  vox.ScaleTranslateArrow::sz = step/2;
  vox.doit();
  return false;
}
void Print(CubeListObject &cube)
{
  for(int i=0;i<cube.array->Size();i++)
    {
      Cube2 c = cube.array->Index(i);
      std::cout << i << ":" << c.x << "," << c.y << "," << c.z << std::endl;
    }
}
