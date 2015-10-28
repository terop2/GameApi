#include "GameApi.hh"
#include <sstream>
#include <math.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cassert>
#include <ctime>
using namespace GameApi;



struct Envi {
  EveryApi *ev;
  GuiApi *gui;
  std::string target_string;
  float x,y,z;
  std::vector<GuiApi::EditTypes> edit_data;
  W editor;
  bool editor_visible;
  W display;
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
  std::vector<W> connect_clicks;
  std::vector<W> connect_targets;
  std::vector<W> connect_links;
  std::string connect_start_uid;

  std::vector<W> display_clicks;
  std::vector<W> edit_clicks;
  int dialog_i1;
  std::string dialog_uid;
  std::vector<GuiApi::EditTypes*> vec4;
  Ft font;
  Ft font3;
  FtA atlas;
  FtA atlas3;
  BM atlas_bm;
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
  std::vector<W> menus;
  int opened_menu_num;
  bool state;
  bool ctrl;
  int unique_id_counter;
  SH sh2;
  SH sh;
  SH sh3;
  int screen_size_x, screen_size_y;
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
void connect_target(int x, int y, Envi *envi)
{
  std::cout << "Connect target!" << std::endl;

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
	  std::cout << "UID: " << uidstring << std::endl;

	  std::stringstream ss(uidstring);
	  std::string uid;
	  ss >> uid;
	  int num;
	  ss >> num;
	  
	  std::string funcname = envi->ev->mod_api.get_funcname(envi->mod, 0, uid);
	  std::vector<int> vec = envi->ev->mod_api.indexes_from_funcname(funcname);
	  int real_index = vec[num];
	  std::cout << "Real index: " << real_index << std::endl;

	  bool b = envi->ev->mod_api.typecheck(envi->mod, 0, envi->connect_start_uid, uid, real_index);
	  if (b) 
	    {
	      envi->ev->mod_api.change_param_value(envi->mod, 0, uid, real_index, envi->connect_start_uid);

	      
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
		      break;
		    }
		}

	      
	      W start_link = envi->gui->find_canvas_item(envi->canvas, envi->connect_start_uid);
	      
	      W link = envi->gui->line( start_link, envi->gui->size_x(start_link),45,
					wid, 0,10, envi->sh2, envi->sh);
	      std::stringstream ss2;
	      ss2 << envi->connect_start_uid << " " << uid << " " << real_index; 
	      envi->gui->set_id(link, ss2.str());
	      envi->connect_links.push_back(link);
	      add_to_canvas(*envi->gui, envi->canvas, link);

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
  envi->ev->mod_api.insert_inserted_to_canvas(*envi->gui, envi->canvas, envi->chosen_item, uid, display_cb, edit_cb);
  envi->display_clicks.push_back(display_cb);
  envi->edit_clicks.push_back(edit_cb);


  std::vector<std::pair<std::string,std::string> > vec = envi->ev->mod_api.defaults_from_function(envi->insert_mod_name);
  envi->ev->mod_api.insert_to_mod(envi->mod, 0, envi->insert_mod_name, uid, envi->gui->pos_x(envi->chosen_item), envi->gui->pos_y(envi->chosen_item), vec);
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

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear();
    //env->ev->mainloop_api.clear_3d();
    
    env->gui->render(env->txt2);

    env->gui->render(env->txt);
    env->gui->render(env->scroll_area);
    //env->gui->render(env->wave);
    //env->gui->render(env->gameapi);
    //env->gui->render(env->test1);
    env->gui->render(env->canvas_area);
    env->gui->render(env->scrollbar_x);
    env->gui->render(env->scrollbar_y);
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
    if (env->opened_menu_num != -1)
      {
	//std::cout << env->opened_menu_num << std::endl;
	env->gui->render(env->menus[env->opened_menu_num]);
      }
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

    // handle esc event
    MainLoopApi::Event e;
    e.last = true;
    while((e=env->ev->mainloop_api.get_event()).last)
      {
	//std::cout << e.type << " " << e.ch << " " << e.button << std::endl;
	//if (e.type==1024 && e.button==-1) continue;
	if (e.type==0x300)
	  std::cout << e.type << " " << e.ch << std::endl;

	if (e.type==1024 && e.button==-1)
	  {
	    env->key_state = true;
	  }
	if (e.type==1024 && e.button==0 && env->key_state==true)
	  {
	    // FIX EMSCRIPTEN EVENTS NOT GIVING KEYDOWN EVENTS.
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
	if (env->display_visible)
	  {
	    int chosen = env->gui->chosen_item(env->display_close);
	    if (chosen==0)
	      {
		env->display_visible = false;
	      }
	    
	  }
	if (!env->display_visible)
	  {
	    int s = env->display_clicks.size();
	    for(int i=0;i<s;i++)
	      {
		W w = env->display_clicks[i];
		int chosen = env->gui->chosen_item(w);
		if (!env->display_visible && chosen==0)
		  {
		    std::string uid = env->gui->get_id(w);
		    
		    std::cout << "Execute for uid: " << uid << std::endl;
		    
		    // Execute
		    int id = env->ev->mod_api.execute(*env->ev, env->mod, 0, uid);

		    // display dialog
		    std::string type = env->ev->mod_api.return_type(env->mod, 0, uid);
		    
		    if (type=="BM")
		      {
			BM bm;
			bm.id = id;
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3);
		      } 
		    else if (type=="BB")
		      {
			BB bb;
			bb.id = id;
			BM bm = env->ev->bool_bitmap_api.to_bitmap(bb, 255,255,255,255, 0,0,0,0);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3);
			
		      }
		    else if (type=="FB")
		      {
			FB fb;
			fb.id = id;
			BM bm = env->ev->float_bitmap_api.to_grayscale_color(fb, 255,255,255,255, 0,0,0,0);
			env->display = env->gui->bitmap_dialog(bm, env->display_close, env->atlas3, env->atlas_bm3);
			
		      }
		    else if (type=="P")
		      {
			P p;
			p.id = id;
			std::cout << "ID: " << p.id << std::endl;
			env->display = env->gui->polygon_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3);
		      }
		    else if (type=="LI")
		      {
			LI p;
			p.id = id;
			env->display = env->gui->lines_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3);
			
		      }
		    else if (type=="PTS")
		      {
			PTS p;
			p.id = id;
			env->display = env->gui->pts_dialog(p, env->sh3, env->screen_size_x, env->screen_size_y, env->display_close, env->atlas3, env->atlas_bm3);

		      }
		    else if (type=="SFO")
		      {
			SFO p;
			p.id = id;
			env->display = env->gui->shader_dialog(p, env->display_close, env->atlas3, env->atlas_bm3);
			
		      }
		    else 
		      {
			std::cout << "Type not found" << type << std::endl;
		      }
		    env->gui->set_pos(env->display, 200.0, 50.0);
		    env->display_visible = true;
		  }
	      }
	  }
	
	if (!env->editor_visible)
	  {
	    int s = env->edit_clicks.size(); //env->gui->num_childs(env->canvas);
	    for(int i=0;i<s;i++)
	      {
		//std::cout << "Child " << i << std::endl;
		W w = //env->gui->get_child(env->canvas, i);
		  env->edit_clicks[i];
		int chosen = env->gui->chosen_item(w);
		if (!env->editor_visible && chosen == 0)
		  {
		    env->dialog_i1 = i;
		    std::string uid = env->gui->get_id(w);
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

		    env->editor = env->gui->edit_dialog(labels,env->vec4,env->atlas3, env->atlas_bm3, types, env->dialog_cancel, env->dialog_ok);
		    env->gui->set_pos(env->editor, 200,50);
		    
		    env->editor_visible = true;
		    
		  }
	      }
	  }
	
	env->gui->update(env->txt, e.cursor_pos, e.button, e.ch, e.type);
	if (env->display_visible)
	  {
	    env->gui->update(env->display, e.cursor_pos, e.button, e.ch, e.type);
	  }
	if (env->editor_visible)
	  env->gui->update(env->editor, e.cursor_pos, e.button, e.ch, e.type);
	env->gui->update(env->txt2, e.cursor_pos, e.button, e.ch, e.type);
	env->gui->update(env->scroll_area, e.cursor_pos, e.button,e.ch, e.type);
	//env->gui->update(env->wave, e.cursor_pos, e.button);
	//env->gui->update(env->gameapi, e.cursor_pos, e.button);
	//env->gui->update(env->test1, e.cursor_pos, e.button);
	env->gui->update(env->canvas_area, e.cursor_pos, e.button,e.ch, e.type);
	env->gui->update(env->scrollbar_x, e.cursor_pos, e.button,e.ch, e.type);
	env->gui->update(env->scrollbar_y, e.cursor_pos, e.button,e.ch, e.type);
	
	//int s4 = env->connect_links.size();
	//for(int i4 = 0;i4<s4;i4++)
	//  {
	//    W wid = env->connect_links[i4];
	//    env->gui->update(wid, e.cursor_pos, e.button, e.ch, e.type);
	//  }
	
	
	if (env->insert_ongoing)
	  {
	    env->gui->update(env->insert_widget, e.cursor_pos, e.button,e.ch, e.type);
	  }
	if (env->connect_ongoing)
	  {
	    env->gui->update(env->connect_widget, e.cursor_pos, e.button, e.ch, e.type);
	    env->gui->update(env->connect_line, e.cursor_pos, e.button, e.ch, e.type);
	  }
	
	int cs = env->connect_clicks.size();
	for(int ci = 0;ci<cs;ci++)
	  {
	    W wid = env->connect_clicks[ci];
	    int connected = env->gui->chosen_item(wid);
	    if (connected==0)
	      {
		std::string uid = env->gui->get_id(wid);
		W canvas_item = env->gui->find_canvas_item(env->canvas, uid);
		
		BM bm = env->ev->bitmap_api.newbitmap(2,2);
		W ico_1 = env->gui->icon(bm);
		env->connect_widget = env->gui->insert_widget(ico_1, std::bind(&connect_target, _1, _2, env));
		
		env->connect_line = env->gui->line(canvas_item, env->gui->size_x(canvas_item),50,
						   env->connect_widget, 0, 0, env->sh2, env->sh);
		
		env->connect_start_uid = uid;
		env->connect_ongoing = true;
		break;
	      }
	    
	  }


	float param_x = env->gui->dynamic_param(env->txt2, 0);
	env->gui->set_dynamic_param(env->scroll_area, 1, param_x);
	float param_x1 = env->gui->dynamic_param(env->scrollbar_x, 0);
	env->gui->set_dynamic_param(env->canvas_area, 0, param_x1);
	float param_y1 = env->gui->dynamic_param(env->scrollbar_y, 0);
	env->gui->set_dynamic_param(env->canvas_area, 1, param_y1);
	env->ev->mod_api.update_lines_from_canvas(env->canvas, env->mod, 0);
	
	int area_y = env->gui->size_y(env->array);
	//std::cout << area_y << std::endl;
	env->gui->set_dynamic_param(env->txt2, 0, area_y);
	
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
		    env->gui->generic_to_string(*env->vec4[i], types[i], *ref);
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
		    name = env->gui->polygonapi_functions_item_label(sel2-1);
		    break;
		  case 2: 
		    name = env->gui->boolbitmapapi_functions_item_label(sel2-1);
		    break;
		  case 3: 
		    name = env->gui->floatbitmapapi_functions_item_label(sel2-1);
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
		    name = env->gui->pointapi_functions_item_label(sel2-1);
		    break;
		  case 8:
		    name = env->gui->vectorapi_functions_item_label(sel2-1);
		    break;

		  case 9:
		    name = env->gui->volumeapi_functions_item_label(sel2-1);
		    break;
		  case 10:
		    name = env->gui->floatvolumeapi_functions_item_label(sel2-1);
		    break;
		  case 11:
		    name = env->gui->colorvolumeapi_functions_item_label(sel2-1);
		    break;
		  case 12:
		    name = env->gui->fontapi_functions_item_label(sel2-1);
		    break;


		  };
		//std::cout << "Chosen label: " << name << std::endl;
		env->insert_mod_name = name;
		W ww = { 0 };
		env->connect_clicks.push_back(ww);
		int uid_num = env->unique_id_counter;
		std::stringstream ss;
		ss << "uid" << uid_num;
		std::string uid = ss.str();
		
		env->chosen_item = env->ev->mod_api.inserted_widget(*env->gui, env->mod, 0, env->atlas, env->atlas_bm, name, env->connect_clicks[env->connect_clicks.size()-1], uid, env->connect_targets);
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
	

	int selected_item = env->gui->chosen_item(env->txt);
	int selected_item2 = -1;
	if (selected_item != -1)
	  {
	    env->opened_menu_num = selected_item;
	  }
	if (env->opened_menu_num != -1)
	  {
	    env->gui->update(env->menus[env->opened_menu_num], e.cursor_pos, e.button, e.ch, e.type);
	    if (e.button == -1) { env->state=1; }
	    if (e.button==0 && e.type==1025 && env->state==1)
	      {
		selected_item2 = env->gui->chosen_item(env->menus[env->opened_menu_num]);
		env->opened_menu_num = -1;
		env->state = 0;
		//std::cout << selected_item2 << std::endl;
	      }
	  }
      }
}
float f(float w)
{
  return cos(w);
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  Env *e2 = new Env;
  Env &e = *e2;
  EveryApi ev(*e2);
  

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
  SH sh2 = ev.shader_api.colour_shader();
  SH sh3 = ev.shader_api.colour_shader();
  Ft font = ev.font_api.newfont("..\\nevis.ttf", 13,15); // 13,15
  Ft font2 = ev.font_api.newfont("..\\nevis.ttf", 10,13); // 10,13
  Ft font3 = ev.font_api.newfont("..\\nevis.ttf", 30,30); // 30,30


  if (argc==2)
    {
      if (std::string(argv[1])=="--generate-font-atlas")
	{
	  std::cout << "Generating font atlas. " << std::endl;
	  std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-();:_";
	  FtA atlas = ev.font_api.font_atlas_info(ev, font, chars, 13,15, 25);
	  FtA atlas2 = ev.font_api.font_atlas_info(ev, font2, chars, 10,13, 25);
	  FtA atlas3 = ev.font_api.font_atlas_info(ev, font3, chars, 30,30, 65);
	  BM atlas_bm = ev.font_api.font_atlas(ev, font, atlas, 13,15);
	  BM atlas_bm2 = ev.font_api.font_atlas(ev,font2, atlas2, 10,13);
	  BM atlas_bm3 = ev.font_api.font_atlas(ev,font3, atlas3, 30,30);
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
	  exit(0);
	}
    }


  FtA atlas = ev.font_api.load_atlas("atlas0.txt");
  FtA atlas2 = ev.font_api.load_atlas("atlas1.txt");
  FtA atlas3 = ev.font_api.load_atlas("atlas2.txt");
  BM atlas_bm = ev.bitmap_api.loadbitmap("atlas_bm0.ppm");
  BM atlas_bm2 = ev.bitmap_api.loadbitmap("atlas_bm1.ppm");
  BM atlas_bm3 = ev.bitmap_api.loadbitmap("atlas_bm2.ppm");

#if 0
  FB atlas_bm_b = ev.float_bitmap_api.from_red(atlas_bm_a);
  FB atlas_bm2_b = ev.float_bitmap_api.from_red(atlas_bm2_a);
  FB atlas_bm3_b = ev.float_bitmap_api.from_red(atlas_bm3_a);

  BM atlas_bm = ev.float_bitmap_api.to_grayscale_color(atlas_bm_b, 255,255,255,255, 0,0,0,0);
  BM atlas_bm2 = ev.float_bitmap_api.to_grayscale_color(atlas_bm2_b,255,255,255,255, 0,0,0,0);
  BM atlas_bm3 = ev.float_bitmap_api.to_grayscale_color(atlas_bm3_b, 255,255,255,255, 0,0,0,0);
#endif
  // rest of the initializations
  ev.mainloop_api.init_3d(sh3, screen_x, screen_y);
  ev.mainloop_api.init(sh, screen_x,screen_y);
  ev.mainloop_api.init(sh2, screen_x,screen_y);

  ev.mainloop_api.switch_to_3d(false, sh3);
  ev.shader_api.use(sh);
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
			 

  //std::vector<std::string> vec3;
  //vec3.push_back("newbitmap");
  //vec3.push_back("newbitmap");
  //W test1 = gui.list_item_opened(100,"Bi tmapApi", font, vec3, font);
  //gui.set_pos(test1, 400,400);

  std::vector<W> items;
  for(int i=0;i<1;i++)
    {
      items.push_back(gui.bitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.polygonapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.boolbitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.floatbitmapapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.shadermoduleapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.linesapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.pointsapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.pointapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.vectorapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));

      items.push_back(gui.volumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.floatvolumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.colorvolumeapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));
      items.push_back(gui.fontapi_functions_list_item(atlas, atlas_bm, atlas2, atlas_bm2));

    }
  W array = gui.array_y(&items[0], items.size(), 5);
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
  ev.mod_api.insert_to_canvas(gui, canvas, mod, 0, atlas, atlas_bm, env.connect_clicks, env.connect_targets, env.display_clicks, env.edit_clicks);

  ev.mod_api.insert_links(ev, gui, mod, 0, env.connect_links, canvas, env.connect_targets, sh2, sh);
  add_to_canvas(gui, canvas, env.connect_links);
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
  env.atlas = atlas;
  env.atlas3 = atlas3;
  env.atlas_bm = atlas_bm;
  env.atlas_bm3 = atlas_bm3;
  env.unique_id_counter = ev.mainloop_api.random();
  //env.editor = editor;
  env.editor_visible = false;
  env.display_visible = false;
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
  env.connect_ongoing = false;
  env.connect_ongoing2 = false;
  env.sh2 = sh2;
  env.sh = sh;
  env.sh3 = sh3;
  env.screen_size_x = screen_x;
  env.screen_size_y = screen_y;

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 30,1);
#endif



}
