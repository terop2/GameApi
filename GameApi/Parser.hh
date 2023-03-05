#include <vector>
#include <sstream>
//#include <typeinfo>
#include "Graph.hh"
#include "Serialize.hh"

struct BitmapHandle
{
  int id;
  virtual ~BitmapHandle() { }
};


struct BitmapColorHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  ~BitmapColorHandle() { delete bm; }
};
struct BitmapIntHandle : public BitmapHandle
{
  Bitmap<int> *bm;
  ~BitmapIntHandle() { delete bm; }
};
struct BitmapArrayHandle : public BitmapHandle
{
  std::vector<BitmapHandle*> vec;
  std::vector<int> owned;
  ~BitmapArrayHandle() 
  {
    // NOT deleted here, because bitmaparray() uses find_bitmap() to find the
    // handles, and thus we don't have ownership of them.
    if (vec.size() != owned.size())
      {
	std::cout << "BitmapArrayHandle destructor: vector sizes do not match" << std::endl;
      }
    int s = vec.size();
    for(int i=0;i<s;i++) 
      if (owned[i]==1)
	delete vec[i];
  }
};
struct BitmapPosHandle : public BitmapHandle
{
  Bitmap<Pos> *bm;
  ~BitmapPosHandle() { delete bm; }
};
struct BitmapTileHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  int tile_sx, tile_sy;
  ~BitmapTileHandle() { delete bm; }
};

struct FaceCollPolyHandle
{
  FaceCollPolyHandle() : coll(0), collarray(0), collowned(false), collarrayowned(false) { }
  int id;
  FaceCollection *coll; 
  //FaceCollection **collarray;
  Array<int, FaceCollection*> *collarray;
  bool collowned;
  bool collarrayowned;
  //int size;
  bool padding1;
  bool padding2;
  ~FaceCollPolyHandle() { /*if (collowned) delete coll; if (collarrayowned) delete collarray;*/ }

};

#if 0

#if 0
struct PolyHandle
{
  int id;
  virtual ~PolyHandle() { }
};
#endif




class ParserPairBase 
{
public:
  //virtual std::type_info TypeInfo() const=0;
  virtual size_t SizeOf() const=0;
  struct PtrSize
  {
    char *ptr;
    size_t size;
  };
  virtual PtrSize BaseParse(std::string s, bool &success) const=0;
  virtual std::string BaseCreate(PtrSize psz) const=0;
};

template<class T>
class ParserPair : public ParserPairBase
{
public:
  virtual std::string Create(T t) const=0;
  virtual T Parse(std::string s, bool &success) const=0;
  //std::type_info TypeInfo() const { return typeid(T); }
  virtual size_t SizeOf() const { return sizeof(T); }
  virtual PtrSize BaseParse(std::string s, bool &success) const
  {
    PtrSize sz;
    T *ptr = new T;
    sz.ptr = (char*)ptr;
    sz.size = sizeof(T);
    *ptr = Parse(s, success);
    return sz;
  }
  virtual std::string BaseCreate(PtrSize psz) const
  {
    T *ptr = (T*)psz.ptr;
#if 0
    char *debugptr = (char*)ptr;
    std::cout << typeid(T).name() << ":";
    for(int i=0;i<psz.size;i++) { std::cout << std::hex << (int)debugptr[i] << " "; }
    std::cout << std::endl;
#endif
    std::string s = Create(*ptr);
    return s;
  }

};

template<class T, class K>
class ParserPairConversion : public ParserPair<K>
{
public:
  ParserPairConversion(ParserPair<T> &p) : p(p) { }
  virtual K Convert_1(T t) const=0;
  virtual T Convert_2(K k) const=0;
  virtual std::string Create(K k) const
  {
    T t = Convert_2(k);
    return p.Create(t);
  }
  virtual K Parse(std::string s, bool &success) const
  {
    T t = p.Parse(s,success);
    return Convert_1(t);
  }

private:
  ParserPair<T> &p;
};

template<class T>
class ObjectToPointerConversionParser : public ParserPairConversion<T,T*>
{
public:
  virtual T* Convert_1(T t) const
  {
    tt=t;
    return &tt;
  }
  virtual T Convert_2(T* t) const
  {
    return *t;
  }
private:
  T tt;
};

class CharParser : public ParserPair<char>
{
public: 
  std::string Create(char c) const { std::string s; s+=c; return s; }
  char Parse(std::string s, bool &success) const { 
    if (s.length()!=1) { success = false; return ' '; } 
    success = true; 
    return s[0]; 
  }
};

template<class T>
class VectorParser : public ParserPair<std::vector<T> >
{
public:
  VectorParser(ParserPair<T> &pair, int level) : pair(pair), level(level) { }
  std::string Create(std::vector<T> vec) const
  {
    int s2 = vec.size();
    std::string s;
    for(int i=0;i<s2;i++)
      {
	s += pair.Create(vec[i]);
	s += level_char();
      }
    return s;
  }
  std::vector<T> Parse(std::string s, bool &success) const
  {
    int index = 0;
    std::vector<T> vec;
    int ss = s.length();
    for(int i=0;i<ss;i++)
      {
	if (s[i]==level_char())
	  {
	    T t = pair.Parse(s.substr(index,i-index), success);
	    if (!success) { std::cout << "VectorParser failed" << std::endl; return vec; }
	    vec.push_back(t);
	    index = i+1;
	  }
      }
    success = true;
    return vec;
  }
  char level_char() const
  {
    char array[] = { '@', '%', '&', '!', 'a','b','c','d','e','f','g','h' };
    return array[level];
  }
private:
  ParserPair<T> &pair;
  int level;
};

template<class T>
class ArrayParser : public ParserPair<Array<int, T>*>
{
public:
  ArrayParser(ParserPair<T> &t, int level) : parser(t, level), v(0) { }
  virtual std::string Create(Array<int,T>* t) const
  {
    if (t==0) return "_";
    //std::cout << "ArrayParser:" << t << std::endl;
    int s = t->Size();
    std::vector<T> vec;
    for(int i=0;i<s;i++)
      {
	vec.push_back(t->Index(i));
      }
    return parser.Create(vec);
  }
  virtual Array<int,T>* Parse(std::string s, bool &success) const
  {
    if (s.length()==1 && s[0]=='_') { success = true; return 0; }
    delete v;
    v = 0;
    vec = parser.Parse(s,success);
    v = new VectorArray<T>(vec.begin(), vec.end());
    return v;
  }
private:
  VectorParser<T> parser;
  mutable std::vector<T> vec;
  mutable VectorArray<T> *v;
};

class StringParser : public ParserPair<std::string>
{
  std::string Create(std::string t) const { return t; }
  std::string Parse(std::string s, bool &success) const
  {
    success = true;
    return s;
  }
};

template<class T>
class StringStreamParser : public ParserPair<T>
{
public:
  std::string Create(T t) const 
  {
    std::stringstream ss;
    ss << t;
    return ss.str();
  }
  T Parse(std::string s, bool &success) const
  {
    //std::cout << "StringStream parser string: '" << s << "'" << std::endl;
    T i;
    std::stringstream ss;
    ss << s;
    if (ss >> i) { success = true; } else { std::cout << "StringStream Parser failed" << std::endl; success = false; }
    return i;
  }
};

class FloatParser : public StringStreamParser<float> { };
class IntParser : public StringStreamParser<int> { };
class UnsignedIntParser : public StringStreamParser<unsigned int> { };
class BoolParser : public StringStreamParser<bool> { };


template<class T>
class StructParser : public ParserPair<T>
{
public:
  StructParser(int level) : level(level) { }
  template<class TA>
  void push_back(const ParserPair<TA> &pair)
  {
    vec.push_back(&pair);
  }
  std::string Create(T mystruct) const
  {
    int s2 = vec.size();
    std::string s;
    int pos =0;
    for(int i=0;i<s2;i++)
      {
	const ParserPairBase *base = vec[i];
	char *ptr = ((char*)&mystruct) + pos;
	int sz = base->SizeOf();
	ParserPairBase::PtrSize ssz;
	ssz.ptr = ptr;
	ssz.size = sz;
	s += base->BaseCreate(ssz);
	s += level_char();
	pos += sz;
      }
    return s;
  }
  T Parse(std::string s, bool &success) const
  {
    int index = 0;
    T vec2;
    int ii=0;
    int pos = 0;
    int sk = s.length();
    for(int i=0;i<sk;i++)
      {
	if (s[i]==level_char())
	  {
	    const ParserPairBase *base = vec[ii];
	    char *ptr = ((char*)&vec2) + pos;
	    int sz = base->SizeOf();
	    ParserPairBase::PtrSize ssz = base->BaseParse(s.substr(index, i-index), success);
	    if(sz!=int(ssz.size)) { std::cout << "ParseStruct size problem" << std::endl; }
	    if (!success) { std::cout << "StructParser failed" << std::endl; return vec2; }
	    std::copy(ssz.ptr, ssz.ptr+ssz.size, ptr);
	    index = i+1;
	    pos+=sz;
	    ii++;
	  }
      }
    success = true;
    return vec2;
  }
  char level_char() const
  {
    char array[] = { '@', '%', '&', '!', 'a','b','c','d','e','f','g','h' };
    return array[level];
  }

private:
  std::vector<const ParserPairBase*> vec;
  int level;
};

template<class T>
class DiscriminatedUnionParser : public StructParser<T>
{
public:
  template<class TA>
  void push_back(const ParserPair<TA> &pair)
  {
    StructParser<T>::push_back(pair);
  }
};

class PointParser : public ParserPair<Point>
{
public:
  PointParser(int level) : vec(fl, level) { }
  std::string Create(Point p) const
  {
    std::vector<float> v;
    v.push_back(p.x);
    v.push_back(p.y);
    v.push_back(p.z);
    return vec.Create(v);
  }
  Point Parse(std::string s, bool &success) const
  {
    std::vector<float> v = vec.Parse(s, success);
    return Point(v[0],v[1],v[2]);
  }
private:
  VectorParser<float> vec;
  FloatParser fl;
};

class Point2dParser : public ParserPair<Point2d>
{
public:
  Point2dParser(int level) : vec(fl, level) { }
  std::string Create(Point2d p) const
  {
    std::vector<float> v;
    v.push_back(p.x);
    v.push_back(p.y);
    return vec.Create(v);
  }
  Point2d Parse(std::string s, bool &success) const
  {
    std::vector<float> v = vec.Parse(s, success);
    Point2d p;
    p.x = v[0];
    p.y = v[1];
    return p;
  }
private:
  VectorParser<float> vec;
  FloatParser fl;
};


class Vector3dParser : public ParserPair<Vector>
{
public:
  Vector3dParser(int level) : vec(fl, level) { }
  std::string Create(Vector p) const
  {
    std::vector<float> v;
    v.push_back(p.dx);
    v.push_back(p.dy);
    v.push_back(p.dz);
    return vec.Create(v);
  }
  Vector Parse(std::string s, bool &success) const
  {
    std::vector<float> v = vec.Parse(s, success);
    return Vector(v[0],v[1],v[2]);
  }
private:
  VectorParser<float> vec;
  FloatParser fl;
};


class FaceCollectionParser : public ParserPair<FaceCollection*>
{ // if you need to change the parser, make FaceCollectionParser2
  // so that old data can be still loaded.
public:
  FaceCollectionParser(int level)
    : pointsparser_level1(pointsparser_level2, level+1),
      facepoint_level1(facepoint_level2, level+1),
      facepoint_level2(facepoint_level3, level+2),
      facepoint_level3(level+3),
      pointnormal_level1(pointnormal_level2, level+1),
      pointnormal_level2(pointnormal_level3, level+2),
      pointnormal_level3(level+3),
      color_level1(color_level2, level+1),
      color_level2(color_level3, level+2),
      texcoord_level1(texcoord_level2, level+1),
      texcoord_level2(texcoord_level3, level+2),
      texcoord_level3(level+3),
      struct_parser(level+0)
  {
    struct_parser.push_back(NumFacesParser);
    struct_parser.push_back(pointsparser_level1);
    struct_parser.push_back(facepoint_level1);
    struct_parser.push_back(pointnormal_level1);
    struct_parser.push_back(color_level1);
    struct_parser.push_back(texcoord_level1);
  }
  struct FaceStruct
  {
    int numfaces;
    std::vector<int> numpoints;
    std::vector<std::vector<Point> > facepoint;
    std::vector<std::vector<Vector> > pointnormal;
    std::vector<std::vector<unsigned int> > color;
    std::vector<std::vector<Point2d> > texcoord;
  };
  class FaceStructFaceCollection : public SingleForwardFaceCollection
  {
  public:
    FaceStructFaceCollection(FaceStruct &st) : st(st) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

    virtual int NumFaces() const { return st.numfaces; }
    virtual int NumPoints(int face) const 
    {
      //std::cout << "numfaces=" << st.numfaces << std::endl;
      //std::cout << "FaceStrucctFaceCollection::NumPoints(" << face << ")" << std::endl;
      return st.numpoints[face]; 
    }
    virtual Point FacePoint(int face, int point) const
    {
      return st.facepoint[face][point];
    }
    virtual Vector PointNormal(int face, int point) const
    {
      return st.pointnormal[face][point];
    }
    virtual float Attrib(int face, int point, int id) const
    {
      return 0.0;
    }
    virtual int AttribI(int face, int point, int id) const
    {
      return 0;
    }
    virtual unsigned int Color(int face, int point) const
    {
      return st.color[face][point];
    }
    virtual Point2d TexCoord(int face, int point) const
    {
      return st.texcoord[face][point];
    }
  private:
    FaceStruct &st;
  };
  void Copy(FaceStruct &st, FaceCollection *coll) const
  {
    st.numfaces = coll->NumFaces();
    int s = st.numfaces;
    for(int i=0;i<s;i++)
      {
	st.numpoints.push_back(coll->NumPoints(i));
	st.facepoint.push_back(std::vector<Point>());
	st.pointnormal.push_back(std::vector<Vector>());
	st.color.push_back(std::vector<unsigned int>());
	st.texcoord.push_back(std::vector<Point2d>());
	for(int k=0;k<st.numpoints[i];k++)
	  {
	    st.facepoint[i].push_back(coll->FacePoint(i,k));
	    st.pointnormal[i].push_back(coll->PointNormal(i,k));
	    st.color[i].push_back(coll->Color(i,k));
	    st.texcoord[i].push_back(coll->TexCoord(i,k));
	  }
      }
  }
  virtual std::string Create(FaceCollection *t) const
  {
    if (t==0) return "_";
    FaceStruct s;
    Copy(s, t);
    return struct_parser.Create(s);
  }
  virtual FaceCollection* Parse(std::string s, bool &success) const
  {
    if (s.length()==1 && s[0]=='_') { success = true; return 0; }
    FaceStruct *ss = new FaceStruct;
    *ss = struct_parser.Parse(s, success);
    return new FaceStructFaceCollection(*ss);
  }

private:
  IntParser NumFacesParser;
  VectorParser<int> pointsparser_level1;
  IntParser pointsparser_level2;
  VectorParser<std::vector<Point> > facepoint_level1;
  VectorParser<Point> facepoint_level2;
  PointParser facepoint_level3;
  VectorParser<std::vector<Vector> > pointnormal_level1;
  VectorParser<Vector> pointnormal_level2;
  Vector3dParser pointnormal_level3;
  VectorParser<std::vector<unsigned int> > color_level1;
  VectorParser<unsigned int> color_level2;
  UnsignedIntParser color_level3;
  VectorParser<std::vector<Point2d> > texcoord_level1;
  VectorParser<Point2d> texcoord_level2;
  Point2dParser texcoord_level3;
  StructParser<FaceStruct> struct_parser;
};

template<class T>
class BitmapParser : public ParserPair<Bitmap<T>*>
{
public:
  BitmapParser(ParserPair<T> &pair, int level) : level1(level2,level+0), level2(pair, level+1), pair(pair) { }
  virtual std::string Create(Bitmap<T> *t) const
  {
    std::vector<std::vector<T> > vec;
    int sx = t->SizeX();
    int sy = t->SizeY();
    for(int i=0;i<sy;i++)
      {
	vec.push_back(std::vector<T>());
	for(int j=0;j<sx;j++)
	  {
	    vec[i].push_back(t->Map(i,j));
	  }
      }
    return level1.Create(vec);
  }
  template<class T2>
  class VectorVectorBitmap : public Bitmap<T2>
  {
  public:
    VectorVectorBitmap(std::vector<std::vector<T2> > &vec) : vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

    void Prepare() {}

    virtual int SizeX() const { if (vec.size()==0) return 0; return vec[0].size(); }
    virtual int SizeY() const { return vec.size(); }
    virtual T Map(int x, int y) const
    {
      return vec[x][y];
    }
  private:
    std::vector<std::vector<T2> > &vec;
  };
  virtual Bitmap<T> *Parse(std::string s, bool &success) const
  {
    std::vector<std::vector<T> > vec = level1.Parse(s, success);
    std::vector<std::vector<T> > *heap = new std::vector<std::vector<T> >(vec);
    return new VectorVectorBitmap<T>(*heap);
  }

private:
  VectorParser<std::vector<T> > level1;
  VectorParser<T> level2;
  ParserPair<T> &pair;
};

class EventInfoParser : public ParserPair<EventInfo>
{
public:
  EventInfoParser(int level)
    : event_info_parser(level)
  {
    event_info_parser.push_back(time_parser);
    event_info_parser.push_back(ac_event_id_parser);
    event_info_parser.push_back(deac_event_id_parser);
    event_info_parser.push_back(time_origin_id_parser);
    event_info_parser.push_back(key_value_parser);
    event_info_parser.push_back(state_change_id_parser);
    event_info_parser.push_back(timer_bool_parser);
    event_info_parser.push_back(key_down_bool_parser);
    event_info_parser.push_back(key_up_bool_parser);
    event_info_parser.push_back(ac_mouse_bool_parser);
    event_info_parser.push_back(deac_mouse_bool_parser);
    event_info_parser.push_back(state_change_bool_parser);
    event_info_parser.push_back(padding1);
    event_info_parser.push_back(padding2);
    
  }
  virtual std::string Create(EventInfo t) const
  {
    return event_info_parser.Create(t);
  }
  virtual EventInfo Parse(std::string s, bool &success) const
  {
    return event_info_parser.Parse(s,success);
  }
private:
  FloatParser time_parser;
  IntParser ac_event_id_parser;
  IntParser deac_event_id_parser;
  IntParser time_origin_id_parser;
  IntParser key_value_parser;
  IntParser state_change_id_parser;
  BoolParser timer_bool_parser;
  BoolParser key_down_bool_parser;
  BoolParser key_up_bool_parser;
  BoolParser ac_mouse_bool_parser;
  BoolParser deac_mouse_bool_parser;
  BoolParser state_change_bool_parser;
  BoolParser padding1;
  BoolParser padding2;
  StructParser<EventInfo> event_info_parser;
};

class LinkageInfoParser : public ParserPair<LinkageInfo>
{
public:
  LinkageInfoParser(int level) 
    : parser(level)
  {
    parser.push_back(start_parser);
    parser.push_back(end_parser);
  }
  virtual std::string Create(LinkageInfo t) const
  {
    return parser.Create(t);
  }
  virtual LinkageInfo Parse(std::string s, bool &success) const
  {
    return parser.Parse(s,success);
  }
private:
  IntParser start_parser;
  IntParser end_parser;
  StructParser<LinkageInfo> parser;
};

class PolyHandleParser : public ParserPair<FaceCollPolyHandle*>
{
public:
  PolyHandleParser(int level) :  coll(level+1), facesparser(level+2), arrayparser(facesparser,level+1), struct_parser(level)
  {
    struct_parser.push_back(id);
    struct_parser.push_back(coll);
    struct_parser.push_back(arrayparser);
    struct_parser.push_back(collowned);
    struct_parser.push_back(arrayowned);
    struct_parser.push_back(padding1);
    struct_parser.push_back(padding2);
  }
  std::string Create(FaceCollPolyHandle* t) const
  {
    if (t==0) return "_";
    FaceCollPolyHandle h = *t;
    h.collowned = false;
    h.collarrayowned = false;
    return struct_parser.Create(h);
  }
  FaceCollPolyHandle* Parse(std::string s, bool &success) const
  {
    if (s.length()==1 && s[0]=='_') { success = true; return 0; }
    // also minor ownership problems..
    faces =  struct_parser.Parse(s,success);
    // note: derived-to-base conversion here.
    return const_cast<FaceCollPolyHandle*>(static_cast<const FaceCollPolyHandle*>(&faces));
  }

private:
  IntParser id;
  FaceCollectionParser coll;
  BoolParser collowned;
  FaceCollectionParser facesparser;
  ArrayParser<FaceCollection*> arrayparser;
  BoolParser arrayowned;
  BoolParser padding1;
  BoolParser padding2;
  StructParser<FaceCollPolyHandle> struct_parser;
  mutable FaceCollPolyHandle faces; // this one is special
};

#if 0

template<class T, class K> // T = baseclass, K = derived class
class DynamicCastParser : public ParserPairConversion<T*, K*>
{
public:
  DynamicCastParser(ParserPair<T*> &p) : ParserPairConversion<T*,K*>(p) { }
  virtual K *Convert_1(T *t) const { return dynamic_cast<K*>(t); }
  virtual T *Convert_2(K *k) const { return k; }

};

#endif

#if 0
class PolyHandleParser : public ParserPair<PolyHandle*>
{
public:
  PolyHandleParser(int level) : parser(level), convert(parser) { }
private:
  FaceCollPolyHandleParser parser;
  DynamicCastParser<PolyHandle, FaceCollPolyHandle> convert;
  
};
#endif

#if 0
class BitmapHandleParser : public ParserPair<BitmapHandle*>
{
public:
  BitmapHandleParser(int level) : level(level), color_parser(level+3), bm_parser(color_parser, level+2)
  {
    color_parser.push_back(r_parser);
    color_parser.push_back(g_parser);
    color_parser.push_back(b_parser);
    color_parser.push_back(alpha_parser);
  }

  virtual std::string Create(BitmapHandle* t) const
  {
    BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(t);
    if (chandle)
      {
	IntParser p;
	std::string s = p.Create(1); // BitmapColorHandle=1
	StringParser string_parser;
	VectorParser<std::string> vec_parser(string_parser, level+1);
	std::vector<std::string> vec;
	vec.push_back(s);
	std::string s2 = bm_parser.Create(chandle->bm);
	vec.push_back(s2);
	return vec_parser.Create(vec);
      }
    else
      {
	std::cout << "ERROR, INVALID BITMAP TYPE IN SERIALIZE" << std::endl;
	IntParser p;
	std::string s = p.Create(-1); // BitmapColorHandle=1
	StringParser string_parser;
	VectorParser<std::string> vec_parser(string_parser, level+1);
	std::vector<std::string> vec;
	vec.push_back(s);
	std::string s2 = "";
	vec.push_back(s2);
	return vec_parser.Create(vec);
      }
    return "";
  }
  virtual BitmapHandle* Parse(std::string s, bool &success) const
  {
    StringParser string_parser;
    VectorParser<std::string> vec_parser(string_parser, level+1);
    std::vector<std::string> vec = vec_parser.Parse(s,success);
    if (!success) return NULL;
    std::string type_string = vec[0];
    IntParser p;
    int value = p.Parse(type_string, success);
    if (!success) return NULL;
    switch(value)
      {
      case 0: // BitmapColorHandle
	{
	std::string bitmap_string = vec[1];
	Bitmap<Color> *bm = bm_parser.Parse(bitmap_string, success);
	if (!success) return NULL;
	BitmapColorHandle *handle = new BitmapColorHandle;
	handle->bm = bm;
	return handle;
	}
      case -1:
	return 0;
      default:
	std::cout << "UNKNOWN BITMAP TYPE IN UNSERIALIZE" << std::endl;
	break;
      }
    return 0;
  }
private: 
  int level;
  IntParser r_parser, g_parser, b_parser, alpha_parser;
  StructParser<Color> color_parser;
  BitmapParser<Color> bm_parser;
};

#endif

class LinkInfoParser : public ParserPair<LinkInfo>
{
public:
  LinkInfoParser(int level) : poly_start(level+1), poly_end(level+1), bitmap_start(level+1), bitmap_end(level+1), link_info_parser(level) 
  {
    link_info_parser.push_back(start_event);
    link_info_parser.push_back(end_event);
    link_info_parser.push_back(poly_start);
    link_info_parser.push_back(poly_end);
    link_info_parser.push_back(bitmap_start);
    link_info_parser.push_back(bitmap_end);
#if 0
    link_info_parser.push_back(func1);
    link_info_parser.push_back(func1_scale);
    link_info_parser.push_back(func2);
    link_info_parser.push_back(func2_scale);
    link_info_parser.push_back(func3);
    link_info_parser.push_back(func3_scale);
    link_info_parser.push_back(func4);
    link_info_parser.push_back(func4_scale);
#endif
    link_info_parser.push_back(polygon_posx_func);
    link_info_parser.push_back(polygon_posy_func);
    link_info_parser.push_back(polygon_posz_func);
    link_info_parser.push_back(bitmap_posx_func);
    link_info_parser.push_back(bitmap_posy_func);
    link_info_parser.push_back(bitmap_posz_func);
    link_info_parser.push_back(polygon_rotx_func);
    link_info_parser.push_back(polygon_roty_func);
    link_info_parser.push_back(polygon_rotz_func);
    link_info_parser.push_back(sound_channel_func);
  }
  
  virtual std::string Create(LinkInfo t) const
  {
    return link_info_parser.Create(t);
  }
  virtual LinkInfo Parse(std::string s, bool &success) const
  {
    return link_info_parser.Parse(s,success);
  } 
private:
  IntParser start_event;
  IntParser end_event;
  PolyHandleParser poly_start;
  PolyHandleParser poly_end;
  BitmapHandleParser bitmap_start;
  BitmapHandleParser bitmap_end;
#if 0
  FunctionParser func1;
  BoolParser func1_scale;
  FunctionParser func2;
  BoolParser func2_scale;
  FunctionParser func3;
  BoolParser func3_scale;
  FunctionParser func4;
  BoolParser func4_scale;
#endif
  IntParser polygon_posx_func;
  IntParser polygon_posy_func;
  IntParser polygon_posz_func;
  IntParser bitmap_posx_func;
  IntParser bitmap_posy_func;
  IntParser bitmap_posz_func;
  IntParser polygon_rotx_func;
  IntParser polygon_roty_func;
  IntParser polygon_rotz_func;
  IntParser sound_channel_func;
  StructParser<LinkInfo> link_info_parser;
};

class NDimParser : public ParserPair<NDim<float,Point>*>
{
public:
  NDimParser(int level) : pointparser(level+1) { }
  virtual std::string Create(NDim<float,Point> *t) const
  {
    int sz = t->Count().size();
    std::cout << "sz: " << sz << std::endl;
    float *array = new float[sz];
    for(int i=0;i<sz;i++) { array[i] = 0.0; }
    std::string ss = "{";
    ss += intparser.Create(sz);
    ss += '#';
    ss += CreateRecurse(t, array, sz,sz);
    ss += '}';
    return ss;
  }
  virtual NDim<float,Point> *Parse(std::string s, bool &success) const
  {
    return NULL;
  }

  std::string CreateRecurse(NDim<float,Point> *t, float *array, int sz, int orig_sz) const
  {
    std::string s = "";
    if (sz==0) return s;
    //float val = array[sz-1];
    array[sz-1]=0.0;
    s+=CreateRecurse(t, array, sz-1, orig_sz);
    array[sz-1]=1.0;
    s+=CreateRecurse(t, array, sz-1, orig_sz);
    array[sz-1]=0.0;
    s+='[';
    s+=pointparser.Create(t->Map(array, orig_sz));
    s+=']';
    array[sz-1]=1.0;
    s+='[';
    s+=pointparser.Create(t->Map(array, orig_sz));
    s+=']';
    //std::string s;
    return s;
  }
  IntParser intparser;
  PointParser pointparser;
};

class PosInfoParser : public ParserPair<PosInfo>
{
public:
  PosInfoParser(int level) : curve(level+1), struct_parser(level) 
  {
    struct_parser.push_back(link);
    struct_parser.push_back(curve);
  }
  virtual std::string Create(PosInfo t) const
  {
    return struct_parser.Create(t);
  }
  virtual PosInfo Parse(std::string s, bool &success) const
  {
    return struct_parser.Parse(s,success);
  }
private:
  IntParser link;
  NDimParser curve;
#if 0
  CurveIn3dParser curve;
#endif
  StructParser<PosInfo> struct_parser;
};

class MousePlaneParser : public ParserPair<MousePlane*>
{
public: // TODO
  MousePlaneParser(int level) { }
  virtual std::string Create(MousePlane* t) const
  {
    if (t==0) return "_";
    return "_";
  }
  virtual MousePlane* Parse(std::string s, bool &success) const
  {
    if (s.length()==1 && s[0]=='_') { success = true; return 0; }
    success=true;
    return 0;
  }

private:
};

class StateInfo2Parser : public ParserPair<StateInfo2>
{
public:
  StateInfo2Parser(int level) : enable_obj(boolparser, level+1), plane(level+1), struct_parser(level) 
  {
    struct_parser.push_back(enable_obj);
    struct_parser.push_back(plane);
  }
  virtual std::string Create(StateInfo2 t) const
  {
    return struct_parser.Create(t);
  }
  virtual StateInfo2 Parse(std::string s, bool &success) const
  {
    return struct_parser.Parse(s,success);
  }

private:
  BoolParser boolparser;
  ArrayParser<bool> enable_obj;
  MousePlaneParser plane;
  StructParser<StateInfo2> struct_parser;
};

class SequencerParser : public ParserPair<Sequencer2*>
{
public:
  SequencerParser() : 
    events_parser(eventinfo_parser, 1),
    eventinfo_parser(1),
    linkage_parser(linkageinfo_parser, 1),
    linkageinfo_parser(1),
    links_parser(linkinfo_parser, 1),
    linkinfo_parser(1),
    pos_parser(posinfo_parser, 1),
    posinfo_parser(1),
    states_parser(stateinfo2_parser,1),
    stateinfo2_parser(1),
    struct_parser(0) 
  {
    struct_parser.push_back(events_parser);
    struct_parser.push_back(linkage_parser);
    struct_parser.push_back(links_parser);
    struct_parser.push_back(pos_parser);
    struct_parser.push_back(states_parser);
  }
  struct SeqData
  {
    std::vector<EventInfo> events;
    std::vector<LinkageInfo> linkage;
    std::vector<LinkInfo> links;
    std::vector<PosInfo> pos;
    std::vector<StateInfo2> states;
  };
  class SeqDataSequencer2 : public Sequencer2
  {
  public:
    SeqDataSequencer2(SeqData &data) : data(data) { }
    virtual int NumEvents() const { return data.events.size(); }
    virtual EventInfo Event(int i) const { return data.events[i]; }
    
    virtual int NumLinks() const { return data.linkage.size(); }
    virtual LinkageInfo Linkage(int i) const { return data.linkage[i]; }
    virtual LinkInfo Links(int i) const { return data.links[i]; }
    // positions
    virtual int NumPositions() const { return data.pos.size(); }
    virtual PosInfo Positions(int i) const { return data.pos[i]; }
    // states
    virtual int NumStates() const { return data.states.size(); }
    virtual StateInfo2 States(int i) const { return data.states[i]; }
    
  private:
    SeqData &data;
  };
  virtual std::string Create(Sequencer2* t) const
  {
    SeqData data;
    int s1 = t->NumEvents();
    for(int i1=0;i1<s1;i1++)
      {
	data.events.push_back(t->Event(i1));
      }
    int s2 = t->NumLinks();
    for(int i2=0;i2<s2;i2++)
      {
	data.linkage.push_back(t->Linkage(i2));
	data.links.push_back(t->Links(i2));
      }
    int s3 = t->NumPositions();
    for(int i3=0;i3<s3;i3++)
      {
	data.pos.push_back(t->Positions(i3));
      }
    int s4 = t->NumStates();
    for(int i4=0;i4<s4;i4++)
      {
	data.states.push_back(t->States(i4));
      }
    return struct_parser.Create(data);
  }
  virtual Sequencer2* Parse(std::string s, bool &success) const
  {
    SeqData data = struct_parser.Parse(s, success);
    if (!success) return 0;
    SeqData *heap = new SeqData(data);
    return new SeqDataSequencer2(*heap);
  }
private:
  VectorParser<EventInfo> events_parser;
  EventInfoParser eventinfo_parser;
  VectorParser<LinkageInfo> linkage_parser;
  LinkageInfoParser linkageinfo_parser;
  VectorParser<LinkInfo> links_parser;
  LinkInfoParser linkinfo_parser;
  VectorParser<PosInfo> pos_parser;
  PosInfoParser posinfo_parser;
  VectorParser<StateInfo2> states_parser;
  StateInfo2Parser stateinfo2_parser;
  StructParser<SeqData> struct_parser;
};
#endif
