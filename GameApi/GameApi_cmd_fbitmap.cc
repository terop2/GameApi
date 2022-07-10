#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

 std::vector<GameApiItem*> floatbitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::fb_empty,
			 "emptyfloat",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "FB", "float_bitmap_api", "fb_empty"));


#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::grayscale,
			 "grayscale",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "grayscale"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_red,
			 "from_red",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_red"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_green,
			 "from_green",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_green"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_blue,
			 "from_blue",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_blue"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_alpha,
			 "from_alpha",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_alpha"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::min_fb,
			 "min_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api", "mix_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::max_fb,
			 "max_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api","max_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::mix_fb,
			 "mix_fb",
			 { "fb1", "fb2", "val" },
			 { "FB", "FB", "float" },
			 { "", "", "0.5" },
			 "FB", "float_bitmap_api", "mix_fb"));
#endif

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient_fb,
			 "gradient_fb",
			 { "sx", "sy", "val", "val2", "flip" },
			 { "int", "int", "float", "float", "bool" },
			 { "100", "100", "0.0", "1.0", "false" },
			 "FB", "bitmap_api", "gradient_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::radial_fb,
			 "radial_fb",
			 { "sx", "sy", "x", "y", "r", "val_at_zero", "val_at_r" },
			 { "int", "int", "float", "float", "float", "float", "float" },
			 { "100", "100", "50", "50", "50", "1.0", "0.0" },
			 "FB", "bitmap_api", "radial_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::sin_fb,
			 "sin_fb",
			 { "gradient" },
			 { "FB" },
			 { "" },
			 "FB", "bitmap_api", "sin_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::plus_fb,
			 "plus_fb",
			 { "f1", "f2" },
			 { "FB", "FB" },
			 { "","" },
			 "FB", "bitmap_api", "plus_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::mul_fb,
			 "mul_fb",
			 { "f", "mul" },
			 { "FB", "float" },
			 { "", "1.0" },
			 "FB", "bitmap_api", "mul_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::dist,
			 "bm_dist",
			 { "sx", "sy", "p_x", "p_y" },
			 { "int", "int", "float", "float" },
			 { "100", "100", "50.0", "50.0" },
			 "FB", "float_bitmap_api", "dist"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::blur_bitmap,
			 "bm_blur",
			 { "fb", "d" },
			 { "FB", "float" },
			 { "", "0.5" },
			 "FB", "float_bitmap_api", "blur_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::gaussian,
			 "bm_gaussian",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "1.0", "100", "100" },
			 "FB", "float_bitmap_api", "gaussian"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::perlin_noise,
			 "bm_perlin",
			 { "grad_1", "grad_2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api", "perlin_noise"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::to_grayscale_color,
			 "to_grayscale_color",
			 { "fb", "r", "g", "b", "a", "r2", "g2", "b2", "a2" },
			 { "FB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "float_bitmap_api", "to_grayscale_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::choose_bitmap,
			 "fb_choose_bitmap",
			 { "fb", "bm1", "bm2" },
			 { "FB", "BM", "BM" },
			 { "", "", "" },
			 "BM", "float_bitmap_api", "choose_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_bool,
			 "from_bool",
			 { "b", "val_true", "val_false" },
			 { "BB", "float", "float" },
			 { "", "1.0", "0.0" },
			 "FB", "float_bitmap_api", "from_bool"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::distance_field,
			 "distance_field",
			 { "fb", "max_value" },
			 { "FB", "float" },
			 { "", "100.0" },
			 "FB", "float_bitmap_api", "distance_field"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::add_border,
			 "add_border",
			 { "fb" },
			 { "FB" },
			 { "" },
			 "FB", "float_bitmap_api", "add_border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_loader,
			 "load_map",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/examplemap.txt" },
			 "IBM", "bitmap_api", "intbitmap_loader"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_bm,
			 "ibm_to_bm",
			 { "ibm" },
			 { "IBM" },
			 { "" },
			 "BM", "bitmap_api", "intbitmap_bm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_fb_to_ibm_bitmap,
			 "fb_to_ibm",
			 { "fb", "start", "d" },
			 { "FB", "float", "float" },
			 { "", "0.0", "0.3" },
			 "IBM", "bitmap_api", "convert_fb_to_ibm_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_ibm_to_vx,
			 "ibm_to_vx",
			 { "bm" },
			 { "IBM" },
			 { "" },
			 "VX", "bitmap_api", "convert_ibm_to_vx"));

#endif
  return vec;
}
