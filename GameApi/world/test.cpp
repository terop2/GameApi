#include "GameApi.hh"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <functional>


using namespace GameApi;

char world[] = \
  "+---------+---------+"
  "|....%...T|.P...Y...|"
  "|..+---+..|...L....S|"
  "|.Q|...|........I...|"
  "|..+.*.+..|........G|"
  "|&..W....#|K....H...|"
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
    P pk = ev.polygon_api.color_faces(p2a, 0xaa8888af, 0xaa4444af, 0xaa2222af, 0xaaaaaaaf);
    //return ev.polygon_api.empty();
     return pk;
}

P pieces3(unsigned int i, EveryApi &ev)
{
    P p1a = ev.polygon_api.cube(0.0, 100.0, 0.0, 1.0, 0.0, 100.0);
    P pk = ev.polygon_api.color_faces(p1a, 0x8888884f, 0x4444444f, 0x2222224f, 0xaaaaaa4f);
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
  P pp1 = ev.polygon_api.color_faces(pl, 0xccccccff, 0xff8844ff, 0x2288ffff, 0xff8844ff);
  P pp2 = ev.polygon_api.or_elem(pp, pp1);
  return pp2;
}

P poly_func(int face, float p1_x, float p1_y, float p1_z,
	    float p2_x, float p2_y, float p2_z,
	    float p3_x, float p3_y, float p3_z,
	    float p4_x, float p4_y, float p4_z, EveryApi &ev)
{
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

using std::placeholders::_10;
using std::placeholders::_11;
using std::placeholders::_12;
using std::placeholders::_13;

P cube_parts(int face,
	     float p1_x, float p1_y, float p1_z,
	     float p2_x, float p2_y, float p2_z,
	     float p3_x, float p3_y, float p3_z,
	     float p4_x, float p4_y, float p4_z,
	     EveryApi &ev)
{
  PT pt1 = ev.point_api.point(p1_x, p1_y, p1_z);
  PT pt2 = ev.point_api.point(p2_x, p2_y, p2_z);
  PT pt3 = ev.point_api.point(p3_x, p3_y, p3_z);
  PT pt4 = ev.point_api.point(p4_x, p4_y, p4_z);

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
  P p2 = ev.polygon_api.from_polygon(p, std::bind(&cube_parts, _1,
						  _2,_3,_4,
						  _5,_6,_7,
						  _8,_9,_10,
						  _11,_12,_13, std::ref(ev)));
  return p2;
}

P heightmap_cube(float val, EveryApi &ev)
{
  float s = 10.0;
  P p1 = ev.polygon_api.cube(0.0,0.5,
			     0.0, val*s,
			     0.0, 0.5);
  P p1a = ev.polygon_api.color_faces(p1, 0x000000ff, 0x222222ff, 0x111111ff, 0x333333ff);

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
      P p4 = ev.polygon_api.color_from_normals(p3);
      return p4;
#endif
    }
  case 17:
    {
      P p = m.lucy;
      P p2 = ev.polygon_api.scale(p, 5.0,5.0,5.0);
      P p3 = ev.polygon_api.color_from_normals(p2);
      return p3;
    }
  case 16:
    {
      PT center = ev.point_api.point(50.0, 40.0, 50.0);
      V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
      V uu_x = ev.vector_api.vector(1.0, 0.0, 0.0);
      V uu_y = ev.vector_api.vector(0.0, 0.0, 1.0);
      P p = ev.polygon_api.torus(30,20, center, u_x, u_y, 30.0, uu_x, uu_y, 10.0);
      LI lines = ev.lines_api.from_polygon(p);
      
     P p2 = ev.polygon_api.from_lines(lines, std::bind(&line_func, _1,_2,_3,_4,_5,_6,_7, _8,_9, std::ref(ev)));
 
     return p2;

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
      P p2 = ev.polygon_api.color_from_normals(p);
      return p2;
      
    }
  case 14:
    {
      BM bm = ev.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 200, 200, 256);
      FB fb = ev.float_bitmap_api.from_green(bm);
      //P p = ev.polygon_api.heightmap(bm, PolygonApi::EQuad, 0.0, 100.0,
      //				     20.0, 0.0, 
      //				     0.0, 100.0);
      P p = ev.polygon_api.heightmap(fb, std::bind(&heightmap_cube, _1, std::ref(ev)), 0.5,0.5);
      //P p2 = ev.polygon_api.color_faces(p, 0x112233ff, 0x224466ff, 0x336699ff, 0x4488aaff);
      // P pp = ev.polygon_api.rotatex(p,90.0*3.14159*2.0/360.0);
      return p;
    }
  case 13:
    {
      PT p1 = ev.point_api.point(50.0, 0.0, 50.0);
      PT p2 = ev.point_api.point(50.0, 80.0, 50.0);
      P p = ev.polygon_api.cone(50, p1, p2, 50.0, 30.0);
      P p3 = ev.polygon_api.color_faces(p, 0xffffffff, 0xaaaaaaff, 0xccccccff, 0x888888ff);
      return p3;
    }
  case 12:
    {
      P p = m.teapot;
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
      P p2 = ev.polygon_api.color_faces(p, 0x000000ff, 0x222222ff, 0x111111ff, 0x333333ff);
      return p2;
    }
  case 10:
    {
      PT pt = ev.point_api.point(40.0, 40.0, 40.0);
      P p = ev.polygon_api.sphere(pt, 40.0, 20,20);
      P p2 = ev.polygon_api.from_polygon(p, std::bind(&poly_func, _1,  _2,_3,_4,
						      _5,_6,_7,
						      _8,_9,_10,
						      _11,_12,_13, std::ref(ev)));
      P p3 = ev.polygon_api.color_faces(p2, 0x888888ff, 0x444444ff, 0x222222ff, 0x666666ff);
     return p3;
    }
  case 9:
    {
      PT pt = ev.point_api.point(40.0, 40.0, 40.0);
      P p = ev.polygon_api.sphere(pt, 40.0, 20,20);
      LI li = ev.lines_api.from_polygon(p);
      P p2 = ev.polygon_api.from_lines(li, std::bind(&line_func, _1,_2,_3,_4,_5,_6,_7, _8,_9, std::ref(ev)));
      return p2;
    }
  case 8:
    {
      BB bg = ev.bool_bitmap_api.empty(80,80);
      BB circle1 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 40.0);
      BB circle2 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 30.0);
      BB circle3 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 20.0);
      BB circle4 = ev.bool_bitmap_api.circle(bg, 40.0, 40.0, 10.0);
      BM circle_bm1 = ev.bool_bitmap_api.to_bitmap(circle1, 255,255,255,255, 0,0,0,0);
      BM circle_bm2 = ev.bool_bitmap_api.to_bitmap(circle2, 255,200,200,200, 0,0,0,0);
      BM circle_bm3 = ev.bool_bitmap_api.to_bitmap(circle3, 255,155,155,155, 0,0,0,0);
      BM circle_bm4 = ev.bool_bitmap_api.to_bitmap(circle4, 255,100,100,100, 0,0,0,0);
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
      
      return p2;
    }
  case 7:
    {
      PT pt = ev.point_api.point(50.0,40.0,50.0);
      O o = ev.volume_api.sphere(pt, 40.0);
      FO fo = ev.float_volume_api.from_volume(o, 0.0, 1.0);
      PTS pts = ev.points_api.from_float_volume(fo, 1000, 0.0, 0.0, 0.0, 100.0, 100.0, 100.0);
      P p2 = ev.polygon_api.from_points(pts, std::bind(points_func, _1, _2, _3, _4, _5, std::ref(ev)));
      
      return p2;
    }
    
 case 6:
   {
     P p1 = ev.polygon_api.cube(30.0,70.0,30.0,70.0,30.0,70.0);
     P p11 = ev.polygon_api.rotatex(p1, 45.0*3.14159*2.0/360.0);
     P p2 = ev.polygon_api.translate(p11,0.0,40.0,0.0);
     P p3 = ev.polygon_api.color_faces(p2, 0x888888ff, 0x444444ff, 0x222222ff, 0xaaaaaaff);
     return p3;
   }
  case 4:
    {
      PT pt = ev.point_api.point(50.0,40.0,50.0);
      P p2 = ev.polygon_api.sphere(pt, 40.0, 40,40);
      P p3 = ev.polygon_api.color_from_normals(p2);
      return p3;
    }
  case 3:
    {
      P p = cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0, ev);
      P pa = cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0, ev);
      P pp = ev.polygon_api.or_elem(p,pa);
      P p2 = ev.polygon_api.color_faces(pp, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff); 
      return p2;
    }
  case 2:
    {
      P p = cube(50.0, 58.0, 0.0, 80.0, 0.0, 100.0, ev);
      P p2 = ev.polygon_api.color_faces(p, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff);
      return p2;
    }
    
  case 1:
    {
      P p = cube(0.0, 100.0, 0.0, 80.0, 50.0, 58.0, ev);
      P p2 = ev.polygon_api.color_faces(p, 0xff8844ff, 0x884422ff, 0xff8844ff, 0x884422ff);
      return p2;
    }
  case 0:
    break;
  };
  return ev.polygon_api.empty();
}

unsigned int color_change_func(unsigned int orig, int face, int point)
{
  unsigned int color = orig;
  unsigned int color_r = color & 0xff000000;
  unsigned int color_g = color & 0x00ff0000;
  unsigned int color_b = color & 0x0000ff00;
  unsigned int color_a = color & 0x000000ff;
  
  color_r /= 4;
  color_g /= 4;
  color_b /= 4;
  unsigned int color2 = color_r | color_g | color_b |color_a;
  unsigned int color3 = color2 & 0x3f3f3fff;
  return color3;
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

  Models m;
  m.teapot = ev.polygon_api.load_model("./teapot.obj", 0);
  m.lucy = ev.polygon_api.load_model("./lucy.obj", 1);
  P pk = ev.polygon_api.empty();
#if 0
  for(int i=0;i<60;i++)
    {
      pk = ev.polygon_api.or_elem(pk, ev.polygon_api.load_model("./sponzasimple.obj", i));
    }
  m.sponza = pk;
#endif

  BM bm = ev.bitmap_api.newintbitmap(world, 21, 7, func);
  P p = ev.polygon_api.world_from_bitmap(std::bind(&pieces, _1, std::ref(ev), std::ref(m)), bm   , 100.0, 100.0);
  P p2 = ev.polygon_api.world_from_bitmap(std::bind(&pieces2, _1, std::ref(ev)), bm, 100.0, 100.0);
  P p3 = ev.polygon_api.world_from_bitmap(std::bind(&pieces3, _1, std::ref(ev)), bm, 100.0, 100.0);
  P p3a = ev.polygon_api.or_elem(p,p3);
  P p2a = ev.polygon_api.or_elem(p,p2);
  PolygonObj poly(ev, p2a, sh);
  poly.set_scale(3.0,3.0,3.0);
  poly.prepare();

  PolygonObj poly2(ev,p3, sh);
  poly2.set_scale(3.0,3.0,3.0);
  poly2.prepare();


  PT plane_pos = ev.point_api.point(0.0,0.0,0.0);
  V plane_x = ev.vector_api.vector(1.0, 0.0, 0.0);
  V plane_y = ev.vector_api.vector(0.0,0.0,1.0);
  V light_vec = ev.vector_api.vector(20.0,40.0,20.0);
  V reflect_vec = ev.vector_api.vector(0.0, 40.0, 0.0);
  P shadow = ev.polygon_api.shadow(p, plane_pos, plane_x, plane_y, light_vec);
  P shadow_color = ev.polygon_api.color_faces(shadow, 0x333333ff, 0x333333ff, 0x333333ff, 0x333333ff);
  
  P reflect = ev.polygon_api.reflection(p2a, plane_pos, plane_x, plane_y, reflect_vec);
  P reflect_color = ev.polygon_api.change_colors(reflect, color_change_func);


  
  PolygonObj shadow_obj(ev,shadow_color, sh);
  shadow_obj.set_scale(3.0,3.0,3.0);
  shadow_obj.prepare();

  PolygonObj reflect_obj(ev,reflect_color, sh);
  reflect_obj.set_scale(3.0,3.0,3.0);
  reflect_obj.prepare();


  ev.mainloop_api.alpha(true);
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

    //shadow_obj.set_rotation_matrix2(mm);
    //shadow_obj.set_pos(pos_x, -75.0, pos_y);
    //shadow_obj.render();

    reflect_obj.set_rotation_matrix2(mm);
    reflect_obj.set_pos(pos_x, -75.0, pos_y);
    reflect_obj.render();


    ev.mainloop_api.transparency(true);
    poly2.set_rotation_matrix2(mm);
    poly2.set_pos(pos_x, -80.0, pos_y);
    poly2.render();
    ev.mainloop_api.transparency(false);

    //ev.mainloop_api.depth_test(true);
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
