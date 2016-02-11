
#include "GameApi_h.hh"

GameApi::PT GameApi::SpaceApi::pos(SP spa, float x, float y, float z)
{
  SpaceImpl *sp = find_space(e, spa);
  x /= (sp->tr - sp->tl).Dist();
  y /= (sp->bl - sp->tl).Dist();
  z /= (sp->z - sp->tl).Dist();
  Vector u_x = sp->tr - sp->tl;
  Vector u_y = sp->bl - sp->tl;
  Vector u_z = sp->z - sp->tl;
  Point p = sp->tl + x*u_x + y*u_y + z*u_z;
  return add_point(e, p.x, p.y, p.z);
}
GameApi::PT GameApi::SpaceApi::posN(SP spa, float x, float y, float z)
{
  SpaceImpl *sp = find_space(e, spa);
  Vector u_x = sp->tr - sp->tl;
  Vector u_y = sp->bl - sp->tl;
  Vector u_z = sp->z - sp->tl;
  Point p = sp->tl + x*u_x + y*u_y + z*u_z;
  return add_point(e, p.x, p.y, p.z);
}
float GameApi::SpaceApi::pt_x(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->x;
}
float GameApi::SpaceApi::pt_y(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->y;
}
float GameApi::SpaceApi::pt_z(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->z;
}
