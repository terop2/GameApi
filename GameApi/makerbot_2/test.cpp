#include "GameApi.hh"
#include <math.h>

using namespace GameApi;

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);


  PT center1 = ev.point_api.point(-3.8, 0.0, 0.0);
  PT center2 = ev.point_api.point(3.8, 0.0, 0.0);
  O o1 = ev.volume_api.sphere(center1, 2.0);
  O o2 = ev.volume_api.sphere(center2, 2.0);
  O o3 = ev.volume_api.cone(center1, center2, 0.5, 0.5);


  O oo = ev.volume_api.max_op(o1,o2);
  O ooo = ev.volume_api.max_op(oo,o3);

  P p3 = ev.volume_api.rendercubes3(ooo, 200, 200, 200,
				     -10.0, 10.0,
				     -10.0, 10.0,
				     -10.0, 10.0);
  P p3a = ev.polygon_api.scale(p3,2.0,2.0,2.0);
  P p3aa = ev.polygon_api.translate(p3a, -400.0, 0.0, 0.0);
  P p3b = ev.polygon_api.scale(p3a, 30.0, 30.0, 30.0);
  //P p3b = ev.polygon_api.color_faces(p3aa, 0xffffffff, 0x88888888, 0x44444444, 0x22222222);
  ev.polygon_api.save_model(p3b, "weight.obj");

  PolygonObj poly(ev, p3b, sh);
  poly.prepare();
  float frame=0.0;
  float speed_x=0.0;
  float speed_y=0.0;
  float pos_x=0.0;
  float pos_y=0.0;
  float rot_y = 0.0;
  float rot_speed = 8.0*3.14159*2.0/360.0;
  float speed = 100.0;
  while(1) {
    frame+=0.01;
    // clear frame buffer
    ev.mainloop_api.clear_3d();
    //poly.set_rotation_y(frame);
    M m = ev.matrix_api.yrot(rot_y);
    M m2 = ev.matrix_api.trans(0.0,0.0,400.0);
    M m3 = ev.matrix_api.trans(0.0,0.0,-400.0);
    M mm = ev.matrix_api.mult(ev.matrix_api.mult(m3,m),m2);
    poly.set_rotation_matrix2(mm);

    poly.set_pos(pos_x, 0.0, pos_y);
    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    if (e.ch=='a') { pos_y+=speed_y; pos_x+=speed_x; }
    if (e.ch=='z') { pos_y-=speed_y; pos_x-=speed_x; }
    if (e.ch==',') { rot_y -= rot_speed; }
    if (e.ch=='.') { rot_y += rot_speed; }
    speed_x = speed*cos(rot_y+3.14159/2.0);
    speed_y = speed*sin(rot_y+3.14159/2.0);

  }



}
