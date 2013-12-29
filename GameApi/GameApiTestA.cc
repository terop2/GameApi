
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#include <cmath>
using namespace GameApi;

unsigned int f(EveryApi &ev, float x, float y, float z, void *data)
{
  x-=100.0;
  y-=100.0;
  z-=100.0;
  x/=200.0;
  y/=200.0;
  z/=200.0;
  x*=255.0;
  y*=255.0;
  z*=255.0;
  int xx = x;
  int yy = y;
  int zz = z;
  return (xx<<16)+(yy<<8)+(zz)+0xff000000;
}

void GameTestA(EveryApi &e)
{
  e.mainloop_api.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.shader_api.bind_attrib(sh, 0, "in_Position");
  e.shader_api.bind_attrib(sh, 1, "in_Normal");
  e.shader_api.bind_attrib(sh, 2, "in_Color");
  e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.link(sh);
  e.shader_api.use(sh);
  e.mainloop_api.init(sh );
  
  PT center = e.point_api.point(0.0,0.0,0.0);
  PT pos = e.point_api.point(-100.0,-100.0, -300.0);
  V u_x = e.vector_api.vector(640.0, 0.0, 0.0);
  V u_y = e.vector_api.vector(0.0, 480.0, 0.0);
  V u_z = e.vector_api.vector(0.0,0.0, 200.0);
  FD fd = e.dist_api.sphere(center, 100.0);
  FD fd2 = e.dist_api.line(pos,center, 30.0);
  FD fd12 = e.dist_api.min(fd, fd2);
  COV cov = e.color_volume_api.function(f, 0);
  BM bm = e.dist_api.render(fd12, cov, pos, u_x, u_y, u_z, 640, 480);

  e.sprite_api.preparesprite(bm);
  while(1) 
    {
      e.mainloop_api.clear();
      e.sprite_api.rendersprite(bm, sh, 100.0, 100.0, 1.0, 1.0);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }

}
