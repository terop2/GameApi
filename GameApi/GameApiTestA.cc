
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#pragma comment(lib, "d:\\cvs\\gameapi-code\\GameApi\\GameApi\\Debug\\GameApi\\GameApi.lib")
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
  PT center2 = e.point_api.point(100.0,-30.0,-50.0);
  PT center3 = e.point_api.point(100.0,100.0,0.0);
  PT center4 = e.point_api.point(200.0,0.0,70.0);
  PT pos = e.point_api.point(-100.0,-100.0, -300.0);
  V u_x = e.vector_api.vector(640.0, 0.0, 0.0);
  V u_y = e.vector_api.vector(0.0, 480.0, 0.0);
  V u_z = e.vector_api.vector(0.0,0.0, 200.0);
  V u_sh = e.vector_api.vector(-10.0,0.0,-10.0);
  FD fd = e.dist_api.sphere(center, 100.0);
  FD fda = e.dist_api.sphere(center2, 100.0);
  FD fdb = e.dist_api.sphere(center3, 100.0);
  FD fdc = e.dist_api.sphere(center4, 100.0);
  //FD fd2 = e.dist_api.line(pos,center, 30.0);
  FD fd12a = e.dist_api.min(fd, fdb);
  FD fd12 = e.dist_api.and_not(fd12a, fda);
  FD fd123 = e.dist_api.min(fd12, fdc);
  //COV cov = e.color_volume_api.function(f, 0);
  FO fo = e.float_volume_api.shadow(fd123, u_sh, 0.02, 200.0, 128);
  COV cov = e.color_volume_api.from_float_volume(fo, 0xff333333, 0xffffffff);
  VO vo = e.vector_volume_api.normal(fd123);
  PT light = e.point_api.point(-10.0,0.0,-200.0);
  CO i_s = e.color_api.u_color(0xffffffff);
  CO i_d = e.color_api.u_color(0xff6688ff);
  CO i_a = e.color_api.u_color(0xff000088);
  COV cov2 = e.color_volume_api.phong(vo, light, i_s, i_d, i_a, 0.4, 0.4, 0.2, 15.0);
  COV cov3 = e.color_volume_api.mix(cov, cov2, 0.95);
  BM bm = e.dist_api.render(fd123, cov3, pos, u_x, u_y, u_z, 640, 480);

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
