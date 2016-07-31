#include "GameApi.hh"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <functional>
#include <sstream>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace GameApi;

char world[] = \
  "+---------+---------+"
  "|....%...T|.P...Y...|"
  "|..+---+..|...L....S|"
  "|.Q|...|...KKK..I...|"
  "|..+.*.+..|KKK.....G|"
  "|&..W....#|KKK..H...|"
  "+--.....--+---------+";

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
  case 'Q': return 8;
  case 'W': return 9;
  case 'T': return 10;
  case 'P': return 11;
  case 'H': return 12;
  case 'S': return 13;
  case 'K': return 14;
  case 'G': return 15;
  case 'Y': return 16;
  case 'L': return 17;
  case 'I': return 18;
  };
}

P points_func(int i, float x, float y, float z, unsigned int color, EveryApi &ev)
{
  return ev.polygon_api.cube(x,x+1.0,y,y+1.0,z,z+1.0);
}

P points_func2(int i, float x, float y, float z, unsigned int color, EveryApi &ev)
{
  if (color==0) return ev.polygon_api.empty();
  P p = ev.polygon_api.cube(x,x+1.0,y,y+1.0,z,z+1.0);
  return ev.polygon_api.color_faces(p, color, color, color, color);
}

float float_bitmap(int x, int y)
{
  x-=40;
  y-=40;
  float xx = x;
  float yy = y;
  float k = xx*xx+yy*yy;
  float k2 = sqrt(k);
  k2/=40;
  k2 = 1.0-k2;
  return k2;
}

P pieces2(unsigned int i, EveryApi &ev)
{
    P p2a = ev.polygon_api.cube(0.0, 100.0, 80.0, 81.0, 0.0, 100.0);
    P pk = ev.polygon_api.color_faces(p2a, 0xafaa8888, 0xafaa4444, 0xafaa2222, 0xafaaaaaa);
    //return ev.polygon_api.empty();
     return pk;
}

P pieces3(unsigned int i, EveryApi &ev)
{
    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P pk = ev.polygon_api.color_faces(p1a, 0x01888888, 0x01444444, 0x01222222, 0x01aaaaaa);
    //return ev.polygon_api.empty();
     return pk;
}

P line_func(int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor, EveryApi &ev)
{
  P p = ev.polygon_api.cube(sx,sx+1.0,sy,sy+1.0,sz,sz+1.0);
  P p2 = ev.polygon_api.cube(ex,ex+1.0,ey,ey+1.0,ez,ez+1.0);
  P pp = ev.polygon_api.or_elem(p,p2);
  PT pt1 = ev.point_api.point(sx,sy,sz);
  PT pt2 = ev.point_api.point(ex,ey,ez);
  PT pt3 = ev.point_api.point(sx+1.0,sy+1.0,sz+1.0);
  P pl = ev.polygon_api.triangle(pt1, pt2, pt3);
  P pp1 = ev.polygon_api.color_faces(pl, 0xffcccccc, 0xffff8844, 0xff2288ff, 0xffff8844);
  P pp2 = ev.polygon_api.or_elem(pp, pp1);
  return pp2;
}

P poly_func(int face,
	    PT pp1, PT pp2, PT pp3, PT pp4,
	    EveryApi &ev)
{
  float p1_x = ev.point_api.pt_x(pp1);
  float p1_y = ev.point_api.pt_y(pp1);
  float p1_z = ev.point_api.pt_z(pp1);

  float p2_x = ev.point_api.pt_x(pp2);
  float p2_y = ev.point_api.pt_y(pp2);
  float p2_z = ev.point_api.pt_z(pp2);

  float p3_x = ev.point_api.pt_x(pp3);
  float p3_y = ev.point_api.pt_y(pp3);
  float p3_z = ev.point_api.pt_z(pp3);

  float p4_x = ev.point_api.pt_x(pp4);
  float p4_y = ev.point_api.pt_y(pp4);
  float p4_z = ev.point_api.pt_z(pp4);


  float m_x = (p1_x+p2_x+p3_x+p4_x)/4;
  float m_y = (p1_y+p2_y+p3_y+p4_y)/4;
  float m_z = (p1_z+p2_z+p3_z+p4_z)/4;

  float dx = p2_x-p1_x;
  float dy = p2_y-p1_y;
  float dz = p2_z-p1_z;
  
  float ddx = p3_x-p1_x;
  float ddy = p3_y-p1_y;
  float ddz = p3_z-p1_z;
  
  float cross_x = dy*ddz-dz*ddy;
  float cross_y = dx*ddz-dz*ddx;
  float cross_z = dx*ddy-dy*ddx;

  float dist_dx = sqrt(dx*dx+dy*dy+dz*dz);
  float dist_ddx = sqrt(ddx*ddx+ddy*ddy+ddz*ddz);
  cross_x/=dist_dx;
  cross_x/=dist_ddx;
  cross_y/=dist_dx;
  cross_y/=dist_ddx;
  cross_z/=dist_dx;
  cross_z/=dist_ddx;
  
  cross_x*=5.0;
  cross_y*=5.0;
  cross_z*=5.0;

  cross_x+=m_x;
  cross_y+=m_y;
  cross_z+=m_z;

  PT pt1 = ev.point_api.point(p1_x,p1_y,p1_z);
  PT pt2 = ev.point_api.point(p2_x,p2_y,p2_z);

  PT pt3 = ev.point_api.point(p3_x,p3_y,p3_z);
  PT pt4 = ev.point_api.point(p4_x,p4_y,p4_z);

  PT mid = ev.point_api.point(cross_x,cross_y,cross_z);

  P p1 = ev.polygon_api.triangle(pt1,pt2,mid);
  P p2 = ev.polygon_api.triangle(pt2,pt3,mid);
  P p3 = ev.polygon_api.triangle(pt3,pt4,mid);
  P p4 = ev.polygon_api.triangle(pt4,pt1,mid);

  std::vector<P> vec;
  vec.push_back(p1);
  vec.push_back(p2);
  vec.push_back(p3);
  vec.push_back(p4);
  
  return ev.polygon_api.or_array(&vec[0], vec.size());

}

//using std::placeholders::_10;
//using std::placeholders::_11;
//using std::placeholders::_12;
//using std::placeholders::_13;

P cube_parts(int face,
	     PT pt1, PT pt2, PT pt3, PT pt4,
	     EveryApi &ev)
{
  float p1_x = ev.point_api.pt_x(pt1);
  float p1_y = ev.point_api.pt_y(pt1);
  float p1_z = ev.point_api.pt_z(pt1);

  float p2_x = ev.point_api.pt_x(pt2);
  float p2_y = ev.point_api.pt_y(pt2);
  float p2_z = ev.point_api.pt_z(pt2);

  float p3_x = ev.point_api.pt_x(pt3);
  float p3_y = ev.point_api.pt_y(pt3);
  float p3_z = ev.point_api.pt_z(pt3);

  float p4_x = ev.point_api.pt_x(pt4);
  float p4_y = ev.point_api.pt_y(pt4);
  float p4_z = ev.point_api.pt_z(pt4);


  float mid_x = (p1_x+p2_x+p3_x+p4_x)/4;
  float mid_y = (p1_y+p2_y+p3_y+p4_y)/4;
  float mid_z = (p1_z+p2_z+p3_z+p4_z)/4;

  float dx = p2_x-p1_x;
  float dy = p2_y-p1_y;
  float dz = p2_z-p1_z;
  
  float ddx = p3_x-p1_x;
  float ddy = p3_y-p1_y;
  float ddz = p3_z-p1_z;
  
  float cross_x = dy*ddz-dz*ddy;
  float cross_y = dx*ddz-dz*ddx;
  float cross_z = dx*ddy-dy*ddx;

  float dist_dx = sqrt(dx*dx+dy*dy+dz*dz);
  float dist_ddx = sqrt(ddx*ddx+ddy*ddy+ddz*ddz);
  cross_x/=dist_dx;
  cross_x/=dist_ddx;
  cross_y/=dist_dx;
  cross_y/=dist_ddx;
  cross_z/=dist_dx;
  cross_z/=dist_ddx;
  
  cross_x*=5.0;
  cross_y*=5.0;
  cross_z*=5.0;

  cross_x+=mid_x;
  cross_y+=mid_y;
  cross_z+=mid_z;
  
  PT mid = ev.point_api.point(cross_x, cross_y, cross_z);

  P p1 = ev.polygon_api.triangle(pt1,pt2,mid);
  P p2 = ev.polygon_api.triangle(pt2,pt3,mid);
  P p3 = ev.polygon_api.triangle(pt3,pt4,mid);
  P p4 = ev.polygon_api.triangle(pt4,pt1,mid);

  std::vector<P> vec;
  vec.push_back(p1);
  vec.push_back(p2);
  vec.push_back(p3);
  vec.push_back(p4);
  
  return ev.polygon_api.or_array(&vec[0], vec.size());
}

P cube(float x1, float x2, 
       float y1, float y2,
       float z1, float z2, EveryApi &ev)
{
  P p = ev.polygon_api.cube(x1,x2, y1,y2, z1,z2);
  P p2 = ev.polygon_api.from_polygon(p, [&ev](float x, PT p1, PT p2, PT p3, PT p4) { return cube_parts(x,p1,p2,p3,p4, ev); } );

  //    std::bind(&cube_parts, _1,
  //						  _2,_3,_4,_5, std::ref(ev)));
  return p2;
}

P heightmap_cube(float val, EveryApi &ev)
{

  float s = 10.0;
  P p1 = ev.polygon_api.cube(0.0,0.5,
			     0.0, val*s,
			     0.0, 0.5);
  P p1a = ev.polygon_api.color_faces(p1, 0xff000000, 0xff222222, 0xff111111, 0xff333333);

  P p2 = ev.polygon_api.cube(0.0, 0.5,
			     val*s, s*val+0.2,
			     0.0, 0.5);
  int val2 = val*255.0;
  int color = val2 + (val2<<8) + (val2<<16) + (val2<<24);
  P p2a = ev.polygon_api.color_faces(p2, color, color, color, color);
  
  return ev.polygon_api.or_elem(p1a,p2a);
}

struct Models
{
  P teapot;
  P lucy;
  P sponza;
};

P pieces(unsigned int i, EveryApi &ev, Models &m)
{
  switch(i) {
  case 18:
    {
#if 0
      P p = m.sponza;
      P p2 = ev.polygon_api.scale(p, 5.0,5.0,5.0);
      P p2a = ev.polygon_api.translate(p2, 0.0, 15.0, 0.0);
      P p3 = ev.polygon_api.recalculate_normals(p2a);
      V v = ev.vector_api.vector(10,-10,10);
      P p4 = ev.polygon_api.color_lambert(p3, 0xffff8844, v);
      //P p4 = ev.polygon_api.color_from_normals(p3);
      P p5 = ev.polygon_api.color_grayscale(p4);
      //P p6 = ev.polygon_api.color(p5, 0xffff8844);
      //P p7 = ev.polygon_api.mix_color(p5,p6, 0.01);
      return p5;
#endif
    }
  case 17:
    {
      P p2 = ev.polygon_api.empty(); //m.lucy;
#if 0

      P p3 = ev.polygon_api.color_from_normals(p);
      P p4 = ev.polygon_api.color_range(p3, 0xffffffff, 0xff888888);

      PT pos = ev.point_api.point(0.0,0.0,0.0);
      V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
      LI lines = ev.lines_api.render_slice_2d(p, pos, u_x, u_y);
      PT center = ev.point_api.point(0.0,0.0,0.0);
      P p0 = ev.polygon_api.dist_from_lines(lines, 0.2, 2.0, center);
      P p0a = ev.polygon_api.color(p0, 0xffffffff);
      //P p0b = ev.polygon_api.translate(p0a, 0.0, 0.0, -40.0);
      P p1 = ev.polygon_api.or_elem(p4, p0a);
      P p2 = ev.polygon_api.scale(p1, 5.0,5.0,5.0);
#endif
      //P p4 = ev.polygon_api.empty();
      return p2;
    }
  case 16:
    {
#if 1
      PT center = ev.point_api.point(50.0, 40.0, 50.0);
      V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
      V uu_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V uu_y = ev.vector_api.vector(0.0, 0.0, 1.0);
      P p = ev.polygon_api.torus(30,20, center, u_x, u_y, 30.0, uu_x, uu_y, 10.0);
      LI lines = ev.lines_api.from_polygon(p);
      
     P p2 = ev.polygon_api.from_lines(lines,
				      [&ev](int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor){ return line_func(i,sx,sy,sz,ex,ey,ez,scolor,ecolor,ev); });

				      //				      std::bind(&line_func, _1,_2,_3,_4,_5,_6,_7, _8,_9, std::ref(ev)));
#endif
     //P p2 = ev.polygon_api.empty();
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

     return p2a2;

    }
  case 15:
    {
      PT center = ev.point_api.point(50.0, 40.0, 50.0);
      V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
      V uu_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V uu_y = ev.vector_api.vector(0.0, 0.0, 1.0);
      P p = ev.polygon_api.torus(40,40, center, u_x, u_y, 30.0, uu_x, uu_y, 10.0);
      //P p2 = ev.polygon_api.color_faces(p, 0x000000ff, 0x222222ff, 0x111111ff, 0x333333ff);
#if 1
      P pp = ev.polygon_api.from_polygon(p, [&ev](int i, PT p1, PT p2, PT p3, PT p4) { return poly_func(i,p1,p2,p3,p4,ev); });

	//					 std::bind(&poly_func, _1, 
	//					      _2,_3,_4,_5
	//					      _5,_6,_7,
	//					      _8,_9,_10,
	//					      _11,_12,_13, std::ref(ev)));
#endif
      P p2 = ev.polygon_api.color_from_normals(pp);
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
      
    }
  case 14:
    {

             return ev.polygon_api.empty();
#if 0
      BM bm = ev.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 200, 200, 256);
      FB fb = ev.float_bitmap_api.from_green(bm);
      //P p = ev.polygon_api.heightmap(bm, PolygonApi::EQuad, 0.0, 100.0,
      //				     20.0, 0.0, 
      //				     0.0, 100.0);
      P p = ev.polygon_api.heightmap(fb, std::bind(&heightmap_cube, _1, std::ref(ev)), 0.5,0.5);
      //P p = ev.polygon_api.empty();
      //P p2 = ev.polygon_api.color_faces(p, 0x112233ff, 0x224466ff, 0x336699ff, 0x4488aaff);
      // P pp = ev.polygon_api.rotatex(p,90.0*3.14159*2.0/360.0);
      //P p = ev.polygon_api.empty();
#endif
      //return p;
    }
  case 13:
    {
      PT p1 = ev.point_api.point(50.0, 0.0, 50.0);
      PT p2 = ev.point_api.point(50.0, 80.0, 50.0);
      P p = ev.polygon_api.cone(50, p1, p2, 50.0, 30.0);
      P p3 = ev.polygon_api.color_faces(p, 0xffffffff, 0xffaaaaaa, 0xffcccccc, 0xff888888);
  P p2a1 = ev.polygon_api.recalculate_normals(p3);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
    }
  case 12:
    {
      //P p = m.teapot;
      P p = ev.polygon_api.empty();
      P p2 = ev.polygon_api.scale(p, 20.0,20.0,20.0);
#if 0
      P p2a = ev.polygon_api.from_polygon(p2, std::bind(&poly_func, _1,  _2,_3,_4,
						      _5,_6,_7,
						      _8,_9,_10,
						      _11,_12,_13, std::ref(ev)));
#endif
      //P p3 = ev.polygon_api.color_faces(p2, 0xffffffff, 0xaaaaaaff, 0xccccccff, 0x888888ff);
      
      P p3 = ev.polygon_api.recalculate_normals(p2);
      P p4 = ev.polygon_api.color_from_normals(p3);
      return p4;
    }
  case 11:
    {
      std::vector<P> vec;
      for(int x=0;x<4;x++)
	{
	  for(int y=0;y<4;y++)
	    {
	      float pos_x = 100.0*x/4;
	      float pos_y = 100.0*y/4;
	      vec.push_back(ev.polygon_api.cube(pos_x,pos_x+20.0,
						0.0, 20.0,
						pos_y,pos_y+20.0
						));
	    }
	}
      for(int x=0;x<3;x++)
	{
	  for(int y=0;y<3;y++)
	    {
	      float pos_x = 100.0*x/4+100.0/8;
	      float pos_y = 100.0*y/4+100.0/8;
	      vec.push_back(ev.polygon_api.cube(pos_x,pos_x+20.0,
						20.0, 40.0,
						pos_y,pos_y+20.0
						));
	    }
	}
      for(int x=0;x<2;x++)
	{
	  for(int y=0;y<2;y++)
	    {
	      float pos_x = 100.0*x/4+100.0/4;
	      float pos_y = 100.0*y/4+100.0/4;
	      vec.push_back(ev.polygon_api.cube(pos_x,pos_x+20.0,
						40.0, 60.0,
						pos_y,pos_y+20.0
						));
	    }
	}
      float pos_x = 100.0/2-10.0;
      float pos_y = 100.0/2-10.0;
      vec.push_back(ev.polygon_api.cube(pos_x,pos_x+20.0,
					60.0, 80.0,
					pos_y, pos_y+20.0));
      P p = ev.polygon_api.or_array(&vec[0], 4*4+3*3+2*2+1);
      P p2 = ev.polygon_api.color_faces(p, 0xff000000, 0xff222222, 0xff111111, 0xff333333);

  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);
      return p2a2;
    }
  case 10:
    {
      PT pt = ev.point_api.point(40.0, 40.0, 40.0);
      P p = ev.polygon_api.sphere(pt, 40.0, 20,20);
#if 1
      P p2 = ev.polygon_api.from_polygon(p,
					 [&ev](int i, PT pp1, PT pp2, PT pp3, PT pp4) { return poly_func(i,pp1,pp2,pp3,pp4,ev); });

      //					 std::bind(&poly_func, _1,  _2,_3,_4,
      //					      _5,_6,_7,
      //					      _8,_9,_10,
      //					      _11,_12,_13, std::ref(ev)));
#endif
      P p3 = ev.polygon_api.color_faces(p2, 0xff888888, 0xff444444, 0xff222222, 0xff666666);
  P p2a1 = ev.polygon_api.recalculate_normals(p3);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

     return p2a2;
    }
  case 9:
    {
#if 1
      PT pt = ev.point_api.point(40.0, 40.0, 40.0);
      P p = ev.polygon_api.sphere(pt, 40.0, 20,20);
      LI li = ev.lines_api.from_polygon(p);
      P p2 = ev.polygon_api.from_lines(li, [&ev](int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor) { return line_func(i,sx,sy,sz,ex,ey,ez,scolor,ecolor,ev); });

      //				       std::bind(&line_func, _1,_2,_3,_4,_5,_6,_7, _8,_9, std::ref(ev)));
#endif
      //      P p2 = ev.polygon_api.empty();
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
    }
  case 8:
    {
#if 0
      BB bg = ev.bool_bitmap_api.empty(80,80);
      BB circle1 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 40.0);
      BB circle2 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 30.0);
      BB circle3 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 20.0);
      BB circle4 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 10.0);
      BM circle_bm1 = ev.bool_bitmap_api.to_bitmap(circle1, 255,255,255,255, 0,0,0,0);
      BM circle_bm2 = ev.bool_bitmap_api.to_bitmap(circle2, 200,200,200,255, 0,0,0,0);
      BM circle_bm3 = ev.bool_bitmap_api.to_bitmap(circle3, 155,155,155,255, 0,0,0,0);
      BM circle_bm4 = ev.bool_bitmap_api.to_bitmap(circle4, 100,100,100,255, 0,0,0,0);
      BM c1 = ev.bitmap_api.blitbitmap(circle_bm1, circle_bm2, 0,0);
      BM c2 = ev.bitmap_api.blitbitmap(c1, circle_bm3, 0,0);
      BM c3 = ev.bitmap_api.blitbitmap(c2, circle_bm4, 0,0);
      
      FB circle_fb = ev.float_bitmap_api.function(&float_bitmap, 80,80);
      PT pos = ev.point_api.point(0.0,0.0,0.0);
      V u_x = ev.vector_api.vector(80.0,0.0,0.0);
      V u_y = ev.vector_api.vector(0.0,80.0,0.0);
      V u_z = ev.vector_api.vector(0.0,0.0,36.0);
      PTS heightmap = ev.points_api.heightmap(c3, circle_fb, pos, u_x, u_y, u_z, 100,100);
      
      P p2 = ev.polygon_api.from_points(heightmap,std::bind(points_func2, _1, _2, _3, _4, _5, std::ref(ev)));
#endif
      P p2 = ev.polygon_api.empty();
      return p2;
    }
  case 7:
    {
#if 1
      PT pt = ev.point_api.point(50.0,40.0,50.0);
      O o = ev.volume_api.sphere(pt, 40.0);
      FO fo = ev.float_volume_api.from_volume(o, 0.0, 1.0);
      PTS pts = ev.points_api.from_float_volume(fo, 1000, 0.0, 0.0, 0.0, 100.0, 100.0, 100.0);
      P p2 = ev.polygon_api.from_points(pts, [&ev](int i, float x, float y, float z, unsigned int color) { return points_func(i,x,y,z,color,ev); }); //std::bind(points_func, _1, _2, _3, _4, _5, std::ref(ev)));
#endif
					//P p2 = ev.polygon_api.empty();
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
    }
    
 case 6:
   {
     P p1 = ev.polygon_api.cube(30.0,70.0,30.0,70.0,30.0,70.0);
     P p11 = ev.polygon_api.rotatex(p1, 45.0*3.14159*2.0/360.0);
     P p2 = ev.polygon_api.translate(p11,0.0,40.0,0.0);
     P p3 = ev.polygon_api.color_faces(p2, 0xff888888, 0xff444444, 0xff222222, 0xffaaaaaa);
  P p2a1 = ev.polygon_api.recalculate_normals(p3);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);
     return p2a2;
   }
  case 4:
    {
      PT pt = ev.point_api.point(50.0,40.0,50.0);
      P p2 = ev.polygon_api.sphere(pt, 40.0, 40,40);
      P p3 = ev.polygon_api.color_from_normals(p2);
  P p2a1 = ev.polygon_api.recalculate_normals(p3);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
    }
  case 3:
    {
      P p = cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0, ev);
      P pa = cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0, ev);
      P pp = ev.polygon_api.or_elem(p,pa);
      P p2 = ev.polygon_api.color_faces(pp, 0xffff8844, 0xff884422, 0xffff8844, 0xff884422); 
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);
      return p2a2;
    }
  case 2:
    {
      P p = cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0, ev);
      P p2 = ev.polygon_api.color_faces(p, 0xffff8844, 0xff884422, 0xffff8844, 0xff884422);
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);
      return p2a2;
    }
    
  case 1:
    {
      P p = cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0, ev);
      P p2 = ev.polygon_api.color_faces(p, 0xffff8844, 0xff884422, 0xffff8844, 0xff884422);
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

      return p2a2;
    }
  case 0:
    break;
  };
  return ev.polygon_api.empty();
}

unsigned int color_change_func(unsigned int orig, int face, int point)
{
  unsigned int color = orig;
  unsigned int color_a = color & 0xff000000;
  unsigned int color_r = color & 0x00ff0000;
  unsigned int color_g = color & 0x0000ff00;
  unsigned int color_b = color & 0x000000ff;
  
  color_r /= 4;
  color_g /= 4;
  color_b /= 4;
  unsigned int color2 = color_r | color_g | color_b |color_a;
  unsigned int color3 = color2 & 0xff3f3f3f;
  return color3;
}

struct Envi
{
  EveryApi *ev;
  float pos_x, pos_y;
  float rot_y;
  float speed;
  float rot_speed;
  float speed_x, speed_y;
  int frame;
  float mouse_x, mouse_y;
  SH sh;
  SH sh2;
  SH sh3;
  PolygonObj *poly;
  WorldObj *poly_world;
  PolygonObj *reflect_obj;
  PolygonObj *poly2;
  PolygonObj *mirror;
  PolygonObj *fbo_bg;
  PolygonObj *monster;
  PolygonObj *lines_obj;
  FBO fbo;
  FBO mirror_fbo;
  float jump_start_frame;
  float jump_duration;
  float y_delta;
  bool logo_shown=true;
  int counter = 0;
  bool forward=false, backward=false,left=false,right=false;
} env;

void iter()
{
  if (env.logo_shown)
    {
#ifdef EMSCRIPTEN
      env.counter++;
      if (env.counter==1) {   env.ev->mainloop_api.reset_time(); }
#endif

      bool b = env.ev->mainloop_api.logo_iter();
      if (b) { 
	env.logo_shown = false; 
      }
      return;
    }


   env.frame++;
   env.ev->shader_api.set_var(env.sh, "in_time",float(env.frame)*0.01f);
#if 1
    env.ev->shader_api.set_var(env.sh2, "in_time",float(env.frame)*0.01f);
    env.ev->shader_api.set_var(env.sh3, "in_time",float(env.frame)*0.01f);
#endif
    env.ev->shader_api.set_var(env.sh, "eye_position", env.pos_x, -80.0, env.pos_y);
    env.ev->shader_api.use(env.sh);
    env.ev->shader_api.set_var(env.sh, "lightpos", -env.pos_x/3.0, -0.0, -env.pos_y/2.0);
    //std::cout << pos_x << " " << pos_y << std::endl;
    // clear frame buffer
#if 1
    env.ev->fbo_api.bind_fbo(env.mirror_fbo);
    env.ev->mainloop_api.clear_3d();
#endif
    M a_m = env.ev->matrix_api.yrot(env.rot_y+3.14159);
    M a_m2 = env.ev->matrix_api.trans(0.0,0.0,400.0);
    M a_m3 = env.ev->matrix_api.trans(0.0,0.0,-400.0);
    M a_mm = env.ev->matrix_api.mult(env.ev->matrix_api.mult(a_m3,a_m),a_m2);
#if 1
    env.poly->set_rotation_matrix2(a_mm);
    env.poly->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.poly->render();
#endif
    //env.poly_world->set_rotation_matrix2(a_mm);
    //env.poly_world->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    //env.poly_world->render();

    //env.reflect_obj->set_rotation_matrix2(a_mm);
    //env.reflect_obj->set_pos(env.pos_x, -75.0+env.y_delta, env.pos_y);
    //env.reflect_obj->render();


    //env.ev->mainloop_api.transparency(true);
    env.poly2->set_rotation_matrix2(a_mm);
    env.poly2->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.poly2->render();
    //env.ev->mainloop_api.transparency(false);

    M m = env.ev->matrix_api.yrot(env.rot_y);
    M m2 = env.ev->matrix_api.trans(0.0,0.0,400.0);
    M m3 = env.ev->matrix_api.trans(0.0,0.0,-400.0);
    M mm = env.ev->matrix_api.mult(env.ev->matrix_api.mult(m3,m),m2);

    env.monster->set_rotation_matrix2(mm);
    env.monster->set_pos(env.pos_x, -80.0-env.y_delta, env.pos_y-300.0);
    env.monster->render();


    M a_ms = env.ev->matrix_api.yrot(env.frame/30.0);
    M a_ms2 = env.ev->matrix_api.trans(500.0*3.0,0.0,600.0*3.0);
    M a_msa = env.ev->matrix_api.mult(a_ms, a_ms2);
    env.lines_obj->set_rotation_matrix(a_msa);
    env.lines_obj->set_rotation_matrix2(a_mm);
    env.lines_obj->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.lines_obj->render();



#if 1
    env.ev->fbo_api.bind_fbo(env.fbo);
#endif
    env.ev->mainloop_api.clear_3d();

    M ms = env.ev->matrix_api.yrot(env.frame/30.0);
    M ms2 = env.ev->matrix_api.trans(500.0*3.0,0.0,600.0*3.0);
    M msa = env.ev->matrix_api.mult(ms, ms2);
    env.lines_obj->set_rotation_matrix(msa);
    env.lines_obj->set_rotation_matrix2(mm);
    env.lines_obj->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.lines_obj->render();


    //poly.set_rotation_matrix(ev.matrix_api.xrot(frame));
#if 1 
   env.poly->set_rotation_matrix2(mm);
    env.poly->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.poly->render();
#endif
    env.poly_world->set_rotation_matrix2(mm);
    env.poly_world->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.poly_world->render();


    //shadow_obj.set_rotation_matrix2(mm);
    //shadow_obj.set_pos(pos_x, -75.0, pos_y);
    //shadow_obj.render();

    // env.reflect_obj->set_rotation_matrix2(mm);
    //env.reflect_obj->set_pos(env.pos_x, -75.0+env.y_delta, env.pos_y);
    //env.reflect_obj->render();


    env.ev->mainloop_api.transparency(true);
    env.poly2->set_rotation_matrix2(mm);
    env.poly2->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.poly2->render();
    env.ev->mainloop_api.transparency(false);

    env.mirror->set_rotation_matrix2(mm);
    env.mirror->set_pos(env.pos_x, -80.0+env.y_delta, env.pos_y);
    env.mirror->render();
    //ev.mainloop_api.depth_test(true);
    //sphere.set_pos(0.0,0.0,400.0);
    //sphere.render();
#if 1                               
    env.ev->fbo_api.bind_screen(800,600);

    env.ev->mainloop_api.clear_3d();

    env.ev->shader_api.use(env.sh2);
    env.fbo_bg->set_pos(-800,-600,0.0);
    env.fbo_bg->set_scale(2.0,2.0,1.0);
    env.fbo_bg->render();
#endif

#if 0
    BM bm = ev.mainloop_api.screenshot();
    screenshot_images.push_back(bm);
#endif
#if 0
    BM bm = ev.mainloop_api.screenshot();
#endif
    env.ev->mainloop_api.fpscounter();
    // swapbuffers
    env.ev->mainloop_api.swapbuffers();

    // handle esc event
    //MainLoopApi::Event e = env.ev->mainloop_api.get_event();
    MainLoopApi::Event e;
    while((e = env.ev->mainloop_api.get_event()).last==true)
      {
    // if (e.type==0x300)
    //std::cout << e.type << " " << e.ch << std::endl;

    env.mouse_x = env.ev->point_api.pt_x(e.cursor_pos);
    env.mouse_y = env.ev->point_api.pt_y(e.cursor_pos);
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
    

    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { env.forward = true; }
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x301) { env.forward = false; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) { env.backward = true; }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x301) { env.backward = false; }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env.left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x301) { env.left=false; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env.right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x301) { env.right=false; }
    if ((e.ch==' '||e.ch==44)&&env.frame+env.jump_duration>env.jump_start_frame && e.type==0x300)
      {
	//std::cout << "Space" << std::endl;
	env.jump_start_frame = env.frame;
      }
      }
    if (env.frame>=env.jump_start_frame && env.frame<env.jump_start_frame+env.jump_duration)
      {
	float delta_time = env.frame - env.jump_start_frame;
	delta_time /= env.jump_duration;
	delta_time *= 3.14159;
	env.y_delta = -60.0*sin(delta_time);
	//std::cout << "Space2" << env.y_delta << std::endl;
      }
#if 0
    if (e.ch=='s') { 
      for(int i=0;i<screenshot_images.size();i++)
	{
	  std::stringstream ss; 
	  ss << "SCR";
	  ss.width(3);
	  ss.fill('0');
	  ss << i;
	  ss << ".png";
	  std::string s = ss.str();
	  ev.bitmap_api.savebitmap(screenshot_images[i], s);
	}
    }
#endif
    float delta = env.ev->mainloop_api.get_delta_time();
    if (env.forward)
      {
	env.pos_y+=delta*env.speed_y; env.pos_x+=delta*env.speed_x;
      }
    if (env.backward)
      {
	env.pos_y-=delta*env.speed_y; env.pos_x-=delta*env.speed_x;
      }
    if (env.left)
      {
	env.rot_y -= delta*env.rot_speed;
      }
    if (env.right)
      {
	env.rot_y += delta*env.rot_speed;
      }

    env.speed_x = env.speed*cos(env.rot_y+3.14159/2.0);
    env.speed_y = env.speed*sin(env.rot_y+3.14159/2.0);
}

int main(int argc, char *argv[]) {
  srand(1);
  Env e;
  EveryApi ev(e);

  env.ev = &ev;

  // initialize xindow
  ev.mainloop_api.init_window();

  // shader initialization
  //ev.shader_api.load("Shader.txt");
  ev.shader_api.load_default();
  //SH sh = ev.shader_api.get_shader("comb", "comb", "", "colour:snoise:snoise:point_light:light", "colour:light:light:bands:snoise:snoise:point_light");
  SH sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:snoise:point_light:light", "colour:light:light:bands:snoise:snoise",false);
  //SH sh = ev.shader_api.get_shader("comb", "comb", "", "passall", "diffuse:specular:ambient",false);

  SH sh2 = ev.shader_api.get_normal_shader("comb", "comb", "", "blur", "blur",false);
  SH sh3 = ev.shader_api.get_normal_shader("comb", "comb", "", "texture:light:snoise", "texture:light:snoise",false);
  SH sh4 = ev.shader_api.get_normal_shader("empty", "empty", "", "", "", false);
#if 0
  ev.shader_api.bind_attrib(sh, 0, "in_Position");
  ev.shader_api.bind_attrib(sh, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh, 2, "in_Color");
  ev.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  ev.shader_api.bind_frag(sh, 0, "out_Color");
  ev.shader_api.link(sh);
  ev.shader_api.use(sh);
  ev.shader_api.set_default_projection(sh, "in_P");

#if 1
  ev.shader_api.bind_attrib(sh2, 0, "in_Position");
  ev.shader_api.bind_attrib(sh2, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh2, 2, "in_Color");
  ev.shader_api.bind_attrib(sh2, 3, "in_TexCoord");
  ev.shader_api.link(sh2);
  ev.shader_api.use(sh2);
  ev.shader_api.set_default_projection(sh2, "in_P");
#endif

#if 1
  ev.shader_api.bind_attrib(sh3, 0, "in_Position");
  ev.shader_api.bind_attrib(sh3, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh3, 2, "in_Color");
  ev.shader_api.bind_attrib(sh3, 3, "in_TexCoord");
  ev.shader_api.link(sh3);
  ev.shader_api.use(sh3);
  ev.shader_api.set_default_projection(sh3, "in_P");
#endif
#if 1
  ev.shader_api.bind_attrib(sh4, 0, "in_Position");
  ev.shader_api.bind_attrib(sh4, 1, "in_Normal");
  ev.shader_api.bind_attrib(sh4, 2, "in_Color");
  ev.shader_api.bind_attrib(sh4, 3, "in_TexCoord");
  ev.shader_api.link(sh4);
  ev.shader_api.use(sh4);
  ev.shader_api.set_default_projection(sh4, "in_P");
#endif
#endif

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
#if 1
  ev.mainloop_api.init_3d(sh2);
  ev.mainloop_api.init_3d(sh3);
  ev.mainloop_api.init_3d(sh4);
#endif

  if (argc==2 && std::string(argv[1])=="--generate-logo")
    {
      std::cout << "Generating Logo" << std::endl;
      ev.mainloop_api.save_logo(ev);
      exit(0);
    }


  Models m;
#if 0
  m.teapot = ev.polygon_api.load_model("./teapot.obj", 0);
  m.lucy = ev.polygon_api.load_model("./lucy.obj", 1);
  P pk = ev.polygon_api.empty();
#endif
#if 0
  for(int i=0;i<60;i++)
    {
      pk = ev.polygon_api.or_elem(pk, ev.polygon_api.load_model("./sponza.obj", i));
    }
  
  m.sponza = pk;
#endif

  BM bm = ev.bitmap_api.newintbitmap(world, 21, 7, func);
  P p = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev), std::ref(m)), bm   , 100.0, 100.0, 19);
  P p2 = ev.polygon_api.world_from_bitmap(std::bind(&pieces2, _1, std::ref(ev)), bm, 100.0, 100.0, 19);
  P p3 = ev.polygon_api.world_from_bitmap(std::bind(&pieces3, _1, std::ref(ev)), bm, 100.0, 100.0, 19);
  P p3a = ev.polygon_api.or_elem(p,p3);
  P p2a = ev.polygon_api.or_elem(p,p2);
  P p2a1 = ev.polygon_api.recalculate_normals(p2);
  P p2a2 = ev.polygon_api.texcoord_manual(p2a1, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);

  WorldObj poly_world(ev, std::bind(&pieces, _1, std::ref(ev), std::ref(m)), 19, bm, 100.0,100.0, sh);
  poly_world.set_scale(3.0,3.0,3.0);
  poly_world.prepare();
  PolygonObj poly(ev, p2a2, sh);
  poly.set_scale(3.0,3.0,3.0);
  poly.prepare();

  P p31 = ev.polygon_api.recalculate_normals(p3);
  P p32 = ev.polygon_api.texcoord_manual(p31, 
					 0.0, 0.0, 
					 1.0, 0.0,
					 1.0, 1.0,
					 0.0, 1.0);
  PolygonObj poly2(ev,p32, sh);
  poly2.set_scale(3.0,3.0,3.0);
  poly2.prepare();


  PT plane_pos = ev.point_api.point(0.0,0.0,0.0);
  V plane_x = ev.vector_api.vector(1.0, 0.0, 0.0);
  V plane_y = ev.vector_api.vector(0.0,0.0,1.0);
  V light_vec = ev.vector_api.vector(20.0,40.0,20.0);
  V reflect_vec = ev.vector_api.vector(0.0, 40.0, 0.0);
  //P shadow = ev.polygon_api.shadow(p, plane_pos, plane_x, plane_y, light_vec);
  //P shadow_color = ev.polygon_api.color_faces(shadow, 0xff333333, 0xff333333, 0xff333333, 0xff333333);
#if 0
  P reflect = ev.polygon_api.reflection(p2a, plane_pos, plane_x, plane_y, reflect_vec);
  P reflect_color = ev.polygon_api.change_colors(reflect, color_change_func);

#endif
  
  //PolygonObj shadow_obj(ev,shadow_color, sh);
  //shadow_obj.set_scale(3.0,3.0,3.0);
  //shadow_obj.prepare();

  
  P ppx = ev.polygon_api.empty();
  PolygonObj reflect_obj(ev,ppx /*reflect_color*/, sh);
  reflect_obj.set_scale(3.0,3.0,3.0);
  reflect_obj.prepare();


  PT cone_p1 = ev.point_api.point(0.0, 0.0, 0.0);
  PT cone_p2 = ev.point_api.point(300.0, 0.0, 0.0);
  PT cone_p21 = ev.point_api.point(-300.0, 0.0, 0.0);
  PT cone_p22 = ev.point_api.point(0.0, 0.0, 300.0);
  PT cone_p23 = ev.point_api.point(0.0, 0.0, -300.0);
  P lines_3 = ev.polygon_api.cone(20, cone_p1, cone_p2, 2.0, 2.0);
  P lines_3a = ev.polygon_api.cone(20, cone_p1, cone_p21, 2.0, 2.0);
  P lines_3b = ev.polygon_api.cone(20, cone_p1, cone_p22, 2.0, 2.0);
  P lines_3c = ev.polygon_api.cone(20, cone_p1, cone_p23, 2.0, 2.0);
  P array[] = { lines_3, lines_3a, lines_3b, lines_3c };
  P lines_33 = ev.polygon_api.or_array(&array[0], 4);
  P lines_4 = ev.polygon_api.translate(lines_33,0.0,20.0,0.0);
  PolygonObj lines_obj(ev, lines_4, sh4);
  lines_obj.prepare();



  //P gp = ev.polygon_api.or_elem(p3, reflect_color);
  //P gp2 = ev.polygon_api.or_elem(gp, p2a);

  //ev.polygon_api.save_model(gp2, "world.obj");
#if 1
  FBO fbo = ev.fbo_api.create_fbo(512,512);
  ev.fbo_api.config_fbo(fbo);

  P fbo_poly = ev.polygon_api.quad_z(0.0, 800.0,
				 0.0, 600.0,
				 0.0);
  P fbo_poly2 = ev.polygon_api.texcoord_manual(fbo_poly, 0.0, 0.0,
					   1.0, 0.0,
					   1.0, 1.0,
					   0.0, 1.0);
  TXID txid = ev.fbo_api.tex_id(fbo);

  PolygonObj fbo_bg(ev, fbo_poly2, sh2);
  fbo_bg.bind_texture(0,txid);
  fbo_bg.prepare();

  FBO mirror_fbo = ev.fbo_api.create_fbo(512,512);
  ev.fbo_api.config_fbo(mirror_fbo);
  TXID mirror_txid = ev.fbo_api.tex_id(mirror_fbo);
  

#endif

  P mirror_p = ev.polygon_api.quad_z(0.0, 100.0,
				     0.0, 80.0,
				     0.0);
  P mirror_p_t = ev.polygon_api.translate(mirror_p, 500.0, 0.0, 500.0);
  P mirror_p_t_t = ev.polygon_api.texcoord_manual(mirror_p_t, 0.0, 0.0,
						  1.0, 0.0,
						  1.0, 1.0,
						  0.0, 1.0);
  PolygonObj mirror(ev, mirror_p_t_t, sh3);
  mirror.set_scale(3.0,3.0,3.0);
  mirror.bind_texture(0, mirror_txid);
  mirror.prepare();


  
  PT monster_center = ev.point_api.point(50.0, 40.0, 0.0);
  P monster_sphere = ev.polygon_api.sphere(monster_center, 30.0, 20,20);
  PT monster_p1 = ev.point_api.point(50.0, 40.0, 0.0);
  PT monster_p2 = ev.point_api.point(50.0, 0.0, 0.0);
  P monster_cone = ev.polygon_api.cone(30, monster_p1, monster_p2, 40.0, 30.0);

  P monster_or = ev.polygon_api.or_elem(monster_sphere, monster_cone);

  P monster_p = monster_or; //ev.polygon_api.quad_z(0.0, 100.0, 0.0, 80.0, 0.0);
  P monster_p_t = ev.polygon_api.translate(monster_p, 500.0, 0.0, 600.0);

  PolygonObj monster(ev, monster_p_t, sh);
  monster.set_scale(3.0,3.0,3.0);
  monster.prepare();


  ev.mainloop_api.alpha(true);
  float pos_x = 0.0;
  float pos_y = 0.0;
  float rot_y = 0.0;
  float speed = 100.0;
  float rot_speed = 8.0*3.14159*2.0/360.0;
  float speed_x, speed_y;
  int frame = 0;
  float mouse_x = 0.0, mouse_y=0.0;
  std::vector<BM> screenshot_images;
  e.free_memory();

  env.pos_x = pos_x;
  env.pos_y = pos_y;
  env.rot_y = rot_y;
  env.speed = speed;
  env.rot_speed = rot_speed;
  env.speed_x = speed_x;
  env.speed_y = speed_y;
  env.frame = frame;
  env.mouse_x = mouse_x;
  env.mouse_y = mouse_y;
  env.sh = sh;
  env.sh2 = sh2;
  env.sh3 = sh3;
  env.poly = &poly;
  env.poly_world = &poly_world;
  env.reflect_obj = &reflect_obj;
  env.poly2 = &poly2;
  env.mirror = &mirror;
  env.fbo_bg = &fbo_bg;
  env.fbo = fbo;
  env.mirror_fbo = mirror_fbo;
  env.monster = &monster;
  env.jump_start_frame = -9999.0;
  env.jump_duration=0.2*100;
  env.y_delta = 0.0;
  env.lines_obj = &lines_obj;
  //env.sh2 = sh2;

  ev.mainloop_api.reset_time();
  ev.mainloop_api.display_logo(ev);
  ev.mainloop_api.alpha(true);

#ifndef EMSCRIPTEN
  while(1) {
    iter();
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop(iter, 60,1);
#endif

}
