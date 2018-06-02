
#include "GameApi.hh"

#define NO_SDL_GLEXT
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>
#include <iostream>
#include <cmath>
using namespace GameApi;

PT Map(PT p, int face, int point, void *data)
{
  return p;
#if 0
  EveryApi &api = *(EveryApi*)data;
  SpaceApi *g_space = &api.space_api;
  MainLoopApi *g_loop = &api.mainloop_api;
  AnimApi *g_anim = &api.anim_api;

  float val_x = g_space->pt_x(p);
  float val_z = g_space->pt_z(p);
  
  float val = sqrt((val_x-100.0)*(val_x-100.0)+(val_z-100.0)*(val_z-100.0));
  val*=2.0*3.14159265/100.0;
  float val2 = std::sin(val-g_mod);
  //F f = g_function->sin();
  //float val2 = g_function->get_value(f, val+g_mod);
  SP screen = g_loop->screenspace();
  PT center = g_space->pos(screen, 0.0, 0.0);
  PT delta = g_space->pos(screen, 0.0, 30.0);
  
  IS l = g_anim->line(center, delta, 100);
  float val3 = (val2+1.0)*48.0;
  PT pos = g_anim->timed_value_point(l, val3);
  return g_space->plus(p,pos);
#endif
}
  

void GameApiTest()
{
#if 0
  Env e;
  MainLoopApi loop(e);
  SpriteApi spr(e);
  BitmapApi bm(e); 
  SpaceApi sp(e); 
  PolygonApi poly(e);
  AnimApi a(e); 
  //Movement mv(e);
  GridApi gr(e);
  TextApi tx(bm, spr);
  ShaderApi shader(e);
  FunctionApi func(e);
  SurfaceApi surf(e);
  //VBO vbo(e);
  loop.init_window();

  shader.load("Shader.txt");
  SH sh = shader.get_shader("texture", "texture", "");
  shader.use(sh);
  shader.set_default_projection(sh, "in_P");

  loop.init(sh);
  loop.antialias(true);



  tx.load_font("./maps/font.png", 32,32, 20,2,'a','z');
  BM b1 = bm.loadbitmap("./maps/circle.png");
  spr.preparesprite(b1);
  //BM mand2 = bm.newbitmap(600,300);
  //BM mand1 = bm.mandelbrot(false, 
  //			  -2.0, 1.0, // x
  //			  -1.0, 1.0, // y
  //			  0.0,0.0,   // pos
  //			  600,300,   // sx,sy
  //			  128); // iterations

  //V v = vbo.alloc(1);
  //vbo.sprite(v, 0, mand1, 0.0,0.0);
  //vbo.prepare(v);
  
  SP screen = loop.screenspace();
  //PT center_b = sp.pos(screen, 0.0, 0.0, 0.0);
  //PT center_a = sp.pos(screen, -100.0, -100.0,-100.0);
  //PT center_x = sp.pos(screen, 100.0, -100.0,-100.0);
  //PT center_y = sp.pos(screen, -100.0, 100.0,-100.0);
  //PT center_z = sp.pos(screen, -100.0, -100.0,100.0);
  //S ss = surf.sphere(center_b, 50.0);
  //S ss = surf.bitmapsphere(center_b, 50.0, 80.0, mand1);
  //BM mand3 = bm.repeatbitmap(mand1, 100,100);
  //S s2 = surf.texture(ss, mand1);
  //BM res = surf.render(s2, 300,300, center_a, center_x, center_y, center_z);
  
  //std::vector<BM> vec;
  //for(int i=0;i<50;i++)
    //{
      //BM mand = bm.interpolatebitmap(mand1, mand2, 1.0*i/50.0);
      //vec.push_back(mand);
      //}
  //BM mand = bm.bitmaparray(&vec[0], 50);
  //spr.preparesprite(res);
  BM tile = bm.loadtilebitmap("./maps/circle.png", 30,30);
  gr.preparegrid(tile);
  BM grid = bm.loadposbitmap("./maps/circle.png");
  //SP s = loop.screenspace();
  P ppp = poly.cube(-100.0, 100.0,
		  -200.0, -250.0,
		  -100.0, 100.0);
  P ppp3 = poly.quad_y(-400,400,
		       -200,
		       -400,400);
    //poly.cube(-400.0, 400.0,
    //	  -200.0, -250.0,
    //		  -400.0, 400.0);

  P ppp2 = poly.color_faces(ppp, 
			   0xffffffff,
			   0xff888888,
			   0xff888888,
			   0xffffffff
			   );
  P ppp4 = poly.color_faces(ppp3, 
			   0xffffffff,
			   0xff888888,
			   0xff888888,
			   0xffffffff
			   );
  std::vector<P> vec2;
  PT center = sp.pos(screen, 100.0, 100.0);
  PT corner = sp.pos(screen, 320.0, 200.0);

  P split = poly.splitquads(ppp4, 40,40);

  for(int i=0;i<20;i++)
    {
      //float g_mod = i*2.0*3.14159265/20;
      //FunctionCb<PT,PT> *f = new ChangeFunction(&bm, &func, &sp, &a, &loop, &poly, g_mod);
      EveryApi every(e);
      P pm = poly.change_positions(split, std::bind(Map, _1,_2,_3, (void*)&every));
      P memo = poly.memoize(pm);
      P pm2 = poly.recalculate_normals(memo);
      vec2.push_back(pm2);
    }
  P anim = poly.anim_array(&vec2[0], 20);

  P sphere = poly.sphere(center, 100.0, 40,40);
  IS path = a.line(center, corner, 100.0);

  //IS alpha = a.line(0, 50, 100.0);
  //IS alpha2 = a.line(50,0, 100.0);
  //IS arr[] = { alpha, alpha2 };
  //IS alpha3 = a.seq_line(arr, 2);
  //IS alpha4 = a.repeat_infinite(alpha3);

  poly.preparepoly(sphere);
  poly.preparepoly(ppp2);
  poly.preparepoly(anim);
  int val = 0;
  //shader.load("Shader2.txt");
  //SH shader1 = shader.get_shader("passdata", "red", "geomquads");
  
  

  while(1)
    {
     
      float time = loop.get_time()/30.0;
      loop.clear();
      loop.alpha(false);
      gr.rendergrid(grid, -1, 0,0);
      tx.draw_text("tekstiabczyx", 100,100+time, sh);
      loop.alpha(true);
      spr.rendersprite(b1, sh, 0+time, 0, 1.0, 1.0);
      spr.rendersprite(b1, sh, 20+time, 20.0, 1.0, 1.0);
      //PT p = sp.pos(s, 100.0,100.0);
      //spr.rendersprite(b1, p);
      loop.alpha(false);
      loop.switch_to_3d(true, sh);
      glPushMatrix();
      glRotatef(loop.get_time()/30.0, 0.0,1.0,0.0);
      //shader.use(shader1);
      glBegin(GL_TRIANGLES);
      glVertex3f(100.0,0.0,0.0);
      glVertex3f(0.0,100.0,0.0);
      glVertex3f(100.0,100.0,0.0);      
      glEnd();
      poly.renderpoly(ppp2, 0.0,0.0,0.0);
      //shader.unuse(shader1);
      poly.renderpoly(anim, int(loop.get_time()/30.0)%20, 0.0,0.0,0.0);
      glPopMatrix();
      //vbo.render(v);
      poly.renderpoly(sphere, a.timed_value_point(path, loop.get_time()/30.0));
      loop.switch_to_3d(false, sh);
      loop.alpha(true);
      //spr.rendersprite(res, 300.0, 300.0);
      loop.alpha(false);
      loop.swapbuffers();
      MainLoopApi::Event e = loop.get_event();
      if (e.ch==27) break;
      if (e.type==2 && e.ch==32) { val++; }
    }
#endif
}

