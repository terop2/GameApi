#include "GameApi.hh"
#include <math.h>
#include <iostream>
using namespace GameApi;

unsigned int grid_colors[] =
  {
    0xffff8844,
    0xffffffff,
    0x88888888,
    0x44444444,
    0x22222222,
    0x11111111
  };
BM grid_bms(int id, int cell_x, int cell_y, EveryApi &ev)
{
  unsigned int color = grid_colors[id];
  int r = (color>>16)&0xff;
  int g = (color>>8)&0xff;
  int b = (color>>0)&0xff;
  BB em = ev.bool_bitmap_api.empty(cell_x+1, cell_y+1);
  BM em2 = ev.bool_bitmap_api.to_bitmap(em,r,g,b,255, r,g,b,255);
  return em2;
}
BM grid_content(EveryApi &ev, int sx, int sy, int cell_x, int cell_y,
		BM bm2)
{
  BM bm = ev.bitmap_api.world_from_bitmap(std::bind(grid_bms, _1, cell_x, cell_y,std::ref(ev)), bm2, cell_x+1, cell_y+1);
  return bm;
}

BM grid(EveryApi &ev, int sx, int sy, int cell_x, int cell_y)
{
  BB b1 = ev.bool_bitmap_api.empty(sx+sx*cell_x+1, sy+sy*cell_y+1);
  for(int i=0;i<sx+1;i++)
    {
      b1 = ev.bool_bitmap_api.rectangle(b1, float(i*(cell_x+1)), float(0), float(1), float(sy+sy*cell_y));
    }
  for(int j=0;j<sy+1;j++)
    {
      b1 = ev.bool_bitmap_api.rectangle(b1, float(0), float(j*(cell_y+1)), float(sx+sx*cell_x), float(1));
    }
  BM bm = ev.bool_bitmap_api.to_bitmap(b1, 255,255,255,255, 0,0,0,0);
  return bm;
}

int f(char c)
{
  return 0;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.get_normal_shader("colour", "colour", "");
  SH sh2 = ev.shader_api.get_normal_shader("texture", "texture", "");

  // rest of the initializations
  ev.mainloop_api.init(sh);
  ev.mainloop_api.init(sh2);
  

  ev.mainloop_api.alpha(true);
  // prepare a circle for drawing
  char *array = new char[20*20];
  for(int i=0;i<20*20;i++) array[i] = 0;
  BM bm2 = ev.bitmap_api.newintbitmap(array, 20,20, f);
  BM gr2 = grid_content(ev, 20,20,20,20, bm2);
  SpriteObj gr2_obj(ev, gr2, sh2);
  gr2_obj.set_pos(20,150);
  gr2_obj.prepare();

  BM gr = grid(ev, 20,20,20,20);
  SpriteObj gr_obj(ev, gr, sh2);
  gr_obj.set_pos(20,150);
  gr_obj.prepare();

  while(1) {
    ev.mainloop_api.clear();
    gr2_obj.render();
    gr_obj.render();
    ev.mainloop_api.swapbuffers();
    ev.mainloop_api.fpscounter();
    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }
}
