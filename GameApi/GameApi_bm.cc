 
#include "GameApi_h.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "GameApi_low.hh"

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

EXPORT GameApi::BMA GameApi::BitmapApi::empty_array()
{
  return array(NULL, 0);
}
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
GameApi::BMA GameApi::BitmapApi::array(BM *array, int size)
{
  return add_bitmap_array(e, new BitmapArrayImpl(this, array, size));
}

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
  BitmapArray2<Color> *arr = find_bitmap_array(e, array);
  return add_color_bitmap2(e, new BitmapArrayElem(arr, i));
}

EXPORT GameApi::BM GameApi::BitmapApi::alpha_color(BM orig, unsigned int color_key)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b = new AlphaColorBitmap(*b2, color_key);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
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

EXPORT GameApi::BM GameApi::BitmapApi::radial_gradient(int sx, int sy, PT pos, float r1, float r2, unsigned int color_1, unsigned int color_2)
{
  Point *pos_p1 = find_point(e, pos);
  Point2d pos_p = { pos_p1->x, pos_p1->y };
   ::Bitmap<Color> *b = new RadialGradient(sx,sy,pos_p, r1,r2, color_1, color_2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
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
}
EXPORT GameApi::BM GameApi::BitmapApi::function(std::function<unsigned(int,int)> f, int sx, int sy)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<unsigned int> *bm = new BitmapFromFunction<unsigned int>(f,sx,sy);  
  return add_color_bitmap(e, new BitmapFromUnsignedInt(*bm));
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
  SaveBitmapML(GameApi::EveryApi &ev, GameApi::BM bm, std::string filename, bool alpha, float time) : ev(ev), bm(bm), filename(filename), alpha(alpha), time(time) { }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (time > e.time*10.0 && time < e.time*10.0 + e.delta_time)
      {
	ev.bitmap_api.savebitmap(bm, filename, alpha);
      }
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::EveryApi &ev;
  GameApi::BM bm;
  std::string filename;
  bool alpha;
  float time;
};
EXPORT GameApi::ML GameApi::BitmapApi::savebitmap_ml(EveryApi &ev, BM bm, std::string filename, bool alpha, float time)
{
  return add_main_loop(e, new SaveBitmapML(ev, bm, filename, alpha, time));
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
  emscripten_run_script("stackTrace()");
#endif
}
void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max=15);

extern std::string gameapi_homepageurl;

class LoadBitmapFromUrl : public Bitmap<Color>
{
public:
  LoadBitmapFromUrl(GameApi::Env &env, std::string url, std::string homepage) : env(env), url(url),homepage(homepage) { cbm = 0; }

  virtual int SizeX() const {
    if (!cbm) { return 100; }
    return cbm->SizeX(); }
  virtual int SizeY() const {
    if (!cbm) { return 100; } 
    return cbm->SizeY(); }
  virtual Color Map(int x, int y) const { 
    if (!cbm) { return Color(0xffffffff); }
    return cbm->Map(x,y); }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    Prepare();
  }
  void Prepare()
  {
    if (!cbm) {
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::vector<unsigned char> vec2(vec->begin(),vec->end());
      
      bool b = false;
      img = LoadImageFromString(vec2, b);
      
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
    }
  }    
private:
  GameApi::Env &env;
  std::string url;
  std::string homepage;
  BufferRef img;
  Bitmap<Color> *cbm=0;
  bool load_finished = false;  
};
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
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at SizeX()" << std::endl; }

    return cbm->SizeX(); }
  virtual int SizeY() const {
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at SizeY()" << std::endl; } 
    return cbm->SizeY(); }
  virtual Color Map(int x, int y) const { 
    if (!cbm) { std::cout << "LoadBitmapBitmap::Prepare() for Bitmap not called at Map()" << std::endl; }
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
private:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
  mutable Bitmap<Color> *bm_cache=0;
};

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
}
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
  BitmapTileHandle *chandle = dynamic_cast<BitmapTileHandle*>(handle);
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

BitmapHandle *subbitmap_h(BitmapHandle *handle, int x, int y, int width, int height)
{
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
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
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
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
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(orig);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(orig);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(orig);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(orig);
  //BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle1)
    { // color
      BitmapColorHandle *ahandle1 = dynamic_cast<BitmapColorHandle*>(bm);
      Bitmap<Color> *c = new ModifyBitmap<Color>(*chandle1->bm, *ahandle1->bm, x,y); 
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle2)
    { // color
      BitmapIntHandle *ahandle1 = dynamic_cast<BitmapIntHandle*>(bm);
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
      BitmapPosHandle *ahandle1 = dynamic_cast<BitmapPosHandle*>(bm);
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
  BitmapIntHandle *handle2 = dynamic_cast<BitmapIntHandle*>(handle);
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
  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
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
  BitmapColorHandle *h1 = dynamic_cast<BitmapColorHandle*>(handle1);
  BitmapColorHandle *h2 = dynamic_cast<BitmapColorHandle*>(handle2);
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
  //EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  BitmapColorHandle *handle = new BitmapColorHandle;
  ::Bitmap<unsigned int> *bm = new BitmapFromString<unsigned int>(array, sx, sy, f);
  env->deletes.push_back(std::shared_ptr<void>(bm));
  handle->bm = new BitmapFromUnsignedInt(*bm);
  return add_bitmap(e, handle);
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
  i.wave_int = new SingleAnimInt(val, duration);
  i.wave_point = 0;
  i.wave_float = 0;
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
EXPORT GameApi::BM GameApi::BitmapApi::dup_x(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new DupXBitmap(*chandle->bm);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}

EXPORT GameApi::BM GameApi::BitmapApi::flip_x(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, true, false);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::flip_y(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, false, true);

  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::rot90(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new Rot90Bitmap(*chandle->bm);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e, chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::repeat_bitmap(BM orig, int xcount, int ycount)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
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
EXPORT GameApi::BB GameApi::BoolBitmapApi::line(BB bg, float p_x, float p_y, float p2_x, float p2_y, float line_width1, float line_width2)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d p = { p_x, p_y };
  Point2d p2 = { p2_x, p2_y };
  return add_bool_bitmap(e, new LineBoolBitmap(*bm, p,p2, line_width1,line_width2));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::ellipse(BB bg, float center_x, float center_y, float center2_x, float center2_y, float sum_of_distances)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d center = { center_x, center_y };
  Point2d center2 = { center2_x, center2_y };
  return add_bool_bitmap(e, new EllipseBoolBitmap(*bm, center, center2, sum_of_distances));
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
  return add_color_bitmap2(e, new ConicalGradientBitmap(sx,sy,x,y, angle1,angle2,color_1,color_2));
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::sections(int sx, int sy, float x, float y, std::function<bool (float angle)> f)
{
  return add_bool_bitmap(e, new SectionsBoolBitmap(sx,sy,x,y,f));
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
EXPORT GameApi::FB GameApi::FloatBitmapApi::space_fill(PT *array, float *array2, int size, int sx, int sy)
{
  Point *array3 = new Point[size];
  for(int i=0;i<size;i++)
    array3[i] = *find_point(e, array[i]);
  return add_float_bitmap(e, new SpaceFillFloatBitmap(array3, array2, size, sx,sy));
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

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_color(FB r, FB g, FB b, FB a)
{
  Bitmap<float> *rb = find_float_bitmap(e,r)->bitmap;
  Bitmap<float> *gb = find_float_bitmap(e,g)->bitmap;
  Bitmap<float> *bb = find_float_bitmap(e,b)->bitmap;
  Bitmap<float> *ab = find_float_bitmap(e,a)->bitmap;
  Bitmap<Color> *bm2 = new BitmapFromRGBA(*rb,*gb,*bb,*ab);
  return add_color_bitmap2(e, bm2);
}
GameApi::BB GameApi::FloatBitmapApi::to_bool(FB fb, float true_range_start, float true_range_end)
{
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  return add_bool_bitmap(e, new FloatRangeBitmap(*f, true_range_start, true_range_end));
}

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

GameApi::BB GameApi::FloatBitmapApi::to_bool_mod(FB fb, float mod_value)
{
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  return add_bool_bitmap(e, new FloatModBitmap(*f, mod_value));
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
}

EXPORT GameApi::ContinuousBitmapApi::ContinuousBitmapApi(Env &e) : e(e) { }

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::cbm_empty(float x, float y)
{
  return constant(0x00000000, x, y);
}
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
GameApi::CBM GameApi::ContinuousBitmapApi::distance_render(FD obj, COV colors, float sx, float sy)
{
  DistanceRenderable *dist = find_distance(e, obj);
  ColorVolumeObject *colorsI = find_color_volume(e, colors);
  return add_continuous_bitmap(e, new DistanceRenderContinuousBitmap(dist, colorsI, sx,sy));
}
GameApi::CBM GameApi::ContinuousBitmapApi::constant(unsigned int color, float x, float y)
{
  return add_continuous_bitmap(e, new ConstantContinuousBitmap<Color>(x,y,Color(color)));
}



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

GameApi::CBM GameApi::ContinuousBitmapApi::bicubic(float f_0, float f_1, float df_0, float df_1,
						   float ff_0, float ff_1, float dff_0, float dff_1)
{
  return add_continuous_bitmap(e, new BiCubicInterpolateBitmap(f_0,f_1,df_0,df_1, ff_0, ff_1, dff_0, dff_1));
}
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

  return add_continuous_bitmap(e, new FunctionContinuousBitmap( f, sx, sy));
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

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::surfacecolor(S s, COV cov)
{
  SurfaceImpl *impl = find_surface(e, s);
  ColorVolumeObject *obj = find_color_volume(e, cov);
  return add_continuous_bitmap(e, new ComposeSurfaceColor(impl, obj));
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
EXPORT GameApi::ML GameApi::SpriteApi::bitmap_anim_ml(EveryApi &ev, std::vector<BM> vec,
						      std::vector<float> key_frames,
						      float repeat_time)
{
 
  return add_main_loop(e, new BitmapAnimML(ev, *this, vec, key_frames, repeat_time));
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
  SaveImage(ref, filename);
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
  SaveRawBitmap(Bitmap<Color> &bm, std::string filename) : bm(bm), filename(filename) { }
  void Collect(CollectVisitor &vis)
  {
    bm.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    std::ofstream file(filename.c_str(), std::ios_base::out|std::ios_base::binary);
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
    file.close();
  }
  
  virtual void Prepare()
  {
    bm.Prepare();
    std::ofstream file(filename.c_str(), std::ios_base::out|std::ios_base::binary);
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
    file.close();
  }
  virtual void execute(MainLoopEnv &e)
  {
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
private:
  Bitmap<Color> &bm;
  std::string filename;
};
void save_raw_bitmap(GameApi::Env &e, GameApi::BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *c = find_color_bitmap(handle);
  SaveRawBitmap *save = new SaveRawBitmap(*c, filename);
  save->Prepare();
}

class SaveRawBitmapML : public MainLoopItem
{
public:
  SaveRawBitmapML(Bitmap<Color> *bm, std::string filename) : bm(bm), filename(filename) { }
  void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() { Prepare(); }

  void Prepare() {
    SaveRawBitmap *save = new SaveRawBitmap(*bm, filename);
    save->Prepare();
  }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
private:
  Bitmap<Color> *bm;
  std::string filename;
  
};
GameApi::ML GameApi::BitmapApi::save_raw(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  return add_main_loop(e, new SaveRawBitmapML(bitmap, filename));
}

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

GameApi::BM load_raw_bitmap(GameApi::Env &e, std::string filename)
{
  LoadRawBitmap *b = new LoadRawBitmap(filename);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  GameApi::BM bm = add_bitmap(e, handle2);
  return bm;
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

