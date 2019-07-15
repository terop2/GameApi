
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
//#define THREADS 1
#endif
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "GameApi_h.hh"
#include "FreeType2.hh"

#include <cstring>

#ifdef LOOKING_GLASS
#define HP_LOAD_LIBRARY
#include <holoplay.h>
#endif

#ifndef FIRST
#ifndef SECOND
#ifndef THIRD
#define FIRST_PART 1
#define SECOND_PART 1
#define THIRD_PART 1
#endif
#endif
#endif
#ifdef FIRST
#define FIRST_PART 1
#endif
#ifdef SECOND
#define SECOND_PART 1
#endif
#ifdef THIRD
#define THIRD_PART 1
#endif

void InstallProgress(int num,std::string label, int max=15);
void ProgressBar(int num, int val, int max, std::string label);
std::string funccall_to_string(ShaderModule *mod);
std::string color_funccall_to_string(ShaderModule *mod);
std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);
std::string color_funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);
extern std::string gameapi_homepageurl;
extern int async_pending_count;
extern bool async_is_done;
extern float debug_pos_x, debug_pos_y, debug_pos_z;


class MaterialForward : public Material
{
public:
  GameApi::ML call(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = mat(p.id);
    return ml;
  }
  GameApi::ML call_inst(GameApi::P p, GameApi::PTS pts)
  {
    GameApi::ML ml;
    ml.id = mat_inst(p.id,pts.id);
    return ml;
  }
  int mat(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::ML ml = mat2(p2);
    return ml.id;
  }
  int mat_inst(int p, int pts) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst(p2,p3);
    return ml.id;
  }
  int mat_inst2(int p, int pta) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTA p3;
    p3.id = pta;
    GameApi::ML ml = mat2_inst2(p2,p3);
    return ml.id;

  }
  int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat_inst_fade(p2,p3, flip, start_time, end_time);
    return ml.id;

  }
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};


#ifdef FIRST_PART

#if 0
void *operator new( std::size_t count)
{
  static int counter = 0;
  counter++;
  if (counter % 10000==0) {
    std::printf("Large number of allocs: %d\n", counter);
  }
  if (count >1000000) {
    std::printf("global op new called, size= %zu\n", count);
  }
  return malloc(count);
}
void operator delete(void* ptr) noexcept
{
  free(ptr);
}
#endif

int array_type_to_int(GameApi::BM b) { return E_BM; }
int array_type_to_int(GameApi::P b) { return E_P; }




GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm);

#if MOVED_TO_PARSER_HH
struct BitmapHandle
{
  int id;
  virtual ~BitmapHandle() { }
};
#endif

#if MOVED_TO_PARSER_HH
struct BitmapColorHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  ~BitmapColorHandle() { delete bm; }
};
struct BitmapIntHandle : public BitmapHandle
{
  Bitmap<int> *bm;
  ~BitmapIntHandle() { delete bm; }
};
struct BitmapArrayHandle : public BitmapHandle
{
  std::vector<BitmapHandle*> vec;
  std::vector<int> owned;
  ~BitmapArrayHandle() 
  {
    // NOT deleted here, because bitmaparray() uses find_bitmap() to find the
    // handles, and thus we don't have ownership of them.
    if (vec.size() != owned.size())
      {
	std::cout << "BitmapArrayHandle destructor: vector sizes do not match" << std::endl;
      }
    int s = vec.size();
    for(int i=0;i<s;i++) 
      if (owned[i]==1)
	delete vec[i];
  }
};
struct BitmapPosHandle : public BitmapHandle
{
  Bitmap<Pos> *bm;
  ~BitmapPosHandle() { delete bm; }
};
struct BitmapTileHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  int tile_sx, tile_sy;
  ~BitmapTileHandle() { delete bm; }
};
#endif



ArrayRender *FindRender(GameApi::Env &e, int bm_i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->renders[bm_i];
}
ArrayRender *FindRender2(GameApi::Env &e, int bm_i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->renders2[bm_i];
}

GameApi::PT add_point(GameApi::Env &e, float x, float y);
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z);


SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);

#define ARRMACRO2(arr,arr2) \
  arr add_array_ ## arr2 (GameApi::Env &e, ArrayType *type) {	\
    ::EnvImpl *env = ::EnvImpl::Environment(&e);	    \
  env->arrays2.push_back(type); \
  arr im;\
  im.id = env->arrays2.size()-1; \
  return im;\
  } \
  ArrayType *find_array_ ## arr2(GameApi::Env &e, arr a) \
  { \
  ::EnvImpl *env = ::EnvImpl::Environment(&e);\
  return env->arrays2[a.id];\
  }

ARRMACRO2(GameApi::PAR,par)
#undef ARRMACRO2

class EnableLinkArray : public Array<int, bool>
{
public:
  EnableLinkArray(Array<int,bool> *next, int num) : next(next), num(num) { }
  int Size() const {
    if (!next) return num;
    //std::cout << "EnableLinkArray: " << this << " " << next << " " << num << std::endl;
    return std::max(num, next->Size()); }
  bool Index(int i) const {
    if (i==num) return true;
    return next->Index(i);
  }
private:
  Array<int,bool> *next;
  int num;
};

GameApi::ST GameApi::EventApi::enable_obj(ST states, int state, LL link)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int state_range_id = states.id;
  int pos_id = link.id;
  int range_start = env->state_ranges[state_range_id].start_range;
  int range_size = env->state_ranges[state_range_id].range_size;
  if (state > range_size-1) { std::cout << "Enable obj, state range error!" << std::endl; }
  std::cout << "Enable_obj:" << range_start << " " << state << " " << range_size << std::endl;
  StateInfo2 &info = env->states[range_start + state];
  Array<int,bool> *enable = info.enable_obj_array;
  info.enable_obj_array = new EnableLinkArray(enable, pos_id);
  return states;
}



EXPORT GameApi::GridApi::GridApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new GridPriv;
}
EXPORT GameApi::GridApi::~GridApi()
{
  delete (GridPriv*)priv;
}

EXPORT GameApi::BM GameApi::WaveformApi::waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  Waveform *m_wave = find_waveform(e, wave);
  return add_color_bitmap(e, new WaveformBitmap(*m_wave, 0.0, m_wave->Length(), m_wave->Min(), m_wave->Max(), sx, sy, Color(true_color), Color(false_color)));
}

GameApi::SpaceApi::SpaceApi(Env &e) : e(e) { }

GameApi::SpaceApi::~SpaceApi() { }

#if 0
GameApi::PT GameApi::MovementApi::pos(float x, float y)
{
  return add_point(e, x,y);
}
#endif


std::map<int, void (*)(GameApi::EveryApi&) > gamefunctions;
std::map<int, int> gamemapping;

EXPORT GameApi::GamesApi::GamesApi(Env &e) : e(e)
{
}
EXPORT GameApi::GamesApi::~GamesApi()
{
}
EXPORT void GameApi::GamesApi::register_game(int game_id, void (*fptr)(GameApi::EveryApi &e))
{
  gamefunctions[game_id] = fptr;
}
EXPORT void GameApi::GamesApi::modify_map(int event, int game_id)
{
  gamemapping[event] = game_id;
}
EXPORT void GameApi::GamesApi::start_game(int event)
{
  int id = gamemapping[event];
  void (*fptr)(GameApi::EveryApi &) = gamefunctions[id];
  if (fptr)
    {
      GameApi::EveryApi ea(e);
      fptr(ea);
    }
}





































GameApi::SV GameApi::SpaceVectorApi::from_points(GameApi::PC pc) {
  // choose polygon via hit test
  // find value of vector via interpolation
  // return it.
  // SV = PT->V
  // PC = int->PT
  // algo fills the space with polygons, finds vector.
  GameApi::SV sv;
  sv.id = 0;
  return sv;
}
GameApi::PT GameApi::SpaceVectorApi::flow_next_point(SV v, PT p, float mult)
{
  GameApi::PT pt;
  pt.id = 0;
  return pt;
}

GameApi::SA GameApi::SeparateApi::empty()
{
  return add_separate(e, new SeparateEmpty);
}

GameApi::SA GameApi::SeparateApi::u_sep(SA orig, float (*sep_x)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateX(sep, sep_x, data));
}
GameApi::SA GameApi::SeparateApi::v_sep(SA orig, float (*sep_y)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateY(sep, sep_y, data));
}
GameApi::SA GameApi::SeparateApi::surf_sep(SA orig, float (*sep_dist)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateDist(sep, sep_dist, data));
}

GameApi::P GameApi::SeparateApi::create_quads(GameApi::SA object, float dist, float dist_accuracy,
			float x_range_start, float x_range_end, float x_step,
			float y_range_start, float y_range_end, float y_step,
			float z_range_start, float z_range_end, float z_step,
			float u_range_start, float u_range_end, float u_step,
			float v_range_start, float v_range_end, float v_step
)
{
  Separate *sep = find_separate(e, object);
  return add_polygon(e, new SeparateRender(*sep, dist, dist_accuracy,
					   x_range_start, x_range_end, x_step,
					   y_range_start, y_range_end, y_step,
					   z_range_start, z_range_end, z_step,
					   u_range_start, u_range_end, u_step,
					   v_range_start, v_range_end, v_step), 1);
}



class EmptyMove : public Array<int, ObjectWithPos>
{
public:
  EmptyMove(int s) : s(s) { }
  ObjectWithPos Index(int i) const
  {
    ObjectWithPos p;
    GameApi::P pp;
    GameApi::M m;
    pp.id = 0;
    for(int i=0;i<16;i++)
      {
	m.mat[i] = 0.0f;
      }
    p.obj = pp;
    p.m = m;
    return p;
  }
  int Size() const { return s; }
private:
  int s;
};

class AddObjectMove : public Array<int,ObjectWithPos>
{
public:
  AddObjectMove(Array<int,ObjectWithPos> &arr, int index, GameApi::P coll) : arr(arr), index(index), coll(coll) { }
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
    if (i==index) { 
      ObjectWithPos p;
      p.obj = coll;
      p.m = arr.Index(i).m;
      return p;
    }
    return arr.Index(i);
  }  
private:
  Array<int,ObjectWithPos> &arr;
  int index;
  GameApi::P coll;
};
class PosMove : public Array<int,ObjectWithPos>
{
public:
  PosMove(Array<int,ObjectWithPos> &arr, int index, GameApi::M m) : arr(arr), index(index), m(m) { }
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
    if (i==index) { 
      ObjectWithPos p;
      p.obj = arr.Index(i).obj;
      p.m = m;
      return p;
    }
    return arr.Index(i);
  }
private:
  Array<int, ObjectWithPos> &arr;
  int index;
  GameApi::M m;
};

#if 0
class VertexMove : public Array<int, ObjectWithPos>
{
public:
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
  }
};
#endif

//  std::vector<Array<ObjectWithPos> *> object_move;
GameApi::OM GameApi::ObjectMoveApi::empty(int numobjects)
{
  return add_move(e, new EmptyMove(numobjects));
}
GameApi::OM GameApi::ObjectMoveApi::add_object(OM orig, int index, P obj)
{
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new AddObjectMove(*arr, index, obj));
}
GameApi::OM GameApi::ObjectMoveApi::clear_object(OM orig, int index)
{
  P p;
  p.id = 0;
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new AddObjectMove(*arr, index, p));
}
GameApi::OM GameApi::ObjectMoveApi::change_pos(OM orig, int index, M mat)
{
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new PosMove(*arr, index, mat));
}
GameApi::VAA GameApi::ObjectMoveApi::prepare_all(OM orig)
{
  PolygonApi poly(e);
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  int s = arr->Size();
  std::vector<VertexArrayWithPos> *vec = new std::vector<VertexArrayWithPos>;
  for(int i=0;i<s;i++)
    {
      ObjectWithPos p = arr->Index(i);
      GameApi::VA va = poly.create_vertex_array(p.obj);
      VertexArrayWithPos pos;
      pos.va = va;
      pos.m = p.m;
      vec->push_back(pos);
    }
  return add_move_array(e, vec);
}
void GameApi::ObjectMoveApi::render_all(GameApi::VAA va)
{
  PolygonApi poly(e);
  std::vector<VertexArrayWithPos> *vec = find_move_array(e, va);
  int s = vec->size();
  for(int i=0;i<s;i++)
    {
      VertexArrayWithPos p = (*vec)[i];
      Matrix mm = find_matrix(e, p.m);
#ifndef EMSCRIPTEN
      g_low->ogl->glPushMatrix();
      float mat[16] = { mm.matrix[0], mm.matrix[4], mm.matrix[8], mm.matrix[12],
			mm.matrix[1], mm.matrix[5], mm.matrix[9], mm.matrix[13],
			mm.matrix[2], mm.matrix[6], mm.matrix[10], mm.matrix[14],
			mm.matrix[3], mm.matrix[7], mm.matrix[11], mm.matrix[15] };
      
      g_low->ogl->glMultMatrixf(&mat[0]);

      poly.render_vertex_array(p.va);
      g_low->ogl->glPopMatrix();
#endif
    }
}




void *Thread_Call(void *data)
{
  GameApi::RenderObject *o = (GameApi::RenderObject*)data;
  o->prepare();
#ifndef ARM
  pthread_exit(NULL);
#endif
  return 0;
}
#ifndef ARM
pthread_t thread;
#endif
void GameApi::prepare(GameApi::RenderObject &o)
{
#ifndef ARM 
  pthread_create(&thread, NULL, Thread_Call, (void*)&o);
#endif
}

GameApi::Pa GameApi::PolygonArrayApi::split_p(EveryApi &ev, P p, int max_chunk)
{
  FaceCollection *f = find_facecoll(e, p);
  f->Prepare();
  std::vector<P> vec;
  int num = f->NumFaces();
  int start = 0;
  std::cout << "split_p" << std::endl;
  while(num > max_chunk)
    { 
      std::cout << "Range: " << start << " " << max_chunk << std::endl;
      vec.push_back(ev.polygon_api.split_p(p, start, start+max_chunk));
      start += max_chunk;
      num -= max_chunk;
    }
  std::cout << "LastRange: " << start << " " << num << std::endl;
  vec.push_back(ev.polygon_api.split_p(p, start, start+num));
  Pa_Impl impl;
  impl.vec = vec;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polygon_array.push_back(impl);
  Pa pa;
  pa.id = env->polygon_array.size()-1;
  return pa;  
}
GameApi::Va GameApi::PolygonArrayApi::create_vertex_array(EveryApi &ev, Pa p, bool keep)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Pa_Impl &impl = env->polygon_array[p.id];
  int s = impl.vec.size();
  Va_Impl va_impl;
  for(int i=0;i<s;i++)
    {
      P pp = impl.vec[i];
      VA va = ev.polygon_api.create_vertex_array(pp, keep);
      va_impl.vec.push_back(va);
    }
  env->va_array.push_back(va_impl);
  Va va;
  va.id = env->va_array.size()-1;
  return va;
}
void GameApi::PolygonArrayApi::render_vertex_array(EveryApi &ev, Va p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[p.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.render_vertex_array(va);
    }
}
void GameApi::PolygonArrayApi::prepare_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.prepare_vertex_array_instanced(sha, va, pta, sh);
    }
}
void GameApi::PolygonArrayApi::render_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.render_vertex_array_instanced(sha, va, pta, sh);
    }
}
void GameApi::PolygonArrayApi::delete_vertex_array(EveryApi &ev, Va va)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.delete_vertex_array(va);
    }
}

EXPORT GameApi::BO GameApi::BooleanOps::create_bo(P mesh, O bools, FD fd)
{
  BO_Impl bo;
  bo.mesh = mesh;
  bo.bools = bools;
  bo.fd = fd;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->boolean_ops.push_back(bo);
  BO bo2;
  bo2.id = env->boolean_ops.size()-1;
  return bo2;
  
}

EXPORT GameApi::BO GameApi::BooleanOps::cube(GameApi::EveryApi &ev, 
					 float start_x, float end_x,
					 float start_y, float end_y,
				      float start_z, float end_z,
				      int split_x, int split_y)
{
  P mesh = ev.polygon_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  P mesh2 = ev.polygon_api.splitquads(mesh, split_x, split_y);

  O bools = ev.volume_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  FD fd = ev.dist_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  //CT cutter = ev.cutter_api.distance_cut(fd);
  return create_bo(mesh2, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::sphere(GameApi::EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
{
  P mesh = ev.polygon_api.sphere(center, radius, numfaces1, numfaces2);
  O bools = ev.volume_api.sphere(center, radius);
  FD fd = ev.dist_api.sphere(center, radius);
  return create_bo(mesh, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::or_elem(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];
  P mesh = ev.polygon_api.or_elem(obj_i.mesh, obj2_i.mesh);
  O bools = ev.volume_api.max_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.min(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::and_not(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];

  CT cutter = ev.cutter_api.distance_cut(obj_i.fd);
  CT cutter2 = ev.cutter_api.distance_cut(obj2_i.fd);
  P mesh = ev.polygon_api.and_not_elem(ev, obj2_i.mesh, obj_i.mesh,
				       obj2_i.bools, obj_i.bools,
				       cutter2, cutter);
  //P mesh2 = ev.polygon_api.tri_to_quad(mesh);
  O bools = ev.volume_api.andnot_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.and_not(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}

EXPORT GameApi::BO GameApi::BooleanOps::intersect(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];

  CT cutter = ev.cutter_api.distance_cut(obj_i.fd);
  CT cutter2 = ev.cutter_api.distance_cut(obj2_i.fd);
  P mesh = ev.polygon_api.intersect(ev, obj_i.mesh, obj2_i.mesh,
				       obj_i.bools, obj2_i.bools,
				       cutter, cutter2);
  O bools = ev.volume_api.min_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.max(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}

EXPORT GameApi::P GameApi::BooleanOps::to_polygon(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.mesh;
}
EXPORT GameApi::O GameApi::BooleanOps::to_volume(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.bools;
}
EXPORT GameApi::FD GameApi::BooleanOps::to_dist(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.fd;
}

// Explicit instantiation of array api
namespace GameApi {
#ifndef EMSCRIPTEN
  //class template ArrayApi<GameApi::BM>;
  //class template ArrayApi<GameApi::P>;
#endif
};
class RectangleCollision : public Collision
{
public:
  RectangleCollision(int id, float x, float y, float sx, float sy) : Collision(id), x(x),y(y),sx(sx),sy(sy) { }
  bool check_collision(float xx,float yy) const
  {
    xx-=m_x;
    yy-=m_y;
    if (xx<x) return false;
    if (yy<y) return false;
    if (xx>x+sx) return false;
    if (yy>y+sy) return false;
    return true;
  }
private:
  float x,y,sx,sy;
};
class OrElemCollision : public Collision
{
public:
  OrElemCollision(int id, std::vector<Collision*> vec) : Collision(id), vec(vec) { }
  bool check_collision(float x, float y) const
  {
    x-=m_x;
    y-=m_y;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	bool b = vec[i]->check_collision(x,y);
	if (b) { return true; }
      }
    return false;
  }
  void set_pos(int id, float x, float y)
  {
    if (m_id == id) { m_x = x; m_y = y; }
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Collision *c = vec[i];
	c->set_pos(id, x,y);
      }
  }
private:
  std::vector<Collision*> vec;
};
class CircleCollision : public Collision
{
public:
  CircleCollision(int id, float radius) : Collision(id), radius(radius) { }
  bool check_collision(float x, float y) const
  {
    x-=m_x;
    y-=m_y;
    float d = x*x+y*y;
    return d < radius*radius;
  }
private:
  float radius;
};
GameApi::CP GameApi::CollisionPlane::rectangle(int id, float x, float y, float sx, float sy)
{
  return add_collision(e, new RectangleCollision(id, x,y,sx,sy));
}
GameApi::CP GameApi::CollisionPlane::circle(int id, float radius)
{
  return add_collision(e, new CircleCollision(id, radius));
}
GameApi::CP GameApi::CollisionPlane::or_elem(int id, std::vector<CP> vec)
{
  std::vector<Collision*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_collision(e, vec[i]));
    }
  return add_collision(e, new OrElemCollision(id, vec2));
}
void GameApi::CollisionPlane::set_pos(CP plane, int id, float x, float y)
{
  Collision *c = find_collision(e, plane);
  c->set_pos(id, x,y);
}
bool GameApi::CollisionPlane::check_collision(CP plane, float x, float y)
{
  Collision *c = find_collision(e, plane);
  return c->check_collision(x,y);
}

class EmptyMovement : public Movement
{
public:
  EmptyMovement() : m_m(Matrix::Identity()) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  void draw_event(FrameLoopEvent &e) {}

  void set_matrix(Matrix m) { m_m = m; }
  Matrix get_whole_matrix(float time, float delta_time) const { return m_m; }
private:
  Matrix m_m;
};
EXPORT GameApi::MN GameApi::MovementNode::empty()
{
  return add_move(e, new EmptyMovement);
}
class ColorStart : public ColorChange
{
public:
  ColorStart(unsigned int color) : color(color) { }
  Color get_whole_color(float time, float delta_time) const
  {
    return color;
  }
private:
  unsigned int color;
};
class ColorInterpolate2 : public ColorChange
{
public:
  ColorInterpolate2(ColorChange *next, unsigned int color, unsigned int color2, float start_time, float end_time) : next(next), color(color), color2(color2), start_time(start_time), end_time(end_time) { }
  Color get_whole_color(float time, float delta_time) const
  {
    if (time < start_time) return next->get_whole_color(time, delta_time);
    if (time >= end_time) return next->get_whole_color(time, delta_time);
    float d =  time - start_time;
    d/=(end_time-start_time);
    return Color::Interpolate(color, color2, d);
  }
private:
  ColorChange *next;
  unsigned int color;
  unsigned int color2;
  float start_time;
  float end_time;
};

EXPORT GameApi::CC GameApi::MovementNode::color_start(unsigned int color)
{
  return add_color(e, new ColorStart(color));
}
EXPORT GameApi::CC GameApi::MovementNode::color_interpolate(CC next, unsigned int color, unsigned int color2, float start_time, float end_time)
{
  ColorChange *cc = find_color(e, next);
  return add_color(e, new ColorInterpolate2(cc, color, color2, start_time, end_time));
}
class LevelMovement : public Movement
{
public:
  LevelMovement(Movement *m) : m(m) { }
  virtual void event(MainLoopEvent &e) { m->event(e); }
  virtual void frame(MainLoopEnv &e) { m->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { m->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { m->draw_event(e); }
  void set_matrix(Matrix m) { m_m = m; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return m_m * m->get_whole_matrix(time, delta_time);
  }
private:
  Movement *m;
  Matrix m_m;
};
EXPORT GameApi::MN GameApi::MovementNode::level(MN next)
{
  Movement *move = find_move(e, next);
  return add_move(e, new LevelMovement(move));
}
class TranslateMovement : public Movement
{
public:
  TranslateMovement(Movement *next, float start_time, float end_time,
		    float dx, float dy, float dz)
    : next(next), start_time(start_time), end_time(end_time),
      dx(dx), dy(dy), dz(dz) { }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { if (next) next->draw_event(e); }

  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { dx=ddx; dy=ddy; dz=ddz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { Matrix m=Matrix::Identity(); return next?next->get_whole_matrix(time, delta_time):m; }
    if (time>=end_time) { Matrix m=Matrix::Identity(); return Matrix::Translate(dx,dy,dz)*(next?next->get_whole_matrix(time,delta_time):m); }
    float d = time - start_time;
    //if (fabs(end_time-start_time)>0.01)
      d/=(end_time-start_time);
    return Matrix::Translate(dx*d,dy*d,dz*d)*(next?next->get_whole_matrix(time, delta_time):Matrix::Identity());
  }
private:
  Movement *next;
  float start_time, end_time;
  float dx,dy,dz;
};

class TranslateWaveMovement : public Movement
{
public:
  TranslateWaveMovement(Movement *next, float start_time, float end_time,
			Waveform *wave,
			float dx, float dy, float dz)
    : next(next), start_time(start_time), end_time(end_time),wave(wave),
      dx(dx), dy(dy), dz(dz) { }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { if (next) next->draw_event(e); }

  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { dx=ddx; dy=ddy; dz=ddz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { 
      float l = 0.0f;
      float value = wave->Index(l);
      Matrix m=Matrix::Identity(); return Matrix::Translate(dx*value,dy*value,dz*value)*(next?next->get_whole_matrix(time, delta_time):m); }
    if (time>=end_time) {
      float l = wave->Length();
      float value = wave->Index(l);
      Matrix m=Matrix::Identity(); return Matrix::Translate(dx*value,dy*value,dz*value)*(next?next->get_whole_matrix(time,delta_time):m); }
    float d = time - start_time;
    //if (fabs(end_time-start_time)>0.01)
      d/=(end_time-start_time);
      float l = wave->Length()*d;
      float value = wave->Index(l);
    return Matrix::Translate(dx*value,dy*value,dz*value)*(next?next->get_whole_matrix(time, delta_time):Matrix::Identity());
  }
private:
  Movement *next;
  float start_time, end_time;
  Waveform *wave;
  float dx,dy,dz;
};



class DebugTranslateMovement : public Movement
{
public:
  DebugTranslateMovement(Movement *next)
    : next(next){ }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { if (next) next->draw_event(e); }

  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return Matrix::Translate(debug_pos_x, debug_pos_y, debug_pos_z);
  }
private:
  Movement *next;
  float start_time, end_time;
  float dx,dy,dz;
};

EXPORT GameApi::MN GameApi::MovementNode::debug_translate(MN next)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new DebugTranslateMovement(nxt));
}

EXPORT GameApi::MN GameApi::MovementNode::translate_wave(MN next,
						    float start_time, float end_time,
						    WV wave,
						    float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  Waveform *m_wave = find_waveform(e, wave);
  return add_move(e, new TranslateWaveMovement(nxt,start_time, end_time, m_wave,
					   dx,dy,dz));

}

EXPORT GameApi::MN GameApi::MovementNode::translate(MN next, 
					     float start_time, float end_time,
					     float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TranslateMovement(nxt,start_time, end_time,
					   dx,dy,dz));
}
int FindProgressVal();
int FindProgressMax();
class ScaleProgress : public Movement
{
public:
  ScaleProgress(Movement *next, bool is_x, bool is_y, bool is_z) : next(next), is_x(is_x), is_y(is_y), is_z(is_z) { 
    max_async_pending = 0;
  }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }
  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    float x = is_x ? val() : 1.0;
    float y = is_y ? val() : 1.0;
    float z = is_z ? val() : 1.0;
    return Matrix::Scale(x,y,z)*next->get_whole_matrix(time,delta_time);
  }
  float val() const {
    //if (async_pending_count>max_async_pending) max_async_pending=async_pending_count;
    //float val1 = 1.0-(async_pending_count/max_async_pending);
    //if (val1<0.1) val1=0.1;
    //if (val1>1.0) val1=1.0;
    const_cast<ScaleProgress*>(this)->time+=1.0;
    float val2 = float(FindProgressVal())/float(FindProgressMax()+171.0);
    val2+=time/float(FindProgressMax()+171.0);
    //std::cout << "Time:" << time << std::endl;
    if (val2<0.1) val2=0.1;
    if (val2>1.0) val2=1.0;
    return val2;
  }
private:
  float time=0.0;
  Movement *next;
  mutable int max_async_pending;
  bool is_x, is_y, is_z;
};

GameApi::MN GameApi::MovementNode::scale_progress(MN next, bool is_x, bool is_y, bool is_z)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new ScaleProgress(nxt,is_x,is_y,is_z));
}
class MN_Fetcher : public Movement
{
public:
  MN_Fetcher(Fetcher<Point> *pf) : pf(pf) {}
  virtual void event(MainLoopEvent &e) { pf->event(e); }
  virtual void frame(MainLoopEnv &e) { pf->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { std::cout << "MN_fetcher not supporting draw_frame" << std::endl; }
  virtual void draw_event(FrameLoopEvent &e) { std::cout << "MN_fetcher not supporting draw_event" << std::endl; }

  virtual void set_matrix(Matrix m) { }
  virtual Matrix get_whole_matrix(float time, float delta_time) const
  {
    Point p = pf->get();
    return Matrix::Translate(p.x,p.y,p.z);
  }
private:
  Fetcher<Point> *pf;
};
EXPORT GameApi::MN GameApi::MovementNode::mn_fetcher(PF pf)
{
  Fetcher<Point> *ppf = find_point_fetcher(e, pf);
  return add_move(e, new MN_Fetcher(ppf));
}
class ScaleMovement : public Movement
{
public:
  ScaleMovement(Movement *next, float start_time, float end_time,
		    float sx, float sy, float sz)
    : next(next), start_time(start_time), end_time(end_time),
      sx(sx), sy(sy), sz(sz) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }
  void set_matrix(Matrix m) { }
  void set_scale(float ssx, float ssy, float ssz) { sx=ssx; sy=ssy; sz=ssz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { return Matrix::Scale(sx,sy,sz)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    float ssx = (1.0-d)*1.0 + d*sx;
    float ssy = (1.0-d)*1.0 + d*sy;
    float ssz = (1.0-d)*1.0 + d*sz;
    return Matrix::Scale(ssx,ssy,ssz)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  float sx,sy,sz;
};

class ScaleWaveMovement : public Movement
{
public:
  ScaleWaveMovement(Movement *next, float start_time, float end_time, Waveform *wave,
		    float sx, float sy, float sz)
    : next(next), start_time(start_time), end_time(end_time), wave(wave),
      sx(sx), sy(sy), sz(sz) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }
  void set_matrix(Matrix m) { }
  void set_scale(float ssx, float ssy, float ssz) { sx=ssx; sy=ssy; sz=ssz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { 
      float l = 0.0f;
      float value = wave->Index(l);
      float ssx = 1.0*(1.0-value) + sx*value;
      float ssy = 1.0*(1.0-value) + sy*value;
      float ssz = 1.0*(1.0-value) + sz*value;

      return Matrix::Scale(ssx,ssy,ssz)*next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { 
      float l = wave->Length();
      float value = wave->Index(l);
      float ssx = 1.0*(1.0-value) + sx*value;
      float ssy = 1.0*(1.0-value) + sy*value;
      float ssz = 1.0*(1.0-value) + sz*value;

      return Matrix::Scale(ssx,ssy,ssz)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    float l = d*wave->Length();
    float value = wave->Index(l);
    float ssx = (1.0-value)*1.0 + value*sx;
    float ssy = (1.0-value)*1.0 + value*sy;
    float ssz = (1.0-value)*1.0 + value*sz;
    return Matrix::Scale(ssx,ssy,ssz)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  Waveform *wave;
  float sx,sy,sz;
};


EXPORT GameApi::MN GameApi::MovementNode::scale_wave(MN next,
					 float start_time, float end_time,
						     WV wave,
					 float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  Waveform *m_wave = find_waveform(e, wave);
  return add_move(e, new ScaleWaveMovement(nxt, start_time, end_time, m_wave,
				       sx,sy,sz));
}


EXPORT GameApi::MN GameApi::MovementNode::scale(MN next,
					 float start_time, float end_time,
					 float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new ScaleMovement(nxt, start_time, end_time,
				       sx,sy,sz));
}
class RotateMovement : public Movement
{
public:
public:
  RotateMovement(Movement *next, float start_time, float end_time,
		 float p_x, float p_y, float p_z,
		float v_x, float v_y, float v_z, float angle)
    : next(next), start_time(start_time), end_time(end_time),
      p_x(p_x), p_y(p_y), p_z(p_z),
      v_x(v_x), v_y(v_y), v_z(v_z), angle(angle) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), angle)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), d*angle)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  float p_x,p_y,p_z;
  float v_x,v_y,v_z;
  float angle;
};

class RotateWaveMovement : public Movement
{
public:
public:
  RotateWaveMovement(Movement *next, float start_time, float end_time, Waveform *wave,
		 float p_x, float p_y, float p_z,
		float v_x, float v_y, float v_z, float angle)
    : next(next), start_time(start_time), end_time(end_time), wave(wave),
      p_x(p_x), p_y(p_y), p_z(p_z),
      v_x(v_x), v_y(v_y), v_z(v_z), angle(angle) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { 
      float l = 0.0f;
      float value = wave->Index(l);
      return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), angle*value)*next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { 
      float l = wave->Length();
      float value = wave->Index(l);
      return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), angle*value)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    float l = wave->Length()*d;
    float value = wave->Index(l);
    return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), value*angle)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  Waveform *wave;
  float p_x,p_y,p_z;
  float v_x,v_y,v_z;
  float angle;
};

EXPORT GameApi::MN GameApi::MovementNode::rotate_wave(MN next, float start_time, float end_time, WV wave, float p_x, float p_y, float p_z, float v_x, float v_y, float v_z, float angle)
{
  Waveform *m_wave = find_waveform(e, wave);
  Movement *nxt = find_move(e, next);
  return add_move(e, new RotateWaveMovement(nxt, start_time, end_time, m_wave,
					p_x,p_y,p_z,v_x, v_y, v_z, angle));
}


EXPORT GameApi::MN GameApi::MovementNode::rotate(MN next, float start_time, float end_time, float p_x, float p_y, float p_z, float v_x, float v_y, float v_z, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new RotateMovement(nxt, start_time, end_time,
					p_x,p_y,p_z,v_x, v_y, v_z, angle));
}

class CompressMovement : public Movement
{
public:
  CompressMovement(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) 
  { 
    zero = next->get_whole_matrix(start_time, 1.0);
    one = next->get_whole_matrix(end_time,1.0);
  }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }
  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return zero; }
    if (time>end_time) { return one; }
    return next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  Matrix zero;
  Matrix one;
};

class AnimEnable : public Movement
{
public:
  AnimEnable(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return Matrix::Zero(); }
    if (time>end_time) { return Matrix::Zero(); }
    return next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time,end_time;
};

class AnimDisable : public Movement
{
public:
  AnimDisable(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time,delta_time); }
    if (time>end_time) { return next->get_whole_matrix(time,delta_time); }
    return Matrix::Zero(); 
  }
private:
  Movement *next;
  float start_time,end_time;
};
class AnimChoose : public Movement
{
public:
  AnimChoose(std::vector<Movement*> vec, float start_time, float duration) : vec(vec), start_time(start_time), duration(duration) { }
  virtual void event(MainLoopEvent &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->event(e);
  }
  virtual void frame(MainLoopEnv &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->frame(e);
  }
  virtual void draw_frame(DrawLoopEnv &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->draw_frame(e);
  }
  virtual void draw_event(FrameLoopEvent &e) { 
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->draw_event(e);
  }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (vec.size()==0) return Matrix::Identity();
    if (time < start_time) return vec[0]->get_whole_matrix(time,delta_time);
    if (time > start_time+duration*vec.size()) return vec[vec.size()-1]->get_whole_matrix(time,delta_time);
    int val = (time-start_time)/duration;
    return vec[val]->get_whole_matrix(time,delta_time);
  }
private:
  std::vector<Movement*> vec;
  float start_time;
  float duration;
};

EXPORT GameApi::MN GameApi::MovementNode::anim_enable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimEnable(next2, start_time, end_time));
}
EXPORT GameApi::MN GameApi::MovementNode::anim_disable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimDisable(next2, start_time, end_time));
}
EXPORT GameApi::MN GameApi::MovementNode::anim_choose(std::vector<MN> vec, float start_time, float duration)
{
  std::vector<Movement*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_move(e,vec[i]));
    }

  return add_move(e, new AnimChoose(vec2, start_time, duration));
}


EXPORT GameApi::MN GameApi::MovementNode::compress(MN next, float start_time, float end_time)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new CompressMovement(nxt, start_time, end_time));  
}
class TimeChangeMovement : public Movement
{
public:
  TimeChangeMovement(Movement *nxt, float d_time) : nxt(nxt), d_time(d_time) { }
  virtual void event(MainLoopEvent &e) { nxt->event(e); }
  virtual void frame(MainLoopEnv &e) { nxt->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { nxt->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { nxt->draw_event(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    time -= d_time;
    return nxt->get_whole_matrix(time, delta_time);
  }
  
private:
  Movement *nxt;
  float d_time;
};
class MatrixMovement : public Movement
{
public:
  MatrixMovement(Movement *next, Matrix m) : next(next), m(m) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix mm) { m = mm; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return next->get_whole_matrix(time, delta_time)*m;
  }
private:
  Movement *next;
  Matrix m;
};
class EventActivateMovement : public Movement
{
public:
  EventActivateMovement(Movement *next, Movement *event2, float activate_time, float duration) : next(next), event2(event2), activate_time(activate_time), duration(duration) { 
    event_activated=false;
    collect = Matrix::Identity();
  }
  virtual void event(MainLoopEvent &e) { next->event(e); event2->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); event2->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); event2->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); event2->draw_event(e); }

  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    Matrix n = next->get_whole_matrix(time, delta_time);
    
    if (time > activate_time && time - delta_time < activate_time)
      {
	event_activated = true;
      }
    if (event_activated) { n = n * event2->get_whole_matrix(time-activate_time, delta_time) * collect; }
    else { n = n * collect; }
    if (event_activated && time - activate_time > duration) {
      event_activated = false;
      collect = collect * event2->get_whole_matrix(duration,delta_time);
    }
    return n;
  }
private:
  Movement *next;
  Movement *event2;
  float activate_time;
  mutable bool event_activated;
  float duration;
  mutable Matrix collect;
};
EXPORT GameApi::MN GameApi::MovementNode::event_activate(MN next, MN event, float activate_time, float duration)
{
  Movement *nxt = find_move(e, next);
  Movement *eve = find_move(e, event);
  return add_move(e, new EventActivateMovement(nxt,eve,activate_time, duration));
}
EXPORT GameApi::MN GameApi::MovementNode::trans2(MN next, float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Translate(dx,dy,dz)));  
}
EXPORT GameApi::MN GameApi::MovementNode::scale2(MN next, float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Scale(sx,sy,sz)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatex(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::XRotation(angle)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatey(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::YRotation(angle)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatez(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::ZRotation(angle)));  
}
class TimeRepeatMovement : public Movement
{
public:
  TimeRepeatMovement(Movement *m, float start_time, float repeat_duration) : m(m), start_time(start_time), repeat_duration(repeat_duration) { }
  virtual void event(MainLoopEvent &e) { m->event(e); }
  virtual void frame(MainLoopEnv &e) { m->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { m->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { m->draw_event(e); }

  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time < start_time) { return m->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    float dd = fmod(d,repeat_duration);
    return m->get_whole_matrix(start_time+dd, delta_time);
  }
private:
  Movement *m;
  float start_time;
  float repeat_duration;
};
EXPORT GameApi::MN GameApi::MovementNode::time_repeat(MN next, float start_time,
					       float repeat_duration)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TimeRepeatMovement(nxt, start_time, repeat_duration));
}

EXPORT GameApi::MN GameApi::MovementNode::change_time(MN next, float d_time)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TimeChangeMovement(nxt, d_time));  
}
void GameApi::MovementNode::set_matrix(MN n, M m)
{
  Movement *nn = find_move(e, n);
  Matrix mm = find_matrix(e, m);
  nn->set_matrix(mm);
}
EXPORT GameApi::M GameApi::MovementNode::get_matrix(MN n, float time, float delta_time)
{
  Movement *nn = find_move(e, n);
  Matrix m = nn->get_whole_matrix(time, delta_time);
  return add_matrix2(e, m);
}
class KeyEventML : public MainLoopItem
{
public:
  KeyEventML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int type, int ch, int button, float duration) : e(e), ev(ev), next(next), mn(mn), type(type), ch(ch), button(button), duration(duration) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    b=false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  bool compare(MainLoopEvent &e)
  {
    //std::cout << "Compare1: " << type << " " << ch << " " << button << std::endl;
    //std::cout << "Compare2: " << e.type << " " << e.ch << " " << e.button << std::endl;
    if (type!=-1 && e.type!=type) return false;
    if (ch!=-1 && e.ch != ch) return false;
    if (button!=-1 && e.button != button) return false;
    //std::cout << "COMPARE true" << std::endl;
    return true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    bool b = compare(env);
    if (b) {
      start_times.push_back(ev.mainloop_api.get_time());
    }

    next->handle_event(env);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    
    int s = start_times.size();
    GameApi::M res = ev.matrix_api.identity();
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M res2 = ev.matrix_api.mult(m2,res);
    for(int i=0;i<s;i++)
      {
	float time = (ev.mainloop_api.get_time()-start_times[i])/100.0;
	GameApi::M mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
	res2 = ev.matrix_api.mult(res2, mat);
      }
    for(int i=0;i<s;i++)
      {
	float time = (ev.mainloop_api.get_time()-start_times[i])/100.0;
	if (time>duration)
	  {
	    start_times.erase(start_times.begin()+i);
	    i--; s--;
	  }
      }
#ifdef HAS_MATRIX_INVERSE
    GameApi::M res2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", res2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", res2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", res2i);
#endif
    env.in_MV = find_matrix(e, res2);
    Matrix old_env = env.env;
    env.env = env.env * find_matrix(e,res2);
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;

  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int type, ch, button;
  bool b;
  std::vector<float> start_times;
  float duration;
};
class KeyPrinter : public MainLoopItem
{
public:
  KeyPrinter(GameApi::Env &e, MainLoopItem *next) : e(e), next(next) { }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Point pt = eve.cursor_pos;
    std::cout << "type: " << eve.type << " char: " << eve.ch << " button: " << eve.button;
    std::cout << " PT(" << pt.x << "," << pt.y << "," << pt.z << ")";
    std::cout << std::endl;
    next->handle_event(eve);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    next->execute(env);
   }
private:
  GameApi::Env &e;
  MainLoopItem *next;
};

class JumpML : public MainLoopItem
{
public:
  JumpML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int key_jump, float height, float jump_duration) : e(e), ev(ev), next(next), key_jump(key_jump), height(height), jump_duration(jump_duration) { pos=0.0; jump_position=0.0; jump_going_up = false; jump_going_down=false; }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (eve.type==0x300 && ch==key_jump && !jump_going_up && !jump_going_down) { jump_going_up=true; }

  }

  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;

    if (jump_going_down)
      {
      pos-=height/(jump_duration/2.0)*env.delta_time;
      jump_position += env.delta_time;
      if (jump_position > jump_duration) {
	jump_going_down = false;
	pos = 0.0;
	jump_position = 0.0;
      }
      }
    if (jump_going_up) {
      pos+=height/(jump_duration/2.0)*env.delta_time;
      jump_position += env.delta_time;
      if (jump_position > jump_duration/2.0) {
	jump_going_down = true;
	jump_going_up = false;
      }
    }
    
    Matrix m = Matrix::Identity();
    m = Matrix::Translate(0.0,pos,0.0);

    GameApi::M mat = add_matrix2(e, m);
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);
    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2);/* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int key_jump;
  float height;
  float jump_duration;
  bool jump_going_up;
  bool jump_going_down;
  float pos;
  float jump_position;
};

class KeyMoveML : public MainLoopItem
{
public:
  KeyMoveML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int key_forward, int key_backward, float speed, float rot_speed, bool x, bool y, bool z, bool rot_x, bool rot_y, bool rot_z, float start, float end) : e(e), ev(ev), next(next),forward(key_forward), backward(key_backward),speed(speed),rot_speed(rot_speed), x(x),y(y),z(z),rot_x(rot_x), rot_y(rot_y), rot_z(rot_z), start(start),end(end) { 
    move_forward=false;
    move_backward=false;
    pos = 0.0;
    angle=0.0;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif


    if (eve.type==0x300 && ch==forward) { move_forward=true; }
    if (eve.type==0x301 && ch==forward) { move_forward=false; }
    if (eve.type==0x300 && ch==backward) { move_backward=true; }
    if (eve.type==0x301 && ch==backward) { move_backward=false; }
    next->handle_event(eve);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;


    if (x||y||z)
      {
    if (move_forward) { pos+=speed;
      if (pos>end) {pos=end; }
    }
    if (move_backward) { pos-=speed; 
      if (pos<start) { pos=start; }
    }
      }
    if (rot_x||rot_y||rot_z)
      {
	if (move_forward) { angle+=rot_speed; }
	if (angle>end) { angle=end; }
	if (move_backward) { angle-=rot_speed; }
	if (angle<start) { angle=start; }
      }
    
    Matrix m = Matrix::Identity();
    if (x) {
      m = Matrix::Translate(pos,0.0,0.0);
      //e.in_MV = e.in_MV * m;
    }
    if (y) {
      m = Matrix::Translate(0.0,pos,0.0);
      //e.in_MV = e.in_MV * m;

    }
    if (z) {
      m = Matrix::Translate(0.0,0.0,pos);
      //e.in_MV = e.in_MV * m;
    }
    if (rot_x) {
      m = Matrix::XRotation(angle);
    }
    if (rot_y) {
      m = Matrix::YRotation(angle);
    }
    if (rot_z) {
      m = Matrix::ZRotation(angle);
    }
    GameApi::M mat = add_matrix2(e, m);
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);
    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2);/* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int forward;
  int backward;
  bool move_forward;
  bool move_backward;
  float pos;
  float angle;
  float speed;
  float rot_speed;
  bool x,y,z;
  bool rot_x, rot_y, rot_z;
  float start,end;
};

class MoveML : public MainLoopItem
{
public:
  MoveML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int clone_count, float time_delta) : e(e), ev(ev), next(next), mn(mn), clone_count(clone_count), time_delta(time_delta) 
  {
    firsttime = true;
    firsttime2 = false;
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    next->handle_event(env);
    Movement *move = find_move(e,mn);
    move->event(env);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e,mn);
    move->frame(env);

    if (firsttime) {
      firsttime2 = true;
    }
    if (!firsttime && firsttime2)
      {
	reset_time();
	firsttime2 = false;
      }
    firsttime = false;
    for(int i=0;i<clone_count;i++)
      {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s11;
    s11.id = env.sh_texture_2d;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    float time = (env.time*1000.0-start_time)/100.0+i*time_delta;
    GameApi::M mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
    //Matrix mat_i = find_matrix(e, mat);
    //std::cout << mat_i << std::endl;
    GameApi::M m2 = add_matrix2(e, env.env);
    //std::cout << env.env << std::endl;
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s11, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
    //Matrix old_in_MV = env.in_MV;
    MainLoopEnv ee = env;
    ee.in_MV = find_matrix(e, mat2);

    //Matrix old_env = env.env;
    //float old_time = env.time;
    ee.env = find_matrix(e,mat2); /* * env.env*/;
    ee.time = env.time + i*time_delta/10.0;
    next->execute(ee);
    ev.shader_api.unuse(s3);
    //env.env = old_env;
    //env.time = old_time;
    //env.in_MV = old_in_MV;
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int clone_count;
  float time_delta;
  bool firsttime;
  bool firsttime2;
};


class ColorML : public MainLoopItem
{
public:
  ColorML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int color_num, ColorChange *cc) : e(e), ev(ev), next(next), color_num(color_num), cc(cc) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    std::stringstream ss;
    ss << color_num;
    color_num2 = "color_array[" + ss.str() + "]";
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    next->handle_event(env);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    Color c2 = cc->get_whole_color(time, env.delta_time);
    //std::cout << color_num2 << " " << c2 << std::endl;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());

    next->execute(env);
    ev.shader_api.unuse(s3);
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  int color_num;
  ColorChange *cc;
  std::string color_num2;
};


class KeyActivateML : public MainLoopItem
{
public:
  KeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int key, float duration) : e(e), ev(ev), next(next), mn(mn),key(key), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    key_pressed = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    bool start_anim = false;
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x300 && ch == key && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x301 && ch == key) { key_pressed = false; }

    if (start_anim) {
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
    }

    next->handle_event(eve);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);


    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	anim_ongoing = false;
	collect = ev.matrix_api.mult(collect, ev.move_api.get_matrix(mn, duration, env.delta_time));
	
	if (key_pressed) { // repeat animation if key is being pressed down when anim stops
	  anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
	}

      }

    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  bool anim_ongoing;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
};



class CombKeyActivateML : public MainLoopItem
{
public:
  CombKeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int key, int key_2, float duration) : e(e), ev(ev), next(next), mn(mn),key(key), key_2(key_2), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    key_pressed = false;
    key2_pressed = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    bool start_anim = false;
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x300 && ch == key_2) { key2_pressed = true; }
    if (eve.type==0x300 && key2_pressed && ch == key && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x300 && key_pressed && ch == key_2 && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x301 && ch == key) { key_pressed = false; }
    if (eve.type==0x301 && ch == key_2) { key2_pressed = false; }

    if (start_anim) {
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
    }

    next->handle_event(eve);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);

    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	anim_ongoing = false;
	collect = ev.matrix_api.mult(collect, ev.move_api.get_matrix(mn, duration, env.delta_time));
	
	if (key_pressed && key2_pressed) { // repeat animation if key is being pressed down when anim stops
	  anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
	}

      }

    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  int key_2;
  bool anim_ongoing;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
  bool key2_pressed;
};




class TempKeyActivateML : public MainLoopItem
{
public:
  TempKeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN nm, int key, float duration) : e(e), ev(ev), next(next), mn(nm),key(key), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    start_time2 = 0.0;
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    anim_ongoing2 = false;
    key_pressed = false;
    key_canceled=false;
    start_anim_chain = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    bool start_anim = false;
    bool start_anim2 = false;
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (eve.type==0x300 && ch == key && !anim_ongoing && !anim_ongoing2 && !key_pressed) { start_anim = true; } else if (eve.type==0x300 && ch==key && !key_pressed)
      {
	key_canceled=true;
      }
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x301 && ch == key && !anim_ongoing2 && !key_canceled) { key_pressed = false;  start_anim2 = true; } 
    if (eve.type==0x301 && ch==key) { key_canceled=false;
      //if (!anim_ongoing && !anim_ongoing2 && anim_pos>0.0) {
      //	anim_pos = duration;
      //	start_anim2 = true;
      //}
    }

    if (start_anim) {
      start_anim_chain = false;
      //std::cout << "start_anim" << std::endl;
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
      start_anim = false;
    }
    if (start_anim2 && !start_anim_chain) {
      start_anim_chain = true;
      //std::cout << "start_anim2" << std::endl;
      anim_ongoing = false;
      anim_ongoing2 = true; 
      start_time2 = ev.mainloop_api.get_time();  
      anim_pos_at_change = anim_pos;
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      time_at_change = std::min(time,duration);

      //      collect = ev.move_api.get_matrix(mn, anim_pos_at_change, ev.mainloop_api.get_delta_time());
      start_anim2 = false;
    }

    
    next->handle_event(eve);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);


    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      //mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	//std::cout << "anim_ongoing=false" << std::endl;
	anim_ongoing = false;
	collect = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());

	if (0)
	{
	  start_anim_chain = true;
	  //std::cout << "start_anim2" << std::endl;
	  anim_ongoing = false;
	  anim_ongoing2 = true; 
	  start_time2 = ev.mainloop_api.get_time();  
	  anim_pos_at_change = anim_pos;
	  float time = (ev.mainloop_api.get_time()-start_time)/100.0;
	  time_at_change = std::min(time,duration);
	}

	//anim_ongoing2 = true;
	//start_time2 = ev.mainloop_api.get_time();  
	//anim_pos_at_change = anim_pos;
      }
    } else if (anim_ongoing2) {
      float time = (ev.mainloop_api.get_time()-start_time2)/100.0;
      mat = ev.move_api.get_matrix(mn, time_at_change-time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      //mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos -= env.delta_time;
      if (time_at_change - time - ev.mainloop_api.get_delta_time() < 0.0) {
	//std::cout << "anim_ongoing2=false" << std::endl;
	anim_ongoing2 = false;
	collect = ev.matrix_api.identity();
      }
    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
#ifdef HAS_MATRIX_INVERSE
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  float start_time2;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  bool anim_ongoing;
  bool anim_ongoing2;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
  float anim_pos_at_change;
  float time_at_change;
  bool key_canceled;
  bool start_anim_chain;
};




class EnableML : public MainLoopItem
{
public:
  EnableML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, float start_time2, float end_time2) : e(e), ev(ev), next(next), start_time2(start_time2), end_time2(end_time2) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    if (time>=start_time2 && time<end_time2)
      {
	next->handle_event(env);
      }
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    if (time>=start_time2 && time<end_time2)
      {
	next->execute(env);
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  float start_time2, end_time2;
};


class MoveMLArray : public MainLoopItem
{
public:
  MoveMLArray(GameApi::Env &e, GameApi::EveryApi &ev, std::vector<MainLoopItem *> next, std::vector<GameApi::MN> mn) : e(e), ev(ev), next(next), mn(mn) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next[0]->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    int s = std::min(next.size(), mn.size());
    for(int i=0;i<s;i++)
      {
	next[i]->handle_event(env);
      }
  }
  void Prepare() { 
    int s = next.size();
    for(int i=0;i<s;i++)
    next[i]->Prepare(); }
  void execute(MainLoopEnv &env)
  {
    int s = std::min(next.size(), mn.size());
    for(int i=0;i<s;i++)
      {
	GameApi::SH s1;
	s1.id = env.sh_texture;
	GameApi::SH s2;
	s2.id = env.sh_array_texture;
	GameApi::SH s3;
	s3.id = env.sh_color;
	float time = (ev.mainloop_api.get_time()-start_time)/100.0;
	GameApi::M mat = ev.move_api.get_matrix(mn[i], time, ev.mainloop_api.get_delta_time());
	GameApi::M m2 = add_matrix2(e, env.env);
	GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
#ifdef HAS_MATRIX_INVERSE
	GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
	ev.shader_api.use(s2);
	ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
	ev.shader_api.use(s3);
	ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
	env.in_MV = find_matrix(e, mat2);
	
	Matrix old_env = env.env;
	env.env = find_matrix(e,mat2) * env.env;
	next[i]->execute(env);
	ev.shader_api.unuse(s3);
	env.env = old_env;
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  std::vector<MainLoopItem *> next;
  std::vector<GameApi::MN> mn;
};

EXPORT GameApi::ML GameApi::MovementNode::key_printer_ml(GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyPrinter(e,item));
}
class RepeatML : public MainLoopItem
{
public:
  RepeatML(MainLoopItem *next, float duration) : next(next), duration(duration) { }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    float time = e.time;
    float newtime = fmod(time, duration);
    MainLoopEnv ee = e;
    ee.time = newtime;
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e) { return next->handle_event(e); }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  float duration;

};
EXPORT GameApi::ML GameApi::MovementNode::repeat_ml(EveryApi &ev, GameApi::ML ml, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new RepeatML(item, duration/10.0));  
}
EXPORT GameApi::ML GameApi::MovementNode::move_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int clone_count, float time_delta)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new MoveML(e,ev,item, move, clone_count, time_delta));
}
EXPORT GameApi::ML GameApi::MovementNode::color_ml(EveryApi &ev, int color_num, ML ml, CC cc)
{
  MainLoopItem *item = find_main_loop(e, ml);
  ColorChange *cc2 = find_color(e, cc);
  return add_main_loop(e, new ColorML(e,ev,item, color_num, cc2));
}
EXPORT GameApi::ML GameApi::MovementNode::key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyActivateML(e,ev,item, move, key, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::comb_key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, int key_2, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new CombKeyActivateML(e,ev,item, move, key, key_2, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::temp_key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new TempKeyActivateML(e, ev, item, move, key, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::move_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,true,false,false,false,false,false,start_x, end_x));
}
EXPORT GameApi::ML GameApi::MovementNode::jump_ml(EveryApi &ev, GameApi::ML ml, int key_jump, float height, float jump_duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new JumpML(e,ev,item,key_jump, height, jump_duration));
}
EXPORT GameApi::ML GameApi::MovementNode::move_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0, false,true,false,false,false,false,start_y,end_y));
}
EXPORT GameApi::ML GameApi::MovementNode::move_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,false,false,true,false,false,false,start_z,end_z));
}

EXPORT GameApi::ML GameApi::MovementNode::rot_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,true,false,false,start_x, end_x));
}
EXPORT GameApi::ML GameApi::MovementNode::rot_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,true,false,false,true,false,start_y,end_y));
}
EXPORT GameApi::ML GameApi::MovementNode::rot_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,false,false,true,start_z,end_z));
}

EXPORT GameApi::ML GameApi::MovementNode::enable_ml(EveryApi &ev, GameApi::ML ml, float start_time, float end_time)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new EnableML(e,ev,item, start_time, end_time));

}
EXPORT GameApi::ML GameApi::MovementNode::move_ml_array(EveryApi &ev, std::vector<GameApi::ML> ml, std::vector<GameApi::MN> move)
{
  int s = ml.size();
  std::vector<MainLoopItem*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_main_loop(e, ml[i]));
    }
  return add_main_loop(e, new MoveMLArray(e,ev, vec2, move));
}
EXPORT GameApi::ML GameApi::MovementNode::key_event(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int type, int ch, int button, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyEventML(e,ev,item, move,type,ch,button, duration));
}

EXPORT GameApi::ML GameApi::MovementNode::wasd(EveryApi &ev, GameApi::ML ml,
					GameApi::MN w, GameApi::MN a,
					GameApi::MN s, GameApi::MN d,
					float duration)
{
  GameApi::ML m1 = key_event(ev, ml, w, 0x300, 'w',-1, duration);
  GameApi::ML m2 = key_event(ev, m1, a, 0x300, 'a',-1, duration);
  GameApi::ML m3 = key_event(ev, m2, s, 0x300, 's',-1, duration);
  GameApi::ML m4 = key_event(ev, m3, d, 0x300, 'd',-1, duration);
  return m4;
}

GameApi::BB GameApi::PickingApi::pick_area(GameApi::EveryApi &ev, float mouse_x, float mouse_y, float radius, int scr_size_x, int scr_size_y)
{
  BB empty = ev.bool_bitmap_api.empty(scr_size_x, scr_size_y);
  BB circle = ev.bool_bitmap_api.circle(empty, mouse_x, mouse_y, radius);
  return circle;
}
class MatrixVolume : public VolumeObject
{
public:
  MatrixVolume(Bitmap<bool> &b, Matrix m) : b(b),m(m) { }
  bool Inside(Point v) const
  {
    std::cout << "P: " << v.x << " " << v.y << " " << v.z << std::endl;
    Point p2 = v * m;
    p2.x *=800.0;
    p2.y *=600.0;
    p2.x /= p2.z;
    p2.y /= p2.z;
    p2.x += 800/2;
    p2.y += 600/2;
    std::cout << "pick_pos " << p2.x << " " << p2.y << " " << p2.z << std::endl;
    if (p2.x>0.0 && p2.y>0.0 && p2.x<b.SizeX() && p2.y<b.SizeY())
      {
	bool bb = b.Map(p2.x,p2.y);
	return bb;
      }
    return false;
  }
private:
  Bitmap<bool> &b;
  Matrix m;
};
GameApi::O GameApi::PickingApi::pick_volume(M in_P, BB pick)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, pick)->bitmap;
  Matrix m = find_matrix(e, in_P);
  for(int i=0;i<16;i++) std::cout << m.matrix[i] << " ";
  std::cout << std::endl;
  return add_volume(e, new MatrixVolume(*bm, m));
}
bool GameApi::PickingApi::picked(O o, float x, float y, float z)
{
  VolumeObject *obj = find_volume(e, o);
  Point p(x,y,z);
  bool b = obj->Inside(p);
  return b;
}

class TreeLevelImpl : public TreeLevel
{
public:
  TreeLevelImpl(std::vector<Movement*> vec) : vec(vec) { }
  int num_childs() const { return vec.size(); }
  Matrix get_child(int i, float time) const
  {
    // TODO tree cannot use delta_time properly
    Matrix m = vec[i]->get_whole_matrix(time, 1.0);
    return m;
  }
private:
  std::vector<Movement*> vec;
};

GameApi::TL GameApi::TreeApi::level(std::vector<MN> vec)
{
  std::vector<Movement*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_move(e, vec[i]));
    }
  return add_tree_level(e, new TreeLevelImpl(vec2));
}
class TreeImpl : public TreeStack
{
public:
  TreeImpl(std::vector<TreeLevel*> vec) : vec(vec) { }
  int num_levels() const
  {
    return vec.size();
  }
  TreeLevel *get_level(int i) const
  {
    return vec[i];
  }
private:
  std::vector<TreeLevel*> vec;
};
GameApi::T GameApi::TreeApi::tree(std::vector<TL> vec)
{
  std::vector<TreeLevel*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_tree_level(e, vec[i]));
    }
  return add_tree(e, new TreeImpl(vec2));
}

class TreeMainLoop : public MainLoopItem
{
public:
  TreeMainLoop(GameApi::Env &e, GameApi::EveryApi &ev, TreeStack *t, std::vector<MainLoopItem*> vec) : e(e), ev(ev), next(vec), tree2(t) 
  {
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  int shader_id() { return -1; }
  void execute_one(MainLoopEnv &env, Matrix m, int level)
  {
	  GameApi::M m2 = add_matrix2(e, m);
	  GameApi::SH s1;
	  s1.id = env.sh_texture;
	  GameApi::SH s2;
	  s2.id = env.sh_array_texture;
	  GameApi::SH s3;
	  s3.id = env.sh_color;
	  //GameApi::M mat = ev.move_api.get_matrix(mn, time);
	  //GameApi::M m2 = add_matrix2(e, env.env);
	  GameApi::M mat2 = m2; //ev.matrix_api.mult(mat,m2);
#ifdef HAS_MATRIX_INVERSE
	  GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
#endif
	  ev.shader_api.use(s1);
	  ev.shader_api.set_var(s1, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	  ev.shader_api.set_var(s1, "in_iMV", mat2i);
#endif
	  ev.shader_api.use(s2);
	  ev.shader_api.set_var(s2, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	  ev.shader_api.set_var(s2, "in_iMV", mat2i);
#endif
	  ev.shader_api.use(s3);
	  ev.shader_api.set_var(s3, "in_MV", mat2);
#ifdef HAS_MATRIX_INVERSE
	  ev.shader_api.set_var(s3, "in_iMV", mat2i);
#endif
	  env.in_MV = find_matrix(e, mat2);
	  
	  Matrix old_env = env.env;
	  env.env = find_matrix(e,mat2);
	  int ss = next.size();
	  if (level<ss) {
	    next[level]->execute(env);
	  }
	  ev.shader_api.unuse(s3);
	  env.env = old_env;
  }
  void execute_recurse(MainLoopEnv &e, Matrix mm, int current_level)
  {
    execute_one(e, mm, current_level);
    if (current_level >= tree2->num_levels()-1) return;
    TreeLevel *lvl = tree2->get_level(current_level);
    int s2 = lvl->num_childs();
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    for(int j=0;j<s2;j++)
      {
	Matrix m = lvl->get_child(j, time);
	Matrix m2 = m * mm;
	execute_recurse(e, m2, current_level+1);
      }    
  }
  void handle_event(MainLoopEvent &e)
  {
    int s = next.size();
    for(int i=0;i<s;i++)
      {
	next[i]->handle_event(e);
      }
  }
  void Prepare() {
    int s = next.size();
    for(int i=0;i<s;i++)
      {
	next[i]->Prepare();
      }
}
  void execute(MainLoopEnv &e)
  {
    execute_recurse(e, e.env, 0);
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> next;
  TreeStack *tree2;
  float start_time;
};

GameApi::ML GameApi::TreeApi::tree_ml(EveryApi &ev, T tree, std::vector<ML> vec)
{
  TreeStack *tree2 = find_tree(e, tree);
  std::vector<MainLoopItem*> vec2;
  int s=vec.size();
  for(int i=0;i<s;i++)
    {
      MainLoopItem* ii = find_main_loop(e, vec[i]);
      vec2.push_back(ii);
    }

  return add_main_loop(e, new TreeMainLoop(e,ev,tree2, vec2));
}

GameApi::P execute_one(GameApi::Env &e, GameApi::EveryApi &ev, const std::vector<GameApi::P> &v, Matrix m, int level)
{
  int s = v.size();
  if (level<s)
    return ev.polygon_api.matrix(v[level], add_matrix2(e,m));
  return ev.polygon_api.empty();
}
GameApi::P execute_recurse(GameApi::Env &e, GameApi::EveryApi &ev, const std::vector<GameApi::P> &v, Matrix mm, int current_level, TreeStack *tree2, float time)
{
  GameApi::P p0 = execute_one(e, ev, v, mm, current_level);
  if (current_level>=tree2->num_levels()-1) return p0;
  TreeLevel *lvl = tree2->get_level(current_level);
  int s2 = lvl->num_childs();
  std::vector<GameApi::P> vec;
  vec.push_back(p0);
  for(int j=0;j<s2;j++)
    {
      Matrix m = lvl->get_child(j,time);
      Matrix m2 = m * mm;
      GameApi::P p2 = execute_recurse(e, ev, v, m2, current_level+1, tree2, time);
      vec.push_back(p2);
    }
  return ev.polygon_api.or_array2(vec);
}

GameApi::P GameApi::TreeApi::tree_p(EveryApi &ev, T tree, std::vector<P> vec, float time)
{
  TreeStack *tree2 = find_tree(e, tree);
  return execute_recurse(e, ev, vec, Matrix::Identity(), 0, tree2, time); 
}

class DefaultMaterial : public MaterialForward
{
public:
  DefaultMaterial(GameApi::EveryApi &ev) : ev(ev) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
#ifndef HAS_INSTANCING
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
#else
    GameApi::PTS pts = ev.points_api.single_pts();
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev,p,pts);
#endif
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev, p, pts);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml = ev.materials_api.render_instanced_ml_fade(ev, p, pts, flip, start_time, end_time);
    return ml;
  }

private:
  GameApi::EveryApi &ev;
};

class ColourMaterial : public MaterialForward
{
public:
  ColourMaterial(GameApi::EveryApi &ev, float mix) : ev(ev),mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::PTS pts = ev.points_api.single_pts();
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev,p,pts);
    GameApi::ML mla=ev.polygon_api.colour_shader(ev, ml, mix);
    return mla;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev, p, pts);
    GameApi::ML mla=ev.polygon_api.colour_shader(ev, ml, mix);
    return mla;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML mla=ev.polygon_api.colour_shader(ev, ml, mix);
    return mla;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml = ev.materials_api.render_instanced_ml_fade(ev, p, pts, flip, start_time, end_time);
    GameApi::ML mla=ev.polygon_api.colour_shader(ev, ml, mix);
    return mla;
  }

private:
  GameApi::EveryApi &ev;
  float mix;
};


class ShadowMaterial : public MaterialForward
{
public:
  ShadowMaterial(GameApi::EveryApi &ev, GameApi::P objs, std::vector<GameApi::BM> vec, Point pos, int sx, int sy, unsigned int dark_color, float mix, float mix2) : ev(ev), objs(objs), vec(vec), pos(pos), sx(sx), sy(sy), dark_color(dark_color), mix(mix), mix2(mix2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::BM bm2 = bm; //ev.bitmap_api.flip_y(bm);
    std::vector<GameApi::BM> vec2 = vec;
    vec2.push_back(bm2);
    //GameApi::P I10=ev.polygon_api.or_elem(objs,p); 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,p,vec2);

    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, I17, vec2.size()-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, ml2, mix);

    GameApi::ML I17a=ev.polygon_api.render_vertex_array_ml2_texture(ev,objs,vec);

    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a, mix);

    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml});
    return arr;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    std::vector<GameApi::BM> vec2 = vec;
    vec2.push_back(bm);
    //GameApi::P p2 = ev.polygon_api.or_elem(objs,p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,vec2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, I17,mix);

    GameApi::ML I17a=ev.materials_api.render_instanced_ml_texture(ev,objs,pts,vec);
    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a,mix);

    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, ml, vec2.size()-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml2});
    return arr;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst2(p.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  GameApi::P objs;
  std::vector<GameApi::BM> vec;
  Point pos;
  int sx, sy;
  unsigned int dark_color;
  float mix;
  float mix2;
};

class ShadowMaterial2 : public MaterialForward
{
public:
  ShadowMaterial2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P objs, Point pos, int sx, int sy, unsigned int dark_color, float mix, float mix2, int numtextures) : e(e), ev(ev), objs(objs), pos(pos), sx(sx), sy(sy), dark_color(dark_color), mix(mix), mix2(mix2), numtextures(numtextures) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {

    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::P p2 = ev.polygon_api.texture_add(p, bm);
    //GameApi::BM bm2 = bm; //ev.bitmap_api.flip_y(bm);
    //std::vector<GameApi::BM> vec2 = vec;
    //vec2.push_back(bm2);
    //GameApi::P I10=ev.polygon_api.or_elem(objs,p); 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture2(ev,p2);

    FaceCollection *coll = find_facecoll(e, p2);
    int count = coll->NumTextures();
    if (numtextures>count) count=numtextures;
    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, I17, count-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, ml2, mix);

    GameApi::ML I17a=ev.polygon_api.render_vertex_array_ml2_texture2(ev,objs);

    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a, mix);

    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml});
    return arr;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::P p2 = ev.polygon_api.texture_add(p, bm);
    //std::vector<GameApi::BM> vec2 = vec;
    //vec2.push_back(bm);
    //GameApi::P p2 = ev.polygon_api.or_elem(objs,p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture2(ev,p,pts);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, I17,mix);

    GameApi::ML I17a=ev.materials_api.render_instanced_ml_texture2(ev,objs,pts);
    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a,mix);

    FaceCollection *coll = find_facecoll(e, p2);
    int count = coll->NumTextures();
    if (numtextures>count) count=numtextures;
    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, ml, count-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml2});
    return arr;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst2(p.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::P objs;
  std::vector<GameApi::BM> vec;
  Point pos;
  int sx, sy;
  unsigned int dark_color;
  float mix;
  float mix2;
  int numtextures;
};


class FogMaterial : public MaterialForward
{
public:
  FogMaterial(GameApi::EveryApi &ev, Material *next, float fog_dist, unsigned int dark_color, unsigned int light_color) : ev(ev), next(next),fog_dist(fog_dist), dark_color(dark_color), light_color(light_color) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    ml = ev.polygon_api.fog_shader(ev, ml, fog_dist, dark_color, light_color);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  float fog_dist;
  unsigned int dark_color, light_color;
};

class SkeletalMaterial : public MaterialForward
{
public:
  SkeletalMaterial(GameApi::EveryApi &ev) : ev(ev) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    // these are in skeletal_bind_materiaal
    //GameApi::P pp = ev.polygon_api.or_array2(vec);
    //GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);

    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(),vecattribs);
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    //GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //std::cout << "mat2_inst" << std::endl;
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml_fade(ev, va, pta, flip, start_time, end_time);
    return ml;
  }

private:
  GameApi::EveryApi &ev;
};


class TextureArrayMaterial : public MaterialForward
{
public:
  TextureArrayMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, int sx, int sy, float mix) : ev(ev), vec(vec),sx(sx),sy(sy), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;
    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta,flip,start_time,end_time);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> vec;
  int sx,sy;
  float mix;
};

class ForwardRenderToTextureId : public MainLoopItem
{
public:
  ForwardRenderToTextureId(VertexArraySet *s, MainLoopItem *next, TextureID *id) : s(s), next(next), id(id) { }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    id->render(e);
    s->texture_id = SPECIAL_TEX_ID+id->texture();
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    id->handle_event(e);
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  VertexArraySet *s;
  MainLoopItem *next;
  TextureID *id;
};
GameApi::ML GameApi::TextureApi::forward_to_txid(VA va, ML mainloop, TXID id)
{
  VertexArraySet *s = find_vertex_array(e, va);
  MainLoopItem *next = find_main_loop(e, mainloop);
  TextureID *txid = find_txid(e, id);
  return add_main_loop(e, new ForwardRenderToTextureId(s,next, txid));
}

class TextureIDMaterial : public MaterialForward
{
public:
  TextureIDMaterial(GameApi::EveryApi &ev, GameApi::TXID id, float mix) : ev(ev), txid(id), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {

    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I16,I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I16,I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I16,I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    // GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta, flip, start_time, end_time);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I16,I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
private:
  GameApi::EveryApi &ev;
  GameApi::TXID txid;
  float mix;
};

class TextureMaterial : public MaterialForward
{
public:
  TextureMaterial(GameApi::EveryApi &ev, GameApi::BM bm, float mix) : ev(ev), bm(bm), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta, flip, start_time, end_time);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  GameApi::BM bm;
  float mix;
};


class ManyTextureMaterial : public MaterialForward
{
public:
  ManyTextureMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> bm, float mix) : ev(ev), bm(bm), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    // END OF NOT WORKING
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> bm;
  float mix;
};

class CubemapTextureMaterial : public MaterialForward
{
public:
  CubemapTextureMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> bm, float mix, float mix2) : ev(ev), bm(bm), mix(mix),mix2(mix2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_cubemap(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.texture_cubemap_shader(ev, I17, mix,mix2);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_cubemap(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.texture_cubemap_shader(ev, I17,mix,mix2);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    // END OF NOT WORKING
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> bm;
  float mix;
  float mix2;
};


class ManyTextureMaterial2 : public MaterialForward
{
public:
  ManyTextureMaterial2(GameApi::EveryApi &ev, float mix) : ev(ev), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture2(ev,I10);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture2(ev,p,pts);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
#if 0
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    // END OF NOT WORKING
    GameApi::ML I18;
    I18.id = 0;
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
#if 0
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    GameApi::ML I18;
    I18.id =0;
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  float mix;
};



class BrashMetal : public MaterialForward
{
public:
  BrashMetal(GameApi::EveryApi &ev, Material *next, int count=80000, bool web=true) : ev(ev), next(next), count(count),web(web) { }

  virtual GameApi::ML mat2(GameApi::P I4) const
  {
    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst(I2,I5,I7);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I4 = ev.polygon_api.static_instancing(ev, p, pts);

    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst(I2,I5,I7);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;



    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    //return ml;

    // TODO how to implement instancing
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
    // TODO how to implement instancing
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I4 = ev.polygon_api.static_instancing(ev, p, pts);

    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst_fade(I2,I5,I7, flip, start_time, end_time);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;



    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    //return ml;

    // TODO how to implement instancing
  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  int count;
  bool web;
};

class ShadingMaterial1 : public MaterialForward
{
public:
  ShadingMaterial1(GameApi::EveryApi &ev, Material *next, float mix_val, float mix_val2) : ev(ev), next(next), mix_val(mix_val), mix_val2(mix_val2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat(p4.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat_inst(p4.id, pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p4.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p4.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  float mix_val;
  float mix_val2;
};

class ShadingMaterial2 : public MaterialForward
{
public:
  ShadingMaterial2(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3) : ev(ev), next(next), color1(color1), color2(color2), color3(color3) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
};


class SnowMaterial : public MaterialForward
{
public:
  SnowMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3, float mix_val) : ev(ev), next(next), color1(color1), color2(color2), color3(color3), mix_val(mix_val) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat(p3.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst(p3.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p3.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p3.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
  float mix_val;
};

class GIMaterial : public MaterialForward
{
public:
  GIMaterial(GameApi::EveryApi &ev, Material *next, GameApi::PTS pts, float obj_size) : ev(ev), next(next), pts(pts), obj_size(obj_size) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.gi_shader(ev, ml, pts, obj_size);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts2) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts2.id);
    GameApi::ML sh = ev.polygon_api.gi_shader(ev, ml, pts, obj_size);
    return sh;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML sh = ev.polygon_api.gi_shader(ev, ml, pts, obj_size);
    return sh;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts2, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts2.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.gi_shader(ev, ml, pts, obj_size);
    return sh;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::PTS pts;
  float obj_size;
};

class PhongMaterial : public MaterialForward
{
public:
  PhongMaterial(GameApi::EveryApi &ev, Material *next, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow) : ev(ev), next(next), light_dir_x(light_dir_x), light_dir_y(light_dir_y), light_dir_z(light_dir_z), ambient(ambient), highlight(highlight), pow(pow) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat(p0.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst(p0.id, pts.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p0.id, pta.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p0.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float light_dir_x, light_dir_y, light_dir_z;
  unsigned int ambient, highlight;
  float pow;
};


class BumpPhongMaterial : public MaterialForward
{
public:
  BumpPhongMaterial(GameApi::EveryApi &ev, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, GameApi::FB bm_1, float bump_width) : ev(ev), light_dir_x(light_dir_x), light_dir_y(light_dir_y), light_dir_z(light_dir_z), ambient(ambient), highlight(highlight), pow(pow) {
    GameApi::FB bm_11 = ev.bitmap_api.mul_fb(bm_1,40.0);
    GameApi::BM I17=ev.bitmap_api.bump_map(bm_11,bump_width);
    GameApi::BM I18=ev.bitmap_api.flip_y(I17);
    GameApi::BM I19=ev.bitmap_api.flip_x(I18);
    GameApi::BM bm_2=ev.float_bitmap_api.to_grayscale_color(bm_1,255,255,255,255,0,0,0,0);
    GameApi::BM bm_3=ev.bitmap_api.flip_y(bm_2);
    GameApi::BM bm_4=ev.bitmap_api.flip_x(bm_3);
    bm.push_back(I19);
    bm.push_back(bm_4);
  }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p00, 0xff000000);
    //GameApi::ML ml;
    GameApi::ML ml=ev.polygon_api.render_vertex_array_ml2_texture(ev,p1,bm);
    //ml.id = next->mat(p1.id);
    GameApi::ML sh = ev.polygon_api.bump_phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p00, 0xff000000);
    //GameApi::ML ml;
    GameApi::ML ml=ev.materials_api.render_instanced_ml_texture(ev,p1,pts,bm);
    //ml.id = next->mat_inst(p1.id, pts.id);
    GameApi::ML sh = ev.polygon_api.bump_phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
#if 0
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p1.id, pta.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;
#endif
    GameApi::ML ml;
    ml.id=-1;
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
#if 0
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p1.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;
#endif
    GameApi::ML ml;
    ml.id=-1;
    return ml;
  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float light_dir_x, light_dir_y, light_dir_z;
  unsigned int ambient, highlight;
  float pow;
  std::vector<GameApi::BM> bm;
};



class DynLightsMaterial : public MaterialForward
{
public:
  DynLightsMaterial(GameApi::EveryApi &ev, Material *next, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point) : ev(ev), next(next), light_pos_x(light_pos_x), light_pos_y(light_pos_y), light_pos_z(light_pos_z), dist(dist), dyn_point(dyn_point) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);

    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float light_pos_x, light_pos_y, light_pos_z;
  float dist;
  int dyn_point;
};



class FlatMaterial : public MaterialForward
{
public:
  FlatMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3) : ev(ev), next(next), color1(color1), color2(color2), color3(color3) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat(p1.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst(p1.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p1.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p1.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
};

class BloomMaterial : public MaterialForward
{
public:
  BloomMaterial(GameApi::EveryApi &ev, Material *next, GameApi::BM bm, float r_cut, float g_cut, float b_cut, float pixel_x, float pixel_y) : ev(ev), next(next),bm(bm),r_cut(r_cut), g_cut(g_cut), b_cut(b_cut), pixel_x(pixel_x), pixel_y(pixel_y) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    // TODO next material support.
    // requires adding m_bind/m_bind_inst/m_bind_inst2/m_bind_inst_fade to Material class.
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind(p,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
/*
   GameApi::P I1=ev.polygon_api.fullscreen_quad(ev);
   GameApi::P I2=ev.polygon_api.texcoord_manual(I1,0.0,0.0,1.0,0.0,1.0,1.0,0.0,1.0);
   GameApi::P I3=p; 
   //GameApi::P I4=ev.polygon_api.color_from_normals(I3);
   //GameApi::ML I5=ev.polygon_api.render_vertex_array_ml2(ev,I3);
   GameApi::ML I5;
   I5.id = next->mat(I3.id);
   GameApi::ML II1 = ev.move_api.move_ml(ev, I5, move, 1,10.0);
   GameApi::TXID I1a=ev.fbo_api.fbo_ml(ev,II1,800,600,false);
   GameApi::MT I9=ev.materials_api.textureid(ev,I1a,0.9);
   GameApi::ML I10=ev.materials_api.bind(I2,I9);
   GameApi::MN I11=ev.move_api.empty();
   GameApi::MN I12=ev.move_api.trans2(I11,0,0,-0.001);
   GameApi::ML I13=ev.move_api.move_ml(ev,I10,I12,1,10.0);
   GameApi::ML I14=ev.sprite_api.turn_to_2d(ev,I13,0.0,0.0,800.0,600.0);
   GameApi::P I15=ev.polygon_api.fullscreen_quad(ev);
   GameApi::P I16=ev.polygon_api.texcoord_manual(I15,0.0,0.0,1.0,0.0,1.0,1.0,0.0,1.0);
   GameApi::P I17=p;
   //GameApi::ML I19=ev.polygon_api.render_vertex_array_ml2(ev,I17);
   GameApi::ML I19;
   I19.id = next->mat(I17.id);
   GameApi::TXID I2a=ev.fbo_api.fbo_ml(ev,I19,800,600,false);
   GameApi::MT I23=ev.materials_api.textureid(ev,I2a,0);
   GameApi::ML I24=ev.materials_api.bind(I16,I23);
   GameApi::ML I25=ev.polygon_api.bloom1_shader(ev,I24,0.4,0.4,0.4);
   GameApi::ML I26=ev.polygon_api.blur_shader(ev,I25,5,0);
   GameApi::ML I27=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I14,I26});
   return I27;
*/
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst(p,pts,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst2(p,pta,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst_fade(p,pts,I10,flip,start_time,end_time);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;

  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::BM bm;
  float r_cut, g_cut, b_cut;
  float pixel_x, pixel_y;
};

class FurMaterial : public MaterialForward
{
public:
  FurMaterial(GameApi::EveryApi &ev, Material *next, GameApi::PT center, float dist, float max_angle, int count, float size, int cone_numfaces) : ev(ev), next(next), center(center), dist(dist), max_angle(max_angle), count(count), size(size), cone_numfaces(cone_numfaces) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::PTS pts = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat(pl2.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::PTS pts2 = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts2, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst(pl2.id, pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::PTS pts = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst2(pl2.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::PTS pts2 = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts2, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(pl2.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::PT center;
  float dist;
  float max_angle;
  int count;
  float size;
  int cone_numfaces;
};
class Marble : public MaterialForward
{
public:
  Marble(GameApi::EveryApi &ev, Material *next, int count, float cubesize) : ev(ev), next(next), count(count), cubesize(cubesize) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);
    
        GameApi::ML ml;
    ml.id = next->mat(I7.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst(I7.id, pts.id);
    return ml;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst2(I7.id, pta.id);

    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(I7.id, pts.id, flip, start_time, end_time);

    return ml;
  }
  
private:
  GameApi::EveryApi &ev;
  Material *next;
  int count;
  float cubesize;
};

class ChooseColorMaterial : public MaterialForward
{
public:
  ChooseColorMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color, float mix_val) : ev(ev), next(next),color(color), mix_val(mix_val) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color;
  float mix_val;
};
#endif
#ifdef FIRST_PART
EXPORT GameApi::MT GameApi::MaterialsApi::def(EveryApi &ev)
{
  return add_material(e, new DefaultMaterial(ev));
}
EXPORT GameApi::MT GameApi::MaterialsApi::skeletal(EveryApi &ev)
{
  return add_material(e, new SkeletalMaterial(ev));
}
EXPORT GameApi::MT GameApi::MaterialsApi::fur(EveryApi &ev, MT nxt, PT center, float dist, float max_angle, int count, float size, int cone_numfaces)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FurMaterial(ev, mat, center, dist, max_angle, count, size, cone_numfaces));
}

EXPORT GameApi::MT GameApi::MaterialsApi::bloom(EveryApi &ev, MT nxt, BM bm, float r_cut, float g_cut, float b_cut, float pixel_x, float pixel_y)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new BloomMaterial(ev, mat, bm, r_cut, g_cut, b_cut, pixel_x, pixel_y));
}

				      
EXPORT GameApi::MT GameApi::MaterialsApi::texture(EveryApi &ev, BM bm, float mix)
{
  return add_material(e, new TextureMaterial(ev, bm,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::textureid(EveryApi &ev, TXID txid, float mix)
{
  return add_material(e, new TextureIDMaterial(ev, txid,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_many(EveryApi &ev, std::vector<BM> vec, float mix)
{
  return add_material(e, new ManyTextureMaterial(ev, vec,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::colour_material(EveryApi &ev, float mix)
{
  return add_material(e, new ColourMaterial(ev,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_cubemap(EveryApi &ev, std::vector<BM> vec, float mix, float mix2)
{
  return add_material(e, new CubemapTextureMaterial(ev, vec,mix,mix2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_many2(EveryApi &ev, float mix)
{
  return add_material(e, new ManyTextureMaterial2(ev,mix));
}


EXPORT GameApi::MT GameApi::MaterialsApi::texture_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy, float mix)
{
  return add_material(e, new TextureArrayMaterial(ev, vec, sx,sy,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::snow(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3, float mix_val)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new SnowMaterial(ev, mat, color1, color2, color3, mix_val));
}
EXPORT GameApi::MT GameApi::MaterialsApi::phong(EveryApi &ev, MT nxt, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new PhongMaterial(ev, mat, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow));
}
EXPORT GameApi::MT GameApi::MaterialsApi::gi(EveryApi &ev, MT nxt, PTS points, float obj_size)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new GIMaterial(ev, mat, points, obj_size));
}
EXPORT GameApi::MT GameApi::MaterialsApi::bump_phong(EveryApi &ev, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, FB bm, float bump_width)
{
  return add_material(e, new BumpPhongMaterial(ev, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow,bm, bump_width));
}
EXPORT GameApi::MT GameApi::MaterialsApi::fog(EveryApi &ev, MT nxt, float fog_dist, unsigned int dark_color, unsigned int light_color)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FogMaterial(ev, mat, fog_dist, dark_color, light_color));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shadow(EveryApi &ev, GameApi::P p, std::vector<BM> vec, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2)
{
  return add_material(e, new ShadowMaterial(ev, p, vec, Point(p_x,p_y,p_z),sx,sy , dark_color,mix,mix2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shadow2(EveryApi &ev, GameApi::P p, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2, int numtextures)
{
  return add_material(e, new ShadowMaterial2(e,ev, p, Point(p_x,p_y,p_z),sx,sy , dark_color,mix,mix2,numtextures));
}
EXPORT GameApi::MT GameApi::MaterialsApi::dyn_lights(EveryApi &ev, MT nxt, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new DynLightsMaterial(ev, mat, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shading1(EveryApi &ev, MT nxt, float mix_val, float mix_val2)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ShadingMaterial1(ev, mat, mix_val,mix_val2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shading2(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ShadingMaterial2(ev, mat, color1, color2, color3));
}
EXPORT GameApi::MT GameApi::MaterialsApi::flat(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FlatMaterial(ev, mat, color1, color2, color3));
}
EXPORT GameApi::MT GameApi::MaterialsApi::choose_color(EveryApi &ev, MT nxt, unsigned int color, float mix_val)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ChooseColorMaterial(ev, mat,color,mix_val));
}
EXPORT GameApi::MT GameApi::MaterialsApi::brashmetal(EveryApi &ev, MT nxt, int count, bool web)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new BrashMetal(ev, mat, count, web));
}
EXPORT GameApi::MT GameApi::MaterialsApi::marble(EveryApi &ev, MT nxt, int count, float cubesize)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new Marble(ev, mat, count, cubesize));
}
#endif // FIRST_PART
#ifdef SECOND_PART

#if 0
EXPORT GameApi::ML GameApi::MaterialsApi::snow(EveryApi &ev, P p)
{
  P p0 = ev.polygon_api.recalculate_normals(p);
  P p1 = ev.polygon_api.color_from_normals(p0);
  P p2 = ev.polygon_api.color_grayscale(p1);
  P p3 = ev.polygon_api.mix_color(p1,p2,0.5);
  VA va = ev.polygon_api.create_vertex_array(p3,false);
  ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  ML sh = ev.polygon_api.shading_shader(ev, ml, 0xffaaaaaa, 0xffeeeeee, 0xffffffff);
  return sh;
}
#endif

class WebMaterial : public MaterialForward
{
public:
  WebMaterial(GameApi::EveryApi &ev, Material *next, float val, float linewidth, unsigned int color) : ev(ev),next(next),val(val),linewidth(linewidth),color(color) {}
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    g_low->ogl->glLineWidth(linewidth);
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_ml2(ev,I5);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat(I8.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    g_low->ogl->glLineWidth(linewidth);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml3(ev,I5,pts);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst(I8.id,pts.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    //GameApi::ML I12=ev.mainloop_api.prepare_pts(I11,pts);
    return I11;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    g_low->ogl->glLineWidth(linewidth);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml2(ev,I5,pta);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst2(I8.id,pta.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    g_low->ogl->glLineWidth(linewidth);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);

    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml2(ev,I5,pta);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst_fade(I8.id,pts.id, flip, start_time, end_time);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float val;
  float linewidth;
  unsigned int color;
};

EXPORT GameApi::MT GameApi::MaterialsApi::web(EveryApi &ev, MT nxt, float val, float linewidth, unsigned int color)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new WebMaterial(ev,mat,val,linewidth,color));
}


#if 0
EXPORT GameApi::ML GameApi::MaterialsApi::web(EveryApi &ev, P p)
{

  // TODO: this might require std::function<ML(ev,P)> parameter, but
  // builder can't do it yet.

  P I2=p;
LI I3=ev.lines_api.from_polygon(I2);
LI I4=ev.lines_api.change_color(I3,0xff000000);
LLA I5=ev.lines_api.prepare(I4);
ML I6=ev.lines_api.render_ml(ev,I5);
 P I8=p; 
VA I9=ev.polygon_api.create_vertex_array(I8,true);
ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
ML I11=ev.mainloop_api.array_ml(std::vector<ML>{I6,I10});
 return I11;
}

#endif

EXPORT GameApi::ML GameApi::MaterialsApi::bind(P p, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = 0;
  if (mat2) {
    val = mat2->mat(p.id);
  }
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst2(P p, PTA pta, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = 0;
  if (mat2) {
    val = mat2->mat_inst2(p.id,pta.id);
  }
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst(P p, PTS pts, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = 0;
  if (mat2) {
    val = mat2->mat_inst(p.id,pts.id);
  }
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst_fade(P p, PTS pts, MT mat, bool flip, float start_time, float end_time)
{
  Material *mat2 = find_material(e, mat);
  int val = 0;
  if (mat2) {
    val = mat2->mat_inst_fade(p.id,pts.id, flip, start_time, end_time);
  }
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
class RenderInstanced : public MainLoopItem
{
public:
  RenderInstanced(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time)  { firsttime = true; initialized=false; shader.id=-1; va.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void Prepare() {
    if (initialized) { std::cout << "Prepare in RenderInstanced called twice" << std::endl; return; }
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->Prepare(); 
    pta = ev.points_api.prepare(pts);
    va = ev.polygon_api.create_vertex_array(p,false);
    initialized=true;
  }
  void execute(MainLoopEnv &e)
  {
    if (!initialized) { std::cout << "Prepare not called in RenderInstanced!" << std::endl; return; }
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    //if (firsttime) { obj2->Prepare(); }
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (firsttime || shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	//ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
      }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    fade = false;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.print_log(sh);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  bool initialized;
};

class RenderInstancedTex : public MainLoopItem
{
public:
  RenderInstancedTex(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time, std::vector<GameApi::BM> bm) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time),bm(bm)  { firsttime = true; initialized=false; shader.id=-1; va.id=-1;}
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void Prepare() {
    if (initialized) { std::cout << "Prepare in RenderInstanced called twice" << std::endl; return; }
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    pta = ev.points_api.prepare(pts);
    va = ev.polygon_api.create_vertex_array(p,true);
    std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev,bm);
    va = ev.texture_api.bind_many(va, id);
    initialized=true;
  }
  void execute(MainLoopEnv &e)
  {
    if (!initialized) { std::cout << "Prepare not called in RenderInstanced!" << std::endl; return; }
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (va.id!=-1 && ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      if (va.id!=-1) {
	ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh); 
	firsttime = false;
      }
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    if (va.id!=-1)
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.print_log(sh);

    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  std::vector<GameApi::BM> bm;
  bool initialized;
};


class RenderInstancedTex_id : public MainLoopItem
{
public:
  RenderInstancedTex_id(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time, std::vector<GameApi::TXID> *bm) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time),bm(bm)  { firsttime = true; shader.id=-1; initialized=false; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void Prepare() {
    if (initialized) { std::cout << "Prepare in RenderInstanced called twice" << std::endl; return; }
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    pta = ev.points_api.prepare(pts);
    va = ev.polygon_api.create_vertex_array(p,true);
    std::vector<GameApi::TXID> id = *bm; //ev.texture_api.prepare_many(ev,bm);
    va = ev.texture_api.bind_many(va, id);
    initialized=true;
  }
  void execute(MainLoopEnv &e)
  {
    if (!initialized) { std::cout << "Prepare not called in RenderInstanced!" << std::endl; return; }

    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }
    // dynamically change textures
    VertexArraySet *s = find_vertex_array(env, va);
    s->texture_many_ids = std::vector<int>();
    int sk = bm->size();
    for(int i=0;i<sk;i++)
      {
	GameApi::TXID id = bm->operator[](i);
	TextureID *iid = find_txid(env,id);
	int tex = iid->texture();

	s->texture_many_ids.push_back(tex);
      }
    
    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.print_log(sh);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  std::vector<GameApi::TXID> *bm;
  bool initialized;
};


class RenderInstancedCubemap : public MainLoopItem
{
public:
  RenderInstancedCubemap(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time, std::vector<GameApi::BM> bm) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time),bm(bm)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void Prepare() {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,true);

	GameApi::BM right,left,top,bottom,back,front;
	right.id = 0;
	left.id = 0;
	top.id = 0;
	bottom.id = 0;
	back.id = 0;
	front.id = 0;
	if (bm.size()>0) right = bm[0];
	if (bm.size()>1) left = bm[1];
	if (bm.size()>2) top = bm[2];
	if (bm.size()>3) bottom = bm[3];
	if (bm.size()>4) back = bm[4];
	if (bm.size()>5) front = bm[5];
	if (left.id==0) left=right;
	if (top.id==0) top=right;
	if (bottom.id==0) bottom=right;
	if (back.id==0) back=right;
	if (front.id==0) front=right;
	GameApi::TXID id = ev.texture_api.prepare_cubemap(ev, right,left,top,bottom,back,front);
	va = ev.texture_api.bind_cubemap(va, id);

  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  std::vector<GameApi::BM> bm;
};



class RenderInstancedTex2 : public MainLoopItem
{
public:
  RenderInstancedTex2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void Prepare() {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    pta = ev.points_api.prepare(pts);
    va = ev.polygon_api.create_vertex_array(p,true);
    std::vector<GameApi::BM> bm;
    int s = ev.polygon_api.p_num_textures(p);
    for(int i=0;i<s;i++) {
      ev.polygon_api.p_gen_texture(p,i);
      bm.push_back(ev.polygon_api.p_texture(p,i));
    }
    std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev,bm);
    va = ev.texture_api.bind_many(va, id);
    
  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.print_log(sh);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
};



class RenderInstanced2 : public MainLoopItem
{
public:
  RenderInstanced2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::VA va, GameApi::PTA pta, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), va(va), pta(pta), fade(fade), flip(flip), start_time(start_time), end_time(end_time) { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Prepare() {
  }
  void execute(MainLoopEnv &e)
  {
    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	    u_f = ev.uber_api.f_texture(u_f);
	  }

	if (ev.polygon_api.is_array_texture(va))

	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
	    if (e.us_vertex_shader==-1)
	    		u_v = ev.uber_api.v_texture_arr(u_v);
	    if (e.us_fragment_shader==-1)
	    		u_f = ev.uber_api.f_texture_arr(u_f);
	       }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_colour(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_colour(u_f);
	  }
      }
#endif
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced2::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced2::USESHADER" << std::endl;
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
#ifdef HAS_MATRIX_INVERSE
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
#endif
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "in_POS", e.in_POS);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime) {
      firsttime = false;
      //pta = ev.points_api.prepare(pts);
      //va = ev.polygon_api.create_vertex_array(p,false);
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.num_points(pta);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }
    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.print_log(sh);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  //GameApi::P p;
  //GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
};

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml(GameApi::EveryApi &ev, P p, PTS pts)
{
#ifdef HAS_INSTANCING
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts, false,false,0.0,0.0
					      ));
#else
  //P inst = ev.polygon_api.static_instancing(ev, p, pts);
  GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
  GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  GameApi::ML ml2 = ev.move_api.local_move(ev,ml, pts);
  return ml2;
#endif
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_texture(GameApi::EveryApi &ev, P p, PTS pts, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedTex(e, ev, p,pts, false,false,0.0,0.0,bm));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_texture_id(GameApi::EveryApi &ev, P p, PTS pts, std::vector<TXID> *bm)
{
  return add_main_loop(e, new RenderInstancedTex_id(e, ev, p,pts, false,false,0.0,0.0,bm));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_cubemap(GameApi::EveryApi &ev, P p, PTS pts, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedCubemap(e, ev, p,pts, false,false,0.0,0.0,bm));
}

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_texture2(GameApi::EveryApi &ev, P p, PTS pts)
{
  return add_main_loop(e, new RenderInstancedTex2(e, ev, p,pts, false,false,0.0,0.0));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_fade(GameApi::EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time)
{
#ifdef HAS_INSTANCING
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts, true,flip,start_time,end_time));
#else
  // P inst = ev.polygon_api.static_instancing(ev, p, pts);
  GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
  GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  GameApi::ML ml2 = ev.move_api.local_move(ev,ml, pts);
  return ml2;
#endif
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_fade_texture(GameApi::EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedTex(e, ev, p,pts, true,flip,start_time,end_time,bm));  
}


EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced2_ml(GameApi::EveryApi &ev, VA va, PTA pta)
{
  return add_main_loop(e, new RenderInstanced2(e, ev, va,pta,false,false,0.0,0.0));
}

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced2_ml_fade(GameApi::EveryApi &ev, VA va, PTA pta, bool flip, float start_time, float end_time)
{
  return add_main_loop(e, new RenderInstanced2(e, ev, va,pta, true,flip,start_time,end_time));  
}


class V_ShaderCallFunction : public ShaderCall
{
public:
  V_ShaderCallFunction(std::string funcname, ShaderCall *next, std::string defines) : funcname(funcname), next(next),defines(defines) { id=-1; }
  int index(int base) const {
    id = next->index(base)+1; 
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 pos";
    out+=ss.str();
    out+=" = ";
    out+=funcname;
    out+="(pos";
    out+=ss2.str();
    out+=");\n";
    return out;
  }
  std::string define_strings() const { 
    std::string s1 = defines;
    std::string s2 = next->define_strings();
    std::string m = (s1=="" ||s2=="") ? "" : " ";
    return s1 + m + s2; 
  }
private:
  std::string funcname;
  ShaderCall *next;
  mutable int id;
  std::string defines;
};

class F_ShaderCallFunction : public ShaderCall
{
public:
  F_ShaderCallFunction(std::string funcname, ShaderCall *next, std::string defines) : funcname(funcname), next(next),defines(defines) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=funcname;
    out+="(rgb";
    out+=ss2.str();
    out+=");\n";
    return out;
  }
  std::string define_strings() const { 
    std::string s1 = defines;
    std::string s2 = next->define_strings();
    std::string m = (s1=="" ||s2=="") ? "" : " ";
    return s1 + m + s2; 
  }
private:
  std::string funcname;
  ShaderCall *next;
  mutable int id;
  std::string defines;
};
class EmptyV : public ShaderCall
{
public:
  EmptyV() : id(-1) { }
  int index(int base) const { id = base; return base; }
  std::string func_call() const
  {
    std::string out;
    out+="vec3 p = mix(in_Position, in_Position2, in_POS);\n";
    std::stringstream ss;
    ss << id+1;
    out+="vec4 pos" + ss.str() + " = vec4(p,1.0);\n";
    return out;
  }
  std::string define_strings() const { return ""; }
private:
  mutable int id;
};

class EmptyF : public ShaderCall
{
public:
  EmptyF(bool transparent) : transparent(transparent) { id = -1; }
  int index(int base) const { id = base; return base; }
  std::string func_call() const
  {
    std::string out;
    std::stringstream ss;
    ss << id+1;
    if (transparent)
      out+="vec4 rgb" + ss.str() + " = vec4(0.0,0.0,0.0,0.0);\n";
    else
      out+="vec4 rgb" + ss.str() + " = vec4(0.0,0.0,0.0,1.0);\n";
    return out;
  }
  std::string define_strings() const { return ""; }
private:
  mutable int id;
  bool transparent;
};

GameApi::US GameApi::UberShaderApi::v_empty()
{
  return add_uber(e, new EmptyV());
}
GameApi::US GameApi::UberShaderApi::v_color_from_normals(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("color_from_normals", next,""));
}
GameApi::US GameApi::UberShaderApi::v_recalc_normal(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("recalc_normal", next, "EX_NORMAL IN_NORMAL IN_N"));
}

GameApi::US GameApi::UberShaderApi::v_inst(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("inst", next, "INST"));
}

GameApi::US GameApi::UberShaderApi::v_ambient(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("ambient", next,""));
}
GameApi::US GameApi::UberShaderApi::v_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("phong", next,"EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}
GameApi::US GameApi::UberShaderApi::v_gi(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("gi", next,"IN_POSITION EX_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_colour_with_mix(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("colour_with_mix", next,"EX_COLOR IN_COLOR COLOR_MIX"));
}
GameApi::US GameApi::UberShaderApi::v_bump_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("bump_phong", next,"BUMPPHONG EX_TEXCOORD IN_TEXCOORD EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}
GameApi::US GameApi::UberShaderApi::v_fog(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("fog", next,"EX_POSITION IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_shadow(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("shadow", next,"EX_POSITION IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_dyn_lights(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("dyn_lights", next,"IN_NORMAL EX_NORMAL2 IN_POSITION EX_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_diffuse(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("diffuse", next, "EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_specular(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("specular", next, "EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_passall(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("passall", next,"EX_COLOR EX_TEXCOORD EX_NORMAL EX_POSITION IN_COLOR IN_TEXCOORD IN_NORMAL IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_pass_position(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("passpos", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_point_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("point_light", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_snoise(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("snoise", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("light", next,""));
}

GameApi::US GameApi::UberShaderApi::v_ref(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("ref", next,"EX_POSITION IN_POSITION EX_NORMAL IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_wave(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("wave", next, "IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_toon(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("toon", next,"EX_NORMAL IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_texture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("texture", next,"EX_TEXCOORD IN_TEXCOORD"));
}
GameApi::US GameApi::UberShaderApi::v_manytexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("manytextures", next,"MANYTEXTURES EX_TEXCOORD IN_TEXCOORD"));
}
GameApi::US GameApi::UberShaderApi::v_cubemaptexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("cubemaptextures", next,"CUBEMAPTEXTURES EX_TEXCOORD IN_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::v_texture_arr(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("texture_arr", next,"EX_TEXCOORD IN_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::v_colour(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("colour", next,"EX_COLOR IN_COLOR"));
}

GameApi::US GameApi::UberShaderApi::v_blur(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("blur", next, "EX_TEXCOORD IN_TEXCOORD"));
}
class V_DistFieldMesh : public ShaderCall
{
public:
  V_DistFieldMesh(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 pos";
    out+=ss.str();
    out+=" = ";
    out+=funccall_to_string_with_replace(mod, "p1", "pos" + ss2.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};


class F_MeshColor : public ShaderCall
{
public:
  F_MeshColor(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec3 pos";
    out+=ss.str();
    out+=" = ";
    out+="ex_Position;\n";
    //out+="pos" + ss.str() + ".x/=gl_FragCoord.w;\n";
    //out+="pos" + ss.str() + ".y/=gl_FragCoord.w;\n";
    //out+="pos" + ss.str() + ".z/=gl_FragCoord.w;\n";
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=color_funccall_to_string_with_replace(mod, "pt", "pos" + ss.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return "EX_POSITION " + next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};

class F_Sandbox : public ShaderCall
{
public:
  F_Sandbox(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec3 pos";
    out+=ss.str();
    out+=" = ";
    out+="ex_TexCoord;\n";
    out+="pos" + ss.str() + ".x*=300.0;\n";
    out+="pos" + ss.str() + ".y*=300.0;\n";
    //out+="pos" + ss.str() + ".z/=gl_FragCoord.w;\n";
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=color_funccall_to_string_with_replace(mod, "pt", "pos" + ss.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return "EX_TEXCOORD " + next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};


GameApi::US GameApi::UberShaderApi::v_dist_field_mesh(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e,us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new V_DistFieldMesh(next,mod));
}
GameApi::US GameApi::UberShaderApi::f_mesh_color(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e, us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new F_MeshColor(next,mod));
}
GameApi::US GameApi::UberShaderApi::f_sandbox(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e, us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new F_Sandbox(next,mod));
}
GameApi::US GameApi::UberShaderApi::v_skeletal(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("skeletal", next, "SKELETAL"));
}


GameApi::US GameApi::UberShaderApi::f_empty(bool transparent)
{
  return add_uber(e, new EmptyF(transparent));
}

GameApi::US GameApi::UberShaderApi::f_diffuse(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("diffuse", next,"EX_NORMAL2 EX_LIGHTPOS2 LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("phong", next,"EX_NORMAL2 EX_LIGHTPOS2 LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_gi(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("gi", next,"EX_POSITION"));
}

GameApi::US GameApi::UberShaderApi::f_colour_with_mix(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("colour_with_mix", next,"EX_COLOR COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_bump_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("bump_phong", next,"BUMPPHONG EX_TEXCOORD EX_NORMAL2 EX_LIGHTPOS2 LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_fog(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("fog", next,"EX_POSITION"));
}
GameApi::US GameApi::UberShaderApi::f_shadow(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("shadow", next,"EX_POSITION MANYTEXTURES COLOR_MIX"));
}
GameApi::US GameApi::UberShaderApi::f_dyn_lights(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("dyn_lights", next,"EX_NORMAL2 EX_POSITION"));

}

GameApi::US GameApi::UberShaderApi::f_ambient(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("ambient", next,"LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_color_from_id(US us, int id)
{
  ShaderCall *next = find_uber(e, us);
  std::stringstream ss;
  ss << id;
  return add_uber(e, new F_ShaderCallFunction("color_from_id_" + ss.str(), next, "COLOR_ARRAY"));
}

GameApi::US GameApi::UberShaderApi::f_specular(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("specular", next,"EX_NORMAL2 EX_LIGHTPOS2 SPECULAR_SIZE LEVELS"));
}

GameApi::US GameApi::UberShaderApi::f_color_from_normals(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("color_from_normals", next,"EX_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::f_point_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  std::string def = next->define_strings();
  def+="EX_POSITION LIGHTPOS";
  std::string cur_def = "LIGHTPOS";
  std::vector<std::string> str = replace_c_template_unique_ids(def, cur_def);
  std::string id = str[str.size()-1];
  return add_uber(e, new F_ShaderCallFunction("point_light" + id, next,"EX_POSITION LIGHTPOS"));
}

GameApi::US GameApi::UberShaderApi::f_bands(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("bands", next,""));
}

GameApi::US GameApi::UberShaderApi::f_snoise(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("snoise", next,"EX_POSITION"));
}

GameApi::US GameApi::UberShaderApi::f_blur(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("blur", next,"EX_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::f_ref(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("ref", next,"EX_POSITION EX_NORMAL"));
}
GameApi::US GameApi::UberShaderApi::v_custom(US us, std::string v_funcname)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction(v_funcname, next,"MANYTEXTURES IN_POSITION EX_POSITION IN_TEXCOORD EX_TEXCOORD COLOR_MIX"));

}
GameApi::US GameApi::UberShaderApi::f_custom(US us, std::string f_funcname)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction(f_funcname, next,"MANYTEXTURES EX_POSITION IN_TEXCOORD EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("light", next,""));
}

GameApi::US GameApi::UberShaderApi::f_toon(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("toon", next,"EX_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::f_texture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("texture", next,"EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_manytexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("manytextures", next,"MANYTEXTURES EX_TEXCOORD COLOR_MIX"));
}
GameApi::US GameApi::UberShaderApi::f_cubemaptexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("cubemaptextures", next,"CUBEMAPTEXTURES EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_texture_arr(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("texture_arr", next,"EX_TEXCOORD TEXTURE_ARRAY COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_colour(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("colour", next,"EX_COLOR"));
}
GameApi::US GameApi::UberShaderApi::f_choose_color(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("choose_color", next,"CHOOSE_COLOR"));
}

class LineCurve : public Curve<Point>
{
public:
  LineCurve(Point p1, Point p2) : p1(p1), p2(p2) {}
  float Size() const {
    return 1.0;
  }
  Point Index(float p) const
  {
    return Point((1.0-p)*Vector(p1) + p*Vector(p2));
  }
private:
  Point p1,p2;
};

class CircleCurveXY : public Curve<Point>
{
public:
  CircleCurveXY(Point center, float r) : center(center), r(r) { }
  float Size() const { return 2.0*3.14159; }
  Point Index(float p) const
  {
    Point pp;
    pp.x = r*cos(p);
    pp.y = r*sin(p);
    pp.z = 0.0;
    return pp;
  }
private:
  Point center;
  float r;
};

class CircleCurveXZ : public Curve<Point>
{
public:
  CircleCurveXZ(Point center, float r) : center(center), r(r) { }
  float Size() const { return 2.0*3.14159; }
  Point Index(float p) const
  {
    Point pp;
    pp.x = r*cos(p);
    pp.y = 0.0;
    pp.z = r*sin(p);
    return pp;
  }
private:
  Point center;
  float r;
};

class XYSumCurvePos : public CurvePos
{
public:
  virtual float FindPos(Point p, float curve_length) const { return  (p.x+p.y+p.z)/(300.0+300.0+300.0)*curve_length; }
};
EXPORT GameApi::CPP GameApi::CurveApi::xy_sum()
{
  return add_curve_pos(e, new XYSumCurvePos);
}

class XYSumCurvePos2 : public CurvePos
{
public:
  XYSumCurvePos2(float xmult, float ymult, float zmult) : xmult(xmult), ymult(ymult), zmult(zmult) { }
  virtual float FindPos(Point p, float curve_length) const { return  (p.x*xmult+p.y*ymult+p.z*zmult)/(300.0+300.0+300.0)*curve_length; }
private:
  float xmult,ymult,zmult;
};
EXPORT GameApi::CPP GameApi::CurveApi::xy_sum2(float xmult, float ymult, float zmult)
{
  return add_curve_pos(e, new XYSumCurvePos2(xmult,ymult,zmult));
}

EXPORT GameApi::C GameApi::CurveApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  return add_curve(e, new LineCurve(*pp1, *pp2));
}
class WaveCircleCurve : public Curve<Point>
{
public:
  WaveCircleCurve(Waveform *wave, float start_r, float end_r, float length) : wave(wave), start_r(start_r), end_r(end_r), length(length) { }
  float Size() const { return length; }
  Point Index(float p) const {
    float pos = p;
    pos/=length; // [0..1]
    pos*=wave->Length(); // [0..wave->Length()]
    float r = wave->Index(pos); // [min..max]
    //r/=wave->Max()-wave->Min();  // [min..min+1]
    //r-=wave->Min();  // [0..1]
    r*=end_r-start_r; // [0..(end_r-start_r)]
    r+=start_r; // [start_r..end_r]
    float x = r*cos(p);
    float y = r*sin(p);
    Point p2 = { x,y,0.0f };
    return p2;
  }
private:
  Waveform *wave;
  float start_r, end_r;
  float length;
};
EXPORT GameApi::C GameApi::CurveApi::circle_xy_wave(float start_r, float end_r, WV wave, float length)
{
  Waveform *wv = find_waveform(e, wave);
  return add_curve(e, new WaveCircleCurve(wv, start_r, end_r, length));
}
EXPORT GameApi::C GameApi::CurveApi::circle_xy(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXY(*c, r));
}
EXPORT GameApi::C GameApi::CurveApi::circle_xz(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXZ(*c, r));
}
EXPORT GameApi::C GameApi::CurveApi::linear(std::vector<PT> vec)
{
  if (vec.size()==0) { GameApi::C c; c.id = -1; return c; }
  if (vec.size()==1) { GameApi::PT pt = vec[0]; return line(pt,pt); }
  int s = vec.size();
  std::vector<C> vec2;
  for(int i=0;i<s-1;i++)
    {
      GameApi::C c = line(vec[i],vec[i+1]);
      vec2.push_back(c);
    }
  return compose(vec2);
}
class BezierPoints : public PointCollection
{
public:
  BezierPoints(std::vector<Point> vec) : vec(vec) { }
  int Size() const { return vec.size(); }
  Point Index(int i) const { return vec[i]; }

private:
  std::vector<Point> vec;

};
EXPORT GameApi::C GameApi::CurveApi::bezier(std::vector<PT> vec)
{
  int s = vec.size();
  std::vector<Point> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(*find_point(e,vec[i]));
    }
  BezierPoints *pt = new BezierPoints(vec2);
  return add_curve(e, new BezierCurve(*pt));
}

class MatrixCurve3 : public Curve<Point>
{
public:
  MatrixCurve3(Curve<Point> *c, Matrix m) : c(c), m(m) {}
  float Size() const { return c->Size(); }
  Point Index(float pos) const
  {
    Point p = c->Index(pos);
    Point p2 = p*m; 
    return p2;
  }
private:
  Curve<Point> *c;
  Matrix m;
};
EXPORT GameApi::C GameApi::CurveApi::scale(C curve, float mx, float my, float mz)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new MatrixCurve3(c, Matrix::Scale(mx, my, mz)));
}

EXPORT GameApi::C GameApi::CurveApi::trans(C curve, float dx, float dy, float dz)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new MatrixCurve3(c, Matrix::Translate(dx, dy, dz)));
}

class ComposeCurve2 : public Curve<Point>
{
public:
  ComposeCurve2(std::vector<Curve<Point>*> vec) : vec(vec) { }
  Point Index(float p) const
  {
    float div = fmod(p, Size());
    int pos = int(p);
    if (pos<0 || pos>=vec.size()) {
      pos = vec.size()-1;
    }
    Curve<Point> *c = vec[pos];
    div*=c->Size();
    return c->Index(div);
  }
  float Size() const {
    return float(vec.size());
  }
  
private:
  std::vector<Curve<Point>*> vec;
};

EXPORT GameApi::C GameApi::CurveApi::compose(std::vector<C> vec)
{
  int s = vec.size();
  std::vector<Curve<Point>*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_curve(e,vec[i]));
    }
  return add_curve(e, new ComposeCurve2(vec2));
}

class LengthCurve : public Curve<Point>
{
public:
  LengthCurve(Curve<Point> *c, float new_length) : c(c), new_length(new_length) { }
  float Size() const {
    return new_length;
  }
  Point Index(float p) const
  {
    p/=new_length;
    p*=c->Size();
    return c->Index(p);
  }
private:
  Curve<Point> *c;
  float new_length;
};
EXPORT GameApi::C GameApi::CurveApi::change_length(C curve, float new_length)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new LengthCurve(c, new_length));
}

class SplitCurve : public Curve<Point>
{
public:
  SplitCurve(Curve<Point> *c, float start, float end) : c(c), start(start), end(end) { }
  float Size() const { return end-start; }
  Point Index(float p) const
  {
    return c->Index(start+p);
  }
private:
  Curve<Point> *c;
  float start, end;
};

EXPORT GameApi::C GameApi::CurveApi::split(C curve, float start_var, float end_var)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new SplitCurve(c, start_var, end_var));
}

EXPORT GameApi::PT GameApi::CurveApi::pos(C curve, float p)
{
  Curve<Point> *c = find_curve(e, curve);
  Point pp = c->Index(p);
  return add_point(e, pp.x,pp.y,pp.z);
}

class SampleCurve : public PointsApiPoints
{
public:
  SampleCurve(Curve<Point> *c, int num_points) : c(c), num_points(num_points) { }
  int NumPoints() const { return num_points+1; }
  Point Pos(int i) const
  {
    float val = float(i);
    val/=float(num_points);
    val*=c->Size();
    return c->Index(val);
  }
  unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Curve<Point> *c;
  int num_points;
  float start,end;
};
EXPORT GameApi::PTS GameApi::CurveApi::sample(C curve, int num_samples)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_points_api_points(e, new SampleCurve(c,num_samples));
}
class CurveLineCollection : public LineCollection
{
public:
  CurveLineCollection(Curve<Point> *c, int num_lines) : c(c), num_lines(num_lines) { }
  int NumLines() const { return num_lines; }
  Point LinePoint(int line, int point) const
  {
    int pos = line+point;
    float pos2 = float(pos);
    pos2/=float(num_lines+1);
    pos2*=c->Size();
    return c->Index(pos2);
  }
private:
  Curve<Point> *c;
  int num_lines;
};

EXPORT GameApi::LI GameApi::CurveApi::to_lines(C curve, int num_lines)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_line_array(e, new CurveLineCollection(c, num_lines));
}

class ProductPatch : public CurvePatch
{
public:
  ProductPatch(Curve<Point> &c1, Curve<Point> &c2, Point c2_center) : c1(c1), c2(c2), c2_center(c2_center) { }
  virtual float start_x() const { return 0.0; }
  virtual float end_x() const { return c1.Size(); }
  virtual float start_y() const { return 0.0; }
  virtual float end_y() const { return c2.Size(); }
  virtual Point Map(float x, float y) const
  {
    Point p = c1.Index(x);
    Point p2 = c2.Index(y);
    Vector v2 = p2-c2_center;
    p+=v2;
    return p;
  }
private:
  Curve<Point> &c1;
  Curve<Point> &c2;
  Point c2_center;
};

EXPORT GameApi::PA GameApi::CurveApi::curve_product(C c1, C c2, PT c2_center)
{
  Point *pt = find_point(e, c2_center);
  Curve<Point> *p1 = find_curve(e, c1);
  Curve<Point> *p2 = find_curve(e, c2);
  return add_patch(e, new ProductPatch(*p1, *p2, *pt));
}
class PatchSample : public FaceCollection
{
public:
  PatchSample(CurvePatch &patch, int sx, int sy) : patch(patch), sx(sx), sy(sy) { }
  virtual void Prepare() { }
  virtual int NumFaces() const { return sx*sy; }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int y = face/sx;
    int x = face - y*sx;
    float dx = (patch.end_x() - patch.start_x())/sx;
    float dy = (patch.end_y() - patch.start_y())/sy;
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    float pos_x = patch.start_x() + xx * dx;
    float pos_y = patch.start_y() + yy * dy;
    if (point==1 ||point==2) pos_x+=dx;
    if (point==2 ||point==3) pos_y+=dy;
    return patch.Map(pos_x, pos_y);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face,0);
    Point p2 = FacePoint(face,1);
    Point p3 = FacePoint(face,2);
    return -Vector::CrossProduct(p2-p1, p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const {
    int y = face/sx;
    int x = face - y*sx;
    float dx = (1.0 - 0.0)/sx;
    float dy = (1.0 - 0.0)/sy;
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    float pos_x = xx * dx;
    float pos_y = yy * dy;
    Point2d p;
    p.x = pos_x;
    p.y = pos_y;
    return p;
    
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
private:
  CurvePatch &patch;
  int sx,sy;
};
EXPORT GameApi::P GameApi::CurveApi::patch_sample(PA patch, int sx, int sy)
{
  CurvePatch *p = find_patch(e, patch);
  return add_polygon2(e, new PatchSample(*p, sx,sy),1);
}

class Patch_X_Curve : public Curve<Point>
{
public:
  Patch_X_Curve(CurvePatch &p, float y) : p(p), y(y) { }
  float Size() const { return p.end_x() - p.start_x(); }
  Point Index(float x) const
  {
    Point pp = p.Map(p.start_x()+x,y);
    return pp;
  }
private:
  CurvePatch &p;
  float y;
};
class Patch_Y_Curve : public Curve<Point>
{
public:
  Patch_Y_Curve(CurvePatch &p, float x) : p(p), x(x) { }
  float Size() const { return p.end_y() - p.start_y(); }
  Point Index(float y) const
  {
    Point pp = p.Map(x, p.start_y()+y);
    return pp;
  }
private:
  CurvePatch &p;
  float x;
};
EXPORT GameApi::C GameApi::CurveApi::x_curve(PA patch, float y)
{
  CurvePatch *p = find_patch(e, patch);
  return add_curve(e, new Patch_X_Curve(*p, y));
}
EXPORT GameApi::C GameApi::CurveApi::y_curve(PA patch, float x)
{
  CurvePatch *p = find_patch(e, patch);
  return add_curve(e, new Patch_Y_Curve(*p, x));
}

class FromPointsMatrices : public MatrixArray
{
public:
  FromPointsMatrices(PointsApiPoints *p) : p(p) { }
  int Size() const {
    return p->NumPoints();
  }
  Matrix Index(int i) const
  {
    Point pp = p->Pos(i);
    return Matrix::Translate(pp.x, pp.y, pp.z);
  }
private:
  PointsApiPoints *p;
};

class MSRandomRot : public MatrixArray
{
public:
  MSRandomRot(Point p, int count) : p(p), count(count) { }
  int Size() const { return count; }
  Matrix Index(int i) const
  {
    Random r;
    float val = double(r.next())/r.maximum();
    val*=2.0*3.14159;
    return Matrix::RotateAroundAxis(Vector(p), val);
  }
private:
  Point p;
  int count;
};

EXPORT GameApi::MS GameApi::MatricesApi::ms_random_rot(float px, float py, float pz, int count)
{
  return add_matrix_array(e, new MSRandomRot(Point(px,py,pz), count));
}

EXPORT GameApi::MS GameApi::MatricesApi::from_points(PTS pts)
{
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_matrix_array(e, new FromPointsMatrices(points));
}

class From2dLinesMatrices : public MatrixArray
{
public:
  From2dLinesMatrices(LineCollection *coll) : coll(coll) { }
  int Size() const {
    return coll->NumLines();
  }
  Matrix Index(int i) const
  {
    Point p1 = coll->LinePoint(i, 0);
    Point p2 = coll->LinePoint(i, 1);
    LineProperties lprop(p1,p2);
    Matrix m0 = Matrix::Scale(1.0/300.0, 1.0, 1.0);
    Matrix m1 = lprop.translate_to_p1();
    Matrix m2 = lprop.rotate_z_from_p1_in_2d();
    Matrix m3 = lprop.scale_length_in_2d();
    return m0*m3*m2*m1;
  }
private:
  LineCollection *coll;
};

// requires x=0..300 3d object
EXPORT GameApi::MS GameApi::MatricesApi::from_lines_2d(LI li)
{
  LineCollection *lines = find_line_array(e,li);
  return add_matrix_array(e, new From2dLinesMatrices(lines));
}

class MultArray1 : public MatrixArray
{
public:
  MultArray1(MatrixArray *arr, Matrix m) : arr(arr), m(m) { }
  int Size() const { return arr->Size(); }
  Matrix Index(int i) const { return arr->Index(i)*m; }
private:
  MatrixArray *arr;
  Matrix m;
};

class MultArray2 : public MatrixArray
{
public:
  MultArray2(Matrix m, MatrixArray *arr) : m(m), arr(arr) { }
  int Size() const { return arr->Size(); }
  Matrix Index(int i) const { return m*arr->Index(i); }
private:
  Matrix m;
  MatrixArray *arr;
};

EXPORT GameApi::MS GameApi::MatricesApi::mult(MS m, M mat)
{
  MatrixArray *arr = find_matrix_array(e, m);
  Matrix mm = find_matrix(e, mat);
  return add_matrix_array(e, new MultArray1(arr,mm));
}
class MultArray3 : public MatrixArray
{
public:
  MultArray3(MatrixArray *arr1, MatrixArray *arr2) : arr1(arr1), arr2(arr2) { }
  int Size() const { return std::min(arr1->Size(),arr2->Size()); }
  Matrix Index(int i) const { return arr1->Index(i)*arr2->Index(i); }
private:
  MatrixArray *arr1;
  MatrixArray *arr2;
};
EXPORT GameApi::MS GameApi::MatricesApi::mult_array(MS m1, MS m2)
{
  MatrixArray *arr1 = find_matrix_array(e, m1);
  MatrixArray *arr2 = find_matrix_array(e, m2);
  return add_matrix_array(e, new MultArray3(arr1,arr2));
}
EXPORT GameApi::MS GameApi::MatricesApi::mult(M mat, MS m)
{
  Matrix mm = find_matrix(e,mat);
  MatrixArray *arr = find_matrix_array(e,m);
  return add_matrix_array(e, new MultArray2(mm,arr));
}
class SubArrayMatrices : public MatrixArray
{
public:
  SubArrayMatrices(MatrixArray *m, int start, int count) : m(m) { }
  int Size() const { return std::min(count, m->Size()-start); }
  Matrix Index(int i) const { return m->Index(start+i); }
private:
  MatrixArray *m;
  int start;
  int count;
};
EXPORT GameApi::MS GameApi::MatricesApi::subarray(MS m, int start, int count)
{
  MatrixArray *arr = find_matrix_array(e,m);
  return add_matrix_array(e, new SubArrayMatrices(arr,start,count));
}

class FromCurveMatrices : public Curve<Matrix>
{
public:
  FromCurveMatrices(Curve<Point> *c) : c(c) { }
  float Size() const { return c->Size(); }
  Matrix Index(float i) const {
    Point p = c->Index(i);
    Matrix m = Matrix::Translate(p.x,p.y,p.z);
    return m;
  }
private:
  Curve<Point> *c;
};
EXPORT GameApi::MC GameApi::MatrixCurveApi::from_curve(C curve)
{
  Curve<Point> *c = find_curve(e,curve);
  return add_matrix_curve(e, new FromCurveMatrices(c));
}
class CircleXYMatrix : public Curve<Matrix>
{
public:
  CircleXYMatrix(float radius) : r(radius) { }
  float Size() const { return 2.0*3.14159; }
  Matrix Index(float x) const
  {
    Matrix m = Matrix::Translate(r,0.0,0.0);
    Matrix m2 = Matrix::ZRotation(x);
    return m * m2;
  }
private:
  float r;
};

class CircleXZMatrix : public Curve<Matrix>
{
public:
  CircleXZMatrix(float radius) : r(radius) { }
  float Size() const { return 2.0*3.14159; }
  Matrix Index(float x) const
  {
    Matrix m = Matrix::Translate(r,0.0,0.0);
    Matrix m2 = Matrix::YRotation(x);
    return m * m2;
  }
private:
  float r;
};
EXPORT GameApi::MC GameApi::MatrixCurveApi::circle_xy(float radius)
{
  return add_matrix_curve(e, new CircleXYMatrix(radius));
}
EXPORT GameApi::MC GameApi::MatrixCurveApi::circle_xz(float radius)
{
  return add_matrix_curve(e, new CircleXZMatrix(radius));
}
class SampleMatrixCurve2 : public MatrixArray
{
public:
  SampleMatrixCurve2(Curve<Matrix> *c, int num_points) : c(c), num_points(num_points) { }
  int Size() const { return num_points; }
  Matrix Index(int i) const
  {
    float val = float(i);
    val/=float(num_points);
    val*=c->Size();
    return c->Index(val);
  }
  unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Curve<Matrix> *c;
  int num_points;
};

EXPORT GameApi::MS GameApi::MatrixCurveApi::sample(MC m_curve, int num)
{
  Curve<Matrix> *c = find_matrix_curve(e, m_curve);
  return add_matrix_array(e, new SampleMatrixCurve2(c,num));
}

class DistanceFieldMesh : public MaterialForward
{
public:
  DistanceFieldMesh(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo),next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id,flip,start_time, end_time);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }

private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

class SFO_Sandbox : public MaterialForward
{
public:
  SFO_Sandbox(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo), next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const 
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;

  }

private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

class MeshColorFromSfo : public MaterialForward
{
public:
  MeshColorFromSfo(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo),next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }

  
private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

GameApi::MT GameApi::MaterialsApi::dist_field_mesh(EveryApi &ev, SFO sfo, MT next)
{
  SFO sfo2 = ev.sh_api.v_render(sfo);
  Material *nxt = find_material(e, next);
  return add_material(e, new DistanceFieldMesh(ev,sfo2,nxt));
}
GameApi::MT GameApi::MaterialsApi::sfo_sandbox(EveryApi &ev, SFO sfo, MT next)
{
  Material *nxt = find_material(e, next);
  return add_material(e, new SFO_Sandbox(ev, sfo, nxt));
}
GameApi::MT GameApi::MaterialsApi::mesh_color_from_sfo(EveryApi &ev, SFO sfo, MT next)
{
  Material *nxt = find_material(e, next);
  return add_material(e, new MeshColorFromSfo(ev, sfo, nxt));
}

class SkeletalRoot : public SkeletalNode
{
public:
  SkeletalRoot(Point p) : p(p) { }
  Point pos(float time) const { return p; }
  Matrix mat(float time) const { return Matrix::Identity(); }
private:
  Point p;
};

GameApi::SA GameApi::Skeletal::root(PT pos)
{
  Point *pt = find_point(e, pos);
  return add_skeletal(e, new SkeletalRoot(*pt));
}
class SkeletalImpl : public SkeletalNode
{
public:
  SkeletalImpl(SkeletalNode *sk, Movement *mn, Point pt) : sk(sk), mn(mn), pt(pt) { }
  virtual Matrix mat(float time) const
  {
    // TODO: skeletal can't use delta_time properly.
    return mn->get_whole_matrix(time, 1.0);
  }
  virtual Point pos(float time) const
  {
    Point pt0 = sk->pos(time);
    Matrix m = mat(time);
    return pt0 + pt * m;
  }
private:
  SkeletalNode *sk;
  Movement *mn;
  Point pt;
}; 
GameApi::SA GameApi::Skeletal::node(SA parent, MN matrix, PT point_offset)
{
  SkeletalNode *sk = find_skeletal(e, parent);
  Movement *mn = find_move(e, matrix);
  Point *pt = find_point(e, point_offset);
  return add_skeletal(e, new SkeletalImpl(sk, mn, *pt));
}
GameApi::ML GameApi::Skeletal::skeletal_bind(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> savec)
{
  GameApi::P pp = ev.polygon_api.or_array2(vec);
  GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);

  std::vector<int> vecattribs;
  vecattribs.push_back(AttrPart);
  GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(pp2,false,std::vector<int>(), vecattribs);
  GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  
  GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
  return ml2;
}
GameApi::ML GameApi::Skeletal::skeletal_bind_material(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> savec, MT material)
{
    GameApi::P pp = ev.polygon_api.or_array2(vec);
    GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);
    Material *mat2 = find_material(e, material);
    int val = mat2->mat(pp2.id);
    GameApi::ML ml;
    ml.id = val;
    GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
    return ml2;
}

#if 0
class SA_Bind : public MainLoopItem
{
public:
  SA_Bind(std::vector<SkeletalNode*> sk, std::vector<>);
  void execute(MainLoopEnv &e)
  {
  }
};
  GameApi::ML GameApi::Skeletal::sa_bind(EveryApi &ev, std::vector<SA> pos, std::vector<P> vec)
{
  GameApi::P obj = ev.polygon_api.or_array();
}
#endif
EXPORT GameApi::PD GameApi::PolygonDistanceField::empty(EveryApi &ev)
{
  PD_Impl pd;
  pd.mesh = ev.polygon_api.empty();
  pd.distance_field = ev.sh_api.cube();
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polydistfield.push_back(pd);
  PD pd2;
  pd2.id = env->polydistfield.size()-1;
  return pd2;
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::create_pd(P mesh, SFO distance_field)
{
  PD_Impl pd;
  pd.mesh = mesh;
  pd.distance_field = distance_field;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polydistfield.push_back(pd);
  PD pd2;
  pd2.id = env->polydistfield.size()-1;
  return pd2;
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::cube(EveryApi &ev, float start_x, float end_x,
				       float start_y, float end_y,
				       float start_z, float end_z)
{
  SFO cube_sfo = ev.sh_api.cube(start_x, end_x,
				start_y, end_y,
				start_z, end_z);
  P cube_p = ev.polygon_api.cube(start_x, end_x,
				 start_y, end_y,
				 start_z, end_z);
  return create_pd(cube_p, cube_sfo);
}

EXPORT GameApi::PD GameApi::PolygonDistanceField::rounded_cube(EveryApi &ev,
					       float start_x, float end_x,
					       float start_y, float end_y,
					       float start_z, float end_z,
					       float radius)
{
  SFO c_sfo = ev.sh_api.rounded_cube(start_x, end_x,
				     start_y, end_y,
				     start_z, end_z,
				     radius);
  P c_p = ev.polygon_api.rounded_cube(ev,start_x, end_x,
				      start_y, end_y,
				      start_z, end_z,
				      radius);
  return create_pd(c_p, c_sfo);
}

EXPORT GameApi::PD GameApi::PolygonDistanceField::sphere(EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
{
  SFO sfo = ev.sh_api.sphere(center, radius);
  P p = ev.polygon_api.sphere(center, radius, numfaces1, numfaces2);
  return create_pd(p,sfo);
}
#if 0
GameApi::PD GameApi::PolygonDistanceField::cone(EveryApi &ev, int numfaces, PT p1, PT p2, float rad1, float rad2)
{
}
#endif

EXPORT GameApi::PD GameApi::PolygonDistanceField::or_array(EveryApi &ev, std::vector<PD> vec)
{
  if (vec.size()==0) { GameApi::PD p; p.id = 0; return p; }
  PD p = vec[0];
  PD_Impl pi = find_polydistfield(e, p);
  SFO pi_sfo = pi.distance_field;
  P pi_p = pi.mesh;
  std::vector<P> vec2;
  vec2.push_back(pi_p);
  int s = vec.size();
  for(int i=1;i<s;i++)
    {
      PD pp = vec[i];
      PD_Impl ppi = find_polydistfield(e,pp);
      pi_sfo = ev.sh_api.or_elem(pi_sfo, ppi.distance_field);
      vec2.push_back(ppi.mesh);
    }
  P res_p = ev.polygon_api.or_array2(vec2);
  return create_pd(res_p, pi_sfo);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::color_from_normal(EveryApi &ev, PD obj)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.color_from_normal(sfo);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::ambient_occulsion_sfo(EveryApi &ev,
						       PD obj, float d, float i)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.ambient_occulsion(sfo, d,i);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::colormod_from_position(EveryApi &ev, PD obj, float px, float py, float pz, float sx, float sy, float sz)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.colormod_from_position(sfo, px,py,pz,sx,sy,sz);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);

}
EXPORT GameApi::SFO GameApi::PolygonDistanceField::get_distance_field(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.distance_field;
}
EXPORT GameApi::P GameApi::PolygonDistanceField::get_polygon(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.mesh;
}

EXPORT GameApi::MT GameApi::PolygonDistanceField::mesh_color_from_sfo(EveryApi &ev, PD orig, MT next)
{
  PD_Impl impl = find_polydistfield(e,orig);
  return ev.materials_api.mesh_color_from_sfo(ev, impl.distance_field, next);
}
EXPORT GameApi::ML GameApi::PolygonDistanceField::render_scene(EveryApi &ev, PD pd, PD world)
{
  // TODO
  //P p = get_poly(pd);
  //SFO sfo = get_distance_field(world);
  GameApi::ML ml;
  ml.id = 0;
  return ml;
}

EXPORT std::vector<GameApi::ML> GameApi::PolygonDistanceField::render_scene_array(EveryApi &ev, std::vector<PD> vec)
{
  int s = vec.size();
  std::vector<ML> mls;
  std::vector<PD> world_vec;
  for(int i=0;i<s;i++)
    {
      PD pd = vec[i];
      world_vec.push_back(pd);
    }
  PD world = or_array(ev,world_vec);
  for(int i=0;i<s;i++)
    {
      PD pd = vec[i];
      ML ml = render_scene(ev, pd, world);
      mls.push_back(ml);
    }
  return mls;
}



#include "GameApi_gui.hh"

class ClickHide : public GuiWidgetForward
{
public:
  ClickHide(GameApi::EveryApi &ev, GuiWidget *wid) : GuiWidgetForward(ev, {wid}) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    //state=0;
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos,button, ch, type, mouse_wheel_y);
    //std::cout << "clickhide button=" << button << " " << type << std::endl;
    if (button==0 && type==1025) {
      if (vec.size() >0) {
	Point2d pos = vec[0]->get_pos();
	Vector2d sz = vec[0]->get_size();
	//std::cout << "clickhide:" << pos.x << " " << pos.y << " " << sz.dx << " " << sz.dy << std::endl;

	if (mouse_pos.x > pos.x && mouse_pos.x< pos.x+sz.dx &&
	    mouse_pos.y > pos.y && mouse_pos.y< pos.y+sz.dy)
	  {
	    vec[0]->hide();
	    //state=1;
	  }
      }
    }
    //if (state==1 && button==-1) state=0;
    Vector2d sz = vec[0]->get_size();
    size.dx = sz.dx;
    size.dy = sz.dy;

  }
private:
  //int state;
};

class ClickVisibilityWidget : public GuiWidgetForward
{
public:
  ClickVisibilityWidget(GameApi::EveryApi &ev, GuiWidget *area, GuiWidget *hidden) : GuiWidgetForward(ev, {area,hidden}), area(area), hidden(hidden) { 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    hidden->hide();
    //state = 0;
  }

  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos,button, ch, type, mouse_wheel_y);
    //std::cout << "clickvisibility button=" << button << " " << type << std::endl;
    if (button==0 && type==1025) {
      Point2d pos = area->get_pos();
      Vector2d sz = area->get_size();
      //Point2d pos2 = hidden->get_pos();
      //Vector2d sz2 = hidden->get_size();
      //std::cout << "vis: " << pos.x << " " << pos.y << " " << sz.dx << " " << sz.dy << std::endl;
      //std::cout << "hid: " << pos2.x << " " << pos2.y << " " << sz2.dx << " " << sz2.dy << std::endl;
      if (mouse_pos.x > pos.x && mouse_pos.x< pos.x+sz.dx &&
	  mouse_pos.y > pos.y && mouse_pos.y< pos.y+sz.dy)
	{
	  if (hidden->is_visible()) { hidden->hide(); } else { hidden->show(); }
	}
      //state=1;
    }
    //if (state==1 && button==-1) { state=0; }
    Vector2d sz = area->get_size();
    size.dx = sz.dx;
    size.dy = sz.dy;
  }

private:
  GuiWidget *area;
  GuiWidget *hidden;
  //int state;
};

GameApi::W GameApi::GuiApi::click_visibility(W area_widget, W hidden_widget)
{
  GuiWidget *w1 = find_widget(e, area_widget);
  GuiWidget *w2 = find_widget(e, hidden_widget);
  return add_widget(e, new ClickVisibilityWidget(ev, w1, w2));
}


GameApi::W GameApi::GuiApi::click_hide(W widget)
{
  GuiWidget *w1 = find_widget(e, widget);
  return add_widget(e, new ClickHide(ev, w1));
}


GameApi::W GameApi::GuiApi::popup_box(std::string label, std::vector<std::string> options, FtA atlas, BM atlas_bm)
{
  W txt_1 = text(label, atlas, atlas_bm);
  W txt_2 = margin(txt_1, 5,5,5,5);
  W txt_2a = center_y(txt_2, 70.0);
  //W but_1 = button(size_x(txt_2)+30.0, size_y(txt_2), c_list_item_title, c_list_item_title2);
  W but_2 = button(20.0, 20.0, c_canvas_item_node_1, c_canvas_item_node_1_2);
  W but_2a = center_align(but_2, 70.0);
  W but_2b = center_y(but_2a, 70.0);
  //W but_2a = margin(but_2,5,5,5,5);
  std::vector<W> vec;
  vec.push_back(txt_2a);
  vec.push_back(but_2b);
  W x_arr = array_x(&vec[0], vec.size(), 3);

  W mar = margin(x_arr, 2,2,2,2);
  W but_1 = button(size_x(mar), size_y(mar), c_list_item_title, c_list_item_title2);
  W lay = layer(but_1, mar);

  W menu = popup_box_menu(options, atlas, atlas_bm);
  W w6 = top_right_corner_match(lay,menu);
  W menu_hide = click_hide(w6);
  W menu_visibility = click_visibility(lay, menu_hide);
  return menu_visibility;
}

class TopRightBottomRight : public GuiWidgetForward
{
public:
  TopRightBottomRight(GameApi::EveryApi &ev, GuiWidget *wid, GuiWidget *floating) : GuiWidgetForward(ev, {floating } ),wid(wid), floating(floating) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2); 
  }
  void set_pos(Point2d pos)
  {
    //wid->set_pos(pos);
    Point2d p = wid->get_pos();
    Vector2d v1 = { wid->get_size().dx -floating->get_size().dx,wid->get_size().dy };
    p += v1;
    GuiWidgetForward::set_pos(p);
    floating->set_pos(p);
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
    floating->set_size(size);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type, mouse_wheel_y);
    Vector2d v1 = floating->get_size();
    //Vector2d v2 = vec[1]->get_size();
    size = v1;

  }
private:
  GuiWidget *wid;
  GuiWidget *floating;
};

GameApi::W GameApi::GuiApi::top_right_corner_match(W wid, W floating)
{
  GuiWidget *wid2 = find_widget(e, wid);
  GuiWidget *floating2 = find_widget(e, floating);
  return add_widget(e, new TopRightBottomRight(ev, wid2, floating2));
}

GameApi::W GameApi::GuiApi::popup_box_menu(std::vector<std::string> options, FtA atlas, BM atlas_bm)
{
  std::vector<W> vec;
  int s = options.size();
  int width = 0;
  for(int i=0;i<s;i++)
    {
      std::string s = options[i];
      W txt_1 = text(s, atlas, atlas_bm);
      if (width<size_x(txt_1)) { width = size_x(txt_1); }
      vec.push_back(txt_1);
    }
  int s2 = options.size();
  for(int i=0;i<s2;i++)
    {
      W txt_1 = vec[i];
      W txt_1a = left_align(txt_1, width);
      W txt_2 = margin(txt_1a, 5,5,5,5);
      W txt_3 = highlight(size_x(txt_2), size_y(txt_2));
      W txt_4 = layer(txt_2, txt_3);
      vec[i] = txt_4;
    }
  W w = array_y(&vec[0], vec.size(), 3);
  W w2 = button(size_x(w), size_y(w), c_list_item_title, c_list_item_title2);
  W w3 = layer(w2,w);
  W w5 = margin(w3,2,2,2,2);
  return w5;
}

template<class T>
class GenericMixed : public MixedI
{
public:
  GenericMixed(T val, std::string type) : val(val), type(type) { }
  virtual std::string Type() const { return type; }
  virtual int NumItems() const { return 1; }
  virtual std::string Name(int i) const { std::stringstream ss; ss << i; return ss.str(); }
  virtual std::string Value(int i) const { std::stringstream ss; ss << val; return ss.str(); }
  virtual std::string Print() const { return Value(0); }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new GenericMixed<T>(val,type); }
private:
  T val;
  std::string type;
};

class PairMixed : public MixedI
{
public:
  PairMixed(std::string name, MixedI &mx) : name(name), mx(mx) { }
  virtual std::string Type() const { return "pair"; }
  virtual int NumItems() const { return 1; }
  virtual std::string Name(int i) const { return name; }
  virtual std::string Value(int i) const { return mx.Print(); }
  virtual std::string Print() const
  {
    return " ( " + Name(0) + " => " + Value(0) + " ) "; 
  }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new PairMixed(name,mx); }
private:
  std::string name;
  MixedI &mx;
};

class ArrayMixed : public MixedI
{
public:
  ArrayMixed(std::vector<MixedI*> vec) : vec(vec) { }
  virtual std::string Type() const { return "array"; }
  virtual int NumItems() const { return vec.size(); }
  virtual std::string Name(int i) const 
  {
    std::stringstream ss;
    ss << i;
    return ss.str();
  }
  virtual std::string Value(int i) const {
    return vec[i]->Print();
  }
  virtual std::string Print() const
  {
    std::string res;
    res += " [ ";
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	if (i!=0) { res+=","; }
	res+=vec[i]->Print();
      }
    res+= " ] ";
    return res;
  }
  MixedI *Access(int i) const { return vec[i]; }
  MixedI *Clone() const { return new ArrayMixed(vec); }
private:
  std::vector<MixedI*> vec;
};

GameApi::MX GameApi::MixedApi::mx_float(float val)
{
  return add_mixed(e, new GenericMixed<float>(val,"float"));
}
GameApi::MX GameApi::MixedApi::mx_int(int val)
{
  return add_mixed(e, new GenericMixed<int>(val,"int"));
}
GameApi::MX GameApi::MixedApi::mx_bool(bool val)
{
  return add_mixed(e, new GenericMixed<bool>(val, "bool"));
}
GameApi::MX GameApi::MixedApi::mx_point(float x, float y, float z)
{
  Point val(x,y,z);
  return add_mixed(e, new GenericMixed<Point>(val, "point"));
}
GameApi::MX GameApi::MixedApi::mx_vector(float dx, float dy, float dz)
{
  Vector val(dx,dy,dz);
  return add_mixed(e, new GenericMixed<Vector>(val, "vector"));
}
GameApi::MX GameApi::MixedApi::mx_color(int r, int g, int b, int a)
{
  return add_mixed(e, new GenericMixed<Color>(Color(r,g,b,a), "color"));
}
GameApi::MX GameApi::MixedApi::mx_pair(std::string name, MX val)
{
  MixedI *val2 = find_mixed(e, val);
  return add_mixed(e, new PairMixed(name,*val2));
}
GameApi::MX GameApi::MixedApi::mx_string(std::string value)
{
  return add_mixed(e, new GenericMixed<std::string>(value,"string"));
}
GameApi::MX GameApi::MixedApi::mx_array(std::vector<MX> vec)
{
  std::vector<MixedI*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_mixed(e,vec[i]));
    }
  return add_mixed(e, new ArrayMixed(vec2));
}

int GameApi::MixedApi::mx_size(MX arr)
{
  MixedI *arr2 = find_mixed(e, arr);
  return arr2->NumItems();
}
class EmptyMixed : public MixedI 
{
public:
  virtual std::string Type() const { return "empty"; }
  virtual int NumItems() const { return 0; }
  virtual std::string Name(int i) const { return ""; }
  virtual std::string Value(int i) const { return ""; }
  virtual std::string Print() const { return "()"; }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new EmptyMixed; }
};
GameApi::MX GameApi::MixedApi::mx_index(MX arr, int idx)
{
  MixedI *arr2 = find_mixed(e, arr);
  MixedI *item = arr2->Access(idx);
  if (!item) { return add_mixed(e, new EmptyMixed); }
  return add_mixed(e, item);
}
class CloneMixed : public MixedI
{
public:
  CloneMixed(MixedI *val) : val(val) { }
  virtual std::string Type() const { return val->Type(); }
  virtual int NumItems() const { return val->NumItems(); }
  virtual std::string Name(int i) const { return val->Name(i); }
  virtual std::string Value(int i) const { return val->Value(i); }
  virtual std::string Print() const { return val->Print(); }
  virtual MixedI *Access(int i) const { return val->Access(i); }
  virtual MixedI *Clone() const { return val->Clone(); }
private:
  MixedI *val;
};

GameApi::MX GameApi::MixedApi::mx_find(MX arr, std::string name)
{
  MixedI *arr2 = find_mixed(e, arr);
  int s = arr2->NumItems();
  for(int i=0;i<s;i++)
    {
      MixedI *item = arr2->Access(i);
      if (item->Type()!="pair") { continue; }
      if (name==item->Name(0))
	{
	  return add_mixed(e, item);
	}
    }
  std::stringstream ss(name);
  int idx;
  if (ss >> idx) {
    if (idx>=0 && idx<s) {
      MixedI *val = arr2->Access(idx);
      return add_mixed(e,new CloneMixed(val));
    }
  }
  return add_mixed(e, new EmptyMixed);
}

float GameApi::MixedApi::mx_to_float(MX val, float def)
{
  MixedI *item = find_mixed(e, val);
  if (item->Type()=="float") {
    std::stringstream ss(item->Value(0));
    float val=def;
    ss >> val;
    return val;
  }
  return def;
}
int GameApi::MixedApi::mx_to_int(MX val, int def)
{
  MixedI *item = find_mixed(e, val);
  if (item->Type()=="int") {
    std::stringstream ss(item->Value(0));
    int val=def;
    ss >> val;
    return val;
  }
  return def;
}

std::vector<std::string> GameApi::MixedApi::mx_names(MX val)
{
  MixedI *item = find_mixed(e, val);
  std::vector<std::string> names;
  int s = item->NumItems();
  for(int i=0;i<s;i++) names.push_back(item->Name(i));
  return names;
}
std::vector<GameApi::MX> GameApi::MixedApi::mx_values(MX val)
{
  MixedI *item = find_mixed(e, val);
  std::vector<GameApi::MX> values;
  int s = item->NumItems();
  for(int i=0;i<s;i++) values.push_back(add_mixed(e,new CloneMixed(item->Access(i))));
  return values;
}

class EmptyEvent : public Event
{
public:
  void execute(MainLoopEnv &e) { }
  void handle_event(MainLoopEvent &e) { }
  bool event_triggered() const { return false; }
};
class KeyEvent : public Event
{
public:
  KeyEvent(Event *next, int key_id, int key_type) : next(next), key_id(key_id), key_type(key_type) { 
    trigger = false;
  }
  void execute(MainLoopEnv &e) { return next->execute(e); }
  void handle_event(MainLoopEvent &e)
  {
    if (e.ch == key_id && e.type == key_type)
      {
	trigger = true;
      }
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger ||next->event_triggered();
    trigger = false;
    return t;
  }
private:
  Event *next;
  int key_id;
  int key_type;
  mutable bool trigger;
};
class TimerEvent : public Event
{
public:
  TimerEvent(Event *next, float time) : next(next), time(time) { 
    trigger = false;
  }
  
  void execute(MainLoopEnv &e) { 
    if (time > e.time*10.0 && time < e.time*10.0 + e.delta_time)
      {
	trigger = true;
      }
    return next->execute(e); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger||next->event_triggered();
    trigger = false;
    return t;
  }
private:
  Event *next;
  float time;
  mutable bool trigger;
};

class DeltaTimerEvent : public Event
{
public:
  DeltaTimerEvent(Event *next, Event *ev, float delta_time) : next(next), ev(ev), delta_time(delta_time) { 
    state = 0;
    trigger = false;
  }
  void execute(MainLoopEnv &e) {
    if (ev->event_triggered())
      {
	start_time = e.time*10.0;
	state = 1;
      }
    if (state == 1 && e.time*10.0 > start_time + delta_time)
      {
	state = 0;
	trigger = true;
      }
    return next->execute(e); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger||next->event_triggered();
    trigger = false;
    return t;
  }
  
private:
  Event *next;
  Event *ev;
  float delta_time;
  float start_time;
  int state;
  mutable bool trigger;
};

class EmptyAction : public Action
{
public:
  MainLoopItem *get_main_loop(MainLoopItem *next) { return next; }
  void trigger() { }
};

class ChooseMLAction : public Action
{
public:
  ChooseMLAction(Action *next, MainLoopItem *alt) : next(next), alt(alt) { 
    triggered = false;
  }
  MainLoopItem *get_main_loop(MainLoopItem *nxt)
  {
    if (triggered) { return next->get_main_loop(alt); }
    return next->get_main_loop(nxt);
  }
  void trigger() {
    next->trigger();
    triggered = true;
  }
private:
  Action *next;
  MainLoopItem *alt;
  bool triggered;
};

class MoveMLAction : public Action, private MainLoopItem
{
public:
  MoveMLAction(Action *next, Movement *mov, float start_time, float end_time) : next(next), mov(mov), start_time(start_time), end_time(end_time) { 
    next_item = 0;
    current_time = 0.0;
  }
  virtual MainLoopItem *get_main_loop(MainLoopItem *next)
  {
    next_item = next;
    return this;
  }
  virtual void trigger()=0;
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    current_time = e.time*10.0;
    if (next_item) { next_item->execute(e); }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (next_item) { next_item->handle_event(e); }
  }
  virtual int shader_id() { 
    if (next_item) { return next_item->shader_id(); }
    return -1;
  }
private:
  Action *next;
  Movement *mov;
  float start_time;
  float end_time;
  MainLoopItem *next_item;
  float current_time;
};



struct Envi_2 {
  GameApi::EveryApi *ev;
  GameApi::MN move;
  GameApi::ML mainloop;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  GameApi::InteractionApi::Quake_data data;
  bool logo_shown = true;
  bool fpscounter = false;
  float start_time = 0.0f;
  GameApi::SH color_sh;
  GameApi::SH texture_sh;
  GameApi::SH arr_texture_sh;
  bool exit = false;
  float timeout = 100000.0f;
  int screen_width=800;
  int screen_height=600;
  bool firsttime = true;
};
bool async_is_done=false;
extern std::string gameapi_seamless_url;
void blocker_iter(void *arg)
{
  Envi_2 *env = (Envi_2*)arg;
  //std::cout << "async: " << async_pending_count << std::endl;
  if (async_pending_count > 0 && !async_is_done) { env->logo_shown = true; }
  if (env->logo_shown)
    {
      bool b=false;
      if (gameapi_seamless_url=="")
	b = env->ev->mainloop_api.logo_iter();
      else
	b = env->ev->mainloop_api.seamless_iter();
      if (b && async_pending_count==0) { env->logo_shown = false;
	env->ev->mainloop_api.reset_time();
	env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
      }
      return;
    }
  async_is_done = true;

    if (env->firsttime) {
      MainLoopItem *item = find_main_loop(env->ev->get_env(),env->mainloop);
      item->Prepare();
      env->firsttime = false;
    }


    env->ev->mainloop_api.clear_3d(0xff000000);

    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { std::cout << "ESC pressed!" << std::endl; env->exit = true; }
#endif

	GameApi::InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e);

      }
    GameApi::InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);

    GameApi::M mat = env->ev->matrix_api.identity();
	env->ev->shader_api.use(env->color_sh);
	env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));

	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->color_sh);

	GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
	GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
	GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

	if (env->fpscounter)
	  env->ev->mainloop_api.fpscounter();
	if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
	  {
	    //std::cout << "Timeout, existing..." << std::endl;
	    //env->exit = true;
	  }

    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    g_low->ogl->glGetError();
}
extern int async_pending_count;
int async_pending_count_previous=-1;

int no_draw_count=0;

extern int score;
extern int hidden_score;
extern std::vector<int> g_hide_container;


class MainLoopSplitter_win32_and_emscripten : public Splitter
{
public:
  MainLoopSplitter_win32_and_emscripten(GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
    firsttime = true;
  }
  virtual void set_env(GameApi::Env *ei)
  {
    e = ei;
  }
  virtual void set_everyapi(GameApi::EveryApi *evi)
  {
    ev = evi;
  }
  virtual void Init()
  {
    score = 0;
    hidden_score = 0;

    Envi_2 &env = envi;
    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;
    env.ev = ev;
    
    GameApi::SH sh = env.ev->shader_api.colour_shader();
    GameApi::SH sh2 = env.ev->shader_api.texture_shader();
    GameApi::SH sh3 = env.ev->shader_api.texture_array_shader();
    
    // rest of the initializations
    env.ev->mainloop_api.init_3d(sh);
    env.ev->mainloop_api.init_3d(sh2);
    env.ev->mainloop_api.init_3d(sh3);
    env.ev->shader_api.use(sh);
    
    GameApi::ML ml = mainloop(*env.ev);
    if (async_pending_count > 0) { env.logo_shown = true; }
    
    env.mainloop = ml;
    
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    env.ev->mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url == "") {
	env.ev->mainloop_api.display_logo(*env.ev);
      } else {
	env.ev->mainloop_api.display_seamless(*env.ev);
      }
    } else {
	env.ev->mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     env.ev->mainloop_api.alpha(true);
     g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
     GameApi::MainLoopApi::Event e;
     while((e = env.ev->mainloop_api.get_event()).last==true)
       {
	 /* this eats all events from queue */
       }
     g_hide_container.clear();

  }
  virtual int Iter()
  {
    Envi_2 *env = (Envi_2*)&envi;
    //std::cout << "async: " << async_pending_count << std::endl;
    if ((async_pending_count > 0 && !async_is_done)||no_draw_count>0) { env->logo_shown = true; }
    if (async_pending_count != async_pending_count_previous)
      {
	std::cout << "ASync pending count=" << async_pending_count << std::endl;
	async_pending_count_previous = async_pending_count;
      }
    if (env->logo_shown)
      {
	bool b = false;
	if (gameapi_seamless_url=="") {
	  //std::cout << "Logo iter" << std::endl;
	  b = env->ev->mainloop_api.logo_iter();
	  //std::cout << "End of Logo iter" << std::endl;
	} else {
	  b = env->ev->mainloop_api.seamless_iter();
	}
	if (b && async_pending_count==0 && no_draw_count==0) { env->logo_shown = false;
	  env->ev->mainloop_api.reset_time();
	  env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
	}
	if (async_pending_count==0 && no_draw_count>0) { /* pass through */}
	else
	  return -1;
      }
    async_is_done = true;

    if (firsttime) {
      MainLoopItem *item = find_main_loop(env->ev->get_env(),code);
      //std::cout << "Splitter/Prepare:" << std::endl;
      item->Prepare();
      //std::cout << "Splitter/End of Prepare:" << std::endl;
      firsttime = false;
    }
    if (no_draw_count==0)
      env->ev->mainloop_api.clear_3d(0xff000000);
    
    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { std::cout << "Esc pressed2!" << std::endl; env->exit = true; return 0; }
#endif
	
	//GameApi::InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
	//					      env->data, env->speed_x, env->speed_y,
	//			   1.0, 1.0*3.14159*2.0/360.0);
	//std::cout << "Splitter/Event:" << std::endl;
	env->ev->mainloop_api.event_ml(env->mainloop, e);
	//std::cout << "Splitter/End of Event:" << std::endl;
	
      }
    //GameApi::InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
    //						  env->data, env->speed_x, env->speed_y,
    //						  1.0, 1.0*3.14159*2.0/360.0);
    
    GameApi::M mat = env->ev->matrix_api.identity();
    if (screen_width<600) {
      mat = env->ev->matrix_api.scale(-1.0,-1.0,1.0);
    }
    env->ev->shader_api.use(env->color_sh);
    env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->texture_sh);
    env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    
    env->ev->shader_api.use(env->arr_texture_sh);
    env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->color_sh);
    
    GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
    GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
    GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
    
    //std::cout << "Splitter/execute_ml" << std::endl;
    env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);
    //std::cout << "Splitter/end of execute_ml" << std::endl;

    if (env->fpscounter)
      env->ev->mainloop_api.fpscounter();
    if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
      {
	//env->exit = true;
	std::cout << "Timeout2, exiting.." << std::endl;
      }
    
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    g_low->ogl->glGetError();
    return -1;
  }
  virtual void Destroy()
  {
    // this is needed for win32 build in editor
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
  }
  virtual Splitter* NextState(int code)
  {
    return 0;
  }
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }

private:
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
  Envi_2 envi;
  bool firsttime;
};

extern int score;
extern int hidden_score;

class MainLoopBlocker_win32_and_emscripten : public Blocker
{
public:
  MainLoopBlocker_win32_and_emscripten(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : e(e), ev(ev), code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
    score = 0;
    hidden_score=0;
  }
  void SetTimeout(float duration) {
  }
  void Execute()
  {
    
    Envi_2 env;

    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;

    GameApi::SH sh = ev.shader_api.colour_shader();
    GameApi::SH sh2 = ev.shader_api.texture_shader();
    GameApi::SH sh3 = ev.shader_api.texture_array_shader();
    
    // rest of the initializations
    ev.mainloop_api.init_3d(sh);
    ev.mainloop_api.init_3d(sh2);
    ev.mainloop_api.init_3d(sh3);
    ev.shader_api.use(sh);
    
    GameApi::ML ml = mainloop(ev);
    if (async_pending_count > 0) { env.logo_shown = true; }

    //GameApi::MN mn0 = ev.move_api.empty();
    //GameApi::MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, -400.0);
    //GameApi::ML ml2 = ev.move_api.move_ml(ev, ml, mn);
    env.mainloop = ml;
    
    env.ev = &ev;
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    ev.mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url=="") {
	ev.mainloop_api.display_logo(ev);
      } else {
	ev.mainloop_api.display_seamless(ev);
      }
    } 
    else 
      {
	ev.mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     ev.mainloop_api.alpha(true);
     g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
    GameApi::MainLoopApi::Event e;
    while((e = env.ev->mainloop_api.get_event()).last==true)
      {
	/* this eats all events from queue */
      }

#ifndef EMSCRIPTEN
    while(!env.exit) {
      blocker_iter(&env);
      //ev.mainloop_api.delay(10);
    }
#else
      emscripten_set_main_loop_arg(blocker_iter, (void*)&env, 0,1);
#endif
      std::cout << "Mainloop existing.." << std::endl;
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
  }
  private:
  
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
};

EXPORT GameApi::BLK GameApi::BlockerApi::game_window(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{
  ml = ev.mainloop_api.display_background(ev,ml);

  Blocker *blk = new MainLoopBlocker_win32_and_emscripten(e,ev,ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_blocker(e, blk);
}
EXPORT GameApi::RUN GameApi::BlockerApi::game_window2(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{
  ml = ev.mainloop_api.display_background(ev,ml);
  Splitter *spl = new MainLoopSplitter_win32_and_emscripten(ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_splitter(e, spl);
}

class SplitterSeq : public Splitter
{
public:
  SplitterSeq(std::vector<Splitter*> vec) : vec(vec) { current_seq=0; }
  virtual void Init() {
    vec[current_seq]->e = e;
    vec[current_seq]->ev = ev;
    vec[current_seq]->Init();
  }
  virtual int Iter() { 
    int exit = vec[current_seq]->Iter(); return exit; 
  }
  virtual void Destroy() { vec[current_seq]->Destroy(); }
  virtual Splitter* NextState(int code) {
    current_seq++;
    int s = vec.size();
    if (current_seq>=s) { return 0; }
    return vec[current_seq];
  }
private:
  std::vector<Splitter*> vec;
  int current_seq;
};

EXPORT GameApi::RUN GameApi::BlockerApi::run_seq(GameApi::EveryApi &ev, std::vector<RUN> vec)
{
  std::vector<Splitter*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      Splitter *spl = find_splitter(e, vec[i]);
      vec2.push_back(spl);
    }
  return add_splitter(e, new SplitterSeq(vec2));
}

class BlockerSeq : public Blocker
{
public:
  BlockerSeq(GameApi::EveryApi &ev, std::vector<Blocker*> vec) : ev(ev), vec(vec) { }
  void SetTimeout(float duration) {
  }
  void Execute() 
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Blocker *blk = vec[i];
	blk->Execute();
      }
  }
private:
  GameApi::EveryApi &ev;
  std::vector<Blocker*> vec;
};
EXPORT GameApi::BLK GameApi::BlockerApi::game_seq(GameApi::EveryApi &ev,std::vector<BLK> vec)
{
  std::vector<Blocker*> blks;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      Blocker *blk = find_blocker(e,vec[i]);
      blks.push_back(blk);
    }
  return add_blocker(e, new BlockerSeq(ev,blks));
}
EXPORT void GameApi::BlockerApi::run(BLK blk)
{
  Blocker *blk2 = find_blocker(e, blk);
  blk2->Execute();
}


Splitter *splitter_current = 0;
void splitter_iter2(void *arg)
{
  if (!arg) { std::cout << "FAIL: Splitter_iter2 NULL" << std::endl; return; }
  Splitter *blk2 = (Splitter*)arg;
  int blocker_exit_code = blk2->Iter();
  if (blocker_exit_code!=-1) 
    {
      Splitter *next = blk2->NextState(blocker_exit_code);
      if (next) {
	blk2->EnvTransfer(next);
      }
      blk2->Destroy();
      if (next) {
	next->Init();
      }
#ifdef EMSCRIPTEN
      // TODO, VR ISSUES
      emscripten_set_main_loop_arg(splitter_iter2, (void*)next, 0,1);
#else
      splitter_current = next;
#endif
    }
}

#ifdef VIRTUAL_REALITY
void vr_run2(Splitter *spl2);
#endif
EXPORT void GameApi::BlockerApi::run2(EveryApi &ev, RUN spl)
{
  Splitter *spl2 = find_splitter(e, spl);
  spl2->e = &e;
  spl2->ev = &ev;
  spl2->Init();
  splitter_current = spl2;
#ifdef EMSCRIPTEN
#ifdef VIRTUAL_REALITY
  vr_run2(spl2);
#else
  emscripten_set_main_loop_arg(splitter_iter2, (void*)spl2, 0,1);
#endif
#else
  while(splitter_current) {
    splitter_iter2((void*)splitter_current);
  }
#endif
}

class KeyFrameMesh : public VertexAnimNode
{
public:
  KeyFrameMesh(int coll) : coll(coll) { }
  int NumKeyFrames() { return 2; }
  virtual float StepDuration(int keyframe_gap) const { return 0.001; }
  int FaceColl(int keyframe_gap) const
  {
    return coll;
  }
  int Lines(int keyframe_gap) const
  {
    return -1;
  }
  int Points(int keyframe_gap) const
  {
    return -1;
  }
  Point KeyFramePoint(Point p, int keyframe_gap, int point_num) const
  {
    return p;
  }
private:
  int coll;
};

class KeyFrameLines : public VertexAnimNode
{
public:
  KeyFrameLines(int coll) : coll(coll) { }
  int NumKeyFrames() { return 2; }
  virtual float StepDuration(int keyframe_gap) const { return 0.001; }
  int FaceColl(int keyframe_gap) const
  {
    return -1;
  }
  int Lines(int keyframe_gap) const
  {
    return coll;
  }
  int Points(int keyframe_gap) const
  {
    return -1;
  }
  Point KeyFramePoint(Point p, int keyframe_gap, int point_num) const
  {
    return p;
  }
private:
  int coll;
};

EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_mesh(P part)
{
  return add_vertex_anim(e, new KeyFrameMesh(part.id));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_lines(LI part)
{
  return add_vertex_anim(e, new KeyFrameLines(part.id));
}
class ChangePosLI : public LineCollection
{
public:
  ChangePosLI(LineCollection &coll, LineCollection &orig, PointTransform &trans, float delta_time, bool dif) : coll(coll), orig(orig), trans(trans), delta_time(delta_time),dif(dif) {}

  virtual int NumLines() const { return coll.NumLines(); }
  virtual Point LinePoint(int line, int point) const
  {
    Point p;
    if (!dif) {
      p = coll.EndLinePoint(line,point);
    } else {
      p = orig.LinePoint(line,point);
      return trans.Map(p, 0.0);
    }
    return p;

  }
  virtual Point EndLinePoint(int line, int point) const {
    Point p;
    if (!dif) {
      p = coll.EndLinePoint(line,point);
    } else {
      p = orig.LinePoint(line,point);
    }
    return trans.Map(p, delta_time);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

  
private:
  LineCollection &coll;
  LineCollection &orig;
  PointTransform &trans;
  float delta_time;
  bool dif;
};

class ChangePos : public ForwardFaceCollection
{
public:
  ChangePos(FaceCollection &coll, FaceCollection &orig, PointTransform &trans, float delta_time, bool dif) : ForwardFaceCollection(coll), orig(orig), trans(trans), delta_time(delta_time), dif(dif) { }
  virtual Point FacePoint(int face, int point) const
  {
    // this FacePoint/EndFacePoint logic looks odd, but is needed
    // because of chaining of facecollections.
    Point p;
    if (!dif) {
      p = ForwardFaceCollection::EndFacePoint(face,point);
    } else {
      p = orig.FacePoint(face,point);
      return trans.Map(p, 0.0);
    }
    return p;
  }
  virtual Point EndFacePoint(int face, int point) const
  {
    Point p;
    if (!dif) {
      p = ForwardFaceCollection::EndFacePoint(face,point);
    } else {
      p = orig.FacePoint(face,point);
    }
    return trans.Map(p, delta_time);
  }
private:
  FaceCollection &orig;
  PointTransform &trans;
  float delta_time;
  bool dif;
};

EXPORT GameApi::LI GameApi::VertexAnimApi::change_pos_li(LI p, LI orig, PTT transform, float delta_time, bool different_pos)
{
  LineCollection *li = find_line_array(e, p);
  LineCollection *orig2 = find_line_array(e, orig);
  PointTransform *trans = find_point_transform(e, transform);
  return add_line_array(e, new ChangePosLI(*li, *orig2, *trans, delta_time, different_pos));
}

EXPORT GameApi::P GameApi::VertexAnimApi::change_pos(P p, P orig, PTT transform, float delta_time, bool different_pos)
{
  FaceCollection *coll = find_facecoll(e, p);
  FaceCollection *orig2 = find_facecoll(e, orig);
  PointTransform *trans = find_point_transform(e, transform);
  return add_polygon2(e, new ChangePos(*coll, *orig2, *trans, delta_time, different_pos),1);
}

class CurveAccessor : public PointTransform
{
public:
  CurveAccessor(float start_time, Curve<Point> &curve, CurvePos &pos, float time_multiplier) : start_time(start_time), curve(curve), pos(pos), time_mult(time_multiplier) { }
  Point Map(Point p, float delta_time) const {
    float val = pos.FindPos(p, curve.Size());
    Point pp = p+curve.Index(start_time*time_mult+val+time_mult*(delta_time));
    return pp;
  }
private:
  float start_time;
  Curve<Point> &curve;
  CurvePos &pos;
  float time_mult;
};
class RotAccessor : public PointTransform
{
public:
  RotAccessor(float start_time, float time_mult, float nx, float ny, float nz, float dist_angle) : start_time(start_time), time_mult(time_mult), nx(nx), ny(ny), nz(nz), dist_angle(dist_angle) { }
  Point Map(Point p, float delta_time) const {
    float progress = start_time*time_mult + time_mult*delta_time;
    Matrix m = Matrix::RotateAroundAxis(Vector(nx,ny,nz), progress);
    return p*m;
  }
private:
  float start_time;
  float time_mult;
  float nx,ny,nz;
  float dist_angle;
};
EXPORT GameApi::PTT GameApi::VertexAnimApi::curve_accessor(C curve, CPP pos, float start_time, float time_mult)
{
  Curve<Point> *cur = find_curve(e, curve);
  CurvePos *cur_pos = find_curve_pos(e, pos);
  return add_point_transform(e, new CurveAccessor(start_time, *cur, *cur_pos, time_mult));
}


EXPORT GameApi::PTT GameApi::VertexAnimApi::rot_accessor(float start_time, float time_mult, float nx, float ny, float nz, float dist_angle)
{
  return add_point_transform(e, new RotAccessor(start_time, time_mult, nx, ny,nz, dist_angle));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::curve_trans(EveryApi &ev, KF kf, C curve, CPP pos, int numsamples, float duration)
{
  Curve<Point> *cur = find_curve(e, curve);
  float curve_length = cur->Size();
  int s = numsamples;
  //float delta_pos = curve_length/numsamples;
  float delta_time = duration/numsamples;
  for(int i=0;i<s;i++)
    {
      PTT pt = curve_accessor(curve, pos, i*delta_time, curve_length/duration);
      kf = keyframe_bind2(ev, kf, pt, delta_time, true);
    }
  return kf;
}
EXPORT GameApi::KF GameApi::VertexAnimApi::sample_rot(EveryApi &ev, KF kf, float nx, float ny, float nz, float angle, int numsamples, float duration)
{
  //Curve<Point> *cur = find_curve(e, curve);
  float curve_length = angle; //cur->Size();
  int s = numsamples;
  //float delta_pos = curve_length/numsamples;
  float delta_time = duration/numsamples;
  for(int i=0;i<s;i++)
    {
      //PTT empty = empty_trans();
      //PTT pt = rotate_trans2(empty, delta_time, nx,ny,nz, angle/numsamples);
      PTT pt = rot_accessor(i*delta_time, curve_length/duration, nx,ny,nz, angle);
      kf = keyframe_bind2(ev, kf, pt, delta_time, true);
    }
  return kf;
}

// change pos needs to be different.
class KeyFrameBind : public VertexAnimNode
{
public:
  KeyFrameBind(GameApi::EveryApi &ev, VertexAnimNode *va, GameApi::PTT pt, float delta_time, bool dif) : ev(ev), va(va), pt(pt), delta_time(delta_time), dif(dif) { temp = 0; }
  int NumKeyFrames() { return va->NumKeyFrames() + 1; }
  virtual float StepDuration(int keyframe_gap) const { 
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->StepDuration(keyframe_gap); }
    return delta_time;
  }
  int FaceColl(int keyframe_gap) const
  {
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->FaceColl(keyframe_gap); }
    int coll = va->FaceColl(prev_count-1);
    int orig = va->FaceColl(0);
    GameApi::P p = { coll };
    GameApi::P orig2 = { orig };
    if (coll==-1 || orig==-1) {
      return -1;
    }
    GameApi::P p2 = ev.vertex_anim_api.change_pos(p, orig2, pt, delta_time, dif);
    return p2.id;
  }
  int Lines(int keyframe_gap) const
  {
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->Lines(keyframe_gap); }
    int lines = va->Lines(prev_count-1);
    int orig = va->Lines(0);
    GameApi::LI p = { lines };
    GameApi::LI orig2 = { orig };
    if (lines==-1||orig==-1) {
      return -1;
    }
    GameApi::LI p2 = ev.vertex_anim_api.change_pos_li(p, orig2, pt, delta_time,dif);
    return p2.id;
  }
private:
  GameApi::EveryApi &ev;
  VertexAnimNode *va;
  GameApi::PTT pt;
  float delta_time;
  mutable FaceCollection *temp;
  bool dif;
};


EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_bind(EveryApi &ev, KF keyframe, PTT transform, float delta_time)
{
  VertexAnimNode *va = find_vertex_anim(e, keyframe);
  return add_vertex_anim(e, new KeyFrameBind(ev,va,transform,delta_time, false));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_bind2(EveryApi &ev, KF keyframe, PTT transform, float delta_time, bool dif)
{
  VertexAnimNode *va = find_vertex_anim(e, keyframe);
  return add_vertex_anim(e, new KeyFrameBind(ev,va,transform,delta_time, dif));
}

class EmptyTransform : public PointTransform
{
public:
  EmptyTransform() { }
  Point Map(Point p, float delta_time) const { return p; }
};
class TransformSpeed : public PointTransform
{
public:
  TransformSpeed(PointTransform *prev) : prev(prev) { }
  virtual Matrix Mat() const=0;
  Point Map(Point p, float delta_time) const
  {
    Matrix m = Mat();
    delta_time /= 10.0;
    m*=delta_time;
    return prev->Map(p,delta_time)*m;
  }
private:
  PointTransform *prev;
};
class TransformDist : public PointTransform
{
public:
  TransformDist(PointTransform *prev, float duration) : prev(prev), duration(duration) { 
    if (duration==0.0f) duration=10.0f;
  }
  virtual Matrix Mat() const=0;
  Point Map(Point p, float delta_time) const
  {
    Matrix m = Mat();
    m*=1.0/duration;
    m*=delta_time;
    return prev->Map(p,delta_time)*m;
  }
private:
  PointTransform *prev;
  float duration;
};

GameApi::PTT GameApi::VertexAnimApi::empty_trans()
{
  return add_point_transform(e, new EmptyTransform);
}

class TranslateSpeed : public TransformSpeed
{
public:
  TranslateSpeed(PointTransform *prev, float speed_x, float speed_y, float speed_z) : TransformSpeed(prev), speed_x(speed_x), speed_y(speed_y), speed_z(speed_z) { }
  virtual Matrix Mat() const { return Matrix::Translate(speed_x, speed_y, speed_z); }
private:
  float speed_x, speed_y, speed_z;
};
class TranslateDist : public TransformDist
{
public:
  TranslateDist(PointTransform *prev, float duration, float dist_x, float dist_y, float dist_z) : TransformDist(prev, duration), dist_x(dist_x), dist_y(dist_y), dist_z(dist_z) { }
  virtual Matrix Mat() const { return Matrix::Translate(dist_x, dist_y, dist_z); }
private:
  float dist_x, dist_y, dist_z;
};
class RotateSpeed : public TransformSpeed
{
public:
  RotateSpeed(PointTransform *prev, float nx, float ny, float nz, float angle) : TransformSpeed(prev), nx(nx), ny(ny), nz(nz), angle(angle) { }
  virtual Matrix Mat() const { return Matrix::RotateAroundAxis(Vector(nx,ny,nz),angle); }
private:
  float nx,ny,nz;
  float angle;
};
class RotateDist : public TransformDist
{
public:
  RotateDist(PointTransform *prev, float duration, float nx, float ny, float nz, float angle) : TransformDist(prev, duration), nx(nx), ny(ny), nz(nz) { }
  virtual Matrix Mat() const { return Matrix::RotateAroundAxis(Vector(nx,ny,nz), angle); }
private:
  float nx,ny,nz;
  float angle;
};


class ScaleSpeed : public TransformSpeed
{
public:
  ScaleSpeed(PointTransform *prev, float sx, float sy, float sz) : TransformSpeed(prev), sx(sx), sy(sy), sz(sz) { }
  virtual Matrix Mat() const { return Matrix::Scale(sx,sy,sz); }
private:
  float sx,sy,sz;
};
class ScaleDist : public TransformDist
{
public:
  ScaleDist(PointTransform *prev, float duration, float sx, float sy, float sz) : TransformDist(prev, duration), sx(sx), sy(sy), sz(sz) { }
  virtual Matrix Mat() const { return Matrix::Scale(sx,sy,sz); }
private:
  float sx,sy,sz;
};


EXPORT GameApi::PTT GameApi::VertexAnimApi::translate_trans(PTT prev, float speed_x, float speed_y, float speed_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new TranslateSpeed(prev2, speed_x, speed_y, speed_z));
}
EXPORT GameApi::PTT GameApi::VertexAnimApi::translate_trans2(PTT prev, float duration, float dist_x, float dist_y, float dist_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new TranslateDist(prev2, duration, dist_x, dist_y, dist_z));

}

EXPORT GameApi::PTT GameApi::VertexAnimApi::rotate_trans(PTT prev, float nx, float ny, float nz, float speed_angle)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new RotateSpeed(prev2, nx, ny, nz, speed_angle));
}
EXPORT GameApi::PTT GameApi::VertexAnimApi::rotate_trans2(PTT prev, float duration, float nx, float ny, float nz, float dist_angle)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new RotateDist(prev2, duration, nx, ny, nz, dist_angle));

}
EXPORT GameApi::PTT GameApi::VertexAnimApi::scale_trans(PTT prev, float scale_speed_x, float scale_speed_y, float scale_speed_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new ScaleSpeed(prev2, scale_speed_x, scale_speed_y, scale_speed_z));

}

EXPORT GameApi::PTT GameApi::VertexAnimApi::scale_trans2(PTT prev, float duration, float scale_dist_x, float scale_dist_y, float scale_dist_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new ScaleDist(prev2, duration, scale_dist_x, scale_dist_y, scale_dist_z));

}

GameApi::P linear_func(float val, GameApi::P c_start, GameApi::P c_end, float time_start, float time_end)
{
  float time_avg = (time_start + time_end)/2.0f;
  if (val < time_avg) { return c_start; }
  return c_end;
}

class AnimFaceCollection : public ForwardFaceCollection
{
public:
  AnimFaceCollection(FaceCollection *coll1, FaceCollection *coll2) : ForwardFaceCollection(*coll1), coll2(coll2) { }
  
  virtual Point EndFacePoint(int face, int point) const { return coll2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return coll2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return coll2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return coll2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return coll2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return coll2->TexCoord(face,point); }
  virtual float EndTexCoord3(int face, int point) const { return TexCoord3(face,point); }

private:
  FaceCollection *coll2;
};

class VertexAnimMainLoop : public MainLoopItem
{
public:
  VertexAnimMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, VertexAnimNode *va) : env(env), ev(ev), va(va) 
  {
    Prepare_1();
    Prepare_2();
    shader.id=-1;
    firsttime = true;
  }
  void Prepare_1() {
    std::vector<GameApi::P> vec;
    int s = va->NumKeyFrames();
    for(int i=0;i<s;i++)
      {
        int coll = va->FaceColl(i);
	GameApi::P p = { coll };
	vec.push_back(p);
      }
    ranges = vec;

    std::vector<GameApi::LI> vec2;
    int s2 = va->NumKeyFrames();
    for(int i=0;i<s2;i++)
      {
	int coll = va->Lines(i);
	GameApi::LI li = { coll };
	vec2.push_back(li);
      }
    li_ranges = vec2;

  }
  void Prepare_2()
  {
    std::vector<GameApi::VA> vas;
    int s = ranges.size();
    for(int i=0;i<s;i++)
      {
	GameApi::P p1 = ranges[i];
	if (p1.id==-1) {
	  GameApi::VA va;
	  va.id = -1;
	  vas.push_back(va);
	} else {
	  GameApi::VA va = ev.polygon_api.create_vertex_array(p1);
	  vas.push_back(va);
	}
      }
    ranges2 = vas;

    std::vector<GameApi::LLA> vas2;
    int s2 = li_ranges.size();
    for(int i=0;i<s2;i++)
      {
	GameApi::LI li = li_ranges[i];
	if (li.id==-1) {
	  GameApi::LLA lla;
	  lla.id = -1;
	  vas2.push_back(lla);
	} else {
	  GameApi::LLA lla = ev.lines_api.prepare(li);
	  vas2.push_back(lla);
	}
      }
    li_ranges2 = vas2;

  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Prepare() { }
  void execute(MainLoopEnv &e) 
  {
    if (ranges2.size()<1) { return; }
    GameApi::SH sh = { e.sh_color };
    if (ranges2[0].id!=-1) {
    if (ev.polygon_api.is_texture(ranges2[0]))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(ranges2[0]))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }
    }

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ranges2[0].id!=-1) {
    if (ev.polygon_api.is_texture(ranges2[0]))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(ranges2[0]))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
    }
#endif
    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	//ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);


    //std::cout << e.time << " " << start_time << std::endl;
    float time = e.time*10.0;
    int s = va->NumKeyFrames();
    float currenttime = 0.0f;
    int choose = -1;
    float step_size = 1.0f;
    for(int i=0;i<s;i++)
      {
	float val = va->StepDuration(i);
	if (currenttime + val > time && currenttime < time)
	  {
	    step_size = val;
	    choose=i;
	    break;
	  }
	currenttime += val;
      }
    bool end = false;
    if (choose==-1)
      {
	choose = ranges2.size()-1;
	end = true;
      }
    if (choose!=-1) {
      //std::cout << "choose: " << choose << ": " << (time-currenttime)/step_size << std::endl;
      ev.shader_api.use(sh);
      if (!end)
	ev.shader_api.set_var(sh, "in_POS", (time - currenttime)/step_size); 
      else
	ev.shader_api.set_var(sh, "in_POS", 1.0f);
      if (ranges2[choose].id != -1)
	ev.polygon_api.render_vertex_array(ranges2[choose]);
      if (li_ranges2[choose].id != -1)
	ev.lines_api.render(li_ranges2[choose]);
    }
    ev.shader_api.unuse(sh);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  VertexAnimNode *va;
  std::vector<GameApi::P> ranges;
  std::vector<GameApi::VA> ranges2;
  std::vector<GameApi::LI> li_ranges;
  std::vector<GameApi::LLA> li_ranges2;
  bool firsttime;
  //GameApi::SH sh;
  GameApi::SH shader;

  float start_time;
};

EXPORT GameApi::ML GameApi::VertexAnimApi::vertex_anim_render(EveryApi &ev, KF kf)
{
  VertexAnimNode *va = find_vertex_anim(e, kf);
  return add_main_loop(e, new VertexAnimMainLoop(e,ev, va));
  
}
class RepeatVertexAnimNode : public VertexAnimNode
{
public:
  RepeatVertexAnimNode(VertexAnimNode *next, int repeat_count) : next(next), repeat_count(repeat_count) { }
  virtual int NumKeyFrames() { return next->NumKeyFrames()*repeat_count; } // n
  virtual float StepDuration(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->StepDuration(keyframe_gap);
  }
  virtual int FaceColl(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->FaceColl(keyframe_gap);
  }
  virtual int Lines(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->Lines(keyframe_gap);
  }
private:
  VertexAnimNode *next;
  int repeat_count;
};
EXPORT GameApi::KF GameApi::VertexAnimApi::repeat_keyframes(KF rep, int count)
{
  VertexAnimNode *va = find_vertex_anim(e, rep);
  return add_vertex_anim(e, new RepeatVertexAnimNode(va,count));
}




class StringDisplayToBitmap : public Bitmap<int>
{
public:
  StringDisplayToBitmap(StringDisplay &sd, int def) : sd(sd),def(def) {}
  void Prepare() { sd.Prepare(); }
  int SizeX() const
  {
    int s = sd.Count();
    int maxx = 0;
    for(int i=0;i<s;i++)
      {
	int x = sd.X(i) + sd.SX(i);
	if (x>maxx) maxx = x;
      }
    return maxx;
  }
  int SizeY() const
  {
    int s = sd.Count();
    int maxy = 0;
    for(int i=0;i<s;i++)
      {
	int y = sd.Y(i) + sd.SY(i);
	if (y>maxy) maxy = y;
      }
    return maxy;

  }
  int Map(int x, int y) const
  {
    int s = sd.Count();
    //int maxy = 0;
    for(int i=0;i<s;i++)
      {
	int xx = sd.X(i);
	if (x<xx) continue;
	int yy = sd.Y(i);
	if (y<yy) continue;
	if (x>xx+sd.SX(i)) continue;
	if (y>yy+sd.SY(i)) continue;
	return sd.Map(i, x-xx, y-yy);
      }
    return def;
  }
private:
  StringDisplay &sd;
  int def;
};
GameApi::BM GameApi::FontApi::string_display_to_bitmap(SD sd, int def)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StringDisplay *sdd = find_string_display(e, sd);
  Bitmap<int> *bm1 = new StringDisplayToBitmap(*sdd, def);
  env->deletes.push_back(std::shared_ptr<void>(bm1)); 
  Bitmap<Color> *bm2 = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(0,0,0,0), *bm1);
  return add_color_bitmap(e, bm2);
}

class ChooseGlyphFromFont : public GlyphInterface
{
public:
  ChooseGlyphFromFont(FontInterface &fi, long idx) : fi(fi), idx(idx) { }
  virtual void Prepare() { fi.Prepare(); }
  virtual int Top() const { return fi.Top(idx); }
  virtual int SizeX() const { return fi.SizeX(idx); }
  virtual int SizeY() const { return fi.SizeY(idx); }
  virtual int Map(int x, int y) const
  {
    return fi.Map(idx,x,y);
  }
private:
  FontInterface &fi;
  long idx;
};
GameApi::GI GameApi::FontApi::choose_glyph_from_font(FI font, long idx)
{
  FontInterface *fi = find_font_interface(e, font);
  return add_glyph_interface(e, new ChooseGlyphFromFont(*fi, idx));
}

class StringDisplayFromGlyphs : public StringDisplay
{
public:
  StringDisplayFromGlyphs(std::vector<GlyphInterface*> vec, std::string str, int x_gap, int empty_line_height) : vec(vec), x_gap(x_gap), str(str), empty_line_height(empty_line_height) { }
  virtual void Prepare() { 
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Prepare();
      }
  }
  virtual int Count() const { return vec.size(); }
  virtual int X(int c) const
  {
    int s = vec.size();
    int x = 0;
    for(int i=0;i<s;i++)
      {
	if (i==c) { return x; }
	GlyphInterface *gi = vec[i];
	x+=gi->SizeX();
	x+=x_gap;
	if (str[i]=='\n') x=0;
      }
    return 0;
  }
  virtual int Y(int c) const
  {
    //int s = str.size();
    int y = 0;
    int prev_pos = 0;
    for(int i=0;i<c;i++)
      {
	if (str[i]=='\n') { 
	  int maxy = 0;
	  if (prev_pos+1 >= c) { maxy = empty_line_height; }
	  for(int j=prev_pos+1;j<c;j++)
	    {
	      maxy = std::max(maxy, vec[j]->Top() + vec[j]->SizeY());
	    }
	  y+=maxy;
	  prev_pos = i;
	}
      }
    return y+PositiveDelta()+vec[c]->Top();
  }
  virtual int SX(int c) const
  {
    return vec[c]->SizeX();
  }
  virtual int SY(int c) const
  {
    return vec[c]->SizeY();
  }
  virtual int Map(int c, int x, int y) const
  {
    return vec[c]->Map(x,y);
  }
  int PositiveDelta() const
  {
    int sz = vec.size();
    int min_delta=0;
    for(int i=0;i<sz;i++)
      {
	int delta = vec[i]->Top();
	if (delta<min_delta) min_delta=delta;
      }
    return -min_delta;
  }
private:
  std::vector<GlyphInterface*> vec;
  int x_gap;
  std::string str;
  int empty_line_height;
};

GameApi::SD GameApi::FontApi::draw_text_string_sd(std::vector<GI> glyphs, std::string str, int gap_x, int empty_line_height)
{
  int s = glyphs.size();
  std::vector<GlyphInterface*> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_glyph_interface(e, glyphs[i]));
    }
  return add_string_display(e, new StringDisplayFromGlyphs(vec, str, gap_x, empty_line_height));
}

extern std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
int bitmap_find_data(std::string data);

class BitmapPrepareCache2 : public Bitmap<Color>
{
public:
  BitmapPrepareCache2(GameApi::Env &e, std::string id, Bitmap<Color> *bm) : e(e), id(id), bm(bm) { }
  void Prepare()
  {
    if (bitmap_find_data(id)!=-1) {
      return;
    }
    bm->Prepare();
    GameApi::BM num = add_color_bitmap2(e, bm);
    bitmap_prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  Bitmap<Color> *get_bm() const
  {
    int num = bitmap_find_data(id);
    if (num==-1) { const_cast<BitmapPrepareCache2*>(this)->Prepare(); num=bitmap_find_data(id); }
    GameApi::BM bm2;
    bm2.id = num;
    BitmapHandle *handle = find_bitmap(e, bm2);
    Bitmap<Color> *bbm = find_color_bitmap(handle);
    return bbm;
  }
  virtual int SizeX() const { return get_bm()->SizeX(); }
  virtual int SizeY() const { return get_bm()->SizeY(); }
  virtual Color Map(int x, int y) const
  {
    return get_bm()->Map(x,y);
  }
private:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
};


GameApi::BM GameApi::FontApi::draw_text_string(FI font, std::string str, int x_gap, int empty_line_height)
{
  std::cout << "draw_text_string: " << str << std::endl;
  int s = str.size();
  std::vector<GI> glyphs;
  for(int i=0;i<s;i++)
    {
      GI glyph = choose_glyph_from_font(font, (long)(str[i]));
      glyphs.push_back(glyph);
    }
  SD sd = draw_text_string_sd(glyphs, str, x_gap, empty_line_height);
  BM bm = string_display_to_bitmap(sd,0);
  //BitmapHandle *bbm = find_bitmap(e, bm);
  //Bitmap<Color> *bbm1 = find_color_bitmap(bbm);
  //Bitmap<Color> *bbm2 = new BitmapPrepareCache2(e, str, bbm1);
  //BitmapColorHandle *handle = new BitmapColorHandle;
  //handle->bm = bbm2;
  //BM bm2 = add_bitmap(e,handle);

  return bm;
}


GameApi::FI GameApi::FontApi::load_font(std::string ttf_filename, int sx, int sy)
{
#ifdef HAS_FREETYPE
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  void *priv_ = (void*)&env->lib;
  return add_font_interface(e, new FontInterfaceImpl(e, priv_, ttf_filename, gameapi_homepageurl, sx,sy));
#else
#endif
}

class RotateCmds : public CmdExecute
{
public:
  RotateCmds(CmdExecute *cmds, float v_x, float v_y, float v_z, float angle, float delta_angle) : cmds(cmds),v_x(v_x), v_y(v_y), v_z(v_z), old_angle(0.0), angle(angle), delta_angle(delta_angle) { do_changes(angle); }
  Point pos() const { return cmds->pos(); }
  int obj_type() const { return cmds->obj_type(); }
  void set_hooks(std::function<void (char)> f) { cmds->set_hooks(f); }
  void Execute(char c) {
    switch(c) {
    case 'r': 
      angle+=delta_angle; 
      setup_angle(); 
      break;
    case 't': 
      angle-=delta_angle; 
      setup_angle();
      break;
    default:
      cmds->Execute(c);
      break;
    }
  }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z) { return cmds->set_units(v_x,v_y,v_z); }
  virtual Vector get_unit_x() const { return cmds->get_unit_x(); }
  virtual Vector get_unit_y() const { return cmds->get_unit_y(); }
  virtual Vector get_unit_z() const { return cmds->get_unit_z(); }

  void setup_angle()
  {
    undo_changes();
    do_changes(angle);
  }
  void undo_changes()
  {
    Vector a_x = cmds->get_unit_x();
    Vector a_y = cmds->get_unit_y();
    Vector a_z = cmds->get_unit_z();
    Matrix m = Matrix::RotateAroundAxis(Vector(v_x,v_y,v_z), -old_angle);
    cmds->set_units(a_x*m,a_y*m,a_z*m);
    old_angle = 0.0f;
  }
  void do_changes(float newangle)
  {
    Vector a_x = cmds->get_unit_x();
    Vector a_y = cmds->get_unit_y();
    Vector a_z = cmds->get_unit_z();
    Matrix m = Matrix::RotateAroundAxis(Vector(v_x,v_y,v_z), newangle);
    cmds->set_units(a_x*m, a_y*m, a_z*m);
    old_angle = newangle;
  }
private:
  CmdExecute *cmds;
  float v_x, v_y, v_z;
  float old_angle;
  float angle;
  float delta_angle;
};
class RepeatCmds : public CmdExecute
{
public:
  RepeatCmds(CmdExecute *cmds, std::string repeat, float dx, float dy, float dz) : cmds(cmds),repeat(repeat),dx(Vector(dx,0.0,0.0)),dy(Vector(0.0,dy,0.0)),dz(Vector(0.0,0.0,dz)) 
  {
    type_delta=0;
  }
  Point pos() const { return cmds->pos() + Vector(pos_delta.x,pos_delta.y,pos_delta.z); }
  int obj_type() const { return type_delta+cmds->obj_type(); }
  void set_hooks(std::function<void (char)> f) { cmds->set_hooks(f); }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z) { cmds->set_units(v_x, v_y, v_z); }
  virtual Vector get_unit_x() const { return dx; }
  virtual Vector get_unit_y() const { return dy; }
  virtual Vector get_unit_z() const { return dz; }

  void Execute(char c) {
    switch(c) {
    case 'w': pos_delta-=dy; break;
    case 'a': pos_delta-=dx; break;
    case 's': pos_delta+=dy; break;
    case 'd': pos_delta+=dx; break;
    case 'n': pos_delta-=dz; break;
    case 'm': pos_delta+=dz; break;
    case 'u': type_delta++; break;
    case 'i': type_delta--; break;
    case '.':
      {
	int s = repeat.size();
	for(int i=0;i<s;i++) {
	  char c = repeat[i];
	  cmds->Execute(c);
	}
	break;
      }
    default:
      cmds->Execute(c);
      break;
    };
  }  
private:
  CmdExecute *cmds;
  std::string repeat;
  Point pos_delta;
  int type_delta;
  Vector dx,dy,dz;
};
class DefaultCmds : public CmdExecute
{
public:
  DefaultCmds(float dx, float dy, float dz) : ddx(dx),ddy(dy),ddz(dz), p_x(0), p_y(0), p_z(0),type(0),dx(Vector(dx,0.0,0.0)),dy(Vector(0.0,dy,0.0)),dz(Vector(0.0,0.0,dz)),point(0.0,0.0,0.0) { }
  Point pos() const { return point; }
  int obj_type() const { return type; }
  void set_hooks(std::function<void (char)> f) { m_f = f; }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z)
  {
    dx = v_x;
    dy = v_y;
    dz = v_z;
  }
  virtual Vector get_unit_x() const
  {
    return dz;
  }
  virtual Vector get_unit_y() const
  {
    return dy;
  }
  virtual Vector get_unit_z() const
  {
    return dz;
  }
  void Execute(char c) {
    switch(c) {
    case 'w': point-=dy; break;
    case 'a': point-=dx; break;
    case 's': point+=dy; break;
    case 'd': point+=dx; break;
    case 'n': point-=dz; break;
    case 'm': point+=dz; break;
    case 'u': type++; break;
    case 'i': type--; break;
    case '#': point=Point(0.0,0.0,0.0); 
      dx=Vector(ddx,0.0,0.0);
      dy=Vector(0.0,ddy,0.0);
      dz=Vector(0.0,0.0,ddz);
      break;

    case '.': m_f('.'); break;
    case ':': m_f(':'); break;
    };
  }

private:
  float ddx,ddy,ddz;
  int p_x, p_y, p_z;
  int type;
  Vector dx,dy,dz;
  Point point;
  std::function<void (char)> m_f;
};

GameApi::CMD GameApi::MovementNode::default_cmds(float dx, float dy, float dz)
{
  return add_cmds(e, new DefaultCmds(dx,dy,dz));
}
GameApi::CMD GameApi::MovementNode::cmd_repeat(CMD cmds2, std::string repeat, float dx, float dy, float dz)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_cmds(e, new RepeatCmds(cmds, repeat,dx,dy,dz));
}
GameApi::CMD GameApi::MovementNode::cmd_rotate(CMD cmds2, float v_x, float v_y, float v_z, float angle, float delta_angle)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_cmds(e, new RotateCmds(cmds,v_x,v_y,v_z, angle, delta_angle));
}

class CmdPoints : public PointsApiPoints
{
public:
  CmdPoints(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdPoints::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      cmds->Execute(commands[i]);
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      vec.push_back(p);
    }
  }
  virtual int NumPoints() const { return vec.size(); }
  virtual Point Pos(int i) const { return vec[i]; }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  CmdExecute *cmds;
  std::vector<Point> vec;
};

GameApi::PTS GameApi::MovementNode::cmd_to_pts(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_points_api_points(e, new CmdPoints(cmds, commands));
}

class CmdLines : public LineCollection
{
public:
  CmdLines(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdLines::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      cmds->Execute(commands[i]);
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      vec.push_back(old_point);
      vec.push_back(p);
      old_point = p;
    }
    if (c==':') {
      old_point = cmds->pos();
    }
  }
  virtual int NumLines() const { return vec.size()/2; }
  virtual Point LinePoint(int line, int point) const
  {
    line*=2;
    if (point==1) line++;
    return vec[line];
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  CmdExecute *cmds;
  std::vector<Point> vec;
  Point old_point;
};

class CmdPolygon : public FacesInPlane
{
public:
  CmdPolygon(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdPolygon::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      {
	cmds->Execute(commands[i]);
      }
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      Point2d pp = { p.x, p.y };
      vec.push_back(pp);
    }
  }
  virtual int Size() const { return 1; }
  virtual int NumPoints(int face) const { return vec.size(); }
  virtual Point2d Map(int face, int point) const
  {
    return vec[point];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
private:
  CmdExecute *cmds;
  std::vector<Point2d> vec;
};


GameApi::LI GameApi::MovementNode::cmd_to_li(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_line_array(e, new CmdLines(cmds, commands));
}
GameApi::PLF GameApi::MovementNode::cmd_to_plf(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_plane_faces(e, new CmdPolygon(cmds, commands));
}


void check_world(MainLoopEnv &e)
{
  if (e.current_world) return;
  e.current_world = new World;
}
void check_world_elements(MainLoopEnv &e)
{
  World *w = e.current_world;
  if (!w) return;
  if (w->world) return;
  w->world = new WorldElement[w->world_sx*w->world_sy*w->world_sz];
}

class Player : public MainLoopItem
{
public:
  Player(MainLoopItem *next) : next(next) { }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->player_matrix = e.in_MV;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
};


class Enemy : public MainLoopItem
{
public:
  Enemy(MainLoopItem *next) : next(next) { }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->enemy_matrix = e.in_MV;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
};

GameApi::ML GameApi::MovementNode::player(ML prev)
{
  MainLoopItem *item = find_main_loop(e, prev);
  return add_main_loop(e, new Player(item));
}
GameApi::ML GameApi::MovementNode::enemy(ML prev)
{
  MainLoopItem *item = find_main_loop(e, prev);
  return add_main_loop(e, new Enemy(item));
}
#endif // SECOND_PART
#ifdef THIRD_PART


class PlayerPos : public MainLoopItem
{
public:
  PlayerPos(MainLoopItem *next, Point p) : next(next),p(p) { }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->player_pos = p;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  Point p;
};

GameApi::ML GameApi::MovementNode::player_pos(ML prev, PT pos)
{
  MainLoopItem *item = find_main_loop(e, prev);
  Point *pt = find_point(e, pos);
  return add_main_loop(e, new PlayerPos(item, *pt));
}
EnemyPiece &get_enemy_piece(MainLoopEnv &e, int index)
{
  check_world(e);
  World *w = e.current_world;
  while (index >= (int)w->enemy_pieces.size()) {
    EnemyPiece piece;
    w->enemy_pieces.push_back(piece);
  }
  EnemyPiece &piece = w->enemy_pieces[index];
  return piece;
}

class EnemyPos : public MainLoopItem
{
public:
  EnemyPos(MainLoopItem *next, PointsApiPoints *pts) : next(next), pts(pts) { firsttime = true; }
  void Prepare() { 
    next->Prepare(); 

  }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    if (firsttime)
      {
    int s = pts->NumPoints();
    for(int i=0;i<s;i++)
      {
	Point pt = pts->Pos(i);
	EnemyPiece &p = get_enemy_piece(e, i);
	p.enemy_position = pt;
      }
	firsttime = false;
      }
    
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  PointsApiPoints *pts;
  bool firsttime;
};

GameApi::ML GameApi::MovementNode::enemy_pos(ML prev, PTS pos)
{
  MainLoopItem *item = find_main_loop(e, prev);
  PointsApiPoints *pts = find_pointsapi_points(e, pos);
  return add_main_loop(e, new EnemyPos(item, pts));
}

class BlitVoxel_voxel : public Voxel<int>
{
public:
  BlitVoxel_voxel(Voxel<int> *bg, Voxel<int> *obj, int p_x, int p_y, int p_z) : bg(bg), obj(obj),p_x(p_x), p_y(p_y), p_z(p_z) { }
  void Prepare() { bg->Prepare(); obj->Prepare(); }
  virtual int SizeX() const { return bg->SizeX(); }
  virtual int SizeY() const { return bg->SizeY(); }
  virtual int SizeZ() const { return bg->SizeZ(); }
  virtual int Map(int x, int y, int z) const
  {
    int val = obj->Map(x-p_x,y-p_y,z-p_z);
    if (val==-1) return bg->Map(x,y,z);
    return val;
  }
private:
  Voxel<int> *bg;
  Voxel<int> *obj;
  int p_x, p_y, p_z;
};
class BlitVoxel : public Voxel<int>
{
public:
  BlitVoxel(VolumeObject *obj,
	    int sx, int sy, int sz,
	    float start_x, float end_x,
	    float start_y, float end_y,
	    float start_z, float end_z,
	    int false_value, int true_value) : obj(obj),
					       sx(sx), sy(sy), sz(sz),
					       start_x(start_x), end_x(end_x),
					       start_y(start_y), end_y(end_y),
					       start_z(start_z), end_z(end_z),
					       false_value(false_value), true_value(true_value) { 
    int ss = sx*sy*sz;
    while (sx>600 || sy>600 ||sz>600 || ss>600*600*600) { sx/=2; sy/=2; sz/=2; ss=sx*sy*sz; std::cout << "Warning: Voxel size too large (" << sx << "x" << sy << "x" << sz << ")" << std::endl; }
}
  void Prepare() {}
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    float xx = float(x)/sx*(end_x-start_x)+start_x;
    float yy = float(y)/sy*(end_y-start_y)+start_y;
    float zz = float(z)/sz*(end_z-start_z)+start_z;
    Point p(xx,yy,zz);
    bool b = obj->Inside(p);
    if (b) {
      return true_value;
    }
    return false_value;
  }

private:
  Voxel<int> *next;
  VolumeObject *obj;
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int false_value, true_value;
};
class EmptyVoxel : public Voxel<int>
{
public:
  EmptyVoxel(int sx, int sy, int sz) : sx(sx), sy(sy), sz(sz) { }
  void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    return -1;
  }
private:
  int sx,sy,sz;
};
GameApi::VX GameApi::VoxelApi::empty_voxel(int sx, int sy, int sz)
{
  return add_int_voxel(e, new EmptyVoxel(sx,sy,sz));
}
class VoxelLandscape : public Voxel<int>
{
public:
  VoxelLandscape(Bitmap<float> &bm, int height, int false_value, int true_value) : bm(bm), height(height), false_value(false_value), true_value(true_value) {}
  void Prepare() { bm.Prepare(); }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return height; }
  virtual int SizeZ() const { return bm.SizeY(); }
  virtual int Map(int x, int y, int z) const
  {
    float val = bm.Map(x,z);
    float val0 = val*height;
    if (val0 < float(y)) { return false_value; }
    float val1 = bm.Map(x+1,z);
    float val2 = bm.Map(x,z+1);
    float val3 = bm.Map(x-1,z);
    float val4 = bm.Map(x,z-1);
    float m = std::min(std::min(val1,val2), std::min(val3,val4));
    float val00 = m*height - 1;
    if (val00 > float(y)) { return false_value; }
    return true_value;
  }
private:
  Bitmap<float> &bm;
  int height;
  int false_value;
  int true_value;
};
class SubVoxel : public Voxel<int>
{
public:
  SubVoxel(Voxel<int> *vx, int start_x, int end_x,
	   int start_y, int end_y,
	   int start_z, int end_z) : vx(vx), start_x(start_x), end_x(end_x),
				     start_y(start_y), end_y(end_y),
				     start_z(start_z), end_z(end_z) { }
  void Prepare() { vx->Prepare(); }

  virtual int SizeX() const { return end_x-start_x; }
  virtual int SizeY() const { return end_y-start_y; }
  virtual int SizeZ() const { return end_z-start_z; }
  virtual int Map(int x, int y, int z) const
  {
    // clip to specified area
    if (x<start_x) return -1;
    if (x>=end_x) return -1;
    if (y<start_y) return -1;
    if (y>=end_y) return -1;
    if (z<start_z) return -1;
    if (x>=end_z) return -1;
    // clip to available space
    int xx = x+start_x;
    int yy = y+start_y;
    int zz = z+start_z;
    if (xx>=vx->SizeX()) return -1;
    if (yy>=vx->SizeY()) return -1;
    if (zz>=vx->SizeZ()) return -1;
    if (xx<0) return -1;
    if (yy<0) return -1;
    if (zz<0) return -1;
    return vx->Map(xx, yy, zz);
  }
private:
  Voxel<int> *vx;
  int start_x, end_x;
  int start_y, end_y;
  int start_z, end_z;
};
GameApi::VX GameApi::VoxelApi::subvoxel(VX voxel, int start_x, int end_x,
					int start_y, int end_y,
					int start_z, int end_z)
{
  Voxel<int> *vx = find_int_voxel(e, voxel);
  return add_int_voxel(e, new SubVoxel(vx, start_x, end_x, start_y, end_y, start_z, end_z));
}
GameApi::VX GameApi::VoxelApi::voxel_landscape_from_fbm(FB bitmap, int height, int false_value, int true_value)
{
  Bitmap<float> *fbm = find_float_bitmap(e, bitmap)->bitmap;
  return add_int_voxel(e, new VoxelLandscape(*fbm, height, false_value, true_value));
}
GameApi::VX GameApi::VoxelApi::blit_voxel2(VX voxel, VX voxel2, int p_x, int p_y, int p_z)
{
  Voxel<int> *v0 = find_int_voxel(e, voxel);
  Voxel<int> *v1 = find_int_voxel(e, voxel2);
  return add_int_voxel(e, new BlitVoxel_voxel(v0,v1, p_x,p_y,p_z));
}
GameApi::VX GameApi::VoxelApi::blit_voxel(O object,
					  int sx, int sy, int sz,
					  float start_x, float end_x,
					  float start_y, float end_y,
					  float start_z, float end_z,
					  int false_value, int true_value)
{
  VolumeObject *obj = find_volume(e, object);
  return add_int_voxel(e,new BlitVoxel(obj, sx,sy,sz,
				       start_x, end_x,
				       start_y, end_y,
				       start_z, end_z,
				       false_value, true_value));
}


class VoxelToPTS 
{
public:
  VoxelToPTS(Voxel<int> *vx, int count, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : vx(vx),count(count), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z)
  {
    prepared=false;
  }
  void Prepare() {
    if (!prepared) {
    vx->Prepare();
    int sx = vx->SizeX();
    int sy = vx->SizeY();
    int sz = vx->SizeZ();
    if (sx && sy && sz)  prepared=true;
    Point p;
    float dx = end_x-start_x;
    float dy = end_y-start_y;
    float dz = end_z-start_z;
    pts.resize(count);
    for(int i=0;i<sx;i++) {
      p.x = float(i)/sx*dx+start_x;
      for(int j=0;j<sy;j++) {
	p.y = float(j)/sy*dy+start_y;
	for(int k=0;k<sz;k++)
	  {
	    int val = vx->Map(i,j,k);
	    if (val>=0 && val<count) {
	      p.z = float(k)/sz*dz+start_z;
	      int val_mx = vx->Map(i-1,j,k);
	      bool b_mx = val_mx>=0 && val_mx<count;
	      if (!b_mx) { pts[val].push_back(p); continue; }
	      int val_px = vx->Map(i+1,j,k);
	      bool b_px = val_px>=0 && val_px<count;
	      if (!b_px) { pts[val].push_back(p); continue; }
	      int val_my = vx->Map(i,j-1,k);
	      bool b_my = val_my>=0 && val_my<count;
	      if (!b_my) { pts[val].push_back(p); continue; }
	      int val_py = vx->Map(i,j+1,k);
	      bool b_py = val_py>=0 && val_py<count;
	      if (!b_py) { pts[val].push_back(p); continue; }
	      int val_mz = vx->Map(i,j,k-1);
	      bool b_mz = val_mz>=0 && val_mz<count;
	      if (!b_mz) { pts[val].push_back(p); continue; }
	      int val_pz = vx->Map(i,j,k+1);
	      bool b_pz = val_pz>=0 && val_pz<count;
	      if (!b_pz) { pts[val].push_back(p); continue; }
	    }
	  }
      }
    }
    }
  }
  PointsApiPoints *get(int val);
public:
  std::vector<std::vector<Point> > pts;
  Voxel<int> *vx;
  bool prepared;
  int count;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};

class VPTS : public PointsApiPoints
{
public:
  VPTS(VoxelToPTS &pts, int val, Voxel<int> *vx) : pts(pts), val(val),vx(vx) { }
  void Prepare() {
    pts.Prepare();
    /*vx->Prepare();*/ }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return pts.pts[val].size();
  }
  virtual Point Pos(int i) const
  {
    return pts.pts[val][i];
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  VoxelToPTS &pts;
  int val;
  Voxel<int> *vx;
};

PointsApiPoints *VoxelToPTS::get(int val)
{
  return new VPTS(*this, val,vx);
}

std::vector<GameApi::PTS> GameApi::arr_to_pts_arr(GameApi::EveryApi &ev, GameApi::ARR a)
{
  std::vector<GameApi::PTS> vec;
  ArrayType *t = find_array(ev.get_env(), a);
  int s = t->vec.size();
  for(int i=0;i<s;i++)
    {
      int val = t->vec[i];
      GameApi::PTS pts;
      pts.id = val;
      vec.push_back(pts);
    }
  return vec;
}

// ARR = array of PTS
GameApi::ARR GameApi::VoxelApi::voxel_instancing(VX voxel,
						 int count,
						 float start_x, float end_x,
						 float start_y, float end_y,
						 float start_z, float end_z)
{
  Voxel<int> *vx = find_int_voxel(e, voxel);
  VoxelToPTS *pts = new VoxelToPTS(vx, count, start_x, end_x, start_y, end_y, start_z, end_z);
  int s = count;
  std::vector<int> vec;
  ArrayType *array = new ArrayType;
  array->type = 2;
  for(int i=0;i<s;i++)
    {
      PointsApiPoints *p = pts->get(i);
      PTS pts = add_points_api_points(e, p);
      array->vec.push_back(pts.id);
    }
  return add_array(e,array);
}
GameApi::ML GameApi::VoxelApi::voxel_render(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<ML> mls;
  for(int i=0;i<s;i++)
    {
      ML o = ev.materials_api.render_instanced_ml(ev, objs[i], vec[i]);
      mls.push_back(o);
    }
  return ev.mainloop_api.array_ml(mls);
}
GameApi::ML GameApi::VoxelApi::voxel_bind(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec, MT mt)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<ML> mls;
  for(int i=0;i<s;i++)
    {
      ML o = ev.materials_api.bind_inst(objs[i], vec[i], mt);
      mls.push_back(o);
    }
  return ev.mainloop_api.array_ml(mls);
}
GameApi::P GameApi::VoxelApi::voxel_static(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<P> mls;
  for(int i=0;i<s;i++)
    {
      P o = ev.polygon_api.static_instancing(ev, objs[i], vec[i]);
      mls.push_back(o);
    }
  return ev.polygon_api.or_array2(mls);
}

GameApi::ML GameApi::MovementNode::all_cursor_keys(EveryApi &ev, ML ml, float speed, float duration)
{
  ML ml1 = cursor_keys(ev,ml,119,97,115,100, speed, duration);
  ML ml2 = cursor_keys(ev,ml1, 1073741906, 1073741904, 1073741905, 1073741903, speed, duration);
  ML ml3 = cursor_keys(ev,ml2, 82, 80, 81, 79, speed, duration);
  return ml3;
}

GameApi::ML GameApi::MovementNode::cursor_keys(EveryApi &ev, ML I10, int key_up, int key_left, int key_down, int key_right, float speed, float duration)
{
  int k_100 = key_right;
  int k_119 = key_up;
  int k_115 = key_down;
  int k_97 = key_left;

  float s_8 = speed;
  float s_234 = speed*sqrt(2) - speed;
  
  float s_1 = duration;
MN I11=ev.move_api.empty();
MN I12=ev.move_api.translate(I11,0,s_1,s_8,0,0);
ML I13=ev.move_api.key_activate_ml(ev,I10,I12,k_100,s_1);
MN I14=ev.move_api.empty();
MN I15=ev.move_api.translate(I14,0,s_1,0,-s_8,0);
ML I16=ev.move_api.key_activate_ml(ev,I13,I15,k_119,s_1);
MN I17=ev.move_api.empty();
MN I18=ev.move_api.translate(I17,0,s_1,0,s_8,0);
ML I19=ev.move_api.key_activate_ml(ev,I16,I18,k_115,s_1);
MN I20=ev.move_api.empty();
MN I21=ev.move_api.translate(I20,0,s_1,-s_8,0,0);
ML I22=ev.move_api.key_activate_ml(ev,I19,I21,k_97,s_1);
MN I23=ev.move_api.empty();
MN I24=ev.move_api.translate(I23,0,s_1,-s_234,s_234,0);
ML I25=ev.move_api.comb_key_activate_ml(ev,I22,I24,k_100,k_119,s_1);
MN I26=ev.move_api.empty();
MN I27=ev.move_api.translate(I26,0,s_1,-s_234,-s_234,0);
ML I28=ev.move_api.comb_key_activate_ml(ev,I25,I27,k_115,k_100,s_1);
MN I29=ev.move_api.empty();
MN I30=ev.move_api.translate(I29,0,s_1,s_234,-s_234,0);
ML I31=ev.move_api.comb_key_activate_ml(ev,I28,I30,k_115,k_97,s_1);
MN I32=ev.move_api.empty();
MN I33=ev.move_api.translate(I32,0,s_1,s_234,s_234,0);
ML I34=ev.move_api.comb_key_activate_ml(ev,I31,I33,k_119,k_97,s_1);
 return I34;
}

float quake_pos_x, quake_pos_y;
float quake_rot_y;

bool g_is_quake = false;

class QuakeML : public MainLoopItem
{
public:
  QuakeML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float speed, float rot_speed) : env(env), ev(ev), next(next), speed(speed), rot_speed(rot_speed) { 
    point.id=-1;
  }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    g_is_quake=true;
    GameApi::InteractionApi::quake_movement_frame(ev, pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    quake_pos_x = pos_x;
    quake_pos_y = pos_y;
    quake_rot_y = rot_y;
    MainLoopEnv eee = e;
    GameApi::M env_m = add_matrix2(env, e.in_MV);
    GameApi::M rot_y2 = ev.matrix_api.yrot(rot_y);
    GameApi::M trans = ev.matrix_api.trans(pos_x, 0.0, pos_y+400);
    GameApi::M trans2 = ev.matrix_api.trans(0.0,0.0,-400.0);
    GameApi::M scale = ev.matrix_api.scale(1.0,1.0,-1.0);
    GameApi::M res = ev.matrix_api.mult(env_m, ev.matrix_api.mult(ev.matrix_api.mult(ev.matrix_api.mult(trans,rot_y2),trans2),scale));

    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res));
    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);

    
    eee.in_MV = find_matrix(env, res); 
    eee.env = find_matrix(env, res);

    next->execute(eee);
    ev.shader_api.unuse(s3);

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (point.id==-1) {
      point = add_point(env, 0.0,0.0,0.0);
    }
    Point *pt = find_point(env,point);
    pt->x = e.cursor_pos.x;
    pt->y = e.cursor_pos.y;
    pt->z = e.cursor_pos.z;
    GameApi::MainLoopApi::Event ee;
    ee.type = e.type;
    ee.ch = e.ch;
    ee.button = e.button;
    ee.cursor_pos = point;
    GameApi::InteractionApi::quake_movement_event(ev,ee,pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    next->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PT point;
  MainLoopItem *next;
  float pos_x=0.0, pos_y=0.0, rot_y=0.0;
  GameApi::InteractionApi::Quake_data dt;
  float speed_x=0.0, speed_y=0.0;
  float speed, rot_speed;
};

GameApi::ML GameApi::MovementNode::quake_ml(EveryApi &ev, ML ml,float speed, float rot_speed)
{
  MainLoopItem *mml = find_main_loop(e,ml);
  return add_main_loop(e, new QuakeML(e,ev, mml, speed, rot_speed));
}

class QuakeML2 : public MainLoopItem
{
public:
  QuakeML2(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float speed, float rot_speed) : env(env), ev(ev), next(next), speed(speed), rot_speed(rot_speed) { 
    point.id=-1;
  }
  void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    g_is_quake=true;
    GameApi::InteractionApi::quake_movement_frame(ev, pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    quake_pos_x = pos_x;
    quake_pos_y = -pos_y;
    quake_rot_y = rot_y;
    MainLoopEnv eee = e;
    GameApi::M env_m = add_matrix2(env, e.in_MV);
    GameApi::M rot_y2 = ev.matrix_api.yrot(rot_y);
    GameApi::M trans = ev.matrix_api.trans(pos_x, 0.0, pos_y +400.0);
    GameApi::M trans2 = ev.matrix_api.trans(0.0,0.0,-400.0);
    GameApi::M scale = ev.matrix_api.scale(1.0,1.0,-1.0);
    GameApi::M res = ev.matrix_api.mult(env_m, ev.matrix_api.mult(ev.matrix_api.mult(ev.matrix_api.mult(trans,rot_y2),trans2),scale));

    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res));
    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res);
    ev.shader_api.set_var(s1, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res);
    ev.shader_api.set_var(s2, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res);
    ev.shader_api.set_var(s3, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);

    
    eee.in_MV = find_matrix(env, res); 
    eee.env = find_matrix(env, res);

    next->execute(eee);
    ev.shader_api.unuse(s3);

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (point.id==-1) {
      point = add_point(env, 0.0,0.0,0.0);
    }
    Point *pt = find_point(env,point);
    pt->x = e.cursor_pos.x;
    pt->y = e.cursor_pos.y;
    pt->z = e.cursor_pos.z;
    GameApi::MainLoopApi::Event ee;
    ee.type = e.type;
    ee.ch = e.ch;
    ee.button = e.button;
    ee.cursor_pos = point;
    GameApi::InteractionApi::quake_movement_event(ev,ee,pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    next->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PT point;
  MainLoopItem *next;
  float pos_x=0.0, pos_y=0.0, rot_y=0.0;
  GameApi::InteractionApi::Quake_data dt;
  float speed_x=0.0, speed_y=0.0;
  float speed, rot_speed;
};

GameApi::ML GameApi::MovementNode::quake_ml2(EveryApi &ev, ML ml,float speed, float rot_speed)
{
  GameApi::MN mn = ev.move_api.empty();
  GameApi::MN scale = ev.move_api.scale2(mn, 1.0,1.0,-1.0);
  GameApi::ML ml2 = ev.move_api.move_ml(ev,ml,scale, 1, 10.0);
  MainLoopItem *mml = find_main_loop(e,ml2);
  return add_main_loop(e, new QuakeML2(e,ev, mml, speed, rot_speed));
}

class QuakeML3 : public MainLoopItem
{
public:
  QuakeML3(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, MainLoopItem *next2, float speed, float rot_speed, Point p) : env(env), ev(ev), next(next), next2(next2), speed(speed), rot_speed(rot_speed),p(p) { 
    point.id=-1;
  }
  void Prepare() { next->Prepare(); next2->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    g_is_quake=true;
    GameApi::InteractionApi::quake_movement_frame(ev, pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    quake_pos_x = pos_x;
    quake_pos_y = -pos_y;
    quake_rot_y = rot_y;
    MainLoopEnv eee = e;
    GameApi::M env_m = add_matrix2(env, e.in_MV);
    GameApi::M rot_y2 = ev.matrix_api.yrot(rot_y);
    GameApi::M trans_y2 = ev.matrix_api.trans(p.x,p.y,p.z);
    GameApi::M res = ev.matrix_api.mult(env_m, ev.matrix_api.mult(rot_y2,trans_y2));
    GameApi::M trans = ev.matrix_api.trans(pos_x, 0.0, pos_y +400.0);
    GameApi::M trans2 = ev.matrix_api.trans(0.0,0.0,-400.0);
    GameApi::M scale = ev.matrix_api.scale(1.0,1.0,-1.0);
    GameApi::M res2 = ev.matrix_api.mult(env_m, ev.matrix_api.mult(ev.matrix_api.mult(trans,trans2),scale));

    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res));
    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res);
    ev.shader_api.set_var(s1, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res);
    ev.shader_api.set_var(s2, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res);
    ev.shader_api.set_var(s3, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);

    
    eee.in_MV = find_matrix(env, res); 
    eee.env = find_matrix(env, res);

    next->execute(eee);
    ev.shader_api.unuse(s3);

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res2);
    ev.shader_api.set_var(s1, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res2);
    ev.shader_api.set_var(s2, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res2);
    ev.shader_api.set_var(s3, "in_T", ev.matrix_api.identity());
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);

    
    eee.in_MV = find_matrix(env, res2); 
    eee.env = find_matrix(env, res2);

    next2->execute(eee);
    ev.shader_api.unuse(s3);


  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (point.id==-1) {
      point = add_point(env, 0.0,0.0,0.0);
    }
    Point *pt = find_point(env,point);
    pt->x = e.cursor_pos.x;
    pt->y = e.cursor_pos.y;
    pt->z = e.cursor_pos.z;
    GameApi::MainLoopApi::Event ee;
    ee.type = e.type;
    ee.ch = e.ch;
    ee.button = e.button;
    ee.cursor_pos = point;
    GameApi::InteractionApi::quake_movement_event(ev,ee,pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    next->handle_event(e);
    next2->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PT point;
  MainLoopItem *next;
  MainLoopItem *next2;
  float pos_x=0.0, pos_y=0.0, rot_y=0.0;
  GameApi::InteractionApi::Quake_data dt;
  float speed_x=0.0, speed_y=0.0;
  float speed, rot_speed;
  Point p;
};

GameApi::ML GameApi::MovementNode::quake_ml3(EveryApi &ev, ML ml,ML ml3,float speed, float rot_speed, float p_x, float p_y, float p_z)
{
  GameApi::MN mn = ev.move_api.empty();
  float y_flip = 1.0;
  // not needed since android changed again.
  //if (ev.mainloop_api.get_screen_width() < 700)
  //  {
  //    y_flip = -1.0;
  //  }
  GameApi::MN scale = ev.move_api.scale2(mn, 1.0,1.0,-1.0);
  GameApi::ML ml2 = ev.move_api.move_ml(ev,ml3,scale, 1, 10.0);
  MainLoopItem *mml = find_main_loop(e,ml2);

  GameApi::MN scale4 = ev.move_api.scale2(mn, 1.0,y_flip,1.0);
  GameApi::ML ml4 = ev.move_api.move_ml(ev,ml,scale4, 1, 10.0);

  MainLoopItem *mml2 = find_main_loop(e,ml4);
  return add_main_loop(e, new QuakeML3(e,ev, mml2, mml, speed, rot_speed, Point(p_x,p_y,p_z)));
}


class LocalMove : public MainLoopItem
{
public:
  LocalMove(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *inner, PointsApiPoints *o) : env(env), ev(ev), inner(inner), o(o) { }
  void Prepare() { inner->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    o->Update(e);
    int s = o->NumPoints();
    for(int i=0;i<s;i++)
      {
	Point p = o->Pos(i);
	GameApi::M m = ev.matrix_api.trans(p.x,p.y,p.z);
	GameApi::M m2 = add_matrix2(env, e.in_MV);
	MainLoopEnv ee = e;

	GameApi::SH s1;
	s1.id = e.sh_texture;
	GameApi::SH s11;
	s11.id = e.sh_texture_2d;
	GameApi::SH s2;
	s2.id = e.sh_array_texture;
	GameApi::SH s3;
	s3.id = e.sh_color;
	
	GameApi::M mat2 = ev.matrix_api.mult(m,m2);
	//GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1, "in_MV", mat2);
	//ev.shader_api.set_var(s1, "in_iMV", mat2i);
	ev.shader_api.use(s11);
	ev.shader_api.set_var(s11, "in_MV", mat2);
	//ev.shader_api.set_var(s11, "in_iMV", mat2i);
	ev.shader_api.use(s2);
	ev.shader_api.set_var(s2, "in_MV", mat2);
	//ev.shader_api.set_var(s2, "in_iMV", mat2i);
	ev.shader_api.use(s3);
	ev.shader_api.set_var(s3, "in_MV", mat2);
	//ev.shader_api.set_var(s3, "in_iMV", mat2i);
	
	
	ee.in_MV = find_matrix(env, m) * e.in_MV;
	ee.env = find_matrix(env, m) * e.in_MV;
	inner->execute(ee);
	ev.shader_api.unuse(s3);
      }	  
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    inner->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *inner;
  PointsApiPoints *o;
};
GameApi::ML GameApi::MovementNode::local_move(EveryApi &ev, ML inner_ml, PTS center_points)
{
  MainLoopItem *item = find_main_loop(e, inner_ml);
  PointsApiPoints *o = find_pointsapi_points(e, center_points);
  return add_main_loop(e, new LocalMove(e, ev, item, o));
}


#if 0
class ModifyWorld : public MainLoopItem, public WorldSpec
{
public:
  ModifyWorld(Cursor &c) : c(c) { }
  virtual int NumItems() const { return item.size(); }
  virtual int PosX(int i) const { return pos_x[i]; }
  virtual int PosY(int i) const { return pos_y[i]; }
  virtual int Item(int i) const { return item[i]; }

  virtual void execute(MainLoopEnv &e)
  {
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    c.update();
    static const char *letters = "qwertyuiopasdfghjklzxcvbnm";
    int s = strlen(letters);
    int value = -1;
    for(int i=0;i<s;i++)
      {
	if (e.ch==letters[i]) {
	  value = i;
	}
      }
    if (value != -1 && e.type==0x300)
      {
	int p_x = c.PosX();
	int p_y = c.PosY();
	pos_x.push_back(p_x);
	pos_y.push_back(p_y);
	item.push_back(value);
      }
  }
private:
  std::vector<int> pos_x;
  std::vector<int> pos_y;
  std::vector<int> item;
};
#endif

class ExplosionChange : public DynamicChange
{
public:
  ExplosionChange(Point center, float start_val, float end_val, float start_time, float end_time) : center(center), start_val(start_val), end_val(end_val), start_time(start_time), end_time(end_time) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    float time = e.time*10.0;
    float d = start_val;
    if (time < start_time) { d=start_val; }
    else if (time>end_time) { d=end_val; }
    else
      {
	d = start_val + (end_val-start_val)*(time-start_time)/(end_time-start_time);
      }
    int s = size/3;
    for(int i=0;i<s;i++)
      {
	float x = source[i*3];
	float y = source[i*3+1];
	float z = source[i*3+2];
	Point p(x,y,z);
	Vector v=p-center;
	v*=d;
	p+=v;
	target[i*3] = p.x;
	target[i*3+1] = p.y;
	target[i*3+2] = p.z;
      }
    
  }
private:
  Point center;
  float start_val, end_val;
  float start_time, end_time;
};

class SphWaveChange : public DynamicChange
{
public:
  SphWaveChange(float r1, float fr_1, float r2, float fr_2, float t_1, float t_2) : r1(r1), r2(r2), fr_1(fr_1), fr_2(fr_2), t_1(t_1), t_2(t_2) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    int s = size/3;
    SphericalPoint sp(Point(0.0,0.0,0.0));
    for(int i=0;i<s;i++)
      {
	Point p(source[0],source[1],source[2]);
	sp.FromPoint(p);
	sp.r = sp.r + r1*cos(fr_1*sp.alfa+t_1*e.time) + r2*cos(fr_2*sp.beta+t_2*e.time);
	Point p2 = sp.ToPoint();
	target[0] = p2.x;
	target[1] = p2.y;
	target[2] = p2.z;
	source+=3;
	target+=3;
      }
  }
private:
  float r1,r2;
  float fr_1, fr_2;
  float t_1, t_2;
};

class WaveChange : public DynamicChange
{
public:
  WaveChange(float r, float speed1,float speed2, float dist1, float dist2, int sx, int sy) : r(r),speed1(speed1), speed2(speed2), dist1(dist1), dist2(dist2), sx(sx), sy(sy) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    //std::cout << "applychange: " << size << std::endl;
    int s = size/3;
    float s1 = speed1*e.time;
    float s2 = speed2*e.time;
    float delta1 = dist1/sx;
    float delta2 = dist2/sy;
    float x,y,z;
    for(int i=0;i<s;i++)
      {
	if (i%sx==0) { s2+=delta2; s1=speed1*e.time; }
	x = source[i*3];
	y = source[i*3+1];
	z = source[i*3+2];
	x+=r*sin(s1)*cos(s2);
	y+=r*sin(s1)*sin(s2);
	z+=r*cos(s1);
	target[i*3] = x;
	target[i*3+1] = y;
	target[i*3+2] = z;
	s1+=delta1;
      }
  }
private:
  float r;
  float speed1,speed2;
  float dist1, dist2;
  int sx,sy;
};
class IdentityChange : public DynamicChange
{
public:
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    for(int i=0;i<size;i++)
      {
	*target++ = *source++;
      }     
  }
};

GameApi::DC GameApi::MovementNode::identity()
{
  return add_dyn_change(e, new IdentityChange);
}
GameApi::DC GameApi::MovementNode::explosion(float center_x, float center_y, float center_z, float start_val, float end_val, float start_time, float end_time)
{
  return add_dyn_change(e, new ExplosionChange(Point(center_x, center_y, center_z), start_val, end_val, start_time, end_time));
}
GameApi::DC GameApi::MovementNode::wave(float r, float speed1, float speed2, float dist1, float dist2, int sx, int sy)
{
  return add_dyn_change(e, new WaveChange(r,speed1, speed2, dist1, dist2, sx,sy));
}
GameApi::DC GameApi::MovementNode::sphwave(float r1, float fr_1, float t_1, float r2, float fr_2, float t_2)
{
  return add_dyn_change(e, new SphWaveChange(r1,fr_1,r2,fr_2,t_1,t_2));
}
class SplitChange : public DynamicChange
{
public:
  SplitChange(DynamicChange *d1, DynamicChange *d2, float val) : d1(d1),d2(d2), val(val) {
    if (val<0.0) val=0.0;
    if (val>1.0) val=1.0;
  }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    int s = size/3;
    float v = val*s;
    int ss = (int)v;
    ss*=3;
    d1->applychange(source,target,ss,e);
    d2->applychange(source+ss,target+ss,size-ss,e);
  }
private:
  DynamicChange *d1;
  DynamicChange *d2;
  float val;
};
GameApi::DC GameApi::MovementNode::split(DC d1, DC d2, float val)
{
  DynamicChange *dd1 = find_dyn_change(e, d1);
  DynamicChange *dd2 = find_dyn_change(e, d2);
  return add_dyn_change(e, new SplitChange(dd1,dd2,val));
}

class WavePoints : public PointsApiPoints
{
public:
  WavePoints(Waveform &wv, int num_samples, Point pos, Vector u_x, Vector u_y) : wv(wv), num_samples(num_samples),pos(pos), u_x(u_x), u_y(u_y) { }
  virtual void HandleEvent(MainLoopEvent &event) { wv.HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return wv.Update(e); }
  virtual int NumPoints() const
  {
    return num_samples;
  }
  virtual Point Pos(int i) const
  {
    float val = float(i)/num_samples*wv.Length();
    float v = wv.Index(val);
    return pos + val*u_x + v*u_y;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Waveform &wv;
  int num_samples;
  Point pos;
  Vector u_x, u_y;
};
GameApi::PTS GameApi::PointsApi::wave_points(WV wave, int num_samples,
					     float pos_x, float pos_y, float pos_z,
					     float u_x_x, float u_x_y, float u_x_z,
					     float u_y_x, float u_y_y, float u_y_z)
{
  Waveform *wv = find_waveform(e, wave);
  return add_points_api_points(e, new WavePoints(*wv, num_samples, Point(pos_x, pos_y, pos_z), Vector(u_x_x, u_x_y, u_x_z), Vector(u_y_x, u_y_y, u_y_z)));
}

class FilterComponent : public PointsApiPoints
{
public:
  FilterComponent(PointsApiPoints *pts, int comp, float val) : pts(pts), comp(comp), val(val) { }
  void HandleEveent(MainLoopEvent &event) { pts->HandleEvent(event); }
  bool Update(MainLoopEnv &e) { return pts->Update(e); }
  int NumPoints() const { return pts->NumPoints(); }
  Point Pos(int i) const
  {
    Point p = pts->Pos(i);
    if (comp==0) { p.x=val; }
    if (comp==1) { p.y=val; }
    if (comp==2) { p.z=val; }
    return p;
  }
  unsigned int Color(int i) const {return pts->Color(i); }
private:
  PointsApiPoints *pts;
  int comp;
  float val;
};

GameApi::PTS GameApi::PointsApi::filter_component(PTS pts, int comp, float val)
{
  PointsApiPoints *pts2 = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new FilterComponent(pts2, comp, val));
}

class AnimRotPTS : public PointsApiPoints
{
public:
  AnimRotPTS(PointsApiPoints *next, float start_time, float end_time, Vector v, float rotate_amount) : next(next), start_time(start_time), end_time(end_time), v(v), rotate_amount(rotate_amount) {
    m = Matrix::Identity();
    firsttime = true;
  }
  void HandleEvent(MainLoopEvent &event) { next->HandleEvent(event); }
  bool Update(MainLoopEnv &e) {
    if (firsttime) { firsttime = false; return true; }
    float time = e.time*10.0;
    if (time < start_time) { m = Matrix::Identity(); return false; }
    if (time > end_time) { m = Matrix::RotateAroundAxis(v,rotate_amount); return false; }
    
    float val = (time-start_time)/(end_time-start_time);
    val*=rotate_amount;
    m = Matrix::RotateAroundAxis(v, val);
    
    return true;
  }
  int NumPoints() const { return next->NumPoints(); }
  Point Pos(int i) const
  {
    return next->Pos(i)*m;
  }
  unsigned int Color(int i) const { return next->Color(i); }
private:
  PointsApiPoints *next;
  float start_time, end_time;
  Vector v;
  float rotate_amount;
  Matrix m;
  bool firsttime;
};

GameApi::PTS GameApi::PointsApi::anim_rot_pts(PTS pts, float start_time, float end_time, float v_x, float v_y, float v_z, float rotate_amount)
{
  PointsApiPoints *next = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new AnimRotPTS(next, start_time, end_time, Vector(v_x,v_y,v_z), rotate_amount));
}

class Scale2dScreen : public MainLoopItem
{
public:
  Scale2dScreen(GameApi::EveryApi &ev, MainLoopItem *item, float sx, float sy) : ev(ev), item(item), sx(sx), sy(sy) {}
  void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    float source_sx = sx;
    float source_sy = sy;
    float target_sx = ev.mainloop_api.get_screen_sx();
    float target_sy = ev.mainloop_api.get_screen_sy();
    //float target_sx = 200;
    //float target_sy = 200;
    Matrix m1 = Matrix::Scale(target_sx/source_sx,target_sy/source_sy,1.0);
    ee.in_MV = e.in_MV * m1;
    ee.env = e.env * m1;
    item->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  float sx,sy;
};

GameApi::ML GameApi::MainLoopApi::scale_2d_screen(GameApi::EveryApi &ev, ML orig, float sx, float sy)
{
  MainLoopItem *item = find_main_loop(e, orig);
  return add_main_loop(e, new Scale2dScreen(ev, item,sx,sy));
}

GameApi::PN GameApi::WaveformApi::std_polynomial(float x_5, float x_4, float x_3, float x_2, float x_1, float c)
{
  std::vector<float> *vec = new std::vector<float>;
  vec->push_back(c);
  vec->push_back(x_1);
  vec->push_back(x_2);
  vec->push_back(x_3);
  vec->push_back(x_4);
  vec->push_back(x_5);
  return add_polynomial(e, vec);
}
class PolynomialWave : public Waveform
{
public:
  PolynomialWave(std::vector<float> *vec, float start_x, float end_x, float start_y, float end_y) : vec(vec), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
  void HandleEvent(MainLoopEvent &event) { }
  bool Update(MainLoopEnv &e) { return false; }
  float Length() const {
    return end_x-start_x;
  }
  float Min() const { return start_y; }
  float Max() const { return end_y; }
  float Index(float val) const
  {
    val+=start_x;
    std::vector<float> basis = Polynomial::basis_vector(vec->size(), val);
    float v = Polynomial::mul(*vec, basis);
    return v;
  }
private:
  std::vector<float> *vec;
  float start_x, end_x;
  float start_y, end_y;
};

GameApi::WV GameApi::WaveformApi::polynomial_wave(PN pn, float start_x, float end_x, float start_y, float end_y)
{
  std::vector<float> *poly = find_polynomial(e, pn);
  return add_waveform(e, new PolynomialWave(poly, start_x, end_x, start_y, end_y));
}

GameApi::PN GameApi::WaveformApi::df_dx(PN poly)
{
  std::vector<float> *vec = find_polynomial(e, poly);
  int sx = vec->size();
  int sy = vec->size();
  std::vector<float> vec2 = Polynomial::mul_matrix(PolyMatrix::df_per_dx(sx,sy), *vec);
  std::vector<float> *vec3 = new std::vector<float>(vec2);
  return add_polynomial(e, vec3);
}


class TouchRotate : public MainLoopItem
{
public:
  TouchRotate(GameApi::Env &e2, GameApi::EveryApi &ev, MainLoopItem *next, bool leftright, bool topdown, float x_speed, float y_speed) : e2(e2), ev(ev), next(next), leftright(leftright), topdown(topdown), x_speed(x_speed), y_speed(y_speed) { mousedown=false; fixed=Point(0.0,0.0,0.0); 
    if (!leftright) x_speed=0.0;
    if (!topdown) y_speed=0.0;
    accumulated=Matrix::Identity();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e) 
  {
    if ((e.type==1025 && e.button==0) /*|| e.type==Low_SDL_FINGERDOWN*/) {
      mousedown=true;
      mousedown_pos = e.cursor_pos;
    }
    if ((e.type==1026 && e.button==-1) /*|| e.type==Low_SDL_FINGERUP*/) {
      mousedown=false;
      fixed+=mouse_delta;
      mouse_delta=Vector(0.0,0.0,0.0);
    }
    if ((e.type==1024 /*||e.type==Low_SDL_FINGERMOTION*/) && mousedown)
      {
	mouse_delta = e.cursor_pos - mousedown_pos;
        mousedown_pos = e.cursor_pos;
      }
    next->handle_event(e);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    float spx = x_speed;
    float spy = y_speed;
    if (e.screen_width <700) { // test for mobile
    spx = -spx;
    spy = -spy;
    }
    Matrix curr1 = Matrix::YRotation(mouse_delta.dx*spx);
    Matrix curr2 = Matrix::XRotation(mouse_delta.dy*spy);
    Matrix curr = curr1 * curr2;
    
    accumulated *= curr;
    mouse_delta=Vector(0.0,0.0,0.0);

    MainLoopEnv ee = e;
    ee.in_MV = ee.in_MV * accumulated; //Matrix::XRotation((fixed.y+mouse_delta.dy)*y_speed) * Matrix::YRotation((fixed.x+mouse_delta.dx)*x_speed);
    ee.env = ee.env * accumulated; //Matrix::XRotation((fixed.y+mouse_delta.dy)*y_speed) * Matrix::YRotation((fixed.x+mouse_delta.dx)*x_speed);

    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s11;
    s11.id = e.sh_texture_2d;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;


    GameApi::M mat2 = add_matrix2(e2,ee.env);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);


    next->execute(ee);
    ev.shader_api.unuse(s3);
  }
private:
  GameApi::Env &e2;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  bool leftright;
  bool topdown;
  
  bool mousedown;
  Point mousedown_pos;
  Vector mouse_delta;
  Point fixed;
  float x_speed, y_speed;

  Matrix accumulated;
  
};

GameApi::ML GameApi::MainLoopApi::touch_rotate(GameApi::EveryApi &ev, ML ml, bool leftright, bool topdown, float x_speed, float y_speed)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new TouchRotate(e,ev,item, leftright, topdown,x_speed,y_speed));
}

GameApi::ML GameApi::MainLoopApi::score_display(EveryApi &ev, ML ml, std::string font)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);

  IF I21=ev.font_api.score_fetcher(ev);
  SF I22=ev.font_api.int_to_string_fetcher(I21);

  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);

  IF I25=ev.font_api.char_fetcher_from_string(I22,"0123456789",1);
  ML I26=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I25,0,0);

  IF I27=ev.font_api.char_fetcher_from_string(I22,"0123456789",2);
  ML I28=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I27,0,0);

  IF I29=ev.font_api.char_fetcher_from_string(I22,"0123456789",3);
  ML I30=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I29,0,0);

  IF I31=ev.font_api.char_fetcher_from_string(I22,"0123456789",4);
  ML I32=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I31,0,0);

  MN I49a=ev.move_api.empty();
  MN I50a=ev.move_api.trans2(I49a,35,0,0);
  ML I51a=ev.move_api.move_ml(ev,I26,I50a,1,10.0);

  MN I49b=ev.move_api.empty();
  MN I50b=ev.move_api.trans2(I49b,35+35,0,0);
  ML I51b=ev.move_api.move_ml(ev,I28,I50b,1,10.0);

  MN I49c=ev.move_api.empty();
  MN I50c=ev.move_api.trans2(I49c,35+35+35,0,0);
  ML I51c=ev.move_api.move_ml(ev,I30,I50c,1,10.0);

  MN I49d=ev.move_api.empty();
  MN I50d=ev.move_api.trans2(I49d,35+35+35+35,0,0);
  ML I51d=ev.move_api.move_ml(ev,I32,I50d,1,10.0);


  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51a,I51b,I51c,I51d});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;

}

GameApi::ML GameApi::MainLoopApi::time_display(EveryApi &ev, ML ml, std::string font, float start_time)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);

  IF I21=ev.font_api.time_fetcher(ev, start_time);
  SF I22=ev.font_api.int_to_string_fetcher(I21);

  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);

  IF I25=ev.font_api.char_fetcher_from_string(I22,"0123456789",1);
  ML I26=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I25,0,0);

  IF I27=ev.font_api.char_fetcher_from_string(I22,"0123456789",2);
  ML I28=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I27,0,0);

  IF I29=ev.font_api.char_fetcher_from_string(I22,"0123456789",3);
  ML I30=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I29,0,0);

  IF I31=ev.font_api.char_fetcher_from_string(I22,"0123456789",4);
  ML I32=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I31,0,0);

  MN I49a=ev.move_api.empty();
  MN I50a=ev.move_api.trans2(I49a,35,0,0);
  ML I51a=ev.move_api.move_ml(ev,I26,I50a,1,10.0);

  MN I49b=ev.move_api.empty();
  MN I50b=ev.move_api.trans2(I49b,35+35,0,0);
  ML I51b=ev.move_api.move_ml(ev,I28,I50b,1,10.0);

  MN I49c=ev.move_api.empty();
  MN I50c=ev.move_api.trans2(I49c,35+35+35,0,0);
  ML I51c=ev.move_api.move_ml(ev,I30,I50c,1,10.0);

  MN I49d=ev.move_api.empty();
  MN I50d=ev.move_api.trans2(I49d,35+35+35+35,0,0);
  ML I51d=ev.move_api.move_ml(ev,I32,I50d,1,10.0);


  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51a,I51b,I51c,I51d});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;

}


GameApi::ML GameApi::MainLoopApi::fps_display(EveryApi &ev, ML ml, std::string font)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);
  FF I21=ev.font_api.fps_fetcher(ev);
  SF I22=ev.font_api.float_to_string_fetcher(I21);
  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);
  FI I25=ev.font_api.load_font(font,30,30);
  BM I26=ev.font_api.draw_text_string(I25,"0",5,30);
  FI I27=ev.font_api.load_font(font,30,30);
  BM I28=ev.font_api.draw_text_string(I27,"1",5,30);
  FI I29=ev.font_api.load_font(font,30,30);
  BM I30=ev.font_api.draw_text_string(I29,"2",5,30);
  FI I31=ev.font_api.load_font(font,30,30);
  BM I32=ev.font_api.draw_text_string(I31,"3",5,30);
  FI I33=ev.font_api.load_font(font,30,30);
  BM I34=ev.font_api.draw_text_string(I33,"4",5,30);
  FI I35=ev.font_api.load_font(font,30,30);
  BM I36=ev.font_api.draw_text_string(I35,"5",5,30);
  FI I37=ev.font_api.load_font(font,30,30);
  BM I38=ev.font_api.draw_text_string(I37,"6",5,30);
  FI I39=ev.font_api.load_font(font,30,30);
  BM I40=ev.font_api.draw_text_string(I39,"7",5,30);
  FI I41=ev.font_api.load_font(font,30,30);
  BM I42=ev.font_api.draw_text_string(I41,"8",5,30);
  FI I43=ev.font_api.load_font(font,30,30);
  BM I44=ev.font_api.draw_text_string(I43,"9",5,30);
  FF I45=ev.font_api.fps_fetcher(ev);
  SF I46=ev.font_api.float_to_string_fetcher(I45);
  IF I47=ev.font_api.char_fetcher_from_string(I46,"0123456789",1);
  ML I48=ev.font_api.dynamic_character(ev,std::vector<BM>{I26,I28,I30,I32,I34,I36,I38,I40,I42,I44},I47,0,0);
  MN I49=ev.move_api.empty();
  MN I50=ev.move_api.trans2(I49,35,0,0);
  ML I51=ev.move_api.move_ml(ev,I48,I50,1,10.0);
  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;
}

int find_str(std::string val, std::string repl)
{
  std::size_t pos = val.find(repl);
  if (pos==std::string::npos) return -1;
  return pos;
}

std::string replace_str(std::string val, std::string repl, std::string subst)
{
  std::string s = "";
  int p = 0;
  while(1) {
    int pos = find_str(val,repl);
    if (pos==-1) { s+=val.substr(p,val.size()-p); return s; }
    s+=val.substr(p,pos-p);
    s+=subst;
    val[pos]='X'; // remove %N
    p=pos+repl.size();
  }

  while(1) {
  int pos = find_str(val, repl);
  if (pos==-1) return val;
  val.replace(pos,pos+repl.size()-1,subst);
  }
  return "ERROR";
}
void P_cb(void *data);

class P_script : public FaceCollection
{
public:
  P_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), coll(0) {
    e.async_load_callback(url, &P_cb, this);
#ifdef EMSCRIPTEN
    async_pending_count++; async_taken = true;
#endif
       std::cout << "async_pending_count inc (P_sctipr) " << async_pending_count << std::endl;
       std::cout << "P_script url: " << url << std::endl;
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    code = replace_str(code, "%1", p1);
    code = replace_str(code, "%2", p2);
    code = replace_str(code, "%3", p3);
    code = replace_str(code, "%4", p4);
    code = replace_str(code, "%5", p5);
    GameApi::ExecuteEnv e2;
    std::pair<int,std::string> p = GameApi::execute_codegen(e,ev,code,e2);
    if (p.second=="P") {
      GameApi::P pp;
      pp.id = p.first;
      p_data = pp;
      coll = find_facecoll(e,p_data);
#ifdef EMSCRIPTEN
      if (async_taken)
      	async_pending_count--;
#endif
      async_taken=false;
       std::cout << "async_pending_count inc (P_sctipt) " << async_pending_count << std::endl;

      return;
    }
#ifdef EMSCRIPTEN
    if (async_taken)
      async_pending_count--;
#endif
    async_taken = false;
       std::cout << "async_pending_count inc (P_sctipt2) " << async_pending_count << std::endl;
    
  }
  void Prepare() { 
    if (!coll) { 
#ifdef EMSCRIPTEN
      std::cout << "P_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2(); }
    if (coll) {
      coll->Prepare();
      return;
    }
    GameApi::P pp;
    pp.id = -1;
    p_data = pp;
    coll = 0;
  }
  virtual int NumFaces() const { if (!coll) return 0; return coll->NumFaces(); }
  virtual int NumPoints(int face) const { if (!coll) return 3; return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { if (!coll) return Point(0.0,0.0,0.0); return coll->FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { if (!coll) return Vector(0.0,0.0,0.0); return coll->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { if (!coll) return 0.0; return coll->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { if (!coll) return 0; return coll->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { if (!coll) return 0xffffffff; return coll->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { if (!coll) { Point2d p; p.x = 0.0; p.y = 0.0; return p; } return coll->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { if (!coll) return 0.0; return coll->TexCoord3(face,point); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  GameApi::P p_data;
  FaceCollection *coll;
  bool async_taken;
};

void P_cb(void *data)
{
  std::cout << "P_cb" << std::endl;
  P_script *script = (P_script*)data;
  script->Prepare2();
}


GameApi::P GameApi::MainLoopApi::load_P_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  return add_polygon2(e, new P_script(e,ev,url, p1,p2,p3,p4,p5),1);
}

std::vector<std::string> parse_sep(std::string s, char sep)
{
  int ss = s.size();
  int pos = 0;
  std::vector<std::string> vec;
  for(int i=0;i<ss;i++)
    {
      if (s[i]==sep) {
	std::string beg = s.substr(pos, i-pos);
	vec.push_back(beg);
	pos = i+1;
      }
    }
  std::string beg = s.substr(pos,s.size()-pos);
  vec.push_back(beg);
  //std::cout << "Parse sep: ";
  //int s2 = vec.size();
  //for(int i=0;i<s2;i++) std::cout << vec[i] << "::";
  //std::cout << std::endl;
  return vec;
}

GameApi::ARR GameApi::MainLoopApi::load_P_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }
     std::stringstream ss;
     ss << i;
     
     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     FaceCollection *coll = new P_script(e,ev,url, k1,k2,k3,k4,k5);
     GameApi::P coll_p = add_polygon2(e, coll,1);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}



void ML_cb(void* data);


class ML_script : public MainLoopItem
{
public:
  ML_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url),p1(p1), p2(p2), p3(p3), p4(p4), p5(p5) , main2(0) { firsttime = true; 
       e.async_load_callback(url, &ML_cb, this); 
#ifdef EMSCRIPTEN
       async_pending_count++; async_taken=true;
#endif
       std::cout << "async_pending_count inc (ML_sctipr) " << async_pending_count << std::endl;
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepage);
#endif
      std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string code(vec->begin(), vec->end());
      code = replace_str(code, "%1", p1);
      code = replace_str(code, "%2", p2);
      code = replace_str(code, "%3", p3);
      code = replace_str(code, "%4", p4);
      code = replace_str(code, "%5", p5);
      GameApi::ExecuteEnv e2;
      std::pair<int,std::string> p = GameApi::execute_codegen(e,ev,code,e2);
      if (p.second=="ML") {
	GameApi::ML pp;
	pp.id = p.first;
	main2 = find_main_loop(e,pp);
	main2->Prepare();
#ifdef EMSCRIPTEN
	if (async_taken)
	  async_pending_count--;
#endif
	std::cout << "async_pending_count dec (ML_sctipr) " << async_pending_count << std::endl;
	async_taken = false;
	//main2->execute(e3);
	//firsttime = false;
	return;
      }
      //GameApi::P pp;
      //pp.id = -1;
      main2 = 0;
#ifdef EMSCRIPTEN
      if (async_taken)
      async_pending_count--;
#endif
      async_taken = false;
       std::cout << "async_pending_count dec (ML_sctipr2) " << async_pending_count << std::endl;

  }
  void Prepare() {}
  virtual void execute(MainLoopEnv &e3)
  {
    if (firsttime) {
      if (!main2) {
#ifdef EMSCRIPTEN
      std::cout << "ML_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2();
      }
      firsttime = false;
    }
    if (main2)
      main2->execute(e3);
  }

  virtual void handle_event(MainLoopEvent &e)
  {
    if (main2) {
      main2->handle_event(e);
    }
  }
  virtual int shader_id() { 
    if (main2) {
      return main2->shader_id();
    }
    return -1;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  bool firsttime;
  MainLoopItem *main2;
  bool async_taken;
};

void ML_cb(void *data)
{
  std::cout << "ML_cb" << std::endl;
  ML_script *script = (ML_script*)data;
  script->Prepare2();
}


GameApi::ML GameApi::MainLoopApi::load_ML_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  return add_main_loop(e, new ML_script(e,ev,url,p1,p2,p3,p4,p5));
}

GameApi::ARR GameApi::MainLoopApi::load_ML_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }

     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     MainLoopItem *coll = new ML_script(e,ev,url, k1,k2,k3,k4,k5);
     GameApi::ML coll_p = add_main_loop(e, coll);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}



void BM_cb(void *data);

class BM_script : public Bitmap<Color>
{
public:
  BM_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), bitmap(0) 
  {
    e.async_load_callback(url, &BM_cb, this);
#ifdef EMSCRIPTEN
    async_pending_count++; async_taken = true;
#endif
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    //std::cout << "BM_scriptA: " << code << std::endl;
    code = replace_str(code, "%1", p1);
    code = replace_str(code, "%2", p2);
    code = replace_str(code, "%3", p3);
    code = replace_str(code, "%4", p4);
    code = replace_str(code, "%5", p5);
    //std::cout << "BM_script: " << code << std::endl;
    GameApi::ExecuteEnv e2;
    std::pair<int, std::string> p = GameApi::execute_codegen(e,ev,code,e2);
    if (p.second=="BM") {
      GameApi::BM bm;
      bm.id = p.first;
      BitmapHandle *handle = find_bitmap(e, bm);
      bitmap = find_color_bitmap(handle);
#ifdef EMSCRIPTEN
      if (async_taken)
      	async_pending_count--;
#endif
      async_taken=false;
      return;
    }
    bitmap=0;
#ifdef EMSCRIPTEN
    if (async_taken)
      async_pending_count--;
#endif
    async_taken=false;

  }
  void Prepare() {
    if (bitmap)
     bitmap->Prepare();
    else {
#ifdef EMSCRIPTEN
      std::cout << "BM_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2();
      if (bitmap)
	bitmap->Prepare();
    }
  }
  virtual int SizeX() const { if (bitmap) return bitmap->SizeX(); return 1; }
  virtual int SizeY() const { if (bitmap) return bitmap->SizeY(); return 1; }
  virtual Color Map(int x, int y) const
  {
    if (bitmap) {
      return bitmap->Map(x,y);
    }
    return Color();
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  Bitmap<Color> *bitmap;
  bool async_taken;
};

void BM_cb(void *data)
{
  std::cout << "BM_cb" << std::endl;

  BM_script *script = (BM_script*)data;
  script->Prepare2();
}


GameApi::BM GameApi::MainLoopApi::load_BM_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  Bitmap<Color> *bm = new BM_script(e,ev,url,p1,p2,p3,p4,p5);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = bm;
  return add_bitmap(e, handle2);
}

GameApi::ARR GameApi::MainLoopApi::load_BM_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }
     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     Bitmap<Color> *coll = new BM_script(e,ev,url, k1,k2,k3,k4,k5);
     BitmapColorHandle *handle2 = new BitmapColorHandle;
     handle2->bm = coll;
     GameApi::BM coll_p = add_bitmap(e, handle2);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}


struct FileHeader
{
  char d;
  char s;
  int type;
  int numblocks;
};
struct FileBlock
{
  int block_type;
  int block_size_in_bytes;
  int block_offset_from_beginning_of_file;
};

class LoadDS : public DiskStore
{
public:
  LoadDS(std::vector<unsigned char> file) : vec(file), valid(false) { }
  void Prepare() {
    header = (FileHeader*)&vec[0];
    blocks = (FileBlock*)&vec[sizeof(FileHeader)];
    wholefile = (unsigned char*)&vec[0];
    valid = header->d=='d' && header->s=='s';
    std::cout << "LoadDS: valid=" << valid << " " << header->d << " " << header->s << std::endl;
  }
  int Type() const { return header->type; }
  int NumBlocks() const { return valid ? header->numblocks : 0; }
  int BlockType(int block) const { return blocks[block].block_type; }
  int BlockSizeInBytes(int block) const { return blocks[block].block_size_in_bytes; }
  unsigned char *Block(int block) const { return wholefile + blocks[block].block_offset_from_beginning_of_file; }
private:
  std::vector<unsigned char> vec;
  FileHeader *header;
  FileBlock *blocks;
  unsigned char *wholefile;
  bool valid;
};

GameApi::DS GameApi::MainLoopApi::load_ds_from_disk(std::string filename)
{
  std::ifstream t(filename,std::ios::in | std::ios::binary);
  std::string str;
  if (t) {
    t.seekg(0,std::ios::end);
    str.resize(t.tellg());
    t.seekg(0,std::ios::beg);
    t.read(&str[0],str.size());
    t.close();
  }
  std::vector<unsigned char> vec(str.begin(),str.end());
  return add_disk_store(e, new LoadDS(vec));
}
GameApi::DS GameApi::MainLoopApi::load_ds_from_mem(std::vector<unsigned char> vec)
{
   return add_disk_store(e, new LoadDS(vec));
}

class SaveDSMain : public MainLoopItem
{
public:
  SaveDSMain(GameApi::EveryApi &ev, std::string out_file, GameApi::P p) : ev(ev), out_file(out_file), p(p) { firsttime = true; }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      std::cout << "Saving to " << out_file << std::endl;
      GameApi::DS ds = ev.polygon_api.p_ds_inv(p);
      ev.mainloop_api.save_ds(out_file, ds);
      firsttime = false;
    }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual int shader_id() { return -1; }
private:
  GameApi::EveryApi &ev;
  std::string out_file;
  bool firsttime;
  GameApi::P p;
};

GameApi::ML GameApi::MainLoopApi::save_ds_ml(GameApi::EveryApi &ev, std::string output_filename, P p)
{
  return add_main_loop(e, new SaveDSMain(ev,output_filename, p));
}

void GameApi::MainLoopApi::save_ds(std::string output_filename, DS ds)
{
  std::ofstream ff(output_filename, std::ios::out | std::ios::binary);

  int offset = 0;
  DiskStore *dds = find_disk_store(e, ds);
  dds->Prepare();
  FileHeader h;
  h.d = 'd';
  h.s = 's';
  h.type = dds->Type();
  h.numblocks = dds->NumBlocks();
  std::cout << "Writing Header:" << sizeof(FileHeader) << std::endl;
  ff.write((char*)&h, (int)sizeof(FileHeader));
  offset += sizeof(FileHeader);
  int s = h.numblocks;
  offset += sizeof(FileBlock)*s;
  for(int i=0;i<s;i++)
    {
      FileBlock b;
      b.block_type = dds->BlockType(i);
      b.block_size_in_bytes = dds->BlockSizeInBytes(i);
      b.block_offset_from_beginning_of_file = offset;
      offset += b.block_size_in_bytes;
      std::cout << "Writing FileBlock:" << sizeof(FileBlock) << std::endl;
      ff.write((char*)&b,(int)sizeof(FileBlock));
    }
  for(int i=0;i<s;i++)
    {
      int s = dds->BlockSizeInBytes(i);
      unsigned char *ptr = dds->Block(i);
      std::cout << "Writing Block:" << s << std::endl;
      ff.write((char*)ptr, s);
    }
  std::cout << "Closing file" << std::endl;
  ff.close();
}

class RandomIntBitmap : public Bitmap<int>
{
public:
  RandomIntBitmap(int sx, int sy, int min_value, int max_value) : sx(sx), sy(sy), min_value(min_value), max_value(max_value) {}
  void Prepare() { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  int Map(int x, int y) const {
    Random r;
    float val = double(r.next())/r.maximum();
    val*=(max_value-min_value);
    val+=min_value;
    return int(val);
  }
private:
  int sx,sy;
  int min_value, max_value;
};

GameApi::IBM GameApi::BitmapApi::random_int_bitmap(int sx, int sy, int min_value, int max_value)
{
  return add_int_bitmap(e,new RandomIntBitmap(sx,sy,min_value,max_value));
}

class NormalMove : public InputForMoving
{
public:
  NormalMove() : up(false), down(false), left(false), right(false),x(0.0), y(0.0) { }
  void execute(MainLoopEnv &e) { calc(); }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }

  }
  float XPos() const { 
    return x;
  }

  float YPos() const {
    return y;
  }
  float ZPos() const { return 0.0; }
  void calc() {
    x=0.0;
    y=0.0;
    if (up && !down && !left && !right) y=1.0;
    if (!up && down && !left && !right) y=-1.0;

    if (!up && !down && left && !right) x=-1.0;
    if (!up && !down && !left && right) x=1.0;

    if (up && !down && left && !right) { y=1.0/sqrt(2.0); x=-1.0/sqrt(2.0); }
    if (!up && down && left && !right) { y=-1.0/sqrt(2.0); x=-1.0/sqrt(2.0); }

    if (up && !down && !left && right) { y=1.0/sqrt(2.0); x=+1.0/sqrt(2.0); }
    if (!up && down && !left && right) { y=-1.0/sqrt(2.0); x=+1.0/sqrt(2.0); }
  }
private:
  bool up;
  bool down;
  bool left;
  bool right;
  float x;
  float y;
};

GameApi::INP GameApi::MainLoopApi::cursor_keys_normal_move()
{
  return add_move_input(e, new NormalMove);
}

class MoveSequence : public InputForMoving
{
public:
  MoveSequence(float time_per_char, std::string str, float start_time) : time(time_per_char), str(str), start_time(start_time) { 
  }
  void execute(MainLoopEnv &env)
  {
    float t = (env.time*1000.0-start_time)/100.0;
    int val = int(t/time);
    int s = str.size();
    x=0.0;
    y=0.0;
    z=0.0;
    if (val>=0 && val<s) {
      char ch = str[val];
      if (ch=='w') { y=-1.0; }
      if (ch=='s') { y=1.0; }
      if (ch=='a') { x=-1.0; }
      if (ch=='d') { x=1.0; }
      if (ch=='z') { z=-1.0; }
      if (ch=='x') { z=1.0; }
    }
  }  
  void handle_event(MainLoopEvent &e)
  {
  }
  float XPos() const { 
    return x;
  }

  float YPos() const {
    return y;
  }
  float ZPos() const { return z; }
private:
  float time;
  std::string str;
  float start_time;
  float x,y,z;
};

class RotateMove : public InputForMoving
{
public:
  RotateMove(float speed_rot, float r_forward, float r_normal, float r_backward) : speed_rot(speed_rot), r_forward(r_forward), r_normal(r_normal), r_backward(r_backward) { }
  void execute(MainLoopEnv &e) { 
    calc();
  }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }
  }
  void calc() { 
    if (left) current_angle-=speed_rot;
    if (right) current_angle+=speed_rot;
    if (up) current_radius=r_forward; 
    else
      if (down) current_radius=r_backward; 
      else
	current_radius=r_normal;
  }
  float XPos() const { 
    return current_radius*cos(current_angle);
  }

  float YPos() const {
    return current_radius*sin(current_angle);
  }
  float ZPos() const { return 0.0; }

private:
  float speed_rot, r_forward, r_normal, r_backward;
  float current_angle;
  float current_radius;
  bool up,down,left,right;
};

GameApi::INP GameApi::MainLoopApi::cursor_keys_rotate_move(float speed_rot, float r_forward, float r_normal, float r_backward)
{
  return add_move_input(e, new RotateMove(speed_rot, r_forward, r_normal, r_backward));
}
GameApi::INP GameApi::MainLoopApi::move_sequence(float start_time, float time_per_char, std::string str)
{
  return add_move_input(e, new MoveSequence(time_per_char, str, start_time));
}

class CollisionMove : public InputForMoving
{
public:
  CollisionMove(Bitmap<int> *scene, float start_x, float end_x, float start_y, float end_y, float s_x, float m_x, float e_x, float s_y, float m_y, float e_y, float speed_up, float speed_down, float speed_left, float speed_right, float speed_gravity) : scene(scene), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y),up(false),down(false),left(false), right(false),s_x(s_x), m_x(m_x), e_x(e_x), s_y(s_y), m_y(m_y), e_y(e_y), speed_up(speed_up), speed_down(speed_down), speed_left(speed_left), speed_right(speed_right), speed_gravity(speed_gravity) { firsttime = true; xpos=0.0; ypos=0.0; zpos=0.0; up=false; down=false; left=false; right=false; }
  void execute(MainLoopEnv &e) { 
    if (firsttime) { firsttime = false; scene->Prepare(); }
    Point p(xpos+e_x,ypos+m_y,zpos);
    Point2d pp = to_bm(p);
    int val = scene->Map(int(pp.x),int(pp.y));
    if (right && val==0) { xpos+=speed_right*e.delta_time; }

    Point p2(xpos+s_x,ypos+m_y,zpos);
    Point2d pp2 = to_bm(p2);
    int val2 = scene->Map(int(pp2.x),int(pp2.y));
    if (left && val2==0) { xpos+=speed_left*e.delta_time; }

    Point p3(xpos+m_x,ypos+s_y,zpos);
    Point2d pp3 = to_bm(p3);
    int val3 = scene->Map(int(pp3.x),int(pp3.y));
    if (up && val3==0) { ypos+=speed_up*e.delta_time; }

    Point p4(xpos+m_x,ypos+e_y,zpos);
    Point2d pp4 = to_bm(p4);
    int val4 = scene->Map(int(pp4.x),int(pp4.y));
    if (down && val4==0) { ypos+=speed_down*e.delta_time; }
    if (val4==0) { ypos+=speed_gravity*e.delta_time; }
  }
  Point2d to_bm(Point p) const
  {
    p-=Vector(start_x,start_y,0.0);
    p.x /= (end_x-start_x);
    p.y /= (end_y-start_y);
    if (p.x<0.0 || p.x>1.0) { } else
      if (p.y<0.0 || p.y>1.0) { } else
	{
	  p.x *= scene->SizeX();
	  p.y *= scene->SizeY();
	}
    Point2d pp = { p.x,p.y };
    return pp;
  }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }

  }
  float XPos() const {
    return xpos;
  }
  float YPos() const {
    return ypos;
  }
  float ZPos() const
  {
    return zpos;
  }

private:
  Bitmap<int> *scene;
  float start_x, end_x;
  float start_y, end_y;
  bool up,down,left,right;
  float xpos,ypos,zpos;
  float s_x,m_x,e_x,s_y,m_y,e_y;
  float speed_up, speed_down, speed_left, speed_right;
  bool firsttime;
  float speed_gravity;
};

class MoveSpeed : public InputForMoving
{
public:
  MoveSpeed(InputForMoving *next, float speed) : next(next), speed(speed),x(0.0),y(0.0),z(0.0) { }
  void execute(MainLoopEnv &e) { 
    next->execute(e);
    x+=speed*next->XPos()*e.delta_time;
    y+=speed*next->YPos()*e.delta_time;
    z+=speed*next->ZPos()*e.delta_time;
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  float XPos() const {
    return x;
  }
  float YPos() const {
    return y;
  }
  float ZPos() const
  {
    return z;
  }
private:
  InputForMoving *next;
  float speed;
  float x,y,z;
};
GameApi::INP GameApi::MainLoopApi::move_collision(IBM scene, float start_x, float end_x, float start_y, float end_y, float s_x, float m_x, float e_x, float s_y, float m_y, float e_y, float speed_up, float speed_down, float speed_left, float speed_right, float speed_gravity)
{
  Bitmap<int> *bm = find_int_bitmap(e, scene);
  return add_move_input(e, new CollisionMove(bm, start_x, end_x, start_y, end_y, s_x, m_x, e_x, s_y, m_y, e_y, speed_up, speed_down, speed_left, speed_right, speed_gravity));
}
GameApi::INP GameApi::MainLoopApi::move_speed(INP orig, float speed)
{
  InputForMoving *o = find_move_input(e, orig);
  return add_move_input(e, new MoveSpeed(o, speed));
};


class MoveIN : public MainLoopItem
{
public:
  MoveIN(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *item, InputForMoving *in) : e(e), ev(ev), item(item), in(in) { }
  void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &env)
  {
    GameApi::M mat = ev.matrix_api.trans(in->XPos(), in->YPos(), in->ZPos());
    GameApi::M m2 = add_matrix2(e,env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
 
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s11;
    s11.id = env.sh_texture_2d;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
 
   ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);
 

    MainLoopEnv ee = env;
    ee.in_MV = find_matrix(e, mat2);
    ee.env = find_matrix(e,mat2);
    item->execute(ee);
    in->execute(ee);
    ev.shader_api.unuse(s3);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
    in->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  InputForMoving *in;
};
GameApi::ML GameApi::MainLoopApi::move_in(EveryApi &ev, ML ml, INP in)
{
  MainLoopItem *item = find_main_loop(e,ml);
  InputForMoving *in2 = find_move_input(e,in);
  return add_main_loop(e, new MoveIN(e,ev,item, in2));
}

class Gravity2 : public InputForMoving
{
public:
  Gravity2(InputForMoving *mv, Bitmap<int> *bm, float start_x, float end_x, float start_y, float end_y, float speed) : mv(mv), bm(bm), speed(speed), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { ypos=0.0; firsttime = true; }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
      bm->Prepare();
    }
    Point p(mv->XPos(), mv->YPos()+ypos, mv->ZPos());
    p-=Vector(start_x,start_y,0.0);
    p.x /= (end_x-start_x);
    p.y /= (end_y-start_y);
    if (p.x<0.0 || p.x>1.0) { } else
      if (p.y<0.0 || p.y>1.0) { } else
	{
	  p.x *= bm->SizeX();
	  p.y *= bm->SizeY();
	  int val = bm->Map(int(p.x),int(p.y));
	  if (val==0) { ypos+=speed*e.delta_time; }
	}
    mv->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { mv->handle_event(e); }
  virtual float XPos() const { return mv->XPos(); }
  virtual float YPos() const { return mv->YPos()+ypos; }
  virtual float ZPos() const { return mv->ZPos(); }
private:
  InputForMoving *mv;
  Bitmap<int> *bm;
  float speed;
  float start_x, end_x;
  float start_y, end_y;
  float ypos;
  bool firsttime;
};

GameApi::INP GameApi::MainLoopApi::gravity(INP im, IBM bitmap, float start_x, float end_x, float start_y, float end_y, float speed)
{
  InputForMoving *mv = find_move_input(e, im);
  Bitmap<int> *bm = find_int_bitmap(e, bitmap);
  return add_move_input(e, new Gravity2(mv, bm, start_x, end_x, start_y, end_y, speed));
}


std::vector<float> dyn_points_global_x;
std::vector<float> dyn_points_global_y;
std::vector<float> dyn_points_global_z;


class DynPoints : public MainLoopItem
{
public:
  DynPoints(GameApi::Env &ee,GameApi::EveryApi &ev, MainLoopItem *next, Movement *mn, int pointnum, Point start_pos) : ee(ee), ev(ev), next(next), mn(mn), pointnum(pointnum), start_pos(start_pos) {
    start_time = 0.0; //ev.mainloop_api.get_time();
    firsttime = true;
    firsttime2 = false;
  }
  void reset_time()
  {
    start_time = ev.mainloop_api.get_time();
  }

  void Prepare() {
    next->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (!mn ||!next) return;
    MainLoopEnv ee = e; 
   mn->frame(ee);
    if (firsttime) {
      firsttime2 = true;
    }
    if (!firsttime && firsttime2)
      {
	reset_time();
	firsttime2 = false;
      }
    firsttime = false;

    float time = (e.time*1000.0-start_time) / 100.0;
    Matrix m = mn->get_whole_matrix(time,ev.mainloop_api.get_delta_time());
    Point pp = start_pos*m*e.env;
    int s = dyn_points_global_x.size();
    while (pointnum>s-1) { 
      dyn_points_global_x.push_back(pp.x); 
      dyn_points_global_y.push_back(pp.y); 
      dyn_points_global_z.push_back(pp.z); 
      s=dyn_points_global_x.size(); 
    }
    if (pointnum>=0) {
      dyn_points_global_x[pointnum] = pp.x;
      dyn_points_global_y[pointnum] = pp.y;
      dyn_points_global_z[pointnum] = pp.z;
    }
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
    mn->event(e);
  }
  virtual int shader_id() { return next?next->shader_id():-1; }

private:
  GameApi::Env &ee;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Movement *mn;
  int pointnum;
  bool firsttime;
  bool firsttime2;
  float start_time;
  Point start_pos;
};

GameApi::ML GameApi::MainLoopApi::dyn_points(EveryApi &ev, ML ml, MN move, int pointnum, float pos_x, float pos_y, float pos_z)
{
  MainLoopItem *ma = find_main_loop(e,ml);
  Movement *mn = find_move(e, move);
  Point pos(pos_x,pos_y,pos_z);
  return add_main_loop(e, new DynPoints(e,ev, ma, mn, pointnum, pos));
}
extern std::map<std::string, int> number_map;
int score=0; // remember to add to number_map["score"]
int hidden_score=0;

class ScoreAdder : public MainLoopItem
{
public:
  ScoreAdder(GameApi::EveryApi &ev,MainLoopItem *item, VolumeObject *obj, Movement *move, int enter_score, int leave_score, int dyn_point, float timeout) : ev(ev), item(item), obj(obj), move(move), enter_score(enter_score), leave_score(leave_score), dyn_point(dyn_point),timeout(timeout) {
    prev_p = false;
    prev_time = 0.0;
    prev_time2 = 0.0;
}

  void enter_event(float time) {
    if (time>prev_time+timeout) {
      prev_time = time;
      score+=enter_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  void leave_event(float time) {
    if (time>prev_time2+timeout) {
      prev_time2 = time;
      score+=leave_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    Point p = Point(0, 0, -400);
    //Matrix m = Matrix::YRotation(-quake_rot_y);
    //p = p * m;
    //p.x*=1.2;
    //p.y*=1.2;
    //p.z*=1.2;
    //Matrix m2 = Matrix::YRotation(-quake_rot_y);
    //p = p * m2;
    p.x-=quake_pos_x;
    p.z-=quake_pos_y;

    debug_pos_x = p.x;
    debug_pos_y = p.y;
    debug_pos_z = p.z;

    int s = dyn_points_global_x.size();
    if (dyn_point>=0 && dyn_point<s)
      {
	p = Point(dyn_points_global_x[dyn_point],dyn_points_global_y[dyn_point],dyn_points_global_z[dyn_point]);
      }
    p = p * Matrix::Inverse(move->get_whole_matrix(e.time*10.0, ev.mainloop_api.get_delta_time()));


    bool b = obj->Inside(p);
    if (b && b!=prev_p) { prev_p = b; enter_event(e.time*10.0); } else
    if (!b && b!=prev_p) { prev_p = b; leave_event(e.time*10.0); }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  VolumeObject *obj;
  Movement *move;
  int enter_score, leave_score;
  int dyn_point;
  float timeout;
  bool prev_p;
  float prev_time, prev_time2;
};

class ScoreAdder2 : public MainLoopItem
{
public:
  ScoreAdder2(GameApi::EveryApi &ev,MainLoopItem *item, VolumeObject *obj, Movement *move, PointsApiPoints *pts, int enter_score, int leave_score, int dyn_point, float timeout, Fetcher<int> *fetcher) : ev(ev), item(item), obj(obj), move(move), pts(pts), enter_score(enter_score), leave_score(leave_score), dyn_point(dyn_point),timeout(timeout), fetcher(fetcher) {
    prev_p = false;
    prev_time = 0.0;
    prev_time2 = 0.0;
}

  void enter_event(int i, float time) {
    if (time>prev_time+timeout) {
      prev_time = time;
      score+=enter_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  void leave_event(int i, float time) {
    if (time>prev_time2+timeout) {
      prev_time2 = time;
      score+=leave_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  void Prepare() { item->Prepare(); pts->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    Point p = Point(0, 0, 0); // z=-400
    //Matrix m = Matrix::YRotation(-quake_rot_y);
    //p = p * m;
    //p.x*=1.2;
    //p.y*=1.2;
    //p.z*=1.2;
    //Matrix m2 = Matrix::YRotation(-quake_rot_y);
    //p = p * m2;
    p.x-=quake_pos_x;
    p.z-=quake_pos_y;


    int s = dyn_points_global_x.size();
    if (dyn_point>=0 && dyn_point<s)
      {
	p = Point(dyn_points_global_x[dyn_point],dyn_points_global_y[dyn_point],dyn_points_global_z[dyn_point]);
      }
    p = p * Matrix::Inverse(move->get_whole_matrix(e.time*10.0, ev.mainloop_api.get_delta_time()));

	debug_pos_x = p.x;
	debug_pos_y = p.y;
	debug_pos_z = p.z;


    int sk = pts->NumPoints();
    for(int i=0;i<sk;i++) {
      Point pp = p;
      Point p2 = pts->Pos(i);
      p2.x = -p2.x;
      pp-=Vector(p2);
	
      bool b = obj->Inside(pp);
      if (b) {

	//std::cout << "ScoreAdder hit: " << i << " " << p << " " << pp << " " << pts->Pos(i) << std::endl;
	fetcher->set(i);
      }
      if (b && b!=prev_p) { prev_p = b; enter_event(i,e.time*10.0); } else
	if (!b && b!=prev_p) { prev_p = b; leave_event(i,e.time*10.0); }
    }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  VolumeObject *obj;
  Movement *move;
  PointsApiPoints *pts;
  int enter_score, leave_score;
  int dyn_point;
  float timeout;
  bool prev_p;
  float prev_time, prev_time2;
  Fetcher<int> *fetcher;
};


float debug_pos_x=0.0, debug_pos_y=0.0, debug_pos_z=0.0;

GameApi::ML GameApi::MainLoopApi::debug_obj(EveryApi &ev)
{  
#if 0
  P p = ev.polygon_api.cube(-30.0,30.0, -30.0, 30.0, -30.0,30.0);
  MT mat0 = ev.materials_api.def(ev);
  MT mat = ev.materials_api.snow(ev,mat0,0xffaaaaaa, 0xffeeeeee, 0xffffffff, 0.95);
  ML ml1 = ev.materials_api.bind(p,mat);
  MN mn2 = ev.move_api.empty();
  MN mn = ev.move_api.debug_translate(mn2);
  ML ml2 = ev.move_api.move_ml(ev,ml1,mn,1,10.0);
#endif
  P p = ev.polygon_api.empty();
  MT mat0 = ev.materials_api.def(ev);
  
  ML ml2 = ev.materials_api.bind(p,mat0);
  return ml2;
}

GameApi::ML GameApi::MainLoopApi::score_adder(EveryApi &ev,ML ml, O o, MN transform, int enter_score, int leave_score, int dyn_point, float timeout)
{
  MainLoopItem *item = find_main_loop(e,ml);
  VolumeObject *obj = find_volume(e, o);
  Movement *mv = find_move(e, transform);
  return add_main_loop(e, new ScoreAdder(ev,item, obj, mv, enter_score, leave_score, dyn_point,timeout));
}

GameApi::ML GameApi::MainLoopApi::score_adder2(EveryApi &ev,ML ml, O o, PTS points, MN transform, int enter_score, int leave_score, int dyn_point, float timeout, IF int_fetcher)
{
  MainLoopItem *item = find_main_loop(e,ml);
  VolumeObject *obj = find_volume(e, o);
  Movement *mv = find_move(e, transform);
  Fetcher<int> *fetcher = find_int_fetcher(e, int_fetcher);
  PointsApiPoints *points2 = find_pointsapi_points(e, points);
  return add_main_loop(e, new ScoreAdder2(ev,item, obj, mv, points2, enter_score, leave_score, dyn_point,timeout, fetcher));
}

std::vector<int> g_hide_container;

class ScoreHidePTS : public PointsApiPoints
{
public:
  ScoreHidePTS(PointsApiPoints *pts) : pts(pts) { }
  virtual void Prepare() { pts->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { 
    pts->HandleEvent(event);
  }
  virtual bool Update(MainLoopEnv &e) {
    if (g_hide_container.size()!=hidecount) {
      hidecount = g_hide_container.size();
      bool b = pts->Update(e);
      return true; 
    }
    bool b = pts->Update(e);
    return true;
    return b;
  }
  virtual int NumPoints() const { return pts->NumPoints(); }
  virtual Point Pos(int t) const
  {
    int s = g_hide_container.size();
    for(int i=0;i<s;i++) {
      if (g_hide_container[i]==t) { /*std::cout << "Hiding: " << i << " " << t << std::endl;*/ return Point(0.0,-444440.0,0.0); }
    }
    return pts->Pos(t);
  }
  virtual unsigned int Color(int i) const { return pts->Color(i); }
private:
  PointsApiPoints *pts;
  int hidecount;
};
class ScoreSetHide : public Fetcher<int>
{
public:
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void set(int t) { 
    //std::cout << "ScoreSetHide: " << t << std::endl;
    int s = g_hide_container.size();
    for(int i=0;i<s;i++) {
      if (g_hide_container[i]==t) return;
    }
    g_hide_container.push_back(t);
  }
  virtual int get() const { return 0; }

};
GameApi::PTS GameApi::MainLoopApi::score_hide_pts(PTS pts)
{
  PointsApiPoints *pts2 = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new ScoreHidePTS(pts2));
}
GameApi::IF GameApi::MainLoopApi::score_set_hide()
{
  return add_int_fetcher(e, new ScoreSetHide);
}

class ScoreHidder : public MainLoopItem
{
public:
  ScoreHidder(GameApi::EveryApi &ev, MainLoopItem *item, VolumeObject *obj, int max_count) : ev(ev), item(item), obj(obj), max_count(max_count) { prev_p=false; }

  void enter_event(float time) {
      int transfer = score>max_count?max_count:score;
      if (transfer<0) transfer=0;
      hidden_score+=transfer;
      score-=transfer;
      number_map["score"] = score;
  }
  void leave_event(float time) {
      int transfer = score>max_count?max_count:score;
      if (transfer<0) transfer=0;
      hidden_score+=transfer;
      score-= transfer;
      number_map["score"] = score;
  }

  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
  void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    Point p = Point(0, 0, -400);
    p.x-=quake_pos_x;
    p.z-=quake_pos_y;

    bool b = obj->Inside(p);
    if (b && b!=prev_p) { prev_p = b; enter_event(e.time*10.0); } else
    if (!b && b!=prev_p) { prev_p = b; leave_event(e.time*10.0); }
    item->execute(e);
  }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  VolumeObject *obj;
  int max_count;
  bool prev_p;
};

GameApi::ML GameApi::MainLoopApi::score_hidder(EveryApi &ev, ML ml, O o, int max_count)
{
  MainLoopItem *item = find_main_loop(e,ml);
  VolumeObject *obj = find_volume(e, o);
  return add_main_loop(e, new ScoreHidder(ev,item,obj,max_count));
}

GameApi::MT GameApi::MaterialsApi::noise(EveryApi &ev, int sx, int sy, int r, int g, int b, int a,
					 int r2, int g2, int b2, int a2, float mix)
{
  BM I1=ev.bitmap_api.noise_vectors(sx,sy);
  FB I2=ev.float_bitmap_api.from_red(I1);
  BM I3=ev.float_bitmap_api.to_grayscale_color(I2,r,g,b,a,r2,g2,b2,a2);
  MT I4=ev.materials_api.texture_many(ev,std::vector<BM>{I3},mix);
  return I4;
}

class LinesCurveGroup : public CurveGroup
{
public:
  LinesCurveGroup(LineCollection *coll) : coll(coll) { }
  void Prepare() { coll->Prepare(); }
  int NumCurves() const { return coll->NumLines(); }
  Point Pos(int num, float t) const
  {
    Point p1 = coll->LinePoint(num, 0);
    Point p2 = coll->LinePoint(num, 1);
    LineIn3d line(p1,p2);
    return line.Index(t);
  }
  unsigned int Color(int num, float t) const
  {
    unsigned int c1 = coll->LineColor(num,0);
    unsigned int c2 = coll->LineColor(num,1);
    return Color::Interpolate(c1,c2,t);
  }
  int Shape(int num, float t) const { return 0; }
private:
  LineCollection *coll;
};

GameApi::CG GameApi::CurveApi::curve_group_from_lines(LI li)
{
  LineCollection *coll = find_line_array(e, li);
  return add_curve_group(e, new LinesCurveGroup(coll));
}

class LinesFromQuads : public LineCollection
{
public:
  LinesFromQuads(FaceCollection *coll, int sx, int sy) : coll(coll), sx(sx), sy(sy) { }
  void Prepare() { coll->Prepare(); }
  int NumLines() const { return coll->NumFaces()*(sx+sy+1); }
  Point LinePoint(int line, int point) const
  {
    int s = coll->NumFaces();
    int a1_r = line/s;
    int a1 = line-a1_r*s;
    int a2 = a1_r;
    bool b = false;
    if (a2>=sx) { b=true; a2-=sx; } 
    float t = float(a2);
    if (b) { t/=float(sy); } else { t/=float(sx); }

    if (!b) {
      if (point==0) {
	Point p1 = coll->FacePoint(a1,0);
	Point p2 = coll->FacePoint(a1,1);
	LineIn3d line(p1,p2);
	Point pp1 = line.Index(t);
	return pp1;
      } else {
	Point p3 = coll->FacePoint(a1,2);
	Point p4 = coll->FacePoint(a1,3%coll->NumPoints(a1));
	LineIn3d line2(p4,p3);
	Point pp2 = line2.Index(t);
	return pp2; 
      }
    } else {
      if (point==0) {
	Point p1 = coll->FacePoint(a1,0);
	Point p4 = coll->FacePoint(a1,3%coll->NumPoints(a1));
	LineIn3d line(p1,p4);
	Point pp1 = line.Index(t);
	return pp1;
      } else {
	Point p2 = coll->FacePoint(a1,1);
	Point p3 = coll->FacePoint(a1,2);
	LineIn3d line2(p2,p3);
	Point pp2 = line2.Index(t);
	return pp2;
      }
    }
  }
  unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  FaceCollection *coll;
  int sx;
  int sy;
};

GameApi::LI GameApi::LinesApi::lines_from_quads(P p, int sx, int sy)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_line_array(e, new LinesFromQuads(coll,sx,sy));
}

class AnimCurveGroup : public CurveGroup
{
public:
  AnimCurveGroup(MeshAnim *ma, float start_time, float end_time) : ma(ma), start_time(start_time), end_time(end_time) { }
  void Prepare() { ma->Prepare(); numfaces = ma->NumFaces(); }
  int NumCurves() const { return numfaces*ma->NumPoints(0); }
  Point Pos(int num, float t) const
  {
    int s = ma->NumFaces();
    int p = num/s;
    int p0 = num - p*s;
    return ma->Vertex(p0,p,(1.0-t)*start_time + t*end_time);
  }
  unsigned int Color(int num, float t) const
  {
    int s = ma->NumFaces();
    int p = num/s;
    int p0 = num - p*s;
    return ma->Color(p0,p,(1.0-t)*start_time + t*end_time);
  }
  int Shape(int num, float t) const { return 0; }
private:
  MeshAnim *ma;
  int numfaces;
  float start_time, end_time;
};
GameApi::CG GameApi::PolygonApi::curve_group_from_anim(MA ma, float start_time, float end_time)
{
  MeshAnim *maa = find_mesh_anim(e, ma);
  return add_curve_group(e, new AnimCurveGroup(maa, start_time, end_time));
}

class LineFromCurveGroup : public LineCollection
{
public:
  LineFromCurveGroup(CurveGroup *cg, int split) : cg(cg), split(split) {}
  void Prepare() { cg->Prepare(); }
  int NumLines() const { return split*cg->NumCurves(); }
  Point LinePoint(int line, int point) const
  {
    int k = line/split;
    int k2 = line-k*split;
    float t = float(k2)/float(split);
    float t0 = float(1.0)/float(split);
    switch(point) { 
    case 0: return cg->Pos(k, t);
    case 1: return cg->Pos(k, t+t0);
    }
    Point p(0.0,0.0,0.0);
    return p;
  }
  unsigned int LineColor(int line, int point) const
  {
    int k = line/split;
    int k2 = line-k*split;
    float t = float(k2)/float(split);
    float t0 = float(1.0)/float(split);
    switch(point) {
    case 0: return cg->Color(k,t);
    case 1: return cg->Color(k,t+t0);
      };
    return 0xffffffff;
  }
private:
  CurveGroup *cg;
  int split;
};

GameApi::LI GameApi::CurveApi::lines_from_curve_group(CG curvegroup, int split)
{
  CurveGroup *cg = find_curve_group(e, curvegroup);
  return add_line_array(e, new LineFromCurveGroup(cg, split));
}

class FromImplicitVoxel : public Voxel<int>
{
public:
  FromImplicitVoxel(ImplicitFunction3d *im, int sx, int sy, int sz,
		    float start_x, float end_x,
		    float start_y, float end_y,
		    float start_z, float end_z, int value) : im(im),sx(sx),sy(sy),sz(sz),
						  start_x(start_x), end_x(end_x),
start_y(start_y), end_y(end_y),
							     start_z(start_z), end_z(end_z), value(value)
  { }
  virtual void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    float xx = float(x)/float(sx)*(end_x-start_x)+start_x;
    float yy = float(y)/float(sy)*(end_y-start_y)+start_y;
    float zz = float(z)/float(sz)*(end_z-start_z)+start_z;
    float low_val = im->f_l(xx,yy);
    float high_val = im->f_u(xx,yy);
    if (zz > low_val && zz < high_val) return value;
    return -1;
  }
private:
  ImplicitFunction3d *im;
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int value;
};

GameApi::VX GameApi::VoxelApi::from_implicit(IM i, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value)
{
  ImplicitFunction3d *im = find_implicit(e,i);
  return add_int_voxel(e, new FromImplicitVoxel(im, sx,sy,sz, start_x, end_x, start_y, end_y, start_z, end_z, value));
}



SourceBitmap::SourceBitmap(DrawBufferFormat fmt, int depth) : m_data(0), fmt(fmt), m_width(1), m_height(1), m_ydelta(1), m_depth(depth),m_owned(false),m_owned2(false) 
  {
  }
SourceBitmap::SourceBitmap(const SourceBitmap &src)  
{ 
  if (src.m_owned && src.fmt==D_Mono1 && src.m_data) {
    unsigned char *buf = (unsigned char*)src.m_data;
    unsigned char *buf2 = new unsigned char[src.m_width*src.m_height/8];
    std::memcpy(buf2,buf,src.m_width*src.m_height/8);
    m_data = buf2;
  }
  if (src.m_owned2 && src.fmt==D_RGBA8888 && src.m_data) {
    unsigned int *buf = (unsigned int*)src.m_data;
    unsigned int *buf2 = new unsigned int[src.m_width*src.m_height];
    std::memcpy(buf2,buf,src.m_width*src.m_height*sizeof(unsigned int));
    m_data = buf2;
  }
  if (!src.m_owned && src.fmt==D_Mono1) { m_data = src.m_data; }
  if (!src.m_owned2 && src.fmt==D_RGBA8888) { m_data=src.m_data; }
  m_width = src.m_width;
  m_height = src.m_height;
  m_ydelta = src.m_ydelta;
  m_depth = src.m_depth;
  m_owned = src.m_owned;
  m_owned2 = src.m_owned2;
  fmt = src.fmt;
}
void SourceBitmap::operator=(const SourceBitmap &src)
{
  if (src.m_owned && src.fmt==D_Mono1 && src.m_data) {
    unsigned char *buf = (unsigned char*)src.m_data;
    unsigned char *buf2 = new unsigned char[src.m_width*src.m_height/8];
    std::memcpy(buf2,buf,src.m_width*src.m_height/8);
    m_data = buf2;
  }
  if (src.m_owned2 && src.fmt==D_RGBA8888 && src.m_data) {
    unsigned int *buf = (unsigned int*)src.m_data;
    unsigned int *buf2 = new unsigned int[src.m_width*src.m_height];
    std::memcpy(buf2,buf,src.m_width*src.m_height*sizeof(unsigned int));
    m_data = buf2;
  }
  if (!src.m_owned && src.fmt==D_Mono1) { m_data = src.m_data; }
  if (!src.m_owned2 && src.fmt==D_RGBA8888) { m_data=src.m_data; }
  m_width = src.m_width;
  m_height = src.m_height;
  m_ydelta = src.m_ydelta;
  m_depth = src.m_depth;
  m_owned = src.m_owned;
  m_owned2 = src.m_owned2;
  fmt = src.fmt;
}
  void SourceBitmap::set_data(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 1 " << std::endl;
    unsigned int *buf = new unsigned int[width*height];
    std::memset(buf, 0, width*height);
    m_data = buf;
    std::memcpy(buf, data, width*height*sizeof(unsigned int));
    m_width=width;
    m_height=height;
    m_ydelta=width;
    m_owned2 = true;
  }
  void SourceBitmap::set_data_mono1(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 2 " << std::endl;
    unsigned char *buf = new unsigned char[width*height/8];
    std::memset(buf, 0, width*height/8);
    m_data = buf;
    m_width=width;
    m_height=height;
    m_ydelta =width/8;
    for(int y=0;y<height;y++)
      for(int x=0;x<width;x++)
	{
	  unsigned int color = ((unsigned int*)data)[x+y*ydelta];
	  bool b = false;
	  //if ((color & 0xff000000)>0x80000000) b=true;
	  if ((color & 0x00ff0000)>0x00800000) b=true;
	  if ((color & 0x0000ff00)>0x00008000) b=true;
	  if ((color & 0x000000ff)>0x00000080) b=true;
	  if (b)
	    buf[x/8+y*m_ydelta] |= 1 << (7-(x%8));
	}
    m_owned = true;
  }
  SourceBitmap::~SourceBitmap() { 
    if (m_owned) delete[]((unsigned char*)m_data); 
    if (m_owned2) delete[]((unsigned int*)m_data);
  }



void ClearDepthBuffer(float *arr, int width, int height)
{
  float val = std::numeric_limits<float>::max();
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	*arr++ = val;
      }
}


void CopyFrameToSurface(FrameBuffer *buf, Low_SDL_Surface *surf)
{
  g_low->sdl->SDL_LockSurface(surf);
  int width = buf->Width();
  int height = buf->Height();
  //int depth = buf->Depth();
  void *buffer = buf->Buffer();
  FrameBufferFormat f = buf->format();
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	// GETPIXEL
	unsigned int val = 0xffff00ff;
#if 1
	switch(f) {
	case FrameBufferFormat::F_Mono1:
	  {
	  int byte = (y*width + x)/8;
	  int bit = (y*width+x)&0x7;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  v>>=7-bit;
	  v&=1;
	  val = v?0xffffffff:0xff000000;
	  //std::cout << "F_Mono1 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_Mono8:
	  {
	  int byte = y*width + x;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  unsigned int vv = v;
	  vv <<= 8;
	  unsigned int vvv = v;
	  vvv <<= 16;
	  val = 0xff000000 + v + vv + vvv;
	  //std::cout << "F_Mono8 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_RGB565:
	  //std::cout << "F_RGB565 not supported\n" << std::endl;
	  {
	  unsigned short *ptr = ((unsigned short*)buffer) + y*width+x;
	  unsigned short v = *ptr;
	  unsigned int r = v&0x001f;
	  float rv = r/16.0;
	  rv*=255;
	  unsigned int g = v&(((4+2+1)+(8+4+2))*0x10);
	  g>>=5;
	  float gv = g/32.0;
	  gv*=255;
	  unsigned int b = v&(((8+4+2+1)+(8))*(1024));
	  b>>=11;
	  float bv = b/16.0;
	  bv*=255;
	  unsigned int rr = (unsigned int)rv;
	  unsigned int gg = (unsigned int)gv;
	  unsigned int bb = (unsigned int)bv;
	  rr<<=16;
	  gg<<=8;
	  val=0xff000000 + rr+gg+bb;
	  break;
	  }
	case FrameBufferFormat::F_RGB888:
	  //std::cout << "F_RGB888 not supported\n" << std::endl;
	  {
	  unsigned int *b = ((unsigned int*)buffer) + y*width+x;
	  val = *b;
	  val |= 0xff000000;
	  break;
	  }
	case FrameBufferFormat::F_RGBA8888:
	  {
	  unsigned int *b = ((unsigned int*) buffer) + y*width + x;
	  val = *b;
#ifdef EMSCRIPTEN
	  unsigned int a = val&0xff000000;
	  unsigned int r = val&0x00ff0000;
	  unsigned int g = val&0x0000ff00;
	  unsigned int bb = val&0x000000ff;
	  //a>>=24;
	  r>>=16;
	  g>>=8;
	  
	  bb<<=16;
	  g<<=8;
	  //r<<=8;
	  val=a+r+g+bb;
#endif

	  break;
	  }
	};
#endif
	
	// PUTPIXEL
	unsigned int *target_pixel = (unsigned int*)(((unsigned char*) surf->pixels) + y*surf->pitch + x*sizeof(*target_pixel));
	*target_pixel = val;
      }
  g_low->sdl->SDL_UnlockSurface(surf);

}

void clear_sdl_surface(Low_SDL_Surface *surf, int width, int height, unsigned int val)
{
#if 0
  SDL_LockSurface(surf);
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	unsigned int *target_pixel = (unsigned int*)(((unsigned char*) surf->pixels) + y*surf->pitch + x*sizeof(*target_pixel));
	*target_pixel = val;
      }
  SDL_UnlockSurface(surf);
#endif
}
extern Low_SDL_Window *sdl_framebuffer_window;

Low_SDL_Surface *init_sdl_surface_framebuffer(int width, int height);


void get_iot_event(const GameApi::MainLoopApi::Event &e, bool *array);
class FBU_run : public Splitter
{
public:
  FBU_run(GameApi::Env &env, GameApi::EveryApi &ev, FrameBuffer *buf, int mode, int scr_x, int scr_y) : env(env), ev(ev), buf(buf), scr_x(scr_x), scr_y(scr_y) { exit=false; firsttime = true; }
  virtual void Init() {
    surf = init_sdl_surface_framebuffer(scr_x, scr_y);
  }
  virtual Splitter* NextState(int code) { return 0; }
  virtual int Iter() {
    //std::cout << "Iter: " << async_pending_count << std::endl;
    if (async_pending_count>0 && !async_is_done) {
      return -1;
    } 
    async_is_done = true;
    if (firsttime) {
      buf->Prepare();
      firsttime = false;
    }
    //std::cout << "FBU_run::Iter" << std::endl;
    // TODO clear the screen
    clear_sdl_surface(surf,scr_x,scr_y,0xffffff00);
    // TODO event loop.
    // 
    buf->frame();
    CopyFrameToSurface(buf, surf);
    g_low->sdl->SDL_UpdateWindowSurface(sdl_framebuffer_window);

    GameApi::MainLoopApi::Event e;
    while((e = ev.mainloop_api.get_event()).last==true)
      {
    	if (e.ch==27 && e.type==0x300) { std::cout << "ESC pressed3" << std::endl; exit=true; }

	FrameLoopEvent event;
	event.type = e.type;
	event.ch = e.ch;
	event.cursor_pos = *find_point(env,e.cursor_pos);
	event.button = e.button;
	//event.pin = 
	get_iot_event(e,&event.pin[0]);
	buf->handle_event(event);
     }
#ifndef EMSCRIPTEN
    // this tries to make emscripten port and win32 build run the same speed.
#ifndef LINUX
    g_low->sdl->SDL_Delay(16);
#endif
#endif
    if (exit) return 0;
    return -1;
  }
  virtual void Destroy()
  {
    g_low->sdl->SDL_FreeSurface(surf);
    g_low->sdl->SDL_DestroyWindow(sdl_framebuffer_window);
    sdl_framebuffer_window = 0;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  FrameBuffer *buf;
  Low_SDL_Surface *surf;
  int scr_x, scr_y;
  bool exit;
  bool firsttime;
};

GameApi::RUN GameApi::LowFrameBufferApi::low_framebuffer_run(EveryApi &ev, GameApi::FBU buf, int mode, int scr_x, int scr_y)
{
  FrameBuffer *buf2 = find_framebuffer(e, buf);
  return add_splitter(e, new FBU_run(e,ev,buf2,mode,scr_x,scr_y));
}


void BufferRefToSourceBitmap(BufferRef &ref, SourceBitmap &target, DrawBufferFormat fmt)
{
  unsigned int *buffer = ref.buffer;
  unsigned int width = ref.width;
  unsigned int height = ref.height;
  unsigned int ydelta = ref.ydelta;
  switch(fmt) {
  case D_Mono1:
    //std::cout << "D_Mono1 not supported in BufferRefToSourceBitmap" << std::endl;
    target.set_data_mono1(buffer, width, height, ydelta);
    break;
  case D_RGBA8888:
    target.set_data(buffer, width, height, ydelta);
    break;
  default:
    std::cout << "Default not supported in BufferRefToSourceBitmap" << std::endl;
    break;
  };
}
void BitmapCollectionToSourceBitmap(BitmapCollection<Color> &bms, SourceBitmap &target, DrawBufferFormat fmt, int i)
{
  BufferFromBitmapCollection buf(bms,i);
  buf.Gen();
  BufferRef buf2 = buf.Buffer();
  BufferRefToSourceBitmap(buf2, target, fmt);
}
void BitmapToSourceBitmap(Bitmap<Color> &bm, SourceBitmap &target, DrawBufferFormat fmt)
{
  //std::cout << "::" << bm.SizeX() << " " << bm.SizeY() << std::endl;
  BufferFromBitmap buf(bm);
  //buf.Gen();
#ifndef THREADS
  buf.Gen();
#else
  buf.GenPrepare();

  int numthreads = 4;
  ThreadedUpdateTexture threads;
  int sx = bm.SizeX();
  int sy = bm.SizeY();
  int dsy = sy/numthreads + 1;
  std::vector<int> ids;
  for(int i=0;i<numthreads;i++)
    {
      int start_x = 0;
      int end_x = sx;
      int start_y = i*dsy;
      int end_y = (i+1)*dsy;
      if (start_y>sy) { start_y = sy; }
      if (end_y>sy) end_y = sy;
      
      if (end_y-start_y > 0)
	ids.push_back(threads.push_thread(&buf, start_x, end_x, start_y, end_y));
    }
  int ss = ids.size();
  for(int i=0;i<ss;i++)
    {
      threads.join(ids[i]);
    }
#endif

  BufferRef buf2 = buf.Buffer();
  BufferRefToSourceBitmap(buf2, target, fmt);
}

struct PosC { std::string name; Point p; };
std::vector<PosC> pos_vec;
void add_posc(std::string name, Point p)
{
  int s = pos_vec.size();
  for(int i=0;i<s;i++)
    {
      if (pos_vec[i].name==name) { pos_vec[i].p = p; return; }
    }
  PosC c;
  c.name = name;
  c.p = p;
  pos_vec.push_back(c);
}
PosC find_posc(std::string name)
{
  int s = pos_vec.size();
  for(int i=0;i<s;i++) {
    if (pos_vec[i].name==name) return pos_vec[i];
  }
  PosC p = { "",{0.0,0.0,0.0} };
  return p;
}


class SpriteDraw : public FrameBufferLoop
{
public:
  SpriteDraw(std::string name, Bitmap<Color> &bm, Movement *move, int x, int y, int fmt, float start_time) : name(name), bm(bm), move(move), x(x),y(y), src(fmt?D_RGBA8888:D_Mono1,0), fmt(fmt?D_RGBA8888:D_Mono1), start_time(start_time) { 
    prepared=false;
  }
  ~SpriteDraw() { }

  virtual void Prepare()
  {
    bm.Prepare();
    BitmapToSourceBitmap(bm,src, fmt);
    prepared=true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    move->draw_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    if (prepared) {
    move->draw_frame(e);
    Point p = {float(x),float(y),0.0};
    //std::cout << "Time: " << e.time << " " << e.delta_time << std::endl;
    Matrix m = move->get_whole_matrix(e.time*10.0, (e.delta_time*10.0));
    Point p2 = p*m;
    //std::cout << "Pos: " << p2 << std::endl;
    e.drawbuffer->draw_sprite(&src, p2.x,p2.y);
    add_posc(name, Point(p2.x,p2.y,0.0));
    } else { std::cout << "frame before prepare in SpriteDraw" << std::endl; }
  }
private:
  std::string name;
  Bitmap<Color> &bm;
  Movement *move;
  int x, y;
  SourceBitmap src;
  DrawBufferFormat fmt;
  float start_time;
  bool prepared;
};

GameApi::FML GameApi::LowFrameBufferApi::low_sprite_draw(std::string name, BM bm, MN mn, int x, int y, int fmt, float start_time)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  Movement *move = find_move(e, mn);
  return add_framemainloop(e,new SpriteDraw(name, *b2, move,x,y,fmt,start_time));

}

int last_io_pin = -1;
bool iot_firsttime = true;

void get_iot_event(const GameApi::MainLoopApi::Event &e, bool *array)
{
  static bool but[10];
  if (iot_firsttime) {
    for(int i=0;i<9;i++) but[i]=false;
    iot_firsttime = false;
  }
  if (e.type==768 && e.ch>='0' && e.ch<='9') {
    int num = e.ch-'1';
    but[num] = true;
  }
  if (e.type==769 && e.ch>='0' && e.ch<='9') {
    int num = e.ch-'1';
    but[num] = false;
  }

#ifdef IOT_EVENTS
  auto mIO = (sf::IOPin*)sf::pmap::myIO);
  mIO->setEvent(sf::IOPinEvent::IN_Rising, [](auto pin, auto pol) {
      if (pin>=0 && pin<=9)
	but[pin] = true;
    });
  mIO->setEvent(sf::IOPinEvent::IN_Falling, [](auto pin, auto pol) {
      if (pin>=0 && pin<=9)
	but[pin] = false;
    }
#endif
  for(int i=0;i<9;i++) {
    array[i] = but[i];
  }
}

    void DrawGouraudTri(FrameBuffer *buf, DrawBufferFormat format, Point *points, unsigned int *colours, int numtriangles, Matrix m, float *depth_buffer);

class GouraudDraw : public FrameBufferLoop
{
public:
  GouraudDraw(std::string name, FaceCollection *coll, Movement *move) : name(name), coll(coll), size(0), move(move) { ss=0;}
  void Prepare() { 
    coll->Prepare();
    int s = coll->NumFaces();
    size = s;
    ss = coll->NumPoints(0);

    // THIS EATS WAY TOO MUCH MEMORY, BUT WE CAN'T DO ANYTHING SINCE
    // WE DON'T HAVE GPU MEMORY AVAILABLE. NORMAL SOLUTION TO THIS PROBLEM
    // IS BATCHING WHICH SPLITS THESE ARRAYS TO SMALL PIECES + TRANSFERS
    // TO GPU MEMORY IN SMALL PIECES.
    vertex_array = new Point[s*3];
    color_array = new unsigned int[s*3];
    for(int i=0;i<s;i++)
      {
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,1);
	Point p3 = coll->FacePoint(i,2);
	vertex_array[i*3+0] = p1;
	vertex_array[i*3+1] = p2;
	vertex_array[i*3+2] = p3;
	unsigned int c1=coll->Color(i,0);
	unsigned int c2=coll->Color(i,1);
	unsigned int c3=coll->Color(i,2);
	color_array[i*3+0] = c1;
	color_array[i*3+1] = c2;
	color_array[i*3+2] = c3;
      }
    if (ss==4) {
    vertex_array2 = new Point[s*3];
    color_array2 = new unsigned int[s*3];
    for(int i=0;i<s;i++)
      {
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,2);
	Point p3 = coll->FacePoint(i,3);
	vertex_array2[i*3+0] = p1;
	vertex_array2[i*3+1] = p2;
	vertex_array2[i*3+2] = p3;
	unsigned int c1=coll->Color(i,0);
	unsigned int c2=coll->Color(i,2);
	unsigned int c3=coll->Color(i,3);
	color_array2[i*3+0] = c1;
	color_array2[i*3+1] = c2;
	color_array2[i*3+2] = c3;
      }
    }
  }
  void handle_event(FrameLoopEvent &e) {
    move->draw_event(e);
  }
  void frame(DrawLoopEnv &e)
  {
    //Point ptr[]= { {5.0,5.0,0.0}, {100.0,50.0,0.0}, {50.0,100.0,0.0} };
    //unsigned int color[] = { 0xffffffff, 0xff888888, 0xff000000 };
    Matrix m = move->get_whole_matrix(e.time*10.0, (e.delta_time*10.0));

    Matrix m2 = m*Matrix::Scale(1.0,-1.0,1.0)*Matrix::Translate(400.0,300.0,0.0);
    DrawGouraudTri(e.drawbuffer, e.format, vertex_array, color_array, size,m2 , e.depth_buffer);
    if (ss==4) {
    DrawGouraudTri(e.drawbuffer, e.format, vertex_array2, color_array2, size,m2 , e.depth_buffer);
    }
    Point p(0.0,0.0,0.0);
    Point p2 = p*m2;
    add_posc(name,p2);
    
  }
private:
  std::string name;
  FaceCollection *coll;
  int ss;
  Point *vertex_array;
  unsigned int *color_array;
  Point *vertex_array2;
  unsigned int *color_array2;
  int size;
  Movement *move;
};

GameApi::FML GameApi::LowFrameBufferApi::low_poly_draw(std::string name, P p, MN mn)
{
  FaceCollection *coll = find_facecoll(e,p);
  Movement *move = find_move(e, mn);
  return add_framemainloop(e, new GouraudDraw(name, coll,move));
}

void DrawGouraudTri(FrameBuffer *buf, DrawBufferFormat format, Point *points, unsigned int *colours, int numtriangles, Matrix m, float *depth_buffer)
{
  void *buffer = buf->Buffer();
  unsigned int *buffer2 = (unsigned int*)buffer;
  int width = buf->Width();
  int height = buf->Height();
  switch(format) {
  case D_Mono1:
    std::cout << "GouraudTri mono1 not implemented" << std::endl;
    break;
  case D_RGBA8888:
    {
      for(int p=0;p<numtriangles;p++) {
	Point p1 = points[0];
	Point p2 = points[1];
	Point p3 = points[2];
	unsigned int c1 = colours[0];
	unsigned int c2 = colours[1];
	unsigned int c3 = colours[2];
	p1 = p1*m;
	p2=p2*m;
	p3=p3*m;
	//std::cout << p1 << " " << p2 << " " << p3 << std::endl;
	// now points are in correct position
	if (p2.y<p1.y) { std::swap(p1,p2); std::swap(c1,c2); }
	if (p3.y<p1.y) { std::swap(p1,p3); std::swap(c1,c3); }
	if (p3.y<p2.y) { std::swap(p2,p3); std::swap(c2,c3); }


	// now points are in correct order
	int y = int(p1.y)+1;
	int ey = int(p2.y);
	
	float dy = p2.y-p1.y;
	float ddy = p3.y-p1.y;
	float dx = p2.x-p1.x;
	float ddx = p3.x-p1.x;
	float dz = p2.z-p1.z;
	float ddz = p3.z-p1.z;
	

	dx/=dy;
	ddx/=ddy;

	dx*=(float(y)-p1.y);
	ddx*=(float(y)-p1.y);


	float xx = p1.x+dx;
	float xxx = p1.x+ddx;

	float zz = p1.z+dz;
	float zzz = p1.z+ddz;

	unsigned int cxx = dy>0.01?Color::Interpolate(c1,c2, (float(y)-p1.y)/dy):0xffffffff;
	unsigned int cxxx = ddy>0.01?Color::Interpolate(c1,c3, (float(y)-p1.y)/ddy):0xffffffff;
	float dxx = (p2.x-p1.x)/(p2.y-p1.y);
	float dxxx = (p3.x-p1.x)/(p3.y-p1.y);
	float dzz = (p2.z-p1.z)/(p2.y-p1.y);
	float dzzz = (p3.z-p1.z)/(p3.y-p1.y);
	unsigned int dcxx = dy>0.01?Color::Interpolate(c1,c2,1.0/dy):0xffffffff;
	unsigned int dcxxx = ddy>0.01?Color::Interpolate(c1,c3,1.0/ddy):0xffffffff;
	bool b = false;
	if (xxx<xx) { std::swap(xx,xxx); std::swap(cxx,cxxx); std::swap(dxx,dxxx); 
	  std::swap(dcxx,dcxxx);
	  std::swap(zz,zzz);
	  std::swap(dzz,dzzz);
	  b = true;
	}

	for(int yy = y; yy<=ey;yy++)
	  {
	    if (yy<0 || yy>=height) continue;
	    unsigned int cxx = (p2.y-p1.y>0.01)?Color::Interpolate(c1,c2, (float(yy)-p1.y)/(p2.y-p1.y)):0xffffffff;
	    unsigned int cxxx = (p3.y-p1.y>0.01)?Color::Interpolate(c1,c3, (float(yy)-p1.y)/(p3.y-p1.y)):0xffffffff;
	    if (b) { std::swap(cxx,cxxx); }
	    unsigned int *bk = buffer2 + yy*width + int(xx) + 1;
	    float *dk = depth_buffer + yy*width + int(xx)+1;
	    for(int k = xx;k<xxx;k++)
	      {
		if (k<0 || k>=width) continue;
		float r = float(k-xx)/float(xxx-xx);
		float z = (1.0-r)*zz+r*zzz;
		if (*dk >= z) {
		  unsigned int c = Color::Interpolate(cxx,cxxx,r);
		  *bk = c;
		  *dk = z;
		}
		bk++;
		dk++;
	      }
	    xx+=dxx;
	    xxx+=dxxx;
	    zz+=dzz;
	    zzz+=dzzz;
	  }
	int eey = int(p3.y);
	float d = (p2.x-p1.x)*(p3.y-p1.y)-(p2.y-p1.y)*(p3.x-p1.x);
	if (d>0.0) {
	  xxx = p2.x;
	  zzz = p2.z;
	  dxxx = (p3.x-p2.x)/(p3.y-p2.y);
	  dzzz = (p3.z-p2.z)/(p3.y-p2.y);
	} else {
	  xx = p2.x;
	  zz = p2.z;
	  dxx = (p3.x-p2.x)/(p3.y-p2.y);
	  dzz = (p3.z-p2.z)/(p3.y-p2.y);
	}
	for(int yy=ey+1;yy<eey+1;yy++)
	  {
	    if (yy<0 || yy>=height) continue;
	    unsigned int cxx = p3.y-p2.y>0.01?Color::Interpolate(c2,c3, (float(yy)-p2.y)/(p3.y-p2.y)):0xffffffff;
	    unsigned int cxxx = p3.y-p1.y>0.01?Color::Interpolate(c1,c3, (float(yy)-p1.y)/(p3.y-p1.y)):0xffffffff;
	    if (b) { std::swap(cxx,cxxx); }
	    unsigned int *bk = buffer2 + yy*width + int(xx) +1;
	    float *dk = depth_buffer + yy*width + int(xx)+1;
	    for(int k=xx;k<xxx;k++)
	      {
		if (k<0 || k>=width) continue;
		float r = float(k-xx)/float(xxx-xx);
		float z = (1.0-r)*zz+r*zzz;
		if (*dk >= z) {
		  unsigned int c = Color::Interpolate(cxx,cxxx, r);
		  *bk = c;
		  *dk = z;
		}
		bk++;
		dk++;
	      }
	    xx+=dxx;
	    xxx+=dxxx;
	    zz+=dzz;
	    zzz+=dzzz;
	  }
	points+=3;
	colours+=3;
      }
    break;
    }
  };
}

#if 0
bool operator<(ptw32_handle_t t1,ptw32_handle_t t2) { return t1.x<t2.x; }
#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/language/chaiscript_common.hpp>

class ChaiBitmap : public Bitmap<Color>
{
public:
  ChaiBitmap(GameApi::Env &e, std::string url, std::string homepage, int sx, int sy) : e(e), url(url),homepage(homepage), sx(sx), sy(sy) { 
    chai = new chaiscript::ChaiScript;
    firsttime = true;
  }
  void Prepare() {
    if (firsttime) {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    script = std::string(ptr->begin(), ptr->end());
    try {
      chai->eval(script);
    } catch (chaiscript::exception::eval_error &ee) {
      std::cout << ee.start_position.line << ":" << ee.reason << std::endl;
      exit(0);
    }
    try {
    chai->eval(R"(
        bm_prepare();
     )");
    } catch (chaiscript::exception::eval_error &ee) {
      std::cout << ee.start_position.line << ":" << ee.reason << std::endl;
      exit(0);
    }
    try {
      f = chai->eval<std::function<unsigned int (int,int)> >("bm_map");
    } catch (chaiscript::exception::eval_error &ee) {
      std::cout << ee.start_position.line << ":" << ee.reason << std::endl;
      exit(0);
    }
    firsttime = false;
    }
  }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    //std::stringstream ss;
    //ss << x << "," << y;
    //unsigned int v;
    //std::string s = std::string("bm_map(") + ss.str() + ");";
    //std::cout << s << std::endl;
    //try {
    //  v = chai->eval<unsigned int>(s);
    //} catch (chaiscript::exception::eval_error &ee) {
    //  std::cout << ee.start_position.line << ":" << ee.reason << std::endl;
    //  exit(0);
    //}
    unsigned int v = f(x,y);
    return v;
  }
  ~ChaiBitmap() { delete chai; }
private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  int sx,sy;
  chaiscript::ChaiScript *chai;
  std::string script;
  bool firsttime;
  std::function<unsigned int (int,int)> f;
};

GameApi::BM GameApi::BitmapApi::chai_bm(std::string url, int sx, int sy)
{
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = new ChaiBitmap(e,url,gameapi_homepageurl,sx,sy);
  BM bm = add_bitmap(e, handle2);
  return bm;
}
#endif


//
// EXAMPLE FILE:
// a,b,c :: flags
// k_1 :: A->B
// k_2 :: B->C
// k_1 = keyup 32
// k_2 = timer 10
// A := a,b,c
// B := a,c
// C := a,b
// 

struct ST_type {
  std::vector<std::string> labels;
  std::string right_type;
  bool is_function;
  std::string left_type;
};
std::string cut_spaces(std::string s)
{
  //int len = 0;
  int ss = s.size();
  int i=0;
  for(;i<ss;i++) { if (s[i]!=' '&&s[i]!='\n'&&s[i]!='\r') break; }
  int j=ss-1;
  int c=0;
  for(;j>=0;j--,c++) { if (s[j]!=' '&&s[j]!='\n'&&s[j]!='\r') break; }
  return s.substr(i,ss-i-c);
}
int find_substr1(std::string line, char char1)
{
  int s = line.size();
  for(int i=0;i<s;i++) { if (line[i]==char1) return i; }
  return -1;
}
int find_substr2(std::string line, char char1, char char2)
{
  int s = line.size()-1;
  for(int i=0;i<s;i++)
    {
      int ch1 = line[i];
      int ch2 = line[i+1];
      if (ch1==char1 && ch2 == char2) { return i; }
    }
  return -1;
}

ST_type parse_type(std::string line, bool &success)
{
  int pos1 = find_substr2(line, ':', ':');
  if (pos1==-1) { /*std::cout << "parse_type: pos1==-1" << std::endl;*/  success=false; return ST_type{}; }

  int pos2 = find_substr2(line, '-', '>');

  ST_type t;
  t.is_function = pos2!=-1;

  std::string lst = line.substr(0,pos1);
  t.labels = parse_sep(lst, ',');

  int sk = t.labels.size();
  for(int i=0;i<sk;i++) { t.labels[i] = cut_spaces(t.labels[i]); }

  int type1last = t.is_function?pos2:line.size()-1;
  std::string type = line.substr(pos1+2, type1last-pos1-2);
  type = cut_spaces(type);

  if (t.is_function) t.left_type = type; else t.right_type = type;

  if (t.is_function)
    {
      std::string type2 = line.substr(pos2+2, line.size()-1-pos2-2);
      type2 = cut_spaces(type2);
      t.right_type = type2;
    }
  //std::cout << "Type:" << t.is_function << " " << lst << " " << t.left_type << " " << t.right_type << std::endl;
  success=true;
  return t;
}

struct ST_event
{
  std::string label;
  int event_type; // key=1, timer=2
  int value;
};

ST_event parse_event(std::string line, bool &success)
{
  int pos0 = find_substr2(line, ':', '=');
  if (pos0!=-1) { /*std::cout << "parse_event: pos0!=-1" << std::endl;*/ success=false; return ST_event{}; }

  ST_event res;
  int pos1 = find_substr1(line, '=');
  if (pos1==-1) { /*std::cout << "parse_event: pos1==-1" << std::endl; */success=false; return ST_event{}; }
  std::string label = line.substr(0,pos1);
  res.label = cut_spaces(label);
  std::string rest = line.substr(pos1+1, line.size()-pos1-1);
  //std::cout << "parse_event, rest=" << rest << std::endl;
  rest = cut_spaces(rest);
  int pos2 = find_substr1(rest, ' ');
  if (pos2==-1) { /*std::cout << "parse_event: pos2==-1" << std::endl;*/ success=false; return ST_event{}; }
  std::string e = rest.substr(0,pos2);
  //std::cout << "e=" << e << std::endl;
  if (e=="keyup") { res.event_type = 1; }
  if (e=="keydown") { res.event_type = 2; }
  if (e=="timer") { res.event_type = 3; }
  if (e=="frame") { res.event_type = 4; }
  std::string v = rest.substr(pos2+1,rest.size()-pos2-1);
  std::stringstream ss(v);
  int val = -1; 
  if (!(ss >> val)) { /*std::cout << "parse_event: ss>>val" << std::endl;*/ success=false; return ST_event{}; }
  res.value = val;
  success=true;
  //std::cout << "Event data: " << res.label << " " << res.event_type << " " << res.value << std::endl;
  return res;
}
struct ST_state
{
  std::string state_name;
  std::vector<std::string> flags;
  //  std::vector<std::string> positions;
  //  std::vector<std::string> floats;
};
ST_state parse_state(std::string line, bool &success)
{
  int pos1 = find_substr2(line, ':', '=');
  if (pos1==-1) { /*std::cout << "parse_state: pos1==-1" << std::endl;*/ success=false; return ST_state{}; }
  std::string label = line.substr(0,pos1);
  ST_state st;
  st.state_name = cut_spaces(label);
  
  std::string rest = line.substr(pos1+2,line.size()-pos1-2);
  rest = cut_spaces(rest);
  std::vector<std::string> flags = parse_sep(rest, ',');
  int s = flags.size();
  for(int i=0;i<s;i++) flags[i]=cut_spaces(flags[i]);
  st.flags = flags;
  success = true;
  //std::cout << "State:" << st.state_name << " " << rest << std::endl;
  return st;
}


//class State {
//public:
//  virtual int NextState(int current_state, int key) =0;
//};




class StateMachineImpl : public IStateMachine
{
public:
  StateMachineImpl(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url), homepage(homepage) { current_state = 0; }
  void Prepare() {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string script = std::string(ptr->begin(), ptr->end());

    std::stringstream ss(script);
    std::string line;
    types = std::vector<ST_type>();
    events = std::vector<ST_event>();
    states = std::vector<ST_state>();
    int linenum = 0;
    while(std::getline(ss,line)) {
      if (cut_spaces(line)=="") continue; // skip empty lines
      bool success_t = false;
      bool success_e = false;
      bool success_s = false;
      ST_type t = parse_type(line, success_t);
      if (success_t) types.push_back(t);
      ST_event e = parse_event(line, success_e);
      if (success_e) events.push_back(e);
      ST_state s = parse_state(line, success_s);
      if (success_s) states.push_back(s);
      if (!success_t && !success_e && !success_s) {
	std::cout << "Error at line " << linenum << ":" << line << std::endl;
      }
      linenum++;
    }
  }
  virtual void event(MainLoopEvent &ee)
  {
    int key = ee.ch;
    //std::cout << "Event: " << key << std::endl;
    int s = events.size();
    for(int i=0;i<s;i++)
      {
	ST_event e = events[i];
	//std::cout << ee.type << " " << e.event_type << " " << e.value << " " << key << std::endl;
	if (ee.type==0x301 && e.event_type==1 && e.value==key) {
	  event(e);
	}
	if (ee.type==0x300 && e.event_type==2 && e.value==key) {
	  event(e);
	}
      }
  }
  virtual void frame(MainLoopEnv &e) 
  {
    current_frame++;
    int s = events.size();
    for(int i=0;i<s;i++)
      {
	ST_event e = events[i];
	if (e.event_type==4 && (current_frame%e.value) ==0) {
	  event(e);
	}
      }
  }
  virtual void draw_event(FrameLoopEvent &ee)
  {
    int key = ee.ch;
    //std::cout << "DrawEvent: " << key << std::endl;
    int s = events.size();
    for(int i=0;i<s;i++)
      {
	ST_event e = events[i];
	//std::cout << ee.type << " " << e.event_type << " " << e.value << " " << key << std::endl;
	if (ee.type==0x301 && e.event_type==1 && e.value==key) {
	  event(e);
	}
	if (ee.type==0x300 && e.event_type==2 && e.value==key) {
	  event(e);
	}

      }

  }
  virtual void draw_frame(DrawLoopEnv &ee) { 
    current_frame++;
    int s = events.size();
    for(int i=0;i<s;i++)
      {
	ST_event e = events[i];
	if (e.event_type==4 && (current_frame%e.value) ==0) {
	  event(e);
	}
      }

  }

  virtual int num_flags() const { return states[current_state].flags.size(); }
  //virtual int num_pos() const=0;
  //virtual int num_floats() const=0;
  virtual std::string flag(int val) const
  {
    int s = states[current_state].flags.size();
    if (val>=0 && val<s)
      return states[current_state].flags[val];
    return "";
  }

  void event(const ST_event &e)
  {
    std::string event_name = e.label;
    int pos = 0;
    int old_current_state = current_state;
    while(1) {
      int t = find_type(pos,event_name);
      //std::cout << "Found: " << event_name << " " << t << std::endl;
      if (t==-1) { /*std::cout << "Event name not found: " << event_name << std::endl;*/ break; }
      pos = t+1;
      if (types[t].is_function) {
	std::string left_type = types[t].left_type;
	if (states[old_current_state].state_name == left_type)
	  {
	    std::string right_type = types[t].right_type;
	    int new_state = find_state(right_type);
	    if (new_state == -1) {std::cout << "invalid state transition:" << event_name << "::" << left_type << "->" << right_type; continue; }
	    set_state(new_state);
	    
	  }
      }
    }
    
  }
  void set_state(int new_state)
  {
    //std::cout << "new state: " << new_state << " " << states[new_state].state_name << std::endl;
    //int s = states[new_state].flags.size();
    //for(int i=0;i<s;i++)
    //  std::cout << "Flag:" << states[new_state].flags[i] << std::endl;
    current_state = new_state;
  }

  int find_type(int pos, std::string name)
  {
    int s = types.size();
    for(int i=pos;i<s;i++) {
      int ss = types[i].labels.size();
      for(int j=0;j<ss;j++)
	{
	  if (name==types[i].labels[j]) { return i; }
	}
    }
    return -1;
  }
  int find_event(std::string name)
  {
    int s = events.size();
    for(int i=0;i<s;i++)
      {
	if (events[i].label == name) { return i; }
      }
    return -1;
  }
  int find_state(std::string name)
  {
    int s = states.size();
    for(int i=0;i<s;i++)
      {
	if (states[i].state_name == name) { return i; }
      }
    return -1;
  }
private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  std::vector<ST_type> types;
  std::vector<ST_event> events;
  std::vector<ST_state> states;
  int current_state;
  int current_frame;
};

class StateIntFetcher : public Fetcher<int>
{
public:
  StateIntFetcher(GameApi::Env &e, std::string url, std::string homepage, std::string states) : impl(e,url,homepage), states(parse_sep(states,'&')) { 
    firsttime = true;
  }
  void draw_event(FrameLoopEvent &e) { if (!firsttime) impl.draw_event(e); }
  void draw_frame(DrawLoopEnv &e) { 
    if (firsttime) { impl.Prepare(); firsttime=false; }
    impl.draw_frame(e); }
  void event(MainLoopEvent &e) { if (!firsttime) impl.event(e); }
  void frame(MainLoopEnv &e) { 
    if (firsttime) { impl.Prepare(); firsttime=false; }
    impl.frame(e); 
  }
  void set(int i) { }
  int get() const {
    if (!firsttime) {
    int s = states.size();
    for(int i=0;i<s;i++)
      {
	std::string s2 = states[i];
	int s3 = impl.num_flags();
	for(int j=0;j<s3;j++)
	  {
	    std::string flag = impl.flag(j);
	    //std::cout << "FlagCompare: '" << s2 << "' '" << flag << "'" << std::endl;
	    if (s2==flag) return i;
	  }
      }
    }
    return 0;
  }
private:
  StateMachineImpl impl;
  std::vector<std::string> states;
  bool firsttime;
};

GameApi::IF GameApi::MainLoopApi::state_int_fetcher(std::string url, std::string states)
{
  return add_int_fetcher(e, new StateIntFetcher(e, url, gameapi_homepageurl, states));
}

class StateMovement : public Movement
{
public:
  StateMovement(GameApi::Env &e, Movement *next, std::string url, std::string homepage, std::string states, std::string x_speed, std::string y_speed, std::string z_speed) : e(e), next(next), impl(e,url,homepage), m_states(parse_sep(states,'&')), m_x_speed(parse_sep(x_speed,'&')), m_y_speed(parse_sep(y_speed,'&')), m_z_speed(parse_sep(z_speed,'&')) { 
    firsttime = true;
    pos_x=0.0;
    pos_y=0.0;
    pos_z=0.0;
    sp_x=0.0;
    sp_y=0.0;
    sp_z=0.0;
  }
  void draw_event(FrameLoopEvent &e) { next->draw_event(e); impl.draw_event(e); }
  void draw_frame(DrawLoopEnv &e) { 
    next->draw_frame(e);
    if (firsttime) { impl.Prepare(); firsttime=false; }
    impl.draw_frame(e);
    update_data();
    pos_x+=sp_x*e.delta_time;
    pos_y+=sp_y*e.delta_time;
    pos_z+=sp_z*e.delta_time;

  }
  void event(MainLoopEvent &e) { next->event(e); impl.event(e); }
  void frame(MainLoopEnv &e) { 
    next->frame(e);
    if (firsttime) { impl.Prepare(); firsttime=false; }
    impl.frame(e); 
    update_data();
    pos_x+=sp_x*e.delta_time;
    pos_y+=sp_y*e.delta_time;
    pos_z+=sp_z*e.delta_time;
  }
  void set_matrix(Matrix i) { }
  Matrix get_whole_matrix(float time, float delta_time) const {
    return Matrix::Translate(pos_x,pos_y,pos_z)*next->get_whole_matrix(time,delta_time);
  }
  void update_data()
  {
    int s = m_states.size();
    int pos = 0;
    for(int i=0;i<s;i++)
      {
	std::string s2 = m_states[i];
	int s3 = impl.num_flags();
	for(int j=0;j<s3;j++)
	  {
	    std::string flag = impl.flag(j);
	    //std::cout << "FlagCompare: '" << s2 << "' '" << flag << "'" << std::endl;
	    if (s2==flag) { pos=i; }
	  }
      }
    int sxa = m_x_speed.size();
    int sya = m_y_speed.size();
    int sza = m_z_speed.size();
    if (pos<0 || pos>=sxa) return;
    if (pos<0 || pos>=sya) return;
    if (pos<0 || pos>=sza) return;
    std::string sx = m_x_speed[pos];
    std::string sy = m_y_speed[pos];
    std::string sz = m_z_speed[pos];
    std::stringstream ss(sx);
    ss >> sp_x;
    sp_x*=100.0;
    std::stringstream ss2(sy);
    ss2 >> sp_y;
    sp_y*=100.0;
    std::stringstream ss3(sz);
    ss3 >> sp_z;
    sp_z*=100.0;

  }
private:
  GameApi::Env &e;
  Movement *next;
  StateMachineImpl impl;
  std::vector<std::string> m_states;
  std::vector<std::string> m_x_speed;
  std::vector<std::string> m_y_speed;
  std::vector<std::string> m_z_speed;
  float sp_x, sp_y, sp_z;
  float pos_x, pos_y, pos_z;
  bool firsttime;
};


GameApi::MN GameApi::MainLoopApi::state_speed_movement(MN mn, std::string url, std::string states, std::string x_speeds, std::string y_speeds, std::string z_speeds)
{
  Movement *next = find_move(e, mn);
  return add_move(e, new StateMovement(e, next, url, gameapi_homepageurl, states, x_speeds, y_speeds, z_speeds));
}

struct BM_coll
{
  int num;
  Point tl;
  Point br;
};

BM_coll parse_bm_collection(std::string line, bool &success)
{
  std::stringstream ss(line);
  char c,c2,c3,c4,c5;
  BM_coll coll;
  ss >> coll.num >> c;
  if (c!=':') { std::cout << "A" << c << std::endl; success=false; return BM_coll{}; }
  ss >> c2 >> coll.tl.x >> c3 >> coll.tl.y >> c4 >> coll.tl.z >> c5;
  if (c2!='(') { std::cout << "B" << std::endl; success=false; return BM_coll{}; }
  if (c3!=',') { std::cout << "C" << std::endl; success=false; return BM_coll{}; }
  if (c4!=',') { std::cout << "D" << std::endl; success=false; return BM_coll{}; }
  if (c5!=')') { std::cout << "E" << std::endl; success=false; return BM_coll{}; }
  ss >> c2 >> coll.br.x >> c3 >> coll.br.y >> c4 >> coll.br.z >> c5;
  if (c2!='(') { std::cout << "F" << std::endl; success=false; return BM_coll{}; }
  if (c3!=',') { std::cout << "G" << std::endl; success=false; return BM_coll{}; }
  if (c4!=',') { std::cout << "H" << std::endl; success=false; return BM_coll{}; }
  if (c5!=')') { std::cout << "I" << std::endl; success=false; return BM_coll{}; }
  //std::cout << coll.num << " " << coll.tl << " " << coll.br << std::endl;
  success=true;
  return coll;
}

class SpriteDraw2 : public FrameBufferLoop
{
public:
  SpriteDraw2(std::string name, BitmapCollection<Color> &bms, Movement *move, int x, int y, int fmt, float start_time) : name(name), bms(bms), move(move), x(x), y(y), fmt(fmt?D_RGBA8888:D_Mono1), start_time(start_time) {
    prepared=false;
  }
  ~SpriteDraw2() { }
  virtual void Prepare()
  {
    bms.Prepare();
    int s = bms.Size();
    for(int i=0;i<s;i++)
      {
	SourceBitmap s(fmt?D_RGBA8888:D_Mono1,0);
	src.push_back(s);
	BitmapCollectionToSourceBitmap(bms,src[i], fmt, i);
      }
    prepared=true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    move->draw_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    if (prepared) {
      move->draw_frame(e);
      int s = bms.Size();
      for(int i=0;i<s;i++) {
	Point tl = bms.PosTL(i);
	//Point br = bms.PosBR(i);
	Point p = { tl.x+float(x), tl.y+float(y), tl.z };
	Matrix m = move->get_whole_matrix(e.time*10.0, (e.delta_time*10.0));
	Point p2 = p*m;
	e.drawbuffer->draw_sprite(&src[i], p2.x,p2.y);
      }
    }
  }
  
private:
  std::string name;
  BitmapCollection<Color> &bms;
  Movement *move;
  int x, y;
  std::vector<SourceBitmap> src;
  DrawBufferFormat fmt;
  float start_time;
  bool prepared;
};

class BitmapColl : public BitmapCollection<Color>
{
public:
  BitmapColl(GameApi::Env &e, std::string url, std::string homepage, std::vector<Bitmap<Color>*> vec) : e(e), url(url), homepage(homepage),vec(vec) {}
  virtual void Prepare()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string script = std::string(ptr->begin(), ptr->end());
    mappings = std::vector<BM_coll>();
    int linenum = 0;
    std::string line;
    std::stringstream ss(script);
    while(std::getline(ss,line)) {
      bool bm_success = false;
      BM_coll c = parse_bm_collection(line,bm_success);
      if (bm_success) mappings.push_back(c);
      if (!bm_success) {
	std::cout << "Error at line " << linenum << ":" << line << std::endl;
      }
      linenum++;
    }
  }
  virtual Point PosTL(int i) const
  {
    return mappings[i].tl;
  }							   
  virtual Point PosBR(int i) const
  {
    return mappings[i].br;
  }
  virtual int Size() const { return mappings.size(); }
  virtual int SizeX(int i) const {
    int num = mappings[i].num;
    int s = vec.size();
    if (num>=0 && num<s)
      return vec[num]->SizeX();
    return 2;
  }
  
  virtual int SizeY(int i) const {
    int num = mappings[i].num;
    int s = vec.size();
    if (num>=0 && num<s)
      return vec[num]->SizeY();
    return 2;
  }
  virtual Color Map(int i, int x, int y) const {
    int num = mappings[i].num;
    int s = vec.size();
    if (num>=0 && num<s)
      return vec[num]->Map(x,y);
    return Color(0xffffffff);
  }

private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  std::vector<Bitmap<Color>*> vec;
  std::vector<BM_coll> mappings;
};

GameApi::FML GameApi::LowFrameBufferApi::low_sprite_array(std::string name, std::string url, std::vector<BM> bms, MN mn, int x, int y, int fmt, float start_time)
{
  Movement *move = find_move(e,mn);
  
  std::vector<Bitmap<Color>*> vec;
  int s = bms.size();
  for(int i=0;i<s;i++)
    {
      BitmapHandle *handle = find_bitmap(e, bms[i]);
      ::Bitmap<Color> *b2 = find_color_bitmap(handle);
      vec.push_back(b2);
    }
  
  BitmapColl *coll = new BitmapColl(e,url, gameapi_homepageurl, vec);
  SpriteDraw2 *spr = new SpriteDraw2(name, *coll, move, x,y, fmt, start_time);
  return add_framemainloop(e, spr);
}

struct EData
{
  float start_x, end_x;
  float start_y, end_y;
  
};
class WorldImpl : public WorldBlocks
{
public:
  WorldImpl(int sx, int sy, int cx, int cy, Point2d tl, Point2d br) : sx(sx), sy(sy), cx(cx), cy(cy), tl(tl), br(br) 
  {
    array = new int[sx*sy];
    std::memset(array,0, sx*sy*sizeof(int));
  }
  ~WorldImpl() { delete[] array; }
  
  virtual void clear_enemies() { vec = std::vector<EData>(); }
  virtual void add_enemy(float start_x, float end_x, float start_y, float end_y) { EData d; d.start_x = start_x; d.end_x = end_x; d.start_y=start_y; d.end_y=end_y; vec.push_back(d); 
    if (vec.size()>1000) vec=std::vector<EData>();
  }
  virtual int num_enemies() const { return vec.size(); }
  virtual float enemy_start_x(int e) const { return vec[e].start_x; }
  virtual float enemy_end_x(int e) const { return vec[e].end_x; }
  virtual float enemy_start_y(int e) const { return vec[e].start_y; }
  virtual float enemy_end_y(int e) const { return vec[e].end_y; }



  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int Map(int x, int y) const
  {
    if (x>=0 && x<sx)
      if (y>=0 && y<sy)
	return array[x+y*sx];
    return 0;
  }

  // block sizes
  virtual int CellSizeX() const { return cx; }
  virtual int CellSizeY() const { return cy; }
  virtual void SetCellSize(int a_cx, int a_cy)
  {
    cx = a_cx;
    cy = a_cy;
  }
  
  // set elements
  virtual void SetElem(int x, int y, int val)
  {
    if (x>=0 && x<sx)
      if (y>=0 && y<sy)
	array[x+y*sx] = val;
  }
  virtual void SetElemBlock(int *arr, int width, int height, int ydelta, int start_x, int start_y)
  {
    for(int y=0;y<height;y++)
      for(int x=0;x<width;x++)
	{
	  if (x+start_x>=0 && x+start_x<sx)
	    if (y+start_y>=0 && y+start_y<sy)
	      array[(x+start_x)+(y+start_y)*sx] = arr[x+y*ydelta];
	}
  }
  
  // Position of the blocks in 2d space
  virtual Point2d GetTL() const { return tl; }
  virtual Point2d GetBR() const { return br; }
  virtual void SetExtends(Point2d p1, Point2d p2)
  {
    tl=p1;
    br=p2;
  }
  virtual std::pair<int,int> BlockPosition(Point2d pos) const
  {
    pos.x-=tl.x;
    pos.y-=tl.y;
    int px = pos.x;
    int py = pos.y;
    px/=cx;
    py/=cy;
    return std::make_pair(px,py);
  }
  virtual std::pair<int,int> CellPosition(Point2d pos) const
  {
    pos.x-=tl.x;
    pos.y-=tl.y;
    int px = pos.x;
    int py = pos.y;
    px = px % cx;
    py = py & cy;
    return std::make_pair(px,py);
  }
  virtual std::pair<float,float> CellPositionF(Point2d pos) const
  {
    pos.x-=tl.x;
    pos.y-=tl.y;
    pos.x=fmod(pos.x,float(cx));
    pos.y=fmod(pos.y,float(cy));
    return std::make_pair(pos.x,pos.y);
  }
  virtual std::pair<Point2d,Point2d> BlockToWorld(int x, int y) const
  {
    Point2d tla = { tl.x+x*cx, tl.y+y*cy };
    Point2d bra = { tl.x+(x+1)*cx, tl.y+(y+1)*cy };
    return std::make_pair(tla,bra);
  }


  // Position of the blocks in 3d space
  virtual Point GetTL3d() const { Point p; return p;  }
  virtual Point GetBR3d() const { Point p; return p; }
  virtual void SetExtends3d(Point tl, Point tr, Point bl, Point br) { }
  virtual void SetMV3d(Matrix m) { }
  virtual std::pair<int,int> BlockPosition3d(Point ray_start, Point ray_end) const { return std::make_pair(0,0); }
  virtual std::pair<int,int> CellPosition3d(Point ray_start, Point ray_end) const { return std::make_pair(0,0); }
  virtual std::pair<float,float> CellPositionF3d(Point ray_start, Point ray_end) const { return std::make_pair(0.0,0.0); }

  // Reallocating space
  virtual void ReserveSize(int n_sx, int n_sy)
  {
    int *newarray = new int[n_sx*n_sy];
    int ssx = std::min(sx,n_sx);
    int ssy = std::min(sy,n_sy);
    for(int y=0;y<ssy;y++)
      for(int x=0;x<ssx;x++)
	{
	  newarray[x+y*n_sx] = array[x+y*sx];
	}
    delete [] array;
    array = newarray;
    sx = n_sx;
    sy = n_sy;
  }
private:
  int *array;
  int sx,sy,cx,cy;
  Point2d tl,br;
  std::vector<EData> vec;
};
WorldBlocks *g_world=0;
WorldBlocks *GetWorld() { 
  if (g_world) return g_world;
  Point2d tl = { 0.0,0.0 };
  Point2d br = { 100.0, 100.0 };
  SetWorld(new WorldImpl(30,30,10,10,tl,br));
  return g_world;
}
void SetWorld(WorldBlocks *w)
{
  delete g_world;
  g_world = w;
}

class SpriteDraw3 : public FrameBufferLoop
{
public:
  SpriteDraw3(std::vector<Bitmap<Color>*> vec, int screen_width, int screen_height, int fmt) : vec(vec), screen_width(screen_width), screen_height(screen_height), fmt(fmt?D_RGBA8888:D_Mono1) 
  {
    Point2d tl = { 0.0,0.0 };
    Point2d br = { 100.0, 100.0 };
    SetWorld(new WorldImpl(30,30,10,10,tl,br)); 
  }
  virtual void Prepare()
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
    int cx = vec[0]->SizeX();
    int cy = vec[0]->SizeY();
    WorldBlocks *blk = GetWorld();
    blk->SetCellSize(cx,cy);

    for(int i=0;i<s;i++)
      {
	SourceBitmap s2(fmt?D_RGBA8888:D_Mono1,0);
	src.push_back(s2);
	BitmapToSourceBitmap(*vec[i],src[i], fmt);
      }
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    // move->draw_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    WorldBlocks *blk = GetWorld();
    Point2d tl = { 0.0, 0.0 };
    Point2d br = { float(screen_width), float(screen_height) };
    std::pair<int,int> p = blk->BlockPosition(tl);
    std::pair<int,int> p2 = blk->BlockPosition(br);
    int start_x = p.first-1;
    int start_y = p.second-1;
    int end_x = p2.first+1;
    int end_y = p2.second+1;
    for(int y=start_y;y<end_y;y++)
      for(int x=start_x;x<end_x;x++)
	{
	  int val = blk->Map(x,y);
	  std::pair<Point2d, Point2d> p = blk->BlockToWorld(x,y);
	  Point2d tl = p.first;
	  int s = src.size();
	  if (val>=0 && val<s)
	    e.drawbuffer->draw_sprite(&src[val], tl.x, tl.y);
	}
  }
private:
  std::vector<Bitmap<Color>*> vec;
  std::vector<SourceBitmap> src;
  int screen_width, screen_height;
  DrawBufferFormat fmt;
};

GameApi::FML GameApi::LowFrameBufferApi::low_render_world(std::vector<BM> blocks, int screen_width, int screen_height, int fmt)
{
  std::vector<Bitmap<Color>*> vec;
  int s = blocks.size();
  for(int i=0;i<s;i++)
    {
      BitmapHandle *handle = find_bitmap(e, blocks[i]);
      ::Bitmap<Color> *b2 = find_color_bitmap(handle);
      vec.push_back(b2);
    }
  return add_framemainloop(e, new SpriteDraw3(vec, screen_width, screen_height, fmt));
}


class ScrollWorld : public FrameBufferLoop
{
public:
  ScrollWorld(FrameBufferLoop *next, float speed_x, float speed_y, float p_x, float p_y, float left_offset, float right_offset, float height, float height2, int mode, int jump_frames) : next(next), speed_x(speed_x), speed_y(speed_y),p_x(p_x), p_y(p_y), pl_x(p_x-left_offset), pl_y(p_y-height), pr_x(p_x+right_offset), pr_y(p_y-height), pu_x(p_x), pu_y(p_y-height2), mode(mode), jump_frames(jump_frames) 
  {
    if (mode==0) {
      gravity=true;
    }
    if (mode==1) {
      gravity=false;
    }
    left=right=up=down=false;
    jump_up = false;
    temp_up=temp_down=false;

  }
  virtual void Prepare() { 
    next->Prepare(); 
  }
  virtual void handle_event(FrameLoopEvent &e) {
    next->handle_event(e);

    int screen_x = 800; //ev.mainloop_api.get_screen_width();
    int screen_y = 600; //ev.mainloop_api.get_screen_height();
    int scr_x_0 = 0.0;
    int scr_x_1 = screen_x/3.0;
    int scr_x_2 = screen_x*2.0/3.0;
    int scr_x_3 = screen_x;
    int scr_y_0 = 0.0;
    int scr_y_1 = screen_y/3.0;
    int scr_y_2 = screen_y*2.0/3.0;
    int scr_y_3 = screen_y;

    Point cursor = e.cursor_pos;
    
    if (mode==0)
      { // left&right + jump
	if (e.type==0x300 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) // left
	  {
	    left=true;
	  }
	if (e.type==0x301 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) // left
	  {
	    left=false;
	  }
#if 0
	if (e.type==1025 && e.button==0 && cursor.x>scr_x_0 && cursor.x<scr_x_1 && cursor.y>scr_y_1 && cursor.y<scr_y_2) left=true;
  if (e.type==1026 && e.button==-1 && cursor.x>scr_x_0 && cursor.x<scr_x_1 && cursor.y>scr_y_1 && cursor.y<scr_y_2) left=false;
#endif
	

	if (e.type==0x300 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) // right
	  {
	    right=true;
	  }
	if (e.type==0x301 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) // right
	  {
	    right=false;
	  }

#if 0
	if (e.type==1025 && e.button==0 && cursor.x>scr_x_2 && cursor.x<scr_x_3 && cursor.y>scr_y_1 && cursor.y<scr_y_2) right=true;
  if (e.type==1026 && e.button==-1 && cursor.x>scr_x_2 && cursor.x<scr_x_3 && cursor.y>scr_y_1 && cursor.y<scr_y_2) right=false;
#endif
	WorldBlocks *blk = GetWorld();
	Point2d p = { p_x,p_y };
	std::pair<int,int> pos = blk->BlockPosition(p);
	if (blk->Map(pos.first,pos.second)!=0) {
	  if (e.type==0x300 && (e.ch==' '||e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906) && !jump_up) // jump start
	    {
	      jump_up = true;
	      gravity = false;
	      jump_frame=0;
	    }
#if 0
	  if (e.type==1025 && e.button==0 && cursor.x>scr_x_1 && cursor.x<scr_x_2 && cursor.y>scr_y_0 && cursor.y<scr_y_1 && !jump_up) { jump_up=true; gravity=false; jump_frame=0; }
#endif
	  
	}
	if (e.type==0x301 && (e.ch==' '||e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906)) // jump stop
	  {
	    jump_up=false;
	    gravity=true;
	  }
#if 0
	if (e.type==1026 && e.button==-1 && cursor.x>scr_x_1 && cursor.x<scr_x_2 && cursor.y>scr_y_0 && cursor.y<scr_y_1) { jump_up=false; gravity=true; }
#endif
      }

    if (mode==1)
      { // scroll
    // keydown
	if (e.type==0x300 && (e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906)) // up
	  {
	    up=true;
	  }
	if (e.type==0x300 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) // left
	  {
	    left=true;
	  }
	if (e.type==0x300 && (e.ch=='s'||e.ch==22||e.ch==81||e.ch==1073741905)) // down
	  {
	    down=true;
	  }
	if (e.type==0x300 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) // right
	  {
	    right=true;
	  }
	// keyup
	if (e.type==0x301 && (e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906)) // up
	  {
	    up=false;
	  }
	if (e.type==0x301 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) // left
	  {
	    left=false;
	  }
	if (e.type==0x301 && (e.ch=='s'||e.ch==22||e.ch==81||e.ch==1073741905)) // down
	  {
	    down=false;
	  }
	if (e.type==0x301 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) // right
	  {
	    right=false;
	  }
      }
  }
  virtual void frame(DrawLoopEnv &e) {
    WorldBlocks *blk = GetWorld();
    Point2d p = { p_x,p_y };
    Point2d pl = { pl_x, pl_y };
    Point2d pr = { pr_x, pr_y };
    Point2d pu = { pu_x, pu_y };
    std::pair<int,int> pos = blk->BlockPosition(p);
    std::pair<int,int> pos_l = blk->BlockPosition(pl);
    std::pair<int,int> pos_r = blk->BlockPosition(pr);
    std::pair<int,int> pos_u = blk->BlockPosition(pu);
    
    // gravity
    if (gravity && blk->Map(pos.first,pos.second)==0) {
      temp_down=true;
    } else { temp_down=false; }

    // move prevention
    temp_not_left=false;
    temp_not_right=false;
    temp_not_up = false;
    temp_not_down = false;
    if (blk->Map(pos_l.first, pos_l.second)!=0) temp_not_left=true;
    if (blk->Map(pos_r.first, pos_r.second)!=0) temp_not_right=true;
    if (blk->Map(pos.first,pos.second)!=0) temp_not_down=true;
    if (blk->Map(pos_u.first, pos_u.second)!=0) { temp_not_up=true;  }
    if (temp_not_down && temp_not_up) { temp_not_left=true; temp_not_right=true; }

    // jumping
    if (jump_up) {
      temp_up=true;
      jump_frame++;
      if (jump_frame>jump_frames) { temp_up=false; jump_up=false; gravity=true; }
    } else { temp_up=false; }



    // actual movement
    Point2d tl = blk->GetTL();
    Point2d br = blk->GetBR();
    if ((up||temp_up)&&!temp_not_up) { tl.y+=speed_y; br.y+=speed_y; } else
    if ((down||temp_down)&&!temp_not_down) { tl.y-=speed_y; br.y-=speed_y; }
    if (left&&!temp_not_left) { tl.x+=speed_x; br.x+=speed_x; } else
    if (right&&!temp_not_right) { tl.x-=speed_x; br.x-=speed_x; }

    blk->SetExtends(tl,br);

    next->frame(e);
  }
  
private:
  FrameBufferLoop *next;
  float speed_x;
  float speed_y;
  float p_x, p_y;
  float pl_x, pl_y;
  float pr_x, pr_y;
  float pu_x, pu_y;
  bool left,right,up,down;
  bool temp_up, temp_down;
  bool temp_not_left, temp_not_right;
  bool temp_not_up, temp_not_down;
  bool jump_up, gravity;
  int jump_frame;
  int mode;
  int jump_frames;
};

GameApi::FML GameApi::LowFrameBufferApi::low_scroll_world(GameApi::FML ml, float speed_x, float speed_y, float p_x, float p_y, float left_offset, float right_offset, float height, float height2, int mode, int jump_frames)
{
  FrameBufferLoop *loop = find_framemainloop(e, ml);
  return add_framemainloop(e, new ScrollWorld(loop, speed_x, speed_y,p_x,p_y,left_offset, right_offset, height, height2, mode, jump_frames));
}

class BuildWorld : public FrameBufferLoop
{
public:
  BuildWorld(GameApi::Env &e, FrameBufferLoop *next, std::string url, std::string homepageurl, std::string chars, int x, int y) : e(e), next(next), url(url), homepage(homepageurl), chars(chars), x(x), y(y), ssx(1), ssy(1) {}
  virtual void Prepare()
  {
    next->Prepare();

#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string data = std::string(ptr->begin(), ptr->end());

    std::stringstream ss(data);
    ss >> ssx >> ssy;

    int *array = new int[ssx*ssy];
    for(int i=0;i<ssx*ssy;i++)
      {
	unsigned char c;
	ss >> c;
	int s = chars.size();
	int val = 0;
	for(int j=0;j<s;j++)
	  {
	    if (c==(unsigned char)chars[j]) val=j;
	  }
	array[i] = val;
      }
    WorldBlocks *blk = GetWorld();
    int sx = std::max(ssx+x,blk->SizeX());
    int sy = std::max(ssy+y,blk->SizeY());
    blk->ReserveSize(sx,sy);
    blk->SetElemBlock(array, ssx, ssy, ssx, x,y);
    delete []array;
   
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    next->frame(e);
  }

private:
  GameApi::Env &e;
  FrameBufferLoop *next;
  std::string url;
  std::string homepage;
  std::string chars;
  int x;
  int y;
  int ssx;
  int ssy;
};

GameApi::FML GameApi::LowFrameBufferApi::low_build_world(GameApi::FML ml, std::string url, std::string chars, int x, int y)
{
  FrameBufferLoop *loop = find_framemainloop(e, ml);
  return add_framemainloop(e, new BuildWorld(e,loop, url, gameapi_homepageurl, chars, x,y));
}

class FrameBufferBitmap : public Bitmap<Color>
{
public:
  FrameBufferBitmap(FrameBuffer *buf) : buf(buf) { }
  virtual int SizeX() const { return buf->Width(); }
  virtual int SizeY() const { return buf->Height(); }
  virtual Color Map(int x, int y) const
  {
    unsigned int color = *(((unsigned int*)buf->Buffer())+x+y*buf->Width());
    return Color(color);
  }
  virtual void Prepare() { 
    buf->Prepare();
    buf->frame();
  }

private:
  FrameBuffer *buf;
};

GameApi::BM GameApi::MainLoopApi::framebuffer_bitmap(FBU buf)
{
  FrameBuffer *fbuf = find_framebuffer(e, buf);
  return add_color_bitmap(e, new FrameBufferBitmap(fbuf));
}

class LowFrameBuffer : public FrameBuffer
{
public:
  LowFrameBuffer(FrameBufferLoop *loop, int format, int width, int height, int depth) : loop(loop),m_format(format), width(width), height(height), depth(depth) { firsttime = true;  }
  virtual void Prepare()
  {

    loop->Prepare();

    depth_buffer = new float[width*height];
    // 
    FrameBufferFormat fmt = (FrameBufferFormat)m_format;
    switch(fmt) {
    case FrameBufferFormat::F_Mono1:
      buffer = new unsigned char[width*height/8];
      size = width*height/8;
      break;
    case FrameBufferFormat::F_Mono8:
      buffer = new unsigned char[width*height];
      size = width*height;
      break;
    case FrameBufferFormat::F_RGB565:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGB888:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGBA8888:
      buffer = new unsigned int[width*height];
      size=width*height*sizeof(unsigned int);
      break;
    }
    firsttime = true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    auto p3 = std::chrono::system_clock::now();
    auto dur_in_seconds3 = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p3.time_since_epoch()));
    double val = dur_in_seconds3.count();  
    e.time = float(val-start_time_epoch);

    loop->handle_event(e);
  }
  virtual void frame()
  {
    // clear the buffer
    std::memset(buffer,0, size);
    ClearDepthBuffer(depth_buffer, width, height);
    if (firsttime) {
      auto p2 = std::chrono::system_clock::now();
      auto dur_in_seconds = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p2.time_since_epoch()));
      start_time = dur_in_seconds.count();
      //start_time/=1000.0;
      start_time_epoch = start_time;
      firsttime = false;
    }
    DrawLoopEnv e;
    e.format = D_RGBA8888;
    e.drawbuffer = this;
    e.depth_buffer = depth_buffer;
    //e.drawbuffer_width = width;
    //e.drawbuffer_height = height;
    //e.drawbuffer_depth = depth;
    auto p3 = std::chrono::system_clock::now();
    auto dur_in_seconds3 = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p3.time_since_epoch()));
    double val = dur_in_seconds3.count();  
    //val/=1000.0;
    // std::cout << "FTime: " << val << " " << start_time_epoch << std::endl;
    
    //std::chrono::time_point p = std::chrono::high_resolution_clock::now();
    e.time = float(val-start_time_epoch);
    e.delta_time = float(val-start_time);
    start_time = val;
    loop->frame(e);

    // TODO submit to iot platform
  }
  virtual void *Buffer() const { return buffer; }
  virtual int Width() const { return width; }
  virtual int Height() const { return height; }
  virtual int Depth() const { return depth; }
  virtual FrameBufferFormat format() const { return (FrameBufferFormat)m_format; }
  virtual float *DepthBuffer() const { return depth_buffer; }

  virtual void draw_rect(int pos_x, int pos_y, int sp_width, int sp_height, unsigned int color)
  {
    switch(m_format) {
    case FrameBufferFormat::F_RGBA8888:
      	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	unsigned int *bufp = &((unsigned int*)buffer)[pos_x+start_x+(start_y+pos_y)*width];
	//unsigned int *cp = &((unsigned int*)buf)[start_x+start_y*sp_ydelta];
	int delta_w = w-start_x;
	for(int y=start_y;y<h;y++) {
	  for(int x=start_x;x<w;x++)
	    {
	      unsigned int c = color;
	      
	      //if (c>0x80000000) {
		*bufp = c;
		//}
	      //cp++;
	      bufp++;
	    }
	  //cp+=sp_ydelta-delta_w;
	  bufp+=width-delta_w;
	}
	}

    }
  }
  virtual void draw_sprite(SourceBitmap *bm, int pos_x, int pos_y)
  {
    int sp_width=bm->m_width;
    int sp_height = bm->m_height;
    int sp_ydelta = bm->m_ydelta;
    void *buf = bm->m_data;
    switch(m_format) {
    case FrameBufferFormat::F_Mono1:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      int s_pos = (x/8+y*sp_ydelta);
	      int t_pos = ((pos_x+x)/8+(y+pos_y)*(width));
	      int s_bit = x&0x7;
	      int t_bit = (x+pos_x)&0x7;
	      unsigned char b = ((unsigned char*)buf)[s_pos];
	      b>>=7-s_bit;
	      b&=1;
	      if (b) {
		((unsigned char*)buffer)[t_pos]|=1<<(7-t_bit);
	      } else {
		((unsigned char*)buffer)[t_pos]&=~(1<<(7-t_bit));
	      }
	      //((unsigned char*)buffer)[(pos_x+x)/8+(y+pos_y)*width/8] = ((unsigned char*)buf)[x/8+y*sp_ydelta];

	    }
	}
	//std::cout << "draw_sprite, mono1->mono1 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono1 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_Mono8:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->mono8 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono8 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB565:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb565 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb565 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb888 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb888 not implemented" << std::endl;
	break;
      };
      break;
    case FrameBufferFormat::F_RGBA8888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:

	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      //std::cout << "(" << x << "," << y << ") " << buffer << " " << buf << std::endl;
	      int pos = (x/8+y*sp_ydelta);
	      int bit = x&0x7;
	      unsigned char b = ((unsigned char*)buf)[pos];
	      b>>=7-bit;
	      b&=1;

	      ((unsigned int*)buffer)[pos_x+x+(y+pos_y)*width] = b?0xffffffff:0xff000000;
	    }
	} 

	//std::cout << "draw_sprite, mono1->rgba8888 not implemented" << std::endl;
	break;
      case DrawBufferFormat::D_RGBA8888:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	unsigned int *bufp = &((unsigned int*)buffer)[pos_x+start_x+(start_y+pos_y)*width];
	unsigned int *cp = &((unsigned int*)buf)[start_x+start_y*sp_ydelta];
	int delta_w = w-start_x;
	for(int y=start_y;y<h;y++) {
	  for(int x=start_x;x<w;x++)
	    {
	      unsigned int c = *cp;
	      
	      if (c>0x80000000) {
		*bufp = c;
	      }
	      cp++;
	      bufp++;
	    }
	  cp+=sp_ydelta-delta_w;
	  bufp+=width-delta_w;
	}
	}
	break;
      };
      break;
    }

  }
private:
  bool firsttime;
  FrameBufferLoop *loop;
  int m_format;
  void *buffer;
  int size;
  int width;
  int height;
  int depth;
  double start_time;
  double start_time_epoch;
  float *depth_buffer;
};

GameApi::FBU GameApi::LowFrameBufferApi::low_framebuffer(GameApi::FML ml, int format, int width, int height, int depth)
{
  FrameBufferLoop *loop = find_framemainloop(e,ml);
  return add_framebuffer(e, new LowFrameBuffer(loop, format,width,height,depth));
}
struct EnemyData
{
    int p_x;
    int p_y;
    char c;
    int length;
    float pos;
  int state;
};

class EnemyDraw : public FrameBufferLoop
{
public:
  EnemyDraw(GameApi::Env &e, Bitmap<Color>* shape, std::string url, std::string homepage, int fmt, float speed) : e(e), shape(shape), url(url), homepage(homepage), src(fmt?D_RGBA8888:D_Mono1,0), fmt(fmt?D_RGBA8888:D_Mono1), speed(speed) { }
  virtual void Prepare()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string data = std::string(ptr->begin(), ptr->end());

    std::stringstream ss(data);
    int p_x=0;
    int p_y=0;
    char c;
    int length=0;
    float start_pos=0.0;
    while(ss >> p_x >> p_y >> c >> length >> start_pos)
      {
	EnemyData dt;
	dt.p_x = p_x;
	dt.p_y = p_y;
	dt.c = c;
	dt.length = length;
	dt.pos = start_pos;
	dt.state = 0;
	vec.push_back(dt);
      }
    shape->Prepare();
    BitmapToSourceBitmap(*shape,src, fmt);
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
  }
  virtual void frame(DrawLoopEnv &e)
  {
    WorldBlocks *blk = GetWorld();
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	int p_x = vec[i].p_x;
	int p_y = vec[i].p_y;
	int pe_x = p_x;
	int pe_y = p_y;
	char c = vec[i].c;
	switch(c) {
	case 'l': pe_x-=vec[i].length; break;
	case 'r': pe_x+=vec[i].length; break;
	case 'u': pe_y-=vec[i].length; break;
	case 'd': pe_y+=vec[i].length; break;
	};
	std::pair<Point2d,Point2d> p1 = blk->BlockToWorld(p_x,p_y);
	std::pair<Point2d,Point2d> p2 = blk->BlockToWorld(pe_x,pe_y);
	float f = vec[i].pos;
	float pp_x = (1.0-f)*(p1.first.x+shape->SizeX()/2.0) + f*(p2.second.x-shape->SizeX()/2.0)-shape->SizeX()/2.0;
	float pp_y = ((1.0-f)*p1.second.y + f*p2.second.y)-shape->SizeY();

	// this communicates with collision detection
	// note the memory leakage if collision detection is not used.
	blk->add_enemy(pp_x, pp_x+shape->SizeX(), pp_y, pp_y+shape->SizeY());
	
	if (vec[i].state==0) {
	  f+=fabs(speed/vec[i].length);
	  if (f>1.0) vec[i].state=1;
	} else {
	  f-=fabs(speed/vec[i].length);
	  if (f<0.0) vec[i].state=0;
	}	
	vec[i].pos = f;
	//std::cout << "F:" << f << std::endl;
	e.drawbuffer->draw_sprite(&src, int(pp_x), int(pp_y));
      }
  }
private:
  GameApi::Env &e;
  Bitmap<Color> *shape;
  std::string url;
  std::string homepage;
  SourceBitmap src;
  DrawBufferFormat fmt;
  std::vector<EnemyData> vec;
  float speed;
};

GameApi::FML GameApi::LowFrameBufferApi::low_enemy_draw(BM bm, std::string url, int fmt, float speed)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  return add_framemainloop(e, new EnemyDraw(e, b2, url, gameapi_homepageurl, fmt, speed));
}


class EnemyDraw2 : public FrameBufferLoop
{
public:
  EnemyDraw2(GameApi::Env &e, std::vector<Bitmap<Color>*> shape, std::string url, std::string homepage, int fmt, float speed, int time_delta, int time_duration) : e(e), shape(shape), url(url), homepage(homepage), fmt(fmt?D_RGBA8888:D_Mono1), speed(speed), time_delta(time_delta), time_duration(time_duration) { 
    current_frame=0;
    firsttime = true;
  }
  virtual void Prepare()
  {
    if (firsttime) {
      firsttime = false;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string data = std::string(ptr->begin(), ptr->end());

    std::stringstream ss(data);
    int p_x=0;
    int p_y=0;
    char c;
    int length=0;
    float start_pos=0.0;
    while(ss >> p_x >> p_y >> c >> length >> start_pos)
      {
	EnemyData dt;
	dt.p_x = p_x;
	dt.p_y = p_y;
	dt.c = c;
	dt.length = length;
	dt.pos = start_pos;
	dt.state = 0;
	vec.push_back(dt);
      }
    int s = shape.size();
    for(int i=0;i<s;i++)
      {
	shape[i]->Prepare();
	SourceBitmap src2(fmt?D_RGBA8888:D_Mono1,0);
	src.push_back(src2);
	BitmapToSourceBitmap(*shape[i],src[i], fmt);
      }
    }
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
  }
  virtual void frame(DrawLoopEnv &e)
  {
    current_frame++;
    WorldBlocks *blk = GetWorld();
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	int index = (i/time_delta + current_frame/time_duration)%src.size();
	
	int p_x = vec[i].p_x;
	int p_y = vec[i].p_y;
	int pe_x = p_x;
	int pe_y = p_y;
	char c = vec[i].c;
	switch(c) {
	case 'l': pe_x-=vec[i].length; break;
	case 'r': pe_x+=vec[i].length; break;
	case 'u': pe_y-=vec[i].length; break;
	case 'd': pe_y+=vec[i].length; break;
	};
	std::pair<Point2d,Point2d> p1 = blk->BlockToWorld(p_x,p_y);
	std::pair<Point2d,Point2d> p2 = blk->BlockToWorld(pe_x,pe_y);
	float f = vec[i].pos;
	float pp_x = (1.0-f)*(p1.first.x+shape[index]->SizeX()/2.0) + f*(p2.second.x-shape[index]->SizeX()/2.0)-shape[index]->SizeX()/2.0;
	float pp_y = ((1.0-f)*p1.second.y + f*p2.second.y)-shape[index]->SizeY();

	// this communicates with collision detection
	// note the memory leakage if collision detection is not used.
	blk->add_enemy(pp_x, pp_x+shape[index]->SizeX(), pp_y, pp_y+shape[index]->SizeY());
	
	if (vec[i].state==0) {
	  f+=fabs(speed/vec[i].length);
	  if (f>1.0) vec[i].state=1;
	} else {
	  f-=fabs(speed/vec[i].length);
	  if (f<0.0) vec[i].state=0;
	}	
	vec[i].pos = f;
	//std::cout << "F:" << f << std::endl;
	e.drawbuffer->draw_sprite(&src[index], int(pp_x), int(pp_y));
      }
  }
private:
  GameApi::Env &e;
  std::vector<Bitmap<Color> *> shape;
  std::string url;
  std::string homepage;
  std::vector<SourceBitmap> src;
  DrawBufferFormat fmt;
  std::vector<EnemyData> vec;
  float speed;
  int time_delta, time_duration;
  int current_frame;
  bool firsttime;
};

GameApi::FML GameApi::LowFrameBufferApi::low_enemy_draw2(std::vector<BM> bm, std::string url, int fmt, float speed, int time_delta, int time_duration)
{
  std::vector<Bitmap<Color>*> vec;
  int s = bm.size();
  for(int i=0;i<s;i++){
    BitmapHandle *handle = find_bitmap(e, bm[i]);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    vec.push_back(b2);
  }

  return add_framemainloop(e, new EnemyDraw2(e, vec, url, gameapi_homepageurl, fmt, speed, time_delta, time_duration));
}

class LowCollision : public FrameBufferLoop
{
public:
  LowCollision(FrameBufferLoop *next, float start_x, float end_x, float start_y, float end_y, int key) : next(next), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), key(key) { keep=false; }
  virtual void Prepare() { next->Prepare(); }
  virtual void handle_event(FrameLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    WorldBlocks *blk = GetWorld();
    blk->clear_enemies();

    // it is assumed that this frame() call will populate enemies array
    // usually done by low_enemy_draw
    next->frame(e);

    int es = blk->num_enemies();
    bool collide=false;
    for(int i=0;i<es;i++) {
      int s_x = blk->enemy_start_x(i);
      int e_x = blk->enemy_end_x(i);
      int s_y = blk->enemy_start_y(i);
      int e_y = blk->enemy_end_y(i);
      if (start_x < e_x &&
	  end_x > s_x &&
	  start_y < e_y &&
	  end_y > s_y) collide = true;
    }
    if (!keep && collide)
      { // collision
	FrameLoopEvent ee;
	ee.type = 0x300;
	ee.ch = key;
	ee.cursor_pos = Point(0.0,0.0,0.0);
	ee.button = -1;
	ee.time = e.time;
	for(int i=0;i<9;i++) ee.pin[i]=false;
	next->handle_event(ee);
	ee.type=0x301;
	next->handle_event(ee);
	keep = true;
      }
    if (!collide) keep=false;
  }
private:
  FrameBufferLoop *next;
  float start_x, end_x;
  float start_y, end_y;
  int key;
  bool keep;
};

GameApi::FML GameApi::LowFrameBufferApi::low_collision(FML ml, float start_x, float end_x, float start_y, float end_y, int key)
{ // sends key event if detects collision
  FrameBufferLoop *loop = find_framemainloop(e, ml);
  return add_framemainloop(e, new LowCollision(loop, start_x, end_x, start_y, end_y, key));
}


class LowFrameBitmap : public Bitmap<Color>
{
public:
  LowFrameBitmap(FrameBufferLoop *loop, int sx, int sy) : buf(loop,4,sx,sy,0),sx(sx),sy(sy) {}
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    unsigned int *b = (unsigned int*)buf.Buffer();
    unsigned int c = b[x+y*buf.Width()];
    return Color(c);
  }
  virtual void Prepare()
  {
    buf.Prepare();
    buf.frame();
  }
private:
  LowFrameBuffer buf;
  int sx,sy;
};

GameApi::BM GameApi::LowFrameBufferApi::low_frame_bitmap(FML ml, int sx, int sy)
{
  FrameBufferLoop *loop = find_framemainloop(e, ml);
  Bitmap<Color> *b = new LowFrameBitmap(loop, sx,sy);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}

class LowKeyBMPrepare : public FrameBufferLoop
{
public:
  LowKeyBMPrepare(FrameBufferLoop *loop, Bitmap<Color> *bm, int key, FrameBufferLoop *normal, float duration, FrameBufferLoop *next) : loop(loop), bm(bm),key(key),normal(normal),duration(duration), next(next)
  {
    time = 0.0;
    state = 0;
  }
  virtual void Prepare() { normal->Prepare(); next->Prepare(); }
  virtual void handle_event(FrameLoopEvent &e)
  {
    if (e.type==0x300 && e.ch==key) {
      bm->Prepare();
      loop->Prepare();
      time = e.time;
      state = 1;
    }
    switch(state) {
    case 0: normal->handle_event(e); break;
    case 1: loop->handle_event(e); break;
    case 2: next->handle_event(e); break;
    };

  }
  virtual void frame(DrawLoopEnv &e)
  {
    DrawLoopEnv ee = e;
    ee.time = ee.time-time;

    if (state==1 && ee.time*10.0>duration)
      {
	state=2;
      }
    switch(state) {
    case 0: normal->frame(ee); break;
    case 1: loop->frame(ee); break;
    case 2: next->frame(ee); break;
    };
  }
private:
  FrameBufferLoop *loop;
  Bitmap<Color> *bm;
  float time;
  int key;
  FrameBufferLoop *normal;
  float duration;
  FrameBufferLoop *next;
  int state;
};

GameApi::FML GameApi::LowFrameBufferApi::low_key_bm_prepare(FML ml, BM bm, int key, FML normal, float duration, FML next)
{
  FrameBufferLoop *loop = find_framemainloop(e, ml);
  FrameBufferLoop *nxt = find_framemainloop(e, next);
  FrameBufferLoop *n = find_framemainloop(e, normal);
  Bitmap<Color> *b2 = find_bitmap2(e,bm);
  return add_framemainloop(e, new LowKeyBMPrepare(loop, b2, key,n,duration,nxt));
}

GameApi::FML GameApi::LowFrameBufferApi::low_activate_snapshot(EveryApi &ev, FML ml, int key, MN move, float duration, FML next)
{
  BM I47=ev.low_frame_api.low_frame_bitmap(ml,800,600);
  FML I50=ev.low_frame_api.low_sprite_draw("s_tmp1",I47,move,0,0,1,0.0);
  FML I51=ev.low_frame_api.low_key_bm_prepare(I50, I47, key, ml,duration,next);
  return I51;
}

void nh_cb(void *obj);

HeavyOperation::~HeavyOperation() { }

class NetworkHeavy : public HeavyOperation
{
public:
  NetworkHeavy(GameApi::Env &e, std::string url, std::string homepageurl, HeavyOperation *timing) : e(e), url(url), homepage(homepageurl), timing(timing) { publish_ptr=0;
    ptr = 0;
    current_slot_num = 0;
    //std::cout << "NetworkHeavy:" << url << std::endl;
  }
  virtual bool RequestPrepares() const { return timing->RequestPrepares(); }
  virtual void TriggerPrepares()
  {
    timing->TriggerPrepares();
    ptr = 0;
    current_slot_num = 0;
  }
  virtual int NumPrepares() const
  {
    return 0;
  }
  virtual void Prepare(int prepare)
  {
  }
  virtual int NumSlots() const
  {
    return timing->NumSlots()+current_slot_num+1;
  }
  virtual void Slot(int slot)
  {
    int s = timing->NumSlots();
    if (slot<s) { timing->Slot(slot); return; }
    slot-=s;
    //std::cout << "NetworkHeavy: #" << slot << std::endl;
    if (slot==0) {
      // TODO, how to prevent async_pending_count to show logo
      // start async network access
      static int i = 0;
      std::stringstream ss;
      ss<<"?id=" << i;
      str = ss.str();
      i++;
#ifdef EMSCRIPTEN
      e.async_load_callback(url + str, &nh_cb, this);

      e.async_load_url(url + str, homepage);
#endif
#ifndef EMSCRIPTEN
      Callback();
#endif
      
    }
#ifdef EMSCRIPTEN
    if (!ptr)
      current_slot_num++;
#endif
  }
  virtual void FinishSlots()
  {
    timing->FinishSlots();
  }

  void Callback() {
#ifndef EMSCRIPTEN
    e.async_load_url(url+str, homepage);
#endif
    ptr = e.get_loaded_async_url(url+str);
    publish_ptr = ptr;
    if (!ptr) {std::cout << "Network heavy callback received 0" << std::endl; }
  }

  virtual void* get_data(std::string type)
  {
    if (type=="std::vector<unsigned char>" && publish_ptr) { return publish_ptr; }
    return 0;
  }
  virtual void set_usage_index(int slot)
  {
  }
private:
  GameApi::Env &e;
  std::string url;
  std::string str;
  std::string homepage;
  HeavyOperation *timing;
  int current_slot_num;
  std::vector<unsigned char> *ptr;
  std::vector<unsigned char> *publish_ptr;
};

void nh_cb(void *obj)
{
  NetworkHeavy *h = (NetworkHeavy*)obj;
  h->Callback();
}

class BitmapPrepareHeavy : public HeavyOperation
{
public:
  BitmapPrepareHeavy(GameApi::EveryApi &ev, HeavyOperation *bitmap_gen, int scanlines_per_slot) : ev(ev), bitmap_gen(bitmap_gen), scanlines(scanlines_per_slot) { ref=BufferRef::NewBuffer(1,1); bm=0; id.id=-1; }
  void AllocMem() {
    if (bm) {
      BufferRef::FreeBuffer(ref);
      ref = BufferRef::NewBuffer(bm->SizeX(),bm->SizeY());
    }
  }
  virtual bool RequestPrepares() const { return bitmap_gen->RequestPrepares(); }
  virtual void TriggerPrepares() {
    bitmap_gen->TriggerPrepares();
  }
  virtual int NumPrepares() const
  {
    if (bm) 
      return bm->SizeY();
    return 0;
  }
  virtual void Prepare(int prepare)
  {
    if (bm) {
      int s = bm->SizeX();
      for(int i=0;i<s;i++)
	{
	  ref.buffer[i+ref.ydelta*prepare] = bm->Map(i,prepare).Pixel();
	}
    }
  }
  virtual int NumSlots() const {
    return bitmap_gen->NumSlots()+NumPrepares()/scanlines;
  }
  virtual void Slot(int slot)
  {
    int s = bitmap_gen->NumSlots();
    if (slot<s) return bitmap_gen->Slot(slot);
    bm=(Bitmap<Color>*)bitmap_gen->get_data("Bitmap<Color>");
    if (!bm) { std::cout << "BitmapPrepareHeavyOperation cannot find Bitmap<Color>::0" << std::endl; return; }
    AllocMem();
    slot-=s;
    //std::cout << "BitmapPrepareHeavy: #" << slot << std::endl;
    int start_scanline = slot*scanlines;
    int end_scanline = (slot+1)*scanlines-1;
    if (range_start>start_scanline) { range_start = start_scanline; }
    if (range_end<end_scanline) { range_end = end_scanline; }
    int ss = end_scanline-start_scanline;
    for(int i=0;i<ss;i++)
      {
	Prepare(start_scanline+i);
      }
  }

  virtual void FinishSlots() { bitmap_gen->FinishSlots(); }
  /*
  virtual bool IsReadyForTriggerFrames() const
  {
    if (range_start==100000) return false;
    if (range_end==-1) return false;
    if (bm && range_end<bm.SizeY()) return false;
    if (bm && range_start>0) return false;
    return true;
  }
  virtual void TriggerFrames()
  {
    bitmap_gen->TriggerFrames();
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    bitmap_gen->handle_event(e);
  }
  virtual void draw_event(FrameLoopEvent &e)
  {
    bitmap_gen->draw_event(e);
  }
  virtual void frame(MainLoopEnv &e)
  {
    bitmap_gen->frame(e);
  }
  virtual void draw_frame(DrawLoopEnv &e)
  {
    bitmap_gen->draw_frame(e);
  }
  */
  virtual void* get_data(std::string type)
  {
    if (type=="BufferRef") { return &ref; }
    if (type=="TXID") {
      // note, this call requires opengl.
      id = ev.texture_api.bufferref_to_txid(id,ref);
      return &id;
    }
    return 0;
    //return bitmap_gen->get_data(type);
  }
  void set_usage_index(int slot) { }
private:
  GameApi::EveryApi &ev;
  HeavyOperation *bitmap_gen;
  Bitmap<Color> *bm;
  BufferRef ref;
  int scanlines;
  int range_start = 100000;
  int range_end = -1;
  GameApi::TXID id;
};


#if 1
class ThreadHeavy;
void *thread_heavy_main(void* ptr);

#ifdef THREAD_HEAVY
//#include <thread>
#endif


class ThreadHeavy : public HeavyOperation
{
public:
  ThreadHeavy(HeavyOperation *op) : op(op) { current_slot_num=0; ready=false; }
  virtual bool RequestPrepares() const { return op->RequestPrepares(); }
  virtual void TriggerPrepares() {
#ifdef THREAD_HEAVY
    ready=false;
    current_slot_num=0;
#endif
  }
  virtual int NumPrepares() const { return 0; }
  virtual void Prepare(int prepare) { }
  virtual int NumSlots() const {
#ifdef THREAD_HEAVY
    return current_slot_num+1;
#else
    return op->NumSlots();
#endif
  }
  virtual void Slot(int slot)
  {
    //std::cout << "Thread heavy slot:" << slot << std::endl;
#ifdef THREAD_HEAVY
    if (slot==0) {
      //Callback();
      //g_thread_heavy_ptr = this;
#if 1
      pthread_attr_t attr;
    pthread_t thread_id;
    
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 30000);
    //std::cout << "phread_create" << std::endl;
    pthread_create(&thread_id, &attr, &thread_heavy_main, (void*)this);
    //std::cout << "pthread_create_return: " << val << std::endl;
    pthread_attr_destroy(&attr);
#endif
    //new std::thread(&thread_heavy_main);
    }
    if (!ready)
      current_slot_num++;
#else
    op->Slot(slot);
#endif
  }
  virtual void FinishSlots()
  {
    op->FinishSlots();
  }
  void Callback()
  {
    op->TriggerPrepares();
    int s = op->NumSlots();
    //std::cout << "Callback num: " << s << std::endl;
    for(int i=0;i<s;i++)
      {
	//std::cout << "Callback slot: "<< i << "/" << s << std::endl;
	op->Slot(i);
	s = op->NumSlots();
	//std::cout << "newCallback num: " << s << std::endl;
      }
    ready=true;
  }
  virtual void* get_data(std::string type)
  {
#ifdef THREAD_HEAVY
    if (ready) { return op->get_data(type); }
    return op->get_data(type);
#else
    return op->get_data(type);
#endif
  }
  void set_usage_index(int slot) { }
  
private:
  HeavyOperation *op;
  int current_slot_num;
  bool ready;
};

void* thread_heavy_main(void *ptr2)
{
  ThreadHeavy *ptr = (ThreadHeavy*)ptr2;
  ptr->Callback();
  return 0;
}
#endif

class ArrayHeavy : public HeavyOperation
{
public:
  ArrayHeavy(std::vector<HeavyOperation*> vec) : vec(vec) { }
  virtual bool RequestPrepares() const { return vec[0]->RequestPrepares(); }
  virtual void TriggerPrepares() {
    vec[0]->TriggerPrepares();
    current_i = -1;
  }
  virtual int NumPrepares() const
  {
    return 0;
  }
  virtual void Prepare(int prepare)
  {
  }
  virtual int NumSlots() const {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++) count+=vec[i]->NumSlots();
    return count;
  }
  virtual void Slot(int slot)
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	int s = vec[i]->NumSlots();
	if (slot>=count && slot<count+s) { current_i = i; vec[i]->Slot(slot-oldcount); return; }
	count+=s;
      }
    current_i = s;
    current_slot = slot;
  }
  virtual void FinishSlots()
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->FinishSlots();
  }
  virtual void* get_data(std::string type)
  {
    if (type=="std::vector") {
      res_vec=std::vector<void*>();
      int s = current_i;
      for(int i=0;i<s;i++) {
	res_vec.push_back(vec[i]->get_data(type));
      }
      return &res_vec;
    }
    return 0;
  }
  void set_usage_index(int slot) { }
private:
  std::vector<HeavyOperation*> vec;
  std::vector<void*> res_vec;
  int current_slot;
  int current_i;
};

class PngHeavy : public HeavyOperation
{
public:
  PngHeavy(GameApi::EveryApi &ev, HeavyOperation *data, std::string url, int ssx, int ssy) : ev(ev), data(data),url(url),ssx(ssx), ssy(ssy) { res_ref=BufferRef::NewBuffer(ssx,ssy);
    ref=BufferRef::NewBuffer(1,1);
    id.id=-1; }
  virtual bool RequestPrepares() const { return data->RequestPrepares(); }
  virtual void TriggerPrepares() { data->TriggerPrepares(); }
  virtual int NumPrepares() const { return 0; }
  virtual void Prepare(int prepare) { }
  virtual int NumSlots() const { return data->NumSlots()+4; }
  virtual void Slot(int slot)
  {
    int s = data->NumSlots();
    if (slot<s) { data->Slot(slot); return; }
    slot-=s;
    //std::cout << "PngHeavy: #" << slot << std::endl;
    if (slot==0) {
      // TODO, this slot might be too slow for frame loop
      void *dt = data->get_data("std::vector<unsigned char>");
      if (!dt) { std::cout << "PngHeavy got null pointer" << std::endl; return; }
      std::vector<unsigned char> *ptr = (std::vector<unsigned char>*)dt;      
      bool success=false;
      BufferRef::FreeBuffer(ref);
      ref = LoadImageFromString(*ptr,success);
      if (success) { }
      else std::cout << "PngHeavy failed to parse png!" << std::endl;
    }
    if (slot==1) {
      // flip texture in y-direction
      int sx = ref.width;
      int sy = ref.height;
      for(int y=0;y<sy/2;y++)
	for(int x=0;x<sx;x++)
	  {
	    std::swap(ref.buffer[x+y*ref.ydelta],ref.buffer[x+(sy-y-1)*ref.ydelta]);
	  }

    }
    if (slot==2) {
      // argb -> rgba conversion (not used)
      int sx = ref.width;
      int sy = ref.height;
      for(int y=0;y<sy;y++) {
	//std::cout << std::endl;
	for(int x=0;x<sx;x++) {
	  unsigned int val = ref.buffer[x+y*ref.ydelta];
	  //std::cout <<" "<< std::hex << val;
	  unsigned int a = val&0xff000000;
	  unsigned int r = val&0x00ff0000;
	  unsigned int g = val&0x0000ff00;
	  unsigned int b = val&0x000000ff;
	  a>>=24;
	  r>>=16;
	  g>>=8;
	  
	  a<<=24;
	  b<<=16;
	  g<<=8;
	  val=a+r+g+b;
	  val|=0xff000000;
	  ref.buffer[x+y*ref.ydelta]=val;
	}
      }
      //std::cout << "URL: " << url << " finishes" << std::endl;
    }
    if (slot==3) // resize
      {
      int sx = ref.width;
      int sy = ref.height;

	for(int y=0;y<ssy;y++)
	  for(int x=0;x<ssx;x++)
	    {
	      float xx = float(sx)*x/ssx;
	      float yy = float(sy)*y/ssy;
	      int kx = int(xx);
	      int ky = int(yy);
	      unsigned int val = ref.buffer[kx+ky*ref.ydelta];
	      res_ref.buffer[x+y*res_ref.ydelta] = val;
	    }

      id = ev.texture_api.bufferref_to_txid(id,res_ref);

      }
  }
  virtual void FinishSlots()
  {
    data->FinishSlots();
  }
  virtual void* get_data(std::string type)
  {
    if (type=="BufferRef") return &res_ref;
    if (type=="TXID") {
      // note, this call requires opengl.
      if (id.id==-1) { id = ev.texture_api.bufferref_to_txid(id,res_ref);
	return &id; }


      int sx = res_ref.width;
      int sy = res_ref.height;
      bool power_of_two = true;
      if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	power_of_two = false;
      if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	power_of_two = false;


#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id.id);
  //g_low->ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, res_ref.width,res_ref.height, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, res_ref.buffer);

       g_low->ogl->glTexSubImage2D(Low_GL_TEXTURE_2D, 0, 0,0, res_ref.width,res_ref.height, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, res_ref.buffer);


  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  g_low->ogl->glEnable(Low_GL_DEPTH_TEST);

      return &id;
    }

    return 0;
  }
  void set_usage_index(int slot) { }
private:
  GameApi::EveryApi &ev;
  HeavyOperation *data;
  BufferRef ref;
  BufferRef res_ref;
  GameApi::TXID id;
  std::string url;
  int ssx,ssy;
};

class BitmapHeavy : public HeavyOperation
{
public:
  BitmapHeavy(Bitmap<Color> *bm, HeavyOperation *timing) : bm(bm),timing(timing) { }
  virtual bool RequestPrepares() const { return timing->RequestPrepares(); }
  virtual void TriggerPrepares() { timing->TriggerPrepares(); }
  virtual int NumPrepares() const { return 0; }
  virtual void Prepare(int prepare) { }
  virtual int NumSlots() const { return 0; }
  virtual void Slot(int slot) { }
  virtual void FinishSlots()
  {
    timing->FinishSlots();
  }
  virtual void* get_data(std::string type)
  {
    if (type=="Bitmap<Color>") return bm;
    return 0;
  }
  virtual void set_usage_index(int slot)
  {
  }

private:
  Bitmap<Color> *bm;
  HeavyOperation *timing;
};
class TimingHeavy : public HeavyOperation
{
public:
  TimingHeavy(int numframes) : numframes(numframes) { current_frame=0; }
  virtual bool RequestPrepares() const {
    current_frame++;
    return current_frame>numframes;
  }
  virtual void TriggerPrepares() { current_frame=0;}
  virtual int NumPrepares() const { return 0; }
  virtual void Prepare(int prepare) { }
  virtual int NumSlots() const { return 0; }
  virtual void Slot(int slot) { }
  virtual void FinishSlots() { }
  virtual void* get_data(std::string type) { return 0; }
  virtual void set_usage_index(int slot) { }
private:
  mutable int current_frame;
  int numframes;
};
class MTLParseHeavy : public HeavyOperation
{
public:
  MTLParseHeavy(GameApi::EveryApi &ev, HeavyOperation *mtl_data, std::string url_prefix) : ev(ev), mtl_data(mtl_data), url_prefix(url_prefix) { }
  virtual bool RequestPrepares() const
  {
   return mtl_data->RequestPrepares();
  }
  virtual void TriggerPrepares()
  {
    mtl_data->TriggerPrepares();
  }
  virtual int NumPrepares() const { return 0; }
  virtual void Prepare(int prepare) { }
  virtual int NumSlots() const
  {
    return mtl_data->NumSlots() + 1;
  }
  virtual void Slot(int slot)
  {
    int s = mtl_data->NumSlots();
    if (slot<s) { mtl_data->Slot(slot); return; }
    slot-=s;
    //std::cout << "MTLHeavy #" << slot << std::endl;
    if (slot==0) {
      std::vector<unsigned char> *ptr = (std::vector<unsigned char>*)mtl_data->get_data("std::vector<unsigned char>");
      if (!ptr) { std::cout << "MTLParseHeavy: got null pointer!" << std::endl; return; }
      vec = ev.polygon_api.mtl_parse(ev, *ptr, url_prefix);
    }
  }
  virtual void FinishSlots()
  {
    mtl_data->FinishSlots();
  }
  virtual void* get_data(std::string type)
  {
    if (type=="std::vector<TXID>") {
      return &vec;
    }
    return 0;
  }
  virtual void set_usage_index(int slot) { }

private:
  GameApi::EveryApi &ev;
  HeavyOperation *mtl_data;
  std::vector<GameApi::TXID> vec;
  std::string url_prefix;
};
GameApi::H GameApi::BitmapApi::mtl_heavy(GameApi::EveryApi &ev, H net, std::string url_prefix)
{
  HeavyOperation *op = find_heavy(e, net);
  return add_heavy(e, new MTLParseHeavy(ev, op,url_prefix));
}
GameApi::H GameApi::BitmapApi::timing_heavy(int numframes)
{
  return add_heavy(e, new TimingHeavy(numframes));
}
GameApi::H GameApi::BitmapApi::bitmap_heavy(BM bm, H timing)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  HeavyOperation *op = find_heavy(e, timing);
  return add_heavy(e, new BitmapHeavy(b2,op));
}
GameApi::H GameApi::BitmapApi::png_heavy(EveryApi &ev, H net, std::string url)
{
  HeavyOperation *op = find_heavy(e, net);
  return add_heavy(e, new PngHeavy(ev,op,url,1024,1024));
}
GameApi::H GameApi::BitmapApi::array_heavy(std::vector<H> vec)
{
  int s = vec.size();
  std::vector<HeavyOperation*> hev;
  for(int i=0;i<s;i++)
    {
      hev.push_back(find_heavy(e, vec[i]));
    }
  return add_heavy(e, new ArrayHeavy(hev));
}
#if 1
GameApi::H GameApi::BitmapApi::thread_heavy(H threaded)
{
  HeavyOperation *op = find_heavy(e, threaded);
  return add_heavy(e, new ThreadHeavy(op));
}
#endif
GameApi::H GameApi::BitmapApi::network_heavy(std::string url, std::string homepage, H timing)
{
  HeavyOperation *op = find_heavy(e,timing);
  return add_heavy(e, new NetworkHeavy(e, url, homepage, op));
}
GameApi::H GameApi::BitmapApi::bitmap_prepare_heavy(EveryApi &ev, H bitmap_gen, int scanlines_per_slot)
{
  HeavyOperation *op = find_heavy(e,bitmap_gen);
  return add_heavy(e, new BitmapPrepareHeavy(ev, op, scanlines_per_slot));
}

class HeavyTextureID : public TextureID
{
public:
  HeavyTextureID(HeavyOperation *heavy) : heavy(heavy) { heavycount=0; id.id=-1; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void render(MainLoopEnv &e) {
    if (heavy->RequestPrepares()) { heavycount=0; heavy->TriggerPrepares(); }
    int n = heavy->NumSlots();
    if (heavycount>=n) return;
    //std::cout << "heavy: " << heavycount << " " << n << std::endl;
    heavy->Slot(heavycount);
    heavycount++;
    //TODOif (heavycount>=n) { heavy->FinishSlots(); }
    if (id.id!=-1&&id.id!=0)
      g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id.id);
    g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
  }   
 
  int texture() const
  {
    void* txid = heavy->get_data("TXID");
    if (!txid) { std::cout << "HeavyTextureID got null pointer" << std::endl; return 0; }
    id = *(GameApi::TXID*)txid;
    return id.id;
  }
private:
  HeavyOperation *heavy;
  int heavycount;
  mutable GameApi::TXID id;
};


class HeavyTextureIDArray : public TextureID
{
public:
  HeavyTextureIDArray(HeavyOperation *heavy, int num) : heavy(heavy),num(num) { heavycount=0; id.id=-1; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void render(MainLoopEnv &e) {
    if (heavy->RequestPrepares()) { heavycount=0; heavy->TriggerPrepares(); }
    int n = heavy->NumSlots();
    if (heavycount>=n) return;
    heavy->Slot(heavycount);
    heavycount++;
    if (heavycount>=n) { heavy->FinishSlots(); }
    //if (id.id!=-1)
    //  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id.id);
    //g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
    
  }
  int texture() const
  {
    void* txid = heavy->get_data("std::vector<TXID>");
    if (!txid) { std::cout << "HeavyTextureID got null pointer" << std::endl; return 0; }
    std::vector<GameApi::TXID> *vec = (std::vector<GameApi::TXID>*)txid;
    int s = vec->size();
    if (num>=0 && num<s)
      id = vec->operator[](num);
    else
      {
	std::cout << "TextureIDArray index outside of array" << std::endl;
      }
    return id.id;
  }
private:
  HeavyOperation *heavy;
  int num;
  int heavycount;
  mutable GameApi::TXID id;
};

GameApi::TXID GameApi::BitmapApi::txid_from_heavy(H heavy)
{
  HeavyOperation *op = find_heavy(e, heavy);
  return add_txid(e, new HeavyTextureID(op));
}
// note, modifies the txid vector afterwards

class TXIDArrayMainLoop : public MainLoopItem
{
public:
  TXIDArrayMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, HeavyOperation *op, std::vector<GameApi::TXID> *vec, MainLoopItem *next, int start_range, int end_range) : env(env), ev(ev), op(op), vec(vec),next(next), start_range(start_range), end_range(end_range) {
    heavycount=0; 
  }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (op->RequestPrepares()) { heavycount=0; op->TriggerPrepares(); }
    int n = op->NumSlots();
    if (heavycount<n) {
      //std::cout << "NumSlots: " << heavycount << "/" << n << std::endl;
      op->Slot(heavycount);
      heavycount++;
      if (heavycount>=n) { op->FinishSlots(); }
    }
    
    void *txidarray = op->get_data("std::vector<TXID>");
    if (txidarray) {
      //std::cout << "Found txidarray" << std::endl;
      std::vector<GameApi::TXID> *v = (std::vector<GameApi::TXID>*)txidarray;
      std::vector<GameApi::TXID> vd;
      for(int i=start_range;i<end_range;i++) {
	int sk = v->size();
	if (i>=0 && i<sk)
	  vd.push_back(v->operator[](i));
      }
      *vec = vd;
    }
    int s=vec->size();
    for(int i=0;i<s;i++)
      {
	//if (i<start_range||i>=end_range) continue;
	GameApi::TXID id = vec->operator[](i);
	TextureID *iid = find_txid(env,id);
	iid->render(e);
      }
    //ev.texture_api.use_many(*vec,0);
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
   int s=vec->size();
    for(int i=0;i<s;i++)
      {
	GameApi::TXID id = vec->operator[](i);
	TextureID *iid = find_txid(env,id);
	iid->handle_event(e);
      }
 
    next->handle_event(e);
  }
  virtual int shader_id() {
    return next->shader_id();
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  HeavyOperation *op;
  std::vector<GameApi::TXID> *vec;
  MainLoopItem *next;
  int heavycount;
  int start_range, end_range;
};
// modifies std::vector on the fly, so need to have longer lifetime
GameApi::ML GameApi::BitmapApi::txidarray_from_heavy(GameApi::EveryApi &ev, H heavy, std::vector<TXID> *vec, ML ml, int start_range, int end_range)
{
  HeavyOperation *op = find_heavy(e, heavy);
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new TXIDArrayMainLoop(e,ev,op, vec,item, start_range,end_range));
}

GameApi::TXID GameApi::BitmapApi::dyn_fetch_bitmap(EveryApi &ev, std::string url, int time)
{
  H timing = timing_heavy(time);
  H net = network_heavy(url, gameapi_homepageurl, timing);
  H png = png_heavy(ev,net,url);
  H thr = thread_heavy(png);
  TXID id = txid_from_heavy(thr);
  return id;
}

std::vector<GameApi::TXID> GameApi::BitmapApi::dyn_fetch_mtl(EveryApi &ev, std::string mtl_url, ML ml2)
{
  return std::vector<GameApi::TXID>();
}
extern int g_use_texid_material;
class ManyTextureIDMaterial : public MaterialForward
{
public:
  ManyTextureIDMaterial(GameApi::EveryApi &ev, std::string mtl_url, std::string url_prefix,float mix, int start_range, int end_range) : ev(ev), mtl_url(mtl_url), url_prefix(url_prefix), mix(mix), start_range(start_range), end_range(end_range) {
    g_use_texid_material = 1;
  }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::H timing = ev.bitmap_api.timing_heavy(1000000000);
    GameApi::H net = ev.bitmap_api.network_heavy(mtl_url, gameapi_homepageurl, timing);
    GameApi::H mtl = ev.bitmap_api.mtl_heavy(ev,net, url_prefix);
    // this array gets modified all the time during gameplay
    std::vector<GameApi::TXID> *txids = new std::vector<GameApi::TXID>;

    GameApi::P I10=p;
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture_id(ev,I10,txids);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17, mix);
    GameApi::ML I19 = ev.bitmap_api.txidarray_from_heavy(ev,mtl, txids, I18, start_range, end_range);

    return I19;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::H timing = ev.bitmap_api.timing_heavy(1000000000);
    GameApi::H net = ev.bitmap_api.network_heavy(mtl_url, gameapi_homepageurl, timing);
    GameApi::H mtl = ev.bitmap_api.mtl_heavy(ev,net,url_prefix);
    // this array gets modified all the time during gameplay
    std::vector<GameApi::TXID> *txids = new std::vector<GameApi::TXID>;

    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_id(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    GameApi::ML I19 = ev.bitmap_api.txidarray_from_heavy(ev,mtl, txids, I18, start_range, end_range);
    return I19;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
#if 0
    // NOT WORKING
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    // END OF NOT WORKING
    GameApi::ML I18;
    I18.id = -1;
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
#if 0
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    GameApi::ML I18;
    I18.id = -1;
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::TXID> *bm;
  std::string mtl_url;
  std::string url_prefix;
  float mix;
  int start_range, end_range;
};
GameApi::MT GameApi::MaterialsApi::many_texture_id_material(GameApi::EveryApi &ev, std::string mtl_url, std::string url_prefix, float mix, int start_range, int end_range)
{
  return add_material(e, new ManyTextureIDMaterial(ev,mtl_url, url_prefix, mix, start_range, end_range));
}

class CombineMaterials : public MaterialForward
{
public:
  CombineMaterials(GameApi::EveryApi &ev, Material *mat1, Material *mat2a) : ev(ev), mat1(mat1), mat2a(mat2a) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    int ml1 = mat1->mat(p.id);
    int ml2 = mat2a->mat(p.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{m1,I12});
    return I13;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    int ml1 = mat1->mat_inst(p.id,pts.id);
    int ml2 = mat2a->mat_inst(p.id,pts.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{m1,I12});
    return I13;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {

    int ml1 = mat1->mat_inst2(p.id,pta.id);
    int ml2 = mat2a->mat_inst2(p.id,pta.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{m1,I12});
    return I13;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    int ml1 = mat1->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    int ml2 = mat2a->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{m1,I12});
    return I13;

  }
private:
  GameApi::EveryApi &ev;
  Material *mat1, *mat2a;
};

GameApi::MT GameApi::MaterialsApi::combine_materials(EveryApi &ev, MT mat1, MT mat2)
{
  Material *m1 = find_material(e,mat1);
  Material *m2 = find_material(e,mat2);
  return add_material(e, new CombineMaterials(ev,m1,m2));
}

class SmallWindow : public MainLoopItem
{
public:
  SmallWindow(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *draw, int x, int y, int sx, int sy) : env(env), ev(ev), draw(draw), x(x), y(y), sx(sx), sy(sy) { 
    firsttime = true;
  }
  void Prepare() { draw->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    if (1) {
      screen_x = ev.mainloop_api.get_screen_sx();
      screen_y = ev.mainloop_api.get_screen_sy();
      corner_x = ev.mainloop_api.get_corner_x();
      corner_y = ev.mainloop_api.get_corner_y();
      rect_sx = ev.mainloop_api.get_screen_rect_sx();
      rect_sy = ev.mainloop_api.get_screen_rect_sy();
      firsttime = false;
    }
    GameApi::SH sh_color = { e.sh_color };
    Program *prog = find_shader_program(env, sh_color);
    prog->use(); // 80.0, 10.1, 60000.0
    
    Matrix m = Matrix::Perspective(120.0*double(sy)/double(sx), (double)sx/sy, 10.1, 60000.0);
    //Matrix m = Matrix::Perspective2(-300.0, 300.0, -300.0, 300.0, 1.0, 610.0);
    prog->set_var("in_P", m);
    g_low->ogl->glViewport(corner_x+x,screen_y-corner_y-sy-y, sx, sy);
    draw->execute(e);
    g_low->ogl->glViewport(corner_x,screen_y-corner_y-rect_sy,rect_sx, rect_sy);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    draw->handle_event(e);
  }
  virtual int shader_id() { return draw->shader_id(); }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *draw;
  int x;
  int y;
  int sx;
  int sy;

  int screen_x, screen_y;
  int corner_x, corner_y;
  int rect_sx, rect_sy;
  bool firsttime;
};

GameApi::ML GameApi::MainLoopApi::small_window(EveryApi &ev, ML ml, int x, int y, int sx, int sy)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new SmallWindow(e,ev,item, x,y,sx,sy));
}

GameApi::ML part(GameApi::EveryApi &ev, GameApi::ML ml, float x_trans, float y_rot, int x, int y, int mode=0) {
  GameApi::MN I1=ev.move_api.empty();
  //GameApi::MN I2=ev.move_api.rotatey(I1,y_rot);
  //GameApi::MN I3=ev.move_api.rotatex(I2,z_rot);
  GameApi::MN I3;
  if (mode==0) {
    I3 = ev.move_api.trans2(I1, x_trans,0.0,0.0);
  } else if (mode==2) {
    I3 = ev.move_api.rotatey(I1, y_rot);
    I3 = ev.move_api.trans2(I3, x_trans, 0.0,0.0);
  } else {
    I3 = ev.move_api.rotatey(I1, y_rot);
  }
  GameApi::ML I4=ev.move_api.move_ml(ev,ml,I3,1,10.0);
  //GameApi::ML I5=ev.mainloop_api.small_window(ev,I4,0/*x*512*/,0/*y*256*/,512,256);
  return I4;
}
float part_x_trans(int x, int y, float amount=150.0)
{
  int pos = x+y*5;
  pos-=5*9/2;
  return -pos*(amount/5/9);
}
float part_y_rot(int x, int y, float amount=150.0)
{
  int pos = x+y*5;
  pos-=5*9/2;
  return (-pos)*amount/150.0/5/9*3.14159*2.0/8.0;
}
float part_z_rot(int x, int y)
{
  return 0.0;
  //x-=2;
  //y-=4;
  //return x*3.14159*2.0/2.0/8.0/2.0;
}

std::vector<GameApi::ML> part_line(GameApi::EveryApi &ev, GameApi::ML ml, int line, float amount=150.0, int mode=0)
{
  GameApi::ML ml_0_0 = part(ev,ml, part_x_trans(0,line,amount),part_y_rot(0,line,amount),0,line,mode);
  GameApi::ML ml_1_0 = part(ev,ml, part_x_trans(1,line,amount), part_y_rot(1,line,amount),1,line,mode);
  GameApi::ML ml_2_0 = part(ev,ml, part_x_trans(2,line,amount), part_y_rot(2,line,amount),2,line,mode);
  GameApi::ML ml_3_0 = part(ev,ml, part_x_trans(3,line,amount), part_y_rot(3,line,amount),3,line,mode);
  GameApi::ML ml_4_0 = part(ev,ml,part_x_trans(4,line,amount), part_y_rot(4,line,amount),3,line,mode);
  //GameApi::ML I1=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{ml_0_0,ml_1_0,ml_2_0,ml_3_0});
  std::vector<GameApi::ML> vec;
  vec.push_back(ml_0_0);
  vec.push_back(ml_1_0);
  vec.push_back(ml_2_0);
  vec.push_back(ml_3_0);
  vec.push_back(ml_4_0);
  return vec;
}
std::vector<GameApi::ML> append_vec(std::vector<GameApi::ML> vec, std::vector<GameApi::ML> vec2)
{
  int s = vec2.size();
  for(int i=0;i<s;i++) vec.push_back(vec2[i]);
  return vec;
}

class FilterPrepares : public MainLoopItem
{
public:
  FilterPrepares(MainLoopItem *item) : item(item) { firsttime = true;}
  virtual void Prepare()
  {
    if (firsttime) { item->Prepare(); }
    firsttime = false;
  }
  virtual void execute(MainLoopEnv &e) { 
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
private:
  MainLoopItem *item;
  bool firsttime;
};
GameApi::ML filter_prepares(GameApi::Env &e, GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new FilterPrepares(item));
}

std::vector<GameApi::ML> looking_glass(GameApi::EveryApi &ev, GameApi::ML ml, int amount, int mode)
{
  GameApi::ML ml2 = filter_prepares(ev.get_env(), ml);
  std::vector<GameApi::ML> l0 = part_line(ev,ml2, 0, amount,mode);
  std::vector<GameApi::ML> l1 = part_line(ev,ml2, 1, amount,mode);
  std::vector<GameApi::ML> l2 = part_line(ev,ml2, 2, amount,mode);
  std::vector<GameApi::ML> l3 = part_line(ev,ml2, 3, amount,mode);
  std::vector<GameApi::ML> l4 = part_line(ev,ml2, 4, amount,mode);
  std::vector<GameApi::ML> l5 = part_line(ev,ml2, 5, amount,mode);
  std::vector<GameApi::ML> l6 = part_line(ev,ml2, 6, amount,mode);
  std::vector<GameApi::ML> l7 = part_line(ev,ml2, 7, amount,mode);
  std::vector<GameApi::ML> l8 = part_line(ev,ml2, 8, amount,mode);

  std::vector<GameApi::ML> K1 = append_vec(l0,l1);
  std::vector<GameApi::ML> K2 = append_vec(K1,l2);
  std::vector<GameApi::ML> K3 = append_vec(K2,l3);
  std::vector<GameApi::ML> K4 = append_vec(K3,l4);
  std::vector<GameApi::ML> K5 = append_vec(K4,l5);
  std::vector<GameApi::ML> K6 = append_vec(K5,l6);
  std::vector<GameApi::ML> K7 = append_vec(K6,l7);
  std::vector<GameApi::ML> K8 = append_vec(K7,l8);
  return K8;
}

std::vector<GameApi::TXID> looking_glass_txid(GameApi::EveryApi &ev, GameApi::ML ml, int sx=819, int sy=455, float amount=150.0, int mode=0)
{
  std::vector<GameApi::ML> I9 = looking_glass(ev, ml,amount,mode);
  
  //GameApi::ML I1=ev.mainloop_api.array_ml(I9);

  std::vector<GameApi::TXID> vec;
  int s = I9.size();
  for(int i=0;i<s;i++)
    {
      GameApi::TXID id = ev.fbo_api.fbo_ml(ev,I9[i],sx,sy,false);
      vec.push_back(id);
    }

  return vec;
}

class LookingGlassSharedLibraryUse : public MainLoopItem
{
public:
  LookingGlassSharedLibraryUse(GameApi::Env &e, std::vector<GameApi::TXID> id, int start, int end) : m_e(e), id(id), start(start), end(end) { firsttime = true;}
  void Prepare() {
#ifdef LOOKING_GLASS
      hp_loadLibrary();
      hp_initialize();
    hp_setupQuiltSettings(1);
#endif   
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false; 
      //  pixels = new unsigned char[sx*sy*x*y];
    }
#ifdef LOOKING_GLASS
    g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MIN_FILTER, Low_GL_NEAREST);
    g_low->ogl->glDisable(Low_GL_BLEND);


    int sk = id.size();
    int s = std::min(sk,end);
    int st = std::max(0,start);
    for(int i=st;i<s;i++) {
      TextureID *txid = find_txid(m_e, id[i]);
      txid->render(e);

      g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, txid->texture());
      hp_copyViewToQuilt(i);
      }
    //g_low->ogl->glGetTexImage(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, pixels);
    //g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, hp_quiltTexture);
    // g_low->ogl->glTexSubImage2D(Low_GL_TEXTURE_2D, 0, 0,0, sx*x,sy*y, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, pixels);
    //g_low->ogl->glCopyImageSubData(txid->texture(), Low_GL_TEXTURE_2D, 0,0,0,0, hp_quiltTexture, Low_GL_TEXTURE_2D, 0,0,0,0, 10, 6, 1); // TODO 10,6,1
    hp_drawLightfield();
    g_low->ogl->glEnable(Low_GL_BLEND);
#endif
  }
  virtual void handle_event(MainLoopEvent &e) { 
      TextureID *txid = find_txid(m_e, id[0]);
      txid->handle_event(e);
  }
  virtual int shader_id() { return -1; }
  virtual void destroy() {
#ifdef LOOKING_GLASS
    hp_release();
#endif
  }

private:
  GameApi::Env &m_e;
  bool firsttime;
  std::vector<GameApi::TXID> id;
  //int sx,sy,x,y;
  unsigned char *pixels;
  int start,end;
};

GameApi::ML GameApi::MainLoopApi::looking_glass_full(GameApi::EveryApi &ev, GameApi::ML ml, float amount, int mode, int start, int end)
{
  std::vector<GameApi::TXID> id = looking_glass_txid(ev,ml,819,455,amount,mode);
  return add_main_loop(e, new LookingGlassSharedLibraryUse(e,id,start,end));
}

class SaveFont : public MainLoopItem 
{
public:
  SaveFont(FontInterface *font, std::string chars, std::string filename) : font(font), chars(chars), filename(filename)  
  {
    firsttime = true;
  }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      std::cout << "Saving font file to: " << filename << std::endl;
      std::ofstream ss(filename.c_str());
      ss << chars.size() << std::endl;
      int s = chars.size();
      for(int i=0;i<s;i++)
	{
	  long idx = chars[i];
	  int top = font->Top(idx);
	  int sx = font->SizeX(idx);
	  int sy = font->SizeY(idx);
	  ss << idx << " " << top << " " << sx << " " << sy << std::endl;
	  for(int y=0;y<sy;y++)
	    for(int x=0;x<sx;x++)
	      {
		int val = font->Map(idx,x,y);
		ss << val << " ";
	      }
	  ss << std::endl;
	}
      ss.close();
      firsttime = false;
    }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual int shader_id() { return -1; }
private:
  FontInterface *font;
  std::string chars;
  std::string filename;
  bool firsttime;
};

GameApi::ML GameApi::FontApi::save_font_dump(FI font, std::string chars, std::string filename)
{
  FontInterface *fnt = find_font_interface(e,font);
  return add_main_loop(e, new SaveFont(fnt, chars, filename));
}

class LoadFont : public FontInterface
{
public:
  LoadFont(GameApi::Env &e, std::string url, std::string homepageurl) : e(e), url(url), homepageurl(homepageurl) { firsttime = true; }
  virtual void Prepare() { 
    if (firsttime) {
      firsttime = false;
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepageurl);
#endif
      std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::stringstream ss(s);
      int count = -1;
      long idx = -1;
      int top = -1;
      int sx = -1;
      int sy = -1;
      ss >> count;
      //std::cout << "Loading font: ";
      for(int i=0;i<count;i++)
	{
	  ss >> idx >> top >> sx >> sy;
	  //std::cout << idx;
	  int *ptr = new int[sx*sy];
	  for(int y=0;y<sy;y++) {
	    for(int x=0;x<sx;x++) {
	    int val = 0;
	    ss >> val;
	    ptr[x+y*sx] = val;
	    }
	  }
	  m_idx.push_back(idx);
	  m_top.push_back(top);
	  m_sx.push_back(sx);
	  m_sy.push_back(sy);
	  m_data.push_back(ptr);
	}
      //std::cout << std::endl;
    }
  }
  virtual int Top(long idx) const { 
    int index = find_index(idx);
    if (index!=-1) {
      return m_top[index];
    }
    return 0;
  }
  virtual int SizeX(long idx) const
  {
    int index = find_index(idx);
    if (index!=-1) {
      return m_sx[index];
    }
    return 0;
  }
  virtual int SizeY(long idx) const
  {
    int index = find_index(idx);
    if (index!=-1) {
      return m_sy[index];
    }
    return 0;
  }
  virtual int Map(long idx, int x, int y) const
  {
    int sx = SizeX(idx);
    int sy = SizeY(idx);
    int index = find_index(idx);
    if (index==-1) return 0;
    int *ptr = m_data[index];
    if (x>=0&&x<sx)
      if (y>=0&&y<sy)
	return ptr[x+y*sx];
    return 0;
  }

  int find_index(long idx) const
  {
    int s = m_idx.size();
    for(int i=0;i<s;i++)
      if (m_idx[i]==idx) return i;
    return -1;
  }

private:
  GameApi::Env &e;
  std::string url;
  std::string homepageurl;
  std::vector<long> m_idx;
  std::vector<int> m_top;
  std::vector<int> m_sx;
  std::vector<int> m_sy;
  std::vector<int*> m_data;
  bool firsttime;
};

GameApi::FI GameApi::FontApi::load_font_dump(std::string url)
{
  return add_font_interface(e, new LoadFont(e,url, gameapi_homepageurl));
}

Low_SDL_Surface *init_2nd_display(int scr_x, int scr_y);


extern Low_SDL_Window *sdl_display2_window;
extern Low_SDL_Surface *sdl_display2_framebuffer;
extern Low_SDL_GLContext context_display2;
extern Low_SDL_GLContext g_context;
extern Low_SDL_Window *sdl_window;

class MainLoopSplitter_win32_and_emscripten_display2 : public Splitter
{
public:
  MainLoopSplitter_win32_and_emscripten_display2(GameApi::Env &env, GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : env2(env), code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
    firsttime = true;
  }
  virtual void set_env(GameApi::Env *ei)
  {
    e = ei;
  }
  virtual void set_everyapi(GameApi::EveryApi *evi)
  {
    ev = evi;
  }
  virtual void Init()
  {
    g_low->ogl->glGetIntegerv(Low_GL_VIEWPORT, viewport.viewport);
    surf=init_2nd_display(2560, 1600);
    g_low->ogl->glViewport(0,0,2560, 1600);

    make_current(true);
    score = 0;
    hidden_score = 0;

    Envi_2 &env = envi;
    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;
    env.ev = ev;
    
    GameApi::SH sh = env.ev->shader_api.colour_shader();
    GameApi::SH sh2 = env.ev->shader_api.texture_shader();
    GameApi::SH sh3 = env.ev->shader_api.texture_array_shader();
    
    // rest of the initializations
    env.ev->mainloop_api.init_3d(sh);
    env.ev->mainloop_api.init_3d(sh2);
    env.ev->mainloop_api.init_3d(sh3);
    env.ev->shader_api.use(sh);
    
    GameApi::ML ml = mainloop(*env.ev);
    if (async_pending_count > 0) { env.logo_shown = true; }
    
    env.mainloop = ml;
    
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    env.ev->mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url == "") {
	env.ev->mainloop_api.display_logo(*env.ev);
      } else {
	env.ev->mainloop_api.display_seamless(*env.ev);
      }
    } else {
	env.ev->mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     env.ev->mainloop_api.alpha(true);
     g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
     GameApi::MainLoopApi::Event e;
     while((e = env.ev->mainloop_api.get_event()).last==true)
       {
	 /* this eats all events from queue */
       }
    make_current(false);

  }
  virtual int Iter()
  {
    make_current(true);

    Envi_2 *env = (Envi_2*)&envi;
    //std::cout << "async: " << async_pending_count << std::endl;
    if (async_pending_count > 0 && !async_is_done) { env->logo_shown = true; }
    if (async_pending_count != async_pending_count_previous)
      {
	std::cout << "ASync pending count=" << async_pending_count << std::endl;
	async_pending_count_previous = async_pending_count;
      }
    if (env->logo_shown)
      {
	bool b = false;
	if (gameapi_seamless_url=="") {
	  b = env->ev->mainloop_api.logo_iter();
	} else {
	  b = env->ev->mainloop_api.seamless_iter();
	}
	if (b && async_pending_count==0) { env->logo_shown = false;
	  env->ev->mainloop_api.reset_time();
	  env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
	}
	make_current(false);
	return -1;
      }
    async_is_done = true;

    if (firsttime) {
      MainLoopItem *item = find_main_loop(env->ev->get_env(),code);
      item->Prepare();
      firsttime = false;
    }

    env->ev->mainloop_api.clear_3d(0xff000000);
    
    make_current(false);
    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { 
	  GameApi::ML ml = env->mainloop;
	  MainLoopItem *item = find_main_loop(env2, ml);
	  item->destroy();
	  env->exit = true; 
	  std::cout << "Esc pressed4" << std::endl;
	  make_current(false);
	  return 0; 
	}
#endif
	
	//GameApi::InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
	//					      env->data, env->speed_x, env->speed_y,
	//			   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e);
	
      }
    make_current(true);

    //GameApi::InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
    //						  env->data, env->speed_x, env->speed_y,
    //						  1.0, 1.0*3.14159*2.0/360.0);
    
    GameApi::M mat = env->ev->matrix_api.identity();
    env->ev->shader_api.use(env->color_sh);
    env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->texture_sh);
    env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    
    env->ev->shader_api.use(env->arr_texture_sh);
    env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->color_sh);
    
    GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
    GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
    GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
    
    env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

    if (env->fpscounter)
      env->ev->mainloop_api.fpscounter();
    if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
      {
	std::cout << "Timeout3, exiting" << std::endl;
	env->exit = true;
      }
    
    // swapbuffers
    //env->ev->mainloop_api.swapbuffers();
    g_low->sdl->SDL_GL_SwapWindow(sdl_display2_window);
    g_low->ogl->glGetError();
    make_current(false);
    return -1;
  }
  virtual void Destroy()
  {
    // restore viewport
    g_low->ogl->glViewport(viewport.viewport[0],viewport.viewport[1],viewport.viewport[2],viewport.viewport[3]);

    // this is needed for win32 build in editor
    make_current(true);
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
    make_current(false);

    //g_low->sdl->SDL_FreeSurface(surf);
    //g_low->sdl->SDL_GL_DeleteContext(context_display2);
    g_low->sdl->SDL_DestroyWindow(sdl_display2_window);
    sdl_display2_window = 0;
    context_display2 = 0;


  }
  virtual Splitter* NextState(int code)
  {
    return 0;
  }
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }
  void make_current(bool is_display2)
  {
    if (is_display2) {
      g_low->sdl->SDL_GL_MakeCurrent(sdl_window, NULL);
      g_low->sdl->SDL_GL_MakeCurrent(sdl_display2_window, g_context /*context_display2*/);
    } else {
      g_low->sdl->SDL_GL_MakeCurrent(sdl_display2_window, NULL);
      g_low->sdl->SDL_GL_MakeCurrent(sdl_window, g_context);
    }
  }


private:
  GameApi::Env &env2;
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
  Envi_2 envi;
  Low_SDL_Surface *surf;
  GameApi::FrameBufferApi::vp viewport;
  bool firsttime;
};

EXPORT GameApi::RUN GameApi::BlockerApi::game_window_2nd_display(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{

  Splitter *spl = new MainLoopSplitter_win32_and_emscripten_display2(e,ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_splitter(e, spl);
}

class DragDropArea : public MainLoopItem
{
public:
  DragDropArea(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *mainloop, GameApi::RUN (*fptr)(GameApi::Env &e, GameApi::EveryApi &ev, std::string filename)) : m_e(e), ev(ev), mainloop(mainloop), fptr(fptr) { }
  void Prepare() { mainloop->Prepare(); }
  virtual void execute(MainLoopEnv &e) { 
    mainloop->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    mainloop->handle_event(e);
    if (e.drag_drop_filename != "") {
      std::cout << "DragDropArea::handle_event -- dragdrop event" << std::endl;
      GameApi::RUN r = fptr(m_e,ev,e.drag_drop_filename);
      Splitter *old = splitter_current;
      if (r.id!=0)
	ev.blocker_api.run2(ev,r);
      splitter_current = old;
      // eat all events
      GameApi::MainLoopApi::Event e;
      while((e = ev.mainloop_api.get_event()).last==true);
    }
  }
  virtual int shader_id() { return mainloop->shader_id(); }
  virtual void destroy() { mainloop->destroy(); }
private:
  GameApi::Env &m_e;
  GameApi::EveryApi &ev;
  MainLoopItem *mainloop;
  GameApi::RUN (*fptr)(GameApi::Env &e, GameApi::EveryApi &ev, std::string filename);
};


EXPORT GameApi::ML GameApi::MainLoopApi::drag_drop_area(EveryApi &ev, ML mainloop, GameApi::RUN (*fptr)(Env &e, EveryApi &ev, std::string filename))
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_main_loop(e, new DragDropArea(e,ev,ml,fptr));
}



std::vector<PosDelta> parse_layout(std::string s)
{
  std::vector<PosDelta> pos;
  std::stringstream ss(s);
  std::string line;
  while(std::getline(ss,line)) {
    //std::cout << "parse_layout: line: " << line << std::endl;
    std::vector<std::string> v = parse_sep(line, ',');
    int s = v.size();
    PosDelta d;
    int numopts = 0;
    for(int i=0;i<s;i++) {
      std::string sk = cut_spaces(v[i]);
      //std::cout << "parse_layout: elem: " << sk << std::endl;

      std::vector<std::string> opt = parse_sep(sk, '/');
      int s1 = opt.size();
      std::vector<int> opt_vec;
      for(int o=0;o<s1;o++) {
	std::string sk = cut_spaces(opt[o]);
	//std::cout << "parse_layout: opt: " << sk << std::endl;
	int val = Pos_E;
	std::stringstream ss2(sk);
	if (ss2>>val)
	  opt_vec.push_back(val);
	else
	  opt_vec.push_back(Pos_E);
      }
      if (i==0) d.l = opt_vec;
      if (i==1) d.t = opt_vec;
      if (i==2) d.W = opt_vec;
      if (i==3) d.H = opt_vec;
      if (i==4) d.r = opt_vec;
      if (i==5) d.b = opt_vec;

    }
    pos.push_back(d);
  }
  return pos;
}

void FrmContainerWidget::Prepare()
{
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) { std::cout << "async not ready!" << std::endl; return; }
    std::string code(ptr->begin(), ptr->end());
    pos = parse_layout(code);
    
  int s = wid.size();
  for(int i=0;i<s;i++) {
    wid[i]->Prepare();
  }
  option_num.resize(wid.size());
}

GameApi::FML GameApi::LowFrameBufferApi::w_root(GameApi::EveryApi &ev, GameApi::W wd)
{
  int scrx = ev.mainloop_api.get_screen_width();
  int scry = ev.mainloop_api.get_screen_height();
  FrmWidget *w = find_frm_widget(e, wd);
  return add_framemainloop(e, new FrmRootWidget(w, scrx, scry));
}
GameApi::W GameApi::LowFrameBufferApi::w_layout(std::vector<GameApi::W> vec, std::string url)
{
  std::vector<FrmWidget*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_frm_widget(e, vec[i]));
    }
  return add_frm_widget(e, new FrmContainerWidget(e,vec2, url, gameapi_homepageurl));
}

void FrmContainerWidget::set_pos(int x_, int y_) { 
    FrmWidget::set_pos(x_,y_);
    int s = std::min(wid.size(),pos.size());
    for(int i=0;i<s;i++) {
      int x = x_;
      int y = y_;
      PosDelta &p = pos[i];
      PosOption &o = option_num[i];
      if (p.l[o.l]!=Pos_E) x+=p.l[o.l];
      else x+=w-p.r[o.r]-p.W[o.W];
      if (p.t[o.t]!=Pos_E) y+=p.t[o.t];
      else y+=h-p.b[o.b]-p.H[o.H];
      wid[i]->set_pos(x,y);
    }
  }
void FrmContainerWidget::print_vec(std::vector<int> v) {
    int s = v.size();
    for(int i=0;i<s;i++) { std::cout << v[i] << "/"; }
  }
void FrmContainerWidget::set_size(int w_, int h_) { 
    FrmWidget::set_size(w_,h_);
    set_pos(x,y);
    int s = std::min(wid.size(),pos.size());
    for(int i=0;i<s;i++) {
      PosDelta &p = pos[i];
      PosOption &o = option_num[i];

#if 0
      std::cout << "Layout: ";
      print_vec(p.l);
      std::cout << " ";
      print_vec(p.t);
      std::cout << " ";
      print_vec(p.W);
      std::cout << " ";
      print_vec(p.H);
      std::cout << " ";
      print_vec(p.r);
      std::cout << " ";
      print_vec(p.b);
      std::cout << std::endl;
      std::cout << "Options:" << o.l << " " << o.t << " " << o.W << " " << o.H << " " << o.r << " " << o.b << std::endl;
#endif

      //int x = x_;
      //int y = y_;
      //if (p.l!=Pos_E) x+=p.l;
      //else x+=w_-p.r-p.W;
      //if (p.t!=Pos_E) y+=p.t;
      //else y+=h_-p.b-p.H;
      //wid[i]->set_pos(x,y);
      int w = w_;
      int h = h_;
      if (p.l[o.l]!=Pos_E) w-=p.l[o.l];
      else w-=w_-p.W[o.W]-p.r[o.r];
      if (p.r[o.r]!=Pos_E) w-=p.r[o.r];
      else w-=w_-p.l[o.l]-p.W[o.W];
      if (p.t[o.t]!=Pos_E) h-=p.t[o.t];
      else h-=h_-p.H[o.H]-p.b[o.b];
      if (p.b[o.b]!=Pos_E) h-=p.b[o.b];
      else h-=h_-p.H[o.H]-p.t[o.t];
      wid[i]->set_size(w, h);
    }
  }


class FrmRect : public FrmWidget
{
public:
  FrmRect(unsigned int color) : color(color) { }
  virtual void Prepare() { }
  virtual void handle_event(FrameLoopEvent &e) { }
  virtual void frame(DrawLoopEnv &e)
  {
#if 0
    std::cout << x << " " << y << " " << w << " " << h << std::endl;
#endif
    e.drawbuffer->draw_rect(x,y,w,h,color);
  }

private:
  unsigned int color;
};

GameApi::W GameApi::LowFrameBufferApi::w_rect(unsigned int color)
{
  return add_frm_widget(e, new FrmRect(color));
}

class FrmBitmap : public FrmWidget
{
public:
  FrmBitmap(Bitmap<Color> &bm) : bm(bm), fmt(D_RGBA8888), sbm(fmt,0), buffer(0) { }
  virtual void set_pos(int x_, int y_) { 
    FrmWidget::set_pos(x_,y_);
  }
  virtual void set_size(int w_, int h_) { 
    FrmWidget::set_size(w_,h_);
    Prepare();
  }

  virtual void Prepare() 
  { 
    if (w<1 ||h<1) return;
    delete [] buffer;
    std::cout << "Bitmap size: " << w << "x" << h << std::endl;
    buffer = new unsigned int[w*h];
    int sp_w = bm.SizeX();
    int sp_h = bm.SizeY();
    for(int y=0;y<h;y++)
      for(int x=0;x<w;x++) {
	float xx = float(x)/w*sp_w;
	float yy = float(y)/h*sp_h;
	Color c = bm.Map(xx,yy);
	unsigned int cc = c.Pixel();
	buffer[x+y*w] = cc;
      }
    sbm.set_data(buffer, w,h,w);
  }
  virtual void handle_event(FrameLoopEvent &e) { }
  virtual void frame(DrawLoopEnv &e)
  {
#if 0
    std::cout << x << " " << y << " " << w << " " << h << std::endl;
#endif
    e.drawbuffer->draw_sprite(&sbm, x,y);
  }

private:
  DrawBufferFormat fmt;
  Bitmap<Color> &bm;
  unsigned int *buffer;
  SourceBitmap sbm;
};

GameApi::W GameApi::LowFrameBufferApi::w_bitmap(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);
  return add_frm_widget(e, new FrmBitmap(*bm2));

}

class FrmText : public FrmWidget
{
public:
  FrmText(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::Ft font, std::string str, int x_gap, float baseline_percentage) : env(env), ev(ev), font(font), str(str), x_gap(x_gap), baseline_percentage(baseline_percentage), fmt(D_RGBA8888), sbm(D_RGBA8888,0) { }
  // BM GameApi::FontApi::font_string(Ft font, std::string str, int x_gap)
  virtual void Prepare()
  {
    bm = ev.font_api.font_string(font, str, x_gap);
    BitmapHandle *handle = find_bitmap(env, bm);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    int bm_height = b2->SizeY()*baseline_percentage;
    int area_height = h*baseline_percentage;
    yy = y + area_height - bm_height;
    xx = x;
    BitmapToSourceBitmap(*b2, sbm, fmt);

  }
  virtual void set_label(std::string s) { str=s; Prepare(); }
  virtual void handle_event(FrameLoopEvent &e)
  {
  }
  virtual void frame(DrawLoopEnv &e)
  {
    e.drawbuffer->draw_sprite(&sbm, xx,yy);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::Ft font;
  std::string str;
  int x_gap;
  float baseline_percentage;
  GameApi::BM bm;
  int xx,yy;
  DrawBufferFormat fmt;
  SourceBitmap sbm;
};

GameApi::W GameApi::LowFrameBufferApi::w_text(EveryApi &ev, Ft font, std::string str, int x_gap, float baseline)
{
  return add_frm_widget(e, new FrmText(e,ev,font,str,x_gap, baseline));
}


bool skin_start(std::string s, std::string &url, std::string &name) {
  s = cut_spaces(s);
  if (s.length()<3) return false;
  if (s[0]!='[' || s[1]!='[') return false;
  s = s.substr(2);
  std::stringstream ss(s);
  ss >> name >> url;
  return true;
}
bool skin_end(std::string s) {
  s = cut_spaces(s);
  if (s.length()<2) return false;
  if (s[0]==']' && s[1]==']') return true;
  return false;
}

struct SkinData
{
  std::string name;
  std::string url;
  std::string contents;
};

std::vector<SkinData> g_skin_vec;

void set_skin_data(std::vector<SkinData> vec)
{
  g_skin_vec = vec;

  int s = vec.size();
  for(int i=0;i<s;i++) {
    const char *ptr = vec[i].contents.c_str();
    const unsigned char *ptr2 = (unsigned char*)strdup(ptr);
    const int size = vec[i].contents.size();
    const char *url = vec[i].url.c_str();
    GameApi::append_url_map(url, ptr2, ptr2+size);
  }
}

class SkinDescription : public MainLoopItem
{
public:
  SkinDescription(GameApi::Env &e, MainLoopItem *next, std::string url, std::string homepage) : e(e), next(next), url(url), homepage(homepage) {}
  void Prepare() {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    std::stringstream ss(code);
    std::string line;
    bool mode=0;
    std::string contents;
    std::string url;
    std::string name;

    while(std::getline(ss,line)) {
      if (skin_end(line)) {
	mode = 0;
	SkinData dt;
	dt.name = name;
	dt.url = url;
	dt.contents = contents;
	data.push_back(dt);
	contents="";
      }
      if (mode==1) {
	contents+=line+"\n";
      }
      if (skin_start(line, url, name)) {
	mode = 1;
      }
    }
    set_skin_data(data);
  }
private:
  GameApi::Env &e;
  MainLoopItem *next;
  std::vector<SkinData> data;
  std::string url;
  std::string homepage;
};



bool is_inside_extends(Point p, Extends e)
{
  if (p.x<e.start_x || p.x>e.end_x) return false;
  if (p.y<e.start_y || p.y>e.end_y) return false;
  if (p.z<e.start_z || p.z>e.end_z) return false;
  return true;
}

class QuadTreeImpl : public QuadTree
{
public:
  QuadTreeImpl() { }
  virtual int RootType() const { return 0; }
  virtual Extends RootExtends() const { return g_root_extends; }
  virtual QuadNode *Root() const { return g_root; }
  virtual int NumChildren(QuadNode *n) const { return n->child.size(); }
  virtual QuadNode *Child(int i, QuadNode *n) const { return n->child[i]; }
  virtual Extends ChildExtends(int i, QuadNode *n) const { return n->child_extends[i]; }
  virtual int ChildType(int i, QuadNode *n) const { return 0; }
  virtual QuadNode *Parent(QuadNode *n) const { return n->parent; }
public:
  QuadNode *g_root = 0;
  Extends g_root_extends;
};

class PolygonQuadNode : public QuadNode, public FaceCollection
{
public:
  PolygonQuadNode(QuadTree *tree, QuadNode *parent, Extends e)
  {
    QuadNode::parent = parent;
    if (parent) {
      parent->child.push_back(this);
      parent->child_extends.push_back(e);
    } else {
      QuadTreeImpl *impl = (QuadTreeImpl*)tree;
      impl->g_root = this;
      impl->g_root_extends = e;
    }
  }
  int push_vertex(Vertex v)
  {
    m_vertices.push_back(v.p);
    m_normals.push_back(v.n);
    m_colours.push_back(v.c);
    m_texcoord.push_back(v.tx);
    m_texcoord3.push_back(v.tx3);
    return m_vertices.size()-1;
  }
  int push_face(std::vector<int> face_vertices)
  {
    int s = face_vertices.size();
    m_face_vertex_counts.push_back(s);
    for(int i=0;i<s;i++) {
      m_faces.push_back(face_vertices[i]);
    }
  }

  virtual void Prepare() { }
  virtual int NumFaces() const { return m_face_vertex_counts.size(); }
  virtual int NumPoints(int face) const { return m_face_vertex_counts[face]; }
  virtual Point FacePoint(int face, int point) const
  {
    int v = Vertex(face,point);
    return m_vertices[v];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    int v = Vertex(face,point);
    return m_normals[v];
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int v = Vertex(face,point);
    return m_colours[v];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int v = Vertex(face,point);
    return m_texcoord[v];

  }
  virtual float TexCoord3(int face, int point) const { 
    int v = Vertex(face,point);
    return m_texcoord3[v];
  }

  int Vertex(int face, int point) const {
    int s = m_face_vertex_counts.size();
    int count = 0;
    for(int i=0;i<face;i++)
      {
	count+=m_face_vertex_counts[i];
      }
    return count+point;
  }

public:
  std::vector<Point> m_vertices;
  std::vector<Vector> m_normals;
  std::vector<unsigned int> m_colours;
  std::vector<Point2d> m_texcoord;
  std::vector<float> m_texcoord3;
  std::vector<int> m_face_vertex_counts;
  std::vector<int> m_faces;
};


void QuadTree::push_tri(Vertex v1, Vertex v2, Vertex v3)
  {
    QuadNode *r = Root();
    Extends e = RootExtends();
    bool b = is_inside_extends(v1.p,e) && is_inside_extends(v2.p,e) && is_inside_extends(v3.p,e);
    if (!b) { std::cout << "Triangle outside of extends. skipping." << std::endl; return; }
    while(b) {
      int s = r->child.size();
      for(int i=0;i<s;i++) {
	QuadNode *n = r->child[i];
	Extends e = r->child_extends[i];
	bool b2 = is_inside_extends(v1.p,e) && is_inside_extends(v2.p,e) && is_inside_extends(v3.p,e);
	if (!b2) { b=false; continue; }
	r = n;
	b=true;
	break;
      }
    }
    // here r is the node chosen for this triangle
    PolygonQuadNode *rr = static_cast<PolygonQuadNode*>(r);
    int c1 = rr->push_vertex(v1);
    int c2 = rr->push_vertex(v2);
    int c3 = rr->push_vertex(v3);
    std::vector<int> f;
    f.push_back(c1);
    f.push_back(c2);
    f.push_back(c3);
    rr->push_face(f);
  }
  void QuadTree::push_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4)
  {
    QuadNode *r = Root();
    Extends e = RootExtends();
    bool b = is_inside_extends(v1.p,e) && is_inside_extends(v2.p,e) && is_inside_extends(v3.p,e) && is_inside_extends(v4.p,e);
    if (!b) { std::cout << "Quad outside of extends. skipping." << std::endl; return; }
    while(b) {
      int s = r->child.size();
      for(int i=0;i<s;i++) {
	QuadNode *n = r->child[i];
	Extends e = r->child_extends[i];
	bool b2 = is_inside_extends(v1.p,e) && is_inside_extends(v2.p,e) && is_inside_extends(v3.p,e);
	if (!b2) { b=false; continue; }
	r = n;
	b=true;
	break;
      }
    }
    // here r is the node chosen for this triangle
    PolygonQuadNode *rr = static_cast<PolygonQuadNode*>(r);
    int c1 = rr->push_vertex(v1);
    int c2 = rr->push_vertex(v2);
    int c3 = rr->push_vertex(v3);
    int c4 = rr->push_vertex(v4);
    std::vector<int> f;
    f.push_back(c1);
    f.push_back(c2);
    f.push_back(c3);
    f.push_back(c4);
    rr->push_face(f);
  }
  void QuadTree::push_poly(std::vector<Vertex> vec)
  {
    int sk = vec.size();
    QuadNode *r = Root();
    Extends e = RootExtends();
    bool b = true;
    for(int i=0;i<sk;i++) {
      b= b & is_inside_extends(vec[i].p,e);
    }
    if (!b) { std::cout << "Poly outside of extends. skipping." << std::endl; return; }
    while(b) {
      int s = r->child.size();
      for(int i=0;i<s;i++) {
	QuadNode *n = r->child[i];
	Extends e = r->child_extends[i];

	bool b2 = true;
	for(int k=0;k<sk;k++) {
	  b2= b2 & is_inside_extends(vec[k].p,e);
	}
	if (!b2) { b=false; continue; }
	r = n;
	b = true;
	break;
      }
    }
    // here r is the node chosen for this triangle
    PolygonQuadNode *rr = static_cast<PolygonQuadNode*>(r);
    std::vector<int> f;
    for(int i=0;i<sk;i++)
      f.push_back( rr->push_vertex( vec[i] ) );
    rr->push_face(f);
  }


class QuadTreeEntry : public MainLoopItem
{
public:
  QuadTreeEntry(QuadTree *tree, FaceCollection *coll) : tree(tree), coll(coll) { }
  virtual void Prepare()
  {
    coll->Prepare();
    
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) 
      {
	int p = coll->NumPoints(i);
	std::vector<Vertex> vec;
	for(int j=0;j<s;j++) {
	  Vertex v;
	  v.p = coll->FacePoint(i,j);
	  v.n = coll->PointNormal(i,j);
	  v.c = coll->Color(i,j);
	  v.tx = coll->TexCoord(i,j);
	  v.tx3 = coll->TexCoord3(i,j);
	  vec.push_back(v);
	}
	tree->push_poly(vec);
      }
  }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual int shader_id() { return -1; }
private:
  QuadTree *tree;
  FaceCollection *coll;
};

QuadTree *g_tree = new QuadTreeImpl;

GameApi::ML GameApi::PolygonApi::quad_tree(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_main_loop(e, new QuadTreeEntry(g_tree, coll));
}

class ActivateMainLoopItem : public MainLoopItem
{
public:
  ActivateMainLoopItem(MainLoopItem *next, int time, MainLoopItem *def) : next(next),time(time),def(def) { }
  virtual void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (is_activated) {
      is_activated=false;
      next->Prepare();
      is_finished=true;
    }
    if (is_finished)
      next->execute(e);
    else
      def->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (e.ch=='*' && e.type==0x777 && !is_activated && !is_finished) {
      std::cout << "Activated!" << std::endl;
      is_activated=true;
    }
    if (is_finished) {
      next->handle_event(e);
    } else {
      def->handle_event(e);
    }
  }
  virtual int shader_id() { return next->shader_id(); }
public:
  MainLoopItem *next;
  bool is_activated=false;
  bool is_finished=false;
  int time;
  MainLoopItem *def;
};


extern int no_draw_count;

class SlowActivateArray : public MainLoopItem
{
public:
  SlowActivateArray(std::vector<MainLoopItem*> vec) : vec(vec) { 
    no_draw_count++;
    no_draw=true;
    InstallProgress(vec.size(), "progress", 15);
  }
  virtual void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (activated_num==int(vec.size()) && time>30) {
      int s = vec.size();
      for(int i=0;i<s;i++) {
	MainLoopEnv ee = e;
	vec[i]->execute(ee);
      }
    }
    if (activated_num<int(vec.size())) activated_num++;
    if (no_draw && activated_num==int(vec.size())) time++;
    if (no_draw && activated_num==int(vec.size())&&time>30) { no_draw_count--; no_draw=false; }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->handle_event(e);

    for(int j=done_num;j<=activated_num;j++)
      {
	MainLoopEvent ev = e;
	ev.type=0x777;
	ev.ch='*';
	if (j>=0 && j<int(vec.size()))
	  vec[j]->handle_event(ev);
      }
    done_num = activated_num;
    ProgressBar(vec.size(), int(float(done_num)/vec.size()*15), 15, "progress");
  }
private:
  std::vector<MainLoopItem*> vec;
  int activated_num=-1;
  int done_num=-1;
  bool no_draw=false;
  int time=0;
};

GameApi::ML GameApi::MainLoopApi::activate_item(ML ml, ML def2)
{
  MainLoopItem *item = find_main_loop(e, ml);
  MainLoopItem *def = find_main_loop(e, def2);
  return add_main_loop(e, new ActivateMainLoopItem(item,60,def));
}
GameApi::ML GameApi::MainLoopApi::activate_array(std::vector<ML> vec)
{
  int s = vec.size();
  std::vector<MainLoopItem*> vec2;
  for(int i=0;i<s;i++)
    {
      GameApi::ML ml = vec[i];
      ml = activate_item(ml,vec[0]);
      vec2.push_back(find_main_loop(e, ml));
    }
  return add_main_loop(e, new SlowActivateArray(vec2));
}

class SlowActivateItem : public MainLoopItem
{
public:
  SlowActivateItem(MainLoopItem *item, int i) : item(item),i(i) { 
    activated=false;
    committed=false;
  }
  virtual void Prepare() { 
    item->Prepare();
  }
  virtual void execute(MainLoopEnv &e) { 
    item->execute(e);
    activated=true;
  }
  virtual void handle_event(MainLoopEvent &e) 
  {
    time++;
    if (time>i*60) activated=true;
    item->handle_event(e);
    if (activated && !committed) {
      MainLoopEvent ee = e;
      ee.type=0x777;
      ee.ch='*';
      item->handle_event(ee);
      committed=true;
    }
  }
private:
  MainLoopItem *item;
  bool activated;
  bool committed;
  int time=0;
  int i;
};

GameApi::ML GameApi::MainLoopApi::slow_activate_item(ML ml, int i)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new SlowActivateItem(item,i));
}

GameApi::ARR GameApi::MainLoopApi::activate_arr_arr(std::vector<ML> arr)
{

  ArrayType *array = new ArrayType;
  array->type = 2;
  int s = arr.size();
  for(int i=0;i<s;i++) {
    GameApi::ML ml = arr[i];
    ml=activate_item(ml,arr[0]);
    ml=slow_activate_item(ml,i);
    array->vec.push_back(ml.id);
  }
  return add_array(e,array);
}

#endif // THIRD_PART
