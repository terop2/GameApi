#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d();

    M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(0.0,0.0,400.0);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,-400.0);
    M a_mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(a_m3,a_m),a_m2);

    env->poly->set_rotation_matrix2(a_mm);
    env->poly->set_pos(env->pos_x, 0.0, env->pos_y-400.0);
    env->poly->render();

    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env->ev->mainloop_api.get_event();
#ifndef EMSCRIPTEN
    if (e.ch==27) { exit(0); }
#endif
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { env->pos_y+=env->speed_y; env->pos_x+=env->speed_x; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) { env->pos_y-=env->speed_y; env->pos_x-=env->speed_x; }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env->rot_y -= env->rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env->rot_y += env->rot_speed; }
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
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);



  //P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);

  PT center1 = ev.point_api.point(0.0,0.0,0.0);
  PT center2 = ev.point_api.point(30.0,0.0,50.0);
  P p1 = ev.polygon_api.sphere(center1, 50.0, 30*2, 15*2);
  P p2 = ev.polygon_api.sphere(center2, 40.0, 30*2, 15*2);
  O o1 = ev.volume_api.sphere(center1, 50.0);
  O o2 = ev.volume_api.sphere(center2, 40.0);
  FD fd1 = ev.dist_api.sphere(center1, 50.0);
  FD fd2 = ev.dist_api.sphere(center2, 40.0);
  CT ct1 = ev.cutter_api.distance_cut(fd1);
  CT ct2 = ev.cutter_api.distance_cut(fd2);
  P p3 = ev.polygon_api.and_not_elem(ev, p1,p2, o1, o2, ct1, ct2);

  P p31 = ev.polygon_api.recalculate_normals(p3);
  P p32 = ev.polygon_api.color_from_normals(p31);

  PolygonObj poly(ev, p32, sh);
  poly.prepare();

  env.ev = &ev;
  env.poly = &poly;

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
