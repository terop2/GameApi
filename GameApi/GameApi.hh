

#ifndef GAMEAPI_HH
#define GAMEAPI_HH

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <iostream>
namespace GameApi
{
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;
using std::placeholders::_6;
using std::placeholders::_7;
using std::placeholders::_8;
using std::placeholders::_9;

#undef rad1
#undef rad2

  struct DR { int id; };
  struct OM { int id; };
  struct FO { int id; };
  struct WV { int id; }; // waveform
  struct BM { int id; }; // bitmap
  struct BMA { int id; }; // bitmap array
  struct VBM { int id; }; // vector bitmap
  struct BB { int id; }; // bool bitmap
  struct FB { int id; }; // float bitmap
  struct CBM { int id; }; // continuousbitmap
  struct SP { int id; }; // space
  struct SA { int id; }; // separate
  struct PT { int id; }; // point
  struct V { int id; }; // vector
  struct M { float mat[16]; }; // matrix
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
  struct F { int id; }; // function or float
  struct FA { int id; }; // float array
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
  struct LAY { int id; }; // layout
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
  struct VAA { int id; }; // vertex array array
  struct VX { int id; }; // voxel
  struct PL { int id; }; // plane
  struct PLA { int id; }; // planearray
  struct TX { int id; }; // texture
  struct TXID { int id; }; // texture id
  struct TR { int id; }; // time range
  struct VV { int id; }; // time range vertex array
  struct Q { int id; }; // quad texture coordinates
  struct DO { int id; };
  struct PC { int id; };
  struct SV { int id; };
  struct FOA { int id; };
  struct COV { int id; };
  struct LI { int id; };
  struct LLA { int id; };
  struct FD { int id; };
  struct VO { int id; };
  struct PTS { int id; };
  struct PTA { int id; };
  struct RD { int id; };
  struct FBO { int id; };
  struct SM { int id; };
  struct TRK { int id; };
  struct WAV { int id; };
  struct TBUF { int id; };
  struct SFO { int id; };
  struct W { int id; };
  struct WM { int id; };
  struct FtA { int id; };
  //template<class T>
  //struct E { int id; };



  template<class P, class R>
  class FunctionCb
  {
  public:
    virtual R Map(P p) const=0;
  };

  struct EveryApi;
#ifndef __clang__
#define IMPORT 
#define EXPORT
#else
#ifdef GAMEAPI_COMPILATION
#define IMPORT __declspec(dllexport)
#define EXPORT __declspec(dllexport)
#else
#define IMPORT
#define EXPORT
#endif
#endif
class Env
{
public:
  IMPORT Env();
  IMPORT void free_memory();
  IMPORT ~Env();
private:
  Env(const Env &);
  void operator=(const Env &);
public:
  void *envimpl;
  friend struct EnvImpl;
};
class MainLoopApi
{
public:
	IMPORT MainLoopApi(Env &e);
	IMPORT ~MainLoopApi();
  IMPORT void init_window(int screen_width = 800, int screen_height=600);
  IMPORT void init(SH sh, int screen_width = 800, int screen_height = 600);
  IMPORT void init_3d(SH sh, int screen_width = 800, int screen_heigth = 600);
  IMPORT void nvidia_init();
  IMPORT void transfer_sdl_surface(MainLoopApi &orig);
  IMPORT void clear();
  IMPORT void clear_3d();
  IMPORT void switch_to_3d(bool b, SH sh);
  IMPORT void alpha(bool enabled);
  void alpha_1(bool enabled);
  IMPORT void depth_test(bool enabled);
  IMPORT void transparency(bool enabled);
  IMPORT void cursor_visible(bool enabled);
  IMPORT void antialias(bool enabled);
  IMPORT float get_time();
  IMPORT int get_framenum();
  IMPORT void swapbuffers();
  IMPORT BM screenshot();
  IMPORT void fpscounter();
  IMPORT void delay(int ms);
  IMPORT unsigned int random();
  IMPORT unsigned int rand_max();
  struct Event
  {
    int type;
    int ch;
    PT cursor_pos;
    int button;
    bool joy0_button0;
    bool joy0_button1;
    bool joy0_button2;
    bool joy0_button3;

    bool joy1_button0;
    bool joy1_button1;
    bool joy1_button2;
    bool joy1_button3;
    bool last;
  };
  IMPORT Event get_event();
  void waittof();
  SP screenspace();
private:
  MainLoopApi(const MainLoopApi&);
  void operator=(const MainLoopApi&);
  float frame;
  float time;
  Env &e;
  void *priv;
};


class SpriteApi
{
public:
	IMPORT SpriteApi(Env &e);
	IMPORT ~SpriteApi();
	IMPORT void spritepos(BM bm, float x, float y);
	IMPORT void preparesprite(BM bm, int bbm_choose = -1);

	IMPORT VA create_vertex_array(BM bm);
        IMPORT void update_vertex_array(VA va, BM bm);
        IMPORT void clipping_sprite(VA va, int sx, int sy, float tex_l, float tex_t, float tex_r, float teb_b);
	IMPORT void render_sprite_vertex_array(VA va);

	IMPORT void rendersprite(BM bm, SH sh, float x, float y, float mult_x = 1.0, float mult_y = 1.0);
	IMPORT void rendersprite2(BM bm, SH sh, PT pos);
	IMPORT void rendersprite3(BM bm, int bm_choose, SH sh, float x, float y, float mult_x, float mult_y);
	void rendersprite3_1(BM bm, int bm_choose, SH sh, float x, float y, float mult_x, float mult_y);
	void rendersprite4(BM bm, int bm_choose, SH sh, PT pos);
	IMPORT void rendersprite5(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, float x, float y);
	IMPORT void rendersprite6(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, PT pos);
  //IMPORT void rendersprite(BM bm, SH sh, float x, float y, float x1, float y1, float inside_x, float inside_y, float inside_x1, float inside_y1);
	IMPORT SP spritespace(BM bm);
	IMPORT PT pixelpos(BM bm, int x, int y);
private:
  SpriteApi(const SpriteApi&);
  void operator=(const SpriteApi&);
  void *priv;
  Env &e;
};

class TextureApi
{
public:
	IMPORT TextureApi(Env &e);
	IMPORT TX tex_plane(int sx, int sy);
	IMPORT TX tex_bitmap(BM bm);
	IMPORT int unique_id();
	IMPORT TX tex_assign(TX tx, int id, int x, int y, BM bm);
	IMPORT TX tex_coord(TX tx, int id, int x, int y, int width, int height);
	IMPORT Q get_tex_coord(TX tx, int id);
	Q get_tex_coord_1(TX tx, int id);
	IMPORT TXID prepare(TX tx);
	IMPORT void use(TXID tx, int i = 0);
	IMPORT void unuse(TXID tx);
	IMPORT VA bind(VA va, TXID tx);
private:
  TextureApi(const TextureApi&);
  void operator=(const TextureApi&);
  Env &e;
  int count;
};

class GridApi
{
public:
	IMPORT GridApi(Env &e);
	IMPORT ~GridApi();
	IMPORT SP gridtilespace(BM tilebm, int sx, int sy, int x, int y);
	IMPORT SP wholegridspace(int sx, int sy, BM grid);

	IMPORT void preparegrid(BM tile_bitmap, int tile_choose = -1);
	IMPORT void rendergrid(BM grid, int grid_choose, float top_x, float top_y);

private:
  GridApi(const GridApi&);
  void operator=(const GridApi&);
  void *priv;
  Env &e;
};

class BitmapApi
{
public:
	IMPORT BitmapApi(Env &e);
	IMPORT ~BitmapApi();
        IMPORT BM newbitmap(int sx, int sy, unsigned int color = 0x00000000);
	IMPORT BM function(std::function<unsigned(int, int)> f, int sx, int sy);
	IMPORT BM transform(BM orig, std::function<unsigned int(int, int, unsigned int)> f);
	IMPORT BM newintbitmap(char *array, int sx, int sy, std::function<int(char)> f);
	IMPORT BM newcolorbitmap(char *array, int sz, int sy, std::function<unsigned int(char)> f);
	IMPORT BM newtilebitmap(int sx, int sy, int tile_sx, int tile_sy);
	IMPORT BM loadbitmap(std::string filename);
	IMPORT BM loadtilebitmap(std::string filename, int tile_sx, int tile_sy);
	IMPORT BM loadposbitmap(std::string filename);
	IMPORT BM findtile(BM tile_bitmap, int x, int y);
	BM subbitmap(BM orig, int x, int y, int width, int height);
	IMPORT BM subbitmapimage(BM orig, int r_start_range, int r_end_range, int g_start_range, int g_end_range, int b_start_range, int b_end_range, unsigned int empty_color);

	IMPORT BM growbitmap(BM small_orig_bitmap, int l, int t, int r, int b);
	IMPORT BM blitbitmap(BM bg, BM orig, int x, int y);
	IMPORT BM blitbitmap(BM bg, BM orig, int x, int y, FB mask);
	IMPORT BM blitbitmap(BM bg, BM orig, int x, int y, BB mask);
	IMPORT BM anim_array(BM *array, int size);
	IMPORT BM modify_bitmap(BM orig, BM bm, int x, int y);
	IMPORT BM interpolate_bitmap(BM orig1, BM orig2, float x); // x=[0..1]
	IMPORT BM repeat_bitmap(BM orig, int xcount, int ycount);
	IMPORT BM sample_bitmap(BM orig, float xmult, float ymult, float x, float y);
        IMPORT BM world_from_bitmap(std::function<BM(int)> f, BM int_bm, int dx, int dy);
        IMPORT BM dup_x(BM orig);
	IMPORT BM flip_x(BM orig);
	IMPORT BM flip_y(BM orig);
        IMPORT BM alpha_color(BM orig, unsigned int color_key);
  IMPORT BM gradient(PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy);
  IMPORT BM radial_gradient(int sx, int sy, PT pos, float r1, float r2, unsigned int color_1, unsigned int color_2);
  IMPORT BM conical_gradient(int sx, int sy, float x, float y, float angle1, float angle2, unsigned int color_1, unsigned int color_2);
  //BM bitmapandtypes(BM bm, BM (*fptr)(int)); // bm has ints in it
  IMPORT SP space(BM bm);
  
  IMPORT BM addsubrects(BM orig); // use subbitmap with orig bitmap.
  IMPORT void savebitmap(BM orig, std::string filename, bool alpha=false);
  IMPORT BM mandelbrot(bool julia,
		       float start_x, float end_x, // [-2..1]
		       float start_y, float end_y, // [-1,1]
		       float xx, float yy, // [0,0]
		       int sx, int sy,
		       int count);
  IMPORT BM mandelbrot2(bool julia,
			float start_x, float end_x, // [-2..1]
			float start_y, float end_y, // [-1,1]
			float xx, float yy, // [0,0]
			int sx, int sy,
			int count);
  IMPORT BM chessboard(int tile_sx, int tile_sy, int count_x, int count_y, unsigned int c1, unsigned int c2);
  IMPORT BM memoize(BM orig);
  IMPORT BM memoize_all(BM orig);
  IMPORT int intvalue(BM bm, int x, int y);
  IMPORT unsigned int colorvalue(BM bm, int x, int y);
  IMPORT int size_x(BM bm);
  IMPORT int size_y(BM bm);
  
  
  IMPORT BMA empty_array();
  BMA array(BM *array, int size);
  IMPORT BM array_elem(BMA array, int i);
private:
  BitmapApi(const BitmapApi&);
  void operator=(const BitmapApi&);
  void *priv;
  Env &e;
};


class FloatApi
{
public:
  FloatApi(Env &e);
  F value(float v);
  F ref(float *v);
  F array_index(float *array, int pos);
  float get_value(F f);
private:
  Env &e;
};

class FloatArrayApi
{
public:
  FloatArrayApi(Env &e);
  FA array(float *array, int size);
  FA f_array(F *array, int size);
  FA duparray(float value, int size);
  FA duparray(FA fa, int count);
  FA subarray(FA fa, int start_index, int length);
  FA ramp(float start_value, float end_value, int steps);
  F array_index(FA fa, int index);
  BM span_arrays(FA fa1, FA fa2, CBM f);
private:
  Env &e;
};

class FontApi
{
public:
  IMPORT FontApi(Env &e);
  IMPORT ~FontApi();
  IMPORT Ft newfont(const char *filename, int sx, int sy);
  IMPORT BM glyph(Ft font, long idx);
  IMPORT LI glyph_outline(Ft font, long idx, float sx, float sy);
  IMPORT PL glyph_plane(Ft font, long idx, float sx, float sy, float dx, float dy);
  IMPORT BM font_string(Ft font, const char *str, int x_gap);
  IMPORT FB glyph_fb(Ft font, long idx);
  IMPORT BB glyph_bb(Ft font, long idx);
  
  IMPORT FtA font_atlas_info(EveryApi &ev,Ft font, std::string chars, float sx, float sy, int y_delta);
  IMPORT BM font_atlas(EveryApi &ev, Ft font, FtA atlas, float sx, float sy);
  IMPORT BM font_string_from_atlas(EveryApi &ev, FtA atlas, BM atlas_bm, const char *str, int x_gap);
  IMPORT void save_atlas(FtA atlas, std::string filename);
  IMPORT FtA  load_atlas(std::string filename);
private:
  FontApi(const FontApi&);
  void operator=(const FontApi&);
  void *priv;
  Env &e;
};

class AnimApi
{
public:
  IMPORT AnimApi(Env &e);
  IMPORT ~AnimApi();
  IMPORT IS single(int val, float duration);
  IMPORT IS single(PT point, float duration);
  IMPORT IS single(float val, float duration);
  IMPORT IS line(int val1, int val2, float duration);
  IMPORT IS line(PT p1, PT p2, float duration);
  IMPORT IS line(float val1, float val2, float duration);
  IMPORT IS line_obj(P p, P p2, float duration);

  IMPORT IS repeat(IS i, int count);
  IMPORT IS repeat_infinite(IS i);

  IMPORT int timed_value(IS i, float time);  
  IMPORT PT timed_value_point(IS i, float time);
  IMPORT float timed_value_float(IS i, float time);

  IMPORT int timed_value_repeat_num(IS i, float time);
  IMPORT P timeline_obj(IS i, float time);
    
  IMPORT IS seq_line(IS *array, int size);
  IMPORT IS par_line(IS *array, int arraysize);
  IMPORT int timeline_int(IS l, int choose_par, float time);
  IMPORT float timeline_float(IS i, int choose_par, float time);
  IMPORT PT timeline_point(IS l, int choose_par, float time);
  IMPORT P timeline_obj(IS i, int choose_par, float time);
private:
  AnimApi(const AnimApi&);
  void operator=(const AnimApi&);

  void *priv;
  Env &e;
};

class EventApi
{
public:
  IMPORT EventApi(Env &e);
  IMPORT ~EventApi();
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
  EventApi(const EventApi&);
  void operator=(const EventApi&);
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
  IMPORT FunctionApi(Env &e);
  IMPORT ~FunctionApi();
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
  FunctionApi(const FunctionApi&);
  void operator=(const FunctionApi&);

  Env &e;
  void *priv;
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
  SpaceApi(const SpaceApi&);
  void operator=(const SpaceApi&);
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
	IMPORT TextApi(BitmapApi &bm, SpriteApi &sp) : bm(bm), sp(sp), priv(0) { }
	IMPORT ~TextApi();
	IMPORT void load_font(std::string filename, int sx, int sy, int x, int y, char start_char, char end_char);
	IMPORT void draw_text(std::string text, int x, int y, SH sh);
private:
  TextApi(const TextApi&);
  void operator=(const TextApi&);
  BitmapApi &bm;
  SpriteApi &sp;
  void *priv;
  int sx,sy;
  char start_char, end_char;
};

class VolumeApi
{
public:	IMPORT VolumeApi(Env &e);
	IMPORT ~VolumeApi();
	IMPORT O boolfunction(std::function<bool(float x, float y, float z)> f);
	IMPORT O subvolume(std::function<float(float x, float y, float z)> f, float start_range, float end_range);
  IMPORT O from_polygon(P p, float x, float y, float z);  // point outside of shape.
        IMPORT O from_bool_bitmap(BB b, float dist);
	IMPORT O link_areas(O o, PT p1, PT p2, float d);
	IMPORT O sphere(PT center, float radius);
	IMPORT O cube(float start_x, float end_x,
	 float start_y, float end_y,
	 float start_z, float end_z);
	IMPORT O cone(PT p1, PT p2, float rad1, float rad2);
	IMPORT O torus(PT center, PT u_x, PT u_y, float dist1, float dist2);

	IMPORT O colour(O object, unsigned int col);
	IMPORT O reflect(O object, float val); // val = [0..1]
	IMPORT O cubetexture(O object, PT origo, PT u_x, PT u_y, PT u_z, BM bm);
	IMPORT O spheretexture(O object, PT center, BM texture, float multx, float multy);
  IMPORT O subset_color(O model, O color_subset, unsigned int color);


  //IMPORT O move(O object, PT pos);
        IMPORT O move(O obj, float dx, float dy, float dz);
	IMPORT O rotatex(O object, float angle);
	IMPORT O rotatey(O object, float angle);
	IMPORT O rotatez(O object, float angle);
	IMPORT O scale(O object, float sx, float sy, float sz);

	IMPORT O not_op(O object1);
	IMPORT O min_op(O object1, O object2);
	IMPORT O max_op(O object1, O object2);
	IMPORT O andnot_op(O object1, O object2);

	IMPORT O mandelbulb(float n, float p_x, float p_y, float p_z,
	       float c_x, float c_y, float c_z,
	       float radius,
	       int iterations);

	IMPORT BB plane(O o, int sx, int sy,
	   PT u_p, V u_x, V u_y,
	   float start_x, float end_x,
	   float start_y, float end_y,
	   float start_z, float end_z);

	IMPORT BM render(O object, int sx, int sy, PT ray_0, PT ray_x, PT ray_y, PT ray_z);
  typedef std::function<P (float start_x, float end_x, 
			float start_y, float end_y, 
			float start_z, float end_z, 
			unsigned int color)> fptrtype;
  IMPORT P rendercubes(O object,
		fptrtype fptr,
		int size,
		float wholesize); // marching cubes algo
  IMPORT P rendercubes2(EveryApi &ev, O object,
			fptrtype fptr,
			int sx, int sy, int sz,
			float world_x, float world_y, float world_z); // normal for loop algo
  IMPORT P rendercubes3(O object,
			int sx, int sy, int sz,
			float start_x, float end_x,
			float start_y, float end_y,
			float start_z, float end_z);
  IMPORT void find_surface(O object, PT p1, PT p2, PT *res1, PT *res2, int level);
  // use RayTracingBitmap class in graph.hh
  // problem1: float values in O. (currently uses bool)
  // problem2: colors in O.
  // problem3: conversion from volumeobject to continuousvoxel<float> and continuousvoxel<color> (something like that exists already)
  // BM raytrace(O volume, int sx, int sy, V v, float z);  IMPORT BM montecarlo(O object, PT p_top, PT p_x, PT p_y, PT p_z, int sx, int sy);
private:
  VolumeApi(const VolumeApi&);
  void operator=(const VolumeApi&);

  Env &e;
};

class ShaderModuleApi
{
public:
  IMPORT ShaderModuleApi(Env &e) : e(e) { }
  IMPORT SFO function(std::function<std::string (std::string id)> function, std::string function_name, std::vector<std::string> param_names = std::vector<std::string>(), std::vector<std::string> arg_values = std::vector<std::string>());
  IMPORT SFO color_function(SFO orig, std::function<std::string (std::string)> function, std::string function_name);
  IMPORT SFO sphere(); // vec3 center, float radius
  IMPORT SFO sphere(PT center, float radius); // ()
  IMPORT SFO cube(); // vec3 tl, vec3 br
  IMPORT SFO cube(float start_x, float end_x,
	   float start_y, float end_y,
	   float start_z, float end_z); // ()
  IMPORT SFO rounded_cube(float start_x, float end_x,
		   float start_y, float end_y,
		   float start_z, float end_z,
		   float r);
  IMPORT SFO texture(SFO obj);
  IMPORT SFO texture_box(float start_x, float end_x,
  			 float start_y, float end_y,
  			 float start_z, float end_z);
  IMPORT SFO line(); // vec3 tl, vec3 br, float line_width1, float line_width2
  IMPORT SFO line(float start_x, float start_y, float start_z,
	   float end_x, float end_y, float end_z,
	   float line_width1, float line_width2);
  IMPORT SFO plane(PT center, V u_x, V u_y);
  IMPORT SFO torus(float radius_1, float radius_2);
  IMPORT SFO color(SFO obj, float r, float g, float b, float a);
  IMPORT SFO rot_x(SFO obj, float angle);
  IMPORT SFO rot_y(SFO obj, float angle); // float angle
  IMPORT SFO rot_z(SFO obj, float angle);
  IMPORT SFO rot_x(SFO obj, float angle, PT center);
  IMPORT SFO rot_y(SFO obj, float angle, PT center);
  IMPORT SFO rot_z(SFO obj, float angle, PT center);
  IMPORT SFO mod_x(SFO obj, float dx);
  IMPORT SFO mod_y(SFO obj, float dy);
  IMPORT SFO mod_z(SFO obj, float dz);
  IMPORT SFO and_not(SFO obj, SFO not_obj);
  IMPORT SFO or_elem(SFO obj1, SFO obj2);
  IMPORT SFO blend(SFO obj1, SFO obj2);
  IMPORT SFO trans(SFO obj);
  IMPORT SFO trans(SFO obj, float dx, float dy, float dz);
  IMPORT SFO from_points(PTS p, SFO obj);
  IMPORT SFO from_lines(LI li, SFO obj);
  IMPORT SFO bind_arg(SFO obj, std::string name, std::string value);
  IMPORT SFO color_from_normal(SFO obj);
  IMPORT SFO grayscale_from_distance(SFO obj, SFO dist, float length);
  IMPORT SFO stop_generation(SFO obj);
  IMPORT SFO mix_color(SFO col1, SFO col2, float t); // kills obj side
  IMPORT SFO grayscale(SFO obj);
  IMPORT SFO noise(SFO obj, float strength);
  IMPORT SFO ambient_occulsion(SFO obj, float d, float i);
  IMPORT SFO soft_shadow(SFO scene, V light_dir, float mint, float maxt, float k, float strong);
  IMPORT SFO bounding_primitive(SFO prim, SFO inside, SFO outside);
  IMPORT SFO render(SFO obj);
private:
  Env &e;
};

class GuiApi
{
public:
  GuiApi(Env &e, EveryApi &ev, SH sh) : e(e), ev(ev), sh(sh) { }
  W text(std::string label, FtA atlas, BM atlas_bm, int x_gap=2);
  W icon(BM bitmap);
  W line(W target1, int delta_x, int delta_y,
	 W target2, int delta2_x, int delta2_y, SH sh);
  W gradient(int sx, int sy, PT pos_1, PT pos_2, unsigned int colot_1, unsigned int color_2);
  W button(int sx, int sy, unsigned int color_1, unsigned int color_2);
  W mouse_move(W widget, int area_x, int area_y, int area_width, int area_height);
  W click_area(W widget, int area_x, int area_y, int area_width, int area_height);
  W or_elem(W w1, W w2);
  W highlight(int sx, int sy);
  W highlight(W wid);
  W margin(W item, int left, int top, int right, int bottom);
  W right_align(W item, int sx);
  W center_align(W item, int sx);
  W center_y(W item, int sy);
  W layer(W w1, W w2);
  W array_y(W *arr, int size, int y_gap);
  W array_x(W *arr, int size, int x_gap);
  W main_menu(std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  W menu(W main_menu, int menu_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  W submenu(W menu, int menu_pane_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  W scrollbar_y(int sx, int sy, int area_y);
  W scrollbar_x(int sx, int sy, int area_x);
  W scroll_area(W orig, int sx, int sy, int screen_y);
  W waveform(std::function<float (float)> f, float start_range, float end_range, float min_value, float max_value, int sx, int sy, unsigned int true_color, unsigned int false_color);
  W canvas(int sx, int sy);
  W find_canvas_item(W canvas, std::string id);
  int canvas_item(W canvas, W item, int x, int y);
  void del_canvas_item(W canvas, int id);
  W canvas_item_gameapi_node(int sx, int sy, std::string label, std::vector<std::string> param_types, std::string return_type, FtA atlas, BM atlas_bm, W &connect_click, std::string uid, std::vector<W> &params);
  W list_item_title(int sx, std::string label, FtA atlas, BM atlas_bm);
  W list_item_opened(int sx, std::string label, FtA atlas, BM atlas_bm, std::vector<std::string> subitems, FtA atlas2, BM atlas_bm2);
  W list_item(BM icon, std::string label, int sx, int sy);
  W list(W *array, int size, int sx, int sy);
  W dialog_item(std::string text, BM icon, int sx, int sy);
  W dialog_border(W item);
  W bitmap_dialog(BM bm, W &close_button, FtA atlas, BM atlas_bm);

  W button_with_text(std::string label);
  W button_with_icon(BM bitmap);
  W opengl_wrapper(W widget);
  W string_editor(std::string allowed_chars, std::string &target, FtA atlas, BM atlas_bm, int x_gap);
  W float_editor(float &target, FtA atlas, BM atlas_bm, int x_gap);
  W int_editor(int &target, FtA atlas, BM atlas_bm, int x_gap);
  W point_editor(float &x, float &y, float &z, FtA atlas, BM atlas_bm, int x_gap);
  W color_editor(std::string &col, FtA atlas, BM atlas_bm, int x_gap);
  struct EditTypes
  {
    int i_value;
    float f_value;
    float f_x, f_y, f_z;
    std::string color;
    std::string s;
  };
  W generic_editor(EditTypes &target, FtA atlas, BM atlas_bm, std::string type, int x_gap);
  void generic_to_string(const EditTypes &source, std::string type, std::string &target);
  void string_to_generic(EditTypes &target, std::string type, const std::string &source);
  IMPORT W edit_dialog(const std::vector<std::string> &labels, const std::vector<EditTypes*> &vec, Ft font, const std::vector<std::string> &types, W &cancel_button, W &ok_button);
  IMPORT W edit_dialog(const std::vector<std::string> &labels, const std::vector<EditTypes*> &vec, FtA atlas, BM atlas_bm, const std::vector<std::string> &types, W &cancel_button, W &ok_button);
  W bitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W boolbitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W floatbitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W polygonapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W shadermoduleapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W linesapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W pointsapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W pointapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);
  W vectorapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm);

  std::string bitmapapi_functions_item_label(int i);
  std::string boolbitmapapi_functions_item_label(int i);
  std::string floatbitmapapi_functions_item_label(int i);
  std::string polygonapi_functions_item_label(int i);
  std::string shadermoduleapi_functions_item_label(int i);
  std::string linesapi_functions_item_label(int i);
  std::string pointsapi_functions_item_label(int i);
  std::string pointapi_functions_item_label(int i);
  std::string vectorapi_functions_item_label(int i);
  W insert_widget(W item, std::function<void(int,int)> f);
  void insert_widget_activate(W w, bool b);

  void set_pos(W w, float px, float py);
  void set_size(W ow, float sx, float sy);
  void update(W w, PT mouse_cursor_pos, int button, int ch, int type);
  void render(W w);
  int chosen_item(W w);
  void select_item(W w, int item);
  float dynamic_param(W w, int id);
  void set_dynamic_param(W w, int id, float val);
  std::string get_id(W w);
  void set_id(W w, std::string id);
  int num_childs(W w);
  W get_child(W w, int i);
  int pos_x(W w);
  int pos_y(W w);
  int size_x(W w);
  int size_y(W w);
private:
  Env &e;
  EveryApi &ev;
  SH sh;
};

class WModApi
{
public:
  WModApi(Env &e) : e(e) { }
  WM load(std::string filename);
  void save(WM mod, std::string ilename);
  void insert_to_canvas(GuiApi &gui, W canvas, WM mod, int id, FtA font, BM font_bm, std::vector<W> &connect_clicks, std::vector<W> &params, std::vector<W> &diaplay_clicks, std::vector<W> &edit_clicks);
  void update_lines_from_canvas(W canvas, WM mod, int id);
  void insert_inserted_to_canvas(GuiApi &gui, W canvas, W item, std::string uid, W &display_clicks, W &edit_clicks);
  W inserted_widget(GuiApi &gui, WM mod2, int id, FtA atlas, BM atlas_bm, std::string func_name, W &connect_click, std::string uid, std::vector<W> &params);
  std::vector<int> indexes_from_funcname(std::string funcname);
  std::vector<std::string> types_from_function(WM mod, int id, std::string funcname);
  std::vector<std::string> labels_from_function(WM mod, int id, std::string funcname);
  std::vector<std::string*> refs_from_function(WM mod, int id, std::string funcname);
  std::vector<std::pair<std::string,std::string> > defaults_from_function(std::string module_name);
  void insert_to_mod(WM mod, int id, std::string modname, std::string uid, int x, int y, std::vector<std::pair<std::string, std::string> > params);
  
  std::string get_funcname(WM mod2, int id, std::string uid);
  void change_param_value(WM mod2, int id, std::string uid, int param_index, std::string newvalue);
  bool typecheck(WM mod2, int id, std::string uid1, std::string uid2, int param_index);
  void insert_links(EveryApi &ev, GuiApi &gui, WM mod2, int id, std::vector<W> &links, W canvas, const std::vector<W> &connect_targets, SH sh2);

  int execute(EveryApi &ev, WM mod2, int id, std::string line_uid);
  std::string return_type(WM mod2, int id, std::string line_uid);
private:
  Env &e;
};

class FloatVolumeApi
{
public:
	IMPORT FloatVolumeApi(Env &e) : e(e) { }
	IMPORT FO function(std::function<float(float x, float y, float z)> f);
	IMPORT FO from_volume(O o, float false_val, float true_val);
	IMPORT FO from_float_bitmap(FB bm,
		       float start_x, float end_x, 
		       float start_y, float end_y, 
		       float start_z, float end_z);
	IMPORT FO distance();
	IMPORT FO torusdistance(PT center, V u_x, V u_y, float radius);
	IMPORT FO move(FO f1, float dx, float dy, float dz);
	IMPORT FO minimum(FO f1, FO f2);
	IMPORT FO maximum(FO f1, FO f2);

        IMPORT O subvolume(FO f, float start_range, float end_range);

	IMPORT FO shadow(FD fd, V light_dir, float mint, float maxt, float k);

  //FO plus(FO f1, FO f2);
	IMPORT BM raytrace(FO object, int sx, int sy,
	      PT ray_0, PT ray_x, PT ray_y, PT ray_z, float surface_value);
	IMPORT FOA prepare(FO object, int numpoints,
	      float start_x, float start_y, float start_z, 
	      float end_x, float end_y, float end_z);
	IMPORT void render(FOA array);
private:
  FloatVolumeApi(const FloatVolumeApi&);
  void operator=(const FloatVolumeApi&);
  Env &e;
};


class ColorVolumeApi
{
public:
	IMPORT ColorVolumeApi(Env &e) : e(e) { }
	IMPORT COV function(std::function<unsigned int(float x, float y, float z)> f);
	IMPORT COV from_float_volume(FO obj, unsigned int col0, unsigned int col1);
	IMPORT COV from_volume(O obj, unsigned int col_true, unsigned int col_false);
  IMPORT COV from_continuous_bitmap(CBM bm);

	IMPORT COV mix(COV p1, COV p2, float value); // value=[0..1]
	IMPORT COV or_cov(COV p1, COV p2);
	IMPORT COV phong(VO normal, PT light_pos, CO i_s, CO i_d, CO i_a, float k_s, float k_d, float k_a, float alfa);
	IMPORT COV directcolor(VO normal);
	IMPORT P texture(P obj, COV colors);
	IMPORT BM texture_bm(P obj, COV colors, int face, int sx, int sy);
  // TODO
private:
  ColorVolumeApi(const ColorVolumeApi&);
  void operator=(const ColorVolumeApi&);
  Env &e;
};

class VectorVolumeApi
{
public:
	IMPORT VectorVolumeApi(Env &e) : e(e) { }
	IMPORT VO function(std::function<V(float x, float y, float z)> f);
	IMPORT VO normal(FD fd);
private:
  VectorVolumeApi(const VectorVolumeApi&);
  void operator=(const VectorVolumeApi&);

  Env &e;
};

class DistanceFloatVolumeApi
{
public:
	IMPORT DistanceFloatVolumeApi(Env &e) : e(e) { }
	IMPORT FD function(std::function<float(float x, float y, float z)> f);
	IMPORT FD sphere(PT center, float radius);
	IMPORT FD cube(float start_x, float end_x,
	  float start_y, float end_y,
	  float start_z, float end_z);
  IMPORT FD round_cube(float start_x, float end_x,
		       float start_y, float end_y,
		       float start_z, float end_z,
		       float r);
  IMPORT FD torus(float t_x, float t_y);
  IMPORT FD cone(float c_x, float c_y);
  IMPORT FD plane(float n_x, float n_y, float n_z, float n_w);
  IMPORT FD hex_prism(float h_x, float h_y);
  IMPORT FD tri_prism(float h_x, float h_y);
  IMPORT FD triangle(PT a, PT b, PT c);
  IMPORT FD quad(PT a, PT b, PT c, PT d);
	IMPORT FD line(PT start, PT end, float dist);

	IMPORT FD min(FD a1, FD a2);
	IMPORT FD and_not(FD a1, FD a2);
	IMPORT BM render(FD obj, COV color, PT pos, V u_x, V u_y, V u_z, int sx, int sy);
	IMPORT std::string shader_func(std::string name, FD obj, COV color);
private:
  DistanceFloatVolumeApi(const DistanceFloatVolumeApi&);
  void operator=(const DistanceFloatVolumeApi&);

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

class SurfaceApi
{
public:
  SurfaceApi(Env &e);
  ~SurfaceApi();
  S plane(PT pos, V u_x, V u_y);
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
	IMPORT PolygonApi(Env &e);
	IMPORT ~PolygonApi();
  
  
	IMPORT P empty();
        IMPORT P load_model(std::string filename, int obj_num);
        IMPORT void save_model(P poly, std::string filename);

  //IMPORT P line(PT p1, PT p2);
	IMPORT P triangle(PT p1, PT p2, PT p3);
	IMPORT P quad(PT p1, PT p2, PT p3, PT p4);
	IMPORT P quad_x(float x,
	   float y1, float y2,
	   float z1, float z2);
	IMPORT P quad_y(float x1, float x2,
	   float y,
	   float z1, float z2);
	IMPORT P quad_z(float x1, float x2,
	   float y1, float y2,
	   float z);
	IMPORT P polygon(PT *array, int size); // use render_dynamic with this.
	IMPORT P tri_vertex_array(float *v_array, int v_size,
		     float *n_array, int n_size,
		     unsigned int *c_array, int c_size,
		     float *tex_array, int tex_size,
		     float **attrib_array, int a_size1, int a_size2);
	IMPORT P cube(float start_x, float end_x,
	 float start_y, float end_y,
	 float start_z, float end_z);
	IMPORT P cube(PT *p); // 8 points needed
	IMPORT P sphere(PT center, float radius, int numfaces1, int numfaces2);
	IMPORT P cone(int numfaces, PT p1, PT p2, float rad1, float rad2);
  IMPORT P torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2);
	IMPORT P ring(float sx, float sy, float x, int steps); // use RingEffect::Init() to implement
  enum HeightMapType { EQuad, ETriangle };
  IMPORT P heightmap(BM bm, HeightMapType t,
	      float min_x, float max_x, 
	      float min_y, float max_y,
	      float min_z, float max_z);
  IMPORT P heightmap(FB fb, std::function<P (float)> f, float dx, float dz);
  IMPORT P fromsurface(S s, float thickness);
  IMPORT P fromsurface(S s1, S s2, C curve); // surfacebetweensurfaces
  
  IMPORT P sprite_bind(P p, TX tx, int id);
  IMPORT P sprite_bind(P p, Q bm, TX tx);
  P sprite_bind_1(P p, Q bm, TX tx);
  IMPORT P texture(P orig, BM bm, int bm_choose = -1); // all quads

  IMPORT P color(P orig, unsigned int color);
  IMPORT P color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z);
  IMPORT P texcoord_cube(P orig,
		  PT o, PT u_x, PT u_y, PT u_z,  // these are 3d
		  PT tex_o, PT tex_x, PT tex_y, PT tex_z); // tex_* are 2d
  IMPORT P texcoord_manual(P orig, float p1_x, float p1_y,
			   float p2_x, float p2_y,
			   float p3_x, float p3_y,
			   float p4_x, float p4_y);
  IMPORT P texcoord_spherical(PT center, P orig);
  IMPORT P texcoord_cylindar(P orig, float start_y, float end_y);
  IMPORT P color_cube(P orig,
	       PT o, PT u_x, PT u_y, PT u_z,
	       unsigned int color_o, unsigned int color_x, unsigned int color_y, unsigned int color_z);
  IMPORT P color_faces(P orig,
		unsigned int color_1, unsigned int color_2,
		unsigned int color_3, unsigned int color_4);
  IMPORT P color_from_normals(P orig);
  IMPORT P color_grayscale(P orig);
  IMPORT P color_range(P orig, unsigned int upper_range, unsigned int lower_range);

  IMPORT P texcoord_poly(P orig, int facenum, PT *array, int size);
  IMPORT P color_poly(P orig, int facenum, unsigned int *array, int size);

  IMPORT P quads_to_triangles(P p);
  IMPORT P flip_polygon_order(P p);

  IMPORT P or_elem(P p1, P p2);
  IMPORT P or_array(P *array, int size);
  P or_array_1(P *array, int size);
  //P and_not_elem(P p1, P p_not);


  IMPORT P grid(PT o, PT u_x, PT u_y, int num_x, int num_y);
  IMPORT P grid(PT o, PT u_x, PT u_y, int num_x, int num_y, P *grid);


  IMPORT P translate(P orig, float dx, float dy, float dz);
  P translate_1(P orig, float dx, float dy, float dz);
  IMPORT P rotatex(P orig, float angle);
  IMPORT P rotatey(P orig, float angle);
  IMPORT P rotatez(P orig, float angle);
  IMPORT P rotate(P orig, PT pt, V axis, float angle);
  IMPORT P scale(P orig, float sx, float sy, float sz);

  IMPORT P unit_cube(P orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT P unit_to_cube(P orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT P unit_to_flex(P orig, 
			PT bTL, PT bTR, PT bBL, PT bBR,
			PT fTL, PT fTR, PT fBL, PT fBR);

  IMPORT P move(P orig, PT obj_0, V obj_x, V obj_y, V obj_z,
	 PT world_0, V world_x, V world_y, V world_z);

  IMPORT L color_lighting(float dx, float dy, float dz,
		   int r, int g, int b, int a);
  IMPORT L radial_lighting(float dx, float dy, float dz,
		    int r, int g, int b, int a,
		    int r2, int g2, int b2, int a2,
		    float dist);
  IMPORT L array_lighting(L *array, int size);
  IMPORT P lighting(P orig, L lighting);
  IMPORT BM light_bm(L lighting, P poly);

  IMPORT P shadow(P orig, PT pos, V u_x, V u_y, V light_vec);
  IMPORT P reflection(P orig, PT pos, V u_x, V u_y, V ref_vec); // note, requires transparent object, which are not yet implemented.

  IMPORT P single_texture(P orig, BM texture);

  IMPORT P anim_array(P *array, int size); // OLD

  IMPORT P splitquads(P orig, int x_count, int y_count);
  IMPORT P change_positions(P orig, std::function<PT(PT p, int face, int point)> f);
  IMPORT P change_normals(P orig, std::function<V(V orig, int face, int point)> f);
  //P change_attrib(P orig, float (*fptr)(float orig, int face, int point, void *data), void *data=0);
  //P change_attribI(P orig, int (*fptr)(int orig, int face, int point, void *data), void *data=0);
  IMPORT P change_colors(P orig, std::function<unsigned int(unsigned int orig, int face, int point)> f);
  IMPORT P change_texture(P orig, std::function<int(int face)> f, BM *array, int size);

  IMPORT P recalculate_normals(P orig);
  IMPORT P memoize(P orig);
  IMPORT P memoize_all(P orig);

  IMPORT ID find_point_id(P p, int facenum, int pointnum); // save id's to ENV.
  IMPORT ID find_normal_id(P p, int facenum, int pointnum);
  IMPORT ID find_color_id(P p, int facenum, int pointnum);
  IMPORT ID find_texcoord_id(P p, int facenum, int pointnum);
  IMPORT ID id_array(ID *array, int size);
  IMPORT void preparepoly(P p, int bbm_choose = -1);
  void renderpoly(P p, float x, float y, float z);
  void renderpoly(P p, PT pos);
  void renderpoly(P p, int choose, float x, float y, float z);
  IMPORT void prepare(P p, int bbm_choose = -1);
  IMPORT void render(P p, int choose, float x, float y, float z);
  
  IMPORT void update_vertex_array(VA va, P p, bool keep=false);
  IMPORT VA create_vertex_array(P p, bool keep=false); // slow
  IMPORT void render_vertex_array(VA va); // fast
  //IMPORT int access_point_count(VA va, bool triangle);
  //IMPORT float *access_points(VA va, bool triangle, int face, int point);
  //IMPORT float *access_color(VA va, bool triangle, int face, int point);
  //IMPORT float *access_normals(VA va, bool triangle, int face, int point);
  //IMPORT float *access_texcoord(VA va, bool triangle, int face, int point);
  //IMPORT void update(VA va);

  IMPORT P anim_target_vector(P p, V v);
  IMPORT P anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z);
  IMPORT P anim_target_matrix(P p, M matrix);
  IMPORT P anim_endpoints(P p1, P p2); // must have similar 3d objects/same number of faces.
  IMPORT P anim_interpolate(P p, float val);

  IMPORT P counts(P p1, int numfaces);
  IMPORT P count_function(P p1, std::function<int(int face)> f);
  IMPORT P point_function(P p1, std::function<PT(int face, int point)> f);
  IMPORT P color_function(P p1, std::function<unsigned int(int face, int point)> f);
  IMPORT P texcoord_function(P p1, std::function<PT(int face, int point)> f);
  IMPORT P normal_function(P p1, std::function<V(int face, int point)> f);
  IMPORT P attrib_function(P p1, std::function<float(int face, int point, int idx)> f, int idx);
  IMPORT P attribi_function(P p1, std::function<int(int face, int point, int idx)> f, int idx);


  // must call prepare for P before these.
  IMPORT int get_tri_vertex_array_frames(P p);
  IMPORT int get_tri_vertex_array_rows(P p);
  IMPORT void get_tri_vertex_array(P p, int choose, int row,
			    int *v_size, float **v_array,
			    int *n_size, float **n_array,
			    int *c_size, unsigned int **c_array,
			    int *tex_size, float **tex_array,
			    int *attrib_size1, int *attrib_size2, float ***attrib_array);

  // this is for better animations, separates modified and non-modified parts
  IMPORT P create_static_geometry(P *array, int size);
  IMPORT P create_dynamic_geometry(P *array, int size);
  IMPORT void render_dynamic(P p, int array_elem, bool textures); // use memoize_all for p before calling this.

  IMPORT P world_from_bitmap(std::function<P (int c)> f, BM int_bm, float dx, float dy, int max_c);
  IMPORT P world_from_bitmap2(EveryApi &ev, std::function<P (int c, PT tl, PT tr, PT bl, PT br)> f, BM int_bm, FB float_bm, float dx, float dz, float height);
  IMPORT P world_from_voxel(std::function<P (unsigned int c)> f, VX voxel, float dx, float dy, float dz, int max_c);

  IMPORT P from_points(PTS p, std::function<P (int i, float x,float y,float z, unsigned int color)> f);
  IMPORT P from_lines(LI li, std::function<P (int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)> f);
  IMPORT P from_polygon(P p, std::function<P (int face,
					      PT p1, PT p2, PT p3, PT p4)> f);
  P from_polygon_1(P p, std::function<P (int face, 
					 PT p1, PT p2, PT p3, PT p4)> f);


  IMPORT BM renderpolytobitmap(P p, float x, float y, float z, int sx, int sy);
private:
  PolygonApi(const PolygonApi&);
  void operator=(const PolygonApi&);

  void *priv;
  Env &e;
};

class WaveformApi
{ // [0..length] -> [-1..1]
public: 
	IMPORT WaveformApi(Env &e) : e(e) { }
	IMPORT WV empty(float length);
	IMPORT WV function(std::function<float(float)> f, float length, float min_value, float max_value);
	IMPORT WV sinwave(float length, float freq);
	IMPORT WV sample(float *array, int length, float samplelength);
	IMPORT WV int_sample(int *array, int length, float samplelength, int min_value, int max_value);
	IMPORT WV mix(WV orig, float pos, WV sample);
	IMPORT WV volume_ramp(WV orig, float old_y_value, float x_pos1, float x_pos2, float y_pos1, float y_pos2);
	IMPORT WV freq_change(WV orig, float old_freq, float new_freq);
	IMPORT float length(WV orig);
	IMPORT float get_value(WV orig, float val);
	IMPORT WV length_change(WV orig, float new_length);
	IMPORT BM waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color);

private:
  WaveformApi(const WaveformApi&);
  void operator=(const WaveformApi&);
  Env &e;
};
class SampleCollectionApi
{
public:
  IMPORT SampleCollectionApi(Env &e) : e(e) { }
  IMPORT SM empty();
  IMPORT SM add(SM orig, WV wave, int sample_rate, int id);
  //WV find_wave(SM orig, int id);
  //int find_rate(SM orig, int id);
  IMPORT void init_audio();
  IMPORT WAV prepare(SM samples);
  IMPORT void play_sample(int channel, WAV w, int id);
  void play_sample_1(int channel, WAV w, int id);
private:
  Env &e;
};
class TrackerApi
{
public:
  IMPORT TrackerApi(Env &e) : e(e) { }
  IMPORT TRK empty(int numchannels, int numslots);
  IMPORT TRK audio_slot(TRK orig, int channel, int slot, int duration, int sample);
  IMPORT TRK array(TRK *array, int size);
  IMPORT TBUF prepare(TRK trk);
  IMPORT void play_song(EveryApi &ev, TBUF buf, WAV samples, int framenum, int speed);
private:
  Env &e;
};

class ShaderApi;

class StateChangeApi
{
public:
	IMPORT StateChangeApi(Env &e, ShaderApi &api);
	IMPORT TR init(int paths);
	IMPORT TR linear(TR s, int path_num, std::function<P(float val)> f, float start_v, float end_v, float duration);
	TR linear_1(TR s, int path_num, std::function<P(float val)> f, float start_v, float end_v, float duration);
	IMPORT VV prepare(TR sc);
	VV prepare_1(TR sc);
	IMPORT VV prepareloop(float *array, int arraysize,
		 std::function<P (float val)> f,
		 float step_duration);
	IMPORT void render(VV sc, float time, SH shadero);
	IMPORT void render(VV sc, float time, SH shadero, float(*fptr)(int path, std::string name));
private:
  StateChangeApi(const StateChangeApi&);
  void operator=(const StateChangeApi&);
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
  IMPORT PlaneApi(Env &e);
  IMPORT PL function(PT (*fptr)(EveryApi &e, int idx, void *data), int num_points, float sx, float sy, void *data);
  IMPORT PL color_function(PL pl, CO (*fptr)(EveryApi &ev, int idx, PT pos, void *data), void *data);
  IMPORT PL empty_plane(float sx, float sy);
  IMPORT PL flip_y(PL pl);
  IMPORT PL move(PL pl, float dx, float dy);
  PL or_plane(PL p1, PL p2);

  IMPORT PL circle(PT center, float radius, int numpoints);
  IMPORT PL star(PT center, float radius_1, float radius_2, int numpoints);

  IMPORT PL floodfill_border(BB bitmap, int x, int y);
  // TODO: how to represent/load fonts to this type.
  IMPORT PL empty(float sx, float sy);
  IMPORT PL circle(PL bg, PT center, float radius, int numpoints); 
  IMPORT PL rectangle(PL bg, PT p1, PT p2);
  IMPORT PL rectangle(PL bg, PT p1, PT p2, PT p3, PT p4);
  IMPORT PL sprite(PL bg, BM bitmap, PT pos, float size_x, float size_y);
  IMPORT PL polygon(PL bg, PT *points, int size);
  IMPORT PL color(PL orig, unsigned int new_color);
  IMPORT PL plane_elem(PL p1, PL p2);
  IMPORT PL plane_array(PL *array, int size);
  IMPORT PL move_plane(PL p1, V delta);

  IMPORT PL and_not(PL p1, PL not_p); // needed in fonts for the holes
                               // draw to bitmap, do and_not, put to texture

  IMPORT PL render_p(P p, M proj_matrix, float sx, float sy);
  IMPORT PL remove_splines(PL pl, float xdelta);

  IMPORT std::pair<PL,PL> triangulate(EveryApi &ev, PL pl, int obj);
  IMPORT PL triangulate_all(EveryApi &ev, PL pl, int point_count, int max_obj);
  IMPORT PL remove_empty_faces(PL pl);

  IMPORT P to_polygon_face(PL triangulated_pl, PT pos, V u_x, V u_y);
  IMPORT P to_polygon_lines(PL pl, PT pos, V u_x, V u_y, V u_z, float z_mult);
  P to_polygon_lines_1(PL pl, PT pos, V u_x, V u_y, V u_z, float z_mult);
  IMPORT P to_polygon(EveryApi &ev, PL pl, PT pos, V u_x, V u_y, V u_z, float z_mult);

  // 1) get black bitmap
  // 2) draw white polygon
  // 3) draw more black
  // 4) blit to screen, take only white pixels
  // PRoblem: How to change from black pixels to alpha pixels?

  // TODO: bezier curves in 2d.

  IMPORT P substitute_quads_with_plane(P orig, PL (*fptr)(EveryApi &ev, int face, void *data), void *data);
  IMPORT P plane_in_3d(PL plane, PT u_p, V v1, V v2);
  
  CBM render_continuous(PL pl, int num, unsigned int color_0, unsigned int color_1);
  IMPORT BB render_bool(PL pl, int num, int sx, int sy);

  IMPORT PLA prepare(PL pl);
  IMPORT void render(PLA pl, float x, float y, float mult_x, float mult_y);

private:
  PlaneApi(const PlaneApi&);
  void operator=(const PlaneApi&);
  Env &e;
};

class BoolBitmapApi
{ // NxN->2
public:
  IMPORT BoolBitmapApi(Env &e);
  IMPORT ~BoolBitmapApi();
  IMPORT BB empty(int sx, int sy);
  BB function(std::function<bool(int, int)> f, int sx, int sy);
  IMPORT BB transform(BB orig, std::function<bool(int, int, bool)> f);
  IMPORT O to_volume(BB b, float dist);
  IMPORT BB from_float_bitmap(FB float_bm, float range_start, float range_end);
  IMPORT BB from_bitmaps_color(BM bm, int r, int g, int b);
  IMPORT BB from_bitmaps_color_area(BM bm, std::function<bool(int r, int g, int b, int a)> f);
  IMPORT BB from_bitmaps_color_area(BM bm, int r_start, int r_end,
			            int g_start, int g_end, 
			            int b_start, int b_end, 
			            int a_start, int a_end);
  IMPORT BB line(BB bg, float p_x, float p_y, float p2_x, float p2_y, float line_width1, float line_width2);
  IMPORT BB circle(BB bg, float center_x, float center_y, float radius);
  IMPORT BB ellipse(BB bg, float center_x, float center_y, float center2_x, float center2_y, float sum_of_distances);
  //IMPORT BB rectangle(BB bg, int x, int y, int width, int height); // for static ones
  IMPORT BB rectangle(BB bg, float x, float y, float width, float height); // for moving
  IMPORT BB sprite(BB bg, BB sprite, float x, float y, float size_multiplier_x, float size_multiplier_y);
  IMPORT BB polygon(BB bg, PT *points, int size);
  IMPORT BB text(BB bg, int x, int y, const char *string, int size,
		 BB *glyphs, int glyphcount, int(*fptr)(EveryApi &ev, char));
  
  IMPORT BB part_circle(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad);
  IMPORT BB sections(int sx, int sy, float x, float y, std::function<bool (float angle)> f);
  
  IMPORT BB not_bitmap(BB b);
  BB or_bitmap(BB b1, BB b2);
  IMPORT BB andnot_bitmap(BB b1, BB not_b2);
  IMPORT BB xor_bitmap(BB b1, BB flip_b2);
  IMPORT BM choose_bitmap(BB bools, BM true_bitmap, BM false_bitmap);
  
  IMPORT BM to_bitmap(BB bools,
		      int true_r, int true_g, int true_b, int true_a,
		      int false_r, int false_g, int false_b, int false_a);
  BM to_bitmap_1(BB bools,
		 int true_r, int true_g, int true_b, int true_a,
		 int false_r, int false_g, int false_b, int false_a);
  IMPORT BM texture(BM bg,
		    BB bools1, int l1, int t1,
		    BM texturebitmap2, int l2, int t2);
  IMPORT int size_x(BB bm);
  IMPORT int size_y(BB bm);
  IMPORT bool boolvalue(BB bb, int x, int y);
private:
  BoolBitmapApi(const BoolBitmapApi&);
  void operator=(const BoolBitmapApi&);

  Env &e;
};

class FloatBitmapApi
{ // NxN->R
public: // values are [0.0..1.0]
  IMPORT FloatBitmapApi(Env &e);
  IMPORT ~FloatBitmapApi();
  IMPORT FB empty(int sx, int sy);
  IMPORT FB function(std::function<float(int, int)> f, int sx, int sy);
  IMPORT FB newfloatbitmap(char *array, int sx, int sy, std::function<float(char)> f);
  IMPORT FB from_bool_bitmap(BB bm, int csx, int csy);
  IMPORT FB grayscale(BM color_bm);
  IMPORT FB from_red(BM color_bm);
  IMPORT FB from_green(BM color_bm);
  IMPORT FB from_blue(BM color_bm);
  IMPORT FB from_alpha(BM color_bm);

  IMPORT FB min_fb(FB fb1, FB fb2);
  IMPORT FB max_fb(FB fb1, FB fb2);
  
  IMPORT FB mix_fb(FB fb1, FB fb2, float val);
  IMPORT FB space_fill(PT *array, float *array2, int size, int sx, int sy);
  
  IMPORT BM to_grayscale(FB fb);
  IMPORT BM to_grayscale_color(FB fb,
			       int r, int g, int b, int a,
			       int r2, int g2, int b2, int a2);
  IMPORT BM to_color(FB r, FB g, FB b, FB a);
  IMPORT BM choose_bitmap(FB fb, BM bm1, BM bm2);
  IMPORT FB perlin_noise(FB grad_1, FB grad_2);
  IMPORT BM subfloatbitmap(FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color);
  
  IMPORT FB from_bool(BB b, float val_true, float val_false);
  IMPORT FB distance_field(BB bb);
  
  BB to_bool(FB f, float true_range_start, float true_range_end);
  
  IMPORT TXID to_texid(FB bm);
  
  IMPORT int size_x(FB bm);
  IMPORT int size_y(FB bm);
  IMPORT float floatvalue(FB bm, int x, int y);
private:
  FloatBitmapApi(const FloatBitmapApi&);
  void operator=(const FloatBitmapApi&);
  Env &e;
};

class VectorBitmapApi
{
public: 
  VectorBitmapApi(Env &e) : e(e) { }
  VBM spheredelta(float radius1, float radius2, float x, float y, float z,
		  float sx, float sy);
  VBM plane(PT pos, V u_x, V u_y, V u_z);
  P span_object(VBM vectors, FB floats); // dimensions must be the same
private:
  Env &e;
};

class ContinuousBitmapApi
{ // RxR->RGB
public:
  IMPORT ContinuousBitmapApi(Env &e);
  IMPORT CBM empty(float x, float y);
  CBM constant(unsigned int color, float x, float y);
  IMPORT CBM function(std::function<unsigned int(float, float)> f, float sx, float sy);
  IMPORT BM sample(CBM c_bitmap, int sx, int sy);
  IMPORT CBM from_bitmap(BM bm, float xsize, float ysize);
  IMPORT BM to_bitmap(CBM bm, int sx, int sy);

  IMPORT CBM rotate(CBM bm, float center_x, float center_y, float angle);
  IMPORT CBM surfacecolor(S s, COV cov);
  IMPORT unsigned int get_pixel(CBM bitmap, float x, float y);
public: // Different rendering functions...
  CBM distance_render(FD obj, COV colours, float sx,float sy);

private:
  ContinuousBitmapApi(const ContinuousBitmapApi&);
  void operator=(const ContinuousBitmapApi&);

  Env &e;
};

class VoxelApi
{ // we don't have good ways to render these. RayTraceBitmap is one way,
  // but it could be useful in other rendering systems to get colours from
  // 3d space.
  // NxNxN->RGB
public:
  IMPORT VoxelApi(Env &e);
  IMPORT VX function(unsigned int (*fptr)(EveryApi &ev, int x, int y, int z, void *data), int sx, int sy, int sz, void *data);
  IMPORT unsigned int get_pixel(VX v, int x, int y, int z);
  IMPORT BM sw_rays(O volume, VX colours, int sx, int sy, float vx, float vy, float vz, float z);
  IMPORT P render_boxes(VX v, float sx, float sy, float sz);
private:
  Env &e;
};


class ColorApi
{ // ()->RGB
public:
	IMPORT ColorApi(Env &e);
	IMPORT CO u_color(unsigned int color); // argb
	IMPORT CO rgb_color(int r, int g, int b, int a); // r,g,b,a [0..255]
	IMPORT CO rgbf_color(float r, float g, float b, float a);
private:
  ColorApi(const ColorApi&);
  void operator=(const ColorApi&);
  Env &e;
};

class PointApi
{ // ()->PT
public:
	IMPORT PointApi(Env &e);
	IMPORT PT origo();
	IMPORT PT point(float x, float y, float z = 0.0);
	IMPORT PT move(PT p1, float dx, float dy, float dz = 0.0);
	IMPORT PT move(PT p1, V vec);

	IMPORT PT mix(PT p1, PT p2, float val); // val=[0.0..1.0]

	IMPORT PT from_angle(float radius, float angle);
	IMPORT PT from_angle(PT center, float radius, float angle);

	IMPORT float pt_x(PT p);
	IMPORT float pt_y(PT p);
	IMPORT float pt_z(PT p);
	IMPORT float dist3d(PT p, PT p2);
	IMPORT float dist2d(PT p, PT p2);
	IMPORT V minus(PT p1, PT p2);
private:
  PointApi(const PointApi&);
  void operator=(const PointApi&);
  Env &e;
};
		     
class PointCollectionApi
  { // int -> PT
public:
	IMPORT PointCollectionApi(Env &e) : e(e) { }
    IMPORT PC function(std::function<PT (int)> f, int count);
	IMPORT PC empty();
	IMPORT PC single(PT point);
	IMPORT PC single(float x, float y, float z);
	IMPORT PC array(PT *array, int size);
	IMPORT PC bezier(PT *array, PT *control_array, int size, int iteration_count);
	IMPORT P tri_object3d(PC p);
private:
  PointCollectionApi(const PointCollectionApi&);
  void operator=(const PointCollectionApi&);
  Env &e;
};
class PointsApi
{
public:
  PointsApi(Env &e) : e(e) { }
  IMPORT PTS function(std::function<PT(int pointnum)> f, int numpoints);
  IMPORT PTS color_function(PTS orig, std::function<unsigned int(int pointnum, PT pos)> f);
  IMPORT PTS from_float_volume(FO float_volume, int numpoints, 
			float start_x, float start_y, float start_z,
			float end_x, float end_y, float end_z);
  IMPORT PTS or_points(PTS p1, PTS p2);
  IMPORT PTS heightmap(BM colour, FB floatbitmap, PT pos, V u_x, V u_y, V u_z, int sx, int sy);
  IMPORT PTS from_volume(O o, PT pos, V u_x, V u_y, V u_z, int sx, int sy, int sz);
  IMPORT PTS shadow_points(PTS obj, PT pos, V u_x, V u_y, V light_vec);

  IMPORT PTS unit_cube(PTS orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT PTS unit_to_cube(PTS orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT PTS unit_to_flex(PTS orig, 
			PT bTL, PT bTR, PT bBL, PT bBR,
			PT fTL, PT fTR, PT fBL, PT fBR);

  IMPORT PTA prepare(PTS p);
  //float *point_access(PTA pta, int pointnum);
  //unsigned int *color_access(PTA pta, int pointnum);
  //void update(PTA array);
  IMPORT void render(PTA array);
private:
  PointsApi(const PointsApi&);
  void operator=(const PointsApi&);

  Env &e;
};
		     
class LinesApi
{
public:
	IMPORT LinesApi(Env &e) : e(e) { }
	IMPORT LI function(std::function<PT(int linenum, bool id)> f,
	      int numlines);
  IMPORT LI change_color(LI li, unsigned int color);
  IMPORT LI change_color(LI li, unsigned int color_1, unsigned int color_2);
	IMPORT LI from_points(PC points, bool loops);
        IMPORT LI from_plane(PL plane);
	IMPORT LI from_polygon(P poly);
        IMPORT LI from_polygon2(P poly1, P poly2);
  IMPORT LI normals_from_polygon(P poly, float length);
        IMPORT P line_anim(P poly, LI lines_from_polygon2, float val);
	IMPORT LI border_from_bool_bitmap(BB b, float start_x, float end_x,
			     float start_y, float end_y, float z);
  IMPORT LI render_slice_2d(P p, PT pos, V u_x, V u_y);
  IMPORT LI translate(LI lines, float dx, float dy, float dz);
  IMPORT LI scale(LI lines, float m_x, float m_y, float m_z);

  IMPORT LI unit_cube(LI orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT LI unit_to_cube(LI orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT LI unit_to_flex(LI orig, 
			PT bTL, PT bTR, PT bBL, PT bBR,
			PT fTL, PT fTR, PT fBL, PT fBR);

  IMPORT LLA prepare(LI l);
  IMPORT void update(LLA la, LI l);
  //IMPORT int line_count(LLA l);
  //      IMPORT float *line_access(LLA lines, int line, bool b);
  //IMPORT unsigned int *color_access(LLA lines, int line, bool b);
  //IMPORT void update(LLA lines);
	IMPORT void render(LLA array);
private:
  LinesApi(const LinesApi&);
  void operator=(const LinesApi&);
  Env &e;
};
		     

class VectorApi
{ // to be implemented via virtual Vector vec() const=0;
  // ()->V
public:
	IMPORT VectorApi(Env &e);
	IMPORT V null_vector();
	IMPORT V vector(float delta_x, float delta_y, float delta_z);
	IMPORT V sum(V v1, V v2);
	IMPORT V mul(V v1, float scalar);
	IMPORT float dot(V v1, V v2);
	IMPORT V cross(V v1, V v2);
	IMPORT float projection_length(V u, V u_x);
	IMPORT V projection_1(V u, V u_x);
	IMPORT V projection_2(V u, V u_x);
	IMPORT V neg(V v);
	IMPORT float dist3d(V v);
	IMPORT float dist2d(V v);
private: 
  VectorApi(const VectorApi&);
  void operator=(const VectorApi&);
 Env &e;
};

class SpaceVectorApi
{ // f : PT->V
public:
  SpaceVectorApi(Env &e) : e(e) { }
  SV function(std::function<V (float x, float y, float z)> f); // TODO
  SV from_points(PC coll); // choose poly(nearest points), linear interpoate, ensure no failures
  PT flow_next_point(SV v, PT p, float mult);
private:
  SpaceVectorApi(const SpaceVectorApi&);
  void operator=(const SpaceVectorApi&);
  Env &e;
};
class MatrixApi
{ 
public:
	IMPORT MatrixApi(Env &e);
	IMPORT M identity();
	IMPORT M xrot(float rot);
	IMPORT M yrot(float rot);
	IMPORT M zrot(float rot);
	IMPORT M trans(float x, float y, float z);
	IMPORT M trans(V vec);
	IMPORT M scale(float sx, float sy, float sz);
	IMPORT M projection(float z_min);
	IMPORT M perspective_projection(float dist);
	IMPORT M perspective(float fovy, float aspect, float near, float far);
	IMPORT M inverse(M m1);
	IMPORT M mult(M m1, M m2);
	IMPORT M rotate_around_axis(V v, float angle);
	IMPORT M rotate_around_axis(PT point, V v, float angle);
	IMPORT PT mult(PT point, M matrix);
private:
  MatrixApi(const MatrixApi&);
  void operator=(const MatrixApi&);
  Env &e;
};
class ObjectMoveApi
{
public:
  ObjectMoveApi(Env &e) : e(e) { }
  OM empty(int numobjects);
  OM add_object(OM orig, int index, P obj);
  OM clear_object(OM orig, int index);
  OM change_pos(OM orig, int index, M mat);
  VAA prepare_all(OM orig);
  void render_all(VAA orig);
private:
  ObjectMoveApi(const ObjectMoveApi&);
  void operator=(const ObjectMoveApi&);

  Env &e;
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
  VBOApi(const VBOApi&);
  void operator=(const VBOApi&);

  void *priv;
  Env &e;
};



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
  IMPORT ShaderApi(Env &e);
  IMPORT ~ShaderApi();
  IMPORT void load_default();
  IMPORT void load(std::string filename);
  IMPORT SH get_shader(std::string v_format, std::string f_format, std::string g_format,
		       std::string v_comb="", std::string f_comb="", bool trans=true, SFO module={-1} );
  IMPORT SH get_normal_shader(std::string v_format, std::string f_format, std::string g_format,
			      std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod={-1});
  SH get_shader_1(std::string v_format, std::string f_format, std::string g_format,
		  std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod={-1});
  SH get_normal_shader_1(std::string v_format, std::string f_format, std::string g_format,
			 std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod = { -1 });
  IMPORT SH texture_shader();
  IMPORT SH colour_shader();
  IMPORT SH colour_texture_shader();
  IMPORT void link(SH shader);
  IMPORT void use(SH shader);
  void link_1(SH shader);
  void use_1(SH shader);
  IMPORT void unuse(SH shader);
  IMPORT void bindnames(GameApi::SH shader,
			std::string s_vertex,
			std::string s_normal,
			std::string s_color,
			std::string s_texcoord);
  IMPORT void set_default_projection(GameApi::SH shader, std::string name);
  void set_default_projection_1(GameApi::SH shader, std::string name);
  IMPORT void set_y_rotation(SH shader, std::string name, float angle);
  IMPORT void bind_attrib(GameApi::SH shader, int num, std::string name);
  void bind_attrib_1(GameApi::SH shader, int num, std::string name);
  IMPORT void bind_frag(GameApi::SH shader, int attachment_num, std::string name);
  IMPORT void set_var(GameApi::SH shader, std::string name, float val);
  IMPORT void set_var(GameApi::SH shader, std::string name, float x, float y, float z);
  IMPORT void set_var(GameApi::SH shader, std::string name, float x, float y, float z, float k);
  IMPORT void set_var(GameApi::SH shader, std::string name, int val);
  IMPORT void set_var(GameApi::SH shader, std::string name, M matrix);
  IMPORT M get_matrix_var(GameApi::SH shader, std::string name);
private:
  ShaderApi(const ShaderApi&);
  void operator=(const ShaderApi&);

  friend class StateChangeApi;
  void *priv;
  Env &e;
};

class FrameBufferApi
{
public:
  IMPORT FrameBufferApi(Env &e) : e(e) { }
  IMPORT FBO create_fbo(int sx, int sy);
  IMPORT void config_fbo(FBO buffer);
  IMPORT void bind_fbo(FBO buffer);
  IMPORT void bind_screen(int sx, int sy);
  IMPORT TXID tex_id(FBO buffer);
  IMPORT TXID depth_id(FBO buffer);
private:
  FrameBufferApi(const FrameBufferApi &);
  void operator=(const FrameBufferApi&);
  Env &e;
};

class LayoutApi
{
public:
  LayoutApi(Env &e) : e(e) { }
  LAY root(int sx, int sy); // id = {0}
  LAY split_y(LAY l, int id, int num); // id = {0..num-1}
  LAY split_x(LAY l, int id, int num); // id = {0..num-1}
  LAY split_xy(LAY l, int id, int num_x, int num_y); // id={0..(num_x*num_y-1)}
  LAY margin(LAY l, int id, int lx, int rx, int ty, int by); // id={ 0 }
  LAY size(LAY l, int id, int lx, int size_x, int ty, int size_y); // id = { 0 }
  LAY center(LAY l, int id, int content_size_x, int content_size_y); // id={0}
  LAY array(LAY *array, int *id, int size); // id = { 0..size }

  int root_sx(LAY l);
  int root_sy(LAY l);

  int count(LAY l);
  int pos_x(LAY l, int id);
  int pos_y(LAY l, int id);
  int size_x(LAY l, int id);
  int size_y(LAY l, int id);

private:
  Env &e;
};

class DrawApi
{
public:
  DrawApi(Env &e) : e(e) { }
  DR label(LAY l, int id, std::string str, Ft font);
  DR icon(LAY l, int id, BM bm);
  DR rect(LAY l, int id, unsigned int color);
  DR scroll(DR dr, int delta_x, int delta_y);
  DR scroll_area(DR dr, LAY l, int id);
  DR cliprect(DR cmds, LAY l, int id);

  DR array(DR *array, int size);
  DR alternatives_array(std::string id, DR *array, int size);
  DR choose_alternative(DR dr, std::string id, int val);

  DR deep_copy(DR d);
  bool is_specific_alternative(DR d, std::string id);
  int array_count(DR d);
  int alternative_array_count(DR d);
  DR array_index(DR d, int index);
  DR alternative_array_index(DR d, int index);
  bool is_array(DR d);
  bool is_alternative_array(DR d);
  bool is_primitive(DR d);

  BM render_to_bm(DR dr);
  std::string render_to_html_canvas(DR dr);
  std::string generate_java_code(DR dr);
private:
  Env &e;
};

  
struct EveryApi
{
	EveryApi(Env &e)
  : mainloop_api(e), point_api(e), vector_api(e), matrix_api(e), sprite_api(e), grid_api(e), bitmap_api(e), polygon_api(e), bool_bitmap_api(e), float_bitmap_api(e), cont_bitmap_api(e),
    font_api(e), anim_api(e), event_api(e), /*curve_api(e),*/ function_api(e), volume_api(e), float_volume_api(e), color_volume_api(e), dist_api(e), vector_volume_api(e), shader_api(e), state_change_api(e, shader_api), texture_api(e), separate_api(e), waveform_api(e),  color_api(e), lines_api(e), plane_api(e), points_api(e), voxel_api(e), fbo_api(e), sample_api(e), tracker_api(e), sh_api(e), mod_api(e) { }

  MainLoopApi mainloop_api;
  PointApi point_api;
  VectorApi vector_api;
  MatrixApi matrix_api;
  SpriteApi sprite_api;
  GridApi grid_api;
  BitmapApi bitmap_api;
  PolygonApi polygon_api;
  BoolBitmapApi bool_bitmap_api;
  FloatBitmapApi float_bitmap_api;
  ContinuousBitmapApi cont_bitmap_api;
  FontApi font_api;
  AnimApi anim_api;
  EventApi event_api;
  //CurveApi curve_api;
  FunctionApi function_api;
  VolumeApi volume_api;
  FloatVolumeApi float_volume_api;
  ColorVolumeApi color_volume_api;
  DistanceFloatVolumeApi dist_api;
  VectorVolumeApi vector_volume_api;
  ShaderApi shader_api;
  StateChangeApi state_change_api;
  TextureApi texture_api;
  SeparateApi separate_api;
  WaveformApi waveform_api;
  ColorApi color_api;
  LinesApi lines_api;
  PlaneApi plane_api;
  PointsApi points_api;
  VoxelApi voxel_api;
  FrameBufferApi fbo_api;
  SampleCollectionApi sample_api;
  TrackerApi tracker_api;
  ShaderModuleApi sh_api;
  WModApi mod_api;
private:
  EveryApi(const EveryApi&);
  void operator=(const EveryApi&);

};

class GamesApi
{
public:
  IMPORT GamesApi(Env &e);
  IMPORT ~GamesApi();
  IMPORT void register_game(int game_id, void (*fptr)(EveryApi &e));
  IMPORT void modify_map(int event, int game_id);
  IMPORT void start_game(int event);
private:
  Env &e;
  //void *priv;
};


  //
  // Object-Oriented API
  //


  class RenderObject
  {
  public:
    virtual ~RenderObject() { }
    virtual void prepare()=0;
    virtual void render()=0;
  };
  void prepare(RenderObject &o);
  class MoveScaleObject2d
  {
  public:
    virtual ~MoveScaleObject2d() { }
    virtual void set_pos(float pos_x, float pos_y)=0;
    virtual void set_scale(float mult_x, float mult_y)=0;
  };
  class MoveScaleObject3d
  {
  public:
    virtual ~MoveScaleObject3d() { }
    virtual void set_pos(float pos_x, float pos_y, float pos_z)=0;
    virtual void set_scale(float mult_x, float mult_y, float mult_z)=0;
    virtual void set_rotation_matrix(M m)=0;
  };
  
  class ArrayObj3d : public RenderObject, public MoveScaleObject3d
  {
  public:
    ArrayObj3d(EveryApi &ev) : ev(ev), p_x(0.0), p_y(0.0), p_z(0.0), s_x(1.0), s_y(1.0), s_z(1.0) 
    {
      current_rot_matrix = ev.matrix_api.identity();
    }
    void push_back(RenderObject *obj, MoveScaleObject3d *obj2)
    {
      render_vec.push_back(obj);
      move_scale_vec.push_back(obj2);
      pos_x.push_back(0.0);
      pos_y.push_back(0.0);
      pos_z.push_back(0.0);
      scale_x.push_back(1.0);
      scale_y.push_back(1.0);
      scale_z.push_back(1.0);
      rot_matrix.push_back(ev.matrix_api.identity());
    }
    void prepare() {
      int s = render_vec.size();
      for(int i=0;i<s;i++) {
	render_vec[i]->prepare();
      }
    }
    void render() {
      int s = render_vec.size();
      for(int i=0;i<s;i++) {
	render_vec[i]->render();
      }
    }
    int size() const { return render_vec.size(); }

    void set_child_pos(int i, float cpos_x, float cpos_y, float cpos_z)
    {
      pos_x[i] = cpos_x;
      pos_y[i] = cpos_y;
      pos_z[i] = cpos_z;
      setup_one(i);
    }
    void set_child_scale(int i, float mult_x, float mult_y, float mult_z)
    {
      scale_x[i] = mult_x;
      scale_y[i] = mult_y;
      scale_z[i] = mult_z;
      setup_one(i);
    }
    void set_child_rot_matrix(int i, M m)
    {
      rot_matrix[i] = m;
      setup_one(i);
    }
    void set_pos(float pos_x, float pos_y, float pos_z)
    {
      p_x = pos_x;
      p_y = pos_y;
      p_z = pos_z;
      setup();
    }
    void set_scale(float mult_x, float mult_y, float mult_z)
    {
      s_x = mult_x;
      s_y = mult_y;
      s_z = mult_z;
      setup();
    }
    void set_rotation_matrix(M m)
    {
      current_rot_matrix = m;
      setup();
    }
  private:
    void setup_one(int i)
    {
      float ap_x = pos_x[i];
      float ap_y = pos_y[i];
      float ap_z = pos_z[i];
      float as_x = scale_x[i];
      float as_y = scale_y[i];
      float as_z = scale_z[i];
      M m = rot_matrix[i];
      ap_x += p_x;
      ap_y += p_y;
      ap_z += p_z;
      as_x*=s_x;
      as_y*=s_y;
      as_z*=s_z;
      m = ev.matrix_api.mult(current_rot_matrix, m);
      move_scale_vec[i]->set_pos(ap_x,ap_y,ap_z);
      move_scale_vec[i]->set_scale(as_x,as_y,as_z);
      move_scale_vec[i]->set_rotation_matrix(m);
    }
    void setup() 
    {
      int s = pos_x.size();
      for(int i=0;i<s;i++)
	{
	  setup_one(i);
	}
    }
  private:
    EveryApi &ev;
    std::vector<RenderObject *> render_vec;
    std::vector<MoveScaleObject3d *> move_scale_vec;
    std::vector<float> pos_x;
    std::vector<float> pos_y;
    std::vector<float> pos_z;
    std::vector<float> scale_x;
    std::vector<float> scale_y;
    std::vector<float> scale_z;
    std::vector<M> rot_matrix;
    float p_x, p_y, p_z;
    float s_x, s_y, s_z;
    M current_rot_matrix;
  };
  class SpriteObj : public RenderObject, public MoveScaleObject2d
  {
  public:
    SpriteObj(EveryApi &ev, BM bm_, SH sh) : sp(ev.sprite_api), matrix_api(ev.matrix_api), shader_api(ev.shader_api), sh(sh) 
    { 
      bm.push_back(bm_);
      pos_x = 0.0; pos_y = 0.0;
      mult_x = 1.0; mult_y = 1.0;
      anim_id = 0;
      m = matrix_api.identity();
      prepared=false;
    }
    SpriteObj(EveryApi &ev, std::vector<BM> anim, SH sh) : sp(ev.sprite_api), matrix_api(ev.matrix_api), shader_api(ev.shader_api), bm(anim), sh(sh) 
    {
      pos_x = 0.0; pos_y = 0.0;
      mult_x = 1.0; mult_y = 1.0;
      anim_id = 0;
      m = matrix_api.identity();
      prepared=false;
    }
    void prepare() 
    {
      va.clear();
      for(int i=0;i<(int)bm.size();i++)
	va.push_back(sp.create_vertex_array(bm[i])); 
      prepared=true;
    }
    void render() 
    { 
      if (prepared)
	{
	  shader_api.set_var(sh, "in_MV", matrix_api.mult(m,matrix_api.mult(matrix_api.scale(mult_x, mult_y, 1.0), matrix_api.trans(pos_x, pos_y, 0.0))));
	  sp.render_sprite_vertex_array(va[anim_id]);
	}
      //sp.rendersprite(bm[anim_id], sh, pos_x, pos_y, mult_x, mult_y); 
    }
    void set_pos(float p_x, float p_y) { pos_x=p_x; pos_y=p_y; }
    void set_scale(float m_x, float m_y) { mult_x = m_x; mult_y=m_y; }
    void set_matrix(M m1) { m = m1; }
    void set_anim_frame(int id) 
    {
      if (id>=0 && id<(int)bm.size())
	anim_id = id; 
    }
  private:
    float pos_x, pos_y;
    float mult_x, mult_y;
    SpriteApi &sp;
    MatrixApi &matrix_api;
    ShaderApi &shader_api;
    std::vector<BM> bm;
    std::vector<VA> va;
    M m;
    int anim_id;
    SH sh;
    bool prepared;
  };
  class SpriteWorldObj : public RenderObject, public MoveScaleObject2d
  {
  public:
    SpriteWorldObj(EveryApi &ev, std::function<BM(int)> f, int numvalues, BM int_bm, int dx, int dy, SH sh) : bmapi(ev.bitmap_api), sp(ev.sprite_api), matrix_api(ev.matrix_api), shader_api(ev.shader_api), sh(sh), f(f), numvalues(numvalues), int_bm(int_bm), dx(dx), dy(dy) 
    {
      m_t = 0;
      m_l = 0;
      m_width = bmapi.size_x(int_bm);
      m_height = bmapi.size_y(int_bm);
      mult_x = 1.0;
      mult_y = 1.0;
      bitmap = new int[m_width*m_height];
      for(int y=0;y<m_height;y++)
	for(int x=0;x<m_width;x++)
	  {
	    bitmap[x+y*m_width] = bmapi.intvalue(int_bm, x,y);
	  }
    }
    ~SpriteWorldObj() { delete [] bitmap; }
    void set_block(int x, int y, int c)
    {
      if (x>=0&&x<m_width)
	if (y>=0&&y<m_height)
	  bitmap[x+y*m_width] = c;
    }
    int read_block(int x, int y)
    {
      if (x>=0&&x<m_width)
	if (y>=0&&y<m_height)
	  return bitmap[x+y*m_width];
      return 0;
    }
    void set_range(int l, int t, int width, int height)
    {
      m_l = l;
      m_t = t;
      m_width = width;
      m_height = height;
    }
    void prepare() 
    {
      va.clear();
      for(int i=0;i<(int)numvalues;i++)
	va.push_back(sp.create_vertex_array(f(i))); 
      prepared=true;
    }
    void render() 
    { 
      if (prepared)
	{
	  M m = matrix_api.mult(matrix_api.scale(mult_x, mult_y, 1.0), matrix_api.trans(pos_x, pos_y, 0.0));
	  for(int y=m_t;y<m_height;y++)
	    for(int x=m_l;x<m_width;x++)
	      {
		M m2 = matrix_api.mult(matrix_api.trans(x*dx, y*dy, 0.0),m);
		shader_api.set_var(sh, "in_MV", m2);
		int index = bitmap[x+y*m_width]; //bmapi.intvalue(int_bm, x,y);
		sp.render_sprite_vertex_array(va[index]);
	      }
	}
      //sp.rendersprite(bm[anim_id], sh, pos_x, pos_y, mult_x, mult_y); 
    }
    void set_pos(float p_x, float p_y) { pos_x=p_x; pos_y=p_y; }
    void set_scale(float m_x, float m_y) { mult_x = m_x; mult_y=m_y; }
    int get_pos_x(float mouse_x) const
    {
      mouse_x-=pos_x;
      mouse_x/=mult_x;
      mouse_x/=dx;
      return (int)mouse_x;
    }
    int get_pos_y(float mouse_y) const
    {
      mouse_y-=pos_y;
      mouse_y/=mult_y;
      mouse_y/=dy;
      return (int)mouse_y;
    }
  private:
    BitmapApi &bmapi;
    SpriteApi &sp;
    MatrixApi &matrix_api;
    ShaderApi &shader_api;
    SH sh;
    std::function<BM(int)> f;
    std::vector<VA> va;
    int *bitmap;
    int numvalues;
    BM int_bm;
    float dx,dy;
    int m_t, m_l, m_width, m_height;
    bool prepared;
    float mult_x,mult_y;
    float pos_x, pos_y;
  };


  class PolygonObj : public RenderObject, public MoveScaleObject3d
  {
  public:
    PolygonObj(EveryApi &ev, P p, SH sh) : api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), sh(sh) 
    {
      m_p.push_back(p);
      id.resize(1);
      m_va2.resize(1);
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      current_rot2 = mat.identity();
      setup_m();
      //id.id = 0;
      //va.id = 0;
      anim_id = 0;
      anim_time = 0.0;
    }

    PolygonObj(EveryApi &ev, std::vector<P> anim_p, SH sh) : api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), sh(sh) 
    {
      m_p = anim_p;
      id.resize(anim_p.size());
      m_va2.resize(anim_p.size());
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      current_rot2 = mat.identity();
      setup_m();
      //id.id = 0;
      //va.id = 0;
      anim_id = 0;
      anim_time = 0.0;
    }

    PolygonObj(PolygonApi &api, ShaderApi &shapi, MatrixApi &mat, TextureApi &tex,P p, SH sh) : api(api), shapi(shapi), mat(mat), tex(tex), sh(sh) 
    {
      m_p.push_back(p);
      id.resize(1);
      m_va2.resize(1);
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      current_rot2 = mat.identity();
      setup_m();
      //id.id=0;
      //va.id =0;
      anim_id = 0;
      anim_time = 0.0;
    }
    void prepare() 
    { 
      m_va2.clear();
      for(int i=0;i<(int)m_p.size();i++)
	{
	  VA va = api.create_vertex_array(m_p[i], id[i].id!=0);
	  VA va2;
	  if (id[i].id!=0) {
	    va2 = tex.bind(va, id[i]);
	  } else {
	    va2 = va;
	  }
	  m_va2.push_back(va2);
	}
    }
    void render() {
      shapi.use(sh);
      shapi.set_var(sh, "in_MV", m); 
      //shapi.set_var(sh, "in_T", m2);
      shapi.set_var(sh, "in_POS", anim_time);
      api.render_vertex_array(m_va2[anim_id]); 
      shapi.set_var(sh, "in_POS", 0.0f);
    }
    void set_pos(float pos_x, float pos_y, float pos_z)
    {
      current_pos = mat.trans(pos_x, pos_y, pos_z);
      setup_m();
    }
    void set_scale(float mult_x, float mult_y, float mult_z)
    {
      current_scale = mat.scale(mult_x, mult_y, mult_z);
      setup_m();
    }
    void set_rotation_y(float angle)
    {
      current_rot = mat.yrot(angle);
      setup_m();
    }
    void set_rotation_matrix(M m)
    {
      current_rot = m;
      setup_m();
    }
    void set_rotation_matrix2(M m)
    {
      current_rot2 = m;
      setup_m();
    }
    void bind_texture(int anim_id, TXID id_)
    {
      id[anim_id] = id_;
    }
    void set_anim_frame(int id)
    {
      if (id>=0&&id<(int)m_va2.size())
	anim_id = id;
    }
    void set_anim_time(float time)
    {
      anim_time = time;
    }
  private:
    void setup_m() {
      m = mat.mult(mat.mult(mat.mult(current_rot,current_scale), current_pos), current_rot2);
      //m2 = mat.mult(mat.mult(current_rot, current_scale), current_rot2);
    }
  private:
    PolygonApi &api;
    ShaderApi &shapi;
    MatrixApi &mat;
    TextureApi &tex;
    M current_pos;
    M current_scale;
    M current_rot;
    M current_rot2;
    M m;
    M m2;
    std::vector<P> m_p;
    SH sh;
    //VA va;
    std::vector<VA> m_va2;
    std::vector<TXID> id;
    int anim_id;
    float anim_time;
  };

  class WorldObj : public RenderObject, public MoveScaleObject3d
  {
  public:
    WorldObj(EveryApi &ev, std::function<P(int)> f, int numvalues, BM bm, float dx, float dy, SH sh) : bmapi(ev.bitmap_api), api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), pts(ev.point_api), numvalues(numvalues), bm(bm), f(f), dx(dx), dy(dy), sh(sh) 
    {
      int sx = bmapi.size_x(bm);
      int sy = bmapi.size_y(bm);
      bitmap = new int[sx*sy];
      anim_time = new float[sx*sy];
      for(int y=0;y<sy;y++)
	{
	  for(int x=0;x<sx;x++)
	    {
	      int val = bmapi.intvalue(bm, x,y);
	      bitmap[x+y*sx] = val;
	      anim_time[x+y*sx] = 0.0f;
	    }
	}

      m_x = 0;
      m_y = 0;
      m_sx = sx;
      m_sy = sy;


      id.resize(numvalues);
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      current_rot2 = mat.identity();
      setup_m();
      //id.id = 0;
      //va.id = 0;
      anim_id = 0;
    }
    ~WorldObj() { delete [] bitmap; }
    void set_block(int x, int y, int c) { 
      int sx=m_sx; //bmapi.size_x(bm);
      bitmap[x+y*sx] = c;
    }
    int read_block(int x, int y) const {
      int sx=m_sx; //bmapi.size_x(bm);
      return bitmap[x+y*sx];
    }
    void set_anim_time(int x, int y, float time)
    {
      int sx = m_sx; //bmapi.size_x(bm);
      anim_time[x+y*sx] = time;
    }
    void prepare() 
    { 
      m_va2.clear();
      for(int i=0;i<numvalues;i++)
	{
	  P p = f(i);
	  VA va = api.create_vertex_array(p);
	  VA va2;
	  if (id[i].id!=0) {
	    va2 = tex.bind(va, id[i]);
	  } else {
	    va2 = va;
	  }
	  m_va2[i] = va2;
	}
    }
    void set_range(int x, int y, int sx, int sy)
    {
      m_x = x;
      m_y = y;
      m_sx = sx;
      m_sy = sy;
    }
    P collect() {
      std::vector<P> vec2;
      int sx = m_sx; //bmapi.size_x(bm);
      for(int y=m_y;y<m_y+m_sy;y++) {
	std::vector<P> vec1;
	for(int x=m_x;x<m_x+m_sx;x++) {
	  P p = f(bitmap[x+y*sx]);
	  P p2 = api.translate(p, dx*x,0.0,dy*y);
	  vec1.push_back(p2);
	}
	vec2.push_back(api.or_array(&vec1[0], vec1.size()));
      }
      return api.or_array(&vec2[0], vec2.size());
    }
    void render() {
      shapi.use(sh);
      int sx = m_sx; //bmapi.size_x(bm);
      for(int y=m_y;y<m_y+m_sy;y++) {
	for(int x=m_x;x<m_x+m_sx;x++) {
	  M t = mat.trans(dx*x,0.0,dy*y);
	  M t2 = mat.mult(t,m);
	  shapi.set_var(sh, "in_MV", t2);
	  shapi.set_var(sh, "in_POS", anim_time[x+y*sx]);
	  //std::cout << x << " " << y << " " << anim_time[x+y*sx] << std::endl;
	  api.render_vertex_array(m_va2[bitmap[x+y*sx]]);
	  shapi.set_var(sh, "in_POS", 0.0f);
	}
      }
    }
    void set_pos(float pos_x, float pos_y, float pos_z)
    {
      current_pos = mat.trans(pos_x, pos_y, pos_z);
      setup_m();
    }
    void set_scale(float mult_x, float mult_y, float mult_z)
    {
      current_scale = mat.scale(mult_x, mult_y, mult_z);
      setup_m();
    }
    void set_rotation_y(float angle)
    {
      current_rot = mat.yrot(angle);
      setup_m();
    }
    void set_rotation_matrix(M m)
    {
      current_rot = m;
      setup_m();
    }
    void set_rotation_matrix2(M m)
    {
      current_rot2 = m;
      setup_m();
    }
    void bind_texture(int num_id, TXID id_)
    {
      id[num_id] = id_;
    }
    void set_anim_frame(int id)
    {
      if (id>=0&&id<(int)m_va2.size())
	anim_id = id;
    }
    void pick_object1(float x, float y, float &xx, float &yy, float &zz)
    {
      PT point = pts.point(x,y,0.0f);

      M mp = shapi.get_matrix_var(sh, "in_P");
      M m3 = mat.inverse(mp);
      PT point1 = mat.mult(point, m3);

      M mp2 = shapi.get_matrix_var(sh, "in_T");
      M m4 = mat.inverse(mp2);
      PT point2 = mat.mult(point1, m4);

      M m2 = mat.inverse(m);
      PT point3 = mat.mult(point2, m2);
     
      xx = pts.pt_x(point3);
      yy = pts.pt_y(point3);
      zz = pts.pt_z(point3);

    }
  private:
    void setup_m() {
      m = mat.mult(mat.mult(mat.mult(current_rot,current_scale), current_pos), current_rot2);
    }
  private:
    BitmapApi &bmapi;
    PolygonApi &api;
    ShaderApi &shapi;
    MatrixApi &mat;
    TextureApi &tex;
    PointApi &pts;
    M current_pos;
    M current_scale;
    M current_rot;
    M current_rot2;
    M m;
    //std::vector<P> m_p;
    int numvalues;
    BM bm;
    std::function<P(int)> f;
    float dx,dy;
    int *bitmap;
    float *anim_time;
    SH sh;
    int m_x, m_y, m_sx, m_sy;
    //VA va;
    //std::vector<VA> m_va2;
    std::map<int, VA> m_va2;
    std::vector<TXID> id;
    int anim_id;
  };

  class PointsObj : public RenderObject, public MoveScaleObject3d
  {
  public:
    PointsObj(EveryApi &ev, PTS fo, SH sh) : points_api(ev.points_api), mat(ev.matrix_api), shapi(ev.shader_api), fo(fo), sh(sh) 
    {
      numpoints = 5000;
      start_x = -1.0;
      start_y = -1.0;
      start_z = -1.0;
      end_x = 1.0;
      end_y = 1.0;
      end_z = 1.0;
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      setup_m(); 
    }
    void set_numpoints(int count) { numpoints = count; }
    void set_bounds(float s_x, float s_y, float s_z,
		    float e_x, float e_y, float e_z)
    {
      start_x = s_x;
      start_y = s_y;
      start_z = s_z;
      end_x = e_x;
      end_y = e_y;
      end_z = e_z;
    }
    void prepare() {
      array = points_api.prepare(fo);
      //array = floatvolume.prepare(fo, numpoints, start_x, start_y, start_z, end_x, end_y, end_z); 
    }
    void render() {
      shapi.set_var(sh, "in_MV", m);
      points_api.render(array);
      //floatvolume.render(array);
    }
    void set_pos(float pos_x, float pos_y, float pos_z)
    {
      current_pos = mat.trans(pos_x, pos_y, pos_z);
      setup_m();
    }
    void set_scale(float mult_x, float mult_y, float mult_z)
    {
      current_scale = mat.scale(mult_x, mult_y, mult_z);
      setup_m();
    }
    void set_rotation_matrix(M m)
    {
      current_rot = m;
      setup_m();
    }
  private:
    void setup_m() {
      m = mat.mult(current_rot, mat.mult(current_scale, current_pos));
    }

  private:
    PointsApi &points_api;
    MatrixApi &mat;
    ShaderApi &shapi;
    PTS fo;
    PTS fo2;
    PTA array;
    SH sh;
    M current_pos;
    M current_scale;
    M current_rot;
    M m;
    int numpoints;
    float start_x, start_y, start_z;
    float end_x, end_y, end_z;
  };
  class LinesObj : public RenderObject, public MoveScaleObject3d
  {
  public:
    LinesObj(EveryApi &ev, LI li, SH sh) : lines(ev.lines_api), mat(ev.matrix_api), shapi(ev.shader_api), li(li), sh(sh) 
    {
      current_pos = mat.identity();
      current_scale = mat.identity();
      current_rot = mat.identity();
      current_rot2 = mat.identity();
      setup_m();
    }
    //LinesObj(LinesApi &lines, MatrixApi &mat, ShaderApi &shapi, LI li, SH sh) : lines(lines), mat(mat), shapi(shapi), li(li), sh(sh) { }
    void prepare() { li2 = lines.prepare(li); }
    void render() {
      shapi.use(sh);
      shapi.set_var(sh, "in_MV", m);
      lines.render(li2); 
    }
    void set_pos(float pos_x, float pos_y, float pos_z)
    {
      current_pos = mat.trans(pos_x, pos_y, pos_z);
      setup_m();
    }
    void set_scale(float mult_x, float mult_y, float mult_z)
    {
      current_scale = mat.scale(mult_x, mult_y, mult_z);
      setup_m();
    }
    void set_rotation_matrix(M m) 
    {
      current_rot = m;
      setup_m();
    }
    void set_rotation_matrix2(M m)
    {
      current_rot2 = m;
      setup_m();
    }
  private:
    void setup_m() {
      m = mat.mult(mat.mult(current_rot, mat.mult(current_scale, current_pos)),current_rot2);
    }
  private:
    LinesApi &lines;
    MatrixApi &mat;
    ShaderApi &shapi;
    LI li;
    SH sh;
    LLA li2;
    M current_pos;
    M current_scale;
    M current_rot;
    M current_rot2;
    M m;
  };



}; // GameApi namespace

#endif
