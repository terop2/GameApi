#include "GameApi.hh"
#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace GameApi;

char world[] = \
  "+---------+"
  "|....%....|"
  "|..+---+..|"
  "|..|...|..|"
  "|..+.*.+..|"
  "|&.......#|"
  "+--.....--+";

unsigned int func(char c) {
  switch(c) {
  case '.': return 0;
  case '-': return 1;
  case '|': return 2;
  case '+': return 3;
  case '*': return 4;
  case '%': return 5;
  case '&': return 6;
  case '#': return 7;
  };
}

P points_func(int i, float x, float y, float z, unsigned int color, EveryApi &ev)
{
#if 0
  PT pt = ev.point_api.point(x,y,z);
  return ev.polygon_api.sphere(pt, 1.0, 10,10);
#endif
  return ev.polygon_api.cube(x,x+1.0,y,y+1.0,z,z+1.0);
}

P pieces(unsigned int i, EveryApi &ev)
{
 switch(i) {
 case 7:
   {
     PT pt = ev.point_api.point(50.0,40.0,50.0);
     O o = ev.volume_api.sphere(pt, 40.0);
     FO fo = ev.float_volume_api.from_volume(o, 0.0, 1.0);
     PTS pts = ev.points_api.from_float_volume(fo, 1000, 0.0, 0.0, 0.0, 100.0, 100.0, 100.0);
     P p2 = ev.polygon_api.from_points(pts, std::bind(points_func, _1, _2, _3, _4, _5, std::ref(ev)));
    
    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);
    return ppa;
   }

 case 6:
   {
     P p1 = ev.polygon_api.cube(30.0,70.0,30.0,70.0,30.0,70.0);
     P p11 = ev.polygon_api.rotatex(p1, 45.0*3.14159*2.0/360.0);
     P p2 = ev.polygon_api.translate(p11,0.0,40.0,0.0);

    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);
    return ppa;

   }
 case 5:
   {
     PT pt1 = ev.point_api.point(50.0,0.0, 50.0);
     PT pt2 = ev.point_api.point(50.0,80.0,50.0);
     P p2 = ev.polygon_api.cone(80, pt1, pt2, 30.0, 50.0);

    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);
    return ppa;
 }
 case 4:
   {
     PT pt = ev.point_api.point(50.0,40.0,50.0);
     P p2 = ev.polygon_api.sphere(pt, 40.0, 40,40);

    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);
    return ppa;
   }
 case 3:
   {
    P p = ev.polygon_api.cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0);
    P pa = ev.polygon_api.cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0);
    P pp = ev.polygon_api.or_elem(p,pa);
    P p2 = ev.polygon_api.color_faces(pp, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff);

    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);

    return ppa;
   }
 case 2:
   {
    P p = ev.polygon_api.cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0);
    P p2 = ev.polygon_api.color_faces(p, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff);


    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);

    return ppa;
   }
   
  case 1:
    {
    P p = ev.polygon_api.cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0);
    P p2 = ev.polygon_api.color_faces(p, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff);

    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1a,p2a);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    P ppa = ev.polygon_api.or_elem(p2,pk);

    return ppa;
    }
  case 0:
    P p1 = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P p2 = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pc = ev.polygon_api.or_elem(p1,p2);
    P pk = ev.polygon_api.color_faces(pc, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
    return pk; 
  };
  return ev.polygon_api.empty();
}

int main() {
  srand(1);
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

  BM bm = ev.bitmap_api.newintbitmap(world, 11, 7, func);
  P p = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev)), bm   , 100.0, 100.0);
  PolygonObj poly(ev, p, sh);
  poly.set_scale(3.0,3.0,3.0);
  poly.prepare();


  PT pt = ev.point_api.point(0.0,0.0,0.0);
  P pp = ev.polygon_api.sphere(pt, 20.0, 40,40);
  PolygonObj sphere(ev, pp, sh);
  sphere.set_scale(3.0,3.0,3.0);
  sphere.prepare();

  float pos_x = 0.0;
  float pos_y = 0.0;
  float rot_y = 0.0;
  float speed = 100.0;
  float rot_speed = 8.0*3.14159*2.0/360.0;
  float speed_x, speed_y;
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();
    //poly.set_rotation_matrix(ev.matrix_api.xrot(frame));
    M m = ev.matrix_api.yrot(rot_y);
    M m2 = ev.matrix_api.trans(0.0,0.0,400.0);
    M m3 = ev.matrix_api.trans(0.0,0.0,-400.0);
    M mm = ev.matrix_api.mult(ev.matrix_api.mult(m3,m),m2);
    poly.set_rotation_matrix2(mm);
    poly.set_pos(pos_x, -80.0, pos_y);
    poly.render();
    
    //sphere.set_pos(0.0,0.0,400.0);
    //sphere.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    if (e.ch=='a') { pos_y+=speed_y; pos_x+=speed_x; }
    if (e.ch=='z') { pos_y-=speed_y; pos_x-=speed_x; }
    if (e.ch==',') { rot_y -= rot_speed; }
    if (e.ch=='.') { rot_y += rot_speed; }
    speed_x = speed*cos(rot_y+3.14159/2.0);
    speed_y = speed*sin(rot_y+3.14159/2.0);
  }



}
