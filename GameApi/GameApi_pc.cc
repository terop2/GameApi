
#include "GameApi_h.hh"

#if 0
class BezierArray : public PointCollection
{
public:
  BezierArray() { }
  void RunRecurse(Point p1, Point p1c, Point p2, Point p2c, int count) 
  {
    if (count==0) 
      {
	vec.push_back(p1);
	vec2.push_back(p2);
	return;
      }    
    
  }
private:
  std::vector<Point> vec;
  std::vector<Point> vec2;
};



GameApi::PC GameApi::PointCollectionApi::bezier(PT *array, PT *control_array, int size, int iteration_count)
{
}
#endif

class PointCollectionFunction : public PointCollection
{
public:
  PointCollectionFunction(GameApi::Env &e, std::function<GameApi::PT (int)> f, int count) : env(e), f(f), count(count) { }
  int Size() const { return count; }
  Point Index(int i) const {
    GameApi::PT p = f(i);
    Point pp = *(find_point(env, p));
    return pp;
  }
private:
  GameApi::Env &env;
  std::function<GameApi::PT (int)> f;
  int count;
};
EXPORT GameApi::PC GameApi::PointCollectionApi::function(std::function<GameApi::PT (int)> f, int count)
{
  return add_pointcoll_array(e, new PointCollectionFunction(e, f, count));
}
