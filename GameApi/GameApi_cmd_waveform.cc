#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"

std::vector<GameApiItem*> waveform_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::wv_empty,
			 "wv_empty",
			 { "length" },
			 { "float" },
			 { "1.0" },
			 "WV", "waveform_api", "wv_empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::step,
			 "wv_step",
			 { "flip" },
			 { "bool" },
			 { "false" },
			 "WV", "waveform_api", "step"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::sinwave,
			 "wv_sin",
			 { "length", "freq" },
			 { "float", "float" },
			 { "6.28318", "1.0" },
			 "WV", "waveform_api", "sinwave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::gaussian,
			 "wv_gaussian",
			 { "start_x", "end_x", "start_y", "end_y" },
			 { "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-1.0", "1.0" },
			 "WV", "waveform_api", "gaussian"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::sum,
			 "wv_sum",
			 { "w1", "w2" },
			 { "WV", "WV" },
			 { "", "" },
			 "WV", "waveform_api", "sum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::move,
			 "wv_move",
			 { "w1", "delta" },
			 { "WV", "float" },
			 { "", "0.0" },
			 "WV", "waveform_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::wave_move_y,
			 "wv_move_y",
			 { "wave", "delta" },
			 { "WV", "float" },
			 { "", "0.0" },
			 "WV", "float_volume_api", "wave_move_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::scale,
			 "wv_scale",
			 { "w1", "scale" },
			 { "WV", "float" },
			 { "", "1.0" },
			 "WV", "waveform_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::waveform_rotated_polygon,
			 "wv_rot_poly",
			 { "wave", "start_angle", "end_angle", "radius", "num_samples", "num_waves" },
			 { "WV", "float", "float", "float", "int", "int" },
			 { "", "0.0", "6.28318", "300.0", "40", "40" },
			 "P", "waveform_api", "waveform_rotated_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::cubic,
			 "wv_cubic",
			 { "f_0", "f_1", "df_0", "df_1", "min_y", "max_y" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.5", "-0.5", "-5.0", "5.0" },
			 "WV", "waveform_api", "cubic"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::repeat,
			 "wv_repeat",
			 { "wave", "num" },
			 { "WV", "int" },
			 { "", "2" },
			 "WV", "waveform_api", "repeat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::length_change,
			 "wv_set_length",
			 { "wave", "new_length" },
			 { "WV", "float" },
			 { "", "1.0" },
			 "WV", "waveform_api", "length_change"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::waveform_bitmap,
			 "wv_bitmap",
			 { "wave", "sx", "sy", "true_color", "false_color" },
			 { "WV", "int", "int", "unsigned int", "unsigned int" },
			 { "", "100", "100", "ffffffff", "00000000" },
			 "BM", "waveform_api", "waveform_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::std_polynomial,
			 "pn_std",
			 { "x_5", "x_4", "x_3", "x_2", "x_1", "c" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "0.0", "0.0", "0.0" },
			 "PN", "waveform_api", "std_polynomial"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::df_dx,
			 "pn_df_dx",
			 { "poly" },
			 { "PN" },
			 { "" },
			 "PN", "waveform_api", "df_dx"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::polynomial_wave,
			 "wv_polynomial",
			 { "pn", "start_x", "end_x", "start_y", "end_y" },
			 { "PN", "float", "float", "float", "float" },
			 { "", "-5.0", "5.0", "5.0", "-5.0" },
			 "WV", "waveform_api", "polynomial_wave"));
  return vec;
}
