
#include "GameApi_h.hh"
#include "GameApi.hh"

EXPORT GameApi::ShaderApi::ShaderApi(Env &e) : e(e)
{
  priv = (void*)new ShaderPriv2;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = 0;
  p->seq = 0;
  p->count=0;
}
EXPORT GameApi::ShaderApi::~ShaderApi()
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  delete p->file;
  delete p->seq;
  delete (ShaderPriv2*)priv;
}

EXPORT void GameApi::ShaderApi::load(std::string filename)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = new ShaderFile(filename);
  ShaderSeq *seq = new ShaderSeq(*p->file);
  p->seq = seq;
  p->count = 0;
}
EXPORT void GameApi::ShaderApi::load_default()
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = new ShaderFile;
  ShaderSeq *seq = new ShaderSeq(*p->file);
  p->seq = seq;
  p->count = 0;
}

EXPORT void GameApi::ShaderApi::set_default_projection(SH shader, std::string name)
{
  if (shader.id<0) return;
  set_default_projection_1(shader, name);
}
void GameApi::ShaderApi::set_default_projection_1(SH shader, std::string name)
{
  if (shader.id<0) return;
  //std::cout << "SetDefaultProjection:" << std::endl;
  Matrix m = Matrix::Perspective(80.0, (double)800/600, 10.1, 1600.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name.c_str(), m);  
}
EXPORT void GameApi::ShaderApi::set_y_rotation(SH shader, std::string name, float angle)
{
  if (shader.id<0) return;
  Matrix m = Matrix::YRotation(angle); //*Matrix::Translate(0.0,0.0,-300.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name.c_str(), m);  
}
EXPORT GameApi::PinIn GameApi::ShaderApi::link(GameApi::SH shader)
{
  if (shader.id<0) return PinIn();
  link_1(shader);
  return PinIn();
}
EXPORT void GameApi::ShaderApi::print_log(GameApi::SH shader)
{
  if (shader.id<0) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  if (seq)
    seq->print_log(shader.id);

}
void GameApi::ShaderApi::link_1(GameApi::SH shader)
{
  if (shader.id<0) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  if (seq)
    seq->link(shader.id);
}
extern std::string g_gpu_vendor;
EXPORT GameApi::SH GameApi::ShaderApi::texture_shader()
{
  if (g_gpu_vendor.substr(0,4)=="NVID") {
  return get_normal_shader_1("comb", "comb", "", "texture_impl", "texture_impl:light:light", true, {-1}, {-1}, {-1}, "EX_TEXCOORD IN_TEXCOORD", "EX_TEXCOORD COLOR_MIX TEXTURE_IMPL LIGHT","","");
  } else {
  return get_normal_shader_1("comb", "comb", "", "texture_impl", "texture_impl:light:light", true, {-1}, {-1}, {-1}, "EX_TEXCOORD IN_TEXCOORD", "EX_TEXCOORD COLOR_MIX TEXTURE_IMPL LIGHT","","");
  }
}
EXPORT GameApi::SH GameApi::ShaderApi::texture_array_shader()
{
  return get_normal_shader_1("comb", "comb", "", "texture_arr:light:light", "texture_arr:light:light", true, {-1}, {-1}, {-1}, "EX_TEXCOORD IN_TEXCOORD", "EX_TEXCOORD COLOR_MIX LIGHT","","");
}
EXPORT GameApi::SH GameApi::ShaderApi::texture_many_shader()
{
  return get_normal_shader_1("comb", "comb", "", "manytextures:light:light", "manytextures:light:light", true, {-1}, {-1}, {-1}, "EX_TEXCOORD IN_TEXCOORD", "MANYTEXTURES EX_TEXCOORD COLOR_MIX LIGHT","","");
}

EXPORT GameApi::SH GameApi::ShaderApi::colour_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:light:light", "colour:light:light", true, {-1}, {-1}, {-1}, "EX_COLOR IN_COLOR", "EX_COLOR COLOUR_SHADER LIGHT","","");
}
EXPORT GameApi::SH GameApi::ShaderApi::shader_choice(EveryApi &ev, int i)
{
  SH sh = colour_shader();
  switch(i) {
  case 0: sh = colour_shader(); break;
  case 1: sh = texture_shader();break;
  case 2: sh = texture_shader(); break;
  case 3: sh = colour_texture_shader();break;
  };
  int width = ev.mainloop_api.get_screen_width();
  int height = ev.mainloop_api.get_screen_height();
  ev.mainloop_api.init_3d(sh, width, height);
  ev.mainloop_api.alpha(true);

  return sh;
}
EXPORT GameApi::SH GameApi::ShaderApi::colour_texture_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:texture:light:light", "colour:texture:light:light", true, {-1}, {-1}, {-1}, "EX_TEXCOORD IN_TEXCOORD", "EX_TEXCOORD COLOR_MIX TEXTURE_IMPL LIGHT COLOUR_SHADER","","");
}
EXPORT GameApi::SH GameApi::ShaderApi::get_normal_shader(std::string v_format, std::string f_format,std::string g_format,std::string v_comb, std::string f_comb, bool trans, SFO mod, std::string v_defines, std::string f_defines, bool is_new)
{
  GameApi::US us1 = { -1 };
  if (!is_new) {
    return get_normal_shader_1(v_format, f_format, g_format, v_comb, f_comb, trans,mod, us1,us1, v_defines, f_defines,"","");
  } else {
    return get_normal_shader_1_new(v_format, f_format, g_format, v_comb, f_comb, trans,mod, us1,us1, v_defines, f_defines,"","");
  }
}

EXPORT GameApi::SH GameApi::ShaderApi::get_normal_shader(std::string v_format, std::string f_format, std::string g_format, US v_comb, US f_comb, bool trans, SFO mod, std::string v_defines, std::string f_defines, bool is_new)
{
  if (!is_new) {
    return get_normal_shader_1(v_format, f_format, g_format, "", "", trans,mod, v_comb, f_comb,v_defines,f_defines,"","");
  } else {
    return get_normal_shader_1_new(v_format, f_format, g_format, "", "", trans,mod, v_comb, f_comb,v_defines,f_defines,"","");
  }
}
EXPORT GameApi::SH GameApi::ShaderApi::get_normal_shader(std::string v_format, std::string f_format, std::string g_format, US v_comb, US f_comb, std::string v_shader, std::string f_shader, bool trans, SFO mod, std::string v_defines, std::string f_defines, bool is_new)
{
  if (!is_new) {
  return get_normal_shader_1(v_format, f_format, g_format, "", "", trans,mod, v_comb, f_comb,v_defines,f_defines, v_shader, f_shader);
  } else {
  return get_normal_shader_1_new(v_format, f_format, g_format, "", "", trans,mod, v_comb, f_comb,v_defines,f_defines, v_shader, f_shader);
  }
}


GameApi::SH GameApi::ShaderApi::get_normal_shader_1(std::string v_format, std::string f_format, std::string g_format,std::string v_comb, std::string f_comb, bool trans, SFO mod, US v_c, US f_c, std::string v_defines, std::string f_defines, std::string v_shader, std::string f_shader)
{
  SH sh = get_shader_1(v_format, f_format, g_format, v_comb, f_comb,trans,mod,v_c, f_c, v_defines, f_defines, v_shader, f_shader);
  //const int vertex_id = 7;
  bind_attrib_1(sh, 0, "in_Position");
  bind_attrib_1(sh, 1, "in_Normal");
  bind_attrib_1(sh, 2, "in_Color");
  bind_attrib_1(sh, 3, "in_TexCoord");
  bind_attrib_1(sh, 4, "in_Position2");
  bind_attrib_1(sh, 5, "in_InstPos");
  //bind_attrib_1(sh, 6, "bone_id");
  bind_attrib_1(sh, 7, "in_InstMat"); // also uses 7,8,9,10
  bind_attrib_1(sh, 11, "JOINTS_0");
  bind_attrib_1(sh, 12, "WEIGHTS_0");
  bind_attrib_1(sh, 13, "in_TexCoord2");
  bind_attrib_1(sh, 14, "in_Color2");
  bind_attrib_1(sh, 15, "JOINTS_1");
  //bind_attrib_1(sh, 6, "WEIGHTS_1");
  //bind_attrib_1(sh, 14, "INDICES");
  //bind_attrib_1(sh, 6, "in_LightAmount");
  link_1(sh);
  use_1(sh);
  set_default_projection_1(sh, "in_P");
  return sh;
}
GameApi::SH GameApi::ShaderApi::get_normal_shader_1_new(std::string v_format, std::string f_format, std::string g_format,std::string v_comb, std::string f_comb, bool trans, SFO mod, US v_c, US f_c, std::string v_defines, std::string f_defines, std::string v_shader, std::string f_shader)
{
  SH sh = get_shader_1(v_format, f_format, g_format, v_comb, f_comb,trans,mod,v_c, f_c, v_defines, f_defines, v_shader, f_shader);
  //const int vertex_id = 7;
  bind_attrib_1(sh, 0, "in_Position");
  bind_attrib_1(sh, 1, "in_Normal");
  bind_attrib_1(sh, 2, "in_Color");
  bind_attrib_1(sh, 3, "in_TexCoord");
  bind_attrib_1(sh, 4, "in_Position2");
  bind_attrib_1(sh, 5, "in_InstPos");
  //bind_attrib_1(sh, 6, "bone_id");
  bind_attrib_1(sh, 7, "in_InstMat"); // also uses 7,8,9,10
  bind_attrib_1(sh, 11, "JOINTS_0");
  bind_attrib_1(sh, 12, "WEIGHTS_0");
  //bind_attrib_1(sh, 13, "in_TexCoord2");
  //bind_attrib_1(sh, 14, "in_Color2");
  bind_attrib_1(sh, 15, "JOINTS_1");
  bind_attrib_1(sh, 13, "WEIGHTS_1");
  bind_attrib_1(sh, 14, "INDICES");
  bind_attrib_1(sh, 6, "in_LightAmount");
  link_1(sh);
  use_1(sh);
  set_default_projection_1(sh, "in_P");
  return sh;
}

void combparse(std::string comb, std::vector<std::string> &vec)
{
  //std::cout << "COMBPARSE:'" << comb << "'" << std::endl;
  std::string s;
  int ss = comb.size();
  //std::cout << "COMBSIZE:" << ss << std::endl;
  for(int i=0;i<ss;i++)
    {
      char c = comb[i];
      if (c==0) break;
      if (c==':')
	{
	  vec.push_back(s);
	  s="";
	}
      else
	{
	  s+=c;
	}
    }
  if (s!="")
    {
      vec.push_back(s);
    }
}

EXPORT GameApi::SH GameApi::ShaderApi::get_shader(std::string v_format, std::string f_format, std::string g_format, std::string v_comb,std::string f_comb, bool trans, SFO module)
{
  return get_shader_1(v_format, f_format, g_format, v_comb, f_comb, trans, module);
}



GameApi::SH GameApi::ShaderApi::get_shader_1(std::string v_format, std::string f_format,std::string g_format, std::string v_comb, std::string f_comb, bool trans, SFO module, US v_c, US f_c, std::string v_defines, std::string f_defines, std::string v_shader, std::string f_shader)
{

  
  ShaderModule *mod = 0;
  if (module.id!=-1)
    mod = find_shader_module(e, module);
  ShaderCall *vertex_c = 0;
  if (v_c.id!=-1)
    vertex_c = find_uber(e, v_c);
  ShaderCall *fragment_c = 0;
  if (f_c.id!=-1)
    fragment_c = find_uber(e, f_c);

  ShaderPriv2 *p = (ShaderPriv2*)priv;
  //std::cout << "p->count=" << p->count << std::endl;
  if (!p->file)
    {
      std::cout << "ERROR: Call Shader::load before get_shader()" << std::endl;
      SH sh;
      sh.id = -1;
      return sh;
    }
  std::vector<std::string> v_vec;
  std::vector<std::string> f_vec;
  combparse(v_comb, v_vec);
  combparse(f_comb, f_vec);
  if (p->seq)
    p->ids[p->count] = p->seq->GetShader(v_format, f_format, g_format, v_vec, f_vec, trans, mod,vertex_c,fragment_c, v_defines, f_defines, v_shader, f_shader);
  else std::cout << "get_shader_1 skipped GetShader because p->seq==0" << std::endl;
  p->count++;
  //std::cout << "p->count=" << p->count << std::endl;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_privs[p->count-1] = p;
  GameApi::SH sh;
  sh.id = p->count-1;
  //std::cout << "returning:" << sh.id << std::endl;
  
  return sh;
}
EXPORT GameApi::PinIn GameApi::ShaderApi::use(GameApi::SH shader)
{
  use_1(shader);
  return PinIn();
}
void GameApi::ShaderApi::use_1(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  if (shader.id<0||p->ids.find(shader.id)==p->ids.end())  { if (shader.id>=0) std::cout << "use_1::Shader id=" << shader.id << "rejected" << std::endl; return; }
  ShaderSeq *seq = p->seq;
  seq->use(p->ids[shader.id]);
}

EXPORT GameApi::PinIn GameApi::ShaderApi::unuse(GameApi::SH shader)
{
  if (shader.id<0) { /*std::cout << "unuse shader.id=" << shader.id << " rejected" << std::endl;*/ return PinIn(); }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->unuse(p->ids[shader.id]);
  return PinIn();
}
EXPORT void GameApi::ShaderApi::bind_attrib(GameApi::SH shader, int num, std::string name)
{
  if (shader.id<0) { std::cout << "bind_attrib shader.id=" << shader.id << " rejected" << std::endl; return; }
  return bind_attrib_1(shader,num,name);
}
EXPORT void GameApi::ShaderApi::bind_frag(GameApi::SH shader, int attachment_num, std::string name)
{
  if (shader.id<0) { std::cout << "bind_frag shader.id=" << shader.id << " rejected" << std::endl; return; }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_frag(attachment_num, name);
}
void GameApi::ShaderApi::bind_attrib_1(GameApi::SH shader, int num, std::string name)
{
  if (shader.id<0) { std::cout << "set_matrix_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return; }
  //std::cout << "SHADER: " << shader.id << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_attrib(num, name);
}
EXPORT GameApi::PinOut<GameApi::M> GameApi::ShaderApi::get_matrix_var(GameApi::SH shader, std::string name)
{
  if (shader.id<0) { std::cout << "set_matrix_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; GameApi::M m2= add_matrix(e, new SimpleMatrix(Matrix::Identity())); PinOut<GameApi::M> out; out.data=m2; return out; }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Matrix m = prog->get_matrix_var(name);
  GameApi::M m2 = add_matrix(e, new SimpleMatrix(m));
  PinOut<GameApi::M> ret;
  ret.data=m2;
  return ret;
}
EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, float val)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
  return PinIn();
}

EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, float x, float y, float z)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Point px(x, y, z);
  prog->set_var(name, px);
  return PinIn();
}

EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, float x, float y, float z, float k)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, x,y,z,k);
  return PinIn();
}


EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, int val)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  //std::cout << "Set var int" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
  return PinIn();
}

EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, M matrix)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  Matrix mat = find_matrix(e, matrix);

  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, mat);
  return PinIn();
}
EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, const std::vector<PT> &m)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }
  std::vector<Point> v;
  int s=m.size();
  for(int i=0;i<s;i++)
    {
      v.push_back(*find_point(e,m[i]));
    }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, v);
  return PinIn();
}



std::map<std::string,std::vector<float> *> cache_matrix;

EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char *name, const std::vector<M> &m, int start, int end)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }


  //std::cout << name << " " << start << " " << end << std::endl;
  
  std::string name2 = name;
  name2+=char(shader.id);
  name2+=char(start);
  name2+=char(end);
  
  std::vector<float> *v = new std::vector<float>;
  int s = std::min(m.size(),size_t(end-start));
  std::vector<float> *v2 = cache_matrix[name2];
  int pos =0;
  bool same = true;
  for(int i=std::max(0,start);i<=std::min(start+s,end);i++)
    {
      Matrix mm = find_matrix(e,m[i]);
      Matrix mm2 = Matrix::Transpose(mm);
      //std::cout << mm << std::endl;
      for(int ii=0;ii<16;ii++) {
	v->push_back(mm2.matrix[ii]);
	if (v2) {
	  //std::cout << mm2.matrix[ii] << " == " << v2->operator[](pos);
	  
	  if (fabs(mm2.matrix[ii]-v2->operator[](pos))>0.0000001) same=false;
	  //std::cout << "SAME:" << same << std::endl;
	} else same=false;
	pos++;
       }
    }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  //if (!same)
    prog->set_var_matrix2(name, *v,start);
  delete cache_matrix[name2];
  cache_matrix[name2]=v;
  return PinIn();

}

EXPORT GameApi::PinIn GameApi::ShaderApi::set_var(GameApi::SH shader, const char * name, const std::vector<M> &m, int num)
{
  if (shader.id<0) { std::cout << "set_var shader.id=" << shader.id << " " << name << " rejected" << std::endl; return PinIn(); }

  std::string name2 = name;
  name2+=char(shader.id);
  
  std::vector<float> *v = new std::vector<float>;
  int s = std::min(m.size(),size_t(num));
  std::vector<float> *v2 = cache_matrix[name2];
  int pos =0;
  bool same = true;
  for(int i=0;i<s;i++)
    {
      Matrix mm = find_matrix(e,m[i]);
      Matrix mm2 = Matrix::Transpose(mm);
      //std::cout << mm << std::endl;
      for(int ii=0;ii<16;ii++) {
	v->push_back(mm2.matrix[ii]);
	if (v2) {
	  //std::cout << mm2.matrix[ii] << " == " << v2->operator[](pos);
	  if (fabs(mm2.matrix[ii]-v2->operator[](pos))>0.0000001f) same=false;
	  //std::cout << "SAME:" << same << std::endl;
	} else same=false;
	pos++;
       }
    }
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  //if (!same)
    prog->set_var_matrix(name, *v);
  delete cache_matrix[name2];
  cache_matrix[name2]=v;
  return PinIn();
}

EXPORT void GameApi::ShaderApi::bindnames(GameApi::SH shader, 
				   std::string s_vertex,
				   std::string s_normal,
				   std::string s_color,
				   std::string s_texcoord)
{
  //ShaderPriv2 *p = (ShaderPriv2*)priv;
  //ShaderSeq *seq = p->seq;
  //Program *prog = seq->prog(p->ids[shader.id]);
  //prog->attr_loc(s_vertex, 10);
  //prog->attr_loc(s_normal, 11);
  //prog->attr_loc(s_color, 12);
  //prog->attr_loc(s_texcoord, 13);
}
