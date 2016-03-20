#include "GameApi.hh"
#include <math.h>

using namespace GameApi;




int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "inst:colour", "colour");

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);

  PT pos = ev.point_api.point(0.0, 0.0, 0.0);
  V u_x = ev.vector_api.vector(4000.0, 0.0, 0.0);
  V u_y = ev.vector_api.vector(0.0, 0.0, 4000.0);
  PTS pts = ev.points_api.random_plane(pos, u_x, u_y, 141072);
  PTA instances = ev.points_api.prepare(pts);

  TS tris = ev.ts_api.load("..\\objed\\test2.tri");
  P p2 = ev.ts_api.to_poly(tris);
  P p3 = ev.polygon_api.translate(p2, 0.0, -400.0, 0.0);
  P p4 = ev.polygon_api.color_faces(p3, 0xff00ff00, 0xff008800, 0xff004400, 0xff002200);
  P p5 = ev.polygon_api.scale(p4, 0.1, -0.1, 0.1);

  P p52 = ev.polygon_api.rotatey(p5, 90*3.14159/2.0/360.0);

  PolygonObj poly(ev, p5, sh);
  poly.prepare();
 
  poly.set_pos(0.0,-80.0,0.0);
  poly.set_scale(1.0,1.0,1.0);

  PolygonObj poly2(ev, p52, sh);
  poly2.prepare();
 
  poly2.set_pos(0.0,-80.0,0.0);
  poly2.set_scale(1.0,1.0,1.0);

  float pos_x=0.0, pos_y=0.0;
  float speed_y = 0.0;
  float speed_x = 0.0;
  float speed = 100.0;
  float rot_y = 0.0;
  float rot_speed = 8.0*3.14159*2.0/360.0;
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    M m = ev.matrix_api.yrot(rot_y);
    M m2 = ev.matrix_api.trans(0.0,0.0,400.0);
    M m3 = ev.matrix_api.trans(0.0,0.0,-400.0);
    M mm = ev.matrix_api.mult(ev.matrix_api.mult(m3,m),m2);

    poly.set_rotation_matrix2(mm);
    poly.set_pos(pos_x, -80.0, pos_y);
    poly.render_instanced(instances);

    poly2.set_rotation_matrix2(mm);
    poly2.set_pos(pos_x, -80.0, pos_y);
    poly2.render_instanced(instances);

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { pos_y+=speed_y; pos_x+=speed_x; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) { pos_y-=speed_y; pos_x-=speed_x; }
    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { rot_y -= rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { rot_y += rot_speed; }
    speed_x = speed*cos(rot_y+3.14159/2.0);
    speed_y = speed*sin(rot_y+3.14159/2.0);

  }



}
