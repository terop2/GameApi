#include "GameApi.hh"
#include <vector>

using namespace GameApi;

class GameApiWorld
{
public:
  GameApiWorld(EveryApi &e) : e(e) { }
  P get() const;

  void push_box(float start_x, float end_x, 
		float start_y, float end_y,
		float start_z, float end_z)
  {
    PT p1 = e.point_api.point(start_x, start_y, start_z);
    PT p2 = e.point_api.point(end_x, start_y, start_z);
    PT p3 = e.point_api.point(end_x, end_y, start_z);
    PT p4 = e.point_api.point(start_x, end_y, start_z);
    PT p5 = e.point_api.point(start_x, start_y, end_z);
    PT p6 = e.point_api.point(end_x, start_y, end_z);
    PT p7 = e.point_api.point(end_x, end_y, end_z);
    PT p8 = e.point_api.point(start_x, end_y, end_z);
    push_line(p1,p2);
    push_line(p2,p3);
    push_line(p3,p4);
    push_line(p4,p1);

    push_line(p5,p6);
    push_line(p6,p7);
    push_line(p7,p8);
    push_line(p8,p5);

    push_line(p1,p5);
    push_line(p2,p6);
    push_line(p3,p7);
    push_line(p4,p8);
  }
  
  void push_line(PT p1, PT p2)
  {
    polys.push_back(e.polygon_api.line(p1,p2));
  }
private:
  EveryApi &e;
  std::vector<P> polys;
};

void GameApiWorldFunc(EveryApi &e)
{
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.shader_api.use(sh);
  e.shader_api.set_default_projection(sh, "in_P");

  e.mainloop_api.init(sh);
  e.mainloop_api.antialias(true);


  GameApiWorld world(e);
  world.push_box(-100.0, 100.0,
		 -100.0, 100.0,
		 -100.0, 100.0);
  P p = world.get();
  e.polygon_api.preparepoly(p);

  while(1) {
      e.mainloop_api.clear();
      e.polygon_api.renderpoly(p, 0.0,0.0,0.0);
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
  }
}
