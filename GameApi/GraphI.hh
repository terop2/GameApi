
#ifndef GRAPHI_HH
#define GRAPHI_HH

#include <vector>
#include <utility>
#include "VectorTools.hh"

namespace GameApi
{
  class EveryApi;
  struct FloatExprEnv { std::string name; float value; };
  struct IntExprEnv { std::string name; int value; };

};

struct Quad
{
  Point p1,p2,p3,p4;
};
struct Tex
{
  Point2d t1,t2,t3,t4;
};
struct QuadNormal
{
  Vector n1,n2,n3,n4;
};
struct QuadColor
{
  Color c1,c2,c3,c4;
};

template<class C>
class SimpleShape
{ // these use scanline conversion
public:
  virtual int SizeY() const=0; 
  virtual int NumEdges(int y) const=0;
  virtual float Edge(int y, int edge) const=0;
  virtual C EdgeLeftSide(int y, int edge) const=0;
  virtual C EdgeRightSide(int y, int edge) const=0;
};
template<class C, class I>
class Filler
{
public:
  virtual C Interpolate(I pos) const=0;
};

template<class C>
class LineShape : public SimpleShape<C>
{
public:
  LineShape(int sizey, Point2d pp1, Point2d pp2, C left, C right) : sizey(sizey),pp1(pp1), pp2(pp2),left(left), right(right) { }
  int SizeY() const {return sizey;}
  int NumEdges(int y) const
  {
    return 1;
  }
  float Edge(int y, int edge) const
  {
    Point2d p1 = { 0.0f, float(y) };
    Point2d p2 = { 100.0f, float(y) };
    Point2d p3 = pp1;
    Point2d p4 = pp2;

    float p_x = P_x(p1,p2,p3,p4);
    return p_x;
  }

  float P_x(Point2d p1, Point2d p2,  Point2d p3, Point2d p4)
  {
    return det(det(p1.x,p1.y,p2.x,p2.y), det(p1.x,1.0,p2.x,1.0),
	       det(p3.x,p3.y,p4.x,p4.y), det(p3.x,1.0,p4.x,1.0)) /
      det(det(p1.x,1.0,p2.x,1.0), det(p1.y,1.0,p2.y,1.0),
	  det(p3.x,1.0,p4.x,1.0), det(p3.y,1.0,p4.y,1.0));
  }
  /*
  float P_y(Point2d p1, Point2d p2,  Point2d p3, Point2d p4)
  {
    return det(det(p1.x,p1.y,p2.x,p2.y), det(p1.y,1.0,p2.y,1.0),
	       det(p3.x,p3.y,p4.x,p4.y), det(p3.y,1.0,p4.y,1.0)) /
      det(det(p1.x,1.0,p2.x,1.0), det(p1.y,1.0,p2.y,1.0),
	  det(p3.x,1.0,p4.x,1.0), det(p3.y,1.0,p4.y,1.0));
  }
  */
  float det(float a, float b, float c, float d)
  {
    return a*d-b*c;
  }
  C EdgeLeftSide(int y, int edge) const { return left; }
  C EdgeRightSide(int y, int edge) const { return right; }
private:
  int sizey;
  Point2d pp1,pp2;
  C left,right;
};


// This is meant for supporting derivative
template<class I, class T>
class DFunction
{
public:
  virtual T DIndex(I i) const=0;
};









class CompactBitmap
{
public:
  
};

template<class C>
class Bitmap
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual C Map(int x, int y) const=0;
  virtual void Prepare()=0;
  virtual ~Bitmap() { }
};
template<class C>
class BitmapArray2
{
public:
  virtual int Size() const=0;
  virtual int SizeX(int i) const=0;
  virtual int SizeY(int i) const=0;
  virtual C Map(int i, int x, int y) const=0;
  virtual ~BitmapArray2() { }
};


typedef Bitmap<Color> ColorBitmap;
typedef Bitmap<Point> PointBitmap;
typedef Bitmap<Quad> QuadBitmap;

template<class C>
class Voxel
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual int SizeZ() const=0;
  virtual C Map(int x, int y, int z) const=0;
  virtual ~Voxel() { }
};

template<class I, class B>
class NDim
{
public:
  virtual std::vector<I> Count() const=0;
  virtual B Map(I *array, int size) const=0;
  virtual NDim<I,B> *Interpolate(const NDim<I,B> &b1, const NDim<I,B> &b2, I val) const=0;
  virtual ~NDim() { }
};

template<class C>
class ContinuousBitmap 
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual C Map(float x, float y) const=0;
  virtual void Prepare()=0;
  virtual ~ContinuousBitmap() { }
};



template<class Z, class C>
class ContinuousBitmap2
{
public:
  virtual Z SizeX() const=0;
  virtual Z SizeY() const=0;
  virtual C Map(Z x,Z y) const=0;
};

template<class C>
class ContinuousVoxel
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual float SizeZ() const=0;
  virtual C Map(float x, float y, float z) const=0;
  virtual ~ContinuousVoxel() { }
};

template<class T>
class HandleValue
{
public:
  virtual void Handle(T t)=0;
};
class Graph
{
public:
  virtual int NumVertexes() const=0;
  virtual int NumEdges() const=0;
  virtual std::pair<int, int> EdgeVertex(int edge) const=0;
};
template<class V, class E>
class GraphData
{
public:
  virtual V Vertex(int v) const =0;
  virtual E Edge(int e, int v1, int v2) const=0;
};

class Sprite
{
public:
  virtual int NumFrames() const=0;
  virtual int XSize(int frame) const=0;
  virtual int YSize(int frame) const=0;
  virtual Point2d Pos(int frame) const=0;
  virtual Color Pixel(int frame, int x, int y) const=0;
  virtual ~Sprite() { }
};

struct Size
{
  int sx,sy;
};
struct Pos
{
  int x,y;
};
struct TRect
{
  float x,y;
  float width, height;
};

class Samples
{
public:
  virtual ~Samples() { }
  virtual int NumWaves() const=0;
  virtual int SampleRate(int wave) const=0;
  virtual int Id(int wave) const=0;
  virtual float Length(int wave) const=0;
  virtual float Index(int wave, float val) const=0;
};

struct Wavs
{
  std::vector<unsigned char *> data;
  std::vector<int> size;
  std::vector<int> rate;
  std::vector<void*> chunks;
  std::vector<int> ids;
};

class Tracker
{
public:
  virtual ~Tracker() { }
  virtual int NumChannels() const=0;
  virtual int NumTimeSlots() const=0;
  virtual int Type(int channel, int timeslot) const=0;
  virtual int Duration(int channel, int timeslot) const { return 1; }
  virtual int Sample(int channel, int timeslot) const { return 0; }
  virtual int NumData(int channel, int timeslot) const { return 0; }
  virtual unsigned int Data(int channel, int timeslot, int data) const { return 0; }
};

class ShaderModule
{
public:
  virtual ~ShaderModule() { }
  virtual std::string Function() const=0;
  virtual std::string FunctionName() const=0;
  virtual std::string ColorFunctionName() const=0;
  virtual int NumArgs() const=0;
  virtual std::string ArgName(int i) const=0;
  virtual std::string ArgValue(int i) const=0;
  //virtual int id() const=0;
  //virtual bool FreeVariable(int i) const=0;
  //virtual std::string ArgType(int i) const=0;
};

class GuiWidget
{
public:
  void set_id(std::string id_m) { id = id_m; }
  std::string get_id() const { return id; }
  virtual ~GuiWidget() { }
  virtual void hide() { }
  virtual void show() { }
  virtual Point2d get_pos() const=0;
  virtual Vector2d get_size() const=0;
  virtual void set_pos(Point2d pos)=0;
  virtual void set_size(Vector2d size)=0;
  virtual void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)=0;
  virtual bool is_visible() const=0;
  virtual void render()=0;
  virtual int render_to_bitmap()=0; // returns bitmap id
  virtual bool content_changed() const=0;
  virtual void clear_content_changed()=0;
  virtual void select_item(int item)=0;
  virtual int chosen_item() const=0;
  virtual float dynamic_param(int id) const=0;
  virtual void set_dynamic_param(int id, float val)=0;
  virtual int child_count() const=0;
  virtual GuiWidget *child(int num) const=0;
  virtual std::vector<GuiWidget*> *child_from_path(std::string path) { return 0; }
private:
  std::string id;
};
namespace GameApi
{
  class ExecuteEnv;
};

struct GameApiParam
{
  std::string param_name;
  std::string value;
};
namespace GameApi {
class EditNode;
};
class GameApiItem
{
public:
  virtual int Count() const=0;
  virtual std::string Name(int i) const=0;
  virtual int ParamCount(int i) const=0;
  virtual std::string ParamName(int i, int p) const=0;
  virtual std::string ParamType(int i, int p) const=0;
  virtual std::string ParamDefault(int i, int p) const=0;
  virtual std::string ReturnType(int i) const=0;
  virtual std::string ApiName(int i) const=0;
  virtual std::string FuncName(int i) const=0;
  virtual std::string Symbols() const=0;
  virtual std::string Comment() const=0;
  virtual int Execute(GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e)=0;
  //virtual std::vector<GameApi::EditNode*> CollectNodes(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)=0;
  virtual std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)=0;
  virtual void BeginEnv(GameApi::ExecuteEnv &e, std::vector<GameApiParam> params) { }
  virtual void EndEnv(GameApi::ExecuteEnv &e) { }
};

struct GameApiLine
{
  int x,y;
  std::string module_name;
  std::string uid;
  std::vector<GameApiParam> params;
};

struct GameApiFunction
{
  std::string function_name;
  std::vector<std::string> param_names;
  std::vector<std::string> param_types;
  std::vector<GameApiLine> lines;
};

struct GameApiModule
{
  std::vector<GameApiFunction> funcs;
};

struct MainLoopEnv
{
  int sh_color = 0;
  int sh_texture = 0;
  int sh_array_texture = 0;

  // from event api
  //int type;
  //int ch;
  //Point cursor_pos;
  //int button;

  float time = 0.0;
  float delta_time = 0.0;

  Matrix env;
  //std::string vertex_shader = "";
  //std::string fragment_shader = "";
  int us_vertex_shader = -1;
  int us_fragment_shader = -1;

  Matrix in_MV;
  Matrix in_T;
  Matrix in_N;
  int sfo_id=-1;
};

struct MainLoopEvent
{
  int type;
  int ch;
  Point cursor_pos;
  int button;
};

class MainLoopItem
{
public:
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual int shader_id() { return -1; }
};

class ExprNode
{
public:
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)=0;
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)=0;
};

class PhysicsNode
{
public:
  virtual ~PhysicsNode() { }
  virtual int NumAnchors() const=0;
  virtual Point AnchorPoint(int i) const=0;
  virtual int NumForces(int i) const=0;
  virtual Vector Force(int i, int f) const=0;
  virtual int NumLinks() const=0;
  virtual std::pair<int,int> Link(int i) const=0;
  virtual float LinkDistance(int i) const=0;
  virtual int NumForceVolumes() const=0;
  virtual Vector ForceVolume(int v, Point p) const=0;
};

class ForwardPhysics : public PhysicsNode
{
public:
  ForwardPhysics(PhysicsNode &node) : node(node) {}
  virtual int NumAnchors() const { return node.NumAnchors(); }
  virtual Point AnchorPoint(int i) const { return node.AnchorPoint(i); }
  virtual int NumForces(int i) const { return node.NumForces(i); }
  virtual Vector Force(int i, int f) const { return node.Force(i,f); }
  virtual int NumLinks() const { return node.NumLinks(); }
  virtual std::pair<int,int> Link(int i) const { return node.Link(i); }
  virtual float LinkDistance(int i) const { return node.LinkDistance(i); }
  virtual int NumForceVolumes() const { return node.NumForceVolumes(); }
  virtual Vector ForceVolume(int v, Point p) const { return node.ForceVolume(v,p); }

private:
  PhysicsNode &node;
};

class TriStrip
{
public:
  virtual int Size() const=0;
  virtual Point Pos(int i) const=0;
  virtual unsigned int Color(int i) const=0;
  virtual Point2d TexCoord(int i) const=0;
  virtual Vector Normal(int i) const=0;
};

class TriStripForward : public TriStrip
{
public:
  TriStripForward(TriStrip &next) : next(next) { }
  virtual int Size() const { return next.Size(); }
  virtual Point Pos(int i) const { return next.Pos(i); }
  virtual unsigned int Color(int i) const { return next.Color(i); }
  virtual Point2d TexCoord(int i) const { return next.TexCoord(i); }
  virtual Vector Normal(int i) const { return next.Normal(i); }
protected:
  TriStrip &next;
};

class Cutter
{
public:
  virtual Point cut(Point p1, Point p2) const=0;
};

class Collision
{
public:
  Collision(int id) : m_id(id), m_x(0.0), m_y(0.0) { }
  int m_id;
  float m_x,m_y;
  virtual bool check_collision(float x, float y) const=0;
  virtual void set_pos(int id, float x, float y)
  {
    if (m_id == id)
      {
	m_x = x;
	m_y = y;
      }
  }
};

class Movement
{
public:
  virtual void set_matrix(Matrix m)=0;
  virtual Matrix get_whole_matrix(float time, float delta_time) const=0;
};

class ImplicitFunction2d
{
public:
  virtual float f(float x, float y) const=0;
  virtual float f2(float x) const=0; // use substitution y::=f(x)
};

class ImplicitFunction3d
{
public:
  virtual float f(float x, float y, float z) const=0;
  virtual float f_u(float x, float y) const=0; // substitute z=f(x,y)
  virtual float f_l(float x, float y) const=0; // substitute z=f(x,y)
  virtual unsigned int f_color_u(float x, float y) const { return 0xffffffff;}
  virtual unsigned int f_color_l(float x, float y) const { return 0xff888888;}
  virtual float pos_x() const=0;
  virtual float pos_y() const=0;
  virtual float size_x() const=0;
  virtual float size_y() const=0;
};

class ImplicitFunction4d
{
public:
  virtual float f(float x, float y, float z, float t) const=0;
  virtual float f_u(float x, float y, float t) const=0;
  virtual float f_l(float x, float y, float t) const=0;
};

class ImplicitFunction3dArray
{
public:
  std::vector<ImplicitFunction3d*> vec;
};

class TreeLevel
{
public:
  virtual int num_childs() const=0;
  virtual Matrix get_child(int i, float time) const=0;
};

class TreeStack
{
public:
  virtual int num_levels() const=0;
  virtual TreeLevel *get_level(int i) const=0;
};

//void draw_tree(Tree *spec, std::vector<ML> vec);

class Material
{
public:
  virtual int mat(int p) const=0; 
  virtual int mat_inst(int p, int pts) const=0;
  virtual int mat_inst2(int p, int pta) const=0;
};

class ShaderCall
{
public:
  virtual int index(int base) const=0;
  virtual std::string func_call() const=0;
  virtual std::string define_strings() const=0;
};

class MatrixArray
{
public:
  virtual int Size() const=0;
  virtual Matrix Index(int i) const=0;
};

class PlaneShape
{
public:
  virtual int NumShapes() const=0;
  virtual int NumPoints(int shape) const=0;
  virtual Point2d FacePoint(int shape, int point) const=0;
  virtual unsigned int Color(int shape, int point) const=0;
  virtual Point2d TexCoord(int shape, int point) const=0;
  virtual float PosX() const=0;
  virtual float PosY() const=0;
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
};

class SkeletalNode
{
public:
  virtual Matrix mat(float time) const=0;
  virtual Point pos(float time) const=0;
};

namespace GameApi {
class EditNode {
public:
  // 0=no source, 1=mouse_x, 2=mouse_y
  virtual void SetSource(int i)=0;
  // 0=no edit, 1=edit_x, 2=edit_y, 3=edit_z
  virtual void SetMode(int i)=0;
  virtual void CurrentPos(Point &p) const=0;
  virtual void HandleMouseMove(int x, int y)=0;
  virtual void HandleMousePress(int type)=0;
  virtual std::string GetValue() const=0;
};
}

class MixedI
{
public:
  virtual int NumItems() const=0;
  virtual std::string Type() const=0;
  virtual std::string Print() const=0;
  virtual std::string Name(int i) const=0;
  virtual std::string Value(int i) const=0;
  virtual MixedI *Access(int i) const=0;
  virtual MixedI *Clone() const=0;
};

#endif
