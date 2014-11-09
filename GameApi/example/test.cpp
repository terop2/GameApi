#include "GameApi.hh"
#include <math.h>
using namespace GameApi;

float bm_func(int x, int y) {
  x-=320;
  y-=200;
  float xx = x;
  float yy = y;
  xx/=320;
  yy/=320;
  return fmod(xx,1.0)+fmod(yy,1.0);
  //return sqrt(xx*xx+yy*yy)+sqrt((xx-0.5)*(xx-0.5)+(yy-0.5)*(yy-0.5));
//return 1.0/(1.0-x/640.0);
  //return cos(x*x/800.0)*sin(y*y/800.0);
  //return sqrt(x*x-y*y);
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.get_shader("texture", "texture", "");
  ev.shader_api.bind_attrib(sh, 0, "in_Position");
  ev.shader_api.bind_attrib(sh, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh, 2, "in_Color");
  ev.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  ev.shader_api.link(sh);
  ev.shader_api.use(sh);
  ev.shader_api.set_default_projection(sh, "in_P");

  // rest of the initializations
  ev.mainloop_api.init(sh);
  
  // prepare a circle for drawing
  BB bg = ev.bool_bitmap_api.empty(100,100);
  BB bb = ev.bool_bitmap_api.circle(bg, 50.0, 50.0, 50.0);
  BM bm = ev.bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0, 0, 0, 0);
  BM bm2 = ev.bool_bitmap_api.to_bitmap(bb, 255,0,0,255, 0,0,0,0);
  ev.sprite_api.preparesprite(bm);

  FB fb = ev.float_bitmap_api.function(&bm_func, 640,400);
  BM bm3 = ev.float_bitmap_api.to_grayscale_color(fb, 255,128,0,255,
				      0,0,0,0);
  ev.sprite_api.preparesprite(bm3);

  ev.mainloop_api.alpha(true);
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear();

    // render sprite
    ev.sprite_api.rendersprite(bm, sh, 100.0, 100.0);
    // swapbuffers
    ev.sprite_api.rendersprite(bm3, sh, 200.0, 200.0);
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
