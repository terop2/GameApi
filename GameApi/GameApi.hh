

#ifndef GAMEAPI_HH
#define GAMEAPI_HH

#include "deps.hh"
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <iostream>

int add_block();
void set_current_block(int id);
int get_current_block();
void clear_block(int id);



class ASyncData;
class BufferRef;
class MainLoopEnv;
class LoadStream;

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

  class ShaderApi;

#undef rad1
#undef rad2


#define MAC(name) \
  struct name { int id;\
  name(const name &i) : id(i.id) { } \
  name() : id(-1) { } \
  name(int i) : id(i) { }\
  name* clone() const { if (id!=-1) { return new name(id); } return 0; } \
  };
  MAC(GS)
  MAC(HML)
  MAC(UV)
  MAC(AV)
  MAC(H)
  MAC(FBU)
  MAC(FML)
  MAC(CG)
  MAC(INP)
  MAC(IBM)
  MAC(DS)
  MAC(SBM)
  MAC(PN)
  MAC(DC)
  MAC(CBB)
  MAC(CFB)
  MAC(SI)
  MAC(PLP)
  MAC(PLL)
  MAC(PLF)
  MAC(PR)
  MAC(CMD)
  MAC(FI)
  MAC(SD)
  MAC(GI)
  MAC(FF)
  MAC(IF)
  MAC(PF)
  MAC(SF)
  MAC(ARR)
  MAC(PAR)
  MAC(CPP)
  MAC(PTT)
  MAC(KF)
    MAC(BLK)
  MAC(RUN)
  MAC(EV)
  MAC(AC)
  MAC(MX)
  MAC(Pa)
  MAC(Va)
  MAC(AS)
  MAC(MC)
  MAC(MS)
  MAC(US)
  MAC(MT)
  MAC(TL)
  MAC(MN)
  MAC(CP)
  MAC(DR)
  MAC(OM)
  MAC(FO)
  MAC(WV)
  MAC(BM)
  MAC(BMA)
  MAC(VBM)
  MAC(BB)
  MAC(FB)
  MAC(CBM)
  MAC(SP)
  MAC(SA)
  MAC(PT)
  MAC(V)
  struct M { float mat[16]; M(const M &ii) { for(int i=0;i<16;i++) mat[i]=ii.mat[i]; }  M() { }}; // matrix
  MAC(LN)
  MAC(RM)
  MAC(IS)
  MAC(O)
  MAC(P)
  MAC(OO)
  MAC(PP)
  MAC(PA)
  MAC(LA)
  MAC(TN)
  MAC(SH)
  MAC(C)
  MAC(CO)
  MAC(F)
  MAC(FA)
  MAC(Ft)
  MAC(VF)
  MAC(PPT)
  MAC(PSP)
  MAC(MA)
  MAC(S)
  MAC(Vb)
  MAC(T)
  MAC(E)
  MAC(L)
  MAC(LAY)
  MAC(MP)
  MAC(MV)
  MAC(LL)
  MAC(ME)
  MAC(ST)
  MAC(TY)
  MAC(PS)
  MAC(ID)
  MAC(Str)
  MAC(Fi)
  MAC(Op)
  MAC(D)
  MAC(Ht)
  MAC(BF)
  MAC(VA)
  MAC(VAA)
  MAC(VX)
  MAC(PL)
  MAC(PLA)
  MAC(TX)
  MAC(TXID)
  MAC(TXA)
  MAC(TR)
  MAC(VV)
  MAC(Q)
  MAC(DO)
  MAC(PC)
  MAC(SV)
  MAC(FOA)
  MAC(COV)
  MAC(LI)
  MAC(LLA)
  MAC(FD)
  MAC(VO)
  MAC(PTS)
  MAC(MSA)
  MAC(PTA)
  MAC(RD)
  MAC(FBO)
  MAC(SM)
  MAC(TRK)
  MAC(WAV)
  MAC(TBUF)
  MAC(SFO)
  MAC(W)
  MAC(WM)
  MAC(FtA)
  MAC(ML)

  template<class T>
  struct A { int id; };
  MAC(EX)
  MAC(PH)
  MAC(TS)
  MAC(CT)
  MAC(CC)
  MAC(IM)
  MAC(FFi)
  MAC(VFi)
  MAC(MB)
  MAC(PKG)
  MAC(ATT)
#undef MAC
  
  //template<class T>
  //struct E { int id; };
  //struct A { int id; };

  
  struct EveryApi;
  std::vector<PTS> arr_to_pts_arr(EveryApi &ev, ARR a);


struct ExecuteEnv
{
  std::vector<std::string> names;
  std::vector<std::string> values;
  std::map<std::string, int> envmap;
  std::vector<int> env; // contains indexes to EnvImpl/all vectors
};



  template<class P, class R>
  class FunctionCb
  {
  public:
    virtual R Map(P p) const=0;
  };

  class Env;
  struct EveryApi;

  std::pair<int,std::string> execute_codegen(Env &e2, EveryApi &ev, std::string text, ExecuteEnv &e);

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

  class ASyncVec;
  
class Env
{
public:
  IMPORT Env();
  IMPORT void free_memory();
  IMPORT std::vector<int> store_counts();
  IMPORT void free_to_counts(std::vector<int> vec);
  IMPORT void free_temp_memory();
  IMPORT void async_load_url(std::string url, std::string homepage);
  IMPORT void async_load_all_urls(std::vector<std::string> urls, std::string homepage);
  IMPORT void async_load_callback(std::string url, void (*fptr)(void*), void *data);
  IMPORT void async_rem_callback(std::string url);
  IMPORT ASyncVec *get_loaded_async_url(std::string url);
  IMPORT ~Env();
  IMPORT static Env *Latest_Env();
private:
  Env(const Env &);
  void operator=(const Env &);
public:
  void *envimpl;
  friend struct EnvImpl;
};
#if 0
class SoundApi
{
public:
  SoundApi(Env e) : e(e) { }
  SD physical_key_press(ML ml, WVI wv); 
private:
  Env &e;
};
#endif

class BlockerApi
{
public:
  BlockerApi(Env &e) : e(e) { }
  IMPORT BLK game_window(EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration);
  IMPORT BLK game_seq(EveryApi &ev, std::vector<BLK> vec);
  IMPORT void run(BLK blk);
  IMPORT RUN game_window2(EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration);
  IMPORT RUN game_window_2nd_display(EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration);
  IMPORT RUN vr_window(EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float dura, bool invert, bool translate);
  IMPORT ML vr_submit(EveryApi &ev, TXID left, TXID right);
  IMPORT ML vr_submit_ml(EveryApi &ev, ML ml, TXID left, TXID right, bool invert, bool translate);
  IMPORT void run2(EveryApi &ev, RUN spl);
  IMPORT RUN run_seq(EveryApi &ev, std::vector<RUN> vec);
private:
  Env &e;
};

#ifdef F_MAINLOOP_API
class MainLoopApi
{
public:
	IMPORT MainLoopApi(Env &e);
	IMPORT ~MainLoopApi();
  ML right_mouse_pan(EveryApi &ev, ML next);
  ML mouse_roll_zoom(EveryApi &ev, ML next);
  ML perspective(EveryApi &ev, ML next, float mult, float front_plane, float end_plane);
  ML anim_ML(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5, IF dyn);
  ML glClear(EveryApi &ev);
  GS game_state();
  ML gs_mouse_pos(GS gs, ML ml, int x, int y);
  ML gs_obj_pos(GS gs, ML ml, int x, int y, int z);
  ML gs_delta(GS gs, ML ml, int p, int n, float delta);
  ML gs_time(GS gs, ML ml, int t);
  ML gltf_mesh_all_env(EveryApi &ev, std::string base_url, std::string url, BM diffuse, BM specular, BM bfrd);
  ML transparent(ML ml);
  ML async_gltf(ML ml, std::string base_url, std::string url);
  ML tunnel_tree(EveryApi &ev, std::vector<P> faces, std::vector<MN> moves, std::string url, MT mat2);
  ML edit_3d(EveryApi &ev, P p, float radius);
  P edit_3d_p(EveryApi &ev);
  //ML gltf_joint_matrices(EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index, ML next);
  ML choose_ml_from_status(ML connect, ML download, ML prepare);
  ML key_ml(std::vector<ML> vec, std::string keys);
  ML async_url(std::string url, ML ml);
  ARR gltf_anim_skeleton(EveryApi &ev, std::string base_url, std::string url, int skin_num, int animation, int channelk, int num_keyframes);
  IMPORT ML memmap_window2(EveryApi &ev, std::string url);
  IMPORT ML memmap_window3(EveryApi &ev, std::string url_1, std::string url_2, std::string url_3, std::string url_4, std::string url_5, std::string url_6);
  IMPORT ML ml_load_um(EveryApi &ev, std::string url);
  IMPORT ML memmap_window(EveryApi &ev, PKG p);
  IMPORT PKG load_um(DS st);
  IMPORT MB network(std::string url, std::string homapge);
  IMPORT BM img_decode(MB memblock);
  
  IMPORT PTS whack_a_mole_explosion(EveryApi &ev);
  IMPORT ML whack_a_mole_flag_bmchooser(ML ml);
  IMPORT ML whack_a_mole_collision(ML ml);
  IMPORT ML disable_z_buffer(ML ml);
  IMPORT ML random_instantiate(EveryApi &ev, ML ml, std::vector<MN> vec, float start_time, float time_step, float random_chance);
  IMPORT BM flip_bitmap_if_mobile(EveryApi &ev, BM bm);
  IMPORT ML isometric(ML ml, float y_angle, float x_angle, float translate);
  IMPORT ML joystick_to_wasd(ML ml);
  IMPORT ML joystick_printer(ML ml);
  IMPORT ML dyn(D d);
  IMPORT ML print_stats(P p);
  IMPORT ML chai_mainloop(EveryApi &ev, std::string url);
  IMPORT ML chai_mainloop2(EveryApi &ev, std::string url, GameApi::ML ml);
  IMPORT ML bind_obj_type(std::string name, GameApi::P obj, GameApi::MN move, GameApi::MT mat, float radius);
  IMPORT ML bind_obj_type(GameApi::EveryApi &ev, std::string url);
  IMPORT ML read_obj_pos(std::string url);
  IMPORT ML parse_areatype(EveryApi &ev, std::string url, GameApi::FB heightmap, GameApi::BM top_texture, GameApi::BM side_texture);
  IMPORT ML create_landscape(EveryApi &ev, std::string url);
  IMPORT ML create_objs(EveryApi &ev, int area_id);
  IMPORT HML emscripten_frame(EveryApi &ev, RUN r, std::string homepage);
  IMPORT HML emscripten_frame2(EveryApi &ev, RUN r, std::string homepage);
  IMPORT void init_window(int screen_width = 800, int screen_height=600, std::string window_title="GameApi", bool vr_init=false);
  IMPORT void init(SH sh, int screen_width = 800, int screen_height = 600);
  IMPORT void init_3d(SH sh, int screen_width = 800, int screen_heigth = 600);
  IMPORT void nvidia_init();
  IMPORT void transfer_sdl_surface(MainLoopApi &orig);
  IMPORT void clear(unsigned int col = 0xff000000);
  IMPORT void clear_3d_transparent();
  IMPORT void clear_3d(unsigned int col = 0xff000000);
  IMPORT void switch_to_3d(bool b, SH sh, int screen_width=800., int screen_height = 600);
  IMPORT ML restart_game(EveryApi &ev, ML ml, int key);
  IMPORT ML matrix_range_check(EveryApi &ev, ML ml, ML ml2, std::string url); // this uses restart_game.
  IMPORT LI gltf_skeleton(EveryApi &ev, std::string base_url, std::string url, int start_node);
  IMPORT ML gltf_mesh( EveryApi &ev, std::string base_url, std::string url, int mesh_id );
  IMPORT ML gltf_mesh_all( EveryApi &ev, std::string base_url, std::string url );
  IMPORT ML gltf_node( EveryApi &ev, std::string base_url, std::string url, int node_id );
  IMPORT ML gltf_scene( EveryApi &ev, std::string base_url, std::string url, int scene_id );
  //IMPORT ML gltf_anim( EveryApi &ev, std::string base_url, std::string url, int animation, int channel, int mesh_index, int prim_index, MT mat );
  IMPORT ML gltf_anim2( EveryApi &ev, std::string base_url, std::string url, int animation, int channel);
  IMPORT ML gltf_anim4( EveryApi &ev, std::string base_url, std::string url, int animation, int channel);
  IMPORT ML gltf_scene_anim(EveryApi &ev, std::string base_url, std::string url, int scene_id, int animation);
  IMPORT ML flip_scene_if_mobile(EveryApi &ev, ML ml);
  IMPORT ML flip_scene_x_if_mobile(EveryApi &ev, ML ml);
  IMPORT ML activate_item(ML ml, ML def);
  IMPORT ML activate_array(std::vector<ML> vec);
  IMPORT ARR activate_arr_arr(std::vector<ML> arr);
  IMPORT ML slow_activate_item(ML ml,int i);
  IMPORT BM framebuffer_bitmap(FBU framebuffer);
  IMPORT ML display_background(EveryApi &ev, ML ml);
  IMPORT ML small_window(EveryApi &ev, ML ml, int x, int y, int sx, int sy);
  IMPORT ML drag_drop_area(EveryApi &ev, ML mainloop, RUN (*fptr)(Env &e, EveryApi &ev, std::string filename));
  IMPORT PTS score_hide_pts(PTS pts);
  IMPORT IF score_set_hide();
  //IMPORT ML looking_glass(EveryApi &ev, ML ml);
  //IMPORT TXID looking_glass_txid(EveryApi &ev, ML ml);
  //IMPORT ML looking_glass_lib(TXID id, int sx, int sy, int x, int y);
  IMPORT ML looking_glass_full(EveryApi &ev, ML ml, float amount, int mode, int start, int end);
  IMPORT void alpha(bool enabled);
  void alpha_1(bool enabled);
  IMPORT void depth_test(bool enabled);
  IMPORT void transparency(bool enabled);
  IMPORT void cursor_visible(bool enabled);
  IMPORT void antialias(bool enabled);
  IMPORT void outline_first();
  IMPORT void outline_second();
  IMPORT void outline_third();
  IMPORT void outline_disable();
  IMPORT float get_time();
  IMPORT void advance_time(float val);
  IMPORT float get_delta_time();
  IMPORT void reset_time();
  IMPORT int get_framenum();
  IMPORT void swapbuffers();
  IMPORT void finish();
  IMPORT BM screenshot();
  IMPORT float fpscounter(bool print=true);
  IMPORT ML fps_display(EveryApi &ev, ML ml, std::string font);
  IMPORT ML score_display(EveryApi &ev, ML ml, std::string font);
  IMPORT ML time_display(EveryApi &ev, ML ml, std::string font, float start_time);

  IMPORT UV face_fetcher(P p, int facenum);
  IMPORT void profile(std::string label, bool start);
  IMPORT void print_profile();
  IMPORT void delay(int ms);
  IMPORT unsigned int random();
  IMPORT unsigned int rand_max();
  IMPORT PT random_point_3d(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
  IMPORT PT random_point_2d(float start_x, float end_x,float start_y, float end_y,float z);
  IMPORT V random_dir_vector_2d_xy(float length); 
  IMPORT V random_dir_vector_2d_xz(float length); 
  IMPORT V random_dir_vector_3d(float length);
  IMPORT int get_screen_width();
  IMPORT int get_screen_height();
  IMPORT void set_corner(int x, int y, int screen_sx, int screen_sy);
  IMPORT int get_corner_x();
  IMPORT int get_corner_y();
  IMPORT int get_screen_rect_sx();
  IMPORT int get_screen_rect_sy();
  IMPORT int get_screen_sx();
  IMPORT int get_screen_sy();
  IMPORT void set_screen_size(int sx, int sy);
  IMPORT void set_viewport(int x, int y, int sx, int sy);

  IMPORT bool logo_iter();
  IMPORT void save_logo(EveryApi &ev);
  IMPORT void display_logo(EveryApi &ev);
  IMPORT bool seamless_iter();
  IMPORT void display_seamless(EveryApi &ev);
  IMPORT ML debug_obj(EveryApi &ev);
  IMPORT ML restart_screen(EveryApi &ev, ML ml, std::string fontname);
  IMPORT ML save_ds_ml(EveryApi &ev, std::string output_filename, P p);
  struct Event
  {
    int type;
    int ch;
    PT cursor_pos;
    int button;
    int mouse_wheel_y;
    bool joy0_button0;
    bool joy0_button1;
    bool joy0_button2;
    bool joy0_button3;

    bool joy1_button0;
    bool joy1_button1;
    bool joy1_button2;
    bool joy1_button3;

    int joy0_current_axis=0;
    int joy0_axis0 =0;
    int joy0_axis1 =0;
    int joy0_axis2 =0;
    int joy0_axis3 =0;
    int joy0_axis4 =0;
    int joy1_current_axis=0;
    int joy1_axis0 =0;
    int joy1_axis1 =0;
    int joy1_axis2 =0;
    int joy1_axis3 =0;
    int joy1_axis4 =0;

    int joy0_ball0 = 0;
    int joy0_ball1 = 0;
    int joy1_ball0 = 0;
    int joy1_ball1 = 0;
    
    
    bool last;
    std::string drag_drop_filename;
  };
  struct DoubleTapState { DoubleTapState() : start_frame(0), in_between(false) { } int start_frame; bool in_between; };
  IMPORT bool ch_doubletap_detect(Event &e, int exprire_timer_count, int ch, DoubleTapState &state);
  IMPORT Event get_event();
  void waittof();
  SP screenspace();
  void execute_ml(ML ml, SH color, SH texture, SH texture_2d, SH arr_texture, M in_MV, M in_T, M in_N, int screen_width, int screen_height);
  void event_ml(ML ml, const Event &e);
  IMPORT ML array_ml(GameApi::EveryApi &ev, std::vector<ML> vec);
  IMPORT FML array_fml(std::vector<FML> vec);
  //IMPORT ML timing_ml(std::vector<ML> vec, float duration);
  IMPORT ML dyn_points(EveryApi &ev, ML ml, MN move, int pointnum, float pos_x, float pos_y, float pos_z);
  ML seq_ml(std::vector<ML> vec, float time);
  ML seq_ml_score(ML ml1, ML ml2, int score);
  ML timed_tmp_seq_ml(ML curr, ML end, float start_time, float end_time, float show_duration, int key);
  ML collision_seq_ml(ML curr, ML end, std::string obj1, std::string obj2, float show_duration);
  ML collision_gen_key(ML curr, std::string obj1, std::string obj2, int key, float duration);
  ML collision_detection(EveryApi &ev,float player_size,float enemy_size, ML normal_game_screen, ML gameover_screen);

  M in_MV(EveryApi &ev, bool is_3d);
  M in_T(EveryApi &ev, bool is_3d);
  M in_N(EveryApi &ev, bool is_3d);
  M in_P(EveryApi &ev, bool is_3d);
  std::string get_homepage_url();
  void set_homepage_url(std::string url);
  void set_seamless_url(std::string url);

  ML load_song(EveryApi& ev, ML next, std::string url);
  ML skybox(EveryApi &ev, BM land, BM sky); // 100x100 bm's required
  ML scale_2d_screen(EveryApi &ev, ML orig, float sx, float sy);
  ML keyboard_toggle(ML m1, ML m2, int key);
  ML touch_rotate(EveryApi &ev, ML ml, bool leftright, bool topdown, float x_speed, float y_speed);
  P load_P_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  ARR load_P_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  ML load_ML_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  MN load_MN_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  MT load_MT_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);

  ARR load_ML_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  BM load_BM_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  ARR load_BM_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5);
  ML prepare_pts(ML ml, PTS pts);
  ML depthfunc(ML ml, int val); 
  ML blendfunc(ML ml, int val, int val2);
  ML record_keypresses(ML ml, std::string output_filename);
  ML playback_keypresses(ML ml, std::string input_url);
  ML setup_hmd_projection(EveryApi &ev, ML ml, bool eye, bool is_standard, float n, float f, bool translate);
  
  DS load_ds_from_mem(const unsigned char *buf, const unsigned char *end);
  DS load_ds_from_disk(std::string filename);
  DS load_ds_from_disk_incrementally(std::string filename);
  void save_ds(std::string output_filename, DS ds);
  std::string ds_to_string(DS ds);
  
  INP cursor_keys_normal_move();
  INP cursor_keys_rotate_move(float speed_rot, float pos_forward, float pos_normal, float pos_backward);
  INP move_sequence(float start_time, float time_per_char, std::string str);
  INP move_speed(INP orig, float speed);
  INP gravity(INP im, IBM bitmap, float start_x, float end_x, float start_y, float end_y, float speed);
  INP move_collision(IBM scene, float start_x, float end_x, float start_y, float end_y, float s_x, float m_x, float e_x, float s_y, float m_y, float e_y, float speed_up, float speed_down, float speed_left, float speed_right, float speed_gravity);
  ML move_in(EveryApi &ev, ML ml, INP in);
  ML score_adder(EveryApi &ev, ML ml, O o, MN transform, int enter_score, int leave_score, int dyn_point, float timeout);
  ML score_adder2(EveryApi &ev, ML ml, O o,PTS points, MN transform, int enter_score, int leave_score, int dyn_point, float timeout, IF fetcher);
  ML score_hidder(EveryApi &ev, ML ml, O o, int max_count);
  IF state_int_fetcher(std::string url, std::string states);
  MN state_speed_movement(MN mn, std::string url, std::string states, std::string x_speeds, std::string y_speeds, std::string z_speeds);
private:
  MainLoopApi(const MainLoopApi&);
  void operator=(const MainLoopApi&);
  float frame;
  float time;
  Env &e;
  void *priv;
};
#endif

class MixedApi
{
public:
  MixedApi(Env &e) : e(e) { }
  // constructing
  MX mx_float(float val);
  MX mx_int(int val);
  MX mx_bool(bool val);
  MX mx_point(float x, float y, float z);
  MX mx_vector(float dx, float dy, float dz);
  MX mx_color(int r, int g, int b, int a);
  MX mx_pair(std::string name, MX val);
  MX mx_string(std::string value);
  MX mx_array(std::vector<MX> vec);

  // accessing
  int mx_size(MX arr);
  MX mx_index(MX arr, int idx);
  MX mx_find(MX arr, std::string name);
  float mx_to_float(MX val, float def=0.0);
  int mx_to_int(MX val, int def=-1);
  std::vector<std::string> mx_names(MX val);
  std::vector<MX> mx_values(MX val);
private:
  Env &e;
};


#ifdef F_SPRITE_API
class SpriteApi
{
public:
	IMPORT SpriteApi(Env &e);
	IMPORT ~SpriteApi();
	IMPORT void spritepos(BM bm, float x, float y);
	IMPORT void preparesprite(BM bm, int bbm_choose = -1);

  IMPORT VA create_vertex_array(BM bm, bool progress=false);
  IMPORT void clear_arrays(VA va);
        IMPORT void delete_vertex_array(VA va);
        IMPORT void update_vertex_array(VA va, BM bm);
        IMPORT ML update_vertex_array_ml(VA va, BM bm);
        IMPORT void clipping_sprite(VA va, int sx, int sy, float tex_l, float tex_t, float tex_r, float teb_b);
	IMPORT void render_sprite_vertex_array(VA va);
  IMPORT ML vertex_array_render(EveryApi &ev, BM bm);
  IMPORT ML render_sprite_vertex_array_ml(EveryApi &ev, BM bm);

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
  IMPORT std::vector<BM> bitmap_anim(std::function<BM (float)> f, std::vector<float> key_frames);
  IMPORT ML bitmap_anim_ml(EveryApi &ev, std::vector<BM> vec, std::vector<float> key_frames, float repeat_time);
  IMPORT ML turn_to_2d(EveryApi &ev, ML ml, float tl_x, float tl_y, float br_x, float br_y);
  IMPORT ML alt_ml_array(EveryApi &ev, std::vector<ML> vec, float start_time, float time_delta, bool repeat);
  IMPORT ARR sprite_atlas_x(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_x, int count);
  IMPORT ARR sprite_atlas_y(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_y, int count);
  IMPORT ARR sprite_atlas_xy(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_x, int delta_y, int count_x, int count_y);
private:
  SpriteApi(const SpriteApi&);
  void operator=(const SpriteApi&);
  void *priv;
  Env &e;
};
#endif

#ifdef F_TEXTURE_API
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
  IMPORT TXID bufferref_to_txid(TXID old, const BufferRef &buf);
  IMPORT TXID webcam_txid(int num);
  IMPORT TXID webcam_txid_slow(EveryApi &ev, int num);
  IMPORT TXID webcam_txid_win(EveryApi &ev, int sx, int sy, int num);
  IMPORT ML vr_overlay(TXID id, std::string key, std::string name, int sx, int sy);
  IMPORT std::vector<TXID> prepare_many(EveryApi &ev, std::vector<BM> vec, std::vector<int> types=std::vector<int>(),bool mipmaps=true);
  IMPORT TXID prepare_cubemap(EveryApi &ev, BM right, BM left, BM top, BM bottom, BM back, BM front);
	IMPORT void use(TXID tx, int i = 0);
  IMPORT void use_many(std::vector<TXID> vec, int i=0);
	IMPORT void unuse(TXID tx);
  IMPORT void delete_texid(TXID tx);
  IMPORT void delete_texid(std::vector<TXID> vec);
  IMPORT VA bind(VA va, TXID tx);
  IMPORT VA bind_many(VA va, std::vector<TXID> tx, std::vector<int> types=std::vector<int>());
        IMPORT VA bind_cubemap(VA va, TXID id);
	IMPORT VA bind_arr(VA va, TXA tx);
        IMPORT TXA prepare_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy);
        IMPORT BM to_bitmap(TXID id);
  IMPORT ML forward_to_txid(VA va, ML mainloop, TXID id);
private:
  TextureApi(const TextureApi&);
  void operator=(const TextureApi&);
  Env &e;
  int count;
};
#endif

#ifdef F_GRID_API
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
#endif

#ifdef F_BITMAP_API
class BitmapApi
{
public:
	IMPORT BitmapApi(Env &e);
	IMPORT ~BitmapApi();
  IMPORT CBM scale_bitmap2(BM bm);
  IMPORT ML save_raw(BM bm, std::string filename);
  IMPORT BM lightmap_bitmap(int sx, int sy, P faces, P faces2, int face, float light_dir_x, float light_dir_y, float light_dir_z);
  IMPORT BM circular_gradient(int sx, int sy, unsigned int center_color, unsigned int edge_color);
  IMPORT BM script_bitmap(std::string url, int sx, int sy);
  IMPORT ML grid_ml(EveryApi &ev, ML next, IBM map, float y, float pos_x, float pos_z, float x_vec_x, float x_vec_z, float z_vec_x, float z_vec_z, int start_x, int start_z, float frame_inc);
  IMPORT P p_world_from_bitmap(EveryApi &ev, std::vector<P> vec, IBM world, float pos_x, float pos_y, float pos_z, float dx, float dz, float y);
  IMPORT BM scale_to_size(BM bm, int sz);
  IMPORT BM median_filter(BM bm, int sx, int sy);
        IMPORT BM newbitmap(int sx, int sy, unsigned int color = 0x00000000);
	IMPORT BM function(std::function<unsigned(int, int)> f, int sx, int sy);
	IMPORT BM transform(BM orig, std::function<unsigned int(int, int, unsigned int)> f);
	IMPORT BM newintbitmap(char *array, int sx, int sy, std::function<int(char)> f);
        IMPORT IBM intbitmap_loader(std::string url);
  IMPORT BM calculate_baked_light(P p, P p2, BM texture, int count, float light_pos_x, float light_pos_y, float light_pos_z, float shadow_darkness, float softness);
  IMPORT BM color_bm(BM bm, unsigned int color);
  IMPORT BB choose_color(BM bm, unsigned int c1, unsigned int c2);
  IMPORT BM intbitmap_bm(IBM ibm);
  IMPORT BM chai_bm(std::string url, int sx, int sy);
  IMPORT BM bump_map(FB fb, float h);
	IMPORT BM newcolorbitmap(char *array, int sz, int sy, std::function<unsigned int(char)> f);
	IMPORT BM newtilebitmap(int sx, int sy, int tile_sx, int tile_sy);
	IMPORT BM loadbitmap(std::string filename);
        IMPORT BM loadbitmapfromurl(std::string url);
	IMPORT BM loadtilebitmap(std::string filename, int tile_sx, int tile_sy);
	IMPORT BM loadposbitmap(std::string filename);
	IMPORT BM findtile(BM tile_bitmap, int x, int y);
  IMPORT BM compose_x(BM bm1, BM bm2);
  IMPORT BM compose_y(BM bm1, BM bm2);
  IMPORT BM border(BM, int left ,int right ,int top ,int bottom);
  IMPORT BM add_shape_border(EveryApi &ev, BM bm, float dist_field_size, float start_range, float end_range, int r, int g, int b, int a, int border_size);
	BM subbitmap(BM orig, int x, int y, int width, int height);
	IMPORT BM subbitmapimage(BM orig, int r_start_range, int r_end_range, int g_start_range, int g_end_range, int b_start_range, int b_end_range, unsigned int empty_color);
  IMPORT BM plus_bitmap(BM bm, BM bm2);
  IMPORT BM noise_vectors(int sx, int sy);
  IMPORT IBM random_int_bitmap(int sx,int sy,int min_value,int max_value);
  IMPORT IBM convert_fb_to_ibm_bitmap(FB fb, float start, float d);
  IMPORT VX convert_ibm_to_vx(IBM bm);
	IMPORT BM growbitmap(BM small_orig_bitmap, int l, int t, int r, int b);
	IMPORT BM blitbitmap(BM bg, BM orig, int x, int y);
	IMPORT BM blitbitmap_fb(BM bg, BM orig, int x, int y, FB mask);
	IMPORT BM blitbitmap_bb(BM bg, BM orig, int x, int y, BB mask);
	IMPORT BM anim_array(BM *array, int size);
	IMPORT BM modify_bitmap(BM orig, BM bm, int x, int y);
	IMPORT BM interpolate_bitmap(BM orig1, BM orig2, float x); // x=[0..1]
	IMPORT BM repeat_bitmap(BM orig, int xcount, int ycount);
	IMPORT BM sample_bitmap(BM orig, float xmult, float ymult, float x, float y);
  IMPORT BM scale_bitmap(EveryApi &ev, BM orig, int sx, int sy);
  IMPORT BM scale_bitmap_fullscreen(EveryApi &ev, BM orig);
        IMPORT BM world_from_bitmap(std::function<BM(int)> f, BM int_bm, int dx, int dy);
  IMPORT ARR cubemap(BM bm);
  IMPORT BM bm_cubemap(EveryApi &ev, BM left, BM top, BM middle, BM right, BM back, BM down, int sx, int sy);
  IMPORT BM world_from_bitmap2(EveryApi &ev, std::vector<BM> v, std::string filename, std::string chars, int dx, int dy, int sx, int sy);
  IMPORT BM world_from_bitmap3(EveryApi &ev, std::vector<BM> v, std::string url, std::string chars, int dx, int dy);
        IMPORT BM dup_x(BM orig);
	IMPORT BM flip_x(BM orig);
	IMPORT BM flip_y(BM orig);
  IMPORT BM rot90(BM orig);
        IMPORT BM alpha_color(BM orig, unsigned int color_key);
  IMPORT BM gradient(PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy);
  IMPORT BM radial_gradient(int sx, int sy, PT pos, float r1, float r2, unsigned int color_1, unsigned int color_2);
  IMPORT BM conical_gradient(int sx, int sy, float x, float y, float angle1, float angle2, unsigned int color_1, unsigned int color_2);
  //BM bitmapandtypes(BM bm, BM (*fptr)(int)); // bm has ints in it
  IMPORT SP space(BM bm);
  
  IMPORT BM addsubrects(BM orig); // use subbitmap with orig bitmap.
  IMPORT void savebitmap(BM orig, std::string filename, bool alpha=false);
  IMPORT ML savebitmap_ml(EveryApi &ev, BM bm, std::string filename, bool alpha, float time);
  IMPORT void save_png(BM bm, std::string filename);
  IMPORT ML save_png_ml(EveryApi &ev, BM bm, std::string filename);
  IMPORT BM mandelbrot(bool julia,float start_x, float end_x, float start_y, float end_y, float xx, float yy, int sx, int sy,int count);
  IMPORT BM mandelbrot2(bool julia,float start_x, float end_x, float start_y, float end_y, float xx, float yy, int sx, int sy,int count);
  IMPORT BM chessboard(int tile_sx, int tile_sy, int count_x, int count_y, unsigned int c1, unsigned int c2);
  IMPORT BM color_range(BM orig, unsigned int source_upper, unsigned int source_lower, unsigned int target_upper, unsigned int target_lower);
  IMPORT BM memoize(BM orig);
  IMPORT BM memoize_all(BM orig);
  IMPORT BM persistent_cache(BM orig, std::string filename);
  IMPORT BM alt(std::vector<BM> vec, int index);
  IMPORT BM simple_blur(BM bm, float center, float left, float right, float top, float bottom); // this is dangerous operation
  IMPORT int intvalue(BM bm, int x, int y);
  IMPORT unsigned int colorvalue(BM bm, int x, int y);
  IMPORT int size_x(BM bm);
  IMPORT int size_y(BM bm);
  IMPORT void prepare(BM bm);
  
  IMPORT BMA empty_array();
  BMA array(BM *array, int size);
  IMPORT BM array_elem(BMA array, int i);
  IMPORT BM Indicator(int sx, int sy, int g_ind);
  IMPORT BM avg(BM bm1, BM bm2);
  IMPORT BM fix_edges(BM bm);

  IMPORT FB gradient_fb(int sx, int sy, float val, float val2, bool flip);
  IMPORT FB radial_fb(int sx, int sy, float x, float y, float r, float val_at_zero, float val_at_r);
  IMPORT FB sin_fb(FB gradient);
  IMPORT FB plus_fb(FB f1, FB f2);
  IMPORT FB mul_fb(FB f, float mul);
  IMPORT TXID txid_from_heavy(H heavy);
  IMPORT H bitmap_prepare_heavy(EveryApi &ev, H bitmap_gen, int scanlines_per_slot);
  IMPORT H network_heavy(std::string url, std::string homepageurl, H timing_heavy);
  IMPORT H bitmap_heavy(BM bm, H timing);
  IMPORT H png_heavy(EveryApi &ev, H data, std::string url, int texture_unit);
  IMPORT H mtl_heavy(EveryApi &ev, H net, std::string url_prefix, int texture_unit_delta);
  IMPORT H array_heavy(std::vector<H> vec);
  IMPORT H thread_heavy(H threaded);
  IMPORT H timing_heavy(int num_frames);
  IMPORT TXID dyn_fetch_bitmap(EveryApi& ev, std::string url, int time, int texture_unit);
  IMPORT std::vector<TXID> dyn_fetch_mtl(EveryApi &ev, std::string mtl_url, ML ml2);
  IMPORT ML txidarray_from_heavy(EveryApi &ev, H heavy, std::vector<TXID> *vec, ML ml, int start_range, int end_range, int heavycount);

  IBM create_ibm(std::vector<BB> vec);
  BB choose_bool(IBM bm, int val);
  ARR choose_ints(IBM bm, int count);
private:
  BitmapApi(const BitmapApi&);
  void operator=(const BitmapApi&);
  void *priv;
  Env &e;
};
#endif

#ifdef F_FLOAT_API
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
#endif

#ifdef F_ARRAY_API
template<class T>
class ArrayApi
{ // handle arrays
public:
  ArrayApi(Env &e) : e(e) { }
  A<T> array(std::vector<T> vec);
private:
  Env &e;
};
#endif

#ifdef F_FLOAT_ARRAY_API
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
#endif

#ifdef F_FONT_API
class FontApi
{
public:
  IMPORT FontApi(Env &e);
  IMPORT ~FontApi();
  IMPORT IF quake_area_fetcher(float start_x, float end_x, float start_z, float end_z);
  IMPORT Ft newfont(std::string filename, int sx, int sy);
  IMPORT BM glyph(Ft font, long idx);
  IMPORT LI glyph_outline(Ft font, long idx, float sx, float sy);
  IMPORT PL glyph_plane(Ft font, long idx, float sx, float sy, float dx, float dy);
  IMPORT BM font_string(Ft font, std::string str, int x_gap);
  IMPORT FB glyph_fb(Ft font, long idx);
  IMPORT BB glyph_bb(Ft font, long idx);
  
  IMPORT FtA font_atlas_info(EveryApi &ev,Ft font, std::string chars, float sx, float sy, int y_delta);
  IMPORT BM font_atlas(EveryApi &ev, Ft font, FtA atlas, float sx, float sy);
  IMPORT BM font_string_from_atlas(EveryApi &ev, FtA atlas, BM atlas_bm, std::string str, int x_gap);
  IMPORT void save_atlas(FtA atlas, std::string filename);
  IMPORT FtA  load_atlas(std::string filename);
  IMPORT ARR font_string_array(Ft font, std::string s, int x_gap);
  IMPORT ARR font_string_array2(FI font, std::string s);
  IMPORT SF time_string_fetcher(EveryApi &ev);
  IMPORT SF score_string_fetcher(std::string id, std::string label, int numdigits); // use score_display instead.
  IMPORT IF char_fetcher_from_string(SF string_fetcher, std::string alternatives, int idx);
  IMPORT ML dynamic_character(EveryApi &ev, std::vector<BM> vec, IF fetcher, int x, int y);
  IMPORT FML dynamic_character_frame(EveryApi &ev, std::vector<BM> vec, IF fetcher, int x, int y, int fmt, MN mn);
  IMPORT ML dynamic_polygon(EveryApi &ev, std::vector<P> vec, MT material, IF fetcher);
  IMPORT ML dynamic_string(EveryApi &ev, Ft font, std::string alternative_chars, SF fetcher, int x, int y, int numchars);
  IMPORT FI load_font(std::string ttf_filename, int sx, int sy);
  IMPORT ML save_font_dump(FI font, std::string chars, std::string filename);
  IMPORT FI load_font_dump(std::string url);
  IMPORT GI choose_glyph_from_font(FI font, long idx);
  IMPORT SD draw_text_string_sd(std::vector<GI> glyphs, std::string str, int gap_x, int empty_line_height);
  IMPORT BM string_display_to_bitmap(SD sd, int def);
  IMPORT BM draw_text_string(FI font, std::string str, int x_gap, int empty_line_height);
  IMPORT BM draw_text_large(EveryApi &ev, FI font, std::string url, int x_gap, int empty_line_height, int baseline_separation);
  IMPORT ML dynamic_character2(EveryApi &ev, std::vector<GI> vec, std::string alternatives, IF fetcher, int x, int y);
  IMPORT IF timed_int_fetcher(EveryApi &ev, int start, int end, float start_time, float end_time);
  IMPORT IF movement_int_fetcher(int count, float x_mult, float y_mult, float z_mult);
  IMPORT IF repeat_int_fetcher(IF fetcher, float duration);
  IMPORT IF toggle_button_fetcher(float start_x, float end_x, float start_y, float end_y);
  IMPORT IF hmd_state_fetcher();
  IMPORT IF keypress_int_fetcher(int key, int key_down_value, int key_up_value);
  IMPORT IF x_comp(PF point_fetcher, float start_x, float end_x, int num_steps);
  IMPORT IF y_comp(PF point_fetcher, float start_y, float end_y, int num_steps);
  IMPORT IF z_comp(PF point_fetcher, float start_z, float end_z, int num_steps);
  IMPORT PF point_fetcher_constant(float x, float y, float z);
  IMPORT PF point_fetcher_part(PF point_fetcher, int component, FF float_fetcher);
  IMPORT PF mouse_fetcher();
  IMPORT FF choose_float_fetcher(IF int_fetcher, float a_1, float a_2, float a_3, float a_4, float a_5, float a_6, float a_7);
  IMPORT FF fps_fetcher(EveryApi &ev);
  IMPORT FF time_fetcher2(EveryApi &ev);
  IMPORT FF time_range_fetcher(float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat);
  IMPORT FF time_range_fetcher_key(int key, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat);
  IMPORT FF time_range_fetcher_state(int state, IF states, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat);
  IMPORT FF time_range_fetcher_state_key(int state, int key, IF if_state, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat);
  IMPORT IF score_fetcher(EveryApi &ev);
  IMPORT IF time_fetcher(EveryApi &ev, float start_time);
  IMPORT IF hmd_request_presenting(IF i);
  IMPORT SF float_to_string_fetcher(FF fetcher);
  IMPORT SF int_to_string_fetcher(IF fetcher);
  IMPORT ML ml_chooser(std::vector<ML> vec, IF fetcher);
  IMPORT std::vector<GameApi::BM> bm_array_id_inv(ARR arr);
  IMPORT ARR bm_array_id(std::vector<BM> vec);
private:
  FontApi(const FontApi&);
  void operator=(const FontApi&);
  void *priv;
  Env &e;
};
#endif

#ifdef F_ANIM_API
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
#endif

#ifdef F_EVENT_API
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
#endif

#ifdef F_CURVE_API
class CurveApi
{
public:
  CurveApi(Env &e) : e(e) { }
  IMPORT C line(PT p1, PT p2);
  IMPORT C circle_xy(PT center, float r);
  IMPORT C circle_xz(PT center, float r);
  IMPORT C circle_xy_wave(float start_r, float end_r, WV wave, float length);
  IMPORT C linear(std::vector<PT> vec);
  IMPORT C bezier(std::vector<PT> vec);
  

  IMPORT C scale(C curve, float mx, float my, float mz);
  IMPORT C trans(C curve, float dx, float dy, float dz);
  //C constantspeed(C curve, float speed);
  IMPORT C compose(std::vector<C> vec);
  //C plane(C curve2d, PT pos, PT u_x, PT u_y);
  IMPORT C change_length(C curve, float new_length);
  IMPORT C split(C orig_curve, float start_var, float end_var);

  IMPORT PA curve_product(C c1, C c2, PT c2_center);
  IMPORT P patch_sample(PA patch, int sx, int sy);
  IMPORT C x_curve(PA patch, float y);
  IMPORT C y_curve(PA patch, float x);

  IMPORT PT pos(C curve, float p);
  
  IMPORT PTS sample(C input_curve, int num_samples);
  IMPORT LI to_lines(C curve, int num_lines);
  IMPORT CG curve_group_from_lines(LI li);
  IMPORT LI lines_from_curve_group(CG curvegroup, int split);
  // curve_pos
  IMPORT CPP xy_sum();
  IMPORT CPP xy_sum2(float xmult, float ymult, float zmult);
private:
  Env &e;
};
#endif

#ifdef F_MATRIX_CURVE_API
class MatrixCurveApi
{
public:
  MatrixCurveApi(Env &e) : e(e) { }
  IMPORT MC from_curve(C curve);
  IMPORT MC circle_xy(float radius);
  IMPORT MC circle_xz(float radius);
  IMPORT MS sample(MC m_curve, int num);
private:
  Env &e;
};
#endif

#ifdef F_CURVES_API
class CurvesApi
{
public:
  CurvesApi(Env &e) : e(e) { }
  CC start(PTS points);
  CC comb(CC prev, PTS points);
  PTS extract(CC curves, int block, float val);
private:
  Env &e;
};
#endif

#ifdef F_FUNCTION_API
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
  std::vector<PT> equalizer(F f1, F f2, float start, float length);
  //float pullback(F a_f, F b_f, F p_a, F p_b, F f_p, float t_0, float t_1);
private:
  FunctionApi(const FunctionApi&);
  void operator=(const FunctionApi&);

  Env &e;
  void *priv;
};
#endif

#ifdef F_SPACE_API
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
#endif

#ifdef F_DUP_API
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
#endif

#ifdef F_TEXT_API
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
#endif

#ifdef F_IMPLICIT_API
class ImplicitApi
{
public:
  ImplicitApi(Env &e) : e(e) { }
  IMPORT IM im_sphere(float r);
  IMPORT IM blob(float c, float c_x, float c_y, float cc_x, float cc_y);
  IMPORT IM im_translate(IM obj, float dx, float dy, float dz);
  IMPORT IM from_distance(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy);
  IMPORT IM from_distance_cyl(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy);
  IMPORT IM from_distance_sph(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy);
  IMPORT FB render_upper(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy);
  IMPORT FB render_lower(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy);

  IMPORT BM render_upper_color(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy);
  IMPORT BM render_lower_color(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy);
private:
  Env &e;
};
#endif

#ifdef F_VOLUME_API
class VolumeApi
{
public:	IMPORT VolumeApi(Env &e);
	IMPORT ~VolumeApi();
  IMPORT BB volumeprojection(O o, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy, int numsamples);
  IMPORT O boolfunction(std::function<bool(float x, float y, float z)> f);
	IMPORT O subvolume(std::function<float(float x, float y, float z)> f, float start_range, float end_range);
  IMPORT PTS random_vol_object(O o, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int numpoints);
  IMPORT O from_polygon(P p, float x, float y, float z);  // point outside of shape.
        IMPORT O from_bool_bitmap(BB b, float dist);
	IMPORT O link_areas(O o, PT p1, PT p2, float d);
	IMPORT O o_sphere(PT center, float radius);
	IMPORT O o_cube(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
	IMPORT O o_cone(PT p1, PT p2, float rad1, float rad2);
	IMPORT O o_torus(PT center, PT u_x, PT u_y, float dist1, float dist2);

  IMPORT O instancing_volume(O o, PTS p);
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

	IMPORT O mandelbulb(float n, float p_x, float p_y, float p_z,float c_x, float c_y, float c_z,float radius,int iterations);
  IMPORT O mandelbrot_volume(bool julia, int count, float yy);

	IMPORT BB plane(O o, int sx, int sy,PT u_p, V u_x, V u_y,float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);

	IMPORT BM render(O object, int sx, int sy, PT ray_0, PT ray_x, PT ray_y, PT ray_z);
  typedef std::function<P (float start_x, float end_x, 
			float start_y, float end_y, 
			float start_z, float end_z, 
			unsigned int color)> fptrtype;
  IMPORT P rendercubes(O object,fptrtype fptr,int size,float wholesize); // marching cubes algo
  IMPORT PTS instanced_positions(O object,int size,float wholesize);

  IMPORT P rendercubes2(EveryApi &ev, O object,fptrtype fptr,int sx, int sy, int sz,float world_x, float world_y, float world_z); // normal for loop algo
  IMPORT P rendercubes3(O object,int sx, int sy, int sz,float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
  IMPORT PTS instanced_positions(O object,int sx, int sy, int sz,float start_x, float end_x,float start_y, float end_y, float start_z, float end_z);
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
#endif

#ifdef F_SHADER_MODULE_API
class ShaderModuleApi
{
public:
  IMPORT ShaderModuleApi(Env &e) : e(e) { }
  IMPORT SFO empty();
  IMPORT SFO function(std::function<std::string (std::string id)> function, std::string function_name, std::vector<std::string> param_names = std::vector<std::string>(), std::vector<std::string> arg_values = std::vector<std::string>());
  IMPORT SFO color_function(SFO orig, std::function<std::string (std::string)> function, std::string function_name);
  IMPORT SFO sphere(); // vec3 center, float radius
  IMPORT SFO sphere(PT center, float radius); // ()
  IMPORT SFO cube(); // vec3 tl, vec3 br
  IMPORT SFO cube(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z); // ()
  IMPORT SFO rounded_cube(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z,float r);
  IMPORT SFO texture(SFO obj);
  IMPORT SFO texture_box(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
  IMPORT SFO line(); // vec3 tl, vec3 br, float line_width1, float line_width2
  IMPORT SFO line(float start_x, float start_y, float start_z,float end_x, float end_y, float end_z,float line_width1, float line_width2);
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
  IMPORT SFO blend(SFO obj1, SFO obj2, float k);
  IMPORT SFO trans(SFO obj);
  IMPORT SFO trans(SFO obj, float dx, float dy, float dz);
  IMPORT SFO scale(SFO obj);
  IMPORT SFO scale(SFO obj, float sx, float sy, float sz);
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
  IMPORT SFO spherical(SFO obj, PT tl, PT br, float rr, float rp);
  IMPORT SFO render(SFO obj);
  IMPORT SFO v_render(SFO obj);
  IMPORT SFO f_render_color(SFO obj);
  IMPORT SFO color_from_position(SFO obj, float sx, float sy, float sz);
  IMPORT SFO colormod_from_position(SFO obj, float px, float py, float pz, float sx, float sy, float sz);
  IMPORT ML sfo_to_ml(EveryApi &ev, SFO sfo, float sx, float sy);
private:

  Env &e;
};
#endif

#ifdef F_INTERACTION_API
class InteractionApi
{
public:
  struct Wasd_data {
    bool up=false;
    bool down=false;
    bool left=false;
    bool right=false;
  };
  static void wasd_movement_event(MainLoopApi::Event &e,
			    float &pos_x, float &pos_y, Wasd_data &data,
			    float speed_x, float speed_y);
  static void wasd_movement_frame(MainLoopApi::Event &e,
			    float &pos_x, float &pos_y, Wasd_data &data,
			    float speed_x, float speed_y);
  struct Quake_data {
    bool forward=false;
    bool backward=false;
    bool left=false;
    bool right=false;
    bool side_left=false;
    bool side_right=false;
    float old_pos_x=0.0;
    float old_pos_y=0.0;
    bool mv_mode= false;
  };
  static void quake_movement_event(GameApi::EveryApi &ev, MainLoopApi::Event &e,
			     float &pos_x, float &pos_y, float &rot_y,
			     Quake_data &data,
			     float &speed_x, float &speed_y, float speed, float rot_speed);
  static void quake_movement_frame(GameApi::EveryApi &ev, 
			     float &pos_x, float &pos_y, float &rot_y,
			     Quake_data &data,
			     float &speed_x, float &speed_y, float speed, float rot_speed);
};
#endif

#ifdef F_PICKING_API
class PickingApi
{
public:
  PickingApi(Env &e) : e(e) { }
  BB pick_area(EveryApi &ev, float mouse_x, float mouse_y, float radius, int scr_size_x, int scr_size_y);
  O pick_volume(M in_P, BB pick);
  bool picked( O o, float x, float y, float z);
private:
  Env &e;
};
#endif

#ifdef F_TREE_API
class TreeApi
{
public:
  TreeApi(Env &e) : e(e) { }
  TL level(std::vector<MN> vec);
  T tree(std::vector<TL> vec);
  ML tree_ml(EveryApi &ev, T tree, std::vector<ML> vec);
  P tree_p(EveryApi &ev, T tree, std::vector<P> vec, float time);
  MS tree_ms(EveryApi &ev, T tree, float time);
private:
  Env &e;
};
#endif

#ifdef F_MATERIALS_API
class MaterialsApi
{
public:
  MaterialsApi(Env &e) : e(e) { }
  IMPORT MT transparent_material(EveryApi &ev, BM bm, MT next);
  IMPORT MT m_keys(EveryApi &ev, std::vector<MT> vec, std::string keys);
  IMPORT MT gltf_anim_material(EveryApi &ev, std::string base_url, std::string url, int skin_num, int animation, int num_timeindexes, MT next, int key);
  IMPORT MT gltf_anim_material2(EveryApi &e, std::string base_url, std::string url, int skin_num, int num_timeindexes, MT next, std::string keys);
  IMPORT MT toon_border(EveryApi &ev, MT next, float border_width, unsigned int color);
  IMPORT ARR material_pack_1(EveryApi &ev);
  IMPORT MT m_def(EveryApi &ev);
  IMPORT MT skeletal(EveryApi &ev);
  IMPORT MT texture(EveryApi &ev, BM bm, float mix);
  IMPORT MT textureid(EveryApi &ev, TXID txid, float mix);
  IMPORT MT texture_many(EveryApi&ev, std::vector<BM> vec, float mix);
  IMPORT MT colour_material(EveryApi &ev, float mix);
  IMPORT MT many_texture_id_material(EveryApi &ev, std::string mtl_url, std::string url_prefix, float mix, int start_range, int end_range);
  IMPORT MT texture_cubemap(EveryApi&ev, std::vector<BM> vec, float mix, float mix2);
  IMPORT MT texture_many2(EveryApi &ev, float mix);
  IMPORT MT texture_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy, float mix);
  IMPORT MT gltf_material( EveryApi &ev, std::string base_url, std::string url, int material_id, float mix );
  IMPORT MT gltf_material_manual( EveryApi &ev, std::string base_url, std::string url, int material_id, float mix, BM, BM, BM,BM,BM,bool,bool,bool,bool,bool );
  IMPORT MT gltf_material_env( EveryApi &ev, std::string base_url, std::string url, int material_id, float mix, BM diffuse_env, BM specular_env, BM bfrd);
  IMPORT MT gltf_material3( EveryApi &ev, float roughness, float metallic, float base_r, float base_g, float base_b, float base_a, float mix);
  IMPORT MT glow_edge(EveryApi &ev, MT next, float light_level, float gray_level, float edge_pos);
  IMPORT MT phong(EveryApi &ev, MT nxt, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT MT phong2(EveryApi &ev, MT nxt, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT MT vertex_phong(EveryApi &ev, MT nxt, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, float mix);
  IMPORT ARR m_apply_phong(EveryApi &ev, std::vector<MT> vec, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT MT edge(EveryApi &ev, MT nxt, float edge_width, unsigned int edge_color);
  IMPORT MT gi(EveryApi &ev, MT nxt, PTS points, float obj_size);
  IMPORT MT bump_phong(EveryApi &ev, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, FB bm, float bump_width);
  IMPORT MT fog(EveryApi &ev, MT nxt, float fog_dist, unsigned int dark_color, unsigned int light_color);
  IMPORT MT shadow(EveryApi &ev, P p, std::vector<BM> vec, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2);
  IMPORT MT shadow2(EveryApi &ev, P p, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2, int numtextures);
  IMPORT MT dyn_lights(EveryApi &ev, MT nxt, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point);
  IMPORT MT snow(EveryApi &ev, MT nxt, unsigned int color1=0xffaaaaaa, unsigned int color2=0xffeeeeee, unsigned int color3=0xffffffff, float mix_val=0.5f);
  IMPORT MT shading1(EveryApi &ev, MT nxt, float mix_val, float mix_val2);
  IMPORT MT shading2(EveryApi &ev, MT nxt, unsigned int color1, unsigned int colo2, unsigned int color3);
  IMPORT MT flat(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3);
  IMPORT MT fur(EveryApi &ev, MT nxt, PT center, float dist, float max_angle, int count, float size, int cone_numfaces);
  IMPORT MT noise(EveryApi &ev, int sx, int sy, int r, int g, int b, int a, int r2, int g2, int b2, int a2, float mix);
  
  IMPORT MT choose_color(EveryApi &ev, MT nxt, unsigned int color, float mix_val);
  IMPORT MT brashmetal(EveryApi &ev, MT nxt, int count, bool web);
  IMPORT MT marble(EveryApi &ev, MT nxt, int count, float cubesize);
  IMPORT MT web(EveryApi &ev, MT nxt, float val=1.01, float linewidth=2.0, unsigned int color=0xff000000); // TODO: add line width property
  IMPORT MT bevel(EveryApi &ev, MT nxt, float dir, float linewidth);
  IMPORT MT bloom(EveryApi &ev, MT nxt, BM bm, float r_cut, float g_cut, float b_cut, float pixel_x, float pixel_y);
  IMPORT MT dist_field_mesh(EveryApi &ev, SFO sfo, MT next);

  IMPORT MT mesh_color_from_sfo(EveryApi &ev, SFO sfo, MT next);

  IMPORT MT sfo_sandbox(EveryApi &ev, SFO sfo, MT next);
  IMPORT MT combine_materials(EveryApi &ev, MT mat1, MT mat2);

  IMPORT MT fade(EveryApi &ev, MT next, float start_time, float end_time, float start_time2, float end_time2);

  IMPORT ML bind(P p, MT mat);
  IMPORT ML bind_inst(P p, PTS pts, MT mat);
  IMPORT ML bind_inst_matrix(P p, MS ms, MT mat);
  IMPORT ML bind_inst2(P p, PTA pta, MT mat);
  IMPORT ML bind_inst_fade(P p, PTS pts, MT mat, bool flip, float start_time, float end_time);
  IMPORT ML render_instanced_ml(EveryApi &ev, P p, PTS pts);
  IMPORT ML render_instanced_ml_matrix(EveryApi &ev, P p, MS pts);
  IMPORT ML render_instanced_ml_fade(EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time);
  IMPORT ML render_instanced_ml_texture(EveryApi &ev, P p, PTS pts, std::vector<BM> vec, std::vector<int> types=std::vector<int>());
  IMPORT ML render_instanced_ml_texture_id(EveryApi &ev, P p, PTS pts, std::vector<TXID> *vec);
  IMPORT ML render_instanced_ml_cubemap(EveryApi &ev, P p, PTS pts, std::vector<BM> vec);
  IMPORT ML render_instanced_ml_texture2(EveryApi &ev, P p, PTS pts);
  //--
  IMPORT ML render_instanced_ml_texture_matrix(EveryApi &ev, P p, MS pts, std::vector<BM> vec, std::vector<int> types=std::vector<int>());
  IMPORT ML render_instanced_ml_texture_id_matrix(EveryApi &ev, P p, MS pts, std::vector<TXID> *vec);
  IMPORT ML render_instanced_ml_cubemap_matrix(EveryApi &ev, P p, MS pts, std::vector<BM> vec);
  IMPORT ML render_instanced_ml_texture2_matrix(EveryApi &ev, P p, MS pts);
  //--

  IMPORT ML render_instanced_ml_fade_texture(EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time, std::vector<BM> vec);
  IMPORT ML render_instanced2_ml(EveryApi &ev, VA va, PTA pta);
  IMPORT ML render_instanced2_ml_matrix(EveryApi &ev, VA va, MSA pta);
  IMPORT ML render_instanced2_ml_fade(EveryApi &ev, VA va, PTA pta, bool flip, float start_time, float end_time);
  //ML snow(EveryApi &ev, P p);
  //ML web(EveryApi &ev, P p);
private:
  Env &e;
};
#endif

#ifdef F_INPUT_API
class InputApi
{
public:
  InputApi(Env &e) : e(e) { }
  
private:
  Env &e;
};
#endif

#ifdef F_PHYSICS_API
class PhysicsApi
{
public:
  PhysicsApi(Env &e) : e(e) { }
  struct PHI { PH phy; int id; };
  PH phy_empty();
  PHI anchor_point(PH phy, PT pos);
  PH anchor_point2(PH phy, PT pos) { PHI pp = anchor_point(phy,pos); return pp.phy; }
  PH ext_force(PH phy, int point, V dir);
  PH ext_force_all(PH phy, V dir);
  PH anchor_link(PH phy, int p1, int p2, float dist);
  PH force_obj(PH phy, O obj,
	       V dir);
  PH phy_from_p(P p);
  // PH gameover_obj(PH phy, O obj, int point)
  // PH keypress_force(PH phy, int point, V dir, int key);
  //PH array(PH *arr, int size);

  int num_anchors(PH phy);
  int num_forces(PH phy, int anchor);
  int num_links(PH phy);
  int num_force_volumes(PH phy);

  PTS init_points(PH pos);
  void step_points(PH phy, PTA prev_frame, float timestep);
  PTS physics_action(EveryApi &ev, PH phy);
private:
  Env &e;
};
#endif

#ifdef F_COLLISION_PLANE
class CollisionPlane
{
public:
  CollisionPlane(Env &e) : e(e) { }
  CP circle(int id, float radius);
  CP rectangle(int id, float x, float y, float sx, float sy);
  CP or_elem(int id, std::vector<CP> vec);
  void set_pos(CP plane, int id, float x, float y);
  bool check_collision(CP plane, float x, float y);
private:
  Env &e;
};
#endif

#ifdef F_SKELETAL
class Skeletal
{
public:
  Skeletal(Env &e) : e(e) { }
  SA root(PT absolute_pos);
  SA node(SA parent, MN matrix, PT point_offset);
  ML skeletal_bind(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> sa_vec);
  ML skeletal_bind_material(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> sa_vec, MT material);
private:
  Env &e;
};
#endif

class VertexAnimApi
{
public:
  VertexAnimApi(Env &e) : e(e) { }
  IMPORT KF keyframe_mesh(P part);
  IMPORT KF keyframe_lines(LI part);
  IMPORT KF keyframe_bind(EveryApi &ev, KF keyframe, PTT pointtransform, float delta_time);
  IMPORT KF keyframe_bind2(EveryApi &ev, KF keyframe, PTT pointtransform, float delta_time, bool dif);
  IMPORT KF curve_trans(EveryApi &ev, KF kf, C curve, CPP pos, int numsamples, float duration);
  IMPORT KF repeat_keyframes(KF rep, int count);

  IMPORT KF sample_rot(EveryApi &ev, KF kf, float nx, float ny, float nz, float angle, int numsamples, float duration);
  

  //KF keyframe_bind2(KF keyframe, O subpart, PTT pointtransform, float delta_time);
  IMPORT PTT empty_trans();
  IMPORT PTT curve_accessor(C curve, CPP pos, float start_pos, float time_mult);
  IMPORT PTT rot_accessor(float start_time, float time_mult, float nx, float ny, float nz, float dist_angle);
  IMPORT PTT translate_trans(PTT prev, float speed_x, float speed_y, float speed_z);
  IMPORT PTT translate_trans2(PTT prev, float duration,float dist_x, float dist_y, float dist_z);
  IMPORT PTT rotate_trans(PTT prev, float nx, float ny, float nz, float speed_angle);
  IMPORT PTT rotate_trans2(PTT prev, float duration,float nx, float ny, float nz, float dist_angle);
  IMPORT PTT scale_trans(PTT prev, float scale_speed_x, float scale_speed_y, float scale_speed_z);
  IMPORT PTT scale_trans2(PTT prev, float duration,float scale_dist_x, float scale_dist_y, float scale_dist_z);
  IMPORT ML vertex_anim_render(EveryApi &ev, KF kf);
public:
  IMPORT P change_pos(P p, P orig, PTT transform, float delta_time, bool dif);
  IMPORT LI change_pos_li(LI p, LI orig, PTT transform, float delta_time, bool dif);
private:
  Env &e;
};

class EventNode
{
public:
  EventNode(Env &e) : e(e) { }
  EV empty_event();
  EV key_event(EV next, int key_id, int key_type);
  EV timer_event(EV next, float time);
  EV delta_timer(EV next, EV event, float delta_time);
  //EV event_enable(EV next, EV event, EV start_event, EV end_event);
  //EV event_disable(EV next, EV event, EV start_event, EV end_event);

  AC empty_action();
  AC choose_ml_action(AC action, ML alternative);
  AC move_ml_action(AC action, MN movement, float start_time, float end_time);
  AC color_ml_action(AC action, int color_num, CC cc);
  AC send_key_action(AC action, int key_id, int key_type);
  AC change_time_action(AC action, float new_time);
  AC array_action(std::vector<AC> arr);
  AC sequence_action(std::vector<AC> arr, float time_step);
  AC skip_time_action(AC action, float delta_time);

  ML event_bind(ML next, EV event, AC action);
private:
  Env &e;
};

#ifdef F_MOVEMENT_NODE
class MovementNode
{
public:
  MovementNode(Env &e) : e(e) {}
  IMPORT MN whack_a_mole_start(EveryApi &ev, MN prev, float &time);
  IMPORT MN whack_a_mole_rotate_around(EveryApi &ev, MN prev, int num, float &time);
  IMPORT MN whack_a_mole_exit(EveryApi &ev, MN prev, int exit_num, float &time);
  IMPORT MN whack_a_mole_all(EveryApi &ev, int cycle_count, int exit_num);
  IMPORT MN mn_interpolate(MN n1, MN n2, FF fetcher);
  IMPORT MN mn_interpolate2(MN n1, MN n2, float start_time, float end_time);
  IMPORT MN interpolate(MN n1, MN n2, float start_time, float end_time, float start_value, float end_value);
  IMPORT MN mn_empty();
  IMPORT MN level(MN next);
  IMPORT MN trans2(MN next, float dx, float dy, float dz);
  IMPORT MN scale2(MN next, float sx, float sy, float sz);
  IMPORT MN rotatex(MN next, float angle);
  IMPORT MN rotatey(MN next, float angle);
  IMPORT MN rotatez(MN next, float angle);
  IMPORT MN matrix(MN next, M mat);
  IMPORT MN pose(MN next, bool pose_in_screen);
  IMPORT MN debug_translate(MN next);
  IMPORT MN translate(MN next, float start_time, float end_time,float dx, float dy, float dz);
  IMPORT MN translate_wave(MN next, float start_time, float end_time, WV wave, float dx, float dy, float dz);
  IMPORT MN scale_progress(MN next, bool is_x, bool is_y, bool is_z);
  IMPORT MN mn_fetcher(PF pf);
  IMPORT MN scale(MN next, float start_time, float end_time,float sx, float sy, float sz);
  IMPORT MN scale_wave(MN next, float start_time, float end_time,WV wave,float sx, float sy, float sz);
  IMPORT MN rotate(MN next, float start_time, float end_time,float p_x, float p_y, float p_z,float v_x, float v_y, float v_z, float angle);
  IMPORT MN rotate_wave(MN next, float start_time, float end_time,WV wave,float p_x, float p_y, float p_z,float v_x, float v_y, float v_z, float angle);


  IMPORT MN compress(MN next, float start_time, float end_time);
  IMPORT MN change_time(MN next, float d_time);
  IMPORT MN event_activate(MN next, MN event, float event_time, float duration);
  IMPORT MN anim_enable(MN next, float start_time, float end_time);
  IMPORT MN anim_disable(MN next, float start_time, float end_time);
  IMPORT MN anim_choose(std::vector<MN> vec, float start_time, float duration);
  IMPORT MN time_repeat(MN next, float start_time, float repeat_duration);
  IMPORT CC color_start(unsigned int color);
  IMPORT CC color_interpolate(CC next, unsigned int color, unsigned int color2, float start_time, float end_time);
  IMPORT void set_matrix(MN n, M m);
  IMPORT M get_matrix(MN n, float time, float delta_time);
  IMPORT ML color_ml(EveryApi &ev, int color_num, ML ml, CC cc);
  IMPORT ML move_ml(EveryApi &ev, ML ml, MN mn, int clone_count=1, float time_delta=10.0);
  IMPORT P move_ml_p(EveryApi &ev, P p, MN move, float time);
  IMPORT ML repeat_ml(EveryApi &ev, ML ml, float duration);
  IMPORT ML key_activate_ml(EveryApi &ev, ML ml, MN mn, int key, float duration);
  IMPORT ML comb_key_activate_ml(EveryApi &ev, ML ml, MN mn, int key, int key_2, float duration);
 IMPORT  ML temp_key_activate_ml(EveryApi &ev, ML ml, MN mn, int key, float duration);
  IMPORT ML move_x_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x);
  IMPORT ML move_y_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y);
  IMPORT ML move_z_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z);
  IMPORT ML rot_x_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x);
  IMPORT ML rot_y_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y);
  IMPORT ML rot_z_ml(EveryApi &ev, ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z);
  IMPORT ML jump_ml(EveryApi &ev, ML ml, int key_jump, float height, float jump_duration);
  IMPORT ML move_ml_array(EveryApi &ev, std::vector<ML> ml, std::vector<MN> mn);
  IMPORT ML enable_ml(EveryApi &ev, ML ml, float start_time, float end_time);
  IMPORT ML key_event(EveryApi &ev, ML ml, MN mn, int type, int ch, int button, float duration);
  IMPORT ML wasd(EveryApi &ev, ML ml, MN w, MN a, MN s, MN d, float duration);
  IMPORT ML quake_ml(EveryApi &ev, ML ml, float speed, float rot_speed);
  IMPORT ML quake_ml2(EveryApi &ev, ML ml, float speed, float rot_speed);
  IMPORT ML quake_ml3(EveryApi &ev, ML ml, ML ml2, float speed, float rot_speed, float p_x, float p_y, float p_z);
  IMPORT ML key_printer_ml(ML ml);
  IMPORT ML local_move(EveryApi &ev, ML inner_ml, PTS center_points);
  IMPORT ML local_move_matrix(EveryApi &ev, ML inner_ml, MS center_points);
  IMPORT CMD default_cmds(float dx, float dy, float dz);
  IMPORT CMD cmd_repeat(CMD cmds, std::string repeat, float dx, float dy, float dz);
  IMPORT CMD cmd_rotate(CMD cmds, float v_x, float v_y, float v_z, float angle, float delta_angle);
  IMPORT PTS cmd_to_pts(CMD cmds, std::string commands);
  IMPORT LI cmd_to_li(CMD cmds, std::string commands);
  IMPORT PLF cmd_to_plf(CMD cmds, std::string commands);

  IMPORT ML all_cursor_keys(EveryApi &ev, ML ml, float speed, float duration);
  IMPORT ML cursor_keys(EveryApi &ev, ML ml, int key_up, int key_left, int key_down, int key_right, float speed, float duration);
  
  IMPORT ML player(ML prev);
  IMPORT ML enemy(ML prev);
  IMPORT ML player_pos(ML prev, PT pos);
  IMPORT ML enemy_pos(ML prev, PTS pos);
  IMPORT DC identity();
  IMPORT DC wave(float r,float speed1,float speed2,float dist1, float dist2,int sx,int sy);
  IMPORT DC sphwave(float r1, float fr_1, float t_1, float r2, float fr_2, float t_2);
  IMPORT DC explosion(float center_x, float center_y, float center_z, float start_val, float end_val, float start_time, float end_time);
  IMPORT DC compose(DC d1, DC d2);
  IMPORT DC split(DC d1, DC d2, float val);
private:
  Env &e;
};
#endif

#ifdef F_EXPR_API
class ExprApi
{
public:
  ExprApi(Env &e) : e(e) {}
  EX variable(std::string name);
  EX plus(EX e1, EX e2);
  EX mul(EX e1, EX e2);
  EX minus(EX e1, EX e2);
  EX div(EX e1, EX e2);
  EX sin(EX e1);
  EX cos(EX e1);
  EX float_constant(float val);
  EX int_constant(int val);

  EX expr_float(std::string expr, bool &success);
  EX expr_int(std::string expr, bool &success);

  struct FloatExprEnv { std::string name; float value; };
  struct IntExprEnv { std::string name; int value; };
  float expr_eval_float(EX expr, std::vector<FloatExprEnv> env);
  int expr_eval_int(EX expr, std::vector<IntExprEnv> env);
private:
  Env &e;
};
#endif

#ifdef F_GUI_API
class GuiApi
{
public:
  GuiApi(Env &e, EveryApi &ev, SH sh) : e(e), ev(ev), sh(sh) { }
  IMPORT void delete_widget(W w);
  IMPORT W window_decoration(int sx, int sy, std::string label, FtA atlas, BM atlas_bm);
  IMPORT W empty();
  IMPORT W text(std::string label, FtA atlas, BM atlas_bm, int x_gap=3);
  IMPORT W icon(BM bitmap);
  IMPORT W icon_shared(BM bitmap, int key);
  IMPORT W poly(P p, SH sh, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W va(VA p, SH sh, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W ml(ML p, SH sh, SH sh2, SH sh_2d, SH sh_arr, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W shader_plane(SFO p, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W lines(LI p, SH sh, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W pts(PTS p, SH sh, int sx, int sy, int screen_size_x, int screen_size_y);
  IMPORT W line(W target1, int delta_x, int delta_y,W target2, int delta2_x, int delta2_y, SH sh, SH old_sh);
  IMPORT W gradient(int sx, int sy, PT pos_1, PT pos_2, unsigned int colot_1, unsigned int color_2);
  IMPORT W button(int sx, int sy, unsigned int color_1, unsigned int color_2);
  IMPORT W mouse_move(W widget, int area_x, int area_y, int area_width, int area_height);
  IMPORT W window_move(W widget, int area_x, int area_y, int area_width, int area_height);
  IMPORT W click_area(W widget, int area_x, int area_y, int area_width, int area_height, int button_id);
  IMPORT W key_area(W widget, int area_x, int area_y, int area_width, int area_height, int key);
  IMPORT W click_visibility(W area_widget, W hidden_widget);
  IMPORT W click_hide(W widget);
  IMPORT W top_right_corner_match(W wid, W floating); // returns floating
  IMPORT W or_elem(W w1, W w2);
  IMPORT W rectangle(int start_x, int end_x, int start_y, int end_y, unsigned int color);
  IMPORT W highlight(int sx, int sy);
  IMPORT W highlight(W wid);
  IMPORT W margin(W item, int left, int top, int right, int bottom);
  IMPORT W left_align(W item, int sx);
  IMPORT W right_align(W item, int sx);
  IMPORT W center_align(W item, int sx);
  IMPORT W center_y(W item, int sy);
  IMPORT W layer(W w1, W w2);
  IMPORT W array_y(W *arr, int size, int y_gap);
  IMPORT W array_x(W *arr, int size, int x_gap);
  IMPORT W timed_visibility(W orig, W timed_widget, W insert, float start_duration, float duration, float dx);
  IMPORT W tooltip(W orig, W insert, std::string label, FtA atlas, BM atlas_bm, int x_gap=2, float dx=40.0);
  IMPORT W popup_box(std::string label, std::vector<std::string> options, FtA atlas, BM atlas_bm); 
  IMPORT W popup_box_menu(std::vector<std::string> options, FtA atlas, BM atlas_bm);

  IMPORT W main_menu(std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  IMPORT W menu(W main_menu, int menu_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  IMPORT W submenu(W menu, int menu_pane_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm);
  IMPORT W popup_menu(int x, int y, std::vector<std::string> labels, FtA atlas, BM atlas_bm, std::vector<W> &areas);
  IMPORT W scrollbar_y(int sx, int sy, int area_y);
  IMPORT W scrollbar_x(int sx, int sy, int area_x);
  IMPORT W scroll_area(W orig, int sx, int sy, int screen_y);
  IMPORT W waveform(std::function<float (float)> f, float start_range, float end_range, float min_value, float max_value, int sx, int sy, unsigned int true_color, unsigned int false_color);
  IMPORT W canvas(int sx, int sy);
  IMPORT W find_canvas_item(W canvas, std::string id);
  IMPORT int canvas_item_index(W canvas, W item);
  IMPORT int canvas_item(W canvas, W item, int x, int y);
  IMPORT void del_canvas_item(W canvas, int id);
  IMPORT W canvas_item_gameapi_node(int sx, int sy, std::string label, std::vector<std::string> param_types, std::vector<std::string> param_tooltips, std::string return_type, FtA atlas, BM atlas_bm, std::vector<W *> connect_click, std::string uid, std::vector<W> &params, std::string symbol, std::string comment);
  IMPORT W list_item_title(int sx, std::string label, FtA atlas, BM atlas_bm);
  IMPORT W list_item_opened(int sx, std::string label, FtA atlas, BM atlas_bm, std::vector<std::string> subitems, std::vector<std::string> subitems_tooltip, FtA atlas2, BM atlas_bm2, W insert);
  IMPORT W list_item(BM icon, std::string label, int sx, int sy);
  IMPORT W list(W *array, int size, int sx, int sy);
  IMPORT W dialog_item(std::string text, BM icon, int sx, int sy);
  IMPORT W dialog_border(W item);
  IMPORT W copy_paste_dialog(SH sh, W &close_button, FI font, FtA atlas, BM atlas_bm, std::string &edit);
  IMPORT W bitmap_dialog(BM bm, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button);
  IMPORT W polygon_dialog(P p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button, W &mem);
  IMPORT W va_dialog(VA p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button);
  IMPORT W ml_dialog(ML p, SH sh, SH sh2, SH sh_2d, SH sh_arr, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button);
  IMPORT W shader_dialog(SFO p, W &close_button, FtA atlas, BM atlas_bm, int screen_size_x, int screen_size_y, W &codegen_button, W &collect_button);
  IMPORT W lines_dialog(LI p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button);
  IMPORT W pts_dialog(PTS p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button);

  IMPORT W button_with_text(std::string label);
  IMPORT W button_with_icon(BM bitmap);
  IMPORT W opengl_wrapper(W widget);
  IMPORT W string_editor(std::string allowed_chars, std::string &target, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W multiline_string_editor(std::string allowed_chars, std::string &target, FI font, int x_gap, int line_height);
  IMPORT W url_editor(std::string &target, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W float_editor(float &target, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W int_editor(int &target, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W long_editor(long &target, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W point_editor(float &x, float &y, float &z, FtA atlas, BM atlas_bm, int x_gap);
  IMPORT W color_editor(std::string &col, FtA atlas, BM atlas_bm, int x_gap);
  struct EditTypes
  {
    int i_value;
    long l_value;
    float f_value;
    float f_x, f_y, f_z;
    std::string color;
    std::string s;
  };
  IMPORT W generic_editor(EditTypes &target, FtA atlas, BM atlas_bm, std::string type, int x_gap, FtA atlas_tiny, BM atlas_tiny_bm, int sy);
  IMPORT void generic_to_string(const EditTypes &source, std::string type, std::string &target);
  IMPORT void string_to_generic(EditTypes &target, std::string type, const std::string &source);
  IMPORT W edit_dialog(const std::vector<std::string> &labels, const std::vector<EditTypes*> &vec, Ft font, const std::vector<std::string> &types, W &cancel_button, W &ok_button);
  IMPORT W edit_dialog(const std::vector<std::string> &labels, const std::vector<EditTypes*> &vec, FtA atlas, BM atlas_bm, const std::vector<std::string> &types, W &cancel_button, W &ok_button, FtA atlas_tiny, BM atlas_tiny_bm);
  IMPORT W bitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W boolbitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W floatbitmapapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W polygonapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W polygondistapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W shadermoduleapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W framebuffermoduleapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W linesapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W pointsapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W pointapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W vectorapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W volumeapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W floatvolumeapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W colorvolumeapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W fontapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W textureapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W booleanopsapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W moveapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W waveformapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);
  IMPORT W blockerapi_functions_list_item(FtA font1, BM font1_bm, FtA font2, BM font2_bm, W insert);


  IMPORT std::string bitmapapi_functions_item_label(int i);
  IMPORT std::string waveformapi_functions_item_label(int i);
  IMPORT std::string blockerapi_functions_item_label(int i);
  IMPORT std::string boolbitmapapi_functions_item_label(int i);
  IMPORT std::string floatbitmapapi_functions_item_label(int i);
  IMPORT std::string polygonapi_functions_item_label(int i);
  IMPORT std::string polygondistapi_functions_item_label(int i);
  IMPORT std::string shadermoduleapi_functions_item_label(int i);
  IMPORT std::string framebuffermoduleapi_functions_item_label(int i);
  IMPORT std::string linesapi_functions_item_label(int i);
  IMPORT std::string pointsapi_functions_item_label(int i);
  IMPORT std::string pointapi_functions_item_label(int i);
  IMPORT std::string vectorapi_functions_item_label(int i);
  IMPORT std::string volumeapi_functions_item_label(int i);
  IMPORT std::string floatvolumeapi_functions_item_label(int i);
  IMPORT std::string colorvolumeapi_functions_item_label(int i);
  IMPORT std::string fontapi_functions_item_label(int i);
  IMPORT std::string textureapi_functions_item_label(int i);
  IMPORT std::string booleanopsapi_functions_item_label(int i);
  IMPORT std::string moveapi_functions_item_label(int i);
  IMPORT W insert_widget(W item, std::function<void(int,int)> f);
  IMPORT void insert_widget_activate(W w, bool b);

  IMPORT void set_pos(W w, float px, float py);
  IMPORT void set_size(W ow, float sx, float sy);
  IMPORT void update(W w, PT mouse_cursor_pos, int button, int ch, int type, int mouse_wheel_y);
  IMPORT void render(W w);
  IMPORT int chosen_item(W w);
  IMPORT void select_item(W w, int item);
  IMPORT float dynamic_param(W w, int id);
  IMPORT void set_dynamic_param(W w, int id, float val);
  IMPORT std::string get_id(W w);
  IMPORT void set_id(W w, std::string id);
  IMPORT int get_index(W w);
  IMPORT void set_index(W w, int j);
  IMPORT int get_size2(W w);
  IMPORT void set_size2(W w, int sz);
  IMPORT int num_childs(W w);
  IMPORT W get_child(W w, int i);
  IMPORT int pos_x(W w);
  IMPORT int pos_y(W w);
  IMPORT int size_x(W w);
  IMPORT int size_y(W w);

  IMPORT std::vector<std::pair<std::string,std::string> > get_functions_mapping();
private:
  Env &e;
  EveryApi &ev;
  SH sh;
};
#endif

  class EditNode;
  struct CollectResult {
    std::vector<EditNode*> res;
    std::string p;
    std::string pn;
  };


  int collect_counter(int mode); // 0=reset, 1=count_next

#ifdef F_WMOD_API
class WModApi
{
public:
  WModApi(Env &e) : e(e) { }
  IMPORT std::string dump_functions();
  IMPORT WM load(std::string filename);
  IMPORT void save(WM mod, std::string ilename);
  IMPORT void insert_to_canvas(GuiApi &gui, W canvas, WM mod, int id, FtA font, BM font_bm, std::vector<W> &connect_clicks, std::vector<W> &params, std::vector<W> &diaplay_clicks, std::vector<W> &edit_clicks, std::vector<W> &delete_key, std::vector<W> &codegen_button, std::vector<W> &popup_open);
  IMPORT void update_lines_from_canvas(W canvas, WM mod, int id);
  IMPORT void insert_inserted_to_canvas(GuiApi &gui, W canvas, W item, std::string uid, W &display_clicks, W &edit_clicks, W &delete_key, W &codegen_button, W &popup_open);
  IMPORT W inserted_widget(GuiApi &gui, WM mod2, int id, FtA atlas, BM atlas_bm, std::string func_name, std::vector<W *> connect_click, std::string uid, std::vector<W> &params);
  IMPORT std::vector<int> indexes_from_funcname(std::string funcname);
  IMPORT std::vector<std::string> types_from_function(WM mod, int id, std::string funcname);
  IMPORT std::vector<std::string> labels_from_function(WM mod, int id, std::string funcname);
  IMPORT std::vector<std::string*> refs_from_function(WM mod, int id, std::string funcname);
  IMPORT std::vector<std::pair<std::string,std::string> > defaults_from_function(std::string module_name);
  struct InsertParam {
    std::string first;
    std::string second;
    bool is_array = false;
    int line_index_in_gameapi_function_lines_array = -1;
  };
  IMPORT void insert_to_mod(WM mod, int id, std::string modname, std::string uid, bool array_return, int x, int y, std::vector<InsertParam> params);
  
  IMPORT std::string get_funcname(WM mod2, int id, std::string uid);
  IMPORT void change_param_value(WM mod2, int id, std::string uid, int param_index, std::string newvalue, int j);
  IMPORT void change_param_is_array(WM mod2, int id, std::string uid, int param_index, bool is_array, int ref_line_index);
  IMPORT void change_param_multiple_return_values(WM mod2, int id, std::string uid, int j, int sz);
  IMPORT int find_line_index(WM mod2, int id, std::string uid);
  IMPORT std::string param_value(WM mod2, int id, std::string uid, int param_index);
  IMPORT std::vector<std::string> parse_param_array(std::string s);
  IMPORT std::pair<std::string,int> parse_multiple_return_uid(std::string s, bool &success);
  IMPORT std::string generate_param_array(std::vector<std::string> v);
  IMPORT bool typecheck(WM mod2, int id, std::string uid1, std::string uid2, int param_index, int ret_index, bool &is_array, bool &is_array_return);
  IMPORT void insert_links(EveryApi &ev, GuiApi &gui, WM mod2, int id, std::vector<W> &links, W canvas, const std::vector<W> &connect_targets, SH sh2, SH sh);

  IMPORT int execute(EveryApi &ev, WM mod2, int id, std::string line_uid, ExecuteEnv &exeenv, int level, int j);
  IMPORT std::pair<int,std::vector<std::string> > collect_urls(EveryApi &ev, WM mod2, int id, std::string line_uid, ExecuteEnv &exeenv, int level, ASyncData *arr, int arr_size, int j);
  

  IMPORT CollectResult collect_nodes(EveryApi &ev, WM mod2, int id, std::string line_uid, int level);
  IMPORT void codegen_reset_counter();
  IMPORT std::pair<std::string, std::string> codegen(EveryApi &ev, WM mod2, int id, std::string line_uid, int level, int j);
  IMPORT std::string return_type(WM mod2, int id, std::string line_uid);
  IMPORT void delete_by_uid(WM mod2, int id, std::string line_uid);
private:
  Env &e;
};
#endif

#ifdef F_FLOAT_VOLUME_API
class FloatVolumeApi
{
public:
	IMPORT FloatVolumeApi(Env &e) : e(e) { }
	IMPORT FO function(std::function<float(float x, float y, float z)> f);
	IMPORT FO from_volume(O o, float false_val, float true_val);
	IMPORT FO from_float_bitmap(FB bm,float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
	IMPORT FO distance();
	IMPORT FO torusdistance(PT center, V u_x, V u_y, float radius);
	IMPORT FO move(FO f1, float dx, float dy, float dz);
	IMPORT FO minimum(FO f1, FO f2);
	IMPORT FO maximum(FO f1, FO f2);
  IMPORT FO waveform_sphere(WV wav, float r);
  IMPORT WV wave_move_y(WV wav, float delta);
        IMPORT O subvolume(FO f, float start_range, float end_range);
        IMPORT FO smooth(std::vector<FO> vec, float val);
        IMPORT FO interpolate(FO f1, FO f2, float val);
        IMPORT FO min_distance(C curve);
  
	IMPORT FO shadow(FD fd, V light_dir, float mint, float maxt, float k);

  IMPORT FB integrate_render(FO obj, int sx, int sy, int numsamples);
  
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
#endif

#ifdef F_COLOR_VOLUME_API
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
  IMPORT BM array_bm(COV colours, int sx, int sy, float ssx, float ssy, float z);
  // TODO
private:
  ColorVolumeApi(const ColorVolumeApi&);
  void operator=(const ColorVolumeApi&);
  Env &e;
};
#endif

#ifdef F_VECTOR_VOLUME_API
class VectorVolumeApi
{
public:
	IMPORT VectorVolumeApi(Env &e) : e(e) { }
	IMPORT VO function(std::function<V(float x, float y, float z)> f);
	IMPORT VO normal(FD fd);
  IMPORT VO normal2(FO fo, float stepsize);
  IMPORT P setup_normal(P orig, VO v);
private:
  VectorVolumeApi(const VectorVolumeApi&);
  void operator=(const VectorVolumeApi&);

  Env &e;
};
#endif

#ifdef F_DISTANCE_FLOAT_VOLUME_API
class DistanceFloatVolumeApi
{
public:
	IMPORT DistanceFloatVolumeApi(Env &e) : e(e) { }
	IMPORT FD function(std::function<float(float x, float y, float z)> f);
	IMPORT FD fd_sphere(PT center, float radius);
	IMPORT FD fd_cube(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
  IMPORT FD fd_round_cube(float start_x, float end_x,float start_y, float end_y,float start_z, float end_z,float r);
  IMPORT FD fd_torus(float radius_1, float radius_2);
  IMPORT FD fd_cone(float c_x, float c_y);
  IMPORT FD fd_plane(float n_x, float n_y, float n_z, float n_w);
  IMPORT FD fd_hex_prism(float h_x, float h_y);
  IMPORT FD fd_tri_prism(float h_x, float h_y);
  IMPORT FD fd_triangle(PT a, PT b, PT c);
  IMPORT FD fd_quad(PT a, PT b, PT c, PT d);
	IMPORT FD fd_line(PT start, PT end, float dist);
  IMPORT FD fd_color(FD fd, float r, float g, float b, float a);
  IMPORT FD fd_rot_x(FD fd, float angle);
  IMPORT FD fd_rot_y(FD fd, float angle);
  IMPORT FD fd_rot_z(FD fd, float angle);
  IMPORT FD fd_trans(FD fd, float dx, float dy, float dz);

	IMPORT FD fd_min(FD a1, FD a2);
        IMPORT FD fd_max(FD a1, FD a2);
  IMPORT FD fd_and_not(FD a1, FD a2);
  IMPORT FD fd_blend(FD a1, FD a2, float k);
  IMPORT FD recalculate_normals(FD fd);
  IMPORT FD ambient_occulsion(FD fd, float d, float i);
  IMPORT P distance_poly(EveryApi &ev, FD fd, 
			 float dx, float dy, float dz, 
			 int sx, int sy,
			 float ssx, float ssy, 
			 int ssxi, int ssyi, 
			 float ssx2, float ssy2);
  IMPORT P distance_poly_cyl(EveryApi &ev, FD fd,
			     float pos_x, float pos_y, float pos_z,
			     float dx, float dy, float dz, 
			     int sx, int sy,
			     float ssx, float ssy, 
			     int ssxi, int ssyi, 
			     float ssx2, float ssy2);
  IMPORT P distance_poly_sph(EveryApi &ev, FD fd, 
			 float dx, float dy, float dz, 
			 int sx, int sy,
			 float ssx, float ssy, 
			 int ssxi, int ssyi, 
			 float ssx2, float ssy2);
  IMPORT PT ray_shape_intersect(FD shape, PT pos, V vec);
	IMPORT BM render(FD obj,PT pos, V u_x, V u_y, V u_z, int sx, int sy);
  IMPORT BM render2(EveryApi &ev, FD obj, int sx, int sy);
	IMPORT std::string shader_func(std::string name, FD obj, COV color);
private:
  DistanceFloatVolumeApi(const DistanceFloatVolumeApi&);
  void operator=(const DistanceFloatVolumeApi&);

  Env &e;
};
#endif

#ifdef F_SEPARATE_API
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
#endif

#ifdef F_SURFACE_API
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
  PTS surface_points(S surf, int sx, int sy);
private:
  void *priv;
  Env &e;
};
#endif

#ifdef F_TRISTRIP_API
class TriStripApi
{
public:
  IMPORT TriStripApi(Env &e) : e(e) { }
  IMPORT ~TriStripApi() { }
  IMPORT TS from_array(PT *arr, int size);
  IMPORT TS function(std::function<PT (int i)> f, int count);
  IMPORT TS color_function(TS ts, std::function<unsigned int (int i)> f);
  IMPORT TS texcoord_function(TS ts, std::function<PT (int i)> texcoord);
  IMPORT TS normal_function(TS ts, std::function<V (int i)> normal);
  IMPORT TS rectangle_xy(float start_x, float end_x, float start_y, float end_y, float z, int sx, int sy);
  IMPORT TS rectangle_xz(float start_x, float end_x, float y, float start_z, float end_z);
  IMPORT TS circle(float center_x, float center_y, float center_z, float radius, int count);
  IMPORT TS transform(TS orig, std::function<PT (PT)> trans);
  
  IMPORT TS from_convex_plane(PL pl, PT pos, V u_x, V u_y);
  IMPORT TS span_3d_object(TS orig, float z_dist);

  IMPORT TS or_elem(TS t1, TS t2);
  IMPORT TS or_array(TS *array, int size);

  IMPORT void save(TS tris, std::string filename);
  IMPORT TS load(std::string filename);
  IMPORT P to_poly(TS strip);
  IMPORT TS from_poly(P poly);
private:
  Env &e;
};
#endif

#ifdef F_CUTTER_API
class CutterApi
{
public:
  IMPORT CutterApi(Env &e) : e(e) { }
  IMPORT CT function(std::function<PT (PT,PT)> f);
  IMPORT CT plane_cut(PT pos, V u_x, V u_y);
  //IMPORT CT sphere_cut(PT center, float radius);
  IMPORT CT distance_cut(FD dist);
  IMPORT O cutter_volume(CT c, PT point_from_inside, float outside_dist);
private:
  Env &e;
};
#endif

  struct BO { int id; };
#ifdef F_BOOLEAN_OPS

class BooleanOps
{
public:
  BooleanOps(Env &e) : e(e) { }
  IMPORT BO create_bo(P mesh, O bools, FD fd);
  IMPORT BO cube_bo(EveryApi &ev, float start_x, float end_x,float start_y, float end_y,float start_z, float end_z,int split_x, int split_y);
  IMPORT BO sphere_bo(EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2);
  //BO cone(int numfaces, PT p1, PT p2, float rad1, float rad2);
  //BO torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2);
  IMPORT BO or_elem_bo(EveryApi &ev, BO obj, BO obj2);
  IMPORT BO and_not_bo(EveryApi &ev, BO obj, BO not_obj);
  IMPORT BO intersect_bo(EveryApi &ev, BO obj, BO obj2);
  IMPORT P to_polygon(BO obj);
  IMPORT O to_volume(BO obj);
  IMPORT FD to_dist(BO obj);
private:
  Env &e;
};
#endif
struct PD { int id; };

#ifdef F_POLYGON_DISTANCE_FIELD
class PolygonDistanceField
{
public:
  PolygonDistanceField(Env &e) : e(e) { }
  IMPORT PD empty(EveryApi &ev);
  IMPORT PD create_pd(P mesh, SFO distance_field);
  IMPORT PD cube_pd(EveryApi &ev, float start_x, float end_x,float start_y, float end_y,float start_z, float end_z);
  IMPORT PD rounded_cube_pd(EveryApi &ev, float start_x, float end_x,float start_y, float end_y,float start_z, float end_z,float radius);
  IMPORT PD sphere_pd(EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2);
  IMPORT PD cone_pd(EveryApi &ev, int numfaces, PT p1, PT p2, float rad1, float rad2);
  IMPORT PD or_array_pd(EveryApi &ev, std::vector<PD> vec);
  IMPORT PD translate_pd(EveryApi &ev, PD orig, float dx, float dy, float dz);
  IMPORT PD rotatex_pd(EveryApi &ev, PD orig, float angle);
  IMPORT PD rotatey_pd(EveryApi &ev, PD orig, float angle);
  IMPORT PD rotatez_pd(EveryApi &ev, PD orig, float angle);
  IMPORT PD scale_pd(EveryApi &ev, PD orig, float sx, float sy, float sz);
  IMPORT PD color_from_normal_pd(EveryApi &ev, PD obj);
  IMPORT PD ambient_occulsion_sfo(EveryApi &ev, PD obj, float d, float i);
  IMPORT PD colormod_from_position(EveryApi &ev, PD obj, float px, float py, float pz, float sx, float sy, float sz);
  IMPORT MT mesh_color_from_sfo(EveryApi &ev, PD orig, MT next);
  
  IMPORT ML render_scene(EveryApi &ev, PD object, PD world); // this has access to sfo, P and matrix stack -- should make dynamic movement possible with proper shading which responds to movement of the objects in the scene. especially AO needs to be recalculated, but will also allow other shading stuff. This hasnt worked before since P object doesnt have access to shader side, SFO can't deal with meshes and matrix stack/ML is completely separate and it's needed for movement. // shader side needs to respond to matrix stack changes. So this is basically a bridge between matrix stack and shaders.
  IMPORT std::vector<ML> render_scene_array(EveryApi &ev, std::vector<PD> vec); // this on the other hand, is completely different -- it can connect multiple objects together in nice way. Unfortunately, builder doesn't support this api, but it's anyway useful later. (we can make builder support this prototype)

  IMPORT P get_polygon(PD p);
  IMPORT SFO get_distance_field(PD p);
private:
  Env &e;
};
#endif
#ifdef F_POLYGON_ARRAY_API
class PolygonArrayApi
{
public:
  PolygonArrayApi(Env &e) : e(e) { }
  Pa split_p(EveryApi &ev, P p, int max_chunk);
  Va create_vertex_array(EveryApi &ev, Pa p, bool keep=false);
  void delete_vertex_array(EveryApi &ev, Va va);
  void render_vertex_array(EveryApi &ev, Va p);
  void prepare_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh);
  void prepare_vertex_array_instanced_matrix(EveryApi &ev, ShaderApi &sha, Va va, MSA pta, SH sh);
  void render_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh);
private:
  Env &e;
};
#endif
#ifdef F_POLYGON_API

struct MaterialDef
{
  std::string material_name;
  float Ns;
  float Ni;
  float d;
  float Tr;
  int illum;
  float Ka_x, Ka_y, Ka_z;
  float Kd_x,Kd_y,Kd_z;
  float Ks_x,Ks_y,Ks_z;
  float Ke_x,Ke_y,Ke_z;
  std::string map_Ka;
  std::string map_Kd;
  std::string map_d;
  std::string map_bump;
  std::string bump;
};


class PolygonApi
{
public:
	IMPORT PolygonApi(Env &e);
	IMPORT ~PolygonApi();
  ML p_mtl2_prepare(P p);
  ARR material_choose(std::vector<MT> mat, std::vector<P> p);
  ARR comb_mat(GameApi::EveryApi &ev, std::vector<MT> vec1, std::vector<MT> vec2);
  ARR p_mtl_materials(EveryApi &ev, P P);
  ARR p_mtl2_materials(EveryApi &ev, P p);
  P get_face_count(P p);
  P transparent_separate(P p, BM bm, bool opaque);
  void sort_vertices(VA va, M m);
  P remove_faces(P p);
  P combine_textures(P p1, P p2); 
  P slow_calc_lights(P p, float light_dir_x, float light_dir_y, float light_dir_z);
  MS identity_pose(int li_size);
  ARR gltf_split_faces2(EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index, int max_attach);
  
  ARR split_faces(P p, ATT att, int max_attach);
  std::vector<P> orig_pose(EveryApi &ev, P p, LI li, int li_size);
  std::vector<P> orig_pose2(EveryApi &ev, std::vector<P> vec, LI li, int li_size);
  MS new_pose(EveryApi &ev, LI li_orig, LI li);
  ML ske_anim(EveryApi &ev, P mesh, LI orig_pose, int li_size, std::string url, std::vector<LI> new_poses, MT mat);
  ML ske_anim2(EveryApi &ev, std::vector<P> mesh, LI orig_pose, int li_size, std::string url, std::vector<LI> new_poses, MT mat);
  ATT find_attach2(P p, LI li);
  ATT attach_cache(ATT a, P p);
  
  P convex_hull(PTS pts);
  P optimize_mesh(P p, float max);
  P toon_outline(P p, float border_width);
  ML cullface(ML ml, bool b);
  P substitute(P p1, P p2, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float normal);
  ARR block_divide(P p, float pos_x, float pos_z, int sx, int sz, float delta_x, float delta_z);
  ARR block_render(GameApi::EveryApi &ev, std::vector<P> vec, MT mat);
  ARR block_render2(GameApi::EveryApi &ev, std::vector<P> vec, std::vector<MT> mat);
  ML block_draw(std::vector<ML> vec, float pos_x, float pos_z, int sx, int sz, float delta_x, float delta_z, int view);
  P bitmapsized_quad(BM bm);
  // Array functions
  PTS voxelarray_to_pts(AV arr, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  AV pts_to_voxel(PTS pts, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy, int sz);
  AV av_unique(AV arr);
  ML quad_tree(P p);
  P mix_mesh(P p, PTS points, float val);
  ARR poly_array(std::vector<P> vec);
  P poly_index(ARR arr, int idx);
  int poly_size(ARR arr);
  ARR poly_execute(EveryApi &ev, ARR arr, std::string gameapi_script);
  P normal_darkness(P p, float dark);
  P gltf_load( EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index );
  P gltf_load_nr( EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index );
  BM gltf_load_bitmap( GameApi::EveryApi &ev, std::string base_url, std::string url, int image_index );
  ARR material_extractor_p(P p, int start_index, int end_index);
  ARR material_extractor_bm(P p, int start_index, int end_index);
  ARR material_extractor_mt(EveryApi &ev, P p, float mix, int start_index, int end_index);
  MT material_index(EveryApi &ev, std::vector<MT> vec, int index);
  ARR material_arr(std::vector<MT> vec, int start_range, int end_range);
  ML mesh_anim_display_inst(EveryApi &ev, P p, FF val, MN move, MT mat, MS inst);
  ML mesh_anim_display_inst2(EveryApi &ev, P p, FF val, MN mn, MT mat, PTS inst);
  ML mesh_anim(EveryApi &ev, std::vector<P> vec, std::vector<MN> move, std::vector<MT> mat, std::vector<MS> inst, std::vector<IF> states, std::string url);
  P mesh_elem(P start, P end);
  P bar_chart( GameApi::EveryApi &ev, std::string url, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per );
  P bar_chart2( GameApi::EveryApi &ev, std::string url, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per, float per2 );
  LI li_piechart(float c_x, float c_y, float c_z, float start_angle, float end_angle, float radius, int numsteps);
  P p_piechart(EveryApi &ev, float c_x, float c_y, float start_angle, float end_angle, float radius, int numsteps, float start_z, float end_z);
  ML piechart_full(EveryApi &ev, float c_x, float c_y, std::string url, float radius, int numsteps, float start_z, float end_z);
  P li_polygon(LI li, float width);
  P gradient_color(P p, float p_x, float p_y, float p_z, float v_x, float v_y, float v_z, unsigned int start_color, unsigned int end_color);
  P texture_add(P p, BM bm);
  P stl_load(std::string url);
  P fix_vertex_order(P p);
  P filter_invisible(P p, float size);
  std::vector<TXID> mtl_parse(EveryApi&ev, std::vector<unsigned char> mtlfilecontents, std::string url_prefix, int delta=0);
  
  ML m_bind_inst_many(EveryApi &ev, std::vector<P> vec, std::vector<MT> materials, PTS pts);
  ML load_scene(EveryApi &ev, std::string url, int sx, int sy);

  CG curve_group_from_anim(MA ma, float start_time, float end_time);
  MA meshanim(std::vector<P> vec, float start_time, float end_time);
  P meshanim_mesh(MA ma, float time);
  P meshanim_mesh2(MA ma, float time1, float time2);
  ARR meshanim_anim_meshes(MA ma, float start_time, float delta_time, int count);

  P lod_choose(std::vector<P> vec, std::string name);
  P lod_set(P p, std::string name, int value);
  IF lod_select(float start_dist, float dist_step, int max_value);

  BM shadow_map(EveryApi &ev, P p, float p_x, float p_y, float p_z, int sx, int sy);
  BB shadow_map2(P p, float p_x, float p_y, float p_z, int sx, int sy, P quad);
  BM shadow_map3(EveryApi &ev, P objs, float p_x, float p_Y, float p_z, int sx, int sy, P quad);
  ML choose_time(ML next, std::vector<ML> vec, float delta_time);
  ML anim(EveryApi &ev, ML next, MA anim, float start_time, float delta_time, int count);
  ML anim_bind(EveryApi &ev, ML next, MA anim, MT material, float start_time, float delta_time, int count);
  
  // normal functions
  IMPORT void print_stat(P p);
  IMPORT void print_data(P p);
  IMPORT void print_data2(P p);
  IMPORT void print_stat(VA p);
	IMPORT P p_empty();
        IMPORT P load_model(std::string filename, int obj_num);
  IMPORT P load_model_all(std::string filename, int count);
  IMPORT P load_model_all_no_cache(std::string filename, int count);
  IMPORT P load_model_all_no_cache(LoadStream * file_data, int count);
  IMPORT P load_model_all_no_cache_mtl(std::string filename, int count, std::vector<std::string> material_names);
  IMPORT P load_model_all_no_cache_mtl(LoadStream * file_data, int count, std::vector<std::string> material_names);
  IMPORT P p_url(EveryApi &ev, std::string url, int count);
  IMPORT P p_mtl(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count);
  IMPORT ARR p_mtl2(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count, int start_index, int end_index, float mix);
  IMPORT ARR p_mtl_d(P p);
  IMPORT ARR p_mtl_bump(P p);
  IMPORT P p_url_mtl(EveryApi &ev, std::string url, int count, std::vector<std::string> material_names);
  IMPORT P p_ds(EveryApi &ev, const unsigned char *beg, const unsigned char *end);
  IMPORT P p_ds_url(EveryApi &ev, std::string url);
  IMPORT DS p_ds_inv(P model);
  IMPORT P file_cache(P model, std::string filename, int obj_num);
  IMPORT P resize_to_correct_size(P model);
        IMPORT void save_model(P poly, std::string filename);
  IMPORT ML save_model_ml(P poly, std::string filename);
  IMPORT P prepare_cut(P p);
  IMPORT P mesh_resize(P p, float start_x, float end_x, float start_y, float end_Y, float start_z, float end_z);
  
  IMPORT P texture_splitter(P p, int start_index, int end_index);
  IMPORT P texture_splitter2(P p, int num);
  IMPORT P replace_texture(P p, BM bm, int num);
  IMPORT P texture_storage(P p, int texture_sx, int texture_sy);
  IMPORT P light_transport(P p, int num, float light_dir_x, float light_dir_y, float light_dir_z);
  IMPORT BM texture_from_p(P p, int num);
  
  IMPORT P smooth_normals2(P p);
  IMPORT P from_normal_to_texcoord(P p);
  
  //IMPORT P line(PT p1, PT p2);
	IMPORT P triangle(PT p1, PT p2, PT p3);
	IMPORT P quad(PT p1, PT p2, PT p3, PT p4);
  IMPORT P quad_x(float x,float y1, float y2,float z1, float z2);
	IMPORT P quad_y(float x1, float x2,float y,float z1, float z2);
	IMPORT P quad_z(float x1, float x2,float y1, float y2,float z);
        IMPORT P fullscreen_quad(EveryApi &ev);
  IMPORT P vr_fullscreen_quad(EveryApi &ev, bool is_right);
  IMPORT P tri_strip(PT *array, int size);
        IMPORT P polygon2(std::vector<PT> vec);
        IMPORT P polygon(PT *array, int size); // use render_dynamic with this.
        IMPORT P polygon3(PTS points);
	IMPORT P tri_vertex_array(float *v_array, int v_size,
		     float *n_array, int n_size,
		     unsigned int *c_array, int c_size,
		     float *tex_array, int tex_size,
		     float **attrib_array, int a_size1, int a_size2);
	IMPORT P cube(float start_x, float end_x,
	 float start_y, float end_y,
	 float start_z, float end_z);
	IMPORT P cube(PT *p); // 8 points needed
  IMPORT P rounded_cube(EveryApi &ev, float start_x, float end_x,float start_y, float end_y,float start_z, float end_z,float round_radius);
  IMPORT P repeat_xy_p(EveryApi &ev, P p, float start_x, float start_y, float dx, float dy, int sx, int sy);
  IMPORT P repeat_xz_p(EveryApi &ev, P p, float start_x, float start_z, float dx, float dz, int sx, int sz);
  IMPORT P repeat_yz_p(EveryApi &ev, P p, float start_y, float start_z, float dy, float dz, int sy, int sz);
  IMPORT P deform(P obj, O bools, float dx, float dy, float dz);
  IMPORT P flip_normals(P obj);
  IMPORT P color_map2(BM bm, PT pos, V u_x, V u_y);
  IMPORT P color_map(BM bm, float start_x, float end_x, float start_y, float end_y, float z);
  IMPORT P color_map3(BM bm, FB height, PT pos, V u_x, V u_y);
  IMPORT P color_map3(BM bm, FB height, float sx, float sy, float z);
  IMPORT P color_map3_cyl(BM bm, FB height, PT pos, V u_x, V u_y);
  IMPORT P color_map3_cyl(BM bm, FB height, float sx, float sy, float z);
  IMPORT P color_map3_sph(BM bm, FB height, PT pos, V u_x, V u_y);
  IMPORT P color_map3_sph(BM bm, FB height, float sx, float sy, float z);
  IMPORT P color_map4(BM bm, FB height, float sx, float sy, float z);
  IMPORT P cube_map(float start_x, float end_x,float stary_y, float end_y,float start_z, float end_z,BM bm_front, BM bm_back, BM bm_left, BM bm_right, BM bm_top, BM bm_bottom);
	IMPORT P sphere(PT center, float radius, int numfaces1, int numfaces2);
  IMPORT P sphere_map(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values, int sx, int sy);
  IMPORT P plane_map(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float start_values, float end_values, FB fb, int sx, int sy);
	IMPORT P cone(int numfaces, PT p1, PT p2, float rad1, float rad2);
  IMPORT P torus2(EveryApi &ev, int numfaces1, int numfacesw2, PT center, float radius1, float radius2);
  IMPORT P torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2);
	IMPORT P ring(float sx, float sy, float x, int steps); // use RingEffect::Init() to implement
  IMPORT P disc(EveryApi &ev, int numfaces, float center_x, float center_y, float center_z, float normal_x, float normal_y, float normal_z, float radius);
  enum HeightMapType { EQuad, ETriangle };
  IMPORT P heightmap(BM bm, HeightMapType t,
	      float min_x, float max_x, 
	      float min_y, float max_y,
	      float min_z, float max_z);
  IMPORT P heightmap(FB fb, std::function<P (float)> f, float dx, float dz);
  IMPORT P circular_span(EveryApi &ev, LI li, float delta_angle_around_y_axis, int num_steps);
  IMPORT P linear_span(EveryApi &ev, LI li, float dx, float dy, float dz, int num_steps);
  IMPORT P cut_faces(P p, O o, CT cutter);
  IMPORT P tri_to_quad(P p);
  IMPORT P curve_to_poly(C c, float start_x, float end_x, float start_y, float end_y, float start_angle, float end_angle, int numinstances);

  IMPORT P split_p(P p, int start_face, int end_face);
  IMPORT P subpoly_change(P p, P p2, O choose);

  IMPORT P span(LI li, M matrix, int num_steps);
  IMPORT P fromsurface(S s, float thickness);
  IMPORT P fromsurface(S s1, S s2, C curve); // surfacebetweensurfaces
  
  IMPORT P sprite_bind(P p, TX tx, int id);
  IMPORT P sprite_bind(P p, Q bm, TX tx);
  P sprite_bind_1(P p, Q bm, TX tx);
  IMPORT P texture(P orig, BM bm, int bm_choose = -1); // all quads

  IMPORT P line_to_cone(EveryApi &ev, LI li, float size, int numfaces);
  IMPORT ML line_to_cone2(EveryApi &ev, LI li, float size, int numfaces, MT mt);
  IMPORT P static_instancing(EveryApi &ev, P obj, PTS pos);
  IMPORT P static_instancing_vertex_color(EveryApi &ev, P obj, PTS pos);
  IMPORT P static_instancing_matrix(EveryApi &ev, P obj, MS matrix_array);
  IMPORT LI li_static_instancing_matrix(EveryApi &ev, LI obj, MS matrix_array);
  IMPORT P static_instancing_with_color(EveryApi &ev, P obj, BM bm, float start_x, float end_x, float start_y, float end_y, float z);
  
  IMPORT P color_distance(P faces, float center_x, float center_y, float center_z, unsigned int color_center, unsigned int color_dist, float dist_center, float dist_dist);
  IMPORT P color(P orig, unsigned int color);
  IMPORT P color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z);
  IMPORT P mix_color(P orig, P orig2, float val);
  IMPORT P min_color(P orig, P orig2);
  IMPORT P max_color(P orig, P orig2);
  IMPORT P color_lambert(P orig, unsigned int color, float light_dir_x, float light_dir_y, float light_dir_z, float pow, float intensity);
  IMPORT P texcoord_cube(P orig,PT o, PT u_x, PT u_y, PT u_z, PT tex_o, PT tex_x, PT tex_y, PT tex_z); // tex_* are 2d
  IMPORT P texcoord_default(P orig);
  IMPORT P texcoord_manual(P orig, float p1_x, float p1_y,float p2_x, float p2_y,float p3_x, float p3_y,float p4_x, float p4_y);
  IMPORT P texcoord_spherical(PT center, P orig);
  IMPORT P texcoord_spherical2(EveryApi &ev, PT center, float r, P orig);
  IMPORT P texcoord_cylindar(P orig, float start_y, float end_y);
  IMPORT P texcoord_from_normal(P p);
  IMPORT P texcoord_from_normal2(P p);
  IMPORT P texcoord_plane(P p, float start_x, float end_x, float start_y, float end_y);
  IMPORT P texcoord_subarea(P p, float start_x, float end_x, float start_y, float end_y);
  IMPORT P fix_texcoord(P p);
  IMPORT P color_cube(P orig,
	       PT o, PT u_x, PT u_y, PT u_z,
	       unsigned int color_o, unsigned int color_x, unsigned int color_y, unsigned int color_z);
  IMPORT P color_faces(P orig,unsigned int color_1, unsigned int color_2,unsigned int color_3, unsigned int color_4);
  IMPORT P color_from_normals(P orig);
  IMPORT P color_alpha(P orig, unsigned int alpha);
  IMPORT P color_grayscale(P orig);
  IMPORT P color_from_texcoord(P orig,unsigned int color_1, unsigned int color_2,unsigned int color_3, unsigned int color_4);
  IMPORT P color_range(P orig, unsigned int source_upper, unsigned int source_lower, unsigned int upper_range, unsigned int lower_range);

  IMPORT P texcoord_poly(P orig, int facenum, PT *array, int size);
  IMPORT P choose_texture(P orig, int num);
  IMPORT P color_poly(P orig, int facenum, unsigned int *array, int size);

  IMPORT P quads_to_triangles(P p);
  IMPORT P flip_polygon_order(P p);

  IMPORT P skeletal_anim(P p, PT p_0, PT p_1, PT n_0, PT n_1);

  IMPORT P build_offsets(P p, std::vector<PT> vec);

  IMPORT P or_elem(P p1, P p2);
  IMPORT P or_array(P *array, int size);
  IMPORT P or_array2(std::vector<P> vec);
  P or_array_1(P *array, int size);
  IMPORT P and_not_elem(EveryApi &ev, P p1, P p_not, 
			O o1, O o_not, 
			CT cutter1, CT cutter_not);
  IMPORT P intersect(EveryApi &ev, P p1, P p2,
		     O o1, O o2,
		     CT cutter1, CT cutter2);
  IMPORT P alt(std::vector<P> vec, int index);
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
  IMPORT P matrix(P orig, M mat);
  IMPORT P matrix44(P orig, M mat); // otherwise the same as matrix, but doesnt do normals.
  
  IMPORT P log_coords(P p);
  IMPORT P log_coords2(P p, int x_count, int y_count, float sx, float sy, float sz);
  IMPORT P spherical_wave(P p, float r1, float fr_1, float r2, float fr_2);
  
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
  IMPORT P spherical_normals(P p, float p_x, float p_y, float p_z);
  IMPORT P average_normals(P orig, int sx, int sy);
  IMPORT P smooth_normals(P orig);
  IMPORT P memoize(P orig);
  IMPORT P memoize_all(P orig);
  IMPORT P persistent_cache(P orig, std::string filename);

  IMPORT S s_spherical(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values);
  IMPORT P s_sample(S surf, int sx, int sy);

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
  IMPORT void update(VA va);
  IMPORT bool is_texture(VA va);
  IMPORT bool is_array_texture(VA va);

  IMPORT void update_vertex_array(VA va, P p, bool keep=false);
  IMPORT void update_vertex_array_no_memory(VA va, P p);
  IMPORT ML update_vertex_array_ml(VA va, P p, bool keep=false);
  IMPORT VA create_vertex_array(P p, bool keep=false); // slow
  IMPORT void create_vertex_array_hw(VA va);
  IMPORT void delete_vertex_array(VA arr);
  IMPORT VA create_vertex_array_attribs(P p, bool keep,std::vector<int> attribs, std::vector<int> attribi); // slow
  IMPORT void render_vertex_array(VA va); // fast
  IMPORT void clone(VA va);
  IMPORT void render_vertex_array_dyn(VA va, DC dc, MainLoopEnv &e);
  IMPORT void prepare_vertex_array_instanced(ShaderApi &ev, VA va, PTA pta, SH sh);
  IMPORT void render_vertex_array_instanced(ShaderApi &ev, VA va, PTA pta, SH sh, int hide_n = -1); // fast
  IMPORT void prepare_vertex_array_instanced_matrix(ShaderApi &ev, VA va, MSA pta, SH sh);
  IMPORT void render_vertex_array_instanced_matrix(ShaderApi &ev, VA va, MSA pta, SH sh, int hide_n = -1); // fast
  IMPORT ML render_vertex_array_ml(EveryApi &ev, VA va);
  IMPORT ML render_vertex_array_ml2(EveryApi &ev, P va);
  IMPORT ML render_vertex_array_ml2_texture(EveryApi &ev, P va, std::vector<BM> vec, std::vector<int> types=std::vector<int>());
  IMPORT ML render_vertex_array_ml2_texture_id(EveryApi &ev, P va, std::vector<TXID> *vec);
  IMPORT ML render_vertex_array_ml2_cubemap(EveryApi &ev, P va, std::vector<BM> vec);
  IMPORT ML render_vertex_array_ml2_texture2(EveryApi &ev, P p);
  IMPORT SBM texture_sbm();
  IMPORT SBM sbm_circle(float start_time, float end_time, float start_pos_x, float start_pos_y, float end_pos_x, float end_pos_y, float start_radius, float end_radius);
  IMPORT SBM sfo_sbm(EveryApi &ev, SFO sfo);
  IMPORT SBM combine_sbm(SBM texture1, SBM texture2);
  IMPORT SBM bloom_cut_sbm(SBM texture, float r, float g, float b);
  IMPORT SBM blur_sbm(SBM texture, float pixel_x, float pixel_y);
  IMPORT ML sbm_texture(EveryApi &ev, ML mainloop, SBM bitmap);
  IMPORT ML render_dynamic_ml(EveryApi &ev, P p, DC dyn);
  IMPORT ML dist_field_mesh_shader(EveryApi &ev, ML mainloop, SFO sfo);
  IMPORT ML mesh_color_shader(EveryApi &ev, ML mainloop, SFO sfo);
  IMPORT ML sfo_sandbox_shader(EveryApi &ev, ML mainloop, SFO sfo);
  IMPORT ML glowedge_shader(EveryApi &ev, ML mainloop, float white_level, float gray_level, float edge_pos);
  IMPORT ML phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT ML phong_shader2(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT ML vertex_phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, float mix);
  IMPORT ML edge_shader(EveryApi &ev, ML mainloop, float edge_width, unsigned int edge_color);
  IMPORT ML gltf_anim_shader(GameApi::EveryApi &ev, ML ml_orig, std::vector<GameApi::ML> mls, int key);
  IMPORT ML globe_shader(EveryApi &ev, ML mainloop, float globe_r);
  //IMPORT ML ao_shader(EveryApi &ev, ML mainloop, float radius, int kernelsize, int noisesize);
  IMPORT ML colour_shader(EveryApi &ev, ML mainloop, float mix);
  IMPORT ML gi_shader(EveryApi &ev, ML mainloop, PTS points, float obj_size);
  IMPORT ML bump_phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow);
  IMPORT ML fog_shader(EveryApi &ev, ML mainloop, float fog_dist, unsigned int dark_color, unsigned int light_color);
  IMPORT ML shadow_shader(EveryApi &ev, ML mainloop, int tex_num, float p_x, float p_y, float p_z, unsigned int dark_color, float mix);
  IMPORT ML dyn_lights_shader(EveryApi &ev, ML mainloop, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point);
  IMPORT ML shading_shader(EveryApi &ev, ML mainloop,unsigned int level1,unsigned int level2,unsigned int level3, float spec_size=5.0f, bool ambient=true, bool diffuse=true, bool specular=false);
  IMPORT ML spotlight_shader(EveryApi &ev, ML mainloop,int light_color_id, MN move);
  IMPORT ML ambient_shader(EveryApi &ev, ML mainloop,int ambient_color_id,float ambient_level);
  IMPORT ML noise_shader(EveryApi &ev, ML mainloop);
  IMPORT ML custom_shader(EveryApi &ev, ML mainloop, std::string v_shader, std::string f_shader, std::string v_funcname, std::string f_funcname);
  IMPORT ML dither_shader(EveryApi &ev, ML mainloop);
  IMPORT ML light_shader(EveryApi &ev, ML mainloop);
  IMPORT ML choose_color_shader(EveryApi &ev, ML mainloop, unsigned int color, float mix_val);
  IMPORT ML blur_shader(EveryApi &ev, ML mainloop, float x_val, float y_val);
  IMPORT ML bloom1_shader(EveryApi &ev, ML mainloop, float r_val, float g_val, float b_val);
  IMPORT ML wave_shader(EveryApi &ev, ML mainloop, float radius, float t_mult, float x_mult, float y_mult);
  IMPORT ML toon_shader(EveryApi &ev, ML mainloop);  
  IMPORT ML fade_shader(EveryApi &ev, ML mainloop, float start_time, float end_time, float start_time2, float end_time2);
  IMPORT ML texture_shader(EveryApi &ev, ML mainloop, float mix);
  IMPORT ML texture_many_shader(EveryApi &ev, ML mainloop, float mix);
  IMPORT ML texture_cubemap_shader(EveryApi &ev, ML mainloop, float mix, float mix2);
  IMPORT ML texture_arr_shader(EveryApi &ev, ML mainloop, float mix);
  IMPORT ML skeletal_shader(EveryApi &ev, ML mainloop, std::vector<SA> vec);
  IMPORT ML gltf_shader(EveryApi &ev, ML mainloop, float mix, bool tex0, bool tex1, bool tex2, bool tex3, bool tex4, bool tex5, bool tex6, bool tex7, float roughness, float metallic, float basecolor0, float basecolor1, float basecolor2, float basecolor3, float occul, float emiss);
  IMPORT void explode(VA va, PT pos, float dist);
  //IMPORT int accexss_point_count(VA va, bool triangle);
  //IMPORT float *access_points(VA va, bool triangle, int face, int point);
  //IMPORT float *access_color(VA va, bool triangle, int face, int point);
  //IMPORT float *access_normals(VA va, bool triangle, int face, int point);
  //IMPORT float *access_texcoord(VA va, bool triangle, int face, int point);
  //IMPORT void update(VA va);

  IMPORT ML sprite_render(EveryApi &ev, BM bm, float start_x, float end_x, float start_y, float end_y, float z);
  IMPORT ML sprite_render_inst(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z);
  IMPORT ML sprite_render_fade(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z, bool flip, float start_time, float end_time);
  
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
  IMPORT P world_from_bitmap(EveryApi &ev, std::vector<P> vec, std::string filename, std::string chars, float dx, float dy, int sx, int sy);
  IMPORT P world_from_bitmap2(EveryApi &ev, std::function<P (int c, PT tl, PT tr, PT bl, PT br)> f, BM int_bm, FB float_bm, float dx, float dz, float height);
  IMPORT P world_from_voxel(std::function<P (unsigned int c)> f, VX voxel, float dx, float dy, float dz, int max_c);

  IMPORT P from_points(PTS p, std::function<P (int i, float x,float y,float z, unsigned int color)> f);
  IMPORT P from_lines(LI li, std::function<P (int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)> f);
  IMPORT P from_polygon(P p, std::function<P (int face,
					      PT p1, PT p2, PT p3, PT p4)> f);
  P from_polygon_1(P p, std::function<P (int face, 
					 PT p1, PT p2, PT p3, PT p4)> f);


  IMPORT P dist_from_lines(LI li, float d1, float d2, PT center);

  IMPORT BM renderpolytobitmap(EveryApi &ev, P p, SH sh, float x, float y, float z, int sx, int sy);
  IMPORT ML position_based_on_screen(ML obj); 


  IMPORT std::vector<MaterialDef> parse_mtl(std::string filename);
  IMPORT std::string output_ml(std::string objfileurl, int count, std::string prefix, std::vector<MaterialDef> filenames);

  IMPORT int p_num_textures(P p);
  IMPORT void p_gen_texture(P p, int i);
  IMPORT BM p_texture(P p, int i);
  
private:
  PolygonApi(const PolygonApi&);
  void operator=(const PolygonApi&);

  void *priv;
  Env &e;
};
#endif

#ifdef F_WAVEFORM_API
class WaveformApi
{ // [0..length] -> [-1..1]
public: 
	IMPORT WaveformApi(Env &e) : e(e) { }
	IMPORT WV wv_empty(float length);
	IMPORT WV function(std::function<float(float)> f, float length, float min_value, float max_value);
	IMPORT WV sinwave(float length, float freq);
  IMPORT WV gaussian(float start_x, float end_x, float start_y, float end_y);
  IMPORT WV sum(WV w1, WV w2);
  IMPORT WV move(WV w1, float delta);
  IMPORT WV scale(WV w1, float scale);
        IMPORT WV repeat(WV wave, int num);
	IMPORT WV sample(float *array, int length, float samplelength);
	IMPORT WV int_sample(int *array, int length, float samplelength, int min_value, int max_value);
	IMPORT WV mix(WV orig, float pos, WV sample);
	IMPORT WV volume_ramp(WV orig, float old_y_value, float x_pos1, float x_pos2, float y_pos1, float y_pos2);
	IMPORT WV freq_change(WV orig, float old_freq, float new_freq);
	IMPORT float length(WV orig);
	IMPORT float get_value(WV orig, float val);
	IMPORT WV length_change(WV orig, float new_length);
  IMPORT WV step(bool b);
  IMPORT WV cubic(float f_0, float f_1, float df_0, float df_1, float min_y, float max_y);
	IMPORT BM waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color);
  IMPORT P waveform_rotated_polygon(WV wave, float start_angle, float end_angle, float radius, int num_samples, int num_waves);

  IMPORT PN std_polynomial(float x_5, float x_4, float x_3, float x_2, float x_1, float c);
  IMPORT PN df_dx(PN poly);
  IMPORT WV polynomial_wave(PN pn, float start_x, float end_x, float start_y, float end_y);
  
private:
  WaveformApi(const WaveformApi&);
  void operator=(const WaveformApi&);
  Env &e;
};
#endif
#ifdef F_SAMPLE_COLLECTION_API
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
#endif
#ifdef F_TRACKER_API
class TrackerApi
{
public:
  IMPORT TrackerApi(Env &e) : e(e) { }
  IMPORT TRK empty(int numchannels, int numslots);
  IMPORT TRK audio_slot(TRK orig, int channel, int slot, int duration, int sample);
  IMPORT TRK array(TRK *array, int size);
  IMPORT TBUF prepare(TRK trk);
  IMPORT void play_song(EveryApi &ev, TBUF buf, WAV samples, int framenum, int speed);
  IMPORT void play_mp3(std::string filename);
  IMPORT void play_ogg(std::string filename);
  IMPORT void play_ogg(const std::vector<unsigned char> &vec);
  IMPORT void play_ogg(void*);
  IMPORT void* setup_ogg(const std::vector<unsigned char> &data);
  IMPORT void stop_music_playing();
  IMPORT ML play_wave_via_keypress(EveryApi &ev, ML ml, std::string url, int key);
private:
  Env &e;
};
#endif

class ShaderApi;

#ifdef F_STATE_CHANGE_API
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
#endif

#ifdef F_NEW_PLANE_API
class NewPlaneApi
{
public:
  IMPORT NewPlaneApi(Env &e) : e(e) { }
  IMPORT PP function(std::function<PT (int idx)> f, int num_points, float px, float py, float sx, float sy);
  IMPORT PP color_function(PP pl, std::function<unsigned int (int idx)> f);
  IMPORT PP polygon(std::vector<PT> vec);
  IMPORT PP polygon2(PTS pts); // kills z
  IMPORT PP color(PP p, unsigned int color);
  IMPORT PP trans(PP pl, float dx, float dy);
  IMPORT std::vector<PP> single_elem(PP p1);
  IMPORT std::vector<PP> or_elem(PP p1, PP p2);
  IMPORT std::vector<PP> or_elem(std::vector<PP> p1, std::vector<PP> p2);
  IMPORT PP cicle(PT center, float radius, int numpoints);
  IMPORT PP star(PT center, float radius_1, float radius_2, int numpoints);
  
  IMPORT LI to_lines(PP pl, float z);
  IMPORT P to_poly(PP pl, float z);

  //
  // new plane
  // 
  PLF reverse_faces(PLF faces); // if triangulate doesnt work, use reverse_faces first
  PLF triangulate(PLF faces);
  P to_polygon(PLF faces);
private:
  Env &e;
};
#endif

#ifdef F_PLANE_API
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

  IMPORT P to_polygon_strip(EveryApi &ev, PL pl, PT pos, V u_x, V u_y);

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
#endif

#ifdef F_BOOL_BITMAP_API
class BoolBitmapApi
{ // NxN->2
public:
  IMPORT BoolBitmapApi(Env &e);
  IMPORT ~BoolBitmapApi();
  IMPORT BB bb_empty(int sx, int sy);
  BB function(std::function<bool(int, int)> f, int sx, int sy);
  IMPORT BB transform(BB orig, std::function<bool(int, int, bool)> f);
  IMPORT O to_volume(BB b, float dist);
  IMPORT BB from_float_bitmap(FB float_bm, float range_start, float range_end);
  IMPORT BB from_bitmaps_color(BM bm, int r, int g, int b);
  IMPORT BB from_bitmaps_color_area(BM bm, std::function<bool(int r, int g, int b, int a)> f);
  IMPORT BB from_bitmaps_color_area(BM bm, int r_start, int r_end,int g_start, int g_end, int b_start, int b_end, int a_start, int a_end);
  IMPORT BB line(BB bg, float p_x, float p_y, float p2_x, float p2_y, float line_width1, float line_width2);
  IMPORT BB tri(BB orig, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y);

  IMPORT BB circle(BB bg, float center_x, float center_y, float radius);
  IMPORT BB ellipse(BB bg, float center_x, float center_y, float center2_x, float center2_y, float sum_of_distances);
  //IMPORT BB rectangle(BB bg, int x, int y, int width, int height); // for static ones
  IMPORT BB rectangle(BB bg, float x, float y, float width, float height); // for moving
  IMPORT BB sprite(BB bg, BB sprite, float x, float y, float size_multiplier_x, float size_multiplier_y);
  IMPORT BB polygon(BB bg, PT *points, int size);
  IMPORT BB text(BB bg, int x, int y, const char *string, int size,BB *glyphs, int glyphcount, int(*fptr)(EveryApi &ev, char));
  
  IMPORT BB part_circle(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad);
  IMPORT BB sections(int sx, int sy, float x, float y, std::function<bool (float angle)> f);
  IMPORT BB rings(int sx, int sy, float center_x_start, float center_y_start, float center_x_end, float center_y_end, float start_radius, float end_radius, float start_thickness, float end_thickness, int numrings);

  IMPORT BB not_bitmap(BB b);
  IMPORT BB or_bitmap(BB b1, BB b2);
  IMPORT BB andnot_bitmap(BB b1, BB not_b2);
  IMPORT BB xor_bitmap(BB b1, BB flip_b2);
  IMPORT BM choose_bitmap(BB bools, BM true_bitmap, BM false_bitmap);
  
  IMPORT BM to_bitmap(BB bools,int true_r, int true_g, int true_b, int true_a,int false_r, int false_g, int false_b, int false_a);
  BM to_bitmap_1(BB bools,int true_r, int true_g, int true_b, int true_a,int false_r, int false_g, int false_b, int false_a);
  IMPORT BM texture(BM bg,BB bools1, int l1, int t1,BM texturebitmap2, int l2, int t2);
  IMPORT int size_x(BB bm);
  IMPORT int size_y(BB bm);
  IMPORT bool boolvalue(BB bb, int x, int y);
  IMPORT BB black_white_dithering(FB fb);
private:
  BoolBitmapApi(const BoolBitmapApi&);
  void operator=(const BoolBitmapApi&);

  Env &e;
};
#endif

#ifdef F_FLOAT_BITMAP_API
class FloatBitmapApi
{ // NxN->R
public: // values are [0.0..1.0]
  IMPORT FloatBitmapApi(Env &e);
  IMPORT ~FloatBitmapApi();
  IMPORT FB fb_empty(int sx, int sy);
  IMPORT FB function(std::function<float(int, int)> f, int sx, int sy);
  IMPORT FB newfloatbitmap(char *array, int sx, int sy, std::function<float(char)> f);
  IMPORT FB from_bool_bitmap(BB bm, int csx, int csy);
  IMPORT FB dist(int sx, int sy, float p_x, float p_y);
  IMPORT FB blur_bitmap(FB fb, float d);
  IMPORT FB gaussian(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy);
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
  IMPORT BM to_grayscale_color(FB fb,int r, int g, int b, int a,int r2, int g2, int b2, int a2);
  IMPORT BM to_color(FB r, FB g, FB b, FB a);
  IMPORT BM choose_bitmap(FB fb, BM bm1, BM bm2);
  IMPORT FB perlin_noise(FB grad_1, FB grad_2);
  IMPORT BM subfloatbitmap(EveryApi &ev, FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color);
  
  IMPORT FB from_bool(BB b, float val_true, float val_false);
  IMPORT FB distance_field(FB bb, float max_value);
  
  IMPORT FB add_border(FB fb);

  BB to_bool(FB f, float true_range_start, float true_range_end);
  
  IMPORT BB to_bool_mod(FB f, float mod_value);

  IMPORT TXID to_texid(FB bm);

  IMPORT CFB C_bitmap(float sx, float sy, float val);
  IMPORT CFB X_bitmap(float sx, float sy);
  IMPORT CFB Y_bitmap(float sx, float sy);
  IMPORT CFB MulBitmap(std::vector<CFB> vec);
  IMPORT CFB AddBitmap(std::vector<CFB> vec);
  IMPORT CFB SqrtContFloat(CFB val);
  IMPORT CFB Sin(CFB val);
  IMPORT CFB Cos(CFB val);
  IMPORT CFB Tan(CFB val);
  IMPORT FB SampleContFloat(CFB a1, float start_x,float end_x,float start_y,float end_y,int sx,int sy, float mult);
  IMPORT BB SampleContBool(CBB a1, float start_x,float end_x,float start_y,float end_y,int sx,int sy);
  IMPORT CBB Equalizer(CFB,CFB);
  

  IMPORT int size_x(FB bm);
  IMPORT int size_y(FB bm);
  IMPORT float floatvalue(FB bm, int x, int y);
private:
  FloatBitmapApi(const FloatBitmapApi&);
  void operator=(const FloatBitmapApi&);
  Env &e;
};
#endif

#ifdef F_VECTOR_BITMAP_API
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
#endif

#ifdef F_CONTINUOUS_BITMAP_API
class ContinuousBitmapApi
{ // RxR->RGB
public:
  IMPORT ContinuousBitmapApi(Env &e);
  IMPORT CBM cbm_empty(float x, float y);
  CBM constant(unsigned int color, float x, float y);
  IMPORT CBM function(std::function<unsigned int(float, float)> f, float sx, float sy);
  IMPORT BM sample(CBM c_bitmap, int sx, int sy);
  IMPORT CBM from_bitmap(BM bm, float xsize, float ysize);
  IMPORT BM to_bitmap(CBM bm, int sx, int sy);

  IMPORT CBM rotate(CBM bm, float center_x, float center_y, float angle);
  IMPORT CBM surfacecolor(S s, COV cov);
  IMPORT CBM bicubic(float f_0, float f_1, float df_0, float df_1,
		     float ff_0, float ff_1, float dff_0, float dff_1);
  IMPORT unsigned int get_pixel(CBM bitmap, float x, float y);
public: // Different rendering functions...
  CBM distance_render(FD obj, COV colours, float sx,float sy);

private:
  ContinuousBitmapApi(const ContinuousBitmapApi&);
  void operator=(const ContinuousBitmapApi&);

  Env &e;
};
#endif

#ifdef F_VOXEL_API
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
  IMPORT PTS instanced_positions(VX x, float sx, float sy, float sz, unsigned int value);

  // int voxel
  IMPORT VX empty_voxel(int sx, int sy, int sz);
  IMPORT VX subvoxel(VX voxel, int start_x, int end_x, int start_y, int end_y, int start_z, int end_z);
  IMPORT VX blit_voxel2(VX voxel, VX voxel2, int p_x, int p_y, int p_z);
  IMPORT VX blit_voxel(O object, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int false_value, int true_value);
  IMPORT ARR voxel_instancing(VX voxel, int count, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  IMPORT ML voxel_render(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec);
  IMPORT ML voxel_bind(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec, MT material);
  IMPORT P voxel_static(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec);
  IMPORT VX voxel_landscape_from_fbm(FB bitmap, int height, int false_val, int true_val);
  IMPORT VX convert_p_to_vx(P p, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value);
  IMPORT VX from_implicit(IM i, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value);
private:
  Env &e;
};
#endif

#ifdef F_COLOR_API
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
#endif

#ifdef F_POINT_API
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
        IMPORT PT spherical_coords(PT pos);
        IMPORT PT cartesian_coords(PT pos);

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
#endif

#ifdef F_POINT_COLLECTION_API		     
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
#endif

#ifdef F_MATRICES_API
class MatricesApi
{
public:
  MatricesApi(Env &e) :e(e) { }
  IMPORT MS interpolate(MS start, MS end, float val);
  IMPORT MS inverse_ms(MS ms);
  IMPORT MS from_points(PTS pts);
  IMPORT MS mult(MS m, M mat);
  IMPORT MS mult(M mat, MS m);
  IMPORT MS repeat_ms(EveryApi &ev, MN mn, int val);
  IMPORT MS subarray(MS m, int start, int count);
  IMPORT MS ms_random_rot(float px, float py, float pz, int count);
  IMPORT MS mult_array(MS m1, MS m2);
  IMPORT MS from_lines_2d(LI li);
  IMPORT MS from_lines_3d(LI li);
  IMPORT MSA prepare(MS p);

private:
  Env &e;
};
#endif

#ifdef F_POINTS_API
class PointsApi
{
public:
  PointsApi(Env &e) : e(e) { }
  IMPORT P ply_faces(std::string url);
  IMPORT PTS ply_pts(std::string url);
  IMPORT PTS iterate_points(PTS points, VFi field, FA speed);
  IMPORT VFi pressure_gradient(FFi pressure);
  IMPORT VFi matrix_field(MN start, MN end);
  IMPORT FA random_speeds(float start_speed, float end_speed, int num);
  IMPORT PTS bullet(int key, float vx, float vy, float vz, float px, float py, float pz, float dist, int max_count, float repeat_time);
  IMPORT PTS voxelarray_to_pts(AV arr);
  IMPORT PTS polygon_face_center_points(P p);
  IMPORT PTS single_pts();
  IMPORT PTS pt_array(EveryApi &ev, std::vector<PT> vec);
  IMPORT PTS function(std::function<PT(int pointnum)> f, int numpoints);
  IMPORT PTS color_function(PTS orig, std::function<unsigned int(int pointnum, PT pos)> f);
  IMPORT PTS collision_points(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  IMPORT ML collision_bind(EveryApi &ev, PTS bounding_box, std::string name);
  IMPORT ML collision_bind_inst(EveryApi &ev, PTS bounding_box, PTS inst_points, std::string name);
  IMPORT ML collision_collect(ML mainloop);
  IMPORT PTS from_float_volume(FO float_volume, int numpoints, float start_x, float start_y, float start_z,float end_x, float end_y, float end_z);
  IMPORT PTS color_points(PTS p, unsigned int color);
  IMPORT PTS li_pts(LI li, float pos);
  IMPORT PTS li_pts2(LI li); 
  IMPORT PTS hemisphere_points(PT points, V normal, float r, int numpoints);
  IMPORT PTS pts_grid(BM bm, float start_x, float end_x, float start_y, float en_y, float z);
  IMPORT PTS pts_grid_bb(BB bb, float start_x, float end_x, float start_y, float en_y, float z);
  IMPORT PTS or_points(PTS p1, PTS p2);
  IMPORT PTS sort_pts(PTS p);
  IMPORT PTS heightmap(BM colour, FB floatbitmap, PT pos, V u_x, V u_y, V u_z, int sx, int sy);
  IMPORT PTS random_plane(PT pos, V u_x, V u_y, int numpoints);
  IMPORT PTS random_bitmap_instancing(EveryApi &ev, BB bm, int count, float start_x, float end_x, float start_y, float end_y, float z);
  IMPORT PTS random_mesh_quad_instancing(EveryApi &ev, P p, int count);
  IMPORT PTS surface(S surf, int sx, int sy);
  IMPORT PTS surface(std::function<PT (float,float)> surf,
		     std::function<unsigned int (PT,float,float)> color,
		     float start_u, float end_u,
		     float start_v, float end_v,
		     float step_u, float step_v);
  IMPORT PTS from_volume(O o, PT pos, V u_x, V u_y, V u_z, int sx, int sy, int sz);
  IMPORT PTS anim_mix(PTS o1, PTS o2, float start_val, float end_val, float start_time, float end_time);
  IMPORT PTS move(PTS obj, float dx, float dy, float dz);
  IMPORT PTS scale(PTS obj, float sx, float sy, float sz);
  IMPORT PTS rot_x(PTS obj, float angle);
  IMPORT PTS rot_y(PTS obj, float angle);
  IMPORT PTS rot_z(PTS obj, float angle);
  IMPORT PTS shadow_points(PTS obj, PT pos, V u_x, V u_y, V light_vec);

  IMPORT PTS unit_cube(PTS orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT PTS unit_to_cube(PTS orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT PTS unit_to_flex(PTS orig, 
			PT bTL, PT bTR, PT bBL, PT bBR,
			PT fTL, PT fTR, PT fBL, PT fBR);
  IMPORT PTS wave_points(WV wave, int num_samples,
			 float pos_x, float pos_y, float pos_z,
			 float u_x_x, float u_x_y, float u_x_z,
			 float u_y_x, float u_y_y, float u_y_z);
  IMPORT PTS filter_component(PTS pts, int comp, float val);
  IMPORT PTS anim_rot_pts(PTS pts, float start_time, float end_time, float v_x, float v_y, float v_z, float rotate_amount);

  IMPORT PTA prepare(PTS p);
  IMPORT int num_points(PTA pta);
  IMPORT int num_points(MSA pta);
  float *point_access(PTA pta, int pointnum); // use ptr[0], ptr[1] and ptr[2] to access the x,y,z coordinate
  IMPORT void set_point(PTA pta, int pointnum, float x, float y, float z);
  //unsigned int *color_access(PTA pta, int pointnum);
  void update_from_data(PTA array, PTS p);
  void update_from_data(MSA array, MS p);
  void update(PTA array);
  IMPORT void render(PTA array);
  IMPORT ML render_ml(EveryApi &ev, PTA array);
  IMPORT void explode(PTA array, float x, float y, float z, float dist);

  IMPORT PTS standard_box(int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  IMPORT PTS matrix_points(PTS orig, M matrix);
  IMPORT LI matrix_display(EveryApi &ev, M matrix);
  IMPORT LI matrix2_display(EveryApi &ev, M matrix1, M matrix2, int sx,int sy,int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  IMPORT ML movement_display(EveryApi &ev, ML ml, MN mn, int count, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z);
  
  IMPORT int NumPoints(PTS p);
  IMPORT int NumPoints(MS p);
  IMPORT float pos_x(PTS p, int index);
  IMPORT float pos_y(PTS p, int index);
  IMPORT float pos_z(PTS p, int index);
  IMPORT ML pts_render(EveryApi &ev, PTS pts);
  IMPORT LI li_from_pts(PTS pts, float dx, float dy, float dz);
  IMPORT PTS memoize_pts(PTS pts);
private:
  PointsApi(const PointsApi&);
  void operator=(const PointsApi&);

  Env &e;
};
#endif

#ifdef F_LINES_API		     
class LinesApi
{
public:
	IMPORT LinesApi(Env &e) : e(e) { }
  IMPORT LI alt(std::vector<LI> v, int index);
  IMPORT LI li_empty();
  IMPORT LI li_url(std::string url);
  IMPORT LI function(std::function<PT(int linenum, bool id)> f,
	      int numlines);
  IMPORT LI line_pos_mult(float val, LI li);
  IMPORT LI point_array(std::vector<PT> vec);
  IMPORT LI li_or_array(std::vector<LI> vec);
  IMPORT LI li_bevel(LI li, P p, float mix);
  IMPORT P p_towards_normal(P p, float amount);
  IMPORT LI li_matrix(LI lines, M matrix);
  IMPORT LI color_function(LI lines, std::function<unsigned int(int linenum, bool id)> f);
  IMPORT LI change_color(LI li, unsigned int color);
  IMPORT LI change_color(LI li, unsigned int color_1, unsigned int color_2);
	IMPORT LI from_points(PC points, bool loops);
        IMPORT LI from_points2(PTS start_points, PTS end_points);
        IMPORT LI from_plane(PL plane);
	IMPORT LI from_polygon(P poly);
        IMPORT LI from_polygon2(P poly1, P poly2);
  IMPORT LI normals_from_polygon(P poly, float length);
        IMPORT P line_anim(P poly, LI lines_from_polygon2, float val);
        IMPORT PTS pts_line_anim(LI lines, float val);
  IMPORT PTS pts_line_anim2(LI lines, std::function<float (int)> f);
	IMPORT LI border_from_bool_bitmap(BB b, float start_x, float end_x,
			     float start_y, float end_y, float z);
  IMPORT LI render_slice_2d(P p, PT pos, V u_x, V u_y);
  IMPORT LI translate(LI lines, float dx, float dy, float dz);
  IMPORT LI scale(LI lines, float m_x, float m_y, float m_z);
  IMPORT LI li_or_elem(LI li1, LI li2);
  IMPORT LI rotx(LI lines, float angle);
  IMPORT LI roty(LI lines, float angle);
  IMPORT LI rotz(LI lines, float angle);
  IMPORT P line_product(LI lines1, LI lines2);
  IMPORT LI fur(PTS pts, PT center, float dist);
  IMPORT LI split_lines(LI lines, float dist);
  IMPORT LI twist_y(LI lines, float y_0, float angle_per_y_unit);
  IMPORT LI random_angle(LI lines, float max_angle);
  
  IMPORT LI unit_cube(LI orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT LI unit_to_cube(LI orig, PT pos, V u_x, V u_y, V u_z);
  IMPORT LI unit_to_flex(LI orig, 
			PT bTL, PT bTR, PT bBL, PT bBR,
			PT fTL, PT fTR, PT fBL, PT fBR);
  IMPORT LI random_mesh_quad_lines(EveryApi &ev, P p, int count);
  IMPORT LI lines_from_quads(P p, int sx, int sy);

  IMPORT LLA prepare(LI l);
  IMPORT void update(LLA la, LI l);
  IMPORT ML update_ml(LLA la, LI l);
  //IMPORT int line_count(LLA l);
  //      IMPORT float *line_access(LLA lines, int line, bool b);
  //IMPORT unsigned int *color_access(LLA lines, int line, bool b);
  //IMPORT void update(LLA lines);
	IMPORT void render(LLA array);
  IMPORT void prepare_inst(LLA array, PTA instances);
  IMPORT void render_inst(LLA array, PTA instances);
  IMPORT void prepare_inst_matrix(LLA array, MSA instances);
  IMPORT void render_inst_matrix(LLA array, MSA instances);
  IMPORT ML render_ml(EveryApi &ev, LLA array, float linewidth);
  IMPORT ML render_ml2(EveryApi &ev, LI array, float linewidth);
  IMPORT ML render_inst_ml(EveryApi &ev, LLA array, PTA pta, float linewidth);
  IMPORT ML render_inst_ml2(EveryApi &ev, LI array, PTA pta, float linewidth);
  IMPORT ML render_inst_ml3(EveryApi &ev, LI array, PTS pts, float linewidth);
  IMPORT ML render_inst_ml3_matrix(EveryApi &ev, LI array, MS pts, float linewidth);
  IMPORT LI import_ifc(EveryApi &ev, std::string url);
private:
  LinesApi(const LinesApi&);
  void operator=(const LinesApi&);
  Env &e;
};
#endif		     

#ifdef F_VECTOR_API
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
#endif

#ifdef F_SPACE_VECTOR_API
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
#endif

#ifdef F_MATRIX_API
class MatrixApi
{ 
public:
	IMPORT MatrixApi(Env &e);
	IMPORT M identity();
        IMPORT M transpose(M m);
	IMPORT M xrot(float rot);
	IMPORT M yrot(float rot);
	IMPORT M zrot(float rot);
	IMPORT M trans(float x, float y, float z);
	IMPORT M trans(V vec);
	IMPORT M scale(float sx, float sy, float sz);
	IMPORT M projection(float z_min);
	IMPORT M perspective_projection(float dist);
	IMPORT M perspective(float fovy, float aspect, float near, float far);
        IMPORT M ortho( float left, float right, float bottom, float top, float near, float far);
	IMPORT M inverse(M m1);
	IMPORT M mult(M m1, M m2);
	IMPORT M rotate_around_axis(V v, float angle);
	IMPORT M rotate_around_axis(PT point, V v, float angle);
        IMPORT M keep_rotation(M m);
	IMPORT PT mult(PT point, M matrix);
private:
  MatrixApi(const MatrixApi&);
  void operator=(const MatrixApi&);
  Env &e;
};
#endif
#ifdef F_OBJECT_MOVE_API
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
#endif

#ifdef F_VBO_API
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
#endif

#ifdef F_PATH_API
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
#endif

#ifdef F_UBER_SHADER_API
class UberShaderApi
{
public:
  UberShaderApi(Env &e) : e(e) {}
  US v_empty();
  US v_edge(US us);
  US v_gltf_anim(US us);
  US v_globe(US us);
  US v_color_from_normals(US us);
  US v_recalc_normal(US us);
  US v_diffuse(US us);
  US v_ambient(US us);
  US v_specular(US us);
  US v_inst(US us);
  US v_inst_matrix(US us);
  US v_passall(US us);
  US v_pass_position(US us);
  US v_point_light(US us);
  US v_snoise(US us);
  US v_light(US us);
  US v_ref(US us);
  US v_wave(US us);
  US v_toon(US us);
  US v_texture(US us);
  US v_manytexture(US us);
  US v_cubemaptexture(US us);
  US v_texture_arr(US us);
  US v_colour(US us);
  US v_blur(US us); // dangerous operation
  US v_dist_field_mesh(US us, SFO sfo);
  US v_skeletal(US us);
  US v_custom(US us, std::string v_funcname);
  US v_phong(US us);
  US v_vertexphong(US us);
  US v_glowedge(US us);
  US v_bump_phong(US us);
  US v_fog(US us);
  US v_shadow(US us);
  US v_dyn_lights(US us);
  US v_gi(US us);
  US v_gltf(US us);
  US v_colour_with_mix(US us);
  US v_fade(US us);
  US f_edge(US us);
  US f_gltf_anim(US us);
  US f_mesh_color(US us, SFO sfo); // this requires v_pass_position() in vertex shader
  US f_sandbox(US us, SFO sfo); // this requires texture coordinates
  US f_empty(bool transparent);
  US f_diffuse(US us);
  US f_ambient(US us);
  US f_specular(US us);
  US f_phong(US us);
  US f_phong2(US us);
  US f_vertexphong(US us);
  US f_glowedge(US us);
  US f_bump_phong(US us);
  US f_fog(US us);
  US f_shadow(US us);
  US f_dyn_lights(US us);
  US f_color_from_normals(US us);
  US f_color_from_id(US us, int id); // id = [0..9]
  US f_point_light(US us);
  US f_bands(US us);
  US f_snoise(US us);
  US f_blur(US us); // dangerous operation
  US f_ref(US us);
  US f_light(US us);
  US f_toon(US us);
  US f_texture(US us);
  US f_manytexture(US us);
  US f_cubemaptexture(US us);
  US f_texture_arr(US us);
  US f_colour(US us);
  US f_mix_color(US us, US us2, float val); // TODO
  US f_choose_color(US us);
  US f_custom(US us, std::string f_funcname);
  US f_gi(US us);
  US f_colour_with_mix(US us);
  US f_gltf(US us, bool tex0, bool tex1, bool tex2, bool tex3, bool tex4, bool tex5, bool tex6, bool tex7);
  US f_fade(US us);
  US f_flip(US us, US us2);
private:
  Env &e;
};
#endif

#ifdef F_SHADER_API
class ShaderApi
{
public:
  IMPORT ShaderApi(Env &e);
  IMPORT ~ShaderApi();
  IMPORT void load_default();
  IMPORT void load(std::string filename);
  IMPORT SH get_shader(std::string v_format, std::string f_format, std::string g_format,std::string v_comb="", std::string f_comb="", bool trans=true, SFO module={-1} );
  IMPORT SH get_normal_shader(std::string v_format, std::string f_format, std::string g_format,std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod={-1}, std::string v_defines="IN_NORMAL IN_COLOR IN_TEXCOORD IN_POSITION EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD", std::string f_defines="EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD");
  IMPORT SH get_normal_shader(std::string v_format, std::string f_format, std::string g_format,US v_comb, US f_comb, bool trans=true, SFO mod={-1}, std::string v_defines="", std::string f_defines="");
  IMPORT SH get_normal_shader(std::string v_format, std::string f_format, std::string g_format, US v_comb, US f_comb, std::string v_shader, std::string f_shader, bool trans=true, SFO mod={-1}, std::string v_defines="", std::string f_defines="");
  SH get_shader_1(std::string v_format, std::string f_format, std::string g_format,
		  std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod={-1}, US v_c={-1}, US f_c={-1}, std::string v_defines="IN_NORMAL IN_COLOR IN_TEXCOORD IN_POSITION EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD EX_POSITION", std::string f_defines="EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD", std::string v_shader="", std::string f_shader="");
  SH get_normal_shader_1(std::string v_format, std::string f_format, std::string g_format,
			 std::string v_comb="", std::string f_comb="", bool trans=true, SFO mod = { -1 }, US v_c = { -1 }, US f_c = { -1 }, std::string v_defines="IN_NORMAL IN_POSITION IN_COLOR IN_TEXCOORD EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD", std::string f_defines="EX_COLOR EX_NORMAL EX_POSITION EX_TEXCOORD", std::string v_shader="", std::string f_shader="");
  IMPORT SH texture_shader();
  IMPORT SH texture_array_shader();
  IMPORT SH texture_many_shader();
  IMPORT SH colour_shader();
  IMPORT SH colour_texture_shader();
  IMPORT SH shader_choice(EveryApi &ev, int i);
  IMPORT void link(SH shader);
  IMPORT void use(SH shader);
  IMPORT void print_log(SH shader);
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
  IMPORT void set_var(GameApi::SH shader, const char * name, float val);
  IMPORT void set_var(GameApi::SH shader, const char * name, float x, float y, float z);
  IMPORT void set_var(GameApi::SH shader, const char *name, float x, float y, float z, float k);
  IMPORT void set_var(GameApi::SH shader, const char * name, int val);
  IMPORT void set_var(GameApi::SH shader, const char * name, M matrix);
  IMPORT void set_var(GameApi::SH shader, const char * name, const std::vector<M> &m, int num);
  IMPORT void set_var(GameApi::SH shader, const char * name, const std::vector<PT> &v);
  IMPORT M get_matrix_var(GameApi::SH shader, std::string name);
private:
  ShaderApi(const ShaderApi&);
  void operator=(const ShaderApi&);

  friend class StateChangeApi;
  void *priv;
  Env &e;
};
#endif

class LowFrameBufferApi
{
public:
  LowFrameBufferApi(Env &e) : e(e) { }
  RUN low_framebuffer_run(EveryApi &ev, FBU buffer, int mode, int scr_x, int scr_y);
  FBU low_framebuffer(FML mainloop, int format, int width, int height, int depth);
  FML low_sprite_draw(std::string name, BM bm, MN mn, int x, int y, int fmt, float start_time);
  FML low_poly_draw(std::string name, P p,MN mn);
  FML low_sprite_array(std::string name, std::string url, std::vector<BM> bms, MN mn, int x, int y, int fmt, float start_time);
  FML low_render_world(std::vector<BM> blocks, int screen_width, int screen_height, int fmt);
  FML low_scroll_world(FML ml, float speed_x, float speed_y, float p_x, float p_y, float left_offset, float right_offset, float height, float height2, int mode, int jump_frames);
  FML low_build_world(FML ml, std::string url, std::string chars,int x, int y);
  FML low_enemy_draw(BM bm, std::string url, int fmt, float speed);
  FML low_enemy_draw2(std::vector<BM> bm, std::string url, int fmt, float speed, int time_delta, int time_duration);
  FML low_collision(FML ml, float start_x, float end_x, float start_y, float end_y, int key);
  BM low_frame_bitmap(FML ml, int sx, int sy);
  FML low_key_bm_prepare(FML ml, BM bm, int key,FML normal, float duration, FML next);
  FML low_activate_snapshot(EveryApi &ev, FML ml, int key, MN move, float duration, FML next);
  FML qml_print(std::string url);
  FML qml_create_node(std::string url);
  FML w_root(GameApi::EveryApi &ev, GameApi::W wd);
  W w_layout(std::vector<W> vec, std::string url);
  W w_rect(unsigned int color);
  W w_bitmap(BM bm);
  W w_text(EveryApi &ev, Ft font, std::string str, int x_gap, float baseline);
private:
  LowFrameBufferApi(const LowFrameBufferApi &);
  void operator=(const LowFrameBufferApi&);
  Env &e;
};

#ifdef F_FRAMEBUFFER_API
class FrameBufferApi
{
public:
  struct vp { int viewport[4]; };
  IMPORT FrameBufferApi(Env &e) : e(e) { }
  IMPORT FBO create_fbo(int sx, int sy);
  IMPORT void config_fbo(FBO buffer);
  IMPORT vp bind_fbo(FBO buffer);
  IMPORT void bind_screen(vp viewport);
  IMPORT TXID tex_id(FBO buffer);
  IMPORT TXID depth_id(FBO buffer);
  IMPORT bool fbo_status(FBO buffer);
  IMPORT BM fbo_to_bitmap(EveryApi &ev, FBO buffer);
  IMPORT TXID fbo_ml(EveryApi &ev, ML mainloop, int sx, int sy, bool translate);
  IMPORT TXID depth_ml(EveryApi &ev, ML mainloop, int sx, int sy, bool translate);
  IMPORT ML fbo_ml_blit(EveryApi &ev, TXID id, float start_x, float end_x, float start_y, float end_y, float z);
private:
  FrameBufferApi(const FrameBufferApi &);
  void operator=(const FrameBufferApi&);
  Env &e;
};
#endif

#ifdef F_LAYOUT_API
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
#endif

#ifdef F_DRAW_API
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
#endif

  
struct EveryApi
{
	EveryApi(Env &e)
	  : mainloop_api(e), point_api(e), vector_api(e), matrix_api(e), sprite_api(e), grid_api(e), bitmap_api(e), polygon_api(e), bool_bitmap_api(e), float_bitmap_api(e), cont_bitmap_api(e),
	    font_api(e), anim_api(e), event_api(e), /*curve_api(e),*/ function_api(e), volume_api(e), float_volume_api(e), color_volume_api(e), dist_api(e), vector_volume_api(e), shader_api(e), state_change_api(e, shader_api), texture_api(e), separate_api(e), waveform_api(e),  color_api(e), lines_api(e), plane_api(e), points_api(e), voxel_api(e), fbo_api(e), sample_api(e), tracker_api(e), sh_api(e), mod_api(e), physics_api(e), ts_api(e), cutter_api(e), bool_api(e), collision_api(e), move_api(e), implicit_api(e), picking_api(e), tree_api(e), materials_api(e), uber_api(e), curve_api(e), matrices_api(e), skeletal_api(e), polygon_arr_api(e),polygon_dist_api(e), blocker_api(e), vertex_anim_api(e), newplane_api(e), surface_api(e), low_frame_api(e),

	    env(e)
  { }
  Env &get_env() { return env; }
  
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
  PhysicsApi physics_api;
  TriStripApi ts_api;
  CutterApi cutter_api;
  BooleanOps bool_api;
  CollisionPlane collision_api;
  MovementNode move_api;
  ImplicitApi implicit_api;
  PickingApi picking_api;
  TreeApi tree_api;
  MaterialsApi materials_api;
  UberShaderApi uber_api;
  CurveApi curve_api;
  MatricesApi matrices_api;
  Skeletal skeletal_api;
  PolygonArrayApi polygon_arr_api;
  PolygonDistanceField polygon_dist_api;
  BlockerApi blocker_api;
  VertexAnimApi vertex_anim_api;
  NewPlaneApi newplane_api;
  SurfaceApi surface_api;
  LowFrameBufferApi low_frame_api;
private:
  Env &env;
  EveryApi(const EveryApi&);
  void operator=(const EveryApi&);

};

#ifdef F_GAMES_API
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
#endif


  //
  // Object-Oriented API
  //


  class RenderObject
  {
  public:
    virtual ~RenderObject() { }
    virtual void prepare(bool keep=false)=0;
    virtual void render()=0;
    virtual void set_var(std::string name, float r, float g, float b, float a)=0;
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
    virtual void set_rotation_matrix2(M m)=0;
  };
  
  class ArrayObj3d : public RenderObject, public MoveScaleObject3d
  {
  public:
    ArrayObj3d(EveryApi &ev) : ev(ev), p_x(0.0), p_y(0.0), p_z(0.0), s_x(1.0), s_y(1.0), s_z(1.0) 
    {
      current_rot_matrix = ev.matrix_api.identity(); 
      current_rot_matrix2 = ev.matrix_api.identity();
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
    void prepare(bool keep=false) {
      int s = render_vec.size();
      for(int i=0;i<s;i++) {
	render_vec[i]->prepare();
      }
    }
    void render() {
      int s = render_vec.size();
      for(int i=0;i<s;i++) {
	int ss = child_var.size();
	for(int j=0;j<ss;j++)
	  {
	    ChildVar &v = child_var[j];
	    if (v.i==i)
	      render_vec[i]->set_var(v.name, v.r, v.g, v.b,v.a);
	  }
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
    M rot_y(float c_x, float c_y, float c_z, float angle)
    {
      M m1 = ev.matrix_api.trans(c_x, c_y, c_z);
      M m2 = ev.matrix_api.yrot(angle);
      M m3 = ev.matrix_api.trans(-c_x, -c_y, -c_z);
      M mm = ev.matrix_api.mult(m1,m2);
      M mmm = ev.matrix_api.mult(mm, m3);
      return mmm;
    }
    M rot_x(float c_x, float c_y, float c_z, float angle)
    {
      M m1 = ev.matrix_api.trans(c_x, c_y, c_z);
      M m2 = ev.matrix_api.xrot(angle);
      M m3 = ev.matrix_api.trans(-c_x, -c_y, -c_z);
      M mm = ev.matrix_api.mult(m1,m2);
      M mmm = ev.matrix_api.mult(mm, m3);
      return mmm;
    }
    M rot_z(float c_x, float c_y, float c_z, float angle)
    {
      M m1 = ev.matrix_api.trans(c_x, c_y, c_z);
      M m2 = ev.matrix_api.zrot(angle);
      M m3 = ev.matrix_api.trans(-c_x, -c_y, -c_z);
      M mm = ev.matrix_api.mult(m1,m2);
      M mmm = ev.matrix_api.mult(mm, m3);
      return mmm;
    }

    void set_child_rot_matrix(int i, M m)
    {
      rot_matrix[i] = m;
      setup_one(i);
    }
    struct ChildVar
    {
      int i;
      std::string name;
      float r,g,b,a;
    };

    void set_var(std::string name, float r, float g, float b, float a)
    {
      int s = render_vec.size();
      for(int i=0;i<s;i++)
	{
	  set_child_shader_var(i, name, r,g,b,a);
	}
    }

    void set_child_shader_var(int i, std::string name, float r, float g, float b, float a)
    {
      ChildVar v;
      v.i = i;
      v.name = name;
      v.r = r; v.g = g; v.b=b; v.a=a;
      child_var.push_back(v);
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
    void set_rotation_matrix2(M m)
    {
      current_rot_matrix2 = m;
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
      m = ev.matrix_api.mult(m, current_rot_matrix);
      move_scale_vec[i]->set_pos(ap_x,ap_y,ap_z);
      move_scale_vec[i]->set_scale(as_x,as_y,as_z);
      move_scale_vec[i]->set_rotation_matrix(m);
      move_scale_vec[i]->set_rotation_matrix2(current_rot_matrix2);
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
    std::vector<M> rot_matrix2;
    std::vector<ChildVar> child_var;
    float p_x, p_y, p_z;
    float s_x, s_y, s_z;
    M current_rot_matrix;
    M current_rot_matrix2;
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
    void prepare(bool keep=false) 
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
	  shader_api.set_var(sh, "in_MV", matrix_api.mult(m,matrix_api.mult(matrix_api.scale(mult_x, mult_y, 1.0), matrix_api.trans(pos_x+0.5, pos_y+0.5, 0.0))));
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
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shader_api.set_var(sh, name.c_str(), r,g,b,a);
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
    void prepare(bool keep=false) 
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
		M m2 = matrix_api.mult(matrix_api.trans(x*dx+0.5, y*dy+0.5, 0.0),m);
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
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shader_api.set_var(sh, name.c_str(), r,g,b,a);
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
    PolygonObj(EveryApi &ev, P p, SH sh) : ev(ev), api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), sh(sh) 
    {
      m_p.push_back(ev.polygon_arr_api.split_p(ev,p,500000));
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
      txa_id.id = 0;
      first = true;
    }

    PolygonObj(EveryApi &ev, std::vector<P> anim_p, SH sh) : ev(ev), api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), sh(sh) 
    {
      int s = anim_p.size();
      for(int i=0;i<s;i++)
	{
	  m_p.push_back(ev.polygon_arr_api.split_p(ev,anim_p[i],500000));
	}
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
      first = true;
    }
#if 0
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
      first = true;
    }
#endif
    ~PolygonObj() {
      int s = va_mem.size();
      for(int i=0;i<s;i++)
	{
	  ev.polygon_arr_api.delete_vertex_array(ev,va_mem[i]);
	}
    }
    void prepare(bool keep=false) 
    { 
      m_va2.clear();
      for(int i=0;i<(int)m_p.size();i++)
	{
	  Va va = ev.polygon_arr_api.create_vertex_array(ev,m_p[i], id[i].id!=0||keep);
	  va_mem.push_back(va);
	  Va va2;
#ifdef TODO
	  if (id[i].id!=0) {
	    va2 = tex.bind(va, id[i]);
	  } else {
	    if (txa_id.id!=0) {
	      va2 = tex.bind_arr(va, txa_id);
	    }
	    else
#endif
	      va2 = va;
#ifdef TODO
	  }
#endif
	  m_va2.push_back(va2);
	}
    }
    void render() {
      shapi.use(sh);
      shapi.set_var(sh, "in_MV", m); 
      shapi.set_var(sh, "in_N", normal_matrix); 
      //shapi.set_var(sh, "in_T", m2);
      shapi.set_var(sh, "in_POS", anim_time);
      ev.polygon_arr_api.render_vertex_array(ev,m_va2[anim_id]); 
      shapi.set_var(sh, "in_POS", 0.0f);
    }
    void render_instanced(PTA pta) {
      shapi.use(sh);
      shapi.set_var(sh, "in_MV", m); 
      shapi.set_var(sh, "in_N", normal_matrix); 
      //shapi.set_var(sh, "in_T", m2);
      shapi.set_var(sh, "in_POS", anim_time);
      if (first)
	{
	  ev.polygon_arr_api.prepare_vertex_array_instanced(ev,shapi, m_va2[anim_id], pta,sh);
	  first = false;
	}
      ev.polygon_arr_api.render_vertex_array_instanced(ev,shapi, m_va2[anim_id], pta, sh); 
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
    void bind_texarray(TXA id)
    {
      txa_id = id;
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
#if 0
    void explode(PT pos, float val)
    {
      api.explode(m_va2[anim_id], pos, val);
      api.update(m_va2[anim_id]);
    }
#endif
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shapi.set_var(sh, name.c_str(), r,g,b,a);
    }
  private:
    void setup_m() {
      m = mat.mult(mat.mult(mat.mult(current_rot,current_scale), current_pos), current_rot2);
      normal_matrix = mat.keep_rotation(m);
      //m2 = mat.mult(mat.mult(current_rot, current_scale), current_rot2);
    }
  private:
    EveryApi &ev;
    PolygonApi &api;
    ShaderApi &shapi;
    MatrixApi &mat;
    TextureApi &tex;
    M current_pos;
    M current_scale;
    M current_rot;
    M current_rot2;
    M m;
    M normal_matrix;
    M m2;
    std::vector<Pa> m_p;
    SH sh;
    //VA va;
    std::vector<Va> m_va2;
    std::vector<Va> va_mem;
    std::vector<TXID> id;
    TXA txa_id;
    int anim_id;
    float anim_time;
    bool first;
  };

  class WorldObj : public RenderObject, public MoveScaleObject3d
  {
  public:
    WorldObj(EveryApi &ev, std::function<P(int)> f, int numvalues, BM bm, float dx, float dy, SH sh) : ev(ev), bmapi(ev.bitmap_api), api(ev.polygon_api), shapi(ev.shader_api), mat(ev.matrix_api), tex(ev.texture_api), pts(ev.point_api), numvalues(numvalues), bm(bm), f(f), dx(dx), dy(dy), sh(sh) 
    {
      int sx = bmapi.size_x(bm);
      int sy = bmapi.size_y(bm);
      bitmap = new int[sx*sy];
      var_bitmap = new std::vector<Var>[sx*sy];
      outline_bitmap = new bool[sx*sy];
      anim_time = new float[sx*sy];
      for(int y=0;y<sy;y++)
	{
	  for(int x=0;x<sx;x++)
	    {
	      int val = bmapi.intvalue(bm, x,y);
	      bitmap[x+y*sx] = val;
	      var_bitmap[x+y*sx] = std::vector<Var>();
	      outline_bitmap[x+y*sx] = false;
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
      txa_id.id = 0;
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
    void prepare(bool keep=false) 
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
	    if (txa_id.id!=0)
	      {
		va2 = tex.bind_arr(va, txa_id);
	      }
	    else
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
    void render_outline(SH sh_new)
    {
#ifndef EMSCRIPTEN
      SH sh2 = sh;
      ev.mainloop_api.outline_first();
      render();
      sh = sh_new;
      ev.mainloop_api.outline_second();
      render();
      ev.mainloop_api.outline_first();
      render();
      sh = sh2;
      ev.mainloop_api.outline_third();
      //render();
      ev.mainloop_api.outline_disable();
#else
      render();
#endif
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
	  if (outline_bitmap[x+y*m_sx])
	    {
	      shapi.use(m_sh3);
	      shapi.set_var(m_sh3, "in_MV", t2);
	      shapi.set_var(m_sh3, "in_POS", anim_time[x+y*sx]);
	      shapi.use(sh);
	    }
	  //std::cout << x << " " << y << " " << anim_time[x+y*sx] << std::endl;
	  std::vector<Var> &v = var_bitmap[x+y*sx];
	  int s = v.size();
	  for(int i=0;i<s;i++)
	    {
	      Var &vv = v[i];
	      shapi.set_var(sh, vv.name.c_str(), vv.r, vv.g, vv.b, vv.a);
	      //std::cout << i << ":" << vv.name << " " << vv.r << " " << vv.g << " " << vv.b <<" " << vv.a << std::endl;
	      if (outline_bitmap[x+y*m_sx])
		{
		  shapi.use(m_sh3);
		  shapi.set_var(m_sh3, vv.name.c_str(), vv.r, vv.g, vv.b, vv.a);
		  shapi.use(sh);
		}

	    }

	  if (outline_bitmap[x+y*m_sx])
	    render_one_outline(x,y,m_sh3);
	    else
	      render_one(x,y);
	  //api.render_vertex_array(m_va2[bitmap[x+y*sx]]);
	  shapi.set_var(sh, "in_POS", 0.0f);
	}
      }
    }
  private:
    void render_one(int x, int y)
    {
      api.render_vertex_array(m_va2[bitmap[x+y*m_sx]]);
    }
    void render_one_outline(int x, int y, SH sh_new)
    {
#ifndef EMSCRIPTEN
      ev.mainloop_api.outline_first();
      render_one(x,y);
      ev.shader_api.use(sh_new);
      ev.mainloop_api.outline_second();
      render_one(x,y);
      ev.shader_api.use(sh);
      ev.mainloop_api.outline_third();
      ev.mainloop_api.outline_disable();
#else
      render_one(x,y);
#endif
    }
  public:
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
    void bind_texarray(TXA id)
    {
      txa_id = id;
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
    bool pick_object2(float mouse_x, float mouse_y, float radius, int scr_x, int scr_y, M in_P, int x, int y)
    {
      float pos_x = dx*x + radius/2;
      float pos_y = 0.0;
      float pos_z = dy*y + radius/2;
      BB area = ev.picking_api.pick_area(ev, mouse_x, mouse_y, radius, scr_x, scr_y);
      O vol = ev.picking_api.pick_volume(in_P, area);
      bool b = ev.picking_api.picked(vol, pos_x, pos_y, pos_z);
      return b;
    }
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shapi.set_var(sh, name.c_str(), r,g,b,a);
    }
    struct Var
    {
      std::string name;
      float r,g,b,a;
    };
    void clear_child_var(int x, int y)
    {
      var_bitmap[x+y*m_sx]=std::vector<Var>();
    }
    void set_child_var(int x, int y, std::string name, float r, float g, float b, float a)
    {
      Var v;
      v.name = name;
      v.r = r;
      v.g = g;
      v.b = b;
      v.a = a;
      var_bitmap[x+y*m_sx].push_back(v);
    }
    void set_outline(int x, int y, bool b, SH sh3)
    {
      m_sh3 = sh3;
      outline_bitmap[x+y*m_sx] = b;
    }
  private:
    void setup_m() {
      m = mat.mult(mat.mult(mat.mult(current_rot,current_scale), current_pos), current_rot2);
    }
    
  private:
    EveryApi &ev;
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
    std::vector<Var> *var_bitmap;
    bool *outline_bitmap;
    float *anim_time;
    SH sh;
    SH m_sh3;
    int m_x, m_y, m_sx, m_sy;
    //VA va;
    //std::vector<VA> m_va2;
    std::map<int, VA> m_va2;
    std::vector<TXID> id;
    TXA txa_id;
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
    void prepare(bool keep=false) {
      array = points_api.prepare(fo);
      //array = floatvolume.prepare(fo, numpoints, start_x, start_y, start_z, end_x, end_y, end_z); 
    }
    void render() {
      shapi.use(sh);
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
    void set_rotation_matrix2(M m) { }
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shapi.set_var(sh, name.c_str(), r,g,b,a);
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
    //PTS fo2;
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
    void prepare(bool keep=false) { li2 = lines.prepare(li); }
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
    void set_var(std::string name, float r, float g, float b, float a)
    {
      shapi.set_var(sh, name.c_str(), r,g,b,a);
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

  void append_url_map(const char* url, const unsigned char* data, const unsigned char *data_end);

}; // GameApi namespace

#endif
