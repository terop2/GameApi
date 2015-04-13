#include "GameApi.hh"
#include <iostream>

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
  ".........."

  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."

  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."
  ".........."

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
}

P func(int c, EveryApi &ev)
{
  switch(c)
    {
    case 0: 
      P p = ev.polygon_api.cube(0.0, 40.0, 0.0, 40.0, 0.0, 40.0);
      P p2 = ev.polygon_api.color_faces(p, 0xffffffff, 0xff888888, 0xffffffff, 0xff888888);
      return p2;
    };
  return ev.polygon_api.empty();
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

  BM bm = ev.bitmap_api.newintbitmap(world, 10, 40, mapping);
  P p = ev.polygon_api.world_from_bitmap(std::bind(func, _1, std::ref(ev)), bm, 40.0, 40.0);
  PolygonObj poly(ev, p, sh);
  
  M m = ev.matrix_api.xrot(90.0*3.14159*2.0/360.0);
  poly.set_rotation_matrix2(m);
  poly.prepare();

  float pos_x = -202.0;
  float pos_y = 336.0;
  float pos_z = -1460.0;
  float speed = 2.0;
  while(1) {
    pos_z+=speed/8.0;
    // clear frame buffer
    ev.mainloop_api.clear_3d();
    poly.set_pos(pos_x, pos_y, pos_z);
    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    if (e.ch=='a'&&e.type==0x300) pos_y-=speed;
    if (e.ch=='z'&&e.type==0x300) pos_y+=speed;
    if (e.ch=='.'&&e.type==0x300) pos_x+=speed;
    if (e.ch==','&&e.type==0x300) pos_x-=speed;
    if (e.ch=='g'&&e.type==0x300) pos_z+=speed;
    if (e.ch=='b'&&e.type==0x300) pos_z-=speed;
    if (e.ch=='p')
      {
	std::cout << pos_x << "," << pos_y << "," << pos_z << std::endl;
      }
  }



}
