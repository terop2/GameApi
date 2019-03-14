
#include "GameApi.hh"
#include <fstream>

using namespace GameApi;

GameApi::RUN code_refresh(Env &e, EveryApi &ev, std::string filename);


GameApi::RUN code_stl(Env &e, EveryApi &ev,std::string filename="http://tpgames.org/wooly_sheep.stl")
{
  P I1=ev.polygon_api.stl_load(filename.c_str());
  P I2=ev.polygon_api.scale(I1,1.5,1.5,1.5);
  P I3=ev.polygon_api.rotatex(I2,-1.57);
  MT I4=ev.materials_api.def(ev);
  MT I5=ev.materials_api.phong(ev,I4,-0.3,0.3,-1.0,0xffff8800,0xff666666,5.0);
  ML I6=ev.materials_api.bind(I3,I5);
  ML I7=ev.move_api.rot_y_ml(ev,I6,100,97,0.001,-100.0,100.0);
  //RUN I8=ev.blocker_api.game_window2(ev,I7,false,false,0.0,100000.0);
 ML I7a=ev.mainloop_api.drag_drop_area(ev, I7, &code_refresh);

 ML I82=ev.mainloop_api.looking_glass_full(ev,I7a,150,2);
RUN I83=ev.blocker_api.game_window_2nd_display(ev,I82,false,false,0.0,1000000.0)
; 
 return I83;
}

GameApi::RUN code_obj(Env &e, EveryApi &ev,std::string filename="http://tpgames.org/example.obj")
{
  P I1=ev.polygon_api.p_url(ev,filename.c_str(),1);
  P I2=ev.polygon_api.scale(I1,1.5,1.5,1.5);
  P I3=ev.polygon_api.rotatex(I2,-1.57);
  MT I4=ev.materials_api.def(ev);
  MT I5=ev.materials_api.phong(ev,I4,-0.3,0.3,-1.0,0xffff8800,0xff666666,5.0);
  ML I6=ev.materials_api.bind(I3,I5);
  ML I7=ev.move_api.rot_y_ml(ev,I6,100,97,0.001,-100.0,100.0);
  //RUN I8=ev.blocker_api.game_window2(ev,I7,false,false,0.0,100000.0);
 ML I7a=ev.mainloop_api.drag_drop_area(ev, I7, &code_refresh);

 ML I82=ev.mainloop_api.looking_glass_full(ev,I7a,150,2);
RUN I83=ev.blocker_api.game_window_2nd_display(ev,I82,false,false,0.0,1000000.0)
; 

  return I83;
}

void setup_data(std::string filename);
GameApi::RUN code(Env &e, EveryApi &ev, std::string filename) {
  std::cout << "DragDrop filename: " << filename << std::endl;
  setup_data(filename);
  std::string ext = filename.substr(filename.size()-3,3);
  if (ext=="obj") return code_obj(e,ev,filename);
  if (ext=="stl") return code_stl(e,ev,filename);
  std::cout << "Invalid extension: " << ext << std::endl;
  GameApi::RUN r;
  r.id = 0;
  return r;
}

GameApi::RUN code_refresh(Env &e, EveryApi &ev, std::string filename) {
  return code(e,ev,filename);
}


GameApi::RUN code1(Env &e, EveryApi &ev)
{
BB I1=ev.bool_bitmap_api.empty(100,100);
BB I2=ev.bool_bitmap_api.rectangle(I1,30,30,4,40);
BB I3=ev.bool_bitmap_api.rectangle(I2,30,70,40,4);
BB I4=ev.bool_bitmap_api.rectangle(I3,66,30,4,40);
BM I5=ev.bool_bitmap_api.to_bitmap(I4,255,255,255,255,0,0,0,0);
ML I6=ev.sprite_api.vertex_array_render(ev,I5);
ML I7=ev.sprite_api.turn_to_2d(ev,I6,0.0,0.0,800.0,600.0);
 ML I7a=ev.mainloop_api.drag_drop_area(ev, I7, &code);
RUN I8=ev.blocker_api.game_window2(ev,I7a,false,false,0.0,100000.0);
 return I8;
}


void setup_data(std::string filename)
{
  std::ifstream ss(filename.c_str(),std::ifstream::in|std::ifstream::binary);
  char ch;
  std::vector<unsigned char> *s = new std::vector<unsigned char>;
  while(ss.get(ch)) { s->push_back(ch); }
  char *ptr = new char[filename.size()+1];
  std::copy(filename.begin(),filename.end(), ptr);
  append_url_map(ptr, &(*s)[0], &(*s)[s->size()]);
}

int main()
{
  Env e;
  EveryApi ev(e);
  

#ifdef WINDOWS
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800;
  int w_height = 600;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif
#ifdef LINUX
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800;
  int w_height = 600;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif

  RUN r = code1(e,ev);
  ev.blocker_api.run2(ev,r);
}
