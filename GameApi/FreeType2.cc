
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
  int top;
  int sx,sy;
  int advance_x;
  int *bitmap_data=0;

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



int FontInterfaceImpl::Top(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return global_glyph_data[key]->operator[](idx)->top;
}
int FontInterfaceImpl::SizeX(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return global_glyph_data[key]->operator[](idx)->sx;
}
int FontInterfaceImpl::SizeY(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return global_glyph_data[key]->operator[](idx)->sy;
}
int FontInterfaceImpl::AdvanceX(long idx) const {
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  return global_glyph_data[key]->operator[](idx)->advance_x;
}
int FontInterfaceImpl::Map(long idx, int x, int y) const
{
  if (x<0 || x>=SizeX(idx) || y<0 || y>=SizeY(idx))
    return 0;
  const_cast<FontInterfaceImpl*>(this)->gen_glyph_data(idx);
  int ssx = SizeX(idx);
  return global_glyph_data[key]->operator[](idx)->bitmap_data[x+y*ssx];
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



void FontInterfaceImpl::gen_glyph_data(long idx)
{
  //std::cout << "try gen_glyph_data:" << idx << std::endl;
  //std::string key = glyph_key(ttf_filename,sx,sy);
#ifndef EMSCRIPTEN
  pthread_mutex_lock(&mutex);
#endif
  std::map<long, GlyphData*> *mymap = global_glyph_data[key];
  if (!mymap) {
    global_glyph_data[key] = new std::map<long,GlyphData*>();
    mymap = global_glyph_data[key];
  }
  GlyphData *data = mymap?mymap->operator[](idx):0; //glyph_data[idx];
  if (data) { 
#ifndef EMSCRIPTEN
    pthread_mutex_unlock(&mutex);
#endif
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
  e.async_load_url(ttf_filename, homepage);
#endif
  std::stringstream ss2;
  ss2 << "font" << idx << ".ttf";

  std::vector<unsigned char> *ptr = e.get_loaded_async_url(ttf_filename);
  if (!ptr) {
    std::cout << "async not ready yet, failing..." << std::endl;
#ifndef EMSCRIPTEN
    pthread_mutex_unlock(&mutex);
#endif
    exit(0);
  } else {
    //std::fstream ss(ss2.str().c_str(), std::ios_base::binary | std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.close();
  }
  ptr2 = new unsigned char[ptr->size()+1];
  std::copy(ptr->begin(), ptr->end(), ptr2);
  size = ptr->size();
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
#ifndef EMSCRIPTEN
    pthread_mutex_unlock(&mutex);
#endif
    //TODO exit(0);
    }
  FT_Set_Char_Size(data->face, sx*64,sy*64,100,100);

  FT_UInt glyphindex = FT_Get_Char_Index(data->face, idx);
  FT_Load_Glyph(data->face, glyphindex, FT_LOAD_RENDER|FT_LOAD_TARGET_NORMAL);
  FT_Render_Glyph(data->face->glyph, FT_RENDER_MODE_NORMAL);

  data->top = -data->face->glyph->bitmap_top;
  data->sx = data->face->glyph->bitmap.width;
  data->sy = data->face->glyph->bitmap.rows;
  data->advance_x = data->face->glyph->advance.x >> 6;
  //std::cout << "Glyph:" << idx << " " << data->sx << " " << data->sy << " " << data->top << std::endl;
  data->bitmap_data = new int[data->sx*data->sy];
  for(int iy=0;iy<data->sy;iy++)
    for(int ix=0;ix<data->sx;ix++)
      {
	data->bitmap_data[ix+iy*data->sx] = (int)data->face->glyph->bitmap.buffer[ix+iy*data->face->glyph->bitmap.pitch];
      }
#ifndef EMSCRIPTEN
    pthread_mutex_unlock(&mutex);
#endif

}
#endif
