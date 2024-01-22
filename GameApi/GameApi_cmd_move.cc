#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> moveapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::mn_empty,
			 "mn_empty",
			 { },
			 { },
			 { },
			 "MN", "move_api", "mn_empty"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::level,
			 "mn_custom",
			 { "mn" },
			 { "MN" },
			 { "" },
			 "MN", "move_api", "level"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::trans2,
			 "mn_translate",
			 { "next", "dx", "dy", "dz" },
			 { "MN", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "trans2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale2,
			 "mn_scale",
			 { "next", "sx", "sy", "sz" },
			 { "MN", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatex,
			 "mn_rotatex",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatey,
			 "mn_rotatey",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatey"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatez,
			 "mn_rotatez",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatez"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotate_around_axis,
			 "mn_rotate_axis",
			 { "next", "p_x", "p_y", "p_z", "v_x", "v_y", "v_z", "angle" },
			 { "MN", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0","0","0","0", "1","0","3.14159" },
			 "MN", "move_api", "rotate_around_axis"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::mn_mouse_y,
			 "mn_mouse_y",
			 { "fetcher", "next", "start_x", "end_x", "start_y", "end_y", "start_val", "end_val" },
			 { "FF", "MN", "float", "float", "float", "float", "float", "float" },
			 { "", "", "0.0", "800.0", "0.0", "1200.0", "0.0", "1200.0" },
			 "MN", "move_api", "mn_mouse_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::pose,
			 "mn_hmd_pose",
			 { "next", "pose_in_screen" },
			 { "MN", "bool" },
			 { "", "false" },
			 "MN", "move_api", "pose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::state_speed_movement,
			 "mn_statemachine",
			 { "mn", "url", "states", "x_speeds", "y_speeds", "z_speeds" },
			 { "MN", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "", "http://tpgames.org/move.sm@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "s0&s1&s2&s3&s4", "0.0&-1.0&0.0&0.0&1.0", "0.0&0.0&-1.0&1.0&0.0", "0.0&0.0&0.0&0.0&0.0" },
			 "MN", "mainloop_api", "state_speed_movement"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale_progress,
			 "mn_scale_progress",
			 { "next", "is_x", "is_y", "is_z" },
			 { "MN", "bool", "bool", "bool" },
			 { "", "true", "false", "false" },
			 "MN", "move_api", "scale_progress"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::mn_fetcher,
			 "mn_pos_fetcher",
			 { "point_fetcher" },
			 { "PF" },
			 { "" },
			 "MN", "move_api", "mn_fetcher"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::mn_interpolate,
			 "mn_slope",
			 { "n1", "n2", "fetcher" },
			 { "MN", "MN", "FF" },
			 { "", "", "" },
			 "MN", "move_api", "mn_interpolate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::interpolate,
			 "mn_interpolate",
			 { "n1", "n2", "start_time", "end_time", "start_value", "end_value" },
			 { "MN", "MN", "float", "float", "float", "float" },
			 { "", "", "0.0", "30.0", "0.0", "1.0" },
			 "MN", "move_api", "interpolate"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::local_move,
			 "local_move",
			 { "ev", "inner_ml", "center_points" },
			 { "EveryApi&", "ML", "PTS" },
			 { "ev", "", "" },
			 "ML", "move_api", "local_move"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::local_move_matrix,
			 "local_move_mat",
			 { "ev", "inner_ml", "matrices" },
			 { "EveryApi&", "ML", "MS" },
			 { "ev", "", "" },
			 "ML", "move_api", "local_move_matrix"));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_anim,
			 "mesh_anim",
			 { "ev", "faces", "move", "materials", "inst", "states", "url" },
			 { "EveryApi&", "[P]", "[MN]", "[MT]", "[MS]", "[IF]", "std::string" },
			 { "ev", "", "", "", "", "","https://tpgames.org/gameapi_anim.mp@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "ML", "polygon_api", "mesh_anim"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::ske_anim,
			 "ske_anim",
			 { "ev", "mesh", "orig_pose", "li_size", "url", "new_poses", "material" },
			 { "EveryApi&", "P", "LI", "int", "std::string", "[LI]", "MT" },
			 { "ev", "", "", "30", "https://tpgames.org/ske_example.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "", "" },
			 "ML", "polygon_api", "ske_anim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::ske_anim2,
			 "ske_anim2",
			 { "ev", "mesh", "orig_pose", "li_size", "url", "new_poses", "material" },
			 { "EveryApi&", "[P]", "LI", "int", "std::string", "[LI]", "MT" },
			 { "ev", "", "", "30", "https://tpgames.org/ske_example.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "", "" },
			 "ML", "polygon_api", "ske_anim2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::tunnel_tree,
			 "tunnel_tree",
			 { "ev", "faces", "moves", "url", "mat2" },
			 { "EveryApi&", "[P]", "[MN]", "std::string", "MT" },
			 { "ev", "", "", "https://tpgames.org/tunnel.sm@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "" },
			 "ML", "mainloop_api", "tunnel_tree"));
  /*			
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_skeleton,
			 "li_gltf_ske",
			 { "ev", "base_url", "url", "start_node" },
			 { "EveryApi&", "std::string", "std::string", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0" },
			 "LI", "mainloop_api", "gltf_skeleton"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_anim_skeleton,
			 "li_gltf_aske",
			 { "ev", "base_url", "url", "skin_num", "animation", "channel", "num_keyframes" },
			 { "EveryApi&", "std::string", "std::string", "int", "int", "int", "int" },
			 { "ev", "https://tpgames.org/", "https://tpgames.org/test.glb", "0", "0", "0", "64" },
			 "[LI]", "mainloop_api", "gltf_anim_skeleton"));
  */

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_split_faces2,
			 "p_gltf_split",
			 { "ev", "tf", "mesh_index", "prim_index", "max_attach" },
			 { "EveryApi&", "TF", "int", "int", "int" },
			 { "ev", "", "0", "0", "64" },
			 "[P]", "polygon_api", "gltf_split_faces2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::translate,
			 "anim_translate",
			 { "next", "start_time", "end_time", "dx", "dy", "dz" },
			 { "MN", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale,
			 "anim_scale",
			 { "next", "start_time", "end_time", "sx", "sy", "sz" },
			 { "MN", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotate,
			 "anim_rotate",
			 { "next", "start_time", "end_time", "p_x", "p_y", "p_z",
			     "v_x", "v_y", "v_z", "angle" },
			 { "MN", "float", "float",
			     "float", "float","float",
			     "float", "float","float",
			     "float" },
			 { "", "0.0", "100.0",
			     "0.0", "0.0", "0.0",
			     "0.0", "1.0", "0.0",
			     "6.28318" },
			 "MN", "move_api", "rotate"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::translate_wave,
			 "wave_translate",
			 { "next", "start_time", "end_time", "wave", "dx", "dy", "dz" },
			 { "MN", "float", "float", "WV", "float", "float", "float" },
			 { "", "0.0", "100.0", "", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "translate_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale_wave,
			 "wave_scale",
			 { "next", "start_time", "end_time", "wave", "sx", "sy", "sz" },
			 { "MN", "float", "float", "WV", "float", "float", "float" },
			 { "", "0.0", "100.0", "", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotate_wave,
			 "wave_rotate",
			 { "next", "start_time", "end_time", "wave", "p_x", "p_y", "p_z",
			     "v_x", "v_y", "v_z", "angle" },
			 { "MN", "float", "float", "WV",
			     "float", "float","float",
			     "float", "float","float",
			     "float" },
			 { "", "0.0", "100.0", "",
			     "0.0", "0.0", "0.0",
			     "0.0", "1.0", "0.0",
			     "6.28318" },
			 "MN", "move_api", "rotate_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::compress,
			 "anim_compress",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "compress"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::change_time,
			 "anim_time",
			 { "next", "delta_time" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "change_time"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::time_repeat,
			 "anim_repeat",
			 { "next", "start_time", "repeat_duration" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "time_repeat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_enable,
			 "anim_enable",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_enable"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_disable,
			 "anim_disable",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_disable"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_choose,
			 "anim_choose",
			 { "vec", "start_time", "duration" },
			 { "[MN]", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_choose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::event_activate,
			 "anim_event",
			 { "next", "event", "event_time", "event_duration" },
			 { "MN", "MN", "float", "float" },
			 { "", "", "10.0", "100.0" },
			 "MN", "move_api", "event_activate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::whack_a_mole_all,
			 "whack",
			 { "ev", "cycle_count", "exit_num" },
			 { "EveryApi&","int", "int" },
			 { "ev", "0", "0" },
			 "MN", "move_api", "whack_a_mole_all"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::whack_a_mole_collision,
			 "whack_collide",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "whack_a_mole_collision"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::whack_a_mole_flag_bmchooser,
			 "whack_bmchoose",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "mainloop_api", "whack_a_mole_flag_bmchooser"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::whack_a_mole_explosion,
			 "whack_explosion",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "PTS", "mainloop_api", "whack_a_mole_explosion"));
#endif

#ifndef STABLE
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_start,
			 "color_start",
			 { "color" },
			 { "unsigned int" },
			 { "ffffffff" },
			 "CC", "move_api", "color_start"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_interpolate,
			 "color_interpolate",
			 { "next", "start_color", "end_color", "start_time", "end_time" },
			 { "CC", "unsigned int", "unsigned int", "float", "float" },
			 { "", "ffffffff", "ff888888", "0.0", "100.0" },
			 "CC", "move_api", "color_interpolate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::cursor_keys_normal_move,
			 "move_normal",
			 { },
			 { },
			 { },
			 "INP", "mainloop_api", "cursor_keys_normal_move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::cursor_keys_rotate_move,
			 "move_rotate",
			 { "speed_rot", "r_forward", "r_normal", "r_backward" },
			 { "float", "float", "float", "float" },
			 { "0.3141", "1.0", "0.0", "-1.0" },
			 "INP", "mainloop_api", "cursor_keys_rotate_move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_speed,
			 "move_speed",
			 { "orig", "speed" },
			 { "INP", "float" },
			 { "", "1.0" },
			 "INP", "mainloop_api", "move_speed"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_collision,
			 "move_coll",
			 { "scene", "start_x", "end_x", "start_y", "end_y", "s_x", "m_x", "e_x", "s_y", "m_y", "e_y", "speed_up", "speed_down", "speed_left", "speed_right", "speed_gravity" },
			 { "IBM", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "","-200.0", "200.0", "-200.0", "200.0", "-5.0", "0.0", "5.0", "5.0", "0.0", "-5.0", "20.0", "-10.0", "-10.0", "10.0", "-10.0" },
			 "INP", "mainloop_api", "move_collision"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_sequence,
			 "move_sequence",
			 { "start_time", "time_per_char", "string" },
			 { "float", "float", "std::string" },
			 { "0.0", "3.0", "wwddssaa" },
			 "INP", "mainloop_api", "move_sequence"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gravity,
			 "move_gravity",
			 { "inp", "scene", "start_x", "end_x", "start_y", "end_y","speed" },
			 { "INP", "IBM", "float", "float", "float", "float","float" },
			 { "", "", "0.0", "800.0", "0.0", "600.0", "10.0" },
			 "INP", "mainloop_api", "gravity"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::root,
			 "sa_root",
			 { "points" },
			 { "PT" },
			 { "" },
			 "SA", "skeletal_api", "root"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::node,
			 "sa_node",
			 { "parent", "matrix", "point_offset" },
			 { "SA", "MN", "PT" },
			 { "", "", "" },
			 "SA", "skeletal_api", "node"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_x_ml,
			 "move_x_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed", "start_x", "end_x" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "5.0","-100.0", "100.0" },
			 "ML", "move_api", "move_x_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_y_ml,
			 "move_y_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_y", "end_y" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "119", "115", "5.0","-100.0","100.0" },
			 "ML", "move_api", "move_y_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_z_ml,
			 "move_z_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_z", "end_z" },
			 { "EveryApi&", "ML", "int", "int", "float","float","float" },
			 { "ev", "", "107", "109", "5.0","-100.0","100.0" },
			 "ML", "move_api", "move_z_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::jump_ml,
			 "jump_ml",
			 { "ev", "ml", "key_jump", "height", "jump_duration" },
			 { "EveryApi&", "ML", "int", "float", "float" },
			 { "ev", "", "32", "300.0", "10.0" },
			 "ML", "move_api", "jump_ml"));
			 
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::sfo_sandbox,
			 "m_sandbox",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "sfo_sandbox"));
#endif

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_event,
			 "keyevent_ml",
			 { "ev", "ml", "mn", "type", "ch", "button", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "int", "int","float" },
			 { "ev", "", "", "768", "-1", "-1","10.0" },
			 "ML", "move_api", "key_event"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::wasd,
			 "wasd_ml",
			 { "ev", "ml", "w", "a", "s", "d", "duration" },
			 { "EveryApi&", "ML", "MN", "MN", "MN", "MN","float" },
			 { "ev", "",  "", "", "", "", "10.0" },
			 "ML", "move_api", "wasd"));
#endif
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_mesh,
			 "kf_mesh",
			 { "part" },
			 { "P" },
			 { "" },
			 "KF", "vertex_anim_api", "keyframe_mesh"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_lines,
			 "kf_lines",
			 { "part" },
			 { "LI" },
			 { "" },
			 "KF", "vertex_anim_api", "keyframe_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_bind,
			 "kf_bind",
			 { "ev", "keyframe", "transform", "delta_time" },
			 { "EveryApi&", "KF", "PTT", "float" },
			 { "ev", "", "", "10.0" },
			 "KF", "vertex_anim_api", "keyframe_bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::repeat_keyframes,
			 "kf_repeat",
			 { "keyframe", "count" },
			 { "KF", "int" },
			 { "", "100" },
			 "KF", "vertex_anim_api", "repeat_keyframes"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::curve_trans,
			 "kf_curve",
			 { "ev", "keyframe", "curve", "pos", "numsamples", "duration" },
			 { "EveryApi&", "KF", "C", "CPP", "int", "float" },
			 { "ev", "", "", "", "10", "30.0" },
			 "KF", "vertex_anim_api", "curve_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::sample_rot,
			 "kf_rot",
			 { "ev", "kf", "nx", "ny", "nz", "angle", "numsamples", "duration" },
			 { "EveryApi&", "KF", "float", "float", "float", "float", "int", "float" },
			 { "ev", "", "0.0", "1.0", "0.0", "6.282", "15", "30.0" },
			 "KF", "vertex_anim_api", "sample_rot"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::empty_trans,
			 "ptt_empty",
			 { },
			 { },
			 { },
			 "PTT", "vertex_anim_api", "empty_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::translate_trans,
			 "ptt_trans_s",
			 { "prev", "speed_x", "speed_y", "speed_z" },
			 { "PTT", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PTT", "vertex_anim_api", "translate_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::translate_trans2,
			 "ptt_trans_d",
			 { "prev", "duration", "dist_x", "dist_y", "dist_z" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "10.0", "0.0", "0.0", "0.0" },
			 "PTT", "vertex_anim_api", "translate_trans2"));
#if 0
  // rotation doesnt work in vertex anim, since vertex anim
  // uses time ranges + linear interpolation inside the range
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::rotate_trans,
			 "ptt_rotate_s",
			 { "prev", "nx", "ny", "nz", "speed_angle" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "0.0", "1.0", "0.0", "6.282" },
			 "PTT", "vertex_anim_api", "rotate_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::rotate_trans2,
			 "ptt_rotate_d",
			 { "prev", "duration", "nx", "ny", "nz", "dist_angle" },
			 { "PTT", "float", "float", "float", "float", "float" },
			 { "", "10.0", "0.0", "1.0", "0.0", "6.282" },
			 "PTT", "vertex_anim_api", "rotate_trans2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::scale_trans,
			 "ptt_scale_s",
			 { "prev", "speed_x", "speed_y", "speed_z" },
			 { "PTT", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "PTT", "vertex_anim_api", "scale_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::scale_trans2,
			 "ptt_scale_d",
			 { "prev", "duration", "dist_x", "dist_y", "dist_z" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "10.0", "1.0", "1.0", "1.0" },
			 "PTT", "vertex_anim_api", "scale_trans2"));
#endif
  return vec;
}
