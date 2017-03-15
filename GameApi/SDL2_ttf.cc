
#define SDL2_USED
#define GAMEAPI_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifdef EMSCRIPTEN
#include <SDL/SDL_ttf.h>
#include <emscripten.h>
#else
#include <SDL2/SDL_ttf.h>
#endif
#include "GameApi_h.hh"
#include <unistd.h>
extern std::map<std::string, std::vector<unsigned char>*> load_url_font_buffers;

struct GlyphPriv
{
  TTF_Font *font;
  SDL_Surface *surf;
};
FontGlyphBitmap::FontGlyphBitmap(void *priv_, std::string filename, int sx, int sy) : m_sx(sx), m_sy(sy), url(filename)
{
  priv = new GlyphPriv;
  priv->font = 0;
  priv->surf = 0;
}
FontGlyphBitmap::~FontGlyphBitmap()
{
  delete (GlyphPriv*)priv;
}
extern std::vector<unsigned char> load_from_url(std::string url);
void FontGlyphBitmap::Prepare()
{
  std::cout << "FontGlyphBitmap::Prepare() " << url << std::endl;
#ifdef EMSCRIPTEN
  url = "load_url.php?url=" + url;
  
  std::vector<unsigned char> *vec = load_url_font_buffers[url];
  std::fstream ss("font.ttf",std::ios_base::out|std::ios_base::binary);
  int s = vec->size();
  for(int i=0;i<s;i++) ss.put(vec->operator[](i));
  ss.close();
  std::string filename;
  char buff[100];
  getcwd(buff, sizeof(buff));
  filename = buff;
  //filename+="\\";
  filename+="font.ttf";
  std::cout << "Loading " << filename << std::endl;

  if (!priv->font)
    priv->font = TTF_OpenFont( filename.c_str(), m_sy ); 
  std::cout << TTF_GetError() << std::endl;
#else
  std::vector<unsigned char> buf = load_from_url(url);
  std::cout << "url loaded!" << std::endl;
  std::fstream ss("font.ttf", std::ios_base::out|std::ios_base::binary);
  int s = buf.size();
  for(int i=0;i<s;i++) ss.put(buf.operator[](i));
  ss.close();
  std::cout << "file saved!" << std::endl;
  std::string filename;
  char buff[100];
  getcwd(buff, sizeof(buff));
  filename = buff;
  filename+="\\";
  filename+="font.ttf";
  std::cout << "Loading " << filename << std::endl;

  if (!priv->font)
    priv->font = TTF_OpenFont( filename.c_str(), m_sy ); 
  std::cout << "ttf_openfont done!" << std::endl;
  std::cout << TTF_GetError() << std::endl;
#endif
  
}
void FontGlyphBitmap::load_glyph(long idx)
{
  char c = (char)idx;
  char *ptr = new char[2];
  ptr[0] = c;
  ptr[1] = 0;
  GlyphPriv *p = (GlyphPriv*)priv;
  SDL_Color col;
  col.r = 255;
  col.g = 255;
  col.b = 255;
  //col.a = 255;
  if (p->font)
    p->surf = TTF_RenderText_Blended(p->font, ptr, col);
  std::cout << TTF_GetError() << std::endl;
}
int FontGlyphBitmap::NumLines() const
{
  return 0;
}
Point FontGlyphBitmap::LinePoint(int line, int point) const
{
  Point p;
  return p;
}
void FontGlyphBitmap::load_glyph_outline(long idx, float sx, float sy)
{
}
int FontGlyphBitmap::bitmap_top(long idx) const
{
  //GlyphPriv *p = (GlyphPriv*)priv;
  return 0; //TTF_FontAscent(p->font);
}
int FontGlyphBitmap::SizeX() const
{
  GlyphPriv *p = (GlyphPriv*)priv;
  if (p->surf)
    return p->surf->w;
  return 0;
}
int FontGlyphBitmap::SizeY() const
{
  GlyphPriv *p = (GlyphPriv*)priv;
  if (p->surf)
    return p->surf->h;
  return 0;
}
int FontGlyphBitmap::Map(int x, int y) const
{
  if (x<0 || x>=SizeX() || y<0 || y>=SizeY())
    return 0;
  GlyphPriv *p = (GlyphPriv*)priv;
  unsigned int gray = 255;
  if (p->surf) {
    SDL_LockSurface(p->surf);
    void *pix = p->surf->pixels;
    Uint32 *ptr = (Uint32*)pix;
    unsigned int color = ptr[(p->surf->format->BytesPerPixel*x+y*p->surf->pitch)/sizeof(Uint32)];
    SDL_UnlockSurface(p->surf);
    gray = color &0xff000000;
    gray >>= 24;
  }
  return gray;
}
