#include "GameApi_h.hh"

#if 0
class OrElemTriStrip : public TriStrip
{
public:
  OrElemTriStrip(TriStrip *t1, TriStrip *t2) : t1(t1), t2(t2) { }
  virtual int Size() const { return 2 + t1->Size() + t2->Size(); }
  virtual Point Pos(int i) const
  {
    int s = t1->Size();
    if (i<s) { return t1->Pos(i); }
    if (i==s) { return t1->Pos(i-1); }
    if (i==s+1) { return t2->Pos(0); }
    return t2->Pos(i-2-s);
  }
  virtual unsigned int Color(int i) const
  {
    int s = t1->Size();
    if (i<s) { return t1->Color(i); }
    if (i==s) { return t1->Color(i-1); }
    if (i==s+1) { return t2->Color(0); }
    return t2->Color(i-2-s);
  }
  virtual Point2d TexCoord(int i) const
  {
    int s = t1->Size();
    if (i<s) { return t1->TexCoord(i); }
    if (i==s) { return t1->TexCoord(i-1); }
    if (i==s+1) { return t2->TexCoord(0); }
    return t2->TexCoord(i-2-s);
  }
  virtual Vector Normal(int i) const
  {
    int s = t1->Size();
    if (i<s) { return t1->Normal(i); }
    if (i==s) { return t1->Normal(i-1); }
    if (i==s+1) { return t2->Normal(0); }
    return t2->Normal(i-2-s);
  }
private:
  TriStrip *t1;
  TriStrip *t2;
};
class ArrayTriStrip : public TriStrip
{
public:
  ArrayTriStrip(std::vector<Point> vec) : vec(vec) { }
  virtual int Size() const { return vec.size(); }
  virtual Point Pos(int i) const { return vec[i]; }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int i) const
  {
    Point2d p = {0.0, 0.0 };
    return p;
  }
  virtual Vector Normal(int i) const
  {
    Vector v(0.0, 0.0, 0.0);
    return v;
  }

private:
  std::vector<Point> vec;
};
class FunctionTriStrip : public TriStrip
{
public:
  FunctionTriStrip(GameApi::Env &e, std::function<GameApi::PT (int)> f, int count) : e(e), f(f), count(count) { }
  virtual int Size() const { return count; }
  virtual Point Pos(int i) const { return *find_point(e, f(i)); }
  virtual unsigned int Color(int i) const { return 0xffffffff; }
  virtual Point2d TexCoord(int i) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
  virtual Vector Normal(int i) const
  {
    Vector v;
    return v;
  }

private:
  GameApi::Env &e;
  std::function<GameApi::PT (int)> f;
  int count;
};

#endif
class FromPolyTriStrip : public TriStrip
{
public:
  FromPolyTriStrip(FaceCollection *coll) : coll(coll) 
  {
    int f = coll->NumFaces();
    for(int i=0;i<f;i++)
      {
	int c = coll->NumPoints(i);
	for(int j=0;j<c/2+1;j++)
	  {
	    int jj = j;
	    Point p = coll->FacePoint(i,jj);
	    Vector n = coll->PointNormal(i,jj);
	    unsigned int col = coll->Color(i,jj);
	    Point2d tex = coll->TexCoord(i,jj);
	    pos.push_back(p);
	    color.push_back(col);
	    texcoord.push_back(tex);
	    normal.push_back(n);

	    Point pa = coll->FacePoint(i,c-jj-1);
	    Vector na = coll->PointNormal(i,c-jj-1);
	    unsigned int cola = coll->Color(i,c-jj-1);
	    Point2d texa = coll->TexCoord(i,c-jj-1);
	    pos.push_back(pa);
	    color.push_back(cola);
	    texcoord.push_back(texa);
	    normal.push_back(na);

	  }
	if (c!=0)
	  {
	    pos.push_back(pos[pos.size()-1]);
	    color.push_back(color[color.size()-1]);
	    texcoord.push_back(texcoord[texcoord.size()-1]);
	    normal.push_back(normal[normal.size()-1]);

	    pos.push_back(pos[pos.size()-1]);
	    color.push_back(color[color.size()-1]);
	    texcoord.push_back(texcoord[texcoord.size()-1]);
	    normal.push_back(normal[normal.size()-1]);
	  }
	if (i!=f-1) {
	  Point p = coll->FacePoint(i+1,0);
	  Vector n = coll->PointNormal(i+1,0);
	  unsigned int col = coll->Color(i+1,0);
	  Point2d tex = coll->TexCoord(i+1,0);
	  pos.push_back(p);
	  color.push_back(col);
	  texcoord.push_back(tex);
	  normal.push_back(n);

	  pos.push_back(p);
	  color.push_back(col);
	  texcoord.push_back(tex);
	  normal.push_back(n);
	  
	}
      }

  }
  virtual int Size() const { return pos.size(); }
  virtual Point Pos(int i) const { return pos[i]; }
  virtual unsigned int Color(int i) const { return color[i]; }
  virtual Point2d TexCoord(int i) const { return texcoord[i]; }
  virtual Vector Normal(int i) const { return normal[i]; }
private:
  FaceCollection *coll;
  std::vector<Point> pos;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
  std::vector<Vector> normal;
};
EXPORT GameApi::TS GameApi::TriStripApi::from_poly(P poly)
{
  FaceCollection *coll = find_facecoll(e, poly);
  return add_tri_strip(e, new FromPolyTriStrip(coll));
}
#if 0
class LoadTriStrip : public TriStrip
{
public:
  LoadTriStrip(std::string filename) {
    std::ifstream file(filename.c_str());
    file >> size;
    for(int i=0;i<size;i++)
      {
	Point p;
	unsigned int c;
	Point2d tx;
	Vector n;
	
	file >> p >> std::hex >> c >> std::dec >> tx >> n;
	pos.push_back(p);
	color.push_back(c);
	texcoord.push_back(tx);
	normal.push_back(n);
      }
  }

  virtual int Size() const { return size; }
  virtual Point Pos(int i) const { return pos[i]; }
  virtual unsigned int Color(int i) const { return color[i]; }
  virtual Point2d TexCoord(int i) const { return texcoord[i]; }
  virtual Vector Normal(int i) const { return normal[i]; }

private:
  int size;
  std::vector<Point> pos;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
  std::vector<Vector> normal;
};

EXPORT GameApi::TS GameApi::TriStripApi::load(std::string filename)
{
  return add_tri_strip(e, new LoadTriStrip(filename));
}

EXPORT GameApi::TS GameApi::TriStripApi::function(std::function<GameApi::PT (int)> f, int count)
{
  return add_tri_strip(e, new FunctionTriStrip(e, f, count));
}

class ColorFunctionTriStrip : public TriStripForward
{
public:
  ColorFunctionTriStrip(TriStrip *ts, std::function<unsigned int (int)> f) : TriStripForward(*ts), ts(ts), f(f) { }
  virtual unsigned int Color(int i) const
  {
    return f(i);
  }
private:
  TriStrip *ts;
  std::function<unsigned int(int)> f;
};
EXPORT GameApi::TS GameApi::TriStripApi::color_function(TS ts, std::function<unsigned int (int i)> f)
{
  TriStrip *ts1 = find_tri_strip(e, ts);
  return add_tri_strip(e, new ColorFunctionTriStrip(ts1, f));
}
class TexCoordTriStrip : public TriStripForward
{
public:
  TexCoordTriStrip(GameApi::Env &e, TriStrip *ts, std::function<GameApi::PT (int)> f) : TriStripForward(*ts), e(e), f(f) { }
  virtual Point2d TexCoord(int i) const
  {
    GameApi::PT p = f(i);
    Point *pt = find_point(e, p);
    Point2d pp = { pt->x, pt->y };
    return pp;
  }
private:
  GameApi::Env &e;
  std::function<GameApi::PT (int)> f;
};
EXPORT GameApi::TS GameApi::TriStripApi::texcoord_function(TS ts, std::function<PT (int i)>  f)
{
  TriStrip *ts1 = find_tri_strip(e, ts);
  return add_tri_strip(e, new TexCoordTriStrip(e, ts1, f));
}
class NormalTriStrip : public TriStripForward
{
public:
  NormalTriStrip(GameApi::Env &e, TriStrip *ts, std::function<GameApi::V (int i)> f) : TriStripForward(*ts), e(e), f(f) { }
  virtual Vector Normal(int i) const
  {
    GameApi::V v = f(i);
    Vector *vv = find_vector(e, v);
    return *vv;
  }
private:
  GameApi::Env &e;
  TriStrip *ts;
  std::function<GameApi::V (int i)> f;
};
EXPORT GameApi::TS GameApi::TriStripApi::normal_function(TS ts, std::function<V (int i)> f)
{
  TriStrip *ts1 = find_tri_strip(e, ts);
  return add_tri_strip(e, new NormalTriStrip(e, ts1, f));
}
EXPORT GameApi::TS GameApi::TriStripApi::from_array(PT *arr, int size)
{
  std::vector<Point> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(*find_point(e, arr[i]));
    }
  return add_tri_strip(e, new ArrayTriStrip(vec));
}
EXPORT GameApi::TS GameApi::TriStripApi::or_elem(TS t1, TS t2)
{
  TriStrip *tt1 = find_tri_strip(e, t1);
  TriStrip *tt2 = find_tri_strip(e, t2);
  return add_tri_strip(e, new OrElemTriStrip(tt1,tt2));
}

class OrArrayTriStrip : public TriStrip
{
public:
  OrArrayTriStrip(std::vector<TriStrip*> vec) : vec(vec) 
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	int ss = vec[i]->Size();
	for(int j=0;j<ss;j++)
	  {
	    pos.push_back(i);
	    pos2.push_back(j);
	  }
	pos.push_back(i);
	pos.push_back(i+1);
	pos2.push_back(ss-1);
	pos2.push_back(0);
      }
  }
  virtual int Size() const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	count += vec[i]->Size();
	if (i!=s-1)
	  {
	    count += 2;
	  }
      }
    return count;
  }
  virtual Point Pos(int i) const
  {
    int a1 = Pos1(i);
    int a2 = Pos2(i);
    return vec[a1]->Pos(a2);
  }
  virtual unsigned int Color(int i) const
  {
    int a1 = Pos1(i);
    int a2 = Pos2(i);
    return vec[a1]->Color(a2);
  }
  virtual Point2d TexCoord(int i) const
  {
    int a1 = Pos1(i);
    int a2 = Pos2(i);
    return vec[a1]->TexCoord(a2);
  }
  virtual Vector Normal(int i) const
  {
    int a1 = Pos1(i);
    int a2 = Pos2(i);
    return vec[a1]->Normal(a2);
  }
  
  int Pos1(int i) const
  {
    return pos[i];
  }
  int Pos2(int i) const
  {
    return pos2[i];
  }
private:
  std::vector<TriStrip*> vec;
  std::vector<int> pos;
  std::vector<int> pos2;
};
EXPORT GameApi::TS GameApi::TriStripApi::or_array(TS *array, int size)
{
  std::vector<TriStrip*> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(find_tri_strip(e, array[i]));
    }
  return add_tri_strip(e, new OrArrayTriStrip(vec));
}
#endif
class TriStripToPoly : public SingleForwardFaceCollection
{
public:
  TriStripToPoly(TriStrip *ts) : ts(ts) { }
  virtual int NumFaces() const {
    int s = ts->Size();
    if (s==0) return 0;
    if (s==1) return 0;
    return s-2;
  }
  virtual int NumPoints(int face) const
  {
    return 3;
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (face%2==1) { point=3-point-1; }
    return ts->Pos(face+point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (face%2==1) { point=3-point-1; }
    return ts->Normal(face+point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    if (face%2==1) { point=3-point-1; }
    return ts->Color(face+point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (face%2==1) { point=3-point-1; }
    return ts->TexCoord(face+point);
  }

private:
  TriStrip *ts;
};

EXPORT GameApi::P GameApi::TriStripApi::to_poly(TS strip)
{
  TriStrip *ts = find_tri_strip(e, strip);
  return add_polygon2(e, new TriStripToPoly(ts), 1);
}
#if 0
EXPORT void GameApi::TriStripApi::save(TS ts, std::string filename)
{
  TriStrip *tts = find_tri_strip(e, ts);
  std::ofstream file(filename);
  file << tts->Size() << std::endl;
  int s = tts->Size();
  for(int i=0;i<s;i++)
    {
      file << tts->Pos(i) << " " << std::hex << tts->Color(i) << " " << std::dec << tts->TexCoord(i) << " " << tts->Normal(i) << std::endl;
    }
  file.close();
}
#endif
