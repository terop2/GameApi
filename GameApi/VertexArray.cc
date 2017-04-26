 
#include "VertexArray.hh"

//#define GL_GLEXT_PROTOTYPES
#define NO_SDL_GLEXT
#include "Effect.hh"
#include "Effect2.hh"
#include <GL/glew.h>
#include <SDL_opengl.h>

#ifndef EMSCRIPTEN
#define VAO 1
#endif
void VertexArraySet::check_m_set(int id)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }  
}
void VertexArraySet::print_stat(int id)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];  
    }
  std::cout << "tri_polys: " << p->tri_polys.size() << std::endl;
  std::cout << "quad_polys: " << p->quad_polys.size() << std::endl;

}
void VertexArraySet::explode(int id, Point pt, float dist)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];  
    }
  std::vector<std::vector<Point>* > vec;
  vec.push_back(&p->tri_polys);
  vec.push_back(&p->quad_polys);

  int s = vec.size();
  for(int k=0;k<s;k++)
    {
      std::vector<Point> *v = vec[k];
      float *arr2 = (float*)&v->operator[](0);
      Point p2 = pt;
      int numpoints = v->size();
      //std::cout << numpoints << std::endl;
      for(int i=0;i<numpoints;i++)
	{
	  Point p(arr2[0], arr2[1], arr2[2]);
	  //std::cout << p << " ";
	  Vector v = p-p2;
	  float d = v.Dist();
	  v/=d;
	  v*=d+dist;
	  Point pp = p2+v;
	  //std::cout << pp << std::endl;
	  arr2[0] = pp.x;
	  arr2[1] = pp.y;
	  arr2[2] = pp.z;
	  arr2+=3;
	}
    }
}
void VertexArraySet::set_reserve(int id, int tri_count, int quad_count, int poly_count)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];  
    }
  //std::cout << "Reserve: " << tri_count << " " << quad_count << std::endl;
  p->tri_polys.reserve(tri_count*3);  
  p->quad_polys.reserve(quad_count*6);
  p->poly_polys.reserve(poly_count*3);
  p->tri_polys2.reserve(tri_count*3);
  p->quad_polys2.reserve(quad_count*6);  
  p->poly_polys2.reserve(poly_count*3);
  p->tri_normals.reserve(tri_count*3);  
  p->quad_normals.reserve(quad_count*6); 
  p->poly_normals.reserve(poly_count*3);
  p->tri_color.reserve(tri_count*3); 
  p->quad_color.reserve(quad_count*6);
  p->poly_color.reserve(poly_count*3);
  p->tri_texcoord.reserve(tri_count*3);
  p->quad_texcoord.reserve(quad_count*6);
  p->poly_texcoord.reserve(poly_count*3);
}
void VertexArraySet::push_poly(int id, int num, Point *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_polys.push_back(points[0]);
      p->tri_polys.push_back(points[1]);
      p->tri_polys.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_polys.push_back(points[0]);
      p->quad_polys.push_back(points[1]);
      p->quad_polys.push_back(points[2]);
      p->quad_polys.push_back(points[0]);
      p->quad_polys.push_back(points[2]);
      p->quad_polys.push_back(points[3]);
    }
  else
    {
      if (p->poly_polys.size()!=0)
	{
	  p->poly_polys.push_back(points[0]);
	}
      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_polys.push_back(points[i]);
	  p->poly_polys.push_back(points[j]);
	}
      p->poly_polys.push_back(p->poly_polys[p->poly_polys.size()-1]);
    }
}

void VertexArraySet::push_poly2(int id, int num, Point *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_polys2.push_back(points[0]);
      p->tri_polys2.push_back(points[1]);
      p->tri_polys2.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_polys2.push_back(points[0]);
      p->quad_polys2.push_back(points[1]);
      p->quad_polys2.push_back(points[2]);
      p->quad_polys2.push_back(points[0]);
      p->quad_polys2.push_back(points[2]);
      p->quad_polys2.push_back(points[3]);
    }
  else
    {
      if (p->poly_polys2.size()!=0)
	{
	  p->poly_polys2.push_back(points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_polys2.push_back(points[i]);
	  p->poly_polys2.push_back(points[j]);
	}
      p->poly_polys2.push_back(p->poly_polys2[p->poly_polys2.size()-1]);

    }
}



void VertexArraySet::push_normal(int id, int num, Vector *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_normals.push_back(points[0]);
      p->tri_normals.push_back(points[1]);
      p->tri_normals.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_normals.push_back(points[0]);
      p->quad_normals.push_back(points[1]);
      p->quad_normals.push_back(points[2]);
      p->quad_normals.push_back(points[0]);
      p->quad_normals.push_back(points[2]);
      p->quad_normals.push_back(points[3]);
    }
  else
    {
      if (p->poly_normals.size()!=0)
	{
	  p->poly_normals.push_back(points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_normals.push_back(points[i]);
	  p->poly_normals.push_back(points[j]);
	}
      p->poly_normals.push_back(p->poly_normals[p->poly_normals.size()-1]);

    }
}

void VertexArraySet::push_attrib(int id, int attrib_id, int num, float *points)
{
#if 1
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_attribs[attrib_id].push_back(points[0]);
      p->tri_attribs[attrib_id].push_back(points[1]);
      p->tri_attribs[attrib_id].push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << "Attrib1: " << points[0] << std::endl;
      //std::cout << "Attrib2: " << points[1] << std::endl;
      //std::cout << "Attrib3: " << points[2] << std::endl;
      //std::cout << "Attrib4: " << points[3] << std::endl;
      p->quad_attribs[attrib_id].push_back(points[0]);
      p->quad_attribs[attrib_id].push_back(points[1]);
      p->quad_attribs[attrib_id].push_back(points[2]);
      p->quad_attribs[attrib_id].push_back(points[0]);
      p->quad_attribs[attrib_id].push_back(points[2]);
      p->quad_attribs[attrib_id].push_back(points[3]);
    }
  else
    {
#if 0
      p->poly_attribs[attrib_id].push_back(std::vector<float>());
      int s = p->poly_attribs[attrib_id].size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_attribs[attrib_id][s].push_back(points[i]);
	}
#endif
    }
#endif
}

void VertexArraySet::push_attribi(int id, int attrib_id, int num, int *points)
{
#if 1
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_attribsi[attrib_id].push_back(points[0]);
      p->tri_attribsi[attrib_id].push_back(points[1]);
      p->tri_attribsi[attrib_id].push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << attrib_id << std::endl;
      //std::cout << points[0] << " " << points[1] << " " << points[2] << " " << points[3] << std::endl;
      p->quad_attribsi[attrib_id].push_back(points[0]);
      p->quad_attribsi[attrib_id].push_back(points[1]);
      p->quad_attribsi[attrib_id].push_back(points[2]);
      p->quad_attribsi[attrib_id].push_back(points[0]);
      p->quad_attribsi[attrib_id].push_back(points[2]);
      p->quad_attribsi[attrib_id].push_back(points[3]);
    }
  else
    {
#if 0
      p->poly_attribsi[attrib_id].push_back(std::vector<int>());
      int s = p->poly_attribsi[attrib_id].size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_attribsi[attrib_id][s].push_back(points[i]);
	}
#endif
    }
#endif
}
void VertexArraySet::split_color(std::vector<float> &target, unsigned int color)
{
  unsigned int alpha = color & 0xff000000;
  unsigned int red = color & 0xff0000;
  unsigned int green = color & 0xff00;
  unsigned int blue = color & 0xff;

  alpha >>= 24;
  red >>= 16;
  green >>= 8;
  
  float falpha = float(alpha)/256.0;
  float fred = float(red)/256.0;
  float fgreen = float(green)/256.0;
  float fblue = float(blue)/256.0;

  target.push_back(fred);
  target.push_back(fgreen);
  target.push_back(fblue);
  target.push_back(falpha);
  //std::cout << "Color value" << fred << " " << fgreen << " " << fblue << std::endl;
}

void VertexArraySet::push_color(int id, int num, unsigned int *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      split_color(p->tri_color, points[0]);
      split_color(p->tri_color, points[1]);
      split_color(p->tri_color, points[2]);
    }
  else if (num==4)
    {
      split_color(p->quad_color, points[0]);
      split_color(p->quad_color, points[1]);
      split_color(p->quad_color, points[2]);
      split_color(p->quad_color, points[0]);
      split_color(p->quad_color, points[2]);
      split_color(p->quad_color, points[3]);
    }
  else
    {
      if (p->poly_color.size()!=0)
	{
	  split_color(p->poly_color, points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  split_color(p->poly_color,points[i]);
	  split_color(p->poly_color,points[j]);
	}
      split_color(p->poly_color, points[num/2]); // todo, check if correct
    }
}


void VertexArraySet::push_texcoord(int id, int num, Point *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_texcoord.push_back(points[0]);
      p->tri_texcoord.push_back(points[1]);
      p->tri_texcoord.push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << "Vertex: " << points[0] << " " << points[1] << " " << points[2] << " " << points[3] << std::endl;
      p->quad_texcoord.push_back(points[0]);
      p->quad_texcoord.push_back(points[1]);
      p->quad_texcoord.push_back(points[2]);
      p->quad_texcoord.push_back(points[0]);
      p->quad_texcoord.push_back(points[2]);
      p->quad_texcoord.push_back(points[3]);
    }
  else
    {
      if (p->poly_texcoord.size()!=0)
	{
	  p->poly_texcoord.push_back(points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_texcoord.push_back(points[i]);
	  p->poly_texcoord.push_back(points[j]);
	}
      p->poly_texcoord.push_back(p->poly_texcoord[p->poly_texcoord.size()-1]);

    }
}
VertexArraySet::~VertexArraySet()
{
    std::map<int,Polys*>::iterator it = m_set.begin();
    for(;it!=m_set.end();it++)
      {
	Polys *ptr = (*it).second;
	delete ptr;
      }
}
void VertexArraySet::free_memory()
{
#if 1
  std::map<int,Polys*>::iterator i;
  for(i=m_set.begin();i!=m_set.end();i++)
    {
      Polys *poly = (*i).second;
      poly->tri_polys.resize(0);
      poly->quad_polys.resize(0);
      poly->poly_polys.resize(0);
      poly->tri_polys2.resize(0);
      poly->quad_polys2.resize(0);
      poly->poly_polys2.resize(0);
      poly->tri_normals.resize(0);
      poly->quad_normals.resize(0);
      poly->poly_normals.resize(0);
      poly->tri_color.resize(0);
      poly->quad_color.resize(0);
      poly->poly_color.resize(0);
      poly->tri_texcoord.resize(0);
      poly->quad_texcoord.resize(0);
      poly->poly_texcoord.resize(0);

      poly->tri_polys.shrink_to_fit();
      poly->quad_polys.shrink_to_fit();
      poly->poly_polys.shrink_to_fit();
      poly->tri_polys2.shrink_to_fit();
      poly->quad_polys2.shrink_to_fit();
      poly->poly_polys2.shrink_to_fit();
      poly->tri_normals.shrink_to_fit();
      poly->quad_normals.shrink_to_fit();
      poly->poly_normals.shrink_to_fit();
      poly->tri_color.shrink_to_fit();
      poly->quad_color.shrink_to_fit();
      poly->poly_color.shrink_to_fit();
      poly->tri_texcoord.shrink_to_fit();
      poly->quad_texcoord.shrink_to_fit();
      poly->poly_texcoord.shrink_to_fit();
    }
#endif
}
template<class T>
void append_from_map(const std::map<int,std::vector<T> > &m,
		     std::map<int,std::vector<T> > &m2)
{
  typename std::map<int,std::vector<T> >::const_iterator i = m.begin();
  for(;i!=m.end();i++) 
    {
    int s = ((*i).second).size();
      for(int ii=0;ii<s;ii++)
	{
	  T t = ((*i).second)[ii];
	  m2[(*i).first].push_back(t);
	}
    }
  
}

void VertexArraySet::append_to_polys(Polys &target, const Polys &source)
{
  int s1 = source.tri_polys.size();
  target.tri_polys.reserve(target.tri_polys.size() + s1);
  for(int i=0;i<s1;i++)
    {
      target.tri_polys.push_back(source.tri_polys[i]);
    }
  //std::cout << "tri_polys: " << target.tri_polys.size() << std::endl;
  int s2 = source.quad_polys.size();
  target.quad_polys.reserve(target.quad_polys.size()+s2);
  for(int i=0;i<s2;i++)
    {
      target.quad_polys.push_back(source.quad_polys[i]);
    }
  //std::cout << "quad_polys: " << target.quad_polys.size() << std::endl;
  int s2a = source.poly_polys.size();
  target.poly_polys.reserve(s2a);
  for(int i=0;i<s2a;i++)
    {
      target.poly_polys.push_back(source.poly_polys[i]);
    }
  //std::cout << "poly_polys: " << target.poly_polys.size() << std::endl;
  int s3 = source.tri_polys2.size();
  target.tri_polys2.reserve(target.tri_polys2.size()+s3);
  for(int i=0;i<s3;i++)
    {
      target.tri_polys2.push_back(source.tri_polys2[i]);
    }
  //std::cout << "tri_polys2: " << target.tri_polys2.size() << std::endl;
  int s4 = source.quad_polys2.size();
  target.quad_polys2.reserve(target.quad_polys2.size()+s4);
  for(int i=0;i<s4;i++)
    {
      target.quad_polys2.push_back(source.quad_polys2[i]);
    }
  //std::cout << "quad_polys2: " << target.quad_polys2.size() << std::endl;
  int s2b = source.poly_polys2.size();
  target.poly_polys2.reserve(target.poly_polys2.size()+s2b);
  for(int i=0;i<s2b;i++)
    {
      target.poly_polys2.push_back(source.poly_polys2[i]);
    }
  //std::cout << "poly_polys2: " << target.poly_polys2.size() << std::endl;

  int s5 = source.tri_normals.size();
  target.tri_normals.reserve(target.tri_normals.size()+s5);
  for(int i=0;i<s5;i++)
    {
      target.tri_normals.push_back(source.tri_normals[i]);
    }
  //std::cout << "tri_normals: " << target.tri_normals.size() << std::endl;
  int s6 = source.quad_normals.size();
  target.quad_normals.reserve(target.quad_normals.size()+s6);
  for(int i=0;i<s6;i++)
    {
      target.quad_normals.push_back(source.quad_normals[i]);
    }
  //std::cout << "quad_normals: " << target.quad_normals.size() << std::endl;
  int s2c = source.poly_normals.size();
  target.poly_normals.reserve(target.poly_normals.size()+s2c);
  for(int i=0;i<s2c;i++)
    {
      target.poly_normals.push_back(source.poly_normals[i]);
    }
  //std::cout << "poly_normals: " << target.poly_normals.size() << std::endl;

  int s7 = source.tri_color.size();
  target.tri_color.reserve(target.tri_color.size()+s7);
  for(int i=0;i<s7;i++)
    {
      target.tri_color.push_back(source.tri_color[i]);
    }
  //std::cout << "tri_color: " << target.tri_color.size() << std::endl;
  int s8 = source.quad_color.size();
  target.quad_color.reserve(target.quad_color.size()+s8);
  for(int i=0;i<s8;i++)
    {
      target.quad_color.push_back(source.quad_color[i]);
    }
  //std::cout << "quad_color: " << target.quad_color.size() << std::endl;
  int s2d = source.poly_color.size();
  target.poly_color.reserve(target.poly_color.size()+s2d);
  for(int i=0;i<s2d;i++)
    {
      target.poly_color.push_back(source.poly_color[i]);
    }
  //std::cout << "poly_color: " << target.poly_color.size() << std::endl;

  int s9 = source.tri_texcoord.size();
  target.tri_texcoord.reserve(target.tri_texcoord.size()+s9);
  for(int i=0;i<s9;i++)
    {
      target.tri_texcoord.push_back(source.tri_texcoord[i]);
    }
  //std::cout << "tri_texcoord: " << target.tri_texcoord.size() << std::endl;
  int s10 = source.quad_texcoord.size();
  target.quad_texcoord.reserve(target.quad_texcoord.size()+s10);
  for(int i=0;i<s10;i++)
    {
      target.quad_texcoord.push_back(source.quad_texcoord[i]);
    }
  //std::cout << "quad_texcoord: " << target.quad_texcoord.size() << std::endl;
  int s2e = source.poly_texcoord.size();
  target.poly_texcoord.reserve(target.poly_texcoord.size()+s2e);
  for(int i=0;i<s2e;i++)
    {
      target.poly_texcoord.push_back(source.poly_texcoord[i]);
    }
  //std::cout << "poly_texcoord: " << target.poly_texcoord.size() << std::endl;

  append_from_map(source.tri_attribs, target.tri_attribs);
  append_from_map(source.quad_attribs, target.quad_attribs);
  append_from_map(source.poly_attribs, target.poly_attribs);
  append_from_map(source.tri_attribsi, target.tri_attribsi);
  append_from_map(source.quad_attribsi, target.quad_attribsi);
  append_from_map(source.poly_attribsi, target.poly_attribsi);

}

#define ATTRIB_OFFSET(X) ((const GLvoid *)(sizeof(GLfloat) * (X)))
void RenderVertexArray::update(int id)
{
#ifdef VAO
  glBindVertexArray(vao[0]);
#endif

  //std::cout << "Update" << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.tri_count(id)*sizeof(float)*3, s.tri_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.tri_count(id)*sizeof(float)*3, s.tri_normal_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.tri_count(id)*sizeof(float)*4, s.tri_color_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.tri_count(id)*sizeof(float)*3, s.tri_texcoord_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.tri_count(id)*sizeof(float)*3, s.tri_polys2(id));



#ifdef VAO
  glBindVertexArray(vao[1]);
#endif

  glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers2[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_normal_polys(id));

  glBindBuffer(GL_ARRAY_BUFFER, buffers2[2]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*4, s.quad_color_polys(id));

  glBindBuffer(GL_ARRAY_BUFFER, buffers2[3]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_texcoord_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers2[4]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys2(id));


#ifdef VAO
  glBindVertexArray(vao[2]);
#endif

  glBindBuffer(GL_ARRAY_BUFFER, buffers3[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers3[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_normal_polys(id));

  glBindBuffer(GL_ARRAY_BUFFER, buffers3[2]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*4, s.poly_color_polys(id));

  glBindBuffer(GL_ARRAY_BUFFER, buffers3[3]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_texcoord_polys(id));
  glBindBuffer(GL_ARRAY_BUFFER, buffers3[4]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys2(id));


}

void RenderVertexArray::prepare(int id)
{
  s.check_m_set(id);
#ifdef VAO
  glGenVertexArrays(3,vao);
  glBindVertexArray(vao[0]);
#endif
  glGenBuffers(1,&buffers[0]);
  glGenBuffers(1,&buffers[1]);
  glGenBuffers(1,&buffers[2]);
  glGenBuffers(1,&buffers[3]);
  glGenBuffers(1,&buffers[4]);
  glGenBuffers(1, &pos_buffer);
  VertexArraySet::Polys *p = s.m_set[id];
  { // attrib buffers
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attrib_buffer.push_back(val);
    }
  int ss2 = p->quad_attribs.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attrib_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribs.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attrib_buffer3.push_back(val);
    }
  }

  { // attribi buffers
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attribi_buffer.push_back(val);
    }
  int ss2 = p->quad_attribsi.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attribi_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribsi.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      glGenBuffers(1, &val);
      attribi_buffer3.push_back(val);
    }
  }
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float)*3, s.tri_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float)*3, s.tri_normal_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
  glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float)*4, s.tri_color_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
  glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float)*3, s.tri_texcoord_polys(id), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
  glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float)*3, s.tri_polys2(id), GL_STATIC_DRAW);

  {
  // ATTRIB STARTS HERE
  int ss = p->tri_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->tri_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer[i]);
      glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(float), s.tri_attrib_polys(id, (*ii).first), GL_STATIC_DRAW);
      ii++;
    }

  }

  {
  // ATTRIBI start here
  int ss = p->tri_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->tri_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer[i]);
      glBufferData(GL_ARRAY_BUFFER, s.tri_count(id)*sizeof(int), s.tri_attribi_polys(id, (*ii).first), GL_STATIC_DRAW);
      ii++;
    }

  }



  glBindBuffer(GL_ARRAY_BUFFER, 0);

#ifdef VAO
  glBindVertexArray(vao[1]);
#endif
  glGenBuffers(1,&buffers2[0]);
  glGenBuffers(1,&buffers2[1]);
  glGenBuffers(1,&buffers2[2]);
  glGenBuffers(1,&buffers2[3]);
  glGenBuffers(1,&buffers2[4]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
  glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float)*3, s.quad_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers2[1]);
  glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float)*3, s.quad_normal_polys(id), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers2[2]);
  glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float)*4, s.quad_color_polys(id), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers2[3]);
  glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float)*3, s.quad_texcoord_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers2[4]);
  glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float)*3, s.quad_polys2(id), GL_STATIC_DRAW);

  {
  int ss = p->quad_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->quad_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer2[i]);
      glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(float), s.quad_attrib_polys(id, (*ii).first), GL_STATIC_DRAW);
      ii++;
    }
  }
  {
  int ss = p->quad_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->quad_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      //std::cout << "Quad Attribi" << std::endl;
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer2[i]);
      const int *ptr = s.quad_attribi_polys(id, (*ii).first);
      glBufferData(GL_ARRAY_BUFFER, s.quad_count(id)*sizeof(int), ptr, GL_STATIC_DRAW);
      ii++;
    }

  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);



#ifdef VAO
  glBindVertexArray(vao[2]);
#endif
  glGenBuffers(1,&buffers3[0]);
  glGenBuffers(1,&buffers3[1]);
  glGenBuffers(1,&buffers3[2]);
  glGenBuffers(1,&buffers3[3]);
  glGenBuffers(1,&buffers3[4]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers3[0]);
  glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers3[1]);
  glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float)*3, s.poly_normal_polys(id), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers3[2]);
  glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float)*4, s.poly_color_polys(id), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers3[3]);
  glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float)*3, s.poly_texcoord_polys(id), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, buffers3[4]);
  glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys2(id), GL_STATIC_DRAW);

  {
#if 1
  int ss = p->poly_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->poly_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer3[i]);
      glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(float), s.poly_attrib_polys(id, (*ii).first), GL_STATIC_DRAW);
      ii++;
    }
#endif
  }
  {
#if 1 
  int ss = p->poly_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->poly_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer3[i]);
      glBufferData(GL_ARRAY_BUFFER, s.poly_count_f(id)*sizeof(int), s.poly_attribi_polys(id, (*ii).first), GL_STATIC_DRAW);
      ii++;
    }
#endif
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);


#ifdef VAO
    glBindVertexArray(vao[0]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);

#ifdef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

    int counter=6;

    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer[i]);
      glVertexAttribPointer(counter, 1, GL_FLOAT, GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer[i]);
      glVertexAttribIPointer(counter, 1, GL_INT, 0,0);
      counter++;
    }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
#ifdef VAO
    glBindVertexArray(vao[1]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    counter=6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer2[i]);
      glVertexAttribPointer(counter, 1, GL_FLOAT, GL_FALSE, 0,0);
      counter++;			    
    }
    }

    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer2[i]);
      std::cout << "Counter: " << counter << std::endl;
      glVertexAttribIPointer(counter, 1, GL_INT, 0,0);
      counter++;			    
    }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif


#ifdef VAO
    glBindVertexArray(vao[2]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    counter=6;

    {
  int ss = p->poly_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer3[i]);
      glVertexAttribPointer(counter, 1, GL_FLOAT, GL_FALSE, 0,0);
      counter++;			    
    }
    }
    {
  int ss = p->poly_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer3[i]);
      glVertexAttribIPointer(counter, 1, GL_INT, 0,0);
      counter++;			    
    }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif


#ifdef VAO
    glBindVertexArray(0);
#endif
    tri_count = s.tri_count(id);
    quad_count = s.quad_count(id);
    poly_count = s.poly_count_f(id);
}
void RenderVertexArray::del()
{
#ifdef VAO
  glDeleteVertexArrays(3, &vao[0]);
#endif
  std::cout << "Del" << std::endl;
  glDeleteBuffers(1,&buffers[0]);
  glDeleteBuffers(1,&buffers[1]);
  glDeleteBuffers(1,&buffers[2]);
  glDeleteBuffers(1,&buffers[3]);
  glDeleteBuffers(1,&buffers[4]);
  glDeleteBuffers(1,&pos_buffer);

  glDeleteBuffers(1,&buffers2[0]);
  glDeleteBuffers(1,&buffers2[1]);
  glDeleteBuffers(1,&buffers2[2]);
  glDeleteBuffers(1,&buffers2[3]);
  glDeleteBuffers(1,&buffers2[4]);

  glDeleteBuffers(1,&buffers3[0]);
  glDeleteBuffers(1,&buffers3[1]);
  glDeleteBuffers(1,&buffers3[2]);
  glDeleteBuffers(1,&buffers3[3]);
  glDeleteBuffers(1,&buffers3[4]);
}
void RenderVertexArray::prepare_instanced(int id, Point *positions, int size)
{
#ifdef VAO
  glBindVertexArray(vao[0]);
#endif
  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );

  glBufferData( GL_ARRAY_BUFFER, sizeof(Point) * size, positions, GL_DYNAMIC_DRAW);


#ifdef VAO
  glBindVertexArray(vao[1]);
#endif

  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(Point) * size, positions, GL_DYNAMIC_DRAW);

#ifdef VAO
  glBindVertexArray(vao[2]);
#endif

  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(Point) * size, positions, GL_DYNAMIC_DRAW);

}
void RenderVertexArray::render_instanced(int id, Point *positions, int size)
{
#ifdef VAO
  glBindVertexArray(vao[0]);
#endif

  // INSTANCED DRAWING
  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);

  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);

  // END INSTANCED

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);


#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
#endif

    if (tri_count && size) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, tri_count, size);
      //std::cout << "InstancingTRI: " << tri_count << " " << size << std::endl;
    }

#if 1
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
#endif  

#ifdef VAO
  glBindVertexArray(vao[1]);
#endif

  // INSTANCED DRAWING
  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);
  // END INSTANCED

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);


#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
#endif
    if (quad_count && size) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, quad_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
    }
#if 11
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
#endif

#ifdef VAO
  glBindVertexArray(vao[2]);
#endif

  // INSTANCED DRAWING
  glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);
  // END INSTANCED

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);


#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
#endif
    if (poly_count && size) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, poly_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
    }


#ifdef VAO
    glBindVertexArray(0);
#endif


}
void RenderVertexArray::render(int id)
{
  VertexArraySet::Polys *p = s.m_set[id];

#ifdef VAO
  glBindVertexArray(vao[0]);
#endif
    int counter = 6;

#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attrib_buffer[i]);
      glVertexAttribPointer(counter, 1, GL_FLOAT, GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, attribi_buffer[i]);
      glVertexAttribIPointer(counter, 1, GL_INT, 0,0);
      counter++;
    }
    }

#endif


#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
    if (tri_count > 0)
      glDrawArrays(GL_TRIANGLES, 0, tri_count);
#if 1
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glDisableVertexAttribArray(counter);
      counter++;
    }
    }


#endif
#ifdef VAO
  glBindVertexArray(vao[1]);
#endif

#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers2[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
    if (quad_count >0)
      glDrawArrays(GL_TRIANGLES, 0, quad_count);
#if 1
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      glDisableVertexAttribArray(counter);
      counter++;
    }
    }

#endif

#ifdef VAO
  glBindVertexArray(vao[2]);
#endif

#ifndef VAO
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers3[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 1
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
#endif
    if (poly_count > 0)
      glDrawArrays(GL_TRIANGLE_STRIP, 0, poly_count);
#if 1
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
#endif


#ifdef VAO
    glBindVertexArray(0);
#endif


   
  }

void RenderVertexArray2::render(int id, int attr1, int attr2, int attr3, int attr4,
				int aattr1, int aattr2, int aattr3, int aattr4)
{
    // triangles
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableVertexAttribArray(aattr1);
    glEnableVertexAttribArray(aattr2);
    glEnableVertexAttribArray(aattr3);
    glEnableVertexAttribArray(aattr4);

    glEnableVertexAttribArray(attr1);
    glEnableVertexAttribArray(attr2);
    glEnableVertexAttribArray(attr3);
    glEnableVertexAttribArray(attr4);

    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.tri_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.tri_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.tri_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.tri_polys(id));
    glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.tri_normal_polys(id));
    glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.tri_color_polys(id));
    glVertexAttribPointer(aattr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.tri_polys(id));
    glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.tri_normal_polys(id));
    glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.tri_color_polys(id));
    glVertexAttribPointer(attr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.tri_texcoord_polys(id));

    glDrawArrays(GL_TRIANGLES, 0, s1.tri_count(id));
    // quads
    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.quad_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.quad_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.quad_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.quad_polys(id));
    glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.quad_normal_polys(id));
    glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.quad_color_polys(id));
    glVertexAttribPointer(aattr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.quad_polys(id));
    glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.quad_normal_polys(id));
    glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.quad_color_polys(id));
    glVertexAttribPointer(attr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.quad_texcoord_polys(id));

    glDrawArrays(GL_QUADS, 0, s1.quad_count(id));

    // polygons
#if 0
    int ss = s1.poly_count(id);
    for(int i=0;i<ss;i++)
      {
	//glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.poly_polys(id,i));
	//glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.poly_normal_polys(id,i));
	//glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.poly_color_polys(id,i));
	//glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.poly_polys(id,i));
	glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.poly_normal_polys(id,i));
	glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.poly_color_polys(id,i));
	glVertexAttribPointer(aattr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.poly_polys(id,i));
	glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.poly_normal_polys(id,i));
	glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.poly_color_polys(id,i));
	glVertexAttribPointer(attr4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.poly_texcoord_polys(id,i));

	glDrawArrays(GL_POLYGON, 0, s1.poly2_count(id,i));
      }
#endif

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableVertexAttribArray(attr1);
    glDisableVertexAttribArray(attr2);
    glDisableVertexAttribArray(attr3);
    glDisableVertexAttribArray(attr4);

    glDisableVertexAttribArray(aattr1);
    glDisableVertexAttribArray(aattr2);
    glDisableVertexAttribArray(aattr3);
    glDisableVertexAttribArray(aattr4);
  }

void *thread_func(void *data)
{
  ThreadInfo *ti = (ThreadInfo*)data;
  ti->va->copy(ti->start_range, ti->end_range,ti->attrib, ti->attribi);
  return 0; 
}
 
