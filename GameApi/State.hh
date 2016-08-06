#ifndef STATE_HH
#define STATE_HH
#include "Buffer.hh"
#include "Graph.hh"
#include "Effect.hh"



class State
{
public:
  void Alloc(int count);
  void SetTexture(int statenum, BufferRef buffer);
public:
  void SwitchToState(int statenum);
};

#if 0
class MeshStates : public Mesh
{
public:
  MeshStates(Mesh &m, int state) : m(m), state(state) 
  {
    numstates = m.NumStates();
    points = new Point*[numstates];
    for(int i=0;i<numstates;i++)
      {
	points[i] = new Point[];
      }
  }
  
  virtual int NumFrames() const=0;
  virtual int NumFaces(int framenum) const=0;
  virtual int NumPoints() const=0;
  virtual Point FacePoint(int framenum, int face, int point) const=0;

  int NumPoints(int framenum, int state) const
  {
    int numpoints = m.NumPoints();
    int fr = m.NumFrames();
    int count = 0;
    for(int f=0;f<fr;f++)
      {
	int faces = m.NumFaces(f);
	for(int fa=0;fa<faces;fa++)
	  {
	    count += numpoints;
	  }
      }
  }
private:
  Mesh &m;
  int state;  
  Point **points;
  int numstates;
};
#endif

template<class T, class CX, class CY>
class ColumnBitmap : public Bitmap<T>
{
public:
  virtual CX Column(int x) const=0;
  virtual CY Row(int y) const=0;
};

template<class C, class CX, class CY>
class DynColumnBitmap : public ColumnBitmap<C,CX,CY>
{
public:
  DynColumnBitmap(int sx, int sy) : sx(sx), sy(sy) 
  {
    cx_array = new CX[sx];
    cy_array = new CY[sy];
    c_array = new C[sx*sy];
  }
  ~DynColumnBitmap() { delete [] cx_array; delete [] cy_array; delete [] c_array; }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual C Map(int x, int y) const 
  {
    return c_array[x+y*sx];
  }
  CX Column(int x) const
  {
    return cx_array[x];
  }
  CY Row(int y) const
  {
    return cy_array[y];
  }

  C &DynMap(int x, int y) const 
  {
    //std::cout << "DynMap " << x << " " << y << std::endl;
    return c_array[x+y*sx];
  }
  CX &DynColumn(int x) const
  {
    return cx_array[x];
  }
  CY &DynRow(int y) const
  {
    return cy_array[y];
  }

private:
  int sx,sy;
  CX *cx_array;
  CY *cy_array;
  C *c_array;
};
struct StateColumn
{
  StateColumn() : framenum(0), numstates(0), m(Matrix::Identity()) { }
  int framenum; 
  int numstates;
  Matrix m;
};
struct Texture
{
  BufferRef buffer;
};

struct StateRow
{
  StateRow() : statenum(0), numframes(0), texture(0), rend(0) { }
  int statenum;
  int numframes;
  Texture *texture;
  ArrayRender *rend;
};

class FaceInfo
{
public:
  virtual int NumFaces() const=0;
  virtual int NumPoints() const=0;
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual unsigned int VertexColor(int face, int point) const=0;

  virtual int Pos() const =0;
  virtual int Size() const=0;
};

class EmptyFaceInfo : public FaceInfo
{
public:
  virtual int NumFaces() const { return 0; }
  virtual int NumPoints() const { return 0; }
  virtual Point FacePoint(int face, int point) const { return Point(0.0,0.0,0.0); }
  virtual Vector PointNormal(int face, int point) const { return Vector(0.0,0.0,0.0); }
  virtual Point2d TexCoord(int face, int point) const { Point2d p; p.x = 0.0;  return p; }
  virtual unsigned int VertexColor(int face, int point) const
  {
    return 0;
  }

  virtual int Pos() const { return 0; }
  virtual int Size() const { return 0; }
};


class FaceInfoMesh : public Mesh
{
public:
  FaceInfoMesh(FaceInfo &fi) : fi(fi) { }
  virtual int NumFrames() const { return 1; }
  virtual int NumFaces(int framenum) const { return fi.NumFaces(); }
  virtual int NumPoints() const { return fi.NumPoints(); }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const { return fi.FacePoint(face, point); }

  virtual int NumStates() const { return 0; }
  virtual int StateNum(int frame, int face) const { return 0; }

private:
  FaceInfo &fi;
};


class FaceInfoMeshNormals : public MeshNormals
{
public:
  FaceInfoMeshNormals(FaceInfo &fi) : fi(fi) { }
  virtual int NumFrames() const { return 1; }
  virtual Vector PointNormal(int framenum, int statenum, int face, int point) const 
  {
    return fi.PointNormal(face,point);
  }
private:
  FaceInfo &fi;
};

class FaceInfoMeshTexCoords : public MeshTexCoords
{
public:
  FaceInfoMeshTexCoords(FaceInfo &fi) : fi(fi) { }
  virtual int NumFrames() const { return 1; }
  Point2d TexCoord(int framenum, int statenum, int face, int point) const
  {
    return fi.TexCoord(face,point);
  }
private:
  FaceInfo &fi;
};

class FaceInfoMeshColors : public MeshColors
{
public:
  FaceInfoMeshColors(FaceInfo &fi) : fi(fi) { }
  virtual int NumFrames() const { return 1; }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point) const { return fi.VertexColor(face, point); }

private:
  FaceInfo &fi;
};




class FaceInfoImpl : public FaceInfo
{
public:
  FaceInfoImpl(FaceInfo *next) : next(next) { }
  virtual int NumFaces() const { return next->NumFaces(); }
  virtual int NumPoints() const { return next->NumPoints(); }
  virtual Point FacePoint(int face, int point) const { return next->FacePoint(face, point); }
  virtual Vector PointNormal(int face, int point) const { return next->PointNormal(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return next->TexCoord(face,point); }
  virtual unsigned int VertexColor(int face, int point) const
  {
    return next->VertexColor(face,point);
  }
private:
  FaceInfo *next;
};

class FaceInfoCalc
{
public:
  FaceInfoCalc(Mesh &m, int framenum, int statenum) : m(&m), framenum(framenum), statenum(statenum) 
  {
    m_currentstate = m.StateNum(framenum, 0);
  }
  FaceInfoCalc(Mesh &m, MeshNormals &n, int framenum, int statenum) : m(&m), normals(&n), framenum(framenum), statenum(statenum) { 
    m_currentstate = m.StateNum(framenum, 0);

  }
  FaceInfoCalc(Mesh &m, MeshTexCoords &c, int framenum, int statenum) : m(&m), coords(&c), framenum(framenum), statenum(statenum) 
  {
    m_currentstate = m.StateNum(framenum, 0); 
  }
  FaceInfoCalc(Mesh &m, MeshColors &c, int framenum, int statenum) : m(&m), color(&c), framenum(framenum), statenum(statenum) 
  {
    m_currentstate = m.StateNum(framenum, 0); 
  }

  mutable int currentpos;
  mutable int currentsize;
  virtual int NumFaces() const
  {
    //int framenum = 0;
    int facenum = m->NumFaces(framenum);
    if (m->NumStates()<2) { return facenum; }
    int currentstate = -1;
    if (facenum) { currentstate = m_currentstate; /* m->StateNum(framenum,0);*/ }
    int count = 0;
    int count2 =0;
    int face = 0;
    if (statenum != 0)
      {
	for(; face < facenum; face++)
	  {
	    int state = m->StateNum(framenum, face);
	    if (state != currentstate)
	      {
		if (count == statenum) break;
		count ++;
		currentstate = state;
		if (count == statenum) break;
	      }
	  }
      } else { if (facenum) currentstate = m_currentstate; /* m->StateNum(framenum,0);*/ }
    currentpos = face;
    //std::cout << "NumFaces currentpos = " << currentpos << std::endl;
    for(;face<facenum;face++)
      {
	int state = m->StateNum(framenum, face);
	count2++;
	if (state != currentstate)
	  {
	    break;
	  }
      }
    currentsize = face - currentpos;
    //std::cout << "NumFaces currentsize = " << currentsize << std::endl;
    return currentsize; // count2    
  }
  virtual Point FacePoint(int face2, int point2) const 
  { 
    if (!m) return Point(0.0,0.0,0.0);
    //int framenum = 0;
    int facenum = m->NumFaces(framenum);
    if (m->NumStates()<2) { return m->FacePoint(framenum, 0, face2, point2); }
    int currentstate = -1;
    if (facenum) { currentstate = m->StateNum(framenum,0); }
    int count = 0;
    int count2 = 0;
    int face = 0;
    if (statenum != 0) {
    for(; face < facenum; face++)
      {
	int state = m->StateNum(framenum, face);
	if (state != currentstate)
	  {
	    currentstate = state;
	    if (count == statenum) break;
	    count ++;
	    if (count == statenum) break;
	  }
      }
    } else { if (facenum) currentstate = m->StateNum(framenum, 0); }
    //std::cout << "FacePoint face1 = " << face << std::endl;
    int oldface = face;
    for(;face<facenum;face++)
      {
	int state = m->StateNum(framenum, face);
	if (face2==count2)
	  {
	    //std::cout << "FacePoint face1 = " << face << std::endl;
	    return m->FacePoint(framenum, state, oldface+count2, point2);
	  }
	count2++;
	if (state != currentstate)
	  {
	    break;
	  }
	if (face2==count2)
	  {
	    return m->FacePoint(framenum, state, oldface+count2, point2);
	  }
      }

    Point p(0.0,0.0,0.0);
    return p;
  }
  virtual Vector PointNormal(int face2, int point2) const
  {
    if (!m) return Vector(0.0,0.0,0.0);
    if (!normals) return Vector(0.0,0.0,0.0);
    //int framenum = 0;
    if (m->NumStates()<2) { return normals->PointNormal(framenum, 0, face2, point2); }
    int facenum = m->NumFaces(framenum);
    int currentstate = -1;
    if (facenum) { currentstate = m->StateNum(framenum,0); }
    int count = 0; 
    int count2 = 0;
    int face = 0;
    if (statenum != 0)
      {
    for(; face < facenum; face++)
      {
	int state = m->StateNum(framenum, face);
	if (state != currentstate)
	  {
	    currentstate = state;
	    if (count == statenum) break;
	    count ++;
	    if (count == statenum) break;
	  }
      }
      }else { if (facenum) currentstate = m->StateNum(framenum, 0); }
    int oldface = face;
    for(;face<facenum;face++)
      {
	int state = m->StateNum(framenum, face);
	if (face2==count2)
	  {
	    return normals->PointNormal(framenum, state, oldface+count2, point2);
	  }
	count2++;
	if (state != currentstate)
	  {
	    break;
	  }
	if (face2==count2)
	  {
	    return normals->PointNormal(framenum, state, oldface+count2, point2);
	  }
      }

    Vector p(0.0,0.0,0.0);
    return p;

  }
  virtual Point2d TexCoord(int face2, int point2) const
  {
    if (!m) { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
    if (!coords) { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
    //int framenum = 0;
    int facenum = m->NumFaces(framenum);
    int currentstate = -1;
    if (m->NumStates()<2) { return coords->TexCoord(framenum, 0, face2, point2); }
    if (facenum) { currentstate = m->StateNum(framenum,0); }
    int count = 0;
     int count2 = 0;
   int face = 0;
   if (statenum != 0) {
    for(; face < facenum; face++)
      {
	int state = m->StateNum(framenum, face);
	if (state != currentstate)
	  {
	    currentstate = state;
	    if (count == statenum) break;
	    count ++;
	    if (count == statenum) break;
	  }
      }
   } else { if (facenum) currentstate = m->StateNum(framenum, 0); }
    int oldface = face;

    for(;face<facenum;face++)
      {
	int state = m->StateNum(framenum, face);
	if (face2==count2)
	  {
	    Point2d p = coords->TexCoord(framenum, state, oldface+count2, point2); 
	    // std::cout << framenum << " " << state << ":" << count2 << " " << point2 << ":" << p.x << " " << p.y << std::endl;
	    return p;
	  }
	count2++;
	if (state != currentstate)
	  {
	    break;
	  }
	if (face2==count2)
	  {
	    Point2d p = coords->TexCoord(framenum, state, oldface+count2, point2);
	    //std::cout << framenum << " " << state << ":" << count2 << " " << point2 << ":" << p.x << " " << p.y << std::endl;
	    return p;
	  }
      }

    Point2d p;
    p.x = 0.0; p.y = 0.0;
    return p;

  }
  virtual unsigned int VertexColor(int face2, int point2) const
  {
    if (!m) { unsigned int p = 0; return p; }
    if (!color) { unsigned int p = 0; return p; }
    //int framenum = 0;
    if (m->NumStates()<2) { return color->VertexColor(framenum, 0, face2, point2); }
    int facenum = m->NumFaces(framenum);
    int currentstate = -1;
    if (facenum) { currentstate = m->StateNum(framenum,0); }
    int count = 0;
    int count2 = 0;
    int face = 0;
    if (statenum != 0) {
    for(; face < facenum; face++)
      {
	int state = m->StateNum(framenum, face);
	if (state != currentstate)
	  {
	    currentstate = state;
	    if (count == statenum) break;
	    count ++;
	    if (count == statenum) break;
	  }
      }
    } else { if (facenum) currentstate = m->StateNum(framenum, 0); }
    int oldface = face;

    for(;face<facenum;face++)
      {
	int state = m->StateNum(framenum, face);
	if (face2==count2)
	  {
	    return color->VertexColor(framenum, state, oldface+count2, point2);
	  }
	count2++;
	if (state != currentstate)
	  {
	    break;
	  }
	if (face2==count2)
	  {
	    return color->VertexColor(framenum, state, oldface+count2, point2);
	  }
      }

    unsigned int p=0;
    return p;
  }

private:
  Mesh *m;
  MeshNormals *normals;
  MeshTexCoords *coords;
  MeshColors *color;
  int framenum, statenum;
  int m_currentstate;
};

class MeshFaceInfo : public FaceInfoImpl
{
public:
  MeshFaceInfo(FaceInfo *next, Mesh &m, int framenum, int statenum) : FaceInfoImpl(next), calc(m, framenum, statenum), m(m) { }
  virtual int Pos() const { calc.NumFaces(); return calc.currentpos; }
  virtual int Size() const { calc.NumFaces(); return calc.currentsize; }

  virtual int NumFaces() const
  {
    return calc.NumFaces();
  }
  virtual int NumPoints() const
  {
    return m.NumPoints();
  }
  virtual Point FacePoint(int face2, int point2) const 
  { 
    return calc.FacePoint(face2, point2);
  }
private:
  FaceInfoCalc calc;
  Mesh &m;
};

class MeshNormalsFaceInfo : public FaceInfoImpl
{
public:
  MeshNormalsFaceInfo(FaceInfo *next, Mesh &m, MeshNormals &n, int framenum, int statenum) : FaceInfoImpl(next), calc(m, n, framenum, statenum), m(m) { }
  virtual int Pos() const { calc.NumFaces(); return calc.currentpos; }
  virtual int Size() const { calc.NumFaces(); return calc.currentsize; }
  virtual int NumFaces() const
  {
    return calc.NumFaces();
  }
  virtual int NumPoints() const
  {
    return m.NumPoints();
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return calc.PointNormal(face,point);
  }
  virtual Point FacePoint(int face2, int point2) const 
  { 
    return calc.FacePoint(face2, point2);
  }
private:
  FaceInfoCalc calc;
  Mesh &m;
};

class MeshTexCoordsFaceInfo : public FaceInfoImpl
{
public:
  MeshTexCoordsFaceInfo(FaceInfo *next, Mesh &m, MeshTexCoords &tx, int framenum, int statenum) : FaceInfoImpl(next), calc(m, tx, framenum, statenum), m(m) { }
  virtual int Pos() const { calc.NumFaces(); return calc.currentpos; }
  virtual int Size() const { calc.NumFaces(); return calc.currentsize; }
  virtual int NumFaces() const
  {
    return calc.NumFaces();
  }
  virtual int NumPoints() const
  {
    return m.NumPoints();
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return calc.TexCoord(face,point);
  }
  virtual Point FacePoint(int face2, int point2) const 
  { 
    return calc.FacePoint(face2, point2);
  }
private:
  FaceInfoCalc calc;
  Mesh &m;
};


class MeshColorsFaceInfo : public FaceInfoImpl
{
public:
  MeshColorsFaceInfo(FaceInfo *next, Mesh &m, MeshColors &cl, int framenum, int statenum) : FaceInfoImpl(next), calc(m, cl, framenum, statenum), m(m) { }
  virtual int Pos() const { calc.NumFaces(); return calc.currentpos; }
  virtual int Size() const { calc.NumFaces(); return calc.currentsize; }
  virtual int NumFaces() const
  {
    return calc.NumFaces();
  }
  virtual int NumPoints() const
  {
    return m.NumPoints();
  }
  virtual unsigned int VertexColor(int face, int point) const
  {
    return calc.VertexColor(face,point);
  }
  virtual Point FacePoint(int face2, int point2) const 
  { 
    return calc.FacePoint(face2, point2);
  }
private:
  FaceInfoCalc calc;
  Mesh &m;
};



struct StateInfo
{
  FaceInfo *info;
};
// indexed by (sx=framenum, sy=statenum)
template<class C, class CX, class CY>
class StateBitmap : public DynColumnBitmap<C,CX,CY>
{
public:
  StateBitmap(int sx, int sy) : DynColumnBitmap<C, CX, CY>(sx,sy) { }
  void SetColumn(int framenum, int numstates)
  {
    DynColumnBitmap<C,CX,CY>::DynColumn(framenum).framenum = framenum;
    DynColumnBitmap<C,CX,CY>::DynColumn(framenum).numstates = numstates;
  }
  void SetRow(int statenum, int numframes)
  {
    DynColumnBitmap<C,CX,CY>::DynRow(statenum).statenum = statenum;
    DynColumnBitmap<C,CX,CY>::DynRow(statenum).numframes = numframes;
  }
  void SetGrid(int framenum, int statenum, FaceInfo *info)
  {
    DynColumnBitmap<C,CX,CY>::DynMap(framenum,statenum).info = info;
  }  
};

class MeshStateBitmap : public StateBitmap<StateInfo, StateColumn, StateRow>
{
public:
  MeshStateBitmap(Mesh &m) : StateBitmap<StateInfo, StateColumn, StateRow>(CountX(m), CountY(m)), m(m) { }
  void Prepare() { }

  void Copy()
  {
    int frames = m.NumFrames();
    int states = CountY(m);
    for(int x=0;x<frames;x++)
      {
	for(int y=0;y<states;y++)
	  {
	    SetGrid(x,y, new MeshFaceInfo(new EmptyFaceInfo, m, x,y));
	  }
      }
  }
  int CountX(Mesh &m)
  {
    int count = m.NumFrames();
    return count;
  }
  int CountY(Mesh &m)
  {
    int framenum = 0;
    int facenum = m.NumFaces(framenum);
    int currentstate = -1;
    int count = 0;
    std::cout << "CountY: faces=" << facenum << std::endl;
    for(int face = 0; face < facenum; face++)
      {
	int state = m.StateNum(framenum, face);
	if (state != currentstate)
	  {
	    count ++;
	    currentstate = state;
	  }
      }
    return count;
  }

private:
  std::map<int,int> counts;
  Mesh &m;
};


class MeshNormalsStateBitmap : public StateBitmap<StateInfo, StateColumn, StateRow>
{
public:
  MeshNormalsStateBitmap(Mesh &m, MeshNormals &n) : StateBitmap<StateInfo, StateColumn, StateRow>(CountX(m), CountY(m)), m(m),n(n) { }

  void Prepare() { }
  void Copy()
  {
    int frames = m.NumFrames();
    int states = CountY(m);
    for(int x=0;x<frames;x++)
      {
	for(int y=0;y<states;y++)
	  {
	    SetGrid(x,y, new MeshNormalsFaceInfo(new EmptyFaceInfo, m, n, x,y));
	  }
      }
  }
  int CountX(Mesh &m)
  {
    int count = m.NumFrames();
    return count;
  }
  int CountY(Mesh &m)
  {
    int framenum = 0;
    int facenum = m.NumFaces(framenum);
    int currentstate = -1;
    int count = 0;
    for(int face = 0; face < facenum; face++)
      {
	int state = m.StateNum(framenum, face);
	if (state != currentstate)
	  {
	    count ++;
	    currentstate = state;
	  }
      }
    return count;
  }

private:
  //std::map<int,int> counts;
  Mesh &m;
  MeshNormals &n;
};


class MeshColorsStateBitmap : public StateBitmap<StateInfo, StateColumn, StateRow>
{
public:
  MeshColorsStateBitmap(Mesh &m, MeshColors &n) : StateBitmap<StateInfo, StateColumn, StateRow>(CountX(m), CountY(m)), m(m),n(n) { }

  void Prepare() { }
  void Copy()
  {
    int frames = m.NumFrames();
    int states = CountY(m);
    for(int x=0;x<frames;x++)
      {
	for(int y=0;y<states;y++)
	  {
	    SetGrid(x,y, new MeshColorsFaceInfo(new EmptyFaceInfo,m, n, x,y));
	  }
      }
  }
  int CountX(Mesh &m)
  {
    int count = m.NumFrames();
    return count;
  }
  int CountY(Mesh &m)
  {
    int framenum = 0;
    int facenum = m.NumFaces(framenum);
    int currentstate = -1;
    int count = 0;
    for(int face = 0; face < facenum; face++)
      {
	int state = m.StateNum(framenum, face);
	if (state != currentstate)
	  {
	    count ++;
	    currentstate = state;
	  }
      }
    return count;
  }

private:
  //std::map<int,int> counts;
  Mesh &m;
  MeshColors &n;
};

class MeshTexCoordStateBitmap : public StateBitmap<StateInfo, StateColumn, StateRow>
{
public:
  MeshTexCoordStateBitmap(Mesh &m, MeshTexCoords &n) : StateBitmap<StateInfo, StateColumn, StateRow>(CountX(m), CountY(m)), m(m),n(n) { }
  void Prepare() { }

  void Copy()
  {
    int frames = m.NumFrames();
    int states = CountY(m);
    for(int x=0;x<frames;x++)
      {
	for(int y=0;y<states;y++)
	  {
	    SetGrid(x,y, new MeshTexCoordsFaceInfo(new EmptyFaceInfo,m, n, x,y));
	  }
      }
  }
  int CountX(Mesh &m)
  {
    int count = m.NumFrames();
    return count;
  }
  int CountY(Mesh &m)
  {
    int framenum = 0;
    int facenum = m.NumFaces(framenum);
    int currentstate = -1;
    int count = 0;
    for(int face = 0; face < facenum; face++)
      {
	int state = m.StateNum(framenum, face);
	if (state != currentstate)
	  {
	    count ++;
	    currentstate = state;
	  }
      }
    return count;
  }

private:
  //std::map<int,int> counts;
  Mesh &m;
  MeshTexCoords &n;
};


class FrameArrayMesh : public Mesh
{
public:
  FrameArrayMesh(Mesh **array, int size) : array(array), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual int NumFaces(int framenum) const 
  {
    return array[framenum]->NumFaces(0);
    //int s = size;
    //int count = 0;
    //for(int i=0;i<s;i++)
    //  {
    //	count += array[i]->NumFaces(framenum);
    //  }
    //return count;
  }
  virtual int NumPoints() const 
  {
    return array[0]->NumPoints();
  }
  virtual Point FacePoint(int framenum, int statenum, int face, int point) const  {
    return array[framenum]->FacePoint(0, statenum, face, point);
  }

  virtual int NumStates() const { return 0; }
  virtual int StateNum(int framenum, int face) const { return 0; }

private:
  Mesh **array;
  int size;
};


class FrameArrayMeshNormals : public MeshNormals
{
public:
  FrameArrayMeshNormals(MeshNormals **normals, int size) : normals(normals), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual Vector PointNormal(int framenum, int statenum, int face, int point) const 
  {
    //std::cout << framenum << " " << size << std::endl;
    //std::cout << normals << std::endl;
    //std::cout << normals[framenum] << std::endl;
    return normals[framenum]->PointNormal(0, statenum, face, point);
  }
private:
  MeshNormals **normals;
  int size;
};

class FrameArrayMeshColors : public MeshColors
{
public:
  FrameArrayMeshColors(MeshColors **colors, int size) : colors(colors), size(size) { }
  virtual int NumFrames() const { return size; }
  virtual unsigned int VertexColor(int framenum, int statenum, int face, int point) const 
  { 
    return colors[framenum]->VertexColor(0, statenum, face, point);
  }
private:
  MeshColors **colors;
  int size;
};

class FrameArrayMeshTexCoords : public MeshTexCoords
{
public:
  FrameArrayMeshTexCoords(MeshTexCoords **coords, int size) : coords(coords), size(size) { }
  virtual int NumFrames() const { return size; }
  Point2d TexCoord(int framenum, int statenum, int face, int point) const
  {
    return coords[framenum]->TexCoord(0, statenum, face, point);
  }
private:
  MeshTexCoords **coords;
  int size;
};

struct StateBitmaps
{
  StateBitmaps(Mesh *m, MeshNormals *n, MeshTexCoords *coords, MeshColors *c, MeshTextures *tex, Function<int,int> &texfaces)
    : tex(tex), texfaces(texfaces)
  {
    if (m) { bitmap = new MeshStateBitmap(*m); }
    if (n) { normals = new MeshNormalsStateBitmap(*m, *n); }
    if (coords) { texcoord = new MeshTexCoordStateBitmap(*m, *coords); }
    if (c) { colors = new MeshColorsStateBitmap(*m, *c); }
  }
  ~StateBitmaps() { delete bitmap; delete normals; delete colors; delete texcoord; delete tex; }
  void SetMatrix(int framenum, Matrix m)
  {
    StateColumn &column = bitmap->DynColumn(framenum);
    column.m = m;    
  }
  void SetTexture(int face, BufferRef ref)
  {
    int sy = bitmap->SizeY();
    for(int y=0;y<sy;y++)
      {
	StateInfo &info = bitmap->DynMap(0, y);
	int pos = info.info->Pos();
	int size = info.info->Size();
	//std::cout << "SetTexturepos: " << pos << " " << size << ":" << face << std::endl;
	if (face >= pos && face < pos+size)
	  {
	    //std::cout << "SetTexture" << y << std::endl;
	    StateRow &row = texcoord->DynRow(y);
	    row.texture = new Texture;
	    row.texture->buffer = ref;
	    break;
	  }
      }
  }
  void Prepare()
  {
    bitmap->Copy();
    normals->Copy();
    colors->Copy();
    texcoord->Copy();
    if (tex)
      {
	int s = tex->NumTextures();
	//std::cout << "NumTextures: " << s << std::endl;
	for(int i=0;i<s;i++)
	  {
	    tex->GenTexture(i);
	    BufferRef r = tex->TextureBuf(i);
	    int face = texfaces.Index(i);
	    //std::cout << "Setting texture: " << face << " " << r.width << " " << r.height << std::endl;
	    SetTexture(face, r);
	  }
      }
    int sx = bitmap->SizeX();
    int sy = bitmap->SizeY();
    //std::cout << sx << " " << sy << std::endl;
    for(int y=0;y<sy;y++) // statenum
      {
	//std::cout << "State:" << y << std::endl;
	StateRow &row = bitmap->DynRow(y);
	row.rend = new ArrayRender;
	if (row.texture)
	  row.rend->AllocTexture(1);
	Mesh **meshes = new Mesh*[sx];
	MeshNormals **normals2 = new MeshNormals*[sx];
	MeshTexCoords **texcoords = new MeshTexCoords*[sx];
	MeshColors **colors2 = new MeshColors*[sx];
	for(int x=0;x<sx;x++) // framenum
	  {
	    StateInfo &s = bitmap->DynMap(x,y);
	    StateInfo &sn = normals->DynMap(x,y);
	    StateInfo &st = texcoord->DynMap(x,y);
	    StateInfo &sc = colors->DynMap(x,y);

	    //std::cout << "Info:" << sn.info << std::endl;
	    
	    //sn.rend = 0;
	    //sc.rend = 0;
	    //st.rend = 0;
	    
	    meshes[x] = new FaceInfoMesh(*s.info);
	    normals2[x] = new FaceInfoMeshNormals(*sn.info);
	    texcoords[x] = new FaceInfoMeshTexCoords(*st.info);
	    colors2[x] = new FaceInfoMeshColors(*sc.info);
	  }
	FrameArrayMesh mesh(meshes, sx);
	FrameArrayMeshNormals normal(normals2, sx);
	FrameArrayMeshTexCoords coord(texcoords, sx);
	FrameArrayMeshColors color(colors2, sx);

	MeshToTriangles trimesh(mesh);
	MeshNormalsToTriangles trinormal(normal, mesh.NumPoints()==4);
	MeshTexCoordsToTriangles tricoord(coord, mesh.NumPoints()==4);
	MeshColorsToTriangles tricolor(color, mesh.NumPoints()==4);
	row.rend->Alloc(trimesh.NumFaces(0), trimesh.NumFaces(0)*trimesh.NumPoints(), mesh.NumFrames(),normal.NumFrames(),color.NumFrames(),coord.NumFrames());
	row.rend->InsertAll(trimesh, trinormal, tricoord, tricolor);
	for(int i=0;i<sx;i++)
	  {
	    delete meshes[i];
	    delete normals2[i];
	    delete texcoords[i];
	    delete colors2[i];
	  }
	delete [] meshes;
	delete [] normals2;
	delete [] texcoords;
	delete [] colors2;
      }
    int texcount = sy;
    if (texcount && tex->NumTextures())
      {
	
	MeshTextures *texture = 0;
	for(int y=sy-1;y>=0;y--)
	  {
	    StateRow &r = texcoord->DynRow(y);	  
	    StateRow &rr = bitmap->DynRow(y);
	    rr.rend->AllocTexture(1);
	    BufferRef buf = r.texture->buffer;
	    BitmapFromBuffer *bitmap = new BitmapFromBuffer(buf);
	    //if (texture)
	    //  {
	    //	texture = new MeshTexturesImpl(*bitmap, *texture);
	    //  }
	    //else
	    //  {
	    texture = new MeshTexturesImpl(*bitmap);
		//  }
	    rr.rend->UpdateTexture(*texture, 0);
	    delete texture;
	  }
      }

  }
  void Render(int framenum);
  ArrayRender *GetRender(int row) const { 
    StateRow &r = bitmap->DynRow(row);
    return r.rend;
  }
  Matrix GetMatrix(int framenum) const {
    StateColumn &col = bitmap->DynColumn(framenum);
    return col.m;
  }
  
  MeshStateBitmap *bitmap;
  MeshNormalsStateBitmap *normals;
  MeshColorsStateBitmap *colors;
  MeshTexCoordStateBitmap *texcoord;
  MeshTextures *tex;
  Function<int,int> &texfaces;
};



// LINK a->a
// LINK a->(a,a)
// LINK (a,a)->a
// LINK a->_|_
// LINK T->a
class Linkage
{
public:
  virtual void Link(int face1, int face2)=0; // a->a
  virtual void LinkSplit(int face, int face1, int face2)=0; // a->(a,a)
  virtual void LinkCombine(int face1, int face2, int face)=0; // (a,a)->a
  virtual void LinkTerminate(int face1)=0;
  virtual void LinkStart(int face1)=0;
};
class FrameLinkage : public Linkage
{
public:
  FrameLinkage(int frame) : frame(frame) { }
  int FrameNum() const { return frame; }
  
private:
  int frame;
};

class FrameLinkageImpl : public FrameLinkage
{
public:
  FrameLinkageImpl(int frame) : FrameLinkage(frame) { }
  virtual void Link(int face1, int face2) // a->a
  {
    Node n;
    n.type = 1;
    n.input_face1 = face1;
    n.output_face1 = face2;
    CheckInput(face1);
    CheckOutput(face2);
    vec.push_back(n);    
  }
  virtual void LinkSplit(int face, int face1, int face2) // a->(a,a)
  {
    Node n;
    n.type = 2;
    n.input_face1 = face;
    n.output_face1 = face1;
    n.output_face2 = face2;
    CheckInput(face);
    CheckOutput(face1);
    CheckOutput(face2);
    vec.push_back(n);
  }
  virtual void LinkCombine(int face1, int face2, int face) // (a,a)->a
  {
    Node n;
    n.type = 3;
    n.input_face1 = face1;
    n.input_face2 = face2;
    n.output_face1 = face;
    CheckInput(face1);
    CheckInput(face2);
    CheckOutput(face);
    vec.push_back(n);
  }
  virtual void LinkTerminate(int face1) // a->_|_
  {
    Node n;
    n.type = 4;
    n.input_face1 = face1;
    n.output_face1 = -1;
    CheckInput(face1);
    vec.push_back(n);
  }
  virtual void LinkStart(int face1) // T -> a
  {
    Node n;
    n.type = 5;
    n.input_face1 = -1;
    n.output_face1 = face1;
    CheckOutput(face1);
    vec.push_back(n);
  }
  struct Node
  {
    Node() 
      : type(-1), input_face1(-1), input_face2(-1), output_face1(-1), output_face2(-1) { }
    Node(const Node &n) : type(n.type), input_face1(n.input_face1),
			  input_face2(n.input_face2),
			  output_face1(n.output_face1),
			  output_face2(n.output_face2) { }
    int type;
    int input_face1;
    int input_face2;
    int output_face1;
    int output_face2;
  };
  std::vector<Node> vec;

  int CalcDelta() const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	const Node &n = vec[i];
	switch(n.type)
	  {
	  case 1: count+=0; break;  // a->a
	  case 2: count+=1; break; // a->(a,a)
	  case 3: count+=0; break; // (a,a)->a
	  case 4: count+=0; break; // a->_|_
	  case 5: count+=1; break; // T->a
	  };
      }
    return count;
  }
  void NextUsedOnes(std::vector<int> &vec2)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	const Node &n = vec[i];
	if (n.input_face1 != -1)
	  vec2[n.input_face1] = 0;

	if (n.input_face2 != -1)
	  vec2[n.input_face2] = 0;

	if (n.output_face1 != -1)
	  vec2[n.output_face1] = 1;

	if (n.output_face2 != -1)
	  vec2[n.output_face2] = 1;
      }
  } 
  
  void CheckInput(int input)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Node &n = vec[i];
	if (n.input_face1 == input) std::cout << "ERROR, INPUT DUPLICATE" << std::endl;
	if (n.input_face2 == input) std::cout << "ERROR, INPUT DUPLICATE" << std::endl;
      }
  }

  void CheckOutput(int output)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Node &n = vec[i];
	if (n.output_face1 == output) std::cout << "ERROR, OUTPUT DUPLICATE" << std::endl;
	if (n.output_face2 == output) std::cout << "ERROR, OUTPUT DUPLICATE" << std::endl;
      }
  }

};

class FrameLinkageArray
{
public:
  std::vector<FrameLinkage*> vec; // indexed by framenum
};
#endif
