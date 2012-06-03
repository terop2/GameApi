
#include "Physics.hh"

Vector SplitToComponent(Vector vec, Vector base)
{
  base /= base.Dist();
  float val = Vector::DotProduct(vec, base);
  vec *= val;
  return vec;
}

Point2d SolveWithFailure(Function<Point2d, float> &func, Point2d tl, Point2d br, bool &b)
{
  //Point2dFunction pf(func); // Point2d->float
  SurfacePlaneIntersection sect(func, 0.0);
  Point2d center = { float((tl.x+br.x)/2.0), float((tl.y+br.y)/2.0) };
  Point2d pos = sect.Loop(center, tl,br, b);
  b = !b;
  return pos;
}

Point SolveWithFailure(const Function<Point, float> &func, Point tl, Point br, bool &error)
{
  //Point2dFunction pf(func); // Point2d->float
  WorldWorldIntersection sect(const_cast<Function<Point,float>&>(func), 0.0);
  Point center((tl.x+br.x)/2.0, (tl.y+br.y)/2.0, (tl.z+br.z)/2.0);
  Point pos = sect.Loop(center, tl,br, error);
  //std::cout << "Found: " << pos << ":" << error << std::endl;
  error = !error;
  return pos;
}
