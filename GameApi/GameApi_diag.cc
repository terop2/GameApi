
#include "GameApi_h.hh"

extern std::string gameapi_homepageurl;

class BarChart : public FaceCollection
{
public:
  BarChart(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string homepage, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per) : env(env), ev(ev), url(url), homepage(homepage), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), per(per) { 
    result.id = -1;
  }
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

GameApi::P GameApi::PolygonApi::bar_chart( GameApi::EveryApi &ev, std::string url, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float per )
{
#ifdef EMSCRIPTEN
  return add_polygon2(e, new BarChart(e, ev, url, gameapi_homepageurl, start_x, end_x, start_y, end_y, start_z, end_z, per/100.0), 1);
#else
  return add_polygon2(e, new BarChart(e, ev, url, gameapi_homepageurl, start_x, end_x, end_y, start_y, start_z, end_z, per/100.0), 1);
#endif
}

class PieChart : public LineCollection
{
public:
  PieChart(Point center, float start_angle, float end_angle, float radius, int numsteps) : center(center), start_angle(start_angle), end_angle(end_angle), radius(radius), numsteps(numsteps) { }
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
