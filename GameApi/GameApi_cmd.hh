

int ret_type_count(std::string s);
struct DelApiItemF
{
  std::vector<GameApiItem*> items;
  ~DelApiItemF() { int s = items.size(); for(int i=0;i<s;i++) delete items[i]; }
};
int to_int(std::string s);
float to_float(std::string s);

extern DelApiItemF g_api_item_deleter;

template<class T>
void set_empty(GameApi::EveryApi &ev, T &t) { t.id=0; }

#define MACRO2(lab, funccall) \
  void set_empty(GameApi::EveryApi&ev,lab &l);

MACRO2(GameApi::BM,ev.bitmap_api.newbitmap(10,10,0x00000000))
MACRO2(GameApi::FD,ev.dist_api.fd_cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::BO,ev.bool_api.cube_bo(ev,0.0,0.0,0.0,0.0,0.0,0.0,1,1))
MACRO2(GameApi::BB,ev.bool_bitmap_api.bb_empty(10,10))
MACRO2(GameApi::FB,ev.float_bitmap_api.fb_empty(10,10))
MACRO2(GameApi::PT,ev.point_api.origo())
MACRO2(GameApi::V,ev.vector_api.null_vector())
MACRO2(GameApi::FO,ev.float_volume_api.distance())
MACRO2(GameApi::SFO,ev.sh_api.cube())
MACRO2(GameApi::O,ev.volume_api.o_cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::CBM,ev.cont_bitmap_api.cbm_empty(10.0,10.0))
MACRO2(GameApi::P,ev.polygon_api.p_empty())
MACRO2(GameApi::SH,ev.shader_api.colour_shader())
MACRO2(GameApi::CO,ev.color_api.u_color(0xffffffff))
MACRO2(GameApi::LI,ev.lines_api.from_polygon(ev.polygon_api.p_empty()))
MACRO2(GameApi::MN,ev.move_api.mn_empty())
MACRO2(GameApi::MT,ev.materials_api.m_def(ev))
MACRO2(GameApi::C,ev.curve_api.linear(std::vector<GameApi::PT>()))
MACRO2(GameApi::PD,ev.polygon_dist_api.empty(ev))
MACRO2(GameApi::WV,ev.waveform_api.wv_empty(1.0))
MACRO2(GameApi::ML,ev.polygon_api.render_vertex_array_ml2(ev,ev.polygon_api.p_empty()))
MACRO2(GameApi::CFB,ev.float_bitmap_api.X_bitmap(300.0,300.0))
#undef MACRO2

std::string empty_param(std::string s);  
std::string unique_id_apiitem();


template<class T, class RT, class... P>
int funccall(std::stringstream &ss, GameApi::Env &ee, GameApi::EveryApi &ev, T (GameApi::EveryApi::*api),
	     RT (T::*fptr)(P...), std::vector<std::string> s, GameApi::ExecuteEnv &e, const std::vector<std::string> &param_name, const std::string &return_type);

std::pair<std::string,std::string> CodeGen_1(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, std::vector<std::string> param_type, std::string return_type, std::string api_name, std::string func_name, int j);

struct ApiSplit
{
  std::string def;
  std::string author;
  std::string license;
};

inline ApiSplit split_api_default(std::string def)
{
  int s = def.size();
  int pos1=-1;
  int pos2=-1;
  int i=0;
  for(;i<s;i++)
    {
      if (def[i]=='@') { pos1=i; i++; break; }
    }
  for(;i<s;i++)
    {
      if (def[i]=='@') { pos2=i; i++; break; }
    }
  ApiSplit spl;
  spl.def = def.substr(0,pos1);
  spl.author = pos1!=-1?def.substr(pos1+1,pos2-pos1):"";
  spl.license = pos2!=-1?def.substr(pos2+1):"";
  return spl;
}


template<class T, class RT, class... P>
class ApiItem : public GameApiItem
{
public:
  ApiItem(T (GameApi::EveryApi::*api), RT (T::*fptr)(P...),
	  std::string name, 
	  std::vector<std::string> param_name,
	  std::vector<std::string> param_type,
	  std::vector<std::string> param_default,
	  std::string ret_type, std::string api_name, std::string func_name,
	  std::string symbols, std::string comment) 
    : api(api), fptr(fptr), name(name), param_name(param_name),
      param_type(param_type), param_default(param_default), 
      return_type(ret_type), api_name(api_name), func_name(func_name), symbols(symbols), comment(comment) { }
  int Count() const { return 1; }
  std::string Name(int i) const { return name; }
  int ParamCount(int i) const { return param_name.size(); }
  std::string ParamName(int i, int p) const { return param_name[p]; }
  std::string ParamType(int i, int p) const { return param_type[p]; }
  std::string ParamDefault(int i, int p) const { return split_api_default(param_default[p]).def; }
  std::string DefaultAuthor(int i, int p) const { return split_api_default(param_default[p]).author; }
  std::string DefaultLicense(int i, int p) const { return split_api_default(param_default[p]).license; }
  std::string ReturnType(int i) const { return return_type; }
  std::string ApiName(int i) const { return api_name; }
  std::string FuncName(int i) const { return func_name; }
  std::string Symbols() const { return symbols; }
  std::string Comment() const { return comment; }

#if USE_CHAISCRIPT
  static RT chai_cb(GameApiItem *item, GameApi::EveryApi *ev, T (GameApi::EveryApi::*api), P... p) {
    ApiItem<T,RT,P...> *ptr = (ApiItem<T,RT,P...>*)item;
    T *ptr2 = &(ev->*api);
    RT val = (ptr2->*(ptr->fptr))(p...);
    return val;
  }
  void RegisterToChai(GameApi::EveryApi *ev, chaiscript::ChaiScript *chai)
  {
    std::function<RT (P... p)> f = [ev,this](P...p) -> RT { return chai_cb(this, ev, this->api, p...); };
    chai->add(chaiscript::fun(f), func_name.c_str());
  }
#endif
  
  int Execute(std::stringstream &ss, GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e, int j)
  {
    int s = params.size();
    for(int i=0;i<s;i++) if (params[i]=="") params[i]="?";
    
    if (params.size()!=param_name.size()) {
	if (ApiName(0)=="mainloop_api" && FuncName(0)=="array_ml") {
	  params.insert(params.begin(),"ev");
	} else {
	  std::cout << "Error: param vectors different size: " << ApiName(0) << "::" << FuncName(0) << std::endl;
	}
    }
    int ret = funccall(ss,ee, ev, api, fptr, params, e, param_name, return_type);
    int ret_count = ret_type_count(return_type);
    if (ret_count>1 && j!=-1)
      { // multiple return values
	GameApi::ARR arr; 
	arr.id = ret;
	ArrayType *t = find_array(ee, arr);
	ret = t->vec[j];
      }
    return ret;
  }
#if 0
  std::vector<GameApi::EditNode*> CollectNodes(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)
  {
    return collectnodes(name,param_type, param_names, params, param_name);
  }
#endif
  

  std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)
  {
    std::pair<std::string,std::string> p = CodeGen_1(ev,params, param_names, param_type, return_type,api_name,func_name,j);
    return p;
#if 0
    int ss = params.size();
   for(int i=0;i<ss;i++)
      {
	s+= params[i];
      }

    s+= return_type;
    s+= " ";
    std::string id = unique_id_apiitem();
    s+= id;
    s+= "=";
    s+= "ev.";
    s+= api_name;
    s+= ".";
    s+= func_name;
    s+= "(";
    int sk = param_names.size();
    for(int i=0;i<sk;i++)
      {
	if (param_names[i]=="@")
	  {
	    s+=empty_param(param_type[i]);
	  }
	else
	  s+= param_names[i];
	if (i!=int(param_names.size())-1) s+=",";
      }
    s+=");\n";
    return std::make_pair(id, s);
#endif
  }
private:
  T GameApi::EveryApi::*api;
  RT (T::*fptr)(P...);
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> param_type;
  std::vector<std::string> param_default;
  std::string return_type;
  std::string api_name;
  std::string func_name;
  std::string symbols;
  std::string comment;
};


template<class T>
class FromStreamClass
{
public:
  T from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
    //std::cout << "Type using default: " << typeid(T).name() << std::endl;
    bool neg = false;
    std::string s;
    ss >> s;

    if (s.size()>0 && s[0]=='-')
      {
	neg = true;
	s = s.substr(1);
      }
    
  T t;
  std::stringstream is(s);
  is >> t;
  if (neg) t = -t;

  //std::cout << "Default: " << t << std::endl;
  return t;
  }
};

std::string FloatExprEval(std::string s);
int find_float_ch(std::string s, char ch);


float to_float(std::string s);



#define MACRO(lab) \
template<>		   \
class FromStreamClass<lab> \
{ \
public:\
  lab from_stream(std::stringstream &ss, GameApi::EveryApi &ev)\
  {\
  lab bm;\
  std::string s;\
  ss >> s;\
  if (s=="@") {\
        set_empty(ev,bm);\
      return bm;\
  }\
  bm.id = to_int(s);\
  return bm;\
  }\
};
MACRO(GameApi::BM)
MACRO(GameApi::FtA)
MACRO(GameApi::FD)
MACRO(GameApi::BO)
MACRO(GameApi::BB)
MACRO(GameApi::FB)
MACRO(GameApi::PT)
MACRO(GameApi::V)
MACRO(GameApi::FO)
MACRO(GameApi::SFO)
MACRO(GameApi::O)
MACRO(GameApi::COV)
MACRO(GameApi::CBM)
MACRO(GameApi::P)
MACRO(GameApi::SH)
MACRO(GameApi::CO)
MACRO(GameApi::Ft)
MACRO(GameApi::VA)
MACRO(GameApi::VX)
MACRO(GameApi::PL)
MACRO(GameApi::TX)
MACRO(GameApi::TXID)
MACRO(GameApi::TXA)
MACRO(GameApi::LI)
MACRO(GameApi::LLA)
MACRO(GameApi::PTS)
MACRO(GameApi::FBO)
MACRO(GameApi::W)
MACRO(GameApi::ML)
MACRO(GameApi::PTA)
MACRO(GameApi::MN)
MACRO(GameApi::VO)
MACRO(GameApi::IM)
MACRO(GameApi::TL)
MACRO(GameApi::T)
MACRO(GameApi::MT)
MACRO(GameApi::C)
MACRO(GameApi::MS)
MACRO(GameApi::MC)
MACRO(GameApi::SA)
MACRO(GameApi::PD)
MACRO(GameApi::WV)
MACRO(GameApi::CC)
MACRO(GameApi::BLK)
MACRO(GameApi::RUN)
MACRO(GameApi::PTT)
MACRO(GameApi::KF)
MACRO(GameApi::CPP)
MACRO(GameApi::IF)
MACRO(GameApi::SF)
MACRO(GameApi::SD)
MACRO(GameApi::FI)
MACRO(GameApi::GI)
MACRO(GameApi::ARR)
MACRO(GameApi::CMD)
MACRO(GameApi::PA)
MACRO(GameApi::PLF)
MACRO(GameApi::PLL)
MACRO(GameApi::PLP)
MACRO(GameApi::CBB)
MACRO(GameApi::CFB)
MACRO(GameApi::PH)
MACRO(GameApi::DC)
MACRO(GameApi::PN)
MACRO(GameApi::PF)
MACRO(GameApi::FF)
MACRO(GameApi::S)
MACRO(GameApi::SBM)
MACRO(GameApi::IBM)
MACRO(GameApi::INP)
MACRO(GameApi::MA)
MACRO(GameApi::CG)
MACRO(GameApi::FML)
MACRO(GameApi::FBU)
MACRO(GameApi::AV)
MACRO(GameApi::UV)
MACRO(GameApi::FFi)
MACRO(GameApi::VFi)
MACRO(GameApi::FA)
MACRO(GameApi::PKG)
MACRO(GameApi::GS)
MACRO(GameApi::TF)
MACRO(GameApi::HML)
MACRO(GameApi::PBO)
MACRO(GameApi::SHP)
MACRO(GameApi::SHI)
MACRO(GameApi::SHC)
MACRO(GameApi::SMT)
MACRO(GameApi::TT)
#undef MACRO


template<>
class FromStreamClass<float>
{
public:
  float from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
    //std::cout << "Type using default: " << typeid(T).name() << std::endl;

    std::string s;
    ss >> s;
    bool neg = false;
    if (s.size()>0 && s[0]=='-')
      {
	neg = true;
	s = s.substr(1);
      }
    s=FloatExprEval(s);

  float t;
  //std::stringstream is(s);
  t = to_float(s);
  if (neg) t = -t;

  //std::cout << "Default: " << t << std::endl;
  return t;
  }
};


template<>
class FromStreamClass<std::string>
{
public:
  std::string from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
    std::string s;
    ss >> s;
    if (s=="?") s="";
    return s;
  }
};
template<>
class FromStreamClass<GameApi::EveryApi &>
{
public:
  GameApi::EveryApi &from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
    std::string s;
    ss >> s;
    return ev;
  }
};

template<>
class FromStreamClass<bool>
{
public:
  bool from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
    std::string s;
    ss >> s;
  if (s=="false") return false;
  if (s=="true") return true;
  if (s=="1") return true;
  return false;
  }
};



template<>
class FromStreamClass<unsigned int>
{
public:
  unsigned int from_stream(std::stringstream &ss, GameApi::EveryApi &ev)
  {
  unsigned int bm;
  //char c;
  //is >> c;
  //is >> c;
  //std::stringstream is(s);
  ss >> std::hex >> bm >> std::dec;
  return bm;
  }
};

template<>
class FromStreamClass<std::vector<std::string>>
{
public:
  std::vector<std::string> from_stream(std::stringstream &sk, GameApi::EveryApi &ev)
  {
    std::string s;
    sk >> s;
    std::vector<std::string> vec;
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass<std::string> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    std::stringstream sk1(substr);
	    //std::cout << "SUBSTR:" << substr << std::endl;
	    std::string t = cls.from_stream(sk1, ev);
	    //std::cout << "RESULT:" << t << std::endl;
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};

template<class T>
class FromStreamClass<std::vector<T>>
{
public:
  std::vector<T> from_stream(std::stringstream &sk, GameApi::EveryApi &ev)
  {
    std::string s;
    sk >> s;

    if (s=="@") return std::vector<T>();
    //std::cout << "Vector:" << s << std::endl;
    std::vector<T> vec;
    if (s.size()>0 && s[0]!='[')
      { // ARR version
	//std::cout << "Arr version" << s << std::endl;
	GameApi::ARR arr;
	std::stringstream ss(s);
	ss >> arr.id;
	ArrayType *t = find_array(ev.get_env(), arr);
	int s = t->vec.size();
	for(int i=0;i<s;i++)
	  {
	    T tt;
	    tt.id = t->vec[i];
	    vec.push_back(tt);
	  }
	return vec;
      }
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass<T> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    std::stringstream sk1(substr);
	    // std::cout << "SUBSTR:" << substr << ":" << std::endl;
	    T t = cls.from_stream(sk1, ev);
	    //std::cout << "RESULT:" << t.id << std::endl;
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};




void funccall_1(std::vector<std::string> &s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name);

template<typename T> T from_stream2(std::stringstream &is, GameApi::EveryApi &ev)
{
  FromStreamClass<T> cls;
  //std::string s;
  //is >> s;
  //std::cout << "FromStreamClass: " << s << " " << typeid(T).name() << std::endl;
  return cls.from_stream(is,ev);
}


template<class T, class RT, class... P>
int funccall(std::stringstream &ss, GameApi::Env &ee, GameApi::EveryApi &ev, T (GameApi::EveryApi::*api),
	     RT (T::*fptr)(P...), std::vector<std::string> s, GameApi::ExecuteEnv &e, const std::vector<std::string> &param_name, const std::string &return_type)
{
  funccall_1(s,e,param_name);
#if 0
  int s3 = s.size();
  if (s.size()!=param_name.size()) { std::cout << "funccall: param_names and parameter values std::vectors different size" << std::endl; }
  for(int i=0;i<s3;i++)
    {
      std::string pn = param_name[i];
      int s4 = e.names.size();
      for(int j=0;j<s4;j++)
	{
	  std::string n = e.names[j];
	  std::string v = e.values[j];
	  if (pn == n)
	    {
	      s[i] = v;
	    }
	}
      
    }
#endif

#ifdef EMSCRIPTEN
#define ORDER 1
#endif
#ifdef ANDROID
#define ORDER 1
#endif
#ifdef RASPI
#define ORDER 1
#endif
  
  //std::stringstream ss;
  int s2 = s.size();
#ifndef ORDER
  for(int i=s2-1;i>=0;i--)
    {
      ss << s[i] << " ";
    }
#else
  for(int i=0;i<s2;i++)
    {
      ss << s[i] << " ";
    }
#endif
  // std::cout << "FuncCall: " << ss.str() << std::endl;

  //std::stringstream ss2(ss.str());
  T *ptr = &(ev.*api);
  RT val = (ptr->*fptr)(from_stream2<P>(ss,ev)...);

  //std::cout << "RETURN:" << val.id << std::endl;
  //std::cout << "RETURN TYPE:" << return_type << std::endl;
  
#if 0
  if (return_type.size()>2 && return_type[0]=='[' && return_type[return_type.size()-1]==']')
    { // array return type
      GameApi::ARR arr = add_array(ee, val);
      return arr.id;
    }
#endif
  
  //std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id; //template_get_id(val);
}


template<class T, class RT, class... P>
GameApiItem* ApiItemF(T (GameApi::EveryApi::*api), RT (T::*fptr)(P...),
		      std::string name,
		      std::vector<std::string> param_name,
		      std::vector<std::string> param_type,
		      std::vector<std::string> param_default,
		      std::string return_type, std::string api_name, std::string func_name, std::string symbols="", std::string comment="")
{
  GameApiItem *item = new ApiItem<T,RT,P...>(api, fptr, name, param_name, param_type, param_default, return_type, api_name, func_name, symbols,comment);
  g_api_item_deleter.items.push_back(item);
  return item;
}


