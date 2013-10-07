
#include "FreeType.hh"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <freetype/ftoutln.h>

struct GlyphPriv
{
  FT_Library *lib;
  FT_Face face;
  FT_UInt chosen_glyph;
};

FontGlyphBitmap::FontGlyphBitmap(void *priv_, std::string filename, int sx, int sy)
{
  priv = new GlyphPriv;
  priv->lib = (FT_Library*)priv_;
  int err = FT_New_Face( *priv->lib,
	       filename.c_str(),
	       0,
	       &priv->face);
  int err3 = FT_Set_Char_Size(priv->face, sx*64,sy*64,100,100);
  //FT_Size_RequestRec sizerequest;
  //sizerequest.type = FT_SIZE_REQUEST_TYPE_BBOX;
  //sizerequest.width = sx;
  //sizerequest.height = sy;
  //sizerequest.horiResolution = 0;
  //sizerequest.vertResolution = 0;
  //int err2 = FT_Request_Size( priv->face, &sizerequest );
  std::cout << "FontGlyphBitmap:" << std::hex << err << " " << err3 << std::endl;
}
FontGlyphBitmap::~FontGlyphBitmap()
{
  delete (GlyphPriv*)priv;
}
void FontGlyphBitmap::load_glyph(long idx)
{
  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  /*int err1 =*/ FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  /*int err2 =*/ FT_Render_Glyph(priv->face->glyph, FT_RENDER_MODE_NORMAL);
  //std::cout << "load_glyph: " << err1 << " " << err2 << std::endl;
}

int MoveToFunc(const FT_Vector *to, void *user)
{
  std::cout << "MoveToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  bm->move_point = p;
  return 0;
}
int LineToFunc(const FT_Vector *to, void *user)
{
  std::cout << "LineToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  bm->types.push_back(0);
  bm->types.push_back(0);
  bm->points.push_back(bm->move_point);
  bm->points.push_back(p);
  bm->move_point = p;
  Point2d p2 = { 0.0, 0.0 };
  bm->control1.push_back(p2);
  bm->control1.push_back(p2);
  bm->control2.push_back(p2);
  bm->control2.push_back(p2);
  return 0;
}
int ConicToFunc(const FT_Vector *control, const FT_Vector *to, void *user)
{
  std::cout << "ConicToFunc" << std::endl;

  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  bm->types.push_back(1);
  bm->types.push_back(1);
  bm->points.push_back(bm->move_point);
  bm->points.push_back(p);
  bm->move_point = p;
  Point2d p1 = { float(control->x)/65536.0f*bm->m_sx, float(control->y)/65536.0f*bm->m_sy };
  bm->control1.push_back(p1);
  Point2d p2 = { 0.0, 0.0 };
  bm->control2.push_back(p2);
  return 0;
}

int CubicToFunc(const FT_Vector *control1, const FT_Vector *control2,
		const FT_Vector *to,
		void *user)
{
  std::cout << "CubicToFunc" << std::endl;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)user;
  Point2d p = { float(to->x)/65536.0f*bm->m_sx, float(to->y)/65536.0f*bm->m_sy };
  bm->types.push_back(2);
  bm->types.push_back(2);
  bm->points.push_back(bm->move_point);
  bm->points.push_back(p);
  bm->move_point = p;
  Point2d p1 = { float(control1->x)/65536.0f*bm->m_sx, float(control1->y)/65536.0f*bm->m_sy };
  bm->control1.push_back(p1);
  Point2d p2 = { float(control2->x)/65536.0f*bm->m_sx, float(control2->y)/65536.0f*bm->m_sy };
  bm->control2.push_back(p2);
  return 0;
}

int FontGlyphBitmap::NumLines() const
{
  std::cout << "NumLines" << points.size() << std::endl;
  return points.size()/2;
}
Point FontGlyphBitmap::LinePoint(int line, int point) const
{
  Point p = Point(points[line*2+point].x, points[line*2+point].y, 0.0); 
  std::cout << line << " " << point << ":" << p.x << " " << p.y << std::endl;
  return p;
}

void FontGlyphBitmap::load_glyph_outline(long idx, float sx, float sy)
{
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
  
}

int FontGlyphBitmap::bitmap_top(long idx) const
{
  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  /*int err1 =*/ FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  return priv->face->glyph->bitmap_top;
}

int FontGlyphBitmap::SizeX() const
{
  int val = priv->face->glyph->bitmap.width;
  //std::cout << "SizeX:" << val << std::endl;
  return val;
}

int FontGlyphBitmap::SizeY() const
{
  int val = priv->face->glyph->bitmap.rows; 
  //std::cout << "SizeY:" << val << std::endl;
  return val;
}

int FontGlyphBitmap::Map(int x, int y) const
{
  return (int)priv->face->glyph->bitmap.buffer[x+y*priv->face->glyph->bitmap.pitch];
}
