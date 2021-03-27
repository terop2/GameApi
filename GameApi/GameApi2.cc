#include "GameApi_h.hh"

#if 0
void subdiv_cube(SubDivide &d, std::string id,
		 float left, float right,
		 float top, float bottom,
		 float forward, float backward)
{
  std::string str_left = id + ".left";
  std::string str_right = id + ".right";
  std::string str_top = id + ".top";
  std::string str_bottom = id + ".bottom";
  std::string str_forward = id + ".forward";
  std::string str_backward = id + ".backward";
  
  int id_left = d.id(str_left);
  int id_right = d.id(str_right);
  int id_top = d.id(str_top);
  int id_bottom = d.id(str_bottom);
  int id_forward = d.id(str_forward);
  int id_backward = d.id(str_backward);

  int id_base = d.id(id);
  
  d.set_delta(id_left, id_base, Vector(-left,0.0,0.0));
  d.set_delta(id_right, id_base, Vector(right,0.0,0.0));
  d.set_delta(id_top, id_base, Vector(0.0,-top,0.0));
  d.set_delta(id_bottom, id_base, Vector(0.0,bottom,0.0));
  d.set_delta(id_forward, id_base, Vector(0.0,0.0,-forward));
  d.set_delta(id_backward, id_base, Vector(0.0,0.0,backward));
}
#endif
