
#include "GameApi.hh"

using namespace GameApi;

void GameTest9(GameApi::EveryApi &e)
{
  e.mainloop_api.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.mainloop_api.init_3d(sh);
  
  
  while(1)
    {
      e.mainloop_api.clear_3d();
      
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }
}
