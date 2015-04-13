#include "GameApi.hh"
#include <math.h>

using namespace GameApi;

char world[] = 
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  "..........";

int mapping(char c)
{
  switch(c) {
  case '.': return 0;
  };
  return 0;
}

P pieces(int c, EveryApi &ev)
{
  P p = ev.polygon_api.cube(0.0, 300.0, 0.0, 300.0, 0.0, 300.0);
  P p2 = ev.polygon_api.color_faces(p, 0xff884422, 0xff888888, 0xff884422, 0xff888888);
  return p2;
}

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

  BM bm = ev.bitmap_api.newintbitmap(world, 10, 10, mapping);
  P p = ev.polygon_api.world_from_bitmap(std::bind(pieces,_1,std::ref(ev)), bm, 300.0, 300.0);

  PolygonObj poly(ev, p, sh);
  poly.prepare();


  P player = ev.polygon_api.cube(-60.0, 60.0, -40.0, 40.0, -40.0, 40.0);

  P player_2 = ev.polygon_api.cube(65.0, 60.0, -30.0, -20.0, -40.0, 40.0);


  P player_3 = ev.polygon_api.cube(65.0, 60.0, 30.0, 20.0, -40.0, 40.0);

  P pp = ev.polygon_api.or_elem(player, player_2);
  P ppp = ev.polygon_api.or_elem(pp, player_3);
  PolygonObj player_obj(ev, ppp, sh);

  player_obj.prepare();

  float angle=0.0;
  float speed = 3.14159*2.0/360.0;
  float pos_x =0.0;
  float pos_y =0.0;
  float travel_speed = 1.0;
  while(1) {
    pos_x += cos(angle)*travel_speed;
    pos_y += sin(angle)*travel_speed;

    // clear frame buffer
    ev.mainloop_api.clear_3d();
    poly.set_pos(-pos_x, 0.0, -pos_y);
    poly.set_rotation_matrix2(ev.matrix_api.xrot(90.0*3.14159*2.0/360.0));
    poly.render();
    player_obj.set_pos(0.0, 0.0, 300.0);
    player_obj.set_rotation_matrix2(ev.matrix_api.zrot(-angle));
    player_obj.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    if (e.ch==',') angle+=speed;
    if (e.ch=='.') angle-=speed;
  }



}
