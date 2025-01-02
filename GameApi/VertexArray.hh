
#ifndef VERTEXARRAY_HH
#define VERTEXARRAY_HH

#define NO_PREPARE_RENDER 1

#include <vector>
#include <map>
#include "MyMap.hh"
#include "VectorTools.hh"
#include "Effect.hh"
#include "GameApi.hh"
//#include "GameApi_low.hh"
#ifndef HAS_PTHREAD
#define HAS_PTHREAD 1
#include <pthread.h>
#undef PTW32_LEVEL
#undef PTW32_SCHED_LEVEL
#endif
struct LowApi;
extern LowApi *g_low;

#ifdef NO_PREPARE_RENDER

class Dyn : public VertexArrayDyn
{
public:
  Dyn(VertexArrays *arr);
  void prepare(int i);
  void update(int i);
  void ready();
  void render();
  void set_texture(std::vector<GameApi::TXID> vec);
private:
  VertexArrays *arr;
  bool firsttime;
  unsigned int vao[1];
  unsigned int buffers[11];
  unsigned int indices_buffer;
  bool enabled[11];
  std::vector<GameApi::TXID> textures;
};
#endif


class VertexArraySet
{
public:
  int texture_id;
  std::vector<int> texture_many_ids;
  int bm_id;
  bool is_texture() { return texture_id!=-1; }
  bool is_array_texture() { return texture_id>= 700000; }
  bool is_many_texture() { return texture_many_ids.size()!=0; }
  VertexArraySet() : texture_id(-1),bm_id(0) { ref.numfaces=0;
    ref.color=0;
    ref.facepoint=0;
    ref.facepoint2=0;
    ref.texcoords=0;
    ref.joints=0;
    ref.weights=0;

  }
  VertexArraySet(const VertexArraySet &s)
  {
    ref.numfaces=0;
    ref.color=0;
    ref.facepoint=0;
    ref.facepoint2=0;
    ref.texcoords=0;
    ref.joints=0;
    ref.weights=0;
    std::mymap<int, Polys*>::iterator it = s.m_set.begin();
    for(;it!=s.m_set.end();it++)
      {
	m_set[(*it).first] = new Polys(*(*it).second);
      }
    bm_id = 0;
    texture_id = s.texture_id;
    texture_many_ids = s.texture_many_ids;
    has_normal = s.has_normal;
    has_attrib = s.has_attrib;
    has_color = s.has_color;
    has_texcoord = s.has_texcoord;
    has_skeleton = s.has_skeleton;
  }
  void SetRef(FaceBufferRef r) { ref = r; }
  void clear_poly_and_poly2(int id);
  void clear_arrays()
  {
    std::mymap<int, Polys*>::iterator it = m_set.begin();
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
  void push_poly_with_indices(int id, int num, Point *points, unsigned int *indexes, int points_num);
  void push_poly2_with_indices(int id, int num, Point *points, unsigned int *indexes, int points_num);

  void push_normal_with_indices(int id, int num, Vector *vectors, unsigned int *indexes, int points_num);
  void push_attrib_with_indices(int id, int attrib_id, int num, float *attribs, unsigned int *indexes);
  void push_attribi_with_indices(int id, int attrib_id, int num, int *attribi, unsigned int *indexes);
  void push_color_with_indices(int id, int num, unsigned int *colors, unsigned int *indexes, int points_num);
  void push_texcoord_with_indices(int id, int num, Point *points, unsigned int *indexes, int points_num);
  void push_joint_with_indices(int id, int num, VEC4 *points, unsigned int *indexes, int points_num);
  void push_weight_with_indices(int id, int num, VEC4 *points, unsigned int *indexes, int points_num); 

  void push_poly(int id, int num, Point *points);
  void push_poly2(int id, int num, Point *points);
  void push_normal(int id, int num, Vector *vectors);
  void push_attrib(int id, int attrib_id, int num, float *attribs);
  void push_attribi(int id, int attrib_id, int num, int *attribi);
  void push_color(int id, int num, unsigned int *colors);
  void push_texcoord(int id, int num, Point *points);
  void push_joint(int id, int num, VEC4 *points);
  void push_weight(int id, int num, VEC4 *points); 
  
  void split_color(std::vector<float> &vec, unsigned int color);
  void split_color2(std::vector<float> &vec, unsigned int color);

  // way to get data out
  int tri_count(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices;
    return m_set[id]->tri_polys.size(); }
  int tri_count2(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices;
    return m_set[id]->tri_polys2.size(); }
  Point *tri_polys(int id) {
    //if (ref.numfaces!=0) return ref.facepoint;
    return tri_count(id) ? &m_set[id]->tri_polys[0] : NULL;
  }
  const Point *tri_polys2(int id) const {
    //if (ref.numfaces!=0) return ref.facepoint2;
    return tri_count2(id) ? &m_set[id]->tri_polys2[0] : NULL; }
  int quad_count(int id) const {
    //if (ref.numfaces!=0) return 0; // QUADS NOT SUPPORTED IN GLTF
    return m_set[id]->quad_polys.size(); }
  int quad_count2(int id) const {
    //if (ref.numfaces!=0) return 0;
    return m_set[id]->quad_polys2.size(); }
  const Point *quad_polys(int id) const { return quad_count(id) ? &m_set[id]->quad_polys[0] : NULL; }
  const Point *quad_polys2(int id) const { return quad_count2(id) ? &m_set[id]->quad_polys2[0] : NULL; }
  int poly_count_f(int id) const { return m_set[id]->poly_polys.size(); }
  const Point *poly_polys(int id) const { return poly_count_f(id)? &m_set[id]->poly_polys[0] : NULL; }
  int poly_count2(int id) const { return m_set[id]->poly_polys2.size(); }
  const Point *poly_polys2(int id) const { return poly_count2(id)? &m_set[id]->poly_polys2[0] : NULL; }

#if 0
  int poly2_count(int id, int i) const { return m_set[id]->poly_polys[i].size(); }
#endif

  const unsigned char *indices_char() const { return ref.indices_char; }
  const unsigned short *indices_short() const { return ref.indices_short; }
  const unsigned int *indices_int() const { return ref.indices_int; }
  
  int tri_normal_count(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices;    
    return m_set[id]->tri_normals.size(); }
  const Vector *tri_normal_polys(int id) const {
    //if (ref.numfaces!=0) return ref.pointnormal;
    return tri_normal_count(id) ? &m_set[id]->tri_normals[0] : NULL; }
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

  int tri_color_count(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices; TODO, COLOURS NOT GOING OK IN GLTF    
    return m_set[id]->tri_color.size(); }
  const float *tri_color_polys(int id) const {
    //if (ref.numfaces!=0) return ref.color; TODO, COLOURS NOT GOING OK IN GLTF
    return tri_color_count(id) ? &m_set[id]->tri_color[0] : NULL; }
  int quad_color_count(int id) const { return m_set[id]->quad_color.size(); }
  const float *quad_color_polys(int id) const { return quad_color_count(id) ? &m_set[id]->quad_color[0] : NULL; }
  int poly_color_count(int id) const { return m_set[id]->poly_color.size(); }
  const float *poly_color_polys(int id) const { return poly_color_count(id) ? &m_set[id]->poly_color[0] : NULL; }



  int tri_joint_count(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices;
    return m_set[id]->tri_joint.size(); }
  const VEC4 *tri_joint_polys(int id) const {
    //if (ref.numfaces!=0) return ref.joints;

    return tri_joint_count(id) ? &m_set[id]->tri_joint[0] : NULL; }
  int quad_joint_count(int id) const { return m_set[id]->quad_joint.size(); }
  const VEC4 *quad_joint_polys(int id) const { return quad_joint_count(id) ? &m_set[id]->quad_joint[0] : NULL; }
  int poly_joint_count(int id) const { return m_set[id]->poly_joint.size(); }
  const VEC4 *poly_joint_polys(int id) const { return poly_joint_count(id) ? &m_set[id]->poly_joint[0] : NULL; }


  int tri_weight_count(int id) const {
    //if (ref.numfaces!=0) return ref.numvertices;
    return m_set[id]->tri_weight.size(); }
  const VEC4 *tri_weight_polys(int id) const {
    //if (ref.numfaces!=0) return ref.weights;
    return tri_weight_count(id) ? &m_set[id]->tri_weight[0] : NULL; }
  int quad_weight_count(int id) const { return m_set[id]->quad_weight.size(); }
  const VEC4 *quad_weight_polys(int id) const { return quad_weight_count(id) ? &m_set[id]->quad_weight[0] : NULL; }
  int poly_weight_count(int id) const { return m_set[id]->poly_weight.size(); }
  const VEC4 *poly_weight_polys(int id) const { return poly_weight_count(id) ? &m_set[id]->poly_weight[0] : NULL; }

  
  
  
#if 0
  int poly2_color_count(int id, int i) const { return m_set[id]->poly_color[i].size(); }
#endif

  int tri_texcoord_count(int id) const { return m_set[id]->tri_texcoord.size(); }
  Point *tri_texcoord_polys(int id) {
    //if (ref.numfaces!=0) return ref.texcoords;
    return tri_texcoord_count(id) ? &m_set[id]->tri_texcoord[0] : NULL; }
  int quad_texcoord_count(int id) const { return m_set[id]->quad_texcoord.size(); }
  Point *quad_texcoord_polys(int id) { return quad_texcoord_count(id) ? &m_set[id]->quad_texcoord[0] : NULL; }
  int poly_texcoord_count(int id) const { return m_set[id]->poly_texcoord.size(); }
  const Point *poly_texcoord_polys(int id) const { return poly_texcoord_count(id) ? &m_set[id]->poly_texcoord[0] : NULL; }


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

    std::vector<VEC4> tri_joint;
    std::vector<VEC4> quad_joint;
    std::vector<VEC4> poly_joint;

    std::vector<VEC4> tri_weight;
    std::vector<VEC4> quad_weight;
    std::vector<VEC4> poly_weight;
    

    
    std::mymap<int,std::vector<float> > tri_attribs;
    std::mymap<int,std::vector<float> > quad_attribs;
    std::mymap<int,std::vector<float> > poly_attribs;

    std::mymap<int,std::vector<int> > tri_attribsi;
    std::mymap<int,std::vector<int> > quad_attribsi;
    std::mymap<int,std::vector<int> > poly_attribsi;

  };
public:
  static void append_to_polys(VertexArraySet::Polys &target, const VertexArraySet::Polys &source);

  mutable std::mymap<int, Polys*> m_set;
  bool has_normal=true;
  bool has_attrib=false;
  bool has_color=true;
  bool has_texcoord=true;
  bool has_skeleton=false;
  FaceBufferRef ref;
};

struct RenderVertexArray_bufferids
{
  unsigned int buffers[7];
  unsigned int vao[3];
  unsigned int buffers2[7];
  unsigned int buffers3[7];
  unsigned int pos_buffer;
  unsigned int pos_buffer_matrix;
  unsigned int normals_buffer;
  unsigned int normals_buffer_matrix;
  unsigned int color_buffer;
  unsigned int color_buffer_matrix;
  std::vector<unsigned int> attrib_buffer;
  std::vector<unsigned int> attrib_buffer2;
  std::vector<unsigned int> attrib_buffer3;
  std::vector<unsigned int> attribi_buffer;
  std::vector<unsigned int> attribi_buffer2;
  std::vector<unsigned int> attribi_buffer3;
  unsigned int indices_buffer;
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
  RenderVertexArray(VertexArraySet &s2, RenderVertexArray &rend)
    : s(s2), nodelete(false) {
    int ss = 7;
    for(int i=0;i<ss;i++)
      {
	if (i<3) vao[i]=rend.vao[i];
	buffers[i]=rend.buffers[i];
	buffers2[i]=rend.buffers2[i];
	buffers3[i]=rend.buffers3[i];
      }
    tri_count = rend.tri_count;
    quad_count = rend.quad_count;
    poly_count = rend.poly_count;
    pos_buffer = rend.pos_buffer;
    pos_buffer_matrix = rend.pos_buffer_matrix;
    normals_buffer = rend.normals_buffer;
    normals_buffer_matrix = rend.normals_buffer_matrix;
    color_buffer = rend.color_buffer;
    color_buffer_matrix = rend.color_buffer_matrix;
    attrib_buffer = rend.attrib_buffer;
    attrib_buffer2 = rend.attrib_buffer2;
    attrib_buffer2 = rend.attrib_buffer2;
    attribi_buffer = rend.attribi_buffer;
    attribi_buffer2 = rend.attribi_buffer2;
    attribi_buffer2 = rend.attribi_buffer2;
  }
  RenderVertexArray(LowApi *g_low, VertexArraySet &s)
    : s(s),nodelete(false),tri_count(0), quad_count(0), poly_count(0) { }
  GameApi::PinIn prepare(int id, bool isnull=false, int tri_count=-1, int quad_count=-1, int poly_count=-1);
  GameApi::PinIn update(int id);
  GameApi::PinIn update_tri(int id, int buffer_id, int start, int end);
  GameApi::PinIn render(int id);
  void sort_blit(int id, Matrix in_MV);
  GameApi::PinIn prepare_instanced(int id, Point *positions, Vector *normals, unsigned int *colors, int size);
  GameApi::PinIn prepare_instanced_matrix(int id, Matrix *positions, Vector *normals, unsigned int *colors, int size);
  GameApi::PinIn prepare_indices(int id, const unsigned char *ind1, const unsigned short *ind2, const unsigned int *ind3, int facecount);
  GameApi::PinIn render_instanced(int id, Point *positions, Vector *normals, unsigned int *colors, int size);
  GameApi::PinIn render_instanced_matrix(int id, Matrix *positions, Vector *normals, unsigned int *colors, int size);
  void update_buffers(RenderVertexArray_bufferids ids);
  void fetch_buffers(RenderVertexArray_bufferids &ids);
  void set_no_delete(bool b) { nodelete=b; }
  void del();
  ~RenderVertexArray() { if (!nodelete) del(); }
public:
  //LowApi *g_low; // doesnt work because ctor is run before global variable is initialized
  VertexArraySet &s;
  unsigned int buffers[7];
  unsigned int vao[3];
  unsigned int buffers2[7];
  unsigned int buffers3[7];
  int tri_count;
  int quad_count;
  int poly_count;
  unsigned int pos_buffer;
  unsigned int pos_buffer_matrix;
  unsigned int normals_buffer;
  unsigned int normals_buffer_matrix;
  unsigned int color_buffer;
  unsigned int color_buffer_matrix;
  std::vector<unsigned int> attrib_buffer;
  std::vector<unsigned int> attrib_buffer2;
  std::vector<unsigned int> attrib_buffer3;
  std::vector<unsigned int> attribi_buffer;
  std::vector<unsigned int> attribi_buffer2;
  std::vector<unsigned int> attribi_buffer3;
  unsigned int indices_buffer;
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
  void reserve_fixed2(int id, int count_tri, int count_quad, int count_poly)
  {
    int tri_count=count_tri*3;
    int quad_count=count_quad*6;
    int poly_count=count_poly*3;
    s.set_reserve(id, tri_count, quad_count, poly_count);
  }
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
  void copy(int start_range, int end_range, std::vector<int> attribs = std::vector<int>(), std::vector<int> attribsi = std::vector<int>());
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
  VEC4 joints[200];
  VEC4 weights[200];
  
  const FaceCollection &coll;
  VertexArraySet &s;
};

#ifndef ARM
class ThreadedPrepare;
struct ThreadInfo
{
  ThreadInfo() { }
  pthread_t thread_id;
  VertexArraySet *set;
  FaceCollectionVertexArray2 *va;
  int start_range;
  int end_range;
  std::vector<int> attrib;
  std::vector<int> attribi;
  RenderVertexArray *r;
  FaceCollection *faces;
  ThreadedPrepare *prep;
  pthread_mutex_t *mutex1;
  pthread_mutex_t *mutex2;
  pthread_mutex_t *mutex3;
  Counts ct2_offsets;
  Counts ct2_counts;
};
void *thread_func(void *data);
extern long long g_copy_total;
extern long long g_copy_progress;

class ThreadedPrepare
{
public:
  ThreadedPrepare(FaceCollection *faces) : faces(faces) { }
  int push_thread(int start_range, int end_range, std::vector<int> attrib=std::vector<int>(), std::vector<int> attribi=std::vector<int>())
  {
    g_copy_total += end_range-start_range;
    
    //std::cout << "Thread " << start_range << " " << end_range << std::endl;
    VertexArraySet *s = new VertexArraySet;
    s->set_reserve(0, end_range-start_range, end_range-start_range, 0 /*end_range-start_range*/);
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
    info->r = 0;
    info->faces = 0;
    info->prep = 0;
    ti.push_back(info);

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 300000);
    //std::cout << "phread_create" << std::endl;
    pthread_create(&info->thread_id, &attr, &thread_func, (void*)info);
    //std::cout << "pthread_create_return: " << val << std::endl;
    pthread_attr_destroy(&attr);
    //std::cout << "returning: " << sets.size()-1 << std::endl;
    return sets.size()-1;
  }

  int push_thread2(int start_range, int end_range, RenderVertexArray *r, pthread_mutex_t *mutex1, pthread_mutex_t *mutex2, pthread_mutex_t *mutex3, std::vector<int> attrib=std::vector<int>(), std::vector<int> attribi=std::vector<int>())
  {
    g_copy_total += end_range-start_range;
    //std::cout << "Thread " << start_range << " " << end_range << std::endl;
    VertexArraySet *s = new VertexArraySet;
    //s->set_reserve(0, (end_range-start_range)/10+1, (end_range-start_range)/10+1, (end_range-start_range)/10+1);
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
    info->r = r;
    info->faces = faces;
    info->prep = this;
    info->mutex1 = mutex1;
    info->mutex2 = mutex2;
    info->mutex3 = mutex3;
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
    g_copy_total=0;
    g_copy_progress=0;
    //ti.clear();
    //sets.clear();
    //va2.clear();
    delete ti[id];
    delete sets[id];
    delete va2[id];
    ti[id]=0;
    sets[id]=0;
    va2[id]=0;
  }
  void transfer_to_gpu_mem(VertexArraySet *set, RenderVertexArray &r, int rend_id, int buf, int start, int end) {
    RenderVertexArray_bufferids ids;
    r.fetch_buffers(ids);
    VertexArraySet * vs = set;
    RenderVertexArray rend(*vs, r /*g_low, *vs*/);
    rend.update_buffers(ids);
    rend.set_no_delete(true);
    rend.update_tri(rend_id, buf, start,end);
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

    int s2 = ti.size();
    for(int i2=0;i2<s2;i2++)
      {
	if (ti[i2])
	  join(i2);
      }
    
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

#endif

