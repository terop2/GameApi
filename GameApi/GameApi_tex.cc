#include "GameApi_h.hh"

EXPORT GameApi::TextureApi::TextureApi(GameApi::Env &e) : e(e) { count=0; }

EXPORT GameApi::TX GameApi::TextureApi::tex_plane(int sx, int sy)
{
  return add_texture(e, new TexPlane(sx,sy));
}
EXPORT GameApi::TX GameApi::TextureApi::tex_bitmap(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  return add_texture(e, new TexBitmap(*bmc));
}
EXPORT GameApi::VA GameApi::TextureApi::bind(GameApi::VA va, GameApi::TXID tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = SPECIAL_TEX_ID+tx.id;
  RenderVertexArray *arr = new RenderVertexArray(*ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT GameApi::VA GameApi::TextureApi::bind_arr(GameApi::VA va, GameApi::TXA tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = SPECIAL_TEX_IDA+tx.id;
  RenderVertexArray *arr = new RenderVertexArray(*ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT int GameApi::TextureApi::unique_id()
{
  count++;
  return count;
}
EXPORT GameApi::TX GameApi::TextureApi::tex_assign(GameApi::TX tex, int id, int x, int y, GameApi::BM bm)
{
  TextureI *texture = find_texture(e, tex);
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  return add_texture(e, new TexAssign(*texture, id, x, y, *bmc));
}
EXPORT GameApi::TX GameApi::TextureApi::tex_coord(GameApi::TX tex, int id, int x, int y, int width, int height)
{ TextureI *texture = find_texture(e, tex);
  return add_texture(e, new TextureITexCoord(*texture, id, x,y,width,height));
}
EXPORT GameApi::Q GameApi::TextureApi::get_tex_coord(TX tx, int id)
{
  return get_tex_coord_1(tx,id);
}
GameApi::Q GameApi::TextureApi::get_tex_coord_1(TX tx, int id)
{
  TextureI *tex = find_texture(e,tx);
  int s = tex->AreaCount();
  int i=0;
  for(;i<s;i++)
    {
      if (tex->Id(i)==id) { break; }
    }
  if (i==s) { Point2d p = { 0.0, 0.0 }; return add_tex_quad(e, p,p); } 
  Point2d p1 = tex->AreaS(i);
  Point2d p2 = tex->AreaE(i);
  return add_tex_quad(e, p1,p2);
}
EXPORT GameApi::TXA GameApi::TextureApi::prepare_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy)
{
  GLuint id;
  glGenTextures(1, &id); 

  if (vec.size()==0) { GameApi::TXA i; i.id=id; return i; }

  GLsizei width = sx;
  GLsizei height = sy;
  GLsizei layercount = vec.size();

  glBindTexture(GL_TEXTURE_2D_ARRAY, id);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layercount);

  int s = layercount;
  for(int i=0;i<s;i++)
    {
      BM bm = vec[i];
      BM bm2 = ev.bitmap_api.scale_bitmap(ev, bm, sx,sy);

      BitmapHandle *handle = find_bitmap(e, bm2);
      ::Bitmap<Color> *b2 = find_color_bitmap(handle);
      
      GLsizei twidth = b2->SizeX();
      GLsizei theight = b2->SizeY();
      assert(twidth==width);
      assert(theight==height);

      BufferFromBitmap buf(*b2);
      buf.Gen();

      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
    }
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  GameApi::TXA i; 
  i.id=id; 
  return i;
}
EXPORT GameApi::TXID GameApi::TextureApi::prepare(TX tx)
{
  TextureI *tex = find_texture(e, tx);
  TextureIBitmap bm(*tex);
  FlipColours flip(bm);
  BufferFromBitmap buf(flip);
  buf.Gen();

  GLuint id;
  glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  glClientActiveTexture(GL_TEXTURE0+0);
#endif
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.SizeX(),bm.SizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;
}

EXPORT void GameApi::TextureApi::use(TXID tx, int i)
{
  glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
  glClientActiveTexture(GL_TEXTURE0+i);
#endif
  glActiveTexture(GL_TEXTURE0+i);
  glBindTexture(GL_TEXTURE_2D, tx.id);
}
EXPORT void GameApi::TextureApi::unuse(TXID tx)
{
  glDisable(GL_TEXTURE_2D);
}
EXPORT GameApi::BM GameApi::TextureApi::to_bitmap(TXID tx)
{
  //EnvImpl *env = ::EnvImpl::Environment(&e);

  glBindTexture(GL_TEXTURE_2D, tx.id);
  int width=1, height=1;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
  BufferRef ref = BufferRef::NewBuffer(width, height);
  glReadBuffer( GL_COLOR_ATTACHMENT0 );
  glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);
  
  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  //env->deletes.push_back(std::shared_ptr<void>(bm));
  //Bitmap<Color> *bm2 = new FlipColours(*bm);
  return add_color_bitmap2(e, bm);
}
