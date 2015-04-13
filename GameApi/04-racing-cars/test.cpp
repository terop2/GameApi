#include "GameApi.hh"

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
  return ev.polygon_api.cube(0.0, 300.0, 0.0, 300.0, 0.0, 300.0);
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
 
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
