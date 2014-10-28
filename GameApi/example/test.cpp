#include "GameApi.hh"

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
  BB bg = ev.bool_bitmap_api.empty(100,100);
  BB bb = ev.bool_bitmap_api.circle(bg, 50.0, 50.0, 50.0);
  BM bm = ev.bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0, 0, 0, 0);
  BM bm2 = ev.bool_bitmap_api.to_bitmap(bb, 255,0,0,255, 0,0,0,0);
  ev.sprite_api.preparesprite(bm);

  std::vector<BM> vec;
  vec.push_back(bm);
  vec.push_back(bm2);
  SpriteObj spr1(ev, vec, sh);
  spr1.prepare();
  int id = 0;
  float s = 1.0;
  while(1) {
    id=1-id;
    s+=0.01;
    // clear frame buffer
    ev.mainloop_api.clear();

    // render sprite
    ev.sprite_api.rendersprite(bm, sh, 100.0, 100.0);
    spr1.set_anim_frame(id);
    spr1.set_scale(s,s);
    spr1.render();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
