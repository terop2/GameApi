
#ifndef GAMEAPI_HH
#define GAMEAPI_HH

#include <string>

namespace GameApi
{

  struct BM { int id; }; // bitmap
  struct BB { int id; }; // bool bitmap
  struct FB { int id; }; // float bitmap
  struct CBM { int id; }; // continuousbitmap
  struct SP { int id; }; // space
  struct SA { int id; }; // separate
  struct PT { int id; }; // point
  struct V { int id; }; // vector
  struct M { int id; }; // matrix
  struct LN { int id; }; // timeline
  struct RM { int id; }; // room
  struct IS { int id; }; // timed int sequence
  struct O { int id; }; // 3d object via volumes
  struct P { int id; }; // 3d object via polygons
  struct OO { int id; };
  struct PP { int id; };
  struct PA { int id; }; // path in 3d space
  struct LA { int id; }; // loop in 3d space
  struct TN { int id; }; // tree node
  struct SH { int id; }; // shader
  struct C { int id; }; // curve
  struct CO { int id; }; // color
  struct F { int id; }; // function
  struct Ft { int id; }; // font
  struct VF { int id; }; // vectorfield
  struct PPT { int id; }; // physics point
  struct PSP { int id; }; // physics space
  struct MA { int id; };
  //struct M { int id; };
  struct S { int id; }; // surface
  struct Vb { int id; }; // vbo object
  //struct L { int id; }; // lighting
  struct T { int id; }; // time
  struct E { int id; }; // event
  struct L { int id; }; // link
  struct MP { int id; }; // mouse plane
  struct MV { int id; }; // movement
  struct LL { int id; }; // linkage
  struct ME { int id; }; // memory
  struct ST { int id; }; // state
  struct TY { int id; }; // type
  struct PS { int id; }; // powerset
  struct ID { int id; }; // id
  struct Str { int id; }; // string
  struct Fi { int id; }; // file
  struct Op { int id; }; // file operation
  struct D { int id; }; // directory
  struct Ht { int id; }; // html
  struct BF { int id; };
  struct VA { int id; }; // vertex array
  struct VX { int id; }; // voxel
  struct PL { int id; }; // plane
  struct TX { int id; }; // texture
  struct TXID { int id; }; // texture id
  struct TR { int id; }; // time range
  struct VV { int id; }; // time range vertex array
  struct Q { int id; }; // quad texture coordinates
  struct DO { int id; };
  struct PC { int id; };
  struct SV { int id; };

  template<class P, class R>
  class FunctionCb
  {
  public:
    virtual R Map(P p) const=0;
  };

  class EveryApi;

class Env
{
public:
  Env();
  ~Env();
public:
  void *envimpl;
  friend struct EnvImpl;
};

class MainLoopApi
{
public:
  MainLoopApi(Env &e);
  ~MainLoopApi();
  void init(int screen_width = 800, int screen_height = 600);
  void init_3d(int screen_width = 800, int screen_heigth = 600);
  void transfer_sdl_surface(MainLoopApi &orig);
  void clear();
  void clear_3d();
  void switch_to_3d(bool b);
  void alpha(bool enabled);
  void antialias(bool enabled);
  float get_time();
  int get_framenum();
  void swapbuffers();
  struct Event
  {
    int type;
    int ch;
    PT cursor_pos;
  };
  Event get_event();
  void waittof();
  SP screenspace();
private:
  float frame;
  float time;
  Env &e;
  void *priv;
};


class SpriteApi
{
public:
  SpriteApi(Env &e);
  ~SpriteApi();
  void spritepos(BM bm, float x, float y);
  void preparesprite(BM bm, int bbm_choose=-1);

  void rendersprite(BM bm, float x, float y, float mult_x, float mult_y);
  void rendersprite(BM bm, PT pos);
  void rendersprite(BM bm, int bm_choose, float x, float y, float mult_x, float mult_y);
  void rendersprite(BM bm, int bm_choose, PT pos);
  void rendersprite(BM bm, int bm_choose, SP move_space, SP sprite_space, float x, float y);
  void rendersprite(BM bm, int bm_choose, SP move_space, SP sprite_space, PT pos);
  void rendersprite(BM bm, float x, float y, float x1, float y1, float inside_x, float inside_y, float inside_x1, float inside_y1);
  SP spritespace(BM bm);
  PT pixelpos(BM bm, int x, int y);
private:
  void *priv;
  Env &e;
};

class TextureApi
{
public:
  TextureApi(Env &e);
  TX tex_plane(int sx, int sy);
  TX tex_bitmap(BM bm);
  int unique_id();
  TX tex_assign(TX tx, int id, int x, int y, BM bm);
  TX tex_coord(TX tx, int id, int x, int y, int width, int height);
  Q get_tex_coord(TX tx, int id);
  TXID prepare(TX tx);
  void use(TXID tx);
  void unuse(TXID tx);
private:
  Env &e;
  int count;
};

class GridApi
{
public:
  GridApi(Env &e);
  ~GridApi();
  SP gridtilespace(BM tilebm, int sx, int sy, int x, int y);
  SP wholegridspace(int sx, int sy, BM grid);

  void preparegrid(BM tile_bitmap, int tile_choose=-1);
  void rendergrid(BM grid, int grid_choose, float top_x, float top_y);

private:
  void *priv;
  Env &e;
};

class BitmapApi
{
public:
  BitmapApi(Env &e);
  ~BitmapApi();
  BM newbitmap(int sx, int sy);
  BM function(unsigned int (*fptr)(EveryApi &e,int,int, void*), int sx, int sy, void *data);
  BM transform(BM orig, unsigned int (*fptr)(EveryApi &e,int,int,unsigned int, void*), void *data=0);
  BM newintbitmap(char *array, int sx, int sy, int (*fptr)(EveryApi &ev, char));
  BM newcolorbitmap(char *array, int sz, int sy, unsigned int (*fptr)(EveryApi &ev, char));
  BM newtilebitmap(int sx, int sy, int tile_sx, int tile_sy);
  BM loadbitmap(std::string filename);
  BM loadtilebitmap(std::string filename, int tile_sx, int tile_sy);
  BM loadposbitmap(std::string filename);
  BM findtile(BM tile_bitmap, int x, int y);
  BM subbitmap(BM orig, int x, int y, int width, int height);
  BM subbitmapimage(BM orig, int r_start_range, int r_end_range, int g_start_range, int g_end_range, int b_start_range, int b_end_range, unsigned int empty_color);

  BM growbitmap(BM small_orig_bitmap, int l, int t, int r, int b);
  BM blitbitmap(BM bg, BM orig, int x, int y);
  BM anim_array(BM *array, int size);
  BM modify_bitmap(BM orig, BM bm, int x, int y);
  BM interpolate_bitmap(BM orig1, BM orig2, float x); // x=[0..1]
  BM repeat_bitmap(BM orig, int xcount, int ycount);
  BM sample_bitmap(BM orig, float xmult, float ymult, float x, float y);
  //BM bitmapandtypes(BM bm, BM (*fptr)(int)); // bm has ints in it
  SP space(BM bm);

  BM addsubrects(BM orig); // use subbitmap with orig bitmap.
  void savebitmap(BM orig, std::string filename);
  BM mandelbrot(bool julia,
		float start_x, float end_x, // [-2..1]
		float start_y, float end_y, // [-1,1]
		float xx, float yy, // [0,0]
		int sx, int sy,
		int count);
  BM memoize(BM orig);
  int intvalue(BM bm, int x, int y);
  unsigned int colorvalue(BM bm, int x, int y);
  int size_x(BM bm);
  int size_y(BM bm);
private:
  void *priv;
  Env &e;
};

class FontApi
{
public:
  FontApi(Env &e);
  ~FontApi();
  Ft newfont(const char *filename, int sx, int sy);
  BM glyph(Ft font, long idx);
  BM font_string(Ft font, const char *str, int x_gap);
  FB glyph_fb(Ft font, long idx);
  BB glyph_bb(Ft font, long idx);
private:
  void *priv;
  Env &e;
};

class AnimApi
{
public:
  AnimApi(Env &e);
  ~AnimApi();
  IS single(int val, float duration);
  IS single(PT point, float duration);
  IS single(float val, float duration);
  IS line(int val1, int val2, float duration);
  IS line(PT p1, PT p2, float duration);
  IS line(float val1, float val2, float duration);
  IS line_obj(P p, P p2, float duration);

  IS repeat(IS i, int count);
  IS repeat_infinite(IS i);

  int timed_value(IS i, float time);  
  PT timed_value_point(IS i, float time);
  float timed_value_float(IS i, float time);

  int timed_value_repeat_num(IS i, float time);
  P timeline_obj(IS i, float time);
    
  IS seq_line(IS *array, int size);
  IS par_line(IS *array, int arraysize);
  int timeline_int(IS l, int choose_par, float time);
  float timeline_float(IS i, int choose_par, float time);
  PT timeline_point(IS l, int choose_par, float time);
  P timeline_obj(IS i, int choose_par, float time);
private:
  void *priv;
  Env &e;
};

class EventApi
{
public:
  EventApi(Env &e);
  ~EventApi();
  E root_event();
  E timer(E activation_event, float time);
  E key_down_event(int key, E start_time, E end_time);
  E key_up_event(int key, E start_time, E end_time);
  E activate_mouse_plane(E activation_event, MP m);
  E deactivate_mouse_plane(E deactivation_event, MP m);
  E pair(E e1, E e2);
  E array(E *array, int size);
  L polygon(E start, E end, P polygon);
  L polygon(E start, E end, P start_polygon, P end_polygon);
  L bitmap(E start, E end, BM bitmap);
  L bitmap(E start, E end, BM start_bitmap, BM end_bitmap);
  L array(L *array, int size);
  M mouse_plane();
  M mouse_rectangle(MP m, int x, int y, int width, int height);
  M array(M *array, int size);
  MV point(float x, float y, float z);
  MV line(E start, E end, MV start_mv, MV end_mv, int start_choose, int end_choose);
  LL link(L obj, MV point);
  LL link(L obj, ID pointnum, MV point);
  LL array(LL *array, int size);
  ST states(int count_states);
  ST mouse_plane(ST states, int state, M mouse);
  ST activate_event(ST states, int state, E event);
  ST enable_obj(ST states, int state, LL link);
  ST disable_obj(ST states, int state, LL link);
  E state_change(E event, int old_state, int new_state);
  void run_game(ST states, int start_state);
  std::string Serialize(ST states, int start_state);
  ST UnSerialize(std::string s);
private:
  void *priv;
  Env &e;
};


class CurveApi
{
public:
  CurveApi(Env &e);
  ~CurveApi();

  C line(PT p1, PT p2);
  C circle(PT center, float r);
  C linear(PT *array, int size);
  C bezier(PT *array, int size);
  
  C scale(C curve, float mx, float my, float mz);
  C constantspeed(C curve, float speed);
  C compose(C *array, int size);
  C plane(C curve2d, PT pos, PT u_x, PT u_y);
  C change_length(C curve, float length);
  C split(C orig_curve, float start_var, float end_var);


  PT pos(C curve, float p);
  
  void sample(PT *outputarray, int size, C input_curve);
};

class FunctionApi
{
public:
  FunctionApi(Env &e);
  ~FunctionApi();
  F constant(float y);
  F sin();
  F cos();
  F line_from_bm_x(BM bm, int bm_x, float length);
  F line_from_bm_y(BM bm, int bm_y, float length);
  F curve_x(C c);
  F curve_y(C c);
  F curve_z(C c);
  F line(float x, float y, float x2, float y2, float begin_y, float end_y);

  float get_value(F f, float x);

  F change_range(F orig, F replace, float x_start, float x_end);
  F move_x(F orig, float delta);
  F move_y(F orig, float delta);
  F max(F orig, F orig2);
  F min(F orig, F orig2);
  F scale_y(F orig, float old_min_y, float old_max_y, float new_min_y, float new_max_y);

  F deriv(F f);
  F integrate(F f);
  //float pullback(F a_f, F b_f, F p_a, F p_b, F f_p, float t_0, float t_1);
private:
  Env &e;
  void *priv;
};

class PhysicsApi
{
public:
  PPT point(PT pos, PT speed_from_origo, float time, float mass);
  PPT add_force(PPT orig, VF force);
  PPT move(PPT old_pos, C curve, float delta_time);

  VF constant_vectorfield(PT dir_from_origo);
  VF plane_vectorfield(PT pos, PT u_x, PT u_y, PT dir_from_origo_up, PT dir_from_origo_down);
  VF limit_to_area(VF inside, VF outside, O volume);
  VF array(VF *array, int size);

  VF move(VF object, PT pos);
  VF rotatex(VF object, float angle);
  VF rotatey(VF object, float angle);
  VF rotatez(VF object, float angle);
  VF scale(VF object, float sx, float sy, float sz);

  PSP plane(PT p, PT u_x, PT u_y);
  PSP sphere(PT center, float r);
  PSP cube(float start_x, float end_x,
	   float start_y, float end_y,
	   float start_z, float end_z);
  PSP min_array(PSP *array, int size);
  PSP max_array(PSP *array, int size);

  PSP move(PSP object, PT pos);
  PSP rotatex(PSP object, float angle);
  PSP rotatey(PSP object, float angle);
  PSP rotatez(PSP object, float angle);
  PSP scale(PSP object, float sx, float sy, float sz);

  C movement(PPT point, float time_step, float delta);
  float collision_time(C curve, PSP phys_space);
  PT collision_point(C curve, PSP phys_space);
};

class SpaceApi
{
public:
  SpaceApi(Env &e);
  ~SpaceApi();
  SP rotatespace(SP s, float angle);
  SP rotatespace(SP s, PT center, float angle);
  //SP rotatespace(SP s, float center_x, float center_y, float angle);
  SP translatespace(SP s, float x, float y);
  SP scalespace(SP s, float scale_x, float scale_y);
  SP moveorigo(SP s, float x, float y);
  float sizex(SP s);
  float sizey(SP s);
  float origo_x(SP s);
  float origo_y(SP s);
  PT pos(SP sp, float x, float y, float z = 0.0); // x = (0..sizex), y=(0..sizey)
  PT posN(SP sp, float x, float y, float z=0.0); // x = (0..1), y = (0..1)
  float pt_x(PT p);
  float pt_y(PT p);
  float pt_z(PT p);
  PT plus(PT p1, PT p2);
private:
  Env &e;
};

#ifdef USE_ANIM_INSTEAD
class MovementApi
{
public:
  MovementApi(Env &e);
  ~MovementApi();
  //PT pos(float x, float y); // x = (0..1), y = (0..1)
  //PT mix(PT p1, PT p2, float val);
  float dist(PT p1, PT p2);
  LN empty(float time);
  LN line(PT p1, PT p2, float time);
  LN seq_line(LN *array, int arraysize);
  LN par_line(LN *array, int arraysize);
  PT timeline(LN l, int choose_par, float time);
private:
  Env &e;
};
#endif
class WorldApi
{
public:
  WorldApi(Env &e);
  RM bitmap(BM bm, PT tl, PT tr, PT bl, PT br);
  RM horiz(RM r1, RM r2, bool top_or_bottom);
  RM vert(RM r1, RM r2, bool left_or_right);
  RM horiz_array(RM *array, int size);
  RM vert_array(RM *array, int size);
  RM fourdir(RM center, RM r1, RM r2, RM r3, RM r4);
  BM result(RM r);
private:
  Env &e;
};
#if 0
class WorldApi2
{
public:
  WorldApi2(Env &e);
  W initial(PT p, V u_x, V u_y, V u_z);
  W split(W w, int sx, int sy, int sz);
  
};
#endif
class WorldChangesApi
{
public:
  WorldChangesApi(Env &e);
  BM change(BM orig, int x, int y, int val);
  BM change_all(BM orig, int old_val, int new_val);
  BM change_area(BM orig, int x, int y, int width, int height, int old_val, int new_val);
  BM change_area_value(BM orig, int x, int y, int width, int height, int val);
  BM blit(BM orig, int x, int y, BM bm);

private:
  Env &e;
};

class DupApi
{
public:
  DupApi(Env &e);
  ~DupApi();
  BM dup_bm(BM b);
  SP dup_sp(SP s);
  PT dup_pt(PT p);
  LN dup_ln(LN l);
  RM dup_rm(RM r);
  IS dup_is(IS i);
};

class TextApi
{
public:
  TextApi(BitmapApi &bm, SpriteApi &sp) : bm(bm), sp(sp), priv(0) { }
  ~TextApi();
  void load_font(std::string filename, int sx, int sy, int x, int y, char start_char, char end_char);
  void draw_text(std::string text, int x, int y);
private:
  BitmapApi &bm;
  SpriteApi &sp;
  void *priv;
  int sx,sy;
  char start_char, end_char;
};

class VolumeApi
{
public:
  VolumeApi(Env &e);
  ~VolumeApi();
  O boolfunction(bool (*fptr)(EveryApi &e,float x, float y, float z, void *data), void *data=0);
  O subvolume(float (*fptr)(EveryApi &e,float x, float y, float z, void *data), void *data, float start_range, float end_range);
  O link_areas(O o, PT p1, PT p2, float d);
  O sphere(PT center, float radius);
  O cube(float start_x, float end_x, 
	 float start_y, float end_y,
	 float start_z, float end_z);
  O cone(PT p1, PT p2, float rad1, float rad2);
  O torus(PT center, PT u_x, PT u_y, float dist1, float dist2);

  O colour(O object, int r, int g, int b, int a);
  O reflect(O object, float val); // val = [0..1]
  O cubetexture(O object, PT origo, PT u_x, PT u_y, PT u_z, BM bm);
  O spheretexture(O object, PT center, BM texture, float multx, float multy);

  O move(O object, PT pos);
  O rotatex(O object, float angle);
  O rotatey(O object, float angle);
  O rotatez(O object, float angle);
  O scale(O object, float sx, float sy, float sz);

  O min_op(O object1, O object2);
  O max_op(O object1, O object2);
  O andnot_op(O object1, O object2);

  BB plane(O o, int sx, int sy,
	   PT u_p, V u_x, V u_y,
	   float start_x, float end_x,
	   float start_y, float end_y,
	   float start_z, float end_z);

  BM render(O object, int sx, int sy, PT ray_0, PT ray_x, PT ray_y, PT ray_z);
  typedef P (*fptrtype)(EveryApi &api,
			float start_x, float end_x, 
			float start_y, float end_y, 
			float start_z, float end_z, 
			unsigned int color, 
			void *data);
  P rendercubes(O object,
		fptrtype fptr,
		void *data, 
		int size,
		float wholesize); // marching cubes algo
  void find_surface(O object, PT p1, PT p2, PT *res1, PT *res2, int level);
  // use RayTracingBitmap class in graph.hh
  // problem1: float values in O. (currently uses bool)
  // problem2: colors in O.
  // problem3: conversion from volumeobject to continuousvoxel<float> and continuousvoxel<color> (something like that exists already)
  // BM raytrace(O volume, int sx, int sy, V v, float z);
private:
  Env &e;
};

class SeparateApi
{
public:
  SeparateApi(Env &e) :e(e) { }
  SA empty();
  SA u_sep(SA orig, float (*sep_u)(float x, float y, float z, void *data), void *data);
  SA v_sep(SA orig, float (*sep_v)(float x, float y, float z, void *data), void *data);
  SA surf_sep(SA orig, float (*sep_dist)(float x, float y, float z, void *data), void *data);
  P create_quads(SA object, float dist, float dist_accuracy,
		 float x_range_start, float x_range_end, float x_step,
		 float y_range_start, float y_range_end, float y_step,
		 float z_range_start, float z_range_end, float z_step,
		 float u_range_start, float u_range_end, float u_step,
		 float v_range_start, float v_range_end, float v_step);

private:
  Env &e;
};

class DistanceApi
{
public:
  DO sphere(float x, float y, float z);
  DO plane(float x, float y, float z,
	   float dx, float dy, float dz);
  DO array(DO *array, int size);
};

class SurfaceApi
{
public:
  SurfaceApi(Env &e);
  ~SurfaceApi();
  S plane(PT pos, PT u_x, PT u_y);
  S sphere(PT center, float radius);
  S torus(PT center, float dist1, float dist2, PT pos, PT u_x, PT u_y);
  S interpolate(S s1, S s2, float val); // val = [0..1]

  S betweensurface(C curve1, C curve2);

  S bitmapsphere(PT center, float radius0, float radius1, BM height);
  
  S color(S orig, unsigned int color);
  S texture(S orig, BM texture); // see bitmaptexturesurface from graph.hh

  BM render(S surf, int sx, int sy, PT ray_0, PT ray_x, PT ray_y, PT ray_z);
private:
  void *priv;
  Env &e;
};

class PolygonApi
{
public:
  PolygonApi(Env &e);
  ~PolygonApi();
  

  P empty();
  P line(PT p1, PT p2);
  P triangle(PT p1, PT p2, PT p3);
  P quad(PT p1, PT p2, PT p3, PT p4);
  P quad_x(float x,
	   float y1, float y2,
	   float z1, float z2);
  P quad_y(float x1, float x2,
	   float y,
	   float z1, float z2);
  P quad_z(float x1, float x2,
	   float y1, float y2,
	   float z);
  P polygon(PT *array, int size); // use render_dynamic with this.
  P tri_vertex_array(float *v_array, int v_size,
		     float *n_array, int n_size,
		     unsigned int *c_array, int c_size,
		     float *tex_array, int tex_size,
		     float **attrib_array, int a_size1, int a_size2);
  P cube(float start_x, float end_x, 
	 float start_y, float end_y,
	 float start_z, float end_z);
  P cube(PT *p); // 8 points needed
  P sphere(PT center, float radius, int numfaces1, int numfaces2);
  P cone(int numfaces, PT p1, PT p2, float rad1, float rad2);
  P ring(float sx, float sy, float x, int steps); // use RingEffect::Init() to implement
  enum HeightMapType { EQuad, ETriangle };
  P heightmap(BM bm, HeightMapType t,
	      float min_x, float max_x, 
	      float min_y, float max_y,
	      float min_z, float max_z);
  P fromsurface(S s, float thickness);
  P fromsurface(S s1, S s2, C curve); // surfacebetweensurfaces
  
  P sprite_bind(P p, TX tx, int id);
  P sprite_bind(P p, Q bm, TX tx);
  P texture(P orig, BM bm, int bm_choose=-1); // all quads

  P color(P orig, unsigned int color);
  P color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z);
  P texcoord_cube(P orig, 
		  PT o, PT u_x, PT u_y, PT u_z,  // these are 3d
		  PT tex_o, PT tex_x, PT tex_y, PT tex_z); // tex_* are 2d
  P color_cube(P orig,
	       PT o, PT u_x, PT u_y, PT u_z,
	       unsigned int color_o, unsigned int color_x, unsigned int color_y, unsigned int color_z);
  P color_faces(P orig,
		unsigned int color_1, unsigned int color_2,
		unsigned int color_3, unsigned int color_4);

  P texcoord_poly(P orig, int facenum, PT *array, int size);
  P color_poly(P orig, int facenum, unsigned int *array, int size);

  P or_elem(P p1, P p2);
  P or_array(P *array, int size); 
  //P and_not_elem(P p1, P p_not);

  P grid(PT o, PT u_x, PT u_y, int num_x, int num_y);
  P grid(PT o, PT u_x, PT u_y, int num_x, int num_y, P *grid);


  P translate(P orig, float dx, float dy, float dz);
  P rotatex(P orig, float angle);
  P rotatey(P orig, float angle);
  P rotatez(P orig, float angle);
  P rotate(P orig, PT pt, V axis, float angle);
  P scale(P orig, float sx, float sy, float sz);

  L color_lighting(float dx, float dy, float dz, 
		   int r, int g, int b, int a);
  L radial_lighting(float dx, float dy, float dz,
		    int r, int g, int b, int a,
		    int r2, int g2, int b2, int a2,
		    float dist);
  L array_lighting(L *array, int size);
  P lighting(P orig, L lighting);
  BM light_bm(L lighting, P poly);
  P single_texture(P orig, BM texture);

  P anim_array(P *array, int size); // OLD

  P splitquads(P orig, int x_count, int y_count);
  P change_positions(P orig, PT (*fptr)(EveryApi &e, PT p, int face, int point, void* data), void *data=0);
  P change_normals(P orig, V (*fptr)(EveryApi &e, V orig, int face, int point, void *data), void *data=0);
  //P change_attrib(P orig, float (*fptr)(float orig, int face, int point, void *data), void *data=0);
  //P change_attribI(P orig, int (*fptr)(int orig, int face, int point, void *data), void *data=0);
  P change_colors(P orig, unsigned int (*fptr)(EveryApi &e, unsigned int orig, int face, int point, void *data), void *data=0);
  P change_texture(P orig, int (*fptr)(EveryApi &e, int face, void *data), BM *array, int size, void *data);

  P recalculate_normals(P orig);
  P memoize(P orig);
  P memoize_all(P orig);

  ID find_point_id(P p, int facenum, int pointnum); // save id's to ENV.
  ID find_normal_id(P p, int facenum, int pointnum);
  ID find_color_id(P p, int facenum, int pointnum);
  ID find_texcoord_id(P p, int facenum, int pointnum);
  ID id_array(ID *array, int size);
  void preparepoly(P p, int bbm_choose=-1);
  void renderpoly(P p, float x, float y, float z);
  void renderpoly(P p, PT pos);
  void renderpoly(P p, int choose, float x, float y, float z);
  void prepare(P p, int bbm_choose=-1);
  void render(P p, int choose, float x, float y, float z);
  
  VA create_vertex_array(P p); // slow
  void render_vertex_array(VA va); // fast

  // these calls require vertex shader.
  P anim_target_vector(P p, V v);
  P anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z);
  P anim_target_matrix(P p, M matrix);


  P counts(P p1, int numfaces);
  P count_function(P p1, int (*numpoints)(EveryApi &e, int face, void *data), void *data);
  P point_function(P p1, PT (*fptr)(EveryApi &e, int face, int point, void *data), void *data);
  P color_function(P p1, unsigned int (*fptr)(EveryApi &e, int face, int point, void *data), void *data);
  P texcoord_function(P p1, PT (*fptr)(EveryApi &e, int face, int point, void *data), void *data);
  P normal_function(P p1, V (*fptr)(EveryApi &e, int face, int point, void *data), void *data);
  P attrib_function(P p1, float (*fptr)(EveryApi &e, int face, int point, int idx, void *data), int idx, void *data);
  P attribi_function(P p1, int (*fptr)(EveryApi &e, int face, int point, int idx, void *data), int idx, void *data);


  // must call prepare for P before these.
  int get_tri_vertex_array_frames(P p);
  int get_tri_vertex_array_rows(P p);
  void get_tri_vertex_array(P p, int choose, int row,
			    int *v_size, float **v_array,
			    int *n_size, float **n_array,
			    int *c_size, unsigned int **c_array,
			    int *tex_size, float **tex_array,
			    int *attrib_size1, int *attrib_size2, float ***attrib_array);

  // this is for better animations, separates modified and non-modified parts
  P create_static_geometry(P *array, int size);
  P create_dynamic_geometry(P *array, int size);
  void render_dynamic(P p, int array_elem, bool textures); // use memoize_all for p before calling this.

  BM renderpolytobitmap(P p, float x, float y, float z, int sx, int sy);
private:
  void *priv;
  Env &e;
};

class ShaderApi;

class StateChangeApi
{
public:
  StateChangeApi(Env &e, ShaderApi &api);
  TR init(int paths);
  TR linear(TR s, int path_num, P (*fptr)(EveryApi &e, float val, void *cb), float start_v, float end_v, float duration, void *cb=NULL);
  VV prepare(TR sc);
  void render(VV sc, float time, SH shadero);
private:
  Env &e;
  ShaderApi &api;
};

class PlaneApi
{ // 2d coordinates in PT
  // could be array of pointcollections
  // but colours etc and textures still needed.
  // so really it's P type with 2d points.
  // int->PT
public:
  PlaneApi(Env &e);
  PL function(PT (*fptr)(EveryApi &e, int idx, void *data), int num_points, void *data);
  PL color_function(PL pl, CO (*fptr)(EveryApi &ev, int idx, PT pos, void *data), void *data);
  // TODO: how to represent/load fonts to this type.
  PL empty(float sx, float sy);
  PL circle(PL bg, PT center, float radius, int numpoints); 
  PL rectangle(PL bg, PT p1, PT p2);
  PL rectangle(PL bg, PT p1, PT p2, PT p3, PT p4);
  PL sprite(PL bg, BM bitmap, PT pos, float size_x, float size_y);
  PL polygon(PL bg, PT *points, int size);
  PL color(PL orig, unsigned int new_color);
  PL plane_elem(PL p1, PL p2);
  PL plane_array(PL *array, int size);
  PL move_plane(PL p1, V delta);

  PL and_not(PL p1, PL not_p); // needed in fonts for the holes
                               // draw to bitmap, do and_not, put to texture

  PL render_p(P p);

  // 1) get black bitmap
  // 2) draw white polygon
  // 3) draw more black
  // 4) blit to screen, take only white pixels
  // PRoblem: How to change from black pixels to alpha pixels?

  // TODO: bezier curves in 2d.

  P substitute_quads_with_plane(P orig, PL (*fptr)(EveryApi &ev, int face, void *data), void *data);
  P plane_in_3d(PL plane, PT u_p, V v1, V v2);
  
private:
  Env &e;
};

#if 0
class SpaceApi2
{
public:
  SpaceApi2(Env &e) : e(e) { }
  P2 point(PT p); // zero dimensions
  P2 line(P2 p1, P2 p2); // N+1 dimensions, p1 and p2 must have same number of dims
  
  PT get_point(P2 p, float *array); // array must have same number of floats than P2 has dimensions
  P2 get_lower_dim(P2 p, float value, int dim); // dim = [0..dims-1]
  
private:
  Env &e;
};
#endif

class SubstitutionApi
{
public:
  SubstitutionApi(EveryApi &e) : e(e) { }
  P ReplacePolygons(P p, P (*fptr)(EveryApi &ev, int index, PT *array, int size, void *data), void *data);
  P ReplacePoints(P p, P (*fptr)(EveryApi &ev, int index, PT point, void *data), void *data);
  //P2 ReplaceLines(P p, C (*fptr)(PT p1, PT p2, void *data), void *data);
  

private:
  EveryApi &e;
};


#if 0
class ChangeApi
{
public:
  // T = event range
  T single_event(E activation_event, float duration);
  T start_end(E activation_event, E end_event);
  T repeat(E activation_event, float duration, int number_of_times);
  T repeat(E activation_event, E end_event, int number_of_times);
  T repeat_infinite(E activation_event, float duration);
  T repeat_infinite(E activation_event, E end_event);
  SC change_variable(V var, int delta,
  SC stop_execution(DP p); 
  DP polygon(P p);
  DP sprite(BM p);
  DP rotate(float angle, V rotate_axis, T event_range, DP object);
  DP rotate(float angle, V rotate_axis, T event_range, DP object, V center_point_of_rotation); // implement using matrix translate*rotate*translate
		     
  //DP translate(V direction_vector, T event_range, DP object);
  DP translate(V direction_vector, T event_range, SC state_change_for_range);
  DP translate(PT destination_point, T event_range, DP object);
  DP translate(PT point, PT point2, T event_range, DP object);
  DP translate(PT point, V vector, T event_range, DP object);
  DP translate(
  DP or_elem(DP o1, DP o2);
  DP or_array(DP *o1, int size);
};

class Object3dApi
{
public:
  OO rect(float start_x, float start_y, float end_x, float end_y);
  OO circle(PT center, PT u_x, PT u_y, float radius, int numpoints);
  OO tri(PT p1, PT p2, PT p3);

  OO range(OO o, bool choose, int start_point, int end_point);
  OO concat(OO o1, OO o2);

  P face(OO o);

  PP line(OO o, float dx, float dy, float dz);
  PP scale(OO o, PT center, float sx, float sy, float sz);
  PP cylindar_rotate(OO o, PT center_1, PT center2, float angle);
  PP sphere_rotate(OO o, PT center, float angle1, float angle2);
  

  OO curve(PP p);
  
  PP array(PP *array, int size);
  

  P side_faces(PP obj);
  P array(P *array, int size);
};
#endif


class BufferApi
{ // P->BF
public:
  BF vertex(P p);
  BF normal(P p);
  BF color(P p);
  BF texcoord(P p);
  BF attrib(P p, int id);

  BF compose(BF b1, BF b2);
  BF compose_array(BF *array, int size);
  BF anim(BF b1, BF b2, float val);
  BF anim_array(BF *array, float *val, int size, float time);
  
  void render(BF *vertex, BF *normal=0, BF *color=0, BF *texcoord=0, BF *attrib_array=0, int attrib_size=0);
};

class BoolBitmapApi
{ // NxN->2
public:
  BoolBitmapApi(Env &e);
  ~BoolBitmapApi();
  BB empty(int sx, int sy);
  BB function(bool (*fptr)(EveryApi &ev, int,int,void*), int sx, int sy, void* data=0);
  BB transform(BB orig, bool (*fptr)(EveryApi &ev, int,int,bool, void*), void *data=0);
  BB from_bitmaps_color(BM bm, int r, int g, int b);
  BB from_bitmaps_color_area(BM bm, bool(*fptr)(EveryApi &ev, int r, int g, int b, int a, void* ptr), void *ptr);

  BB circle(BB bg, float center_x, float center_y, float radius);
  BB rectangle(BB bg, int x, int y, int width, int height); // for static ones
  BB rectangle(BB bg, float x, float y, float width, float height); // for moving
  BB sprite(BB bg, BB sprite, float x, float y, float size_multiplier_x, float size_multiplier_y);
  BB polygon(BB bg, PT *points, int size);
  BB text(BB bg, int x, int y, const char *string, int size, 
	  BB *glyphs, int glyphcount, int(*fptr)(EveryApi &ev, char));
  
  BB not_bitmap(BB b);
  BB or_bitmap(BB b1, BB b2);
  BB andnot_bitmap(BB b1, BB not_b2);
  BB xor_bitmap(BB b1, BB flip_b2);
  
  BM to_bitmap(BB bools, 
	       int true_r, int true_g, int true_b, int true_a,
	       int false_r, int false_g, int false_b, int false_a);
  BM texture(BM bg, 
	     BB bools1, int l1, int t1,
	     BM texturebitmap2, int l2, int t2);
  int size_x(BB bm);
  int size_y(BB bm);
  bool boolvalue(BB bb, int x, int y);
private:
  Env &e;
};

class FloatBitmapApi
{ // NxN->R
public: // values are [0.0..1.0]
  FloatBitmapApi(Env &e);
  ~FloatBitmapApi();
  FB empty(int sx, int sy);
  FB function(float (*fptr)(EveryApi &ev, int,int, void*), int sx, int sy, void* data=0);
  FB from_bool_bitmap(BB bm, int csx, int csy);
  FB grayscale(BM color_bm);
  FB from_red(BM color_bm);
  FB from_green(BM color_bm);
  FB from_blue(BM color_bm);
  FB from_alpha(BM color_bm);

  FB min_fb(FB fb1, FB fb2);
  FB max_fb(FB fb1, FB fb2);

  FB mix_fb(FB fb1, FB fb2, float val);
  
  BM to_grayscale(FB fb);
  BM to_grayscale_color(FB fb, 
			int r, int g, int b, int a,
			int r2, int g2, int b2, int a2);
  BM to_color(FB r, FB g, FB b, FB a);

  FB from_bool(BB b, float val_true, float val_false);
  int size_x(FB bm);
  int size_y(FB bm);
  float floatvalue(FB bm, int x, int y);
private:
  Env &e;
};

class ContinuousBitmapApi
{ // RxR->RGB
public:
  ContinuousBitmapApi(Env &e);
  CBM empty(float x, float y);
  CBM constant(unsigned int color, float x, float y);
  CBM function(unsigned int (*fptr)(EveryApi &ev, float,float, void*), float sx, float sy, void *data);
  BM sample(CBM c_bitmap, int sx, int sy); 
  CBM from_bitmap(BM bm, float xsize, float ysize);
  BM to_bitmap(CBM bm, int sx, int sy);

private:
  Env &e;
};

class VoxelApi
{ // we don't have good ways to render these. RayTraceBitmap is one way,
  // but it could be useful in other rendering systems to get colours from
  // 3d space.
  // NxNxN->RGB
public:
  VoxelApi(Env &e);
  VX function(unsigned int (*fptr)(EveryApi &ev, int x, int y, int z, void *data), int sx, int sy, int sz, void *data);
  unsigned int get_pixel(VX v, int x, int y, int z);
  BM sw_rays(O volume, VX colours, int sx, int sy, float vx, float vy, float vz, float z);
private:
  Env &e;
};


class ColorApi
{ // ()->RGB
public:
  ColorApi(Env &e);
  CO rgb_color(int r, int g, int b, int a); // r,g,b,a [0..255]
  CO rgbf_color(float r, float g, float b, float a);
private:
  Env &e;
};

class PointApi
{ // ()->PT
public:
  PointApi(Env &e);
  PT origo();
  PT point(float x, float y, float z=0.0);
  PT move(PT p1, float dx, float dy, float dz=0.0);
  PT move(PT p1, V vec);

  PT mix(PT p1, PT p2, float val); // val=[0.0..1.0]

  PT from_angle(float radius, float angle);
  PT from_angle(PT center, float radius, float angle);

  float pt_x(PT p);
  float pt_y(PT p);
  float pt_z(PT p);
  float dist3d(PT p, PT p2);
  float dist2d(PT p, PT p2);
  V minus(PT p1, PT p2);
private:
  Env &e;
};
		     
class PointCollectionApi
{ // int -> PT
public:
  PointCollectionApi(Env &e) : e(e) { }
  PC empty();
  PC single(PT point);
  PC single(float x, float y, float z);
  PC array(PT *array, int size);
  P tri_object3d(PC p);
private:
  Env &e;
};


class VectorApi
{ // to be implemented via virtual Vector vec() const=0;
  // ()->V
public:
  VectorApi(Env &e);
  V null_vector();
  V vector(float delta_x, float delta_y, float delta_z);
  V sum(V v1, V v2);
  V mul(V v1, float scalar);
  float dot(V v1, V v2);
  V cross(V v1, V v2);
  float projection_length(V u, V u_x);
  V projection_1(V u, V u_x);
  V projection_2(V u, V u_x);
  V neg(V v);
  float dist3d(V v);
  float dist2d(V v);
private:
  Env &e;
};

class SpaceVectorApi
{ // f : PT->V
public:
  SpaceVectorApi(Env &e) : e(e) { }
  SV function(V (*fptr)(EveryApi &e, float x, float y, float z, void* data), void *data); // TODO
  SV from_points(PC coll); // choose poly(nearest points), linear interpoate, ensure no failures
  PT flow_next_point(SV v, PT p, float mult);
private:
  Env &e;
};

class EquationApi
{
public:
  BB equal(FB bm1, FB bm2, float accuracy);
  BB isocurve(FB bm1, float start_range, float end_range);
  
};

class MatrixApi
{ // to be implemented with virtual Matrix get_matrix() const=0;
public:
  MatrixApi(Env &e);
  M identity();
  M xrot(float rot);
  M yrot(float rot);
  M zrot(float rot);
  M trans(float x, float y, float z);
  M trans(V vec);
  M scale(float sx, float sy, float sz);
  M projection(float z_min);
  M perspective_projection(float dist);
  M perspective(float fovy, float aspect, float near, float far);
  M inverse(M m1);
  M mult(M m1, M m2);
  M rotate_around_axis(V v, float angle);
  M rotate_around_axis(PT point, V v, float angle);
  PT mult(PT point, M matrix);
};

class MemoryApi
{
public:
  ME mem(int num_of_bits);
  ME range(ME mem, int start_bit, int end_bit);
  ME array(ME item, int count);
  ME array_item(ME array, int index);
  ME create_struct(ME *array, int count);
  ME struct_item(ME struct1, int index);
  ME change_bit(ME mem, int bit_number, bool new_value);
  void register_type(int size, int type);
  TY one();
  TY type(ME mem, int type);
  TY combine(int *value, int size);
  F func(TY ret_val, TY param, int param_value, int ret_val_value);
  F comb_func(F f1, F f2);
  F composition(F f1, F f2);
  F split_proj1(F f, TY center);
  F split_proj2(F f, TY center);
  F constant(F f); // split_proj2(f, one())
  F universe(F f); // split_proj1(f, one());
};

class PowerSetApi
{
public:
  PS power(TY type);
  PS power_elem(TY type, int value);
  PS power_pair(TY type, int left_value, int right_value);
  PS inverse_image(PS pw, F func);
  PS equalizer(PS pw, F func);
  PS pullback(PS pw, F func);
  PS subset(PS pw, F func);
  PS power_intersection(PS pw, PS pw2);
  bool elem(PS p, int value);
};

class VBOApi
{
public:
  VBOApi(Env &e);
  ~VBOApi();
  // preparing
  Vb alloc(int obj_count);
  void sprite(Vb v, int obj_num, BM bm, float x, float y);
  void polygon(Vb v, int obj_num, P p, float x, float y, float z);
  void grid(Vb v, int obj_num, BM tile_bitmap, BM grid, float top_x, float top_y);
  void heightmap(Vb v, int obj_num, BM bitmap_float);
  void prepare(Vb v);
  // rendering
  void swapframe(Vb v, int obj_num, int type, int frame);
  void move(Vb v, int obj_num, float x, float y, float z);
  void render(Vb v);
private:
  void *priv;
  Env &e;
};

#if 0
class MatrixApi
{
public:
  MA model_scaling(float sx, float sy, float sz);
  MA model_rotation(float angle, PT vec);
  MA obj_pos_in_world(PT pos);
  MA camera_pos(PT pos);
  MA camera_rotation(float y_angle, float x_angle=0.0, float z_angle=0.0);

  MA matrix_array(MA *array, int size);
  void push(MA matrix);
  void pop();
private:
  Env &e;
};
#endif


class PathApi
{
public:
  PathApi(Env &e);
  ~PathApi();
  PA point(PT p);
  PA next_point(PA pa, PT p);
  LA loop(PA p);
  P object(LA l, PT o, float dx, float dy, float dz);

  PA line(PT p1, PT p2, int splitnum);
  PA cross(PT pos, PA p1, PA p2);
  PA grid(PT pos, PT p_x, PT p_y, int sx, int sy);
  PA cube(PT pos, PT p_x, PT p_y, PT p_z, int sx, int sy, int sz);
private:
  void *priv;
  Env &e;
};

class ShaderApi
{
public:
  ShaderApi(Env &e);
  ~ShaderApi();
  void load(std::string filename);
  SH get_shader(std::string v_format, std::string f_format, std::string g_format);
  void use(SH shader);
  void unuse(SH shader);
  void bindnames(GameApi::SH shader, 
		 std::string s_vertex,
		 std::string s_normal,
		 std::string s_color,
		 std::string s_texcoord);
  void set_var(GameApi::SH shader, std::string name, float val);
  void set_var(GameApi::SH shader, std::string name, int val);
private:
  friend class StateChangeApi;
  void *priv;
  Env &e;
};

// Note: This interface has large state space. Passing it from
// function local scope to upper scope would require so huge and
// complex dynamic data structure that it does not exists, as it would
// need to fetch all data from all interfaces, create matching data
// structure, implement the same interface again using the dynamic
// data structure, and then copy all data. Only handles are available
// to end users and copying the data is not available. So once you have it
// in function local scope, passing it to outside of the current function
// becomes very difficult. Complexity level way too large in the data structure. 
struct EveryApi
{
  EveryApi(Env &e) 
    : mainloop_api(e), point_api(e), vector_api(e), sprite_api(e), grid_api(e), bitmap_api(e), polygon_api(e), bool_bitmap_api(e), float_bitmap_api(e),
      font_api(e), anim_api(e), event_api(e), /*curve_api(e),*/ function_api(e), volume_api(e), shader_api(e), state_change_api(e, shader_api), texture_api(e), separate_api(e) { }

  MainLoopApi mainloop_api;
  PointApi point_api;
  VectorApi vector_api;
  SpriteApi sprite_api;
  GridApi grid_api;
  BitmapApi bitmap_api;
  PolygonApi polygon_api;
  BoolBitmapApi bool_bitmap_api;
  FloatBitmapApi float_bitmap_api;
  FontApi font_api;
  AnimApi anim_api;
  EventApi event_api;
  //CurveApi curve_api;
  FunctionApi function_api;
  VolumeApi volume_api;
  ShaderApi shader_api;
  StateChangeApi state_change_api;
  TextureApi texture_api;
  SeparateApi separate_api;
};

class GamesApi
{
public:
  GamesApi(Env &e);
  ~GamesApi();
  void register_game(int game_id, void (*fptr)(EveryApi &e));
  void modify_map(int event, int game_id);
  void start_game(int event);
private:
  Env &e;
  void *priv;
};

class SocketApi
{
public:
  SocketApi();
  ~SocketApi();
  int open_connection(); // TODO
  void close_connection(int id);
  void send_msg(int id, char *msg);
  char *receive_msg(int id);
};



class FilesApi
{
public:
  D dir_files(Fi *array, int size);
  D dir_dirs(D *array, int size);
  D directory_name(D d, char *dirname);
  D combine(D d1, D d2);
  void root(D d); // this call creates all objects

  Fi file_contents(Str contents);
};

class StringApi
{
public:
  Str empty();
  Str combine(Str *array, int size);
  Str character(char c);
  Str string(char *string);
  void split(Str **output, int *count, Str orig, char separator);
};

class HtmlApi
{
public:
  Ht text(Str txt);
  Ht link(Str url, Ht text);
  Ht image(Str filename, int width, int height);
  Ht table(int sx, int sy, Ht *array);

  Str to_string(Ht h);
};


}; // GameApi namespace

#endif
