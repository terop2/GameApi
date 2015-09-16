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
  
  
  PT centert = ev.point_api.point(50.0,50.0,50.0);
  O o = ev.volume_api.sphere(centert, 50.0);
  FO fo = ev.float_volume_api.from_volume(o, 0.0, 1.0);
  PTS pts = ev.points_api.from_float_volume(fo, 15, -100.0,-100.0,-100.0,
					    100.0,100.0,100.0);

  PT center = ev.point_api.point(0.0,0.0,0.0);
  SFO sph = ev.sh_api.sphere(center, 10.0);
  SFO sph_2 = ev.sh_api.from_points(pts, sph);


  SFO sphere = ev.sh_api.sphere(center, 130.0);
  PT centerA = ev.point_api.point(100.0,0.0,0.0);
  SFO sphereA = ev.sh_api.sphere(centerA, 90.0);
  SFO sphereB = ev.sh_api.bind_arg(sphereA, "center", "vec3(0.0+100.0*sin(time*3.0),0.0,0.0)");
  SFO sphereB_c = ev.sh_api.color(sphereB, 0.0, 0.0, 1.0);

  SFO sphereC = ev.sh_api.sphere(centerA, 90.0);
  SFO sphereD = ev.sh_api.bind_arg(sphereC, "center", "vec3(0.0,0.0,0.0+100.0*sin(time/3.0*30.0))");
  SFO sphereD_c = ev.sh_api.color(sphereD, 0.0,1.0,0.0);


  SFO line_1 = ev.sh_api.line(0.0, 0.0, 0.0,
			      100.0, 100.0, 0.0,
			      20.0, 10.0);

  //SFO sphere_1 = ev.sh_api.bind_arg(sphere, "radius", "120+10*sin(time/10.0)");
  SFO sphere2 = ev.sh_api.cube(0.0,100.0,
			      0.0,100.0,
			      0.0,100.0);
  SFO sphere2_c = ev.sh_api.color(sphere2, 1.0,0.0,0.0);

  SFO andnot = ev.sh_api.and_not(sphere2_c, sphere);
  SFO sphere_1a = ev.sh_api.or_elem(andnot, sphereB_c);
  SFO sphere_2a = ev.sh_api.or_elem(sphere_1a, sphereD_c);
  SFO line_2a = ev.sh_api.or_elem(sphere_2a, line_1);
  SFO line_2b = ev.sh_api.or_elem(line_2a, sph_2);
  //SFO move = ev.sh_api.trans(line_2a, 100.0, 0.0,0.0);
  SFO rot_y = ev.sh_api.rot_y(line_2b);
  SFO rot_y_2 = ev.sh_api.bind_arg(rot_y, "angle", "time");
  SFO color = ev.sh_api.color_from_normal(rot_y_2);
  SFO color_3 = ev.sh_api.color_from_obj(rot_y_2);
  SFO color_2 = ev.sh_api.grayscale(color);
  SFO mix_color = ev.sh_api.mix_color(color_2, color_3, 0.4);
  SFO sphere_render = ev.sh_api.render(rot_y_2, mix_color);

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
    f+=0.2/30.0;
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
