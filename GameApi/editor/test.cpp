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
#include <filesystem>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include "GameApi_low.hh"
#ifdef LINUX
#include <sys/time.h>
#include <unistd.h>
#endif
using namespace GameApi;
#ifndef WINDOWS
#define WAYLAND 1
#endif

#ifdef WAYLAND
#include <wayland-client.h>
#endif

#include "Tasks.hh"

extern std::string g_gpu_vendor;

struct ArrayType
{
  int type; // arraytypesinuse
  std::vector<int> vec;
};
class Envi;
void render_cb(Envi *env);
void refresh();
void ClearProgress();

IMPORT std::string remove_prefix(std::string url);
IMPORT extern Low_SDL_Window *sdl_window;

int find_str(std::string s, std::string el);

#ifdef LINUX
extern "C" void _udev_device_get_action() { }
#endif


extern std::vector<void (*)(void*)> g_transparent_callback_objs;
extern std::vector<void*> g_transparent_callback_params;
extern std::vector<int> g_transparent_callback_ids;
extern std::vector<float> g_transparent_pos;
extern const char *g_videodriver;

extern std::vector<std::vector<std::string> > g_collect_authors;
extern std::vector<std::vector<std::string> > g_collect_licenses;

IMPORT std::string get_zip_license_file(std::string s);

std::vector<std::string> g_license_filenames;
std::vector<std::string> g_license_urls;
std::vector<std::string> g_license_authors;

bool event_lock=true;
uint64_t event_lock_time=0;
extern GameApi::W enum_popup;
extern GameApi::W enum_click;
extern bool g_update_download_bar;
IMPORT bool enum_editor_callback(GameApi::Env &env,GameApi::GuiApi &gui, GameApi::W click_widget, std::string type, int mouse_x, int mouse_y, GameApi::FtA atlas, GameApi::BM atlas_bm, int x_gap, std::vector<GameApi::W> &areas, int button, int type2);
IMPORT void enum_editor_draw(GameApi::EveryApi &ev, GameApi::GuiApi &gui);
IMPORT void enum_editor_handle_event(GameApi::GuiApi &gui, std::vector<GameApi::W> vec, int button);
IMPORT void enum_set_value(GameApi::Env &e, GameApi::W enum_click, int value);
IMPORT ArrayType *find_array(GameApi::Env &e, GameApi::ARR arr);

IMPORT std::string replace_string(std::string str, char ch, char ch2);
IMPORT void send_post_request(std::string url, std::string headers, std::string data);
IMPORT std::string replace_str(std::string ste, std::string repl, std::string subst);

IMPORT void clear_all_caches();

extern std::vector<std::string> g_registered_urls;
extern int g_event_screen_x;
extern int g_event_screen_y;
IMPORT extern int g_display_width;
IMPORT extern int g_display_height;
extern int g_window_pos_x;
extern int g_window_pos_y;
extern std::string gameapi_homepageurl;
extern int g_has_wayland;
extern GameApi::W g_progress_dialog;
extern std::vector<std::string> g_prog_labels;
extern GameApi::EveryApi *g_everyapi2;
extern GameApi::FtA g_atlas;
extern GameApi::BM g_atlas_bm;
extern GameApi::W g_progress_dialog;
extern GameApi::GuiApi *g_everyapi_gui;
IMPORT extern pthread_t g_main_thread_id;


struct wl_display;
struct wl_surface;
struct wl_shell_surface;

extern wl_display *g_wl_display; 
extern wl_surface *g_wl_surface;
extern wl_shell_surface *g_wl_shell_surface;

extern int current_mem_usage;
extern int max_mem_usage;

void mem_summary()
{
  //std::cout << "Current mem usage: " << current_mem_usage/1024000 << "Mb" << std::endl;
  //std::cout << "Max mem usage: " << max_mem_usage/1024000 << "Mb" << std::endl;
}

IMPORT void clear_codegen();
IMPORT void InstallProgress(int num, std::string label, int max=15);
IMPORT void ProgressBar(int num, int val, int max, std::string label);
IMPORT void set_codegen_values(GameApi::WM mod2, int id, std::string line_uid, int level);
IMPORT void pthread_system(std::string str);
IMPORT std::string find_html2(GameApi::HML ml, GameApi::Env &env);
IMPORT std::string find_homepage2(GameApi::HML ml, GameApi::Env &env);
IMPORT std::vector<unsigned char> load_from_url(std::string url);

IMPORT std::string get_last_line(std::string file, char ch);

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


IMPORT std::vector<std::string> find_additional_urls(GameApi::Env &e, GameApi::EveryApi &ev, std::string url);





struct Envi {
  bool envi_ready=false;
  Env *env;
  EveryApi *ev;
  GuiApi *gui;
  std::string target_string;
  float x,y,z;
  std::vector<GuiApi::EditTypes> edit_data;
  W editor;
  std::vector<W> enum_click_targets;
  std::vector<GameApi::W> areas;
  std::vector<std::string> enum_types;
  bool editor_visible;
  W display;
  W license_dialog;
  W license_dialog_next;
  W license_dialog_cancel;
  W license_dialog_canvas;
  W license_dialog_canvas_area;
  W license_dialog_scrollbar;
  W mem;
  bool display_visible;
  bool license_dialog_visible;
  bool progress_visible;
  bool progress_rest;
  W txt;
  W txt2;
  W scroll_area;
  //W scroll_area2;
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
  bool insert_ongoing=false;
  bool insert_ongoing2=false;

  bool connect_ongoing=false;
  bool connect_ongoing2=false;
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
	  std::vector<int> vec = envi->ev->mod_api.indexes_from_funcname(*envi->ev,funcname);
	  int real_index = vec[num];
	  //std::cout << "Real index: " << real_index << std::endl;
	  bool is_array = false;
	  bool is_array_return = false; // TODO
	  bool b = envi->ev->mod_api.typecheck(*envi->ev,envi->mod, 0, envi->connect_start_uid, uid, real_index, envi->connect_start_j, is_array, is_array_return);
	  bool erase = false;
	  if (b) 
	    {
	      if (envi->connect_start_sz>1 && !is_array)
		{ // multiple return values
		  std::string val2 = envi->connect_start_uid; 
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, val2, envi->connect_start_j);
		  envi->ev->mod_api.change_param_multiple_return_values(envi->mod, 0, val2, -1 /*envi->connect_start_j*/, envi->connect_start_sz);
		  erase=true;
		  
		}
	      else
		if (envi->connect_start_sz>1 && is_array)
		  {

		  std::string val = envi->ev->mod_api.param_value(envi->mod, 0, uid, real_index);
		  std::vector<std::string> vec = envi->ev->mod_api.parse_param_array(val);
		  std::stringstream ss;
		  ss << envi->connect_start_j;
		  vec.push_back(envi->connect_start_uid + "@" + ss.str());
		  std::string val2 = envi->ev->mod_api.generate_param_array(vec);
		  envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, val2,0);
		  erase=false;
		    
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
		  if (sz<0||sz>25) sz=1;
		  if (j<0||j>25) j=0;

		  int yy = 16+16+5+4+(envi->gui->size_y(start_link)-16-16)*j/sz;
		  if (sz<=1) yy=16+5+4+(envi->gui->size_y(start_link)-16-16)/2;
		  W link = envi->gui->line( start_link, envi->gui->size_x(start_link),yy /*(envi->gui->size_y(start_link)-16)+*/ /*16+16+5+4+(envi->gui->size_y(start_link)-16-16)*j/sz*/,
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


  std::vector<std::pair<std::string,std::string> > vec = envi->ev->mod_api.defaults_from_function(*envi->ev,envi->insert_mod_name);
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


IMPORT void save_dd(GameApi::Env &e, GameApi::EveryApi &ev, std::string filename, std::string script, std::vector<std::string> urls);
PT old_cursor_pos;



void FinishProgress();
Envi *g_env;
IMPORT extern bool g_progress_halt;
class IterTab;
extern IterTab *g_start2;

extern bool g_dragdrop_enabled;
extern std::string g_dragdrop_filename;


class BuilderIter
{
public:
  virtual void start(void *arg)=0;
  virtual void render(void *arg)=0;
  virtual void update(void *arg, MainLoopApi::Event &e)=0;
};

  uint64_t get_time_us() {
#ifdef LINUX
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#endif
#ifdef WINDOWS
    static LARGE_INTEGER frequency;
    static BOOL frequencyInitialized = FALSE;
    if (!frequencyInitialized) {
        QueryPerformanceFrequency(&frequency);
        frequencyInitialized = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    // Convert counts to microseconds
    return (uint64_t)((counter.QuadPart * 1000000) / frequency.QuadPart);
#endif
  }

std::string create_tmp_filename(std::string prefix, std::string suffix)
{
  std::string start="";
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder/tmp.counter";
#endif
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\_gameapi_builder\\tmp.counter";
#endif
  std::ifstream ss(start.c_str());
  int val=0;
  ss >> val;
  ss.close();
  val++;
  std::ofstream ss2(start.c_str());
  ss2 << val;
  ss2.flush();
  ss2.close();
  std::stringstream ss3;
  ss3 << prefix << val << suffix;
  return ss3.str();
}

class MainIter : public BuilderIter
{
public:
  void start(void *arg)
  {
    Envi *env = (Envi*)arg;
    //g_progress_halt = true;
    //env->env->async_scheduler();
    //std::cout << "MainIter start" << std::endl;
    if (!env->envi_ready) return;
    //std::cout << "MainIter cont" << std::endl;
    
    env->ev->shader_api.use(env->sh);
    if (!event_lock) {
    if (env->has_wayland) {
      env->ev->mainloop_api.clear(0x00000000);
    } else {
      env->ev->mainloop_api.clear(0xff000000);
    }
    }
    if (env->old_window_pos_x != g_window_pos_x || env->old_window_pos_y != g_window_pos_y) {
      int x = g_window_pos_x;
      int y = g_window_pos_y;
      
      env->gui->set_pos(env->window_decoration, x,y);
      env->gui->set_pos(env->line, x+140-5+env->extra_left,y+env->extra_top);
      env->gui->set_pos(env->canvas_area, x+140+env->extra_left, y+env->extra_top);
      env->gui->set_pos(env->scrollbar_x, x+140+env->extra_left, y+env->extra_top+env->screen_y-20);
      //env->gui->set_pos(env->scroll_area2, x+env->extra_left, y+env->extra_top);
      env->gui->set_pos(env->scroll_area, x+env->extra_left, y+env->extra_top);

      
    }
#ifdef WAYLAND
    static bool firsttime = true;
    if (firsttime)
      t0=get_time_us();
    firsttime = false;
#endif    
  }
#ifdef WAYLAND
  const uint64_t target_frametime_us = 10000;
#endif
  void render(void *arg)
  {
    Envi *env = (Envi*)arg;
    //std::cout << "MainIter::render start" << std::endl;
    if (!env->envi_ready) return;
    //std::cout << "Render attempt!" << std::endl;
    //std::cout << "MainIter::render cont" << std::endl;
#ifdef WAYLAND
    uint64_t t1 = get_time_us();
    if (t1-t0 < target_frametime_us)
      usleep(target_frametime_us - (t1-t0));
    t0=get_time_us();
#endif    
    
    render_cb(env);

    FinishProgress();
  }
 
  virtual void update(void *arg, MainLoopApi::Event &e) { 
    //std::cout << "update:" << std::hex << e.type << std::dec << " " << e.ch << " " << e.button << std::endl;

    

    
    Envi *env = (Envi*)arg;
    //std::cout << "MainIter::update start" << std::endl;
    //std::cout << "1" << std::endl;
    if (!env->envi_ready) return;
    //std::cout << "2" << std::endl;
    if (env->ev->point_api.pt_y(e.cursor_pos)<34.0/*125.0-40.0*/) return;
    //std::cout << "3" << std::endl;
    if (env->env->download_bar_count()>0 && env->ev->point_api.pt_y(e.cursor_pos)>env->ev->mainloop_api.get_screen_height()-40.0) return;
    //std::cout << "4" << std::endl;
    //std::cout << "MainIter::update cont" << std::endl;


    old_cursor_pos = e.cursor_pos;
    if (e.type==256) { mem_summary(); exit(0); }
    if (e.type==0x300 && e.ch=='m') { mem_summary(); }
    if (e.type==0x200) {
      int sx = g_event_screen_x;
      int sy = g_event_screen_y;
      if (sx!=-1 && sy != -1) {
	W w = env->canvas_area;
	
	
      }
    }

    enum_editor_handle_event(*env->gui, env->enum_click_targets, e.button);

    //std::cout << "EVENT: " << e.type << std::endl;

    if (e.type==4096)
      { // dropfile
	std::string filename = e.drag_drop_filename;
	std::string shortfile;
	int pos=0;
	int s = filename.size();
	for(int i=0;i<s;i++)
	  {
	    if (filename[i]=='/'||filename[i]=='\\') pos=i+1;
	  }
	shortfile = filename.substr(pos);
	int id = env->env->add_to_download_bar(shortfile);
	int ii = env->env->download_index_mapping(id);

#ifdef LINUX
	std::ifstream ss(filename.c_str());
#endif
#ifdef WINDOWS
	std::ifstream ss(filename.c_str(),std::ios::in | std::ios::binary);
#endif
	std::vector<unsigned char> file;
	char ch;
	while(ss.get(ch)) {
	  unsigned char ch2 = ch;
	  file.push_back(ch2);
	}
	env->env->set_download_data(ii, file);
	env->env->set_download_progress(ii,100.0);
	env->env->set_download_ready(ii);
      }
    
    
    if (e.type==1024 && e.button==-1)
      {
	env->key_state = true;
      }
#ifdef EMSCRIPTEN
    if (e.type==1024 && e.button==0 && env->key_state==true)
      {
	// FIX EMSCRIPTEN EENTS NOT GIVING KEYDOWN EVENTS.
	e.type=1025;
	env->key_state = false;
      }
#endif
    
    
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
    
#ifndef EMSCRIPTEN
#endif
    if (e.type != 0x300 && e.type != 0x301)
      {
	e.ch=-1;
      }

    //std::cout << "update2:" << std::dec << e.type << std::dec << " " << e.ch << " " << e.button << std::endl;


    
    if (env->display_visible)
      {
	int chosen = env->gui->chosen_item(env->display_close);
	if (chosen==0)
	  {
	    env->display_visible = false;
	    env->ev->tracker_api.stop_music_playing();
	  }
	int chosen2 = env->gui->chosen_item(env->codegen_button);
	if (chosen2 == 0)
	  {
	    /* code generation here */
	    std::cout << "CodeGen" << std::endl;
	    env->ev->mod_api.codegen_reset_counter();
	    clear_codegen();
	    std::pair<std::string, std::string> p = env->ev->mod_api.codegen(*env->ev, env->mod, 0, env->codegen_uid,1000,0);
	    std::string s = p.second;
	    s = replace_str(s, "&", "&amp;");
	    s = replace_str(s, ">", "&gt;");
	    s = replace_str(s, "<", "&lt;");
	    s = replace_str(s, "\"", "&quot;");
	    s = replace_str(s, "\'", "&apos;");

	    std::string filename = create_tmp_filename("script_", ".txt");
	    
	    int id = env->env->add_to_download_bar(filename);
	    int ii = env->env->download_index_mapping(id);
	    std::vector<unsigned char> vec(s.begin(),s.end());
	    env->env->set_download_data(ii,vec);
	    env->env->set_download_progress(ii,100.0);
	    env->env->set_download_ready(ii);
	    std::cout << s << std::endl;
	  }
	int chosen3 = env->gui->chosen_item(env->collect_button);
	if (chosen3 == 0)
	  {

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

    if (1)
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
    if (1)
      {
	//std::cout << "e.button=" << e.button << std::endl; 
	static bool g_update = true;
	if (env->popup_visible==false && e.button != 2) { g_update=true; }
	if (g_update && e.button==2) { env->popup_visible=false; }
	int s = env->popup_open.size();
	for(int i=0;i<s;i++)
	  {
	    W w = env->popup_open[i];
	    int chosen = env->gui->chosen_item(w);
	    if (chosen==0 && g_update) // chosen==0 already checked e.button==2
	      {
		g_update=false; 
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
	if (1)
	  {
	    std::cout << "CodeGen!" << std::endl;
	    clear_codegen();
	    std::pair<std::string, std::string> p = env->ev->mod_api.codegen(*env->ev, env->mod, 0, uid,1000,0);
	    std::string s = p.second;
	    s = replace_str(s, "&", "&amp;");
	    s = replace_str(s, ">", "&gt;");
	    s = replace_str(s, "<", "&lt;");
	    s = replace_str(s, "\"", "&quot;");
	    s = replace_str(s, "\'", "&apos;");

	    std::string filename = create_tmp_filename("script_",".txt");
	    
	    int id = env->env->add_to_download_bar(filename);
	    int ii = env->env->download_index_mapping(id);
	    std::vector<unsigned char> vec(s.begin(),s.end());
	    env->env->set_download_data(ii,vec);
	    env->env->set_download_progress(ii,100.0);
	    env->env->set_download_ready(ii);


	    std::cout << s << std::endl;
	    
	  }
      }
    if (!env->display_visible && pkggen_button)
      {
	pkggen_button = false;
	std::string uid = popup_uid;
	
	std::string type2 = env->ev->mod_api.return_type(*env->ev,env->mod, 0, uid);
	//ProgressBar(933, 7,15, "Execute");
	GameApi::ExecuteEnv exeenv;
	if (type2 == "ML") {
	  g_collect_authors = std::vector<std::vector<std::string> >();
	  g_collect_licenses = std::vector<std::vector<std::string> >();
	  std::pair<int,std::vector<std::string> > ids = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr, g_async_count,0);
	  g_collect_authors = std::vector<std::vector<std::string> >();
	  g_collect_licenses = std::vector<std::vector<std::string> >();
	  std::pair<int,std::vector<std::string> > ids2 = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr2, g_async_count2,0);
	  
	  

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
	  
	  clear_codegen();
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

    static std::string license_uid;
    bool next_stage=false;
    bool next_stage2=false;
    if (!env->display_visible && !env->license_dialog_visible && display_button)
      {

	std::string uid = popup_uid;
	std::string s = env->ev->mod_api.get_funcname(env->mod,0,uid);
	if (s=="save_deploy") {	
	  env->license_dialog_visible = true;
	  license_uid=uid;
	  
	  GameApi::ExecuteEnv exeenv;
	  g_collect_authors = std::vector<std::vector<std::string> >();
	  g_collect_licenses = std::vector<std::vector<std::string> >();
 
	  std::pair<int,std::vector<std::string> > ids = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr, g_async_count,0);
	  //ProgressBar(933, 15,15, "Execute");
	  
	  //std::cout << "URLS:" << ids.second << std::endl;
	  std::vector<std::string> urls = ids.second;
	  std::vector<std::string> authors = g_collect_authors.size()>0?g_collect_authors[0]:std::vector<std::string>();
	  std::vector<std::string> licenses = g_collect_licenses.size()>0?g_collect_licenses[0]:std::vector<std::string>();
	  std::vector<std::string> urls2 = g_registered_urls;
	  int s = urls2.size();
	  for(int i=0;i<s;i++) urls.push_back(urls2[i]);
	  
	  //std::sort(urls.begin(),urls.end());
	  //auto last3 = std::unique(urls.begin(),urls.end());
	  //urls.erase(last3,urls.end());
	  
	  //std::vector<std::string> filenames;
	  g_license_filenames = std::vector<std::string>();
	  g_license_urls=licenses; //std::vector<std::string>();
	  g_license_authors = authors; //std::vector<std::string>();
	  int s2 = urls.size();
	  for(int i=0;i<s2;i++) {
	    std::cout << urls[i] << std::endl;
	    g_license_filenames.push_back(remove_prefix(urls[i]));

	    if (urls[i].substr(urls[i].size()-4,4)==".zip") {
	      std::cout << "ZIP HANDLING" << std::endl;
	      std::string contents = get_zip_license_file(urls[i]);
	      std::stringstream ss(contents);
	      std::string line;
	      std::string aut,license;
	      bool stop=false;
	      while(std::getline(ss,line)) {
		std::stringstream ss2(line);
		std::string star, author,aut2,type,lic,url;
		ss2 >> star >> author >> aut2 >> type >> lic >> url;
		stop=true;
		std::cout << author << std::endl;
		if (author=="author:") { aut=aut2; if (type[0]!='(') aut+=" " + type; }
		if (author=="license") { license=lic.substr(1,lic.size()-2); break; }
		stop=false;
	      }
	      if (stop) {
		g_license_authors[i] = aut;
		g_license_urls[i]=license;
	      }
	    }
	  }
	  
	  //static std::vector<std::string> license_urls;
	  s2=std::min(std::min(int(g_license_urls.size()),int(g_license_authors.size())),s2);
	  for(int i=0;i<s2;i++) {
	    if (g_license_urls[i]=="")
	      g_license_urls[i] = "https://tpgames.org/NO_LICENSE.txt";
	    if (g_license_authors[i]=="")
	      g_license_authors[i]= "LoremIpsum";
	  }

#ifdef WINDOWS
	    std::string drive = getenv("systemdrive");
	  std::string path = getenv("homepath");
	    std::string license_file = drive+path+"\\GAMEAPI_LICENSES.txt";
#else
	    const char *dd = getenv("BUILDER_DOCKER_DIR");
	    std::string dockerdir = dd?dd:"";
	    std::string license_file = dockerdir+"GAMEAPI_LICENSES.txt";
#endif
	  
	    std::ifstream ss(license_file.c_str());
	  std::string line;
	  while(std::getline(ss,line)) {
	    std::string filename;
	    std::string url;
	    std::string author;
	    std::stringstream ss2(line);
	    ss2 >> filename >> url >> author;
	    int sk=g_license_filenames.size();
	    for(int i=0;i<sk;i++)
	      {
		if (g_license_filenames[i]==filename) {
		  g_license_urls[i]=url;
		  g_license_authors[i]=author;
		}
	      }
	  }
	  
	  if (g_license_filenames.size()>0) {
	    env->license_dialog = env->gui->license_dialog(g_license_filenames, g_license_urls, g_license_authors, env->atlas, env->atlas_bm, env->atlas2, env->atlas_bm2,env->atlas3, env->atlas_bm3, env->license_dialog_next, env->license_dialog_cancel, env->license_dialog_canvas, env->license_dialog_canvas_area, env->license_dialog_scrollbar);
	    env->gui->set_pos(env->license_dialog, 200, 100);
	  } else {
	    next_stage=true;
	  }
	} else { // not save deploy
	  next_stage=true;
	}
      }

    if (env->license_dialog_visible)
      {
	
	int chosen = env->gui->chosen_item(env->license_dialog_next);
	if (chosen==0)
	  {
	    popup_uid=license_uid;
	    next_stage=true;
	  }
	int chosen2 = env->gui->chosen_item(env->license_dialog_cancel);
	if (chosen2==0)
	  {
	    env->license_dialog_visible=false;
	    display_button = false;
	  }
      }
    if (next_stage)
      {
	if (env->license_dialog_visible) {
	    env->license_dialog_visible = false;


	    std::vector<std::string> filenames;
	    std::vector<std::string> urls;
	    std::vector<std::string> authors;
	    

#ifdef WINDOWS
	    std::string drive = getenv("systemdrive");
	  std::string path = getenv("homepath");
	    std::string license_file = drive+path+"\\GAMEAPI_LICENSES.txt";
#else
	    const char *dd = getenv("BUILDER_DOCKER_DIR");
	    std::string dockerdir = dd?dd:"";
	    std::string license_file = dockerdir+"GAMEAPI_LICENSES.txt";
#endif

	    
	    std::ifstream ss(license_file.c_str());
	    std::string line;
	    while(std::getline(ss,line)) {
	    std::string filename;
	    std::string url;
	    std::string author;
	    std::stringstream ss2(line);
	    ss2 >> filename >> url >> author;
	    filenames.push_back(filename);
	    urls.push_back(url);
	    authors.push_back(author);
	    }
	    ss.close();
	    int s = g_license_filenames.size();
	    int pos = -1;
	    for(int i=0;i<s;i++)
	      {
		int ss = filenames.size();
		for(int j=0;j<ss;j++)
		  {
		    if (g_license_filenames[i]==filenames[j])
		      {
			pos = j;
		      }
		  }
		if (pos==-1)
		  {
		    filenames.push_back(g_license_filenames[i]);
		    urls.push_back(g_license_urls[i]);
		    authors.push_back(g_license_authors[i]);
		  }
		else
		  {
		    filenames[pos]=g_license_filenames[i];
		    urls[pos]=g_license_urls[i];
		    authors[pos]=g_license_authors[i];
		  }
	      }

	    std::ofstream ss2(license_file.c_str());
	    int sr = std::min(filenames.size(),urls.size());
	    for(int i=0;i<sr;i++)
	      {
		ss2 << filenames[i] << " " << urls[i] << " " << authors[i] << std::endl;
	      }
	    ss2.close();
	    
	}
	next_stage2 = true;
      }
    
    if (next_stage2)
      {
	display_button = false;
	if (1)
	  {
	    std::string uid = popup_uid;
	    if (1)
	      {
		//std::cout << "Execute for uid: " << uid << std::endl;
		clear_all_caches();
		env->env->free_temp_memory();


		// remove transparent callbacks if they're still here.
		// this can happen if part of the graph that contains
		// transparent stuff is not deleted properly.
		g_transparent_callback_objs.clear();
		g_transparent_callback_params.clear();
		g_transparent_callback_ids.clear();
		g_transparent_pos.clear();
		// Execute
		//InstallProgress(933, "Execute", 15);
		//ProgressBar(933, 0,15, "Execute");
		
		env->progress_visible = true;
		g_prog_labels = std::vector<std::string>();
		g_prog_labels.push_back("");
		g_everyapi2 = env->ev;
		g_atlas = env->atlas;
		g_atlas_bm = env->atlas_bm;
		
		std::string type2 = env->ev->mod_api.return_type(*env->ev,env->mod, 0, uid);
		//ProgressBar(933, 7,15, "Execute");
		GameApi::ExecuteEnv exeenv;
		if (type2 != "HML") {
		  g_collect_authors = std::vector<std::vector<std::string> >();
		  g_collect_licenses = std::vector<std::vector<std::string> >();

		  std::pair<int,std::vector<std::string> > ids = env->ev->mod_api.collect_urls(*env->ev, env->mod, 0, uid, exeenv, 1000, g_async_ptr, g_async_count,0);
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
		    
		static int g_id = -1;
		//std::cout << "ClearBlock" << std::endl;
		if (g_id!=-1) clear_block(g_id);
		g_id = add_block();
		set_current_block(g_id);
		set_codegen_values(env->mod,0,uid,1000);
		ClearProgress();
		int id = env->ev->mod_api.execute(*env->ev, env->mod, 0, uid, exeenv,1000,0); // TODO last 0=wrong
		set_current_block(-2);

		//if (g_update_download_bar)
		//  env->env->start_async(new DownloadUpdateTask(g_start));
		
		
		if (id==-1) {
		  std::cout << "Execute failed!" << std::endl;
		  //display = false;
		  env->progress_visible=false;
		  return;
		}
		env->codegen_uid = uid;
		
		// display dialog
		std::string type = env->ev->mod_api.return_type(*env->ev,env->mod, 0, uid);

		
		    
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
			if (type=="TF") {
			  TF tf;
			  tf.id = id;
			  ML ml2 = env->ev->mainloop_api.gltf_mesh_all(*env->ev,tf,1,1.0,1.0,0,400.0,-400.0,300.0,0,0xff00000,true);
			  //env->env->free_temp_memory();
			  //env->gui->delete_widget(env->mem);
			  env->display = env->gui->ml_dialog(ml2, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			  
			} else if (type=="TX") {
			  TX id1;
			  id1.id = id;
			  TXID id2=env->ev->texture_api.prepare(id1);			    
			    P I1=env->ev->polygon_api.fullscreen_quad(*env->ev);
			    MT I2=env->ev->materials_api.textureid(*env->ev,id2,1.0);
			    ML ml=env->ev->materials_api.bind(I1,I2);

			      env->env->free_temp_memory();
			      env->gui->delete_widget(env->mem);
			      env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			}
			else if (type=="TXID")
			  {
			    TXID id2;
			    id2.id = id;
			    P I1=env->ev->polygon_api.fullscreen_quad(*env->ev);
			    MT I2=env->ev->materials_api.textureid(*env->ev,id2,1.0);
			    ML ml=env->ev->materials_api.bind(I1,I2);

			      env->env->free_temp_memory();
			      env->gui->delete_widget(env->mem);
			      env->display = env->gui->ml_dialog(ml, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);
			  }
			else
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
				      //if (id!=1) {
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
					env->ev->tracker_api.stop_music_playing();
					//}
		     
				      display = false;
				    }
				  else
				    if (type=="O")
				      {
					O o;
					o.id = id;
					P I18=env->ev->polygon_api.cube(-10,10,-10,10,-10,10);
					VX I32=env->ev->voxel_api.blit_voxel(o,30,30,30,-300,300,-300,300,-300,300,-1,0);
					ARR I33=env->ev->voxel_api.voxel_instancing(I32,1,-300,300,-300,300,-300,300);
					std::vector<int> vec2 = find_array(*env->env,I33)->vec;
					std::vector<PTS> vec;
					for(int i=0;i<vec2.size();i++) vec.push_back(vec2[i]);
					//MT I34=env->ev->materials_api.m_def(*env->ev);
					//MT I35=env->ev->materials_api.phong(*env->ev,I34,-0.3,0.3,-1.0,0xffff8800,0xff666666,15.0);
					MT I35=env->ev->materials_api.gltf_material3(*env->ev,0.5,0.8,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,0.0,-400.0);
					//MT I35=env->ev->materials_api.snow(*env->ev,I34,0xffaaaa88,0xffeeee88,0xffffff88,0.5);
					ML I36=env->ev->voxel_api.voxel_bind(*env->ev,std::vector<P>{I18},vec,I35);
					env->env->free_temp_memory();
					env->gui->delete_widget(env->mem);
					
					env->display = env->gui->ml_dialog(I36, env->sh2, env->sh, env->sh_2d, env->sh_arr, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button);

			/*
			    
			  P p = env->ev->volume_api.rendercubes3(o, 255,255,255,
			  -300.0, 300.0,
			  -300.0, 300.0,
			  -300.0, 300.0);
			  env->env->free_temp_memory();
			  env->gui->delete_widget(env->mem);
			  env->display = env->gui->polygon_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->mem);*/
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
					  env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					  
					}
				      else
					if (type=="IBM")
					  {
					    IBM bm;
					    bm.id = id;
					    BM bm2 = env->ev->bitmap_api.intbitmap_bm(bm);
					    env->env->free_temp_memory();
					    env->gui->delete_widget(env->mem);
					    env->display = env->gui->bitmap_dialog(bm2, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					    
					  } else
					  if (type=="BM")
					    {
					      BM bm;
					      bm.id = id;
					      env->env->free_temp_memory();
					      env->gui->delete_widget(env->mem);
					      env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					    }
					  else if (type=="HML")
					    {
					      
					      display = false;
					      clear_codegen();
					      
					      time_t now = time(0);
					      char *dt = ctime(&now);
					      tm *gmtm = gmtime(&now);
					      dt = asctime(gmtm);
					      
					      std::string dt2(dt);
					      dt2 = replace_str(dt2, " ", "");
			
					      HML ml;
					      ml.id = id;
					      
					      env->ev->mod_api.codegen_reset_counter();

					      std::string htmlfile = find_html2(ml,*env->env);
					      htmlfile = replace_str(htmlfile, "@", "\n");					      
					      while(htmlfile[htmlfile.size()-1]=='\n') htmlfile=htmlfile.substr(0,htmlfile.size()-1);

					      htmlfile+='\n';
					      std::string lastline = get_last_line(htmlfile,'\n');
					      std::string label,id;
					      std::stringstream ss(lastline);
					      ss >> label >> id;
					      int ii=id.size();
					      for(int i=0;i<ii;i++) { if (id[i]=='=') id=id.substr(0,i); }
					      if (label=="ML")
						{
						  htmlfile+=std::string("RUN I888=ev.blocker_api.game_window2(ev,") + id + ",false,false,0.0,1000000.0);\n";
						}
					      if (label=="P")
						{
						  htmlfile+=std::string("ML I888=ev.polygon_api.render_vertex_array_ml2(ev,") + id + ");\n";
						  htmlfile+="RUN I889=ev.blocker_api.game_window2(ev,I888,false,false,0.0,100000.0);\n";
						  
						}
					      if (label=="MN")
						{
						  /* TODO
						     M m = env->ev->move_api.get_matrix(mn, 10.0, 0.01);
						     LI p = env->ev->points_api.matrix_display(*env->ev, m);
						     ML I5=ev.lines_api.ml_li_render(ev,E1,1.0);
						  */
						}
					      if (label=="MT")
						{
						  htmlfile+="PT I888=ev.point_api.point(0.0,0.0,0.0);\n";
						  htmlfile+="P I889=ev.polygon_api.sphere(I888,350,30,30);\n";
						  htmlfile+=std::string("ML I890=ev.materials_api.bind(I889,") + id + ");\n";
						  htmlfile+=std::string("RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,1000000.0);\n");
						}


					      std::string homepage = find_homepage2(ml,*env->env);
					      int val = rand();
					      std::stringstream ss2;
					      ss2 << val;
					      htmlfile = replace_str(htmlfile, "&", "&amp;");
					      htmlfile = replace_str(htmlfile, ">", "&gt;");
					      htmlfile = replace_str(htmlfile, "<", "&lt;");
					      htmlfile = replace_str(htmlfile, "\"", "&quot;");
					      htmlfile = replace_str(htmlfile, "\'", "&apos;");
#ifdef WINDOWS

					      int s = htmlfile.size()/3000+1;
					      for(int i=0;i<s;i++)
						{
						  int end = (i+1)*3000;
						  if (end>htmlfile.size()) end=htmlfile.size();
						  std::string file = htmlfile.substr(i*3000,end-i*3000);
						  if (i==0)
						    send_post_request(std::string("\"https://meshpage.org/save_tmp_script.php?id=")+ss2.str()+"&append=0\"" ,"Content-Type:text/plain", file);
						  else
						    send_post_request(std::string("\"https://meshpage.org/save_tmp_script.php?id=")+ss2.str()+"&append=1\"" ,"Content-Type:text/plain", file);
						}

					      
					      htmlfile = replace_string(htmlfile,'\n','@');
					      homepage = replace_string(homepage,'\n','@');
					      std::string cmd = std::string("explorer \"https://meshpage.org/gameapi_example.php?homepage=") + homepage + std::string("&id=") + ss2.str() + std::string("&date=") + dt2 + std::string("\"");

					      pthread_system(cmd.c_str());
			
#else

					      htmlfile = replace_string(htmlfile,'\n','@');
					      homepage = replace_string(homepage,'\n','@');
					      send_post_request(std::string("https://meshpage.org/save_tmp_script.php?id=")+ss2.str(),"Content-Type: text/plain", htmlfile);

					      std::string name = "gameapi_example.php";
					      int pos = find_str(htmlfile,"scene_transparency");
					      if (pos!=-1)
						{
						  name = "gameapi_example_transparent.php";
						}
					      
					      
			
					      const char *doc = getenv("BUILDER_DOCKER_DIR");
					      std::string dockerdir = doc?doc:"";
					      std::string cmd;
					      if (dockerdir!="") {
					      cmd = std::string("/usr/share/chromium \"https://meshpage.org/" + name + "?homepage=") + homepage + std::string("&id=") + ss2.str() + std::string("&date=") + dt2 + std::string("\"");
					      } else {
					      cmd = std::string("chromium \"https://meshpage.org/" + name + "?homepage=") + homepage + std::string("&id=") + ss2.str() + std::string("&date=") + dt2 + std::string("\"");
					      }
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
					      env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					    } 
					  else if (type=="CBM")
					    {
					      CBM cbm;
					      cbm.id = id;
					      BM bm = env->ev->cont_bitmap_api.sample(cbm, 200,200);
					      env->env->free_temp_memory();
					      env->gui->delete_widget(env->mem);
					      env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					      
					    }
					  else if (type=="BB")
					    {
					      BB bb;
					      bb.id = id;
					      BM bm = env->ev->bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);
					      env->env->free_temp_memory();
					      env->gui->delete_widget(env->mem);
					      env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					      
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
					      env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3, env->codegen_button, env->collect_button, env->atlas, env->atlas_bm);
					      
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
		    env->gui->set_pos(env->display, 200.0, 100.0);
		    env->display_visible = true;
		    g_env->ev->mainloop_api.end_editor_state();
		    
		  }
		env->progress_visible = false;
		
	      }
	  }
      }
  
    
    if (!env->editor_visible && !env->license_dialog_visible)
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
	      std::string uid = popup_uid;
	      env->dialog_uid = uid;
	      env->vec4.clear();
	      env->edit_data.clear();
	      std::vector<std::string> types;
	      types = env->ev->mod_api.types_from_function(*env->ev,env->mod, 0, uid);
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
	      labels = env->ev->mod_api.labels_from_function(*env->ev,env->mod, 0, uid);
	      std::vector<std::string*> refs;
	      refs = env->ev->mod_api.refs_from_function(*env->ev,env->mod, 0, uid);
	      
	      //std::cout << labels << " " << refs << std::endl;
	      assert(refs.size()==labels.size());
	      assert(types.size()==labels.size());
	      for(int e=0;e<s;e++)
		{
		  std::string *ref = refs[e];
		  env->gui->string_to_generic(*env->vec4[e], types[e], *ref); 
		}
	      
	      
	      
	      env->enum_click_targets = std::vector<GameApi::W>();
	      env->editor = env->gui->edit_dialog(*env->ev,labels,env->vec4,env->atlas3, env->atlas_bm3, types, env->dialog_cancel, env->dialog_ok, env->atlas2, env->atlas_bm2,env->enum_click_targets);
	      env->enum_types = types;
	      env->gui->set_pos(env->editor, 200,100);
	      
	      env->editor_visible = true;
		    
	    }
	  }
      }
  
    PT cursor_pos = e.cursor_pos;
    

    bool update = true;
    bool update2 = true;
    if (enum_popup.id!=0) update2=false;
    int s55 = env->enum_click_targets.size();
    for(int i=0;i<s55;i++)
      {
	if (enum_editor_callback(*env->env, *env->gui, env->enum_click_targets[i], env->enum_types[i], env->ev->point_api.pt_x(cursor_pos), env->ev->point_api.pt_y(cursor_pos), env->atlas2, env->atlas_bm2, 5, env->areas, e.button,e.type)) break;
	
	
      }
    
    

    env->gui->update(env->line, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
    if (enum_popup.id!=0 && update) {
      env->gui->update(enum_popup, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);

      
    }
	
    if (env->popup_visible && enum_popup.id==0 && update)
      env->gui->update(env->popup_dialog, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
    if (env->display_visible && enum_popup.id==0 && update)
      {
	env->gui->update(env->display, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
      }
    if (env->license_dialog_visible && enum_popup.id==0 && update)
      {
	env->gui->update(env->license_dialog, cursor_pos, e.button, e.ch,e.type,e.mouse_wheel_y);
      }
    if (env->editor_visible && enum_popup.id==0 && update)
      env->gui->update(env->editor, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
    
    if (enum_popup.id!=0) update=false;
    if (env->popup_visible) update=false;
    if (env->display_visible) update=false;
    if (env->license_dialog_visible) update=false;
    if (env->editor_visible) update=false;
    

	
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
    
    if (e.button == 0 && enum_popup.id!=0) { 
      PT pos = cursor_pos;
      int x = env->ev->point_api.pt_x(pos);
      int y = env->ev->point_api.pt_y(pos);
      int px = env->gui->pos_x(enum_popup);
      int py = env->gui->pos_y(enum_popup);
      int sx = env->gui->size_x(enum_popup);
      int sy = env->gui->size_y(enum_popup);
      
      if (x>=px && y>=py && x<px+sx && y<py+sy)
	{
	  /* do nothing */
	}
      else
	{
	  enum_popup.id = 0; 
	}
    }

    

    if (update) {
      env->gui->update(env->scroll_area, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
      
      env->gui->update(env->canvas_area, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
      if (env->license_dialog_visible) {
	env->gui->update(env->license_dialog_canvas_area, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
      }
      env->gui->update(env->scrollbar_x, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
      if (env->license_dialog_visible) {
	env->gui->update(env->license_dialog_scrollbar, cursor_pos, e.button, e.ch,e.type,e.mouse_wheel_y);
      }
      
      env->gui->update(env->list_tooltips, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	
      
      if (env->insert_ongoing)
	{
	  env->gui->update(env->insert_widget, cursor_pos, e.button,e.ch, e.type, e.mouse_wheel_y);
	}
      if (env->connect_ongoing)
	{
	  env->gui->update(env->connect_widget, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	  env->gui->update(env->connect_line, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
	}
    }
    if (env->has_wayland)
      {
	env->gui->update(env->window_decoration, cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
      }

    if (e.button==0 && e.type==1025 && !update2)
      {
	int s = env->areas.size();
	for(int i=0;i<s;i++)
	  {
	    //std::cout << "Area" << i << std::endl;
	    W wid = env->areas[i];
	    env->gui->update(wid,cursor_pos,e.button,e.ch,e.type,e.mouse_wheel_y);
	    int clicked = env->gui->chosen_item(wid);
	    //std::cout << "Area " << i << "==" << clicked << std::endl;
	    if (clicked==0)
	      {
		//std::cout << "Clicked" << i << std::endl;
		enum_popup.id=0;
		enum_set_value(*env->env,enum_click, i);
		
	      }
	  }
      }
    
    if (e.button==0 && update)
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
		if (sz<0||sz>25) sz=1;
		if (j<0||j>25) j=0;
		
		//std::cout << "Connect_click: " << j << " " << sz << std::endl;
		W canvas_item = env->gui->find_canvas_item(env->canvas, uid);
		if (canvas_item.id==-1) continue;
		
		BM bm = env->ev->bitmap_api.newbitmap(2,2);
		W ico_1 = env->gui->icon(bm);
		env->connect_widget = env->gui->insert_widget(ico_1, std::bind(&connect_target, _1, _2, env));
		
		int yy = 16+16+5+4+(env->gui->size_y(canvas_item)-16-16)*j/sz;
		if (sz<=1) yy=16+5+4+(env->gui->size_y(canvas_item)-16-16)/2;
		env->connect_line = env->gui->line(canvas_item, env->gui->size_x(canvas_item),yy /*16+16+5+4+(env->gui->size_y(canvas_item)-16-16)*j/sz*/,
						   env->connect_widget, 0, 0, env->sh2, env->sh);
		
		env->connect_start_uid = uid;
		env->connect_start_j = j;
		env->connect_start_sz = sz;
		env->connect_ongoing = true;
		break;
	      }
	    
	  }
	
      }
  
    
    if (update)
       env->ev->mod_api.update_lines_from_canvas(env->canvas, env->mod, 0);
    
    //int area_y = env->gui->size_y(env->array);
    //std::cout << area_y << std::endl;
    //env->gui->set_dynamic_param(env->txt2, 0, area_y);
    
    if (env->editor_visible && enum_popup.id==0 && update2)
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
	    refs = env->ev->mod_api.refs_from_function(*env->ev,env->mod, 0, uid);
	    std::vector<std::string> types;
	    types = env->ev->mod_api.types_from_function(*env->ev,env->mod, 0, uid);
	    
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
    if (sel != -1 && e.button==0 && e.type==1025 && !env->insert_ongoing && update)
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
	    //std::cout << "SEL: " << sel << " " << sel2 << std::endl;
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
		name = env->gui->shaderapi_functions_item_label(sel2-1);
		break;
	      case 6:
		name = env->gui->linesapi_functions_item_label(sel2-1);
		break;
	      case 7:
		name = env->gui->pointsapi_functions_item_label(sel2-1);
		break;
	      case 8:
		name = env->gui->moveapi_functions_item_label(sel2-1);
		break;
	      case 9:
		name = env->gui->pointapi_functions_item_label(sel2-1);
		break;
	      case 10:
		name = env->gui->vectorapi_functions_item_label(sel2-1);
		break;
		
	      case 11:
		name = env->gui->volumeapi_functions_item_label(sel2-1);
		break;
	      case 12:
		name = env->gui->floatvolumeapi_functions_item_label(sel2-1);
		break;
	      case 13:
		name = env->gui->colorvolumeapi_functions_item_label(sel2-1);
		break;
	      case 14:
		name = env->gui->fontapi_functions_item_label(sel2-1);
		break;
	      case 15:
		name = env->gui->textureapi_functions_item_label(sel2-1);
		break;
	      case 16:
		name = env->gui->booleanopsapi_functions_item_label(sel2-1);
		break;
	      case 17:
		name = env->gui->polygondistapi_functions_item_label(sel2-1);
		break;
	      case 18:
		name = env->gui->waveformapi_functions_item_label(sel2-1);
		break;
	      case 19:
		name = env->gui->blockerapi_functions_item_label(*env->ev,sel2-1);
		break;
	      case 20:
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
	    W ww = { -2 };
	    std::vector<W*> conn;
	    int ssk = 8;
	    std::vector<int> conn2;
	    for(int i=0;i<ssk;i++) {
	      env->connect_clicks.push_back(ww);
	      conn2.push_back(env->connect_clicks.size()-1);
	    }
	    for(int i=0;i<ssk;i++) {
	      conn.push_back(&env->connect_clicks[conn2[i]]);
	    }
	    int uid_num = env->unique_id_counter;
	    //std::cout << "unique_id" << uid_num << std::endl;
	    std::stringstream ss;
	    ss << "uid" << uid_num;
	    std::string uid = ss.str();
	    
	    env->chosen_item = env->ev->mod_api.inserted_widget(*env->ev,*env->gui, env->mod, 0, env->atlas, env->atlas_bm, name, conn /*env->connect_clicks[env->connect_clicks.size()-1]*/, uid, env->connect_targets);
	    int sk = env->connect_clicks.size()-1;
	    for(;sk>=0;sk--) { if (env->connect_clicks[sk].id==-2) env->connect_clicks.pop_back(); else break; }
	    env->insert_widget = env->gui->insert_widget(env->chosen_item, std::bind(&callback_func, _1, _2, env));
	    env->insert_ongoing = true;
	  }
      }
    if (update) {
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
    }
    float param_x1 = env->gui->dynamic_param(env->scrollbar_x, 0);
    env->gui->set_dynamic_param(env->canvas_area, 0, param_x1);

    if (env->license_dialog_visible)
      {
	// todo, check if this works.
	float param_y1 = env->gui->dynamic_param(env->license_dialog_scrollbar, 0);
	env->gui->set_dynamic_param(env->license_dialog_canvas_area, 1, param_y1);

	
      }

    
  }
private:
  bool properties_button = false;
  bool codegen_button = false;
  bool pkggen_button = false;
  bool display_button = false;
  std::string popup_uid;
  bool ready_to_render=true;
  uint64_t t0;
};


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

#ifdef LINUX
#include <execinfo.h>
#endif

void print_stack_trace()
{
#ifdef LINUX
    const int maxFrames = 64;
    void* frames[maxFrames];
    int frameCount = backtrace(frames, maxFrames);
    char** symbols = backtrace_symbols(frames, frameCount);
    
    std::cerr << "Stack trace:\n";
    for (int i = 0; i < frameCount; ++i)
    {
        std::cerr << symbols[i] << std::endl;
    }

    free(symbols);
#endif

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

IMPORT extern std::string g_original_title;

void terminate_handler()
{
  //print_counters();
  print_stack_trace();
  std::cout << "Terminate Handler called!" << std::endl;
  //for(;;);
}
IMPORT void clear_counters();
IMPORT void print_counters();
IMPORT bool file_exists(std::string s);

IMPORT extern bool g_vr_enable;
IMPORT extern int g_vr_device_id;
//extern pthread_t g_main_thread;

IMPORT extern bool g_progress_callback_set;
IMPORT extern void (*g_progress_callback)();

void update_progress_dialog_cb_impl(GameApi::W &w, int x,int y, GameApi::FtA f, GameApi::BM b, std::vector<std::string>, int val, int max);
extern void (*update_progress_dialog_cb)(GameApi::W &, int,int, GameApi::FtA, GameApi::BM, std::vector<std::string>, int val, int max);

class ASyncTask
{
public:
  virtual int NumTasks() const=0;
  virtual void DoTask(int i)=0;
};

void MoveCB(void* data)
{
  Envi *env = (Envi*)data;
  //env->ev->mod_api.update_lines_from_canvas(env->canvas, env->mod, 0);
  //std::cout << "update" << std::endl;
}

class StartMainTask : public ASyncTask
{
public:
  StartMainTask(Env &e, EveryApi &ev, Envi &env, SH sh,SH sh_2d,SH sh_arr,SH sh2, SH sh3, int screen_x, int screen_y, std::string filename, int argc, char **argv) : e(e), ev(ev), env(env), sh(sh),sh_2d(sh_2d),sh_arr(sh_arr), sh2(sh2), sh3(sh3),gui(e,ev,sh),screen_x(screen_x), screen_y(screen_y), filename(filename), argc(argc), argv(argv) { }
  virtual int NumTasks() const { return 7; }
  virtual void DoTask(int i) {
    //std::cout << "DOTASK:" << i << std::endl;
    //ev.mainloop_api.make_current();
    switch(i) {
    case 0:
      {
	srand(time(NULL));
	
	
	update_progress_dialog_cb = &update_progress_dialog_cb_impl;
	 
	//pid_t pid = getpid();
	//std::cout << "pid: " << (long)pid << std::endl;
	InstallProgress(888,"init",10);
	//ProgressBar(888,0,10,"init");
	set_current_block(-2);
	
	//g_main_thread = pthread_self();
	
	std::set_terminate(&terminate_handler);
	srand(time(NULL));
	
	
#ifdef WINDOWS
	//env.dlls = load_dlls("DllList.txt");
#else
	//env.dlls = load_dlls("DllList_linux.txt");
#endif

	std::ifstream stream(filename.c_str());
	if (!stream.is_open())
	  {
	    std::ofstream stream2(filename.c_str());
	    stream2 << "\n\ntest" << std::flush;
	    stream2.close();
	  }
	

	mod = ev.mod_api.load(ev,filename);
	
		break;
	 }
	case 1:
	 {
      has_wayland = false;
      
      const char *ee = getenv("XDG_SESSION_TYPE");
      if (ee)
	std::cout << "SESSION TYPE:" << ee << std::endl;

      const char *driver = g_low->sdl->SDL_GetCurrentVideoDriver();
      if (driver)
	std::cout << "VIDEODRIVER:" << driver << std::endl;

      g_videodriver = driver;

#if 0
      if (g_gpu_vendor == std::string("NVID") && (getenv("__GL_SYNC_TO_VBLANK")==0 ||std::string(getenv("__GL_SYNC_TO_VBLANK"))=="1"))
	{
	  std::cout << "Warning: You should do: export __GL_SYNC_TO_VBLACK=0 to run gameapi-builder in nvidia gpu cards" << std::endl;
	}
#endif
      
      
      if (driver && std::string(driver)=="wayland")
	{
	  std::cout << "WARNING: you should use 'export SDL_VIDEODRIVER=x11' with gameapi-builder in wayland." << std::endl;
	}
      
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
      
      extra_width = 0;
      extra_height = 0;
      extra_top = 0;
      extra_left = 0;
      
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
  if (has_wayland)
    {
      display_width = g_display_width;
      display_height = g_display_height;
    }
  
  float font_scale = 1.5;

  ProgressBar(888,0,5,"init");
  // shader initialization
  // Chunkfive.otf
  font = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 8*font_scale,12*font_scale); // 13,15 
  font2 = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 8*font_scale,12*font_scale); // 10,13
  font3 = ev.font_api.newfont("http://tpgames.org/Chunkfive.otf", 20*font_scale,20*font_scale); // 30,30

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
	  //ev.mainloop_api.save_logo(ev);
	  std::cout << "Generating font atlas. " << std::endl;
	  std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!\"#¤%&/()=?+\\*^.,-<>|§½;:[]_ $";

	  //std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-();:_*/%+><[]";
	  FtA atlas = ev.font_api.font_atlas_info(ev, font, chars, 8*font_scale,12*font_scale, 25*font_scale);
	  FtA atlas2 = ev.font_api.font_atlas_info(ev, font2, chars, 8*font_scale,12*font_scale, 25*font_scale);
	  FtA atlas3 = ev.font_api.font_atlas_info(ev, font3, chars, 20*font_scale,20*font_scale, 65*font_scale);
	  BM atlas_bm = ev.font_api.font_atlas(ev, font, atlas, 8*font_scale,12*font_scale);
	  BM atlas_bm2 = ev.font_api.font_atlas(ev,font2, atlas2, 8*font_scale,12*font_scale);
	  BM atlas_bm3 = ev.font_api.font_atlas(ev,font3, atlas3, 20*font_scale,20*font_scale);
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
  
  if (file_exists("./atlas0.txt")) {
    a_atlas0 = "atlas0.txt";
    a_atlas1 = "atlas1.txt";
    a_atlas2 = "atlas2.txt";
    a_atlas_bm0 = "atlas_bm0.ppm";
    a_atlas_bm1 = "atlas_bm1.ppm";
    a_atlas_bm2 = "atlas_bm2.ppm";
  } else {
    a_atlas0 = "/usr/share/atlas0.txt";
    a_atlas1 = "/usr/share/atlas1.txt";
    a_atlas2 = "/usr/share/atlas2.txt";
    a_atlas_bm0 = "/usr/share/atlas_bm0.ppm";
    a_atlas_bm1 = "/usr/share/atlas_bm1.ppm";
    a_atlas_bm2 = "/usr/share/atlas_bm2.ppm";
  }
#else
  std::string a_atlas0 = "atlas0.txt";
  std::string a_atlas1 = "atlas1.txt";
  std::string a_atlas2 = "atlas2.txt";
  std::string a_atlas_bm0 = "atlas_bm0.ppm";
  std::string a_atlas_bm1 = "atlas_bm1.ppm";
  std::string a_atlas_bm2 = "atlas_bm2.ppm";
#endif
  
  atlas = ev.font_api.load_atlas(a_atlas0);
  atlas2 = ev.font_api.load_atlas(a_atlas1);
  atlas3 = ev.font_api.load_atlas(a_atlas2);
  atlas_bm = ev.bitmap_api.loadbitmap(a_atlas_bm0);
  atlas_bm2 = ev.bitmap_api.loadbitmap(a_atlas_bm1);
  atlas_bm3 = ev.bitmap_api.loadbitmap(a_atlas_bm2);
  
    break;
    }
    case 2:
      {
  
  g_everyapi_gui = &gui;
  env.gui = &gui;

 
  env.atlas = atlas;
  env.atlas2 = atlas2;
  env.atlas3 = atlas3;
  env.atlas_bm = atlas_bm;
  env.atlas_bm2 = atlas_bm2;
  env.atlas_bm3 = atlas_bm3;

  g_progress_callback = &refresh;
  g_progress_callback_set = true;

  env.progress_visible = true;
  g_env = &env;
  env.progress_rest = false;
  
  env.progress_visible = true;
  g_prog_labels = std::vector<std::string>();
  g_prog_labels.push_back("");
  g_everyapi2 = env.ev;
  g_atlas = env.atlas;
  g_atlas_bm = env.atlas_bm;
  
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
      items.push_back(gui.shaderapi_functions_list_item(atlas,atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
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
      items.push_back(gui.blockerapi_functions_list_item(ev,atlas, atlas_bm, atlas2, atlas_bm2, env.list_tooltips));
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
  array = gui.array_y(&items[0], items.size(), 5);
  scroll_area = gui.scroll_area(array, gui.size_x(array), screen_y-30, screen_y);
  // scroll_area2 = gui.bounding_box(scroll_area);
  
  //W txt2 = gui.scrollbar_y(20, screen_y-30, gui.size_y(array));
  //gui.set_pos(txt2, gui.size_x(scroll_area), 30);

  gui.set_pos(scroll_area, extra_left, extra_top);
  gui.set_size(scroll_area, 140.0, screen_y);
  //gui.set_pos(scroll_area2, extra_left, extra_top);
  //gui.set_size(scroll_area2, 140.0, screen_y);
  //W wave = gui.waveform(f, 0.0, 3.14159*2.0, -1.5, 1.5, 200, 100, 0xffffffff, 0x00000000);

  std::vector<std::string> params;
  params.push_back("BM");
  params.push_back("BM");
  //W gameapi = gui.canvas_item_gameapi_node(100,100, "subbitmap", params, "BM", font2);
  // W gameapi_2 = gui.mouse_move(gameapi, 0,0,gui.size_x(gameapi), gui.size_y(gameapi));
  //gui.set_pos(gameapi_2, 200.0, 200.0);

  sx = 1638;
  sy = 1638;
  screen2_x = screen_x-140;
  screen2_y = screen_y-30;

  canvas = gui.canvas(16384,16384);
  //gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), 100,100);
  //gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), 150,100);
  //for(int i=0;i<200;i++)
  //  gui.canvas_item(canvas, gui.button(30,30, 0xffffffff, 0xff888888), i*30, i*30);
  ProgressBar(888,5,5,"init");
  break;
      }
    case 3:
      {
	ev.mod_api.insert_to_canvas(ev,gui, canvas, mod, 0, atlas, atlas_bm, env.connect_clicks, env.connect_targets, env.display_clicks, env.edit_clicks, env.delete_key, env.codegen_button_vec, env.popup_open, &MoveCB,(void*)&env);

  break;
      }
    case 4:
      {
  
  //ProgressBar(888,7,10,"init");
  ev.mod_api.insert_links(ev, gui, mod, 0, env.connect_links, canvas, env.connect_targets, sh2, sh);
  break;
      }
   case 5:
      {
  //ProgressBar(888,8,10,"init");
  add_to_canvas(gui, canvas, env.connect_links);
  //print_connect_targets("init", &env);
  //ProgressBar(888,9,10,"init");
  break;
     }
    case 6:
     {
  canvas_area = gui.scroll_area(canvas, screen2_x, screen2_y, screen_y);
  //W scrollbar_y = gui.scrollbar_y(20, screen2_y-20, sy);
  scrollbar_x = gui.scrollbar_x(screen2_x-20, 20, sx); 

  line = gui.rectangle(0.0, 4.0, 0.0, screen_y, 0xffffffff);
  decoration = gui.window_decoration(screen_x,screen_y, "GameApi Builder -- meshpage.org", atlas, atlas_bm);
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
  //env.txt = txt;
  //env.txt2 = txt2;
  env.font = font;
  env.font3 = font3;
  env.unique_id_counter = ev.mainloop_api.random();
  //env.editor = editor;
  env.editor_visible = false;
  env.display = gui.empty();
  env.mem = gui.empty();
  env.display_visible = false;
  env.license_dialog_visible=false;
  env.progress_visible = false;
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
  // env.scroll_area2 = scroll_area2;
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



  env.progress_visible = false;
  env.progress_rest = true;
  env.envi_ready = true;
  break;
      }
    };
  }
private:
  Env &e;
  EveryApi &ev;
  Envi &env;
  SH sh;
  SH sh_2d;
  SH sh_arr;
  SH sh2;
  SH sh3;
  GuiApi gui; //(e, ev, sh);
  WM mod;
  int screen_x, screen_y;
  FtA atlas, atlas2, atlas3;
  BM atlas_bm, atlas_bm2, atlas_bm3;
  std::string filename;
  bool has_wayland=false;
  int extra_width, extra_height;
  int extra_top, extra_left;
  W scroll_area;
  //W scroll_area2;
  int screen2_x,screen2_y;
  W array;
  Ft font, font2, font3;
  int sx, sy;
  W canvas;
  int argc;
  char **argv;
  W line;
  W decoration;
  W canvas_area;
  W scrollbar_x;
};

struct Envi_tabs
{
  bool envi_ready=false;
  Env *env;
  EveryApi *ev;
  GuiApi *gui = 0;

  std::vector<std::string> titles;
  W navi_bar= {-1 };
  W back_button;
  W forward_button;
  W save_button;
  W url_button;
  //---
  W download_bar = { -1 };
  std::vector<W> db_close_button;
  std::vector<W> db_buttons;
  int db_active_tab=0;
  int old_download_bar_count=0;
  float old_download_bar_perc=0.0;
  //---
  std::vector<W> close_button;
  std::vector<W> tab_change_button;
  W new_tab_button;
  FtA atlas;
  BM atlas_bm;
  FtA atlas2;
  BM atlas_bm2;
  FtA atlas3;
  BM atlas_bm3;
  bool has_wayland = false;
  SH sh;
};

class Tabs_Gui : public ASyncTask
{
public:
  Tabs_Gui(Env &e, EveryApi &ev, Envi_tabs &env, SH sh, SH sh_2d, SH sh_arr, SH  sh2, SH sh3,int screen_x, int screen_y, std::vector<std::string> *filenames, int *active_tab) : e(e), ev(ev), env(&env), sh(sh), sh_2d(sh_2d), sh_arr(sh_arr), sh2(sh2), sh3(sh3), screen_x(screen_x), screen_y(screen_y), filenames(filenames), active_tab(active_tab) {
    gui = new GuiApi(e,ev,sh);
    env.gui=gui;
  } 
  virtual int NumTasks() const
  {
    return 3;
  }
  virtual void DoTask(int i)
 {
    switch(i) {
    case 0:
      {
#ifdef LINUX
  std::string a_atlas0;
  std::string a_atlas1;
  std::string a_atlas2;
  std::string a_atlas_bm0;
  std::string a_atlas_bm1;
  std::string a_atlas_bm2;
  
  if (file_exists("./atlas0.txt")) {
    a_atlas0 = "atlas0.txt";
    a_atlas1 = "atlas1.txt";
    a_atlas2 = "atlas2.txt";
    a_atlas_bm0 = "atlas_bm0.ppm";
    a_atlas_bm1 = "atlas_bm1.ppm";
    a_atlas_bm2 = "atlas_bm2.ppm";
  } else {
    a_atlas0 = "/usr/share/atlas0.txt";
    a_atlas1 = "/usr/share/atlas1.txt";
    a_atlas2 = "/usr/share/atlas2.txt";
    a_atlas_bm0 = "/usr/share/atlas_bm0.ppm";
    a_atlas_bm1 = "/usr/share/atlas_bm1.ppm";
    a_atlas_bm2 = "/usr/share/atlas_bm2.ppm";
  }
#else
  std::string a_atlas0 = "atlas0.txt";
  std::string a_atlas1 = "atlas1.txt";
  std::string a_atlas2 = "atlas2.txt";
  std::string a_atlas_bm0 = "atlas_bm0.ppm";
  std::string a_atlas_bm1 = "atlas_bm1.ppm";
  std::string a_atlas_bm2 = "atlas_bm2.ppm";
#endif
 
  env->atlas = ev.font_api.load_atlas(a_atlas0);
  env->atlas2 = ev.font_api.load_atlas(a_atlas1);
  env->atlas3 = ev.font_api.load_atlas(a_atlas2);
  env->atlas_bm = ev.bitmap_api.loadbitmap(a_atlas_bm0);
  env->atlas_bm2 = ev.bitmap_api.loadbitmap(a_atlas_bm1);
  env->atlas_bm3 = ev.bitmap_api.loadbitmap(a_atlas_bm2);
  break;
      }
    case 1:
      {
	//env->titles = std::vector<std::string>();
      //env->titles.push_back(filename);
      //env->titles.push_back("testtab1");
      //env->titles.push_back("testtab2");
      //env->titles.push_back("testtab3");
      //int s = filenames.size();
      //for(int i=0;i<s;i++) env->titles.push_back(filenames[i]);
      break;
      }
    case 2:
      {
	env->close_button = std::vector<W>();
	env->tab_change_button = std::vector<W>();
	env->navi_bar = gui->navi_bar(*env->ev,*filenames, env->back_button, env->forward_button, env->save_button, (*filenames)[*active_tab>=0&&*active_tab<(*filenames).size()?*active_tab:0], env->url_button, env->close_button, env->tab_change_button, env->new_tab_button, std::vector<std::string>{}, std::vector<std::string>{}, env->atlas2, env->atlas_bm2,*active_tab);
	env->envi_ready=true;
	break;
      }
    case 3:
      {
	int count = env->env->download_bar_count();
	env->db_close_button = std::vector<W>();
	env->db_buttons = std::vector<W>();
	std::vector<std::string> titles;
	std::vector<float> perc;
	float new_perc=0.0;
	for(int i=0;i<count;i++)
	  {
	    titles.push_back(env->env->get_download_bar_filename(i));
	    if (env->env->get_download_bar_ready(i)) {
	      perc.push_back(-1.0);
	      new_perc+=100.0;
	    }
	    else {
	      perc.push_back(env->env->get_download_bar_progress(i));
	      new_perc+=env->env->get_download_bar_progress(i);
	    }
	  }
	env->download_bar = gui->download_bar(*env->ev, titles, env->db_close_button, env->db_buttons, env->atlas2, env->atlas_bm2, env->db_active_tab, perc);
	env->old_download_bar_count = count;
	env->old_download_bar_perc = new_perc;
      break;
      }
      
    };
  }
private:
  Env &e;
  EveryApi &ev;
  Envi_tabs *env;
  SH sh;
  SH sh_2d;
  SH sh_arr;
  SH sh2;
  SH sh3;
  GuiApi *gui;
  int screen_x, screen_y;
  std::vector<std::string> *filenames;
  std::string filename;
  int *active_tab;
};


void IterAlgo(Env &ee, std::vector<BuilderIter*> vec, std::vector<void*> args,EveryApi *ev)
{
  int s = vec.size();
  //std::cout << "IterAlgo " << s << std::endl;
  for(int i=0;i<s;i++)
    {
      if (vec[i])
	vec[i]->start(args[i]);
    }
  if (!event_lock) {
  int s2 = vec.size();
  for(int i=0;i<s2;i++)
    {
      if (vec[i])
	vec[i]->render(args[i]);
    }
  ev->mainloop_api.swapbuffers();

#ifdef WAYLAND
    if (!g_wl_display) {
    Low_SDL_SysWMinfo info;
  if (g_low->sdl->SDL_GetWindowWMInfo(sdl_window, &info))
    {
      g_wl_display = info.display;
      g_wl_surface = info.surface;
      g_wl_shell_surface = info.shell_surface;
    }
    }
#endif

    }
    MainLoopApi::Event e;
    e.ch = 0;
    e.last = true;
    event_lock=true;

    uint64_t t = get_time_us();
    if (t>event_lock_time+3000) event_lock=false;
    
    while((e=ev->mainloop_api.get_event()).last)
      {
	event_lock=false;
	int s3 = vec.size();
	for(int i=0;i<s3;i++)
	  {
	    if (vec[i])
	      vec[i]->update(args[i],e);
	  }
	
      }
    if (!event_lock) event_lock_time=get_time_us();
    
    ee.async_scheduler();    
}

Tabs_Gui *g_start;
Envi_tabs *g_start_tab;
IterTab *g_start2;

class TabsUpdateTask : public ASyncTask
{
public:
  TabsUpdateTask(Tabs_Gui *ptr) : ptr(ptr) { }
  virtual int NumTasks() const
  {
    return 1;
  }
  virtual void DoTask(int i) {
    if (i==0) ptr->DoTask(2); // navibar update
  }
private:
  Tabs_Gui *ptr;
};

class DownloadUpdateTask : public ASyncTask
{
public:
  DownloadUpdateTask(Tabs_Gui *ptr) : ptr(ptr) { }
  virtual int NumTasks() const
  {
    return 1;
  }
  virtual void DoTask(int i) {
    if (i==0) ptr->DoTask(3); // downloadbar update
  }
private:
  Tabs_Gui *ptr;
};


struct IterData
{
  EveryApi *ev;
  Env *env;
  Envi *envi;
  SH sh;
  SH sh_2d;
  SH sh_arr;
  SH sh2;
  SH sh3;
  int screen_x;
  int screen_y;
  std::string filename;
  int argc;
  char **argv;
  std::vector<std::string> *filenames;
};

class IterTab : public BuilderIter
{
public:
  IterTab(int *active_tab, std::vector<BuilderIter*> *nodes, std::vector<void*> *args,   std::vector<ASyncTask*> *perm_tasks, std::vector<BuilderIter*> *perm_nodes, std::vector<void*> *perm_args, IterData *dt
	  ) : active_tab(active_tab), nodes(nodes), args(args), perm_tasks(perm_tasks), perm_nodes(perm_nodes), perm_args(perm_args),dt(dt) { }
  void start(void *arg)
  {
    Envi_tabs *env = (Envi_tabs*)arg;
    //env->env->async_scheduler();
    
    //std::cout << "IterTab::start start" << std::endl;
    if (!env->envi_ready) return;
    //std::cout << "IterTab::start cont" << std::endl;
    
    env->ev->shader_api.use(env->sh);
    static bool firsttime = true;
    if (firsttime) {
      if (!event_lock) {
    if (env->has_wayland) {
      env->ev->mainloop_api.clear(0x00000000);
    } else {
      env->ev->mainloop_api.clear(0xff000000);
    }
      }
    firsttime = false;
    }
  }
  void render(void *arg) {
    Envi_tabs *env = (Envi_tabs*)arg;
    //std::cout << "IterTab::render start" << std::endl;
    if (!env->envi_ready) return;
    //std::cout << "IterTab::render cont" << std::endl;
  // render here
    if (env->gui && env->navi_bar.id!=-1)
      env->gui->render(env->navi_bar);

    if (env->gui && env->download_bar.id!=-1)
      env->gui->render(env->download_bar);

    counter++;
  }
  bool start_new(Envi_tabs *env, int i)
  {
    if ((*perm_tasks)[i]==0)
      { // must create new content
	Envi *new_envi = new Envi;
	new_envi->env = env->env;
	new_envi->ev = env->ev;
	//std::cout << "CHOOSE: " << (*dt->filenames)[i] << std::endl;
	(*perm_tasks)[i]=new StartMainTask(*dt->env,*dt->ev,*new_envi,dt->sh,dt->sh_2d,dt->sh_arr,dt->sh2,dt->sh3,dt->screen_x,dt->screen_y,(*dt->filenames)[i],dt->argc,dt->argv);
	(*perm_nodes)[i]=new MainIter;
	(*perm_args)[i]=new_envi;
	env->env->start_async((*perm_tasks)[i]);
	return true;
      }
    return false;
  }
  void update(void *arg, MainLoopApi::Event &e)
  {
    Envi_tabs *env = (Envi_tabs*)arg;
    //std::cout << "IterTab::update start" << std::endl;
    //std::cout << "A" << std::endl;
    if (!env->envi_ready) return;
    ///std::cout << "B" << std::endl;
    //std::cout << "IterTab::update cont" << std::endl;


	int count2 = env->env->download_bar_count();
	float perc = 0.0;
	for(int i=0;i<count2;i++)
	  {
	    if (env->env->get_download_bar_ready(i))
	      perc+=100.0;
	    else
	      perc+=env->env->get_download_bar_progress(i);
	  }

    
    int count = env->env->download_bar_count();
    if (count != env->old_download_bar_count || fabs(perc-env->old_download_bar_perc)>0.001)
      {
	env->env->start_async(new DownloadUpdateTask(g_start)); 
     }

    static bool flag=false;
    if (e.button != 0 && flag==true)
      g_dragdrop_enabled=false;
    if (e.button != 0) flag=true;
    else flag=false;
    
    int s5 = env->db_buttons.size();
    for(int i=0;i<s5;i++)
      {
	W w = env->db_buttons[i];
	int chosen = env->gui->chosen_item(w);
	if (state==0 && chosen==0 && e.button==0)
	  {
	    g_dragdrop_enabled=true;
#ifdef LINUX
	    std::string s = getenv("HOME");
	    g_dragdrop_filename = std::string("file://") + s + "/.gameapi_builder/Downloads/" + env->env->get_download_bar_filename(i);
#endif
#ifdef WINDOWS
	      std::string home = getenv("TEMP");


	      int s = home.size();
	      for(int i=0;i<s;i++)
		if (home[i]=='\\') home[i]='/';
	      
	    //std::string drive = getenv("systemdrive");
	    //std::string path = getenv("homepath");
	    g_dragdrop_filename = std::string("file://") + home + "/_gameapi_builder/Downloads/" + env->env->get_download_bar_filename(i);
#endif
	  }
	if (state==2 && chosen==0)
	  {
	    if (counter<200)
	      { // double-click on the download bar
#ifdef LINUX
		std::string s = getenv("HOME");
		pthread_system((std::string("xdg-open ")+s+"/.gameapi_builder/Downloads/").c_str());
#endif
#ifdef WINDOWS
#endif
	      }
	    state=0;
	  }
	else
	if (state==1 && chosen==-1)
	  {
	    state=2;
	  }
	else
	if (state==0 && chosen==0)
	  {
	    env->db_active_tab = i;
	    env->env->start_async(new DownloadUpdateTask(g_start));
	    state=1;
	    counter=0;
	  }


      }

    int s6 = env->db_close_button.size();
    for(int i=0;i<s6;i++)
      {
	W w = env->db_close_button[i];
	int chosen = env->gui->chosen_item(w);
	if (chosen==0)
	  {
	    env->env->remove_download_bar_item(i);
	    if (i<env->db_active_tab) { env->db_active_tab--; }
	    env->env->start_async(new DownloadUpdateTask(g_start));
	  }
      }

    
    
    W w2 = env->new_tab_button;
    int chosen2 = env->gui->chosen_item(w2);
    if (chosen2==0)
      {
	int ii = 0;
	std::string s;
	do {
	  ii++;
	  std::stringstream ss;
	  ss << ii;

	  std::string out;
#ifdef WINDOWS
	std::string drive = getenv("systemdrive");
	std::string path = getenv("homepath");
	//std::string filename = drive+path+"\\mod.txt";
	out = drive+path+"\\";
#else
	const char *dd = getenv("BUILDER_DOCKER_DIR");
	std::string dockerdir = dd?dd:"";
	//std::string filename = dockerdir + "mod.txt";
	out = dockerdir;
#endif

	  
	  s = out+std::string("mod") + ss.str() + ".txt";
	} while(file_exists(s));
	
	Envi *new_envi = new Envi;
	new_envi->env = env->env;
	new_envi->ev = env->ev;
	//std::cout << "CHOOSE: " << (*dt->filenames)[i] << std::endl;
	(*dt->filenames).push_back(s);
	(*perm_tasks).push_back(new StartMainTask(*dt->env,*dt->ev,*new_envi,dt->sh,dt->sh_2d,dt->sh_arr,dt->sh2,dt->sh3,dt->screen_x,dt->screen_y,s /*(*dt->filenames)[i]*/,dt->argc,dt->argv));
	(*perm_nodes).push_back(new MainIter);
	(*perm_args).push_back(new_envi);
	int pos = (*perm_tasks).size()-1;
	env->env->start_async((*perm_tasks)[pos]);	
	(*nodes)[0]=(*perm_nodes)[pos];
	(*args)[0]=(*perm_args)[pos];
	*active_tab = pos;
	env->env->start_async(new TabsUpdateTask(g_start));
      }
    
    int s2 = env->close_button.size();
    for(int i=0;i<s2;i++)
      {
	W w = env->close_button[i];
	int chosen = env->gui->chosen_item(w);
	if (chosen==0)
	  {
	    //std::cout << "ERASE:" << i << std::endl;
	    (*perm_tasks).erase((*perm_tasks).begin()+i);
	    (*perm_nodes).erase((*perm_nodes).begin()+i);
	    (*perm_args).erase((*perm_args).begin()+i);
	    dt->filenames->erase(dt->filenames->begin()+i);

	    if (i==*active_tab)
	      {
		int num = i;
		if (num>=(*perm_nodes).size()) { num--; (*active_tab)--; }
		if (num<0) exit(0);
		(*nodes)[0]=(*perm_nodes)[num];
		(*args)[0]=(*perm_args)[num];

		if (!((*nodes)[0]))
		  {
		    if (!start_new(env,num)) { g_env=(Envi*)((*perm_args)[num]); }
		    (*nodes)[0]=(*perm_nodes)[num];
		    (*args)[0]=(*perm_args)[num];
		  }
	      } else
	      if (i<*active_tab) { (*active_tab)--; }
	    env->env->start_async(new TabsUpdateTask(g_start));
	  }
      }

    
    int s = env->tab_change_button.size();
    for(int i=0;i<s;i++)
      {
	W w = env->tab_change_button[i];
	int chosen = env->gui->chosen_item(w);
	if (chosen==0)
	  {
	    if (!start_new(env,i)) { g_env = (Envi*)((*perm_args)[i]); }

	    (*nodes)[0]=(*perm_nodes)[i];
	    (*args)[0]=(*perm_args)[i];

	    *active_tab = i;
	    env->env->start_async(new TabsUpdateTask(g_start));
      
	  }
      }

    if (env->navi_bar.id!=-1)
      env->gui->update(env->navi_bar, e.cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
    if (env->download_bar.id!=-1)
      env->gui->update(env->download_bar, e.cursor_pos, e.button, e.ch, e.type, e.mouse_wheel_y);
  }  
private:
  int *active_tab;
  std::vector<BuilderIter*> *nodes;
  std::vector<void*> *args;
  std::vector<ASyncTask*> *perm_tasks;
  std::vector<BuilderIter*> *perm_nodes;
  std::vector<void*> *perm_args;
  IterData *dt;
  int state=0;
  int counter = 0;
};

void render_cb(Envi *env)
{
  //std::cout << "render_cb" << std::endl;
   if (env->progress_rest) {
     //std::cout << "progress_rest=true" << std::endl;
  
  if (env->has_wayland) {
    //std::cout << "has_wayland=true" << std::endl;
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
    if (env->display_visible)
      {
	env->gui->render(env->display);
      }
    if (env->license_dialog_visible)
      {
	env->gui->render(env->license_dialog);
      }
    
    if (env->editor_visible)
      env->gui->render(env->editor);
   }

  enum_editor_draw(*env->ev, *env->gui);

    if (env->progress_visible)
      {
	env->gui->set_pos(g_progress_dialog, 400,300);
  PT cursor_pos = old_cursor_pos;
	int button = -1;
	int ch = -1;
	int type = 0;
	int mouse_wheel_y = 0;
	env->gui->update(g_progress_dialog, cursor_pos, button, ch, type, mouse_wheel_y);

	env->gui->render(g_progress_dialog);
      }

}

void refresh()
{
  if (event_lock) return;
  if (g_progress_halt) return;
  if (g_env && g_env->progress_visible) {
  g_env->ev->shader_api.use(g_env->sh);

  if (!event_lock) {
  if (g_env->has_wayland) {
    g_env->ev->mainloop_api.clear(0x00000000);
  } else {
    g_env->ev->mainloop_api.clear(0xff000000);
  }
  }
  g_env->ev->mainloop_api.start_editor_state();
  PT cursor_pos = old_cursor_pos;
	int button = -1;
	int ch = -1;
	int type = 0;
	int mouse_wheel_y = 0;
	if (g_env->progress_rest) {
  g_env->gui->update(g_env->canvas_area, cursor_pos, button,ch, type, mouse_wheel_y);
  if (g_env->license_dialog_visible)
    {
      g_env->gui->update(g_env->license_dialog_canvas_area, cursor_pos, button,ch,type,mouse_wheel_y);
    }
	}

  render_cb(g_env);
  g_start2->render(g_start_tab);
  g_env->ev->mainloop_api.end_editor_state();
  g_env->ev->mainloop_api.swapbuffers();

#ifdef WAYLAND
    if (!g_wl_display) {
    Low_SDL_SysWMinfo info;
  if (g_low->sdl->SDL_GetWindowWMInfo(sdl_window, &info))
    {
      g_wl_display = info.display;
      g_wl_surface = info.surface;
      g_wl_shell_surface = info.shell_surface;
    }
    }
#endif

  }
}

IMPORT extern std::string gameapi_temp_dir;
#ifdef LINUX
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/ocl.hpp>
#endif
#ifdef WINDOWS
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <wrl/client.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#define INITGUID
#include <initguid.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>


#endif


int main(int argc, char *argv[]) {
#ifdef LINUX
  try {
#endif
#ifdef WINDOWS
    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {std::cout << "Warning: CoInitializeEx failed! => videofile fails..." << std::endl; }
#endif
#ifdef LINUX
  cv::setNumThreads(10);
  cv::ocl::setUseOpenCL(false);
#endif
 g_main_thread_id = pthread_self();
	tasks_init();
	
	Env *e2 = new Env;
  Env &e = *e2;
  EveryApi ev(*e2);
  Envi *env = new Envi;
  Envi_tabs *env_tab = new Envi_tabs;
  g_start_tab = env_tab;
  env->env = e2;
  env->ev = &ev;
  env_tab->env = e2;
  env_tab->ev = &ev;
  g_env = env;


  int screen_x = 1200;
	int screen_y = 900;
#ifdef WINDOWS
	std::string drive = getenv("systemdrive");
	std::string path = getenv("homepath");
	std::string filename = drive+path+"\\mod.txt";
#else
	const char *dd = getenv("BUILDER_DOCKER_DIR");
	std::string dockerdir = dd?dd:"";
	std::string filename = dockerdir + "mod.txt";
#endif
	std::vector<std::string> filenames;
	
	//std::cout << "Loading:" << filename << std::endl;
	for(int i=1;i<argc;i++)
	  {
	    if (std::string(argv[i])=="--help"||std::string(argv[i])=="-h"||std::string(argv[i])=="-?"||std::string(argv[i])=="/?")
	      {
		std::cout << "Usage: gameapi-builder <options>" << std::endl;
		std::cout << "Possible options:" << std::endl;
		std::cout << "  --file mod.txt    (open a file for editing)" << std::endl;
		std::cout << "  --mg, --huge, --mega, --small (window size)" << std::endl;
		std::cout << "  --vr oculus  (choose oculus for vr)" << std::endl;
		std::cout << "  --vr vive    (choose vive for vr)" << std::endl;
		std::cout << "  --dump (list all functions)" << std::endl;
		std::cout << "  --tempdir <dir> (choose temporary dir)" << std::endl;
		exit(0);
	      }
	    if (std::string(argv[i])=="--dump_count")
	      {
		int i = ev.mod_api.dump_functions_count();
		std::cout << i << std::endl;
		exit(0);
	      }
	    if (std::string(argv[i])=="--dump_get")
	      {
		std::string s2 = argv[i+1];
		std::stringstream ss(s2);
		int i;
		ss >> i;
		std::string s = ev.mod_api.dump_functions(ev,i);
		std::cout << s << std::endl;
		exit(0);
	      }
	    if (std::string(argv[i])=="--short")
	      {
		screen_x = 1080;
		screen_y = 1920;
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
		filenames.push_back(filename);
		i++;
	      }
	    if (std::string(argv[i])=="--dep")
	      {
		std::string filename = std::string(argv[i+1]);
		i++;
		std::string res = ev.mod_api.extract_deps(filename);
		std::ofstream ss("../GameApi_short_defines.hh");
		ss << res << std::endl;
		ss.close();
		exit(0);
	      }
	    if (std::string(argv[i])=="--tempdir")
	      {
		gameapi_temp_dir=std::string(argv[i+1]);
		i++;
	      }
	  }
	if (filenames.size()==0) filenames.push_back(filename);
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
      env_tab->has_wayland=has_wayland;

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
  if (has_wayland)
    {
      display_width = g_display_width;
      display_height = g_display_height;
    }

	

  env_tab->env = e2;
  env_tab->ev = &ev;
  
  //clear_counters();
  //SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T)-1);
  ev.mainloop_api.init_window(display_width,display_height,"GameApi Builder -- meshpage.org");
  ev.shader_api.load_default();
  SH sh = ev.shader_api.texture_shader();
  SH sh_2d = ev.shader_api.texture_shader();
  SH sh_arr = ev.shader_api.texture_shader();
  SH sh2 = ev.shader_api.colour_shader();
  SH sh3 = ev.shader_api.colour_shader();


  //ev.mainloop_api.alpha(true);
  //ev.mainloop_api.transparency(true);
  //ev.mainloop_api.depth_test(false);
  
  IterData iter_dt;
  iter_dt.ev = &ev;
  iter_dt.env = e2;
  iter_dt.envi = env;
  iter_dt.sh = sh;
  iter_dt.sh_2d = sh_2d;
  iter_dt.sh_arr = sh_arr;
  iter_dt.sh2 = sh2;
  iter_dt.sh3 = sh3;
  iter_dt.screen_x = screen_x;
  iter_dt.screen_y = screen_y;
  iter_dt.filename = filenames[0];
  iter_dt.argc = argc;
  iter_dt.argv = argv;
  iter_dt.filenames = &filenames;
  
  // rest of the initializations
  ev.mainloop_api.init_3d(sh3, display_width, display_height);
  ev.mainloop_api.init_3d(sh_arr, display_width, display_height);
  ev.mainloop_api.init(sh, display_width, display_height);
  ev.mainloop_api.init(sh2, display_width, display_height);
  ev.mainloop_api.init(sh_2d, display_width, display_height);
  /*
  ev.mainloop_api.switch_to_3d(false, sh3, screen_x+extra_width, screen_y+extra_height);
  ev.shader_api.use(sh);
  */
  ev.mainloop_api.alpha(true);
  int *active_tab = new int(0);


  std::vector<ASyncTask*> perm_tasks;
  std::vector<BuilderIter*> perm_nodes;
  std::vector<void*> perm_args;
  int s = filenames.size();
  for(int i=0;i<s;i++)
    {

      perm_tasks.push_back(0);
      perm_nodes.push_back(0);
      perm_args.push_back(0);
    }
  

  
  e.start_async(g_start=new Tabs_Gui(e,ev,*env_tab,sh,sh_2d,sh_arr,sh2,sh3,screen_x, screen_y, &filenames,active_tab));
  e.start_async(perm_tasks[0]=new StartMainTask(e,ev,*env,sh,sh_2d,sh_arr,sh2,sh3,screen_x,screen_y,filenames[0],argc,argv));

  std::vector<void*> args;
  args.push_back(perm_args[0]=env);
  args.push_back(env_tab);
  std::vector<BuilderIter*> nodes;
  nodes.push_back(perm_nodes[0]=new MainIter); 
  nodes.push_back(g_start2 = new IterTab(active_tab,&nodes,&args,&perm_tasks, &perm_nodes, &perm_args, &iter_dt));
  
  while(1) {
    IterAlgo(e,nodes,args,&ev);
  }
  return 0;
#ifdef LINUX
  } catch(...) { std::cout << "EXCEPTION" << std::endl; }
#endif
  }


