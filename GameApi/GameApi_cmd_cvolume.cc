
#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"
std::vector<GameApiItem*> colorvolumeapi_functions()
{
  std::vector<GameApiItem*> vec;
#ifndef STABLE
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_float_volume,
			 "cov_from_fo",
			 { "fo", "color0", "color1" },
			 { "FO", "unsigned int", "unsigned int" },
			 { "", "ff888888", "ffffffff" },
			 "COV", "color_volume_api", "from_float_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_volume,
			 "cov_from_o",
			 { "obj", "col_true", "col_false" },
			 { "O", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "COV", "color_volume_api", "from_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_continuous_bitmap,
			 "cov_from_cbm",
			 { "bm" },
			 { "CBM" },
			 { "" },
			 "COV", "color_volume_api", "from_continuous_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::mix,
			 "cov_mix",
			 { "p1", "p2", "value" },
			 { "COV", "COV", "float" },
			 { "", "", "0.5" },
			 "COV", "color_volume_api", "mix"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::directcolor,
			 "cov_color_from_normal",
			 { "normal" },
			 { "VO" },
			 { "" },
			 "COV", "color_volume_api", "directcolor"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::or_cov,
			 "cov_or",
			 { "p1", "p2" },
			 { "COV", "COV" },
			 { "", "" },
			 "COV", "color_volume_api", "or_cov"));
#endif
#endif
  return vec;
}
