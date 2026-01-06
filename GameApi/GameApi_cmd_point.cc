#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> pointapi_functions()
{
  std::vector<GameApiItem*> vec;
#if (ALL==1)||(POINT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, &GameApi::PointApi::point,
			 "point",
			 { "x", "y", "z" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "PT", "point_api", "point"));
#endif
#if (ALL==1)||(MOVE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, (GameApi::PT (GameApi::PointApi::*)(GameApi::PT,float,float,float))&GameApi::PointApi::move,
			 "move",
			 { "p1", "dx", "dy", "dz" },
			 { "PT", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PT", "point_api", "move"));
#endif
#if (ALL==1)||(MIX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, &GameApi::PointApi::mix,
			 "mix",
			 { "p1", "p2", "val" },
			 { "PT", "PT", "float" },
			 { "", "", "0.5" },
			 "PT", "point_api", "mix"));

#endif
#ifndef STABLE
#if (ALL==1)||(DEFAULT_CMDS==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::default_cmds,
			 "cmd_def",
			 { "dx", "dy", "dz" },
			 { "float", "float", "float" },
			 { "10.0", "10.0", "10.0" },
			 "CMD", "move_api", "default_cmds"));
#endif
#if (ALL==1)||(CMD_REPEAT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_repeat,
			 "cmd_repeat",
			 { "cmds", "repeat", "dx", "dy", "dz" },
			 { "CMD", "std::string", "float", "float", "float" },
			 { "", ".dd.", "10.0", "10.0", "10.0" },
			 "CMD", "move_api", "cmd_repeat"));
#endif
#if (ALL==1)||(CMD_ROTATE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_rotate,
			 "cmd_rotate",
			 { "cmds", "v_x", "v_y", "v_z", "angle", "delta_angle" },
			 { "CMD", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "1.0", "0.0", "1.57" },
			 "CMD", "move_api", "cmd_rotate"));
#endif
#if (ALL==1)||(CMD_TO_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_pts,
			 "cmd_to_pts",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "PTS", "move_api", "cmd_to_pts"));
#endif
#if (ALL==1)||(CMD_TO_LI==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_li,
			 "cmd_to_li",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "LI", "move_api", "cmd_to_li"));
#endif
#if (ALL==1)||(CMD_TO_PLF==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_plf,
			 "cmd_to_plf",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "PLF", "move_api", "cmd_to_plf"));
#endif
#endif
#if (ALL==1)||(VOXEL_FROM_BITMAPS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_from_bitmaps,
			 "vx_from_bm",
			 { "vec", "url" },
			 { "[BM]", "std::string" },
			 { "", "https://meshpage.org/test.col" },
			 "VX", "voxel_api", "voxel_from_bitmaps"));
#endif
#if (ALL==1)||(EMPTY_VOXEL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::empty_voxel,
			 "vx_empty",
			 { "sx", "sy", "sz" },
			 { "int", "int", "int" },
			 { "30", "30", "30" },
			 "VX", "voxel_api", "empty_voxel"));
#endif
#if (ALL==1)||(VOX_VOXEL2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_voxel2,
			 "vox_vox",
			 { "ev", "url", "model", "sx", "sy", "sz" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0" },
			 "VX", "voxel_api", "vox_voxel2"));
#endif  
#if (ALL==1)||(VOX_CUBES==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_cubes,
			 "vox_cubes",
			 { "ev", "url", "model", "sx", "sy", "sz" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0" },
			 "[P]", "voxel_api", "vox_cubes"));
#endif
#if (ALL==1)||(VOX_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_ml,
			 "vox_ml",
			 { "ev", "url", "model", "sx", "sy", "sz" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0" },
			 "ML", "voxel_api", "vox_ml"));
#endif
#if (ALL==1)||(VOX_ML2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_ml2,
			 "vox_ml2",
			 { "ev", "url", "model", "sx", "sy", "sz" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0" },
			 "ML", "voxel_api", "vox_ml2"));
#endif
#if (ALL==1)||(VOX_BIND_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_bind_ml,
			 "vox_bind_ml",
			 { "ev", "url", "model", "sx", "sy", "sz", "material" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float", "MT" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0", "" },
			 "ML", "voxel_api", "vox_bind_ml"));
#endif
#if (ALL==1)||(VOX_BIND_ML2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vox_bind_ml2,
			 "vox_bind_ml2",
			 { "ev", "url", "model", "sx", "sy", "sz", "material" },
			 { "EveryApi&", "std::string", "int", "float", "float", "float", "MT" },
			 { "ev", "https://meshpage.org/assets/test.vox", "0", "20.0", "20.0", "20.0", "" },
			 "ML", "voxel_api", "vox_bind_ml2"));
#endif
#if (ALL==1)||(VX_TO_OVX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::vx_to_ovx,
			 "vx_to_ovx",
			 { "voxel", "cubes"},
			 { "VX", "[P]" },
			 { "", "" },
			 "OVX", "voxel_api", "vx_to_ovx"));
#endif
#if (ALL==1)||(REMOVE_NOT_ENABLED==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::remove_not_enabled,
			 "remove_empty_ovx",
			 { "voxel" },
			 { "OVX" },
			 { "" },
			 "OVX", "voxel_api", "remove_not_enabled"));
#endif

#if (ALL==1)||(RENDER_OVX==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::render_ovx,
			 "render_ovx",
			 { "ev", "voxel", "material", "sx", "sy", "sz" },
			 { "EveryApi&", "OVX", "MT", "float", "float", "float" },
			 { "ev", "","", "20.0", "20.0", "20.0" },
			 "ML", "voxel_api", "render_ovx"));
#endif

#if (ALL==1)||(SUBVOXEL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::subvoxel,
			 "vx_sub",
			 { "voxel", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "VX", "int", "int", "int", "int", "int", "int" },
			 { "", "0", "100", "0", "100", "0", "100" },
			 "VX", "voxel_api", "subvoxel"));
#endif
#if (ALL==1)||(VOXEL_LANDSCAPE_FROM_FBM==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_landscape_from_fbm,
			 "vx_landscape_bm",
			 { "bitmap", "height", "false_val", "true_val" },
			 { "FB", "int", "int", "int" },
			 { "", "30", "-1", "0" },
			 "VX", "voxel_api", "voxel_landscape_from_fbm"));
#endif
#if (ALL==1)||(BLIT_VOXEL2==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::blit_voxel2,
			 "vx_or_elem",
			 { "v1", "v2", "p_x", "p_y", "p_z" },
			 { "VX", "VX", "int", "int", "int" },
			 { "", "", "0", "0", "0" },
			 "VX", "voxel_api", "blit_voxel2"));
#endif
#if (ALL==1)||(BLIT_VOXEL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::blit_voxel,
			 "vx_blit",
			 { "object", "sx","sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "false_value", "true_value" },
			 { "O", "int", "int", "int", "float","float", "float", "float", "float", "float", "int", "int" },
			 { "", "30", "30", "30", "-200.0", "200.0", "-200.0", "200.0", "-200.0", "200.0", "-1", "0" },
			 "VX", "voxel_api", "blit_voxel"));
#endif
#if (ALL==1)||(VOXEL_INSTANCING==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_instancing,
			 "vx_inst",
			 { "voxel", "count", "start_x", "end_x", "start_y","end_y", "start_z", "end_z" },
			 { "VX", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "1", "-200.0","200.0", "-200.0", "200.0", "-200.0", "200.0" },
			 "[PTS]", "voxel_api", "voxel_instancing"));
#endif
#if (ALL==1)||(VOXEL_RENDER==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_render,
			 "vx_render",
			 { "ev", "objs", "ptss" },
			 { "EveryApi&", "[P]", "[PTS]" },
			 { "ev", "", "" },
			  "ML", "voxel_api", "voxel_render"));
#endif
#if (ALL==1)||(VOXEL_BIND==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_bind,
			 "vx_bind",
			 { "ev", "objs", "ptss", "mt" },
			 { "EveryApi&", "[P]", "[PTS]", "MT" },
			 { "ev", "", "", "" },
			 "ML", "voxel_api", "voxel_bind"));
#endif
#if (ALL==1)||(VOXEL_STATIC==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_static,
			 "vx_static",
			 { "ev", "objs", "ptss" },
			 { "EveryApi&", "[P]", "[PTS]" },
			 { "ev", "", "" },
			 "P", "voxel_api", "voxel_static"));
#endif
#if (ALL==1)||(CONVERT_P_TO_VX==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::convert_p_to_vx,
			 "vx_p",
			 { "p", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "value" },
			 { "P", "int", "int", "int", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "0" },
			 "VX", "voxel_api", "convert_p_to_vx"));
#endif
#ifndef STABLE
#if (ALL==1)||(FROM_IMPLICIT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::from_implicit,
			 "vx_im",
			 { "im", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "value" },
			 { "IM", "int", "int", "int", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "0" },
			 "VX", "voxel_api", "from_implicit"));
#endif
#if (ALL==1)||(PTS_TO_VOXEL==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::pts_to_voxel,
			 "av_from_pts",
			 { "pts", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy", "sz" },
			 { "PTS", "float", "float", "float", "float", "float", "float", "int", "int", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "30", "30", "30" },
			 "AV", "polygon_api", "pts_to_voxel"));
#endif
#if (ALL==1)||(VOXELARRAY_TO_PTS==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::voxelarray_to_pts,
			 "av_to_pts",
			 { "att", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "AV", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "polygon_api", "voxelarray_to_pts"));
#endif
#if (ALL==1)||(AV_UNIQUE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::av_unique,
			 "av_unique",
			 { "arr" },
			 { "AV" },
			 { "" },
			 "AV", "polygon_api", "av_unique"));
#endif
#endif
  return vec;
}
