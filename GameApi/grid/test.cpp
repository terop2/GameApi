#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

unsigned int f(EveryApi &ev, int x, int y, int z, void* data)
{
  x-=50;
  y-=50;
  z-=50;
  bool b = x*x+y*y+z*z < 50*50;
  if (b) { return 1; } else { return 0; }
  return 0;
}
P g(unsigned int c, EveryApi &ev)
{
  switch(c)
    {
    case 1:
      {
	P p = ev.polygon_api.cube(-2.5, 2.5, -2.5, 2.5, -2.5, 2.5);
	// P p2 = ev.polygon_api.scale(p, 20.0, 20.0, 20.0);
      return p;
	}
    case 0:
      return ev.polygon_api.empty();
    };
  return ev.polygon_api.empty();
}

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
  float pos_x=0.0, pos_y=0.0;
  float speed_x = 10.0;
  float speed_y = 10.0;
  float rot_y = 0.0;
  float rot_speed = 1.0*2.0*3.14159/360.0;
  float speed = 10.0;

  
  //PTS pos;
  PTA pta;
  //float *pos;
  // int pos_size;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d();

    M m = env->ev->matrix_api.yrot(env->rot_y);
    M m2 = env->ev->matrix_api.trans(0.0,0.0,400.0);
    M m3 = env->ev->matrix_api.trans(0.0,0.0,-400.0);
    M mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(m3,m),m2);
    env->poly->set_rotation_matrix2(mm);
    env->poly->set_pos(env->pos_x, 0.0, env->pos_y);

    //float arr[] = { 0.0, 0.0, 0.0, 100.0, 0.0, 0.0 };

    env->ev->points_api.explode(env->pta, 20.0, 0.0, 0.0, 1.0);
    env->poly->render_instanced(env->pta);

    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { env->pos_y+=env->speed_y; env->pos_x+=env->speed_x; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) { env->pos_y-=env->speed_y; env->pos_x-=env->speed_x; }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env->rot_y -= env->rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env->rot_y += env->rot_speed; }
      }
    env->speed_x = env->speed*cos(env->rot_y+3.14159/2.0);
    env->speed_y = env->speed*sin(env->rot_y+3.14159/2.0);
}

int main() {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  //SH sh = ev.shader_api.colour_shader();
  SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "inst:colour", "colour");

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);

  //VX vx = ev.voxel_api.function(&f, 60, 60, 60, 0);
  PT center = ev.point_api.point(0.0,0.0,0.0);
  O o = ev.volume_api.sphere(center, 1.0);

  PTS pos = ev.volume_api.instanced_positions(o, 100, 100.0);
  PTA pta = ev.points_api.prepare(pos);
  P p3 = g(1,ev);
  //P p3 = ev.voxel_api.render_boxes(vx, 1.0, 1.0, 1.0);
  //P p3a = ev.polygon_api.scale(p3, 5.0,5.0,5.0);
  P p3b = ev.polygon_api.color_from_normals(p3);


  ev.polygon_api.print_stat(p3b);
  //P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
  PolygonObj poly(ev, p3b, sh);
  poly.prepare();

  env.ev = &ev;
  env.poly = &poly;
  env.pta = pta;
  e.free_memory();
#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
