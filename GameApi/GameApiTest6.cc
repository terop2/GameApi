
#include "GameApi.hh"
#include <math.h>

using namespace GameApi;

float u_func(float x, float y, float z, void *data)
{
  return acos(z);
}
float v_func(float x, float y, float z, void *data)
{
  return atan2f(y,x);
}
float surf_func(float x, float y, float z, void *data)
{
  return sqrt(x*x+y*y+z*z);
}


void GameTest6(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  PointApi &points = e.point_api;
  VectorApi &vec = e.vector_api;
  BitmapApi &bm = e.bitmap_api;
  BoolBitmapApi &bool_bm = e.bool_bitmap_api;
  FloatBitmapApi &float_bm = e.float_bitmap_api;
  PolygonApi &poly = e.polygon_api;
  SpriteApi &sprite = e.sprite_api;
  VolumeApi &volume = e.volume_api;
  SeparateApi &sep = e.separate_api;

  BB empty = bool_bm.empty(100,30);
  BB rect = bool_bm.rectangle(empty, 0.0f,0.0f, 100.0f,30.0f);
  BM bm2 = bool_bm.to_bitmap(rect, 255,255,255,255, 0,255,0,0);

  SA ee = sep.empty();
  SA ex = sep.u_sep(ee, u_func, 0);
  SA ey = sep.v_sep(ex, v_func, 0);
  SA esurf = sep.surf_sep(ey, surf_func, 0);
  P p = sep.create_quads(esurf, 50.0, 2.5, 
			 -100.0, 100.0,1.0,
			 -100.0, 100.0,1.0,
			 -100.0, 100.0,1.0,
			 0.0, 3.14159*2.0,0.018,
			 -3.14159, 3.14149,0.018);
  //VA array = poly.create_vertex_array(p);
  poly.preparepoly(p);

  loop.init_3d();
  //loop.alpha(true);
    
  sprite.preparesprite(bm2);
  
  while(1)
    {
      e.mainloop_api.clear_3d();
      //sprite.rendersprite(bm2, 300.0,400.0,0.0,0.0);
      //poly.render_vertex_array(array);
      poly.renderpoly(p,100.0,100.0,100.0);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;

    }
}
