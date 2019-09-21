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


#include "Pieces.hh"
#define NO_SDL_GLEXT
//#include <GL/glew.h>
//#include <SDL_opengl.h>
#include "GameApi_low.hh"


void PlainPiece(const Heights &h, const Color2 &c)
{
#if 0
  g_low->ogl->glBegin( GL_QUADS );
  g_low->ogl->glColor4f( c.r, c.g, c.b, c.t);
  g_low->ogl->glVertex3f( 0.0, h.tl, 0.0);
  g_low->ogl->glVertex3f( 100.0, h.tr, 0.0);
  g_low->ogl->glVertex3f( 100.0, h.br, 100.0);
  g_low->ogl->glVertex3f( 0.0, h.bl, 100.0);
  g_low->ogl->glEnd();
#endif
}
void glNormal3f_a(const Vector &v);
void glVertex3f_a(const Point &p, const Point &center)
{
#if 0
  Vector v = p -center;
  //v.Normalize();
  g_low->ogl->glNormal3f_a(v);
  g_low->ogl->glVertex3f(p.x, p.y, p.z);
#endif
}
void glNormal3f_a(const Vector &v)
{
  g_low->ogl->glNormal3f(v.dx, v.dy, v.dz);
}
void Cube(const Point &center, const Dir &d, const Color2 &c, const Size2 &s)
{
  //Point p1 = center + d.u_z*s.z/2 - d.u_x*s.x/2 - d.u_y*s.y/2;
  //Vector v_x = d.u_x*s.x;
  //Vector v_y = d.u_y*s.y;
  //Vector v_z = d.u_z*s.z;
  //Point p2 = p1 + v_x;
  //Point p3 = p2 - v_z;
  //Point p4 = p3 + v_y;
  //Point p5 = p2 + v_y;
  //Point p6 = p1 + v_y;
  //Point p7 = p6 - v_z;
  //Point p8 = p1 - v_z;

  //Vector v1 = p1 - center;
  //Vector v2 = p2 - center;
  //Vector v3 = p3 - center;
  //Vector v4 = p4 - center;
  //Vector v5 = p5 - center;
  //Vector v6 = p6 - center;
  //Vector v7 = p7 - center;
  //Vector v8 = p8 - center;

#if 0
  g_low->ogl->glBegin( GL_QUADS );
  g_low->ogl->glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( v_z );
  g_low->ogl->glVertex3f_a( p1, center );
  g_low->ogl->glVertex3f_a( p2, center  );
  g_low->ogl->glVertex3f_a( p5, center  );
  glVertex3f_a( p6, center  );
  glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( -v_x );
  glVertex3f_a( p1, center  );
  glVertex3f_a( p6, center  );
  glVertex3f_a( p7, center  );
  glVertex3f_a( p8, center  );
  glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( -v_y );
  glVertex3f_a( p1, center  );
  glVertex3f_a( p8, center  );
  glVertex3f_a( p3, center  );
  glVertex3f_a( p2, center  );
  glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( v_y );
  glVertex3f_a( p6, center  );
  glVertex3f_a( p5, center  );
  glVertex3f_a( p4, center  );
  glVertex3f_a( p7, center  );
  glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( -v_z );
  glVertex3f_a( p8, center  );
  glVertex3f_a( p7, center  );
  glVertex3f_a( p4, center  );
  glVertex3f_a( p3, center  );
  glColor4f( c.r, c.g, c.b, c.t);
  //glNormal3f_a( v_x );
  glVertex3f_a( p2, center  );
  glVertex3f_a( p3, center  );
  glVertex3f_a( p4, center  );
  glVertex3f_a( p5, center  );
  glEnd();
#endif
}

float TransformFloat(const float &f1, const float &f2,
		     float starttime, float endtime, float time)
{
  if (time <= starttime) return f1;
  if (time >= endtime) return f2;
  float delta = time - starttime;
  float scale = endtime-starttime;
  float mult = delta/scale; // 0.0 .. 1.0
  return f1*(1.0-mult) + f2*(mult);
}

Color2 TransformColor(const Color2 &c1, const Color2 &c2,
		    float starttime, float endtime, float time)
{
  float r = TransformFloat(c1.r, c2.r, starttime, endtime, time);
  float g = TransformFloat(c1.g, c2.g, starttime, endtime, time);
  float b = TransformFloat(c1.b, c2.b, starttime, endtime, time);
  float t = TransformFloat(c1.t, c2.t, starttime, endtime, time);
  Color2 c;
  c.r=r;
  c.g=g;
  c.b=b;
  c.t=t;
  return c;
}
Point TransformPoint(const Point &p1, const Point &p2,
		     float starttime, float endtime, float time)
{
  float x = TransformFloat(p1.x, p2.x, starttime, endtime, time);
  float y = TransformFloat(p1.y, p2.y, starttime, endtime, time);
  float z = TransformFloat(p1.z, p2.z, starttime, endtime, time);
  Point c;
  c.x=x;
  c.y=y;
  c.z=z;
  return c;
}

Vector TransformVector(const Vector &p1, const Vector &p2,
		     float starttime, float endtime, float time)
{
  float x = TransformFloat(p1.dx, p2.dx, starttime, endtime, time);
  float y = TransformFloat(p1.dy, p2.dy, starttime, endtime, time);
  float z = TransformFloat(p1.dz, p2.dz, starttime, endtime, time);
  Vector c;
  c.dx=x;
  c.dy=y;
  c.dz=z;
  return c;
}


Color2 AnimColor(const TimedColor *color, int count, float time)
{
  int i;
  for(i=0;i<count-2;i++)
    {
      float start_time = color[i].time;
      if (time > start_time && time < color[i+1].time)
	{
	  break;
	}
    }
  return TransformColor(*color[i].c, *color[i+1].c, color[i].time, color[i+1].time, time);
}
#if 0
Point AnimPoint(const TimedPoint *points, int count, float time)
{
  int i;
  for(i=0;i<count-2;i++)
    {
      float start_time = points[i].time;
      if (time > start_time && time < points[i+1].time)
	{
	  break;
	}
    }
  return TransformPoint(*points[i].p, *points[i+1].p, points[i].time, points[i+1].time, time);
}
#endif
