#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
  float pos_x=-0.124, pos_y=755.0;
  float rot_y=-5.58;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  InteractionApi::Quake_data data;
  bool logo_shown = true;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; }
      return;
    }

    env->ev->mainloop_api.clear_3d(0xff000000);

    M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(0.0,0.0,400.0);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,-400.0);
    M a_mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(a_m3,a_m),a_m2);

    env->poly->set_rotation_matrix2(a_mm);
    env->poly->set_pos(env->pos_x, 0.0, env->pos_y);
    env->poly->render();

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

	InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
      }

	InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);


#if 0
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { env->pos_y+=env->speed_y; env->pos_x+=env->speed_x; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) { env->pos_y-=env->speed_y; env->pos_x-=env->speed_x; }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env->rot_y -= env->rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env->rot_y += env->rot_speed; }
    env->speed_x = env->speed*cos(env->rot_y+3.14159/2.0);
    env->speed_y = env->speed*sin(env->rot_y+3.14159/2.0);
#endif

    //    std::cout << env->pos_y << " " << env->pos_x << " " << env->rot_y << std::endl;

}

int main(int argc, char *argv[]) {
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

  if (argc==2 && std::string(argv[1])=="--generate-logo")
    {
      std::cout << "Generating Logo" << std::endl;
      ev.mainloop_api.save_logo(ev);
      exit(0);
    }

  //P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);

  PT center1 = ev.point_api.point(0.0,0.0,0.0);
  PT center2 = ev.point_api.point(30.0,0.0,50.0);
  //P p1 = ev.polygon_api.sphere(center1, 50.0, 30*2*2, 15*2*2);
  float cs = 40.0;

  BO p1 = ev.bool_api.cube(ev, -cs,cs, -cs,cs, -cs,cs, 8, 8);
  BO p2 = ev.bool_api.sphere(ev, center1, 50.0, 30*2*2, 30*2*2);
  BO p3bo = ev.bool_api.and_not(ev, p1, p2);
  BO p22 = ev.bool_api.sphere(ev, center2, 40.0, 30*2, 30*2);
  BO b22bo = ev.bool_api.and_not(ev, p3bo, p22);
  P p3 = ev.bool_api.to_polygon(b22bo);
  //O p3o = ev.bool_api.to_volume(b22bo);
  //P p3 = ev.volume_api.rendercubes3(p3o, 80, 80, 80,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0);

  P p31 = ev.polygon_api.recalculate_normals(p3);
  P p32 = ev.polygon_api.color_from_normals(p31);

  PolygonObj poly(ev, p32, sh);
  poly.prepare();

  env.ev = &ev;
  env.poly = &poly;

  ev.mainloop_api.reset_time();
  ev.mainloop_api.display_logo(ev);

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
