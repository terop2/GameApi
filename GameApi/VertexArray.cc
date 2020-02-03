  
#include "VertexArray.hh"

//#define NO_SDL_GLEXT
//#include <GL/glew.h>
//#include <SDL_opengl.h>

#include "GameApi_h.hh"
#include "GameApi_low.hh"

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
  //std::cout << "tri_polys: " << p->tri_polys.size() << std::endl;
  //std::cout << "quad_polys: " << p->quad_polys.size() << std::endl;

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
void VertexArraySet::free_reserve(int id)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];  
    }
  p->tri_polys.clear();
  p->quad_polys.clear();
  p->poly_polys.clear();
  p->tri_polys2.clear();
  p->quad_polys2.clear();
  p->poly_polys2.clear();
  p->tri_normals.clear();
  p->quad_normals.clear();
  p->poly_normals.clear();
  p->tri_color.clear();
  p->quad_color.clear();
  p->poly_color.clear();
  p->tri_texcoord.clear();
  p->quad_texcoord.clear();
  p->poly_texcoord.clear();  
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
  if (tri_count)
    p->tri_polys.reserve(tri_count*3/3);  
  if (quad_count)
    p->quad_polys.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_polys.reserve(poly_count*3/3);
  if (tri_count)
    p->tri_polys2.reserve(tri_count*3/3);
  if (quad_count)
    p->quad_polys2.reserve(quad_count*6/4);  
  if (poly_count)
    p->poly_polys2.reserve(poly_count*3/3);
  if (tri_count)
    p->tri_normals.reserve(tri_count*3/3);  
  if (quad_count)
    p->quad_normals.reserve(quad_count*6/4); 
  if (poly_count)
    p->poly_normals.reserve(poly_count*3/3);
  if (tri_count)
    p->tri_color.reserve(tri_count*3/3); 
  if (quad_count)
    p->quad_color.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_color.reserve(poly_count*3/3);
  if (tri_count)
    p->tri_texcoord.reserve(tri_count*3/3);
  if (quad_count)
    p->quad_texcoord.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_texcoord.reserve(poly_count*3/3);
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
template<class T>
void clone_vector(std::vector<T> &source, std::vector<T> &target)
{
  target.clear();
  int s = source.size();
  for(int i=0;i<s;i++) target.push_back(source[i]);
}
template<class T>
void clone_map(std::map<int, std::vector<T>> &source,
	       std::map<int, std::vector<T>> &target)
{
  target.clear();
  typename std::map<int,std::vector<T>>::iterator i=source.begin();
  for(;i!=source.end();i++)
    {
      target[(*i).first] = (*i).second;
    }
}
void VertexArraySet::clone(int id_source, int id_target)
{
  check_m_set(id_target);
  Polys *source = m_set[id_source];
  Polys *target = m_set[id_target];
  clone_vector(source->tri_polys, target->tri_polys);
  clone_vector(source->quad_polys, target->quad_polys);
  clone_vector(source->poly_polys, target->poly_polys);
  clone_vector(source->tri_polys2, target->tri_polys2);
  clone_vector(source->quad_polys2, target->quad_polys2);
  clone_vector(source->poly_polys2, target->poly_polys2);
  clone_vector(source->tri_normals, target->tri_normals);
  clone_vector(source->quad_normals, target->quad_normals);
  clone_vector(source->poly_normals, target->poly_normals);
  clone_vector(source->tri_color, target->tri_color);
  clone_vector(source->quad_color, target->quad_color);
  clone_vector(source->poly_color, target->poly_color);
  clone_vector(source->tri_texcoord, target->tri_texcoord);
  clone_vector(source->quad_texcoord, target->quad_texcoord);
  clone_vector(source->poly_texcoord, target->poly_texcoord);

  clone_map(source->tri_attribs, target->tri_attribs);
  clone_map(source->quad_attribs, target->quad_attribs);
  clone_map(source->poly_attribs, target->poly_attribs);

  clone_map(source->tri_attribsi, target->tri_attribsi);
  clone_map(source->quad_attribsi, target->quad_attribsi);
  clone_map(source->poly_attribsi, target->poly_attribsi);
}
void VertexArraySet::apply_change(DynamicChange *change, int id_source, int id_target, MainLoopEnv &e)
{
  Polys *source = m_set[id_source];
  Polys *target = m_set[id_target];
  change->applychange(&source->tri_polys[0].x, &target->tri_polys[0].x, source->tri_polys.size()*3, e);
  change->applychange(&source->quad_polys[0].x, &target->quad_polys[0].x, source->quad_polys.size()*3, e);
  change->applychange(&source->poly_polys[0].x, &target->poly_polys[0].x, source->poly_polys.size()*3, e);

  change->applychange(&source->tri_polys2[0].x, &target->tri_polys2[0].x, source->tri_polys2.size()*3, e);
  change->applychange(&source->quad_polys2[0].x, &target->quad_polys2[0].x, source->quad_polys2.size()*3, e);
  change->applychange(&source->poly_polys2[0].x, &target->poly_polys2[0].x, source->poly_polys2.size()*3, e);

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

#define ATTRIB_OFFSET(X) ((const Low_GLvoid *)(sizeof(Low_GLfloat) * (X)))
// buffer_id = 0 triangles
// buffer_id = 1 quads
// buffer_id = 2 polys


void RenderVertexArray::update_tri(int id, int buffer_id, int start, int end)
{
  if (start>=end) return;
  switch(buffer_id) {
  case 0: {
#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[0]);
#endif
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    if (s.tri_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    if (s.tri_normal_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_normal_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    if (s.tri_color_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.tri_color_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    if (s.tri_texcoord_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_texcoord_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    if (s.tri_polys2(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_polys2(id));
  } break;
  case 1: {
#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[1]);
#endif
    
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    if (s.quad_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    if (s.quad_normal_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_normal_polys(id));
    
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    if (s.quad_color_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.quad_color_polys(id));
    
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    if (s.quad_texcoord_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_texcoord_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    if (s.quad_polys2(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_polys2(id));
  } break;
  case 2: {
#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[2]);
#endif
    
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    if (s.poly_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    if (s.poly_normal_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_normal_polys(id));

    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    if (s.poly_color_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.poly_color_polys(id));
    
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    if (s.poly_texcoord_polys(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_texcoord_polys(id));
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    if (s.poly_polys2(id))
      g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_polys2(id));
    

  } break;

}
#ifdef VAO
  g_low->ogl->glBindVertexArray(0);
#endif

}
void RenderVertexArray::update(int id)
{ 
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
 int start=0;
  int end = s.tri_count(id);
  //std::cout << "Update" << std::endl;
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_normal_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, end*sizeof(float)*4, s.tri_color_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_texcoord_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_polys2(id));



#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_normal_polys(id));

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*4, s.quad_color_polys(id));

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_texcoord_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys2(id));


#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_normal_polys(id));

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*4, s.poly_color_polys(id));

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_texcoord_polys(id));
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
  g_low->ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys2(id));

#ifdef VAO
  g_low->ogl->glBindVertexArray(0);
#endif

}
void RenderVertexArray::update_buffers(RenderVertexArray_bufferids ids)
{
  int s = 5;
  for(int i=0;i<s;i++) { buffers[i]=ids.buffers[i]; buffers2[i]=ids.buffers2[i], buffers3[i]=ids.buffers3[i]; }
  int ss = 3;
  for(int i=0;i<ss;i++) { vao[i]=ids.vao[i]; }
  pos_buffer=ids.pos_buffer;
  pos_buffer_matrix = ids.pos_buffer_matrix;
  //int a1 = ids.attrib_buffer.size();
  // todo, attrib & attribi buffers


  int a1 = ids.attrib_buffer.size();
  if (a1>0)
    attrib_buffer.resize(a1);
  for(int i=0;i<a1;i++) { 
    attrib_buffer[i] = ids.attrib_buffer[i];
  }

  int a2 = ids.attrib_buffer2.size();
  if (a2>0)
    attrib_buffer2.resize(a2);
  for(int i=0;i<a2;i++) { 
    attrib_buffer2[i] = ids.attrib_buffer2[i];
  }

  int a3 = ids.attrib_buffer3.size();
  if (a3>0)
    attrib_buffer3.resize(a3);
  for(int i=0;i<a3;i++) { 
    attrib_buffer3[i] = ids.attrib_buffer3[i];
  }


  {
  int a1 = ids.attribi_buffer.size();
  if (a1>0)
    attribi_buffer.resize(a1);
  for(int i=0;i<a1;i++) { 
    attribi_buffer[i] = ids.attribi_buffer[i];
  }

  int a2 = ids.attribi_buffer2.size();
  if (a2>0)
    attribi_buffer2.resize(a2);
  for(int i=0;i<a2;i++) { 
    attribi_buffer2[i] = ids.attribi_buffer2[i];
  }

  int a3 = ids.attribi_buffer3.size();
  if (a3>0)
    attribi_buffer3.resize(a3);
  for(int i=0;i<a3;i++) { 
    attribi_buffer3[i] = ids.attribi_buffer3[i];
  }

  }
}
void RenderVertexArray::fetch_buffers(RenderVertexArray_bufferids &ids)
{
  int s = 5;
  for(int i=0;i<s;i++) { ids.buffers[i]=buffers[i]; ids.buffers2[i]=buffers2[i], ids.buffers3[i]=buffers3[i]; }
  int ss = 3;
  for(int i=0;i<ss;i++) { ids.vao[i]=vao[i]; }
  ids.pos_buffer=pos_buffer;
  ids.pos_buffer_matrix = pos_buffer_matrix;

  int a1 = attrib_buffer.size();
  if (a1>0)
    ids.attrib_buffer.resize(a1);
  for(int i=0;i<a1;i++) { 
    ids.attrib_buffer[i] = attrib_buffer[i];
  }

  int a2 = attrib_buffer2.size();
  if (a2>0)
    ids.attrib_buffer2.resize(a2);
  for(int i=0;i<a2;i++) { 
    ids.attrib_buffer2[i] = attrib_buffer2[i];
  }

  int a3 = attrib_buffer3.size();
  if (a3>0)
    ids.attrib_buffer3.resize(a3);
  for(int i=0;i<a3;i++) { 
    ids.attrib_buffer3[i] = attrib_buffer3[i];
  }


  {
  int a1 = attribi_buffer.size();
  if (a1>0)
    ids.attribi_buffer.resize(a1);
  for(int i=0;i<a1;i++) { 
    ids.attribi_buffer[i] = attribi_buffer[i];
  }

  int a2 = attribi_buffer2.size();
  if (a2>0)
    ids.attribi_buffer2.resize(a2);
  for(int i=0;i<a2;i++) { 
    ids.attribi_buffer2[i] = attribi_buffer2[i];
  }

  int a3 = attribi_buffer3.size();
  if (a3>0)
    ids.attribi_buffer3.resize(a3);
  for(int i=0;i<a3;i++) { 
    ids.attribi_buffer3[i] = attribi_buffer3[i];
  }

  }
}

void RenderVertexArray::prepare(int id, bool isnull, int tri_count_, int quad_count_, int poly_count_)
{
  if (id==-1) { std::cout << "RenderVertexArray::prepare id=-1" << std::endl; id=0; }

  if (tri_count_==-1) tri_count_ = s.tri_count(id);
  if (quad_count_==-1) quad_count_ = s.quad_count(id);
  if (poly_count_==-1) poly_count_ = s.poly_count_f(id);
  tri_count = tri_count_;
  quad_count = quad_count_;
  poly_count = poly_count_;
  
  s.check_m_set(id);
#ifdef VAO
  g_low->ogl->glGenVertexArrays(3,vao);
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
  g_low->ogl->glGenBuffers(1,&buffers[0]);
  g_low->ogl->glGenBuffers(1,&buffers[1]);
  g_low->ogl->glGenBuffers(1,&buffers[2]);
  g_low->ogl->glGenBuffers(1,&buffers[3]);
  g_low->ogl->glGenBuffers(1,&buffers[4]);
  g_low->ogl->glGenBuffers(1, &pos_buffer);
  g_low->ogl->glGenBuffers(1, &pos_buffer_matrix);
  VertexArraySet::Polys *p = s.m_set[id];
  { // attrib buffers
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attrib_buffer.push_back(val);
    }
  int ss2 = p->quad_attribs.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attrib_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribs.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attrib_buffer3.push_back(val);
    }
  }

  { // attribi buffers
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attribi_buffer.push_back(val);
    }
  int ss2 = p->quad_attribsi.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attribi_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribsi.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      g_low->ogl->glGenBuffers(1, &val);
      attribi_buffer3.push_back(val);
    }
  }
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_normal_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*4, isnull?0:s.tri_color_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_texcoord_polys(id), Low_GL_STATIC_DRAW);

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_polys2(id), Low_GL_STATIC_DRAW);

  {
  // ATTRIB STARTS HERE
  int ss = p->tri_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->tri_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float), isnull?0:s.tri_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }

  }

  {
  // ATTRIBI start here
  int ss = p->tri_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->tri_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(int), isnull?0:s.tri_attribi_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }

  }



  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif
  g_low->ogl->glGenBuffers(1,&buffers2[0]);
  g_low->ogl->glGenBuffers(1,&buffers2[1]);
  g_low->ogl->glGenBuffers(1,&buffers2[2]);
  g_low->ogl->glGenBuffers(1,&buffers2[3]);
  g_low->ogl->glGenBuffers(1,&buffers2[4]);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_normal_polys(id), Low_GL_STATIC_DRAW);

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*4, isnull?0:s.quad_color_polys(id), Low_GL_STATIC_DRAW);

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_texcoord_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_polys2(id), Low_GL_STATIC_DRAW);

  {
  int ss = p->quad_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->quad_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer2[i]);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float), isnull?0:s.quad_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }
  }
  {
  int ss = p->quad_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->quad_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      //std::cout << "Quad Attribi" << std::endl;
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer2[i]);
      const int *ptr = isnull?0:s.quad_attribi_polys(id, (*ii).first);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(int), ptr, Low_GL_STATIC_DRAW);
      ii++;
    }

  }

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);



#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif
  g_low->ogl->glGenBuffers(1,&buffers3[0]);
  g_low->ogl->glGenBuffers(1,&buffers3[1]);
  g_low->ogl->glGenBuffers(1,&buffers3[2]);
  g_low->ogl->glGenBuffers(1,&buffers3[3]);
  g_low->ogl->glGenBuffers(1,&buffers3[4]);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_normal_polys(id), Low_GL_STATIC_DRAW);

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*4, isnull?0:s.poly_color_polys(id), Low_GL_STATIC_DRAW);

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_texcoord_polys(id), Low_GL_STATIC_DRAW);
  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
  g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_polys2(id), Low_GL_STATIC_DRAW);

  {
#if 1
  int ss = p->poly_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->poly_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer3[i]);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float), isnull?0:s.poly_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
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
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer3[i]);
      g_low->ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(int), isnull?0:s.poly_attribi_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }
#endif
  }

  g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[0]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);

#ifdef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    int counter=6;

    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      g_low->ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
#ifndef EMSCRIPTEN
      g_low->ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;
    }
    }

    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif
#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[1]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    counter=6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer2[i]);
      g_low->ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;			    
    }
    }

    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer2[i]);
      //std::cout << "Counter: " << counter << std::endl;
#ifndef EMSCRIPTEN
      g_low->ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;			    
    }
    }

    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif


#ifdef VAO
    g_low->ogl->glBindVertexArray(vao[2]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    counter=6;

    {
  int ss = p->poly_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer3[i]);
      g_low->ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;			    
    }
    }
    {
  int ss = p->poly_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer3[i]);
#ifndef EMSCRIPTEN
      g_low->ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;			    
    }
    }

    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif


#ifdef VAO
    g_low->ogl->glBindVertexArray(0);
#endif
    //tri_count = s.tri_count(id);
    //quad_count = s.quad_count(id);
    //poly_count = s.poly_count_f(id);
}
void RenderVertexArray::del()
{
#ifdef VAO
  g_low->ogl->glDeleteVertexArrays(3, &vao[0]);
#endif
  //std::cout << "Del" << std::endl;
  g_low->ogl->glDeleteBuffers(1,&buffers[0]);
  g_low->ogl->glDeleteBuffers(1,&buffers[1]);
  g_low->ogl->glDeleteBuffers(1,&buffers[2]);
  g_low->ogl->glDeleteBuffers(1,&buffers[3]);
  g_low->ogl->glDeleteBuffers(1,&buffers[4]);
  g_low->ogl->glDeleteBuffers(1,&pos_buffer);
  g_low->ogl->glDeleteBuffers(1,&pos_buffer_matrix);

  g_low->ogl->glDeleteBuffers(1,&buffers2[0]);
  g_low->ogl->glDeleteBuffers(1,&buffers2[1]);
  g_low->ogl->glDeleteBuffers(1,&buffers2[2]);
  g_low->ogl->glDeleteBuffers(1,&buffers2[3]);
  g_low->ogl->glDeleteBuffers(1,&buffers2[4]);

  g_low->ogl->glDeleteBuffers(1,&buffers3[0]);
  g_low->ogl->glDeleteBuffers(1,&buffers3[1]);
  g_low->ogl->glDeleteBuffers(1,&buffers3[2]);
  g_low->ogl->glDeleteBuffers(1,&buffers3[3]);
  g_low->ogl->glDeleteBuffers(1,&buffers3[4]);
}
void RenderVertexArray::prepare_instanced_matrix(int id, Matrix *positions, int size)
{
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );

  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);


#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif

  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif

  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  g_low->ogl->glBindVertexArray(0);
#endif


}
void RenderVertexArray::prepare_instanced(int id, Point *positions, int size)
{
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );

  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);


#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif

  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif

  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  g_low->ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  g_low->ogl->glBindVertexArray(0);
#endif


}
std::map<Point*,bool> g_inst_map;
std::map<Matrix*,bool> g_inst_map_matrix;
void RenderVertexArray::render_instanced_matrix(int id, Matrix *positions, int size)
{
  //std::cout << "render_instanced_matrix: " << size << std::endl;

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
    if (tri_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
    g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  }
    
  g_low->ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
  g_low->ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  g_low->ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  g_low->ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  g_low->ogl->glVertexAttribDivisor(7, 1);
  g_low->ogl->glVertexAttribDivisor(8, 1);
  g_low->ogl->glVertexAttribDivisor(9, 1);
  g_low->ogl->glVertexAttribDivisor(10, 1);
  g_low->ogl->glEnableVertexAttribArray(7);
  g_low->ogl->glEnableVertexAttribArray(8);
  g_low->ogl->glEnableVertexAttribArray(9);
  g_low->ogl->glEnableVertexAttribArray(10);

  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif

      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, tri_count, size);
      //std::cout << "InstancingTRI: " << tri_count << " " << size << std::endl;

#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(7);
    g_low->ogl->glDisableVertexAttribArray(8);
    g_low->ogl->glDisableVertexAttribArray(9);
    g_low->ogl->glDisableVertexAttribArray(10);
#endif  
    }

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
  g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  }
				    g_low->ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
				    g_low->ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  g_low->ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  g_low->ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  g_low->ogl->glVertexAttribDivisor(7, 1);
  g_low->ogl->glVertexAttribDivisor(8, 1);
  g_low->ogl->glVertexAttribDivisor(9, 1);
  g_low->ogl->glVertexAttribDivisor(10, 1);
  g_low->ogl->glEnableVertexAttribArray(7);
  g_low->ogl->glEnableVertexAttribArray(8);
  g_low->ogl->glEnableVertexAttribArray(9);
  g_low->ogl->glEnableVertexAttribArray(10);
  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif
      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, quad_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(7);
    g_low->ogl->glDisableVertexAttribArray(8);
    g_low->ogl->glDisableVertexAttribArray(9);
    g_low->ogl->glDisableVertexAttribArray(10);
#endif
    }

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif
    if (poly_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
  g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  }
				    g_low->ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
				    g_low->ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  g_low->ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  g_low->ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  g_low->ogl->glVertexAttribDivisor(7, 1);
  g_low->ogl->glVertexAttribDivisor(8, 1);
  g_low->ogl->glVertexAttribDivisor(9, 1);
  g_low->ogl->glVertexAttribDivisor(10, 1);
  g_low->ogl->glEnableVertexAttribArray(7);
  g_low->ogl->glEnableVertexAttribArray(8);
  g_low->ogl->glEnableVertexAttribArray(9);
  g_low->ogl->glEnableVertexAttribArray(10);
  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif
      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLE_STRIP, 0, poly_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;

#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(7);
    g_low->ogl->glDisableVertexAttribArray(8);
    g_low->ogl->glDisableVertexAttribArray(9);
    g_low->ogl->glDisableVertexAttribArray(10);
#endif
    }

#ifdef VAO
    g_low->ogl->glBindVertexArray(0);
#endif
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
    g_inst_map_matrix[positions]=true;



}
void RenderVertexArray::render_instanced(int id, Point *positions, int size)
{
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
    if (tri_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
    g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  }
    
  g_low->ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  g_low->ogl->glVertexAttribDivisor(5, 1);
  g_low->ogl->glEnableVertexAttribArray(5);

  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif

      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, tri_count, size);
      //std::cout << "InstancingTRI: " << tri_count << " " << size << std::endl;

#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(5);
#endif  
    }

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
  g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  }
  g_low->ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  g_low->ogl->glVertexAttribDivisor(5, 1);
  g_low->ogl->glEnableVertexAttribArray(5);
  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif
      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, quad_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(5);
#endif
    }

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif
    if (poly_count && size) {

  // INSTANCED DRAWING
  g_low->ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
  g_low->ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  }
  g_low->ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  g_low->ogl->glVertexAttribDivisor(5, 1);
  g_low->ogl->glEnableVertexAttribArray(5);
  // END INSTANCED

  g_low->ogl->glVertexAttribDivisor(0, 0);
  g_low->ogl->glVertexAttribDivisor(1, 0);
  g_low->ogl->glVertexAttribDivisor(2, 0);
  g_low->ogl->glVertexAttribDivisor(3, 0);
  g_low->ogl->glVertexAttribDivisor(4, 0);


#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif
      g_low->ogl->glDrawArraysInstanced(Low_GL_TRIANGLE_STRIP, 0, poly_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;

#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
    g_low->ogl->glDisableVertexAttribArray(5);
#endif
    }

#ifdef VAO
    g_low->ogl->glBindVertexArray(0);
#endif
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
    g_inst_map[positions]=true;


}

void RenderVertexArray::render(int id)
{
  VertexArraySet::Polys *p = s.m_set[id];

#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[0]);
#endif
    int counter = 6;
    if (tri_count > 0)
      {
#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      g_low->ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
#ifndef EMSCRIPTEN
      g_low->ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;
    }
    }

#endif


#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);

    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      g_low->ogl->glDrawArrays(Low_GL_TRIANGLES, 0, tri_count);
#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }


#endif
      }
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count >0)
      {
#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      g_low->ogl->glDrawArrays(Low_GL_TRIANGLES, 0, quad_count);
#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);


    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      g_low->ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      }
#ifdef VAO
  g_low->ogl->glBindVertexArray(vao[2]);
#endif
  if (poly_count > 0) {

#ifndef VAO
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    g_low->ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    g_low->ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    g_low->ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    g_low->ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    g_low->ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    g_low->ogl->glEnableVertexAttribArray(0);
    g_low->ogl->glEnableVertexAttribArray(1);
    g_low->ogl->glEnableVertexAttribArray(2);
    g_low->ogl->glEnableVertexAttribArray(3);
    g_low->ogl->glEnableVertexAttribArray(4);
#endif
      g_low->ogl->glDrawArrays(Low_GL_TRIANGLE_STRIP, 0, poly_count);
#if 1
    g_low->ogl->glDisableVertexAttribArray(0);
    g_low->ogl->glDisableVertexAttribArray(1);
    g_low->ogl->glDisableVertexAttribArray(2);
    g_low->ogl->glDisableVertexAttribArray(3);
    g_low->ogl->glDisableVertexAttribArray(4);
#endif
  }

#ifdef VAO
    g_low->ogl->glBindVertexArray(0);
#endif
    g_low->ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


   
  }

void RenderVertexArray2::render(int id, int attr1, int attr2, int attr3, int attr4,
				int aattr1, int aattr2, int aattr3, int aattr4)
{
    // triangles
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    g_low->ogl->glEnableVertexAttribArray(aattr1);
    g_low->ogl->glEnableVertexAttribArray(aattr2);
    g_low->ogl->glEnableVertexAttribArray(aattr3);
    g_low->ogl->glEnableVertexAttribArray(aattr4);

    g_low->ogl->glEnableVertexAttribArray(attr1);
    g_low->ogl->glEnableVertexAttribArray(attr2);
    g_low->ogl->glEnableVertexAttribArray(attr3);
    g_low->ogl->glEnableVertexAttribArray(attr4);

    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.tri_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.tri_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.tri_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    g_low->ogl->glVertexAttribPointer(aattr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_normal_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_color_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_texcoord_polys(id));

    g_low->ogl->glVertexAttribPointer(attr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_polys(id));
    g_low->ogl->glVertexAttribPointer(attr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_normal_polys(id));
    g_low->ogl->glVertexAttribPointer(attr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_color_polys(id));
    g_low->ogl->glVertexAttribPointer(attr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_texcoord_polys(id));

    g_low->ogl->glDrawArrays(Low_GL_TRIANGLES, 0, s1.tri_count(id));
    // quads
    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.quad_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.quad_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.quad_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    g_low->ogl->glVertexAttribPointer(aattr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_normal_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_color_polys(id));
    g_low->ogl->glVertexAttribPointer(aattr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_texcoord_polys(id));

    g_low->ogl->glVertexAttribPointer(attr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_polys(id));
    g_low->ogl->glVertexAttribPointer(attr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_normal_polys(id));
    g_low->ogl->glVertexAttribPointer(attr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_color_polys(id));
    g_low->ogl->glVertexAttribPointer(attr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_texcoord_polys(id));

    g_low->ogl->glDrawArrays(Low_GL_QUADS, 0, s1.quad_count(id));

    // polygons
#if 0
    int ss = s1.poly_count(id);
    for(int i=0;i<ss;i++)
      {
	//glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.poly_polys(id,i));
	//glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.poly_normal_polys(id,i));
	//glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.poly_color_polys(id,i));
	//glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(aattr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.poly_polys(id,i));
	glVertexAttribPointer(aattr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.poly_normal_polys(id,i));
	glVertexAttribPointer(aattr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s1.poly_color_polys(id,i));
	glVertexAttribPointer(aattr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(attr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.poly_polys(id,i));
	glVertexAttribPointer(attr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.poly_normal_polys(id,i));
	glVertexAttribPointer(attr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s2.poly_color_polys(id,i));
	glVertexAttribPointer(attr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s2.poly_texcoord_polys(id,i));

	glDrawArrays(GL_POLYGON, 0, s1.poly2_count(id,i));
      }
#endif

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    g_low->ogl->glDisableVertexAttribArray(attr1);
    g_low->ogl->glDisableVertexAttribArray(attr2);
    g_low->ogl->glDisableVertexAttribArray(attr3);
    g_low->ogl->glDisableVertexAttribArray(attr4);

    g_low->ogl->glDisableVertexAttribArray(aattr1);
    g_low->ogl->glDisableVertexAttribArray(aattr2);
    g_low->ogl->glDisableVertexAttribArray(aattr3);
    g_low->ogl->glDisableVertexAttribArray(aattr4);
  }
Counts CalcCounts(FaceCollection *coll, int start, int end);
Counts CalcOffsets(FaceCollection *coll, int start);
void ProgressBar(int val, int max);
volatile bool g_lock1=false, g_lock2=false, g_lock3=false;

#ifndef ARM
ThreadInfo volatile *ti_global;
volatile int thread_counter=0;


void *thread_func(void *data)
{
#ifndef BATCHING
  ThreadInfo *ti = (ThreadInfo*)data;
  ti->va->copy(ti->start_range, ti->end_range,ti->attrib, ti->attribi);
  return 0;
#else
  ThreadInfo *ti = (ThreadInfo*)data;
  int s = 1;
  int delta_s = (ti->end_range - ti->start_range)/s + 1;
  for(int i=0;i<s;i++)
    {
      //ProgressBar(i,s);
      int start_range = ti->start_range + i*delta_s;
      int end_range = ti->start_range + (i+1)*delta_s;
      if (start_range>ti->end_range) start_range=ti->end_range;
      if (end_range>ti->end_range) end_range = ti->end_range;
      if (i==s-1) end_range = ti->end_range;
      Counts ct2_counts = CalcCounts(ti->faces, start_range, end_range);
      Counts ct2_offsets = CalcOffsets(ti->faces, start_range);
      ti->va->copy(start_range, end_range,ti->attrib, ti->attribi);
      ti->ct2_counts = ct2_counts;
      ti->ct2_offsets = ct2_offsets;
      //std::cout << "wait 1" << std::endl;
      pthread_mutex_lock(ti->mutex1); // LOCK mutex1
      //std::cout << "wait 1 end" << std::endl;
      //std::cout << "Lock1 wait" << std::endl;
      //while(g_lock1==true);
      //g_lock1 = true;
      //std::cout << "Lock1 wait end" << std::endl;
      // set data
      ti_global = ti;
      if (i==s-1) thread_counter++;
      //std::cout << "lock 3 release" << std::endl;
      g_lock3 = false;
      //pthread_mutex_unlock(ti->mutex3); // unlock mutex3
      //std::cout << "Lock2 wait" << std::endl;
      //std::cout << "wait 2" << std::endl;
      //std::cout << "lock 2 wait" << std::endl;
     while(g_lock2==true);
      g_lock2 = true;
      //std::cout << "lock 2 wait end" << std::endl;
      //std::cout << "wait 2 end" << std::endl;
      //std::cout << "Lock2 wait end" << std::endl;
      //pthread_mutex_lock(ti->mutex2); // WAIT FOR mutex2 to open
      g_lock1 = false;
      //std::cout << "scope 1 rel" << std::endl;
      pthread_mutex_unlock(ti->mutex1); // release scope mutex1 
      ti->set->free_reserve(0);
    }
 #endif
  return 0;
}
#endif

Counts CalcCounts(FaceCollection *coll, int start, int end)
{
  Counts c;
  c.tri_count = 0;
  c.quad_count = 0;
  c.poly_count = 0;
  if (!coll) return c;
  int s = coll->NumFaces();
  if (start<0) start=0;
  if (end>s) end=s;
  for(int i=start;i<end;i++)
    {
      int p = coll->NumPoints(i);
      if (p==3) c.tri_count++;
      else if (p==4) c.quad_count++;
      else c.poly_count+=p/2*2+2;
    }
  return c;
}

Counts CalcOffsets(FaceCollection *coll, int start)
{
  Counts c;
  c.tri_count = 0;
  c.quad_count = 0;
  c.poly_count = 0;
  if (!coll) return c;
  int s = coll->NumFaces();
  if (start<0) start=0;
  if (start>s) start=s;
  for(int i=0;i<start;i++)
    {
      int p = coll->NumPoints(i);
      if (p==3) c.tri_count++;
      else if (p==4) c.quad_count++;
      else c.poly_count+=p/2*2+2;
    }
  return c;
}

void FaceCollectionVertexArray2::copy(int start_range, int end_range, std::vector<int> attribs, std::vector<int> attribsi)
  {
    //std::cout << "Copy: " << start_range << " " << end_range << std::endl;
    //int ss = coll.NumFaces();
    //std::cout << "NumFaces: " << ss << std::endl;
    for(int i=start_range;i<end_range;i++)
      {
	//std::cout << "F" << i << std::endl;
	//if ((i-start_range)%1000==0) { std::cout << "Face: " << i << std::endl; }
	int w = coll.NumPoints(i);
	//std::cout << i << " " << w << std::endl;
	for(int j=0;j<w;j++)
	  {
	    //std::cout << "FP!"  << i << std::endl;
	    p[j] = coll.FacePoint(i,j);
	    //std::cout << "EFP!" << i << std::endl;
	    p2[j] = coll.EndFacePoint(i,j);
	    //std::cout << "PN!" << i << std::endl;
	    v[j] = coll.PointNormal(i,j);

#if 0
	    for (int k=0;k<(int)attribs.size();k++)
	      {
		a[k][j] = coll.Attrib(i,j,attribs[k]);
	      }

	    for (int k=0;k<(int)attribsi.size();k++)
	      {
		ai[k][j] = coll.AttribI(i,j,attribsi[k]);
	      }
#endif
	    //std::cout << "COL!" << i << std::endl;
	    c[j] = coll.Color(i,j);
	    //std::cout << "Tex!" << i << std::endl;
	    Point2d p = coll.TexCoord(i,j);
	    float p2 = coll.TexCoord3(i,j);
	    tex[j] = Point(p.x,p.y,p2);
	    //std::cout << "VA: " << tex[j] << std::endl;
	  }
	//std::cout << "push_p!" << i << std::endl;
	s.push_poly(0, w, &p[0]);
	//  std::cout << "push_p2!" << i << std::endl;
	s.push_poly2(0, w, &p2[0]);
	//  std::cout << "push_n!" << i << std::endl
	s.push_normal(0, w, &v[0]);

#if 0
	for (int k=0;k<(int)attribs.size();k++)
	  s.push_attrib(0, attribs[k], w, &a[k][0]);
	for (int k=0;k<(int)attribsi.size();k++)
	  s.push_attribi(0, attribsi[k], w, &ai[k][0]);
#endif
	//  std::cout << "push_c!" << std::endl;
	s.push_color(0, w, &c[0]);
	//  std::cout << "push_texcoord!" << std::endl;
	s.push_texcoord(0, w, &tex[0]);

      }
    //std::cout << "Copy returns" << std::endl;
  }
