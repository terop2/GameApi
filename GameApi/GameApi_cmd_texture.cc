#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> all_functions(GameApi::EveryApi &ev);
std::vector<GameApiItem*> polydistfield_functions();
std::vector<GameApiItem*> waveform_functions();
std::vector<GameApiItem*> blocker_functions(GameApi::EveryApi &ev);
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> volumeapi_functions();
std::vector<GameApiItem*> floatvolumeapi_functions();
std::vector<GameApiItem*> colorvolumeapi_functions();
std::vector<GameApiItem*> vectorapi_functions();
std::vector<GameApiItem*> pointapi_functions();
std::vector<GameApiItem*> fontapi_functions();
std::vector<GameApiItem*> moveapi_functions();
std::vector<GameApiItem*> polygonapi_functions();
std::vector<GameApiItem*> shadermoduleapi_functions();
std::vector<GameApiItem*> framebuffermoduleapi_functions();
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> booleanopsapi_functions();
std::vector<GameApiItem*> polygonapi_functions1();
std::vector<GameApiItem*> polygonapi_functions2();
std::vector<GameApiItem*> linesapi_functions();
std::vector<GameApiItem*> pointsapi_functions();
std::vector<GameApiItem*> floatbitmapapi_functions();
std::vector<GameApiItem*> boolbitmapapi_functions();
std::vector<GameApiItem*> bitmapapi_functions();

std::vector<GameApiItem*> textureapi_functions()
{
  std::vector<GameApiItem*> vec;

#ifndef STABLE
#if (ALL==1)||(TEX_PLANE==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_plane,
			 "tex_plane",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "256", "256" },
			 "TX", "texture_api", "tex_plane"));
#endif
#if (ALL==1)||(TEX_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_bitmap,
			 "tex_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "TX", "texture_api", "tex_bitmap"));
#endif
#if (ALL==1)||(TEX_ASSIGN==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_assign,
			 "tex_assign",
			 { "tx", "id", "x", "y", "bm" },
			 { "TX", "int", "int", "int", "BM" },
			 { "", "0", "0", "0", "" },
			 "TX", "texture_api", "tex_assign"));
#endif
#if (ALL==1)||(TEX_COORD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_coord,
			 "tex_coord",
			 { "tx", "id", "x", "y", "width", "height" },
			 { "TX", "int", "int", "int", "int", "int" },
			 { "", "0", "0", "0", "100", "100" },
			 "TX", "texture_api", "tex_coord"));
#endif
#if (ALL==1)||(GET_TEX_COORD==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::get_tex_coord,
			 "get_tex_coord",
			 { "tx", "id" },
			 { "TX", "int" },
			 { "", "0" },
			 "Q", "texture_api", "get_tex_coord"));
#endif
#if 1
#if (ALL==1)||(PREPARE==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare,
			 "tx_prepare",
			 { "tx" },
			 { "TX" },
			 { "" },
			 "TXID", "texture_api", "prepare"));
#endif
#if (ALL==1)||(BIND==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::bind,
			 "tx_bind",
			 { "va", "txid" },
			 { "VA", "TXID" },
			 { "", "" },
			 "VA", "texture_api", "bind"));
#endif
#if (ALL==1)||(PREPARE_ARR==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare_arr,
			 "tx_prepare_arr",
			 { "ev", "vec", "sx", "sy" },
			 { "EveryApi&", "[BM]", "int", "int" },
			 { "ev", "", "255", "255"  },
			 "TXA", "texture_api", "prepare_arr"));
#endif
#if (ALL==1)||(BIND_ARR==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::bind_arr,
			 "tx_bind_arr",
			 { "va", "txa" },
			 { "VA", "TXA" },
			 { "", "" },
			 "VA", "texture_api", "bind_arr"));
#endif
#endif
#if (ALL==1)||(TO_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::to_bitmap,
			 "tx_to_bitmap",
			 { "ev", "txid" },
			 { "EveryApi&", "TXID" },
			 { "ev","" },
			 "BM", "texture_api", "to_bitmap"));
#endif
#endif
#if (ALL==1)||(FBO_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::fbo_api, &GameApi::FrameBufferApi::fbo_ml,
			 "fbo_id",
			 { "ev", "mainloop", "sx", "sy", "translate" },
			 { "EveryApi&", "ML", "int", "int", "bool" },
			 { "ev", "", "-1", "-1", "false" },
			 "TXID", "fbo_api", "fbo_ml"));
#endif
#ifndef STABLE
#if (ALL==1)||(DEPTH_ML==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::fbo_api, &GameApi::FrameBufferApi::depth_ml,
			 "depth_id",
			 { "ev", "mainloop", "sx", "sy", "translate" },
			 { "EveryApi&", "ML", "int", "int", "bool" },
			 { "ev", "", "-1", "-1", "false" },
			 "TXID", "fbo_api", "depth_ml"));
#endif
#if (ALL==1)||(POSITION_BASED_ON_SCREEN==1) 
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::position_based_on_screen,
			 "fbo_pos",
			 { "obj" },
			 { "ML" },
			 { "" },
			 "ML", "polygon_api", "position_based_on_screen"));
#endif
#endif
#if (ALL==1)||(FBO_ML_BLIT==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::fbo_api, &GameApi::FrameBufferApi::fbo_ml_blit,
			 "fbo_ml",
			 { "ev", "txid", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "TXID", "float", "float", "float", "float", "float" },
			 { "ev", "", "0.0", "800", "0.0", "600", "0.0" },
			 "ML", "fbo_api", "fbo_ml_blit"));
#endif
#if (ALL==1)||(CREATE_PBO==1)  
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::create_pbo,
			 "pbo_create",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "PBO", "texture_api", "create_pbo"));
#endif
#if (ALL==1)||(UPLOAD_BM_TO_PBO==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::upload_bm_to_pbo,
			 "pbo_bm",
			 { "bm", "pbo" },
			 { "BM", "PBO" },
			 { "", "" },
			 "ML", "texture_api", "upload_bm_to_pbo"));
#endif
#if (ALL==1)||(PBO_TO_BITMAP==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::pbo_to_bitmap,
			 "pbo_to_bitmap",
			 { "pbo" },
			 { "PBO" },
			 { "" },
			 "BM", "texture_api", "pbo_to_bitmap"));
#endif
#if (ALL==1)||(PBO_TO_TXID==1)
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::pbo_to_txid,
			 "pbo_to_txid",
			 { "pbo" },
			 { "PBO" },
			 { "" },
			 "TXID", "texture_api", "pbo_to_txid"));
#endif
  return vec;
}
