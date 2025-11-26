#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

 std::vector<GameApiItem*> floatbitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
#if (ALL==1)||(FB_EMPTY==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::fb_empty,
			 "emptyfloat",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "FB", "float_bitmap_api", "fb_empty"));
#endif

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::grayscale,
			 "grayscale",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "grayscale"));
#endif
#if (ALL==1)||(FROM_RED==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_red,
			 "from_red",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_red"));
#endif
#if (ALL==1)||(FROM_GREEN==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_green,
			 "from_green",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_green"));
#endif
#if (ALL==1)||(FROM_BLUE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_blue,
			 "from_blue",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_blue"));
#endif
#if (ALL==1)||(FROM_ALPHA==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_alpha,
			 "from_alpha",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_alpha"));
#endif
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
#if (ALL==1)||(GRADIENT_FB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient_fb,
			 "gradient_fb",
			 { "sx", "sy", "val", "val2", "flip" },
			 { "int", "int", "float", "float", "bool" },
			 { "100", "100", "0.0", "1.0", "false" },
			 "FB", "bitmap_api", "gradient_fb"));
#endif
#if (ALL==1)||(RADIAL_FB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::radial_fb,
			 "radial_fb",
			 { "sx", "sy", "x", "y", "r", "val_at_zero", "val_at_r" },
			 { "int", "int", "float", "float", "float", "float", "float" },
			 { "100", "100", "50", "50", "50", "1.0", "0.0" },
			 "FB", "bitmap_api", "radial_fb"));
#endif
#if (ALL==1)||(SIN_FB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::sin_fb,
			 "sin_fb",
			 { "gradient" },
			 { "FB" },
			 { "" },
			 "FB", "bitmap_api", "sin_fb"));
#endif
#if (ALL==1)||(PLUS_FB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::plus_fb,
			 "plus_fb",
			 { "f1", "f2" },
			 { "FB", "FB" },
			 { "","" },
			 "FB", "bitmap_api", "plus_fb"));
#endif
#if (ALL==1)||(MUL_FB==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::mul_fb,
			 "mul_fb",
			 { "f", "mul" },
			 { "FB", "float" },
			 { "", "1.0" },
			 "FB", "bitmap_api", "mul_fb"));
#endif
#if (ALL==1)||(DIST==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::dist,
			 "bm_dist",
			 { "sx", "sy", "p_x", "p_y" },
			 { "int", "int", "float", "float" },
			 { "100", "100", "50.0", "50.0" },
			 "FB", "float_bitmap_api", "dist"));
#endif
#if (ALL==1)||(BLUR_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::blur_bitmap,
			 "bm_blur",
			 { "fb", "d" },
			 { "FB", "float" },
			 { "", "0.5" },
			 "FB", "float_bitmap_api", "blur_bitmap"));
#endif
#if (ALL==1)||(GAUSSIAN==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::gaussian,
			 "bm_gaussian",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "1.0", "100", "100" },
			 "FB", "float_bitmap_api", "gaussian"));
#endif
#if (ALL==1)||(PERLIN_NOISE2==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::perlin_noise2,
			 "bm_perlin",
		       { "sx", "sy", "key" },
		       { "int", "int", "float" },
		       { "256", "256", "0.5" },
			 "FB", "float_bitmap_api", "perlin_noise2"));
#endif
#if (ALL==1)||(TO_GRAYSCALE_COLOR==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::to_grayscale_color,
			 "to_grayscale_color",
			 { "fb", "r", "g", "b", "a", "r2", "g2", "b2", "a2" },
			 { "FB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "float_bitmap_api", "to_grayscale_color"));
#endif
#if (ALL==1)||(CHOOSE_BITMAP==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::choose_bitmap,
			 "fb_choose_bitmap",
			 { "fb", "bm1", "bm2" },
			 { "FB", "BM", "BM" },
			 { "", "", "" },
			 "BM", "float_bitmap_api", "choose_bitmap"));
#endif
#endif
#if (ALL==1)||(FROM_BOOL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_bool,
			 "from_bool",
			 { "b", "val_true", "val_false" },
			 { "BB", "float", "float" },
			 { "", "1.0", "0.0" },
			 "FB", "float_bitmap_api", "from_bool"));
#endif
#ifndef STABLE
#if (ALL==1)||(DISTANCE_FIELD==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::distance_field,
			 "distance_field",
			 { "fb", "max_value" },
			 { "FB", "float" },
			 { "", "100.0" },
			 "FB", "float_bitmap_api", "distance_field"));
#endif
#if (ALL==1)||(ADD_BORDER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::add_border,
			 "add_border",
			 { "fb" },
			 { "FB" },
			 { "" },
			 "FB", "float_bitmap_api", "add_border"));
#endif
#if (ALL==1)||(INTBITMAP_LOADER==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_loader,
			 "load_map",
			 { "url" },
			 { "std::string" },
			 { "file://$(instdir)/examplemap.txt@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "IBM", "bitmap_api", "intbitmap_loader"));
#endif
#if (ALL==1)||(INTBITMAP_BM==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_bm,
			 "ibm_to_bm",
			 { "ibm" },
			 { "IBM" },
			 { "" },
			 "BM", "bitmap_api", "intbitmap_bm"));
#endif
#if (ALL==1)||(CONVERT_FB_TO_IBM_BITMAP==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_fb_to_ibm_bitmap,
			 "fb_to_ibm",
			 { "fb", "start", "d" },
			 { "FB", "float", "float" },
			 { "", "0.0", "0.3" },
			 "IBM", "bitmap_api", "convert_fb_to_ibm_bitmap"));
#endif
#if (ALL==1)||(CONVERT_IBM_TO_VX==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_ibm_to_vx,
			 "ibm_to_vx",
			 { "bm" },
			 { "IBM" },
			 { "" },
			 "VX", "bitmap_api", "convert_ibm_to_vx"));

#endif
#endif
  return vec;
}
