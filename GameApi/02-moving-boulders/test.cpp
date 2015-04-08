#include "GameApi.hh"
#include <iostream>

using namespace GameApi;

char world[] = 
  "O........"
  "XOOOOOO.."
  "OXXXXXOO."
  "......XX."
  "...@....."
  ".........";

P pieces(int c, EveryApi &ev)
{
  std::cout << c << std::endl;
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
    case 3:
      {
	PT center = ev.point_api.point(50.0, 50.0, 20.0);
	P p = ev.polygon_api.sphere(center, 20.0, 30,30);
	PT pt1 = ev.point_api.point(50.0, 50.0, 20.0);
	PT pt2 = ev.point_api.point(50.0, 50.0, 100.0);
	P p2 = ev.polygon_api.cone(30, pt1, pt2, 20.0, 20.0);
	P p3 = ev.polygon_api.or_elem(p,p2);
	P p4 = ev.polygon_api.color_from_normals(p3);
	return p4;
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
  case '@': return 3;
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
  //P p3 = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev)), bm, 100.0, 100.0);
  //P p2 = ev.polygon_api.rotatex(p3, 90.0*3.14159*2.0/360.0);


  WorldObj poly(ev, std::bind(&pieces, _1, std::ref(ev)), 4, bm, 100.0, 100.0, sh);
  poly.set_pos(-500.0, 400.0, 0.0);
  poly.set_rotation_matrix(ev.matrix_api.xrot(90.0*3.14159*2.0/360.0));
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
    if (e.ch=='a') { poly.set_block(3,3,0); }
  }



}
