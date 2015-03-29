


#include "Graph.hh"
#include <string>
#include <vector>
// gives out 0..255
struct GlyphPriv;
class FontLineCollectionWrapper : public PlanePoints2d
{
public:
  FontLineCollectionWrapper(LineCollection *coll, std::vector<int> &types, float sx, float sy, float dx, float dy) : coll(coll), m_sx(sx), m_sy(sy), m_dx(dx), m_dy(dy), types(types)  { }
  virtual float SizeX() const { return m_sx; }
  virtual float SizeY() const { return m_sy; }
  virtual int Size() const { return coll->NumLines()+1; }
  virtual Point2d Map(int i) const {
    Point p = coll->LinePoint(i,0);
    if (i==Size()-1) p=coll->LinePoint(Size()-2,1);
    Point2d pp = { p.x, p.y };
    //std::cout << "Point:" << i << ":" << pp.x << " " << pp.y << std::endl;
    pp.x+=m_dx;
    pp.y+=m_dy;
    return pp;
  }
  virtual PlanePointsType Type(int i) const 
  { 
    //std::cout << "Type: " << i << ":" << types[i] << std::endl;
    if (types[i]==0) return PlanePoints2d::EMove;
    if (types[i]==1) return PlanePoints2d::ELineTo;
    if (types[i]==2) return PlanePoints2d::EConic;
    if (types[i]==3) return PlanePoints2d::ECubic;
    return PlanePoints2d::EMove;
    //if (i==0) return EMove;
    //return ELineTo;
  }

private:
  LineCollection *coll;
  float m_sx, m_sy;
  float m_dx, m_dy;
  std::vector<int> types;
};

class FontGlyphBitmap : public Bitmap<int>, public LineCollection
{
public:
  FontGlyphBitmap(void *priv_, std::string filename, int sx, int sy);
  void load_glyph(long idx);
  void load_glyph_outline(long idx, float sx, float sy);
  int bitmap_top(long idx) const;
  virtual int SizeX() const;
  virtual int SizeY() const;
  virtual int Map(int x, int y) const;

  virtual int NumLines() const;
  virtual Point LinePoint(int line, int point) const;

  std::vector<int> &Types() const {return const_cast<std::vector<int>&>(types); }

  //virtual float SizeX() const { return m_sx; }
  //virtual float SizeY() const { return m_sy; }

  ~FontGlyphBitmap();
private:
  GlyphPriv *priv;
public:
  float m_sx, m_sy;
  Point2d move_point;
  std::vector<int> types;
  std::vector<Point2d> points; // twice the amount of points
  std::vector<Point2d> control1;
  std::vector<Point2d> control2;
};
