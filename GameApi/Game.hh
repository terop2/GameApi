
#include "Effect.hh"
#include "Graph.hh"

struct Position2d;

class GameEffect : public FrameAnimPlugins
{
public:
  GameEffect(const Position2d &p, Render *r) : FrameAnimPlugins(r), p(p) { }
  void Init();
  void PreFrame(float time);
  bool Frame(float time);
  float XRot() const { return 0.0; }
  float YRot() const { return 0.0; }
private:
  const Position2d &p;
  VBOState vbostate;
  //VBOState vbostate2;
};

typedef int Type;

class TypeArray : public Array<int, Type>
{
};

class TypeToPoly : public Function<Type, const BoxableFaceCollection*>
{
public:
  virtual int NumTypes() const=0;
  virtual Type TypeIndex(int i) const=0;
};
void UpdateVBOTypeToPoly(const TypeToPoly &tp, VBOState &state, VBOUpdate update);

class TypeToPolyArray : public Array<int, TypeToPoly*>
{
};
typedef ArrayConvert<TypeToPolyArray, int, TypeToPoly*> TypeToPolyArrayConvert;

class TypeToPolyArrayFunction : public TypeToPoly
{
public:
  TypeToPolyArrayFunction(TypeToPolyArray &arr) : arr(arr) { }
  int NumTypes() const
  {
    int s = arr.Size();
    int count=0;
    for(int i=0;i<s;i++)
      {
	int ii = arr.Index(i)->NumTypes();
	count += ii;
      }
    return count;
  }
  Type TypeIndex(int iii) const
  {
    int s = arr.Size();
    int count=0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	int ii = arr.Index(i)->NumTypes();
	count += ii;
	if (count > iii)
	  {
	    return arr.Index(i)->TypeIndex(iii-oldcount);
	  }
      }
    return count;
  }
  const BoxableFaceCollection *Index(Type t) const
  {
    int s = arr.Size();
    for(int i=0;i<s;i++)
      {
	const BoxableFaceCollection *coll = arr.Index(i)->Index(t);
	if (coll)
	  {
	    return coll;
	  }
      }
    return 0;
  }
private:
  TypeToPolyArray &arr;
};

class FaceCollectionPoly : public TypeToPoly
{
public:
  FaceCollectionPoly(Type tt, const BoxableFaceCollection *poly) : tt(tt), poly(poly) { }
  const BoxableFaceCollection* Index(Type t) const { if (t==tt) return poly; else return 0;  }
  int NumTypes() const { return 1; }
  Type TypeIndex(int i) const { return tt; }
private:
  Type tt;
  const BoxableFaceCollection *poly;
};

class GridPoly : public TypeToPoly
{
public:
  GridPoly(Type t, Bitmap<Type> &bm, TypeToPoly &poly, Point p, Vector u_x, Vector u_y, float scale) : t(t), bm(bm), poly(poly), p(p), u_x(u_x), u_y(u_y), scale(scale) { Init(); }

  void Init()
  {
    vec = new VectorArray<const FaceCollection*>;

    int sx = bm.SizeX();
    int sy = bm.SizeY();
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  Point pp = p + u_x*x+u_y*y;
	  Type type = bm.Map(x,y);
	  const BoxableFaceCollection *faces = poly.Index(type);
	  const BoxableFaceCollection *moveface = new MatrixElem(*faces, Matrix::Scale(scale, scale, scale)*Matrix::Translate(pp.x,pp.y,pp.z));
	  vec->push_back(moveface);
	}
    conv = new FaceCollectionArrayConvert(*vec);
    compress = new CompressObject(*conv);
    boxable = new BoxableFaceCollectionConvert(*compress);
  }
  int NumTypes() const
  {
    return 1;
  }
  Type TypeIndex(int i) const
  {
    return t;
  }
  const BoxableFaceCollection* Index(Type tt) const
  {
    if (tt==t)
      {
	return boxable;
      }
    else
      {
	return 0;
      }
  }
  
private:
  Type t;
  Bitmap<Type> &bm;
  TypeToPoly &poly;
  Point p;
  Vector u_x, u_y;

  VectorArray<const FaceCollection *> *vec;
  FaceCollectionArrayConvert *conv;
  CompressObject *compress;
  BoxableFaceCollectionConvert *boxable;
  float scale;
}; 

class CombinePoly : public TypeToPoly
{
public:
  CombinePoly(Type t, Array<int,Type> &arr, TypeToPoly &polys)
    : t(t), arr(arr), polys(polys) { Init(); }
  int NumTypes() const
  {
    return 1;
  }
  Type TypeIndex(int iii) const
  {
    return t; 
  }
  void Init()
  {
	vec = new VectorArray<const FaceCollection*>;
	int s = arr.Size();
	for(int i=0;i<s;i++)
	  {
	    Type type = arr.Index(i);
	    const FaceCollection *faces = polys.Index(type);
	    vec->push_back(faces);
	  }
	conv = new FaceCollectionArrayConvert(*vec);
	compress = new CompressObject(*conv);
	boxable = new BoxableFaceCollectionConvert(*compress);
  }
  const BoxableFaceCollection *Index(Type tt) const
  {
    if (t == tt)
      {
	return boxable;
      }
    else
      return 0;
  }

private:
  Type t;
  Array<int,Type> &arr;
  TypeToPoly &polys;

  VectorArray<const FaceCollection *> *vec;
  FaceCollectionArrayConvert *conv;
  CompressObject *compress;
  BoxableFaceCollectionConvert *boxable;
};

class TunnelWall : public TypeToPoly
{
public:
  TunnelWall(Type t, std::string filename, std::vector<FramePlugin*> *plugin) : t(t), filename(filename), plugin(plugin), r(0), coords(0), te(0)
  {
    Init();
  }
  void Init();
  int NumTypes() const
  {
    return 1;
  }
  Type TypeIndex(int iii) const
  {
    return t; 
  }
  const BoxableFaceCollection *Index(Type tt) const
  {
    if (t==tt)
      {
	return boxable;
      }
    else
      return 0;
  }
private:
  Type t;
  std::string filename;

  std::vector<FramePlugin*> *plugin;
  RectangleElem *r;
  SimpleTexCoords *coords;
  TextureElem *te;

  MatrixElem *top;
  MatrixElem *bottom;
  MatrixElem *left;
  MatrixElem *right;
  VectorArray<const FaceCollection*> *vec;
  FaceCollectionArrayConvert *convert;
  FaceCollection *compress;
  BoxableFaceCollectionConvert *boxable;
};


//
//
//
//
//

class ObjectPlane2d
{
public:
  virtual int NumObjects() const=0;
  virtual float Rotation(int obj) const=0;
  virtual Point2d Position(int obj) const=0;
  virtual float Width() const=0;
  virtual float Height() const=0;
};

class PlaneFocus
{
public:
  virtual int NumFocus() const=0;
  virtual float FocusRadius(int num) const=0;
  virtual Point2d FocusPosition(int num) const=0;
};

class PlaneCircleBorder : public ContinuousBitmap<bool>
{
public:
  // border_pos = [-1..1]
  PlaneCircleBorder(Point2d pos, float sx, float sy, float radius, float border_width, float border_pos) : pos(pos), sx(sx), sy(sy), radius(radius), border_width(border_width), border_pos(border_pos) { } 
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  bool Map(float x, float y) const
  {
    float distx = x-pos.x;
    float disty = y-pos.y;
    float dist = sqrt(distx*distx + disty*disty);
    dist += border_width*border_pos;
    return dist >= radius && dist < radius+border_width;
  }
private:
  Point2d pos;
  float sx,sy,radius, border_width, border_pos;
};

class TimeStep
{
public:
  virtual void DoChanges(float time)=0;
};

class BooleanTimeStep : public TimeStep
{
public:
  BooleanTimeStep(bool initial_value, bool change_required_initial, float time_required_for_one_flip) : value(initial_value),  change_required(change_required_initial), req(time_required_for_one_flip), remaining(0.0) { }
  bool value;
  bool change_required;
public:
  void DoChanges(float time) {
    float delta = time - old_time + remaining;
    delta /= req;
    int d = (int)delta;
    remaining = delta - d;
    bool b = (d & 1)==1;
    if (change_required)
      value = value ^ b;
  old_time = time;
  }
private:
  float req;
  float old_time;
  float remaining;
};

class RIAAWaveform : public Waveform
{
public:
  RIAAWaveform(float length) : timestep(false, true, 1.0), length(length) { }
  float Length() const { return length; }
  float Min() const { return 0.0; }
  float Max() const { return 1.0; }
  float Index(float val) const {
    if (timestep.value) return 1.0;
    return 0.0;
  }
  void AdvanceTime(float time) { timestep.DoChanges(time); }
private:
  BooleanTimeStep timestep;
  float length;
};

class Fusion
{
public:
};

class CategoryTheory
{
  // sets for mathematics
  // categories for working mathematician
};

class Voxel2
{
public:
  void Render(ColorBitmap *colors)
  {
  }

private:
  //BufferRef ref;
};

// tero, you can fix control operator. The unsafeperformio is no longer
// needed. You have all the info needed. Exceptions. a->_|_. intersections.
// pullbacks. tHIS IS IMPORTANT DEVELOPMENT.NEED TO ASK HOW IT usES PULLBacks.

class Independent
{
  // independent of electricity grid.
  // dependencies are important.
  // write something that tracks dependencies.
  // category theory is good choice for this.
  // cold fusion solves this problem for electricity.
};

class Clock
{
  // interference is dangerous
  // very amazing looking pattern, but difficult to control
  // we do not have control operator for it.
  // important development needed to make it happen
  // intersections are the main problem
  // vector addition is not good enough
  // michaelson-morley interference experiment caused it
  // E=mc^2 was the result of this
  // this is why the interference pattern in the ad is so dangerous.
  // you should not know this.
  // something failed.
  // lightspeed is leaking too
  // internet has a fix for this, and it should not happen any longer
  // but it's still leaking. We need to know why
  // something to do with combining different stuff together.
  // basic physics for clocks is just a "heiluri".
  // that stuff is no longer used in clocks, instead they use something
  // more accurate.
  // computer level clocks do have some problems it seems
  // the gps relies on accurate clocks
  // E=mc^2 + relativity is needed in gps
  // some people have strage satellite signature
  // we cannot explain it
  // difficult to control who buys which satellite tv channels
  // this is so well developed area that there are large number of rules
  // that need to be accurately followed.
  // this makes it dangerous
  // balance is very difficult to archieve with current systems
  // but it's needed by other stuff
  // chess like battle is all around us. This is good alternative.
  // balance is also difficult to archieve in there.
  // but it was done, and both sides are equal. (except opening move)
  // binary numbers used already in chess: either the block is occupied
  // or it is not occupied in chessboard.
  // intersections are also used with the 8x8 grid
  // color-coding is used in chessboard
  // batman colours
  // roads & cities caused this basically
  // they had lots of intersections
  // and those are collision-hazard
  // cars are a result of roads
  // parking place = 2d problem
  // 3d parking solved part of the problem
  // electricity is 3d
  // games have both 2d and 3d elements
  // math helped with 2d and 3d understanding
  // 3d chess never happened except in star trek
  // star trek style teleporters never happened
  // moving stuff is still difficult
  // combinations are still not solved
  // text/writing is everywhere
  // bookstores are IMPORTANT!
  // category theory + intersections = bad combination
  // darwin -> globalisation
  // the web is amazing
  // irc is strange.
  // this is quite good idea btw.
  // Codex_.
  // Paranoids.
  // sounds right.
  // really?
  // yup.
  // ##C++
  // definitely right
  // Now we found him!
  // Seems pretty complex stuff.
  // speed -> assembly'95. Sounds right.
  // show me the demo, immediately. Sounds 3d to me.
  // wow. This is nice.
  // what intersections are possible now?
  // IMPORTANT INFO: NOTHING WILL WORK.
  // THIS IS TOO FAST!
  // computers did this!
  // definitely.
  // computer-internet -category makes people too fast!
  // really?
  // 
};

class Names
{
  // remembering names is broken because of logic.
  // they somehow require that everything has some logic in them.
  // there is no logic in how names are chosen.
  // so they cannot remember them.
  // that's the real problem with them. Too much logic, too little
  // random information.
  // RANDOM!
  // NO, this didnt work, we tried it.
  // it caused bigger problem when they tried to find the logic buried
  // in the random patterns.
  // Everyone we talk to is saying the same thing.
  // some replacement needs to be developed for this.
  // name tags works in workplace.
  // yup, that's good solution.
  // HOW DID THIS GET SO FUCKED UP?
  // oh, this is a real big problem.
  // 
};

class CompressedInformation
{
  // requirements are basically just names of features
  // the people can write code based on just the name
  // oh, that's nice new feature.
  // we never had that.
  // not all of them can.
  // what is special in these people who can do it?
  // they have lots more experience.
  // oh.
  // this one might have experience that we don't know about.
  // yup, assembly + open source
  // oh, nice
  // 
};

class Pizza : public PointCollection2d
{
public:
  Pizza(int numpoints_in_whole_circle, float angle1, float angle2, float radius)
    : num(numpoints_in_whole_circle), angle1(angle1), angle2(angle2), r(radius) 
  {
    if (angle2<angle1) { std::swap(angle1,angle2); }
  }
  bool Inside(Point p) const {
    return false;
  }
  int Size() const { return 1+2+num*(angle2-angle1)/2/3.14159; }
  Point2d Index(int i) const
  {
    if (i==0) { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
    if (i==1) {
      Point2d p;
      p.x = 0.0 + r*cos(angle1);
      p.y = 0.0 + r*sin(angle1);
      return p;
    }
    if (i==Size()-1)
      {
	Point2d p;
	p.x = 0.0 + r*cos(angle2);
	p.y = 0.0 + r*sin(angle2);
	return p;
      }
    i-=2;
    int i2 = angle1/3.14159/2.0*(Size()-2)+1;
    float angle2 = i2 * 3.14159*2.0 / num;
    Point2d p;
    p.x = 0.0 + r*cos((i2+i)*angle2);
    p.y = 0.0 + r*sin((i2+i)*angle2);
    return p;
  }
private:
  int num;
  float angle1, angle2;
  float r;
};

// Intersection problem solution proposals:

#if 0

void Intersection(const T &t, const K &k, bool &res); // full pullback, boolean intersection
void Intersection(const UID &id1, const UID &id2, bool &res); // db join, comparision
void Intersection(Function<A,bool> &f1, Function<B,bool> &f2, Function<pair<A,B>,bool> &res); // A->2 && B->2, set membership solution
// C++ class hierarchy in pullback shape.
// Unique up-to isomorphism. Single solution exists normally, but if isomorphism line is broken, then there are more solutions.
// THERE IS ONLY one path in a pullback from T->AxB. and thus only one path
// will need to be handled.
// There is no comparision etc at all. And no intersection.
// T could have more elements and then it gets very complicated.
// but normally we can rely on one path given the following restrictions:
// 1) up-to isomorphism
//      a) several solutions to the equation is breakages in isomorphism
//
template<class T, class K, class S> 
void Pullback(const T &t, const K &k, const bool &b, S &s,
	      Function<T,S> &pb1, Function<K,S> &pb2) // TxK->S pullback.
{
  if (b)
    {
      return pb1.Index(t);
    }
  else
    {
      return pb2.Index(k);
    }
}

// UNIQUE ELEMENT IN THE EQUALIZER type E results in a two different elements of S. They're different representations of the same thing. 

// Ax(C)B is not a product. Single element from either A or B is chosen. It's actually either-or.
// This will solve pullbacks.
// S_1 and S_2 will need to be isomorphic.
// Ax(C)B is actually both either-or and product at the same time. Only one element will be chosen from A+B, but product is needed so that other dimension can be ignored when correct path is chosen.
// e |-> S_1
// e |-> S_2
// S_1 ~= S_2 (isomorphism)

#endif

