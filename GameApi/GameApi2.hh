
#include "GameApi.hh"

using namespace GameApi;

namespace GameApi2
{

  class WorldOps
  {
  public:
    WorldOps(Bitmap &bm, Polygon &poly) : bm(bm),poly(poly) { }
    P cubebitmap(char *bitmap, int sx, int sy, float ssx, float ssy, float ssz, float xgap, float ygap);
  private:
    Bitmap &bm;
    Polygon &poly;
  };



}; // GameApi2 namespace
