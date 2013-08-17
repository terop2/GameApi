#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "GameApi.hh"
#include <cmath>

using namespace GameApi;

float SphereObj(EveryApi &ev, float x, float y, float z, void *data)
{
  if (x*x+y*y+z*z<100.0*100.0)
    {
      return 1.0;
    }
  if (x*x+y*y+z*z<150.0*150.0)
    {
      return 0.2;
    }
  else
    return 0.0;
}

void GameTest6(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  FloatVolumeApi &api = e.float_volume_api;
  loop.init_3d();
  
  FO obj = api.function(&SphereObj, 0);
  FOA array = api.prepare(obj, 300000, -150.0,-150.0,-150.0, 150.0,150.0,150.0);
  float time;
  while(1)
    {
      e.mainloop_api.clear_3d();
      glColor4f(1.0,0.5,0.3,0.2);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
      glRotatef(time, 0.0,1.0,0.0);
      api.render(array);
      glPopMatrix();
      glDisable(GL_BLEND);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time+=0.1;
    }
}
