
#include "GameApi_h.hh"


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
  set_default_projection_1(shader, name);
}
void GameApi::ShaderApi::set_default_projection_1(SH shader, std::string name)
{
  std::cout << "SetDefaultProjection:" << std::endl;
  Matrix m = Matrix::Perspective(80.0, (double)800/600, 10.1, 60000.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, m);  
}
EXPORT void GameApi::ShaderApi::set_y_rotation(SH shader, std::string name, float angle)
{
  Matrix m = Matrix::YRotation(angle); //*Matrix::Translate(0.0,0.0,-300.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, m);  
}
EXPORT void GameApi::ShaderApi::link(GameApi::SH shader)
{
  link_1(shader);
}
void GameApi::ShaderApi::link_1(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->link(shader.id);
}
EXPORT GameApi::SH GameApi::ShaderApi::texture_shader()
{
  return get_normal_shader_1("comb", "comb", "", "texture:light:light", "texture:light:light");
}

EXPORT GameApi::SH GameApi::ShaderApi::colour_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:light:light", "colour:light:light");
}
EXPORT GameApi::SH GameApi::ShaderApi::colour_texture_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:texture:light:light", "colour:texture:light:light");
}
EXPORT GameApi::SH GameApi::ShaderApi::get_normal_shader(std::string v_format,
						  std::string f_format,
						  std::string g_format,
						  std::string v_comb,
							 std::string f_comb, bool trans, SFO mod)
{
  return get_normal_shader_1(v_format, f_format, g_format, v_comb, f_comb, trans,mod);
}
GameApi::SH GameApi::ShaderApi::get_normal_shader_1(std::string v_format,
						  std::string f_format,
						  std::string g_format,
						  std::string v_comb,
						    std::string f_comb, bool trans, SFO mod)
{
  SH sh = get_shader_1(v_format, f_format, g_format, v_comb, f_comb,trans,mod);
  bind_attrib_1(sh, 0, "in_Position");
  bind_attrib_1(sh, 1, "in_Normal");
  bind_attrib_1(sh, 2, "in_Color");
  bind_attrib_1(sh, 3, "in_TexCoord");
  bind_attrib_1(sh, 4, "in_Position2");
  bind_attrib_1(sh, 5, "in_InstPos");
  link_1(sh);
  use_1(sh);
  set_default_projection_1(sh, "in_P");
  return sh;
}

void combparse(std::string comb, std::vector<std::string> &vec)
{
  std::string s;
  int ss = comb.size();
  for(int i=0;i<ss;i++)
    {
      char c = comb[i];
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

EXPORT GameApi::SH GameApi::ShaderApi::get_shader(std::string v_format,
					std::string f_format,
					   std::string g_format, 
					   std::string v_comb,
						  std::string f_comb, bool trans, SFO module)
{
  return get_shader_1(v_format, f_format, g_format, v_comb, f_comb, trans, module);
}

GameApi::SH GameApi::ShaderApi::get_shader_1(std::string v_format,
					std::string f_format,
					   std::string g_format, 
					   std::string v_comb,
					     std::string f_comb, bool trans, SFO module)
{
  ShaderModule *mod = 0;
  if (module.id!=-1)
    mod = find_shader_module(e, module);
  ShaderPriv2 *p = (ShaderPriv2*)priv;
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
  p->ids[p->count] = p->seq->GetShader(v_format, f_format, g_format, v_vec, f_vec, trans, mod);
  p->count++;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_privs[p->count-1] = p;
  GameApi::SH sh;
  sh.id = p->count-1;
  return sh;
}
EXPORT void GameApi::ShaderApi::use(GameApi::SH shader)
{
  use_1(shader);
}
void GameApi::ShaderApi::use_1(GameApi::SH shader)
{
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->use(p->ids[shader.id]);
}

EXPORT void GameApi::ShaderApi::unuse(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->unuse(p->ids[shader.id]);
}
EXPORT void GameApi::ShaderApi::bind_attrib(GameApi::SH shader, int num, std::string name)
{
  return bind_attrib_1(shader,num,name);
}
EXPORT void GameApi::ShaderApi::bind_frag(GameApi::SH shader, int attachment_num, std::string name)
{
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_frag(attachment_num, name);
}
void GameApi::ShaderApi::bind_attrib_1(GameApi::SH shader, int num, std::string name)
{
  std::cout << "SHADER: " << shader.id << std::endl;
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_attrib(num, name);
}
EXPORT GameApi::M GameApi::ShaderApi::get_matrix_var(GameApi::SH shader, std::string name)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Matrix m = prog->get_matrix_var(name);
  return add_matrix(e, new SimpleMatrix(m));

}
EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float val)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float x, float y, float z)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Point px(x, y, z);
  prog->set_var(name, px);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float x, float y, float z, float k)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, x,y,z,k);
}


EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, int val)
{
  //std::cout << "Set var int" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, M matrix)
{
  Matrix mat = find_matrix(e, matrix);

  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, mat);
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