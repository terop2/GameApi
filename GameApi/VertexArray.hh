
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
  bool is_texture() { return texture_id!=-1; }
  bool is_array_texture() { return texture_id>= 700000; }
  VertexArraySet() : texture_id(-1) { }
  VertexArraySet(const VertexArraySet &s)
  {
    std::map<int, Polys*>::iterator it = s.m_set.begin();
    for(;it!=s.m_set.end();it++)
      {
	m_set[(*it).first] = new Polys(*(*it).second);
      }
    bm_id = 0;
    texture_id = s.texture_id;
  }
  void clear_arrays()
  {
    std::map<int, Polys*>::iterator it = m_set.begin();
    for(;it!=m_set.end();it++)
      {
	delete m_set[(*it).first];
	m_set[(*it).first] = new Polys;
      }
  }
  void set_bm_id(int id) { bm_id=0; }
  int get_bm_id() const { return bm_id; }
  ~VertexArraySet();
  void set_reserve(int id, int tri_count, int quad_count, int poly_count);
  void free_reserve(int id);
  // id is the vertex array num to be used 0 = beginning, 1 = end
  // num is the number of points (all calls should share same num)
  void push_poly(int id, int num, Point *points);
  void push_poly2(int id, int num, Point *points);
  void push_normal(int id, int num, Vector *vectors);
  void push_attrib(int id, int attrib_id, int num, float *attribs);
  void push_attribi(int id, int attrib_id, int num, int *attribi);
  void push_color(int id, int num, unsigned int *colors);
  void push_texcoord(int id, int num, Point *points);

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
  int poly_count_f(int id) const { return m_set[id]->poly_polys.size(); }
  const Point *poly_polys(int id) const { return poly_count_f(id)? &m_set[id]->poly_polys[0] : NULL; }
  int poly_count2(int id) const { return m_set[id]->poly_polys2.size(); }
  const Point *poly_polys2(int id) const { return poly_count2(id)? &m_set[id]->poly_polys2[0] : NULL; }

#if 0
  int poly2_count(int id, int i) const { return m_set[id]->poly_polys[i].size(); }
#endif

  int tri_normal_count(int id) const { return m_set[id]->tri_normals.size(); }
  const Vector *tri_normal_polys(int id) const { return tri_normal_count(id) ? &m_set[id]->tri_normals[0] : NULL; }
  int quad_normal_count(int id) const { return m_set[id]->quad_normals.size(); }
  const Vector *quad_normal_polys(int id) const { return quad_normal_count(id) ? &m_set[id]->quad_normals[0] : NULL; }
  int poly_normal_count(int id) const { return m_set[id]->poly_normals.size(); }
  const Vector *poly_normal_polys(int id) const { return poly_normal_count(id) ? &m_set[id]->poly_normals[0] : NULL; }

#if 0
  int poly2_normal_count(int id, int i) const { return m_set[id]->poly_normals[i].size(); }
#endif

#if 1
  int tri_attrib_count(int id, int attrib_id) const { return m_set[id]->tri_attribs[attrib_id].size(); }
  const float *tri_attrib_polys(int id, int attrib_id) const { return tri_attrib_count(id,attrib_id) ? &m_set[id]->tri_attribs[attrib_id][0] : NULL; }
  int quad_attrib_count(int id, int attrib_id) const { return m_set[id]->quad_attribs[attrib_id].size(); }
  const float *quad_attrib_polys(int id, int attrib_id) const { return quad_attrib_count(id, attrib_id) ? &m_set[id]->quad_attribs[attrib_id][0] : NULL; }
  int poly_attrib_count(int id, int attrib_id) const { return m_set[id]->poly_attribs[attrib_id].size(); }
  int poly2_attrib_count(int id, int attrib_id) const { return m_set[id]->poly_attribs[attrib_id].size(); }
  const float *poly_attrib_polys(int id, int attrib_id) const { return poly2_attrib_count(id, attrib_id) ? &m_set[id]->poly_attribs[attrib_id][0] : NULL; }

  int tri_attribi_count(int id, int attrib_id) const { return m_set[id]->tri_attribsi[attrib_id].size(); }
  const int *tri_attribi_polys(int id, int attrib_id) const { return tri_attribi_count(id,attrib_id) ? &m_set[id]->tri_attribsi[attrib_id][0] : NULL; }
  int quad_attribi_count(int id, int attrib_id) const { return m_set[id]->quad_attribsi[attrib_id].size(); }
  const int *quad_attribi_polys(int id, int attrib_id) const { return quad_attribi_count(id,attrib_id) ? &m_set[id]->quad_attribsi[attrib_id][0] : NULL; }
  int poly_attribi_count(int id, int attrib_id) const { return m_set[id]->poly_attribsi[attrib_id].size(); }
  int poly2_attribi_count(int id, int attrib_id) const { return m_set[id]->poly_attribsi[attrib_id].size(); }
  const int *poly_attribi_polys(int id, int attrib_id) const { return poly2_attribi_count(id, attrib_id) ? &m_set[id]->poly_attribsi[attrib_id][0] : NULL; }

#endif

  int tri_color_count(int id) const { return m_set[id]->tri_color.size(); }
  const float *tri_color_polys(int id) const { return tri_color_count(id) ? &m_set[id]->tri_color[0] : NULL; }
  int quad_color_count(int id) const { return m_set[id]->quad_color.size(); }
  const float *quad_color_polys(int id) const { return quad_color_count(id) ? &m_set[id]->quad_color[0] : NULL; }
  int poly_color_count(int id) const { return m_set[id]->poly_color.size(); }
  const float *poly_color_polys(int id) const { return poly_color_count(id) ? &m_set[id]->poly_color[0] : NULL; }

#if 0
  int poly2_color_count(int id, int i) const { return m_set[id]->poly_color[i].size(); }
#endif

  int tri_texcoord_count(int id) const { return m_set[id]->tri_texcoord.size(); }
  Point *tri_texcoord_polys(int id) { return tri_texcoord_count(id) ? &m_set[id]->tri_texcoord[0] : NULL; }
  int quad_texcoord_count(int id) const { return m_set[id]->quad_texcoord.size(); }
  Point *quad_texcoord_polys(int id) { return quad_texcoord_count(id) ? &m_set[id]->quad_texcoord[0] : NULL; }
  int poly_texcoord_count(int id) const { return m_set[id]->poly_texcoord.size(); }
  const Point *poly_texcoord_polys(int id) const { return poly_texcoord_count(id) ? &m_set[id]->poly_texcoord[0] : NULL; }

#if 0

  int poly2_texcoord_count(int id, int i) const { return m_set[id]->poly_texcoord[i].size(); }
#endif  

  void check_m_set(int id);
  void free_memory();  
  void explode(int id, Point p, float dist);
  void print_stat(int id);
  void clone(int id_source, int id_target);
  void apply_change(DynamicChange *change, int id_source, int id_target, MainLoopEnv &e);
public:
  struct Polys {
    std::vector<Point> tri_polys;
    std::vector<Point> quad_polys;
    std::vector<Point> poly_polys;
    std::vector<Point> tri_polys2;
    std::vector<Point> quad_polys2;
    std::vector<Point> poly_polys2;
    
    std::vector<Vector> tri_normals;
    std::vector<Vector> quad_normals;
    std::vector<Vector> poly_normals;

    std::vector<float> tri_color;
    std::vector<float> quad_color;
    std::vector<float> poly_color;

    std::vector<Point> tri_texcoord;
    std::vector<Point> quad_texcoord;
    std::vector<Point> poly_texcoord;

    std::map<int,std::vector<float> > tri_attribs;
    std::map<int,std::vector<float> > quad_attribs;
    std::map<int,std::vector<float> > poly_attribs;

    std::map<int,std::vector<int> > tri_attribsi;
    std::map<int,std::vector<int> > quad_attribsi;
    std::map<int,std::vector<int> > poly_attribsi;

  };
public:
  static void append_to_polys(VertexArraySet::Polys &target, const VertexArraySet::Polys &source);

  mutable std::map<int, Polys*> m_set;
};

struct RenderVertexArray_bufferids
{
  unsigned int buffers[5];
  unsigned int vao[3];
  unsigned int buffers2[5];
  unsigned int buffers3[5];
  unsigned int pos_buffer;
  std::vector<unsigned int> attrib_buffer;
  std::vector<unsigned int> attrib_buffer2;
  std::vector<unsigned int> attrib_buffer3;
  std::vector<unsigned int> attribi_buffer;
  std::vector<unsigned int> attribi_buffer2;
  std::vector<unsigned int> attribi_buffer3;
};

struct Counts {
  int tri_count;
  int quad_count;
  int poly_count;
};

Counts CalcOffsets(FaceCollection *coll, int start);
Counts CalcCounts(FaceCollection *coll, int start, int end);

class RenderVertexArray
{
public:
  RenderVertexArray(VertexArraySet &s) : s(s),nodelete(false) { }
  void prepare(int id, bool isnull=false, int tri_count=-1, int quad_count=-1, int poly_count=-1);
  void update(int id);
  void update_tri(int id, int buffer_id, int start, int end);
  void render(int id);
  void prepare_instanced(int id, Point *positions, int size);
  void render_instanced(int id, Point *positions, int size);
  void update_buffers(RenderVertexArray_bufferids ids);
  void fetch_buffers(RenderVertexArray_bufferids &ids);
  void set_no_delete(bool b) { nodelete=b; }
  void del();
  ~RenderVertexArray() { if (!nodelete) del(); }
private:
  VertexArraySet &s;
  unsigned int buffers[5];
  unsigned int vao[3];
  unsigned int buffers2[5];
  unsigned int buffers3[5];
  int tri_count;
  int quad_count;
  int poly_count;
  unsigned int pos_buffer;
  std::vector<unsigned int> attrib_buffer;
  std::vector<unsigned int> attrib_buffer2;
  std::vector<unsigned int> attrib_buffer3;
  std::vector<unsigned int> attribi_buffer;
  std::vector<unsigned int> attribi_buffer2;
  std::vector<unsigned int> attribi_buffer3;
  bool nodelete;
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
  void reserve_fixed(int id, int count) {
    //int ss = coll.NumFaces();
    int tri_count=count*3;
    int quad_count=count*4;
    int poly_count=count*3;
    s.set_reserve(id, tri_count, quad_count, poly_count);
  }
  void reserve(int id)
  {
    int ss = coll.NumFaces();
    int tri_count=4;
    int quad_count=4;
    int poly_count=4;
    for(int i=0;i<ss;i++)
      {
	int w = coll.NumPoints(i);
	if (w==3)
	  {
	    tri_count+=3;
	  }
	else if (w==4)
	  {
	    quad_count+=4;
	  }
	else
	  {
	    poly_count+=3;
	  }
      }
    s.set_reserve(id, tri_count, quad_count, poly_count);
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

#if 1
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

#if 1
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

private:
  Point p[200];
  Point p2[200];
  Vector v[200];
#if 1
  float a[30][200];
  int ai[30][200];
#endif
  unsigned int c[200];
  Point tex[200];

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
  std::vector<int> attrib;
  std::vector<int> attribi;
};
void *thread_func(void *data);
class ThreadedPrepare
{
public:
  ThreadedPrepare(FaceCollection *faces) : faces(faces) { }
  int push_thread(int start_range, int end_range, std::vector<int> attrib=std::vector<int>(), std::vector<int> attribi=std::vector<int>())
  {
    //std::cout << "Thread " << start_range << " " << end_range << std::endl;
    VertexArraySet *s = new VertexArraySet;
    s->set_reserve(0, end_range-start_range, end_range-start_range, end_range-start_range);
    sets.push_back(s);
    FaceCollectionVertexArray2 *va = new FaceCollectionVertexArray2(*faces, *s);
    va2.push_back(va);
    ThreadInfo *info = new ThreadInfo;
    info->set = s;
    info->va = va;
    info->start_range = start_range;
    info->end_range = end_range;
    info->attrib = attrib;
    info->attribi = attribi;
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
  void transfer_to_gpu_mem(RenderVertexArray &r, int rend_id, int buf, int start, int end) {
    int s = sets.size();
    RenderVertexArray_bufferids ids;
    r.fetch_buffers(ids);
    for(int i=0;i<s;i++)
      {
	VertexArraySet * vs = sets[i];
	RenderVertexArray rend(*vs);
	rend.update_buffers(ids);
	rend.set_no_delete(true);
	rend.update_tri(rend_id, buf, start,end);
      }

  }
  VertexArraySet *collect()
  {
    //std::cout << "collect" << std::endl;

    VertexArraySet *res = new VertexArraySet;
    res->set_reserve(0, 1,1,1);
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
