
#ifndef GRAPHI_HH
#define GRAPHI_HH

#include <vector>
#include <utility>
#include "VectorTools.hh"

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
class Bitmap
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual C Map(int x, int y) const=0;
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
  virtual ~GuiWidget() { }
  virtual Point2d get_pos() const=0;
  virtual Vector2d get_size() const=0;
  virtual void set_pos(Point2d pos)=0;
  virtual void set_size(Vector2d size)=0;
  virtual void update(Point2d mouse_pos, int button)=0;
  virtual void render()=0;
  virtual int render_to_bitmap()=0; // returns bitmap id
  virtual void select_item(int item)=0;
  virtual int chosen_item() const=0;
  virtual float dynamic_param(int id) const=0;
  virtual void set_dynamic_param(int id, float val)=0;
  virtual int child_count() const=0;
  virtual GuiWidget *child(int num) const=0;
};

class GuiWidgetForward : public GuiWidget
{
public:
  GuiWidgetForward(GameApi::EveryApi &ev, std::vector<GuiWidget*> vec) : ev(ev), vec(vec) { pos=Point2d::NewPoint(0.0, 0.0); size.dx=0.0; size.dy=0.0; current_selected_item=-1; }
  virtual Point2d get_pos() const { return pos; }
  virtual Vector2d get_size() const { return size; }
  virtual void set_pos(Point2d new_pos) {
    Point2d old_pos = pos;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	Point2d child_old_pos = w->get_pos();
	Point2d child_new_pos = child_old_pos;
	child_new_pos -= old_pos;
	child_new_pos += new_pos;
	w->set_pos(child_new_pos);
      }
    pos = new_pos;
  }
  virtual void set_size(Vector2d size_p)
  {
    size = size_p;
    // derived widget must override and implement to set child widgets.
  }
  virtual void update(Point2d mouse_pos, int button)
  {
    int s = vec.size();
    int selected_item = -1;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	w->update(mouse_pos, button);
	
	Point2d p = w->get_pos();
	Vector2d s = w->get_size();
	if (mouse_pos.x >= p.x && mouse_pos.x < p.x+s.dx &&
	    mouse_pos.y >= p.y && mouse_pos.y < p.y+s.dy)
	  {
	    selected_item = i;
	  }
      }
    if (button==0) {
      current_selected_item = selected_item;
    }
    if (button==-1) {
      current_selected_item = -1;
    }
  }
  virtual void render()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	w->render();
      }
  }
  virtual int render_to_bitmap();

  virtual void select_item(int item) {
    current_selected_item=item; 
  }
  virtual int chosen_item() const
  {
    return current_selected_item;
  }
  virtual float dynamic_param(int id) const { return 0.0; }
  virtual void set_dynamic_param(int id, float val)
  {
  }
  virtual int child_count() const { return vec.size(); }
  virtual GuiWidget *child(int num) const { return vec[num]; }
protected:
  GameApi::EveryApi &ev;
  Point2d pos;
  Vector2d size;
  int current_selected_item;
  std::vector<GuiWidget *> vec;
};

#endif
