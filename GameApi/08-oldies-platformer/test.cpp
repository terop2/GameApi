#include "GameApi.hh"

using namespace GameApi;

char world1[] =
  "......--......"
  ".....--......."
  "--------------"
  ".............."
  "--------------"
  ".............-";
int blocks_map(char c)
{
  switch(c) {
  case '.': return 0;
  case '-': return 1;
  };
}
P blocks(int c, EveryApi &ev)
{
  switch(c) {
  case 0: return ev.polygon_api.empty();
  case 1: 
    {
    P p = ev.polygon_api.cube(0.0,10.0,0.0,10.0, 0.0, 10.0);
    P p2 = ev.polygon_api.color_from_normals(p);
    return p2;
    }
  };
  return ev.polygon_api.empty();
}
P pieces(int c, EveryApi &ev)
{
  char *piece = 0;
  int sx=1, sy=1;
  switch(c) {
  case 0: piece=&world1[0]; sx=14; sy=6; break;
  };
  BM bm = ev.bitmap_api.newintbitmap(piece,sx,sy, &blocks_map); 
  P p = ev.polygon_api.world_from_bitmap(std::bind(&blocks, _1, std::ref(ev)),
					 bm, 10.0, 10.0);
  return p;
}
char world2[] = 
  "..."
  "..."
  "...";

int world2_map(char c)
{
  switch(c)
    {
    case '.': return 0;
    };
  return 0;
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

  BM bm = ev.bitmap_api.newintbitmap(world2, 3,3, &world2_map);
  WorldObj world(ev, std::bind(&pieces, _1, std::ref(ev)), 1, bm, 140.0, 100.0, sh);
  world.set_scale(2.7,2.7,2.7);
  world.set_pos(-630.0, 0.0, -400.0);
  world.prepare();
  M m = ev.matrix_api.xrot(90.0*2.0*3.14159/360.0);
  world.set_rotation_matrix2(m);
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    world.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
