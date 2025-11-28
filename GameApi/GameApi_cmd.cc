#include "GameApi_h.hh"
#include "GameApi_gui.hh"

#include "GameApi_cmd.hh"




#define MACRO2(lab, funccall) \
  void set_empty(GameApi::EveryApi&ev,lab &l) {	\
    l = funccall;\
  }
MACRO2(GameApi::BM,ev.bitmap_api.newbitmap(10,10,0x00000000))
MACRO2(GameApi::FD,ev.dist_api.fd_cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::BO,ev.bool_api.cube_bo(ev,0.0,0.0,0.0,0.0,0.0,0.0,1,1))
MACRO2(GameApi::BB,ev.bool_bitmap_api.bb_empty(10,10))
MACRO2(GameApi::FB,ev.float_bitmap_api.fb_empty(10,10))
MACRO2(GameApi::PT,ev.point_api.origo())
MACRO2(GameApi::V,ev.vector_api.null_vector())
MACRO2(GameApi::FO,ev.float_volume_api.distance())
MACRO2(GameApi::SFO,ev.sh_api.cube())
MACRO2(GameApi::O,ev.volume_api.o_cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::CBM,ev.cont_bitmap_api.cbm_empty(10.0,10.0))
MACRO2(GameApi::P,ev.polygon_api.p_empty())
MACRO2(GameApi::SH,ev.shader_api.colour_shader())
MACRO2(GameApi::CO,ev.color_api.u_color(0xffffffff))
MACRO2(GameApi::LI,ev.lines_api.from_polygon(ev.polygon_api.p_empty()))
MACRO2(GameApi::MN,ev.move_api.mn_empty())
MACRO2(GameApi::MT,ev.materials_api.m_def(ev))
MACRO2(GameApi::C,ev.curve_api.linear(std::vector<GameApi::PT>()))
MACRO2(GameApi::PD,ev.polygon_dist_api.empty(ev))
MACRO2(GameApi::WV,ev.waveform_api.wv_empty(1.0))
MACRO2(GameApi::ML,ev.polygon_api.render_vertex_array_ml2(ev,ev.polygon_api.p_empty()))
MACRO2(GameApi::CFB,ev.float_bitmap_api.X_bitmap(300.0,300.0))
MACRO2(GameApi::FI,ev.font_api.load_font("http://$(instdir)/Chunkfive.otf",20,20))
MACRO2(GameApi::HML,ev.mainloop_api.html_url("https://meshpage.org/assets/empty_hml.mp"))
MACRO2(GameApi::PTS,ev.points_api.pt_array(ev,std::vector<GameApi::PT>{ev.point_api.point(0.0,0.0,0.0)}))
MACRO2(GameApi::IBM,ev.bitmap_api.convert_fb_to_ibm_bitmap(ev.float_bitmap_api.fb_empty(10,10),0.0,0.3));
MACRO2(GameApi::TF,ev.mainloop_api.gltf_loadKK2("https://meshpage.org/assets/test.glb"))
#undef MACRO2


std::pair<std::string,std::string> CodeGen_1(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, std::vector<std::string> param_type, std::string return_type, std::string api_name, std::string func_name, int j)
{
      std::string s;
    int ss = params.size();
   for(int i=0;i<ss;i++)
      {
	s+= params[i];
      }

    s+= return_type;
    s+= " ";
    std::string id = unique_id_apiitem();
    s+= id;
    s+= "=";
    s+= "ev.";
    s+= api_name;
    s+= ".";
    s+= func_name;
    s+= "(";
    int sk = param_names.size();
    for(int i=0;i<sk;i++)
      {
	if (param_names[i]=="@")
	  {
	    s+=empty_param(param_type[i]);
	  }
	else
	  s+= param_names[i];
	if (i!=int(param_names.size())-1) s+=",";
      }
    s+=");\n";
    return std::make_pair(id, s);

}

void funccall_1(std::vector<std::string> &s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name)
{
  int s3 = s.size();
  if (s.size()!=param_name.size()) { std::cout << "funccall: param_names and parameter values std::vectors different size" << std::endl; }
  for(int i=0;i<s3;i++)
    {
      std::string pn = param_name[i];
      int s4 = e.names.size();
      for(int j=0;j<s4;j++)
	{
	  std::string n = e.names[j];
	  std::string v = e.values[j];
	  if (pn == n)
	    {
	      s[i] = v;
	    }
	}
      
    }
}

#define MACRO3(lab, funccall) \
  if (s==#lab) { return #funccall; }


std::string empty_param(std::string s)
{
  if (s.size()>1 && s[0]=='[')
    {
      return "std::vector<"+s+">()";
    }
  if (s=="float") { return "0.0"; }
  if (s=="int") { return "0"; }
  if (s=="std::string") { return ""; }
MACRO3(BM,ev.bitmap_api.newbitmap(10,10,0x00000000))
MACRO3(FD,ev.dist_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO3(BO,ev.bool_api.cube(ev,0.0,0.0,0.0,0.0,0.0,0.0,1,1))
MACRO3(BB,ev.bool_bitmap_api.bb_empty(10,10))
MACRO3(FB,ev.float_bitmap_api.fb_empty(10,10))
MACRO3(PT,ev.point_api.origo())
MACRO3(V,ev.vector_api.null_vector())
MACRO3(FO,ev.float_volume_api.distance())
MACRO3(SFO,ev.sh_api.cube())
MACRO3(O,ev.volume_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO3(CBM,ev.cont_bitmap_api.cbm_empty(10.0,10.0))
MACRO3(P,ev.polygon_api.p_empty())
MACRO3(SH,ev.shader_api.colour_shader())
MACRO3(CO,ev.color_api.u_color(0xffffffff))
MACRO3(LI,ev.lines_api.from_polygon(ev.polygon_api.p_empty()))
MACRO3(MN,ev.move_api.mn_empty())
MACRO3(MT,ev.materials_api.m_def(ev))
MACRO3(C,ev.curve_api.linear(std::vector<GameApi::PT>()))
MACRO3(PD,ev.polygon_dist_api.empty(ev))
MACRO3(WV,ev.waveform_api.wv_empty(1.0))
MACRO3(ML,ev.polygon_api.render_vertex_array_ml2(ev,ev.polygon_api.p_empty()));
MACRO3(CFB,ev.float_bitmap_api.X_bitmap(300.0,300.0))
MACRO3(FI,ev.font_api.load_font("file://$(instdir)/Chunkfive.otf",20,20))
MACRO3(GameApi::HML,ev.mainloop_api.html_url("https://meshpage.org/assets/empty_hml.mp"))
MACRO3(PTS,ev.points_api.pt_array(ev,std::vector<GameApi::PT>{ev.point_api.point(0.0,0.0,0.0)}))
MACRO3(IBM,ev.bitmap_api.convert_fb_to_ibm_bitmap(ev.float_bitmap_api.fb_empty(10,10),0.0,0.3));
MACRO3(TF,ev.mainloop_api.gltf_loadKK2("https://meshpage.org/assets/test.glb"))
 return "@";
}

int to_int(std::string s) {
  std::stringstream ss(s);
  int a;
  ss >> a;
  return a;
}

float to_float(std::string s)
{
  std::stringstream ss(s);
  float a;
  ss >> a;
  return a;
}

std::string unique_id_apiitem()
{
  static int id = 0;
  id++;
  std::stringstream ss;
  ss << id;
  return std::string("I") + ss.str();
}

ApiSplit split_api_default(std::string def)
{
  int s = def.size();
  int pos1=-1;
  int pos2=-1;
  int i=0;
  for(;i<s;i++)
    {
      if (def[i]=='@') { pos1=i; i++; break; }
    }
  for(;i<s;i++)
    {
      if (def[i]=='@') { pos2=i; i++; break; }
    }
  ApiSplit spl;
  spl.def = def.substr(0,pos1);
  spl.author = pos1!=-1?def.substr(pos1+1,pos2-pos1):"";
  spl.license = pos2!=-1?def.substr(pos2+1):"";
  return spl;
}

