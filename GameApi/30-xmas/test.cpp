#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

using namespace GameApi;

struct Envi {
  EveryApi *ev;
  //PolygonObj *poly;
  MN move;
  ML mainloop;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  InteractionApi::Quake_data data;
  bool logo_shown = true;
  SH color_sh;
  SH texture_sh;
  SH arr_texture_sh;
  int counter = 0;
};

ML train(EveryApi &ev)
{
P I1=ev.polygon_api.rounded_cube(ev,-300,300,-100,100,-150,150,60);
PT I2=ev.point_api.point(0,0,150);
PT I3=ev.point_api.point(0,0,151);
P I4=ev.polygon_api.cone(30,I2,I3,0,60);
PT I5=ev.point_api.point(0,0,151);
PT I6=ev.point_api.point(0,0,200);
P I7=ev.polygon_api.cone(30,I5,I6,60,60);
P I8=ev.polygon_api.or_elem(I4,I7);
PT I9=ev.point_api.point(0,0,201);
PT I10=ev.point_api.point(0,0,200);
P I11=ev.polygon_api.cone(30,I9,I10,60,1);
P I12=ev.polygon_api.or_elem(I8,I11);
P I13=ev.polygon_api.rotatey(I12,0.1);
P I14=ev.polygon_api.rotatez(I13,0.78);
P I15=ev.polygon_api.translate(I14,-220,150,-50);
PT I16=ev.point_api.point(0,0,150);
PT I17=ev.point_api.point(0,0,151);
P I18=ev.polygon_api.cone(30,I16,I17,0,60);
PT I19=ev.point_api.point(0,0,151);
PT I20=ev.point_api.point(0,0,200);
P I21=ev.polygon_api.cone(30,I19,I20,60,60);
P I22=ev.polygon_api.or_elem(I18,I21);
PT I23=ev.point_api.point(0,0,201);
PT I24=ev.point_api.point(0,0,200);
P I25=ev.polygon_api.cone(30,I23,I24,60,1);
P I26=ev.polygon_api.or_elem(I22,I25);
P I27=ev.polygon_api.rotatey(I26,0.1);
P I28=ev.polygon_api.rotatez(I27,0.78);
P I29=ev.polygon_api.translate(I28,-220,150,-50);
P I30=ev.polygon_api.translate(I29,450,0,0);
P I31=ev.polygon_api.or_elem(I15,I30);
PT I32=ev.point_api.point(0,0,150);
PT I33=ev.point_api.point(0,0,151);
P I34=ev.polygon_api.cone(30,I32,I33,0,60);
PT I35=ev.point_api.point(0,0,151);
PT I36=ev.point_api.point(0,0,200);
P I37=ev.polygon_api.cone(30,I35,I36,60,60);
P I38=ev.polygon_api.or_elem(I34,I37);
PT I39=ev.point_api.point(0,0,201);
PT I40=ev.point_api.point(0,0,200);
P I41=ev.polygon_api.cone(30,I39,I40,60,1);
P I42=ev.polygon_api.or_elem(I38,I41);
P I43=ev.polygon_api.rotatey(I42,0.1);
P I44=ev.polygon_api.rotatez(I43,0.78);
P I45=ev.polygon_api.translate(I44,-220,150,-50);
P I46=ev.polygon_api.translate(I45,140,0,0);
PT I47=ev.point_api.point(0,0,150);
PT I48=ev.point_api.point(0,0,151);
P I49=ev.polygon_api.cone(30,I47,I48,0,60);
PT I50=ev.point_api.point(0,0,151);
PT I51=ev.point_api.point(0,0,200);
P I52=ev.polygon_api.cone(30,I50,I51,60,60);
P I53=ev.polygon_api.or_elem(I49,I52);
PT I54=ev.point_api.point(0,0,201);
PT I55=ev.point_api.point(0,0,200);
P I56=ev.polygon_api.cone(30,I54,I55,60,1);
P I57=ev.polygon_api.or_elem(I53,I56);
P I58=ev.polygon_api.rotatey(I57,0.1);
P I59=ev.polygon_api.rotatez(I58,0.78);
P I60=ev.polygon_api.translate(I59,-220,150,-50);
P I61=ev.polygon_api.translate(I60,310,0,0);
P I62=ev.polygon_api.or_elem(I46,I61);
P I63=ev.polygon_api.or_elem(I31,I62);
PT I64=ev.point_api.point(0,0,150);
PT I65=ev.point_api.point(0,0,151);
P I66=ev.polygon_api.cone(30,I64,I65,0,60);
PT I67=ev.point_api.point(0,0,151);
PT I68=ev.point_api.point(0,0,200);
P I69=ev.polygon_api.cone(30,I67,I68,60,60);
P I70=ev.polygon_api.or_elem(I66,I69);
PT I71=ev.point_api.point(0,0,201);
PT I72=ev.point_api.point(0,0,200);
P I73=ev.polygon_api.cone(30,I71,I72,60,1);
P I74=ev.polygon_api.or_elem(I70,I73);
P I75=ev.polygon_api.rotatey(I74,0.1);
P I76=ev.polygon_api.rotatez(I75,0.78);
P I77=ev.polygon_api.translate(I76,-220,150,-50);
PT I78=ev.point_api.point(0,0,150);
PT I79=ev.point_api.point(0,0,151);
P I80=ev.polygon_api.cone(30,I78,I79,0,60);
PT I81=ev.point_api.point(0,0,151);
PT I82=ev.point_api.point(0,0,200);
P I83=ev.polygon_api.cone(30,I81,I82,60,60);
P I84=ev.polygon_api.or_elem(I80,I83);
PT I85=ev.point_api.point(0,0,201);
PT I86=ev.point_api.point(0,0,200);
P I87=ev.polygon_api.cone(30,I85,I86,60,1);
P I88=ev.polygon_api.or_elem(I84,I87);
P I89=ev.polygon_api.rotatey(I88,0.1);
P I90=ev.polygon_api.rotatez(I89,0.78);
P I91=ev.polygon_api.translate(I90,-220,150,-50);
P I92=ev.polygon_api.translate(I91,450,0,0);
P I93=ev.polygon_api.or_elem(I77,I92);
PT I94=ev.point_api.point(0,0,150);
PT I95=ev.point_api.point(0,0,151);
P I96=ev.polygon_api.cone(30,I94,I95,0,60);
PT I97=ev.point_api.point(0,0,151);
PT I98=ev.point_api.point(0,0,200);
P I99=ev.polygon_api.cone(30,I97,I98,60,60);
P I100=ev.polygon_api.or_elem(I96,I99);
PT I101=ev.point_api.point(0,0,201);
PT I102=ev.point_api.point(0,0,200);
P I103=ev.polygon_api.cone(30,I101,I102,60,1);
P I104=ev.polygon_api.or_elem(I100,I103);
P I105=ev.polygon_api.rotatey(I104,0.1);
P I106=ev.polygon_api.rotatez(I105,0.78);
P I107=ev.polygon_api.translate(I106,-220,150,-50);
P I108=ev.polygon_api.translate(I107,140,0,0);
PT I109=ev.point_api.point(0,0,150);
PT I110=ev.point_api.point(0,0,151);
P I111=ev.polygon_api.cone(30,I109,I110,0,60);
PT I112=ev.point_api.point(0,0,151);
PT I113=ev.point_api.point(0,0,200);
P I114=ev.polygon_api.cone(30,I112,I113,60,60);
P I115=ev.polygon_api.or_elem(I111,I114);
PT I116=ev.point_api.point(0,0,201);
PT I117=ev.point_api.point(0,0,200);
P I118=ev.polygon_api.cone(30,I116,I117,60,1);
P I119=ev.polygon_api.or_elem(I115,I118);
P I120=ev.polygon_api.rotatey(I119,0.1);
P I121=ev.polygon_api.rotatez(I120,0.78);
P I122=ev.polygon_api.translate(I121,-220,150,-50);
P I123=ev.polygon_api.translate(I122,310,0,0);
P I124=ev.polygon_api.or_elem(I108,I123);
P I125=ev.polygon_api.or_elem(I93,I124);
P I126=ev.polygon_api.translate(I125,0,0,-250);
P I127=ev.polygon_api.or_elem(I63,I126);
P I128=ev.polygon_api.or_elem(I1,I127);
P I129=ev.polygon_api.recalculate_normals(I128);
P I130=ev.polygon_api.scale(I129,1,-1,1);
P I131=ev.polygon_api.color_from_normals(I130);
P I132=ev.polygon_api.color_grayscale(I131);
PT I133=ev.point_api.point(0,50,0);
PT I134=ev.point_api.point(0,150,0);
P I135=ev.polygon_api.cone(30,I133,I134,50,100);
PT I136=ev.point_api.point(0,150,0);
PT I137=ev.point_api.point(0,200,0);
P I138=ev.polygon_api.cone(30,I136,I137,50,50);
P I139=ev.polygon_api.or_elem(I135,I138);
P I140=ev.polygon_api.translate(I139,220,-290,0);
P I141=ev.polygon_api.recalculate_normals(I140);
P I142=ev.polygon_api.scale(I141,1,-1,1);
P I143=ev.polygon_api.color_from_normals(I142);
P I144=ev.polygon_api.color_grayscale(I143);
P I145=ev.polygon_api.or_elem(I132,I144);
P I146=ev.polygon_api.cube(-300,-30,160,180,-100,100);
P I147=ev.polygon_api.cube(-300,-290,160,260,-100,-90);
P I148=ev.polygon_api.translate(I147,270,0,0);
P I149=ev.polygon_api.cube(-300,-290,160,260,-100,-90);
P I150=ev.polygon_api.translate(I149,0,0,200);
P I151=ev.polygon_api.cube(-300,-290,160,260,-100,-90);
P I152=ev.polygon_api.translate(I151,270,0,200);
P I153=ev.polygon_api.cube(-300,-290,160,260,-100,-90);
P I154=ev.polygon_api.or_array2(std::vector<P>{I146,I148,I150,I152,I153});
P I155=ev.polygon_api.scale(I154,1,-1,1);
P I156=ev.polygon_api.translate(I155,0,350,0);
P I157=ev.polygon_api.color_faces(I156,0xffffffff,0xffffffff,0xff888888,0xff888888);
P I158=ev.polygon_api.or_elem(I145,I157);
MT I159=ev.materials_api.def(ev);
MT I160=ev.materials_api.snow(ev,I159);
ML I161=ev.materials_api.bind(I158,I160);
ML I162=ev.polygon_api.light_shader(ev,I161);
 return I162;
}

ML happy_xmas_text(EveryApi &ev)
{
BM I1=ev.bitmap_api.newbitmap(250,50,00000000);
Ft I2=ev.font_api.newfont("FreeSans.ttf",20,20);
BM I3=ev.font_api.font_string(I2,"Merry",5);
BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
Ft I5=ev.font_api.newfont("FreeSans.ttf",20,20);
BM I6=ev.font_api.font_string(I5,"Christmas",5);
BM I7=ev.bitmap_api.blitbitmap(I4,I6,100,2);
BM I8=ev.bitmap_api.persistent_cache(I7,"cache.ppm");
P I9=ev.polygon_api.color_map(I8,250,50,0);
P I10=ev.polygon_api.scale(I9,1,-1,1);
P I11=ev.polygon_api.translate(I10,-125,0,0);
P I12=ev.polygon_api.scale(I11,3,3,3);
VA I13=ev.polygon_api.create_vertex_array(I12,true);
ML I14=ev.polygon_api.render_vertex_array_ml(ev,I13);
MN I15=ev.move_api.empty();
MN I16=ev.move_api.trans2(I15,0,0,300);
 float time_delta = 500.0;
MN I17=ev.move_api.translate(I16,time_delta+0,time_delta+30,0,0,-300);
MN I18=ev.move_api.translate(I17,time_delta+100,time_delta+130,0,1000,0);
 MN I18a = ev.move_api.scale2(I18, -1.0,1.0,-1.0);
ML I19=ev.move_api.move_ml(ev,I14,I18a);
 ML I19a = ev.move_api.enable_ml(ev,I19, time_delta, time_delta+130.0);

#if 0
BM I1=ev.bitmap_api.newbitmap(250,50,00000000);
Ft I2=ev.font_api.newfont("FreeSans.ttf",20,20);
BM I3=ev.font_api.font_string(I2,"Happy",5);
BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
Ft I5=ev.font_api.newfont("FreeSans.ttf",20,20);
BM I6=ev.font_api.font_string(I5,"Christmas",5);
BM I7=ev.bitmap_api.blitbitmap(I4,I6,100,2);
P I8=ev.polygon_api.color_map(I7,250,50,0);
P I9=ev.polygon_api.scale(I8,1,-1,1);
P I10=ev.polygon_api.translate(I9,-125,0,0);
P I11=ev.polygon_api.scale(I10,3,3,3);
VA I12=ev.polygon_api.create_vertex_array(I11,true);
ML I13=ev.polygon_api.render_vertex_array_ml(ev,I12);
MN I14=ev.move_api.empty();
MN I15=ev.move_api.trans2(I14,0,0,300);
 float time_delta = 500.0;
MN I16=ev.move_api.translate(I15,time_delta+0,time_delta+30,0,0,-300);
MN I17=ev.move_api.translate(I16,time_delta+100,time_delta+130,0,1000,0);
 MN I17a = ev.move_api.scale2(I17, -1.0,1.0,-1.0);
ML I18=ev.move_api.move_ml(ev,I13,I17a);
 ML I18a = ev.move_api.enable_ml(ev,I18, time_delta, time_delta+130.0);
#endif
 return I19a;
}

ML reki(EveryApi &ev)
{
P I1=ev.polygon_api.cube(-300,600,300,310,0,10);
P I2=ev.polygon_api.cube(600,800,300,310,0,10);
P I3=ev.polygon_api.translate(I2,-600,0,0);
P I4=ev.polygon_api.rotatez(I3,-0.3);
P I5=ev.polygon_api.translate(I4,500,10,0);
P I6=ev.polygon_api.or_array2(std::vector<P>{I1,I5});
P I7=ev.polygon_api.scale(I6,1,-1,1);
P I8=ev.polygon_api.cube(-300,600,300,310,0,10);
P I9=ev.polygon_api.cube(600,800,300,310,0,10);
P I10=ev.polygon_api.translate(I9,-600,0,0);
P I11=ev.polygon_api.rotatez(I10,-0.3);
P I12=ev.polygon_api.translate(I11,500,10,0);
P I13=ev.polygon_api.or_array2(std::vector<P>{I8,I12});
P I14=ev.polygon_api.scale(I13,1,-1,1);
P I15=ev.polygon_api.translate(I14,0,0,-300);
P I16=ev.polygon_api.or_array2(std::vector<P>{I7,I15});
P I17=ev.polygon_api.rounded_cube(ev,-300,300,100,200,-270,0,20);
P I18=ev.polygon_api.scale(I17,1,-1,1);
P I19=ev.polygon_api.or_elem(I16,I18);
P I20=ev.polygon_api.recalculate_normals(I19);
P I21=ev.polygon_api.color_from_normals(I20);
P I22=ev.polygon_api.cube(-300,600,300,310,0,10);
P I23=ev.polygon_api.cube(600,800,300,310,0,10);
P I24=ev.polygon_api.translate(I23,-600,0,0);
P I25=ev.polygon_api.rotatez(I24,-0.3);
P I26=ev.polygon_api.translate(I25,500,10,0);
P I27=ev.polygon_api.or_array2(std::vector<P>{I22,I26});
P I28=ev.polygon_api.scale(I27,1,-1,1);
P I29=ev.polygon_api.cube(-300,600,300,310,0,10);
P I30=ev.polygon_api.cube(600,800,300,310,0,10);
P I31=ev.polygon_api.translate(I30,-600,0,0);
P I32=ev.polygon_api.rotatez(I31,-0.3);
P I33=ev.polygon_api.translate(I32,500,10,0);
P I34=ev.polygon_api.or_array2(std::vector<P>{I29,I33});
P I35=ev.polygon_api.scale(I34,1,-1,1);
P I36=ev.polygon_api.translate(I35,0,0,-300);
P I37=ev.polygon_api.or_array2(std::vector<P>{I28,I36});
P I38=ev.polygon_api.rounded_cube(ev,-300,300,100,200,-270,0,20);
P I39=ev.polygon_api.scale(I38,1,-1,1);
P I40=ev.polygon_api.or_elem(I37,I39);
P I41=ev.polygon_api.recalculate_normals(I40);
P I42=ev.polygon_api.color_from_normals(I41);
P I43=ev.polygon_api.color_grayscale(I42);
P I44=ev.polygon_api.mix_color(I21,I43,0.5);
VA I45=ev.polygon_api.create_vertex_array(I44,true);
ML I46=ev.polygon_api.render_vertex_array_ml(ev,I45);
ML I47=ev.polygon_api.shading_shader(ev,I46,0xffffffff,0xffeeeeee,0xffffffff);
ML I48=ev.polygon_api.light_shader(ev,I47);
ML I49=ev.polygon_api.light_shader(ev,I48);
 return I49;
}
P tree(EveryApi &ev)
{
MN I1=ev.move_api.empty();
MN I2=ev.move_api.scale2(I1,0.5,0.5,0.5);
MN I3=ev.move_api.trans2(I2,0,150,0);
MN I4=ev.move_api.empty();
MN I5=ev.move_api.scale2(I4,0.5,0.5,0.5);
MN I6=ev.move_api.trans2(I5,0,150,0);
MN I7=ev.move_api.rotatez(I6,0.8);
MN I8=ev.move_api.empty();
MN I9=ev.move_api.scale2(I8,0.5,0.5,0.5);
MN I10=ev.move_api.trans2(I9,0,150,0);
MN I11=ev.move_api.rotatez(I10,-0.8);
MN I12=ev.move_api.empty();
MN I13=ev.move_api.scale2(I12,0.5,0.5,0.5);
MN I14=ev.move_api.trans2(I13,0,150,0);
MN I15=ev.move_api.rotatez(I14,0.8);
MN I16=ev.move_api.rotatey(I15,1.57);
MN I17=ev.move_api.empty();
MN I18=ev.move_api.scale2(I17,0.5,0.5,0.5);
MN I19=ev.move_api.trans2(I18,0,150,0);
MN I20=ev.move_api.rotatez(I19,-0.8);
MN I21=ev.move_api.rotatey(I20,-1.57);
MN I22=ev.move_api.empty();
MN I23=ev.move_api.scale2(I22,0.5,0.5,0.5);
MN I24=ev.move_api.trans2(I23,0,150,0);
MN I25=ev.move_api.rotatez(I24,0.8);
MN I26=ev.move_api.rotatey(I25,1.57);
MN I27=ev.move_api.scale2(I26,-1,1,-1);
MN I28=ev.move_api.empty();
MN I29=ev.move_api.scale2(I28,0.5,0.5,0.5);
MN I30=ev.move_api.trans2(I29,0,150,0);
MN I31=ev.move_api.rotatez(I30,0.8);
MN I32=ev.move_api.trans2(I31,0,-200,0);
MN I33=ev.move_api.empty();
MN I34=ev.move_api.scale2(I33,0.5,0.5,0.5);
MN I35=ev.move_api.trans2(I34,0,150,0);
MN I36=ev.move_api.rotatez(I35,-0.8);
MN I37=ev.move_api.trans2(I36,0,-200,0);
MN I38=ev.move_api.empty();
MN I39=ev.move_api.scale2(I38,0.5,0.5,0.5);
MN I40=ev.move_api.trans2(I39,0,150,0);
MN I41=ev.move_api.rotatez(I40,0.8);
MN I42=ev.move_api.rotatey(I41,1.57);
MN I43=ev.move_api.trans2(I42,0,-200,0);
MN I44=ev.move_api.empty();
MN I45=ev.move_api.scale2(I44,0.5,0.5,0.5);
MN I46=ev.move_api.trans2(I45,0,150,0);
MN I47=ev.move_api.rotatez(I46,-0.8);
MN I48=ev.move_api.rotatey(I47,-1.57);
MN I49=ev.move_api.trans2(I48,0,-200,0);
MN I50=ev.move_api.empty();
MN I51=ev.move_api.scale2(I50,0.5,0.5,0.5);
MN I52=ev.move_api.trans2(I51,0,150,0);
MN I53=ev.move_api.rotatez(I52,0.8);
MN I54=ev.move_api.rotatey(I53,1.57);
MN I55=ev.move_api.scale2(I54,-1,1,-1);
MN I56=ev.move_api.trans2(I55,0,-200,0);
TL I57=ev.tree_api.level(std::vector<MN>{I3,I7,I11,I16,I21,I27,I32,I37,I43,I49,I56});
MN I58=ev.move_api.empty();
MN I59=ev.move_api.scale2(I58,0.5,0.5,0.5);
MN I60=ev.move_api.trans2(I59,0,150,0);
MN I61=ev.move_api.empty();
MN I62=ev.move_api.scale2(I61,0.5,0.5,0.5);
MN I63=ev.move_api.trans2(I62,0,150,0);
MN I64=ev.move_api.rotatez(I63,0.8);
MN I65=ev.move_api.empty();
MN I66=ev.move_api.scale2(I65,0.5,0.5,0.5);
MN I67=ev.move_api.trans2(I66,0,150,0);
MN I68=ev.move_api.rotatez(I67,-0.8);
MN I69=ev.move_api.empty();
MN I70=ev.move_api.scale2(I69,0.5,0.5,0.5);
MN I71=ev.move_api.trans2(I70,0,150,0);
MN I72=ev.move_api.rotatez(I71,0.8);
MN I73=ev.move_api.rotatey(I72,1.57);
MN I74=ev.move_api.empty();
MN I75=ev.move_api.scale2(I74,0.5,0.5,0.5);
MN I76=ev.move_api.trans2(I75,0,150,0);
MN I77=ev.move_api.rotatez(I76,-0.8);
MN I78=ev.move_api.rotatey(I77,-1.57);
MN I79=ev.move_api.empty();
MN I80=ev.move_api.scale2(I79,0.5,0.5,0.5);
MN I81=ev.move_api.trans2(I80,0,150,0);
MN I82=ev.move_api.rotatez(I81,0.8);
MN I83=ev.move_api.rotatey(I82,1.57);
MN I84=ev.move_api.scale2(I83,-1,1,-1);
MN I85=ev.move_api.empty();
MN I86=ev.move_api.scale2(I85,0.5,0.5,0.5);
MN I87=ev.move_api.trans2(I86,0,150,0);
MN I88=ev.move_api.rotatez(I87,0.8);
MN I89=ev.move_api.trans2(I88,0,-200,0);
MN I90=ev.move_api.empty();
MN I91=ev.move_api.scale2(I90,0.5,0.5,0.5);
MN I92=ev.move_api.trans2(I91,0,150,0);
MN I93=ev.move_api.rotatez(I92,-0.8);
MN I94=ev.move_api.trans2(I93,0,-200,0);
MN I95=ev.move_api.empty();
MN I96=ev.move_api.scale2(I95,0.5,0.5,0.5);
MN I97=ev.move_api.trans2(I96,0,150,0);
MN I98=ev.move_api.rotatez(I97,0.8);
MN I99=ev.move_api.rotatey(I98,1.57);
MN I100=ev.move_api.trans2(I99,0,-200,0);
MN I101=ev.move_api.empty();
MN I102=ev.move_api.scale2(I101,0.5,0.5,0.5);
MN I103=ev.move_api.trans2(I102,0,150,0);
MN I104=ev.move_api.rotatez(I103,-0.8);
MN I105=ev.move_api.rotatey(I104,-1.57);
MN I106=ev.move_api.trans2(I105,0,-200,0);
MN I107=ev.move_api.empty();
MN I108=ev.move_api.scale2(I107,0.5,0.5,0.5);
MN I109=ev.move_api.trans2(I108,0,150,0);
MN I110=ev.move_api.rotatez(I109,0.8);
MN I111=ev.move_api.rotatey(I110,1.57);
MN I112=ev.move_api.scale2(I111,-1,1,-1);
MN I113=ev.move_api.trans2(I112,0,-200,0);
TL I114=ev.tree_api.level(std::vector<MN>{I60,I64,I68,I73,I78,I84,I89,I94,I100,I106,I113});
MN I115=ev.move_api.empty();
MN I116=ev.move_api.scale2(I115,0.5,0.5,0.5);
MN I117=ev.move_api.trans2(I116,0,150,0);
MN I118=ev.move_api.empty();
MN I119=ev.move_api.scale2(I118,0.5,0.5,0.5);
MN I120=ev.move_api.trans2(I119,0,150,0);
MN I121=ev.move_api.rotatez(I120,0.8);
MN I122=ev.move_api.empty();
MN I123=ev.move_api.scale2(I122,0.5,0.5,0.5);
MN I124=ev.move_api.trans2(I123,0,150,0);
MN I125=ev.move_api.rotatez(I124,-0.8);
MN I126=ev.move_api.empty();
MN I127=ev.move_api.scale2(I126,0.5,0.5,0.5);
MN I128=ev.move_api.trans2(I127,0,150,0);
MN I129=ev.move_api.rotatez(I128,0.8);
MN I130=ev.move_api.rotatey(I129,1.57);
MN I131=ev.move_api.empty();
MN I132=ev.move_api.scale2(I131,0.5,0.5,0.5);
MN I133=ev.move_api.trans2(I132,0,150,0);
MN I134=ev.move_api.rotatez(I133,-0.8);
MN I135=ev.move_api.rotatey(I134,-1.57);
MN I136=ev.move_api.empty();
MN I137=ev.move_api.scale2(I136,0.5,0.5,0.5);
MN I138=ev.move_api.trans2(I137,0,150,0);
MN I139=ev.move_api.rotatez(I138,0.8);
MN I140=ev.move_api.rotatey(I139,1.57);
MN I141=ev.move_api.scale2(I140,-1,1,-1);
MN I142=ev.move_api.empty();
MN I143=ev.move_api.scale2(I142,0.5,0.5,0.5);
MN I144=ev.move_api.trans2(I143,0,150,0);
MN I145=ev.move_api.rotatez(I144,0.8);
MN I146=ev.move_api.trans2(I145,0,-200,0);
MN I147=ev.move_api.empty();
MN I148=ev.move_api.scale2(I147,0.5,0.5,0.5);
MN I149=ev.move_api.trans2(I148,0,150,0);
MN I150=ev.move_api.rotatez(I149,-0.8);
MN I151=ev.move_api.trans2(I150,0,-200,0);
MN I152=ev.move_api.empty();
MN I153=ev.move_api.scale2(I152,0.5,0.5,0.5);
MN I154=ev.move_api.trans2(I153,0,150,0);
MN I155=ev.move_api.rotatez(I154,0.8);
MN I156=ev.move_api.rotatey(I155,1.57);
MN I157=ev.move_api.trans2(I156,0,-200,0);
MN I158=ev.move_api.empty();
MN I159=ev.move_api.scale2(I158,0.5,0.5,0.5);
MN I160=ev.move_api.trans2(I159,0,150,0);
MN I161=ev.move_api.rotatez(I160,-0.8);
MN I162=ev.move_api.rotatey(I161,-1.57);
MN I163=ev.move_api.trans2(I162,0,-200,0);
MN I164=ev.move_api.empty();
MN I165=ev.move_api.scale2(I164,0.5,0.5,0.5);
MN I166=ev.move_api.trans2(I165,0,150,0);
MN I167=ev.move_api.rotatez(I166,0.8);
MN I168=ev.move_api.rotatey(I167,1.57);
MN I169=ev.move_api.scale2(I168,-1,1,-1);
MN I170=ev.move_api.trans2(I169,0,-200,0);
TL I171=ev.tree_api.level(std::vector<MN>{I117,I121,I125,I130,I135,I141,I146,I151,I157,I163,I170});
MN I172=ev.move_api.empty();
MN I173=ev.move_api.scale2(I172,0.5,0.5,0.5);
MN I174=ev.move_api.trans2(I173,0,150,0);
MN I175=ev.move_api.empty();
MN I176=ev.move_api.scale2(I175,0.5,0.5,0.5);
MN I177=ev.move_api.trans2(I176,0,150,0);
MN I178=ev.move_api.rotatez(I177,0.8);
MN I179=ev.move_api.empty();
MN I180=ev.move_api.scale2(I179,0.5,0.5,0.5);
MN I181=ev.move_api.trans2(I180,0,150,0);
MN I182=ev.move_api.rotatez(I181,-0.8);
MN I183=ev.move_api.empty();
MN I184=ev.move_api.scale2(I183,0.5,0.5,0.5);
MN I185=ev.move_api.trans2(I184,0,150,0);
MN I186=ev.move_api.rotatez(I185,0.8);
MN I187=ev.move_api.rotatey(I186,1.57);
MN I188=ev.move_api.empty();
MN I189=ev.move_api.scale2(I188,0.5,0.5,0.5);
MN I190=ev.move_api.trans2(I189,0,150,0);
MN I191=ev.move_api.rotatez(I190,-0.8);
MN I192=ev.move_api.rotatey(I191,-1.57);
MN I193=ev.move_api.empty();
MN I194=ev.move_api.scale2(I193,0.5,0.5,0.5);
MN I195=ev.move_api.trans2(I194,0,150,0);
MN I196=ev.move_api.rotatez(I195,0.8);
MN I197=ev.move_api.rotatey(I196,1.57);
MN I198=ev.move_api.scale2(I197,-1,1,-1);
MN I199=ev.move_api.empty();
MN I200=ev.move_api.scale2(I199,0.5,0.5,0.5);
MN I201=ev.move_api.trans2(I200,0,150,0);
MN I202=ev.move_api.rotatez(I201,0.8);
MN I203=ev.move_api.trans2(I202,0,-200,0);
MN I204=ev.move_api.empty();
MN I205=ev.move_api.scale2(I204,0.5,0.5,0.5);
MN I206=ev.move_api.trans2(I205,0,150,0);
MN I207=ev.move_api.rotatez(I206,-0.8);
MN I208=ev.move_api.trans2(I207,0,-200,0);
MN I209=ev.move_api.empty();
MN I210=ev.move_api.scale2(I209,0.5,0.5,0.5);
MN I211=ev.move_api.trans2(I210,0,150,0);
MN I212=ev.move_api.rotatez(I211,0.8);
MN I213=ev.move_api.rotatey(I212,1.57);
MN I214=ev.move_api.trans2(I213,0,-200,0);
MN I215=ev.move_api.empty();
MN I216=ev.move_api.scale2(I215,0.5,0.5,0.5);
MN I217=ev.move_api.trans2(I216,0,150,0);
MN I218=ev.move_api.rotatez(I217,-0.8);
MN I219=ev.move_api.rotatey(I218,-1.57);
MN I220=ev.move_api.trans2(I219,0,-200,0);
MN I221=ev.move_api.empty();
MN I222=ev.move_api.scale2(I221,0.5,0.5,0.5);
MN I223=ev.move_api.trans2(I222,0,150,0);
MN I224=ev.move_api.rotatez(I223,0.8);
MN I225=ev.move_api.rotatey(I224,1.57);
MN I226=ev.move_api.scale2(I225,-1,1,-1);
MN I227=ev.move_api.trans2(I226,0,-200,0);
TL I228=ev.tree_api.level(std::vector<MN>{I174,I178,I182,I187,I192,I198,I203,I208,I214,I220,I227});
MN I229=ev.move_api.empty();
MN I230=ev.move_api.scale2(I229,0.5,0.5,0.5);
MN I231=ev.move_api.trans2(I230,0,150,0);
MN I232=ev.move_api.empty();
MN I233=ev.move_api.scale2(I232,0.5,0.5,0.5);
MN I234=ev.move_api.trans2(I233,0,150,0);
MN I235=ev.move_api.rotatez(I234,0.8);
MN I236=ev.move_api.empty();
MN I237=ev.move_api.scale2(I236,0.5,0.5,0.5);
MN I238=ev.move_api.trans2(I237,0,150,0);
MN I239=ev.move_api.rotatez(I238,-0.8);
MN I240=ev.move_api.empty();
MN I241=ev.move_api.scale2(I240,0.5,0.5,0.5);
MN I242=ev.move_api.trans2(I241,0,150,0);
MN I243=ev.move_api.rotatez(I242,0.8);
MN I244=ev.move_api.rotatey(I243,1.57);
MN I245=ev.move_api.empty();
MN I246=ev.move_api.scale2(I245,0.5,0.5,0.5);
MN I247=ev.move_api.trans2(I246,0,150,0);
MN I248=ev.move_api.rotatez(I247,-0.8);
MN I249=ev.move_api.rotatey(I248,-1.57);
MN I250=ev.move_api.empty();
MN I251=ev.move_api.scale2(I250,0.5,0.5,0.5);
MN I252=ev.move_api.trans2(I251,0,150,0);
MN I253=ev.move_api.rotatez(I252,0.8);
MN I254=ev.move_api.rotatey(I253,1.57);
MN I255=ev.move_api.scale2(I254,-1,1,-1);
MN I256=ev.move_api.empty();
MN I257=ev.move_api.scale2(I256,0.5,0.5,0.5);
MN I258=ev.move_api.trans2(I257,0,150,0);
MN I259=ev.move_api.rotatez(I258,0.8);
MN I260=ev.move_api.trans2(I259,0,-200,0);
MN I261=ev.move_api.empty();
MN I262=ev.move_api.scale2(I261,0.5,0.5,0.5);
MN I263=ev.move_api.trans2(I262,0,150,0);
MN I264=ev.move_api.rotatez(I263,-0.8);
MN I265=ev.move_api.trans2(I264,0,-200,0);
MN I266=ev.move_api.empty();
MN I267=ev.move_api.scale2(I266,0.5,0.5,0.5);
MN I268=ev.move_api.trans2(I267,0,150,0);
MN I269=ev.move_api.rotatez(I268,0.8);
MN I270=ev.move_api.rotatey(I269,1.57);
MN I271=ev.move_api.trans2(I270,0,-200,0);
MN I272=ev.move_api.empty();
MN I273=ev.move_api.scale2(I272,0.5,0.5,0.5);
MN I274=ev.move_api.trans2(I273,0,150,0);
MN I275=ev.move_api.rotatez(I274,-0.8);
MN I276=ev.move_api.rotatey(I275,-1.57);
MN I277=ev.move_api.trans2(I276,0,-200,0);
MN I278=ev.move_api.empty();
MN I279=ev.move_api.scale2(I278,0.5,0.5,0.5);
MN I280=ev.move_api.trans2(I279,0,150,0);
MN I281=ev.move_api.rotatez(I280,0.8);
MN I282=ev.move_api.rotatey(I281,1.57);
MN I283=ev.move_api.scale2(I282,-1,1,-1);
MN I284=ev.move_api.trans2(I283,0,-200,0);
TL I285=ev.tree_api.level(std::vector<MN>{I231,I235,I239,I244,I249,I255,I260,I265,I271,I277,I284});
 T I286=ev.tree_api.tree(std::vector<TL>{I57,I114,I171 ,I228 /*,I285*/});
PT I287=ev.point_api.point(0,0,0);
PT I288=ev.point_api.point(0,300,0);
P I289=ev.polygon_api.cone(10,I287,I288,50,50);
P I290=ev.polygon_api.translate(I289,0,-300,0);
P I291=ev.polygon_api.recalculate_normals(I290);
P I292=ev.polygon_api.color_from_normals(I291);
P I293=ev.polygon_api.color_grayscale(I292);
PT I294=ev.point_api.point(0,0,0);
PT I295=ev.point_api.point(0,300,0);
P I296=ev.polygon_api.cone(10,I294,I295,50,50);
P I297=ev.polygon_api.translate(I296,0,-300,0);
P I298=ev.polygon_api.recalculate_normals(I297);
P I299=ev.polygon_api.color_from_normals(I298);
P I300=ev.polygon_api.color_grayscale(I299);
PT I301=ev.point_api.point(0,0,0);
PT I302=ev.point_api.point(0,300,0);
P I303=ev.polygon_api.cone(10,I301,I302,50,50);
P I304=ev.polygon_api.translate(I303,0,-300,0);
P I305=ev.polygon_api.recalculate_normals(I304);
P I306=ev.polygon_api.color_from_normals(I305);
P I307=ev.polygon_api.color_grayscale(I306);
PT I308=ev.point_api.point(0,0,0);
PT I309=ev.point_api.point(0,300,0);
P I310=ev.polygon_api.cone(10,I308,I309,50,50);
P I311=ev.polygon_api.translate(I310,0,-300,0);
P I312=ev.polygon_api.recalculate_normals(I311);
P I313=ev.polygon_api.color_from_normals(I312);
P I314=ev.polygon_api.color_grayscale(I313);
PT I315=ev.point_api.point(0,0,0);
PT I316=ev.point_api.point(0,300,0);
P I317=ev.polygon_api.cone(10,I315,I316,50,50);
P I318=ev.polygon_api.translate(I317,0,-300,0);
P I319=ev.polygon_api.recalculate_normals(I318);
P I320=ev.polygon_api.color_from_normals(I319);
P I321=ev.polygon_api.color_grayscale(I320);
P I322=ev.tree_api.tree_p(ev,I286,std::vector<P>{I293,I300,I307,I314,I321},0.0);
 return I322;
}

P snowman(EveryApi &ev)
{
PT I1=ev.point_api.point(0,0,0);
BO I2=ev.bool_api.sphere(ev,I1,100.0,30,30);
PT I3=ev.point_api.point(0,-130,0);
BO I4=ev.bool_api.sphere(ev,I3,130,30,30);
BO I5=ev.bool_api.or_elem(ev,I2,I4);
PT I6=ev.point_api.point(0,-300,0);
BO I7=ev.bool_api.sphere(ev,I6,150,30,30);
BO I8=ev.bool_api.or_elem(ev,I5,I7);
P I9=ev.bool_api.to_polygon(I8);
P I10=ev.polygon_api.recalculate_normals(I9);
P I11=ev.polygon_api.color_from_normals(I10);
PT I12=ev.point_api.point(0,0,0);
BO I13=ev.bool_api.sphere(ev,I12,100.0,30,30);
PT I14=ev.point_api.point(0,-130,0);
BO I15=ev.bool_api.sphere(ev,I14,130,30,30);
BO I16=ev.bool_api.or_elem(ev,I13,I15);
PT I17=ev.point_api.point(0,-300,0);
BO I18=ev.bool_api.sphere(ev,I17,150,30,30);
BO I19=ev.bool_api.or_elem(ev,I16,I18);
P I20=ev.bool_api.to_polygon(I19);
P I21=ev.polygon_api.recalculate_normals(I20);
P I22=ev.polygon_api.color_from_normals(I21);
P I23=ev.polygon_api.color_grayscale(I22);
P I24=ev.polygon_api.mix_color(I11,I23,0.5);
P I25=ev.polygon_api.scale(I24,1,0.8,1);
//VA I26=ev.polygon_api.create_vertex_array(I25,true);
//ML I27=ev.polygon_api.render_vertex_array_ml(ev,I26);
//ML I28=ev.polygon_api.shading_shader(ev,I27,0xffaaaaaa,0xffeeeeee,0xffffffff);
 return I25;
}

ML gift(EveryApi &ev)
{
  P I1=ev.polygon_api.cube(-150,150,-100,100,-100,100);
  P I2=ev.polygon_api.scale(I1,1.5,1.5,1.5);
  P I3=ev.polygon_api.translate(I2,0,-200,0);
  BB I4=ev.bool_bitmap_api.empty(100,100);
  BB I5=ev.bool_bitmap_api.rectangle(I4,40,0,20,100);
  BB I6=ev.bool_bitmap_api.rectangle(I5,0,40,100,20);
  BM I7=ev.bool_bitmap_api.to_bitmap(I6,0,0,255,255,255,255,255,255);
  MT I8=ev.materials_api.texture(ev,I7);
  MT I9=ev.materials_api.web(ev,I8);
  ML I10=ev.materials_api.bind(I3,I9);
  return I10;
}

ML santa(EveryApi &ev)
{
PT I1=ev.point_api.point(-30,300,0);
PT I2=ev.point_api.point(-30,220,0);
P I3=ev.polygon_api.cone(30,I1,I2,150,150);
PT I4=ev.point_api.point(-30,220,0);
PT I5=ev.point_api.point(-30,100,0);
P I6=ev.polygon_api.cone(30,I4,I5,160,150);
P I7=ev.polygon_api.color(I6,0xffff0000);
PT I8=ev.point_api.point(-30,100,0);
PT I9=ev.point_api.point(-30,0,0);
P I10=ev.polygon_api.cone(30,I8,I9,160,160);
P I11=ev.polygon_api.color(I10,0xffff0000);
PT I12=ev.point_api.point(-30,30,0);
PT I13=ev.point_api.point(-50,-100,0);
P I14=ev.polygon_api.cone(30,I12,I13,100,160);
P I15=ev.polygon_api.color(I14,0xffff0000);
PT I16=ev.point_api.point(-50,-100,0);
PT I17=ev.point_api.point(-50,-200,0);
P I18=ev.polygon_api.cone(30,I16,I17,100,100);
P I19=ev.polygon_api.color(I18,0xffff8844);
PT I20=ev.point_api.point(-50,-180,0);
PT I21=ev.point_api.point(-70,-200,0);
P I22=ev.polygon_api.cone(30,I20,I21,110,110);
P I23=ev.polygon_api.color(I22,0xffffffff);
PT I24=ev.point_api.point(-70,-200,0);
PT I25=ev.point_api.point(-105,-202,0);
P I26=ev.polygon_api.cone(30,I24,I25,90,110);
PT I27=ev.point_api.point(-105,-202,0);
PT I28=ev.point_api.point(-150,-200,0);
P I29=ev.polygon_api.cone(30,I27,I28,80,90);
PT I30=ev.point_api.point(-150,-200,0);
PT I31=ev.point_api.point(-190,-180,0);
P I32=ev.polygon_api.cone(30,I30,I31,60,80);
PT I33=ev.point_api.point(-190,-180,0);
PT I34=ev.point_api.point(-200,-120,0);
P I35=ev.polygon_api.cone(30,I33,I34,30,60);
PT I36=ev.point_api.point(-200,-120,0);
PT I37=ev.point_api.point(-200,-100,0);
P I38=ev.polygon_api.cone(30,I36,I37,10,30);
P I39=ev.polygon_api.or_array2(std::vector<P>{I26,I29,I32,I35,I38});
P I40=ev.polygon_api.color(I39,0xffff0000);
PT I41=ev.point_api.point(-200,-100,0);
P I42=ev.polygon_api.sphere(I41,30,30,30);
P I43=ev.polygon_api.color(I42,0xffffffff);
PT I44=ev.point_api.point(90,110,140);
PT I45=ev.point_api.point(50,110,140);
P I46=ev.polygon_api.cone(30,I44,I45,50,50);
PT I47=ev.point_api.point(-20,100,140);
PT I48=ev.point_api.point(50,110,140);
P I49=ev.polygon_api.cone(30,I47,I48,50,50);
PT I50=ev.point_api.point(-20,100,140);
PT I51=ev.point_api.point(-95,80,140);
P I52=ev.polygon_api.cone(30,I50,I51,50,50);
PT I53=ev.point_api.point(-95,80,140);
PT I54=ev.point_api.point(-105,20,140);
P I55=ev.polygon_api.cone(30,I53,I54,40,40);
P I56=ev.polygon_api.or_array2(std::vector<P>{I49,I52,I55});
P I57=ev.polygon_api.color(I56,0xffff0000);
P I58=ev.polygon_api.or_elem(I46,I57);
PT I59=ev.point_api.point(150,-170,-140);
PT I60=ev.point_api.point(110,-150,-140);
P I61=ev.polygon_api.cone(30,I59,I60,60,60);
PT I62=ev.point_api.point(110,-150,-140);
PT I63=ev.point_api.point(0,10,-140);
P I64=ev.polygon_api.cone(30,I62,I63,60,60);
P I65=ev.polygon_api.color(I64,0xffff0000);
P I66=ev.polygon_api.or_elem(I61,I65);
P I67=ev.polygon_api.or_array2(std::vector<P>{I3,I7,I11,I15,I19,I23,I40,I43,I58,I66});
P I68=ev.polygon_api.scale(I67,2,-1,1.5);
P I69=ev.polygon_api.recalculate_normals(I68);
P I70=ev.polygon_api.color_from_normals(I69);
P I71=ev.polygon_api.color_grayscale(I70);
PT I72=ev.point_api.point(-30,300,0);
PT I73=ev.point_api.point(-30,220,0);
P I74=ev.polygon_api.cone(30,I72,I73,150,150);
PT I75=ev.point_api.point(-30,220,0);
PT I76=ev.point_api.point(-30,100,0);
P I77=ev.polygon_api.cone(30,I75,I76,160,150);
P I78=ev.polygon_api.color(I77,0xffff0000);
PT I79=ev.point_api.point(-30,100,0);
PT I80=ev.point_api.point(-30,0,0);
P I81=ev.polygon_api.cone(30,I79,I80,160,160);
P I82=ev.polygon_api.color(I81,0xffff0000);
PT I83=ev.point_api.point(-30,30,0);
PT I84=ev.point_api.point(-50,-100,0);
P I85=ev.polygon_api.cone(30,I83,I84,100,160);
P I86=ev.polygon_api.color(I85,0xffff0000);
PT I87=ev.point_api.point(-50,-100,0);
PT I88=ev.point_api.point(-50,-200,0);
P I89=ev.polygon_api.cone(30,I87,I88,100,100);
P I90=ev.polygon_api.color(I89,0xffff8844);
PT I91=ev.point_api.point(-50,-180,0);
PT I92=ev.point_api.point(-70,-200,0);
P I93=ev.polygon_api.cone(30,I91,I92,110,110);
P I94=ev.polygon_api.color(I93,0xffffffff);
PT I95=ev.point_api.point(-70,-200,0);
PT I96=ev.point_api.point(-105,-202,0);
P I97=ev.polygon_api.cone(30,I95,I96,90,110);
PT I98=ev.point_api.point(-105,-202,0);
PT I99=ev.point_api.point(-150,-200,0);
P I100=ev.polygon_api.cone(30,I98,I99,80,90);
PT I101=ev.point_api.point(-150,-200,0);
PT I102=ev.point_api.point(-190,-180,0);
P I103=ev.polygon_api.cone(30,I101,I102,60,80);
PT I104=ev.point_api.point(-190,-180,0);
PT I105=ev.point_api.point(-200,-120,0);
P I106=ev.polygon_api.cone(30,I104,I105,30,60);
PT I107=ev.point_api.point(-200,-120,0);
PT I108=ev.point_api.point(-200,-100,0);
P I109=ev.polygon_api.cone(30,I107,I108,10,30);
P I110=ev.polygon_api.or_array2(std::vector<P>{I97,I100,I103,I106,I109});
P I111=ev.polygon_api.color(I110,0xffff0000);
PT I112=ev.point_api.point(-200,-100,0);
P I113=ev.polygon_api.sphere(I112,30,30,30);
P I114=ev.polygon_api.color(I113,0xffffffff);
PT I115=ev.point_api.point(90,110,140);
PT I116=ev.point_api.point(50,110,140);
P I117=ev.polygon_api.cone(30,I115,I116,50,50);
PT I118=ev.point_api.point(-20,100,140);
PT I119=ev.point_api.point(50,110,140);
P I120=ev.polygon_api.cone(30,I118,I119,50,50);
PT I121=ev.point_api.point(-20,100,140);
PT I122=ev.point_api.point(-95,80,140);
P I123=ev.polygon_api.cone(30,I121,I122,50,50);
PT I124=ev.point_api.point(-95,80,140);
PT I125=ev.point_api.point(-105,20,140);
P I126=ev.polygon_api.cone(30,I124,I125,40,40);
P I127=ev.polygon_api.or_array2(std::vector<P>{I120,I123,I126});
P I128=ev.polygon_api.color(I127,0xffff0000);
P I129=ev.polygon_api.or_elem(I117,I128);
PT I130=ev.point_api.point(150,-170,-140);
PT I131=ev.point_api.point(110,-150,-140);
P I132=ev.polygon_api.cone(30,I130,I131,60,60);
PT I133=ev.point_api.point(110,-150,-140);
PT I134=ev.point_api.point(0,10,-140);
P I135=ev.polygon_api.cone(30,I133,I134,60,60);
P I136=ev.polygon_api.color(I135,0xffff0000);
P I137=ev.polygon_api.or_elem(I132,I136);
P I138=ev.polygon_api.or_array2(std::vector<P>{I74,I78,I82,I86,I90,I94,I111,I114,I129,I137});
P I139=ev.polygon_api.scale(I138,2,-1,1.5);
P I140=ev.polygon_api.recalculate_normals(I139);
P I141=ev.polygon_api.mix_color(I71,I140,0.5);
VA I142=ev.polygon_api.create_vertex_array(I141,true);
ML I143=ev.polygon_api.render_vertex_array_ml(ev,I142);
PT I144=ev.point_api.point(-60,-30,0);
FD I145=ev.dist_api.sphere(I144,30);
PT I146=ev.point_api.point(0,-30,0);
FD I147=ev.dist_api.sphere(I146,30);
FD I148=ev.dist_api.blend(I145,I147,15.0);
PT I149=ev.point_api.point(60,-30,0);
FD I150=ev.dist_api.sphere(I149,30);
FD I151=ev.dist_api.blend(I148,I150,15.0);
PT I152=ev.point_api.point(-30,0,0);
FD I153=ev.dist_api.sphere(I152,30);
PT I154=ev.point_api.point(30,0,0);
FD I155=ev.dist_api.sphere(I154,30);
FD I156=ev.dist_api.blend(I153,I155,15.0);
PT I157=ev.point_api.point(0,30,0);
FD I158=ev.dist_api.sphere(I157,30);
FD I159=ev.dist_api.blend(I156,I158,15.0);
FD I160=ev.dist_api.blend(I151,I159,15.0);
FD I161=ev.dist_api.trans(I160,0,0,15);
P I162=ev.dist_api.distance_poly_sph(ev,I161,400,400,400,256,256,40,40,50,50,256,256);
P I163=ev.polygon_api.scale(I162,0.2,0.4,0.2);
P I164=ev.polygon_api.rotatex(I163,-0.5);
P I165=ev.polygon_api.rotatey(I164,1);
P I166=ev.polygon_api.translate(I165,140,0,150);
MT I167=ev.materials_api.def(ev);
MT I168=ev.materials_api.snow(ev,I167);
ML I169=ev.materials_api.bind(I166,I168);
ML I170=ev.mainloop_api.array_ml(std::vector<ML>{I143,I169});
 return I170;
}

ML mainloop(EveryApi &ev, MN &move)
{
  //P I1=tree(ev); //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
  ML ml1 = train(ev);
  MN tmn0 = ev.move_api.empty();
  MN tmn1 = ev.move_api.scale2(tmn0, -1.0,1.0,1.0);
  MN tmn2 = ev.move_api.trans2(tmn1, 0.0,0.0,0.0);
  ML ml1a = ev.move_api.move_ml(ev,ml1,tmn2);

  ML ml2 = reki(ev);
  MN mn0 = ev.move_api.empty();
  MN mn1 = ev.move_api.scale2(mn0, -1.0,1.0,1.0);
  MN mn = ev.move_api.trans2(mn1, 1500.0, 0.0, 0.0);
  ML ml2a = ev.move_api.move_ml(ev, ml2, mn);

  // SANTA
  P p3 = tree(ev);
  VA I2=ev.polygon_api.create_vertex_array(p3,true);
  ML ml3=ev.polygon_api.render_vertex_array_ml(ev,I2);
  P ml4 = snowman(ev);
  ML ml5 = gift(ev);
  ML ml6 = santa(ev);
  MN mn6_0 = ev.move_api.empty();
  float mult = 2.0;
  MN mn6_1 = ev.move_api.scale2(mn6_0, -0.5*mult, 0.5*mult, -0.5*mult);
  MN mn6_2 = ev.move_api.trans2(mn6_1, 1500.0, 50.0, -150.0);
  ML ml6a = ev.move_api.move_ml(ev, ml6, mn6_2);
  ML I3 = ev.mainloop_api.array_ml(std::vector<ML>{ml1a, ml2a,ml6a});
  MN I4=ev.move_api.empty();
  MN I5=ev.move_api.trans2(I4,0.0,0.0,0.0);
  ML I6=ev.move_api.move_ml(ev,I3,I5);


  move = I5;

  // TREES
  //P aI1=ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
  P aI1 = p3;
  P aI1a = ev.polygon_api.scale(aI1,2.0,2.0,2.0);

  //P aI1=ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
PT aI2=ev.point_api.point(-3000,0,0);
PT aI3=ev.point_api.point(3000,0,0);
C aI4=ev.curve_api.line(aI2,aI3);
PTS aI5=ev.curve_api.sample(aI4,10);
PT aI6=ev.point_api.point(-3000,0,200);
PT aI7=ev.point_api.point(3000,0,200);
C aI8=ev.curve_api.line(aI6,aI7);
PTS aI9=ev.curve_api.sample(aI8,10);
PTS aI10=ev.points_api.or_points(aI5,aI9);
 PTS aI10a = ev.points_api.scale(aI10, 2.0,2.0,2.0);
 PTS aI10b = ev.points_api.move(aI10a, 12000.0,0.0,0.0);

MT aI11=ev.materials_api.def(ev);
MT aI12=ev.materials_api.web(ev,aI11);
MT aI13=ev.materials_api.snow(ev,aI12);
ML aI14=ev.materials_api.bind_inst(aI1a,aI10b,aI13);


  // SNOWMANS
  //P aI1=ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
 P bI1 = ml4;
  P bI1a = ev.polygon_api.scale(bI1,2.0,2.0,2.0);

  //P aI1=ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
PT bI2=ev.point_api.point(-3000,0,-300);
PT bI3=ev.point_api.point(3000,0,-300);
C bI4=ev.curve_api.line(bI2,bI3);
PTS bI5=ev.curve_api.sample(bI4,4);
PT bI6=ev.point_api.point(-3000,0,200);
PT bI7=ev.point_api.point(3000,0,200);
C bI8=ev.curve_api.line(bI6,bI7);
PTS bI9=ev.curve_api.sample(bI8,4);
PTS bI10=ev.points_api.or_points(bI5,bI9);
 PTS bI10a = ev.points_api.scale(bI10, 2.0,2.0,2.0);
 PTS bI10b = ev.points_api.move(bI10a, -9000.0,0.0,0.0);

 MT bI11=ev.materials_api.def(ev);
 //MT bI12=ev.materials_api.web(ev,bI11);
 MT bI13=ev.materials_api.snow(ev,bI11);
ML bI14=ev.materials_api.bind_inst(bI1a,bI10b,bI13);
ML bI14b=ev.polygon_api.shading_shader(ev,bI14,0xffaaaaaa,0xffeeeeee,0xffffffff);

  
 ML Icomb = ev.mainloop_api.array_ml(std::vector<ML>{I6,aI14, bI14b});


 MN cmn0 = ev.move_api.empty();
 MN cmn1 = ev.move_api.trans2(cmn0, -4400.0*mult, 0.0, -700.0);
 MN cmn1a = ev.move_api.trans2(cmn1, 0.0, 0.0, 0.0);
 MN cmn2 = ev.move_api.translate(cmn1, 0.0, 500.0,  10000.0*mult, 0.0,0.0);
 MN cmn3 = ev.move_api.scale2(cmn2, 1.0, 1.0, 0.6);
 ML Icomb2 = ev.move_api.move_ml(ev, Icomb, cmn3);

 ML txt = happy_xmas_text(ev);
 ML Icomb3 = ev.mainloop_api.array_ml(std::vector<ML>{Icomb2, txt});

  return Icomb3;
}

void iter(void *arg)
{
  Envi *env = (Envi*)arg;
  if (env->logo_shown)
    {
      env->counter++;
      if (env->counter==1) { env->ev->mainloop_api.reset_time(); }
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; 
	env->ev->mainloop_api.reset_time();
#if 1
  env->ev->tracker_api.play_ogg("xmassong.ogg");
#endif
      }
      return;
    }

    env->ev->mainloop_api.clear_3d(0xffaaaaaa);

    M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(env->pos_x,0.0,-env->pos_y);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,-200.0);
    M a_mm = env->ev->matrix_api.mult(env->ev->matrix_api.mult(a_m3,a_m),a_m2);

    env->ev->move_api.set_matrix(env->move, a_mm);
    //env->poly->set_rotation_matrix2(a_mm);
    //env->poly->set_pos(env->pos_x, 0.0, env->pos_y);
    //env->poly->render();

    


    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif

#if 0
	InteractionApi::quake_movement_event(*env->ev, e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
#endif
      }

#if 0
    InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
#endif

    M mat = env->ev->matrix_api.identity();
	env->ev->shader_api.use(env->color_sh);
	env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->color_sh);

	M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
	M in_T = env->ev->mainloop_api.in_MV(*env->ev, true);
	M in_N = env->ev->mainloop_api.in_MV(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N);


    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

}

int main(int argc, char *argv[]) {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.texture_shader();
  SH sh3 = ev.shader_api.texture_array_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);
  ev.mainloop_api.init_3d(sh3);
  ev.shader_api.use(sh);

#if 0
  //M m = ev.matrix_api.perspective(500.0, double(800)/800, 100.1, 2000.0);
  M m = ev.matrix_api.ortho(0, 1280.0, 900.0, 0,0,1);
  ev.shader_api.set_var(sh, "in_P", m);
  ev.shader_api.use(sh2);
  ev.shader_api.set_var(sh2, "in_P", m);
  ev.shader_api.use(sh3);
  ev.shader_api.set_var(sh3, "in_P", m);
  ev.shader_api.use(sh);
#endif

  if (argc==2 && std::string(argv[1])=="--generate-logo")
    {
      std::cout << "Generating Logo" << std::endl;
      ev.mainloop_api.save_logo(ev);
      exit(0);
    }

  //P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
#if 0
  PT center1 = ev.point_api.point(0.0,0.0,0.0);
  PT center2 = ev.point_api.point(30.0,0.0,50.0);
  //P p1 = ev.polygon_api.sphere(center1, 50.0, 30*2*2, 15*2*2);
  float cs = 40.0;

  BO p1 = ev.bool_api.cube(ev, -cs,cs, -cs,cs, -cs,cs, 8, 8);
  BO p2 = ev.bool_api.sphere(ev, center1, 50.0, 30*2*2, 30*2*2);
  BO p3bo = ev.bool_api.and_not(ev, p1, p2);
  BO p22 = ev.bool_api.sphere(ev, center2, 40.0, 30*2, 30*2);
  BO b22bo = ev.bool_api.and_not(ev, p3bo, p22);
  P p3 = ev.bool_api.to_polygon(b22bo);
  //O p3o = ev.bool_api.to_volume(b22bo);
  //P p3 = ev.volume_api.rendercubes3(p3o, 80, 80, 80,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0);

  P p31 = ev.polygon_api.recalculate_normals(p3);
  P p32 = ev.polygon_api.color_from_normals(p31);

  PolygonObj poly(ev, p32, sh);
  poly.prepare();

#endif
  ML ml = mainloop(ev, env.move);
  MN mn0 = ev.move_api.empty();
  MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, 400.0);
  MN mn2 = ev.move_api.scale2(mn, -1.0, 1.0, -1.0);
  ML ml2 = ev.move_api.move_ml(ev, ml, mn2);
  env.mainloop = ml2;


  env.ev = &ev;
  //env.poly = &poly;
  env.color_sh = sh;
  env.texture_sh = sh2;
  env.arr_texture_sh = sh3;

  ev.mainloop_api.reset_time();
  ev.mainloop_api.display_logo(ev);

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
