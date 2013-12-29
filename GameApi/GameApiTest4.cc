#include "GameApi.hh"

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


using namespace GameApi;

unsigned int red_block(int x, int y, void *data)
{
  return 0xff0000;
}
unsigned int green_block(int x, int y, void *data)
{
  return 0x00ff00;
}

P Cube(EveryApi &api,
       float start_x, float end_x, 
       float start_y, float end_y, 
       float start_z, float end_z, 
       unsigned int color, 
       void *data)
{
  P p = api.polygon_api.cube(start_x, end_x,
			      start_y, end_y,
			      start_z, end_z);
  P p2 = api.polygon_api.color_faces(p, 0xffffffff, 0xffffff00, 0xff00ffff, 0xffff00ff);
  
  return p2;
}


void Game(EveryApi &e)
{
  MainLoopApi &loop = e.mainloop_api;
  PointApi &points = e.point_api;
  //VectorApi &vec = e.vector_api;
  BitmapApi &bm = e.bitmap_api;
  //BoolBitmapApi &bool_bm = e.bool_bitmap_api;
  //FloatBitmapApi &float_bm = e.float_bitmap_api;
  PolygonApi &poly = e.polygon_api;
  SpriteApi &sprite = e.sprite_api;
  VolumeApi &volume = e.volume_api;
  WaveformApi &wv = e.waveform_api;

  loop.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.shader_api.bind_attrib(sh, 0, "in_Position");
  e.shader_api.bind_attrib(sh, 1, "in_Normal");
  e.shader_api.bind_attrib(sh, 2, "in_Color");
  e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.link(sh);
  e.shader_api.use(sh);
  e.shader_api.set_default_projection(sh, "in_P");

  loop.init(sh);
  loop.alpha(true);


  //BM red = bm.function(&red_block, 10,10, 0);
  //BM green = bm.function(&green_block, 10,10, 0);
  
  WV sinw = wv.sinwave(2.0*3.14159, 1.0);
  BM red = wv.waveform_bitmap(sinw, 100,100, 0xffffffff, 0xff000000);

  //BM red = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);
  BM green = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);

  O torus = volume.torus(points.point(0.0,0.0,0.0),
			 points.point(2.0,0.0,0.0),
			 points.point(0.0,2.0,0.0), 1.0, 0.4);

  O sphere = volume.sphere(points.point(0.0, 1.0, 0.0), 1.0);
  O andnot = volume.andnot_op(torus, sphere);
  P cubes = volume.rendercubes(andnot, &Cube, 0, 40, 4.0);
  P cubes2 = e.polygon_api.memoize(cubes);
  P cubes3 = e.polygon_api.scale(cubes2, 300.0,300.0,300.0);
  //poly.prepare(cubes3);
  TX tx = e.texture_api.tex_plane(128,128);
  int id = e.texture_api.unique_id();
  TX tx2 = e.texture_api.tex_assign(tx, id, 0,0, red);
  P cubes3_texture = e.polygon_api.sprite_bind(cubes3, tx2, id);
  VA va = poly.create_vertex_array(cubes3_texture);
  TXID tex = e.texture_api.prepare(tx2);
  sprite.preparesprite(red);
  sprite.preparesprite(green);

  // 3d text
  //Ft font = e.font_api.newfont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 50, 50);
  //BM letter = e.font_api.glyph(font, 'a');
  //BB letter2 = e.bool_bitmap_api.from_bitmaps_color_area(letter, 1,255,1,255,1,255,1,255);
  //O o = e.bool_bitmap_api.to_volume(letter2, 20.0);
  //BM bxm = e.volume_api.render(o, 100,100, e.point_api.point(0.0,0.0,0.0),
  //			      e.point_api.point(50.0, 0.0,0.0),
  //			      e.point_api.point(0.0,50.0,0.0),
  //			      e.point_api.point(0.0,0.0,50.0));
  //e.sprite_api.preparesprite(bxm);

  while(1)
    {
      float time = e.mainloop_api.get_time();

      e.mainloop_api.clear();
      e.mainloop_api.switch_to_3d(false, sh);
      e.sprite_api.rendersprite(red,sh,0.0,0.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,100.0,100.0,1.0,1.0);
      e.sprite_api.rendersprite(red,sh,200.0,200.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,300.0,300.0,1.0,1.0);
      e.sprite_api.rendersprite(red,sh,400.0,400.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,500.0,500.0,1.0,1.0);
      e.sprite_api.rendersprite(red,sh,600.0,600.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,700.0,700.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh, 799.0,599.0,1.0,1.0);
      e.mainloop_api.switch_to_3d(true,sh);
      glPushMatrix();
      glRotatef(time/40.0, 0.0,1.0,0.0);
      //e.polygon_api.render(cubes3, 0, 0.0,0.0,0.0);
      e.texture_api.use(tex);
      e.shader_api.set_y_rotation(sh, "in_MV", time/500.0);
      e.polygon_api.render_vertex_array(va);
      e.texture_api.unuse(tex);
      //e.polygon_api.render_vertex_array(va2);
      //e.polygon_api.render_vertex_array(vba);
      //e.sprite_api.rendersprite(bxm,sh,100.0,100.0);
      glPopMatrix();
      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
    }

}
