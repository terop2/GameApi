
#include "Graph.hh"
#include <string>
#include <vector>

struct GlyphData;
class FontInterfaceImpl : public FontInterface
{
public:
  FontInterfaceImpl(GameApi::Env &e, void *priv_, std::string ttf_filename, std::string homepage, int sx, int sy);
  virtual int Top(long idx) const;
  virtual int SizeX(long idx) const;
  virtual int SizeY(long idx) const;
  virtual int Map(long idx, int x, int y) const;
  virtual int AdvanceX(long idx) const;
  void gen_glyph_data(long idx);
private:
  GameApi::Env &e;
  std::string ttf_filename;
  std::string homepage;
  int sx,sy;
  mutable std::map<long, GlyphData*> glyph_data;
  void *priv_;
  void *priv;
  int state;
  long old_idx;
  mutable pthread_mutex_t mutex;
  std::string key;
};
