#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
 
std::vector<GameApiItem*> bitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::newbitmap,
			 "new", 
			 { "sx", "sy", "color" },
			 { "int", "int", "unsigned int" },
			 { "100", "100", "00000000" },
			 "BM", "bitmap_api", "newbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::newbitmap_bm,
			 "new_bm",
			 { "bm", "color" },
			 { "BM", "unsigned int" },
			 { "", "00000000" },
			 "BM", "bitmap_api", "newbitmap_bm"));
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::newbitmap_fb,
			 "new_fb",
			 { "fb", "color" },
			 { "FB", "unsigned int" },
			 { "", "00000000" },
			 "BM", "bitmap_api", "newbitmap_fb"));
      vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::newbitmap_bb,
			 "new_bb",
			 { "bb", "color" },
			 { "BB", "unsigned int" },
			 { "", "00000000" },
			 "BM", "bitmap_api", "newbitmap_bb"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::script_bitmap,
			 "bm_expr",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "https://tpgames.org/example_expr_e.mp", "100", "100" },
			 "BM", "bitmap_api", "script_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::loadbitmap,
			 "load",
			 { "filename" },
			 { "std::string" },
			 { "test.png" },
			 "BM", "bitmap_api", "loadbitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::save_png_ml,
			 "save_png",
			 { "ev", "bm", "filename" },
			 { "EveryApi&", "BM", "std::string" },
			 { "ev", "", "test.png" },
			 "ML", "bitmap_api", "save_png_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::savepng_array_ml,
			 "save_png_arr",
			 { "ev", "bms", "filename_start", "filename_end", "alpha", "time" },
			 { "EveryApi&", "[BM]", "std::string", "std::string", "bool", "float" },
			 { "ev", "", "test", ".png", "false", "1.0" },
			 "ML", "bitmap_api", "savepng_array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::save_raw,
			 "save_raw",
			 { "bm", "filename" },
			 { "BM", "std::string" },
			 { "", "test.raw" },
			 "ML", "bitmap_api", "save_raw"));
#if 1
  // doesnt work in emscripten, all solutions seem to fail miserably,
  // with emscripten_async_wget didn't work fine.
  // and emscripten_wget had simlar problems
  // also it would make emscripten build alot larger for no good reason
  // when you had to use -s ASYNCIFY and other flags
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::loadbitmapfromurl,
			 "bm_url",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/gameapi_logo.png" },
			 "BM", "bitmap_api", "loadbitmapfromurl"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::dyn_fetch_bitmap,
			 "bm_fetch",
			 { "ev", "url", "reload_frame", "texture_unit" },
			 { "EveryApi&", "std::string", "int", "int" },
			 { "ev", "http://tpgames.org/gameapi_logo.png", "300000", "0" },
			 "TXID", "bitmap_api", "dyn_fetch_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::dyn_fetch_bitmap2,
			 "bm_fetch2",
			 { "ev", "url", "url2", "texture_unit", "sx", "sy", "sx2", "sy2" },
			 { "EveryApi&", "std::string", "std::string", "int", "int", "int", "int", "int" },
			 { "ev", "http://tpgames.org/gameapi_logo.png", "http://tpgames.org/gameapi_logo.png", "0", "256", "256", "1024", "1024" },
			 "TXID", "bitmap_api", "dyn_fetch_bitmap2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gltf_load_bitmap,
			 "bm_gltf",
			 { "ev", "tf", "image_index" },
			 { "EveryApi&", "TF", "int" },
			 { "ev", "", "0" },
			 "BM", "polygon_api", "gltf_load_bitmap"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_x,
			 "bm_atlas_x",
			 { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_x", "count" },
			 { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int" },
			 { "ev", "", "0", "10", "0", "10", "10", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_y,
			 "bm_atlas_y",
			 { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_y", "count" },
			 { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int" },
			 { "ev", "", "0", "10", "0", "10", "10", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_y"));
    vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_xy,
			 "bm_atlas_xy",
			   { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_x", "delta_y", "count_x", "count_y" },
			   { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int", "int", "int" },
			   { "ev", "", "0", "10", "0", "10", "10", "10", "2", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_xy"));
#endif
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::cubemap,
			   "bm_cubemap",
			   { "bm" },
			   { "BM" },
			   { "" },
			   "BM,BM,BM,BM,BM,BM", "bitmap_api", "cubemap")); // this was array before
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::bm_cubemap,
			   "bm_invcubemap",
			   { "ev", "left", "top", "middle", "right", "back", "down", "sx", "sy" },
			   { "EveryApi&", "BM", "BM", "BM", "BM", "BM", "BM", "int", "int" },
			   { "ev", "", "", "", "", "", "", "256", "256" },
			   "BM", "bitmap_api", "bm_cubemap"));

#ifndef STABLE
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::fix_edges,
			   "edge_fix",
			   { "bm" },
			   { "BM" },
			   { "" },
			   "BM", "bitmap_api", "fix_edges"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::border,
			 "border",
			 { "bm", "left", "right", "top", "bottom" },
			 { "BM", "int", "int", "int", "int" },
			 { "", "5", "5", "5", "5" },
			 "BM", "bitmap_api", "border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::add_shape_border,
			 "add_shape_border",
			 { "ev", "bm", "dist_field_size", "start_range", "end_range", "r", "g", "b", "a", "border_size" },
			 { "EveryApi&", "BM", "float", "float", "float", "int", "int", "int", "int", "int" },
			 { "ev", "", "7.0", "0.5", "0.9", "0", "0", "0", "255", "5" },
			 "BM", "bitmap_api", "add_shape_border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_color,
			 "bm_choose_color",
			 { "bm", "c1", "c2" },
			 { "BM", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "00888888" },
			 "BB", "bitmap_api", "choose_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::subbitmap,
			 "sub",
			 { "orig", "x", "y", "width", "height" },
			 { "BM", "int", "int", "int", "int" },
			 { "",   "0", "0", "100", "100" },
			 "BM", "bitmap_api", "subbitmap"));

#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::alt,
			 "alt",
			 { "vec", "index" },
			 { "[BM]", "int" },
			 { "",   "0" },
			 "BM", "bitmap_api", "alt"));

  //vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::growbitmap,
  //			 "grow",
  //			 { "orig", "l", "t", "r", "b" },
  //			 { "BM", "int", "int", "int", "int" },
  //			 { "", "2", "2", "2", "2" },
  //			 "BM", "bitmap_api", "growbitmap"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::compose_x,
			 "compose_x",
			 { "bm1", "bm2" },
			 { "BM", "BM" },
			 { "", "" },
			 "BM", "bitmap_api", "compose_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::compose_y,
			 "compose_y",
			 { "bm1", "bm2" },
			 { "BM", "BM" },
			 { "", "" },
			 "BM", "bitmap_api", "compose_y"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int))&GameApi::BitmapApi::blitbitmap,
			 "blit",
			 { "bg", "orig", "x", "y" },
			 { "BM", "BM", "int", "int" },
			 { "", "", "0", "0" },
			 "BM", "bitmap_api", "blitbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::FB))&GameApi::BitmapApi::blitbitmap_fb,
			 "blitFB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "FB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap_fb"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::BB))&GameApi::BitmapApi::blitbitmap_bb,
			 "blitBB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "BB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap_bb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_bitmap,
			 "bm_scale",
			 { "ev", "orig", "sx", "sy" },
			 { "EveryApi&", "BM", "int", "int" },
			 { "ev", "", "800", "600" },
			 "BM", "bitmap_api", "scale_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_to_size,
			 "bm_scale2",
			 { "bm", "sz" },
			 { "BM", "int" },
			 { "", "400" },
			 "BM", "bitmap_api", "scale_to_size"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_bitmap_fullscreen,
			 "bm_fullscreen",
			 { "ev", "orig" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "BM", "bitmap_api", "scale_bitmap_fullscreen"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::interpolate_bitmap,
			 "interpolate",
			 { "orig1", "orig2", "x" },
			 { "BM", "BM", "float" },
			 { "", "", "0.0" },
			 "BM", "bitmap_api", "interpolate_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::color_bm,
			 "bm_color",
			 { "bm", "color" },
			 { "BM", "unsigned int" },
			 { "", "fff8844" },
			 "BM", "bitmap_api", "color_bm"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::plus_bitmap,
			 "bm_plus",
			 { "bm1", "bm2" },
			 { "BM", "BM" },
			 { "", "" },
			 "BM", "bitmap_api", "plus_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::median_filter,
			 "bm_medianfilter",
			 { "bm", "sx", "sy" },
			 { "BM", "int", "int" },
			 { "", "1", "1" },
			 "BM", "bitmap_api", "median_filter"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::repeat_bitmap,
			 "repeat",
			 { "orig1", "x_count", "y_xount" },
			 { "BM", "int", "int" },
			 { "", "1", "1" },
			 "BM", "bitmap_api", "repeat_bitmap"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap,
			 "world",
			 { "f", "int_bm", "dx", "dy" },
			 { "std::function<BM(int>", "BM", "int", "int" },
			 { "", "", "100", "100" },
			 "BM", "bitmap_api", "world_from_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_x,
			 "flip_x",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "flip_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_y,
			 "flip_y",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "flip_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::rot90,
			 "rot90",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "rot90"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::Indicator,
			 "indicator",
			 { "sx", "sy", "g_ind" },
			 { "int", "int", "int" },
			 { "256", "256", "0" },
			 "BM", "bitmap_api", "Indicator"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fractal_mountain,
			 "fractal_mountain",
			 { "level", "sx", "sy" },
			 { "int", "int", "int" },
			 { "3", "100", "100" },
			 "FB", "polygon_api", "fractal_mountain"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::lightmap_bitmap,
			 "lightmap",
			 { "sx", "sy", "faces", "faces2", "face", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "int", "int", "P", "P", "int", "float", "float", "float" },
			 { "256", "256", "", "", "0", "1.0", "2.0", "1.0" },
			 "BM", "bitmap_api", "lightmap_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::noise_vectors,
			 "noise_vec",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "256", "256" },
			 "BM", "bitmap_api", "noise_vectors"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient,
			 "gradient",
			 { "pos_1", "pos_2", "color_1", "color_2", "sx", "sy" },
			 { "PT",    "PT",    "unsigned int", "unsigned int", "int", "int" },
			 { "ev.point_api.point(0.0,0.0,0.0)", "ev.point_api.point(0.0, 100.0, 0.0)", "ffffffff", "ff888888", "100", "100" },
			 "BM", "bitmap_api", "gradient"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::circular_gradient,
			 "circ_gradient",
			 { "sx", "sy", "center_color", "edge_color" },
			 { "int", "int", "unsigned int", "unsigned int" },
			 { "256", "256", "88888888", "00000000" },
			 "BM", "bitmap_api", "circular_gradient"));
  
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chessboard,
			 "chessboard",
			 { "tile_sx", "tile_sy", "count_x", "count_y", "color_1", "color_2" },
			 { "int", "int", "int", "int", "unsigned int", "unsigned int" },
			 { "10", "10", "8", "8", "ffffffff", "ff888888" },
			 "BM", "bitmap_api", "chessboard"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::mandelbrot,
			 "mandelbrot",
			 { "is_julia", "start_x", "end_x", "start_y", "end_y", "xx", "yy", "sx", "sy", "count" },
			 { "bool", "float", "float", "float", "float", "float", "float", "int", "int", "int" },
			 { "false", "-2.0", "1.0", "-1.0", "1.0", "0.0", "0.0", "256", "256", "64" },
			 "BM", "bitmap_api", "mandelbrot"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::simple_blur,
			 "blur",
			 { "bm", "center", "left", "right", "top", "bottom" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "0.5", "0.125", "0.125", "0.125", "0.125" },
			 "BM", "bitmap_api", "simple_blur"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flood_fill,
			 "floodfill",
			 { "fb", "percentage", "x", "y", "inv" },
			 { "FB", "float", "int", "int", "bool" },
			 { "", "0.97", "0", "0", "false" },
			 "FB", "bitmap_api", "flood_fill"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flood_fill_color,
			 "floodfill_color",
			 { "ev", "bm", "percentage", "x", "y", "color" },
			 { "EveryApi&", "BM", "float", "int", "int", "unsigned int" },
			 { "ev", "", "0.97", "0", "0", "ffff0000" },
			 "BM", "bitmap_api", "flood_fill_color"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flood_fill_color_inv,
			 "floodfill_color_inv",
			 { "ev", "bm", "percentage", "x", "y", "color" },
			 { "EveryApi&", "BM", "float", "int", "int", "unsigned int" },
			 { "ev", "", "0.97", "0", "0", "ffff0000" },
			 "BM", "bitmap_api", "flood_fill_color_inv"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gray_to_black,
			 "gray_to_black",
			 { "bm", "val" },
			 { "BM", "float" },
			 { "", "0.7" },
			 "BM", "bitmap_api", "gray_to_black"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::calculate_baked_light,
			 "baked_shadow",
			 { "texture_p", "scene_p", "texture", "count", "light_pos_x", "light_pos_y", "light_pos_z", "shadow_darkness", "softness" },
			 { "P", "P", "BM", "int", "float", "float", "float", "float", "float" },
			 { "", "", "", "10000", "-300.0", "300.0", "-300.0", "0.2", "50.0" },
			 "BM", "bitmap_api", "calculate_baked_light"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::bump_map,
			 "bm_bumpmap",
			 { "fb", "h" },
			 { "FB", "float" },
			 { "", "10.0" },
			 "BM", "bitmap_api", "bump_map"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::memoize,
			 "bm_memoize",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "memoize"));
#ifndef STABLE
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap3,
			 "bm_world_from_bitmap",
			 { "ev", "v", "url", "chars", "dx", "dy" },
			 { "EveryApi&", "[BM]", "std::string", "std::string", "int", "int" },
			 { "ev", "", "http://tpgames.org/map.txt", ".#", "100", "100" },
			 "BM", "bitmap_api", "world_from_bitmap3"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::persistent_cache,
			 "bm_persistent_cache",
			 { "bm", "filename" },
			 { "BM", "std::string" },
			 { "", "cache.ppm" },
			 "BM", "bitmap_api", "persistent_cache"));
#endif
#if 0 
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::create_vertex_array,
			 "bm_prepare",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "VA", "sprite_api", "create_vertex_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::update_vertex_array_ml,
			 "bm_update",
			 { "va", "bm" },
			 { "VA", "BM" },
			 { "", "" },
			 "ML", "sprite_api", "update_vertex_array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::render_sprite_vertex_array_ml,
			 "bm_render",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "sprite_api", "render_sprite_vertex_array_ml"));
#endif


#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::cbm_empty,
			 "cbm_empty",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "1.0", "1.0" },
			 "CBM", "cont_bitmap_api", "cbm_empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::from_bitmap,
			 "cbm_from_bitmap",
			 { "bm", "xsize", "ysize" },
			 { "BM", "float", "float" },
			 { "", "1.0", "1.0" },
			 "CBM", "cont_bitmap_api", "from_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::to_bitmap,
			 "cbm_to_bitmap",
			 { "cbm", "sx", "sy" },
			 { "CBM", "int", "int" },
			 { "", "200", "200" },
			 "BM", "cont_bitmap_api", "to_bitmap"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_bitmap2,
			 "cbm_scale_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "CBM", "bitmap_api", "scale_bitmap2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::bicubic,
			 "cbm_bicubic",
			 { "f_0", "f_1", "df_0", "df_1", "ff_0", "ff_1", "dff_0", "dff_1" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.5", "-0.5", "1.0", "1.0", "0.5", "-0.5" },
			 "CBM", "cont_bitmap_api", "bicubic"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::sample,
			 "cbm_sample",
			 { "cbm", "sx", "sy" },
			 { "CBM", "int", "int" },
			 { "", "200", "200" },
			 "BM", "cont_bitmap_api", "sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::rotate,
			 "cbm_rotate",
			 { "cbm", "center_x", "center_y", "angle" },
			 { "CBM", "float", "float", "float" },
			 { "", "0.5", "0.5", "0.0" },
			 "CBM", "cont_bitmap_api", "rotate"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::s_spherical,
			 "s_spherical",
			 { "c_x", "c_y", "c_z", "fb", "start_radius", "end_radius", "start_values", "end_values" },
			 { "float", "float", "float", "FB", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "", "300", "350", "0.0", "1.0" },
			 "S", "polygon_api", "s_spherical"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::surface_api, &GameApi::SurfaceApi::texture,
			 "s_bitmap",
			 { "orig", "texture" },
			 { "S", "BM" },
			 { "", "" },
			 "S", "surface_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::s_sample,
			 "s_sample",
			 { "surf", "sx", "sy" },
			 { "S", "int", "int" },
			 { "", "300", "300" },
			 "P", "polygon_api", "s_sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::C_bitmap,
			 "cfb_c",
			 { "sx", "sy", "C" },
			 { "float","float", "float" },
			 { "100.0", "100.0", "1.0" },
			 "CFB", "float_bitmap_api", "C_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::X_bitmap,
			 "cfb_x",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "300.0", "300.0" },
			 "CFB", "float_bitmap_api", "X_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Y_bitmap,
			 "cfb_y",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "300.0", "300.0" },
			 "CFB", "float_bitmap_api", "Y_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::AddBitmap,
			 "cfb_add",
			 { "vec" },
			 { "[CFB]" },
			 { "" },
			 "CFB", "float_bitmap_api", "AddBitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::MulBitmap,
			 "cfb_mul",
			 { "vec" },
			 { "[CFB]" },
			 { "" },
			 "CFB", "float_bitmap_api", "MulBitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SqrtContFloat,
			 "cfb_sqrt",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "SqrtContFloat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Sin,
			 "cfb_sin",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Sin"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Cos,
			 "cfb_cos",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Cos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Tan,
			 "cfb_tan",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Tan"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SampleContFloat,
			 "cfb_sample",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "sx", "sy", "mult" },
			 { "CFB", "float", "float", "float", "float", "int", "int", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "100", "100", "1.0" },
			 "FB", "float_bitmap_api", "SampleContFloat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Equalizer,
			 "cfb_equalizer",
			 { "a1", "a2" },
			 { "CFB", "CFB" },
			 { "", "" },
			 "CBB", "float_bitmap_api", "Equalizer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SampleContBool,
			 "cbb_sample",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "sx", "sy" },
			 { "CBB", "float", "float", "float", "float", "int", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "100", "100" },
			 "BB", "float_bitmap_api", "SampleContBool"));
  
#endif
  return vec;
}
