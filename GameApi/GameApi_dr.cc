
#include "GameApi_h.hh"

#if 0
GameApi::DR GameApi::DrawApi::label(LAY l, int id, std::string str, Ft font)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(0, tl, br);
  Draw *dr1 = new DrawProp(*dr, "label", str);
  Draw *dr2 = new DrawProp(*dr1, "font", font.id);
  return add_draw(e, dr2);
}

GameApi::DR GameApi::DrawApi::icon(LAY l, int id, BM bm)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(1, tl, br);
  Draw *dr1 = new DrawProp(*dr, "bitmap", bm.id);
  return add_draw(e, dr1);
}

GameApi::DR GameApi::DrawApi::rect(LAY l, int id, unsigned int color)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(2, tl, br);
  Draw *dr1 = new DrawProp(*dr, "rect", color);
  return add_draw(e, dr1);
}
GameApi::DR GameApi::DrawApi::scroll(DR dr, int delta_x, int delta_y)
{
  Draw *ddr = find_draw(e, dr);
  Point2d p = { 0.0, 0.0 };
  Draw *drx = new DrawEmpty(3, p,p); 
  Draw *dr1 = new DrawProp(*dr, "delta_x", delta_x);
  Draw *dr2 = new DrawProp(*dr1, "delta_y", delta_y);
  Draw *dr3 = new DrawProp(*dr2, "draw", ddr);
  return add_draw(e, dr3);
}
GameApi::DR GameApi::DrawApi::scroll_area(DR dr, LAY l, int id);
GameApi::DR GameApi::DrawApi::cliprect(DR cmds, LAY l, int id);
#endif
