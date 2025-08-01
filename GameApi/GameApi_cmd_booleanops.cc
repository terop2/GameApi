
#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
 
std::vector<GameApiItem*> booleanopsapi_functions()
{
  std::vector<GameApiItem*> vec;
#ifndef STABLE
#if (ALL==1)||(CREATE_BO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::create_bo,
			 "create_bo", 
			 { "mesh", "bools", "fd" },
			 { "P", "O", "FD" },
			 { "", "", "" },
			 "BO", "bool_api", "create_bo"));
#endif

#if (ALL==1)||(CUBE_BO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::cube_bo,
			 "cube_bo", 
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "split_x", "split_y" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "ev", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "18", "18" },
			 "BO", "bool_api", "cube_bo"));
#endif
#if (ALL==1)||(SPHERE_BO==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::sphere_bo,
			 "sphere_bo", 
			 { "ev", "center", "radius", "numfaces1", "numfaces2" },
			 { "EveryApi&", "PT", "float", "int", "int" },
			 { "ev", "", "100.0", "30", "30" },
			 "BO", "bool_api", "sphere_bo"));
#endif
#if (ALL==1)||(OR_ELEM_BO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::or_elem_bo,
			 "or_elem_bo", 
			 { "ev", "obj", "obj2" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "or_elem_bo"));
#endif
#if (ALL==1)||(AND_NOT_BO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::and_not_bo,
			 "and_not_bo", 
			 { "ev", "obj", "not_obj" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "and_not_bo"));
#endif
#if (ALL==1)||(INTERSECT_BO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::intersect_bo,
			 "intersect_bo", 
			 { "ev", "obj", "obj2" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "intersect_bo"));
#endif
#if (ALL==1)||(TO_POLYGON==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_polygon,
			 "to_poly", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "P", "bool_api", "to_polygon"));
#endif
#if (ALL==1)||(TO_VOLUME==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_volume,
			 "to_volume", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "O", "bool_api", "to_volume"));
#endif
#if (ALL==1)||(TO_DIST==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_dist,
			 "to_dist", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "FD", "bool_api", "to_dist"));
#endif
  
#endif
  
  return vec;
}
