
#include "GameApi_h.hh"


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
EXPORT GameApi::ShaderApi::ShaderApi(Env &e) : e(e)
{
  priv = (void*)new ShaderPriv2;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = 0;
  p->seq = 0;
}
EXPORT GameApi::ShaderApi::~ShaderApi()
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  delete p->file;
  delete p->seq;
  delete (ShaderPriv2*)priv;
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
  GradientBitmap2(Point2d pos_1, Point2d pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy) : pos_1(pos_1), pos_2(pos_2), color_1(color_1), color_2(color_2), sx(sx), sy(sy) { }
  void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    //std::cout << "gradient" << std::endl;
    Vector u = Point(x, y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    Vector u_x = Point(pos_2.x, pos_2.y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    float val = Vector::DotProduct(u, u_x);
    float v = u_x.Dist();
    val /= v;
    val /= v; 
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
  ::Bitmap<Color> *b = new ConstantBitmap<Color>(Color(color), sx,sy);
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
  ::Bitmap<Color> *b = new ConstantBitmap<Color>(Color(0,0,0), sx,sy);
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = b;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

BufferRef LoadImage(std::string filename, bool &success);

EXPORT void GameApi::BitmapApi::savebitmap(BM bm, std::string filename, bool alpha)
{  
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);
  PpmFile file(filename, *bm2, alpha);
  std::string pngcontents = file.Contents();
  std::ofstream filehandle(filename.c_str(), std::ios_base::out|std::ios::binary);
  filehandle.write(pngcontents.c_str(), pngcontents.size()); // << pngcontents;
  filehandle.close();
}

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
  BM bm = add_bitmap(e, handle2);
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

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, FB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  FloatBitmap *handle3 = find_float_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<float> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked(*bm1, *bm2, x,y, *bm3));
}

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, BB mask)
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

class FlipBitmap : public Bitmap<Color>
{
public:
  FlipBitmap(Bitmap<Color> &bm, bool x, bool y) : bm(bm), flip_x(x), flip_y(y) { }
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
EXPORT GameApi::BM GameApi::FloatBitmapApi::subfloatbitmap(FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  GameApi::BB b = to_bool(fb, range_start, range_end);
  Color c(true_color);
  Color c2(false_color);
  GameApi::BM bm = ev->bool_bitmap_api.to_bitmap_1(b, c.r, c.g, c.b, c.alpha,
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

EXPORT GameApi::BB GameApi::BoolBitmapApi::empty(int sx, int sy)
{
  return add_bool_bitmap(e, new ConstantBitmap<bool>(false, sx,sy));
}

EXPORT GameApi::FB GameApi::FloatBitmapApi::empty(int sx, int sy)
{
  return add_float_bitmap(e, new ConstantBitmap<float>(0.0, sx,sy));
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


EXPORT GameApi::BB GameApi::BoolBitmapApi::rectangle(BB bg, float x, float y, float width, float height)
{

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
}

class BoolBitmapSprite : public Bitmap<bool>
{
public:
  BoolBitmapSprite(Bitmap<bool> &bg, Bitmap<bool> &sprite, float x, float y, float mult_x, float mult_y) : bg(bg), sprite(sprite), x(x), y(y), mult_x(mult_x), mult_y(mult_y) { }
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
EXPORT GameApi::BM GameApi::BoolBitmapApi::to_bitmap(BB bools,
					      int true_r, int true_g, int true_b, int true_a,
					      int false_r, int false_g, int false_b, int false_a)
{
  return to_bitmap_1(bools, true_r, true_g, true_b, true_a,
		     false_r, false_g, false_b, false_a);
}
GameApi::BM GameApi::BoolBitmapApi::to_bitmap_1(BB bools,
					      int true_r, int true_g, int true_b, int true_a,
					      int false_r, int false_g, int false_b, int false_a)
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
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_float_bitmap(e, new BitmapFromFunction<float>(f,sx,sy));
}

class BitmapFromRed : public Bitmap<float>
{
public:
  BitmapFromRed(Bitmap<Color> &bm) : bm(bm) { }
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
  void Prepare() 
  { 
    bm->Prepare(); 
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    array_x = new float[sx*sy];
    array_y = new float[sx*sy];
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

  DistanceFieldBitmap(Bitmap<float> *bm, float max_value) : bm(bm), max_value(max_value) 
  {
  }
  int SizeX() const { return bm->SizeX(); }
  int SizeY() const { return bm->SizeY(); }
  float Map(int x, int y) const
  {
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
  float *array_x;
  float *array_y;
  float max_value;
};
class BorderFloatBitmap : public Bitmap<float>
{
public:
  BorderFloatBitmap(Bitmap<float> *bm) : bm(bm) { }
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
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  
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

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::empty(float x, float y)
{
  return constant(0x00000000, x, y);
}
class DistanceRenderContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  DistanceRenderContinuousBitmap(DistanceRenderable *dist, ColorVolumeObject *colours, float sx, float sy) : dist(dist), colours(colours),sx(sx),sy(sy) { }
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
  int shader_id() { return -1; }
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

  GLuint id;
  glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  glClientActiveTexture(GL_TEXTURE0+0);
#endif
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sx,sy, 0, GL_RED, GL_FLOAT, array);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
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
  virtual void Prepare()
  {
    if (!res)
      {
	std::ifstream ss(filename.c_str());
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

