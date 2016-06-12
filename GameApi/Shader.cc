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
#include <SDL_opengl.h>
#include "VectorTools.hh"
#include <vector>
#include <algorithm>
#include "Effect.hh"
#include <fstream>
#include "GraphI.hh"

std::string funccall_to_string(ShaderModule *mod);


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
  delete [] lengths;
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
      delete (*i);
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
void Program::bind_frag(int num, std::string name)
{
#ifndef EMSCRIPTEN
  glBindFragDataLocation( priv->program, num, name.c_str());
#endif
}
void Program::bind_attrib(int num, std::string name)
{
  //int val2 = glGetError();
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
Matrix Program::get_matrix_var(const std::string &name)
{
  GLint loc = glGetUniformLocation(priv->program, name.c_str());
  float mat[16];
  glGetUniformfv( priv->program, loc, &mat[0]);
  
  Matrix m = { { mat[0], mat[4], mat[8], mat[12],
		 mat[1], mat[5], mat[9], mat[13],
		 mat[2], mat[6], mat[10], mat[14],
		 mat[3], mat[7], mat[11], mat[15] } , false };
  return m;
  
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

#ifdef EMSCRIPTEN
#define OLD_SHADER 1
#endif
#ifdef RASBERRY
#define OLD_SHADER 1
#endif

ShaderFile::ShaderFile()
{
#ifdef OLD_SHADER
  std::string s =
"//V: comb\n"
"#version 100\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"uniform mat4 in_N;\n"
"uniform float in_POS;\n"
"uniform float in_time;\n"
"attribute vec3 in_InstPos;\n"
"attribute vec3 in_Position;\n"
"attribute vec3 in_Position2;\n"
"attribute vec3 in_Normal;\n"
"attribute vec4 in_Color;\n"
"attribute vec3 in_TexCoord;\n"
"varying vec3 ex_TexCoord;\n"
"varying vec4 ex_Color;\n"
    //"flat varying vec4 ex_FlatColor;\n"
"varying vec3 ex_Normal;\n"
"varying vec3 ex_Position;\n"
"vec4 recalc_normal(vec4 pos)\n"
"{\n"
    //"   mat4 inN = transpose(inverse(in_MV));\n"
"   ex_Normal = normalize((vec4(in_Normal,1.0)*in_N).xyz);\n"
"   return pos;\n"
"}\n"
"vec4 flat_shading(vec4 pos)\n"
"{\n"
    //"   ex_FlatColor = in_Color;\n"
"   return pos;\n"
"}\n"
"vec4 inst(vec4 pos)\n"
"{\n"
"   return pos + vec4(in_InstPos,0.0);\n"
"}\n"
"vec4 passall(vec4 pos)\n"
"{\n"
"  ex_Color = in_Color;\n"
"  ex_TexCoord = in_TexCoord;\n"
"  ex_Normal = in_Normal;\n"
"  ex_Position = in_Position;\n"
"  return pos;\n"
"}\n"
"vec4 point_light(vec4 pos)"
"{"
"   ex_Position = in_Position;"
"   return pos;"
"}"
"vec4 snoise(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"   return pos;\n"
"}\n"
"vec4 light(vec4 pos)\n"
"{\n"
"return pos;\n"
"}\n"
"vec4 ref(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"vec4 wave(vec4 pos)\n"
"{\n"
"  vec4 pos2 = pos + vec4(10.0*cos(2.0*in_time*in_Position.x*3.14159*2.0/360.0),\n"
"                         10.0*sin(3.0*in_time*in_Position.y*3.14159*2.0/360.0),\n"
"			 10.0*cos(5.0*in_time*in_Position.z*3.14159*2.0/360.0), 0.0);\n"
"  return pos2;\n"
"}\n"
"vec4 toon(vec4 pos)\n"
"{\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"vec4 texture(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"vec4 texture_arr(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"vec4 colour(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"vec4 empty(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"
"vec4 blur(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"\n"
"//V: empty\n"
"#version 100\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"attribute vec3 in_Position;\n"
"void main(void)\n"
"{\n"
"  vec4 v = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"  gl_Position = v;\n"
"}\n"
"\n"
"//F: comb\n"
"#version 100\n"
"precision mediump float;\n"
"varying vec4 ex_Color;\n"
    //"flat varying vec4 ex_FlatColor;\n"
    //"out vec4 out_Color;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"uniform mat4 in_N;\n"
"varying vec3 ex_TexCoord;\n"
"varying vec3 ex_Normal;\n"
"varying vec3 ex_Position;\n"
"uniform vec3 lightpos;\n"
"uniform vec3 light_dir;\n"
"uniform vec4 level1_color;\n"
"uniform vec4 level2_color;\n"
"uniform vec4 level3_color;\n"
"uniform sampler2D tex;\n"
    //"uniform sampler2DArray texarr;\n"
"vec4 white(vec4 rgb)\n"
"{\n"
"   return vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"vec4 diffuse(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = normalize(vec3(in_T*in_MV*vec4(ex_Normal,0.0)));\n"
"    vec3 LightDir_cam = vec3(in_T*in_MV * vec4(lightpos,1.0));\n"
"    vec3 light_dir_cam = normalize( LightDir_cam );\n"
"    float theta = dot( normal_cam, light_dir_cam ) ;\n"
"    return rgb + level1_color*theta;\n"
"}\n"
"vec4 ambient(vec4 rgb)\n"
"{\n"
"    return rgb + level2_color*vec4(0.2,0.2,0.2,1.0)*0.8;\n"
"}\n"
"vec4 specular(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = normalize(vec3(in_T*in_MV*vec4(ex_Normal,0.0)));\n"
"    vec3 LightDir_cam = vec3(in_T*in_MV * vec4(lightpos,1.0));\n"
"    vec3 light_dir_cam = normalize( LightDir_cam );\n"
"    vec3 R = reflect(-light_dir_cam, normal_cam);\n"
"    vec3 E = vec3(0.0,0.0,0.5);\n"
"    float alpha = dot(E,R);\n"
"    return rgb + level3_color * alpha*alpha*alpha*alpha*alpha*0.8;\n"
"}\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
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
"vec4 point_light(vec4 rgb)\n"
"{\n"
"   float dist = length(ex_Position.xyz - lightpos);\n"
"   float val = 100.0/dist;\n"
"   return vec4(val*vec3(rgb),rgb.a);\n"
"}\n"
"vec4 dot2(vec4 rgb)\n"
"{\n"
"   vec3 pos = vec3(ex_TexCoord.xy,1.0);\n"
"   //xpos = normalize(pos);\n"
"   return vec4(pos, 1.0);\n"
"}\n"
"vec4 snoise(vec4 rgb)\n"
"{\n"
"   vec2 coords = ex_Position.xy;\n"
"   float v = fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"   return vec4(mix(vec3(rgb), vec3(v,v,v), 0.2),rgb.a);\n"
"}\n"
"vec4 blur(vec4 rgb)\n"
"{\n"
"  vec2 t_mx = ex_TexCoord.xy + vec2(-0.005,0.0);\n"
"  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-0.005);\n"
"  vec2 t_px = ex_TexCoord.xy + vec2(0.005,0.0);\n"
"  vec2 t_py = ex_TexCoord.xy + vec2(0.0,0.005);\n"
"\n"
"   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
"   vec4 tex_mx = texture2D(tex, t_mx);	\n"
"   vec4 tex_my = texture2D(tex, t_my);	\n"
"   vec4 tex_px = texture2D(tex, t_px);	\n"
"   vec4 tex_py = texture2D(tex, t_py);\n"
"   vec4 t1 = mix(tex_mx, tex_px, 0.5);\n"
"   vec4 t2 = mix(tex_my, tex_py, 0.5);\n"
"   vec4 t12 = mix(t1,t2,0.5);\n"
"   vec4 t12t = mix(t12,tex2,0.5);\n"   	
"   return vec4(mix(vec3(rgb),vec3(t12t),t12t.a),t12t.a);\n"
"}\n"
"\n"
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
"vec4 light(vec4 rgb)\n"
"{\n"
"  return clamp(1.5*rgb, vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"vec4 wave(vec4 rgb)\n"
"{\n"
"  return rgb;\n"
"}\n"
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
"vec4 texture(vec4 rgb)\n"
"{\n"
"   return mix(rgb, texture2D(tex, ex_TexCoord.xy), 0.5);\n"
"}\n"
    //"vec4 texture_arr(vec4 rgb)\n"
    //"{\n"
    //"   return mix(rgb, texture2DArray(texarr, ex_TexCoord), 0.5);\n"
    //"}\n"
"vec4 colour(vec4 rgb)\n"
"{\n"
"  return mix(rgb, ex_Color, 0.5);    \n"
"}\n"
"vec4 empty(vec4 rgb)\n"
"{\n"
"   return rgb;\n"
"}\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"//V: screen\n"
"#version 100\n"
"precision mediump float;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"attribute vec3 in_Normal;\n"
"attribute vec3 in_Position;\n"
"varying vec2 XY;\n"
"\n"
"void main(void)\n"
"{\n"
"  XY = in_Normal.xy;\n"
"  gl_Position = in_P * in_T * in_MV *vec4(in_Position,1.0);\n"
"}\n"
"//F: screen\n"
"#version 100\n"
"precision mediump float;\n"
"varying vec2 XY;\n"
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
"void main(void)\n"
"{\n"
"  vec4 color;\n"
"  mainImage(color, XY);\n"
"  gl_FragColor = color;\n"
"}\n"
"//V: colour\n"
"#version 100\n"
"/*layout(location=0)*/ attribute vec3 in_Position;\n"
"/*layout(location=1)*/ attribute vec3 in_Normal;\n"
"/*layout(location=2)*/ attribute vec3 in_Color;\n"
"/*layout(location=3)*/ attribute vec3 in_TexCoord;\n"
"varying vec3 ex_Color;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"   ex_Color = in_Color;\n"
"}\n"
"//F: colour\n"
"#version 100\n"
"precision mediump float;\n"
"varying vec3 ex_Color;\n"
"void main(void)\n"
"{\n"
"   gl_FragColor = vec4(ex_Color,1.0);\n"
"}\n"
"\n"
"//F: empty\n"
"#version 100\n"
"precision mediump float;\n" 
"varying vec3 ex_Color;\n"
"void main(void)\n"
"{\n"
"   gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"\n"
"//V: texture\n"
"#version 100\n"
"/*layout(location=0)*/ in vec3 in_Position;\n"
"/*layout(location=1)*/ in vec3 in_Normal;\n"
"/*layout(location=2)*/ in vec3 in_Color;\n"
"/*layout(location=3)*/ in vec3 in_TexCoord;\n"
"out vec3 ex_TexCoord;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV *  vec4(in_Position,1.0);\n"
"   ex_TexCoord = in_TexCoord;\n"
"}\n"
"//F: texture\n"
"#version 100\n"
"//precision mediump float;\n"
"uniform sampler2D tex;\n"
"attribute vec3 ex_TexCoord;\n"
    //" vec4 out_Color;\n"
"void main(void)\n"
"{\n"
"   gl_FragColor = texture2D(tex, ex_TexCoord.xy);\n"
  "}\n";
#else
  std::string s =
"//V: comb\n"
"#version 330\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"uniform mat4 in_N;\n"
"uniform float in_POS;\n"
"uniform float in_time;\n"
"in vec3 in_InstPos;\n"
"in vec3 in_Position;\n"
"in vec3 in_Position2;\n"
"in vec3 in_Normal;\n"
"in vec4 in_Color;\n"
"in vec3 in_TexCoord;\n"
"out vec3 ex_TexCoord;\n"
"out vec4 ex_Color;\n"
"out vec3 ex_Normal;\n"
"out vec3 ex_Position;\n"
    //"flat out vec4 ex_FlatColor;\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
"vec4 recalc_normal(vec4 pos)\n"
"{\n"
    //"   mat4 inN = transpose(inverse(in_MV));\n"
"   ex_Normal = normalize((vec4(in_Normal,1.0)*in_N).xyz);\n"
"   return pos;\n"
"}\n"
"vec4 flat_shading(vec4 pos)\n"
"{\n"
    //"   ex_FlatColor = in_Color;\n"
"   return pos;\n"
"}\n"
"vec4 inst(vec4 pos)\n"
"{\n"
"   return pos + vec4(in_InstPos,0.0);\n"
"}\n"
"vec4 passall(vec4 pos)\n"
"{\n"
"  ex_Color = in_Color;\n"
"  ex_TexCoord = in_TexCoord;\n"
"  ex_Normal = in_Normal;\n"
"  ex_Position = in_Position;\n"
"  return pos;\n"
"}\n"
"vec4 point_light(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"   return pos;\n"
"}\n"
"vec4 snoise(vec4 pos)\n"
"{\n"
"   ex_Position = in_Position;\n"
"   return pos;\n"
"}\n"
"vec4 light(vec4 pos)\n"
"{\n"
"return pos;\n"
"}\n"
"vec4 ref(vec4 pos)\n"
"{\n"
"  ex_Position = in_Position;\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"vec4 wave(vec4 pos)\n"
"{\n"
"  vec4 pos2 = pos + vec4(10.0*cos(2.0*in_time*in_Position.x*3.14159*2.0/360.0),\n"
"                         10.0*sin(3.0*in_time*in_Position.y*3.14159*2.0/360.0),\n"
"			 10.0*cos(5.0*in_time*in_Position.z*3.14159*2.0/360.0), 0.0);\n"
"  return pos2;\n"
"}\n"
"vec4 toon(vec4 pos)\n"
"{\n"
"  ex_Normal = in_Normal;\n"
"  return pos;\n"
"}\n"
"vec4 texture(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"vec4 texture_arr(vec4 pos)\n"
"{\n"
"  ex_TexCoord = in_TexCoord;\n"
"  return pos;\n"
"}\n"
"vec4 colour(vec4 pos)\n"
"{\n"
"   ex_Color = in_Color;\n"
"   return pos;\n"
"}\n"
"vec4 blur(vec4 pos)"
"{"
"  ex_TexCoord = in_TexCoord;"
"  return pos;"
"}"
"vec4 empty(vec4 pos)\n"
"{\n"
"   return pos;\n"
"}\n"
"void main(void)\n"
"{\n"
"//C:\n"
"}\n"
"\n"
"//F: comb\n"
"#version 330\n"
"#extension GL_EXT_texture_array : enable\n"
"precision mediump float;\n"
"in vec4 ex_Color;\n"
    //"flat in vec4 ex_FlatColor;\n"
"out vec4 out_Color;\n"
"in vec3 ex_TexCoord;\n"
"in vec3 ex_Normal;\n"
"in vec3 ex_Position;\n"
"uniform mat4 in_P;\n"
"uniform mat4 in_MV;\n"
"uniform mat4 in_T;\n"
"uniform mat4 in_N;\n"
"uniform sampler2D tex;\n"
"uniform sampler2DArray texarr;\n"
"uniform vec3 lightpos;\n"
"uniform vec3 light_dir;\n"
"uniform vec4 level1_color;\n"
"uniform vec4 level2_color;\n"
"uniform vec4 level3_color;\n"
"vec4 white(vec4 rgb)\n"
"{\n"
"   return vec4(1.0,1.0,1.0,1.0);\n"
"}\n"
"vec4 diffuse(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = normalize(vec3(in_T*in_MV*vec4(ex_Normal,0.0)));\n"
"    vec3 LightDir_cam = vec3(in_T*in_MV * vec4(lightpos,1.0));\n"
"    vec3 light_dir_cam = normalize( LightDir_cam );\n"
"    float theta = clamp( dot( normal_cam, light_dir_cam ) , 0,1);\n"
"    return rgb + level1_color*theta;\n"
"}\n"
"vec4 ambient(vec4 rgb)\n"
"{\n"
"    return rgb + level2_color*vec4(0.2,0.2,0.2,1.0)*0.8;\n"
"}\n"
"vec4 specular(vec4 rgb)\n"
"{\n"
"    vec3 normal_cam = normalize(vec3(in_T*in_MV*vec4(ex_Normal,0.0)));\n"
"    vec3 LightDir_cam = vec3(in_T*in_MV * vec4(lightpos,1.0));\n"
"    vec3 light_dir_cam = normalize( LightDir_cam );\n"
"    vec3 R = reflect(-light_dir_cam, normal_cam);\n"
"    vec3 E = vec3(0.0,0.0,0.5);\n"
"    float alpha = clamp( dot(E,R), 0,1);\n"
"    return rgb + level3_color * pow(alpha,5)*0.8;\n"
"}\n"
"vec4 color_from_normals(vec4 rgb)\n"
"{\n"
"    return vec4(ex_Normal*0.5,1.0)+vec4(0.5,0.5,0.5,0.0);\n"
"}\n"
"vec4 flat_shading(vec4 rgb)\n"
"{\n"
    //"   return mix(rgb,ex_FlatColor,0.5);\n"
"   return rgb;\n"
"}\n"
"vec4 point_light(vec4 rgb)"
"{"
"   float dist = length(ex_Position.xyz - lightpos);"
"   float val = 100.0/dist;"
"   return vec4(val*vec3(rgb),rgb.a);"
"}"
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
"vec4 dot2(vec4 rgb)\n"
"{\n"
"   vec3 pos = vec3(ex_TexCoord.xy,1.0);\n"
"   //xpos = normalize(pos);\n"
"   return vec4(pos, 1.0);\n"
"}\n"
"vec4 snoise(vec4 rgb)\n"
"{\n"
"   vec2 coords = ex_Position.xy;\n"
"   float v = fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"   return vec4(mix(vec3(rgb), vec3(v,v,v), 0.2),rgb.a);\n"
"}\n"
"vec4 blur(vec4 rgb)\n"
"{\n"
"  vec2 t_mx = ex_TexCoord.xy + vec2(-0.005,0.0);\n"
"  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-0.005);\n"
"  vec2 t_px = ex_TexCoord.xy + vec2(0.005,0.0);\n"
"  vec2 t_py = ex_TexCoord.xy + vec2(0.0,0.005);\n"
"\n"
"   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
"   vec4 tex_mx = texture2D(tex, t_mx);	\n"
"   vec4 tex_my = texture2D(tex, t_my);	\n"
"   vec4 tex_px = texture2D(tex, t_px);	\n"
"   vec4 tex_py = texture2D(tex, t_py);\n"
"   vec4 t1 = mix(tex_mx, tex_px, 0.5);\n"
"   vec4 t2 = mix(tex_my, tex_py, 0.5);\n"
"   vec4 t12 = mix(t1,t2,0.5);\n"
"   vec4 t12t = mix(t12,tex2,0.5);\n"   	
"   return vec4(mix(vec3(rgb),vec3(t12t),t12t.a),t12t.a);\n"
"}\n"
"\n"
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
"vec4 light(vec4 rgb)\n"
"{\n"
"  return clamp(1.5*rgb, vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));\n"
"}\n"
"vec4 wave(vec4 rgb)\n"
"{\n"
"  return rgb;\n"
"}\n"
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
"vec4 texture(vec4 rgb)\n"
"{\n"
"   return mix(rgb, texture2D(tex, ex_TexCoord.xy), 0.5);\n"
"}\n"
"vec4 texture_arr(vec4 rgb)\n"
"{\n"
"   return mix(rgb, texture2DArray(texarr, ex_TexCoord), 0.5);\n"
"}\n"
"vec4 colour(vec4 rgb)\n"
"{\n"
"  return mix(rgb, ex_Color, 0.5);    \n"
"}\n"
"vec4 empty(vec4 rgb)\n"
"{\n"
"   return rgb;\n"
"}\n"
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
"void main(void)\n"
"{\n"
"  vec4 v = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"  gl_Position = v;\n"
"}\n"
"\n"
"//F: empty\n"
"#version 330\n"
"precision mediump float;\n"
"out vec4 out_Color;\n"
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
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV * vec4(in_Position,1.0);\n"
"   ex_Color = in_Color;\n"
"}\n"
"//F: colour\n"
"#version 330\n"
"//precision mediump float;\n"
"in vec3 ex_Color;\n"
"out vec4 out_Color;\n"
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
"void main(void)\n"
"{\n"
"   gl_Position = in_P * in_T * in_MV *  vec4(in_Position,1.0);\n"
"   ex_TexCoord = in_TexCoord;\n"
"}\n"
"//F: texture\n"
"#version 330\n"
"//precision mediump float;\n"
"uniform sampler2D texture;\n"
"in vec3 ex_TexCoord;\n"
"out vec4 out_Color;\n"
"void main(void)\n"
"{\n"
"   out_Color = texture2D(texture, ex_TexCoord.xy);\n"
  "}\n";
#endif

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
std::string replace_c(std::string s, std::vector<std::string> comb, bool is_fragment, bool is_fbo, bool is_transparent, ShaderModule *mod)
{
  std::stringstream ss(s);
  std::string ww;
  std::string out;
  while(std::getline(ss,ww))
    {
      if (mod && ww.substr(0,4)=="//M:")
	{
	  if (is_fragment)
	    {
	      out+=mod->Function();
	    }
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
		  out+=comb[i];
		  out+="(pos";
		  out+=ss2.str();
		  out+=");\n";
		}
	      std::stringstream ss3;
	      ss3 << s;
	      out+="gl_Position = in_P * in_T * in_MV * pos";
	      out+=ss3.str();
	      out+=";\n";
	    }
	  else
	    { // fragment
	      if (is_transparent)
		out+="vec4 rgb0 = vec4(0.0,0.0,0.0,0.0);\n";
	      else
		out+="vec4 rgb0 = vec4(0.0,0.0,0.0,1.0);\n";
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
	      
#ifdef OLD_SHADER
	      if (is_fbo)
		out += "gl_FragData[0] = rgb";
	      else
		out+="gl_FragColor = rgb";
#else
	      out+="out_Color = rgb";
#endif
	      out+= ss3.str();
	      out+=";\n";
	    }

	}
      else
	{
	  out+=ww;
	  out+= "\n";
	}
    }
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
int ShaderSeq::GetShader(std::string v_format, std::string f_format, std::string g_format, std::vector<std::string> v_vec, std::vector<std::string> f_vec, bool is_trans, ShaderModule *mod)
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
      std::string ss = replace_c(shader, v_vec, false, false, is_trans, mod);
      
      //std::cout << "::" << ss << "::" << std::endl;
      ShaderSpec *spec = new SingletonShaderSpec(ss);
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
      std::string ss = replace_c(shader, f_vec, true, false,is_trans, mod);
      std::cout << "::" << add_line_numbers(ss) << "::" << std::endl;
      ShaderSpec *spec = new SingletonShaderSpec(ss);
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
      //std::cout << "::" << shader << "::" << std::endl;
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
