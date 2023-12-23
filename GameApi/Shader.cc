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
//#include <GL/glew.h>
//#include <SDL_opengl.h>
#include "VectorTools.hh"
#include <vector>
#include <algorithm>
#include "Effect.hh" 
#include <fstream>
#include "GraphI.hh"

#include "GameApi_low.hh"

// this flag needs to be changes also from
// 1) GameApi_h.hh
// 2) Main.cc
// 3) Shader.cc
//#define OPENGL_ES 1

#ifdef RASPI
#define OPENGL_ES 1
#endif

#ifdef LINUX
//#define OPENGL_ES 1
#endif

#ifdef AMD
#define OPENGL_ES 1
#endif

#ifdef EMSCRIPTEN
#define OPENGL_ES 1
#endif


#ifdef WINDOWS
//#define OPENGL_ES 1
#endif

//#define WAYLAND 1

#ifdef WAYLAND
//#define OPENGL_ES 1
#endif

#ifdef EMSCRIPTEN
#define WEBGL2 1
#endif



#define HAS_GL_GETERROR 1

#if 0
//#ifdef WEBGL2
//#define ATTRIBUTE "in"
//#define VARYING_IN "in"
//#define VARYING_OUT "out"
//#else
//#define ATTRIBUTE "attribute"
//#define VARYING_IN "varying"
//#define VARYING_OUT "varying"
//#endif
#endif

std::string funccall_to_string(ShaderModule *mod);
std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string value);

void InstallProgress(int num, std::string label, int max);
void ProgressBar(int num, int val, int max, std::string label);


struct ShaderPriv
{
  Low_GLuint handle;
  std::vector<Program*> programs;
  ShaderSpec *shader;
};


Shader::Shader(ShaderSpec &shader, bool vertex, bool geom)
{
  //InstallProgress(111, shader.Name().c_str(), 15);
  //ProgressBar(111,0,15,shader.Name().c_str());
  int count = shader.Count();
  bool b = vertex;
  Low_GLuint handle = g_low->ogl->glCreateShader(geom?Low_GL_GEOMETRY_SHADER:(b?Low_GL_VERTEX_SHADER:Low_GL_FRAGMENT_SHADER));
  //ProgressBar(111,5,15,shader.Name().c_str());
  std::vector<std::string> vec;
  const char **strings = new const char *[count];
  int *lengths = new int[count];
  for(int i=0;i<count;i++)
    {
     vec.push_back(shader.Source(i));
     strings[i] = vec[i].c_str();
     lengths[i] = vec[i].size();
    }
  g_low->ogl->glShaderSource(handle, count, strings, lengths);
  //ProgressBar(111,10,15,shader.Name().c_str());
  g_low->ogl->glCompileShader(handle);

  int res=0;
  g_low->ogl->glGetShaderiv(handle, Low_GL_COMPILE_STATUS, &res);
  //std::cout << "COMPILE STATUS: " << res << std::endl;
  if (res!=1) {

  
#ifdef HAS_GL_GETERROR
  int val = g_low->ogl->glGetError();
#else
  int val = Low_GL_NO_ERROR;
#endif
  //ProgressBar(111,15,15,shader.Name().c_str());

  // if (val!=Low_GL_NO_ERROR)
    {
    std::cout << "glCompileShader ERROR: " << val << std::endl;
    char buf[256];
    int length=0;
    g_low->ogl->glGetShaderInfoLog(handle, 256, &length, &buf[0]);
    buf[length]=0;
    if (length>0)
      std::cout << "" << buf << std::endl;

  }
  }

  delete [] strings; strings=0;
  delete [] lengths; lengths = 0;
  priv = new ShaderPriv;
  priv->handle = handle;
  priv->shader = &shader;
}
void Shader::print_log() const
{
#if 0
      int len=0;
      char buf[255];
      g_low->ogl->glGetShaderInfoLog(priv->handle, 255, &len, buf);
      std::cout << " ShaInfoLog: " << buf << std::endl;
#endif
}
Shader::~Shader()
{
  std::vector<Program*> vec(priv->programs.begin(), priv->programs.end());
  for(std::vector<Program*>::iterator i = vec.begin();i!=vec.end();i++)
    {
      (*i)->detach(*this);
    }
  g_low->ogl->glDeleteShader(priv->handle);
  delete priv;
}


struct ProgramPriv
{
  Low_GLuint program;
  std::vector<const Shader*> shaders;
};

Program::Program()
{
  //std::cout << "VENDOR=" << glGetString(GL_VENDOR) << std::endl;
  //std::cout << "RENDERER=" << glGetString(GL_RENDERER) << std::endl;
  //std::cout << "VERSION=" << glGetString(GL_VERSION) << std::endl;
  priv = new ProgramPriv;
  priv->program = g_low->ogl->glCreateProgram(); //ObjectARB();  
}
Program::~Program()
{
  std::vector<const Shader*> vec(priv->shaders.begin(), priv->shaders.end());
  for(std::vector<const Shader*>::iterator i = vec.begin();i!=vec.end();i++)
    {
      detach(*(*i));
      delete (*i);
    }
  g_low->ogl->glDeleteProgram(priv->program);
  delete priv;
}
void Program::push_back(const Shader &shader)
{
  //std::cout << "AttachShader: " << shader.priv->handle << std::endl;
  g_low->ogl->glAttachShader/*ObjectARB*/(priv->program, shader.priv->handle);
  /*
  int val = g_low->ogl->glGetError();
  if (val!=Low_GL_NO_ERROR)
    {
      //std::cout << "glAttachShader ERROR: " << val << std::endl;
    char buf[256];
    int length=0;
    g_low->ogl->glGetProgramInfoLog(priv->program, 256, &length, &buf[0]);
    buf[length]=0;
    std::cout << "" << buf << std::endl;
    }
  */
  priv->shaders.push_back(&shader);
  shader.priv->programs.push_back(this);
}
void Program::bind_frag(int num, std::string name)
{
#ifndef EMSCRIPTEN
  g_low->ogl->glBindFragDataLocation( priv->program, num, name.c_str());
#endif
}
void Program::bind_attrib(int num, std::string name)
{
  g_low->ogl->glBindAttribLocation(priv->program, num, name.c_str());
}
void Program::detach(const Shader &shader)
{
  std::cout << "detach " << shader.priv->shader->Name() << std::endl;
  g_low->ogl->glDetachShader /*ObjectARB*/(priv->program, shader.priv->handle); 
  priv->shaders.erase(std::remove(priv->shaders.begin(), priv->shaders.end(),&shader), priv->shaders.end());
  shader.priv->programs.erase(std::remove(shader.priv->programs.begin(), shader.priv->programs.end(), this), shader.priv->programs.end());
}
void Program::GeomTypes(int input, int output)
{
  int inputtype, outputtype;
  switch(input)
    {
    case 0: inputtype = Low_GL_POINTS; break;
    case 1: inputtype = Low_GL_LINES; break;
    case 2: inputtype = Low_GL_TRIANGLES; break;
    case 3: inputtype = Low_GL_LINES_ADJACENCY_EXT; break;
    case 4: inputtype = Low_GL_TRIANGLES_ADJACENCY_EXT; break;
    case 5: inputtype = Low_GL_QUADS; break;
    default: inputtype = Low_GL_POINTS; break;
    };
  switch(output)
    {
    case 0: outputtype = Low_GL_POINTS; break;
    case 1: outputtype = Low_GL_LINE_STRIP; break;
    case 2: outputtype = Low_GL_TRIANGLE_STRIP; break;
    case 3: outputtype = Low_GL_QUADS; break;
    case 4: outputtype = Low_GL_TRIANGLES; break;
    default: outputtype = Low_GL_POINTS; break;
    };

  g_low->ogl->glProgramParameteriEXT(priv->program, Low_GL_GEOMETRY_INPUT_TYPE_EXT, inputtype);
  g_low->ogl->glProgramParameteriEXT(priv->program, Low_GL_GEOMETRY_OUTPUT_TYPE_EXT, outputtype);
}
void Program::GeomOutputVertices(int i)
{
  g_low->ogl->glProgramParameteriEXT(priv->program, Low_GL_GEOMETRY_VERTICES_OUT_EXT, i);
}

void Program::link()
{
  g_low->ogl->glLinkProgram(priv->program);
  /*
  int res=0;
  g_low->ogl->glGetProgramiv(priv->program, Low_GL_LINK_STATUS, &res);

  if (res!=1) {
  int val = g_low->ogl->glGetError();
  std::cout << "LINK ERROR: " << val << std::endl;
  if (val!=Low_GL_NO_ERROR)
  {
  int len=0;
  char log[255];
  g_low->ogl->glGetProgramInfoLog(priv->program, 255, &len, log);
  log[len]=0;
  if (len>0)
    std::cout << "LINK ERROR: " << std::endl << log << std::endl;
  }
  }
  */
}
void Program::print_log()
{
  int s = priv->shaders.size();
  for(int i=0;i<s;i++) {
    const Shader *sh = priv->shaders[i];
    sh->print_log();
  }
}
void Program::use()
{
  g_low->ogl->glUseProgram(priv->program);
}
void Program::unuse()
{
  g_low->ogl->glUseProgram(0);
}

void Program::set_var(const char *name, float val)
{
  //std::cout << "set_var float:" << name << ":" << val << std::endl;
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];
  g_low->ogl->glUniform1f(loc, val);
}
void Program::set_var(int loc, float val)
{
  g_low->ogl->glUniform1f(loc, val);
}

void Program::set_var(const char *name, float val1, float val2)
{
  // Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform2f(loc, val1, val2);
}
void Program::set_var(const char *name, const Point &p)
{
  // Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform3f(loc, p.x,p.y,p.z);
}
void Program::set_var(const char *name, const Vector &v)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform3f(loc, v.dx,v.dy,v.dz);
}

void Program::set_var(const char *name, const Color &c)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform3f(loc, c.r/255.0, c.g/255.0, c.b/255.0);
}

int Program::get_loc(std::string name)
{
  Low_GLint loc;
  if (locs.find(name.c_str())==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
    locs[name.c_str()]=loc;
  }
  else
    loc=locs[name.c_str()];
  return loc;
  //return g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
}
void Program::set_var(int loc, float val1, float val2, float val3, float val4)
{
  g_low->ogl->glUniform4f(loc, val1, val2, val3, val4);
}
void Program::set_var(const char *name, float val1, float val2, float val3, float val4)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform4f(loc, val1, val2, val3, val4);
}
void Program::set_var(const char *name, float *array, int count)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform1fv(loc, count, array);
}
void Program::set_var(const char *name, int *array, int count)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform1iv(loc, count, array);
}
void Program::set_var(const char *name, int val)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform1i(loc, val);  
}
Matrix Program::get_matrix_var(const std::string &name)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name.c_str())==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
    locs[name.c_str()]=loc;
  }
  else
    loc=locs[name.c_str()];

  float mat[16];
  g_low->ogl->glGetUniformfv( priv->program, loc, &mat[0]);
  
  Matrix m = { { mat[0], mat[4], mat[8], mat[12],
		 mat[1], mat[5], mat[9], mat[13],
		 mat[2], mat[6], mat[10], mat[14],
		 mat[3], mat[7], mat[11], mat[15] } };
  return m;
  
}
void Program::set_var_matrix(const char *name, const std::vector<float> &v)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniformMatrix4fv(loc, v.size()/16, Low_GL_FALSE, &v[0]);

#if 0
  int s = v.size()/16;
  for(int i=0;i<s;i++)
    {
      const float *ptr = &v[i*16];
      std::stringstream ss;
      ss << name << "[" << i << "]";
    }

#endif
}
void Program::set_var(const char *name, const std::vector<Point> &v)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  g_low->ogl->glUniform3fv(loc, v.size(), (float*)&v[0]);
}
void Program::set_var(const char *name, Matrix m)
{
  //Low_GLint loc = g_low->ogl->glGetUniformLocation(priv->program, name.c_str());
  Low_GLint loc;
  if (locs.find(name)==locs.end()) {
    loc = g_low->ogl->glGetUniformLocation(priv->program, name);
    locs[name]=loc;
  }
  else
    loc=locs[name];

  //std::cout << "glUniformLocation: " << loc << ":" << glGetError() << std::endl;
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };

  g_low->ogl->glUniformMatrix4fv(loc, 1, Low_GL_FALSE, (float*)&mat[0]);
  //int val = glGetError();
  //std::cout << "UniformMatrix: " << std::hex << val << std::endl;

}
Attrib Program::find_attr(const std::string &attr_name, int id)
{
  Attrib atr;
  atr.loc = g_low->ogl->glGetAttribLocation(priv->program, attr_name.c_str());
  atr.id = id;
  atr.is_int = false;
  return atr;
}
Attrib Program::find_attr_int(const std::string &attr_name, int id)
{
  Attrib atr;
  atr.loc = g_low->ogl->glGetAttribLocation(priv->program, attr_name.c_str());
  atr.id = id;
  atr.is_int = true;
  return atr;
}
void Program::attr_loc(std::string s, int index)
{
  g_low->ogl->glBindAttribLocation(priv->program, index, s.c_str());
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

#ifdef EMSCRIPTEN
#define OLD_SHADER 1
#endif
#ifdef OPENGL_ES
#define OLD_SHADER 1
#endif
#ifdef RASBERRY
#define OLD_SHADER 1
#endif 
//#define OLD_SHADER 1

std::string get_uber_shader(bool oldshader, bool webgl2, bool emscripten, bool apple)
{
  std::string ATTRIBUTE="in";
  std::string VARYING_IN="in";
  std::string VARYING_OUT="out";

  if (!webgl2) {
    ATTRIBUTE="attribute";
    VARYING_IN="varying";
    VARYING_OUT="varying";
  }
  

  
  std::string s = "";
  
  if (oldshader) {
    s+="//V: comb\n";
    if (webgl2) {
      s+="#version 300 es\n";
    } else {
      s+="#version 100\n";
    }
   
    //s+="#extension GL_ARB_gpu_shader5 : enable\n"
    // NOTE: ADDING MORE uniform or attribute or varying varibles does not work, and gives black screen
  s+=  "precision highp float;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"uniform mat4 in_iMV;\n"
"#ifdef IN_N\n"
"uniform mat4 in_N;\n"
"#endif\n"
"uniform float in_POS;\n"
"uniform float time;\n"
"#ifdef INST\n" +
ATTRIBUTE + " vec3 in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n" +
ATTRIBUTE +" mat4 in_InstMat;\n"
"#endif\n" +
ATTRIBUTE + " vec3 in_Position;\n" +
ATTRIBUTE + " vec3 in_Position2;\n"
"#ifdef IN_NORMAL\n" +
ATTRIBUTE + " vec3 in_Normal;\n"
"#endif\n"
"#ifdef IN_COLOR\n" +
ATTRIBUTE + " vec4 in_Color;\n"
"#endif\n"
"#ifdef IN_TEXCOORD\n"
+ ATTRIBUTE + " vec3 in_TexCoord;\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
+ VARYING_OUT + " vec3 ex_TexCoord;\n"
    "#endif\n"
"#ifdef EX_COLOR\n" +
VARYING_OUT + " vec4 ex_Color;\n"
    "#endif\n"
    //"flat varying vec4 ex_FlatColor;\n"
"#ifdef EX_NORMAL\n"
+ VARYING_OUT + " vec3 ex_Normal;\n"
    "#endif\n"
"#ifdef EX_POSITION\n"
+ VARYING_OUT + " vec3 ex_Position;\n"
    "#endif\n"
"#ifdef EX_NORMAL2\n"
+ VARYING_OUT + " vec3 ex_Normal2;\n"
    "#endif\n"
"#ifdef EX_LIGHTPOS2\n"
+ VARYING_OUT + " vec3 ex_LightPos2;\n"
    "#endif\n"

"#ifdef VERTEX_LEVELS\n"
"uniform vec4 level1_color;\n"
"uniform vec4 level2_color;\n"
"uniform vec4 level3_color;\n"
"#endif\n"

"#ifdef LIGHTDIR\n"
"uniform vec3 light_dir;\n"
"#endif\n"
+ VARYING_OUT + " float fog_intensity;\n"
+ VARYING_OUT + " vec2 shadow_position;\n"
    "#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"uniform vec3 lightpos@;\n"
"#endif\n"
"#endtemplate\n"
"#ifdef SKELETAL\n"
"in int bone_id;\n"
"mat4 bones[50];\n"
"vec3 bone_pos[50];\n"
"#endif\n"

    // NOTE: ADDING MORE uniform or attribute or varying varibles does not work, and gives black screen
"//M:\n"
"//B:\n"


"#ifdef WATER\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
    "vec4 water(vec4 pos)\n"
"{\n"
"    ex_Position = in_Position;\n" //vec3(mat3(in_View2)*mat3(in_iMV)*in_Position);\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n" //mat3(in_View2)*mat3(in_iMV)*(in_Position + in_InstPos);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0)*in_InstMat);\n" //mat3(in_View2)*mat3(in_iMV)* (in_Position * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"    

    "#ifdef SKELETAL\n"
"vec4 skeletal(vec4 pos)\n"
"{\n"
"    mat4 m = bones[bone_id];\n"
"    vec3 p = bone_pos[bone_id];\n"
"    vec4 p2 = pos * m;\n"
"    vec4 p3 = p2 + vec4(p,0.0);\n"
"    return p3;\n"
"}\n"
"#endif\n"
"vec4 ambient(vec4 pos)\n"
"{\n"
"    return pos;\n"
"}\n"
"    mat4 transpose_2(mat4 i0)\n"
"    {\n"
"       mat4 i2;\n"
"       for(int i=0;i<4;i++) { \n"
"          for(int j=0;j<4;j++) {\n"
"           i2[i][j]=i0[j][i];\n"
"      } }\n"
"       return i2;\n"
"    }\n"
"#ifdef INVERSE\n"
"#endif\n"
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"vec4 diffuse(vec4 pos)\n"
"{\n"
   "    ex_Normal2 = normalize(mat3(in_iMV)*in_Normal);\n"
    "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"vec4 specular(vec4 pos)\n"
"{\n"
    //"    ex_Normal2 = normalize(vec3(in_T * in_MV * vec4(in_Normal,0.0)));\n"
    ////"    ex_Normal2 = normalize(mat3(in_iMV)*in_Normal);\n"
    // "    ex_Normal2 = mat3(in_iMV)*in_Normal;\n"
    ////"    ex_LightPos2 = normalize(vec3(in_T*in_MV*vec4(light_dir,1.0)));\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_N\n"
"#ifdef IN_NORMAL\n"
"#ifdef EX_NORMAL\n"
"vec4 recalc_normal(vec4 pos)\n"
"{\n"
    //"   mat4 inN = transpose(inverse(in_MV));\n"
"   ex_Normal = normalize((vec4(in_Normal,1.0)*in_N).xyz);\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"vec4 flat_shading(vec4 pos)\n"
"{\n"
    //"   ex_FlatColor = in_Color;\n"
"   return pos;\n"
"}\n"
"#ifdef INST\n"
"vec4 inst(vec4 pos)\n"
"{\n"
"   return pos + vec4(in_InstPos,0.0);\n"
"}\n"
"#endif\n"
"#ifdef INSTMAT\n"
"vec4 instmat(vec4 pos)\n"
"{\n"
"   return pos*in_InstMat;\n"
"}\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 passall(vec4 pos)\n"
"{\n"
"  ex_Color = in_Color;\n"
"  ex_TexCoord = in_TexCoord;\n"
"  ex_Normal = in_Normal;\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"uniform float globe_mult;\n"
"vec4 globe(vec4 pos)\n"
"{\n"
"    vec4 pos2 = in_T*in_MV*pos;\n"
"    float d = length(pos2.xz);\n"
    "    float y = globe_mult*d;\n"
        "    y/=2.0;\n"
"    return pos + vec4(0.0,y,0.0,0.0);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 passpos(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"vec4 point_light(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"vec4 coloured_lights(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
    //"#ifdef INST\n"
    //"  ex_Position = in_Position + in_InstPos;\n"
    //"#endif\n"
    //"#ifdef INSTMAT\n"
    //"  ex_Position = in_Position * in_InstMat;\n"
    //"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
    
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"vec4 snoise(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 light(vec4 pos)\n"
"{\n"
"return pos;\n"
"}\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"vec4 ref(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"vec4 wave(vec4 pos)\n"
"{\n"
"  vec4 pos2 = pos + vec4(10.0*cos(2.0*time*in_Position.x*3.14159*2.0/360.0),\n"
"                         10.0*sin(3.0*time*in_Position.y*3.14159*2.0/360.0),\n"
"			 10.0*cos(5.0*time*in_Position.z*3.14159*2.0/360.0), 0.0);\n"
"  return pos2;\n"
"}\n"
"#endif\n"
"vec4 fade(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"vec4 toon(vec4 pos)\n"
"{\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 texture_impl(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 mixshader(vec4 pos)\n"
"{ return pos; }\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 glowedge(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"


"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 manytextures(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 cubemaptextures(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 texture_arr(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"vec4 colour(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"vec4 colour_with_mix(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"

"vec4 empty(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 blur(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"uniform mat4 in_View2;\n"

"vec4 newshadow_1(vec4 pos)\n"
"{\n"
    "    ex_Position = in_Position; \n" //vec3(mat3(in_View2)*mat3(in_iMV)*in_Position);\n"
"#ifdef INST\n"
    "  ex_Position = in_Position + in_InstPos;\n" //mat3(in_View2)*mat3(in_iMV)*(in_Position + in_InstPos);\n"
"#endif\n"
"#ifdef INSTMAT\n"
    "  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n" //mat3(in_View2)*mat3(in_iMV)* (in_Position * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
    "#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"vec4 newshadow_2(vec4 pos)\n"
"{\n"
"    ex_Position = in_Position;\n" //vec3(mat3(in_View2)*mat3(in_iMV)*in_Position);\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n" //mat3(in_View2)*mat3(in_iMV)*(in_Position + in_InstPos);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0)*in_InstMat);\n" //mat3(in_View2)*mat3(in_iMV)* (in_Position * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"


"#ifdef ADJUST\n"
    "vec4 adjust(vec4 pos)\n"
    "{\n"
    "   return pos;\n"
    "}\n"
"#endif\n"    
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"uniform mat4 in_View;\n"
"uniform vec3 in_PhongLightPos;\n"
    "vec4 phong(vec4 pos)\n"
    "{\n"
    //"    vec3 pos2 = vec3(in_MV*vec4(in_Position,0.0));\n"
    "    vec3 n = normalize(mat3(in_View)*mat3(in_iMV)*(in_Normal));\n"
    //"    vec3 h = normalize(normalize(0.0,0.0,-1.0)+normalize(0.0,0.0,-1.0));\n"
    
    "    ex_Normal2 = n;\n"
    "    ex_LightPos2 = in_PhongLightPos;\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef BLURRED_RENDER\n"
    
"vec4 blurred_render(vec4 pos)\n"
"{\n"
    " ex_TexCoord = in_TexCoord;\n"
    "return pos;\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_COLOR\n"
"#ifdef VERTEX_LEVELS\n"
"uniform float hilight;\n"	  

"float V_intensity(vec3 dir) {\n"
" float n = dot(normalize(-dir),normalize(vec3(0.0,0.0,-1.0)));\n"
" return n;\n"
"}\n"
"float V_intensity2(vec3 dir) {\n"
" float i=V_intensity(dir); return pow(i,hilight);\n"
"}\n"

"const float GAMMA2=2.2;\n"
"const float INV_GAMMA2 = 1.0/GAMMA2;\n"
"vec3 V_LINEARtoSRGB2(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA2));\n"
"}\n"
"vec3 V_SRGBtoLINEAR2(vec3 srgbIn)\n"
"{\n"
"  return pow(srgbIn.xyz,vec3(GAMMA2));\n"
"}\n"

    
    "vec4 vertex_phong(vec4 pos)\n"
"{\n"
    "    vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"    
    ///"    ex_Normal2 = n;\n"
    //"    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"

    "    vec3 c = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = n;\n"
    "    c+=V_intensity(normal)*V_SRGBtoLINEAR2(level1_color.rgb);\n"
    "    c=mix(c,V_SRGBtoLINEAR2(level2_color.rgb),V_intensity2(normal));\n"
    //"    c = sqrt(c);\n"
    //"     c+=rgb.rgb;\n"
    //"     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    ex_Color = vec4(c,1.0);\n"
    "    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
"#ifdef SKELETON\n"
+ ATTRIBUTE + " vec4 JOINTS_0;\n"
+ ATTRIBUTE + " vec4 WEIGHTS_0;\n"
    //"uniform mat4 inverseBind[64];\n"
"uniform mat4 jointMatrix[200];\n"
"mat4 getSkinningMatrix()\n"
"{\n"
    "mat4 skin = mat4(0);\n"
    "skin += \n"
    "    WEIGHTS_0.x * jointMatrix[int(JOINTS_0.x)] +\n"
    "    WEIGHTS_0.y * jointMatrix[int(JOINTS_0.y)] +\n"
    "    WEIGHTS_0.z * jointMatrix[int(JOINTS_0.z)] +\n"
    "    WEIGHTS_0.w * jointMatrix[int(JOINTS_0.w)];\n"
    "return skin;\n"
    "}\n"
"vec4 gltf_anim(vec4 pos)\n"
"{\n"
    "   return (getSkinningMatrix()* pos);\n"   
"}\n"
"#endif\n"

    
"#ifdef EX_NORMAL2\n"
"#ifdef IN_NORMAL\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
+ VARYING_OUT + " vec3 eye;\n"
    "vec4 edge(vec4 pos)\n"
"{\n"
    "    vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"
    //"    vec3 n = normalize(mat3(in_iMV)*vec3(in_MV*vec4(in_Normal,1.0)));\n"
"    ex_Normal2 = n;\n"
"    ex_Position = in_Position;\n"
    //"    eye = normalize(vec3(in_MV*vec4(in_Position,0.0)) * -1.0 + vec3(inverse(in_P)*vec4(0,0,-1,0.0)));\n"
"    return pos;\n"
"}\n"    
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 bump_phong(vec4 pos)\n"
"{\n"
    "    ex_Normal2 = normalize(mat3(in_iMV)*  in_Normal);\n"
    "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n" //normalize(vec3(in_T*in_MV*vec4(light_dir,1.0)));\n"
"  ex_TexCoord = in_TexCoord;\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

    
"uniform vec3 shadow_center;\n"
"vec4 shadow(vec4 pos)\n"
"{\n"
"  vec4 pp = pos;\n"
"#ifdef INST\n"
"  pp = pp + vec4(in_InstPos,0.0);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  pp = pp * in_InstMat;\n"
"#endif\n"
    //"   pp = in_MV*pp;\n"
"   pp=pp-vec4(shadow_center,0.0);\n"
"   float r = sqrt(pp.x*pp.x+pp.y*pp.y+pp.z*pp.z);\n"
"   float alfa = acos(pp.z/r);\n"
"   float beta = atan(pp.y,pp.x);\n"
"   beta+=3.14159;\n"
"   alfa/=3.14159;\n"
"   beta/=3.14159*2.0;\n"
"   shadow_position = vec2(alfa,beta);\n"
"    return pos;\n"
"}\n"

    "#ifdef EX_POSITION\n"
    "#ifdef IN_POSITION\n"

"vec4 gi(vec4 pos)\n"
"{\n"
" ex_Position = in_Position;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"    


"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"uniform float fog_dist;\n"
"vec4 fog(vec4 pos)\n"
"{\n"
"  vec4 pp = pos;\n"
"#ifdef INST\n"
"  pp = pp + vec4(in_InstPos,0.0);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  pp = pp * in_InstMat;\n"
"#endif\n"
"    vec4 p = in_MV*pp;\n"
"    float f = p.z;\n"
    "    f/=fog_dist;\n"
"    f = clamp(f,0.0,1.0);\n"
"    fog_intensity = f;\n"
    //"    ex_Position = in_Position;\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef IN_NORMAL\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
    "vec4 dyn_lights(vec4 pos)\n"
    "{\n"
    " ex_Position = in_Position;\n"
    "    ex_Normal2 = normalize(mat3(in_iMV)*in_Normal);\n"
    //"#ifdef INST\n"
    //"  ex_Position = in_Position + in_InstPos;\n"
    //"#endif\n"
    "  return pos;\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_COLOR\n"
"#ifdef EX_COLOR\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"vec4 gltf(vec4 pos)\n"
"{\n"
    "   ex_Position = pos.xyz; /*in_Position*/;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
    "    ex_Normal = normalize(mat3(in_iMV)*in_Normal);\n"

    //    "   ex_Normal = in_Normal;\n"
"   ex_Color = in_Color;\n"
"   ex_TexCoord = in_TexCoord;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

    "//T:\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"\n"
      "//V: empty\n";
    if (webgl2) {
      s+="#version 300 es\n";
    } else {
      s+="#version 100\n";
    }
    
    s+="uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
+ ATTRIBUTE + " vec3 in_Position;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  vec4 v = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"  gl_Position = v;\n"
"}\n"
"\n"
      "//F: comb\n";
    if (emscripten) {
      s+="#version 300 es\n";
    } else {
      s+="#version 100\n";
    }
    if (!emscripten) {
      if (!apple) {
s+="#ifdef TEXTURE_ARRAY\n"
"#extension GL_EXT_texture_array : enable\n"
  "#endif\n";
      }
      
    }

    s+="#ifdef GLTF\n";
    if (webgl2) {
    } else {
   s+= "#extension GL_OES_standard_derivatives : enable\n"
     "#extension GL_NV_shadow_samplers_cube : enable\n";
    }
    
s+="#endif\n"
  "precision highp float;\n";
 if (webgl2) {
   s+="out vec4 out_Color;\n";
 }

    s+="uniform float time;\n"
+ VARYING_IN + " vec4 ex_Color;\n"
    //"flat varying vec4 ex_FlatColor;\n"
    //"out vec4 out_Color;\n"
    //"uniform mat4 in_P;\n"
    //"uniform mat4 in_MV;\n"
    //"uniform mat4 in_T;\n"
    //"uniform mat4 in_N;\n"
"#ifdef EX_TEXCOORD\n"
+ VARYING_IN +   " vec3 ex_TexCoord;\n"
    "#endif\n"
"#ifdef EX_NORMAL\n"
+ VARYING_IN + " vec3 ex_Normal;\n"
    "#endif\n"
"#ifdef EX_POSITION\n"
+ VARYING_IN + " vec3 ex_Position;\n"
    "#endif\n"
"#ifdef EX_NORMAL2\n"
+ VARYING_IN + " vec3 ex_Normal2;\n"
    "#endif\n"
"#ifdef EX_LIGHTPOS2\n"
+ VARYING_IN + " vec3 ex_LightPos2;\n"
    "#endif\n"

"#ifdef COLOR_ARRAY\n"
"uniform vec4 color_array[10];\n"
"#endif\n"
"#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"uniform vec3 lightpos@;\n"
"#endif\n"
"#endtemplate\n"
    //"uniform vec3 light_dir;\n"
"#ifdef LEVELS\n"
    "uniform vec4 level1_color;\n"
 "uniform vec4 level2_color;\n"
 "uniform vec4 level3_color;\n"
"#endif\n"
"uniform float hilight;\n"	  

    "uniform sampler2D tex;\n"
      "uniform sampler2D in_txid;\n";
    if (!emscripten) {
      if (!apple) {
s+="#ifdef TEXTURE_ARRAY\n"
"uniform sampler2DArray texarr;\n"
  "#endif\n";
      }
    }
s+="#ifdef SPECULAR_SIZE\n"
"uniform float specular_size;\n"
"#endif\n"
"#ifdef COLOR_MIX\n"
"uniform float color_mix;\n"
"uniform float color_mix2;\n"
"#endif\n"
"#ifdef EX_POSITION\n"
	"uniform vec3 dyn_light_pos;\n"
	"uniform float dyn_light_dist;\n"
"uniform vec4 c_positions[8];\n"
"uniform float c_scale[8];\n"
"uniform vec4 c_color[8];\n"

"#endif\n"


"#ifdef WATER\n"
"#ifdef EX_POSITION\n"
"uniform vec3 water_center;\n"
"uniform float wave_mult;\n"
"uniform float wave_time;\n"
"uniform vec4 water_color1;\n"
"uniform vec4 water_color2;\n"
"uniform vec4 water_color3;\n"
"vec4 water(vec4 rgb)\n"
"{\n"
"    float d = length(ex_Position-water_center);\n"
"    float s = sin(d*wave_mult+wave_time);\n"
"    s/=2.0; s+=0.5;\n"
   "    if (s<0.5) { return mix(water_color1,water_color2,s*2.0); }\n"
"    return mix(water_color2,water_color3,(s-0.5)*2.0);\n"
"}\n"
"#endif\n"
"#endif\n"


  "#ifdef CHOOSE_COLOR\n"
"uniform vec4 color_choice;\n"
"uniform float mix_val;\n"
"vec4 choose_color(vec4 rgb)\n"
"{\n"
"    return vec4(mix(rgb.rgb,color_choice.rgb,mix_val),rgb.a);\n"
"}\n"
"#endif\n"
"#ifdef MANYTEXTURES\n"
"uniform sampler2D texsampler[15];\n"
"#endif\n"
"#ifdef GLTF\n"
"uniform sampler2D texsampler[8];\n"
"uniform samplerCube texsampler_cube[8];\n"
"#endif\n"
"#ifdef NEWSHADOW\n"
"uniform sampler2D texsampler[1];\n"
"#endif\n"
"#ifdef BUMPPHONG\n"
"uniform sampler2D texsampler[1];\n"
"#endif\n"
"#ifdef CUBEMAPTEXTURES\n"
"uniform samplerCube cubesampler;\n"
"#endif\n"
"//M:\n"
"//B:\n"
"vec4 white(vec4 rgb)\n"
"{\n"
"   return vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"vec4 diffuse(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = ex_Normal2;\n"
"    vec3 light_dir_cam = ex_LightPos2;\n"
"    float theta = clamp(dot( normalize(-normal_cam), normalize(light_dir_cam) ), 0.0,1.0) ;\n"
"    return clamp(rgb + level1_color*theta,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"uniform vec3 in_dx;\n"
"uniform vec3 in_dy;\n"
"uniform vec3 in_pp;\n"
"uniform float in_scale;\n"
"uniform mat4 in_Mat;\n"
"float find_projection_length(vec3 u, vec3 u_x) {\n"
"   float dd = dot(u,u_x)/dot(u_x,u_x);\n"
"   vec3 proj = (dd)*u_x;\n"
"   float len = length(proj);\n"
"   len/=length(u_x);\n"
"   if (dd<0.0) len=-len;\n"
"   return len;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"vec4 newshadow_1(vec4 rgb)\n"
"{\n"
"    vec3 pos = ex_Position;\n"
    //"    vec3 pos2 = vec3(in_Mat * vec4(pos,0.0));\n"
"    float tx_x = find_projection_length(pos,in_dx)/in_scale;\n"
"    float tx_y = find_projection_length(pos,in_dy)/in_scale;\n"
    //"    tx_x=mod((tx_x+1.0),2.0)-1.0;\n" //clamp(tx_x,-1.0,1.0);\n"
    //"    tx_y=mod((tx_y+1.0),2.0)-1.0;\n" //clamp(tx_y,-1.0,1.0);\n"
    //"    tx_x=clamp(tx_x,-1.0,1.0);\n"
    //"    tx_y=clamp(tx_y,-1.0,1.0);\n"
    //"    vec2 tx = vec2(tx_x,tx_y);\n"
"    float dist = length(pos - (in_pp+tx_x*(in_dx)+tx_y*(in_dy)));\n"
    //#ifdef WEBGL2
    //"    texture(in_txid,tx).r = min(texture(in_txid,tx).r,dist);\n"
    //#else
    //"    texture2D(in_txid,tx).r = min(texture2D(in_txid,tx).r,dist);\n"
    //#endif
    "   dist=clamp(dist,0.0,800.0);\n"
"    return vec4(dist/800.0,dist/800.0,dist/800.0,1.0);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"uniform float in_darklevel;\n"
"uniform float in_lightlevel;\n"
"uniform int texindex;\n"
"float newshadow_level()\n"
"{\n"
"    vec3 pos = ex_Position;\n"
    //"    vec3 pos2 = vec3(in_Mat * vec4(pos,0.0));\n"
"    float tx_x = find_projection_length(pos,in_dx)/in_scale;\n"
"    float tx_y = find_projection_length(pos,in_dy)/in_scale;\n"
    //    "    tx_x=mod((tx_x+1.0),2.0)-1.0;\n" //clamp(tx_x,-1.0,1.0);\n"
    //  "    tx_y=mod((tx_y+1.0),2.0)-1.0;\n" //clamp(tx_y,-1.0,1.0);\n"
    //"    tx_x=clamp(tx_x,-1.0,1.0);\n"
    //"    tx_y=clamp(tx_y,-1.0,1.0);\n"
    //"    vec2 tx = vec2(tx_x/2.0+1.0,tx_y/2.0+1.0);\n"
    "vec2 tx=vec2(1.0-((tx_x+1.0)/2.0),(tx_y+1.0)/2.0);\n"
  "    tx=clamp(tx,vec2(0.0,0.0),vec2(1.0,1.0));\n";
 if (webgl2) {
   s+= "    float d;\n"
    "if (texindex==0)\n"
     "       d = texture(texsampler[0] /*in_txid*/,tx).r*800.0;\n";
   if (0) {
   s+= "if (texindex==1)\n"
    "       d = texture(texsampler[1] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==2)\n"
    "       d = texture(texsampler[2] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==3)\n"
    "       d = texture(texsampler[3] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==4)\n"
    "       d = texture(texsampler[4] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==5)\n"
    "       d = texture(texsampler[5] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==6)\n"
    "       d = texture(texsampler[6] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==7)\n"
    "       d = texture(texsampler[7] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==8)\n"
    "       d = texture(texsampler[8] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==9)\n"
    "       d = texture(texsampler[9] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==10)\n"
    "       d = texture(texsampler[10] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==11)\n"
    "       d = texture(texsampler[11] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==12)\n"
    "       d = texture(texsampler[12] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==13)\n"
    "       d = texture(texsampler[13] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==14)\n"
    "       d = texture(texsampler[14] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==15)\n"
    "       d = texture(texsampler[15] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==16)\n"
     "       d = texture(texsampler[16] /*in_txid*/,tx).r*800.0;\n";
   }
 } else { // !webgl2
 
    s+="    float d;\n"
    "if (texindex==0)\n"
      " d= texture2D(texsampler[0] /*in_txid*/,tx).r*800.0;\n";
    if (0) {
   s+= "if (texindex==1)\n"
    " d= texture2D(texsampler[1] /*in_txid*/,tx).r*800.0;\n"

    "if (texindex==2)\n"
    " d= texture2D(texsampler[2] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==3)\n"
    " d= texture2D(texsampler[3] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==4)\n"
    " d= texture2D(texsampler[4] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==5)\n"
    " d= texture2D(texsampler[5] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==6)\n"
    " d= texture2D(texsampler[6] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==7)\n"
    " d= texture2D(texsampler[7] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==8)\n"
    " d= texture2D(texsampler[8] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==9)\n"
    " d= texture2D(texsampler[9] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==10)\n"
    " d= texture2D(texsampler[10] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==11)\n"
    " d= texture2D(texsampler[11] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==12)\n"
    " d= texture2D(texsampler[12] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==13)\n"
    " d= texture2D(texsampler[13] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==14)\n"
    " d= texture2D(texsampler[14] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==15)\n"
     " d= texture2D(texsampler[15] /*in_txid*/,tx).r*800.0;\n";
    }
 }

 s+="    float d2 = length(pos - (in_pp + tx_x*(in_dx) + tx_y*(in_dy)));\n"
    "   d2=clamp(d2,0.0,800.0);\n"
"    bool shade = d2 < d;\n"
"    float level;\n"
"    if (shade) { level=in_darklevel; }\n"
"    else { level=in_lightlevel; }\n"
" return level;\n"
    // " float dd = abs(d-d2)/800.0;\n"
    //" dd = clamp(dd,-1.0,1.0);\n"
    //"    return vec4(d/800.0,d2/800.0,0.0/800.0,1.0);\n"
    //    //"  return vec4(0.0,0.0/800.0,d/800.0,1.0);\n"
    //"return vec4(tx.x,0.0,0.0,1.0);\n"
    //"return vec4(tx.x,tx.y,0.0,1.0);\n"
    //   "return vec4(d/800.0,dd/800.0,0.0,1.0);\n"
    //          "    return vec4((level)*rgb.rgb,1.0);\n"
    //" return rgb;\n"
"}\n"
"vec4 newshadow_2(vec4 rgb) {\n"
"   float level = newshadow_level();\n"
"   return vec4(level*rgb.rgb,1.0);\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef ADJUST\n"
"uniform vec4 ad_color;\n"
"uniform float ad_light;\n"
"uniform float ad_dark;\n"
"vec4 adjust(vec4 rgb)\n"
   "{\n"
   "   vec3 cc = ad_color.rgb*rgb.rgb;\n"
   "   cc *= (ad_light-ad_dark);\n"
   "   cc += vec3(ad_dark,ad_dark,ad_dark);\n"
   "   return vec4(cc,rgb.a);\n"
   "}\n"   
"#endif\n"

"#ifdef EX_TEXCOORD\n"
"#ifdef COLOR_MIX\n"
"#ifdef BLURRED_RENDER\n"
"uniform int blur_num_samples;\n"
"uniform float blur_radius;\n"

"const float GAMMA3=2.2;\n"
"const float INV_GAMMA3 = 1.0/GAMMA3;\n"
"vec4 V_LINEARtoSRGB3(vec4 color)\n"
"{\n"
" return vec4(pow(color.xyz, vec3(INV_GAMMA3)),color.a);\n"
"}\n"
"vec4 V_SRGBtoLINEAR3(vec4 srgbIn)\n"
"{\n"
"  return vec4(pow(srgbIn.xyz,vec3(GAMMA3)),srgbIn.a);\n"
"}\n"

   "vec4 blurred_render(vec4 rgb)\n"
"{\n"
" vec4 t2=vec4(0.0,0.0,0.0,0.0);\n"
"  for(int i=0;i<blur_num_samples;i++)\n"
" {\n"
 " vec2 delta = vec2(blur_radius*cos(float(i)*3.14159265*2.0/float(blur_num_samples)),"
   "		     blur_radius*sin(float(i)*3.14159265*2.0/float(blur_num_samples)));";
   
 if (webgl2) {
   s+=  "   vec4 t = V_SRGBtoLINEAR3(texture(tex, ex_TexCoord.xy+delta));\n";
 } else {
   s+=  "   vec4 t = V_SRGBtoLINEAR3(texture2D(tex, ex_TexCoord.xy+delta));\n";
 }
s+=""
  " t2+=t;\n"
"}\n"
  " t2/=float(blur_num_samples);\n"
  "   return V_LINEARtoSRGB3(vec4(mix(rgb.rgb, t2.rgb, color_mix),t2.a));\n"
  //" return t2;
  "}\n"

"#endif\n"
"#endif\n"
"#endif\n"
  
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"

"float intensity(vec3 dir) {\n"
    " float val = clamp(dot(normalize(-dir),normalize(ex_LightPos2)),0.2,1.0);\n"
    //"  val*=0.4;\n"
    //"  val+=1.1*0.4;\n"
    " return val;\n"
    "}\n"
"float intensity2(vec3 dir) {\n"
" float i=intensity(dir); return pow(i,hilight);\n"
"}\n"
    //"float intensity3(vec3 dir) {\n"
    //" float i=intensity(dir); return pow(i,1.2);\n"
    //"}\n"
"vec3 normalmixp(vec3 a, vec3 b) {\n"
"float angle1 = acos(a.z)+acos(b.z);\n"
"float angle2 = atan(a.y,a.x)+atan(b.y,b.x);\n"
"return vec3(sin(angle1)*cos(angle2),sin(angle1)*sin(angle2),cos(angle1));\n"
"}\n"
"const float GAMMA2=2.2;\n"
"const float INV_GAMMA2 = 1.0/GAMMA2;\n"
"vec3 LINEARtoSRGB2(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA2));\n"
"}\n"
"vec3 SRGBtoLINEAR2(vec3 srgbIn)\n"
"{\n"
"  return pow(srgbIn.xyz,vec3(GAMMA2));\n"
"}\n"

"vec4 phong(vec4 rgb)\n"
    "{\n"
    "    vec3 c = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = ex_Normal2;\n"
    "    c+=SRGBtoLINEAR2(rgb.rgb);\n"
"#ifdef PHONG_TEXTURE\n"
    "    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb*rgb.rgb,vec3(1,1,1),0.15));\n"
    "    c=mix(c,SRGBtoLINEAR2(level2_color.rgb),intensity2(normal));\n"
"#endif\n"
"#ifndef PHONG_TEXTURE\n"
    "    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb,vec3(1,1,1),0.1));\n"
    "    c=mix(c,SRGBtoLINEAR2(level2_color.rgb),intensity2(normal));\n"
"#endif\n"
    
    //"    c = sqrt(c);\n"
    //"     c*=rgb.rgb;\n"
    "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    return vec4(LINEARtoSRGB2(c),rgb.a);\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"vec4 shadowphong(vec4 rgb)\n"
"{\n"
"   vec4 rgb2 = phong(rgb);\n"
"   float level = newshadow_level();\n"
"   if (level>0.5) {\n"
"     float level2 = (level-0.5)*2.0;\n"
"     vec4 res = vec4((1.0-level2)*rgb.rgb + level2*rgb2.rgb,1.0);\n"
"     return res;\n"
"   } else {\n"
"     vec4 res = vec4(rgb.rgb*(level*2.0),1.0);\n"
"     return res;\n"
"   }\n"
"}\n"

"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
"#ifdef SKELETON\n"
"vec4 gltf_anim(vec4 rgb)\n"
"    {\n"
"     return rgb;\n"
"    }\n"
"#endif\n"
    "#ifdef EX_NORMAL2\n"
"#ifdef EX_POSITION\n"
"uniform float edge_width;\n"
"uniform vec4 edge_color;\n"
+ VARYING_IN + " vec3 eye;\n"
    "vec4 edge(vec4 rgb)\n"
"{\n"
    "    vec3 view = eye;\n" //-ex_Position + vec3(0.0,0.0,-4000.0);\n"
"    vec3 n = ex_Normal2;\n"
"    float d = 1.0-dot(normalize(view),normalize(n));\n"
    //"    float val = smoothstep(edge_width-0.01,edge_width+0.01,d);\n"
"    vec4 color = rgb;\n"
    //"    if (val>0.5) { }\n"
    " color = d*edge_color;\n"
"    return color;\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef BUMPPHONG\n"

    //"uniform float hilight;\n"	  
    //"float intensity(vec3 dir) {\n"
    //" return dot(dir,ex_LightPos2);\n"
    //"}\n"
    //"float intensity2(vec3 dir) {\n"
    //" float i=intensity(dir); return pow(i,hilight);\n"
    //"}\n"
"float intensitya(vec3 dir) {\n"
" float n = dot(normalize(-dir),normalize(ex_LightPos2));\n"
" return n;\n"
"}\n"
"float intensity2a(vec3 dir) {\n"
" float i=intensitya(dir); return pow(i,hilight);\n"
"}\n"

"vec3 normalmix(vec3 a, vec3 b) {\n"
    //"float angle1 = acos(a.z)+3.0*acos(b.z);\n"
    //"float angle2 = atan(a.y,a.x)+3.0*atan(b.y,b.x);\n"
    //"return vec3(sin(angle1)*cos(angle2),sin(angle1)*sin(angle2),cos(angle1));\n"
    "return normalize(normalize(a)+normalize(b)*0.2);\n"
"}\n"
"vec4 bump_phong(vec4 rgb)\n"
    "{\n"
    "    vec3 c = vec3(0.0,0.0,0.0);\n"
   "    vec3 n = ex_Normal2;\n";
 if (webgl2) {
   s+="    vec3 n2 = texture(texsampler[0],ex_TexCoord.xy).rgb;\n";
 } else {
   s+=   "    vec3 n2 = texture2D(texsampler[0],ex_TexCoord.xy).rgb;\n";
 }
 s+=
    "    n2-=vec3(0.5,0.5,0.5);\n"
    "    n2*=2.0;\n"
    "    vec3 normal=normalmix(n,n2);\n"
    "    c+=intensitya(normal)*level1_color.rgb;\n"
    "    c+=mix(c,level2_color.rgb,intensity2a(normal));\n"
    //"    c=sqrt(c);\n"
    "    c+=rgb.rgb;\n"
    "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    return vec4(c,rgb.a);\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"#ifdef GI\n"
    
"uniform int num_pos;\n"
"uniform vec3 obj_pos[100];\n"
"uniform float obj_size;\n"

"vec4 gi(vec4 rgb)\n"
"{\n"
    // " return rgb;\n" 
"   vec3 frag_pos = ex_Position;\n"
"   float Ka = 0.0;\n"
"   for(int i=0;i<100;i++) {\n"
"     vec3 pos = obj_pos[i];\n"
"     float size = obj_size;\n"
"     pos-=frag_pos;\n"
"     float d = sqrt(pos.x*pos.x+pos.y*pos.y+pos.z*pos.z);\n"
"     float K = tan(asin(size/d));\n"
"     float KK = K*K;\n"
"     Ka+=KK;\n"
"    }\n"
"    Ka/=4.0*3.14159;\n"
"    return clamp(rgb-vec4(Ka,Ka,Ka,0.0),vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));\n"


"}\n"
"#endif\n"
"#endif\n"

"#ifdef MANYTEXTURES\n"
+ VARYING_IN + " vec2 shadow_position;\n"
    "uniform vec4 shadow_dark;\n"
"uniform float shadow_tex;\n"
"vec4 shadow(vec4 rgb)\n"
"{\n"
"   vec2 f = shadow_position;\n"
   "   vec4 tx = vec4(0.0,0.0,0.0,0.0);\n";
 if (webgl2) {
s+="   if (shadow_tex<0.7) tx=texture(texsampler[0],f); else\n"
"   if (shadow_tex<1.7) tx=texture(texsampler[1],f); else\n"
"   if (shadow_tex<2.7) tx=texture(texsampler[2],f); else\n"
"   if (shadow_tex<3.7) tx=texture(texsampler[3],f); else\n"
"   if (shadow_tex<4.7) tx=texture(texsampler[4],f); else\n"
"   if (shadow_tex<5.7) tx=texture(texsampler[5],f); else\n"
"   if (shadow_tex<6.7) tx=texture(texsampler[6],f); else\n"
"   if (shadow_tex<7.7) tx=texture(texsampler[7],f); else\n"
"   if (shadow_tex<8.7) tx=texture(texsampler[8],f); else\n"
"   if (shadow_tex<9.7) tx=texture(texsampler[9],f); else\n"
"   if (shadow_tex<10.7) tx=texture(texsampler[10],f); else\n"
"   if (shadow_tex<11.7) tx=texture(texsampler[11],f); else\n"
"   if (shadow_tex<12.7) tx=texture(texsampler[12],f); else\n"
"   if (shadow_tex<13.7) tx=texture(texsampler[13],f); else\n"
  "     tx=texture(texsampler[14],f);\n";
 } else {
s+="   if (shadow_tex<0.7) tx=texture2D(texsampler[0],f); else\n"
"   if (shadow_tex<1.7) tx=texture2D(texsampler[1],f); else\n"
"   if (shadow_tex<2.7) tx=texture2D(texsampler[2],f); else\n"
"   if (shadow_tex<3.7) tx=texture2D(texsampler[3],f); else\n"
"   if (shadow_tex<4.7) tx=texture2D(texsampler[4],f); else\n"
"   if (shadow_tex<5.7) tx=texture2D(texsampler[5],f); else\n"
"   if (shadow_tex<6.7) tx=texture2D(texsampler[6],f); else\n"
"   if (shadow_tex<7.7) tx=texture2D(texsampler[7],f); else\n"
"   if (shadow_tex<8.7) tx=texture2D(texsampler[8],f); else\n"
"   if (shadow_tex<9.7) tx=texture2D(texsampler[9],f); else\n"
"   if (shadow_tex<10.7) tx=texture2D(texsampler[10],f); else\n"
"   if (shadow_tex<11.7) tx=texture2D(texsampler[11],f); else\n"
"   if (shadow_tex<12.7) tx=texture2D(texsampler[12],f); else\n"
"   if (shadow_tex<13.7) tx=texture2D(texsampler[13],f); else\n"
  "     tx=texture2D(texsampler[14],f);\n";
  }

 s+=   "   float alpha = tx.r;\n"
"   float visibility = 1.0;\n"
"   if (alpha>0.5) visibility = 0.5;\n"
"   rgb = shadow_dark+visibility*rgb;\n"
"   return rgb;\n"
"}\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef FOG\n"
+ VARYING_IN + " float fog_intensity;\n"
    "uniform vec4 fog_dark;\n"
"uniform vec4 fog_light;\n"
"vec4 fog(vec4 rgb)\n"
"{\n"
"   float f = fog_intensity;\n"
	//"   f = 1.0/exp(f*0.5);\n"
"   return max(rgb,mix(fog_dark,fog_light,1.0-f));\n"
	//"   return mix(rgb,vec4(1.0,1.0,1.0,1.0),1.0-f);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_POSITION\n"
	"vec4 dyn_lights(vec4 rgb)\n"
	"{\n"
	" vec3 p = ex_Position-dyn_light_pos;\n"
        " vec3 n = normalize(ex_Normal2);\n"
        " vec3 r = reflect(p,n);\n"
        " vec3 v = vec3(0.0,0.0,-1.0);\n"
	" float d=length(p);\n"
    "d/=dyn_light_dist;\n"
    //"d=1.0-d;\n"
    //"d=clamp(d,0.0,1.0);\n"
           "d=d*d;\n"
           "d=1.0/d;\n"
    "float dt =dot(normalize(v),normalize(r));\n"
    "float dt2 = sqrt(1.0-dt*dt);\n"
    "d=d*dt2;\n"
           "d=clamp(d,0.0,1.0);\n"
    " return vec4(d,d,d,1.0 /*rgb.a*/);\n"
	"}\n"
"#endif\n"
"#endif\n"
	"#ifdef LEVELS\n"
"vec4 ambient(vec4 rgb)\n"
"{\n"
"    return clamp(rgb + level2_color*vec4(0.2,0.2,0.2,1.0)*0.8,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef SPECULAR_SIZE\n"
"#ifdef LEVELS\n"
"vec4 specular(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = ex_Normal2;\n"
"    vec3 light_dir_cam = ex_LightPos2;\n"
"    vec3 R = reflect(-light_dir_cam, normalize(normal_cam));\n"
"    vec3 E = vec3(0.0,0.0,0.5);\n"
"    float alpha =abs(dot(E,R));\n"
"    float p = pow(alpha,specular_size);\n"
"    float p2 = clamp(p, 0.0, 1.0);\n"
"    p2 = 1.0-p2;\n"
"    return clamp(rgb + level3_color * p2 * length(rgb),vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"#ifdef COLOR_FROM_NORMALS\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 flat_shading(vec4 rgb)\n"
"{\n"
    //"   return mix(rgb, ex_FlatColor,0.5);\n"
"   return rgb;\n"
"}\n"
"vec4 bands(vec4 rgb)\n"
"{\n"
    //"   float r = rgb.r;\n"
    //"   float g = rgb.g;\n"
    //"   float b = rgb.b;\n"
    //"   float a = rgb.a;\n"
    //"   int rr = int(r*256.0)&0xfff0;\n"
    //"   int gg = int(g*256.0)&0xfff0;\n"
    //"   int bb = int(b*256.0)&0xfff0;\n"
    //"   int aa = int(a*256.0)&0xfff0;\n"
    //"   float rrr = float(rr)/256.0;\n"
    //"   float ggg = float(gg)/256.0;\n"
    //"   float bbb = float(bb)/256.0;\n"
    //"   float aaa = float(aa)/256.0;\n"
    //"   vec4 rgb2 = vec4(rrr,ggg,bbb,aaa);\n"
"   return rgb;\n"
"}\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_0(vec4 rgb)\n"
"{\n"
"   return color_array[0];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_1(vec4 rgb)\n"
"{\n"
"   return color_array[1];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_2(vec4 rgb)\n"
"{\n"
"   return color_array[2];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_3(vec4 rgb)\n"
"{\n"
"   return color_array[3];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_4(vec4 rgb)\n"
"{\n"
"   return color_array[4];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_5(vec4 rgb)\n"
"{\n"
"   return color_array[5];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_6(vec4 rgb)\n"
"{\n"
"   return color_array[6];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_7(vec4 rgb)\n"
"{\n"
"   return color_array[7];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_8(vec4 rgb)\n"
"{\n"
"   return color_array[8];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_9(vec4 rgb)\n"
"{\n"
"   return color_array[9];\n"
"}\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"vec4 point_light@(vec4 rgb)\n"
"{\n"
"   float dist = length(ex_Position.xyz - lightpos@);\n"
"   float val = 100.0/dist;\n"
"   return vec4(val*vec3(rgb)+vec3(rgb),rgb.a);\n"
"}\n"
"#endif\n"
"#endtemplate\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef COLOURED_LIGHTS\n"
   "vec4 coloured_lights(vec4 rgb)\n"
"{\n"
"   vec3 res=vec3(0.0,0.0,0.0);\n"
    //"   for(int i=0;i<8;i++) {\n"
    //"     vec3 pos = coloured_positions[i];\n"
    //"     vec3 delta = ex_Position.xyz-pos;\n"
    //"     float dist = sqrt(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);\n"
    //"     float d2 = coloured_scale[i]*dist;\n"
    //"     float db = d2*d2;\n"
    //"     if (db<1.0) { db=1.0; }\n"
    //"     float inv = 1.0/db;\n"
"     vec3 cl = c_color[0].rgb;\n"
"     res+=cl;\n"
    //"   }\n"
"   return vec4(clamp(res,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0)),rgb.a);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"vec4 dot2(vec4 rgb)\n"
"{\n"
"   vec3 pos = vec3(ex_TexCoord.xy,1.0);\n"
"   //xpos = normalize(pos);\n"
"   return vec4(pos, 1.0);\n"
"}\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef SNOISE\n"
"vec4 snoise(vec4 rgb)\n"
"{\n"
"   vec2 coords = ex_Position.xy;\n"
"   float v = fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"   return vec4(mix(vec3(rgb), vec3(v,v,v), 0.2),rgb.a);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef BLUR\n"
   "vec4 blur(vec4 rgb)\n"
"{\n"
"  vec2 t_mx = ex_TexCoord.xy + vec2(-0.005,0.0);\n"
"  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-0.005);\n"
"  vec2 t_px = ex_TexCoord.xy + vec2(0.005,0.0);\n"
"  vec2 t_py = ex_TexCoord.xy + vec2(0.0,0.005);\n"
   "\n";
 if (webgl2) {
  s+=  "   vec4 tex2 = texture(tex, ex_TexCoord.xy);\n"
    "   vec4 tex_mx = texture(tex, t_mx);	\n"
"   vec4 tex_my = texture(tex, t_my);	\n"
"   vec4 tex_px = texture(tex, t_px);	\n"
    "   vec4 tex_py = texture(tex, t_py);\n";
 } else {
  s+=  "   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
    "   vec4 tex_mx = texture2D(tex, t_mx);	\n"
"   vec4 tex_my = texture2D(tex, t_my);	\n"
"   vec4 tex_px = texture2D(tex, t_px);	\n"
    "   vec4 tex_py = texture2D(tex, t_py);\n";
    }
s+= "   vec4 t1 = mix(tex_mx, tex_px, 0.5);\n"
"   vec4 t2 = mix(tex_my, tex_py, 0.5);\n"
"   vec4 t12 = mix(t1,t2,0.5);\n"
"   vec4 t12t = mix(t12,tex2,0.5);\n"   	
"   return vec4(mix(vec3(rgb),vec3(t12t),t12t.a),t12t.a);\n"
"}\n"
"#endif\n"
"#endif\n"
"\n"
"#ifdef EX_POSITION\n"
"#ifdef EX_NORMAL\n"
"#ifdef REF\n"
"vec4 ref(vec4 rgb)\n"
"{\n"
"   vec3 pos = ex_Position;\n"
"   vec3 norm = ex_Normal;\n"
"   pos = normalize(pos);\n"
"   norm = normalize(norm);\n"
"   vec4 r = vec4(refract(pos, norm, 0.5),1.0);\n"
"   float val = (r.r + r.g + r.b)/3.0;\n"
"   vec4 rr = vec4(val,val,val,1.0);\n"
"   return mix(rgb, rr, 0.5);\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef LIGHT\n"
"vec4 light(vec4 rgb)\n"
"{\n"
"  return clamp(vec4(1.0*rgb.rgb,rgb.a), vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"vec4 wave(vec4 rgb)\n"
"{\n"
"  return rgb;\n"
"}\n"
"#ifdef FADE\n"
"uniform float time_begin_start;\n"
"uniform float time_begin_end;\n"
"uniform float time_end_start;\n"
"uniform float time_end_end;\n"
"vec4 fade(vec4 rgb)\n"
"{\n"
"   float val = 1.0;\n"
"   if (time<time_begin_start) { val = 0.0; }\n"
"   if (time>=time_begin_start && time<time_begin_end) {\n"
"     val = (time-time_begin_start)/(time_begin_end-time_begin_start);\n"
"    }\n"
"   if (time>=time_end_start && time<time_end_end) {\n"
"     val = (time-time_end_start)/(time_end_end-time_end_start);\n"
"     val = 1.0-val;\n"
"   }\n"
"   if (time>=time_end_end) { val=0.0; }\n"
"   rgb.a = rgb.a*val;\n"
"   return rgb;\n"
"}\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"#ifdef TOON\n"
"vec4 toon(vec4 rgb)\n"
"{\n"
"   float intensity;\n"
"   vec4 color;\n"
"   intensity = dot(vec3(3.0,3.0,-3.0), normalize(ex_Normal));\n"
"   if (intensity >0.95)\n"
"      color = vec4(0.2,0.2,0.2, 1.0);\n"
"   else if (intensity >0.5)\n"
"      color = vec4(0.4,0.4,0.4, 1.0);\n"
"   else if (intensity >0.25)\n"
"      color = vec4(0.6,0.6,0.6, 1.0);\n"
"   else\n"
"      color = vec4(1.0,1.0,1.0, 1.0);\n"
"   return mix(rgb,color,0.5);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef MIXSHADER\n"
"uniform float color_mix3;\n"
"vec4 mixshader(vec4 rgb)\n"
"{\n"
"   return vec4(mix(rgb.rgb,vec3(0.0,0.0,0.0),color_mix3),rgb.a);\n"
"}\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef COLOR_MIX\n"
"#ifdef TEXTURE_IMPL\n"


  "vec4 texture_impl(vec4 rgb)\n"
  "{\n";
 if (webgl2) {
   s+=  "   vec4 t = texture(tex, ex_TexCoord.xy);\n";
 } else {
   s+=  "   vec4 t = texture2D(tex, ex_TexCoord.xy);\n";
 }
 s+=   "   return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a);\n"
    //"   return vec4(mix(rgb.rgb, texture2D(tex, ex_TexCoord.xy).rgb, color_mix),1.0);\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef CUBEMAPTEXTURES\n"
"vec4 cubemaptextures(vec4 rgb)\n"
   "{\n";
 if (webgl2) {
   s+=  "  vec4 tex = texture(cubesampler,ex_TexCoord.xyz);\n";
 } else {
   s+=   "  vec4 tex = textureCube(cubesampler,ex_TexCoord.xyz);\n";
 }
s+=    "  return mix(vec4(0.0,0.0,0.0,1.0),rgb, color_mix)+mix(vec4(0.0,0.0,0.0,1.0),tex,color_mix2);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef GLOWEDGE\n"
"uniform float white_level;\n"
"uniform float gray_level;\n"
"uniform float edge_pos;\n"
"vec4 glowedge(vec4 rgb)\n"
"{\n"
"   float u = ex_TexCoord.x;\n"
"   float v = ex_TexCoord.y;\n"
"   if (u>0.5) u = 1.0-u;\n"
"   if (v>0.5) v = 1.0-v;\n"
"   float val = min(u,v);\n"
"   float level;\n"
"   if (val<edge_pos) {\n"
"     val/=edge_pos;\n"
"     level = (1.0-val)*white_level + val*gray_level;\n"
"   } else {\n"
"     level=gray_level;\n"
"   }\n"
"   vec3 color;\n"
"   vec3 wht=vec3(1.0,1.0,1.0);\n"
"   vec3 blk=vec3(0.0,0.0,0.0);\n"
"   if (level>0.5) {\n"
"     level = 1.0-level;\n"
"     level*=2.0;\n"
"     color = (1.0-level)*wht + level*rgb.rgb;\n"
"   } else {\n"
"     level *= 2.0;\n"
"     color = (1.0-level)*rgb.rgb + level*blk;\n"
"   }\n"
"   return vec4(color,1.0);\n"
"}\n"
"#endif\n"
"#endif\n"
    /*
"#ifdef EX_TEXCOORD\n"
"in float object_id;\n"
"vec4 objectids(vec4 rgb)\n"
"{\n"
"    return vec4(object_id, ex_TexCoord.x,ex_TexCoord.y,ex_TexCoord.z);\n"
"}\n"
"#endif\n"
    */
    "#ifdef EX_TEXCOORD\n"
"#ifdef MANYTEXTURES\n"
"vec4 manytextures(vec4 rgb)\n"
  "{\n";
 if (webgl2) {
  s+=  "  if (ex_TexCoord.z<0.7)\n"
"  { vec4 t = texture(texsampler[0],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
    "  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
"  if (ex_TexCoord.z<1.7)\n"
"  { vec4 t = texture(texsampler[1],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[1],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<2.7)\n"
"  { vec4 t = texture(texsampler[2],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[2],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<3.7)\n"
"  { vec4 t = texture(texsampler[3],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[3],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<4.7)\n"
"  { vec4 t = texture(texsampler[4],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[4],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<5.7)\n"
"  { vec4 t = texture(texsampler[5],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[5],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<6.7)\n"
"  { vec4 t = texture(texsampler[6],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[6],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<7.7)\n"
"  { vec4 t = texture(texsampler[7],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[7],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<8.7)\n"
"  { vec4 t = texture(texsampler[8],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[8],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<9.7)\n"
"  { vec4 t = texture(texsampler[9],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[9],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<10.7)\n"
"  { vec4 t = texture(texsampler[10],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[10],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<11.7)\n"
"  { vec4 t = texture(texsampler[11],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[11],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<12.7)\n"
"  { vec4 t = texture(texsampler[12],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[12],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<13.7)\n"
"  { vec4 t = texture(texsampler[13],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[13],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<14.7)\n"
"  { vec4 t = texture(texsampler[14],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
    "  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n";
    //"  return mix(rgb, texture2D(texsampler[14],ex_TexCoord.xy), color_mix);\n";
    } else {
s+=    "  if (ex_TexCoord.z<0.7)\n"
"  { vec4 t = texture2D(texsampler[0],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
    "  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
"  if (ex_TexCoord.z<1.7)\n"
"  { vec4 t = texture2D(texsampler[1],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[1],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<2.7)\n"
"  { vec4 t = texture2D(texsampler[2],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[2],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<3.7)\n"
"  { vec4 t = texture2D(texsampler[3],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[3],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<4.7)\n"
"  { vec4 t = texture2D(texsampler[4],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[4],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<5.7)\n"
"  { vec4 t = texture2D(texsampler[5],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[5],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<6.7)\n"
"  { vec4 t = texture2D(texsampler[6],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[6],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<7.7)\n"
"  { vec4 t = texture2D(texsampler[7],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[7],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<8.7)\n"
"  { vec4 t = texture2D(texsampler[8],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[8],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<9.7)\n"
"  { vec4 t = texture2D(texsampler[9],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[9],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<10.7)\n"
"  { vec4 t = texture2D(texsampler[10],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[10],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<11.7)\n"
"  { vec4 t = texture2D(texsampler[11],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[11],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<12.7)\n"
"  { vec4 t = texture2D(texsampler[12],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[12],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<13.7)\n"
"  { vec4 t = texture2D(texsampler[13],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n"
    //"  return mix(rgb, texture2D(texsampler[13],ex_TexCoord.xy), color_mix);\n"
"  if (ex_TexCoord.z<14.7)\n"
"  { vec4 t = texture2D(texsampler[14],ex_TexCoord.xy);\n"
    //"    if (t.a<0.5) discard;\n"
  "  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a); }\n";
    //"  return mix(rgb, texture2D(texsampler[14],ex_TexCoord.xy), color_mix);\n";
  }

s+=    "}\n"
"#endif\n"
"#endif\n"
"const float GAMMA=2.2;\n"
"const float INV_GAMMA = 1.0/GAMMA;\n"
"vec3 LINEARtoSRGB(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA));\n"
"}\n"
"vec4 SRGBtoLINEAR(vec4 srgbIn)\n"
"{\n"
"  return vec4(pow(srgbIn.xyz,vec3(GAMMA)), srgbIn.w);\n"
"}\n"

"#ifdef EX_COLOR\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef EX_NORMAL\n"
"#ifdef EX_POSITION\n"
"#ifdef COLOR_MIX\n"
"uniform float u_RoughnessFactor;\n"
"uniform float u_MetallicFactor;\n"
"uniform vec4 u_BaseColorFactor;\n"
"uniform float u_OcculsionStrength;\n" 
"uniform float u_NormalScale;\n"
    "uniform float u_EmissiveFactor;\n"
    "uniform vec3 u_EmissiveFactor2;\n"
    "uniform vec3 u_DiffFactor;\n"
"uniform vec3 u_SpecFactor;\n"
"uniform float u_GlossiFactor;\n"
    "vec4 getVertexColor()\n"
"{\n"
"  return ex_Color;\n"
"}\n"
"struct MaterialInfo {\n"
"  float perceptualRoughness;\n"
"  vec3 reflectance0;\n"
"  float alphaRoughness;\n"
"  vec3 diffuseColor;\n"
"  vec3 reflectance90;\n"
"  vec3 specularColor;\n"
"};\n"
"struct AngularInfo {\n"
"  float NdotL;\n"
"  float NdotV;\n"
"  float NdotH;\n"
"  float LdotH;\n"
"  float VdotH;\n"
"  vec3 padding;\n"
"};\n"
"#ifdef GLTF_TEX5\n"
"#ifdef GLTF_TEX6\n"
"#ifdef GLTF_TEX7\n"
"vec3 getIBLContribution(MaterialInfo info, vec3 n, vec3 v)\n"
"{\n"
"   float NdotV = clamp(dot(n,v), 0.0, 1.0);\n"
"   vec3 ref = normalize(reflect(-v,n));\n"
  "   vec2 bfrdsample = clamp(vec2(NdotV, info.perceptualRoughness), vec2(0.0,0.0), vec2(1.0,1.0));\n";
 if (webgl2) {
s+="   vec2 bfrd = texture(texsampler[7], bfrdsample).rg;\n"
    "   vec4 diff = texture(texsampler_cube[5], n);\n"
  "   vec4 spec = texture(texsampler_cube[6], ref);\n";
 } else {
s+="   vec2 bfrd = texture2D(texsampler[7], bfrdsample).rg;\n"
    "   vec4 diff = textureCube(texsampler_cube[5], n);\n"
  "   vec4 spec = textureCube(texsampler_cube[6], ref);\n";
 }
s+="   vec3 diffLight = SRGBtoLINEAR(diff).rgb;\n"
"   vec3 specLight = SRGBtoLINEAR(spec).rgb;\n"
    "   vec3 diffuse = diffLight * info.diffuseColor;\n"
    "   vec3 specular = specLight * (info.specularColor * bfrd.x + bfrd.y);\n"
"   return diffuse+specular;\n"   
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"vec3 ddFdx(vec3 p) {\n"
"return vec3(dFdx(p.x),dFdx(p.y),dFdx(p.z));\n"
    "}\n"
"vec3 ddFdy(vec3 p) {\n"
"return vec3(dFdy(p.x),dFdy(p.y),dFdy(p.z));\n"
"}\n"
    
"vec3 getNormal() {\n"
"   vec2 uv = ex_TexCoord.xy;\n"
    "   vec3 pos_dx = ddFdx(ex_Position);\n"
    "   vec3 pos_dy = ddFdy(ex_Position);\n"
"   vec3 tex_dx = ddFdx(vec3(uv,0.0));\n"
"   vec3 tex_dy = ddFdy(vec3(uv,0.0));\n"
"   vec3 t = (tex_dx.t * pos_dx - tex_dx.t*pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);\n"
"   vec3 ng = normalize(ex_Normal);\n"
"   t = normalize(t-ng*dot(ng,t));\n"
"   vec3 b = normalize(cross(ng,t));\n"
"   mat3 tbn = mat3(t,b,ng);\n"
  "#ifdef GLTF_TEX2\n";
 if (webgl2) {
   s+=  "   vec3 n = texture(texsampler[2], uv).rgb;\n";
 } else {
   s+=  "   vec3 n = texture2D(texsampler[2], uv).rgb;\n";
 }
  s+=  "   n = normalize(tbn*((2.0*n-1.0) * vec3(u_NormalScale,u_NormalScale,1.0)));\n"
"   return n;\n"
"#endif\n"
"   return normalize(ex_Normal);\n"
"}\n"
"AngularInfo getAngularInfo( vec3 pointToLight, vec3 normal, vec3 view) {\n"
"  vec3 n = normalize(normal);\n"
"  vec3 v = normalize(view); \n"
"  vec3 l = normalize(pointToLight); \n"
"  vec3 h = normalize(l+v);\n"
"  float NdotL = clamp(dot(n,l), 0.0, 1.0);\n"
"  float NdotV = clamp(dot(n,v), 0.0, 1.0);\n"
"  float NdotH = clamp(dot(n,h), 0.0, 1.0);\n"
"  float LdotH = clamp(dot(l,h), 0.0, 1.0);\n"
"  float VdotH = clamp(dot(v,h), 0.0, 1.0);\n"
"  return AngularInfo(\n"
"     NdotL, NdotV, NdotH, LdotH, VdotH, vec3(0,0,0) );\n"
"}\n"
"vec3 diffuse(MaterialInfo info) {\n"
"  return info.diffuseColor / 3.14159265;\n"
"}\n"
"vec3 specularReflection(MaterialInfo info, AngularInfo ainfo) {\n"
"  return info.reflectance0 + (info.reflectance90 - info.reflectance0) * pow(clamp(1.0-ainfo.VdotH,0.0,1.0),5.0);\n"
"}\n"
"float visibilityOcclusion(MaterialInfo info, AngularInfo ainfo) {\n"
"  float NdotL = ainfo.NdotL;\n"
"  float NdotV = ainfo.NdotV;\n"
"  float alphaSq = info.alphaRoughness * info.alphaRoughness;\n"
"  float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0-alphaSq) + alphaSq);\n"
"  float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0-alphaSq) + alphaSq);\n"
"  float GGX = GGXV + GGXL;\n"
"  if (GGX > 0.0) { return 0.5 / GGX; }\n"
"  return 0.0;\n"
"}\n"
"float micro(MaterialInfo info, AngularInfo ainfo) {\n"
"  float alphaSq = info.alphaRoughness * info.alphaRoughness;\n"
"  float f = (ainfo.NdotH * alphaSq - ainfo.NdotH) * ainfo.NdotH + 1.0;\n"
"  return alphaSq / (3.14159265 * f * f);\n"
"}\n"
"vec3 getPointShade(vec3 pointToLight, MaterialInfo info, vec3 normal, vec3 view) { \n"
"  AngularInfo ainfo = getAngularInfo(pointToLight, normal, view);\n"
    "  if (ainfo.NdotL > 0.0 || ainfo.NdotV > 0.0) {\n"
"    vec3 F = specularReflection(info, ainfo);\n"
"    float Vis = visibilityOcclusion(info, ainfo);\n"
"    float D = micro(info, ainfo);\n"
"    vec3 diffuseContrib = (1.0-F) * diffuse(info);\n"
"    vec3 specContrib = F * Vis * D;\n"
"    return ainfo.NdotL * (diffuseContrib + specContrib);\n"
    "  }\n"
    "  return vec3(0.0,0.0,0.0);\n"
"}\n"
"uniform float color_mix4;\n"
"uniform vec3 light_dir2;\n"
    "vec4 gltf(vec4 rgb)\n"
"{\n"
"float perceptualRoughness=0.0;\n"
"float metallic=0.0;\n"
"vec4 baseColor=vec4(0.0,0.0,0.0,1.0);\n"
"vec3 diffuseColor=vec3(0);\n"
"vec3 specularColor=vec3(0.0);\n"
"vec3 f0 =vec3(0.04);\n"
"float specAlpha=1.0;\n"
"#ifndef SPEC\n"
    "#ifdef GLTF_TEX1\n";
  if (webgl2) {
    s+=  "  vec4 mrSample = texture(texsampler[1],ex_TexCoord.xy);\n";
  } else {
    s+=  "  vec4 mrSample = texture2D(texsampler[1],ex_TexCoord.xy);\n";
  }
  s+= "  perceptualRoughness = mrSample.g * u_RoughnessFactor;\n"
"  metallic = mrSample.b * u_MetallicFactor;\n"
"#endif\n"
    "#ifndef GLTF_TEX1\n"
    "  metallic = u_MetallicFactor;\n"
    "  perceptualRoughness = u_RoughnessFactor;\n"
    "#endif\n"
    "#ifdef GLTF_TEX0\n";
  if (webgl2) {
    s+=  "  baseColor = SRGBtoLINEAR(texture(texsampler[0],ex_TexCoord.xy)) * u_BaseColorFactor;\n";
  } else {
    s+=  "  baseColor = SRGBtoLINEAR(texture2D(texsampler[0],ex_TexCoord.xy)) * u_BaseColorFactor;\n";
  }
  s+="#endif\n"
    "#endif\n"

    "#ifndef GLTF_TEX0\n"
    " baseColor = u_BaseColorFactor;\n"
    "#endif\n"


"#ifdef SPEC\n"


    "#ifdef GLTF_TEX0\n";
  if (webgl2) {
    s+=  "  baseColor = SRGBtoLINEAR(texture(texsampler[0],ex_TexCoord.xy))* vec4(u_SpecFactor,1.0);\n";
  } else {
    s+=  "  baseColor = SRGBtoLINEAR(texture2D(texsampler[0],ex_TexCoord.xy)) * vec4(u_SpecFactor,1.0);\n";
  }

s+= " perceptualRoughness = 1.0-u_GlossiFactor;\n" 
    
    "#endif\n"

    "#ifndef GLTF_TEX0\n"
    " baseColor = u_BaseColorFactor;\n"
    " perceptualRoughness=1.0-u_GlossiFactor;\n"
    "#endif\n"

    //"baseColor= clamp(baseColor,vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));\n"
    
  "#ifdef GLTF_TEX1\n";
 if (webgl2) {
   s+= "  vec4 mrSample2 = texture(texsampler[1],ex_TexCoord.xy);\n";
 } else {
   s+=  "  vec4 mrSample2 = texture2D(texsampler[1],ex_TexCoord.xy);\n";
 }
  s+=  "mrSample2.r *= u_DiffFactor.r;\n"
    "mrSample2.g *= u_DiffFactor.g;\n"
    "mrSample2.b *= u_DiffFactor.b;\n"

    " diffuseColor = mrSample2.rgb;\n" 
    "f0 = baseColor.rgb;\n"
    "f0.r *= u_SpecFactor.r;\n"
    "f0.g *= u_SpecFactor.g;\n"
    "f0.b *= u_SpecFactor.b;\n"
    "specAlpha = 1.0;\n"
    "#endif\n"
    
    
    "#ifndef GLTF_TEX1\n"
    " diffuseColor = vec3(1,1,1)*(1.0-max(max(baseColor.r,baseColor.g),baseColor.b));\n"
    " f0 = baseColor.rgb;\n"
    "f0.r *= u_SpecFactor.r;\n"
    "f0.g *= u_SpecFactor.g;\n"
    "f0.b *= u_SpecFactor.b;\n"
    " perceptualRoughness = 1.0-u_GlossiFactor;\n" 
    "#endif\n"
    "#endif\n"

    "#ifdef SPEC\n"
    "baseColor.rgb *= u_SpecFactor.rgb;\n"
    "#endif\n"
    
    "#ifndef SPEC\n"
    "  baseColor *= getVertexColor();\n"
    "#endif\n"
"#ifndef SPEC\n"
"  diffuseColor = baseColor.rgb * (vec3(1.0)-f0) * (1.0-metallic);\n"
"#endif\n"

    "#ifndef SPEC\n"
    "  specularColor = mix(f0, baseColor.rgb, metallic);\n"
    "#endif\n"
    "#ifdef SPEC\n"
    "  specularColor.rgb = diffuseColor.rgb*(1.0-max(baseColor.r,max(baseColor.g,baseColor.b)));\n"
    //mix(diffuseColor.rgb,diffuseColor.rgb + f0,u_GlossiFactor);\n"
    "#endif\n"


"  perceptualRoughness = clamp(perceptualRoughness, 0.0, 1.0);\n"
"  metallic = clamp(metallic, 0.0, 1.0);\n"
""
"  float alphaRoughness = perceptualRoughness * perceptualRoughness;\n"
"  float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);\n"
"  vec3 specularEnvironmentR0 = specularColor.rgb;\n"
"  vec3 specularEnvironmentR90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));\n"
"#ifdef SPEC\n"
    "  specularEnvironmentR90=mix(specularColor.rgb,specularEnvironmentR90,u_GlossiFactor);\n"
"#endif\n"
    
    "  MaterialInfo materialInfo = MaterialInfo(\n"
    "    perceptualRoughness,\n"
    "    specularEnvironmentR0,\n"
    "    alphaRoughness,\n"
    "    diffuseColor,\n"
    "    specularEnvironmentR90,\n"
    "    specularColor\n"
    "    );\n"
"    vec3 color = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = getNormal();\n"
    "    vec3 view = vec3(0.0,0.0,1.0);\n"

    // 0.0, 0.0, 400.0
    "  color = getPointShade(light_dir2 /*vec3(0.0,0.0,400.0)*/ /*+ex_Position/100.0*/, materialInfo, normal, view);\n"
    //" color*=baseColor.rgb;\n"
    " color+=baseColor.rgb/4.0;\n"
    //"#ifdef SPEC\n"
    //" color = diffuseColor;\n"
    //"#endif\n"
"#ifdef GLTF_TEX5\n"
"#ifdef GLTF_TEX6\n"
"#ifdef GLTF_TEX7\n"
"  color += getIBLContribution(materialInfo, normal, view);\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef UNLIT\n"
    "color = baseColor.rgb/4.0;\n"
"#endif\n"


    // TODO LIGHTS
"   float ao=1.0;\n"
    //"#ifndef SPEC\n"
    "#ifdef GLTF_TEX3\n";
  if (webgl2) {
    s+="   ao = texture(texsampler[3], ex_TexCoord.xy).r;\n";
  } else {
    s+="   ao = texture2D(texsampler[3], ex_TexCoord.xy).r;\n";
  }
s+=    "   color = mix(color, color*ao, u_OcculsionStrength);\n"
"#endif\n"
    //"#endif\n"
"   vec3 emissive = vec3(0);\n"

    
    //"#ifndef SPEC\n"
  "#ifdef GLTF_TEX4\n";
 if (webgl2) {

   s+="   emissive = SRGBtoLINEAR(texture(texsampler[4], ex_TexCoord.xy)).rgb * u_EmissiveFactor2;\n";
 } else {
   s+="   emissive = SRGBtoLINEAR(texture2D(texsampler[4], ex_TexCoord.xy)).rgb * u_EmissiveFactor2;\n";
 }

s+=   "  vec3 emi = vec3(1.0)-color;\n"
   "  emi*=emissive;\n"
"  color+=emi;\n"

    //    "   color += emissive;\n"
"#endif\n"

"#ifndef GLTF_TEX4\n"
    "  vec3 emi = vec3(1.0)-color;\n"
    "  emi*=u_EmissiveFactor2;\n"
"  color+=emi;\n"
"#endif\n"

    //"#endif\n"
    //"#ifndef GLTF_TEX4\n"
    //"emissive = vec3(1.0,1.0,1.0)*u_EmissiveFactor;\n"
    //"color+=emissive;\n"
    //"#endif\n"
    //"   return vec4(metallic,metallic,metallic,1.0);\n"
    //"   return vec4(vec3(perceptualRoughness),1.0);\n"
    //"   return vec4(vec3(ao),1.0);\n"
    //"   return vec4(vec3(LINEARtoSRGB(emissive),1.0);\n"
    //"   return vec4(vec3(baseColor.a),1.0);\n"

    // ADD BRIGHTNESS TO THE MODELS
    "color*=1.5;\n"
    
    "color = clamp(color,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"

    "#ifdef SPEC\n"
    "   return vec4(mix(rgb.rgb,LINEARtoSRGB(color),color_mix4), specAlpha);\n"
"#endif\n"
"#ifndef SPEC\n"
    "   return vec4(mix(rgb.rgb,LINEARtoSRGB(color),color_mix4), baseColor.a);\n"
"#endif\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef EX_TEXCOORD\n"
"#ifdef TEXTURE_ARRAY\n"
"#ifdef COLOR_MIX\n"
"vec4 texture_arr(vec4 rgb)\n"
  "{\n";
 if (emscripten) {
   s+="   return rgb;\n";
 } else {
   if (!apple) {
     if (webgl2) {
       s+=  "   return mix(rgb, textureArray(texarr, ex_TexCoord), color_mix);\n";
     } else {
       s+="   return mix(rgb, texture2DArray(texarr, ex_TexCoord), color_mix);\n";
     }
   } else { // apple
     s+="   return rgb;\n";
   }
 } // emscripten
s+=
 "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef COLOUR_SHADER\n"
"vec4 colour(vec4 rgb)\n"
"{\n"
"  return mix(rgb, ex_Color, 1.0);    \n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef COLOR_MIX\n"
"#ifdef COLOUR_WITH_MIX\n"
"vec4 colour_with_mix(vec4 rgb)\n"
"{\n"
"  return vec4(mix(rgb.rgb, ex_Color.rgb, color_mix),ex_Color.a);    \n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef COLOR_MIX\n"
"vec4 vertex_phong(vec4 rgb)\n"
"{\n"
"    vec4 c = vec4(mix(rgb.rgb, ex_Color.rgb, color_mix),ex_Color.a);\n"
    "    c=clamp(c,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"   
    "  return c;    \n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 empty(vec4 rgb)\n"
"{\n"
"   return rgb;\n"
"}\n"
"//T:\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
  "//V: screen\n";
 if (webgl2) {
   s+=
     "#version 300 es\n";
 } else {
   s+=  "#version 100\n";
 }
 s+= "precision highp float;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
+ ATTRIBUTE + " vec3 in_Normal;\n"
+ ATTRIBUTE + " vec3 in_Position;\n"
+ VARYING_IN + " vec2 XY;\n"
    "\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  XY = in_Normal.xy;\n"
"  gl_Position = in_P * in_T * in_MV *vec4(in_Position,1.0);\n"
"}\n"
   "//F: screen\n";
 if (webgl2) {
   s+=   "#version 300 es\n";
 } else {
   s+=   "#version 100\n";
 }
s+="precision highp float;\n"
+ VARYING_IN + " vec2 XY;\n"

    "uniform float time;\n"
"uniform float time2;\n"
"uniform float time3;\n"
"uniform float roty;\n"
"uniform sampler2D tex;\n"
"uniform float rotangle;\n"
"uniform vec3 player_pos;\n"
"uniform vec3 target_pos;\n"
"//M:\n"
"\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
"{\n"
"	vec2 iResolution = vec2(1.0,1.0);\n"
"        vec2 pos = fragCoord.xy;\n"
"	pos.x*=800.0/600.0;\n"
"        vec3 p0 = vec3(pos-vec2(0.5),-400.0);\n"
"	vec3 p1 = vec3(pos*2.0-vec2(1.0),-399.0);\n"
"//N:\n"
"	fragColor = rgb;\n"
"}\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  vec4 color;\n"
"  mainImage(color, XY);\n"
"  gl_FragColor = color;\n"
"}\n"
  "//V: colour\n";
 if (webgl2) {
   s+="#version 300 es\n";
 } else {
   s+=   "#version 100\n";
 }
s+="/*layout(location=0)*/ " + ATTRIBUTE + " vec3 in_Position;\n"
"/*layout(location=1)*/ " + ATTRIBUTE + " vec3 in_Normal;\n"
"/*layout(location=2)*/ " + ATTRIBUTE + " vec3 in_Color;\n"
"/*layout(location=3)*/ " + ATTRIBUTE + " vec3 in_TexCoord;\n"
+ VARYING_OUT + " vec3 ex_Color;\n"
    "uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"   ex_Color = in_Color;\n"
"}\n"
  "//F: colour\n";
 if (webgl2) {
   s+=  "#version 300 es\n";
 } else {
   s+=  "#version 100\n";
 }
 s+=   "precision highp float;\n"
+ VARYING_IN + " vec3 ex_Color;\n"
    "//T:\n"
"void main(void)\n"
"{\n"
"   gl_FragColor = vec4(ex_Color,1.0);\n"
"}\n"
"\n"
   "//F: empty\n";
 if (webgl2) {
   s+=    "#version 300 es\n";
 } else {
   s+=   "#version 100\n";
 }
s+=    "precision highp float;\n" 
+ VARYING_IN + " vec3 ex_Color;\n"
    "//T:\n"
"void main(void)\n"
"{\n"
"   gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"\n"
  "//V: texture\n";
 if (webgl2) {
   s+=  "#version 300 es\n";
 } else {
   s+="#version 100\n";
 }
 s+=   "/*layout(location=0)*/ in vec3 in_Position;\n"
"/*layout(location=1)*/ in vec3 in_Normal;\n"
"/*layout(location=2)*/ in vec3 in_Color;\n"
"/*layout(location=3)*/ in vec3 in_TexCoord;\n"
"out vec3 ex_TexCoord;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV *  vec4(in_Position,1.0);\n"
"   ex_TexCoord = in_TexCoord;\n"
"}\n"
   "//F: texture\n";
 if (webgl2) {
   s+=  "#version 300 es\n";
 } else {
   s+="#version 100\n";
 }
 s+=   "//precision highp float;\n"
"uniform sampler2D tex;\n"
+ ATTRIBUTE + " vec3 ex_TexCoord;\n"
    //" vec4 out_Color;\n"
"//T:\n"
"void main(void)\n"
   "{\n";
 if (webgl2) {
   s+=   "   out_Color = texture(tex, ex_TexCoord.xy);\n";
 } else {
   s+=   "   gl_FragColor = texture2D(tex, ex_TexCoord.xy);\n";
 }
 s+=  "}\n";
  } else { // OLDSHADER
    s+="//V: comb\n";
    if (emscripten) {
      s+="#version 300 es\n";
    } else {
    //"#version 330\n"
      s+="#version 460\n";
    }
s+="precision highp float;\n"
    //   "uniform float globe_r;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"#ifdef IN_N\n"
"uniform mat4 in_N;\n"
"#endif\n"
"uniform mat4 in_iMV;\n"

"uniform float in_POS;\n"
"uniform float time;\n"
"#ifdef INST\n"
"in vec3 in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"in mat4 in_InstMat;\n"
"#endif\n"
"in vec3 in_Position;\n"
"in vec3 in_Position2;\n"
"#ifdef IN_NORMAL\n"
"in vec3 in_Normal;\n"
"#endif\n"
"#ifdef IN_COLOR\n"
"in vec4 in_Color;\n"
"#endif\n"
"#ifdef IN_TEXCOORD\n"
"in vec3 in_TexCoord;\n"
"#endif\n"
"#ifdef SKELETON\n"
"in vec4 JOINTS_0;\n"
"in vec4 WEIGHTS_0;\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"out vec3 ex_TexCoord;\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"out vec4 ex_Color;\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"out vec3 ex_Normal;\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"out vec3 ex_Position;\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"out vec3 ex_Normal2;\n"
"#endif\n"
"#ifdef EX_LIGHTPOS2\n"
"out vec3 ex_LightPos2;\n"
"#endif\n"
"#ifdef LIGHTDIR\n"
"in vec3 light_dir;\n"
"#endif\n"
"#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"in vec3 lightpos@;\n"
"#endif\n"
"#endtemplate\n"
"#ifdef SKELETAL\n"
"in int bone_id;\n"
"uniform mat4 bones[50];\n"
"uniform vec3 bone_pos[50];\n"
"#endif\n"
"out float fog_intensity;\n"
"out vec2 shadow_position;\n"
    //"flat out vec4 ex_FlatColor;\n"
"#ifdef VERTEX_LEVELS\n"
"uniform vec4 level1_color;\n"
"uniform vec4 level2_color;\n"
"uniform vec4 level3_color;\n"
"#endif\n"


    "//M:\n"
"//B:\n"
"#ifdef SKELETAL\n"
"vec4 skeletal(vec4 pos)\n"
"{\n"
"    vec3 p = bone_pos[bone_id];\n"
    "    mat4 m = bones[bone_id];\n" // TODO, how to pass mat4 to vertex shader
"    vec4 p2 = pos*m;\n"
"    vec4 p3 = p2 + vec4(p,0.0);\n"
"    return p3;\n"
"}\n"
"#endif\n"

"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 newshadow_1(vec4 pos)\n"
"{\n"
"    ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 newshadow_2(vec4 pos)\n"
"{\n"
"    ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"


"vec4 adjust(vec4 pos)\n"
    "{\n"
    "   return pos;\n"
    "}\n"


"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef BLURRED_RENDER\n"
    
"vec4 blurred_render(vec4 pos)\n"
"{\n"
    " ex_TexCoord = in_TexCoord;\n"
    "return pos;\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"

  
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
    "uniform mat4 in_View;\n"
"uniform vec3 in_PhongLightPos;\n"
    "vec4 phong(vec4 pos)\n"
"{\n"
    "   vec3 n = normalize(mat3(in_View)*mat3(in_iMV)* in_Normal);\n"    
    "    ex_Normal2 = n;\n"
    "    ex_LightPos2 = in_PhongLightPos;\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"


"#ifdef WATER\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
    "vec4 water(vec4 pos)\n"
"{\n"
"    ex_Position = in_Position;\n" //vec3(mat3(in_View2)*mat3(in_iMV)*in_Position);\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n" //mat3(in_View2)*mat3(in_iMV)*(in_Position + in_InstPos);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0)*in_InstMat);\n" //mat3(in_View2)*mat3(in_iMV)* (in_Position * in_InstMat);\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"    

  
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_COLOR\n"
"#ifdef VERTEX_LEVELS\n"

"uniform float hilight;\n"	  

    
"float V_intensity(vec3 dir) {\n"
" float n = dot(normalize(-dir),normalize(vec3(0.0,0.0,-1.0)));\n"
" return n;\n"
"}\n"
"float V_intensity2(vec3 dir) {\n"
" float i=V_intensity(dir); return pow(i,hilight);\n"
"}\n"

"const float GAMMA2=2.2;\n"
"const float INV_GAMMA2 = 1.0/GAMMA2;\n"
"vec3 V_LINEARtoSRGB2(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA2));\n"
"}\n"
"vec3 V_SRGBtoLINEAR2(vec3 srgbIn)\n"
"{\n"
"  return pow(srgbIn.xyz,vec3(GAMMA2));\n"
"}\n"
    
"vec4 vertex_phong(vec4 pos)\n"
"{\n"
    "    vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"    
    //"    ex_Normal2 = n;\n"
    //"    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"

    "    vec3 c = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = n;\n"
    "    c+=V_intensity(normal)*V_SRGBtoLINEAR2(level1_color.rgb);\n"
    "    c+=V_intensity2(normal)*V_SRGBtoLINEAR2(level2_color.rgb);\n"
    //"    c = sqrt(c);\n"
    //"     c+=rgb.rgb;\n"
    //"     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    ex_Color = vec4(c,1.0);\n"
    "    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
"#ifdef SKELETON\n"
    //"uniform mat4 inverseBind[64];\n"
"uniform mat4 jointMatrix[200];\n"
    "mat4 getSkinningMatrix()\n"
"{\n"
    "mat4 skin = mat4(0);\n"
    "skin += \n"
    "    WEIGHTS_0.x * jointMatrix[int(JOINTS_0.x)] +\n"
    "    WEIGHTS_0.y * jointMatrix[int(JOINTS_0.y)] +\n"
    "    WEIGHTS_0.z * jointMatrix[int(JOINTS_0.z)] +\n"
    "    WEIGHTS_0.w * jointMatrix[int(JOINTS_0.w)];\n"
    "return skin;\n"
    "}\n"
"vec4 gltf_anim(vec4 pos)\n"
"{\n"
    "   return (getSkinningMatrix()*pos);\n" 
"}\n"
"#endif\n"

    
"#ifdef EX_NORMAL2\n"
"#ifdef IN_NORMAL\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"out vec3 eye;\n"
"vec4 edge(vec4 pos)\n"
"{\n"
    "   vec3 n = normalize(mat3(in_iMV)*in_Normal);\n"   
    // "    vec3 n = normalize(mat3(inverse(in_MV))*in_Normal);\n"
"    ex_Normal2 = n;\n"
"    ex_Position = in_Position;\n"
    //"    eye = normalize(vec3(in_MV*vec4(in_Position,0.0)) * -1.0 + vec3(inverse(in_P)*vec4(0,0,-1,0.0)));\n"
"    return pos;\n"
"}\n"    
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"uniform float globe_mult;\n"

"vec4 globe(vec4 pos)\n"
"{\n"
"    vec4 pos2 = in_T*in_MV*pos;\n"
"    float d = length(pos2.xz);\n"
    "    float y = globe_mult*d;\n"
        "    y/=2.0;\n"
"    return pos + vec4(0.0,y,0.0,0.0);\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 bump_phong(vec4 pos)\n"
"{\n"
"    ex_Normal2 = normalize(mat3(in_iMV)*in_Normal);\n"
    "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n" //normalize(vec3(in_T*in_MV*vec4(light_dir,1.0)));\n"
"     ex_TexCoord = in_TexCoord;\n"
    "    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"


    
"vec4 fade(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"


"uniform vec3 shadow_center;\n"
"vec4 shadow(vec4 pos)\n"
"{\n"
"  vec4 pp = pos;\n"
"#ifdef INST\n"
"  pp = pp + vec4(in_InstPos,0.0);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  pp = pp * in_InstMat;\n"
"#endif\n"
    //"   pp = in_MV*pp;\n"
"   pp=pp-vec4(shadow_center,0.0);\n"
"   float r = sqrt(pp.x*pp.x+pp.y*pp.y+pp.z*pp.z);\n"
"   float alfa = acos(pp.z/r);\n"
"   float beta = atan(pp.y,pp.x);\n"
"   beta+=3.14159;\n"
"   alfa/=3.14159;\n"
"   beta/=3.14159*2.0;\n"
"   shadow_position = vec2(alfa,beta);\n"
"    return pos;\n"
"}\n"
    "#ifdef EX_POSITION\n"
    "#ifdef IN_POSITION\n"

"vec4 gi(vec4 pos)\n"
"{\n"
" ex_Position = in_Position;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"    

    "#ifdef EX_POSITION\n"
    "#ifdef IN_POSITION\n"

"uniform float fog_dist;\n"
"vec4 fog(vec4 pos)\n"
"{\n"
"    vec4 pp = pos;\n"
"#ifdef INST\n"
"  pp = pp + vec4(in_InstPos,0.0);\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  pp = pp * in_InstMat;\n"
"#endif\n"
"    vec4 p = in_MV*pp;\n"
"    float f = p.z;\n"
   "    f/=fog_dist;\n"
"    f = clamp(f,0.0,1.0);\n"
"    fog_intensity = f;\n"
    //"    ex_Position = in_Position;\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef IN_NORMAL\n"
"#ifdef EX_NORMAL2\n"
    "#ifdef EX_POSITION\n"
    "#ifdef IN_POSITION\n"
    "vec4 dyn_lights(vec4 pos)\n"
    "{\n"
    " ex_Position = in_Position;\n"
"    ex_Normal2 = normalize(mat3(in_iMV)*in_Normal);\n"
    //"#ifdef INST\n"
    //"  ex_Position = in_Position + in_InstPos;\n"
    //"#endif\n"
    "  return pos;\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
    
    "vec4 ambient(vec4 pos)\n"
"{\n"
"    return pos;\n"
"}\n"
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"vec4 diffuse(vec4 pos)\n"
"{\n"

"    ex_Normal2 =normalize(mat3(in_iMV)*in_Normal);\n"
    "    ex_LightPos2 = normalize(vec3(0.0,0.0,-1.0));\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_NORMAL\n"
"#ifdef LIGHTDIR\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"vec4 specular(vec4 pos)\n"
"{\n"
    //    "    ex_Normal2 = normalize(vec3(in_T * in_MV * vec4(in_Normal,0.0)));\n"
"    ex_Normal2 = mat3(in_iMV)*in_Normal;\n"
"    ex_LightPos2 = normalize(vec3(in_T*in_MV*vec4(light_dir,1.0)));\n"
"    return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
"#endif\n"
"#ifdef IN_N\n"
"#ifdef IN_NORMAL\n"
"#ifdef EX_NORMAL\n"
"vec4 recalc_normal(vec4 pos)\n"
"{\n"
    //"   mat4 inN = transpose(inverse(in_MV));\n"
"   ex_Normal = normalize((vec4(in_Normal,1.0)*in_N).xyz);\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"vec4 flat_shading(vec4 pos)\n"
"{\n"
    //"   ex_FlatColor = in_Color;\n"
"   return pos;\n"
"}\n"
"#ifdef INST\n"
"vec4 inst(vec4 pos)\n"
"{\n"
"   return pos + vec4(in_InstPos,0.0);\n"
"}\n"
"#endif\n"
"#ifdef INSTMAT\n"
"vec4 instmat(vec4 pos)\n"
"{\n"
"   return pos * in_InstMat;\n"
"}\n"
"#endif\n"

"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 passall(vec4 pos)\n"
"{\n"
"  ex_Color = in_Color;\n"
"  ex_TexCoord = in_TexCoord;\n"
"  ex_Normal = in_Normal;\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_POSITION\n"
"vec4 passpos(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"vec4 point_light(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"

"vec4 coloured_lights(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
    //"#ifdef INST\n"
    //"  ex_Position = in_Position + in_InstPos;\n"
    //"#endif\n"
    //"#ifdef INSTMAT\n"
    //"  ex_Position = in_Position * in_InstMat;\n"
    //"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"

    
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"vec4 snoise(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 light(vec4 pos)\n"
"{\n"
"return pos;\n"
"}\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"vec4 ref(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = in_Position * in_InstMat;\n"
"#endif\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef IN_POSITION\n"
"vec4 wave(vec4 pos)\n"
"{\n"
"  vec4 pos2 = pos + vec4(10.0*cos(2.0*time*in_Position.x*3.14159*2.0/360.0),\n"
"                         10.0*sin(3.0*time*in_Position.y*3.14159*2.0/360.0),\n"
"			 10.0*cos(5.0*time*in_Position.z*3.14159*2.0/360.0), 0.0);\n"
"  return pos2;\n"
"}\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"
"vec4 toon(vec4 pos)\n"
"{\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 texture_impl(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 mixshader(vec4 pos)\n"
"{ return pos; }\n"    
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 glowedge(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
    
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 manytextures(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 cubemaptextures(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 texture_arr(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"vec4 colour(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef IN_COLOR\n"
"vec4 colour_with_mix(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_TEXCOORD\n"
"vec4 blur(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 empty(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"
"#ifdef IN_TEXCOORD\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef IN_COLOR\n"
"#ifdef EX_COLOR\n"
"#ifdef EX_POSITION\n"
"#ifdef IN_POSITION\n"
"#ifdef EX_NORMAL\n"
"#ifdef IN_NORMAL\n"

"vec4 gltf(vec4 pos)\n"
"{\n"
    "   ex_Position = pos.xyz /*in_Position*/;\n"
"#ifdef INST\n"
"  ex_Position = in_Position + in_InstPos;\n"
"#endif\n"
"#ifdef INSTMAT\n"
"  ex_Position = vec3(vec4(in_Position,0.0) * in_InstMat);\n"
"#endif\n"
"  ex_Normal = normalize(mat3(in_iMV)*in_Normal);\n"
"   ex_Color = in_Color;\n"
"   ex_TexCoord = in_TexCoord;\n"
"   return pos;\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"//T:\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"\n"
  "//F: comb\n";
 if (emscripten) {
   s+="#version 300 es\n";
 } else {
    //"#version 330\n"
   s+=   "#version 460\n";
 }
s+="#ifdef CUBEMAPTEXTURES\n"
"#extension GL_NV_shadow_samplers_cube : enable\n"
"#endif\n"
"#ifdef GLTF\n"
    //    "#extension GL_OES_standard_derivatives : enable\n"
"#extension GL_NV_shadow_samplers_cube : enable\n"
"#endif\n"
"#ifdef TEXTURE_ARRAY\n"
"#extension GL_EXT_texture_array : enable\n"
"#endif\n"
"precision highp float;\n"
"in vec4 ex_Color;\n"
    //"flat in vec4 ex_FlatColor;\n"
"out vec4 out_Color;\n"
"#ifdef EX_TEXCOORD\n"
"in vec3 ex_TexCoord;\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"in vec3 ex_Normal;\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"in vec3 ex_Position;\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"in vec3 ex_Normal2;\n"
"#endif\n"
"#ifdef EX_LIGHTPOS2\n"
"in vec3 ex_LightPos2;\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"uniform vec4 color_array[10];\n"
"#endif\n"
"uniform sampler2D tex;\n"
"#ifdef TEXTURE_ARRAY\n"
"uniform sampler2DArray texarr;\n"
"#endif\n"
"#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"uniform vec3 lightpos@;\n"
"#endif\n"
"#endtemplate\n"
"#ifdef SPECULAR_SIZE\n"
"uniform float specular_size;\n"
"#endif\n"
"#ifdef COLOR_MIX\n"
"uniform float color_mix;\n"
"uniform float color_mix2;\n"
"#endif\n"
    //"uniform vec3 light_dir;\n"
"#ifdef LEVELS\n"
    "uniform vec4 level1_color;\n"
"uniform vec4 level2_color;\n"
"uniform vec4 level3_color;\n"
"#endif\n"
"uniform float hilight;\n"	  
    
"uniform float time;\n"
"#ifdef MANYTEXTURES\n"
"uniform sampler2D texsampler[15];\n"
"#endif\n"
"#ifdef NEWSHADOW\n"
"uniform sampler2D texsampler[1];\n"
"#endif\n"

"#ifdef GLTF\n"
"uniform sampler2D texsampler[15];\n"
"uniform samplerCube texsampler_cube[15];\n"
"#endif\n"
"#ifdef BUMPPHONG\n"
"uniform sampler2D texsampler[15];\n"
"#endif\n"
"#ifdef CUBEMAPTEXTURES\n"
"uniform samplerCube cubesampler;\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"uniform vec4 c_positions[8];\n"
"uniform float c_scale[8];\n"
"uniform vec4 c_color[8];\n"
"#endif\n"
    
    //"//M:\n"


"#ifdef WATER\n"
"#ifdef EX_POSITION\n"
"uniform vec3 water_center;\n"
"uniform float wave_mult;\n"
"uniform float wave_time;\n"
"uniform vec4 water_color1;\n"
"uniform vec4 water_color2;\n"
"uniform vec4 water_color3;\n"
"vec4 water(vec4 rgb)\n"
"{\n"
"    float d = length(ex_Position-water_center);\n"
"    float s = sin(d*wave_mult+wave_time);\n"
"    s/=2.0; s+=0.5;\n"
   "    if (s<0.5) { return mix(water_color1,water_color2,s*2.0); }\n"
"    return mix(water_color2,water_color3,(s-0.5)*2.0);\n"
"}\n"
"#endif\n"
"#endif\n"

  
"#ifdef CHOOSE_COLOR\n"
"uniform vec4 color_choice;\n"
"uniform float mix_val;\n"
"vec4 choose_color(vec4 rgb)\n"
"{\n"
"    return vec4(mix(rgb.rgb,color_choice.rgb,mix_val),rgb.a);\n"
"}\n"
"#endif\n"
"//M:\n"
"//B:\n"
"vec4 white(vec4 rgb)\n"
"{\n"
"   return vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"vec4 diffuse(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = ex_Normal2;\n"
"    vec3 light_dir_cam = ex_LightPos2;\n"
"    float theta = clamp(dot(  normalize(-normal_cam), normalize(light_dir_cam) ) , 0.0,1.0);\n"
"    return clamp(rgb + level1_color*theta,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef LEVELS\n"
"vec4 ambient(vec4 rgb)\n"
"{\n"
"    return clamp(rgb + level2_color*vec4(0.2,0.2,0.2,1.0)*0.8,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef SPECULAR_SIZE\n"
"#ifdef LEVELS\n"
"vec4 specular(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = ex_Normal2;\n"
"    vec3 light_dir_cam = ex_LightPos2;\n"
"    vec3 R = reflect(-light_dir_cam, normalize(normal_cam));\n"
"    vec3 E = vec3(0.0,0.0,0.5);\n"
"    float alpha = abs(dot(E,R));\n"
"    float p = pow(alpha,specular_size);\n"
"    float p2 = clamp(p, 0.0, 1.0);\n"
"    p2 = 1.0-p2;\n"
"    return clamp(rgb + level3_color * p2 * length(rgb),vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_0(vec4 rgb)\n"
"{\n"
"   return color_array[0];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_1(vec4 rgb)\n"
"{\n"
"   return color_array[1];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_2(vec4 rgb)\n"
"{\n"
"   return color_array[2];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_3(vec4 rgb)\n"
"{\n"
"   return color_array[3];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_4(vec4 rgb)\n"
"{\n"
"   return color_array[4];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_5(vec4 rgb)\n"
"{\n"
"   return color_array[5];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_6(vec4 rgb)\n"
"{\n"
"   return color_array[6];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_7(vec4 rgb)\n"
"{\n"
"   return color_array[7];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_8(vec4 rgb)\n"
"{\n"
"   return color_array[8];\n"
"}\n"
"#endif\n"
"#ifdef COLOR_ARRAY\n"
"vec4 color_from_id_9(vec4 rgb)\n"
"{\n"
"   return color_array[9];\n"
"}\n"
"#endif\n"
"#ifdef EX_NORMAL\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
"#endif\n"
"vec4 flat_shading(vec4 rgb)\n"
"{\n"
    //"   return mix(rgb,ex_FlatColor,0.5);\n"
"   return rgb;\n"
"}\n"
"#ifdef EX_POSITION\n"
"#template LIGHTPOS\n"
"#ifdef LIGHTPOS\n"
"vec4 point_light@(vec4 rgb)"
"{\n"
"   float dist = length(ex_Position.xyz - lightpos@);\n"
"   float val = 100.0/dist;\n"
"   return vec4(val*vec3(rgb)+vec3(rgb),rgb.a);\n"
"}\n"
"#endif\n"
"#endtemplate\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"vec4 coloured_lights(vec4 rgb)\n"
"{\n"
"   vec3 res=vec3(0.5,0.5,0.5);\n"
    //"   for(int i=0;i<8;i++) {\n"
    //"     vec3 pos = coloured_positions[i];\n"
    //"     vec3 delta = ex_Position.xyz-pos;\n"
    //"     float dist = sqrt(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);\n"
    //"     float d2 = coloured_scale[i]*dist;\n"
    //"     float db = d2*d2;\n"
    //"     if (db<1.0) { db=1.0; }\n"
    //"     float inv = 1.0/db;\n"
    "     vec3 cl = c_color[0].rgb;\n"
"     res+=cl;\n"
    //"   }\n"
"   return vec4(clamp(res,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0)),rgb.a);\n"
"}\n"
"#endif\n"
"vec4 bands(vec4 rgb)\n"
"{\n"
"   float r = rgb.r;\n"
"   float g = rgb.g;\n"
"   float b = rgb.b;\n"
"   float a = rgb.a;\n"
"   int rr = int(r*256)&0xfff0;\n"
"   int gg = int(g*256)&0xfff0;\n"
"   int bb = int(b*256)&0xfff0;\n"
"   int aa = int(a*256)&0xfff0;\n"
"   float rrr = float(rr)/256.0;\n"
"   float ggg = float(gg)/256.0;\n"
"   float bbb = float(bb)/256.0;\n"
"   float aaa = float(aa)/256.0;\n"
"   vec4 rgb2 = vec4(rrr,ggg,bbb,aaa);\n"
"   return rgb2;\n"
"}\n"
"#ifdef EX_TEXCOORD\n"
"vec4 dot2(vec4 rgb)\n"
"{\n"
"   vec3 pos = vec3(ex_TexCoord.xy,1.0);\n"
"   //xpos = normalize(pos);\n"
"   return vec4(pos, 1.0);\n"
"}\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"vec4 snoise(vec4 rgb)\n"
"{\n"
"   vec2 coords = ex_Position.xy;\n"
"   float v = fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"   return vec4(mix(vec3(rgb), vec3(v,v,v), 0.2),rgb.a);\n"
"}\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"vec4 blur(vec4 rgb)\n"
"{\n"
"  vec2 t_mx = ex_TexCoord.xy + vec2(-0.005,0.0);\n"
"  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-0.005);\n"
"  vec2 t_px = ex_TexCoord.xy + vec2(0.005,0.0);\n"
"  vec2 t_py = ex_TexCoord.xy + vec2(0.0,0.005);\n"
  "\n";
 if (webgl2) {
 s+=   "   vec4 tex2 = texture(tex, ex_TexCoord.xy);\n"
    "   vec4 tex_mx = texture(tex, t_mx);	\n"
"   vec4 tex_my = texture(tex, t_my);	\n"
"   vec4 tex_px = texture(tex, t_px);	\n"
   "   vec4 tex_py = texture(tex, t_py);\n";
 } else {
s+=    "   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
    "   vec4 tex_mx = texture2D(tex, t_mx);	\n"
"   vec4 tex_my = texture2D(tex, t_my);	\n"
"   vec4 tex_px = texture2D(tex, t_px);	\n"
  "   vec4 tex_py = texture2D(tex, t_py);\n";
 }

s+="   vec4 t1 = mix(tex_mx, tex_px, 0.5);\n"
"   vec4 t2 = mix(tex_my, tex_py, 0.5);\n"
"   vec4 t12 = mix(t1,t2,0.5);\n"
"   vec4 t12t = mix(t12,tex2,0.5);\n"   	
"   return vec4(mix(vec3(rgb),vec3(t12t),t12t.a),t12t.a);\n"
"}\n"
"#endif\n"
"\n"
"#ifdef EX_POSITION\n"
"#ifdef EX_NORMAL\n"
"vec4 ref(vec4 rgb)\n"
"{\n"
"   vec3 pos = ex_Position;\n"
"   vec3 norm = ex_Normal;\n"
"   pos = normalize(pos);\n"
"   norm = normalize(norm);\n"
"   vec4 r = vec4(refract(pos, norm, 0.5),1.0);\n"
"   float val = (r.r + r.g + r.b)/3.0;\n"
"   vec4 rr = vec4(val,val,val,1.0);\n"
"   return mix(rgb, rr, 0.5);\n"
"}\n"
"#endif\n"
"#endif\n"
"vec4 light(vec4 rgb)\n"
"{\n"
"  return clamp(vec4(1.0*rgb.rgb,rgb.a), vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"vec4 wave(vec4 rgb)\n"
"{\n"
"  return rgb;\n"
"}\n"
"uniform float time_begin_start;\n"
"uniform float time_begin_end;\n"
"uniform float time_end_start;\n"
"uniform float time_end_end;\n"
"vec4 fade(vec4 rgb)\n"
"{\n"
"   float val = 1.0;\n"
"   if (time<time_begin_start) { val = 0.0; }\n"
"   if (time>=time_begin_start && time<time_begin_end) {\n"
"     val = (time-time_begin_start)/(time_begin_end-time_begin_start);\n"
"    }\n"
"   if (time>=time_end_start && time<time_end_end) {\n"
"     val = (time-time_end_start)/(time_end_end-time_end_start);\n"
"     val = 1.0-val;\n"
"   }\n"
"   if (time>=time_end_end) { val=0.0; }\n"
"   rgb.a = rgb.a*val;\n"
"   return rgb;\n"
"}\n"
"#ifdef EX_NORMAL\n"
"vec4 toon(vec4 rgb)\n"
"{\n"
"   float intensity;\n"
"   vec4 color;\n"
"   intensity = dot(vec3(3.0,3.0,-3.0), normalize(ex_Normal));\n"
"   if (intensity >0.95)\n"
"      color = vec4(0.2,0.2,0.2, 1.0);\n"
"   else if (intensity >0.5)\n"
"      color = vec4(0.4,0.4,0.4, 1.0);\n"
"   else if (intensity >0.25)\n"
"      color = vec4(0.6,0.6,0.6, 1.0);\n"
"   else\n"
"      color = vec4(1.0,1.0,1.0, 1.0);\n"
"   return mix(rgb,color,0.5);\n"
"}\n"
"#endif\n"
"uniform float color_mix3;\n"
"vec4 mixshader(vec4 rgb)\n"
"{\n"
"   return vec4(mix(rgb.rgb,vec3(0.0,0.0,0.0),color_mix3),rgb.a);\n"
"}\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef COLOR_MIX\n"


  "vec4 texture_impl(vec4 rgb)\n"
"{\n"
"   vec4 t = texture2D(tex, ex_TexCoord.xy);\n"
"   return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a);\n"
    //"   return mix(rgb, texture2D(tex, ex_TexCoord.xy), color_mix);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_TEXCOORD\n"
"uniform float white_level;\n"
"uniform float gray_level;\n"
"uniform float edge_pos;\n"
"vec4 glowedge(vec4 rgb)\n"
"{\n"
"   float u = ex_TexCoord.x;\n"
"   float v = ex_TexCoord.y;\n"
"   if (u>0.5) u = 1.0-u;\n"
"   if (v>0.5) v = 1.0-v;\n"
"   float val = min(u,v);\n"
"   float level;\n"
"   if (val<edge_pos) {\n"
"     val/=edge_pos;\n"
"     level = (1.0-val)*white_level + val*gray_level;\n"
"   } else {\n"
"     level=gray_level;\n"
"   }\n"
"   vec3 color;\n"
"   vec3 wht=vec3(1.0,1.0,1.0);\n"
"   vec3 blk=vec3(0.0,0.0,0.0);\n"
"   if (level>0.5) {\n"
"     level = 1.0-level;\n"
"     level*=2.0;\n"
"     color = (1.0-level)*wht + level*rgb.rgb;\n"
"   } else {\n"
"     level *= 2.0;\n"
"     color = (1.0-level)*rgb.rgb + level*blk;\n"
"   }\n"
"   return vec4(color,1.0);\n"
"}\n"
"#endif\n"
    
"#ifdef EX_TEXCOORD\n"
"#ifdef MANYTEXTURES\n"
"vec4 manytextures(vec4 rgb)\n"
"{\n"
"  vec4 t = texture2D(texsampler[int(ex_TexCoord.z)],ex_TexCoord.xy);\n"
"  return vec4(mix(rgb.rgb, t.rgb, color_mix),t.a);\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef EX_TEXCOORD\n"
"#ifdef CUBEMAPTEXTURES\n"
"vec4 cubemaptextures(vec4 rgb)\n"
  "{\n";
 if (webgl2) {

   s+=   "  vec4 tex = texture(cubesampler,ex_TexCoord.xyz);\n";
 } else {
   s+=  "  vec4 tex = textureCube(cubesampler,ex_TexCoord.xyz);\n";
 }
s+=    "   return vec4(mix(vec3(0.0,0.0,0.0),rgb.rgb,color_mix)+mix(vec3(0.0,0.0,0.0),tex.rgb,color_mix2),1.0);\n"
    //"  return mix(vec4(0.0,0.0,0.0,1.0),rgb, color_mix)+mix(vec4(0.0,0.0,0.0,1.0),tex,color_mix2);\n"
"}\n"
"#endif\n"
"#endif\n"

"#ifdef EX_TEXCOORD\n"
"#ifdef TEXTURE_ARRAY\n"
"#ifdef COLOR_MIX\n"
"vec4 texture_arr(vec4 rgb)\n"
"{\n"
"   return mix(rgb, texture2DArray(texarr, ex_TexCoord), color_mix);\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"uniform vec3 in_dx;\n"
"uniform vec3 in_dy;\n"
"uniform vec3 in_pp;\n"
"uniform float in_scale;\n"
"uniform mat4 in_Mat;\n"
"float find_projection_length(vec3 u, vec3 u_x) {\n"
"   float dd = dot(u,u_x)/dot(u_x,u_x);\n"
"   vec3 proj = (dd)*u_x;\n"
"   float len = length(proj);\n"
"   len/=length(u_x);\n"
"   if (dd<0.0) len=-len;\n"
"   return len;\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
  "vec4 newshadow_1(vec4 rgb)\n"
"{\n"
"    vec3 pos = ex_Position;\n"
"    float tx_x = find_projection_length(pos,in_dx)/in_scale;\n"
"    float tx_y = find_projection_length(pos,in_dy)/in_scale;\n"
  //"    vec2 tx = vec2(tx_x,tx_y);\n"
"    float dist = length(pos - (in_pp+tx_x*in_dx+tx_y*in_dy));\n"
    //#ifdef WEBGL2
    //"    texture(in_txid,tx).r = min(texture(in_txid,tx).r,dist);\n"
    //#else
    //"    texture2D(in_txid,tx).r = min(texture2D(in_txid,tx).r,dist);\n"
    //#endif
    "   dist=clamp(dist,0.0,800.0);\n"
"    return vec4(dist/800.0,dist/800.0,dist/800.0,1.0);\n"
"}\n"
"#endif\n"
"#endif\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"uniform float in_darklevel;\n"
"uniform float in_lightlevel;\n"
"uniform int texindex;\n"

"float newshadow_level()\n"
"{\n"
"    vec3 pos = ex_Position;\n"
    //"    vec3 pos2 = vec3(in_Mat * vec4(pos,0.0));\n"
"    float tx_x = find_projection_length(pos,in_dx)/in_scale;\n"
"    float tx_y = find_projection_length(pos,in_dy)/in_scale;\n"
    //    "    tx_x=mod((tx_x+1.0),2.0)-1.0;\n" //clamp(tx_x,-1.0,1.0);\n"
    //  "    tx_y=mod((tx_y+1.0),2.0)-1.0;\n" //clamp(tx_y,-1.0,1.0);\n"
    //"    tx_x=clamp(tx_x,-1.0,1.0);\n"
    //"    tx_y=clamp(tx_y,-1.0,1.0);\n"
    //"    vec2 tx = vec2(tx_x/2.0+1.0,tx_y/2.0+1.0);\n"
    "vec2 tx=vec2(1.0-((tx_x+1.0)/2.0),(tx_y+1.0)/2.0);\n"
  "    tx=clamp(tx,vec2(0.0,0.0),vec2(1.0,1.0));\n";
 if (webgl2) {
   s+= "    float d;\n"
    "if (texindex==0)\n"
     "       d = texture(texsampler[0] /*in_txid*/,tx).r*800.0;\n";
   if (0) {
   s+= "if (texindex==1)\n"
    "       d = texture(texsampler[1] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==2)\n"
    "       d = texture(texsampler[2] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==3)\n"
    "       d = texture(texsampler[3] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==4)\n"
    "       d = texture(texsampler[4] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==5)\n"
    "       d = texture(texsampler[5] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==6)\n"
    "       d = texture(texsampler[6] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==7)\n"
    "       d = texture(texsampler[7] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==8)\n"
    "       d = texture(texsampler[8] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==9)\n"
    "       d = texture(texsampler[9] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==10)\n"
    "       d = texture(texsampler[10] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==11)\n"
    "       d = texture(texsampler[11] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==12)\n"
    "       d = texture(texsampler[12] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==13)\n"
    "       d = texture(texsampler[13] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==14)\n"
    "       d = texture(texsampler[14] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==15)\n"
    "       d = texture(texsampler[15] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==16)\n"
     "       d = texture(texsampler[16] /*in_txid*/,tx).r*800.0;\n";
   }
 } else { // !webgl2
 
    s+="    float d;\n"
    "if (texindex==0)\n"
      " d= texture2D(texsampler[0] /*in_txid*/,tx).r*800.0;\n";
    if (0) {
   s+= "if (texindex==1)\n"
    " d= texture2D(texsampler[1] /*in_txid*/,tx).r*800.0;\n"

    "if (texindex==2)\n"
    " d= texture2D(texsampler[2] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==3)\n"
    " d= texture2D(texsampler[3] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==4)\n"
    " d= texture2D(texsampler[4] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==5)\n"
    " d= texture2D(texsampler[5] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==6)\n"
    " d= texture2D(texsampler[6] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==7)\n"
    " d= texture2D(texsampler[7] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==8)\n"
    " d= texture2D(texsampler[8] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==9)\n"
    " d= texture2D(texsampler[9] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==10)\n"
    " d= texture2D(texsampler[10] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==11)\n"
    " d= texture2D(texsampler[11] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==12)\n"
    " d= texture2D(texsampler[12] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==13)\n"
    " d= texture2D(texsampler[13] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==14)\n"
    " d= texture2D(texsampler[14] /*in_txid*/,tx).r*800.0;\n"
    "if (texindex==15)\n"
     " d= texture2D(texsampler[15] /*in_txid*/,tx).r*800.0;\n";
    }
 }

 s+="    float d2 = length(pos - (in_pp + tx_x*(in_dx) + tx_y*(in_dy)));\n"
    "   d2=clamp(d2,0.0,800.0);\n"
"    bool shade = d2 < d;\n"
"    float level;\n"
"    if (shade) { level=in_darklevel; }\n"
"    else { level=in_lightlevel; }\n"
" return level;\n"
    // " float dd = abs(d-d2)/800.0;\n"
    //" dd = clamp(dd,-1.0,1.0);\n"
    //"    return vec4(d/800.0,d2/800.0,0.0/800.0,1.0);\n"
    //    //"  return vec4(0.0,0.0/800.0,d/800.0,1.0);\n"
    //"return vec4(tx.x,0.0,0.0,1.0);\n"
    //"return vec4(tx.x,tx.y,0.0,1.0);\n"
    //   "return vec4(d/800.0,dd/800.0,0.0,1.0);\n"
    //          "    return vec4((level)*rgb.rgb,1.0);\n"
    //" return rgb;\n"
"}\n"
"vec4 newshadow_2(vec4 rgb) {\n"
"   float level = newshadow_level();\n"
"   return vec4(level*rgb.rgb,1.0);\n"
"}\n"
"#endif\n"
"#endif\n"


"uniform vec4 ad_color;\n"
"uniform float ad_light;\n"
"uniform float ad_dark;\n"
"vec4 adjust(vec4 rgb)\n"
   "{\n"
   "   vec3 cc = ad_color.rgb*rgb.rgb;\n"
   "   cc *= (ad_light-ad_dark);\n"
   "   cc += vec3(ad_dark,ad_dark,ad_dark);\n"
   "   return vec4(cc,rgb.a);\n"
   "}\n"

   
"#ifdef EX_TEXCOORD\n"
"#ifdef COLOR_MIX\n"
"#ifdef BLURRED_RENDER\n"
"uniform int blur_num_samples;\n"
"uniform float blur_radius;\n"


"const float GAMMA3=2.2;\n"
"const float INV_GAMMA3 = 1.0/GAMMA3;\n"
"vec4 V_LINEARtoSRGB3(vec4 color)\n"
"{\n"
" return vec4(pow(color.xyz, vec3(INV_GAMMA3)),color.a);\n"
"}\n"
"vec4 V_SRGBtoLINEAR3(vec4 srgbIn)\n"
"{\n"
"  return vec4(pow(srgbIn.xyz,vec3(GAMMA3)),srgbIn.a);\n"
"}\n"

   "vec4 blurred_render(vec4 rgb)\n"
"{\n"
" vec4 t2=vec4(0.0,0.0,0.0,0.0);\n"
"  for(int i=0;i<blur_num_samples;i++)\n"
" {\n"
 " vec2 delta = vec2(blur_radius*cos(i*3.14159265*2.0/blur_num_samples),"
   "		     blur_radius*sin(i*3.14159265*2.0/blur_num_samples));";
   
 if (webgl2) {
   s+=  "   vec4 t = V_SRGBtoLINEAR3(texture(tex, ex_TexCoord.xy+delta));\n";
 } else {
   s+=  "   vec4 t = V_SRGBtoLINEAR3(texture2D(tex, ex_TexCoord.xy+delta));\n";
 }
s+=""
  //" t2=max(t2,t);\n"
    " t2+=t;\n"
"}\n"
  " t2/=blur_num_samples;\n"
  "   return V_LINEARtoSRGB3(vec4(mix(rgb.rgb, t2.rgb, color_mix),t2.a));\n"
  //" return t2;
  "}\n"

"#endif\n"
"#endif\n"
"#endif\n"


   
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"

"float intensity(vec3 dir) {\n"
" float n = clamp(dot(normalize(-dir),normalize(ex_LightPos2)),0.2,1.0);\n"
" return n;\n"
"}\n"
"float intensity2(vec3 dir) {\n"
" float i=intensity(dir); return pow(i,hilight);\n"
"}\n"
"vec3 normalmixp(vec3 a, vec3 b) {\n"
"float angle1 = acos(a.z)+acos(b.z);\n"
"float angle2 = atan(a.y,a.x)+atan(b.y,b.x);\n"
"return vec3(sin(angle1)*cos(angle2),sin(angle1)*sin(angle2),cos(angle1));\n"
"}\n"
"const float GAMMA2=2.2;\n"
"const float INV_GAMMA2 = 1.0/GAMMA2;\n"
"vec3 LINEARtoSRGB2(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA2));\n"
"}\n"
"vec3 SRGBtoLINEAR2(vec3 srgbIn)\n"
"{\n"
"  return pow(srgbIn.xyz,vec3(GAMMA2));\n"
"}\n"

"vec4 phong(vec4 rgb)\n"
    "{\n"
    "    vec3 c = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = ex_Normal2;\n"
    "    c+=SRGBtoLINEAR2(rgb.rgb);\n"
"#ifdef PHONG_TEXTURE\n"
    "    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb*rgb.rgb,vec3(1,1,1),0.15));\n"
    "    c=mix(c,SRGBtoLINEAR2(level2_color.rgb),intensity2(normal));\n"
"#endif\n"
"#ifndef PHONG_TEXTURE\n"
    "    c+=intensity(normal)*SRGBtoLINEAR2(mix(level1_color.rgb,vec3(1,1,1),0.1));\n"
    "    c=mix(c,SRGBtoLINEAR2(level2_color.rgb),intensity2(normal));\n"
"#endif\n"
    
    //"    c = sqrt(c);\n"
    //"     c*=rgb.rgb;\n"
    "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    return vec4(LINEARtoSRGB2(c),rgb.a);\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"

"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"vec4 shadowphong(vec4 rgb)\n"
"{\n"
"   vec4 rgb2 = phong(rgb);\n"
"   float level = newshadow_level();\n"
"   if (level>0.5) {\n"
"     float level2 = (level-0.5)*2.0;\n"
"     vec4 res = vec4((1.0-level2)*rgb.rgb + level2*rgb2.rgb,1.0);\n"
"     return res;\n"
"   } else {\n"
"     vec4 res = vec4(rgb.rgb*(level*2.0),1.0);\n"
"     return res;\n"
"   }\n"
"}\n"

"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"

   

   "#ifdef EX_COLOR\n"
"#ifdef COLOR_MIX\n"
"vec4 vertex_phong(vec4 rgb)\n"
"{\n"
"    vec4 c = vec4(mix(rgb.rgb, ex_Color.rgb, color_mix),ex_Color.a);\n"
    "    c=clamp(c,vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"   
    "  return c;    \n"

    //"  return vec4(mix(rgb.rgb, ex_Color.rgb, color_mix),ex_Color.a);    \n"
    //"return vec4(1.0,0.0,0.0,1.0);\n"
    "}\n"
"#endif\n"
"#endif\n"
"#ifdef SKELETON\n"
"vec4 gltf_anim(vec4 rgb)\n"
"    {\n"
"     return rgb;\n"
"    }\n"
"#endif\n"

    "#ifdef EX_NORMAL2\n"
"#ifdef EX_POSITION\n"
"uniform float edge_width;\n"
"uniform vec4 edge_color;\n"
"in vec3 eye;\n"
"vec4 edge(vec4 rgb)\n"
"{\n"
    "    vec3 view = vec3(0.0,0.0,-400.0);\n"
"    vec3 n = ex_Normal2;\n"
"    float d = 1.0-dot(normalize(view),normalize(n));\n"
"    float val = smoothstep(edge_width-0.01,edge_width+0.01,d);\n"
"    vec4 color = rgb;\n"
"   color = val*edge_color;\n"
"    return color;\n"
"}\n"
"#endif\n"
"#endif\n"

    
"#ifdef EX_NORMAL2\n"
"#ifdef EX_LIGHTPOS2\n"
"#ifdef LEVELS\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef BUMPPHONG\n"

    //"uniform float hilight;\n"	  
    //"float intensity(vec3 dir) {\n"
    //" return dot(dir,ex_LightPos2);\n"
    //"}\n"
// "float intensity2(vec3 dir) {\n"
// " float i=intensity(dir); return pow(i,hilight);\n"
// "}\n"
"float intensityb(vec3 dir) {\n"
" return dot(normalize(-dir),normalize(ex_LightPos2));\n"
"}\n"
"float intensity2b(vec3 dir) {\n"
" float i=intensityb(dir); return pow(i,hilight);\n"
"}\n"

"vec3 normalmix(vec3 a, vec3 b) {\n"
    //"float angle1 = acos(a.z)+1.0*acos(b.z);\n"
    //"float angle2 = atan(a.y,a.x)+1.0*atan(b.y,b.x);\n"
    //"return vec3(sin(angle1)*cos(angle2),sin(angle1)*sin(angle2),cos(angle1));\n"
    "return normalize(normalize(a)+normalize(b)*0.2);\n"

"}\n"
"vec4 bump_phong(vec4 rgb)\n"
    "{\n"
    "    vec3 c = vec3(0.0,0.0,0.0);\n"
    "    vec3 n = ex_Normal2;\n"
    "    vec3 n2 = texture2D(texsampler[0],ex_TexCoord.xy).rgb;\n"
    "    n2-=vec3(0.5,0.5,0.5);\n"
    "    n2*=2.0;\n"
    "    vec3 normal=normalmix(n,n2);\n"

    //"     c+=0.2*texture2D(texsampler[1],ex_TexCoord.xy).rgb;\n"
    "    c+=intensityb(normal)*level1_color.rgb;\n"
    "    c=mix(c,level2_color.rgb,intensity2b(normal));\n"
    //"     c=sqrt(c);\n"
    "     c+=rgb.rgb;\n"
    "     c=clamp(c,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "    return vec4(c,rgb.a);\n"
    "}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
   
"#ifdef EX_POSITION\n"
    
"uniform int num_pos;\n"
"uniform vec3 obj_pos[100];\n"
"uniform float obj_size;\n"

"vec4 gi(vec4 rgb)\n"
"{\n"
    // " return rgb;\n" 
"   vec3 frag_pos = ex_Position;\n"
"   float Ka = 0.0;\n"
"   for(int i=0;i<100;i++) {\n"
"     vec3 pos = obj_pos[i];\n"
"     float size = obj_size;\n"
"     pos-=frag_pos;\n"
"     float d = sqrt(pos.x*pos.x+pos.y*pos.y+pos.z*pos.z);\n"
"     float K = tan(asin(size/d));\n"
"     float KK = K*K;\n"
"     Ka+=KK;\n"
"    }\n"
"    Ka/=4.0*3.14159;\n"
"    return clamp(rgb-vec4(Ka,Ka,Ka,0.0),vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));\n"


"}\n"
"#endif\n"
    
"#ifdef MANYTEXTURES\n"
"in vec2 shadow_position;\n"
"uniform vec4 shadow_dark;\n"
"uniform float shadow_tex;\n"
"vec4 shadow(vec4 rgb)\n"
"{\n"
"   vec2 f = shadow_position;\n"
"   vec4 tx = vec4(0.0,0.0,0.0,0.0);\n"
"   if (shadow_tex<0.7) tx=texture2D(texsampler[0],f); else\n"
"   if (shadow_tex<1.7) tx=texture2D(texsampler[1],f); else\n"
"   if (shadow_tex<2.7) tx=texture2D(texsampler[2],f); else\n"
"   if (shadow_tex<3.7) tx=texture2D(texsampler[3],f); else\n"
"   if (shadow_tex<4.7) tx=texture2D(texsampler[4],f); else\n"
"   if (shadow_tex<5.7) tx=texture2D(texsampler[5],f); else\n"
"   if (shadow_tex<6.7) tx=texture2D(texsampler[6],f); else\n"
"   if (shadow_tex<7.7) tx=texture2D(texsampler[7],f); else\n"
"   if (shadow_tex<8.7) tx=texture2D(texsampler[8],f); else\n"
"   if (shadow_tex<9.7) tx=texture2D(texsampler[9],f); else\n"
"   if (shadow_tex<10.7) tx=texture2D(texsampler[10],f); else\n"
"   if (shadow_tex<11.7) tx=texture2D(texsampler[11],f); else\n"
"   if (shadow_tex<12.7) tx=texture2D(texsampler[12],f); else\n"
"   if (shadow_tex<13.7) tx=texture2D(texsampler[13],f); else\n"
"     tx=texture2D(texsampler[14],f);\n"
"   float alpha = tx.r;\n"
"   float visibility = 1.0;\n"
"   if (alpha>0.5) visibility = 0.5;\n"
"   rgb = shadow_dark+visibility*rgb;\n"
"   return rgb;\n"
"}\n"
"#endif\n"

"#ifdef EX_POSITION\n"
"in float fog_intensity;\n"
"uniform vec4 fog_dark;\n"
"uniform vec4 fog_light;\n"
"vec4 fog(vec4 rgb)\n"
"{\n"
"   float f = fog_intensity;\n"
    //"   f = 1.0/exp(f*0.5);\n"
"   return max(rgb,mix(fog_dark,fog_light,1.0-f));\n"
"}\n"
"#endif\n"
    "#ifdef EX_POSITION\n"
    "#ifdef EX_NORMAL2\n"
    	"uniform vec3 dyn_light_pos;\n"
         "uniform float dyn_light_dist;\n"
"vec4 dyn_lights(vec4 rgb)\n"
	"{\n"
	" vec3 p =ex_Position-dyn_light_pos;\n"
        " vec3 n = normalize(ex_Normal2);\n"
        " vec3 r = reflect(p,n);\n"
        " vec3 v = vec3(0.0,0.0,-1.0);\n"
	" float d=length(p);\n"
    "d/=dyn_light_dist;\n"
    //"d=1.0-d;\n"
    //"d=clamp(d,0.0,1.0);\n"
           "d=d*d;\n"
    "d=1.0/d;\n"
    "float dt =dot(normalize(v),normalize(r));\n"
    "float dt2 = sqrt(1.0-dt*dt);\n"
    "d=d*dt2;\n"
           "d=clamp(d,0.0,1.0);\n"
	" return vec4(d,d,d,rgb.a);\n"
	"}\n"
"#endif\n"
"#endif\n"
    "#ifdef EX_COLOR\n"
"vec4 colour(vec4 rgb)\n"
"{\n"
"  return mix(rgb, ex_Color, 1.0);    \n"
"}\n"
"#endif\n"
    "#ifdef EX_COLOR\n"
"#ifdef COLOR_MIX\n"
"vec4 colour_with_mix(vec4 rgb)\n"
"{\n"
"  return vec4(mix(rgb.rgb, ex_Color.rgb, color_mix),ex_Color.a);    \n"
"}\n"
"#endif\n"
"#endif\n"
"const float GAMMA=2.2;\n"
"const float INV_GAMMA = 1.0/GAMMA;\n"
"vec3 LINEARtoSRGB(vec3 color)\n"
"{\n"
" return pow(color, vec3(INV_GAMMA));\n"
"}\n"
"vec4 SRGBtoLINEAR(vec4 srgbIn)\n"
"{\n"
"  return vec4(pow(srgbIn.xyz,vec3(GAMMA)), srgbIn.w);\n"
"}\n"

"#ifdef EX_COLOR\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef EX_NORMAL\n"
"#ifdef EX_POSITION\n"
"#ifdef COLOR_MIX\n"
"uniform float u_RoughnessFactor;\n"
"uniform float u_MetallicFactor;\n"
"uniform vec4 u_BaseColorFactor;\n"
"uniform float u_OcculsionStrength;\n" 
"uniform float u_NormalScale;\n"
    "uniform float u_EmissiveFactor;\n"
"uniform vec3 u_DiffFactor;\n"
"uniform vec3 u_SpecFactor;\n"
"uniform float u_GlossiFactor;\n"

    "vec4 getVertexColor()\n"
"{\n"
"  return ex_Color;\n"
"}\n"
"struct MaterialInfo {\n"
"  float perceptualRoughness;\n"
"  vec3 reflectance0;\n"
"  float alphaRoughness;\n"
"  vec3 diffuseColor;\n"
"  vec3 reflectance90;\n"
"  vec3 specularColor;\n"
"};\n"
"struct AngularInfo {\n"
"  float NdotL;\n"
"  float NdotV;\n"
"  float NdotH;\n"
"  float LdotH;\n"
"  float VdotH;\n"
"  vec3 padding;\n"
"};\n"
"#ifdef GLTF_TEX5\n"
"#ifdef GLTF_TEX6\n"
"#ifdef GLTF_TEX7\n"
"vec3 getIBLContribution(MaterialInfo info, vec3 n, vec3 v)\n"
"{\n"
"   float NdotV = clamp(dot(n,v), 0.0, 1.0);\n"
"   vec3 ref = normalize(reflect(-v,n));\n"
  "   vec2 bfrdsample = clamp(vec2(NdotV, info.perceptualRoughness), vec2(0.0,0.0), vec2(1.0,1.0));\n";
 if (webgl2) {
s+="   vec2 bfrd = texture(texsampler[7], bfrdsample).rg;\n"
"   vec4 diff = texture(texsampler_cube[5], n);\n"
  "   vec4 spec = texture(texsampler_cube[6], ref);\n";
 } else {
s+="   vec2 bfrd = texture2D(texsampler[7], bfrdsample).rg;\n"
"   vec4 diff = textureCube(texsampler_cube[5], n);\n"
  "   vec4 spec = textureCube(texsampler_cube[6], ref);\n";
 }
 s+= "   vec3 diffLight = SRGBtoLINEAR(diff).rgb;\n"
"   vec3 specLight = SRGBtoLINEAR(spec).rgb;\n"
    "   vec3 diffuse = diffLight * info.diffuseColor;\n"
    "   vec3 specular = specLight * (info.specularColor * bfrd.x + bfrd.y);\n"
"   return diffuse+specular;\n"   
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"vec3 ddFdx(vec3 p) {\n"
"return vec3(dFdx(p.x),dFdx(p.y),dFdx(p.z));\n"
    "}\n"
"vec3 ddFdy(vec3 p) {\n"
"return vec3(dFdy(p.x),dFdy(p.y),dFdy(p.z));\n"
"}\n"

    
"vec3 getNormal() {\n"
"   vec2 uv = ex_TexCoord.xy;\n"
"   vec3 pos_dx = ddFdx(ex_Position);\n"
"   vec3 pos_dy = ddFdy(ex_Position);\n"
"   vec3 tex_dx = ddFdx(vec3(uv,0.0));\n"
"   vec3 tex_dy = ddFdy(vec3(uv,0.0));\n"
"   vec3 t = (tex_dx.t * pos_dx - tex_dx.t*pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);\n"
"   vec3 ng = normalize(ex_Normal);\n"
"   t = normalize(t-ng*dot(ng,t));\n"
"   vec3 b = normalize(cross(ng,t));\n"
"   mat3 tbn = mat3(t,b,ng);\n"
"#ifdef GLTF_TEX2\n"
"   vec3 n = texture2D(texsampler[2], uv).rgb;\n"
"   n = normalize(tbn*((2.0*n-1.0) * vec3(u_NormalScale,u_NormalScale,1.0)));\n"
"   return n;\n"
"#endif\n"
"   return normalize(ex_Normal);\n"
"}\n"
"AngularInfo getAngularInfo( vec3 pointToLight, vec3 normal, vec3 view) {\n"
"  vec3 n = normalize(normal);\n"
"  vec3 v = normalize(view); \n"
"  vec3 l = normalize(pointToLight); \n"
"  vec3 h = normalize(l+v);\n"
"  float NdotL = clamp(dot(n,l), 0.0, 1.0);\n"
"  float NdotV = clamp(dot(n,v), 0.0, 1.0);\n"
"  float NdotH = clamp(dot(n,h), 0.0, 1.0);\n"
"  float LdotH = clamp(dot(l,h), 0.0, 1.0);\n"
"  float VdotH = clamp(dot(v,h), 0.0, 1.0);\n"
"  return AngularInfo(\n"
"     NdotL, NdotV, NdotH, LdotH, VdotH, vec3(0,0,0) );\n"
"}\n"
"vec3 diffuse(MaterialInfo info) {\n"
"  return info.diffuseColor / 3.14159265;\n"
"}\n"
"vec3 specularReflection(MaterialInfo info, AngularInfo ainfo) {\n"
"  return info.reflectance0 + (info.reflectance90 - info.reflectance0) * pow(clamp(1.0-ainfo.VdotH,0.0,1.0),5.0);\n"
"}\n"
"float visibilityOcclusion(MaterialInfo info, AngularInfo ainfo) {\n"
"  float NdotL = ainfo.NdotL;\n"
"  float NdotV = ainfo.NdotV;\n"
"  float alphaSq = info.alphaRoughness * info.alphaRoughness;\n"
"  float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0-alphaSq) + alphaSq);\n"
"  float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0-alphaSq) + alphaSq);\n"
"  float GGX = GGXV + GGXL;\n"
"  if (GGX > 0.0) { return 0.5 / GGX; }\n"
"  return 0.0;\n"
"}\n"
"float micro(MaterialInfo info, AngularInfo ainfo) {\n"
"  float alphaSq = info.alphaRoughness * info.alphaRoughness;\n"
"  float f = (ainfo.NdotH * alphaSq - ainfo.NdotH) * ainfo.NdotH + 1.0;\n"
"  return alphaSq / (3.14159265 * f * f);\n"
"}\n"
"vec3 getPointShade(vec3 pointToLight, MaterialInfo info, vec3 normal, vec3 view) { \n"
"  AngularInfo ainfo = getAngularInfo(pointToLight, normal, view);\n"
    "  if (ainfo.NdotL > 0.0 || ainfo.NdotV > 0.0) {\n"
"    vec3 F = specularReflection(info, ainfo);\n"
"    float Vis = visibilityOcclusion(info, ainfo);\n"
"    float D = micro(info, ainfo);\n"
"    vec3 diffuseContrib = (1.0-F) * diffuse(info);\n"
"    vec3 specContrib = F * Vis * D;\n"
"    return ainfo.NdotL * (diffuseContrib + specContrib);\n"
    "  }\n"
    "  return vec3(0.0,0.0,0.0);\n"
"}\n"
"uniform float color_mix4;\n"
"uniform vec3 light_dir2;\n"
"vec4 gltf(vec4 rgb)\n"
"{\n"
"float perceptualRoughness=0.0;\n"
"float metallic=0.0;\n"
"vec4 baseColor=vec4(0.0,0.0,0.0,1.0);\n"
"vec3 diffuseColor=vec3(0);\n"
"vec3 specularColor=vec3(0.0);\n"
"vec3 f0 =vec3(0.04);\n"
"float specAlpha=1.0;\n"
    "#ifndef SPEC\n"
"#ifdef GLTF_TEX1\n"
"  vec4 mrSample = texture2D(texsampler[1],ex_TexCoord.xy);\n"
"  perceptualRoughness = mrSample.g * u_RoughnessFactor;\n"
"  metallic = mrSample.b * u_MetallicFactor;\n"
"#endif\n"
    "#ifndef GLTF_TEX1\n"
    "  metallic = u_MetallicFactor;\n"
    "  perceptualRoughness = u_RoughnessFactor;\n"
    "#endif\n"
"#ifdef GLTF_TEX0\n"
"  baseColor = SRGBtoLINEAR(texture2D(texsampler[0],ex_TexCoord.xy)) * u_BaseColorFactor;\n"
"#endif\n"
    "#ifndef GLTF_TEX0\n"
    "  baseColor = u_BaseColorFactor;\n"
    "#endif\n"
    "#endif\n"

"#ifdef SPEC\n"



    "#ifdef GLTF_TEX0\n"
"  baseColor = SRGBtoLINEAR(texture2D(texsampler[0],ex_TexCoord.xy)) * u_BaseColorFactor;\n"
"#endif\n"
    "#ifndef GLTF_TEX0\n"
    "  baseColor = u_BaseColorFactor;\n"
    "#endif\n"
    
   "#ifdef GLTF_TEX1\n";
 if (webgl2) {
   s+=   "  vec4 mrSample2 = texture(texsampler[1],ex_TexCoord.xy);\n";
 } else {
   s+=   "  vec4 mrSample2 = texture2D(texsampler[1],ex_TexCoord.xy);\n";
 }
s+=        "mrSample2.r *= u_DiffFactor.r;\n"
    "mrSample2.g *= u_DiffFactor.g;\n"
    "mrSample2.b *= u_DiffFactor.b;\n"

   " diffuseColor = mrSample2.rgb * (1.0-max(baseColor.r,baseColor.g,baseColor.b));\n"
   " f0 = baseColor.rgb;\n"
   "f0.r *= u_SpecFactor.r;\n"
   "f0.g *= u_SpecFactor.g;\n"
   "f0.b *= u_SpecFactor.b;\n"
    "specAlpha = mrSample2.a;\n"
    " perceptualRoughness = (1.0-baseColor.a);\n"
    //" perceptualRoughness=perceptualRoughness*perceptualRoughness;\n"
    "#endif\n"
    "#ifndef GLTF_TEX1\n"
   " diffuseColor = vec3(1.0,1.0,1.0)*(1.0-max(baseColor.r,baseColor.g,baseColor.b));\n"
   " f0 = baseColor.rgb;\n"
   "f0.r *= u_SpecFactor.r;\n"
   "f0.g *= u_SpecFactor.g;\n"
   "f0.b *= u_SpecFactor.b;\n"
    "perceptualRoughness = (1.0-baseColor.a);\n"
    //"perceptualRoughness=perceptualRoughness*perceptualRoughness;\n"
    "#endif\n"
    "#endif\n"

    "#ifndef SPEC\n"    
    "  baseColor *= getVertexColor();\n"
    "#endif\n"
"#ifndef SPEC\n"
"  diffuseColor = baseColor.rgb * (vec3(1.0)-f0) * (1.0-metallic);\n"
"#endif\n"
"#ifdef SPEC\n"
"  diffuseColor = baseColor.rgb * (f0) * (1.0-metallic);\n"
"#endif\n"
    "  specularColor = mix(f0, baseColor.rgb, metallic);\n"

    //   "  baseColor.a = 1.0;\n"
    // use next one if material is unlit
   //"  return vec4(LINEARtoSRGB(baseColor.rgb),baseColor.a);\n"

"  perceptualRoughness = clamp(perceptualRoughness, 0.0, 1.0);\n"
"  metallic = clamp(metallic, 0.0, 1.0);\n"
""
"  float alphaRoughness = perceptualRoughness * perceptualRoughness;\n"
"  float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);\n"
"  vec3 specularEnvironmentR0 = specularColor.rgb;\n"
"  vec3 specularEnvironmentR90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));\n"
    "  MaterialInfo materialInfo = MaterialInfo(\n"
    "    perceptualRoughness,\n"
    "    specularEnvironmentR0,\n"
    "    alphaRoughness,\n"
    "    diffuseColor,\n"
    "    specularEnvironmentR90,\n"
    "    specularColor\n"
    "    );\n"
  "    vec3 color = vec3(0.0,0.0,0.0);\n"
    "    vec3 normal = getNormal();\n"
    "    vec3 view = vec3(0.0,0.0,1.0);\n"

    // 0.0, 0.0,400.0
    "  color = getPointShade(light_dir2 /*+ex_Position/100.0*/, materialInfo, normal, view);\n"
  //"  color*=baseColor.rgb;\n"
  "color+=baseColor.rgb/4.0;\n"
  
"#ifdef GLTF_TEX5\n"
"#ifdef GLTF_TEX6\n"
"#ifdef GLTF_TEX7\n"
"  color += getIBLContribution(materialInfo, normal, view);\n"
"#endif\n"
"#endif\n"
"#endif\n"

    // TODO LIGHTS
"   float ao=1.0;\n"
"#ifdef GLTF_TEX3\n"
"   ao = texture2D(texsampler[3], ex_TexCoord.xy).r;\n"
"   color = mix(color, color*ao, u_OcculsionStrength);\n"
"#endif\n"
"   vec3 emissive = vec3(0);\n"
"#ifdef GLTF_TEX4\n"
"   emissive = SRGBtoLINEAR(texture2D(texsampler[4], ex_TexCoord.xy)).rgb * u_EmissiveFactor;\n"
"   color += emissive;\n"
"#endif\n"
    //"   return vec4(metallic,metallic,metallic,1.0);\n"
    //"   return vec4(vec3(perceptualRoughness),1.0);\n"
    //"   return vec4(vec3(ao),1.0);\n"
    //"   return vec4(vec3(LINEARtoSRGB(emissive),1.0);\n"
    //"   return vec4(vec3(baseColor.a),1.0);\n"

"#ifdef UNLIT\n"
    "color = baseColor.rgb/4.0;\n"
"#endif\n"
    
    "color = clamp(color,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));\n"
    "#ifdef SPEC\n"
    "   return vec4(mix(rgb.rgb,LINEARtoSRGB(color),color_mix4), specAlpha);\n"

"#endif\n"
"#ifndef SPEC\n"
    "   return vec4(mix(rgb.rgb,LINEARtoSRGB(color),color_mix4), baseColor.a);\n"
"#endif\n"

    //"   return vec4(LINEARtoSRGB(color), baseColor.a);\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#ifdef EX_COLOR\n"
"#ifdef EX_TEXCOORD\n"
"#ifdef EX_NORMAL\n"
"#ifdef EX_POSITION\n"
"#ifdef COLOR_MIX\n"
"#ifdef EX_POSITION\n"
"#ifdef NEWSHADOW\n"
"vec4 shadowgltf(vec4 rgb)\n"
"{\n"
"   vec4 rgb2 = gltf(rgb);\n"
"   float level = newshadow_level();\n"
"   if (level>0.5) {\n"
"     float level2 = (level-0.5)*2.0;\n"
"   vec4 res = (1.0-level2)*rgb + level2*rgb2;\n"
"     return res;\n"
"   } else {\n"
"     return rgb*(level*2.0);\n"
"   }\n"
"}\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"
"#endif\n"


    
"vec4 empty(vec4 rgb)\n"
"{\n"
"   return rgb;\n"
"}\n"
"//T:\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"//V: screen\n"
"#version 330\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"in vec3 in_Normal;\n"
"in vec3 in_Position;\n"
"out vec2 XY;\n"
"\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  XY = in_Normal.xy;\n"
"  gl_Position = in_P * in_T * in_MV *vec4(in_Position,1.0);\n"
"}\n"
"//F: screen\n"
"#version 330\n"
"out vec4 out_Color;\n"
"in vec2 XY;\n"
"uniform float time;\n"
"uniform float time2;\n"
"uniform float time3;\n"
"uniform float roty;\n"
"uniform sampler2D tex;\n"
"uniform float rotangle;\n"
"uniform vec3 player_pos;\n"
"uniform vec3 target_pos;\n"
"//M:\n"
"\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
"{\n"
"	vec2 iResolution = vec2(1.0,1.0);\n"
"        vec2 pos = fragCoord.xy;\n"
"	pos.x*=800.0/600.0;\n"
"        vec3 p0 = vec3(pos-vec2(0.5),-400.0);\n"
"	vec3 p1 = vec3(pos*2.0-vec2(1.0),-399.0);\n"
"//N:\n"
"	fragColor = rgb;\n"
"}\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  vec4 color;\n"
"  mainImage(color, XY);\n"
"  out_Color = color;\n"
"}\n"
"\n"
"//V: empty\n"
"#version 330\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"in vec3 in_Position;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"  vec4 v = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"  gl_Position = v;\n"
"}\n"
"\n"
"//F: empty\n"
"#version 330\n"
"precision highp float;\n"
"out vec4 out_Color;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   out_Color = vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"\n"
"//V: colour\n"
"#version 330\n"
"/*layout(location=0)*/ in vec3 in_Position;\n"
"/*layout(location=1)*/ in vec3 in_Normal;\n"
"/*layout(location=2)*/ in vec3 in_Color;\n"
"/*layout(location=3)*/ in vec3 in_TexCoord;\n"
"out vec3 ex_Color;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"   ex_Color = in_Color;\n"
"}\n"
"//F: colour\n"
"#version 330\n"
"//precision highp float;\n"
"in vec3 ex_Color;\n"
"out vec4 out_Color;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   out_Color = vec4(ex_Color,1.0);\n"
"}\n"
"\n"
"//V: texture\n"
"#version 330\n"
"/*layout(location=0)*/ in vec3 in_Position;\n"
"/*layout(location=1)*/ in vec3 in_Normal;\n"
"/*layout(location=2)*/ in vec3 in_Color;\n"
"/*layout(location=3)*/ in vec3 in_TexCoord;\n"
"out vec3 ex_TexCoord;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV *  vec4(in_Position,1.0);\n"
"   ex_TexCoord = in_TexCoord;\n"
"}\n"
"//F: texture\n"
"#version 330\n"
"//precision highp float;\n"
"uniform sampler2D texture;\n"
"in vec3 ex_TexCoord;\n"
"out vec4 out_Color;\n"
"//T:\n"
"void main(void)\n"
"{\n"
"   out_Color = texture2D(texture, ex_TexCoord.xy);\n"
  "}\n";
  }
  return s;
}

extern std::string g_gpu_vendor;

ShaderFile::ShaderFile() {
  bool oldshader=false;
  bool webgl2 = false;
  bool emscripten = false;
  bool apple = false;
#ifdef OLD_SHADER
  oldshader=true;
#endif
#ifdef WEBGL2
  webgl2 = true;
#endif
#ifdef EMSCRIPTEN
  emscripten=true;
#endif
#ifdef __APPLE__
  apple=true;
#endif

  if (g_gpu_vendor=="AMD") {
    oldshader=true;
  }

  if (!(g_gpu_vendor=="NVID" || g_gpu_vendor=="AMD"))
    { // intel gfx cards didn't support newer shaders
      oldshader=true;
    }

  
  std::string s = get_uber_shader(oldshader, webgl2, emscripten, apple);
  std::stringstream file;
  file << s;
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
std::vector<std::string> replace_c_template_unique_ids(std::string defines, std::string define)
{
  int id = 0;
  std::vector<std::string> ids;
  std::stringstream ss(defines);
  std::string s;
  while(ss >> s)
    {
      if (s == define) { 
	id++;
	std::stringstream ss2;
	ss2 << id;
	ids.push_back(ss2.str()); }
    }
  return ids;
}
extern std::string g_gpu_vendor;

struct replace_c_params
{
  std::string s;
  std::vector<std::string> comb;
  bool is_fragment;
  bool is_fbo;
  bool is_transparent;
  ShaderModule *mod;
  ShaderCall *call;
  std::string defines;
  bool is_get_pixel;
  std::string shader;
};

std::string replace_c(const replace_c_params &pp)
{
  bool oldshader=false;
  bool webgl2 = false;
  bool emscripten = false;
  bool apple = false;
#ifdef OLD_SHADER
  oldshader=true;
#endif
#ifdef WEBGL2
  webgl2 = true;
#endif
#ifdef EMSCRIPTEN
  emscripten=true;
#endif
#ifdef __APPLE__
  apple=true;
#endif

  if (g_gpu_vendor=="AMD") {
    oldshader=true;
  }

  if (!(g_gpu_vendor=="NVID" || g_gpu_vendor=="AMD"))
    { // intel gfx cards didn't support newer shaders
      oldshader=true;
    }



  
  std::string s = pp.s;
  std::vector<std::string> comb = pp.comb;
  bool is_fragment= pp.is_fragment;
  bool is_fbo = pp.is_fbo;
  bool is_transparent = pp.is_transparent;
  ShaderModule *mod = pp.mod;
  ShaderCall *call = pp.call;
  std::string defines = pp.defines;
  bool is_get_pixel = pp.is_get_pixel;
  std::string shader = pp.shader;



  int unique_id = 0;
  std::stringstream ss(s);
  std::string ww;
  std::string out;
  std::vector<std::string> current_define;
  std::vector<int> current_bools;
  std::vector<std::string> current_template;
  std::vector<int> current_template_id;

  bool inside_template = false;

  int line_num = 0;

  std::string templates = "";
  std::vector<std::string> lines;
  while(std::getline(ss,ww)) { lines.push_back(ww); }

  while(unsigned(line_num)<lines.size())
    {
      std::string ww = lines[line_num];
      //std::cout << "Line: " << line_num << " : " << ww << std::endl;
      line_num ++;
      if (ww.substr(0,4)=="//T:")
	{
	  std::stringstream ss(templates);
	  std::string ww2;
	  int line = line_num;
	  while(std::getline(ss,ww2)) { lines.insert( lines.begin()+line, ww2); line++; } 
	  //out+=templates;
	  continue;
	}

      if (ww.substr(0,10)=="#template ")
	{
	  std::stringstream ss(ww.substr(10));
	  std::string s;
	  ss >> s;


	  std::string strings= defines;
	  if (call)
	    strings += " " + call->define_strings();
	  std::stringstream ss2(strings);
	  std::string s2;
	  while(ss2>>s2)
	    {
	      if (s2==s)
		{
		  //int id = unique_id;
		  unique_id++;
		  current_template_id.push_back(unique_id);	  
		  current_template.push_back("");
		}
	    }
	  inside_template = true;
	  continue;
	}
      if (ww.substr(0,12)=="#endtemplate")
	{
	  inside_template = false;
	  int s = current_template.size();
	  for(int i=0;i<s;i++)
	    {
	      templates+= current_template[i];
	    }

	  while (current_template.size() != 0)
	    current_template.pop_back();
	  while (current_template_id.size() != 0)
	    current_template_id.pop_back();
	  continue;
	}
      if (inside_template)
	{ // here we're inside a template
	  int s = current_template_id.size();
	  for(int i=0;i<s;i++) {
	    // here we should replace template num with the unique_id
	    std::string res = "";
	    char ch;
	    std::stringstream ss(ww);
	    while(ss.get(ch)) {
	      if (ch=='@') {
		std::stringstream ss2;
		ss2 << i+1;
		res+=ss2.str();
	      } else {
		res+=ch;
	      }
	    }
	    current_template[i] += res + "\n";
	  }
	  continue;
	}

      if (ww.substr(0,7)=="#ifdef ")
	{
	  std::stringstream ss(ww.substr(7));
	  std::string s;
	  ss >> s;

	  std::string strings= defines;
	  if (call)
	    strings += " " + call->define_strings();
	  std::stringstream ss2(strings);


	  //std::cout << "Strings:" << strings << std::endl;

	  std::string s2;
	  bool b = true;
	  while(ss2>>s2)
	    {
	      if (s2==s)
		{
		  b = false;
		  //std::cout << "Success:" << s2 << "==" << s << " -> false" << std::endl;
		  break;
		}
	    }
	  if (b) {
	    //std::cout << "// Exclude " << s << std::endl;
	  } else {
	    //std::cout << "// Include " << s << std::endl;
	  };
	  current_bools.push_back(b?1:0);
	  if (b) {
	    //std::cout << "Push " << s << std::endl;
	    current_define.push_back(s);
	  }
	  continue;
	}
      if (ww.substr(0,8)=="#ifndef ")
	{
	  std::stringstream ss(ww.substr(8));
	  std::string s;
	  ss >> s;

	  std::string strings= defines;
	  if (call)
	    strings += " " + call->define_strings();
	  std::stringstream ss2(strings);
	  std::string s2;
	  bool b = false;

	  //std::cout << "Strings:" << strings << std::endl;
	  
	  while(ss2>>s2)
	    {
	      if (s2==s)
		{
		  //std::cout << "Success:" << s2 << "==" << s << " -> true" << std::endl;
		  b = true;
		  break;
		}
	    }
	  if (b) {
	    //std::cout << "// Exclude ndef " << s << std::endl;
	  } else {
	    //std::cout << "// Include ndef " << s << std::endl;
	  };
	  current_bools.push_back(b?1:0);
	  if (b) {
	    //  std::cout << "Push " << s << std::endl;
	    current_define.push_back(s);
	  }
	  continue;
	}
      if (ww.substr(0,6)=="#endif")
	{
	  if (current_bools.size()>0)
	    {
	    int b = current_bools[current_bools.size()-1];
	    if (b) {
	      //std::cout << "Pop:" << current_define[current_define.size()-1] << std::endl;
	      current_define.pop_back();
	    }
	    current_bools.pop_back();
	    }
	  continue;
	}
      if (current_define.size() != 0) { continue; }

      if (mod && ww.substr(0,4)=="//M:")
	{
	  out+=mod->Function();
	}
      if (ww.substr(0,4)=="//B:")
	{
	  out+=shader;
	}
      if (mod && ww.substr(0,4)=="//N:")
	{
	  if (is_fragment) {
	    out+="vec4 rgb = ";
	    out+=funccall_to_string(mod);
	    out+=";\n";
	  }
	}
      
      if (ww.substr(0,4)=="//C:")
	{
	  if (!is_fragment)
	    { // vertex
	      if (call)
		{
		  int num = call->index(0);
		  std::string s = call->func_call();
		  out+=s;
		  int num2 = num + 1;
		  std::string s2 = call->func_call2(num2);
		  out+=s2;
		  //std::cout << s << std::endl;

		  std::stringstream ss3;
		  ss3 << num2;
		  out+="gl_Position = in_P * in_T * in_MV * pos";
		  out+=ss3.str();
		  out+=";\n";
		  
		}
	      else
		{
	      out+="vec3 p = mix(in_Position, in_Position2, in_POS);\n";
	      out+="vec4 pos0 =  vec4(p,1.0);\n";
	      int s = comb.size();
	      for(int i=0;i<s;i++)
		{
		  std::stringstream ss;
		  ss << i+1;
		  std::stringstream ss2;
		  ss2 << i;
		  out+="vec4 pos";
		  out+=ss.str();
		  out+=" = ";
		  if (comb[i]=="SFO")
		    {
		      out+=funccall_to_string_with_replace(mod, "p1", "pos" + ss.str());
		      out+=";\n";
		    }
		  else
		    {
		      out+=comb[i];
		      out+="(pos";
		      out+=ss2.str();
		      out+=");\n";
		    }
		}
	      std::stringstream ss3;
	      ss3 << s;
	      out+="gl_Position = in_P * in_T * in_MV * pos";
	      out+=ss3.str();
	      out+=";\n";
		}
	    }
	  else
	    { // fragment
	      if (call) {
		int num = call->index(0);
		std::string s = call->func_call();
		out+=s;
		int num2 = num + 1;
		std::string s2 = call->func_call2(num2);
		out+=s2;
		//std::cout << "Fragment: " << s << std::endl;
		std::stringstream ss3;
		ss3 << num2;

		if (oldshader) {
		  if (webgl2) {
		if (is_fbo)
		  out += "gl_FragData[0] = rgb"; // TODO, fbo's probably not working.
		else
		  out+="out_Color = rgb";
		  } else {
		if (is_fbo)
		  out += "gl_FragData[0] = rgb";
		else
		  out+="gl_FragColor = rgb";
		  }
		} else {
		out+="out_Color = rgb";
		}
		out+= ss3.str();
		out+=";\n";
		
	      }
	      else
		{
		  if (is_get_pixel) {
		    out+="vec4 rgb0 = gl_FragColor;\n";
		  } else {
		    if (is_transparent)
		      out+="vec4 rgb0 = vec4(0.0,0.0,0.0,0.0);\n";
		    else
		      out+="vec4 rgb0 = vec4(0.0,0.0,0.0,1.0);\n";
		  }
	      int s = comb.size();
	      for(int i=0;i<s;i++)
		{
		  std::stringstream ss;
		  ss << i+1;
		  std::stringstream ss2;
		  ss2 << i;
		  out+="vec4 rgb";
		  out+=ss.str();
		  out+=" = ";
		  out+=comb[i];
		  out+="(rgb";
		  out+=ss2.str();
		  out+=");\n";
		}
	      std::stringstream ss3;
	      ss3 << s;

	      if (oldshader) {
		if (webgl2) {
	      if (is_fbo) {
		out += "gl_FragData[0] = rgb"; // TODO
	      out+= ss3.str();
	      }
	      else {
	      if (g_gpu_vendor!="NVID") {
		out+="out_Color = rgb";
	      out+= ss3.str();
	      } else {
		out+="out_Color = rgb" + ss3.str() + "";
	      }
	      }
		} else {
	      if (is_fbo) {
		out += "gl_FragData[0] = rgb";
	      out+= ss3.str();
	      }
	      else {
		out+="gl_FragColor = rgb";
	      out+= ss3.str();
	      }
		}

	      } else {
	      if (g_gpu_vendor!="NVID") {
		out+="out_Color = rgb" + ss3.str() + "";
	      } else {
		out+="out_Color = rgb" + ss3.str() + "";
	      }
	      }
	      out+=";\n";
		}
	    }

	}
      else
	{
	  out+=ww;
	  out+= "\n";
	}
    }

  /*
  if  (current_define.size()!=0)
    {
      std::cout << "ERROR:" << std::endl;
      int s=current_define.size();
      for(int i=0;i<s;i++) std::cout << "STACK STILL HAS:" << current_define[i] << std::endl;
    }
  */
  return out;
}
std::string add_line_numbers(std::string s)
{
  std::string res;
  std::stringstream ss(s);
  std::string s2;
  int num = 0;
  while(std::getline(ss,s2))
    {
      std::stringstream t;
      t << num;
      res+= t.str() + ": " + s2 + "\n";
      num++;
    }
  return res;
}

struct ShaderCacheItem
{
  std::string v_format, f_format, g_format;
  std::vector<std::string> v_vec, f_vec;
  bool is_trans;
  ShaderModule *mod;
  ShaderCall *vertex_c, *fragment_c;
  std::string v_defines, f_defines, v_shader, f_shader;

  std::vector<Shader*> v_shaders;
  std::vector<Shader*> f_shaders;
  std::vector<Shader*> g_shaders;
  
  friend bool operator==(const ShaderCacheItem &i1,const ShaderCacheItem &i2);
};
std::vector<ShaderCacheItem> shader_cache;

void clear_shader_cache()
{
  shader_cache.clear();
}

bool operator==(const ShaderCacheItem &i1, const ShaderCacheItem &i2)
{
  bool b = true;
  if (i1.vertex_c && i2.vertex_c) {
    bool a0=i1.vertex_c->func_call()==i2.vertex_c->func_call();
    
    
    bool a1=  i1.vertex_c->define_strings()==i2.vertex_c->define_strings();
    bool a2=  i1.vertex_c->func_name()==i2.vertex_c->func_name();

    //if (a0==false) std::cout << "K";
    //if (a1==false) std::cout << "L";
    //if (a2==false) std::cout << "M";

    b=a1&&a2;
  }
  bool b2 = true;
  if (i1.fragment_c && i2.fragment_c)
    {

      bool c0=i1.fragment_c->func_call()==i2.fragment_c->func_call();
      bool c1=i1.fragment_c->define_strings()==i2.fragment_c->define_strings();
      bool c2=i1.fragment_c->func_name()==i2.fragment_c->func_name();
      //if (c0==false) std::cout << "Q";
      //if (c1==false) std::cout << "W";
      //if (c2==false) std::cout << "E";
      b2 = c1&&c2;
	
	

    }
  
  bool b3= i1.v_format==i2.v_format;
    bool b4=  i1.f_format==i2.f_format;
    bool b5=  i1.g_format==i2.g_format;
    bool b6=  i1.v_vec==i2.v_vec;
    bool b7=  i1.f_vec==i2.f_vec;
    bool b8=  i1.is_trans==i2.is_trans;
    bool b9=  i1.mod==i2.mod;

    bool b10=   i1.v_defines==i2.v_defines;
    bool b11=   i1.f_defines==i2.f_defines;
    bool b12=   i1.v_shader==i2.v_shader;
 bool b13=   i1.f_shader==i2.f_shader;
 /*
 if (b==false) std::cout << "1";
 if (b2==false) std::cout << "2";
 if (b3==false) std::cout << "3";
 if (b4==false) std::cout << "4";
 if (b5==false) std::cout << "5";
 if (b6==false) std::cout << "6";
 if (b7==false) std::cout << "7";
 if (b8==false) std::cout << "8";
 if (b9==false) std::cout << "9";
 if (b10==false) std::cout << "A";
 if (b11==false) std::cout << "B";
 if (b12==false) std::cout << "C";
 if (b13==false) std::cout << "D";
 if (i1.fragment_c && i2.fragment_c) std::cout << i1.fragment_c->func_name() << " " << i2.fragment_c->func_name()<< std::endl;
 */
 
 return b&&b2&&b3&&b4&&b5&&b6&&b7&&b8&&b9&&b10&&b11&&b12&&b13;
}

const ShaderCacheItem *find_from_shader_cache(const ShaderCacheItem &i2)
{
  int s = shader_cache.size();
  for(int i=0;i<s;i++)
    {
      const ShaderCacheItem &ii=shader_cache[i];
      if (i2==ii) {
	return &ii;
      }
    }
  return 0;
}

template<class T>
std::ostream &operator<<(std::ostream &o, const std::vector<T> &v)
{
  int s = v.size();
  for(int i=0;i<s;i++)
    {
    o << v[i];
    if (i!=s-1) { o << ","; }
    }
  return o;
}


int ShaderSeq::GetShader(std::string v_format, std::string f_format, std::string g_format, std::vector<std::string> v_vec, std::vector<std::string> f_vec, bool is_trans, ShaderModule *mod, ShaderCall *vertex_c, ShaderCall *fragment_c, std::string v_defines, std::string f_defines, std::string v_shader, std::string f_shader)
{
  ShaderCacheItem ci;
  ci.v_format = v_format;
  ci.f_format = f_format;
  ci.g_format = g_format;
  ci.v_vec = v_vec;
  ci.f_vec = f_vec;
  ci.is_trans = is_trans;
  ci.mod = mod;
  ci.vertex_c = 0; //vertex_c;
  ci.fragment_c = 0; //fragment_c;
  ci.v_defines = v_defines;
  ci.f_defines = f_defines;
  ci.v_shader = v_shader;
  ci.f_shader = f_shader;
  const ShaderCacheItem *res = find_from_shader_cache(ci); 

  int id = progs.size();
  Program *p = new Program;
  progs.push_back(p);

  // std::cout << "Checking: " << v_format << " " << f_format << " " << v_vec << " " << f_vec << std::endl;
  
  if (res) {
    //std::cout << "FOUND FROM SHADER CACHE" << std::endl;
    int s1 = res->v_shaders.size();
    for(int i=0;i<s1;i++) p->push_back(*res->v_shaders[i]);

    int s2 = res->f_shaders.size();
    for(int i=0;i<s2;i++) p->push_back(*res->f_shaders[i]);

    int s3 = res->g_shaders.size();
    for(int i=0;i<s3;i++) p->push_back(*res->g_shaders[i]);
    
    return id;
  }
  // std::cout << "NOT FOUND" << v_vec << " " << f_vec << std::endl;

  
  std::string::iterator i = v_format.begin();
  while(i!=v_format.end())
    {
      std::string::iterator ii = std::find(i, v_format.end(), ':');
      std::string name(i, ii);
      //std::cout << "VName: " << name << std::endl;
      std::string shader = file.VertexShader(name);
      //std::cout << "SHADER:" << shader << std::endl;
      replace_c_params *pp = new replace_c_params;
      pp->s = shader;
      pp->comb = v_vec;
      pp->is_fragment = false;
      pp->is_fbo = false;
      pp->is_transparent = is_trans;
      pp->mod = mod;
      pp->call = vertex_c;
      pp->defines = v_defines;
      pp->is_get_pixel = false;
      pp->shader = v_shader;
      std::string ss = replace_c(*pp /*shader, v_vec, false, false, is_trans, mod, vertex_c, v_defines, false,v_shader*/);

      delete pp; pp = 0;
      
      //std::cout << "::" << ss << "::" << std::endl;
      //                        std::cout << "::" << add_line_numbers(ss) << "::" << std::endl;
      ShaderSpec *spec = new SingletonShaderSpec(ss,vertex_c?vertex_c->func_name():"unknown");
      Shader *sha1;
      sha1 = new Shader(*spec, true, false);
      p->push_back(*sha1);
      ci.v_shaders.push_back(sha1);
      if (ii!=v_format.end())
	ii++;
      i = ii;
    }
  i = f_format.begin();
  while(i!=f_format.end())
    {
      std::string::iterator ii = std::find(i, f_format.end(), ':');
      std::string name(i, ii);
      //std::cout << "FName: " << name << std::endl;
      std::string shader = file.FragmentShader(name);
      //std::cout << "SHADER:" << shader << std::endl;
      replace_c_params *pp = new replace_c_params;
      pp->s = shader;
      pp->comb = f_vec;
      pp->is_fragment = true;
      pp->is_fbo = false;
      pp->is_transparent = is_trans;
      pp->mod = mod;
      pp->call = fragment_c;
      pp->defines = f_defines;
      pp->is_get_pixel = false;
      pp->shader = f_shader;

      std::string ss = replace_c(*pp /*shader, f_vec, true, false,is_trans, mod, fragment_c, f_defines, false, f_shader*/);
      delete pp; pp = 0;
      //                               std::cout << "::" << add_line_numbers(ss) << "::" << std::endl;
      ShaderSpec *spec = new SingletonShaderSpec(ss,fragment_c?fragment_c->func_name():"unknown");
      Shader *sha2 = new Shader(*spec, false, false);
      p->push_back(*sha2);
      ci.f_shaders.push_back(sha2);
      if (ii!=f_format.end())
	ii++;
      i = ii;
    }
  i = g_format.begin();
  while(i!=g_format.end())
    {
      std::string::iterator ii = std::find(i, g_format.end(), ':');
      std::string name(i, ii);
      //std::cout << "GName: " << name << std::endl;
      std::string shader = file.GeometryShader(name);
      //std::cout << "::" << shader << "::" << std::endl;
      ShaderSpec *spec = new SingletonShaderSpec(shader,g_format);
      Shader *sha2 = new Shader(*spec, false, true);
      p->push_back(*sha2);
      ci.g_shaders.push_back(sha2);
      if (ii!=g_format.end())
	ii++;
      i = ii;
    }
  shader_cache.push_back(ci);
  //std::cout << "ADDING TO CACHE: " << v_format << " " << f_format << "::" << v_vec << " " << f_vec << std::endl;
  //p->link();
  //p->GeomTypes(5,2); 
  //p->GeomOutputVertices(100000);
  return id;
}
void ShaderSeq::print_log(int i)
{
  if (progs[i])
    progs[i]->print_log();
}
void ShaderSeq::link(int i)
{
  if (progs[i])
    progs[i]->link();
  else
    std::cout << "Error: progs[i] in ShaderSeq::link is 0" << std::endl;
}
void ShaderSeq::use(int i)
{
  if (progs[i])
    progs[i]->use();
  else
    std::cout << "Error: progs[i] i=" << i << " in ShaderSeq::use is 0" << std::endl;
}
void ShaderSeq::unuse(int i)
{
  if (progs[i])
    progs[i]->unuse();
  else
    std::cout << "Error: progs[i] in ShaderSeq::unuse is 0" << std::endl;
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
  return s.substr(i,ii-i+1);
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

#if 0
struct AnimStateI
{
  AnimStateI(ShaderFile &f) : seq(f) { }
  VBOState vbostate;
  ShaderSeq seq;
  int s;
};
#endif

AnimState::AnimState(ShaderFile &f)
{
#if 0
  priv = new AnimStateI(f);
#endif
}
AnimState::~AnimState()
{
#if 0
  delete priv;
#endif
}

void UpdateAnim(Anim &anim, AnimState &state, const std::vector<Attrib> &attribs)
{
#if 0
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
#endif
}

void DrawAnim(float start, float end, AnimState &state, const std::vector<Attrib> &attribs)
{
#if 0
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
#endif
}

std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val)
{
  std::string res = mod->FunctionName();
  res+="(";
  int s = mod->NumArgs();
  for(int i=0;i<s;i++)
    {
      std::string argname = mod->ArgName(i);
      std::string value = mod->ArgValue(i);
      if (argname == name)
	{
	  value = val;
	}
      res += value;
      if (i!=s-1)
	res+=",";
    }
  res+=")";
  return res;
}

std::string funccall_to_string(ShaderModule *mod)
{
  std::string res = mod->FunctionName();
  res+="(";
  int s = mod->NumArgs();
  for(int i=0;i<s;i++)
    {
      res += mod->ArgValue(i);
      if (i!=s-1)
	res+=",";
    }
  res+=")";
  return res;
}
