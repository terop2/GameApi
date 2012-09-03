
#include "GameApi.hh"

using namespace GameApi;

P torus_func(EveryApi &e, float val, void *cb)
{
  float *cb2 = (float*)cb;
  PT p = e.point_api.point(*cb2,*cb2,0.0);
  return e.polygon_api.sphere(p, val, 20, 20);
}

void GameTest5(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  ShaderApi &shader = e.shader_api;
  StateChangeApi &change_api = e.state_change_api;
  loop.init_3d();
  loop.alpha(true);
  shader.load("Shader.txt");
  SH sh = shader.get_shader("linear", "", "");
  shader.bindnames(sh, "vertex2", "","","");
  shader.use(sh);
  
  TR t = change_api.init(2);
  float val = 0.0;
  float val2 = 200.0;
  TR t2 = change_api.linear(t, 0, &torus_func, 100.0, 200.0, 100.0, &val);
  TR t3 = change_api.linear(t2, 0, &torus_func, 200.0, 100.0, 100.0, &val);
  TR t4 = change_api.linear(t3, 1, &torus_func, 100.0, 200.0, 100.0, &val2);
  TR t5 = change_api.linear(t4, 1, &torus_func, 200.0, 100.0, 100.0, &val2);
  
  VV valval = change_api.prepare(t5);
  float time = 0.0;
  while(1) {
    e.mainloop_api.clear();
    time += 1.0;
    if (time > 200.0) time = 0.0;
    change_api.render(valval, time, sh);
    e.mainloop_api.swapbuffers();
    MainLoopApi::Event ev = e.mainloop_api.get_event();
    if (ev.ch==27) break;
  }
  shader.unuse(sh);
}
