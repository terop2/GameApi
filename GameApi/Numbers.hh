
class Integer
{
  std::string rep;
};

class Rational
{
  Integer a;
  Integer b;
};

class IReal
{
public:
  virtual bool Inside(Rational r) const=0; // {x \in Q: x<a}
  virtual IReal *Upgrade(Rational r) const=0; // if (r<a) return r else return 0
};
struct Real
{
  IReal *real;
};
class RealCut : public IReal
{
public:
  RealCut(Real &p) : p(p) { }
  bool Inside(Rational r) const
  {
    return 
  }
private:
  Real &p;
};
