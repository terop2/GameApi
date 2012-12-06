#ifndef PLANE_HH
#define PLANE_HH

#include "Effect.hh"


class GridIn2d
{
public:
  virtual int XSize() const=0;
  virtual int YSize() const=0;
  virtual Point2d Index(int x, int y) const=0;
};

class Polygons2d
{
public:
  virtual int NumFaces() const=0;
  virtual int NumPoints(int face) const=0;
  virtual Point2d FacePoint(int face, int point) const=0;
};

class Polygons2dFromFaceCollection : public Polygons2d
{
public:
  Polygons2dFromFaceCollection(FaceCollection &coll, int face, Plane pl) : coll(coll), face(face), pl(pl) { }
  int NumFaces() const { return 1; }
  int NumPoints(int /*face*/) const
  {
    return coll.NumPoints(face);
  }
  Point2d FacePoint(int /*face*/, int point) const
  {
    Point p = coll.FacePoint(face, point);
    float x = pl.CoordsX(p);
    float y = pl.CoordsY(p);
    Point2d pp;
    pp.x = x;
    pp.y = y;
    return pp;
  }
private:
  FaceCollection &coll;
  int face;
  Plane pl;
};

class TriangulateConvexPolygons : public Polygons2d
{
public:
  TriangulateConvexPolygons(Polygons2d &pol) : pol(pol) { }
  void Gen()
  {
    int facescount = pol.NumFaces();
    for(int i=0;i<facescount;i++)
      {
	int line1 = 0;
	int line2 = 1;
	
	int third = 2;
	int size = pol.NumPoints(i);
	while(third < size)
	  {
	    Tri t;
	    t.a = line1;
	    t.b = line2;
	    t.c = third;
	    
	    triangles.push_back(t);
	    faces.push_back(i);
	    line2 = third;
	    third = line2 + 1;
	  }
      }
  }

  int NumFaces() const { return triangles.size(); }
  int NumPoints(int face) const
  {
    return 3;
  }
  Point2d FacePoint(int face, int point) const
  {
    Tri t = triangles[face];
    int index;
    if (point == 0) index = t.a;
    if (point == 1) index = t.b;
    if (point == 2) index = t.c;
    return pol.FacePoint(faces[face], index);
  }
  struct Tri
  {
    int a,b,c;
  };
private:
  Polygons2d &pol;
  std::vector<int> faces;
  std::vector<Tri> triangles;
};

class Polygons2dFaceCollection : public FaceCollection
{
public:
  Polygons2dFaceCollection(Polygons2d &p, Plane pl) : p(p), pl(pl) { }
  int NumFaces() const 
  {
    return p.NumFaces();
  }
  int NumPoints(int face) const
  {
    return p.NumPoints(face);
  }
  Point FacePoint(int face, int point) const
  {
    PlaneIn3d pp(p.FacePoint(face,point), pl);
    return pp.Elem();
  }
  Vector PointNormal(int face, int point) const
  {
    return -Vector::CrossProduct(pl.u_x, pl.u_y);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return p.FacePoint(face, point);
  }
private:
  Polygons2d &p;
  Plane pl;
};

class Polygons2Faces : public FaceCollection
{
public:
  Polygons2Faces(Polygons2d &p, Plane p1, Plane p2) : p(p), p1(p1), p2(p2) { }
  int NumFaces() const 
  {
    int s = p.NumFaces();

    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += p.NumPoints(i);
      }
    return count;
  }
  int NumPoints(int face) const
  {
    return 4;
  }
  int FaceToFace(int face) const
  {
    int s = p.NumFaces();

    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += p.NumPoints(i);
	if (face<count) return i;
      }
    return count;
  }
  int FaceToRemainder(int face) const
  {
    int s = p.NumFaces();

    int count = 0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	count += p.NumPoints(i);
	if (face<count) return face-oldcount;
      }
    return count;
  }

  Point FacePoint(int face, int point) const
  {
    int f = FaceToFace(face);
    int px = FaceToRemainder(face);

    if (point==1 || point==2) f++;
    Plane pl;
    if (point==0 || point==1) pl = p1;
    if (point==2 || point==3) pl = p2;
    Point2d pp = p.FacePoint(f,px);
    PlaneIn3d pp1(pp, pl);
    return pp1.Elem();
  }
  Vector PointNormal(int face, int point) const
  {
    Point p = FacePoint(face,0);
    Point px = FacePoint(face, 1);
    Point py = FacePoint(face, 2);
    Vector vx = px-p;
    Vector vy = py-p;
    return -Vector::CrossProduct(vx, vy);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return p.FacePoint(face, point);
  }

private:
  Polygons2d &p;
  Plane p1;
  Plane p2;
};

class SinglePolygon2d : public Polygons2d
{
public:
  SinglePolygon2d(PointCollection2d &p) : p(p) { }
  int NumFaces() const { return 1; }
  int NumPoints(int) const { return p.Size(); }
  Point2d FacePoint(int face, int point) const
  {
    return p.Index(point);
  }
private:
  PointCollection2d &p;
};

class ArrayPolygons2d : public Polygons2d
{
public:
  ArrayPolygons2d(Array<int, Polygons2d*> &arr) : arr(arr) { }
  int NumFaces() const
  {
    int s = arr.Size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += arr.Index(i)->NumFaces();
      }
    return count;
  }
  int NumPoints(int face) const
  {
    return arr.Index(FaceToIndex(face))->NumPoints(FaceToRemainder(face));
  }
  Point2d FacePoint(int face, int point) const
  {
    return arr.Index(FaceToIndex(face))->FacePoint(FaceToRemainder(face), point);
  }
  int FaceToIndex(int face) const
  {
    int s = arr.Size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += arr.Index(i)->NumFaces();
	if (face<count) return i;
      }
    return -1;
  }
  int FaceToRemainder(int face) const
  {
    int s = arr.Size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	count += arr.Index(i)->NumFaces();
	if (face<count) return face-oldcount;
      }
    return -1;    
  }

private:
  Array<int, Polygons2d*> &arr;
};


class BoolPlane
{
public:
  virtual bool Inside(Point2d p) const=0;
};

class FuncBoolPlane : public BoolPlane
{
public:
  FuncBoolPlane(const BoolPlane &p1, 
	       const BoolPlane &p2, 
	       bool (*fptr)(bool, bool)) 
    : p1(p1), p2(p2), fptr(fptr) { }
  bool Inside(Point2d p) const 
  {
    bool b1 = p1.Inside(p);
    bool b2 = p2.Inside(p);
    return fptr(b1,b2);
  }
private:
  const BoolPlane &p1;
  const BoolPlane &p2;
  bool (*fptr)(bool, bool);
};


class BoolRectangle : public BoolPlane, public PointCollection2d
{
public:
  BoolRectangle(Point2d tl, Point2d br) : tl(tl), br(br) { }
  int Size() const
  {
    return 2;
  }
  Point2d Index(int i) const
  {
    switch(i)
      {
      case 0: return tl;
      case 1: return br;
      };
    Point2d p;
    return p;
  }
  bool Inside(Point2d p) const
  {
    if (p.x < tl.x) return false;
    if (p.x > br.x) return false;
    if (p.y < tl.y) return false;
    if (p.y > br.y) return false;
    return true;
  }
private:
  Point2d tl;
  Point2d br;
};

class RectangleGrid
{
public:
  virtual int SizeX() const=0;
  virtual int SizeY() const=0;
  virtual BoolRectangle Index(int x, int y) const=0;
};

class RectangleGridFromGrid : public RectangleGrid
{
public:
  RectangleGridFromGrid(GridIn2d &g) : g(g) { }
  int SizeX() const { return g.XSize()-1; }
  int SizeY() const { return g.YSize()-1; }
  BoolRectangle Index(int x, int y) const
  {
    Point2d tl = g.Index(x, y);
    Point2d br = g.Index(x+1, y+1);
    BoolRectangle b(tl, br);
    return b;
  }
private:
  GridIn2d &g;
};

class RectangleCollection : public Array<int, BoolRectangle>
{
};

class RectangleCollectionBoolPlane : public BoolPlane
{
public:
  RectangleCollectionBoolPlane(const RectangleCollection &coll) : coll(coll) { }
  bool Inside(Point2d p) const
  {
    int size = coll.Size();
    bool bb = false;
    for(int i=0;i<size;i++)
      {
	BoolRectangle r = coll.Index(i);
	bool b = r.Inside(p);
	bb = bb || b;
      }
    return bb;
  }
private:
  const RectangleCollection &coll;
};

class PointsFromRectangleCollection : public PointCollection2d
{
public:
  PointsFromRectangleCollection(const RectangleCollection &c) : c(c) { }
  int Size() const { return c.Size()*2; }
  Point2d Index(int i) const
  {
    int x = i & 1;
    int y = i >> 1;
    BoolRectangle r = c.Index(y);
    return r.Index(x);
  }
private:
 const RectangleCollection &c;
};

class RectCollectionFromRectGrid : public RectangleCollection
{
public:
  RectCollectionFromRectGrid(const RectangleGrid &g) : g(g) { }
  int Size() const { return g.SizeX()*g.SizeY(); }
  BoolRectangle Index(int a) const
  {
    int size = g.SizeX();
    int x = a / size;
    int y = a - x*size;
    return g.Index(x,y);
  }
  
private:
  const RectangleGrid &g;
};

class RectCenterPoint : public Element<Point2d>
{
public:
  RectCenterPoint(const BoolRectangle &r) : r(r) { }
  Point2d Elem() const
  {
    Point2d p;
    Point2d tl = r.Index(0);
    Point2d br = r.Index(1);
    p.x = (tl.x + br.x)/2;
    p.y = (tl.y + br.y)/2;
    return p;
  }
private:
  const BoolRectangle &r;
};

class CompareX
{
public:
  CompareX(const PointCollection2d &pc1) : pc1(pc1) { }

  bool operator()(int p1_, int p2_) const
  {
    Point2d p1 = pc1.Index(p1_);
    Point2d p2 = pc1.Index(p2_);
    return p1.x<p2.x;
  }
private:
  const PointCollection2d &pc1;
};
class CompareY
{
public:
  CompareY(const PointCollection2d &pc1) : pc1(pc1) { }

  bool operator()(int p1_, int p2_) const
  {
    Point2d p1 = pc1.Index(p1_);
    Point2d p2 = pc1.Index(p2_);
    return p1.y<p2.y;
  }
private:
  const PointCollection2d &pc1;
};

class SplitRect : public GridIn2d
{
public:
  SplitRect(const PointCollection2d &p) : p(p)
  {
    int size = p.Size();
    for(int i=0;i<size;i++)
      {
	x.push_back(i);
	y.push_back(i);
      }
    std::sort(x.begin(), x.end(), CompareX(p));
    std::sort(y.begin(), y.end(), CompareY(p));
  }
  void Refresh()
  {
    std::sort(x.begin(), x.end(), CompareX(p));
    std::sort(y.begin(), y.end(), CompareY(p));
  }
  int XSize() const { return p.Size(); }
  int YSize() const { return p.Size(); }
  Point2d Index(int xx, int yy) const
  {
    Point2d pp;
    pp.x = p.Index(x[xx]).x;
    pp.y = p.Index(y[yy]).y;
    return pp;
  }
private:
  const PointCollection2d &p;
  std::vector<int> x;
  std::vector<int> y;
};

class RectCondition : public Function<BoolRectangle, bool>
{
public:
  RectCondition(const BoolPlane &bp) : bp(bp) { }
  bool Index(BoolRectangle r) const
  {
    RectCenterPoint cp(r);
    Point2d p = cp.Elem();
    bool b = bp.Inside(p);
    return b;
  }
private:
  const BoolPlane &bp;
};

inline bool AndFuncOp(bool b, bool b2) { return b && b2; }
inline bool OrFuncOp(bool b, bool b2) { return b || b2; }
inline bool AndNotFuncOp(bool b, bool b2) { return b && !b2; }


class BooleanOp : public RectangleCollection
{
  BooleanOp(const RectangleCollection &r,
	    const RectangleCollection &r2,
	    bool (*op)(bool b, bool b2)) 
    : points1(r), points2(r2), 
      pointscomb(points1, points2),
      conv(pointscomb),
      sr(conv),
      rects(sr),
      coll(rects),
      filter(coll, func),
      func(boolplane),
      boolplane(bp1, bp2, op),
      bp1(r),
      bp2(r2)
  { }
  int Size() const { return coll.Size(); }
  BoolRectangle Index(int i) const
  {
    return coll.Index(i);
  }

private:
  PointsFromRectangleCollection points1;
  PointsFromRectangleCollection points2;
  CombineArrays<Point2d> pointscomb;
  PointCollection2dConvert conv;
  SplitRect sr; // PointCollection2d => GridIn2d
  RectangleGridFromGrid rects;
  RectCollectionFromRectGrid coll;
  FilterArray<BoolRectangle> filter;
  RectCondition func;
  FuncBoolPlane boolplane;
  RectangleCollectionBoolPlane bp1;
  RectangleCollectionBoolPlane bp2;
};

#endif
