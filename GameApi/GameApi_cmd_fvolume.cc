#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> all_functions(GameApi::EveryApi &ev);
std::vector<GameApiItem*> polydistfield_functions();
std::vector<GameApiItem*> waveform_functions();
std::vector<GameApiItem*> blocker_functions(GameApi::EveryApi &ev);
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

#if 0
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::colourspace_sphere2,
			 "cs_sphere",
			 { },
			 { },
			 { },
			 "CS", "polygon_api", "colourspace_sphere2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::colourspace_or_elem,
			 "cs_or_elem",
			 { "cs1", "delta_t", "cs2", "delta_t2" },
			 { "CS", "float", "CS", "float" },
			 { "", "0.0", "", "0.0" },
			 "CS", "polygon_api", "colourspace_or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::colourspace_sample,
			 "cs_sample",
			 { "cs", "sx", "sy", "sz" },
			 { "CS", "int", "int", "int" },
			 { "", "30", "30", "30" },
			 "CSI", "polygon_api", "colourspace_sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::colourspace_facecoll,
			 "cs_faces",
			 { "csi", "t" },
			 { "CSI", "float" },
			 { "", "350.0" },
			 "P", "polygon_api", "colourspace_facecoll"));


#endif
  
#if 0
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::light,
			 "fo_light",
			 { "dist" },
			 { "float" },
			 { "600.0" },
			 "FO", "float_volume_api", "light"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal2,
			 "fo_normal",
			 { "fo", "stepsize" },
			 { "FO", "float" },
			 { "", "0.1" },
			 "VO", "vector_volume_api", "normal2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::julia,
			 "fo_julia",
			 { "c_x", "c_y", "limit" },
			 { "float", "float", "float" },
			 { "0.4", "0.233", "5.0" },
			 "FO", "float_volume_api", "julia"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::mandelbrot,
			 "fo_mandelbrot",
			 { "x_x", "x_y", "limit" },
			 { "float", "float", "float" },
			 { "0.4", "0.233", "5.0" },
			 "FO", "float_volume_api", "mandelbrot"));
  
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

#if (ALL==1)||(FD_LINE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_line,
			 "fd_line",
			 { "start", "end", "dist" },
			 { "PT", "PT", "float" },
			 { "", "", "10.0" },
			 "FD", "dist_api", "fd_line"));
#endif
#if (ALL==1)||(FD_MIN==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_min,
			 "fd_min",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_min"));
#endif
#if (ALL==1)||(FD_MAX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_max,
			 "fd_max",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_max"));
#endif
#if (ALL==1)||(FD_AND_NOT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_and_not,
			 "fd_and_not",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "fd_and_not"));
#endif
#if (ALL==1)||(FD_BLEND==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_blend,
			 "fd_blend",
			 { "a1", "a2", "k" },
			 { "FD", "FD", "float" },
			 { "", "", "15.0" },
			 "FD", "dist_api", "fd_blend"));
#endif
#if (ALL==1)||(FD_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_color,
			 "fd_color",
			 { "fd", "r", "g", "b", "a" },
			 { "FD", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "FD", "dist_api", "fd_color"));
#endif
#if (ALL==1)||(FD_TRANS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_trans,
			 "fd_trans",
			 { "fd", "dx", "dy", "dz" },
			 { "FD", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FD", "dist_api", "fd_trans"));
#endif
#if (ALL==1)||(FD_ROT_X==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_x,
			 "fd_rot_x",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_x"));
#endif
#if (ALL==1)||(FD_ROT_Y==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_y,
			 "fd_rot_y",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_y"));
#endif
#if (ALL==1)||(FD_ROT_Z==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::fd_rot_z,
			 "fd_rot_z",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "fd_rot_z"));
#endif
#if (ALL==1)||(NORMAL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal,
			 "fd_normal",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "VO", "vector_volume_api", "normal"));
#endif
#if (ALL==1)||(RECALCULATE_NORMALS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::recalculate_normals,
			 "fd_recalc_normals",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "FD", "dist_api", "recalculate_normals"));
#endif
#if (ALL==1)||(AMBIENT_OCCULSION==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::ambient_occulsion,
			 "fd_amb_occul",
			 { "fd", "d", "i" },
			 { "FD", "float", "float" },
			 { "", "10.2", "30.0" },
			 "FD", "dist_api", "ambient_occulsion","[B]", "Broken"));
#endif
#if (ALL==1)||(RENDER2==1) 
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render2,
			 "fd_render2",
			 { "ev", "obj", "sx", "sy" },
			 { "EveryApi&", "FD", "int", "int" },
			 { "ev", "", "300", "300" },
			 "BM", "dist_api", "render2"));
#endif
#if (ALL==1)||(RENDER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render,
			 "fd_render_bm",
			 { "obj", "pos", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "PT", "V", "V", "V", "int", "int" },
			 { "", "", "", "", "", "100", "100" },
			 "BM", "dist_api", "render"));
#endif
#if (ALL==1)||(DISTANCE_POLY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly,
			 "fd_render_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "400.0", "400.0", "50", "50", "100.0", "100.0" },
			 "P", "dist_api", "distance_poly","[B]","Sometimes gives bad results"));
#endif
#if (ALL==1)||(DISTANCE_POLY_CYL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_cyl,
			 "fd_render_cyl_p",
			 { "ev", "fd", "pos_x", "pos_y", "pos_z", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "0.0", "-600.0", "0.0", "300.0", "1200.0", "300.0", "50", "50", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_cyl","[B]", "Sometimes gives bad result"));
#endif
#if (ALL==1)||(DISTANCE_POLY_SPH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_sph,
			 "fd_render_sph_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_sph", "[B]", "Sometimes gives bad result"));
#endif
#endif
#endif  
  return vec;
}
