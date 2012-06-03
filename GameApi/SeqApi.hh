
enum SeqApiType
  {
    EQuad,
    ECube,
    ESphere,
    ECone,
    ERing
  };
enum SeqApiTimeType
  {
    ELine,
    ECurve,
  };

class SeqObj;
class TimeObj;

class SeqApi
{
public:
  // returns point
  int push_point() { points.push_back(Point()); return points.size()-1; }
  // EQuad, ECube, ESphere, ECone, ERing, returns object
  int push_obj(int seq_api_type);
  // EQuad => 4 points
  // ECube => 8 points
  // ESphere => 2 points (center + radius)
  // ECone => 2 points for line + 2 points for circles
  // ERing => 1 point for center + 1 point for radius1, 1 point for 2nd radius
  void link_obj_point(int object, int point, int obj_point);
  // time_type={ELine,ECurve}, returns time_obj
  int push_time_obj(int time_type, float start_time, float end_time);
  int link_time_point(int time_obj, float time, int point, Point pos);
  int link_timeobj_and_obj(int time_obj, int object);
  P frame(int time) const;
private:
  std::vector<SeqObj*> *Objects() { return &objects; }
  std::vector<Point> *Points() { return &points; }
private:
  std::vector<Point> points; // points in 3d and 2d space
  std::vector<SeqObj*> objects; // displayable objects
  std::vector<TimeObj*> timeline; // timed objects
  friend class SeqObj;
  friend class TimeObj;
};

class SeqObj
{
public:
  void init();
  void set_point(int num, int idx) { points[num]=idx; }
  int num_points() const { return points.size(); }
  virtual int NumPoints() const=0;
  virtual P draw(const std::vector<Point> &points) const=0;
protected:
  std::vector<int> points;
};

class TimeObj
{
public:
  TimeObj(SeqApi &api, float start_time, float duration) : api(api), start_time(start_time), duration(duration) { }
  void push_object(int idx) { objects.push_back(idx); }
  void set_point(int point, float time, Point pos)
  {
    point_idx.push_back(point);
    points.push_back(pos);
    point_times.push_back(time);
  }
  bool decide_visibility(float time) const;
  virtual P frame(float time) const;
protected:
  virtual void set_points(float time, std::vector<Point> &points) const=0;
private:
  void generate_objects(float time);
protected:
  SeqApi &api;
  float start_time;
  float duration;
  std::vector<int> objects;
  std::vector<P> objects_in_current_frame;
  std::vector<int> point_idx;
  std::vector<Point> points;
  std::vector<float> point_times;
};

//
// Objects
//
class QuadSeqObj : public SeqObj
{
public:
  QuadSeqObj(GameApi::Env &e) : poly(e) { }
  virtual int NumPoints() const { return 4; }
  virtual P draw(const std::vector<Point> &points_list) const
  {
    int p1 = points[0];
    int p2 = points[1];
    int p3 = points[2];
    int p4 = points[3];
    Point pp1 = points_list[p1];
    Point pp2 = points_list[p2];
    Point pp3 = points_list[p3];
    Point pp4 = points_list[p4];
    PT pt1 = point.point(pp1.x,pp1.y,pp1.z);
    PT pt2 = point.point(pp2.x,pp2.y,pp2.z);
    PT pt3 = point.point(pp3.x,pp3.y,pp3.z);
    PT pt4 = point.point(pp4.x,pp4.y,pp4.z);
    return poly.quad(pt1,pt2,pt3,pt4);
  }
private:
  GameApi::Polygon poly;
  GameApi::Point point;
};
