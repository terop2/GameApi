#include "GameApi.hh"

using namespace GameApi;

char world[] =
  "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
  "O........................................O"
  "O....BBBB....BBBBB.....BBBB....BBBB......O"
  "O.....BB......BBB.......BB......BB.......O"
  "O....BBBB....BBBBB.....BBBB....BBBB......O"
  "O.....BB......BBB.......BB......BB.......O"
  "O....BBBBB...BBBBB.....BBBBB...BBBB......O"
  "O.....BB......BBB.......BB......BB.......O"
  "O........................................O"
  "O........................................O"
  "O........................................O"
  "O........................................O"
  "OWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWO";

int pieces_map(char c)
{
  switch(c) {
  case 'O': return 0; 
  case '.': return 1;

  case 'W': return 2;
  case 'B': return 3;
  };
}

P pieces(int c, EveryApi &ev)
{
  switch(c)
    {
    case 0: // 'O'
      {
	P p = ev.polygon_api.cube(0.0, 10.0, 0.0, 10.0, 0.0, 30.0);
	return p;
      }
    case 1: // '.'
      return ev.polygon_api.empty();
    case 2: // 'W'
      return ev.polygon_api.empty();
    case 3: // 'B'
      {
	P p = ev.polygon_api.cube(0.0, 10.0, 0.0, 10.0, 0.0, 30.0);
	P p2 = ev.polygon_api.color_faces(p, 0xffffffff, 0xff888888, 0xff444444, 0xff222222);
	return p2;
      }
    };
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

  BM bm = ev.bitmap_api.newintbitmap(world, 42, 13, pieces_map);
  WorldObj world(ev, std::bind(&pieces, _1, std::ref(ev)), 3, bm, 10.0, 30.0, sh);
  world.prepare();
  world.set_rotation_matrix2(ev.matrix_api.xrot(90.0*3.14159*2.0/360.0));
  world.set_scale(2.8,2.8,2.8);
  world.set_pos(-580.0, 0.0, -500.0);

  P p = ev.polygon_api.cube(0.0,200.0, 0.0, 10.0, 0.0, 10.0);
  PolygonObj club(ev, p, sh);
  club.prepare();

  float club_x = 0.0;
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    world.render();
    club.set_pos(club_x*1.2-600.0, -480.0, 0.0);
    club.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27&&e.type==0x300) break;
    PT pos = e.cursor_pos;
    club_x = ev.point_api.pt_x(pos);
  }



}
