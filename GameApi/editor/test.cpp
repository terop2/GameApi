#include "GameApi.hh"
#include <sstream>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace GameApi;

struct Envi {
  EveryApi *ev;
  GuiApi *gui;
  W txt;
  W txt2;
  W scroll_area;
  std::vector<W> menus;
  int opened_menu_num;
  bool state;
};

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d();
    
    env->gui->render(env->txt2);

    env->gui->render(env->txt);
    env->gui->render(env->scroll_area);
    if (env->opened_menu_num != -1)
      {
	//std::cout << env->opened_menu_num << std::endl;
	env->gui->render(env->menus[env->opened_menu_num]);
      }
    
    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env->ev->mainloop_api.get_event();
    //std::cout << e.button << std::endl;
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
    env->gui->update(env->txt, e.cursor_pos, e.button);
    env->gui->update(env->txt2, e.cursor_pos, e.button);
    env->gui->update(env->scroll_area, e.cursor_pos, e.button);
    float param_x = env->gui->dynamic_param(env->txt2, 0);
    env->gui->set_dynamic_param(env->scroll_area, 1, param_x);
    int selected_item = env->gui->chosen_item(env->txt);
    int selected_item2 = -1;
    if (selected_item != -1)
      {
	env->opened_menu_num = selected_item;
      }
    if (env->opened_menu_num != -1)
      {
	env->gui->update(env->menus[env->opened_menu_num], e.cursor_pos, e.button);
	if (e.button == -1) { env->state=1; }
	if (e.button==0 && env->state==1)
	  {
	    selected_item2 = env->gui->chosen_item(env->menus[env->opened_menu_num]);
	    env->opened_menu_num = -1;
	    env->state = 0;
	    std::cout << selected_item2 << std::endl;
	  }
      }
}

int main() {
  Env e;
  EveryApi ev(e);

  Envi env;

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.texture_shader();
  Ft font = ev.font_api.newfont("FreeSans.ttf", 13,15);

  // rest of the initializations
  ev.mainloop_api.init(sh);
  ev.mainloop_api.alpha(true);
  
  GuiApi gui(e, ev, sh);
  
  //W txt_0 = gui.highlight(100,100);
  //W txt_1 = gui.margin(txt_0, 10, 10, 10, 10);
  //W txt_2 = gui.text("Hello World", font);
  //W txt_3 = gui.layer(txt_1, txt_2);
  //W arr[] = { txt_1, txt_2 };
  //W txt = gui.array_x(arr, 2, 2);

  std::vector<std::string> vec;
  vec.push_back("Edit");
  vec.push_back("File");
  vec.push_back("Open");
  W txt = gui.main_menu(vec, font);
  std::vector<std::string> vec2;
  vec2.push_back("test1");
  vec2.push_back("test2");
  vec2.push_back("test3");
  std::vector<W> menus;
  for(int i=0;i<vec.size();i++)
    {
      W txt_2 = gui.menu(txt,i, vec2, font);
      menus.push_back(txt_2);
    }
			 

  std::vector<W> items;
  for(int i=0;i<100;i++)
    {
      std::string label;
      std::stringstream ss;
      ss << "Label " << i;
      label = ss.str();
      items.push_back(gui.text(label, font));
    }
  W array = gui.array_y(&items[0], items.size(), 15);
  W scroll_area = gui.scroll_area(array, gui.size_x(array), 300);

  W txt2 = gui.scrollbar_y(20, 300, gui.size_y(array));
  gui.set_pos(txt2, 15.0+gui.size_x(scroll_area), 100);

  gui.set_pos(scroll_area, 15.0, 100.0);

  env.gui = &gui;
  env.ev = &ev;
  env.txt = txt;
  env.txt2 = txt2;
  env.scroll_area = scroll_area;
  env.menus = menus;
  env.opened_menu_num = -1;
#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
