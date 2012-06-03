
#include "GameApi.hh"

class Renderer
{
public:
  virtual void Tri(int num, Point p1, Point p2, Point p3)=0;
  virtual void Quad(int num, Point p1, Point p2, Point p3, Point p4)=0;
};

class IPoly
{
public:
  virtual P Poly() const=0;
};

class GameApiRenderer : public Renderer, public IPoly
{
public:
  GameApiRenderer(Space &sp, Polygon &poly, SP spx) : sp(sp), poly(poly), spx(spx) { }
  virtual void Tri(int num, Point p1, Point p2, Point p3)
  {
    PT point1 = sp.pos(spx, p1.x,p1.y, p1.z);
    PT point2 = sp.pos(spx, p2.x,p2.y,p2.z);
    PT point3 = sp.pos(spx, p3.x,p3.y,p3.z);
    polys.push_back(poly.triangle(point1, point2, point3));
  }
  virtual void Quad(int num, Point p1, Point p2, Point p3, Point p4)
  {
    PT point1 = sp.pos(spx, p1.x,p1.y, p1.z);
    PT point2 = sp.pos(spx, p2.x,p2.y,p2.z);
    PT point3 = sp.pos(spx, p3.x,p3.y,p3.z);
    PT point4 = sp.pos(spx, p4.x,p4.y,p4.z);
    polys.push_back(poly.quad(point1, point2, point3, point4));
  }
  P Poly()
  {
    return poly.or_array(&polys[0], polys.size());
  }
private:
  Space &sp;
  Polygon &poly;
  std::vector<P> polys;
  SP spx;
};

