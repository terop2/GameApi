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
  BM bm = ev.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 800, 600, 256);
  BM bm2 = ev.bitmap_api.memoize(bm);
  for (int i=0;i<256;i++)
    {
      BB bb = ev.bool_bitmap_api.from_bitmaps_color_area(bm2, 256-i, 255,
						   256-i, 255,
						   256-i, 255,
						   256-i, 255);
      BM bm3 = ev.bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);
      //ev.sprite_api.preparesprite(bm3);
      VA va = ev.sprite_api.create_vertex_array(bm3);
      vec.push_back(va);
    }
  ev.mainloop_api.alpha(true);
  int frame = 0;
  while(1) {
    //frame++;
    // clear frame buffer
    ev.mainloop_api.clear();

    // render sprite
    //ev.sprite_api.rendersprite(vec[frame%256], sh, 0.0, 0.0);
    ev.sprite_api.render_sprite_vertex_array(vec[frame%256]);

    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==' ') frame++;
    if (e.ch==27) break;
  }



}
