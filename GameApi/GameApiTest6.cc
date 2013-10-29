#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <iostream>

#include "GameApi.hh"
#include <cmath>

using namespace GameApi;

float SphereObj(EveryApi &ev, float x, float y, float z, void *data)
{
  float val = x*x+y*y+z*z;
  if (val <= 150.0*150.0 && val >= 140.0*140.0) { return 1.0; }
  if (val <= 100.0*100.0 && val >= 90.0*90.0) { return 1.0; }
  if (val <= 50.0*50.0 && val >= 40.0*40.0) { return 1.0; }
  return 0.0;
}

void GameTest6(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  FloatVolumeApi &api = e.float_volume_api;

  loop.init_window();

  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  //e.shader_api.use(sh);
  loop.init_3d(sh);

  e.shader_api.use(sh);
  e.shader_api.set_default_projection(sh, "in_P");

  
  //BM bm_mand = e.bitmap_api.mandelbrot2(false, -2.0, 1.0, -1.0, 1.0, 0.0,0.0, 150, 150, 256);
  //BM bm_julia = e.bitmap_api.mandelbrot2(true, -1.0, 1.0, -1.0, 1.0, 0.25,0.15, 150, 150, 256);

  //FO obj = api.function(&SphereObj, &bm_mand);
  //FO obj2 = api.move(obj, 100.0,0.0,0.0);
  //FO obj3 = api.maximum(obj,obj2);
  //FOA array = api.prepare(obj, 800000, -150.0,-150.0,-150.0, 350.0,350.0,350.0);

  
  std::string filename = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";
  Ft font = e.font_api.newfont(filename.c_str(), 500,500);
  BM fontbm = e.font_api.glyph(font, u'R');
  BM logobm = e.bitmap_api.loadbitmap("./pics/Logo uusi.jpg");
  LI fontli = e.font_api.glyph_outline(font, u'@', 300.0,-300.0);
  BM fontbm2 = e.bitmap_api.growbitmap(fontbm, 1, 1, 1, 1);
  FB fb = e.float_bitmap_api.from_blue(fontbm2);
  BB fb2 = e.bool_bitmap_api.from_float_bitmap(fb, 0.0, 0.9);
  FB fb3 = e.float_bitmap_api.from_bool(fb2, 1.0, 0.0);
  FO obj2 = api.from_float_bitmap(fb3, -150.0, 150.0,
  				  -150.0, 150.0,
  				  -50.0, 50.0);
  

  BB bb = e.bool_bitmap_api.from_float_bitmap(fb, 0.9, 1.1);
  LI li = e.lines_api.border_from_bool_bitmap(bb, -150.0, 150.0,
					      -150.0, 150.0,
					      -50.0);
  LI li2 = e.lines_api.border_from_bool_bitmap(bb, -150.0, 150.0,
					      -150.0, 150.0,
					      50.0);
  LI li3 = e.lines_api.border_from_bool_bitmap(bb, -150.0, 150.0,
					      -150.0, 150.0,
					      -20.0);
  LI li4 = e.lines_api.border_from_bool_bitmap(bb, -150.0, 150.0,
					      -150.0, 150.0,
					      20.0);
  LI li5 = e.lines_api.border_from_bool_bitmap(bb, -150.0, 150.0,
					      -150.0, 150.0,
					      0.0);

  LLA lia = e.lines_api.prepare(li);
  LLA lia2 = e.lines_api.prepare(li2);
  LLA lia3 = e.lines_api.prepare(li3);
  LLA lia4 = e.lines_api.prepare(li4);
  LLA lia5 = e.lines_api.prepare(li5);


  LLA fontlia = e.lines_api.prepare(fontli);
  //O o = e.volume_api.mandelbulb(8.0, 0.0,0.0,0.0,
  //		     0.0,0.0,0.0,
  //		     2.0,
  //		     60);
  //O o2 = e.volume_api.scale(o, 150.0,150.0,150.0);
  //FO obj2 = e.float_volume_api.from_volume(o2, 0.0, 1.0);
  FOA array2 = api.prepare(obj2, 200000, -150.0,-150.0,-150.0, 150.0,150.0,150.0);


  P poly = e.polygon_api.quad_z(-150.0, 150.0,
				-150.0, 150.0,
				0.0);
  P poly2 = e.polygon_api.color_faces(poly, 0xffff0000, 0xff88ff00, 0xff00ff00, 0xff0000ff);
  //TX tx1 = e.texture_api.tex_plane(1500,1500);
  int id = e.texture_api.unique_id();
  //TX tx2 = e.texture_api.tex_assign(tx1, id, 0,0, logobm);
  TX tx = e.texture_api.tex_bitmap(logobm);
  TX tx2 = e.texture_api.tex_coord(tx, id, 0,0, e.bitmap_api.size_x(logobm), e.bitmap_api.size_y(logobm));
  Q q = e.texture_api.get_tex_coord(tx2, id);
  P texpoly = e.polygon_api.sprite_bind(poly2, q, tx2);
  TXID txid = e.texture_api.prepare(tx2);
  
  VA va = e.polygon_api.create_vertex_array(texpoly);
  

  float time = 0.0;
  glLineWidth(1);
  int frame = 0;
  while(1)
    {
      e.mainloop_api.clear_3d();
      //e.mainloop_api.switch_to_3d(true,sh);
      e.shader_api.set_y_rotation(sh, "in_MV", time/50.0);
      glColor4f(1.0,0.5,0.3,0.2);
      //glEnable(GL_BLEND);
      //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      //glPushMatrix();
      //glRotatef(time, 0.0,1.0,0.0);
      //glScalef(1.0,-1.0,1.0);
      //api.render(array);

      //api.render(array2);
      //glColor4f(1.0,1.0,1.0,1.0);
      //e.lines_api.render(lia);
      //e.lines_api.render(lia2);
      //e.lines_api.render(lia3);
      //e.lines_api.render(lia4);
      //e.lines_api.render(lia5);

      //e.lines_api.render(fontlia);
      M mat = e.matrix_api.yrot(sin(time/50.0)*3.14159/4.0);
      M mat2 = e.matrix_api.scale(2.0,-2.0,2.0);
      M mat3 = e.matrix_api.mult(mat, mat2);
      e.shader_api.set_var(sh, "in_MV", mat3);
      e.texture_api.use(txid);
      e.shader_api.use(sh);
      e.polygon_api.render_vertex_array(va);
      e.texture_api.unuse(txid);

      //glPopMatrix();
      //glDisable(GL_BLEND);
      e.mainloop_api.swapbuffers();
#if 0
      BM scr = e.mainloop_api.screenshot();
      BM scr2 = e.bitmap_api.flip_y(scr);
      std::string filename = "scr.bmp";
      frame++;
      std::string lead = "000";
      lead[0] = frame /100;
      lead[1] = (frame-lead[0]*100)/10;
      lead[2] = (frame-lead[0]*100-lead[1]*10);
      lead[0]+='0';
      lead[1]+='0';
      lead[2]+='0';
      e.bitmap_api.savebitmap(scr2, lead+filename);
#endif
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time+=0.04;
    }
}
