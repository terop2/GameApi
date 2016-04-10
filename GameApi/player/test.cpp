#include "GameApi.hh"
#include <cmath>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace GameApi;

struct Envi {
  EveryApi *ev;
  ArrayObj3d *poly;
  PolygonObj *poly2;
  float time;
  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  float pos_x=0.0, pos_y=0.0, pos_z=0.0;
  float rotangle = 0.0;
  float rotspeed = 0.04;
  float speed2 = 4.0;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;
  Envi &envi = *env;

  env->time = env->ev->mainloop_api.get_time()/100.0;
    env->ev->mainloop_api.clear_3d();
    
    M m = env->poly->rot_x(-50.0, -5.0-15.0, -50.0, 1.5*cos(env->time/6.0+3.14159));
    M m3 = env->poly->rot_x(-50.0, -5.0-15.0, -50.0, 1.5*cos(env->time/6.0));
    M m2 = env->poly->rot_y(-50.0, 0.0, -50.0, env->rotangle);
    env->poly->set_rotation_matrix(m2);
    env->poly->set_child_rot_matrix(1, m);
    env->poly->set_child_rot_matrix(2, m3);
    env->poly->set_scale(2.0,-2.0,2.0);
    env->poly->set_pos(env->pos_x, env->pos_y, env->pos_z);
    env->poly->render();

    env->poly2->set_pos(env->pos_x, env->pos_y, env->pos_z);
    env->poly2->set_rotation_matrix2(m2);
    //env->poly2->set_scale(200.0,200.0,200.0);
    //env->poly2->render();
	  
    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e;
    while((e = envi.ev->mainloop_api.get_event()).last==true)
      { 

#ifndef EMSCRIPTEN
    if (e.ch==27) { exit(0); }
#endif

    if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { env->forward = true; }
    if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { env->forward = false; }
    if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { env->backward = true; }
    if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { env->backward = false; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { env->left = true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { env->left = false; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { env->right = true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { env->right = false; }
    if ((e.ch=='k'||e.ch==4||e.ch==80) && e.type==0x300) { env->up = true; }
    if ((e.ch=='k'||e.ch==4||e.ch==80) && e.type==0x301) { env->up = false; }
    if ((e.ch=='m'||e.ch==7||e.ch==79) && e.type==0x300) { env->down = true; }
    if ((e.ch=='m'||e.ch==7||e.ch==79) && e.type==0x301) { env->down = false; }

      }
    if (env->forward)
      {
	env->pos_x += envi.speed2*cos(-envi.rotangle+3.14159/2.0);
	env->pos_z += envi.speed2*sin(-envi.rotangle+3.14159/2.0);
      }
    if (env->backward)
      {
	env->pos_x -= envi.speed2*cos(-envi.rotangle+3.14159/2.0);
	env->pos_z -= envi.speed2*sin(-envi.rotangle+3.14159/2.0);
      }
    if (env->up)
      {
	env->pos_y += envi.speed2;
      }
    if (env->down)
      {
	env->pos_y -= envi.speed2;
      }
    if (env->left)
      {
	env->rotangle+=envi.rotspeed;
      }
    if (env->right)
      {
	env->rotangle-=envi.rotspeed;
      }

}

P colour(EveryApi &ev, P p1, unsigned int col)
{
  P p = p1;
  P p0 = ev.polygon_api.recalculate_normals(p);
  //P p00 = ev.polygon_api.smooth_normals(p0);
  P p2 = ev.polygon_api.color_from_normals(p0);
  P p2a = ev.polygon_api.color_grayscale(p2);
  P p3 = ev.polygon_api.color(p, col);
  //V v = ev.vector_api.vector(10.0, 10.0, 10.0);
  //P p3 = ev.polygon_api.color_lambert(p0, 0xffff8888, v);
  P p4 = ev.polygon_api.mix_color(p2a,p3,0.5);
  return p4;
}

int main() {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  //SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour", "colour");
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);



  P p3 = ev.polygon_api.rounded_cube(ev, 0.0, 100.0, 0.0, 150.0, 0.0, 100.0 , 10.0); // body
  P p3c = colour(ev,p3, 0xffffffff);
  P p4 = ev.polygon_api.rounded_cube(ev, -50.0, -5.0, 0.0, 100.0, 0.0+30, 100.0-30, 10.0); // l-arm
  P p4a = ev.polygon_api.rounded_cube(ev, -50.0, -5.0, 100.0, 130.0, 30.0, 100.0-30, 10.0);
  P p4b = ev.polygon_api.or_elem(p4, p4a);
  P p4c = colour(ev,p4b, 0xff88ff88);
  P p5 = ev.polygon_api.rounded_cube(ev, 105.0, 150.0, 0.0, 100.0, 0.0+30, 100.0-30,10.0); // r-arm
  P p5a = ev.polygon_api.rounded_cube(ev, 105.0, 150.0, 100.0, 130.0, 30.0, 100.0-30, 10.0);
  P p5b = ev.polygon_api.or_elem(p5, p5a);

  P p5c = colour(ev,p5b, 0xffff88ff);
  P p6 = ev.polygon_api.rounded_cube(ev,0.0, 45.0, 150.0, 200.0, 0.0, 100.0, 10.0); // l-leg
  P p6c = colour(ev,p6, 0xff888888);
  P p7 = ev.polygon_api.rounded_cube(ev,55.0, 100.0, 150.0, 200.0, 0.0, 100.0, 10.0); // r-leg
  P p7c = colour(ev,p7, 0xffffff88);
  PT center = ev.point_api.point(50.0, -70.0, 50.0);
  P p8 = ev.polygon_api.sphere(center, 70.0, 80/2, 80/2);
  P p8c = colour(ev,p8, 0xffff8888);


  P p3ca = ev.polygon_api.recalculate_normals(p3c);
  P p4ca = ev.polygon_api.recalculate_normals(p4c);
  P p5ca = ev.polygon_api.recalculate_normals(p5c);
  P p6ca = ev.polygon_api.recalculate_normals(p6c);
  P p7ca = ev.polygon_api.recalculate_normals(p7c);
  P p8ca = ev.polygon_api.recalculate_normals(p8c);

  P p3cn = ev.polygon_api.color_from_normals(p3ca);
  P p4cn = ev.polygon_api.color_from_normals(p4ca);
  P p5cn = ev.polygon_api.color_from_normals(p5ca);
  P p6cn = ev.polygon_api.color_from_normals(p6ca);
  P p7cn = ev.polygon_api.color_from_normals(p7ca);
  P p8cn = ev.polygon_api.color_from_normals(p8ca);

  PolygonObj poly_body(ev, p3cn, sh);
  PolygonObj poly_left_arm(ev, p4cn, sh);
  PolygonObj poly_right_arm(ev, p5cn, sh);
  PolygonObj poly_left_leg(ev, p6cn, sh);
  PolygonObj poly_right_leg(ev, p7cn, sh);
  PolygonObj poly_head(ev, p8cn, sh);
  ArrayObj3d array(ev);
  array.push_back(&poly_body, &poly_body);
  array.push_back(&poly_left_arm, &poly_left_arm);
  array.push_back(&poly_right_arm, &poly_right_arm);
  array.push_back(&poly_left_leg, &poly_left_leg);
  array.push_back(&poly_right_leg, &poly_right_leg);
  array.push_back(&poly_head, &poly_head);
  array.prepare();
  array.set_scale(1.0, -1.0, 1.0);


  //P o = ev.polygon_api.load_model("statue2.obj",1);
  PT center2 = ev.point_api.point(90.0,0.0,0.0);
  PL pl = ev.plane_api.star(center2, 50.0, 80.0, 8);
  LI li = ev.lines_api.from_plane(pl);
  LI li2 = ev.lines_api.rotz(li, 90.0/360.0*2.0*3.14159);
  P c = ev.polygon_api.circular_span(ev, li, 2.0*3.14159/360.0*10.0, 36);
  P o2 = colour(ev,c, 0xff88ff88);
  PolygonObj obj(ev, o2, sh);
  obj.prepare();


  env.ev = &ev;
  env.poly = &array;
  env.poly2 = &obj;

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
