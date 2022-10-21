#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> polygonapi_functions1();
std::vector<GameApiItem*> polygonapi_functions2();

std::vector<GameApiItem*> polygonapi_functions()
{
  std::vector<GameApiItem*> i1 = polygonapi_functions1();
  std::vector<GameApiItem*> i2 = polygonapi_functions2();
  std::copy(i2.begin(),i2.end(),std::back_inserter(i1));
  return i1;
}
std::vector<GameApiItem*> polygonapi_functions1()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_empty,
			 "empty",
			 { },
			 { },
			 { },
			 "P", "polygon_api", "p_empty"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::edit_3d,
			 "p_edit",
			 { "ev", "p", "radius" },
			 { "EveryApi&", "P", "float" },
			 { "ev", "", "30.0" },
			 "ML", "mainloop_api", "edit_3d"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::edit_3d_p,
			 "p_edit2",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "P", "mainloop_api", "edit_3d_p"));
#endif
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_model,
			 "load_model",
			 { "filename", "obj_num" },
			 { "std::string", "int" },
			 { "test.obj", "0" },
			 "P", "polygon_api", "load_model"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_model_all,
			 "load_model_all",
			 { "filename", "count" },
			 { "std::string", "int" },
			 { "test.obj", "30" },
			 "P", "polygon_api", "load_model_all"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::save_model_ml,
			 "save_model",
			 { "poly", "filename" },
			 { "P", "std::string" },
			 { "", "test.obj" },
			 "ML", "polygon_api", "save_model_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::save_ds_ml,
			 "save_ds",
			 { "ev", "out_filename", "poly", "disable_normal", "disable_color", "disable_texcoord", "disable_texcoord3", "disable_objects" },
			 { "EveryApi&", "std::string", "P","bool", "bool", "bool", "bool", "bool" },
			 { "ev", "test.ds", "", "false", "false", "false", "false", "false" },
			 "ML", "mainloop_api", "save_ds_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url,
			 "p_url",
			 { "ev", "url", "count" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://tpgames.org/example.obj", "10" },
			 "P", "polygon_api", "p_url"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url_nr,
			 "p_url_nr",
			 { "ev", "url", "count" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://tpgames.org/example.obj", "10" },
			 "P", "polygon_api", "p_url_nr"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl,
			 "p_mtl",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int" },
			 { "ev", "http://tpgames.org/sponza/sponza.ds", "http://tpgames.org/sponza/sponza.mtl", "http://tpgames.org/sponza", "600" },
			 "P", "polygon_api", "p_mtl"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_nr,
			 "p_mtl_nr",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int" },
			 { "ev", "http://tpgames.org/sponza/sponza.ds", "http://tpgames.org/sponza/sponza.mtl", "http://tpgames.org/sponza", "600" },
			 "P", "polygon_api", "p_mtl_nr"));
  
#ifndef STABLE
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl2,
			 "p_mtl2",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count", "start_index", "end_index", "mix" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int", "int", "int", "float" },
			 { "ev", "http://tpgames.org/sponza/sponza.ds", "http://tpgames.org/sponza/sponza.mtl", "http://tpgames.org/sponza", "600", "0", "32", "1.0" },
			 "P,[P],[MT],[MT],[BM],ML", "polygon_api", "p_mtl2"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_d,
			 "p_mtl_d",
			 { "p" },
			 { "P" },
			 { "" },
			 "[BM]", "polygon_api", "p_mtl_d"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_bump,
			 "p_mtl_bump",
			 { "p" },
			 { "P" },
			 { "" },
			 "[BM]", "polygon_api", "p_mtl_bump"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::ply_faces,
			 "p_ply",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/test.ply" },
			 "P", "points_api", "ply_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url_mtl,
			 "p_url_mtl",
			 { "ev", "url", "count", "material_names" },
			 { "EveryApi&", "std::string", "int", "[std::string]" },
			 { "ev", "http://tpgames.org/example.obj", "10", "" },
			 "P", "polygon_api", "p_url_mtl"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_ds_url,
			 "p_ds_url",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/sponza.ds" },
			 "P", "polygon_api", "p_ds_url"));
#endif

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::stl_load,
			 "p_stl",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/teapot.stl" },
			 "P", "polygon_api", "stl_load"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_loadKK,
			 "gltf_load",
			 { "base_url", "url" },
			 { "std::string", "std::string" },
			 { "https://tpgames.org/", "https://tpgames.org/test.glb" },
			 "TF", "mainloop_api", "gltf_loadKK"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_load_sketchfab_zip,
			 "gltf_sketchfab_zip",
			 { "url_to_zip" },
			 { "std::string" },
			 { "https://meshpage.org/test.zip" },
			 "TF", "mainloop_api", "gltf_load_sketchfab_zip"));
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::glb_load_sketchfab_zip,
			 "glb_sketchfab_zip",
			 { "url_to_zip" },
			 { "std::string" },
			 { "https://meshpage.org/test.zip" },
			 "TF", "mainloop_api", "glb_load_sketchfab_zip"));
  */
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_load,
			 "p_gltf",
			 { "ev", "tf", "mesh_index", "prim_index" },
			 { "EveryApi&", "TF", "int", "int" },
			 { "ev", "", "0", "0" },
			 "P", "polygon_api", "gltf_load"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_load_nr,
			 "p_gltf_nr",
			 { "ev", "tf", "mesh_index", "prim_index" },
			 { "EveryApi&", "TF", "int", "int" },
			 { "ev", "", "0", "0" },
			 "P", "polygon_api", "gltf_load_nr"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene_p,
			 "p_gltf_scene",
			 { "ev", "base_url", "url", "scene_id" },
			 { "EveryApi&", "std::string", "std::string", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0" },
			 "P", "polygon_api", "gltf_scene_p"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::alt,
			 "p_alt",
			 { "vec", "index" },
			 { "[P]", "int" },
			 { "", "0" },
			 "P", "polygon_api", "alt"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::poly_array,
			 "p_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "[P]", "polygon_api", "poly_array"));

#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::triangle,
			 "triangle",
			 { "p1", "p2", "p3" },
			 { "PT", "PT", "PT" },
			 { "", "", "" },
			 "P", "polygon_api", "triangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quads_to_triangles,
			 "to_triangles",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "quads_to_triangles"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad,
			 "quad",
			 { "p1", "p2", "p3", "p4" },
			 { "PT", "PT", "PT", "PT" },
			 { "", "", "", "" },
			 "P", "polygon_api", "quad"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_x,
			 "quad_x",
			 { "x", "y1", "y2", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "100.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_y,
			 "quad_y",
			 { "x1", "x2", "y", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_z,
			 "quad_z",
			 { "x1", "x2", "y1", "y2", "z" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "P", "polygon_api", "quad_z"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fullscreen_quad,
			 "quad_fullscreen",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "P", "polygon_api", "fullscreen_quad"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bitmapsized_quad,
			 "quad_bmsize",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "P", "polygon_api", "bitmapsized_quad"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::vr_fullscreen_quad,
			 "vr_fullscreen_quad",
			 { "ev", "is_right_eye" },
			 { "EveryApi&", "bool" },
			 { "ev", "false" },
			 "P", "polygon_api", "vr_fullscreen_quad"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::polygon3,
			 "polygon",
			 { "vec" },
			 { "PTS" },
			 { "" },
			 "P", "polygon_api", "polygon3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(float,float,float,float,float,float))&GameApi::PolygonApi::cube,
			 "cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "P", "polygon_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rounded_cube,
			 "rounded_cube",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r_radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "20.0" },
			 "P", "polygon_api", "rounded_cube"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere,
			 "sphere",
			 { "center", "radius", "numfaces1", "numfaces2" },
			 { "PT", "float", "int", "int" },
			 { "(0.0,0.0,0.0)",  "100.0", "30", "30" },
			 "P", "polygon_api", "sphere"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::heightmap2,
			 "heightmap",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "FB", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-50.0", "50.0", "-300.0", "300.0" },
			 "P", "polygon_api", "heightmap2"));
  
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::plane_map,
			 "plane_map",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "start_values", "end_values", "fb", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "float", "float", "FB", "int", "int" },
			 { "-200.0", "200.0", "-200.0", "200.0", "0.0", "70.0", "0.0", "1.0", "", "256", "256" },
			 "P", "polygon_api", "plane_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere_map,
			 "sphere_map",
			 { "c_x", "c_y", "c_z", "fb", "start_radius", "end_radius", "start_values", "end_values", "sx", "sy" },
			 { "float", "float", "float", "FB", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "0.0", "0.0", "", "200.0", "250.0", "0.0", "1.0", "30", "30" },
			 "P", "polygon_api", "sphere_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map,
			 "shadow_map",
			 { "ev", "p", "p_x", "p_y", "p_z", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "512", "512" },
			 "BM", "polygon_api", "shadow_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map2,
			 "shadow_map2",
			 { "p", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "P", "float", "float", "float", "int", "int", "P" },
			 { "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BB", "polygon_api", "shadow_map2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map3,
			 "shadow_map3",
			 { "ev", "objs", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int", "P" },
			 { "ev", "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BM", "polygon_api", "shadow_map3"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::disc,
			 "disc",
			 { "ev", "numfaces", "center_x", "center_y", "center_z", "normal_x", "normal_y", "normal_z", "radius" },
			 { "EveryApi&", "int", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "30", "0.0", "0.0" ,"0.0", "0.0", "0.0", "1.0", "100.0" },
			 "P", "polygon_api", "disc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::cone,
			 "cone",
			 { "numfaces", "p1", "p2", "radius1", "radius2" },
			 { "int", "PT", "PT", "float", "float" },
			 { "30", "(0.0,0.0,0.0)", "(0.0,100.0,0.0)", "30.0", "10.0" },
			 "P", "polygon_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::torus2,
			 "torus",
			 { "ev", "numfaces1", "numfaces2", "center", "radius1", "radius2" },
			 { "EveryApi&", "int", "int", "PT", "float", "float" },
			 { "ev", "20", "20", "", "250.0", "50.0" },
			 "P", "polygon_api", "torus2"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bar_chart,
			 "bar_chart",
			 { "ev", "url", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "per" },
			 { "EveryApi&", "std::string", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "http://tpgames.org/test_bar.txt", "-300.0", "300.0", "-300.0", "300.0", "0.0", "40.0", "80.0" },
			 "P", "polygon_api", "bar_chart"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bar_chart2,
			 "bar_chart2",
			 { "ev", "url", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "per", "per2" },
			 { "EveryApi&", "std::string", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "http://tpgames.org/test_bar2.txt", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "80.0", "80.0" },
			 "P", "polygon_api", "bar_chart2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_piechart,
			 "pie_chart",
			 { "ev", "c_x", "c_y", "start_angle", "end_angle", "radius", "numsteps", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "int", "float", "float" },
			 { "ev", "0.0", "0.0", "1.0", "3.0", "100.0", "30", "0.0", "30.0" },
			 "P", "polygon_api", "p_piechart"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_elem,
			 "p_or_elem",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_array2,
			 "p_or_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "P", "polygon_api", "or_array2"));
#ifndef STABLE
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::optimize_mesh,
			 "p_optimize",
			 { "p", "max" },
			 { "P", "float" },
			 { "", "2.0" },
			 "P", "polygon_api", "optimize_mesh"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::face_cutter,
			 "p_facecutter",
			 { "p", "start", "end" },
			 { "P", "float", "float" },
			 { "", "0.0", "1.0" },
			 "P", "polygon_api", "face_cutter"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mix_mesh,
			 "p_mix",
			 { "p", "pts", "val" },
			 { "P", "PTS", "float" },
			 { "", "", "0.5" },
			 "P", "polygon_api", "mix_mesh"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::translate,
			 "translate",
			 { "orig", "dx", "dy", "dz" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatex,
			 "rotatex",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatey,
			 "rotatey",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatey"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatez,
			 "rotatez",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatez"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::scale,
			 "scale",
			 { "orig", "sx", "sy", "sz" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "scale"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::substitute,
			 "subst",
			 { "p1", "p2", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "normal" },
			 { "P", "P", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "30.0" },
			 "P", "polygon_api", "substitute"));
			
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::p_towards_normal,
			 "towards_normal",
			 { "p", "amount" },
			 { "P", "float" },
			 { "", "0.5" },
			 "P", "lines_api", "p_towards_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::slow_calc_lights,
			 "slow_calc_lights",
			 { "mesh", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "2.0", "1.0" },
			 "P", "polygon_api", "slow_calc_lights"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::combine_textures, 
			 "combine_textures",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "combine_textures"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::remove_faces,
			 "remove_faces",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "remove_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::extract_large_polygons,
			 "extract_large",
			 { "p", "minimum_size", "reverse" },
			 { "P", "float", "bool" },
			 { "", "50.0", "false" },
			 "P", "polygon_api", "extract_large_polygons"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_choose,
			 "lod_choose",
			 { "vec", "name" },
			 { "[P]", "std::string" },
			 { "", "lod" },
			 "P", "polygon_api", "lod_choose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_set,
			 "lod_set",
			 { "p", "name", "value" },
			 { "P", "std::string", "int" },
			 { "", "lod", "0" },
			 "P", "polygon_api", "lod_set"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_select,
			 "lod_select",
			 { "start_dist", "dist_step", "max_value" },
			 { "float", "float", "int" },
			 { "0.0", "300.0", "3" },
			 "IF", "polygon_api", "lod_select"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_resize,
			 "mesh_resize",
			 { "p", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "P", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "polygon_api", "mesh_resize"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fix_vertex_order,
			 "fix_vertex_order",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "fix_vertex_order"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::filter_invisible,
			 "filter_invisible",
			 { "p", "size" },
			 { "P", "float" },
			 { "", "1.0" },
			 "P", "polygon_api", "filter_invisible"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xy_p,
			 "p_repeat_xy",
			 { "ev", "p", "start_x", "start_y", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xy_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xz_p,
			 "p_repeat_xz",
			 { "ev", "p", "start_x", "start_z", "dx", "dz", "sx", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xz_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_yz_p,
			 "p_repeat_yz",
			 { "ev", "p", "start_y", "start_z", "dy", "dz", "sy", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_yz_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::deform,
			 "deform",
			 { "obj", "bools", "dx", "dy", "dz" },
			 { "P", "O", "float", "float", "float" },
			 { "", "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "deform"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::linear_span,
			 "linear_span",
			 { "ev", "li", "dx", "dy", "dz", "num_steps" },
			 { "EveryApi&", "LI", "float", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "100.0", "1" },
			 "P", "polygon_api", "linear_span"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::transparent_separate,
			 "transparency_separate",
			 { "p", "bm", "opaque" },
			 { "P", "BM", "bool" },
			 { "", "", "true" },
			 "P", "polygon_api", "transparent_separate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::transparent_separate2,
			 "transparency_separate2",
			 { "p", "bms", "opaque" },
			 { "P", "[BM]", "bool" },
			 { "", "", "true" },
			 "P", "polygon_api", "transparent_separate2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone,
			 "li_to_cone",
			 { "ev", "li", "size", "numfaces" },
			 { "EveryApi&", "LI", "float", "int" },
			 { "ev", "", "10.0", "15" },
			 "P", "polygon_api", "line_to_cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone2,
			 "li_to_cone2",
			 { "ev", "li", "size", "numfaces", "mt" },
			 { "EveryApi&", "LI", "float", "int", "MT" },
			 { "ev", "", "10.0", "15", "" },
			 "ML", "polygon_api", "line_to_cone2"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::curve_to_poly,
			 "curve_to_poly",
			 { "curve", "start_x", "end_x", "start_y", "end_y", "start_angle", "end_angle", "numinstances" },
			 { "C", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "-4.0", "4.0", "-2.0", "2.0", "1.2", "7.48318", "120" },
			 "P", "polygon_api", "curve_to_poly"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing,
			 "static_instancing",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_vertex_color,
			 "inst_vertex_color",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing_vertex_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_with_color,
			 "static_inst_color",
			 { "ev", "obj", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "P", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "P", "polygon_api", "static_instancing_with_color"));
#endif
  return vec;
}
