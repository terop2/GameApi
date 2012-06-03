

class Logo
{
public:
  virtual void Advance(float length)=0;
  virtual void Rotate(float angle)=0; // [0..1]
  virtual void Dot()=0;
};

class LogoImpl : public Logo
{
public:
  LogoImpl() : m_pos(Point(0.0,0.0,0.0)), m_angle(0.0) { }
  virtual void Advance(float length)
  {
    m_pos += length*(Vector(length,0,0)*Matrix::ZRotation(angle*2.0*3.14159));
  }
  virtual void Rotate(float angle) // [0..1]
  {
    m_angle += angle;
  }
  virtual void Dot()
  {
    vec.push_back(m_pos);
  }
  std::vector<Point> Array() const { return vec; }
private:
  Point m_pos;
  float m_angle;
  std::vector<Point> vec;
};

class TriRecursion
{
public:
  TriRecursion(Logo &l) : l(l) { }
  void Recurse(float length, int level)
  {
    if (level==0) { l.Dot(); l.Advance(length); l.Dot(); return; }
    length/=3;
    Recurse(length, level-1);
    l.Rotate(1.0/8.0);
    Recurse(length, level-1);
    l.Rotate(-2.0/8.0);
    Recurse(length, level-1);
    l.Rotate(1.0/8.0);
    Recurse(length, level-1);
  }
private:
  Logo &l;
};
