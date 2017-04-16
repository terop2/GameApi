
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


class FaceCollection
{
public:
  virtual ~FaceCollection() { }
  virtual void Prepare()=0;
  virtual int NumFaces() const = 0;
  virtual int NumPoints(int face) const=0;
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual float Attrib(int face, int point, int id) const=0;
  virtual int AttribI(int face, int point, int id) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual float TexCoord3(int face, int point) const { return 0.0; }

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

class LineCollection
{
public:
  virtual ~LineCollection() { }
  virtual int NumLines() const =0;
  virtual Point LinePoint(int line, int point) const = 0;
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

  virtual Point EndLinePoint(int line, int point) const { return LinePoint(line,point); }
  virtual unsigned int EndLineColor(int line, int point) const { return LineColor(line,point); }
};
