
#include "GameApi_h.hh"
#include <iostream>
#include <iomanip>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

struct FontPriv
{
};

EXPORT GameApi::FontApi::FontApi(Env &e) : e(e) 
{
  priv = new FontPriv;
}
EXPORT GameApi::FontApi::~FontApi()
{
  delete (FontPriv*)priv;
}

EXPORT GameApi::Ft GameApi::FontApi::newfont(std::string filename, int sx, int sy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Font22 fnt; 
#ifdef HAS_FREETYPE
  //std::cout << &env->lib << std::endl;
  fnt.bm = new FontGlyphBitmap(e, (void*)&env->lib,filename.c_str(), sx,sy);
#endif
  env->fonts.push_back(fnt); 
  GameApi::Ft font;
  font.id = env->fonts.size()-1;
  return font;
}
EXPORT GameApi::LI GameApi::FontApi::glyph_outline(GameApi::Ft font, long idx, float sx, float sy)
{
  std::cout << "ERROR, glyph_outline error" << std::endl; 
  // COMMENTED OUT BECAUSE DYNAMIC CAST DISABLED 
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bm = env->fonts[font.id].bm;
#ifdef EMSCRIPTEN
  FontGlyphBitmap *bm2 = static_cast<FontGlyphBitmap*>(bm); 
  LineCollection *bm3 = static_cast<LineCollection*>(bm);
#else
  FontGlyphBitmap *bm2 = dynamic_cast<FontGlyphBitmap*>(bm); 
  LineCollection *bm3 = dynamic_cast<LineCollection*>(bm);
#endif
  bm2->load_glyph_outline(idx, sx, sy);
  LineCollection *coll = bm3;
  LineCollection *coll2 = new ForwardLineCollection(coll);
  return add_line_array(e, coll2);
#endif
  GameApi::LI li = { -1 };
  return li;
}
EXPORT GameApi::PL GameApi::FontApi::glyph_plane(GameApi::Ft font, long idx, float sx, float sy, float dx, float dy)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bmA = env->fonts[font.id].bm;
  FontGlyphBitmap *bm2 = static_cast<FontGlyphBitmap*>(bmA);
  bm2->load_glyph_outline(idx,sx,sy);
  LineCollection *coll = bm2;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)coll;
  PlanePoints2d *plane = new FontLineCollectionWrapper(coll, bm->Types(), sx, sy, dx,dy);
  return add_plane(e, plane);
#endif
  GameApi::PL pl = { -1 };
  return pl; 
}

class GlyphChooser : public Bitmap<int>
{
public:
  GlyphChooser(long idx, FontGlyphBitmap &bm) : idx(idx),bm(bm) { }
  void Collect(CollectVisitor &vis) {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { bm.Prepare(); }
  int SizeX() const {
    bm.load_glyph(idx);
    return bm.SizeX(); 
  }
  int SizeY() const {
    bm.load_glyph(idx);
    return bm.SizeY(); 
  }
  int Map(int x, int y) const {
    bm.load_glyph(idx);
    return bm.Map(x,y);
  }
private:
  long idx;
  FontGlyphBitmap &bm;
};

EXPORT GameApi::BM GameApi::FontApi::glyph(GameApi::Ft font, long idx)
{

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bmA = env->fonts[font.id].bm;
  FontGlyphBitmap *bm3 = static_cast<FontGlyphBitmap*>(bmA);
  //bm3->load_glyph(idx);
  Bitmap<int> *bm4 = new GlyphChooser(idx, *bm3);
  //Bitmap<int> *bm = env->fonts[font.id].bm;
  Bitmap<Color> *cbm = new MapBitmapToColor(0,256,Color(255,255,255,255), Color(255,255,255,0), *bm4);
  //MemoizeBitmap *mbm = new MemoizeBitmap(*cbm);
  //mbm->MemoizeAll();
  env->deletes.push_back(std::shared_ptr<void>(cbm)); 
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = cbm;
  BM bm2 = add_bitmap(e,chandle2); 
  //FB bm2_a = ev.float_bitmap_api.from_red(bm2);
  //BM bm2_b = ev.float_bitmap_api.to_grayscale_color(bm2_a, 255,255,255,255, 0,0,0,0);
  return bm2;
}
EXPORT GameApi::FtA GameApi::FontApi::font_atlas_info(EveryApi &ev, Ft font, std::string chars, float sx, float sy, int y_delta)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int s = chars.size();
  int counter = 0;
  int y = 0;
  int x = 0;
  FontAtlasInfo *info = new FontAtlasInfo;
  int atlas_sx=0;
  int atlas_sy=0;
  for(int i=0;i<s;i++)
    {
      char ch = chars[i];
      std::cout << ch << std::flush;
      BM bm = glyph(font, ch);
      Bitmap<int> *bmA = env->fonts[font.id].bm;
      FontGlyphBitmap *bm2 = static_cast<FontGlyphBitmap*>(bmA);
      int top = bm2->bitmap_top(ch);
      int xx = x;
      int yy = y*y_delta;
      int sx = ev.bitmap_api.size_x(bm);
      int sy = ev.bitmap_api.size_y(bm);
      FontAtlasGlyphInfo info2;
      info2.sx = sx;
      info2.sy = sy;
      info2.x = xx;
      info2.y = yy;
      info2.top = top;

      info->char_map[ch] = info2;
      
      x+=ev.bitmap_api.size_x(bm);      
      if (atlas_sx < x) { atlas_sx = x; }
      if (atlas_sy < y*y_delta + top + ev.bitmap_api.size_y(bm)) { atlas_sy = y*y_delta + top + ev.bitmap_api.size_y(bm); }

      if (counter==10) { y++; counter=-1; x=0; }
      counter++;
    }
  info->atlas_sx = atlas_sx;
  info->atlas_sy = atlas_sy;
  return add_font_atlas(e, info);
}

EXPORT GameApi::BM GameApi::FontApi::font_atlas(EveryApi &ev, Ft font, FtA atlas, float sx, float sy)
{
  FontAtlasInfo *info = find_font_atlas(e, atlas);
  BM bg = ev.bitmap_api.newbitmap(info->atlas_sx, info->atlas_sy, 0x00000000);
  std::map<int, FontAtlasGlyphInfo>::iterator i = info->char_map.begin();
  for(;i!=info->char_map.end();i++)
    {
      std::pair<int,FontAtlasGlyphInfo> p = *i;
      int ch = p.first;
      std::cout << ch << std::flush;
      BM bm = glyph(font, ch);
      bg = ev.bitmap_api.blitbitmap(bg, bm, p.second.x, p.second.y+p.second.top);
    }
  return bg;
}

struct FontCacheData
{
  GameApi::BM atlas_bm;
  char ch;
  GameApi::BM bm;
};
std::vector<FontCacheData*> font_cache;
FontCacheData *find_font_cache_data(GameApi::BM atlas_bm, char ch)
{
  int s = font_cache.size();
  for(int i=0;i<s;i++) { if (atlas_bm.id == font_cache[i]->atlas_bm.id && font_cache[i]->ch == ch) return font_cache[i];
  }
  return 0;
}


class FontAtlasStringFaceCollection : public FaceCollection
{
public:
  FontAtlasStringFaceCollection(GameApi::Env &e, std::vector<GameApi::BM> vec, std::vector<FontAtlasGlyphInfo> info, int x_gap, GameApi::BM atlas_bm) : e(e), vec(vec), info(info), x_gap(x_gap), atlas_bm(atlas_bm) { }
  virtual std::string name() const { return "FontAtlasStringFaceCollection"; }

  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual int NumFaces() const { return std::min(vec.size(),info.size()); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int s = vec.size();
    Point topleft = { 0.0, 0.0, 0.0 };
    for(int i=0;i<face;i++)
      {
	Bitmap<::Color> *bbm = find_bitmap2(e,vec[i]);
	topleft.x+=bbm->SizeX() + x_gap;
      }
    topleft.y = info[face].top;

    if (point==1||point==2) topleft.x+=find_bitmap2(e,vec[face])->SizeX();
    if (point==2||point==3) topleft.y+=find_bitmap2(e,vec[face])->SizeY();
    
    return topleft;
  }
  virtual Vector PointNormal(int face, int point) const { return Vector(0.0,0.0,-1.0); }
  virtual float Attrib(int face, int point, int id) const { return 0.0f; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d topleft = { float(info[face].x), float(info[face].y+info[face].top) };
    if (point==1||point==2) topleft.x+=find_bitmap2(e,vec[face])->SizeX();
    if (point==2||point==3) topleft.y+=find_bitmap2(e,vec[face])->SizeY();
    topleft.x/=(float)find_bitmap2(e,atlas_bm)->SizeX();
    topleft.y/=(float)find_bitmap2(e,atlas_bm)->SizeY();
    return topleft;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
private:
  GameApi::Env &e;
  std::vector<GameApi::BM> vec;
  std::vector<FontAtlasGlyphInfo> info;
  int x_gap;
  GameApi::BM atlas_bm;
};

EXPORT GameApi::P GameApi::FontApi::font_string_from_atlas_opengl_pipeline(EveryApi &ev, FtA atlas, BM atlas_bm, std::string str, int x_gap)
{
  FontAtlasInfo *info = find_font_atlas(e, atlas);
  int sz = str.length();
  std::vector<BM> vec;
  std::vector<FontAtlasGlyphInfo> vec2;
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      FontCacheData *d = find_font_cache_data(atlas_bm,ch);
      BM bm;
      FontAtlasGlyphInfo ii = info->char_map[ch];
      if (!d) {
	bm = ev.bitmap_api.subbitmap(atlas_bm, ii.x, ii.y+ii.top, ii.sx,ii.sy);
	FontCacheData *dd = new FontCacheData;
	dd->atlas_bm = atlas_bm;
	dd->ch = ch;
	dd->bm = bm;
	font_cache.push_back(dd);
      } else
	{
	  bm = d->bm;
	}
      
      vec.push_back(bm);
      vec2.push_back(ii);      
    }
  return add_polygon2(e,new FontAtlasStringFaceCollection(e,vec,vec2,x_gap,atlas_bm),1);
}


EXPORT GameApi::BM GameApi::FontApi::font_string_from_atlas(EveryApi &ev, FtA atlas, BM atlas_bm, std::string str, int x_gap)
{
  FontAtlasInfo *info = find_font_atlas(e, atlas);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  int sz = str.length();
  FontCharacterString<Color> *array = new FontCharacterString<Color>(Color(0.0f,0.0f,0.0f,0.0f), x_gap);
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      FontCacheData *d = find_font_cache_data(atlas_bm,ch);
      BM bm;
      FontAtlasGlyphInfo ii = info->char_map[ch];
      if (!d) {
	bm = ev.bitmap_api.subbitmap(atlas_bm, ii.x, ii.y+ii.top, ii.sx,ii.sy);
	FontCacheData *dd = new FontCacheData;
	dd->atlas_bm = atlas_bm;
	dd->ch = ch;
	dd->bm = bm;
	font_cache.push_back(dd);
      } else
	{
	  bm = d->bm;
	}
	//glyph(font, ch);
      int top = ii.top; //env->fonts[font.id].bm->bitmap_top(ch);
      BitmapHandle *handle = find_bitmap(e,bm);
      Bitmap<Color> *col = find_color_bitmap(handle);
      array->push_back(col, top);
    }
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = array;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::FtA GameApi::FontApi::load_atlas(std::string filename)
{
  FontAtlasInfo *info = new FontAtlasInfo;
  std::ifstream ss(filename.c_str());
  ss.tie(nullptr);
  //char c;
  ss >> info->atlas_sx >> info->atlas_sy;
  int num;
  while(ss >> num)
    {
      int sx;
      int sy;
      int x;
      int y;
      int top;
      ss >> sx;
      ss >> sy;
      ss >> x;
      ss >> y;
      ss >> top;
      FontAtlasGlyphInfo i;
      i.sx = sx;
      i.sy = sy;
      i.x = x;
      i.y = y;
      i.top = top;
      info->char_map[num] = i;
    }
  return add_font_atlas(e, info);
}
EXPORT void GameApi::FontApi::save_atlas(FtA atlas, std::string filename)
{
  FontAtlasInfo *info = find_font_atlas(e, atlas);
  std::ofstream ss(filename.c_str());
  ss << info->atlas_sx << " " << info->atlas_sy << std::endl;
  std::map<int, FontAtlasGlyphInfo>::iterator i = info->char_map.begin();
  for(;i!=info->char_map.end();i++)
    {
      std::pair<int,FontAtlasGlyphInfo> p = *i;
      ss << p.first << std::endl;
      ss << p.second.sx << " " << p.second.sy << std::endl;
      ss << p.second.x << " " << p.second.y << std::endl;
      ss << p.second.top << std::endl;
    }
  ss.close();
}

EXPORT GameApi::ARR GameApi::FontApi::font_string_array2(FI font, std::string str)
{
  int sz = str.size();
  std::vector<int> bms;
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      std::string s;
      s+=ch;
      BM bm = draw_text_string(font, s, 0, 0);
      bms.push_back(bm.id);
    }

  ArrayType *val = new ArrayType;
  val->type = E_BM;
  val->vec = bms;
  return add_array(e, val);
}

EXPORT GameApi::ARR GameApi::FontApi::font_string_array(Ft font, std::string str, int x_gap)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  int sz = str.size();
  std::vector<int> bms;
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      BM bm = glyph(font, ch);
      bms.push_back(bm.id);      
    }
  ArrayType *val = new ArrayType;
  val->type = E_BM;
  val->vec = bms;
  return add_array(e, val);
}
class DynPoly : public MainLoopItem
{
public:
  DynPoly(GameApi::Env &env, GameApi::EveryApi &ev, Fetcher<int> *fetch, std::vector<GameApi::P> vec, GameApi::MT mat) : env(env), ev(ev), fetch(fetch), vec(vec), mat(mat) {
    firsttime = true;
    sh.id = -1;
  }
  void first_time_calc(MainLoopEnv &e)
  {
    std::vector<GameApi::ML> va_vec;
    int s = vec.size();
    Material *mat2 = find_material(env, mat);
    for(int i=0;i<s;i++)
      {
	int val = mat2->mat(vec[i].id);
	GameApi::ML ml;
	ml.id = val;
	va_vec.push_back(ml);

	MainLoopItem *item = find_main_loop(env,ml);
	item->execute(e);

      }
    vas = va_vec;
    sh.id = -1;
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() {
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      first_time_calc(e);
      firsttime = false;
    }

    //sh.id = e.sh_texture;
    fetch->frame(e);
    int idx = fetch->get();
    int s = vas.size();
    if (idx>=0 && idx<s) {
      //ev.shader_api.use(sh);
      //GameApi::M m = add_matrix2(ev.get_env(),e.in_MV);
      ////GameApi::M m2 = ev.matrix_api.trans(x,y,0);
      //GameApi::M mm = ev.matrix_api.mult(m,m2);
      //ev.shader_api.set_var(sh, "in_MV", mm);
      //ev.polygon_api.render_vertex_array(vas[idx]);
      GameApi::ML ml = vas[idx];
      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e) { fetch->event(e); 
    int idx = fetch->get();
    int s = vas.size();
    if (idx>=0 && idx<s) {
      GameApi::ML ml = vas[idx];
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  Fetcher<int> *fetch;
  std::vector<GameApi::P> vec;
  std::vector<GameApi::ML> vas;
  GameApi::SH sh;
  bool firsttime;
  GameApi::MT mat;
};
void BitmapToSourceBitmap(Bitmap<Color> &bm, SourceBitmap &target, DrawBufferFormat fmt);

class DynChar_fb : public FrameBufferLoop
{
public:
  DynChar_fb(GameApi::Env &e, GameApi::EveryApi &ev, Fetcher<int> *fetch, std::vector<GameApi::BM> vec, int x, int y, int fmt, Movement *move) : e(e), ev(ev), fetch(fetch), vec(vec), x(x), y(y), fmt(fmt?D_RGBA8888:D_Mono1), move(move) {
  }
  void Collect(CollectVisitor &vis) {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	BitmapHandle *handle = find_bitmap(e, vec[i]);
	::Bitmap<Color> *b2 = find_color_bitmap(handle);
	b2->Collect(vis);
      }
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	BitmapHandle *handle = find_bitmap(e, vec[i]);
	::Bitmap<Color> *b2 = find_color_bitmap(handle);
	//b2->Prepare();
	SourceBitmap src_bm(fmt,0);
	src.push_back(src_bm);
	BitmapToSourceBitmap(*b2, src[i], fmt);

      }

  }
  virtual void Prepare()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	BitmapHandle *handle = find_bitmap(e, vec[i]);
	::Bitmap<Color> *b2 = find_color_bitmap(handle);
	b2->Prepare();
	SourceBitmap src_bm(fmt,0);
	src.push_back(src_bm);
	BitmapToSourceBitmap(*b2, src[i], fmt);

      }
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    fetch->draw_event(e);
    move->draw_event(e);
  }
  virtual void frame(DrawLoopEnv &e)
  {
    fetch->draw_frame(e);
    move->draw_frame(e);
    int idx = fetch->get();
    int s = src.size();
    if (idx>=0 && idx<s)
      {
	Point p = { float(x), float(y), 0.0 };
	
	Matrix m = move->get_whole_matrix(e.time*10.0, (e.delta_time*10.0));
	Point p2 = p*m;
	e.drawbuffer->draw_sprite(&src[idx], p2.x, p2.y);
      }
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Fetcher<int> *fetch;
  std::vector<GameApi::BM> vec;
  std::vector<SourceBitmap> src;
  int x,y;
  DrawBufferFormat fmt;
  bool firsttime;
  Movement *move;
};

int dynchar_selection = -1;
Matrix dynchar_position;

extern bool g_chooser_flag;

class DynChar : public MainLoopItem
{
public:
  DynChar(GameApi::EveryApi &ev, Fetcher<int> *fetch, std::vector<GameApi::BM> vec, int x, int y) : ev(ev), fetch(fetch), vec(vec),x(x),y(y) 
  {
    firsttime = true;
    sh.id = -1;
  }
  void first_time_calc()
  {
    std::vector<GameApi::VA> va_vec;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	va_vec.push_back(ev.sprite_api.create_vertex_array(vec[i]));
      }
    vas = va_vec;
    sh.id = -1;
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() {
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      first_time_calc();
      firsttime = false;
    }

    sh.id = e.sh_texture;
    fetch->frame(e);
    int idx = fetch->get();
    int s = vas.size();
    if (idx>=0 && idx<s) {
      if (g_chooser_flag)
	dynchar_selection = idx;
      ev.shader_api.use(sh);
      GameApi::M m = add_matrix2(ev.get_env(),e.in_MV);
      GameApi::M m2 = ev.matrix_api.trans(x,y,0);
      GameApi::M mm = ev.matrix_api.mult(m,m2);
      ev.shader_api.set_var(sh, "in_MV", mm);
      Matrix mk = find_matrix(ev.get_env(),mm);
      if (g_chooser_flag)
	dynchar_position = mk;
      ev.sprite_api.render_sprite_vertex_array(vas[idx]);
    }
  }
  virtual void handle_event(MainLoopEvent &e) { fetch->event(e); }
  virtual std::vector<int> shader_id() { if (sh.id>=0) return std::vector<int>{sh.id}; /*std::cout << "DynChar rejected sh.id=" << sh.id << std::endl;*/ return std::vector<int>(); }

private:
  GameApi::EveryApi &ev;
  Fetcher<int> *fetch;
  std::vector<GameApi::BM> vec;
  std::vector<GameApi::VA> vas;
  int x,y;
  GameApi::SH sh;
  bool firsttime;
};
#if 0
class DynChar2 : public MainLoopItem
{
public:
  DynChar(GameApi::EveryApi &ev, GameApi::FI font, Fetcher<int> *fetch, std::vector<GameApi::GI> vec, std::string alternatives, int x, int y) : ev(ev), font(font), fetch(fetch), vec(vec),alternatives(alternatives), x(x),y(y) 
  {
    firsttime = true;
  }

  void first_time_calc()
  {
    std::vector<GameApi::VA> va_vec;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	std::string s;
	s+=alternatives[i];
	BM bm = ev.font_api.draw_text_string(font, s, 0, 0);
	va_vec.push_back(ev.sprite_api.create_vertex_array(bm));
      }
    vas = va_vec;
    sh.id = -1;
  }


  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      first_time_calc();
      firsttime = false;
    }

    sh.id = e.sh_texture;
    int idx = fetch->get();
    int s = vas.size();
    if (idx>=0 && idx<s) {
      ev.shader_api.use(sh);
      ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(x,y,0));
      ev.sprite_api.render_sprite_vertex_array(vas[idx]);
    }
  }

  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { if (sh.id>=0) return std::vector<int>{sh.id}; std::cout << "DynChar2 rejected" << std::endl; return std::vector<int>(); }
  //virtual int shader_id() { return sh.id; }

private:
  GameApi::EveryApi &ev;
  FI font;
  Fetcher<int> *fetch;
  std::vector<GameApi::GI> vec;
  std::string alternatives;
  std::vector<GameApi::VA> vas;
  int x,y;
  GameApi::SH sh;
  bool firsttime;
};
EXPORT GameApi::ML GameApi::FontApi::dynamic_character2(GameApi::EveryApi &ev, std::vector<GameApi::GI> vec, std::string alternatives, IF fetcher, int x, int y)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  return add_main_loop(e, new DynChar2(ev, fetch, vec,alternatives,x,y));
}
#endif

EXPORT GameApi::ML GameApi::FontApi::dynamic_character(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, GameApi::IF fetcher, int x, int y)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  return add_main_loop(e, new DynChar(ev, fetch, vec,x,y));
}
EXPORT GameApi::FML GameApi::FontApi::dynamic_character_frame(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, GameApi::IF fetcher, int x, int y, int fmt, MN mn)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  Movement *move = find_move(e, mn);
  return add_framemainloop(e, new DynChar_fb(e,ev,fetch, vec, x,y, fmt, move)); 
}

EXPORT GameApi::ML GameApi::FontApi::dynamic_polygon(GameApi::EveryApi &ev, std::vector<GameApi::P> vec, GameApi::MT material, GameApi::IF fetcher)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  return add_main_loop(e, new DynPoly(e,ev, fetch, vec, material));  
}
EXPORT GameApi::ML GameApi::FontApi::dynamic_string(GameApi::EveryApi &ev, GameApi::Ft font, std::string alternative_chars, GameApi::SF fetcher, int x, int y, int numchars)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);


  int s = alternative_chars.size();
  std::vector<BM> chars;
  int xmax = 0;
  int ymax = 0;
  for(int i=0;i<s;i++)
    {
      BM bm = glyph(font, (long)alternative_chars[i]);
      int x = ev.bitmap_api.size_x(bm);
      int y = ev.bitmap_api.size_y(bm);
      if (x>xmax) xmax = x;
      if (y>ymax) ymax = y;
    }




  int s2 = alternative_chars.size();
  //std::vector<BM> chars;
  for(int i=0;i<s2;i++)
    {
      char ch = alternative_chars[i];
      Bitmap<int> *bm2 = env->fonts[font.id].bm;
      FontGlyphBitmap *bm3 = static_cast<FontGlyphBitmap*>(bm2);
      int top = bm3->bitmap_top(ch);


      BM bm = glyph(font, (long)alternative_chars[i]);
      int xx = ev.bitmap_api.size_x(bm);
      //int yy = ev.bitmap_api.size_y(bm);
      BM bm4 = ev.bitmap_api.growbitmap(bm, (xmax-xx)/2, ymax-top, (xmax-xx)/2, 0);
      chars.push_back(bm4);
    }

  IF char_fetch;
  std::vector<ML> ml;
  for(int i=0;i<numchars;i++)
    {
      char_fetch = char_fetcher_from_string(fetcher, alternative_chars, i);
      ml.push_back(dynamic_character(ev, chars, char_fetch, x+xmax*i,y));
    }
  return ev.mainloop_api.array_ml(ev,ml);
}

extern Point g_key_move_2_pos;

class KeyMoveAreaFetcher : public Fetcher<int>
{
public:
  KeyMoveAreaFetcher(float start_x, float end_x, float start_z, float end_z) : start_x(start_x), end_x(end_x), start_z(start_z), end_z(end_z) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void set(int t) { }
  virtual int get() const
  {
    Point pos = g_key_move_2_pos;
    if (pos.x>=start_x && pos.x<=end_x &&
	pos.z>=start_z && pos.z<=end_z)
      return 1;
    return 0;
  }
private:
  float start_x, end_x;
  float start_z, end_z;
};

GameApi::IF GameApi::FontApi::key_move_2_area_fetcher(float start_x, float end_x, float start_z, float end_z)
{
  return add_int_fetcher(e, new KeyMoveAreaFetcher(start_x,end_x,start_z,end_z));
}

class MovementIntFetcher : public Fetcher<int>
{
public:
  MovementIntFetcher(int count, float x_mult, float y_mult, float z_mult) : count(count), x_mult(x_mult), y_mult(y_mult), z_mult(z_mult) { val=0; }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) {
    Matrix m = e.in_MV;
    float xx = m.matrix[3];
    float yy = m.matrix[7];
    float zz = m.matrix[11];
    xx*=x_mult;
    yy*=y_mult;
    zz*=z_mult;
    float sum = xx+yy+zz;
    val = int(sum)%count;
    if (val<0) val=-val;
  }
  virtual void set(int t) { }
  virtual int get() const
  {
    return val;
  }

private:
  int val;
  int count;
  float x_mult, y_mult, z_mult;
};

class SendKey : public MainLoopItem
{
public:
  SendKey(MainLoopItem *item, Fetcher<int> *fetcher, std::string keys) : item(item), fetcher(fetcher), keys(keys) {}
  virtual void Collect(CollectVisitor &vis) {
    item->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { item->Prepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    fetcher->frame(e);
    int val = fetcher->get();
    static int old =-1;
    
    if (old!=val && val>=0 && val<keys.size())
      {
	old=val;
	MainLoopEvent keyevent;
	keyevent.type = 0x300;
	keyevent.ch = keys[val];
	keyevent.button=-1;
	keyevent.cursor_pos=Point(0.0,0.0,0.0);
	item->handle_event(keyevent);
      }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    fetcher->event(e);
    item->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return item->shader_id(); }
  
private:
  MainLoopItem *item;
  Fetcher<int> *fetcher;
  std::string keys;
};

GameApi::ML GameApi::MainLoopApi::if_keys(ML ml, IF fetcher, std::string keys)
{
  MainLoopItem *item = find_main_loop(e,ml);
  Fetcher<int> *fetcher2 = find_int_fetcher(e,fetcher);
  return add_main_loop(e, new SendKey(item,fetcher2,keys));
}

class SpanKeyFetcher : public Fetcher<float>
{
public:
  SpanKeyFetcher(float start_x, float end_x, float speed_x, int key_down, int key_up) : start_x(start_x), end_x(end_x), speed_x(speed_x), key_down(key_down), key_up(key_up)
  {
    pos_x = start_x+(end_x-start_x)/2;
    down=false;
    up=false;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e)
  {
    //std::cout << e.type << " " << e.ch << std::endl;
    if (e.type==0x300 && e.ch == key_down) down=true;
    if (e.type==0x300 && e.ch == key_up) up=true;
    if (e.type==0x301 && e.ch == key_down) down=false;
    if (e.type==0x301 && e.ch == key_up) up=false;
    //std::cout << "up:" << up << " down:" << down << std::endl;
  }
  virtual void frame(MainLoopEnv &e) {
    //std::cout << "up2:" << up << " down2:" << down << std::endl;
    if (down) pos_x-=speed_x;
    if (up) pos_x+=speed_x;
    //std::cout << "pos_x1:" << pos_x << std::endl;
    if (pos_x>end_x) pos_x=end_x;
    if (pos_x<start_x) pos_x=start_x;
    //std::cout << "pos_x2:" << pos_x << std::endl;
  }
  virtual void set(float t) { pos_x=t; }
  virtual float get() const
  {
    return pos_x;
  }
private:
  float start_x, end_x, speed_x;
  int key_down, key_up;
  float pos_x;
  bool up,down;
};

GameApi::FF GameApi::FontApi::span_key_fetcher(float start_x, float end_x, float speed_x, int key_down, int key_up)
{
  return add_float_fetcher(e,new SpanKeyFetcher(start_x, end_x, speed_x, key_down, key_up));
}

class ToggleButtonIntFetcher : public Fetcher<int>
{
public:
  ToggleButtonIntFetcher(float start_x, float end_x, float start_y, float end_y) : start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { toggle=false; }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e)
  {
    if (e.cursor_pos.x > start_x && e.cursor_pos.x < end_x)
      if (e.cursor_pos.y > start_y && e.cursor_pos.y < end_y)
	{
	  if (e.type==1025 && e.button ==0) { toggle=!toggle; }
	}
  }
  virtual void frame(MainLoopEnv &e) { }
  virtual void set(int t) { toggle=t==1; }
  virtual int get() const
  {
    return toggle ? 1 : 0;
  }

private:
  float start_x, end_x;
  float start_y, end_y;
  bool toggle;
};

extern int quake_pos_x;
extern int quake_pos_y;
class QuakeAreaFetcher : public Fetcher<int>
{
public:
  QuakeAreaFetcher(float start_x, float end_x, float start_z, float end_z) : start_x(start_x), end_x(end_x), start_z(start_z), end_z(end_z) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void set(int t) { }
  virtual int get() const {
    if (-quake_pos_x>=start_x && -quake_pos_x<end_x)
      if(-quake_pos_y+400.0>=start_z && -quake_pos_y+400.0<end_z)
	return 1;
    return 0;
  }

private:
  float start_x, end_x;
  float start_z, end_z;
};
GameApi::IF GameApi::FontApi::quake_area_fetcher(float start_x, float end_x, float start_z, float end_z)
{
  return add_int_fetcher(e,new QuakeAreaFetcher(start_x, end_x, start_z,end_z));
}


class RepeatIntFetcher : public Fetcher<int>
{
public:
  RepeatIntFetcher(Fetcher<int> *fetch, float duration) : fetch(fetch), duration(duration) { }
  virtual void draw_event(FrameLoopEvent &e) { 
    fetch->draw_event(e);
  }
  virtual void draw_frame(DrawLoopEnv &e) { 
    DrawLoopEnv ee = e;
    ee.time = fmod(e.time, duration/10.0);
    fetch->draw_frame(ee);
  }

  virtual void event(MainLoopEvent &e) {
    fetch->event(e);
  }
  virtual void frame(MainLoopEnv &e) {
    MainLoopEnv ee = e;
    ee.time = fmod(e.time, duration/10.0);
    fetch->frame(ee);
  }
  void set(int i) { fetch->set(i); }
  int get() const { return fetch->get(); }
private:
  Fetcher<int> *fetch;
  float duration;
};

class KeyPressIntFetcher : public Fetcher<int>
{
public:
  KeyPressIntFetcher(int key, int key_down_value, int key_up_value) : key(key), key_down_value(key_down_value), key_up_value(key_up_value) {
    key_down = false;
  }
  virtual void draw_event(FrameLoopEvent &e) { 
    int ch = e.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (e.type==0x300 && ch==key) {
      key_down = true;
    }
    if (e.type==0x301 && ch==key) {
      key_down = false;
    }

  }
  virtual void draw_frame(DrawLoopEnv &e) { 

  }

  virtual void event(MainLoopEvent &e) {
    int ch = e.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (e.type==0x300 && ch==key) {
      key_down = true;
    }
    if (e.type==0x301 && ch==key) {
      key_down = false;
    }
  }
  virtual void frame(MainLoopEnv &e) { }
  void set(int i) { }
  int get() const {
    if (key_down) { return key_down_value; }
    else { return key_up_value; }
  }
  
private:
  int key;
  int key_down_value, key_up_value;
  bool key_down;
};


class FaceFetcher : public Fetcher<FaceID>
{
public:
  FaceFetcher(FaceCollection *coll, int facenum) : coll(coll), facenum(facenum)
  {
    firsttime = true;
    u=0.0;
    v=0.0;
  }
  virtual void event(MainLoopEvent &e) {
    Point pos = e.cursor_pos;
    if (facenum>=0 && facenum<coll->NumFaces()) {
      Point p1 = coll->FacePoint(facenum,0);
      Point p2 = coll->FacePoint(facenum,1);
      Point p3 = coll->FacePoint(facenum,2);
      Point p4 = coll->FacePoint(facenum,3);
      
      Matrix m = in_P * in_T * in_MV;
      
      p1 = p1 * m;
      p2 = p2 * m;
      p3 = p3 * m;
      p4 = p4 * m;
      Vector2d d_1 = { pos.x - p1.x, pos.y - p1.y };
      Vector2d d_2 = { pos.x - p2.x, pos.y - p2.y };
      //Vector2d d_3 = { pos.x - p3.x, pos.y - p3.y };
      Vector2d d_4 = { pos.x - p4.x, pos.y - p4.y };

      Vector2d d12 = { p2.x-p1.x, p2.y-p1.y };
      Vector2d d14 = { p4.x-p1.x, p4.y-p1.y };
      Vector2d d43 = { p4.x-p3.x, p4.y-p3.y };
      Vector2d d23 = { p2.x-p3.x, p2.y-p3.y };

      float dot_12p = Vector2d::DotProduct( d12, d_1 );
      float dot_14p = Vector2d::DotProduct( d14, d_1 );
      float dot_43p = Vector2d::DotProduct( d43, d_4 );
      float dot_23p = Vector2d::DotProduct( d23, d_2 );
      dot_12p /= d12.Dist()*d12.Dist();
      dot_14p /= d14.Dist()*d14.Dist();
      dot_43p /= d43.Dist()*d43.Dist();
      dot_23p /= d23.Dist()*d23.Dist();

      float pos_x = (1.0-dot_14p)*dot_12p + dot_14p*dot_43p;
      float pos_y = (1.0-dot_12p)*dot_14p + dot_12p*dot_23p;

      v = (1.0-pos_x)*dot_14p + pos_x*dot_23p;
      u = (1.0-pos_y)*dot_12p + pos_y*dot_43p;
      std::cout << "UV: " << u << " " << v << std::endl;
    }
    
  }
  virtual void frame(MainLoopEnv &e) { 
    if (firsttime) {
      coll->Prepare();
      firsttime = false;
    }
    in_MV = e.in_MV;
    in_T = e.in_T;
    in_P = e.in_P;
  }
  virtual void draw_event(FrameLoopEvent &e) {
  }
  virtual void draw_frame(DrawLoopEnv &e) { 
  }

  void set(FaceID t) { }
  FaceID get() const {
    FaceID f;
    f.u = u;
    f.v = v;
    f.facenum = facenum;
    return f;
  }
private:
  FaceCollection *coll;
  Matrix in_MV;
  Matrix in_T;
  Matrix in_P;
  bool firsttime;
  int facenum;
  float u,v;
  bool hit;
};
GameApi::UV GameApi::MainLoopApi::face_fetcher(P p, int facenum)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_uv(e, new FaceFetcher(coll, facenum));
}



class ChooseFloatFetcher : public Fetcher<float>
{
public:
  ChooseFloatFetcher(Fetcher<int> *f,
		     float a_1, float a_2, float a_3, float a_4, float a_5, float a_6, float a_7) : f(f), a_1(a_1), a_2(a_2), a_3(a_3), a_4(a_4), a_5(a_5), a_6(a_6), a_7(a_7) { }
  virtual void event(MainLoopEvent &e) { f->event(e); }
  virtual void frame(MainLoopEnv &e) { f->frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { f->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { f->draw_frame(e); }

  void set(float t) { }
  float get() const {
    int val = f->get();
    if (val>=0 && val<=6) {
      float arr[] = { a_1, a_2, a_3, a_4, a_5, a_6, a_7 };
      return arr[val];
    } else { return 0.0; }
  }
private:
  Fetcher<int> *f;
  float a_1,a_2,a_3,a_4,a_5,a_6,a_7;
};

GameApi::FF GameApi::FontApi::choose_float_fetcher(IF int_fetcher, float a_1, float a_2, float a_3, float a_4, float a_5, float a_6, float a_7)
{
  Fetcher<int> *iif = find_int_fetcher(e, int_fetcher);
  return add_float_fetcher(e, new ChooseFloatFetcher(iif, a_1, a_2, a_3,a_4,a_5,a_6,a_7));
}


class FPSFetcher : public Fetcher<float>
{
public:
  FPSFetcher(GameApi::EveryApi &ev) :ev(ev) { counter=0; fps=0.0; }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { 
    counter++;
    fps=ev.mainloop_api.fpscounter(false);
    if (counter>10) { counter=0; fps2=fps; }
  }

  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { 
    counter++;
    fps=ev.mainloop_api.fpscounter(false);
    if (counter>10) { counter=0; fps2=fps; }
  }

  void set(float t) { }
  float get() const {
    return fps2;
  }
private:
  GameApi::EveryApi &ev;
  float fps;
  float fps2;
  int counter;
};
GameApi::FF GameApi::FontApi::fps_fetcher(EveryApi &ev)
{
  return add_float_fetcher(e, new FPSFetcher(ev));
}

class TimeRangeFetcher : public Fetcher<float>
{
public:
  TimeRangeFetcher(float start_time, float end_time, float before_start, float start_value, float end_value, float after_end,float repeat) : start_time(start_time), end_time(end_time), before_start(before_start), start_value(start_value), end_value(end_value), after_end(after_end),repeat(repeat) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { } 
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { time = e.time*10.0; }
  void set(float t) { }
  float get() const {
    float t = time - start_time;
    if (t>=0)
      t = fmod(t,repeat);

    t/=(end_time-start_time);
    if (t<0.0f) t=before_start;
    else if (t>1.0f) t=after_end;
    else {
      t*=(end_value-start_value);
      t+=start_value;
    }
    return t;
  }
private:
  float time;
  float start_time, end_time;
  float before_start, start_value, end_value, after_end;
  float repeat;
};

class TimeRangeFetcherKey : public Fetcher<float>
{
public:
  TimeRangeFetcherKey(int key, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat) : key(key), start_time(start_time), end_time(end_time), before_start(before_start), start_value(start_value), end_value(end_value), after_end(after_end),repeat(repeat) {
    keyclick_time = 10000000.0;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) {
    if (e.type==768 && e.ch==key && !keydown) {
      keyclick_time = time;
      keydown = true;
    }
    if (e.type==769 && e.ch==key) {
      keyclick_time = 10000000.0;
      keydown = false;
    }
  }
  virtual void frame(MainLoopEnv &e) { time = e.time*10.0; }
  void set(float t) { }
  float get() const {
    //std::cout << "time=" << time << ", keyclick_time=" << keyclick_time << ", start_time=" << start_time << std::endl;
    float t = time - keyclick_time - start_time;
    if (t>=0)
      t=fmod(t,repeat);
    t/=(end_time-start_time);
    if (t<0.0f) t=before_start;
    else if (t>1.0f) t=after_end;
    else {
      t*=(end_value-start_value);
      t+=start_value;
    }
    return t;
  }
private:
  int key;
  float time;
  float keyclick_time;
  float start_time, end_time;
  float before_start, start_value, end_value, after_end;
  float repeat;
  bool keydown = false;
};

class TimeRangeFetcherState : public Fetcher<float>
{
public:
  TimeRangeFetcherState(int state, Fetcher<int> *fetcher, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat) : state(state), fetcher(fetcher), start_time(start_time), end_time(end_time), before_start(before_start), start_value(start_value), end_value(end_value), after_end(after_end),repeat(repeat) {
    keyclick_time = 10000000.0;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) {
    fetcher->event(e);
    int val = fetcher->get();
    if (val==state && !keydown) {
      keyclick_time = time;
      keydown = true;
    }
    if (val!=state) {
      keyclick_time = 10000000.0;
      keydown = false;
    }
  }
  virtual void frame(MainLoopEnv &e) {
    fetcher->frame(e); 
   time = e.time*10.0;
  }
  void set(float t) { }
  float get() const {
    //std::cout << "time=" << time << ", keyclick_time=" << keyclick_time << ", start_time=" << start_time << std::endl;
    float t = time - keyclick_time - start_time;
    if (t>=0)
      t = fmod(t,repeat);
    t/=(end_time-start_time);
    if (t<0.0f) t=before_start;
    else if (t>1.0f) t=after_end;
    else {
      t*=(end_value-start_value);
      t+=start_value;
    }
    return t;
  }
private:
  int state;
  Fetcher<int> *fetcher;
  float time;
  float keyclick_time;
  float start_time, end_time;
  float before_start, start_value, end_value, after_end;
  float repeat;
  bool keydown = false;
};

class TimeRangeFetcherStateKey : public Fetcher<float>
{
public:
  TimeRangeFetcherStateKey(int state, int key, Fetcher<int> *fetcher, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat) : state(state), key(key), fetcher(fetcher), start_time(start_time), end_time(end_time), before_start(before_start), start_value(start_value), end_value(end_value), after_end(after_end),repeat(repeat) {
    keyclick_time = 10000000.0;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) {
    fetcher->event(e);
    int val = fetcher->get();
    if (val==state && !keydown && e.type==768 && e.ch==key) {
      keyclick_time = time;
      keydown = true;
    }

    if (e.type==769 && e.ch==key) {
      keyclick_time = 10000000.0;
      keydown = false;
    }
    
    if (val!=state) {
      keyclick_time = 10000000.0;
      keydown = false;
    }
    

    
  }
  virtual void frame(MainLoopEnv &e) {
    fetcher->frame(e);
    time = e.time*10.0;
  }
  void set(float t) { }
  float get() const {
    //std::cout << "time=" << time << ", keyclick_time=" << keyclick_time << ", start_time=" << start_time << std::endl;
    float t = time - keyclick_time - start_time;
    if (t>=0)
      t = fmod(t,repeat);
    t/=(end_time-start_time);
    if (t<0.0f) t=before_start;
    else if (t>1.0f) t=after_end;
    else {
      t*=(end_value-start_value);
      t+=start_value;
    }
    return t;
  }
private:
  int state;
  int key;
  Fetcher<int> *fetcher;
  float time;
  float keyclick_time;
  float start_time, end_time;
  float before_start, start_value, end_value, after_end;
  float repeat;
  bool keydown = false;
};



  
GameApi::FF GameApi::FontApi::time_range_fetcher(float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat)
{
  return add_float_fetcher(e, new TimeRangeFetcher(start_time, end_time, before_start, start_value, end_value, after_end,repeat));
}
  GameApi::FF GameApi::FontApi::time_range_fetcher_key(int key, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat)
  {
    return add_float_fetcher(e, new TimeRangeFetcherKey(key, start_time, end_time, before_start, start_value, end_value, after_end,repeat));
  }
GameApi::FF GameApi::FontApi::time_range_fetcher_state(int state, IF if_state, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat)
{
  Fetcher<int> *fetcher = find_int_fetcher(e, if_state);
  return add_float_fetcher(e, new TimeRangeFetcherState(state, fetcher, start_time, end_time, before_start, start_value, end_value, after_end,repeat));
}
GameApi::FF GameApi::FontApi::time_range_fetcher_state_key(int state, int key, IF if_state, float start_time, float end_time, float before_start, float start_value, float end_value, float after_end, float repeat)
{
  Fetcher<int> *fetcher = find_int_fetcher(e, if_state);
  return add_float_fetcher(e, new TimeRangeFetcherStateKey(state,key,fetcher,start_time,end_time,before_start,start_value, end_value, after_end,repeat));
}
class TimeFetcher2 : public Fetcher<float>
{
public:
  TimeFetcher2(GameApi::EveryApi &ev) : ev(ev) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { 
    time = e.time;
  }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { time = e.time; }
  void set(float t) { }
  float get() const {
    return time;
  }
private:
  GameApi::EveryApi &ev;
  float time;
};
GameApi::FF GameApi::FontApi::time_fetcher2(EveryApi &ev)
{
  return add_float_fetcher(e, new TimeFetcher2(ev));
}
extern int score;

class ScoreFetcher : public Fetcher<int>
{
public:
  ScoreFetcher(GameApi::EveryApi &ev) :ev(ev) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { 
  }

  void set(int t) { }
  int get() const {
    return score;
  }
private:
  GameApi::EveryApi &ev;
};

#if 0
class TimeFetcher : public Fetcher<int>
{
public:
  TimeFetcher(GameApi::EveryApi &ev, float start_time) :ev(ev), start_time(start_time) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e)
  {
  }
  virtual void frame(MainLoopEnv &e) { 
  }

  void set(int t) { }
  int get() const {
    float time = start_time/10.0 - ev.mainloop_api.get_time()/1000.0;
    return int(time);
  }
private:
  GameApi::EveryApi &ev;
  float start_time;
  float time;
};
#endif


GameApi::IF GameApi::FontApi::time_fetcher(EveryApi &ev, float start_time)
{
#if 0
  return add_int_fetcher(e, new TimeFetcher(ev,start_time));
#endif
  GameApi::IF i = { -1 };
  return i;
}

class ChooseScreen : public Fetcher<int>
{
public:
  ChooseScreen(float left_x, float right_x, int min_screen, int max_screen, Matrix &m)
    : left_x(left_x), right_x(right_x), min_screen(min_screen), max_screen(max_screen),m(m)
  {
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e)
  {
  }
  virtual void frame(MainLoopEnv &e) {
    //Matrix m = e.in_MV;
    float x_val = m.matrix[3];
    if (x_val<left_x) counter--;
    if (x_val>right_x) counter++;
    if (counter<min_screen) counter=min_screen;
    if (counter>max_screen) counter=max_screen;
  }

  void set(int t) { }
  int get() const {
    return counter;
  }

private:
  int counter=0;
  float left_x, right_x;
  int min_screen, max_screen;
  Matrix &m;
};

class MoveChooseScreen : public Movement
{
public:
  MoveChooseScreen(float left_x, float right_x, Matrix &m) : left_x(left_x), right_x(right_x),m(m) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) {

    Matrix m2 = e.in_MV*Matrix::Translate(delta,0.0,0.0);
    m=m2;
    float x_val = m2.matrix[3];
    if (x_val<left_x) delta+=(right_x-left_x);
    if (x_val>right_x) delta-=(right_x-left_x);
  }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void draw_event(FrameLoopEvent &e) { }

  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return Matrix::Translate(delta,0.0,0.0);
  }
private:
  float delta=0.0;
  float left_x;
  float right_x;
  Matrix &m;
};

GameApi::ARR GameApi::FontApi::choose_screen(float left_x, float right_x, int min_screen, int max_screen)
{
  Matrix *m = new Matrix;
  GameApi::IF i = add_int_fetcher(e, new ChooseScreen(left_x, right_x, min_screen, max_screen,*m));
  GameApi::MN i2 = add_move(e, new MoveChooseScreen(left_x, right_x,*m));

  ArrayType *array = new ArrayType;
  array->type=0;
  array->vec.push_back(i.id);
  array->vec.push_back(i2.id);
  return add_array(e,array);
}


GameApi::IF GameApi::FontApi::score_fetcher(EveryApi &ev)
{
  return add_int_fetcher(e, new ScoreFetcher(ev));
}
GameApi::IF GameApi::FontApi::toggle_button_fetcher(float start_x, float end_x, float start_y, float end_y)
{
  return add_int_fetcher(e, new ToggleButtonIntFetcher(start_x,end_x,start_y,end_y));
}
GameApi::IF GameApi::FontApi::movement_int_fetcher(int count, float x_mult, float y_mult, float z_mult)
{
  return add_int_fetcher(e, new MovementIntFetcher(count,x_mult,y_mult,z_mult));
}
class FloatToStringFetcher : public Fetcher<std::string>
{
public:
  FloatToStringFetcher(Fetcher<float> &ff) : ff(ff) { }
  virtual void draw_event(FrameLoopEvent &e) { ff.draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { ff.draw_frame(e); }

  virtual void event(MainLoopEvent &e) { ff.event(e); }
  virtual void frame(MainLoopEnv &e) { ff.frame(e); }

  void set(std::string t) { }
  std::string get() const {
    std::stringstream ss;
    float v = ff.get();
    int v2 = (int)v;
    if (v2<0) v2=0;
    if (v2>99999) v2=99999;
    ss << std::setfill('0') << std::setw(5) << v2;
    return ss.str();
  }
private:
  Fetcher<float> &ff;
};
GameApi::SF GameApi::FontApi::float_to_string_fetcher(FF fetcher)
{
  Fetcher<float> *ff = find_float_fetcher(e, fetcher);
  return add_string_fetcher(e,new FloatToStringFetcher(*ff));
}

class IntToStringFetcher : public Fetcher<std::string>
{
public:
  IntToStringFetcher(Fetcher<int> &ff) : ff(ff) { }
  virtual void draw_event(FrameLoopEvent &e) { ff.draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { ff.draw_frame(e); }

  virtual void event(MainLoopEvent &e) { ff.event(e); }
  virtual void frame(MainLoopEnv &e) { ff.frame(e); }

  void set(std::string t) { }
  std::string get() const {
    std::stringstream ss;
    int v = ff.get();
    int v2 = (int)v;
    if (v2<0) v2=0;
    if (v2>99999) v2=99999; 
    ss << std::setfill('0') << std::setw(5) << v2;
    return ss.str();
  }
private:
  Fetcher<int> &ff;
};

GameApi::SF GameApi::FontApi::int_to_string_fetcher(IF fetcher)
{
  Fetcher<int> *ff = find_int_fetcher(e, fetcher);
  return add_string_fetcher(e,new IntToStringFetcher(*ff));
}

class PointFetcherPart : public Fetcher<Point>
{
public:
  PointFetcherPart(Fetcher<Point> *pos, int component, Fetcher<float> *comp) : pos(pos), component(component), comp(comp) { }
  virtual void draw_event(FrameLoopEvent &e) { pos->draw_event(e); comp->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { pos->draw_frame(e); comp->draw_frame(e); }

  virtual void event(MainLoopEvent &e) { pos->event(e); comp->event(e); }
  virtual void frame(MainLoopEnv &e) { pos->frame(e); comp->frame(e); }
  virtual void set(Point t) { pos->set(t); }
  virtual Point get() const {
    Point p = pos->get();
    if (component==0) p.x = comp->get();
    if (component==1) p.y = comp->get();
    if (component==2) p.z = comp->get();
    return p;
  }

private:
  Fetcher<Point> *pos;
  int component;
  Fetcher<float> *comp;
};

template<class T>
class ConstantFetcher : public Fetcher<T>
{
public:
  ConstantFetcher(T t) : m_t(t) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void set(T t) { m_t = t; }
  virtual T get() const { return m_t; }
private:
  T m_t;
};

class MouseFetcher : public Fetcher<Point>
{
public:
  MouseFetcher() { }
  virtual void draw_event(FrameLoopEvent &e) { m_t = e.cursor_pos; }
  virtual void draw_frame(DrawLoopEnv &e) { }

  virtual void event(MainLoopEvent &e) { m_t = e.cursor_pos; }
  virtual void frame(MainLoopEnv &e) { }
  virtual void set(Point t) { m_t = t; }
  virtual Point get() const { return m_t; }
private:
  Point m_t;
};
GameApi::PF GameApi::FontApi::mouse_fetcher()
{
  return add_point_fetcher(e, new MouseFetcher);
}
class XComp : public Fetcher<int>
{
public:
  XComp(Fetcher<Point> *pf, float start_x, float end_x, int num_steps) : pf(pf), start_x(start_x), end_x(end_x), num_steps(num_steps) { }
  virtual void draw_event(FrameLoopEvent &e) { pf->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { pf->draw_frame(e); }
  virtual void event(MainLoopEvent &e) { pf->event(e); }
  virtual void frame(MainLoopEnv &e) { pf->frame(e); }
  virtual void set(int t) { }
  virtual int get() const
  {
    Point val2 = pf->get();
    float val = val2.x;
    val -= start_x;
    val /= (end_x-start_x);
    val *= num_steps;
    return int(val);
  }
private:
  Fetcher<Point> *pf;
  float start_x, end_x;
  int num_steps;
};
class YComp : public Fetcher<int>
{
public:
  YComp(Fetcher<Point> *pf, float start_y, float end_y, int num_steps) : pf(pf), start_y(start_y), end_y(end_y), num_steps(num_steps) { }
  virtual void draw_event(FrameLoopEvent &e) { pf->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { pf->draw_frame(e); }
  virtual void event(MainLoopEvent &e) { pf->event(e); }
  virtual void frame(MainLoopEnv &e) { pf->frame(e); }
  virtual void set(int t) { }
  virtual int get() const
  {
    Point val2 = pf->get();
    float val = val2.y;
    val -= start_y;
    val /= (end_y-start_y);
    val *= num_steps;
    return int(val);
  }
private:
  Fetcher<Point> *pf;
  float start_y, end_y;
  int num_steps;
};
class ZComp : public Fetcher<int>
{
public:
  ZComp(Fetcher<Point> *pf, float start_z, float end_z, int num_steps) : pf(pf), start_z(start_z), end_z(end_z), num_steps(num_steps) { }
  virtual void draw_event(FrameLoopEvent &e) { pf->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { pf->draw_frame(e); }
  virtual void event(MainLoopEvent &e) { pf->event(e); }
  virtual void frame(MainLoopEnv &e) { pf->frame(e); }
  virtual void set(int t) { }
  virtual int get() const
  {
    Point val2 = pf->get();
    float val = val2.z;
    val -= start_z;
    val /= (end_z-start_z);
    val *= num_steps;
    return int(val);
  }
private:
  Fetcher<Point> *pf;
  float start_z, end_z;
  int num_steps;
};
GameApi::IF GameApi::FontApi::x_comp(PF point_fetcher, float start_x, float end_x, int num_steps)
{
  Fetcher<Point> *pf = find_point_fetcher(e, point_fetcher);
  return add_int_fetcher(e, new XComp(pf, start_x, end_x, num_steps));
}
GameApi::IF GameApi::FontApi::y_comp(PF point_fetcher, float start_y, float end_y, int num_steps)
{
  Fetcher<Point> *pf = find_point_fetcher(e, point_fetcher);
  return add_int_fetcher(e, new YComp(pf, start_y, end_y, num_steps));
}
GameApi::IF GameApi::FontApi::z_comp(PF point_fetcher, float start_z, float end_z, int num_steps)
{
  Fetcher<Point> *pf = find_point_fetcher(e, point_fetcher);
  return add_int_fetcher(e, new ZComp(pf, start_z, end_z, num_steps));
}
GameApi::PF GameApi::FontApi::point_fetcher_constant(float x, float y, float z)
{
  return add_point_fetcher(e, new ConstantFetcher<Point>(Point(x,y,z)));
}
GameApi::PF GameApi::FontApi::point_fetcher_part(PF point_fetcher, int component, FF float_fetcher)
{
  Fetcher<Point> *pf = find_point_fetcher(e, point_fetcher);
  Fetcher<float> *ff = find_float_fetcher(e, float_fetcher);
  return add_point_fetcher(e, new PointFetcherPart(pf, component, ff));
}

class TimedIntFetcher : public Fetcher<int>
{
public:
  TimedIntFetcher(GameApi::EveryApi &ev, int start, int end, float start_time, float duration) : ev(ev), start(start), end(end), start_time(start_time), duration(duration)
  {
    firsttime = true;
    time_origin = 0.0;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) {
    if (firsttime)
      time_origin = 0.0; //e.time*10.0;
    firsttime = false;
    
    current_time = e.time*10.0;
    current_time -= time_origin;

  }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) {
    if (firsttime)
      time_origin = 0.0; //e.time*10.0;
    firsttime = false;
    
    current_time = e.time*10.0;
    current_time -= time_origin;
  }

  void set(int i) { }
  int get() const {
    firsttime = false;
    float val = current_time;
    if (val<start_time) return start;
    if (val>=start_time+(end-start)*duration) return end;
    float d = (val-start_time)/duration;
    int di = start + (int)d;
    return di;
  }
private:
  GameApi::EveryApi &ev;
  int start;
  int end;
  float start_time;
  float duration;
  mutable float time_origin;
  mutable bool firsttime;
  mutable float current_time;
};

class ChooseCharFetcher : public Fetcher<int>
{
public:
  ChooseCharFetcher(Fetcher<std::string> *fetch, std::string alternatives, int index) : fetch(fetch), alternatives(alternatives), index(index) { }
  virtual void event(MainLoopEvent &e) { fetch->event(e); }
  virtual void frame(MainLoopEnv &e) { fetch->frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { fetch->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { fetch->draw_frame(e); }

  void set(int i) { }
  int get() const
  {
    std::string val = fetch->get();
    int s = val.size();
    if (index>=0 && index < s) {
      char c = val[index];
      int ss = alternatives.size();
      for(int i=0;i<ss;i++)
	{
	  if (alternatives[i]==c) { return i; }
	}
      return 0;
    } else {
      return 0;
    }
  }
private:
  Fetcher<std::string> *fetch;
  std::string alternatives;
  int index;
};
EXPORT GameApi::IF GameApi::FontApi::timed_int_fetcher(EveryApi &ev, int start, int end, float start_time, float end_time)
{
  return add_int_fetcher(e, new TimedIntFetcher(ev, start, end, start_time, (end_time-start_time)/(end-start)));
}
EXPORT GameApi::IF GameApi::FontApi::repeat_int_fetcher(IF fetcher, float duration)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  return add_int_fetcher(e, new RepeatIntFetcher(fetch, duration));
}
EXPORT GameApi::IF GameApi::FontApi::keypress_int_fetcher(int key, int key_down_value, int key_up_value)
{
  return add_int_fetcher(e, new KeyPressIntFetcher(key, key_down_value, key_up_value));
}
EXPORT GameApi::IF GameApi::FontApi::char_fetcher_from_string(SF string_fetcher, std::string alternatives, int idx)
{
  Fetcher<std::string> *str = find_string_fetcher(e, string_fetcher);
  return add_int_fetcher(e, new ChooseCharFetcher(str, alternatives, idx));
}
class MLChooser : public MainLoopItem
{
public:
  MLChooser(std::vector<MainLoopItem*> vec, Fetcher<int> &f) : vec(vec),f(f) { firsttime = true; }
  void Collect(CollectVisitor &vis) {
    int s = vec.size();
    for(int i=0;i<s;i++) { if (vec[i]) vec[i]->Collect(vis); }
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) { if (vec[i]) vec[i]->Prepare(); }
  }
  virtual void execute(MainLoopEnv &e)
  {
    //std::cout << "ML Chooser execute" << std::endl;
    if (!firsttime) {
      f.frame(e);
      int val = f.get();
      int s = vec.size();
      if (val>=0 && val<s && vec[val]) vec[val]->execute(e);
    } else {
      int s = vec.size();
      for(int i=0;i<s;i++) vec[i]->execute(e);
      firsttime=false;
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    f.event(e);
    int val = f.get();
    int s = vec.size();
    if (val>=0 && val<s && vec[val]) vec[val]->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    int val = f.get();
    int s = vec.size();
    if (val>=0 && val<s && vec[val]) return vec[val]->shader_id();
    return std::vector<int>();
  }
private:
  std::vector<MainLoopItem*> vec;
  Fetcher<int> &f;
  bool firsttime;
};
EXPORT GameApi::ML GameApi::FontApi::ml_chooser(std::vector<ML> vec, IF fetcher)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    vec2.push_back(find_main_loop(e, vec[i]));
  Fetcher<int> *f = find_int_fetcher(e, fetcher);
  return add_main_loop(e, new MLChooser(vec2, *f));
}

std::map<std::string, int> number_map;

class ScoreStringFetcher : public Fetcher<std::string>
{
public:
  ScoreStringFetcher(std::string id, std::string label, int numdigits) : id(id), label(label), numdigits(numdigits) { }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }

  void set(std::string s) { }
  std::string get() const {
    std::string s = label;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(numdigits) << number_map[id];
    s+=ss.str();
    return s;
  }
private:
  std::string id;
  std::string label;
  int numdigits;
};
GameApi::SF GameApi::FontApi::score_string_fetcher(std::string id, std::string label, int numdigits)
{
  return add_string_fetcher(e, new ScoreStringFetcher(id, label, numdigits));
}

class TimeStringFetcher : public Fetcher<std::string>
{
public:
  TimeStringFetcher(GameApi::EveryApi &ev) { 
    firsttime = true;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }

  void set(std::string s) { }
  std::string get() const {
    if (firsttime)
      start_time = g_low->sdl->SDL_GetTicks();
    firsttime = false;
    unsigned int time = g_low->sdl->SDL_GetTicks();
    unsigned int time_to_use = time - start_time;
    time_to_use /= 1000;

    unsigned int beg = time_to_use % 60;
    unsigned int end = (time_to_use / 60) % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << end;
    ss << ":";
    ss << std::setfill('0') << std::setw(2) << beg;
    return ss.str();
  }
private:
  mutable unsigned int start_time;
  mutable bool firsttime;
};

EXPORT GameApi::SF GameApi::FontApi::time_string_fetcher(GameApi::EveryApi &ev)
{
  return add_string_fetcher(e, new TimeStringFetcher(ev));
}

EXPORT GameApi::BM GameApi::FontApi::font_string(Ft font, std::string str, int x_gap)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int sz = str.size();
  FontCharacterString<Color> *array = new FontCharacterString<Color>(Color(1.0f,1.0f,1.0f,0.0f), x_gap);
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      BM bm = glyph(font, ch);
      Bitmap<int> *bm2 = env->fonts[font.id].bm;
      FontGlyphBitmap *bm3 = static_cast<FontGlyphBitmap*>(bm2);
      int top = bm3->bitmap_top(ch);
      BitmapHandle *handle = find_bitmap(e,bm);
      Bitmap<Color> *col = find_color_bitmap(handle);
      array->push_back(col, top);
    }
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = array;
  return add_bitmap(e,chandle2);
}
EXPORT std::vector<GameApi::BM> GameApi::FontApi::bm_array_id_inv(ARR arr)
{
  ArrayType *t = find_array(e, arr);
  std::vector<GameApi::BM> vec;
  int s = t->vec.size();
  for(int i=0;i<s;i++)
    {
      GameApi::BM bm;
      bm.id = t->vec[i];
      vec.push_back(bm);
    }
  return vec;
}
EXPORT GameApi::ARR GameApi::FontApi::bm_array_id(std::vector<BM> vec)
{
  std::vector<int> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(vec[i].id);
    }
  ArrayType *arr = new ArrayType;
  arr->type = 0;
  arr->vec = vec2;
  return add_array(e, arr);
}

class LineSegmentGlyph : public Glyph
{
public:
  LineSegmentGlyph(Glyph &next, bool left_side, float s_x, float e_x, float s_y, float e_y) : next(next), left_side(left_side), s_x(s_x), e_x(e_x), s_y(s_y), e_y(e_y) { }
  virtual float Top() const { return next.Top(); }
  virtual float SizeX() const { return next.SizeX(); }
  virtual float SizeY() const { return next.SizeY(); }
  virtual float Baseline() const { return next.Baseline(); }
  virtual int NumSplits(float y) const
  {
    if (y>=s_y && y<=e_y) return 1+next.NumSplits(y);
    return next.NumSplits(y);
  }
  virtual float Split(float y, int idx) const
  {
    if (y>=s_y && y<=e_y) {
      if (idx==0) {
	float yy = (y-s_y)/(e_y-s_y);
	float xx = (1.0-yy)*s_x + yy*e_x;
	return xx;
      } else { return next.Split(y,idx-1); }
    } else { return next.Split(y,idx); }
  }
  virtual int SplitType(float y, int idx) const
  {
    if (y>=s_y && y<=e_y) {
      if (idx==0) return left_side?1:2;
      return next.SplitType(y,idx-1);
    }
    return next.SplitType(y,idx);
  }
private:
  Glyph &next;
  bool left_side;
  float s_x, e_x;
  float s_y, e_y;
};
class CircleGlyph : public Glyph
{
public:
  CircleGlyph(Glyph &next, bool filled, float center_x, float center_y, float radius) : next(next), filled(filled), center_x(center_x), center_y(center_y), radius(radius) { }
  float Top() const { return next.Top(); }
  float SizeX() const { return next.SizeX(); }
  float SizeY() const { return next.SizeY(); }
  float Baseline() const { return next.Baseline(); }
  int NumSplits(float y) const
  {
    float yy = y-center_y;
    if (fabs(yy)<radius) { return 4+next.NumSplits(y); }
    return 2+next.NumSplits(y);
  }
  float Split(float y, int idx) const
  {
    float yy = y-center_y;
    if (fabs(yy)<radius) {
      if (idx==0||idx==1||idx==2||idx==3) {
	float h = yy;
	float x = sqrt(radius*radius-h*h);
	if (idx==0) return 0.0;
	if (idx==1) return center_x-x;
	if (idx==2) return center_x+x;
	if (idx==3) return SizeX();
      } else { return next.Split(y,idx-4); }
    } else { 
      if (idx==0) return 0.0;
      if (idx==1) return SizeX();
      return next.Split(y,idx-2); 
    }
  }
  int SplitType(float y, int idx) const
  {
    float yy = y-center_y;
    if (fabs(yy)<radius) {
      if (idx==0) { return filled?2:1; }
      if (idx==1) { return filled?1:2; }
      if (idx==2) { return filled?2:1; }
      if (idx==3) { return filled?1:2; }
      return next.SplitType(y,idx-4);
    } else {
      if (idx==0) { return filled?2:1; }
      if (idx==1) { return filled?1:2; }
      return next.SplitType(y,idx-2);
    }
  }
private:
  Glyph &next;
  bool filled;
  float center_x, center_y;
  float radius;
};
class ScalingXGlyph : public Glyph
{
public:
  ScalingXGlyph(Glyph &next, float start_x, float end_x,
	       float target_x, float target_end_x)

    : next(next), start_x(start_x), end_x(end_x),
      target_x(target_x), target_end_x(target_end_x)
  {
  }
  float Top() const { return next.Top(); }
  float SizeX() const { return next.SizeX(); }
  float SizeY() const { return next.SizeY(); }
  float Baseline() const { return next.Baseline(); }
  int NumSplits(float y) const
  {
    return next.NumSplits(y);
  }
  float Split(float y, int idx) const
  {
    float x = next.Split(y,idx);
    x-=start_x;
    x/=end_x-start_x;
    x*=target_end_x-target_x;
    x+=target_x;
    return x;
  }
private:
  Glyph &next;
  float start_x, end_x;
  float target_x, target_end_x;
};


class ScalingYGlyph : public Glyph
{
public:
  ScalingYGlyph(Glyph &next, float start_y, float end_y,
	       float target_y, float target_end_y)

    : next(next), start_y(start_y), end_y(end_y),
      target_y(target_y), target_end_y(target_end_y)
  {
  }
  float Top() const { return next.Top(); }
  float SizeX() const { return next.SizeX(); }
  float SizeY() const { return next.SizeY(); }
  float Baseline() const { return next.Baseline(); }
  int NumSplits(float y) const
  {
    y-=target_y;
    y/=target_end_y-target_y;
    y*=end_y-start_y;
    y+=start_y;
    return next.NumSplits(y);
  }
  float Split(float y, int idx) const
  {
    y-=target_y;
    y/=target_end_y-target_y;
    y*=end_y-start_y;
    y+=start_y;
    float x = next.Split(y,idx);
    return x;
  }
private:
  Glyph &next;
  float start_y, end_y;
  float target_y, target_end_y;
};

class ClippingRectGlyph : public Glyph
{
public:
  ClippingRectGlyph(Glyph &next, float start_x, float end_x, float start_y, float end_y) : next(next), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
  float Top() const { return next.Top(); }
  float SizeX() const { return next.SizeX(); }
  float SizeY() const { return next.SizeY(); }
  float Baseline() const { return next.Baseline(); }
  int NumSplits(float y) const
  {
    if (y<start_y) return 0;
    if (y>end_y) return 0;
    int s = next.NumSplits(y);
    int count = 0;
    for(int i=0;i<s;i++) {
      float x = next.Split(y,i);
      if (x>=start_x && x<=end_x) count++;
    }
    return count;
  }
  float Split(float y, int idx) const
  {
    if (y<start_y) return 0.0;
    if (y>end_y) return 0.0;
    int s = next.NumSplits(y);
    int count = -1;
    for(int i=0;i<s;i++)
      {
	float x = next.Split(y,i);
	if (x>=start_x && x<=end_x) count++;
	if (idx==count) return x;
      }
    return 0.0;
  }
private:
  Glyph &next;
  float start_x, end_x;
  float start_y, end_y;
};

IMPORT extern Low_SDL_Window *sdl_window;
extern int g_resize_event_sx;
extern int g_resize_event_sy;

#ifdef EMSCRIPTEN
#define KP EMSCRIPTEN_KEEPALIVE
#else
#define KP
#endif


KP extern "C" void set_resize_event(int sx, int sy);

bool g_has_fullscreen_button=false;

#ifdef EMSCRIPTEN

EM_JS(void, call_fullscreen, (), {
    var el = document.getElementById("canvas");
    el.requestFullscreen();
  });
EM_JS(void, call_exit_fullscreen, (), {
    document.exitFullscreen();
  });

#endif

class FullscreenButton : public MainLoopItem
{
public:
  FullscreenButton(GameApi::Env &env, GameApi::EveryApi &ev) : env(env), ev(ev) { g_has_fullscreen_button=true; ml.id=-1; }
  ~FullscreenButton() { g_has_fullscreen_button=false; }
  
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {
    GameApi::BB I1=ev.bool_bitmap_api.bb_empty(15,15);
    GameApi::BB I2=ev.bool_bitmap_api.rectangle(I1,0,0,5,2);
    GameApi::BB I3=ev.bool_bitmap_api.rectangle(I2,0,0,2,15);
    GameApi::BB I4=ev.bool_bitmap_api.rectangle(I3,0,13,5,2);
    GameApi::BM I5=ev.bool_bitmap_api.to_bitmap(I4,255,255,255,255,0,0,0,0);
    GameApi::BM I6=ev.bitmap_api.flip_x(I5);
    GameApi::BM I7=ev.bitmap_api.blitbitmap(I5,I6,0,0);
    GameApi::BM I8=ev.bitmap_api.scale_to_size(I7,50);
    GameApi::ML I9=ev.sprite_api.vertex_array_render(ev,I8);
    GameApi::MN I10=ev.move_api.mn_empty();
    GameApi::MN I11=ev.move_api.trans2(I10,1140,844,0);
    GameApi::ML I12=ev.move_api.move_ml(ev,I9,I11,1,10.0);
    GameApi::ML I13=ev.sprite_api.turn_to_2d(ev,I12,0.0,0.0,1200.0,900.0);
    GameApi::ML I14=ev.mainloop_api.disable_z_buffer(I13);
    ml = I14;
    
    MainLoopItem *item = find_main_loop(env,ml);
    item->Prepare();
  }
  virtual void Prepare() {
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
      if (ml.id!=-1) {
	MainLoopItem *item = find_main_loop(env,ml);
	item->execute(e);
      }
  }
  virtual void handle_event(MainLoopEvent &e)
  {

    //std::cout << e.button << " " << e.cursor_pos.x << " " << e.cursor_pos.y << " " << e.type << " " << e.ch << std::endl;

    bool b=false;
#ifdef EMSCRIPTEN
    if (current_state==true) {
      b = e.button==0 && e.cursor_pos.x>=675 && e.cursor_pos.x<=675+50 &&
	e.cursor_pos.y>=550 && e.cursor_pos.y<=550+50;
    } else {
      b = e.button==0 && e.cursor_pos.x>=750 && e.cursor_pos.x<=750+50 &&
	e.cursor_pos.y>=550 && e.cursor_pos.y<=550+50;
    }
#else
    b = e.button==0 && e.cursor_pos.x>=1140 && e.cursor_pos.x<=1140+50 &&
      e.cursor_pos.y>=844 && e.cursor_pos.y<=844+50;
#endif
      if (b)
      {
	req_state=!req_state;
      }
    if (e.type==0x300 && e.ch==1073741892) { req_state=true; }
    if (e.type==0x300 && e.ch==27) { req_state=false; }
#ifdef EMSCRIPTEN
    if (req_state!=current_state) {
      if (req_state==true) {
	old_sx = g_resize_event_sx;
	old_sy = g_resize_event_sy;
	//emscripten_request_fullscreen("canvas", false);
	//g_low->sdl->SDL_SetWindowFullscreen(sdl_window, Low_SDL_WINDOW_FULLSCREEN);
	call_fullscreen();
	int w=800;
	int h=600;
	//g_low->sdl->SDL_GetWindowSize(sdl_window, &w,&h);
	//std::cout << "RESIZING TO:" << w << "x" << h << std::endl; 
	set_resize_event(w,h);
	current_state=true;
      } else
	{
	  //emscripten_exit_fullscreen();
	  //g_low->sdl->SDL_SetWindowFullscreen(sdl_window, 0);
	  call_exit_fullscreen();
	  set_resize_event(old_sx,old_sy);
	  current_state=false;
	}
      

    }
				   
#endif
  }
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      return item->shader_id();
    } else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::ML ml;
  bool current_state=false;
  bool req_state=false;
  int old_sx, old_sy;
};

GameApi::ML GameApi::MainLoopApi::fullscreen_button(EveryApi &ev)
{
  return add_main_loop(e, new FullscreenButton(e,ev));
}
