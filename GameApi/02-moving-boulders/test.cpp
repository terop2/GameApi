#include "GameApi.hh"
#include <iostream>

using namespace GameApi;


char world[] = 
  "+++++++++"
  "+OOOOOO.+"
  "+XXXXXOO+"
  "+.....XX+"
  "+..@....+"
  "+++++++++";

const int number_of_pieces = 5;


bool blocking_piece(int c)
{
  switch(c) {
  case 0: return true;
  case 1:
  case 2:
  case 3:
    return false;
  case 4: return true;
  };
}

P pieces(int c, EveryApi &ev)
{
  // std::cout << c << std::endl;
  switch(c)
    {
    case 4:
      {
      P p = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
      P p2 = ev.polygon_api.color_faces(p, 0xff884422, 0xff442211, 0xff884422, 0xff442211);
      return p2;
      }
    case 0:
      {
      PT center = ev.point_api.point(50.0,50.0,50.0);
      P p = ev.polygon_api.sphere(center, 50.0, 50, 50); 
      P p2 = ev.polygon_api.color_from_normals(p);
     return p2;
      }
    case 1:
      {
      P p = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
      P p2 = ev.polygon_api.color_from_normals(p);
      return p2;
      }
    case 3:
      {
	PT center = ev.point_api.point(50.0, 50.0, 20.0);
	P p = ev.polygon_api.sphere(center, 20.0, 30,30);
	PT pt1 = ev.point_api.point(50.0, 50.0, 20.0);
	PT pt2 = ev.point_api.point(50.0, 50.0, 50.0);
	PT pt3 = ev.point_api.point(50.0, 50.0, 60.0);
	PT pt4 = ev.point_api.point(50.0, 50.0, 100.0);
	P p2 = ev.polygon_api.cone(30, pt1, pt2, 20.0, 20.0);
	P p3 = ev.polygon_api.or_elem(p,p2);
	P p3_a = ev.polygon_api.cone(30, pt2, pt3, 30.0, 20.0);
	P p3_b = ev.polygon_api.cone(30, pt3, pt4, 40.0, 30.0);
	P p3_comb = ev.polygon_api.or_elem(p3_a, p3_b);
	P p3_comb2 = ev.polygon_api.or_elem(p3_comb, p3);
	P p4 = ev.polygon_api.color_from_normals(p3_comb2);
	return p4;
      }
    };
  return ev.polygon_api.empty();
}

int pieces_2(char c)
{
  switch(c) {
  case 'O': return 0;
  case 'X': return 1;
  case '.': return 2;
  case '@': return 3;
  case '+': return 4;
  }
  return 1;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);

  
  BM bm = ev.bitmap_api.newintbitmap(world, 9, 6, pieces_2);
  //P p3 = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev)), bm, 100.0, 100.0);
  //P p2 = ev.polygon_api.rotatex(p3, 90.0*3.14159*2.0/360.0);


  WorldObj poly(ev, std::bind(&pieces, _1, std::ref(ev)), number_of_pieces, bm, 100.0, 100.0, sh);
  poly.set_pos(-500.0, 400.0, 0.0);
  poly.set_rotation_matrix(ev.matrix_api.xrot(90.0*3.14159*2.0/360.0));
  poly.prepare();
 
  int player_pos_x = 3;
  int player_pos_y = 4;

  int old_player_pos_x;
  int old_player_pos_y;

  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
    poly.set_block(player_pos_x,player_pos_y,2);
    old_player_pos_x = player_pos_x;
    old_player_pos_y = player_pos_y;
    if (e.ch=='a'&&e.type==0x300) { player_pos_y--; }
    if (e.ch=='z'&&e.type==0x300) { player_pos_y++; }
    if (e.ch==','&&e.type==0x300) { player_pos_x--; }
    if (e.ch=='.'&&e.type==0x300) { player_pos_x++; }
    int val = poly.read_block(player_pos_x, player_pos_y);
    if (blocking_piece(val))
      {
	player_pos_x = old_player_pos_x;
	player_pos_y = old_player_pos_y;
      }

    poly.set_block(player_pos_x, player_pos_y,3);
  }



}
