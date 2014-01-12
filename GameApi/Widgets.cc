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


//#define GL_GLEXT_PROTOTYPES
#include "Widgets.hh"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
//#include <GL/glut.h>

bool RectFrameAnim::Frame(float time)
  {
    Point2d center;
    center.x = (r.tl.x + r.br.x)/2;
    center.y = (r.tl.y + r.br.y)/2;
    float scalex = r.br.x - r.tl.x;
    float scaley = r.br.y - r.tl.y;
    glPushMatrix();
    Matrix m = Matrix::Scale(scalex/screenx,scaley/screeny,scalex/screenx)*Matrix::Translate(center.x-screenx/2, center.y-screeny/2, 0.0);
    float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		      m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		      m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		      m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
    
    glMultMatrixf(&mat[0]);
    obj.Frame(time);
    glPopMatrix();
    return false;
  }
bool RotateFrameAnim::Frame(float time)
{
  glPushMatrix();
  glTranslatef(0.0, 0.0, -500.0);
  //glRotatef(time, next.XRot(),next.YRot(),next.ZRot());
  //glTranslatef(0.0, -100.0, 0.0);
  next.Frame(time);
  glPopMatrix();
  return false; 
}

void TextureWidget::HandlePointerMove(Point2d p) 
{
#if 0
  std::cout << "Screen pos: " << p.x << " " << p.y << std::endl; 
  float arr[16];
  glGetFloatv(GL_PROJECTION_MATRIX, &arr[0]);
  Matrix m = { {arr[0], arr[4], arr[8], arr[12],
		arr[1], arr[5], arr[9], arr[13],
		arr[2], arr[6], arr[10], arr[14],
		arr[3], arr[7], arr[11], arr[15]} };
  Matrix mi = Matrix::Inverse(m);
  Point p2(p.x,p.y,-1.0);
  Point p3 = p2;
  std::cout << "World pos: " << p3 << std::endl;
  Point p4(p.x, p.y, 1.0);
  Point p5 = p4;
  
  Point ray_start_pos = p3;
  Vector ray_dir = p5-p3;
  std::cout << "Ray: " << ray_start_pos << ", dir: " << ray_dir << std::endl;
  Plane pl = quads_to_planes.Index(0);
  std::cout << "Plane:" << pl.u_p << " " << pl.u_x << " " << pl.u_y << std::endl;
  PlaneRayIntersection intersect(ray_start_pos,ray_dir,pl);
  std::pair<Point2d, float> pp = intersect.Elem();
  Point2d px = pp.first;
  px.x = - px.x;
  px.x *= buffer.width;
  px.y *= buffer.height;
  events.Pos(px);
  changed = true;
#endif
  p.x/= sx;
  p.y/= sy;
  p.x*= buffer.width;
  p.y*= buffer.height;
  events.Pos(p);
  
  changed = true;
}
bool TextureWidget::Frame(float time)
  {
    if (changed) 
      {
	events.HandleTextureChange(*this);
	if (changed2)
	  {
	    //std::cout << "Upload" << std::endl;
	    // need to do texture upload.
	    texture.Init();
	  }
	changed = false;
      }
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    
    SimpleTexCoords tex(rectangle, texture.Texture());
    // FIX RenderOpenGl(rectangle, tex);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    RenderOpenGlLines(outline);
    
    return false;
  }
