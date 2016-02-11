
#include "GameApi_h.hh"

GameApi::LAY GameApi::LayoutApi::root(int sx, int sy)
{
  return add_layout(e, new RootLayout(sx,sy));
}
GameApi::LAY GameApi::LayoutApi::split_y(LAY l, int id, int num)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutY(*ll, id, num));
}
GameApi::LAY GameApi::LayoutApi::split_x(LAY l, int id, int num)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutX(*ll, id, num));
}
GameApi::LAY GameApi::LayoutApi::split_xy(LAY l, int id, int num_x, int num_y)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitXYLayout(*ll, id, num_x, num_y));
}
GameApi::LAY GameApi::LayoutApi::margin(LAY l, int id, int lx, int rx, int ty, int by)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new MarginLayout(*ll, id, lx,rx,ty,by));  
}
GameApi::LAY GameApi::LayoutApi::center(LAY l, int id, int cx, int cy)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new CenterLayout(*ll, id, cx, cy));
}
GameApi::LAY GameApi::LayoutApi::array(LAY *array, int *id, int size)
{
  std::vector<Layout*> *vec = new std::vector<Layout*>;
  for(int i=0;i<size;i++)
    {
      vec->push_back(find_layout(e,array[i]));
    }
  return add_layout(e, new ArrayLayout(&(*vec)[0], id, size));
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
