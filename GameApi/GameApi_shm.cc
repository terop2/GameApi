
#include "GameApi_h.hh"

std::string ToNum(float val)
{
  std::stringstream ss;
  ss << val;
  std::string s = ss.str();
  bool flag = false;
  int sss = s.size();
  for(int i=0;i<sss;i++) { if (s[i]=='.') { flag=true; } }
  if (flag) 
    return ss.str();
  else
    return ss.str()+".0";
}


std::string vec3_to_string(GameApi::Env &e, GameApi::PT pos)
{
  Point *pt = find_point(e,pos);
  std::string x = ToNum(pt->x);
  std::string y = ToNum(pt->y);
  std::string z = ToNum(pt->z);
  std::string res = "vec3(" + x + ", " + y + ", " + z + ")";
  return res;
}

std::string vec3_to_string(GameApi::Env &e, float x, float y, float z)
{
  std::string xx = ToNum(x);
  std::string yy = ToNum(y);
  std::string zz = ToNum(z);
  std::string res = "vec3(" + xx + ", " + yy + ", " + zz + ")";
  return res;
}

std::string vec4_to_string(GameApi::Env &e, float x, float y, float z, float a)
{
  std::string xx = ToNum(x);
  std::string yy = ToNum(y);
  std::string zz = ToNum(z);
  std::string aa = ToNum(a);
  std::string res = "vec4(" + xx + ", " + yy + ", " + zz + ", " + aa + ")";
  return res;
}

std::string unique_id()
{
  static int val=0;
  val++;
  std::stringstream ss;
  ss << val;
  return ss.str();
}
std::string funccall_to_string(ShaderModule *mod);

std::string color_funccall_to_string(ShaderModule *mod)
{
  std::string res = mod->ColorFunctionName();
  res+="(";
  int s = mod->NumArgs();
  for(int i=0;i<s;i++)
    {
      res += mod->ArgValue(i);
      if (i!=s-1)
	res+=",";
    }
  res+=")";
  return res;
}



std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);

std::string color_funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val)
{
  std::string res = mod->ColorFunctionName();
  res+="(";
  int s = mod->NumArgs();
  for(int i=0;i<s;i++)
    {
      std::string argname = mod->ArgName(i);
      std::string value = mod->ArgValue(i);
      if (argname == name)
	{
	  value = val;
	}
      res += value;
      if (i!=s-1)
	res+=",";
    }
  res+=")";
  return res;
}


class SphereModule : public ShaderModule
{
public:
  SphereModule() { uid = unique_id(); }
  virtual int id() const { return 1; }
  virtual std::string Function() const
  {
    return 
      "float sphere"+uid+"(vec3 center, float radius, vec3 pt)\n"
      "{\n"
      "   pt-=center;\n"
      "   return length(pt)-radius;\n"
      "}\n"
      "vec4 sphere_color" + uid + "(vec3 center, float radius, vec3 pt)\n"
      "{\n"
      "   pt -= center;\n"
      "   pt = normalize(pt);\n"
      "   float u = 0.5 + atan(pt.z, pt.x)/2.0/3.14159;\n"
      "   float v = 0.5 - asin(pt.y)/3.14159;\n"
      "   return vec4(u,v,0.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "sphere"+uid; }
  virtual std::string ColorFunctionName() const { return "sphere_color"+uid; }
  virtual int NumArgs() const { return 3; }
  virtual bool FreeVariable(int i) const
  {
    return true;
  }
  virtual std::string ArgName(int i) const
  {
    switch(i) { 
    case 0: return "center";
    case 1: return "radius";
    case 2: return "pt";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "vec3(0.0,0.0,0.0)";
    case 1: return "40.0";
    case 2: return "pt";
    };
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    switch(i) {
    case 0: return "vec3";
    case 1: return "float";
    case 2: return "vec3";
    };
    return "";
  }
  std::string uid;
};


class CubeModule : public ShaderModule
{
public:
  CubeModule() { uid= unique_id(); }
  virtual int id() const { return 1; }
  virtual std::string Function() const
  {
    return 
      "float cube" + uid + "(vec3 tl, vec3 br, vec3 pt)\n"
      "{\n"
      "   pt-=tl;"
      "   vec3 s = br-tl;\n"
      "   s/=2.0;\n"
      "   pt-=s;\n"
      "   vec3 d = abs(pt) - s;\n"
      "   return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));\n"
      //"   return length(max(abs(pt)-s,0.0));\n"
      "}\n"
      "vec4 cube_color" + uid + "(vec3 tl, vec3 br, vec3 pt)\n"
      "{\n"
      "   return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "cube" + uid; }
  virtual std::string ColorFunctionName() const { return "cube_color" + uid; }
  virtual int NumArgs() const { return 3; }
  virtual bool FreeVariable(int i) const
  {
    return true;
  }
  virtual std::string ArgName(int i) const
  {
    switch(i) { 
    case 0: return "tl";
    case 1: return "br";
    case 2: return "pt";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "vec3(-40.0,-40.0,-40.0)";
    case 1: return "vec3(40.0,40.0,40.0)";
    case 2: return "pt";
    };
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    switch(i) {
    case 0: return "vec3";
    case 1: return "vec3";
    case 2: return "vec3";
    };
    return "";
  }
private:
  std::string uid;
};

class SphericalModule : public ShaderModule
{
public:
  SphericalModule(ShaderModule *mod) :mod(mod) { uid=unique_id(); }
  virtual int id() const { return 1; }
  virtual std::string Function() const
  {
    return mod->Function()+ 
      "float spherical" + uid + "(vec3 pt, vec3 tl, vec3 br, float rr, float rp)\n"
      "{ \n"
      "    float r = pt.y;\n"
      "    float a = pt.x;\n"
      "    float d = pt.z;\n"
      "    r-=tl.y;\n"
      "    a-=tl.x;\n"
      "    d-=tl.z;\n"
      "    vec3 s = br-tl;\n"
      "    r/=s.y;\n"
      "    a/=s.x;\n"
      "    d/=s.z;\n"
      "    a*=3.14159;\n"
      "    d*=3.14159*2.0;\n"
      "    r*=rr;\n"
      "    r+=rp;\n"
      "    float x = r*sin(a)*cos(d);\n"
      "    float y = r*sin(a)*sin(d);\n"
      "    float z = r*cos(a);\n"
      "    vec3 pos = vec3(x,y,z);\n"
      "    float v = " + funccall_to_string_with_replace(mod, "pt", "pos") + ";\n"
      "    return v;\n"
      "}\n"
      "vec4 spherical_color" + uid + "(vec3 pt, vec3 tl, vec3 br, float rr, float rp)\n"
      "{\n"
      "    float r = pt.y;\n"
      "    float a = pt.x;\n"
      "    float d = pt.z;\n"
      "    r-=tl.y;\n"
      "    a-=tl.x;\n"
      "    d-=tl.z;\n"
      "    vec3 s = br-tl;\n"
      "    r/=s.y;\n"
      "    a/=s.x;\n"
      "    d/=s.z;\n"
      "    a*=3.14159;\n"
      "    d*=3.14159*2.0;\n"
      "    r*=rr;\n"
      "    r+=rp;\n"
      "    float x = r*sin(a)*cos(d);\n"
      "    float y = r*sin(a)*sin(d);\n"
      "    float z = r*cos(a);\n"
      "    vec3 pos = vec3(x,y,z);\n"
      "    vec4 v = " + color_funccall_to_string_with_replace(mod, "pt", "pos") + ";\n"
      "    return v;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "spherical"+uid; }
  virtual std::string ColorFunctionName() const { return "spherical_color" +uid; }
  virtual int NumArgs() const { return 5; }
  virtual std::string ArgName(int i) const 
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "tl";
    case 2: return "br";
    case 3: return "rr";
    case 4: return "rp";
    };
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(0.0,0.0,0.0)";
    case 2: return "vec3(100.0,100.0,100.0)";
    case 3: return "100.0";
    case 4: return "100.0";
    };
    return "";
  }
private:
  ShaderModule *mod;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::spherical(SFO obj, PT tl, PT br, float rr, float rp)
{
  Point *tl1 = find_point(e, tl);
  Point *br1 = find_point(e, br);
  ShaderModule *mod = find_shader_module(e, obj);
  SFO s0 = add_shader_module(e, new SphericalModule(mod));
  SFO s1 = bind_arg(s0, "tl", vec3_to_string(e, tl1->x, tl1->y, tl1->z));
  SFO s2 = bind_arg(s1, "br", vec3_to_string(e, br1->x, br1->y, br1->z));
  SFO s3 = bind_arg(s2, "rr", ToNum(rr));
  SFO s4 = bind_arg(s3, "rp", ToNum(rp));
  return s4;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::cube()
{
  return add_shader_module(e, new CubeModule);
}


EXPORT GameApi::SFO GameApi::ShaderModuleApi::sphere()
{
  return add_shader_module(e, new SphereModule);
}
class BindArgModule : public ShaderModule
{
public:
  BindArgModule(ShaderModule &next, std::string name, std::string value) : next(next), name(name), value(value) {}
#if 0
  virtual int id() const { return next.id(); }
#endif
  virtual std::string Function() const
  {
    return next.Function();
  }
  virtual std::string FunctionName() const 
  {
    return next.FunctionName();
  }
  virtual std::string ColorFunctionName() const 
  {
    return next.ColorFunctionName();
  }
  virtual int NumArgs() const { return next.NumArgs(); }
#if 0
  virtual bool FreeVariable(int i) const
  {
    std::string name1 = next.ArgName(i);
    if (name1==name)
      {
	return false;
      }
    return next.FreeVariable(i);
  }
#endif
  virtual std::string ArgName(int i) const
  {
    return next.ArgName(i);
  }
  virtual std::string ArgValue(int i) const
  {
    std::string name1 = next.ArgName(i);
    if (name1==name)
      {
	return value;
      }
    return next.ArgValue(i);
  }
#if 0
  virtual std::string ArgType(int i) const
  {
    return next.ArgType(i);
  }
#endif
private:
  ShaderModule &next;
  std::string name, value;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::bind_arg(SFO obj, std::string name, std::string value)
{
  ShaderModule *mod = find_shader_module(e, obj);
  return add_shader_module(e, new BindArgModule(*mod, name, value));
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::sphere(PT center, float radius)
{
  SFO sfo = sphere();
  SFO sfo_1 = bind_arg(sfo, "center", vec3_to_string(e, center));
  SFO sfo_2 = bind_arg(sfo_1, "radius", ToNum(radius));
  return sfo_2;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::cube(float start_x, float end_x,
					    float start_y, float end_y,
					    float start_z, float end_z)
{
  SFO sfo = cube();
  SFO sfo_1 = bind_arg(sfo, "tl", vec3_to_string(e, start_x, start_y, start_z));
  SFO sfo_2 = bind_arg(sfo_1, "br", vec3_to_string(e, end_x, end_y, end_z));
  return sfo_2;
}
class ColorToGrayScale : public ShaderModule
{
public:
  ColorToGrayScale(ShaderModule *obj) : obj(obj) { uid=unique_id(); }
  virtual int id() const { return 3; }
  virtual std::string Function() const {
    return obj->Function() +
      "float grayscale" + uid + "(vec3 pt, float time)\n"
      "{\n"
      "   return " + funccall_to_string(obj) + ";\n"
      "}\n"
      "vec4 grayscale_color"+ uid + "(vec3 pt, float time)\n"
      "{\n"
      "  vec4 col = " + color_funccall_to_string(obj) + ";\n"
      "  float c = (col.x+col.y+col.z+1.0)/4.0;\n"
      "  return vec4(c,c,c,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "grayscale" + uid; }
  virtual std::string ColorFunctionName() const { return "grayscale_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i) {
    case 0: return "pt";
    case 1: return "time";
    };
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i)
      {
      case 0: return "pt";
      case 1: return "time";
      }
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    switch(i)
      {
      case 0: return "vec3";
      case 1: return "float";
      }
    return "";
  }

private:
  ShaderModule *obj;
  std::string uid;
};

class ColorFromNormalModule : public ShaderModule
{
public:
  ColorFromNormalModule(ShaderModule *obj) : obj(obj) { uid=unique_id(); }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return obj->Function()+
      "vec3 normal" + uid + "(vec3 pt, float time)\n"
      "{\n"
      "  float fx = " + funccall_to_string_with_replace(obj, "pt", "pt+vec3(1.0,0.0,0.0)") + " - " + funccall_to_string_with_replace(obj, "pt", "pt-vec3(1.0,0.0,0.0)") + ";\n"
      "  float fy = " + funccall_to_string_with_replace(obj, "pt", "pt+vec3(0.0,1.0,0.0)") + " - " + funccall_to_string_with_replace(obj, "pt", "pt-vec3(0.0,1.0,0.0)") + ";\n"
      "  float fz = " + funccall_to_string_with_replace(obj, "pt", "pt+vec3(0.0,0.0,1.0)") + " - " + funccall_to_string_with_replace(obj, "pt", "pt-vec3(0.0,0.0,1.0)") + ";\n"
      "  return normalize(vec3(-fx,-fy,-fz))/2.0+vec3(0.5,0.5,0.5);\n"
      "}\n"
      "float obj" + uid + "(vec3 pt, float time)\n"
      "{\n"
      "   return " + funccall_to_string(obj) + ";\n"
      "}\n"
      "vec4 obj_color" + uid + "(vec3 pt, float time)\n"
      "{\n"
      "   return vec4(normal" + uid + "(pt,time),1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "obj" + uid; }
  virtual std::string ColorFunctionName() const { return "obj_color"+uid;  }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i) {
    case 0: return "pt";
    case 1: return "time";
    };
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i)
      {
      case 0: return "pt";
      case 1: return "time";
      }
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    switch(i)
      {
      case 0: return "vec3";
      case 1: return "float";
      }
    return "";
  }


private:
  ShaderModule *obj;
  std::string uid;
};
class RenderModule : public ShaderModule
{
public:
  RenderModule(ShaderModule *obj) : obj(obj) { }
  virtual int id() const { return 2; }
  virtual std::string Function() const
  {
    return obj->Function() +
      "vec3 ray(vec3 p1, vec3 p2, float t)\n"
      "{\n"
      "   return p1 + t*(p2-p1);\n"
      "}\n"
      "float solve(vec3 p1, vec3 p2, float t_0, float t_1, float time)\n"
      "{\n"
      "    float t = t_0;\n"
      "    for(int i=0;i<600;i++)\n"
      "    {\n"
      "       vec3 pt = ray(p1,p2,t);\n"
      "       float Ht = " + funccall_to_string(obj) + ";\n"
      "       if (abs(Ht)<0.5) return t;\n"
      "       if (t>t_1) return 0.0;\n"
      "       t+= Ht / 5.0;\n"
      "    }\n"
      "    return 0.0;\n"
      "}\n"
      "vec4 render(vec3 p0, vec3 p1)\n"
      "{\n"
      "   float t = solve(p0,p1,0.0, 1600.0, time);\n"
      "   vec3 pt = ray(p0,p1, t);\n"
      "   vec4 rgb = " + color_funccall_to_string(obj) + ";\n"
      "   if (abs(t)<0.001) { rgb=vec4(0.5,0.5,0.5,1.0); }\n"
      "   return rgb;\n"
      "}\n"
      ;
  }
  virtual std::string FunctionName() const { return "render"; }
  virtual std::string ColorFunctionName() const { return "render"; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i) {
    case 0: return "p0";
    case 1: return "p1";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "p0";
    case 1: return "p1";
    }
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    return "vec3";
  }
private:
  ShaderModule *obj;
};

class V_RenderModule : public ShaderModule
{
public:
  V_RenderModule(ShaderModule *obj) : obj(obj) { }
  virtual int id() const { return 2; }
  virtual std::string Function() const
  {
    return obj->Function() +
      "vec3 ray(vec3 p1, vec3 p2, float t)\n"
      "{\n"
      "   return p1 + t*(p2-p1);\n"
      "}\n"
      "float solve(vec3 p1, vec3 p2, float t_0, float t_1)\n"
      "{\n"
      "    float t = t_0;\n"
      "    for(int i=0;i<600;i++)\n"
      "    {\n"
      "       vec3 pt = ray(p1,p2,t);\n"
      "       float Ht = " + funccall_to_string(obj) + ";\n"
      "       if (abs(Ht)<0.005) return t;\n"
      "       if (t>t_1) return 0.0;\n"
      "       t+= abs(Ht) / 500.0;\n"
      "    }\n"
      "    return 0.0;\n"
      "}\n"
      "vec4 v_render(vec4 p0, vec4 p1)\n"
      "{\n"
      "   vec3 p0a = p0.xyz;\n"
      "   vec3 p1a = p1.xyz;\n"
      "   float t = solve(p0a,p1a,0.5, 10000.0);\n"
      //"   if (t<0.01) t=1.0;\n"
      //"   if (t>1000.0) t=1.0;\n"
      "   vec3 pt = ray(p0a,p1a, t);\n"
      "   vec4 rgb = " + color_funccall_to_string(obj) + ";\n"
      //"   if (abs(t)<0.001) { rgb=vec4(0.5,0.5,0.5,1.0); }\n"
      "   ex_Color = rgb;\n"
      "   return vec4(pt,p1.w);\n"
      //"   return p1;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "v_render"; }
  virtual std::string ColorFunctionName() const { return "v_render_color"; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i) {
    case 0: return "p0";
    case 1: return "p1";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "vec4(0.0,0.0,0.0,1.0)";
    case 1: return "p1";
    }
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    return "vec4";
  }
private:
  ShaderModule *obj;
};


EXPORT GameApi::SFO GameApi::ShaderModuleApi::grayscale(SFO obj)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  return add_shader_module(e, new ColorToGrayScale(obj_m));
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::color_from_normal(SFO obj)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  return add_shader_module(e, new ColorFromNormalModule(obj_m));
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::render(SFO obj)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  return add_shader_module(e, new RenderModule(obj_m));
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::v_render(SFO obj)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  return add_shader_module(e, new V_RenderModule(obj_m));
}
class RotYModule : public ShaderModule
{
public:
  RotYModule(ShaderModule &m) : m(m) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m.Function() +
      "float roty" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3(vec3(cos(angle),0.0,sin(angle)),\n"
      "          vec3(0.0, 1.0, 0.0),\n"
      "          vec3(-sin(angle),0.0,cos(angle)));\n"
      "   float v1 = " + funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n"
      "vec4 roty_color" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3(vec3(cos(angle),0.0,sin(angle)),\n"
      "          vec3(0.0, 1.0, 0.0),\n"
      "          vec3(-sin(angle),0.0,cos(angle)));\n"
      "   vec4 v1 = " + color_funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("roty") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("roty_color") + uid; }
  virtual int NumArgs() const { return 3; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i)
      {
      case 0: return "pt";
      case 1: return "center";
      case 2: return "angle";
      }
    return "";
  }
  virtual std::string ArgValue(int i) const { 
    switch(i)
      {
      case 0: return "pt"; 
      case 1: return "vec3(0.0,0.0,0.0)";
      case 2: return "0.0";
      };
    return "";
  }
  virtual std::string ArgType(int i) const {
    switch(i)
      {
      case 0: return "vec3";
      case 1: return "vec3";
      case 2: return "float";
      }
    return "";
  }

private:
  ShaderModule &m;
  std::string uid;
};

class RotXModule : public ShaderModule
{
public:
  RotXModule(ShaderModule &m) : m(m) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m.Function() +
      "float rotx" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3("
      "          vec3(1.0, 0.0, 0.0),\n"
      "          vec3(0.0,cos(angle),sin(angle)),\n"
      "          vec3(0.0,-sin(angle),cos(angle)));\n"
      "   float v1 = " + funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n"
      "vec4 rotx_color" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3("
      "          vec3(1.0, 0.0, 0.0),\n"
      "          vec3(0.0,cos(angle),sin(angle)),\n"
      "          vec3(0.0,-sin(angle),cos(angle)));\n"
      "   vec4 v1 = " + color_funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("rotx") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("rotx_color") + uid; }
  virtual int NumArgs() const { return 3; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i)
      {
      case 0: return "pt";
      case 1: return "center";
      case 2: return "angle";
      }
    return "";
  }
  virtual std::string ArgValue(int i) const { 
    switch(i)
      {
      case 0: return "pt"; 
      case 1: return "vec3(0.0,0.0,0.0)";
      case 2: return "0.0";
      };
    return "";
  }
  virtual std::string ArgType(int i) const {
    switch(i)
      {
      case 0: return "vec3";
      case 1: return "vec3";
      case 2: return "float";
      }
    return "";
  }

private:
  ShaderModule &m;
  std::string uid;
};

class RotZModule : public ShaderModule
{
public:
  RotZModule(ShaderModule &m) : m(m) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m.Function() +
      "float rotz" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3("
      "          vec3(cos(angle),sin(angle),0.0),\n"
      "          vec3(-sin(angle),cos(angle),0.0),\n"
      "          vec3(0.0, 0.0, 1.0));\n"
      "   float v1 = " + funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n"
      "vec4 rotz_color" + uid + "(vec3 pt, vec3 center, float angle)\n"
      "{\n"
      "  mat3 m = mat3("
      "          vec3(cos(angle),sin(angle),0.0),\n"
      "          vec3(-sin(angle),cos(angle),0.0),\n"
      "          vec3(0.0, 0.0, 1.0));\n"
      "   vec4 v1 = " + color_funccall_to_string_with_replace(&m, "pt", "center+m*(pt-center)") + ";\n"
      "   return v1;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("rotz") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("rotz_color") + uid; }
  virtual int NumArgs() const { return 3; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { 
    switch(i)
      {
      case 0: return "pt";
      case 1: return "center";
      case 2: return "angle";
      }
    return "";
  }
  virtual std::string ArgValue(int i) const { 
    switch(i)
      {
      case 0: return "pt"; 
      case 1: return "vec3(0.0,0.0,0.0)";
      case 2: return "0.0";
      };
    return "";
  }
  virtual std::string ArgType(int i) const {
    switch(i)
      {
      case 0: return "vec3";
      case 1: return "vec3";
      case 2: return "float";
      }
    return "";
  }

private:
  ShaderModule &m;
  std::string uid;
};


class AndNotModule : public ShaderModule
{
public:
  AndNotModule(ShaderModule &m1, ShaderModule &m2) : m1(m1), m2(m2) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m1.Function() + m2.Function() +
      "float and_not" + uid + "(vec3 pt)\n"
      "{\n"
      "   float v1 = " + funccall_to_string(&m1) + ";\n"
      "   float v2 = " + funccall_to_string(&m2) + ";\n"
      "   float res = max(v1,-v2);\n"
      "   return res;\n"
      "}\n"
      "vec4 and_not_color" + uid + "(vec3 pt)\n"
      "{\n"
      "   float v = " + funccall_to_string(&m1) + ";\n"
      "   if (v>-0.8 && v<0.8) return " + color_funccall_to_string(&m1) + ";\n"
      "   return " + color_funccall_to_string(&m2) + ";\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("and_not") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("and_not_color") + uid; }
  virtual int NumArgs() const { return 1; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { return "pt"; }
  virtual std::string ArgValue(int i) const { return "pt"; }
  virtual std::string ArgType(int i) const { return "vec3"; }
private:
  ShaderModule &m1;
  ShaderModule &m2;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::and_not(SFO o1, SFO o2)
{
  ShaderModule *o1_m = find_shader_module(e, o1);
  ShaderModule *o2_m = find_shader_module(e, o2);
  return add_shader_module(e, new AndNotModule(*o1_m, *o2_m));
}

class OrElemModule : public ShaderModule
{
public:
  OrElemModule(ShaderModule &m1, ShaderModule &m2) : m1(m1), m2(m2) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m1.Function() + m2.Function() +
      "float or_elem" + uid + "(vec3 pt)\n"
      "{\n"
      "   float v1 = " + funccall_to_string(&m1) + ";\n"
      "   float v2 = " + funccall_to_string(&m2) + ";\n"
      "   float res = min(v1,v2);\n"
      "   return res;\n"
      "}\n"
      "vec4 or_elem_color" + uid + "(vec3 pt)\n"
      "{\n"
      "   float val = " + funccall_to_string(&m1) + ";\n"
      "   if (val>-0.8 && val<0.8) return " + color_funccall_to_string(&m1) + ";\n"
      "   return " + color_funccall_to_string(&m2) + ";\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("or_elem") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("or_elem_color") + uid; }
  virtual int NumArgs() const { return 1; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { return "pt"; }
  virtual std::string ArgValue(int i) const { return "pt"; }
  virtual std::string ArgType(int i) const { return "vec3"; }
private:
  ShaderModule &m1;
  ShaderModule &m2;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::or_elem(SFO o1, SFO o2)
{
  ShaderModule *o1_m = find_shader_module(e, o1);
  ShaderModule *o2_m = find_shader_module(e, o2);
  return add_shader_module(e, new OrElemModule(*o1_m, *o2_m));
}


class BlendElemModule : public ShaderModule
{
public:
  BlendElemModule(ShaderModule &m1, ShaderModule &m2) : m1(m1), m2(m2) { 
    uid = unique_id();
  }
  virtual int id() const { return 3; }
  virtual std::string Function() const
  {
    return m1.Function() + m2.Function() +
      "float smin" + uid + "(float a, float b, float k)\n"
      "{\n"
      "  float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );\n"
      "  return mix(b,a,h) - k*h*(1.0-h);\n"
      "}"
      "float blend" + uid + "(vec3 pt, float k)\n"
      "{\n"
      "   float v1 = " + funccall_to_string(&m1) + ";\n"
      "   float v2 = " + funccall_to_string(&m2) + ";\n"
      "   float res = smin" + uid + "(v1,v2,k);\n"
      "   return res;\n"
      "}\n"
      "vec4 blend_color" + uid + "(vec3 pt, float k)\n"
      "{\n"
      "   float val = " + funccall_to_string(&m1) + ";\n"
      "   float val2 = " + funccall_to_string(&m2) + ";\n"
      "   vec4 v1 = " + color_funccall_to_string(&m1) + ";\n"
      "   vec4 v2 = " + color_funccall_to_string(&m2) + ";\n"
      "   if (val<val2) return v1;\n"
      "   return v2;\n"
      //"   return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return std::string("blend") + uid; }
  virtual std::string ColorFunctionName() const { return std::string("blend_color") + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { if (i==0) return "pt"; return "k"; }
  virtual std::string ArgValue(int i) const { if (i==0) return "pt"; return "0.5"; }
  virtual std::string ArgType(int i) const { if (i==0) return "vec3"; return "float"; }
private:
  ShaderModule &m1;
  ShaderModule &m2;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::blend(SFO o1, SFO o2, float k)
{
  ShaderModule *o1_m = find_shader_module(e, o1);
  ShaderModule *o2_m = find_shader_module(e, o2);
  SFO s =  add_shader_module(e, new BlendElemModule(*o1_m, *o2_m));
  SFO s2 = bind_arg(s, "k", ToNum(k));
  return s2;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_y(SFO obj, float angle)
{
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotYModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  return s2;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_x(SFO obj, float angle)
{
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotXModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  return s2;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_z(SFO obj, float angle)
{
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotZModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  return s2;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_y(SFO obj, float angle, PT center)
{
  Point *c = find_point(e, center);
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotYModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  SFO s3 = bind_arg(s2, "center", vec3_to_string(e, c->x,c->y,c->z));
  return s3;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_x(SFO obj, float angle, PT center)
{
  Point *c = find_point(e, center);
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotXModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  SFO s3 = bind_arg(s2, "center", vec3_to_string(e, c->x,c->y,c->z));
  return s3;
}

EXPORT GameApi::SFO GameApi::ShaderModuleApi::rot_z(SFO obj, float angle, PT center)
{
  Point *c = find_point(e, center);
  ShaderModule *o1_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new RotZModule(*o1_m));
  SFO s2 = bind_arg(s, "angle", ToNum(angle));
  SFO s3 = bind_arg(s2, "center", vec3_to_string(e, c->x,c->y,c->z));
  return s3;
}


class LineModule : public ShaderModule
{
public:
  LineModule() { uid=unique_id(); }
  virtual int id() const { return 5;}
  virtual std::string Function() const
  {
    return 
      "float line" + uid + "(vec3 pt, vec3 tl, vec3 br, float line_width1, float line_width2)\n"
      "{\n"
      "   vec3 pa = pt-tl, ba = br-tl;\n"
      "   float h = dot(pa,ba)/dot(ba,ba) ;\n"
      "   if (h<0.0 || h>1.0) return length(pa-ba*h)+0.8;\n"
      "   return length( pa - ba*h ) - ((1.0-h)*line_width1+h*line_width2);\n"
      "}\n"
      "vec4 line_color" + uid + "(vec3 pt, vec3 tl, vec3 br, float line_width1, float line_width2)\n"
      "{\n"
      "   return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "line" + uid; }
  virtual std::string ColorFunctionName() const { return "line_color" + uid; }
  virtual int NumArgs() const { return 5; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "tl";
    case 2: return "br";
    case 3: return "line_width1";
    case 4: return "line_width2";
    };
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(0.0,0.0,0.0)";
    case 2: return "vec3(100.0,100.0,0.0)";
    case 3: return "10.0";
    case 4: return "10.0";
    };
    return "";
  }
  virtual std::string ArgType(int i) const
  {
    switch(i) {
    case 0: return "vec3";
    case 1: return "vec3";
    case 2: return "vec3";
    case 3: return "float";
    case 4: return "float";
    };
    return "";
  }


private:
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::line()
{
  return add_shader_module(e, new LineModule());
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::line(float start_x, float start_y, float start_z,
					    float end_x, float end_y, float end_z,
					    float line_width1, float line_width2)
{
  SFO ln = line();
  SFO ln_1 = bind_arg(ln, "tl", vec3_to_string(e, start_x, start_y, start_z));
  SFO ln_2 = bind_arg(ln_1, "br", vec3_to_string(e, end_x, end_y, end_z));
  SFO ln_3 = bind_arg(ln_2, "line_width1", ToNum(line_width1));
  SFO ln_4 = bind_arg(ln_3, "line_width2", ToNum(line_width2));
  return ln_4;
}

class TransModule : public ShaderModule
{
public:
  TransModule(ShaderModule *obj) : obj(obj) { uid=unique_id(); }
  virtual int id() const { return 6; }
  virtual std::string Function() const
  {
    return obj->Function() +
      "float trans" + uid + "(vec3 pt, vec3 delta)\n"
      "{\n"
      "   pt-=delta;\n"
      "   return " + funccall_to_string(obj) + ";\n"
      "}\n"
      "vec4 trans_color" + uid + "(vec3 pt, vec3 delta)\n"
      "{\n"
      "    pt-=delta;\n"
      "    return " + color_funccall_to_string(obj) + ";\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "trans" + uid; }
  virtual std::string ColorFunctionName() const { return "trans_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const
  {
    if (i==0) return "pt";
    return "delta";
  }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "vec3(0.0,0.0,0.0)";
  }
  virtual std::string ArgType(int i) const
  {
    return "vec3";
  }

private:
  ShaderModule *obj;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::trans(SFO obj_m)
{
  ShaderModule *obj = find_shader_module(e, obj_m);
  return add_shader_module(e, new TransModule(obj));
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::trans(SFO obj_m, float dx, float dy, float dz)
{
  SFO sfo = trans(obj_m);
  SFO sfo_1 = bind_arg(sfo, "delta", vec3_to_string(e, dx,dy,dz));
  return sfo_1;
}

class ScaleModule : public ShaderModule
{
public:
  ScaleModule(ShaderModule *obj) : obj(obj) { uid=unique_id(); }
  virtual int id() const { return 6; }
  virtual std::string Function() const
  {
    return obj->Function() +
      "float scale" + uid + "(vec3 pt, vec3 delta)\n"
      "{\n"
      "   pt*=delta;\n"
      "   return " + funccall_to_string(obj) + ";\n"
      "}\n"
      "vec4 scale_color" + uid + "(vec3 pt, vec3 delta)\n"
      "{\n"
      "    pt*=delta;\n"
      "    return " + color_funccall_to_string(obj) + ";\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "scale" + uid; }
  virtual std::string ColorFunctionName() const { return "scale_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const
  {
    if (i==0) return "pt";
    return "delta";
  }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "vec3(0.0,0.0,0.0)";
  }
  virtual std::string ArgType(int i) const
  {
    return "vec3";
  }

private:
  ShaderModule *obj;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::scale(SFO obj_m)
{
  ShaderModule *obj = find_shader_module(e, obj_m);
  return add_shader_module(e, new ScaleModule(obj));
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::scale(SFO obj_m, float dx, float dy, float dz)
{
  SFO sfo = scale(obj_m);
  SFO sfo_1 = bind_arg(sfo, "delta", vec3_to_string(e, dx,dy,dz));
  return sfo_1;
}

class ColorChooseModule : public ShaderModule
{
public:
  ColorChooseModule(ShaderModule *mod) : mod(mod) { uid = unique_id(); }
  virtual int id() const { return 9; }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float color_choose" + uid +  "(vec3 pt, vec4 color) {\n"
      "   return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 color_choose_color" + uid + "(vec3 pt, vec4 color) {\n"
      "   return color;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "color_choose" + uid; }
  virtual std::string ColorFunctionName() const { return "color_choose_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const
  {
    if (i==0) return "pt";
    return "color";
  }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "vec3(1.0,1.0,1.0,1.0);";
  }
  virtual std::string ArgType(int i) const
  {
    if (i==0) return "vec3";
    return "vec4";
  }
private:
  ShaderModule *mod;
  std::string uid;
};
class MixColorModule : public ShaderModule
{
public:
  MixColorModule(ShaderModule *mod1, ShaderModule *mod2) : mod1(mod1), mod2(mod2) { uid=unique_id(); }
  virtual int id() const { return 12; }
  virtual std::string Function() const
  {
    return mod1->Function() + mod2->Function() +
      "float color_mix" + uid +  "(vec3 pt, float t, float time) {\n"
      "   return " + funccall_to_string(mod1) + ";\n"
      "}\n"
      "vec4 color_mix_color" + uid + "(vec3 pt, float t, float time) {\n"
      "   vec4 col1 = " + color_funccall_to_string(mod1) + ";\n"
      "   vec4 col2 = " + color_funccall_to_string(mod2) + ";\n"
      "   return mix(col1, col2, t);\n"
      "}\n";

  }
  virtual std::string FunctionName() const { return "color_mix" + uid; }
  virtual std::string ColorFunctionName() const { return "color_mix_color" + uid; }
  virtual int NumArgs() const {return 3; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const {
    if (i==0) return "pt"; 
    if (i==1) return "t";
    return "time"; 
  }
  virtual std::string ArgValue(int i) const { 
    if (i==0) return "pt"; 
    if (i==1) return "0.5";
    return "time"; 
  }
  virtual std::string ArgType(int i) const { 
    if (i==0) return "vec3"; 
    if (i==1) return "float";
    return "float"; 
  }
private:
  ShaderModule *mod1;
  ShaderModule *mod2;
  std::string uid;
};

class ColorFromObjModule : public ShaderModule
{
public:
  ColorFromObjModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual int id() const { return 9; }
  virtual std::string Function() const
  {
    return 
      "float from_obj_color" + uid + "(vec3 pt, float time) {\n"
      "    return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 from_obj_color_color" + uid + "(vec3 pt, float time) {\n"
      "   return " + color_funccall_to_string(mod) + ";\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "from_obj_color" + uid; }
  virtual std::string ColorFunctionName() const { return "from_obj_color_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { if (i==0) return "pt"; return "time"; }
  virtual std::string ArgValue(int i) const { if (i==0) return "pt"; return "time"; }
  virtual std::string ArgType(int i) const { if (i==0) return "vec3"; return "float"; }
private:
  ShaderModule *mod;
  std::string uid;
};

class FromPointsModule : public ShaderModule
{
public:
  FromPointsModule(GameApi::Env &e, PointsApiPoints *pts, ShaderModule *m) : e(e), pts(pts), m(m) { uid = unique_id(); }
  virtual int id() const { return 8; }
  virtual std::string Function() const
  {
    std::string s = m->Function();
    s+="float pts" + uid + "(vec3 pt)\n";
    s+="{\n";
    s+="    float val2=99999.0;\n";
    s+="    float val;\n";
    int ss = pts->NumPoints();
    for(int i=0;i<ss;i++)
      {
	Point p = pts->Pos(i);
	float dx = p.x;
	float dy = p.y;
	float dz = p.z;
	s+="    val = " + funccall_to_string_with_replace(m, "pt", "pt-" + vec3_to_string(e, dx,dy,dz)) + ";\n";
	s+="    val2 = min(val2,val);\n";
      }
    s+="    return val2;\n";
    s+="}\n";
    s+="vec4 pts_color" + uid + "(vec3 pt) {\n";
    s+="   return vec4(1.0,1.0,1.0,1.0);\n";
    s+="}\n";
    return s;
  }
  virtual std::string FunctionName() const { return "pts" + uid; }
  virtual std::string ColorFunctionName() const { return "pts_color" + uid; }
  virtual int NumArgs() const { return 1; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const { return "pt"; }
  virtual std::string ArgValue(int i) const { return "pt"; }
  virtual std::string ArgType(int i) const { return "vec3"; }

private:
  GameApi::Env &e;
  PointsApiPoints *pts;
  ShaderModule *m;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::stop_generation(SFO obj)
{
  ShaderModule *obj2_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new ColorFromObjModule(obj2_m));
  return s;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::color(SFO obj, float r, float g, float b, float a)
{
  ShaderModule *obj2_m = find_shader_module(e, obj);
  SFO s = add_shader_module(e, new ColorChooseModule(obj2_m));
  SFO s2 = bind_arg(s, "color", vec4_to_string(e, r,g,b,a));
  return s2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::mix_color(SFO mod1, SFO mod2, float t)
{
  ShaderModule *obj1_m = find_shader_module(e, mod1);
  ShaderModule *obj2_m = find_shader_module(e, mod2);
  SFO s = add_shader_module(e, new MixColorModule(obj1_m, obj2_m));
  SFO s2 = bind_arg(s, "t", ToNum(t));
  return s2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::from_points(PTS p, SFO obj)
{
  PointsApiPoints *pts = find_pointsapi_points(e, p);
  ShaderModule *obj2_m = find_shader_module(e, obj);
  return add_shader_module(e, new FromPointsModule(e,pts, obj2_m));
}
class PlaneModule : public ShaderModule
{
public:
  PlaneModule() { uid = unique_id(); }
  virtual int id() const { return 11; }
  virtual std::string Function() const
  {
    return 
      "float plane" + uid + "(vec3 pt, vec3 center, vec3 u_x, vec3 u_y, float sx, float sy)\n"
      "{\n"
      "   pt-=center;\n"
      "   vec3 normal = normalize(cross(u_x, u_y));\n"
      "   float val = dot(pt, normal);\n"
      "   return val;\n"
      "}\n"
      "vec4 plane_color" + uid + "(vec3 pt, vec3 center, vec3 u_x, vec3 u_y, float sx, float sy)\n"
      "{\n"
      "   float v_x = dot(pt-center, u_x)/length(u_x)/length(u_x);\n"
      "   float v_y = dot(pt-center, u_y)/length(u_y)/length(u_y);\n"
      "   v_x/=sx;\n"
      "   v_y/=sy;\n"
      "   v_x = abs(fract(v_x));\n"
      "   v_y = abs(fract(v_y));\n"
      "   return vec4(v_x,v_y,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "plane" + uid; }
  virtual std::string ColorFunctionName() const { return "plane_color" + uid; }
  virtual int NumArgs() const {return 6; }
  virtual bool FreeVariable(int i) const { return true; }
  virtual std::string ArgName(int i) const {
    switch(i) {
    case 0: return "pt";
    case 1: return "center";
    case 2: return "u_x";
    case 3: return "u_y";
    case 4: return "sx";
    case 5: return "sy";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(0.0,40.0,0.0)";
    case 2: return "vec3(1.0,0.0,0.0)";
    case 3: return "vec3(0.0,0.0,1.0)";
    case 4: return "100.0";
    case 5: return "100.0";
    }
    return "";    
  }
  virtual std::string ArgType(int i) const
  {
    return "vec3";
  }
private:
  std::string uid;
};
class TorusModule : public ShaderModule
{
public:
  TorusModule() { uid = unique_id(); }
  virtual std::string Function() const
  {
    return 
      "float torus" + uid + "(vec3 pt, float radius_1, float radius_2)\n"
      "{\n"
      "   return length(vec2(length(pt.xz)-radius_1, pt.y)) - radius_2;\n"
      "}\n"
      "vec4 torus_color" + uid + "(vec3 pt, float radius_1, float radius_2)\n"
      "{\n"
      "   return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "torus" + uid; }
  virtual std::string ColorFunctionName() const { return "torus_color" + uid; }
  virtual int NumArgs() const { return 3; }
  virtual std::string ArgName(int i) const
  {
    switch(i)
      {
      case 0: return "pt";
      case 1: return "radius_1";
      case 2: return "radius_2";
      }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "100.0";
    case 2: return "20.0";
    }
    return "";
  }
    

private:
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::torus(float radius_1, float radius_2)
{
  SFO torus = add_shader_module(e, new TorusModule());
  SFO torus_1 = bind_arg(torus, "radius_1", ToNum(radius_1));
  SFO torus_2 = bind_arg(torus_1, "radius_2", ToNum(radius_2));
  return torus_2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::plane(PT center, V u_x, V u_y)
{
  Point *center_1 = find_point(e, center);
  Vector *normal_1 = find_vector(e, u_x);
  Vector *normal_2 = find_vector(e, u_y);
  SFO pl = add_shader_module(e, new PlaneModule());
  SFO pl_1 = bind_arg(pl, "center", vec3_to_string(e, center_1->x, center_1->y, center_1->z));
  SFO pl_2 = bind_arg(pl_1, "u_x", vec3_to_string(e, normal_1->dx, normal_1->dy, normal_1->dz));
  SFO pl_3 = bind_arg(pl_2, "u_y", vec3_to_string(e, normal_2->dx, normal_2->dy, normal_2->dz));
  return pl_3;
}
class SoftShadowModule : public ShaderModule
{
public:
  SoftShadowModule(ShaderModule *scene) :scene(scene) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return scene->Function() +
      "float softshadow" + uid + "(vec3 pt, vec3 rd, float mint, float maxt, float k)\n"
      "{\n"
      "   float res = 1.0;\n"
      "   float t = mint;\n"
      "   for(int i=0;i<256;i++)\n"
      "   {\n"
      "      float h = " + funccall_to_string_with_replace(scene, "pt", "pt + rd*t") + ";\n"
      "      if (h<0.001) return 0.0;\n"
      "      res = min(res, k*h/t);\n"
      "      t += h;\n"
      "      if (t>maxt) break;\n"
      "    }\n"
      "    return res;\n"
      "}\n"
      "float softshadow_obj" + uid + "(vec3 pt, vec3 rd, float mint, float maxt, float k, float strong) {\n"
      "   return " + funccall_to_string(scene) + ";\n"
      "}\n"
      "vec4 softshadow_color" + uid + "(vec3 pt, vec3 rd, float mint, float maxt, float k, float strong) {\n"
      "   vec4 color = " + color_funccall_to_string(scene) + ";\n"
      "   float shadow = softshadow" + uid + "(pt,rd,mint,maxt,k);\n"
      "   return vec4((shadow/strong+(1.0-1.0/strong))*color.rgb,color.a);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "softshadow_obj" + uid; }
  virtual std::string ColorFunctionName() const { return "softshadow_color" + uid; }
  virtual int NumArgs() const { return 6; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "light_dir";
    case 2: return "mint";
    case 3: return "maxt";
    case 4: return "k";
    case 5: return "strong";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(30.0,60.0,30.0)";
    case 2: return "0.2";
    case 3: return "1.0";
    case 4: return "0.3";
    case 5: return "4.0";
    }
    return "";
  }

private:
  ShaderModule *scene;
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::soft_shadow(SFO scene, V light_dir, float mint, float maxt, float k, float strong)
{
  Vector *vec = find_vector(e,light_dir);
  ShaderModule *mod = find_shader_module(e, scene);
  SFO shadow = add_shader_module(e, new SoftShadowModule(mod));
  SFO shadow_1 = bind_arg(shadow, "light_dir", vec3_to_string(e,vec->dx, vec->dy, vec->dz));
  SFO shadow_2 = bind_arg(shadow_1, "mint", ToNum(mint));
  SFO shadow_3 = bind_arg(shadow_2, "maxt", ToNum(maxt));
  SFO shadow_4 = bind_arg(shadow_3, "k", ToNum(k));
  SFO shadow_5 = bind_arg(shadow_4, "strong", ToNum(strong));
  return shadow_5;
}
class RoundedCube : public ShaderModule
{
public:
  RoundedCube() { uid=unique_id(); }
  virtual std::string Function() const
  {
    return 
      "float rbox" + uid + "(vec3 pt, vec3 tl, vec3 br, float r)\n"
      "{\n"
      "   pt-=tl;\n"
      "   vec3 s = br-tl;\n"
      "   s/=2.0;\n"
      "   pt-=s;\n"
      "   return length(max(abs(pt)-s+vec3(r),0.0))-r;\n"
      "}\n"
      "vec4 rbox_color" + uid + "(vec3 pt, vec3 tl, vec3 br, float r)\n"
      "{\n"
      "   return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "rbox" + uid; }
  virtual std::string ColorFunctionName() const { return "rbox_color" + uid; }
  virtual int NumArgs() const { return 4; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "tl";
    case 2: return "br";
    case 3: return "r";
    };
    return "";
  }

  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(0.0,0.0,0.0)";
    case 2: return "vec3(100.0,100.0,100.0)";
    case 3: return "20.0";
    };
    return "";
  }
private:
  std::string uid;
};
class AOModule : public ShaderModule
{
public:
  AOModule(ShaderModule *mod) : mod(mod) { uid = unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float ao"+uid+"(vec3 pt, vec3 n, float d, float i)\n"
      "{\n"
      "    float o = 0.0;\n"
      "    for(float i=1.;i>0.;i--) {\n"
      "      o-=(i*d-abs(" + funccall_to_string_with_replace(mod, "pt", "pt+n*i*d") + "))/pow(2.,i);\n"
      "    }\n"
      "    return o;\n"
      "}\n"
      "vec3 normal" + uid + "(vec3 pt, float time)\n"
      "{\n"
      "  float fx = " + funccall_to_string_with_replace(mod, "pt", "pt+vec3(1.0,0.0,0.0)") + " - " + funccall_to_string_with_replace(mod, "pt", "pt-vec3(1.0,0.0,0.0)") + ";\n"
      "  float fy = " + funccall_to_string_with_replace(mod, "pt", "pt+vec3(0.0,1.0,0.0)") + " - " + funccall_to_string_with_replace(mod, "pt", "pt-vec3(0.0,1.0,0.0)") + ";\n"
      "  float fz = " + funccall_to_string_with_replace(mod, "pt", "pt+vec3(0.0,0.0,1.0)") + " - " + funccall_to_string_with_replace(mod, "pt", "pt-vec3(0.0,0.0,1.0)") + ";\n"
      "  return normalize(vec3(-fx,-fy,-fz))/2.0+vec3(0.5,0.5,0.5);\n"
      "}\n"
      "float ao_obj" + uid + "(vec3 pt, float d, float i)\n"
      "{\n"
      "   return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 ao_color" + uid + "(vec3 pt, float d, float i)\n"
      "{\n"
      "   vec3 n = normal" + uid + "(pt,time);\n"
      "   float ao = ao" + uid + "(pt,n,d,i);\n"
      "   ao = clamp(ao,0.0,1.0);\n"
      "   vec4 c = " + color_funccall_to_string(mod) + ";\n"
      "   vec3 c2 = mix(c.rgb,vec3(1.0,1.0,1.0),ao);\n"
      "   return vec4(c2,c.a);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "ao_obj" + uid; }
  virtual std::string ColorFunctionName() const { return "ao_color" + uid; }
  virtual int NumArgs() const { return 3; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "d";
    case 2: return "i";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "10.2";
    case 2: return "30.0";
    }
    return "";
  }
private:
  ShaderModule *mod;
  std::string uid;

};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::ambient_occulsion(SFO obj, float d, float i)
{
  ShaderModule *mod = find_shader_module(e,obj);
  SFO ao = add_shader_module(e, new AOModule(mod));
  SFO ao_1 = bind_arg(ao, "i", ToNum(i));
  SFO ao_2 = bind_arg(ao_1, "d", ToNum(d));
  return ao_2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::rounded_cube(float start_x, float end_x,
						    float start_y, float end_y,
						    float start_z, float end_z,
						    float r)
{
  SFO r_cube = add_shader_module(e, new RoundedCube());
  SFO r_cube_1 = bind_arg(r_cube, "tl", vec3_to_string(e,start_x, start_y, start_z));
  SFO r_cube_2 = bind_arg(r_cube_1, "br", vec3_to_string(e,end_x, end_y, end_z));
  SFO r_cube_3 = bind_arg(r_cube_2, "r", ToNum(r));
  return r_cube_3;
}
class NoiseModule : public ShaderModule
{
public:
  NoiseModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float noise" + uid + "(vec3 pt, float str)\n"
      "{\n"
      "    return "+funccall_to_string(mod)+";\n"
      "}\n"
      "vec4 noise_color"+ uid+"(vec3 pt, float str)\n"
      "{\n"
      "   float v = fract(sin(dot(pt.xz, vec2(12.9898,78.233))) * 43758.5453);\n"
      "   vec4 col = " + color_funccall_to_string(mod) + ";\n"
      "   return vec4(mix(col.xyz, vec3(v,v,v), str),1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "noise" + uid; }
  virtual std::string ColorFunctionName() const { return "noise_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual std::string ArgName(int i) const { if (i==0) return "pt"; return "str"; }
  virtual std::string ArgValue(int i) const { if (i==0) return "pt"; return "0.2"; }
private:
  std::string uid;
  ShaderModule *mod;
};
GameApi::SFO GameApi::ShaderModuleApi::noise(SFO obj, float strength)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  SFO noise = add_shader_module(e, new NoiseModule(obj_m));
  SFO noise_2 = bind_arg(noise, "str", ToNum(strength));
  return noise_2;
}
class ModXModule : public ShaderModule
{
public:
  ModXModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float mod_x" + uid + "(vec3 pt, float dx)\n"
      "{\n"
      "   return " + funccall_to_string_with_replace(mod,"pt","vec3(mod(pt.x,dx),pt.y,pt.z)") + ";\n"
      "}\n"
      "vec4 mod_x_color" + uid + "(vec3 pt, float dx)\n"
      "{\n"
       "   return " + color_funccall_to_string_with_replace(mod,"pt","vec3(mod(pt.x,dx),pt.y,pt.z)") + ";\n" 
      "}\n";
  }
  virtual std::string FunctionName() const { return "mod_x" + uid; }
  virtual std::string ColorFunctionName() const { return "mod_x_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual std::string ArgName(int i) const{ if (i==0) return "pt"; return "dx"; }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "100.0";
  }

private:
  ShaderModule *mod;
  std::string uid;
};

class ModYModule : public ShaderModule
{
public:
  ModYModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float mod_y" + uid + "(vec3 pt, float dy)\n"
      "{\n"
      "   return " + funccall_to_string_with_replace(mod,"pt","vec3(pt.x, mod(pt.y,dy),pt.z)") + ";\n"
      "}\n"
      "vec4 mod_y_color" + uid + "(vec3 pt, float dy)\n"
      "{\n"
       "   return " + color_funccall_to_string_with_replace(mod,"pt","vec3(pt.x,mod(pt.y,dy),pt.z)") + ";\n" 
      "}\n";
  }
  virtual std::string FunctionName() const { return "mod_y" + uid; }
  virtual std::string ColorFunctionName() const { return "mod_y_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual std::string ArgName(int i) const{ if (i==0) return "pt"; return "dy"; }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "100.0";
  }

private:
  ShaderModule *mod;
  std::string uid;
};

class ModZModule : public ShaderModule
{
public:
  ModZModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float mod_z" + uid + "(vec3 pt, float dz)\n"
      "{\n"
      "   return " + funccall_to_string_with_replace(mod,"pt","vec3(pt.x, pt.y, mod(pt.z,dz))") + ";\n"
      "}\n"
      "vec4 mod_z_color" + uid + "(vec3 pt, float dz)\n"
      "{\n"
       "   return " + color_funccall_to_string_with_replace(mod,"pt","vec3(pt.x,pt.y, mod(pt.z,dz))") + ";\n" 
      "}\n";
  }
  virtual std::string FunctionName() const { return "mod_z" + uid; }
  virtual std::string ColorFunctionName() const { return "mod_z_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual std::string ArgName(int i) const{ if (i==0) return "pt"; return "dz"; }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    return "100.0";
  }

private:
  ShaderModule *mod;
  std::string uid;
};


EXPORT GameApi::SFO GameApi::ShaderModuleApi::mod_x(SFO obj, float dx)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  SFO mod_1 = add_shader_module(e, new ModXModule(obj_m));
  SFO mod_2 = bind_arg(mod_1, "dx", ToNum(dx));
  return mod_2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::mod_y(SFO obj, float dy)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  SFO mod_1 = add_shader_module(e, new ModYModule(obj_m));
  SFO mod_2 = bind_arg(mod_1, "dy", ToNum(dy));
  return mod_2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::mod_z(SFO obj, float dz)
{
  ShaderModule *obj_m = find_shader_module(e, obj);
  SFO mod_1 = add_shader_module(e, new ModZModule(obj_m));
  SFO mod_2 = bind_arg(mod_1, "dz", ToNum(dz));
  return mod_2;
}
class FunctionModule : public ShaderModule
{
public:
  FunctionModule(std::string func, std::string name, std::vector<std::string> param_name, std::vector<std::string> arg_value) : func(func), name(name), param_name(param_name), arg_value(arg_value) { }
  virtual std::string Function() const { return func; }
  virtual std::string FunctionName() const { return name; }
  virtual std::string ColorFunctionName() const { return ""; }
  virtual int NumArgs() const { return param_name.size(); }
  virtual std::string ArgName(int i) const { return param_name[i]; }
  virtual std::string ArgValue(int i) const { return arg_value[i]; }
private:
  std::string func;
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> arg_value;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::function(std::function<std::string (std::string id)> vec3_to_float_function, std::string function_name, std::vector<std::string> param_names, std::vector<std::string> arg_values)
{
  std::string id = unique_id();
  std::string func = vec3_to_float_function(id);

  SFO func_1 = add_shader_module(e, new FunctionModule(func, function_name+id, param_names, arg_values));
  return func_1;
}

class ColorFunctionModule : public ShaderModule
{
public:
  ColorFunctionModule(ShaderModule *mod, std::string func, std::string name) : mod(mod), func(func), name(name) { }
  virtual std::string Function() const { return mod->Function()+func; }
  virtual std::string FunctionName() const { return mod->FunctionName(); }
  virtual std::string ColorFunctionName() const { return name; }
  virtual int NumArgs() const { return mod->NumArgs(); }
  virtual std::string ArgName(int i) const { return mod->ArgName(i); }
  virtual std::string ArgValue(int i) const { return mod->ArgValue(i); }
private:
  ShaderModule *mod;
  std::string func;
  std::string name;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::color_function(SFO orig, std::function<std::string (std::string id)> vec3_to_float_function, std::string function_name)
{
  std::string id = unique_id();
  std::string func = vec3_to_float_function(id);
  ShaderModule *mod = find_shader_module(e, orig);
  SFO func_1 = add_shader_module(e, new ColorFunctionModule(mod, func, function_name+id));
  return func_1;
}

class BoundingSphereModule : public ShaderModule
{
public:
  BoundingSphereModule(ShaderModule *prim, ShaderModule *inside, ShaderModule *outside ) : prim(prim), inside(inside), outside(outside)
  {
    uid = unique_id();
  }
  virtual std::string Function() const
  {
    return prim->Function() + inside->Function() + outside->Function() + 
      "float bounding_prim_" + uid + "(vec3 pt)\n"
      "{\n"
      "    float dist = " + funccall_to_string(prim) + ";\n"
      "    if (dist < 0.0) {\n"
      "       return " + funccall_to_string(inside) + ";\n"
      "    } else {\n"
      "       return " + funccall_to_string(outside) + ";\n"
      "    }\n"
      "}\n"
      "vec4 bounding_prim_color_" + uid + "(vec3 pt)\n"
      "{\n"
      "    float dist = " + funccall_to_string(prim) + ";\n"
      "    if (dist < 0.0) {\n"
      "       return " + color_funccall_to_string(inside) + ";\n"
      "    } else {\n"
      "       return " + color_funccall_to_string(outside) + ";\n"
      "    }\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "bounding_prim_" + uid; }
  virtual std::string ColorFunctionName() const { return "bounding_prim_color_" + uid; }
  virtual int NumArgs() const { return 1; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    }
    return "";
  }

private:
  std::string uid;
  ShaderModule *prim;
  ShaderModule *inside;
  ShaderModule *outside;
};
class TextureModule : public ShaderModule
{
public:
  TextureModule(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float tex_pos" + uid + "(vec3 pt)\n"
      "{\n"
      "   return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 tex_color" + uid + "(vec3 pt)\n"
      "{\n"
      "   vec4 coords = " + color_funccall_to_string(mod) + ";\n"
      "   vec4 rgb = texture2D(tex, coords.xy);\n"
      "   return rgb;\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "tex_pos" + uid; }
  virtual std::string ColorFunctionName() const { return "tex_color" + uid; }
  virtual int NumArgs() const { return 1; }
  virtual std::string ArgName(int i) const { return "pt"; }
  virtual std::string ArgValue(int i) const { return "pt"; }

private:
  ShaderModule *mod;
  std::string uid;
};
class TextureCube : public ShaderModule
{
public:
  TextureCube() { uid = unique_id(); }
  virtual std::string Function() const
  {
    return 
      "float texcube" + uid + "(vec3 pt, vec3 tl, vec3 br)\n"
      "{\n"
      "   pt-=tl;\n"
      "   vec3 pt2 = pt;\n"
      "   vec3 s = br-tl;\n"
      "   pt /=s;\n"
      "   if (pt.x<0.0||pt.x>1.0||pt.y<0.0||pt.y>1.0||pt.z<0.0||pt.z>1.0){\n"
      "      s/=2.0;\n"
      "      pt2-=s;\n"
      "      return length(max(abs(pt2)-s,0.0))+1.0;\n"
      "   }\n"
      "   vec4 pixel = texture2D(tex, pt.xz);\n"
      "   float dist = pixel.r;\n"
      "   dist*=255.0;\n"
      //"   dist/=600.0;\n"
      //"   dist*=s.x;\n"
      "   return dist-0.4;\n"
      "}\n"
      "vec4 texcube_color" + uid + "(vec3 pt, vec3 tl, vec3 br)\n"
      "{\n"
      "    return vec4(1.0,1.0,1.0,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "texcube" + uid; }
  virtual std::string ColorFunctionName() const { return "texcube_color" + uid; }
  virtual int NumArgs() const { return 3; }
  virtual std::string ArgName(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "tl";
    case 2: return "br";
    }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i) {
    case 0: return "pt";
    case 1: return "vec3(0.0,0.0,0.0)";
    case 2: return "vec3(100.0,100.0,100.0)";
    }
    return "";
  }

private:
  std::string uid;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::texture_box(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  SFO r_cube = add_shader_module(e, new TextureCube());
  SFO r_cube_1 = bind_arg(r_cube, "tl", vec3_to_string(e,start_x, start_y, start_z));
  SFO r_cube_2 = bind_arg(r_cube_1, "br", vec3_to_string(e,end_x, end_y, end_z));
  return r_cube_2;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::texture(SFO orig)
{
  ShaderModule *mod = find_shader_module(e, orig);
  SFO tex_0 = add_shader_module(e, new TextureModule(mod));
  return tex_0;
}
EXPORT GameApi::SFO GameApi::ShaderModuleApi::bounding_primitive(SFO prim, SFO inside, SFO outside)
{
  ShaderModule *prim_1 = find_shader_module(e, prim);
  ShaderModule *inside_1 = find_shader_module(e, inside);
  ShaderModule *outside_1 = find_shader_module(e, outside);
  SFO bound = add_shader_module(e, new BoundingSphereModule(prim_1, inside_1, outside_1));
  return bound;
}
class GrayscaleDistance : public ShaderModule
{
public:
  GrayscaleDistance(ShaderModule *mod, ShaderModule *mod2) : mod(mod),mod2(mod2) { uid = unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() + mod2->Function() +
      "float grayscale_dist" + uid + "(vec3 pt, float length)\n"
      "{\n"
      "   return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 grayscale_dist_color" + uid + "(vec3 pt, float length)\n"
      "{\n"
      "   float dist = " + funccall_to_string(mod2) + ";\n"
      "   dist /= length;\n"
      "   return vec4(dist,dist,dist,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "grayscale_dist" + uid; }
  virtual std::string ColorFunctionName() const { return "grayscale_dist_color" + uid; }
  virtual int NumArgs() const { return 2; }
  virtual std::string ArgName(int i) const
  {
    switch(i)
      {
      case 0: return "pt";
      case 1: return "length";
      }
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    switch(i)
      {
      case 0: return "pt";
      case 1: return "100.0";
      }
    return "";
  }

private:
  std::string uid;
  ShaderModule *mod;
  ShaderModule *mod2;
};
EXPORT GameApi::SFO GameApi::ShaderModuleApi::grayscale_from_distance(SFO obj, SFO dist_obj, float length)
{
  ShaderModule *mod = find_shader_module(e, obj);
  ShaderModule *dist = find_shader_module(e, dist_obj);
  SFO m = add_shader_module(e, new GrayscaleDistance(mod, dist));
  SFO m2 = bind_arg(m, "length", ToNum(length));
  return m2;
}
#if 0
EXPORT GameApi::SFO GameApi::ShaderModuleApi::color_from_position(SFO obj, float sx, float sy, float sz)
{
  ShaderModule *mod = find_shader_module(e, obj);
  SFO m = add_shader_module(e, new ColorFromPosition(mod));
  SFO m2 = bind_arg(m, "sx", ToNum(sx));
  SFO m3 = bind_arg(m2, "sy", ToNum(sy));
  SFO m4 = bind_arg(m3, "sz", ToNum(sz));
  return m4;
}
#endif

class ColorModFromPosition : public ShaderModule
{
public:
  ColorModFromPosition(ShaderModule *mod) : mod(mod) { uid=unique_id(); }
  virtual std::string Function() const
  {
    return mod->Function() +
      "float colormod" + uid + "(vec3 pt, float px, float py, float pz, float sx, float sy, float sz)\n"
      "{\n"
      "    return " + funccall_to_string(mod) + ";\n"
      "}\n"
      "vec4 colormod_color" + uid + "(vec3 pt, float px, float py, float pz, float sx, float sy, float sz)\n"
      "{\n"
      "  float xx = mod(pt.x-px, sx)/sx;\n"    
      "  float yy = mod(pt.y-py, sy)/sy;\n"
      "  float zz = mod(pt.z-pz, sz)/sz;\n"
      "  return vec4(xx,yy,zz,1.0);\n"
      "}\n";
  }
  virtual std::string FunctionName() const { return "colormod" + uid; }
  virtual std::string ColorFunctionName() const { return "colormod_color" + uid; }
  virtual int NumArgs() const { return 7; }
  virtual std::string ArgName(int i) const
  {
    if (i==0) return "pt";
    if (i==1) return "px";
    if (i==2) return "py";
    if (i==3) return "pz";
    if (i==4) return "sx";
    if (i==5) return "sy";
    if (i==6) return "sz";
    return "";
  }
  virtual std::string ArgValue(int i) const
  {
    if (i==0) return "pt";
    if (i==1) return "0.0";
    if (i==2) return "0.0";
    if (i==3) return "0.0";
    if (i==4) return "100.0";
    if (i==5) return "100.0";
    if (i==6) return "100.0";
    return "";
  }
private:
  std::string uid;
  ShaderModule *mod;
};
GameApi::SFO GameApi::ShaderModuleApi::empty()
{
  return cube();
}
GameApi::SFO GameApi::ShaderModuleApi::colormod_from_position(SFO obj, float px, float py, float pz, float sx, float sy, float sz)
{
  ShaderModule *mod = find_shader_module(e, obj);
  SFO m = add_shader_module(e, new ColorModFromPosition(mod));
  SFO m01 = bind_arg(m, "px", ToNum(px));
  SFO m02 = bind_arg(m01, "px", ToNum(py));
  SFO m03 = bind_arg(m02, "px", ToNum(pz));
  SFO m2 = bind_arg(m03, "sx", ToNum(sx));
  SFO m3 = bind_arg(m2, "sy", ToNum(sy));
  SFO m4 = bind_arg(m3, "sz", ToNum(sz));
  return m4;
}

class SFOML : public MainLoopItem
{
public:
  SFOML(GameApi::EveryApi &ev, GameApi::SFO sfo, float sx, float sy) : ev(ev), sfo(sfo), sx(sx), sy(sy) {
    firsttime = true;
  }
  GameApi::V func(int face, int point, GameApi::EveryApi &ev)
  {
    switch(point)
      {
      case 0: return ev.vector_api.vector(0.0,0.0,0.0);
      case 1: return ev.vector_api.vector(1.0,0.0,0.0);
      case 2: return ev.vector_api.vector(1.0,1.0,0.0);
      case 3: return ev.vector_api.vector(0.0,1.0,0.0);
      };
    return ev.vector_api.vector(0.0,0.0,0.0);
  }
  
  GameApi::PT func2(int face, int point, GameApi::EveryApi &ev)
  {
    switch(point)
      {
      case 0: return ev.point_api.point(0.0,1.0,0.0);
      case 1: return ev.point_api.point(1.0,1.0,0.0);
      case 2: return ev.point_api.point(1.0,0.0,0.0);
      case 3: return ev.point_api.point(0.0,0.0,0.0);
      };
    return ev.point_api.point(0.0,0.0,0.0);
  }

  virtual void execute(MainLoopEnv &e)
  {
    using std::placeholders::_1;
    using std::placeholders::_2;

    
    if (firsttime)
      {
	GameApi::SFO render = ev.sh_api.render(sfo);
	sh2 = ev.shader_api.get_normal_shader("screen", "screen", "", "", "", false, render);
	int screen_x = ev.mainloop_api.get_screen_width();
	int screen_y = ev.mainloop_api.get_screen_height();
	ev.mainloop_api.init_3d(sh2, screen_x, screen_y);
	ev.mainloop_api.alpha(true);
	GameApi::P poly_0;
	if (sx<0.0 ||sy<0.0)
	  {
	    poly_0 = ev.polygon_api.fullscreen_quad(ev);
	  }
	else
	  {
	    poly_0 = ev.polygon_api.quad_z(0.0, sx,
					 0.0, sy,
					 0.0);
	  }
	GameApi::P poly_1 = ev.polygon_api.normal_function(poly_0, std::bind(&SFOML::func, this, _1, _2, std::ref(ev)));
	GameApi::P poly_2 = ev.polygon_api.texcoord_function(poly_1, std::bind(&SFOML::func2, this, _1, _2, std::ref(ev)));
	va = ev.polygon_api.create_vertex_array(poly_2, true);
	firsttime = false;
      }

    GameApi::M m = add_matrix2( ev.get_env(), e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(ev.get_env(), e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(ev.get_env(), e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.use(sh2);
    ev.shader_api.set_var(sh2, "in_MV", m);
    ev.shader_api.set_var(sh2, "in_T", m1);
    ev.shader_api.set_var(sh2, "in_N", m2);
    ev.shader_api.set_var(sh2, "time", e.time);
	
    
    ev.polygon_api.render_vertex_array(va);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual int shader_id() { return -1; }
private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  GameApi::PolygonObj *obj;
  bool firsttime;
  GameApi::SH sh2,sh;
  GameApi::VA va;
  float sx,sy;
};

GameApi::ML GameApi::ShaderModuleApi::sfo_to_ml(EveryApi &ev, SFO sfo, float sx, float sy)
{
  return add_main_loop(e, new SFOML(ev, sfo, sx,sy));
}
