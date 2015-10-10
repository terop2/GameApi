#include "GameApi.hh"
#include <sstream>
#include <math.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace GameApi;



struct Envi {
  EveryApi *ev;
  GuiApi *gui;
  std::string target_string;
  float x,y,z;
  std::vector<GuiApi::EditTypes> edit_data;
  W editor;
  bool editor_visible;
  W txt;
  W txt2;
  W scroll_area;
  W wave;
  W array;
  W chosen_item;
  W insert_widget;
  W dialog_cancel, dialog_ok;
  Ft font;
  Ft font3;
  bool insert_ongoing;
  bool insert_ongoing2;
  // gameapi;
  //W test1;
  WM mod;
  W canvas, canvas_area, scrollbar_x, scrollbar_y;
  std::vector<W> menus;
  int opened_menu_num;
  bool state;
  bool ctrl;
};

void callback_func(int x, int y, Envi *envi)
{
  std::cout << "Insert!" << std::endl;
}

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear_3d();
    
    env->gui->render(env->txt2);

    env->gui->render(env->txt);
    env->gui->render(env->scroll_area);
    //env->gui->render(env->wave);
    //env->gui->render(env->gameapi);
    //env->gui->render(env->test1);
    env->gui->render(env->canvas_area);
    env->gui->render(env->scrollbar_x);
    env->gui->render(env->scrollbar_y);
    if (env->editor_visible)
      env->gui->render(env->editor);
    if (env->insert_ongoing)
      {
	env->gui->render(env->insert_widget);
      }
    if (env->opened_menu_num != -1)
      {
	//std::cout << env->opened_menu_num << std::endl;
	env->gui->render(env->menus[env->opened_menu_num]);
      }
    
    //env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env->ev->mainloop_api.get_event();
    //std::cout << e.button << std::endl;
    if (e.ch==1073742048 && e.type == 0x300)
      {
	env->ctrl = true;
      }
    if (e.ch==1073742048 && e.type != 0x300)
      {
	env->ctrl = false;
      }
    if (env->ctrl && e.ch==115 && e.type==0x300)
      { // Save.
	std::cout << "Saving..." << std::endl;
	env->ev->mod_api.save(env->mod, "mod.txt");

      }

    //std::cout << e.ch << std::endl;
#ifndef EMSCRIPTEN
    if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
    if (e.type != 0x300)
      {
	e.ch=-1;
      }

    int s = env->gui->num_childs(env->canvas);
    //std::cout << "Numchilds: " << s << std::endl;
    for(int i=0;i<s;i++)
      {
	W w = env->gui->get_child(env->canvas, i);
	int chosen = env->gui->chosen_item(w);
	//std::cout << "c: " << chosen << std::endl;
	if (chosen == 0)
	  {
	    std::cout << "Chosen: " << i << ":" << chosen << std::endl;


	    env->edit_data.push_back(GuiApi::EditTypes());
	    env->edit_data.push_back(GuiApi::EditTypes());
	    env->edit_data.push_back(GuiApi::EditTypes());
	    env->edit_data.push_back(GuiApi::EditTypes());
	    std::vector<GuiApi::EditTypes*> vec4;
	    vec4.push_back(&env->edit_data[0]);
	    vec4.push_back(&env->edit_data[1]);
	    vec4.push_back(&env->edit_data[2]);
	    vec4.push_back(&env->edit_data[3]);
	    std::vector<std::string> types;
	    types.push_back("int");
	    types.push_back("PT");
	    types.push_back("float");
	    types.push_back("int");
	    std::vector<std::string> labels;
	    labels.push_back("Test: ");
	    labels.push_back("Test2: ");
	    labels.push_back("Test3: ");
	    labels.push_back("Test4: ");
	    env->editor = env->gui->edit_dialog(labels,vec4,env->font3, types, env->dialog_cancel, env->dialog_ok);
	    env->gui->set_pos(env->editor, 200,50);

	    env->editor_visible = true;

	  }
      }

    env->gui->update(env->txt, e.cursor_pos, e.button, e.ch);
    if (env->editor_visible)
      env->gui->update(env->editor, e.cursor_pos, e.button, e.ch);
    env->gui->update(env->txt2, e.cursor_pos, e.button, e.ch);
    env->gui->update(env->scroll_area, e.cursor_pos, e.button,e.ch);
    //env->gui->update(env->wave, e.cursor_pos, e.button);
    //env->gui->update(env->gameapi, e.cursor_pos, e.button);
    //env->gui->update(env->test1, e.cursor_pos, e.button);
    env->gui->update(env->canvas_area, e.cursor_pos, e.button,e.ch);
    env->gui->update(env->scrollbar_x, e.cursor_pos, e.button,e.ch);
    env->gui->update(env->scrollbar_y, e.cursor_pos, e.button,e.ch);
    if (env->insert_ongoing)
      {
	env->gui->update(env->insert_widget, e.cursor_pos, e.button,e.ch);
      }

    float param_x = env->gui->dynamic_param(env->txt2, 0);
    env->gui->set_dynamic_param(env->scroll_area, 1, param_x);
    float param_x1 = env->gui->dynamic_param(env->scrollbar_x, 0);
    env->gui->set_dynamic_param(env->canvas_area, 0, param_x1);
    float param_y1 = env->gui->dynamic_param(env->scrollbar_y, 0);
    env->gui->set_dynamic_param(env->canvas_area, 1, param_y1);
    env->ev->mod_api.update_lines_from_canvas(env->canvas, env->mod, 0);
    
    if (env->editor_visible)
      {
	int diag_cancel = env->gui->chosen_item(env->dialog_cancel);
	if (diag_cancel==0)
	  {
	    env->editor_visible = false;
	  }
	int diag_ok = env->gui->chosen_item(env->dialog_ok);
	if (diag_ok==0)
	  {
	    env->editor_visible = false;
	  }
      }
    int sel = env->gui->chosen_item(env->scroll_area);
    if (sel != -1 && e.button==0 && !env->insert_ongoing)
      {
      std::cout << "Scroll_area: " << sel << std::endl;
      W w = env->gui->get_child(env->array, sel);
      int sel2 = env->gui->chosen_item(w);
      std::cout << "Chosen: " << sel2 << std::endl;
      if (sel2>0)
	{
	  std::string name = env->gui->bitmapapi_functions_item_label(sel2-1);
	  std::cout << "Chosen label: " << name << std::endl;
	  env->chosen_item = env->ev->mod_api.inserted_widget(*env->gui, env->mod, 0, env->font, name);
	  env->insert_widget = env->gui->insert_widget(env->chosen_item, std::bind(&callback_func, _1, _2, env));
	  env->insert_ongoing = true;
	}
      }
    if (env->insert_ongoing && e.button == -1)
      {
	env->insert_ongoing2 = true;
      }
    if (env->insert_ongoing2 && e.button==0)
      {
	env->insert_ongoing = false;
	env->insert_ongoing2 = false;
      }
    
    int selected_item = env->gui->chosen_item(env->txt);
    int selected_item2 = -1;
    if (selected_item != -1)
      {
	env->opened_menu_num = selected_item;
      }
    if (env->opened_menu_num != -1)
      {
	env->gui->update(env->menus[env->opened_menu_num], e.cursor_pos, e.button, e.ch);
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
float f(float w)
{
  return cos(w);
}

int main(int argc, char *argv[]) {
  Env e;
  EveryApi ev(e);
  
  WM mod = ev.mod_api.load("mod.txt");

  Envi env;

  int screen_x = 800;
  int screen_y = 600;
  if(argc==2)
    {
      if (std::string(argv[1])=="--huge")
	{
	  screen_x = 1200;
	  screen_y = 900;
	}
    }


  // initialize window
  ev.mainloop_api.init_window(screen_x,screen_y);

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.texture_shader();
  Ft font = ev.font_api.newfont("FreeSans.ttf", 13,15);
  Ft font2 = ev.font_api.newfont("FreeSans.ttf", 10,13);
  Ft font3 = ev.font_api.newfont("FreeSans.ttf", 40,40);

  // rest of the initializations
  ev.mainloop_api.init(sh, screen_x,screen_y);
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
			 

  //std::vector<std::string> vec3;
  //vec3.push_back("newbitmap");
  //vec3.push_back("newbitmap");
  //W test1 = gui.list_item_opened(100,"BitmapApi", font, vec3, font);
  //gui.set_pos(test1, 400,400);

  std::vector<W> items;
  for(int i=0;i<2;i++)
    {
      items.push_back(gui.bitmapapi_functions_list_item(font, font2));
    }
  W array = gui.array_y(&items[0], items.size(), 15);
  W scroll_area = gui.scroll_area(array, gui.size_x(array), screen_y-30, screen_y);

  W txt2 = gui.scrollbar_y(20, screen_y-30, gui.size_y(array));
  gui.set_pos(txt2, gui.size_x(scroll_area), 30);

  gui.set_pos(scroll_area, 0.0, 30.0);

  //W wave = gui.waveform(f, 0.0, 3.14159*2.0, -1.5, 1.5, 200, 100, 0xffffffff, 0x00000000);

  std::vector<std::string> params;
  params.push_back("BM");
  params.push_back("BM");
  //W gameapi = gui.canvas_item_gameapi_node(100,100, "subbitmap", params, "BM", font2);
  // W gameapi_2 = gui.mouse_move(gameapi, 0,0,gui.size_x(gameapi), gui.size_y(gameapi));
  //gui.set_pos(gameapi_2, 200.0, 200.0);

  int sx=1638, sy = 1638;
  int screen2_x = screen_x-140, screen2_y = screen_y-30;

  W canvas = gui.canvas(16384,16384);
  //gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), 100,100);
  //gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), 150,100);
  //for(int i=0;i<200;i++)
  //  gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), i*30, i*30);
  ev.mod_api.insert_to_canvas(gui, canvas, mod, 0, font);

  W canvas_area = gui.scroll_area(canvas, screen2_x-20, screen2_y-20, screen_y);
  W scrollbar_y = gui.scrollbar_y(20, screen2_y-20, sy);
  W scrollbar_x = gui.scrollbar_x(screen2_x-20, 20, sx); 
  gui.set_pos(canvas_area, 140, 30);
  gui.set_pos(scrollbar_x, 140, screen_y-20);
  gui.set_pos(scrollbar_y, screen_x-20, 30);
  
  env.x = 0.0;
  env.y = 0.0;
  env.z = 0.0;
  env.target_string = "abcde";


  env.gui = &gui;
  env.ev = &ev;
  env.txt = txt;
  env.txt2 = txt2;
  env.font = font;
  env.font3 = font3;
  //env.editor = editor;
  env.editor_visible = false;
  //env.wave = wave;
  //env.test1 = test1;
  env.array = array;
  env.insert_ongoing = false;
  env.mod = mod;
  env.canvas = canvas;
  env.canvas_area = canvas_area;
  env.scrollbar_y = scrollbar_y;
  env.scrollbar_x = scrollbar_x;
  //env.gameapi = gameapi_2;
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
