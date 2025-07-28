
#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> vectorapi_functions()
{
  std::vector<GameApiItem*> vec;
#if (ALL==1)||(VECTOR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_api, &GameApi::VectorApi::vector,
			 "vector",
			 { "dx", "dy", "dz" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "V", "vector_api", "vector"));
#endif
#if 0
#ifndef STABLE

  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::player,
			 "player",
			 { "prev" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "player"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enemy,
			 "enemy",
			 { "prev" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "enemy"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::player_pos,
			 "player_pos",
			 { "prev", "pos" },
			 { "ML", "PT" },
			 { "", "" },
			 "ML", "move_api", "player_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enemy_pos,
			 "enemy_pos",
			 { "prev", "pos" },
			 { "ML", "PTS" },
			 { "", "" },
			 "ML", "move_api", "enemy_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::to_polygon,
			 "plf_poly",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "P", "newplane_api", "to_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::reverse_faces,
			 "plf_reversefaces",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "PLF", "newplane_api", "reverse_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::triangulate,
			 "triangulate2d",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "PLF", "newplane_api", "triangulate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::phy_empty,
			 "phy_empty",
			 { },
			 { },
			 { },
			 "PH", "physics_api", "phy_empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::anchor_point2,
			 "phy_anchor",
			 { "phy", "pos" },
			 { "PH", "PT" },
			 { "", "" },
			 "PH", "physics_api", "anchor_point2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::ext_force,
			 "phy_force",
			 { "phy", "point", "dir" },
			 { "PH", "int", "V" },
			 { "", "0", "" },
			 "PH", "physics_api", "ext_force"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::ext_force_all,
			 "phy_force_all",
			 { "phy", "dir" },
			 { "PH", "V" },
			 { "", "" },
			 "PH", "physics_api", "ext_force_all"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::anchor_link,
			 "phy_link",
			 { "phy", "p1", "p2", "dist" },
			 { "PH", "int", "int", "float" },
			 { "", "0", "1", "100" },
			 "PH", "physics_api", "anchor_link"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::force_obj,
			 "phy_object",
			 { "phy", "obj", "dir" },
			 { "PH", "O", "V" },
			 { "", "", "" },
			 "PH", "physics_api", "force_obj"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::phy_from_p,
			 "phy_p",
			 { "p" },
			 { "P" },
			 { "" },
			 "PH", "physics_api", "phy_from_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::physics_action,
			 "phy_points",
			 { "ev", "phy" },
			 { "EveryApi&", "PH" },
			 { "ev", "" },
			 "PTS", "physics_api", "physics_action"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::identity,
			 "dc_identity",
			 { },
			 { },
			 { },
			 "DC", "move_api", "identity"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::wave,
			 "dc_wave",
			 { "r", "speed1", "speed2", "dist1", "dist2", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "int", "int" },
			 { "30.0", "1.0", "1.0", "30.0", "30.0", "30", "30" },
			 "DC", "move_api", "wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::sphwave,
			 "dc_sphwave",
			 { "r1", "fr_1", "t_1", "r2", "fr_2", "t_2" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "30.0", "3.0", "1.0", "30.0", "5.0", "2.0" },
			 "DC", "move_api", "sphwave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::explosion,
			 "dc_explosion",
			 { "center_x", "center_y", "center_z", "start_val", "end_val", "start_time", "end_time" },
			 { "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "1.0", "10.0", "0.0", "30.0" },
			 "DC", "move_api", "explosion"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::split,
			 "dc_split",
			 { "d1", "d2", "val" },
			 { "DC", "DC", "float" },
			 { "", "", "0.5" },
			 "DC", "move_api", "split"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::game_state,
			 "gs_gamestate",
			 { },
			 { },
			 { },
			 "GS", "mainloop_api", "game_state"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gs_mouse_pos,
			 "gs_mouse",
			 { "gs", "ml", "x", "y" },
			 { "GS", "ML", "int", "int" },
			 { "", "", "0", "1" },
			 "ML", "mainloop_api", "gs_mouse_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gs_obj_pos,
			 "gs_obj",
			 { "gs", "ml", "x", "y", "z" },
			 { "GS", "ML", "int", "int", "int" },
			 { "", "", "0", "1", "2" },
			 "ML", "mainloop_api", "gs_obj_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gs_delta,
			 "gs_delta",
			 { "gs", "ml", "p", "n", "delta" },
			 { "GS", "ML", "int", "int", "float" },
			 { "", "", "0", "1", "15.0" },
			 "ML", "mainloop_api", "gs_delta"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gs_time,
			 "gs_time",
			 { "gs", "ml", "t" },
			 { "GS", "ML", "int" },
			 { "", "", "0" },
			 "ML", "mainloop_api", "gs_time"));
  
#endif
#endif  
  return vec;
}
