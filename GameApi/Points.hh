
#include "VectorTools.hh"


struct CoordPoints
{
  Point center;
  Vector u_x;
  Vector u_y;
  Vector u_z;
};

struct CubePoints
{
  Point tlf;
  Point trf;
  Point blf;
  Point brf;
  Point tlb;
  Point trb;
  Point blb;
  Point brb;
};

struct SpherePoints
{
  Point center;
  float radius;
};

struct CylinderPoints
{
  Point line_1;
  Point line_2;
  float radius;
};
struct ConePoints
{
  Point line_1;
  Point line_2;
  float radius_1;
  float radius_2;
};
struct LinePoints
{
  Point line_1;
  Point line_2;
};

struct RectanglePoints
{
  Point tl;
  Point tr;
  Point bl;
  Point br;
};




//
// 2d shapes
//
struct CirclePoints
{
  Point2d center;
  float radius;
};
struct RectanglePoints2d
{
  Point2d tl;
  Point2d tr;
  Point2d bl;
  Point2d br;
};

//
// Functions
//

CubePoints coord_cube(const CoordPoints &p)
{
  CubePoints p;
  p.tlf = p.center + -1.0*p.u_x + -1.0*p.u_y + -1.0*p.u_z;
  p.trf = p.center + 1.0*p.u_x + -1.0*p.u_y + -1.0*p.u_z;
  p.blf = p.center + -1.0*p.u_x + 1.0*p.u_y + -1.0*p.u_z;
  p.brf = p.center + 1.0*p.u_x + 1.0*p.u_y + -1.0*p.u_z;
  p.tlb = p.center + -1.0*p.u_x + -1.0*p.u_y + 1.0*p.u_z;
  p.trb = p.center + 1.0*p.u_x + -1.0*p.u_y + 1.0*p.u_z;
  p.blb = p.center + -1.0*p.u_x + 1.0*p.u_y + 1.0*p.u_z;
  p.brb = p.center + 1.0*p.u_x + 1.0*p.u_y + 1.0*p.u_z;
  return p;
}
