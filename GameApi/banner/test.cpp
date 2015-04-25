#include "GameApi.hh"
#include <math.h>
#include <iostream>
#include <pthread.h>
using namespace GameApi;

unsigned int color_1(int pointnum, PT pos)
{
  return 0xff000000;
}

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
  BM text2 = ev.font_api.font_string(font3, "Empowering beginners to create games",2);
  //BM text = ev.bitmap_api.newbitmap(30,30);
  //BM text2 = ev.bitmap_api.newbitmap(30,30);
  PT pos_1 = ev.point_api.point(0.0,0.0,0.0);
  PT pos_2 = ev.point_api.point(0.0, 90.0,0.0);
  BM grad = ev.bitmap_api.gradient(pos_1, pos_2, 0xff884422, 0xffffffff, 728, 90);
  BM grad2 = ev.bitmap_api.gradient(pos_1, pos_2, 0xff444444, 0xff222222, 728,90);
  FB tx1 = ev.float_bitmap_api.from_red(text);
  FB tx2 = ev.float_bitmap_api.from_red(text2);

  BM comb1 = ev.bitmap_api.blitbitmap(grad, grad2, 40,15, tx1);
  BM comb2 = ev.bitmap_api.blitbitmap(comb1, grad2, 50, 50, tx2);
  BM comb3 = ev.bitmap_api.memoize(comb2);

  //ev.bitmap_api.savebitmap(comb3, "test.png");

  BB bg = ev.bool_bitmap_api.empty(200,200);
  BB c1 = ev.bool_bitmap_api.circle(bg, 100,100, 100);
  BB c2 = ev.bool_bitmap_api.circle(bg, 100,100, 80);
  BB cc = ev.bool_bitmap_api.andnot_bitmap(c1, c2);
  BB c1a = ev.bool_bitmap_api.circle(bg, 100,100, 60);
  BB c2a = ev.bool_bitmap_api.circle(bg, 100,100, 40);
  BB cca = ev.bool_bitmap_api.andnot_bitmap(c1a, c2a);
  BB c3a = ev.bool_bitmap_api.circle(bg, 100,100, 20);
  BB or_1 = ev.bool_bitmap_api.or_bitmap(cc, cca);
  BB or_2 = ev.bool_bitmap_api.or_bitmap(or_1, c3a);


  BM cbm = ev.bool_bitmap_api.to_bitmap(or_2, 255,128,20,255,
					0, 0, 0, 255);
  BM comb4 = ev.bitmap_api.blitbitmap(comb3, cbm, 450,-50, or_2);


  ev.bitmap_api.savebitmap(comb4, "banner1.png");


  PT pos_1a = ev.point_api.point(0.0,0.0,0.0);
  PT pos_2a = ev.point_api.point(0.0, 300.0,0.0); 
  BM grada = ev.bitmap_api.gradient(pos_1a, pos_2a, 0xff224488, 0xffffffff, 300, 300);
  BM gradc = ev.bitmap_api.gradient(pos_1a, pos_2a, 0xffff8810, 0xff222222, 300, 300);
  Ft font2 = ev.font_api.newfont("FreeSans.ttf", 40,40);
  BM texta = ev.font_api.font_string(font2, "GameApi", 2);
  FB txt2 = ev.float_bitmap_api.from_red(texta);
  BM comb6 = ev.bitmap_api.blitbitmap(grada, gradc, 50,90, or_2);
  BM comb5 = ev.bitmap_api.blitbitmap(comb6, texta, 40,20, txt2);


  ev.bitmap_api.savebitmap(grada, "banner2.png");

  SpriteObj spr2(ev, comb5, sh2);
  spr2.prepare();
  spr2.set_scale(1.0,1.0);
  spr2.set_pos(150.0,150.0);

  SpriteObj spr(ev, comb4, sh2);
  spr.prepare();
  spr.set_scale(1.0,1.0);
  ev.mainloop_api.alpha(true);



  //BM bm_t = ev.bitmap_api.newbitmap(284,198, 0xff888888);
  PT p_1 = ev.point_api.point(100.0,0.0,0.0);
  PT p_2 = ev.point_api.point(100.0,198.0,0.0);
  BM bm_t = ev.bitmap_api.gradient(p_1,p_2, 0xff888888, 0xffffffff, 284,198);
  BM grad2_t = ev.bitmap_api.gradient(pos_1, pos_2, 0xff444444, 0xff222222, 284,198);

  BM text_t1 = ev.font_api.font_string(font1, "100", 2);
  BM text_t2 = ev.font_api.font_string(font1, "MiniGame", 2);
  BM text_t3 = ev.font_api.font_string(font1, "Project", 2);
  FB tx1_t1 = ev.float_bitmap_api.from_red(text_t1);
  FB tx1_t2 = ev.float_bitmap_api.from_red(text_t2);
  FB tx1_t3 = ev.float_bitmap_api.from_red(text_t3);
  BM comb1_t0 = ev.bitmap_api.blitbitmap(bm_t, cbm, 150,0,or_2);
  BM comb1_t1 = ev.bitmap_api.blitbitmap(comb1_t0, grad2_t, 40,15+30, tx1_t1);
  BM comb1_t2 = ev.bitmap_api.blitbitmap(comb1_t1, grad2_t, 40,60+30, tx1_t2);
  BM comb1_t3 = ev.bitmap_api.blitbitmap(comb1_t2, grad2_t, 40,120-15+30, tx1_t3);
  BM bm_t2 = ev.bitmap_api.gradient(p_1,p_2,0xff888888, 0xffffffff, 100,100);
  BM tx2_t = ev.bitmap_api.blitbitmap(bm_t2, grad2_t, 20,15+30, tx1_t1);
  
  ev.bitmap_api.savebitmap(comb1_t3, "AllGames.png");
  ev.bitmap_api.savebitmap(tx2_t, "AllGames2.png");

  SpriteObj spr_t(ev, tx2_t, sh2);
  spr_t.set_pos(500,150);
  spr_t.prepare();
  
  


  while(1) {
    ev.mainloop_api.clear();
    spr.render();
    spr2.render();
    spr_t.render();
    ev.mainloop_api.swapbuffers();
    ev.mainloop_api.fpscounter();
    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }
}
