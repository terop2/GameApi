#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> linesapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::li_url,
			 "li_url",
			 { "url" },
			 { "std::string" },
			 { "https://tpgames.org/li_example.txt" },
			 "LI", "lines_api", "li_url"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::point_array,
			 "li_points",
			 { "arr" },
			 { "[PT]" },
			 { "" },
			 "LI", "lines_api", "point_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::alt,
			 "li_alt",
			 { "arr", "index" },
			 { "[LI]", "int" },
			 { "", "0" },
			 "LI", "lines_api", "alt"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_from_pts,
			 "li_from_pts",
			 { "pts1", "dx", "dy", "dz" },
			 { "PTS", "float", "float", "float" },
			 { "", "0.0","0.01","0.0" },
			 "LI", "points_api", "li_from_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_pts,
			 "li_to_pts",
			 { "li", "pos" },
			 { "LI", "float" },
			 { "", "0.0" },
			 "PTS", "points_api", "li_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_pts2,
			 "li_to_pts2",
			 { "li" },
			 { "LI" },
			 { "" },
			 "PTS", "points_api", "li_pts2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color",
			 { "li", "color" },
			 { "LI", "unsigned int" },
			 { "", "ffffffff" },
			 "LI", "lines_api", "change_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color2",
			 { "li", "color_1", "color_2" },
			 { "LI", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "LI", "lines_api", "change_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::from_polygon,
			 "li_from_polygon",
			 { "poly" },
			 { "P" },
			 { "" },
			 "LI", "lines_api", "from_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::li_polygon,
			 "li_polygon",
			 { "li", "width" },
			 { "LI", "float" },
			 { "", "2.0" },
			 "P", "polygon_api", "li_polygon"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::li_bevel,
			 "li_bevel",
			 { "li", "p", "mix" },
			 { "LI", "P", "float" },
			 { "", "", "0.5" },
			 "LI", "lines_api", "li_bevel"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::fur,
			 "li_fur",
			 { "pts", "center", "dist" },
			 { "PTS", "PT", "float" },
			 { "", "", "10.0" },
			 "LI", "lines_api", "fur"));
#endif
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::random_angle,
			 "li_rand_angle",
			 { "lines", "max_angle" },
			 { "LI", "float" },
			 { "", "0.3" },
			 "LI", "lines_api", "random_angle"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::random_mesh_quad_lines,
			 "li_from_quads",
			 { "ev", "p", "count" },
			 { "EveryApi&", "P", "int" },
			 { "ev", "", "1000" },
			 "LI", "lines_api", "random_mesh_quad_lines"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::lines_from_quads,
			 "li_grid_from_quads",
			 { "p", "sx", "sy" },
			 { "P", "int", "int" },
			 { "", "5", "5" },
			 "LI", "lines_api", "lines_from_quads"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::border_from_bool_bitmap,
			 "border_from_bool_bitmap",
			 { "b", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "",   "-300.0", "300.0", "-300.0", "300.0", "0.0" },
			 "LI", "lines_api", "border_from_bool_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::translate,
			 "li_translate",
			 { "lines", "dx", "dy", "dz" },
			 { "LI", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "LI", "lines_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::scale,
			 "li_scale",
			 { "lines", "m_x", "m_y", "m_z" },
			 { "LI", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "LI", "lines_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::li_or_elem,
			 "li_or_elem",
			 { "li1", "li2" },
			 { "LI", "LI" },
			 { "", "" },
			 "LI", "lines_api", "li_or_elem"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::line_product,
			 "li_product",
			 { "li1", "li2" },
			 { "LI", "LI" },
			 { "", "" },
			 "P", "lines_api", "line_product"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::split_lines,
			 "li_split",
			 { "li", "dist" },
			 { "LI", "float" },
			 { "", "5.0" },
			 "LI", "lines_api", "split_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::twist_y,
			 "li_twist",
			 { "li", "y_0", "angle_per_y_unit" },
			 { "LI", "float", "float" },
			 { "", "0.0", "0.031459" },
			 "LI", "lines_api", "twist_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::update_ml,
			 "li_update",
			 { "lla", "li" },
			 { "LLA", "LI" },
			 { "", "" },
			 "ML", "lines_api", "update_ml"));

  
#endif
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::import_ifc,
			 "li_import_icf",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/" },
			 "LI", "lines_api", "import_ifc"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::line,
			 "c_line",
			 { "p1", "p2" },
			 { "PT", "PT" },
			 { "", "" },
			 "C", "curve_api", "line"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xy,
			 "c_circle_xy",
			 { "center", "r" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "C", "curve_api", "circle_xy"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xz,
			 "c_circle_xz",
			 { "center", "r" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "C", "curve_api", "circle_xz"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xy_wave,
			 "c_circle_wave",
			 { "start_r", "end_r", "wave", "length" },
			 { "float", "float", "WV", "float" },
			 { "200.0", "220.0", "", "6.283" },
			 "C", "curve_api", "circle_xy_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::linear,
			 "c_linear",
			 { "vec" },
			 { "[PT]" },
			 { "" },
			 "C", "curve_api", "linear"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::bezier,
			 "c_bezier",
			 { "vec" },
			 { "[PT]" },
			 { "" },
			 "C", "curve_api", "bezier"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::scale,
			 "c_scale",
			 { "curve", "sx", "sy", "sz" },
			 { "C", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "C", "curve_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::trans,
			 "c_trans",
			 { "curve", "dx", "dy", "dz" },
			 { "C", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "C", "curve_api", "trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::compose,
			 "c_compose",
			 { "vec" },
			 { "[C]" },
			 { "" },
			 "C", "curve_api", "compose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::change_length,
			 "c_change_length",
			 { "curve", "new_legnth" },
			 { "C", "float" },
			 { "", "1.0" },
			 "C", "curve_api", "change_length"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::split,
			 "c_split",
			 { "curve", "start", "end" },
			 { "C", "float", "float" },
			 { "", "0.0", "1.0" },
			 "C", "curve_api", "split"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::sample,
			 "c_sample",
			 { "curve", "num_samples" },
			 { "C", "int" },
			 { "", "10" },
			 "PTS", "curve_api", "sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::to_lines,
			 "c_render",
			 { "curve", "num_lines" },
			 { "C", "int" },
			 { "", "40" },
			 "LI", "curve_api", "to_lines"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::curve_product,
			 "c_product",
			 { "c1", "c2", "point" },
			 { "C", "C", "PT" },
			 { "", "", "" },
			 "PA", "curve_api", "curve_product"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::curve_group_from_lines,
			 "cg_lines",
			 { "li" },
			 { "LI" },
			 { "" },
			 "CG", "curve_api", "curve_group_from_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::lines_from_curve_group,
			 "cg_to_li",
			 { "curvegroup", "split" },
			 { "CG", "int" },
			 { "", "3" },
			 "LI", "curve_api", "lines_from_curve_group"));
#endif
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::meshanim,
			 "ma_p",
			 { "vec", "start_time", "end_time"},
			 { "[P]", "float", "float" },
			 { "", "0.0", "10.0" },
			 "MA", "polygon_api", "meshanim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::meshanim_mesh,
			 "ma_to_p",
			 { "anim", "time" },
			 { "MA", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "meshanim_mesh"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::anim,
			 "ma_render",
			 { "ev", "next", "anim", "start_time", "end_time", "count" },
			 { "EveryApi&", "ML", "MA", "float", "float", "int" },
			 { "ev", "", "", "0.0", "10.0", "3" },
			 "ML", "polygon_api", "anim"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::anim_bind,
			 "ma_bind",
			 { "ev", "next", "anim", "material", "start_time", "end_time", "count" },
			 { "EveryApi&", "ML", "MA", "MT", "float", "float", "int" },
			 { "ev", "", "", "", "0.0", "10.0", "3" },
			 "ML", "polygon_api", "anim_bind"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::curve_group_from_anim,
			 "ma_to_cg",
			 { "meshanim", "start_time", "end_time" },
			 { "MA", "float", "float" },
			 { "", "0.0", "100.0" },
			 "CG", "polygon_api", "curve_group_from_anim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::patch_sample,
			 "pa_sample",
			 { "patch", "sx", "sy" },
			 { "PA", "int", "int" },
			 { "", "10", "10" },
			 "P", "curve_api", "patch_sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::x_curve,
			 "pa_x",
			 { "patch", "y" },
			 { "PA", "float" },
			 { "", "0.0" },
			 "C", "curve_api", "x_curve"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::y_curve,
			 "pa_y",
			 { "patch", "x" },
			 { "PA", "float" },
			 { "", "0.0" },
			 "C", "curve_api", "y_curve"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::xy_sum,
			 "cpp_sum",
			 { },
			 { },
			 { },
			 "CPP", "curve_api", "xy_sum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::xy_sum2,
			 "cpp_sum2",
			 { "xmult", "ymult", "zmult" },
			 { "float", "float", "float" },
			 { "1.0", "1.0", "1.0" },
			 "CPP", "curve_api", "xy_sum2"));
#endif
  
  return vec;
}
