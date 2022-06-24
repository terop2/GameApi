#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

 std::vector<GameApiItem*> boolbitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::bb_empty,
			 "emptybool",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "BB", "bool_bitmap_api", "bb_empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::from_bitmaps_color,
			 "bb_from_bitmap",
			 { "bm", "r", "g", "b" },
			 { "BM", "int", "int", "int" },
			 { "", "255", "255", "255" },
			 "BB", "bool_bitmap_api", "from_bitmaps_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::from_float_bitmap,
			 "from_float_bitmap",
			 { "float_bm", "range_start", "range_end" },
			 { "FB", "float", "float" },
			 { "", "0.5", "1.0" },
			 "BB", "bool_bitmap_api", "from_float_bitmap"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::line,
			 "line",
			 { "bg", "p_x", "p_y", "p2_x", "p2_y", "line_width1", "line_width2" },
			 { "BB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "100.0", "100.0", "3.0", "3.0" },
			 "BB", "bool_bitmap_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::tri,
			 "tri",
			 { "orig", "p1_x", "p1_y", "p2_x", "p2_y", "p3_x", "p3_y" },
			 { "BB", "float", "float", "float", "float", "float", "float" },
			 { "", "0", "0", "100", "30", "50", "100" },
			 "BB", "bool_bitmap_api", "tri"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::circle,
			 "circle",
			 { "bg", "center_x", "center_y", "radius" },
			 { "BB", "float", "float", "float" },
			 { "", "50.0", "50.0", "50.0" },
			 "BB", "bool_bitmap_api", "circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::rectangle,
			 "rectangle",
			 { "bg", "x", "y", "width", "height" },
			 { "BB", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "30.0", "30.0" },
			 "BB", "bool_bitmap_api", "rectangle"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::part_circle,
			 "part_circle",
			 { "sx", "sy", "x", "y", "start_angle", "end_angle", "start_rad", "end_rad" },
			 { "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "100", "100", "50.0", "50.0", "0.0", "1.0", "30.0", "50.0" },
			 "BB", "bool_bitmap_api", "part_circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::rings,
			 "rings",
			 { "sx", "sy", "center_x_start", "center_y_start", "center_x_end", "center_y_end", "start_radius", "end_radius", "start_thickness", "end_thickness", "numrings" },
			 { "int", "int", "float", "float", "float", "float", "float", "float", "float", "float", "int" },
			 { "300", "300", "150", "150", "150", "150", "10", "180", "30", "30", "20" },
			 "BB", "bool_bitmap_api", "rings"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::not_bitmap,
			 "not_bitmap",
			 { "b" },
			 { "BB" },
			 { "" },
			 "BB", "bool_bitmap_api", "not_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::andnot_bitmap,
			 "andnot_bitmap",
			 { "b1", "not_b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "andnot_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::or_bitmap,
			 "or_bitmap",
			 { "b1", "b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "or_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::xor_bitmap,
			 "xor_bitmap",
			 { "b1", "flip_b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "xor_bitmap"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::choose_bitmap,
			 "choose_bitmap",
			 { "bools", "true_bm", "false_bm" },
			 { "BB", "BB", "BB" },
			 { "", "", "" },
			 "BB", "bool_bitmap_api", "choose_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::sprite,
			 "bb_sprite",
			 { "bg", "sprite", "x", "y", "mult_x", "mult_y" },
			 { "BB", "BB", "float", "float", "float", "float" },
			 { "", "", "0.0", "0.0", "1.0", "1.0" },
			 "BB", "bool_bitmap_api", "sprite"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::to_bitmap,
			 "to_bitmap",
			 { "bools", "true_r", "true_g", "true_b", "true_a", "false_r", "false_g", "false_b", "false_a" },
			 { "BB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "bool_bitmap_api", "to_bitmap"));

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::black_white_dithering,
			 "fb_dither",
			 { "float_bitmap" },
			 { "FB" },
			 { "" },
			 "BB", "bool_bitmap_api", "black_white_dithering"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::create_ibm,
			 "bb_ibm",
			 { "vec" },
			 { "[BB]" },
			 { "" },
			 "IBM", "bitmap_api", "create_ibm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_bool,
			 "ibm_bb",
			 { "bm", "val" },
			 { "IBM", "int" },
			 { "", "1" },
			 "BB", "bitmap_api", "choose_bool"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_ints,
			 "ibm_bbs",
			 { "bm", "count" },
			 { "IBM", "int" },
			 { "", "2" },
			 "[BB]", "bitmap_api", "choose_ints"));
#endif
  return vec;
}
