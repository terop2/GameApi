#include "GameApi_h.hh"
#include "GameApi_gui.hh"

 std::vector<GameApiItem*> framebuffermoduleapi_functions()
{
  std::vector<GameApiItem*> vec;
#if 0
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_sprite_draw,
			 "fr_sprite_draw",
			 { "name", "bm", "move", "x", "y", "fmt", "start_time" },
			 { "std::string", "BM", "MN", "int", "int", "int", "float" },
			 { "s_tmp1", "", "", "0", "0", "1", "0.0" },
			 "FML", "low_frame_api", "low_sprite_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_sprite_array,
			 "fr_sprite_array",
			 { "name", "url", "bms", "mn", "x", "y", "fmt", "start_time" },
			 { "std::string", "std::string", "[BM]", "MN", "int", "int", "int", "float" },
			 { "a_tmp1", "http://tpgames.org/pos_lst.sp", "", "", "0", "0", "1", "0.0" },
			 "FML", "low_frame_api", "low_sprite_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_character_frame,
			 "fr_sprite_choose",
			 { "ev", "vec", "fetcher", "x", "y", "fmt", "mn" },
			 { "EveryApi&", "[BM]", "IF", "int", "int", "int", "MN" },
			 { "ev", "", "", "0", "0", "1", "" },
			 "FML", "font_api", "dynamic_character_frame"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_poly_draw,
			 "fr_poly_draw",
			 { "name", "p", "mn" },
			 { "std::string", "P", "MN" },
			 { "p_tmp1", "", "" },
			 "FML", "low_frame_api", "low_poly_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_render_world,
			 "fr_world_draw",
			 { "blocks", "screen_width", "screen_height", "fmt" },
			 { "[BM]", "int", "int", "int" },
			 { "", "800", "600", "1" },
			 "FML", "low_frame_api", "low_render_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_scroll_world,
			 "fr_scroll_world",
			 { "ml", "speed_x", "speed_y", "p_x", "p_y", "left_offset","right_offset", "height", "height2", "mode", "jump_frames" },
			 { "FML", "float", "float", "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "", "1.0", "1.0", "400.0", "300.0", "10.0", "10.0", "5.0", "30.0", "0", "15" },
			 "FML", "low_frame_api", "low_scroll_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_build_world,
			 "fr_build_world",
			 { "ml", "url", "chars", "x", "y" },
			 { "FML", "std::string", "std::string", "int", "int" },
			 { "", "http://tpgames.org/map.txt", ".#", "0", "0" },
			 "FML", "low_frame_api", "low_build_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_enemy_draw,
			 "fr_enemy_draw",
			 { "bm", "url", "fmt", "speed" },
			 { "BM", "std::string", "int", "float" },
			 { "", "http://tpgames.org/enemy0.txt", "1", "0.03" },
			 "FML", "low_frame_api", "low_enemy_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_enemy_draw2,
			 "fr_enemy_draw2",
			 { "bm", "url", "fmt", "speed", "time_delta", "time_duration" },
			 { "[BM]", "std::string", "int", "float", "int", "int" },
			 { "", "http://tpgames.org/enemy0.txt", "1", "0.03", "1", "8" },
			 "FML", "low_frame_api", "low_enemy_draw2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_fml,
			 "array_fml",
			 { "arr" },
			 { "[FML]" },
			 { "" },
			 "FML", "mainloop_api", "array_fml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_frame_bitmap,
			 "fr_bitmap",
			 { "ml", "sx", "sy" },
			 { "FML", "int", "int" },
			 { "", "800", "600" },
			 "BM", "low_frame_api", "low_frame_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_collision,
			 "fr_collision",
			 { "ml", "start_x", "end_x", "start_y", "end_y", "key" },
			 { "FML", "float", "float", "float", "float", "int" },
			 { "", "100.0", "120.0", "100.0", "120.0", "32" },
			 "FML", "low_frame_api", "low_collision"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_activate_snapshot,
			 "fr_snapshot",
			 { "ev", "ml", "key", "move", "duration", "next" },
			 { "EveryApi&","FML", "int", "MN", "float", "FML" },
			 { "ev", "", "32", "", "30.0", "" },
			 "FML", "low_frame_api", "low_activate_snapshot"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::qml_print,
  //			 "fr_print_qml",
  //			 { "url" },
  //			 { "std::string" },
  //			 { "http://tpgames.org/wearable.qml" },
  //			 "FML", "low_frame_api", "qml_print"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::qml_create_node,
  //			 "fr_create_qml",
  //			 { "url" },
  //			 { "std::string" },
  //			 { "http://tpgames.org/wearable.qml" },
  //			 "FML", "low_frame_api", "qml_create_node"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_framebuffer,
			 "fr_framebuffer",
			 { "mainloop", "format", "width", "height", "depth" },
			 { "FML", "int", "int", "int", "int" },
			 { "", "4", "128", "144", "0" },
			 "FBU", "low_frame_api", "low_framebuffer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_framebuffer_run,
			 "fr_run",
			 { "ev", "buffer", "mode", "scr_x", "scr_y" },
			 { "EveryApi&", "FBU", "int", "int", "int" },
			 { "ev", "", "0", "128", "144" },
			 "RUN", "low_frame_api", "low_framebuffer_run"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_root,
			 "w_root",
			 { "ev", "wd" },
			 { "EveryApi&", "W" },
			 { "ev", "" },
			 "FML", "low_frame_api", "w_root"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_layout,
			 "w_layout",
			 { "vec", "url" },
			 { "[W]", "std::string" },
			 { "", "http://tpgames.org/testlayout.lay" },
			 "W", "low_frame_api", "w_layout"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_rect,
			 "w_rect",
			 { "color" },
			 { "unsigned int" },
			 { "ffffffff" },
			 "W", "low_frame_api", "w_rect"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_bitmap,
			 "w_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "W", "low_frame_api", "w_bitmap"));
#endif
#endif
  return vec;
}
