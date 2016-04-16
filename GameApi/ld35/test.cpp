#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

struct Envi {
  EveryApi *ev;
  PolygonObj *poly;
  PolygonObj *poly2;
  PolygonObj *flo;
  PolygonObj *grass;
  PolygonObj *sky_o;
  PTA instances;
  PTA t_instances;
  float pos_x=-0.124, pos_y=755.0;
  float rot_y=-5.58;
  float speed = 60.0;
  float rot_speed = 2.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  InteractionApi::Quake_data data;
  float bee_x = -158.8;
  float bee_y = -21;
  float bee_roty = 0.0;
  bool left=false;
  bool right=false;
  bool forward=false;
  bool backward=false;
  bool space=false;
  int space_count=0;
};

P bee(EveryApi &ev)
{
PT I1=ev.point_api.point(-80,300,30);
P I2=ev.polygon_api.sphere(I1,70,30,30);
PT I3=ev.point_api.point(80,300,30);
P I4=ev.polygon_api.sphere(I3,70,30,30);
P I5=ev.polygon_api.or_elem(I2,I4);
P I6=ev.polygon_api.color_faces(I5,0xff000000,0xff000000,0xff010101,0xff010101);
PT I7=ev.point_api.point(0,220,0);
P I8=ev.polygon_api.sphere(I7,150,30,30);
P I9=ev.polygon_api.color_faces(I8,0xffffff00,0xffffff00,0xff888800,0xff888800);
PT I10=ev.point_api.point(0.0,0.0,0.0);
P I11=ev.polygon_api.sphere(I10,100,60,60);
P I12=ev.polygon_api.scale(I11,2,2.6,2);
P I13=ev.polygon_api.color_faces(I12,0xffffff00,0xffffff00,0xff888800,0xff888800);
PT I14=ev.point_api.point(0.0,0.0,0.0);
P I15=ev.polygon_api.sphere(I14,100,60,60);
P I16=ev.polygon_api.scale(I15,2,2.6,2);
P I17=ev.polygon_api.color_faces(I16,0xff000000,0xff000000,0xff010101,0xff010101);
O I18=ev.volume_api.cube(-300,300,-200,-150,-300,300);
O I19=ev.volume_api.cube(-300,300,-100,-50,-300,300);
O I20=ev.volume_api.max_op(I18,I19);
O I21=ev.volume_api.cube(-300,300,0,50,-300,300);
O I22=ev.volume_api.cube(-300,300,100,150,-300,300);
O I23=ev.volume_api.max_op(I21,I22);
O I24=ev.volume_api.max_op(I20,I23);
P I25=ev.polygon_api.subpoly_change(I13,I17,I24);
P I26=ev.polygon_api.or_elem(I9,I25);
P I27=ev.polygon_api.or_elem(I6,I26);
PT I28=ev.point_api.point(0.0,0.0,0.0);
BO I29=ev.bool_api.sphere(ev,I28,100.0,80,80);
BO I30=ev.bool_api.cube(ev,-100,100,-100,100,-15,15,18,18);
BO I31=ev.bool_api.intersect(ev,I29,I30);
P I32=ev.bool_api.to_polygon(I31);
P I33=ev.polygon_api.scale(I32,2,4,2);
P I34=ev.polygon_api.color(I33,0x22ffffff);
P I35=ev.polygon_api.translate(I34,200,0,0);
PT I36=ev.point_api.point(0.0,0.0,0.0);
BO I37=ev.bool_api.sphere(ev,I36,100.0,80,80);
BO I38=ev.bool_api.cube(ev,-100,100,-100,100,-15,15,18,18);
BO I39=ev.bool_api.intersect(ev,I37,I38);
P I40=ev.bool_api.to_polygon(I39);
P I41=ev.polygon_api.scale(I40,2,4,2);
P I42=ev.polygon_api.color(I41,0x22ffffff);
P I43=ev.polygon_api.translate(I42,-200,0,0);
P I44=ev.polygon_api.or_elem(I35,I43);
P I45=ev.polygon_api.translate(I44,0,-200,250);
P I46=ev.polygon_api.scale(I45,0.6,0.6,0.6);
P I47=ev.polygon_api.or_elem(I27,I46);
 P I48=ev.polygon_api.scale(I47, 0.8, 1.0, -1.0);
 return I48;
}

P flower(EveryApi &ev)
{
PT I1=ev.point_api.point(0.0,0.0,0.0);
BO I2=ev.bool_api.sphere(ev,I1,200,15,15);
BO I3=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I4=ev.bool_api.intersect(ev,I2,I3);
P I5=ev.bool_api.to_polygon(I4);
P I6=ev.polygon_api.rotatex(I5,1.57);
P I7=ev.polygon_api.scale(I6,1,1.7,1);
P I8=ev.polygon_api.translate(I7,0,-300,0);
P I9=ev.polygon_api.rotatez(I8,1);
P I10=ev.polygon_api.rotatez(I9,1);
P I11=ev.polygon_api.rotatez(I10,1);
P I12=ev.polygon_api.rotatez(I11,1);
P I13=ev.polygon_api.rotatez(I12,1);
PT I14=ev.point_api.point(0.0,0.0,0.0);
BO I15=ev.bool_api.sphere(ev,I14,200,15,15);
BO I16=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I17=ev.bool_api.intersect(ev,I15,I16);
P I18=ev.bool_api.to_polygon(I17);
P I19=ev.polygon_api.rotatex(I18,1.57);
P I20=ev.polygon_api.scale(I19,1,1.7,1);
P I21=ev.polygon_api.translate(I20,0,-300,0);
P I22=ev.polygon_api.rotatez(I21,1);
P I23=ev.polygon_api.rotatez(I22,1);
P I24=ev.polygon_api.rotatez(I23,1);
P I25=ev.polygon_api.rotatez(I24,1);
PT I26=ev.point_api.point(0.0,0.0,0.0);
BO I27=ev.bool_api.sphere(ev,I26,200,15,15);
BO I28=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I29=ev.bool_api.intersect(ev,I27,I28);
P I30=ev.bool_api.to_polygon(I29);
P I31=ev.polygon_api.rotatex(I30,1.57);
P I32=ev.polygon_api.scale(I31,1,1.7,1);
P I33=ev.polygon_api.translate(I32,0,-300,0);
P I34=ev.polygon_api.rotatez(I33,1);
P I35=ev.polygon_api.rotatez(I34,1);
P I36=ev.polygon_api.rotatez(I35,1);
PT I37=ev.point_api.point(0.0,0.0,0.0);
BO I38=ev.bool_api.sphere(ev,I37,200,15,15);
BO I39=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I40=ev.bool_api.intersect(ev,I38,I39);
P I41=ev.bool_api.to_polygon(I40);
P I42=ev.polygon_api.rotatex(I41,1.57);
P I43=ev.polygon_api.scale(I42,1,1.7,1);
P I44=ev.polygon_api.translate(I43,0,-300,0);
P I45=ev.polygon_api.rotatez(I44,1);
P I46=ev.polygon_api.rotatez(I45,1);
PT I47=ev.point_api.point(0.0,0.0,0.0);
BO I48=ev.bool_api.sphere(ev,I47,200,15,15);
BO I49=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I50=ev.bool_api.intersect(ev,I48,I49);
P I51=ev.bool_api.to_polygon(I50);
P I52=ev.polygon_api.rotatex(I51,1.57);
P I53=ev.polygon_api.scale(I52,1,1.7,1);
P I54=ev.polygon_api.translate(I53,0,-300,0);
P I55=ev.polygon_api.rotatez(I54,1);
PT I56=ev.point_api.point(0.0,0.0,0.0);
BO I57=ev.bool_api.sphere(ev,I56,200,15,15);
BO I58=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I59=ev.bool_api.intersect(ev,I57,I58);
P I60=ev.bool_api.to_polygon(I59);
P I61=ev.polygon_api.rotatex(I60,1.57);
P I62=ev.polygon_api.scale(I61,1,1.7,1);
P I63=ev.polygon_api.translate(I62,0,-300,0);
P I64=ev.polygon_api.or_elem(I55,I63);
P I65=ev.polygon_api.or_elem(I46,I64);
P I66=ev.polygon_api.or_elem(I36,I65);
P I67=ev.polygon_api.or_elem(I25,I66);
P I68=ev.polygon_api.or_elem(I13,I67);
P I69=ev.polygon_api.color_faces(I68,0xffffffff,0xffffffff,0xff888888,0xff888888);
PT I70=ev.point_api.point(0.0,0.0,0.0);
BO I71=ev.bool_api.sphere(ev,I70,200,15,15);
BO I72=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I73=ev.bool_api.intersect(ev,I71,I72);
P I74=ev.bool_api.to_polygon(I73);
P I75=ev.polygon_api.rotatex(I74,1.57);
P I76=ev.polygon_api.color_faces(I75,0xffffff00,0xffffff00,0xff888800,0xff888800);
P I77=ev.polygon_api.translate(I76,0,0,20);
P I78=ev.polygon_api.or_elem(I69,I77);
PT I79=ev.point_api.point(0.0,0.0,0.0);
BO I80=ev.bool_api.sphere(ev,I79,200,15,15);
BO I81=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I82=ev.bool_api.intersect(ev,I80,I81);
P I83=ev.bool_api.to_polygon(I82);
P I84=ev.polygon_api.rotatex(I83,1.57);
P I85=ev.polygon_api.color_faces(I84,0xff008800,0xff008800,0xff004400,0xff004400);
P I86=ev.polygon_api.scale(I85,1,1.8,1);
P I87=ev.polygon_api.translate(I86,0,300,0);
P I88=ev.polygon_api.rotatez(I87,-1);
PT I89=ev.point_api.point(0.0,0.0,0.0);
BO I90=ev.bool_api.sphere(ev,I89,200,15,15);
BO I91=ev.bool_api.cube(ev,-200,200,30,60,-200,200,8,8);
BO I92=ev.bool_api.intersect(ev,I90,I91);
P I93=ev.bool_api.to_polygon(I92);
P I94=ev.polygon_api.rotatex(I93,1.57);
P I95=ev.polygon_api.color_faces(I94,0xff008800,0xff008800,0xff004400,0xff004400);
P I96=ev.polygon_api.scale(I95,1,1.8,1);
P I97=ev.polygon_api.translate(I96,0,300,0);
P I98=ev.polygon_api.rotatez(I97,1);
P I99=ev.polygon_api.or_elem(I88,I98);
PT I100=ev.point_api.point(0,-800,0);
PT I101=ev.point_api.point(0,800,0);
P I102=ev.polygon_api.cone(15,I100,I101,60,60);
P I103=ev.polygon_api.color_faces(I102,0xff008800,0xff008800,0xff004400,0xff004400);
P I104=ev.polygon_api.translate(I103,0,800,0);
P I105=ev.polygon_api.or_elem(I99,I104);
P I106=ev.polygon_api.translate(I105,0,-1600,0);
P I107=ev.polygon_api.or_elem(I78,I106);
 P I108=ev.polygon_api.scale(I107, 0.8, 1.0, -1.0);

 return I108;
}
P sky(EveryApi &ev)
{
PT I1=ev.point_api.point(0.0,0.0,0.0);
P I2=ev.polygon_api.sphere(I1,10000,30,30);
P I3=ev.polygon_api.color(I2,0xff884422);
PT I4=ev.point_api.point(0.0,0.0,0.0);
P I5=ev.polygon_api.sphere(I4,10000,30,30);
P I6=ev.polygon_api.color(I5,0xff4488ff);
O I7=ev.volume_api.cube(-200000,200000,0,100000,-200000,200000);
P I8=ev.polygon_api.subpoly_change(I3,I6,I7);
 return I8;
}


void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d(0xff888888);

    M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(0.0,0.0,400.0);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,-400.0);
    M a_mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(a_m3,a_m),a_m2);

    M b_a_m = env->ev->matrix_api.yrot(-env->bee_roty+3.14159);
    //M b_a_m2 = env->ev->matrix_api.trans(0.0,0.0,400.0);
    //M b_a_m3 = env->ev->matrix_api.trans(0.0,0.0,-400.0);
    //M b_a_mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(b_a_m3,b_a_m),b_a_m2);


    env->sky_o->set_rotation_matrix2(a_mm);
    //env->sky_o->set_pos(env->pos_x, -800.0, env->pos_y);
    env->sky_o->render();

    if (env->space)
      {
	float val = 1.0;
	if (env->space_count > 100)
	  {
	    val = -1.0;
	  }
	if (env->space_count > 200)
	  {
	    env->space_count = 0;
	  }
	env->poly->explode(env->ev->point_api.point(0.0,0.0,0.0), val);
	env->poly2->explode(env->ev->point_api.point(0.0,0.0,0.0), val);

	env->space_count++;
      }
    if (!env->space)
      {
	env->poly->set_rotation_matrix(b_a_m);
	env->poly->set_pos(env->bee_x, -200.0, env->bee_y);
	env->poly->render();
      } else {
      env->poly2->set_rotation_matrix(b_a_m);
      env->poly2->set_pos(env->bee_x, -200.0, env->bee_y);
      env->poly2->render();
    }


    env->flo->set_rotation_matrix2(a_mm);
    env->flo->set_pos(env->pos_x, 0.0, env->pos_y);
    env->flo->render_instanced(env->instances);

    env->grass->set_rotation_matrix2(a_mm);
    env->grass->set_pos(env->pos_x, -800.0, env->pos_y);
    env->grass->render_instanced(env->t_instances);

    //std::cout << "BEE:" << env->bee_x << " " << env->bee_y << std::endl;
    //std::cout << "CAM:" << env->pos_x << " " << env->pos_y << std::endl;
    
    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();


    env->pos_y-=env->speed_y; env->pos_x-=env->speed_x; 
    if (env->right) { env->bee_roty += env->rot_speed; }
    if (env->left) { env->bee_roty -= env->rot_speed; }
    if (env->forward) { 
      env->pos_y-=env->speed_y; env->pos_x-=env->speed_x; 
    }
    if (env->backward) {
	env->pos_y+=env->speed_y; env->pos_x+=env->speed_x; 
    }
    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << std::hex << e.type << ":" << e.ch << std::endl;
	if (e.type==0x300)
	  {
	  }
#ifndef EMSCRIPTEN
    if (e.ch==27) { exit(0); }
#endif
    float bee_move_speed = 50.0;
    if ((e.ch&0xff)==0x4f && e.type==0x300)
      { // right
	env->right = true;
      }
    if ((e.ch&0xff)==0x50 && e.type==0x300)
      { // left
	env->left=true;
      }
    if ((e.ch&0xff)==0x52 && e.type==0x300)
      { // forward
	env->forward=true;
      }
    if ((e.ch&0xff)==0x51 && e.type==0x300)
      { // backward
	env->backward = true;
      }

    if ((e.ch&0xff)==0x4f && e.type==0x301)
      { // right
	env->right = false;
      }
    if ((e.ch&0xff)==0x50 && e.type==0x301)
      { // left
	env->left=false;
      }
    if ((e.ch&0xff)==0x52 && e.type==0x301)
      { // forward
	env->forward=false;
      }
    if ((e.ch&0xff)==0x51 && e.type==0x301)
      { // backward
	env->backward = false;
      }
    if ((e.ch&0xff)==' ' && e.type==0x300)
      {
	env->space = true;
      }
    if ((e.ch&0xff)==' ' && e.type==0x301)
      {
	env->space = false;
      }

    //    InteractionApi::quake_movement(e, env->pos_x, env->pos_y, env->rot_y,
    //		   env->data, env->speed_x, env->speed_y,
    //				   100.0, 1.0*3.14159*2.0/360.0);
#if 1
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { 
    }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) 
      { 
      }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env->rot_y -= env->rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env->rot_y += env->rot_speed; }
    env->speed_x = env->speed*cos(env->bee_roty+3.14159/2.0+1.0);
    env->speed_y = env->speed*sin(env->bee_roty+3.14159/2.0+1.0);
#endif
      }

    //    std::cout << env->pos_y << " " << env->pos_x << " " << env->rot_y << std::endl;

}

int main() {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.get_normal_shader("comb", "comb", "", "inst:colour", "colour:light:light");

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);
  ev.mainloop_api.alpha(true);

#if 0
  ev.tracker_api.play_mp3(".\\[Free-scores.com]_rimsky-korsakov-nikolai-flight-the-bumblebee-679.mp3");
#endif


  TS tri_strip = ev.ts_api.load("test.tri");
  P tri_p = ev.ts_api.to_poly(tri_strip);
  P tri_c = ev.polygon_api.color(tri_p, 0xff008800);
  P tri_c2 = ev.polygon_api.scale(tri_c, 1.0, -1.0, 1.0);

  PT t_pos = ev.point_api.point(0.0, 0.0, 0.0);
  V t_u_x = ev.vector_api.vector(20000.0, 0.0, 0.0);
  V t_u_y = ev.vector_api.vector(0.0, 0.0, 20000.0);
  PTS t_pts = ev.points_api.random_plane(t_pos, t_u_x, t_u_y, 10000);
  PTA t_instances = ev.points_api.prepare(t_pts);



  PT center1 = ev.point_api.point(0.0,0.0,0.0);
  PT center2 = ev.point_api.point(30.0,0.0,50.0);
  //P p1 = ev.polygon_api.sphere(center1, 50.0, 30*2*2, 15*2*2);
  float cs = 40.0;

  P p32 = bee(ev);
  P p32_a = ev.polygon_api.rotatex(p32, 3.14159*2.0*90.0/360.0);


  PT pos = ev.point_api.point(0.0, 0.0, 0.0);
  V u_x = ev.vector_api.vector(20000.0, 0.0, 0.0);
  V u_y = ev.vector_api.vector(0.0, 0.0, 20000.0);
  PTS pts = ev.points_api.random_plane(pos, u_x, u_y, 100);
  PTA instances = ev.points_api.prepare(pts);


  P p32_b = ev.polygon_api.color_range(p32_a, 0xffffffff, 0xffff0000);
  PolygonObj poly2(ev, p32_b, sh);
  poly2.prepare(true);
  PolygonObj poly(ev, p32_a, sh);
  poly.prepare(true);


  P p31 = flower(ev);
  PolygonObj flo(ev, p31, sh2);
  flo.prepare();

  PolygonObj grass(ev, tri_c2, sh2);
  grass.prepare();

  P sky_p = sky(ev);

  PolygonObj sky_o(ev, sky_p, sh);  
  sky_o.prepare();

  env.ev = &ev;
  env.poly = &poly;
  env.poly2 = &poly2;
  env.flo = &flo;
  env.instances = instances;

  env.grass = &grass;
  env.t_instances = t_instances;
  env.sky_o = &sky_o;

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
