
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
  return add_polygon2(e, new BarChart(e, ev, url, gameapi_homepageurl, start_x, end_x, end_y, start_y, start_z, end_z, per/100.0), 1);
}
