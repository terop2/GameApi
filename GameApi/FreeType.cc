
#include "FreeType.hh"
#ifndef EMSCRIPTEN
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include <string>
//#include <freetype/ftoutln.h>
#ifndef EMSCRIPTEN
#include FT_OUTLINE_H
#endif
struct GlyphPriv
{
#ifndef EMSCRIPTEN
  FT_Library *lib;
  FT_Face face;
  FT_UInt chosen_glyph;
#endif
};
extern std::vector<unsigned char> load_from_url(std::string url);

FontGlyphBitmap::FontGlyphBitmap(void *priv_, std::string filename, int sx, int sy)
  : m_sx(0.0), m_sy(0.0)
{
  std::cout << "Loading font " << filename << std::endl;
  std::vector<unsigned char> buf = load_from_url(filename);
  std::fstream file("font.otf", std::ios_base::out|std::ios_base::binary);
  int s = buf.size();
  for(int i=0;i<s;i++)
    {
      file.put(buf[i]);
    }
  file.close();
  filename = "font.otf";


  priv = new GlyphPriv;
#ifndef EMSCRIPTEN
  priv->lib = (FT_Library*)priv_;
  int err = FT_New_Face( *priv->lib,
	       filename.c_str(), 
	       0,
	       &priv->face);
  //std::cout << "Font err: " << err << std::endl;
  if (err!=0)
    {
      std::cout << "ERROR: " << err << std::endl;
      std::cout << "Remember to recompile the code after changing envimpl size" << std::endl;
    }
  /*int err3 = */
 FT_Set_Char_Size(priv->face, sx*64,sy*64,100,100);
#endif
  //FT_Size_RequestRec sizerequest;
  //sizerequest.type = FT_SIZE_REQUEST_TYPE_BBOX;
  //sizerequest.width = sx;
  //sizerequest.height = sy;
  //sizerequest.horiResolution = 0;
  //sizerequest.vertResolution = 0;
  //int err2 = FT_Request_Size( priv->face, &sizerequest );
  //std::cout << "FontGlyphBitmap:" << std::hex << err << " " << err3 << std::endl;
}
FontGlyphBitmap::~FontGlyphBitmap()
{
  delete (GlyphPriv*)priv;
}
void FontGlyphBitmap::Prepare() { }
void FontGlyphBitmap::load_glyph(long idx)
{
#ifndef EMSCRIPTEN
  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  /*int err1 =*/ FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  /*int err2 =*/ FT_Render_Glyph(priv->face->glyph, FT_RENDER_MODE_NORMAL);
  //std::cout << "load_glyph: " << err1 << " " << err2 << std::endl;
#endif
}

#ifndef EMSCRIPTEN
int MoveToFunc(const FT_Vector *to, void *user)
{
  //std::cout << "MoveToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  bm->types.push_back(0);
  bm->points.push_back(p);
  bm->move_point = p;
  return 0;
}
int LineToFunc(const FT_Vector *to, void *user)
{
  //std::cout << "LineToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  //bm->types.push_back(0);
  bm->types.push_back(1);
  //bm->points.push_back(bm->move_point);
  bm->points.push_back(p);
  bm->move_point = p;
  Point2d p2 = { 0.0, 0.0 };
  bm->control1.push_back(p2);
  bm->control1.push_back(p2);
  bm->control2.push_back(p2);
  bm->control2.push_back(p2);
  return 0;
}
#endif
#if 0
class ConicPointCollection : public PointCollection
{
public:
  ConicPointCollection(FontGlyphBitmap *bm, int index, int type) : bm(bm), index(index), type(type) { }
  int Size() const {
    int count = 0;
    for(int i=index;i<bm->points.size();i+=2)
      {
	if (bm->types(i)!=type) return count; 
	count++;
      }
    return count; 
  }
  Point Index(int i) const 
  {
    int idx = i*2;
    Point2d p = points[idx];
    return Point(p.x,p.y,0.0);
  }
private:
  FontGlyphBitmap *bm;
  int index;
  int type;
};
#endif
#ifndef EMSCRIPTEN
int ConicToFunc(const FT_Vector *control, const FT_Vector *to, void *user)
{
  //std::cout << "ConicToFunc" << std::endl;

  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  Point2d p1 = { float(control->x)/65536.0f*bm->m_sx, float(control->y)/65536.0f*bm->m_sy };
  bm->types.push_back(2);
  bm->types.push_back(2);
  bm->types.push_back(2);
  //bm->types.push_back(1);
  //bm->types.push_back(1);
  //bm->points.push_back(bm->move_point);
  bm->points.push_back(p1);
  bm->points.push_back(p1);
  bm->points.push_back(p);
  bm->move_point = p;
  //bm->control1.push_back(p1);
  //Point2d p2 = { 0.0, 0.0 };
  //bm->control2.push_back(p2);
  //bm->points.push_back(p2);
  return 0;
}

int CubicToFunc(const FT_Vector *control1, const FT_Vector *control2,
		const FT_Vector *to,
		void *user)
{
  //std::cout << "CubicToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  //bm->types.push_back(0);
  bm->types.push_back(3);
  bm->types.push_back(3);
  bm->types.push_back(3);
  //bm->points.push_back(bm->move_point);
  Point2d p1 = { float(control1->x)/65536.0f*bm->m_sx, float(control1->y)/65536.0f*bm->m_sy };
  bm->points.push_back(p1);
  Point2d p2 = { float(control2->x)/65536.0f*bm->m_sx, float(control2->y)/65536.0f*bm->m_sy };
   bm->points.push_back(p2);
 bm->points.push_back(p);
  bm->move_point = p;
  return 0;
}
#endif
int FontGlyphBitmap::NumLines() const
{
  //std::cout << "NumLines" << points.size() << std::endl;
  return points.size()-1;
}
Point FontGlyphBitmap::LinePoint(int line, int point) const
{
  Point p = Point(points[line+point].x, points[line+point].y, 0.0); 
  //std::cout << line << " " << point << ":" << p.x << " " << p.y << std::endl;
  return p;
}

void FontGlyphBitmap::load_glyph_outline(long idx, float sx, float sy)
{
#ifndef EMSCRIPTEN

  m_sx = sx; m_sy = sy;
  types.clear();
  points.clear();
  control1.clear();
  control2.clear();
  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  /*int err1 =*/ FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);

  

  FT_Outline *outline = &priv->face->glyph->outline;
  FT_Outline_Funcs funcs;
  funcs.move_to = &MoveToFunc;
  funcs.line_to = &LineToFunc;
  funcs.conic_to = &ConicToFunc;
  funcs.cubic_to = &CubicToFunc;
  funcs.shift = 0;
  funcs.delta = 0L;
  FT_Outline_Decompose( outline, &funcs, (void*)this);
#endif  
}

int FontGlyphBitmap::bitmap_top(long idx) const
{
#ifndef EMSCRIPTEN

  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  /*int err1 =*/ FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  return priv->face->glyph->bitmap_top;
#endif
}

int FontGlyphBitmap::SizeX() const
{
#ifndef EMSCRIPTEN

  int val = priv->face->glyph->bitmap.width;
  //std::cout << "SizeX:" << val << std::endl;
  return val;
#endif
}

int FontGlyphBitmap::SizeY() const
{
#ifndef EMSCRIPTEN
  int val = priv->face->glyph->bitmap.rows; 
  //std::cout << "SizeY:" << val << std::endl;
  return val;
#endif
}

int FontGlyphBitmap::Map(int x, int y) const
{
#ifndef EMSCRIPTEN
  if (x<0 || x>=SizeX() || y<0 || y>=SizeY())
    return 0;
  return (int)priv->face->glyph->bitmap.buffer[x+y*priv->face->glyph->bitmap.pitch];
#endif
}
