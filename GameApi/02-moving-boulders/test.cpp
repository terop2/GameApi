#include "GameApi.hh"

using namespace GameApi;

char world[] = 
  "O........"
  "X.....O.."
  "O.....OO."
  "......XX."
  "........."
  ".........";

P pieces(int c, EveryApi &ev)
{
  switch(c)
    {
    case 0:
      {
      PT center = ev.point_api.point(50.0,50.0,50.0);
      P p = ev.polygon_api.sphere(center, 50.0, 50, 50); 
      P p2 = ev.polygon_api.color_from_normals(p);
     return p2;
      }
    case 1:
      {
      P p = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
      P p2 = ev.polygon_api.color_from_normals(p);
      return p2;
      }
    };
  return ev.polygon_api.empty();
}

int pieces_2(char c)
{
  switch(c) {
  case 'O': return 0;
  case 'X': return 1;
  case '.': return 2;
  }
  return 1;
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

  
  BM bm = ev.bitmap_api.newintbitmap(world, 9, 6, pieces_2);
  P p3 = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev)), bm, 100.0, 100.0);
  P p2 = ev.polygon_api.rotatex(p3, 90.0*3.14159*2.0/360.0);


  PolygonObj poly(ev, p2, sh);
  poly.set_pos(-500.0, 400.0, 0.0);
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
