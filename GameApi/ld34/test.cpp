#include "GameApi.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <math.h>

using namespace GameApi;

V func(int face, int point, EveryApi &ev)
{
  switch(point)
    {
    case 0: return ev.vector_api.vector(0.0,0.0,0.0);
    case 1: return ev.vector_api.vector(1.0,0.0,0.0);
    case 2: return ev.vector_api.vector(1.0,1.0,0.0);
    case 3: return ev.vector_api.vector(0.0,1.0,0.0);
    };
  return ev.vector_api.vector(0.0,0.0,0.0);
}

PT func2(int face, int point, EveryApi &ev)
{
  switch(point)
    {
    case 0: return ev.point_api.point(0.0,1.0,0.0);
    case 1: return ev.point_api.point(1.0,1.0,0.0);
    case 2: return ev.point_api.point(1.0,0.0,0.0);
    case 3: return ev.point_api.point(0.0,0.0,0.0);
    };
  return ev.point_api.point(0.0,0.0,0.0);
}

struct Envi
{
  EveryApi *ev;
  float f;
  SH sh;
  PolygonObj *poly;
  FBO fbo;
  PolygonObj *poly2;
  bool forward=false;
  bool backward = false;
  bool left = false;
  bool right = false;
  float pos_x=0.0;
  float pos_z=0.0;
  float rotangle=0.0;
  float speed = 15.0;
  float rotspeed = 15.0*3.14159*2.0/360.0;
  float time=0.0;
  float old_time=0.0;
};
void iter(void *data)
{
  Envi &envi = *(Envi*)data;
  envi.f = envi.ev->mainloop_api.get_time()/1000.0;

  envi.time = envi.ev->mainloop_api.get_time()/10.0;
  //envi.f += 1.0/10.0;
  //envi.time +=100.0/10.0;
  envi.speed = envi.time - envi.old_time;
  envi.old_time = envi.time;

  //envi.ev->fbo_api.bind_fbo(envi.fbo);
    envi.ev->mainloop_api.clear();
    envi.ev->shader_api.use(envi.sh);
    envi.ev->shader_api.set_var(envi.sh, "time", envi.f);
    envi.ev->shader_api.set_var(envi.sh, "rotangle", envi.rotangle);
    envi.ev->shader_api.set_var(envi.sh, "player_pos", envi.pos_x, 0.0, envi.pos_z);
    envi.poly->render();
    //envi.ev->fbo_api.bind_screen(800,600);
    //envi.ev->mainloop_api.clear();    
    //envi.poly2->render();

    //envi.ev->mainloop_api.fpscounter();
    // swapbuffers
    envi.ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e;
    while((e = envi.ev->mainloop_api.get_event()).last==true)
      { 
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
	
	if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { envi.forward = true; }
	if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { envi.forward = false; }
	if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { envi.backward = true; }
	if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { envi.backward = false; }
	if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { envi.left = true; }
	if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { envi.left = false; }
	if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { envi.right = true; }
	if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { envi.right = false; }
      }
    if (envi.forward)
      {
	envi.pos_x += envi.speed*cos(-envi.rotangle+3.14159/2.0);
	envi.pos_z += envi.speed*sin(-envi.rotangle+3.14159/2.0);
      }
    if (envi.backward)
      {
	envi.pos_x -= envi.speed*cos(-envi.rotangle+3.14159/2.0);
	envi.pos_z -= envi.speed*sin(-envi.rotangle+3.14159/2.0);
      }
    if (envi.left)
      {
	envi.rotangle-=envi.rotspeed;
      }
    if (envi.right)
      {
	envi.rotangle+=envi.rotspeed;
      }

}
SFO shader_character(EveryApi &ev, SFO outside)
{
  PT center = ev.point_api.point(0.0,-30.0,0.0);
  PT hand_center = ev.point_api.point(40.0, 10.0, 0.0);
  PT hand_center2= ev.point_api.point(-40.0, 10.0, 0.0);
  SFO head_1 = ev.sh_api.sphere(center, 30.0);
  SFO head_2 = ev.sh_api.line(0.0, -30.0, 28.0,
			      0.0, -30.0, 50.0,
			      10.0, 3.0);
  SFO head_3 = ev.sh_api.or_elem(head_1, head_2);
  PT eye_center1 = ev.point_api.point(10.0, -40.0, 25.0);
  SFO head_4 = ev.sh_api.sphere(eye_center1, 10.0);
  PT eye_center2 = ev.point_api.point(-10.0, -40.0,25.0);
  SFO head_5 = ev.sh_api.sphere(eye_center2, 10.0);
  SFO head_6 = ev.sh_api.or_elem(head_4, head_5);
  SFO head_7 = ev.sh_api.and_not(head_3, head_6);
  SFO head_8 = ev.sh_api.color(head_7, 1.0, 0.0, 1.0, 1.0);
  SFO hat_1 = ev.sh_api.line( 0.0, -50.0, -10.0,
			      0.0, -80.0, -30.0,
			      20.0, 2.0);
  SFO hat_2 = ev.sh_api.or_elem(head_8, hat_1);


  SFO shoulders_1 = ev.sh_api.rounded_cube(-30.0, 30.0, 0.0, 20.0, -20.0, 20.0, 5.0);

  SFO hand_1 = ev.sh_api.rounded_cube(30.0, 50.0, 0.0, 50.0, -10.0, 10.0, 5.0);
  SFO hand_2 = ev.sh_api.rounded_cube(-50.0, -30.0, 0.0, 50.0, -10.0, 10.0, 5.0);
  SFO hand_tip_1 = ev.sh_api.rounded_cube(30.0,50.0, 50.0, 70.0, -10.0, 10.0, 5.0);
  SFO hand_tip_2 = ev.sh_api.rounded_cube(-50.0,-30.0, 50.0, 70.0, -10.0, 10.0, 5.0);

  SFO body_1 = ev.sh_api.rounded_cube(-30.0, 30.0, 20.0, 70.0, -20.0, 20.0, 5.0);
  SFO legs_1 = ev.sh_api.rounded_cube(-30.0, -5.0, 70.0, 100.0, -10.0, 10.0, 5.0);
  SFO legs_2 = ev.sh_api.rounded_cube(5.0,30.0, 70.0, 100.0, -10.0, 10.0, 5.0);

  SFO tip_1 = ev.sh_api.cube(-30.0, -5.0, 90.0, 100.0, -10.0+5.0, 20.0);
  SFO tip_2 = ev.sh_api.cube(5.0,30.0, 90.0, 100.0, -10.0+5.0, 20.0);
  
  SFO all_1 = ev.sh_api.or_elem(hat_2, shoulders_1);
  SFO all_2 = hand_1;
  SFO all_2a = ev.sh_api.or_elem(all_2, hand_tip_1);
  SFO all_2b = ev.sh_api.rot_x(all_2a, 1.5, hand_center);
  SFO all_2c = ev.sh_api.bind_arg(all_2b, "angle", "1.5*cos(time*3.0+3.14159)");
  SFO all_2d = ev.sh_api.color(all_2c, 1.0,0.5,0.0, 1.0);
  SFO all_3 = hand_2;
  SFO all_3a = ev.sh_api.or_elem(all_3, hand_tip_2);
  SFO all_3b = ev.sh_api.rot_x(all_3a, -1.5, hand_center2);
  SFO all_3c = ev.sh_api.bind_arg(all_3b, "angle", "1.5*cos(time*3.0)");
  SFO all_3d = ev.sh_api.color(all_3c, 1.0,0.5,0.0, 1.0);
  SFO all_4 = ev.sh_api.or_elem(all_3d, body_1);
  SFO all_4a = ev.sh_api.or_elem(all_4, all_1);
  SFO all_4b = ev.sh_api.or_elem(all_4a, all_2d);
  SFO all_5 = legs_1;
  SFO all_6 = ev.sh_api.or_elem(all_5, tip_1);
  SFO all_6a = ev.sh_api.trans(all_6, 0.0, 0.0, 10.0);
  SFO all_6b = ev.sh_api.bind_arg(all_6a, "delta", "vec3(0.0,0.0,10.0*cos(time*3.0))");
  SFO all_6c = ev.sh_api.color(all_6b, 0.0, 1.0, 1.0, 1.0);
  SFO all_7 = legs_2;
  SFO all_8 = ev.sh_api.or_elem(all_7, tip_2);
  SFO all_8a = ev.sh_api.trans(all_8, 0.0, 0.0, 10.0);
  SFO all_8b = ev.sh_api.bind_arg(all_8a, "delta", "vec3(0.0,0.0,10.0*cos(time*3.0+3.14159))");
  SFO all_8c = ev.sh_api.color(all_8b, 0.0, 1.0, 1.0, 1.0);
  SFO all_9 = ev.sh_api.or_elem(all_8c, all_4b);
  SFO all_10 = ev.sh_api.or_elem(all_9, all_6c);

  PT pt_center = ev.point_api.point(0.0,10.0,0.0);
  SFO bounding_sphere = ev.sh_api.sphere(pt_center, 100.0);
  SFO char_2a = ev.sh_api.bounding_primitive(bounding_sphere, all_10, outside);

  
  SFO all_11 = ev.sh_api.rot_y(all_10, 3.14159/2.0);
  SFO all_12 = ev.sh_api.bind_arg(all_11, "angle", "rotangle");
  SFO all_13 = ev.sh_api.trans(all_12, 0.0,0.0,0.0);
  SFO all_14 = ev.sh_api.bind_arg(all_13, "delta", "player_pos");
  return all_14;
}

SFO background(EveryApi &ev)
{
  SFO tex_cube_1 =  
    ev.sh_api.texture_box(-200.0+0.0,-200.0+800.0,
			  90.0, 105.0,
			  -100.0,100.0);
  SFO tex_cube_11 = ev.sh_api.color(tex_cube_1, 0.0,1.0,1.0,1.0);
  SFO tex_cube_1a = ev.sh_api.rot_x(tex_cube_11, 90.0*3.14159*2.0/360.0);
  SFO tex_cube_1b = ev.sh_api.trans(tex_cube_1a, -100.0, 0.0, -100.0);

  return tex_cube_1a;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default(); 
  
  PT center = ev.point_api.point(0.0,0.0,0.0);
  
  SFO skybox = ev.sh_api.sphere(center, 1000.0);
  SFO skybox_2 = ev.sh_api.sphere(center, 990.0);
  SFO skybox_3 = ev.sh_api.and_not(skybox, skybox_2);
  SFO skybox_4 = ev.sh_api.color(skybox_3, 0.7,0.8,1.0, 0.0);




  SFO plane_1 = ev.sh_api.plane(ev.point_api.point(0.0,100.0,0.0),
				ev.vector_api.vector(4.0,0.0,0.0),
				ev.vector_api.vector(0.0,0.0,4.0));
  SFO plane_11 = ev.sh_api.grayscale(plane_1);
  SFO plane_2 = ev.sh_api.or_elem(plane_11, skybox_4);

  SFO plane_2a = ev.sh_api.stop_generation(plane_2);
  SFO char_1 = shader_character(ev, plane_2a);
  SFO char_2 = ev.sh_api.or_elem(plane_2, char_1);

  SFO char_2w = background(ev);
  SFO char_12 = ev.sh_api.or_elem(char_2, char_2w);


  SFO rot_y = ev.sh_api.rot_y(char_12, 0.0);
  SFO rot_y_2 = ev.sh_api.bind_arg(rot_y, "angle", "time/1.25");




  SFO rot_y_22 = ev.sh_api.trans(rot_y_2, 0.0, 0.0, 0.0);
  SFO rot_y_3 = ev.sh_api.stop_generation(rot_y_22);

  SFO color = ev.sh_api.color_from_normal(rot_y_3 );
  SFO mix_color = ev.sh_api.mix_color(rot_y_22, color, 1.0-0.4);



  V light_dir = ev.vector_api.vector(-20.0, -40.0, -20.0);
  SFO soft_shadow = ev.sh_api.soft_shadow(mix_color, light_dir, 0.0, 1.0, 2.0, 8.0);
  SFO ao = ev.sh_api.ambient_occulsion(soft_shadow, 6.2, 50.0);


  SFO sphere_render = ev.sh_api.render(ao);

  SH sh = ev.shader_api.get_normal_shader("screen", "screen", "", "", "", false, sphere_render);
  SH sh2 = ev.shader_api.texture_shader();

  // rest of the initializations
  ev.mainloop_api.init(sh);
  ev.mainloop_api.init(sh2);

  ev.mainloop_api.alpha(true);

  P p = ev.polygon_api.quad_z(0.0, 800.0,
			      0.0, 600.0,
			      0.0);

  P p2 = ev.polygon_api.normal_function(p, std::bind(func,_1,_2,std::ref(ev)));
  P p3 = ev.polygon_api.texcoord_function(p2, std::bind(func2,_1,_2,std::ref(ev)));
 

  // Texture loading
  //Ft font = ev.font_api.newfont("FreeSans.ttf", 240,240);
  //BM bitmap = ev.font_api.font_string(font, "Powerset Functor", 20);
  BM bitmap = ev.bitmap_api.loadbitmap("house.png");
  BB bb = ev.bool_bitmap_api.from_bitmaps_color(bitmap, 255,255,255);
  BB bb2 = ev.bool_bitmap_api.not_bitmap(bb);
  FB dist = ev.float_bitmap_api.distance_field(bb2);
  BM dist_bm = ev.float_bitmap_api.to_grayscale_color(dist, 255,255,255,255, 0,0,0,0);

  TX tex = ev.texture_api.tex_bitmap(dist_bm);
  TXID tex_id = ev.texture_api.prepare(tex);

  PolygonObj poly(ev, p2, sh);
  poly.bind_texture(0,tex_id);
  poly.prepare();
  float f = 0.0;




  Envi envi;
  //envi.fbo = fbo;
  envi.poly = &poly;
  // envi.poly2 = &poly2;
  envi.f = f ;
  envi.sh = sh;
  envi.ev = &ev;


#ifndef EMSCRIPTEN
  while(1) {
    iter(&envi);
    // ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&envi, 0,1);
#endif


}
