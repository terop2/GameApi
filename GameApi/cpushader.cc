
#include "GameApi_h.hh"

extern std::string gameapi_homepageurl;
std::string replace_str(std::string val, std::string repl, std::string subst);



struct ChildSpec
{
  CollectInterface2 *parent;
  int num;
  CollectInterface2 *child;
};

class VisitorImpl : public CollectVisitor2
{
public:
  virtual void register_obj(CollectInterface2 *i)
  {
    current = i;
    vec.push_back(i);
  }
  virtual void register_child(int num, CollectInterface2 *i)
  {
    ChildSpec s;
    s.parent = current;
    s.num = num;
    s.child = i;
    vec2.push_back(s);
  }
  void HeavyPrepare()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->HeavyPrepare();
      }
  }
  void set_inner(ShaderI2 *shader, int num)
  {
    ShaderI2 *child = 0;
    
    int s = vec2.size();
    for(int i=0;i<s;i++)
      {
	const ChildSpec &ss = vec2[i];
	if (ss.parent != shader) continue;
	int num2 = ss.num;
	child = (ShaderI2*)ss.child;
    
	std::string res;
	set_inner(child, num);
	switch(num)
	  {
	  case 0: res = child->get_webgl_header(); break;
	  case 1: res = child->get_webgl_function(); break;
	  case 2: res = child->get_win32_header(); break;
	  case 3: res = child->get_win32_function();
	  }

	shader->set_inner(num2,res);
      }
  }
private:
  CollectInterface2 *current;
  std::vector<CollectInterface2*> vec;
  std::vector<ChildSpec> vec2;
};



void Bindings::set(GameApi::EveryApi &ev, int ss)
{
  GameApi::SH sh;
  sh.id = ss;
      int s = f_vec.size();
    for(int i=0;i<s;i++)
      {
	FloatBinding b = f_vec[i];
	//std::cout << "FloatBinding:" << b.key << "=" << *b.value << std::endl;
	ev.shader_api.set_var(sh, b.key.c_str(), *b.value);
      }
    int s2 = i_vec.size();
    for(int i=0;i<s2;i++)
      {
	IntBinding b = i_vec[i];
	//std::cout << "IntBinding:" << b.key << "=" << *b.value << std::endl;
	ev.shader_api.set_var(sh,b.key.c_str(), *b.value);
      }
    int s3 = u_vec.size();
    for(int i=0;i<s3;i++)
      {
	UnsignedIntBinding b = u_vec[i];
	unsigned int value = *b.value;
	//std::cout << "UIntBinding:" << b.key << "=" << std::hex << *b.value << std::dec << std::endl;
	ev.shader_api.set_var(sh, b.key.c_str(),
			      ((value&0xff0000)>>16)/255.0,
			      ((value&0xff00)>>8)/255.0,
			      ((value&0xff))/255.0,
			      ((value&0xff000000)>>24)/255.0);
      }
     int s4 = p_vec.size();
     for(int i=0;i<s4;i++)
       {
	 PointBinding b = p_vec[i];
	 Point value = *b.value;
	 //std::cout << "PointBinding:" << b.key << "= (" << value.x << "," << value.y << "," << value.z << ")" << std::endl;
	 ev.shader_api.set_var(sh, b.key.c_str(), value.x,value.y,value.z);
       }
}

class NewBinding : public Bindings
{
  // uses protected default ctor.
};






class EmptyShaderParameter : public ShaderParameterI
{
public:
  virtual void set_time(float time) { }
  //virtual bool enabled() const=0;
  virtual float param_value_f(int i) const { return 0.0; }
  virtual int param_value_i(int i) const { return 0; }
  virtual unsigned int param_value_u(int i) const { return 0xffffffff; }
  virtual Point param_value_p3d(int i) const { return Point(0.0,0.0,0.0); }
  virtual Point param_value_uvw(int i) const { return Point(0.0,0.0,0.0); }
};

class ConstantShaderParameter : public ShaderParameterI
{
public:
  ConstantShaderParameter(ShaderParameterI *next, int num, float value) : next(next), num(num), f(value) { type=0; }
  ConstantShaderParameter(ShaderParameterI *next, int num, int value) : next(next), num(num), ii(value) { type=1;}
  ConstantShaderParameter(ShaderParameterI *next, int num, unsigned int value) : next(next), num(num), u(value) { type=2; }
  ConstantShaderParameter(ShaderParameterI *next, int num, Point value) : next(next), num(num), p3d(value) { type=3; }
  ConstantShaderParameter(ShaderParameterI *next, int num, Point value, int dummy) : next(next), num(num), uvw(value) { type=4; }
  virtual void set_time(float time) { next->set_time(time); }
  virtual bool enabled() const { return true; }
  virtual float param_value_f(int i) const {
    if (type==0 && i==num) return f;
    return next->param_value_f(i);
  }
  virtual int param_value_i(int i) const
  {
    if (type==1 && i==num) return ii;
    return next->param_value_i(i);
  }
  virtual unsigned int param_value_u(int i) const
  {
    if (type==2 && i==num) return u;
    return next->param_value_u(i);
  }
  virtual Point param_value_p3d(int i) const
  {
    if (type==3 && i==num) return p3d;
    return next->param_value_p3d(i);
  }
  virtual Point param_value_uvw(int i) const
  {
    if(type==4 && i==num) return uvw;
    return next->param_value_uvw(i);
  }
private:
  ShaderParameterI *next;
  int num;
  float f;
  int ii;
  unsigned int u;
  Point p3d;
  Point uvw;
  int type;
};
class TimedShaderParameter : public ShaderParameterI
{
public:
  TimedShaderParameter(float start_time, float end_time, ShaderParameterI *next, int num, float start_value, float end_value) : start_time(start_time), end_time(end_time), next(next), num(num), start_value(start_value), end_value(end_value) { type=0;}
  TimedShaderParameter(float start_time, float end_time, ShaderParameterI *next, int num, int start_value, int end_value) : start_time(start_time), end_time(end_time), next(next), num(num), start_value_i(start_value), end_value_i(end_value) { type=1; }
  TimedShaderParameter(float start_time, float end_time, ShaderParameterI *next, int num, unsigned int start_value, unsigned int end_value) : start_time(start_time), end_time(end_time), next(next), num(num), start_value_u(start_value), end_value_u(end_value) { type=2;}
  TimedShaderParameter(float start_time, float end_time, ShaderParameterI *next, int num, Point start_value, Point end_value) : start_time(start_time), end_time(end_time), next(next), num(num), start_value_p3d(start_value), end_value_p3d(end_value) { type=3;}
  TimedShaderParameter(float start_time, float end_time, ShaderParameterI *next, int num, Point start_value, Point end_value, int dummy) : start_time(start_time), end_time(end_time), next(next), num(num), start_value_uvw(start_value), end_value_uvw(end_value) { type=4;}
  virtual void set_time(float time) { current_time=time; next->set_time(time); }
  virtual bool enabled() const
  {
    return current_time >= start_time && current_time<=end_time;
  }
  virtual float param_value_f(int i) const
  {
    if (type==0 &&i==num && enabled()) {
      float dt = (current_time-start_time)/(end_time-start_time);
      dt*=(end_value-start_value);
      dt+=start_value;
      return dt;
    }
    return next->param_value_f(i);
  }
  virtual int param_value_i(int i) const
  {
    if (type==1 &&i==num&& enabled()) {
      float dt = (current_time-start_time)/(end_time-start_time);
      dt*=(end_value_i-start_value_i);
      dt+=start_value_i;
      return int(dt);
    }
    return next->param_value_i(i);
  }
  virtual unsigned int param_value_u(int i) const
  {
    //std::cout << "param_value_u" << i << " " << type << " " << num << " " << enabled() << " " << current_time << " " << start_time << " " << end_time << std::endl;
    if (type==2 &&i==num&& enabled()) {
      float dt = (current_time-start_time)/(end_time-start_time);
      unsigned int res = Color::Interpolate(start_value_u, end_value_u, dt);
      //std::cout << "returning " << std::hex << res << std::dec << std::endl;
      return res;
    }
    return next->param_value_u(i);
  }
  virtual Point param_value_p3d(int i) const
  {
    if (type==3 &&i==num&& enabled()) {
      float dt = (current_time-start_time)/(end_time-start_time);
      float dt2 = dt;
      float dt3 = dt;
      dt*=(end_value_p3d.x-start_value_p3d.x);
      dt+=start_value_p3d.x;
      dt2*=(end_value_p3d.y-start_value_p3d.y);
      dt2+=start_value_p3d.y;
      dt3*=(end_value_p3d.z-start_value_p3d.z);
      dt3+=start_value_p3d.z;
      return Point(dt,dt2,dt3);
    }
    return next->param_value_p3d(i);
  }
  virtual Point param_value_uvw(int i) const
  {
    if (type==2 &&i==num&& enabled()) {
      float dt = (current_time-start_time)/(end_time-start_time);
      float dt2 = dt;
      float dt3 = dt;
      dt*=(end_value_uvw.x-start_value_uvw.x);
      dt+=start_value_uvw.x;
      dt2*=(end_value_uvw.y-start_value_uvw.y);
      dt2+=start_value_uvw.y;
      //dt3*=(end_value_uvw.z-start_value_uvw.z);
      //dt3+=start_value_uvw.z;
      if (dt3<0.5f)
	{
	  dt3 = start_value_uvw.z;
	} else
	{
	  dt3 = end_value_uvw.z;
	}
      return Point(dt,dt2,dt3);
    }
    return next->param_value_uvw(i);
  }
private:
  float current_time;
  float start_time;
  float end_time;
  ShaderParameterI *next;
  int num;
  float start_value;
  float end_value;
  int start_value_i;
  int end_value_i;
  unsigned int start_value_u;
  unsigned int end_value_u;
  Point start_value_p3d;
  Point end_value_p3d;
  Point start_value_uvw;
  Point end_value_uvw;
  int type;
};
GameApi::SHP GameApi::MainLoopApi::empty_shp()
{
  return add_shp(e, new EmptyShaderParameter);
}
GameApi::SHP GameApi::MainLoopApi::constant_shp_f(SHP next, int num, float value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new ConstantShaderParameter(i,num,value));
}
GameApi::SHP GameApi::MainLoopApi::constant_shp_i(SHP next, int num, int value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new ConstantShaderParameter(i,num,value));
}
GameApi::SHP GameApi::MainLoopApi::constant_shp_u(SHP next, int num, unsigned int value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new ConstantShaderParameter(i,num,value));
}
GameApi::SHP GameApi::MainLoopApi::constant_shp_p3d(SHP next, int num, PT value)
{
  Point *pt = find_point(e,value);
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new ConstantShaderParameter(i,num,*pt));
}
GameApi::SHP GameApi::MainLoopApi::constant_shp_uvw(SHP next, int num, PT value)
{
  Point *pt = find_point(e,value);
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new ConstantShaderParameter(i,num,*pt,0));
}

GameApi::SHP GameApi::MainLoopApi::timed_shp_f(float start_time, float end_time, SHP next, int num, float start_value, float end_value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new TimedShaderParameter(start_time, end_time, i,num,start_value,end_value));
}
GameApi::SHP GameApi::MainLoopApi::timed_shp_i(float start_time, float end_time, SHP next, int num, int start_value, int end_value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new TimedShaderParameter(start_time, end_time, i,num,start_value,end_value));
}
GameApi::SHP GameApi::MainLoopApi::timed_shp_u(float start_time, float end_time, SHP next, int num, unsigned int start_value, unsigned int end_value)
{
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new TimedShaderParameter(start_time, end_time, i,num,start_value,end_value));
}
GameApi::SHP GameApi::MainLoopApi::timed_shp_p3d(float start_time, float end_time, SHP next, int num, PT start_value, PT end_value)
{
  Point *sv = find_point(e,start_value);
  Point *ev = find_point(e,end_value);
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new TimedShaderParameter(start_time, end_time, i,num,*sv,*ev));
}
GameApi::SHP GameApi::MainLoopApi::timed_shp_uvw(float start_time, float end_time, SHP next, int num, PT start_value, PT end_value)
{
  Point *sv = find_point(e,start_value);
  Point *ev = find_point(e,end_value);
  
  ShaderParameterI *i = find_shp(e, next);
  return add_shp(e,new TimedShaderParameter(start_time, end_time, i,num,*sv,*ev,0));
}



class EmptyShaderI : public ShaderI2
{
public: 
  virtual void HeavyPrepare() { }
  virtual void Collect(CollectVisitor2 &vis) { }
  virtual void set_inner(int num, std::string value) { }
  virtual std::string get_webgl_header() const { return ""; }
  virtual std::string get_win32_header() const { return ""; }
  virtual std::string get_webgl_function() const { return ""; }
  virtual std::string get_win32_function() const { return ""; }
  virtual Bindings set_var(const Bindings &b) { return b; }
  virtual std::string get_flags() const { return ""; }
  virtual std::string func_name() const { return ""; }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
};

GameApi::SHI GameApi::MainLoopApi::empty_shaderI()
{
  return add_shaderI(e, new EmptyShaderI);
}



class PhongVertex : public ShaderI2
{
public:
  PhongVertex(ShaderI2 *next) : next(next) { }
  virtual void HeavyPrepare() { }
  virtual void Collect(CollectVisitor2 &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
    vis.register_child(0, next);
  }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual void set_inner(int num, std::string value) { inner = value; }
  virtual std::string get_webgl_header() const
  {
    return "";
  }
  virtual std::string get_win32_header() const { return ""; }
  virtual std::string get_webgl_function() const
  {
    return inner +
      //"#ifdef IN_NORMAL\n"
      //"#ifdef LIGHTDIR\n"
      //"#ifdef EX_NORMAL2\n"
      //"#ifdef EX_LIGHTPOS2\n"
      "vec4 phong2(vec4 pos)\n"
      "{\n"
      "    vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"    
      "    ex_Normal2 = n;\n"
      "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"
      "    return pos;\n"
      "}\n";
      //"#endif\n"
      //"#endif\n"
      //"#endif\n"
      //"#endif\n";
      ;

  }
  virtual std::string get_win32_function() const
  {
    return inner+
      //"#ifdef IN_NORMAL\n"
      //"#ifdef LIGHTDIR\n"
      //"#ifdef EX_NORMAL2\n"
      //"#ifdef EX_LIGHTPOS2\n"
      "vec4 phong2(vec4 pos)\n"
      "{\n"
      "    vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"    
      "    ex_Normal2 = n;\n"
      "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"
      "    return pos;\n"
      "}\n"
      //"#endif\n"
      //"#endif\n"
      //"#endif\n"
      //"#endif\n";
      ;
  }
  virtual Bindings set_var(const Bindings &b) { return b; }
  virtual std::string get_flags() const { return "EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"; }
  virtual std::string func_name() const { return "phong2"; }
private:
  ShaderI2 *next;
  std::string inner;
};
GameApi::SHI GameApi::MainLoopApi::phong_vertex(SHI next)
{
  ShaderI2 *next2 = find_shaderI(e,next);
  return add_shaderI(e, new PhongVertex(next2));
}


class PhongFragment : public ShaderI2
{
public:
  PhongFragment(ShaderI2 *next,unsigned int ambient, unsigned int highlight, float pow) : next(next), ambient(ambient), highlight(highlight), pow(pow) { }
  virtual void HeavyPrepare() { }
  virtual void Collect(CollectVisitor2 &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
    vis.register_child(0, next);
  }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual void set_inner(int num, std::string value) { inner=value; } 
  virtual std::string get_webgl_header() const { return inner; }
  virtual std::string get_win32_header() const { return inner; }
  virtual std::string get_webgl_function() const
  {
return inner+
  //"#ifdef EX_NORMAL2\n"
  //"#ifdef EX_LIGHTPOS2\n"
  //"#ifdef LEVELS\n"
  
  "float rr_intensity(vec3 dir) {\n"
  " float n = clamp(dot(normalize(-dir),normalize(ex_LightPos2)),0.0,1.0);\n"
  " return n;\n"
  "}\n"
  "float rr_intensity2(vec3 dir) {\n"
  " float i=rr_intensity(dir); return pow(i,hilight);\n"
  "}\n"
  
  "const float rr_GAMMA2=2.2;\n"
  "const float rr_INV_GAMMA2 = 1.0/rr_GAMMA2;\n"
  "vec3 rr_LINEARtoSRGB2(vec3 color)\n"
  "{\n"
  " return pow(color, vec3(rr_INV_GAMMA2));\n"
  "}\n"
  "vec3 rr_SRGBtoLINEAR2(vec3 srgbIn)\n"
  "{\n"
  "  return pow(srgbIn.xyz,vec3(rr_GAMMA2));\n"
  "}\n"
  "uniform vec4 cc1;\n"
  "uniform vec4 cc2;\n"
  "vec4 phong2(vec4 rgb)\n"
  "{\n"
  "    vec3 c = vec3(0.0,0.0,0.0);\n"
  "    vec3 normal = ex_Normal2;\n"
  "    c+=rr_SRGBtoLINEAR2(rgb.rgb);\n"
  //"#ifdef PHONG_TEXTURE\n"
  //"    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb*rgb.rgb,vec3(1,1,1),0.15));\n"
  //"    c+=mix(intensity2(normal)*SRGBtoLINEAR2(level2_color.rgb),rgb.rgb,0.7);\n"
  //"#endif\n"
  //"#ifndef PHONG_TEXTURE\n"
  "    c+=rr_intensity(normal)*rr_SRGBtoLINEAR2(mix(cc1.rgb,vec3(1,1,1),0.1));\n"
  "    c+=rr_intensity2(normal)*rr_SRGBtoLINEAR2(cc2.rgb);\n"
  //"#endif\n"
  
  "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
  //"    return vec4(rr_LINEARtoSRGB2(c),rgb.a);\n"
  "      return vec4(c,rgb.a);\n"
  "}\n"
  //"#endif\n"
  //"#endif\n"
  //"#endif\n";
  ;
  }
  virtual std::string get_win32_function() const
  {
    return inner+
      //"#ifdef EX_NORMAL2\n"
      //"#ifdef EX_LIGHTPOS2\n"
      //"#ifdef LEVELS\n"
      
      "float rr_intensity(vec3 dir) {\n"
      " float n = clamp(dot(normalize(-dir),normalize(ex_LightPos2)),0.0,1.0);\n"
      " return n;\n"
      "}\n"
      "float rr_intensity2(vec3 dir) {\n"
      " float i=rr_intensity(dir); return pow(i,hilight);\n"
      "}\n"
      
      "const float rr_GAMMA2=2.2;\n"
      "const float rr_INV_GAMMA2 = 1.0/rr_GAMMA2;\n"
      "vec3 rr_LINEARtoSRGB2(vec3 color)\n"
      "{\n"
      " return pow(color, vec3(rr_INV_GAMMA2));\n"
      "}\n"
      "vec3 rr_SRGBtoLINEAR2(vec3 srgbIn)\n"
      "{\n"
      "  return pow(srgbIn.xyz,vec3(rr_GAMMA2));\n"
      "}\n"
  "uniform vec4 cc1;\n"
  "uniform vec4 cc2;\n"
      
      "vec4 phong2(vec4 rgb)\n"
      "{\n"
      "    vec3 c = vec3(0.0,0.0,0.0);\n"
      "    vec3 normal = ex_Normal2;\n"
      "    c+=rr_SRGBtoLINEAR2(rgb.rgb);\n"
      //"#ifdef PHONG_TEXTURE\n"
      //"    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb*rgb.rgb,vec3(1,1,1),0.15));\n"
      //"    c+=mix(intensity2(normal)*SRGBtoLINEAR2(level2_color.rgb),rgb.rgb,0.7);\n"
      //"#endif\n"
      //"#ifndef PHONG_TEXTURE\n"
      "    c+=rr_intensity(normal)*rr_SRGBtoLINEAR2(mix(cc1.rgb,vec3(1,1,1),0.1));\n"
      "    c+=rr_intensity2(normal)*rr_SRGBtoLINEAR2(cc2.rgb);\n"
      //"#endif\n"
      
      "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
      "    return vec4(rr_LINEARtoSRGB2(c),rgb.a);\n"
      "}\n"
      //"#endif\n"
      //"#endif\n"
      //"#endif\n";
      ;
  }
  virtual Bindings set_var(const Bindings &b)
  {
    Bindings b2(b,ambi_bind);
    Bindings b3(b2,high_bind);
    Bindings b4(b3,pow_bind);
    return b4;
  }
  virtual std::string get_flags() const { return "EX_NORMAL2 EX_LIGHTPOS2 LEVELS"; }
  virtual std::string func_name() const { return "phong2"; }
private:
  ShaderI2 *next;
  unsigned int ambient;
  unsigned int highlight;
  float pow;
  UnsignedIntBinding ambi_bind = { "cc1", &ambient };
  UnsignedIntBinding high_bind = { "cc2", &highlight };
  FloatBinding pow_bind = { "hilight", &pow };
  std::string inner;
};

GameApi::SHI GameApi::MainLoopApi::phong_fragment(SHI next, unsigned int ambient, unsigned int highlight, float pow)
{
  ShaderI2 *next2 = find_shaderI(e,next);
  return add_shaderI(e, new PhongFragment(next2,ambient, highlight, pow));
}


class GenericShaderML : public MainLoopItem
{
public:
  GenericShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, ShaderI2 *vertex, ShaderI2 *fragment) : env(env), ev(ev), next(next), vertex(vertex), fragment(fragment) {
    firsttime = true;
    sh.id = -1;
    initialized=false;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
    vertex->handle_event(e);
    fragment->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    vertex->Collect(ii);
    fragment->Collect(ii);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (initialized==false) {
      ii.HeavyPrepare();
    }
    initialized=true;
  }
  void Prepare() {
    next->Prepare();
    HeavyPrepare();
    initialized=true;
  }
  void execute(MainLoopEnv &e)
  {
    vertex->execute(e);
    fragment->execute(e);
    if (!initialized) return;
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex2;
    vertex2.id = ee.us_vertex_shader;
    if (vertex2.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      ee.us_vertex_shader = a0.id;
    }
    vertex2.id = ee.us_vertex_shader;
    vertex2 = ev.uber_api.v_generic(vertex2,vertex->func_name(),vertex->get_flags());
    ee.us_vertex_shader = vertex2.id;
#if OPENGL_ES
    ii.set_inner(vertex, 0);
    std::string header = vertex->get_webgl_header();
    ii.set_inner(vertex, 1);
    std::string function = vertex->get_webgl_function();
    ee.v_shader_functions += header + function;
#else
    ii.set_inner(vertex, 2);
    std::string header = vertex->get_win32_header();
    ii.set_inner(vertex, 3);
    std::string function = vertex->get_win32_function();
    ee.v_shader_functions += header + function;
#endif
    
    GameApi::US fragment2;
    fragment2.id = ee.us_fragment_shader;
    if (fragment2.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment2.id = ee.us_fragment_shader;
    //if (ambient) {
      fragment2 = ev.uber_api.f_generic(fragment2,fragment->func_name(), fragment->get_flags());
      //}
    ee.us_fragment_shader = fragment2.id;
#if OPENGL_ES
    ii.set_inner(fragment, 0);
    std::string header2 = fragment->get_webgl_header();
    ii.set_inner(fragment, 1);
    std::string function2 = fragment->get_webgl_function();

    ee.f_shader_functions += header2 + function2;
#else
    ii.set_inner(fragment, 2);
    std::string header2 = fragment->get_win32_header();
    ii.set_inner(fragment, 3);
    std::string function2 = fragment->get_win32_function();

    ee.f_shader_functions += header2 + function2;
#endif
    
      }
    
	     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
     //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);
	
	NewBinding bb;
	Bindings b = vertex->set_var(bb);
	Bindings b2 = fragment->set_var(b);
	b2.set(ev,sh.id);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  ShaderI2 *vertex;
  ShaderI2 *fragment;
  VisitorImpl ii;
  bool firsttime;
  bool initialized;
  GameApi::SH sh;
};


class GenericShaderML_fragment_only : public MainLoopItem
{
public:
  GenericShaderML_fragment_only(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, ShaderI2 *fragment) : env(env), ev(ev), next(next), fragment(fragment) {
    firsttime = true;
    sh.id = -1;
    initialized=false;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
    fragment->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    fragment->Collect(ii);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (initialized==false) {
      ii.HeavyPrepare();
    }
    initialized=true;
  }
  void Prepare() {
    next->Prepare();
    fragment->Collect(ii);
    HeavyPrepare();
    initialized=true;
  }
  void execute(MainLoopEnv &e)
  {
    fragment->execute(e);
    //std::cout << "init=" << initialized << " firsttime=" << firsttime << std::endl;
    if (!initialized) return;
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex2;
    vertex2.id = ee.us_vertex_shader;
    if (vertex2.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      ee.us_vertex_shader = a0.id;
    }
    vertex2.id = ee.us_vertex_shader;
    //vertex2 = ev.uber_api.v_generic(vertex2,vertex->func_name(),vertex->get_flags());
    ee.us_vertex_shader = vertex2.id;
#if OPENGL_ES
    //ii.set_inner(vertex, 0);
    //std::string header = vertex->get_webgl_header();
    //ii.set_inner(vertex, 1);
    //std::string function = vertex->get_webgl_function();
    //ee.v_shader_functions += header + function;
#else
    // ii.set_inner(vertex, 2);
    //std::string header = vertex->get_win32_header();
    //ii.set_inner(vertex, 3);
    //std::string function = vertex->get_win32_function();
    //ee.v_shader_functions += header + function;
#endif
    
    GameApi::US fragment2;
    fragment2.id = ee.us_fragment_shader;
    if (fragment2.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment2.id = ee.us_fragment_shader;
    //if (ambient) {
      fragment2 = ev.uber_api.f_generic_flip(fragment2,fragment->func_name(), fragment->get_flags());
      //}
    ee.us_fragment_shader = fragment2.id;
#if OPENGL_ES
    ii.set_inner(fragment, 0);
    std::string header2 = fragment->get_webgl_header();
    ii.set_inner(fragment, 1);
    std::string function2 = fragment->get_webgl_function();

    ee.f_shader_functions += header2 + function2;
#else
    ii.set_inner(fragment, 2);
    std::string header2 = fragment->get_win32_header();
    ii.set_inner(fragment, 3);
    std::string function2 = fragment->get_win32_function();

    ee.f_shader_functions += header2 + function2;
#endif
    
      }
    
	     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
     //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);
	
	NewBinding bb;
	//Bindings b = vertex->set_var(bb);
	Bindings b2 = fragment->set_var(bb);
	b2.set(ev,sh.id);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  //ShaderI2 *vertex;
  ShaderI2 *fragment;
  VisitorImpl ii;
  bool firsttime;
  bool initialized;
  GameApi::SH sh;
};

GameApi::ML GameApi::MainLoopApi::generic_shader_fragment_only(GameApi::EveryApi &ev, ML ml, SHI fragment)
{
  MainLoopItem *next = find_main_loop(e,ml);
  ShaderI2 *frag = find_shaderI(e,fragment);
  return add_main_loop(e, new GenericShaderML_fragment_only(e,ev,next,frag));
}

GameApi::ML GameApi::MainLoopApi::generic_shader(GameApi::EveryApi &ev, ML ml, SHI vertex, SHI fragment)
{
  MainLoopItem *next = find_main_loop(e,ml);
  ShaderI2 *ver = find_shaderI(e,vertex);
  ShaderI2 *frag = find_shaderI(e,fragment);
  return add_main_loop(e, new GenericShaderML(e,ev,next,ver,frag));
}

GameApi::ML GameApi::MainLoopApi::phong_shader3(GameApi::EveryApi &ev, ML ml, unsigned int ambient, unsigned int highlight, float pow)
{
  SHI vertex_next = ev.mainloop_api.empty_shaderI();
  SHI fragment_next = ev.mainloop_api.empty_shaderI();
  return generic_shader(ev,ml,ev.mainloop_api.phong_vertex(vertex_next),ev.mainloop_api.phong_fragment(fragment_next,ambient,highlight,pow));
}


class MaterialForward : public Material
{
public:
  GameApi::ML call(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = mat(p.id);
    return ml;
  }
  GameApi::ML call_inst(GameApi::P p, GameApi::PTS pts)
  {
    GameApi::ML ml;
    ml.id = mat_inst(p.id,pts.id);
    return ml;
  }
  GameApi::ML call_inst_matrix(GameApi::P p, GameApi::MS ms)
  {
    GameApi::ML ml;
    ml.id = mat_inst_matrix(p.id,ms.id);
    return ml;
  }

  int mat(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::ML ml = mat2(p2);
    return ml.id;
  }
  int mat_inst(int p, int pts) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst(p2,p3);
    return ml.id;
  }
  int mat_inst_matrix(int p, int ms) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::MS p3;
    p3.id = ms;
    GameApi::ML ml = mat2_inst_matrix(p2,p3);
    return ml.id;
  }

  int mat_inst2(int p, int pta) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTA p3;
    p3.id = pta;
    GameApi::ML ml = mat2_inst2(p2,p3);
    return ml.id;

  }
  int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat_inst_fade(p2,p3, flip, start_time, end_time);
    return ml.id;

  }
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};



class PhongMaterial2 : public MaterialForward
{
public:
  PhongMaterial2(GameApi::Env &env, GameApi::EveryApi &ev, Material *next, unsigned int ambient, unsigned int highlight, float pow) : env(env), ev(ev), next(next), ambient(ambient), highlight(highlight), pow(pow) { }

  void logoexecute() { next->logoexecute(); }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    Vector v = coll->PointNormal(0,0);

    GameApi::P p0 = p;
    if (v.Dist()<0.01)
      p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::ML ml;
    ml.id = next->mat(p0.id);
    GameApi::ML sh;
      sh = ev.mainloop_api.phong_shader3(ev, ml, ambient, highlight, pow);
    return sh;
  }

  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    Vector v = coll->PointNormal(0,0);

    GameApi::P p0 = p;
    if (v.Dist()<0.01)
      p0 = ev.polygon_api.recalculate_normals(p);

    GameApi::ML ml;
    ml.id = next->mat_inst(p0.id, pts.id);
    GameApi::ML sh;
      sh = ev.mainloop_api.phong_shader3(ev, ml, ambient, highlight, pow);

    return sh;

  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    Vector v = coll->PointNormal(0,0);

    GameApi::P p0 = p;
    if (v.Dist()<0.01)
      p0 = ev.polygon_api.recalculate_normals(p);

    GameApi::ML ml;
    ml.id = next->mat_inst_matrix(p0.id, ms.id);
    GameApi::ML sh;
    sh = ev.mainloop_api.phong_shader3(ev, ml, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    Vector v = coll->PointNormal(0,0);

    GameApi::P p0 = p;
    if (v.Dist()<0.01)
      p0 = ev.polygon_api.recalculate_normals(p);

    GameApi::ML ml;
    ml.id = next->mat_inst2(p0.id, pta.id);
    GameApi::ML sh;
    sh = ev.mainloop_api.phong_shader3(ev, ml, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    Vector v = coll->PointNormal(0,0);

    GameApi::P p0 = p;
    if (v.Dist()<0.01)
      p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p0.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh;
      sh = ev.mainloop_api.phong_shader3(ev, ml, ambient, highlight, pow);

    return sh;

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int ambient;
  unsigned int highlight;
  float pow;
};

GameApi::MT GameApi::MaterialsApi::phong3_material(EveryApi &ev, MT next, unsigned int ambient, unsigned int highlight, float pow)
{
  Material *mat = find_material(e,next);
  return add_material(e, new PhongMaterial2(e,ev, mat, ambient, highlight, pow));
}

int find_str(std::string val, std::string repl);

class GenericAnimShaderI : public ShaderI2
{
public:
  GenericAnimShaderI(ShaderParameterI *params, std::string funcname, std::string shadercode, std::vector<ShaderI2*> child) : params(params), funcname(funcname), shadercode(shadercode), child(child) { }
  void divide()
  {
    std::string s = shadercode;
    std::stringstream ss(s);
    std::string line;
    bool webgl = false;
    bool win32 = false;
    int linenum = 0;
    while(std::getline(ss,line))
      {
	if (line=="") continue;
	std::string word;
	std::stringstream ss2(line);
	ss2 >> word;
	if (word == "//flags:")
	  {
	    flags = replace_str(line, "//flags:", "");
	  }
	if (word == "//webgl")
	  {
	    webgl=true;
	    win32=false;
	  }
	if (word == "//win32")
	  {
	    webgl=false;
	    win32=true;
	  }
	if (word == "uniform")
	  {
	    if (webgl)
	      uniform_lines_webgl.push_back(line);
	    else if (win32)
	      uniform_lines_win32.push_back(line);
	    else
	      std::cout << "ERROR(" << linenum << "): You should mark line areas with //webgl or //win32" << std::endl;
	  } else
	  {
	    if (webgl)
	      shader_lines_webgl.push_back(line);
	    else if (win32)
	      shader_lines_win32.push_back(line);
	    else
	      std::cout << "ERROR(" << linenum << "): You should mark line areas with //webgl or //win32" << std::endl;
	  }
	
	linenum++;	
      }
  }
  virtual void HeavyPrepare() { }
  virtual void Collect(CollectVisitor2 &vis)
  {
    vis.register_obj(this);
    int i=0;
    for(;;i++)
      {
	std::stringstream ss;
	ss << i;
	int pos = find_str(shadercode,std::string("%")+ss.str());
	if (pos==-1) break;
	vis.register_child(i,child[i]);
      }
    inner.resize(i);
    divide();
  }

  virtual void set_inner(int num, std::string value)
  {
    inner[num] = value;
  }
  virtual std::string get_webgl_header() const
  {
    std::string res;
    int s = uniform_lines_webgl.size();
    for(int i=0;i<s;i++)
      {
	res+=uniform_lines_webgl[i] + "\n";
      }
    return res;
  }
  virtual std::string get_win32_header() const
  {
    std::string res;
    int s = uniform_lines_win32.size();
    for(int i=0;i<s;i++)
      {
	res+=uniform_lines_win32[i] + "\n";
      }
    return res;
  }
  virtual std::string get_webgl_function() const
  {
    std::string res;
    int s = shader_lines_webgl.size();
    for(int i=0;i<s;i++)
      {
	res+=shader_lines_webgl[i] + "\n";
      }
    return res;
  }
  virtual std::string get_win32_function() const
  {
    std::string res;
    int s = shader_lines_win32.size();
    for(int i=0;i<s;i++)
      {
	res+=shader_lines_win32[i] + "\n";
      }
    return res;
  }
  virtual Bindings set_var(const Bindings &b)
  {
    //std::cout << "set_var.." << std::endl;
    if (m_binding) return *m_binding;
    std::vector<std::string> *ptr;
#ifdef OPENGL_ES
    ptr = &uniform_lines_webgl;
#else
    ptr = &uniform_lines_win32;
#endif
    int s = ptr->size();
    const Bindings *curr = &b;
    floats=std::vector<float>();
    ints=std::vector<int>();
    uints=std::vector<unsigned int>();
    p3ds=std::vector<Point>();
    uvws=std::vector<Point>();
    floats.reserve(30);
    ints.reserve(30);
    uints.reserve(30);
    p3ds.reserve(30);
    uvws.reserve(30);
    for(int i=0;i<s;i++)
      {
	if (floats.size()>30||ints.size()>30||uints.size()>30||p3ds.size()>30||uvws.size()>30)
	  {
	    std::cout << "Warning: Generic shaders not guaranteed to work above 30 uniforms." << std::endl;
	  }
	std::string line = ptr->operator[](i);
	std::stringstream ss(line);
	std::string uniform;
	std::string type;
	std::string name;
	std::string comment;
	ss >> uniform >> type >> name >> comment;
	if (name[name.size()-1]==';') name=name.substr(0,name.size()-1);
	if (type=="float") {
	  floats.push_back(0.0);
	  FloatBinding f;
	  f.key = name;
	  f.value = &floats[floats.size()-1];
	  curr = new Bindings(*curr,f);
	}
	if (type=="int") {
	  ints.push_back(0);
	  IntBinding f;
	  f.key = name;
	  f.value = &ints[ints.size()-1];
	  curr = new Bindings(*curr, f);
	}
	if (type=="vec4") {
	  uints.push_back(0xffffffff);
	  UnsignedIntBinding f;
	  f.key = name;
	  f.value = &uints[uints.size()-1];
	  curr = new Bindings(*curr,f);
	}
	if (type=="vec3" && comment=="") {
	  p3ds.push_back(Point(0.0,0.0,0.0));
	  PointBinding f;
	  f.key = name;
	  f.value = &p3ds[p3ds.size()-1];
	  curr = new Bindings(*curr, f);
	}
	if (type=="vec3" && comment=="//uv") {
	  uvws.push_back(Point(0.0,0.0,0.0));
	  PointBinding f;
	  f.key = name;
	  f.value = &uvws[uvws.size()-1];
	  curr = new Bindings(*curr, f);
	}
      }
    m_binding = curr;
    return *curr;
  }
  virtual std::string get_flags() const { return flags; }
  virtual std::string func_name() const { return funcname; }
  virtual void execute(MainLoopEnv &e) {
    params->set_time(e.time);
    int s = floats.size();
    for(int i=0;i<s;i++)
      {
	floats[i] = params->param_value_f(i);
      }
    int s2 = ints.size();
    for(int i=0;i<s2;i++)
      {
	ints[i] = params->param_value_i(i);
      }
    int s3 = uints.size();
    for(int i=0;i<s3;i++)
      {
	uints[i] = params->param_value_u(i);
      }
    int s4 = p3ds.size();
    for(int i=0;i<s4;i++)
      {
	p3ds[i] = params->param_value_p3d(i);
      }
    int s5 = uvws.size();
    for(int i=0;i<s5;i++)
      {
	uvws[i] = params->param_value_uvw(i);
      }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
private:
  ShaderParameterI *params;
  std::string funcname;
  std::string shadercode;
  std::vector<ShaderI2*> child;
  std::vector<std::string> inner;
  std::vector<std::string> uniform_lines_webgl;
  std::vector<std::string> shader_lines_webgl;
  std::vector<std::string> uniform_lines_win32;
  std::vector<std::string> shader_lines_win32;
  // these 3 should be passed to dialogs etc for asking parameters.
  std::vector<float> floats;
  std::vector<int> ints;
  std::vector<unsigned int> uints;
  std::vector<Point> p3ds;
  std::vector<Point> uvws;
  const Bindings *m_binding=0;
  std::string flags;
};

void LOAD_CB(void *);

class LoadShader : public ShaderCode
{
public:
  LoadShader(GameApi::Env &env, std::string shader_url, std::string homepage) : env(env), shader_url(shader_url), homepage(homepage) {
    env.async_load_callback(shader_url, &LOAD_CB, (void*)this);
    firsttime = true;
  }
  void Prepare2()
  {
    if (firsttime) {
      firsttime = false;
      //std::cout << "LoadShader Loaded " << shader_url << std::endl;
#ifndef EMSCRIPTEN
    env.async_load_url(shader_url, homepage);
#endif
    GameApi::ASyncVec *ptr = env.get_loaded_async_url(shader_url);
    if (!ptr) std::cout << "ERROR: Shader async load error:" << shader_url << std::endl;
    std::string s(ptr->begin(),ptr->end());
    shadercode = s;
    }
  }
  std::string Code() const { if (shadercode=="") const_cast<LoadShader*>(this)->Prepare2(); return shadercode; }
private:
  GameApi::Env &env;
  ShaderI2 *next;
  std::string shader_url;
  std::string homepage;
  std::string shadercode;
  bool firsttime;
};

void LOAD_CB(void *p)
{
  LoadShader *sh = (LoadShader*)p;
  sh->Prepare2();
}
GameApi::SHC GameApi::MainLoopApi::load_shader(std::string shader_url)
{
  return add_shader_code(e, new LoadShader(e,shader_url,gameapi_homepageurl));
}
GameApi::ARR GameApi::MainLoopApi::load_shader2(std::string vertex_url, std::string fragment_url)
{
  GameApi::SHC a1 = load_shader(vertex_url);
  GameApi::SHC a2 = load_shader(fragment_url);
  ArrayType *array = new ArrayType;
  array->type=2;
  array->vec.push_back(a1.id);
  array->vec.push_back(a2.id);
  return add_array(e, array);
}
class SHIWrapper : public ShaderI2
{
public:
  SHIWrapper(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::SHP params, std::string funcname, ShaderCode *code, std::vector<GameApi::SHI> children) : e(e), ev(ev),params(params), funcname(funcname), code(code), children(children) { }
  virtual void Collect(CollectVisitor2 &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    res = ev.mainloop_api.generic_anim_shader(params, funcname, code->Code(), children);
    ShaderI2 *p = find_shaderI(e,res);
    p->Collect(ii);
    ii.HeavyPrepare();
  }
  void set_inner(int num, std::string value)
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    p->set_inner(num,value);
    }
  }
  virtual std::string get_webgl_header() const
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->get_webgl_header();
    } else return "";
  }
  virtual std::string get_win32_header() const
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->get_win32_header();
    } else return "";
  }
  virtual std::string get_webgl_function() const
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->get_webgl_function();
    } else return "";
  }
  virtual std::string get_win32_function() const
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->get_win32_header();
    } else return "";
  }
  virtual Bindings set_var(const Bindings &b)
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->set_var(b);
    } else return b;
  }
  virtual std::string get_flags() const
  { 
    if (res.id!=-1) {
      ShaderI2 *p = find_shaderI(e,res);
      return p->get_flags();
    } else return "";
  }
  virtual std::string func_name() const
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    return p->func_name();
    } else return "";
  }
  virtual void execute(MainLoopEnv &e2)
  { 
    if (res.id!=-1) {
      ShaderI2 *p = find_shaderI(e,res);
      p->execute(e2);
    } 
  }
  virtual void handle_event(MainLoopEvent &e2)
  {
    if (res.id!=-1) {
    ShaderI2 *p = find_shaderI(e,res);
    p->handle_event(e2);
    }
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::SHP params;
  std::string funcname;
  ShaderCode *code;
  std::vector<GameApi::SHI> children;
  GameApi::SHI res = { -1 };
  VisitorImpl ii;
};
GameApi::SHI GameApi::MainLoopApi::generic_anim_shader2(EveryApi &ev, SHP params, std::string funcname, SHC code, std::vector<SHI> children)
{
  ShaderCode *code2 = find_shader_code(e,code);
  return add_shaderI(e, new SHIWrapper(e,ev,params,funcname,code2,children));
}
GameApi::SHI GameApi::MainLoopApi::generic_anim_shader(SHP params, std::string funcname, std::string shadercode, std::vector<SHI> children)
{
  static int counter =0;
  counter++;
  std::stringstream ss;
  ss << counter;
  std::string uid = ss.str();
  
  shadercode = replace_str(shadercode, "%functionname", funcname);
  shadercode = replace_str(shadercode, "%uid", uid);
  
  ShaderParameterI *param = find_shp(e,params);
  int s = children.size();
  std::vector<ShaderI2*> res_vec;
  for(int i=0;i<s;i++)
    {
      res_vec.push_back(find_shaderI(e,children[i]));
    }  
  return add_shaderI(e,new GenericAnimShaderI(param,funcname,shadercode,res_vec));
}
void GEN_CB(void*);
void GEN_CB2(void*);


class GenericAnimMaterial : public MaterialForward
{
public:
  GenericAnimMaterial(GameApi::Env &env, GameApi::EveryApi &ev, Material *next, GameApi::SHP params, std::string funcname, std::string vertexshadercode_url, std::string fragmentshadercode_url, std::string homepage, std::vector<GameApi::SHI> children) : e(env), ev(ev), next(next), params(params), funcname(funcname), vertexshadercode_url(vertexshadercode_url), fragmentshadercode_url(fragmentshadercode_url), homepage(homepage), children(children) {
    env.async_load_callback(vertexshadercode_url, &GEN_CB, (void*)this);
    env.async_load_callback(fragmentshadercode_url, &GEN_CB2, (void*)this);
  }
  void MaterialCallback()
  {
    if (vertexshader.id==-1)
      shader_from_file();
  }
  void MaterialCallback2()
  {
    if (fragmentshader.id==-1)
      shader_from_file2();
  }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    if (vertexshader.id==-1)
      shader_from_file();
    if (fragmentshader.id==-1)
      shader_from_file2();

    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertexshader, fragmentshader);
    return sh;

    
    
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    if (vertexshader.id==-1)
      shader_from_file();
    if (fragmentshader.id==-1)
      shader_from_file2();

    GameApi::ML ml;
    ml.id = next->mat_inst(p.id,pts.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertexshader, fragmentshader);
    return sh;

  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    if (vertexshader.id==-1)
      shader_from_file();
    if (fragmentshader.id==-1)
      shader_from_file2();

    GameApi::ML ml;
    ml.id = next->mat_inst_matrix(p.id,ms.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertexshader, fragmentshader);
    return sh;
    
    
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    if (vertexshader.id==-1)
      shader_from_file();
    if (fragmentshader.id==-1)
      shader_from_file2();

    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertexshader, fragmentshader);
    return sh;
    
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    if (vertexshader.id==-1)
      shader_from_file();
    if (fragmentshader.id==-1)
      shader_from_file2();

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);

    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertexshader, fragmentshader);
    return sh;

  }

  
  void shader_from_file() const
  {
#ifndef EMSCRIPTEN
    e.async_load_url(vertexshadercode_url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(vertexshadercode_url);
    if (!ptr) std::cout << "ERROR: Shader async load error:" << vertexshadercode_url << std::endl;
    std::string s(ptr->begin(),ptr->end());
    vertexshader = ev.mainloop_api.generic_anim_shader(params, funcname, s, children);
  }
  void shader_from_file2() const
  {
#ifndef EMSCRIPTEN
    e.async_load_url(fragmentshadercode_url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(fragmentshadercode_url);
    if (!ptr) std::cout << "ERROR: Shader async load error:" << fragmentshadercode_url << std::endl;
    std::string s(ptr->begin(),ptr->end());
    fragmentshader = ev.mainloop_api.generic_anim_shader(params, funcname, s, children);
  }
  
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::SHP params;
  std::string funcname;
  std::string vertexshadercode_url;
  std::string fragmentshadercode_url;
  std::string homepage;
  std::vector<GameApi::SHI> children;
  mutable GameApi::SHI vertexshader = { -1 };
  mutable GameApi::SHI fragmentshader = { -1 };
};
void GEN_CB(void *p)
{
  GenericAnimMaterial *pp = (GenericAnimMaterial*)p;
  pp->MaterialCallback();
}
void GEN_CB2(void *p)
{
  GenericAnimMaterial *pp = (GenericAnimMaterial*)p;
  pp->MaterialCallback2();
}


class ScreenSpaceMaterialForward : public ScreenSpaceMaterial
{
public:
  GameApi::ML call(GameApi::TXID screen, GameApi::TXID depth, GameApi::P fullscreenquad, GameApi::TXID pos, GameApi::TXID normal) const
  {
    GameApi::ML ml;
    ml.id = mat(screen.id,depth.id,fullscreenquad.id,pos.id,normal.id);
    return ml;
  }
  int mat(int screen, int depth, int fullscreenquad, int pos, int normal) const
  {
    GameApi::TXID screen2;
    screen2.id = screen;
    GameApi::TXID depth2;
    depth2.id = depth;
    GameApi::TXID pos2;
    pos2.id = pos;
    GameApi::TXID normal2;
    normal2.id=normal;
    GameApi::P p2;
    p2.id = fullscreenquad;
    GameApi::ML ml = mat2(screen2,depth2,p2,pos2,normal2);
    return ml.id;
    
  }
  virtual GameApi::ML mat2(GameApi::TXID screen, GameApi::TXID depth, GameApi::P fullscreenquad, GameApi::TXID pos, GameApi::TXID normal) const=0;
};


class GenericScreenSpaceMaterial_s : public ScreenSpaceMaterialForward
{
public:
  GenericScreenSpaceMaterial_s(GameApi::Env &env, GameApi::EveryApi &ev, ScreenSpaceMaterial *next, GameApi::SHI fragment) : e(env), ev(ev), next(next), fragment(fragment) { }
  void DoPrepares() const
  {
    ShaderI2 *f = find_shaderI(e,fragment);
    f->Collect(ii);
    ii.HeavyPrepare();
  }
  virtual void Prepare() { }
  virtual GameApi::ML mat2(GameApi::TXID screen, GameApi::TXID depth, GameApi::P fullscreenquad, GameApi::TXID position, GameApi::TXID normal) const
  {
    DoPrepares();
    GameApi::ML ml = next->mat(screen.id, depth.id, fullscreenquad.id, position.id, normal.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader_fragment_only(ev,ml,fragment);
    return sh;    
    
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  ScreenSpaceMaterial *next;
  GameApi::SHI fragment;
  mutable VisitorImpl ii;
};

class GenericAnimMaterial_s : public MaterialForward
{
public:
  GenericAnimMaterial_s(GameApi::Env &env, GameApi::EveryApi &ev, Material *next, GameApi::SHI vertex, GameApi::SHI fragment) : e(env), ev(ev), next(next), vertex(vertex), fragment(fragment) {
  }

  void DoPrepares() const
  {
    ShaderI2 *v = find_shaderI(e,vertex);
    ShaderI2 *f = find_shaderI(e,fragment);
    v->Collect(ii);
    f->Collect(ii);
    ii.HeavyPrepare();
  }
  
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    DoPrepares();
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertex, fragment);
    return sh;    
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    DoPrepares();

    GameApi::ML ml;
    ml.id = next->mat_inst(p.id,pts.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertex, fragment);
    return sh;

  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    DoPrepares();

    GameApi::ML ml;
    ml.id = next->mat_inst_matrix(p.id,ms.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertex, fragment);
    return sh;
    
    
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    DoPrepares();

    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertex, fragment);
    return sh;
    
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    DoPrepares();

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);

    GameApi::ML sh = ev.mainloop_api.generic_shader(ev,ml,vertex, fragment);
    return sh;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::SHI vertex;
  GameApi::SHI fragment;
  mutable VisitorImpl ii;
};
GameApi::MT GameApi::MaterialsApi::generic_shader_material00(EveryApi &ev, MT next, SHI vertex, SHI fragment)
{
  Material *mat = find_material(e,next);
  return add_material(e,new GenericAnimMaterial_s(e,ev,mat,vertex,fragment));
}

GameApi::MT GameApi::MaterialsApi::generic_shader_material0(EveryApi &ev, MT next, std::string funcname, std::string vertexshadercode_url, std::string fragmentshadercode_url)
{
  SHP params = ev.mainloop_api.empty_shp();
  return generic_shader_material2(ev,next,params,funcname,vertexshadercode_url, fragmentshadercode_url, std::vector<SHI>{});
}
GameApi::MT GameApi::MaterialsApi::generic_shader_material1(EveryApi &ev, MT next, SHP params, std::string funcname, std::string vertexshadercode_url, std::string fragmentshadercode_url)
{
  return generic_shader_material2(ev,next,params,funcname,vertexshadercode_url, fragmentshadercode_url, std::vector<SHI>{});
}
GameApi::MT GameApi::MaterialsApi::generic_shader_material2(EveryApi &ev, MT next, SHP params, std::string funcname, std::string vertexshadercode_url, std::string fragmentshadercode_url, std::vector<SHI> children)
{
  Material *mat = find_material(e, next);
  return add_material(e,new GenericAnimMaterial(e,ev,mat,params,funcname,vertexshadercode_url,fragmentshadercode_url, gameapi_homepageurl, children));
}
GameApi::SMT GameApi::MaterialsApi::generic_screenspace_material00(EveryApi &ev, SMT next_, SHI fragment)
{
  ScreenSpaceMaterial *next = find_screenspace_material(e,next_);
  return add_screenspace_material(e, new GenericScreenSpaceMaterial_s(e,ev,next,fragment));
}
