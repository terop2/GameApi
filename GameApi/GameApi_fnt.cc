
#include "GameApi_h.hh"
#include <iostream>
#include <iomanip>

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
  Font fnt; 
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
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bm = env->fonts[font.id].bm;
  FontGlyphBitmap *bm2 = dynamic_cast<FontGlyphBitmap*>(bm); 
  LineCollection *bm3 = dynamic_cast<LineCollection*>(bm);
  bm2->load_glyph_outline(idx, sx, sy);
  LineCollection *coll = bm3;
  LineCollection *coll2 = new ForwardLineCollection(coll);
  return add_line_array(e, coll2);
}
EXPORT GameApi::PL GameApi::FontApi::glyph_plane(GameApi::Ft font, long idx, float sx, float sy, float dx, float dy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bmA = env->fonts[font.id].bm;
  FontGlyphBitmap *bm2 = dynamic_cast<FontGlyphBitmap*>(bmA);
  bm2->load_glyph_outline(idx,sx,sy);
  LineCollection *coll = bm2;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)coll;
  PlanePoints2d *plane = new FontLineCollectionWrapper(coll, bm->Types(), sx, sy, dx,dy);
  return add_plane(e, plane);
}
class GlyphChooser : public Bitmap<int>
{
public:
  GlyphChooser(long idx, FontGlyphBitmap &bm) : idx(idx),bm(bm) { }
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
  FontGlyphBitmap *bm3 = dynamic_cast<FontGlyphBitmap*>(bmA);
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
      BM bm = glyph(font, ch);
      Bitmap<int> *bmA = env->fonts[font.id].bm;
      FontGlyphBitmap *bm2 = dynamic_cast<FontGlyphBitmap*>(bmA);
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
      BM bm = glyph(font, ch);
      bg = ev.bitmap_api.blitbitmap(bg, bm, p.second.x, p.second.y+p.second.top);
    }
  return bg;
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
      FontAtlasGlyphInfo ii = info->char_map[ch];
      BM bm = ev.bitmap_api.subbitmap(atlas_bm, ii.x, ii.y+ii.top, ii.sx,ii.sy);
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
  virtual int shader_id() { return -1; }
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
  void Prepare() {
      first_time_calc();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
    }

    sh.id = e.sh_texture;
    fetch->frame(e);
    int idx = fetch->get();
    int s = vas.size();
    if (idx>=0 && idx<s) {
      ev.shader_api.use(sh);
      GameApi::M m = add_matrix2(ev.get_env(),e.in_MV);
      GameApi::M m2 = ev.matrix_api.trans(x,y,0);
      GameApi::M mm = ev.matrix_api.mult(m,m2);
      ev.shader_api.set_var(sh, "in_MV", mm);
      ev.sprite_api.render_sprite_vertex_array(vas[idx]);
    }
  }
  virtual void handle_event(MainLoopEvent &e) { fetch->event(e); }
  virtual int shader_id() { return sh.id; }

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
  virtual int shader_id() { return sh.id; }

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
      FontGlyphBitmap *bm3 = dynamic_cast<FontGlyphBitmap*>(bm2);
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



GameApi::IF GameApi::FontApi::time_fetcher(EveryApi &ev, float start_time)
{
  return add_int_fetcher(e, new TimeFetcher(ev,start_time));
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
    if (v2>99) v2=99;
    ss << std::setfill('0') << std::setw(2) << v2;
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
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) { vec[i]->Prepare(); }
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (!firsttime) {
      f.frame(e);
      int val = f.get();
      int s = vec.size();
      if (val>=0 && val<s) vec[val]->execute(e);
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
    if (val>=0 && val<s) vec[val]->handle_event(e);
  }
  virtual int shader_id() {
    int val = f.get();
    int s = vec.size();
    if (val>=0 && val<s) return vec[val]->shader_id();
    return -1;
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
      FontGlyphBitmap *bm3 = dynamic_cast<FontGlyphBitmap*>(bm2);
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
