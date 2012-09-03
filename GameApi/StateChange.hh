
#include <vector>
#include "Effect.hh"
#include "VertexArray.hh"

template<class T>
class TimeRange
{
public:
  virtual T start() const=0;
  virtual T end() const=0;
  virtual float duration() const=0;
};


template<class T>
class DefaultTimeRange : public TimeRange<T>
{
public:
  DefaultTimeRange(T t1, T t2, float dur) : t1(t1), t2(t2), dur(dur) { }
  virtual T start() const { return t1; }
  virtual T end() const { return t2; }
  virtual float duration() const { return dur; }
private:
  T t1, t2;
  float dur;
};

template<class T>
class TimeRangeArray
{
public:
  void push_back(TimeRange<T>* elem) { array.push_back(elem); }
private:
  std::vector<TimeRange<T> *> array;
};

struct SpherePos
{
  float cx,cy,cz;
  float radius;
};

struct TorusPos
{
  
};

class ITimeRangeArray
{
public:
  virtual FaceCollection *Elem(int range_id, bool start) const=0;
};


template<class T>
class Renderer : public Function<T, FaceCollection*>
{
};

class TimeRangeObjects
{
public:
  virtual FaceCollection *start() const=0;
  virtual FaceCollection *end() const=0;
  virtual float duration() const=0;
};

template<class T>
class TRO : public TimeRangeObjects
{
public:
  TRO(TimeRange<T> *tr, Renderer<T> *rend) : tr(tr), rend(rend), start_coll(0), end_coll(0) { }
  virtual FaceCollection *start() const
  {
    start_coll = rend->Index(tr->start());
    return start_coll;
  }
  virtual FaceCollection *end() const
  {
    end_coll = rend->Index(tr->end());
    return end_coll;
  }
  virtual float duration() const { return tr->duration(); }

private:
  TimeRange<T> *tr;
  Renderer<T> *rend;
  mutable FaceCollection *start_coll;
  mutable FaceCollection *end_coll;
};

class TROArray
{ // this is 2d data structure
public:
  TROArray(int paths) { vec.resize(paths); 
    for(int i=0;i<paths;i++) vec[i] = NULL;
  }
  TROArray(const TROArray &arr) : vec(arr.vec)
  {
  }
  template<class T>
  void push_back(int path, TimeRange<T> *tr, Renderer<T> *rend)
  {
    if (!vec[path]) { vec[path] = new std::vector<TimeRangeObjects*>; }
    vec[path]->push_back(new TRO<T>(tr,rend));
  }
  TROArray *copy()
  {
    return new TROArray(*this);
  }
public:
  std::vector<std::vector<TimeRangeObjects*> *> vec;
};

class VArray
{
public:
  VArray(TROArray &arr) : arr(arr) { }
  void copy2()
  {
    int s = arr.vec.size();
    for(int i=0;i<s;i++)
      {
	vec.push_back(std::vector<Pair>());
	int s2 = arr.vec[i]->size();
	float time = 0.0;
	for(int j=0;j<s2;j++)
	  {
	    FaceCollection *coll = (*(arr.vec[i]))[j]->start();
	    FaceCollection *coll2 = (*(arr.vec[i]))[j]->end();
	    float d = (*(arr.vec[i]))[j]->duration();
	    Pair p;
	    p.s1 = new VertexArraySet;
	    p.s2 = new VertexArraySet;
	    p.start_time = time;
	    p.duration = d;
	    time += d;
	    vec[i].push_back(p);
	    FaceCollectionVertexArray2 arr(*coll, *p.s1);
	    FaceCollectionVertexArray2 arr2(*coll2, *p.s2);
	    arr.copy();
	    arr2.copy();
	  }
	current_pair.push_back(0);
      }
  }
  void render(float time, Program *prog)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	int s2 = vec[i].size();
	do {
	  int pos = current_pair[i];
	  Pair *p = &(vec[i][pos]);
	  if (time > p->start_time + p->duration)
	    {
	      current_pair[i]++;
	      continue;
	    }
	  float deltatime = time - p->start_time;
	  float range = deltatime / p->duration;
	  prog->set_var("range", range);
	  RenderVertexArray2 rend(*p->s1, *p->s2);
	  rend.render(0);	
	} while(current_pair[i] < vec[i].size());
      }
  }
private:
  struct Pair
  {
    VertexArraySet *s1;
    VertexArraySet *s2;
    float start_time;
    float duration;
  };
  std::vector<std::vector<Pair> > vec;
  std::vector<int> current_pair;
  TROArray &arr;
};
