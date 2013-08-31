#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#include <cmath>

using namespace GameApi;

float SphereObj(EveryApi &ev, float x, float y, float z, void *data)
{
  float val = x*x+y*y+z*z;
  if (val <= 150.0*150.0 && val >= 140.0*140.0) { return 1.0; }
  if (val <= 100.0*100.0 && val >= 90.0*90.0) { return 1.0; }
  if (val <= 50.0*50.0 && val >= 40.0*40.0) { return 1.0; }
  return 0.0;
}

void GameTest6(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  FloatVolumeApi &api = e.float_volume_api;
  loop.init_3d();
  
  //BM bm_mand = e.bitmap_api.mandelbrot2(false, -2.0, 1.0, -1.0, 1.0, 0.0,0.0, 150, 150, 256);
  //BM bm_julia = e.bitmap_api.mandelbrot2(true, -1.0, 1.0, -1.0, 1.0, 0.25,0.15, 150, 150, 256);

  //FO obj = api.function(&SphereObj, &bm_mand);
  //FO obj2 = api.move(obj, 100.0,0.0,0.0);
  //FO obj3 = api.maximum(obj,obj2);
  //FOA array = api.prepare(obj, 800000, -150.0,-150.0,-150.0, 350.0,350.0,350.0);

  
  std::string filename = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";
  Ft font = e.font_api.newfont(filename.c_str(), 50,50);
  BM fontbm = e.font_api.glyph(font, 'B');
  FB fb = e.float_bitmap_api.from_red(fontbm);
  FO obj2 = api.from_float_bitmap(fb, -150.0, 150.0,
				  -150.0, 150.0,
				  -50.0, 50.0);
  
  FOA array2 = api.prepare(obj2, 80000, -150.0,-150.0,-150.0, 150.0,150.0,150.0);

  float time = 0.0;
  while(1)
    {
      e.mainloop_api.clear_3d();
      glColor4f(1.0,0.5,0.3,0.2);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
      glRotatef(time, 0.0,1.0,0.0);
      glScalef(1.0,-1.0,1.0);
      //api.render(array);
      api.render(array2);
      glPopMatrix();
      glDisable(GL_BLEND);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time+=0.1;
    }
}
