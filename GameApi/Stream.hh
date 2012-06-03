
#include "Graph.hh"
#include "ObjectCreator.hh"

class Stream3d
{
public:
  bool m_poly,m_tex,m_color,m_normal;
  void PolyLine(Point p1, Point p2) { PolyQuad(p1,p1,p2,p2); }
  void TexLine(Point2d t1, Point2d t2) { TexQuad(t1,t1,t2,t2); }
  void ColorLine(Color c1, Color c2) { ColorQuad(c1,c1,c2,c2); }
  void NormalLine(QuadNormal n1, QuadNormal n2) { NormalQuad(n1,n1,n2,n2); }
  void PolyTri(Point p1, Point p2, Point p3) { PolyQuad(p1,p2,p3,p3); }
  void TexTri(Point2d t1, Point2d t2, Point2d t3) { TexQuad(t1,t2,t3,t3); }
  void ColorTri(Color c1, Color c2, Color c3) { ColorQuad(c1,c2,c3,c3); }
  void NormalTri(QuadNormal n1, QuadNormal n2, QuadNormal n3) { NormalQuad(n1,n2,n3,n3); }
  void PolyQuad(Point p1, Point p2, Point p3, Point p4) 
  { 
    tmpquad.p1 = p1;
    tmpquad.p2 = p2;
    tmpquad.p3 = p3;
    tmpquad.p4 = p4;
  }
  void TexQuad(Point2d t1, Point2d t2, Point2d t3, Point2d t4)
  {
    tmptex.t1 = t1;
    tmptex.t2 = t2;
    tmptex.t3 = t3;
    tmptex.t4 = t4;
  }
  void ColorQuad(Color c1, Color c2, Color c3, Color c4)
  {
    tmpcolor.c1 = c1;
    tmpcolor.c2 = c2;
    tmpcolor.c3 = c3;
    tmpcolor.c4 = c4;
  }
  void NormalQuad(QuadNormal n1, QuadNormal n2, QuadNormal n3, QuadNormal n4)
  {
    tmpnormal.n1 = n1;
    tmpnormal.n2 = n2;
    tmpnormal.n3 = n3;
    tmpnormal.n4 = n4;
  }
  void MeshFace()
  {
    quad.push_back(tmpquad);
    m_poly = true;
  }
  void NormalFace()
  {
    normal.push_back(tmpnormal);
    m_normal = true;
  }
  void TexFace()
  {
    tex.push_back(tmptex);
    m_tex = true;
  }
  void ColorFace()
  {
    color.push_back(tmpcolor);
    m_color = true;
  }
  

  void MeshFrame()
  {
    quadindex.push_back(quad.size());
  }
  void TexFrame()
  {
    texindex.push_back(tex.size()); 
  }
  void ColorFrame()
  {
    colorindex.push_back(color.size());
  }
  void NormalFrame()
  {
    normalindex.push_back(normal.size());
  }

public:
  Quad tmpquad;
  Tex tmptex;
  QuadColor tmpcolor;
  Normal tmpnormal;

  std::vector<Quad> quad;
  std::vector<Tex> tex;
  std::vector<QuadColor> color;
  std::vector<Normal> normal;

  std::vector<int> quadindex;
  std::vector<int> texindex;
  std::vector<int> colorindex;
  std::vector<int> normalindex;
};

class Stream3dMesh : public Mesh
{
public:
  Stream3dMesh(Stream3d &s) : s(s) { }
  virtual int NumFrames() const { return s.quadindex.size(); }
  virtual int NumFaces(int framenum) const { return s.quadindex[0]; }
  virtual int NumPoints() const { return 4; }
  virtual Point FacePoint(int framenum, int face, int point) const
  {
    int index;
    if (framenum==0) index=0;
    else index = s.quadindex[framenum-1];

    Quad q = s.quad[index+face];
    if (point ==0) return q.p1;
    if (point ==1) return q.p2;
    if (point ==2) return q.p3;
    if (point ==3) return q.p4;
  }

private:
  Stream3d &s;
};
class Stream3dMeshNormals : public MeshNormals
{
public:
  Stream3dMeshNormals(Stream3d &s) : s(s) { }
  virtual int NumFrames() const { return s.normalindex.size(); }
  virtual Vector PointNormal(int framenum, int face, int point) const 
  { 
    int index;
    if (framenum==0) index=0;
    else index = s.normalindex[framenum-1];
    Normal n = s.normal[index+face];
    if (point==0) return n.n1;
    if (point==1) return n.n2;
    if (point==2) return n.n3;
    if (point==3) return n.n4;
  }

private:
  Stream3d &s;
};


class Stream3dMeshTexCoords : public MeshTexCoords
{
public:
  Stream3dMeshTexCoords(Stream3d &s) : s(s) { }
  virtual int NumFrames() const { return s.normalindex.size(); }
  virtual Point2d PointTexCoord(int framenum, int face, int point) const 
  { 
    int index;
    if (framenum==0) index=0;
    else index = s.texindex[framenum-1];
    Tex t = s.tex[index+face];
    if (point==0) return t.t1;
    if (point==1) return t.t2;
    if (point==2) return t.t3;
    if (point==3) return t.t4;
  }

private:
  Stream3d &s;
};


class Stream3dMeshColors : public MeshColors
{
public:
  Stream3dMeshColors(Stream3d &s) : s(s) { }
  virtual int NumFrames() const { return s.colorindex.size(); }
  virtual unsigned int VertexColor(int framenum, int face, int point) const 
  { 
    int index;
    if (framenum==0) index=0;
    else index = s.colorindex[framenum-1];
    QuadColor c = s.color[index+face];
    if (point==0) return c.c1.Pixel();
    if (point==1) return c.c2.Pixel();
    if (point==2) return c.c3.Pixel();
    if (point==3) return c.c4.Pixel();
  }

private:
  Stream3d &s;
};



Stream3d &operator<<(Stream3d &s, Stream3d & (*fptr)(Stream3d &s))
{
  return fptr(s);
}
Stream3d &operator<<(Stream3d &s, const Quad &q)
{
  s.PolyQuad(q.p1,q.p2,q.p3,q.p4);
  s.MeshFace();
  return s;
}
Stream3d &operator<<(Stream3d &s, const Tex &q)
{
  s.TexQuad(q.t1,q.t2,q.t3,q.t4);
  s.TexFace();
  return s;
}
Stream3d &operator<<(Stream3d &s, const QuadNormal &q)
{
  s.NormalQuad(q.n1,q.n2,q.n3,q.n4);
  s.NormalFace();
  rturn s;
}
Stream3d &operator<<(Stream3d &s, const QuadColor &q)
{
  s.ColorQuad(q.p1,q.p2,q.p3,q.p4);
  s.ColorFace();
  rturn s;
}


Stream3d &endmesh(Stream3d &s)
{
  if (s.m_poly) s.MeshFrame();
  if (s.m_tex) s.TexFrame();
  if (s.m_color) s.ColorFrame();
  if (s.m_normal) s.NormalFrame();
  s.m_poly = false;
  s.m_tex = false;
  s.m_color = false;
  s.m_normal = false;
  return s;
}


class Movement
{
public:
  virtual float NumFrames() const=0;
  virtual int NumObjects(float frame) const=0;
  virtual Point Position(float frame, int obj) const { return Point(0.0,0.0,0.0); }
  virtual Matrix Transform(float frame, int obj) const { Point p = Position(frame,obj); return Matrix::Translate(p.x,p.y,p.z); }
  virtual int VertexFrame(float frame, int obj) const=0;
  virtual int NormalFrame(float frame, int obj) const=0;
  virtual int ColorFrame(float frame, int obj) const=0;
  virtual int TexFrame(float frame, int obj) const=0;
};

void PrepareStream(Stream3d &s, ArrayRender &r);
void RenderMovement(Movement &m, float time, ArrayRender &r);


struct ObjectStream
{
public:
  ObjectStream(Stream3d &s) : s(s) { }

public:
  Stream3d &s;
  bool poly, tex, color, normal;
};
ObjectStream &operator<<(ObjectStream &s, ObjectStream &(*fptr)(ObjectStream &))
{
  return fptr(s);
}
ObjectStream &setpoly(ObjectStream &s)
{
  s.poly = true;
}
ObjectStream &clearpoly(ObjectStream &s)
{
  s.poly = false;
}
ObjectStream &operator<<(ObjectStream &s, const Sphere &s)
{
  if (poly)
    {
    }
  if (tex)
    {
    }
  if (color)
    {
    }
  if (normal)
    {
    }
}
ObjectStream &operator<<(ObjectStream &s, const FaceCollection &f)
{
  int faces = f.NumFaces();
  for(int i=0;i<faces;i++)
    {
      int points = f.NumPoints(i);
      if (points==3)
	{
	  if (poly)
	    {
	      Point p1 = f.FacePoint(i, 0);
	      Point p2 = f.FacePoint(i, 1);
	      Point p3 = f.FacePoint(i, 2);
	      Quad q = { p1, p2, p3, p3 };
	      s.s << q;
	    }
	  if (tex)
	    {
	      Point2d t1 = f.TexCoord(i, 0);
	      Point2d t2 = f.TexCoord(i, 1);
	      Point2d t3 = f.TexCoord(i, 2);
	      Tex t = { t1, t2, t3, t3 };
	      s.s << t;
	    }
	  if (color)
	    {
	      Color c1 = f.Color(i, 0);
	      Color c2 = f.Color(i, 1);
	      Color c3 = f.Color(i, 2);
	      QuadColor c = { c1, c2, c3, c3 };
	      s.s << c;
	    }
	  if (normal)
	    {
	      Vector v1 = f.Normal(i, 0);
	      Vector v2 = f.Normal(i, 1);
	      Vector v3 = f.Normal(i, 2);
	      QuadNormal n = { v1, v2, v3, v3 };
	      s.s << n;
	    }
	}
      if (points==4)
	{
	  if (poly)
	    {
	      Point p1 = f.FacePoint(i, 0);
	      Point p2 = f.FacePoint(i, 1);
	      Point p3 = f.FacePoint(i, 2);
	      Point p4 = f.FacePoint(i, 3);
	      Quad q = { p1, p2, p3, p4 };
	      s.s << q;
	    }
	  if (tex)
	    {
	      Point2d t1 = f.TexCoord(i, 0);
	      Point2d t2 = f.TexCoord(i, 1);
	      Point2d t3 = f.TexCoord(i, 2);
	      Point2d t4 = f.TexCoord(i, 3);
	      Tex t = { t1, t2, t3, t4 };
	      s.s << t;
	    }
	  if (color)
	    {
	      Color c1 = f.Color(i, 0);
	      Color c2 = f.Color(i, 1);
	      Color c3 = f.Color(i, 2);
	      Color c4 = f.Color(i, 3);
	      QuadColor c = { c1, c2, c3, c4 };
	      s.s << c;
	    }
	  if (normal)
	    {
	      Vector v1 = f.Normal(i, 0);
	      Vector v2 = f.Normal(i, 1);
	      Vector v3 = f.Normal(i, 2);
	      Vector v4 = f.Normal(i, 3);
	      QuadNormal n = { v1, v2, v3, v4 };
	      s.s << n;
	    }
	}
    }
  return s;
}

