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


std::vector<GameApiItem*> volumeapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::volumeprojection,
			 "o_vrender",
			 { "o", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy", "numsamples" },
			 { "O", "float", "float", "float", "float", "float", "float", "int", "int", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "256", "256", "20" },
			 "BB", "volume_api", "volumeprojection"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::from_bool_bitmap,
			 "o_from_bool_bitmap",
			 { "b", "dist" },
			 { "BB", "float" },
			 { "", "50.0" },
			 "O", "volume_api", "from_bool_bitmap"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::from_polygon,
			 "o_from_mesh",
			 { "poly", "x", "y", "z" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "O", "volume_api", "from_polygon"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::o_sphere,
			 "o_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "O", "volume_api", "o_sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::o_cube,
			 "o_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "O", "volume_api", "o_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::o_cone,
			 "o_cone",
			 { "p1", "p2", "rad1", "rad2" },
			 { "PT", "PT", "float", "float" },
			 { "", "", "100.0", "90.0" },
			 "O", "volume_api", "o_cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::o_torus,
			 "o_torus",
			 { "center", "u_x", "u_y", "dist1", "dist2" },
			 { "PT", "PT", "PT", "float", "float" },
			 { "", "", "", "100.0", "50.0" },
			 "O", "volume_api", "o_torus"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::mandelbrot_volume,
			 "o_mandelbrot",
			 { "julia", "count", "yy" },
			 { "bool", "int", "float" },
			 { "false", "64", "0.0" },
			 "O", "volume_api", "mandelbrot_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::mandelbulb,
			 "o_mandelbulb",
			 { "n", "p_x", "p_y", "p_z", "c_x", "c_y", "c_z", "radius", "iterations" },
			 { "float", "float", "float", "float", "float", "float", "float", "float", "int" },
			 { "2.5,", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "100.0", "64" },
			 "O", "volume_api", "mandelbulb"));
			   
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::colour,
			 "o_colour",
			 { "object", "color" },
			 { "O", "unsigned int" },
			 { "", "ffffffff" },
			 "O", "volume_api", "colour"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::subset_color,
			 "o_subset_color",
			 { "model", "color_subset", "color" },
			 { "O", "O", "unsigned int" },
			 { "", "", "ffffffff" },
			 "O", "volume_api", "subset_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::move,
			 "o_move",
			 { "obj", "dx", "dy", "dz" },
			 { "O", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "O", "volume_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatex,
			 "o_rotatex",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatey,
			 "o_rotatey",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatey"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatez,
			 "o_rotatez",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatez"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::scale,
			 "o_scale",
			 { "object", "sx", "sy", "sz" },
			 { "O", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "O", "volume_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::not_op,
			 "o_notop",
			 { "object" },
			 { "O" },
			 { "" },
			 "O", "volume_api", "not_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::min_op,
			 "o_minop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "min_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::max_op,
			 "o_maxop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "max_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::andnot_op,
			 "o_andnot",
			 { "object1", "not_obj" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "andnot_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::instancing_volume,
			 "o_inst",
			 { "volume", "pts" },
			 { "O", "PTS" },
			 { "", "" },
			 "O", "volume_api", "instancing_volume"));
#if 0
  // keeps crashing
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts2",
			 { "object", "size", "wholesize" },
			 { "O", "int", "float" },
			 { "", "30", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));
#endif

#ifndef STABLE

#if 0
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::im_sphere,
			 "im_sphere",
			 { "r" },
			 { "float" },
			 { "100.0" },
			 "IM", "implicit_api", "im_sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::blob,
			 "im_blob",
			 { "c", "c_x", "c_y", "cc_x", "cc_y" },
			 { "float", "float", "float", "float", "float" },
			 { "100.0", "0.0", "0.0", "60.0", "0.0" },
			 "IM", "implicit_api", "blob","[B]", "Broken"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::im_translate,
			 "im_translate",
			 { "obj", "dx", "dy", "dz" },
			 { "IM", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "IM", "implicit_api", "im_translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance,
			 "im_from_distance",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance_cyl,
			 "im_distance_cyl",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance_cyl"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance_sph,
			 "im_distance_sph",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance_sph"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_upper,
			 "im_render_upper",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float", "float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "FB", "implicit_api", "render_upper"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_lower,
			 "im_render_lower",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float","float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "FB", "implicit_api", "render_lower"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_upper_color,
			 "im_upper_color",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float", "float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "BM", "implicit_api", "render_upper_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_lower_color,
			 "im_lower_color",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float","float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "BM", "implicit_api", "render_lower_color"));

#endif
  
#endif
  return vec;
}
