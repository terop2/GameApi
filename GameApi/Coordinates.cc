
#include "Coordinates.hh"

Point PlaneSurface::Transform(Point2d p) const
{
  PlaneIn3d plane(p, pl);
  return plane.Elem();
}

bool TransformEffect::Frame(float time)
{
  UnitPlane pl(10,10);
  Point2d pp = { -0.5,-0.5 }; 
  Translate2d trans(pp);
  TransformPlane pl2(pl, trans);
  FromPolarCoordinates coord;
  TransformPlane pl3(pl2, coord);
  Plane plan(Point(0.0,0.0,0.0), Vector(400.0,0.0,0.0), Vector(0.0,400.0,0.0));
  PlaneSurface planesurf(plan);
  GridTo3d grid(pl3, planesurf);
  SampleGrid grid2(grid, 0);
  RenderOpenGl(grid2);
  return false;
}
