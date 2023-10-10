#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> polydistfield_functions()
{
  std::vector<GameApiItem*> vec;
#ifndef STABLE
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::create_pd,
			 "create_pd",
			 { "mesh", "dist_field" },
			 { "P", "SFO" },
			 { "", "" },
			 "PD", "polygon_dist_api", "create_pd"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::cube_pd,
			 "cube_pd",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float" },
			 { "ev", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "PD", "polygon_dist_api", "cube_pd"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::rounded_cube_pd,
			 "rounded_cube_pd",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "20.0" },
			 "PD", "polygon_dist_api", "rounded_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::sphere_pd,
			 "sphere_pd",
			 { "ev", "center", "radius", "numfaces1", "numfaces2" },
			 { "EveryApi&", "PT", "float", "int", "int" },
			 { "ev", "", "100.0", "30", "30" },
			 "PD", "polygon_dist_api", "sphere_pd"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::or_array_pd,
			 "or_array_pd",
			 { "ev", "vec" },
			 { "EveryApi&", "[PD]" },
			 { "ev", "" },
			 "PD", "polygon_dist_api", "or_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::color_from_normal_pd,
			 "pd_color_from_normal",
			 { "ev", "obj" },
			 { "EveryApi&", "PD" },
			 { "ev", "" },
			 "PD", "polygon_dist_api", "color_from_normal_pd"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::colormod_from_position,
			 "pd_colormod",
			 { "ev", "obj", "px", "py", "pz", "sx", "sy", "sz" },
			 { "EveryApi&", "PD", "float", "float", "float", "float", "float", "float" },
			 { "ev", "", "0.0", "0.0", "0.0", "100.0", "100.0", "100.0" },
			 "PD", "polygon_dist_api", "colormod_from_position"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::ambient_occulsion_sfo,
			 "ambient_occulsion_sfo",
			 { "ev", "obj", "d", "i" },
			 { "EveryApi&", "PD", "float", "float" },
			 { "ev", "", "10.2", "30.0" },
			 "PD", "polygon_dist_api", "ambient_occulsion_sfo"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::get_distance_field,
			 "to_sfo",
			 { "pd" },
			 { "PD" },
			 { "" },
			 "SFO", "poly_dist_api", "get_distance_field"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::get_polygon,
			 "to_p",
			 { "pd" },
			 { "PD" },
			 { "" },
			 "P", "poly_dist_api", "get_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::mesh_color_from_sfo,
			 "mesh_color_from_sfo",
			 { "ev", "pd", "mat" },
			 { "EveryApi&", "PD", "MT" },
			 { "ev", "", "" },
			 "MT", "poly_dist_api", "mesh_color_from_sfo"));
 
#endif
#endif
  return vec;
}
