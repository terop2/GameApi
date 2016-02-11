
#include "GameApi_h.hh"

EXPORT GameApi::ColorApi::ColorApi(Env &e) : e(e) { }

EXPORT GameApi::CO GameApi::ColorApi::u_color(unsigned int color)
{
  return add_color(e, 
		   (color&0xff0000) >> 16,
		   (color &0xff00) >> 8,
		   color &0xff,
		   (color&0xff000000)>>24);
}
EXPORT GameApi::CO GameApi::ColorApi::rgb_color(int r, int g, int b, int a)
{
  return add_color(e, r,g,b,a);
}
EXPORT GameApi::CO GameApi::ColorApi::rgbf_color(float r, float g, float b, float a)
{
  return add_color(e,int(r*255.0),int(g*255.0),int(b*255.0),int(a*255.0));
}
