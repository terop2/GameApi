
#ifndef FONT_HH
#define FONT_HH

#include "Effect.hh"
#include "GraphI.hh"
#include <vector>

using namespace std;
struct Path3d2
{
  std::vector<Point> vec;
};
struct Path2d2
{
  std::vector<Point2d> vec;
};

class Visitor2;

struct Loop2
{
  int inverse;
  VectorArray<Point2d> *points;
  void visit(Visitor2 &v);
  void init() { inverse=false; points=new VectorArray<Point2d>; }
};

class Visitor2
{
public:
  virtual void visit(bool &b)=0;
  virtual void visit(int &a)=0; // 0
  virtual void visit(float &b)=0; // 1
  virtual void visit(Point2d &p)=0; // 2
  virtual void visit(Point &p)=0; //3
  virtual void visit(VectorArray<Loop2> *array)=0; // 4
  virtual void visit(VectorArray<Point2d> *array)=0; // 5
};


class Parser {
public:
  Parser() : currentptr(0), size(0) { }
  int ParseInt(std::string s, bool &success);
  int ParseEnum(std::string *strings, int size, std::string s, bool &success);
  bool ParseBool(std::string s, bool &success);
  float ParseFloat(std::string s, bool &success);
  Point2d ParsePoint2d(std::string s, bool &success);
  Point ParsePoint(std::string s, bool &success);
  template<class T>
  VectorArray<T> *ParseArray(std::string s, bool &success);
  template<class T>
  T ParseStruct(std::string s, bool &success);

  Loop2 ParseLoop(std::string s, bool &success)
  {
    Loop2 l = ParseStruct<Loop2>(s,success);
    return l;
  }
  Path3d2 ParsePath3d(std::string s, bool &success);
  Path2d2 ParsePath2d(std::string s, bool &success);
public: // these parse without {}'s.
  template<class T>
  T ParseStruct2(std::string s, bool &success);
  template<class T>
  VectorArray<T> *ParseArray2(std::string s, bool &success);
private:
  template<class T>
  T *Alloc()
  {
    //delete[] currentptr;
    T *obj = new T;
    currentptr = (char*)obj;
    //currentptr = new char[sizeof(T)];
    //new(currentptr)T;
    size = sizeof(T);
    return obj;
  }
private:
  char *currentptr;
  int size;
};



class FontCharacter
{
public:
  virtual int NumLoops() const=0;
  virtual int NumPoints(int loop) const=0;
  virtual Point2d LoopPoint(int loop, int point) const=0;
  virtual bool Inverse(int loop) const=0;

  virtual float Advance() const=0;
  virtual Point2d PosTL() const=0; // from 0,0
  virtual Point2d PosBR() const=0; // from 0,0
};

class Font
{
public:
  virtual int NumCharacters() const=0;
  virtual const FontCharacter *FontGlyph(int ch) const=0;

  virtual int Character(int ch) const=0;
  virtual int FindCharacter(int character) const=0;
    
  virtual float BaseLine() const=0; // from TL to Baseline
  virtual float Height() const=0;
};

class FontCharacterModifiable
{
public:
  virtual void AddLoop()=0;
  virtual void AddPoint(int loop, Point2d pos)=0;
  virtual void MovePoint(int loop, int point, Point2d newpos)=0;
  virtual void ToggleInverse(int loop)=0;
  virtual void SetAdvance(float val)=0;
  virtual void SetTL(Point2d p)=0;
  virtual void SetBR(Point2d p)=0;
};

class DynamicFontCharacter : public FontCharacter, public FontCharacterModifiable
{
public:
  DynamicFontCharacter() : numloops(0) { }
  virtual int NumLoops() const { return numloops; }
  virtual int NumPoints(int loop) const { return numpoints[loop]; }
  virtual Point2d LoopPoint(int loop, int point) const { return find(loop,point); }
  virtual bool Inverse(int loop) const { return inverse[loop]; }

  virtual float Advance() const { return advance; }
  virtual Point2d PosTL() const { return tl; }
  virtual Point2d PosBR() const { return br; }
public:
  virtual void AddLoop() { numloops++; numpoints.push_back(0); inverse.push_back(false); }
  virtual void AddPoint(int loop, Point2d pos) { numpoints[loop]++; Data d = { loop, numpoints[loop]-1, pos }; points.push_back(d); }
  virtual void MovePoint(int loop, int point, Point2d newpos) { Point2d &p = find(loop, point); p=newpos; } 
  virtual void ToggleInverse(int loop) { inverse[loop] = !inverse[loop]; }
  virtual void SetAdvance(float val) { advance = val; }
  virtual void SetTL(Point2d p) { tl = p; }
  virtual void SetBR(Point2d p) { br = p; }
  struct Data
  {
    int loop;
    int point;
    Point2d pos;
  };
  Point2d &find(int loop, int point) const
  {
    int s = points.size();
    for(int i=0;i<s;i++)
      {
	Data &d = points[i];
	if (d.loop==loop && d.point==point)
	  {
	    //std::cout << "Returning " << d.pos.x << " " << d.pos.y << std::endl;
	  return d.pos;
	  }
      }
    std::cout << "DUMMY RETURNED" << std::endl;
    return dummy;
  }
private:
  int numloops;
  std::vector<int> numpoints;
  mutable std::vector<Data> points;
  std::vector<bool> inverse;
  float advance;
  Point2d tl,br;
  mutable Point2d dummy;
};

class DynamicFont : public Font
{
public:
  virtual int NumCharacters() const { return chars.size(); }
  virtual const FontCharacter *FontGlyph(int ch) const { return &chars[ch].ch; }

  virtual int Character(int ch) const 
  {
    return chars[ch].character;
  }
  virtual int FindCharacter(int character) const
  {
    int s = chars.size();
    for(int i=0;i<s;i++)
      {
	const Data &d = chars[i];
	if (d.character == character) return i;
      }
    return -1;
  }
    
  virtual float BaseLine() const { return baseline; }
  virtual float Height() const { return height; }
  struct Data
  {
    DynamicFontCharacter ch;
    int character;
  };
public:
  std::vector<Data> chars;
  float baseline;
  float height;
};


#if 0
class GlyphToPointCollection : public PointCollection2d
{
public:
  GlyphToPointCollection(const FontCharacter &ch, int loop) : ch(ch), loop(loop) { }
  int Size() const { return ch.NumPoints(loop); }
  Point2d Index(int i) const
  {
    //std::cout << "GlyphToPointCollection" << i << std::endl;
    return ch.LoopPoint(loop, i);
  }
private:
  const FontCharacter &ch;
  int loop;
};
#endif
class ChooseColorFunction : public Function<bool, Color>
{
public:
  ChooseColorFunction(Color c1, Color c2) : c1(c1), c2(c2) { }
  Color Index(bool b) const
  {
    if (b) return c1;
    return c2;
  }
private:
  Color c1,c2;
};

BufferRef RenderFont(Font *font, Array<int, int> &text, int zoomlevel); 

#if 0
class ScaleFontCharacter : public FontCharacter
{
public:
  ScaleFontCharacter(const FontCharacter &orig, float scalex, float scaley) : orig(orig), scalex(scalex), scaley(scaley) { }
  virtual ~ScaleFontCharacter() { }
  virtual int NumLoops() const { return orig.NumLoops(); }
  virtual int NumPoints(int loop) const { return orig.NumPoints(loop); }
  virtual Point2d LoopPoint(int loop, int point) const
  {
    Point2d p = orig.LoopPoint(loop, point);
    p.x *= scalex;
    p.y *= scaley;
    return p;
  }
  virtual bool Inverse(int loop) const { return orig.Inverse(loop); }

  virtual float Advance() const { return orig.Advance()*scalex; }
  virtual Point2d PosTL() const 
  { 
    Point2d p = orig.PosTL();
    p.x *= scalex;
    p.y *= scaley; 
    return p;
  }

  virtual Point2d PosBR() const
  {
    Point2d p = orig.PosBR();
    p.x *= scalex;
    p.y *= scaley;
    return p;
  }
  
private:
  const FontCharacter &orig;
  float scalex, scaley;
  
};
#endif
class ScaleFont : public Font
{
public:
  ScaleFont(Font &f, float scalex, float scaley) : f(f), scalex(scalex), scaley(scaley) /*, ptr(0)*/ { }
  virtual int NumCharacters() const { return f.NumCharacters(); }
  virtual const FontCharacter *FontGlyph(int ch) const
  {
    const FontCharacter* cha = f.FontGlyph(ch);
    //delete ptr;
    //ptr = new ScaleFontCharacter(*cha, scalex, scaley);
    //return ptr;
    return 0;
  }

  virtual int Character(int ch) const { return f.Character(ch); }
  virtual int FindCharacter(int character) const { return f.FindCharacter(character); }
    
  virtual float BaseLine() const { return f.BaseLine() * scaley; }
  virtual float Height() const { return f.Height() * scaley; }

private:
  Font &f;
  float scalex, scaley;
  //mutable ScaleFontCharacter *ptr;
};


struct FontCharacterImpl : public FontCharacter
{
  FontCharacterImpl(std::string text) 
    : success(false)
  {
    Parse(text, success);
  }
  virtual int NumLoops() const 
  {
    if (!success) return 0;
    std::cout << fd.vec << std::endl;
    return fd.vec->Size(); 
  }
  virtual int NumPoints(int loop) const 
  {
    if (!success) return 0;
    return fd.vec->Index(loop).points->Size(); 
  }
  virtual Point2d LoopPoint(int loop, int point) const
  {
    if (!success) return Point2d();
    return fd.vec->Index(loop).points->Index(point);
  }
  virtual bool Inverse(int loop) const 
  { 
    if (!success) return false;
    return fd.vec->Index(loop).inverse ==1; 
  }

  virtual float Advance() const { return fd.advance; }
  virtual Point2d PosTL() const { return fd.tl; } // from 0,0
  virtual Point2d PosBR() const { return fd.br; } // from 0,0
private:
  Parser p;
  bool success;
  struct FontData
  {
    float advance;
    Point2d tl;
    Point2d br;
    VectorArray<Loop2> *vec;
    void init() 
    { 
      advance=1.0; 
      Point2d p = { 0.0,0.0 };
      tl = p;
      br = p;
      vec=new VectorArray<Loop2>; 
    }
    void visit(Visitor2 &v)
    {
      v.visit(advance);
      v.visit(tl);
      v.visit(br);
      v.visit(vec);
    }
  };
  FontData fd;
private:
  void Parse(std::string s, bool &success)
  {
    //std::cout << "Font Parse data: '" << s << "'" << std::endl;
    fd = p.ParseStruct<FontData>(s,success);
    //std::cout << "Advance: " << fd.advance << std::endl;
    //std::cout << "tl: " << fd.tl.x << " " << fd.tl.y << std::endl;
    //std::cout << "br: " << fd.br.x << " " << fd.br.y << std::endl;
    //std::cout << "vec:" << fd.vec << std::endl;
  }

};

#endif
