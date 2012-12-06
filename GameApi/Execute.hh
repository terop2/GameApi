
template<class T>
class Execute
{
public:
  virtual T get() const=0;
};

template<class T>
class EmptyExecute : public Execute<T>
{
public:
  EmptyExecute(T t) : t(t) { }
  virtual T get() const { return t; }
private:
  T t;
};
template<class T, class K>
class MapExecute : public Execute<K>
{
public:
  MapExecute(Execute<T> &t, K (*fptr)(T)) : t(t), fptr(fptr) { }
  virtual K get() const
  {
    return fptr(t.get());
  }  
private:
  Execute<T> &t;
  K (*fptr)(T);
};

template<class T>
class LoopExecute : public Execute<T>
{
public:
  LoopExecute(Execute<T> &e, T (*fptr)(T), int count) : e(e), fptr(fptr), count(count) { }
  T get() const
  {
    T val = e.get();
    for(int i=0;i<count;i++)
      {
	val = fptr(val);
      }
    return val;
  }
private:
  Execute<T> &e;
  T (*fptr)(T);
  int count;
}
