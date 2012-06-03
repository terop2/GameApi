

class Boundary1d
{
public:
  virtual void Get(std::vector<float> &pos)=0;
};

class Boundary2d
{
public:
  virtual void Get(int i, std::vector<Point2d> &curve)=0;
};

struct Poly
{
  Point p1,p2,p3,p4;
};

class Boundary3d
{
public:
  virtual void Get(int x, int y, Poly &face)=0;
};
