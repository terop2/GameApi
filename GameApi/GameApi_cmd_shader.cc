
#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> shaderapi_functions()
{
  std::vector<GameApiItem*> vec;
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_shader,
			 "load_shader",
			 { "shader_url" },
			 { "std::string" },
			 { "https://tpgames.org/test.vert" },
			 "SHC", "mainloop_api", "load_shader"));
  */
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_shader2,
			 "load_shader2",
			 { "vertex_url", "fragment_url" },
			 { "std::string", "std::string" },
			 { "https://tpgames.org/test.vert", "https://tpgames.org/test.frag" },
			 "SHC,SHC", "mainloop_api", "load_shader2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::empty_shaderI,
			 "shi_empty",
			 { },
			 { },
			 { },
			 "SHI", "mainloop_api", "empty_ShaderI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::generic_anim_shader2,
			 "shi_generic",
			 { "ev", "params", "funcname", "code", "children" },
			 { "EveryApi&", "SHP", "std::string", "SHC", "[SHI]" },
			 { "ev", "", "test", "", "" },
			 "SHI", "mainloop_api", "generic_anim_shader2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::phong_vertex,
			 "shi_phong_v",
			 { "next" },
			 { "SHI" },
			 { "" },
			 "SHI", "mainloop_api", "phong_vertex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::phong_fragment,
			 "shi_phong_f",
			 { "next", "ambient", "highlight", "pow" },
			 { "SHI", "unsigned int", "unsigned int", "float" },
			 { "", "ffff8800", "ff888888", "5" },
			 "SHI", "mainloop_api", "phong_fragment"));
 

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::empty_shp,
			 "shp_empty",
			 { },
			 { },
			 { },
			 "SHP", "mainloop_api", "empty_shp"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_f,
			 "shp_f",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "float", "float" },
			 { "0.0", "30.0", "", "0", "0.0", "1.0" },
			 "SHP", "mainloop_api", "timed_shp_f"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_i,
			 "shp_i",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "int", "int" },
			 { "0.0", "30.0", "", "0", "0", "255" },
			 "SHP", "mainloop_api", "timed_shp_i"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_u,
			 "shp_u",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "unsigned int", "unsigned int" },
			 { "0.0", "30.0", "", "0", "ff000000", "ffffffff" },
			 "SHP", "mainloop_api", "timed_shp_u"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_p3d,
			 "shp_p3d",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "PT", "PT" },
			 { "0.0", "30.0", "", "0", "", "" },
			 "SHP", "mainloop_api", "timed_shp_p3d"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_uvw,
			 "shp_uvw",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "PT", "PT" },
			 { "0.0", "30.0", "", "0", "", "" },
			 "SHP", "mainloop_api", "timed_shp_uvw"));
		
		       
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material00,
			 "m_generic_s", 
			 { "ev", "next", "vertex", "fragment", "vec" },
			 { "EveryApi&", "MT", "SHI", "SHI", "[TXID]" },
			 { "ev", "", "", "", "" },
			 "MT", "materials_api", "generic_shader_material00"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material0,
			 "m_generic",
			 { "ev", "next", "funcname", "vertex_url", "fragment_url" },
			 { "EveryApi&", "MT", "std::string", "std::string", "std::string" },
			 { "ev", "", "test", "https://tpgames.org/test.vert", "https://tpgames.org/test.frag" },
			 "MT", "materials_api", "generic_shader_material0"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material1,
			 "m_generic_p",
			 { "ev", "next", "params", "funcname", "vertex_url", "fragment_url" },
			 { "EveryApi&", "MT", "SHP", "std::string", "std::string", "std::string" },
			 { "ev", "", "", "test", "https://tpgames.org/test.vert", "https://tpgames.org/test.frag" },
			 "MT", "materials_api", "generic_shader_material1"));
			 
			 
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material2,
			 "m_generic_pc",
				 { "ev", "next", "params", "funcname", "vertex_url", "fragment_url", "children" },
				 { "EveryApi&", "MT", "SHP", "std::string", "std::string", "std::string", "[SHI]" },
				 { "ev", "", "", "test", "https://tpgames.org/test.vert", "https://tpgames.org/test.frag", "" },
			 "MT", "materials_api", "generic_shader_material2"));
			 

	  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::screenspace_rendering,
				 "screenspace",
				 { "ev", "scene", "s_material" },
				 { "EveryApi&", "ML", "SMT" },
				 { "ev", "", "" },
				 "ML", "mainloop_api", "screenspace_rendering"));
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::ss_def,
				 "ss_def",
				 { "ev" },
				 { "EveryApi&" },
				 { "ev" },
				 "SMT", "materials_api", "ss_def"));
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_screenspace_material00,
				 "ss_generic",
				 { "ev", "next", "vertex", "fragment" },
				 { "EveryApi&", "SMT", "SHI", "SHI" },
				 { "ev", "", "","" },
				 "SMT", "materials_api", "generic_screenspace_material00"));
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::screenspace_bloom,
				 "ss_bloom",
				 { "ev", "next", "cut_x", "cut_y", "cut_z", "x_amount", "y_amount" },
				 { "EveryApi&", "SMT", "float", "float", "float", "float", "float" },
				 { "ev", "", "0.7", "0.7", "0.7", "0.01", "0.01" },
				 "SMT", "materials_api", "screenspace_bloom"));
	  return vec;
}
