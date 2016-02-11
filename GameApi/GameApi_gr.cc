

#include "GameApi_h.hh"


EXPORT void GameApi::GridApi::preparegrid(GameApi::BM tile_bitmap, int tile_choose)
{
  GridPriv *p = (GridPriv*)priv;
  p->last_id = tile_bitmap.id;
  BitmapHandle *handle = find_bitmap(e, tile_bitmap);
  int sx = 1;
  int sy = 1;
  BitmapTileHandle *chandle = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle)
    {
      sx = chandle->tile_sx;
      sy = chandle->tile_sy;
    }
  
  p->cellsx[tile_bitmap.id] = sx;
  p->cellsy[tile_bitmap.id] = sy;

  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  if (!bitmap) { std::cout << "preparegrid bitmap==NULL" << std::endl; return; }

  ArrayRender *rend = &((GridPriv*)priv)->rend;
  PrepareGrid(*bitmap, sx, sy, *rend);
}

EXPORT void GameApi::GridApi::rendergrid(GameApi::BM grid, int grid_choose, float top_x, float top_y)
{
  GridPriv *p = (GridPriv*)priv;
  int cellsx = p->cellsx[p->last_id];
  int cellsy = p->cellsy[p->last_id];
  
  BitmapHandle *handle = find_bitmap(e, grid);
  ::Bitmap<Pos> *bitmap = find_pos_bitmap(handle);
  if (!bitmap) { std::cout << "rendergrid bitmap==NULL" << std::endl; return; }
  RenderGrid(*bitmap, top_x, top_y, cellsx, cellsy, p->rend, 0,0, 10,10);
}
