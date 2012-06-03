
#include "GameApi2.hh"



GameApi::P GameApi2::WorldOps::cubebitmap(char *bitmap, int sx, int sy, float ssx, float ssy, float ssz, float xgap, float ygap)
{
  GameApi::P oor = poly.empty();
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
      {
	GameApi::P cube = poly.cube(x*(ssx+xgap), (x+1)*(ssx+xgap),
				    y*(ssy+ygap), (y+1)*(ssy*ygap),
				    0.0, ssz);
	oor = poly.or_elem(oor, cube);
      }
  GameApi::P pp = poly.memoize(oor);
  return pp;
}
