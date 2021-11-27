#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> all_functions();
std::vector<GameApiItem*> polydistfield_functions();
std::vector<GameApiItem*> waveform_functions();
std::vector<GameApiItem*> blocker_functions();
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> volumeapi_functions();
std::vector<GameApiItem*> floatvolumeapi_functions();
std::vector<GameApiItem*> colorvolumeapi_functions();
std::vector<GameApiItem*> vectorapi_functions();
std::vector<GameApiItem*> pointapi_functions();
std::vector<GameApiItem*> fontapi_functions();
std::vector<GameApiItem*> moveapi_functions();
std::vector<GameApiItem*> polygonapi_functions();
std::vector<GameApiItem*> shadermoduleapi_functions();
std::vector<GameApiItem*> framebuffermoduleapi_functions();
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> booleanopsapi_functions();
std::vector<GameApiItem*> polygonapi_functions1();
std::vector<GameApiItem*> polygonapi_functions2();
std::vector<GameApiItem*> linesapi_functions();
std::vector<GameApiItem*> pointsapi_functions();
std::vector<GameApiItem*> floatbitmapapi_functions();
std::vector<GameApiItem*> boolbitmapapi_functions();
std::vector<GameApiItem*> bitmapapi_functions();

std::vector<GameApiItem*> floatvolumeapi_functions()
{
  std::vector<GameApiItem*> vec;
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::from_volume,
			 "fo_from_volume",
			 { "o", "false_val", "true_val" },
			 { "O", "float", "float" },
			 { "", "0.0", "1.0" },
			 "FO", "float_volume_api", "from_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::waveform_sphere,
			 "wv_sphere",
			 { "wave", "r" },
			 { "WV", "float" },
			 { "", "200.0" },
			 "FO", "float_volume_api", "waveform_sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::from_float_bitmap,
			 "fo_from_fbm",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "FB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "FO", "float_volume_api", "from_float_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::move,
			 "fo_move",
			 { "f1", "dx", "dy", "dz" },
			 { "FO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FO", "float_volume_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::minimum,
			 "fo_min",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO", "float_volume_api", "minimum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::maximum,
			 "fo_max",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO", "float_volume_api", "maximum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::min_distance,
			 "fo_min_distance",
			 { "curve" },
			 { "C" },
			 { "" },
			 "FO", "float_volume_api", "min_distance"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::subvolume,
			 "fo_subvolume",
			 { "f", "start_range", "end_range" },
			 { "FO", "float", "float" },
			 { "", "0.5", "1.0" },
			 "O", "float_volume_api", "subvolume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::interpolate,
			 "fo_interpolate",
			 { "f1", "f2", "val" },
			 { "FO", "FO", "float" },
			 { "", "", "0.5" },
			 "FO", "float_volume_api", "interpolate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::smooth,
			 "fo_smooth",
			 { "vec", "val" },
			 { "[FO]", "float" },
			 { "", "0.01" },
			 "FO", "float_volume_api", "smooth"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal2,
			 "fo_normal",
			 { "fo", "stepsize" },
			 { "FO", "float" },
			 { "", "0.1" },
			 "VO", "vector_volume_api", "normal2"));
#if 0
  // doesnt work with threads
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::integrate_render,
			 "fo_itg_render",
			 { "obj", "sx", "sy", "numsamples" },
			 { "FO", "int", "int", "int" },
			 { "", "800", "600", "3" },
			 "FB", "float_volume_api", "integrate_render"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::setup_normal,
			 "setup_normal",
			 { "orig", "v" },
			 { "P", "VO" },
			 { "", "" },
			 "P", "vector_volume_api", "setup_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_sphere,
			 "fd_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "FD", "dist_api", "fd_sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_cube,
			 "fd_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "FD", "dist_api", "fd_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_round_cube,
			 "fd_round_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r" },
			 { "float", "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "10.0" },
			 "FD", "dist_api", "fd_round_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_torus,
			 "fd_torus",
			 { "r_1", "r_2" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "fd_torus"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::cone,
			 "fd_cone",
			 { "c_x", "c_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::plane,
			 "fd_plane",
			 { "n_x", "n_y", "n_z", "n_w" },
			 { "float", "float", "float", "float" },
			 { "0.0", "1.0", "0.0", "1.0" },
			 "FD", "dist_api", "plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::hex_prism,
			 "fd_hex_prism",
			 { "h_x", "h_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "hex_prism"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::tri_prism,
			 "fd_tri_prism",
			 { "h_x", "h_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "tri_prism"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::triangle,
			 "fd_triangle",
			 { "a", "b", "c" },
			 { "PT", "PT", "PT" },
			 { "", "", ""},
			 "FD", "dist_api", "triangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::quad,
			 "fd_quad",
			 { "a", "b", "c", "d" },
			 { "PT","PT", "PT", "PT" },
			 { "", "", "", "" },
			 "FD", "dist_api", "quad"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_line,
			 "fd_line",
			 { "start", "end", "dist" },
			 { "PT", "PT", "float" },
			 { "", "", "10.0" },
			 "FD", "dist_api", "fd_line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_min,
			 "fd_min",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_min"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_max,
			 "fd_max",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_max"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_and_not,
			 "fd_and_not",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_and_not"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_blend,
			 "fd_blend",
			 { "a1", "a2", "k" },
			 { "FD", "FD", "float" },
			 { "", "", "15.0" },
			 "FD", "dist_api", "fd_blend"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_color,
			 "fd_color",
			 { "fd", "r", "g", "b", "a" },
			 { "FD", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "FD", "dist_api", "fd_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_trans,
			 "fd_trans",
			 { "fd", "dx", "dy", "dz" },
			 { "FD", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FD", "dist_api", "fd_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_x,
			 "fd_rot_x",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_y,
			 "fd_rot_y",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_z,
			 "fd_rot_z",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_z"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal,
			 "fd_normal",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "VO", "vector_volume_api", "normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::recalculate_normals,
			 "fd_recalc_normals",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "FD", "dist_api", "recalculate_normals"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::ambient_occulsion,
			 "fd_amb_occul",
			 { "fd", "d", "i" },
			 { "FD", "float", "float" },
			 { "", "10.2", "30.0" },
			 "FD", "dist_api", "ambient_occulsion","[B]", "Broken"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render2,
			 "fd_render2",
			 { "ev", "obj", "sx", "sy" },
			 { "EveryApi&", "FD", "int", "int" },
			 { "ev", "", "300", "300" },
			 "BM", "dist_api", "render2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render,
			 "fd_render_bm",
			 { "obj", "pos", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "PT", "V", "V", "V", "int", "int" },
			 { "", "", "", "", "", "100", "100" },
			 "BM", "dist_api", "render"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly,
			 "fd_render_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "400.0", "400.0", "50", "50", "100.0", "100.0" },
			 "P", "dist_api", "distance_poly","[B]","Sometimes gives bad results"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_cyl,
			 "fd_render_cyl_p",
			 { "ev", "fd", "pos_x", "pos_y", "pos_z", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "0.0", "-600.0", "0.0", "300.0", "1200.0", "300.0", "50", "50", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_cyl","[B]", "Sometimes gives bad result"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_sph,
			 "fd_render_sph_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_sph", "[B]", "Sometimes gives bad result"));
#endif
  
  return vec;
}
