// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//


//#define GL_GLEXT_PROTOTYPES
#define NO_SDL_GLEXT
#include "Shader.hh"
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "VectorTools.hh"
#include <vector>
#include <algorithm>
#include "Effect.hh"
#include <fstream>



struct ShaderPriv
{
  GLuint handle;
  std::vector<Program*> programs;
  ShaderSpec *shader;
};


Shader::Shader(ShaderSpec &shader, bool vertex, bool geom)
{
  int count = shader.Count();
  bool b = vertex;
  GLuint handle = glCreateShader(geom?GL_GEOMETRY_SHADER:(b?GL_VERTEX_SHADER:GL_FRAGMENT_SHADER));
  std::vector<std::string> vec;
  const char **strings = new const char *[count];
  int *lengths = new int[count];
  for(int i=0;i<count;i++)
    {
     vec.push_back(shader.Source(i));
     strings[i] = vec[i].c_str();
     lengths[i] = vec[i].size();
    }
  glShaderSource(handle, count, strings, lengths);
  glCompileShader(handle);
  int i=0;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &i );
  if (i == 1) { std::cout << shader.Name() << " OK" << std::endl; 
    int len=0;
  char log[255];
  glGetShaderInfoLog(handle, 255, &len, log);
  log[len]=0;
  std::cout << log << std::endl;

  }
  else
    {
      int len=0;
      char buf[255];
      glGetShaderInfoLog(handle, 255, &len, buf);
      std::cout << shader.Name() << " ERROR: " << buf << std::endl;
    }
  delete [] strings;
  priv = new ShaderPriv;
  priv->handle = handle;
  priv->shader = &shader;
}
Shader::~Shader()
{
  std::vector<Program*> vec(priv->programs.begin(), priv->programs.end());
  for(std::vector<Program*>::iterator i = vec.begin();i!=vec.end();i++)
    {
      (*i)->detach(*this);
    }
  glDeleteShader(priv->handle);
  delete priv;
}


struct ProgramPriv
{
  GLuint program;
  std::vector<const Shader*> shaders;
};

Program::Program()
{
  //std::cout << "VENDOR=" << glGetString(GL_VENDOR) << std::endl;
  //std::cout << "RENDERER=" << glGetString(GL_RENDERER) << std::endl;
  //std::cout << "VERSION=" << glGetString(GL_VERSION) << std::endl;
  priv = new ProgramPriv;
  priv->program = glCreateProgram(); //ObjectARB();  
}
Program::~Program()
{
  std::vector<const Shader*> vec(priv->shaders.begin(), priv->shaders.end());
  for(std::vector<const Shader*>::iterator i = vec.begin();i!=vec.end();i++)
    {
      detach(*(*i));
    }
  glDeleteProgram(priv->program);
  delete priv;
}
void Program::push_back(const Shader &shader)
{
  std::cout << "AttachShader: " << shader.priv->handle << std::endl;
  glAttachShader/*ObjectARB*/(priv->program, shader.priv->handle);
  priv->shaders.push_back(&shader);
  shader.priv->programs.push_back(this);
}
void Program::bind_attrib(int num, std::string name)
{
  int val2 = glGetError();
  glBindAttribLocation(priv->program, num, name.c_str());
  int val = glGetError();
  std::cout << "BindAttribLocation: " << val << std::endl;
}
void Program::detach(const Shader &shader)
{
  std::cout << "detach " << shader.priv->shader->Name() << std::endl;
  glDetachShader /*ObjectARB*/(priv->program, shader.priv->handle); 
  priv->shaders.erase(std::remove(priv->shaders.begin(), priv->shaders.end(),&shader), priv->shaders.end());
  shader.priv->programs.erase(std::remove(shader.priv->programs.begin(), shader.priv->programs.end(), this), shader.priv->programs.end());
}
void Program::GeomTypes(int input, int output)
{
  int inputtype, outputtype;
  switch(input)
    {
    case 0: inputtype = GL_POINTS; break;
    case 1: inputtype = GL_LINES; break;
    case 2: inputtype = GL_TRIANGLES; break;
    case 3: inputtype = GL_LINES_ADJACENCY_EXT; break;
    case 4: inputtype = GL_TRIANGLES_ADJACENCY_EXT; break;
    case 5: inputtype = GL_QUADS; break;
    default: inputtype = GL_POINTS; break;
    };
  switch(output)
    {
    case 0: outputtype = GL_POINTS; break;
    case 1: outputtype = GL_LINE_STRIP; break;
    case 2: outputtype = GL_TRIANGLE_STRIP; break;
    case 3: outputtype = GL_QUADS; break;
    case 4: outputtype = GL_TRIANGLES; break;
    default: outputtype = GL_POINTS; break;
    };

  glProgramParameteriEXT(priv->program, GL_GEOMETRY_INPUT_TYPE_EXT, inputtype);
  glProgramParameteriEXT(priv->program, GL_GEOMETRY_OUTPUT_TYPE_EXT, outputtype);
}
void Program::GeomOutputVertices(int i)
{
  glProgramParameteriEXT(priv->program, GL_GEOMETRY_VERTICES_OUT_EXT, i);
}

void Program::link()
{
  glLinkProgram(priv->program);
  //int len=0;
  //char log[255];
  //glGetInfoLogARB(priv->program, 255, &len, log);
  //log[len]=0;
  //std::cout << log << std::endl;
}
void Program::use()
{
  glUseProgram(priv->program);
}
void Program::unuse()
{
  glUseProgram(0);
}
void Program::set_var(const std::string &name, float val)
{
  //std::cout << "set_var float:" << name << ":" << val << std::endl;
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform1f(loc, val);
}
void Program::set_var(int loc, float val)
{
  glUniform1f(loc, val);
}

void Program::set_var(const std::string &name, float val1, float val2)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform2f(loc, val1, val2);
}
void Program::set_var(const std::string &name, const Point &p)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform3f(loc, p.x,p.y,p.z);
}
void Program::set_var(const std::string &name, const Vector &v)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform3f(loc, v.dx,v.dy,v.dz);
}

void Program::set_var(const std::string &name, const Color &c)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform3f(loc, c.r/255.0, c.g/255.0, c.b/255.0);
}

int Program::get_loc(std::string name)
{
  return glGetUniformLocation(priv->program, name.c_str());
}
void Program::set_var(int loc, float val1, float val2, float val3, float val4)
{
  glUniform4f(loc, val1, val2, val3, val4);
}
void Program::set_var(const std::string &name, float val1, float val2, float val3, float val4)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform4f(loc, val1, val2, val3, val4);
}
void Program::set_var(const std::string &name, float *array, int count)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform1fv(loc, count, array);
}
void Program::set_var(const std::string &name, int *array, int count)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform1iv(loc, count, array);
}
void Program::set_var(const std::string &name, int val)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  glUniform1i(loc, val);  
}
void Program::set_var(const std::string &name, Matrix m)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  //std::cout << "glUniformLocation: " << loc << ":" << glGetError() << std::endl;
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };

  glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&mat[0]);
  //int val = glGetError();
  //std::cout << "UniformMatrix: " << std::hex << val << std::endl;

}
Attrib Program::find_attr(const std::string &attr_name, int id)
{
  Attrib atr;
  atr.loc = glGetAttribLocation(priv->program, attr_name.c_str());
  atr.id = id;
  atr.is_int = false;
  return atr;
}
Attrib Program::find_attr_int(const std::string &attr_name, int id)
{
  Attrib atr;
  atr.loc = glGetAttribLocation(priv->program, attr_name.c_str());
  atr.id = id;
  atr.is_int = true;
  return atr;
}
void Program::attr_loc(std::string s, int index)
{
  glBindAttribLocation(priv->program, index, s.c_str());
}

void MoveObjectParameters::SetParameters(Program &p) const
{
    int num = coll.NumPoints();
    float *pos_x = new float[num];
    float *pos_y = new float[num];
    float *pos_z = new float[num];
    for(int i = 0; i<num;i++)
      {
	Point p = coll.Points(i);
	pos_x[i] = p.x;
	pos_y[i] = p.y;
	pos_z[i] = p.z;
      }
    p.set_var("pos_x", pos_x, num);
    p.set_var("pos_y", pos_y, num);
    p.set_var("pos_z", pos_z, num);

}

std::string stringify(int i)
{
  std::stringstream s;
  s << i;
  return s.str();
}

ShaderFile::ShaderFile(std::string filename)
{
  std::cout << "Filename: " << filename << std::endl;
  std::ifstream file(filename.c_str());
  std::string line;
  std::string id = "Unknown";
  bool vertex = false;
  bool geom = false;
  std::string block;
  while(std::getline(file, line))
    {
      if (line.substr(0,4)=="//V:" || line.substr(0,4)=="//F:"|| line.substr(0,4)=="//G:")
	{
	  if (geom)
	    {
	      //std::cout << "G: " << block << std::endl;
	      g_shaders[id]=block;
	    }
	  else if (vertex)
	    {
	      //std::cout << "V: " << block << std::endl;
	      v_shaders[id]=block;
	    }
	  else
	    {
	      //std::cout << "F: " << block << std::endl;
	      f_shaders[id]=block;
	    }
	  block="";
	}
      if (line.substr(0,4)=="//V:") { id = line.substr(5); vertex=true; geom=false; }
      else if (line.substr(0,4)=="//F:") { id = line.substr(5); vertex=false; geom=false; }
      else if (line.substr(0,4)=="//G:") { id = line.substr(5); geom=true; }
      else 
	{
	  //std::cout << "Line: " << line << std::endl;
	  block += line;
	  block += "\n";
	}
    }
  if (geom)
    {
      g_shaders[id]=block;
    }
  else if (vertex)
    {
      v_shaders[id]=block;
    }
  else
    {
      f_shaders[id]=block;
    }
}
std::string ShaderFile::VertexShader(std::string name)
{
  return v_shaders[name];
}
std::string ShaderFile::FragmentShader(std::string name)
{
  return f_shaders[name];
}
std::string ShaderFile::GeometryShader(std::string name)
{
  return g_shaders[name];
}
int ShaderSeq::GetShader(std::string v_format, std::string f_format, std::string g_format)
{
  int id = progs.size();
  Program *p = new Program;
  progs.push_back(p);
  std::string::iterator i = v_format.begin();
  while(i!=v_format.end())
    {
      std::string::iterator ii = std::find(i, v_format.end(), ':');
      std::string name(i, ii);
      std::cout << "VName: " << name << std::endl;
      std::string shader = file.VertexShader(name);
      ShaderSpec *spec = new SingletonShaderSpec(shader);
      Shader *sha1;
      sha1 = new Shader(*spec, true, false);
      p->push_back(*sha1);
      if (ii!=v_format.end())
	ii++;
      i = ii;
    }
  i = f_format.begin();
  while(i!=f_format.end())
    {
      std::string::iterator ii = std::find(i, f_format.end(), ':');
      std::string name(i, ii);
      std::cout << "FName: " << name << std::endl;
      std::string shader = file.FragmentShader(name);
      ShaderSpec *spec = new SingletonShaderSpec(shader);
      Shader *sha2 = new Shader(*spec, false, false);
      p->push_back(*sha2);
      if (ii!=f_format.end())
	ii++;
      i = ii;
    }
  i = g_format.begin();
  while(i!=g_format.end())
    {
      std::string::iterator ii = std::find(i, g_format.end(), ':');
      std::string name(i, ii);
      std::cout << "GName: " << name << std::endl;
      std::string shader = file.GeometryShader(name);
      ShaderSpec *spec = new SingletonShaderSpec(shader);
      Shader *sha2 = new Shader(*spec, false, true);
      p->push_back(*sha2);
      if (ii!=g_format.end())
	ii++;
      i = ii;
    }

  //p->link();
  //p->GeomTypes(5,2); 
  //p->GeomOutputVertices(100000);
  return id;
}
void ShaderSeq::link(int i)
{
  progs[i]->link();
}
void ShaderSeq::use(int i)
{
  progs[i]->use();
}
void ShaderSeq::unuse(int i)
{
  progs[i]->unuse();
}
Program *ShaderSeq::prog(int i)
{
  return progs[i];
}

struct ParseResult
{
  bool succeed;
  std::string name;
  std::vector<std::string> param_name;
};
std::string trim(std::string s)
{
  unsigned int i=0;
  while(i<s.size() && s[i]==' ') i++;
  int ii = s.size()-1;
  while(ii > 0 && s[ii]==' ') ii--;
  return s.substr(i,ii-i);
}

std::vector<std::string> parse_array(std::string s)
{
  std::vector<std::string> vec;
  std::string::iterator i = s.begin();
  while(i != s.end())
    {
      std::string::iterator old_i = i;
      i = std::find(i, s.end(), ',');
      vec.push_back(trim(std::string(old_i, i)));
      if (i!= s.end())
	i++;
    }
  return vec;
}

ParseResult parse_fcall(std::string s)
{
  ParseResult res;
  std::string::iterator i = std::find(s.begin(), s.end(), '(');
  if (i == s.end())
    {
      res.succeed = false;
      return res;
    }
  res.name = std::string(s.begin(), i);
  std::string::iterator ii = std::find(i+1, s.end(), ')');
  if (ii == s.end())
    {
      res.succeed = false;
      return res;
    }
  res.param_name = parse_array(std::string(i,ii));
  return res;
}

struct AnimStateI
{
  AnimStateI(ShaderFile &f) : seq(f) { }
  VBOState vbostate;
  ShaderSeq seq;
  int s;
};

AnimState::AnimState(ShaderFile &f)
{
  priv = new AnimStateI(f);
}
AnimState::~AnimState()
{
  delete priv;
}

void UpdateAnim(Anim &anim, AnimState &state, const std::vector<Attrib> &attribs)
{
  state.priv->s = state.priv->seq.GetShader("choose_object","fragmain","");
  state.priv->seq.use(state.priv->s);
  Program *prog = state.priv->seq.prog(state.priv->s);

  std::vector<Attrib> attrib(attribs);
  Attrib a = prog->find_attr("index", AttrFrame); // blob
  attrib.push_back(a);

  AnimFaceCollectionArray arr(anim);
  FrameFaceCollectionArray arr2(arr);
  CompressObject compress(arr2);
  UpdateVBO(compress, state.priv->vbostate, UpdateAll, attrib);
}

void DrawAnim(float start, float end, AnimState &state, const std::vector<Attrib> &attribs)
{
  state.priv->seq.use(state.priv->s);
  Program *prog = state.priv->seq.prog(state.priv->s);
  //std::cout << "Frame: ";
  //for(int i=0;i<select.Size();i++)
  //  {
  //    std::cout << select.SelectArray()[i];
  //  }
  //std::cout << std::endl;
  //select.SelectArray()[1] = 1;

  //prog->set_var("array", select.SelectArray(), select.Size());
  prog->set_var("start", start);
  prog->set_var("end", end);

  std::vector<Attrib> attr(attribs);
  Attrib a = prog->find_attr("index", AttrFrame);  
  attr.push_back(a);

  DrawVBO(state.priv->vbostate, UpdateAll, attr);

  state.priv->seq.unuse(state.priv->s);
}
