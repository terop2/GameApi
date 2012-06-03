
#include "FreeType.hh"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>

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
  int err1 = FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  int err2 = FT_Render_Glyph(priv->face->glyph, FT_RENDER_MODE_NORMAL);
  std::cout << "load_glyph: " << err1 << " " << err2 << std::endl;
}

int FontGlyphBitmap::bitmap_top(long idx) const
{
  FT_UInt glyphindex = FT_Get_Char_Index(priv->face, idx);
  int err1 = FT_Load_Glyph(priv->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  return priv->face->glyph->bitmap_top;
}

int FontGlyphBitmap::SizeX() const
{
  int val = priv->face->glyph->bitmap.width;
  std::cout << "SizeX:" << val << std::endl;
  return val;
}

int FontGlyphBitmap::SizeY() const
{
  int val = priv->face->glyph->bitmap.rows; 
  std::cout << "SizeY:" << val << std::endl;
  return val;
}

int FontGlyphBitmap::Map(int x, int y) const
{
  return (int)priv->face->glyph->bitmap.buffer[x+y*priv->face->glyph->bitmap.pitch];
}
