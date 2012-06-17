#include "GameApi.hh"

using namespace GameApi;

void GameApiTest3(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  FloatBitmapApi &float_api = e.float_bitmap_api;

  loop.init();
  loop.alpha(false);

  Ft fnt = e.font_api.newfont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 150,150);
  //BM bm2 = font.glyph(fnt, 'Q');
  BM bm2 = e.font_api.font_string(fnt,"abcdefg", 20);
  //BM bm2 = bm.mandelbrot(false,-2.0,1.0,-1.0,1.0,0.0,0.0,50,50,255);
  e.sprite_api.preparesprite(bm2);

  BB bg = e.bool_bitmap_api.empty(1000/5.0,1000/5.0);
  BB b1 = e.bool_bitmap_api.circle(bg, 500.0/5.0,500.0/5.0,500.0/5.0);
  BB b2 = e.bool_bitmap_api.circle(b1, 0.0,0.0,100.0);
  BB bb_not = e.bool_bitmap_api.circle(bg, 500.0/5.0,500.0/5.0,400.0/5.0);
  BB bb = e.bool_bitmap_api.andnot_bitmap(b2,bb_not);
  PT points[] = { e.point_api.point(500.0/5.0,500.0/5.0,0.0),
		  e.point_api.point(200.0/5.0,100.0/5.0,0.0),
		  e.point_api.point(0.0,500.0/5.0,0.0) };
  BB bb2 = e.bool_bitmap_api.polygon(bb, points, 3);
  FB fb = float_api.from_bool_bitmap(bb2, 10/5.0,10/5.0);
  BM bx2 = float_api.to_grayscale(fb);
  //BM b2 = e.bool_bitmap_api.to_bitmap(bb, 255,255,255,255,0,0,0,0);
  e.sprite_api.preparesprite(bx2);
  while(1)
    {
      e.mainloop_api.clear();
      e.sprite_api.rendersprite(bm2,100.0,100.0,1.0,1.0);
      e.sprite_api.rendersprite(bx2,200.0,200.0,1.0,1.0);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }
}
