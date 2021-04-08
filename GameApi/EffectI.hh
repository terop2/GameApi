
#include "VectorTools.hh"



template<class A, class B>
class Function 
{
public:
  typedef A Domain;
  typedef B CoDomain;

  virtual ~Function() { }
  virtual B Index(A a) const=0;
};

template<class I, class T>
class Array : public Function<I,T>
{
public:
  virtual I Size() const=0;
};

template<class T>
class Curve : public Array<float, T>
{
public:
  //float Length() const { return static_cast<float>(Size()); }
};





class CurveIn2d : public Curve<Point2d>
{
public:
  virtual ~CurveIn2d() { }
  //virtual Point2d Index(float pos) const=0;
  //virtual float Length() const=0;
};

class CurveIn3d : public Curve<Point>
{
public:
  virtual ~CurveIn3d() { }
  //virtual Point Index(float pos) const=0;
  //virtual float Length() const=0;
};


class FaceCollection : public CollectInterface
{
public:
  virtual ~FaceCollection() { }
  virtual void Collect(CollectVisitor &vis)=0;
  virtual void HeavyPrepare()=0;
  virtual void Prepare()=0;
  virtual int NumFaces() const = 0;
  virtual int NumPoints(int face) const=0;

  virtual bool has_normal() const { return true; }
  virtual bool has_attrib() const { return false; }
  virtual bool has_color() const { return true; }
  virtual bool has_texcoord() const { return true; }
  virtual bool has_skeleton() const { return false; }
  
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual float Attrib(int face, int point, int id) const=0;
  virtual int AttribI(int face, int point, int id) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  
  virtual int NumObjects() const {
    //std::cout << "Warning: FaceCollection::NumObjects() called" << std::endl;
    return 1; }
  virtual std::pair<int,int> GetObject(int o) const {
    return std::make_pair(0,NumFaces()); }
  
  virtual Point EndFacePoint(int face, int point) const { return FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return TexCoord(face,point); }
  virtual float EndTexCoord3(int face, int point) const { return TexCoord3(face,point); }

  virtual float Duration() const { return 1.0; }

  virtual int NumTextures() const { return 0; }
  virtual void GenTexture(int num) { }
  virtual BufferRef TextureBuf(int num) const { BufferRef ref; ref.buffer = 0;  return ref; }
  virtual int FaceTexture(int face) const { return -1; }
  //virtual void Pullback() const=0; // should call P functions only
  //void P(FaceCollection &single);
  //void P(FaceCollection &coll1, FaceCollection &coll2);
  //void P(FaceCollection *coll, int size); // needed in or operation
  //virtual int RangeCount();
  //virtual Range<int> RangeArray(int count);
#if 0
  struct Triplet
  {
    P poly_id;
    int face;
    int point;
  };
  virtual Triplet FaceTriplet(int face, int point) const=0;
#endif
#if 0
  virtual Coords Coordinates() const=0;
#endif
};


class Attach : public CollectInterface
{
public:
  virtual ~Attach() { }
  virtual void Prepare()=0;
  virtual int Attached(int face, int point) const=0;
};


// Face interpolation
Point FaceCollection_FacePoint(FaceCollection *coll, int face, float u, float v);
Vector FaceCollection_Normal(FaceCollection *coll, int face, float u, float v);
Point2d FaceCollection_TexCoord(FaceCollection *coll, int face, float u, float v);
float FaceCollection_TexCoord3(FaceCollection *coll, int face, float u, float v);
unsigned int FaceCollection_Color(FaceCollection *coll, int face, float u, float v);

// face intersection
std::tuple<int,Point,float,float> FaceCollection_IntersectFace(FaceCollection *coll, Point start_pos, Vector ray_dir);

// face area related functions
float FaceCollection_CalculateFaceArea(FaceCollection *coll, int face);
std::tuple<int,float,float> FaceCollection_PickRandomPoint_areasensitive(FaceCollection *coll);



class LineCollection : public CollectInterface
{
public:
  virtual ~LineCollection() { }
  virtual void Collect(CollectVisitor &vis)=0;
  virtual void HeavyPrepare()=0;
  virtual void Prepare() { }
  virtual int NumLines() const =0;
  virtual Point LinePoint(int line, int point) const = 0;
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

  virtual Point EndLinePoint(int line, int point) const { return LinePoint(line,point); }
  virtual unsigned int EndLineColor(int line, int point) const { return LineColor(line,point); }
};
