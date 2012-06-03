
template<class V, class T>
class StateSpace
{
public:
  virtual void set(V v)=0;
  virtual T CurrentState() const=0;
};

class InterpolateStateSpace : public StateSpace<float,Point>
{
public:
  InterpolateStateSpace(Point p1, Point p2) : p1(p1), p2(p2) { }
  void set(float val_) { val = val_; }

  Point CurrentState() const 
  {
    return p1+Vector(p2-p1)*val;
  }
private:
  Point p1,p2;
  float val;
};

template<class V, class T>
class FunctionStateSpace : public StateSpace<V,T>
{
public:
  FunctionStateSpace(Function<V,T> &func) : func(func) { }
  void set(V v) { var = v; }
  T CurrentState() const { return func.Index(var); }
private:
  Function<V,T> &func;
  V var;
};

template<class V1, class V2, class T1, class T2>
class ProductStateSpace : public StateSpace<std::pair<V1,V2>,std::pair<T1,T2> >
{
public:
  ProductStateSpace(StateSpace<V1,T1> &sp, StateSpace<V2,T2> &sp2) : sp(sp), sp2(sp2) { }
  void set(std::pair<V1,V2> p)
  {
    sp.set(p.first);
    sp2.set(p.second);
  }
  std::pair<T1,T2> CurrentState() const 
  { 
    return std::make_pair(sp.CurrentState(),sp2.CurrentState());
  }
private:
  StateSpace<V1,T> &sp;
  StateSpace<V2,T> &sp2;
};
