#include "accel.hh"

#include "..\GameApi_h.hh"

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

std::string unique_id_apiitem()
{
  static int id = 0;
  id++;
  std::stringstream ss;
  ss << id;
  return std::string("I") + ss.str();
}

// GLOBAL VARIABLE
std::vector<AccelStructure*> envimpl_accel;
std::vector<PreCalcMainLoop*> envimpl_precalc;

PML add_pml(PreCalcMainLoop *p)
{
  envimpl_precalc.push_back(p);
  PML as;
  as.id = envimpl_precalc.size()-1;
  return as;
}


AK add_ak(AccelStructure *w)
{
  envimpl_accel.push_back(w);
  AK as;
  as.id = envimpl_accel.size()-1;
  return as;
}
PreCalcMainLoop *find_pml(PML pm)
{
  PreCalcMainLoop *i = envimpl_precalc[pm.id];
  return i;
}
AccelStructure *find_ak(AK as)
{
  AccelStructure *i = envimpl_accel[as.id];
  return i;
}

template<class T>
class FromStreamClass2
{
public:
  T from_stream(std::string s)
  {
    T t;
    std::stringstream is(unhexify(s));
    is >> t;
    return t;
  }
};

template<>
class FromStreamClass2<std::string>
{
public:
  std::string from_stream(std::string s)
  {
    return unhexify(s);
  }
};


template<>
class FromStreamClass2<unsigned int>
{
public:
  unsigned int from_stream(std::string s)
  {
  unsigned int bm;
  //char c;
  //is >> c;
  //is >> c;
  std::stringstream is(unhexify(s));
  is >> std::hex >> bm >> std::dec;
  return bm;
  }
};


template<class T>
class FromStreamClass2<std::vector<T>>
{
public:
  std::vector<T> from_stream(std::string sk)
  {
    std::string s = unhexify(sk);
    std::cout << "Vector" << std::endl;
    std::vector<T> vec;
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass2<T> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    T t = cls.from_stream(hexify(substr));
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};



#define MACRO(lab) \
template<> \
class FromStreamClass2<lab> \
{ \
public:\
  lab from_stream(std::string s)\
  {\
  lab bm;\
  std::stringstream is(unhexify(s));		\
  is >> bm.id;\
  return bm;\
  }\
};
#define MACRO_GAMEAPI(lab) \
template<> \
class FromStreamClass2<lab> \
{ \
public:\
  lab from_stream(std::string s)\
  {\
  lab bm;\
  std::stringstream is(s);		\
  is >> bm.id;\
  return bm;\
  }\
};

MACRO(AK);
MACRO(PML);
MACRO_GAMEAPI(GameApi::P);

template<typename T> T from_stream22(std::stringstream &is)
{
  FromStreamClass2<T> cls;
  std::string s;
  is >> s;
  return cls.from_stream(s);
}

template<class RT, class... P>
int funccall(RT (fptr)(P...), std::vector<std::string> s, std::vector<std::string> param_name)
{
#if 0
  // EXECUTEENV NOT USED
  int s3 = s.size();
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

  std::stringstream ss;
  int s2 = s.size();
#ifndef EMSCRIPTEN
  for(int i=s2-1;i>=0;i--)
    {
      ss << hexify(s[i]) << " ";
    }
#else
  for(int i=0;i<s2;i++)
    {
      ss << hexify(s[i]) << " ";
    }
#endif
  std::cout << "FuncCall: " << ss.str() << std::endl;

  std::stringstream ss2(ss.str());
  
  //T *ptr = &(ev.*api);
  RT val = (*fptr)(from_stream22<P>(ss2)...);
  std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id;
}


template<class RT, class... P>
class ApiItem : public Item
{
public:
  ApiItem(RT (*fptr)(P...),
	  std::string name,
	  std::vector<std::string> param_name,
	  std::vector<std::string> param_type,
	  std::vector<std::string> param_default,
	  std::string ret_type,
	  std::string func_name) : fptr(fptr), name(name), param_name(param_name),
				   param_type(param_type), param_default(param_default), return_type(ret_type), func_name(func_name) { }
  std::string Name() const { return name; }
  int ParamCount() const { return param_name.size(); }
  std::string ParamName(int p) const { return param_name[p]; }
  std::string ParamType(int p) const { return param_type[p]; }
  std::string ParamDefault(int p) const { return param_default[p]; }
  std::string ReturnType() const { return return_type; }
  int Execute(std::vector<std::string> params)
  {
    return funccall(fptr, params, param_name);
  }
  std::pair<std::string,std::string> CodeGen(std::vector<std::string> params, std::vector<std::string> param_names)
  {
    std::string s;
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
    s+= func_name;
    s+= "(";
    int sk = param_names.size();
    for(int i=0;i<sk;i++)
      {
	s+= param_names[i];
	if (i!=int(param_names.size())-1) s+=",";
      }
    s+=");\n";
    return std::make_pair(id, s);

  }
private:
  RT (*fptr)(P...);
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> param_type;
  std::vector<std::string> param_default;
  std::string return_type;
  std::string func_name;
};

template<class RT, class... P>
Item* ApiItemF(RT (*fptr)(P...),
	       std::string name,
	       std::vector<std::string> param_name,
	       std::vector<std::string> param_type,
	       std::vector<std::string> param_default,
	       std::string return_type,
	       std::string func_name)
{
  return new ApiItem<RT,P...>(fptr, name, param_name, param_type, param_default, return_type, func_name);
}

// DLL ENTRY POINTS
std::string api_name() { return "AccelApi"; }
int num_displays() { return 1; }
std::string type_symbol()
{
  return "PML";
}
void display(int i, int disp)
{
  switch(disp)
    {
    case 0:
      PreCalcMainLoop *loop = envimpl_precalc[i];
      loop->Execute();
      break;
    }
}

std::vector<Item*> functions()
{
  std::vector<Item*> vec;

  vec.push_back(ApiItemF(&empty, 
			 "ak_empty", 
			 { "sx", "sy", "sz", 
			     "start_x", "end_x",
			     "start_y", "end_y",
			     "start_z", "end_z"
			     },
			 { "int", "int", "int",
			     "float", "float",
			     "float", "float",
			     "float", "float"
			     },
			 { "100", "100", "100",
			     "-300.0", "300.0",
			     "-300.0", "300.0",
			     "-300.0", "300.0"
			     },
			 "AK", "empty"));
  vec.push_back(ApiItemF(&bind_p,
			 "ak_bind_p",
			 { "orig", "p" },
			 { "AK", "P" },
			 { "", "" },
			 "PML", "bind_p"));
  vec.push_back(ApiItemF(&clear,
			 "ak_clear",
			 { "orig" },
			 { "AK" },
			 { "" },
			 "PML", "clear"));
  vec.push_back(ApiItemF(&or_elem,
			 "ak_or_elem",
			 { "p1", "p2" },
			 { "PML", "PML" },
			 { "", "" },
			 "PML", "or_elem"));
  vec.push_back(ApiItemF(&or_array,
			 "ak_or_array",
			 { "vec" },
			 { "[PML]" },
			 { "" },
			 "PML", "or_array"));
  return vec;
}

GameApi::Env &env()
{
  return *GameApi::Env::Latest_Env();
}

struct AccelNodeSpec
{
  int x,y,z;
};

class GridAccel : public AccelStructure
{
public:
  GridAccel(int sx, int sy, int sz,
	    float start_x, float end_x,
	    float start_y, float end_y,
	    float start_z, float end_z) : sx(sx), sy(sy), sz(sz),
					  start_x(start_x), end_x(end_x),
					  start_y(start_y), end_y(end_y),
					  start_z(start_z), end_z(end_z) 
  { 
    grid = new AccelNode[sx*sy*sz];
  }
  void clear()
  {
    delete [] grid;
    grid = new AccelNode[sx*sy*sz];
  }
  AccelNode *find_node(const AccelNodeSpec &spec) const
  {
    return &grid[spec.z*sx*sy +
		spec.y*sx +
		spec.x];
  }
  AccelNodeSpec find_spec(AccelNode *n) const
  {
    int num = n - grid;
    int zz = num / (sx*sy);
    int rest = num - zz*sx*sy;
    int yy = rest / sx;
    int rest2 = rest - yy*sx;
    int xx = rest2;
    AccelNodeSpec s;
    s.x = xx;
    s.y = yy;
    s.z = zz;
    return s;
  }
  virtual AccelNode *find_point(Point p) const
  {
    if (p.x<start_x) { return 0; }
    if (p.y<start_y) { return 0; }
    if (p.z<start_z) { return 0; }
    if (p.x>end_x) { return 0; }
    if (p.y>end_y) { return 0; }
    if (p.z>end_z) { return 0; }
				 
    p-=Vector(start_x, start_y, start_z);
    float dx = p.x / (end_x-start_x);
    float dy = p.y / (end_y-start_y);
    float dz = p.z / (end_z-start_z);
    // now [0..1]
    float ddx = dx * sx;
    float ddy = dy * sy;
    float ddz = dz * sz;
    // now [0..box]
    AccelNodeSpec s;
    s.x = int(ddx);
    s.y = int(ddy);
    s.z = int(ddz);
    AccelNode *n = find_node(s);
    return n;
  }
  Point point_at_edge_of_box(Point start, Point target, AccelNode *discard) const
  {
    // TODO RAY-BOX INTERSECTION
  }
  virtual AccelNode *find_next(AccelNode *current, Point start, Point target) const
  {
  }
  virtual std::vector<AccelNode*> find_quad(Point p1, Point p2, Point p3, Point p4) const
  {
    int xxxmin = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
    int yyymin = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
    int zzzmin = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));

    int xxxmax = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
    int yyymax = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
    int zzzmax = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));

    return find_cube(xxxmin, xxxmax,
		     yyymin, yyymax,
		     zzzmin, zzzmax);
		     
  }
  virtual std::vector<AccelNode*> find_span(Point p1, Point p2) const
  {
    std::vector<AccelNode*> vec;
    AccelNode *n = find_point(p1);
    vec.push_back(n);
    while(n)
      {
	n = find_next(n, p1, p2);
	if (n)
	  {
	    p1 = point_at_edge_of_box(p1,p2,n);
	    vec.push_back(n);
	  }
      }
    return vec;
  }
  virtual std::vector<AccelNode*> find_tri(Point p1, Point p2, Point p3) const
  {
    int xxxmin = std::min(std::min(p1.x,p2.x),p3.x);
    int yyymin = std::min(std::min(p1.y,p2.y),p3.y);
    int zzzmin = std::min(std::min(p1.z,p2.z),p3.z);

    int xxxmax = std::max(std::max(p1.x,p2.x),p3.x);
    int yyymax = std::max(std::max(p1.y,p2.y),p3.y);
    int zzzmax = std::max(std::max(p1.z,p2.z),p3.z);

    return find_cube(xxxmin, xxxmax,
		     yyymin, yyymax,
		     zzzmin, zzzmax);
  }
  virtual std::vector<AccelNode*> find_cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) const
  {
    AccelNode *n0 = find_point(Point(start_x, start_y, start_z));
    AccelNode *nx = find_point(Point(end_x, start_y, start_z));
    AccelNode *ny = find_point(Point(start_x, end_y, start_z));
    AccelNode *nz = find_point(Point(start_x, start_y, end_z));
    
    AccelNodeSpec s0 = find_spec(n0);
    AccelNodeSpec sx = find_spec(nx);
    AccelNodeSpec sy = find_spec(ny);
    AccelNodeSpec sz = find_spec(nz);

    int s_x = s0.x;
    int e_x = sx.x;
    int s_y = s0.y;
    int e_y = sy.y;
    int s_z = s0.z;
    int e_z = sz.z;
    std::vector<AccelNode*> vec;
    for(int z=s_z;z<e_z;z++)
      for(int y=s_y;y<e_y;y++)
	for(int x=s_x;x<e_x;x++)
	  {
	    AccelNodeSpec s = { x,y,z };
	    vec.push_back(find_node(s));
	  }
    return vec;
  }
  virtual void push_back(Point p, void *data)
  {
    AccelNode *n = find_point(p);
    push_back(n, data);
  }
  virtual void push_back(AccelNode *n, void *data)
  {
    if (n)
      n->vec.push_back(data);
  }

private:
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  AccelNode *grid;
};

AK empty(int sx, int sy, int sz,
	 float start_x, float end_x,
	 float start_y, float end_y,
	 float start_z, float end_z
	 )
{
  return add_ak(new GridAccel(sx,sy,sz,start_x, end_x, start_y, end_y, start_z, end_z));
}

struct Accel_P_ref
{
  FaceCollection *coll;
  int face;
};

class BindPreCalc : public PreCalcMainLoop
{
public:
  BindPreCalc(FaceCollection *coll, AccelStructure *accel) : coll(coll), accel(accel) { }
  void Execute()
  {
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      Accel_P_ref *ref = new Accel_P_ref;
      ref->coll = coll;
      ref->face = i;
      int c = coll->NumPoints(i);
      std::vector<AccelNode*> vec;
      switch(c) {
      case 3:
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  vec = accel->find_tri(p1,p2,p3);
	  break;
	}
      case 4:
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  Point p4 = coll->FacePoint(i,3);
	  vec = accel->find_quad(p1,p2,p3,p4);
	  break;
	}
      };
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	  accel->push_back(vec[i], (void*)ref);
	}
    }

  }
private:
  FaceCollection *coll;
  AccelStructure *accel;
};

class ClearPreCalc : public PreCalcMainLoop
{
public:
  ClearPreCalc(AccelStructure *accel) : accel(accel) { }
  void Execute()
  {
    accel->clear();
  }
private:
  AccelStructure *accel;
};

PML bind_p(AK orig, GameApi::P p)
{
  GameApi::Env &e = env();
  FaceCollection *coll = find_facecoll(e, p);
  AccelStructure *accel = find_ak(orig);
  return add_pml(new BindPreCalc(coll,accel));
}
PML clear(AK orig)
{
  AccelStructure *accel = find_ak(orig);
  return add_pml(new ClearPreCalc(accel));
}
class OrElemPreCalc : public PreCalcMainLoop
{
public:
  OrElemPreCalc(PreCalcMainLoop *m1, PreCalcMainLoop *m2) : m1(m1), m2(m2) { }
  void Execute()
  {
    m1->Execute();
    m2->Execute();
  }
private:
  PreCalcMainLoop *m1, *m2;
};
PML or_elem(PML p1, PML p2)
{
  PreCalcMainLoop *m1 = find_pml(p1);
  PreCalcMainLoop *m2 = find_pml(p2);
  return add_pml(new OrElemPreCalc(m1,m2));
}
class OrArrayPreCalc : public PreCalcMainLoop
{
public:
  OrArrayPreCalc(std::vector<PreCalcMainLoop*> vec) : vec(vec) {}
  void Execute()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Execute();
      }
  }
private:
  std::vector<PreCalcMainLoop*> vec;
};
PML or_array(std::vector<PML> vec)
{
  std::vector<PreCalcMainLoop*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_pml(vec[i]));
    }
  return add_pml(new OrArrayPreCalc(vec2));
}
