
#include "GameApi_h.hh"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
//#include <freetype/ftoutln.h>
#include FT_OUTLINE_H
#include "FreeType2.hh"

struct GlyphData
{
  int top;
  int sx,sy;
  int *bitmap_data=0;

  FT_Library *lib=0;
  FT_Face face;
};

FontInterfaceImpl::FontInterfaceImpl(GameApi::Env &e, void *priv_, std::string ttf_filename, int sx, int sy) : e(e), ttf_filename(ttf_filename), sx(sx), sy(sy), priv_(priv_) 
{ 
  priv = 0;
}

int FontInterfaceImpl::Top(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return glyph_data[idx]->top;
}
int FontInterfaceImpl::SizeX(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return glyph_data[idx]->sx;
}
int FontInterfaceImpl::SizeY(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return glyph_data[idx]->sy;
}
int FontInterfaceImpl::Map(long idx, int x, int y) const
{
  if (x<0 || x>=SizeX(idx) || y<0 || y>=SizeY(idx))
    return 0;
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  int sx = SizeX(idx);
  return glyph_data[idx]->bitmap_data[x+y*sx];
}

void FontInterfaceImpl::gen_glyph_data(long idx)
{
  //std::cout << "try gen_glyph_data:" << idx << std::endl;
  GlyphData *data = glyph_data[idx];
  if (data) return;
  std::cout << "gen_glyph_data:" << idx << std::endl;

  if (!data) {
    data = new GlyphData;
    glyph_data[idx] = data;
  }

#ifndef EMSCRIPTEN
  e.async_load_url(ttf_filename);
#endif
  std::stringstream ss2;
  ss2 << "font" << idx << ".ttf";

  std::vector<unsigned char> *ptr = e.get_loaded_async_url(ttf_filename);
  if (!ptr) {
    std::cout << "async not ready yet, failing..." << std::endl;
    exit(0);
  } else {
    //std::fstream ss(ss2.str().c_str(), std::ios_base::binary | std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.close();
  }
  data->lib = (FT_Library*)priv_;
  unsigned char *ptr2 = new unsigned char[ptr->size()+1];
  std::copy(ptr->begin(), ptr->end(), ptr2);
  int err = FT_New_Memory_Face( *data->lib,
				ptr2 /*"font.ttf"*/,
				ptr->size(),
				0,
				&data->face);
  if (err!=0)
    {
    std::cout << "FT_New_Face ERROR: " << err << std::endl;
    std::cout << "Remember to recompile the code after changing envimpl size" << std::endl;
    exit(0);
    }
  FT_Set_Char_Size(data->face, sx*64,sy*64,100,100);

  FT_UInt glyphindex = FT_Get_Char_Index(data->face, idx);
  FT_Load_Glyph(data->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  FT_Render_Glyph(data->face->glyph, FT_RENDER_MODE_NORMAL);

  data->top = -data->face->glyph->bitmap_top;
  data->sx = data->face->glyph->bitmap.width;
  data->sy = data->face->glyph->bitmap.rows;
  data->bitmap_data = new int[data->sx*data->sy];
  for(int iy=0;iy<data->sy;iy++)
    for(int ix=0;ix<data->sx;ix++)
      {
	data->bitmap_data[ix+iy*data->sx] = (int)data->face->glyph->bitmap.buffer[ix+iy*data->face->glyph->bitmap.pitch];
      }
}
