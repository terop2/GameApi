
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
      std::cout << "LINE1: " << line << std::endl;
      std::stringstream ss2(line);
      std::string param_name;
      while(ss2>>param_name) {
	f.param_names.push_back(param_name);
      }
      std::getline(ss,line);
      std::cout << "LINE2: " << line << std::endl;
      std::stringstream ss3(line);
      std::string param_type;
      while(ss3>>param_type) {
	f.param_types.push_back(param_type);
      }
      while(ss.peek()=='(')
	{
	  GameApiLine g_line;
	  std::getline(ss,line);
	  std::cout << "LINE3: " << line << std::endl;
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
	  g_line.x = x;
	  g_line.y = y;
	  g_line.uid = uid;
	  g_line.module_name = mod_name;
	  std::string name_value;
	  while(ss4>>name_value)
	    {
	      GameApiParam p;
	      //int ii = 0;
	      //std::cout << name_value << std::endl;
	      int s = name_value.size();
	      int i = 0;
	      for(;i<s;i++) { if (name_value[i]==':') break; }
	      p.param_name = name_value.substr(0, std::max(i,0));
	      p.value = unhexify(name_value.substr(i+1, s-i-1));
	      //std::cout << "Name: " << p.param_name << " Value: " << p.value << std::endl;
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
	  ss << '(' << line.x << "," << line.y << ')' << line.module_name << " " << line.uid;
	  int s = line.params.size();
	  if (s!=0) ss << " ";
	  for(int i=0;i<s;i++)
	    {
	      const GameApiParam &p = line.params[i];
	      std::string name = p.param_name;
	      std::string value = hexify(p.value);
	      ss << name << ":" << value;
	      if (i!=s-1) ss << " ";
	    }
	  ss << std::endl;
	}
    }
}
void GameApi::WModApi::save(GameApi::WM wm, std::string filename)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[wm.id];
  save_gameapi(*mod, filename);
}

GameApi::WM GameApi::WModApi::load(std::string filename)
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
void GameApi::WModApi::update_lines_from_canvas(W canvas, WM mod2, int id)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  GuiWidget *wid = find_widget(e, canvas);
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
      Point2d pos = w->get_pos();
      line->x = int(pos.x-canvas_pos.x);
      line->y = int(pos.y-canvas_pos.y);
    }
}

std::vector<int> GameApi::WModApi::indexes_from_funcname(std::string func_name)
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

GameApi::W GameApi::WModApi::inserted_widget(GuiApi &gui, WM mod2, int id, FtA atlas, BM atlas_bm, std::string func_name, W &connect_click, std::string uid, std::vector<W> &params)
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
  W node = gui.canvas_item_gameapi_node(100,100, func_name, param_types2, param_tooltip2, return_type, atlas, atlas_bm, connect_click, uid, params);
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
std::vector<std::string*> GameApi::WModApi::refs_from_function(WM mod2, int id, std::string funcname)
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
std::vector<std::string> GameApi::WModApi::types_from_function(WM mod2, int id, std::string funcname)
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
void GameApi::WModApi::insert_links(EveryApi &ev, GuiApi &gui, WM mod2, int id, std::vector<W> &links, W canvas, const std::vector<W> &connect_targets, SH sh2, SH sh)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine &line = func->lines[i];
      int ss = line.params.size();
      for(int ii=0;ii<ss;ii++)
	{
	  GameApiParam &param = line.params[ii];
	  std::string value = param.value;
	  
	  if (value.size()>1 && value[0]=='[' && value[value.size()-1]==']')
	    {
	      std::vector<std::string> v = parse_param_array(value);
	      int s= v.size();
	      for(int i=0;i<s;i++)
		{
		  std::string value = v[i];
	  int sss = func->lines.size();
	  for(int iii = 0; iii<sss; iii++)
	    {
	      GameApiLine &line2 = func->lines[iii];
	      if (line2.uid == value)
		{
		  W w1 = gui.find_canvas_item(canvas, value);
		  
		  int ssss = connect_targets.size();
		  for(int iiii=0;iiii<ssss; iiii++)
		    {
		      W wid = connect_targets[iiii];
		      std::string u = gui.get_id(wid);
		      std::stringstream ss(u);
		      std::string target_uid;
		      int num;
		      ss >> target_uid >> num;
		      
		      std::string funcname = ev.mod_api.get_funcname(mod2, 0, target_uid);
		      std::vector<int> vec = ev.mod_api.indexes_from_funcname(funcname);
		      int real_num = vec[num];

		      if (target_uid == line.uid && real_num == ii)
			{
			  W w2 = wid;
			  W line = gui.line( w1, gui.size_x(w1), (gui.size_y(w1)-16)/2+16+5,
					     w2, 0, 10, sh2, sh);
			  std::stringstream ss2;
			  ss2 << value << " " << target_uid << " " << real_num;
			  gui.set_id(line, ss2.str());
			  std::cout << "LINK2: " << ss2.str() << std::endl;
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
		  
		  int ssss = connect_targets.size();
		  for(int iiii=0;iiii<ssss; iiii++)
		    {
		      W wid = connect_targets[iiii];
		      std::string u = gui.get_id(wid);
		      std::stringstream ss(u);
		      std::string target_uid;
		      int num;
		      ss >> target_uid >> num;
		      
		      std::string funcname = ev.mod_api.get_funcname(mod2, 0, target_uid);
		      std::vector<int> vec = ev.mod_api.indexes_from_funcname(funcname);
		      int real_num = vec[num];

		      if (target_uid == line.uid && real_num == ii)
			{
			  W w2 = wid;
			  W line = gui.line( w1, gui.size_x(w1), (gui.size_y(w1)-16)/2+16+5,
					     w2, 0, 10, sh2, sh);
			  std::stringstream ss2;
			  ss2 << value << " " << target_uid << " " << real_num;
			  gui.set_id(line, ss2.str());
			  std::cout << "LINK: " << ss2.str() << std::endl;
			  links.push_back(line);
			}
		      
		    }

		}
	    }

	}
    }
}
std::string GameApi::WModApi::return_type(WM mod2, int id, std::string line_uid)
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
	  std::cout << "return type module name: " << module_name << std::endl;
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

void GameApi::WModApi::delete_by_uid(WM mod2, int id, std::string line_uid)
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
std::pair<std::string,std::string> GameApi::WModApi::codegen(EveryApi &ev, WM mod2, int id, std::string line_uid)
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
	  for(int ii=0;ii<ss;ii++)
	    {
	      GameApiParam *param = &line->params[ii];
	      std::string p = "";
	      std::string pn = param->value;
	      if (pn.size()==0)
		{
		  std::cout << "CODEGEN FAILED at param!" << std::endl;
		  return std::make_pair("","");
		}
	      if (pn.size()>3 && pn[0]=='u' && pn[1] == 'i' && pn[2] =='d')
		{
		  std::pair<std::string,std::string> val = codegen(ev, mod2, id, pn);
		  p = val.second;
		  pn = val.first;
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
			      std::pair<std::string,std::string> val = codegen(ev, mod2, id, substr);
			      p += val.second;
			      ss += val.first;
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
		  std::pair<std::string,std::string> val = item->CodeGen(ev, params, param_names);
		  return val;
		}
	    }
	}
    }
  std::cout << "CODEGEN FAILED! " << std::endl;
  return std::make_pair("","");

}
int GameApi::WModApi::execute(EveryApi &ev, WM mod2, int id, std::string line_uid, ExecuteEnv &exeenv)
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
	  std::vector<std::string> params;
	  for(int ii=0;ii<ss;ii++)
	    {
	      GameApiParam *param = &line->params[ii];
	      std::string p = param->value;
	      if (p.size()==0)
		{
		  std::cout << "EXECUTE FAILED at param!" << std::endl;
		  return -1;
		}
	      if (p.size()>3 && p[0]=='u' && p[1] == 'i' && p[2] =='d')
		{
		  int val = execute(ev, mod2, id, p, exeenv);
		  if (val==-1) return -1;
		  std::stringstream sw;
		  sw << val;
		  p = sw.str();
		}
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
			  std::cout << "substr: " << substr << std::endl;
			  if (substr.size()>3 && substr[0]=='u' && substr[1] == 'i' && substr[2] =='d')
			    {
			      int val = execute(ev, mod2, id, substr, exeenv);
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
		  std::cout << "Param: " << ss << std::endl;
		  params.push_back(ss);
		}
	      else
		{
		  std::cout << "Param: " << p << std::endl;

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
		  int val = item->Execute(ev, params, exeenv);
		  item->EndEnv(exeenv);
		  return val;
		}
	    }
	}
    }
  std::cout << "EXECUTE FAILED! " << std::endl;
  return -1;
}
bool GameApi::WModApi::typecheck(WM mod2, int id, std::string uid1, std::string uid2, int param_index, bool &is_array)
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
  std::cout << "TypeCheck: " << type1 << " " << type2 << std::endl;
  return type1==type2;

}
void GameApi::WModApi::change_param_value(WM mod2, int id, std::string uid, int param_index, std::string newvalue)
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
	  std::cout << "Param: " << param.param_name << " changed to " << newvalue << std::endl;
	}
    }
}

std::vector<std::string> GameApi::WModApi::parse_param_array(std::string s)
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

std::string GameApi::WModApi::generate_param_array(std::vector<std::string> v)
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

std::string GameApi::WModApi::param_value(WM mod2, int id, std::string uid, int param_index)
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

std::string GameApi::WModApi::get_funcname(WM mod2, int id, std::string uid)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];

  int s = func->lines.size();
  for(int i=0;i<s;i++)
    {
      GameApiLine line = func->lines[i];
      if (line.uid == uid)
	{
	  return line.module_name;
	}
    }
  return "";
}
void GameApi::WModApi::insert_to_mod(WM mod2, int id, std::string modname, std::string uid, int x, int y, std::vector<std::pair<std::string,std::string> > params)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  
  GameApiLine new_line;
  new_line.x = x;
  new_line.y = y;
  new_line.module_name = modname;
  new_line.uid = uid;
  int s = params.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string, std::string> p = params[i];
      GameApiParam pp = { p.first, p.second };
      new_line.params.push_back(pp);
    }
  func->lines.push_back(new_line);

}
std::vector<std::pair<std::string,std::string> > GameApi::WModApi::defaults_from_function(std::string module_name)
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

std::vector<std::string> GameApi::WModApi::labels_from_function(WM mod2, int id, std::string funcname)
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
void GameApi::WModApi::insert_inserted_to_canvas(GuiApi &gui, W canvas, W item, std::string uid, W &display_clicks, W &edit_clicks, W &delete_key, W &codegen_button, W &popup_open)
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

void GameApi::WModApi::insert_to_canvas(GuiApi &gui, W canvas, WM mod2, int id, FtA atlas, BM atlas_bm, std::vector<W> &connect_clicks_p, std::vector<W> &params, std::vector<W> &display_clicks, std::vector<W> &edit_clicks, std::vector<W> &delete_key, std::vector<W> &codegen_button, std::vector<W> &popup_open)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApiModule *mod = env->gameapi_modules[mod2.id];
  GameApiFunction *func = &mod->funcs[id];
  static std::vector<GameApiItem*> functions = all_functions();
  int s = func->lines.size();
  std::vector<W> connect_clicks;
  for(int j=0;j<s;j++)
    {
      W w = { 0 };
      connect_clicks.push_back(w);
    }

  for(int i=0;i<s;i++)
    {
      GameApiLine *line = &func->lines[i];
      int ss = functions.size();
      int j = 0;
      for(;j<ss;j++)
	{
	  GameApiItem* item = functions[j];
	  if (item->Name(0) == line->module_name) break;
	}
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

      W node = gui.canvas_item_gameapi_node(100,100, line->module_name, param_types2, param_tooltip2, return_type, atlas, atlas_bm, connect_clicks[i], line->uid, params);

      // W codegen_0 = gui.highlight(20,20);
      // W codegen_1 = gui.button(20,20, 0xff88ff88, 0xff44ff44);
      //W codegen_2 = gui.layer(codegen_1, codegen_0);
      W codegen_2 = gui.canvas(20,20);
      W codegen_3 = gui.click_area(codegen_2, 0,0, gui.size_x(codegen_2), gui.size_y(codegen_2),0);
      gui.set_id(codegen_3, line->uid);
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
      delete_key.push_back(node4);
      W node5 = gui.click_area(node4, 0, 0, gui.size_x(node4), gui.size_y(node4), 2);
      gui.set_id(node5, line->uid);
      popup_open.push_back(node5);
      W node3 = gui.mouse_move(node5, 0, 0, gui.size_x(node5), 20);
      gui.set_id(node3, line->uid);
      gui.canvas_item(canvas, node3, line->x, line->y);
    }
  connect_clicks_p = connect_clicks;
}
