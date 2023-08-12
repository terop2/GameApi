#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
 std::vector<GameApiItem*> shadermoduleapi_functions()
{
  std::vector<GameApiItem*> vec;
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::PT,float))&GameApi::ShaderModuleApi::sphere,
			 "sh_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "(0.0,0.0,0.0)", "100.0" },
			 "SFO", "sh_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float, float, float, float,float))&GameApi::ShaderModuleApi::cube,
			 "sh_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO", "sh_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::rounded_cube,
			 "sh_rcube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r" },
			 { "float", "float", "float", "float", "float", "float","float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "20.0" },
			 "SFO", "sh_api", "rounded_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture,
			 "sh_texture",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture_box,
			 "texture_box",
			 { "start_x", "end_x", "start_y", "end_y", "start_x", "end_x" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO", "sh_api", "texture_box"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float,float,float,float,float, float,float))&GameApi::ShaderModuleApi::line,
			 "sh_line",
			 { "start_x", "start_y", "start_z", "end_x", "end_y", "end_z", "width1", "width2" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "100.0", "100.0", "0.0", "15.0", "15.0" },
			 "SFO", "sh_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::plane,
			 "sh_plane",
			 { "center", "u_x", "u_y" },
			 { "PT", "V", "V" },
			 { "", "", "" },
			 "SFO", "sh_api", "plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::torus,
			 "sh_torus",
			 { "radius_1", "radius_2" },
			 { "float", "float" },
			 { "30.0", "10.0" },
			 "SFO", "sh_api", "torus"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color,
			 "sh_color",
			 { "obj", "r", "g", "b", "a" },
			 { "SFO", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "SFO", "sh_api", "color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float))&GameApi::ShaderModuleApi::rot_x,
			 "rot_x",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_y,
			 "rot_y",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_z,
			 "rot_z",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_z"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::and_not,
			 "and_not",
			 { "obj", "not_obj" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO", "sh_api", "and_not"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::or_elem,
			 "sh_or_elem",
			 { "obj1", "obj2" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO", "sh_api", "or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::blend,
			 "sh_blend",
			 { "obj1", "obj2", "k" },
			 { "SFO", "SFO", "float" },
			 { "", "", "15.0" },
			 "SFO", "sh_api", "blend"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float,float,float))&GameApi::ShaderModuleApi::trans,
			 "sh_trans",
			 { "obj", "dx", "dy", "dz" },
			 { "SFO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "SFO", "sh_api", "trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bind_arg,
			 "bind_arg",
			 { "obj", "name", "value" },
			 { "SFO", "std::string", "std::string" },
			 { "", "a", "0" },
			 "SFO", "sh_api", "bind_arg","","Allows dynamic anims in the future"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color_from_normal,
			 "color_from_normal",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "color_from_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::stop_generation,
			 "stop_generation",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "stop_generation"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::mix_color,
			 "mix_color",
			 { "col1", "col2", "t" },
			 { "SFO", "SFO", "float" },
			 { "", "", "0.5" },
			 "SFO", "sh_api", "mix_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::grayscale,
			 "sh_grayscale",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "grayscale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::noise,
			 "sh_noise",
			 { "obj", "strength" },
			 { "SFO", "float" },
			 { "", "1.0" },
			 "SFO", "sh_api", "noise"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::ambient_occulsion,
			 "ambient_occulsion",
			 { "obj", "d", "i" },
			 { "SFO", "float", "float" },
			 { "", "1.0", "1.0" },
			 "SFO", "sh_api", "ambient_occulsion"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::soft_shadow,
			 "soft_shadow",
			 { "scene", "light_dir", "mint", "maxt", "k", "strong" },
			 { "SFO", "V", "float", "float", "float", "float" },
			 { "", "", "1.0", "5.0", "1.0", "3.0" },
			 "SFO", "sh_api", "soft_shadow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bounding_primitive,
			 "bounding_prim",
			 { "prim", "inside", "outside" },
			 { "SFO", "SFO", "SFO" },
			 { "", "", "" },
			 "SFO", "sh_api", "bounding_primitive","","Allows huge speedup in rendering of sfo"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::render,
			 "render",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::v_render,
			 "v_render",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "v_render"));
#endif

  
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::m_def,
			 "m_def",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "MT", "materials_api", "m_def"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::mt_alt,
			 "m_alt",
			 { "ev", "vec", "index" },
			 { "EveryApi&", "[MT]", "int" },
			 { "ev", "", "0" },
			 "MT", "materials_api", "mt_alt"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::mainloop_material,
			 "m_ml",
			 { "ev", "ml" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "MT", "mainloop_api", "mainloop_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::colour_material,
			 "m_colour",
			 { "ev", "mix" },
			 { "EveryApi&", "float" },
			 { "ev", "0.5" },
			 "MT", "materials_api", "colour_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture,
			 "m_texture",
			 { "ev", "bm", "mix" },
			 { "EveryApi&", "BM","float" },
			 { "ev", "","1.0" },
			 "MT", "materials_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::textureid,
			 "m_texture_id",
			 { "ev", "txid", "mix" },
			 { "EveryApi&", "TXID", "float" },
			 { "ev", "", "1.0" },
			 "MT", "materials_api", "textureid"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_many,
			 "m_texture_many",
			 { "ev", "vec", "mix" },
			 { "EveryApi&", "[BM]", "float" },
			 { "ev", "", "1.0" },
			 "MT", "materials_api", "texture_many"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_many2,
			 "m_texture_many_p",
			 { "ev", "mix" },
			 { "EveryApi&", "float" },
			 { "ev", "0.5" },
			 "MT", "materials_api", "texture_many2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::many_texture_id_material,
			 "m_mtl_many",
			 { "ev", "mtl_url", "url_prefix", "mix", "start_range", "end_range" },
			 { "EveryApi&", "std::string", "std::string", "float", "int", "int" },
			 { "ev", "http://tpgames.org/sponza/sponza.mtl@crytek@https://www.cryengine.com/ce-terms", "http://tpgames.org/sponza", "1", "0", "15" },
			 "MT", "materials_api", "many_texture_id_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_cubemap,
			 "m_texture_cubemap",
			 { "ev", "vec", "mix", "mix2" },
			 { "EveryApi&", "[BM]", "float","float" },
			 { "ev", "", "1.0", "1.0" },
			 "MT", "materials_api", "texture_cubemap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_arr,
			 "m_texture_arr",
			 { "ev", "vec", "sx", "sy", "mix" },
			 { "EveryApi&", "[BM]", "int", "int", "float" },
			 { "ev", "", "256", "256", "1.0" },
			 "MT", "materials_api", "texture_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::transparent_material,
			 "m_transparent",
			 { "ev", "bm", "next" },
			 { "EveryApi&", "BM", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "transparent_material"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::skeletal,
			 "m_skeletal",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "MT", "materials_api", "skeletal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shading1,
			 "m_shading1",
			 { "ev", "nxt", "mix_val1", "mix_val2" },
			 { "EveryApi&", "MT", "float", "float" },
			 { "ev", "", "0.95", "0.5" },
			 "MT", "materials_api", "shading1"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shading2,
			 "m_shading2",
			 { "ev", "nxt", "color1", "color2", "color3" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int" },
			 { "ev", "", "ffaaaaaa", "ffeeeeee", "ffffffff" },
			 "MT", "materials_api", "shading2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::adjust,
			 "m_adjust",
			 { "ev", "nxt", "color", "dark", "light" },
			 { "EveryApi&", "MT", "unsigned int", "float", "float" },
			 { "ev", "", "ffffffff", "0.0", "1.0" },
			 "MT", "materials_api", "adjust"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::phong,
			 "m_phong",
			 { "ev", "nxt", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow" },
			 { "EveryApi&", "MT", "float", "float", "float", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "0.0", "0.0", "1.0", "ffff8800", "ffffffff", "30.0" },
			 "MT", "materials_api", "phong"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::phong2,
			 "m_phong2",
			 { "ev", "nxt", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow" },
			 { "EveryApi&", "MT", "float", "float", "float", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "-0.3", "0.3", "-1.0", "ffff8800", "ffffffff", "30.0" },
			 "MT", "materials_api", "phong2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::phong3_material,
			 "m_phong3",
			 { "ev", "nxt", "ambient", "highlight", "pow" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "ffff8844", "ffffffff", "30.0" },
			 "MT", "materials_api", "phong3_material"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::vertex_phong,
			 "m_vertexphong",
			 { "ev", "nxt", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow", "mix" },
			 { "EveryApi&", "MT", "float", "float", "float", "unsigned int", "unsigned int", "float", "float" },
			 { "ev", "", "-0.3", "0.3", "-1.0", "ffff8800", "ffffffff", "30.0", "0.5" },
			 "MT", "materials_api", "vertex_phong"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::water,
			 "m_water",
			 { "ev", "nxt", "color1", "color2", "color3", "center_x", "center_y", "center_z", "wave_mult", "time_mult" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int", "float", "float", "float", "float", "float" },
			 { "ev", "", "ff2288ff", "ff0055ff", "ff0011ff", "0.0", "0.0", "0.0", "0.1", "-5" },
			 "MT", "materials_api", "water"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::glow_edge,
			 "m_glowedge",
			 { "ev", "nxt", "light_level", "gray_level", "edge_pos" },
			 { "EveryApi&", "MT", "float", "float", "float" },
			 { "ev", "", "0.6", "0.5", "0.03" },
			 "MT", "materials_api", "glow_edge"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api,&GameApi::MaterialsApi::m_apply_phong,
			 "m_apply_phong",
			 { "ev", "vec", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow" },
			 { "EveryApi&", "[MT]", "float", "float", "float", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "-0.3", "0.3", "-1.0", "ffff8800", "ff666666", "5.9" },
			 "[MT]", "materials_api", "m_apply_phong"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::toon_border,
			 "m_toon_border",
			 { "ev", "next", "border_width", "border_color" },
			 { "EveryApi&", "MT", "float", "unsigned int" },
			 { "ev", "", "2.00", "ffffffff" },
			 "MT", "materials_api", "toon_border"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::edge,
			 "m_edge",
			 { "ev", "nxt", "edge_width", "edge_color" },
			 { "EveryApi&", "MT", "float", "unsigned int" },
			 { "ev", "", "3.0", "ffffffff" },
			 "MT", "materials_api", "edge"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_material,
			 "m_gltf",
			 { "ev", "tf", "material_id", "mix", "light_x", "light_y", "light_z" },
			 { "EveryApi&", "TF", "int", "float", "float" ,"float", "float" }, 
			 { "ev", "", "0", "1.0", "0.0", "0.0", "-400.0" },
			 "MT", "materials_api", "gltf_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gltf_mesh_all_mt_arr,
			 "m_gltf_arr",
			 { "ev", "tf", "mix", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "EveryApi&", "TF", "float", "float", "float", "float" },
			 { "ev", "", "1.0", "0.0", "0.0", "-400.0" },
			 "[MT]", "materials_api", "gltf_mesh_all_mt_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_material_env,
			 "m_gltf_env",
			 { "ev", "tf", "material_id", "mix", "diffuse", "specular", "bfrd" },
			 { "EveryApi&", "TF", "int", "float", "BM", "BM", "BM" }, 
			 { "ev", "", "0", "1.0", "", "", "" },
			 "MT", "materials_api", "gltf_material_env"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_material_from_file,
			 "m_gltf_file",
			 { "ev", "url", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "EveryApi&", "std::string","float", "float", "float" },
			 { "ev", "https://tpgames.org/materials/wood.mat@AmbientCG@https://docs.ambientcg.com/books/website-licensing/page/license-information", "0.0", "0.0", "-400.0" },
			 "MT", "materials_api", "gltf_material_from_file"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_material_manual,
			 "m_gltf_man",
			 { "ev", "mix", "baseColor", "metalrough", "normal", "occlusion", "emissive", "baseColor_b", "metalrough_b", "normal_b", "occlusion_b", "emissive_b", "roughnessfactor", "metallicfactor", "base_r", "base_g", "base_b", "base_a", "occulsionstrength", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "EveryApi&", "float", "BM", "BM", "BM", "BM", "BM", "bool", "bool", "bool", "bool", "bool", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float" }, 
			 { "ev", "1.0", "", "", "", "", "", "true", "true", "true", "true", "true", "0.5", "0.5", "0.5", "0.5", "0.5", "1.0", "1.0", "0.0", "0.0", "-400.0" },
			 "MT", "materials_api", "gltf_material_manual"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_anim_material,
			 "m_gltf_anim",
			 { "ev", "tf", "skin_num", "animation", "num_timeindexes", "next", "key" },
			 { "EveryApi&", "TF", "int", "int", "int", "MT", "int" },
			 { "ev", "", "0", "0", "30", "", "32" },
			 "MT", "materials_api", "gltf_anim_material"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_anim_material2,
			 "m_gltf_anim2",
			 { "ev", "tf", "skin_num", "num_timeindexes", "next", "keys", "mode" },
			 { "EveryApi&", "TF", "int", "int", "MT", "std::string", "int" },
			 { "ev", "", "0", "150", "", "zxcvbnmfghjklertyuiop", "0" },
			 "MT", "materials_api", "gltf_anim_material2"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_material3,
			 "m_material",
			 { "ev", "roughness", "metallic", "base_r", "base_g", "base_b", "base_a", "mix", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "0.5", "0.8", "1.0", "1.0", "1.0", "1.0", "1.0", "0.0", "0.0", "-400.0" },
			 "MT", "materials_api", "gltf_material3"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bump_phong,
			 "m_bump_phong",
			 { "ev", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow", "fb", "bump_width" },
			 { "EveryApi&", "float", "float", "float", "unsigned int", "unsigned int", "float", "FB", "float" },
			 { "ev", "-0.3", "0.3", "-1.0", "ffff8800", "ffffffff", "10.0", "", "5.0" },
			 "MT", "materials_api", "bump_phong"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::fog,
			 "m_fog",
			 { "ev", "nxt", "dist", "dark_color", "light_color" },
			 { "EveryApi&", "MT", "float", "unsigned int", "unsigned int" },
			 { "ev", "", "300.0", "ff000000", "ffffffff" },
			 "MT", "materials_api", "fog"));
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::newshadow,
			 "m_shadow_directional",
			 { "ev", "nxt", "models", "light_dir_x", "light_dir_y", "light_dir_z", "dark_level", "light_level", "scale", "size" },
			 { "EveryApi&", "MT", "P", "float", "float", "float", "float", "float", "float", "int" },
			 { "ev", "", "", "-0.3", "-1.0", "-0.3", "0.7", "1.0", "1.0","128|256|512|1024|2048|4096" },
			 "MT", "materials_api", "newshadow"));
  */
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::newshadow2_phong,
			 "m_dir_shadow",
			 { "ev", "models", "model_mt", "shadow_mesh", "shadow_mt", "light_dir_x", "light_dir_y", "light_dir_z", "dark_level", "light_level", "dark_color", "light_color", "scale", "size", "drawmodel", "shadow2_mt" },
			 { "EveryApi&", "P", "MT", "P", "MT", "float", "float", "float", "float", "float", "unsigned int", "unsigned int", "float", "int", "bool", "MT" },
			 { "ev", "", "", "", "", "-0.3", "-1.0", "-0.3", "0.7", "1.0", "ff884422", "ffffffff", "0.8", "1024", "true", "" },
			 "ML", "materials_api", "newshadow2_phong"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::newshadow2_gltf,
			 "m_gltf_shadow",
			 { "ev", "model", "shadow_p", "shadow_mt", "light_dir_x", "light_dir_y", "light_dir_z", "dark_level", "light_level", "dark_color", "light_color", "scale", "size", "shadow2_mt" },
			 { "EveryApi&", "TF", "P", "MT", "float", "float", "float", "float", "float", "unsigned int", "unsigned int", "float", "int", "MT" },
			 { "ev", "", "", "", "-0.3", "-1.0", "-0.3", "0", "1", "ffff8800", "ffffffff", "0.8", "1024", "" },
			 "ML", "materials_api", "newshadow2_gltf"));
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gltf_newshadow2,
			 "m_gltf_shadow",
			 { "ev", "models", "model_mt", "shadow_mesh", "shadow_mt", "light_dir_x", "light_dir_y", "light_dir_z", "dark_level", "light_level", "scale", "size", "tf" },
			 { "EveryApi&", "P", "MT", "P", "MT", "float", "float", "float", "float", "float", "float", "int", "TF" },
			 { "ev", "", "", "", "", "-0.3", "-1.0", "-0.3", "0.7", "1.0", "0.8", "1024", "" },
			 "ML", "materials_api", "gltf_newshadow2"));
  */
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gi,
			 "m_gi",
			 { "ev", "nxt", "pts", "obj_size" },
			 { "EveryApi&", "MT", "PTS", "float" },
			 { "ev", "", "", "100.0" },
			 "MT", "materials_api", "gi"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::fade,
			 "m_fade",
			 { "ev", "next", "start_time", "end_time", "start_time2", "end_time2" },
			 { "EveryApi&", "MT", "float", "float", "float", "float" },
			 { "ev", "", "0.0", "10.0", "30.0", "40.0" },
			 "MT", "materials_api", "fade"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::combine_materials,
			 "m_or_elem",
			 { "ev", "mat1", "mat2" },
			 { "EveryApi&", "MT", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "combine_materials"));
#ifndef STABLE
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shadow,
			 "m_shadow",
			 { "ev", "p", "vec", "p_x", "p_y", "p_z", "sx", "sy", "dark_color", "mix", "mix2" },
			 { "EveryApi&", "P", "[BM]", "float", "float", "float", "int", "int", "unsigned int", "float", "float" },
			 { "ev", "", "", "0.0", "0.0", "0.0", "512", "512", "ff000000", "1.0", "0.5" },
			 "MT", "materials_api", "shadow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shadow2,
			 "m_shadow2",
			 { "ev", "p", "p_x", "p_y", "p_z", "sx", "sy", "dark_color", "mix", "mix2", "numtextures" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int", "unsigned int", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "512", "512", "ff000000", "1.0", "0.5", "0" },
			 "MT", "materials_api", "shadow2"));
  */
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::coloured_lights,
			 "m_color_lights",
			 { "ev", "nxt", "scale", "color_1", "color_2", "color_3", "color_4", "color_5", "color_6", "color_7", "color_8",
			    "pos_1", "pos_2", "pos_3", "pos_4", "pos_5", "pos_6", "pos_7", "pos_8",
			    "dist_1", "dist_2", "dist_3", "dist_4", "dist_5", "dist_6", "dist_7", "dist_8" },
			 { "EveryApi&", "MT", "float", "unsigned int","unsigned int","unsigned int","unsigned int","unsigned int","unsigned int","unsigned int","unsigned int",
			    "PT","PT","PT","PT","PT","PT","PT","PT",
			    "float","float","float","float","float","float","float","float"},
			 { "ev", "", "30.0", "ffffffff", "ff000000","ff000000","ff000000","ff000000","ff000000","ff000000","ff000000",
			    "","","","","","","","",
			    "350.0","350.0","350.0","350.0","350.0","350.0","350.0","350.0" },
			 "MT", "materials_api", "coloured_lights"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::dyn_lights,
			 "m_dyn_lights",
			 { "ev", "nxt", "light_pos_x", "light_pos_y", "light_pos_z", "dist", "dyn_point" },
			 { "EveryApi&", "MT", "float", "float", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "500.0", "-1" },
			 "MT", "materials_api", "dyn_lights"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::snow,
			 "m_snow",
			 { "ev", "nxt", "color1", "color2", "color3", "mix_val" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "ffaaaaaa", "ffeeeeee", "ffffffff", "0.95" },
			 "MT", "materials_api", "snow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::flat,
			 "m_flat",
			 { "ev", "nxt", "color1", "color2", "color3"  },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int" },
			 { "ev", "", "ff8888ff", "ffff4422", "ffffffff" },
			 "MT", "materials_api", "flat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::fur,
			 "m_fur",
			 { "ev", "nxt", "center", "dist", "max_angle", "count", "size", "cone_numfaces" },
			 { "EveryApi&", "MT", "PT", "float", "float", "int", "float", "int" },
			 { "ev", "", "", "60.0", "1.59", "1500", "2.0", "4" },
			 "MT", "materials_api", "fur"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::noise,
			 "m_noise",
			 { "ev", "sx", "sy", "r", "g", "b", "a", "r2", "g2", "b2", "a2", "mix" },
			 { "EveryApi&", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "float" },
			 { "ev", "1024", "1024", "255", "255", "255", "255", "0", "0", "0", "255", "0.5" },
			 "MT", "materials_api", "noise"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bloom,
			 "m_bloom",
			 { "ev", "nxt", "bm","r_cut", "g_cut", "b_cut", "pixel_x", "pixel_y" },
			 { "EveryApi&", "MT", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "0.7", "0.7", "0.7", "0.01", "0.01" },
			 "MT", "materials_api", "bloom"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::choose_color,
			 "m_choose_color",
			 { "ev", "nxt", "color", "mix_val" },
			 { "EveryApi&", "MT", "unsigned int", "float" },
			 { "ev", "", "ffff8844", "0.5" },
			 "MT", "materials_api", "choose_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::brashmetal,
			 "m_brashmetal",
			 { "ev", "nxt", "count", "web" },
			 { "EveryApi&", "MT", "int", "bool" },
			 { "ev", "", "80000", "true" },
			 "MT", "materials_api", "brashmetal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::marble,
			 "m_marble",
			 { "ev", "nxt", "count", "cubesize" },
			 { "EveryApi&", "MT", "int", "float" },
			 { "ev", "", "300", "10" },
			 "MT", "materials_api", "marble"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::web,
			 "m_web",
			 { "ev", "nxt", "mult", "linewidth", "linecolor" },
			 { "EveryApi&", "MT", "float", "float","unsigned int" },
			 { "ev", "", "1.03", "2.0","ff000000" },
			 "MT", "materials_api", "web"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bevel,
			 "m_bevel",
			 { "ev", "nxt", "dir", "linewidth" },
			 { "EveryApi&", "MT", "float", "float" },
			 { "ev", "", "-1.5", "2.0" },
			 "MT", "materials_api", "bevel"));
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::dist_field_mesh,
			 "m_dist_field",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "","" },
			 "MT", "materials_api", "dist_field_mesh","[B]","Loses animations from SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::mesh_color_from_sfo,
			 "m_dist_field_color",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "mesh_color_from_sfo"));
#endif

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_extractor_p,
			 "ext_p",
			 { "p", "start_index", "end_index", "num_slots", "current_slot" },
			 { "P", "int", "int", "int", "int" },
			 { "", "0", "32", "1", "0" },
			 "[P]", "polygon_api", "material_extractor_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_extractor_bm,
			 "ext_bm",
			 { "p", "start_index", "end_index" },
			 { "P", "int","int" },
			 { "", "0","32" },
			 "[BM]", "polygon_api", "material_extractor_bm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_extractor_mt,
			 "ext_mt",
			 { "ev", "p", "mix", "start_index", "end_index" },
			 { "EveryApi&", "P", "float", "int","int" },
			 { "ev", "", "1.0", "0", "32" },
			 "[MT]", "polygon_api", "material_extractor_mt"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_choose,
			 "ext_choose",
			 { "mat", "p" },
			 { "[MT]", "[P]" },
			 { "", "" },
			 "[MT]", "polygon_api", "material_choose"));
#endif
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl_materials,
			 "mtl_materials",
			 { "ev", "p" },
			 { "EveryApi&", "P" },
			 { "ev", "" },
			 "[MT]", "polygon_api", "p_mtl_materials"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl2_materials,
			 "mtl2_materials",
			 { "ev", "p" },
			 { "EveryApi&", "P" },
			 { "ev", "" },
			 "[MT]", "polygon_api", "p_mtl2_materials"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_index,
			 "mat_idx",
			 { "ev", "vec", "index" },
			 { "EveryApi&", "[MT]", "int" },
			 { "ev", "", "0" },
			 "MT", "polygon_api", "material_index"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::material_arr,
			 "mat_arr",
			 { "vec", "start_range", "end_range" },
			 { "[MT]", "int", "int" },
			 { "", "0", "32" },
			 "[MT]", "polygon_api", "material_arr"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::comb_mat,
			 "comb_mat",
			 { "ev", "vec1", "vec2" },
			 { "EveryApi&", "[MT]", "[MT]" },
			 { "ev", "", "" },
			 "[MT]", "polygon_api", "comb_mat"));

#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_sbm,
			 "sbm_many_texture",
			 { },
			 { },
			 { },
			 "SBM", "polygon_api", "texture_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sbm_circle,
			 "sbm_circle",
			 { "start_time", "end_time", "start_pos_x", "start_pos_y", "end_pos_x", "end_pos_y", "start_radius", "end_radius" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "0.0", "0.0", "100.0", "120.0" },
			 "SBM", "polygon_api", "sbm_circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::blur_sbm,
			 "sbm_blur",
			 { "texture", "pixel_x", "pixel_y" },
			 { "SBM", "float", "float" },
			 { "", "0.01", "0.01" },
			 "SBM", "polygon_api", "blur_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bloom_cut_sbm,
			 "sbm_bloom_cut",
			 { "texture","r_cut", "g_cut", "b_cut" },
			 { "SBM", "float", "float", "float" },
			 { "", "0.7", "0.7", "0.7" },
			 "SBM", "polygon_api", "bloom_cut_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::combine_sbm,
			 "sbm_combine",
			 { "texture1", "texture2" },
			 { "SBM", "SBM" },
			 { "", "" },
			 "SBM", "polygon_api", "combine_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sfo_sbm,
			 "sfo_sbm",
			 { "ev", "sfo" },
			 { "EveryApi&", "SFO" },
			 { "ev", "" },
			 "SBM", "polygon_api", "sfo_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sbm_texture,
			 "p_sbm",
			 { "ev", "mainloop", "bitmap" },
			 { "EveryApi&", "ML", "SBM" },
			 { "ev", "", "" },
			 "ML", "polygon_api", "sbm_texture"));

#endif
  
  return vec;

}


#if 0
// returns comma-separated string, and vectors for parameter name,type and default values
std::tuple<std::string,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>> > get_list_of_functions(std::vector<GameApiItem*> vec, std::string return_type, std::string param_type)
{
  std::string res="";
  int s = vec.size();
  std::vector<std::vector<std::string>> m_vec1,m_vec2,m_vec3;
 
  for(int i=0;i<s;i++)
    {
      GameApiItem *item = vec[i];
      std::string rettype = item->ReturnType(0);
      int s2 = item->ParamCount(0);
      bool is_param=false;
      int param_num = -1;
      std::vector<std::string> vec1,vec2,vec3;
      for(int j=0;j<s2;j++) {
	if (item->ParamType(0,j)==param_type) { is_param=true; param_num=j; break; } else { vec1.push_back(item->ParamName(0,j)); vec2.push_back(item->ParamType(0,j)); vec3.push_back(item->ParamDefault(0,j));  }
      }
      if (is_param && rettype==return_type) {
	res+=item->Name(0);
	res+=",";
	m_vec1.push_back(vec1);
	m_vec2.push_back(vec2);
	m_vec3.push_back(vec3);
      }
    }
  res.pop_back();
  return std::make_tuple(res,m_vec1,m_vec2,m_vec3);
}


GameApi::MT GameApi::MaterialsApi::m_apply(int e, GameApi::MT mt, std::vector<std::string> params)
{
  
}

class ApiItemF2 : public GameApiItem
{
public:
  ApiItemF2(std::tuple<std::string,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>> > tuple, std::string return_type, std::string apiname, std::string funcname) : tuple(tuple),return_type(return_type),apiname(apiname), funcname(funcname) { }
  void change_enum(int i) { m_num = i; }
  
  virtual int Count() const { return 1; }
  virtual std::string Name(int i) const { return name; }
  virtual int ParamCount(int i) const { return 1+tuple[1].size(); }
  virtual std::string ParamName(int i, int p) const
  {
    if (p==0) {
      return "choose";
    }
    return tuple[1][m_num][p-1];
  }
  virtual std::string ParamType(int i, int p) const
  {
    if (p==0) {
      return std::string("{") + tuple[0] + std::string("}");
    }
    return tuple[2][m_num][p-1];
  }
  virtual std::string ParamDefault(int i, int p) const
  {
    if (p==0) {
      return "0";
    }
    return tuple[3][m_num][p-1];
  }
  virtual std::string ReturnType(int i) const
  {
    return return_type;
  }
  virtual std::string ApiName(int i) const { return apiname; }
  virtual std::string FuncName(int i) const { return funcname; }
  virtual std::string Symbols() const { return ""; }
  virtual std::string Comment() const { return ""; }
  virtual int Execute(std::stringstream &ss, GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e, int j)=0;
  //virtual std::vector<GameApi::EditNode*> CollectNodes(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)=0;
  virtual std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)=0;
private:
  std::tuple<std::string,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>>,std::vector<std::vector<std::string>> > tuple;
  int m_num = 0;
  std::string return_type;
  std::string apiname;
  std::string funcname;
};
#endif
