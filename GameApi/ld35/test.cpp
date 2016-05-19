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
  PolygonObj *poly3;
  PolygonObj *flo;
  PolygonObj *grass;
  PolygonObj *sky_o;
  PolygonObj *enemy_obj;
  PTA instances;
  PTA t_instances;
  PTA e_instances;
  float pos_x=-0.124, pos_y=755.0;
  float rot_y=-5.58;
#ifndef EMSCRIPTEN
  float speed = 60.0;
  float rot_speed = 2.0*3.14159*2.0/360.0;
#else
  float speed = 60.0*2.0;
  float rot_speed = 2.0*2.0*3.14159*2.0/360.0;
#endif
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
  bool damage = false;
};

P bee(EveryApi &ev)
{
  float div = 1.0;
PT I1=ev.point_api.point(-80,300,30);
P I2=ev.polygon_api.sphere(I1,70,30/div,30/div);
PT I3=ev.point_api.point(80,300,30);
P I4=ev.polygon_api.sphere(I3,70,30/div,30/div);
P I5=ev.polygon_api.or_elem(I2,I4);
P I6=ev.polygon_api.color_faces(I5,0xff000000,0xff000000,0xff010101,0xff010101);
PT I7=ev.point_api.point(0,220,0);
P I8=ev.polygon_api.sphere(I7,150,30/div,30/div);
P I9=ev.polygon_api.color_faces(I8,0xffffff00,0xffffff00,0xff888800,0xff888800);
PT I10=ev.point_api.point(0.0,0.0,0.0);
P I11=ev.polygon_api.sphere(I10,100,60/div,60/div);
P I12=ev.polygon_api.scale(I11,2,2.6,2);
P I13=ev.polygon_api.color_faces(I12,0xffffff00,0xffffff00,0xff888800,0xff888800);
PT I14=ev.point_api.point(0.0,0.0,0.0);
P I15=ev.polygon_api.sphere(I14,100,60/div,60/div);
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
BO I29=ev.bool_api.sphere(ev,I28,100.0,80/div,80/div);
BO I30=ev.bool_api.cube(ev,-100,100,-100,100,-15,15,18/div,18/div);
BO I31=ev.bool_api.intersect(ev,I29,I30);
P I32=ev.bool_api.to_polygon(I31);
P I33=ev.polygon_api.scale(I32,2,4,2);
P I34=ev.polygon_api.color(I33,0x22ffffff);
P I35=ev.polygon_api.translate(I34,200,0,0);
PT I36=ev.point_api.point(0.0,0.0,0.0);
BO I37=ev.bool_api.sphere(ev,I36,100.0,80/div,80/div);
BO I38=ev.bool_api.cube(ev,-100,100,-100,100,-15,15,18/div,18/div);
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

P enemy(EveryApi &ev)
{
PT I1=ev.point_api.point(0.0,0.0,0.0);
BO I2=ev.bool_api.sphere(ev,I1,100,30,30);
BO I3=ev.bool_api.cube(ev,-100,100,0,100,-100,100,18,18);
BO I4=ev.bool_api.and_not(ev,I2,I3);
P I5=ev.bool_api.to_polygon(I4);
P I6=ev.polygon_api.scale(I5,1,-1,1);
P I7=ev.polygon_api.color_faces(I6,0xffff0000,0xffff0000,0xff880000,0xff880000);
PT I8=ev.point_api.point(0.0,0.0,0.0);
BO I9=ev.bool_api.sphere(ev,I8,100,30,30);
BO I10=ev.bool_api.cube(ev,-100,100,0,100,-100,100,18,18);
BO I11=ev.bool_api.and_not(ev,I9,I10);
P I12=ev.bool_api.to_polygon(I11);
P I13=ev.polygon_api.scale(I12,1,-1,1);
P I14=ev.polygon_api.color_faces(I13,0xff000000,0xff000000,0xff010101,0xff010101);
PT I15=ev.point_api.point(-100,40,0);
O I16=ev.volume_api.sphere(I15,30);
PT I17=ev.point_api.point(0,40,-100);
O I18=ev.volume_api.sphere(I17,30);
O I19=ev.volume_api.max_op(I16,I18);
PT I20=ev.point_api.point(100,40,0);
O I21=ev.volume_api.sphere(I20,30);
O I22=ev.volume_api.max_op(I19,I21);
PT I23=ev.point_api.point(0,40,100);
O I24=ev.volume_api.sphere(I23,30);
O I25=ev.volume_api.max_op(I22,I24);
P I26=ev.polygon_api.subpoly_change(I7,I14,I25);
PT I27=ev.point_api.point(0.0,0.0,0.0);
BO I28=ev.bool_api.sphere(ev,I27,100,30,30);
BO I29=ev.bool_api.cube(ev,-100,100,0,100,-100,100,18,18);
BO I30=ev.bool_api.and_not(ev,I28,I29);
P I31=ev.bool_api.to_polygon(I30);
P I32=ev.polygon_api.scale(I31,1,-1,1);
P I33=ev.polygon_api.translate(I32,0,0,-200);
P I34=ev.polygon_api.color_faces(I33,0xff000000,0xff000000,0xff010101,0xff010101);
P I35=ev.polygon_api.scale(I34,0.5,0.5,0.5);
P I36=ev.polygon_api.or_elem(I26,I35);
 return I36;
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
	//env->poly->explode(env->ev->point_api.point(0.0,0.0,0.0), val);
	//env->poly2->explode(env->ev->point_api.point(0.0,0.0,0.0), val);

	env->space_count++;
      }
    if (!env->damage)
      {
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
      } else {
	  env->poly3->set_rotation_matrix(b_a_m);
	  env->poly3->set_pos(env->bee_x, -200.0, env->bee_y);
	  env->poly3->render();
      }

    env->flo->set_rotation_matrix2(a_mm);
    env->flo->set_pos(env->pos_x, 0.0, env->pos_y);
    env->flo->render_instanced(env->instances);

    env->grass->set_rotation_matrix2(a_mm);
    env->grass->set_pos(env->pos_x, -800.0, env->pos_y);
    env->grass->render_instanced(env->t_instances);

    env->enemy_obj->set_rotation_matrix2(a_mm);
    env->enemy_obj->set_pos(env->pos_x, -100.0, env->pos_y);
    env->enemy_obj->render_instanced(env->e_instances);

    env->damage = false;
    int s = 20;
    for(int i=0;i<s;i++)
      {
	float *pt = env->ev->points_api.point_access(env->e_instances, i);
	float x = pt[0];
	//float y = pt[1];
	float z = pt[2];
	float dx = x - env->pos_x -env->bee_x;
	float dz = z - env->pos_y -env->bee_y;
	dx/=300;
	dz/=300;
	//pt[0]+=dx;
	//pt[2]+=dz;


	//std::cout << pt[0] << " " << pt[2] << std::endl;
	if (fabs(pt[0]+env->bee_x + env->pos_x)-400.0 < 500.0 &&
	    fabs(pt[2]+env->bee_y + env->pos_y)-400.0 < 500.0)
	  {
	    env->damage = true;
	  }

      }

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

    if (env->pos_x <-20000.0) { env->pos_x = -20000.0; }
    if (env->pos_y <-20000.0) { env->pos_y = -20000.0; }
    if (env->pos_x >0.0) { env->pos_x = 0.0; }
    if (env->pos_y >0.0) { env->pos_y = 0.0; }
    

    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << std::hex << e.type << ":" << e.ch << std::endl;
	if (e.type==0x300)
	  {
	  }
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
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
    if (((e.ch&0xff)==' '||(e.ch&0xff)==44) && e.type==0x300)
      {
	env->space = true;
      }
    if (((e.ch&0xff)==' '||(e.ch&0xff)==44) && e.type==0x301)
      {
	env->space = false;
      }

    if ((e.ch&0xff)=='b' && e.type==0x300)
      {
	env->damage = true;
      }
    if ((e.ch&0xff)=='b' && e.type==0x301)
      {
	env->damage = false;
      }

    //    InteractionApi::quake_movement(e, env->pos_x, env->pos_y, env->rot_y,
    //		   env->data, env->speed_x, env->speed_y,
    //				   100.0, 1.0*3.14159*2.0/360.0);
#if 0
    if ((e.ch=='w' || e.ch==26||e.ch==82)&& e.type==0x300) { 
    }
    if ((e.ch=='s' || e.ch==22||e.ch==81)&& e.type==0x300) 
      { 
      }


    if ((e.ch=='a'||e.ch==4||e.ch==80)&& e.type==0x300) { env->rot_y -= env->rot_speed; }
    if ((e.ch=='d'||e.ch==7||e.ch==79)&& e.type==0x300) { env->rot_y += env->rot_speed; }
#endif
    env->speed_x = env->speed*cos(env->bee_roty+3.14159/2.0+1.0);
    env->speed_y = env->speed*sin(env->bee_roty+3.14159/2.0+1.0);
      }

    //    std::cout << env->pos_y << " " << env->pos_x << " " << env->rot_y << std::endl;

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
  
  cross_x*=15.0;
  cross_y*=15.0;
  cross_z*=15.0;

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

#if 1
  ev.tracker_api.play_ogg("bee.ogg");
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

  PT e_t_pos = ev.point_api.point(0.0, 0.0, 0.0);
  V e_t_u_x = ev.vector_api.vector(20000.0, 0.0, 0.0);
  V e_t_u_y = ev.vector_api.vector(0.0, 0.0, 20000.0);
  PTS e_t_pts = ev.points_api.random_plane(e_t_pos, e_t_u_x, e_t_u_y, 20);
  PTA e_instances = ev.points_api.prepare(e_t_pts);

  


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


  P p32_b = ev.polygon_api.color_range(p32_a, 0xffffffff, 0x00000000, 0xffffffff, 0xffff0000);
  P p32_c = ev.polygon_api.from_polygon(p32_b, [&ev](int i, PT p1, PT p2, PT p3, PT p4) { return poly_func(i, p1,p2,p3,p4,ev); });
  P p32_d = ev.polygon_api.color_faces(p32_c, 0xff888888, 0xff444444, 0xff222222, 0xff666666);

  P r32_d0 = ev.polygon_api.color(p32_a, 0xffff0000);
  P r32_d1 = ev.polygon_api.recalculate_normals(p32_a);
  P r32_d2 = ev.polygon_api.color_from_normals(r32_d1);
  P r32_d = ev.polygon_api.mix_color(r32_d0, r32_d2, 0.5);

  std::cout << "BEE (red): " << std::endl;
  ev.polygon_api.print_stat(r32_d);
  PolygonObj poly3(ev, r32_d, sh);
  poly3.prepare(true);
  std::cout << "BEE (metal): " << std::endl;
  ev.polygon_api.print_stat(p32_d);
  PolygonObj poly2(ev, p32_d, sh);
  poly2.prepare(true);
  std::cout << "BEE: " << std::endl;
  ev.polygon_api.print_stat(p32_a);
  //ev.polygon_api.save_model(p32_a, "normal_bee.obj");
  PolygonObj poly(ev, p32_a, sh);
  poly.prepare(true);

  P en = enemy(ev);
  std::cout << "Enemy: " << std::endl;
  ev.polygon_api.print_stat(en);
  PolygonObj enemy_obj(ev, en, sh2);
  enemy_obj.prepare();

  P p31 = flower(ev);
  std::cout << "Flower: " << std::endl;
  ev.polygon_api.print_stat(p31);

  PolygonObj flo(ev, p31, sh2);
  flo.prepare();

  std::cout << "Grass: " << std::endl;
  ev.polygon_api.print_stat(tri_c2);
  PolygonObj grass(ev, tri_c2, sh2);
  grass.prepare();

  P sky_p = sky(ev);

  std::cout << "SKY: " << std::endl;
  ev.polygon_api.print_stat(sky_p);
  PolygonObj sky_o(ev, sky_p, sh);  
  sky_o.prepare();

  env.ev = &ev;
  env.poly = &poly;
  env.poly2 = &poly2;
  env.poly3 = &poly3;
  env.enemy_obj = &enemy_obj;
  env.flo = &flo;
  env.instances = instances;

  env.grass = &grass;
  env.t_instances = t_instances;
  env.e_instances = e_instances;
  env.sky_o = &sky_o;


#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 30,1);
#endif



}
