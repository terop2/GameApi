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
  SH sh = ev.shader_api.get_normal_shader("texture", "texture", "");

  // rest of the initializations
  ev.mainloop_api.init(sh);
  
  // prepare a circle for drawing

  std::vector<VA> vec;
  PT pos_1 = ev.point_api.point(200.0, 200.0, 0.0);
  PT pos_2 = ev.point_api.point(200.0, 0.0, 0.0);
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, 0xffffffff, 0xff000000, 256,256);
  BM bm2 = ev.bitmap_api.gradient(pos_1, pos_2, 0xffff8844, 0xff444444, 256,256);
  Ft font = ev.font_api.newfont("FreeSans.ttf", 150,150);
  BM mask0 = ev.font_api.glyph(font, 'g');
  BM mask1 = ev.bitmap_api.growbitmap(mask0, 5,5, 50, 50);
  FB mask3 = ev.float_bitmap_api.from_red(mask1);
  BM bm3 = ev.float_bitmap_api.choose_bitmap(mask3, bm, bm2);
  //VA va = ev.sprite_api.create_vertex_array(bm3);
  SpriteObj spr(ev, bm3, sh);
  spr.prepare();
  spr.set_scale(1.0,1.0);
  ev.mainloop_api.alpha(true);
  int frame = 0;
  while(1) {
    ev.mainloop_api.clear();

    spr.render();

    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==' ') frame++;
    if (e.ch==27) break;
  }
}
