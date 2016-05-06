
#include "GameApi.hh"

using namespace GameApi;

int main()
{
  Env e;
  EveryApi ev(e);

  PT pos_1 = ev.point_api.point(0.0, 0.0, 0.0);
  PT pos_2 = ev.point_api.point(0.0, 100.0, 0.0);
  BM grad = ev.bitmap_api.gradient(pos_1, pos_2, 0xffff8844, 0xff884422, 100, 100);
  ev.bitmap_api.savebitmap(grad, "brown_gradient.ppm");

  BB b1 = ev.bool_bitmap_api.empty(100,100);
  BB b2 = ev.bool_bitmap_api.rectangle(b1, 0,0, 100,2);
  BB b3 = ev.bool_bitmap_api.rectangle(b2, 0,0, 2, 100);
  BB b4 = ev.bool_bitmap_api.rectangle(b3, 98,0, 2,100);
  BB b5 = ev.bool_bitmap_api.rectangle(b4, 0,98, 100,2);
  BM bm = ev.bool_bitmap_api.to_bitmap(b5, 0,0,0,0,255,255,255,255);
  ev.bitmap_api.savebitmap(bm, "rectangle.ppm", false);
}
