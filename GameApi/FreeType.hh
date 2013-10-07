


#include "Graph.hh"
#include <string>
#include <vector>
// gives out 0..255
struct GlyphPriv;
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
