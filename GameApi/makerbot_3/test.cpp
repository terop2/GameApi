#include "GameApi.hh"
#include <math.h>

using namespace GameApi;

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default(); //("Shader.txt");
  SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "inst:colour:light:light", "colour:light:light");
  SH sh2 = ev.shader_api.colour_shader();
  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);


  PT cone_1 = ev.point_api.point(0.0,-4.7,0.0);
  PT cone_2 = ev.point_api.point(0.0,-7.0,0.0);
  O cone_12 = ev.volume_api.cone(cone_1, cone_2, 0.3, 0.3);
  O cone_12a = ev.volume_api.rotatex(cone_12, 32.0*3.14159*2.0/360.0);
  O cone_12b = ev.volume_api.rotatex(cone_12, -32.0*3.14159*2.0/360.0);
  O cone_color_a = ev.volume_api.colour(cone_12a, 0xff88ff88);
  O cone_color_b = ev.volume_api.colour(cone_12b, 0xff88ff88);
  O cone_ab = ev.volume_api.max_op(cone_color_a, cone_color_b);
  O cone_ab_1 = ev.volume_api.move(cone_ab, 1.5,0.0,0.0);
  O cone_ab_2 = ev.volume_api.move(cone_ab, -1.5,0.0,0.0);


  PT centerA = ev.point_api.point(0.0,0.0,0.0);
  O ringer = ev.volume_api.sphere(centerA, 2.0);
  O ringer_cube = ev.volume_api.cube(-2.0, 2.0,
				     -2.0, 0.0,
				     -2.0, 2.0);
  O ringer_split = ev.volume_api.andnot_op(ringer, ringer_cube);
  O ringer_pos = ev.volume_api.move(ringer_split, 0.0, 5.0, 0.0);
  O ringer_color = ev.volume_api.colour(ringer_pos, 0xff88ff88);
  O ringer_rot1 = ev.volume_api.rotatex(ringer_color, 45.0*2.0*3.14159/360.0);
  O ringer_rot2 = ev.volume_api.rotatex(ringer_color, -45.0*2.0*3.14159/360.0);


  PT center1 = ev.point_api.point(-2.0, 0.0, 0.0);
  PT center2 = ev.point_api.point(2.0, 0.0, 0.0);
  PT center22 = ev.point_api.point(-1.0,0.0,0.0);
  PT center23 = ev.point_api.point(-1.2,0.0,0.0);
  //O o1 = ev.volume_api.sphere(center1, 2.0);
  //O o2 = ev.volume_api.sphere(center2, 2.0);
  O o3 = ev.volume_api.cone(center1, center2, 5.0, 5.0);
  O o_not = ev.volume_api.cone(center1, center22, 4.7, 4.7);

  O o4 = ev.volume_api.cone(center23, center22, 4.6,4.6);
  O o4_color = ev.volume_api.colour(o4, 0xffff8844);

  O oo = ev.volume_api.andnot_op(o3,o_not);
  O ooo = ev.volume_api.max_op(oo,o4_color);
  O ooo2 = ev.volume_api.max_op(ooo,ringer_rot1);
  O ooo3 = ev.volume_api.max_op(ooo2,ringer_rot2);
  O ooo4 = ev.volume_api.max_op(ooo3,cone_ab_1);
  O ooo5 = ev.volume_api.max_op(ooo4,cone_ab_2);

  O oooo=ooo5;
  for(int i=0;i<12;i++)
    {
      O hand = ev.volume_api.cube(-1.4, -1.2,
				  2.0, 4.3,
				  -0.15, 0.15);
      O hand2 = ev.volume_api.rotatex(hand, i*3.14159*2.0/12.0);
      oooo = ev.volume_api.max_op(oooo,hand2);
    }
  O h1 = ev.volume_api.cube(-1.4, -1.2,
			    0.0, 4.3,
			    -0.20, 0.20);
  O h1_rot = ev.volume_api.rotatex(h1, 66.0*3.14159*2.0/360.0);
  O h1_color = ev.volume_api.colour(h1_rot, 0xff224466);
  O h2 = ev.volume_api.cube(-1.4, -1.2,
			    0.0, 2.0,
			    -0.30, 0.30);
  O h2_rot = ev.volume_api.rotatex(h2, 136.0*3.14159*2.0/360.0);
  O h2_color = ev.volume_api.colour(h2_rot, 0xff224466);

  O h1_comb = ev.volume_api.max_op(oooo, h1_color);
  O h2_comb = ev.volume_api.max_op(h1_comb, h2_color);

  //P p3li = ev.volume_api.rendercubes3(h2_comb, 100, 100, 100,
  // 				     -10.0, 10.0,
  //				     -10.0, 10.0,
  //				     -10.0, 10.0);
  PTS pts3 = ev.volume_api.instanced_positions(h2_comb, 100, 100, 100,
					       -10.0, 10.0,
					       -10.0, 10.0,
					       -10.0, 10.0);
  PTS pts3a = ev.points_api.scale(pts3, 50.0, 50.0, 50.0);
  PTA pta3 = ev.points_api.prepare(pts3a);
  P p3 = ev.polygon_api.cube(-1.0/10.0, 1.0/10.0,
			     -1.0/10.0, 1.0/10.0,
			     -1.0/10.0, 1.0/10.0);
  P p3a = ev.polygon_api.scale(p3,2.0,2.0,2.0);
  P p3aa = ev.polygon_api.translate(p3a, -400.0, 0.0, 0.0);
  P p3b = ev.polygon_api.scale(p3a, 30.0, 30.0, 30.0);
  P p3ba = ev.polygon_api.color_from_normals(p3b);
  P p3bb = ev.polygon_api.color_grayscale(p3ba);
  //P p3bc = ev.polygon_api.color_range(p3bb, 0xffffffff, 0xff222222);
  P p40 = ev.polygon_api.color(p3bb, 0xffffccaa);
  P p41 = ev.polygon_api.mix_color(p3bb, p40, 0.2);

    //ev.polygon_api.color_faces(p3b, 0xffffffff, 0x88888888, 0x44444444, 0x22222222);
  //ev.polygon_api.save_model(p3b, "clock.obj");

  PT center = ev.point_api.point(0.0,0.0,0.0);
  P p_tex = ev.polygon_api.texcoord_spherical(center, p41);

  BM bm = ev.bitmap_api.loadbitmap("iron_texture637.jpg");
  TX tx = ev.texture_api.tex_bitmap(bm);
  TXID id = ev.texture_api.prepare(tx);

  LI lines = ev.lines_api.from_polygon(p3);
  LI lines_1 = ev.lines_api.change_color(lines, 0xffffffff);
  LLA lines_2 = ev.lines_api.prepare(lines);

  LinesObj lines_obj(ev, lines_1, sh2);
  lines_obj.prepare();

  PolygonObj poly(ev, p_tex, sh);
  poly.bind_texture(0,id);
  //poly.set_scale(10.0,10.0,10.0);
  poly.prepare();
  float frame=0.0;
  float speed_x=0.0;
  float speed_y=0.0;
  float pos_x=0.0;
  float pos_y=0.0;
  float rot_y = 0.0;
  float rot_speed = 8.0*3.14159*2.0/360.0;
  float speed = 100.0;
  while(1) {
    frame+=0.01;
    // clear frame buffer
    ev.mainloop_api.clear_3d(0xff0088ff);
    //poly.set_rotation_y(frame);
    M m = ev.matrix_api.yrot(rot_y);
    M m2 = ev.matrix_api.trans(0.0,0.0,400.0);
    M m3 = ev.matrix_api.trans(0.0,0.0,-400.0);
    M mm = ev.matrix_api.mult(ev.matrix_api.mult(m3,m),m2);
    poly.set_rotation_matrix2(mm);

    poly.set_pos(pos_x, 0.0, pos_y);
    //std::cout << pos_x << " " << pos_y << std::endl;
    poly.render_instanced(pta3);
    //lines_obj.set_rotation_matrix2(mm);

    //lines_obj.set_pos(pos_x, 0.0, pos_y);
    //lines_obj.render();
    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    //MainLoopApi::Event e = ev.mainloop_api.get_event();
    MainLoopApi::Event e;
    while((e = ev.mainloop_api.get_event()).last==true)
      {
	if (e.ch==27) { exit(0); }
	if (e.ch=='a') { pos_y+=speed_y; pos_x+=speed_x; }
	if (e.ch=='z') { pos_y-=speed_y; pos_x-=speed_x; }
	if (e.ch==',') { rot_y -= rot_speed; }
	if (e.ch=='.') { rot_y += rot_speed; }
	speed_x = speed*cos(rot_y+3.14159/2.0);
	speed_y = speed*sin(rot_y+3.14159/2.0);
      }
  }



}
