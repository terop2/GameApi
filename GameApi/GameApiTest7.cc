#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#include <cmath>

using namespace GameApi;

float SphereObj2(float x, float y, float z, void *data, EveryApi &ev)
{
  x*=150.0;
  y*=150.0;
  z*=150.0;
#if 0
  float val = x*x+y*y+z*z;
  if (val <= 150.0*150.0 && val >= 140.0*140.0) { return 1.0; }
  if (val <= 100.0*100.0 && val >= 90.0*90.0) { return 1.0; }
  if (val <= 50.0*50.0 && val >= 40.0*40.0) { return 1.0; }
  return 0.0;
#endif

  BM bm_julia = ev.bitmap_api.mandelbrot2(false, -1.0, 1.0, -1.0, 1.0, z/150.0,0.15, 150, 150, 256);
  unsigned int cv = ev.bitmap_api.colorvalue(bm_julia, (int)(x+150.0)/2,(int)(y+150.0)/2);
  int val = cv & 0xff00;
  val>>=8;
  float fval = float(val)/255.0;
  return fval;
}

P Cube2(float start_x, float end_x, 
	float start_y, float end_y, 
	float start_z, float end_z, 
	unsigned int color, 
	void *data,
	EveryApi &api)
{
  P p = api.polygon_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  //P p2 = api.polygon_api.color(p, color);
  return p;
}


void GameTest7(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  VolumeApi &vapi = e.volume_api;
  //FloatVolumeApi &api = e.float_volume_api;

  loop.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.shader_api.bind_attrib(sh, 0, "in_Position");
  e.shader_api.bind_attrib(sh, 1, "in_Normal");
  e.shader_api.bind_attrib(sh, 2, "in_Color");
  e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.link(sh);

  loop.init_3d(sh);
  
  //BM bm_mand = e.bitmap_api.mandelbrot2(false, -2.0, 1.0, -1.0, 1.0, 0.0,0.0, 150, 150, 256);
  //BM bm_julia = e.bitmap_api.mandelbrot2(true, -1.0, 1.0, -1.0, 1.0, 0.25,0.15, 150, 150, 256);

  O obj2 = vapi.subvolume(std::bind(SphereObj2, _1,_2,_3,(void*)0,std::ref(e)), 0.99, 1.5);
  P poly = vapi.rendercubes(obj2, std::bind(Cube2,_1,_2,_3,_4,_5,_6,_7,(void*)0,std::ref(e)), 250, 4.0);
  P poly2 = e.polygon_api.color_faces(poly, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff);
  //P poly2 = poly;
  e.polygon_api.prepare(poly2);

  //FO obj = api.function(&SphereObj2, &bm_mand);
  
  //FOA array = api.prepare(obj, 800000, -150.0,-150.0,-150.0, 150.0,150.0,150.0);
  float time=0.0;
  while(1)
    {
      e.mainloop_api.clear_3d();
      glColor4f(1.0,0.5,0.3,0.2);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
      glRotatef(time, 0.0,1.0,0.0);
      glScalef(150.0,150.0,150.0);
      //api.render(array);
      e.polygon_api.render(poly2,0, 0.0,0.0,0.0);
      glPopMatrix();
      glDisable(GL_BLEND);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time+=0.1;
    }
}
