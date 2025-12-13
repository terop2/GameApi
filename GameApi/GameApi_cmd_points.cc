#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
 std::vector<GameApiItem*> pointsapi_functions()
{
  std::vector<GameApiItem*> vec;
#if (ALL==1)||(LOAD_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::load_points,
			 "pts_load",
			 { "url" },
			 { "std::string" },
			 { "https://meshpage.org/test.pts" },
			 "PTS", "points_api", "load_points"));
  
#endif
#if (ALL==1)||(PTS_ALT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_alt,
			 "pts_alt",
			 { "vec", "index" },
			 { "[PTS]", "int" },
			 { "", "0" },
			 "PTS", "points_api", "pts_alt"));
#endif
#ifndef STABLE
#if (ALL==1)||(PLY_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::ply_pts,
			 "pts_ply",
			 { "url" },
			 { "std::string" },
			 { "http://meshpage.org/assets/test.ply@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "PTS", "points_api", "ply_pts"));
#endif
#endif
#if (ALL==1)||(PT_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pt_array,
			 "pt_array",
			 { "ev", "vec" },
			 { "EveryApi&", "[PT]" },
			 { "ev", "" },
			 "PTS", "points_api", "pt_array"));
#endif
#ifndef STABLE
#if (ALL==1)||(FROM_FLOAT_VOLUME==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::from_float_volume,
			 "from_float_volume",
			 { "volume", "numpoints", "start_x", "start_y", "start_z", "end_x", "end_y", "end_z" },
			 { "FO", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "1000", "0.0", "0.0", "0.0", "100.0", "100.0", "100.0" },
			 "PTS", "points_api", "from_float_volume"));
#endif
#endif
#if (ALL==1)||(OR_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::or_points,
			 "or_points",
			 { "p1", "p2" },
			 { "PTS", "PTS" },
			 { "", "" },
			 "PTS", "points_api", "or_points"));
#endif
#ifndef STABLE
#if (ALL==1)||(CONVEX_HULL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::convex_hull,
			 "pts_convexhull",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "P", "polygon_api", "convex_hull"));
#endif
#endif
#if (ALL==1)||(PTS_LINES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_lines,
			 "pts_lines",
			 { "url", "start_pos", "dist", "speed" },
			 { "std::string", "float", "float", "float" },
			 { "https://meshpage.org/lines.txt", "0.0", "100.0", "3.0" },
			 "PTS", "points_api", "pts_lines"));
#endif
#if (ALL==1)||(MOVE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::move,
			 "move_pts",
			 { "obj", "dx", "dy", "dz" },
			 { "PTS", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PTS", "points_api", "move"));
#endif
#if (ALL==1)||(HEIGHT_FIELD_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::height_field_pts,
			 "heightfield_pts",
			 { "points", "landscape", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "PTS", "CFB", "float", "float", "float", "float", "float", "float" },
			 { "", "", "-1000.0", "1000.0", "-300.0", "300.0", "-3000.0", "1000.0" },
			 "PTS", "points_api", "height_field_pts"));
#endif
#if (ALL==1)||(BULLET==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::bullet,
			 "bullet_pts",
			 { "key", "vx", "vy", "vz", "px", "py", "pz", "dist", "max_count", "repeat_time" },
			 { "int", "float", "float", "float", "float", "float", "float", "float", "int", "float" },
			 { "32", "30.0", "0.0", "0.0", "0.0", "0.0", "0.0", "600.0", "15", "200.0" },
			 "PTS", "points_api", "bullet"));
#endif
#if (ALL==1)||(ANIM_MIX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::anim_mix,
			 "anim_mix_pts",
			 { "obj1", "obj2", "start_val", "end_val", "start_time", "end_time" },
			 { "PTS", "PTS", "float", "float", "float", "float" },
			 { "", "", "0.0", "1.0", "10.0", "40.0" },
			 "PTS", "points_api", "anim_mix"));
#endif
#ifndef STABLE
#if (ALL==1)||(SCORE_HIDE_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_hide_pts,
			 "score_hide_pts",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "PTS", "mainloop_api", "score_hide_pts"));
#endif
#if (ALL==1)||(SCORE_SET_HIDE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_set_hide,
			 "score_set_hide",
			 { },
			 { },
			 { },
			 "IF", "mainloop_api", "score_set_hide"));
#endif
#if (ALL==1)||(SORT_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::sort_pts,
			 "sort_pts",
			 { "points" },
			 { "PTS" },
			 { "" },
			 "PTS", "points_api", "sort_pts"));
#endif
#endif
#if (ALL==1)||(ROT_X==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_x,
			 "rot_x_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_x"));
#endif
#if (ALL==1)||(ROT_Y==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_y,
			 "rot_y_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_y"));
#endif
#if (ALL==1)||(ROT_Z==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_z,
			 "rot_z_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_z"));
#endif
#if (ALL==1)||(ANIM_ROT_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::anim_rot_pts,
			 "anim_rot_pts",
			 { "pts", "start_time", "end_time", "v_x", "v_y", "v_z", "rotate_amount" },
			 { "PTS", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "1.0", "0.0", "6.282" },
			 "PTS", "points_api", "anim_rot_pts"));
#endif
#if (ALL==1)||(SCALE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::scale,
			 "scale_pts",
			 { "obj", "sx", "sy", "sz" },
			 { "PTS", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "PTS", "points_api", "scale"));
#endif
#if (ALL==1)||(COLOR_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::color_points,
			 "color_pts",
			 { "p", "color" },
			 { "PTS", "unsigned int" },
			 { "", "ffffffff" },
			 "PTS", "points_api", "color_points"));
#endif
#if (ALL==1)||(POLYGON_FACE_CENTER_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::polygon_face_center_points,
			 "face_center",
			 { "p" },
			 { "P" },
			 { "" },
			 "PTS", "points_api", "polygon_face_center_points"));
#endif
#if (ALL==1)||(POINTS_FIELD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::points_field,
			 "points_field",
			 { "start_speed_y", "end_speed_y", "numpoints", "start_x", "end_x", "start_y", "end_y" },
			 { "float", "float", "int", "float", "float", "float", "float" },
			 { "3.0", "30.0", "100", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "points_api", "points_field"));
#endif
#ifndef STABLE
#if (ALL==1)||(HEMISPHERE_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::hemisphere_points,
			 "hemisphere_pts",
			 { "points", "normal", "r", "numpoints" },
			 { "PT", "V", "float", "int" },
			 { "", "", "100.0", "100" },
			 "PTS", "points_api", "hemisphere_points"));
#endif
#if (ALL==1)||(WAVE_POINTS==1)
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::wave_points,
			 "wave_pts",
			 { "wave", "num_samples", "pos_x", "pos_y", "pos_z",
			     "u_x_x", "u_x_y", "u_x_z",
			     "u_y_x", "u_y_y", "u_y_z" },
			 { "WV", "int", "float", "float", "float",
			     "float", "float", "float",
			     "float", "float", "float" },
			 { "", "50", "-300.0", "-300.0", "0.0", "600.0", "0.0", "0.0", "0.0", "600.0", "0.0" },
			 "PTS", "points_api", "wave_points"));
#endif
#endif
#if (ALL==1)||(FILTER_COMPONENT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::filter_component,
			 "filter_pts",
			 { "pts", "comp", "val" },
			 { "PTS", "int", "float" },
			 { "", "2", "0.0" },
			 "PTS", "points_api", "filter_component"));
#endif
#if (ALL==1)||(FROM_VOLUME==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::from_volume,
			 "from_volume",
			 { "o", "pos", "u_x", "u_y", "u_z", "sx", "sy", "sz" },
			 { "O", "PT", "V", "V", "V", "int", "int", "int" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,100.0,0.0)", "(0.0,0.0,100.0)", "100", "100", "100" },
			 "PTS", "points_api", "from_volume"));
#endif
#if (ALL==1)||(SHADOW_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::shadow_points,
			 "shadow_points",
			 { "obj", "pos", "u_x", "u_y", "light_vec" },
			 { "PTS", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,0.0,100.0)", "(1.0,1.0,0.0)" },
			 "PTS", "points_api", "shadow_points"));
#endif
#if (ALL==1)||(RANDOM_PLANE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_plane,
			 "random_plane",
			 { "pos", "u_x", "u_y", "numpoints" },
			 { "PT", "V", "V", "int" },
			 { "", "", "", "100" },
			 "PTS", "points_api", "random_plane"));
#endif
#if (ALL==1)||(RANDOM_BITMAP_INSTANCING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_bitmap_instancing,
			 "random_bitmap",
			 { "ev", "bb", "count", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BB", "int", "float", "float", "float", "float", "float" },
			 { "ev", "", "300", "-300.0", "300.0", "-300.0", "300.0", "0.0"},
			 "PTS", "points_api", "random_bitmap_instancing"));
#endif
#endif
#if (ALL==1)||(RANDOM_MESH_QUAD_INSTANCING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_mesh_quad_instancing,
			 "random_quad",
			 { "ev", "p", "count" },
			 { "EveryApi&", "P", "int" },
			 { "ev", "", "300" },
			 "PTS", "points_api", "random_mesh_quad_instancing"));
#endif
#if (ALL==1)||(RANDOM_POINTS_IN_PLANE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_points_in_plane,
			 "perlin_plane",
			 { "ev", "key", "start_x", "end_x", "start_z", "end_z", "y", "num" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "int" },
			 { "ev", "0.5", "-300.0", "300.0", "-300.0", "300.0", "0.0", "100" },
			 "PTS", "points_api", "random_points_in_plane"));
#endif

#ifndef STABLE
#if (ALL==1)||(RANDOM_VOL_OBJECT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::random_vol_object,
			 "random_volume",
			 { "o", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "numpoints" },
			 { "O", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "10000" },
			 "PTS", "volume_api", "random_vol_object"));
#endif
#if (ALL==1)||(PTS_GRID==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_grid,
			 "pts_grid",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "PTS", "points_api", "pts_grid"));
#endif
#if (ALL==1)||(PTS_GRID_BB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_grid_bb,
			 "pts_grid_bb",
			 { "bb", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "PTS", "points_api", "pts_grid_bb"));
#endif
#if (ALL==1)||(MEMOIZE_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::memoize_pts,
			 "pts_memoize",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "PTS", "points_api", "memoize_pts"));
#endif
#if (ALL==1)||(ITERATE_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::iterate_points,
			 "pts_field",
			 { "points", "field", "speed" },
			 { "PTS", "VFi", "FA" },
			 { "", "", "" },
			 "PTS", "points_api", "iterate_points"));
#endif
#if (ALL==1)||(PRESSURE_GRADIENT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pressure_gradient,
			 "pressure_gradient",
			 { "pressure" },
			 { "FFi" },
			 { "" },
			 "VFi", "points_api", "pressure_gradient"));
#endif
#if (ALL==1)||(MATRIX_FIELD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::matrix_field,
			 "mat_field",
			 { "start", "end" },
			 { "MN", "MN" },
			 { "", "" },
			 "VFi", "points_api", "matrix_field"));
#endif
#if (ALL==1)||(RANDOM_SPEEDS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_speeds,
			 "rand_array",
			 { "start", "end", "num" },
			 { "float", "float", "int" },
			 { "0.5", "5.0", "300" },
			 "FA", "points_api", "random_speeds"));
#endif
#if (ALL==1)||(COLLISION_POINTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_points,
			 "pts_bounding_box",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "points_api", "collision_points"));
#endif  
#endif
#if (ALL==1)||(COLLISION_BIND==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_bind,
			 "pts_collision_bind",
			 { "ev","bounding_box", "name" },
			 { "EveryApi&","PTS", "std::string" },
			 { "ev", "", "obj1" },
			 "ML", "points_api", "collision_bind"));
#endif
#if (ALL==1)||(COLLISION_BIND_INST==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_bind_inst,
			 "pts_coll_bind_inst",
			 { "ev","bounding_box", "inst_points", "name" },
			 { "EveryApi&","PTS", "PTS", "std::string" },
			 { "ev", "", "", "obj1" },
			 "ML", "points_api", "collision_bind_inst"));
#endif
#if (ALL==1)||(COLLISION_COLLECT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_collect,
			 "collidion_collect",
			 { "mainloop" },
			 { "ML" },
			 { "" },
			 "ML", "points_api", "collision_collect"));
#endif
#if (ALL==1)||(COLLISION_SEQ_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_seq_ml,
			 "collision_seq_ml",
			 { "curr", "end", "obj1", "obj2", "show_duration" },
			 { "ML", "ML", "std::string", "std::string", "float" },
			 { "", "", "obj1", "obj2", "100.0" },
			 "ML", "mainloop_api", "collision_seq_ml"));
#endif
#if (ALL==1)||(COLLISION_GEN_KEY==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_gen_key,
			 "collision_gen_key",
			 { "curr", "obj1", "obj2", "key", "keypress_duration" },
			 { "ML", "std::string", "std::string", "int", "float" },
			 { "", "obj1", "obj2", "32", "10.0" },
			 "ML", "mainloop_api", "collision_gen_key"));
#endif
#if (ALL==1)||(FROM_POINTS==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_points,
			 "ms_from_points",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "MS", "matrices_api", "from_points"));
#endif
#if (ALL==1)||(MULT_ARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::mult_array,
			 "ms_mult",
			 { "m1", "m2" },
			 { "MS", "MS" },
			 { "","" },
			 "MS", "matrices_api", "mult_array"));
#endif
#if (ALL==1)||(REPEAT_MS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::repeat_ms,
			 "ms_repeat",
			 { "ev", "mn", "count" },
			 { "EveryApi&", "MN", "int" },
			 { "ev", "", "10" },
			 "MS", "matrices_api", "repeat_ms"));
#endif
#ifndef STABLE
#if (ALL==1)||(MS_RANDOM_ROT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::ms_random_rot,
			 "ms_random_rot",
			 { "px", "py", "pz", "count" },
			 { "float", "float", "float", "int" },
			 { "0.0", "1.0", "0.0", "30" },
			 "MS", "matrices_api", "ms_random_rot"));
#endif
#if (ALL==1)||(MS_STEADY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::ms_steady,
			 "ms_steady",
			 { "ms" },
			 { "MS" },
			 { "" },
			 "MS", "matrices_api", "ms_steady"));
#endif

#if (ALL==1)||(SUBARRAY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::subarray,
			 "ms_subarray",
			 { "ms", "start", "count" },
			 { "MS", "int", "int" },
			 { "", "0", "0" },
			 "MS", "matrices_api", "subarray"));
#endif
#if (ALL==1)||(FROM_LINES_2D==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_lines_2d,
			 "ms_from_lines_2d",
			 { "li" },
			 { "LI" },
			 { "" },
			 "MS", "matrices_api", "from_lines_2d"));
#endif
#if (ALL==1)||(FROM_LINES_3D==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_lines_3d,
			 "ms_from_lines_3d",
			 { "li" },
			 { "LI" },
			 { "" },
			 "MS", "matrices_api", "from_lines_3d"));
#endif
#endif

#ifndef STABLE
#if (ALL==1)||(STATIC_INSTANCING_MATRIX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_matrix,
			 "ms_static_inst",
			 { "ev", "obj", "matrices" },
			 { "EveryApi&", "P", "MS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing_matrix"));
#endif
#if (ALL==1)||(LI_STATIC_INSTANCING_MATRIX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::li_static_instancing_matrix,
			 "ms_lines_inst",
			 { "ev", "obj", "matrices" },
			 { "EveryApi&", "LI", "MS" },
			 { "ev", "", "" },
			 "LI", "polygon_api", "li_static_instancing_matrix"));
#endif
#if (ALL==1)||(TREE_MS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_ms,
			 "ms_tree",
			 { "ev", "tree", "time" },
			 { "EveryApi&", "T", "float" },
			 { "ev", "", "0.0" },
			 "MS", "tree_api", "tree_ms"));
#endif
#endif
#if (ALL==1)||(MS_FILE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::ms_file,
			 "ms_file",
			 { "url", "use_type" },
			 { "std::string", "int" },
			 { "https://meshpage.org/assets/test.ms", "0" },
			 "MS", "matrices_api", "ms_file"));
#endif
#if (ALL==1)||(RENDER_MS_FILES_SI==1)
  

  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::render_ms_files_si,
			 "minecraft_render_vx",
			 { "ev", "default_cubes", "default_material", "voxel", "start_type", "end_type", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "EveryApi&", "[P]", "MT", "VX", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "", "0", "1", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "ML", "matrices_api", "render_ms_files_si"));
#endif
#if (ALL==1)||(RENDER_MS_FILES2_SI==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::render_ms_files2_si,
			 "minecraft_render2_vx",
			 { "ev", "default_cubes", "default_materials", "voxel", "start_type", "end_type", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "EveryApi&", "[P]", "[MT]", "VX", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "", "0", "1", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "ML", "matrices_api", "render_ms_files2_si"));
#endif  
#if (ALL==1)||(RENDER_MS_FILES==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::render_ms_files,
			 "minecraft_render_file",
			 { "ev", "default_cubes", "default_material", "url", "start_type", "end_type" },
			 { "EveryApi&", "[P]", "MT", "std::string", "int", "int" },
			 { "ev", "", "", "https://meshpage.org/assets/test.ms", "0", "1" },
			 "ML", "matrices_api", "render_ms_files"));
#endif
#if (ALL==1)||(RENDER_MS_FILES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::render_ms_files2,
			 "minecraft_render_file2",
			 { "ev", "default_cubes", "default_materials", "url", "start_type", "end_type" },
			 { "EveryApi&", "[P]", "[MT]", "std::string", "int", "int" },
			 { "ev", "", "", "https://meshpage.org/assets/test.ms", "0", "1" },
			 "ML", "matrices_api", "render_ms_files"));
#endif
  return vec;
}
