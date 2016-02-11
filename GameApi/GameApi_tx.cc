
#include "GameApi_h.hh"

EXPORT GameApi::TextApi::~TextApi()
{
  delete(std::vector<BM>*)priv;
}

EXPORT void GameApi::TextApi::load_font(std::string filename, int sx, int sy, int x, int y, char start_char, char end_char)
{
  this->sx = sx;
  this->sy = sy;
  this->start_char = start_char;
  this->end_char = end_char;
  std::vector<BM> *bms = new std::vector<BM>();
  priv = (void*)bms;
  BM bm_all = bm.loadtilebitmap(filename, sx,sy);
  for(int yy=0;yy<y;yy++)
    {
      for(int xx=0;xx<x;xx++)
	{
	  BM o = bm.findtile(bm_all, xx,yy);
	  //std::cout << "Text" << o.id << std::endl;
	  sp.preparesprite(o);
	  bms->push_back(o);
	}
    }
}
EXPORT void GameApi::TextApi::draw_text(std::string text, int x, int y, SH sh)
{
  int xpos = x;
  int ypos = y;
  int s = text.size();
  for(int i=0;i<s;i++)
    {
      char c = text[i];
      if (c!='\n' && c>=start_char && c<=end_char)
	{
	  BM b = ((std::vector<BM>*)priv)->operator[](c-start_char);
	  //std::cout << "draw_text" << b.id << std::endl;
	  sp.rendersprite(b, sh, xpos, ypos, 1.0, 1.0);
	}
      xpos += sx;
      if (c=='\n') { xpos=x; ypos+=sy; }
    }
}
