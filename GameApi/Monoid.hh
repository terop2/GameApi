
template<class T>
class Monoid2
{
public:
  virtual T empty() const=0;
  virtual T compose(T a, T b) const=0;
  virtual T concat(const Array<int, T> &arr) const
  {
    T t = empty();
    int size = arr.Size();
    for(int i=0;i<size;i++)
      {
	t = compose(t,arr.Index(i));
      }
    return t;
  }
};


class StringConcat : public Monoid2<std::string>
{
public:
  std::string Empty() { return ""; }
  std::string Compose(std::string a, std::string b) const { return a+b; }
};

template<class T>
class Group : public Monoid2<T>
{
public:
  virtual T Inv(T t) const=0;
};

template<class T>
class NumPlus : public Group<T>
{
public:
  T Empty() const { return 0; }
  T Compose(T a, T b) const { return a+b; }
  T Inv(T t) const { return -t; }
};
template<class T>
class NumMul : public Group<T>
{
public:
  T Empty() const { return 1; }
  T Compose(T a, T b) const { return a*b; }
  T Inv(T t) const { return 1/t; }
};

template<class M, class M2>
class MonoidMorphism
{
public:
  virtual M2 Empty() const=0;
  virtual M2 Compose(M a, M b) const=0;
  virtual M2 Convert(M a) const=0;
};

template<class M, class M2>
class MonoidMorphismImpl : public MonoidMorphism<M, M2>
{
public:
  MonoidMorphismImpl(Monoid2<M> &m1, Monoid2<M2> &m2) : m1(m1), m2(m2)
  {
  }
  virtual M2 Empty() const
  {
    return Convert(m1.empty());
  }
  virtual M2 Compose(M a, M b) const
  {
    return Convert(m1.Compose(a,b));
    //return m2.Compose(Convert(a), Convert(b));
  }
  virtual M2 Convert(M a) const=0;
private:
  Monoid2<M> &m1;
  Monoid2<M2> &m2;
};
template<class T>
class PlusToMul : public MonoidMorphismImpl<T,T>
{
  T Convert(T a) const
  {
    
  }
};

template<class I, class T>
class AltFunction
{
public:
  virtual T IndexA(I i) const=0;
  virtual T IndexB(I i) const=0;
};
template<class T>
class AltObject
{
public:
  virtual T ObjectA() const=0;
  virtual T ObjectB() const=0;
};


template<class A, class B>
class HomSet : public Array<int, Function<A,B> *>
{
public:
};

template<class A, class B, class C>
class ComposeHomSet : public HomSet<A,C>
{
public:
  ComposeHomSet(HomSet<A,B> &a, HomSet<B,C> &b) : a(a), b(b) { }
  
  int Count() const 
  {
    int s = a.Count();
    s *= b.Count();
    return s;
  }
  Function<A,C> *Index(int i) const
  {
    delete func;
    int xindex = i / a.Count();
    int yindex = i - xindex * a.Count();
    Function<A,B> *f1 = a.Index(xindex);
    Function<B,C> *f2 = b.Index(yindex);
    func = new FunctionCompose<A,B,C>(*f1, *f2);
    return func;
  }
private:
  HomSet<A,B> &a;
  HomSet<B,C> &b;
  FunctionCompose<A,B,C> *func;
};
