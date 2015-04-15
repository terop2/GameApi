#include "GameApi.hh"

using namespace GameApi;

char chars[] =
  "thlkqlht"
  "pppppppp"
  "........"
  "........"
  "........"
  "........"
  "PPPPPPPP"
  "THLKQLHT";
int charsmap(char c)
{
  switch(c)
    {
    case 'T': return 0;
    case 'H': return 1;
    case 'L': return 2; 
    case 'K': return 3; 
    case 'Q': return 4;
    case 'P': return 5;

    case 't': return 6;
    case 'h': return 7;
    case 'l': return 8; 
    case 'k': return 9; 
    case 'q': return 10;
    case 'p': return 11;

    }
}

void color_change(int c, P &p, EveryApi &ev)
{
	p = ev.polygon_api.color_from_normals(p);
	if (c>5)
	  {
	    p = ev.polygon_api.color_grayscale(p);
	    p = ev.polygon_api.color_range(p, 0xffaa6666, 0xff002200);
	  }
	else
	  {
	    p = ev.polygon_api.color_grayscale(p);
	    p = ev.polygon_api.color_range(p, 0xffffff88, 0xffaa8822);
	  }
}

P chars_blocks(int c, EveryApi &ev)
{
#if 1
  switch(c)
    {
    case 1: // horse
    case 7:
      {

	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -40.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -32.0, 15.0);
	PT p6 = ev.point_api.point(15.0, -25.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,5.0,6.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,5.0);
	P pc = ev.polygon_api.cone(30, p3,p4,2.0,2.0);
	P pd = ev.polygon_api.cone(30, p4,p5,3.0,2.0);
	P pe = ev.polygon_api.cone(30, p5,p6,0.0,3.0);

	P cube = ev.polygon_api.cube(-2.0, 8.0,
				     -2.0, 2.0,
				     -2.0, 2.0);
	P cube_rot = ev.polygon_api.rotatez(cube, 45.0*3.14159*2.0/360.0);
	P cube_trans = ev.polygon_api.translate(cube_rot, 10.0, -40.0, 15.0);

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);
	p = ev.polygon_api.or_elem(p,cube_trans);

	color_change(c,p,ev);
	return p;

      }
    case 10: // queen
    case 4:
      {
	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -40.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -32.0, 15.0);
	PT p6 = ev.point_api.point(15.0, -25.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,4.0,8.0);
	P pc = ev.polygon_api.cone(30, p3,p4,4.0,4.0);
	P pd = ev.polygon_api.cone(30, p4,p5,6.0,4.0);
	P pe = ev.polygon_api.cone(30, p5,p6,0.0,6.0);

	
	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);
	color_change(c,p, ev);
	return p;

      }
    case 9: // king
    case 3:
      {

	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -40.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -32.0, 15.0);
	PT p6 = ev.point_api.point(15.0, -25.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,4.0,8.0);
	P pc = ev.polygon_api.cone(30, p3,p4,4.0,4.0);
	P pd = ev.polygon_api.cone(30, p4,p5,6.0,4.0);
	P pe = ev.polygon_api.cone(30, p5,p6,0.0,6.0);

	P c1 = ev.polygon_api.cube(13.0, 17.0,
				   -25.0, -18.0,
				   13.0, 17.0);
	P c2 = ev.polygon_api.cube(10.0, 20.0,
				   -20.0, -22.0,
				   13.0, 17.0);
	P cc = ev.polygon_api.or_elem(c1,c2);
	
	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);
	p = ev.polygon_api.or_elem(p, cc);
	color_change(c,p, ev);
	return p;

      }
    case 8: // lähetti
    case 2:
      {

	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -40.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -32.0, 15.0);
	PT p6 = ev.point_api.point(15.0, -25.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,5.0,6.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,5.0);
	P pc = ev.polygon_api.cone(30, p3,p4,3.0,2.0);
	P pd = ev.polygon_api.cone(30, p4,p5,4.0,3.0);
	P pe = ev.polygon_api.cone(30, p5,p6,0.0,4.0);

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);

	color_change(c,p,ev);
	return p;
      }
    case 6:
    case 0: // tower
      {
	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -50.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -42.0, 15.0);
	PT p6 = ev.point_api.point(15.0, -35.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,8.0);
	P pc = ev.polygon_api.cone(30, p3,p4,2.0,2.0);
	P pd = ev.polygon_api.cone(30, p4,p5,6.0,2.0);
	P pe = ev.polygon_api.cone(30, p5,p6,6.0,6.0);
	P pf = ev.polygon_api.cone(30, p6,p6,6.0,0.0);

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);
	p = ev.polygon_api.or_elem(p,pf);
	color_change(c,p,ev);

	return p;

      }
    case 11:
    case 5: // pawn
      {
	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	PT p4 = ev.point_api.point(15.0, -50.0, 15.0);
	PT p5 = ev.point_api.point(15.0, -42.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,8.0);
	P pc = ev.polygon_api.cone(30, p3,p4,2.0,2.0);
	P pd = ev.polygon_api.cone(30, p4,p5,3.0,2.0);
	
	PT p7 = ev.point_api.point(15.0, -42.0, 15.0);
	P p_sphere = ev.polygon_api.sphere(p7, 6.0, 5, 5);

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	p = ev.polygon_api.or_elem(p,pp);
	p = ev.polygon_api.or_elem(p,p_sphere);

	color_change(c,p,ev);


	return p;
      }
    };
#endif
  return ev.polygon_api.empty();
}


char board[] = 
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX";

int boardmap(char c)
{
  switch(c)
    {
    case 'X': return 0;
    case 'O': return 1;
    };
}

P board_blocks(int c, EveryApi &ev)
{
  P block = ev.polygon_api.cube(0.0, 30.0,
				-60.0, -65.0,
				0.0, 30.0);
  P color_block;
  switch(c) {
  case 0:
    color_block = ev.polygon_api.color_faces(block, 0xffffffff, 0xaaaaaaff, 0xccccccff, 0x888888ff);
    break;
  case 1:
    color_block = ev.polygon_api.color_faces(block, 0x000000ff, 0x222222ff, 0x444444ff, 0x111111ff);
  };
  return color_block;
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

  M m = ev.matrix_api.perspective(70.0, double(800)/600, 10.1, 60000.0);
  ev.shader_api.set_var(sh, "in_P", m);
  M m2 = ev.matrix_api.mult(ev.matrix_api.trans(0.0, 0.0, -1000.0),
			    ev.matrix_api.scale(1.0, -1.0, 1.0));

  ev.shader_api.set_var(sh, "in_T", m2);

  BM bm = ev.bitmap_api.newintbitmap(board, 8,8, boardmap);
  P board = ev.polygon_api.world_from_bitmap(std::bind(&board_blocks, _1, std::ref(ev)), bm, 30.0, 30.0);
  P board2 = ev.polygon_api.scale(board, 2.8,2.8,2.8);
  P board3 = ev.polygon_api.translate(board2, -340.0, 30.0, -400.0);

  BM bm2 = ev.bitmap_api.newintbitmap(chars, 8,8, charsmap);
  P chars = ev.polygon_api.world_from_bitmap(std::bind(&chars_blocks, _1, std::ref(ev)), bm2, 30.0, 30.0);
  P chars2 = ev.polygon_api.scale(chars, 2.8,2.8,2.8);
  P chars3 = ev.polygon_api.translate(chars2, -340.0, 30.0, -400.0);

  P or_b = ev.polygon_api.or_elem(board3, chars3);

  P or_b_rotated = ev.polygon_api.rotatex(or_b, 90.0*3.14159/360.0);
  P or_b_rotated_and_scaled = ev.polygon_api.scale(or_b_rotated,2.0,2.0,2.0);
  P or_b_rotated_and_scaled_and_translated = ev.polygon_api.translate(or_b_rotated_and_scaled, 0.0, 100.0, -300.0);
  PolygonObj poly(ev, or_b_rotated_and_scaled_and_translated, sh);
  poly.prepare();
 
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
  }



}
