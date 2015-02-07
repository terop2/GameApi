#include "GameApi.hh"
#include <math.h>
#include <iostream>
#include <pthread.h>
using namespace GameApi;



int main()
{
  Env e;
  EveryApi ev(e);

  ev.mainloop_api.init_window();

  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.get_normal_shader("colour", "colour", "");
  SH sh2 = ev.shader_api.get_normal_shader("texture", "texture", "");

  ev.shader_api.use(sh2);

  ev.mainloop_api.init(sh);
  ev.mainloop_api.init(sh2);

  ev.mainloop_api.alpha(true);
  // prepare a circle for drawing
  //BM bm = ev.bitmap_api.newbitmap(728,90);
  Ft font1 = ev.font_api.newfont("FreeSans.ttf", 30,30);
  Ft font3 = ev.font_api.newfont("FreeSans.ttf", 10,10);
  BM text = ev.font_api.font_string(font1, "GameApi   Library", 2);
  BM text2 = ev.font_api.font_string(font3, "For  beginners  writing  their  first  game",2);
  //BM text = ev.bitmap_api.newbitmap(30,30);
  //BM text2 = ev.bitmap_api.newbitmap(30,30);
  PT pos_1 = ev.point_api.point(0.0,0.0,0.0);
  PT pos_2 = ev.point_api.point(0.0, 90.0,0.0);
  BM grad = ev.bitmap_api.gradient(pos_1, pos_2, 0xff884422, 0xffffffff, 728, 90);
  BM grad2 = ev.bitmap_api.gradient(pos_1, pos_2, 0xff888888, 0xffffffff, 728,90);
  FB tx1 = ev.float_bitmap_api.from_red(text);
  FB tx2 = ev.float_bitmap_api.from_red(text2);

  BM comb1 = ev.bitmap_api.blitbitmap(grad, grad2, 40,15, tx1);
  BM comb2 = ev.bitmap_api.blitbitmap(comb1, grad2, 30, 50, tx2);
  BM comb3 = ev.bitmap_api.memoize(comb2);

  //ev.bitmap_api.savebitmap(comb3, "test.png");

  SpriteObj spr(ev, comb3, sh2);
  spr.prepare();


  //prepare(spr);
    spr.set_scale(1.0,1.0);
  ev.mainloop_api.alpha(true);

  while(1) {
    ev.mainloop_api.clear();
    spr.render();
    ev.mainloop_api.swapbuffers();
    ev.mainloop_api.fpscounter();
    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }
}
