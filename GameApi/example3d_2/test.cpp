#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace GameApi;

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d();

    env->poly->render();

    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
      }
}


int main() {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);



  P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
  PolygonObj poly(ev, p3, sh);
  poly.prepare();

  env.ev = &ev;
  env.poly = &poly;

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
