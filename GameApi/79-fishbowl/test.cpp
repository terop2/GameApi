#include "GameApi.hh"

using namespace GameApi;

PT func(EveryApi &ev, int idx, void *data)
{
  switch(idx) {
  case 0: return ev.point_api.point(0.0,0.0,0.0);
  case 1: return ev.point_api.point(20.0,10.0,0.0);
  case 2: return ev.point_api.point(40.0,15.0,0.0);
  case 3: return ev.point_api.point(60.0,10.0,0.0);
  case 4: return ev.point_api.point(80.0,0.0,0.0);
  case 5: return ev.point_api.point(100.0,5.0,0.0);
  case 6: return ev.point_api.point(120.0,50.0,0.0);
  case 7: return ev.point_api.point(100.0,95.0,0.0);
  case 8: return ev.point_api.point(80.0,100.0,0.0);
  case 9: return ev.point_api.point(60.0,90.0,0.0);
  case 10: return ev.point_api.point(40.0,85.0,0.0);
  case 11: return ev.point_api.point(20.0,90.0,0.0);
  case 12: return ev.point_api.point(0.0,100.0,0.0);
  };
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.texture_shader();
  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);

  PT pp1 = ev.point_api.point(-1900.0, -1900.0, -800.0);
  PT pp2 = ev.point_api.point(1900.0, -1900.0, -800.0);
  PT pp3 = ev.point_api.point(1900.0, 1900.0, -800.0);
  PT pp4 = ev.point_api.point(-1900.0, 1900.0, -800.0);
  P bg = ev.polygon_api.quad(pp1,pp2,pp3,pp4);
  BM sea = ev.bitmap_api.loadbitmap("water_blue_sea.jpg");
  TX tex0 = ev.texture_api.tex_plane(1900*2,1900*2);
  TX tex = ev.texture_api.tex_assign(tex0, 0, 0,0, sea);
  TXID tex2 = ev.texture_api.prepare(tex);
  Q q_bm = ev.texture_api.get_tex_coord(tex, 0);
  P bg2 = ev.polygon_api.sprite_bind(bg, q_bm, tex);
  P bg3 = ev.polygon_api.color_faces(bg2, 0x88888888, 0x88888888, 0x88888888, 0x88888888);
  //P bg4 = ev.polygon_api.texture(bg3, sea);
  PolygonObj bg_obj(ev, bg3, sh2);
  bg_obj.bind_texture(0, tex2);
  bg_obj.prepare();


  PT pos1 = ev.point_api.point(0.0, 0.0, 0.0);
  P p1 = ev.polygon_api.sphere(pos1, 400.0,20,20);
  P p2 = ev.polygon_api.rotatex(p1, 90.0*3.14159*2.0/360.0);
  LI lines = ev.lines_api.from_polygon(p2);
  LinesObj lines_obj(ev, lines, sh);
  lines_obj.prepare();

  PL pl = ev.plane_api.function(&func, 13, 120.0, 100.0, 0);
  PT pos = ev.point_api.point(0.0,0.0,0.0);
  V u_x = ev.vector_api.vector(1.0,0.0,0.0);
  V u_y = ev.vector_api.vector(0.0,1.0,0.0);
  V u_z = ev.vector_api.vector(0.0,0.0,1.0);
  PL pl2 = ev.plane_api.triangulate_all(ev, pl, 13, 1);
  P p3 = ev.plane_api.to_polygon(ev, pl2, pos, u_x, u_y, u_z, 30.0);
  P p4 = ev.polygon_api.scale(p3, 2.0,1.0,1.0);
  P p5 = ev.polygon_api.translate(p4, 0.0, 0.0, 300.0);
  PolygonObj poly(ev, p5, sh);
  poly.prepare();
  float frame = 1.0;
  while(1) {
    frame -=0.01;
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    poly.set_rotation_y(-frame);
    poly.render();
    lines_obj.set_rotation_matrix(ev.matrix_api.yrot(frame*3.14159*2.0/360.0));
    lines_obj.render();
    ev.texture_api.use(tex2);
    bg_obj.render();
    ev.texture_api.unuse(tex2);
    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
