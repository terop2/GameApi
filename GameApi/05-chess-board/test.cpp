#include "GameApi.hh"
#include <iostream>

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
std::vector<Pos> possible_moves(WorldObj &o, int x, int y)
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
  WorldObj board_obj(ev, std::bind(&board_blocks, _1, std::ref(ev)), 4, bm, 30.0, 30.0, sh);
  board_obj.set_scale(2.8,2.8,2.8);
  board_obj.set_pos(-340.0, 30.0, -400.0);
  board_obj.prepare();

  BM bm2 = ev.bitmap_api.newintbitmap(chars, 8,8, charsmap);
  WorldObj pieces_obj(ev, std::bind(&chars_blocks, _1, std::ref(ev)), 13, bm2, 30.0, 30.0, sh);
  pieces_obj.set_scale(2.8,2.8,2.8);
  pieces_obj.set_pos(-340.0, 30.0, -400.0);
  M mm1 = ev.matrix_api.xrot(90.0*3.14159/360.0);
  M mm2 = ev.matrix_api.scale(2.0,2.0,2.0);
  M mm3 = ev.matrix_api.trans(0.0, 100.0, -300.0);
  pieces_obj.set_rotation_matrix2(ev.matrix_api.mult(ev.matrix_api.mult(mm1,mm2),mm3));
  pieces_obj.prepare();
  board_obj.set_rotation_matrix2(ev.matrix_api.mult(ev.matrix_api.mult(mm1,mm2),mm3));

  int cursor_under = 0;
  int cursor_x = 0;
  int cursor_y = 0;
  int *store = new int[8*8];
  int chosen_x = -1;
  int chosen_y = -1;
  float fr=0.0;
  float frspeed=0.01;
  while(1) {
    fr+=frspeed;
    if (fr>1.0) { fr=0.99; frspeed = -frspeed; }
    if (fr<0.0) { fr=0.01; frspeed = -frspeed; }
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    cursor_under = board_obj.read_block(cursor_x, cursor_y);
    board_obj.set_block(cursor_x, cursor_y,2);

    //poly.render();

    for(int x=0;x<8;x++)
      for(int y=0;y<8;y++)
	{
	  pieces_obj.set_anim_time(x,y, fr);
	}

#if 1
    pieces_obj.render();
    board_obj.render();
#endif
    board_obj.set_block(cursor_x, cursor_y, cursor_under);

    


    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    // if (e.type==0x300)
    //  std::cout << std::hex << e.ch << std::endl;
    if (e.ch==27&&e.type==0x300) break;
    if (e.ch==13&&e.type==0x300)
      {
	int val = board_obj.read_block(cursor_x, cursor_y);
	if (val==3)
	  {
	    int block = pieces_obj.read_block(chosen_x, chosen_y);
	    pieces_obj.set_block(chosen_x, chosen_y, 12);
	    pieces_obj.set_block(cursor_x, cursor_y, block);
	    restore_board(store, board_obj);
	    chosen_x = -1;
	    chosen_y = -1;
	  }
	else
	  {
	    if (chosen_x!=-1 ||chosen_y!=-1)
	      {
		restore_board(store, board_obj);
	      }
	    chosen_x = cursor_x;
	    chosen_y = cursor_y;
	    std::vector<Pos> vec = possible_moves(pieces_obj, cursor_x, cursor_y);
	    int s = vec.size();
	    store_board(board_obj, store);
	    for(int i=0;i<s;i++)
	      {
		Pos p = vec[i];
		board_obj.set_block(p.x,p.y,3);
	      }
	  }
      }

    if ((e.ch&0xff)==0x52&&e.type==0x300) // right
      { 
	cursor_y--; 
	if (cursor_y<0) 
	  cursor_y=0; 
      }
    if ((e.ch&0xff)==0x4f&&e.type==0x300) // up
      { 
	cursor_x--; 
	if (cursor_x<0) cursor_x=0; 
      }
    if ((e.ch&0xff)==0x51&&e.type==0x300) // down
      { 
	cursor_y++; 
	if (cursor_y>7) 
	  cursor_y=7; 
      }
    if ((e.ch&0xff)==0x50&&e.type==0x300) // left
      { 
	cursor_x++; 
	if (cursor_x>7) cursor_x=7; 
      }
  }



}
