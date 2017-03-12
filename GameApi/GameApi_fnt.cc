
#include "GameApi_h.hh"
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
#ifndef EMSCRIPTEN
  Font fnt; 
  std::cout << &env->lib << std::endl;
  fnt.bm = new FontGlyphBitmap((void*)&env->lib,filename.c_str(), sx,sy);
  env->fonts.push_back(fnt); 
#else
  Font fnt;
  fnt.bm = new ConstantBitmap<int>(128,sx,sy);
  env->fonts.push_back(fnt);
#endif 
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
EXPORT GameApi::BM GameApi::FontApi::glyph(GameApi::Ft font, long idx)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<int> *bmA = env->fonts[font.id].bm;
  FontGlyphBitmap *bm3 = dynamic_cast<FontGlyphBitmap*>(bmA);
  bm3->load_glyph(idx);
  Bitmap<int> *bm = env->fonts[font.id].bm;
  Bitmap<Color> *cbm = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(255,255,255,0), *bm);
  MemoizeBitmap *mbm = new MemoizeBitmap(*cbm);
  mbm->MemoizeAll();
  env->deletes.push_back(std::shared_ptr<void>(cbm)); 
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = mbm;
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
       
      //Bitmap<int> *bm2 = env->fonts[font.id].bm;
      //FontGlyphBitmap *bm3 = dynamic_cast<FontGlyphBitmap*>(bm2);
      //int top = bm3->bitmap_top(ch);
     //BitmapHandle *handle = find_bitmap(e,bm);
      //Bitmap<Color> *col = find_color_bitmap(handle);
      
    }
  ArrayType *val = new ArrayType;
  val->type = E_BM;
  val->vec = bms;
  return add_array(e, val);
}
class DynChar : public MainLoopItem
{
public:
  DynChar(GameApi::EveryApi &ev, Fetcher<int> *fetch, std::vector<GameApi::BM> vec, int x, int y) : ev(ev), fetch(fetch), vec(vec),x(x),y(y) 
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
  virtual void execute(MainLoopEnv &e)
  {
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
  Fetcher<int> *fetch;
  std::vector<GameApi::BM> vec;
  std::vector<GameApi::VA> vas;
  int x,y;
  GameApi::SH sh;
};

EXPORT GameApi::ML GameApi::FontApi::dynamic_character(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, GameApi::IF fetcher, int x, int y)
{
  Fetcher<int> *fetch = find_int_fetcher(e, fetcher);
  return add_main_loop(e, new DynChar(ev, fetch, vec,x,y));
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
  return ev.mainloop_api.array_ml(ml);
}

class ChooseCharFetcher : public Fetcher<int>
{
public:
  ChooseCharFetcher(Fetcher<std::string> *fetch, std::string alternatives, int index) : fetch(fetch), alternatives(alternatives), index(index) { }
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
EXPORT GameApi::IF GameApi::FontApi::char_fetcher_from_string(SF string_fetcher, std::string alternatives, int idx)
{
  Fetcher<std::string> *str = find_string_fetcher(e, string_fetcher);
  return add_int_fetcher(e, new ChooseCharFetcher(str, alternatives, idx));
}

class TimeStringFetcher : public Fetcher<std::string>
{
public:
  TimeStringFetcher(GameApi::EveryApi &ev) { 
    start_time = SDL_GetTicks();
  }
  void set(std::string s) { }
  std::string get() const {
    unsigned int time = SDL_GetTicks();
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
  unsigned int start_time;
};

EXPORT GameApi::SF GameApi::FontApi::time_string_fetcher(GameApi::EveryApi &ev)
{
  return add_string_fetcher(e, new TimeStringFetcher(ev));
}

EXPORT GameApi::BM GameApi::FontApi::font_string(Ft font, std::string str, int x_gap)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int sz = str.size();
  FontCharacterString<Color> *array = new FontCharacterString<Color>(Color(0.0f,0.0f,0.0f,0.0f), x_gap);
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
