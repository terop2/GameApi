
#include "GameApi_h.hh"

extern std::string gameapi_homepageurl;

class BarChart : public FaceCollection
{
public:
  BarChart(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string homepage, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per) : env(env), ev(ev), url(url), homepage(homepage), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), per(per) { 
    result.id = -1;
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = env.get_loaded_async_url(url);
    if (!vec) return;
    std::string str(vec->begin(),vec->end());
    std::stringstream ss(str);
    float val=0.0;
    data.clear();
    while(ss >> val) data.push_back(val); 

    render_vec.clear();
    int s = data.size();
    for(int i=0;i<s;i++) {
      float val = data[i];
      float s_x = i*(end_x-start_x)/s + start_x;
      float e_x = s_x + per*(end_x-start_x)/s;
      float s_y = (1.0-val)*(end_y-start_y) + start_y;
      float e_y = end_y;
      float s_z = start_z;
      float e_z = end_z;
      GameApi::P cube = ev.polygon_api.cube(s_x,e_x,
					    s_y,e_y,
					    s_z,e_z);
      render_vec.push_back(cube);
    }
    result = ev.polygon_api.or_array2(render_vec);

    FaceCollection *coll = find_facecoll(env, result);
    coll->Prepare();
  }


  virtual int NumFaces() const 
  {
    if (result.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (result.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (result.id==-1) return Point(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env, result);
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (result.id==-1) return Vector(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env, result);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    if (result.id==-1) return 0xffffffff;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (result.id==-1) { Point2d p; p.x =0.0; p.y=0.0; return p; }
    FaceCollection *coll = find_facecoll(env, result);
    return coll->TexCoord(face,point);

  }
  virtual float TexCoord3(int face, int point) const { 
    if (result.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->TexCoord3(face,point);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  float per;
  std::vector<float> data;
  std::vector<GameApi::P> render_vec;
  GameApi::P result;
};

class BarChart2 : public FaceCollection
{
public:
  BarChart2(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string homepage, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per_x, float per_y) : env(env), ev(ev), url(url), homepage(homepage), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), per_x(per_x), per_y(per_y) {
    result.id = -1;
  }
    void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = env.get_loaded_async_url(url);
    if (!vec) return;
    std::string str(vec->begin(),vec->end());
    std::stringstream ss(str);
    float val = 0.0;
    data.clear();
    std::string line;
    int i =0;
    while(std::getline(ss,line)) {
      std::cout << "Line: " << line << std::endl;
      data.push_back(std::vector<float>());
      std::stringstream ss2(line);
      while(ss2>>val) {
	std::cout << "Value: " << val << std::endl;
	data[i].push_back(val);
      }
      i++;
    }
    render_vec.clear();
    int s = data.size();
    std::cout << "s: " << s << std::endl;
    for(int i=0;i<s;i++) {
      int s2 = data[i].size();
      std::cout << "s2: " << s2 << std::endl;
      for(int j=0;j<s2;j++) {
	float val = data[i][j];
	float s_x = j*(end_x-start_x)/s2 + start_x;
	float e_x = s_x + per_x*(end_x-start_x)/s2;
	float s_y = (1.0-val)*(end_y-start_y) + start_y;
	float e_y = end_y;
	float s_z = i*(end_z-start_z)/s + start_z;
	float e_z = s_z + per_y*(end_z-start_z)/s;
	GameApi::P cube = ev.polygon_api.cube(s_x,e_x,
					      s_y,e_y,
					      s_z,e_z);
	render_vec.push_back(cube);
      }
    }
    result = ev.polygon_api.or_array2(render_vec);
    FaceCollection *coll = find_facecoll(env,result);
    coll->Prepare();
  }
  virtual int NumFaces() const 
  {
    if (result.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (result.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (result.id==-1) return Point(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env, result);
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (result.id==-1) return Vector(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env, result);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    if (result.id==-1) return 0xffffffff;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (result.id==-1) { Point2d p; p.x =0.0; p.y=0.0; return p; }
    FaceCollection *coll = find_facecoll(env, result);
    return coll->TexCoord(face,point);

  }
  virtual float TexCoord3(int face, int point) const { 
    if (result.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env, result);
    return coll->TexCoord3(face,point);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  float per_x, per_y;
  std::vector<std::vector<float> > data;
  std::vector<GameApi::P> render_vec;
  GameApi::P result;
};

GameApi::P GameApi::PolygonApi::bar_chart( GameApi::EveryApi &ev, std::string url, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per )
{
  return add_polygon2(e, new BarChart(e, ev, url, gameapi_homepageurl, start_x, end_x, end_y, start_y, start_z, end_z, per/100.0), 1);
}

GameApi::P GameApi::PolygonApi::bar_chart2( GameApi::EveryApi &ev, std::string url, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per, float per2 )
{
  return add_polygon2(e, new BarChart2(e, ev, url, gameapi_homepageurl, start_x, end_x, end_y, start_y, start_z, end_z, per/100.0, per2/100.0), 1);
}


class PieChart : public LineCollection
{
public:
  PieChart(Point center, float start_angle, float end_angle, float radius, int numsteps) : center(center), start_angle(start_angle), end_angle(end_angle), radius(radius), numsteps(numsteps) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  virtual void Prepare() { 
    Point p1 = center + Vector(radius*cos(start_angle), radius*sin(start_angle), 0.0);
    Point p2 = center + Vector(radius*cos(end_angle), radius*sin(end_angle), 0.0);
    vec.clear();
    vec.push_back(p2);
    vec.push_back(center);
    vec.push_back(p1);
    for(int i=0;i<numsteps+1;i++) {
      float angle = float(i)/float(numsteps)*(end_angle-start_angle) + start_angle;
      Point PX = center + Vector(radius*cos(angle), radius*sin(angle), 0.0);
      vec.push_back(PX);
    }
  }
  virtual int NumLines() const { return vec.size(); }
  virtual Point LinePoint(int line, int point) const {
    int pos = line + point;
    int size = vec.size();
    pos = pos % (size-1);
    return vec[pos];
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

private:
  Point center;
  float start_angle, end_angle;
  float radius;
  int numsteps;
  std::vector<Point> vec;
};

GameApi::LI GameApi::PolygonApi::li_piechart(float c_x, float c_y, float c_z, float start_angle, float end_angle, float radius, int numsteps)
{
  return add_line_array(e, new PieChart(Point(c_x, c_y, c_z), start_angle, end_angle, radius, numsteps));
}

GameApi::P GameApi::PolygonApi::p_piechart(EveryApi &ev, float c_x, float c_y, float start_angle, float end_angle, float radius, int numsteps, float start_z, float end_z)
{
  LI li1 = li_piechart(c_x,c_y,start_z, start_angle, end_angle, radius, numsteps);
  LI li2 = li_piechart(c_x,c_y,end_z, start_angle, end_angle, radius, numsteps);

  PTS pts1 = ev.points_api.li_pts2(li1);
  PTS pts2 = ev.points_api.li_pts2(li2);
  
  P p1 = ev.polygon_api.polygon3(pts1);
  P p2 = ev.polygon_api.polygon3(pts2);

  PT pt1 = ev.point_api.point(0.0,0.0,0.0);
  PT pt2 = ev.point_api.point(0.0,0.0,end_z-start_z);

  LI span = ev.lines_api.point_array(std::vector<PT>{pt1,pt2});
  
  P p_span = ev.lines_api.line_product(li1, span);
  
  P res = ev.polygon_api.or_array2(std::vector<P>{p1,p_span,p2});
  return res;
}

unsigned int digit_to_num(char c)
{
  std::string s="0123456789abcdef";
  std::string s2="0123456789ABCDEF";

  int ss = s.size();
  for(int i=0;i<ss;i++) {
    char lower = s[i];
    char upper = s2[i];
    if (c==lower ||c==upper) { return i; }
  }
  return 0x00;
}

unsigned int str_to_color(std::string s)
{
  if (s.size()>2) {
    if (s[0]=='0') {
      s=s.substr(1);
    }
    if (s[0]=='x' ||s[0]=='X')
      {
	s=s.substr(1);
      }
  }
  if (s.size()==6) {
    unsigned int val2 = digit_to_num(s[0]);
    unsigned int val3 = digit_to_num(s[1]);
    unsigned int val4 = digit_to_num(s[2]);
    unsigned int val5 = digit_to_num(s[3]);
    unsigned int val6 = digit_to_num(s[4]);
    unsigned int val7 = digit_to_num(s[5]);
    val6<<=4;
    val5<<=8;
    val4<<=12;
    val3<<=16;
    val2<<=20;
    return 0xff000000+val2+val3+val4+val5+val6+val7;

  }
  if (s.size()==8) {
    unsigned int val0 = digit_to_num(s[0]);
    unsigned int val1 = digit_to_num(s[1]);
    unsigned int val2 = digit_to_num(s[2]);
    unsigned int val3 = digit_to_num(s[3]);
    unsigned int val4 = digit_to_num(s[4]);
    unsigned int val5 = digit_to_num(s[5]);
    unsigned int val6 = digit_to_num(s[6]);
    unsigned int val7 = digit_to_num(s[7]);
    val6<<=4;
    val5<<=8;
    val4<<=12;
    val3<<=16;
    val2<<=20;
    val1<<=24;
    val0<<=28;
    return val0+val1+val2+val3+val4+val5+val6+val7;
  }
  return 0xff000000;
}

class PieChart_full : public MainLoopItem
{
public:
  PieChart_full(GameApi::Env &env, GameApi::EveryApi &ev, float c_x, float c_y, std::string url, std::string homepage, float radius, int numsteps, float start_z, float end_z) : env(env), ev(ev), c_x(c_x), c_y(c_y), url(url), homepage(homepage), radius(radius), numsteps(numsteps), start_z(start_z), end_z(end_z) 
  {
    result.id = -1;
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = env.get_loaded_async_url(url);
    if (!vec) return;
    std::string str(vec->begin(),vec->end());
    std::stringstream ss(str);
    float val = 0.0;
    std::string col = "ffffff";
    data.clear();
    while(ss>>val>>col) {
      data.push_back(val);
      color.push_back(str_to_color(col));
    }

    render_vec.clear();
    int s = data.size();
    float angle = 0.0;
    for(int i=0;i<s;i++) {
      float val = data[i];
      GameApi::P pie = ev.polygon_api.p_piechart(ev, c_x, c_y, angle, angle+val*2.0*3.14159, radius, numsteps, start_z, end_z);

      GameApi::MT I2=ev.materials_api.m_def(ev);
      GameApi::MT I3=ev.materials_api.phong(ev,I2,-0.3,0.3,-1.0,color[i],0xff666666,5.0);
      //GameApi::MT I4=ev.materials_api.bevel(ev,I3,1.2,2);
      GameApi::ML I5=ev.materials_api.bind(pie,I3);
      render_vec.push_back(I5);
      angle+=val*2.0*3.14159;
    }   
    result=ev.mainloop_api.array_ml(ev,render_vec);

    MainLoopItem *item = find_main_loop(env, result);
    if (item)
      item->Prepare();
  }

  virtual void execute(MainLoopEnv &e)
  {
    if (result.id!=-1) {
    MainLoopItem *item = find_main_loop(env, result);
    if (item)
      item->execute(e); 
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (result.id!=-1) {
    MainLoopItem *item = find_main_loop(env, result);
    if (item)
      item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    if (result.id!=-1) {
    MainLoopItem *item = find_main_loop(env, result);
    if (item) 
      return item->shader_id();
    else
      return std::vector<int>();
    } else { return std::vector<int>(); }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  float c_x, c_y;
  std::string url, homepage;
  float radius;
  int numsteps;
  float start_z, end_z;
  std::vector<float> data;
  std::vector<unsigned int> color;
  std::vector<GameApi::ML> render_vec;
  GameApi::ML result;
};
GameApi::ML GameApi::PolygonApi::piechart_full(GameApi::EveryApi &ev, float c_x, float c_y, std::string url, float radius, int numsteps, float start_z, float end_z)
{
  return add_main_loop(e, new PieChart_full(e, ev, c_x, c_y, url, gameapi_homepageurl, radius, numsteps, start_z, end_z));
}
