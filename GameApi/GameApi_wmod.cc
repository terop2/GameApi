 
#include "GameApi_h.hh"
#include "GameApi_gui.hh"


std::string hexify(std::string s)
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
std::string unhexify(std::string s)
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


std::vector<GameApiItem*> all_functions();


GameApiModule load_gameapi(std::string filename)
{
  GameApiModule mod;
  std::ifstream ss(filename.c_str());
  std::string func_name;
  while(ss >> func_name)
    {
      GameApiFunction f;
      f.function_name = func_name;
      std::string line;
      std::getline(ss,line); // eat the previous line
      std::getline(ss,line);
      //std::cout << "LINE1: " << line << std::endl;
      std::stringstream ss2(line);
      std::string param_name;
      while(ss2>>param_name) {
	f.param_names.push_back(param_name);
      }
      std::getline(ss,line);
      //std::cout << "LINE2: " << line << std::endl;
      std::stringstream ss3(line);
      std::string param_type;
      while(ss3>>param_type) {
	f.param_types.push_back(param_type);
      }
      while(ss.peek()=='(')
	{
	  GameApiLine g_line;
	  std::getline(ss,line);
	  //std::cout << "LINE3: " << line << std::endl;
	  std::stringstream ss4(line);
	  char c;
	  int x, y;
	  ss4 >> c; // '('
	  ss4 >> x;
	  ss4 >> c; // ','
	  ss4 >> y;
	  ss4 >> c; // ')'
	  std::string mod_name;
	  ss4 >> mod_name;
	  std::string uid;
	  ss4 >> uid;
	  bool is_array;
	  ss4 >> is_array;
	  while(ss4.peek()==' ') ss4.ignore(1,' ');
	  int val = ss4.peek();
	  int j = 0;
	  int sz = 1;
	  //std::cout << "VALUE: " << val << " " << (char)val << std::endl;
	  if ((val>='0'&&val<='9')||val=='-') {
	  ss4 >> j;
	  ss4 >> sz;
	   }
	  //std::cout << "VALUE: " << j << " " << sz << std::endl;
	  g_line.x = x;
	  g_line.y = y;
	  g_line.uid = uid;
	  //g_line.j = j;
	  g_line.sz = sz;
	  g_line.module_name = mod_name;
	  g_line.array_return = is_array;
	  std::string name_value;
	  while(ss4>>name_value)
	    {
	      GameApiParam p;
	      //int ii = 0;
	      //std::cout << name_value << std::endl;
	      int s = name_value.size();
	      int i = 0;
	      int i1=0,i2=0,i3=0,i4=0;
	      for(;i<s;i++) { if (name_value[i]==':') break; }
	      i1 = i;
	      i++;
	      for(;i<s;i++) { if (name_value[i]==':') break; }
	      i2 = i;
	      i++;
	      for(;i<s;i++) { if (name_value[i]==':') break; }
	      i3 = i;
	      i++;
	      for(;i<s;i++) { if (name_value[i]==':') break; }
	      i4= i;
	      i++;
	      p.param_name = name_value.substr(0, std::max(i1,0));
	      p.value = unhexify(name_value.substr(i1+1, i2-i1-1));
	      std::string is_array_str = name_value.substr(i2+1, i3-i2-1);
	      if (is_array_str=="true") { p.is_array = true; }
	      std::string array_target_str = name_value.substr(i3+1, i4-i3-1);
	      //std::cout << "S:" << s << " I4:" << i4 << std::endl;
	      std::string jj = i4!=s?name_value.substr(i4+1, s-i4-1):"0";
	      int array_index = -1;
	      std::stringstream ss(array_target_str);
	      ss >> array_index;
	      GameApiLine *array_line = p.is_array && array_index != -1 ? &f.lines[array_index] : NULL;
	      p.array_return_target = array_line;

	      std::stringstream ss2(jj);
	      int j=0;
	      ss2 >> j;
	      p.j = j;
	      //std::cout << "Name: " << p.param_name << " Value: " << p.value << std::endl;
	      //std::cout << "PARAMS: " << j << std::endl;
	      g_line.params.push_back(p);
	    }
	  f.lines.push_back(g_line);
	} 
      mod.funcs.push_back(f);
    }
  return mod;
}

void save_gameapi(const GameApiModule &mod, std::string filename)
{
  std::ofstream ss(filename.c_str());


  int s = mod.funcs.size();
  for(int i=0;i<s;i++)
    {
      const GameApiFunction &func = mod.funcs[i];
      ss << func.function_name << std::endl;
      int s2 = func.param_names.size();
      for(int j=0;j<s2;j++)
	{
	  std::string s = func.param_names[j];
	  ss << s;
	  if (j!=s2-1) ss << " ";
	}
      ss << std::endl;
      int s3 = func.param_types.size();
      for(int jj=0;jj<s3;jj++)
	{
	  std::string s = func.param_types[jj];
	  ss << s;
	  if (jj!=s3-1) ss << " ";
	}
      ss << std::endl;
      int s4 = func.lines.size();
      for(int k=0;k<s4;k++)
	{
	  const GameApiLine &line = func.lines[k];
	  ss << '(' << line.x << "," << line.y << ')' << line.module_name << " " << line.uid << " " << line.array_return << " " << line.j << " " << line.sz;
	  int s = line.params.size();
	  if (s!=0) ss << " ";
	  for(int i=0;i<s;i++)
	    {
	      const GameApiParam &p = line.params[i];
	      std::string name = p.param_name;
	      std::string value = hexify(p.value);
	      std::string is_array = p.is_array ? "true" : "false";
	      int j = p.j;
	      int array_line = p.array_return_target ? (p.array_return_target - &func.lines[0])/sizeof(GameApiLine) : -1;
	      std::stringstream ss2;
	      ss2 << array_line;
	      std::string array_line_str = ss2.str();
	      std::stringstream ss3;
	      ss3 << j;
	      std::string jj2 = ss3.str();
	      ss << name << ":" << value << ":" << is_array << ":" << array_line_str << ":" << jj2;
	      if (i!=s-1) ss << " ";
	    }
	  ss << std::endl;
	}
    }
}
EXPORT void GameApi::WModApi::save(GameApi::WM wm, std::string filename)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[wm.id];
  save_gameapi(*mod, filename);
}

EXPORT GameApi::WM GameApi::WModApi::load(std::string filename)
{
  GameApiModule mod = load_gameapi(filename);
  GameApiModule *mod2 = new GameApiModule(mod);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  env->gameapi_modules.push_back(mod2);
  GameApi::WM wm;
  wm.id = env->gameapi_modules.size()-1;
  return wm;
}
std::vector<int> reduce_list_to_indexes_only(std::vector<std::string> type_names, std::vector<int> indexes)
{
  std::vector<int> ret;
  int s = type_names.size();
  for(int i=0;i<s;i++)
    {
      std::string name = type_names[i];
      int index = indexes[i];
      if (name[0]>='A' && name[0]<'Z' && name.size()<=4)
	{
	  ret.push_back(index);
	}
      if (name[0]=='[' && name[name.size()-1]==']')
	{ // array
	  if (name[1]>='A' && name[1]<='Z' && name.size()<=6)
	    {
	      ret.push_back(index);
	    }
	}

    }
  return ret;

}

std::vector<std::string> reduce_list_to_string_only(std::vector<std::string> type_names, std::vector<std::string> indexes)
{
  std::vector<std::string> ret;
  int s = type_names.size();
  for(int i=0;i<s;i++)
    {
      std::string name = type_names[i];
      std::string index = indexes[i];
      if (name[0]>='A' && name[0]<='Z' && name.size()<=4)
	{
	  ret.push_back(index);
	}
      if (name[0]=='[' && name[name.size()-1]==']')
	{ // array
	  if (name[1]>='A' && name[1]<='Z' && name.size()<=6)
	    {
	      ret.push_back(index);
	    }
	}
    }
  return ret;

}

std::vector<std::string> reduce_list_to_types_only(std::vector<std::string> type_names)
{
  std::vector<std::string> ret;
  int s = type_names.size();
  for(int i=0;i<s;i++)
    {
      std::string name = type_names[i];
      if (name[0]>='A' && name[0]<='Z' && name.size()<=4)
	{
	  ret.push_back(name);
	}
      if (name[0]=='[' && name[name.size()-1]==']')
	{ // array
	  if (name[1]>='A' && name[1]<='Z' && name.size()<=6)
	    {
	      ret.push_back(name);
	    }
	}

    }
  return ret;
}
EXPORT void GameApi::WModApi::update_lines_from_canvas(W canvas, WM mod2, int id)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  GuiWidget *wid = find_widget(e, canvas);
  if (!wid) return;
  Point2d canvas_pos = wid->get_pos();
#ifdef EMSCRIPTEN
  CanvasWidget *can = static_cast<CanvasWidget*>(wid);
#else
  CanvasWidget *can = dynamic_cast<CanvasWidget*>(wid);
#endif
  if (!can) return;
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i]; 
      GuiWidget *w = can->find_widget(line->uid);
      if (!w) continue;
      Point2d pos = w->get_pos();
      line->x = int(pos.x-canvas_pos.x);
      line->y = int(pos.y-canvas_pos.y);
    }
}

EXPORT std::vector<int> GameApi::WModApi::indexes_from_funcname(std::string func_name)
{
  static std::vector<GameApiItem*> functions = all_functions();
  static std::map<std::string,GameApiItem*> funcmap;
  if (funcmap.size()==0) {
    int s = functions.size();
    for(int i=0;i<s;i++) {
      GameApiItem *item = functions[i];
      std::string name = item->Name(0);
      funcmap[name]=item;
    }
  }

  /*
  int s = functions.size();
  int i = 0;
  for(;i<s;i++)
    {
      GameApiItem *item = functions[i];
      std::string name = item->Name(0);
      if (func_name == name) break;
    }
  */
  GameApiItem *item = funcmap[func_name]; //functions[i];
  std::vector<std::string> param_types;
  std::vector<int> param_indexes;
  int sss = item->ParamCount(0);
  for(int k=0;k<sss;k++)
    {
      param_types.push_back(item->ParamType(0,k));
      param_indexes.push_back(k);
    }
  std::vector<int> param_indexes2 = reduce_list_to_indexes_only(param_types, param_indexes);
  return param_indexes2;
}

EXPORT GameApi::W GameApi::WModApi::inserted_widget(GuiApi &gui, WM mod2, int id, FtA atlas, BM atlas_bm, std::string func_name, std::vector<W *> connect_click, std::string uid, std::vector<W> &params)
{
  static std::vector<GameApiItem*> functions = all_functions();

  int s = functions.size();
  int i = 0;
  for(;i<s;i++)
    {
      GameApiItem *item = functions[i];
      std::string name = item->Name(0);
      if (func_name == name) break;
    }
  GameApiItem *item = functions[i];
  std::string return_type = item->ReturnType(0);
  std::string symbols = item->Symbols();
  std::string comment = item->Comment();
  std::vector<std::string> param_types;
  std::vector<std::string> param_tooltip;
  int sss = item->ParamCount(0);
  for(int k=0;k<sss;k++)
    {
      param_types.push_back(item->ParamType(0,k));
      param_tooltip.push_back(item->ParamName(0,k));
    }
  std::vector<std::string> param_types2 = reduce_list_to_types_only(param_types);
  std::vector<std::string> param_tooltip2 = reduce_list_to_string_only(param_types, param_tooltip);
  W node = gui.canvas_item_gameapi_node(100,100, func_name, param_types2, param_tooltip2, return_type, atlas, atlas_bm, connect_click, uid, params, symbols, comment);
  return node;
}

std::vector<std::string> remove_unnecessary_labels(std::vector<std::string> types, std::vector<std::string> labels)
{
  std::vector<std::string> res;
  int s = types.size();
  for(int i=0;i<s;i++)
    {
      std::string type = types[i];
      std::string label = labels[i];
      if (type[0]>='A' && type[0]<='Z' && type.size()<=4)
	{
	}
      else
      if (type[0]=='[' && type[type.size()-1]==']' && type[1]>='A' && type[1]<='Z' && type.size()<=6)
	{
	}
      else
	{
	  res.push_back(label);
	}
    }
  return res;

}
std::vector<std::string> filter_unnecessary_types(std::vector<std::string> vec)
{
  std::vector<std::string> res;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      std::string type = vec[i];
      if (type[0]>='A' && type[0]<='Z' && type.size()<=4)
	{
	}
      else
      if (type[0]=='[' && type[type.size()-1]==']' && type[1]>='A' && type[1]<='Z' && type.size()<=6)
	{
	}
      else
	{
	  res.push_back(type);
	}
    }
  return res;
}
std::vector<std::string*> remove_unnecessary_refs(std::vector<std::string*> refs, std::vector<std::string> param_types)
{
  std::vector<std::string*> res;
  int s = param_types.size();
  for(int i=0;i<s;i++)
    {
      std::string type = param_types[i];
      if (type[0]>='A' && type[0]<='Z' && type.size()<=4)
	{
	}
      else
      if (type[0]=='[' && type[type.size()-1]==']' && type[1]>='A' && type[1]<='Z' && type.size()<=6)
	{
	}
      else
	{
	  res.push_back(refs[i]);
	}
    }
  return res;  
}
EXPORT std::vector<std::string*> GameApi::WModApi::refs_from_function(WM mod2, int id, std::string funcname)
{
  //std::cout << "refs_from_function: " << funcname << std::endl;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  std::vector<std::string> param_types;
  std::vector<std::string*> refs;
  
  std::string module_name = "";
  int sk = func->lines.size();
  for(int k=0;k<sk;k++)
    {
      GameApiLine *line = &func->lines[k];
      if (funcname == line->uid)
	{
	  module_name = line->module_name;


	  static std::vector<GameApiItem*> functions = all_functions();

	  //std::vector<GameApiItem*> functions = bitmapapi_functions();
	  std::vector<std::string> types;
	  int s = functions.size();
	  int i = 0;
	  for(;i<s;i++)
	    {
	      GameApiItem* item = functions[i];
	      std::string name = item->Name(0);
	      if (name==module_name)
		{
		  break;
		}
	    }
	  if (i==s) { std::cout << "ERROR! item not found!" << std::endl; }
	  GameApiItem *item = functions[i];
	  int s2 = item->ParamCount(0);
	  int s3 = line->params.size();
	  assert(s2==s3);
	  //std::cout << "Refs count" << s3 << std::endl;
	  //std::cout << "Chosen line:" << line->module_name << std::endl;
	  for(int i=0;i<s3;i++)
	    {
	      GameApiParam *param = &line->params[i];
	      std::string *value = &param->value;
	      std::string paramtype = item->ParamType(0, i);
	      //std::string value = item->ParamDefault(0, i);
	      param_types.push_back(paramtype);
	      refs.push_back(value);
	    }
	  std::vector<std::string*> refs2 = remove_unnecessary_refs(refs, param_types);
	  return refs2;

	}
    }
  return std::vector<std::string*>();

}
EXPORT std::vector<std::string> GameApi::WModApi::types_from_function(WM mod2, int id, std::string funcname)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  std::string module_name = "";
  int sk = func->lines.size();
  for(int k=0;k<sk;k++)
    {
      GameApiLine *line = &func->lines[k];
      if (funcname == line->uid)
	{
	  module_name = line->module_name;
	}
    }
  if (module_name=="")
    {
      return std::vector<std::string>();
    }


  static std::vector<GameApiItem*> functions = all_functions();

  //std::vector<GameApiItem*> functions = bitmapapi_functions();
  std::vector<std::string> types;
  int s = functions.size();
  for(int i=0;i<s;i++)
    {
      GameApiItem* item = functions[i];
      std::string name = item->Name(0);
      if (name==module_name)
	{
	  int ss = item->ParamCount(0);
	  for(int j=0;j<ss;j++)
	    {
	      std::string type = item->ParamType(0, j);
	      types.push_back(type);
	    }
	  break;

	}
    }
  std::vector<std::string> types2 = filter_unnecessary_types(types);
  return types2;
}

void InstallProgress(int num, std::string label, int max);
void ProgressBar(int num, int val, int max, std::string label);

EXPORT void GameApi::WModApi::insert_links(EveryApi &ev, GuiApi &gui, WM mod2, int id, std::vector<W> &links, W canvas, const std::vector<W> &connect_targets, SH sh2, SH sh)
{
  InstallProgress(868,"insert_links", 15);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      if (s/15>0 && i % (s/15) == 0)
	ProgressBar(868, i*15/s, 15, "insert_links");
      GameApiLine &line = func->lines[i];
      int ss = line.params.size();
      for(int ii=0;ii<ss;ii++)
	{
	  GameApiParam &param = line.params[ii];
	  std::string value = param.value;
	  int j = param.j;
	  
	  if (value.size()>1 && value[0]=='[' && value[value.size()-1]==']')
	    {
	      std::vector<std::string> v = parse_param_array(value);
	      int s= v.size();
	      for(int i=0;i<s;i++)
		{
		  std::string value = v[i];
		  bool success;
		  std::pair<std::string,int> pp = parse_multiple_return_uid(value, success);
	  int sss = func->lines.size();
	  for(int iii = 0; iii<sss; iii++)
	    {
	      GameApiLine &line2 = func->lines[iii];
	      if (line2.uid == pp.first /*value*/)
		{
		  W w1 = gui.find_canvas_item(canvas, pp.first /*value*/);
		  if (w1.id==-1) continue;
		  int ssss = connect_targets.size();
		  for(int iiii=0;iiii<ssss; iiii++)
		    {
		      W wid = connect_targets[iiii];
		      std::string u = gui.get_id(wid);
		      int j = pp.second; //gui.get_index(wid);
		      int sz = line2.sz; //gui.get_size2(w1);
		      if (sz==0) sz=1;
		      std::stringstream ss(u);
		      std::string target_uid;
		      int num;
		      ss >> target_uid >> num;
		      
		      std::string funcname = ev.mod_api.get_funcname(mod2, 0, target_uid);
		      std::vector<int> vec = ev.mod_api.indexes_from_funcname(funcname);
		      int real_num = vec[num];

		      bool success;
		      std::pair<std::string,int> pp2 = parse_multiple_return_uid(target_uid, success);
		      
		      if (/*target_uid*/ pp2.first == line.uid && real_num == ii)
			{
			  W w2 = wid;
			  if (sz<0||sz>25) sz=1;
			  if (j<0||j>25) j=0;
			  //std::cout << "NUMS:" << j << " " << sz << std::endl;
			  int yy = 16+16+5+4+(gui.size_y(w1)-16-16)*j/sz;
			  if (sz<=1) yy=16+5+4+(gui.size_y(w1)-16-16)/2;
			  W line = gui.line( w1, gui.size_x(w1), /*(gui.size_y(w1)-16)/2+16+5*/ yy /*16+16+5+4+(gui.size_y(w1)-16-16)*j/sz*/,
					     w2, 0, 10, sh2, sh);
			  std::stringstream ss2;
			  ss2 << pp.first << " " << target_uid << " " << real_num;
			  gui.set_id(line, ss2.str());
			  gui.set_index(line, j);
			  gui.set_size2(line, sz);
			  //std::cout << "LINK2: " << ss2.str() << std::endl;
			  links.push_back(line);
			}
		      
		    }
		}

	    }
		}
	    }
	

	  int sss = func->lines.size();
	  for(int iii = 0; iii<sss; iii++)
	    {
	      GameApiLine &line2 = func->lines[iii];
	      if (line2.uid == value)
		{
		  W w1 = gui.find_canvas_item(canvas, value);
		  if (w1.id==-1) continue;

		  int ssss = connect_targets.size();
		  for(int iiii=0;iiii<ssss; iiii++)
		    {
		      W wid = connect_targets[iiii];
		      std::string u = gui.get_id(wid);
		      //int j = gui.get_index(wid);
		      int sz = line2.sz; //gui.get_size2(w1);
		      if (sz==0) sz=1;
		      if (sz<0||sz>25) sz=1;
		      if (j<0||j>25) j=0;

		      std::stringstream ss(u);
		      std::string target_uid;
		      int num;
		      ss >> target_uid >> num;
		      
		      std::string funcname = ev.mod_api.get_funcname(mod2, 0, target_uid);
		      std::vector<int> vec = ev.mod_api.indexes_from_funcname(funcname);
		      int real_num = vec[num];
		      bool success;
		      std::pair<std::string,int> pp = parse_multiple_return_uid(target_uid, success);

		      if (pp.first /* target_uid */ == line.uid && real_num == ii)
			{
			  W w2 = wid;
			  //std::cout << "NUMS:" << j << " " << sz << std::endl;
		  int yy = 16+16+5+4+(gui.size_y(w1)-16-16)*j/sz;
		  if (sz<=1) yy=16+5+4+(gui.size_y(w1)-16-16)/2;
		  W line = gui.line( w1, gui.size_x(w1), yy /*16+16+5+4+(gui.size_y(w1)-16-16)*j/sz*/ /*(gui.size_y(w1)-16)/2+16+5*/,
					     w2, 0, 10, sh2, sh);
			  std::stringstream ss2;
			  ss2 << value << " " << target_uid << " " << real_num;
			  gui.set_id(line, ss2.str());
			  gui.set_index(line, j);
			  gui.set_size2(line, sz);
			  //std::cout << "LINK: " << ss2.str() << std::endl;
			  links.push_back(line);
			}
		      
		    }

		}
	    }

	}
    }
}
EXPORT std::string GameApi::WModApi::return_type(WM mod2, int id, std::string line_uid)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      if (line->uid == line_uid)
	{
	  std::string module_name = line->module_name;
	  //std::cout << "return type module name: " << module_name << std::endl;
	  static std::vector<GameApiItem*> vec = all_functions();
	  int sd = vec.size();
	  for(int k=0;k<sd;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      if (name == module_name)
		{
		  return item->ReturnType(0);
		}
	    }

	}
    }
  std::cout << "Return type failed!" << std::endl;
  return "";
}

EXPORT void GameApi::WModApi::delete_by_uid(WM mod2, int id, std::string line_uid)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      if (line->uid == line_uid)
	{
	  func->lines.erase(func->lines.begin()+i);
	  break;
	}
    }


  int s2 = func->lines.size();
  for(int i2=0;i2<s2;i2++)
    {
      GameApiLine *line = &func->lines[i2];
      int s3 = line->params.size();
      for(int i3=0;i3<s3;i3++)
	{
	  GameApiParam &param = line->params[i3];
	  std::string val = param.value;
	  if (val==line_uid) { param.value=""; }
	  if (val.size()>1 && val[0]=='[' && val[val.size()-1]==']')
	    {
	      std::vector<std::string> v = parse_param_array(val);
	      int s = v.size();
	      for(int i=0;i<s;i++)
		{
		  if (v[i]==line_uid) {
		    v.erase(v.begin()+i);
		    --i;
		    --s;
		  }
		}
	      std::string val2 = generate_param_array(v);
	      param.value = val2;
	    }
	}
    }


  

}


int env_counter(bool reset)
{
  static int i =0;
  if (reset) i=0;
  return i++;
}
EXPORT void GameApi::WModApi::codegen_reset_counter()
{
  env_counter(true);
}
int ret_type_count(std::string return_type);


struct codegen_item
{
  int id;
  std::string line_uid;
  std::string ret_id;
};
std::vector<codegen_item> codegen_cache;

void clear_codegen()
{
  codegen_cache = std::vector<codegen_item>();
}

void push_codegen(int id, std::string line_uid, std::string ret_id)
{
  codegen_item i;
  i.id = id;
  i.line_uid = line_uid;
  i.ret_id = ret_id;
  codegen_cache.push_back(i);
}

std::string find_codegen(int id, std::string line_uid, bool &success)
{
  int s = codegen_cache.size();
  for(int i=0;i<s;i++)
    {
      codegen_item &ii = codegen_cache[i];
      if (ii.id==id && ii.line_uid==line_uid) { success=true; return ii.ret_id; }
    }
  success=false;
  return "";
}
    

EXPORT std::pair<std::string,std::string> GameApi::WModApi::codegen(EveryApi &ev, WM mod2, int id, std::string line_uid, int level, int j)
{
  static std::vector<GameApiItem*> vec = all_functions();

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      if (line->uid == line_uid)
	{

	  // COLLECT PARAMS & RECURSE
	  int ss = line->params.size();
	  std::vector<std::string> params;
	  std::vector<std::string> param_names;
	  level-=ss;
	  for(int ii=0;ii<ss;ii++)
	    {
	      //level--;
	      GameApiParam *param = &line->params[ii];
	      std::string p = "";
	      std::string pn = param->value;
	      std::string rt = "";
	      int jj = param->j;
	      int sz = line->sz;
	      if (level<=0)
		{ // Stopping recursion
		  int sd = vec.size();
		  for(int k=0;k<sd;k++)
		    {
		      GameApiItem *item = vec[k];
		      std::string name = item->Name(0);
		      if (name == line->module_name)
			{
			  if (pn.size()>0 && pn[0]=='[')
			    pn="[]";
			  else
			    pn = "@";
			  break;
			}
		    }
		}

	      if (pn.size()==0)
		{
		  int val = env_counter(false);
		  std::stringstream ss;
		  ss << val;
		  pn = std::string("E") + ss.str();
		  //std::cout << "CODEGEN FAILED at param!" << std::endl;
		  //return std::make_pair("","");
		}
	      if (pn.size()>3 && pn[0]=='u' && pn[1] == 'i' && pn[2] =='d')
		{
		  bool success = false;
		  std::string id2 = find_codegen(id,pn,success);
		  if (success) {
		    rt = return_type(mod2,id,pn);
		    p = "";
		    pn = id2;
		  } else {
		  std::pair<std::string,std::string> val = codegen(ev, mod2, id, pn, level-1,jj);
		  //std::cout << "CODEGEN:" << line_uid << " " << pn << std::endl;
		  rt = return_type(mod2, id, pn);
		  p = val.second;
		  pn = val.first;
		  }
		}
	      int ret_count = ret_type_count(rt);
	      if (ret_count>1) {
		// Multiple return types
		// Use jj here..
		std::stringstream ss;
		ss << jj;
		pn += "[" + ss.str() + "]";
	      }
	      if (pn.size()>1 && pn[0]=='[' && pn[pn.size()-1]==']')
		{
		  std::string param_type = "";
		  int sd = vec.size();
		  for(int k=0;k<sd;k++)
		    {
		      GameApiItem *item = vec[k];
		      std::string name = item->Name(0);
		      if (name == line->module_name)
			{
			  std::string paramtype = item->ParamType(0,ii);
			  param_type=paramtype;
			}
		    }
		  param_type = param_type.substr(1,param_type.size()-2);

		  // ARRAYTODO, HOW TO HANDLE ARRAY RETURN VALUES
		  
		  int prev = 1;
		  std::string ss = "std::vector<" + param_type + ">{";
		  int sz = pn.size();
		  for(int i=1;i<sz;i++)
		    {
		      if (pn[i]==',' || pn[i]==']')
			{
			  std::string substr = pn.substr(prev, i-prev);
			  //std::cout << "substr: " << substr << std::endl;
			  if (substr.size()>3 && substr[0]=='u' && substr[1] == 'i' && substr[2] =='d')
			    {
		  bool success = false;
		  std::pair<std::string,int> pp = parse_multiple_return_uid(substr, success);

		  std::string id2 = find_codegen(id,pp.first,success);
		  if (success) {
		    rt = return_type(mod2,id,pp.first);
		    p += "";
		    ss+= id2;
		  } else {

			      
			      bool success;
			      std::pair<std::string,std::string> val = codegen(ev, mod2, id, pp.first /*substr*/, level-1,0);
			      // TODO guess no multiple return values here
			      rt = return_type(mod2, id, pp.first /*substr*/);

			      p += val.second;
			      ss += val.first;
		  }

			      int ret_count = ret_type_count(rt);
			      if (ret_count>1) {
			      	std::stringstream ss2;
			      	ss2 << pp.second; 
				ss+= "[" + ss2.str() + "]";
			      }
			    }
			  else
			    {
			      ss+=substr;
			    }
			  prev = i+1;
			  //ss+=substr;
			  if (i!=sz-1) { ss+=","; }
			}

		    }
		  ss+="}";
		  //std::cout << "Param: " << ss << std::endl;
		  param_names.push_back(ss);
		  params.push_back(p);
		}
	      else
		{
		  params.push_back(p);
		  param_names.push_back(pn);
		}
	    }
	  // EXECUTE
	  int sd = vec.size();
	  for(int k=0;k<sd;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      if (name == line->module_name)
		{
		  std::pair<std::string,std::string> val = item->CodeGen(ev, params, param_names,j);
		  push_codegen(id,line_uid, val.first);
		  return val;
		}
	    }
	}
    }
  std::cout << "CODEGEN FAILED! " << std::endl;
  return std::make_pair("","");

}
EXPORT int GameApi::WModApi::execute(EveryApi &ev, WM mod2, int id, std::string line_uid, ExecuteEnv &exeenv, int level, int j)
{
  static std::vector<GameApiItem*> vec = all_functions();

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      if (line->uid == line_uid)
	{
	  // CREATE ENVIRONMENT
	  int sd1 = vec.size();
	  for(int k=0;k<sd1;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      if (name == line->module_name)
		{
		  item->BeginEnv(exeenv, line->params);
		  break;
		}
	    }



	  // COLLECT PARAMS & RECURSE
	  int ss = line->params.size();
	  level-=ss;
	  std::vector<std::string> params;
	  for(int ii=0;ii<ss;ii++)
	    {
	      GameApiParam *param = &line->params[ii];
	      std::string p = param->value;
	      int jj = param->j;
	      if (level<=0)
		{ // Stopping recursion
		  int sd = vec.size();
		  for(int k=0;k<sd;k++)
		    {
		      GameApiItem *item = vec[k];
		      std::string name = item->Name(0);
		      if (name == line->module_name)
			{
			  if (p.size()>0 && p[0]=='[')
			    p="[]";
			  else
			    p = "@";
			  break;
			}
		    }
		}
	      if (p.size()==0)
		{
		  p = "@";
		  //std::cout << "EXECUTE FAILED at param!" << std::endl;
		  //return -1;
		}
	      if (p.size()>3 && p[0]=='u' && p[1] == 'i' && p[2] =='d')
		{
		  int val = execute(ev, mod2, id, p, exeenv, level-1,jj);
		  if (val==-1) return -1;
		  std::stringstream sw;
		  sw << val;
		  p = sw.str();
		  //std::cout << "Num From UID: " << p << std::endl;
		}
	      // ARRAYTODO: HOW TO HANDLE ARRAY RETURN VALUES
	      if (p.size()>1 && p[0]=='[' && p[p.size()-1]==']')
		{
		  int prev = 1;
		  std::string ss = "[";
		  int sz = p.size();
		  for(int i=1;i<sz;i++)
		    {
		      if (p[i]==',' || p[i]==']')
			{
			  std::string substr = p.substr(prev, i-prev);
			  //std::cout << "substr: " << substr << std::endl;
			  if (substr.size()>3 && substr[0]=='u' && substr[1] == 'i' && substr[2] =='d')
			    {
			      bool success;
			      std::pair<std::string,int> pp = parse_multiple_return_uid(substr,success);
			      int val = execute(ev, mod2, id, pp.first /*substr*/, exeenv, level-1,pp.second);
			      if (val==-1) return -1;
			      std::stringstream sw;
			      sw << val;
			      substr = sw.str();
			    }
			  prev = i+1;
			  ss+=substr;
			  if (i!=sz-1) { ss+=","; }
			}

		    }
		  ss+="]";
		  //std::cout << "Param: " << ss << std::endl;
		  params.push_back(ss);
		}
	      else
		{
		  //std::cout << "Param: " << p << std::endl;

		  params.push_back(p);
		}
	    }
	  // EXECUTE
	  int sd = vec.size();
	  for(int k=0;k<sd;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      if (name == line->module_name)
		{
		  //std::cout << "Execute: " << name << std::endl;
		  std::stringstream ss;
		  int val = item->Execute(ss,e, ev, params, exeenv,j);
		  item->EndEnv(exeenv);
		  //std::cout << "Execute " << name << " returns " << val << std::endl;
		  return val;
		}
	    }
  std::cout << "EXECUTE FAILED! "<< line->module_name << std::endl;

	}
    }
  return -1;
}

std::vector<std::string> combine_vec(std::vector<std::string> v1, std::vector<std::string> v2)
{
  std::vector<std::string> vec(v1.begin(),v1.end());
  int s = v2.size();
  for(int i=0;i<s;i++) vec.push_back(v2[i]);
  return vec;
}

EXPORT std::pair<int,std::vector<std::string> > GameApi::WModApi::collect_urls(EveryApi &ev, WM mod2, int id, std::string line_uid, ExecuteEnv &exeenv, int level, ASyncData *arr, int arr_size, int j)
{
  static std::vector<GameApiItem*> vec = all_functions();

  std::vector<std::string> res;
  
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      if (line->uid == line_uid)
	{
	  // CREATE ENVIRONMENT
	  int sd1 = vec.size();
	  for(int k=0;k<sd1;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      if (name == line->module_name)
		{
		  //item->BeginEnv(exeenv, line->params);
		  break;
		}
	    }



	  // COLLECT PARAMS & RECURSE
	  int ss = line->params.size();
	  level-=ss;
	  std::vector<std::string> params;
	  for(int ii=0;ii<ss;ii++)
	    {
	      GameApiParam *param = &line->params[ii];
	      std::string p = param->value;
	      int jj = param->j;
	      if (level<=0)
		{ // Stopping recursion
		  int sd = vec.size();
		  for(int k=0;k<sd;k++)
		    {
		      GameApiItem *item = vec[k];
		      std::string name = item->Name(0);
		      if (name == line->module_name)
			{
			  if (p.size()>0 && p[0]=='[')
			    p="[]";
			  else
			    p = "@";
			  break;
			}
		    }
		}
	      if (p.size()==0)
		{
		  p = "@";
		  //std::cout << "EXECUTE FAILED at param!" << std::endl;
		  //return std::make_pair(-1,res);
		}
	      if (p.size()>3 && p[0]=='u' && p[1] == 'i' && p[2] =='d')
		{
		  std::pair<int,std::vector<std::string> > vals = collect_urls(ev, mod2, id, p, exeenv, level-1,arr,arr_size,jj);
		  int val = vals.first;
		  std::vector<std::string> urls = vals.second;
		  res = combine_vec(res, urls);
		  //if (val==-1) return std::make_pair(-1,res);
		  std::stringstream sw;
		  sw << val;
		  p = sw.str();
		  //std::cout << "Num From UID: " << p << std::endl;
		}
	      // ARRAYTODO: HOW TO HANDLE ARRAY RETURN VALUES
	      if (p.size()>1 && p[0]=='[' && p[p.size()-1]==']')
		{
		  int prev = 1;
		  std::string ss = "[";
		  int sz = p.size();
		  for(int i=1;i<sz;i++)
		    {
		      if (p[i]==',' || p[i]==']')
			{
			  std::string substr = p.substr(prev, i-prev);
			  //std::cout << "substr: " << substr << std::endl;
			  if (substr.size()>3 && substr[0]=='u' && substr[1] == 'i' && substr[2] =='d')
			    {
			      bool success;
			      std::pair<std::string,int> pp = parse_multiple_return_uid(substr, success);
			      std::pair<int,std::vector<std::string> > vals = collect_urls(ev, mod2, id, pp.first /*substr*/, exeenv, level-1,arr,arr_size,pp.second);
			      int val = vals.first;
			      res = combine_vec(res,vals.second);
			      //if (val==-1) return std::make_pair(-1,res);
			      std::stringstream sw;
			      sw << val;
			      substr = sw.str();
			    }
			  prev = i+1;
			  ss+=substr;
			  if (i!=sz-1) { ss+=","; }
			}

		    }
		  ss+="]";
		  //std::cout << "Param: " << ss << std::endl;
		  params.push_back(ss);
		}
	      else
		{
		  //std::cout << "Param: " << p << std::endl;

		  params.push_back(p);
		}
	    }
	  // EXECUTE
	  int sd = vec.size();
	  for(int k=0;k<sd;k++)
	    {
	      GameApiItem *item = vec[k];
	      std::string name = item->Name(0);
	      std::string fname = item->FuncName(0);
	      if (name == line->module_name)
		{
		  int s = arr_size;
		  for(int i=0;i<s;i++)
		    {
		      ASyncData *ptr = &arr[i];
		      //std::cout << "ASYNCDATA: " << name << " " << ptr->func_name << std::endl;
		      if (fname==ptr->func_name) {
			res.push_back(params[ptr->param_num]);
		      }
		    }
		  
		  //std::cout << "Execute: " << name << std::endl;
		  //int val = item->Execute(e, ev, params, exeenv);
		  //item->EndEnv(exeenv);
		  //std::cout << "Execute " << name << " returns " << val << std::endl;
		  return std::make_pair(-1,res);
		}
	    }
	}
    }
  std::cout << "COLLECTURLS FAILED! " << std::endl;
  return std::make_pair(-1,res);
}

int ret_type_count(std::string);
std::string ret_type_index(std::string return_type, int index);


EXPORT bool GameApi::WModApi::typecheck(WM mod2, int id, std::string uid1, std::string uid2, int param_index, int ret_index, bool &is_array, bool &is_array_return)
{
  is_array=false;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  std::string module1;
  std::string module2;
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid1)
	{
	  module1 = line.module_name;
	}
      if (line.uid == uid2)
	{
	  module2 = line.module_name;
	}
    }
  
  std::string type1="";
  std::string type2="";
  static std::vector<GameApiItem*> functions = all_functions();
  int s2 = functions.size();
  for(int ii=0;ii<s2;ii++)
    {
      GameApiItem *item = functions[ii];
      if (item->Name(0) == module1)
	{
	  type1 = item->ReturnType(0);
	  if (type1.size()>2) {
	    if (ret_type_count(type1)>1)
	      {
		type1 = ret_type_index(type1, ret_index);
	      }	    
	    if (type1[0]=='[' && type1[type1.size()-1]==']')
	      {
		is_array_return=true;
		type1 = type1.substr(1,type1.size()-2);
	      }

	  }

	}
      if (item->Name(0) == module2)
	{
	  type2 = item->ParamType(0, param_index);
	  if (type2.size()>2) {
	    if (type2[0]=='[' && type2[type2.size()-1]==']')
	      {
		is_array=true;
		type2 = type2.substr(1,type2.size()-2);
	      }
	  }

	}
    }
  //std::cout << "TypeCheck: " << type1 << " " << type2 << std::endl;
  return type1==type2;

}
EXPORT int GameApi::WModApi::find_line_index(WM mod2, int id, std::string uid)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid)
	{
	  return i;
	}
    }
  return -1;
}
EXPORT void GameApi::WModApi::change_param_multiple_return_values(WM mod2, int id, std::string uid, int j, int sz)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid)
	{
	  line.j = j;
	  line.sz = sz;
	}
    }
}
EXPORT void GameApi::WModApi::change_param_is_array(WM mod2, int id, std::string uid, int param_index, bool is_array, int ref_line_index)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid)
	{
	  GameApiParam &param = line.params[param_index];
	  param.is_array = is_array;
	  if (ref_line_index != -1)
	    param.array_return_target = &func->lines[ref_line_index];
	  else
	    param.array_return_target = 0;
	  //std::cout << "Param: " << param.param_name << " changed to " << newvalue << std::endl;
	}
    }

}
EXPORT void GameApi::WModApi::change_param_value(WM mod2, int id, std::string uid, int param_index, std::string newvalue, int j)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid)
	{
	  GameApiParam &param = line.params[param_index];
	  param.value = newvalue;
	  param.j = j;
	  //std::cout << "Param: " << param.param_name << " changed to " << newvalue << std::endl;
	}
    }
}

EXPORT std::vector<std::string> GameApi::WModApi::parse_param_array(std::string s)
{
  if (s.size()<2) { 
    return std::vector<std::string>();
  }
  if (s[0]!='[') { std::cout << "param_parse_array parse error" << std::endl; 
  }
  if (s[s.size()-1]!=']') { std::cout << "param_parse_array parse error2" << std::endl; }
  int ss = s.size();
  int prev = 1;
  std::vector<std::string> vec;
  for(int i=1;i<ss;i++)
    {
      if (s[i]==',' || s[i]==']')
	{
	  std::string sub = s.substr(prev, i-prev);
	  vec.push_back(sub);
	  prev = i+1;
	}
    }
  return vec;
}

EXPORT std::string GameApi::WModApi::generate_param_array(std::vector<std::string> v)
{
  if (v.size()==0) return "";
  std::stringstream ss;
  ss << "[";
  int s = v.size();
  for(int i=0;i<s;i++)
    {
      ss << v[i];
      if (i!=s-1) { ss << ","; }
    }
  ss << "]";
  return ss.str();
}

EXPORT std::pair<std::string, int> GameApi::WModApi::parse_multiple_return_uid(std::string s, bool &success)
{
  //std::cout << "PARSE: " << s << std::endl;
  if (s.size()<2) { success=false; return std::pair<std::string,int>(s,0); }
  int ss = s.size();
  int pos = 0;
  std::string label = s;
  std::string index = "0";
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='@') {
	label = s.substr(0,i);
	index = s.substr(i+1);
	//std::cout << "LABEL:" << label << std::endl;
	//std::cout << "INDEX: " << index << std::endl;
      }
    }
  std::stringstream ss2(index);
  int index2;
  ss2 >> index2;
  success = true;
  return std::make_pair(label, index2);
}

EXPORT std::string GameApi::WModApi::param_value(WM mod2, int id, std::string uid, int param_index)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      if (line.uid == uid)
	{
	  GameApiParam &param = line.params[param_index];
	  return param.value;
	  //param.value = newvalue;
	  //std::cout << "Param: " << param.param_name << " changed to " << newvalue << std::endl;
	}
    }
  return "";
}

EXPORT std::string GameApi::WModApi::get_funcname(WM mod2, int id, std::string uid)
{
  static std::map<std::string,std::string> cache;

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  if (cache[uid]!="") return cache[uid];

  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine line = func->lines[i];
      if (line.uid == uid)
	{
	  cache[uid]=line.module_name;
	  return line.module_name;
	}
    }
  return "";
}
EXPORT void GameApi::WModApi::insert_to_mod(WM mod2, int id, std::string modname, std::string uid, bool array_return, int x, int y, std::vector<InsertParam> params)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  GameApiLine new_line;
  new_line.x = x;
  new_line.y = y;
  new_line.module_name = modname;
  new_line.uid = uid;
  new_line.array_return = array_return;
  int s = params.size();
  for(int i=0;i<s;i++)
    {
      InsertParam p = params[i];
      if (p.line_index_in_gameapi_function_lines_array != -1) {
	GameApiParam pp;
	pp.param_name = p.first;
	pp.value = p.second;
	pp.is_array = p.is_array;
	pp.array_return_target = &func->lines[p.line_index_in_gameapi_function_lines_array];
	new_line.params.push_back(pp);
      } else {
	GameApiParam pp;
	pp.param_name = p.first;
	pp.value = p.second;
	pp.is_array = false;
	pp.array_return_target = 0;
	new_line.params.push_back(pp);
      }
    }
  func->lines.push_back(new_line);

}
EXPORT std::vector<std::pair<std::string,std::string> > GameApi::WModApi::defaults_from_function(std::string module_name)
{
  std::vector<std::pair<std::string,std::string> > res;

  static std::vector<GameApiItem*> functions = all_functions();


  //std::vector<GameApiItem*> functions = bitmapapi_functions();
  int s = functions.size();
  for(int i=0;i<s;i++)
    {
      GameApiItem* item = functions[i];
      std::string name = item->Name(0);
      if (name==module_name)
	{
	  int ss = item->ParamCount(0);
	  for(int j=0;j<ss;j++)
	    {
	      std::string p_name = item->ParamName(0, j);
	      std::string p_value = item->ParamDefault(0, j);
	      std::pair<std::string,std::string> p = std::make_pair(p_name, p_value);
	      res.push_back(p);
	    }
	  return res;
	}
    }
  return std::vector<std::pair<std::string,std::string> >();
}

EXPORT std::vector<std::string> GameApi::WModApi::labels_from_function(WM mod2, int id, std::string funcname)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];


  std::string module_name = "";
  int sk = func->lines.size();
  for(int k=0;k<sk;k++)
    {
      GameApiLine *line = &func->lines[k];
      if (funcname == line->uid)
	{
	  module_name = line->module_name;
	}
    }
  if (module_name=="")
    {
      return std::vector<std::string>();
    }

  static std::vector<GameApiItem*> functions = all_functions();

  //std::vector<GameApiItem*> functions = bitmapapi_functions();
  int s = functions.size();
  std::vector<std::string> types;
  std::vector<std::string> labels;
  for(int i=0;i<s;i++)
    {
      GameApiItem* item = functions[i];
      std::string name = item->Name(0);
      if (name==module_name)
	{
	  int ss = item->ParamCount(0);
	  for(int j=0;j<ss;j++)
	    {
	      std::string type = item->ParamType(0, j);
	      types.push_back(type);

	      std::string label = item->ParamName(0,j);
	      labels.push_back(label + ": ");
	    }
	  break;
	}
    }
  std::vector<std::string> labels2 = remove_unnecessary_labels(types, labels);
  return labels2;
}
EXPORT void GameApi::WModApi::insert_inserted_to_canvas(GuiApi &gui, W canvas, W item, std::string uid, W &display_clicks, W &edit_clicks, W &delete_key, W &codegen_button, W &popup_open)
{
  int pos_x = gui.pos_x(item);
  int pos_y = gui.pos_y(item);

  pos_x -= gui.pos_x(canvas);
  pos_y -= gui.pos_y(canvas);


  //W codegen_0 = gui.highlight(20,20);
  //    W codegen_1 = gui.button(20,20, 0xff88ff88, 0xff44ff44);
  //    W codegen_2 = gui.layer(codegen_1, codegen_0);
  W codegen_2 = gui.canvas(20,20);
      W codegen_3 = gui.click_area(codegen_2, 0,0, gui.size_x(codegen_2), gui.size_y(codegen_2),0);
      gui.set_id(codegen_3, uid);
      codegen_button = codegen_3;
      W codegen_4 = gui.center_align(codegen_3, gui.size_x(item));
      W codegen_5 = gui.center_y(codegen_4, gui.size_y(item)-40);
      W codegen_6 = gui.margin(codegen_5, 0, 40, 0, 0);


      // W display_2 = gui.highlight(20, 20);
      //   W display2_0 = gui.button(20,20, 0xffff8888, 0xffff4444);
      // W display2_1 = gui.layer(display2_0, display_2);
  W display2_1 = gui.canvas(20,20);

      W display_22 = gui.click_area(display2_1, 0,0, gui.size_x(display2_1), gui.size_y(display2_1),0);
  gui.set_id(display_22, uid);
  display_clicks = display_22;
  W display_3a = gui.center_align(display_22, gui.size_x(item));
  W display_3b = gui.center_y(display_3a, gui.size_y(item)-80);
  W display_3 = gui.margin(display_3b, 0, 80, 0, 0);



  //  W node22 = gui.highlight(20, 20);
  //    W node22_0 = gui.button(20,20, 0xff888888, 0xff444444);
  //      W node22_1 = gui.layer(node22_0, node22);
  W node22 = gui.canvas(20,20);
  W node22_1 = gui.canvas(20,20);


  W node22a = gui.center_align(node22_1, gui.size_x(item));
  W node22b = gui.center_y(node22a, gui.size_y(item));

  W node2 = gui.click_area(item, (gui.size_x(item)-20)/2, (gui.size_y(item)-20)/2, gui.size_x(node22), gui.size_y(node22),0);

  gui.set_id(node2, uid);
  edit_clicks = node2;

  W node221 = gui.margin(node22b, 0,0, 0,0);
  W node222 = gui.layer(node2, node221);
  W node2222 = gui.layer(node222, display_3);
  W node22222 = gui.layer(node2222, codegen_6);
#ifdef EMSCRIPTEN
  W node4 = gui.key_area(node22222, 0, 0, gui.size_x(node22222), 20, 76);
#else
  W node4 = gui.key_area(node22222, 0, 0, gui.size_x(node22222), 20, 127);
#endif
  gui.set_id(node4, uid);
  delete_key = node4;
  W node5 = gui.click_area(node4, 0, 0, gui.size_x(node4), gui.size_y(node4), 2);
  gui.set_id(node5, uid);
  popup_open = node5;
  
  W node3 = gui.mouse_move(node5, 0, 0, gui.size_x(node5), 20);
  gui.set_pos(item, 0.0, 0.0);
  gui.set_id(node3, uid);

  gui.canvas_item(canvas, node3, pos_x, pos_y);
  
}

void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max);

std::string json_string(std::string data)
{
  std::stringstream ss;
  ss << " \"";
  int s = data.size();
  for(int i=0;i<s;i++) {
    if (data[i]=='\\') ss << "\\";
    if (data[i]=='\"') ss << "\\";
    ss << data[i];
  }
  ss << "\" ";
  return ss.str();
}
std::string json_array(std::vector<std::string> vec)
{
  std::stringstream ss;
  ss << "[";
  int s = vec.size();
  for(int i=0;i<s;i++) {
    ss << vec[i];
    if (i!=s-1) ss << ",";
  }
  ss << "]";
  return ss.str();
}
std::string json_number(int i)
{
  std::stringstream ss;
  ss << i;
  return ss.str();
}
struct NameValue
{
  std::string name;
  std::string value;
};
std::string json_object(std::vector<NameValue> vec)
{
  std::stringstream ss;
  ss << "{ ";
  int s = vec.size();
  for(int i=0;i<s;i++) {
    ss << vec[i].name << " :" << vec[i].value;
    if (i!=s-1) ss << ",";
  }
  ss << "}";
  return ss.str();
}

EXPORT std::string GameApi::WModApi::dump_functions()
{
  static std::vector<GameApiItem*> functions = all_functions();

  int s = functions.size();
  std::vector<NameValue> vec2;
  for(int i=0;i<s;i++)
    {
      std::string name = functions[i]->Name(0);
      //ss << name << std::endl;
      std::string rettype = functions[i]->ReturnType(0);
      std::string apiname = functions[i]->ApiName(0);
      std::string funcname = functions[i]->FuncName(0);

      std::string json_name = json_string(name);
      std::string json_rettype = json_string(rettype);
      std::string json_apiname = json_string(apiname);
      std::string json_funcname = json_string(funcname);
      
      std::vector<std::string> vec;
      vec.push_back(json_name);
      vec.push_back(json_rettype);
      vec.push_back(json_apiname);
      vec.push_back(json_funcname);
      
      //ss << name << ": " << rettype << " " << apiname << "::" << funcname;
      int pcount = functions[i]->ParamCount(0);
      std::string json_pcount = json_number(pcount);

      vec.push_back(json_pcount);
      
      //ss << "(";
      for(int j=0;j<pcount;j++) {
	//if (j!=0) ss<< ", ";
	std::string paramname = functions[i]->ParamName(0,j);
	std::string paramtype = functions[i]->ParamType(0,j);
	std::string paramdefault = functions[i]->ParamDefault(0,j);

	std::string json_paramname = json_string(paramname);
	std::string json_paramtype = json_string(paramtype);
	std::string json_paramdefault = json_string(paramdefault);

	vec.push_back(json_paramname);
	vec.push_back(json_paramtype);
	vec.push_back(json_paramdefault);
	
	//ss << paramtype << " " << paramname << " = " << paramdefault;
      }

      std::string array = json_array(vec);

      NameValue nv;
      nv.name = json_string(name);
      nv.value = array;
      vec2.push_back(nv);
      
      //ss << ");" << std::endl;
      //ss << "]";
      
    }
  return json_object(vec2);
}

int ret_type_count(std::string);

EXPORT void GameApi::WModApi::insert_to_canvas(GuiApi &gui, W canvas, WM mod2, int id, FtA atlas, BM atlas_bm, std::vector<W> &connect_clicks_p, std::vector<W> &params, std::vector<W> &display_clicks, std::vector<W> &edit_clicks, std::vector<W> &delete_key, std::vector<W> &codegen_button, std::vector<W> &popup_open)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  static std::vector<GameApiItem*> functions = all_functions();
  int s = func->lines.size();
  std::vector<W> connect_clicks;
  std::vector<int> connect_counts;
  for(int j=0;j<s;j++)
    {
      GameApiLine *line = &func->lines[j];
      W w = { 0 };

      int ss = functions.size();
      int p = 0;
      for(;p<ss;p++)
	{
	  GameApiItem* item = functions[p];
	  if (item->Name(0) == line->module_name) break;
	}
      if (p==ss) { connect_counts.push_back(0); continue; }
      GameApiItem *item = functions[p];
      std::string return_type = item->ReturnType(0);
      int count = ret_type_count(return_type);
      connect_counts.push_back(count);
      for(int k=0;k<count;k++)
	connect_clicks.push_back(w);
    }
  InstallProgress(898, "create boxes", 15);
  int ret_count=0;
  for(int i=0;i<s;i++)
    {
      if (s/15>0 && i % (s/15)==0)
	ProgressBar(898, i*15/s, 15, "create boxes");
      GameApiLine *line = &func->lines[i];
      int ss = functions.size();
      int j = 0;
      for(;j<ss;j++)
	{
	  GameApiItem* item = functions[j];
	  if (item->Name(0) == line->module_name) break;
	}
      int cnt = i<connect_counts.size()?connect_counts[i]:1;
      if (j==ss) { ret_count+=cnt; continue; }
      GameApiItem *item = functions[j];
      std::vector<std::string> param_types;
      std::vector<std::string> param_tooltip;
      int sss = item->ParamCount(0);
      for(int k=0;k<sss;k++)
	{
	  param_types.push_back(item->ParamType(0,k));
	  param_tooltip.push_back(item->ParamName(0,k));
	}
      std::vector<std::string> param_types2 = reduce_list_to_types_only(param_types);
      std::vector<std::string> param_tooltip2 = reduce_list_to_string_only(param_types, param_tooltip);
      std::string return_type = item->ReturnType(0);
      std::string symbols = item->Symbols();
      std::string comment = item->Comment();


      std::vector<W*> connect_clicks2;
      for(int u=0;u<cnt;u++) connect_clicks2.push_back(&connect_clicks[ret_count+u]);
      
      
      W node = gui.canvas_item_gameapi_node(100,100, line->module_name, param_types2, param_tooltip2, return_type, atlas, atlas_bm, connect_clicks2, line->uid, params, symbols,comment);
      ret_count+=cnt;

      // W codegen_0 = gui.highlight(20,20);
      // W codegen_1 = gui.button(20,20, 0xff88ff88, 0xff44ff44);
      //W codegen_2 = gui.layer(codegen_1, codegen_0);
      W codegen_2 = gui.canvas(20,20);
      W codegen_3 = gui.click_area(codegen_2, 0,0, gui.size_x(codegen_2), gui.size_y(codegen_2),0);
      gui.set_id(codegen_3, line->uid);
      gui.set_index(codegen_3, line->j);
      gui.set_size2(codegen_3, line->sz);
      codegen_button.push_back(codegen_3);
      W codegen_4 = gui.center_align(codegen_3, gui.size_x(node));
      W codegen_5 = gui.center_y(codegen_4, gui.size_y(node)-40);
      W codegen_6 = gui.margin(codegen_5, 0, 40, 0, 0);


      //W display_2 = gui.highlight(20, 20);
      //W display2_0 = gui.button(20,20, 0xffff8888, 0xffff4444);
      //W display2_1 = gui.layer(display2_0, display_2);
      W display2_1 = gui.canvas(20,20);
      W display_22 = gui.click_area(display2_1, 0,0, gui.size_x(display2_1), gui.size_y(display2_1),0);
      gui.set_id(display_22, line->uid);
      gui.set_index(display_22, line->j);
      gui.set_size2(display_22, line->sz);
      display_clicks.push_back(display_22);
      W display_3a = gui.center_align(display_22, gui.size_x(node));
      W display_3b = gui.center_y(display_3a, gui.size_y(node)-80);
      W display_3 = gui.margin(display_3b, 0, 80, 0, 0);


      //W node22 = gui.highlight(20, 20);
      //W node22_0 = gui.button(20,20, 0xff888888, 0xff444444);
      //W node22_1 = gui.layer(node22_0, node22);
      W node22 = gui.canvas(20,20);
      W node22_1 = gui.canvas(20,20);
      W node22a = gui.center_align(node22_1, gui.size_x(node));
      W node22b = gui.center_y(node22a, gui.size_y(node));

      W node2 = gui.click_area(node, (gui.size_x(node)-20)/2, (gui.size_y(node)-20)/2, gui.size_x(node22), gui.size_y(node22),0);
      gui.set_id(node2, line->uid);
      gui.set_index(node2, line->j);
      gui.set_size2(node2, line->sz);
      edit_clicks.push_back(node2);


      W node221 = gui.margin(node22b, 0,0, 0,0);
      W node222 = gui.layer(node2, node221);
      W node2222 = gui.layer(node222, display_3);
      W node22222 = gui.layer(node2222, codegen_6);
#ifdef EMSCRIPTEN
      W node4 = gui.key_area(node22222, 0, 0, gui.size_x(node22222), 20, 76);
#else
      W node4 = gui.key_area(node22222, 0, 0, gui.size_x(node22222), 20, 127);
#endif
      gui.set_id(node4, line->uid);
      gui.set_index(node4, line->j);
      gui.set_size2(node4, line->sz);

      delete_key.push_back(node4);
      W node5 = gui.click_area(node4, 0, 0, gui.size_x(node4), gui.size_y(node4), 2);
      gui.set_id(node5, line->uid);
      gui.set_index(node5, line->j);
      gui.set_size2(node5, line->sz);
      popup_open.push_back(node5);
      W node3 = gui.mouse_move(node5, 0, 0, gui.size_x(node5), 20);
      gui.set_id(node3, line->uid);
      gui.set_index(node3, line->j);
      gui.set_size2(node3, line->sz);
      gui.canvas_item(canvas, node3, line->x, line->y);
    }
  connect_clicks_p = connect_clicks;
}
