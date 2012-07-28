#include "GameApi.hh"

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


using namespace GameApi;

unsigned int red_block(int x, int y, void *data)
{
  return 0xff0000;
}
unsigned int green_block(int x, int y, void *data)
{
  return 0x00ff00;
}

P Cube(EveryApi &api,
       float start_x, float end_x, 
       float start_y, float end_y, 
       float start_z, float end_z, 
       unsigned int color, 
       void *data)
{
  P p = api.polygon_api.cube(start_x, end_x,
			      start_y, end_y,
			      start_z, end_z);
  P p2 = api.polygon_api.color_faces(p, 0xffffffff, 0xffffff00, 0xff00ffff, 0xffff00ff);
  
  return p2;
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
  VolumeApi &volume = e.volume_api;

  loop.init();
  loop.alpha(true);

  //BM red = bm.function(&red_block, 10,10, 0);
  //BM green = bm.function(&green_block, 10,10, 0);

  BM red = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);
  BM green = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);

  O torus = volume.torus(points.point(0.0,0.0,0.0),
			 points.point(2.0,0.0,0.0),
			 points.point(0.0,2.0,0.0), 1.0, 0.4);

  O sphere = volume.sphere(points.point(0.0, 1.0, 0.0), 1.0);
  O andnot = volume.andnot_op(torus, sphere);
  P cubes = volume.rendercubes(andnot, &Cube, 0, 40, 4.0);
  P cubes2 = e.polygon_api.memoize(cubes);
  P cubes3 = e.polygon_api.scale(cubes2, 300.0,300.0,300.0);
  //poly.prepare(cubes3);
  VA va = poly.create_vertex_array(cubes3);
  sprite.preparesprite(red);
  sprite.preparesprite(green);

  while(1)
    {
      float time = e.mainloop_api.get_time();
      e.mainloop_api.clear();
      e.mainloop_api.switch_to_3d(false);
      e.sprite_api.rendersprite(red,100.0,100.0,1.0,1.0);
      e.sprite_api.rendersprite(green,200.0,200.0,1.0,1.0);
      e.mainloop_api.switch_to_3d(true);
      glPushMatrix();
      glRotatef(time/40.0, 0.0,1.0,0.0);
      //e.polygon_api.render(cubes3, 0, 0.0,0.0,0.0);
      e.polygon_api.render_vertex_array(va);
      glPopMatrix();
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }

}
