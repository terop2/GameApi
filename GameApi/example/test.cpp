#include "GameApi.hh"
#include <math.h>
using namespace GameApi;

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

  std::vector<VA> vec;
  //BM bm = ev.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 800, 600, 256);
  PT pos_1 = ev.point_api.point(200.0, 200.0, 0.0);
  PT pos_2 = ev.point_api.point(200.0, 0.0, 0.0);
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, 0xffffffff, 0xff000000, 256,256);
  VA va = ev.sprite_api.create_vertex_array(bm);
  ev.mainloop_api.alpha(true);
  int frame = 0;
  while(1) {
    //frame++;
    // clear frame buffer
    ev.mainloop_api.clear();

    // render sprite
    //ev.sprite_api.rendersprite(vec[frame%256], sh, 0.0, 0.0);
    ev.sprite_api.render_sprite_vertex_array(va);

    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==' ') frame++;
    if (e.ch==27) break;
  }



}
