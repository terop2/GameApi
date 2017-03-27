#include "GameApi.hh"

using namespace GameApi;

int main()
{
  Env e;
  EveryApi ev(e);

  ev.mainloop_api.init_window();
  ev.shader_api.load_default();
  // INSERT CODE HERE
BB I1=ev.bool_bitmap_api.rings(1280,900,640,450,640,450,10,800,60,60,8);
BM I2=ev.bool_bitmap_api.to_bitmap(I1,255,255,255,255,200,200,200,200);
BM I3=ev.bitmap_api.scale_bitmap_fullscreen(ev,I2);
ML I4=ev.sprite_api.vertex_array_render(ev,I3);
ML I5=ev.sprite_api.turn_to_2d(ev,I4,0.0,0.0,800.0,600.0);
PT I6=ev.point_api.point(0.0,0.0,0.0);
P I7=ev.polygon_api.torus2(ev,20,20,I6,250.0,50.0);
PT I8=ev.point_api.point(0,0,-300);
PT I9=ev.point_api.point(0,0,300);
C I10=ev.curve_api.line(I8,I9);
PTS I11=ev.curve_api.sample(I10,6);
MT I12=ev.materials_api.def(ev);
MT I13=ev.materials_api.snow(ev,I12,0xffaaaaaa,0xffeeeeee,0xffffffff,0.5);
ML I14=ev.materials_api.bind_inst(I7,I11,I13);
ML I15=ev.mainloop_api.array_ml(std::vector<ML>{I5,I14});
MN I16=ev.move_api.empty();
MN I17=ev.move_api.rotate(I16,0,400,0,0,0,0,1,0,62);
ML I18=ev.move_api.move_ml(ev,I15,I17,1,10.0);
BLK I19=ev.blocker_api.game_window(ev,I18,false,false,0.0,100000.0);
  // INSERT CODE HERE
  ev.blocker_api.run(I19);
}
