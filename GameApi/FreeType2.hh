
#include "Graph.hh"
#include <string>
#include <vector>

struct GlyphData;
class FontInterfaceImpl : public FontInterface
{
public:
  FontInterfaceImpl(GameApi::Env &e, void *priv_, std::string ttf_filename, std::string homepage, int sx, int sy);
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual int Left(long idx, bool is_outline=false) const;
  virtual int Top(long idx, bool is_outline=false) const;
  virtual int SizeX(long idx, bool is_outline=false) const;
  virtual int SizeY(long idx, bool is_outline=false) const;
  virtual unsigned int Map(long idx, int x, int y, bool is_outline=false) const;
  virtual int AdvanceX(long idx, bool is_outline=false) const;
  virtual int Ascender(long idx, bool is_outline=false) const;
  virtual int Descender(long idx, bool is_outline=false) const;
  virtual int Height(long idx, bool is_outline=false) const;
  void gen_glyph_data(long idx, bool is_outline=false);
  struct Bezier2d {
    Point2d p1,p2,p3;
  };
  virtual std::vector<Bezier2d> outlines(char idx) const;
  virtual std::vector<Bezier2d> make_monotonic(std::vector<Bezier2d> vec);
  std::vector<Bezier2d> gen_outline_data(long idx);
  struct OutlineEvent
  {
    float ymin;
    float ymax;
    int curve;
  };
  struct ScanLineContext
  {
    int curr;
  };
  struct ResPair
  {
    double x;
    int winding;
  };

  virtual std::vector<OutlineEvent> convert_to_events(std::vector<Bezier2d> vec);
  ScanLineContext ScanLineContextCreate() const;
  void ScanLineStep(std::vector<int> &active, const std::vector<OutlineEvent> &events, float y, ScanLineContext &ctx, long idx) const;
  std::vector<ResPair> ScanLineXCoord(const std::vector<int> &active, const std::vector<OutlineEvent> &events, const std::vector<Bezier2d> &bezi, float y, long idx) const;
  int IsPixelInside(const std::vector<ResPair> &vec, float x, long idx) const;
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
  mutable pthread_mutex_t outline_mutex;
  std::string key;
  mutable std::map<long, GlyphData*> *data2=0;
};
