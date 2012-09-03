
#include "GameApi.hh"
#include <iostream>

using namespace GameApi;

P torus_func(EveryApi &e, float val, void *cb)
{
  std::cout << "torus_func" << val << std::endl;
  float *cb2 = (float*)cb;
  PT p = e.point_api.point(*cb2,*cb2,100.0);
  P p0 = e.polygon_api.sphere(p, val, 20, 20);
  //P p1 = e.polygon_api.scale(p0, 0.05,0.05,0.2);
  return p0;
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
  
  TR t = change_api.init(2);
  float val = 0.0;
  float val2 = 150.0;
  TR t2 = change_api.linear(t, 0, &torus_func, 100.0, 200.0, 100.0, &val);
  TR t3 = change_api.linear(t2, 0, &torus_func, 200.0, 100.0, 100.0, &val);
  TR t4 = change_api.linear(t3, 1, &torus_func, 100.0, 200.0, 100.0, &val2);
  TR t5 = change_api.linear(t4, 1, &torus_func, 200.0, 100.0, 100.0, &val2);
  
  VV valval = change_api.prepare(t5);
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
    change_api.render(valval, time, sh);
    //e.polygon_api.render_vertex_array(va);
    e.mainloop_api.swapbuffers();
    MainLoopApi::Event ev = e.mainloop_api.get_event();
    if (ev.ch==27) break;
  }
  shader.unuse(sh);
}
