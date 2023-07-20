 
#include "GameApi_h.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "GameApi_low.hh"
#include <iomanip>
#include <fstream>

template<class T>
void ArrayDelete(T *ptr)
{
  delete [] ptr;
}

struct BitmapPriv
{
};
struct AnimPriv
{
};



EXPORT GameApi::BitmapApi::BitmapApi(Env &e) : e(e) 
{
  priv = (void*)new BitmapPriv;
}
EXPORT GameApi::BitmapApi::~BitmapApi()
{
  delete (BitmapPriv*)priv;
}
EXPORT GameApi::AnimApi::AnimApi(Env &e) : e(e) 
{
  priv = (void*)new AnimPriv;
}
EXPORT GameApi::AnimApi::~AnimApi()
{
  delete (AnimPriv*)priv;
}

class ColorMap : public Function<bool,Color>
{
public:
  Color Index(bool b) const
  {
    if (!b) return Color(255,0,0,0);
    return Color(255,255,255,255);
  }
};
class ColorMap2 : public Function<bool,Pos>
{
public:
  Pos Index(bool b) const
  {
    Pos p1 = { 0,0 };
    Pos p2 = { 0,1 };
    if (!b) return p1;
    return p2;
  }
};
#if 0
class AlphaColorBitmap : public Bitmap<Color>
{
public:
  AlphaColorBitmap(Bitmap<Color> &bm, unsigned int key) : bm(bm), key(key) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    unsigned int cc = c.Pixel();
    if (cc == key) cc = 0x00000000;
    return Color(cc);
  }

private:
  Bitmap<Color> &bm;
  unsigned int key;
};
#endif
EXPORT GameApi::BMA GameApi::BitmapApi::empty_array()
{
  return array(NULL, 0);
}
#if 0
class BitmapArrayImpl : public BitmapArray2<Color>
{
public:
  BitmapArrayImpl(GameApi::BitmapApi *bmapi, GameApi::BM *array, int size) : bmapi(bmapi),
    m_size(size) 
  {
    if (size > 0)
      {
	m_array = new GameApi::BM[size];
	for(int i=0;i<size;i++)
	  {
	    m_array[i] = array[i];
	  }
      }
    else
      {
	m_array = 0;
      }
  }
  virtual int Size() const { return m_size; }
  virtual int SizeX(int i) const { return bmapi->size_x(m_array[i]); }
  virtual int SizeY(int i) const { return bmapi->size_y(m_array[i]); }
  virtual Color Map(int i, int x, int y) const
  {
    return bmapi->colorvalue(m_array[i], x, y);
  }

  ~BitmapArrayImpl() { delete [] m_array; }
private:
  GameApi::BitmapApi *bmapi;
  GameApi::BM *m_array;
  int m_size;
};
#endif
GameApi::BMA GameApi::BitmapApi::array(BM *array, int size)
{
#if 0
  return add_bitmap_array(e, new BitmapArrayImpl(this, array, size));
#endif
}

#if 0
class BitmapArrayElem : public Bitmap<Color>
{
public:
  BitmapArrayElem(BitmapArray2<Color> *arr, int i) : arr(arr), i(i) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }

  virtual int SizeX() const { return arr->SizeX(i); }
  virtual int SizeY() const { return arr->SizeY(i); }
  virtual Color Map(int x, int y) const
  {
    return arr->Map(i, x, y);
  }

private:
  BitmapArray2<Color> *arr;
  int i;
};
#endif
EXPORT GameApi::BM GameApi::BitmapApi::scale_bitmap(EveryApi &ev, BM orig, int sx, int sy)
{
  CBM cbm = ev.cont_bitmap_api.from_bitmap(orig, 1.0, 1.0);
  BM ret = ev.cont_bitmap_api.to_bitmap(cbm, sx,sy);
  return ret;
}
EXPORT GameApi::BM GameApi::BitmapApi::scale_bitmap_fullscreen(EveryApi &ev, BM orig)
{
  int sx = ev.mainloop_api.get_screen_sx();
  int sy = ev.mainloop_api.get_screen_sy();
  CBM cbm = ev.cont_bitmap_api.from_bitmap(orig, 1.0, 1.0);
  BM ret = ev.cont_bitmap_api.to_bitmap(cbm, sx,sy);
  return ret;
}

EXPORT GameApi::BM GameApi::BitmapApi::array_elem(BMA array, int i)
{
#if 0
  BitmapArray2<Color> *arr = find_bitmap_array(e, array);
  return add_color_bitmap2(e, new BitmapArrayElem(arr, i));
#endif
}

EXPORT GameApi::BM GameApi::BitmapApi::alpha_color(BM orig, unsigned int color_key)
{
#if 0
  BitmapHandle *handle = find_bitmap(e, orig);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b = new AlphaColorBitmap(*b2, color_key);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
#endif
}
class ColorRangeBitmap : public Bitmap<Color>
{
public:
  ColorRangeBitmap(Bitmap<Color> &bm, unsigned int source_upper, unsigned int source_lower, unsigned int target_upper, unsigned int target_lower) : bm(bm), source_upper(source_upper), source_lower(source_lower), target_upper(target_upper), target_lower(target_lower) { }
  void Collect(CollectVisitor &vis) {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const 
  {
    Color c = bm.Map(x,y);
    unsigned int c2 = c.Pixel();
    unsigned int c3 = Color::RangeChange(c2, source_upper, source_lower, target_upper, target_lower);
    return Color(c3);
  }

private:
  Bitmap<Color> &bm;
  unsigned int source_upper, source_lower;
  unsigned int target_upper, target_lower;
};
EXPORT GameApi::BM GameApi::BitmapApi::color_range(BM orig, unsigned int source_upper, unsigned int source_lower, unsigned int target_upper, unsigned int target_lower)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b = new ColorRangeBitmap(*b2, source_upper, source_lower, target_upper, target_lower);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;

}
class GradientBitmap2 : public Bitmap<Color>
{
public:
  GradientBitmap2(Point2d pos_1, Point2d pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy) : pos_1(pos_1), pos_2(pos_2), color_1(color_1), color_2(color_2), sx(sx), sy(sy) { 
    u_x = Point(pos_2.x, pos_2.y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    v = u_x.Dist();
    v = v*v;
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    //std::cout << "gradient" << std::endl;
    Vector u = Point(x, y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    float val = Vector::DotProduct(u, u_x);
    val /= v;
    //val /= v; 
    // now [0.0 .. 1.0]
    if (val<0.0) return 0;
    if (val>1.0) return 0;
    return Color(Color::CubicInterpolate(color_1, color_2, val));
  }

private:
  Point2d pos_1;
  Point2d pos_2;
  unsigned int color_1;
  unsigned int color_2;
  int sx,sy;
  Vector u_x;
  float v;
};
EXPORT GameApi::BM GameApi::BitmapApi::gradient(PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy)
{
  Point *pos_1a = find_point(e, pos_1);
  Point *pos_2a = find_point(e, pos_2);
  Point2d pos_1b = { pos_1a->x, pos_1a->y };
  Point2d pos_2b = { pos_2a->x, pos_2a->y };
  return add_color_bitmap2(e, new GradientBitmap2(pos_1b, pos_2b, color_1, color_2, sx,sy));
}

#if 0
class RadialGradient : public Bitmap<Color>
{
public:
  RadialGradient(int sx, int sy, Point2d pos, float r1, float r2, unsigned int color_1, unsigned int color_2) : sx(sx),sy(sy), pos(pos), r1(r1), r2(r2), color_1(color_1), color_2(color_2) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }

  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx-=pos.x;
    yy-=pos.y;
    float dist = sqrt(xx*xx+yy*yy);
    dist -= r1;
    dist /= (r2-r1);
    // now [0..1]
    if (dist<0.0) return 0;
    if (dist>1.0) return 0;
    return Color(Color::CubicInterpolate(color_1, color_2, dist));
  }
private:
  int sx,sy;
  Point2d pos;
  float r1,r2;
  unsigned int color_1, color_2;
};
#endif

EXPORT GameApi::BM GameApi::BitmapApi::radial_gradient(int sx, int sy, PT pos, float r1, float r2, unsigned int color_1, unsigned int color_2)
{
#if 0
  Point *pos_p1 = find_point(e, pos);
  Point2d pos_p = { pos_p1->x, pos_p1->y };
   ::Bitmap<Color> *b = new RadialGradient(sx,sy,pos_p, r1,r2, color_1, color_2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
  return bm;
#endif
}

template<class T>
class NewBitmap_SZ : public Bitmap<Color>
{
public:
  NewBitmap_SZ(Bitmap<T> &next, unsigned int color) : next(next), color(color) { }
  void Collect(CollectVisitor &vis) { next.Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { next.Prepare(); }
  int SizeX() const { return next.SizeX(); }
  int SizeY() const { return next.SizeY(); }
  Color Map(int x, int y) const { return Color(color); }
private:
  Bitmap<T> &next;
  unsigned int color;
};

EXPORT GameApi::BM GameApi::BitmapApi::newbitmap_bm(BM bm, unsigned int color)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  Bitmap<Color> *b = new NewBitmap_SZ<::Color>(*b2,color);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}
EXPORT GameApi::BM GameApi::BitmapApi::newbitmap_fb(FB fb, unsigned int color)
{
  Bitmap<float> *b2 = find_float_bitmap(e,fb)->bitmap;
  Bitmap<Color> *b = new NewBitmap_SZ<float>(*b2,color);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}
EXPORT GameApi::BM GameApi::BitmapApi::newbitmap_bb(BB bb, unsigned int color)
{
  Bitmap<bool> *b2 = find_bool_bitmap(e,bb)->bitmap;
  Bitmap<Color> *b = new NewBitmap_SZ<bool>(*b2,color);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}

EXPORT GameApi::BM GameApi::BitmapApi::newbitmap(int sx, int sy, unsigned int color)
{
  ::Bitmap<Color> *b = new ConstantBitmap_Color(Color(color), sx,sy);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

template<class T>
class BitmapTransformFromFunction : public Bitmap<T>
{
public:
  BitmapTransformFromFunction(Bitmap<T> &bm, std::function<T (int,int,T)> f) : bm(bm),  f(f) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual T Map(int x, int y) const
  {
    return f(x,y,bm.Map(x,y));
  }
private:
  Bitmap<T> &bm;
  std::function<T (int,int,T)> f; 
};

template<class T>
class BitmapFromFunction : public Bitmap<T>
{
public:
  BitmapFromFunction(std::function< T(int,int) > f, int sx, int sy) : f(f), sx(sx),sy(sy) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }

  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    return f(x,y);
  }
private:
  std::function<T (int,int)> f;
  int sx,sy;
};


EXPORT GameApi::BM GameApi::BitmapApi::transform(BM orig, std::function<unsigned int (int,int, unsigned int)> f)
{
#if 0
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BitmapHandle *handle = find_bitmap(e, orig);
  Bitmap<Color> *c = find_color_bitmap(handle);
  UnsignedIntFromBitmap *bm1 = new UnsignedIntFromBitmap(*c);
  env->deletes.push_back(std::shared_ptr<void>(bm1));
  BitmapTransformFromFunction<unsigned int> *trans = new BitmapTransformFromFunction<unsigned int>(*bm1, f);
  env->deletes.push_back(std::shared_ptr<void>(trans));

  BitmapFromUnsignedInt *bm2 = new BitmapFromUnsignedInt(*trans);
  return add_color_bitmap(e, bm2);
#endif
}
EXPORT GameApi::BM GameApi::BitmapApi::function(std::function<unsigned(int,int)> f, int sx, int sy)
{
#if 0
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<unsigned int> *bm = new BitmapFromFunction<unsigned int>(f,sx,sy);  
  return add_color_bitmap(e, new BitmapFromUnsignedInt(*bm));
#endif
}

EXPORT GameApi::BM GameApi::BitmapApi::memoize(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  return add_color_bitmap(e, new MemoizeBitmap(*bitmap));
}

EXPORT GameApi::BM GameApi::BitmapApi::memoize_all(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  MemoizeBitmap *membitmap = new MemoizeBitmap(*bitmap);
  membitmap->MemoizeAll();
  return add_color_bitmap(e, membitmap);
}

class ChessBoardBitmap2 : public Bitmap<Color>
{
public:
  ChessBoardBitmap2(int tile_sx, int tile_sy, int count_x, int count_y, Color c1, Color c2) : tile_sx(tile_sx), tile_sy(tile_sy), count_x(count_x), count_y(count_y), c1(c1), c2(c2) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }

  int SizeX() const { return tile_sx*count_x; }
  int SizeY() const { return tile_sy*count_y; }
  Color Map(int x, int y) const
  {
    bool bb = false;
    int xx = x / count_x;
    int yy = y / count_y;
    if (xx & 1) { bb = !bb; }
    if (yy & 1) { bb = !bb; }
    if (bb) { return c1; }
    return c2;
  }
private:
  int tile_sx, tile_sy;
  int count_x, count_y;
  Color c1,c2;
};
EXPORT GameApi::BM GameApi::BitmapApi::chessboard(int tile_sx, int tile_sy, int count_x, int count_y, unsigned int c1, unsigned int c2)
{
  Bitmap<Color> *m = new ChessBoardBitmap2(tile_sx, tile_sy, count_x, count_y, Color(c1), Color(c2));
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = m;
  BM bm = add_bitmap(e, handle);
  return bm;
}
EXPORT GameApi::BM GameApi::BitmapApi::mandelbrot(bool julia,
		float start_x, float end_x,
		float start_y, float end_y,
		float xx, float yy,
		int sx, int sy,
		int count)
{
  ::Bitmap<int> *b = new Mandelbrot(julia, start_x, end_x,
				      start_y, end_y,
				      sx,sy,
				      count,
				      xx,yy);
  ::Bitmap<Color> *b2 = new MapBitmapToColor(0, count, Color(255,255,255), Color(0,128,255), *b);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  env->deletes.push_back(std::shared_ptr<void>(b));
  env->deletes.push_back(std::shared_ptr<void>(b2));
  ::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = m; 
  BM bm = add_bitmap(e, handle);
  return bm;  
}

EXPORT GameApi::BM GameApi::BitmapApi::mandelbrot2(bool julia,
		float start_x, float end_x,
		float start_y, float end_y,
		float xx, float yy,
		int sx, int sy,
		int count)
{
  ::Bitmap<int> *b = new Mandelbrot(julia, start_x, end_x,
				      start_y, end_y,
				      sx,sy,
				      count,
				      xx,yy);
  ::Bitmap<Color> *b2 = new MapBitmapToColor(0, count, Color(255,255,255), Color(0,128,255), *b);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  env->deletes.push_back(std::shared_ptr<void>(b));
  //env->deletes.push_back(std::shared_ptr<void>(b2));
  //::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b2;
  BM bm = add_bitmap(e, handle);
  return bm;  
}

EXPORT GameApi::BM GameApi::BitmapApi::newtilebitmap(int sx, int sy, int tile_sx, int tile_sy)
{
  ::Bitmap<Color> *b = new ConstantBitmap_Color(Color(0,0,0), sx,sy);
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = b;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

BufferRef LoadImage(std::string filename, bool &success);

EXPORT void GameApi::BitmapApi::savebitmap(GameApi::BM bm, std::string filename, bool alpha)
{  
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);
  PpmFile file(filename, *bm2, alpha);
  std::string pngcontents = file.Contents();
  std::ofstream filehandle(filename.c_str(), std::ios_base::out|std::ios::binary);
  filehandle.write(pngcontents.c_str(), pngcontents.size()); // << pngcontents;
  filehandle.close();
}
class SaveBitmapML : public MainLoopItem
{
public:
  SaveBitmapML(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::BM bm, std::string filename, bool alpha, float time) : env(env), ev(ev), bm(bm), filename(filename), alpha(alpha), time(time) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (time > e.time*10.0 && time < e.time*10.0 + e.delta_time)
      {
	const char *dd = getenv("BUILDER_DOCKER_DIR");
	std::string dockerdir = dd?dd:"";
	std::string home = getenv("HOME");
	home+="/";
	if (dockerdir!="") home=dockerdir;
	std::string path = home + ".gameapi_builder/";
	ev.bitmap_api.savebitmap(bm, path+filename, alpha);
	
	std::ifstream ss((path+filename).c_str());
        char ch;
	std::vector<unsigned char> vec;
	while(ss.get(ch)) { vec.push_back(ch); }
	ss.close();
	int bar = env.add_to_download_bar(filename);
	int ii = env.download_index_mapping(bar);
	env.set_download_data(ii, vec);
	env.set_download_ready(ii);
      }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::BM bm;
  std::string filename;
  bool alpha;
  float time;
};
EXPORT GameApi::ML GameApi::BitmapApi::savebitmap_ml(EveryApi &ev, BM bm, std::string filename, bool alpha, float time)
{
  return add_main_loop(e, new SaveBitmapML(e,ev, bm, filename, alpha, time));
}


class SavePngArray : public MainLoopItem
{
public:
  SavePngArray(GameApi::EveryApi &ev, std::vector<GameApi::BM> bms, std::string filename_start, std::string filename_end, bool alpha, float time) : ev(ev), bms(bms), filename_start(filename_start), filename_end(filename_end), alpha(alpha), time(time) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (time > e.time*10.0 && time < e.time*10.0 + e.delta_time)
      {
	int s = bms.size();
	for(int i=0;i<s;i++)
	  {
	    std::stringstream ss;
	    ss << filename_start << std::setw(4) << std::setfill('0') << i << filename_end;
	    //ev.bitmap_api.savebitmap(bms[i], ss.str(), alpha);
	    ev.bitmap_api.save_png(bms[i], ss.str());
	  }
      }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> bms;
  std::string filename_start, filename_end;
  bool alpha;
  float time;
};


EXPORT GameApi::ML GameApi::BitmapApi::savepng_array_ml(EveryApi &ev, std::vector<BM> bms, std::string filename_start, std::string filename_end, bool alpha, float time)
{
  return add_main_loop(e, new SavePngArray(ev,bms, filename_start, filename_end, alpha, time));
}

extern std::vector<const unsigned char*> g_content;
extern std::vector<const unsigned char*> g_content_end;
extern std::vector<const char*> g_urls;

void GameApi::append_url_map(const char* url,const unsigned char* data, const unsigned char *data_end)
{
  g_urls.push_back(url);
  g_content.push_back(data);
  g_content_end.push_back(data_end);
}
std::string stripprefix(std::string s);
std::string remove_load(std::string s);

std::vector<unsigned char> *load_from_url(std::string url);
extern std::map<std::string, std::vector<unsigned char>*> load_url_buffers;

#ifdef LINUX
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

void stackTrace()
{
#ifdef LINUX
#define BT_BUF_SIZE 100
  int j,nptrs;
  void *buffer[BT_BUF_SIZE];
  char **strings;
  nptrs = backtrace(buffer,BT_BUF_SIZE);
  printf("backtrace returned %d addresses\n",nptrs);
  strings = backtrace_symbols(buffer,nptrs);
  for(j=0;j<nptrs;j++) {
    printf("%s\n", strings[j]);
  }
  free(strings);
#endif
  
#ifdef EMSCRIPTEN
  EM_ASM({
      stackTrace();
    });
  // emscripten_run_script("stackTrace()");
#endif
}
void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max=15);

extern std::string gameapi_homepageurl;

//void del_bitmap_cache(void* ptr);
//int register_cache_deleter(void (*fptr)(void*), void*data);
//void unregister_cache_deleter(int id);


//void del_bitmap_cache(void* ptr)
// {
//    LoadBitmapFromUrl *p = (LoadBitmapFromUrl*)ptr;
//    p->del_cache();
//  }


#if 0
class LoadBitmapFromUrl : public Bitmap<Color>
{
public:
  LoadBitmapFromUrl(std::string url) : url(url) { }

  virtual int SizeX() const { 
    if (!cbm) { return 100; }

    return cbm->SizeX(); }
  virtual int SizeY() const {
    if (!cbm) { return 100; } 
    return cbm->SizeY(); }
  virtual Color Map(int x, int y) const { 
    if (!cbm) { return Color(0xffffffff); }
    return cbm->Map(x,y); }
  void LoadFinished(std::vector<unsigned char> *vec)
  {
    if (load_finished) return;
    bool failed = false;
    if (vec==(std::vector<unsigned char>*)-1) {
      std::cout << "URL " << url << " loading failed!" << std::endl;
      failed = true;
    }
    if (vec==0) {
      std::cout << "URL " << url << " loading timeouted!" << std::endl;
      failed = true;
    }
    if (failed)
      {
	img = BufferRef::NewBuffer(10,10);
	for(int x=0;x<10;x++)
	  for(int y=0;y<10;y++)
	    {
	      img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	    }
	cbm = new BitmapFromBuffer(img);    
	return;
      }
    //std::cout << "URL " << url << " loading succeeded!" << std::endl;

    bool b = false;
    img = LoadImageFromString(*vec, b);

    if (b==false) {
      img = BufferRef::NewBuffer(10,10);
      for(int x=0;x<10;x++)
	for(int y=0;y<10;y++)
	  {
	    img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	  }
      //std::cout << "ERROR: File not found: " << filename << std::endl;
    }
    cbm = new BitmapFromBuffer(img);    
    load_finished = true;


  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url);
  }

  }
  virtual void Prepare() {
#ifndef EMSCRIPTEN
  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url,15);
  }

    std::vector<unsigned char> *data = load_from_url(url);

    bool b = false;
    img = LoadImageFromString(*data, b);
    delete data;
    if (b==false) {
      img = BufferRef::NewBuffer(10,10);
      for(int x=0;x<10;x++)
	for(int y=0;y<10;y++)
	  {
	    img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	  }
      //std::cout << "ERROR: File not found: " << filename << std::endl;
    }
    cbm = new BitmapFromBuffer(img);
  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url);
  }

#else
    std::string url2 = "load_url.php?url=" + url;

    std::vector<unsigned char> *vec = load_url_buffers[url2];
    LoadFinished(vec);




#endif
    
  }
private:
  std::string url;
  BufferRef img;
  Bitmap<Color> *cbm=0;
  bool load_finished = false;
};

#endif

class LoadBitmapBitmap : public Bitmap<Color>
{
public:
  LoadBitmapBitmap(std::string filename) : filename(filename),cbm(0) { }
  virtual int SizeX() const { 
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at SizeX()" << std::endl; const_cast<LoadBitmapBitmap*>(this)->Prepare(); }

    return cbm->SizeX(); }
  virtual int SizeY() const {
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at SizeY()" << std::endl;  const_cast<LoadBitmapBitmap*>(this)->Prepare();} 
    return cbm->SizeY(); }
  virtual Color Map(int x, int y) const { 
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at Map()" << std::endl;  const_cast<LoadBitmapBitmap*>(this)->Prepare();}
    return cbm->Map(x,y); }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  virtual void Prepare()
  {
    if (cbm)
      {
	//std::cout << "Attempt to Prepare twice" << std::endl;
      }
    if (!cbm)
      {
	//std::cout << "Load Bitmap Prepare " << filename << std::endl;
    PpmFileReader *file = new PpmFileReader(filename);
    file->Prepare();
    if (file->status()==true)
      {
	cbm = file;
	return;
      }
    delete file;


    bool b = false;
    img = LoadImage(filename, b);
    if (b==false) {
      img = BufferRef::NewBuffer(10,10);
      for(int x=0;x<10;x++)
	for(int y=0;y<10;y++)
	  {
	    img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	  }
      std::cout << "ERROR: File not found: " << filename << std::endl;
    }
    //::EnvImpl *env = ::EnvImpl::Environment(&e);
    //env->deletes.push_back(std::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));
    
    cbm = new BitmapFromBuffer(img);
      }
  }
private:
  std::string filename;
  BufferRef img;
  Bitmap<Color> *cbm;
};

std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
int bitmap_find_data(std::string data) {
  int s = bitmap_prepare_cache_data.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string,int> p = bitmap_prepare_cache_data[i];
      if (p.first==data) return p.second;
    }
  return -1;
}

class BitmapPrepareCache : public Bitmap<Color>
{
public:
  BitmapPrepareCache(GameApi::Env &e, std::string id, Bitmap<Color> *bm) : e(e), id(id), bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void Prepare()
  {

    if (bitmap_find_data(id)!=-1) {


      return;
    }
    bm->Prepare();
        int c = get_current_block();
    set_current_block(-2); // dont take ownership of this
    GameApi::BM num = add_color_bitmap2(e, bm);
        set_current_block(c);
    bitmap_prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  Bitmap<Color> *get_bm() const
  {
    if (bm_cache) return bm_cache;
    int num = bitmap_find_data(id);
    if (num==-1) { const_cast<BitmapPrepareCache*>(this)->Prepare(); num=bitmap_find_data(id); }
    GameApi::BM bm2;
    bm2.id = num;
    BitmapHandle *handle = find_bitmap(e, bm2);
    Bitmap<Color> *bbm = find_color_bitmap(handle);
    bm_cache=bbm;
    return bbm;
  }
  virtual int SizeX() const { return get_bm()->SizeX(); }
  virtual int SizeY() const { return get_bm()->SizeY(); }
  virtual Color Map(int x, int y) const
  {
    return get_bm()->Map(x,y);
  }
  virtual bool IsDirectGltfImage() const { return get_bm()->IsDirectGltfImage(); }
private:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
  mutable Bitmap<Color> *bm_cache=0;
};
/* moved to GameApi_gltf.cc 
EXPORT GameApi::BM GameApi::BitmapApi::loadbitmapfromurl(std::string url)
{
  int c = get_current_block();
  set_current_block(-1);
  Bitmap<Color> *bm = new LoadBitmapFromUrl(e,url,gameapi_homepageurl);
  Bitmap<Color> *bbm = new BitmapPrepareCache(e, url, bm);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bbm;
  BM bm2 = add_bitmap(e, handle);
  set_current_block(c);
  return bm2;
  }*/
std::string MB(long num);

EXPORT GameApi::BM GameApi::BitmapApi::loadbitmap(std::string filename)
{
  Bitmap<Color> *bm = new LoadBitmapBitmap(filename);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  BM bm2 = add_bitmap(e, handle);
  return bm2;

  
#if 0
  // PPM FILE READING
  PpmFileReader *file = new PpmFileReader(filename);
  file->Prepare();
  if (file->status()==true)
    {
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = file;
      BM bm = add_bitmap(e, handle);
      return bm;
    }
  delete file;



  // OTHER FILES
  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  std::cout << "loadbitmap: " << img.width << "x" << img.height << "=" << ::MB(img.width*img.height*sizeof(unsigned int)) << std::endl;
  if (b==false) {
    img = BufferRef::NewBuffer(10,10);
    for(int x=0;x<10;x++)
      for(int y=0;y<10;y++)
	{
	  img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	}
    std::cout << "ERROR: File not found: " << filename << std::endl;
  }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));

  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = buf;
  BM bm = add_bitmap(e, handle);
  return bm;
#endif
}
EXPORT GameApi::BM GameApi::BitmapApi::loadtilebitmap(std::string filename, int sx, int sy)
{

  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  std::cout << "loadtilebitmap: " << img.width << "x" << img.height << "=" << ::MB(img.width*img.height*sizeof(unsigned int)) << std::endl;

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));
  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = buf;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
}


EXPORT GameApi::BM GameApi::BitmapApi::loadposbitmap(std::string filename)
{
  BitmapCircle *circle = new BitmapCircle(Point2d::NewPoint(30.0,30.0), 30.0, 60,60);
  ColorMap2 *f = new ColorMap2;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(f));
  env->deletes.push_back(std::shared_ptr<void>(circle));
  MapBitmap<bool, Pos> *map = new MapBitmap<bool,Pos>(*circle,*f);
  BitmapPosHandle *handle = new BitmapPosHandle;
  handle->bm = map;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

EXPORT GameApi::BM GameApi::BitmapApi::findtile(GameApi::BM tile_bitmap, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, tile_bitmap);
#ifdef EMSCRIPTEN
  BitmapTileHandle *chandle = static_cast<BitmapTileHandle*>(handle);
#else
  BitmapTileHandle *chandle = dynamic_cast<BitmapTileHandle*>(handle);
#endif
  int sx = chandle->tile_sx;
  int sy = chandle->tile_sy;
  return subbitmap(tile_bitmap, sx*x, sy*y, sx, sy);
}
template<class T>
Bitmap<T> *subbitmap_t(Bitmap<T> *bm, int x, int y, int width, int height)
{
  Bitmap<T> *sub = new SubBitmap<T>(*bm, x,y,width, height);
  return sub;
}

//template Bitmap<Color> *subbitmap_t(Bitmap<Color> *bm, int x, int y, int width, int height);

BitmapHandle *subbitmap_h(BitmapHandle *handle, int x, int y, int width, int height)
{
#ifdef EMSCRIPTEN
  BitmapColorHandle *chandle1 = static_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = 0; //dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = 0; //dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = 0; //dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = 0; //dynamic_cast<BitmapTileHandle*>(handle);
#else
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
#endif
  if (chandle1)
    { // color
      Bitmap<Color> *c = subbitmap_t(chandle1->bm, x,y,width,height);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle2)
    { // int
      Bitmap<int> *c = subbitmap_t(chandle2->bm, x,y,width,height);
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = subbitmap_h(h, x,y,width,height);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }
  if (chandle4)
    { // pos
      Bitmap<Pos> *c = subbitmap_t(chandle4->bm, x,y,width,height);
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle5)
    { // tile
      Bitmap<Color> *c = subbitmap_t(chandle5->bm, x,y,width,height);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  std::cout << "subbitmap" << std::endl;
  return 0;
}

GameApi::BM GameApi::BitmapApi::subbitmap(GameApi::BM orig, int x, int y, int width, int height)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = subbitmap_h(handle, x,y,width,height);
  BM bm = add_bitmap3(e, handle2);
  return bm;

}
template<class T>
Bitmap<T> *growbitmap_t(Bitmap<T> *bm, int l, int t, int r, int b,
			T tt)
{
  Bitmap<T> *grow = new GrowBitmap<T>(*bm, l,t,r,b, tt);
  return grow;
} 
BitmapHandle *growbitmap_h(BitmapHandle *handle, int l, int t, int r, int b)
{
#ifdef EMSCRIPTEN
  BitmapColorHandle *chandle1 = static_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = 0; //dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = 0; //dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = 0; //dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = 0; //dynamic_cast<BitmapTileHandle*>(handle);
#else
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
#endif
  if (chandle1)
    { // color
      Color elem(0,0,0,0);
      Color *c = (Color*)&elem;
      Bitmap<Color> *cbm = growbitmap_t(chandle1->bm, l,t,r,b,*c);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle2)
    { // int
      int elem = 0;
      int *c = (int*)&elem;
      Bitmap<int> *cbm = growbitmap_t(chandle2->bm, l,t,r,b,*c);
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = growbitmap_h(h, l,t,r,b);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }
  if (chandle4)
    { // pos
      Pos elem = { 0,0 };
      Pos *c = (Pos*)&elem;
      Bitmap<Pos> *cbm = growbitmap_t(chandle4->bm, l,t,r,b,*c);
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle5)
    { // tile
      Color elem(0,0,0,0);
      Color *c = (Color*)&elem;
      Bitmap<Color> *cbm = growbitmap_t(chandle5->bm, l,t,r,b,*c);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = cbm;
      return handle;
    }
  std::cout << "growbitmap" << std::endl;
  return 0;
}

EXPORT GameApi::BM GameApi::BitmapApi::growbitmap(GameApi::BM orig, int l, int t, int r, int b)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = growbitmap_h(handle, l,t,r,b);
  BM bm = add_bitmap(e, handle2);
  return bm;

}

BitmapHandle *modify_bitmap_h(BitmapHandle *orig, BitmapHandle *bm, int x, int y)
{
#ifdef EMSCRIPTEN
  BitmapColorHandle *chandle1 = static_cast<BitmapColorHandle*>(orig);
  BitmapIntHandle *chandle2 = 0; //dynamic_cast<BitmapIntHandle*>(orig);
  BitmapArrayHandle *chandle3 = 0; //dynamic_cast<BitmapArrayHandle*>(orig);
  BitmapPosHandle *chandle4 = 0; //dynamic_cast<BitmapPosHandle*>(orig);
#else
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(orig);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(orig);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(orig);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(orig);
#endif
  //BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle1)
    { // color
      BitmapColorHandle *ahandle1 = static_cast<BitmapColorHandle*>(bm);
      Bitmap<Color> *c = new ModifyBitmap<Color>(*chandle1->bm, *ahandle1->bm, x,y); 
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle2)
    { // color
      BitmapIntHandle *ahandle1 = static_cast<BitmapIntHandle*>(bm);
      Bitmap<int> *c = new ModifyBitmap<int>(*chandle2->bm, *ahandle1->bm, x,y); 
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = modify_bitmap_h(h, bm, x,y);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }

  if (chandle4)
    { // pos
      BitmapPosHandle *ahandle1 = static_cast<BitmapPosHandle*>(bm);
      Bitmap<Pos> *c = new ModifyBitmap<Pos>(*chandle4->bm, *ahandle1->bm, x,y); 
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = c;
      return handle;
    }
  std::cout << "modify_bitmap_h" << std::endl;
  return 0;
}

EXPORT GameApi::BM GameApi::BitmapApi::modify_bitmap(GameApi::BM orig, BM bm, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = find_bitmap(e, bm);
  BitmapHandle *h = modify_bitmap_h(handle, handle2, x,y);
  BM bbm = add_bitmap(e,h);
  return bbm;
}
EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  return add_color_bitmap(e, new BlitBitmapClass(*bm1, *bm2, x,y));
}

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap_fb(BM bg, BM orig, int x, int y, FB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  FloatBitmap *handle3 = find_float_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<float> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked(*bm1, *bm2, x,y, *bm3));
}

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap_bb(BM bg, BM orig, int x, int y, BB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  BoolBitmap *handle3 = find_bool_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<bool> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked2(*bm1, *bm2, x,y, *bm3));
}

EXPORT int GameApi::BitmapApi::intvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapIntHandle *handle2 = static_cast<BitmapIntHandle*>(handle);
  if (handle2)
    {
      return handle2->bm->Map(x,y);
    }
  return 0;
}

EXPORT int GameApi::BitmapApi::size_x(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
#ifdef EMSCRIPTEN
  BitmapColorHandle *handle2 = static_cast<BitmapColorHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeX();
  return 0;
#else

  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *handle3 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *handle4 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *handle5 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *handle6 = dynamic_cast<BitmapTileHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeX();
  if (handle3)
    return handle3->bm->SizeX();
  if (handle4)
    return dynamic_cast<BitmapColorHandle*>(handle4->vec[0])->bm->SizeX();
  if (handle5)
    return handle5->bm->SizeX();
  if (handle6)
    return handle6->bm->SizeX();
  return 0;
#endif
}
EXPORT int GameApi::BitmapApi::size_y(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
#ifdef EMSCRIPTEN
  BitmapColorHandle *handle2 = static_cast<BitmapColorHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeY();
  return 0;
#else



  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *handle3 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *handle4 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *handle5 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *handle6 = dynamic_cast<BitmapTileHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeY();
  if (handle3)
    return handle3->bm->SizeY();
  if (handle4)
    return dynamic_cast<BitmapColorHandle*>(handle4->vec[0])->bm->SizeY();
  if (handle5)
    return handle5->bm->SizeY();
  if (handle6)
    return handle6->bm->SizeY();
  return 0;
#endif
}

EXPORT unsigned int GameApi::BitmapApi::colorvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
#ifdef EMSCRIPTEN
  BitmapColorHandle *handle2 = static_cast<BitmapColorHandle*>(handle);
#else
  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
#endif
  if (handle2)
    {
      return handle2->bm->Map(x,y).Pixel();
    }
  return 0;
}


EXPORT GameApi::BM GameApi::BitmapApi::interpolate_bitmap(GameApi::BM orig1, GameApi::BM orig2, float x)
{
  BitmapHandle *handle1 = find_bitmap(e, orig1);
  BitmapHandle *handle2 = find_bitmap(e, orig2);
  BitmapColorHandle *newhandle = new BitmapColorHandle;
#ifdef EMSCRIPTEN
  BitmapColorHandle *h1 = static_cast<BitmapColorHandle*>(handle1);
  BitmapColorHandle *h2 = static_cast<BitmapColorHandle*>(handle2);
#else
  BitmapColorHandle *h1 = dynamic_cast<BitmapColorHandle*>(handle1);
  BitmapColorHandle *h2 = dynamic_cast<BitmapColorHandle*>(handle2);
#endif
  newhandle->bm = 0;
  if (h1&&h2)
    {
      newhandle->bm = new InterpolateBitmap(*h1->bm, *h2->bm, x);
    }
  else
    {
      std::cout << "Only COlorbitmaps work" << std::endl;
    }
  BM bbm = add_bitmap(e,newhandle);
  return bbm;
}

template<class T>
class BitmapFromString : public Bitmap<T>
{
public:
  BitmapFromString(char *array, int sx, int sy, std::function<T (char)> f) : array(array), sx(sx), sy(sy),  f(f) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  T Map(int x, int y) const
  {
    return f(array[x+y*sx]);
  }
private:
  char *array;
  int sx;
  int sy;
  std::function<T (char)> f;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::newfloatbitmap(char *array, int sx, int sy, std::function<float(char ch)> f)
{
  return add_float_bitmap(e, new BitmapFromString<float>(array, sx,sy,f));
}
EXPORT GameApi::BM GameApi::BitmapApi::newintbitmap(char *array, int sx, int sy, std::function<int (char ch)> f )
{
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  BitmapIntHandle *handle = new BitmapIntHandle;
  handle->bm = new BitmapFromString<int>(array, sx, sy, f);
  return add_bitmap(e, handle);
}

EXPORT GameApi::BM GameApi::BitmapApi::newcolorbitmap(char *array, int sx, int sy, std::function<unsigned int (char)> f)
{
#if 0
  //EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  BitmapColorHandle *handle = new BitmapColorHandle;
  ::Bitmap<unsigned int> *bm = new BitmapFromString<unsigned int>(array, sx, sy, f);
  env->deletes.push_back(std::shared_ptr<void>(bm));
  handle->bm = new BitmapFromUnsignedInt(*bm);
  return add_bitmap(e, handle);
#endif
}



EXPORT GameApi::BM GameApi::BitmapApi::anim_array(GameApi::BM *bitmaparray, int size)
{
  int s = size;
  BitmapArrayHandle *handle = new BitmapArrayHandle;
  for(int i=0;i<s;i++)
    {
      GameApi::BM bm = bitmaparray[i];
      BitmapHandle *h = find_bitmap(e, bm);
      //std::cout << "bitmaparray: " << h << std::endl;
      handle->vec.push_back(h);
      handle->owned.push_back(0);

    }
  BM bm = add_bitmap(e, handle);
  return bm;
}

#if 0
EXPORT GameApi::BM GameApi::BitmapApi::bitmapandtypes(BM bm, BM (*fptr)(GameApi::EveryApi &ev,int))
{
  BitmapHandle *h = find_bitmap(e, bm);
  BitmapIntHandle *hh = dynamic_cast<BitmapIntHandle*>(h);
  if (!hh) { std::cout << "bm for bitmapandtypes() is not bitmap of integers" << std::endl; return; }
  
}
#endif
EXPORT GameApi::IS GameApi::AnimApi::single(int val, float duration)
{
  AnimImpl i;
  //i.wave_int = new SingleAnimInt(val, duration);
  //i.wave_point = 0;
  //i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

class Rot90Bitmap : public Bitmap<Color>
{
public:
  Rot90Bitmap(Bitmap<Color> &bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }
  virtual int SizeX() const { return bm.SizeY(); }
  virtual int SizeY() const { return bm.SizeX(); }
  virtual Color Map(int x, int y) const
  {
    int yy = x;
    int xx = SizeY()-y-1;
    return bm.Map(xx,yy);
  }
private:
  Bitmap<Color> &bm;
};

class FlipBitmap : public Bitmap<Color>
{
public:
  FlipBitmap(Bitmap<Color> &bm, bool x, bool y) : bm(bm), flip_x(x), flip_y(y) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    int xx = x;
    int yy = y;
    if (flip_x) {
      xx = SizeX()-x-1;
    }
    if (flip_y) {
      yy = SizeY()-y-1;
    }
    return bm.Map(xx,yy);
  }

private:
  Bitmap<Color> &bm;
  bool flip_x, flip_y;
};
#if 0
class DupXBitmap : public Bitmap<Color>
{
public:
  DupXBitmap(Bitmap<Color> &orig) : bm(orig) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX()*2; }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    x/=2;
    return bm.Map(x,y);
  }
private:
  Bitmap<Color> &bm;
};
#endif
EXPORT GameApi::BM GameApi::BitmapApi::dup_x(BM orig)
{
#if 0
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new DupXBitmap(*chandle->bm);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
#endif
}

EXPORT GameApi::BM GameApi::BitmapApi::flip_x(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, true, false);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::flip_y(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, false, true);

  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::rot90(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new Rot90Bitmap(*chandle->bm);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e, chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::repeat_bitmap(BM orig, int xcount, int ycount)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
  if (!chandle) return add_bitmap(e,0);
  RepeatBitmap<Color> *rep = new RepeatBitmap<Color>(*chandle->bm, xcount, ycount);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
int index_from_string(char c, std::string s);
EXPORT GameApi::BM GameApi::BitmapApi::world_from_bitmap2(EveryApi &ev, 
							  std::vector<BM> v,
							  std::string filename,
							  std::string chars,
							  int dx, int dy,
							  int ssx, int ssy)
{
  if (v.size()==0) return newbitmap(1,1);
  char *array = new char[ssx*ssy];
  std::ifstream ss(filename.c_str());
  for(int i=0;i<ssx*ssy;i++)
    {
      ss >> array[i];
    }
  BM int_bm = ev.bitmap_api.newintbitmap(array, ssx, ssy, [&chars](char c) { return index_from_string(c,chars); });
  int sx = size_x(int_bm);
  int sy = size_y(int_bm);
  BM current = newbitmap(sx*dx,sy*dy);
  for(int y=0;y<sy;y++)
    {
      for(int x=0;x<sx;x++)
	{
	  int index = intvalue(int_bm, x,y);
	  int s = v.size();
	  if (index>=s) index=0;
	  if (index<0) index=0;
	  BM bm = v[index];
	  current = blitbitmap(current, bm, x*dx, y*dy);
	}
    }
  return current;

}
EXPORT GameApi::BM GameApi::BitmapApi::world_from_bitmap(std::function<BM(int)> f, BM int_bm, int dx, int dy)
{
  int sx = size_x(int_bm);
  int sy = size_y(int_bm);
  BM current = newbitmap(sx*dx,sy*dy);
  for(int y=0;y<sy;y++)
    {
      for(int x=0;x<sx;x++)
	{
	  int index = intvalue(int_bm, x,y);
	  BM bm = f(index);
	  current = blitbitmap(current, bm, x*dx, y*dy);
	}
    }
  return current;
}
class WorldFromUrl : public Bitmap<Color>
{
public:
  WorldFromUrl(GameApi::Env &e, GameApi::EveryApi &ev, std::vector<GameApi::BM> v, std::string url, std::string homepage, std::string chars, int dx, int dy) : e(e), ev(ev), v(v), url(url), homepage(homepage), chars(chars), dx(dx), dy(dy), ssx(1), ssy(1) { mymap = 0; }
  virtual int SizeX() const { return ssx*dx; }
  virtual int SizeY() const { return ssy*dy; }
  virtual Color Map(int x, int y) const
  {
    int xx = x/dx;
    int yy = y/dy;
    int xxx = x-xx*dx;
    int yyy = y-yy*dy;
    
    int pos = mymap[xx+yy*ssx];
    int ps = v.size();
    GameApi::BM bm;
    if (pos>=0 && pos<ps)
      {
	bm = v[pos];
      }
    else
      {
	bm = ev.bitmap_api.newbitmap(dx,dy,0xffffff00);
      }
    BitmapHandle *handle = find_bitmap(e, bm);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    int sx = b2->SizeX();
    int sy = b2->SizeY();
    if (xxx>=0 && xxx<sx && yyy>=0 && yyy<sy) 
      return b2->Map(xxx,yyy);
    return Color(0xffffff00);
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  virtual void Prepare()
  {
    if (!mymap) {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    std::string data(ptr->begin(), ptr->end());
    std::stringstream ss(data);
    ss >> ssx >> ssy;

    int *array = new int[ssx*ssy];
    for(int i=0;i<ssx*ssy;i++)
      {
	unsigned char c;
	ss >> c;
	int s = chars.size();
	int val = 0;
	for(int i=0;i<s;i++)
	  {
	    if (c==(unsigned char)chars[i]) val=i;
	  }
	array[i] = val;
      }
    mymap = array;
    }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> v;
  std::string url;
  std::string homepage;
  std::string chars;
  int *mymap;
  int dx,dy;
  int ssx,ssy;
};

extern std::string gameapi_homepageurl;

EXPORT GameApi::BM GameApi::BitmapApi::world_from_bitmap3(EveryApi &ev, std::vector<BM> v, std::string url, std::string chars, int dx, int dy)
{
  ::Bitmap<Color> *b = new WorldFromUrl(e,ev,v,url,gameapi_homepageurl, chars, dx,dy);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  return add_bitmap(e, handle);
}

EXPORT GameApi::BM GameApi::FloatBitmapApi::subfloatbitmap(GameApi::EveryApi &ev, FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  GameApi::BB b = to_bool(fb, range_start, range_end);
  Color c(true_color);
  Color c2(false_color);
  GameApi::BM bm = ev.bool_bitmap_api.to_bitmap_1(b, c.r, c.g, c.b, c.alpha,
						 c2.r, c2.g, c2.b, c2.alpha);
  return bm;
}

EXPORT GameApi::O GameApi::BoolBitmapApi::to_volume(BB b, float dist)
{
  BoolBitmap *c = find_bool_bitmap(e,b);
  Bitmap<bool> *bm = c->bitmap;
  return add_volume(e, new BitmapVolume(bm, dist));
}
class ChooseBitmap3 : public Bitmap<Color>
{
public:
  ChooseBitmap3(Bitmap<bool> &bools, Bitmap<Color> &true_bitmap, Bitmap<Color> &false_bitmap) : bools(bools), true_bitmap(true_bitmap), false_bitmap(false_bitmap) { }
  void Collect(CollectVisitor &vis)
  {
    bools.Collect(vis);
    true_bitmap.Collect(vis);
    false_bitmap.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bools.Prepare(); true_bitmap.Prepare(); false_bitmap.Prepare(); }

  virtual int SizeX() const { return std::min(std::min(bools.SizeX(), true_bitmap.SizeX()), false_bitmap.SizeX()); }
  virtual int SizeY() const { return std::min(std::min(bools.SizeY(), true_bitmap.SizeY()), false_bitmap.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    bool b = bools.Map(x,y);
    if (b) { return true_bitmap.Map(x,y); }
    return false_bitmap.Map(x,y);
  }
private:
  Bitmap<bool> &bools;
  Bitmap<Color> &true_bitmap;
  Bitmap<Color> &false_bitmap;
};

class ChooseBitmap4 : public Bitmap<Color>
{
public:
  ChooseBitmap4(Bitmap<float> &floats, Bitmap<Color> &bitmap_0, Bitmap<Color> &bitmap_1) : floats(floats), bitmap_0(bitmap_0), bitmap_1(bitmap_1) { }
  void Collect(CollectVisitor &vis)
  {
    floats.Collect(vis);
    bitmap_0.Collect(vis);
    bitmap_1.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { floats.Prepare(); bitmap_0.Prepare(); bitmap_1.Prepare(); }

  virtual int SizeX() const { return std::min(std::min(floats.SizeX(), bitmap_0.SizeX()), bitmap_1.SizeX()); }
  virtual int SizeY() const { return std::min(std::min(floats.SizeY(), bitmap_0.SizeY()), bitmap_1.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    float b = floats.Map(x,y);
    return Color::Interpolate(bitmap_0.Map(x,y), bitmap_1.Map(x,y), b);
  }
private:
  Bitmap<float> &floats;
  Bitmap<Color> &bitmap_0;
  Bitmap<Color> &bitmap_1;
};

class PerlinNoise : public Bitmap<float>
{
public:
  PerlinNoise(Bitmap<float> &grad_1, Bitmap<float> &grad_2) : grad_1(grad_1), grad_2(grad_2) { }
  void Collect(CollectVisitor &vis)
  {
    grad_1.Collect(vis);
    grad_2.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { grad_1.Prepare(); grad_2.Prepare(); }

  int SizeX() const { return grad_1.SizeX(); }
  int SizeY() const { return grad_1.SizeY(); }
  float Map(int x, int y) const
  {
    float xx = float(x)/SizeX();
    float yy = float(y)/SizeY();

    int x0 = (xx>0.0 ? (int)xx : (int)xx-1);
    int x1 = x0+1;
    int y0 = (yy>0.0 ? (int)yy : (int)yy -1);
    int y1 = y0+1;
    float sx = x-(double)x0;
    float sy = y-(double)y0;
    float n0,n1,ix0,ix1, value;
    n0 = dotGridGradient(x0,y0,xx,yy);
    n1 = dotGridGradient(x1,y0,xx,yy);
    ix0 = lerp(n0,n1,sx);
    n0 = dotGridGradient(x0,y1,xx,yy);
    n1 = dotGridGradient(x1,y1,xx,yy);
    ix1 = lerp(n0,n1,sx);
    value = lerp(ix0, ix1, sy);
    return value;
  }
  float dotGridGradient(int ix, int iy, float x, float y) const {
    float dx = x - (double)ix;
    float dy = y - (double)iy;
    return dx*grad_1.Map(ix,iy) + dy*grad_2.Map(ix,iy);
  }

  float lerp(float a_0, float a_1, float w) const
  {
    return (1.0-w)*a_0+w*a_1;
  }
private:
  Bitmap<float> & grad_1;
  Bitmap<float> & grad_2;
};
EXPORT GameApi::FB GameApi::FloatBitmapApi::perlin_noise(FB grad_1, FB grad_2)
{
  Bitmap<float> *g_1 = find_float_bitmap(e, grad_1)->bitmap;
  Bitmap<float> *g_2 = find_float_bitmap(e, grad_2)->bitmap;
  return add_float_bitmap(e, new PerlinNoise(*g_1, *g_2));
}
EXPORT GameApi::BM GameApi::FloatBitmapApi::choose_bitmap(FB fb, BM bitmap_0, BM bitmap_1)
{
  Bitmap<float> *bools2 = find_float_bitmap(e, fb)->bitmap;
  BitmapHandle *handle = find_bitmap(e, bitmap_0);
  Bitmap<Color> *true2 = find_color_bitmap(handle);
  BitmapHandle *handle2 = find_bitmap(e, bitmap_1);
  Bitmap<Color> *false2 = find_color_bitmap(handle2);
  Bitmap<Color> *bm = new ChooseBitmap4(*bools2, *true2, *false2);
  return add_color_bitmap2(e, bm);
}
EXPORT GameApi::BM GameApi::BoolBitmapApi::choose_bitmap(BB bools, BM true_bitmap, BM false_bitmap)
{
  Bitmap<bool> *bools2 = find_bool_bitmap(e, bools)->bitmap;
  BitmapHandle *handle = find_bitmap(e, true_bitmap);
  Bitmap<Color> *true2 = find_color_bitmap(handle);
  BitmapHandle *handle2 = find_bitmap(e, false_bitmap);
  Bitmap<Color> *false2 = find_color_bitmap(handle2);
  Bitmap<Color> *bm = new ChooseBitmap3(*bools2, *true2, *false2);
  return add_color_bitmap2(e, bm);
}
class TriBoolBitmap : public Bitmap<bool>
{
public:
  TriBoolBitmap(Bitmap<bool> *bb, Point2d p0, Point2d p1, Point2d p2) : bb(bb), p0(p0), p1(p1), p2(p2) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    Prepare();
  }

  void Prepare() {
    float A = (-p1.y * p2.x + p0.y*(-p1.x + p2.x) + p0.x*(p1.y - p2.y) + p1.x*p2.y);
    if (A<=0.0f)
      {
	std::swap(p0,p2);
      }
  }
  int SizeX() const { return bb->SizeX(); }
  int SizeY() const { return bb->SizeY(); }
  bool Map(int x, int y) const
  {
    Point2d p = { float(x),float(y) };
    float s = (p0.y*p2.x - p0.x*p2.y + (p2.y-p0.y)*p.x + (p0.x-p2.x)*p.y);
    float t = (p0.x*p1.y - p0.y*p1.x + (p0.y-p1.y)*p.x + (p1.x-p0.x)*p.y);
    if (s<= 0.0 || t<=0.0) return false;
    float A = (-p1.y * p2.x + p0.y*(-p1.x+p2.x) + p0.x*(p1.y-p2.y) + p1.x*p2.y);
    return (s+t)<A;

  }
private:
  Bitmap<bool> *bb;
  Point2d p0,p1,p2;
};
EXPORT GameApi::BB GameApi::BoolBitmapApi::tri(BB orig, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y)
{
  Bitmap<bool> *bb = find_bool_bitmap(e, orig)->bitmap;
  Point2d p1 = { p1_x, p1_y };
  Point2d p2 = { p2_x, p2_y };
  Point2d p3 = { p3_x, p3_y };
  return add_bool_bitmap(e, new TriBoolBitmap(bb, p1,p2,p3));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::rings(int sx, int sy, float center_x_start, float center_y_start, float center_x_end, float center_y_end, float start_radius, float end_radius, float start_thickness, float end_thickness, int numrings)
{
  BB bg = bb_empty(sx,sy);
  int s = numrings;
  float delta_val = 1.0/numrings;
  for(int i=0;i<s;i++)
    {
      float val = i*delta_val;
      float center_x = (1.0-val)*center_x_start + val*center_x_end;
      float center_y = (1.0-val)*center_y_start + val*center_y_end;
      float radius = (1.0-val)*start_radius + val * end_radius;
      float thickness = (1.0-val)*start_thickness + val * end_thickness;

      BB c1 = circle(bg, center_x, center_y, radius);
      BB c2 = circle(bg, center_x, center_y, radius-thickness);
      BB cc = andnot_bitmap(c1,c2);
      bg = or_bitmap(bg, cc);
    }
  return bg;
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::transform(BB orig, std::function<bool (int,int, bool)> f)
{
  BoolBitmap *c = find_bool_bitmap(e,orig);
  Bitmap<bool> *bm = c->bitmap;
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));

  BitmapTransformFromFunction<bool> *trans = new BitmapTransformFromFunction<bool>(*bm, f);

  return add_bool_bitmap(e, trans);
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::bb_empty(int sx, int sy)
{
  return add_bool_bitmap(e, new ConstantBitmap_bool(false, sx,sy));
}

EXPORT GameApi::FB GameApi::FloatBitmapApi::fb_empty(int sx, int sy)
{
  return add_float_bitmap(e, new ConstantBitmap_float(0.0, sx,sy));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color(BM bm, int r, int g, int b)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  return add_bool_bitmap(e,new EquivalenceClassColorBitmap(*color_bm, Color(r,g,b)));
}

template<class T>
class EquivalenceClassFromArea : public Bitmap<bool>
{
public:
  EquivalenceClassFromArea(Bitmap<Color> &bm, T f) : bm(bm),  f(f) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    return f( c.r,c.g,c.b,c.alpha);
  }
  
private:
  Bitmap<Color> &bm;
  T f;
};

EXPORT GameApi::BB GameApi::BoolBitmapApi::from_float_bitmap(FB float_bm, float range_start, float range_end)
{
  FloatBitmap *fb = find_float_bitmap(e, float_bm);
  Bitmap<float> *bm = fb->bitmap;
  return add_bool_bitmap(e, new BoolBitmapFromFloatBitmap2(*bm, range_start, range_end));
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, std::function<bool (int, int, int,int)> f)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_bool_bitmap(e,new EquivalenceClassFromArea<std::function<bool(int,int,int,int)> >(*color_bm, f));
}

struct RangeData
{
  int r_start, r_end;
  int g_start, g_end;
  int b_start, b_end;
  int a_start, a_end;
};

bool range_select_color_area(int r, int g, int b,int a, void* dt)
{
  RangeData *data = (RangeData*)dt;
  if (r<data->r_start || r>data->r_end) { return false; }
  if (g<data->g_start || g>data->g_end) { return false; }
  if (b<data->b_start || b>data->b_end) { return false; }
  if (a<data->a_start || a>data->a_end) { return false; }
  return true;
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, int r_start, int r_end, int g_start, int g_end, int b_start, int b_end, int a_start, int a_end)
{

  RangeData data = { r_start, r_end, g_start, g_end, b_start, b_end, a_start, a_end };
  RangeData *dt2 = new RangeData;
  *dt2 = data;
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;
  return from_bitmaps_color_area(bm, [dt2](int a,int b,int c,int d) { return range_select_color_area(a,b,c,d,(void*)dt2); } );  //, _1,_2,_3,_4,(void*)dt2));
}
class LineBoolBitmap : public Bitmap<bool>
{
public:
  LineBoolBitmap(Bitmap<bool> &bg, Point2d p1, Point2d p2, float line_width1, float line_width2) : bg(bg), p1(p1), p2(p2), line_width1(line_width1), line_width2(line_width2) { }
  void Collect(CollectVisitor &vis)
  {
    bg.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bg.Prepare(); }

  virtual int SizeX() const { return bg.SizeX(); }
  virtual int SizeY() const { return bg.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Point2d pp = {(float)x,(float)y };
    
    Vector2d vw = p1-p2;
    float l2 = vw.Dist();
    if (l2 == 0.0) { return (pp-p1).Dist() < std::max(line_width1,line_width2); }
    
    float t = Vector2d::DotProduct(pp-p1, p2-p1)/l2/l2;
    if (t<0.0) return (pp-p1).Dist() < line_width1;
    else if (t>1.0) return (pp-p2).Dist() < line_width2;
    
    Point2d proj = p1 + t*(p2-p1);
    float dist = (pp-proj).Dist();
    return dist < ((1.0-t)*line_width1 + t*line_width2); 
  }
private:
  Bitmap<bool> &bg;
  Point2d p1,p2;
  float line_width1, line_width2;
};
#if 0
class EllipseBoolBitmap : public Bitmap<bool>
{
public:
  EllipseBoolBitmap(Bitmap<bool> &bg, Point2d c1, Point2d c2, float sum) : bg(bg), c1(c1), c2(c2), sum(sum) { }
  void Collect(CollectVisitor &vis)
  {
    bg.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bg.Prepare(); }

  virtual int SizeX() const { return bg.SizeX(); }
  virtual int SizeY() const { return bg.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    float dx = c1.x-x;
    float dy = c1.y-y;
    float dx2 = c2.x-x;
    float dy2 = c2.y-y;
    float dist1 = sqrt(dx*dx+dy*dy);
    float dist2 = sqrt(dx2*dx2+dy2*dy2);
    float ss = dist1+dist2;
    bool b = ss < sum;
    if (!b) { b = bg.Map(x,y); }
    return b;
  }

private:
  Bitmap<bool> &bg;
  Point2d c1,c2;
  float sum;
};
#endif
EXPORT GameApi::BB GameApi::BoolBitmapApi::line(BB bg, float p_x, float p_y, float p2_x, float p2_y, float line_width1, float line_width2)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d p = { p_x, p_y };
  Point2d p2 = { p2_x, p2_y };
  return add_bool_bitmap(e, new LineBoolBitmap(*bm, p,p2, line_width1,line_width2));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::ellipse(BB bg, float center_x, float center_y, float center2_x, float center2_y, float sum_of_distances)
{
#if 0
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d center = { center_x, center_y };
  Point2d center2 = { center2_x, center2_y };
  return add_bool_bitmap(e, new EllipseBoolBitmap(*bm, center, center2, sum_of_distances));
#endif
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::circle(BB bg, float center_x, float center_y, float radius)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d center = { center_x, center_y };
  Bitmap<bool> *circle = new BitmapCircle(center, radius, bm->SizeX(), bm->SizeY());
  Bitmap<bool> *orbitmap = new OrBitmap(*bm, *circle);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(circle));
  return add_bool_bitmap(e, orbitmap);
}

struct Rectangle_data
{
  float start_x, end_x;
  float start_y, end_y;
};

bool Rectangle_func(int x, int y, void* data)
{
  Rectangle_data *dt = (Rectangle_data*)data;
  if (x<dt->start_x) return false;
  if (x>=dt->end_x) return false;
  if (y<dt->start_y) return false;
  if (y>=dt->end_y) return false;
  return true;
}

EXPORT int GameApi::BoolBitmapApi::size_x(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
EXPORT int GameApi::BoolBitmapApi::size_y(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
EXPORT bool GameApi::BoolBitmapApi::boolvalue(BB bm, int x, int y)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}   
  
EXPORT GameApi::BM GameApi::BitmapApi::conical_gradient(int sx, int sy, float x, float y, float angle1, float angle2, unsigned int color_1, unsigned int color_2)
{
#if 0
  return add_color_bitmap2(e, new ConicalGradientBitmap(sx,sy,x,y, angle1,angle2,color_1,color_2));
#endif
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::sections(int sx, int sy, float x, float y, std::function<bool (float angle)> f)
{
#if 0
  return add_bool_bitmap(e, new SectionsBoolBitmap(sx,sy,x,y,f));
#endif
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::part_circle(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad)
{
  return add_bool_bitmap(e, new PartCircleBoolBitmap(sx,sy, x,y,start_angle,end_angle, start_rad, end_rad));
}

EXPORT int GameApi::FloatBitmapApi::size_x(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
EXPORT int GameApi::FloatBitmapApi::size_y(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
EXPORT float GameApi::FloatBitmapApi::floatvalue(FB bm, int x, int y)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}

class RectBitmap44 : public Bitmap<bool>
{
public:
  RectBitmap44(Bitmap<bool> *next, float x, float y, float width, float height)
    : next(next), x(x), y(y), width(width), height(height)
  {
  }
  virtual int SizeX() const { if (next) return next->SizeX(); return 1; }
  virtual int SizeY() const { if (next) return next->SizeY(); return 1; }
  virtual bool Map(int xx, int yy) const
  {
    bool res = true;
    if (xx<x) res=false;
  if (xx>=x+width) res=false;
  if (yy<y) res=false;
  if (yy>=y+height) res=false;
  if (res==false&&next) return next->Map(xx,yy);
  return true;
  }
  void Collect(CollectVisitor &vis)
  {
    if (next) next->Collect(vis);
  }
  void HeavyPrepare() { }

  virtual void Prepare() { if (next) next->Prepare(); }
private:
  Bitmap<bool> *next;
  float x,y,width,height;
};
EXPORT GameApi::BB GameApi::BoolBitmapApi::rectangle(BB bg, float x, float y, float width, float height)
{
  BoolBitmap *handle3 = find_bool_bitmap(e,bg);
  Bitmap<bool> *bm = handle3?handle3->bitmap:0;
  return add_bool_bitmap(e, new RectBitmap44(bm, x,y,width,height));
#if 0
  
  Rectangle_data *d = new Rectangle_data;
  d->start_x = x;
  d->start_y = y;
  d->end_x = x+width;
  d->end_y = y+height;
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(d));

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  return or_bitmap(bg, function(std::bind(Rectangle_func,_1, _2,(void*)d), size_x(bg), size_y(bg)));
#endif
}

class BoolBitmapSprite : public Bitmap<bool>
{
public:
  BoolBitmapSprite(Bitmap<bool> &bg, Bitmap<bool> &sprite, float x, float y, float mult_x, float mult_y) : bg(bg), sprite(sprite), x(x), y(y), mult_x(mult_x), mult_y(mult_y) { }
  void Collect(CollectVisitor &vis)
  {
    bg.Collect(vis);
    sprite.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bg.Prepare(); sprite.Prepare(); }

  int SizeX() const {return bg.SizeX(); }
  int SizeY() const {return bg.SizeY(); }
  bool Map(int mx, int my) const {
    float xx = mx - x;
    float yy = my - y;
    xx /= mult_x;
    yy /= mult_y; 
    if (xx >= 0.0 && xx < sprite.SizeX())
      if (yy >= 0.0 && yy < sprite.SizeY())
	{
	  bool b = sprite.Map(xx,yy);
	  if (!b) { return bg.Map(mx,my); }
	  return true;
	}
    return bg.Map(mx,my);
  }
private:
  Bitmap<bool> &bg;
  Bitmap<bool> &sprite;
  float x,y,mult_x, mult_y;
};

EXPORT GameApi::BB GameApi::BoolBitmapApi::sprite(BB bg, BB sprite, float x, float y, float mult_x, float mult_y)
{
  Bitmap<bool> *bg_1 = find_bool_bitmap(e,bg)->bitmap;
  Bitmap<bool> *sprite_1 = find_bool_bitmap(e,sprite)->bitmap;
  return add_bool_bitmap(e, new BoolBitmapSprite(*bg_1, *sprite_1, x, y, mult_x, mult_y));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::not_bitmap(BB b)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, b)->bitmap;
  return add_bool_bitmap(e, new NotBitmap(*bm));
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::or_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new OrBitmap(*bm1,*bm2));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::xor_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new XorBitmap(*bm1,*bm2));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::andnot_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new AndNotBitmap(*bm1,*bm2));
}
EXPORT GameApi::BM GameApi::BoolBitmapApi::to_bitmap(BB bools, int true_r, int true_g, int true_b, int true_a, int false_r, int false_g, int false_b, int false_a)
{
  return to_bitmap_1(bools, true_r, true_g, true_b, true_a,
		     false_r, false_g, false_b, false_a);
}
GameApi::BM GameApi::BoolBitmapApi::to_bitmap_1(BB bools, int true_r, int true_g, int true_b, int true_a, int false_r, int false_g, int false_b, int false_a)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, bools)->bitmap;
  return add_color_bitmap2(e, new ChooseTBitmap<Color>(*bm1, Color(false_r, false_g, false_b, false_a), Color(true_r, true_g, true_b, true_a)));
}

EXPORT GameApi::BoolBitmapApi::BoolBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::BoolBitmapApi::~BoolBitmapApi() { }

EXPORT GameApi::FloatBitmapApi::FloatBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::FloatBitmapApi::~FloatBitmapApi() { }

EXPORT GameApi::FB GameApi::FloatBitmapApi::function(std::function<float (int,int)> f, int sx, int sy)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_float_bitmap(e, new BitmapFromFunction<float>(f,sx,sy));
}

class BitmapFromRed : public Bitmap<float>
{
public:
  BitmapFromRed(Bitmap<Color> &bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.r;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

class GaussianBitmap : public Bitmap<float>
{
public:
  GaussianBitmap(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy) : start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), sx(sx),sy(sy)
  {
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual float Map(int x, int y) const
  {
    float xx = float(x);
    float yy = float(y);
    xx-=start_x;
    yy-=start_y;
    xx/=end_x-start_x;
    yy/=end_y-start_y;

    xx*=6.0;
    yy*=6.0;
    xx-=3.0;
    yy-=3.0;
    float val = exp(-xx*xx-yy*yy);
    val*=(end_z-start_z);
    val+=start_z;
    return val;
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

  virtual void Prepare() { }

private:
  float start_x, end_x, start_y, end_y, start_z, end_z;
  int sx,sy;
};
EXPORT GameApi::FB GameApi::FloatBitmapApi::gaussian(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy)
{
  return add_float_bitmap(e, new GaussianBitmap(start_x, end_x, start_y, end_y, start_z, end_z, sx,sy));
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::from_red(BM bm)
{
  BitmapHandle *handle = find_bitmap(e,bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromRed(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromGreen : public Bitmap<float>
{
public:
  BitmapFromGreen(Bitmap<Color> &bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.g;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};
#if 0
class SpaceFillFloatBitmap : public Bitmap<float>
{
public:
  SpaceFillFloatBitmap(Point *pt, float *values, int size, int sx, int sy) : pt(pt), values(values), size(size), sx(sx), sy(sy) { }
  ~SpaceFillFloatBitmap() { delete [] pt; }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float dist = 50000.0*50000.0;
    int pos = 0;
    for(int i=0;i<size;i++)
      {
	float dx = x-pt[i].x;
	float dy = y-pt[i].y;
	if (dx*dx+dy*dy<dist)
	  {
	    dist = dx*dx+dy*dy;
	    pos = i;
	  }
      }
    return values[pos];
  }
private:
  Point *pt;
  float *values;
  int size;
  int sx,sy;
};
#endif
EXPORT GameApi::FB GameApi::FloatBitmapApi::space_fill(PT *array, float *array2, int size, int sx, int sy)
{
#if 0
  Point *array3 = new Point[size];
  for(int i=0;i<size;i++)
    array3[i] = *find_point(e, array[i]);
  return add_float_bitmap(e, new SpaceFillFloatBitmap(array3, array2, size, sx,sy));
#endif
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::from_green(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromGreen(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromBlue : public Bitmap<float>
{
public:
  BitmapFromBlue(Bitmap<Color> &bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.b;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_blue(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromBlue(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromAlpha : public Bitmap<float>
{
public:
  BitmapFromAlpha(Bitmap<Color> &bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.alpha;
    //std::cout << "Alpha: " << x << " " << y << " " << val << " " << float(val) / 255.0 << std::endl;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_alpha(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromAlpha(*bmc);
  return add_float_bitmap(e, bm2);
}
class FromBoolBitmap : public Bitmap<float>
{
public:
  FromBoolBitmap(Bitmap<bool> &bm, float val_true, float val_false) : bm(bm), val_true(val_true), val_false(val_false) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bm.Prepare(); }

  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    bool b = bm.Map(x,y);
    if (b) return val_true;
    return val_false;
  }

private:
  Bitmap<bool> &bm;
  float val_true;
  float val_false;
};

float mymin(float x, float y)
{
  return (x<y) ? x : y;
}
class DistanceFieldBitmap : public Bitmap<float>
{
public:
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
  }
  void HeavyPrepare() {
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    delete [] array_x;
    delete [] array_y;
    array_x = new float[sx*sy];
    array_y = new float[sx*sy];
    array_sx = sx;
    array_sy = sy;
    for(int x=0;x<sx;x++)
      {
	for(int y=0;y<sy;y++)
	  {
	    float b = bm->Map(x,y);
	    array_x[x+y*sx] = 1000.0-1000.0*b;
	    array_y[x+y*sx] = 1000.0-1000.0*b;
	  }
      }
    step1();
    step2();

  }

  void Prepare() 
  { 
    bm->Prepare(); 
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    delete [] array_x;
    delete [] array_y;
    array_x = new float[sx*sy];
    array_y = new float[sx*sy];
    array_sx = sx;
    array_sy = sy;
    for(int x=0;x<sx;x++)
      {
	for(int y=0;y<sy;y++)
	  {
	    float b = bm->Map(x,y);
	    array_x[x+y*sx] = 1000.0-1000.0*b;
	    array_y[x+y*sx] = 1000.0-1000.0*b;
	  }
      }
    step1();
    step2();

  }

  DistanceFieldBitmap(Bitmap<float> *bm, float max_value) : bm(bm), max_value(max_value), array_x(0), array_y(0), array_sx(0), array_sy(0)
  {
  }
  int SizeX() const { return bm->SizeX(); }
  int SizeY() const { return bm->SizeY(); }
  float Map(int x, int y) const
  {
    if (x<0 || x>=bm->SizeX()) return 0.0;
    if (y<0 || y>=bm->SizeY()) return 0.0;
    if (array_sx != bm->SizeX()) const_cast<DistanceFieldBitmap*>(this)->Prepare();
    if (array_sy != bm->SizeY()) const_cast<DistanceFieldBitmap*>(this)->Prepare();

    int sx = bm->SizeX();
    return scale(mymin(array_x[x+y*sx],array_y[x+y*sx]));
  }
  float scale(float x) const
  {
    x/=max_value;
    x = 1.0 - x;
    x = std::max(x, 0.0f);
    //std::cout << "DistanceFieldBitmap: " << x << std::endl;
    return x;
  }
  
  void step2()
  {
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    for(int y=sy-2;y>=0;y--)
      {
	for(int x=0;x<sx-1;x++)
	  {
	    array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[x+(y+1)*sx]+1.0);
	    array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[x+(y+1)*sx]+1.0);
	  }
	for(int x=1;x<sx-1;x++)
	  {
	    array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[(x-1)+(y)*sx]+1.0);
	    array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[(x-1)+(y)*sx]+1.0);

	  }
	for(int x=sx-2;x>=0;x--)
	  {
	    array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[(x+1)+(y)*sx]+1.0);
	    array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[(x+1)+(y)*sx]+1.0);
	    //std::cout << array_x[x+y*sx] << "#" << array_y[x+y*sx] << std::endl;

	  }
      }
  }
  void step1()
  {
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    for(int y=1;y<sy-1;y++) {
      for(int x=0;x<sx-1;x++)
	{
	  array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[x+(y-1)*sx]+1.0);
	  array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[x+(y-1)*sx]+1.0);
	}
      for(int x=1;x<sx-1;x++)
	{
	  array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[(x-1)+y*sx]+1.0);
	  array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[(x-1)+y*sx]+1.0);
	}
      for(int x=sx-2;x>=0;x--)
	{
	  array_x[x+y*sx] = mymin(array_x[x+y*sx], array_x[(x+1)+y*sx]+1.0);
	  array_y[x+y*sx] = mymin(array_y[x+y*sx], array_y[(x+1)+y*sx]+1.0);
	}
    }
    
  }
  ~DistanceFieldBitmap() { delete [] array_x; delete [] array_y; }

private:
  Bitmap<float> *bm;
  float max_value;
  float *array_x;
  float *array_y;
  int array_sx;
  int array_sy;
};
class BorderFloatBitmap : public Bitmap<float>
{
public:
  BorderFloatBitmap(Bitmap<float> *bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm->Prepare(); }

  int SizeX() const { return bm->SizeX(); }
  int SizeY() const { return bm->SizeY(); }
  float Map(int x, int y) const
  {
    float f0 = bm->Map(x,y);
    if (std::isnan(f0))
    if (x>0 && x<SizeX()-1)
      if (y>0 && y<SizeY()-1)
	{
	  float fx = bm->Map(x+1,y);
	  float fy = bm->Map(x,y+1);
	  float fxx = bm->Map(x-1,y);
	  float fyy = bm->Map(x,y-1);

	  float fx1 = bm->Map(x+1,y+1);
	  float fy1 = bm->Map(x-1,y+1);
	  float fxx1 = bm->Map(x-1,y-1);
	  float fyy1 = bm->Map(x+1,y-1);

	  if (!std::isnan(fx) ||
	      !std::isnan(fy) ||
	      !std::isnan(fxx) ||
	      !std::isnan(fyy) ||
	      !std::isnan(fx1) ||
	      !std::isnan(fy1) ||
	      !std::isnan(fxx1) ||
	      !std::isnan(fyy1)) {
	    return 0.0;
	  }
	}
    return f0;
  }
private:
  Bitmap<float> *bm;
};
EXPORT GameApi::FB GameApi::FloatBitmapApi::distance_field(FB bb, float max_value)
{
  Bitmap<float> *bm = find_float_bitmap(e, bb)->bitmap;
  return add_float_bitmap(e, new DistanceFieldBitmap(bm, max_value));
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::add_border(FB fb)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_float_bitmap(e, new BorderFloatBitmap(bm));
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::from_bool(GameApi::BB b, float val_true, float val_false)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, b)->bitmap;
  return add_float_bitmap(e, new FromBoolBitmap(*bm, val_true, val_false));
}

#if 0
class BitmapFromRGBA : public Bitmap<Color>
{
public:
  BitmapFromRGBA(Bitmap<float> &r, Bitmap<float> &g, Bitmap<float> &b, Bitmap<float> &a) : r(r), g(g), b(b), a(a) { }
  void Collect(CollectVisitor &vis)
  {
    r.Collect(vis);
    g.Collect(vis);
    b.Collect(vis);
    a.Collect(vis);
  }
  void HeavyPrepare() { }
  
  void Prepare() { r.Prepare(); g.Prepare(); b.Prepare(); a.Prepare(); }

  virtual int SizeX() const { return r.SizeX(); }
  virtual int SizeY() const { return r.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    float fr = r.Map(x,y);
    float fg = g.Map(x,y);
    float fb = b.Map(x,y);
    float fa = a.Map(x,y);
    int r = fr*255.0;
    int g = fg*255.0;
    int b = fb*255.0;
    int a = fa*255.0;
    return Color(r,g,b,a);

  }
private:
  Bitmap<float> &r;
  Bitmap<float> &g;
  Bitmap<float> &b;
  Bitmap<float> &a;
};
#endif

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_color(FB r, FB g, FB b, FB a)
{
#if 0
  Bitmap<float> *rb = find_float_bitmap(e,r)->bitmap;
  Bitmap<float> *gb = find_float_bitmap(e,g)->bitmap;
  Bitmap<float> *bb = find_float_bitmap(e,b)->bitmap;
  Bitmap<float> *ab = find_float_bitmap(e,a)->bitmap;
  Bitmap<Color> *bm2 = new BitmapFromRGBA(*rb,*gb,*bb,*ab);
  return add_color_bitmap2(e, bm2);
#endif
}
GameApi::BB GameApi::FloatBitmapApi::to_bool(FB fb, float true_range_start, float true_range_end)
{
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  return add_bool_bitmap(e, new FloatRangeBitmap(*f, true_range_start, true_range_end));
}

#if 0
class FloatModBitmap : public Bitmap<bool>
{
public:
  FloatModBitmap(Bitmap<float> &fb, float mod_value) : fb(fb), mod_value(mod_value) { }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { fb.Prepare(); }

  int SizeX() const { return fb.SizeX(); }
  int SizeY() const { return fb.SizeY(); }
  bool Map(int x, int y) const
  {
    float val = fb.Map(x,y);
    float m_val = std::fmod(val, mod_value);
    return m_val < mod_value/2.0;
  }
private:
  Bitmap<float> &fb;
  float mod_value;
};
#endif

GameApi::BB GameApi::FloatBitmapApi::to_bool_mod(FB fb, float mod_value)
{
#if 0
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  return add_bool_bitmap(e, new FloatModBitmap(*f, mod_value));
#endif
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::from_bool_bitmap(BB bm, int csx, int csy)
{
  Bitmap<bool> *bm2 = find_bool_bitmap(e,bm)->bitmap;
  return add_float_bitmap(e, new FloatBitmapFromBoolBitmap(*bm2, csx, csy));
}

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_grayscale(FB fb)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_color_bitmap2(e, new GrayScaleBitmapFromFloatBitmap(*bm, Color(0,0,0,0), Color(255,255,255,255)));
}

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_grayscale_color(FB fb, int r, int g, int b, int a,
							int r2, int g2, int b2, int a2)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_color_bitmap2(e, new GrayScaleBitmapFromFloatBitmap(*bm, Color(r,g,b,a), Color(r2,g2,b2,a2)));
}

struct PointArray
{
  Point2d *array;
  int size;
  ~PointArray() { delete[] array; }
};

GameApi::BB GameApi::BoolBitmapApi::function(std::function<bool (int,int)> f, int sx, int sy)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  
  return add_bool_bitmap(e, new BitmapFromFunction<bool>(f, sx,sy));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::polygon(BB bg2, PT *points, int size)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  Bitmap<bool> *bg = find_bool_bitmap(e, bg2)->bitmap;
  PointArray *pointarray = new PointArray;
  pointarray->array = new Point2d[size];
  pointarray->size = size;
  for(int i=0;i<size;i++)
    {
      Point p = *find_point(e,points[i]);
      Point2d p2d = { p.x, p.y };
      pointarray->array[i] = p2d;
    }
  NativeArray<Point2d> *array = new NativeArray<Point2d>(pointarray->array, size);
  PointCollection2dConvert *conv = new PointCollection2dConvert(*array);
  ContinuousLines2d *lines = new ContinuousLines2d(*conv, true);
  ContinuousBitmap<bool> *bm = new PolygonFill(bg->SizeX(), bg->SizeY(), *lines);
  BitmapFromContinuousBitmap<bool> *sbm = new BitmapFromContinuousBitmap<bool>(*bm, bg->SizeX(), bg->SizeY());
  OrBitmap *sbm2 = new OrBitmap(*bg, *sbm);
  env->deletes.push_back(std::shared_ptr<void>(pointarray));
  env->deletes.push_back(std::shared_ptr<void>(array));
  env->deletes.push_back(std::shared_ptr<void>(conv));
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(bm));
  env->deletes.push_back(std::shared_ptr<void>(sbm));

  return add_bool_bitmap(e, sbm2);
#endif
}

EXPORT GameApi::ContinuousBitmapApi::ContinuousBitmapApi(Env &e) : e(e) { }

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::cbm_empty(float x, float y)
{
  return constant(0x00000000, x, y);
}
#if 0
class DistanceRenderContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  DistanceRenderContinuousBitmap(DistanceRenderable *dist, ColorVolumeObject *colours, float sx, float sy) : dist(dist), colours(colours),sx(sx),sy(sy) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

  void Prepare() { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    int count = 0; 
    Point p(x,y,0.0);
    while(1) {
      float d = dist->distance(p);
      if (d<1.0) break;
      if (count>30) return Color(0x0000000);
      p+=Vector(0.0,0.0,d);
      count++;
    }
    unsigned int col = colours->ColorValue(p);
    return Color(col);
  }
private:
  DistanceRenderable *dist;
  ColorVolumeObject *colours;
  float sx,sy;
};
#endif

GameApi::CBM GameApi::ContinuousBitmapApi::distance_render(FD obj, COV colors, float sx, float sy)
{
#if 0
  DistanceRenderable *dist = find_distance(e, obj);
  ColorVolumeObject *colorsI = find_color_volume(e, colors);
  return add_continuous_bitmap(e, new DistanceRenderContinuousBitmap(dist, colorsI, sx,sy));
#endif
}
GameApi::CBM GameApi::ContinuousBitmapApi::constant(unsigned int color, float x, float y)
{
  return add_continuous_bitmap(e, new ConstantContinuousBitmap<Color>(x,y,Color(color)));
}


#if 0
struct BiCubicData
{
public:
  float x1_f_0;
  float x1_f_1;
  float x1_df_0;
  float x1_df_1;
  float x2_f_0;
  float x2_f_1;
  float x2_df_0;
  float x2_df_1;
};

class BiCubicInterpolate : public Function<Point2d, float>, DFunction<Point2d, float>
{
public:
  BiCubicInterpolate(BiCubicData &data) : data(data),
					  i1(data.x1_f_0,
					     data.x1_f_1,
					     data.x1_df_0,
					     data.x1_df_1),
					  i2(data.x2_f_0,
					     data.x2_f_1,
					     data.x2_df_0,
					     data.x2_df_1) { }
  float Index(Point2d p) const
  {
    float val1 = i1.Index(p.x);
    float val2 = i2.Index(p.x);
    float d1 = i1.DIndex(p.x);
    float d2 = i2.DIndex(p.x);
    CubicInterpolate iy(val1, val2, d1,d2);
    return iy.Index(p.y);
  }
  float DIndex(Point2d p) const
  {
    float val1 = i1.Index(p.x);
    float val2 = i2.Index(p.x);
    float d1 = i1.DIndex(p.x);
    float d2 = i2.DIndex(p.x);
    CubicInterpolate iy(val1, val2, d1,d2);
    return iy.DIndex(p.y);
  }
private:
  BiCubicData &data;
  CubicInterpolate i1;
  CubicInterpolate i2;
};

class BiCubicInterpolateBitmap : public ContinuousBitmap<Color>
{
public:
  BiCubicInterpolateBitmap(float f_0, float f_1, float df_0, float df_1,
			 float ff_0, float ff_1, float dff_0, float dff_1) 
  {
    data.x1_f_0 = f_0;
    data.x1_f_1 = f_1;
    data.x1_df_0 = df_0;
    data.x1_df_1 = df_1;

    data.x2_f_0 = ff_0;
    data.x2_f_1 = ff_1;
    data.x2_df_0 = dff_0;
    data.x2_df_1 = dff_1;
    inter = new BiCubicInterpolate(data);
  }
  ~BiCubicInterpolateBitmap() { delete inter; }
  virtual float SizeX() const { return 1.0; }
  virtual float SizeY() const { return 1.0; }
  virtual Color Map(float x, float y) const
  {
    Point2d p = {x,y };
    float val = inter->Index(p);
    //std::cout << val << std::endl;
    return Color(val,val,val,1.0f);
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

  virtual void Prepare() { }
 
private:
  BiCubicData data;
  BiCubicInterpolate *inter;
};
#endif
GameApi::CBM GameApi::ContinuousBitmapApi::bicubic(float f_0, float f_1, float df_0, float df_1,
						   float ff_0, float ff_1, float dff_0, float dff_1)
{
#if 0
  return add_continuous_bitmap(e, new BiCubicInterpolateBitmap(f_0,f_1,df_0,df_1, ff_0, ff_1, dff_0, dff_1));
#endif
}
#if 0
class FunctionContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  FunctionContinuousBitmap( std::function<unsigned int (float, float)> f, float sx, float sy) : f(f), sx(sx), sy(sy) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    return Color(f( x,y));
  }
public:
  std::function<unsigned int (float, float)> f; 
  float sx; float sy;
};
#endif
EXPORT unsigned int GameApi::ContinuousBitmapApi::get_pixel(CBM bitmap, float x, float y)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, bitmap);
  return cbm->Map(x,y).Pixel();
}

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::function(std::function<unsigned int (float,float)> f, float sx, float sy)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
#if 0
  return add_continuous_bitmap(e, new FunctionContinuousBitmap( f, sx, sy));
#endif
}
EXPORT GameApi::BM GameApi::ContinuousBitmapApi::sample(CBM c_bitmap, int sx, int sy) // SampleBitmap(CB<Color, int sx,int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_color_bitmap(e, new SampleBitmap(*cbm, sx, sy));
}
EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::rotate(CBM c_bitmap, float center_x, float center_y, float angle)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_continuous_bitmap(e, new RotateContinuousBitmap<Color>(cbm, center_x, center_y, angle));
  
}

EXPORT GameApi::BM GameApi::ContinuousBitmapApi::to_bitmap(CBM bm, int sx, int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, bm);  
  return add_color_bitmap(e, new BitmapFromContinuousBitmap<Color>(*cbm, sx,sy));
}

#if 0
class ComposeSurfaceColor : public ContinuousBitmap<Color>
{
public:
  ComposeSurfaceColor(SurfaceImpl *impl, ColorVolumeObject *obj) : impl(impl), obj(obj) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

  void Prepare() { }
  virtual float SizeX() const { return impl->surf->XSize(); }
  virtual float SizeY() const { return impl->surf->YSize(); }
  virtual Color Map(float x, float y) const
  {
    Point p = impl->surf->Index(x,y);
    unsigned int c = obj->ColorValue(p);
    return Color(c);
  }

private:
  SurfaceImpl *impl;
  ColorVolumeObject *obj;
};
#endif
EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::surfacecolor(S s, COV cov)
{
#if 0
  SurfaceImpl *impl = find_surface(e, s);
  ColorVolumeObject *obj = find_color_volume(e, cov);
  return add_continuous_bitmap(e, new ComposeSurfaceColor(impl, obj));
#endif
}
EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::from_bitmap(BM bm, float xsize, float ysize)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);

  return add_continuous_bitmap(e, new ContinuousBitmapFromBitmap<Color>(*bm2, xsize, ysize));
}
EXPORT GameApi::BM GameApi::BitmapApi::alt(std::vector<BM> vec, int index)
{
  if (vec.size()==0) { return newbitmap(1,1); }
  int s = vec.size();
  if (index<0||index>=s) return newbitmap(1,1);
  return vec[index];
}

EXPORT std::vector<GameApi::BM> GameApi::SpriteApi::bitmap_anim(std::function<GameApi::BM (float)> f, std::vector<float> key_frames)
{
  int s = key_frames.size();
  std::vector<GameApi::BM> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(f(key_frames[i]));
    }
  return vec;
}
#if 0
class BitmapAnimML : public MainLoopItem
{
public:
  BitmapAnimML(GameApi::EveryApi &ev, GameApi::SpriteApi &sp, std::vector<GameApi::BM> vec, std::vector<float> key_frames, float rep_time) : ev(ev), sp(sp), vec(vec), key_frames(key_frames), rep_time(rep_time) 
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec2.push_back(sp.create_vertex_array(vec[i]));
      }
  }
  //int shader_id() { return -1; }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }

  void Prepare() { }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    GameApi::SH sh;
    sh.id = e.sh_texture;
    ev.shader_api.use(sh);
    float time = ev.mainloop_api.get_time();
    if(time > rep_time) { time -= int(time/rep_time)*rep_time; }
    int ss = key_frames.size();
    int i = 0;
    for(;i<ss-1;i++)
      {
	float start_range = key_frames[i];
	float end_range = key_frames[i+1];
	if (time >= start_range && time < end_range)
	  {
	    break;
	  }
      }
    sp.render_sprite_vertex_array(vec2[i]);
  }
private:
  GameApi::EveryApi &ev;
  GameApi::SpriteApi &sp;
  std::vector<GameApi::BM> vec;
  std::vector<GameApi::VA> vec2;
  std::vector<float> key_frames;
  float rep_time;
};
#endif
EXPORT GameApi::ML GameApi::SpriteApi::bitmap_anim_ml(EveryApi &ev, std::vector<BM> vec,
						      std::vector<float> key_frames,
						      float repeat_time)
{
#if 0 
  return add_main_loop(e, new BitmapAnimML(ev, *this, vec, key_frames, repeat_time));
#endif
}


EXPORT GameApi::TXID GameApi::FloatBitmapApi::to_texid(FB fb)
{
  OpenglLowApi *ogl = g_low->ogl;

  Bitmap<float> *ffb = find_float_bitmap(e, fb)->bitmap;
  int sx = ffb->SizeX();
  int sy = ffb->SizeY();
  float *array = new float[sx*sy];
  float *arr = array;
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
      {
	*arr = ffb->Map(x,y);
	arr++;
      }

  Low_GLuint id;
  ogl->glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RED, sx,sy, 0, Low_GL_RED, Low_GL_FLOAT, array);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_LINEAR);      
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
  delete []array;

  GameApi::TXID id2;
  id2.id = id;
  return id2;

}
void GameApi::BitmapApi::prepare(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  b2->Prepare();
}

class PersistentCache : public Bitmap<Color>
{
public:
  PersistentCache(Bitmap<Color> &c, std::string filename) : c(c), filename(filename),res(0) { }
  virtual int SizeX() const { 
    if (!res) { std::cout << "PersistentCache:Bitmap Prepare not called!" << std::endl; }
    return res->SizeX()-1; }
  virtual int SizeY() const { 
    if (!res) { std::cout << "PersistentCache:Bitmap Prepare not called!" << std::endl; }

    return res->SizeY()-1; }
  virtual Color Map(int x, int y) const
  {
    if (!res) { std::cout << "PersistentCache:Bitmap Prepare not called!" << std::endl; }
    return res->Map(x,y);
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }

  virtual void Prepare()
  {
    if (!res)
      {
	std::ifstream ss(filename.c_str());
	ss.tie(nullptr);
	int count = 0;
	char c2;
	while(ss.get(c2)) { count++; }
	if (count==0)
	  {
	    PpmFile file(filename, c, true);
	    std::string contents = file.Contents();
	    std::ofstream ss(filename.c_str());
	    ss << contents;
	    ss.close();
	  }
	res = new PpmFileReader(filename);
	res->Prepare();
      }
  }

private:
  Bitmap<Color> &c;
  std::string filename;
  Bitmap<Color> *res;
};
GameApi::BM GameApi::BitmapApi::persistent_cache(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  ::Bitmap<Color> *b3 = new PersistentCache(*b2, filename);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b3;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}
GameApi::BM GameApi::BitmapApi::border(BM bm, int left, int right, int top, int bottom)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b3 = new BorderBitmap(*b2, Color::Transparent(),Color::Transparent(),Color::Transparent(),Color::Transparent(),
					 left, right, top, bottom);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b3;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}

//Ft I1=ev.font_api.newfont(FreeSans.ttf,40,20);
//BM I2=ev.font_api.font_string(I1,Quantum,5);
GameApi::BM GameApi::BitmapApi::add_shape_border(EveryApi &ev, GameApi::BM bm, float dist_field_size, float start_range, float end_range, int r, int g, int b, int a, int border_size)
{
  BM I1=ev.bitmap_api.border(bm,border_size,border_size,border_size,border_size);
  FB I2=ev.float_bitmap_api.from_alpha(I1);
  FB I3=ev.float_bitmap_api.distance_field(I2,dist_field_size);
  BB I4=ev.bool_bitmap_api.from_float_bitmap(I3,start_range,end_range);
  BM I5=ev.bool_bitmap_api.to_bitmap(I4,r,g,b,a,0,0,0,0);
  BM I5a = ev.bitmap_api.border(I5,2,2,2,2);
  BM I5b = ev.bitmap_api.simple_blur(I5a, 0.5, 0.125, 0.125, 0.125, 0.125);
  BM I5c = ev.bitmap_api.simple_blur(I5b, 0.5, 0.125, 0.125, 0.125, 0.125);
  //Ft I3=ev.font_api.newfont(FreeSans.ttf,40,20);
  //BM I4=ev.font_api.font_string(I3,Quantum,5);
  BM I6=ev.bitmap_api.blitbitmap(I5c,bm,border_size,border_size);
  return I6;
}

class SimpleBlur : public Bitmap<Color>
{
public:
  SimpleBlur(Bitmap<Color> &bm,
       float center_mult,
       float left_mult,
       float right_mult,
       float top_mult,
       float bottom_mult
       ) : bm(bm), center(center_mult),
	   left(left_mult), right(right_mult), top(top_mult), bottom(bottom_mult) 
  { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

  void Prepare() { bm.Prepare(); }
  virtual int SizeX() const { return bm.SizeX()-2; }
  virtual int SizeY() const { return bm.SizeY()-2; }
  virtual Color Map(int x, int y) const
  {
    int xx = x + 1;
    int yy = y + 1;
    Color c = bm.Map(xx,yy);
    Color l = bm.Map(xx-1,yy);
    Color r = bm.Map(xx+1,yy);
    Color t = bm.Map(xx,yy-1);
    Color b = bm.Map(xx,yy+1);
    c*=center;
    l*=left;
    r*=right;
    t*=top;
    b*=bottom;
    return c+l+r+t+b;
  }
private:
  Bitmap<Color> &bm;
  float center, left,right,top,bottom;
};
GameApi::BM GameApi::BitmapApi::simple_blur(BM bm, float center, float left, float right, float top, float bottom)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b3 = new SimpleBlur(*b2, center, left, right, top, bottom);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b3;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}

class Dithering : public Bitmap<bool>
{
public:
  Dithering(Bitmap<float> &val) : val(val) { }
  void Collect(CollectVisitor &vis)
  {
    val.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { val.Prepare(); }
  int SizeX() const { return val.SizeX(); }
  int SizeY() const { return val.SizeY(); }
  float error(int x, int y) const
  {
    int key = x+(y<<16);
    std::map<int, float>::const_iterator i = errors.find(key);
    if (i != errors.end())
      {
	return (*i).second;
      }
    if (x<0 || y<0 || x>=SizeX() || y>=SizeY())
      return 0.0;
    float e1 = error(x-1,y);
    float e2 = error(x,y-1);
    float v = e1+e2;
    v+=val.Map(x-1,y);
    v+=val.Map(x,y-1);
    v-=Map(x-1,y)?1.0:0.0;
    v-=Map(x,y-1)?1.0:0.0;
    if (v>1.0) v=1.0;
    if (v<-1.0) v=-1.0;
    errors[key] = v;
    return v;
  }
  bool Map(int x, int y) const
  {
    float err = error(x,y);
    float v = val.Map(x,y);
    v+=err;
    if (v<0.5) return false;
    return true;
  }
private:
  Bitmap<float> &val;
  mutable std::map<int, float> errors;
};

GameApi::BB GameApi::BoolBitmapApi::black_white_dithering(FB bm)
{
  Bitmap<float> *val = find_float_bitmap(e, bm)->bitmap;
  return add_bool_bitmap(e, new Dithering(*val));
}

class XBitmap : public ContinuousBitmap<float>
{
public:
  XBitmap(float sx, float sy) : sx(sx), sy(sy) {}
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  float Map(float x, float y) const
  {
    return x;
  }
private:
  float sx,sy;
};
class CBitmap : public ContinuousBitmap<float>
{
public:
  CBitmap(float sx, float sy, float val) : sx(sx), sy(sy), val(val) {}
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  float Map(float x, float y) const
  {
    return val;
  }
private:
  float sx,sy, val;
};

GameApi::CFB GameApi::FloatBitmapApi::C_bitmap(float sx, float sy, float c)
{
  return add_cont_float(e, new CBitmap(sx,sy,c));
}
GameApi::CFB GameApi::FloatBitmapApi::X_bitmap(float sx, float sy)
{
  return add_cont_float(e, new XBitmap(sx,sy));
}

class YBitmap : public ContinuousBitmap<float>
{
public:
  YBitmap(float sx, float sy) : sx(sx), sy(sy) {}
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  float Map(float x, float y) const
  {
    return y;
  }
private:
  float sx,sy;

};

GameApi::CFB GameApi::FloatBitmapApi::Y_bitmap(float sx, float sy)
{
  return add_cont_float(e, new YBitmap(sx,sy));
}

class MulBitmap_c : public ContinuousBitmap<float>
{
public:
  MulBitmap_c(std::vector<ContinuousBitmap<float>* > a1) : a1(a1) { }
  void Collect(CollectVisitor &vis)
  {
    int s = a1.size();
    for(int i=0;i<s;i++)
      a1[i]->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = a1.size();
    for(int i=0;i<s;i++)
      a1[i]->Prepare();
  }
  float SizeX() const {
    int s = a1.size();
    float sx = 100000.0;
    for(int i=0;i<s;i++)
      {
	float ssx = a1[i]->SizeX();
	if (sx>ssx) sx=ssx;
      }
    return sx;
  }
  float SizeY() const {
    int s = a1.size();
    float sy = 100000.0;
    for(int i=0;i<s;i++)
      {
	float ssy = a1[i]->SizeY();
	if (sy>ssy) sy=ssy;
      }
    return sy;
   }
  float Map(float x, float y) const
  {
    int s = a1.size();
    float val = 1.0;
    for(int i=0;i<s;i++)
      {
	val*=a1[i]->Map(x,y);
      }
    return val;
  }
private:
  std::vector<ContinuousBitmap<float>*> a1;
};

class AddBitmap_c : public ContinuousBitmap<float>
{
public:
  AddBitmap_c(std::vector<ContinuousBitmap<float>* > a1) : a1(a1) { }
  void Collect(CollectVisitor &vis)
  {
    int s = a1.size();
    for(int i=0;i<s;i++)
      a1[i]->Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  void Prepare() {
    int s = a1.size();
    for(int i=0;i<s;i++)
      a1[i]->Prepare();
  }
  float SizeX() const {
    int s = a1.size();
    float sx = 100000.0;
    for(int i=0;i<s;i++)
      {
	float ssx = a1[i]->SizeX();
	if (sx>ssx) sx=ssx;
      }
    return sx;
  }
  float SizeY() const {
    int s = a1.size();
    float sy = 100000.0;
    for(int i=0;i<s;i++)
      {
	float ssy = a1[i]->SizeY();
	if (sy>ssy) sy=ssy;
      }
    return sy;
   }
  float Map(float x, float y) const
  {
    int s = a1.size();
    float val = 1.0;
    for(int i=0;i<s;i++)
      {
	val+=a1[i]->Map(x,y);
      }
    return val;
  }
private:
  std::vector<ContinuousBitmap<float>*> a1;
};

GameApi::CFB GameApi::FloatBitmapApi::MulBitmap(std::vector<CFB> a1)
{
  int s = a1.size();
  std::vector<ContinuousBitmap<float> *> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_cont_float(e, a1[i]));
    }
  return add_cont_float(e, new MulBitmap_c(vec));
}
GameApi::CFB GameApi::FloatBitmapApi::AddBitmap(std::vector<CFB> a1)
{
  int s = a1.size();
  std::vector<ContinuousBitmap<float> *> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_cont_float(e, a1[i]));
    }
  return add_cont_float(e, new AddBitmap_c(vec));
}

class EqualizerBitmap : public ContinuousBitmap<bool>
{
public:
  EqualizerBitmap(ContinuousBitmap<float> &a1,
		  ContinuousBitmap<float> &a2) : a1(a1), a2(a2) { }
  void Collect(CollectVisitor &vis)
  {
    a1.Collect(vis);
    a2.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() {
    a1.Prepare();
    a2.Prepare();
  }

  float SizeX() const { return std::min(a1.SizeX(), a2.SizeX()); }
  float SizeY() const { return std::min(a1.SizeY(), a2.SizeY()); }
  bool Map(float x, float y) const
  {
    return a1.Map(x,y) < a2.Map(x,y);
  }
private:
  ContinuousBitmap<float> &a1;
  ContinuousBitmap<float> &a2;
};
GameApi::CBB GameApi::FloatBitmapApi::Equalizer(CFB a1, CFB a2)
{
  ContinuousBitmap<float> *aa1 = find_cont_float(e, a1);
  ContinuousBitmap<float> *aa2 = find_cont_float(e, a2);
  return add_cont_bool(e, new EqualizerBitmap(*aa1, *aa2));
}

class SampleContFloatBitmap : public Bitmap<float>
{
public:
  SampleContFloatBitmap(ContinuousBitmap<float> &bm, int sx, int sy, float start_x, float end_x, float start_y, float end_y, float mult) : bm(bm), sx(sx), sy(sy), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y),mult(mult) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { bm.Prepare(); }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float xx = x*(end_x-start_x)/sx + start_x;
    float yy = y*(end_y-start_y)/sy + start_y;
    float val = bm.Map(xx,yy);
    val*=mult;
    return val;
  }
private:
  ContinuousBitmap<float> &bm;
  int sx,sy;
  float start_x, end_x;
  float start_y, end_y;
  float mult;
};

class SampleContBoolBitmap : public Bitmap<bool>
{
public:
  SampleContBoolBitmap(ContinuousBitmap<bool> &bm, int sx, int sy, float start_x, float end_x, float start_y, float end_y) : bm(bm), sx(sx), sy(sy), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { bm.Prepare(); }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  bool Map(int x, int y) const
  {
    float xx = x*(end_x-start_x)/sx + start_x;
    float yy = y*(end_y-start_y)/sy + start_y;
    bool val = bm.Map(xx,yy);
    return val;
  }
private:
  ContinuousBitmap<bool> &bm;
  int sx,sy;
  float start_x, end_x;
  float start_y, end_y;
};
class SqrtContFloat_c : public ContinuousBitmap<float>
{
public:
  SqrtContFloat_c(ContinuousBitmap<float> &bm) : bm(bm) {}
  virtual float SizeX() const { return bm.SizeX(); }
  virtual float SizeY() const { return bm.SizeY(); }
  virtual float Map(float x, float y) const
  {
    return sqrt(bm.Map(x,y));
  }
  void Collect(CollectVisitor &vis)
  {
    
  }
  void HeavyPrepare()
  {
  }

  virtual void Prepare()
  {
  }
private:
  ContinuousBitmap<float> &bm;
};
GameApi::CFB GameApi::FloatBitmapApi::SqrtContFloat(CFB a1)
{
  ContinuousBitmap<float> *bm = find_cont_float(e, a1);
  return add_cont_float(e, new SqrtContFloat_c(*bm));
}

GameApi::FB GameApi::FloatBitmapApi::SampleContFloat(CFB a1, float start_x, float end_x, float start_y, float end_y, int sx, int sy, float mult)
{
  ContinuousBitmap<float> *bm = find_cont_float(e, a1);
  return add_float_bitmap(e, new SampleContFloatBitmap(*bm, sx,sy, start_x, end_x, start_y, end_y, mult));
}
GameApi::BB GameApi::FloatBitmapApi::SampleContBool(CBB a1, float start_x, float end_x, float start_y, float end_y, int sx, int sy)
{
  ContinuousBitmap<bool> *bm = find_cont_bool(e, a1);
  return add_bool_bitmap(e, new SampleContBoolBitmap(*bm, sx,sy, start_x, end_x, start_y, end_y));
}

class FuncCont : public ContinuousBitmap<float>
{
public:
  FuncCont(double (*fptr)(double), ContinuousBitmap<float> &arg) : fptr(fptr), arg(arg) { }
  void Collect(CollectVisitor &vis)
  {
    
  }
  void HeavyPrepare()
  {
  }
  void Prepare() {}
  virtual float SizeX() const { return arg.SizeX(); }
  virtual float SizeY() const { return arg.SizeY(); }
  virtual float Map(float x, float y) const
  {
    return fptr(arg.Map(x,y));
  }
private:
  double (*fptr)(double);
  ContinuousBitmap<float> &arg;
};

GameApi::CFB GameApi::FloatBitmapApi::Sin(CFB arg)
{
  ContinuousBitmap<float> *arg2 = find_cont_float(e, arg);
  return add_cont_float(e, new FuncCont(&sin, *arg2));					
}
GameApi::CFB GameApi::FloatBitmapApi::Cos(CFB arg)
{
  ContinuousBitmap<float> *arg2 = find_cont_float(e, arg);
  return add_cont_float(e, new FuncCont(&cos, *arg2));					
}
GameApi::CFB GameApi::FloatBitmapApi::Tan(CFB arg)
{
  ContinuousBitmap<float> *arg2 = find_cont_float(e, arg);
  return add_cont_float(e, new FuncCont(&tan, *arg2));				
}

class IndicatorBitmap : public Bitmap<Color>
{
public:
  IndicatorBitmap(int sx, int sy, int g_ind) : sx(sx), sy(sy), g_ind(g_ind) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    return Color(x*255/sx,y*255/sy,g_ind);
  }
  void Collect(CollectVisitor &vis)
  {
    
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { }

private:
  int sx,sy,g_ind;
};

GameApi::BM GameApi::BitmapApi::Indicator(int sx, int sy, int g_ind)
{
  Bitmap<Color> *bm = new IndicatorBitmap(sx,sy,g_ind);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  return add_bitmap(e, handle);
}

void SaveImage(BufferRef ref, std::string filename);
void GameApi::BitmapApi::save_png(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bbm = find_color_bitmap(handle);
  BufferFromBitmap buf(*bbm);
  buf.Gen();
  BufferRef ref = buf.Buffer();
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  std::string path = home + ".gameapi_builder/";
  SaveImage(ref, path+filename);
  std::ifstream ss((path+filename).c_str());
  char ch;
  std::vector<unsigned char> vec;
  while(ss.get(ch)) { vec.push_back(ch); }
  ss.close();
  int bar = e.add_to_download_bar(filename);
  int ii = e.download_index_mapping(bar);
  e.set_download_data(ii, vec);
  e.set_download_ready(ii);
}
class SavePngML : public MainLoopItem
{
public:
  SavePngML(GameApi::EveryApi &ev, GameApi::BM bm, std::string filename) : ev(ev), bm(bm), filename(filename) { 
    firsttime = true;
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
      ev.bitmap_api.save_png(bm,filename);
  }
  void Prepare() {
      ev.bitmap_api.save_png(bm,filename);
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) { 
      firsttime = false;
    }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  //virtual int shader_id() { return -1; }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::EveryApi &ev;
  GameApi::BM bm;
  std::string filename;
  bool firsttime;
};
  GameApi::ML GameApi::BitmapApi::save_png_ml(GameApi::EveryApi &ev, BM bm, std::string filename)
{
  return add_main_loop(e, new SavePngML(ev, bm, filename));
}

class AvgBitmap : public Bitmap<Color>
{
public:
  AvgBitmap(Bitmap<Color> &bm1, Bitmap<Color> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return std::min(bm1.SizeX(), bm2.SizeX()); }
  virtual int SizeY() const { return std::min(bm1.SizeY(), bm2.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    Color c1 = bm1.Map(x,y);
    Color c2 = bm2.Map(x,y);
    return Color::Interpolate(c1,c2,0.5);
  }
  void Collect(CollectVisitor &vis)
  {
    bm1.Collect(vis);
    bm2.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare()
  {
    bm1.Prepare();
    bm2.Prepare();
  }
private:
  Bitmap<Color> &bm1;
  Bitmap<Color> &bm2;
};
GameApi::BM GameApi::BitmapApi::avg(BM bm1, BM bm2)
{
  BitmapHandle *handle1 = find_bitmap(e, bm1);
  ::Bitmap<Color> *b1 = find_color_bitmap(handle1);
  BitmapHandle *handle2 = find_bitmap(e, bm2);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle2);

  Bitmap<Color> *b = new AvgBitmap(*b1, *b2);

  BitmapColorHandle *handle3 = new BitmapColorHandle;
  handle3->bm = b;
  BM bm = add_bitmap(e, handle3);
  return bm;
}

GameApi::BM GameApi::BitmapApi::fix_edges(BM bm)
{
  BM bm2 = flip_x(bm);
  BM bm3 = flip_y(bm);
  BM bm4 = flip_y(bm2);
  return avg(avg(bm,bm2),avg(bm4,bm3));
}


class DistFB : public Bitmap<float>
{
public:
  DistFB(int sx, int sy, float p_x, float p_y) : sx(sx), sy(sy),px(p_x), py(p_y) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const {
    float xx = x-px;
    float yy = y-py;
    return sqrtf(xx*xx+yy*yy)/std::max(sx,sy);
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare()
  {
  }
  void Prepare() { }
private:
  int sx,sy;
  float px,py;
};

GameApi::FB GameApi::FloatBitmapApi::dist(int sx, int sy, float p_x, float p_y)
{
  return add_float_bitmap(e, new DistFB(sx,sy,p_x,p_y));
}


class BlurBitmap : public Bitmap<float>
{
public:
  BlurBitmap(Bitmap<float> &fb, float d) : fb(fb), d(d) {
  }
  int SizeX() const { return fb.SizeX(); }
  int SizeY() const { return fb.SizeY(); }
  float Map(int x, int y) const {
    float val = fb.Map(x,y);
    val/=d;
    val*=3.0;
    float xx = -(val*val)/(val*val+1.0)+1.0;
    return xx;
  }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  void Prepare() { fb.Prepare(); }
private:
  Bitmap<float> &fb;
  float d;
};


GameApi::FB GameApi::FloatBitmapApi::blur_bitmap(FB fb, float d)
{
  Bitmap<float> *ffb = find_float_bitmap(e, fb)->bitmap;
  return add_float_bitmap(e, new BlurBitmap(*ffb, d));
}

class FBToIbm : public Bitmap<int>
{
public:
  FBToIbm(Bitmap<float> &fb, float start, float d) : fb(fb), start(start), d(d) { }
  virtual int SizeX() const { return fb.SizeX(); }
  virtual int SizeY() const { return fb.SizeY(); }
  virtual int Map(int x, int y) const
  {
    float val = fb.Map(x,y);
    val-=start;
    val/=d;
    return int(val);
  }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare()
  {
    fb.Prepare();
  }

private:
  Bitmap<float> &fb;
  float start;
  float d;
};

GameApi::IBM GameApi::BitmapApi::convert_fb_to_ibm_bitmap(FB fb, float start, float d)
{
  Bitmap<float> *ffb = find_float_bitmap(e, fb)->bitmap;
  return add_int_bitmap(e, new FBToIbm(*ffb, start, d));
}

class IBMToVX : public Voxel<int>
{
public:
  IBMToVX(Bitmap<int> *bi) : bi(bi) {}
  void Collect(CollectVisitor &vis)
  {
    bi->Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  void Prepare() { bi->Prepare(); }
  virtual int SizeX() const { 
    return bi->SizeX(); 
  }
  virtual int SizeY() const { 
    return bi->SizeY(); 
  }
  virtual int SizeZ() const { 
    return 1; }
  virtual int Map(int x, int y, int z) const
  {
    return bi->Map(x,y);
  }
private:
  Bitmap<int> *bi;
};

GameApi::VX GameApi::BitmapApi::convert_ibm_to_vx(IBM bm)
{
  Bitmap<int> *bi = find_int_bitmap(e, bm);
  return add_int_voxel(e, new IBMToVX(bi));
}

struct Ret {
  int sx;
  int sy;
  char *buffer;
  std::string characters;
};
Ret bitmapparser(unsigned char *buf)
{
  std::string buf3((const char*)buf);
  std::stringstream ss(buf3);
  int sx;
  int sy;
  ss >> sx;
  ss >> sy;
  std::string str;
  std::getline(ss,str);
  std::getline(ss,str);

  char *buf2 = new char[sx*sy];
  char ch;
  for(int y=0;y<sy;y++)
    {
    for(int x=0;x<sx;x++)
      {
	ss >> ch;
	buf2[x+y*sx] = ch;
      }
    //ss >> ch; // eat \n
    //ss >> ch; // eat \r
    }
  Ret ret;
  ret.sx = sx;
  ret.sy = sy;
  ret.buffer = buf2;
  ret.characters = str;
  std::cout << "IntBitmapLoader stats: " << sx << " " << sy << " " << str << std::endl;
  return ret;
}

class IntBitmapLoader : public Bitmap<int>
{
public:
  IntBitmapLoader(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url), homepage(homepage),buf(0),sx(0),sy(0), chars("@") { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int Map(int x, int y) const
  {
    int s = chars.size();
    unsigned char c = buf[x+sx*y];
    int val = -1;
    for(int i=0;i<s;i++)
      {
	if (chars[i]==c) { val=i; break; }
      }
    return val;
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }

  void Prepare() {
    if (!buf) {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "intbitmap_loader async not ready yet, failing..." << std::endl;
      return;
     }
    Ret r = bitmapparser(const_cast<unsigned char*>(&ptr->operator[](0)));
    sx = r.sx;
    sy = r.sy;
    buf = r.buffer;
    chars = r.characters;
    }
  }
  ~IntBitmapLoader() { delete [] buf; }
private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  char *buf;
  int sx;
  int sy;
  std::string chars;
};

extern std::string gameapi_homepageurl;

GameApi::IBM GameApi::BitmapApi::intbitmap_loader(std::string url)
{
  return add_int_bitmap(e, new IntBitmapLoader(e,url, gameapi_homepageurl));
}

unsigned int color_array[] = { 0x00000000, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xff00ffff, 0xffffff00, 0xffffffff };

class IntBitmap_BM : public Bitmap<Color>
{
public:
  IntBitmap_BM(Bitmap<int> &bm) : bm(bm) { }
  int SizeX() const { return bm.SizeX(); }
  int SizeY() const { return bm.SizeY(); }
  Color Map(int x, int y) const
  {
    int val = bm.Map(x,y);
    int end = sizeof(color_array)/sizeof(unsigned int);
    if (val<0 ||val>=end) return Color(0xffffffff);
    unsigned int color = color_array[val];
    return Color(color);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { bm.Prepare(); }
private:
  Bitmap<int> &bm;
};

GameApi::BM GameApi::BitmapApi::intbitmap_bm(IBM ibm)
{
  Bitmap<int> *bi = find_int_bitmap(e, ibm);
  return add_color_bitmap(e, new IntBitmap_BM(*bi));
}

class PlusBitmap : public Bitmap<Color>
{
public:
  PlusBitmap(Bitmap<Color> &bm1, Bitmap<Color> &bm2) : bm1(bm1), bm2(bm2) { }
  virtual int SizeX() const { return std::min(bm1.SizeX(),bm2.SizeX()); }
  virtual int SizeY() const { return std::min(bm1.SizeY(),bm2.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    Color c1 = bm1.Map(x,y);
    Color c2 = bm2.Map(x,y);
    int r = c1.r + c2.r;
    int g = c1.g + c2.g;
    int b = c1.b + c2.b;
    if (r>255) r=255;
    if (g>255) g=255;
    if (b>255) b=255;
    int a = (c1.alpha+c2.alpha)/2;
    return Color(r,g,b,a);
  }
    void Collect(CollectVisitor &vis)
  {
    bm1.Collect(vis);
    bm2.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  virtual void Prepare()
  {
    bm1.Prepare();
    bm2.Prepare();
  }

private:
  Bitmap<Color> &bm1;
  Bitmap<Color> &bm2;
};

GameApi::BM GameApi::BitmapApi::plus_bitmap(BM bm, BM bm2)
{
  BitmapHandle *bm_h = find_bitmap(e,bm);
  BitmapHandle *bm2_h = find_bitmap(e,bm2);
  Bitmap<Color> *bbm = find_color_bitmap(bm_h);
  Bitmap<Color> *bbm2 = find_color_bitmap(bm2_h);
  return add_color_bitmap(e, new PlusBitmap(*bbm,*bbm2));
}

class GradientFB : public Bitmap<float>
{
public:
  GradientFB(int sx, int sy, float val, float val2, bool flip) : sx(sx), sy(sy), val(val), val2(val2), flip(flip) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual float Map(int x, int y) const
  {
    float p;
    if (flip) { p = float(x)/float(sx);  }
    else { p=float(y)/float(sy); }
    p = p*val2+(1.0-p)*val;
    return p;
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare()
  {
  }

  virtual void Prepare() { }
private:
  int sx, sy;
  float val,val2;
  bool flip;
};

GameApi::FB GameApi::BitmapApi::gradient_fb(int sx, int sy, float val, float val2, bool flip)
{
  return add_float_bitmap(e, new GradientFB(sx,sy,val,val2,flip));
}

class RadialFB : public Bitmap<float>
{
public:
  RadialFB(int sx, int sy, float x, float y, float r, float val_at_zero, float val_at_r) : sx(sx), sy(sy), m_x(x),m_y(y), r(r), val_at_zero(val_at_zero), val_at_r(val_at_r) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual float Map(int x, int y) const
  {
    float xx = x-m_x;
    float yy = y-m_y;
    float d = sqrt(xx*xx+yy*yy);
    d/=r;
    return d*val_at_r + (1.0-d)*val_at_zero;
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { }

private:
  int sx, sy;
  float m_x, m_y;
  float mult;
  float r;
  float val_at_zero;
  float val_at_r;
};


GameApi::FB GameApi::BitmapApi::radial_fb(int sx, int sy, float x, float y, float r, float val_at_zero, float val_at_r)
{
  return add_float_bitmap(e, new RadialFB(sx,sy, x,y, r, val_at_zero, val_at_r));
}

class SinFB : public Bitmap<float>
{
public:
  SinFB(Bitmap<float> &fb) : fb(fb) { }
  virtual int SizeX() const { return fb.SizeX(); }
  virtual int SizeY() const { return fb.SizeY(); }
  virtual float Map(int x, int y) const
  {
    return sin(fb.Map(x,y));
  }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { fb.Prepare(); }
private:
  Bitmap<float> &fb;
};

GameApi::FB GameApi::BitmapApi::sin_fb(FB gradient)
{
  Bitmap<float> *fb = find_float_bitmap(e, gradient)->bitmap;
  return add_float_bitmap(e, new SinFB(*fb));
}

class PlusFB : public Bitmap<float>
{
public:
  PlusFB(Bitmap<float> &b1, Bitmap<float> &b2) : b1(b1), b2(b2) { }
  virtual int SizeX() const { return std::min(b1.SizeX(),b2.SizeX()); }
  virtual int SizeY() const { return std::min(b1.SizeY(),b2.SizeY()); }
  virtual float Map(int x, int y) const
  {
    return b1.Map(x,y)+b2.Map(x,y);
  }
  void Collect(CollectVisitor &vis)
  {
    b1.Collect(vis); b2.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  virtual void Prepare() { b1.Prepare(); b2.Prepare(); }
private:
  Bitmap<float> &b1, &b2;
};


GameApi::FB GameApi::BitmapApi::plus_fb(FB f1, FB f2)
{
  Bitmap<float> *fb1 = find_float_bitmap(e, f1)->bitmap;
  Bitmap<float> *fb2 = find_float_bitmap(e, f2)->bitmap;
  return add_float_bitmap(e, new PlusFB(*fb1,*fb2));
}

class MulFB : public Bitmap<float>
{
public:
  MulFB(Bitmap<float> *f, float mul) : f(f),mul(mul) {}
  void Collect(CollectVisitor &vis)
  {
    f->Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  void Prepare() { f->Prepare(); }
  int SizeX() const { return f->SizeX(); }
  int SizeY() const { return f->SizeY(); }
  float Map(int x, int y) const { return mul*f->Map(x,y); }
private:
  Bitmap<float> *f;
  float mul;
};
GameApi::FB GameApi::BitmapApi::mul_fb(FB f, float mul)
{
  Bitmap<float> *fb1 = find_float_bitmap(e, f)->bitmap;
  return add_float_bitmap(e, new MulFB(fb1, mul));
};
class CubeMapBitmap : public Bitmap<Color>
{
public:
  CubeMapBitmap(Bitmap<Color> &left, Bitmap<Color> &top, Bitmap<Color> &middle, Bitmap<Color> &right, Bitmap<Color> &back, Bitmap<Color> &down) : left(left), top(top), middle(middle), right(right), back(back), down(down) { }
  void Collect(CollectVisitor &vis)
  {
    left.Collect(vis);
    top.Collect(vis);
    middle.Collect(vis);
    right.Collect(vis);
    back.Collect(vis);
    down.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { left.Prepare(); top.Prepare(); middle.Prepare(); right.Prepare(); back.Prepare(); down.Prepare(); }
  int SizeX() const { return left.SizeX()+middle.SizeX()+right.SizeX()+back.SizeX(); }
  int SizeY() const { return top.SizeY()+middle.SizeY()+down.SizeY(); }
  Color Map(int x, int y) const {
    if (y>=0 && y<top.SizeY()) { // top
      if (x>=left.SizeX() && x<left.SizeX()+top.SizeX()) {
	// top
	return top.Map(x-left.SizeX(), y);
      }
    }

    if (y>=top.SizeY()&&y<top.SizeY()+middle.SizeY()) {
      if (x>=0 && x<left.SizeX()) {
	// left
	return left.Map(x,y-top.SizeY());
      }
      if (x>=left.SizeX() && x<left.SizeX()+middle.SizeX())
	{ // middle
	  return middle.Map(x-left.SizeX(), y-top.SizeY());
	}
      if (x>=left.SizeX()+middle.SizeX() && x<left.SizeX()+middle.SizeX()+right.SizeX()) { // right
	return right.Map(x-left.SizeX()-middle.SizeX(), y-top.SizeY());
      }
      if (x>=left.SizeX()+middle.SizeX()+right.SizeX()) {
	// back
	return back.Map(x-left.SizeX()-middle.SizeX()-right.SizeX(), y-top.SizeY());
      }
    }
    if (y>=top.SizeY()+middle.SizeY()) {
      if (x>=left.SizeX() && x<left.SizeX()+middle.SizeX()) {
	// down
	return down.Map(x-left.SizeX(),y-top.SizeX()-middle.SizeY());
      }
    }
    return Color(0x0);
  }

private:
  Bitmap<Color> &left, &top, &middle, &right, &back, &down;
};

GameApi::BM GameApi::BitmapApi::bm_cubemap(EveryApi &ev, BM left, BM top, BM middle, BM right, BM back, BM down, int sx, int sy)
{
  BM l = scale_bitmap(ev, left, sx,sy);
  BM t = scale_bitmap(ev, top, sx, sy);
  BM m = scale_bitmap(ev, middle, sx,sy);
  BM r = scale_bitmap(ev, right, sx, sy);
  BM b = scale_bitmap(ev, back, sx,sy);
  BM d = scale_bitmap(ev, down, sx,sy);
  
  BitmapHandle *handle_l = find_bitmap(e, l);
  ::Bitmap<Color> *b2_l = find_color_bitmap(handle_l);
  BitmapHandle *handle_t = find_bitmap(e, t);
  ::Bitmap<Color> *b2_t = find_color_bitmap(handle_t);
  BitmapHandle *handle_m = find_bitmap(e, m);
  ::Bitmap<Color> *b2_m = find_color_bitmap(handle_m);
  BitmapHandle *handle_r = find_bitmap(e, r);
  ::Bitmap<Color> *b2_r = find_color_bitmap(handle_r);
  BitmapHandle *handle_b = find_bitmap(e, b);
  ::Bitmap<Color> *b2_b = find_color_bitmap(handle_b);
  BitmapHandle *handle_d = find_bitmap(e, d);
  ::Bitmap<Color> *b2_d = find_color_bitmap(handle_d);

  Bitmap<Color> *res = new CubeMapBitmap(*b2_l, *b2_t, *b2_m, *b2_r, *b2_b, *b2_d);
  return add_color_bitmap(e, res);
}

class SubBitmap2 : public Bitmap<Color>
{
public:
  SubBitmap2(Bitmap<Color> &bm, int choose) : bm(bm), choose(choose) { }
  int BaseX() const { return bm.SizeX(); }
  int BaseY() const { return bm.SizeY(); }

  int Val() const { return std::min(BaseX()/4,BaseY()/3); }

  int LeftX() const { return 0; }
  int LeftY() const { return Val(); }
  int TopX() const { return Val(); }
  int TopY() const { return 0; }
  int FrontX() const { return Val(); }
  int FrontY() const { return Val(); }
  int BottomX() const { return Val(); }
  int BottomY() const { return Val()*2; }
  int RightX() const { return Val()*2; }
  int RightY() const { return Val(); }
  int BackX() const { return Val()*3; }
  int BackY() const { return Val(); }
  
  int SizeX() const { return Val(); }
  int SizeY() const { return Val(); }
  
  Color Map(int mx, int my) const
  {
    float pos_x = 0.0, pos_y = 0.0;
    switch(choose) {
    case 0: pos_x = RightX(); pos_y = RightY(); break;
    case 1: pos_x = LeftX(); pos_y = LeftY(); break;
    case 2: pos_x = TopX(); pos_y = TopY(); break;
    case 3: pos_x = BottomX(); pos_y = BottomY(); break;
    case 4: pos_x = BackX(); pos_y = BackY(); break;
    case 5: pos_x = FrontX(); pos_y = FrontY(); break;
    };
    return bm.Map(mx+pos_x, my+pos_y);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { bm.Prepare(); }
private:
  Bitmap<Color> &bm;
  int choose;
};


GameApi::ARR GameApi::BitmapApi::cubemap(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *BM = find_color_bitmap(handle);
  Bitmap<Color> *R0 = new SubBitmap2(*BM, 0);
  Bitmap<Color> *R = new FlipBitmap(*R0, true,false);
  Bitmap<Color> *L0 = new SubBitmap2(*BM, 1);
  Bitmap<Color> *L = new FlipBitmap(*L0, true,false);
  Bitmap<Color> *T0 = new SubBitmap2(*BM, 2);
  Bitmap<Color> *T = new FlipBitmap(*T0, false,true);

  Bitmap<Color> *Bot0 = new SubBitmap2(*BM, 3);
  Bitmap<Color> *Bot = new FlipBitmap(*Bot0, false,true);
  Bitmap<Color> *Back0 = new SubBitmap2(*BM, 4);
  Bitmap<Color> *Back = new FlipBitmap(*Back0, true,false);
  Bitmap<Color> *F0 = new SubBitmap2(*BM, 5);
  Bitmap<Color> *F = new FlipBitmap(*F0, true,false);

  BitmapColorHandle *handle_r = new BitmapColorHandle;
  handle_r->bm = R;
  GameApi::BM bm_r = add_bitmap(e, handle_r);

  BitmapColorHandle *handle_l = new BitmapColorHandle;
  handle_l->bm = L;
  GameApi::BM bm_l = add_bitmap(e, handle_l);

  BitmapColorHandle *handle_t = new BitmapColorHandle;
  handle_t->bm = T;
  GameApi::BM bm_t = add_bitmap(e, handle_t);

  BitmapColorHandle *handle_bot = new BitmapColorHandle;
  handle_bot->bm = Bot;
  GameApi::BM bm_bot = add_bitmap(e, handle_bot);

  BitmapColorHandle *handle_back = new BitmapColorHandle;
  handle_back->bm = Back;
  GameApi::BM bm_back = add_bitmap(e, handle_back);

  BitmapColorHandle *handle_f = new BitmapColorHandle;
  handle_f->bm = F;
  GameApi::BM bm_f = add_bitmap(e, handle_f);

  ArrayType *array = new ArrayType;
  array->type=0;
  array->vec.push_back(bm_r.id);
  array->vec.push_back(bm_l.id);
  array->vec.push_back(bm_t.id);
  array->vec.push_back(bm_bot.id);
  array->vec.push_back(bm_back.id);
  array->vec.push_back(bm_f.id);
  //std::cout << "CUBEMAP SIZE:" << array->vec.size() << std::endl;
  return add_array(e, array);
}

class NoiseVectors : public Bitmap<Color>
{
public:
  NoiseVectors(int sx, int sy) : sx(sx), sy(sy) {
    if (sx<=0) sx=1;
    if (sy<=0) sy=1;
    xp_arr = new float[sx*sy];
    yp_arr = new float[sx*sy];
    for(int i=0;i<sx*sy;i++) xp_arr[i]=-60000.0;
    for(int i=0;i<sx*sy;i++) yp_arr[i]=-60000.0;
  }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  
  Color Map(int mx, int my) const
  {
    if (mx<0||mx>=sx) return Color(0.0,0.0,0.0);
    if (my<0||my>=sy) return Color(0.0,0.0,0.0);
    Random r;
    float xp = xp_arr[mx+my*sx];
    float yp = yp_arr[mx+my*sx];
    if (xp<-50000.0)
      {
	xp = double(r.next())/r.maximum();
	xp_arr[mx+my*sx]=xp;
      }
    if (yp<-50000.0) 
      {
	yp = double(r.next())/r.maximum();
	yp_arr[mx+my*sy]=yp;
      }
    xp*=3.14159;
    yp*=3.14159*2.0;
    float x = 127*sin(xp)*cos(yp);
    float y = 127*sin(xp)*sin(yp);
    float z = 127*cos(xp);
    Color c2(128+int(x),128+int(y),128+int(z),255);
    return c2;    
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { }
private:
  int sx,sy;
  float *xp_arr;
  float *yp_arr;
};

GameApi::BM GameApi::BitmapApi::noise_vectors(int sx, int sy)
{
  Bitmap<Color> *b = new NoiseVectors(sx,sy);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;

}

class FractalMountain : public ContinuousBitmap<float>
{
public:
  FractalMountain(int level) : level(level) {
    for(int i=0;i<level;i++) {
      vec.push_back(new NoiseVectors(pow(2,i),pow(2,i)));
    }
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual float SizeX() const { return 1.0; }
  virtual float SizeY() const { return 1.0; }
  virtual float Map(float x, float y) const
  {
    float res=0.0;
    for(int i=0;i<level;i++)
      {
	res+=vec[i]->Map(x*vec[i]->SizeX(),y*vec[i]->SizeY()).rf()/level/level;
      }
    return res;
  }
  virtual void Prepare() { }
private:
  int level;
  std::vector<Bitmap<Color> *> vec;
};

EXPORT GameApi::FB GameApi::PolygonApi::fractal_mountain(int level, int sx, int sy)
{
  if (level>10) level=10;
  ContinuousBitmap<float> *bm = new FractalMountain(level);
  Bitmap<float> *bbm = new BitmapFromContinuousBitmap<float>(*bm,sx,sy);
  FB bm2 = add_float_bitmap(e, bbm);
  return bm2;
}


class BumpMap : public Bitmap<Color>
{
public:
  BumpMap(Bitmap<float> &values, float h) : values(values),h(h) { }
  void Collect(CollectVisitor &vis)
  {
    values.Collect(vis);
  }
  void HeavyPrepare()
  {
  }

  void Prepare() { values.Prepare(); }
  int SizeX() const { return values.SizeX(); }
  int SizeY() const { return values.SizeY(); }
  Color Map(int x, int y) const
  {
    float v_r = x+h/2>=SizeX()?values.Map(SizeX()-1,y):values.Map(x+h/2.0,y);
    float v_l = x-h/2<0?values.Map(0,y):values.Map(x-h/2.0,y);
    float v_t = y-h/2<0?values.Map(x,0):values.Map(x,y-h/2.0);
    float v_b = y+h/2>=SizeY()?values.Map(x,SizeY()-1):values.Map(x,y+h/2.0);
    float angle_x = atan((v_r-v_l)/h);
    float angle_y = atan((v_b-v_t)/h);
    Point p(0.0,1.0,0.0);
    Matrix m_x = Matrix::ZRotation(angle_x);
    Matrix m_z = Matrix::XRotation(angle_y);
    Point res = p*m_x*m_z;
    Vector v(res);
    v/=v.Dist();
    if (std::isnan(v.dx)) v.dx=0.0;
    if (std::isnan(v.dy)) v.dy=0.0;
    if (std::isnan(v.dz)) v.dz=0.0;
    int r = v.dx*127.5+127.5;
    int g = v.dy*127.5+127.5;
    int b = v.dz*127.5+127.5;
    r<<=16;
    g<<=8;
    return Color(0xff000000+r+g+b);
  }
private:
  Bitmap<float> &values;
  float h;
};

GameApi::BM GameApi::BitmapApi::bump_map(FB fb, float h)
{
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  Bitmap<Color> *b = new BumpMap(*f,h);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}

class CreateIntBitmap : public Bitmap<int>
{
public:
  CreateIntBitmap(std::vector<Bitmap<bool>*> vec) : vec(vec) { }
  virtual int SizeX() const
  {
    int s = vec.size();
    int m = 99999999;
    for(int i=0;i<s;i++) if (vec[i]->SizeX()<m) m = vec[i]->SizeX();
    return m;
  }
  virtual int SizeY() const
  {
    int s = vec.size();
    int m = 99999999;
    for(int i=0;i<s;i++) if (vec[i]->SizeY()<m) m = vec[i]->SizeY();
    return m;
  }
  virtual int Map(int x, int y) const
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	if (vec[i]->Map(x,y)) return i;
      }
    return -1;
  }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Collect(vis);
      }

  }
  void HeavyPrepare()
  {
  }

  virtual void Prepare()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Prepare();
      }
  }
private:
  std::vector<Bitmap<bool>*> vec;
};

GameApi::IBM GameApi::BitmapApi::create_ibm(std::vector<BB> vec)
{
  int s = vec.size();
  std::vector<Bitmap<bool>*> vec2;
  for(int i=0;i<s;i++) {
    vec2.push_back(find_bool_bitmap(e,vec[i])->bitmap );
  }
  return add_int_bitmap(e, new CreateIntBitmap(vec2));
}

class ChooseBM : public Bitmap<bool>
{
public:
  ChooseBM(Bitmap<int> *bm, int val) : bm(bm), val(val) { }
  virtual int SizeX() const { return bm->SizeX(); }
  virtual int SizeY() const { return bm->SizeY(); }
  virtual bool Map(int x, int y) const
  {
    return bm->Map(x,y)==val;
  }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare()
  {
    bm->Prepare();
  }
private:
  Bitmap<int> *bm;
  int val;
};

GameApi::BB GameApi::BitmapApi::choose_bool(IBM bm, int val)
{
  Bitmap<int> *bm2 = find_int_bitmap(e, bm);
  return add_bool_bitmap(e, new ChooseBM(bm2,val));
}

GameApi::ARR GameApi::BitmapApi::choose_ints(IBM bm, int count)
{
  std::vector<int> bms;
  for(int i=0;i<count;i++)
    {
      BB bb = choose_bool(bm, i);
      bms.push_back(bb.id);
    }
  ArrayType *val = new ArrayType;
  val->type = E_BM;
  val->vec = bms;
  return add_array(e, val);
}


class ColorBM : public Bitmap<Color>
{
public:
  ColorBM(Bitmap<Color> &bm, unsigned int color) : bm(bm), color(color) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    unsigned int cc = c.Pixel();
    unsigned int cc2 = Color::RangeChange(cc,
					  0xffffffff, 0xff000000,
					  color|0xff000000, 0xff000000
					  );
    return Color(cc2);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { bm.Prepare(); }

private:
  Bitmap<Color> &bm;
  unsigned int color;
};

GameApi::BM GameApi::BitmapApi::color_bm(BM bm, unsigned int color)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  ::Bitmap<Color> *res = new ColorBM(*b2, color);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = res;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}

class CubeColorBool : public ColorBool
{
public:
  CubeColorBool(unsigned int c1, unsigned int c2) : c1(c1), c2(c2) { 
    a1 = c1 &0xff000000;
    r1 = c1 &0xff0000;
    g1 = c1 &0xff00;
    b1 = c1 &0xff;

    a2 = c2 &0xff000000;
    r2 = c2 &0xff0000;
    g2 = c2 &0xff00;
    b2 = c2 &0xff;
  }
  bool is_included(unsigned int color) const
  {
    unsigned int a = color &0xff000000;
    unsigned int r = color &0xff0000;
    unsigned int g = color &0xff00;
    unsigned int b = color &0xff;
    if (a<a1 ||a>a2) return false;
    if (r<r1 ||r>r2) return false;
    if (g<g1 ||g>g2) return false;
    if (b<b1 ||b>b2) return false;
    return true;
  }
private:
  unsigned int c1,c2;
  unsigned int a1,a2;
  unsigned int r1,r2;
  unsigned int g1,g2;
  unsigned int b1,b2;
};

class ChooseBitmapColours : public Bitmap<bool>
{
public:
  ChooseBitmapColours(Bitmap<Color> &bm, ColorBool *bools) : bm(bm), bools(bools) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    unsigned int cc = c.Pixel();
    return bools->is_included(cc);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { bm.Prepare(); }

private:
  Bitmap<Color> &bm;
  ColorBool *bools;
};

GameApi::BB GameApi::BitmapApi::choose_color(BM bm, unsigned int c1, unsigned int c2)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bbm = find_color_bitmap(handle);
  ColorBool *bools = new CubeColorBool(c1,c2);
  return add_bool_bitmap(e, new ChooseBitmapColours(*bbm, bools));
}


std::vector<float> *g_median_float;

bool MedianCompare(int x, int y)
{
  return g_median_float->operator[](x)<g_median_float->operator[](y);
}

class MedianFilter : public Bitmap<Color>
{
public:
  MedianFilter(Bitmap<Color> &bm, int sx, int sy) : bm(bm),sx(sx),sy(sy) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  float value(unsigned int c) const
  {
    unsigned int r = c&0xff0000;
    unsigned int g = c&0x00ff00;
    unsigned int b = c&0x0000ff;
    r>>=16;
    g>>=8;
    return (float(r)+float(g)+float(b))/3.0/256.0;
  }

  virtual Color Map(int x, int y) const
  {
    std::vector<float> c;
    std::vector<int> xxx;
    std::vector<int> yyy;
    for(int yy=-sy;yy<=sy;yy++)
      for(int xx=-sx;xx<=sx;xx++)
	{
	  unsigned int cc;
	  if (x+xx>=0 && x+xx<bm.SizeX() && y+yy>=0 && y+yy<bm.SizeY())
	    {
	      cc = bm.Map(x+xx,y+yy).Pixel();
	    }
	  else 
	    {
	      cc=0x00000000;
	    }
	  c.push_back(value(cc));
	  xxx.push_back(x+xx);
	  yyy.push_back(y+yy);
	}
    std::vector<int> indexes;
    int s = c.size();
    if (!s) return Color(0x0);
    for(int i=0;i<s;i++) indexes.push_back(i);
    g_median_float = &c;
    std::sort(indexes.begin(), indexes.end(), &MedianCompare);
    int pos = s/2;
    int xx = xxx[indexes[pos]];
    int yy = yyy[indexes[pos]];
    if (xx>=0 && xx<bm.SizeX() && yy>=0 && yy<bm.SizeY())
      return bm.Map(xx,yy);
    return Color(0x00000000);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { bm.Prepare(); }
private:
  Bitmap<Color> &bm;
  int sx,sy;
};
GameApi::BM GameApi::BitmapApi::median_filter(BM bm, int sx, int sy)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  return add_color_bitmap(e, new MedianFilter(*b2, sx,sy));
}

class CalcLight : public Bitmap<Color>
{
public:
  CalcLight(FaceCollection *coll, FaceCollection *coll2, Bitmap<Color> *texture, int count, Point light_pos, float shadow_darkness, float softness) : coll(coll), coll2(coll2), texture(texture), count(count), light_pos(light_pos), shadow_darkness(shadow_darkness), softness(softness) { firsttime = true; }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }

  virtual void Prepare()
  {
    if (firsttime) {
      firsttime=false;
    coll->Prepare();
    coll2->Prepare();
    texture->Prepare();
    
    // decide size
    sx = texture->SizeX();
    sy = texture->SizeY();
    shadow = new char[sx*sy];
    done = new bool[sx*sy];

    // clear texture
    char *sh = shadow;
    bool *dn = done;
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  *sh = 0;
	  sh++;
	  *dn = false;
	  dn++;
	}
    // calculate light
    Random r;
    for(int i=0;i<count;i++) {
      int s = coll->NumFaces();
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      float zp = double(r.next())/r.maximum();
      xp*=2.0;
      yp*=2.0;
      xp-=1.0;
      yp-=1.0;
      zp*=float(s);
      int zpi = int(zp);
      if (zpi<0) zpi=0;
      if (zpi>=s) zpi=s-1;
      int num = coll->NumPoints(zpi);
      if (num!=4 && num!=3) { std::cout << "Error Quad: " << num << std::endl; }
      if (num==4) {
	Point pp1 = coll->FacePoint(zpi,0);
	Point pp2 = coll->FacePoint(zpi,1);
	Point pp3 = coll->FacePoint(zpi,2);
	Point pp4 = coll->FacePoint(zpi,3);
	Point2d p1 = coll->TexCoord(zpi, 0);
	Point2d p2 = coll->TexCoord(zpi, 1);
	Point2d p3 = coll->TexCoord(zpi, 2);
	Point2d p4 = coll->TexCoord(zpi, 3);
	Point pp = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(pp1) + (1.0f+xp)*(1.0f-yp)*Vector(pp2) + (1.0f+xp)*(1.0f+yp)*Vector(pp3) + (1.0f-xp)*(1.0f+yp)*Vector(pp4));

	Point2d p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*p1 + (1.0f+xp)*(1.0f-yp)*p2 + (1.0f+xp)*(1.0f+yp)*p3 + (1.0f-xp)*(1.0f+yp)*p4);
	if (std::isnan(p.x)||std::isnan(p.y)) continue;
	if (std::isnan(pp.x)||std::isnan(pp.y)||std::isnan(pp.z)) continue;
	int x = p.x*sx;
	int y = p.y*sy;
	if (x<0||x>=sx) continue;
	if (y<0||y>=sy) continue;
	if (done[x+y*sx]==false) {
	  int count = 0;
	  //if (is_nearest(light_pos+Vector(softness,0.0,0.0), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(-softness,0.0,0.0), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(0.0,0.0,softness), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(0.0,0.0,-softness), pp,zpi)) count++;
	  if (!is_nearest(light_pos, pp,zpi)) count+=4;
	  shadow[x+y*sx] = count;
	  if (x>0 && x<sx-1 && y>0 && y<sy-1) {
	  shadow[(x+1)+y*sx] = count;
	  shadow[x+(y+1)*sx] = count;
	  shadow[(x-1)+y*sx] = count;
	  shadow[x+(y-1)*sx] = count;
	  }
	  //shadow[x+y*sx] = true;
	}
	done[x+y*sx]=true;
      } else if (num==3) {
	Point pp1 = coll->FacePoint(zpi,0);
	Point pp2 = coll->FacePoint(zpi,1);
	Point pp3 = coll->FacePoint(zpi,2);

	Point2d p1 = coll->TexCoord(zpi,0);
	Point2d p2 = coll->TexCoord(zpi,1);
	Point2d p3 = coll->TexCoord(zpi,2);
	float r1 = double(r.next())/r.maximum();
	float r2 = double(r.next())/r.maximum();
	Point pp = Point((1.0-sqrt(r1))*Vector(pp1) + (sqrt(r1)*(1.0-r2))*Vector(pp2) + (r2*sqrt(r1))*Vector(pp3));
	Point2d p = ((1.0-sqrt(r1))*p1 + (sqrt(r1)*(1.0-r2))*p2 + (r2*sqrt(r1))*p3);
	if (std::isnan(p.x)||std::isnan(p.y)) continue;
	if (std::isnan(pp.x)||std::isnan(pp.y)||std::isnan(pp.z)) continue;
	int x = p.x*sx;
	int y = p.y*sy;
	if (x<0||x>=sx) continue;
	if (y<0||y>=sy) continue;

	if (done[x+y*sx]==false ) {
	  int count = 0;
	  //if (is_nearest(light_pos+Vector(softness,0.0,0.0), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(-softness,0.0,0.0), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(0.0,0.0,softness), pp,zpi)) count++;
	  //if (is_nearest(light_pos+Vector(0.0,0.0,-softness), pp,zpi)) count++;
	  if (!is_nearest(light_pos, pp,zpi)) count+=4;
	  shadow[x+y*sx] = count;
	  if (x>0 && x<sx-1 && y>0 && y<sy-1) {
	  shadow[(x+1)+y*sx] = count;
	  shadow[x+(y+1)*sx] = count;
	  shadow[(x-1)+y*sx] = count;
	  shadow[x+(y-1)*sx] = count;
	  }
	}
	done[x+y*sx]=true;
      }
    }
    }
  }

  bool is_nearest(Point light_pos, Point pp, int zpi) const
  {
    //float rr = 100000.0;
    //int zp = -1;
    int s = coll2->NumFaces();
    for(int i=0;i<s;i++) {
      int p = coll2->NumPoints(i);
      if (p==3) {
	Point p1 = coll2->FacePoint(i,0);
	Point p2 = coll2->FacePoint(i,1);
	Point p3 = coll2->FacePoint(i,2);
	LineProperties lp(light_pos,pp);
	float r = 0.0;
	bool b = lp.TriangleIntersection(p1,p2,p3,r);
	//std::cout << r << std::endl;
	if (b) { if (r>0.0 && r<0.98) return false; }
      } else if (p==4)
	{
	Point p1 = coll2->FacePoint(i,0);
	Point p2 = coll2->FacePoint(i,1);
	Point p3 = coll2->FacePoint(i,2);
	Point p4 = coll2->FacePoint(i,3);
	LineProperties lp(light_pos,pp);
	float r = 0.0;
	bool b = lp.QuadIntersection(p1,p2,p3,p4,r);
	//std::cout << r << std::endl;
	if (b) { if (r>0.0 && r<0.98) return false; }
	}
    }
    return true;
  }

  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    int count = shadow[x+y*sx];
    if (!shadow[x+y*sx]) return texture->Map(x,y);
    Color c = texture->Map(x,y);
    float depth = (count)/8.0; // [0..1]
    depth*=(1.0-shadow_darkness);
    depth+=shadow_darkness;
    c.r = c.r*depth;
    c.g = c.g*depth;
    c.b = c.b*depth;
    return c;
  }

private:
  FaceCollection *coll;
  FaceCollection *coll2;
  Bitmap<Color> *texture;
  int sx;
  int sy;
  char *shadow;
  bool *done;
  int count;
  Point light_pos;
  float shadow_darkness;
  float softness;
  bool firsttime;
};

GameApi::BM GameApi::BitmapApi::calculate_baked_light(P p, P p2, BM texture, int count, float light_pos_x, float light_pos_y, float light_pos_z, float shadow_darkness, float softness)
{
  FaceCollection *coll = find_facecoll(e,p);
  FaceCollection *coll2 = find_facecoll(e,p2);
  BitmapHandle *handle = find_bitmap(e, texture);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  Bitmap<Color> *b = new CalcLight(coll, coll2, b2, count, Point(light_pos_x, light_pos_y, light_pos_z), shadow_darkness, softness);

  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}

class ScaleToSize : public Bitmap<Color>
{
public:
  ScaleToSize(Bitmap<Color> &bm, int sz) : bm(bm), sz(sz) { }
  virtual int SizeX() const { return bm.SizeX()*factor; }
  virtual int SizeY() const { return bm.SizeY()*factor; }
  virtual Color Map(int x, int y) const
  {
    x/=factor;
    y/=factor;
    return bm.Map(x,y);
  }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    int sxx = bm.SizeX();
    int syy = bm.SizeY();
    int mm = std::max(sxx,syy);
    factor = float(sz)/float(mm);
  }

  virtual void Prepare()
  {
    bm.Prepare();
    int sxx = bm.SizeX();
    int syy = bm.SizeY();
    int mm = std::max(sxx,syy);
    factor = float(sz)/float(mm);
  }
private:
  Bitmap<Color> &bm;
  float factor;
  int sz;
};

GameApi::BM GameApi::BitmapApi::scale_to_size(BM bm, int sz)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  Bitmap<Color> *b = new ScaleToSize(*b2,sz);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;

}

class GridML : public MainLoopItem
{
public:
  GridML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Bitmap<int> &map, float y, float pos_x, float pos_z, float x_vec_x, float x_vec_z, float z_vec_x, float z_vec_z, int start_x, int start_z, float frame_inc) : env(env), ev(ev), next(next), map(map), y(y), pos_x(pos_x), pos_z(pos_z), x_vec_x(x_vec_x), x_vec_z(x_vec_z), z_vec_x(z_vec_x), z_vec_z(z_vec_z), start_x(start_x), start_z(start_z), frame_inc(frame_inc),pid(2.0,0.0,0.5) {
    curr_x=start_x;
    curr_z=start_z;
    next_x=start_x;
    next_z=start_z;
    loc = 1.0;
    pid.setPID(2.0,0.0,0.5);
    speed = 0.0;
    //pid.setOutputLimits(0.0,1.0);
    //pid.setDirection(true);
  }
  void Collect(CollectVisitor &vis)
  {
    map.Collect(vis);
    next->Collect(vis);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { map.Prepare(); next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    if (loc<1.0) {
      //loc+=frame_inc*e.delta_time;
      //std::cout << "Prev loc:" << loc << std::endl;
      double l = pid.getOutput(loc,1.3);
      //speed += l;
      loc+=l*e.delta_time;
      //std::cout << "New loc:" << loc << std::endl;
     }
    else if (flag==true) {
      flag = false;
      MainLoopEvent ee = ee_store;
      ee.type=769;
      ee.ch=0;
      if (curr_x<next_x) ee.ch='d';
      if (curr_x>next_x) ee.ch='a';
      if (curr_z<next_z) ee.ch='s';
      if (curr_z>next_z) ee.ch='w';
      if (ee.ch!=0)
	next->handle_event(ee);
    }
    
    if (loc>=1.0) loc=1.0;

    float curr_p_x = pos_x + curr_x*x_vec_x + curr_x*x_vec_z;
    float curr_p_y = y;
    float curr_p_z = pos_z + curr_z*z_vec_x + curr_z*z_vec_z;

    float next_p_x = pos_x + next_x*x_vec_x + next_x*x_vec_z;
    float next_p_y = y;
    float next_p_z = pos_z + next_z*z_vec_x + next_z*z_vec_z;

    Point curr_p(curr_p_x, curr_p_y, curr_p_z);
    Point next_p(next_p_x, next_p_y, next_p_z);

    Point middle = Point::Interpolate(curr_p, next_p, loc);

    Point res = middle;
    
    MainLoopEnv ee = e;
    GameApi::M env_m = add_matrix2(env, e.in_MV);
    GameApi::M trans = ev.matrix_api.trans(res.x,res.y,res.z);
    GameApi::M res2 = ev.matrix_api.mult(env_m, trans);
    ee.in_MV = find_matrix(env,res2);
    ee.env = find_matrix(env,res2);
    
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    bool filter_out = false;
    if (e.ch=='w'||e.ch==26||e.ch==82||
	e.ch=='a'||e.ch==4||e.ch==80||
	e.ch=='s'||e.ch==22||e.ch==81||
	e.ch=='d'||e.ch==7||e.ch==79)
      filter_out = true;

    if (e.type==769 && (e.ch=='w'||e.ch==26||e.ch==82))
      speed = 0.0;
    if (e.type==769 && (e.ch=='s'||e.ch==22||e.ch==81))
      speed = 0.0;
    if (e.type==769 && (e.ch=='a'||e.ch==4||e.ch==80))
      speed = 0.0;
    if (e.type==769 && (e.ch=='d'||e.ch==7||e.ch==79))
      speed = 0.0;
    
    if (e.type==768 && (e.ch=='w'||e.ch==26||e.ch==82) && loc>0.99) {
      curr_x = next_x;
      curr_z = next_z;
      loc-=1.0;
      next_z++;
      if (next_z>=map.SizeY() || !access(next_x,next_z)) next_z--;
      ee_store = e;
      filter_out=false;
      flag=true;
    }
    if (e.type==768 && (e.ch=='s'||e.ch==22||e.ch==81) && loc>0.99) {
      curr_x = next_x;
      curr_z = next_z;
      loc-=1.0;
      next_z--;
      if (next_z<0 || !access(next_x,next_z)) next_z++;
      ee_store = e;
      filter_out=false;
      flag=true;
    }
    if (e.type==768 && (e.ch=='a'||e.ch==4||e.ch==80) && loc>0.99) {
      curr_x = next_x;
      curr_z = next_z;
      loc-=1.0;
      next_x++;
      if (next_x>map.SizeX() || !access(next_x,next_z)) next_x--;
      ee_store = e;
      filter_out=false;
      flag=true;
    }
    if(e.type==768 && (e.ch=='d'||e.ch==7||e.ch==79) && loc>0.99) {
      curr_x = next_x;
      curr_z = next_z;
      loc-=1.0;
      next_x--;
      if (next_z<0 || !access(next_x,next_z)) next_x++;
      ee_store = e;
      filter_out=false;
      flag=true;
    }
    if (!filter_out)
      next->handle_event(e);
  }
  bool access(int x, int z)
  {
    if (x<0||x>=map.SizeX()) return false;
    if (z<0||z>=map.SizeY()) return false;
    if (map.Map(x,z)==0) return true;
    return false;
  }
  virtual std::vector<int> shader_id() {
    return next->shader_id();
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Bitmap<int> &map;
  float y;
  float pos_x, pos_z;
  float x_vec_x, x_vec_z;
  float z_vec_x, z_vec_z;
  int start_x, start_z;
  float frame_inc;
  
  // internal
  int curr_x, curr_z;
  int next_x, next_z;
  float loc;
  float speed;
  MainLoopEvent ee_store;
  bool flag = false;
  Pid pid;
};

GameApi::ML GameApi::BitmapApi::grid_ml(EveryApi &ev, ML next, IBM map, float y, float pos_x, float pos_z, float x_vec_x, float x_vec_z, float z_vec_x, float z_vec_z, int start_x, int start_z, float frame_inc)
{
  MainLoopItem *next_m = find_main_loop(e,next);
  Bitmap<int> *bm = find_int_bitmap(e,map);
  return add_main_loop(e, new GridML(e,ev,next_m, *bm, y, pos_x, pos_z, x_vec_x, x_vec_z, z_vec_z, z_vec_z, start_x, start_z, frame_inc));
}


class PWorld : public FaceCollection
{
public:
  PWorld(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<GameApi::P> vec, GameApi::IBM world, float pos_x, float pos_y, float pos_z, float dx, float dz, float y)
    : env(env), ev(ev), vec(vec), world(world), pos_x(pos_x), pos_y(pos_y), pos_z(pos_z), dx(dx), dz(dz), y(y) 
  {
    res.id = -1;
  }
  void Collect(CollectVisitor &vis)
  {
    Bitmap<int> *bm = find_int_bitmap(env, world);
    bm->Collect(vis);
    
  }
  void HeavyPrepare()
  {
    Bitmap<int> *bm = find_int_bitmap(env, world);
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    std::vector<GameApi::P> pieces;
    for(int z=0;z<sy;z++) {
      for(int x=0;x<sx;x++) {
	int piece = bm->Map(x,z);
	int s = vec.size();
	if (piece<0 || piece>=s) continue;
	GameApi::P trans = ev.polygon_api.translate(vec[piece], pos_x+dx*x, pos_y + y, pos_z+dz*z);
	pieces.push_back(trans);
      }
    }
    res = ev.polygon_api.or_array2(pieces);
    FaceCollection *coll = find_facecoll(env,res);
    coll->Prepare();

  }

  void Prepare() {
    Bitmap<int> *bm = find_int_bitmap(env, world);
    bm->Prepare();
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    std::vector<GameApi::P> pieces;
    for(int z=0;z<sy;z++) {
      for(int x=0;x<sx;x++) {
	int piece = bm->Map(x,z);
	int s = vec.size();
	if (piece<0 || piece>=s) continue;
	GameApi::P trans = ev.polygon_api.translate(vec[piece], pos_x+dx*x, pos_y + y, pos_z+dz*z);
	pieces.push_back(trans);
      }
    }
    res = ev.polygon_api.or_array2(pieces);
    FaceCollection *coll = find_facecoll(env,res);
    coll->Prepare();
  }
  virtual int NumFaces() const {
    if (res.id!=-1) {
      FaceCollection *coll = find_facecoll(env,res);
      return coll->NumFaces();
    } else return 1;
  }
  virtual int NumPoints(int face) const
  {
    if (res.id!=-1) {
        FaceCollection *coll = find_facecoll(env,res);
	return coll->NumPoints(face);
    } else return 1;
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->FacePoint(face,point);
    } else { return Point(0.0,0.0,0.0); }
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->PointNormal(face,point);
    } else { return Vector(0.0,0.0,0.0); }
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->Attrib(face,point,id);
    } else { return 0.0; }
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->AttribI(face,point,id);
    } else { return 0; }
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->Color(face,point);
    } else { return 0xffffffff; }
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->TexCoord(face,point);
    } else { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
  }
  virtual float TexCoord3(int face, int point) const {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->TexCoord3(face,point);
    } else {
      return 0.0;
    }
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<GameApi::P> vec;
  GameApi::IBM world;
  float pos_x, pos_y, pos_z;
  float dx,dz;
  float y;
  GameApi::P res;
};

GameApi::P GameApi::BitmapApi::p_world_from_bitmap(GameApi::EveryApi &ev, std::vector<P> vec, IBM world, float pos_x, float pos_y, float pos_z, float dx, float dz, float y)
{
  return add_polygon2(e, new PWorld(e,ev,vec,world,pos_x,pos_y,pos_z,dx,dz,y),1);
}


int cmd_find_ch(std::string expr, char find_ch)
{
  int s = expr.size();
  int level = 0;
  for(int i=0;i<s;i++) {
    char ch = expr[i];
    if (ch=='(') level++;
    if (ch==')') level--;
    if (level==0 && ch==find_ch) return i;
  }
  return -1;
}

bool cmd_is_number(std::string expr)
{
  int s = expr.size();
  for(int i=0;i<s;i++) {
    char ch = expr[i];
    if ((ch>='0'&&ch<='9')||ch=='.') continue;
    return false;
  }
  return true;
}

std::string cmd_stripspaces(std::string expr)
{
  int i = 0;
  for(;i<expr.size();i++)
    {
      if (expr[i]==' '||expr[i]=='\n'||expr[i]=='\r') continue;
      break;
    }
  int j=0;
  for(;j<expr.size();j++)
    {
      if (expr[expr.size()-j-1]==' '||expr[expr.size()-j-1]=='\n'||
	  expr[expr.size()-j-1]=='\r') continue;
      break;
    }
  return expr.substr(i,expr.size()-j-i);
}

bool is_cmd_unary(std::string label, std::string expr)
{
  std::string prefix = expr.substr(0,label.size());
  if (prefix!=label) return false;
  if (expr.size()<label.size()+1) return false;
  char ch = expr[label.size()];
  char ch2 = expr[expr.size()-1];
  if (ch=='(' && ch2==')') {
    return true;
    
  }
  return false;
}


enum {
      E_Less = 1,
      E_Greater,
      E_Empty,
      E_Number,
      E_X,
      E_Y,
      E_Plus,
      E_Minus,
      E_Mul,
      E_Div,
      E_Mod,
      E_Sin,
      E_Cos,
      E_Tan
};


void ParseCmds(std::string expr, std::vector<int> &cmdbuf, std::vector<float> &parambuf)
{
  expr = cmd_stripspaces(expr);
  if (expr=="") { cmdbuf.push_back(E_Empty); return; }
  if (cmd_is_number(expr)) { // number
    cmdbuf.push_back(E_Number);
    std::stringstream ss(expr);
    float val;
    ss >> val;
    parambuf.push_back(val);
    return;
  }
  if (expr=="x" || expr=="X") {
    cmdbuf.push_back(E_X); // X
    return;
  }
  if (expr=="y" || expr=="Y") {
    cmdbuf.push_back(E_Y);
    return;
  }
  
  int pos0 = cmd_find_ch(expr, '<');
  if (pos0!=-1) {
    std::string s1 = expr.substr(0,pos0);
    std::string s2 = expr.substr(pos0+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Less); // operator<
    return;
  }
  int pos0a = cmd_find_ch(expr, '>');
  if (pos0a!=-1) {
    std::string s1 = expr.substr(0,pos0a);
    std::string s2 = expr.substr(pos0a+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Greater); // operator<
    return;
  }

  
  int pos1 = cmd_find_ch(expr, '+');
  if (pos1!=-1) {
    std::string s1 = expr.substr(0,pos1);
    std::string s2 = expr.substr(pos1+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Plus); // operator+
    return;
  }
  int pos1a = cmd_find_ch(expr, '-');
  if (pos1a!=-1) {
    std::string s1 = expr.substr(0,pos1a);
    std::string s2 = expr.substr(pos1a+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Minus); // operator-
    return;
  }
  
  int pos2 = cmd_find_ch(expr, '*');
  if (pos2!=-1) {
    std::string s1 = expr.substr(0,pos2);
    std::string s2 = expr.substr(pos2+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Mul); // operator*
    return;
  }
  
  int pos2a = cmd_find_ch(expr, '/');
  if (pos2a!=-1) {
    std::string s1 = expr.substr(0,pos2a);
    std::string s2 = expr.substr(pos2a+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Div); // operator*
    return;
  }

  int pos2b = cmd_find_ch(expr, '%');
  if (pos2b!=-1) {
    std::string s1 = expr.substr(0,pos2b);
    std::string s2 = expr.substr(pos2b+1);
    ParseCmds(s1,cmdbuf,parambuf);
    ParseCmds(s2,cmdbuf,parambuf);
    cmdbuf.push_back(E_Mod); // operator%
    return;
  }

  bool b0 = is_cmd_unary("sin", expr);
  if (b0) {
    std::string sub = expr.substr(3+2,expr.size()-3-2-1);
    std::cout << "Sub:" << sub << std::endl;
    ParseCmds(sub,cmdbuf,parambuf);
    cmdbuf.push_back(E_Sin);
    return;
  }
  bool b1 = is_cmd_unary("cos", expr);
  if (b1) {
    std::string sub = expr.substr(3+2,expr.size()-3-2-1);
    std::cout << "Sub:" << sub << std::endl;
    ParseCmds(sub,cmdbuf,parambuf);
    cmdbuf.push_back(E_Cos);
    return;
  }
  bool b2 = is_cmd_unary("tan", expr);
  if (b2) {
    std::string sub = expr.substr(3+2,expr.size()-3-2-1);
    std::cout << "Sub:" << sub << std::endl;
    ParseCmds(sub,cmdbuf,parambuf);
    cmdbuf.push_back(E_Tan);
    return;
  }
  
  
  if (expr.size()>2 && expr[0]=='(' && expr[expr.size()-1]==')') {
    std::string sub = expr.substr(1,expr.size()-2);
    ParseCmds(sub,cmdbuf,parambuf);
    return;
  }

  
  std::cout << "PARSE ERROR at expression:\n\"" << expr << "\"\n";
  
}
float executebufs(const std::vector<int> &cmdbuf, const std::vector<float> &parambuf, float X, float Y)
{
  int paramcursor = 0;
  std::vector<float> values;
  int s = cmdbuf.size();
  float res = 0.0;
  for(int i=0;i<s;i++) {
    int cmd = cmdbuf[i];
    switch(cmd) {
    case E_Less: // operator<()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      //std::cout << val1 << "<" << val2 << std::endl;
      if (val2<val1) {
	//std::cout << "Value(<):" << 1.0 << std::endl;
	values.push_back(1.0);
      } else {
	//std::cout << "Value(<):" << 0.0 << std::endl;
	values.push_back(0.0);
      }
      }
      break;
    case E_Greater: // operator>()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      //std::cout << val1 << ">" << val2 << std::endl;
      if (val2>val1) {
	//std::cout << "Value(>):" << 1.0 << std::endl;
	values.push_back(1.0);
      } else {
	//std::cout << "Value(>):" << 0.0 << std::endl;
	values.push_back(0.0);
      }
      }
      break;
    case E_Plus: // operator+()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      values.push_back(val1+val2);
      //std::cout << "Value(+):" << val1+val2 << std::endl;
      break;
      }
    case E_Mul: // operator*()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      values.push_back(val1*val2);
      //std::cout << "Value(*):" << val1*val2 << std::endl;
      break;
      }
    case E_Minus: // operator-()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      values.push_back(val2-val1);
      //std::cout << "Value(-):" << val2-val1 << std::endl;
      break;
      }
    case E_Div: // operator/()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      values.push_back(val2/val1);
      //std::cout << "Value(/):" << val2/val1 << std::endl;
      break;
      }
    case E_Mod: // operator/()
      {
      float val1 = values.back();
      values.pop_back();
      float val2 = values.back();
      values.pop_back();
      values.push_back(fmod(val2,val1));
      //std::cout << "Value(/):" << val2/val1 << std::endl;
      break;
      }
    case E_Sin:
      {
      float val = values.back();
      values.pop_back();
      values.push_back(std::sin(val));
      break;
      }
    case E_Cos:
      {
      float val = values.back();
      values.pop_back();
      values.push_back(std::cos(val));
      break;
      }
    case E_Tan:
      {
      float val = values.back();
      values.pop_back();
      values.push_back(std::tan(val));
      break;
      }
    case E_X: // X
      values.push_back(X);
      //std::cout << "Value(X):" << X << std::endl;
      break;
    case E_Y: // Y
      values.push_back(Y);
      //std::cout << "Value(Y):" << Y << std::endl;
      break;
    case E_Number: // number
      {
	float val = parambuf[paramcursor];
	paramcursor++;
	values.push_back(val);
	//std::cout << "Value(num):" << val << std::endl;
	break;
      }
    case E_Empty:
      values.push_back(0.0);
      //std::cout << "Value(0.0):" << 0.0 << std::endl;
      break;
    };
  }
  res = values.back();
  //std::cout << "res:" << res << std::endl;
  return res;
}

class ScriptBitmap : public Bitmap<Color>
{
public:
  ScriptBitmap(GameApi::Env &e, std::string url, std::string homepage, int sx, int sy) : e(e), url(url), homepage(homepage),sx(sx),sy(sy) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
	      
  virtual void Prepare()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    std::string s(ptr->begin(),ptr->end());
    std::stringstream ss(s);
    std::string line;
    int linenum = 0;
    while(std::getline(ss,line)) {
      ParseCmds(line,cmd_buf[linenum],param_buf[linenum]);
      linenum++;
      if (linenum>=4) return;
    }
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float xx = float(x)/float(sx);
    float yy = float(y)/float(sy);
    float r = executebufs(cmd_buf[0],param_buf[0],xx,yy);
    float g = executebufs(cmd_buf[1],param_buf[1],xx,yy);
    float b = executebufs(cmd_buf[2],param_buf[2],xx,yy);
    float a = executebufs(cmd_buf[3],param_buf[3],xx,yy);
    //std::cout << r << " " << g << " " << b << " " << a << std::endl;
    return Color(r,g,b,a);
  }
private:
  GameApi::Env &e;
  int sx,sy;
  std::string url,homepage;
  std::vector<int> cmd_buf[4];
  std::vector<float> param_buf[4];
};

GameApi::BM GameApi::BitmapApi::script_bitmap(std::string url, int sx, int sy)
{
  Bitmap<Color> *b = new ScriptBitmap(e,url,gameapi_homepageurl,sx,sy);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}

// This is only for the logo bitmap
class SaveRawBitmap : public MainLoopItem
{
public:
  SaveRawBitmap(GameApi::Env &env, Bitmap<Color> &bm, std::string filename) : env(env), bm(bm), filename(filename) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    //std::ofstream file(filename.c_str(), std::ios_base::out|std::ios_base::binary);
    std::stringstream file;
    int sx = bm.SizeX();
    int sy = bm.SizeY();
    file << sx << " " << sy << std::endl;
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++) {
	Color c = bm.Map(x,y);
	unsigned int cc = c.Pixel();
	unsigned char *cc2 = (unsigned char*)&cc;
	file << cc2[0] << cc2[1] << cc2[2] << cc2[3];
      }
    std::string res = file.str();
    std::vector<unsigned char> vec(res.begin(),res.end());
    int bar = env.add_to_download_bar(filename);
    int ii = env.download_index_mapping(bar);
    env.set_download_data(ii, vec);
    env.set_download_ready(ii);
    
    //file.close();
  }
  
  virtual void Prepare()
  {
    bm.Prepare();
    //std::ofstream file(filename.c_str(), std::ios_base::out|std::ios_base::binary);
    std::stringstream file;
    int sx = bm.SizeX();
    int sy = bm.SizeY();
    file << sx << " " << sy << std::endl;
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++) {
	Color c = bm.Map(x,y);
	unsigned int cc = c.Pixel();
	unsigned char *cc2 = (unsigned char*)&cc;
	file << cc2[0] << cc2[1] << cc2[2] << cc2[3];
      }
    std::string res = file.str();
    std::vector<unsigned char> vec(res.begin(),res.end());
    int bar = env.add_to_download_bar(filename);
    int ii = env.download_index_mapping(bar);
    env.set_download_data(ii, vec);
    env.set_download_ready(ii);
    //file.close();
  }
  virtual void execute(MainLoopEnv &e)
  {
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
private:
  GameApi::Env &env;
  Bitmap<Color> &bm;
  std::string filename;
};
void save_raw_bitmap(GameApi::Env &e, GameApi::BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *c = find_color_bitmap(handle);
  SaveRawBitmap *save = new SaveRawBitmap(e,*c, filename);
  save->Prepare();
}

class SaveRawBitmapML : public MainLoopItem
{
public:
  SaveRawBitmapML(GameApi::Env &env,Bitmap<Color> *bm, std::string filename) : env(env), bm(bm), filename(filename) { }
  void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() { Prepare(); }

  void Prepare() {
    SaveRawBitmap *save = new SaveRawBitmap(env,*bm, filename);
    save->Prepare();
  }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
private:
  GameApi::Env &env;
  Bitmap<Color> *bm;
  std::string filename;
  
};
GameApi::ML GameApi::BitmapApi::save_raw(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  return add_main_loop(e, new SaveRawBitmapML(e,bitmap, filename));
}
#if 0
class LoadRawBitmap : public Bitmap<Color>
{
public:
  LoadRawBitmap(std::string filename) : filename(filename) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    return Color(bmdata[x+sx*y]);
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    std::ifstream ss(filename.c_str(), std::ios_base::in|std::ios_base::binary);
    ss >> sx >> sy;
    char ch;
    while(ss.peek()=='\n'||ss.peek()=='\r') ss >> ch;
    bmdata = new unsigned int[sx*sy];
    ss.read((char*)bmdata,sx*sy*sizeof(unsigned int));
  }
  virtual void Prepare()
  {
    std::ifstream ss(filename.c_str(), std::ios_base::in|std::ios_base::binary);
    ss >> sx >> sy;
    char ch;
    while(ss.peek()=='\n'||ss.peek()=='\r') ss >> ch;
    bmdata = new unsigned int[sx*sy];
    ss.read((char*)bmdata,sx*sy*sizeof(unsigned int));
  }
private:
  int sx = 0;
  int sy = 0;
  unsigned int *bmdata;
  std::string filename;
};
#endif


class LoadRawBitmap2 : public Bitmap<Color>
{
public:
  LoadRawBitmap2(std::string data) : data(data) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    return Color(bmdata[x+sx*y]);
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    std::stringstream ss(data);
    ss >> sx >> sy;
    char ch;
    while(ss.peek()=='\n'||ss.peek()=='\r') ss >> ch;
    bmdata = new unsigned int[sx*sy];
    ss.read((char*)bmdata,sx*sy*sizeof(unsigned int));
  }
  virtual void Prepare()
  {
    std::stringstream ss(data);
    ss >> sx >> sy;
    char ch;
    while(ss.peek()=='\n'||ss.peek()=='\r') ss >> ch;
    bmdata = new unsigned int[sx*sy];
    ss.read((char*)bmdata,sx*sy*sizeof(unsigned int));
    /*
    std::ifstream ss(filename.c_str(), std::ios_base::in|std::ios_base::binary);
    ss >> sx >> sy;
    char ch;
    while(ss.peek()=='\n'||ss.peek()=='\r') ss >> ch;
    bmdata = new unsigned int[sx*sy];
    ss.read((char*)bmdata,sx*sy*sizeof(unsigned int));
    */
  }
private:
  int sx = 0;
  int sy = 0;
  unsigned int *bmdata;
  std::string data;
};

GameApi::BM load_raw_bitmap2(GameApi::Env &e, std::string data)
{

  LoadRawBitmap2 *b = new LoadRawBitmap2(data);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  GameApi::BM bm = add_bitmap(e, handle2);
  return bm;
}

GameApi::BM load_raw_bitmap(GameApi::Env &e, std::string filename)
{
#if 0
  LoadRawBitmap *b = new LoadRawBitmap(filename);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  GameApi::BM bm = add_bitmap(e, handle2);
  return bm;
#endif
}

class CircularGradient : public Bitmap<Color>
{
public:
  CircularGradient(int sx, int sy, unsigned int center_color, unsigned int edge_color) : sx(sx), sy(sy), center_color(center_color), edge_color(edge_color) { }
  virtual int SizeX() const {return sx; }
  virtual int SizeY() const {return sy; }
  virtual Color Map(int x, int y) const
  {
    float r = sqrtf(float(x-sx/2)*float(x-sx/2)+float(y-sy/2)*float(y-sy/2));
    r/=sqrtf(float(sx/2*sx/2+sy/2*sy/2));
    Random rr;
    float rand = double(rr.next())/rr.maximum();
    rand/=10.0;
    rand-=1.0/20.0;
    r+=rand;
    if (r<0.0) r=0.0;
    if (r>1.0) r=1.0;
    return Color(Color::Interpolate(center_color, edge_color, r));
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  virtual void Prepare() { }

private:
  int sx,sy;
  unsigned int center_color, edge_color;
};

GameApi::BM GameApi::BitmapApi::circular_gradient(int sx, int sy, unsigned int center_color, unsigned int edge_color)
{
  Bitmap<Color> *b = new CircularGradient(sx,sy,center_color,edge_color);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}


class BitmapGraphicsContext : public Bitmap<Color>, public GraphicsContext
{
public:
  BitmapGraphicsContext(int sx, int sy) : sx(sx), sy(sy) {
    bm = new unsigned int[sx*sy];
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const { return Color(bm[x+sx*y]); } 
  virtual void Prepare() { }

  int size_x() const { return sx; }
  int size_y() const { return sy; }
  void draw_pixel(int x, int y, unsigned int color)
  {
    if (x>=0 && x<sx)
      if (y>=0 && y<sy)
	bm[x+sx*y] = color;
  }
  unsigned int get_pixel(int x, int y) const
  {
    if (x>=0 && x<sx)
      if (y>=0 && y<sy)
	return bm[x+sx*y];
    return 0x0;
  }
  void draw_rect(int xx, int yy, int ssx, int ssy, unsigned int color)
  {
    if (xx<0) { ssx+=xx; xx=0; } 
    if (yy<0) { ssy+=yy; yy=0; }
    if (xx+ssx>sx) { ssx=sx-xx; }
    if (yy+ssy>sy) { ssy=sy-yy; }
    
    for(int y=yy;y<ssy;y++)
      for(int x=xx;x<ssx;x++)
	{
	  bm[x+sx*y] = color;
	}
  }
  void draw_image(int xx, int yy, int ssx, int ssy, Bitmap<Color> &bitmap)
  {
    int bm_x = 0, bm_y =0;
    if (xx<0) { bm_x-=xx; ssx+=xx; xx=0; } 
    if (yy<0) { bm_y-=yy; ssy+=yy; yy=0; }
    if (xx+ssx>sx) { ssx=sx-xx; }
    if (yy+ssy>sy) { ssy=sy-yy; }
    int bm_x_start = bm_x;
    for(int y=yy;y<ssy;y++,bm_y++)
      {
	bm_x = bm_x_start;
	for(int x=xx;x<ssx;x++,bm_x++)
	  {
	    bm[x+sx*y] = bitmap.Map(bm_x,bm_y).Pixel();
	  }
      }
  }
private:
  unsigned int *bm;
  int sx, sy;
};

class FloodFill2 : public Bitmap<float>
{
public:
  FloodFill2(Bitmap<float> *bm, float percentage, int x, int y, bool inv) : bm(bm), percentage(percentage),x(x),y(y),inv(inv) { }
  ~FloodFill2()
  {
    delete [] done_bitmap;
    delete [] result_bitmap;
  }
  virtual void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
    if (!prepare_done) {
    sx = bm->SizeX();
    sy = bm->SizeY();
    done_bitmap = new bool[sx*sy];
    result_bitmap = new float[sx*sy];
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++) {
	done_bitmap[x+y*sx]=false;
	result_bitmap[x+y*sx]=0.0;
      }
    do_all(x,y);
    prepare_done = true;
    }
  }
  void Prepare()
  {
    bm->Prepare();
    HeavyPrepare();
  }
  void do_all(int x, int y) {
    std::vector<int> xx;
    std::vector<int> yy;
    xx.push_back(x);
    yy.push_back(y);
    while(xx.size()>0) {
      int x = xx.back();
      int y = yy.back();
      xx.pop_back();
      yy.pop_back();
      if (x<0||y<0||x>=sx||y>=sy) continue;
      float c = bm->Map(x,y);
      if (inv) {
	if (c<percentage) continue;
      } else {
	if (c>percentage) continue;
      }
      done_bitmap[x+y*sx] = true;
	if (inv)
	  result_bitmap[x+y*sx]=1.0-c; //(c-percentage)/(1.0-percentage);
	else
	  result_bitmap[x+y*sx]=c; //(c-(1.0-percentage))/percentage;
      if (x>0 && !done_bitmap[x-1+y*sx]) {
	xx.push_back(x-1);
	yy.push_back(y);
      }
      if (x<sx-1 && !done_bitmap[x+1+y*sx]) {
	xx.push_back(x+1);
	yy.push_back(y);
      }
      
      if (y>0 && !done_bitmap[x+(y-1)*sx]) {
	xx.push_back(x);
	yy.push_back(y-1);
      }
      if (y<sy-1 && !done_bitmap[x+(y+1)*sx]) {
	xx.push_back(x);
	yy.push_back(y+1);
      }
    }
  }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const {
    if (!done_bitmap ||!result_bitmap) return 0.0;
    if (done_bitmap[x+y*sx]) return result_bitmap[x+y*sx];
    //if (inv) return 0.0;
    return 1.0;
  }
private:
  Bitmap<float> *bm;
  bool *done_bitmap=0;
  float *result_bitmap=0;
  float percentage;
  int x,y;
  int sx,sy;
  bool inv;
  bool prepare_done=false;
};

GameApi::FB GameApi::BitmapApi::flood_fill(FB fb, float percentage, int x, int y, bool inv)
{
  Bitmap<float> *ffb = find_float_bitmap(e,fb)->bitmap;
  Bitmap<float> *ffc = new FloodFill2(ffb, percentage, x,y, inv);
  return add_float_bitmap(e, ffc);
}

GameApi::BM GameApi::BitmapApi::flood_fill_color(EveryApi &ev, BM bm, float percentage, int x, int y, unsigned int color)
{
  
  FB fb = ev.float_bitmap_api.from_red(bm);
  FB fb2 = flood_fill(fb, percentage, x, y, false);
  BM bm2 = newbitmap_fb(fb2,color);
  BM bm3 = blitbitmap_fb(bm2,bm,0,0,fb2);
  return bm3;
}
GameApi::BM GameApi::BitmapApi::flood_fill_color_inv(EveryApi &ev, BM bm, float percentage, int x, int y, unsigned int color)
{
  FB fb = ev.float_bitmap_api.from_red(bm);
  FB fb2 = flood_fill(fb, percentage, x, y, true);
  BM bm2 = newbitmap_fb(fb2,color);
  BM bm3 = blitbitmap_fb(bm2,bm,0,0,fb2);
  return bm3;
}

class GrayToBlack : public Bitmap<Color>
{
public:
  GrayToBlack(Bitmap<Color> *bm, float val) : bm(bm), val(val) { }
  virtual void Collect(CollectVisitor &vis) { bm->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { bm->Prepare(); }
  virtual int SizeX() const { return bm->SizeX(); }
  virtual int SizeY() const { return bm->SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Color c = bm->Map(x,y);
    if (c.rf()<val && c.bf()<val && c.gf()<val && c.af()>0.7 && c.rf()-c.bf()<0.1 && c.rf()-c.gf()<0.1)
      {
	c.r = 0;
	c.g = 0;
	c.b = 0;
	c.alpha = 255;
      }
    return c;
  }

private:
  float val;
  Bitmap<Color> *bm;
};

GameApi::BM GameApi::BitmapApi::gray_to_black(BM bm, float val)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = new GrayToBlack(b2,val);
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}

#include "gif.h"
class WriteGifAnim : public MainLoopItem
{
public:
  WriteGifAnim(GameApi::Env &e, Array<int,int> &arr, std::string filename, int delay) : e(e), arr(arr),filename(filename), delay(delay) { firsttime=true; }
  virtual void Collect(CollectVisitor &vis) {
    int s = arr.Size();
    for(int i=0;i<s;i++)
      {
	int id = arr.Index(i);
	GameApi::BM bm;
	bm.id = id;
	BitmapHandle *handle = find_bitmap(e, bm);
	::Bitmap<Color> *b2 = find_color_bitmap(handle);
	b2->Collect(vis);
      }
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    if (firsttime) {
      firsttime=false;


#ifndef EMSCRIPTEN
#ifdef WINDOWS
  system("mkdir %TEMP%\\_gameapi_builder");
  std::string home  = getenv("TEMP");
  std::string filename_with_path = home + std::string("\\_gameapi_builder\\gif_anim_tmp.gif");
#endif
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  if (dockerdir=="") {
    system("mkdir -p ~/.gameapi_builder");
  } else {
    system(("mkdir -p " + dockerdir + ".gameapi_builder").c_str());
  }
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  std::string filename_with_path = home + std::string(".gameapi_builder/gif_anim_tmp.gif");
#endif
  
  int index = e.add_to_download_bar(filename);
  int ii = e.download_index_mapping(index);

  if (arr.Size()==0) return;

  int bitmapid = arr.Index(0);
  BitmapHandle *handle = find_bitmap(e, bitmapid);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  b2->Prepare();
  
  //::Bitmap<Color> *b2 = vec[0];

    int sx = b2->SizeX();
    int sy = b2->SizeY();
  GifWriter g;
  GifBegin(&g, filename_with_path.c_str(), sx,sy, delay);
  
  int s = arr.Size();
  for(int i=0;i<s;i++) {
  int bitmapid = arr.Index(i);
  BitmapHandle *handle = find_bitmap(e, bitmapid);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  //    ::Bitmap<Color> *b2 = vec[i];
  b2->Prepare();
    int sx = b2->SizeX();
    int sy = b2->SizeY();

    std::vector<uint8_t> bitmap(sx*sy*4,0);
    
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  Color c = b2->Map(x,y);
	  bitmap[x*4+y*sx*4+0]=c.r;
	  bitmap[x*4+y*sx*4+1]=c.g;
	  bitmap[x*4+y*sx*4+2]=c.b;
	  bitmap[x*4+y*sx*4+3]=c.alpha;
	}
    GifWriteFrame(&g, bitmap.data(), sx,sy, delay);    
    e.set_download_progress(ii,float(i+1)/float(s));
  }
  GifEnd(&g);

  std::ifstream ss(filename_with_path.c_str(), std::ios_base::binary|std::ios_base::in);
  std::vector<unsigned char> file_contents;
  char c;
  while(ss.get(c))
    {
      file_contents.push_back((unsigned char)c);
    }

  e.set_download_data(ii,file_contents);
  e.set_download_ready(ii);
  
#endif
      
    }
    
  }
  virtual void Prepare() {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &e;
  //std::vector<Bitmap<::Color>*> vec;
  Array<int,int> &arr;
  bool firsttime;
  std::string filename;
  int delay;
};

GameApi::ML GameApi::BitmapApi::write_gif_anim(std::vector<BM> vec, std::string filename, int delay)
{
  int s = vec.size();
  std::vector<int> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(vec[i].id);
    }
  Array<int,int> *arr = new VectorArray<int>(vec2.begin(),vec2.end());
  return add_main_loop(e, new WriteGifAnim(e,*arr,filename,delay));
}
GameApi::ML GameApi::BitmapApi::write_gif_anim2(Array<int,int> *vec, std::string filename, int delay)
{
  return add_main_loop(e,new WriteGifAnim(e,*vec,filename,delay));
}

GameApi::ML GameApi::BitmapApi::gif_anim(EveryApi &ev, ML ml3, int key, float time_delta, int num, std::string filename, int delay)
{
  GameApi::ARR a = ev.texture_api.send_screenshots_via_key_array(ev, ml3, key, time_delta,num);
  ArrayType *t = find_array(e,a);
  GameApi::ML ml;
  ml.id = t->vec[0];
  GameApi::ARR arr2;
  arr2.id = t->vec[1];
  ArrayType *t2 = find_array(e,arr2);
  GameApi::ML ml4 = write_gif_anim2(t2->vec2, filename, delay);
  return ml4;
}



class FlipTileBitmap : public Bitmap<Color>
{
public:
  FlipTileBitmap(GameApi::Env &env, GameApi::BitmapApi *api, GameApi::BM bm, int sx, int sy, bool is_x) :e(env), api(api),bm(bm), sx(sx), sy(sy),is_x(is_x) { firsttime=true; res.id=-1; }
  virtual void Collect(CollectVisitor &vis)
  {
    BitmapHandle *handle = find_bitmap(e, bm);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    b2->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  virtual int SizeX() const
  {
    if (res.id==-1) return 0;
    BitmapHandle *handle = find_bitmap(e, res);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    return b2->SizeX();
  }
  virtual int SizeY() const
  {
    if (res.id==-1) return 0;
    BitmapHandle *handle = find_bitmap(e, res);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    return b2->SizeY();
  }
  virtual Color Map(int x, int y) const
  {
    if (res.id==-1) return Color(0x000000);
    BitmapHandle *handle = find_bitmap(e, res);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    return b2->Map(x,y);
  }
  virtual void Prepare()
  {
    if (firsttime) {
      firsttime=false;
      int ssx = api->size_x(bm);
      int ssy = api->size_y(bm);
      res = api->newbitmap(ssx,ssy,0x00000000);
      for(int j=0;j<ssy/sy;j++) {
	for(int i=0;i<ssx/sx;i++)
	  {
	    GameApi::BM part = api->subbitmap(bm,i*sx,j*sy,sx,sy);
	    GameApi::BM flipped;
	    if (is_x) {
	      flipped = api->flip_x(part);
	    } else {
	      flipped = api->flip_y(part);
	    }
	    res = api->blitbitmap(res,flipped,i*sx,j*sy);
	  }
      }
    }
  }

private:
  GameApi::Env &e;
  GameApi::BitmapApi *api;
  GameApi::BM bm;
  int sx;
  int sy;
  bool is_x;
  GameApi::BM res;
  bool firsttime;
};

GameApi::BM GameApi::BitmapApi::flip_tile_bitmap(BM bm, int sx, int sy, bool is_x)
{
  Bitmap<Color> *b = new FlipTileBitmap(e,this,bm,sx,sy,is_x);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}


class TileRendererMainLoop : public MainLoopItem
{
public:
  TileRendererMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, Tiles2d *t, std::vector<Bitmap<Color> *> bm, std::vector<int> num_tiles_in_bm, int tile_sx, int tile_sy, int player_tile_sx, int player_tile_sy, TileScroller *scr) : env(env), ev(ev), t(t), bm(bm), num_tiles_in_bm(num_tiles_in_bm), tile_sx(tile_sx), tile_sy(tile_sy), player_tile_sx(player_tile_sx), player_tile_sy(player_tile_sy), scr(scr) { }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare()
  {
    if (tile_sx==0||tile_sy==0) return;
    int ss = bm.size();
    //std::cout << 0 << "::" << 0 << std::endl;
    for(int kk=0;kk<ss;kk++)
      {
	Bitmap<Color>* bm2 = bm[kk];
	int numtiles = num_tiles_in_bm[kk];

	int tx=tile_sx;
	int ty=tile_sy;
	
	if (kk==4||kk==5||kk==6||kk==7) { tx=player_tile_sx; ty=player_tile_sy; }
	if (kk==8||kk==9) { /* weapon */ tx=player_tile_sx*2; ty=player_tile_sy; }
	if (kk==10||kk==11) { /* player death */ tx=player_tile_sx*2; ty=player_tile_sy; }
	
	int sx= bm2->SizeX()/tx;
	int sy= bm2->SizeY()/ty;

	//std::cout << numtiles << "::" << sx << " " << sy << std::endl;
	
	int count=0;
	for(int y=0;y<sy;y++)
	  {
	    for(int x=0;x<sx;x++)
	      {
		tiles.push_back(subbitmap_t<Color>(bm2,x*tx,y*ty,tx,ty));
		count++;
		if (count==numtiles) break;
	      }
	    if (count==numtiles) break;
	  }
	//std::cout << "COUNT=" << count << std::endl;
	//std::cout << kk+1 << "::" << tiles.size() << std::endl;
      }

    int s = tiles.size();
    std::cout << s << std::endl;
    for(int i=0;i<s;i++)
      {
	BitmapColorHandle *handle2 = new BitmapColorHandle;
	handle2->bm = tiles[i];
	GameApi::BM bm2 = add_bitmap(env, handle2);
	bms.push_back(bm2);
      }
    for(int i=0;i<s;i++)
      {
	GameApi::ML ml = ev.sprite_api.vertex_array_render(ev,bms[i]);
	MainLoopItem *ml3 = find_main_loop(env,ml);
	ml3->Prepare();
	mls.push_back(ml);
      }
    blk = add_block();
  }
  virtual void Prepare() {
    //std::cout << "Prepare" << std::endl;
    int s = bm.size();
    for(int i=0;i<s;i++)
      bm[i]->Prepare();
    
    HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
    
    //std::cout << "execute" << std::endl;
    int sx = t->SizeX();
    int sy = t->SizeY();
    //std::cout << "SX: " << sx << " " << sy << std::endl;
    int c = get_current_block();
    set_current_block(blk);
    std::vector<GameApi::ML> vec;
    scr->execute(e);

    Point scroll_pos = scr->get_pos();

    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  int tile = t->Tile(x,y);
	  if (tile>=0 && tile<mls.size()) {
	  GameApi::ML tile_bm_ml = mls[tile];
	  GameApi::MN mn0 = ev.move_api.mn_empty();
	  GameApi::MN mn = ev.move_api.trans2(mn0, scroll_pos.x+x*tile_sx,scroll_pos.y+y*tile_sy+64.0,0);
	  GameApi::ML move = ev.move_api.move_ml(ev, tile_bm_ml, mn, 1, 10);
	  vec.push_back(move);
	  }
	  
	}

    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)

	{
	  int tile2 = t->Tile2(x,y);
	  Point2d delta = t->Tile2Delta(x,y);
	  //if (tile2==5) std::cout << "Blob" << " " << tile2 << " " << mls.size() << std::endl;
	  if (tile2>=0 && tile2<mls.size()) {
	    //std::cout << "Tile2:" << x << " " << y << " " << tile2 << std::endl;
	    //std::cout << "Delta: " << delta.x << " " << delta.y << std::endl;
	  GameApi::ML tile_bm_ml = mls[tile2];
	  GameApi::MN mn0 = ev.move_api.mn_empty();
	  GameApi::MN mn = ev.move_api.trans2(mn0, scroll_pos.x+x*tile_sx+delta.x,scroll_pos.y+y*tile_sy+delta.y+64.0,0);
	  GameApi::ML move = ev.move_api.move_ml(ev, tile_bm_ml, mn, 1, 10);
	  vec.push_back(move);
	  }
	}
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)

	{
	  int tile2 = t->Tile3(x,y);
	  //std::cout << tile2 << "," << std::endl;
	  //if (tile2==5) std::cout << "Blob" << " " << tile2 << " " << mls.size() << std::endl;
	  if (tile2>=0 && tile2<mls.size()) {
	    //std::cout << "Tile2:" << x << " " << y << " " << tile2 << std::endl;
	    //std::cout << "Delta: " << delta.x << " " << delta.y << std::endl;
	  GameApi::ML tile_bm_ml = mls[tile2];
	  GameApi::MN mn0 = ev.move_api.mn_empty();
	  GameApi::MN mn = ev.move_api.trans2(mn0, scroll_pos.x+x*tile_sx,scroll_pos.y+y*tile_sy+64.0,0);
	  GameApi::ML move = ev.move_api.move_ml(ev, tile_bm_ml, mn, 1, 10);
	  vec.push_back(move);
	  
	  }
	}

    GameApi::ML ml3 = ev.mainloop_api.array_ml(ev,vec);
    GameApi::ML ml4 = ev.sprite_api.turn_to_2d(ev,ml3,0.0,0.0,1200.0,900.0);
    MainLoopItem *move_2 = find_main_loop(env,ml4);
    //move_2->Prepare();
    move_2->execute(e);
    //std::cout << std::endl;
    clear_block(blk);
    recreate_block(blk);
    set_current_block(c);
  }
  virtual void handle_event(MainLoopEvent &e) {
    scr->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  Tiles2d *t;
  std::vector<Bitmap<Color> *> bm;
  std::vector<int> num_tiles_in_bm;
  std::vector<Bitmap<Color> *> tiles;
  std::vector<GameApi::BM> bms;
  std::vector<GameApi::ML> mls;
  int blk=-1;
  int tile_sx, tile_sy;
  int player_tile_sx, player_tile_sy;
  TileScroller *scr;
};

class TileHud : public MainLoopItem, public TileHudInterface
{
public:
  TileHud(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::FI font, GameApi::BM status_bm) :env(env), ev(ev), font(font), status_bm(status_bm) { }
  virtual int get_score() const { return score; }
  virtual void set_score(int score1) { score=score1; }
  virtual int get_health() const { return health; }
  virtual void set_health(int health1) { health=health1; }
  virtual int get_lives() const { return lives; }
  virtual void set_lives(int lives1) { lives=lives1; }
  virtual void set_state(int state1) { state=state1; }

  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    int s = 10;
    for(int i=0;i<s;i++)
      {
	std::stringstream ss;
	ss << i;
	std::string label = ss.str();
	GameApi::BM bm = ev.font_api.draw_text_string(font,label,5,30);
	GameApi::ML ml = ev.sprite_api.vertex_array_render(ev,bm);
	MainLoopItem *ml3 = find_main_loop(env,ml);
	ml3->Prepare();
	nodes.push_back(ml);
      }
    GameApi::BM bm2 = ev.font_api.draw_text_string(font,"Score", 5,30);
    score_ml = ev.sprite_api.vertex_array_render(ev,bm2);
    GameApi::BM bm3 = ev.font_api.draw_text_string(font,"Health", 5, 30);
    health_ml = ev.sprite_api.vertex_array_render(ev,bm3);
    GameApi::BM bm4 = ev.font_api.draw_text_string(font,"Lives", 5, 30);
    lives_ml = ev.sprite_api.vertex_array_render(ev,bm4);


    GameApi::PT pt1 = ev.point_api.point(0.0,0.0,0.0);
    GameApi::PT pt2 = ev.point_api.point(0.0,64.0,0.0);
    GameApi::BM bm_grad = ev.bitmap_api.gradient(pt1,pt2,0xff884422,0xff442211, 1280,64);
    gradient_ml = ev.sprite_api.vertex_array_render(ev,bm_grad);
  }
  virtual void Prepare() { HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {


    //int c = get_current_block();
      //blk = add_block();
      //set_current_block(blk);

    std::vector<GameApi::ML> vec;
    vec.push_back(gradient_ml);


    int score_pos = 100.0;
    int health_pos = 500.0;
    int lives_pos = 1000.0;
    
    std::stringstream ss;
    ss << score;
    std::string label = ss.str();
    int s = label.size();
    for(int i=0;i<s;i++)
      {
	GameApi::ML tile = nodes[int(label[i]-'0')];
	GameApi::MN mn0 = ev.move_api.mn_empty();
	GameApi::MN mn = ev.move_api.trans2(mn0, i*20.0+15.0+score_pos,32.0,0);
	GameApi::ML move = ev.move_api.move_ml(ev, tile, mn, 1, 10);
	vec.push_back(move);
      }

    std::stringstream ss2;
    ss2 << health;
    std::string label2 = ss2.str();
    int s2 = label2.size();
    for(int i=0;i<s2;i++)
      {
	GameApi::ML tile = nodes[int(label2[i]-'0')];
	GameApi::MN mn0 = ev.move_api.mn_empty();
	GameApi::MN mn = ev.move_api.trans2(mn0, i*20.0+health_pos+15.0,32.0,0);
	GameApi::ML move = ev.move_api.move_ml(ev, tile, mn, 1, 10);
	vec.push_back(move);
      }


    std::stringstream ss3;
    ss3 << lives;
    std::string label3 = ss3.str();
    int s3 = label3.size();
    for(int i=0;i<s3;i++)
      {
	GameApi::ML tile = nodes[int(label3[i]-'0')];
	GameApi::MN mn0 = ev.move_api.mn_empty();
	GameApi::MN mn = ev.move_api.trans2(mn0, i*20.0+lives_pos+15.0,32.0,0);
	GameApi::ML move = ev.move_api.move_ml(ev, tile, mn, 1, 10);
	vec.push_back(move);
      }

    GameApi::MN mn0 = ev.move_api.mn_empty();
    GameApi::MN mn_score = ev.move_api.trans2(mn0,score_pos,3.0,0.0);
    GameApi::MN mn_health = ev.move_api.trans2(mn0,health_pos,3.0,0.0);
    GameApi::MN mn_lives = ev.move_api.trans2(mn0,lives_pos,3.0,0.0);

    GameApi::ML ml_score = ev.move_api.move_ml(ev, score_ml, mn_score, 1,10);
    GameApi::ML ml_health = ev.move_api.move_ml(ev, health_ml, mn_health, 1, 10);
    GameApi::ML ml_lives = ev.move_api.move_ml(ev, lives_ml, mn_lives, 1, 10);
    vec.push_back(ml_score);
    vec.push_back(ml_health);
    vec.push_back(ml_lives);

    
    GameApi::ML ml3 = ev.mainloop_api.array_ml(ev,vec);
    GameApi::ML ml4 = ev.sprite_api.turn_to_2d(ev,ml3,0.0,0.0,1200.0,900.0);
    MainLoopItem *move_2 = find_main_loop(env,ml4);
    //move_2->Prepare();
    move_2->execute(e);
    //clear_block(blk);
    //set_current_block(c);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  int score=0;
  int health=100;
  int lives=3;
  int state=0;
  std::vector<GameApi::ML> nodes;
  GameApi::FI font;
  GameApi::BM status_bm;
  GameApi::ML score_ml, health_ml, lives_ml;
  GameApi::ML gradient_ml;
  int blk=-1;
};

class TileSplashScreen : public MainLoopItem, public TileSplashScreenInterface
{
public:
  TileSplashScreen(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::BM bm, TileSplashScreenCallback *cb) : env(env), ev(ev), bm(bm),cb(cb) { }
  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  bool enabled() const { return !disable; }
  virtual void HeavyPrepare() {
    ml = ev.sprite_api.vertex_array_render(ev,bm);
    MainLoopItem *ml3 = find_main_loop(env,ml);
    ml3->Prepare();
  }
  virtual void Prepare() {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
    if (!disable) {
      

      
      BitmapHandle *handle = find_bitmap(env, bm);
      ::Bitmap<Color> *b2 = find_color_bitmap(handle);
      float bm_size = b2->SizeX();
      float bm_size_y = b2->SizeY();
      //int c = get_current_block();
      //blk = add_block();
      //set_current_block(blk);
    GameApi::MN mn0 = ev.move_api.mn_empty();
    GameApi::MN mn = ev.move_api.trans2(mn0,(1280-bm_size)/2,(900-bm_size_y)/2,0);
    GameApi::ML move = ev.move_api.move_ml(ev, ml, mn, 1, 10);
    
    GameApi::ML ml4 = ev.sprite_api.turn_to_2d(ev,move,0.0,0.0,1200.0,900.0);
    MainLoopItem *move_2 = find_main_loop(env,ml4);
    //move_2->Prepare();
    move_2->execute(e);
    //clear_block(blk);
    //set_current_block(c);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if ((e.type==0x300 && (e.ch==32||e.ch==13)) || e.button ==0)
      {
	disable=true;
      }
  }
  void reset() { disable=false; cb->reset(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  bool disable=false;
  GameApi::BM bm;
  GameApi::ML ml;
  TileSplashScreenCallback *cb;
  int blk=-1;
};

class TileScroller2d : public TileScroller
{
public:
  TileScroller2d(TilePlayer *player, float speed, int cell_sx, int cell_sy, int screen_sx, int screen_sy) : player(player), speed(speed), cell_sx(cell_sx), cell_sy(cell_sy), screen_sx(screen_sx), screen_sy(screen_sy) { }
  virtual Point get_pos() const
  {
    return Point(-current_pos.x,-current_pos.y,0);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual void execute(MainLoopEnv &e)
  {
    float delta_x = target_pos.x-current_pos.x;
    float delta_y = target_pos.y-current_pos.y;
    bool rest_x=false;
    bool rest_y=false;
    if (fabs(delta_x)<cell_sx) { delta_x=0; rest_x=true; }
    if (fabs(delta_y)<cell_sy) { delta_y=0; rest_x=true; }
    if (delta_x>0.0) current_pos.x+=speed;
    if (delta_x<0.0) current_pos.x-=speed;
    if (delta_y>0.0) current_pos.y+=speed;
    if (delta_y<0.0) current_pos.y-=speed;

    
    int pos_x = player->player_pos_x();
    int pos_y = player->player_pos_y();
    pos_x *= cell_sx;
    pos_y *= cell_sy;
    pos_x += cell_sx/2;
    pos_y += cell_sy/2;
    pos_x -=current_pos.x;
    pos_y -=current_pos.y;
    pos_x += player->delta_pos().x;
    pos_y += player->delta_pos().y;

    
    
    if (rest_x) {
      if (pos_x < screen_sx/10) {
	target_pos.x -= cell_sx*(screen_sx/cell_sx/2);
      }
      if (pos_x > screen_sx*9/10) {
	target_pos.x += cell_sx*(screen_sx/cell_sx/2);
      }
    }
    if (rest_y) {
      if (pos_y < screen_sy/10) {
	target_pos.y -= cell_sy*(screen_sy/cell_sy/2);
      }
      if (pos_y > screen_sy*9/10) {
	target_pos.y += cell_sy*(screen_sy/cell_sy/2);
      }
    }
  }
  void reset()
  {
    current_pos.x=0.0;
    current_pos.y=0.0;
    target_pos.x=0.0;
    target_pos.y=0.0;
  }
private:
  Point current_pos;
  Point target_pos;
  float speed;
  TilePlayer *player;
  int cell_sx, cell_sy;
  int screen_sx, screen_sy;
  };

class TileRender2d : public TileRenderer2d
{
public:
  TileRender2d(GameApi::Env &env, GameApi::EveryApi &ev, int sx, int sy) :env(env), ev(ev), sx(sx), sy(sy) { }
  void set_tiles_2d(Tiles2d *t, std::vector<Bitmap<Color> *> bm, std::vector<int> vec)
  {
    m_t = t;
    m_bm = bm;
    num_tiles_in_bm=vec;
  }
  MainLoopItem *get_renderer(TileScroller *scr) const
  {
    return new TileRendererMainLoop(env,ev,m_t, m_bm, num_tiles_in_bm, sx,sy,sx,sy*2,scr);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  Tiles2d *m_t;
  std::vector<Bitmap<Color> *> m_bm;
  std::vector<int> num_tiles_in_bm;
  int sx,sy;
};

class AnimTiles2d : public Tiles2d, public MainLoopItem
{
public:
  AnimTiles2d(GameApi::Env &env, Tiles2d *tiles, std::string tiles_strings, std::string tiles_strings2, std::string url, std::string homepage) : env(env), tiles(tiles), tiles_strings(tiles_strings), tiles_strings2(tiles_strings2), url(url), homepage(homepage) { }
  int SizeX() const {
    return tiles->SizeX();
  }
  int SizeY() const {
    return tiles->SizeY();
  }
  int Tile(int x, int y) const
  {
    int val = tiles->Tile(x,y);

    if (val>=0&&val<tiles_strings.size()) {
      char ch = tiles_strings[val];

      int s1 = anims.size();
      for(int i=0;i<s1;i++)
	{
	  int s2=anims[i].size();
	  int pos=-1;
	  for(int j=0;j<s2;j++)
	    {
	      if (ch==anims[i][j]) pos=j;
	    }
	  if (pos==-1) continue;
	  int mv = tiles->get_current_move1(x,y);
	  float previous_time2 = tiles->get_previous_time(x,y);
	  if (current_time>previous_time2+pos1[i]) {
	    //pos++;
	    mv++;
	    tiles->set_current_move1(x,y,mv);
	    //previous_time2 = current_time;
	    tiles->set_previous_time(x,y,current_time);
	  }
	  ch=anims[i][(pos+mv)%(anims[i].size())];
	}

      int s = tiles_strings.size();
      for(int i=0;i<s;i++)
	{
	  if (ch==tiles_strings[i]) val=i;
	}
      
    }
    return val;
    
  }
  int Tile2(int x, int y) const { return tiles->Tile2(x,y); }
  int Tile3(int x, int y) const {

    int val = tiles->Tile3(x,y);

    if (val>=0&&val<tiles_strings2.size()) {
      char ch = tiles_strings2[val];

      int s1 = anims2.size();
      for(int i=0;i<s1;i++)
	{
	  int s2=anims2[i].size();
	  int pos=-1;
	  for(int j=0;j<s2;j++)
	    {
	      if (ch==anims2[i][j]) pos=j;
	    }
	  if (pos==-1) continue;
	  int mv = tiles->get_current_move2(x,y);
	  float previous_time = tiles->get_previous_time2(x,y);
	  if (current_time>previous_time+pos2[i]) {
	    mv++;
	    //pos++;
	    tiles->set_current_move2(x,y,mv);
	    tiles->set_previous_time2(x,y,current_time);
	    //previous_time=current_time;
	  }
	  ch=anims2[i][(pos+mv)%(anims2[i].size())];
	  //std::cout << "CH chosen: " << ch << std::endl;
	  break;
	}

      int s = tiles_strings2.size();
      for(int i=0;i<s;i++)
	{
	  if (ch==tiles_strings2[i]) val=i;
	}      
    }
    return val;
  }

  virtual int get_current_move1(int x, int y) const { return tiles->get_current_move1(x,y); }
  virtual void set_current_move1(int x, int y, int val)
  {
    tiles->set_current_move1(x,y,val);
  }
  virtual int get_current_move2(int x, int y) const { return tiles->get_current_move2(x,y); }
  virtual void set_current_move2(int x, int y, int val)
  {
    tiles->set_current_move2(x,y,val);
  }
  virtual float get_previous_time(int x, int y) const { return tiles->get_previous_time(x,y); }
  virtual void set_previous_time(int x, int y, float val) { return tiles->set_previous_time(x,y,val); }

  virtual float get_previous_time2(int x, int y) const { return tiles->get_previous_time2(x,y); }
  virtual void set_previous_time2(int x, int y, float val) { tiles->set_previous_time2(x,y,val); }

  
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::stringstream ss(s);

      std::string line;
      bool mode=false;
      while(std::getline(ss,line)) {
	if (line=="@") { mode=true; continue; }
	std::stringstream ss2(line);
	float time_delta=1.0;
	std::string line2;
	ss2 >> time_delta >> line2;
	if (!mode) {
	  anims.push_back(line2);
	  pos1.push_back(time_delta);
	} else {
	  anims2.push_back(line2);
	  pos2.push_back(time_delta);
	}
      }
  }
  virtual void Prepare() { HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    current_time = e.time;
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  
private:
  GameApi::Env &env;
  Tiles2d *tiles;
  std::string tiles_strings, tiles_strings2;
  std::string url;
  std::string homepage;
  std::vector<std::string> anims;
  std::vector<float> pos1;
  std::vector<std::string> anims2;
  std::vector<float> pos2;
  float current_time;
  //mutable float previous_time=0.0;
  //mutable float previous_time2=0.0;
  float time_delta=1.0;
};

class NetworkTiles2d : public Tiles2d, public MainLoopItem
{
public:
  NetworkTiles2d(GameApi::Env &env, std::string url, std::string homepage, std::string tiles_string, std::string tiles_string2) : env(env), url(url), homepage(homepage), tiles_string(tiles_string), tiles_string2(tiles_string2) { }
  int SizeX() const {
    return sx;
  }
  int SizeY() const {
    return sy;
  }
  int Tile(int x, int y) const
  {
    int i = tiles[std::pair<int,int>(x,y)];
    //if (i<0) return 0;
    return i;

  }
  int Tile2(int x, int y) const { return -1; }
  int Tile3(int x, int y) const { return tiles3[std::pair<int,int>(x,y)]; }
  virtual void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    
    GameApi::ASyncVec *ptr = env.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }
    std::string data(ptr->begin(),ptr->end());
    std::stringstream ss(data);
    std::string line;
    int linenum = 0;
    int linenum2=0;
    int width=0;
    int width2=0;
    bool mode=false;
    while(std::getline(ss,line)) {
      if (line=="@") mode=true;
      if (!mode) {
      int s = line.size();
      width=std::max(width,s);
      for(int i=0;i<s;i++)
	{
	  char ch = line[i];
	  int s = tiles_string.size();
	  int pos=-1;
	  for(int j=0;j<s;j++)
	    {
	      if (ch==tiles_string[j]) { pos=j; break; }
	    }
	  tiles[std::make_pair(i,linenum)] = pos;
	}
      linenum++;
      } else { // mode==true
      int s = line.size();
      width2=std::max(width2,s);
      for(int i=0;i<s;i++)
	{
	  char ch = line[i];
	  int s = tiles_string2.size();
	  int pos=-1;
	  for(int j=0;j<s;j++)
	    {
	      if (ch==tiles_string2[j]) { pos=j; break; }
	    }
	  tiles3[std::make_pair(i,linenum2)] = pos;
	}
      linenum2++;
      }
    }
    sx=width;
    sy=linenum-1;
  }

  virtual void Prepare() {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
  }

  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  virtual int get_current_move1(int x, int y) const { return moves1[std::pair<int,int>(x,y)]; }
  virtual void set_current_move1(int x, int y, int val)
  {
    moves1[std::pair<int,int>(x,y)]=val;
  }
  virtual int get_current_move2(int x, int y) const { return moves2[std::pair<int,int>(x,y)]; }
  virtual void set_current_move2(int x, int y, int val)
  {
    moves2[std::pair<int,int>(x,y)]=val;
  }
  virtual float get_previous_time(int x, int y) const { return time[std::pair<int,int>(x,y)]; }
    virtual void set_previous_time(int x, int y, float val) { time[std::pair<int,int>(x,y)]=val; }

  virtual float get_previous_time2(int x, int y) const { return time2[std::pair<int,int>(x,y)]; }
  virtual void set_previous_time2(int x, int y, float val) { time2[std::pair<int,int>(x,y)]=val; }

  
private:
  GameApi::Env &env;
  std::string url;
  std::string homepage;
  std::string tiles_string;
  std::string tiles_string2;
  int sx=0;
  int sy=0;
  mutable std::map<std::pair<int,int>,int> tiles;
  mutable std::map<std::pair<int,int>,int> tiles3;
  mutable std::map<std::pair<int,int>,int> moves1;
  mutable std::map<std::pair<int,int>,int> moves2;
  mutable std::map<std::pair<int,int>,float> time;
  mutable std::map<std::pair<int,int>,float> time2;
};

class ItemsTile : public MainLoopItem
{
public:
  ItemsTile(GameApi::Env &env, GameApi::EveryApi &ev, TilePlayer &pl, std::string url, std::string homepage, int cell_sx, int cell_sy, TileScroller *scr, std::vector<GameApi::BM> item_types, TileHudInterface &hud) : env(env), ev(ev), pl(pl), url(url), homepage(homepage), cell_sx(cell_sx), cell_sy(cell_sy),scr(scr), item_types(item_types),hud(hud){ }

  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::stringstream ss(s);
      std::string line;
      instances.clear();
      while(std::getline(ss,line)) {
	std::stringstream ss2(line);
	Item i;
	i.type=0;
	if (ss2 >> i.x >> i.y >> i.type) { } else { continue; }
	i.enabled=true;
	i.delta_x=0.0;
	//std::cout << "LINE:" << line << std::endl;
	//std::cout << i.x << " " << i.y << " " << i.type << std::endl;
	instances.push_back(i);
      }
      item_types_ml.clear();
      
      int s3 = item_types.size();
      for(int i=0;i<s3;i++)
	{
	  GameApi::BM bm = item_types[i];
	  GameApi::ML ml = ev.sprite_api.vertex_array_render(ev,bm);
	  MainLoopItem *ml3 = find_main_loop(env,ml);
	  ml3->Prepare();
	  item_types_ml.push_back(ml);
	}
      
  }
  virtual void Prepare()
  {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {

    int xx = pl.player_pos_x();
    int yy = pl.player_pos_y();
    
    
    Point scroll_pos = scr->get_pos();

    //int c = get_current_block();
    //blk = add_block();
    //set_current_block(blk);
    
    std::vector<GameApi::ML> vec;
    int s = instances.size();
    for(int i=0;i<s;i++) {
      Item &ii = instances[i];
      int x = ii.x;
      int y = ii.y;
      if (ii.enabled && x==xx&&y==yy) { ii.enabled=false; hud.set_score(hud.get_score()+1); }
      int type = ii.type;
      float delta_x = ii.delta_x;
      bool enabled = ii.enabled;
      if (enabled && type>=0 && type<item_types_ml.size()) {
	GameApi::ML ml = item_types_ml[type];
	GameApi::MN mn0 = ev.move_api.mn_empty();
	GameApi::MN mn1 = ev.move_api.trans2(mn0,delta_x+x*cell_sx+scroll_pos.x,scroll_pos.y+y*cell_sy+64.0,0.0);
	GameApi::ML ml2 = ev.move_api.move_ml(ev,ml,mn1,1,10);
	vec.push_back(ml2);
      }
    }
    GameApi::ML ml3 = ev.mainloop_api.array_ml(ev,vec);
    GameApi::ML ml4 = ev.sprite_api.turn_to_2d(ev,ml3,0.0,0.0,1200.0,900.0);
    MainLoopItem *move_2 = find_main_loop(env,ml4);
    //move_2->Prepare();
    move_2->execute(e);
    //clear_block(blk);
    //set_current_block(c);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  void add_random_item(int x, int y, float delta_x)
  {
    Item i;
    i.x = x;
    i.y = y;
    static int types=-1;
    types++;
    if (types>=item_types_ml.size()) types=0;
    i.type = types;
    i.enabled=true;
    i.delta_x=delta_x;
    instances.push_back(i);
  }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  TilePlayer &pl;
  Tiles2d *next;
  std::string url, homepage;
  std::vector<GameApi::BM> item_types;
  std::vector<GameApi::ML> item_types_ml;
  struct Item
  {
    int x,y;
    int type;
    bool enabled;
    float delta_x;
  };
  std::vector<Item> instances;
  int cell_sx, cell_sy;
  TileScroller *scr;
  TileHudInterface &hud;
  int blk=-1;
};

#undef HITBOX



class EnemyTile : public MainLoopItem
{
public:
  EnemyTile(GameApi::Env &env, GameApi::EveryApi &ev, TilePlayer &pl, std::string url, std::string homepage, int cell_sx, int cell_sy, TileScroller *scr, std::vector<GameApi::BM> item_types, TileHudInterface &hud, TileSplashScreenInterface &splash, std::vector<GameApi::BM> child_death, std::vector<GameApi::BM> child_death_flip, TileEnemyCallback *ene_cb) : env(env), ev(ev), pl(pl), url(url), homepage(homepage), cell_sx(cell_sx), cell_sy(cell_sy),scr(scr), item_types(item_types),hud(hud),splash(splash), child_death(child_death), child_death_flip(child_death_flip),ene_cb(ene_cb) { }

  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {
#ifdef HITBOX
    hitbox=ev.bitmap_api.newbitmap(4,4,0xffffffff);
    BitmapHandle *handle = find_bitmap(env, hitbox);
    ::Bitmap<Color> *b2 = find_color_bitmap(handle);
    b2->Prepare();
    hitbox_ml=ev.sprite_api.vertex_array_render(ev,hitbox);
#endif    
    int s1 = child_death.size();
    for(int i=0;i<s1;i++)
      {
	BitmapHandle *handle = find_bitmap(env, child_death[i]);
	::Bitmap<Color> *b2 = find_color_bitmap(handle);
	b2->Prepare();
      }
    int s2 = child_death_flip.size();
    for(int i=0;i<s2;i++) {
      BitmapHandle *handle2 = find_bitmap(env, child_death_flip[i]);
      ::Bitmap<Color> *b3 = find_color_bitmap(handle2);
      b3->Prepare();
    }
      
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::stringstream ss(s);
      std::string line;
      instances.clear();
      while(std::getline(ss,line)) {
	std::stringstream ss2(line);
	Item i;
	i.type=0;
	if (ss2 >> i.x0 >> i.x1 >> i.y >> i.type) { } else { continue; }
	i.x = (i.x0+i.x1)*cell_sx/2;
	i.dir=false;
	i.frame=0;
	i.enabled=true;
	i.death_frame=-1;
	i.death_frame_after=0;
	i.start_timer = 0;
	//std::cout << "LINE:" << line << std::endl;
	//std::cout << i.x << " " << i.y << " " << i.type << std::endl;
	instances.push_back(i);
      }
      item_types_ml.clear();
      item_types_flip_ml.clear();
      
      int s3 = item_types.size();
      for(int i=0;i<s3;i++)
	{
	  GameApi::BM bm = item_types[i];
	  int sx = ev.bitmap_api.size_x(bm);
	  sx/=cell_sx;
	  std::vector<GameApi::ML> vec_ml;
	  for(int j=0;j<sx;j++) {
	    GameApi::BM bm2 = ev.bitmap_api.subbitmap(bm,j*cell_sx,0,cell_sx,cell_sy);
	    GameApi::ML ml = ev.sprite_api.vertex_array_render(ev,bm2);
	    MainLoopItem *ml3 = find_main_loop(env,ml);
	    ml3->Prepare();
	    vec_ml.push_back(ml);
	  }
	  item_types_ml.push_back(vec_ml);
	}

      int s4 = item_types.size();
      for(int i=0;i<s4;i++)
	{
	  GameApi::BM bm = item_types[i];
	  GameApi::BM bm1 = ev.bitmap_api.flip_tile_bitmap(bm,cell_sx,cell_sy,true);
	  BitmapHandle *handle = find_bitmap(env, bm);
	  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
	  b2->Prepare();
	  
	  int sx = ev.bitmap_api.size_x(bm);
	  sx/=cell_sx;
	  std::vector<GameApi::ML> vec_ml;
	  for(int j=0;j<sx;j++) {
	    GameApi::BM bm2 = ev.bitmap_api.subbitmap(bm1,j*cell_sx,0,cell_sx,cell_sy);
	    GameApi::ML ml = ev.sprite_api.vertex_array_render(ev,bm2);
	    MainLoopItem *ml3 = find_main_loop(env,ml);
	    ml3->Prepare();
	    vec_ml.push_back(ml);
	  }
	  item_types_flip_ml.push_back(vec_ml);
	}
      child_d_frames.clear();
      child_d_frames_flip.clear();
      int d = child_death.size();
      for(int i=0;i<d;i++)
	{
	  int d_sx = ev.bitmap_api.size_x(child_death[i]);
	  std::vector<GameApi::ML> frms;
	  for(int j=0;j<d_sx;j++) {
	    GameApi::BM sub = ev.bitmap_api.subbitmap(child_death[i], j*cell_sx,0,cell_sx,cell_sy);
	    GameApi::ML ml=ev.sprite_api.vertex_array_render(ev,sub);
	    MainLoopItem *ml3 = find_main_loop(env,ml);
	    ml3->Prepare();
	    frms.push_back(ml);
	  }
	  child_d_frames.push_back(frms);
	}


      int d2 = child_death_flip.size();
      for(int i=0;i<d2;i++)
	{
	  int d_sx = ev.bitmap_api.size_x(child_death_flip[i])/cell_sx;
	  std::vector<GameApi::ML> frms;
	  for(int j=0;j<d_sx;j++) {
	    GameApi::BM sub = ev.bitmap_api.subbitmap(child_death_flip[i], j*cell_sx,0,cell_sx,cell_sy);
	    GameApi::ML ml=ev.sprite_api.vertex_array_render(ev,sub);
	    MainLoopItem *ml3 = find_main_loop(env,ml);
	    ml3->Prepare();
	    frms.push_back(ml);
	  }
	  child_d_frames_flip.push_back(frms);
	}

      
  }
  virtual void Prepare()
  {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    static int m_frame=0;
    static int m_num=0;
    m_frame++;
    if (m_frame==100)
      {
	m_frame=0;
	m_num++;
	int s = instances.size();
	if (s)
	  {
	    int i = m_num%s;
	    Item ii = instances[i];
	    ii.dir=!ii.dir;
	    ii.death_frame=-1;
	    ii.frame=0;
	    ii.start_timer=100;
	    instances.push_back(ii);
	  }
	
      }
    
    int xx = pl.player_pos_x();
    int yy = pl.player_pos_y();
    int delta_x = pl.delta_pos().x-pl.weapon_delta();
    
    Point scroll_pos = scr->get_pos();


    //int c = get_current_block();
    //blk = add_block();
    //set_current_block(blk);

    
    std::vector<GameApi::ML> vec;
    int s = instances.size();
    for(int i=0;i<s;i++) {
      Item &ii = instances[i];
      float x0 = ii.x0*cell_sx;
      float x1 = ii.x1*cell_sx;
      int y = ii.y;
      int x = ii.x;
      bool dir = ii.dir;
      if (ii.start_timer>0) ii.start_timer--;
      
      if (ii.start_timer==0 && ii.enabled && ii.death_frame==-1 && !pl.in_jump() && (x+cell_sx/2)/cell_sx==xx&&y==yy) { pl.kill_player(); ii.enabled=false; hud.set_lives(hud.get_lives()-1); if (hud.get_lives()==0) { std::cout << "GAME OVER" << std::endl; splash.reset(); } }
      if (ii.death_frame==-1) {
	if (dir) x+=1;
	if (!dir) x-=1;
	
	if (x<x0) dir=!dir;
	if (x>x1) dir=!dir;
      }
      ii.frame++; if (ii.frame>=item_types_ml[ii.type].size()) ii.frame=0;
      ii.dir=dir;
      ii.x=x;
      int frame=ii.frame;
      int type = ii.type;
      bool enabled = ii.enabled;
      if (enabled) {
	GameApi::ML ml;
	if (!ii.dir) {
	  ml=item_types_flip_ml[type][frame];
	} else {
	  ml=item_types_ml[type][frame];
	}
	int dd=pl.player_dir();
	//std::cout << "DIR:" << x << " " << cell_sx<< " " << dd << " " << xx*64 << " " << delta_x << " " << cell_sx/2 << std::endl;
	//std::cout << pl.weapon_active() << " " << ii.death_frame << " " << (x+cell_sx/2)/cell_sx << " " << xx-dd << " " << y << " " << yy << std::endl;
	int hitbox = std::fabs(x+cell_sx/2-(dd*64+(xx)*64+delta_x+cell_sx/2));
     
	if (pl.weapon_active() && pl.weapon_at_kill() && ii.death_frame==-1 && hitbox>=0&&hitbox<20&&y==yy) {
	  //std::cout << "ANIM_START" << std::endl;
	  ii.death_frame=0;
	  ii.death_frame_counter=0;
	}
	if (ii.death_frame!=-1)
	  {
	    ii.death_frame_counter++;
	  }
	if (ii.death_frame!=-1)
	  {
	    if (ii.death_frame_counter>10) {
	      ii.death_frame++;
	      ii.death_frame_counter=0;
	    }
	    if (ii.death_frame>=std::min(child_d_frames[type].size(),child_d_frames_flip[type].size())) {
	      ii.death_frame=std::min(child_d_frames[type].size(),child_d_frames_flip[type].size())-1;

	      ii.death_frame_after++;
	      if (ii.death_frame_after>=60)
		{
		  ene_cb->add_random_item(ii.x/64,ii.y,std::fmod(ii.x,64.0));
		  ii.enabled=false;
		}
	      
	    }
	    if (ii.death_frame!=-1) {
	    if (!ii.dir) {
	      ml=child_d_frames_flip[type][ii.death_frame];
	    } else {
	      ml=child_d_frames[type][ii.death_frame];
	    }
	    }
	  }
	GameApi::MN mn0 = ev.move_api.mn_empty();
	GameApi::MN mn1 = ev.move_api.trans2(mn0,x+scroll_pos.x,scroll_pos.y+y*cell_sy+64.0,0.0);
	GameApi::ML ml2 = ev.move_api.move_ml(ev,ml,mn1,1,10);
	vec.push_back(ml2);

       
#ifdef HITBOX	
	GameApi::MN h_mn0 = ev.move_api.mn_empty();
	GameApi::MN h_mn1 = ev.move_api.trans2(h_mn0,x+cell_sx/2-2+scroll_pos.x,y*64-2+scroll_pos.y,0.0);
	GameApi::ML h_ml2 = ev.move_api.move_ml(ev,hitbox_ml,h_mn1,1,10);
	vec.push_back(h_ml2);
	GameApi::MN h2_mn0 = ev.move_api.mn_empty();
	GameApi::MN h2_mn1 = ev.move_api.trans2(h2_mn0,dd*64+(xx)*64+delta_x+cell_sx/2-2+scroll_pos.x,yy*64-2+scroll_pos.y,0.0);
	GameApi::ML h2_ml2 = ev.move_api.move_ml(ev,hitbox_ml,h2_mn1,1,10);
	vec.push_back(h2_ml2);
#endif	
	
      }
   }
    GameApi::ML ml3 = ev.mainloop_api.array_ml(ev,vec);
    GameApi::ML ml4 = ev.sprite_api.turn_to_2d(ev,ml3,0.0,0.0,1200.0,900.0);
    MainLoopItem *move_2 = find_main_loop(env,ml4);
    // move_2->Prepare();
    move_2->execute(e);
    //clear_block(blk);
    //set_current_block(c);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  TilePlayer &pl;
  Tiles2d *next;
  TileSplashScreenInterface &splash;
  std::string url, homepage;
  std::vector<GameApi::BM> item_types;
  std::vector<std::vector<GameApi::ML> > item_types_ml;
  std::vector<std::vector<GameApi::ML> > item_types_flip_ml;
  struct Item
  {
    int x0,x1,y;
    float x;
    int type;
    int frame;
    bool enabled;
    bool dir;
    int death_frame;
    int death_frame_counter;
    int death_frame_after;
    int start_timer;
  };
  std::vector<Item> instances;
  int cell_sx, cell_sy;
  TileScroller *scr;
  TileHudInterface &hud;
  TileEnemyCallback *ene_cb;
  std::vector<GameApi::BM> child_death;
  std::vector<GameApi::BM> child_death_flip;
  std::vector<std::vector<GameApi::ML> > child_d_frames;
  std::vector<std::vector<GameApi::ML> > child_d_frames_flip;
  int blk=-1;
  //MatrixMovement *mat;
  //MainLoopItem *item;
#ifdef HITBOX
  GameApi::BM hitbox;
  GameApi::ML hitbox_ml;
#endif
};


class PlayerTileMapping : public Tiles2d
{
public:
  PlayerTileMapping(TilePlayer &pl, Tiles2d &next) : pl(pl), next(next) { }
  virtual int SizeX() const { return next.SizeX(); }
  virtual int SizeY() const { return next.SizeY(); }
  virtual int Tile(int x, int y) const
  {
    return next.Tile(x,y);
  }
  virtual int Tile2(int x, int y) const
  {
    int xx = pl.player_pos_x();
    int yy = pl.player_pos_y();
    if (x==xx && y==yy) return pl.player_tile();


    //if (x==xx && y==yy+1) return 3;
    return -1;
  }
  virtual int Tile3(int x, int y) const
  {
    return next.Tile3(x,y);
  }
  virtual Point2d Tile2Delta(int x, int y) const
  {
    int xx = pl.player_pos_x();
    int yy = pl.player_pos_y();
    if (x==xx && y==yy) { Point p = pl.delta_pos(); Point2d p2; p2.x=p.x; p2.y=p.y; return p2; }
    Point2d p;
    p.x=0.0;
    p.y=0.0;
    return p;
  }
  virtual int get_current_move1(int x, int y) const { return next.get_current_move1(x,y); }
  virtual void set_current_move1(int x, int y, int val)
  {
    next.set_current_move1(x,y,val);
  }
  virtual int get_current_move2(int x, int y) const { return next.get_current_move2(x,y); }
  virtual void set_current_move2(int x, int y, int val)
  {
    next.set_current_move2(x,y,val);
  }
  virtual float get_previous_time(int x, int y) const { return next.get_previous_time(x,y); }
  virtual void set_previous_time(int x, int y, float val) { next.set_previous_time(x,y,val); }

  virtual float get_previous_time2(int x, int y) const { return next.get_previous_time2(x,y); }
  virtual void set_previous_time2(int x, int y, float val) { next.set_previous_time2(x,y,val); }

  
private:
  TilePlayer &pl;
  Tiles2d &next;
};

bool is_transparent_tile(int val)
{
  if (val>=5+8 && val<5+8+3) return true;
  
  return val<0;
}
bool is_not_wall(int val)
{
  if (val!=0) return true;
  return false;
}

class PlayerTile : public TilePlayer
{
public:
  PlayerTile(int tile_sx, int tile_sy, int start_pos_x, int start_pos_y, int player_start_tile, int player_end_tile, float delta_time, float delta_move_time, TileHudInterface *hud) : tile_sx(tile_sx), tile_sy(tile_sy), pos_x(start_pos_x), pos_y(start_pos_y), player_start_tile(player_start_tile), player_end_tile(player_end_tile), delta_time(delta_time), delta_move_time(delta_move_time),hud(hud) { tile=player_start_tile; }
  virtual void SetTiles2d(Tiles2d *t) { tiles=t; }
  virtual void SetTiles3d(Tiles3d *t) { }
  virtual void handle_event(MainLoopEvent &e) {
      //if (e.type==0x300 && e.ch=='w') { pos_y--; move_freeze=true; y_dir=true; }

    //std::cout << "Event:" << e.type << " " << e.ch << std::endl;

    if (death_frame==-1) {
    
    if (e.type==0x300 && (e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906)&&(!jump_ongoing||jump_aborted)) { jump_aborted=false; jump_trigger=true; jump_ongoing=true; jump_state=0; }
    }
    if (e.type==0x301 && (e.ch=='w'||e.ch==26||e.ch==82||e.ch==1073741906) && jump_ongoing && (jump_state==0||jump_state==1)) {
      if (jump_height<-64) {
	jump_state=2; jump_trigger=true; jump_max_height=jump_height; jump_aborted=true;
      } else { jump_abort_delayed=true; }
    }
    if (death_frame==-1) {
    if (e.type==0x300 && e.ch==32) { weapon_start = true; }
    }
    if (e.type==0x301 && e.ch==32) { weapon_start=false; weapon_ongoing = false; }

    
    if (death_frame==-1) {
    if (e.type==0x300 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) {
      dir_x=1;

      int val_x = tiles->Tile(player_pos_x()+dir_x,player_pos_y()+(delta_pos().y+64)/64);
      
      if ((!jump_ongoing &&!is_not_wall(val_x)) ||(jump_ongoing && !is_transparent_tile(val_x))) {
	dir_x=0;
      }

    }
    if (e.type==0x300 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) {
      dir_x=-1;

      int val_x = tiles->Tile(player_pos_x()+dir_x,player_pos_y()+(delta_pos().y+64)/64);
      
      if ((jump_ongoing && !is_transparent_tile(val_x))||(!jump_ongoing && !is_not_wall(val_x))) {
	dir_x=0;
      }

    }
    }
      //if (e.type==0x300 && e.ch=='s') { pos_y++; move_freeze=true; y_dir=true; }

    if (e.type==0x301 && (e.ch=='d'||e.ch==7||e.ch==79||e.ch==1073741903)) { dir_x=0;  }
    if (e.type==0x301 && (e.ch=='a'||e.ch==4||e.ch==80||e.ch==1073741904)) { dir_x=0;  }
      

  
    /*
    else
      {
	if (e.type==0x300 && e.ch=='w') { prepare_move_dir_x=0; prepare_move_dir_y=-1; }
	if (e.type==0x300 && e.ch=='d') { prepare_move_dir_x=1; prepare_move_dir_y=0; }
	if (e.type==0x300 && e.ch=='a') { prepare_move_dir_x=-1; prepare_move_dir_y=0; }
	if (e.type==0x300 && e.ch=='s') { prepare_move_dir_x=0; prepare_move_dir_y=1; }
	}*/
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (death_frame==-1) { hud->set_health(100); }
    if (death_frame!=-1)
      {
	static int frame=0;
	frame++;
	if (frame>=10) {
	  death_frame++;
	  if (death_frame>=5) { death_frame=4; } else frame=0;

	  if (death_frame==4 && frame>400) death_frame=-1;
	  if (death_frame!=-1) { hud->set_health(100-death_frame*20); }
	}
	
      }
	
    
    if (weapon_start) { weapon_ongoing=true; weapon_start_time=e.time; weapon_start=false; }
    if (weapon_ongoing && e.time>weapon_start_time+100.0)
      {
	//std::cout << "TIME" << std::endl;
	weapon_ongoing=false;
      }
    current_weapon = int((e.time-weapon_start_time)*10.0)%4;
    
    //MainLoopEvent ee;
    if (!move_freeze) {
      if (dir_x==1) { if (last_x==-1) { has_flipped=true; } else { has_flipped=false; } move_x=1; move_freeze=true; flip=false; y_dir=false; last_x=1;  }
      if (dir_x==-1) { if (last_x==1) { has_flipped=true; } else { has_flipped=false; } move_x=-1; move_freeze=true; flip=true; y_dir=false; last_x=-1;  }
    }
    //ee.type=771;
    //ee.ch=-1;
    //handle_event(ee);

    if (jump_abort_delayed && jump_height<-64) { 	jump_state=2; jump_trigger=true; jump_max_height=jump_height; jump_aborted=true; jump_abort_delayed=false; }
    if (jump_trigger) { jump_start_time=e.time; jump_trigger=false; }
    
    
    float jump_delta = e.time-jump_start_time;
    if (jump_ongoing && jump_state==3)
      {
	//std::cout << "Event3" << std::endl;
	jump_height=jump_max_height+jump_speed*jump_delta;
	if(jump_height>0.0) { jump_ongoing=false; jump_height=0.0; }
      }
    if (jump_ongoing && jump_state==2)
      {
	//std::cout << "Event2" << std::endl;
	jump_height=jump_max_height+jump_speed*jump_delta;
	if(jump_height>-64.0*0.5) jump_state=3;
      }
    if (jump_ongoing && jump_state==1)
      {
	//std::cout << "Event1" << std::endl;
	jump_height=-jump_speed*jump_delta;
	if (jump_height<-64.0*3.0) { jump_state=2; jump_start_time=e.time; jump_max_height=jump_height;}
      }
    if (jump_ongoing && jump_state==0)
      {
	//std::cout << "Event0" << std::endl;
	jump_height=-jump_speed*jump_delta;
	if (jump_height<-64.0*0.5) jump_state=1;
      }
    //std::cout << "Jump state: " << jump_state << " " << jump_height << std::endl;

    if (old_move_freeze!=move_freeze)
      {
	last_move_time=e.time;
	old_move_freeze=move_freeze;
      }
    if (old_flip!=flip)
      {
	last_move_time=e.time;
	old_flip=flip;
      }
    if (move_freeze && !y_dir) {
      
      delta_position = (e.time-last_move_time)/delta_move_time;
      if (delta_position<0.0) delta_position=0.0;
      while (delta_position>1.0) { delta_position-=1.0; pos_x+=move_x; }
      if (flip) delta_position=1.0-delta_position;
      //if (has_flipped) delta_position=1.0-delta_position;
    }
    if (e.time > last_move_time+delta_move_time) {
      //delta_position-=1.0;
      //pos_x+=move_x;
      has_flipped=false;
      move_x=0;
      last_move_time = e.time;
      move_freeze = false;

      if (prepare_move_dir_x!=0||prepare_move_dir_y!=0)
	{
	  if (prepare_move_dir_x>0) flip=false;
	  if (prepare_move_dir_x<0) flip=true;
	  //pos_x += prepare_move_dir_x;
	  //pos_y += prepare_move_dir_y;
	  prepare_move_dir_x=0;
	  prepare_move_dir_y=0;
	  //move_freeze=true;
	}
    }
    if (move_freeze && !y_dir && e.time > last_change_time+delta_time) {
      last_change_time = e.time;
      tile++;
      if (tile>=player_end_tile-1) { tile-=(player_end_tile-player_start_tile-1); }
      //std::cout << "TILE: " << tile << " " << player_end_tile << " " << player_start_tile << std::endl;
    }

    int val = tiles->Tile(player_pos_x(),player_pos_y()+(delta_pos().y+64+64)/64);
    int val_x = dir_x!=0 ? tiles->Tile(player_pos_x()+dir_x,player_pos_y()+(delta_pos().y+64)/64) : -1;

    if ((!jump_ongoing && !is_transparent_tile(val_x)) || (jump_ongoing && !is_not_wall(val_x))) {
      dir_x=0;
    }
    
    if (player_pos_y()+(delta_pos().y+64+64)/64<3) val=5+8;
    //std::cout << "VAL=" << val << std::endl;
    if (fmod(-delta_pos().y,64)>56.0) {


    //if (x==int(player_pos_x()) && y==int(pl.player_pos_y()+(pl.delta_pos().y+64)/64)) return 3;
    
      if (jump_ongoing && jump_state==2 && !is_transparent_tile(val))
      {
	jump_ongoing=false;
	pos_y=pos_y+std::ceil(float(delta_pos().y)/64.0);
	jump_height=0.0;
      }
    }
    float fall_delta2 = e.time-fall_start_time;
    bool curr=false;
    if (!jump_ongoing && !fall_ongoing && is_transparent_tile(val)) {
      //std::cout << "Fall start" << std::endl;
      fall_ongoing=true;
      fall_start_time=e.time;
      curr=true;
    }
    if (fall_ongoing && !curr)
      {
	//std::cout << "Fall ongoing" << std::endl;
	fall_delta=fall_speed*fall_delta2;
	//if (fmod(-delta_pos().y-fall_delta,64)>56.0) {
	  int val = tiles->Tile(player_pos_x(),player_pos_y()+(fall_delta+delta_pos().y+64+64)/64);
	  if (player_pos_y()+(delta_pos().y+64+64)/64<3) val=5+8;
	  //std::cout << "VAL2=" << val << std::endl;
	  if (!is_transparent_tile(val)) {fall_ongoing=false; pos_y+=std::ceil(fall_delta/64.0);  fall_delta=0.0; }
	  //}
      }

  }
  virtual int player_pos_x() const { return pos_x; }
  virtual int player_pos_y() const { return pos_y; }
  virtual int player_pos_z() const { return 0; }
  virtual int player_tile() const {
    //std::cout << jump_ongoing << " " << jump_state << " " << fall_ongoing << " " << move_freeze << " " << dir_x << " " << flip << "->";
    // std::cout << weapon_ongoing << " " << flip << " " << current_weapon << std::endl;
    if (death_frame!=-1)
      {
	if (flip) {
	  return death_frame+56+5;
	} else {
	  return death_frame+56;
	}
      }
    
    if (weapon_ongoing)
      {
	int val=0;
	if(flip) {
	  val=52+current_weapon;
	} else {
	  val=48+current_weapon;
	}
	return val;
      }
    
    int jump_val = 0;
    if (jump_ongoing && jump_state==0) { jump_val=0; }
    if (jump_ongoing && jump_state==1) { jump_val=1; }
    if (jump_ongoing && jump_state==2) { jump_val=2; }
    if (jump_ongoing && jump_state==3) { jump_val=3; }
    if (fall_ongoing) { jump_val=3; }
    if ((fall_ongoing||jump_ongoing) && !flip) { jump_val+=40; }
    if ((fall_ongoing||jump_ongoing) && flip) { jump_val+=44; }
    if (fall_ongoing||jump_ongoing) { /*std::cout << "1::" << jump_val<< std::endl;*/ return jump_val; }
    if (!move_freeze && dir_x==0) { /*std::cout << "2::" << (!flip?player_end_tile-1:player_end_tile+(player_end_tile-1-player_start_tile)) << std::endl;*/ return !flip?player_end_tile-1:player_end_tile+(player_end_tile-1-player_start_tile); }
    /*std::cout << "3::" << (!flip?tile:player_end_tile+(tile-player_start_tile)) << std::endl;*/
    return !flip?tile:player_end_tile+(tile-player_start_tile); }
  virtual int player_type() const { return 0; }
  virtual float weapon_delta() const {
    int weapon_delta=0;
    if (death_frame!=-1&&flip) weapon_delta=-64; else
    if (weapon_ongoing&&flip) weapon_delta=-64;
    return weapon_delta;
  }    
  virtual Point delta_pos() const {
    int weapon_delta=0;
    if (death_frame!=-1&&flip) weapon_delta=-64; else
    if (weapon_ongoing&&flip) weapon_delta=-64;
    return Point(weapon_delta+delta_position*tile_sx-(flip?tile_sx:0),jump_height+fall_delta-64,0.0); }
  void reset()
  {
    pos_x=10; pos_y=9;
    flip=false;
    jump_ongoing=false;
    jump_state=0;
    fall_ongoing=0;
    jump_height=0.0;
    last_change_time=0.0;
    last_move_time=0.0;
    delta_position=0.0;
    jump_trigger=false;
    move_x=0;
    prepare_move_dir_x=0;
    prepare_move_dir_y=0;
    old_move_freeze=false;
    y_dir=false;
    old_flip=false;
    last_x=0;
    has_flipped=false;
    dir_x=0;
    jump_start_time=0.0;
    jump_aborted=false;
    jump_abort_delayed=false;
    fall_delta=0.0;
    fall_start_time=0.0;
  }
  bool weapon_at_kill() const { return current_weapon==2 || current_weapon==3; }
  bool weapon_active() const { return weapon_ongoing; }
  int player_dir() const {
    static int dir_cache=1;
    if (dir_x!=0) dir_cache=dir_x;
    return dir_cache; }
  void kill_player() { death_frame=0; }
  bool in_jump() const { return jump_ongoing; }
private:
  int pos_x, pos_y;
  int player_start_tile, player_end_tile;
  float last_change_time=0.0;
  int tile;
  int prepare_move_dir_x=0;
  int prepare_move_dir_y=0;
  int move_x=0;
  bool move_freeze=false;
  float last_move_time=0.0;
  float delta_time;
  float delta_move_time;
  bool flip=false;
  float delta_position=0.0;
  int tile_sx, tile_sy;
  bool old_move_freeze=false;
  bool y_dir=false;
  bool old_flip=false;
  int last_x=0;
  bool has_flipped=false;
  int dir_x=0;
  bool jump_ongoing=false;
  float jump_height=0.0;
  float jump_start_time=0.0;
  float jump_speed=300.0;
  float jump_max_height=0.0;
  bool jump_trigger=false;
  bool jump_aborted=false;
  bool jump_abort_delayed=false;
  int jump_state=0; // 0=going up, displaying 1st
                    // 1=going up, displaying 2nd
                    // 2=going down, displaying 3rd
                    // 3=going down, displaying 4th
  bool fall_ongoing=false;
  float fall_delta=0.0;
  float fall_start_time=0.0;
  float fall_speed=300.0;
  Tiles2d *tiles=0;
  bool weapon_start=false;
  bool weapon_ongoing=false;
  float weapon_start_time=0.0;
  int current_weapon=0;
  int death_frame=-1;
  TileHudInterface *hud;
};

class Game : public MainLoopItem, public TileSplashScreenCallback, public TileEnemyCallback
{
public:
  Game(GameApi::Env &env, GameApi::EveryApi &ev, int tile_sx, int tile_sy, std::string url, std::string url2, std::string url3, std::string url4,std::string homepage, std::string tiles_string, std::string tiles_string2, int start_pos_x, int start_pos_y, int player_start_tile, int player_end_tile, Bitmap<Color> *tile_bm, Bitmap<Color> *player_bm, Bitmap<Color> *player_flip_bm, Bitmap<Color> *ruohikko_bm, Bitmap<Color> *corn_bm, Bitmap<Color> *vesisade_bm, Bitmap<Color> *jump_bm, Bitmap<Color> *jump_flip_bm, GameApi::FI font, GameApi::BM status_bm, GameApi::BM splash, std::vector<GameApi::BM> item_types, std::vector<GameApi::BM> enemy_types, Bitmap<Color> *player_weapon_bm, Bitmap<Color> *player_weapon_flip_bm, std::vector<GameApi::BM> child_death, std::vector<GameApi::BM> child_death_flip, GameApi::BM aku_death, GameApi::BM aku_death_flip) : env(env), ev(ev), render(env,ev,tile_sx,tile_sy), tiles(env,url,homepage,tiles_string,tiles_string2), player(tile_sx,tile_sy,start_pos_x, start_pos_y, player_start_tile, player_end_tile, 0.1/4.0, 1.0/4.0,&hud), anim_tiles(env,&tiles,tiles_string,tiles_string2,url2,homepage), player_tiles(player, anim_tiles), tile_bm(tile_bm), player_bm(player_bm), player_flip_bm(player_flip_bm), ruohikko_bm(ruohikko_bm), corn_bm(corn_bm), vesisade_bm(vesisade_bm), jump_bm(jump_bm), jump_flip_bm(jump_flip_bm),hud(env,ev,font,status_bm), splashscreen(env,ev,splash,this), url3(url3), url4(url4), homepage(homepage), item_types(item_types), enemy_types(enemy_types), player_weapon_bm(player_weapon_bm), player_weapon_flip_bm(player_weapon_flip_bm), child_death(child_death), child_death_flip(child_death_flip) {
  BitmapHandle *handle5 = find_bitmap(env, aku_death);
  aku_death2 = find_color_bitmap(handle5);
  BitmapHandle *handle6 = find_bitmap(env, aku_death_flip);
  aku_death_flip2 = find_color_bitmap(handle6);


  }
  virtual void Collect(CollectVisitor &vis) {
    tile_bm->Collect(vis);
    player_bm->Collect(vis);
    jump_bm->Collect(vis);
    tiles.Collect(vis);
    anim_tiles.Collect(vis);
    hud.Collect(vis);
    splashscreen.Collect(vis);
    vis.register_obj(this);
    
  }
  virtual void HeavyPrepare() {
    std::vector<Bitmap<Color>*> tile_bms;
    tile_bms.push_back(tile_bm); // 0
    tile_bms.push_back(ruohikko_bm); // 1
    tile_bms.push_back(corn_bm); //2
    tile_bms.push_back(vesisade_bm); //3
    tile_bms.push_back(player_bm); //4
    tile_bms.push_back(player_flip_bm); //5
    tile_bms.push_back(jump_bm); //6
    tile_bms.push_back(jump_flip_bm); //7
    tile_bms.push_back(player_weapon_bm); //8
    tile_bms.push_back(player_weapon_flip_bm); // 9
    tile_bms.push_back(aku_death2); // 10
    tile_bms.push_back(aku_death_flip2); //11
    std::vector<int> tile_counts;
    tile_counts.push_back(5); // 0..5
    tile_counts.push_back(8); // 5..13
    tile_counts.push_back(3); // 13..16
    tile_counts.push_back(8); // 16 .. 24
    tile_counts.push_back(8); // 24 .. 32 
    tile_counts.push_back(8); // 32 .. 40
    tile_counts.push_back(4); // 40 .. 43
    tile_counts.push_back(4); // 43 .. 47
    tile_counts.push_back(4); // 47 .. 51
    tile_counts.push_back(4); // 51 .. 55
    tile_counts.push_back(5);
    tile_counts.push_back(5);
    render.set_tiles_2d(&player_tiles, tile_bms,tile_counts);
    player.SetTiles2d(&tiles);
    scr = new TileScroller2d(&player, 10.0, 64,64,1200,900);
    renderer = render.get_renderer(scr);
    renderer->Prepare();
    items_tile = new ItemsTile(env,ev,player,url3,homepage,64,64,scr,item_types,hud);
    items_tile->Prepare();
    enemy_tile = new EnemyTile(env,ev,player,url4,homepage,64,64,scr,enemy_types, hud,splashscreen,child_death, child_death_flip, this);
    enemy_tile->Prepare();
  }
  virtual void Prepare() {
    tile_bm->Prepare();
    player_bm->Prepare();
    tiles.Prepare();
    anim_tiles.Prepare();
    hud.Prepare();
    splashscreen.Prepare();
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
      player.execute(e);
      anim_tiles.execute(e);
      if (renderer)
	renderer->execute(e);
      hud.execute(e);
    items_tile->execute(e);
    enemy_tile->execute(e);
    splashscreen.execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (!splashscreen.enabled()) {
      player.handle_event(e);
      anim_tiles.handle_event(e);
      hud.handle_event(e);
      if (renderer)
	renderer->handle_event(e);
      items_tile->handle_event(e);
      enemy_tile->handle_event(e);
    } else {
      splashscreen.handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() { return renderer->shader_id(); }
  void reset()
  {
    if (items_tile)
      items_tile->HeavyPrepare();
    if (enemy_tile)
      enemy_tile->HeavyPrepare();
    hud.set_score(0);
    hud.set_lives(3);
    hud.set_health(100);
    player.reset();
    if (scr)
      scr->reset();
  }
  void add_random_item(int x, int y, float delta_x)
  {
    if (items_tile)
      items_tile->add_random_item(x,y,delta_x);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  TileRender2d render;
  NetworkTiles2d tiles;
  PlayerTile player;
  AnimTiles2d anim_tiles;
  PlayerTileMapping player_tiles;
  TileHud hud;
  TileSplashScreen splashscreen;
  TileScroller *scr=0;
  Bitmap<Color> *tile_bm;
  Bitmap<Color> *player_bm;
  Bitmap<Color> *player_flip_bm;
  Bitmap<Color> *ruohikko_bm;
  Bitmap<Color> *corn_bm;
  Bitmap<Color> *vesisade_bm;
  Bitmap<Color> *jump_bm;
  Bitmap<Color> *jump_flip_bm;
  Bitmap<Color> *player_weapon_bm;
  Bitmap<Color> *player_weapon_flip_bm;
  std::vector<GameApi::BM> child_death;
  std::vector<GameApi::BM> child_death_flip;
  Bitmap<Color> *aku_death2;
  Bitmap<Color> *aku_death_flip2;
  MainLoopItem *renderer=0;
  std::string url3;
  std::string url4;
  std::string homepage;
  std::vector<GameApi::BM> item_types;
  ItemsTile *items_tile;
  std::vector<GameApi::BM> enemy_types;
  EnemyTile *enemy_tile;
};


GameApi::ML GameApi::MainLoopApi::game(GameApi::EveryApi &ev, int tile_sx, int tile_sy, std::string url, std::string url2, std::string url3, std::string url4, std::string tiles_string, std::string tiles_string2, int start_pos_x, int start_pos_y, int player_start_tile, int player_end_tile, BM tile_bm, BM player_bm, BM ruohikko_bm, BM corn_bm, BM vesisade_bm, BM player_jump_bm, FI font, BM status_bm, BM splash, std::vector<BM> item_types, std::vector<BM> enemy_types, BM player_weapon_bm, std::vector<BM> child_death, BM aku_death)
{
  BitmapHandle *handle = find_bitmap(e, tile_bm);
  ::Bitmap<Color> *tile_bm2 = find_color_bitmap(handle);
  BitmapHandle *handle3 = find_bitmap(e, player_bm);
  ::Bitmap<Color> *player_bm2 = find_color_bitmap(handle3);
  GameApi::BM player_flipped = ev.bitmap_api.flip_tile_bitmap(player_bm,64,64,true);
  BitmapHandle *handle4 = find_bitmap(e, player_flipped);
  ::Bitmap<Color> *player_bm4 = find_color_bitmap(handle4);


  BitmapHandle *handle5 = find_bitmap(e, ruohikko_bm);
  ::Bitmap<Color> *ruohikko_bm2 = find_color_bitmap(handle5);
  BitmapHandle *handle6 = find_bitmap(e, corn_bm);
  ::Bitmap<Color> *corn_bm2 = find_color_bitmap(handle6);

  BitmapHandle *handle7 = find_bitmap(e, vesisade_bm);
  ::Bitmap<Color> *vesisade_bm2 = find_color_bitmap(handle7);

  BitmapHandle *handle8 = find_bitmap(e, player_jump_bm);
  ::Bitmap<Color> *jump_bm2 = find_color_bitmap(handle8);

  GameApi::BM player_jump_flipped = ev.bitmap_api.flip_tile_bitmap(player_jump_bm,64,64,true);

  BitmapHandle *handle9 = find_bitmap(e, player_jump_flipped);
  ::Bitmap<Color> *jump_bm4 = find_color_bitmap(handle9);
  

  BitmapHandle *handle10 = find_bitmap(e, player_weapon_bm);
  ::Bitmap<Color> *weapon_bm = find_color_bitmap(handle10);
 
  GameApi::BM player_weapon_flipped = ev.bitmap_api.flip_tile_bitmap(player_weapon_bm,128,128,true);

  BitmapHandle *handle11 = find_bitmap(e, player_weapon_flipped);
  ::Bitmap<Color> *weapon_flip_bm = find_color_bitmap(handle11);


  // BitmapHandle *handle12 = find_bitmap(e, child_death);
  //::Bitmap<Color> *child_death_bm = find_color_bitmap(handle12);

  std::vector<GameApi::BM> death_flip;
  int s = child_death.size();
  for(int i=0;i<s;i++) {
    GameApi::BM child_death_flipped = ev.bitmap_api.flip_tile_bitmap(child_death[i],64,64,true);
    death_flip.push_back(child_death_flipped);
  }
  

  GameApi::BM aku_death_flip = ev.bitmap_api.flip_tile_bitmap(aku_death,128,128,true);
  
  //BitmapHandle *handle13 = find_bitmap(e, child_death_flipped);
  //::Bitmap<Color> *child_death_flip_bm = find_color_bitmap(handle13);
  
  return add_main_loop(e, new Game(e,ev,tile_sx, tile_sy, url, url2, url3,url4,gameapi_homepageurl, tiles_string, tiles_string2, start_pos_x, start_pos_y, player_start_tile, player_end_tile, tile_bm2, player_bm2, player_bm4, ruohikko_bm2, corn_bm2,vesisade_bm2, jump_bm2, jump_bm4, font,status_bm, splash,item_types,enemy_types, weapon_bm, weapon_flip_bm, child_death, death_flip,aku_death, aku_death_flip));
}
