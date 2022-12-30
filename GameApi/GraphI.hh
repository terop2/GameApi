
#ifndef GRAPHI_HH
#define GRAPHI_HH

#include <vector>
#include <utility>
#include <functional>
#include "VectorTools.hh"

#ifndef TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
#endif

#include "GameApi_low.hh"

template<class T, class K> class Array;


namespace GameApi
{
  class EveryApi;
  struct FloatExprEnv { std::string name; float value; };
  struct IntExprEnv { std::string name; int value; };

};

  


enum DSFlags
  {
    DSDisableNormal=0x1,
    DSDisableColor=0x2,
    DSDisableTexCoord=0x4,
    DSDisableTexCoord3=0x8,
    DSDisableObjects=0x10
  };


class CollectVisitor;
class CollectInterface
{
public:
  virtual ~CollectInterface() { }
  virtual void Collect(CollectVisitor &vis)=0;
  virtual int NumBlocks() const { return 1; }
  virtual void HeavyPrepare()=0;
  virtual void FirstFrame() { }
};

class CollectVisitor
{
public:
  virtual ~CollectVisitor() { }
  virtual void register_obj(CollectInterface *i)=0;
  virtual void register_first_frame(CollectInterface *i)=0;
};

namespace GameApi
{

class ASyncVec
{
public:
  virtual ~ASyncVec() { }
  virtual const unsigned char &operator[](int i) const=0;
  virtual int size() const=0;
  virtual const unsigned char *begin() const=0;
  virtual const unsigned char *end() const=0;
  virtual void del()=0;
};
}

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
class Bitmap : public CollectInterface
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual C Map(int x, int y) const=0;
  virtual void Prepare()=0;
  virtual ~Bitmap() { }
};

class MemoryBlock : public CollectInterface
{
public:
  virtual ~MemoryBlock() { }
  virtual void Prepare()=0;
  virtual unsigned char *buffer() const=0;
  virtual int size_in_bytes() const=0;
};

class UrlMemoryMap : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual int size() const=0;
  virtual std::string get_url(int i) const=0;
  virtual MemoryBlock *get_block(std::string url) const=0;
};

class VertexArray : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual MemoryBlock *triangle_polys()=0;
  virtual MemoryBlock *quad_polys() =0;
  virtual MemoryBlock *poly_polys() =0;
  virtual MemoryBlock *tri_polys2() =0;
  virtual MemoryBlock *quad_polys2() =0;
  virtual MemoryBlock *poly_polys2() =0;

  virtual MemoryBlock *tri_normals()=0;
  virtual MemoryBlock *quad_normals()=0;
  virtual MemoryBlock *poly_normals()=0;

  virtual MemoryBlock *tri_color()=0;
  virtual MemoryBlock *quad_color()=0;
  virtual MemoryBlock *poly_color()=0;

  virtual MemoryBlock *tri_texcoord()=0;
  virtual MemoryBlock *quad_texcoord()=0;
  virtual MemoryBlock *poly_texcoord()=0;
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
class Voxel : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual int SizeZ() const=0;
  virtual C Map(int x, int y, int z) const=0;
  virtual unsigned int Color(int x, int y, int z) const { return 0xffffffff; }
  virtual Vector Normal(int x, int y, int z) const { Vector v{0.0,0.0,-400.0}; return v; }
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
class ContinuousBitmap : public CollectInterface
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
class ContinuousVoxel : public CollectInterface
{
public:
  virtual void Prepare()=0;
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
  GuiWidget() : index(0), size2(1) { }
  void set_id(std::string id_m) { id = id_m; }
  std::string get_id() const { return id; }
  void set_index(int j) { index = j; }
  int get_index() const { return index; }
  void set_size2(int sz) { size2=sz; }
  int get_size2() const { return size2; }
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
  int index;
  int size2;
};
namespace GameApi
{
  class ExecuteEnv;
  class Env;
};

struct GameApiLine;

struct GameApiParam
{
  std::string param_name;
  std::string value;
  bool is_array = false;
  GameApiLine *array_return_target = 0;
  int j; // multiple return values
};
namespace GameApi {
class EditNode;
};
struct ASyncData
{
  std::string api_name;
  std::string func_name;
  int param_num;
};

namespace chaiscript
{
  class ChaiScript;
}

class GameApiItem
{
public:
  virtual ~GameApiItem() { }
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
  virtual int Execute(std::stringstream &ss, GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e, int j)=0;
  //virtual std::vector<GameApi::EditNode*> CollectNodes(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)=0;
  virtual std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, int j)=0;
  virtual void BeginEnv(GameApi::ExecuteEnv &e, std::vector<GameApiParam> params) { }
  virtual void EndEnv(GameApi::ExecuteEnv &e) { }
  virtual void RegisterToChai(GameApi::EveryApi *ev, chaiscript::ChaiScript *chai) { }
};

struct GameApiLine
{
  int x,y;
  std::string module_name;
  std::string uid;
  int j=0;
  int sz=1;
  bool array_return = false;
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
enum PieceUserDataIds
  {
  };
struct WorldPiece
{
  std::string script;
  Point delta_pos;
  int p;  
  std::map<int, void*> user_data;
};
struct PlayerPiece
{
  std::string script;
  Point delta_pos;
  int p;
  std::map<int, void*> user_data;
};
struct EnemyPiece
{
  std::string script;
  Point delta_pos;
  int p;
  Point enemy_position;
  float enemy_rot_y;
  std::map<int, void*> user_data;
};
struct WorldElement
{
  int index;
};
struct CollisionData
{
  std::vector<Point> bounding_box;
};
class CollisionAction
{
public:
  virtual void Collide(std::string name1, std::string name2)=0;
};
struct World
{
  // Actual world
  WorldElement *world = 0; // these are indexes, not owned
  int world_sx = 100;
  int world_sy = 100;
  int world_sz = 1;
  Vector world_dx, world_dy, world_dz;
  Point world_pos;

  std::vector<WorldPiece> world_pieces;

  // Player
  Point player_pos;
  Matrix player_matrix;
  std::vector<PlayerPiece> player_pieces;

  // Camera
  Point camera_pos;
  float rot_y;
  Vector camera_normal;
  
  // enemies
  Matrix enemy_matrix;
  std::vector<EnemyPiece> enemy_pieces;

  std::map<std::string, CollisionData*> collision_data;
  std::vector<std::pair<std::string,std::string> > collisions;
};

struct ContentPiece
{
  std::string url;
  std::string script;
};

struct ContentCollection
{
  std::vector<std::string> content_item_names;
  std::vector<ContentPiece> content_items;
};

struct MainLoopEnv
{
  int sh_color = 0;
  int sh_texture = 0;
  int sh_texture_2d = 0;
  int sh_array_texture = 0;

  // put only shader functions to these
  std::string v_shader_functions;
  std::string f_shader_functions;

  std::vector<std::string> v_shader_funcnames;
  std::vector<std::string> f_shader_funcnames;
 
 
  // from event api
  //int type;
  //int ch;
  //Point cursor_pos;
  //int button;

  float time = 0.0;
  float delta_time = 0.0;

  float in_POS = 0.0;
  
  Matrix env;
  //std::string vertex_shader = "";
  //std::string fragment_shader = "";
  int us_vertex_shader = -1;
  int us_fragment_shader = -1;

  Matrix in_MV;
  bool has_inverse=false;
  Matrix in_iMV;
  Matrix in_T;
  Matrix in_N;
  Matrix in_P;
  int sfo_id=-1;
  int spotlight_id = 1;
  int screen_x = 0; // these are the main window sizes
  int screen_y = 0;
  int screen_width = 800;
  int screen_height = 600;
  bool is_2d = false;

  // Game properties
  World *current_world = 0;
  ContentCollection *avail_content =0;
  int depthfunc=Low_GL_LEQUAL;
  int depthmask=Low_GL_TRUE;
  bool cullface=false;
};

struct FaceID
{
  int facenum;
  float u,v;
};

struct MainLoopEvent
{
  int type;
  int ch;
  Point cursor_pos;
  int button;

    bool joy0_button0;
    bool joy0_button1;
    bool joy0_button2;
    bool joy0_button3;

    bool joy1_button0;
    bool joy1_button1;
    bool joy1_button2;
    bool joy1_button3;

  int joy0_current_axis;
  int joy0_axis0;
  int joy0_axis1;
  int joy0_axis2;
  int joy0_axis3;
  int joy0_axis4;
  int joy1_current_axis;
  int joy1_axis0;
  int joy1_axis1;
  int joy1_axis2;
  int joy1_axis3;
  int joy1_axis4;

  int joy0_ball0;
  int joy0_ball1;
  int joy1_ball0;
  int joy1_ball1;
  
  //float time = 0.0;
  std::string drag_drop_filename = "";

  Point event_location;
};



class MainLoopItem : public CollectInterface
{
public:
  virtual ~MainLoopItem() { }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis)=0;
  virtual void HeavyPrepare()=0;
  virtual void Prepare()=0;
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  virtual void destroy() { }
  virtual void set_vars(std::map<std::string, std::string> vars) { }
  virtual std::map<std::string, std::string> get_vars() const {
    return std::map<std::string,std::string>();
  }
};

enum FrameBufferFormat
  {
    F_Mono1,
    F_Mono8,
    F_RGB565,
    F_RGB888,
    F_RGBA8888
  };
enum DrawBufferFormat
  {
    D_Mono1,
    D_RGBA8888
  };

struct FrameLoopEvent
{
  int type;
  int ch;
  Point cursor_pos;
  int button;
  bool pin[9];
  float time = 0.0;
};

class SourceBitmap
{
public:
  SourceBitmap(DrawBufferFormat fmt, int depth);
  SourceBitmap(const SourceBitmap &src);
  void operator=(const SourceBitmap &src);
  void set_data(void *data, int width, int height, int ydelta);
  void set_data_mono1(void *data, int width, int height, int ydelta);
  ~SourceBitmap();
public:
  void *m_data;
  DrawBufferFormat fmt;
  int m_width;
  int m_height;
  int m_ydelta;
  int m_depth;
  bool m_owned;
  bool m_owned2;
};


class SourceBitmap;

struct Dot
{
  int x,y;
  unsigned int color;
};

struct SourceDots
{
  std::vector<Dot> dots;
};

class FrameBuffer : public CollectInterface
{
public:
  virtual ~FrameBuffer() { }
  virtual void Prepare()=0;
  virtual void handle_event(FrameLoopEvent &e)=0;
  virtual void frame()=0;
  virtual void *Buffer() const=0;
  virtual int Width() const=0;
  virtual int Height() const =0;
  virtual int Depth() const =0;
  virtual FrameBufferFormat format() const=0;
  virtual float *DepthBuffer() const=0;
  virtual void draw_rect(int pos_x, int pos_y, int sp_width, int sp_height, unsigned int color)=0;

  virtual void draw_sprite(SourceBitmap *bm, int x, int y) =0;
  //virtual void draw_dots(SourceDots *dots, int x, int y)=0;
};


struct DrawLoopEnv
{
  DrawBufferFormat format;
  //void *drawbuffer;
  //int drawbuffer_width;
  //int drawbuffer_height;
  //int drawbuffer_depth;
  FrameBuffer *drawbuffer;

  float time = 0.0;
  float delta_time = 0.0;

  float *depth_buffer;
};



class FrameBufferLoop : public CollectInterface
{
public:
  virtual ~FrameBufferLoop() { }
  virtual void Prepare()=0;
  virtual void handle_event(FrameLoopEvent &e)=0;
  virtual void frame(DrawLoopEnv &e)=0;

};


class ExprNode
{
public:
  virtual ~ExprNode() { }
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
  virtual ~TriStrip() { }
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
  virtual std::vector<Point> cut(Point p1, Point p2) const=0;
};

class Collision
{
public:
  Collision(int id) : m_id(id), m_x(0.0), m_y(0.0) { }
  virtual ~Collision() { }
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
  virtual ~Movement() { }
  virtual void event(MainLoopEvent &e)=0;
  virtual void frame(MainLoopEnv &e)=0;
  virtual void draw_event(FrameLoopEvent &e)=0;
  virtual void draw_frame(DrawLoopEnv &e)=0;

  virtual void set_matrix(Matrix m)=0;
  virtual Matrix get_whole_matrix(float time, float delta_time) const=0;
};
class ColorChange
{
public:
  virtual ~ColorChange() { }
  virtual Color get_whole_color(float time, float delta_time) const=0;
};

class ImplicitFunction2d
{
public:
  virtual ~ImplicitFunction2d() { }
  virtual float f(float x, float y) const=0;
  virtual float f2(float x) const=0; // use substitution y::=f(x)
};

class ImplicitFunction3d
{
public:
  virtual ~ImplicitFunction3d() { }
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
  virtual ~TreeLevel() { }
  virtual int num_childs() const=0;
  virtual Matrix get_child(int i, float time) const=0;
};

class TreeStack
{
public:
  virtual ~TreeStack() { }
  virtual int num_levels() const=0;
  virtual TreeLevel *get_level(int i) const=0;
};

//void draw_tree(Tree *spec, std::vector<ML> vec);

class Material
{
public:
  virtual ~Material() { }
  virtual void logoexecute() { }
  virtual int mat(int p) const=0; 
  virtual int mat_inst(int p, int pts) const=0;
  virtual int mat_inst_matrix(int p, int ms) const=0;
  virtual int mat_inst2(int p, int pta) const=0;
  virtual int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const=0;
};

class ScreenSpaceMaterial
{
public:
  virtual ~ScreenSpaceMaterial() { }
  virtual void Prepare()=0;
  virtual int mat(int screen_txid, int depth_txid, int fullscreenquad, int position_txid, int normal_txid) const=0;
};

class ShaderCall
{
public:
  virtual ~ShaderCall() { }
  virtual int index(int base) const=0;
  virtual std::string func_call() const=0;
  virtual std::string func_call2(int &index) const=0;
  virtual std::string define_strings() const=0;
  virtual std::string func_name() const=0;
};

class MatrixArray : public CollectInterface
{
public:
  virtual ~MatrixArray() { }
  virtual void Prepare() { }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int Size() const=0;
  virtual Matrix Index(int i) const=0;
  virtual unsigned int Color(int i) const { return 0xffffffff; }
  virtual Vector Normal(int i) const { Vector v{0.0,0.0,-400.0}; return v; }
};

class PlaneShape
{
public:
  virtual ~PlaneShape() { }
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
  virtual ~SkeletalNode() { }
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
  virtual ~MixedI() { }
  virtual int NumItems() const=0;
  virtual std::string Type() const=0;
  virtual std::string Print() const=0;
  virtual std::string Name(int i) const=0;
  virtual std::string Value(int i) const=0;
  virtual MixedI *Access(int i) const=0;
  virtual MixedI *Clone() const=0;
};

class Event3
{
public:
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual bool event_triggered() const=0;
};

class Action
{
public:
  virtual MainLoopItem *get_main_loop(MainLoopItem *next)=0;
  virtual void trigger()=0;
};

class Blocker
{
public:
  virtual ~Blocker() { }
  virtual void Execute()=0; // must block execution
  virtual void SetTimeout(float duration)=0;
};
class Splitter
{
public:
  GameApi::Env *e;
  GameApi::EveryApi *ev;
public:
  virtual ~Splitter() { }
  virtual void Init()=0;
  virtual int Iter()=0;
  virtual void Destroy()=0;
  virtual Splitter* NextState(int code) { return this; }
  virtual void EnvTransfer(Splitter *next) { next->e = e; next->ev = ev; }
};

class PointTransform
{
public:
  virtual ~PointTransform() { }
  virtual Point Map(Point p, float delta_time) const=0;
};
class FaceCollection;

class VertexAnimNode
{ // KF type, gap=[0..n-1]
public:
  virtual ~VertexAnimNode() { }
  virtual int NumKeyFrames()=0; // n
  virtual float StepDuration(int keyframe_gap) const=0;
  virtual int FaceColl(int keyframe_gap) const=0;
  virtual int Lines(int keyframe_gap) const=0;
  //virtual int Points(int keyframe_gap) const=0;
};
class CurvePos
{
public:
  virtual ~CurvePos() { }
  virtual float FindPos(Point p, float curve_length) const=0;
};

enum ArrayTypesInUse
  {
    E_BM=1,
    E_P
  };

//int array_type_to_int(GameApi::BM b); // the definitions are in gameapi.cc beginning
//int array_type_to_int(GameApi::P b);


struct ArrayType
{
  int type; // arraytypesinuse
  std::vector<int> vec;
  Array<int,int> *vec2=0;
};

template<class T>
class Fetcher
{
public:
  virtual ~Fetcher() { }
  virtual void event(MainLoopEvent &e)=0;
  virtual void frame(MainLoopEnv &e)=0;
  virtual void draw_event(FrameLoopEvent &e)=0;
  virtual void draw_frame(DrawLoopEnv &e)=0;
  virtual void set(T t)=0;
  virtual T get() const=0;
};

class ASyncLoader
{
public:
  void load_urls(std::string url, std::string homepage);
  void load_all_urls(std::vector<std::string> urls, std::string homepage);
  GameApi::ASyncVec *get_loaded_data(std::string url) const;
  void set_callback(std::string url, void (*fptr)(void*), void *data);
  void rem_callback(std::string url);
};

template<class T>
class BuilderValue
{
public:
  virtual T get() const=0;
};

template<class T>
class RefBuilderValue : public BuilderValue<T>
{
public:
  RefBuilderValue(const T &ref) : ref(ref) { }
  T get() const { return ref; }
private:
  const T &ref;
};
template<class T>
class BuilderArray
{
public:
  virtual int Size() const=0;
  virtual BuilderValue<T> *Index(int i) const=0;
};

template<class T>
class BuilderArrImpl : public BuilderArray<T>
{
public:
  BuilderArrImpl(std::vector<T> vec) : vec(vec), ref(val) { }
  int Size() const { return vec.size(); }
  BuilderValue<T> *Index(int i) const {
    val = vec[i];
    return &ref;
  }
private:
  std::vector<T> vec;
  RefBuilderValue<T> ref;
  T val;
};

#if 0
template<class T, class K>
class FMapArray : public BuilderArray<K>
{
public:
  FMapArray(BuilderArray<T> &arr, Function<T,K> &f) : arr(arr), f(f), ref(val) { }
  int Size() const { return arr.Size(); }
  BuilderValue<K> *Index(int i) const {
    BuilderValue<T> *t = arr.Index(i);
    T t2 = t->get();
    K k1 = f.Index(t2);
    val = k1;
    return ref;
  }
private:
  BuilderArray<T> &arr;
  Function<T,K> &f;
  RefBuilderValue<K> ref;
  K val;
};
#endif

class FontInterface : public CollectInterface
{
public:
  virtual ~FontInterface() { }
  virtual void Prepare() { }
  virtual int Top(long idx) const=0;
  virtual int SizeX(long idx) const=0;
  virtual int SizeY(long idx) const=0;
  virtual int AdvanceX(long idx) const=0;
  virtual int Map(long idx, int x, int y) const=0;
};

class GlyphInterface : public CollectInterface
{
public:
  virtual ~GlyphInterface() { }
  virtual void Prepare() { }
  virtual int Top() const=0; // needs to be fast
  virtual int SizeX() const=0;
  virtual int SizeY() const=0; // needs to be fast
  virtual int AdvanceX() const=0;
  virtual int Map(int x, int y) const=0;
};


class StringDisplay : public CollectInterface
{
public:
  virtual ~StringDisplay() { }
  virtual void Prepare() { }
  virtual int Count() const=0;
  virtual int X(int c) const=0;
  virtual int Y(int c) const=0;
  virtual int SX(int c) const=0;
  virtual int SY(int c) const=0;
  virtual int Map(int c, int x, int y) const=0;
};


class CmdExecute {
public:
  virtual void Execute(char c)=0;
  virtual Point pos() const=0;
  virtual int obj_type() const=0;
  virtual void set_hooks(std::function<void (char)> f)=0;
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z)=0;
  virtual Vector get_unit_x() const=0;
  virtual Vector get_unit_y() const=0;
  virtual Vector get_unit_z() const=0;
};

class CurvePatch
{
public:
  virtual ~CurvePatch() { }
  virtual float start_x() const=0;
  virtual float end_x() const=0;
  virtual float start_y() const=0;
  virtual float end_y() const=0;
  virtual Point Map(float x, float y) const=0;
};

class PointsApiPoints : public CollectInterface
{
public:
  virtual void Prepare() { }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const=0;
  virtual Point Pos(int i) const=0;
  virtual unsigned int Color(int i) const=0;
  virtual Vector Normal(int i) const { Vector v{0.0,0.0,-400.0}; return v; }
  virtual ~PointsApiPoints() {}
};

class Space3d
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual float SizeZ() const=0;
  virtual int Map(float x, float y, float z) const=0;
};

class PropertyArray
{
public:
  virtual int NumProp() const=0;
  virtual unsigned int Color(int i) const=0;
};

class EnableArray
{
public:
  virtual int NumEnables() const=0;
  virtual void Enable(int i)=0;
  virtual void Disable(int i)=0;
};

class PointsInPlane
{
public:
  virtual int Size() const=0;
  virtual Point2d Map(int i) const=0;
  virtual unsigned int Color(int i) const=0;
};

class LinesInPlane
{
public:
  virtual int Size() const=0;
  virtual Point2d Map(int i, bool second) const=0;
  virtual unsigned int Color(int i, bool second) const=0;
};

class FacesInPlane
{
public:
  virtual int Size() const=0;
  virtual int NumPoints(int face) const=0;
  virtual Point2d Map(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Vector Normal(int face, int point) const { Vector v; v.dx = 0.0; v.dy=0.0; v.dz=-1.0; return v; }
  // note, no normal implemented
};

template<class T>
class PathValues
{
public:
  virtual float Size() const=0;
  virtual T get(float x) const=0;
};

template<class T>
class AreaValues
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual T get(float x, float y) const=0;
};
template<class T>
class VolumeValues
{
public:
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual float SizeZ() const=0;
  virtual T get(float x, float y, float z) const=0;
  virtual ~VolumeValues() { }
};

class TextureID
{
public:
  virtual ~TextureID() { }
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual void render(MainLoopEnv &e)=0;
  virtual int texture() const=0;
};

class WorldSpec
{
public:
  virtual int NumItems() const=0;
  virtual int PosX(int i) const=0;
  virtual int PosY(int i) const=0;
  virtual int Item(int i) const=0;
};
/*
class Cursor
{
public:
  virtual void update()=0;
  virtual int PosX() const=0;
  virtual int PosY() const=0;
};
*/

class ShaderI
{
public:
  virtual std::string funcname() const=0;
  virtual std::string shader() const=0;
};

class DynamicChange
{
public:
  virtual void applychange(float *source, float *target, int size_in_floats, MainLoopEnv &e)=0;
};

class ShaderBitmap
{
public:
  virtual std::string bitmapname() const=0;
  virtual std::string bitmapbody() const=0;
  virtual std::string bitmapbody_v_init() const=0;
  virtual std::string bitmapbody_v_body() const=0;
  virtual std::string bitmapbody_f_init() const=0;
  virtual std::string bitmapbody_f_body() const=0;
};


class DiskStore : public CollectInterface
{
public:
  virtual ~DiskStore() { }
  // types: 0=P, 1=BM, 2=VX, 3=urlmemorymap
  virtual void Prepare()=0;
  virtual int Type() const=0;
  virtual int NumBlocks() const=0;
  // blocktypes: 0=vertex, 1=color, 2=texcoord, 3=normal, 4=bitmap, 5=voxel, 6=vertexheader, 7=vertexpointcounts, 8=vertexindex, 9=texcoord3, 10=memoryblock
  virtual int BlockType(int block) const=0;
  virtual int BlockSizeInBytes(int block) const=0;
  virtual unsigned char* Block(int block) const=0;
  virtual unsigned char* BlockWithOffset(int block, int offset, int size) const=0;
};

class InputForMoving
{
public:
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual float XPos() const=0;
  virtual float YPos() const=0;
  virtual float ZPos() const=0;
};

class Glyph
{
public:
  virtual float Top() const=0;
  virtual float SizeX() const=0;
  virtual float SizeY() const=0;
  virtual float Baseline() const=0;
  virtual int NumSplits(float y) const=0;
  virtual float Split(float y, int idx) const=0;
  virtual int SplitType(float y, int idx) const=0; // ret: 1=empty->filled, 2=filled->empty, 3=empty->empty, 4=filled->filled
};

class CurveGroup : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual int NumCurves() const=0;
  virtual Point Pos(int num, float t) const=0; // t=[0..1]
  virtual unsigned int Color(int num, float t) const=0;
  virtual int Shape(int num, float t) const=0;
};

class MeshAnim : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual int NumFaces() const=0;
  virtual int NumPoints(int face) const=0;
  virtual float StartTime() const=0;
  virtual float EndTime() const=0;
  virtual Point Vertex(int face,int point, float time) const=0;
  virtual unsigned int Color(int face,int point, float time) const=0;
  virtual Vector Normal(int face, int point, float time) const=0;
  virtual Point2d TexCoord(int face, int point, float time) const=0;
  virtual float TexCoord3(int face, int point, float time) const=0;

};

class IStateMachine : public CollectInterface
{
public:
  virtual int num_flags() const=0;
  virtual std::string flag(int val) const=0;
  
  virtual void Prepare()=0;
  virtual void event(MainLoopEvent &e)=0;
  virtual void frame(MainLoopEnv &e)=0;
  virtual void draw_event(FrameLoopEvent &e)=0;
  virtual void draw_frame(DrawLoopEnv &e)=0;
};

template<class C>
class BitmapCollection : public CollectInterface
{
public:
  virtual int Size() const=0;
  virtual int SizeX(int i) const=0;
  virtual int SizeY(int i) const=0;
  virtual C Map(int i, int x, int y) const=0;
  virtual Point PosTL(int i) const=0;
  virtual Point PosBR(int i) const=0;
  virtual void Prepare()=0;
  virtual ~BitmapCollection() { }
};

class WorldBlocks
{
public:
  virtual ~WorldBlocks() { }
  // access elements (similar to Bitmap<int>)
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual int Map(int x, int y) const=0;

  // block sizes
  virtual int CellSizeX() const=0;
  virtual int CellSizeY() const=0;
  virtual void SetCellSize(int cx, int cy)=0;
  
  // set elements
  virtual void SetElem(int x, int y, int val)=0;
  virtual void SetElemBlock(int *array, int width, int height, int ydelta, int start_x, int start_y)=0;
  
  // Position of the blocks in 2d space
  virtual Point2d GetTL() const=0;
  virtual Point2d GetBR() const=0;
  virtual void SetExtends(Point2d p1, Point2d p2)=0;
  virtual std::pair<int,int> BlockPosition(Point2d pos) const=0;
  virtual std::pair<int,int> CellPosition(Point2d pos) const=0;
  virtual std::pair<float,float> CellPositionF(Point2d pos) const=0;
  virtual std::pair<Point2d,Point2d> BlockToWorld(int x, int y) const=0;

  // Position of the blocks in 3d space
  virtual Point GetTL3d() const=0;
  virtual Point GetBR3d() const=0;
  virtual void SetExtends3d(Point tl, Point tr, Point bl, Point br)=0;
  virtual void SetMV3d(Matrix m)=0;
  virtual std::pair<int,int> BlockPosition3d(Point ray_start, Point ray_end) const=0;
  virtual std::pair<int,int> CellPosition3d(Point ray_start, Point ray_end) const=0;
  virtual std::pair<float,float> CellPositionF3d(Point ray_start, Point ray_end) const=0;

  // Reallocating space
  virtual void ReserveSize(int sx, int sy)=0;  

  // enemy positions
  virtual void clear_enemies()=0;
  virtual void add_enemy(float start_x, float end_x, float start_y, float end_y)=0;
  virtual int num_enemies() const=0;
  virtual float enemy_start_x(int e) const=0;
  virtual float enemy_end_x(int e) const=0;
  virtual float enemy_start_y(int e) const=0;
  virtual float enemy_end_y(int e) const=0;
};

WorldBlocks *GetWorld();
void SetWorld(WorldBlocks *w);

class HeavyOperation
{
public:
  virtual ~HeavyOperation();
  virtual bool RequestPrepares() const=0;
  virtual void TriggerPrepares()=0;
  virtual int NumPrepares() const=0;
  virtual void Prepare(int prepare)=0;
  virtual int NumSlots() const=0;
  virtual void Slot(int slot)=0;
  virtual void FinishSlots()=0;
  virtual void* get_data(std::string type)=0;
  virtual void set_usage_index(int slot)=0;
  //virtual bool IsReadyForTriggerFrames() const=0;
  //virtual void TriggerFrames()=0;
  //virtual void handle_event(MainLoopEvent &e)=0;
  //virtual void draw_event(FrameLoopEvent &e)=0;
  //virtual void frame(MainLoopEnv &e)=0;
  //virtual void draw_frame(DrawLoopEnv &e)=0;
};

class QMLLoop : public FrameBufferLoop
{
public:
  virtual int NumChildren() const=0;
  virtual QMLLoop *Children(int i) const=0;

};


class QMLData : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual QMLData* Parent() const=0;
  virtual std::string Type() const=0;
  virtual int NumImports() const=0;
  virtual std::string Import(int import_i) const=0;
  virtual int NumAttribs() const=0;
  virtual std::string AttrName(int attr_i) const=0;
  virtual std::string AttrValue(int attr_i) const=0;
  virtual int NumChildren() const=0;
  virtual std::string Name(int child_i) const=0;
  virtual std::string Type(int child_i) const=0;
  virtual QMLData *Children(int child_i) const=0;
};


class SkeletalData : public CollectInterface
{
public:
  virtual void Prepare()=0;

  virtual int NumBones() const=0;
  virtual Point Bone(int i, bool is_first) const=0;
  virtual float BoneDeltaAngle(int i) const=0;

  // points are using cylindar coordinates
  virtual int NumPoints() const=0;
  virtual int PointAttachBone(int pt) const=0;
  virtual float DistFromBone(int pt) const=0;
  virtual float PosAtBone(int pt) const=0;
  virtual float BoneAngle(int pt) const=0;
};

class FrmWidget : public CollectInterface
{
public:
  FrmWidget() : x(0), y(0), w(0), h(0) { }
  virtual ~FrmWidget() { }
  virtual void Prepare()=0;
  virtual void handle_event(FrameLoopEvent &e)=0;
  virtual void frame(DrawLoopEnv &e)=0;
public:
  virtual void set_pos(int x_, int y_) { x=x_; y=y_; }
  virtual void set_size(int w_, int h_) { w=w_; h=h_; }
  virtual void set_label(std::string s) { /* override this in derived class */ }
  int pos_x() const { return x; }
  int pos_y() const { return y; }
  int size_w() const { return w; }
  int size_h() const { return h; }
public:
  int x,y;
  int w,h;
};
struct PosDelta {
  std::vector<int> l,t,W,H,r,b;
};
struct PosOption {
  int l,t,W,H,r,b;
};
const int Pos_E = -16384;
class FrmContainerWidget : public FrmWidget
{
public:
  FrmContainerWidget(GameApi::Env &e, std::vector<FrmWidget*> wid, std::string url, std::string homepage) : e(e), wid(wid), url(url), homepage(homepage) { }
  void Collect(CollectVisitor &vis);
  void HeavyPrepare();
  virtual void Prepare();
  virtual void set_size(int w_, int h_); 
    void print_vec(std::vector<int> v);
    virtual void set_pos(int x_, int y_);

  virtual void handle_event(FrameLoopEvent &e)
  {
    int s = std::min(std::min(wid.size(),pos.size()),option_num.size());
    for(int i=0;i<s;i++) {
      wid[i]->handle_event(e);
    }

  }
  virtual void frame(DrawLoopEnv &e)
  {
    int s = std::min(std::min(wid.size(),pos.size()),option_num.size());
    for(int i=0;i<s;i++) {
      wid[i]->frame(e);
    }
  }
  void set_option(int i, int l, int t, int W, int H, int r, int b) {
    PosOption o;
    o.l = l;
    o.t = t;
    o.W = W;
    o.H = H;
    o.r = r;
    o.b = b;
    option_num[i] = o;
  }
private:
  GameApi::Env &e;
  std::vector<FrmWidget*> wid;
  std::vector<PosDelta> pos;
  std::vector<PosOption> option_num;
  std::string url;
  std::string homepage;
};

class FrmRootWidget : public FrameBufferLoop
{
public:
  FrmRootWidget(FrmWidget *w, int scrx, int scry) : w(w), scrx(scrx), scry(scry) { }
  void Collect(CollectVisitor &vis) { w->Collect(vis); vis.register_obj(this); }
  void HeavyPrepare() {
    w->set_pos(0,0);
    w->set_size(scrx,scry);
  }  
  virtual void Prepare() {
    w->Prepare();
    w->set_pos(0,0);
    w->set_size(scrx,scry);
  }
  virtual void handle_event(FrameLoopEvent &e) { w->handle_event(e); }
  virtual void frame(DrawLoopEnv &e) { w->frame(e); }
private:
  FrmWidget *w;
  int scrx, scry;
};

class ColorBool
{
public:
  virtual bool is_included(unsigned int color) const=0;
};

class VoxelArray : public CollectInterface
{
public:
  virtual ~VoxelArray() { }
  virtual void Prepare()=0;
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual int SizeZ() const=0;
  virtual int Size() const=0;
  
  struct Pos { int x; int y; int z; };
  virtual Pos Map(int i) const=0;
  virtual unsigned int Color(int i) const=0;
  virtual int State(int i) const=0;
};

struct Vertex
{
  Point p;
  Vector n;
  unsigned int c;
  Point2d tx;
  float tx3;
};

struct Extends
{
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};


class QuadNode
{
public:
  QuadNode *parent;
  std::vector<QuadNode*> child;
  std::vector<Extends> child_extends;
};


class QuadTree
{
public:
  virtual int RootType() const=0;
  virtual Extends RootExtends() const=0;
  virtual QuadNode *Root() const=0;
  virtual int NumChildren(QuadNode *n) const=0;
  virtual QuadNode *Child(int i, QuadNode *n) const=0;
  virtual Extends ChildExtends(int i, QuadNode *n) const=0;
  virtual int ChildType(int i, QuadNode *n) const=0;
  virtual QuadNode *Parent(QuadNode *n) const { return n->parent; }
public:
  void push_tri(Vertex v1, Vertex v2, Vertex v3);
  void push_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
  void push_poly(std::vector<Vertex> vec);
};

bool is_inside_extends(Point p, Extends e);

struct Prop3d
{
  Prop3d *parent;

  bool has_extends;
  bool has_focus;
  bool has_content_color;
  bool has_border_enabled;
  bool has_border_width;
  bool has_border_color;
  bool has_content_darkness;
  bool has_red_multiplier;
  bool has_green_multiplier;
  bool has_blue_multiplier;
  bool has_red_adder;
  bool has_green_added;
  bool has_blue_adder;
  bool has_alpha_channel;
  bool has_corner_radius;
  bool has_text_color;
  bool has_text_z_coord;
  bool has_font_name;
  bool has_font_size;
  bool has_background_color;

  Extends extends;
  bool focus;
  unsigned int content_color;
  bool border_enabled;
  float border_width;
  unsigned int border_color;
  float content_darkness; // 0.0 .. 1.0
  float red_multiplier;
  float green_multiplier;
  float blue_multiplier;
  float red_adder;
  float green_adder;
  float blue_adder;
  float alpha_channel;
  float corner_radius;
  unsigned int text_color;
  bool text_z_coord;
  std::string font_name;
  float font_size;
  unsigned int background_color;
  
};

class Object3d : public CollectInterface
{
public:
  virtual int current_state() const=0;
  virtual Prop3d &prop(int state)=0;
    
  virtual void Prepare()=0;
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;
  virtual int shader_id() { return -1; }
  virtual void destroy() { }
};


class Html : public CollectInterface
{
public:
  virtual ~Html() { }
  virtual void SetCB(void(*fptr)(void*), void*data)=0;
  virtual void Prepare()=0;
  virtual std::string script_file() const=0;
  virtual std::string homepage() const=0;
};


struct V_Voxel
{
  unsigned char type;
};

struct V_Voxel_Type
{
  std::string voxel_name;
  int /*GameApi::P*/ obj;
};
struct V_Voxel_Type_Array
{
  std::vector<V_Voxel_Type> vec;
};

struct V_Object
{
  int obj_type; // index to g_object_types
  //bool is_pos;
  float x,y,z;
  //bool is_matrix;
  //Matrix m;
  float radius;
};
struct V_Object_Pos
{
  std::string pos_name;
  std::vector<V_Object> pos;
};

struct V_Object_Type
{
  int /*GameApi::P*/ obj;
  int /*GameAPi::MN*/ move;
  int /*GameAPi::MT*/ mat;
  float radius;
};
struct V_Object_Type_Array
{
  std::string type_name;
  std::vector<V_Object_Type> vec;
};


struct V_Wall
{
  int start_x, end_x;
  int start_y, end_y;
  int start_z, end_z;
};
struct V_Wall_Array
{
  std::string name;
  std::vector<V_Wall> vec;
};



struct V_Area_Type
{
  std::string name;
  int voxel_types=-1; // index to g_voxel_types
  // dimensions
  float start_x, start_y, start_z;
  float end_x, end_y, end_z;
  // handle
  float pos_x, pos_y, pos_z; // between start_x..end_x, start_y..end_y, start_z..end_z
  // number of voxels
  int size_x, size_y, size_z;
  V_Voxel *voxels;
  // heightmap
  int /*GameApi::FB*/ ground_heightmap = -1;

  // textures
  int /*GameApi::BM*/ top_texture = -1;
  float top_start_x;
  float top_end_x;
  float top_start_z;
  float top_end_z;

  int /*GameApi::BM*/ left_texture = -1;
  float left_start_y;
  float left_end_y;
  float left_start_z;
  float left_end_z;

  //int /*GameApi::BM*/ right_texture;
  //float right_start_y;
  //float right_end_y;
  //float right_start_z;
  //float right_end_z;
  
  int /*GameApi::BM*/ front_texture = -1;
  float front_start_x;
  float front_end_x;
  float front_start_y;
  float front_end_y;

  //int /*GameApi::BM*/ back_texture;
  //float back_start_x;
  //float back_end_x;
  //float back_start_y;
  //float back_end_y;

  // walls
  int wall=-1; // index to g_walls

  int obj_type=-1;
  int obj_pos=-1;
};


struct V_Area
{
  int area_type; 
};
struct V_Area_Pos
{
  std::vector<V_Area> vec;
  std::vector<Point> pos;
};

extern std::vector<V_Voxel_Type_Array*> g_voxel_types;
extern std::vector<V_Object_Pos> g_object_pos;
extern std::vector<V_Object_Type_Array*> g_object_types;
extern std::vector<V_Wall_Array*> g_walls;
extern std::vector<V_Area_Type> g_area_type_array;
extern std::vector<V_Area_Pos> g_areas;


struct Rect5
{
  float x,y;
  float sx,sy;
};

class RectangleArray : public CollectInterface
{
public:
  virtual void Prepare() =0;
  virtual int NumRects() const=0;
  virtual Rect5 GetRect(int i) const=0;
  virtual int Type(int i) const=0;
};


class LoadStream : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual LoadStream *Clone()=0;
  virtual bool get_ch(unsigned char &ch)=0;
  virtual bool get_line(std::vector<unsigned char> &line)=0;
  virtual bool get_file(std::vector<unsigned char> &file)=0;
};

class PlayerStrings
{
public:
  virtual int NumPlayers() const=0;
  virtual std::string Player_ID(int i) const=0;
  virtual std::string Player_Prop(int i) const=0;
};

class DynMainLoop
{
public:
  virtual int num() const=0;
  virtual MainLoopItem *get_mainloop(int i) const=0;
};

#if 0
class SkeletalAnimMap
{
public:
  // initial pose
  virtual int NumPoints() const=0;
  virtual Vector GetPoint(int p) const=0;
  virtual int Parent(int p) const=0; // -1 = no parent
  virtual float Angle(int p) const=0;

  // mesh coordinates relative to pose
  virtual int FacePoint_PointNum(int face, int point) const=0;
  virtual float SpanPos(int face, int point) const=0;
  virtual float Dist(int face, int point) const=0;
  virtual float Angle(int face, int point) const=0;

  // animation frames
  virtual int NumKeyFrames() const=0;
  virtual Vector GetPoint(int k, int p) const=0;
  virtual float Angle(int k, int p) const=0;
  
  // keyframe mapping
  virtual int NumKeyFrameTransitions() const=0;
  virtual std::pair<int,int> trans(int t) const=0;  
};
#endif

class FloatArray2
{
public:
  virtual int Num() const=0;
  virtual float Map(int i) const=0;
};
class FloatField
{
public:
  virtual float Map(Point p) const=0;
};

class VelocityField
{
public:
  virtual Vector Map(Point p) const=0;
};


struct InEventData
{
  Point pos;
  float radius;
};

class EventSource : public CollectInterface
{ // this is only for child graphs to generate events
  // i.e. generating events
  // use MainLoopItem for sending events.
public:
  virtual void Prepare()=0;
  virtual void handle_event(MainLoopEvent &event)=0;
  virtual void execute(MainLoopEnv &e)=0;
  
  // returns number of events still that can be fetched
  virtual int get_game_event(InEventData &out_data)=0;
};

#if 0
class SubDivide
{
public:
  virtual int num_ids() const=0;
  virtual int id(std::string s) const=0;
  virtual void set_pos(int id, Point p)=0;
  virtual void set_delta(int out_id, int in_id, Vector v)=0;
  virtual void set_span(int out_id, int in_id1, int in_id2)=0;
  virtual void set_span_value(int out_id, int id, float val)=0;
  virtual void set_curve(int out_id, int in_id, CurveIn3d &curve)=0;
  virtual void set_curve_value(int out_id, int id, float val)=0;
  
  virtual void Prepare()=0;
  virtual void handle_event(MainLoopEvent &event)=0;
  virtual void execute(MainLoopEnv &e)=0;
};
#endif

class BitmapRange
{
public:
  virtual int NumSpans() const=0;
  virtual int NumRanges(int span) const=0;
  virtual int StartRange(int span, int range) const=0;
  virtual int EndRange(int span, int range) const=0;
};

class GameState
{
public:
  virtual float *index_float(int i) const=0;
  virtual int *index_int(int i) const=0;
  virtual std::string *index_string(int i) const=0;
};


class GCBitmap : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual unsigned int Map(int x, int y) const=0;
};

class GraphicsContext
{
public:
  virtual int size_x() const=0;
  virtual int size_y() const=0;

  virtual void StartBatch(std::string name)=0;
  virtual void EndBatch(std::string name)=0;
  
  virtual void draw_pixel(int x, int y, unsigned int color)=0;
  //virtual unsigned int get_pixel(int x, int y) const=0; // ei toimi henrin platassa
  virtual void draw_rect(int x, int y, int sx, int sy, unsigned int color)=0;
  virtual void draw_image(int x, int y, int sx, int sy, GCBitmap &bitmap)=0;
  virtual void draw_image2(int x, int y, int sx, int sy, GCBitmap &bitmap)=0;
  virtual void draw_line(int x, int y, int x2, int y2, unsigned int color)=0;

  // text drawing not included, but should do Bitmap<Color> *draw_text(std::string label, Font *font);
};
GCBitmap *to_gcbitmap(Bitmap<Color> *bm);

class TimeAnim : public CollectInterface
{
public:
  virtual void Collect(CollectVisitor &vis)=0;
  virtual void HeavyPrepare()=0;
  virtual void Prepare()=0;
  virtual float start_time() const=0;
  virtual float end_time() const=0;
  virtual FaceCollection *get_frame(float time) const=0;
};


class GLTFModelInterface : public CollectInterface
{
public:
  virtual void Prepare()=0;
  virtual void Collect(CollectVisitor &vis)=0;
  virtual void HeavyPrepare()=0;

  virtual std::string BaseUrl() const=0;
  virtual std::string Url() const=0;

  virtual int get_default_scene() const=0;
  
  virtual int accessors_size() const=0;
  virtual const tinygltf::Accessor &get_accessor(int i) const=0;

  virtual int animations_size() const=0;
  virtual const tinygltf::Animation &get_animation(int i) const=0;

  virtual int buffers_size() const=0;
  virtual const tinygltf::Buffer &get_buffer(int i) const=0;

  virtual int bufferviews_size() const=0;
  virtual const tinygltf::BufferView &get_bufferview(int i) const=0;

  virtual int materials_size() const=0;
  virtual const tinygltf::Material &get_material(int i) const=0;

  virtual int meshes_size() const=0;
  virtual const tinygltf::Mesh &get_mesh(int i) const=0;

  virtual int nodes_size() const=0;
  virtual const tinygltf::Node &get_node(int i) const=0;

  virtual int textures_size() const=0;
  virtual const tinygltf::Texture &get_texture(int i) const=0;

  virtual int images_size() const=0;
  virtual const tinygltf::Image &get_image(int i) const=0;

  virtual int skins_size() const=0;
  virtual const tinygltf::Skin &get_skin(int i) const=0;

  virtual int samplers_size() const=0;
  virtual const tinygltf::Sampler &get_sampler(int i) const=0;

  virtual int cameras_size() const=0;
  virtual const tinygltf::Camera &get_camera(int i) const=0;

  virtual int scenes_size() const=0;
  virtual const tinygltf::Scene &get_scene(int i) const=0;

  virtual int lights_size() const=0;
  virtual const tinygltf::Light &get_light(int i) const=0;
};


struct BBox
{
  int id;
  float start_x;
  float end_x;
  float start_y;
  float end_y;
};

class Program;

class ShaderBlock
{
public:
  virtual int NumChildren() const=0;
  virtual const ShaderBlock *Children(int i) const=0;

  virtual int get_uid() const=0;
  
  virtual float start_time() const=0;
  virtual float end_time() const=0;

  virtual BBox BoundingBox(float time) const=0;

  virtual std::string GetDistField() const=0; // distfield<uid>::((x,y,z,t),(xx,yy))->float
  virtual std::string GetColor() const=0;     // color<uid>::((x,y,z,t),(xx,yy))->vec4
  virtual std::string GetNormal() const=0;    // normal<uid>::((x,y,z,t),(xx,yy))->vec3
  virtual std::string GetTexCoord() const=0;  // texcoord<uid>::((x,y,z,t),(xx,yy))->vec3
  
  virtual void set_vars(Program *sh)=0; // get program from SH type.
  // set_vars must set bbox[id].start_x bbox[id].end_x bbox[id].start_y bbox[id].end_y
  
  // use global function: std::string generate_shader_block_shader(const ShaderBlock *block);
};

class Scalar
{
public:
  virtual float get() const=0;
};

class Vec2
{
public:
  virtual float get_x() const=0;
  virtual float get_y() const=0;
};

class Vec3
{
public:
  virtual float get_x() const=0;
  virtual float get_y() const=0;
  virtual float get_z() const=0;
};

class Vec4
{
public:
  virtual float get_x() const=0;
  virtual float get_y() const=0;
  virtual float get_z() const=0;
  virtual float get_w() const=0;
  float get_r() const { return get_x(); }
  float get_g() const { return get_y(); }
  float get_b() const { return get_z(); }
  float get_a() const { return get_w(); }
};

class Mat2_a
{
public:
  virtual float get_mat(int x, int y) const=0;
};
class Mat3_a
{
public:
  virtual float get_mat(int x, int y) const=0;
};
class Mat4_a
{
public:
  virtual float get_mat(int x, int y) const=0;
};


class CollectVisitor2;
class CollectInterface2
{
public:
  virtual ~CollectInterface2() { }
  virtual void Collect(CollectVisitor2 &vis)=0;
  virtual void HeavyPrepare()=0;
};

class CollectVisitor2
{
public:
  virtual ~CollectVisitor2() { }
  virtual void register_obj(CollectInterface2 *i)=0;
  virtual void register_child(int num, CollectInterface2 *i)=0;
};


struct FloatBinding
{
  std::string key;
  float *value;
};
struct IntBinding
{
  std::string key;
  int *value;
};
struct UnsignedIntBinding
{
  std::string key;
  unsigned int *value;
};
struct PointBinding
{
  std::string key;
  Point *value;
};


class BindingsI
{
public:
  virtual int F_Size() const=0;
  virtual const FloatBinding &F_get(int i) const=0;
  virtual int I_Size() const=0;
  virtual const IntBinding &I_get(int i) const=0;
  virtual int U_Size() const=0;
  virtual const UnsignedIntBinding &U_get(int i) const=0;
  virtual int P_Size() const=0;
  virtual const PointBinding &P_get(int i) const=0;

  virtual void set(GameApi::EveryApi &ev, int sh)=0;
};

class Bindings : public BindingsI
{
public:
  Bindings(const Bindings &c) : f_vec(c.f_vec), i_vec(c.i_vec),u_vec(c.u_vec),p_vec(c.p_vec) { }
  Bindings(const Bindings &c, FloatBinding n) : f_vec(c.f_vec), i_vec(c.i_vec),u_vec(c.u_vec),p_vec(c.p_vec) { f_vec.push_back(n); }
  Bindings(const Bindings &c, IntBinding n) : f_vec(c.f_vec), i_vec(c.i_vec),u_vec(c.u_vec),p_vec(c.p_vec) { i_vec.push_back(n); }
  Bindings(const Bindings &c, UnsignedIntBinding n) : f_vec(c.f_vec), i_vec(c.i_vec), u_vec(c.u_vec),p_vec(c.p_vec) { u_vec.push_back(n); }
  Bindings(const Bindings &c, PointBinding n) : f_vec(c.f_vec), i_vec(c.i_vec), u_vec(c.u_vec),p_vec(c.p_vec) { p_vec.push_back(n); }
  Bindings(const Bindings &c1, const Bindings &c2) : f_vec(c1.f_vec), i_vec(c1.i_vec), u_vec(c1.u_vec),p_vec(c1.p_vec) {
    int fs = c2.F_Size();
    for(int i=0;i<fs;i++)
      {
	f_vec.push_back(c2.F_get(i));
      }
    int is = c2.I_Size();
    for(int i=0;i<is;i++)
      {
	i_vec.push_back(c2.I_get(i));
      }
    int us = c2.U_Size();
    for(int i=0;i<us;i++)
      {
	u_vec.push_back(c2.U_get(i));
      }
    int ps = c2.P_Size();
    for(int i=0;i<ps;i++)
      {
	p_vec.push_back(c2.P_get(i));
      }
    
  }
  int F_Size() const { return f_vec.size(); }
  const FloatBinding &F_get(int i) const { return f_vec[i]; }
  int I_Size() const { return i_vec.size(); }
  const IntBinding &I_get(int i) const { return i_vec[i]; }
  int U_Size() const { return u_vec.size(); }
  const UnsignedIntBinding &U_get(int i) const { return u_vec[i]; }
  int P_Size() const { return p_vec.size(); }
  const PointBinding &P_get(int i) const { return p_vec[i]; }
  void set(GameApi::EveryApi &ev, int sh);
protected:
  Bindings() { }
private:
  std::vector<FloatBinding> f_vec;
  std::vector<IntBinding> i_vec;
  std::vector<UnsignedIntBinding> u_vec;
  std::vector<PointBinding> p_vec;
};

class ShaderCode
{
public:
  virtual ~ShaderCode() { }
  virtual std::string Code() const=0;
};
class ShaderI2 : public CollectInterface2
{
public:
  virtual void set_inner(int num, std::string value)=0; 
  virtual std::string get_webgl_header() const=0;
  virtual std::string get_win32_header() const=0;
  virtual std::string get_webgl_function() const=0;
  virtual std::string get_win32_function() const=0;
  virtual Bindings set_var(const Bindings &b)=0;
  virtual std::string get_flags() const=0;
  virtual std::string func_name() const=0;
  virtual void execute(MainLoopEnv &e)=0;
  virtual void handle_event(MainLoopEvent &e)=0;

  // set_inner needs to be called before each call of get_[webgl|win32]_[header|function]()
};
class ShaderParameterI
{
public:
  virtual void set_time(float time)=0;
  //virtual bool enabled() const=0;
  virtual float param_value_f(int i) const=0;
  virtual int param_value_i(int i) const=0;
  virtual unsigned int param_value_u(int i) const=0;
  virtual Point param_value_p3d(int i) const=0;
  virtual Point param_value_uvw(int i) const=0;
};


class ASyncTask
{
public:
  virtual int NumTasks() const=0;
  virtual void DoTask(int i)=0;
};

class PixelBufferObject : public CollectInterface
{
public:
  virtual unsigned int Id() const=0;
  virtual void Prepare()=0;
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
};

class TexturedAlternatives
{
public:
  virtual int NumArray() const=0; // a
  virtual int NumAlts() const=0; // i
  virtual int Alt(int a, int i) const=0;
};

class TexturedVoxelArea;
class TexturedAreaArray
{
public:
  virtual int Size() const=0;
  virtual TexturedVoxelArea *Index(int i) const=0;
  virtual int Left(int i) const=0;
  virtual int Top(int i) const=0;
  virtual int Right(int i) const=0;
  virtual int Bottom(int i) const=0;
  virtual int SX(int i) const=0;
  virtual int SY(int i) const=0;
  virtual int SZ(int i) const=0;
  virtual int Front(int i) const=0;
  virtual int Back(int i) const=0;
};

class TexturedVoxelArea
{
public:
  TexturedVoxelArea() { }
  virtual int Parent() const=0;
  virtual Bitmap<Color>* texture() const=0;
  virtual FaceCollection* faces() const=0;
  virtual int bounding_start_x() const=0;
  virtual int bounding_end_x() const=0;
  virtual int bounding_start_y() const=0;
  virtual int bounding_end_y() const=0;
  virtual int bounding_start_z() const=0;
  virtual int bounding_end_z() const=0;
};


class TexturedVoxel
{
public:
  TexturedVoxel(int num) : num(num) { }
  virtual int SizeX() const { return 192; }
  virtual int SizeY() const { return num*32; }
  virtual Color Map(int x, int y) const=0;

  virtual int SX() const=0;
  virtual int SY() const=0;
  virtual int SZ() const=0;
  virtual bool IsEnabled(int x, int y, int z) const { return Map(x,y,z)!=0; }
  virtual unsigned char Map(int x, int y, int z) const=0;
private:
  int num;
};

class TexturedQuads
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual Color Map(int x, int y) const=0;

  virtual int Size() const=0;
  virtual Point P1(int i) const=0;
  virtual Point P2(int i) const=0;
  virtual Point P3(int i) const=0;
  virtual Point P4(int i) const=0;
  virtual int Type(int i) const=0;

  virtual Point2d start(int type) const=0;
  virtual Point2d end(int type) const=0;
};


class MarchingCubesInputData
{
public:
  virtual int start_x() const=0;
  virtual int end_x() const=0;
  virtual int start_y() const=0;
  virtual int end_y() const=0;
  virtual int start_z() const=0;
  virtual int end_z() const=0;

  virtual int startcell_x() const=0;
  virtual int startcell_y() const=0;
  virtual int startcell_z() const=0;
  
  virtual bool Map(int x, int y, int z) const=0;
  virtual Vector gradient(int x, int y, int z) const=0;
};

class MarchingCubesCellInputData
{
public:
  virtual bool Map(bool x, bool y, bool z) const=0;
  virtual Vector gradient(bool x, bool y, bool z) const=0;
};

class MarchingCubesCellEdge
{
public:
  virtual bool start(unsigned char c1, unsigned char c2)=0;
  virtual bool end(unsigned char c1, unsigned char c2)=0;
  virtual Point pos(unsigned char c1, unsigned char c2)=0;
  virtual Vector gradient(unsigned char c1, unsigned char c2)=0;
};

class MarchingCubesCellPolygon
{
public:
  virtual int NumPoints() const=0;
  virtual Point FacePoint(int point) const=0;
  virtual Vector PointNormal(int point) const=0;
};

class MarchingCubesMesh
{
public:
  virtual int NumPolygons(int x, int y, int z) const=0; // returns 0..4
  virtual MarchingCubesCellPolygon *Polygon(int x, int y, int z, int poly_num) const=0;
};


class IWorld
{
public:
  virtual int NumBlocks() const=0;
  virtual int BlockType(int i) const=0;
  virtual Point BlockPos(int i) const=0;
  virtual Matrix BlockRot(int i) const=0;
};

class ICache
{
public:
  virtual int Num() const=0;
  virtual FaceCollection *GetFaces(int i) const=0;
  virtual Material *GetMaterial(int i) const=0; 
};


class Platform
{
public:
  virtual bool Allow(float x, float z) const=0;
  virtual float Height(float x, float z) const=0;
  virtual FaceCollection *Render() const=0;
};

#endif

