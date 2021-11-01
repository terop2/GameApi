#include "GameApi.hh"
#include <sstream>
#include <math.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cassert>
#include <ctime>
#ifdef WINDOWS
#include <windows.h>
#include <DbgHelp.h>
#else
#include <dlfcn.h>
#endif
#include <fstream>
#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
using namespace GameApi;
#ifndef WINDOWS
#define WAYLAND 1
#endif

#ifdef WAYLAND
#include <wayland-client.h>
#endif

#ifdef LINUX
extern "C" void _udev_device_get_action() { }
#endif

extern std::vector<std::string> g_registered_urls;
extern int g_event_screen_x;
extern int g_event_screen_y;
extern int g_display_width;
extern int g_display_height;
extern int g_window_pos_x;
extern int g_window_pos_y;
extern std::string gameapi_homepageurl;
extern int g_has_wayland;

struct wl_display;
struct wl_surface;
struct wl_shell_surface;

extern wl_display *g_wl_display; 
extern wl_surface *g_wl_surface;
extern wl_shell_surface *g_wl_shell_surface;


void InstallProgress(int num, std::string label, int max=15);
void ProgressBar(int num, int val, int max, std::string label);
void set_codegen_values(GameApi::WM mod2, int id, std::string line_uid, int level);
void pthread_system(std::string str);
IMPORT std::string find_html2(GameApi::HML ml, GameApi::Env &env);
std::vector<unsigned char> load_from_url(std::string url);

std::string hexify2(std::string s)
{
  std::string res;
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      unsigned char c = s[i];
      unsigned char c2 = c & 0xf;
      unsigned char c3 = c & 0xf0;
      c3>>=4;
      const char *chrs = "0123456789ABCDEF";
      res+=chrs[c3];
      res+=chrs[c2];
    }
  return res;
}

std::string unhexify2(std::string s)
{
  std::string res;
  int ss = s.size();
  for(int i=0;i<ss;i+=2)
    {
      int val = -1;
      int val2 = -1;
      char c1 = s[i];
      char c2 = s[i+1];
      const char *chrs = "0123456789ABCDEF";
      for(int i1=0;i1<16;i1++)
	{
	  if (c1==chrs[i1]) { val=i1; }
	  if (c2==chrs[i1]) { val2=i1; }
	}
      res+=char((val<<4) + val2);
    }
  return res;
}


// Note, this is copied to each dll too.
class Item
{
public:
  virtual std::string Name() const=0;
  virtual int ParamCount() const=0;
  virtual std::string ParamName(int p) const=0;
  virtual std::string ParamType(int p) const=0;
  virtual std::string ParamDefault(int p) const=0;
  virtual std::string ReturnType() const=0;
  //virtual std::string ApiName(int i) const=0;
  //virtual std::string FuncName(int i) const=0;
  virtual std::string Symbols() const=0;
  //virtual std::string Comment() const=0;
  virtual int Execute(std::vector<std::string> params)=0;
  virtual std::pair<std::string, std::string> CodeGen(std::vector<std::string> params, std::vector<std::string> param_names)=0;
};

struct GameApiLine;

struct GameApiParam
{
  std::string param_name;
  std::string value;
  bool is_array = false;
  GameApiLine *array_return_target = 0;
};


class GameApiItem
{
public:
  virtual int Count() const=0;
  virtual std::string Name(int i) const=0;
  virtual int ParamCount(int i) const=0;
  virtual std::string ParamName(int i, int p) const=0;
  virtual std::string ParamType(int i, int p) const=0;
  virtual std::string ParamDefault(int i, int p) const=0;
  virtual std::string ReturnType(int i) const=0;
  virtual std::string ApiName(int i) const=0;
  virtual std::string FuncName(int i) const=0;
  virtual std::string Symbols() const=0;
  virtual std::string Comment() const=0;
  virtual int Execute(GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e, int j)=0;
  virtual std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)=0;
  virtual void BeginEnv(GameApi::ExecuteEnv &e, std::vector<GameApiParam> params) { }
  virtual void EndEnv(GameApi::ExecuteEnv &e) { }
};

extern std::vector<GameApiItem*> global_functions;

struct DllData
{
public:
  std::string (*api_name)();
  std::vector<Item*> (*functions)();
  int (*num_displays)();
  void (*display)(int i, int disp);
  std::string (*type_symbol)();
};
const int num_alternatives = 2;
const char *k_api_name[] = { "_Z8api_namev", "_Z8api_nameB5cxx11v" };
const char *k_functions[] = { "_Z9functionsv", "_Z9functionsv" };
const char *k_num_displays[] = { "_Z12num_displaysv", "_Z12num_displaysv" };
const char *k_display[] = { "_Z7displayii", "_Z7displayii" };
const char *k_type_symbol[] = { "_Z11type_symbolv", "_Z11type_symbolB5cxx11v" };
void load_library(DllData &data, std::string lib_name)
{
#ifdef WINDOWS
  const char *lib = lib_name.c_str();
  HMODULE mod = LoadLibrary(lib);
  if (mod==NULL) {
    std::cout << "ERROR: LoadLibrary: " << GetLastError() << std::endl;
  }
  //std::cout << "HMODULE: " << mod << std::endl;
  FARPROC api=0,func=0,num=0,disp=0,type=0;
  for(int i=0;i<num_alternatives;i++)
    {
      if (!api)
      api = GetProcAddress( mod, k_api_name[i] );
      if (!func)
      func = GetProcAddress( mod, k_functions[i] );
      if (!num)
      num = GetProcAddress( mod, k_num_displays[i] );
      if (!disp)
      disp = GetProcAddress( mod, k_display[i] );
      if (!type)
      type = GetProcAddress( mod, k_type_symbol[i] );
    }
#else
  const char *lib = lib_name.c_str();
  void *handle = dlopen(lib, RTLD_NOW);
  void *api=0,*func=0, *num=0, *disp=0,*type=0;
  for(int i=0;i<num_alternatives;i++)
    {
      if (!api)
	api = dlsym(handle, k_api_name[i]);
      if (!func)
	func = dlsym(handle, k_functions[i]);
      if (!num)
	num = dlsym(handle, k_num_displays[i]);
      if (!disp)
	disp = dlsym(handle, k_display[i]);
      if (!type)
	type = dlsym(handle, k_type_symbol[i] );
    }
  //std::cout << api << " " << func << " "<< num << " "<< disp << " "<< type << std::endl;
  
#endif
  //std::cout << "ApiNameFar: " << api << std::endl;

  data.api_name = (std::string (*)()) api;
  data.functions = (std::vector<Item*> (*)()) func;
  data.num_displays = (int (*)()) num;
  data.display = (void (*)(int,int)) disp;
  data.type_symbol = (std::string (*)()) type;
  // std::cout << "ApiName: " << (void*)data.api_name << std::endl;
  //std::cout << "Functions: " << (void*)data.functions << std::endl;
  //std::cout << "NumDisplays: " << (void*)data.num_displays << std::endl;
  //std::cout << "Display: " << (void*)data.display << std::endl;
}
std::vector<DllData> load_dlls(std::string filename)
{
  std::vector<DllData> vec;
  std::ifstream ss(filename.c_str());
  std::string s;
  while(ss >> s)
    {
      //std::cout << "Loading library: " << s << std::endl;
      DllData d;
      load_library(d, s);
      vec.push_back(d);
    }
  return vec;
}


std::vector<std::string> find_additional_urls(GameApi::Env &e, GameApi::EveryApi &ev, std::string url);

struct Envi {
  Env *env;
  EveryApi *ev;
  GuiApi *gui;
  std::string target_string;
  float x,y,z;
  std::vector<GuiApi::EditTypes> edit_data;
  W editor;
  bool editor_visible;
  W display;
  W mem;
  bool display_visible;
  W txt;
  W txt2;
  W scroll_area;
  W wave;
  W array;
  W chosen_item;
  W insert_widget;
  W connect_line;
  W connect_widget;
  W dialog_cancel, dialog_ok;
  W display_close;
  W codegen_button;
  W collect_button;
  W line;
  std::string codegen_uid;

  W list_tooltips;
  std::vector<W> connect_clicks;
  std::vector<W> connect_targets;
  std::vector<W> connect_links;
  std::string connect_start_uid;
  int connect_start_j;
  int connect_start_sz;

  std::vector<W> display_clicks;
  std::vector<W> edit_clicks;
  std::vector<W> delete_key;
  std::vector<W> codegen_button_vec;
  std::vector<W> popup_open;
  std::vector<W> popup_selections;
  W popup_dialog;
  bool popup_visible = false;
  int dialog_i1;
  std::string dialog_uid;
  std::vector<GuiApi::EditTypes*> vec4;
  Ft font;
  Ft font3;
  FtA atlas;
  FtA atlas2;
  FtA atlas3;
  BM atlas_bm;
  BM atlas_bm2;
  BM atlas_bm3;
  bool insert_ongoing;
  bool insert_ongoing2;

  bool connect_ongoing;
  bool connect_ongoing2;
  bool flip_ongoing;
  bool key_state;
  std::map<int, bool> flip_status;
  std::string insert_func_name;
  std::string insert_mod_name;
  // gameapi;
  //W test1;
  WM mod;
  W canvas, canvas_area, scrollbar_x, scrollbar_y;
  W window_decoration;
  std::vector<W> menus;
  int opened_menu_num;
  bool state;
  bool ctrl = false;
  int unique_id_counter;
  SH sh2;
  SH sh;
  SH sh3;
  SH sh_arr;
  SH sh_2d;
  int screen_size_x, screen_size_y;

  std::string filename;
  std::vector<DllData> dlls;
  bool logo_shown = false;
  bool has_wayland = false;
  int extra_left, extra_top;
  int old_window_pos_x=0, old_window_pos_y=0;
  int screen_x, screen_y;
};
void add_to_canvas(GuiApi &gui, W canvas, W item)
{
  gui.canvas_item(canvas, item, 0, 0);
}
void add_to_canvas(GuiApi &gui, W canvas, std::vector<W> items)
{
  int s = items.size();
  for(int i=0;i<s;i++)
    {
      add_to_canvas(gui,canvas, items[i]);
    }
}
void print_connect_targets(std::string ss, Envi *envi)
{
  std::cout << ss << std::endl;
  int s = envi->connect_targets.size();
  for(int i=0;i<s;i++)
    {
      W wid = envi->connect_targets[i];
      if (wid.id!=0) {
	  int j = envi->gui->get_index(wid);
	  int sz = envi->gui->get_size2(wid);
	  std::cout << "J:" << j << " " << sz << std::endl;
      }
    }
}
void connect_target(int x, int y, Envi *envi)
{
  //std::cout << "Connect target!" << std::endl;

  int s = envi->connect_targets.size();
  for(int i=0;i<s;i++)
    {
      W wid = envi->connect_targets[i];
      int xx = envi->gui->pos_x(wid);
      int yy = envi->gui->pos_y(wid);
      int sx = envi->gui->size_x(wid);
      int sy = envi->gui->size_y(wid);

      if (x>=xx && x<xx+sx && y>=yy && y<yy+sy)
	{
	  std::string uidstring = envi->gui->get_id(wid);
	  //int j = envi->gui->get_index(wid);
	  //int sz = envi->gui->get_size2(wid);
	  //std::cout << "UID: " << uidstring << std::endl;

	  
	  std::stringstream ss(uidstring);
	  std::string uid;
	  ss >> uid;
	  int num;
	  ss >> num;
	  
	  std::string funcname = envi->ev->mod_api.get_funcname(envi->mod, 0, uid);
	  std::vector<int> vec = envi->ev->mod_api.indexes_from_funcname(funcname);
	  int real_index = vec[num];
	  //std::cout << "Real index: " << real_index << std::endl;
	  bool is_array = false;
	  bool is_array_return = false; // TODO
	  bool b = envi->ev->mod_api.typecheck(envi->mod, 0, envi->connect_start_uid, uid, real_index, envi->connect_start_j, is_array, is_array_return);
	  bool erase = false;
	  if (b) 
	    {
	      if (envi->connect_start_sz>1)
		{ // multiple return values
		  std::string val2 = envi->connect_start_uid; 
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, val2, envi->connect_start_j);
		  envi->ev->mod_api.change_param_multiple_return_values(envi->mod, 0, val2, -1 /*envi->connect_start_j*/, envi->connect_start_sz);
		  erase=true;
		  
		}
	      else
	      if (is_array_return)
		{
		  std::string val2 = envi->connect_start_uid; 
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, val2,0);
		  int ii = envi->ev->mod_api.find_line_index(envi->mod, 0, val2);
		  envi->ev->mod_api.change_param_is_array(envi->mod, 0, uid, real_index, true, ii);
		  erase = true;
		}
	      else
	      if (is_array)
		{
		  std::string val = envi->ev->mod_api.param_value(envi->mod, 0, uid, real_index);
		  std::vector<std::string> vec = envi->ev->mod_api.parse_param_array(val);
		  vec.push_back(envi->connect_start_uid);
		  std::string val2 = envi->ev->mod_api.generate_param_array(vec);
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, val2,0);
		  erase=false;
		}
	      else
		{
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, envi->connect_start_uid,0);
		  erase = true;
		}
	      if (erase)
		{
		  int sk = envi->connect_links.size();
		  for(int i=0;i<sk;i++)
		    {
		      W wid = envi->connect_links[i];
		      std::string str = envi->gui->get_id(wid);
		      std::stringstream ss(str);
		      std::string uid1;
		      std::string uid2;
		      int val;
		      ss>> uid1 >> uid2 >> val;
		      if (uid2==uid && val == real_index)
			{
			  envi->connect_links.erase(envi->connect_links.begin()+i);
			  int index = envi->gui->canvas_item_index(envi->canvas, wid);
			  if (index != -1)
			    envi->gui->del_canvas_item(envi->canvas, index);
			  break;
			}
		    }
		
		} 
	    
	  W start_link = envi->gui->find_canvas_item(envi->canvas, envi->connect_start_uid);
	      if (start_link.id!=-1)
		{
		  int j = envi->connect_start_j;
		  int sz = envi->connect_start_sz;
		  W link = envi->gui->line( start_link, envi->gui->size_x(start_link),/*(envi->gui->size_y(start_link)-16)+*/16+16+5+4+(envi->gui->size_y(start_link)-16-16)*j/sz,
					    wid, 0,10, envi->sh2, envi->sh);
		  std::stringstream ss2;
		  ss2 << envi->connect_start_uid << " " << uid << " " << real_index; 
		  envi->gui->set_id(link, ss2.str());
		  envi->gui->set_index(link, j);
		  envi->gui->set_size2(link, sz);
		  envi->connect_links.push_back(link);
		  add_to_canvas(*envi->gui, envi->canvas, link);
		}

	    } else
	    {
	      std::cout << "TypeCheck failed!" << std::endl;
	    }
	}
      
    }

}
void callback_func(int x, int y, Envi *envi)
{
  //std::cout << "Insert!" << std::endl;

  int uid_num = envi->unique_id_counter;
  envi->unique_id_counter++;
  std::stringstream ss;
  ss << "uid" << uid_num;
  std::string uid = ss.str();

  W display_cb;
  W edit_cb;
  W delete_key_cb;
  W codegen_button;
  W popup_open;
  envi->ev->mod_api.insert_inserted_to_canvas(*envi->gui, envi->canvas, envi->chosen_item, uid, display_cb, edit_cb, delete_key_cb, codegen_button, popup_open);
  envi->display_clicks.push_back(display_cb);
  envi->edit_clicks.push_back(edit_cb);
  envi->delete_key.push_back(delete_key_cb);
  envi->codegen_button_vec.push_back(codegen_button);
  envi->popup_open.push_back(popup_open);


  std::vector<std::pair<std::string,std::string> > vec = envi->ev->mod_api.defaults_from_function(envi->insert_mod_name);
  std::vector<GameApi::WModApi::InsertParam> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      GameApi::WModApi::InsertParam pp;
      pp.first = vec[i].first;
      pp.second = vec[i].second;
      pp.is_array = false; // will be filled later
      pp.line_index_in_gameapi_function_lines_array = -1; // will be filled later
      //std::cout << "Insert_To_mod: " << pp.first << " " << pp.second << std::endl;
      vec2.push_back(pp);
    }
  envi->ev->mod_api.insert_to_mod(envi->mod, 0, envi->insert_mod_name, uid, false, envi->gui->pos_x(envi->chosen_item), envi->gui->pos_y(envi->chosen_item), vec2);
}

template<class T>
std::ostream &operator<<(std::ostream &o, const std::vector<T> &vec)
{
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      o << vec[i] << ",";
    }
  return o;
}

struct ASyncData
{
  std::string api_name;
  std::string func_name;
  int param_num;
};

extern ASyncData *g_async_ptr;
extern int g_async_count;
extern ASyncData *g_async_ptr2;
extern int g_async_count2;


void save_dd(GameApi::Env &e, GameApi::EveryApi &ev, std::string filename, std::string script, std::vector<std::string> urls);

void FinishProgress();
void iter(void *arg)
{
  Envi *env = (Envi*)arg;


#if 0
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; }
      return;
    }
#endif

  env->ev->shader_api.use(env->sh);

  if (env->has_wayland) {
    env->ev->mainloop_api.clear(0x00000000);
  } else {
    env->ev->mainloop_api.clear(0xff000000);
  }

  if (env->old_window_pos_x != g_window_pos_x || env->old_window_pos_y != g_window_pos_y) {
    int x = g_window_pos_x;
    int y = g_window_pos_y;
    
    env->gui->set_pos(env->window_decoration, x,y);
    env->gui->set_pos(env->line, x+140-5+env->extra_left,y+env->extra_top);
    env->gui->set_pos(env->canvas_area, x+140+env->extra_left, y+env->extra_top);
    env->gui->set_pos(env->scrollbar_x, x+140+env->extra_left, y+env->extra_top+env->screen_y-20);
    env->gui->set_pos(env->scroll_area, x+env->extra_left, y+env->extra_top);
    
#ifdef WAYLAND
    //wl_surface_damage_buffer(g_wl_surface, 0,0,g_display_width, g_display_height);
    //wl_shell_surface_set_transient(g_wl_shell_surface, g_wl_surface, x,y, 0);
    //wl_surface_commit(g_wl_surface);
#endif
  }
  
    //env->ev->mainloop_api.clear_3d();
    if (env->has_wayland) {
      env->gui->render(env->window_decoration);
    }
    
    //env->gui->render(env->txt2);
    env->gui->render(env->line);
    //env->gui->render(env->txt);
    env->gui->render(env->scroll_area);
    //env->gui->render(env->wave);
    //env->gui->render(env->gameapi);
    //env->gui->render(env->test1);
    env->gui->render(env->canvas_area);
    env->gui->render(env->scrollbar_x);
    //env->gui->render(env->scrollbar_y);
    env->gui->render(env->list_tooltips);
    if (env->popup_visible)
      {
	env->gui->render(env->popup_dialog);
      }
    if (env->insert_ongoing)
      {
	env->gui->render(env->insert_widget);
      }
    if (env->connect_ongoing)
      {
	env->gui->render(env->connect_line);
	env->ev->shader_api.use(env->sh);
	env->gui->render(env->connect_widget);
      }
    //if (env->opened_menu_num != -1)
    //  {
	//std::cout << env->opened_menu_num << std::endl;
    //	env->gui->render(env->menus[env->opened_menu_num]);
    //  }
    //int s5 = env->connect_links.size();
    //for(int i5 = 0;i5<s5;i5++)
    //  {
    //	W wid = env->connect_links[i5];
    //	env->gui->render(wid);
    //	env->ev->shader_api.use(env->sh);
    // }
    if (env->display_visible)
      {
	env->gui->render(env->display);
      }
    if (env->editor_visible)
      env->gui->render(env->editor);


    
    
    //env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    FinishProgress();
	bool properties_button = false;
	bool codegen_button = false;
	bool pkggen_button = false;
	bool display_button = false;
	std::string popup_uid;

    // handle esc event
    MainLoopApi::Event e;
    e.last = true;
    while((e=env->ev->mainloop_api.get_event()).last)
      {
	if (e.type==256) { exit(0); }
	if (e.type==0x200) {
	    int sx = g_event_screen_x;
	    int sy = g_event_screen_y;
	    if (sx!=-1 && sy != -1) {
	    W w = env->canvas_area;
	    //GuiWidget *ww = find_widget(*env->env, w);
	    //Vector2d sz = { sx-140,sy-30 };
	    //ww->set_size(sz);
	    //env->gui->set_size(w, sx-140, sy-30);
	    //env->gui->set_pos(w, 140, 30);
	    //env->gui->set_pos(env->scrollbar_x, 140, sy-20);

	    }
	}
	//std::cout << e.type << " " << e.ch << " " << e.button << std::endl;
	//if (e.type==1024 && e.button==-1) continue;
	//if (e.type==0x300)
	// std::cout << e.type << " " << e.ch << std::endl;

	if (e.type==1024 && e.button==-1)
	  {
	    env->key_state = true;
	  }
	if (e.type==1024 && e.button==0 && env->key_state==true)
	  {
	    // FIX EMSCRIPTEN EENTS NOT GIVING KEYDOWN EVENTS.
	    e.type=1025;
	    env->key_state = false;
	  }
	
	
	//std::cout << e.type << " " << e.ch << " " << e.button << std::endl;
	//std::cout << e.type << std::endl;
	//std::cout << e.button << std::endl;
	if (e.ch==1073742048 && e.type == 0x300)
	  {
	    env->ctrl = true;
	  }
	if (e.ch==1073742048 && e.type == 0x301)
	  {
	    env->ctrl = false;
	  }
	if (env->ctrl && e.ch==115 && e.type==0x300)
	  { // Save.
	    std::cout << "Saving... " << env->filename << std::endl;
	env->ev->mod_api.save(env->mod, env->filename);
	
	  }
	
	//std::cout << e.ch << std::endl;
#ifndef EMSCRIPTEN
	//if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
	if (e.type != 0x300 && e.type != 0x301)
	  {
	    e.ch=-1;
	  }
	if (env->display_visible)
	  {
	    int chosen = env->gui->chosen_item(env->display_close);
	    if (chosen==0)
	      {
		env->display_visible = false;
	      }
	    int chosen2 = env->gui->chosen_item(env->codegen_button);
	    if (chosen2 == 0)
	      {
		/* code generation here */
		std::cout << "CodeGen" << std::endl;
		env->ev->mod_api.codegen_reset_counter();
		std::pair<std::string, std::string> p = env->ev->mod_api.codegen(*env->ev, env->mod, 0, env->codegen_uid,1000,0);
		std::cout << p.second << std::endl;
	      }
	    int chosen3 = env->gui->chosen_item(env->collect_button);
	    if (chosen3 == 0)
	      {
#if 0
		/* collect here */
		//std::cout << "Collect" << std::endl;
		GameApi::collect_counter(0);
		CollectResult res = env->ev->mod_api.collect_nodes(*env->ev, env->mod, 0, env->codegen_uid,100);
		// TODO.
#endif
	      }
	    
	  }
	int s = env->delete_key.size();
	for(int i=0;i<s;i++)
	  {
	    W w = env->delete_key[i];
	    int val = env->gui->chosen_item(w);
	    if (val==0)
	      {
		std::string uid = env->gui->get_id(w);
		//std::cout << "Delete" << i << ":" << uid << std::endl;
		env->ev->mod_api.delete_by_uid(env->mod, 0, uid);

		int ss1 = env->connect_clicks.size();
		for(int i=0;i<ss1;i++)
		  {
		    W w = env->connect_clicks[i];
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    if (id==uid)
		      {
			env->connect_clicks.erase(env->connect_clicks.begin()+i);
			ss1--;
			i--;
		      }
		  }
		int ss2 = env->connect_targets.size();
		for(int i=0;i<ss2;i++)
		  {
		    W w = env->connect_targets[i];
		    std::string id = env->gui->get_id(w);
		    
		    //std::cout << id << std::endl;
		    std::stringstream ss(id);
		    std::string id1;
		    ss >> id1;
		    if (id1==uid)
		      {
			env->connect_targets.erase(env->connect_targets.begin()+i);
			i--;
			ss2--;
		      }
		  }
		int ss3 = env->connect_links.size();
		for(int i=0;i<ss3;i++)
		  {
		    W w = env->connect_links[i];
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    std::stringstream ss(id);
		    std::string id1, id2;
		    ss >> id1 >> id2;
		    if (id1==uid ||id2==uid)
		      {
			env->connect_links.erase(env->connect_links.begin()+i);
			i--;
			ss3--;
		      }
		  }
		int ss4 = env->display_clicks.size();
		for(int i=0;i<ss4;i++)
		  {
		    W w = env->display_clicks[i];
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    if (id==uid)
		      {
			env->display_clicks.erase(env->display_clicks.begin()+i);
			break;
		      }
		  }
		int ss5 = env->edit_clicks.size();
		for(int i=0;i<ss5;i++)
		  {
		    W w = env->edit_clicks[i];
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    if (id==uid)
		      {
			env->edit_clicks.erase(env->edit_clicks.begin()+i);
			break;
		      }

		  }
		int ss6 = env->delete_key.size();
		for(int i=0;i<ss6;i++)
		  {
		    W w = env->delete_key[i];
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    if (id==uid)
		      {
			env->delete_key.erase(env->delete_key.begin()+i);
			break;
		      }
		  }
		int ss7 = env->codegen_button_vec.size();
		for(int i=0;i<ss7;i++)
		  {
		    W w = env->codegen_button_vec[i];
		    std::string id = env->gui->get_id(w);
		    if (id==uid)
		      {
			env->codegen_button_vec.erase(env->codegen_button_vec.begin()+i);
			break;
		      }
		  }
		int ss8 = env->popup_open.size();
		for(int i=0;i<ss8;i++)
		  {
		    W w = env->popup_open[i];
		    std::string id = env->gui->get_id(w);
		    if (id==uid)
		      {
			env->popup_open.erase(env->popup_open.begin()+i);
			break;
		      }
		  }
		W item = env->gui->find_canvas_item(env->canvas, uid);
		int index = env->gui->canvas_item_index(env->canvas, item);
		env->gui->del_canvas_item(env->canvas, index);
		
		int s = env->gui->num_childs(env->canvas);
		for(int i=0;i<s;i++)
		  {
		    W w = env->gui->get_child(env->canvas, i);
		    std::string id = env->gui->get_id(w);
		    //std::cout << id << std::endl;
		    std::stringstream ss(id);
		    std::string uid1, uid2;
		    ss >> uid1 >> uid2;
		    if (uid1==uid||uid2==uid)
		      {
			env->gui->del_canvas_item(env->canvas, i);
			i--;
			s--;
		      }
		  }

	      }
	  }

	{
	  int s = env->popup_selections.size();
	  for(int i=0;i<s;i++)
	    {
	      W w = env->popup_selections[i];
	      int chosen = env->gui->chosen_item(w);
	      if (chosen==0)
		{
		  //std::cout << "popup selection!" << std::endl;
		  env->popup_visible = false;
		  std::string uid = env->gui->get_id(w);
		  popup_uid = uid;
		  switch(i) {
		  default:
		  case 0: properties_button = true; break;
		  case 1: codegen_button = true; break;
		  case 2: pkggen_button = true; break;
		  case 3: display_button = true; break;
		  };
		  env->popup_selections = std::vector<W>();
		  break;
		}
	    }
	}

	{
	  int s = env->popup_open.size();
	  for(int i=0;i<s;i++)
	    {
	      W w = env->popup_open[i];
	      int chosen = env->gui->chosen_item(w);
	      if (chosen==0)
		{
		  //std::cout << "popup open!" << std::endl;
		  std::string uid = env->gui->get_id(w);

		  PT pos = e.cursor_pos;
		  float x = env->ev->point_api.pt_x(pos);
		  float y = env->ev->point_api.pt_y(pos);

		  std::vector<std::string> labels;
		  labels.push_back("Properties");
		  labels.push_back("CodeGen");
		  labels.push_back("Generate Pkg");
		  labels.push_back("Display");
		  
		  W w = env->gui->popup_menu(int(x), int(y), labels, env->atlas, env->atlas_bm, env->popup_selections);
		  env->popup_dialog = w;
		  env->popup_visible = true;
		  int s = env->popup_selections.size();
		  for(int i=0;i<s;i++)
		    {
		      W w = env->popup_selections[i];
		      env->gui->set_id(w, uid);
		    }

		}
	    }
	}

	if (codegen_button==true)
	{
	  codegen_button=false;
	  std::string uid = popup_uid;
	  //int s = env->codegen_button_vec.size();
	  //for(int i=0;i<s;i++)
	  //  {
	  //    W w = env->codegen_button_vec[i];
	  //    std::string uid = env->gui->get_id(w);
	  //int chosen = env->gui->chosen_item(w);
	  //  if (chosen==0)
		{
		  std::cout << "CodeGen!" << std::endl;
		  std::pair<std::string, std::string> p = env->ev->mod_api.codegen(*env->ev, env->mod, 0, uid,1000,0);
		std::cout << p.second << std::endl;

		}
	      //}
	}
	if (!env->display_visible && pkggen_button)
	  {
	    pkggen_button = false;
	      std::string uid = popup_uid;

	    std::string type2 = env->ev->mod_api.return_type(env->mod, 0, uid);
	    //ProgressBar(933, 7,15, "Execute");
	    GameApi::ExecuteEnv exeenv;
	    if (type2 == "ML") {
	      std::pair<int,std::vector<std::string> > ids = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr, g_async_count);
	      std::pair<int,std::vector<std::string> > ids2 = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr2, g_async_count2);
	      
	      

	      //ProgressBar(933, 15,15, "Execute");

	      //std::cout << "URLS:" << ids.second << std::endl;
	      std::vector<std::string> urls = ids.second;
	      urls.insert(urls.begin(),ids2.second.begin(),ids2.second.end());
	      std::sort(urls.begin(),urls.end());
	      auto last = std::unique(urls.begin(),urls.end());
	      urls.erase(last,urls.end());
	      int s4 = urls.size();
	      for(int i=0;i<s4;i++) {
		std::vector<std::string> vec = find_additional_urls(*env->env,*env->ev, urls[i]);
		int s2 = vec.size();
		for(int j=0;j<s2;j++) urls.push_back(vec[j]);
	      }
	      std::sort(urls.begin(),urls.end());
	      auto last2 = std::unique(urls.begin(),urls.end());
	      urls.erase(last2,urls.end());
	      //env->env->async_load_all_urls(urls, gameapi_homepageurl);

	      int s3 = urls.size();
	      for(int i=0;i<s3;i++) {
		std::cout << "Loading: " << urls[i] << std::endl;
	      }
	      
	      std::pair<std::string, std::string> p = env->ev->mod_api.codegen(*env->ev, env->mod, 0, uid,1000,0);

		  
		  
	      
	    std::string script = p.second;
	    std::cout << "Saving game.pkg" << std::endl;
	    save_dd(*env->env, *env->ev, "game.pkg", script, urls);
	    std::cout << "Saving game.pkg complete" << std::endl;
	    std::cout << "Saving game_1.pkg complete" << std::endl;
	    std::cout << "Saving game_2.pkg complete" << std::endl;
	    std::cout << "Saving game_3.pkg complete" << std::endl;
	    std::cout << "Saving game_4.pkg complete" << std::endl;
	    std::cout << "Saving game_5.pkg complete" << std::endl;
	    std::cout << "Saving game_6.pkg complete" << std::endl;

	    std::cout << "Please upload pkg files to web server, then use pkg_window(slower) or pkg_window2(faster) to load it to builder." << std::endl;
	    std::cout << "If you are planning to do web build, use codegen from run_window and copy-paste the script from cmd prompt/shell window to gameapi_example.html file." << std::endl;
	    } else {
	      std::cout << "ERROR: Box return type needs to be ML" << std::endl;
	    }

	  }
	
	if (!env->display_visible && display_button)
	  {
	    display_button = false;
	    {
	      std::string uid = popup_uid;
	    //int s = env->display_clicks.size();
	    //for(int i=0;i<s;i++)
	    //  {
	    //	W w = env->display_clicks[i];
	    //	int chosen = env->gui->chosen_item(w);
	    //	if (!env->display_visible && chosen==0)
	    //	  {
	    //	    std::string uid = env->gui->get_id(w);
		    
	      {
		//std::cout << "Execute for uid: " << uid << std::endl;
		env->env->free_temp_memory();
		    // Execute
		//InstallProgress(933, "Execute", 15);
		//ProgressBar(933, 0,15, "Execute");
		
		    std::string type2 = env->ev->mod_api.return_type(env->mod, 0, uid);
		    //ProgressBar(933, 7,15, "Execute");
		    GameApi::ExecuteEnv exeenv;
		    if (type2 != "HML") {
		    std::pair<int,std::vector<std::string> > ids = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr, g_async_count);
		    //ProgressBar(933, 15,15, "Execute");

		    //std::cout << "URLS:" << ids.second << std::endl;
		    std::vector<std::string> urls = ids.second;
		    std::vector<std::string> urls2 = g_registered_urls;
		    int s = urls2.size();
		    for(int i=0;i<s;i++) urls.push_back(urls2[i]);
		    
		    std::sort(urls.begin(),urls.end());
		    auto last3 = std::unique(urls.begin(),urls.end());
		    urls.erase(last3,urls.end());
		    env->env->async_load_all_urls(urls, gameapi_homepageurl);
		    }
		    //int s = urls.size();
		    //for(int i=0;i<s;i++)
		    //  {
		    //	env->env->async_load_url(urls[i], gameapi_homepageurl);
		    // }

		static int g_id = -1;
		if (g_id!=-1) clear_block(g_id);
		g_id = add_block();
		set_current_block(g_id);
		    set_codegen_values(env->mod,0,uid,1000);
		    
		    int id = env->ev->mod_api.execute(*env->ev, env->mod, 0, uid, exeenv,1000,0); // TODO last 0=wrong
		    set_current_block(-2);

		    if (id==-1) {
		      std::cout << "Execute failed!" << std::endl;
		      break;
		    }
		    env->codegen_uid = uid;

		    // display dialog
		    std::string type = env->ev->mod_api.return_type(env->mod, 0, uid);


		    
		    
		    bool display = true;
		    if (type=="BO")
		      {
			BO p;
			p.id = id;
			P p2 = env->ev->bool_api.to_polygon(p);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->polygon_dialog(p2, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->mem);
		      }
		    else
		    if (type=="VA")
		      {
			VA p;
			p.id = id;
			env->ev->polygon_api.print_stat(p);
			SH sh;
			if (env->ev->polygon_api.is_texture(p))
			  {
			    if (env->ev->polygon_api.is_array_texture(p))
			      {
				sh = env->sh_arr;
			      }
			    else
			      sh=env->sh;
			  }
			else
			  {
			    sh=env->sh3;
			  }
			//std::cout << "ID: " << p.id << std::endl;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->va_dialog(p, sh, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			
		      } else
		      if (type=="VX")
			{
			  GameApi::EveryApi &ev = *env->ev;
			  VX vx;
			  vx.id = id;
			  P I12=ev.polygon_api.cube(-10,10,-10,10,-10,10);
			  P I13=ev.polygon_api.cube(-10,10,-10,10,-10,10);
			  P I14=ev.polygon_api.color(I13,0xffff0000);
			  P I15=ev.polygon_api.cube(-10,10,-10,10,-10,10);
			  P I16=ev.polygon_api.color(I15,0xffffff00);
			  //O I17=ev.volume_api.mandelbrot_volume(false,64,0.0);
			  //VX I18=ev.voxel_api.blit_voxel(I17,30,30,30,-200,200,-200,200,-200,200,-1,2);
			  ARR I19=ev.voxel_api.voxel_instancing(vx,3,-300,300,-300,300,-300,300);
			  MT I20=ev.materials_api.m_def(ev);
			  ML I21=ev.voxel_api.voxel_bind(ev,std::vector<P>{I12,I14,I16},arr_to_pts_arr(ev,I19),I20);
			  ML ml = I21;
			  env->env->free_temp_memory();
			  env->gui->delete_widget(env->mem);
			  env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);


			} else
			if (type=="FML")
			  {
			    FML ml;
			    ml.id = id;
			    
			    FBU I6=env->ev->low_frame_api.low_framebuffer(ml,4,800,600,0);
			    RUN blk=env->ev->low_frame_api.low_framebuffer_run(*env->ev,I6,0,800,600);
			    

			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			int sx = env->ev->mainloop_api.get_screen_sx();
			int sy = env->ev->mainloop_api.get_screen_sy();
			//env->ev->mainloop_api.set_screen_size(1200,900);
			//env->ev->mainloop_api.set_viewport(0,0,1200,900);
			env->ev->blocker_api.run2(*env->ev,blk);
			    //env->env->free_to_counts(v);
			    //env->ev->mainloop_api.set_screen_size(sx,sy);
			    //env->ev->mainloop_api.set_viewport(0,0,sx,sy);
			    env->display = env->gui->empty();
			    env->display_close = env->gui->empty();
			    env->codegen_button = env->gui->empty();
			    env->collect_button = env->gui->empty();
			    env->ev->shader_api.use(env->sh);
			    display = false;

			  } else
		      if (type=="ML")
			{
			  ML ml;
			  ml.id = id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			} else
		      if (type=="MT")
			{
			  MT mat;
			  mat.id = id;
			  PT I1=env->ev->point_api.point(0.0,0.0,0.0);
			  P I2=env->ev->polygon_api.sphere(I1,350,30,30);
			  MT I4=mat;
			  ML I5=env->ev->materials_api.bind(I2,I4);

			  ML ml;
			  ml.id = I5.id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			} else
			if (type=="KF")
			  {
			    KF kf;
			    kf.id = id;
     			  env->env->free_temp_memory();
   			env->gui->delete_widget(env->mem);
			ML ml = env->ev->vertex_anim_api.vertex_anim_render(*env->ev, kf);

			env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			  }
		        else
			if (type=="BLK")
			  {

			    BLK blk;
			    blk.id = id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			//std::vector<int> v = env->env->store_counts();
			int sx = env->ev->mainloop_api.get_screen_sx();
			int sy = env->ev->mainloop_api.get_screen_sy();
			//env->ev->mainloop_api.set_screen_size(1200,900);
			//env->ev->mainloop_api.set_viewport(0,0,1200,900);
			    env->ev->blocker_api.run(blk);
			    //env->env->free_to_counts(v);
			    //env->ev->mainloop_api.set_screen_size(sx,sy);
			    //env->ev->mainloop_api.set_viewport(0,0,sx,sy);
			    env->display = env->gui->empty();
			    env->display_close = env->gui->empty();
			    env->codegen_button = env->gui->empty();
			    env->collect_button = env->gui->empty();
			    env->ev->shader_api.use(env->sh);
			    display = false;

			  }
		        else
			if (type=="RUN")
			  {

			    RUN blk;
			    blk.id = id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			int sx = env->ev->mainloop_api.get_screen_sx();
			int sy = env->ev->mainloop_api.get_screen_sy();
			//env->ev->mainloop_api.set_screen_size(1200,900);
			//env->ev->mainloop_api.set_viewport(0,0,1200,900);
			env->ev->blocker_api.run2(*env->ev,blk);
			    //env->env->free_to_counts(v);
			    //env->ev->mainloop_api.set_screen_size(sx,sy);
			    //env->ev->mainloop_api.set_viewport(0,0,sx,sy);
			    env->display = env->gui->empty();
			    env->display_close = env->gui->empty();
			    env->codegen_button = env->gui->empty();
			    env->collect_button = env->gui->empty();
			    env->ev->shader_api.use(env->sh);
			    display = false;

			  }
		        else
			if (type=="O")
			  {
			    O o;
			    o.id = id;
			    
			    P p = env->ev->volume_api.rendercubes3(o, 255,255,255,
								   -300.0, 300.0,
								   -300.0, 300.0,
								   -300.0, 300.0);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->polygon_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->mem);
			  }
			else
			  if (type=="FD")
			    {
			      FD fd;
			      fd.id = id;
			      PT pt = env->ev->point_api.point(-300.0, -300.0, -300.0);
			      V u_x = env->ev->vector_api.vector(600.0, 0.0, 0.0);
			      V u_y = env->ev->vector_api.vector(0.0, 600.0, 0.0);
			      V u_z = env->ev->vector_api.vector(0.0, 0.0, 600.0);
			      BM bm = env->ev->dist_api.render(fd, pt, u_x, u_y, u_z, 300, 300);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			    }
		    else
		      if (type=="IBM")
			{
			  IBM bm;
			  bm.id = id;
			  BM bm2 = env->ev->bitmap_api.intbitmap_bm(bm);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm2, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			  
			} else
		    if (type=="BM")
		      {
			BM bm;
			bm.id = id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
		      }
		    else if (type=="HML")
		      {
			
			display = false;
			
			HML ml;
			ml.id = id;
			
			env->ev->mod_api.codegen_reset_counter();

			std::string htmlfile = find_html2(ml,*env->env);
#ifdef WINDOWS
			std::string drive = getenv("systemdrive");
			std::string path = getenv("homepath");
			std::string prefix = drive + path + "\\";
			
			std::ofstream f((prefix + "tst.html").c_str());
			f << htmlfile;
			f.close();
			
			pthread_system((std::string("start ") + prefix + "tst.html").c_str());
			
#else
			std::string home = getenv("HOME");
			std::string prefix = home + "/_gameapi_builder";
			system((std::string("mkdir -p ") + prefix).c_str());
			
			prefix+="/";
			std::ofstream f((prefix + "tst.html").c_str());
			f << htmlfile;
			f.close();
			std::string cmd = std::string("chromium --user-data-dir=\"") + prefix + "\" " + prefix + "tst.html";
			std::cout << cmd << std::endl;
			pthread_system(cmd.c_str());
			
#endif
		      }
		    else if (type=="WV")
		      {
			WV wv;
			wv.id = id;
			BM bm = env->ev->waveform_api.waveform_bitmap(wv,300,100,0x0, 0xffffffff);
			//bm.id = id;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
		      } 
		    else if (type=="CBM")
		      {
			CBM cbm;
			cbm.id = id;
			BM bm = env->ev->cont_bitmap_api.sample(cbm, 200,200);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

		      }
		    else if (type=="BB")
		      {
			BB bb;
			bb.id = id;
			BM bm = env->ev->bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			
		      }
		    else if (type=="FB")
		      {
			FB fb;
			fb.id = id;
			
			//BB bb = env->ev->float_bitmap_api.to_bool_mod(fb,100.0);
			//BM bm = env->ev->bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);
			BM bm = env->ev->float_bitmap_api.to_grayscale_color(fb, 255,255,255,255, 0,0,0,0);
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			
		      }
		    else if (type=="P")
		      {
			P p;
			p.id = id;
			env->ev->polygon_api.print_stat(p);
			//std::cout << "ID: " << p.id << std::endl;
			  env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->polygon_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->mem);
		      }
		    else if (type=="MN")
		      {
			MN mn;
			mn.id = id;
			M m = env->ev->move_api.get_matrix(mn, 10.0, 0.01);
			LI p = env->ev->points_api.matrix_display(*env->ev, m);
			env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->lines_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			
		      }
		    else if (type=="LI")
		      {
			LI p;
			p.id = id;
			 env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->lines_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			
		      }
		    else if (type=="C")
		      {
			C p0;
			p0.id = id;
			LI p = env->ev->curve_api.to_lines(p0, 40);
			 env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->lines_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button,env->collect_button);
			
		      }
		    else if (type=="PTS")
		      {
			PTS p;
			p.id = id;
			//std::cout << "PTS NumPoints: " << env->ev->points_api.NumPoints(p) << std::endl;
			//std::cout << "PTS0: " << env->ev->points_api.pos_x(p,0) << " " << env->ev->points_api.pos_y(p,0) << " " << env->ev->points_api.pos_z(p,0) << std::endl;
			env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->pts_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button,env->collect_button);

		      }
		    else if (type=="SFO")
		      {
			SFO p;
			p.id = id;
			env->env->free_temp_memory();
			env->gui->delete_widget(env->mem);
			env->display = env->gui->shader_dialog(p, env->display_close, env->atlas3, env->atlas_bm3, env->screen_size_x, env->screen_size_y, env->codegen_button, env->collect_button);
			
		      }
		    else 
		      {
			display = false;
#if 0
			int s = env->dlls.size();
			bool success = false;
			for(int i=0;i<s;i++)
			  {
			    DllData &d = env->dlls[i];
			    if (d.type_symbol)
			    if (type==(*d.type_symbol)())
			      {
				(*d.display)(id,0);
				success = true;
				break;
			      }
			  }
			if (!success) {
			  std::cout << "Type not found" << type << std::endl;
			}
#endif
		      }
		    if (display)
		      {
			env->gui->set_pos(env->display, 200.0, 50.0);
			env->display_visible = true;
		      }
		  }
	      }
	  }
	
	if (!env->editor_visible)
	  {
	    //	    int s = env->edit_clicks.size(); //env->gui->num_childs(env->canvas);
	    //for(int i=0;i<s;i++)
	    //{
		//std::cout << "Child " << i << std::endl;
	    //	W w = //env->gui->get_child(env->canvas, i);
	    //	  env->edit_clicks[i];
	    //	int chosen = env->gui->chosen_item(w);
	    //	if (!env->editor_visible && chosen == 0)
	    if (properties_button==true)
	    {
	      properties_button = false;
		  {
		    // env->dialog_i1 = i;
		    std::string uid = popup_uid;
		    //std::string uid = env->gui->get_id(w);
		    //std::cout << "Chosen uid: " << uid << std::endl;
		    env->dialog_uid = uid;
		    ///std::vector<GuiApi::EditTypes*> vec4;
		    env->vec4.clear();
		    env->edit_data.clear();
		    std::vector<std::string> types;
		    types = env->ev->mod_api.types_from_function(env->mod, 0, uid);
		    int s = types.size();
		    for(int w=0;w<s;w++)
		      {
			env->edit_data.push_back(GuiApi::EditTypes());
		      }
		    for(int ww = 0;ww<s;ww++)
		      {
			env->vec4.push_back(&env->edit_data[ww]);
		      }
		    
		    std::vector<std::string> labels;
		    labels = env->ev->mod_api.labels_from_function(env->mod, 0, uid);
		    std::vector<std::string*> refs;
		    refs = env->ev->mod_api.refs_from_function(env->mod, 0, uid);
		    
		    //std::cout << labels << " " << refs << std::endl;
		    assert(refs.size()==labels.size());
		    assert(types.size()==labels.size());
		    for(int e=0;e<s;e++)
		      {
			std::string *ref = refs[e];
			env->gui->string_to_generic(*env->vec4[e], types[e], *ref); 
		      }
		    
		    //for(int kk = 0; kk < s; kk++)
		    //     std::cout << env->vec4[kk] << " " << env->vec4[kk]->i_value << std::endl;

		    env->editor = env->gui->edit_dialog(labels,env->vec4,env->atlas3, env->atlas_bm3, types, env->dialog_cancel, env->dialog_ok, env->atlas2, env->atlas_bm2);
		    env->gui->set_pos(env->editor, 200,50);
		    
		    env->editor_visible = true;
		    
		  }
	      }
	  }

	PT cursor_pos = e.cursor_pos;
	
	env->gui->update(env->line, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	//env->gui->update(env->txt, e.cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	if (env->display_visible)
	  {
	    env->gui->update(env->display, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	  }
	if (env->editor_visible)
	  env->gui->update(env->editor, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	if (env->popup_visible)
	  env->gui->update(env->popup_dialog, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	if (e.button == 0 && env->popup_visible) { 
	  PT pos = cursor_pos;
	  int x = env->ev->point_api.pt_x(pos);
	  int y = env->ev->point_api.pt_y(pos);
	  int px = env->gui->pos_x(env->popup_dialog);
	  int py = env->gui->pos_y(env->popup_dialog);
	  int sx = env->gui->size_x(env->popup_dialog);
	  int sy = env->gui->size_y(env->popup_dialog);

	  if (x>=px && y>=py && x<px+sx && y<py+sy)
	    {
	      /* do nothing */
	    }
	  else
	    {
	      env->popup_visible = false; 
	      env->popup_selections = std::vector<W>();
	    }
	}

	//env->gui->update(env->txt2, e.cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	env->gui->update(env->scroll_area, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	//env->gui->update(env->wave, e.cursor_pos, e.button);
	//env->gui->update(env->gameapi, e.cursor_pos, e.button);
	//env->gui->update(env->test1, e.cursor_pos, e.button);
	env->gui->update(env->canvas_area, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	env->gui->update(env->scrollbar_x, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	//env->gui->update(env->scrollbar_y, e.cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	env->gui->update(env->list_tooltips, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	
	//int s4 = env->connect_links.size();
	//for(int i4 = 0;i4<s4;i4++)
	//  {
	//    W wid = env->connect_links[i4];
	//    env->gui->update(wid, e.cursor_pos, e.button, e.ch, e.type);
	//  }
	
	
	if (env->insert_ongoing)
	  {
	    env->gui->update(env->insert_widget, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	  }
	if (env->connect_ongoing)
	  {
	    env->gui->update(env->connect_widget, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	    env->gui->update(env->connect_line, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	  }
	if (env->has_wayland)
	  {
	    env->gui->update(env->window_decoration, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	  }
	
	if (e.button==0)
	  {
	int cs = env->connect_clicks.size();
	for(int ci = 0;ci<cs;ci++)
	  {
	    W wid = env->connect_clicks[ci];
	    int connected = env->gui->chosen_item(wid);
	    if (connected==0)
	      {
		//std::cout << "Connect_click" << std::endl;
		std::string uid = env->gui->get_id(wid);
		int j = env->gui->get_index(wid);
		int sz = env->gui->get_size2(wid);
		//std::cout << "Connect_click: " << j << " " << sz << std::endl;
		W canvas_item = env->gui->find_canvas_item(env->canvas, uid);
		if (canvas_item.id==-1) continue;
		BM bm = env->ev->bitmap_api.newbitmap(2,2);
		W ico_1 = env->gui->icon(bm);
		env->connect_widget = env->gui->insert_widget(ico_1, std::bind(&connect_target, _1, _2, env));
		
		env->connect_line = env->gui->line(canvas_item, env->gui->size_x(canvas_item),16+16+5+4+(env->gui->size_y(canvas_item)-16-16)*j/sz,
						   env->connect_widget, 0, 0, env->sh2, env->sh);
		
		env->connect_start_uid = uid;
		env->connect_start_j = j;
		env->connect_start_sz = sz;
		env->connect_ongoing = true;
		break;
	      }
	    
	  }

	  }
#if 0
	float param_x = env->gui->dynamic_param(env->txt2, 0);
	env->gui->set_dynamic_param(env->scroll_area, 1, param_x);
	float param_x1 = env->gui->dynamic_param(env->scrollbar_x, 0);
	env->gui->set_dynamic_param(env->canvas_area, 0, param_x1);
	//float param_y1 = env->gui->dynamic_param(env->scrollbar_y, 0);
	//env->gui->set_dynamic_param(env->canvas_area, 1, param_y1);
#endif

	env->ev->mod_api.update_lines_from_canvas(env->canvas, env->mod, 0);
	
	//int area_y = env->gui->size_y(env->array);
	//std::cout << area_y << std::endl;
	//env->gui->set_dynamic_param(env->txt2, 0, area_y);
	
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
		
		int i = env->dialog_i1;
		std::string uid = env->dialog_uid;
		std::vector<std::string*> refs;
		refs = env->ev->mod_api.refs_from_function(env->mod, 0, uid);
		std::vector<std::string> types;
		types = env->ev->mod_api.types_from_function(env->mod, 0, uid);
		
		int s = refs.size();
		for(int i=0;i<s;i++)
		  {
		    std::string *ref = refs[i];
		    //std::cout << i << " " << (*env->vec4[i]).i_value << std::endl;
		    std::string val = *ref;
		    env->gui->generic_to_string(*env->vec4[i], types[i], val);
		    *ref = val;
		  }
	      }
	  }
	if (e.button==-1) { env->flip_ongoing = false; }
	
	int sel = env->gui->chosen_item(env->scroll_area);
	if (sel != -1 && e.button==0 && e.type==1025 && !env->insert_ongoing)
	  {
	    //std::cout << "Scroll_area: " << sel << std::endl;
	    W w = env->gui->get_child(env->array, sel);
	    int sel2 = env->gui->chosen_item(w);
	    //std::cout << "Chosen: " << sel2 << std::endl;
	    if (sel2 == 0 && !env->flip_ongoing)
	      {
		//std::cout << "Flip!" << std::endl;
		bool b = env->flip_status[sel];
		b = !b;
		env->flip_status[sel] = b;
		
		env->gui->select_item(w, b?0:1);
		env->flip_ongoing = true;
		env->gui->set_dynamic_param(env->scroll_area, 1, 0.0 /* param_x */);
		//env->gui->update(env->scroll_area, e.cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);

	      }
	    if (sel2>0)
	      {
		std::string name;
		switch(sel)
		  {
		  case 0:
		    name = env->gui->bitmapapi_functions_item_label(sel2-1);
		    break;
		  case 1: 
		    name = env->gui->boolbitmapapi_functions_item_label(sel2-1);
		    break;
		  case 2: 
		    name = env->gui->floatbitmapapi_functions_item_label(sel2-1);
		    break;
		  case 3:
		    name = env->gui->polygonapi_functions_item_label(sel2-1);
		    break;

		  case 4:
		    name = env->gui->shadermoduleapi_functions_item_label(sel2-1);
		    break;
		  case 5:
		    name = env->gui->linesapi_functions_item_label(sel2-1);
		    break;
		  case 6:
		    name = env->gui->pointsapi_functions_item_label(sel2-1);
		    break;
		  case 7:
		    name = env->gui->moveapi_functions_item_label(sel2-1);
		    break;
		  case 8:
		    name = env->gui->pointapi_functions_item_label(sel2-1);
		    break;
		  case 9:
		    name = env->gui->vectorapi_functions_item_label(sel2-1);
		    break;

		  case 10:
		    name = env->gui->volumeapi_functions_item_label(sel2-1);
		    break;
		  case 11:
		    name = env->gui->floatvolumeapi_functions_item_label(sel2-1);
		    break;
		  case 12:
		    name = env->gui->colorvolumeapi_functions_item_label(sel2-1);
		    break;
		  case 13:
		    name = env->gui->fontapi_functions_item_label(sel2-1);
		    break;
		  case 14:
		    name = env->gui->textureapi_functions_item_label(sel2-1);
		    break;
		  case 15:
		    name = env->gui->booleanopsapi_functions_item_label(sel2-1);
		    break;
		  case 16:
		    name = env->gui->polygondistapi_functions_item_label(sel2-1);
		    break;
		  case 17:
		    name = env->gui->waveformapi_functions_item_label(sel2-1);
		    break;
		  case 18:
		    name = env->gui->blockerapi_functions_item_label(sel2-1);
		    break;
		  case 19:
		    name = env->gui->framebuffermoduleapi_functions_item_label(sel2-1);
		    break;
		    
		  default:
		    {
		      //std::cout << "SEL: " << sel << std::endl;
		      DllData &d = env->dlls[sel-20];
		      std::vector<Item*> funcs = (*d.functions)();
		      Item *item = funcs[sel2-1];
		      name = item->Name();
		      break;
		    }

		  };
		//std::cout << "Chosen label: " << name << std::endl;
		env->insert_mod_name = name;
		W ww = { 0 };
		std::vector<W*> conn;
		int ssk = 8;
		for(int i=0;i<ssk;i++) {
		  env->connect_clicks.push_back(ww);
		  conn.push_back(&env->connect_clicks[env->connect_clicks.size()-1]);
		}
		int uid_num = env->unique_id_counter;
		std::stringstream ss;
		ss << "uid" << uid_num;
		std::string uid = ss.str();
		
		env->chosen_item = env->ev->mod_api.inserted_widget(*env->gui, env->mod, 0, env->atlas, env->atlas_bm, name, conn /*env->connect_clicks[env->connect_clicks.size()-1]*/, uid, env->connect_targets);
		int sk = env->connect_clicks.size()-1;
		for(;sk>=0;sk--) { if (env->connect_clicks[sk].id==0) env->connect_clicks.pop_back(); else break; }
		env->insert_widget = env->gui->insert_widget(env->chosen_item, std::bind(&callback_func, _1, _2, env));
		env->insert_ongoing = true;
	      }
	  }
	if (env->insert_ongoing && e.button == -1)
	  {
	    env->insert_ongoing2 = true;
	  }
	if (env->insert_ongoing2 && e.button==0 && e.type==1025)
	  {
	    env->insert_ongoing = false;
	    env->insert_ongoing2 = false;
	  }
	if (env->connect_ongoing && e.button == -1)
	  {
	    env->connect_ongoing2 = true;
	  }
	if (env->connect_ongoing2 && e.button==0 && e.type==1025)
	  {
	    env->connect_ongoing = false;
	    env->connect_ongoing2 = false;
	  }
	

	//int selected_item = env->gui->chosen_item(env->txt);
	//int selected_item2 = -1;
	//if (selected_item != -1)
	//  {
	//    env->opened_menu_num = selected_item;
	//  }
	//if (env->opened_menu_num != -1)
	//  {
	//    env->gui->update(env->menus[env->opened_menu_num], e.cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	//    if (e.button == -1) { env->state=1; }
	//    if (e.button==0 && e.type==1025 && env->state==1)
	//      {
	//	selected_item2 = env->gui->chosen_item(env->menus[env->opened_menu_num]);
	//	env->opened_menu_num = -1;
	//	env->state = 0;
	//	//std::cout << selected_item2 << std::endl;
	//      }
	//  }
      }
    //float param_x = env->gui->dynamic_param(env->txt2, 0);
    //env->gui->set_dynamic_param(env->scroll_area, 1, 0.0 /* param_x */);
	float param_x1 = env->gui->dynamic_param(env->scrollbar_x, 0);
	env->gui->set_dynamic_param(env->canvas_area, 0, param_x1);
	//float param_y1 = env->gui->dynamic_param(env->scrollbar_y, 0);
	//env->gui->set_dynamic_param(env->canvas_area, 1, param_y1);

}
float f(float w)
{
  return cos(w);
}

class ItemConvert : public GameApiItem
{
public:
  ItemConvert(Item *i) : ii(i) { }
  virtual int Count() const { return 1; }
  virtual std::string Name(int i) const { return ii->Name(); }
  virtual int ParamCount(int i) const { return ii->ParamCount(); }
  virtual std::string ParamName(int i, int p) const
  {
    return ii->ParamName(p);
  }
  virtual std::string ParamType(int i, int p) const
  {
    return ii->ParamType(p);
  }
  virtual std::string ParamDefault(int i, int p) const
  {
    return ii->ParamDefault(p);
  }
  virtual std::string ReturnType(int i) const
  {
    return ii->ReturnType();
  }
  virtual std::string ApiName(int i) const { return "WebApi"; }
  virtual std::string FuncName(int i) const { return ""; }
  virtual std::string Symbols() const { return ii->Symbols(); }
  virtual std::string Comment() const { return ""; }

  virtual int Execute(GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e, int j)
  {
    return ii->Execute(params);
  }
  virtual std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)
  {
    return ii->CodeGen(params, param_names);
  }
  virtual void BeginEnv(GameApi::ExecuteEnv &e, std::vector<GameApiParam> params) { }
  virtual void EndEnv(GameApi::ExecuteEnv &e) { }
private:
  Item *ii;
};

W functions_widget(GameApi::GuiApi &gui, std::string label, std::vector<GameApiItem*> vec, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::FtA atlas2, GameApi::BM atlas_bm2, GameApi::W insert);


void print_stack_trace()
{
#ifdef WINDOWS
  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();
  
  CONTEXT context;
  memset(&context, 0, sizeof(CONTEXT));
  context.ContextFlags = CONTEXT_FULL;
  RtlCaptureContext(&context);
  
  SymInitialize(process, NULL, TRUE);
  
  DWORD image;
  STACKFRAME64 stackframe;
  ZeroMemory(&stackframe, sizeof(STACKFRAME64));
  
#ifdef _M_IX86
  image = IMAGE_FILE_MACHINE_I386;
  stackframe.AddrPC.Offset = context.Eip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Ebp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  image = IMAGE_FILE_MACHINE_AMD64;
  stackframe.AddrPC.Offset = context.Rip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Rsp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  image = IMAGE_FILE_MACHINE_IA64;
  stackframe.AddrPC.Offset = context.StIIP;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.IntSp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrBStore.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.IntSp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#endif

  for (size_t i = 0; i < 25; i++) {
    
    BOOL result = StackWalk64(
      image, process, thread,
      &stackframe, &context, NULL, 
      SymFunctionTableAccess64, SymGetModuleBase64, NULL);
    
    if (!result) { break; }
    
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    
    DWORD64 displacement = 0;
    if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
      printf("[%i] %s\n", i, symbol->Name);
    } else {
      printf("[%i] ???\n", i);
    }
    
  }
  
  SymCleanup(process);
#endif
}

extern std::string g_original_title;

void terminate_handler()
{
  //print_counters();
  print_stack_trace();
  std::cout << "Terminate Handler called!" << std::endl;
  for(;;);
}
void clear_counters();
void print_counters();
bool file_exists(std::string s);

extern bool g_vr_enable;
extern int g_vr_device_id;
//extern pthread_t g_main_thread;
int main(int argc, char *argv[]) {
  //clear_counters();
  //SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T)-1);

  //pid_t pid = getpid();
  //std::cout << "pid: " << (long)pid << std::endl;
  InstallProgress(888,"init",10);
  //ProgressBar(888,0,10,"init");
  set_current_block(-2);

  //g_main_thread = pthread_self();

  std::set_terminate(&terminate_handler);
  srand(time(NULL));
  Env *e2 = new Env;
  Env &e = *e2;
  EveryApi ev(*e2);
  
  


  Envi env;
  env.env = e2;

#ifdef WINDOWS
  //env.dlls = load_dlls("DllList.txt");
#else
  //env.dlls = load_dlls("DllList_linux.txt");
#endif
  int screen_x = 1200;
  int screen_y = 900;
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  std::string filename = drive+path+"\\mod.txt";
  std::cout << "Loading:" << filename << std::endl;
#else
  std::string filename = "mod.txt";
#endif
  for(int i=1;i<argc;i++)
    {
      if (std::string(argv[i])=="--dump")
	{
	  std::string s = ev.mod_api.dump_functions();
	  std::cout << s << std::endl;
	  return 0;
	}
      if (std::string(argv[i])=="--mg")
	{
	  screen_x = 2560;
	  screen_y = 1600;
	}

      if (std::string(argv[i])=="--huge")
	{
	  screen_x = 1200;
	  screen_y = 900;
	}
      if (std::string(argv[i])=="--mega")
	{
	  screen_x = 1920;
	  screen_y = 1080;
	}
      if (std::string(argv[i])=="--small")
	{
	  screen_x = 800;
	  screen_y = 600;
	}
      if (std::string(argv[i])=="--vr")
	{
	  g_vr_enable = true;
	  std::string device_id = std::string(argv[i+1]);
	  g_vr_device_id = 0;
	  if (device_id=="vive") { g_vr_device_id = 1; }
	  if (device_id=="oculus") { g_vr_device_id=2; }

	  if (g_vr_device_id==0) g_vr_device_id = 1; else i++;
	  std::string devices[] = { "None", "Vive", "Oculus" };
	  std::cout << "Choosing:" << devices[g_vr_device_id] << std::endl;
	  
	}
      if (std::string(argv[i])=="--file")
	{
	  filename = std::string(argv[i+1]);
	  i++;
	}
    }
  std::ifstream stream(filename.c_str());
  if (!stream.is_open())
    {
      std::ofstream stream2(filename.c_str());
      stream2 << "\n\ntest" << std::flush;
      stream2.close();
    }


  WM mod = ev.mod_api.load(filename);

  bool has_wayland = false;
  
  const char *ee = getenv("XDG_SESSION_TYPE");
  if (ee)
    std::cout << "SESSION TYPE:" << ee << std::endl;
  const char *ee2 = getenv("SDL_VIDEODRIVER");
  bool has_wayland1 = false;
  bool has_wayland2 = false;
  if (ee2 && !strcmp(ee2,"wayland")) {
    has_wayland1 = true;
  }
  if (ee && !strcmp(ee,"wayland")) {
    has_wayland2 = true;
  }
  has_wayland = has_wayland1 && has_wayland2;
  g_has_wayland = has_wayland;
  int extra_width = 0;
  int extra_height = 0;
  int extra_top = 0;
  int extra_left = 0;
  
  if (has_wayland) {
    extra_width = 0;
    extra_height = 10+30;
    extra_top = 30+5;
    extra_left = 5;
  }

  int display_width = screen_x+extra_width;
  int display_height = screen_y+extra_height;
  if (has_wayland)
    {
      display_width = -1;
      display_height = -1;
    }
  
  // initialize window
  g_original_title = "GameApi Builder -- meshpage.org";
  ev.mainloop_api.init_window(display_width,display_height,"GameApi Builder -- meshpage.org");
  if (has_wayland)
    {
      display_width = g_display_width;
      display_height = g_display_height;
    }
  
  int font_scale = 2;

  ProgressBar(888,0,5,"init");
  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.texture_shader();
  SH sh_2d = ev.shader_api.texture_shader();
  SH sh_arr = ev.shader_api.texture_shader();
  SH sh2 = ev.shader_api.colour_shader();
  SH sh3 = ev.shader_api.colour_shader();
  Ft font = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 10*font_scale,13*font_scale); // 13,15 
  Ft font2 = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 10*font_scale,13*font_scale); // 10,13
  Ft font3 = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 30*font_scale,30*font_scale); // 30,30

  std::string fname = "atlas0.txt";
  if (file_exists("/usr/share/atlas0.txt")) {
    fname = "/usr/share/atlas0.txt";
  }
    
  std::ifstream ss(fname.c_str());
  char c;
  bool flag = false;
  if (ss.get(c))
    {
    }
  else
    {
      flag = true;
    }
  if (argc==2 ||flag)
    {
      if (flag ||std::string(argv[1])=="--generate-font-atlas")
	{
	  std::cout << "Generating logo." << std::endl;
	  ev.mainloop_api.save_logo(ev);
	  std::cout << "Generating font atlas. " << std::endl;
	  std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!\"#¤%&/()=?+\\*^.,-<>|§½;:[]_ ";

	  //std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-();:_*/%+><[]";
	  FtA atlas = ev.font_api.font_atlas_info(ev, font, chars, 10*font_scale,13*font_scale, 25*font_scale);
	  FtA atlas2 = ev.font_api.font_atlas_info(ev, font2, chars, 10*font_scale,13*font_scale, 25*font_scale);
	  FtA atlas3 = ev.font_api.font_atlas_info(ev, font3, chars, 30*font_scale,30*font_scale, 65*font_scale);
	  BM atlas_bm = ev.font_api.font_atlas(ev, font, atlas, 10*font_scale,13*font_scale);
	  BM atlas_bm2 = ev.font_api.font_atlas(ev,font2, atlas2, 10*font_scale,13*font_scale);
	  BM atlas_bm3 = ev.font_api.font_atlas(ev,font3, atlas3, 30*font_scale,30*font_scale);
	  std::cout << "Saving 0" << std::endl;
	  ev.font_api.save_atlas(atlas, "atlas0.txt");
	  std::cout << "Saving 1" << std::endl;
	  ev.font_api.save_atlas(atlas2, "atlas1.txt");
	  std::cout << "Saving 2" << std::endl;
	  ev.font_api.save_atlas(atlas3, "atlas2.txt");
	  std::cout << "Saving 0a" << std::endl;
	  ev.bitmap_api.savebitmap(atlas_bm, "atlas_bm0.ppm", true);
	  std::cout << "Saving 1a" << std::endl;
	  ev.bitmap_api.savebitmap(atlas_bm2, "atlas_bm1.ppm", true);
	  std::cout << "Saving 1b" << std::endl;

	  ev.bitmap_api.savebitmap(atlas_bm3, "atlas_bm2.ppm", true);
	  std::cout << "Done." << std::endl;
	  //exit(0);
	}
    }

#ifdef LINUX
  std::string a_atlas0;
  std::string a_atlas1;
  std::string a_atlas2;
  std::string a_atlas_bm0;
  std::string a_atlas_bm1;
  std::string a_atlas_bm2;
  
  if (file_exists("/usr/share/atlas0.txt")) {
    a_atlas0 = "/usr/share/atlas0.txt";
    a_atlas1 = "/usr/share/atlas1.txt";
    a_atlas2 = "/usr/share/atlas2.txt";
    a_atlas_bm0 = "/usr/share/atlas_bm0.ppm";
    a_atlas_bm1 = "/usr/share/atlas_bm1.ppm";
    a_atlas_bm2 = "/usr/share/atlas_bm2.ppm";
  } else {
    a_atlas0 = "atlas0.txt";
    a_atlas1 = "atlas1.txt";
    a_atlas2 = "atlas2.txt";
    a_atlas_bm0 = "atlas_bm0.ppm";
    a_atlas_bm1 = "atlas_bm1.ppm";
    a_atlas_bm2 = "atlas_bm2.ppm";
  }
#else
  std::string a_atlas0 = "atlas0.txt";
  std::string a_atlas1 = "atlas1.txt";
  std::string a_atlas2 = "atlas2.txt";
  std::string a_atlas_bm0 = "atlas_bm0.ppm";
  std::string a_atlas_bm1 = "atlas_bm1.ppm";
  std::string a_atlas_bm2 = "atlas_bm2.ppm";
#endif
  
  FtA atlas = ev.font_api.load_atlas(a_atlas0.c_str());
  FtA atlas2 = ev.font_api.load_atlas(a_atlas1.c_str());
  FtA atlas3 = ev.font_api.load_atlas(a_atlas2.c_str());
  BM atlas_bm = ev.bitmap_api.loadbitmap(a_atlas_bm0.c_str());
  BM atlas_bm2 = ev.bitmap_api.loadbitmap(a_atlas_bm1.c_str());
  BM atlas_bm3 = ev.bitmap_api.loadbitmap(a_atlas_bm2.c_str());

#if 0
  FB atlas_bm_b = ev.float_bitmap_api.from_red(atlas_bm_a);
  FB atlas_bm2_b = ev.float_bitmap_api.from_red(atlas_bm2_a);
  FB atlas_bm3_b = ev.float_bitmap_api.from_red(atlas_bm3_a);

  BM atlas_bm = ev.float_bitmap_api.to_grayscale_color(atlas_bm_b, 255,255,255,255, 0,0,0,0);
  BM atlas_bm2 = ev.float_bitmap_api.to_grayscale_color(atlas_bm2_b,255,255,255,255, 0,0,0,0);
  BM atlas_bm3 = ev.float_bitmap_api.to_grayscale_color(atlas_bm3_b, 255,255,255,255, 0,0,0,0);
#endif


  // rest of the initializations
  ev.mainloop_api.init_3d(sh3, display_width, display_height);
  ev.mainloop_api.init_3d(sh_arr, display_width, display_height);
  ev.mainloop_api.init(sh, display_width, display_height);
  ev.mainloop_api.init(sh2, display_width, display_height);
  ev.mainloop_api.init(sh_2d, display_width, display_height);

  
  
  GuiApi gui(e, ev, sh);
  
  //W txt_0 = gui.highlight(100,100);
  //W txt_1 = gui.margin(txt_0, 10, 10, 10, 10);
  //W txt_2 = gui.text("Hello World", font);
  //W txt_3 = gui.layer(txt_1, txt_2);
  //W arr[] = { txt_1, txt_2 };
  //W txt = gui.array_x(arr, 2, 2);
#if 0
  std::vector<std::string> vec;
  vec.push_back("Edit");
  vec.push_back("File");
  vec.push_back("Open");
  W txt = gui.main_menu(vec, atlas, atlas_bm);
  std::vector<std::string> vec2;
  vec2.push_back("test1");
  vec2.push_back("test2");
  vec2.push_back("test3");
  std::vector<W> menus;
  for(int i=0;i<vec.size();i++)
    {
      W txt_2 = gui.menu(txt,i, vec2, atlas, atlas_bm);
      menus.push_back(txt_2);
    }
#endif			 



  //std::vector<std::string> vec3;
  //vec3.push_back("newbitmap");
  //vec3.push_back("newbitmap");
  //W test1 = gui.list_item_opened(100,"Bi tmapApi", font, vec3, font);
  //gui.set_pos(test1, 400,400);
  ProgressBar(888,1,5,"init");
  env.list_tooltips = gui.empty();
  std::vector<W> items;
  for(int i=0;i<1;i++)
    {
      items.push_back(gui.bitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.boolbitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.floatbitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.polygonapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));

  ProgressBar(888,2,5,"init");

      items.push_back(gui.shadermoduleapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.linesapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.pointsapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.moveapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.pointapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.vectorapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));

  ProgressBar(888,3,5,"init");

      items.push_back(gui.volumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.floatvolumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.colorvolumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.fontapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.textureapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.booleanopsapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.polygondistapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.waveformapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.blockerapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
      items.push_back(gui.framebuffermoduleapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));

  ProgressBar(888,4,5,"init");
      
#ifdef WINDOWS
#if 0
      int s = env.dlls.size();
      for(int ii=0;ii<s;ii++)
	{
	  DllData &d = env.dlls[ii];
	  std::string apiname = (*d.api_name)();
	  std::vector<Item*> v = (*d.functions)();
	  std::vector<GameApiItem*> vec;
	  int s = v.size();
	  for(int k=0;k<s;k++)
	    {
	      vec.push_back(new ItemConvert(v[k]));
	      global_functions.push_back(new ItemConvert(v[k]));
	    }
	  W w = functions_widget(gui, apiname, vec, atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips);
	  items.push_back(w);
	}
#endif
#endif
    }
  W array = gui.array_y(&items[0], items.size(), 5);
  W scroll_area = gui.scroll_area(array, gui.size_x(array), screen_y-30, screen_y);

  //W txt2 = gui.scrollbar_y(20, screen_y-30, gui.size_y(array));
  //gui.set_pos(txt2, gui.size_x(scroll_area), 30);

  gui.set_pos(scroll_area, extra_left, extra_top);
  gui.set_size(scroll_area, 140.0, screen_y);
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
  ProgressBar(888,5,5,"init");
  ev.mod_api.insert_to_canvas(gui, canvas, mod, 0, atlas, atlas_bm, env.connect_clicks, env.connect_targets, env.display_clicks, env.edit_clicks, env.delete_key, env.codegen_button_vec, env.popup_open);
  
  //ProgressBar(888,7,10,"init");
  ev.mod_api.insert_links(ev, gui, mod, 0, env.connect_links, canvas, env.connect_targets, sh2, sh);
  //ProgressBar(888,8,10,"init");
  add_to_canvas(gui, canvas, env.connect_links);
  //print_connect_targets("init", &env);
  //ProgressBar(888,9,10,"init");


  W canvas_area = gui.scroll_area(canvas, screen2_x, screen2_y, screen_y);
  //W scrollbar_y = gui.scrollbar_y(20, screen2_y-20, sy);
  W scrollbar_x = gui.scrollbar_x(screen2_x-20, 20, sx); 

  W line = gui.rectangle(0.0, 4.0, 0.0, screen_y, 0xffffffff);
  W decoration = gui.window_decoration(screen_x,screen_y, "GameApi Builder -- meshpage.org", atlas, atlas_bm);
  gui.set_pos(line, 140-5+extra_left, extra_top);
  gui.set_pos(canvas_area, 140+extra_left, extra_top);
  gui.set_pos(scrollbar_x, 140+extra_left, extra_top+screen_y-20);
  //gui.set_pos(scrollbar_y, screen_x-20, 30);
  gui.set_pos(decoration,0,0);

  
  
  env.x = 0.0;
  env.y = 0.0;
  env.z = 0.0;
  env.target_string = "abcde";

  env.line = line;
  env.gui = &gui;
  env.ev = &ev;
  //env.txt = txt;
  //env.txt2 = txt2;
  env.font = font;
  env.font3 = font3;
  env.atlas = atlas;
  env.atlas2 = atlas2;
  env.atlas3 = atlas3;
  env.atlas_bm = atlas_bm;
  env.atlas_bm2 = atlas_bm2;
  env.atlas_bm3 = atlas_bm3;
  env.unique_id_counter = ev.mainloop_api.random();
  //env.editor = editor;
  env.editor_visible = false;
  env.display = gui.empty();
  env.mem = gui.empty();
  env.display_visible = false;
  //env.wave = wave;
  //env.test1 = test1;
  env.array = array;
  env.insert_ongoing = false;
  env.mod = mod;
  env.canvas = canvas;
  env.canvas_area = canvas_area;
  //env.scrollbar_y = scrollbar_y;
  env.scrollbar_x = scrollbar_x;
  //env.gameapi = gameapi_2;
  env.scroll_area = scroll_area;
  env.window_decoration = decoration;
  //env.menus = menus;
  env.opened_menu_num = -1;
  env.connect_ongoing = false;
  env.connect_ongoing2 = false;
  env.sh2 = sh2;
  env.sh = sh;
  env.sh3 = sh3;
  env.sh_arr = sh_arr;
  env.sh_2d = sh_2d;
  env.screen_size_x = screen_x+extra_width;
  env.screen_size_y = screen_y+extra_height;
  env.filename = filename;
  env.has_wayland = has_wayland;
  env.extra_left = extra_left;
  env.extra_top = extra_top;
  env.screen_y = screen_y;
  env.screen_x = screen_x;
  
  //ev.mainloop_api.reset_time();
  //ev.mainloop_api.display_logo(ev);


  ev.mainloop_api.switch_to_3d(false, sh3, screen_x+extra_width, screen_y+extra_height);
  //ev.mainloop_api.switch_to_3d(false, sh_arr, screen_x, screen_y);
  ev.shader_api.use(sh);
  ev.mainloop_api.alpha(true);

  //print_counters();
  //ev.mainloop_api.display_logo(ev);
  //ProgressBar(888,10,10,"init");

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 30,1);
#endif


}
