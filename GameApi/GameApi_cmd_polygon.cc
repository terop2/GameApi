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
#if (ALL==1)||(P_EMPTY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_empty,
			 "empty",
			 { },
			 { },
			 { },
			 "P", "polygon_api", "p_empty"));
#endif
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
#if (ALL==1)||(LOAD_DS_FROM_TEMP_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_ds_from_temp_p,
			 "p_ds_p",
			 { "ev", "p", "url" },
			 { "EveryApi&", "P", "std::string" },
			 { "ev", "", "https://meshpage.org/temp.ds" },
			 "P", "polygon_api", "load_ds_from_temp_p"));
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
#if (ALL==1)||(SAVE_MODEL_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::save_model_ml,
			 "save_model",
			 { "poly", "filename" },
			 { "P", "std::string" },
			 { "", "test.obj" },
			 "ML", "polygon_api", "save_model_ml"));
#endif
#if (ALL==1)||(SAVE_DS_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::save_ds_ml,
			 "save_ds",
			 { "ev", "out_filename", "poly", "disable_normal", "disable_color", "disable_texcoord", "disable_texcoord3", "disable_objects" },
			 { "EveryApi&", "std::string", "P","bool", "bool", "bool", "bool", "bool" },
			 { "ev", "test.ds", "", "false", "false", "false", "false", "false" },
			 "ML", "mainloop_api", "save_ds_ml"));
#endif
#if (ALL==1)||(P_URL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url,
			 "p_url",
			 { "ev", "url", "count" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://meshpage.org/assets/example.obj@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "10" },
			 "P", "polygon_api", "p_url"));
#endif
#if (ALL==1)||(P_URL_NR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url_nr,
			 "p_url_nr",
			 { "ev", "url", "count" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://meshpage.org/assets/example.obj@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "10" },
			 "P", "polygon_api", "p_url_nr"));
#endif
#if (ALL==1)||(P_MTL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl,
			 "p_mtl",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int" },
			 { "ev", "http://meshpage.org/assets/sponza/sponza.ds@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza/sponza.mtl@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza", "600" },
			 "P", "polygon_api", "p_mtl"));
#endif
#if (ALL==1)||(P_MTL_NR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_nr,
			 "p_mtl_nr",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int" },
			 { "ev", "http://meshpage.org/assets/sponza/sponza.ds@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza/sponza.mtl@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza", "600" },
			 "P", "polygon_api", "p_mtl_nr"));
#endif
  
#ifndef STABLE
#if (ALL==1)||(P_MTL2==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl2,
			 "p_mtl2",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count", "start_index", "end_index", "mix" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int", "int", "int", "float" },
			 { "ev", "http://meshpage.org/assets/sponza/sponza.ds@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza/sponza.mtl@cryengine@https://www.cryengine.com/ce-terms", "http://meshpage.org/assets/sponza", "600", "0", "32", "1.0" },
			 "P,[P],[MT],[MT],[BM],ML", "polygon_api", "p_mtl2"));
#endif
#if (ALL==1)||(P_MTL_D==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_d,
			 "p_mtl_d",
			 { "p" },
			 { "P" },
			 { "" },
			 "[BM]", "polygon_api", "p_mtl_d"));
#endif
#if (ALL==1)||(P_MTL_BUMP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_bump,
			 "p_mtl_bump",
			 { "p" },
			 { "P" },
			 { "" },
			 "[BM]", "polygon_api", "p_mtl_bump"));
#endif
#if (ALL==1)||(PLY_FACES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::ply_faces,
			 "p_ply",
			 { "url" },
			 { "std::string" },
			 { "http://meshpage.org/assets/test.ply@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "P", "points_api", "ply_faces"));
#endif
#if (ALL==1)||(P_URL_MTL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url_mtl,
			 "p_url_mtl",
			 { "ev", "url", "count", "material_names" },
			 { "EveryApi&", "std::string", "int", "[std::string]" },
			 { "ev", "http://meshpage.org/assets/example.obj@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "10", "" },
			 "P", "polygon_api", "p_url_mtl"));
#endif
#if (ALL==1)||(P_DS_URL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_ds_url,
			 "p_ds_url",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://meshpage.org/assets/sponza.ds@cryengine@https://www.cryengine.com/ce-terms" },
			 "P", "polygon_api", "p_ds_url"));
#endif
#endif
#if (ALL==1)||(STL_LOAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::stl_load,
			 "p_stl",
			 { "url" },
			 { "std::string" },
			 { "http://meshpage.org/assets/teapot.stl@FinlayMcWalter@https://creativecommons.org/share-your-work/public-domain/cc0/" },
			 "P", "polygon_api", "stl_load"));
#endif
#if (ALL==1)||(DECIMATE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::decimate,
			 "decimate",
			 { "p", "val" },
			 { "P", "float" },
			 { "", "0.5" },
			 "P", "polygon_api", "decimate"));
#endif
#if (ALL==1)||(DECIMATE2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::decimate2,
			 "decimate2",
			 { "p", "val" },
			 { "P", "float" },
			 { "", "0.5" },
			 "P", "polygon_api", "decimate2"));
#endif
#if (ALL==1)||(TF_GLB_TF==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::tf_glb_tf,
			 "tf_glb_tf",
			 { "ev", "tf", "cache_file" },
			 { "EveryApi&", "TF", "std::string" },
			 { "ev", "", "test44.glb" },
			 "TF", "polygon_api", "tf_glb_tf"));
#endif  
#if (ALL==1)||(GLTF_LOADKK==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_loadKK,
			 "gltf_load",
			 { "base_url", "url" },
			 { "std::string", "std::string" },
			 { "https://meshpage.org/assets/", "https://meshpage.org/assets/test.glb@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "TF", "mainloop_api", "gltf_loadKK"));
#endif
#if (ALL==1)||(GLTF_LOADKK2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_loadKK2,
			 "gltf_load2",
			 { "url" },
			 { "std::string" },
			 { "https://meshpage.org/assets/test.glb@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "TF", "mainloop_api", "gltf_loadKK2"));
#endif
#if (ALL==1)||(GLTF_MESH_ALL_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_p,
			 "gltf_mesh_all_p",
			 { "ev", "model" },
			 { "EveryApi&", "TF" },
			 { "ev", "" },
			 "P", "mainloop_api", "gltf_mesh_all_p"));
#endif
#if (ALL==1)||(GLTF_MESH_ALL_P_ARR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_p_arr,
			 "gltf_mesh_all_p_arr",
			 { "ev", "model", "light_dir_x", "light_dir_y", "light_dir_z", "is_transparent" },
			 { "EveryApi&", "TF", "float", "float", "float","bool" },
			 { "ev", "", "1.0", "1.0", "1.0","true" },
			 "[P]", "mainloop_api", "gltf_mesh_all_p_arr"));
#endif
#if (ALL==1)||(GLTF_LOAD_SKETCHFAB_ZIP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_load_sketchfab_zip,
			 "gltf_sketchfab_zip",
			 { "url_to_zip" },
			 { "std::string" },
			 { "https://meshpage.org/lighthouse.zip@CHL@https://creativecommons.org/licenses/by/4.0/" },
			 "TF", "mainloop_api", "gltf_load_sketchfab_zip"));
#endif
  
#if 0
  // removed for copyright reasons, see copyright office paperwork on AI:
  // https://www.copyright.gov/ai/Copyright-and-Artificial-Intelligence-Part-3-Generative-AI-Training-Report-Pre-Publication-Version.pdf
  // The parts that require removal are:
  // 1) ai training violates reproduction right
  // 2) fair use does not apply if ai output substitutes the original
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::meshy,
			 "meshy_ai",
			 { "ev", "prompt", "cache_file" },
			 { "EveryApi&", "std::string", "std::string" },
			 { "ev", "A_Monster_Truck", "monster.glb" },
			 "TF", "polygon_api", "meshy"));
#endif
  
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::glb_load_sketchfab_zip,
			 "glb_sketchfab_zip",
			 { "url_to_zip" },
			 { "std::string" },
			 { "https://meshpage.org/test.zip" },
			 "TF", "mainloop_api", "glb_load_sketchfab_zip"));
  */
#if (ALL==1)||(GLTF_LOAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_load,
			 "p_gltf",
			 { "ev", "tf", "mesh_index", "prim_index" },
			 { "EveryApi&", "TF", "int", "int" },
			 { "ev", "", "0", "0" },
			 "P", "polygon_api", "gltf_load"));
#endif
#if (ALL==1)||(GLTF_LOAD_NR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_load_nr,
			 "p_gltf_nr",
			 { "ev", "tf", "mesh_index", "prim_index" },
			 { "EveryApi&", "TF", "int", "int" },
			 { "ev", "", "0", "0" },
			 "P", "polygon_api", "gltf_load_nr"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene_p,
			 "p_gltf_scene",
			 { "ev", "base_url", "url", "scene_id" },
			 { "EveryApi&", "std::string", "std::string", "int" },
			 { "ev", "https://meshpage.org/assets/", "https://meshpage.org/assets/test.glb@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "0" },
			 "P", "polygon_api", "gltf_scene_p"));
#endif
#if (ALL==1)||(ALT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::alt,
			 "p_alt",
			 { "vec", "index" },
			 { "[P]", "int" },
			 { "", "0" },
			 "P", "polygon_api", "alt"));
#endif
#ifndef STABLE
#if (ALL==1)||(POLY_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::poly_array,
			 "p_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "[P]", "polygon_api", "poly_array"));
#endif
#endif
#if (ALL==1)||(TRIANGLE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::triangle,
			 "triangle",
			 { "p1", "p2", "p3" },
			 { "PT", "PT", "PT" },
			 { "", "", "" },
			 "P", "polygon_api", "triangle"));
#endif
#if (ALL==1)||(QUADS_TO_TRIANGLES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quads_to_triangles,
			 "to_triangles",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "quads_to_triangles"));
#endif
#if (ALL==1)||(POLYGON_FETCH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::polygon_fetch,
			 "p_fetch",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "polygon_fetch"));
#endif
#if (ALL==1)||(QUAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad,
			 "quad",
			 { "p1", "p2", "p3", "p4" },
			 { "PT", "PT", "PT", "PT" },
			 { "", "", "", "" },
			 "P", "polygon_api", "quad"));
#endif
#if (ALL==1)||(QUAD_X==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_x,
			 "quad_x",
			 { "x", "y1", "y2", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "100.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_x"));
#endif
#if (ALL==1)||(QUAD_Y==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_y,
			 "quad_y",
			 { "x1", "x2", "y", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_y"));
#endif
#if (ALL==1)||(QUAD_Z==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_z,
			 "quad_z",
			 { "x1", "x2", "y1", "y2", "z" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "P", "polygon_api", "quad_z"));
#endif
#if (ALL==1)||(FULLSCREEN_QUAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fullscreen_quad,
			 "quad_fullscreen",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "P", "polygon_api", "fullscreen_quad"));
#endif
#if (ALL==1)||(BG_IMAGE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bg_image,
			 "bg_image",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "polygon_api", "bg_image"));
#endif
#ifndef STABLE
#if (ALL==1)||(BITMAPSIZED_QUAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bitmapsized_quad,
			 "quad_bmsize",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "P", "polygon_api", "bitmapsized_quad"));
#endif
#endif
#if (ALL==1)||(VR_FULLSCREEN_QUAD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::vr_fullscreen_quad,
			 "vr_fullscreen_quad",
			 { "ev", "is_right_eye" },
			 { "EveryApi&", "bool" },
			 { "ev", "false" },
			 "P", "polygon_api", "vr_fullscreen_quad"));
#endif
#if (ALL==1)||(POLYGON3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::polygon3,
			 "polygon",
			 { "vec" },
			 { "PTS" },
			 { "" },
			 "P", "polygon_api", "polygon3"));
#endif
#if (ALL==1)||(CUBE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(float,float,float,float,float,float))&GameApi::PolygonApi::cube,
			 "cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "P", "polygon_api", "cube"));
#endif
#if (ALL==1)||(ROUNDED_CUBE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rounded_cube,
			 "rounded_cube",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r_radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "20.0" },
			 "P", "polygon_api", "rounded_cube"));
#endif
#if (ALL==1)||(SPHERE==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere,
			 "sphere",
			 { "center", "radius", "numfaces1", "numfaces2" },
			 { "PT", "float", "int", "int" },
			 { "(0.0,0.0,0.0)",  "100.0", "30", "30" },
			 "P", "polygon_api", "sphere"));
#endif
#if (ALL==1)||(HEIGHTMAP2==1) 
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::heightmap2,
			 "heightmap",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "FB", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-50.0", "50.0", "-300.0", "300.0" },
			 "P", "polygon_api", "heightmap2"));
#endif  
#ifndef STABLE
#if (ALL==1)||(PLANE_MAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::plane_map,
			 "plane_map",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "start_values", "end_values", "fb", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "float", "float", "FB", "int", "int" },
			 { "-200.0", "200.0", "-200.0", "200.0", "0.0", "70.0", "0.0", "1.0", "", "256", "256" },
			 "P", "polygon_api", "plane_map"));
#endif
#if (ALL==1)||(SPHERE_MAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere_map,
			 "sphere_map",
			 { "c_x", "c_y", "c_z", "fb", "start_radius", "end_radius", "start_values", "end_values", "sx", "sy" },
			 { "float", "float", "float", "FB", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "0.0", "0.0", "", "200.0", "250.0", "0.0", "1.0", "30", "30" },
			 "P", "polygon_api", "sphere_map"));
#endif
#if (ALL==1)||(SHADOW_MAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map,
			 "shadow_map",
			 { "ev", "p", "p_x", "p_y", "p_z", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "512", "512" },
			 "BM", "polygon_api", "shadow_map"));
#endif
#if (ALL==1)||(SHADOW_MAP2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map2,
			 "shadow_map2",
			 { "p", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "P", "float", "float", "float", "int", "int", "P" },
			 { "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BB", "polygon_api", "shadow_map2"));
#endif
#if (ALL==1)||(SHADOW_MAP3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map3,
			 "shadow_map3",
			 { "ev", "objs", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int", "P" },
			 { "ev", "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BM", "polygon_api", "shadow_map3"));
#endif
#endif
#if (ALL==1)||(DISC==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::disc,
			 "disc",
			 { "ev", "numfaces", "center_x", "center_y", "center_z", "normal_x", "normal_y", "normal_z", "radius" },
			 { "EveryApi&", "int", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "30", "0.0", "0.0" ,"0.0", "0.0", "0.0", "1.0", "100.0" },
			 "P", "polygon_api", "disc"));
#endif
#if (ALL==1)||(CONE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::cone,
			 "cone",
			 { "numfaces", "p1", "p2", "radius1", "radius2" },
			 { "int", "PT", "PT", "float", "float" },
			 { "30", "(0.0,0.0,0.0)", "(0.0,100.0,0.0)", "30.0", "10.0" },
			 "P", "polygon_api", "cone"));
#endif
#if (ALL==1)||(TORUS2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::torus2,
			 "torus",
			 { "ev", "numfaces1", "numfaces2", "center", "radius1", "radius2" },
			 { "EveryApi&", "int", "int", "PT", "float", "float" },
			 { "ev", "20", "20", "", "250.0", "50.0" },
			 "P", "polygon_api", "torus2"));
#endif
#ifndef STABLE
#if (ALL==1)||(BAR_CHART==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bar_chart,
			 "bar_chart",
			 { "ev", "url", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "per" },
			 { "EveryApi&", "std::string", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "http://meshpage.org/assets/test_bar.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "-300.0", "300.0", "-300.0", "300.0", "0.0", "40.0", "80.0" },
			 "P", "polygon_api", "bar_chart"));
#endif
#if (ALL==1)||(BAR_CHART2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bar_chart2,
			 "bar_chart2",
			 { "ev", "url", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "per", "per2" },
			 { "EveryApi&", "std::string", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "http://meshpage.org/assets/test_bar2.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "80.0", "80.0" },
			 "P", "polygon_api", "bar_chart2"));
#endif
#if (ALL==1)||(P_PIECHART==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_piechart,
			 "pie_chart",
			 { "ev", "c_x", "c_y", "start_angle", "end_angle", "radius", "numsteps", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "int", "float", "float" },
			 { "ev", "0.0", "0.0", "1.0", "3.0", "100.0", "30", "0.0", "30.0" },
			 "P", "polygon_api", "p_piechart"));
#endif
#endif
#if (ALL==1)||(OR_ELEM==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_elem,
			 "p_or_elem",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "or_elem"));
#endif
#if (ALL==1)||(OR_ARRAY2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_array2,
			 "p_or_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "P", "polygon_api", "or_array2"));
#endif
#if (ALL==1)||(OR_ARRAY3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_array3,
			 "p_or_array_nomem",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "P", "polygon_api", "or_array3"));
#endif
#ifndef STABLE
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::optimize_mesh,
			 "p_optimize",
			 { "p", "max" },
			 { "P", "float" },
			 { "", "2.0" },
			 "P", "polygon_api", "optimize_mesh"));
#endif
#if (ALL==1)||(FACE_CUTTER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::face_cutter,
			 "p_facecutter",
			 { "p", "start", "end" },
			 { "P", "float", "float" },
			 { "", "0.0", "1.0" },
			 "P", "polygon_api", "face_cutter"));
#endif
#if (ALL==1)||(MIX_MESH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mix_mesh,
			 "p_mix",
			 { "p", "pts", "val" },
			 { "P", "PTS", "float" },
			 { "", "", "0.5" },
			 "P", "polygon_api", "mix_mesh"));
#endif
#endif
#if (ALL==1)||(TRANSLATE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::translate,
			 "translate",
			 { "orig", "dx", "dy", "dz" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "translate"));
#endif
#if (ALL==1)||(ROTATEX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatex,
			 "rotatex",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatex"));
#endif
#if (ALL==1)||(ROTATEY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatey,
			 "rotatey",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatey"));
#endif
#if (ALL==1)||(ROTATEZ==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatez,
			 "rotatez",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatez"));
#endif
#if (ALL==1)||(SCALE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::scale,
			 "scale",
			 { "orig", "sx", "sy", "sz" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "scale"));
#endif
#ifndef STABLE
#if (ALL==1)||(SUBSTITUTE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::substitute,
			 "subst",
			 { "p1", "p2", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "normal" },
			 { "P", "P", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "30.0" },
			 "P", "polygon_api", "substitute"));
#endif
#if (ALL==1)||(P_TOWARDS_NORMAL==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::p_towards_normal,
			 "towards_normal",
			 { "p", "amount" },
			 { "P", "float" },
			 { "", "0.5" },
			 "P", "lines_api", "p_towards_normal"));
#endif
#if (ALL==1)||(SLOW_CALC_LIGHTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::slow_calc_lights,
			 "slow_calc_lights",
			 { "mesh", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "2.0", "1.0" },
			 "P", "polygon_api", "slow_calc_lights"));
#endif
#if (ALL==1)||(COMBINE_TEXTURES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::combine_textures, 
			 "combine_textures",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "combine_textures"));
#endif
#if (ALL==1)||(REMOVE_FACES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::remove_faces,
			 "remove_faces",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "remove_faces"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_face_collection,
			 "slow_material",
			 { "base", "material" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "material_face_collection"));
#endif
#if (ALL==1)||(EXTRACT_LARGE_POLYGONS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::extract_large_polygons,
			 "extract_large",
			 { "p", "minimum_size", "reverse" },
			 { "P", "float", "bool" },
			 { "", "50.0", "false" },
			 "P", "polygon_api", "extract_large_polygons"));
#endif
#if (ALL==1)||(LOD_CHOOSE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_choose,
			 "lod_choose",
			 { "vec", "name" },
			 { "[P]", "std::string" },
			 { "", "lod" },
			 "P", "polygon_api", "lod_choose"));
#endif
#if (ALL==1)||(LOD_SET==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_set,
			 "lod_set",
			 { "p", "name", "value" },
			 { "P", "std::string", "int" },
			 { "", "lod", "0" },
			 "P", "polygon_api", "lod_set"));
#endif
#if (ALL==1)||(LOD_SELECT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_select,
			 "lod_select",
			 { "start_dist", "dist_step", "max_value" },
			 { "float", "float", "int" },
			 { "0.0", "300.0", "3" },
			 "IF", "polygon_api", "lod_select"));
#endif
#if (ALL==1)||(MESH_RESIZE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_resize,
			 "mesh_resize",
			 { "p", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "P", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "polygon_api", "mesh_resize"));
#endif
#endif
#if (ALL==1)||(FIX_VERTEX_ORDER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fix_vertex_order,
			 "fix_vertex_order",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "fix_vertex_order"));
#endif
#ifndef STABLE
#if (ALL==1)||(FILTER_INVISIBLE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::filter_invisible,
			 "filter_invisible",
			 { "p", "size" },
			 { "P", "float" },
			 { "", "1.0" },
			 "P", "polygon_api", "filter_invisible"));
#endif
#if (ALL==1)||(REPEAT_XY_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xy_p,
			 "p_repeat_xy",
			 { "ev", "p", "start_x", "start_y", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xy_p"));
#endif
#if (ALL==1)||(REPEAT_XZ_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xz_p,
			 "p_repeat_xz",
			 { "ev", "p", "start_x", "start_z", "dx", "dz", "sx", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xz_p"));
#endif
#if (ALL==1)||(REPEAT_YZ_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_yz_p,
			 "p_repeat_yz",
			 { "ev", "p", "start_y", "start_z", "dy", "dz", "sy", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_yz_p"));
#endif
#if (ALL==1)||(DEFORM==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::deform,
			 "deform",
			 { "obj", "bools", "dx", "dy", "dz" },
			 { "P", "O", "float", "float", "float" },
			 { "", "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "deform"));
#endif
#if (ALL==1)||(LINEAR_SPAN==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::linear_span,
			 "linear_span",
			 { "ev", "li", "dx", "dy", "dz", "num_steps" },
			 { "EveryApi&", "LI", "float", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "100.0", "1" },
			 "P", "polygon_api", "linear_span"));
#endif
#endif
#if (ALL==1)||(TRANSPARENT_SEPARATE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::transparent_separate,
			 "transparency_separate",
			 { "p", "bm", "opaque", "force_transparent" },
			 { "P", "BM", "bool", "bool" },
			 { "", "", "true", "false" },
			 "P", "polygon_api", "transparent_separate"));
#endif
#if (ALL==1)||(TRANSPARENT_SEPARATE2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::transparent_separate2,
			 "transparency_separate2",
			 { "p", "bms", "opaque" },
			 { "P", "[BM]", "bool" },
			 { "", "", "true" },
			 "P", "polygon_api", "transparent_separate2"));
#endif
#if (ALL==1)||(LINE_TO_CONE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone,
			 "li_to_cone",
			 { "ev", "li", "size", "numfaces" },
			 { "EveryApi&", "LI", "float", "int" },
			 { "ev", "", "10.0", "15" },
			 "P", "polygon_api", "line_to_cone"));
#endif
#if (ALL==1)||(LINE_TO_CONE2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone2,
			 "li_to_cone2",
			 { "ev", "li", "size", "numfaces", "mt" },
			 { "EveryApi&", "LI", "float", "int", "MT" },
			 { "ev", "", "10.0", "15", "" },
			 "ML", "polygon_api", "line_to_cone2"));
#endif
#if (ALL==1)||(LINE_TO_CONE3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone3,
			 "li_to_cone3",
			 { "ev", "li", "size", "numfaces", "mt", "color" },
			 { "EveryApi&", "LI", "float", "int", "MT","unsigned int" },
			 { "ev", "", "10.0", "15", "", "ffffffff" },
			 "ML", "polygon_api", "line_to_cone3"));
#endif  
#ifndef STABLE
#if (ALL==1)||(CURVE_TO_POLY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::curve_to_poly,
			 "curve_to_poly",
			 { "curve", "start_x", "end_x", "start_y", "end_y", "start_angle", "end_angle", "numinstances" },
			 { "C", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "-4.0", "4.0", "-2.0", "2.0", "1.2", "7.48318", "120" },
			 "P", "polygon_api", "curve_to_poly"));
#endif
#endif
#if (ALL==1)||(STATIC_INSTANCING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing,
			 "static_instancing",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing"));
#endif
#ifndef STABLE
#if (ALL==1)||(STATIC_INSTANCING_VERTEX_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_vertex_color,
			 "inst_vertex_color",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing_vertex_color"));
#endif
#if (ALL==1)||(STATIC_INSTANCING_WITH_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_with_color,
			 "static_inst_color",
			 { "ev", "obj", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "P", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "P", "polygon_api", "static_instancing_with_color"));
#endif
#endif
  return vec;
}
