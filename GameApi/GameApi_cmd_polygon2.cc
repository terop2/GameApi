#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> polygonapi_functions2()
{
  std::vector<GameApiItem*> vec;

#ifndef STABLE
#if (ALL==1)||(COLOR_MAP==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_map,
			 "color_map",
			 { "bm", "statt_x", "end_x", "start_y", "end_y", "z" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "-200", "200", "-200", "200", "0" },
			 "P", "polygon_api", "color_map"));
#endif
#if (ALL==1)||(COLOR_MAP3==1)
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3,
			 "color_map3",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3", "[B]", "Broken"));
#endif
#if (ALL==1)||(COLOR_MAP3_CYL==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3_cyl,
			 "color_map3_cyl",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3_cyl", "[B]", "Broken"));
#endif
#if (ALL==1)||(COLOR_MAP3_SPH==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3_sph,
			 "color_map3_sph",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3_sph", "[B]", "Slightly Broken"));
#endif
  
#endif
#if (ALL==1)||(COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color,
			 "color",
			 { "orig", "color" },
			 { "P", "unsigned int" },
			 { "", "ffffffff" },
			 "P", "polygon_api", "color"));
#endif
#if (ALL==1)||(MIX_COLOR==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mix_color,
			 "p_mix_color",
			 { "orig", "orig2", "val" },
			 { "P", "P", "float" },
			 { "", "", "0.5" },
			 "P", "polygon_api", "mix_color"));
#endif
#ifndef STABLE
#if (ALL==1)||(MIN_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::min_color,
			 "p_min_color",
			 { "orig", "orig2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "min_color"));
#endif
#if (ALL==1)||(MAX_COLOR==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::max_color,
			 "p_max_color",
			 { "orig", "orig2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "max_color"));
#endif
#endif
  
#if (ALL==1)||(COLOR_FACES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_faces,
			 "color_faces",
			 { "orig", "color_1", "color_2", "color_3", "color_4" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ffffffff", "ff888888", "ff888888" },
			 "P", "polygon_api", "color_faces"));
#endif
#ifndef STABLE
#if (ALL==1)||(COLOR_ALPHA==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_alpha,
			 "color_alpha",
			 { "orig", "alpha" },
			 { "P", "unsigned int" },
			 { "", "ff" },
			 "P", "polygon_api", "color_alpha"));
#endif
#endif
#if (ALL==1)||(COLOR_FROM_NORMALS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_from_normals,
			 "color_from_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "color_from_normals"));
#endif
#ifndef STABLE
#if (ALL==1)||(COLOR_FROM_TEXCOORD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_from_texcoord,
			 "color_from_texcoord",
			 { "orig", "color_tl", "color_tr", "color_bl", "color_br" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffff8844", "ff884422", "ffaa8844", "ffffffff" },
			 "P", "polygon_api", "color_from_texcoord"));
#endif
#if (ALL==1)||(AMBIENT_OCCULSION_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::ambient_occulsion_color,
			 "ao_color",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "ambient_occulsion_color"));
#endif
#if (ALL==1)||(NORMAL_DARKNESS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::normal_darkness,
			 "color_darkness",
			 { "p", "dark" },
			 { "P", "float" },
			 { "", "1.0" },
			 "P", "polygon_api", "normal_darkness"));
#endif
#if (ALL==1)||(GRADIENT_COLOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::gradient_color,
			 "color_gradient",
			 { "p", "p_x", "p_y", "p_z", "v_x", "v_y", "v_z", "start_color", "end_color" },
			 { "P", "float", "float", "float", "float", "float", "float", "unsigned int", "unsigned int" },
			 { "", "0.0", "-300.0", "0.0", "0.0", "600.0", "0.0", "ff000000", "ffffffff" },
			 "P", "polygon_api", "gradient_color"));
#endif
#if (ALL==1)||(COLOR_DISTANCE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_distance,
			 "color_distance",
			 { "model", "center_x", "center_y", "center_z", "color_center", "color_dist", "dist_center", "dist_dist" },
			 { "P", "float", "float", "float", "unsigned int", "unsigned int", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "ffffffff", "ff000000", "10.0", "300.0" },
			 "P", "polygon_api", "color_distance"));
#endif
#endif
#if (ALL==1)||(COLOR_RANGE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_range,
			 "color_range",
			 { "orig", "source_upper", "source_lower", "upper_range", "lower_range" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "00000000","ffffffff", "88888888" },
			 "P", "polygon_api", "color_range"));
#endif
#ifndef STABLE
#if (ALL==1)||(COLOR_LAMBERT==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_lambert,
			 "color_lambert",
			 { "orig", "color", "light_dir_x", "light_dir_y", "light_dir_z", "pow", "intensity" },
			 { "P", "unsigned int", "float", "float", "float", "float","float" },
			 { "", "ff884422", "-2.0", "1.0", "2.0","1.0","0.3" },
			 "P", "polygon_api", "color_lambert"));
#endif
#endif
#if (ALL==1)||(RECALCULATE_NORMALS==1) 
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::recalculate_normals,
			 "recalc_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "recalculate_normals"));
#endif
#if (ALL==1)||(SMOOTH_NORMALS2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::smooth_normals2,
			 "smooth_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "smooth_normals2"));
#endif
#ifndef STABLE
#if (ALL==1)||(SPHERICAL_NORMALS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spherical_normals,
			 "spherical_normals",
			 { "p", "p_x", "p_y", "p_z" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "spherical_normals"));
#endif
#if 0
  // doesnt work too well
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::average_normals,
			 "average_normals",
			 { "orig", "sx", "sy" },
			 { "P", "int", "int" },
			 { "", "30", "30" },
			 "P", "polygon_api", "average_normals"));
#endif
#endif
#ifndef STABLE
#if (ALL==1)||(FLIP_NORMALS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::flip_normals,
			 "flip_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "flip_normals"));
#endif
#if (ALL==1)||(BUILD_OFFSETS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::build_offsets,
			 "p_offsets",
			 { "orig", "vec" },
			 { "P", "[PT]" },
			 { "", "" },
			 "P", "polygon_api", "build_offsets"));
#endif
#endif
#if (ALL==1)||(COLOR_GRAYSCALE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_grayscale,
			 "color_grayscale",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "color_grayscale"));
#endif
#if (ALL==1)||(CHOOSE_TEXTURE==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::choose_texture,
			 "choose_texture",
			 { "orig", "tex" },
			 { "P", "int" },
			 { "", "0" },
			 "P", "polygon_api", "choose_texture"));
#endif
#if (ALL==1)||(TEXCOORD_MANUAL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_manual,
			 "texcoord",
			 { "orig", "p1_x", "p1_y", "p2_x", "p2_y", "p3_x", "p3_y", "p4_x", "p4_y" },
			 { "P", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "1.0", "0.0", "1.0", "1.0", "0.0", "1.0" },
			 "P", "polygon_api", "texcoord_manual"));
#endif
#ifndef STABLE
#if (ALL==1)||(TEXCOORD_PLANE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_plane,
			 "texcoord_plane",
			 { "orig", "start_x", "end_x", "start_y", "end_y" },
			 { "P", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "polygon_api", "texcoord_plane"));
#endif
#if (ALL==1)||(FROM_NORMAL_TO_TEXCOORD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_normal_to_texcoord,
			 "texcoord_from_normal",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "from_normal_to_texcoord"));
#endif
#if (ALL==1)||(TEXCOORD_SUBAREA==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_subarea,
			 "texcoord_subarea",
			 { "p", "start_x", "end_x", "start_y", "end_y" },
			 { "P", "float", "float", "float", "float" },
			 { "", "0.0", "0.5", "0.0", "0.5" },
			 "P", "polygon_api", "texcoord_subarea"));
#endif
#if 0
  // doesnt work since wrapping at edge of texture works wrong
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_from_normal,
			 "texcoord_from_normal",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "texcoord_from_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fix_texcoord,
			 "texcoord_fix",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "fix_texcoord"));
#endif
#if (ALL==1)||(TEXCOORD_SPHERICAL2==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_spherical2,
			 "texcoord_spherical",
			 { "ev", "center", "r", "orig" },
			 { "EveryApi&", "PT", "float", "P" },
			 { "ev", "", "100.0", "" },
			 "P", "polygon_api", "texcoord_spherical2"));
#endif
#endif
#if (ALL==1)||(TEXTURE_SPLITTER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_splitter,
			 "p_tex_splitter",
			 { "obj", "start_index", "end_index" },
			 { "P", "int", "int" },
			 { "", "0", "16" },
			 "P", "polygon_api", "texture_splitter"));
#endif
#ifndef STABLE
#if (ALL==1)||(TEXTURE_STORAGE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_storage,
			 "p_tex_storage",
			 { "obj", "texture_sx", "texture_sy" },
			 { "P", "int", "int" },
			 { "", "1024", "1024" },
			 "P", "polygon_api", "texture_storage"));
#endif
#if (ALL==1)||(LIGHT_TRANSPORT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::light_transport,
			 "p_tex_light",
			 { "obj", "num", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "P", "int", "float", "float", "float" },
			 { "", "10000", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "light_transport"));
#endif
#if (ALL==1)||(TEXTURE_FROM_P==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_from_p,
			 "texture_from_p",
			 { "obj", "num" },
			 { "P", "int" },
			 { "", "0" },
			 "BM", "polygon_api", "texture_from_p"));
#endif
#if (ALL==1)||(REPLACE_TEXTURE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::replace_texture,
			 "p_tex_replace",
			 { "obj", "bm", "num" },
			 { "P", "BM", "int" },
			 { "", "", "0" },
			 "P", "polygon_api", "replace_texture"));
#endif
  #if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::log_coords,
			 "log_coords",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "log_coords"));
  #endif
#if (ALL==1)||(LOG_COORDS2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::log_coords2,
			 "log_coords",
			 { "p", "x_count", "y_count", "sx", "sy", "sz" },
			 { "P", "int", "int", "float", "float", "float" },
			 { "", "5", "5", "30.0", "30.0", "30.0" },
			 "P", "polygon_api", "log_coords2"));
#endif
#if (ALL==1)||(SPHERICAL_WAVE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spherical_wave,
			 "sph_wave",
			 { "p", "r1", "freq_1", "r2", "freq_2" },
			 { "P", "float", "float", "float", "float" },
			 { "", "30.0", "1.0", "30.0", "3.0" },
			 "P", "polygon_api", "spherical_wave"));
#endif
#endif
#if (ALL==1)||(SPLITQUADS==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::splitquads,
			 "splitquads",
			 { "p", "x_count", "y_count" },
			 { "P", "int", "int" },
			 { "", "5", "5" },
			 "P", "polygon_api", "splitquads"));
#endif
#if (ALL==1)||(X_SPLIT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::x_split,
			 "x_split",
			 { "p", "x", "x_0", "x_1" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "-30.0", "30.0" },
			 "P", "polygon_api", "x_split"));
#endif
#if (ALL==1)||(Y_SPLIT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::y_split,
			 "y_split",
			 { "p", "y", "y_0", "y_1" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "-30.0", "30.0" },
			 "P", "polygon_api", "y_split"));
#endif
#if (ALL==1)||(Z_SPLIT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::z_split,
			 "z_split",
			 { "p", "z", "z_0", "z_1" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "-30.0", "30.0" },
			 "P", "polygon_api", "z_split"));
#endif
#ifndef STABLE
#if (ALL==1)||(SHADOW==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow,
			 "shadow",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(1.0,1.0,1.0)" },
			 "P", "polygon_api", "shadow"));
#endif
#if (ALL==1)||(REFLECTION==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::reflection,
			 "reflection",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(0.0, 1.0,0.0)" },
			 "P", "polygon_api", "reflection"));
#endif
#if (ALL==1)||(SUBPOLY_CHANGE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::subpoly_change,
			 "subpoly_change",
			 { "p", "p2", "o" },
			 { "P", "P", "O" },
			 { "", "", "" },
			 "P", "polygon_api", "subpoly_change"));
#endif
#endif
#if (ALL==1)||(MEMOIZE==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::memoize,
			 "memoize",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "memoize"));
#endif
#if (ALL==1)||(PERSISTENT_CACHE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::persistent_cache,
			 "persistent_cache",
			 { "p", "filename" },
			 { "P", "std::string" },
			 { "", "cache.obj" },
			 "P", "polygon_api", "persistent_cache"));
#endif
#ifndef STABLE
#if (ALL==1)||(WORLD_FROM_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::EveryApi&,std::vector<GameApi::P>,std::string,std::string,float,float,int,int))&GameApi::PolygonApi::world_from_bitmap,
			 "world_from_bitmap",
			 { "ev", "pieces", "filename", "chars", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "[P]", "std::string", "std::string", "float", "float", "int", "int" },
			 { "ev", "", "map.txt", ".0123456789", "100.0", "100.0", "5", "5" },
			 "P", "polygon_api", "world_from_bitmap"));
#endif
#if (ALL==1)||(P_WORLD_FROM_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::p_world_from_bitmap,
			 "p_world_from_bitmap",
			 { "ev", "vec", "world", "pos_x", "pos_y", "pos_z", "dx", "dz", "y" },
			 { "EveryApi&", "[P]", "IBM", "float", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "0.0", "0.0", "0.0", "100.0", "100.0", "-150.0" },
			 "P", "bitmap_api", "p_world_from_bitmap"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_points,
			 "from_points",
			 { "p", "f" },
			 { "PTS", "std::function<P (int,float,float,float,unsigned int)>" },
			 { "", "" },
			 "P", "polygon_api", "from_points"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_lines,
			 "from_lines",
			 { "li", "f" },
			 { "LI", "std::function<P (int,float,float,float,float,float,float,float,unsigned int,unsigned int)>" },
			 { "", "" },
			 "P", "polygon_api", "from_lines"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_polygon,
			 "from_polygon",
			 { "p", "f" },
			 { "P", "std::function<P (int,PT,PT,PT,PT)>" },
			 { "", "" },
			 "P", "polygon_api", "from_polygon"));
#endif
#if (ALL==1)||(RENDERPOLYTOBITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::renderpolytobitmap,
			 "p_to_bitmap",
			 { "ev", "p", "sh", "x", "y", "z", "sx", "sy" },
			 { "EveryApi&", "P", "SH", "float", "float", "float", "int", "int" },
			 { "ev", "", "", "0.0", "0.0", "0.0", "800", "600" },
			 "BM", "polygon_api", "renderpolytobitmap"));
#endif
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shading_shader,
			 "p_shading",
			 { "ev", "mainloop", "level1", "level2", "level3", "spec_size", "ambient", "diffuse", "specular" },
			 { "EveryApi&", "ML", "unsigned int", "unsigned int", "unsigned int", "float", "bool", "bool", "bool" },
			 { "ev", "", "ff442211", "ffff8844", "ffffffff", "15.0", "true", "true", "true" },
			 "ML", "polygon_api", "shading_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::choose_color_shader,
			 "p_color",
			 { "ev", "mainloop", "color", "mix_val" },
			 { "EveryApi&", "ML", "unsigned int", "float" },
			 { "ev", "", "ffaa9988", "0.5" },
			 "ML", "polygon_api", "choose_color_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spotlight_shader,
			 "p_spotlight",
			 { "ev", "mainloop", "light_color_id", "move" },
			 { "EveryApi&", "ML", "int", "MN" },
			 { "ev", "", "0", "" },
			 "ML", "polygon_api", "spotlight_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::noise_shader,
			 "p_noise",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "noise_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::dither_shader,
			 "p_dither",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "dither_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::light_shader,
			 "p_light",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "light_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::toon_shader,
			 "p_toon",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "toon_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::blur_shader,
			 "p_blur",
			 { "ev", "mainloop", "x_val", "y_val" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "5", "0" },
			 "ML", "polygon_api", "blur_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bloom1_shader,
			 "p_bloom_cut",
			 { "ev", "mainloop", "r", "g", "b" },
			 { "EveryApi&", "ML", "float", "float", "float" },
			 { "ev", "", "0.5", "0.5", "0.5" },
			 "ML", "polygon_api", "bloom1_shader"));
#endif
#if (ALL==1)||(WAVE_SHADER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::wave_shader,
			 "p_wave",
			 { "ev", "mainloop", "radius", "t_mult", "x_mult", "y_mult" },
			 { "EveryApi&", "ML", "float", "float", "float", "float" },
			 { "ev", "", "0.02", "1.5", "17", "9" },
			 "ML", "polygon_api", "wave_shader"));
#endif
#if (ALL==1)||(FOG_SHADER==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fog_shader,
			 "p_fog",
			 { "ev", "mainloop", "fog_dist", "dark_color", "light_color" },
			 { "EveryApi&", "ML", "float", "unsigned int", "unsigned int" },
			 { "ev", "", "300.0", "ff000000", "ffffffff" },
			 "ML", "polygon_api", "fog_shader"));
#endif
#if (ALL==1)||(SHADER_CHOICE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::shader_api, &GameApi::ShaderApi::shader_choice,
			 "shader",
			 { "ev", "choose" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "SH", "shader_api", "shader_choice"));
#endif
#endif
  return vec;
}
