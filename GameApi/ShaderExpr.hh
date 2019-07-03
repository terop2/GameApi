
#include "VectorTools.hh"
#include "Effect.hh"


struct ChooseVertex
{
  int face;
  int point;
};
struct ChooseFragment
{
};

class ShaderExpr
{
public:
  void SetVertex(ChooseVertex v) { vertex = v; }
  void SetFragment(ChooseFragment f) { fragment = f; }
  ChooseVertex GetVertex() const { return vertex; }
  ChooseFragment GetFragment() const { return fragment; }

  int GetFace() const { return vertex.face; }
  int GetPoint() const { return vertex.point; }


  std::string uniquename() const 
  {
    std::stringstream ss;
    ss << "v" << unique;
    unique++;
    return ss.str();
  }
private:
  ChooseVertex vertex;
  ChooseFragment fragment;

  static int unique;
};

//
// Shader expressions
//
class FloatShader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual float Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class ColorShader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual Color Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class Vector2Shader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual Vector2d Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class Point2Shader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual Point2d Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class Vector3Shader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual Vector Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class Point3Shader : public ShaderExpr
{
public:
  virtual std::string Context() const=0;
  virtual Point Value() const=0;
  virtual std::string Expr(std::string varname) const=0;
  virtual void CollectIds(std::vector<int> &vec) const=0;
};

class FloatAttribute : public FloatShader
{
public:
  FloatAttribute(FaceCollection *coll, int id, std::string name) : coll(coll),id(id), name(name) { }
  std::string Context() const { return "attribute float "+name+";\n"; }
  float Value() const { return coll->Attrib(GetFace(), GetPoint(), id); }
  std::string Expr(std::string varname) const
  {
    return varname + "=" + name + ";\n";
  }
  virtual void CollectIds(std::vector<int> &vec) const 
  { 
    vec.push_back(id);
  }
private:
  FaceCollection *coll;
  int id;
  std::string name;
};
class PointAttribute : public Point3Shader
{
public:
  PointAttribute(FaceCollection *coll, int id_x, int id_y, int id_z, std::string name) : x(coll,id_x,name+"x"), y(coll,id_y, name+"y"), z(coll, id_z, name+"z"), name(name) { }
  std::string Context() const { return x.Context()+y.Context()+z.Context(); }
  Point Value() const { return Point(x.Value(),y.Value(),z.Value()); }
  std::string Expr(std::string varname) const
  {
    std::string s1 = varname + ".x = " + name + "x;\n";
    std::string s2 = varname + ".y = " + name + "y;\n";
    std::string s3 = varname + ".z = " + name + "z;\n";
    return s1+s2+s3;
  }
 
  virtual void CollectIds(std::vector<int> &vec) const
  {
    x.CollectIds(vec);
    y.CollectIds(vec);
    z.CollectIds(vec);
  }
private:
  FloatAttribute x,y,z;
  std::string name;
};

#if 0
class Vertex : public Point3Shader
{
public:
  Vertex(FaceCollection *coll) : coll(coll) { }
  std::string Context() const { return ""; }
  Point Value() const 
  {
    return coll->FacePoint(GetFace(), GetPoint());
  } 
  std::string Expr(std::string varname) const
  {
    return varname + "= gl_Vertex.xyz;";
  }
  virtual void CollectIds(std::vector<int> &vec) const { }
private:
  FaceCollection *coll;
};

#endif

class UniformFloat : public FloatShader
{
public:
  UniformFloat(std::string name, float &val) : name(name), val(val) { }
  std::string Context() const 
  {
    return "uniform float " + name + ";\n";
  }
  float Value() const
  {
    return val;
  }
  std::string Expr(std::string varname) const
  {
    return varname + "=" + name + ";\n";
  }
  virtual void CollectIds(std::vector<int> &vec) const { }
private:
  std::string name;
  float &val;
};

class MixPoint : public Point3Shader
{
public:
  MixPoint(Point3Shader &p1, Point3Shader &p2, std::string uniform_name, float &uniform_value) : p1(p1), p2(p2), uni(uniform_name, uniform_value), uniform_name(uniform_name) { }
  
  std::string Context() const
  {
    return p1.Context() + p2.Context() + uni.Context();
  }
  Point Value() const
  {
    p1.SetVertex(GetVertex());
    p1.SetFragment(GetFragment());
    Point pp1 = p1.Value();
    p2.SetVertex(GetVertex());
    p2.SetFragment(GetFragment());
    Point pp2 = p2.Value();
    return Vector(pp1)*(1.0-uni.Value()) + Vector(pp2)*uni.Value();    
  }
  std::string Expr(std::string varname) const
  {
    std::string n1 = uniquename();
    std::string n2 = uniquename();
    std::string e1 = p1.Expr(n1);
    std::string e2 = p2.Expr(n2);
    return "vec3 " + n1 + ";\nvec3 " + n2 + ";\n" + e1 + e2 + varname + "= mix(" + n1 + "," + n2 + "," + uniform_name + ");\n";
  }
  virtual void CollectIds(std::vector<int> &vec) const
  {
    p1.CollectIds(vec);
    p2.CollectIds(vec);
  }

private:
  Point3Shader &p1, &p2;
  UniformFloat uni;
  std::string uniform_name;
};

void testshader();
