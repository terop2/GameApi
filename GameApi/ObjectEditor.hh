#include "VectorTools.hh"

template<class Loop>
class LoopCircles
{
public:
  virtual Loop NewLoop(Point p, int points) const=0;
  virtual Loop CombineLoops(Loop l1, Loop l2, Point2d p) const=0;
  virtual Loop MoveLoopXY(Loop l1, Vector2d v) const=0;
  virtual Loop MoveLoopZ(Loop l1, float z) const=0;
  virtual void DestroyLoop(Loop l) const=0;
  virtual Loop SizeLoop(Loop l, Point2d p, float dist) const=0;
  virtual Loop LoopShape(Loop l, int shape_num) const=0;
  virtual void LoopBlit(Loop l1, Loop l2)=0; // generates polys
  virtual Loop MoveLoopPoint(Loop l1, int point, Vector2d v) const=0;
  virtual int LoopPointCount(Loop l1) const=0;
  virtual std::pair<Loop,Loop> SplitLoop(Loop l, int point1, int point2) const=0;
  virtual Loop AddPoint(Loop l1, int p1, int p2) const=0;
  virtual Loop GroupLoops(const std::vector<Loop> &loops) const=0;
  virtual std::vector<Loop> UnGroupLoops(Loop l) const=0;  
};

template<class Plane>
class PlaneManip
{
public:
  virtual Plane NewPlane() const=0;
  virtual Plane NewPlane(Plane p, float dist) const=0;
  virtual Plane NewPlaneRotateZ(Plane p, Point2d pp, float angle) const=0;
  virtual Plane NewPlaneRotateX(Plane p, Point2d pp, float angle) const=0;
  virtual Plane NewPlaneRotateY(Plane p, Point2d pp, float angle) const=0;
  virtual Plane MovePlane(Plane p, Vector2d p) const=0;
};

template<class Surface, class Loop>
class SurfaceManip
{
public:
  virtual Surface FindSurf(Loop l1, Loop l2, int p1, int p2) const=0;
  virtual void Color(Surface &s, Color c) const=0; 
  virtual void Texture(Surface &s, int texture) const=0;
  virtual void Light(Surface &s, int light) const=0;
};

class ZoomManip
{
public:
  virtual void IncZoom() const=0;
  virtual void DecZoom() const=0;
};

template<class Plane>
class PlaneSelect
{
public:
  virtual std::vector<Plane> Planes() const=0;
  virtual void SelectPlane(Plane p) =0;
};

class Draw
{
public:
  virtual void Clear()=0;
  virtual void DrawLine(Point2d p, Point2d p2)=0;
  virtual void DrawPoint(Point2d p, float pointsize)=0;
};

class DrawSelect
{
public:
  virtual Point2d MousePos() const=0;
  virtual bool MouseDragged() const=0;
  virtual int LastMouseButton() const=0;
};

struct MLoop
{
  std::vector<Point2d> p;
  float z;
  std::vector<MLoop*> group;
};

template<class Object>
class List
{
public:
  virtual void Clear()=0;
  virtual void AddSurface(Object s)=0;
  virtual int CurrentlySelectedItem() const=0;
};

class FrameControl
{
public:
  virtual void SetTime(int frame) const=0;
};
