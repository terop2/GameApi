
#include "GameApi.hh"
#pragma comment(lib, "d:\\cvs\\gameapi-code\\GameApi\\GameApi\\Debug\\GameApi\\GameApi.lib")
#include <iostream>
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace GameApi;


  struct A {
    float val;
    TX tx;
    int id;
  };

P torus_func(float val, void *cb, EveryApi &e)
{
  std::cout << "torus_func" << val << std::endl;
  A *cb2 = (A*)cb;
  PT p = e.point_api.point(cb2->val,cb2->val,100.0);
  P p0 = e.polygon_api.sphere(p, val, 30, 30);
  //P p0 = e.polygon_api.quad_z(val+cb2->val, cb2->val+val+100.0,
  // 			      val, val+100.0,
  // 			      100.0);
  P p00;
  if (val>105.0)
    {
      p00 = e.polygon_api.color(p0, 0xffff0000);
    }
  else
    {
      p00 = e.polygon_api.color(p0, 0xffff00ff);
    }
  P p1 = e.polygon_api.sprite_bind(p00, e.texture_api.get_tex_coord(cb2->tx, cb2->id), cb2->tx);
  //P p1 = e.polygon_api.scale(p0, 0.05,0.05,0.2);
  //V v = e.vector_api.vector(0.0,-1.0,0.0);
  //P p1 = e.polygon_api.rotate(p00, p, v, 180.0);
  return p1;
}

float OpenglCenterFunc(int path, std::string name)
{
  if (name=="center_x") return 150.0;
  if (name=="center_y") return 150.0;
  if (name=="center_z") return 100.0;
  return 0.0;
}

void GameTest5(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  ShaderApi &shader = e.shader_api;
  StateChangeApi &change_api = e.state_change_api;

  loop.init_window();
  shader.load("Shader.txt");
  SH sh = shader.get_shader("linear", "red", "");
  //e.shader_api.bind_attrib(sh, 0, "in_Position");
  //e.shader_api.bind_attrib(sh, 1, "in_Normal");
  //e.shader_api.bind_attrib(sh, 2, "in_Color");
  //e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.link(sh);

  loop.init_3d(sh);


  //loop.alpha(true);
  shader.bindnames(sh, "vertex2", "normal2","color2","texcoord2");

  BM red = e.bitmap_api.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);
  TX tx = e.texture_api.tex_plane(128,128);
  int id = e.texture_api.unique_id();
  TX tx2 = e.texture_api.tex_assign(tx, id, 0,0, red);
  TXID tex = e.texture_api.prepare(tx2);


  BM normalmap = e.bitmap_api.loadbitmap("./textures/texture.png");
  TX tx3 = e.texture_api.tex_plane(128,128);
  int id3 = e.texture_api.unique_id();
  TX tx3a = e.texture_api.tex_assign(tx3, id3, 0,0, normalmap);
  TXID tex3 = e.texture_api.prepare(tx3a);

  A val;
  val.val = 0.0;
  val.tx = tx2;
  val.id = id;
  A val2;
  val2.val = 150.0;
  val2.tx = tx2;
  val2.id = id;
  TR t = change_api.init(2);
  //float val = 0.0;
  //float val2 = 150.0;
  TR t2 = change_api.linear(t, 0, std::bind(torus_func, _1, (void*)&val,std::ref(e)), 100.0, 200.0, 100.0);
  TR t3 = change_api.linear(t2, 0, std::bind(torus_func,_1, (void*)&val,std::ref(e)), 200.0, 100.0, 100.0);
  TR t4 = change_api.linear(t3, 1, std::bind(torus_func,_1, (void*)&val2,std::ref(e)), 100.0, 200.0, 100.0);
  TR t5 = change_api.linear(t4, 1, std::bind(torus_func,_1, (void*)&val2,std::ref(e)), 200.0, 100.0, 100.0);
  
  //P p = torus_func(e, 50.0, &val);
  //VA vertex = e.polygon_api.create_vertex_array(p);

  VV valval = change_api.prepare(t5);
  float time = 0.0;

  //P p = torus_func(e, 100.0, &val2);
  //VA va = e.polygon_api.create_vertex_array(p);
  
  bool take_screenshot = false;
  std::vector<BM> vec;
  int count = 0;
  int count2 = 0;
  while(1) {
    e.mainloop_api.clear_3d();
    time += 0.1;
    count2++;
    if (count2>10) count2=0;
    if (time > 200.0) { count++; time = 0.0; }
    //std::cout << "Time:" << time << std::endl;
    glPushMatrix();
    glScalef(0.5,0.5,0.5);
    shader.use(sh);
    //loop.switch_to_3d(true);
    e.shader_api.set_var(sh, "tex", 0);
    e.shader_api.set_var(sh, "normaltex", 1);
    e.shader_api.set_var(sh, "center", 0.0,0.0,0.0);
    e.shader_api.set_var(sh, "radius", (float)100.0);
    e.texture_api.use(tex);
    e.texture_api.use(tex3,1);
    change_api.render(valval, time, sh, &OpenglCenterFunc);
    e.texture_api.unuse(tex);
    e.texture_api.unuse(tex3);
    shader.unuse(sh);
    //e.polygon_api.render_vertex_array(vertex);
    glPopMatrix();
    if (count==0 && count2==0 && take_screenshot)
      vec.push_back(e.mainloop_api.screenshot());
    e.mainloop_api.swapbuffers();
    MainLoopApi::Event ev = e.mainloop_api.get_event();
    if (ev.ch==27) break;
  }
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      std::cout << "Saving: BM" << i << ".xpm"<< std::endl;
      std::stringstream s;
      s << "pics/BM";
      s << std::setw(4) << std::setfill('0') << i;
      s << ".xpm";
      e.bitmap_api.savebitmap(e.bitmap_api.subbitmap(vec[i], 300,200, 400,400), s.str());
    }
  shader.unuse(sh);
}
