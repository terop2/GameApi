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
  bool logo_shown = false;
  SH color_sh;
  SH texture_sh;
  SH arr_texture_sh;
};

ML mainloop(EveryApi &ev, MN &move)
{
PT I1=ev.point_api.point(0.0,0.0,0.0);
PT I2=ev.point_api.point(0,690,0);
BM I3=ev.bitmap_api.gradient(I1,I2,0xff884422,0xff221100,1280,690);
PT I4=ev.point_api.point(0.0,0.0,0.0);
PT I5=ev.point_api.point(0,690,0);
BM I6=ev.bitmap_api.gradient(I4,I5,0xffffffff,0xff888888,1280,690);
BM I7=ev.bitmap_api.chessboard(18,10,100,70,0xffffffff,0xff888888);
BB I8=ev.bool_bitmap_api.from_bitmaps_color(I7,255,255,255);
BM I9=ev.bitmap_api.blitbitmap(I3,I6,0,0,I8);
BM I10=ev.bitmap_api.simple_blur(I9,0.5,0.125,0.125,0.125,0.125);
P I11=ev.polygon_api.color_map(I10,745,246,0);
P I12=ev.polygon_api.scale(I11,4,4,4);
P I13=ev.polygon_api.rotatex(I12,1.57);
P I14=ev.polygon_api.translate(I13,-1490,-490,490);
BB I15=ev.bool_bitmap_api.part_circle(100,100,50,50,0,1,20,50);
BB I16=ev.bool_bitmap_api.part_circle(100,100,50,50,2,3,20,50);
BB I17=ev.bool_bitmap_api.or_bitmap(I15,I16);
BB I18=ev.bool_bitmap_api.part_circle(100,100,50,50,-2,-1,20,50);
BB I19=ev.bool_bitmap_api.or_bitmap(I17,I18);
BB I20=ev.bool_bitmap_api.empty(100,100);
BB I21=ev.bool_bitmap_api.circle(I20,50,50,10);
BB I22=ev.bool_bitmap_api.or_bitmap(I19,I21);
BM I23=ev.bool_bitmap_api.to_bitmap(I22,128,64,0,255,140,80,0,255);
BM I24=ev.bitmap_api.repeat_bitmap(I23,6,6);
CBM I25=ev.cont_bitmap_api.from_bitmap(I24,1.0,1.0);
BM I26=ev.cont_bitmap_api.sample(I25,1600,600);
BM I27=ev.bitmap_api.simple_blur(I26,0.5,0.125,0.125,0.125,0.125);
P I28=ev.polygon_api.color_map(I27,745,245,0);
P I29=ev.polygon_api.scale(I28,4,4,4);
P I30=ev.polygon_api.translate(I29,-1490,490,-490);
BB I31=ev.bool_bitmap_api.empty(100,100);
BB I32=ev.bool_bitmap_api.rectangle(I31,50,0,50,50);
BB I33=ev.bool_bitmap_api.empty(100,100);
BB I34=ev.bool_bitmap_api.rectangle(I33,0,50,50,50);
BB I35=ev.bool_bitmap_api.or_bitmap(I32,I34);
BM I36=ev.bool_bitmap_api.to_bitmap(I35,255,128,0,255,255,160,60,255);
BB I37=ev.bool_bitmap_api.empty(100,100);
BB I38=ev.bool_bitmap_api.circle(I37,25,25,15);
BB I39=ev.bool_bitmap_api.empty(100,100);
BB I40=ev.bool_bitmap_api.circle(I39,75,75,15);
BB I41=ev.bool_bitmap_api.or_bitmap(I38,I40);
BM I42=ev.bool_bitmap_api.to_bitmap(I41,255,190,120,255,0,0,0,0);
BM I43=ev.bitmap_api.blitbitmap(I36,I42,0,0);
BM I44=ev.bitmap_api.repeat_bitmap(I43,5,5);
BM I45=ev.bitmap_api.simple_blur(I44,0.5,0.125,0.125,0.125,0.125);
CBM I46=ev.cont_bitmap_api.from_bitmap(I45,1,1);
BM I47=ev.cont_bitmap_api.to_bitmap(I46,254,254);
P I48=ev.polygon_api.color_map(I47,245,245,0);
P I49=ev.polygon_api.rotatey(I48,1.57);
P I50=ev.polygon_api.scale(I49,4,4,4);
P I51=ev.polygon_api.translate(I50,-1490,490,490);
BB I52=ev.bool_bitmap_api.empty(100,100);
BB I53=ev.bool_bitmap_api.rectangle(I52,50,0,50,50);
BB I54=ev.bool_bitmap_api.empty(100,100);
BB I55=ev.bool_bitmap_api.rectangle(I54,0,50,50,50);
BB I56=ev.bool_bitmap_api.or_bitmap(I53,I55);
BM I57=ev.bool_bitmap_api.to_bitmap(I56,255,128,0,255,255,160,60,255);
BB I58=ev.bool_bitmap_api.empty(100,100);
BB I59=ev.bool_bitmap_api.circle(I58,25,25,15);
BB I60=ev.bool_bitmap_api.empty(100,100);
BB I61=ev.bool_bitmap_api.circle(I60,75,75,15);
BB I62=ev.bool_bitmap_api.or_bitmap(I59,I61);
BM I63=ev.bool_bitmap_api.to_bitmap(I62,255,190,120,255,0,0,0,0);
BM I64=ev.bitmap_api.blitbitmap(I57,I63,0,0);
BM I65=ev.bitmap_api.repeat_bitmap(I64,5,5);
BM I66=ev.bitmap_api.simple_blur(I65,0.5,0.125,0.125,0.125,0.125);
CBM I67=ev.cont_bitmap_api.from_bitmap(I66,1,1);
BM I68=ev.cont_bitmap_api.to_bitmap(I67,254,254);
P I69=ev.polygon_api.color_map(I68,245,245,0);
P I70=ev.polygon_api.rotatey(I69,1.57);
P I71=ev.polygon_api.scale(I70,4,4,4);
P I72=ev.polygon_api.translate(I71,1490,490,490);
BB I73=ev.bool_bitmap_api.empty(100,100);
BB I74=ev.bool_bitmap_api.line(I73,0,0,100,100,13,13);
BB I75=ev.bool_bitmap_api.empty(100,100);
BB I76=ev.bool_bitmap_api.line(I75,0,100,100,0,5,5);
BB I77=ev.bool_bitmap_api.or_bitmap(I74,I76);
BM I78=ev.bool_bitmap_api.to_bitmap(I77,150,150,150,255,128,128,128,255);
BM I79=ev.bitmap_api.repeat_bitmap(I78,5,5);
BM I80=ev.bitmap_api.simple_blur(I79,0.5,0.125,0.125,0.125,0.125);
P I81=ev.polygon_api.color_map(I80,745,245,0);
P I82=ev.polygon_api.scale(I81,4,4,4);
P I83=ev.polygon_api.rotatex(I82,1.57);
P I84=ev.polygon_api.translate(I83,-1490,490,490);
P I85=ev.polygon_api.or_array2(std::vector<P>{I14,I30,I51,I72,I84});
MT I86=ev.materials_api.def(ev);
ML I87=ev.materials_api.bind(I85,I86);
P I88=ev.polygon_api.cube(-1490,1490,-490,490,-490,490);
P I89=ev.polygon_api.recalculate_normals(I88);
P I90=ev.polygon_api.color_from_normals(I89);
LI I91=ev.lines_api.from_polygon(I90);
P I92=ev.polygon_api.line_to_cone(ev,I91,35,30);
MT I93=ev.materials_api.def(ev);
MT I94=ev.materials_api.snow(ev,I93);
ML I95=ev.materials_api.bind(I92,I94);
MN I96=ev.move_api.empty();
MN I97=ev.move_api.trans2(I96,100,0,20);
MN I98=ev.move_api.scale2(I97,0.7,0.7,0.7);
MN I99=ev.move_api.empty();
MN I100=ev.move_api.trans2(I99,0,100,0);
MN I101=ev.move_api.scale2(I100,0.7,0.7,0.7);
MN I102=ev.move_api.empty();
MN I103=ev.move_api.rotatex(I102,1.45);
MN I104=ev.move_api.empty();
MN I105=ev.move_api.rotatex(I104,5.6);
TL I106=ev.tree_api.level(std::vector<MN>{I98,I101,I103,I105});
MN I107=ev.move_api.empty();
MN I108=ev.move_api.trans2(I107,100,0,20);
MN I109=ev.move_api.scale2(I108,0.7,0.7,0.7);
MN I110=ev.move_api.empty();
MN I111=ev.move_api.trans2(I110,0,100,0);
MN I112=ev.move_api.scale2(I111,0.7,0.7,0.7);
MN I113=ev.move_api.empty();
MN I114=ev.move_api.rotatex(I113,1.45);
MN I115=ev.move_api.empty();
MN I116=ev.move_api.rotatex(I115,5.6);
TL I117=ev.tree_api.level(std::vector<MN>{I109,I112,I114,I116});
MN I118=ev.move_api.empty();
MN I119=ev.move_api.trans2(I118,100,0,20);
MN I120=ev.move_api.scale2(I119,0.7,0.7,0.7);
MN I121=ev.move_api.empty();
MN I122=ev.move_api.trans2(I121,0,100,0);
MN I123=ev.move_api.scale2(I122,0.7,0.7,0.7);
MN I124=ev.move_api.empty();
MN I125=ev.move_api.rotatex(I124,1.45);
MN I126=ev.move_api.empty();
MN I127=ev.move_api.rotatex(I126,5.6);
TL I128=ev.tree_api.level(std::vector<MN>{I120,I123,I125,I127});
MN I129=ev.move_api.empty();
MN I130=ev.move_api.trans2(I129,100,0,20);
MN I131=ev.move_api.scale2(I130,0.7,0.7,0.7);
MN I132=ev.move_api.empty();
MN I133=ev.move_api.trans2(I132,0,100,0);
MN I134=ev.move_api.scale2(I133,0.7,0.7,0.7);
MN I135=ev.move_api.empty();
MN I136=ev.move_api.rotatex(I135,1.45);
MN I137=ev.move_api.empty();
MN I138=ev.move_api.rotatex(I137,5.6);
TL I139=ev.tree_api.level(std::vector<MN>{I131,I134,I136,I138});
MN I140=ev.move_api.empty();
MN I141=ev.move_api.trans2(I140,100,0,20);
MN I142=ev.move_api.scale2(I141,0.7,0.7,0.7);
MN I143=ev.move_api.empty();
MN I144=ev.move_api.trans2(I143,0,100,0);
MN I145=ev.move_api.scale2(I144,0.7,0.7,0.7);
MN I146=ev.move_api.empty();
MN I147=ev.move_api.rotatex(I146,1.45);
MN I148=ev.move_api.empty();
MN I149=ev.move_api.rotatex(I148,5.6);
TL I150=ev.tree_api.level(std::vector<MN>{I142,I145,I147,I149});
MN I151=ev.move_api.empty();
MN I152=ev.move_api.trans2(I151,100,0,20);
MN I153=ev.move_api.scale2(I152,0.7,0.7,0.7);
MN I154=ev.move_api.empty();
MN I155=ev.move_api.trans2(I154,0,100,0);
MN I156=ev.move_api.scale2(I155,0.7,0.7,0.7);
MN I157=ev.move_api.empty();
MN I158=ev.move_api.rotatex(I157,1.45);
MN I159=ev.move_api.empty();
MN I160=ev.move_api.rotatex(I159,5.6);
TL I161=ev.tree_api.level(std::vector<MN>{I153,I156,I158,I160});
T I162=ev.tree_api.tree(std::vector<TL>{I106,I117,I128,I139,I150,I161});
PT I163=ev.point_api.point(-100,0,0);
PT I164=ev.point_api.point(0,100,40);
LI I165=ev.lines_api.point_array(std::vector<PT>{I163,I164});
P I166=ev.polygon_api.line_to_cone(ev,I165,10,15);
PT I167=ev.point_api.point(-100,0,0);
PT I168=ev.point_api.point(0,100,40);
LI I169=ev.lines_api.point_array(std::vector<PT>{I167,I168});
P I170=ev.polygon_api.line_to_cone(ev,I169,10,15);
PT I171=ev.point_api.point(-100,0,0);
PT I172=ev.point_api.point(0,100,40);
LI I173=ev.lines_api.point_array(std::vector<PT>{I171,I172});
P I174=ev.polygon_api.line_to_cone(ev,I173,10,15);
PT I175=ev.point_api.point(-100,0,0);
PT I176=ev.point_api.point(0,100,40);
LI I177=ev.lines_api.point_array(std::vector<PT>{I175,I176});
P I178=ev.polygon_api.line_to_cone(ev,I177,10,15);
PT I179=ev.point_api.point(-100,0,0);
PT I180=ev.point_api.point(0,100,40);
LI I181=ev.lines_api.point_array(std::vector<PT>{I179,I180});
P I182=ev.polygon_api.line_to_cone(ev,I181,10,15);
PT I183=ev.point_api.point(-100,0,0);
P I184=ev.polygon_api.sphere(I183,10,10,8);
P I185=ev.tree_api.tree_p(ev,I162,std::vector<P>{I166,I170,I174,I178,I182,I184},0.0);
MT I186=ev.materials_api.def(ev);
MT I187=ev.materials_api.web(ev,I186);
MT I188=ev.materials_api.snow(ev,I187);
ML I189=ev.materials_api.bind(I185,I188);
MN I190=ev.move_api.empty();
MN I191=ev.move_api.rotatez(I190,-1.4);
MN I192=ev.move_api.rotate(I191,0,10000,0,0,0,1,0,0,310);
MN I193=ev.move_api.scale2(I192,1,-3,1);
MN I194=ev.move_api.trans2(I193,1000,-200,-250);
ML I195=ev.move_api.move_ml(ev,I189,I194);
PT I196=ev.point_api.point(0.0,0.0,0.0);
P I197=ev.polygon_api.torus2(ev,14,10,I196,250,150);
LI I198=ev.lines_api.from_polygon(I197);
LI I199=ev.lines_api.split_lines(I198,5.0);
LI I200=ev.lines_api.twist_y(I199,0,0.014);
P I201=ev.polygon_api.line_to_cone(ev,I200,2,4);
MT I202=ev.materials_api.def(ev);
MT I203=ev.materials_api.snow(ev,I202);
ML I204=ev.materials_api.bind(I201,I203);
MN I205=ev.move_api.empty();
MN I206=ev.move_api.rotate(I205,0,10000,0,0,0,0,1,0,3100);
MN I207=ev.move_api.trans2(I206,-1600,-400,-400);
MN I208=ev.move_api.scale2(I207,0.5,0.5,0.5);
ML I209=ev.move_api.move_ml(ev,I204,I208);
P I210=ev.polygon_api.cube(-120,120,-100,100,-60,60);
P I211=ev.polygon_api.cube(-90,90,-80,80,60,65);
P I212=ev.polygon_api.or_array2(std::vector<P>{I210,I211});
LI I213=ev.lines_api.from_polygon(I212);
P I214=ev.polygon_api.line_to_cone(ev,I213,3,3);
P I215=ev.polygon_api.translate(I214,-300,-300,-400);
MT I216=ev.materials_api.def(ev);
MT I217=ev.materials_api.web(ev,I216);
ML I218=ev.materials_api.bind(I215,I217);
MN I219=ev.move_api.empty();
MN I220=ev.move_api.translate(I219,0,100,0,1200,0);
MN I221=ev.move_api.translate(I220,100,200,0,-1200,0);
MN I222=ev.move_api.time_repeat(I221,0,250);
ML I223=ev.move_api.move_ml(ev,I218,I222);
PT I224=ev.point_api.point(0,0,0);
P I225=ev.polygon_api.sphere(I224,100,30,30);
PT I226=ev.point_api.point(0,0,0);
PT I227=ev.point_api.point(0,100,0);
P I228=ev.polygon_api.cone(30,I226,I227,30,10);
P I229=ev.polygon_api.cube(0,200,0,100,0,100);
P I230=ev.polygon_api.or_array2(std::vector<P>{I229});
P I231=ev.polygon_api.cube(-200,0,0,100,0,100);
P I232=ev.polygon_api.or_array2(std::vector<P>{I231});
PT I233=ev.point_api.point(0,0,60);
PT I234=ev.point_api.point(0,300,60);
P I235=ev.polygon_api.cone(30,I233,I234,30,40);
PT I236=ev.point_api.point(0,0,60);
PT I237=ev.point_api.point(0,300,60);
P I238=ev.polygon_api.cone(30,I236,I237,30,40);
P I239=ev.polygon_api.rounded_cube(ev,-150,150,120,400,0,100,10);
P I240=ev.polygon_api.empty();
P I241=ev.polygon_api.empty();
PT I242=ev.point_api.point(0,0,60);
PT I243=ev.point_api.point(0,400,60);
P I244=ev.polygon_api.cone(30,I242,I243,30,30);
PT I245=ev.point_api.point(0,0,60);
PT I246=ev.point_api.point(0,400,60);
P I247=ev.polygon_api.cone(30,I245,I246,30,30);
P I248=ev.polygon_api.cube(80,180,-400,-360,0,200);
P I249=ev.polygon_api.cube(-80,-180,-400,-360,0,200);
PT I250=ev.point_api.point(0,0,0);
PT I251=ev.point_api.point(0,100,0);
PT I252=ev.point_api.point(200,100,0);
PT I253=ev.point_api.point(-200,100,0);
PT I254=ev.point_api.point(200,300,0);
PT I255=ev.point_api.point(-200,300,0);
PT I256=ev.point_api.point(0,400,0);
PT I257=ev.point_api.point(-120,0,0);
PT I258=ev.point_api.point(120,0,0);
PT I259=ev.point_api.point(-120,800,0);
PT I260=ev.point_api.point(120,800,0);
PT I261=ev.point_api.point(0,-400,0);
PT I262=ev.point_api.point(0,400,0);
PT I263=ev.point_api.point(0,0,0);
SA I264=ev.skeletal_api.root(I263);
PT I265=ev.point_api.point(0,0,0);
SA I266=ev.skeletal_api.root(I265);
MN I267=ev.move_api.empty();
PT I268=ev.point_api.point(0,100,0);
SA I269=ev.skeletal_api.node(I266,I267,I268);
PT I270=ev.point_api.point(0,0,0);
SA I271=ev.skeletal_api.root(I270);
MN I272=ev.move_api.empty();
PT I273=ev.point_api.point(0,100,0);
SA I274=ev.skeletal_api.node(I271,I272,I273);
MN I275=ev.move_api.empty();
PT I276=ev.point_api.point(200,100,0);
SA I277=ev.skeletal_api.node(I274,I275,I276);
PT I278=ev.point_api.point(0,0,0);
SA I279=ev.skeletal_api.root(I278);
MN I280=ev.move_api.empty();
PT I281=ev.point_api.point(0,100,0);
SA I282=ev.skeletal_api.node(I279,I280,I281);
MN I283=ev.move_api.empty();
PT I284=ev.point_api.point(-200,100,0);
SA I285=ev.skeletal_api.node(I282,I283,I284);
PT I286=ev.point_api.point(0,0,0);
SA I287=ev.skeletal_api.root(I286);
MN I288=ev.move_api.empty();
PT I289=ev.point_api.point(0,100,0);
SA I290=ev.skeletal_api.node(I287,I288,I289);
MN I291=ev.move_api.empty();
PT I292=ev.point_api.point(200,100,0);
SA I293=ev.skeletal_api.node(I290,I291,I292);
MN I294=ev.move_api.empty();
MN I295=ev.move_api.rotate(I294,0,2,0,0,0,1,0,0,0.8);
MN I296=ev.move_api.rotate(I295,6,8,0,0,0,1,0,0,0.8);
MN I297=ev.move_api.rotate(I296,2,6,0,0,0,1,0,0,-1.57);
MN I298=ev.move_api.time_repeat(I297,0,8);
PT I299=ev.point_api.point(200,300,0);
SA I300=ev.skeletal_api.node(I293,I298,I299);
PT I301=ev.point_api.point(0,0,0);
SA I302=ev.skeletal_api.root(I301);
MN I303=ev.move_api.empty();
PT I304=ev.point_api.point(0,100,0);
SA I305=ev.skeletal_api.node(I302,I303,I304);
MN I306=ev.move_api.empty();
PT I307=ev.point_api.point(-200,100,0);
SA I308=ev.skeletal_api.node(I305,I306,I307);
MN I309=ev.move_api.empty();
MN I310=ev.move_api.rotate(I309,0,2,0,0,0,1,0,0,-0.8);
MN I311=ev.move_api.rotate(I310,2,6,0,0,0,1,0,0,1.57);
MN I312=ev.move_api.rotate(I311,6,8,0,0,0,1,0,0,-0.8);
MN I313=ev.move_api.time_repeat(I312,0,8);
PT I314=ev.point_api.point(-200,300,0);
SA I315=ev.skeletal_api.node(I308,I313,I314);
PT I316=ev.point_api.point(0,0,0);
SA I317=ev.skeletal_api.root(I316);
MN I318=ev.move_api.empty();
PT I319=ev.point_api.point(0,100,0);
SA I320=ev.skeletal_api.node(I317,I318,I319);
MN I321=ev.move_api.empty();
PT I322=ev.point_api.point(0,400,0);
SA I323=ev.skeletal_api.node(I320,I321,I322);
PT I324=ev.point_api.point(0,0,0);
SA I325=ev.skeletal_api.root(I324);
MN I326=ev.move_api.empty();
PT I327=ev.point_api.point(0,100,0);
SA I328=ev.skeletal_api.node(I325,I326,I327);
MN I329=ev.move_api.empty();
PT I330=ev.point_api.point(0,400,0);
SA I331=ev.skeletal_api.node(I328,I329,I330);
MN I332=ev.move_api.empty();
PT I333=ev.point_api.point(-120,0,0);
SA I334=ev.skeletal_api.node(I331,I332,I333);
PT I335=ev.point_api.point(0,0,0);
SA I336=ev.skeletal_api.root(I335);
MN I337=ev.move_api.empty();
PT I338=ev.point_api.point(0,100,0);
SA I339=ev.skeletal_api.node(I336,I337,I338);
MN I340=ev.move_api.empty();
PT I341=ev.point_api.point(0,400,0);
SA I342=ev.skeletal_api.node(I339,I340,I341);
MN I343=ev.move_api.empty();
PT I344=ev.point_api.point(120,0,0);
SA I345=ev.skeletal_api.node(I342,I343,I344);
PT I346=ev.point_api.point(0,0,0);
SA I347=ev.skeletal_api.root(I346);
MN I348=ev.move_api.empty();
PT I349=ev.point_api.point(0,100,0);
SA I350=ev.skeletal_api.node(I347,I348,I349);
MN I351=ev.move_api.empty();
PT I352=ev.point_api.point(0,400,0);
SA I353=ev.skeletal_api.node(I350,I351,I352);
MN I354=ev.move_api.empty();
PT I355=ev.point_api.point(-120,0,0);
SA I356=ev.skeletal_api.node(I353,I354,I355);
MN I357=ev.move_api.empty();
MN I358=ev.move_api.rotate(I357,0,2,0,0,0,1,0,0,0.2);
MN I359=ev.move_api.rotate(I358,2,6,0,0,0,1,0,0,-0.4);
MN I360=ev.move_api.rotate(I359,6,8,0,0,0,1,0,0,0.2);
MN I361=ev.move_api.time_repeat(I360,0,8);
PT I362=ev.point_api.point(-120,800,0);
SA I363=ev.skeletal_api.node(I356,I361,I362);
PT I364=ev.point_api.point(0,0,0);
SA I365=ev.skeletal_api.root(I364);
MN I366=ev.move_api.empty();
PT I367=ev.point_api.point(0,100,0);
SA I368=ev.skeletal_api.node(I365,I366,I367);
MN I369=ev.move_api.empty();
PT I370=ev.point_api.point(0,400,0);
SA I371=ev.skeletal_api.node(I368,I369,I370);
MN I372=ev.move_api.empty();
PT I373=ev.point_api.point(120,0,0);
SA I374=ev.skeletal_api.node(I371,I372,I373);
MN I375=ev.move_api.empty();
MN I376=ev.move_api.rotate(I375,0,2,0,0,0,1,0,0,-0.2);
MN I377=ev.move_api.rotate(I376,2,6,0,0,0,1,0,0,0.4);
MN I378=ev.move_api.rotate(I377,6,8,0,0,0,1,0,0,-0.2);
MN I379=ev.move_api.time_repeat(I378,0,8);
PT I380=ev.point_api.point(120,800,0);
SA I381=ev.skeletal_api.node(I374,I379,I380);
PT I382=ev.point_api.point(0,0,0);
SA I383=ev.skeletal_api.root(I382);
MN I384=ev.move_api.empty();
PT I385=ev.point_api.point(0,100,0);
SA I386=ev.skeletal_api.node(I383,I384,I385);
MN I387=ev.move_api.empty();
PT I388=ev.point_api.point(0,400,0);
SA I389=ev.skeletal_api.node(I386,I387,I388);
MN I390=ev.move_api.empty();
PT I391=ev.point_api.point(-120,0,0);
SA I392=ev.skeletal_api.node(I389,I390,I391);
MN I393=ev.move_api.empty();
MN I394=ev.move_api.rotate(I393,0,2,0,0,0,1,0,0,0.2);
MN I395=ev.move_api.rotate(I394,2,6,0,0,0,1,0,0,-0.4);
MN I396=ev.move_api.rotate(I395,6,8,0,0,0,1,0,0,0.2);
MN I397=ev.move_api.time_repeat(I396,0,8);
PT I398=ev.point_api.point(-120,800,0);
SA I399=ev.skeletal_api.node(I392,I397,I398);
MN I400=ev.move_api.empty();
PT I401=ev.point_api.point(0,-400,0);
SA I402=ev.skeletal_api.node(I399,I400,I401);
PT I403=ev.point_api.point(0,0,0);
SA I404=ev.skeletal_api.root(I403);
MN I405=ev.move_api.empty();
PT I406=ev.point_api.point(0,100,0);
SA I407=ev.skeletal_api.node(I404,I405,I406);
MN I408=ev.move_api.empty();
PT I409=ev.point_api.point(0,400,0);
SA I410=ev.skeletal_api.node(I407,I408,I409);
MN I411=ev.move_api.empty();
PT I412=ev.point_api.point(120,0,0);
SA I413=ev.skeletal_api.node(I410,I411,I412);
MN I414=ev.move_api.empty();
MN I415=ev.move_api.rotate(I414,0,2,0,0,0,1,0,0,-0.2);
MN I416=ev.move_api.rotate(I415,2,6,0,0,0,1,0,0,0.4);
MN I417=ev.move_api.rotate(I416,6,8,0,0,0,1,0,0,-0.2);
MN I418=ev.move_api.time_repeat(I417,0,8);
PT I419=ev.point_api.point(120,800,0);
SA I420=ev.skeletal_api.node(I413,I418,I419);
MN I421=ev.move_api.empty();
PT I422=ev.point_api.point(0,400,0);
SA I423=ev.skeletal_api.node(I420,I421,I422);
MT I424=ev.materials_api.skeletal(ev);
MT I425=ev.materials_api.snow(ev,I424);
ML I426=ev.skeletal_api.skeletal_bind_material(ev,std::vector<P>{I225,I228,I230,I232,I235,I238,I239,I240,I241,I244,I247,I248,I249},std::vector<PT>{I250,I251,I252,I253,I254,I255,I256,I257,I258,I259,I260,I261,I262},std::vector<SA>{I264,I269,I277,I285,I300,I315,I323,I334,I345,I363,I381,I402,I423},I425);
MN I427=ev.move_api.empty();
MN I428=ev.move_api.scale2(I427,0.5,-0.5,0.5);
MN I429=ev.move_api.trans2(I428,0,0,0);
ML I430=ev.move_api.move_ml(ev,I426,I429);
MN I431=ev.move_api.empty();
MN I432=ev.move_api.rotate(I431,0,10,0,0,0,0,1,0,1.59);
ML I433=ev.move_api.temp_key_activate_ml(ev,I430,I432,100,10);
MN I434=ev.move_api.empty();
MN I435=ev.move_api.rotate(I434,0,10,0,0,0,0,1,0,-1.59);
ML I436=ev.move_api.temp_key_activate_ml(ev,I433,I435,97,10);
MN I437=ev.move_api.empty();
MN I438=ev.move_api.translate(I437,0,10,-150,0,0);
ML I439=ev.move_api.key_activate_ml(ev,I436,I438,97,10);
MN I440=ev.move_api.empty();
MN I441=ev.move_api.translate(I440,0,10,0,0,150);
ML I442=ev.move_api.key_activate_ml(ev,I439,I441,115,10);
MN I443=ev.move_api.empty();
MN I444=ev.move_api.translate(I443,0,10,0,0,-150);
ML I445=ev.move_api.key_activate_ml(ev,I442,I444,119,10);
MN I446=ev.move_api.empty();
MN I447=ev.move_api.translate(I446,0,10,0,500,0);
ML I448=ev.move_api.temp_key_activate_ml(ev,I445,I447,32,10);
MN I449=ev.move_api.empty();
MN I450=ev.move_api.translate(I449,0,10,150,0,0);
ML I451=ev.move_api.key_activate_ml(ev,I448,I450,100,10);
ML I452=ev.mainloop_api.array_ml(std::vector<ML>{I451});
PT I453=ev.point_api.point(0.0,0.0,0.0);
P I454=ev.polygon_api.sphere(I453,300,30,30);
LI I455=ev.lines_api.from_polygon(I454);
LI I456=ev.lines_api.twist_y(I455,0,0.003);
P I457=ev.polygon_api.line_to_cone(ev,I456,2,15);
MT I458=ev.materials_api.def(ev);
MT I459=ev.materials_api.brashmetal(ev,I458,2000,true);
ML I460=ev.materials_api.bind(I457,I459);
MN I461=ev.move_api.empty();
MN I462=ev.move_api.trans2(I461,-2500,0,0);
MN I463=ev.move_api.translate(I462,0,100,5000,0,0);
MN I464=ev.move_api.rotate(I463,0,1000,0,0,0,0,0,1,-628.3);
ML I465=ev.move_api.move_ml(ev,I460,I464);
ML I466=ev.mainloop_api.array_ml(std::vector<ML>{I87,I95,I195,I209,I223,I452,I465});
MN I467=ev.move_api.empty();
MN I468=ev.move_api.trans2(I467,-2500,0,-1500);
MN I469=ev.move_api.translate(I468,0,150,2500,0,0);
MN I470=ev.move_api.translate(I469,150,250,0,0,1100);
ML I471=ev.move_api.move_ml(ev,I466,I470);
 return I471;
}

void iter(void *arg)
{
  Envi *env = (Envi*)arg;
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; }
      return;
    }

    env->ev->mainloop_api.clear_3d(0xff000000);

    //M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(env->pos_x,0.0,-env->pos_y);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,0.0);
    M a_mm = env->ev->matrix_api.mult(a_m3 /*env->ev->matrix_api.mult(a_m3,a_m)*/,a_m2);

    //env->ev->move_api.set_matrix(env->move, a_mm);
    //env->poly->set_rotation_matrix2(a_mm);
    //env->poly->set_pos(env->pos_x, 0.0, env->pos_y);
    //env->poly->render();

    


    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif

	InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e);

      }

    InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);

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
  MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, -400.0);
  ML ml2 = ev.move_api.move_ml(ev, ml, mn);
  env.mainloop = ml2;

  env.ev = &ev;
  //env.poly = &poly;
  env.color_sh = sh;
  env.texture_sh = sh2;
  env.arr_texture_sh = sh3;

  ev.mainloop_api.reset_time();
  if (env.logo_shown)
    ev.mainloop_api.display_logo(ev);
  ev.mainloop_api.alpha(true);

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 0,1);
#endif



}
