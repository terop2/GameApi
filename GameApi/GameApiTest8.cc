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

void GameTest8(EveryApi &e)
{
  e.mainloop_api.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_normal_shader("colour", "colour", "");

  //e.shader_api.link(sh);
  
  //e.shader_api.use(sh);
  e.mainloop_api.init_3d(sh );
  //e.shader_api.bind_attrib(sh, 0, "in_Position");
  //e.shader_api.bind_attrib(sh, 2, "in_Color");
  //e.shader_api.use(sh);
  //e.shader_api.set_default_projection(sh, "in_P");

  P cube = //e.polygon_api.quad_z(-100.0,100.0, -100.0,100.0, 0.0);
    e.polygon_api.cube(-100.0,100.0, -100.0,100.0, -100.0,100.0);
    //e.polygon_api.sphere(e.point_api.point(0.0,0.0,0.0), 100.0, 100, 100);
  LI lines = e.lines_api.from_polygon(cube);
  LI lines3 = e.lines_api.change_color(lines, 0xff880088, 0xff008800);
  LLA array = e.lines_api.prepare(lines3);
  float time = 0.0;
  //glLineWidth(0.02);

  BB bg = e.bool_bitmap_api.empty(150,150);
  BB b = e.bool_bitmap_api.circle(bg, 50.0,50.0, 50.0);
  BB b2 = e.bool_bitmap_api.circle(bg, 80.0,50.0, 40.0);
  BB b3 = e.bool_bitmap_api.andnot_bitmap(b,b2);
  LI l = e.lines_api.border_from_bool_bitmap(b3, -100.0, 100.0, -100.0, 100.0, 0.0);
  LI l2 = e.lines_api.change_color(l, 0xff880088, 0xff008800);
  LLA array2 = e.lines_api.prepare(l2);

  LinesObj lines2(e, l2, sh);
  lines2.prepare();

  while(1)
    {
      e.mainloop_api.delay(10);
      e.mainloop_api.clear_3d();
      glPushMatrix();
      glRotatef(time, 0.0,1.0,0.0);
      e.shader_api.set_y_rotation(sh, "in_MV", time/50.0);
      M m = e.matrix_api.yrot(time/50.0);
      M m2 = e.matrix_api.scale(2.0,2.0,2.0);
      M m3 = e.matrix_api.mult(m,m2);
      e.shader_api.set_var(sh, "in_MV", m3);
      //float *arr = e.lines_api.line_access(array, 0, false);
      //arr[0]+=0.2;
      //e.lines_api.update(array);
      e.lines_api.render(array);
      e.lines_api.render(array2);
      glPopMatrix();
      lines2.render();
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time += 1.0;
    }
}
