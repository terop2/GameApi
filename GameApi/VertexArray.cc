  
#include "VertexArray.hh"

//#define NO_SDL_GLEXT
//#include <GL/glew.h>
//#include <SDL_opengl.h>

#include "GameApi_h.hh"
#include "GameApi_low.hh"
#include <atomic>

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
void VertexArraySet::clear_poly_and_poly2(int id)
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
}
void VertexArraySet::free_reserve(int id)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];  
    }
  p->tri_polys.resize(0);
  p->quad_polys.resize(0);
  p->poly_polys.resize(0);
  p->tri_polys2.resize(0);
  p->quad_polys2.resize(0);
  p->poly_polys2.resize(0);
  p->tri_normals.resize(0);
  p->quad_normals.resize(0);
  p->poly_normals.resize(0);
  p->tri_color.resize(0);
  p->quad_color.resize(0);
  p->poly_color.resize(0);
  p->tri_texcoord.resize(0);
  p->quad_texcoord.resize(0);
  p->poly_texcoord.resize(0);
  p->tri_joint.resize(0);
  p->quad_joint.resize(0);
  p->poly_joint.resize(0);
  p->tri_weight.resize(0);
  p->quad_weight.resize(0);
  p->poly_weight.resize(0);
}
void stackTrace();
void VertexArraySet::set_reserve(int id, int tri_count, int quad_count, int poly_count)
{
  if (tri_count<1) { tri_count=0; }
  if (quad_count<1) { quad_count=0; }
  if (poly_count<1) { poly_count=0; }

  // if (tri_count==58630) {
  //  stackTrace();
  //  std::cout << "BLOB" << std::endl;
  //}
  
  //std::cout << "set_reserve: " << tri_count << " " << quad_count << " " << poly_count << std::endl;
  
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
    p->tri_color.reserve(tri_count*3/3*4); 
  if (quad_count)
    p->quad_color.reserve(quad_count*6/4*4);
  if (poly_count)
    p->poly_color.reserve(poly_count*3/3*4);
  if (tri_count)
    p->tri_texcoord.reserve(tri_count*3/3);
  if (quad_count)
    p->quad_texcoord.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_texcoord.reserve(poly_count*3/3);
  if (tri_count)
    p->tri_joint.reserve(tri_count*3/3);
  if (quad_count)
    p->quad_joint.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_joint.reserve(poly_count*3/3);

  if (tri_count)
    p->tri_weight.reserve(tri_count*3/3);
  if (quad_count)
    p->quad_weight.reserve(quad_count*6/4);
  if (poly_count)
    p->poly_weight.reserve(poly_count*3/3);

}
void VertexArraySet::push_poly(int id, int num, Point *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num < 3) return;
  else
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
      if (p->poly_polys.size()!=0)
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
  if (num<3) return;
  else
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
      if (p->poly_polys2.size()!=0)
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
  if (num<3)
    return;
  else
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
  if (num<3) return;
  else
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
  if (num<3) return;
  else
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
  if (num<3) return;
  else
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



void VertexArraySet::push_joint(int id, int num, VEC4 *points)
{

  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num<3) return;
  else
  if (num == 3)
    {
      p->tri_joint.push_back(points[0]);
      p->tri_joint.push_back(points[1]);
      p->tri_joint.push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << "Vertex: " << points[0] << " " << points[1] << " " << points[2] << " " << points[3] << std::endl;
      p->quad_joint.push_back(points[0]);
      p->quad_joint.push_back(points[1]);
      p->quad_joint.push_back(points[2]);
      p->quad_joint.push_back(points[0]);
      p->quad_joint.push_back(points[2]);
      p->quad_joint.push_back(points[3]);
    }
  else
    {
      if (p->poly_joint.size()!=0)
	{
	  p->poly_joint.push_back(points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_joint.push_back(points[i]);
	  p->poly_joint.push_back(points[j]);
	}
      p->poly_joint.push_back(p->poly_joint[p->poly_joint.size()-1]);

    }

}

void VertexArraySet::push_weight(int id, int num, VEC4 *points)
{

  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num<3) return;
  else
  if (num == 3)
    {
      p->tri_weight.push_back(points[0]);
      p->tri_weight.push_back(points[1]);
      p->tri_weight.push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << "Vertex: " << points[0] << " " << points[1] << " " << points[2] << " " << points[3] << std::endl;
      p->quad_weight.push_back(points[0]);
      p->quad_weight.push_back(points[1]);
      p->quad_weight.push_back(points[2]);
      p->quad_weight.push_back(points[0]);
      p->quad_weight.push_back(points[2]);
      p->quad_weight.push_back(points[3]);
    }
  else
    {
      if (p->poly_weight.size()!=0)
	{
	  p->poly_weight.push_back(points[0]);
	}

      int s = num;
      int j = s-1;
      for(int i=0;i<num/2;i++,j--)
	{
	  p->poly_weight.push_back(points[i]);
	  p->poly_weight.push_back(points[j]);
	}
      p->poly_weight.push_back(p->poly_weight[p->poly_weight.size()-1]);

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
  if (num<3) return;
  else
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
  clone_vector(source->tri_joint, target->tri_joint);
  clone_vector(source->quad_joint, target->quad_joint);
  clone_vector(source->poly_joint, target->poly_joint);

  clone_vector(source->tri_weight, target->tri_weight);
  clone_vector(source->quad_weight, target->quad_weight);
  clone_vector(source->poly_weight, target->poly_weight);

  
  
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

      poly->tri_joint.resize(0);
      poly->quad_joint.resize(0);
      poly->poly_joint.resize(0);
      poly->tri_weight.resize(0);
      poly->quad_weight.resize(0);
      poly->poly_weight.resize(0);
      
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

      poly->tri_joint.shrink_to_fit();
      poly->quad_joint.shrink_to_fit();
      poly->poly_joint.shrink_to_fit();
      poly->tri_weight.shrink_to_fit();
      poly->quad_weight.shrink_to_fit();
      poly->poly_weight.shrink_to_fit();
      

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



  int js7 = source.tri_joint.size();
  target.tri_joint.reserve(target.tri_joint.size()+js7);
  for(int i=0;i<js7;i++)
    {
      target.tri_joint.push_back(source.tri_joint[i]);
    }
  //std::cout << "tri_color: " << target.tri_color.size() << std::endl;
  int js8 = source.quad_joint.size();
  target.quad_joint.reserve(target.quad_joint.size()+js8);
  for(int i=0;i<js8;i++)
    {
      target.quad_joint.push_back(source.quad_joint[i]);
    }
  //std::cout << "quad_color: " << target.quad_color.size() << std::endl;
  int js2d = source.poly_joint.size();
  target.poly_joint.reserve(target.poly_joint.size()+js2d);
  for(int i=0;i<js2d;i++)
    {
      target.poly_joint.push_back(source.poly_joint[i]);
    }
  //std::cout << "poly_color: " << target.poly_color.size() << std::endl;


  int ws7 = source.tri_weight.size();
  target.tri_weight.reserve(target.tri_weight.size()+ws7);
  for(int i=0;i<ws7;i++)
    {
      target.tri_weight.push_back(source.tri_weight[i]);
    }
  //std::cout << "tri_color: " << target.tri_color.size() << std::endl;
  int ws8 = source.quad_weight.size();
  target.quad_weight.reserve(target.quad_weight.size()+ws8);
  for(int i=0;i<ws8;i++)
    {
      target.quad_weight.push_back(source.quad_weight[i]);
    }
  //std::cout << "quad_color: " << target.quad_color.size() << std::endl;
  int ws2d = source.poly_weight.size();
  target.poly_weight.reserve(target.poly_weight.size()+ws2d);
  for(int i=0;i<ws2d;i++)
    {
      target.poly_weight.push_back(source.poly_weight[i]);
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


GameApi::PinIn RenderVertexArray::update_tri(int id, int buffer_id, int start, int end)
{
  if (start>=end) return GameApi::PinIn();
  OpenglLowApi *ogl = g_low->ogl;

  switch(buffer_id) {
  case 0: {
#ifdef VAO
    ogl->glBindVertexArray(vao[0]);
#endif
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    if (s.tri_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    if (s.tri_normal_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_normal_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    if (s.tri_color_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.tri_color_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    if (s.tri_texcoord_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_texcoord_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    if (s.tri_polys2(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.tri_polys2(id));

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
    if (s.tri_joint_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.tri_joint_polys(id));

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
    if (s.tri_weight_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.tri_weight_polys(id));

    

  } break;
  case 1: {
#ifdef VAO
    ogl->glBindVertexArray(vao[1]);
#endif
    
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    if (s.quad_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    if (s.quad_normal_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_normal_polys(id));
    
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    if (s.quad_color_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.quad_color_polys(id));
    
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    if (s.quad_texcoord_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_texcoord_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    if (s.quad_polys2(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.quad_polys2(id));


    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
    if (s.quad_joint_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.quad_joint_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
    if (s.quad_weight_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.quad_weight_polys(id));

    
  } break;
  case 2: {
#ifdef VAO
    ogl->glBindVertexArray(vao[2]);
#endif
    
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    if (s.poly_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    if (s.poly_normal_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_normal_polys(id));

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    if (s.poly_color_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.poly_color_polys(id));
    
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    if (s.poly_texcoord_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_texcoord_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    if (s.poly_polys2(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, (end-start)*sizeof(float)*3, s.poly_polys2(id));


    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
    if (s.poly_joint_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.poly_joint_polys(id));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
    if (s.poly_weight_polys(id))
      ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, (end-start)*sizeof(float)*4, s.poly_weight_polys(id));


    
    

  } break;

}
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER,0);
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  return GameApi::PinIn();
}
Matrix g_in_MV;
std::vector<Point> *g_ptr;
std::vector<Vector> *g_ptr_normal;
std::vector<float> *g_ptr_color;
std::vector<Point> *g_ptr_texcoord;
std::vector<VEC4> *g_ptr_joint;
std::vector<VEC4> *g_ptr_weight;
int g_count;
bool sort_compare_tri_polys(const Point *p1, const Point *p2)
{
  if (g_count==3) {
    Point pp1_1 = p1[0];
    Point pp1_2 = p1[1];
    Point pp1_3 = p1[2];
    Point pp2_1 = p2[0];
    Point pp2_2 = p2[1];
    Point pp2_3 = p2[2];
    Point p1_center = (Vector(pp1_1) + Vector(pp1_2) + Vector(pp1_3))/3.0;
    Point p2_center = (Vector(pp2_1) + Vector(pp2_2) + Vector(pp2_3))/3.0;
    Point p1_trans = p1_center * g_in_MV;
    Point p2_trans = p2_center * g_in_MV;
    return p1_trans.z>p2_trans.z;
  } else if (g_count==6) {
    Point pp1_1 = p1[0];
    Point pp1_2 = p1[1];
    Point pp1_3 = p1[2];
    Point pp1_4 = p1[5];
    Point pp2_1 = p2[0];
    Point pp2_2 = p2[1];
    Point pp2_3 = p2[2];
    Point pp2_4 = p2[5];
    Point p1_center = (Vector(pp1_1) + Vector(pp1_2) + Vector(pp1_3) + Vector(pp1_4))/4.0;
    Point p2_center = (Vector(pp2_1) + Vector(pp2_2) + Vector(pp2_3) + Vector(pp2_4))/4.0;
    Point p1_trans = p1_center * g_in_MV;
    Point p2_trans = p2_center * g_in_MV;
    return p1_trans.z>p2_trans.z;
  }
  return false;
}
void swap_elem(int i1, int i2)
{
  if (g_count==3) {
	  g_ptr->operator[](i1*g_count+0) = g_ptr->operator[](i2*g_count+0);
	  g_ptr->operator[](i1*g_count+1) = g_ptr->operator[](i2*g_count+1);
	  g_ptr->operator[](i1*g_count+2) = g_ptr->operator[](i2*g_count+2);

	  if (g_ptr_normal) {
	  g_ptr_normal->operator[](i1*g_count+0) = g_ptr_normal->operator[](i2*g_count+0);
	  g_ptr_normal->operator[](i1*g_count+1) = g_ptr_normal->operator[](i2*g_count+1);
	  g_ptr_normal->operator[](i1*g_count+2) = g_ptr_normal->operator[](i2*g_count+2);
	  }

	  if (g_ptr_color) {
	  g_ptr_color->operator[](i1*g_count*4+0) = g_ptr_color->operator[](i2*g_count*4+0);
	  g_ptr_color->operator[](i1*g_count*4+1) = g_ptr_color->operator[](i2*g_count*4+1);
	  g_ptr_color->operator[](i1*g_count*4+2) = g_ptr_color->operator[](i2*g_count*4+2);

	  g_ptr_color->operator[](i1*g_count*4+3) = g_ptr_color->operator[](i2*g_count*4+3);
	  g_ptr_color->operator[](i1*g_count*4+4) = g_ptr_color->operator[](i2*g_count*4+4);
	  g_ptr_color->operator[](i1*g_count*4+5) = g_ptr_color->operator[](i2*g_count*4+5);

	  g_ptr_color->operator[](i1*g_count*4+6) = g_ptr_color->operator[](i2*g_count*4+6
									    );
	  g_ptr_color->operator[](i1*g_count*4+7) = g_ptr_color->operator[](i2*g_count*4+7);
	  g_ptr_color->operator[](i1*g_count*4+8) = g_ptr_color->operator[](i2*g_count*4+8);

	  g_ptr_color->operator[](i1*g_count*4+9) = g_ptr_color->operator[](i2*g_count*4+0);
	  g_ptr_color->operator[](i1*g_count*4+10) = g_ptr_color->operator[](i2*g_count*4+10);
	  g_ptr_color->operator[](i1*g_count*4+11) = g_ptr_color->operator[](i2*g_count*4+11);

	  
	  }
	  if (g_ptr_texcoord) {
	  g_ptr_texcoord->operator[](i1*g_count+0) = g_ptr_texcoord->operator[](i2*g_count+0);
	  g_ptr_texcoord->operator[](i1*g_count+1) = g_ptr_texcoord->operator[](i2*g_count+1);
	  g_ptr_texcoord->operator[](i1*g_count+2) = g_ptr_texcoord->operator[](i2*g_count+2);
	  }

	  if (g_ptr_joint) {
	  g_ptr_joint->operator[](i1*g_count+0) = g_ptr_joint->operator[](i2*g_count+0);
	  g_ptr_joint->operator[](i1*g_count+1) = g_ptr_joint->operator[](i2*g_count+1);
	  g_ptr_joint->operator[](i1*g_count+2) = g_ptr_joint->operator[](i2*g_count+2);
	  }

	  if (g_ptr_weight) {
	  
	  g_ptr_weight->operator[](i1*g_count+0) = g_ptr_weight->operator[](i2*g_count+0);
	  g_ptr_weight->operator[](i1*g_count+1) = g_ptr_weight->operator[](i2*g_count+1);
	  g_ptr_weight->operator[](i1*g_count+2) = g_ptr_weight->operator[](i2*g_count+2);
	  }

	  
  } else if (g_count==6)
    {
	  g_ptr->operator[](i1*g_count+0) = g_ptr->operator[](i2*g_count+0);
	  g_ptr->operator[](i1*g_count+1) = g_ptr->operator[](i2*g_count+1);
	  g_ptr->operator[](i1*g_count+2) = g_ptr->operator[](i2*g_count+2);
	  g_ptr->operator[](i1*g_count+3) = g_ptr->operator[](i2*g_count+3);
	  g_ptr->operator[](i1*g_count+4) = g_ptr->operator[](i2*g_count+4);
	  g_ptr->operator[](i1*g_count+5) = g_ptr->operator[](i2*g_count+5);

	  if (g_ptr_normal) {
	  g_ptr_normal->operator[](i1*g_count+0) = g_ptr_normal->operator[](i2*g_count+0);
	  g_ptr_normal->operator[](i1*g_count+1) = g_ptr_normal->operator[](i2*g_count+1);
	  g_ptr_normal->operator[](i1*g_count+2) = g_ptr_normal->operator[](i2*g_count+2);
	  g_ptr_normal->operator[](i1*g_count+3) = g_ptr_normal->operator[](i2*g_count+3);
	  g_ptr_normal->operator[](i1*g_count+4) = g_ptr_normal->operator[](i2*g_count+4);
	  g_ptr_normal->operator[](i1*g_count+5) = g_ptr_normal->operator[](i2*g_count+5);
	  }
	  
	  if (g_ptr_color) {
	  g_ptr_color->operator[](i1*g_count*4+0) = g_ptr_color->operator[](i2*g_count*4+0);
	  g_ptr_color->operator[](i1*g_count*4+1) = g_ptr_color->operator[](i2*g_count*4+1);
	  g_ptr_color->operator[](i1*g_count*4+2) = g_ptr_color->operator[](i2*g_count*4+2);
	  g_ptr_color->operator[](i1*g_count*4+3) = g_ptr_color->operator[](i2*g_count*4+3);
	  g_ptr_color->operator[](i1*g_count*4+4) = g_ptr_color->operator[](i2*g_count*4+4);
	  g_ptr_color->operator[](i1*g_count*4+5) = g_ptr_color->operator[](i2*g_count*4+5);


	  g_ptr_color->operator[](i1*g_count*4+6) = g_ptr_color->operator[](i2*g_count*4+6);
	  g_ptr_color->operator[](i1*g_count*4+7) = g_ptr_color->operator[](i2*g_count*4+7);
	  g_ptr_color->operator[](i1*g_count*4+8) = g_ptr_color->operator[](i2*g_count*4+8);
	  g_ptr_color->operator[](i1*g_count*4+9) = g_ptr_color->operator[](i2*g_count*4+9);
	  g_ptr_color->operator[](i1*g_count*4+10) = g_ptr_color->operator[](i2*g_count*4+10);
	  g_ptr_color->operator[](i1*g_count*4+11) = g_ptr_color->operator[](i2*g_count*4+11);


	  g_ptr_color->operator[](i1*g_count*4+12) = g_ptr_color->operator[](i2*g_count*4+12);
	  g_ptr_color->operator[](i1*g_count*4+13) = g_ptr_color->operator[](i2*g_count*4+13);
	  g_ptr_color->operator[](i1*g_count*4+14) = g_ptr_color->operator[](i2*g_count*4+14);
	  g_ptr_color->operator[](i1*g_count*4+15) = g_ptr_color->operator[](i2*g_count*4+15);
	  g_ptr_color->operator[](i1*g_count*4+16) = g_ptr_color->operator[](i2*g_count*4+16);
	  g_ptr_color->operator[](i1*g_count*4+17) = g_ptr_color->operator[](i2*g_count*4+17);


	  g_ptr_color->operator[](i1*g_count*4+18) = g_ptr_color->operator[](i2*g_count*4+18);
	  g_ptr_color->operator[](i1*g_count*4+19) = g_ptr_color->operator[](i2*g_count*4+19);
	  g_ptr_color->operator[](i1*g_count*4+20) = g_ptr_color->operator[](i2*g_count*4+20);
	  g_ptr_color->operator[](i1*g_count*4+21) = g_ptr_color->operator[](i2*g_count*4+21);
	  g_ptr_color->operator[](i1*g_count*4+22) = g_ptr_color->operator[](i2*g_count*4+22);
	  g_ptr_color->operator[](i1*g_count*4+23) = g_ptr_color->operator[](i2*g_count*4+23);

	  
	  
	  }
	  if (g_ptr_texcoord) {
	  g_ptr_texcoord->operator[](i1*g_count+0) = g_ptr_texcoord->operator[](i2*g_count+0);
	  g_ptr_texcoord->operator[](i1*g_count+1) = g_ptr_texcoord->operator[](i2*g_count+1);
	  g_ptr_texcoord->operator[](i1*g_count+2) = g_ptr_texcoord->operator[](i2*g_count+2);
	  g_ptr_texcoord->operator[](i1*g_count+3) = g_ptr_texcoord->operator[](i2*g_count+3);
	  g_ptr_texcoord->operator[](i1*g_count+4) = g_ptr_texcoord->operator[](i2*g_count+4);
	  g_ptr_texcoord->operator[](i1*g_count+5) = g_ptr_texcoord->operator[](i2*g_count+5);
	  }
	  if (g_ptr_joint) {
	  g_ptr_joint->operator[](i1*g_count+0) = g_ptr_joint->operator[](i2*g_count+0);
	  g_ptr_joint->operator[](i1*g_count+1) = g_ptr_joint->operator[](i2*g_count+1);
	  g_ptr_joint->operator[](i1*g_count+2) = g_ptr_joint->operator[](i2*g_count+2);
	  g_ptr_joint->operator[](i1*g_count+3) = g_ptr_joint->operator[](i2*g_count+3);
	  g_ptr_joint->operator[](i1*g_count+4) = g_ptr_joint->operator[](i2*g_count+4);
	  g_ptr_joint->operator[](i1*g_count+5) = g_ptr_joint->operator[](i2*g_count+5);	  
	  }

	  if (g_ptr_weight) {
	  g_ptr_weight->operator[](i1*g_count+0) = g_ptr_weight->operator[](i2*g_count+0);
	  g_ptr_weight->operator[](i1*g_count+1) = g_ptr_weight->operator[](i2*g_count+1);
	  g_ptr_weight->operator[](i1*g_count+2) = g_ptr_weight->operator[](i2*g_count+2);
	  g_ptr_weight->operator[](i1*g_count+3) = g_ptr_weight->operator[](i2*g_count+3);
	  g_ptr_weight->operator[](i1*g_count+4) = g_ptr_weight->operator[](i2*g_count+4);
	  g_ptr_weight->operator[](i1*g_count+5) = g_ptr_weight->operator[](i2*g_count+5);	  
	  }

	  
    }
}
Point g_x[6];
Vector gv_x[6];
float gc_x[6*4];
Point gt_x[6];
VEC4 gj_x[6];
VEC4 gw_x[6];
void store_elem(int i)
{
  if (g_count==3) {
    g_x[0] = g_ptr->operator[](i*g_count+0);
    g_x[1] = g_ptr->operator[](i*g_count+1);
    g_x[2] = g_ptr->operator[](i*g_count+2);

    if (g_ptr_normal) {
    gv_x[0] = g_ptr_normal->operator[](i*g_count+0);
    gv_x[1] = g_ptr_normal->operator[](i*g_count+1);
    gv_x[2] = g_ptr_normal->operator[](i*g_count+2);
    }

    
    if (g_ptr_color) {
    gc_x[0] = g_ptr_color->operator[](i*g_count*4+0);
    gc_x[1] = g_ptr_color->operator[](i*g_count*4+1);
    gc_x[2] = g_ptr_color->operator[](i*g_count*4+2);

    gc_x[3] = g_ptr_color->operator[](i*g_count*4+3);
    gc_x[4] = g_ptr_color->operator[](i*g_count*4+4);
    gc_x[5] = g_ptr_color->operator[](i*g_count*4+5);

    gc_x[6] = g_ptr_color->operator[](i*g_count*4+6);
    gc_x[7] = g_ptr_color->operator[](i*g_count*4+7);
    gc_x[8] = g_ptr_color->operator[](i*g_count*4+8);

    gc_x[9] = g_ptr_color->operator[](i*g_count*4+9);
    gc_x[10] = g_ptr_color->operator[](i*g_count*4+10);
    gc_x[11] = g_ptr_color->operator[](i*g_count*4+11);

    
    
    }
    
    if (g_ptr_texcoord) {
    gt_x[0] = g_ptr_texcoord->operator[](i*g_count+0);
    gt_x[1] = g_ptr_texcoord->operator[](i*g_count+1);
    gt_x[2] = g_ptr_texcoord->operator[](i*g_count+2);
    }
    
    if (g_ptr_joint) {
    gj_x[0] = g_ptr_joint->operator[](i*g_count+0);
    gj_x[1] = g_ptr_joint->operator[](i*g_count+1);
    gj_x[2] = g_ptr_joint->operator[](i*g_count+2);
    }
    if (g_ptr_weight) {
    gw_x[0] = g_ptr_weight->operator[](i*g_count+0);
    gw_x[1] = g_ptr_weight->operator[](i*g_count+1);
    gw_x[2] = g_ptr_weight->operator[](i*g_count+2);
    }
    
    
  } else if (g_count==6)
    {
    g_x[0] = g_ptr->operator[](i*g_count+0);
    g_x[1] = g_ptr->operator[](i*g_count+1);
    g_x[2] = g_ptr->operator[](i*g_count+2);
    g_x[3] = g_ptr->operator[](i*g_count+3);
    g_x[4] = g_ptr->operator[](i*g_count+4);
    g_x[5] = g_ptr->operator[](i*g_count+5);

    if (g_ptr_normal) {
    gv_x[0] = g_ptr_normal->operator[](i*g_count+0);
    gv_x[1] = g_ptr_normal->operator[](i*g_count+1);
    gv_x[2] = g_ptr_normal->operator[](i*g_count+2);
    gv_x[3] = g_ptr_normal->operator[](i*g_count+3);
    gv_x[4] = g_ptr_normal->operator[](i*g_count+4);
    gv_x[5] = g_ptr_normal->operator[](i*g_count+5);
    }
    
    if (g_ptr_color) {
    gc_x[0] = g_ptr_color->operator[](i*g_count*4+0);
    gc_x[1] = g_ptr_color->operator[](i*g_count*4+1);
    gc_x[2] = g_ptr_color->operator[](i*g_count*4+2);
    gc_x[3] = g_ptr_color->operator[](i*g_count*4+3);
    gc_x[4] = g_ptr_color->operator[](i*g_count*4+4);
    gc_x[5] = g_ptr_color->operator[](i*g_count*4+5);

    gc_x[6] = g_ptr_color->operator[](i*g_count*4+6);
    gc_x[7] = g_ptr_color->operator[](i*g_count*4+7);
    gc_x[8] = g_ptr_color->operator[](i*g_count*4+8);
    gc_x[9] = g_ptr_color->operator[](i*g_count*4+9);
    gc_x[10] = g_ptr_color->operator[](i*g_count*4+10);
    gc_x[11] = g_ptr_color->operator[](i*g_count*4+11);

    gc_x[12] = g_ptr_color->operator[](i*g_count*4+12);
    gc_x[13] = g_ptr_color->operator[](i*g_count*4+13);
    gc_x[14] = g_ptr_color->operator[](i*g_count*4+14);
    gc_x[15] = g_ptr_color->operator[](i*g_count*4+15);
    gc_x[16] = g_ptr_color->operator[](i*g_count*4+16);
    gc_x[17] = g_ptr_color->operator[](i*g_count*4+17);

    gc_x[18] = g_ptr_color->operator[](i*g_count*4+18);
    gc_x[19] = g_ptr_color->operator[](i*g_count*4+19);
    gc_x[20] = g_ptr_color->operator[](i*g_count*4+20);
    gc_x[21] = g_ptr_color->operator[](i*g_count*4+21);
    gc_x[22] = g_ptr_color->operator[](i*g_count*4+22);
    gc_x[23] = g_ptr_color->operator[](i*g_count*4+23);

    
    
    }
    
    if (g_ptr_texcoord) {
    gt_x[0] = g_ptr_texcoord->operator[](i*g_count+0);
    gt_x[1] = g_ptr_texcoord->operator[](i*g_count+1);
    gt_x[2] = g_ptr_texcoord->operator[](i*g_count+2);
    gt_x[3] = g_ptr_texcoord->operator[](i*g_count+3);
    gt_x[4] = g_ptr_texcoord->operator[](i*g_count+4);
    gt_x[5] = g_ptr_texcoord->operator[](i*g_count+5);
    }

    if (g_ptr_joint) {
      gj_x[0] = g_ptr_joint->operator[](i*g_count+0);
      gj_x[1] = g_ptr_joint->operator[](i*g_count+1);
      gj_x[2] = g_ptr_joint->operator[](i*g_count+2);
      gj_x[3] = g_ptr_joint->operator[](i*g_count+3);
      gj_x[4] = g_ptr_joint->operator[](i*g_count+4);
      gj_x[5] = g_ptr_joint->operator[](i*g_count+5);
    }

    if (g_ptr_weight) {
      gw_x[0] = g_ptr_weight->operator[](i*g_count+0);
      gw_x[1] = g_ptr_weight->operator[](i*g_count+1);
      gw_x[2] = g_ptr_weight->operator[](i*g_count+2);
      gw_x[3] = g_ptr_weight->operator[](i*g_count+3);
      gw_x[4] = g_ptr_weight->operator[](i*g_count+4);
      gw_x[5] = g_ptr_weight->operator[](i*g_count+5);
    }
    }
}
void set_elem(int i2)
{
  if (g_count==3) {
    g_ptr->operator[](i2*g_count+0) = g_x[0];
    g_ptr->operator[](i2*g_count+1) = g_x[1];
    g_ptr->operator[](i2*g_count+2) = g_x[2];

    if (g_ptr_normal) {
    g_ptr_normal->operator[](i2*g_count+0) = gv_x[0];
    g_ptr_normal->operator[](i2*g_count+1) = gv_x[1];
    g_ptr_normal->operator[](i2*g_count+2) = gv_x[2];
    }
    
    if (g_ptr_color) {
    g_ptr_color->operator[](i2*g_count*4+0) = gc_x[0];
    g_ptr_color->operator[](i2*g_count*4+1) = gc_x[1];
    g_ptr_color->operator[](i2*g_count*4+2) = gc_x[2];

    g_ptr_color->operator[](i2*g_count*4+3) = gc_x[3];
    g_ptr_color->operator[](i2*g_count*4+4) = gc_x[4];
    g_ptr_color->operator[](i2*g_count*4+5) = gc_x[5];

    g_ptr_color->operator[](i2*g_count*4+6) = gc_x[6];
    g_ptr_color->operator[](i2*g_count*4+7) = gc_x[7];
    g_ptr_color->operator[](i2*g_count*4+8) = gc_x[8];

    g_ptr_color->operator[](i2*g_count*4+9) = gc_x[9];
    g_ptr_color->operator[](i2*g_count*4+10) = gc_x[10];
    g_ptr_color->operator[](i2*g_count*4+11) = gc_x[11];

    }
    
    if (g_ptr_texcoord) {
    g_ptr_texcoord->operator[](i2*g_count+0) = gt_x[0];
    g_ptr_texcoord->operator[](i2*g_count+1) = gt_x[1];
    g_ptr_texcoord->operator[](i2*g_count+2) = gt_x[2];
    }
    if (g_ptr_joint) {
    g_ptr_joint->operator[](i2*g_count+0) = gj_x[0];
    g_ptr_joint->operator[](i2*g_count+1) = gj_x[1];
    g_ptr_joint->operator[](i2*g_count+2) = gj_x[2];
    }
    if (g_ptr_weight) {
    g_ptr_weight->operator[](i2*g_count+0) = gw_x[0];
    g_ptr_weight->operator[](i2*g_count+1) = gw_x[1];
    g_ptr_weight->operator[](i2*g_count+2) = gw_x[2];
    }
    
    
    
  } else if (g_count==6)
    {
    g_ptr->operator[](i2*g_count+0) = g_x[0];
    g_ptr->operator[](i2*g_count+1) = g_x[1];
    g_ptr->operator[](i2*g_count+2) = g_x[2];
    g_ptr->operator[](i2*g_count+3) = g_x[3];
    g_ptr->operator[](i2*g_count+4) = g_x[4];
    g_ptr->operator[](i2*g_count+5) = g_x[5];

    if (g_ptr_normal) {
    g_ptr_normal->operator[](i2*g_count+0) = gv_x[0];
    g_ptr_normal->operator[](i2*g_count+1) = gv_x[1];
    g_ptr_normal->operator[](i2*g_count+2) = gv_x[2];
    g_ptr_normal->operator[](i2*g_count+3) = gv_x[3];
    g_ptr_normal->operator[](i2*g_count+4) = gv_x[4];
    g_ptr_normal->operator[](i2*g_count+5) = gv_x[5];
    }
    if (g_ptr_color) {
    g_ptr_color->operator[](i2*g_count*4+0) = gc_x[0];
    g_ptr_color->operator[](i2*g_count*4+1) = gc_x[1];
    g_ptr_color->operator[](i2*g_count*4+2) = gc_x[2];
    g_ptr_color->operator[](i2*g_count*4+3) = gc_x[3];
    g_ptr_color->operator[](i2*g_count*4+4) = gc_x[4];
    g_ptr_color->operator[](i2*g_count*4+5) = gc_x[5];

    g_ptr_color->operator[](i2*g_count*4+6) = gc_x[6];
    g_ptr_color->operator[](i2*g_count*4+7) = gc_x[7];
    g_ptr_color->operator[](i2*g_count*4+8) = gc_x[8];
    g_ptr_color->operator[](i2*g_count*4+9) = gc_x[9];
    g_ptr_color->operator[](i2*g_count*4+10) = gc_x[10];
    g_ptr_color->operator[](i2*g_count*4+11) = gc_x[11];

    g_ptr_color->operator[](i2*g_count*4+12) = gc_x[12];
    g_ptr_color->operator[](i2*g_count*4+13) = gc_x[13];
    g_ptr_color->operator[](i2*g_count*4+14) = gc_x[14];
    g_ptr_color->operator[](i2*g_count*4+15) = gc_x[15];
    g_ptr_color->operator[](i2*g_count*4+16) = gc_x[16];
    g_ptr_color->operator[](i2*g_count*4+17) = gc_x[17];

    g_ptr_color->operator[](i2*g_count*4+18) = gc_x[18];
    g_ptr_color->operator[](i2*g_count*4+19) = gc_x[19];
    g_ptr_color->operator[](i2*g_count*4+20) = gc_x[20];
    g_ptr_color->operator[](i2*g_count*4+21) = gc_x[21];
    g_ptr_color->operator[](i2*g_count*4+22) = gc_x[22];
    g_ptr_color->operator[](i2*g_count*4+23) = gc_x[23];

    
    
    }
    if (g_ptr_texcoord) {
    g_ptr_texcoord->operator[](i2*g_count+0) = gt_x[0];
    g_ptr_texcoord->operator[](i2*g_count+1) = gt_x[1];
    g_ptr_texcoord->operator[](i2*g_count+2) = gt_x[2];
    g_ptr_texcoord->operator[](i2*g_count+3) = gt_x[3];
    g_ptr_texcoord->operator[](i2*g_count+4) = gt_x[4];
    g_ptr_texcoord->operator[](i2*g_count+5) = gt_x[5];
    }
    if (g_ptr_joint) {
    g_ptr_joint->operator[](i2*g_count+0) = gj_x[0];
    g_ptr_joint->operator[](i2*g_count+1) = gj_x[1];
    g_ptr_joint->operator[](i2*g_count+2) = gj_x[2];
    g_ptr_joint->operator[](i2*g_count+3) = gj_x[3];
    g_ptr_joint->operator[](i2*g_count+4) = gj_x[4];
    g_ptr_joint->operator[](i2*g_count+5) = gj_x[5];
    }
    if (g_ptr_weight) {
    g_ptr_weight->operator[](i2*g_count+0) = gw_x[0];
    g_ptr_weight->operator[](i2*g_count+1) = gw_x[1];
    g_ptr_weight->operator[](i2*g_count+2) = gw_x[2];
    g_ptr_weight->operator[](i2*g_count+3) = gw_x[3];
    g_ptr_weight->operator[](i2*g_count+4) = gw_x[4];
    g_ptr_weight->operator[](i2*g_count+5) = gw_x[5];
    }
    
    }
    
}
void sort_vertex()
{
  if (g_count==3) {
    int i = 1;
    int s = g_ptr->size()/g_count;
    while(i<s) {
      //Point *x_p1 = &(g_ptr->operator[](i*g_count));
      store_elem(i);
      int j = i - 1;
      while(j >= 0 && sort_compare_tri_polys(&g_ptr->operator[](j*g_count),&g_x[0]))
	{
	  swap_elem(j+1,j);
	  j=j-1;
	}
      set_elem(j+1);
      i = i + 1;
    }
  } else if (g_count==4) {

    int i = 1;
    int s = g_ptr->size()/g_count;
    while(i<s) {
      //Point *x_p1 = &(g_ptr->operator[](i*g_count));
      store_elem(i);
      int j = i - 1;
      while(j >= 0 && sort_compare_tri_polys(&g_ptr->operator[](j*g_count),&g_x[0]))
	{
	  swap_elem(j+1,j);
	  j=j-1;
	}
      set_elem(j+1);
      i = i + 1;
    }
  }
}
void RenderVertexArray::sort_blit(int id, Matrix in_MV)
{
#if 1
  g_in_MV = in_MV;
  g_ptr = &(s.m_set[id]->tri_polys);
  g_ptr_normal = s.has_normal ? &(s.m_set[id]->tri_normals) : 0;
  g_ptr_color = s.has_color ? &(s.m_set[id]->tri_color) : 0;
  g_ptr_texcoord = s.has_texcoord ? &(s.m_set[id]->tri_texcoord) : 0;
  g_ptr_joint = s.has_skeleton ? &(s.m_set[id]->tri_joint) : 0;
  g_ptr_weight = s.has_skeleton ? &(s.m_set[id]->tri_weight) : 0;
  g_count = 3;
  sort_vertex();
  g_ptr = &(s.m_set[id]->quad_polys);
  g_ptr_normal = s.has_normal ? &(s.m_set[id]->quad_normals) : 0;
  g_ptr_color = s.has_color ? &(s.m_set[id]->quad_color) : 0;
  g_ptr_texcoord = s.has_texcoord ? &(s.m_set[id]->quad_texcoord) : 0;
  g_ptr_joint = s.has_skeleton ? &(s.m_set[id]->quad_joint) : 0;
  g_ptr_weight = s.has_skeleton ? &(s.m_set[id]->quad_weight) : 0;
  g_count=6;
  sort_vertex();
  update(id);
#endif
}
extern bool g_disable_polygons;

GameApi::PinIn RenderVertexArray::update(int id)
{ 
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
 int start=0;
  int end = s.tri_count(id);
  //std::cout << "Update" << std::endl;
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_normal_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, end*sizeof(float)*4, s.tri_color_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_texcoord_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*3, end*sizeof(float)*3, s.tri_polys2(id));

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, end*sizeof(float)*4, s.tri_joint_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, start*sizeof(float)*4, end*sizeof(float)*4, s.tri_weight_polys(id));


#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_normal_polys(id));

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*4, s.quad_color_polys(id));

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_texcoord_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*3, s.quad_polys2(id));


  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*4, s.quad_joint_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.quad_count(id)*sizeof(float)*4, s.quad_weight_polys(id));


  if (!g_disable_polygons) {
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_normal_polys(id));

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*4, s.poly_color_polys(id));

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_texcoord_polys(id));
  
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*3, s.poly_polys2(id));


  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*4, s.poly_joint_polys(id));
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
  ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, s.poly_count_f(id)*sizeof(float)*4, s.poly_weight_polys(id));
  }
  
  
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  return GameApi::PinIn();
}
void RenderVertexArray::update_buffers(RenderVertexArray_bufferids ids)
{
  int s = 7;
  for(int i=0;i<s;i++) { buffers[i]=ids.buffers[i]; buffers2[i]=ids.buffers2[i]; buffers3[i]=ids.buffers3[i]; }
  int ss = 3;
  for(int i=0;i<ss;i++) { vao[i]=ids.vao[i]; }
  pos_buffer=ids.pos_buffer;
  pos_buffer_matrix = ids.pos_buffer_matrix;
  normals_buffer=ids.normals_buffer;
  normals_buffer_matrix = ids.normals_buffer_matrix;
  color_buffer=ids.color_buffer;
  color_buffer_matrix = ids.color_buffer_matrix;
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
  int s = 7;
  for(int i=0;i<s;i++) { ids.buffers[i]=buffers[i]; ids.buffers2[i]=buffers2[i]; ids.buffers3[i]=buffers3[i]; }
  int ss = 3;
  for(int i=0;i<ss;i++) { ids.vao[i]=vao[i]; }
  ids.pos_buffer=pos_buffer;
  ids.pos_buffer_matrix = pos_buffer_matrix;
  ids.normals_buffer=normals_buffer;
  ids.normals_buffer_matrix = normals_buffer_matrix;
  ids.color_buffer=color_buffer;
  ids.color_buffer_matrix = color_buffer_matrix;

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

extern bool g_disable_polygons;

GameApi::PinIn RenderVertexArray::prepare(int id, bool isnull, int tri_count_, int quad_count_, int poly_count_)
{
  OpenglLowApi *ogl = g_low->ogl;

  if (id==-1) { std::cout << "RenderVertexArray::prepare id=-1" << std::endl; id=0; }

  if (tri_count_==-1) tri_count_ = s.tri_count(id);
  if (quad_count_==-1) quad_count_ = s.quad_count(id);
  if (poly_count_==-1) poly_count_ = s.poly_count_f(id);
  tri_count = tri_count_;
  quad_count = quad_count_;
  poly_count = poly_count_;
  
  s.check_m_set(id);
#ifdef VAO
  ogl->glGenVertexArrays(3,vao);
  ogl->glBindVertexArray(vao[0]);
#endif
  ogl->glGenBuffers(1,&buffers[0]);
  ogl->glGenBuffers(1,&buffers[1]);
  ogl->glGenBuffers(1,&buffers[2]);
  ogl->glGenBuffers(1,&buffers[3]);
  ogl->glGenBuffers(1,&buffers[4]);
  ogl->glGenBuffers(1,&buffers[5]);
  ogl->glGenBuffers(1,&buffers[6]);
  ogl->glGenBuffers(1, &pos_buffer);
  ogl->glGenBuffers(1, &pos_buffer_matrix);
  ogl->glGenBuffers(1, &normals_buffer);
  ogl->glGenBuffers(1, &normals_buffer_matrix);
  ogl->glGenBuffers(1, &color_buffer);
  ogl->glGenBuffers(1, &color_buffer_matrix);
  VertexArraySet::Polys *p = s.m_set[id];
  { // attrib buffers
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attrib_buffer.push_back(val);
    }
  int ss2 = p->quad_attribs.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attrib_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribs.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attrib_buffer3.push_back(val);
    }
  }

  { // attribi buffers
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attribi_buffer.push_back(val);
    }
  int ss2 = p->quad_attribsi.size();
  for(int i=0;i<ss2;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attribi_buffer2.push_back(val);
    }
  int ss3 = p->poly_attribsi.size();
  for(int i=0;i<ss3;i++)
    {
      unsigned int val;
      ogl->glGenBuffers(1, &val);
      attribi_buffer3.push_back(val);
    }
  }
  if (tri_count) {
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_normal_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*4, isnull?0:s.tri_color_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_texcoord_polys(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*3, isnull?0:s.tri_polys2(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*4, isnull?0:s.tri_joint_polys(id), Low_GL_STATIC_DRAW);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float)*4, isnull?0:s.tri_weight_polys(id), Low_GL_STATIC_DRAW);

  
  {
  // ATTRIB STARTS HERE
  int ss = p->tri_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->tri_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(float), isnull?0:s.tri_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }

  }

  {
  // ATTRIBI start here
  int ss = p->tri_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->tri_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, tri_count*sizeof(int), isnull?0:s.tri_attribi_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }

  }

  }

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif
  ogl->glGenBuffers(1,&buffers2[0]);
  ogl->glGenBuffers(1,&buffers2[1]);
  ogl->glGenBuffers(1,&buffers2[2]);
  ogl->glGenBuffers(1,&buffers2[3]);
  ogl->glGenBuffers(1,&buffers2[4]);
  ogl->glGenBuffers(1,&buffers2[5]);
  ogl->glGenBuffers(1,&buffers2[6]);
  if (quad_count) {
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_normal_polys(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*4, isnull?0:s.quad_color_polys(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_texcoord_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*3, isnull?0:s.quad_polys2(id), Low_GL_STATIC_DRAW);


  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*4, isnull?0:s.quad_joint_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float)*4, isnull?0:s.quad_weight_polys(id), Low_GL_STATIC_DRAW);

  
  {
  int ss = p->quad_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->quad_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer2[i]);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(float), isnull?0:s.quad_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }
  }
  {
  int ss = p->quad_attribsi.size();
  std::map<int,std::vector<int> >::iterator ii = p->quad_attribsi.begin();
  for(int i=0;i<ss;i++)
    {
      //std::cout << "Quad Attribi" << std::endl;
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer2[i]);
      const int *ptr = isnull?0:s.quad_attribi_polys(id, (*ii).first);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, quad_count*sizeof(int), ptr, Low_GL_STATIC_DRAW);
      ii++;
    }

  }
  }
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


  if (!g_disable_polygons) {
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif
  ogl->glGenBuffers(1,&buffers3[0]);
  ogl->glGenBuffers(1,&buffers3[1]);
  ogl->glGenBuffers(1,&buffers3[2]);
  ogl->glGenBuffers(1,&buffers3[3]);
  ogl->glGenBuffers(1,&buffers3[4]);
  ogl->glGenBuffers(1,&buffers3[5]);
  ogl->glGenBuffers(1,&buffers3[6]);
  if (poly_count) {
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_normal_polys(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*4, isnull?0:s.poly_color_polys(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_texcoord_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*3, isnull?0:s.poly_polys2(id), Low_GL_STATIC_DRAW);

  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*4, isnull?0:s.poly_joint_polys(id), Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float)*4, isnull?0:s.poly_weight_polys(id), Low_GL_STATIC_DRAW);

  
  
  {
#if 1
  int ss = p->poly_attribs.size();
  std::map<int,std::vector<float> >::iterator ii = p->poly_attribs.begin();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer3[i]);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(float), isnull?0:s.poly_attrib_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
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
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer3[i]);
      ogl->glBufferData(Low_GL_ARRAY_BUFFER, poly_count*sizeof(int), isnull?0:s.poly_attribi_polys(id, (*ii).first), Low_GL_STATIC_DRAW);
      ii++;
    }
#endif
  }

  }
  }
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


#ifdef VAO
    ogl->glBindVertexArray(vao[0]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);

#ifdef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    
    int counter=8;

    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
#ifndef EMSCRIPTEN
      ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;
    }
    }

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif
#ifdef VAO
    ogl->glBindVertexArray(vao[1]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    counter=8;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer2[i]);
      ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;			    
    }
    }

    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer2[i]);
      //std::cout << "Counter: " << counter << std::endl;
#ifndef EMSCRIPTEN
      ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;			    
    }
    }

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

    if (!g_disable_polygons) {

#ifdef VAO
    ogl->glBindVertexArray(vao[2]);
#endif
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
#ifdef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    counter=6;

    {
  int ss = p->poly_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer3[i]);
      ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;			    
    }
    }
    {
  int ss = p->poly_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer3[i]);
#ifndef EMSCRIPTEN
      ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;			    
    }
    }
    }
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif


#ifdef VAO
    ogl->glBindVertexArray(0);
#endif
    //tri_count = s.tri_count(id);
    //quad_count = s.quad_count(id);
    //poly_count = s.poly_count_f(id);
  return GameApi::PinIn();
}
void RenderVertexArray::del()
{
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glDeleteVertexArrays(3, &vao[0]);
#endif
  //std::cout << "Del" << std::endl;
  ogl->glDeleteBuffers(1,&buffers[0]);
  ogl->glDeleteBuffers(1,&buffers[1]);
  ogl->glDeleteBuffers(1,&buffers[2]);
  ogl->glDeleteBuffers(1,&buffers[3]);
  ogl->glDeleteBuffers(1,&buffers[4]);
  ogl->glDeleteBuffers(1,&buffers[5]);
  ogl->glDeleteBuffers(1,&buffers[6]);
  ogl->glDeleteBuffers(1,&pos_buffer);
  ogl->glDeleteBuffers(1,&pos_buffer_matrix);
  ogl->glDeleteBuffers(1,&normals_buffer);
  ogl->glDeleteBuffers(1,&normals_buffer_matrix);
  ogl->glDeleteBuffers(1,&color_buffer);
  ogl->glDeleteBuffers(1,&color_buffer_matrix);

  ogl->glDeleteBuffers(1,&buffers2[0]);
  ogl->glDeleteBuffers(1,&buffers2[1]);
  ogl->glDeleteBuffers(1,&buffers2[2]);
  ogl->glDeleteBuffers(1,&buffers2[3]);
  ogl->glDeleteBuffers(1,&buffers2[4]);
  ogl->glDeleteBuffers(1,&buffers2[5]);
  ogl->glDeleteBuffers(1,&buffers2[6]);

  ogl->glDeleteBuffers(1,&buffers3[0]);
  ogl->glDeleteBuffers(1,&buffers3[1]);
  ogl->glDeleteBuffers(1,&buffers3[2]);
  ogl->glDeleteBuffers(1,&buffers3[3]);
  ogl->glDeleteBuffers(1,&buffers3[4]);
  ogl->glDeleteBuffers(1,&buffers3[5]);
  ogl->glDeleteBuffers(1,&buffers3[6]);
}
GameApi::PinIn RenderVertexArray::prepare_instanced_matrix(int id, Matrix *positions, Vector *normals, unsigned int *colors, int size)
{
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );

  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);
  if (normals) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }

#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif

  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);
  if (normals) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif

  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);
  if (normals) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif

  return GameApi::PinIn();
}
GameApi::PinIn RenderVertexArray::prepare_instanced(int id, Point *positions, Vector *normals, unsigned int *colors, int size)
{
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );

  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }
  
#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif

  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }

  


  
  
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif

  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Vector) * size, normals, Low_GL_DYNAMIC_DRAW);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(unsigned int) * size, colors, Low_GL_DYNAMIC_DRAW);
  }


  
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif

  return GameApi::PinIn();
}
std::map<Point*,bool> g_inst_map;
std::map<Matrix*,bool> g_inst_map_matrix;
GameApi::PinIn RenderVertexArray::render_instanced_matrix(int id, Matrix *positions, Vector *normals, unsigned int *colors, int size)
{
  OpenglLowApi *ogl = g_low->ogl;
  //std::cout << "render_instanced_matrix: " << size << std::endl;

#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
    if (tri_count && size) {

      
  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
    if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
    }
    if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
    }
  }
  //#ifndef VAO    
  ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
  ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  ogl->glVertexAttribDivisor(7, 1);
  ogl->glVertexAttribDivisor(8, 1);
  ogl->glVertexAttribDivisor(9, 1);
  ogl->glVertexAttribDivisor(10, 1);
  ogl->glEnableVertexAttribArray(7);
  ogl->glEnableVertexAttribArray(8);
  ogl->glEnableVertexAttribArray(9);
  ogl->glEnableVertexAttribArray(10);

  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);

  //#endif
  

#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
    ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
    ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);

    if (s.has_skeleton) {
    ogl->glEnableVertexAttribArray(11);
    ogl->glEnableVertexAttribArray(12);
    }
#endif

      ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, tri_count, size);
      //std::cout << "InstancingTRI: " << tri_count << " " << size << std::endl;

#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(7);
    ogl->glDisableVertexAttribArray(8);
    ogl->glDisableVertexAttribArray(9);
    ogl->glDisableVertexAttribArray(10);
#endif  
    }

#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count && size) {

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
  ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
  }
  }


  //#ifndef VAO
  
  ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
				    ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  ogl->glVertexAttribDivisor(7, 1);
  ogl->glVertexAttribDivisor(8, 1);
  ogl->glVertexAttribDivisor(9, 1);
  ogl->glVertexAttribDivisor(10, 1);
  ogl->glEnableVertexAttribArray(7);
  ogl->glEnableVertexAttribArray(8);
  ogl->glEnableVertexAttribArray(9);
  ogl->glEnableVertexAttribArray(10);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);

  ogl->glVertexAttribDivisor(11, 0);
  ogl->glVertexAttribDivisor(12, 0);

  //#endif  

#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
    ogl->glVertexAttribPointer(11, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
    ogl->glVertexAttribPointer(12, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);
    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
#endif
      ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, quad_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(7);
    ogl->glDisableVertexAttribArray(8);
    ogl->glDisableVertexAttribArray(9);
    ogl->glDisableVertexAttribArray(10);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);
#endif
    }

#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif
    if (poly_count && size) {

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer_matrix );
  if (g_inst_map_matrix[positions]==false) {
  ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer_matrix );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
  }
  }

  //#ifndef VAO
  ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
				    ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  ogl->glVertexAttribDivisor(7, 1);
  ogl->glVertexAttribDivisor(8, 1);
  ogl->glVertexAttribDivisor(9, 1);
  ogl->glVertexAttribDivisor(10, 1);
  ogl->glEnableVertexAttribArray(7);
  ogl->glEnableVertexAttribArray(8);
  ogl->glEnableVertexAttribArray(9);
  ogl->glEnableVertexAttribArray(10);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);
  ogl->glVertexAttribDivisor(11, 0);
  ogl->glVertexAttribDivisor(12, 0);

  //#endif
  
#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);

    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
#endif
      ogl->glDrawArraysInstanced(Low_GL_TRIANGLE_STRIP, 0, poly_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;

#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(7);
    ogl->glDisableVertexAttribArray(8);
    ogl->glDisableVertexAttribArray(9);
    ogl->glDisableVertexAttribArray(10);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);
#endif
    }

#ifdef VAO
    ogl->glBindVertexArray(0);
#endif
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
    g_inst_map_matrix[positions]=true;


    return GameApi::PinIn();
}
GameApi::PinIn RenderVertexArray::render_instanced(int id, Point *positions, Vector *normals, unsigned int *colors, int size)
{
  OpenglLowApi *ogl = g_low->ogl;
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
    if (tri_count && size) {

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
    if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
    }
    if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
    }
  }
    
  ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glVertexAttribDivisor(5, 1);
  ogl->glEnableVertexAttribArray(5);

  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);

  ogl->glVertexAttribDivisor(11, 0);
  ogl->glVertexAttribDivisor(12, 0);

  

#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
    ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);

    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
#endif

      ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, tri_count, size);
      //std::cout << "InstancingTRI: " << tri_count << " " << size << std::endl;

#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(5);

    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);

#endif  
    }

#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count && size) {

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
  ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
  }
  }
  ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glVertexAttribDivisor(5, 1);
  ogl->glEnableVertexAttribArray(5);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);

  ogl->glVertexAttribDivisor(11, 0);
  ogl->glVertexAttribDivisor(12, 0);

  
#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);

    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
      
#endif
      ogl->glDrawArraysInstanced(Low_GL_TRIANGLES, 0, quad_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;
#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(5);

    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);

#endif
    }

    if (!g_disable_polygons) {
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif
    if (poly_count && size) {

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, pos_buffer );
  if (g_inst_map[positions]==false) {
  ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  if (normals) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, normals_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Vector) * size, normals);
  }
  if (colors) {
    ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, color_buffer );
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * size, colors);
  }
  }
  ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glVertexAttribDivisor(5, 1);
  ogl->glEnableVertexAttribArray(5);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);

  ogl->glVertexAttribDivisor(11, 0);
  ogl->glVertexAttribDivisor(12, 0);

  

#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);
    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
#endif
      ogl->glDrawArraysInstanced(Low_GL_TRIANGLE_STRIP, 0, poly_count, size);
      //std::cout << "InstancingQUAD: " << quad_count << " " << size << std::endl;

#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(5);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);
#endif
    }
    }
#ifdef VAO
    ogl->glBindVertexArray(0);
#endif
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);
    g_inst_map[positions]=true;

    return GameApi::PinIn();
}

extern bool g_disable_polygons;

GameApi::PinIn RenderVertexArray::render(int id)
{
  VertexArraySet::Polys *p = s.m_set[id];
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
    int counter = 6;
    if (tri_count > 0)
      {
#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attrib_buffer[i]);
      ogl->glVertexAttribPointer(counter, 1, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
      counter++;
    }
    }

    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, attribi_buffer[i]);
#ifndef EMSCRIPTEN
      ogl->glVertexAttribIPointer(counter, 1, Low_GL_INT, 0,0);
#endif
      counter++;
    }
    }

#endif


#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal) 
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);
    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      ogl->glDrawArrays(Low_GL_TRIANGLES, 0, tri_count);
#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);


    counter = 6;
    {
  int ss = p->tri_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->tri_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }


#endif
      }
#ifdef VAO
  ogl->glBindVertexArray(vao[1]);
#endif
    if (quad_count >0)
      {
#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers2[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
      ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);

    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }

    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glEnableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      ogl->glDrawArrays(Low_GL_TRIANGLES, 0, quad_count);
#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);


    counter = 6;
    {
  int ss = p->quad_attribs.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }
    {
  int ss = p->quad_attribsi.size();
  for(int i=0;i<ss;i++)
    {
      ogl->glDisableVertexAttribArray(counter);
      counter++;
    }
    }

#endif
      }

    if (!g_disable_polygons) {
#ifdef VAO
  ogl->glBindVertexArray(vao[2]);
#endif
  if (poly_count > 0) {

#ifndef VAO
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[0]);
    ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[1]);
    ogl->glVertexAttribPointer(1, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[2]);
    ogl->glVertexAttribPointer(2, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[3]);
    ogl->glVertexAttribPointer(3, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[4]);
    ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);

    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[5]);
    ogl->glVertexAttribPointer(11, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers3[6]);
    ogl->glVertexAttribPointer(12, 4, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);

#endif

#if 1
    ogl->glEnableVertexAttribArray(0);
    if (s.has_normal)
    ogl->glEnableVertexAttribArray(1);
    if (s.has_color)
      ogl->glEnableVertexAttribArray(2);
    if (s.has_texcoord)
      ogl->glEnableVertexAttribArray(3);
    ogl->glEnableVertexAttribArray(4);
    
    if (s.has_skeleton) {
      ogl->glEnableVertexAttribArray(11);
      ogl->glEnableVertexAttribArray(12);
    }
#endif
      ogl->glDrawArrays(Low_GL_TRIANGLE_STRIP, 0, poly_count);
#if 1
    ogl->glDisableVertexAttribArray(0);
    ogl->glDisableVertexAttribArray(1);
    ogl->glDisableVertexAttribArray(2);
    ogl->glDisableVertexAttribArray(3);
    ogl->glDisableVertexAttribArray(4);
    ogl->glDisableVertexAttribArray(11);
    ogl->glDisableVertexAttribArray(12);
#endif
  }
    }

#ifdef VAO
    ogl->glBindVertexArray(0);
#endif
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, 0);


    return GameApi::PinIn();
  }

void RenderVertexArray2::render(int id, int attr1, int attr2, int attr3, int attr4,
				int aattr1, int aattr2, int aattr3, int aattr4)
{
  OpenglLowApi *ogl = g_low->ogl;
    // triangles
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    ogl->glEnableVertexAttribArray(aattr1);
    ogl->glEnableVertexAttribArray(aattr2);
    ogl->glEnableVertexAttribArray(aattr3);
    ogl->glEnableVertexAttribArray(aattr4);

    ogl->glEnableVertexAttribArray(attr1);
    ogl->glEnableVertexAttribArray(attr2);
    ogl->glEnableVertexAttribArray(attr3);
    ogl->glEnableVertexAttribArray(attr4);

    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.tri_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.tri_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.tri_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    ogl->glVertexAttribPointer(aattr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_polys(id));
    ogl->glVertexAttribPointer(aattr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_normal_polys(id));
    ogl->glVertexAttribPointer(aattr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_color_polys(id));
    ogl->glVertexAttribPointer(aattr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s1.tri_texcoord_polys(id));

    ogl->glVertexAttribPointer(attr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_polys(id));
    ogl->glVertexAttribPointer(attr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_normal_polys(id));
    ogl->glVertexAttribPointer(attr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_color_polys(id));
    ogl->glVertexAttribPointer(attr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s2.tri_texcoord_polys(id));

    ogl->glDrawArrays(Low_GL_TRIANGLES, 0, s1.tri_count(id));
    // quads
    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.quad_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.quad_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.quad_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    ogl->glVertexAttribPointer(aattr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_polys(id));
    ogl->glVertexAttribPointer(aattr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_normal_polys(id));
    ogl->glVertexAttribPointer(aattr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_color_polys(id));
    ogl->glVertexAttribPointer(aattr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s1.quad_texcoord_polys(id));

    ogl->glVertexAttribPointer(attr1, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_polys(id));
    ogl->glVertexAttribPointer(attr2, 3, Low_GL_FLOAT,  Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_normal_polys(id));
    ogl->glVertexAttribPointer(attr3, 4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_color_polys(id));
    ogl->glVertexAttribPointer(attr4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, (Low_GLvoid*)s2.quad_texcoord_polys(id));

    ogl->glDrawArrays(Low_GL_QUADS, 0, s1.quad_count(id));

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
    ogl->glDisableVertexAttribArray(attr1);
    ogl->glDisableVertexAttribArray(attr2);
    ogl->glDisableVertexAttribArray(attr3);
    ogl->glDisableVertexAttribArray(attr4);

    ogl->glDisableVertexAttribArray(aattr1);
    ogl->glDisableVertexAttribArray(aattr2);
    ogl->glDisableVertexAttribArray(aattr3);
    ogl->glDisableVertexAttribArray(aattr4);
  }
Counts CalcCounts(FaceCollection *coll, int start, int end);
Counts CalcOffsets(FaceCollection *coll, int start);
void ProgressBar(int val, int max);
std::atomic<bool> g_lock1, g_lock2, g_lock3;

#ifndef ARM
ThreadInfo volatile *ti_global;
volatile int thread_counter=0;


void *thread_func(void *data)
{
#ifndef BATCHING
  ThreadInfo *ti = (ThreadInfo*)data;
  //ti->va->reserve(0);
  ti->va->copy(ti->start_range, ti->end_range,ti->attrib, ti->attribi);
  //pthread_exit(NULL);
  return 0;
#else
  ThreadInfo *ti = (ThreadInfo*)data;
  int s = (ti->end_range-ti->start_range)/100000;
  if (s==0) s=1;
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
      //ti->va->reserve(0);
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
      //g_lock2.wait(false);
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
  //pthread_exit(NULL);
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
std::string g_msg_string;
long long g_copy_total;
long long g_copy_progress;
bool is_texture_usage_confirmed(const FaceCollection *p);
bool is_texture_usage_confirmed(VertexArraySet *set);

bool g_use_vertices_only = false;

void FaceCollectionVertexArray2::copy(int start_range, int end_range, std::vector<int> attribs, std::vector<int> attribsi)
{
    //std::cout << "Copy: " << start_range << " " << end_range << std::endl;
    //int ss = coll.NumFaces();
    //std::cout << "NumFaces: " << ss << std::endl;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(&s)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();

    //g_msg_string += std::string(has_normal2?"normal":"!n") + "+" + std::string(has_attrib2?"attrib":"!a") + "+" + std::string(has_color2?"color":"!c") + "+" + std::string(has_texcoord2?"texcoord":"!t") + "+" + std::string(has_skeleton2?"skeleton":"!s") + "\n";
    
    s.has_normal = has_normal2;
    s.has_attrib = has_attrib2;
    s.has_color = has_color2;
    s.has_texcoord = has_texcoord2;
    s.has_skeleton = has_skeleton2;

    
    for(int i=start_range;i<end_range;i++)
      {
	g_copy_progress++;
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
	    if (!g_use_vertices_only) {
	    if (has_normal2)
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
	    if (has_color2)
	      c[j] = coll.Color(i,j);
	    //std::cout << "Tex!" << i << std::endl;
	    if (has_texcoord2) {
	      Point2d p = coll.TexCoord(i,j);
	      float p2 = coll.TexCoord3(i,j);
	      tex[j] = Point(p.x,p.y,p2);
	    }

	    if (has_skeleton2) {
	      VEC4 jj = coll.Joints(i,j);
	      VEC4 ww = coll.Weights(i,j);
	      //if (jj.x>63.0)
	      //std::cout << "i=" << i << " j=" << j << "::" << jj.x << std::endl;
	      joints[j] = jj;
	      weights[j] = ww;
	    }
	    } // use vertices only
	    //std::cout << "VA: " << tex[j] << std::endl;
	  }
	//std::cout << "push_p!" << i << std::endl;
	s.push_poly(0, w, &p[0]);
	//  std::cout << "push_p2!" << i << std::endl;
	s.push_poly2(0, w, &p2[0]);
	//  std::cout << "push_n!" << i << std::endl
	if (!g_use_vertices_only) {
	if (has_normal2)
	  s.push_normal(0, w, &v[0]);

#if 0
	for (int k=0;k<(int)attribs.size();k++)
	  s.push_attrib(0, attribs[k], w, &a[k][0]);
	for (int k=0;k<(int)attribsi.size();k++)
	  s.push_attribi(0, attribsi[k], w, &ai[k][0]);
#endif
	//  std::cout << "push_c!" << std::endl;
	if (has_color2)
	  s.push_color(0, w, &c[0]);
	//  std::cout << "push_texcoord!" << std::endl;
	if (has_texcoord2)
	  s.push_texcoord(0, w, &tex[0]);
	if (has_skeleton2)
	  {
	    s.push_joint(0,w,&joints[0]);
	    s.push_weight(0,w,&weights[0]);
	  }
	}
      }
      
    //std::cout << "Copy returns" << std::endl;
  }

#ifdef NO_PREPARE_RENDER

void check_error(std::string loc)
{
  int val = g_low->ogl->glGetError();
  if (val!=Low_GL_NO_ERROR) {
    std::cout << loc << " " << val << std::endl;
  }
}


Dyn::Dyn(VertexArrays *arr) : arr(arr) { firsttime=true;}
void Dyn::set_texture(std::vector<GameApi::TXID> vec)
{
  //  std::cout << "Dyn::set_texture()" << std::endl;
  textures=vec;
}
void Dyn::prepare(int i)
  {
    // std::cout << "Dyn::Prepare()" << std::endl;
  OpenglLowApi *ogl = g_low->ogl;

  if (i<0||i>9) return;
    enabled[i]=true;
    void *data =0;
    void *indi_data=0;
    size_t sz=0;
    bool joint_s=false;
    switch(i) {
    case 0: data=arr->Attrib(VA_Position); sz=sizeof(float)*arr->ComponentCount(VA_Position);; break;
    case 1: data=arr->Attrib(VA_Normal); sz=sizeof(float)*arr->ComponentCount(VA_Normal);; break;
    case 2: data=arr->Attrib(VA_TexCoord_0); sz=sizeof(float)*arr->ComponentCount(VA_TexCoord_0); break;
    case 3: data=arr->Attrib(VA_TexCoord_1); sz=sizeof(float)*arr->ComponentCount(VA_TexCoord_1); break;
    case 4: data=arr->Attrib(VA_Color_0); sz=sizeof(float)*arr->ComponentCount(VA_Color_0); break;
    case 5: data=arr->Attrib(VA_Color_1); sz=sizeof(float)*arr->ComponentCount(VA_Color_1); break;
    case 6:
      data=arr->Attrib(VA_Joints_0); sz=sizeof(float)*4;
      if (arr->ComponentType(VA_Joints_0)==VA_USHORT) {
	sz=sizeof(unsigned short)*arr->ComponentCount(VA_Joints_0); joint_s=true;
      }
      break;
    case 7: data=arr->Attrib(VA_Joints_1); sz=sizeof(float)*arr->ComponentCount(VA_Joints_1); break;
    case 8: data=arr->Attrib(VA_Weights_0); sz=sizeof(float)*arr->ComponentCount(VA_Weights_0); break;
    case 9: data=arr->Attrib(VA_Weights_1); sz=sizeof(float)*arr->ComponentCount(VA_Weights_1); break;
    };
    indi_data=arr->Indices();

    if (firsttime) {
#ifdef VAO
      check_error("before");
      ogl->glGenVertexArrays(1,&vao[0]);
  check_error("genvertexarray");
#endif
    }
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
  check_error("bindvertexarray_aftergen");
#endif

    if (firsttime) {
      
      ogl->glGenBuffers(1,&buffers[0]);
      ogl->glGenBuffers(1,&buffers[1]);
      ogl->glGenBuffers(1,&buffers[2]);
      ogl->glGenBuffers(1,&buffers[3]);
      ogl->glGenBuffers(1,&buffers[4]);
      ogl->glGenBuffers(1,&buffers[5]);
      ogl->glGenBuffers(1,&buffers[6]);
      ogl->glGenBuffers(1,&buffers[7]);
      ogl->glGenBuffers(1,&buffers[8]);
      ogl->glGenBuffers(1,&buffers[9]);
      ogl->glGenBuffers(1,&buffers[10]);
      
      ogl->glGenBuffers(1,&indices_buffer);
  check_error("genbuffers");
      
      firsttime=false;
    }
    int num = arr->Num(VertexArrayEnum(i));

#if 1
    std::string arr4[] = { "POSITION", "NORMAL", "TEXCOORD_0", "TEXCOORD_1", "COLOR_0", "COLOR_1", "JOINTS_0", "JOINTS_1", "WEIGHTS_0", "WEIGHTS_1" };
      std::cout << "DATA " << i << "(" << arr4[i] << ")" << std::endl;
      if (joint_s) {
	int s = std::min(int(num*sz/sizeof(unsigned short)),30);
	for(int i=0;i<s;i++) std::cout << ((unsigned short*)data)[i] << ",";
      } else {
	int s = std::min(int(num*sz/sizeof(float)),30);
	for(int i=0;i<s;i++) std::cout << ((float*)data)[i] << ",";
      }
	
      std::cout << std::endl;
#endif
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[i]);
  check_error("bindbuffer");
    ogl->glBufferData(Low_GL_ARRAY_BUFFER, joint_s?num*sz/sizeof(float):num*sz/sizeof(unsigned short), data, Low_GL_STATIC_DRAW);
  check_error("glbufferdata");

    std::string arr3[] = { "POSITION", "NORMAL", "TEXCOORD_0", "TEXCOORD_1", "COLOR_0", "COLOR_1", "JOINTS_0", "JOINTS_1", "WEIGHTS_0", "WEIGHTS_1" };
    std::cout << "NUMDATA " << i << "(" << arr3[i] << ") = " << num << std::endl;

    
    
#ifdef VAO
    int arr2[] = { 0, 1, 3, 13, 2, 14, 11, 15, 12,6 };
    int stride = arr->Stride(VertexArrayEnum(i));
    if (joint_s) {
      ogl->glVertexAttribPointer(arr2[i],sz/sizeof(unsigned short), Low_GL_UNSIGNED_SHORT, Low_GL_FALSE, stride, 0);
    }
    else
      {
      ogl->glVertexAttribPointer(arr2[i],sz/sizeof(float), Low_GL_FLOAT, Low_GL_FALSE, stride, 0);
      }
      check_error("vertexattribpointer");


    if (i==0)
      {
	int num2 = arr->NumIndices();
	
	//std::cout << "NUMINDICES: " << num2 << std::endl;
	unsigned char *indices = arr->Indices();
	ogl->glBindBuffer(Low_GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
  check_error("bindbuffer2");
    size_t sz = sizeof(unsigned short);
    int low = Low_GL_UNSIGNED_SHORT;
    VA_ComponentType tt = arr->IndicesComponentType();
    switch(tt) {
    case VA_BYTE: sz = sizeof(char); low=Low_GL_BYTE; break;
    case VA_UBYTE: sz=sizeof(unsigned char); low=Low_GL_UNSIGNED_BYTE; break;
    case VA_SHORT: sz=sizeof(short); low=Low_GL_SHORT; break;
    case VA_USHORT: sz=sizeof(unsigned short); low=Low_GL_UNSIGNED_SHORT; break;
    case VA_INT: sz=sizeof(int); low=Low_GL_INT; break;
    case VA_UINT: sz=sizeof(unsigned int); low=Low_GL_UNSIGNED_INT; break;
    case VA_FLOAT: sz=sizeof(float); low=Low_GL_FLOAT; break;
    case VA_DOUBLE: sz=sizeof(double); low=Low_GL_DOUBLE; break;
      case VA_INVALID: sz=sizeof(char); low=Low_GL_BYTE; break;
    };


  if (i==0)
      {
    	//ogl->glVertexAttribPointer(14,3,low, Low_GL_FALSE, 0, 0);
     }

    ogl->glBufferData(Low_GL_ELEMENT_ARRAY_BUFFER, num2*sz, indices, Low_GL_STATIC_DRAW);
  check_error("bufferdata2");
      }
    
    
    
#endif

    
    
#ifdef VAO
    ogl->glBindVertexArray(0);
#endif
  check_error("bindvertexarray0");



  }
void Dyn::update(int i)
  {
    //std::cout << "Dyn::Update()" << std::endl;
  OpenglLowApi *ogl = g_low->ogl;
    void *data =0;
    size_t sz=0;
    switch(i) {
    case 0: data=arr->Attrib(VA_Position); sz=sizeof(float)*3; break;
    case 1: data=arr->Attrib(VA_Normal); sz=sizeof(float)*3; break;
    case 2: data=arr->Attrib(VA_TexCoord_0); sz=sizeof(float)*3; break;
    case 3: data=arr->Attrib(VA_TexCoord_1); sz=sizeof(float)*3; break;
    case 4: data=arr->Attrib(VA_Color_0); sz=sizeof(float)*4; break;
    case 5: data=arr->Attrib(VA_Color_1); sz=sizeof(float)*4; break;
    case 6: data=arr->Attrib(VA_Joints_0); sz=sizeof(float)*4;
      if (arr->ComponentType(VA_Joints_0)==VA_USHORT) sz=sizeof(unsigned short)*4;
      break;
    case 7: data=arr->Attrib(VA_Joints_1); sz=sizeof(float)*4; break;
    case 8: data=arr->Attrib(VA_Weights_0); sz=sizeof(float)*4; break;
    case 9: data=arr->Attrib(VA_Weights_1); sz=sizeof(float)*4; break;
    }
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
  int num = arr->Num(VertexArrayEnum(i));
    ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffers[i]);
    ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0,num*sz, data);

    


#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  }
void Dyn::ready()
  {
    //std::cout << "Dyn::Ready()" << std::endl;
    OpenglLowApi *ogl = g_low->ogl;
    check_error("beforeready");
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
  check_error("bindvertexarray_afterready");

#ifndef VAO
  std::cout << "Warning: VAO is not enabled in emscripten => code doesn't work yet" << std::endl;
#endif
  
#ifdef VAO
    int arr[] = { 0, 1, 3, 13, 2, 14, 11, 15, 12,6 };
    for(int i=0;i<9;i++)
      {
	if (enabled[i]) {
	  int index = arr[i];
	  ogl->glEnableVertexAttribArray(index);
	  check_error("enablevertexattribarray");
	}
      }
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  check_error("bindvertexarray_afterready2");

  }
void Dyn::render()
  {
    // std::cout << "Dyn::Render()" << std::endl;
  OpenglLowApi *ogl = g_low->ogl;
  check_error("beforerender");
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#endif
  check_error("bindvertexarray_render");

  int s = textures.size();
    for(int i=0;i<s;i++)
      {
       GameApi::TXID id = textures[i];
       ogl->glActiveTexture(Low_GL_TEXTURE0 + i);
  check_error("activetexture");
       ogl->glBindTexture(Low_GL_TEXTURE_2D, id.id);
  check_error("bindtexture");
       }


#ifdef VAO
    int arr3[] = { 0, 1, 3, 13, 2, 14, 11, 15, 12,6 };
    for(int i=0;i<9;i++)
      {
	if (enabled[i]) {
	  int index = arr3[i];
	  ogl->glEnableVertexAttribArray(index);
	  check_error("enablevertexattribarray");
	}
      }
#endif

    
    //int arr2[] = { 0, 1, 2, 13, 3, 14, 11, 15, 12,16 };
    int num = arr->Num(VA_Position);
    unsigned char *indices = arr->Indices();
    if (indices) {
      //std::cout << "INDICES" << std::endl;
      //for(int i=0;i<num/3;i++)
      //std::cout << indices[i] << ",";
      //std::cout << std::endl;
      int numindices = arr->NumIndices();
      //std::cout << "NUMINDICES: " << numindices << std::endl;
      ogl->glBindBuffer(Low_GL_ELEMENT_ARRAY_BUFFER, indices_buffer);


    size_t sz = sizeof(unsigned short);
    int low = Low_GL_UNSIGNED_SHORT;
    VA_ComponentType tt = arr->IndicesComponentType();
    switch(tt) {
    case VA_BYTE: sz = sizeof(char); low=Low_GL_UNSIGNED_BYTE; break;
    case VA_UBYTE: sz=sizeof(unsigned char); low=Low_GL_UNSIGNED_BYTE; break;
    case VA_SHORT: sz=sizeof(short); low=Low_GL_UNSIGNED_SHORT; break;
    case VA_USHORT: sz=sizeof(unsigned short); low=Low_GL_UNSIGNED_SHORT; break;
    case VA_INT: sz=sizeof(int); low=Low_GL_UNSIGNED_INT; break;
    case VA_UINT: sz=sizeof(unsigned int); low=Low_GL_UNSIGNED_INT; break;
      case VA_FLOAT: sz=sizeof(float); low=Low_GL_FLOAT; break;
      case VA_DOUBLE: sz=sizeof(double); low=Low_GL_DOUBLE; break;
      case VA_INVALID: sz=sizeof(char); low=Low_GL_UNSIGNED_BYTE; break;
    };

    std::cout << "DRAWELEMENTS: " << numindices << " " << low << std::endl;
    if (numindices/3>0)
      ogl->glDrawElements(Low_GL_TRIANGLES, numindices/3, low, 0);
  check_error("drawelements");
    } else {
      ogl->glDrawArrays(Low_GL_TRIANGLES, 0, num/3);
  check_error("drawarrays");
    }



#ifdef VAO
    int arr4[] = { 0, 1, 3, 13, 2, 14, 11, 15, 12,6 };
    for(int i=0;i<9;i++)
      {
	if (enabled[i]) {
	  int index = arr4[i];
	  ogl->glDisableVertexAttribArray(index);
	  check_error("disablevertexattribarray");
	}
      }
#endif

#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  check_error("bindvertexarray0");


    int s2 = textures.size();
    for(int i=0;i<s2;i++)
      {
       ogl->glActiveTexture(Low_GL_TEXTURE0 + i);
       ogl->glBindTexture(Low_GL_TEXTURE_2D, 0);
    }
  check_error("bindtexture0");

  }

#endif
