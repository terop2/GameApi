


#include "Graph.hh"
#include <string>

// gives out 0..255
struct GlyphPriv;
class FontGlyphBitmap : public Bitmap<int>
{
public:
  FontGlyphBitmap(void *priv_, std::string filename, int sx, int sy);
  void load_glyph(long idx);
  int bitmap_top(long idx) const;
  virtual int SizeX() const;
  virtual int SizeY() const;
  virtual int Map(int x, int y) const;

  ~FontGlyphBitmap();
private:
  GlyphPriv *priv;
};
