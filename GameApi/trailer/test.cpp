#include "GameApi.hh"

using namespace GameApi;

unsigned int slide(int x, int y) {
  float yy = float(y)/600;
  int r = 20*yy;
  int g = 120*yy;
  int b = 255*yy;
  g<<=8;
  r<<=16;
  return 0xff000000 + r + g + b;
}

unsigned int color_func(int pointnum, PT pos)
{
  return 0xffff8844;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.get_shader("texture", "texture", "");
  SH sh2 = ev.shader_api.get_shader("colour", "colour", "");
  ev.shader_api.bind_attrib(sh, 0, "in_Position");
  ev.shader_api.bind_attrib(sh, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh, 2, "in_Color");
  ev.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  ev.shader_api.link(sh);
  ev.shader_api.bind_attrib(sh2, 0, "in_Position");
  ev.shader_api.bind_attrib(sh2, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh2, 2, "in_Color");
  ev.shader_api.bind_attrib(sh2, 3, "in_TexCoord");
  ev.shader_api.link(sh2);
  ev.shader_api.use(sh);
  ev.shader_api.set_default_projection(sh, "in_P");
  // ev.shader_api.use(sh2);
  //ev.shader_api.set_default_projection(sh2, "in_P");

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);
  
  ev.mainloop_api.alpha(false);

  P p = ev.polygon_api.cube(-100.0, 100.0,
			    -100.0, 100.0,
			    -100.0, 100.0);
  PolygonObj poly(ev, p, sh);
  poly.prepare();
 
  BM bm = ev.bitmap_api.function(slide, 800, 600);
  SpriteObj sp(ev, bm, sh);
  sp.set_pos(-800.0, -600.0);
  sp.set_scale(2.0, 2.0);
  sp.prepare();

  O o1 = ev.volume_api.sphere(ev.point_api.point(0.0,0.0,0.0), 100.0);
  O o2 = ev.volume_api.sphere(ev.point_api.point(100.0,0.0,0.0), 100.0);
  O o3 = ev.volume_api.sphere(ev.point_api.point(100.0,0.0,-100.0), 100.0);
  O o4 = ev.volume_api.sphere(ev.point_api.point(100.0,100.0,-50.0), 100.0);
  O o_comb = ev.volume_api.max_op(o1,o2);
  O o_comb2 = ev.volume_api.max_op(o3,o4);
  O o_comb3 = ev.volume_api.max_op(o_comb, o_comb2);

  O eo1 = ev.volume_api.sphere(ev.point_api.point(0.0,0.0,0.0), 80.0);
  O eo2 = ev.volume_api.sphere(ev.point_api.point(100.0,0.0,0.0), 80.0);
  O eo3 = ev.volume_api.sphere(ev.point_api.point(100.0,0.0,-100.0), 80.0);
  O eo4 = ev.volume_api.sphere(ev.point_api.point(100.0,100.0,-50.0), 80.0);
  O eo_comb = ev.volume_api.max_op(eo1,eo2);
  O eo_comb2 = ev.volume_api.max_op(eo3,eo4);
  O eo_comb3 = ev.volume_api.max_op(eo_comb, eo_comb2);


  FO fo1 = ev.float_volume_api.from_volume(o_comb3, 0.0, 0.8);
  FO fo2 = ev.float_volume_api.from_volume(eo_comb3, 0.0, 1.0);
  //FO fo_comb = ev.float_volume_api.maximum(fo1, fo2);
  PTS pts1 = ev.points_api.from_float_volume(fo1, 10000, 
					     -200.0, -200.0, -200.0,
					     200.0, 200.0, 200.0);
  PTS pts2 = ev.points_api.from_float_volume(fo2, 10000, 
					     -200.0, -200.0, -200.0,
					     200.0, 200.0, 200.0);
  PTS pts2_color = ev.points_api.color_function(pts1, color_func);
  PTS pts_comb = ev.points_api.or_points(pts2, pts2_color);
  PointsObj points_1(ev, pts_comb, sh2);
  PointsObj points_2(ev, pts_comb, sh2);
  PointsObj points_3(ev, pts_comb, sh2);
  PointsObj points_4(ev, pts_comb, sh2);
  ArrayObj3d array(ev);
  array.push_back(&points_1, &points_1);
  array.push_back(&points_2, &points_2);
  array.push_back(&points_3, &points_3);
  array.push_back(&points_4, &points_4);
  array.set_child_pos(0, 0.0, 0.0, 0.0);
  array.set_child_pos(1, 100.0, -300.0, 0.0);
  array.set_child_pos(2, 200.0, 120.0, 0.0);
  array.set_child_pos(3, 300.0, 110.0, 0.0);
  array.prepare();


  //points_1.prepare();
  float frame = 0.0;
  while(1) {
    frame += 1.0;
    // clear frame buffer
    ev.mainloop_api.clear_3d();
    ev.shader_api.use(sh);
    sp.render();
    poly.render();
    ev.shader_api.use(sh2);
    array.set_pos(800.0-frame,0.0,0.0);
    array.render();
    array.set_pos(400.0-frame, 50.0,50.0);
    array.render();
    array.set_pos(200.0-frame, 150.0,-50.0);
    array.render();
    //poly2.set_scale(2.0,2.0,2.0);
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
