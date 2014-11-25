#include "GameApi.hh"

using namespace GameApi;

unsigned int color_cone(int pointnum, PT pos)
{
  return 0x00ff2200;
}

unsigned int color_sphere1(int pointnum, PT pos)
{
  return 0x00ffffff;
}

unsigned int color_shadow(int pointnum, PT pos)
{
  return 0x00888888;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.get_shader("colour", "colour", "");
  ev.shader_api.bind_attrib(sh, 0, "in_Position");
  ev.shader_api.bind_attrib(sh, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh, 2, "in_Color");
  ev.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  ev.shader_api.link(sh);
  ev.shader_api.use(sh);
  ev.shader_api.set_default_projection(sh, "in_P");

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
 
  BM bm = ev.bitmap_api.newbitmap(640,480, 0xffffffff);
  SpriteObj sp(ev, bm, sh);
  sp.set_pos(-800,-600);
  sp.set_scale(3.0, 3.0);
  sp.prepare();

  PT p1 = ev.point_api.point(0.0,0.0,0.0);
  PT p2 = ev.point_api.point(0.0,0.0,30.0);
  O cone = ev.volume_api.cone(p1, p2, 100.0, 100.0); 
  FO fo_cone = ev.float_volume_api.from_volume(cone, 0.0, 1.0);
  PTS pts_cone = ev.points_api.from_float_volume(fo_cone, 1000000, -100.0, -100.0, -100.0,
						 100.0, 100.0, 100.0);
  PTS pts_cone_color = ev.points_api.color_function(pts_cone, color_cone);


 
  PT p3 = ev.point_api.point(-40.0, 30.0, 10.0);
  O sphere1 = ev.volume_api.sphere(p3, 30.0);
  FO fo_sphere1 = ev.float_volume_api.from_volume(sphere1, 0.0, 1.0);
  PTS pts_sphere1 = ev.points_api.from_float_volume(fo_sphere1, 10000, -100.0, -100.0, -100.0,
						 100.0, 100.0, 100.0);
  PTS pts_sphere1_color = ev.points_api.color_function(pts_sphere1, color_sphere1);


  PT p4 = ev.point_api.point(40.0, 30.0, 10.0);
  O sphere2 = ev.volume_api.sphere(p4, 30.0);
  FO fo_sphere2 = ev.float_volume_api.from_volume(sphere2, 0.0, 1.0);
  PTS pts_sphere2 = ev.points_api.from_float_volume(fo_sphere2, 10000, -100.0, -100.0, -100.0,
						 100.0, 100.0, 100.0);
  PTS pts_sphere2_color = ev.points_api.color_function(pts_sphere2, color_sphere1);


  O cube1 = ev.volume_api.cube(-60.0, 60.0, -50.0, -30.0, 0.0, 30.0);
  FO fo_cube1 = ev.float_volume_api.from_volume(cube1, 0.0, 1.0);
  PTS pts_cube1 = ev.points_api.from_float_volume(fo_cube1, 10000, -100.0, -100.0, -100.0, 100.0, 100.0, 100.0);
  PTS pts_cube1_color = ev.points_api.color_function(pts_cube1, color_sphere1);
  

  PTS comb = ev.points_api.or_points(pts_cone_color, pts_sphere1_color);
  PTS comb2 = ev.points_api.or_points(comb, pts_sphere2_color);
  PTS comb3 = ev.points_api.or_points(comb2, pts_cube1_color);

  PT pos = ev.point_api.point(0.0, -100.0, 0.0);
  V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
  V u_y = ev.vector_api.vector(0.0, 0.0, 1.0);
  V light_vec = ev.vector_api.vector(40.0, -20.0, -20.0);
  PTS shadow = ev.points_api.shadow_points(comb3, pos, u_x, u_y, light_vec);

  PTS shadow_color = ev.points_api.color_function(shadow, color_shadow);
  PTS comb4 = ev.points_api.or_points(shadow_color, comb3);

  PointsObj cone1(ev, comb4, sh);
  cone1.set_scale(4.0,4.0,4.0);
  cone1.prepare();
 
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();
    sp.render();
    cone1.render();

    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
