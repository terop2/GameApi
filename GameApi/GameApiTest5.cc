
#include "GameApi.hh"
#include <iostream>


using namespace GameApi;


  struct A {
    float val;
    TX tx;
  };

P torus_func(EveryApi &e, float val, void *cb)
{
  std::cout << "torus_func" << val << std::endl;
  A *cb2 = (A*)cb;
  PT p = e.point_api.point(cb2->val,cb2->val,100.0);
  P p0 = e.polygon_api.sphere(p, val, 20, 20);
  //P p1 = e.polygon_api.sprite_bind(p0, e.texture_api.get_tex_coord(cb2->tx, 0), cb2->tx);
  //P p1 = e.polygon_api.scale(p0, 0.05,0.05,0.2);
  V v = e.vector_api.vector(0.0,-1.0,0.0);
  P p1 = e.polygon_api.rotate(p0, p, v, 180.0);
  return p1;
}

void GameTest5(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  ShaderApi &shader = e.shader_api;
  StateChangeApi &change_api = e.state_change_api;
  loop.init();
  loop.alpha(true);
  shader.load("Shader.txt");
  SH sh = shader.get_shader("linear", "red", "");
  shader.bindnames(sh, "vertex2", "","","");

  BM red = e.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);
  TX tx = e.texture_api.tex_plane(128,128);
  int id = e.texture_api.unique_id();
  TX tx2 = e.texture_api.tex_assign(tx, id, 0,0, red);
  TXID tex = e.texture_api.prepare(tx2);


  A val;
  val.val = 0.0;
  val.tx = tx2;
  A val2;
  val2.val = 150.0;
  val2.tx = tx2;
  TR t = change_api.init(1);
  //float val = 0.0;
  //float val2 = 150.0;
  TR t2 = change_api.linear(t, 0, &torus_func, 100.0, 200.0, 100.0, &val);
  //TR t3 = change_api.linear(t2, 0, &torus_func, 200.0, 100.0, 100.0, &val);
  //TR t4 = change_api.linear(t3, 1, &torus_func, 100.0, 200.0, 100.0, &val2);
  //TR t5 = change_api.linear(t4, 1, &torus_func, 200.0, 100.0, 100.0, &val2);
  
  VV valval = change_api.prepare(t2);
  float time = 0.0;

  //P p = torus_func(e, 100.0, &val2);
  //VA va = e.polygon_api.create_vertex_array(p);
  

  while(1) {
    e.mainloop_api.clear();
    time += 0.1;
    if (time > 200.0) time = 0.0;
    //std::cout << "Time:" << time << std::endl;
    shader.use(sh);
    loop.switch_to_3d(true);
    e.shader_api.set_var(sh, "tex", 0);

    e.texture_api.use(tex);
    change_api.render(valval, time, sh);
    e.texture_api.unuse(tex);
    //e.polygon_api.render_vertex_array(va);
    e.mainloop_api.swapbuffers();
    MainLoopApi::Event ev = e.mainloop_api.get_event();
    if (ev.ch==27) break;
  }
  shader.unuse(sh);
}
