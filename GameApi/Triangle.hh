
#include "VectorTools.hh"
#include "Effect.hh"
#include "Plane.hh"
#include "GraphI.hh"
#include "Widgets.hh"

struct TriIndex
{
  int i1;
  int i2;
  int i3;
};
struct FaceIndex
{
  int face;
};

struct Tri
{
  Point p1;
  Point p2;
  Point p3;
  Vector n1;
  Vector n2;
  Vector n3;
  unsigned int c1;
  unsigned int c2;
  unsigned int c3;
  Point2d tex1;
  Point2d tex2;
  Point2d tex3;
};

template<class T>
class ModifyValue
{
public:
  virtual void Modify(T &t) const=0;
  virtual ~ModifyValue() { }
};

class MoveTri : public ModifyValue<Tri>
{
public:
  MoveTri(Vector v) : v(v) { }
  void Modify(Tri &t) const
  {
    t.p1 += v;
    t.p2 += v;
    t.p3 += v;
  }
private:
  Vector v;
};
class ColorTri : public ModifyValue<Tri>
{
public:
  ColorTri(unsigned int c) : c(c) { }
  void Modify(Tri &t) const
  {
    t.c1 = c;
    t.c2 = c;
    t.c3 = c;
  }
private:
  unsigned int c;
};
class NormalTri : public ModifyValue<Tri>
{
public:
  void Modify(Tri &t) const
  {
    Point p1 = t.p1;
    Point p2 = t.p2;
    Point p3 = t.p3;
    Vector n = Vector::CrossProduct(p2-p1, p3-p1);
    t.n1 = n;
    t.n2 = n;
    t.n3 = n;
  }
};
class MoveBox : public ModifyValue<Tri>
{
public:
  MoveBox(Cube3 oldbox, Cube3 newbox) : oldbox(oldbox), newbox(newbox) { }
  void Modify(Tri &t) const
  {
    Point p1 = t.p1;
    Point p2 = t.p2;
    Point p3 = t.p3;
    Vector v = Vector(oldbox.minx, oldbox.miny, oldbox.minz);
    p1 -= v;
    p2 -= v;
    p3 -= v;
    p1.x /= oldbox.maxx-oldbox.minx;
    p1.y /= oldbox.maxy-oldbox.miny;
    p1.z /= oldbox.maxz-oldbox.minz;
    p2.x /= oldbox.maxx-oldbox.minx;
    p2.y /= oldbox.maxy-oldbox.miny;
    p2.z /= oldbox.maxz-oldbox.minz;
    p3.x /= oldbox.maxx-oldbox.minx;
    p3.y /= oldbox.maxy-oldbox.miny;
    p3.z /= oldbox.maxz-oldbox.minz;
    
    p1.x *= newbox.maxx-newbox.minx;
    p1.y *= newbox.maxy-newbox.miny;
    p1.z *= newbox.maxz-newbox.minz;
    p2.x *= newbox.maxx-newbox.minx;
    p2.y *= newbox.maxy-newbox.miny;
    p2.z *= newbox.maxz-newbox.minz;
    p3.x *= newbox.maxx-newbox.minx;
    p3.y *= newbox.maxy-newbox.miny;
    p3.z *= newbox.maxz-newbox.minz;
    
    Vector vv = Vector(newbox.minx, newbox.miny, newbox.minz);
    p1 += vv;
    p2 += vv;
    p3 += vv;
    t.p1 = p1;
    t.p2 = p2;
    t.p3 = p3;
  }
private:
  Cube3 oldbox, newbox;
};

class MoveCoords : public ModifyValue<Tri>
{
public:
  MoveCoords(Coords oldcoords, Coords newcoords) : oldcoords(oldcoords), newcoords(newcoords) { }
  void Modify(Tri &t) const
  {
    Point p1 = t.p1;
    Point p2 = t.p2;
    Point p3 = t.p3;
    p1 -= oldcoords.center;
    p2 -= oldcoords.center;
    p3 -= oldcoords.center;
    float p1x = Vector::DotProduct(p1, oldcoords.u_x);
    p1x /= oldcoords.u_x.Dist();

    float p1y = Vector::DotProduct(p1, oldcoords.u_y);
    p1y /= oldcoords.u_y.Dist();

    float p1z = Vector::DotProduct(p1, oldcoords.u_z);
    p1z /= oldcoords.u_z.Dist();


    float p2x = Vector::DotProduct(p2, oldcoords.u_x);
    p2x /= oldcoords.u_x.Dist();

    float p2y = Vector::DotProduct(p2, oldcoords.u_y);
    p2y /= oldcoords.u_y.Dist();

    float p2z = Vector::DotProduct(p2, oldcoords.u_z);
    p2z /= oldcoords.u_z.Dist();


    float p3x = Vector::DotProduct(p3, oldcoords.u_x);
    p3x /= oldcoords.u_x.Dist();

    float p3y = Vector::DotProduct(p3, oldcoords.u_y);
    p3y /= oldcoords.u_y.Dist();

    float p3z = Vector::DotProduct(p3, oldcoords.u_z);
    p3z /= oldcoords.u_z.Dist();

    //-------------------------------

    Point v1 = newcoords.center + p1x*newcoords.u_x + p1y*newcoords.u_y + p1z*newcoords.u_z;
    Point v2 = newcoords.center + p2x*newcoords.u_x + p2y*newcoords.u_y + p2z*newcoords.u_z;
    Point v3 = newcoords.center + p3x*newcoords.u_x + p3y*newcoords.u_y + p3z*newcoords.u_z;
    t.p1 = v1;
    t.p2 = v2;
    t.p3 = v3;
  }
private:
  Coords oldcoords;
  Coords newcoords;
};

class TriArray : public Array<int, Tri>
{
};

class BoxedTriangles : public TriArray
{
public:
  BoxedTriangles(Array<int, Cube3> &pos, TriArray &obj, Cube3 objpos) : pos(pos), obj(obj), objpos(objpos) { }
  int Size() const { return pos.Size()*obj.Size(); }
  Tri Index(int i) const
  {
    int i1 = i / obj.Size();
    int i2 = i - i1*obj.Size();
    Cube3 c = pos.Index(i1);
    Tri t = obj.Index(i2);
    MoveBox b(objpos, c);
    b.Modify(t);
    return t;
  }  
private:
  Array<int, Cube3> &pos;
  TriArray &obj;
  Cube3 objpos;
};

class ApplyTriArray : public TriArray
{
public:
  ApplyTriArray(const TriArray &arr, ModifyValue<Tri> &mv) : arr(arr), mv(mv) { }
  int Size() const { return arr.Size(); }
  Tri Index(int i) const
  {
    Tri t = arr.Index(i);
    mv.Modify(t);
    return t;
  }
private:
  const TriArray &arr;
  ModifyValue<Tri> &mv;
};

class TriArrayFromFaceCollection : public TriArray
{ // numpoints must be 3
public:
  TriArrayFromFaceCollection(FaceCollection &coll) : coll(coll) { }
  int Size() const
  {
    return coll.NumFaces();
  }
  Tri Index(int i) const
  {
    Tri tr;
    tr.p1 = coll.FacePoint(i, 0);
    tr.p2 = coll.FacePoint(i, 1);
    tr.p3 = coll.FacePoint(i, 2);
    tr.n1 = coll.PointNormal(i, 0);
    tr.n2 = coll.PointNormal(i, 1);
    tr.n3 = coll.PointNormal(i, 2);
    tr.c1 = coll.Color(i, 0);
    tr.c2 = coll.Color(i, 1);
    tr.c3 = coll.Color(i, 2);
    tr.tex1 = coll.TexCoord(i, 0);
    tr.tex2 = coll.TexCoord(i, 1);
    tr.tex3 = coll.TexCoord(i, 2);
    return tr;
  }
private:
  FaceCollection &coll;
};

class FaceCollectionFromTriArray : public SingleForwardFaceCollection
{
public:
  FaceCollectionFromTriArray(const TriArray &arr) : arr(arr) { } 
  virtual std::string name() const { return "FaceCollectionFromTriArray"; }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  int NumFaces() const
  {
    //std::cout << "NUMFACES: " << arr.Size() << std::endl;
    return arr.Size(); // *3 is hack to make this work with UpdateVBO.
  }
  int NumPoints(int face) const
  {
    return 3;
  }
  Point FacePoint(int face, int point) const
  {
    Tri t = arr.Index(face);

    Point p;
    switch(point)
      {
      case 0: p= t.p1; break;
      case 1: p= t.p2; break;
      case 2: p= t.p3; break;
      }
    //std::cout << face << " " << point << " " << p << std::endl;
    return p;
  }
  Vector PointNormal(int face, int point) const
  {
    Tri t = arr.Index(face);
    switch(point)
      {
      case 0: return t.n1;
      case 1: return t.n2;
      case 2: return t.n3;
      }
    return t.n1;    
  }
  float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  unsigned int Color(int face, int point) const
  {
    Tri t = arr.Index(face);
    switch(point)
      {
      case 0: return t.c1;
      case 1: return t.c2;
      case 2: return t.c3;
      }
    return t.c1;    
  }
  Point2d TexCoord(int face, int point) const
  {
    Tri t = arr.Index(face);
    switch(point)
      {
      case 0: return t.tex1;
      case 1: return t.tex2;
      case 2: return t.tex3;
      }
    return t.tex1;
  }
private:
  const TriArray &arr;
};

class TriArrayFromFaceCollectionIndexFace : public TriArray
{
public:
  TriArrayFromFaceCollectionIndexFace(FaceCollection &coll, Array<int, TriIndex> &index, Array<int, FaceIndex> &faces) : coll(coll), index(index), faces(faces) { }
  int Size() const { return index.Size(); }
  Tri Index(int i) const
  {
    TriIndex ia = index.Index(i);
    int i1 = ia.i1;
    int i2 = ia.i2;
    int i3 = ia.i3;
    FaceIndex fa = faces.Index(i);
    int f = fa.face;
    Tri tr;
    tr.p1 = coll.FacePoint(f, i1);
    tr.p2 = coll.FacePoint(f, i2);
    tr.p3 = coll.FacePoint(f, i3);
    tr.n1 = coll.PointNormal(f, i1);
    tr.n2 = coll.PointNormal(f, i2);
    tr.n3 = coll.PointNormal(f, i3);
    tr.c1 = coll.Color(f, i1);
    tr.c2 = coll.Color(f, i2);
    tr.c3 = coll.Color(f, i3);
    tr.tex1 = coll.TexCoord(f, i1);
    tr.tex2 = coll.TexCoord(f, i2);
    tr.tex3 = coll.TexCoord(f, i3);
    return tr;
  }
private:
  FaceCollection &coll;
  Array<int, TriIndex> &index;
  Array<int, FaceIndex> &faces;
};

class TriangulateConvexPolygonsIndex : public Array<int, TriIndex>
{
public:
  TriangulateConvexPolygonsIndex(FaceCollection &pol) : pol(pol) 
  { }
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
	    TriIndex t;
	    t.i1 = line1;
	    t.i2 = line2;
	    t.i3 = third;
	    //std::cout << line1 << " " << line2 << " " << third << std::endl;
	    triangles.push_back(t);
	    line2 = third;
	    third = line2 + 1;
	  }
      }
  }

  int Size() const { return triangles.size(); }
  TriIndex Index(int face) const
  {
    TriIndex t = triangles[face];
    return t;
  }
private:
  FaceCollection &pol;
  std::vector<TriIndex> triangles;
};

class TriangulateConvexPolygonsFaces : public Array<int, FaceIndex>
{
public:
  TriangulateConvexPolygonsFaces(FaceCollection &pol) : pol(pol) { }
  void Gen()
  {
    int facescount = pol.NumFaces();
    for(int i=0;i<facescount;i++)
      {
	//int line1 = 0;
	int line2 = 1;
	
	int third = 2;
	int size = pol.NumPoints(i);
	while(third < size)
	  {
	    faces.push_back(i);
	    line2 = third;
	    third = line2 + 1;
	  }
      }
  }
  int Size() const
  {
    return faces.size();
  }
  FaceIndex Index(int i) const
  {
    FaceIndex fi;
    fi.face = faces[i];
    return fi;
  }

private:
  FaceCollection &pol;
  std::vector<int> faces;
};

class Triangulate : public TriArray
{
public:
  Triangulate(FaceCollection &coll) : faces(coll), index(coll), triarray(coll, index, faces) { }
  void Gen()
  {
    faces.Gen();
    index.Gen();
  }
  int Size() const { return triarray.Size(); }
  Tri Index(int i) const
  {
    return triarray.Index(i);
  }
private:
  TriangulateConvexPolygonsFaces faces;
  TriangulateConvexPolygonsIndex index;
  TriArrayFromFaceCollectionIndexFace triarray;
};


class CoordObject
{
public:
  virtual TriArray *Triangles(Coords pos) const=0;
};

class FaceCollectionCoordObject : public CoordObject
{
public:
  FaceCollectionCoordObject(FaceCollection &coll, CoordSource &source) : source(source), tris(coll), c(0), c2(0)
  {
    tris.Gen();
  }

  TriArray *Triangles(Coords pos) const
  {
    delete c;
    delete c2;
    c = new MoveCoords(source.coord(), pos);
    c2 = new ApplyTriArray(tris, *c);
    return c2;
  }
private:
  CoordSource &source;
  Triangulate tris;
  mutable MoveCoords *c;
  mutable ApplyTriArray *c2;
};

struct Door
{
public:
  Plane plane;
  Point2d topleft;
  Point2d topright;
  Point2d bottomleft;
  Point2d bottomright;
};

class Room
{
public:
  virtual TriArray *Triangles(const Array<int, Door> &doors) const=0;
};

class Shadow : public ModifyValue<Tri>
{
public:
  Shadow(Plane pl, Point p) : pl(pl), p(p) { }
  void Modify(Tri &t) const
  {
    LinePlaneIntersection intersect1 = LinePlaneIntersectionFunc(p, t.p1, pl.u_p, pl.u_p+pl.u_x, pl.u_p+pl.u_y);
    t.p1 = IntersectionPoint(intersect1, p, t.p1);

    LinePlaneIntersection intersect2 = LinePlaneIntersectionFunc(p, t.p2, pl.u_p, pl.u_p+pl.u_x, pl.u_p+pl.u_y);
    t.p2 = IntersectionPoint(intersect2, p, t.p2);

    LinePlaneIntersection intersect3 = LinePlaneIntersectionFunc(p, t.p3, pl.u_p, pl.u_p+pl.u_x, pl.u_p+pl.u_y);
    t.p3 = IntersectionPoint(intersect3, p, t.p3);
  }
private:
  Plane pl;
  Point p;
};
#if 0
#define retp(x,y,z) if(A++==a) return Point(x*scale,y*scale,z*scale);
#define forq(i) for(i=-1;i<=1;i+=2)
class Dodecahedron : public PointCollection
{
public:
  double scale;
  Dodecahedron(double s) : scale(s) {}; 
  virtual int Size() const { return 20; }
  virtual Point Index(int a) const { 
      double phi = (1+sqrt(5))/2;
      int A = 0;
      int i,j,k; // general counters
      forq(i) {
          forq(j) {
              forq(k) {
                  retp(i,j,k);
              }
          }
      };
      // (+-1,+-1,+-1)

      forq(i) {
          forq(j) {
              retp(0,i*(1/phi),j*phi);
          }
      };
      // (0,+-1/phi,+-phi)
      
      forq(i) {
          forq(j) {
              retp(i*(1/phi),j*phi,0);
          }
      };
      // (+-1/phi,+-phi,0)

      forq(i) {
          forq(j) {
              retp(j*phi,0,i*(1/phi));
          }
      };
      // (+-phi,0,+-1/phi)
      return Point(0.0,0.0,0.0);
  }
};

#endif

class TriangulateEffect : public FrameAnim
{
public:
  TriangulateEffect(Render *r) : FrameAnim(r) { }
  void Init();
  bool Frame(float time);
private:
  VBOState vbostate;
};

class DecodahedronEffect : public Widget
{
public:
  DecodahedronEffect(Render *r) : Widget(r) { }
  void Init();
  bool Frame(float time);
  void PreFrame(float fr);
  virtual void HandleKey(int key) { }
  virtual void HandlePointer(int button) { }
  virtual void HandlePointerMove(Point2d p) { pp = p; }

private:
  Point2d pp;
};
