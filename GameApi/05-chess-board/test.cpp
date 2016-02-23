#include "GameApi.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cassert>



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
    case '.': return 12;
    }
}
struct Pos { int x; int y; };

struct Envi {
  EveryApi *ev;
  WorldObj *board_obj;
  WorldObj *pieces_obj;
  int cursor_under = 0;
  int cursor_x = 0;
  int cursor_y = 0;
  int *store = new int[8*8];
  int chosen_x = -1;
  int chosen_y = -1;

  float fr=0.0;
  float frspeed=0.01;
  //PolygonObj *cursor;
  int prevbutton;
  bool towering_allowed_white_left;
  bool towering_allowed_white_right;
  bool towering_allowed_black_left;
  bool towering_allowed_black_right;
#if 0
  PolygonObj *bg;

  FBO fbo;
  SH sh2;
  SH sh;
#endif
} env;

std::vector<Pos> possible_moves(WorldObj &o, int x, int y, Envi &e);
bool check_towering(WorldObj *pieces, bool left, bool white, bool check);
Pos towering_new_block(bool left, bool white);


bool piece_color_is_white(int c)
{
  switch(c)
    {
    case 0: return true;
    case 1: return true;
    case 2: return true; 
    case 3: return true; 
    case 4: return true;
    case 5: return true;

    case 6: return false;
    case 7: return false;
    case 8: return false; 
    case 9: return false; 
    case 10: return false;
    case 11: return false;
    case 12: return false;
    };
  assert(0);
}
bool is_inside_board(Pos p)
{
  if (p.x<0||p.x>7) return false;
  if (p.y<0||p.y>7) return false;
  return true;
}
bool piece_action(std::vector<Pos> &pos, int piece2, int piece_color, int opponent_color, Pos p1)
{
  if (piece2==12)
    {
      pos.push_back(p1);
    }
  else
    if (piece_color_is_white(piece2)==piece_color)
      {
	return true;
      }
    else
      if (piece_color_is_white(piece2)==opponent_color)
	{
	  pos.push_back(p1);
	  return true;
	}
  return false;
}

Pos position_attacked(WorldObj &o, int x, int y, bool attacker_is_white, Envi &e)
{
  for(int xx=0;xx<8;xx++)
    for(int yy=0;yy<8;yy++)
      {
	Pos res;
	res.x = xx;
	res.y = yy;
	int piece = o.read_block(xx,yy);
	if (piece==12) continue;
	bool piece_color = piece_color_is_white(piece);
	if (piece_color == attacker_is_white)
	  {
	    std::vector<Pos> p = possible_moves(o, xx,yy, e);
	    for(int i=0;i<p.size();i++)
	      {
		Pos pp = p[i];
		if (pp.x==x && pp.y==y) return res;
	      }
	  }
	else
	  {
	    continue;
	  }
      }
  Pos res2;
  res2.x = -1;
  res2.y = -1;
  return res2;
}
std::vector<Pos> possible_moves(WorldObj &o, int x, int y, Envi &e)
{
  int piece = o.read_block(x,y);
  bool piece_color = piece_color_is_white(piece);
  bool opponent_color = !piece_color;
  std::vector<Pos> pos;
  switch(piece)
    {
    case 0: // T
    case 6:
      {
      for(int xx=1;xx<8;xx++)
	{
	  Pos p1 = { x+xx, y };
	  if (is_inside_board(p1))
	    {
	      int piece2 = o.read_block(p1.x,p1.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p1)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p2 = { x-xx, y };
	  if (is_inside_board(p2))
	    {
	      int piece2 = o.read_block(p2.x,p2.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p2)) { break; }

	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p3 = { x, y-xx };
	  if (is_inside_board(p3))
	    {
	      int piece2 = o.read_block(p3.x,p3.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p3)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p4 = { x, y+xx };
	  if (is_inside_board(p4))
	    {
	      int piece2 = o.read_block(p4.x,p4.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p4)) { break; }
	    }
	}
      }
      break;
    
    case 1: // H
    case 7:
      {
	Pos p1 = { x+1,y+2 };
	if (is_inside_board(p1))
	  {
	    int piece2 = o.read_block(p1.x,p1.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p1);
	  }
	Pos p2 = { x-1,y+2 };
	if (is_inside_board(p2))
	  {
	    int piece2 = o.read_block(p2.x,p2.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p2);
	  }
	Pos p3 = { x+1,y-2 };
	if (is_inside_board(p3))
	  {
	    int piece2 = o.read_block(p3.x,p3.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p3);
	  }

	Pos p4 = { x-1,y-2 };
	if (is_inside_board(p4))
	  {
	    int piece2 = o.read_block(p4.x,p4.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p4);
	  }

	Pos p1a = { x+2,y+1 };
	if (is_inside_board(p1a))
	  {
	    int piece2 = o.read_block(p1a.x,p1a.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p1a);
	  }
	Pos p2a = { x-2,y+1 };
	if (is_inside_board(p2a))
	  {
	    int piece2 = o.read_block(p2a.x,p2a.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p2a);
	  }
	Pos p3a = { x+2,y-1 };
	if (is_inside_board(p3a))
	  {
	    int piece2 = o.read_block(p3a.x,p3a.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p3a);
	  }
	Pos p4a = { x-2,y-1 };
	if (is_inside_board(p4a))
	  {
	    int piece2 = o.read_block(p4a.x,p4a.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p4a);
	  }
      }
      break;
    case 2: // L
    case 8:
      for(int xx=1;xx<8;xx++)
	{
	  Pos p1 = { x+xx, y+xx };
	  if (is_inside_board(p1))
	    {
	      int piece2 = o.read_block(p1.x,p1.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p1)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p2 = { x-xx, y-xx };
	  if (is_inside_board(p2))
	    {
	      int piece2 = o.read_block(p2.x,p2.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p2)) { break; }

	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p3 = { x+xx, y-xx };
	  if (is_inside_board(p3))
	    {
	      int piece2 = o.read_block(p3.x,p3.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p3)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p4 = { x-xx, y+xx };
	  if (is_inside_board(p4))
	    {
	      int piece2 = o.read_block(p4.x,p4.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p4)) { break; }
	    }
	}
      break;

    case 3: // K
    case 9:
      {
	int xx = 1;
	Pos p1 = { x+xx, y };
	if (is_inside_board(p1))
	  {
	    int piece2 = o.read_block(p1.x,p1.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p1);
	  }
	Pos p2 = { x-xx, y };
	if (is_inside_board(p2))
	  {
	    int piece2 = o.read_block(p2.x,p2.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p2);
	    
	  }
	Pos p3 = { x, y-xx };
	if (is_inside_board(p3))
	  {
	    int piece2 = o.read_block(p3.x,p3.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p3);
	  }
	Pos p4 = { x, y+xx };
	if (is_inside_board(p4))
	  {
	    int piece2 = o.read_block(p4.x,p4.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p4);
	  }
      }
      {
	int xx = 1;
	Pos p1 = { x+xx, y+xx };
	if (is_inside_board(p1))
	  {
	    int piece2 = o.read_block(p1.x,p1.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p1);
	  }
	Pos p2 = { x-xx, y-xx };
	if (is_inside_board(p2))
	  {
	    int piece2 = o.read_block(p2.x,p2.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p2);
	    
	  }
	
	Pos p3 = { x+xx, y-xx };
	if (is_inside_board(p3))
	  {
	    int piece2 = o.read_block(p3.x,p3.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p3);
	  }
	Pos p4 = { x-xx, y+xx };
	if (is_inside_board(p4))
	  {
	    int piece2 = o.read_block(p4.x,p4.y);
	    piece_action(pos, piece2, piece_color, opponent_color,p4);
	  }
      }
      { // Towering
	bool white = false;
	if (piece==3) { white=true; }
	bool b1 = check_towering(&o, false, white, false);
	bool b2 = check_towering(&o, true, white, false);
	bool allowed1 = false;
	bool allowed2 = false;
	if (white)
	  {
	    allowed1 = e.towering_allowed_white_right;
	    allowed2 = e.towering_allowed_white_left;
	  }
	else
	  {
	    allowed1 = e.towering_allowed_black_right;
	    allowed2 = e.towering_allowed_black_left;
	  }
	if (allowed1)
	if (b1) pos.push_back(towering_new_block(false, white));
	if (allowed2)
	if (b2) pos.push_back(towering_new_block(true, white));
      }
  break;


    case 4: // Q
    case 10:

      {
      for(int xx=1;xx<8;xx++)
	{
	  Pos p1 = { x+xx, y };
	  if (is_inside_board(p1))
	    {
	      int piece2 = o.read_block(p1.x,p1.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p1)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p2 = { x-xx, y };
	  if (is_inside_board(p2))
	    {
	      int piece2 = o.read_block(p2.x,p2.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p2)) { break; }

	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p3 = { x, y-xx };
	  if (is_inside_board(p3))
	    {
	      int piece2 = o.read_block(p3.x,p3.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p3)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p4 = { x, y+xx };
	  if (is_inside_board(p4))
	    {
	      int piece2 = o.read_block(p4.x,p4.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p4)) { break; }
	    }
	}
      }

      for(int xx=1;xx<8;xx++)
	{
	  Pos p1 = { x+xx, y+xx };
	  if (is_inside_board(p1))
	    {
	      int piece2 = o.read_block(p1.x,p1.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p1)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p2 = { x-xx, y-xx };
	  if (is_inside_board(p2))
	    {
	      int piece2 = o.read_block(p2.x,p2.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p2)) { break; }

	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p3 = { x+xx, y-xx };
	  if (is_inside_board(p3))
	    {
	      int piece2 = o.read_block(p3.x,p3.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p3)) { break; }
	    }
	}
      for(int xx=1;xx<8;xx++)
	{
	  Pos p4 = { x-xx, y+xx };
	  if (is_inside_board(p4))
	    {
	      int piece2 = o.read_block(p4.x,p4.y);
	      if (piece_action(pos, piece2, piece_color, opponent_color,p4)) { break; }
	    }
	}
    
  break;
    
    case 5: // P
    case 11:
      {
	//std::cout << "PIECE_COLOR: " << piece_color << ":" << y << std::endl;
	if (piece_color==true && y==6)
	  {
	    Pos p0 = { x, y-1 };
	    int piece2 = o.read_block(p0.x,p0.y);
	    if (piece2==12)
	      {
		Pos p1 = { x, y-2 };
		int piece3 = o.read_block(p1.x,p1.y);
		if (piece3==12)
		  {
		    if (is_inside_board(p1)) { pos.push_back(p1); }
		  }
	      }
	  }
	else if (piece_color==false && y==1)
	  {
	    Pos p0 = { x, y+1 };
	    int piece2 = o.read_block(p0.x,p0.y);
	    if (piece2==12)
	      {
		Pos p1 = { x, y+2 };
		int piece3 = o.read_block(p1.x,p1.y);
		if (piece3==12)
		  {
		    if (is_inside_board(p1)) { pos.push_back(p1); }
		  }
	      }
	  }

	int delta = 0;
	if (piece_color==true) { delta = -1; } else {delta = 1; }
	Pos p1 = { x,y+delta };
	if (is_inside_board(p1))
	  {
	    int piece2 = o.read_block(p1.x,p1.y);
	    if (piece2 == 12)
	      {
		pos.push_back(p1);
	      }
	  }
	Pos p2 = { x-1,y+delta };
	if (is_inside_board(p2))
	  {
	    int piece2 = o.read_block(p2.x,p2.y);
	    if (piece2!=12 && opponent_color == piece_color_is_white(piece2))
	      {
		pos.push_back(p2);
	      }
	  }
	Pos p3 = { x+1,y+delta };
	if (is_inside_board(p3))
	  {
	    int piece2 = o.read_block(p3.x,p3.y);
	    if (piece2!=12 && opponent_color == piece_color_is_white(piece2))
	      {
		pos.push_back(p3);
	      }
	  }
      }


    case 12: break;
    };
  return pos;
}
// TODO: Pawn in last row, changes to queen
void do_promotion(WorldObj *board)
{
  for(int x=0;x<8;x++)
    {
      int block = board->read_block(x, 0);
      if (block==5) board->set_block(x,0,4);
      int block2 = board->read_block(x,7);
      if (block2==11) board->set_block(x,7,10);
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


PT horse_head_func(EveryApi &ev, int idx, void *data)
{
  switch(idx)
    {
    case 0: return ev.point_api.point(8+0.0,0.0,0.0);
    case 1: return ev.point_api.point(8+15.0,0.0,0.0);
    case 2: return ev.point_api.point(8+16.0,16.0,0.0);
    case 3: return ev.point_api.point(8+7.0,30.0,0.0);
    case 4: return ev.point_api.point(8+7.0,35.0,0.0);
    case 5: return ev.point_api.point(8+14.0,30.0,0.0);
    case 6: return ev.point_api.point(8+16.0,35.0,0.0);
    case 7: return ev.point_api.point(8+3.0,45.0,0.0);
    case 8: return ev.point_api.point(8+(-7.0),38.0,0.0);
    case 9: return ev.point_api.point(8+(-7.0),30.0,0.0);
    case 10: return ev.point_api.point(8+0.0,15.0,0.0);
    };
  return ev.point_api.point(8+0.0,0.0,0.0);
}


P chars_blocks(int c, EveryApi &ev)
{
#if 1
  switch(c)
    {
    case 1: // horse
    case 7:
      {
	PL horse_head = ev.plane_api.function(horse_head_func, 11, 300.0, 300.0, 0);
	PL horse_head_tri = ev.plane_api.triangulate_all(ev, horse_head, 20, 1);
	PT pos = ev.point_api.point(0.0, 0.0, 0.0);
	V u_x = ev.vector_api.vector(1.0, 0.0,0.0);
	V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
	V u_z = ev.vector_api.vector(0.0, 0.0, 1.0);
	P horse_head_obj = ev.plane_api.to_polygon(ev, horse_head_tri, pos, u_x, u_y, u_z, 3.0);
	P horse_head_obj_scale = ev.polygon_api.scale(horse_head_obj, 0.75,0.75,0.75);
	P horse_head_obj_rot = ev.polygon_api.rotatey(horse_head_obj_scale,180.0*2.0*3.14159/360.0);
	P horse_head_obj_trans = ev.polygon_api.translate(horse_head_obj_rot, 27.0, -60.0, 17.0);

#if 0
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
#endif

	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -57.0, 15.0);
	PT p3 = ev.point_api.point(15.0, -53.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,8.0);

	P pp = ev.polygon_api.or_elem(pa,pb);
	P p = ev.polygon_api.or_elem(pp, horse_head_obj_trans);
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
	//P cc1 = ev.polygon_api.rotatey(cc01, 90.0*3.14159*2.0/360.0);
	P cc1 = ev.polygon_api.translate(cc, 0.0, 5.0, 0.0);
	P cc2 = ev.polygon_api.anim_endpoints(cc,cc1);
	

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp, pe);
	p = ev.polygon_api.or_elem(p,ppp);
	p = ev.polygon_api.or_elem(p, cc2);
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
	PT p6 = ev.point_api.point(15.0, -61.0, 15.0);
	P pa = ev.polygon_api.cone(30, p1,p2,8.0,9.0);
	P pb = ev.polygon_api.cone(30, p2,p3,2.0,8.0);
	P pc = ev.polygon_api.cone(30, p3,p4,2.0,2.0);
	P pd = ev.polygon_api.cone(30, p4,p5,3.0,2.0);
	P pe = ev.polygon_api.cone(30, p1,p6,0.01,9.0);
	
	PT p7 = ev.point_api.point(15.0, -42.0, 15.0);
	P p_sphere = ev.polygon_api.sphere(p7, 6.0, 5, 5);

	P p = ev.polygon_api.or_elem(pa,pb);
	P pp = ev.polygon_api.or_elem(pc,pd);
	P ppp = ev.polygon_api.or_elem(pp,pe);
	p = ev.polygon_api.or_elem(p,ppp);
	p = ev.polygon_api.or_elem(p,p_sphere);

	color_change(c,p,ev);

	ev.polygon_api.save_model(p, "pawn.obj");

	return p;
      }
    };
#endif
  return ev.polygon_api.empty();
}


char board[] = 
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO";

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
    break;
  case 2:
    color_block = ev.polygon_api.color_faces(block, 0x00ff0000, 0x22ff22ff, 0x44ff44ff, 0x11ff11ff);
    break;
  case 3:
    color_block = ev.polygon_api.color_faces(block, 0xffffff00, 0x88888800, 0xffffff00, 0x88888800);
    break;
  };
  return color_block;
}

void store_board(WorldObj &o, int *array)
{
  for(int x=0;x<8;x++)
    {
      for(int y=0;y<8;y++)
	{
	  array[x+y*8] = o.read_block(x,y);
	}
    }
}
void restore_board(int *array, WorldObj &o)
{
  for(int x=0;x<8;x++)
    {
      for(int y=0;y<8;y++)
	{
	  o.set_block(x,y, array[x+y*8]);
	}
    }
}

Pos towering_new_block(bool left, bool white)
{
  if (left && !white) { Pos p={5,0}; return p; }
  if (left && white) { Pos p={5,7}; return p; }
  if (!left && !white) { Pos p={1,0}; return p; }
  if (!left && white) { Pos p ={1,7}; return p; }
  Pos p = { 0,0 };
  return p;
}
bool towering_needs_update(WorldObj *pieces, int chosen_x, int chosen_y, int cursor_x, int cursor_y)
{
  int piece = pieces->read_block(chosen_x, chosen_y);
  if (piece ==3 || piece==9)
    {
      int delta = chosen_x - cursor_x;
      if (delta == 2 ||delta == -2)
	return true;
    }
  return false;
}
void towering_update(WorldObj *pieces, bool left, bool white)
{
  if (!left && !white)
    {
      pieces->set_block(0,0,12);
      pieces->set_block(2,0,6);
    }
  if (!left && white)
    {
      pieces->set_block(0,7,12);
      pieces->set_block(2,7,0);
    }

  if (left && !white)
    {
      pieces->set_block(7,0,12);
      pieces->set_block(4,0,6);
    }
  if (left && white)
    {
      pieces->set_block(7,7,12);
      pieces->set_block(4,7,0);
    }
}
bool check_towering(WorldObj *pieces, bool left, bool white, bool check)
{
  if (check) return false;
  if (!left && !white)
  if (pieces->read_block(1,0)==12) {
    if (pieces->read_block(2,0)==12)
      {
	return true;
      }
  }
  if (!left && white)
  if (pieces->read_block(1,7)==12) {
    if (pieces->read_block(2,7)==12)
      {
	return true;
      }
  }
  if (left && !white)
    if (pieces->read_block(6,0)==12) {
      if (pieces->read_block(5,0)==12) {
	if (pieces->read_block(4,0)==12) {
	  return true;
	}
      }
    }
  if (left && white)
    if (pieces->read_block(6,7)==12) {
      if (pieces->read_block(5,7)==12) {
	if (pieces->read_block(4,7)==12) {
	  return true;
	}
      }
    }
  return false;
}
bool update_towering_allowed(int x, int y, bool left, bool white)
{
  if (!white && left)
  if (x==0&&y==0) return false; // black tower
  if (white && left)
  if (x==0&&y==7) return false; // white tower
  if (!white && !left)
  if (x==7&&y==0) return false; // black tower
  if (white && !left)
  if (x==7&&y==7) return false; // white tower
  if (!white)
  if (x==3&&y==0) return false; // king black
  if (white)
  if (x==3&&y==7) return false; // king white
  return true;
}

void iter()
{
  //float fr=0.0;
  //float frspeed=0.01;
    env.fr+=env.frspeed;
    if (env.fr>1.0) { env.fr=0.99; env.frspeed = -env.frspeed; }
    if (env.fr<0.0) { env.fr=0.01; env.frspeed = -env.frspeed; }

    //env.ev->fbo_api.bind_fbo(env.fbo);
    // clear frame buffer
    env.ev->mainloop_api.clear_3d();

   

    env.cursor_under = env.board_obj->read_block(env.cursor_x, env.cursor_y);
    env.board_obj->set_block(env.cursor_x, env.cursor_y,2);

    //poly.render();

    for(int x=0;x<8;x++)
      for(int y=0;y<8;y++)
	{
	  env.pieces_obj->set_anim_time(x,y, env.fr);
	}

#if 1
    env.pieces_obj->render();
    env.board_obj->render();
    //env.cursor->render();
#endif
    env.board_obj->set_block(env.cursor_x, env.cursor_y, env.cursor_under);

    //env.ev->fbo_api.bind_screen(800,600);
    //env.ev->mainloop_api.clear();
   
#if 0 
    M m = env.ev->matrix_api.scale(1.0,-1.0,1.0);
    M m2 = env.ev->matrix_api.trans(0.0,600.0,0.0);
    M m3 = env.ev->matrix_api.mult(m,m2);

    M mp = env.ev->matrix_api.identity();
    //env.ev->shader_api.set_var(env.sh2, "in_P", env.ev->matrix_api.identity());
    env.ev->shader_api.set_var(env.sh2, "in_T", m3);

    env.ev->shader_api.use(env.sh2);
    env.bg->render();
    env.ev->shader_api.use(env.sh);
#endif
    //env.ev->mainloop_api.fpscounter();
    // swapbuffers
    env.ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env.ev->mainloop_api.get_event();
    //if (e.type==0x300)
    // std::cout << std::hex << e.ch << std::endl;
    

    PT cursor = e.cursor_pos;
    float xxx, yyy, zzz;
    float x1 = env.ev->point_api.pt_x(cursor);
    float y1 = env.ev->point_api.pt_y(cursor);
    
    

    //x1/=800.0;
    //y1/=600.0;

    //env.board_obj->pick_object1(x1,
    //				y1,
    //				xxx,yyy,zzz);
    //xxx/=200.0;
    //yyy/=200.0;
    //zzz/=200.0;
    //env.cursor->set_pos(xxx,yyy, zzz);
    if (x1>0.01&&y1>0.01 && (e.type==0x400||e.type==0x700))
      {
	xxx = x1;
	yyy = y1;
	zzz = 0.0f;
	//std::cout << xxx << " " << yyy << " " << zzz << std::endl;
	float xx = xxx;
	float yy = yyy;
	float rx_start = 36.0f;
	float rx_end = 768.0f;
	float rx2_start = 189.0f;
	float rx2_end = 614.0f;
	float ry_start = 152.0f;
	float ry_end = 575.0f;
	
	yy-=ry_start;
	yy/=ry_end-ry_start;
	yy*=8;
	
	float rx3_step = (yy/8.0);
	float rx3_start = rx3_step*(rx_start)+(1.0-rx3_step)*rx2_start;
	float rx3_end = rx3_step*(rx_end)+(1.0-rx3_step)*rx2_end;
	
	yy++;
	
	xx-=rx3_start;
	xx/=rx3_end-rx3_start;
	xx*=8;
	xx = 8-xx;
	//std::cout <<"L:" << xx << " " << yy << std::endl;
    
	if (xx<0) xx = 0;
	if (xx>7) xx = 7;
	if (yy<0) yy = 0;
	if (yy>7) yy = 7;
	env.cursor_x = xx;
	env.cursor_y = yy;
      }
    
     if (e.ch==27&&e.type==0x300) { 
#ifndef EMSCRIPTEN
       exit(0); 
#endif
     }
     if (((e.ch==13||e.ch==0x28)&&e.type==0x300) || (env.prevbutton == -1 && e.button == 0) || e.type==0x700)
      {
	int val = env.board_obj->read_block(env.cursor_x, env.cursor_y);
	if (val==3)
	  {
	    env.towering_allowed_white_left &= update_towering_allowed(env.chosen_x, env.chosen_y, true, true);
	    env.towering_allowed_white_right &= update_towering_allowed(env.chosen_x, env.chosen_y, false, true);
	    env.towering_allowed_black_left &= update_towering_allowed(env.chosen_x, env.chosen_y, true, false);
	    env.towering_allowed_black_right &= update_towering_allowed(env.chosen_x, env.chosen_y, false, false);
	    int block = env.pieces_obj->read_block(env.chosen_x, env.chosen_y);
	    if (towering_needs_update(env.pieces_obj, env.chosen_x, env.chosen_y, env.cursor_x, env.cursor_y))
	      {
		bool white= false;
		if (block==3) white=true;
		bool left = false;
		int delta = env.chosen_x - env.cursor_x;
		if (delta==-2) left=true; 
		towering_update(env.pieces_obj, left, white);
	      }
	    env.pieces_obj->set_block(env.chosen_x, env.chosen_y, 12);
	    env.pieces_obj->set_block(env.cursor_x, env.cursor_y, block);
	    restore_board(env.store, *env.board_obj);
	    env.chosen_x = -1;
	    env.chosen_y = -1;
	    do_promotion(env.pieces_obj);
	  }
	else if (env.chosen_x!=-1)
	  {
	    restore_board(env.store, *env.board_obj);
	    env.chosen_x = -1;
	    env.chosen_y = -1;
	  }
	else
	  {
	    if (env.chosen_x!=-1 ||env.chosen_y!=-1)
	      {
		restore_board(env.store, *env.board_obj);
	      }
	    env.chosen_x = env.cursor_x;
	    env.chosen_y = env.cursor_y;
	    std::vector<Pos> vec = possible_moves(*env.pieces_obj, env.cursor_x, env.cursor_y, env);
	    int s = vec.size();
	    store_board(*env.board_obj, env.store);
	    for(int i=0;i<s;i++)
	      {
		Pos p = vec[i];
		env.board_obj->set_block(p.x,p.y,3);
	      }
	  }
      }

    if (((e.ch&0xff)==0x52||(e.ch&0xff)=='w')&&e.type==0x300) // right
      { 
	env.cursor_y--; 
	if (env.cursor_y<0) 
	  env.cursor_y=0; 
      }
    if (((e.ch&0xff)==0x4f||(e.ch&0xff)=='d')&&e.type==0x300) // up
      { 
	env.cursor_x--; 
	if (env.cursor_x<0) env.cursor_x=0; 
      }
    if (((e.ch&0xff)==0x51||(e.ch&0xff)=='s')&&e.type==0x300) // down
      { 
	env.cursor_y++; 
	if (env.cursor_y>7) 
	  env.cursor_y=7; 
      }
    if (((e.ch&0xff)==0x50||(e.ch&0xff)=='a')&&e.type==0x300) // left
      { 
	env.cursor_x++; 
	if (env.cursor_x>7) env.cursor_x=7; 
      }
    env.prevbutton = e.button;
}

int main() {
  GameApi::Env *e = new GameApi::Env;
  EveryApi *ev1 = new EveryApi(*e);
  EveryApi &ev = *ev1;
  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:light:snoise", "colour:light:snoise");
  //SH sh2 = ev.shader_api.get_normal_shader("comb", "comb", "", "texture", "blur");
    //ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  //ev.mainloop_api.init(sh2);

  M m = ev.matrix_api.perspective(70.0, double(800)/600, 10.1, 60000.0);
  ev.shader_api.set_var(sh, "in_P", m);
  M m2 = ev.matrix_api.mult(ev.matrix_api.trans(0.0, 0.0, -1000.0), // -1000
			    ev.matrix_api.scale(1.0, -1.0, 1.0));

  ev.shader_api.set_var(sh, "in_T", m2);

  BM bm = ev.bitmap_api.newintbitmap(board, 8,8, boardmap);
  WorldObj *board_obj = new WorldObj(ev, std::bind(&board_blocks, _1, std::ref(ev)), 4, bm, 30.0, 30.0, sh);
  board_obj->set_scale(2.8,2.8,2.8);
  board_obj->set_pos(-340.0, 30.0, -400.0); // -400
  board_obj->prepare();

  BM bm2 = ev.bitmap_api.newintbitmap(chars, 8,8, charsmap);
  WorldObj *pieces_obj = new WorldObj(ev, std::bind(&chars_blocks, _1, std::ref(ev)), 13, bm2, 30.0, 30.0, sh);
  pieces_obj->set_scale(2.8,2.8,2.8);
  pieces_obj->set_pos(-340.0, 30.0, -400.0);
  M mm1 = ev.matrix_api.xrot(90.0*3.14159/360.0);
  M mm2 = ev.matrix_api.scale(2.0,2.0,2.0);
  M mm3 = ev.matrix_api.trans(0.0, 100.0, -300.0);
  pieces_obj->set_rotation_matrix2(ev.matrix_api.mult(ev.matrix_api.mult(mm1,mm2),mm3));
  pieces_obj->prepare();
  board_obj->set_rotation_matrix2(ev.matrix_api.mult(ev.matrix_api.mult(mm1,mm2),mm3));
  env.store = new int[8*8];
  env.cursor_under = 0;
  env.cursor_x = 0;
  env.cursor_y = 0;
  env.chosen_x = -1;
  env.chosen_y = -1;
  env.ev = ev1;
  env.board_obj = board_obj;
  env.pieces_obj = pieces_obj;
  env.towering_allowed_white_left=true;
  env.towering_allowed_white_right=true;
  env.towering_allowed_black_left=true;
  env.towering_allowed_black_right=true;
#if 0
  env.fbo = ev.fbo_api.create_fbo(3800,3600);
  //env.sh2 = sh2;
  env.sh = sh;
  ev.fbo_api.config_fbo(env.fbo);
  P poly = ev.polygon_api.quad_z(800.0, -800.0,
				 -600.0, 600.0,
				 -200.0);
  P poly2 = ev.polygon_api.texcoord_manual(poly, 
					   1.0, 1.0,
					   0.0, 1.0,
					   0.0, 0.0,
					   1.0, 0.0
					   );
  TXID txid = ev.fbo_api.tex_id(env.fbo);
  env.bg = new PolygonObj(ev, poly2, sh2);
  env.bg->bind_texture(0, txid);
  env.bg->prepare();
#endif
  //PT pt = ev.point_api.point(0.0,0.0,0.0);
  //P p = ev.polygon_api.sphere(pt, 50.0, 20, 20);
  //PolygonObj *obj = new PolygonObj(ev, p, sh);
  //obj->prepare();
  //env.cursor = obj;
  e->free_memory();
#ifndef EMSCRIPTEN
  while(1) {
    iter();
    ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop(iter, 60,1);
#endif


}
