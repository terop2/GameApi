#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> blocker_functions()
{

  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script,
			 "bm_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/tiiliseina_bm.mp", "a", "b", "c", "d", "e" },
			 "BM", "mainloop_api", "load_BM_script"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script,
			 "p_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/blob_p.mp", "a", "b", "c", "d", "e" },
			 "P", "mainloop_api", "load_P_script"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script,
			 "ml_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/marble_cube_ml.mp", "a", "b", "c", "d", "e" },
			 "ML", "mainloop_api", "load_ML_script"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_MN_script,
			 "mn_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string","std::string","std::string","std::string","std::string" },
			 { "ev", "http://tpgames.org/test_mn.mp", "a", "b", "c","d", "e" },
			 "MN", "mainloop_api", "load_MN_script"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_MT_script,
			 "mt_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string","std::string","std::string","std::string","std::string" },
			 { "ev", "http://tpgames.org/test_mt.mp", "a", "b", "c", "d", "e" },
			 "MT", "mainloop_api", "load_MT_script"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script_array,
			 "bm_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/tiiliseina_bm.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[BM]", "mainloop_api", "load_BM_script_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script_array,
			 "p_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/blob_p.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[P]", "mainloop_api", "load_P_script_array"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script_array,
			 "ml_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/marble_cube_ml.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[ML]", "mainloop_api", "load_ML_script_array"));
#if USE_CHAISCRIPT
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::chai_mainloop,
			 "ml_chai",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/chai_example.txt" },
			 "ML", "mainloop_api", "chai_mainloop"));
#endif

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chai_bm,
			 "chai_bm",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/test_bm.chai", "100", "100" },
			 "BM", "bitmap_api", "chai_bm"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh,
			 "ml_gltf",
			 { "ev", "base_url", "url", "mesh_id" },
			 { "EveryApi&", "std::string", "std::string", "int" }, 
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0" },
			 "ML", "mainloop_api", "gltf_mesh"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_node,
			 "ml_gltf_node",
			 { "ev", "base_url", "url", "node_id" },
			 { "EveryApi&", "std::string", "std::string", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0" },
			 "ML", "mainloop_api", "gltf_node"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene,
			 "ml_gltf_scene",
			 { "ev", "base_url", "url", "scene_id" },
			 { "EveryApi&", "std::string", "std::string", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0" },
			 "ML", "mainloop_api", "gltf_scene"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all,
			 "ml_gltf_all",
			 { "ev", "base_url", "url" },
			 { "EveryApi&", "std::string", "std::string" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb" },
			 "ML", "mainloop_api", "gltf_mesh_all"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_env,
			 "ml_gltf_all_env",
			 { "ev", "base_url", "url", "diffuse", "specular", "bfrd" },
			 { "EveryApi&", "std::string", "std::string", "BM", "BM", "BM" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "", "", "" },
			 "ML", "mainloop_api", "gltf_mesh_all_env"));

  /*  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_anim,
			 "ml_gltf_anim",
			 { "ev", "base_url", "url", "animation", "channel", "mesh_index", "prim_index", "mat" },
			 { "EveryApi&", "std::string", "std::string", "int", "int", "int", "int", "MT" },
			 { "ev", "http://tpgames.org/", "http://tpgames.org/test.glb", "0", "0", "0", "0", "" },
			 "ML", "mainloop_api", "gltf_anim"));*/
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_anim4,
			 "ml_gltf_anim",
			 { "ev", "base_url", "url", "animation", "channel" },
			 { "EveryApi&", "std::string", "std::string", "int", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0", "0" },
			 "ML", "mainloop_api", "gltf_anim4"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_scene_anim,
			 "ml_gltf_sc_anim",
			 { "ev", "base_url", "url", "scene_id", "animation" },
			 { "EveryApi&", "std::string", "std::string", "int", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0", "0" },
			 "ML", "mainloop_api", "gltf_scene_anim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::async_gltf,
			 "async_gltf",
			 { "ml", "base_url", "url" },
			 { "ML", "std::string", "std::string" },
			 { "", "https://tpgames.org/", "https://tpgames.org/test.gltf" },
			 "ML", "mainloop_api", "async_gltf"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_scene,
			 "scene_ml",
			 { "ev", "url", "sx", "sy" },
			 { "EveryApi&", "std::string", "int", "int" },
			 { "ev", "http://tpgames.org/landscape.scn", "600", "600" },
			 "ML", "polygon_api", "load_scene"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::async_url,
			 "async_url",
			 { "url", "ml" },
			 { "std::string", "ML" },
			 { "https://tpgames.org/", "" },
			 "ML", "mainloop_api", "async_url"));
		     
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::piechart_full,
			 "piechart_ml",
			 { "ev", "c_x", "c_y", "url", "radius", "numsteps", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "std::string", "float", "int", "float", "float" },
			 { "ev", "0.0", "0.0", "http://tpgames.org/piechart_full.txt", "350.0", "30", "0.0", "40.0" },
			 "ML", "polygon_api", "piechart_full"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::skybox,
			 "skybox_ml",
			 { "ev", "land", "sky" },
			 { "EveryApi&", "BM", "BM" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "skybox"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::alt_ml_array,
			 "alt_ml",
			 { "ev", "vec", "start_time", "time_delta", "repeat" },
			 { "EveryApi&", "[ML]", "float", "float", "bool" },
			 { "ev", "", "0.0", "10.0", "true" },
			 "ML", "sprite_api", "alt_ml_array"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_ml,
			 "array_ml",
			 { "ev", "arr" },
			 { "EveryApi&", "[ML]" },
			 { "ev", "" },
			 "ML", "mainloop_api", "array_ml"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::random_instantiate,
			 "rand_inst",
			 { "ev", "ml", "vec", "start_time", "time_step", "random_chance" },
			 { "EveryApi&", "ML", "[MN]", "float", "float", "float" },
			 { "ev", "", "", "0.0", "2.0", "0.5" },
			 "ML", "mainloop_api", "random_instantiate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_array,
			 "activate_array_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "ML", "mainloop_api", "activate_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_arr_arr,
			 "ac_arr_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "[ML]", "mainloop_api", "activate_arr_arr"));
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_ml,
			 "timing_ml",
			 { "arr", "duration" },
			 { "[ML]", "float" },
			 { "", "10.0" },
			 "ML", "mainloop_api", "timing_ml"));*/
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml,
			 "seq_ml",
			 { "vec", "time" },
			 { "[ML]", "float" },
			 { "", "30.0" },
			 "ML", "mainloop_api", "seq_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml_score,
			 "seq_ml_score",
			 { "ml1", "ml2", "target_score" },
			 { "ML", "ML", "int" },
			 { "", "", "100" },
			 "ML", "mainloop_api", "seq_ml_score"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_tmp_seq_ml,
			 "timed_tmp_seq_ml",
			 { "curr", "end", "start_time", "end_time", "show_duration", "key" },
			 { "ML", "ML", "float", "float", "float", "int" },
			 { "", "", "0.0", "100.0", "30.0", "32" },
			 "ML", "mainloop_api", "timed_tmp_seq_ml"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_detection,
			 "collision_ml",
			 { "ev", "player_size", "enemy_size", "normal_game", "gameover_screen" },
			 { "EveryApi&", "float", "float",  "ML", "ML" },
			 { "ev", "100.0", "10.0", "", "" },
			 "ML", "mainloop_api", "collision_detection"));
#endif  
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml,
			 "move_ml",
			 { "ev", "ml", "mn", "clone_count", "time_delta" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "1", "10.0" },
			 "ML", "move_api", "move_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml_p,
			 "move_ml_p",
			 { "ev", "p", "move", "time"  },
			 { "EveryApi&", "P", "MN", "float" },
			 { "ev", "", "", "0.0" },
			 "P", "move_api", "move_ml_p"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_in,
			 "move_in",
			 { "ev", "ml", "in" },
			 { "EveryApi&", "ML", "INP" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "move_in"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::dyn_points,
			 "dyn_points_ml",
			 { "ev", "ml", "move", "pointnum", "pos_x", "pos_y", "pos_z" },
			 { "EveryApi&", "ML", "MN", "int", "float", "float", "float" },
			 { "ev", "", "", "0", "0.0", "0.0", "0.0" },
			 "ML", "mainloop_api", "dyn_points"));
		     
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

  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::repeat_ml,
			 "repeat_ml",
			 { "ev", "ml", "duration" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "100.0" },
			 "ML", "move_api", "repeat_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::vertex_array_render,
			 "bm_render",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "sprite_api", "vertex_array_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::turn_to_2d,
			 "bm_2d",
			 { "ev", "ml", "tl_x", "tl_y", "br_x", "br_y" },
			 { "EveryApi&", "ML","float", "float", "float", "float" },
			 { "ev", "", "0.0", "0.0", "800.0", "600.0" },
			 "ML", "sprite_api", "turn_to_2d"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::scale_2d_screen,
			 "bm_2d_screen_scale",
			 { "ev", "orig", "sx", "sy" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "1024", "768" },
			 "ML", "mainloop_api", "scale_2d_screen"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render,
			 "sp_render",
			 { "ev", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_inst,
			 "sp_render_inst",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render_inst"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_fade,
			 "sp_render_fade",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z", "flip", "start_time", "end_time" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float", "bool", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0", "false", "10.0", "40.0" },
			 "ML", "polygon_api", "sprite_render_fade"));
			 
#endif
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::create_vertex_array,
			 "p_prepare",
			 { "p", "b" },
			 { "P", "bool" },
			 { "", "false" },
			 "VA", "polygon_api", "create_vertex_array"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_vertex_array_ml2,
			 "p_render",
			 { "ev", "p" },
			 { "EveryApi&", "P" },
			 { "ev", "" },
			 "ML", "polygon_api", "render_vertex_array_ml2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml,
			 "p_render_inst",
			 { "ev", "p", "pts" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced_ml"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_dynamic_ml,
			 "p_render_dyn",
			 { "ev", "p", "dyn" },
			 { "EveryApi&", "P", "DC" },
			 { "ev", "", "" },
			 "ML", "polygon_api", "render_dynamic_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml_fade,
			 "p_render_inst_fade",
			 { "ev", "p", "pts", "flip", "start_time", "end_time" },
			 { "EveryApi&", "P", "PTS", "bool", "float", "float" },
			 { "ev", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "render_instanced_ml_fade"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced2_ml,
			 "p_render_inst2",
			 { "ev", "va", "pta" },
			 { "EveryApi&", "VA", "PTA" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced2_ml","","Can be used for dynamic changes for pta"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_render,
			 "pts_render",
			 { "ev", "pts" },
			 { "EveryApi&", "PTS" },
			 { "ev", "" },
			 "ML", "points_api", "pts_render"));
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
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::sfo_to_ml,
			 "sfo_render",
			 { "ev", "sfo", "sx", "sy" },
			 { "EveryApi&", "SFO", "float", "float" },
			 { "ev", "", "-1.0", "-1.0" },
			 "ML", "sh_api", "sfo_to_ml"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,int,int, float,float, float,float, float,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "30", "30", "30", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rendercubes3,
			 "o_render",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "volume_api", "rendercubes3"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind,
			 "m_bind",
			 { "p", "mat" },
			 { "P", "MT" },
			 { "", "" },
			 "ML", "materials_api", "bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst,
			 "m_bind_inst",
			 { "p", "pts", "mat" },
			 { "P", "PTS", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst_matrix,
			 "m_bind_inst_mat",
			 { "p", "ms", "mat" },
			 { "P", "MS", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst_matrix"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::m_bind_inst_many,
			 "m_bind_inst_many",
			 { "ev", "vec", "mat", "pts" },
			 { "EveryApi&", "[P]", "[MT]", "PTS" },
			 { "ev", "", "", "" },
			 "ML", "polygon_api", "m_bind_inst_many"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_anim_display_inst,
			 "m_bind_anim",
			 { "ev", "mesh", "val", "move", "mat", "inst", "repeat_time" },
			 { "EveryApi&", "P", "FF", "MN", "MT", "MS", "float" },
			 { "ev", "", "", "", "", "", "10000.0" },
			 "ML", "polygon_api", "mesh_anim_display_inst"));
#ifndef STABLE			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst_fade,
			 "m_fade_inst",
			 { "p", "pts", "mat", "flip", "start_time", "end_time" },
			 { "P", "PTS", "MT", "bool", "float", "float" },
			 { "", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "bind_inst_fade"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_divide,
			 "b_divide",
			 { "p", "pos_x", "pos_z", "sx", "sz", "delta_x", "delta_z" },
			 { "P", "float", "float", "int", "int", "float", "float" },
			 { "", "-3000.0", "-3000.0", "60", "60", "100.0", "100.0" },
			 "[P]", "polygon_api", "block_divide"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_render,
			 "b_bind",
			 { "ev", "vec", "mat" },
			 { "EveryApi&", "[P]", "MT" },
			 { "ev", "", "" },
			 "[ML]",  "polygon_api", "block_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_render2,
			 "b_bind2",
			 { "ev", "vec", "mats" },
			 { "EveryApi&", "[P]", "[MT]" },
			 { "ev", "", "" },
			 "[ML]", "polygon_api", "block_render2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::block_draw,
			 "b_draw",
			 { "vec", "pos_x", "pos_z", "sx", "sz", "delta_x", "delta_z", "view" },
			 { "[ML]", "float", "float", "int", "int", "float", "float", "int" },
			 { "", "-3000.0", "-3000.0", "60", "60", "100.0", "100.0", "10" },
			 "ML", "polygon_api", "block_draw"));
  
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_x_ml,
			 "rot_x_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed", "start_angle", "end_angle"},
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100000.0", "100000.0" },
			 "ML", "move_api", "rot_x_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_y_ml,
			 "rot_y_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_y", "end_y" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100.0","100.0" },
			 "ML", "move_api", "rot_y_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_z_ml,
			 "rot_z_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_z", "end_z" },
			 { "EveryApi&", "ML", "int", "int", "float","float","float" },
			 { "ev", "", "100", "97", "0.01","-100000.0","100000.0" },
			 "ML", "move_api", "rot_z_ml"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enable_ml,
			 "enable_ml",
			 { "ev", "ml", "start_time", "end_time" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "0.0", "100.0" },
			 "ML", "move_api", "enable_ml"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_ml,
			 "color_ml",
			 { "ev", "color_num", "ml", "cc" },
			 { "EveryApi&", "int", "ML", "CC" },
			 { "ev", "0", "", "" },
			 "ML", "move_api", "color_ml"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::all_cursor_keys,
			 "wasd_ml",
			 { "ev", "ml", "speed", "duration" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "8.0", "1.0" },
			 "ML", "move_api", "all_cursor_keys"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::grid_ml,
			 "grid_ml",
			 { "ev", "next", "map", "y", "pos_x", "pos_z", "x_vec_x", "x_vec_z", "z_vec_x", "z_vec_z", "start_x", "start_z", "frame_inc" },
			 { "EveryApi&", "ML", "IBM", "float", "float", "float", "float", "float", "float", "float", "int", "int", "float" },
			 { "ev", "", "", "-120", "0.0", "0.0", "100.0", "0.0", "0.0", "100.0", "0", "0", "0.2" },
			 "ML", "bitmap_api", "grid_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml,
			 "quake_ml",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml2,
			 "quake_ml2",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml3,
			 "fly_ml",
			 { "ev", "ship", "world", "speed", "rot_speed", "p_x", "p_y", "p_z" },
			 { "EveryApi&", "ML", "ML", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "20.0", "0.03", "0.0", "0.0", "0.0" },
			 "ML", "move_api", "quake_ml3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::isometric,
			 "isometric_ml",
			 { "ml", "y_angle", "x_angle", "translate" },
			 { "ML", "float", "float", "float" },
			 { "", "0.5236", "0.5236", "-1400.0" },
			 "ML", "mainloop_api", "isometric"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::debug_obj, 
			 "debug_obj",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "ML", "mainloop_api", "debug_obj"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_activate_ml,
			 "key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::temp_key_activate_ml,
			 "tmp_key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "temp_key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::comb_key_activate_ml,
			 "comb_key_activate_ml",
			 { "ev", "ml", "mn", "key", "key_2", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "int", "float" },
			 { "ev", "", "", "32", "32", "10.0" },
			 "ML", "move_api", "comb_key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::joystick_to_wasd,
			 "joystick_to_wasd",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "joystick_to_wasd"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::keyboard_toggle,
			 "key_toggle",
			 { "ml1", "ml2", "key" },
			 { "ML", "ML", "int" },
			 { "", "", "32" },
			 "ML", "mainloop_api", "keyboard_toggle"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::touch_rotate,
			 "touch_rotate",
			 { "ev", "ml", "leftright", "topdown", "x_speed", "y_speed" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "true", "true", "0.01", "0" },
			 "ML", "mainloop_api", "touch_rotate"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::mouse_roll_zoom,
			 "mouse_zoom",
			 { "ev", "next" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "mainloop_api", "mouse_roll_zoom"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::right_mouse_pan,
			 "r_mouse_pan",
			 { "ev", "next" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "mainloop_api", "right_mouse_pan"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::record_keypresses,
			 "key_record_ml",
			 { "ml", "output_filename" },
			 { "ML", "std::string" },
			 { "", "key_record.txt" },
			 "ML", "mainloop_api", "record_keypresses"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::playback_keypresses,
			 "key_playback_ml",
			 { "ml", "input_url" },
			 { "ML", "std::string" },
			 { "", "http://tpgames.org/key_record.txt" },
			 "ML", "mainloop_api", "playback_keypresses" ));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_printer_ml,
			 "key_printer_ml",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "key_printer_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::print_stats,
			 "mesh_stats_ml",
			 { "p" },
			 { "P" },
			 { "" },
			 "ML", "mainloop_api", "print_stats"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::joystick_printer,
			 "joy_printer_ml",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "joystick_printer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::fps_display,
			 "fps_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "fps_display"));

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::parse_areatype,
			 "w_areatype",
			 { "ev", "url", "heightmap", "top_texture", "side_texture" },
			 { "EveryApi&", "std::string", "FB", "BM", "BM" },
			 { "ev", "https://tpgames.org/areatype.txt", "", "", "" },
			 "ML", "mainloop_api", "parse_areatype"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::create_landscape,
			 "w_landscape",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "https://tpgames.org/landscape.txt" },
			 "ML", "mainloop_api", "create_landscape"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, (GameApi::ML (GameApi::MainLoopApi::*)(GameApi::EveryApi&,std::string))&GameApi::MainLoopApi::bind_obj_type,
			 "w_objtype",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "https://tpgames.org/objtype.txt" },
			 "ML", "mainloop_api", "bind_obj_type"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::read_obj_pos,
			 "w_objpos",
			 { "url" },
			 { "std::string" },
			 { "https://tpgames.org/objpos.txt" },
			 "ML", "mainloop_api", "read_obj_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::create_objs,
			 "w_objs",
			 { "ev", "area_id" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "ML", "mainloop_api", "create_objs"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::globe_shader,
			 "globe_shader",
			 { "ev", "mainloop", "globe_r" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "1.0" },
			 "ML", "polygon_api", "globe_shader"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_display,
			 "score_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "score_display"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::time_display,
			 "time_display",
			 { "ev", "ml", "font", "time" },
			 { "EveryApi&", "ML", "std::string", "float" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf", "1000.0" },
			 "ML", "mainloop_api", "time_display"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::matrix_range_check,
			 "matrix_range_check",
			 { "ev", "ml", "ml2", "url" },
			 { "EveryApi&", "ML", "ML", "std::string" },
			 { "ev", "", "", "http://tpgames.org/test_data.txt" },
			 "ML", "mainloop_api", "matrix_range_check"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::restart_game,
			 "restart_key",
			 { "ev", "ml", "key" },
			 { "EveryApi&", "ML", "int" },
			 { "ev", "", "114" },
			 "ML", "mainloop_api", "restart_game"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::restart_screen,
			 "restart_screen",
			 { "ev", "ml", "fontname" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "restart_screen"));
			
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind,
			 "skeletal_bind",
			 { "ev", "model", "points", "movement" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]" },
			 { "ev", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind_material,
			 "skeletal_material",
			 { "ev", "model", "points", "movement", "material" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]", "MT" },
			 { "ev", "", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind_material"));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_ml,
			 "tree_ml",
			 { "ev", "tree", "vec" },
			 { "EveryApi&", "T", "[ML]" },
			 { "ev", "", "" },
			 "ML", "tree_api", "tree_ml"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst2,
			 "m_bind_inst2",
			 { "p", "pta", "mat" },
			 { "P", "PTA", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst2"));
#endif

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::vertex_anim_render,
			 "kf_render",
			 { "ev", "keyframe" },
			 { "EveryApi&",  "KF" },
			 { "ev",  "" },
			 "ML", "vertex_anim_api", "vertex_anim_render"));

#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::depthfunc,
			 "ogl_depthfunc",
			 { "ml", "val" },
			 { "ML", "int" },
			 { "", "1" },
			 "ML", "mainloop_api", "depthfunc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::blendfunc,
			 "ogl_blendfunc",
			 { "ml", "val", "val2" },
			 { "ML", "int", "int" },
			 { "", "2", "3" },
			 "ML", "mainloop_api", "blendfunc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::disable_z_buffer,
			 "ogl_zbuf_disable",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "disable_z_buffer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::transparent,
			 "ogl_transparent",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "transparent"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::perspective,
			 "ogl_perspective",
			 { "ev", "ml", "mult", "front_plane", "end_plane" },
			 { "EveryApi&", "ML", "float", "float" ,"float" },
			 { "ev", "", "80.0", "10.1", "60000.0" },
			 "ML", "mainloop_api", "perspective"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::get_face_count,
			 "eng_face_count",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "get_face_count"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_song,
			 "song_ml",
			 { "ev", "next", "url" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/piano_variations.ogg" },
			 "ML", "mainloop_api", "load_song"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tracker_api, &GameApi::TrackerApi::play_wave_via_keypress,
			 "sound_ml",
			 { "ev", "ml", "url", "key" },
			 { "EveryApi&", "ML", "std::string", "int" },
			 { "ev", "", "http://tpgames.org/Clap.wav", "32" },
			 "ML", "tracker_api", "play_wave_via_keypress"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window, 
			 "blk_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "BLK", "blocker_api", "game_window"));
#endif
   vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window2, 
			 "run_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "RUN", "blocker_api", "game_window2"));
#ifndef STABLE
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame,
			 "html_window", 
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "RUN", "std::string" },
			 { "ev", "", "http://tpgames.org/" },
			 "HML", "mainloop_api", "emscripten_frame"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::emscripten_frame2,
			 "html_window2",
			 { "ev", "r", "homepage" },
			 { "EveryApi&", "RUN", "std::string" },
			 { "ev", "", "https://tpgames.org/" },
			 "HML", "mainloop_api", "emscripten_frame2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::small_window,
			 "sml_window",
			 { "ev", "ml", "x", "y", "sx", "sy" },
			 { "EveryApi&", "ML", "int", "int", "int", "int" },
			 { "ev", "", "100", "100", "320", "200" },
			 "ML", "mainloop_api", "small_window"));
#endif
//vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass,
  //			 "run_looking_glass",
  //			 { "ev", "ml" },
  //			 { "EveryApi&", "ML" },
  //			 { "ev", "" },
  //			 "ML", "mainloop_api", "looking_glass"));
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window_2nd_display,
			 "looking_glass_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "false", "false", "0.0", "1000000.0" },
			 "RUN", "blocker_api", "game_window_2nd_display"));
#endif
  //vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_lib,
  //			 "looking_glass_lib",
  //			 { "txid", "sx", "sy", "x", "y" },
  //			 { "TXID", "int", "int", "int", "int" },
  //			 { "", "512", "256", "4", "8" },
  //			 "ML", "mainloop_api", "looking_glass_lib"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_full,
			 "looking_glass_full",
			 { "ev", "ml", "amount", "mode", "start", "end" },
			 { "EveryApi&", "ML", "float", "int", "int", "int" },
			 { "ev", "", "150.0", "0", "0", "45" },
			 "ML", "mainloop_api", "looking_glass_full"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_window,
			 "vr_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration", "invert", "translate" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float", "bool", "bool" },
			 { "ev", "", "false", "false", "0.0", "100000.0", "true", "true" },
			 "RUN", "blocker_api", "vr_window"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::webcam_txid_win,
			 "vr_webcam_win",
			 { "ev", "sx", "sy", "num" },
			 { "EveryApi&", "int", "int", "int" },
			 { "ev", "800", "600", "0" },
			 "TXID", "texture_api", "webcam_txid_win"));
#endif
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::memmap_window2,
			 "pkg_window",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/game1.pkg" },
			 "ML", "mainloop_api", "memmap_window2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::memmap_window3,
			 "pkg_window2",
			 { "ev", "url_1", "url_2", "url_3", "url_4", "url_5", "url_6" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/game1_1.pkg", "http://tpgames.org/game1_2.pkg", "http://tpgames.org/game1_3.pkg", "http://tpgames.org/game1_4.pkg", "http://tpgames.org/game1_5.pkg", "http://tpgames.org/game1_6.pkg" },
			 "ML", "mainloop_api", "memmap_window3"));
  
  return vec;
}