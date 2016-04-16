
#ifndef VERTEXARRAY_HH
#define VERTEXARRAY_HH

#include <vector>
#include <map>
#include "VectorTools.hh"
#include "Effect.hh"
#include <pthread.h>


class VertexArraySet
{
public:
  int texture_id;
  int bm_id;
  VertexArraySet() : texture_id(-1) { }
  VertexArraySet(const VertexArraySet &s)
  {
    std::map<int, Polys*>::iterator it = s.m_set.begin();
    for(;it!=s.m_set.end();it++)
      {
	m_set[(*it).first] = new Polys(*(*it).second);
      }
    bm_id = 0;
  }
  void set_bm_id(int id) { bm_id=0; }
  int get_bm_id() const { return bm_id; }
  ~VertexArraySet();
  void set_reserve(int id, int tri_count, int quad_count);
  // id is the vertex array num to be used 0 = beginning, 1 = end
  // num is the number of points (all calls should share same num)
  void push_poly(int id, int num, Point *points);
  void push_poly2(int id, int num, Point *points);
  void push_normal(int id, int num, Vector *vectors);
  void push_attrib(int id, int attrib_id, int num, float *attribs);
  void push_attribi(int id, int attrib_id, int num, int *attribi);
  void push_color(int id, int num, unsigned int *colors);
  void push_texcoord(int id, int num, Point2d *points);

  void split_color(std::vector<float> &vec, unsigned int color);

  // way to get data out
  int tri_count(int id) const { return m_set[id]->tri_polys.size(); }
  int tri_count2(int id) const { return m_set[id]->tri_polys2.size(); }
  Point *tri_polys(int id) { return tri_count(id) ? &m_set[id]->tri_polys[0] : NULL; }
  const Point *tri_polys2(int id) const { return tri_count2(id) ? &m_set[id]->tri_polys2[0] : NULL; }
  int quad_count(int id) const { return m_set[id]->quad_polys.size(); }
  int quad_count2(int id) const { return m_set[id]->quad_polys2.size(); }
  const Point *quad_polys(int id) const { return quad_count(id) ? &m_set[id]->quad_polys[0] : NULL; }
  const Point *quad_polys2(int id) const { return quad_count2(id) ? &m_set[id]->quad_polys2[0] : NULL; }
#if 0
  int poly_count(int id) const { return m_set[id]->poly_polys.size(); }
  int poly2_count(int id, int i) const { return m_set[id]->poly_polys[i].size(); }
  const Point *poly_polys(int id, int i) const { return poly2_count(id,i)? &m_set[id]->poly_polys[i][0] : NULL; }
#endif

  int tri_normal_count(int id) const { return m_set[id]->tri_normals.size(); }
  const Vector *tri_normal_polys(int id) const { return tri_normal_count(id) ? &m_set[id]->tri_normals[0] : NULL; }
  int quad_normal_count(int id) const { return m_set[id]->quad_normals.size(); }
  const Vector *quad_normal_polys(int id) const { return quad_normal_count(id) ? &m_set[id]->quad_normals[0] : NULL; }
#if 0
  int poly_normal_count(int id) const { return m_set[id]->poly_normals.size(); }
  int poly2_normal_count(int id, int i) const { return m_set[id]->poly_normals[i].size(); }
  const Vector *poly_normal_polys(int id, int i) const { return poly2_normal_count(id,i) ? &m_set[id]->poly_normals[i][0] : NULL; }
#endif

#if 0
  int tri_attrib_count(int id, int attrib_id) const { return m_set[id]->tri_attribs[attrib_id].size(); }
  const float *tri_attrib_polys(int id, int attrib_id) const { return tri_attrib_count(id,attrib_id) ? &m_set[id]->tri_attribs[attrib_id][0] : NULL; }
  int quad_attrib_count(int id, int attrib_id) const { return m_set[id]->quad_attribs[attrib_id].size(); }
  const float *quad_attrib_polys(int id, int attrib_id) const { return quad_attrib_count(id, attrib_id) ? &m_set[id]->quad_attribs[attrib_id][0] : NULL; }
  int poly_attrib_count(int id, int attrib_id) const { return m_set[id]->poly_attribs[attrib_id].size(); }
  int poly2_attrib_count(int id, int attrib_id, int i) const { return m_set[id]->poly_attribs[attrib_id][i].size(); }
  const float *poly_attrib_polys(int id, int attrib_id, int i) const { return poly2_attrib_count(id, attrib_id, i) ? &m_set[id]->poly_attribs[attrib_id][i][0] : NULL; }

  int tri_attribi_count(int id, int attrib_id) const { return m_set[id]->tri_attribsi[attrib_id].size(); }
  const int *tri_attribi_polys(int id, int attrib_id) const { return tri_attribi_count(id,attrib_id) ? &m_set[id]->tri_attribsi[attrib_id][0] : NULL; }
  int quad_attribi_count(int id, int attrib_id) const { return m_set[id]->quad_attribsi[attrib_id].size(); }
  const int *quad_attribi_polys(int id, int attrib_id) const { return quad_attribi_count(id,attrib_id) ? &m_set[id]->quad_attribsi[attrib_id][0] : NULL; }
  int poly_attribi_count(int id, int attrib_id) const { return m_set[id]->poly_attribsi[attrib_id].size(); }
  int poly2_attribi_count(int id, int attrib_id, int i) const { return m_set[id]->poly_attribsi[attrib_id][i].size(); }
  const int *poly_attribi_polys(int id, int attrib_id, int i) const { return poly2_attribi_count(id, attrib_id, i) ? &m_set[id]->poly_attribsi[attrib_id][i][0] : NULL; }

#endif

  int tri_color_count(int id) const { return m_set[id]->tri_color.size(); }
  const float *tri_color_polys(int id) const { return tri_color_count(id) ? &m_set[id]->tri_color[0] : NULL; }
  int quad_color_count(int id) const { return m_set[id]->quad_color.size(); }
  const float *quad_color_polys(int id) const { return quad_color_count(id) ? &m_set[id]->quad_color[0] : NULL; }
#if 0
  int poly_color_count(int id) const { return m_set[id]->poly_color.size(); }
  int poly2_color_count(int id, int i) const { return m_set[id]->poly_color[i].size(); }
  const float *poly_color_polys(int id, int i) const { return poly2_color_count(id,i) ? &m_set[id]->poly_color[i][0] : NULL; }
#endif

  int tri_texcoord_count(int id) const { return m_set[id]->tri_texcoord.size(); }
  Point2d *tri_texcoord_polys(int id) { return tri_texcoord_count(id) ? &m_set[id]->tri_texcoord[0] : NULL; }
  int quad_texcoord_count(int id) const { return m_set[id]->quad_texcoord.size(); }
  Point2d *quad_texcoord_polys(int id) { return quad_texcoord_count(id) ? &m_set[id]->quad_texcoord[0] : NULL; }
#if 0
  int poly_texcoord_count(int id) const { return m_set[id]->poly_texcoord.size(); }
  int poly2_texcoord_count(int id, int i) const { return m_set[id]->poly_texcoord[i].size(); }
  const Point2d *poly_texcoord_polys(int id, int i) const { return poly2_texcoord_count(id,i) ? &m_set[id]->poly_texcoord[i][0] : NULL; }
#endif  

  void check_m_set(int id);
  void free_memory();  
  void explode(int id, Point p, float dist);
public:
  struct Polys {
    std::vector<Point> tri_polys;
    std::vector<Point> quad_polys;
    std::vector<Point> tri_polys2;
    std::vector<Point> quad_polys2;
    
    std::vector<Vector> tri_normals;
    std::vector<Vector> quad_normals;

    std::vector<float> tri_color;
    std::vector<float> quad_color;
    std::vector<Point2d> tri_texcoord;
    std::vector<Point2d> quad_texcoord;
  };
public:
  static void append_to_polys(VertexArraySet::Polys &target, const VertexArraySet::Polys &source);

  mutable std::map<int, Polys*> m_set;
};

class RenderVertexArray
{
public:
  RenderVertexArray(VertexArraySet &s) : s(s) { }
  void prepare(int id);
  void update(int id);
  void render(int id);
  void render_instanced(int id, Point *positions, int size);
  void del();
  ~RenderVertexArray() { del(); }
private:
  VertexArraySet &s;
  unsigned int buffers[5];
  unsigned int vao[2];
  unsigned int buffers2[5];
  int tri_count;
  int quad_count;
  unsigned int pos_buffer;
};

class RenderVertexArray2
{
public:
  RenderVertexArray2(VertexArraySet &s1, VertexArraySet &s2) : s1(s1), s2(s2) { }
  void prepare(int id);
  void render(int id, int a1, int a2, int a3, int a4,
	      int aa1, int aa2, int aa3, int aa4);
private:
  VertexArraySet &s1;
  VertexArraySet &s2;
};

class FaceCollectionVertexArray2
{
public:
  FaceCollectionVertexArray2(const FaceCollection &coll, VertexArraySet &s) : coll(coll), s(s) { }
  void reserve(int id)
  {
    int ss = coll.NumFaces();
    int tri_count=4;
    int quad_count=4;
    for(int i=0;i<ss;i++)
      {
	int w = coll.NumPoints(i);
	if (w==3)
	  {
	    tri_count+=3;
	  }
	else if (w==4)
	  {
	    quad_count+=6;
	  }
      }
    s.set_reserve(id, tri_count, quad_count);
  }
  void copy(int start_range, int end_range, std::vector<int> attribs = std::vector<int>(), std::vector<int> attribsi = std::vector<int>())
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

	    for (int k=0;k<(int)attribs.size();k++)
	      {
		ai[k][j] = coll.AttribI(i,j,attribs[k]);
	      }
#endif
	    //std::cout << "COL!" << i << std::endl;
	    c[j] = coll.Color(i,j);
	    //std::cout << "Tex!" << i << std::endl;
	    tex[j] = coll.TexCoord(i,j);
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
	  s.push_attrib(0, k, w, &a[k][0]);
	for (int k=0;k<(int)attribsi.size();k++)
	  s.push_attribi(0, k, w, &ai[k][0]);
#endif
	//  std::cout << "push_c!" << std::endl;
	s.push_color(0, w, &c[0]);
	//  std::cout << "push_texcoord!" << std::endl;
	s.push_texcoord(0, w, &tex[0]);

      }
    //std::cout << "Copy returns" << std::endl;
  }

private:
  Point p[200];
  Point p2[200];
  Vector v[200];
#if 0
  float a[30][200];
  int ai[30][200];
#endif
  unsigned int c[200];
  Point2d tex[200];

  const FaceCollection &coll;
  VertexArraySet &s;
};
struct ThreadInfo
{
  pthread_t thread_id;
  VertexArraySet *set;
  FaceCollectionVertexArray2 *va;
  int start_range;
  int end_range;
};
void *thread_func(void *data);
class ThreadedPrepare
{
public:
  ThreadedPrepare(FaceCollection *faces) : faces(faces) { }
  int push_thread(int start_range, int end_range)
  {
    //std::cout << "Thread " << start_range << " " << end_range << std::endl;
    VertexArraySet *s = new VertexArraySet;
    s->set_reserve(0, end_range-start_range, end_range-start_range);
    sets.push_back(s);
    FaceCollectionVertexArray2 *va = new FaceCollectionVertexArray2(*faces, *s);
    va2.push_back(va);
    ThreadInfo *info = new ThreadInfo;
    info->set = s;
    info->va = va;
    info->start_range = start_range;
    info->end_range = end_range;
    ti.push_back(info);

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 3000000);
    //std::cout << "phread_create" << std::endl;
    pthread_create(&info->thread_id, &attr, &thread_func, (void*)info);
    //std::cout << "pthread_create_return: " << val << std::endl;
    pthread_attr_destroy(&attr);
    //std::cout << "returning: " << sets.size()-1 << std::endl;
    return sets.size()-1;
  }
  void join(int id)
  {
    void *res;
    //std::cout << "phread_join" << id << std::endl;
    pthread_join(ti[id]->thread_id, &res);
  }
  VertexArraySet *collect()
  {
    //std::cout << "collect" << std::endl;

    VertexArraySet *res = new VertexArraySet;
    res->set_reserve(0, 1,1);
    int s = sets.size();
    for(int i=0;i<s;i++)
      {
	VertexArraySet * vs = sets[i];
	VertexArraySet::append_to_polys(*res->m_set[0], *vs->m_set[0]);
      }
    return res;
  }
  ~ThreadedPrepare()
  {
    //std::cout << "threaded_prepare dtor" << std::endl;

    int s = sets.size();
    for(int i=0;i<s;i++)
      {
	delete sets[i];
	delete va2[i];
	delete ti[i];
      }
  }
private:
  std::vector<VertexArraySet *> sets;
  std::vector<FaceCollectionVertexArray2 *> va2;
  std::vector<ThreadInfo *> ti;
  FaceCollection *faces;
};


#endif
