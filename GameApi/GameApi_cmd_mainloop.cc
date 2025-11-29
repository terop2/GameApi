#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> blocker_functions(GameApi::EveryApi &ev)
{

  std::vector<GameApiItem*> vec;
#if (ALL==1)||(STRING_BYTESTORE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::string_bytestore,
			 "bs_string",
			 { "s" },
			 { "std::string" },
			 { "abc" },
			 "BS", "mainloop_api", "string_bytestore"));
#endif
#if (ALL==1)||(BYTESTORE_TIMING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::bytestore_timing,
			 "bs_timing",
			 { "store", "ch", "s", "time_delta" },
			 { "BS", "int", "std::string", "float" },
			 { "", "0", "abc", "3.0" },
			 "ML", "mainloop_api", "bytestore_timing"));
#endif
#if (ALL==1)||(BYTESTORE_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::bytestore_array,
			 "bs_array",
			 { "ev", "store", "url", "time_delta" },
			 { "EveryApi&", "BS", "std::string", "float" },
			 { "ev", "", "https://meshpage.org/bs.txt", "3.0" },
			 "ML", "mainloop_api", "bytestore_array"));
#endif
#if (ALL==1)||(BYTESTORE_RENDER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::bytestore_render,
			 "bs_render",
			 { "ev", "store", "mls", "url", "delta_x", "delta_y" },
			 { "EveryApi&", "BS", "[ML]", "std::string", "float", "float" },
			 { "ev", "", "", "https://meshpage.org/bs.txt", "30.0", "30.0" },
			 "ML", "mainloop_api", "bytestore_render"));
#endif
#if (ALL==1)||(BYTESTORE_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::bytestore_pts,
			 "bs_pts",
			 { "bs", "pts", "disable_char", "enable_char", "default_is_enable" },
			 { "BS", "PTS", "std::string", "std::string", "bool" },
			 { "", "", "a", "b", "true" },
			 "PTS", "mainloop_api", "bytestore_pts"));
#endif
#if (ALL==1)||(ML_EMPTY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::ml_empty,
			 "ml_empty",
			 { },
			 { },
			 { },
			 "ML", "mainloop_api", "ml_empty")); 
#endif
#if (ALL==1)||(LOAD_ZIP2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_zip2,
			 "load_gameapi_zip2",
			 { "ev", "zip_url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "file://$(instdir)/test.zip" },
			 "HML", "mainloop_api", "load_zip2"));
#endif
#if (ALL==1)||(LOAD_ZIP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_zip,
			 "load_gameapi_zip",
			 { "ev", "zip_url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "file://$(instdir)/test.zip" },
			 "ML", "mainloop_api", "load_zip"));
#endif
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_zip_assets,
			 "load_gameapi_zip_assets",
			 { "zip_url" },
			 { "std::string" },
			 { "" },
			 "ML", "mainloop_api", "load_zip_assets"));
  */
#if (ALL==1)||(HIRES_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::hires_ml,
			 "hires_ml",
			 { "ev", "I3", "size", "numsamples", "blur_radius" },
			 { "EveryApi&", "ML", "int", "int", "float" },
			 { "ev", "", "8192", "320", "0.001" },
			 "ML", "mainloop_api", "hires_ml"));
#endif
#if (ALL==1)||(TIMING_START==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_start,
  			 "t_start",
  			 { },
  			 { },
  			 { },
  			 "TT", "mainloop_api", "timing_start"));
#endif  
#if (ALL==1)||(TIMING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing,

			 "t_time",
			 { "duration", "link", "show" },
			 { "float", "TT", "ML" },
			 { "5.0", "", "" },
			 "TT", "mainloop_api", "timing"));
#endif
#if (ALL==1)||(TIMING_EVENT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_event,
			 "t_event",
			 { "trigger", "link", "show" },
			 { "IF", "TT", "ML" },
			 { "", "", "" },
			 "TT", "mainloop_api", "timing_event"));
#endif
#if (ALL==1)||(TIMING_SWITCH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_switch,
			 "t_time_switch",
			 { "ev", "duration", "link", "show", "show2", "switch_dir" },
			 { "EveryApi&", "float", "TT", "ML", "ML", "int" },
			 { "ev", "5.0", "", "", "", "0" },
			 "TT", "mainloop_api", "timing_switch"));
#endif
#if (ALL==1)||(TIMING_EXIT==1) 
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_exit,
			 "t_exit",
			 { "link" },
			 { "TT" },
			 { "" },
			 "ML", "mainloop_api", "timing_exit"));
#endif
#if (ALL==1)||(HTML_URL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::html_url,
			 "html_url",
			 { "url" },
			 { "std::string" },
			 { "http://meshpage.org/test.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "HML", "mainloop_api", "html_url"));
#endif
#if (ALL==1)||(SAVE_SCRIPT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::save_script,
			 "save_html",
			 { "html", "filename" },
			 { "HML", "std::string" },
			 { "", "test.mp" },
			 "ML", "mainloop_api", "save_script"));
#endif
#if (ALL==1)||(LOAD_BM_SCRIPT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script,
			 "bm_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/test.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "a", "b", "c", "d", "e" },
			 "BM", "mainloop_api", "load_BM_script"));
#endif
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script,
			 "p_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/assets/blob_p.mp", "a", "b", "c", "d", "e" },
			 "P", "mainloop_api", "load_P_script"));
  */
#if (ALL==1)||(P_SCRIPT2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::p_script2,
			 "p_script",
			 { "ev", "hml", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "HML", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "", "a", "b", "c", "d", "e" },
			 "P", "mainloop_api", "p_script2"));
#endif  
#if (ALL==1)||(LOAD_ML_SCRIPT2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script2,
			 "ml_script",
			 { "ev", "hml", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "HML", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "", "a", "b", "c", "d", "e" },
			 "ML", "mainloop_api", "load_ML_script2"));
#endif
#if (ALL==1)||(LOAD_ML_SCRIPT3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script3,
			 "ml_script2",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "test.txt" },
			 "ML", "mainloop_api", "load_ML_script3"));
#endif
#if (ALL==1)||(LOAD_MN_SCRIPT2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_MN_script2,
			 "mn_script",
			 { "ev", "hml", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "HML", "std::string","std::string","std::string","std::string","std::string" },
			 { "ev", "", "a", "b", "c","d", "e" },
			 "MN", "mainloop_api", "load_MN_script2"));
#endif
#if (ALL==1)||(LOAD_MT_SCRIPT2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_MT_script2,
			 "mt_script",
			 { "ev", "hml", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "HML", "std::string","std::string","std::string","std::string","std::string" },
			 { "ev", "", "a", "b", "c", "d", "e" },
			 "MT", "mainloop_api", "load_MT_script2"));
#endif
  
#if (ALL==1)||(LOAD_BM_SCRIPT_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script_array,
			 "bm_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/assets/tiiliseina_bm.mp@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[BM]", "mainloop_api", "load_BM_script_array"));
#endif
#if (ALL==1)||(LOAD_BM_SCRIPT_ARRAY_COMB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script_array_comb,
			 "bm_script_comb",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/assets/tiiliseina_bm.mp@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "a&a", "b", "c", "d", "e" },
			 "[BM]", "mainloop_api", "load_BM_script_array_comb"));
#endif
#if (ALL==1)||(LOAD_P_SCRIPT_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script_array,
			 "p_script_arr",
			 { "ev", "hml", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "HML", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[P]", "mainloop_api", "load_P_script_array"));

#endif
#if (ALL==1)||(LOAD_ML_SCRIPT_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script_array,
			 "ml_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/assets/marble_cube_ml.mp@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[ML]", "mainloop_api", "load_ML_script_array"));
#endif
#if USE_CHAISCRIPT
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::chai_mainloop,
			 "ml_chai",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://meshpage.org/assets/chai_example.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "ML", "mainloop_api", "chai_mainloop"));
#endif

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chai_bm,
			 "chai_bm",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://meshpage.org/assets/test_bm.chai@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "100", "100" },
			 "BM", "bitmap_api", "chai_bm"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh,
			 "ml_gltf",
			 { "ev", "tf", "mesh_id", "skin_id", "keys" },
			 { "EveryApi&", "TF", "int", "int", "std::string" }, 
			 { "ev", "", "0", "0", "cvbnm" },
			 "ML", "mainloop_api", "gltf_mesh"));
#endif
#ifndef STABLE
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_node,
			 "ml_gltf_node",
			 { "ev", "tf", "node_id", "keys","mode" },
			 { "EveryApi&", "TF", "int", "std::string","int" },
			 { "ev", "", "0", "cvbnmfghjklertyuiop","0" },
			 "ML", "mainloop_api", "gltf_node"));
#endif
#if (ALL==1)||(GLTF_SCENE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene,
			 "ml_gltf_scene",
			 { "ev", "tf", "scene_id", "keys","mix","self_mult", "rest_mult","mode","light_dir_x", "light_dir_y", "light_dir_z", "animation","border_width", "border_color","is_transparent" },
			 { "EveryApi&", "TF", "int", "std::string","float","float", "float","int", "float", "float", "float", "int", "float", "unsigned int","bool" },
			 { "ev", "", "0", "cvbnmfghjklertyuiop","1.0","1.0", "1.0","0", "400.0", "-400.0", "300.0", "0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_scene"));
#endif
#endif
#if (ALL==1)||(GLTF_MESH_ALL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all,
			 "ml_gltf_all",
			 { "ev", "tf", "mix", "self_mult", "rest_mult", "mode", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color", "transparent" },
			 { "EveryApi&", "TF", "float", "float", "float", "int", "float", "float", "float", "float", "unsigned int", "bool" },
			 { "ev", "", "1.0","1.0", "1.0", "0", "400.0", "-400.0", "300.0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_mesh_all"));
#endif
#if (ALL==1)||(GLTF_MESH_ALL_INST==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_inst2,
			 "ml_gltf_all_inst",
			 { "ev", "tf", "resize_obj", "pts", "mix", "self_mult", "rest_mult", "mode", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color", "transparent" },
			 { "EveryApi&", "TF", "TF", "PTS", "float", "float", "float", "int", "float", "float", "float", "float", "unsigned int", "bool" },
			 { "ev", "", "", "", "1.0","1.0", "1.0", "0", "400.0", "-400.0", "300.0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_mesh_all_inst2"));
#endif

#if (ALL==1)||(GLTF_MESH_ALL_INST_MATRIX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_inst_matrix,
			 "ml_gltf_all_inst_m",
			 { "ev", "tf", "resize_obj", "ms", "mix", "self_mult", "rest_mult", "mode", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color", "transparent" },
			 { "EveryApi&", "TF", "TF", "MS", "float", "float", "float", "int", "float", "float", "float", "float", "unsigned int", "bool" },
			 { "ev", "", "", "", "1.0","1.0", "1.0", "0", "400.0", "-400.0", "300.0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_mesh_all_inst_matrix"));
#endif
#if (ALL==1)||(GLTF_MESH_ALL_ANIM==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_anim,
			 "ml_gltf_all_anim",
			 { "ev", "tf","mix","self_mult", "rest_mult", "mode", "keys", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color","transparent" },
			 { "EveryApi&", "TF","float","float", "float","int","std::string", "float", "float", "float", "float", "unsigned int","bool" },
			 { "ev", "", "1.0","1.0","1.0","0", "c", "400.0", "-400.0", "300.0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_mesh_all_anim"));
#endif
#if (ALL==1)||(GLTF_MESH_ALL_ENV==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_env,
			 "ml_gltf_all_env",
			 { "ev", "tf", "diffuse", "specular", "bfrd","mix" },
			 { "EveryApi&", "TF", "BM", "BM", "BM", "float" },
			 { "ev", "", "", "", "","1.0" },
			 "ML", "mainloop_api", "gltf_mesh_all_env"));
#endif
#if 0
#if (ALL==1)||(GLTF_MESH_ALL_INST==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_inst,
			 "ml_gltf_all_inst",
			 { "ev", "tf", "mix", "light_x", "light_y", "light_z", "skin_id" },
			 { "EveryApi&", "TF", "float", "float", "float", "float","int" },
			 { "ev", "", "1.0", "0.0", "0.0", "-1.0","0" },
			 "ML", "mainloop_api", "gltf_mesh_all_inst"));
#endif
#endif
  
  /*  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_anim,
			 "ml_gltf_anim",
			 { "ev", "base_url", "url", "animation", "channel", "mesh_index", "prim_index", "mat" },
			 { "EveryApi&", "std::string", "std::string", "int", "int", "int", "int", "MT" },
			 { "ev", "http://meshpage.org/assets/", "http://meshpage.org/assets/test.glb", "0", "0", "0", "0", "" },
			 "ML", "mainloop_api", "gltf_anim"));*/
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_anim4,
			 "ml_gltf_anim",
			 { "ev", "tf", "animation", "channel" },
			 { "EveryApi&", "TF", "int", "int" },
			 { "ev", "", "0", "0" },
			 "ML", "mainloop_api", "gltf_anim4"));
#endif
#ifndef STABLE
#if (ALL==1)||(GLTF_SCENE_ANIM==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene_anim,
			 "ml_gltf_sc_anim",
			 { "ev", "tf", "scene_id", "animation", "keys","mix","self_mult", "rest_mult","mode", "light_dir_x", "light_dir_y", "light_dir_z","border_width", "border_color" , "transparent"},
			 { "EveryApi&", "TF", "int", "int", "std::string","float","float", "float", "int", "float", "float", "float", "float", "unsigned int","bool" },
			 { "ev", "", "0", "0", "cvbnmfghjklertyuiop","1.0","1.0", "1.0","0", "400.0", "-400.0", "300.0", "0.0", "ff000000","true" },
			 "ML", "mainloop_api", "gltf_scene_anim"));
#endif

#ifdef NO_PREPARE_RENDER
#if (ALL==1)||(GLTF_MATERIAL_NOP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_material_nop_resize,
			 "tf_nop",
			 { "ev", "tf", "mesh_index", "prim_index", "mix" },
			 { "EveryApi&", "TF", "int", "int", "float" },
			 { "ev", "", "0","0", "0.5" },
			 "ML","mainloop_api", "gltf_material_nop"));
#endif
#if (ALL==1)||(GLTF_MATERIAL_MESH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_material_mesh,
			 "tf_material",
			 { "ev", "tf", "mesh_index", "mix" },
			 { "EveryApi&", "TF", "int", "float" },
			 { "ev", "", "0", "0.5" },
			 "ML", "mainloop_api", "gltf_material_mesh"));
#endif
#if (ALL==1)||(GLTF_NODE2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_node2,
			 "tf_node",
			 { "ev", "tf","node_id", "mix" },
			 { "EveryApi&", "TF", "int", "float" },
			 { "ev", "", "0", "0.5" },
			 "ML", "mainloop_api", "gltf_node2"));
#endif		       
#if (ALL==1)||(GLTF_SCENE2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene2,
			 "tf_scene2",
			 { "ev", "tf", "scene_id", "mix" },
			 { "EveryApi&", "TF","int", "float" },
			 { "ev", "", "0", "0.5" },
			 "ML", "mainloop_api", "gltf_scene2"));

#endif
#endif
#if (ALL==1)||(ASYNC_GLTF==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::async_gltf,
			 "async_gltf",
			 { "ml", "tf" },
			 { "ML", "TF" },
			 { "", "" },
			 "ML", "mainloop_api", "async_gltf"));
#endif
#if (ALL==1)||(FADE_PIC==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fade_pic,
			 "fade_pic",
			 { "ev", "bm1", "start_time", "transition_time", "end_time", "end_transition_time", "bm2", "start2_time", "transition2_time", "end2_time", "end2_transition_time" },
			 { "EveryApi&", "BM", "float", "float", "float", "float", "BM", "float", "float", "float", "float" },
			 { "ev", "", "0", "20", "50", "20", "", "50", "20", "100", "20" },
			 "ML", "polygon_api", "fade_pic"));
#endif
#if (ALL==1)||(LOAD_SCENE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_scene,
			 "scene_ml",
			 { "ev", "url", "sx", "sy" },
			 { "EveryApi&", "std::string", "int", "int" },
			 { "ev", "http://meshpage.org/assets/landscape.scn@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "600", "600" },
			 "ML", "polygon_api", "load_scene"));
#endif
#endif
#if (ALL==1)||(ASYNC_URL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::async_url,
			 "async_url",
			 { "url", "ml" },
			 { "std::string", "ML" },
			 { "https://meshpage.org/assets/", "" },
			 "ML", "mainloop_api", "async_url"));
#endif
  
#ifndef STABLE
#if (ALL==1)||(PIECHART_FULL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::piechart_full,
			 "piechart_ml",
			 { "ev", "c_x", "c_y", "url", "radius", "numsteps", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "std::string", "float", "int", "float", "float" },
			 { "ev", "0.0", "0.0", "http://meshpage.org/assets/piechart_full.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "350.0", "30", "0.0", "40.0" },
			 "ML", "polygon_api", "piechart_full"));
#endif
#if (ALL==1)||(SKYBOX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::skybox,
			 "skybox_ml",
			 { "ev", "land", "sky" },
			 { "EveryApi&", "BM", "BM" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "skybox"));
#endif
#endif
#if (ALL==1)||(ALT_ML_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::alt_ml_array,
			 "alt_ml",
			 { "ev", "vec", "start_time", "time_delta", "repeat" },
			 { "EveryApi&", "[ML]", "float", "float", "bool" },
			 { "ev", "", "0.0", "10.0", "true" },
			 "ML", "sprite_api", "alt_ml_array"));
#endif

#if (ALL==1)||(OR_ELEM_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::or_elem_ml,
			 "or_elem_ml",
			 { "ev", "m1", "m2" },
			 { "EveryApi&", "ML", "ML" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "or_elem_ml"));
#endif
#if (ALL==1)||(ARRAY_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_ml,
			 "array_ml",
			 { "ev", "arr" },
			 { "EveryApi&", "[ML]" },
			 { "ev", "" },
			 "ML", "mainloop_api", "array_ml"));
#endif
#ifndef STABLE
#if (ALL==1)||(RANDOM_INSTANTIATE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::random_instantiate,
			 "rand_inst",
			 { "ev", "ml", "vec", "start_time", "time_step", "random_chance" },
			 { "EveryApi&", "ML", "[MN]", "float", "float", "float" },
			 { "ev", "", "", "0.0", "2.0", "0.5" },
			 "ML", "mainloop_api", "random_instantiate"));
#endif
#if (ALL==1)||(ACTIVATE_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_array,
			 "activate_array_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "ML", "mainloop_api", "activate_array"));
#endif
#if (ALL==1)||(ACTIVATE_ARR_ARR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_arr_arr,
			 "ac_arr_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "[ML]", "mainloop_api", "activate_arr_arr"));
#endif
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_ml,
			 "timing_ml",
			 { "arr", "duration" },
			 { "[ML]", "float" },
			 { "", "10.0" },
			 "ML", "mainloop_api", "timing_ml"));*/
#if (ALL==1)||(SEQ_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml,
			 "seq_ml",
			 { "vec", "time" },
			 { "[ML]", "float" },
			 { "", "30.0" },
			 "ML", "mainloop_api", "seq_ml"));
#endif
#if (ALL==1)||(SEQ_ML_SCORE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml_score,
			 "seq_ml_score",
			 { "ml1", "ml2", "target_score" },
			 { "ML", "ML", "int" },
			 { "", "", "100" },
			 "ML", "mainloop_api", "seq_ml_score"));
#endif
#if (ALL==1)||(TIMED_TMP_SEQ_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_tmp_seq_ml,
			 "timed_tmp_seq_ml",
			 { "curr", "end", "start_time", "end_time", "show_duration", "key" },
			 { "ML", "ML", "float", "float", "float", "int" },
			 { "", "", "0.0", "100.0", "30.0", "32" },
			 "ML", "mainloop_api", "timed_tmp_seq_ml"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_detection,
			 "collision_ml",
			 { "ev", "player_size", "enemy_size", "normal_game", "gameover_screen" },
			 { "EveryApi&", "float", "float",  "ML", "ML" },
			 { "ev", "100.0", "10.0", "", "" },
			 "ML", "mainloop_api", "collision_detection"));
#endif  
#endif
#if (ALL==1)||(MOVE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml,
			 "move_ml",
			 { "ev", "ml", "mn", "clone_count", "time_delta" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "1", "10.0" },
			 "ML", "move_api", "move_ml"));
#endif
#if (ALL==1)||(KEYPRESS_MOVE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::keypress_move_ml,
			 "key_move_ml",
		       { "ev", "ml", "mn", "clone_count", "time_delta", "key", "flipenabled" },
		       { "EveryApi&", "ML", "MN", "int", "float", "int", "bool" },
		       { "ev", "", "", "1", "10.0", "32", "false" },
			 "ML", "move_api", "keypress_move_ml"));
#endif
#if (ALL==1)||(MOVE_ML_P==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml_p,
			 "move_ml_p",
			 { "ev", "p", "move", "time"  },
			 { "EveryApi&", "P", "MN", "float" },
			 { "ev", "", "", "0.0" },
			 "P", "move_api", "move_ml_p"));
#endif
#ifndef STABLE
#if (ALL==1)||(MOVE_IN==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_in,
			 "move_in",
			 { "ev", "ml", "in" },
			 { "EveryApi&", "ML", "INP" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "move_in"));
#endif
#if (ALL==1)||(DYN_POINTS==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::dyn_points,
			 "dyn_points_ml",
			 { "ev", "ml", "move", "pointnum", "pos_x", "pos_y", "pos_z" },
			 { "EveryApi&", "ML", "MN", "int", "float", "float", "float" },
			 { "ev", "", "", "0", "0.0", "0.0", "0.0" },
			 "ML", "mainloop_api", "dyn_points"));
#endif		     
#endif
#if 0
  // This doesnt work since it eats too much memory.
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::movement_display,
			 "move_display_ml",
			 { "ev", "ml", "mn", "frames", "sx","sy","sz","s_x","e_x", "s_y", "e_y", "s_z", "e_z" },
			 { "EveryApi&", "ML", "MN","int", "int","int","int","float","float", "float","float","float","float" },
			 { "ev", "", "","30", "10","10","10","-300.0","300.0","-300.0","300.0", "-300.0","300.0" },
			 "ML", "points_api", "movement_display"));
#endif

#if (ALL==1)||(REPEAT_ML==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::repeat_ml,
			 "repeat_ml",
			 { "ev", "ml", "duration" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "100.0" },
			 "ML", "move_api", "repeat_ml"));
#endif
#if (ALL==1)||(VERTEX_ARRAY_RENDER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::vertex_array_render,
			 "bm_render",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "sprite_api", "vertex_array_render"));
#endif
#if (ALL==1)||(TURN_TO_2D==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::turn_to_2d,
			 "bm_2d",
			 { "ev", "ml", "tl_x", "tl_y", "br_x", "br_y" },
			 { "EveryApi&", "ML","float", "float", "float", "float" },
			 { "ev", "", "0.0", "0.0", "800.0", "600.0" },
			 "ML", "sprite_api", "turn_to_2d"));
#endif
#if (ALL==1)||(TURN_TO_METERS_DEFAULT==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::turn_to_meters_default,
			 "meters_default",
			 { "ev", "ml", "p" },
			 { "EveryApi&", "ML", "P" },
			 { "ev", "", ""},
			 "ML", "mainloop_api", "turn_to_meters_default"));
#endif
#if (ALL==1)||(TURN_TO_METERS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::turn_to_meters,
			 "meters_scale",
			 { "ev", "ml", "sx1","sx2", "sy1","sy2", "sz1","sz2" },
			 { "EveryApi&", "ML", "float", "float", "float","float", "float", "float" },
			 { "ev", "", "-400","400", "-400","400", "-400","400" },
			 "ML", "mainloop_api", "turn_to_meters"));
#endif
#ifndef STABLE
#if (ALL==1)||(SCALE_2D_SCREEN==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::scale_2d_screen,
			 "bm_2d_screen_scale",
			 { "ev", "orig", "sx", "sy" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "1024", "768" },
			 "ML", "mainloop_api", "scale_2d_screen"));
#endif
#if (ALL==1)||(SPRITE_RENDER==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render,
			 "sp_render",
			 { "ev", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render"));
#endif
#if (ALL==1)||(SPRITE_RENDER_INST==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_inst,
			 "sp_render_inst",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render_inst"));
#endif
#if (ALL==1)||(SPRITE_RENDER_FADE==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_fade,
			 "sp_render_fade",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z", "flip", "start_time", "end_time" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float", "bool", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0", "false", "10.0", "40.0" },
			 "ML", "polygon_api", "sprite_render_fade"));
#endif			 
#endif
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::create_vertex_array,
			 "p_prepare",
			 { "p", "b" },
			 { "P", "bool" },
			 { "", "false" },
			 "VA", "polygon_api", "create_vertex_array"));
#endif
#if (ALL==1)||(RENDER_VERTEX_ARRAY_ML2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_vertex_array_ml2,
			 "p_render",
			 { "ev", "p" },
			 { "EveryApi&", "P" },
			 { "ev", "" },
			 "ML", "polygon_api", "render_vertex_array_ml2"));
#endif
#if (ALL==1)||(RENDER_INSTANCED_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml,
			 "p_render_inst",
			 { "ev", "p", "pts" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced_ml"));
#endif
#ifndef STABLE
#if (ALL==1)||(RENDER_DYNAMIC_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_dynamic_ml,
			 "p_render_dyn",
			 { "ev", "p", "dyn" },
			 { "EveryApi&", "P", "DC" },
			 { "ev", "", "" },
			 "ML", "polygon_api", "render_dynamic_ml"));
#endif
#if (ALL==1)||(RENDER_INSTANCED_ML_FADE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml_fade,
			 "p_render_inst_fade",
			 { "ev", "p", "pts", "flip", "start_time", "end_time" },
			 { "EveryApi&", "P", "PTS", "bool", "float", "float" },
			 { "ev", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "render_instanced_ml_fade"));
#endif
#if (ALL==1)||(RENDER_INSTANCED2_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced2_ml,
			 "p_render_inst2",
			 { "ev", "va", "pta" },
			 { "EveryApi&", "VA", "PTA" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced2_ml","","Can be used for dynamic changes for pta"));
#endif
#endif
#if (ALL==1)||(RENDER_TXID==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::render_txid,
			 "p_render_txid",
			 { "ev", "p1", "I7", "size" },
			 { "EveryApi&", "P", "TXID", "int" },
			 { "ev", "", "", "1024" },
			 "ML", "mainloop_api", "render_txid"));
#endif
#if 0
  // doesnt work in emscripten

  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::prepare,
			 "pts_prepare",
			 { "p" },
			 { "PTS" },
			 { "" },
			 "PTA", "points_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::render_ml,
			 "pts_render",
			 { "ev", "a" },
			 { "EveryApi&", "PTA" },
			 { "ev", "" },
			 "ML", "points_api", "render_ml"));

#endif
#if (ALL==1)||(PTS_RENDER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_render,
			 "pts_render",
			 { "ev", "pts" },
			 { "EveryApi&", "PTS" },
			 { "ev", "" },
			 "ML", "points_api", "pts_render"));
#endif
/*
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::prepare,
			 "li_prepare",
			 { "li" },
			 { "LI" },
			 { "" },
			 "LLA", "lines_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::render_ml,
			 "li_render",
			 { "ev", "lla", "linewidth" },
			 { "EveryApi&", "LLA", "float" },
			 { "ev", "", "1.0" },
			 "ML", "lines_api", "render_ml"));
  */
#if (ALL==1)||(ML_LI_RENDER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::ml_li_render,
			 "ml_li_render",
			 { "ev", "l", "linewidth" },
			 { "EveryApi&", "LI", "float" },
			 { "ev", "", "1.0" },
			 "ML", "lines_api", "ml_li_render"));
#endif
#ifndef STABLE
#if (ALL==1)||(SFO_TO_ML==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::sfo_to_ml,
			 "sfo_render",
			 { "ev", "sfo", "sx", "sy" },
			 { "EveryApi&", "SFO", "float", "float" },
			 { "ev", "", "-1.0", "-1.0" },
			 "ML", "sh_api", "sfo_to_ml"));
#endif
#endif
#if (ALL==1)||(INSTANCED_POSITIONS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,int,int, float,float, float,float, float,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "30", "30", "30", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));
#endif
#if (ALL==1)||(RENDERCUBES3==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rendercubes3,
			 "o_render",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "volume_api", "rendercubes3"));
#endif
#if (ALL==1)||(BIND==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind,
			 "m_bind",
			 { "p", "mat" },
			 { "P", "MT" },
			 { "", "" },
			 "ML", "materials_api", "bind"));
#endif
#if (ALL==1)||(M_BIND_MANY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::m_bind_many,
			 "m_bind_many",
			 { "ev", "vec", "mats", "ticks" },
			 { "EveryApi&", "[P]", "[MT]", "int" },
			 { "ev", "", "", "16" },
			 "ML", "materials_api", "m_bind_many"));
#endif
#if (ALL==1)||(BIND_INST==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst,
			 "m_bind_inst",
			 { "p", "pts", "mat" },
			 { "P", "PTS", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst"));
#endif
#if (ALL==1)||(BIND_INST_MATRIX==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst_matrix,
			 "m_bind_inst_mat",
			 { "p", "ms", "mat" },
			 { "P", "MS", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst_matrix"));
#endif
#if 0
#if (ALL==1)||(BINDINST_PARR_MSARR==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::bindinst_parr_msarr,
			 "m_bindinst_parr",
		       { "ev", "parr", "msarr", "mat" },
		       {"EveryApi&", "[P]", "[MS]", "MT" },
		       { "ev", "", "", "" },
		       "ML", "mainloop_api", "bindinst_parr_msarr"));
#endif
#endif
#if (ALL==1)||(M_BIND_INST_MANY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::m_bind_inst_many,
			 "m_bind_inst_many",
			 { "ev", "vec", "mat", "pts", "max_ticks" },
			 { "EveryApi&", "[P]", "[MT]", "PTS", "int" },
			 { "ev", "", "", "", "16" },
			 "ML", "polygon_api", "m_bind_inst_many"));
#endif
#if (ALL==1)||(MESH_ANIM_DISPLAY_INST==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_anim_display_inst,
			 "m_bind_anim",
			 { "ev", "mesh", "val", "move", "mat", "inst" },
			 { "EveryApi&", "P", "FF", "MN", "MT", "MS" },
			 { "ev", "", "", "", "", "" },
			 "ML", "polygon_api", "mesh_anim_display_inst"));
#endif
#if (ALL==1)||(LOD==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::lod,
			"m_lod",
			{ "ev", "mesh", "mat", "pts", "decimate_level1", "decimate_level2", "decimate_level3", "decimate_level4", "max_obj_count1", "max_obj_count2", "max_obj_count3", "max_obj_count4" },
			{ "EveryApi&", "P", "MT", "PTS", "float", "float","float", "float", "int", "int", "int", "int" },
			{ "ev", "", "", "", "0.8", "0.6","0.4", "0.2", "45", "45", "95", "120" },
			"ML", "mainloop_api", "lod"));
#endif
#if (ALL==1)||(LOD_MATRIX==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::lod_matrix,
			"m_lod_matrix",
			{ "ev", "mesh", "mat", "ms", "decimate_level1", "decimate_level2", "decimate_level3", "decimate_level4", "max_obj_count1", "max_obj_count2", "max_obj_count3", "max_obj_count4" },
			{ "EveryApi&", "P", "MT", "MS", "float", "float","float", "float", "int", "int", "int", "int" },
			{ "ev", "", "", "", "0.8", "0.6","0.4", "0.2", "45", "45", "95", "120" },
			"ML", "mainloop_api", "lod_matrix"));
#endif
#if (ALL==1)||(LOD_PTS_TF==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::lod_pts_tf,
			"m_lod_pts_tf",
			{ "ev", "tf", "pts", "decimate_level1", "decimate_level2", "decimate_level3", "decimate_level4", "max_obj_count1", "max_obj_count2", "max_obj_count3", "max_obj_count4", "mix", "self_mult", "rest_mult", "mode", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color", "transparent", "start_brighness", "end_brightness" },
			{ "EveryApi&", "TF", "PTS", "float", "float","float", "float", "int", "int", "int", "int", "float", "float", "float", "int", "float", "float", "float", "float", "unsigned int", "bool", "float", "float" },
    { "ev", "", "", "0.8", "0.6","0.4", "0.2", "45", "45", "95", "120", "1.0", "1.0", "1.0", "0", "0.1", "0.1", "-400.0", "0", "ff000000", "false", "1.0", "0.3" },
			"ML", "mainloop_api", "lod_pts_tf"));
#endif

#if (ALL==1)||(LOD_MATRIX_TF==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::lod_matrix_tf,
			"m_lod_matrix_tf",
			{ "ev", "tf", "ms", "decimate_level1", "decimate_level2", "decimate_level3", "decimate_level4", "max_obj_count1", "max_obj_count2", "max_obj_count3", "max_obj_count4", "mix", "self_mult", "rest_mult", "mode", "light_dir_x", "light_dir_y", "light_dir_z", "border_width", "border_color", "transparent", "start_brightness", "end_brightness" },
			{ "EveryApi&", "TF", "MS", "float", "float","float", "float", "int", "int", "int", "int", "float", "float", "float", "int", "float", "float", "float", "float", "unsigned int", "bool", "float", "float" },
			{ "ev", "", "", "0.8", "0.6","0.4", "0.2", "45", "45", "95", "120", "1.0", "1.0", "1.0", "0", "0.1", "0.1", "-400.0", "0", "ff000000", "false", "1.0", "0.3" },
			"ML", "mainloop_api", "lod_matrix_tf"));
#endif
#if (ALL==1)||(GET_MOVEMENT_FROM_MV==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::get_movement_from_MV,
			"quake_in_MV",
			{ "ml" },
			{ "ML" },
			{ "" },
			"ML", "mainloop_api", "get_movement_from_MV"));
#endif

 
#ifndef STABLE
#if (ALL==1)||(BIND_INST_FADE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst_fade,
			 "m_fade_inst",
			 { "p", "pts", "mat", "flip", "start_time", "end_time" },
			 { "P", "PTS", "MT", "bool", "float", "float" },
			 { "", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "bind_inst_fade"));
#endif
#if (ALL==1)||(BLOCK_DIVIDE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_divide,
			 "b_divide",
			 { "p", "pos_x", "pos_z", "sx", "sz", "delta_x", "delta_z" },
			 { "P", "float", "float", "int", "int", "float", "float" },
			 { "", "-3000.0", "-3000.0", "60", "60", "100.0", "100.0" },
			 "[P]", "polygon_api", "block_divide"));
#endif
#if (ALL==1)||(BLOCK_RENDER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_render,
			 "b_bind",
			 { "ev", "vec", "mat" },
			 { "EveryApi&", "[P]", "MT" },
			 { "ev", "", "" },
			 "[ML]",  "polygon_api", "block_render"));
#endif
#if (ALL==1)||(BLOCK_RENDER2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_render2,
			 "b_bind2",
			 { "ev", "vec", "mats" },
			 { "EveryApi&", "[P]", "[MT]" },
			 { "ev", "", "" },
			 "[ML]", "polygon_api", "block_render2"));
#endif
#if (ALL==1)||(BLOCK_PTS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::block_pts,
			 "b_pts",
			 { "pts", "d", "max_points_visible" },
			 { "PTS", "float","int" },
			 { "", "300.0","35" },
			 "PTS", "points_api", "block_pts"));
#endif
#if (ALL==1)||(BLOCK_PTS_LOD==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::block_pts_lod,
			 "b_pts_lod",
		       { "pts", "start_x", "end_x", "start_z", "end_z", "max_points_visible", "ncd_start", "ncd_end" },
		       { "PTS", "float", "float", "float", "float","int","float", "float" },
	      { "", "-1000.0", "1000.0", "100.0", "300.0","35", "-1.0", "1.0" },
			 "PTS", "points_api", "block_pts_lod"));
#endif
#if (ALL==1)||(BLOCK_PTS_DEBUG==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::block_pts_debug,
			 "b_pts_debug",
		       { },
		       { },
		       { },
		       "PTS", "points_api", "block_pts_debug"));
#endif
#if (ALL==1)||(BLOCK_DRAW==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_draw,
			 "b_draw",
			 { "vec", "pos_x", "pos_z", "sx", "sz", "delta_x", "delta_z", "view" },
			 { "[ML]", "float", "float", "int", "int", "float", "float", "int" },
			 { "", "-3000.0", "-3000.0", "60", "60", "100.0", "100.0", "10" },
			 "ML", "polygon_api", "block_draw"));
#endif  
#endif
#if (ALL==1)||(ROT_X_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_x_ml,
			 "rot_x_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed", "start_angle", "end_angle"},
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100000.0", "100000.0" },
			 "ML", "move_api", "rot_x_ml"));
#endif
#if (ALL==1)||(KEY_MOVE_ML==1)
 vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_move_ml,
			"move_char_ml",
			{ "ev", "ml", "key_forward", "key_backward", "key_rot_minus", "key_rot_plus", "speed", "rot_speed", "start_angle", "start_x", "end_x", "start_z", "end_z" },
			{ "EveryApi&", "ML", "int", "int", "int", "int", "float", "float", "float", "float", "float", "float", "float" },
	       { "ev", "", "119", "115", "97", "100", "10.0", "0.01", "0.0", "-1000.0", "1000.0", "-1000.0", "1000.0" },
			"ML", "move_api", "key_move_ml"));
#endif
#if (ALL==1)||(ROT_Y_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_y_ml,
			 "rot_y_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_y", "end_y" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100.0","100.0" },
			 "ML", "move_api", "rot_y_ml"));
#endif
#if (ALL==1)||(ROT_Z_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_z_ml,
			 "rot_z_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_z", "end_z" },
			 { "EveryApi&", "ML", "int", "int", "float","float","float" },
			 { "ev", "", "100", "97", "0.01","-100000.0","100000.0" },
			 "ML", "move_api", "rot_z_ml"));
#endif
#ifndef STABLE
#if (ALL==1)||(ENABLE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enable_ml,
			 "enable_ml",
			 { "ev", "ml", "start_time", "end_time" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "0.0", "100.0" },
			 "ML", "move_api", "enable_ml"));
#endif
#if (ALL==1)||(COLOR_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_ml,
			 "color_ml",
			 { "ev", "color_num", "ml", "cc" },
			 { "EveryApi&", "int", "ML", "CC" },
			 { "ev", "0", "", "" },
			 "ML", "move_api", "color_ml"));
#endif
#endif
#if (ALL==1)||(CURSORKEYS_TO_WASD==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::cursorkeys_to_wasd,
			 "keys_to_wasd",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "cursorkeys_to_wasd"));
#endif
#if (ALL==1)||(ALL_CURSOR_KEYS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::all_cursor_keys,
			 "wasd_ml",
			 { "ev", "ml", "speed", "duration" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "8.0", "1.0" },
			 "ML", "move_api", "all_cursor_keys"));
#endif
#if (ALL==1)||(GRID_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::grid_ml,
			 "grid_ml",
			 { "ev", "next", "map", "y", "pos_x", "pos_z", "x_vec_x", "x_vec_z", "z_vec_x", "z_vec_z", "start_x", "start_z", "frame_inc" },
			 { "EveryApi&", "ML", "IBM", "float", "float", "float", "float", "float", "float", "float", "int", "int", "float" },
			 { "ev", "", "", "-120", "0.0", "0.0", "100.0", "0.0", "0.0", "100.0", "0", "0", "0.2" },
			 "ML", "bitmap_api", "grid_ml"));
#endif
#if (ALL==1)||(QUAKE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml,
			 "quake_ml",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml"));
#endif
#if (ALL==1)||(QUAKE_ML2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml2,
			 "quake_ml2",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml2"));
#endif
#if (ALL==1)||(QUAKE_ML3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml3,
			 "fly_ml",
			 { "ev", "ship", "world", "speed", "rot_speed", "p_x", "p_y", "p_z" },
			 { "EveryApi&", "ML", "ML", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "20.0", "0.03", "0.0", "0.0", "0.0" },
			 "ML", "move_api", "quake_ml3"));
#endif
#if (ALL==1)||(ISOMETRIC==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::isometric,
			 "isometric_ml",
			 { "ml", "y_angle", "x_angle", "translate" },
			 { "ML", "float", "float", "float" },
			 { "", "0.5236", "0.5236", "-1400.0" },
			 "ML", "mainloop_api", "isometric"));
#endif
#ifndef STABLE
#if (ALL==1)||(DEBUG_OBJ==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::debug_obj, 
			 "debug_obj",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "ML", "mainloop_api", "debug_obj"));
#endif
#endif
#if (ALL==1)||(PREPARE_KEY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare_key,
			 "prepare_key_ml",
			 { "next", "keyed", "key" },
			 { "ML", "ML", "int" },
			 { "", "", "32" },
			 "ML", "texture_api", "prepare_key"));
#endif
#if (ALL==1)||(SAVE_SCREENSHOT_VIA_KEY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::save_screenshot_via_key,
			 "screenshot_key_ml",
			 { "ev", "ml3", "key", "filename" },
			 { "EveryApi&", "ML", "int", "std::string" },
			 { "ev", "", "32", "screenshot.png" },
			 "ML", "texture_api", "save_screenshot_via_key"));
#endif
#if (ALL==1)||(SAVE_SCREENSHOTS_VIA_KEY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::save_screenshots_via_key,
			 "screenshots_key_ml",
			 { "ev", "ml3", "key", "time_delta", "num" },
			 { "EveryApi&", "ML", "int", "float", "int" },
			 { "ev", "", "32", "10.0", "15" },
			 "ML", "texture_api", "save_screenshots_via_key"));
#endif
#if (ALL==1)||(SEND_KEY_AT_TIME==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::send_key_at_time,
			 "key_at_time",
			 { "ml", "time", "key" },
			 { "ML", "float", "int" },
			 { "", "0.0", "32" },
			 "ML", "mainloop_api", "send_key_at_time"));
#endif
#if (ALL==1)||(KEY_ACTIVATE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_activate_ml,
			 "key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "key_activate_ml"));
#endif
#if (ALL==1)||(TEMP_KEY_ACTIVATE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::temp_key_activate_ml,
			 "tmp_key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "temp_key_activate_ml"));
#endif
#if (ALL==1)||(COMB_KEY_ACTIVATE_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::comb_key_activate_ml,
			 "comb_key_activate_ml",
			 { "ev", "ml", "mn", "key", "key_2", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "int", "float" },
			 { "ev", "", "", "32", "32", "10.0" },
			 "ML", "move_api", "comb_key_activate_ml"));
#endif
#if (ALL==1)||(JOYSTICK_TO_WASD==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::joystick_to_wasd,
			 "joystick_to_wasd",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "joystick_to_wasd"));
#endif
#ifndef STABLE
#if (ALL==1)||(KEYBOARD_TOGGLE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::keyboard_toggle,
			 "key_toggle",
			 { "ml1", "ml2", "key" },
			 { "ML", "ML", "int" },
			 { "", "", "32" },
			 "ML", "mainloop_api", "keyboard_toggle"));
#endif
#endif
#if (ALL==1)||(TOUCH_ROTATE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::touch_rotate,
			 "touch_rotate",
			 { "ev", "ml", "leftright", "topdown", "x_speed", "y_speed" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "true", "true", "0.01", "0.01" },
			 "ML", "mainloop_api", "touch_rotate"));
#endif			 
#if (ALL==1)||(MOUSE_ROLL_ZOOM==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::mouse_roll_zoom,
			 "mouse_zoom",
			 { "ev", "next" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "mainloop_api", "mouse_roll_zoom"));
#endif
#if (ALL==1)||(MOUSE_ROLL_ZOOM2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::mouse_roll_zoom2,
			 "mouse_zoom2",
			 { "ev", "next" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "mainloop_api", "mouse_roll_zoom2"));
#endif  
#if (ALL==1)||(RIGHT_MOUSE_PAN==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::right_mouse_pan,
			 "r_mouse_pan",
			 { "ev", "next" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "mainloop_api", "right_mouse_pan"));
#endif
#ifndef STABLE
#if (ALL==1)||(RECORD_KEYPRESSES==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::record_keypresses,
			 "key_record_ml",
			 { "ml", "output_filename" },
			 { "ML", "std::string" },
			 { "", "key_record.txt" },
			 "ML", "mainloop_api", "record_keypresses"));
#endif
#if (ALL==1)||(PLAYBACK_KEYPRESSES==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::playback_keypresses,
			 "key_playback_ml",
			 { "ml", "input_url" },
			 { "ML", "std::string" },
			 { "", "http://meshpage.org/assets/key_record.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "ML", "mainloop_api", "playback_keypresses" ));
#endif
#endif
#if (ALL==1)||(KEY_PRINTER_ML==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_printer_ml,
			 "key_printer_ml",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "key_printer_ml"));
#endif
#if (ALL==1)||(PRINT_STATS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::print_stats,
			 "mesh_stats_ml",
			 { "p" },
			 { "P" },
			 { "" },
			 "ML", "mainloop_api", "print_stats"));
#endif
#if (ALL==1)||(PRINT_DEPS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::print_deps,
			 "print_deps",
			 { "ml", "num" },
			 { "ML", "int" },
			 { "", "10" },
			 "ML", "mainloop_api", "print_deps"));
#endif
#if (ALL==1)||(JOYSTICK_PRINTER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::joystick_printer,
			 "joy_printer_ml",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "joystick_printer"));
#endif
#if (ALL==1)||(FPS_DISPLAY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::fps_display,
			 "fps_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "file://$(instdir)/Chunkfive.otf@TheLeagueOfMoveableType@https://www.fontsquirrel.com/license/chunkfive" },
			 "ML", "mainloop_api", "fps_display"));
#endif

#ifndef STABLE
#if (ALL==1)||(PARSE_AREATYPE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::parse_areatype,
			 "w_areatype",
			 { "ev", "url", "heightmap", "top_texture", "side_texture" },
			 { "EveryApi&", "std::string", "FB", "BM", "BM" },
			 { "ev", "https://meshpage.org/assets/areatype.txt", "", "", "" },
			 "ML", "mainloop_api", "parse_areatype"));
#endif
#if (ALL==1)||(CREATE_LANDSCAPE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::create_landscape,
			 "w_landscape",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "https://meshpage.org/assets/landscape.txt" },
			 "ML", "mainloop_api", "create_landscape"));
#endif
/*  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, (GameApi::ML (GameApi::MainLoopApi::*)(GameApi::EveryApi&,std::string))&GameApi::MainLoopApi::bind_obj_type,
			 "w_objtype",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "https://meshpage.org/assets/objtype.txt" },
			 "ML", "mainloop_api", "bind_obj_type"));*/
#if (ALL==1)||(READ_OBJ_POS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::read_obj_pos,
			 "w_objpos",
			 { "url" },
			 { "std::string" },
			 { "https://meshpage.org/assets/objpos.txt" },
			 "ML", "mainloop_api", "read_obj_pos"));
#endif
#if (ALL==1)||(CREATE_OBJS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::create_objs,
			 "w_objs",
			 { "ev", "area_id" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "ML", "mainloop_api", "create_objs"));
#endif
#if (ALL==1)||(GLOBE_SHADER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::globe_shader,
			 "globe_shader",
			 { "ev", "mainloop", "globe_r" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "1.0" },
			 "ML", "polygon_api", "globe_shader"));
#endif
#endif
#if (ALL==1)||(SCORE_DISPLAY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_display,
			 "score_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "file://$(instdir)/Chunkfive.otf@TheLeagueOfMoveableType@https://www.fontsquirrel.com/license/chunkfive" },
			 "ML", "mainloop_api", "score_display"));
#endif
#ifndef STABLE
#if (ALL==1)||(TIME_DISPLAY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::time_display,
			 "time_display",
			 { "ev", "ml", "font", "time" },
			 { "EveryApi&", "ML", "std::string", "float" },
			 { "ev", "", "file://$(instdir)/Chunkfive.otf@TheLeagueOfMoveableType@https://www.fontsquirrel.com/license/chunkfive", "1000.0" },
			 "ML", "mainloop_api", "time_display"));
#endif
#if (ALL==1)||(MATRIX_RANGE_CHECK==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::matrix_range_check,
			 "matrix_range_check",
			 { "ev", "ml", "ml2", "url" },
			 { "EveryApi&", "ML", "ML", "std::string" },
			 { "ev", "", "", "http://meshpage.org/assets/test_data.txt" },
			 "ML", "mainloop_api", "matrix_range_check"));
#endif
#endif
#if (ALL==1)||(RESTART_GAME==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::restart_game,
			 "restart_key",
			 { "ev", "ml", "key" },
			 { "EveryApi&", "ML", "int" },
			 { "ev", "", "114" },
			 "ML", "mainloop_api", "restart_game"));
#endif
#if (ALL==1)||(RESTART_SCREEN==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::restart_screen,
			 "restart_screen",
			 { "ev", "ml", "fontname" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "file://$(instdir)/Chunkfive.otf@TheLeagueOfMoveableType@https://www.fontsquirrel.com/license/chunkfive" },
			 "ML", "mainloop_api", "restart_screen"));
#endif
#if (ALL==1)||(FULLSCREEN_BUTTON==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::fullscreen_button,
			 "fullscreen_button",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "ML", "mainloop_api", "fullscreen_button"));
#endif  
#ifndef STABLE
#if (ALL==1)||(SKELETAL_BIND==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind,
			 "skeletal_bind",
			 { "ev", "model", "points", "movement" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]" },
			 { "ev", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind"));
#endif
#if (ALL==1)||(SKELETAL_BIND_MATERIAL==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind_material,
			 "skeletal_material",
			 { "ev", "model", "points", "movement", "material" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]", "MT" },
			 { "ev", "", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind_material"));
#endif
#endif
#if (ALL==1)||(TREE_ML==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_ml,
			 "tree_ml",
			 { "ev", "tree", "vec" },
			 { "EveryApi&", "T", "[ML]" },
			 { "ev", "", "" },
			 "ML", "tree_api", "tree_ml"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst2,
			 "m_bind_inst2",
			 { "p", "pta", "mat" },
			 { "P", "PTA", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst2"));
#endif

#ifndef STABLE
#if (ALL==1)||(VERTEX_ANIM_RENDER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::vertex_anim_render,
			 "kf_render",
			 { "ev", "keyframe" },
			 { "EveryApi&",  "KF" },
			 { "ev",  "" },
			 "ML", "vertex_anim_api", "vertex_anim_render"));
#endif
#endif

#if (ALL==1)||(DISABLE_POLYGONS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::disable_polygons,
			 "ogl_disable_polys",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "disable_polygons"));
#endif
#if (ALL==1)||(DEPTHFUNC==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::depthfunc,
			 "ogl_depthfunc",
			 { "ml", "val" },
			 { "ML", "{lequal,equal,always,less,greater,gequal}" },
			 { "", "1" },
			 "ML", "mainloop_api", "depthfunc"));
#endif
#if (ALL==1)||(DEPTHMASK==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::depthmask,
			 "ogl_depthmask",
			 { "ml", "b" },
			 { "ML", "bool" },
			 { "", "true" },
			 "ML", "mainloop_api", "depthmask"));
#endif
#if (ALL==1)||(BLENDFUNC==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::blendfunc,
			 "ogl_blendfunc",
			 { "ml", "val", "val2" },
			 { "ML", "{zero,one,src_color,one_minus_src_color,dst_color,one_minus_dst_color,src_alpha,one_minus_src_alpha,dst_alpha,one_minus_dst_alpha,constant_color,one_minus_constant_color,constant_alpha}", "{zero,one,src_color,one_minus_src_color,dst_color,one_minus_dst_color,src_alpha,one_minus_src_alpha,dst_alpha,one_minus_dst_alpha,constant_color,one_minus_constant_color,constant_alpha}" },
			 { "", "2", "3" },
			 "ML", "mainloop_api", "blendfunc"));
#endif
#if (ALL==1)||(SHADER_CACHE_DISABLE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::shader_cache_disable,
			 "ogl_shcache_disable",
			 { "ml", "disable" },
			 { "ML", "bool" },
			 { "", "true" },
			 "ML", "mainloop_api", "shader_cache_disable"));
#endif
#if (ALL==1)||(DISABLE_Z_BUFFER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::disable_z_buffer,
			 "ogl_zbuf_disable",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "disable_z_buffer"));
#endif
#if (ALL==1)||(TRANSPARENT==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::transparent,
			 "ogl_transparent",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "transparent"));
#endif
#if (ALL==1)||(SCENE_TRANSPARENCY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::scene_transparency,
			 "ogl_canvas_transparent",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "scene_transparency"));
#endif
#ifndef STABLE
#if (ALL==1)||(PERSPECTIVE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::perspective,
			 "ogl_perspective",
			 { "ev", "ml", "mult", "front_plane", "end_plane" },
			 { "EveryApi&", "ML", "float", "float" ,"float" },
			 { "ev", "", "80.0", "10.1", "60000.0" },
			 "ML", "mainloop_api", "perspective"));
#endif
#if (ALL==1)||(CONCURRENT_DOWNLOAD==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::concurrent_download,
			 "concurrent_download",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "concurrent_download"));
#endif
#if (ALL==1)||(GET_FACE_COUNT==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::get_face_count,
			 "eng_face_count",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "get_face_count"));
#endif
#endif
#if (ALL==1)||(LOAD_SONG==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_song,
			 "song_ml",
			 { "ev", "next", "url" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://meshpage.org/assets/piano_variations.ogg" },
			 "ML", "mainloop_api", "load_song"));
#endif
#if (ALL==1)||(PLAY_WAVE_VIA_KEYPRESS==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::tracker_api, &GameApi::TrackerApi::play_wave_via_keypress,
			 "sound_ml",
			 { "ev", "ml", "url", "key" },
			 { "EveryApi&", "ML", "std::string", "int" },
			 { "ev", "", "http://meshpage.org/assets/Clap.wav", "32" },
			 "ML", "tracker_api", "play_wave_via_keypress"));
#endif
#if (ALL==1)||(LOAD_MIDI==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_midi,
			 "midi_ml",
			 { "ev", "next", "url", "url_patchset" },
			 { "EveryApi&", "ML", "std::string", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/BRAND3.MID", "https://meshpage.org/assets/default.ptc" },
			 "ML", "mainloop_api", "load_midi"));
#endif
#if (ALL==1)||(ANDROID_RESIZE==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::android_resize,
			 "android_resize",
			 { "ev", "ml", "mult" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "1.0" },
			 "ML", "mainloop_api", "android_resize"));
#endif
#ifndef STABLE
#if (ALL==1)||(GAME_WINDOW==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window, 
			 "blk_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "BLK", "blocker_api", "game_window"));
#endif
#endif
#if (ALL==1)||(GAME_WINDOW2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window2, 
			 "run_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "RUN", "blocker_api", "game_window2"));
#endif
#if 0
   vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::webgpu_window,
			  "webgpu_window",
			  { "ev", "ml" },
			  { "EveryApi&", "GML" },
			  { "ev", "" },
			  "RUN", "blocker_api", "webgpu_window"));
#endif
#if (ALL==1)||(EMSCRIPTEN_FRAME2==1)   
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2,
			 "html_run",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "RUN", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame2"));
#endif
#if (ALL==1)||(EMSCRIPTEN_FRAME2_ML==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2_ML,
			 "html_ml",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame2_ML"));
  
#endif
#if (ALL==1)||(EMSCRIPTEN_FRAME2_P==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2_P,
			 "html_p",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "P", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame2_P"));
#endif
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2_MN,
			 "html_mn",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "MN", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame2_mn"));
  */
#if (ALL==1)||(EMSCRIPTEN_FRAME2_MT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2_MT,
			 "html_mt",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "MT", "std::string" },
			 { "ev", "", "https://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame2_MT"));

#endif
#ifndef STABLE


   
#if 0
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame,
			 "html_window", 
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "RUN", "std::string" },
			 { "ev", "", "http://meshpage.org/assets/" },
			 "HML", "mainloop_api", "emscripten_frame"));
#endif
#endif

#if (ALL==1)||(SMALL_WINDOW==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::small_window,
			 "sml_window",
			 { "ev", "ml", "x", "y", "sx", "sy" },
			 { "EveryApi&", "ML", "int", "int", "int", "int" },
			 { "ev", "", "100", "100", "320", "200" },
			 "ML", "mainloop_api", "small_window"));
#endif
//#endif
//vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass,
  //			 "run_looking_glass",
  //			 { "ev", "ml" },
  //			 { "EveryApi&", "ML" },
  //			 { "ev", "" },
  //			 "ML", "mainloop_api", "looking_glass"));
#if 1
#if (ALL==1)||(GAME_WINDOW_2ND_DISPLAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window_2nd_display,
			 "looking_glass_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "false", "false", "0.0", "1000000.0" },
			 "RUN", "blocker_api", "game_window_2nd_display"));
#endif
#endif
  //vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_lib,
  //			 "looking_glass_lib",
  //			 { "txid", "sx", "sy", "x", "y" },
  //			 { "TXID", "int", "int", "int", "int" },
  //			 { "", "512", "256", "4", "8" },
  //			 "ML", "mainloop_api", "looking_glass_lib"));
#if (ALL==1)||(LOOKING_GLASS_FULL==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_full,
			 "looking_glass_full",
			 { "ev", "ml", "amount", "mode", "start", "end" },
			 { "EveryApi&", "ML", "float", "int", "int", "int" },
			 { "ev", "", "150.0", "0", "0", "45" },
			 "ML", "mainloop_api", "looking_glass_full"));
#endif
#if (ALL==1)||(VR_WINDOW==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_window,
			 "vr_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration", "invert", "translate" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float", "bool", "bool" },
			 { "ev", "", "false", "false", "0.0", "100000.0", "true", "true" },
			 "RUN", "blocker_api", "vr_window"));
#endif
#ifdef VIRTUAL_REALITY
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_submit,
			 "vr_submit",
			 { "ev", "left_eye", "right_eye" },
			 { "EveryApi&", "TXID", "TXID" },
			 { "ev", "", "" },
			 "ML", "blocker_api", "vr_submit"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_submit_ml,
			 "vr_submit_ml",
			 { "ev", "ml", "left_eye", "right_eye", "invert", "translate" },
			 { "EveryApi&", "ML", "TXID", "TXID", "bool", "bool" },
			 { "ev", "", "", "", "false", "true" },
			 "ML", "blocker_api", "vr_submit_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::setup_hmd_projection,
			 "vr_projection",
			 { "ev", "ml", "eye", "is_std", "near", "far","translate" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float","bool" },
			 { "ev", "", "false", "true", "10.1", "60000.0", "true" },
			 "ML", "mainloop_api", "setup_hmd_projection"));
#endif
#ifdef VIRTUAL_REALITY_OVERLAY
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::webcam_txid,
			 "vr_webcam",
			 { "num" },
			 { "int" },
			 { "0" },
			 "TXID", "texture_api", "webcam_txid"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::webcam_txid_slow,
			 "vr_webcam_slow",
			 { "ev", "num" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "TXID", "texture_api", "webcam_txid_slow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::vr_overlay,
			 "vr_overlay",
			 { "txid", "key", "name","sx", "sy" },
			 { "TXID", "std::string", "std::string", "int", "int" },
			 { "", "a", "testi", "800", "600" },
			 "ML", "texture_api", "vr_overlay"));
#endif
#ifndef EMSCRIPTEN
#ifndef LINUX
#ifndef ANDROID
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::webcam_txid_win,
			 "vr_webcam_win",
			 { "ev", "sx", "sy", "num" },
			 { "EveryApi&", "int", "int", "int" },
			 { "ev", "800", "600", "0" },
			 "TXID", "texture_api", "webcam_txid_win"));
#endif
#endif
#endif
#if (ALL==1)||(VIDEOFILE_TXID_GENERIC==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::videofile_txid_generic,
			 "videofile_generic",
			 { "ev", "sx", "sy", "url" },
			 { "EveryApi&", "int", "int", "std::string" },
			 { "ev","800", "600", "https://meshpage.org/assets/gameapi_bdcalvin_intro.mp4" },
			 "TXID", "texture_api", "videofile_txid_generic"));
#endif
#if (ALL==1)||(WEBCAM_TXID_GENERIC==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::webcam_txid_generic,
			 "webcam_generic",
			 { "ev", "sx", "sy", "num" },
			 { "EveryApi&", "int", "int", "int" },
			 { "ev", "800", "600", "0" },
			 "TXID", "texture_api", "webcam_txid_generic"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::run_seq,
			 "run_seq",
			 { "ev", "vec" },
			 { "EveryApi&", "[RUN]" },
			 { "ev", "" },
			 "RUN", "blocker_api", "run_seq"));
#endif  
#if 0
// doesnt work in emscripten
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_seq,
			 "blk_seq",
			 { "ev", "vec" },
			 { "EveryApi&", "[BLK]" },
			 { "ev", "" },
			 "BLK", "blocker_api", "game_seq"));

#endif

#if (ALL==1)||(MEMMAP_WINDOW2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::memmap_window2,
			 "pkg_window",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://meshpage.org/assets/game1.pkg" },
			 "ML", "mainloop_api", "memmap_window2"));
#endif
#if (ALL==1)||(MEMMAP_WINDOW3==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::memmap_window3,
			 "pkg_window2",
			 { "ev", "url_1", "url_2", "url_3", "url_4", "url_5", "url_6" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://meshpage.org/assets/game1_1.pkg", "http://meshpage.org/assets/game1_2.pkg", "http://meshpage.org/assets/game1_3.pkg", "http://meshpage.org/assets/game1_4.pkg", "http://meshpage.org/assets/game1_5.pkg", "http://meshpage.org/assets/game1_6.pkg" },
			 "ML", "mainloop_api", "memmap_window3"));
#endif  

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::grab_screen,
			 "grab_screen",
			 { "ev", "r" },
			 { "EveryApi&", "RUN" },
			 { "ev", "" },
			 "RUN,BM", "texture_api", "grab_screen"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::combine_screens,
			 "combine_screens",
			 { "r1", "r2" },
			 { "RUN", "RUN" },
			 { "", "" },
			 "RUN", "texture_api", "combine_screens"))

#endif
#if 0
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gif_anim,
			   "gif_anim",
			   { "ev", "ml", "key", "time_delta", "num", "filename", "delay" },
			   { "EveryApi&", "ML", "int", "float", "int", "std::string", "int" },
			   { "ev", "", "32", "1.0", "30", "output.gif", "4" },
			   "ML", "bitmap_api", "gif_anim"));
#endif
#if (ALL==1)||(SAVE_DEPLOY==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::save_deploy,
			   "save_deploy",
			   { "h", "filename" },
			   { "HML", "std::string" },
			   { "", "gameapi_deploy.zip" },
			   "ZIP", "mainloop_api", "save_deploy"));
#endif    
  return vec;
}
