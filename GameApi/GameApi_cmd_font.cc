#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> fontapi_functions()
{
  std::vector<GameApiItem*> vec;
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::newfont,
			 "newfont",
			 { "file", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/FreeSans.ttf", "20", "20" },
			 "Ft", "font_api", "newfont"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph,
			 "glyph",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BM", "font_api", "glyph"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_outline,
			 "glyph_outline",
			 { "font", "idx", "sx", "sy" },
			 { "Ft", "long", "float", "float" },
			 { "", "64", "100.0", "100.0" },
			 "LI", "font_api", "glyph_outline"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_plane,
			 "glyph_plane",
			 { "font", "idx", "sx", "sy", "dx", "dy" },
			 { "Ft", "long", "float", "float", "float", "float" },
			 { "", "64", "100.0", "100.0", "30.0", "30.0" },
			 "PL", "font_api", "glyph_plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string,
			 "font_string",
			 { "font", "str", "x_gap" },
			 { "Ft", "std::string", "int" },
			 { "", "Hello", "5" },
			 "BM", "font_api", "font_string"));
#endif

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_string,
			 "dyn_string",
			 { "ev", "font", "alternative_chars", "fetcher", "x", "y", "numchars" },
			 { "EveryApi&", "Ft", "std::string", "SF", "int", "int", "int" },
			 { "ev", "", "0123456789:", "", "0", "0", "5" },
			 "ML", "font_api", "dynamic_string"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_fb,
			 "glyph_fb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "FB", "font_api", "glyph_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_bb,
			 "glyph_bb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BB", "font_api", "glyph_bb"));
#endif
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas_info,
			 "atlas_info",
			 { "ev", "font", "chars", "sx", "sy", "y_delta" },
			 { "EveryApi&", "Ft", "std::string", "float", "float", "int" },
			 { "ev", "", "0123456789", "30", "30", "40" },
			 "FtA", "font_api", "font_atlas_info"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas,
			 "font_atlas",
			 { "ev", "font", "atlas", "sx", "sy" },
			 { "EveryApi&", "Ft", "FtA", "float", "float" },
			 { "ev", "", "", "30", "30" },
			 "BM", "font_api", "font_atlas"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_from_atlas,
			 "font_string2",
			 { "ev", "atlas", "atlas_bm", "str", "x_gap" },
			 { "EveryApi&", "FtA", "BM", "std::string", "int" },
			 { "ev", "", "", "Hello", "5" },
			 "BM", "font_api", "font_string_from_atlas"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::save_atlas,
			 "save_atlas",
			 { "atlas", "filename" },
			 { "FtA", "std::string" },
			 { "", "atlas1.txt" },
			 "()", "font_api", "save_atlas"));
#endif
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_atlas,
			 "load_atlas",
			 { "filename" },
			 { "std::string" },
			 { "atlas1.txt" },
			 "FtA", "font_api", "load_atlas"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_array,
			 "fnt_chars",
			 { "font", "string", "x_gap" },
			 { "Ft", "std::string", "int" },
			 { "", "0123456789", "2" },
			 "ARR", "font_api", "font_string_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_array2,
			 "fnt_chars2",
			 { "font", "string" },
			 { "FI", "std::string" },
			 { "", "0123456789" },
			 "[BM]", "font_api", "font_string_array2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_string_fetcher,
			 "fnt_time",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "SF", "font_api", "time_string_fetcher"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::score_string_fetcher,
			 "fnt_score",
			 { "id", "label", "numdigits" },
			 { "std::string", "std::string", "int" },
			 { "score", "Score: ", "5" },
			 "SF", "font_api", "score_string_fetcher"));
#endif
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_adder,
			 "score_adder",
			 { "ev", "ml", "o", "mn", "enter_score", "leave_score", "dyn_point", "timeout" },
			 { "EveryApi&", "ML", "O", "MN", "int", "int", "int", "float" },
			 { "ev", "", "", "", "1", "0", "-1", "5.0" },
			 "ML", "mainloop_api", "score_adder"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_adder2,
			 "score_adder2",
			 { "ev", "ml", "o", "points", "mn", "enter_score", "leave_score", "dyn_point", "timeout", "fetcher" },
			 { "EveryApi&", "ML", "O", "PTS", "MN", "int", "int", "int", "float", "IF" },
			 { "ev", "", "", "", "", "1", "0", "-1", "5.0", "" },
			 "ML", "mainloop_api", "score_adder2"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_hidder,
			 "score_hidder",
			 { "ev", "ml", "o","max_count" },
			 { "EveryApi&", "ML", "O", "int" },
			 { "ev", "", "", "50" },
			 "ML", "mainloop_api", "score_hidder"));

  
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_font,
			 "FI_load",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/Chunkfive.otf", "200", "200" },
			 "FI", "font_api", "load_font"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::draw_text_string,
			 "FI_drawtext",
			 { "font", "str", "x_gap", "line_height" },
			 { "FI", "std::string", "int", "int" },
			 { "", "Hello", "5", "30" },
			 "BM", "font_api", "draw_text_string"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::draw_text_large,
			 "FI_largetext",
			 { "ev", "font", "texturl", "x_gap", "line_height", "baseline_separation" },
			 { "EveryApi&", "FI", "std::string", "int", "int", "int" },
			 { "ev", "", "http://tpgames.org/text_test.txt", "5", "30", "-1" },
			 "BM", "font_api", "draw_text_large"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::save_font_dump,
			 "FI_save_dump",
			 { "font", "chars", "filename" },
			 { "FI", "std::string", "std::string" },
			 { "", "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!\"#€%&/()=?+\\*^.,-<>|§œ;:[]_ ", "font.txt" },
			 "ML", "font_api", "save_font_dump"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_font_dump,
			 "FI_load_dump",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/font.txt" },
			 "FI", "font_api", "load_font_dump"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::bm_array_id,
			 "bm_array_id",
			 { "vec" },
			 { "[BM]" },
			 { "" },
			 "[BM]", "font_api", "bm_array_id"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_character,
			 "bm_chooser",
			 { "ev", "vec", "fetcher", "x", "y" },
			 { "EveryApi&", "[BM]", "IF", "int", "int" },
			 { "ev", "", "", "0","0" },
			 "ML", "font_api", "dynamic_character"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_polygon,
			 "p_chooser",
			 { "ev", "vec", "mat", "fetcher" },
			 { "EveryApi&", "[P]", "MT", "IF" },
			 { "ev", "", "", "" },
			 "ML", "font_api", "dynamic_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::ml_chooser,
			 "ml_chooser",
			 { "vec", "fetcher" },
			 { "[ML]", "IF" },
			 { "", "" },
			 "ML", "font_api", "ml_chooser"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::anim_ML,
			 "ml_anim",
			 { "ev", "url", "p1", "p2", "p3", "p4", "p5", "dyn" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string", "IF" },
			 { "ev", "http://tpgames.org/marble_cube_ml.mp", "a/a", "b/b", "c/c", "d/d", "e/e", "" },
			 "ML", "mainloop_api", "anim_ML"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::key_ml,
			 "key_ml",
			 { "vec", "keys" },
			 { "[ML]", "std::string" },
			 { "", "zxcvbnmfghjklertyuiop" },
			 "ML", "mainloop_api", "key_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::quake_area_fetcher,
			 "if_qarea",
			 { "start_x", "end_x", "start_z", "end_z" },
			 { "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0" },
			 "IF", "font_api", "quake_area_fetcher"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::toggle_button_fetcher,
			 "if_toggle",
			 { "start_x", "end_x", "start_y", "end_y" },
			 { "float", "float", "float", "float" },
			 { "0.0", "800.0", "0.0", "600.0" },
			 "IF", "font_api", "toggle_button_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::timed_int_fetcher,
			 "if_timed",
			 { "ev", "start", "end", "start_time", "end_time" },
			 { "EveryApi&", "int", "int", "float", "float" },
			 { "ev", "0", "10", "0.0", "30.0" },
			 "IF", "font_api", "timed_int_fetcher"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::movement_int_fetcher,
			 "if_move",
			 { "count", "x_mult", "y_mult", "z_mult" },
			 { "int", "float", "float", "float" },
			 { "10", "0.1", "0.1", "0.1" },
			 "IF", "font_api", "movement_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::repeat_int_fetcher,
			 "if_repeat",
			 { "fetcher", "duration" },
			 { "IF", "float" },
			 { "", "30.0" },
			 "IF", "font_api", "repeat_int_fetcher"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::keypress_int_fetcher,
			 "if_keypress",
			 { "key", "key_down_value", "key_up_value" },
			 { "int", "int", "int" },
			 { "32", "1", "0" },
			 "IF", "font_api", "keypress_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::state_int_fetcher,
			 "if_statemachine",
			 { "url", "states" },
			 { "std::string", "std::string" },
			 { "http://tpgames.org/move.sm", "s0&s1&s2&s3&s4" },
			 "IF", "mainloop_api", "state_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::hmd_state_fetcher,
			 "if_hmd",
			 { },
			 { },
			 { },
			 "IF", "font_api", "hmd_state_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::choose_screen,
			 "choose_screen",
			 { "left_x", "right_x", "min_screen", "max_screen" },
			 { "float", "float", "int", "int" },
			 { "-600.0", "600.0", "0", "4" },
			 "IF,MN", "font_api", "choose_screen"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::face_fetcher,
			 "uv_face",
			 { "p", "facenum" },
			 { "P", "int" },
			 { "", "0" },
			 "UV", "mainloop_api", "face_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::choose_float_fetcher,
			 "ff_choose",
			 { "int_fetcher", "a_0", "a_1", "a_2", "a_3", "a_4", "a_5", "a_6" },
			 { "IF", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0" },
			 "FF", "font_api", "choose_float_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_fetcher2,
			 "ff_time",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "FF", "font_api", "time_fetcher2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_range_fetcher,
			 "ff_range",
			 { "start_time", "end_time", "before_start", "start_value", "end_value", "after_end", "repeat" },
			 { "float", "float", "float" ,"float", "float", "float","float" },
			 { "0.0", "30.0", "-1000.0", "0.0", "1.0", "-1000.0", "10000.0" },
			 "FF", "font_api", "time_range_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_range_fetcher_key,
			 "ff_key_range",
			 { "key", "start_time", "end_time", "before_start", "start_value", "end_value", "after_end", "repeat" },
			 { "int", "float", "float", "float" ,"float", "float", "float", "float" },
			 { "32", "0.0", "30.0", "-1000.0", "0.0", "1.0", "-1000.0", "10000.0" },
			 "FF", "font_api", "time_range_fetcher_key"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::point_fetcher_constant,
			 "pf_constant",
			 { "x", "y", "z" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "PF", "font_api", "point_fetcher_constant"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::mouse_fetcher,
			 "pf_mouse",
			 { },
			 { },
			 { },
			 "PF", "font_api", "mouse_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::point_fetcher_part,
			 "pf_component",
			 { "point_fetcher", "component", "float_fetcher" },
			 { "PF", "int", "FF" },
			 { "", "0", "" },
			 "PF", "font_api", "point_fetcher_part"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::fps_fetcher,
			 "fnt_fps",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "FF", "font_api", "fps_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::float_to_string_fetcher,
			 "fnt_float_to_string",
			 { "fetcher" },
			 { "FF" },
			 { "" },
			 "SF", "font_api", "float_to_string_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::char_fetcher_from_string,
			 "fnt_char_idx",
			 { "string_fetcher", "alternatives", "idx" },
			 { "SF", "std::string", "int" },
			 { "", "0123456789", "0" },
			 "IF", "font_api", "char_fetcher_from_string"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::x_comp,
			 "pf_x",
			 { "point_fetcher", "start_x", "end_x", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "x_comp"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::y_comp,
			 "pf_y",
			 { "point_fetcher", "start_y", "end_y", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "y_comp"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::z_comp,
			 "pf_z",
			 { "point_fetcher", "start_z", "end_z", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "z_comp"));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::level,
			 "tree_level",
			 { "vec" },
			 { "[MN]" },
			 { "" },
			 "TL", "tree_api", "level"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree,
			 "tree",
			 { "vec" },
			 { "[TL]" },
			 { "" },
			 "T", "tree_api", "tree"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_p,
			 "tree_p",
			 { "ev", "tree", "vec", "time", "percentage" },
			 { "EveryApi&", "T", "[P]", "float", "float" },
			 { "ev", "", "", "0.0", "0.5" },
			 "P", "tree_api", "tree_p"));
  
  return vec;
}
