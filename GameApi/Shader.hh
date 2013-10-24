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


#ifndef SHADER_HH
#define SHADER_HH
#include <string>
#include <vector>
#include <sstream>
#include "VectorTools.hh"
#include <algorithm>
#include <map>

struct Attrib
{
  int loc;
  int id;
  bool is_int; // true = int, false = float
};


class ShaderSpec
{
public:
  virtual std::string Name() const=0;
  virtual int Count() const=0;
  virtual std::string Source(int i) const=0;
};
class SingletonShaderSpec : public ShaderSpec
{
public:
  SingletonShaderSpec(std::string s) : s(s) { }
  std::string Name() const { return ""; }
  int Count() const { return 1; }
  std::string Source(int i) const { return s; }
private:
  std::string s;
};

struct ShaderPriv;
class Shader
{
public:
  Shader(ShaderSpec &spec, bool vertex, bool geom);
  ~Shader();
public:
  ShaderPriv *priv;

};

struct ProgramPriv;
class Program
{
public:
  Program();
  void push_back(const Shader &shader);
  void detach(const Shader &shader);
  void link();
  void use();
  void unuse();
  ~Program();
public: // geometry shader specific
  void GeomTypes(int input, int output); // 0 = points, 1 = lines, 2 = triangles
  void GeomOutputVertices(int i);
public: // uniform variable
  int get_loc(std::string name);
  void set_var(const std::string &name, float val);
  void set_var(int name, float val);
  void set_var(const std::string &name, float val1, float val2);
  void set_var(const std::string &name, const Point &p);
  void set_var(const std::string &name, const Vector &p);
  void set_var(const std::string &name, const Color &c);
  void set_var(const std::string &name, float val1, float val2, float val3, float val4);
  void set_var(int name, float val1, float val2, float val3, float val4);
  void set_var(const std::string &name, Matrix m);
  void set_var(const std::string &name, float *array, int count);
  void set_var(const std::string &name, int *array, int count);
  void set_var(const std::string &name, int val);
  
  void bind_attrib(int num, std::string name);
  Attrib find_attr(const std::string &attr_name, int id);
  Attrib find_attr_int(const std::string &attr_name, int id);
  void attr_loc(std::string s, int index);
public:
  ProgramPriv *priv;
};

class TestVertexShader : public ShaderSpec
{
public:
  std::string Name() const { return "testshader"; }
  int Count() const { return 1; }
  std::string Source(int i) const { return 
				"void main(void)\n"
				"{\n"
				"   gl_position = ftransform();\n"
				"}\n";
  }
};
class TestFragmentShader : public ShaderSpec
{
public:
  std::string Name() const { return "testfshader"; }
  int Count() const { return 1; }
  std::string Source(int i) const { return 
				"void main(void)\n"
				"{\n"
				"   gl_FragColor=vec4(1.0,0.0,0.0,1.0);\n"
				"}\n";
  }
};


class ShaderString
{
public:
  virtual std::string VertexShader() const=0;
  virtual std::string FragmentShader() const=0;
};


class ShaderStringImpl : public ShaderString
{
public:
  // whole shader
  virtual std::string VertexShader() const { return VertexGlobals() + "void main() { \n" + VertexMain()+"}\n"; }
  virtual std::string FragmentShader() const { return FragmentGlobals()+"void main() {\n"+FragmentMain()+"}\n";}

  // Vertex shader
  virtual std::string VertexGlobals() const { return ""; }
  virtual std::string VertexMain() const { return VertexDecls()+VertexCode()+VertexAssigns(); }

  // Fragment shader
  virtual std::string FragmentGlobals() const { return ""; }
  virtual std::string FragmentMain() const { return FragmentDecls()+FragmentCode()+FragmentAssigns(); }

  // Vertex shader parts
  virtual std::string VertexDecls() const { return ""; }
  virtual std::string VertexCode() const { return ""; }
  virtual std::string VertexAssigns() const { return "gl_Position = " + VertexPosition() + ";"; }

  // Fragment shader parts
  virtual std::string FragmentDecls() const { return ""; }
  virtual std::string FragmentCode() const { return ""; }
  virtual std::string FragmentAssigns() const { return "gl_FragColor = " + FragmentColor() + ";"; }
  
  // Vertex shader
  virtual std::string VertexPosition() const { return "ftransform()"; }
  
  // Fragment shader
  virtual std::string FragmentColor() const { return "vec4(1.0,0.0,0.0,1.0)"; }		 
};

class VertexShader : public ShaderSpec
{
public:
  VertexShader(ShaderString &shader) : shader(shader) { }
  std::string Name() const { return "Shader"; }
  int Count() const { return 1; }
  std::string Source(int i) const
  {
    std::cout << shader.VertexShader() << std::endl;
    return shader.VertexShader();
  }
private: 
  ShaderString &shader;
};

class FragmentShader : public ShaderSpec
{
public:
  FragmentShader(ShaderString &shader) : shader(shader) { }
  std::string Name() const { return "Shader"; }
  int Count() const { return 1; }
  std::string Source(int i) const
  {
    std::cout << shader.FragmentShader() << std::endl;
    return shader.FragmentShader();
  }
private:
  ShaderString &shader;
};

class ArrayShader : public ShaderSpec
{
public:
  ArrayShader() { }
  std::string Name() const { return "Array"; }
  int Count() const { return shaders.size(); }
  std::string Source(int i) const
  {
    return shaders[i];
  }
  void push_back(std::string s) { shaders.push_back(s); }
private:
  std::vector<std::string> shaders;
};

class ShaderParameters
{
public:
  virtual void SetParameters(Program &p) const=0;
};
class ShaderAttributes
{
public:
  virtual void SetAttribs(Program &p) const=0;
};

class CombineParameters : public ShaderParameters
{
public:
  CombineParameters() : p1(0), p2(0) { }
  CombineParameters(ShaderParameters &p1_, ShaderParameters &p2_) : p1(&p1_), p2(&p2_) { }
  void SetParameters(Program &p) const
  {
    p1->SetParameters(p);
    p2->SetParameters(p);
  }
  void SetValues(ShaderParameters *p1_, ShaderParameters *p2_) { p1 = p1_; p2 = p2_; }
private:
  ShaderParameters *p1, *p2;
};

class ShaderPair
{
public:
  ShaderPair(ShaderString &shader) : /*shader(shader),*/ vs(shader), fs(shader), vertex(vs, true, false), fragment(fs,false, false)
  {
    prog.push_back(vertex);
    prog.push_back(fragment);
    prog.link();
  }
  void use() { prog.use(); }
  void set_params(ShaderParameters &p) 
  { 
    p.SetParameters(prog);
  }
  void set_attribs(ShaderAttributes &p)
  {
    p.SetAttribs(prog);
  }
  void unuse() { prog.unuse(); }
  int get_loc(std::string name) { return prog.get_loc(name); }
private:
  //ShaderString &shader;
  Program prog;
  VertexShader vs;
  FragmentShader fs;
  Shader vertex, fragment;
};

class PhongParameters : public ShaderParameters
{
public:
  PhongParameters(Point viewer, Point light, Color light_color, Color speccolor) : viewer(viewer), light(light), light_color(light_color), speccolor(speccolor) { }
  virtual void SetParameters(Program &p) const
  {
    Point halfway = HalfWay(viewer, light);
    p.set_var("halfway", halfway);
    p.set_var("light", light);
    p.set_var("lightcolor", light_color);
    p.set_var("speccolor", speccolor);  
  }

  Point HalfWay(Point viewer, Point light) const
  {
    Vector p = Vector(viewer)+Vector(light);
    Vector hw = p/p.Dist();
    return Point(hw);
  }
private:
  Point viewer, light;
  Color light_color, speccolor;
};

//
// Shader representation
//

enum ShaderProvides
  {
    ReqNormal,
    ReqFragColorRGB,
    ReqFragColorA,
    ReqPosition,
    ReqTexCoord0,
    ReqFragColorCombiner
  };

class ShaderPiece
{
public:
  virtual std::string VertexGlobals() const=0;
  virtual std::string VertexCodeGen() const=0;
  virtual std::string FragmentGlobals() const=0;
  virtual std::string FragmentCodeGen() const=0;
  virtual void Provides(std::vector<int> &vec) const { }
  virtual void Config(const std::vector<int> &env) { }
};


class GouraudPiece : public ShaderPiece
{
public:
  GouraudPiece() : fragnum(0) { }
  void Provides(std::vector<int> &vec) const
  {
    for(unsigned int i=0;i<vec.size();i++)
      {
	if (vec[i]==ReqFragColorRGB) fragnum++;
      }

    vec.push_back(ReqFragColorRGB);
  }
  virtual std::string VertexGlobals() const
  {
    return 
      "uniform vec3 halfway;\n"
      "uniform vec3 light;\n"
      "uniform vec3 lightcolor;\n"
      "uniform vec3 speccolor;\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "vec3 n = normalize(gl_Normal);\n"
      "float col = dot(n, halfway);\n"
      "float col2 = dot(n, light);\n"
      "col = col*col;\n"
      "if (col < 0.0) col = 0.0;\n"
      "if (col > 1.0) col = 1.0;\n"
      "if (col2 < 0.0) col2 = 0.0;\n"
      "if (col2 > 1.0) col2 = 1.0;\n"
      "float r = col*lightcolor.r+col2*speccolor.r;\n"
      "float g = col*lightcolor.g+col2*speccolor.g;\n"
      "float b = col*lightcolor.b+col2*speccolor.b;\n"
      "if (r > 1.0) r=1.0;\n"
      "if (g > 1.0) g=1.0;\n"
      "if (b > 1.0) b=1.0;\n" 
      "vec3 vv = vec3(r,g,b);\n"
      "gl_FrontColor.rgb = vv;\n"
      "gl_BackColor.rgb = vv;\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    std::stringstream s;
    s << fragnum;
    return 
      std::string("vec4 FragColor") + s.str() + ";\n"
      "FragColor" + s.str() +".rgb=gl_Color.rgb;\n";

  }
private:
  mutable int fragnum;
};

class NormalParameters : public ShaderParameters
{
public:
  NormalParameters(Point p, float mix_rate) : p(p), mix_rate(mix_rate) { }
  virtual void SetParameters(Program &pp) const
  {
    pp.set_var("normal_center", p);
    pp.set_var("mix_rate", mix_rate);
  }
private:
  Point p;
  float mix_rate;
};
class NormalPiece : public ShaderPiece
{
public:
  void Provides(std::vector<int> &vec) const
  {
    vec.push_back(ReqNormal);
  }
  virtual std::string VertexGlobals() const
  {
    return 
      "uniform vec3 normal_center;\n"
      "uniform float mix_rate;\n"
      "varying vec3 normal;\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return "vec3 vl = gl_NormalMatrix * gl_Normal;\n"
      "vec3 vk = (gl_ModelViewMatrix*gl_Vertex).xyz-normal_center;\n"
      "vl = normalize(vl);\n"
      "vk = normalize(vk);\n"
      "normal=mix(vk, vl, mix_rate);\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
};

class PhongPiece : public ShaderPiece
{
public:
  PhongPiece() : normal(true), fragnum(0) { }
  virtual void Config(const std::vector<int> &env) 
  {
    for(std::vector<int>::const_iterator i=env.begin();i!=env.end();i++)
      {
	if (*i==ReqNormal) normal = false;
      }
  }
  void Provides(std::vector<int> &vec) const
  {
    for(unsigned int i=0;i<vec.size();i++)
      {
	if (vec[i]==ReqFragColorRGB) fragnum++;
      }
    vec.push_back(ReqFragColorRGB);
  }

  virtual std::string VertexGlobals() const
  {
    if (normal)
      return "varying vec3 normal;\n";
    else
      return "";
  }
  virtual std::string VertexCodeGen() const
  {
    if (normal)
      return "normal=gl_NormalMatrix * gl_Normal;\n";
    else
      return "";
  }
  virtual std::string FragmentGlobals() const
  {
    return 
      "varying vec3 normal;\n"
      "uniform vec3 halfway;\n"
      "uniform vec3 light;\n"
      "uniform vec3 lightcolor;\n"
      "uniform vec3 speccolor;\n";
  }
  virtual std::string FragmentCodeGen() const
  {
    std::stringstream s;
    s << fragnum;
    return
      std::string(
		  "vec3 n = normalize(normal);\n"
		  "float col = dot(n, halfway);\n"
		  "float col2 = dot(n, light);\n"
		  "col = col*col;\n"
		  "col = col*col;\n"
		  //"col = col*col;\n"
		  //"col = col*col;\n"
		  //"col = col*col;\n"
		  //"col = col*col;\n"
		  "if (col < 0.0) col = 0.0;\n"
		  "if (col > 1.0) col = 1.0;\n"
		  "if (col2 < 0.0) col2 = 0.0;\n"
		  "if (col2 > 1.0) col2 = 1.0;\n"
		  "float r = col*lightcolor.r+col2*speccolor.r;\n"
		  "float g = col*lightcolor.g+col2*speccolor.g;\n"
		  "float b = col*lightcolor.b+col2*speccolor.b;\n"
		  "if (r > 1.0) r=1.0;\n"
		  "if (g > 1.0) g=1.0;\n"
		  "if (b > 1.0) b=1.0;\n" 
		  "vec4 FragColor") + s.str() + ";\n"
		  "FragColor" + s.str() + ".r = r;\n"
                  "FragColor" + s.str() + ".g = g;\n"
                  "FragColor" + s.str() + ".b = b;\n";
  }
private:
  bool normal;
  mutable int fragnum;
};
class EmptyVertexPiece : public ShaderPiece
{
public:
  virtual std::string VertexGlobals() const
  {
    return "";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "gl_Position = ftransform();\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
};

class EmptyRGBPiece : public ShaderPiece
{
public:
  virtual std::string VertexGlobals() const
  {
    return "";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return 
      "gl_FragColor.r=1.0;\n"
      "gl_FragColor.g=0.0;\n"
      "gl_FragColor.b=0.0;\n";
  }
};


class EmptyAPiece : public ShaderPiece
{
public:
  virtual std::string VertexGlobals() const
  {
    return "";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return 
      "gl_FragColor.a=1.0;\n";
  }
};




class DiscardPiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const 
  {
    vec.push_back(ReqFragColorA);
  }
  virtual void Config(const std::vector<int> &env) { }

  virtual std::string VertexGlobals() const
  {
    return "varying float dis;\n"
      "attribute float centerx;\n"
      "attribute float centery;\n"
      "attribute float centerz;\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "vec3 v = vec3(centerx,centery,centerz);\n"
      "dis = v.x*v.x+v.y*v.y+v.z*v.z;\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return 
      "varying float dis;\n"
      "uniform float distance;\n";
  }
  virtual std::string FragmentCodeGen() const
  {
    return //"if (dis>distance) discard;\n"
    "gl_FragColor.a = dis-distance;\n";
  }
};

class DiscardParameters : public ShaderParameters
{
public:
  DiscardParameters(float distance) : distance(distance) { }
  void SetParameters(Program &p) const
  {
    p.set_var("distance", distance);
  }
private:
  float distance;
};

class DiscardAttributes : public ShaderAttributes
{
public:
  DiscardAttributes(int centerx, int centery, int centerz) : centerx(centerx), centery(centery), centerz(centerz) { }
  void SetAttribs(Program &p) const
  {
    x = p.find_attr("centerx", centerx);
    y = p.find_attr("centery", centery);
    z = p.find_attr("centerz", centerz);
  }
  void Attr(std::vector<Attrib> &vec)
  {
    vec.push_back(x);
    vec.push_back(y);
    vec.push_back(z);
  }
private:
  mutable Attrib x,y,z;
  int centerx, centery, centerz;
};

// Does not work!!
class RandomPositionPiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const 
  {
    vec.push_back(ReqPosition);
  }

  virtual std::string VertexGlobals() const
  {
    return 
      "uniform vec3 box;\n"
      "uniform float time;\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "   vec3 n = vec3(0.0,0.0,0.0); //noise3(gl_Vertex+vec4(time,time,time,time));\n"
      "   vec4 vertex = gl_Vertex + vec4(box.x*n.x, box.y*n.y, box.z*n.z,0.0);\n"
      "   gl_Position = gl_ModelViewProjectionMatrix * vertex;\n";
    
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
};
class RandomPositionParameters : public ShaderParameters
{
public:
  RandomPositionParameters(Point box, float time) : box(box), time(time) { }
  virtual void SetParameters(Program &p) const
  {
    p.set_var("box", box);
    p.set_var("time", time);
  }

private:
  Point box;
  float time;
};
class BlobParameters : public ShaderParameters
{
public:
  BlobParameters(Point center, Point center2, float r) : center(center), center2(center2), r(r) { }
  void set_points(Point c, Point c2, float r) { center = c; center2=c2; this->r=r; }
  void SetParameters(Program &p) const
  {
    p.set_var("center1x", center.x);
    p.set_var("center1y", center.y);
    p.set_var("center1z", center.z);
    p.set_var("center2x", center2.x);
    p.set_var("center2y", center2.y);
    p.set_var("center2z", center2.z);
    p.set_var("radius", r);
  }
  virtual ~BlobParameters() { }
private:
  Point center, center2;
  float r;
};

class BlobPiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const 
  {
    vec.push_back(ReqPosition);
  }

  virtual std::string VertexGlobals() const
  {
    return 
      "vec4 line(float t, vec4 x_0, vec4 x_1) { return x_0+(x_1-x_0)*t; }\n"
      "float f(vec4 p, vec4 x_0, vec4 x_1, vec4 x_2) { return ((p.x-x_0.x)*(p.x-x_0.x)+(p.y-x_0.y)*(p.y-x_0.y)+(p.z-x_0.z)*(p.z-x_0.z)); }\n"
      "float g(vec4 p, vec4 x_0, vec4 x_1, vec4 x_2) { return ((p.x-x_2.x)*(p.x-x_2.x)+(p.y-x_2.y)*(p.y-x_2.y)+(p.z-x_2.z)*(p.z-x_2.z)); }\n"
      "float h(vec4 p, vec4 x_0, vec4 x_1, vec4 x_2, float r)\n"
      "{\n"
      "    float fp = f(p, x_0, x_1, x_2);\n"
      "    float gp = g(p, x_0, x_1, x_2);\n"
      "    if (fp < 1.0) fp = 1.0;\n"
      "    if (gp < 1.0) gp = 1.0;\n"
      "    return 1.0/(fp)+1.0/(gp)-r;\n"
      //"return gp; \n"
      "  }\n"
      "float H(float tt, vec4 x_0, vec4 x_1, vec4 x_2, float r) \n"
      "  { \n"
      "    return h(line(tt, x_0, x_1), x_0, x_1, x_2, r); \n"
      "  }\n"
      "  float t_func(float t_0, float t_1, vec4 x_0, vec4 x_1, vec4 x_2, float r)\n"
      "  {\n"
      "    float t=t_0;\n"
      "    //for(;t<t_1;)\n"
      "      {\n"
      "	float Ht = H(t, x_0, x_1, x_2, r);\n"
      "	if (Ht<0.1) return t;\n"
      "	t+=Ht/20.0;\n"
      "	Ht = H(t, x_0, x_1, x_2, r);\n"
      "	if (Ht<0.1) return t;\n"
      "	t+=Ht/20.0;\n"
      "	Ht = H(t, x_0, x_1, x_2, r);\n"
      "	if (Ht<0.1) return t;\n"
      "	t+=Ht/20.0;\n"
      "	Ht = H(t, x_0, x_1, x_2, r);\n"
      "	if (Ht<0.1) return t;\n"
      "	t+=Ht/20.0;\n"
      "      }\n"
      " return t;\n"
      "}\n"
      "vec4 adjustpoint(vec4 x_0, vec4 x_1, vec4 x_2, float r)\n"
      "{\n"
      "    float tt = t_func(1.0,5.0, x_0, x_1, x_2, r);\n"
      "    return line(tt, x_0, x_1);\n"
      //" return gl_Vertex;\n"
      "}\n"
      "uniform float center1x;\n"
      "uniform float center1y;\n"
      "uniform float center1z;\n"
      "uniform float center2x;\n"
      "uniform float center2y;\n"
      "uniform float center2z;\n"
      "uniform float radius;\n"
      ;
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "vec4 center1 = vec4(center1x, center1y, center1z,0.0);\n"
      "vec4 center2 = vec4(center2x, center2y, center2z,0.0);\n"
      "vec4 vertex = gl_Vertex;\n"
      "vertex.x += center1.x;\n"
      "vertex.y += center1.y;\n"
      "vertex.z += center1.z;\n"
      "vec4 p = adjustpoint(center1,vertex, center2, radius);\n"	    
      "p.x *= 40.0;\n"
      "p.y *= 40.0;\n"
      "p.z *= 40.0;\n"
      "gl_Position = gl_ModelViewProjectionMatrix * p;\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
};
class TextureParameters : public ShaderParameters
{
public:
  TextureParameters(int textureunit) : textureunit(textureunit) { }
  void SetParameters(Program &p) const
  {
    p.set_var("tex", textureunit);
  }
private:
  int textureunit;
};
class GlobalTextureParameters : public ShaderParameters
{
public:
  GlobalTextureParameters(Point p, Vector x, Vector y, Vector z) : p(p), x(x), y(y), z(z) { }
  void SetParameters(Program &pp) const
  {
    pp.set_var("point", p);
    pp.set_var("x_vec", x);
    pp.set_var("y_vec", y);
    //pp.set_var("z_vec", z);
  }
private:
  Point p;
  Vector x;
  Vector y;
  Vector z;
};

class GlobalTexturePiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const
  {
    vec.push_back(ReqTexCoord0);
  }
  virtual std::string VertexGlobals() const
  {
    return 
      "uniform vec3 point;\n"
      "uniform vec3 x_vec;\n"
      "uniform vec3 y_vec;\n"
      //"uniform vec3 z_vec;\n"
      ;
  }
  virtual std::string VertexCodeGen() const
  {
    return
      "vec3 c = gl_Vertex.xyz;\n"
      "vec3 v = c - point;\n"
      "float d1 = dot(v,x_vec);\n"
      "float d2 = dot(v,y_vec);\n"
      //"float d3 = dot(v,z_vec);\n"
      "float xdist = length(x_vec);\n"
      "float ydist = length(y_vec);\n"
      //"float zdist = length(z_vec);\n"
      "float d1a = d1 / xdist / xdist*512;\n"
      "float d1b = d2 / ydist / ydist*512;\n"
      //"float d1c = d3 / zdist / zdist*512;\n"
      "gl_TexCoord[0] = vec4(d1a, d1b,0.0,0.0);\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return
      "";
  }
  
};
class FragColorCombinerPiece : public ShaderPiece
{
public:
  FragColorCombinerPiece() : fragnum(0) { }
  virtual void Config(const std::vector<int> &env)
  {
    for(std::vector<int>::const_iterator i=env.begin();i!=env.end();i++)
      {
	if (*i == ReqFragColorRGB) fragnum++;
      }
  }
  virtual void Provides(std::vector<int> &vec) const
  {
    vec.push_back(ReqFragColorCombiner);
  }
  virtual std::string VertexGlobals() const
  {
    return "";
  }
  virtual std::string VertexCodeGen() const
  {
      return "";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    std::string s;
    if (fragnum > 0)
      {
	s+= "vec4 f0 = FragColor0;\n";
      }
    for(int i=1;i<fragnum;i++)
      {
	std::stringstream ss;
	ss << i;
	std::stringstream ss1;
	ss1 << i-1;
	s += std::string("vec4 f") + ss.str() + " = max(f" + ss1.str() + ",FragColor" + ss.str() + ");\n";
      }
    std::stringstream frag;
    frag << fragnum-1;
    if (fragnum > 0)
      {
	s+=std::string("gl_FragColor.rgb = f") + frag.str() + ".rgb;\n";
      }
    return s; 
  }

private:
  int fragnum;
};

class TexturePiece : public ShaderPiece
{
public:
  TexturePiece() : coord(true), fragnum(0) { }
  virtual void Config(const std::vector<int> &env)
  {
    for(std::vector<int>::const_iterator i=env.begin();i!=env.end();i++)
      {
	if (*i==ReqTexCoord0) coord = false;
      }
  }
  virtual void Provides(std::vector<int> &vec) const
  {
    for(unsigned int i=0;i<vec.size();i++)
      {
	if (vec[i]==ReqFragColorRGB) fragnum++;
      }
    vec.push_back(ReqFragColorRGB);
  }
  virtual std::string VertexGlobals() const
  {
    return "";
  }
  virtual std::string VertexCodeGen() const
  {
    if (coord)
      return "gl_TexCoord[0] = gl_MultiTexCoord0;\n";
    else
      return "";
  }
  virtual std::string FragmentGlobals() const
  {
    return "uniform sampler2D tex;\n";
  }
  virtual std::string FragmentCodeGen() const
  {
    std::stringstream s;
    s << fragnum;
    return
      std::string("vec4 color = texture2D(tex,gl_TexCoord[0].st);\n"
		  "vec4 FragColor") + s.str() + ";\n"
                  "FragColor" + s.str() + " = color;\n";
  }
private:
  bool coord;
  mutable int fragnum;
};
class WaterPiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const 
  {
    vec.push_back(ReqNormal);
    vec.push_back(ReqPosition);
  }

  virtual std::string VertexGlobals() const
  {
    return 
      "float Index(float xx, float yy, float time) {\n"
      "  float m = 8.0;\n"
      "  float n = 5.0;\n"
      "  float lx = cos(m*3.14159*xx);\n"
      "  float ly = cos(n*3.14159*yy);\n"
      "  float lt = cos(8.0*time);\n"
      "  float wave = 0.02*lx*ly*lt;\n"
      "    float m2 = 7;\n"
      "    float n2 = 13;\n" 
      "    // waves\n"
      "    float lx2 = cos(m2*3.14159*xx);\n"
      "    float ly2 = cos(n2*3.14159*yy);\n"
      "    float lt2 = cos(18.0*time);\n"
      "    float wave2 = 0.02*lx2*ly2*lt2;\n"
      ""
      "  return wave+wave2;\n"
      "}\n"
      "uniform float time;\n"
      "attribute float x;\n"
      "attribute float y;\n"
      "attribute float nx_x;\n"
      "attribute float nx_y;\n"
      "attribute float nx_z;\n"
      "attribute float ny_x;\n"
      "attribute float ny_y;\n"
      "attribute float ny_z;\n"
      "varying vec3 normal;\n"
      ;
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "float wave = Index(x,y,time);\n"
      "float wavex = Index(x+0.01,y,time);\n"
      "float wavey = Index(x,y+0.01,time);\n"
      //"float wave1 = Index(x+0.1,y,time);\n"
      //"float wave2 = Index(x,y+0.1,time);\n"
      "vec3 n = gl_NormalMatrix * gl_Normal;\n"
      "vec3 a = wave*n;\n"
      "vec3 ax = wavex*n;\n"
      "vec3 ay = wavey*n;\n"
      "vec4 v = gl_Vertex+vec4(a.x,a.y,a.z,0.0);\n"
      "vec3 vx = vec3(nx_x+ax.x,nx_y+ax.y,nx_z+ax.z);\n"
      "vec3 vy = vec3(ny_x+ay.x,ny_y+ay.y,ny_z+ay.z);\n"
      "vec3 dx = vx-v.xyz;\n"
      "vec3 dy = vy-v.xyz;\n"
      "normal = cross(dx,dy);\n"
      "gl_Position = gl_ModelViewProjectionMatrix * v;\n"
      //"gl_Normal = gl_Normal + vec3(wave1, wave2,0.0);\n" 
      ;
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
};
class WaterAttributes : public ShaderAttributes
{
public:
  WaterAttributes(int xid, int yid, 
		  int nx_x, int nx_y, int nx_z,
		  int ny_x, int ny_y, int ny_z
		  ) : xid(xid), yid(yid),
		      nx_x(nx_x), nx_y(nx_y), nx_z(nx_z),
		      ny_x(ny_x), ny_y(ny_y), ny_z(ny_z)
		      
  { }
  void SetAttribs(Program &p) const
  {
    x = p.find_attr("x", xid);
    y = p.find_attr("y", yid);
    vnx_x = p.find_attr("nx_x", nx_x);
    vnx_y = p.find_attr("nx_y", nx_y);
    vnx_z = p.find_attr("nx_z", nx_z);
    vny_x = p.find_attr("ny_x", ny_x);
    vny_y = p.find_attr("ny_y", ny_y);
    vny_z = p.find_attr("ny_z", ny_z);
  }
  void Attr(std::vector<Attrib> &vector)
  {
    vector.push_back(x);
    vector.push_back(y);
    vector.push_back(vnx_x);
    vector.push_back(vnx_y);
    vector.push_back(vnx_z);
    vector.push_back(vny_x);
    vector.push_back(vny_y);
    vector.push_back(vny_z);
  }
private:
  mutable Attrib x, y;
  mutable Attrib vnx_x, vnx_y, vnx_z;
  mutable Attrib vny_x, vny_y, vny_z;
  int xid, yid;
  int nx_x, nx_y, nx_z;
  int ny_x, ny_y, ny_z;
};
class WaterParameters : public ShaderParameters
{
public:
  WaterParameters(float time) : time(time) { }
  void SetParameters(Program &p) const
  {
    p.set_var("time", time);
  }
private:
  float time;
};


std::string stringify(int i);


class MoveObjectPiece : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const 
  {
    vec.push_back(ReqPosition);
  }

  MoveObjectPiece() { count = 1; }
  void set_count(int i) { count = i; }
  virtual std::string VertexGlobals() const
  {
    return 
      "attribute int object_id;\n"
      "uniform float pos_x[" + stringify(count) + "];\n"
      "uniform float pos_y[" + stringify(count) + "];\n"
      "uniform float pox_z[" + stringify(count) + "];\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "vec4 v = vec4(pos_x[object_id], pos_y[object_id], pos_z[object_id], 0.0);\n"
      "v+=gl_Vertex;\n"
      "gl_Position = gl_ModelViewProjectionMatrix * v;\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return "";
  }
  virtual std::string FragmentCodeGen() const
  {
    return "";
  }
private:
  int count;
};
class PointCollection;
class MoveObjectParameters : public ShaderParameters
{
public:
  MoveObjectParameters(const PointCollection &coll) : coll(coll) { }
  void SetParameters(Program &p) const;
private:
  const PointCollection &coll;
};


class ShaderFromPieces : public ShaderString
{
public:
  ShaderFromPieces() : configured(false) { }
  typedef std::vector<ShaderPiece*>::iterator iterator;
  typedef std::vector<ShaderPiece*>::const_iterator const_iterator;
  virtual std::string VertexShader() const
  {
    std::string s;
    for(const_iterator i = vec.begin();i!=vec.end();i++)
      {
	s += (*i)->VertexGlobals();
      }
    s+="void main() {\n";
    for(const_iterator i = vec.begin();i!=vec.end();i++)
      {
	s += (*i)->VertexCodeGen();
      }
    s+="}\n";
    return s;
  }
  virtual std::string FragmentShader() const
  {
    std::string s;
    for(const_iterator i = vec.begin();i!=vec.end();i++)
      {
	s += (*i)->FragmentGlobals();
      }
    s+="void main() {\n";
    for(const_iterator i = vec.begin();i!=vec.end();i++)
      {
	s += (*i)->FragmentCodeGen();
      }
    s+="}\n";
    return s;
  }
  void push_back(ShaderPiece* p) { vec.push_back(p); }
  void config()
  {
    if (!configured)
      {
	configured = true;
	std::cout << "Config" << std::endl;
	std::vector<int> conf;
	for(std::vector<ShaderPiece*>::iterator i=vec.begin();i!=vec.end();i++)
	  {
	    (*i)->Provides(conf);
	  }
	std::vector<int>::iterator i1 = std::find(conf.begin(), conf.end(), (int)ReqPosition);
	if (i1==conf.end())
	  vec.push_back(new EmptyVertexPiece);
	std::vector<int>::iterator i2 = std::find(conf.begin(), conf.end(), (int)ReqFragColorRGB);
	if (i2==conf.end())
	  vec.push_back(new EmptyRGBPiece);
	
	std::vector<int>::iterator i3 = std::find(conf.begin(), conf.end(), (int)ReqFragColorA);
	if (i3==conf.end())
	  vec.push_back(new EmptyAPiece);
	
	std::vector<int>::iterator i4 = std::find(conf.begin(), conf.end(), (int)ReqFragColorCombiner);
	if (i4 == conf.end())
	  vec.push_back(new FragColorCombinerPiece);
	
	for(std::vector<ShaderPiece*>::iterator i=vec.begin();i!=vec.end();i++)
	  {
	    (*i)->Config(conf);
	  }
      }
  }
private:
  std::vector<ShaderPiece*> vec;
  bool configured;
};

class ShaderUse
{
public:
  virtual ~ShaderUse() { }
  virtual void use()=0;
  virtual void set_params(ShaderParameters &p)=0;
  virtual void set_attribs(ShaderAttributes &p)=0;
  virtual void unuse()=0;
};
class PieceShader : public ShaderUse
{
public:
  PieceShader() : pair(0) { }
  ~PieceShader() { delete pair; }
  void push_back(ShaderPiece *p) 
  { pieces.push_back(p); }
  //void push_back(ShaderFunctionCollection *f)
  //{
  //}
  void config()
  {
    pieces.config();
  }
  void use() 
  {
    config();
    if (pair==0)
      {
	pair = new ShaderPair(pieces);
      }
    pair->use();
  }
  void set_params(ShaderParameters &p) 
  {
    config();
    if (pair==0)
      {
	pair = new ShaderPair(pieces);
      }
    pair->set_params(p);
  }
  void set_attribs(ShaderAttributes &p)
  {
    config();
    if (pair==0)
      {
	pair = new ShaderPair(pieces);
      }
    pair->set_attribs(p);
  }
  void unuse() { pair->unuse(); }
  int get_loc(std::string name) { return pair->get_loc(name); }
private:
  ShaderFromPieces pieces;
  ShaderPair *pair;
};


class VertexMain : public ShaderPiece
{
public:
  std::string CodeGen() const
  {
    return
      "void main() {\n"
      "   gl_Position = gl_ModelViewProjectionMatrix * Transform(gl_Vertex);\n"
      "}\n";
  }
};
class LinearInterpolationParameters : public ShaderParameters
{
public:
  LinearInterpolationParameters(int startpos_id, int endpos_id, int time_id, Point startpos, Point endpos, float starttime, float endtime, float currenttime) : startpos(startpos), endpos(endpos), starttime(starttime), endtime(endtime), currenttime(currenttime), startpos_id(startpos_id), endpos_id(endpos_id), time_id(time_id) { }
  void SetParameters(Program &pp) const
  {
    pp.set_var(startpos_id, startpos.x, startpos.y, startpos.z, starttime);
    pp.set_var(endpos_id, endpos.x, endpos.y, endpos.z, endtime);
    pp.set_var(time_id, currenttime);
  }
private:
  Point startpos;
  Point endpos;
  float starttime;
  float endtime;
  float currenttime;
  int startpos_id;
  int endpos_id;
  int time_id;
};

class LinearInterpolationParameters2 : public ShaderParameters
{
public:
  LinearInterpolationParameters2(Point startpos, Point endpos) : startpos(startpos), endpos(endpos) { }
  void SetParameters(Program &pp) const
  {
    pp.set_var("startpos", startpos );
    pp.set_var("endpos", endpos);
  }
private:
  Point startpos;
  Point endpos;
};

class LinearInterpolationShader : public ShaderPiece
{
public:
  virtual void Provides(std::vector<int> &vec) const
  {
    vec.push_back(ReqPosition);
  }
  virtual std::string VertexGlobals() const
  {
    return 
      "uniform vec4 startpos;\n"
      "uniform vec4 endpos;\n"
      "uniform float time;\n";
  }
  virtual std::string VertexCodeGen() const
  {
    return 
      "float t = time - startpos.w;\n"
      "t /= endpos.w-startpos.w;\n"
      "vec4 p = mix(startpos, endpos, t);\n"//startpos*(1.0-t)+endpos*t;\n"
      "vec4 k = p + gl_Vertex;\n"
      "k.w = 1.0; \n"
      "vec4 res = gl_ModelViewProjectionMatrix * k;\n"
      "gl_Position = res;\n";
  }
  virtual std::string FragmentGlobals() const
  {
    return ""; 
  } 
  virtual std::string FragmentCodeGen() const
  { 
    return "";
  }   
private:
  
};


// SHADER FUNCTIONS SUPPORT
//


//class ShaderFunctionCollection : public VectorArray<ShaderFunction*>
//{
//};

class ShaderFunction : public ShaderParameters
{
public:
  virtual void SetParameters(Program &p) const=0;
  virtual std::string Shader() const=0;
  virtual bool VertexShader() const=0;
};

class CirclePart : public ShaderFunction
{
public:
  void Set(Point center, float r_) { c=center; r=r_; }
  void Link(std::string s2d, std::string r, std::string c1) { c2d=s2d; rr = r; cc = c1; }
  void SetParameters(Program &p) const
  {
    p.set_var("r" + rr, r);
    p.set_var("c" + cc, c);
  }
  std::string Shader() const
  {
    return 
      "uniform float r"+rr+";\n"
      "uniform vec2 c"+cc+";\n"
      "vec2 Curve2d"+c2d+"(float k) {\n"
      "vec2 v;\n"
      "v.x = r"+rr+"*cos(k) + c"+cc+".x;\n"
      "v.y = r"+rr+"*sin(k) + c"+cc+".y;\n"
      "return v;\n"
      "}\n"
      ;
  }
  bool VertexShader() const { return true; }
private:
  Point c;
  float r;
  std::string c2d;
  std::string rr;
  std::string cc;
};

class Curve2dTo3dPart : public ShaderFunction
{
public:
  void Set(Plane p) { plane = p; }
  void Link(std::string s2d, std::string s3d, std::string pl) { c2d=s2d; c3d=s3d; pll = pl; }
  void SetParameters(Program &p) const
  {
    p.set_var("plane_u_p" + pll, plane.u_p);
    p.set_var("plane_u_x" + pll, plane.u_x);
    p.set_var("plane_u_y" + pll, plane.u_y);
  }
  std::string Shader() const
  {
    return 
      "uniform vec3 plane_u_p"+pll+";\n"
      "uniform vec3 plane_u_x"+pll+";\n"
      "uniform vec3 plane_u_y"+pll+";\n"
      "vec2 Curve2d"+c2d+"(float x);\n"
      "vec3 Curve3d"+c3d+"(float a) {\n"
      "   vec2 pp = Curve2d(a); \n"
      "   vec3 pp2 = plane_u_p"+pll+" + pp.x*plane_u_x"+pll+" + pp.y*plane_u_y"+pll+";\n"
      "   return pp2;\n"
      "}\n"
      ;
  }
  bool VertexShader() const { return true; }

private:
  Plane plane;
  std::string c2d;
  std::string c3d;
  std::string pll;
};

class ShaderFile
{
public:
  ShaderFile(std::string filename);
  std::string VertexShader(std::string name);
  std::string FragmentShader(std::string name);
  std::string GeometryShader(std::string name);
private:
  std::map<std::string, std::string> v_shaders;
  std::map<std::string, std::string> f_shaders;
  std::map<std::string, std::string> g_shaders;
};

class ShaderSeq
{
public:
  ShaderSeq(ShaderFile &file) : file(file) { }
  int GetShader(std::string v_format, std::string f_format, std::string g_format);
  void use(int i);
  void unuse(int i);
  Program *prog(int i);
private:
  ShaderFile &file;
private:
  std::vector<Program*> progs;
};

struct AnimStateI;
struct AnimState
{
  AnimState(ShaderFile &f);
  ~AnimState();
  AnimStateI *priv;
};

class VBOState;
class Anim;
class AnimSelectI;
void UpdateAnim(Anim &anim, AnimState &state, const std::vector<Attrib> &attribs=std::vector<Attrib>());

void DrawAnim(float start, float end, AnimState &state, const std::vector<Attrib> &attribs=std::vector<Attrib>());


#endif
