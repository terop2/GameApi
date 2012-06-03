
struct PointData
{
  float x,y,z;
};
template<class T>
struct LineData
{
  T start;
  T end;
};
template<class T>
struct PolyData
{
  std::vector<T> vec;
};
template<class T>
class QuadData
{
  T p1,p2,p3,p4;
};
struct TPointData
{
  float x,y,z,t;
};
struct TAngleData
{
  float angle, t;
};

//
// Functions
//
bool angle_time(const TAngleData &t1, const TAngleData &t2, float time,
		float &angle)
{
  int mi = std::min(t1.t, t2.t);
  int ma = std::max(t1.t, t2.t);
  if (time < mi) return false;
  if (time > ma) return false;
  time -= mi;
  time /= ma-mi;
  if (t1.t < t2.t)
    {
      float ami = t1.angle;
      float ama = t2.angle;
      float a = ami*(1.0-time)+ama*time;
      angle = a;
      return true;
    }
  else
    {
      float ami = t2.angle;
      float ama = t1.angle;
      float a = ami*(1.0-time)+ama*time;
      angle = a;
      return true;
    }

}
bool point_time(const TPointData &t1, const TPointData &t2, float time, 
		PointData &res)
{
  int mi = std::min(t1.t, t2.t);
  int ma = std::max(t1.t, t2.t);
  if (time < mi) return false;
  if (time > ma) return false;
  time -= mi;
  time /= ma-mi;
  if (t1.t < t2.t)
    {
      Point pmi(t1.x,t1.y,t1.z);
      Point pma(t2.x,t2.y,t2.z);
      Vector r = Vector(pmi)*(1.0-time)+Vector(pma)*time;
      res.x = r.dx;
      res.y = r.dy;
      res.z = r.dz;
      return true;
    }
  else
    {
      Point pmi(t2.x,t2.y,t2.z);
      Point pma(t1.x,t1.y,t1.z);
      Vector r = Vector(pmi)*(1.0-time)+Vector(pma)*time;
      res.x = r.dx;
      res.y = r.dy;
      res.z = r.dz;
      return true;
    }
}

class EnemyMovement : public Array<int, Point>
{
public:
  void push_back(const TPointData &t1, const TPointData &t2, int id)
  {
    v1.push_back(t1);
    v2.push_back(t2);
    vid.push_back(id);
  }

  int Size() const { return v1.size(); }
  Point Index(int i) const
  {
    PointData pd;
    bool b = point_time(v1[i], v2[i], time, pd);
    if (b) { return Point(pd.x,pd.y,pd.z); }
    return Point(0.0,0.0,0.0);
  }
  bool IsValid(int i) const
  {
    PointData pd;
    bool b = point_time(v1[i], v2[i], time, pd);
    return b;
  }
  int Id(int i) const
  {
    return vid[i];
  }
  void set_time(float t) { time = t; }
private:
  std::vector<TPointData> v1;
  std::vector<TPointData> v2;
  std::vector<int> vid;
  float time;
};
