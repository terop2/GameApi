#include "GameApi.hh"
#include <math.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace GameApi;

struct Bullet
{
  float pos_x;
  float pos_y;
  float speed_x;
  float speed_y;
};

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
  BM fnt_bm;
  VA fnt_va;
  VA fnt_va1;
  VA bullet_va;
  SH sh;
  SH sh2;
  bool pressed;
  int framenum;
  WAV samples;
  TBUF song;
  std::vector<Bullet> bullets;
} env;

void iter()
{
  env.ev->tracker_api.play_song(*env.ev, env.song, env.samples, env.framenum, 2);
    env.ev->mainloop_api.clear();

    //env.poly->render();
    
    if (env.pressed)
      { 
	env.ev->sprite_api.render_sprite_vertex_array(env.fnt_va1);
      }
    else
      {
	env.ev->sprite_api.render_sprite_vertex_array(env.fnt_va);
      }

    int size = env.bullets.size();
    //std::cout << size << std::endl;
    for(int i=0;i<size;i++)
      {
	env.ev->shader_api.use(env.sh2);
	M m = env.ev->matrix_api.trans(env.bullets[i].pos_x, env.bullets[i].pos_y, 0.0);
	env.ev->shader_api.set_var(env.sh2, "in_MV", m);
	env.ev->sprite_api.render_sprite_vertex_array(env.bullet_va);
	env.ev->shader_api.set_var(env.sh2, "in_MV", env.ev->matrix_api.identity());
	env.bullets[i].pos_x += env.bullets[i].speed_x;
	env.bullets[i].pos_y += env.bullets[i].speed_y;
      }

    //env.ev->mainloop_api.fpscounter();
    // swapbuffers
    env.ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env.ev->mainloop_api.get_event();
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
    PT cursor_pos = e.cursor_pos;
    float pos_x = env.ev->point_api.pt_x(cursor_pos);
    float pos_y = env.ev->point_api.pt_y(cursor_pos);

    if (e.button==0 && ((pos_x-128.0)*(pos_x-128.0)+(pos_y-128.0)*(pos_y-128.0) < 128.0*128.0))
      {
	env.pressed = true;
	int rand = env.ev->mainloop_api.random();
	int rand_max = env.ev->mainloop_api.rand_max();
	float num = float(rand)/float(rand_max);
	float num2 = num*3.14159*2.0;
	float angle = num2;
	float pos_x = 128.0 + 100.0*cos(angle);
	float pos_y = 128.0 + 100.0*sin(angle);
	float speed_x = 1.0*cos(angle);
	float speed_y = 1.0*sin(angle);
	Bullet b;
	b.pos_x = pos_x;
	b.pos_y = pos_y;
	b.speed_x = speed_x;
	b.speed_y = speed_y;
	env.bullets.push_back(b);
      }
    else
      {
	env.pressed = false;
      }
    env.framenum++;
    // std::cout << e.button << std::endl;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.texture_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init(sh2);

  ev.sample_api.init_audio();
  WV w1 = ev.waveform_api.sinwave(1000.0, 1.0);
  SM s1 = ev.sample_api.empty();
  SM s2 = ev.sample_api.add(s1, w1, 1046, 0);
  SM s3 = ev.sample_api.add(s2, w1, 1108, 1);
  SM s4 = ev.sample_api.add(s3, w1, 1174, 2);
  SM s5 = ev.sample_api.add(s4, w1, 1244, 3);
  SM s6 = ev.sample_api.add(s5, w1, 1318, 4);
  SM s7 = ev.sample_api.add(s6, w1, 1396, 5);
  SM s8 = ev.sample_api.add(s7, w1, 1479, 6);
  SM s9 = ev.sample_api.add(s8, w1, 1567, 7);
  SM s10 = ev.sample_api.add(s9, w1, 1661, 8);
  SM s11 = ev.sample_api.add(s10, w1, 1760, 9);
  SM s12 = ev.sample_api.add(s11, w1, 1864, 10);
  SM s13 = ev.sample_api.add(s12, w1, 1975, 11);


  WAV ww1 = ev.sample_api.prepare(s13);

  TRK t1 = ev.tracker_api.empty(1,120);
  TRK t2 = ev.tracker_api.audio_slot(t1, 0, 0, 1, 0);
  TRK t3 = ev.tracker_api.audio_slot(t2, 0, 10, 1, 1);
  TRK t4 = ev.tracker_api.audio_slot(t3, 0, 20, 1, 2);
  TRK t5 = ev.tracker_api.audio_slot(t4, 0, 30, 1, 3);
  TRK t6 = ev.tracker_api.audio_slot(t5, 0, 40, 1, 4);
  TRK t7 = ev.tracker_api.audio_slot(t6, 0, 50, 1, 5);
  TRK t8 = ev.tracker_api.audio_slot(t7, 0, 60, 1, 6);
  TRK t9 = ev.tracker_api.audio_slot(t8, 0, 70, 1, 7);
  TRK t10 = ev.tracker_api.audio_slot(t9, 0, 80, 1, 8);
  TRK t11 = ev.tracker_api.audio_slot(t10, 0, 90, 1, 9);
  TRK t12 = ev.tracker_api.audio_slot(t11, 0, 100, 1, 10);
  TRK t13 = ev.tracker_api.audio_slot(t12, 0, 110, 1, 11);

  TBUF song = ev.tracker_api.prepare(t13);
  

  ev.sample_api.play_sample(0, ww1, 11);


  BB button_0 = ev.bool_bitmap_api.empty(256,256);
  BB button_1 = ev.bool_bitmap_api.circle(button_0, 128.0, 128.0, 128.0);
  BB button_2 = ev.bool_bitmap_api.circle(button_0, 128.0, 128.0, 100.0);
  BB button_22 = ev.bool_bitmap_api.circle(button_0, 128.0, 128.0, 90.0);
  BB button_3 = ev.bool_bitmap_api.andnot_bitmap(button_1, button_2);
  BB button_33 = ev.bool_bitmap_api.or_bitmap(button_3, button_22);
  BM button_4 = ev.bool_bitmap_api.to_bitmap(button_3, 255,255,255,255, 0,0,0,0);
  BM button_44 = ev.bool_bitmap_api.to_bitmap(button_33, 255,255,255,255, 0,0,0,0);
  BM fnt_bm = button_4;
  BM fnt_bm1 = button_44;

  BB bullet_0 = ev.bool_bitmap_api.empty(32,32);
  BB bullet_1 = ev.bool_bitmap_api.circle(bullet_0, 10.0, 10.0, 10.0);
  BM bullet_2 = ev.bool_bitmap_api.to_bitmap(bullet_1, 255,255,255,255, 0,0,0,0);

  //BM fnt_bm = ev.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 256, 256, 256);

  VA fnt_va = ev.sprite_api.create_vertex_array(fnt_bm);
  VA fnt_va1 = ev.sprite_api.create_vertex_array(fnt_bm1);
  VA bullet_va = ev.sprite_api.create_vertex_array(bullet_2);
  //ev.sprite_api.preparesprite(fnt_bm);


  ev.mainloop_api.alpha(true);

  P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
  PolygonObj poly(ev, p3, sh);
  poly.prepare();

  env.sh = sh;
  env.sh2 = sh2;
  env.ev = &ev;
  env.poly = &poly;
  env.fnt_bm = fnt_bm;
  env.fnt_va = fnt_va;
  env.fnt_va1 = fnt_va1;
  env.bullet_va = bullet_va;
  env.song = song;
  env.samples = ww1;
  env.framenum = 0;
#ifndef EMSCRIPTEN
  while(1) {
    iter();
    ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop(iter, 60,1);
#endif



}
