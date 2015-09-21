#include "GameApi.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

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
struct Envi
{
  EveryApi *ev;
  float f;
  SH sh;
  PolygonObj *poly;
};
void iter(void *data)
{
  Envi &envi = *(Envi*)data;
  envi.f = envi.ev->mainloop_api.get_time()/1000.0;
  //envi.ev->shader_api.set_var(envi.sh, "time", envi.f);
    // clear frame buffer
    envi.ev->mainloop_api.clear();

    envi.poly->render();

    envi.ev->mainloop_api.fpscounter();
    // swapbuffers
    envi.ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = envi.ev->mainloop_api.get_event();
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
}
int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default(); 
  
  PT center = ev.point_api.point(0.0,0.0,0.0);
  
  SFO skybox = ev.sh_api.sphere(center, 1000.0);
  SFO skybox_2 = ev.sh_api.sphere(center, 990.0);
  SFO skybox_3 = ev.sh_api.and_not(skybox, skybox_2);
  SFO skybox_4 = ev.sh_api.color(skybox_3, 0.7,0.8,1.0);

  SFO sphere = ev.sh_api.sphere(center, 130.0);
  PT centerA = ev.point_api.point(100.0,0.0,0.0);
  SFO sphereA = ev.sh_api.sphere(centerA, 90.0);
  SFO sphereB = ev.sh_api.bind_arg(sphereA, "center", "vec3(0.0+100.0*sin(time*12.0/5.0),0.0,0.0)");
  SFO sphereB_c = ev.sh_api.color(sphereB, 1.0, 0.1, 0.6);

  SFO sphereC = ev.sh_api.sphere(centerA, 90.0);

  SFO sphereD = ev.sh_api.bind_arg(sphereC, "center", "vec3(0.0,0.0,0.0+100.0*sin(time))");

  SFO sphereD_c = ev.sh_api.color(sphereD, 0.0,1.0,0.0);
  //SFO noise_1 = ev.sh_api.noise(sphereD_c, 1.0);

  //SFO tr = ev.sh_api.trans(sphereD_c);
  //SFO tr2 = ev.sh_api.bind_arg(tr, "delta", "vec3(10.0*sin(pt.x+time/3.0),10.0*cos(pt.y+time/4.0),0.0)");


  SFO line_1 = ev.sh_api.line(-300.0, -50.0, 0.0,
			      -300.0, 100.0, 0.0,
			      20.0, 10.0);

  SFO sphere2 = ev.sh_api.rounded_cube(-100.0,100.0,
				       -100.0,100.0,
				       -100.0,100.0, 20);
  SFO sphere2_c = ev.sh_api.color(sphere2, 1.0,0.0,0.0);

  SFO andnot = ev.sh_api.and_not(sphere2_c, sphere);
  SFO sphere_1a = ev.sh_api.or_elem(andnot, sphereB_c);
  SFO sphere_2a = ev.sh_api.or_elem(sphereD_c, sphere_1a);
  SFO line_2a = ev.sh_api.or_elem(sphere_2a, line_1);
  //SFO line_2b = ev.sh_api.or_elem(line_2a, sph_2);
  //SFO move = ev.sh_api.trans(line_2a, 100.0, 0.0,0.0);
  SFO plane_1 = ev.sh_api.plane(ev.point_api.point(0.0,100.0,0.0),
				ev.vector_api.vector(4.0,0.0,0.0),
				ev.vector_api.vector(0.0,0.0,4.0));
  SFO plane_11 = ev.sh_api.grayscale(plane_1);
  SFO plane_2 = ev.sh_api.or_elem(plane_11, line_2a);
  SFO torus_1 = ev.sh_api.torus(100.0, 30.0);
  SFO torus_2 = ev.sh_api.trans(torus_1, 300.0, 60.0,0.0);
  SFO torus_3 = ev.sh_api.or_elem(plane_2, torus_2);

  //SFO mod_trans = ev.sh_api.trans(torus_3,0.0,0.0,50.0);
  //SFO mod = ev.sh_api.mod_z(mod_trans, 200.0);

  SFO skybox_or = ev.sh_api.or_elem(torus_3, skybox_4);
  

  SFO rot_y = ev.sh_api.rot_y(skybox_or, 0.0);
  SFO rot_y_2 = ev.sh_api.bind_arg(rot_y, "angle", "time/1.25");
  //SFO trans_test = ev.sh_api.trans(rot_y_2);
  //SFO trans_test_22 = ev.sh_api.bind_arg(trans_test, "delta", "vec3(5.0*sin(time+pt.x/10.0),3.0*sin(time*3.0+pt.y/2.0),2.0*cos(time*3.0+pt.z/5.0))");
  SFO rot_y_3 = ev.sh_api.stop_generation(rot_y_2);

  SFO color = ev.sh_api.color_from_normal(rot_y_3 );
  //SFO color_2 = ev.sh_api.grayscale(color);
  SFO mix_color = ev.sh_api.mix_color(rot_y_2, color, 1.0-0.4);

  V light_dir = ev.vector_api.vector(-20.0, -40.0, -20.0);
  SFO soft_shadow = ev.sh_api.soft_shadow(mix_color, light_dir, 0.0, 1.0, 2.0, 8.0);
  SFO ao = ev.sh_api.ambient_occulsion(soft_shadow, 6.2, 50.0);
  SFO sphere_render = ev.sh_api.render(ao);

  SH sh = ev.shader_api.get_normal_shader("screen", "screen", "", "", "", false, sphere_render);

  // rest of the initializations
  ev.mainloop_api.init(sh);


  P p = ev.polygon_api.quad_z(0.0, 800.0,
			      0.0, 600.0,
			      0.0);

  P p2 = ev.polygon_api.normal_function(p, std::bind(func,_1,_2,std::ref(ev)));
 
  PolygonObj poly(ev, p2, sh);
  poly.prepare();
  float f = 0.0;



  Envi envi;
  envi.poly = &poly;
  envi.f = f ;
  envi.sh = sh;
  envi.ev = &ev;


#ifndef EMSCRIPTEN
  while(1) {
    iter(&envi);
    // ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&envi, 30,1);
#endif


}
