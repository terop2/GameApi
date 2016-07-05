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
#define NO_SDL_GLEXT
#define _SCL_SECURE_NO_WARNINGS
#include "Effect.hh"
#include "Effect2.hh"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "VectorTools.hh"
//#include <GL/glut.h>
#include "Bitmap.hh"

#include "Shader.hh"
#include "Event.hh"
#include "Graph.hh"

#ifndef EMSCRIPTEN
#define VAO 1
#endif



std::ostream &operator<<(std::ostream &o, const ObjectData &od)
{
  o << "(" << od.matrix << " : " << od.object << ")";
  return o;
}


Color2 colors[] =
  {
    // #1
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.7, 1.0, 1.0 },
    { 1.0, 0.5, 1.0, 1.0 },
    { 1.0, 0.2, 1.0, 1.0 },
    { 1.0, 0.0, 0.8, 1.0 },
    { 1.0, 0.0, 0.6, 1.0 },
    { 1.0, 0.0, 0.4, 1.0 },
    { 1.0, 0.0, 0.2, 1.0 },
    { 1.0, 0.2, 0.0, 1.0 },
    { 1.0, 0.4, 0.0, 1.0 },
    { 1.0, 0.6, 0.0, 1.0 },
    { 1.0, 0.8, 0.0, 1.0 },
    { 1.0, 1.0, 0.2, 1.0 },
    { 1.0, 1.0, 0.4, 1.0 },
    { 1.0, 1.0, 0.6, 1.0 },
    { 1.0, 1.0, 0.8, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },

    // #2
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.7, 1.0, 1.0 },
    { 1.0, 0.5, 1.0, 1.0 },
    { 1.0, 0.2, 1.0, 1.0 },
    { 1.0, 0.0, 0.8, 1.0 },
    { 1.0, 0.0, 0.6, 1.0 },
    { 1.0, 0.0, 0.4, 1.0 },
    { 1.0, 0.0, 0.2, 1.0 },
    { 1.0, 0.2, 0.0, 1.0 },
    { 1.0, 0.4, 0.0, 1.0 },
    { 1.0, 0.6, 0.0, 1.0 },
    { 1.0, 0.8, 0.0, 1.0 },
    { 1.0, 1.0, 0.2, 1.0 },
    { 1.0, 1.0, 0.4, 1.0 },
    { 1.0, 1.0, 0.6, 1.0 },
    { 1.0, 1.0, 0.8, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },

    // #3
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.7, 1.0, 1.0 },
    { 1.0, 0.5, 1.0, 1.0 },
    { 1.0, 0.2, 1.0, 1.0 },
    { 1.0, 0.0, 0.8, 1.0 },
    { 1.0, 0.0, 0.6, 1.0 },
    { 1.0, 0.0, 0.4, 1.0 },
    { 1.0, 0.0, 0.2, 1.0 },
    { 1.0, 0.2, 0.0, 1.0 },
    { 1.0, 0.4, 0.0, 1.0 },
    { 1.0, 0.6, 0.0, 1.0 },
    { 1.0, 0.8, 0.0, 1.0 },
    { 1.0, 1.0, 0.2, 1.0 },
    { 1.0, 1.0, 0.4, 1.0 },
    { 1.0, 1.0, 0.6, 1.0 },
    { 1.0, 1.0, 0.8, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },

    // #4
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.7, 1.0, 1.0 },
    { 1.0, 0.5, 1.0, 1.0 },
    { 1.0, 0.2, 1.0, 1.0 },
    { 1.0, 0.0, 0.8, 1.0 },
    { 1.0, 0.0, 0.6, 1.0 },
    { 1.0, 0.0, 0.4, 1.0 },
    { 1.0, 0.0, 0.2, 1.0 },
    { 1.0, 0.2, 0.0, 1.0 },
    { 1.0, 0.4, 0.0, 1.0 },
    { 1.0, 0.6, 0.0, 1.0 },
    { 1.0, 0.8, 0.0, 1.0 },
    { 1.0, 1.0, 0.2, 1.0 },
    { 1.0, 1.0, 0.4, 1.0 },
    { 1.0, 1.0, 0.6, 1.0 },
    { 1.0, 1.0, 0.8, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },

    // #5
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.7, 1.0, 1.0 },
    { 1.0, 0.5, 1.0, 1.0 },
    { 1.0, 0.2, 1.0, 1.0 },
    { 1.0, 0.0, 0.8, 1.0 },
    { 1.0, 0.0, 0.6, 1.0 },
    { 1.0, 0.0, 0.4, 1.0 },
    { 1.0, 0.0, 0.2, 1.0 },
    { 1.0, 0.2, 0.0, 1.0 },
    { 1.0, 0.4, 0.0, 1.0 },
    { 1.0, 0.6, 0.0, 1.0 },
    { 1.0, 0.8, 0.0, 1.0 },
    { 1.0, 1.0, 0.2, 1.0 },
    { 1.0, 1.0, 0.4, 1.0 },
    { 1.0, 1.0, 0.6, 1.0 },
    { 1.0, 1.0, 0.8, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 }

    
  };


void AttachBoxes(const BoxCollection &c, const std::vector<Boxable*> &v)
{
  int count = c.NumBoxes();
  for(int i=0;i<count;i++)
    {
      v[i]->SetBox(c.BoxIndex(i));
    }
}

Point SphereElem::FacePoint(int face, int point) const
{
  int face1 = face / numfaces;
  int face2 = face - face1*numfaces;
  int numfaces3 = numfaces*2;
  //Vector p = Vector(0.0, 0.0, radius); 
  float deltaangle1 = 2.0*  3.14159/numfaces3;
  float deltaangle2 =   3.14159/numfaces2; 
  float angle1 = face2*deltaangle1;
  float angle2 = face1*deltaangle2;
  if (point==2||point==1) angle1+=deltaangle1;
  if (point==3||point==2) angle2+=deltaangle2;
  //Matrix m = Matrix::XRotation(angle1)*Matrix::YRotation(angle2);
  Vector pp;
  pp.dx = radius*sin(angle1)*cos(angle2);
  pp.dy = radius*sin(angle1)*sin(angle2);
  pp.dz = radius*cos(angle1);
  return center+pp;
  
}
Point ConeElem::FacePoint(int face, int point) const
{
  Point2d pp;
  pp.x= 0.0;
  pp.y= 0.0;
  Circle c1(pp, rad1);
  Circle c2(pp, rad2);
  PlaneCurveIn3d curve1(c1, pl1);
  PlaneCurveIn3d curve2(c2, pl2);
  SampleCurve3d sample1(curve1, numfaces);
  SampleCurve3d sample2(curve2, numfaces);
  int addx = 0;
  int addy = 0;
  switch(point)
    {
    case 2: case 3: addx = 1;
    };
  switch(point)
    {
    case 1: case 2: addy = 1;
    };
  SampleCurve3d *ptr = addx ? &sample1 : &sample2;
  Point p = ptr->Index(face+addy);
  return p;
}

Point DiskElem::FacePoint(int face, int point) const
{
  float delta = 2.0*3.14159/float(numfaces);
  float alfa = delta*float(face);
  if (point==2) alfa+=delta;
  float val = disk_pos;
 
  Point p;
  switch(point)
    {
    case 0:
      p = Point(0.0, 0.0, val);
      break;
    case 1: case 2:
      p = Point(cos(alfa),sin(alfa),val);
      break;
    }
  return p; //box.center + x + y + z;
}


Point CylinderElem::FacePoint(int face, int point) const
{
  float delta = 2.0*3.14159/float(numfaces);
  float alfa = delta*float(face);
  if (point==1 || point==2) alfa+=delta;
  
  float val = 1.0;
  switch(point)
    {
    case 2: case 3:
      val = -1.0;
    default: ;
    }
  Point p = Point(cos(alfa), sin(alfa), val);
  return p;
}

Point CubeElem::FacePoint(int face, int point) const
{
  static int face1[] = { 1, 0, 5, 4 };
  static int face2[] = { 0, 7, 6, 5 };
  static int face3[] = { 7, 2, 3, 6 };
  static int face4[] = { 1, 4, 3, 2 };
  static int face5[] = { 0, 1, 2, 7 };
  static int face6[] = { 5, 6, 3, 4 };
  static int *faces[] = { &face1[0], &face2[0], &face3[0], &face4[0], &face5[0], &face6[0] };
  int *array = faces[face]; // face
  int p = array[point];
  //float x=0.0,y=0.0,z=0.0;
  switch(p)
    {
    case 0: return p111; //x=-1.0; y=-1.0; z=-1.0;  break;
    case 1: return p211; // x=1.0; y=-1.0; z=-1.0; break;
    case 2: return p212; //x=1.0; y=-1.0; z=1.0; break;
    case 3: return p222; //x=1.0; y=1.0; z=1.0; break;
    case 4: return p221; //x=1.0; y=1.0; z=-1.0; break;
    case 5: return p121; //x=-1.0; y=1.0; z=-1.0; break;
    case 6: return p122; //x=-1.0; y=1.0; z=1.0; break;
    case 7: return p112; //x=-1.0; y=-1.0; z=1.0; break;
    };
  Point pp = Point(0.0,0.0,0.0);
  return pp;
}
Point Box::BoxPoint(float x, float y, float z) const
{
  Point pp = center + x*u_x + y*u_y + z*u_z;
  //std::cout << center << std::endl;
  return pp;
}

void TexturePlugin::Init()
{
  if (texture != 5566)
    {
      glDeleteTextures(1, &texture);
    }
    if (buf.width != buf.ydelta) { std::cout << "TexturePlugin::ERROR" << std::endl; }
    int sizex = buf.width;
    int sizey = buf.height;
    glGenTextures(1, &texture); // TODO, move glGenTextures somewhere else. Should not call this here, instead many textures must be allocated with same call.
    glBindTexture(GL_TEXTURE_2D, texture);
    //unsigned int col = buf.buffer[0];
    std::cout << "TexturePlugin: " << texture << std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.buffer);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
TexturePlugin::~TexturePlugin()
{
  glDeleteTextures(1, &texture);
}

void TexturePlugin::Start()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

}
void TexturePlugin::End()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void RenderOpenGlCmds::Execute()
{
  RenderOpenGl(col);
}
void RenderToTexture::PreCalc()
{
  cmds.PreCalc();
  if (in_precalc)
    {
      glEnable(GL_TEXTURE_2D);
      GLfloat params[4];
      glGetFloatv(GL_VIEWPORT, params);
      glViewport(0,0, plugin->buf.width, plugin->buf.height);
      cmds.Execute();
      glBindTexture(GL_TEXTURE_2D, plugin->texture);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, plugin->buf.width, plugin->buf.height, 0);
      glViewport(params[0],params[1],params[2],params[3]);
      glDisable(GL_TEXTURE_2D);
    }
}
void RenderToTexture::Execute()
{
  if (!in_precalc)
    {
      glEnable(GL_TEXTURE_2D);
      GLfloat params[4];
      glGetFloatv(GL_VIEWPORT, params);
      glViewport(0,0, plugin->buf.width, plugin->buf.height);
      cmds.Execute();
      glBindTexture(GL_TEXTURE_2D, plugin->texture);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, plugin->buf.width, plugin->buf.height, 0);
      glViewport(params[0],params[1],params[2],params[3]);
      glDisable(GL_TEXTURE_2D);
    }
}

void TransparencyCmds::Execute()
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  inner_cmds.Execute();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}

void TextureCommands::Execute()
{
  glEnable(GL_TEXTURE_2D);
  cmds.Execute();
  glDisable(GL_TEXTURE_2D);
}
void ColorCmds::Execute()
{
  glColor4ub(c.r, c.g, c.b, c.alpha);
  cmds.Execute();
  glColor4f(1.0,1.0,1.0,1.0);
}

struct DisplayListPriv
{
  GLuint displaylist;
  ~DisplayListPriv() { glDeleteLists(displaylist, 1); }
};
DisplayListCmds::~DisplayListCmds()
{
  DisplayListPriv *priv2 = (DisplayListPriv*)priv;
  delete priv2;
}
void DisplayListCmds::PreCalc()
{
  DisplayListPriv *priv2 = (DisplayListPriv*)priv;
  delete priv2;
  priv2 = new DisplayListPriv;
  priv = priv2;
  priv2->displaylist = glGenLists(1);
  glNewList(priv2->displaylist, GL_COMPILE);
  cmds.Execute();
  glEnd();
}
void DisplayListCmds::Execute()
{
  DisplayListPriv *priv2 = (DisplayListPriv*)priv;
  glCallList(priv2->displaylist);
}
Camera::Camera(MatrixCurve &curve, float time)
{
  glPushMatrix();
  Matrix m = curve.Index(fmodf(time,curve.Size()));
  m = Matrix::Inverse(m);
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };

#ifndef EMSCRIPTEN
  glMultMatrixf(&mat[0]);
#endif
}
Camera::~Camera()
{
  glPopMatrix();
}

void RenderOpenGl(const FaceCollection &col, const BoxCollection &b, Boxable &elem)
{
  BoxCombine be(b,elem);
  RenderOpenGl(col, be);
}

void RenderOpenGl(const FaceCollection &col, const BoxCollection &b, Boxable &elem, const TextureCoords &tex)
{
  BoxCombine be(b,elem);
  RenderOpenGl(col, be, tex);
}


void RenderOpenGl(const FaceCollection &col, ElementCollection &nums)
{
  int count = nums.NumElements();
  for(int i=0;i<count;i++)
    {
      nums.SetElemId(i);
      RenderOpenGl(col);
    }
}
void RenderOpenGl(const FaceCollection &col, ElementCollection &nums, const TextureCoords &tex)
{
  int count = nums.NumElements();
  for(int i=0;i<count;i++)
    {
      nums.SetElemId(i);
      RenderOpenGl(col, tex);
    }
}
void RenderOpenGlTextures(const FaceCollection &col)
{
  FaceCollectionTextureCoords coords(col);
  RenderOpenGl(col, coords);
}

void RenderOpenGl(const FaceCollection &col)
{
  int count = col.NumFaces();
  //std::cout << "numfaces=" << count << std::endl;
  for(int face = 0;face<count; face++)
    {
      int polycount = col.NumPoints(face);
      //std::cout << "polycount=" << polycount << std::endl;
      glBegin( GL_POLYGON );
      for(int v=0;v<polycount;v++)
	{
	  unsigned int color = col.Color(face, v);
	  glColor4ub(color, color>>8, color>>16, color>>24);
	  Vector vv = col.PointNormal(face, v);
	  glNormal3f( vv.dx, vv.dy, vv.dz );
	  Point p = col.FacePoint(face, v);
	  //std::cout << "Vertex: " << p.x << " " << p.y << " " << p.z << std::endl;
	  glVertex3f( p.x, p.y, p.z );
	}
      glEnd();
    }
}

void RenderOpenGl(const LineCollection &col) { RenderOpenGlLines(col); }
void RenderOpenGl(const PointCollection &col) { RenderOpenGlPoly(col); }


void RenderOpenGl(const FaceCollection &col, Function<int,bool> &removepolys)
{
  int count = col.NumFaces();
  for(int face = 0;face<count; face++)
    {
      if (removepolys.Index(face)) continue;
      int polycount = col.NumPoints(face);
      glBegin( GL_POLYGON );
      for(int v=0;v<polycount;v++)
	{
	  unsigned int color = col.Color(face, v);
	  glColor4ub(color, color>>8, color>>16, color>>24);
	  Vector vv = col.PointNormal(face, v);
	  glNormal3f( vv.dx, vv.dy, vv.dz );
	  Point p = col.FacePoint(face, v);
	  glVertex3f( p.x, p.y, p.z );
	}
      glEnd();
    }
}



void RenderOpenGlLines(const LineCollection &col, ElementCollection &nums)
{
  int count = nums.NumElements();
  for(int i=0;i<count;i++)
    {
      nums.SetElemId(i);
      RenderOpenGlLines(col);
    }
}
void RenderOpenGlLines(const LineCollection &col, const BoxCollection &b, Boxable &elem)
{
  BoxCombine be(b,elem);
  RenderOpenGlLines(col, be);

}
void RenderOpenGlLines(const LineCollection &lines)
{
  int count = lines.NumLines();
  for(int line=0;line<count;line++)
    {
      Point p = lines.LinePoint(line, 0);
      Point p2 = lines.LinePoint(line, 1);
      glBegin(GL_LINES);
      glVertex3f(p.x,p.y,p.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glEnd();
    }
}
void RenderOpenGlPoly(const PointCollection &col)
{
  int count = col.NumPoints();
  glBegin(GL_POLYGON);
  for(int i = 0;i<count;i++)
    {
      Point p = col.Points(i);
      glVertex3f(p.x,p.y,p.z);
    }
  glEnd();
}

void RenderOpenGl(const FaceCollection &col, const TextureCoords &coords)
{
  int count = col.NumFaces();
  int lasttex = -1;
  for(int face = 0;face<count; face++)
    {
      int tex = coords.Texture(face);
      if (tex != -1)
	{
	  if (lasttex == -1)
	    {
	      //glEnable(GL_TEXTURE_2D);
	    }
	  //std::cout << "BIND: " << tex << std::endl;
	  glBindTexture(GL_TEXTURE_2D, tex);
	}
      else
	{
	  if (lasttex != -1)
	    {
	      //glDisable(GL_TEXTURE_2D);
	    }
	}
      int polycount = col.NumPoints(face);
      glBegin( GL_POLYGON );
      for(int v=0;v<polycount;v++)
	{
	  Point2d texcoord = coords.TexCoord(face, v);
	  glTexCoord2f( texcoord.x, texcoord.y );
	  
	  Vector vv = col.PointNormal(face, v);
	  glNormal3f( vv.dx, vv.dy, vv.dz );
	  Point p = col.FacePoint(face, v);
	  glVertex3f( p.x, p.y, p.z );
	}
      glEnd();
    }
  //glDisable(GL_TEXTURE_2D);
}

void RenderOpenGlObjects(const BoxCollectionWithObject &coll, VBOState &vbostate, VBOUpdate u)
{
  int size = coll.NumBoxes();
  for(int i=0;i<size;i++)
    {
      Matrix m = coll.BoxIndex(i);
      int object = coll.BoxObject(i);
      //std::cout << m << " " << object << std::endl;
      glPushMatrix();
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#ifndef EMSCRIPTEN
      glMultMatrixf(&mat[0]);
#endif
      DrawVBO(object, vbostate, u);
      glPopMatrix();
    }
}
void RenderOpenGlObjects(const ObjectDataArray &arr, VBOState &vbostate, VBOUpdate u, ShaderUse *use)
{
  int size = arr.Size();
  for(int i=0;i<size;i++)
    {
      ObjectData data = arr.Index(i);
      Matrix m = data.matrix;
      int object = data.object;
      ShaderParameters *params = data.params;
      if (params&&use) use->set_params(*params);
      glPushMatrix();
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#ifndef EMSCRIPTEN
      glMultMatrixf(&mat[0]);
#endif
      DrawVBO(object, vbostate, u);
      glPopMatrix();
    }
  //if (use)
  //   use->unuse();
}

void ColorElement::SetElemId(int id) 
{ 
  Color2 c = cont[id%cont.size()];
  glColor3f(c.r, c.g, c.b);
  ec.SetElemId(id);
}
CylinderEffect::CylinderEffect(Render *r)
  : FrameAnimPlugins(r)
{
  /*
  BufferRef buf = BufferRef::NewBuffer(256,256);

   int sizex = buf.width;
  int sizey = buf.height;
  int *texturebitmap = buf.buffer;
  for(int y=0;y<sizey;y++)
    for(int x=0;x<sizex;x++)
      {
	texturebitmap[x+y*sizex]=0;
	//texturebitmap[1+x*4+y*128*4]=y*2; // green
	//texturebitmap[2+x*4+y*128*4]=128; // blue
	//texturebitmap[3+x*4+y*128*4]=128; // Alpha
      } 
    //
  // Texture testing
  // 
  Point viewer(0.0, 0.0, 0.0);
  Point light(-1.0,-1.0,-3.0);
  Color light_color(255,255,255);
  Color spec_color(255,128,0);
  
  Point cc(0.0,0.3,0);
  Vector l(0,-0.1,-0.1);
  Cone glass(cc, l, 0.2, 0.02);
  Point cc2 = cc + l;
  Vector l2(0, -0.3, -0.1);
  Cone glass2(cc2, l2, 0.02, 0.02);
  Or or_12(glass, glass2);
  Point cc3 = cc2 + l2;
  Vector l3(0, -0.2, -0.1);
  Cone glass3(cc3, l3, 0.02, 0.3);
  Or or_o3(or_12, glass3);
  Point cc4 = cc3 + l3 + Vector(0.0,-0.04, 0.0);
  Vector l4(0, 0.18, 0.1);
  Cone glass4(cc4, l4, 0.35, 0.02);
  AndNot andnot_o4(or_o3, glass4);
  Phong phong_glass(andnot_o4, viewer, light, light_color, spec_color);
 
  RenderToBitmap(phong_glass, buf);
   //
  //
  //

  plugins.push_back(new TexturePlugin(buf));
 */

}

bool CylinderEffect::Frame(float time)
{
  //if (time > 500.0) return true;
  Matrix b;
  Matrix m = Matrix::ZRotation(time/50.0)*Matrix::YRotation(time/30.0)*Matrix::ZRotation(time/10.0);
  b = m*Matrix::Scale(50.0, 50.0, 50.0)*Matrix::Translate(-150.0*2.5, -160.0*2.5, -150*2.5);
  //b.center = Point(
  //b.u_x = Vector(50.0,0.0,0.0)*m;
  //b.u_y = Vector(0.0,50.0,0.0)*m;
  //b.u_z = Vector(0.0,0.0,50.0)*m;

  CubeGrid g(5,5,5, 150.0, 160.0, 150.0, b);
  //CubeElem c;
  CylinderElem c(20);
  DiskElem c2(20, 1.0);
  BoxableFaceCollection *array[] = { &c2, &c };
  OrElem<BoxableFaceCollection> cc(array, array+2);
  cc.update_faces_cache();
  
  //HorizTexCoords tex(c, ((TexturePlugin*)plugins[0])->Texture());

  //glEnable(GL_TEXTURE_2D);

  MatrixElem ccm(cc, Matrix::Identity());
  //FIX RenderOpenGl(ccm, g, ccm);
  //glDisable(GL_TEXTURE_2D);
  //RenderOpenGl(c2, g, c2);

  return false;
}

bool SphereEffect::Frame(float time)
{
  //if (time > 500.0) return true;
  Matrix b;
  Matrix m = Matrix::ZRotation(time/50.0)*Matrix::YRotation(time/30.0)*Matrix::ZRotation(time/10.0);
  b=m*Matrix::Scale(50.0, 50.0, 50.0)*Matrix::Translate(-150.0*2.5, -160.0*2.5, -150*2.5);
  //b.center = Point(
  //b.u_x = Vector(50.0,0.0,0.0)*m;
  //b.u_y = Vector(0.0,50.0,0.0)*m;
  //b.u_z = Vector(0.0,0.0,50.0)*m;

  CubeGrid g(5,5,5, 150.0, 160.0, 150.0, b);
  //CubeElem c;
  //CylinderElem c(15);
  //DiskElem c2(15, 1.0);
  //BoxableFaceCollection *array[] = { &c2, &c };
  //OrElem cc(array, array+2);
  SphereElem cc(10,10);
  MatrixElem ccm(cc, Matrix::Identity());
  BoxCombine be(g, ccm);
  ColorElement ce(colors, colors+25*5, be);

  // FIX RenderOpenGl(ccm, ce);


  return false;
}


bool CubeEffect::Frame(float time)
{  
  //if (time > 500.0) return true;
  Matrix b;
  Matrix m = Matrix::ZRotation(time/50.0)*Matrix::YRotation(time/30.0)*Matrix::ZRotation(time/10.0);
  b = m*Matrix::Scale(50.0, 50.0, 50.0)*Matrix::Translate(-150.0*2.5, -160.0*2.5, -150*2.5);
  //b.center = Point(
  //b.u_x = Vector(50.0,0.0,0.0)*m;
  //b.u_y = Vector(0.0,50.0,0.0)*m;
  //b.u_z = Vector(0.0,0.0,50.0)*m;
  CubeGrid g(5,5,5, 150.0, 160.0, 150.0, b);
  CubeGrid g2(5,5,5, 500.0, 500.0, 500.0, b);
  AnimBox ab(g2, g, 150);
  ab.ChangeTime(time);
  CubeElem c;
  MatrixElem cm(c, Matrix::Identity());
  BoxCombine be(ab, cm);
  ColorElement ce(colors, colors+25*5, be);
  //FIX RenderOpenGl(cm, ce);
  return false;
}
#undef LoadImage
MoleculeEffect::MoleculeEffect(Render *r)
  : FrameAnimPlugins(r)
{
#if 0
  bool b;
  BufferRef buf1 = LoadImage("./textures/texture_carpet2.jpeg", b);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();
  #endif
#if 0
  BufferRef buf = BufferRef::NewBuffer(512,512);

  int sizex = buf.width;
  int sizey = buf.height;
  int *texturebitmap = buf.buffer;
  for(int y=0;y<sizey;y++)
    for(int x=0;x<sizex;x++)
      {
	texturebitmap[x+y*sizex]=0; //255+((y*2)<<8)+((x*2)<<16)+(255<<24); // red
	//texturebitmap[1+x*4+y*128*4]=y*2; // green
	//texturebitmap[2+x*4+y*128*4]=128; // blue
	//texturebitmap[3+x*4+y*128*4]=128; // Alpha
      }
  for(int i=0;i<10000;i++)
    {
      int x= 1 + (int) (float(sizex) * (rand() / (RAND_MAX + 1.0)));
      int y= 1 + (int) (float(sizey) * (rand() / (RAND_MAX + 1.0)));
      int c = 128 + (int) (float(128) * (rand() / (RAND_MAX + 1.0)));
      texturebitmap[x+y*sizex]=c+((c)<<8)+((c)<<16)+(c<<24); // red
      c/=2;
      texturebitmap[(x+1)+y*sizex]=c+((c)<<8)+((c)<<16)+(c<<24); // red
      texturebitmap[x+(y+1)*sizex]=c+((c)<<8)+((c)<<16)+(c<<24); // red
      texturebitmap[(x-1)+y*sizex]=c+((c)<<8)+((c)<<16)+(c<<24); // red
      texturebitmap[x+(y-1)*sizex]=c+((c)<<8)+((c)<<16)+(c<<24); // red
    }
#endif
  
#if 0
  plugins.push_back(new TexturePlugin(buf));

  //shader.push_back(&globpiece);
  shader.push_back(&texturepiece);
  shader.push_back(&phongpiece);
  shader.push_back(&normalpiece);
  shader.config();
#endif
}
void MoleculeEffect::Init()
{
  FrameAnimPlugins::Init();
  
  //glGenTextures(1, &texture); 
  //glBindTexture(GL_TEXTURE_2D, texture);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturebitmap);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
}
void MoleculeEffect::Cleanup()
{
  glDisable(GL_TEXTURE_2D);
}

GLfloat light_position1[] = { 100.0, 1.0, 0.0, -200.0 };
GLfloat light_position3[] = { 1.0, 100.0, 0.0, -200.0 };


void MoleculeEffect::PreFrame(float time)
{
  //glDisable(GL_LIGHTING);

  glEnable(GL_TEXTURE_2D);
  glColor4f(1.0,1.0,1.0,0.0);

  shader.use();

  //TextureParameters params(0);
  //shader.set_params(params);

  GlobalTextureParameters params3(Point(0,0,0), Vector(2400.0,0.0,0.0), Vector(0.0, 2400.0, 0.0), Vector(0.0, 0.0, 2400.0));
  shader.set_params(params3);
  TextureParameters params4(0);
  shader.set_params(params4);

  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,250,240);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);
  NormalParameters np(Point(300000.0*cos(time/6.0),300000.0*sin(time/3.0),400000.0*sin(time/5.0)),0.9);
  shader.set_params(np);


  glPushMatrix();
  glRotatef(time, 1.0,1.0,0.0);
  CubeElem stars_cube;
  Matrix m[] = { Matrix::Scale(600,600,600) };
  MatrixCollection stars_coll(m,m+1);
  MatrixElem stars_cubem(stars_cube, Matrix::Identity());
  BoxCombine stars_bc(stars_coll, stars_cubem);
  SimpleTexCoords tex(stars_cube, ((TexturePlugin*)plugins[0])->Texture());
  //SphericalTexCoords tex(stars_cube, ((TexturePlugin*)plugins[0])->Texture(), Point(0.0,0.0, 0.0));
  //FIX RenderOpenGl(stars_cube, stars_bc, tex);
  shader.unuse();
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  //glEnable(GL_LIGHTING);
}
bool MoleculeEffect::Frame(float time)
{
  // if (time > 1000.0) return true;
  //
  // stars
  //

  //
  // molecule
  //
  SphereElem cc(30,30);
  Matrix array[] =
    {
      Matrix::Scale(150,150,150)*Matrix::Identity(),
      Matrix::Scale(100,100,100)*Matrix::Translate(0,150*cos(104.45*2.0*3.14159/360.0),150*sin(104.45*2.0*3.14159/360.0)),
      Matrix::Scale(100,100,100)*Matrix::Translate(0,150,0)
    };
  MatrixCollection ab(array, array+3);

  Matrix array2[] =
    {
      Matrix::Scale(150,150,150)*Matrix::Translate(0,-2500,-2500),
      Matrix::Scale(100,100,100)*Matrix::Translate(0,2500*cos(104.45*2.0*3.14159/360.0),2500*sin(104.45*2.0*3.14159/360.0)),
      Matrix::Scale(100,100,100)*Matrix::Translate(0,2500,0)
    };
  MatrixCollection ab2(array2, array2+3);
  AnimBox ab3(ab, ab2, 150);
  ab3.ChangeTime(time-850.0);

  //Matrix m = Matrix::ZRotation(time/50.0)*Matrix::YRotation(time/30.0)*Matrix::ZRotation(time/10.0);
  //Matrix b = Matrix::Translate(-150.0*2.5/100.0, -160.0*2.5/100.0, -150*2.5/100.0);
  //CubeGrid bc(5,5,5, 150.0/100.0, 150.0/100.0, 150.0/100.0, b);
  //CombineBoxCollection combine(bc, ab);
  MatrixElem ccm(cc, Matrix::Identity());
  BoxCombine be(ab3, ccm);
  ColorElement ce(colors, colors+25*5, be);
  // FIX RenderOpenGl(ccm, ce); 
  //glDisable(GL_LIGHTING);
  //glColor3f( 1.00, 1.00, 1.00 ); 
  //Fur fur(cc, 2,2, 0.2);
  //RenderOpenGlLines(fur, be);
  //glEnable(GL_LIGHTING);
  return false;
}
bool TunnelEffect::Frame(float time)
{
    glColor3f(1.0,1.0,1.0);

  LineIn3d line(Point(0.0,0.0,-5800.0), Point(0.0,0.0,5800.0));
  Circle circle;
  Plane plane(Point(0.0,0.0,0.0), Vector(800.0,0.0,0.0), Vector(0.0,800.0,0.0));
  PlaneCurveIn3d circle_plane(circle, plane);
  MultiplyCurves mult(line, circle_plane);
  SampleSurfaceIn3d sample(mult, 0, 30,30);
  RenderOpenGl(sample);

  //SampleCurve3d curve(circle_plane, 50);
  //ContinuousLines l(curve);
  //RenderOpenGlLines(l);

  //
  // Torus...
  //
  {
    glColor3f(1.0,0.5,0.3);
  CircleMatrixCurve2 cmc(150.0, 0.0);
  //Circle circle;
  AnimCircle circle(time/35.0, 4.0);
  Plane plane(Point(0.0,0.0,0.0), Vector(50.0,0.0,0.0), Vector(0.0,50.0,0.0));
  PlaneCurveIn3d circle_plane(circle, plane);
  CurveMultiply cmult(cmc, circle_plane);
  SampleSurfaceIn3d sample(cmult, 0, 40,40);
  RenderOpenGl(sample);
  

  }
  return false;
}
void TunnelEffect2::Init() 
{
  
}

bool TunnelEffect2::Frame(float time)
{
  CubeElem e;
  e.SetBox(Matrix::Identity());
  MatrixElem me(e, Matrix::Scale(500.0,500.0,500.0));
  QuadsToPlanes pl(me); // Array<int, Plane>
  Circle c; // curvein2d
  int s = pl.Size();
  for(int i=0;i<s;i++)
    {
      PlaneCurveIn3d plane(c, pl.Index(i));
      SampleCurve3d sample(plane, 20);
      ContinuousLines lines(sample, true);
      RenderOpenGlLines(lines);
    }
  return false;
}

bool MoleculeInsideEffect::Frame(float time)
{
  //if (time>2000.0) return true;
  SawWaveform saw;
  FitWaveform fit(saw, 150, 100.0, 850.0);
  RepeatWaveform rep(fit, 50);
  Matrix array[] =
    {
      Matrix::Scale(250,250,250)*Matrix::Identity(),
      Matrix::Scale(250,250,250)*Matrix::Translate(0,-rep.Index(time+75.0) /**cos(104.45*2.0*3.14159/360.0)*/,0 /*rep.Index(time)*/ /**sin(104.45*2.0*3.14159/360.0)*/),
      Matrix::Scale(250,250,250)*Matrix::Translate(0,rep.Index(time),0)
    };

  SphereElem cc(10,10);
  MatrixElem m1(cc, array[0]);
  MatrixElem m2(cc, array[1]);
  MatrixElem m3(cc, array[2]);
  RemovePolys rem(m1,m2);
  RemovePolys rema(m1,m3);
  AndFunc and_f(rem, rema);
  MemoizeFunction<bool> and_f_m(m1.NumFaces(), and_f);
  //RemovePolys poly(cc);
  //BoxCombine poly_be(ab, poly);

  //BoxCombine be(m1, rem);
  //ColorElement ce(colors, colors+25*5, be);
  m1.SetBox(Matrix::Identity());
  m2.SetBox(Matrix::Identity());
  m3.SetBox(Matrix::Identity());
  RemovePolys rem2(m2,m1, EDraw);
  NotFunc remnot2(rem2);
  MemoizeFunction<bool> remnot2_m(m2.NumFaces(), rem2);
  glColor3f(1.0,1.0,1.0);

  NotFunc and_f_m_n(and_f_m);

  FilterFaces ff1(m1, and_f_m);
  FilterFaces ff2(m2, remnot2_m);
  //RenderOpenGl(m1, and_f); 
  //RenderOpenGl(m2, remnot2); 

  RemovePolys rem22(m3,m1, EDraw);
  NotFunc remnot22(rem22);
  MemoizeFunction<bool> remnot22_m(m3.NumFaces(), rem22);
  FilterFaces ff3(m3, remnot22_m);
  //RenderOpenGl(m3, remnot22);

  glColor3f(1.0,0.0,0.0);
  IntersectFaces iface(m1,m2);
  //RenderOpenGl(iface);
  glColor3f(1.0,0.0,0.0);
  IntersectFaces iface3(m2,m1);
  SplitPolygons split1(iface, iface3, m2, true);
  split1.DoSplitting();
  SplitPolygons split1a(iface3, iface, m1, false);
  split1a.DoSplitting();
  //RenderOpenGl(iface3);


  glColor3f(1.0,1.0,1.0);

  IntersectFaces iface2(m1,m3);
  // RenderOpenGl(iface2);
  IntersectFaces iface4(m3,m1);

  SplitPolygons split2(iface2, iface4, m3, true);
  split2.DoSplitting();
  SplitPolygons split2a(iface4, iface2, m1, false);
  split2a.DoSplitting();
  //RenderOpenGl(iface4);
  FaceCollection *arr[] = 
    {
      &ff1,
      &ff2,
      &ff3,
      &split1,
      &split2
    };
  OrElem<FaceCollection> or_elem(arr, arr+3);
  or_elem.update_faces_cache();

  glColor3f(1.0,0.5, 0.3);
  RenderOpenGl(or_elem);
  //glColor3f(1.0,0.0,0.0);
  RenderOpenGl(split1);
  RenderOpenGl(split1a);


  //RenderOpenGl(split1a);
  //RenderOpenGl(iface);
  //RenderOpenGl(iface3);
  //glColor3f(0.0,1.0,0.0);
  RenderOpenGl(split2);
  RenderOpenGl(split2a);

  glColor3f(1.0,1.0,1.0);
  glDisable(GL_LIGHTING);
  OutlineFaces out1(m2);
  RenderOpenGlLines(out1);
  OutlineFaces out2(m3);
  RenderOpenGlLines(out2);
  glEnable(GL_LIGHTING);
  //RenderOpenGl(split2a);
  //RenderOpenGl(iface2);
  //RenderOpenGl(iface4);
  return false;
}
RoboEffect::RoboEffect(Render *r)
  : FrameAnim(r)
{
  piece.push_back(&gouraud);
  piece.push_back(&empty);
  piece.use();
}
RoboEffect::~RoboEffect()
{
  piece.unuse();
}
bool RoboEffect::Frame(float time)
{
  //if (time > 1000.0) return true;
  SawWaveform saw;
  FitWaveform fit(saw, 50.0*2.0, 0.0+2.0*3.14159*(180.0-45)/360.0, 2.0*3.14159/4.0+2.0*3.14159*(180.0-45)/360.0);
  RepeatWaveform rep(fit, 30);

  CubeElem cc;

  UnitBox g;
  MatrixBox g2(g, Matrix::Scale(50.0, 50.0, 50.0));
  //MatrixBox g2_2(g2, Matrix::XRotation(time/20.0));
  MatrixBox g2_0(g2, Matrix::Translate(-240.0,0.0,0.0));
  MatrixBox g2_1(g2, Matrix::Translate(-120.0,0.0,0.0));
  MatrixBox g2_2(g2, Matrix::Translate(0.0,0.0,0.0));
  MatrixBox g2_3(g2, Matrix::Translate(0.0,0.0,0.0));
  MatrixBox g2_4(g2, Matrix::Translate(120.0,0.0,0.0));
  MatrixBox g2_5(g2, Matrix::Translate(240.0,0.0,0.0));
  MatrixBox g2_6(g2, Matrix::Translate(-240.0,0.0,0.0));
  MatrixBox g2_7(g2, Matrix::Translate(-120.0,0.0,0.0));
  MatrixBox g2_8(g2, Matrix::Translate(0.0,0.0,0.0));

  MatrixBox g3_1(g2, Matrix::Translate(-120.0,120.0,0.0));
  MatrixBox g3_2(g2, Matrix::Translate(-120.0,-120.0,0.0));
  MatrixBox g3_3(g2, Matrix::Translate(-120.0,0.0,120.0));
  MatrixBox g3_4(g2, Matrix::Translate(-120.0,0.0,-120.0));


  ArrayBox left_leg;
  left_leg.push_back(&g2_0);
  left_leg.push_back(&g2_1);
  left_leg.push_back(&g2_2);  
  MatrixBox left_leg_r(left_leg, Matrix::YRotation(rep.Index(time)));
  MatrixBox left_leg_t(left_leg_r, Matrix::Translate(240.0, 0.0, 0.0));

  ArrayBox right_leg;
  right_leg.push_back(&g2_6);
  right_leg.push_back(&g2_7);
  right_leg.push_back(&g2_8);
  MatrixBox right_leg_r(right_leg, Matrix::YRotation(rep.Index(time+50.0)));
  MatrixBox right_leg_t(right_leg_r, Matrix::Translate(240.0, 240.0, 0.0));
  ArrayBox body;
  body.push_back(&g2_3);
  body.push_back(&g2_4);
  body.push_back(&g2_5);
  MatrixBox body_t(right_leg, Matrix::Translate(240.0, 120.0, 0.0));

  ArrayBox hat;
  hat.push_back(&g3_1);
  hat.push_back(&g3_2);
  hat.push_back(&g3_3);
  hat.push_back(&g3_4);
  MatrixBox hat_t(hat, Matrix::XRotation(time/30.0)*Matrix::Translate(0.0,120.0,0.0));
  ArrayBox b;
  b.push_back(&left_leg_t);
  b.push_back(&right_leg_t);
  b.push_back(&body_t);
  b.push_back(&hat_t);
  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,255,255);
  Color spec_color(255,128,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  piece.set_params(pp);
  //Point hw(phong.HalfWay(viewer,light));
  //pair.set_var("halfway", hw);
  //pair.set_var("light", light);
  //pair.set_var("lc", light_color);
  //pair.set_var("speccolor", spec_color);
  MatrixElem ccm(cc, Matrix::Identity());
  // FIX RenderOpenGl(ccm, b, ccm);
  return false;
}
bool TitleEffect::Frame(float time)
{
  //if (time > 1000.0) return true;
  //glRasterPos3f(0.0, 0.0, 0.0);
  //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'a');
  return false;
}



bool TestEffect::first=true;
bool TestEffect::Frame(float time)
{
  //if (time>1000.0) return true;


  static FaceCollection *obj=0;
  if (first)
    {
      float scale = 4.0;
      SphereElem *elem = new SphereElem(30,30);
      MatrixElem *m1 = new MatrixElem(*elem, Matrix::Scale(150,150,150)*Matrix::Scale(scale,scale,scale));
      MatrixElem *m2 = new MatrixElem(*elem, Matrix::Scale(100,100,100)*Matrix::Translate(0,-150, 0)*Matrix::Scale(scale,scale,scale));
      MatrixElem *m3 = new MatrixElem(*elem, Matrix::Scale(100,100,100)*Matrix::Translate(0,150,0)*Matrix::Scale(scale,scale,scale));
      
      BoxableFaceCollection **array=new BoxableFaceCollection*[3];
      array[0]=m2;
      array[1]=m3;

      //
      // Main sphere
      //
      OrElem<BoxableFaceCollection> *or_elem = new OrElem<BoxableFaceCollection>(array, array+2);
      or_elem->update_faces_cache();
      //AndNotElem *and_not_elem = new AndNotElem(*m1, *or_elem);
      RemovePolys *rempolys = new RemovePolys(*m1, *or_elem);
      MemoizeFunction<bool> *mem = new MemoizeFunction<bool>(m1->NumFaces(), *rempolys);
      FilterFaces *f1 = new FilterFaces(*m1, *mem);

      //
      // smaller sphere
      //
      RemovePolys *rempolys2 = new RemovePolys(*m2, *m1);
      MemoizeFunction<bool> *mem2 = new MemoizeFunction<bool>(m2->NumFaces(), *rempolys2);
      FilterFaces *f2 = new FilterFaces(*m2, *mem2);


      //
      // 2nd smaller sphere
      //
      RemovePolys *rempolys3 = new RemovePolys(*m3, *m1);
      MemoizeFunction<bool> *mem3 = new MemoizeFunction<bool>(m3->NumFaces(), *rempolys3);
      FilterFaces *f3 = new FilterFaces(*m3, *mem3);

      //
      // Gaps
      //
      IntersectFaces *iface12 = new IntersectFaces(*m1,*m2);
      IntersectFaces *iface21 = new IntersectFaces(*m2,*m1);
      IntersectFaces *iface13 = new IntersectFaces(*m1,*m3);
      IntersectFaces *iface31 = new IntersectFaces(*m3,*m1);
      SplitPolygons *split12 = new SplitPolygons(*iface12, *iface21, *m2, true);
      SplitPolygons *split21 = new SplitPolygons(*iface21, *iface12, *m1, true);
      SplitPolygons *split13 = new SplitPolygons(*iface13, *iface31, *m3, true);
      SplitPolygons *split31 = new SplitPolygons(*iface31, *iface13, *m1, true);

      
      //
      // Collect together
      //
      
      FaceCollection **array2 = new FaceCollection*[7];
      array2[0]=f1;
      array2[1]=f2;
      array2[2]=f3;
      array2[3]=split12;
      array2[4]=split21;
      array2[5]=split13;
      array2[6]=split31;
      OrElem<FaceCollection> *or_elem2 = new OrElem<FaceCollection>(array2, array2+7);
      or_elem2->update_faces_cache();
      //FlipNormals *flip = new FlipNormals(*or_elem2);
      MemoizeFaces *memo = new MemoizeFaces(*or_elem2);
      obj = memo;
      first = false;
      or_elem->SetBox(Matrix::Identity());

      split12->DoSplitting();
      split21->DoSplitting();
      split13->DoSplitting();
      split31->DoSplitting();

      //f1and_not_elem->DoSplitting();
    }
  //SphereElem cc(30,10);
  //SphereElem cc2(10,10);
  //MatrixElem m2(cc2, Matrix::Scale(100,100,100)*Matrix::Translate(0,150,0));
  //AndNotElem a(m1,m2);
  //a.SetBox(Matrix::Identity());
  //a.DoSplittin();
  glColor3f(1.0,1.0,1.0);
  RenderOpenGl(*obj);

  return false;
}

  HeightMapEffect::HeightMapEffect(Render *r)
    : FrameAnimPlugins(r) 
  { 
#if 0
    BufferRef buf = BufferRef::NewBuffer(30,30);
    
    int sizex = buf.width;
    int sizey = buf.height;
    int *texturebitmap = buf.buffer;
    for(int y=0;y<sizey;y++)
      for(int x=0;x<sizex;x++)
	{
	  texturebitmap[x+y*sizex]=255+((x/16)<<8)+((255)<<16)+((y/16)<<24); // red
	  //texturebitmap[1+x*4+y*128*4]=y*2; // green
	  //texturebitmap[2+x*4+y*128*4]=128; // blue
	  //texturebitmap[3+x*4+y*128*4]=128; // Alpha
	}

    
#endif
#if 0
    bool b;
    BufferRef buf1 = LoadImage("./textures/texture_carpet3.jpeg", b);
    BitmapFromBuffer buf2(buf1);
    ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
    BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
    BufferFromBitmap buf5(buf4);
    buf5.Gen();
    BufferRef buf = buf5.Buffer();

    plugins.push_back(new TexturePlugin(buf));
#endif
  }
bool HeightMapEffect::Frame(float time)
{
  //if (time>1000.0) return true;
  glEnable(GL_TEXTURE_2D);

  //glColor3f(0.1, 0.3, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  Water w(time);
  HeightMap h(90,90, w);
  //HeightMap h(50,50, w);
  MatrixElem hm(h, Matrix::Translate(-0.5,-0.0,-0.5)* Matrix::XRotation(40.0*3.14159*2.0/360.0)* Matrix::Scale(1650,1650,1650));
  //SimpleTexCoords tex(h, ((TexturePlugin*)plugins[0])->Texture());
  SpaceTexCoords tex(hm, ((TexturePlugin*)plugins[0])->Texture(), Point(0.0,0.0,0.0), Vector(500.0,0.0,0.0), Vector(0.0,500.0,0.0));
  // FIX RenderOpenGl(hm , tex);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0,1.0,1.0);
  SphereElem s(30,30);
  MatrixElem m(s, Matrix::Scale(150.0,150.0,150.0)*Matrix::Translate(0.0,0.0+(300.0-time)*5.0,0.0));
  m.SetBox(Matrix::Identity());
  RenderOpenGl(m);

  return false;
}
BlobEffect::BlobEffect(Render *r)
  : FrameAnimPlugins(r)
{
#if 0
  bool b;
  BufferRef buf1 = LoadImage("./textures/texture_carpet1.jpeg", b);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();

  plugins.push_back(new TexturePlugin(buf));

  shader.push_back(&globpiece);
  shader.push_back(&texturepiece);
  shader.push_back(&phongpiece);
  shader.push_back(&normalpiece);
  shader.config();
#endif
}
BlobEffect::~BlobEffect()
{
  shader.unuse();
}
bool BlobEffect::Frame(float time)
{

  shader.use();
  GlobalTextureParameters params3(Point(0,0,0), Vector(200.0,0.0,0.0), Vector(0.0, 200.0, 0.0), Vector(0.0, 0.0, 200.0));
  shader.set_params(params3);
  TextureParameters params4(0);
  shader.set_params(params4);

  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,250,240);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);
  NormalParameters np(Point(300000.0*cos(time/6.0),300000.0*sin(time/3.0),400000.0*sin(time/5.0)),0.9);
  shader.set_params(np);


  SawWaveform saw;
  FitWaveform fit(saw, 600.0, 0.2, 1.8);
  RepeatWaveform rep(fit, 50);
  float dist = rep.Index(time);
  SphereElem s(30,30);
  MatrixElem m(s, Matrix::Translate(dist,0.0,0.0));

  SphereElem s1(30,30);
  MatrixElem m1(s1, Matrix::Translate(-dist,0.0,0.0));

  s.SetBox(Matrix::Identity());
  s1.SetBox(Matrix::Identity());

  BlobPointMap p(Point(dist,0.0,0.0), Point(-dist,0.0,0.0), 1.0);
  ModifyObject mo(m, p);
  
  BlobPointMap p1(Point(-dist,0.0,0.0), Point(dist,0.0,0.0), 1.0);
  ModifyObject mo1(m1,p1);

  MatrixElem mm(mo, Matrix::Scale(200.0,200.0,200.0));
  MatrixElem mm1(mo1, Matrix::Scale(200.0,200.0,200.0));

  RenderOpenGl(mm);
  RenderOpenGl(mm1);

  return false;
}

VBOEffect::VBOEffect(Render *r) 
  : FrameAnim(r)
{
  piece.push_back(&phong);
  piece.push_back(&empty);
  //piece.push_back(&blob);
  piece.use();
}
void VBOEffect::Init()
{
  SphereElem s(30,30); // 30*30 quads
  //CubeElem s;
  MatrixElem m(s, Matrix::Scale(40.0,40.0,40.0));
  m.SetBox(Matrix::Identity());
  QuadsToTris q(m); // faces=30*30*2, points=30*30*4
  FaceCollectionTri *array[] = { &q };
  ObjectArray oa(array, array+1);
  VBO vbo(oa);
  UpdateVBO(vbo, 0, vbostate, UpdateVertexNormalIndex);
}
VBOEffect::~VBOEffect()
{
  piece.unuse();
}
bool VBOEffect::Frame(float time)
{

  // spheres
  CircleMatrixCurve2 cmc((1500.0-time*2.0)/*/40.0*/, 0.0);
  //Circle circle;
  AnimCircle circle(time/35.0, 4.0);
  Plane plane(Point(0.0,0.0,0.0), Vector((50.0+time/5)/*/40.0*/,0.0,0.0), Vector(0.0,(50.0+time/5.0)/*/40.0*/,0.0));
  PlaneCurveIn3d circle_plane(circle, plane);
  CurveMultiply cmult(cmc, circle_plane);
  SampleSurfaceForGrid sample(cmult, 30,30);
  PointCollectionFromGrid pcoll(sample);
  PointsToBoxCollection pbox2(pcoll);
  ConstantObjectCollection objcoll(pbox2.NumBoxes(), 0);
  //CombineBoxesAndObjects pcombine(pbox2, objcoll);

  // Shading
  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,255,255);
  Color spec_color(60,192,60);
  PhongParameters pp(viewer, light, light_color, spec_color);
  RandomPositionParameters rp(Point(20.0,20.0,20.0), time);
  CombineParameters pc(pp,rp);
  DuplicateArray<int, ShaderParameters*> d_1(&pc,pbox2.NumBoxes()); 
  ShaderParameterArrayConvert d(d_1);
  //piece.use(pc);
  //FindNearestPoint nearest(pcoll);
  //DuplicateArray<int,float> radius_1(1.0, pbox2.NumBoxes());
  //FloatArrayConvert radius(radius_1);
  //BlobParameterArray blobparams(pcoll, nearest, radius);
  //CombineParameterArrays combinedarrays(blobparams, d);
  //DuplicateArray<int,Point> points(Point(0.0,0.0,0.0), pbox2.NumBoxes());
  //PointCollectionConvert points2(points);
  //PointsToBoxCollection points3(points2);

  CombineObjectData pcombine(pbox2 /*points3*/, objcoll, d /*combinedarrays*/);

  RenderOpenGlObjects(pcombine, vbostate, UpdateVertexNormalIndex, &piece);
  //piece.unuse();
  //SampleSurfaceIn3d sample(cmult, 0, 40,40);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glDepthMask(false);
  //glColor4f(1.0,1.0,0.0,0.5);
  //for(int y=0;y<sample.YSize();y++)
  //  for(int x=0;x<sample.XSize();x++)
  //    {
  //	Point p = sample.Index(x,y);
  //glPushMatrix();
  //	glTranslatef(p.x,p.y,p.z);
  //	DrawVBO(0, vbostate, UpdateVertexNormal);
  //	glPopMatrix();
  //   }
  //glDepthMask(true);
  //glDisable(GL_BLEND);
  return false;
}
bool CircleEffect::Frame(float time)
{
  Circle c;
  Plane pl(Point(0.0,0.0,0.0), Vector(500.0,0.0,0.0), Vector(0.0,0.0,500.0)); 
  PlaneCurveIn3d pc(c, pl);
  TranslateToMatrixCurve mc(pc);

  SpiralMatrixCurve screw(2.2);
  CircleMatrixCurve circlecurve(0.0);
  MultiplyMatrixCurve mult(circlecurve, screw);

  MultiplyMatrixCurve mc2(mult,mc);

  MatrixCurveToCurveIn3d curv(mc2, Vector(0.0,0.0,0.0));
  SampleCurve3d sample(curv, 100);
  ContinuousLines lines(sample, true);
  
  Camera cam(mc2, time/60.0);
  RenderOpenGlLines(lines);

  Point p(0.0,0.0,0.0);
  SingletonArray<Point> sa(p);
  PointCollectionConvert pcoll(sa);
  PointsToBoxCollection pbox2(pcoll);
  ConstantObjectCollection objcoll(pbox2.NumBoxes(), 0);
  CombineBoxesAndObjects pcombine(pbox2, objcoll);
  RenderOpenGlObjects(pcombine, vbostate, UpdateVertexNormalIndex);
  return false;
}

void CircleEffect::Init()
{
  //Circle c;
  //Plane p(Point(0.0,0.0,0.0), Vector(50.0,0.0,0.0), Vector(0.0,50.0,0.0)); 
  //PlaneCurveIn3d pc(c,p);
  //SampleCurve3d samples(pc, 50);
  StarCurve samples(100.0, 50.0, 20);
  //ContinuousLines lines(samples, true);
  //RenderOpenGlLines(lines);
  //RenderOpenGlPoly(samples);
  
  Circle c;
  Plane pl(Point(0.0,0.0,0.0), Vector(500.0,0.0,0.0), Vector(0.0,0.0,500.0)); 
  PlaneCurveIn3d pc(c, pl);
  SampleCurve3d sampl(pc, 150);
  //DiffVectorCollection diff(sampl, true);

  //SimpleVectorCollection v(Vector(0.0,0.0,100.0));
  SpiralMatrixCurve spiral(2.2);
  CircleMatrixCurve circlecurve(0.0);
  MultiplyMatrixCurve mult(circlecurve, spiral);
  SpanObject span(samples, sampl , mult);
  SampleGrid grid(span, 0);

  QuadsToTris q(grid); // faces=30*30*2, points=30*30*4
  FaceCollectionTri *array[] = { &q };
  ObjectArray oa(array, array+1);
  VBO vbo(oa);
  UpdateVBO(vbo, 0, vbostate, UpdateVertexNormalIndex);

  //RenderOpenGl(grid);

}

bool PathEffect::Frame(float time)
{
  //
  //
  //
  Point array_b[] = { Point(0.0,0.0,200.0),
		    Point(200.0,0.0,0.0),
		    Point(0.0,200.0,0.0),
                    Point(200.0,200.0,200.0) };
  PointCollectionArray p(array_b, array_b+4);
  BezierCurve cn(p);
  SampleCurve3d sample_c(cn, 20);
  ContinuousLines cont_sample_c(sample_c, false);
  RenderOpenGlLines(cont_sample_c);

  //
  // motor
  //
  CylinderElem c(20);
  DiskElem c2(20, 1.0);
  c2.SetBox(Matrix::Identity());
  c.SetBox(Matrix::Identity());
  SphereElem c3(20,20);
  MatrixElem c3m(c3, Matrix::Translate(0.0,0.0,-1.0));
  c3.SetBox(Matrix::Identity()); 

  // 
  // base
  //
  SphereElem s(20,20);
  MatrixElem sm(s, Matrix::Translate(0.0,-4.0, 0.0));
  s.SetBox(Matrix::Identity());

  SphereElem ce(20,20);
  MatrixElem cm(ce, Matrix::Translate(0.0,-4.05/4.1, 0.0)*Matrix::Scale(4.1,4.1,4.1)*Matrix::Translate(0.0,-3.3,0.0));
  ce.SetBox(Matrix::Identity());

  AndNotElem ane(sm, cm);
  ane.SetBox(Matrix::Identity());
  ane.DoSplitting();
  MatrixElem anem(ane, Matrix::Scale(1.2,1.0,3.0)*Matrix::Translate(0.0,-3.0,0.0));

  CylinderElem cyl(20);
  cyl.SetBox(Matrix::Identity());
  MatrixElem cylm(cyl, Matrix::YRotation(2.0*3.14159*90.0/360.0)*Matrix::ZRotation(2.0*3.14159*90.0/360.0)*Matrix::Scale(0.5,0.5,0.5)*Matrix::Translate(0.0,-2.5,0.0));
  
  CubeGrid grid(1,5,1, 0.0, 1.1, 0.0, Matrix::Identity()); 
  ConstantObjectSelection sel(&cylm);
  BoxedElem be(grid, sel);
  MatrixElem bem(be, Matrix::XRotation(2.0*3.14159*45.0/360.0)*Matrix::Scale(0.5,0.5,0.5)*Matrix::Translate(0.0, -5.0,0.0));

  CubeGrid grid2(1,5,1, 0.0, 1.1, 0.0, Matrix::Identity()); 
  ConstantObjectSelection sel2(&cylm);
  BoxedElem be2(grid2, sel2);
  MatrixElem bem2(be2, Matrix::XRotation(2.0*3.14159*-45.0/360.0)*Matrix::Scale(0.5,0.5,0.5)*Matrix::Translate(0.0, -3.0,0.0));


  BoxableFaceCollection *array[] = { &c, &c2, &c3m, &anem, &bem, &bem2 };
  OrElem<BoxableFaceCollection> cc(array, array+6);
  cc.update_faces_cache();
  MatrixElem m(cc, Matrix::Scale(50.0,50.0,50.0));
  RenderOpenGl(m);
  return false;
}

Matrix AnimBox::Index(int box) const
{
  Matrix cc = c.BoxIndex(box);
  Matrix cc2 = c2.BoxIndex(box);

  Matrix m;
  for(int i=0;i<16;i++)
    m.matrix[i]=TransformFloat(cc.matrix[i], cc2.matrix[i], 0.0, length, time);
  m.is_identity=false;

  //Box b;
  // b.center = TransformPoint(cc.center, cc2.center, 0.0, length, time);
  //b.u_x = TransformVector(cc.u_x, cc2.u_x, 0.0, length, time);
  //b.u_y = TransformVector(cc.u_y, cc2.u_y, 0.0, length, time);
  //b.u_z = TransformVector(cc.u_z, cc2.u_z, 0.0, length, time);
  return m;
}
Matrix CubeGrid::Index(int box) const
{
  int zz = box / (x*y);
  int ddz = box - zz * (x*y);
  int yy = ddz / x;
  int ddy = ddz - yy*x;
  int xx = ddy;
  
  
  
  Matrix bb = b*Matrix::Translate(float(xx)*dx, float(yy)*dy, float(zz)*dz);
  //Box bb;
  //bb.center = b.center + Vector(float(xx)*dx, float(yy)*dy, float(zz)*dz);
  //bb.u_x = b.u_x;
  //bb.u_y = b.u_y;
  //bb.u_z = b.u_z;
  
  return bb;
}

float Solve(const Function<float, float> &func, float t_0, float t_1)
{
  float t = t_0;
  for(;t<t_1;)
    {
      float Ht = func.Index(t);
      if (Ht < 0.1) return t;
      t += Ht / 20.0;
    }
  return t_0;
}

float SolveWithFailure(const Function<float, float> &func, float t_0, float t_1, bool &success)
{
  float t = t_0;
  for(;t<t_1;)
    {
      float Ht = func.Index(t);
      if (Ht < 0.1) { success = true; return t; }
      t += Ht / 20.0;
    }
  success = false;
  return t_0;
}


float InterpolateWaveform::Index(float time) const
{    
  std::vector<Point2d>::const_iterator i=cont.begin();
  for(;i+1<cont.end();i++)
    {
      float start_time = (*(i)).x;
      if (time > start_time && time <= (*(i+1)).x)
	{
	  break;
	}
    }
  return TransformFloat((*i).y, (*(i+1)).y, (*i).x, (*(i+1)).x, time);
}

Point2d SawWaveform::array[3] = { {0.0,0.0},{0.5,1.0},{1.0,0.0} };

void SplitPolygons::DoSplitting()
{
  
    int num_1 = a1.NumFaces();
    for(int i1=0;i1<num_1;i1++)
      {
	Point p = points1.Points(i1);
	float dist = 99999.0;
	int num_2 = a2.NumFaces();
	int matching_face = -1;
	//std::cout << "Start i1 " << std::endl;
	for(int i2=0;i2<num_2;i2++)
	  {
	    Point p2 = points2.Points(i2);
	    float dist_p_p2 = (p2-p).Dist();
	    //std::cout << "Dist: " << dist_p_p2 << std::endl;
	    if (dist_p_p2 < dist)
	      {
		//std::cout << "yes" << std::endl;
		dist = dist_p_p2;
		matching_face=i2;
	      }
	  }
	if (matching_face==-1) continue;
	
	//
	// Here we have (i1,matching_face) pairs
	//
	int index1 = i1;
	int index2 = matching_face;

	//Point pa = points1.Points(index1);
	//Point pb = points2.Points(index2);


	std::vector<Point> vec;
	std::vector<Vector> norm;
	bool bb = ops.Inside(a1.FacePoint(index1, 0));
	if (b) { bb = !bb; }
	SplitPolygon(vec, norm, a1, index1, a2.FacePoint(index2, 0), a2.FacePoint(index2, 1), a2.FacePoint(index2, 2), bb);
	OnePolygon g;
	g.poly = vec;
	OneNormal gg;
	gg.poly = norm;

	faces.push_back(g);
	normals.push_back(gg);
      }
    

}

//#define VBOS
#define DISPLAY_LISTS

class VBOStatePriv
{
public:
  GLuint *id;
  //GLuint id[5]; 
  bool initialized;
  GLfloat *vertex;
  GLfloat *normal;
  GLfloat *texture;
  GLbyte *color;
  std::map<int, GLfloat*> attribs;
  std::map<int, GLshort*> attribsI;
  int indexsize;
  int **index;
  struct State
  {
    int startrange;
    int endrange;
    int startindexrange;
    int endindexrange;
  };
  std::map<int, State> ranges;
  GLuint *displaylist;
  int numobjects;
  int numattribs;
};

#if 0
// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;					// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;					// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;					// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;				// VBO Deletion Procedure
PFNGLBUFFERSUBDATAARBPROC  glBufferSubDataARB = NULL;
#endif

VBOState::VBOState()
{
  impl = new VBOStatePriv;
  impl->initialized = false;

  //bool g_fVBOSupported = IsExtensionSupported( "GL_ARB_vertex_buffer_object" );
  //if( g_fVBOSupported )
  //{
  std::cout << glGetString( GL_EXTENSIONS ) << std::endl;
      // Get Pointers To The GL Functions
#ifdef VBOS

#if 0
      glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
      glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
      glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
      glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");
      glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)SDL_GL_GetProcAddress("glBufferSubDataARB");

#endif
#endif
      //}
  impl->displaylist = 0;
  impl->id = 0;
  impl->vertex=0;
  impl->normal = 0;
  impl->texture =0;
  impl->color = 0;
  impl->index = 0;
}
VBOState::~VBOState()
{
#ifdef DISPLAY_LISTS
  if (impl->initialized)
    for(int i=0;i<impl->numobjects;i++)
      if (impl->displaylist[i])
	glDeleteLists(impl->displaylist[i], 1);
#endif
#ifdef VBOS
  glDeleteBuffersARB(4+impl->numobjects, &impl->id[0]);
#endif
  delete [] impl->id;
  delete [] impl->vertex;
  delete [] impl->normal;
  delete [] impl->texture;
  delete [] impl->color;
  delete [] impl->index;
  delete impl;
}
GLfloat mat_ambient1[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_diffuse1[] = { 0.9, 0.7, 0.1, 1.0 };
GLfloat mat_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess1[] = { 50.0 };

void UpdateVBO(const FaceCollection &faces, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs)
{
  if (faces.NumPoints(0) == 3)
    {
      TrisToTris q(faces);
      FaceCollectionTri *arr[] = { &q };
      ObjectArray tri(arr, arr+1);
      VBO v(tri);
      UpdateVBO(v, state, u, attribs);
    }
  if (faces.NumPoints(0) == 4)
    {
      QuadsToTris q(faces);
      FaceCollectionTri *arr[] = { &q };
      //VectorArray<FaceCollectionTri*> vecarr(arr, arr+1);
      ObjectArray tri(arr,arr+1);
      VBO v(tri);
      UpdateVBO(v, state, u, attribs);
    }
}
void UpdateVBO(const FaceCollectionVBO &faces, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs)
{
  int num = faces.NumObjects();
  for(int i=0;i<num;i++)
    UpdateVBO(faces, i, state, u, attribs);
}

void UpdateVBO(const FaceCollectionVBO &faces, int obj, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs)
{
  std::cout << "Update: " << u << std::endl;
  std::cout << "CurrentObject: " << obj << std::endl;
  if (!state.impl->initialized)
    state.impl->numobjects = faces.NumObjects();
  state.impl->numattribs = attribs.size();
  std::cout << "numobjects: " << state.impl->numobjects << std::endl;
#ifdef DISPLAY_LISTS
  if (!state.impl->displaylist)
    {
      state.impl->displaylist = new GLuint[state.impl->numobjects];
      for(int i=0;i<state.impl->numobjects;i++)
	state.impl->displaylist[i]=0;
    }
  state.impl->displaylist[obj] = glGenLists(1);
#endif

#ifdef VBOS
  if (!state.impl->initialized)
    {
    state.impl->id = new GLuint[4+state.impl->numobjects+attribs.size()];
    glGenBuffersARB(4+state.impl->numobjects+attribs.size(), &state.impl->id[0]);
    }
#endif

  int rangestart = faces.StartRange(obj);
  int rangeend = faces.EndRange(obj);
  int rangestarti = faces.StartRangeIndex(obj);
  int rangeendi = faces.EndRangeIndex(obj);

  std::cout << "ranges: (" << rangestart << "," << rangeend << "), (" << rangestarti << "," << rangeendi << ")" << std::endl;

  VBOStatePriv::State s;
  s.startrange = rangestart;
  s.endrange = rangeend;
  s.startindexrange = rangestarti;
  s.endindexrange = rangeendi;
  state.impl->ranges[obj]=s;

  if (!state.impl->initialized)
    {
      rangestart = 0;
      rangeend = faces.NumPoints();
      //u = UpdateAll;
    }
  if (u & UpdateVertex)
    glEnableClientState(GL_VERTEX_ARRAY);
  if (u & UpdateNormal)
    glEnableClientState(GL_NORMAL_ARRAY);
  if (u & UpdateColor)
    glEnableClientState(GL_COLOR_ARRAY);
  //if (u & UpdateTexture)
  // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  for(std::vector<Attrib>::const_iterator i = attribs.begin();i!=attribs.end();i++)
    glEnableVertexAttribArray((*i).loc);
  
  if (u & UpdateVertex)
    {
#ifdef VBOS

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[0]); // vertex

#endif
      if (!state.impl->initialized)
	{
	  state.impl->vertex = new GLfloat[(rangeend-rangestart)*3];
	  faces.UpdateVertexList(rangestart, rangeend, state.impl->vertex);
#ifdef VBOS
	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, (rangeend-rangestart)*3*sizeof(GLfloat), state.impl->vertex, GL_STATIC_DRAW_ARB);
#endif
	}
      else
	{
	  faces.UpdateVertexList(rangestart, rangeend, state.impl->vertex);
#ifdef VBOS

	  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, rangestart*3*sizeof(GLfloat), (rangeend-rangestart)*3*sizeof(GLfloat), state.impl->vertex);
#endif
	}
#ifdef DISPLAY_LISTS
      glVertexPointer(3, GL_FLOAT, 0, state.impl->vertex); // 0 if ARB
#endif
    }
  if (u & UpdateNormal)
    {
#ifdef VBOS

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[1]); // normal
#endif
      if (!state.impl->initialized)
	{
	  state.impl->normal = new GLfloat[(rangeend-rangestart)*3];
	  faces.UpdateNormalList(rangestart, rangeend, state.impl->normal);
#ifdef VBOS

	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, (rangeend-rangestart)*3*sizeof(GLfloat), state.impl->normal, GL_STATIC_DRAW_ARB);
#endif
	}
      else
	{

	  faces.UpdateNormalList(rangestart, rangeend, state.impl->normal);
#ifdef VBOS

	  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, rangestart*3*sizeof(GLfloat), (rangeend-rangestart)*3*sizeof(GLfloat), state.impl->normal);
#endif
	}
#ifdef DISPLAY_LISTS
      glNormalPointer(GL_FLOAT, 0, state.impl->normal);
#endif
    }
  if (u & UpdateTexture)
    {
#ifdef VBOS

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[2]); // texture
#endif
      if (!state.impl->initialized)
	{
	  state.impl->texture = new GLfloat[(rangeend-rangestart)*2];
	  faces.UpdateTextureCoordList(rangestart, rangeend, state.impl->texture);
#ifdef VBOS
	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, (rangeend-rangestart)*2*sizeof(GLfloat), state.impl->vertex, GL_STATIC_DRAW_ARB);
#endif
	}
      else
	{
	  faces.UpdateTextureCoordList(rangestart, rangeend, state.impl->texture);
#ifdef VBOS

	  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, rangestart*3*sizeof(GLfloat), (rangeend-rangestart)*3*sizeof(GLfloat), state.impl->texture);
#endif
	}
#ifdef DISPLAY_LISTS
  //glTexCoordPointer(4, GL_UNSIGNED_BYTE, 0, state.impl->texture);
#endif
    }
  if (u & UpdateColor)
    {
#ifdef VBOS

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[3]); // color
#endif
      //faces.UpdateColorList(rangestart, rangeend, (char*)state.impl->color);
      if (!state.impl->initialized)
	{
	  state.impl->color = new GLbyte[(rangeend-rangestart)*4];
	  faces.UpdateColorList(rangestart, rangeend, (char*)state.impl->color);
#ifdef VBOS

	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, (rangeend-rangestart)*4*sizeof(GLbyte), state.impl->color, GL_STATIC_DRAW_ARB);
#endif
	}
      else
	{
	  faces.UpdateColorList(rangestart, rangeend, (char*)state.impl->color);
#ifdef VBOS

	  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, rangestart*4*sizeof(GLbyte), (rangeend-rangestart)*4*sizeof(GLbyte), state.impl->color);
#endif
	}
#ifdef DISPLAY_LISTS
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, state.impl->color);
#endif

    }
  int ii=0;
  for(std::vector<Attrib>::const_iterator i = attribs.begin();i!=attribs.end();i++,ii++)
    {
#ifdef VBOS

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[4+state.impl->numobjects+ii]); // color
#endif

      if (!state.impl->initialized)
	{
	  if ((*i).is_int)
	    {
	      state.impl->attribsI[(*i).loc] = new GLshort[(rangeend-rangestart)];
	      faces.UpdateAttribI(rangestart, rangeend, state.impl->attribsI[(*i).loc], (*i).id);
	    }
	  else
	    {
	      state.impl->attribs[(*i).loc] = new GLfloat[(rangeend-rangestart)];
	      faces.UpdateAttrib(rangestart, rangeend, state.impl->attribs[(*i).loc], (*i).id);
	    }
#ifdef VBOS

	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, (rangeend-rangestart)*sizeof(GLfloat), state.impl->attribs[(*i).loc], GL_STATIC_DRAW_ARB);
#endif

	}
      else
	{
	  if ((*i).is_int)
	    {
	      faces.UpdateAttribI(rangestart, rangeend, state.impl->attribsI[(*i).loc], (*i).id);
	    }
	  else
	    {
	      faces.UpdateAttrib(rangestart, rangeend, state.impl->attribs[(*i).loc], (*i).id);
	    }
#ifdef VBOS

	  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, rangestart*sizeof(GLfloat), (rangeend-rangestart)*sizeof(GLfloat), state.impl->attribs[(*i).loc]);
#endif

	}
#ifdef DISPLAY_LISTS
      if ((*i).is_int)
	{
	  glVertexAttribPointer((*i).loc, 1, GL_SHORT, 0, 0, state.impl->attribsI[(*i).loc]);
	}
      else
	{
	  glVertexAttribPointer((*i).loc, 1, GL_FLOAT, 0, 0, state.impl->attribs[(*i).loc]);
	}
#endif

    }

  if (u & UpdateIndex)
    {
#ifdef VBOS
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, state.impl->id[4+obj]); // index
#endif
      if (!state.impl->initialized)
	{
	  state.impl->indexsize = faces.IndexSize();
	  state.impl->index = new int*[state.impl->numobjects];
	  for(int i=0;i<state.impl->numobjects;i++)
	    state.impl->index[i] = 0;
	  state.impl->index[obj] = new int[(rangeendi-rangestarti)*3];
	  faces.UpdateIndexList(rangestarti, rangeendi, state.impl->index[obj]);
#ifdef VBOS

	  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, (rangeendi-rangestarti)*sizeof(GLshort)*3, state.impl->index[obj], GL_STATIC_DRAW_ARB);
#endif
	}
      else
	{
	  bool b = false;
	  if (!state.impl->index[obj])
	    {
	    state.impl->index[obj] = new int[(rangeendi-rangestarti)*3];
	    b = true;
	    }
	  (void)b;
	  faces.UpdateIndexList(rangestarti, rangeendi, state.impl->index[obj]);
#ifdef VBOS

	  if (b)
	    {
	      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, (rangeendi-rangestarti)*sizeof(GLshort)*3, state.impl->index[obj], GL_STATIC_DRAW_ARB);
	    }
	  else
	    {
	      glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0 /*rangestart*sizeof(GLshort)*/, (rangeendi-rangestarti)*sizeof(GLshort)*3, state.impl->index[obj]);
	    }
#endif
	}


    }
#ifdef DISPLAY_LISTS
  glNewList(state.impl->displaylist[obj], GL_COMPILE);
  glBegin( GL_TRIANGLES );
  for(int i=0;i<rangeendi-rangestarti /*state.impl->indexsize*/;i+=2)
      {
	int vertex1 = state.impl->index[obj][i*3];
	int vertex2 = state.impl->index[obj][i*3+1];
	int vertex3 = state.impl->index[obj][i*3+2];
	for(std::vector<Attrib>::const_iterator ii = attribs.begin();ii!=attribs.end();ii++)
	  {
	    if ((*ii).is_int)
	      {
		GLshort a = state.impl->attribsI[(*ii).loc][vertex1];
		//std::cout << "1:" << (*ii).loc << " " << vertex1 << " " << a << std::endl;
		glVertexAttrib1s((*ii).loc,a);
	      }
	    else
	      {
		GLfloat a = state.impl->attribs[(*ii).loc][vertex1];
		//std::cout << "1:" << (*ii).loc << " " << vertex1 << " " << a << std::endl;
		glVertexAttrib1f((*ii).loc,a);
	      }
	  }
	if (u & UpdateColor)
	  {
	    unsigned char r = state.impl->color[vertex1*4];
	    unsigned char g = state.impl->color[vertex1*4+1];
	    unsigned char b = state.impl->color[vertex1*4+2];
	    unsigned char a = state.impl->color[vertex1*4+3];
	    //std::cout << std::hex << r << " " << g << " " << b << std::endl;

	    glColor4ub(r,g,b,a);
	  }
	if (u & UpdateTexture)
	  {
	    float tx = state.impl->texture[vertex1*2];
	    float ty = state.impl->texture[vertex1*2+1];
	    //std::cout << tx << " " << ty << std::endl;
	    glTexCoord2f( tx, ty );
	  }

	vertex1 *= 3;
	GLfloat nx, ny, nz;
	if (u & UpdateNormal)
	  {
	    //std::cout << i << " " << vertex1 << std::endl;
	    nx = state.impl->normal[vertex1];
	    ny = state.impl->normal[vertex1+1];
	    nz = state.impl->normal[vertex1+2];
	    glNormal3f(nx,ny,nz);
	  }
	GLfloat x,y,z;
	if (u & UpdateVertex)
	  {
	    x = state.impl->vertex[vertex1];
	    y = state.impl->vertex[vertex1+1];
	    z = state.impl->vertex[vertex1+2];
	    glVertex3f(x,y,z);
	    //std::cout << "Vertex1: "<< vertex1 << " " << x << " " << y << " " << z << std::endl;
	  }

	for(std::vector<Attrib>::const_iterator ii = attribs.begin();ii!=attribs.end();ii++)
	  {
	    if ((*ii).is_int)
	      {
		GLshort a = state.impl->attribsI[(*ii).loc][vertex2];
	    //std::cout << "2:" << (*ii).loc << " " << vertex2 << " " << a << std::endl;
		glVertexAttrib1s((*ii).loc,a);
	      }
	    else
	      {
		GLfloat a = state.impl->attribs[(*ii).loc][vertex2];
	    //std::cout << "2:" << (*ii).loc << " " << vertex2 << " " << a << std::endl;
		glVertexAttrib1f((*ii).loc,a);
	      }
	  }

	if (u & UpdateColor)
	  {
	    unsigned char r = state.impl->color[vertex2*4];
	    unsigned char g = state.impl->color[vertex2*4+1];
	    unsigned char b = state.impl->color[vertex2*4+2];
	    unsigned char a = state.impl->color[vertex2*4+3];
	    glColor4ub(r,g,b,a);
	  }
	if (u & UpdateTexture)
	  {
	    float tx = state.impl->texture[vertex2*2];
	    float ty = state.impl->texture[vertex2*2+1];
	    glTexCoord2f( tx, ty );
	  }

	vertex2 *= 3;
	if (u & UpdateNormal)
	  {
	    nx = state.impl->normal[vertex2];
	    ny = state.impl->normal[vertex2+1];
	    nz = state.impl->normal[vertex2+2];
	    glNormal3f(nx,ny,nz);
	  }
	if (u & UpdateVertex)
	  {
	    x = state.impl->vertex[vertex2];
	    y = state.impl->vertex[vertex2+1];
	    z = state.impl->vertex[vertex2+2];
	    glVertex3f(x,y,z);
	    //std::cout << "Vertex2: "<< vertex2 << " " << x << " " << y << " " << z << std::endl;
	  }

	for(std::vector<Attrib>::const_iterator ii = attribs.begin();ii!=attribs.end();ii++)
	  {
	    if ((*ii).is_int)
	      {
		GLshort a = state.impl->attribsI[(*ii).loc][vertex3];
	    //std::cout << "3:" << (*ii).loc << " " << vertex3 << " " << a << std::endl;
		glVertexAttrib1s((*ii).loc,a);
	      }
	    else
	      {
		GLfloat a = state.impl->attribs[(*ii).loc][vertex3];
	    //std::cout << "3:" << (*ii).loc << " " << vertex3 << " " << a << std::endl;
		glVertexAttrib1f((*ii).loc,a);
	      }
	  }
	if (u & UpdateColor)
	  {
	    unsigned char r = state.impl->color[vertex3*4];
	    unsigned char g = state.impl->color[vertex3*4+1];
	    unsigned char b = state.impl->color[vertex3*4+2];
	    unsigned char a = state.impl->color[vertex3*4+3];
	    //std::cout << int(r) << " " << int(g) << " " << int(b) << std::endl;
	    glColor4ub(r,g,b,a);
	  }
	if (u & UpdateTexture)
	  {
	    float tx = state.impl->texture[vertex3*2];
	    float ty = state.impl->texture[vertex3*2+1];
	    glTexCoord2f( tx, ty );
	  }

	vertex3 *= 3;
	if (u & UpdateNormal)
	  {
	    nx = state.impl->normal[vertex3];
	    ny = state.impl->normal[vertex3+1];
	    nz = state.impl->normal[vertex3+2];
	    glNormal3f(nx,ny,nz);
	  }
	if (u & UpdateVertex)
	  {
	    x = state.impl->vertex[vertex3];
	    y = state.impl->vertex[vertex3+1];
	    z = state.impl->vertex[vertex3+2];
	    glVertex3f(x,y,z);
	    //std::cout << "Vertex3: "<< vertex3 << " " << x << " " << y << " " << z << std::endl;	
	  }

      }
    glEnd();
#endif
#ifdef VBOS
    //std::cout << "glDrawElements " << std::endl;
    //glDrawElements(GL_TRIANGLES, /*rangestart, rangeend,*/ state.impl->indexsize*3, GL_UNSIGNED_INT, state.impl->index);
    //std::cout << "glDrawElements_ " << std::endl;
#endif

#ifdef DISPLAY_LISTS
  glEndList();
#endif
  if (u & UpdateVertex)
    glDisableClientState(GL_VERTEX_ARRAY);
  if (u & UpdateNormal)
    glDisableClientState(GL_NORMAL_ARRAY);
  if (u & UpdateColor)
    glDisableClientState(GL_COLOR_ARRAY);
  //if (u & UpdateTexture)
  // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  for(std::vector<Attrib>::const_iterator i = attribs.begin();i!=attribs.end();i++)
    glDisableVertexAttribArray((*i).loc);


  state.impl->initialized = true;
}
void DrawVBO(VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs)
{
  int num = state.impl->numobjects;
  for(int i=0;i<num;i++)
    DrawVBO(i, state, u, attribs);
}

void DrawVBO(int obj, VBOState &state, VBOUpdate u, const std::vector<Attrib> &attribs)
{
  //std::cout << "DrawVBO: obj=" << obj << std::endl;
#ifdef DISPLAY_LISTS
  glCallList(state.impl->displaylist[obj]);
#endif
#ifdef VBOS

  int rangestart = state.impl->ranges[obj].startrange; 
  int rangeend = state.impl->ranges[obj].endrange;
  int rangestarti = state.impl->ranges[obj].startindexrange; 
  int rangeendi = state.impl->ranges[obj].endindexrange;

  if (u & UpdateVertex)
    glEnableClientState(GL_VERTEX_ARRAY);
  if (u & UpdateNormal)
    glEnableClientState(GL_NORMAL_ARRAY);
  if (u & UpdateColor)
    glEnableClientState(GL_COLOR_ARRAY);
  //if (u & UpdateTexture)
  // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  int size1 = state.impl->numattribs;
  if (size1)
    {
      std::vector<Attrib>::const_iterator ii = attribs.begin();
      for(int i=0;i<size1;i++,ii++)
	{
	  glEnableVertexAttribArrayARB((*ii).loc);
	}
    }
  if (u & UpdateNormal)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[1]); // normal
      glNormalPointer(GL_FLOAT, 0, 0/*state.impl->normal*/);
    }

  //if (u & UpdateTexture)
  // {
  //glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[2]); // texture
  //glTexCoordPointer(4, GL_UNSIGNED_BYTE, 0, state.impl->texture);
  // }

  if (u & UpdateColor)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[3]); // color
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0 /*state.impl->color*/);
    }
  int size = state.impl->numattribs;
  if (size)
    {
      std::vector<Attrib>::const_iterator ii = attribs.begin();
      for(int i=0;i<size;i++,ii++)
	{
	  glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[4+state.impl->numobjects+i]); // color
	  glVertexAttribPointer((*ii).loc, 1, GL_FLOAT, 0, 0, 0);
	}
    }

  if (u & UpdateVertex)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[0]); // vertex
      glVertexPointer(3, GL_FLOAT, 0, 0 /*state.impl->vertex*/); // 0 if ARB
    }

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, state.impl->id[0]); // vertex
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, state.impl->id[4+obj]); // index

  glDrawRangeElements(GL_TRIANGLES, rangestart, rangeend, (rangeendi-rangestarti)*3, GL_UNSIGNED_SHORT, 0 /*state.impl->index*/);

  if (u & UpdateVertex)
    glDisableClientState(GL_VERTEX_ARRAY);

  if (u & UpdateNormal)
    glDisableClientState(GL_NORMAL_ARRAY);
  if (u & UpdateColor)
    glDisableClientState(GL_COLOR_ARRAY);
  //if (u & UpdateTexture)
  // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  int size3 = state.impl->numattribs;
  if (size3)
    {
      std::vector<Attrib>::const_iterator ii = attribs.begin();
      for(int i=0;i<size3;i++,ii++)
	{
	  glDisableVertexAttribArrayARB((*ii).loc);
	}
    }


  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
#endif
}

void MatrixVBOCmd::cmd()
{
  glPushMatrix();
#ifndef EMSCRIPTEN
  glMultMatrixf(&m.matrix[0]);
#endif
  c.cmd();    
  glPopMatrix();
}

SurfaceEffect::SurfaceEffect(Render *r)
  : FrameAnim(r)
{
  shader.push_back(&water);
  shader.push_back(&phong);
  shader.config();
  shader.use();
}
SurfaceEffect::~SurfaceEffect()
{
  shader.unuse();
}
void SurfaceEffect::Init()
{
  CubeElem cube;
  cube.SetBox(Matrix::Identity());
  
  MatrixElem m(cube, Matrix::Scale(200,200,200));

  SplitQuads split(m, 30,30);
  QuadsToTris q(split); // faces=30*30*2, points=30*30*4
  FaceCollectionTri *array[] = { &q };
  ObjectArray oa(array, array+1);
  VBO vbo(oa);

  WaterAttributes water_attrib(AttrInternalX, AttrInternalY,
			       AttrInternalNX_X,
			       AttrInternalNX_Y,
			       AttrInternalNX_Z,
			       AttrInternalNY_X,
			       AttrInternalNY_Y,
			       AttrInternalNY_Z
			       );
  shader.set_attribs(water_attrib);
  std::vector<Attrib> attribs;
  water_attrib.Attr(attribs);

  UpdateVBO(vbo, 0, vbostate, UpdateVertexNormalIndex, attribs);
}

bool SurfaceEffect::Frame(float time)
{
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  WaterParameters params(time/20.0);
  shader.set_params(params);

  // Shading
  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,255,255);
  Color spec_color(60,60,192);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);

  ConstantObjectCollection obj(1,0);
  SingletonArray<Matrix> box1(Matrix::Identity());
  BoxCollectionConvert box(box1);
  CombineBoxesAndObjects coll(box, obj);
  RenderOpenGlObjects(coll, vbostate, UpdateVertexNormalIndex);

  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  return false;
}
void RingEffect::Init()
{
  std::pair<Point, Vector> array[] = 
    { 
      std::make_pair(Point(-30.0,-10.0,0.0), Vector(-1.0,-1.0,0.0)),
      std::make_pair(Point(30.0,-10.0,0.0), Vector(1.0, -1.0,0.0)),
      std::make_pair(Point(30.0,10.0,0.0), Vector(1.0, 1.0,0.0)),
      std::make_pair(Point(-30.0,10.0,0.0), Vector(-1.0, 1.0,0.0))
    };
  VectorArray<std::pair<Point,Vector> > pvarray(array, array+4);
  PointVectorCollectionConvert pointvector(pvarray);
  RingElem ring(pointvector, 100.0, 50);
  RingElem ring1(pointvector, 150.0, 50);
  RingElem ring2(pointvector, 200.0, 50);
  RingElem ring3(pointvector, 250.0, 50);


  QuadsToTris q(ring); // faces=30*30*2, points=30*30*4
  QuadsToTris q1(ring1); // faces=30*30*2, points=30*30*4
  QuadsToTris q2(ring2); // faces=30*30*2, points=30*30*4
  QuadsToTris q3(ring3); // faces=30*30*2, points=30*30*4
  FaceCollectionTri *arr[] = { &q, &q1, &q2, &q3 };
  ObjectArray oa(arr, arr+4);
  VBO vbo(oa);
  UpdateVBO(vbo, 0, vbostate, UpdateVertexNormalIndex);
  UpdateVBO(vbo, 1, vbostate, UpdateVertexNormalIndex);
  UpdateVBO(vbo, 2, vbostate, UpdateVertexNormalIndex);
  UpdateVBO(vbo, 3, vbostate, UpdateVertexNormalIndex);

}
bool RingEffect::Frame(float time)
{

  Matrix m(Matrix::XRotation(time/100.0));
  Matrix m1(Matrix::ZRotation(time/75.0));
  Matrix m2(Matrix::XRotation(time/50.0));
  Matrix m3(Matrix::ZRotation(time/25.0));

  Matrix array[] = { m, m1, m2, m3 };
  
  VectorArray<Matrix> varray(array, array+4);
  BoxCollectionConvert box(varray);
  LinearObjectCollection obj(box.Size());
  CombineBoxesAndObjects coll(box, obj);
  RenderOpenGlObjects(coll, vbostate, UpdateVertexNormalIndex);

  //RenderOpenGl(m);
  //RenderOpenGl(m1);
  //RenderOpenGl(m2);
  //RenderOpenGl(m3);
  return false;
}
bool MovementEffect::Frame(float time)
{
#if 0
  //glDisable(GL_LIGHT0);
  SphereElem b(10.0,10.0);
  b.SetBox(Matrix::Identity());

  //CircleMatrixCurve2 c3d1(150.0,0.0);
  //MatrixCurveToCurveIn3d c3d(c3d1, Vector(0.0,1.0,0.0));
  Circle c2d;
  ScaleCurveIn2d sc2d(c2d, time/200.0, time/200.0);
  //CircleMatrixCurve matrixcurve(100.0);
  //CurveMultiply mult(matrixcurve, c3d);

  CircleMatrixCurve2 cmc(150.0, 0.0);
  //Circle circle;
  AnimCircle circle(time/35.0, 4.0);
  Plane plane(Point(0.0,0.0,0.0), Vector(50.0,0.0,0.0), Vector(0.0,50.0,0.0));
  PlaneCurveIn3d circle_plane(circle, plane);
  CurveMultiply cmult(cmc, circle_plane);


  CurveInSurface curve(cmult, sc2d);
  CurveMovement move(curve);

  //SampleSurfaceIn3d s1(cmult, 0, 20,20);
  //RenderOpenGl(s1);

  SampleCurve3d sample(curve, 600);
  ContinuousLines lines(sample, true);
  RenderOpenGlLines(lines);

  MatrixElem mat(b, Matrix::Scale(20.0,20.0,20.0));

  for(int i=0;i<300;i++)
    {
      MatrixElem mat2(mat, move.Index(time/20.0 + i/30.0));
      RenderOpenGl(mat2);
    }
#endif

  Packing p(3, 1.0); // int -> Point2d
  CircleFunction cf(0.5); // Point2d -> CurveIn2d*
  PlaneCurveIn3dFunction pc(Plane(Point(0.0,0.0,0.0), Vector(100.0,0.0,0.0), Vector(0.0,100.0,0.0))); // CurveIn2d* => CurveIn3d*
  SampleCurve3dFunction sc(30); // CurveIn3d* -> PointCollection*
  ContinuousLinesFunction clf(true); // PointCollection * -> LineCollection*

  ComposeArrays<int, Point2d, CurveIn2d*> comp(p,cf); // int -> CurveIn2d*
  ComposeArrays<int, CurveIn2d*, CurveIn3d*> comp2(comp, pc); // int -> CurveIn3d*
  ComposeArrays<int, CurveIn3d*, PointCollection*> comp3(comp2, sc); // int -> PointCollection*
  ComposeArrays<int, PointCollection*, LineCollection*> comp4(comp3, clf); // int -> LineCollection*
  // FIX RenderOpenGl(comp4);
  

  return false;
}
void SinusEffect::Init()
{
  //SphereElem sphere(10,10);
  CubeElem sphere;
  sphere.SetBox(Matrix::Identity());
  MatrixElem sphere2(sphere, Matrix::Scale(20.0,20.0,20.0));
  QuadsToTris q(sphere2);
  FaceCollectionTri *arr[] = { &q };
  ObjectArray oa(arr, arr+1);
  VBO vbo(oa);
  UpdateVBO(vbo, 0, vbostate, UpdateVertexNormalIndex);
}

bool SinusEffect::Frame(float time)
{
  SinWaveform s(3.14159*2.0, 1.0); // float -> float
  SampleWaveform sample(s, 10); // int -> float
  Point2d p;
  p.x=0;
  p.y=0;
  Vector2d v1;
  v1.dx = 10.0;
  v1.dy = 0.0;
  Vector2d v2;
  v2.dx = 0.0;
  v2.dy = -1.0;
  FloatArrayToPointCollection2d points(sample, p, v1, v2); // int -> Point2d
  PlaneIn3dFunction pl(Plane(Point(0.0,0.0,0.0), Vector(100.0,0.0,0.0), Vector(0.0,100.0,0.0))); // Point2d -> Element<Point>
  ElementFunction<Point> elem; // Element<Point> -> Point
  TranslateElementFunction trans;
  ElementFunction<Matrix> elem2;

  ComposeChain<int, Matrix> chain(points, pl, elem, trans, elem2);
  BoxCollectionConvert box(chain);

  CubeElem cube;
  //SphereElem cube(20,10);
  cube.SetBox(Matrix::Identity());
  MatrixElem cube2(cube, Matrix::Scale(20.0,20.0,20.0));
  BoxedElem boxed(box, cube2);

  //FaceCenterPoints facepoints(boxed); // int -> Point
  //FaceNormals normals(boxed);     // int -> Vector
  //ArrayPair<int, Point, Vector> combined(facepoints, normals); // int ->(Point,Vector)
  SawWaveform saw;
  FitWaveform fit(saw, 20, 0.0, 1.0);
  RepeatWaveform rep(fit, 50);

  FacesToBoxCollection boxes(boxed, rep.Index(time));

  ConstantObjectCollection obj(boxes.Size(),0);
  CombineBoxesAndObjects coll(boxes, obj);
  //RenderOpenGlObjects(coll, vbostate, UpdateVertexNormalIndex);

  
  //RenderOpenGl(boxed2);


  LineIn3d l1(Point(0.0,0.0,0.0), Point(0.0,100.0,0.0));
  LineIn3d l2(Point(200.0,20.0,20.0), Point(50.0,500.0,50.0));
  ConnectCurves conn(l1, l2);
  SampleCurve3d sampl(conn, 50);
  ContinuousLines cont(sampl, false);
  //RenderOpenGlLines(cont);


  IncreasingWaveform w;
  SampleWaveform sw(w, 10);
  Point arr[] = { Point(0.0,0.0,0.0),
		 Point(100.0,0.0,0.0),
		 Point(0.0,100.0,0.0),
		 Point(0.0,0.0,100.0),
		 Point(0.0,50.0,0.0),
		 Point(50.0,0.0,0.0),
		 Point(50.0,50.0,0.0),
		 Point(0.0,100.0,100.0),
		 Point(0.0,50.0,0.0),
		 Point(50.0,0.0,50.0) };
  VectorArray<Point> va(arr, arr+10);
  PointCollectionConvert cva(va);
  SplineCurve curve(cva, sw);
  SampleCurve3d sampl2(curve, 100);

  ContinuousLines cont2(sampl2, false);
  RenderOpenGlLines(cont2);

  return false;
}

bool WalkCubeEffect::Frame(float time)
{
  CubeElem ce;
  ce.SetBox(Matrix::Identity());
  MatrixElem me(ce, Matrix::Scale(100.0,100.0,100.0));
  
  MatrixCurveSequencer seq;
  float start_time;
  start_time = 0.0;
  RotateAroundEdge rot1(seq.Index(start_time), me, 0, 0, 1, 0.1, 100.0);
  seq.push_back(start_time, 100.0, &rot1);

  start_time = 100.0;
  RotateAroundEdge rot2(seq.Index(start_time), me, 1, 0, 1, 0.1, 100.0);
  seq.push_back(start_time, 100.0, &rot2);

  start_time = 200.0;
  RotateAroundEdge rot3(seq.Index(start_time), me, 2, 0, 1, 0.1, 100.0);
  seq.push_back(start_time, 100.0, &rot3);

  MatrixElem m2(me, seq.Index(time));
  RenderOpenGl(m2);
  return false;
}
void PrecalcCmds(OpenGlCommands &cmds) { cmds.PreCalc(); }
void RenderCmds(OpenGlCommands &cmds) { cmds.Execute(); }
void PrecalcRenderCmds(OpenGlCommands &cmds) { cmds.PreCalc(); cmds.Execute(); }
ScopeMatrix::ScopeMatrix(Matrix m)
{
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  glPushMatrix();
  //glIdentityMatrix();
#ifndef EMSCRIPTEN
  glMultMatrixf(&mat[0]);
#endif
}
ScopeMatrix::~ScopeMatrix()
{
  glPopMatrix();
}

void SDLRender::SetTime(float time) { }
void SDLRender::SetMatrix(Matrix m) { }
void SDLRender::RenderFaces(const FaceCollection &col) { RenderOpenGl(col); }
void SDLRender::RenderFaces(const FaceCollection &col, const TextureCoords &tex) { RenderOpenGl(col, tex); }

void SDLRender::RenderLines(const LineCollection &col) { RenderOpenGlLines(col); }
void SDLRender::RenderPoly(const PointCollection &col) { RenderOpenGlPoly(col); }
void* SDLRender::CreateState() { return new VBOState; }
void SDLRender::UpdateVBO(const FaceCollection &faces, void *state)
{
  ::UpdateVBO(faces, *(VBOState*)state, UpdateAll);
}
void SDLRender::DrawVBO(void *state)
{
  ::DrawVBO(*(VBOState*)state);
}
void SDLRender::DestroyState(void *state)
{
  delete (VBOState*)state;
}


int ArrayRender::NumVertices(FaceCollection &coll)
{
  return coll.NumFaces()*coll.NumPoints(0);
}
int ArrayRender::NumFaces(FaceCollection &coll)
{
  return coll.NumFaces();
}
std::pair<int,int> ArrayRender::Insert(FaceCollection &coll)
{
  if (used_vertex_count[0]==0)
    {
      quads = coll.NumPoints(0)==4;
    }
  int old_used_vertex_count = used_vertex_count[0];
  int old_face_count = used_face_count[0];
  int offset3 = used_vertex_count[0]*3;
  int offset4 = used_vertex_count[0]*4;
  int offset2 = used_vertex_count[0]*2;
  int size_faces = coll.NumFaces();
  int size_points = coll.NumPoints(0);
  int i = 0;
  for(int f=0;f<size_faces;f++)
    {
    index_array[used_face_count[0] + f] = used_vertex_count[0] + i;
    for(int p=0;p<size_points;p++)
      {
	Point pp = coll.FacePoint(f,p);
	//std::cout << "f: " << f << " p: " << p << " " << pp << std::endl;
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	Vector n = coll.PointNormal(f,p);
	normal_array[offset3 + i*3 + 0] = n.dx;
	normal_array[offset3 + i*3 + 1] = n.dy;
	normal_array[offset3 + i*3 + 2] = n.dz;
	unsigned int color = coll.Color(f,p);
	unsigned char *c = (unsigned char*)&color;
	color_array[offset4 + i*4] = c[0];
	color_array[offset4 + i*4 + 1] = c[1];
	color_array[offset4 + i*4 + 2] = c[2];
	color_array[offset4 + i*4 + 3] = c[3];
	Point2d texcoord = coll.TexCoord(f,p);
	tex_coord_array[offset2 + i*2 + 0] = texcoord.x;
	tex_coord_array[offset2 + i*2 + 1] = texcoord.y;

	i++;
      }
    }
  used_face_count += size_faces;
  used_vertex_count[0] += i;
  return std::make_pair(old_used_vertex_count, old_face_count);
}
void ArrayRender::UpdatePos(FaceCollection &coll, std::pair<int,int> p)
{
  int vertex_count = p.first;
  int face_count = p.second;
  int offset3 = vertex_count*3;
  int size_faces = coll.NumFaces();
  int size_points = coll.NumPoints(0);
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
    for(int p=0;p<size_points;p++)
      {
	Point pp = coll.FacePoint(f,p);
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	i++;
      }
    }
}
std::pair<int,int> ArrayRender::InsertMesh(Mesh &mesh, int frame)
{
  //std::cout << "InsertMesh start" << std::endl;
  if (used_vertex_count[frame]==0)
    {
      quads = mesh.NumPoints()==4;
    }
  else
    {
      if ((mesh.NumPoints() == 4)!=quads) std::cout << "ArrayRender::InsertMesh ERROR!" << std::endl;
    }
  int old_used_vertex_count = used_vertex_count[frame];
  int old_face_count = used_face_count[frame];
  int offset3 = used_vertex_count[frame]*3 + vertex_array_size*3*frame;
  //int offset4 = used_vertex_count[frame]*4 + vertex_array_size*4*frame;
  //int offset2 = used_vertex_count[frame]*2 + vertex_array_size*2*frame;;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = 0;
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "M: " << f << "/" << size_faces << std::endl;
	}

    index_array[used_face_count[frame] + f] = used_vertex_count[frame] + i;
    for(int p=0;p<size_points;p++)
      {
	Point pp = mesh.FacePoint(frame, 0, f,p);
	//std::cout << "f: " << f << " p: " << p << " " << pp << std::endl;
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	i++;
      }
    }
  used_face_count[frame] += size_faces;
  used_vertex_count[frame] += i;
  //std::cout << "InsertMesh end" << std::endl;
  return std::make_pair(old_used_vertex_count, old_face_count);
}

void ArrayRender::UpdateTexCoord(Mesh &mesh, MeshTexCoords &coord, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  //int offset3 = vertex_count*3 + vertex_array_size*3*frame;
  int offset2 = vertex_count*2 + vertex_array_size*2*frame;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "TX: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	Point2d texcoord = coord.TexCoord(frame, 0, f,p);
	//std::cout << "f: " << f << " p: " << p << " " << texcoord.x << "," << texcoord.y << std::endl;
	tex_coord_array[offset2 + i*2 + 0] = texcoord.x;
	tex_coord_array[offset2 + i*2 + 1] = texcoord.y;
	i++;
      }
    }
}
void ArrayRender::AllocTexture(int count)
{
  int texcount = count;
  texture = new int[texcount];
  glGenTextures(texcount, (GLuint*)&texture[0]);
#if 0
  GLenum e = glGetError();
  if (e!=GL_NO_ERROR)
    {
      std::cout << "ArrayRender::AllocTexture error!" << e << std::endl;
    }
#endif
  texture_count = texcount;
  textures = new BufferRef*[count];
}
void ArrayRender::UpdateAllTextures(MeshTextures &tex)
{
  int count = tex.NumTextures();
  for(int i=0;i<count;i++)
    {
      UpdateTexture(tex, i);
    }
}
void ArrayRender::UpdateTexture(MeshTextures &tex, int num)
{
  //std::cout << "UpdateTexture " << num << std::endl;
  glActiveTexture(GL_TEXTURE0+num);
#if 0
  GLenum e = glGetError();
  if (e!=GL_NO_ERROR)
    {
      std::cout << "ArrayRender::UpdateTexture1 error!" << e << std::endl;
    }
#endif
  tex.GenTexture(num);
  BufferRef ref = tex.TextureBuf(num);
  BitmapFromBuffer buf(ref);
  FlipColours flip(buf);
  BufferFromBitmap buf2(flip);
  buf2.Gen(); 
  BufferRef ref2 = buf2.Buffer(); 
  int sizex = ref2.width;
  int sizey = ref2.height;
  glBindTexture(GL_TEXTURE_2D, texture[num]);
  //for(int i=0;i<sizex;i++)
  //  {
  //    std::cout << std::hex << ref2.buffer[i+5*ref2.ydelta] << std::dec << std::endl;
  //  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref2.buffer);
#if 0
  GLenum e2 = glGetError();
  if (e2!=GL_NO_ERROR)
    {
      std::cout << "ArrayRender::UpdateTexture2 error!" << e2 << std::endl;
    }
#endif
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}
void ArrayRender::EnableTexture(int num)
{
  //std::cout << "EnableTexture " << num << std::endl;
  glActiveTexture(GL_TEXTURE0+num);
  glBindTexture(GL_TEXTURE_2D, texture[num]);
  glEnable(GL_TEXTURE_2D);
}
void ArrayRender::DisableTexture()
{
  glDisable(GL_TEXTURE_2D);
}
void ArrayRender::UpdateColors(Mesh &mesh, MeshColors &color, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  //int offset3 = vertex_count*3 + vertex_array_size*3*frame;
  //int offset2 = vertex_count*2 + vertex_array_size*2*frame;
  int offset4 = vertex_count*4 + vertex_array_size*4*frame;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "C: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	unsigned int col = color.VertexColor(frame, 0, f,p);
	//std::cout << "UpdateColors: " << std::hex << col << std::endl;
	unsigned char *c = (unsigned char*)&col;
	color_array[offset4 + i*4] = c[0];
	color_array[offset4 + i*4 + 1] = c[1];
	color_array[offset4 + i*4 + 2] = c[2];
	color_array[offset4 + i*4 + 3] = c[3];
	i++;
      }
    }
}

std::vector<std::pair<int,int> > ArrayRender::InsertAll(Mesh &mesh, MeshNormals &normal, MeshTexCoords &coord, MeshColors &color)
{
  std::vector<std::pair<int,int> > vec;
  int meshsize = mesh.NumFrames();
  for(int i=0;i<meshsize;i++)
    {
      std::pair<int,int> p = InsertMesh(mesh,i);
      //std::cout << "Mesh: " << p.first << " " << p.second << std::endl;
      vec.push_back(p);
    }
  int normalsize = normal.NumFrames();
  for(int ii=0;ii<meshsize;ii++)
    {
      for(int k=0;k<normalsize;k++)
	{
	  UpdateNormals(mesh, normal, vec[ii], k);
	}
    }

  int texcoordsize = coord.NumFrames();
  for(int ie=0;ie<meshsize;ie++)
    {
      for(int k=0;k<texcoordsize;k++)
	{
	  UpdateTexCoord(mesh, coord, vec[ie], k);
	}
    }

  int colorsize = color.NumFrames();
  for(int id=0;id<meshsize;id++)
    {
      for(int k=0;k<colorsize;k++)
	{
	  UpdateColors(mesh, color, vec[id], k);
	}
    }
  return vec;
}

void ArrayRender::UpdateNormals(Mesh &mesh, MeshNormals &normal, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  int offset3 = vertex_count*3;
  //int offset2 = vertex_count*2;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "N: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	Vector norm = normal.PointNormal(frame, 0, f,p);
	normal_array[offset3 + i*3 + 0] = norm.dx;
	normal_array[offset3 + i*3 + 1] = norm.dy;
	normal_array[offset3 + i*3 + 2] = norm.dz;
	i++;
      }
    }
}



void ArrayRender::Alloc(int numfaces, int numvertices)
{
  std::cout << "ArrayRender::Alloc " << numfaces << " " << numvertices << std::endl;
  vertex_array = new float[3*numvertices];
  vertex_array_size = numvertices;
  used_vertex_count = new int[1];
  used_face_count = new int[1];
  used_vertex_count[0] = 0;
  used_face_count[0] = 0;
  normal_array = new float[3*numvertices];
  color_array = new unsigned char[4*numvertices];
  tex_coord_array = new float[2*numvertices];

  index_array = new int[numfaces];


  q_vertex_array=0;
  q_normal_array=0;
  q_color_array=0;
  q_tex_coord_array=0;

}
ArrayRender::~ArrayRender()
{
  if (texture_count)
    {
      glDeleteTextures(texture_count, (const GLuint*)&texture[0]);
    }
  delete [] q_vertex_array;
  delete [] q_normal_array;
  delete [] q_color_array;
  delete [] q_tex_coord_array;
  delete [] textures;
  delete [] vertex_array;
  delete [] normal_array;
  delete [] color_array;
  delete [] tex_coord_array;
  delete [] index_array;
  delete [] texture;
  delete [] used_vertex_count;
  delete [] used_face_count;
  
}
void ArrayRender::Alloc(int numfaces, int numvertices, int vertexframes, int normalframes, int colorframes, int texcoordframes)
{
  vertex_array = new float[3*numvertices*vertexframes];
  for(int i=0;i<3*numvertices*vertexframes;i++)
    {
      vertex_array[i]=0.0f;
    }
  vertex_array_size = numvertices;
  used_vertex_count = new int[vertexframes];
  used_face_count = new int[vertexframes];
  for(int i=0;i<vertexframes;i++)
    {
      used_vertex_count[i] = 0;
      used_face_count[i] = 0;
    }
  normal_array = new float[3*numvertices*normalframes*vertexframes];
  color_array = new unsigned char[4*numvertices*colorframes*vertexframes];
  tex_coord_array = new float[2*numvertices*texcoordframes*vertexframes];

  index_array = new int[numfaces*vertexframes];

  m_vertex_frames = vertexframes;
  m_normal_frames = normalframes;
  m_color_frames = colorframes;
  m_texcoord_frames = texcoordframes;
}

void ArrayRender::Prepare()
{
  if (quads)
    {
      q_num_vertices = vertex_array_size*6/4;
    }
  else
    {
      q_num_vertices = vertex_array_size;
    }
  q_vertex_array = new float[q_num_vertices*3];
  q_normal_array = new float[q_num_vertices*3];
  q_color_array = new unsigned char[q_num_vertices*4];
  q_tex_coord_array = new float[q_num_vertices*2];

  if (quads) {
    int count = 0;
    //std::cout << "Vertex Array Size2: " << vertex_array_size << std::endl;
    for(int i=0;i<vertex_array_size/(4*3);i++)
      {
	q_vertex_array[count+0*3+0] = vertex_array[i*4*3+0*3+0];
	q_vertex_array[count+0*3+1] = vertex_array[i*4*3+0*3+1];
	q_vertex_array[count+0*3+2] = vertex_array[i*4*3+0*3+2];
	q_vertex_array[count+1*3+0] = vertex_array[i*4*3+1*3+0];
	q_vertex_array[count+1*3+1] = vertex_array[i*4*3+1*3+1];
	q_vertex_array[count+1*3+2] = vertex_array[i*4*3+1*3+2];
	q_vertex_array[count+2*3+0] = vertex_array[i*4*3+2*3+0];
	q_vertex_array[count+2*3+1] = vertex_array[i*4*3+2*3+1];
	q_vertex_array[count+2*3+2] = vertex_array[i*4*3+2*3+2];
	q_vertex_array[count+3*3+0] = vertex_array[i*4*3+0*3+0];
	q_vertex_array[count+3*3+1] = vertex_array[i*4*3+0*3+1];
	q_vertex_array[count+3*3+2] = vertex_array[i*4*3+0*3+2];
	q_vertex_array[count+4*3+0] = vertex_array[i*4*3+2*3+0];
	q_vertex_array[count+4*3+1] = vertex_array[i*4*3+2*3+1];
	q_vertex_array[count+4*3+2] = vertex_array[i*4*3+2*3+2];
	q_vertex_array[count+5*3+0] = vertex_array[i*4*3+3*3+0];
	q_vertex_array[count+5*3+1] = vertex_array[i*4*3+3*3+1];
	q_vertex_array[count+5*3+2] = vertex_array[i*4*3+3*3+2];
	
	q_normal_array[count+0*3+0] = normal_array[i*4*3+0*3+0];
	q_normal_array[count+0*3+1] = normal_array[i*4*3+0*3+1];
	q_normal_array[count+0*3+2] = normal_array[i*4*3+0*3+2];
	q_normal_array[count+1*3+0] = normal_array[i*4*3+1*3+0];
	q_normal_array[count+1*3+1] = normal_array[i*4*3+1*3+1];
	q_normal_array[count+1*3+2] = normal_array[i*4*3+1*3+2];
	q_normal_array[count+2*3+0] = normal_array[i*4*3+2*3+0];
	q_normal_array[count+2*3+1] = normal_array[i*4*3+2*3+1];
	q_normal_array[count+2*3+2] = normal_array[i*4*3+2*3+2];
	q_normal_array[count+3*3+0] = normal_array[i*4*3+0*3+0];
	q_normal_array[count+3*3+1] = normal_array[i*4*3+0*3+1];
	q_normal_array[count+3*3+2] = normal_array[i*4*3+0*3+2];
	q_normal_array[count+4*3+0] = normal_array[i*4*3+2*3+0];
	q_normal_array[count+4*3+1] = normal_array[i*4*3+2*3+1];
	q_normal_array[count+4*3+2] = normal_array[i*4*3+2*3+2];
	q_normal_array[count+5*3+0] = normal_array[i*4*3+3*3+0];
	q_normal_array[count+5*3+1] = normal_array[i*4*3+3*3+1];
	q_normal_array[count+5*3+2] = normal_array[i*4*3+3*3+2];
	
	
	q_color_array[count+0*3+0] = color_array[i*4*3+0*3+0];
	q_color_array[count+0*3+1] = color_array[i*4*3+0*3+1];
	q_color_array[count+0*3+2] = color_array[i*4*3+0*3+2];
	q_color_array[count+1*3+0] = color_array[i*4*3+1*3+0];
	q_color_array[count+1*3+1] = color_array[i*4*3+1*3+1];
	q_color_array[count+1*3+2] = color_array[i*4*3+1*3+2];
	q_color_array[count+2*3+0] = color_array[i*4*3+2*3+0];
	q_color_array[count+2*3+1] = color_array[i*4*3+2*3+1];
	q_color_array[count+2*3+2] = color_array[i*4*3+2*3+2];
	q_color_array[count+3*3+0] = color_array[i*4*3+0*3+0];
	q_color_array[count+3*3+1] = color_array[i*4*3+0*3+1];
	q_color_array[count+3*3+2] = color_array[i*4*3+0*3+2];
	q_color_array[count+4*3+0] = color_array[i*4*3+2*3+0];
	q_color_array[count+4*3+1] = color_array[i*4*3+2*3+1];
	q_color_array[count+4*3+2] = color_array[i*4*3+2*3+2];
	q_color_array[count+5*3+0] = color_array[i*4*3+3*3+0];
	q_color_array[count+5*3+1] = color_array[i*4*3+3*3+1];
	q_color_array[count+5*3+2] = color_array[i*4*3+3*3+2];
	

	q_tex_coord_array[count+0*3+0] = tex_coord_array[i*4*3+0*3+0];
	q_tex_coord_array[count+0*3+1] = tex_coord_array[i*4*3+0*3+1];
	q_tex_coord_array[count+0*3+2] = tex_coord_array[i*4*3+0*3+2];
	q_tex_coord_array[count+1*3+0] = tex_coord_array[i*4*3+1*3+0];
	q_tex_coord_array[count+1*3+1] = tex_coord_array[i*4*3+1*3+1];
	q_tex_coord_array[count+1*3+2] = tex_coord_array[i*4*3+1*3+2];
	q_tex_coord_array[count+2*3+0] = tex_coord_array[i*4*3+2*3+0];
	q_tex_coord_array[count+2*3+1] = tex_coord_array[i*4*3+2*3+1];
	q_tex_coord_array[count+2*3+2] = tex_coord_array[i*4*3+2*3+2];
	q_tex_coord_array[count+3*3+0] = tex_coord_array[i*4*3+0*3+0];
	q_tex_coord_array[count+3*3+1] = tex_coord_array[i*4*3+0*3+1];
	q_tex_coord_array[count+3*3+2] = tex_coord_array[i*4*3+0*3+2];
	q_tex_coord_array[count+4*3+0] = tex_coord_array[i*4*3+2*3+0];
	q_tex_coord_array[count+4*3+1] = tex_coord_array[i*4*3+2*3+1];
	q_tex_coord_array[count+4*3+2] = tex_coord_array[i*4*3+2*3+2];
	q_tex_coord_array[count+5*3+0] = tex_coord_array[i*4*3+3*3+0];
	q_tex_coord_array[count+5*3+1] = tex_coord_array[i*4*3+3*3+1];
	q_tex_coord_array[count+5*3+2] = tex_coord_array[i*4*3+3*3+2];
	
	count += 6*3;
      }
  } else
    {
      //int count = 0;
    //std::cout << "Vertex Array Size: " << vertex_array_size << std::endl;
    for(int i=0;i<vertex_array_size;i++) {
      q_vertex_array[i*3+0] = vertex_array[i*3+0];
      q_vertex_array[i*3+1] = vertex_array[i*3+1];
      q_vertex_array[i*3+2] = vertex_array[i*3+2];
      q_normal_array[i*3+0] = normal_array[i*3+0];
      q_normal_array[i*3+1] = normal_array[i*3+1];
      q_normal_array[i*3+2] = normal_array[i*3+2];
      q_color_array[i*4+0] = color_array[i*4+0];
      q_color_array[i*4+1] = color_array[i*4+1];
      q_color_array[i*4+2] = color_array[i*4+2];
      q_color_array[i*4+3] = color_array[i*4+3];
      q_tex_coord_array[i*2+0] = tex_coord_array[i*2+0];
      q_tex_coord_array[i*2+1] = tex_coord_array[i*2+1];
    }
    }

#ifdef VAO
  glGenVertexArrays(1,vao);
  glBindVertexArray(vao[0]);
#endif
  glGenBuffers(5, &buffer[0]);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_vertex_array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
  glBufferData(GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_normal_array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  glBufferData(GL_ARRAY_BUFFER, q_num_vertices*sizeof(unsigned char)*4, q_color_array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glBufferData(GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*2, q_tex_coord_array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
  glBufferData(GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_vertex_array, GL_STATIC_DRAW);

#ifdef VAO
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
  glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  glVertexAttribPointer(2,4, GL_UNSIGNED_BYTE, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
  glVertexAttribPointer(4,3, GL_FLOAT, GL_FALSE, 0,0);
#endif
  
}
void ArrayRender::Render(int vertexframe, int normalframe, int colorframe, int texcoordframe, int vertex_pos, int vertex_size)
{
#ifdef VAO
  glBindVertexArray(vao[0]);
#else
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
  glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  glVertexAttribPointer(2,4, GL_UNSIGNED_BYTE, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
  glVertexAttribPointer(4,3, GL_FLOAT, GL_FALSE, 0,0);
#endif
  glDrawArrays(GL_TRIANGLES, 0, vertex_size*6/4);
#ifndef VAO
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
#endif

#if 0
  // enabling
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertex_array+vertex_pos*3+vertexframe*vertex_array_size*3);

  if (normalframe != -1)
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, normal_array+vertex_pos*3+normalframe*vertex_array_size*3);
    }
  if (colorframe != -1)
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, color_array+vertex_pos*4+colorframe*vertex_array_size*4);
    }
  if (texcoordframe != -1)
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, tex_coord_array+vertex_pos*2+texcoordframe*vertex_array_size*2);
    }

  // Render
  if (quads)
    {
      glDrawArrays(GL_QUADS, 0, vertex_size);
    }
  else
    {
      glDrawArrays(GL_TRIANGLES, 0, vertex_size);
    }

  // disabling
  if (texcoordframe != -1)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if (colorframe != -1)
    {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  if (normalframe != -1)
    {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  glDisableClientState(GL_VERTEX_ARRAY);
#endif
}


void ArrayRender::Render(bool normal, bool color, bool texcoord, int vertex_pos, int vertex_size)
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
  glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  glVertexAttribPointer(2,4, GL_UNSIGNED_BYTE, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, 0,0);
  glDrawArrays(GL_TRIANGLES, 0, q_num_vertices);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

#if 0
  // enabling
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertex_array+vertex_pos*3);

  if (normal)
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, normal_array+vertex_pos*3);
    }
  if (color)
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, color_array+vertex_pos*4);
    }
  if (texcoord)
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, tex_coord_array+vertex_pos*2);
    }

  // Render
  if (quads)
    {
      glDrawArrays(GL_QUADS, 0, vertex_size);
    }
  else
    {
      glDrawArrays(GL_TRIANGLES, 0, vertex_size);
    }

  // disabling
  if (texcoord)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if (color)
    {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  if (normal)
    {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  glDisableClientState(GL_VERTEX_ARRAY);
#endif
}
void HeightMapEffect(FaceStore &s, float time, float repeat_length);

void ArrayEffect::Init()
{
  //CubeElem c;
  PreCalcFaceStore(&HeightMapEffect, 2.0*3.14159, 50, rend, s);
}

bool ArrayEffect::Frame(float time)
{
  //Water w(time/30.0);
  //HeightMap h(100,100,w);
  //MatrixElem m(h, Matrix::Scale(400,400,400)*Matrix::Translate(-200,-200,0));
  //rend.UpdatePos(m, p);
  ExecuteFaceStore(rend, s, time);
  return false;
}

class DeltaFloatBitmap : public Bitmap<float>
{
public:
  DeltaFloatBitmap(int sx, int sy, float time, float dx, float dy) : sx(sx), sy(sy), time(time), dx(dx), dy(dy) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    return time + x*dx + y*dy;
  }
private:
  int sx,sy;
  float time;
  float dx,dy;
};

class DeltaPosBitmap : public Bitmap<Point>
{
public:
  DeltaPosBitmap(int sx, int sy, Point p, Vector dx, Vector dy) : sx(sx), sy(sy), p(p), dx(dx), dy(dy) { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Point Map(int x, int y) const { return p + x*dx + y*dy; }
private:
  int sx,sy;
  Point p;
  Vector dx,dy;
};


class CircleHeightMap : public Bitmap<Point>
{ // both bitmaps need to be same size
public:
  CircleHeightMap(CurveIn3d &curve, Bitmap<float> &bm, Bitmap<Point> &pos, float time) : curve(curve), bm(bm), pos(pos), time(time) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  Point Map(int x, int y) const
  {
    float p = bm.Map(x,y);
    Point pp = pos.Map(x,y);
    Point p2 = curve.Index(p+time);
    return pp+p2;
  }
  
private:
  CurveIn3d &curve;
  Bitmap<float> &bm;
  Bitmap<Point> &pos;
  float time;
};

class TrueBitmap : public Bitmap<bool>
{
public:
  TrueBitmap(int sx, int sy) : sx(sx), sy(sy) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  bool Map(int x, int y) const { return true; }
private:
  int sx,sy;
};

class HeightMapPolygons : public BoxableFaceCollection
{
public:
  HeightMapPolygons(Bitmap<Point> &pos) : tr(pos.SizeX(),pos.SizeY()), faces(tr, pos) { }
  virtual int NumFaces() const { return faces.NumFaces(); }
  virtual int NumPoints(int face) const { return faces.NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return faces.FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { return faces.PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return faces.Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return faces.AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return faces.Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return faces.TexCoord(face,point); }
  
private:
  TrueBitmap tr;
  FacesFromMap faces;
};

float Mult(float x) { return 0.03*x; }

class SumBitmap : public Bitmap<float>
{
public:
  SumBitmap(Bitmap<float> &a1, Bitmap<float> &a2) : a1(a1), a2(a2) { }
  int SizeX() const { return a1.SizeX(); }
  int SizeY() const { return a1.SizeY(); }
  float Map(int x, int y) const { return a1.Map(x,y)+a2.Map(x,y); }
private:
  Bitmap<float> &a1;
  Bitmap<float> &a2;
};

void HeightMapEffect(FaceStore &s, float time, float repeat_length)
{
  DeltaFloatBitmap dfloat(100,100,0.0,2*3.14159/100,2*3.14159/50);
  DeltaPosBitmap dpos(100,100, Point(-500,0.0,-500), Vector(10.0,0.0,0.0), Vector(0.0,0.0,10.0));
  DistanceField field(Point(0.0,0.0,0.0), 0.0);
  FieldBitmap dfloat2(field, dpos);
  NativeFunction<float,float> func(&Mult);
  MapBitmap<float, float> dmap(dfloat2, func);
  SumBitmap dsum(dfloat, dmap);
  Point2d p2d = { 0.0, 0.0 };
  Circle c(p2d, 15.0);
  Plane pl(Point(0.0,0.0,0.0), Vector(2.0,0.0,0.0), Vector(0.0,2.0,0.0));
  PlaneCurveIn3d cpl(c, pl);
  CircleHeightMap hm(cpl, dsum, dpos, time);
  HeightMapPolygons polys(hm);
  s.Add(polys);
}
void InterpolateFaceCollection(void (*fptr)(FaceStore &s, float time, float repeat_length), float value)
{
  FaceCollectionStore store;
  fptr(store, value, 1.0);
}
void PreCalcMesh(Mesh &mesh, ArrayRender &r, PrecalcStore &prestore)
{
  int count = 0;
  int frames = mesh.NumFrames();
  for(int i=0;i<frames;i++)
    {
      count += mesh.NumFaces(i);
    }
  if (count!=0)
    {
      //std::cout << "Precalcmesh: Count: " << count << " " << mesh.NumPoints() << std::endl;
      r.Alloc(count, count*mesh.NumPoints());
  
      bool firsttime = true;
      int used_count = r.used_vertex_count[0];
      for(int k=0;k<frames;k++)
	{
	  //std::cout << "Frame: " << k << "/" << frames << std::endl;
	  MeshFaceCollection coll(mesh, k);
	  prestore.vec.push_back(r.Insert(coll));
	  if (firsttime)
	    {
	      prestore.vertex_size = r.used_vertex_count[k]-used_count;
	      firsttime = false;
	    }

	}
    }
}

void PreCalcFaceStore(void (*fptr)(FaceStore &s, float time, float repeat_length), float repeat_duration, int repeat_framecount, ArrayRender &r, PrecalcStore &prestore)
{
  prestore.framecount = repeat_framecount;
  float time = 0.0;
  float timestep = repeat_duration/repeat_framecount;

  FaceCollectionStore store2;
  fptr(store2, 0.0, repeat_duration);

  int faces = r.NumFaces(store2);
  int vert = r.NumVertices(store2);
  r.Alloc(faces*repeat_framecount,vert*repeat_framecount);

  // Precalc
  bool firsttime = true;
  int used_count = r.used_vertex_count[0];
  for(int i=0;i<repeat_framecount;i++,time+=timestep)
    {
      //std::cout << time << "/" << repeat_duration << std::endl;
      FaceCollectionStore store;
      fptr(store, time, repeat_duration);
      prestore.vec.push_back(r.Insert(store));
      if (firsttime)
	{
	  prestore.vertex_size = r.used_vertex_count[0]-used_count;
	  firsttime = false;
	}
    }
    
}
void ExecuteFaceStore(ArrayRender &r, PrecalcStore &prestore, int frame)
{
  int frame2 = frame % prestore.framecount;
  r.Render(true,false,false, frame2*prestore.vertex_size, prestore.vertex_size);
}

struct SDLArrayRenderImpl
{
};

void SDLArrayRender::SetTime(float time) 
{ 
  dyn.push_frame();
}
void SDLArrayRender::SetMatrix(Matrix m) 
{ 
}
void SDLArrayRender::RenderFaces(const FaceCollection &col) 
{ 
  dyn.push_back(col);
}
void SDLArrayRender::RenderFaces(const FaceCollection &col, const TextureCoords &tex)
{
  dyn.push_back(col);
  // TODO textures
}
void SDLArrayRender::RenderFacesTexture(const FaceCollection &col)
{
  dyn.push_back(col);
}

void SDLArrayRender::RenderLines(const LineCollection &col) 
{ 
}
void SDLArrayRender::RenderPoly(const PointCollection &col) 
{ 
}
void *SDLArrayRender::CreateState()
{
  return new SDLArrayRenderImpl;
}
void SDLArrayRender::UpdateVBO(const FaceCollection &faces, void *state)
{
  //std::cout << "UpdateVBO" << std::endl;
  store.Add(faces);
  //dyn.push_back(faces);
}
void SDLArrayRender::DrawVBO(void *state)
{
  dyn.push_back(store);
}
void SDLArrayRender::DestroyState(void *state)
{
  delete (SDLArrayRenderImpl*)state;
}
void SDLArrayRender::Show()
{
  PreCalcMesh(dyn, rend, s);
  
  SDL_Event event;
  //glRotatef(speed*x, f.XRot(),f.YRot(),f.ZRot());
  glColor3f(1.0,1.0,1.0);
  SDL_ShowCursor(true);

  for(int i=0;;i++)
    {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
#ifndef EMSCRIPTEN
      glTranslatef(0.0, 0.0, -500.0);
#endif
      glRotatef(2.0*3.14159/360.0*i, 0.0,1.0,0.0);
#ifndef EMSCRIPTEN
      glTranslatef(0.0, -100.0, 0.0);
#endif
      ExecuteFaceStore(rend, s, i%dyn.NumFrames());
#ifndef EMSCRIPTEN
      glLoadIdentity();
#endif
      //SDL_GL_SwapBuffers();
    SDL_PollEvent(&event);
    if (event.type==SDL_QUIT || (event.type==SDL_KEYDOWN && event.key.keysym.sym==27)) { SDL_Quit(); exit(0); }
    }

}

void FrameAnim::RenderOpenGl(const FaceCollection &col) { render->RenderFaces(col); }
void FrameAnim::RenderOpenGlLines(const LineCollection &col) { render->RenderLines(col); }
void FrameAnim::RenderOpenGlPoly(const PointCollection &col) { render->RenderPoly(col); }

void TextureGeneration(FaceCollection &coll, FrameAnimPlugins &plugins)
{
  int s = coll.NumTextures();
  for(int i=0;i<s;i++)
    {
      //std::cout << "TextureGeneration: " << i << std::endl;
      coll.GenTexture(i);
      BufferRef ref = coll.TextureBuf(i);
      TexturePlugin *plugin = new TexturePlugin(ref);
      //plugin->texture = i;
      plugins.plugins.push_back(plugin);
    }
}

#if 0
struct VBOObjects::Priv
{
  GLuint vboId;
  bool allocated;
};


int DataFrames::NumBytes()
{
  int s = vec.size();
  int count = 0;
  for(int i=0;i<s;i++)
    {
      count = std::max(vec[i].size_in_bytes, count);
    }
  return count;
}
int DataFrames::Count()
{
  return vec.size();
}

static void FillRest( char *orig_ptr, int size_in_bytes, int size, int sizeof_elem )
{
      char *ptr = orig_ptr+size_in_bytes;
      char *ptr2 = ptr;
      ptr-=sizeof_elem;
      int count = (size)/sizeof_elem - (size_in_bytes)/sizeof_elem;
      int ii = 0;
      for(int i=0;i<count;i++)
	{
	  ptr2[i] = ptr[ii];
	  ii++;
	  if (ii >= sizeof_elem) ii-=sizeof_elem;
	}
}
DataBlock::~DataBlock()
{
  // orig_ptr is deleted with shared_ptr<void>
  delete [] adjusted_buffer;
}
void DataBlock::OrigToAdjusted(int size)
{
  adjusted_size = size;
  adjusted_buffer = new char[size];
  std::copy(orig_ptr, orig_ptr+size_in_bytes, adjusted_buffer);
  switch(type)
    {
    case 0: // vertex
      FillRest(orig_ptr, size_in_bytes, size, sizeof(VertexElem));
      break;
    case 1: // normal
      FillRest(orig_ptr, size_in_bytes, size, sizeof(NormalElem));
      break;
    case 2: // color
      FillRest(orig_ptr, size_in_bytes, size, sizeof(ColorElem));
      break;
    case 3: // texcoord
      FillRest(orig_ptr, size_in_bytes, size, sizeof(TexCoordElem));
      break;
    };
}

void DataTypes::OrigToAdjusted(bool has_normal, bool has_color, bool has_texcoord)
{
  int vertex_count = vertex.Count();
  for(int v=0;v<vertex_count;v++)
    vertex.OrigToAdjusted(v, vertex.NumBytes());
  if (has_normal)
    {
      int normal_count = normal.Count();
      for(int n=0;n<normal_count;n++)
	{
	  normal.OrigToAdjusted(n, normal.NumBytes());
	}
    }
  if (has_color)
    {
      int color_count = color.Count();
      for(int c=0;c<color_count;c++)
	{
	  color.OrigToAdjusted(c, color.NumBytes());
	}
    }
  if (has_texcoord)
    {
      int texcoord_count = texcoord.Count();
      for(int t = 0;t<texcoord_count;t++)
	{
	  texcoord.OrigToAdjusted(t, texcoord.NumBytes());
	}
    }
}
void VBOObjects::OrigToAdjusted(bool has_normal, bool has_color, bool has_texcoord)
{
  int s = objs.size();
  for(int i=0;i<s;i++)
    {
      DataTypes &o = objs[i];
      o.OrigToAdjusted(has_normal, has_color, has_texcoord);
    }
}
void VBOObjects::AllocWholeBuffer(bool has_normal, bool has_color, bool has_texcoord)
{
  m_has_normal = has_normal;
  m_has_color = has_color;
  m_has_texcoord = has_texcoord;
  int s = objs.size();
  int vertex_count = 0;
  int normal_count = 0;
  int color_count = 0;
  int texcoord_count = 0;
  for(int i=0;i<s;i++)
    {
      DataTypes &dt = objs[i];
      vertex_count += dt.vertex.NumBytes();
      if (has_normal)
	normal_count += dt.normal.NumBytes();
      if (has_color)
	color_count += dt.color.NumBytes();
      if (has_texcoord)
	texcoord_count += dt.texcoord.NumBytes();
    }
  delete [] wholebuffer;
  wholebuffer = new char[vertex_count+normal_count+color_count+texcoord_count];
  wholebuffer_vertex_size = vertex_count;
  wholebuffer_normal_size = normal_count;
  wholebuffer_color_size = color_count;
  wholebuffer_texcoord_size = texcoord_count;
}
std::pair<char*, int> VBOObjects::BufferPosPart(int type, int obj_num, int frame)
{
  return objs[obj_num].BufferPosPart(type, frame);
}
std::pair<int,int> VBOObjects::BufferPosWhole(int type, int obj_num)
{
  int s = objs.size();
  int vertex_count = 0;
  int normal_count = 0;
  int color_count = 0;
  int texcoord_count = 0;
  int vertex_size = 0;
  int normal_size = 0;
  int color_size = 0;
  int texcoord_size = 0;
  for(int i=0;i<s;i++)
    {
      DataTypes &dt = objs[i];
      if (i==obj_num)
	{
	  vertex_size = dt.vertex.NumBytes();
	  normal_size = dt.normal.NumBytes();
	  color_size = dt.color.NumBytes();
	  texcoord_size = dt.texcoord.NumBytes();
	break;
	}
      vertex_count += dt.vertex.NumBytes();
      if (m_has_normal)
	normal_count += dt.normal.NumBytes();
      if (m_has_color)
	color_count += dt.color.NumBytes();
      if (m_has_texcoord)
	texcoord_count += dt.texcoord.NumBytes();
    }
  switch(type)
    {
    case 0: return std::make_pair(vertex_count, vertex_size);
    case 1: return std::make_pair(normal_count, normal_size);
    case 2: return std::make_pair(color_count, color_size);
    case 3: return std::make_pair(texcoord_count, texcoord_size);
    };
  return std::make_pair(0,0);
}

class BlitWholeInterface
{
public:
  BlitWholeInterface(VBOObjects &obj, int obj_num, char *whole_buffer) : obj(obj),obj_num(obj_num), whole_buffer(whole_buffer) { }
  void Blit(int type, int choose)
  {
    std::pair<char*, int> pospart = obj.BufferPosPart(type, obj_num, choose);
    std::pair<int,int> pos = obj.BufferPosWhole(type, obj_num);
    std::copy(pospart.first, pospart.first+pospart.second, whole_buffer+pos.first);
  }
private:
  VBOObjects &obj;
  int obj_num;
  char *whole_buffer;
};
class BlitPartInterface
{
public:
  BlitPartInterface(VBOObjects &obj, int obj_num) : obj(obj), obj_num(obj_num) { }
  void Blit(int type, int choose)
  {
    std::pair<char*, int> pospart = obj.BufferPosPart(type, obj_num, choose);
    std::pair<int,int> pos = obj.BufferPosWhole(type, obj_num);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, obj.priv->vboId);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, pos.first, pos.second, pospart.first);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  }
private:
  VBOObjects &obj;
  int obj_num;
};

void VBOObjects::CopyPartsToWholeBuffer(int vertex, int normal, int color, int texcoord)
{
  int s = objs.size();
  for(int i=0;i<s;i++)
    {
      int vertex_offset = 0;
      int normal_offset = vertex_offset+wholebuffer_vertex_size;
      int color_offset = normal_offset + wholebuffer_normal_size;
      int texcoord_offset = color_offset + wholebuffer_color_size;
      BlitWholeInterface blit_vertex(*this, i, wholebuffer+ vertex_offset);
      blit_vertex.Blit(0, vertex);
      if (normal != -1)
	{
	  BlitWholeInterface blit_normal(*this, i, wholebuffer+normal_offset);
	  blit_normal.Blit(1, normal);
	}
      if (color != -1)
	{
	  BlitWholeInterface blit_color(*this, i, wholebuffer+color_offset);
	  blit_color.Blit(2, color);
	}
      if (texcoord != -1)
	{
	  BlitWholeInterface blit_texcoord(*this, i, wholebuffer+texcoord_offset);
	  blit_texcoord.Blit(3, texcoord);
	}
    }
}


VBOObjects::VBOObjects()
  : wholebuffer(0),
    wholebuffer_vertex_size(0),
    wholebuffer_normal_size(0),
    wholebuffer_color_size(0),
    wholebuffer_texcoord_size(0),
    m_has_normal(false),
    m_has_color(false),
    m_has_texcoord(false),
    m_quads(false)
{
  priv = new VBOObjects::Priv;
  priv->allocated=false;
}
void VBOObjects::BlitWholeBufferToGPU()
{
  if (!priv->allocated)
    {
      //std::cout << glGenBuffersARB << std::endl;
      glGenBuffersARB(1, &priv->vboId);
      priv->allocated=true;
    }
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vboId);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, wholebuffer_vertex_size+wholebuffer_normal_size+wholebuffer_color_size+wholebuffer_texcoord_size, wholebuffer, GL_DYNAMIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
void VBOObjects::BlitGPUToScreen()
{
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vboId);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  if (m_has_normal)
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, (GLvoid*)wholebuffer_vertex_size);
    }
  if (m_has_color)
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)(wholebuffer_vertex_size+wholebuffer_normal_size));
    }
  if (m_has_texcoord)
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(wholebuffer_vertex_size+wholebuffer_normal_size+wholebuffer_color_size));
    }
  
  if (m_quads)
    {
      glDrawArrays(GL_QUADS, 0, wholebuffer_vertex_size/sizeof(VertexElem));
    }
  else
    {
      glDrawArrays(GL_TRIANGLES, 0, wholebuffer_vertex_size/sizeof(VertexElem));
    }
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}


void VBOObjects::CopyPartToGPU(int obj_num, int vertex, int normal, int color, int texcoord)
{

  BlitPartInterface blit_vertex(*this, obj_num);
  blit_vertex.Blit(0, vertex);
  if (normal != -1)
    {
      BlitPartInterface blit_normal(*this, obj_num);
      blit_normal.Blit(1, normal);
    }
  if (color != -1)
    {
      BlitPartInterface blit_color(*this, obj_num);
      blit_color.Blit(2, color);
    }
  if (texcoord != -1)
    {
      BlitPartInterface blit_texcoord(*this, obj_num);
      blit_texcoord.Blit(3, texcoord);
    }
}

void DataTypes::SetFrames(int type, int frames)
{
  switch(type)
    {
    case 0: vertex.SetFrames(frames); break;
    case 1: normal.SetFrames(frames); break;
    case 2: color.SetFrames(frames); break;
    case 3: texcoord.SetFrames(frames); break;
    }
}

void VBOObjects::SetFrameCount(int obj_num, int type, int num_frames)
{
  objs[obj_num].SetFrames(type, num_frames);
}
void VBOObjects::MeshToOrig(int objnum, Mesh &mesh)
{
  Mesh *a_mesh = &mesh;
  MeshToTriangles tris(mesh);
  if (mesh.NumPoints() == 4)
    {
      a_mesh = &tris;
    }

  int frames = a_mesh->NumFrames();
  SetFrameCount(objnum, 0, frames);
  for(int fr = 0; fr<frames; fr++)
    {
      int faces = a_mesh->NumFaces(fr);
      int points = a_mesh->NumPoints();
      m_quads = points==4;
      VertexElem *e = new VertexElem[faces*points];
      for(int f=0;f<faces;f++)
	{
	  for(int p=0;p<points;p++)
	    {
	      Point pp = a_mesh->FacePoint(fr, 0, f, p);
	      VertexElem *elem = &e[f*points+p];
	      elem->x = pp.x;
	      elem->y = pp.y;
	      elem->z = pp.z;
	    }
	}
      objs[objnum].vertex.vec[fr] = DataBlock(0, e, faces*points);
    }
}

void VBOObjects::MeshNormalsToOrig(int objnum, Mesh &mesh, MeshNormals &normals)
{
  Mesh *a_mesh = &mesh;
  MeshNormals *a_normals = &normals;
  MeshToTriangles tris(mesh);
  MeshNormalsToTriangles norm(normals, true);
  if (mesh.NumPoints() == 4)
    {
      a_mesh = &tris;
      a_normals = &norm;
    }

  int frames = a_normals->NumFrames();
  SetFrameCount(objnum, 1, frames);
  for(int fr = 0; fr<frames; fr++)
    {
      int faces = a_mesh->NumFaces(fr);
      int points = a_mesh->NumPoints();
      NormalElem *e = new NormalElem[faces*points];
      for(int f=0;f<faces;f++)
	{
	  for(int p=0;p<points;p++)
	    {
	      Vector vv = a_normals->PointNormal(fr, 0, f, p);
	      NormalElem *elem = &e[f*points+p];
	      elem->nx = vv.dx;
	      elem->ny = vv.dy;
	      elem->nz = vv.dz;
	    }
	}
      objs[objnum].normal.vec[fr] = DataBlock(0, e, faces*points);
    }
}

void VBOObjects::MeshColorsToOrig(int objnum, Mesh &mesh, MeshColors &colors)
{
  Mesh *a_mesh = &mesh;
  MeshColors *a_colors = &colors;
  MeshToTriangles tris(mesh);
  MeshColorsToTriangles color(colors, true);
  if (mesh.NumPoints() == 4)
    {
      a_mesh = &tris;
      a_colors = &color;
    }

  int frames = a_colors->NumFrames();
  SetFrameCount(objnum, 2, frames);
  for(int fr = 0; fr<frames; fr++)
    {
      int faces = a_mesh->NumFaces(fr);
      int points = a_mesh->NumPoints();
      ColorElem2 *e = new ColorElem2[faces*points];
      for(int f=0;f<faces;f++)
	{
	  for(int p=0;p<points;p++)
	    {
	      unsigned int pp = a_colors->VertexColor(fr,0, f, p);
	      ColorElem2 *elem = &e[f*points+p];
	      elem->r = (pp & 0xff000000)>>24;
	      elem->g = (pp & 0x00ff0000)>>16;
	      elem->b = (pp & 0x0000ff00)>>8;
	      elem->a = (pp & 0x000000ff);
	    }
	}
      objs[objnum].color.vec[fr] = DataBlock(0, e, faces*points);
    }
}


void VBOObjects::MeshTexCoordsToOrig(int objnum, Mesh &mesh, MeshTexCoords &texcoords)
{
  Mesh *a_mesh = &mesh;
  MeshTexCoords *a_texcoords = &texcoords;
  MeshToTriangles tris(mesh);
  MeshTexCoordsToTriangles norm(texcoords, true);
  if (mesh.NumPoints() == 4)
    {
      a_mesh = &tris;
      a_texcoords = &norm;
    }

  int frames = a_texcoords->NumFrames();
  SetFrameCount(objnum, 3, frames);
  for(int fr = 0; fr<frames; fr++)
    {
      int faces = a_mesh->NumFaces(fr);
      int points = a_mesh->NumPoints();
      TexCoordElem *e = new TexCoordElem[faces*points];
      for(int f=0;f<faces;f++)
	{
	  for(int p=0;p<points;p++)
	    {
	      Point2d pp = a_texcoords->TexCoord(fr, 0, f, p);
	      TexCoordElem *elem = &e[f*points+p];
	      elem->tx = pp.x;
	      elem->ty = pp.y;
	    }
	}
      objs[objnum].texcoord.vec[fr] = DataBlock(0, e, faces*points);
    }
}

void VBOObjects::AllocTexture(int count)
{
  int texcount = count;
  texture = new int[texcount];
  glGenTextures(texcount, (GLuint*)&texture[0]);
}
void VBOObjects::UpdateTexture(MeshTextures &tex, int frame, int pos)
{
  tex.GenTexture(frame-pos);
  BufferRef ref = tex.TextureBuf(frame-pos);
  int sizex = ref.width;
  int sizey = ref.height;
  glBindTexture(GL_TEXTURE_2D, texture[frame]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}
void VBOObjects::EnableTexture(int frame)
{
  glBindTexture(GL_TEXTURE_2D, texture[frame]);
  glEnable(GL_TEXTURE_2D);
}
void VBOObjects::DisableTexture()
{
  glDisable(GL_TEXTURE_2D);
}
#endif

void FaceCollectionBuffer::CalculateSizes()
{
  int vertex_count = 0;
  int normal_count = 0;
  int color_count = 0;
  int tex_count = 0;
  int faces = coll->NumFaces();
  for(int f=0;f<faces;f++)
    {
      int points = coll->NumPoints(f);
      if (points==3)
	{
	  if (m_vertex)
	    vertex_count+=3*3;
	  if (m_normal)
	    normal_count+=3*3;
	  if (m_color)
	    color_count+=3*1;
	  if (m_texcoord)
	    tex_count+=2*3;
	}
      else if (points==4)
	{
	  if (m_vertex)
	    vertex_count+=6*3;
	  if (m_normal)
	    normal_count+=6*3;
	  if (m_color)
	    color_count+=6*1;
	  if (m_texcoord)
	    tex_count+=2*3;
	}
    }
  vertex_size = vertex_count;
  normal_size = normal_count;
  color_size = color_count;
  texcoord_size = tex_count;
}
void FaceCollectionBuffer::CreateBuffers()
{
  vertex_buffer = new Buffer<float>(vertex_size);
  normal_buffer = new Buffer<float>(normal_size);
  color_buffer = new Buffer<unsigned int>(color_size);
  texcoord_buffer = new Buffer<float>(texcoord_size);
  if (attrib_ids.size())
    attrib_buffers = new Buffer<float>*[attrib_ids.size()];
  else
    attrib_buffers = 0;

  int *attrib_index;
  attrib_index = 0;
  if (attrib_ids.size()) { attrib_index = new int[attrib_ids.size()]; }
  int vertex_index = 0;
  int normal_index = 0;
  int color_index = 0;
  int texcoord_index = 0;

  int faces = coll->NumFaces();
  for(int f=0;f<faces;f++)
    {
      int points = coll->NumPoints(f);
      if (m_vertex) {
      if (points==3) {
	Point p1 = coll->FacePoint(f,0);
	vertex_buffer[vertex_index++]=p1.x;
	vertex_buffer[vertex_index++]=p1.y;
	vertex_buffer[vertex_index++]=p1.z;
	Point p2 = coll->FacePoint(f,1);
	vertex_buffer[vertex_index++]=p2.x;
	vertex_buffer[vertex_index++]=p2.y;
	vertex_buffer[vertex_index++]=p2.z;
	Point p3 = coll->FacePoint(f,2);
	vertex_buffer[vertex_index++]=p3.x;
	vertex_buffer[vertex_index++]=p3.y;
	vertex_buffer[vertex_index++]=p3.z;
      } else if (points==4) {
	Point p1 = coll->FacePoint(f,0);
	Point p2 = coll->FacePoint(f,1);
	Point p3 = coll->FacePoint(f,2);
	Point p4 = coll->FacePoint(f,3);
	vertex_buffer[vertex_index++]=p1.x;
	vertex_buffer[vertex_index++]=p1.y;
	vertex_buffer[vertex_index++]=p1.z;
	vertex_buffer[vertex_index++]=p2.x;
	vertex_buffer[vertex_index++]=p2.y;
	vertex_buffer[vertex_index++]=p2.z;
	vertex_buffer[vertex_index++]=p3.x;
	vertex_buffer[vertex_index++]=p3.y;
	vertex_buffer[vertex_index++]=p3.z;

	vertex_buffer[vertex_index++]=p1.x;
	vertex_buffer[vertex_index++]=p1.y;
	vertex_buffer[vertex_index++]=p1.z;
	vertex_buffer[vertex_index++]=p3.x;
	vertex_buffer[vertex_index++]=p3.y;
	vertex_buffer[vertex_index++]=p3.z;
	vertex_buffer[vertex_index++]=p4.x;
	vertex_buffer[vertex_index++]=p4.y;
	vertex_buffer[vertex_index++]=p4.z;
      }
      }
      if (m_normal) {
      if (points==3) {
	Vector p1 = coll->PointNormal(f,0);
	normal_buffer[normal_index++]=p1.dx;
	normal_buffer[normal_index++]=p1.dy;
	normal_buffer[normal_index++]=p1.dz;
	Vector p2 = coll->PointNormal(f,1);
	normal_buffer[normal_index++]=p2.dx;
	normal_buffer[normal_index++]=p2.dy;
	normal_buffer[normal_index++]=p2.dz;
	Vector p3 = coll->PointNormal(f,2);
	normal_buffer[normal_index++]=p3.dx;
	normal_buffer[normal_index++]=p3.dy;
	normal_buffer[normal_index++]=p3.dz;
      } else if (points==4) {
	Vector p1 = coll->PointNormal(f,0);
	Vector p2 = coll->PointNormal(f,1);
	Vector p3 = coll->PointNormal(f,2);
	Vector p4 = coll->PointNormal(f,3);
	normal_buffer[normal_index++]=p1.dx;
        normal_buffer[normal_index++]=p1.dy;
	normal_buffer[normal_index++]=p1.dz;
        normal_buffer[normal_index++]=p2.dx;
	normal_buffer[normal_index++]=p2.dy;
	normal_buffer[normal_index++]=p2.dz;
	normal_buffer[normal_index++]=p3.dx;
	normal_buffer[normal_index++]=p3.dy;
	normal_buffer[normal_index++]=p3.dz;

	normal_buffer[normal_index++]=p1.dx;
	normal_buffer[normal_index++]=p1.dy;
        normal_buffer[normal_index++]=p1.dz;
	normal_buffer[normal_index++]=p3.dx;
	normal_buffer[normal_index++]=p3.dy;
	normal_buffer[normal_index++]=p3.dz;
	normal_buffer[normal_index++]=p4.dx;
	normal_buffer[normal_index++]=p4.dy;
	normal_buffer[normal_index++]=p4.dz;
      }
      }
      int s = attrib_ids.size();
      for(int id=0;id<s;id++)
	{
	  if (points==3) {
	    float p1 = coll->Attrib(f,0,attrib_ids[id]);
	    attrib_buffers[id]->set(attrib_index[id]++,p1);
	    float p2 = coll->Attrib(f,1,attrib_ids[id]);
	    attrib_buffers[id]->set(attrib_index[id]++,p2);
	    float p3 = coll->Attrib(f,2,attrib_ids[id]);
	    attrib_buffers[id]->set(attrib_index[id]++,p3);
	  } else if (points==4) {
	    float p1 = coll->Attrib(f,0,attrib_ids[id]);
	    float p2 = coll->Attrib(f,1,attrib_ids[id]);
	    float p3 = coll->Attrib(f,2,attrib_ids[id]);
	    float p4 = coll->Attrib(f,3,attrib_ids[id]);
	    attrib_buffers[id]->set(attrib_index[id]++,p1);
	    attrib_buffers[id]->set(attrib_index[id]++,p2);
	    attrib_buffers[id]->set(attrib_index[id]++,p3);
	    
	    attrib_buffers[id]->set(attrib_index[id]++,p1);
	    attrib_buffers[id]->set(attrib_index[id]++,p3);
	    attrib_buffers[id]->set(attrib_index[id]++,p4);
	  }
	}

      if (m_color) {
      if (points==3) {
	unsigned int p1 = coll->Color(f,0);
	color_buffer[color_index++]=p1;
	unsigned int p2 = coll->Color(f,1);
	color_buffer[color_index++]=p2;
	unsigned int p3 = coll->Color(f,2);
	color_buffer[color_index++]=p3;
      } else if (points==4) {
	unsigned int p1 = coll->Color(f,0);
	unsigned int p2 = coll->Color(f,1);
	unsigned int p3 = coll->Color(f,2);
	unsigned int p4 = coll->Color(f,3);
	color_buffer[color_index++]=p1;
	color_buffer[color_index++]=p2;
	color_buffer[color_index++]=p3;
	
	color_buffer[color_index++]=p1;
	color_buffer[color_index++]=p3;
	color_buffer[color_index++]=p4;
      }
      }
      if (m_texcoord) {
      if (points==3) {
	Point2d  p1 = coll->TexCoord(f,0);
	texcoord_buffer[texcoord_index++]=p1.x;
	texcoord_buffer[texcoord_index++]=p1.y;
	Point2d p2 = coll->TexCoord(f,1);
	texcoord_buffer[texcoord_index++]=p2.x;
	texcoord_buffer[texcoord_index++]=p2.y;
	Point2d p3 = coll->TexCoord(f,2);
	texcoord_buffer[texcoord_index++]=p3.x;
	texcoord_buffer[texcoord_index++]=p3.y;
      } else if (points==4) {
	Point2d p1 = coll->TexCoord(f,0);
        Point2d p2 = coll->TexCoord(f,1);
	Point2d p3 = coll->TexCoord(f,2);
	Point2d p4 = coll->TexCoord(f,3);
	texcoord_buffer[texcoord_index++]=p1.x;
	texcoord_buffer[texcoord_index++]=p1.y;
	texcoord_buffer[texcoord_index++]=p2.x;
	texcoord_buffer[texcoord_index++]=p2.y;
	texcoord_buffer[texcoord_index++]=p3.x;
	texcoord_buffer[texcoord_index++]=p3.y;
	
	texcoord_buffer[texcoord_index++]=p1.x;
	texcoord_buffer[texcoord_index++]=p1.y;
	texcoord_buffer[texcoord_index++]=p3.x;
	texcoord_buffer[texcoord_index++]=p3.y;
	texcoord_buffer[texcoord_index++]=p4.x;
	texcoord_buffer[texcoord_index++]=p4.y;
      }
      }
    }
}

void RenderBuffers::SetupVertex(const Buffer<float> &buf, int pos)
{
  vertex=true;
  // enabling
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, buf.Array()+pos);
}
void RenderBuffers::SetupAttrib(int id, const Buffer<float> &buf, int pos)
{
  attrib_ids.push_back(id);
  glEnableVertexAttribArray(id);
  glVertexAttribPointer(id, 1, GL_FLOAT, GL_FALSE, 0, buf.Array()+pos);  
}

void RenderBuffers::SetupNormal(const Buffer<float> &buf, int pos)
{
  normal=true;
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, buf.Array()+pos);
}
void RenderBuffers::SetupColor(const Buffer<unsigned int> &buf, int pos)
{
  color=true;
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, buf.Array()+pos);
}
void RenderBuffers::SetupTexCoord(const Buffer<float> &buf, int pos)
{
  texcoord=true;
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, buf.Array()+pos);
}
void RenderBuffers::Render(bool quads, int start_elem, int vertex_size)
{
  // Render
  if (quads)
    {
      glDrawArrays(GL_QUADS, start_elem, vertex_size);
    }
  else
    {
      glDrawArrays(GL_TRIANGLES, start_elem, vertex_size);
    }
}
#if 0
void RenderBuffers::RenderOne(int start_elem, int vertex_size)
{
  glDrawArrays(GL_POLYGON, start_elem, vertex_size);
}
#endif

void RenderBuffers::DisableAll()
{
  // disabling
  if (texcoord)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if (color)
    {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  if (normal)
    {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  if (vertex)
    {
    glDisableClientState(GL_VERTEX_ARRAY);
    }
  int s = attrib_ids.size();
  for(int i=0;i<s;i++)
    {
      glDisableVertexAttribArray(attrib_ids[i]);
    }
  vertex=false;
  normal=false;
  color=false;
  texcoord=false;
}

Point ShadowFaceCollection::FacePoint(int face, int point) const
{
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Vector v = light_vec;
    LinePlaneIntersection sect = LinePlaneIntersectionFunc(p, p+v, pos, pos+u_x, pos+u_y);
    return p+v*sect.tuv.dx;
}

Point ReflectFaceCollection::FacePoint(int face, int point) const
{
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Vector v = ref_vec;
    LinePlaneIntersection sect = LinePlaneIntersectionFunc(p, p+v, pos, pos+u_x, pos+u_y);
    Point pp = p+v*sect.tuv.dx;
    Vector v2 = pp-p;
    Point pp2 = pp + v2;
    return pp2;
}

