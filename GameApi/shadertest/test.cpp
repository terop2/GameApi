#include "GameApi.hh"

using namespace GameApi;

V func(int face, int point, EveryApi &ev)
{
  switch(point)
    {
    case 0: return ev.vector_api.vector(0.0,0.0,0.0);
    case 1: return ev.vector_api.vector(1.0,0.0,0.0);
    case 2: return ev.vector_api.vector(1.0,1.0,0.0);
    case 3: return ev.vector_api.vector(0.0,1.0,0.0);
    };
  return ev.vector_api.vector(0.0,0.0,0.0);
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  
  PT center = ev.point_api.point(0.0,0.0,0.0);
  SFO sphere = ev.sh_api.sphere(center, 130.0);
  PT centerA = ev.point_api.point(0.0,0.0,0.0);
  SFO sphereA = ev.sh_api.sphere(centerA, 90.0);

  //SFO sphere_1 = ev.sh_api.bind_arg(sphere, "radius", "120+10*sin(time/10.0)");
  SFO sphere2 = ev.sh_api.cube(0.0,100.0,
			      0.0,100.0,
			      0.0,100.0);
  
  SFO andnot = ev.sh_api.and_not(sphere2, sphere);
  SFO sphere_1a = ev.sh_api.or_elem(andnot, sphereA);
  SFO rot_y = ev.sh_api.rot_y(sphere_1a);
  SFO rot_y_2 = ev.sh_api.bind_arg(rot_y, "angle", "time/30.0");
  SFO color = ev.sh_api.color_from_normal(rot_y_2);
  SFO sphere_render = ev.sh_api.render(rot_y_2, color);

  SH sh = ev.shader_api.get_normal_shader("test", "screen", "", "", "", false, sphere_render);

  // rest of the initializations
  ev.mainloop_api.init(sh);


  P p = ev.polygon_api.quad_z(0.0, 800.0,
			      0.0, 600.0,
			      0.0);

  P p2 = ev.polygon_api.normal_function(p, std::bind(func,_1,_2,std::ref(ev)));
 
  PolygonObj poly(ev, p2, sh);
  poly.prepare();
  float f = 0.0;
  while(1) {
    f+=0.2;
    ev.shader_api.set_var(sh, "time", f);
    // clear frame buffer
    ev.mainloop_api.clear();

    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
