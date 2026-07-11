
#include "GameApi_h.hh"
#ifdef HAS_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
//#include <freetype/ftoutln.h>
#include FT_OUTLINE_H
#include "FreeType2.hh"



struct GlyphData
{
  int left=0;
  int top=0;
  int sx=0,sy=0;
  int advance_x=0;
  int ascender=0;
  int descender=0;
  int height=0;
  unsigned int *bitmap_data=0;

  FT_Library *lib=0;
  FT_Face face;
};

std::string glyph_key(std::string ttf_filename, int sx, int sy)
{
  std::stringstream ss;
  ss << ttf_filename << "@" << sx << "," << sy;
  return ss.str();
}


FontInterfaceImpl::FontInterfaceImpl(GameApi::Env &e, void *priv_, std::string ttf_filename, std::string homepage, int sx, int sy) : e(e), ttf_filename(ttf_filename), homepage(homepage), sx(sx), sy(sy), priv_(priv_), mutex(PTHREAD_MUTEX_INITIALIZER)
{ 
  priv = 0;
  key = glyph_key(ttf_filename, sx,sy);
}

std::map<std::string,std::map<long,GlyphData*>*> global_glyph_data;

std::vector<typename FontInterfaceImpl::Bezier2d> FontInterfaceImpl::outlines(char idx) const
{
  return const_cast<FontInterfaceImpl*>(this)->gen_outline_data((long)idx);
}

Point2d lerp2(Point2d a, Point2d b, float t)
{
  return { a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t };
}

void split(std::vector<FontInterfaceImpl::Bezier2d> &vec, FontInterfaceImpl::Bezier2d orig, float t, bool emit_left, bool emit_right)
{
  Point2d A = lerp2(orig.p1,orig.p2,t);
  Point2d B = lerp2(orig.p2,orig.p3,t);
  Point2d C = lerp2(A,B,t);
  if (emit_left) {
    FontInterfaceImpl::Bezier2d bez;
    bez.p1 = orig.p1;
    bez.p2 = A;
    bez.p3 = C;
    vec.push_back(bez);
  }
  if (emit_right) {
    FontInterfaceImpl::Bezier2d bez;
    bez.p1 = C;
    bez.p2 = B;
    bez.p3 = orig.p3;
    vec.push_back(bez);
  }
}


std::vector<FontInterfaceImpl::Bezier2d> FontInterfaceImpl::make_monotonic(std::vector<FontInterfaceImpl::Bezier2d> vec)
{
  std::vector<FontInterfaceImpl::Bezier2d> res;
  
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      Bezier2d curr = vec[i];

      //std::cout << "BEZ:" << curr.p1 << " " << curr.p2 << " " << curr.p3 << std::endl;
      
      

      float tx = (curr.p1.x - curr.p2.x) / (curr.p1.x - 2.0*curr.p2.x + curr.p3.x);
      bool accept_tx = tx>=-0.0000000001f && tx<=1.0000000001f;
      float ty = (curr.p1.y - curr.p2.y) / (curr.p1.y - 2.0*curr.p2.y + curr.p3.y);
      bool accept_ty = ty>=-0.0000000001f && tx<=1.0000000001f;

      std::vector<float> splits;
      if (accept_tx) splits.push_back(tx);
      if (accept_ty) splits.push_back(ty);
      std::sort(splits.begin(),splits.end());
      if (fabs(tx-ty)<0.000000001) splits.resize(1);
      
      Bezier2d current = curr;
      float previous = 0.0;

      int s = splits.size();
      for(int i=0;i<s;i++)
	{
	  float local = (splits[i]-previous)/(1.0-previous);
	  split(res,current,local,true,false);
	  std::vector<Bezier2d> right;
	  split(right,current,local,false,true);
	  current = right[0];
	  previous=splits[i];
	}
      res.push_back(current);
    }
  return res;
}

std::vector<FontInterfaceImpl::OutlineEvent> FontInterfaceImpl::convert_to_events(std::vector<FontInterfaceImpl::Bezier2d> vec)
{
  int s = vec.size();
  std::vector<FontInterfaceImpl::OutlineEvent> events;
  for(int i=0;i<s;i++)
    {
      OutlineEvent e;
      e.ymin = std::min(vec[i].p1.y,vec[i].p3.y);
      e.ymax = std::max(vec[i].p1.y,vec[i].p3.y);
      e.curve = i;
      //std::cout << "Event:" << e.ymin << " " << e.ymax << " " << e.curve << std::endl;
      if (!std::isnan(e.ymin) && !std::isnan(e.ymax))
	events.push_back(e);
    }
  std::sort(events.begin(),events.end(),[](FontInterfaceImpl::OutlineEvent e1,FontInterfaceImpl::OutlineEvent e2)
  {
    return e1.ymin < e2.ymin;
  });
  return events;
}

FontInterfaceImpl::ScanLineContext FontInterfaceImpl::ScanLineContextCreate() const
{
  FontInterfaceImpl::ScanLineContext ctx;
  ctx.curr = 0;
  return ctx;
}

void FontInterfaceImpl::ScanLineStep(std::vector<int> &active, const std::vector<OutlineEvent> &events, float y, FontInterfaceImpl::ScanLineContext &ctx, long idx) const
{
  y+=Top(idx)/64.0f; /*-5.0f*/;
  if (ctx.curr>=events.size()) return;
  OutlineEvent e = events[ctx.curr];
  while(e.ymin <= y) {
    if (ctx.curr>=events.size()) break;
    //std::cout << "ADDED:" << ctx.curr << std::endl;
    active.push_back(ctx.curr);
    e = events[ctx.curr];
    ctx.curr++;
  }
  int s = active.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "ACTIVE:" << i << "::" << active[i] << " " << y << " \in " << events[active[i]].ymin << ".." << events[active[i]].ymax << std::endl;
      int a = active[i];
      if (a>=0 && a<events.size()) {
      if (events[a].ymax <= y) {
	//std::cout << "ERASED!" << std::endl;
	active.erase(active.begin()+i);
	i--;
	s--;
      }
      } else {
	active.erase(active.begin()+i);
	i--;
	s--;
      }
    }
}

std::vector<FontInterfaceImpl::ResPair> FontInterfaceImpl::ScanLineXCoord(const std::vector<int> &active, const std::vector<OutlineEvent> &events, const std::vector<Bezier2d> &bezi, float y, long idx) const
{
  y+=Top(idx)/64.0f; /*-5.0f*/;
  
  std::vector<ResPair> res;
  int s = active.size();
  for(int i=0;i<s;i++)
    {
      int a = active[i];
      if (a<0||a>=events.size()) continue;
      const OutlineEvent &e = events[a];
      int curve = e.curve;
      //std::cout << "CURVE:" << curve << std::endl;
      if (curve<0 || curve>=bezi.size()) continue;
      const Bezier2d &bez = bezi[curve];
      float A = bez.p1.y-2.0*bez.p2.y+bez.p3.y;
      float B = 2.0*(bez.p2.y-bez.p1.y);
      float C = bez.p1.y - y;
      if (std::isnan(A)||std::isnan(B)||std::isnan(C)) continue;
      //std::cout << "ABC:" << A << " " << B << " " << C << std::endl;
      float D = B*B-4.0f*A*C;
      if (D<0.0f && D >-1e-12) D=0.0f;
      float x_1 = (-B + sqrt(D))/2.0/A;
      float x_2 = (-B - sqrt(D))/2.0/A;
      //std::cout << "X_n:" << x_1 << " " << x_2 << std::endl;
      ResPair p;
      float t;
      if (A==0.0f &&B==0.0f) continue;
      if (A==0.0f) t=-C/B; else
      if (x_1>=-0.0000000001f && x_1<=1.0000000001f) 
	t = x_1;
      else
	t = x_2;

      if (t<=-0.0000000001f || t>=1.0000000001f) continue;


      
      //std::cout << "t=" << t << std::endl;
      
      p.x = (1.0f-t)*(1.0f-t) * bez.p1.x + 2.0*(1.0-t)*t*bez.p2.x + t*t*bez.p3.x; 
      //float y2= (1.0f-t)*(1.0f-t) * bez.p1.y + 2.0*(1.0-t)*t*bez.p2.y + t*t*bez.p3.y; 

      //if (fabs(y2-y)>1e-5) continue;

      //std::cout << "p.x=" << p.x << std::endl;
      //if (fabs(bez.p3.y-bez.p1.y)<0.000001f) p.winding = 0.0f; else
      if (bez.p3.y>=bez.p1.y) p.winding = 1.0f;
      else p.winding=-1.0f;
      res.push_back(p);
    }
  std::sort(res.begin(),res.end(),[](ResPair p1, ResPair p2) { return p1.x<p2.x; });
  /*
  int s0 = res.size();
  for(int i=0;i<s0-1;i++)
    {
      float x0 = res[i].x;
      float x1 = res[i+1].x;
      if (fabs(x0-x1)<0.00001) {
	res[i].winding=res[i].winding + res[i+1].winding;
	res.erase(res.begin()+i+1);
	i--;
	s0--;
      }			 
    }
  */
  return res;
}
int FontInterfaceImpl::IsPixelInside(const std::vector<ResPair> &vec, float x, long idx) const
{
  x+=Left(idx);
  
  int w = 0;
  float previous = 0.0f;
  int s = vec.size();
  float coverage = 0.0f;
  for(int i=0;i<s;i++)
    {
      
      if (w!=0)
	{
	  if (x>=previous && x<vec[i].x) {
	    return 255;
	  }
	}
      w+=vec[i].winding;
      previous = vec[i].x;
    }
  return 0;
}

int FontInterfaceImpl::Ascender(long idx) const
{
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->ascender;
}
int FontInterfaceImpl::Descender(long idx) const
{
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->descender;
}
int FontInterfaceImpl::Height(long idx) const
{
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->height;
}

int FontInterfaceImpl::Left(long idx) const
{
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->left;
}

int FontInterfaceImpl::Top(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->top;
}
int FontInterfaceImpl::SizeX(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->sx;
}
int FontInterfaceImpl::SizeY(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->sy;
}
int FontInterfaceImpl::AdvanceX(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  return data2->operator[](idx)->advance_x;
}
unsigned int FontInterfaceImpl::Map(long idx, int x, int y) const
{
  if (x<0 || x>=SizeX(idx) || y<0 || y>=SizeY(idx))
    return 0;
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  int ssx = SizeX(idx);
  if (!data2) data2=global_glyph_data[key];
  if (!data2) return 0;
  if (!data2->operator[](idx)) return 0;
  unsigned int val = data2->operator[](idx)->bitmap_data[x+y*ssx]; 
  if (val==0xff000000) return 0;
  return val;
}

struct K { std::string filename; unsigned char *buffer; int size; };

std::vector<K> loaded_vec;

K find_loaded(std::string filename, bool &success) {
  int s = loaded_vec.size();
  for(int i=0;i<s;i++) {
    K k = loaded_vec[i];
    if (k.filename == filename) { success=true; return k; }
  }
  success=false;
  K k;
  k.filename="";
  k.buffer=0;
  k.size=0;
  return k;
}

//std::map<std::string, unsigned char *> loaded_maps;
//std::map<std::string, int> loaded_sizes;

struct OutlineContext
{
  std::vector<FontInterfaceImpl::Bezier2d> vec;
  Point2d current;
};

Point2d pt(const FT_Vector *v)
{
  Point2d p;
  p.x = v->x / 64.0;
  p.y = v->y / 64.0;
  return p;
}

int move_to(const FT_Vector *to, void *user)
{
  //std::cout << "move_to" << std::endl;
  OutlineContext *ctx = (OutlineContext*)user;
  ctx->current = pt(to);
  return 0;
}
int line_to(const FT_Vector *to, void *user)
{
  //std::cout << "line_to" << std::endl;
  OutlineContext *ctx = (OutlineContext*)user;
  Point2d p0 = ctx->current;
  Point2d p2 = pt(to);
  Point2d p1 = { (p0.x+p2.x)*0.5f + 0.01f,
	       (p0.y + p2.y)*0.5f + 0.01f };
  FontInterfaceImpl::Bezier2d bez;
  bez.p1=p0;
  bez.p2=p1;
  bez.p3=p2;
  ctx->vec.push_back(bez);
  ctx->current=p2;
  return 0;
}
int conic_to(const FT_Vector *control, const FT_Vector *to, void *user)
{
  OutlineContext *ctx = (OutlineContext*)user;
  //std::cout << "conic_to" << ctx->current << " " << pt(control) << " " << pt(to) << std::endl;
  ctx->vec.push_back({ ctx->current, pt(control), pt(to) });
  ctx->current = pt(to);
  return 0;
}
int cubic_to(const FT_Vector *control1, const FT_Vector *control2,
	     const FT_Vector *to, void *user)
{
  // std::cout << "cubic_to" << std::endl;
  OutlineContext *ctx = (OutlineContext*)user;
  Point2d p0 = ctx->current;
  Point2d c1 = pt(control1);
  Point2d c2 = pt(control2);
  Point2d p3 = pt(to);
  auto lerp = []( Point2d a, Point2d b) {
    return Point2d{ (a.x+b.x)*0.5f, (a.y+b.y)*0.5f };
  };
  Point2d p01 = lerp(p0,c1);
  Point2d p12 = lerp(c1,c2);
  Point2d p23 = lerp(c2,p3);

  Point2d p012 = lerp(p01,p12);
  Point2d p123 = lerp(p12,p23);

  Point2d mid = lerp(p012,p123);
  ctx->vec.push_back( { p0, p012, mid });
  ctx->vec.push_back( { mid, p123, p3 });
  ctx->current = p3;
  return 0;
}

bool g_is_outline=false;

std::vector<FontInterfaceImpl::Bezier2d> FontInterfaceImpl::gen_outline_data(long idx)
{
#ifdef THREADS
  static bool firsttime=true;
  if (firsttime)
    {
      pthread_mutex_init(&mutex,NULL);
      firsttime=false;
    }
  
  pthread_mutex_lock(&mutex);
#endif

  bool success;
  K k = find_loaded(ttf_filename, success);
  unsigned char *ptr2 = 0; //loaded_maps[ttf_filename];
  int size = 0; //loaded_sizes[ttf_filename];
  if (success) { ptr2 = k.buffer; size=k.size; }
  if (!ptr2) {

  GameApi::ASyncVec *ptr = e.get_loaded_async_url(ttf_filename);
  if (!ptr) {
    std::cout << "async not ready yet, failing..." << std::endl;
    //#ifndef EMSCRIPTEN
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif
    return std::vector<FontInterfaceImpl::Bezier2d>(); 
  } else {
  }


  ptr2 = new unsigned char[ptr->size()+1];
  std::copy(ptr->begin(), ptr->end(), ptr2);
  size = ptr->size();
  delete ptr;
  //loaded_maps[ttf_filename] = ptr2;
  //loaded_sizes[ttf_filename] = size;
  K k;
  k.filename = ttf_filename;
  k.buffer = ptr2;
  k.size = size;
  loaded_vec.push_back(k);
  } else {
    unsigned char *ptr3 = new unsigned char[size+1];
    std::copy(ptr2, ptr2+size, ptr3);
    ptr2 = ptr3;
  }
  GlyphData *data = new GlyphData;
  
  data->lib = (FT_Library*)priv_;
  //FT_Library_SetLcdFilter( *data->lib, FT_LCD_FILTER_DEFAULT );
  int err = FT_New_Memory_Face( *data->lib,
				ptr2 /*"font.ttf"*/,
			        size,
				0,
				&data->face);
  if (err!=0)
    {
    std::cout << "FT_New_Face ERROR: " << err << std::endl;
    //std::cout << "ptr2: " << std::hex << (int)ptr2 << " size:" << std::dec << size << std::endl;
    //std::cout << ptr2 << std::endl;
    std::cout << "Remember to recompile the code after changing envimpl size" << std::endl;
    //#ifndef EMSCRIPTEN
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif
    return std::vector<FontInterfaceImpl::Bezier2d>(); //exit(0);
    }
  FT_Set_Char_Size(data->face, sx*64,sy*64,96,96);

  if (idx<0||idx>0x80) idx=idx&0xff;
  FT_UInt glyphindex = FT_Get_Char_Index(data->face, idx);
  FT_Load_Glyph(data->face, glyphindex, FT_LOAD_NO_BITMAP|FT_LOAD_TARGET_LIGHT);

  FT_GlyphSlot slot = data->face->glyph;
  FT_Outline *outline = &slot->outline;

  FT_Outline_Funcs funcs;
  funcs.move_to = move_to;
  funcs.line_to = line_to;
  funcs.conic_to = conic_to;
  funcs.cubic_to = cubic_to;
  funcs.shift = 0;
  funcs.delta = 0;
  OutlineContext *ctx = new OutlineContext;
  FT_Outline_Decompose(outline, &funcs, (void*)ctx);
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif

  return ctx->vec;
}

void FontInterfaceImpl::gen_glyph_data(long idx)
{
  //std::cout << "try gen_glyph_data:" << idx << std::endl;
  //std::string key = glyph_key(ttf_filename,sx,sy);
  //#ifndef EMSCRIPTEN
  //#ifndef EMSCRIPTEN
#ifdef THREADS
  static bool firsttime=true;
  if (firsttime)
    {
      pthread_mutex_init(&mutex,NULL);
      firsttime=false;
    }
  
  pthread_mutex_lock(&mutex);
#endif
  //#endif

  std::map<long, GlyphData*> *mymap = data2;
  if (!mymap) { mymap = global_glyph_data[key]; }
  if (!mymap) {
    global_glyph_data[key] = new std::map<long,GlyphData*>();
    mymap = global_glyph_data[key];
    data2 = mymap;
  }
  GlyphData *data = mymap?mymap->operator[](idx):0; //glyph_data[idx];
  if (data) { 
    //#ifndef EMSCRIPTEN
    //#ifndef EMSCRIPTEN
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif
    //#endif
    return; }
  //std::cout << "gen_glyph_data:" << idx << std::endl;

  if (!data) {
    data = new GlyphData;
    global_glyph_data[key]->operator[](idx) = data;
  }

  bool success;
  K k = find_loaded(ttf_filename, success);

  unsigned char *ptr2 = 0; //loaded_maps[ttf_filename];
  int size = 0; //loaded_sizes[ttf_filename];
  if (success) { ptr2 = k.buffer; size=k.size; }
  if (!ptr2) {
    //std::cout << "loading font: " << ttf_filename << std::endl;
#ifndef EMSCRIPTEN
    //std::cout << "async: " << ttf_filename << " " << homepage << std::endl;
  e.async_load_url(ttf_filename, homepage);
#endif

  std::stringstream ss2;
  ss2 << "font" << idx << ".ttf";

  GameApi::ASyncVec *ptr = e.get_loaded_async_url(ttf_filename);
  if (!ptr) {
    std::cout << "async not ready yet, failing..." << std::endl;
    //#ifndef EMSCRIPTEN
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif
    return; 
  } else {
    //std::fstream ss(ss2.str().c_str(), std::ios_base::binary | std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.close();
  }

  ptr2 = new unsigned char[ptr->size()+1];
  std::copy(ptr->begin(), ptr->end(), ptr2);
  size = ptr->size();
  delete ptr;
  //loaded_maps[ttf_filename] = ptr2;
  //loaded_sizes[ttf_filename] = size;
  K k;
  k.filename = ttf_filename;
  k.buffer = ptr2;
  k.size = size;
  loaded_vec.push_back(k);

  } else {
    unsigned char *ptr3 = new unsigned char[size+1];
    std::copy(ptr2, ptr2+size, ptr3);
    ptr2 = ptr3;

  }
  data->lib = (FT_Library*)priv_;
  //FT_Library_SetLcdFilter( *data->lib, FT_LCD_FILTER_DEFAULT );
  int err = FT_New_Memory_Face( *data->lib,
				ptr2 /*"font.ttf"*/,
			        size,
				0,
				&data->face);
  if (err!=0)
    {
    std::cout << "FT_New_Face ERROR: " << err << std::endl;
    //std::cout << "ptr2: " << std::hex << (int)ptr2 << " size:" << std::dec << size << std::endl;
    //std::cout << ptr2 << std::endl;
    std::cout << "Remember to recompile the code after changing envimpl size" << std::endl;
    //#ifndef EMSCRIPTEN
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif
    return; //exit(0);
    }
  FT_Set_Char_Size(data->face, sx*64,sy*64,96,96);

  if (idx<0||idx>0x80) idx=idx&0xff;
  FT_UInt glyphindex = FT_Get_Char_Index(data->face, idx);
  if (g_is_outline)
    {
      FT_Load_Glyph(data->face, glyphindex, FT_LOAD_NO_BITMAP|FT_LOAD_TARGET_LIGHT);
      //FT_Render_Glyph(data->face->glyph, FT_RENDER_MODE_NORMAL);
    } else {
      FT_Load_Glyph(data->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_LIGHT);
      FT_Render_Glyph(data->face->glyph, FT_RENDER_MODE_NORMAL);
    }
  data->left = data->face->glyph->bitmap_left;
  data->top = -data->face->glyph->bitmap_top;
  data->sx = data->face->glyph->bitmap.width;
  data->sy = data->face->glyph->bitmap.rows;
  data->advance_x = data->face->glyph->advance.x >> 6;

  data->ascender = data->face->size->metrics.ascender >> 6;
  data->descender = data->face->size->metrics.descender >> 6;
  data->height = data->face->size->metrics.height >> 6;

  //std::cout << "Glyph:" << idx << " " << data->sx << " " << data->sy << " " << data->top << std::endl;
  if (!g_is_outline) {
  int ssx = data->sx;
  int ssy = data->sy;
  if (ssx<1) ssx=1;
  if (ssy<1) ssy=1;
  data->bitmap_data = new unsigned int[ssx*ssy];
  for(int iy=0;iy<data->sy;iy++)
    for(int ix=0;ix<data->sx;ix++)
      {
	unsigned char alpha = data->face->glyph->bitmap.buffer[ix+iy*data->face->glyph->bitmap.pitch];
	//std::cout << (int)alpha << ",";
	data->bitmap_data[ix+iy*data->sx] = (alpha<<24) | 0xffffff;
	//std::cout << (int) data->bitmap_data[ix+iy*data->sx] << ",";
      }
  } else {
    data->bitmap_data=0;
  }
  //std::cout << std::endl;

  //#ifndef EMSCRIPTEN
#ifdef THREADS
  pthread_mutex_unlock(&mutex);
#endif

}
#endif
