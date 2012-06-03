#include "GameApi.hh"

using namespace GameApi;

unsigned int red_block(int x, int y, void *data)
{
  return 0xff0000;
}
unsigned int green_block(int x, int y, void *data)
{
  return 0x00ff00;
}

void Game(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  PointApi &points = e.point_api;
  VectorApi &vec = e.vector_api;
  BitmapApi &bm = e.bitmap_api;
  BoolBitmapApi &bool_bm = e.bool_bitmap_api;
  FloatBitmapApi &float_bm = e.float_bitmap_api;
  PolygonApi &poly = e.polygon_api;
  SpriteApi &sprite = e.sprite_api;


  loop.init();
  loop.alpha(true);

  //BM red = bm.function(&red_block, 10,10, 0);
  //BM green = bm.function(&green_block, 10,10, 0);

  BM red = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,10,128);
  BM green = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,10,128);

  sprite.preparesprite(red);
  sprite.preparesprite(green);

  while(1)
    {
      e.mainloop_api.clear();
      e.sprite_api.rendersprite(red,100.0,100.0);
      e.sprite_api.rendersprite(green,200.0,200.0);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }

}
