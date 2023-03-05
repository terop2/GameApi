
#include "GameApi_h.hh"

GameApi::LAY GameApi::LayoutApi::root(int sx, int sy)
{
#if 0
  return add_layout(e, new RootLayout(sx,sy));
#endif
}
GameApi::LAY GameApi::LayoutApi::split_y(LAY l, int id, int num)
{
#if 0
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutY(*ll, id, num));
#endif
}
GameApi::LAY GameApi::LayoutApi::split_x(LAY l, int id, int num)
{
#if 0
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutX(*ll, id, num));
#endif
}
GameApi::LAY GameApi::LayoutApi::split_xy(LAY l, int id, int num_x, int num_y)
{
#if 0
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitXYLayout(*ll, id, num_x, num_y));
#endif
}
GameApi::LAY GameApi::LayoutApi::margin(LAY l, int id, int lx, int rx, int ty, int by)
{
#if 0
  Layout *ll = find_layout(e, l);
  return add_layout(e, new MarginLayout(*ll, id, lx,rx,ty,by));  
#endif
}
GameApi::LAY GameApi::LayoutApi::center(LAY l, int id, int cx, int cy)
{
#if 0
  Layout *ll = find_layout(e, l);
  return add_layout(e, new CenterLayout(*ll, id, cx, cy));
#endif
}
GameApi::LAY GameApi::LayoutApi::array(LAY *array, int *id, int size)
{
#if 0
  std::vector<Layout*> *vec = new std::vector<Layout*>;
  for(int i=0;i<size;i++)
    {
      vec->push_back(find_layout(e,array[i]));
    }
  return add_layout(e, new ArrayLayout(&(*vec)[0], id, size));
#endif
}

int GameApi::LayoutApi::count(LAY l)
{
  Layout *ll = find_layout(e, l);
  return ll->count();
}
int GameApi::LayoutApi::pos_x(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).x;
}
int GameApi::LayoutApi::pos_y(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).y;
}

int GameApi::LayoutApi::size_x(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).width;
}
int GameApi::LayoutApi::size_y(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).height;
}
