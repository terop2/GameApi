

#ifdef DEFINE_ALL
#define FEATURE_GLTF 1
#define FEATURE_VOXEL 1
#define FEATURE_VOLUME 1
#else
#define FEATURE_GLTF 1
#define FEATURE_VOXEL 1
#define FEATURE_VOLUME 1
#endif
#define FEATURE_FONT 1
#define FEATURE_MATERIAL 1
#define FEATURE_FRAMEBUFFER 1
#define FEATURE_PTS 1
#define FEATURE_WAVEFORM 1
#define FEATURE_GUI 1
#define FEATURE_BOOLEANOPS 1
#define FEATURE_OGL_SETUP 1

#ifdef DEFINE_ALL
#define ALL 1
#else
#include "GameApi_short_defines.hh"
#endif




#if (NEWSHADOW2_GLTF==1)||(GLTF_NEWSHADOW2==1)
#define FEATURE_GLTF 1
#endif
#if (SPHERE_BO==1)||(OR_ELEM_BO==1)||(INTERSECT_BO==1)||(CUBE_BO==1)||(FD_MIN==1)
#define FEATURE_VOLUME 1
#endif



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
#if (FEATURE_VOLUME==1)
MACRO2(GameApi::FD,ev.dist_api.fd_cube(0.0,0.0,0.0,0.0,0.0,0.0))
#endif
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
MACRO2(GameApi::FI,ev.font_api.load_font("http://meshpage.org/assets/Chunkfive.otf",20,20))
MACRO2(GameApi::HML,ev.mainloop_api.html_url("https://meshpage.org/assets/empty_hml.mp"))
MACRO2(GameApi::PTS,ev.points_api.pt_array(ev,std::vector<GameApi::PT>{ev.point_api.point(0.0,0.0,0.0)}))
MACRO2(GameApi::IBM,ev.bitmap_api.convert_fb_to_ibm_bitmap(ev.float_bitmap_api.fb_empty(10,10),0.0,0.3))
MACRO2(GameApi::TF,ev.mainloop_api.gltf_loadKK2("https://meshpage.org/assets/test.glb"))
#undef MACRO2

std::string empty_param(std::string s);  
std::string unique_id_apiitem();


template<class T>
class TypeToString { public: static constexpr const char * str=""; };

#define MAC(a) \
  template<> class TypeToString<GameApi::a> { public: static constexpr const char * str=#a; };

  MAC(BS)
  MAC(US)
  MAC(CS)
  MAC(CSI)
  MAC(TT)
  MAC(PBO)
  MAC(SHP)
  MAC(SHI)
  MAC(AA)
  MAC(GC)
  MAC(GS)
  MAC(HML)
  MAC(UV)
  MAC(AV)
  MAC(H)
  MAC(FBU)
  MAC(FML)
  MAC(CG)
  MAC(INP)
  MAC(IBM)
  MAC(DS)
  MAC(SBM)
  MAC(PN)
  MAC(DC)
  MAC(CBB)
  MAC(CFB)
  MAC(SI)
  MAC(PLP)
  MAC(PLL)
  MAC(PLF)
  MAC(PR)
  MAC(CMD)
  MAC(FI)
  MAC(SD)
  MAC(GI)
  MAC(FF)
  MAC(IF)
  MAC(PF)
  MAC(SF)
  MAC(ARR)
  MAC(PAR)
  MAC(CPP)
  MAC(PTT)
  MAC(KF)
    MAC(BLK)
  MAC(RUN)
  MAC(EV)
  MAC(AC)
  MAC(MX)
  MAC(Pa)
  MAC(Va)
  MAC(AS)
  MAC(MC)
  MAC(MS)
  MAC(MT)
  MAC(SMT)
  MAC(TL)
  MAC(MN)
  MAC(CP)
  MAC(DR)
  MAC(OM)
  MAC(FO)
  MAC(WV)
  MAC(BM)
  MAC(BMA)
  MAC(VBM)
  MAC(BB)
  MAC(FB)
  MAC(CBM)
  MAC(SP)
  MAC(SA)
  MAC(PT)
  MAC(V)
  MAC(LN)
  MAC(RM)
  MAC(IS)
  MAC(O)
  MAC(P)
  MAC(OO)
  MAC(PP)
  MAC(PA)
  MAC(LA)
  MAC(TN)
  MAC(SH)
  MAC(C)
  MAC(CO)
  MAC(F)
  MAC(FA)
  MAC(Ft)
  MAC(VF)
  MAC(PPT)
  MAC(PSP)
  MAC(MA)
  MAC(S)
  MAC(Vb)
  MAC(T)
  MAC(E)
  MAC(L)
  MAC(LAY)
  MAC(MP)
  MAC(MV)
  MAC(LL)
  MAC(ME)
  MAC(ST)
  MAC(TY)
  MAC(PS)
  MAC(ID)
  MAC(Str)
  MAC(Fi)
  MAC(Op)
  MAC(D)
  MAC(Ht)
  MAC(BF)
  MAC(VA)
  MAC(VAA)
  MAC(VX)
  MAC(PL)
  MAC(PLA)
  MAC(TX)
  MAC(TXID)
  MAC(TXA)
  MAC(TR)
  MAC(VV)
  MAC(Q)
  MAC(DO)
  MAC(PC)
  MAC(SV)
  MAC(FOA)
  MAC(COV)
  MAC(LI)
  MAC(LLA)
  MAC(FD)
  MAC(VO)
  MAC(PTS)
  MAC(MSA)
  MAC(PTA)
  MAC(RD)
  MAC(FBO)
  MAC(SM)
  MAC(TRK)
  MAC(WAV)
  MAC(TBUF)
  MAC(SFO)
  MAC(W)
  MAC(WM)
  MAC(FtA)
  MAC(ML)
  MAC(GML)
  MAC(TF)
  MAC(SHC)
  MAC(EX)
  MAC(PH)
  MAC(TS)
  MAC(CT)
  MAC(CC)
  MAC(IM)
  MAC(FFi)
  MAC(VFi)
  MAC(MB)
  MAC(PKG)
  MAC(ATT)
  MAC(CX)
#undef MAC


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

ApiSplit split_api_default(std::string def);


template<std::size_t ...>
struct add_all : std::integral_constant< std::size_t,0 > { };

template<std::size_t X, std::size_t ... Xs>
struct add_all<X,Xs...> : std::integral_constant< std::size_t, X+add_all<Xs...>::value> { };


inline bool print_if_false(const bool assertion, std::string msg) {
    if(!assertion) {
        // endl to flush
        std::cout << msg << std::endl;
    }
    return assertion;
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
  int ParamCount(int i) const {
    assert(print_if_false(sizeof...(P) == param_name.size(),"ERROR: param_name array size problems: " + api_name + "::" + name));
    assert(print_if_false(sizeof...(P) == param_type.size(),"ERROR: param_type array size problems: " + api_name + "::" + name));
    assert(print_if_false(sizeof...(P) == param_default.size(),"ERROR: param_default array size problems: " + name));
    return param_name.size(); }
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

    //std::cout << "PARAMS_SIZE:" << params.size() << "==" << param_name.size() << std::endl;
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
  

  std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)
  {
    std::pair<std::string,std::string> p = CodeGen_1(ev,params, param_names, param_type, return_type,api_name,func_name,j);
    return p;
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
	std::cout << "Warning: Empty slot in left side of node detected! " << TypeToString<lab>::str << std::endl; \
      return bm;\
  }\
  bm.id = to_int(s);\
  return bm;\
  }\
};
MACRO(GameApi::CS)
MACRO(GameApi::CSI)
MACRO(GameApi::GML)
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
MACRO(GameApi::BS)
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

#ifdef EMSCRIPTEN
#define ORDER 1
#endif
#ifdef ANDROID
#define ORDER 1
#endif
#ifdef RASPI
#define ORDER 1
#endif
#ifdef ARM2
#define ORDER 1
#endif
  
  //std::stringstream ss;
  int s2 = s.size();
#ifndef ORDER
  for(int i=s2-1;i>=0;i--)
    {
      ss << s[i] << " ";
    }
  //std::cout << "NO ORDER!" << std::endl;
#else
  for(int i=0;i<s2;i++)
    {
      ss << s[i] << " ";
    }
  // std::cout << "YES ORDER!" << std::endl;
#endif
  // std::cout << "FuncCall: " << ss.str() << std::endl;

  //std::stringstream ss2(ss.str());
  T *ptr = &(ev.*api);
  RT val = (ptr->*fptr)(from_stream2<P>(ss,ev)...);

  //std::cout << "RETURN:" << val.id << std::endl;
  //std::cout << "RETURN TYPE:" << return_type << std::endl;
   
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

