#include "GameApi.hh"

using namespace GameApi;

int main()
{
  Env e;
  EveryApi ev(e);

  ev.mainloop_api.init_window();
  ev.shader_api.load_default();
  // INSERT CODE HERE
  P I1=ev.polygon_api.cube(-100.0,100.0,-100.0,100.0,-100.0,100.0);
  VA I2=ev.polygon_api.create_vertex_array(I1,false);
  ML I3=ev.polygon_api.render_vertex_array_ml(ev,I2);
  BLK I4=ev.blocker_api.game_window(ev,I3);
  // INSERT CODE HERE
  ev.blocker_api.run(I4);
}
