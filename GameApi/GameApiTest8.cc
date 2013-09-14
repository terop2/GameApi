#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#include <cmath>

using namespace GameApi;

void GameTest8(EveryApi &e)
{
  e.mainloop_api.init_3d();
  

  P cube = //e.polygon_api.quad_z(-100.0,100.0, -100.0,100.0, 0.0);
    e.polygon_api.cube(-100.0,100.0, -100.0,100.0, -100.0,100.0);
    //e.polygon_api.sphere(e.point_api.point(0.0,0.0,0.0), 100.0, 100, 100);
  LI lines = e.lines_api.from_polygon(cube);
  LLA array = e.lines_api.prepare(lines);
  float time = 0.0;
  glLineWidth(0.02);

  BB bg = e.bool_bitmap_api.empty(150,150);
  BB b = e.bool_bitmap_api.circle(bg, 50.0,50.0, 50.0);
  BB b2 = e.bool_bitmap_api.circle(bg, 80.0,50.0, 40.0);
  BB b3 = e.bool_bitmap_api.andnot_bitmap(b,b2);
  LI l = e.lines_api.border_from_bool_bitmap(b3, -100.0, 100.0, -100.0, 100.0, 0.0);
  LLA array2 = e.lines_api.prepare(l);

  while(1)
    {
      e.mainloop_api.clear_3d();
      glPushMatrix();
      glRotatef(time, 0.0,1.0,0.0);
      e.lines_api.render(array);
      e.lines_api.render(array2);
      glPopMatrix();
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time += 0.1;
    }
}
