
#include "DistanceObject.hh"

Point find_pos(const DistanceObject &o, float val, Ray &r, float raydist)
{
  assert(o.NumDistances()==1);
  DistanceObjectFunction f(o, val, r);
  float p = Solve(f, 0.0, raydist);
  Point pp = r.Index(p);
  return pp;
};

Color find_color(const DistanceObject &o, float val, Ray &r, float raydist)
{
  Point p = find_pos(o, val, r, raydist);
  int idx = o.Idx(p);
  return o.ColorValue(idx, p);
}

#if 0
bool operator<(const SeparateRender::Key &k1, const SeparateRender::Key &k2)
{
  return k1.kx+k1.ky<k2.kx+k2.ky;
}
#endif
