#include "GameApi.hh"
#pragma comment(lib, "d:\\cvs\\gameapi-code\\GameApi\\GameApi\\Debug\\GameApi\\GameApi.lib")
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>


class PathRendering
{
public:
  void init();
  void draw();
  void drawShape();
};
 
void PathRendering::init()
{
#ifndef __clang__
#ifndef EMSCRIPTEN
#ifndef RASBERRY
  glMatrixLoadIdentityEXT(GL_PROJECTION);
  glMatrixOrthoEXT(GL_PROJECTION, 0, 500, 0, 400, -1, 1);
  glMatrixLoadIdentityEXT(GL_MODELVIEW);

  glClearStencil(0);
  glClearColor(0.1,0.3,0.6,0.0);
  glStencilMask(~0);
  
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
#endif
#endif
#endif
} 
void PathRendering::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  drawShape();
}
void PathRendering::drawShape()
{
#ifndef __clang__
#ifndef EMSCRIPTEN
#ifndef RASBERRY
  //GLuint pathObj = 42;
  const char *pathString = "M100,180 L40,10 L190,120 L10,120 L160,10 z";
  GLuint pathObj = glGenPathsNV(1);
  glPathStringNV(pathObj, GL_PATH_FORMAT_SVG_NV, strlen(pathString), pathString);
  glStencilFillPathNV(pathObj, GL_COUNT_UP_NV, 0x1F);
  glColor3f(1.0,1.0,1.0);
  glCoverStrokePathNV(pathObj, GL_CONVEX_HULL_NV);
#endif
#endif
#endif
}


using namespace GameApi;

unsigned int red_block(int x, int y, void *data)
{
  return 0xff0000;
}
unsigned int green_block(int x, int y, void *data)
{
  return 0x00ff00;
}

P Cube(
       float start_x, float end_x, 
       float start_y, float end_y, 
       float start_z, float end_z, 
       unsigned int color, 
       EveryApi &api)
{
  P p = api.polygon_api.cube(start_x, end_x,
			      start_y, end_y,
			      start_z, end_z);
  P p2 = api.polygon_api.color_faces(p, 0xffffffff, 0xffffff00, 0xff00ffff, 0xffff00ff);
  
  return p2;
}

PT point_func(EveryApi &e, int idx, void *data)
{
  switch(idx) {
  case 0: return e.point_api.point(0.0,50.0,0.0);
  case 1: return e.point_api.point(100.0,0.0,0.0);
  case 2: return e.point_api.point(100.0,100.0,0.0);
  case 3: return e.point_api.point(50.0,100.0,0.0);
  case 4: return e.point_api.point(0.0,50.0,0.0);
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
  PlaneApi &plane = e.plane_api;

  loop.init_window();
  e.shader_api.load("Shader.txt");
  SH sh = e.shader_api.get_shader("comb", "comb", "", "texture", "texture");
  SH sh2 = e.shader_api.get_shader("comb", "comb", "", "empty", "empty");
  SH sh3 = e.shader_api.colour_texture_shader();
  e.shader_api.bind_attrib(sh, 0, "in_Position");
  e.shader_api.bind_attrib(sh, 1, "in_Normal");
  e.shader_api.bind_attrib(sh, 2, "in_Color");
  e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.bind_attrib(sh, 4, "in_Position2");
  e.shader_api.link(sh);
  e.shader_api.bind_attrib(sh2, 0, "in_Position");
  e.shader_api.bind_attrib(sh2, 1, "in_Normal");
  e.shader_api.bind_attrib(sh2, 2, "in_Color");
  e.shader_api.bind_attrib(sh2, 3, "in_TexCoord");
  e.shader_api.bind_attrib(sh2, 4, "in_Position2");
  e.shader_api.link(sh2);
  e.shader_api.use(sh);
  e.shader_api.set_default_projection(sh, "in_P");

  loop.init(sh);
  loop.init(sh2);
  loop.nvidia_init();
  loop.alpha(false);


  //BM red = bm.function(&red_block, 10,10, 0);
  //BM green = bm.function(&green_block, 10,10, 0);
  
  WV sinw = wv.sinwave(2.0*3.14159, 1.0);
  BM red = wv.waveform_bitmap(sinw, 100,100, 0xffffffff, 0xff000000);
  BB red_1 = e.bool_bitmap_api.from_bitmaps_color(red, 255,255,255);
  FB red_2 = e.float_bitmap_api.distance_field(red_1);
  //FB red_2 = e.float_bitmap_api.from_bool(red_1, 1.0, 0.0);
  BM red_3 = e.float_bitmap_api.to_grayscale_color(red_2, 255,255,255,255, 0,0,0,0);

  //BM red = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);
  BM green = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0, 0.0, 0.0, 100,100,128);

  O torus = volume.torus(points.point(0.0,0.0,0.0),
			 points.point(2.0,0.0,0.0),
			 points.point(0.0,2.0,0.0), 1.0, 0.4);

  O sphere = volume.sphere(points.point(0.0, 1.0, 0.0), 1.0);
  O andnot = volume.andnot_op(torus, sphere);
  P cubes = volume.rendercubes3(andnot, 30,30,30, -2.0,2.0, -2.0,2.0, -2.0, 2.0 );
  P cubesY = e.polygon_api.recalculate_normals(cubes);
  P cubesO = e.polygon_api.flip_polygon_order(cubesY);
  LI normals = e.lines_api.normals_from_polygon(cubesY, 3.0/200.0);
  
				//std::bind(&Cube, _1,_2,_3,_4,_5,_6,_7,std::ref(e)), 40, 4.0);
  P cubesI = e.polygon_api.color_faces(cubes, 0xff888888, 0xff444444, 0xff222222, 0xff111111);
  P cubes0 = e.polygon_api.scale(cubesI, 200.0, 200.0, 200.0);
  ///P cubes00 = e.polygon_api.translate(cubes0, -2.0*20.0*30.0,-2.0*20.0*30.0, -2.0*20.0*30.0);
  P cubes2 = e.polygon_api.memoize(cubes0);
  P cubes3 = e.polygon_api.scale(cubes2, 0.4,0.4,0.4);
  P cubes4 = e.polygon_api.scale(cubes2, 2.0,2.0,2.0);
  P cubes5 = e.polygon_api.quads_to_triangles(cubes2);
  e.polygon_api.save_model(cubes5, "torus_model.obj");
  P anim = e.polygon_api.anim_endpoints(cubes5, cubes5);
  //poly.prepare(anim);
  TX tx = e.texture_api.tex_plane(128,128);
  int id = e.texture_api.unique_id();
  TX tx2 = e.texture_api.tex_assign(tx, id, 0,0, red_3);
  P cubes3_texture = e.polygon_api.sprite_bind(anim, tx2, id);
  VA va = poly.create_vertex_array(cubes3_texture);
  TXID tex = e.texture_api.prepare(tx2);
  sprite.preparesprite(red_3);
  sprite.preparesprite(green);


  SpriteObj spr(e, green, sh);
  spr.prepare();
  SpriteObj spr2(e, green, sh);
  spr2.prepare();

  LinesObj normals_obj(e, normals, sh);
  normals_obj.prepare();

  // 3d text
  //Ft font = e.font_api.newfont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 50, 50);
  //BM letter = e.font_api.glyph(font, 'a');
  //BB letter2 = e.bool_bitmap_api.from_bitmaps_color_area(letter, 1,255,1,255,1,255,1,255);
  //O o = e.bool_bitmap_api.to_volume(letter2, 20.0);
  //BM bxm = e.volume_api.render(o, 100,100, e.point_api.point(0.0,0.0,0.0),
  //			      e.point_api.point(50.0, 0.0,0.0),
  //			      e.point_api.point(0.0,50.0,0.0),
  //	1		      e.point_api.point(0.0,0.0,50.0));
  //e.sprite_api.preparesprite(bxm);
  
  PL f = plane.function(&point_func, 5, 150.0,150.0, 0);
  P p = e.polygon_api.quad_z(0.0, 10.0, 
			     0.0, 10.0,
			     0.0);
  M mat = e.matrix_api.identity();
  PL f2 = plane.render_p(p, mat, 450.0, 450.0);
  Ft font = e.font_api.newfont("FreeSans.ttf", 450,450);
  PL f3 = e.font_api.glyph_plane(font, '5', 450.0, 450.0, 30.0,30.0);
  PL f4 = e.plane_api.flip_y(f3);
  PL f5 = e.plane_api.remove_splines(f4, 0.1);
  
  LI lines = e.lines_api.from_plane(f5);
  LLA linesa = e.lines_api.prepare(lines);
  PLA pla = plane.prepare(f5 );
  PLA pla2 = plane.prepare(f4);
  //CBM cbm = plane.render(f3, 0, 0x00000000, 0xffffffff);
  //BM bm_f = e.cont_bitmap_api.sample(cbm, 150,150);
  //BM bm_f2 = e.bitmap_api.flip_y(bm_f);
  //SpriteObj spr_f(e, bm_f2, sh);
  //spr_f.prepare();
  //spr_f.set_pos(200.0,200.0);
  PT pt = e.point_api.point(100.0, 100.0, 0.0);
  //PL circle = e.plane_api.star(pt, 100.0, 60.0, 8);
  PL circle = e.plane_api.function(horse_head_func, 11, 300.0, 300.0, 0);

  PL circle2 = e.plane_api.triangulate_all(e, circle, 40, 1);

  PT pos = e.point_api.point(-70.0, -20.0, 0.0);
  V u_x = e.vector_api.vector(1.0,0.0,0.0);
  V u_y = e.vector_api.vector(0.0,1.0,0.0);
  P circle_3 = e.plane_api.to_polygon_face(circle2, pos, u_x, u_y);

  P circle_3_2 = e.polygon_api.color_faces(circle_3, 0xffffffff, 0xff888888, 0xff444444, 0xffff0000);
  P circle_3_3 = e.polygon_api.scale(circle_3_2,8.0,8.0,8.0);

  BB bb = e.plane_api.render_bool(circle, 0, 300,300);
  BM bm2 = e.bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);


  SpriteObj spr_obj(e, bm2, sh);
  spr_obj.set_pos(0.0,100.0);
  spr_obj.prepare();

  PolygonObj star_obj(e, circle_3_3, sh3);
  star_obj.prepare();

  PLA circle_pla = e.plane_api.prepare(circle2);

  


  float frame = 0.0;

  //PathRendering r;
  //r.init();

#if 0
  while(1) {  
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    //e.mainloop_api.clear();
    //r.draw();
     e.mainloop_api.swapbuffers();
    MainLoopApi::Event ev = e.mainloop_api.get_event();
    if (ev.ch==27) break;
   }
#endif
  float ff = 0.0f;
  while(1)
    {
      ff+=0.01;
      if (ff>1.0) {ff=0.0; }
      e.shader_api.set_var(sh, "in_POS", 0.0f);
      frame += 0.1;
      float time = e.mainloop_api.get_time();
      //glClearStencil(0);
      //glStencilMask(~0);
      e.mainloop_api.clear();
     e.mainloop_api.switch_to_3d(false, sh);
     //     e.shader_api.set_var(sh, "in_MV", e.matrix_api.identity());
     e.sprite_api.rendersprite(red_3,sh,0.0,0.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,100.0,100.0,1.0,1.0);
      e.sprite_api.rendersprite(red_3,sh,200.0,200.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,300.0,300.0,1.0,1.0);
      e.sprite_api.rendersprite(red_3,sh,400.0,400.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,500.0,500.0,1.0,1.0);
      e.sprite_api.rendersprite(red_3,sh,600.0,600.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh,700.0,700.0,1.0,1.0);
      e.sprite_api.rendersprite(green,sh, 799.0,599.0,1.0,1.0);
      spr.set_pos(250.0+frame*4, 250.0);
      spr.set_scale(2.0,2.0);
      spr.render();
      spr2.set_pos(250.0, 250.0-frame*4.0);
      spr2.set_scale(2.0,2.0);
      spr2.render();

      star_obj.render();
      spr_obj.render();
      //spr_f.render();

      //e.texture_api.use(tex);
      //e.texture_api.unuse(tex);
          e.mainloop_api.switch_to_3d(true,sh);
      // glPushMatrix();
      //glRotatef(time/40.0, 0.0,1.0,0.0);
      //e.polygon_api.render(cubes3, 0, 0.0,0.0,0.0);
#if 1
	  //e.texture_api.use(tex);
      e.shader_api.set_y_rotation(sh, "in_MV", time/500.0);
      e.shader_api.set_var(sh, "in_POS", ff);
      e.polygon_api.render_vertex_array(va);
      e.shader_api.set_var(sh, "in_POS", 0.0f);
      normals_obj.set_rotation_matrix(e.matrix_api.yrot(time/500.0));
      normals_obj.set_scale(200.0,200.0,200.0);
      normals_obj.render();
      //e.plane_api.render(pla);
      //e.texture_api.unuse(tex);
      //e.polygon_api.render_vertex_array(va2);
      //e.polygon_api.render_vertex_array(vba);
      //e.sprite_api.rendersprite(bxm,sh,100.0,100.0);
      //glPopMatrix();
#endif
      glColor3f(1.0,1.0,1.0);
      plane.render(pla, 200.0,150.0,1.0,1.0);
      plane.render(pla2, 400.0,150.0,1.0,1.0);
      e.shader_api.use(sh);
      e.shader_api.set_var(sh,"in_MV", e.matrix_api.trans(-350.0,0.0,0.0));
      e.lines_api.render(linesa);
      e.shader_api.use(sh2);
      e.shader_api.set_var(sh2, "in_Color", 1.0,1.0,1.0,1.0);
      plane.render(circle_pla, 300.0, 100.0, 1.0, 1.0);
      e.shader_api.use(sh);
      e.mainloop_api.fpscounter(); 
     e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event(); 
      if (ev.ch==27) break;
    }
 
}
