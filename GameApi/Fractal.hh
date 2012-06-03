
//{x,y,z}n = rn{cos(n\u03b8)cos(n\u03c6),sin(n\u03b8)cos(n\u03c6),-sin(n\u03c6)}
//r=sqrt(x2+y2+z2), \u03b8=atan(y/x), \u03c6=atan(z/sqrt(x2+y2))

//http://www.bugman123.com/Hypercomplex/index.html

#include "VolumeObjects.hh"

class FractalVolume : public VolumeObject
{
public:
  bool Inside(Point p) const;
};
