
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
			 { "https://meshpage.org/assets/test.vert" },
			 "SHC", "mainloop_api", "load_shader"));
  */
#if (ALL==1)||(LOAD_SHADER2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_shader2,
			 "load_shader2",
			 { "vertex_url", "fragment_url" },
			 { "std::string", "std::string" },
			 { "https://meshpage.org/assets/test.vert@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "https://meshpage.org/assets/test.frag@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "SHC,SHC", "mainloop_api", "load_shader2"));
#endif
#if (ALL==1)||(EMPTY_SHADERI==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::empty_shaderI,
			 "shi_empty",
			 { },
			 { },
			 { },
			 "SHI", "mainloop_api", "empty_ShaderI"));
#endif
#if (ALL==1)||(GENERIC_ANIM_SHADER2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::generic_anim_shader2,
			 "shi_generic",
			 { "ev", "params", "funcname", "code", "children" },
			 { "EveryApi&", "SHP", "std::string", "SHC", "[SHI]" },
			 { "ev", "", "test", "", "" },
			 "SHI", "mainloop_api", "generic_anim_shader2"));
#endif
#if (ALL==1)||(PHONG_VERTEX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::phong_vertex,
			 "shi_phong_v",
			 { "next" },
			 { "SHI" },
			 { "" },
			 "SHI", "mainloop_api", "phong_vertex"));
#endif
#if (ALL==1)||(PHONG_FRAGMENT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::phong_fragment,
			 "shi_phong_f",
			 { "next", "ambient", "highlight", "pow" },
			 { "SHI", "unsigned int", "unsigned int", "float" },
			 { "", "ffff8800", "ff888888", "5" },
			 "SHI", "mainloop_api", "phong_fragment"));
#endif 
#if (ALL==1)||(EMPTY_SHP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::empty_shp,
			 "shp_empty",
			 { },
			 { },
			 { },
			 "SHP", "mainloop_api", "empty_shp"));
#endif
#if (ALL==1)||(TIMED_SHP_F==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_f,
			 "shp_f",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "float", "float" },
			 { "0.0", "30.0", "", "0", "0.0", "1.0" },
			 "SHP", "mainloop_api", "timed_shp_f"));
#endif
#if (ALL==1)||(TIMED_SHP_I==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_i,
			 "shp_i",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "int", "int" },
			 { "0.0", "30.0", "", "0", "0", "255" },
			 "SHP", "mainloop_api", "timed_shp_i"));
#endif
#if (ALL==1)||(TIMED_SHP_U==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_u,
			 "shp_u",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "unsigned int", "unsigned int" },
			 { "0.0", "30.0", "", "0", "ff000000", "ffffffff" },
			 "SHP", "mainloop_api", "timed_shp_u"));
#endif
#if (ALL==1)||(TIMED_SHP_P3D==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_p3d,
			 "shp_p3d",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "PT", "PT" },
			 { "0.0", "30.0", "", "0", "", "" },
			 "SHP", "mainloop_api", "timed_shp_p3d"));
#endif
#if (ALL==1)||(TIMED_SHP_UVW==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_shp_uvw,
			 "shp_uvw",
			 { "start_time", "end_time", "next", "num", "start_value", "end_value" },
			 { "float", "float", "SHP", "int", "PT", "PT" },
			 { "0.0", "30.0", "", "0", "", "" },
			 "SHP", "mainloop_api", "timed_shp_uvw"));
#endif		
#if (ALL==1)||(GENERIC_SHADER_MATERIAL00==1)		       
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material00,
			 "m_generic_s", 
			 { "ev", "next", "vertex", "fragment", "vec" },
			 { "EveryApi&", "MT", "SHI", "SHI", "[TXID]" },
			 { "ev", "", "", "", "" },
			 "MT", "materials_api", "generic_shader_material00"));
#endif
#if (ALL==1)||(GENERIC_SHADER_MATERIAL0==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material0,
			 "m_generic",
			 { "ev", "next", "funcname", "vertex_url", "fragment_url" },
			 { "EveryApi&", "MT", "std::string", "std::string", "std::string" },
			 { "ev", "", "test", "https://meshpage.org/assets/test.vert@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "https://meshpage.org/assets/test.frag@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "MT", "materials_api", "generic_shader_material0"));
#endif			 
#if (ALL==1)||(GENERIC_SHADER_MATERIAL1==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material1,
			 "m_generic_p",
			 { "ev", "next", "params", "funcname", "vertex_url", "fragment_url" },
			 { "EveryApi&", "MT", "SHP", "std::string", "std::string", "std::string" },
			 { "ev", "", "", "test", "https://meshpage.org/assets/test.vert@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "https://meshpage.org/assets/test.frag@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0" },
			 "MT", "materials_api", "generic_shader_material1"));
#endif			 
#if (ALL==1)||(GENERIC_SHADER_MATERIAL2==1)			 
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_shader_material2,
			 "m_generic_pc",
				 { "ev", "next", "params", "funcname", "vertex_url", "fragment_url", "children" },
				 { "EveryApi&", "MT", "SHP", "std::string", "std::string", "std::string", "[SHI]" },
				 { "ev", "", "", "test", "https://meshpage.org/assets/test.vert@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "https://meshpage.org/assets/test.frag@TeroPulkkinen@https://creativecommons.org/licenses/by/3.0", "" },
			 "MT", "materials_api", "generic_shader_material2"));
#endif			 
#if (ALL==1)||(SCREENSPACE_RENDERING==1)
	  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::screenspace_rendering,
				 "screenspace",
				 { "ev", "scene", "s_material" },
				 { "EveryApi&", "ML", "SMT" },
				 { "ev", "", "" },
				 "ML", "mainloop_api", "screenspace_rendering"));
#endif
#if (ALL==1)||(SS_DEF==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::ss_def,
				 "ss_def",
				 { "ev" },
				 { "EveryApi&" },
				 { "ev" },
				 "SMT", "materials_api", "ss_def"));
#endif
#if (ALL==1)||(GENERIC_SCREENSPACE_MATERIAL00==1)
	  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::generic_screenspace_material00,
				 "ss_generic",
				 { "ev", "next", "vertex", "fragment" },
				 { "EveryApi&", "SMT", "SHI", "SHI" },
				 { "ev", "", "","" },
				 "SMT", "materials_api", "generic_screenspace_material00"));
#endif
#if (ALL==1)||(SCREENSPACE_BLOOM==1)
vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::screenspace_bloom,
				 "ss_bloom",
				 { "ev", "next", "cut_x", "cut_y", "cut_z", "x_amount", "y_amount" },
				 { "EveryApi&", "SMT", "float", "float", "float", "float", "float" },
				 { "ev", "", "0.7", "0.7", "0.7", "0.01", "0.01" },
				 "SMT", "materials_api", "screenspace_bloom"));
#endif
return vec;
}
