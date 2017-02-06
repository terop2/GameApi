#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

int main()
{
  Env e;
  EveryApi ev(e);

  ev.mainloop_api.init_window();
  ev.shader_api.load_default();
  // INSERT CODE HERE

  // INSERT CODE HERE
  ev.blocker_api.run(blk);
}
