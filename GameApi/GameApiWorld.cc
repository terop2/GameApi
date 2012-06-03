
#include "GameApiWorld.hh"

P GameApiWorld::get() const
{
  P p = e.polygon_api.empty();
  for(int i=0;i<polys.size();i++)
    {
      p = e.polygon_api.or_elem(p, polys[i]);
    }
  return p;
}
