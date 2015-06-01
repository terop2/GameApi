#include "GameApi.hh"
#include <math.h>
#include <iostream>
using namespace GameApi;

bool sections_f(float angle)
{
  float a = angle/(360.0/21);
  int aa = a;
  return aa & 0x1;
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

  std::vector<VA> vec;
  PT pos_1 = ev.point_api.point(200.0, 200.0, 0.0);
  PT pos_2 = ev.point_api.point(100.0, 100.0, 0.0);
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, 0xffffffff, 0xff000000, 256,256);
  BM bm2 = ev.bitmap_api.gradient(pos_1, pos_2, 0xffff8844, 0xff444444, 256,256);
  Ft font = ev.font_api.newfont("FreeSans.ttf", 150,150);
  BM mask0 = ev.font_api.glyph(font, 'g');
  BM mask1 = ev.bitmap_api.growbitmap(mask0, 5,5, 50, 50);
  FB mask3 = ev.float_bitmap_api.from_red(mask1);
  BM bm3 = ev.float_bitmap_api.choose_bitmap(mask3, bm, bm2);

  SpriteObj spr(ev, bm3, sh);
  spr.prepare();
  spr.set_scale(1.0,1.0);
  ev.mainloop_api.alpha(true);


  BB bb = ev.bool_bitmap_api.sections(100,100, 50.0,50.0, sections_f);
  BM bb_bm1 = ev.bool_bitmap_api.to_bitmap(bb, 255,255,255,255,
					   0,0,0,0);

  SpriteObj spr2(ev, bb_bm1, sh);
  spr2.set_pos(100.0,100.0);
  spr2.prepare();

  BM radial1 = ev.bitmap_api.conical_gradient(300,300, 150.0,150.0, 0.0,90.0, 0xffff0000, 0xff0088ff);
  BM radial2 = ev.bitmap_api.conical_gradient(300,300, 150.0,150.0, -180.0,0.0, 0xff00ffff, 0xffff0000);
  BM radial3 = ev.bitmap_api.conical_gradient(300,300, 150.0,150.0, 90.0,181.0, 0xff0088ff, 0xff00ffff);
  BM comb = ev.bitmap_api.blitbitmap(radial1, radial2, 0, 0);
  BM comb2 = ev.bitmap_api.blitbitmap(comb, radial3, 0, 0);
  SpriteObj spr3(ev, comb2, sh);
  spr3.set_pos(150.0,150.0);
  spr3.prepare();


  int frame = 0;
  float z = 0.0;
  float r = 0.0;
  float speed = 30.0;
  float rotspeed = 5.0;
  e.delete_all_data();
  while(1) {
    ev.mainloop_api.clear();
    spr.render();
    spr2.render();
    spr3.render();
    ev.mainloop_api.swapbuffers();
    ev.mainloop_api.fpscounter();
    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }
}
