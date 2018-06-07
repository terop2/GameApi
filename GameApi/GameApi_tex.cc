#include "GameApi_h.hh"
#include "GameApi_low.hh"

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
EXPORT GameApi::VA GameApi::TextureApi::bind_cubemap(GameApi::VA va, GameApi::TXID tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = SPECIAL_TEX_ID_CUBEMAP+tx.id;
  RenderVertexArray *arr = new RenderVertexArray(*ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT GameApi::VA GameApi::TextureApi::bind_many(GameApi::VA va, std::vector<GameApi::TXID> vec)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  int s1 = vec.size();
  for(int i=0;i<s1;i++) {
    ns->texture_many_ids.push_back(vec[i].id);
  }
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
EXPORT GameApi::TXID GameApi::TextureApi::prepare_cubemap(EveryApi &ev, BM right, BM left, BM top, BM bottom, BM back, BM front)
{
  GLuint id;
  GameApi::TXID txid;
  g_low->ogl->glGenTextures(1, &id);
  txid.id = id;
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(GL_TEXTURE0+0);

  std::vector<BM> vec;
  vec.push_back(right);
  vec.push_back(left);
  vec.push_back(top);
  vec.push_back(bottom);
  vec.push_back(back);
  vec.push_back(front);


  int sizex=-1;
  int sizey=-1;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      BitmapHandle *handle = find_bitmap(e, vec[i]);
      Bitmap<Color> *bm = find_color_bitmap(handle);
      FlipColours flip(*bm);
      BufferFromBitmap buf(flip);
      buf.Gen();

      if (sizex==-1) sizex=bm->SizeX();
      if (sizey==-1) sizey=bm->SizeY();
      if (sizex!=sizey) {std::cout << "Warning: Cubemap textures dimensions need to be the same" << std::endl; }

      if (bm->SizeX() != sizex) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }
      if (bm->SizeY() != sizey) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }


      g_low->ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, id);
      g_low->ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGBA,bm->SizeX(),bm->SizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
    }
  g_low->ogl->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  g_low->ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  g_low->ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  g_low->ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
  g_low->ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
  g_low->ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

  return txid;
}
EXPORT std::vector<GameApi::TXID> GameApi::TextureApi::prepare_many(EveryApi &ev, std::vector<BM> vec)
{
  std::vector<GLuint> ids;
  std::vector<GameApi::TXID> txidvec;
  ids.resize(vec.size());
  g_low->ogl->glGenTextures(vec.size(), &ids[0]);
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(GL_TEXTURE0+0);
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      BitmapHandle *handle = find_bitmap(e, vec[i]);
      Bitmap<Color> *bm = find_color_bitmap(handle);
      FlipColours flip(*bm);
      BufferFromBitmap buf(flip);
      buf.Gen();

      int sx = bm->SizeX();
      int sy = bm->SizeY();
      bool power_of_two = true;
      if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	power_of_two = false;
      if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	power_of_two = false;
      
      g_low->ogl->glBindTexture(GL_TEXTURE_2D, ids[i]);
      g_low->ogl->glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bm->SizeX(),bm->SizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
      g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
      g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
      g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, power_of_two?GL_REPEAT:GL_CLAMP_TO_EDGE); // GL_REPEAT
      g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, power_of_two?GL_REPEAT:GL_CLAMP_TO_EDGE); // these cause power-of-two texture requirement in emscripten.
      g_low->ogl->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
      GameApi::TXID id2;
      id2.id = ids[i];
      txidvec.push_back(id2);
    }
  return txidvec;
}
EXPORT GameApi::TXA GameApi::TextureApi::prepare_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy)
{
  GLuint id;
  g_low->ogl->glGenTextures(1, &id); 

  if (vec.size()==0) { GameApi::TXA i; i.id=id; return i; }

  GLsizei width = sx;
  GLsizei height = sy;
  GLsizei layercount = vec.size();

  g_low->ogl->glBindTexture(GL_TEXTURE_2D_ARRAY, id);
#ifndef EMSCRIPTEN
  g_low->ogl->glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layercount);
#endif

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
#ifndef EMSCRIPTEN
      g_low->ogl->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
#endif
    }
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);      
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  g_low->ogl->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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
  g_low->ogl->glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(GL_TEXTURE0+0);
  g_low->ogl->glBindTexture(GL_TEXTURE_2D, id);
  g_low->ogl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.SizeX(),bm.SizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);      
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  g_low->ogl->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;
}

EXPORT void GameApi::TextureApi::use(TXID tx, int i)
{
  g_low->ogl->glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(GL_TEXTURE0+i);
#endif
  g_low->ogl->glActiveTexture(GL_TEXTURE0+i);
  g_low->ogl->glBindTexture(GL_TEXTURE_2D, tx.id);
}
EXPORT void GameApi::TextureApi::use_many(std::vector<TXID> tx, int i)
{
  g_low->ogl->glEnable(GL_TEXTURE_2D);
  int s = tx.size();
  for(int i=0;i<s;i++)
    {
    g_low->ogl->glActiveTexture(GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
    g_low->ogl->glClientActiveTexture(GL_TEXTURE0+i);
#endif
    g_low->ogl->glBindTexture(GL_TEXTURE_2D, tx[i].id);
    }
  g_low->ogl->glActiveTexture(GL_TEXTURE0+0);
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(GL_TEXTURE0+0);
#endif

}
EXPORT void GameApi::TextureApi::unuse(TXID tx)
{
  g_low->ogl->glDisable(GL_TEXTURE_2D);
}

class TextureID;

#if 0
class ToBitmap : public Bitmap<Color>
{
public:
  ToBitmap(TextureID *id) : id(id) { ref=Buffer::NewBuffer(1,1); }
  virtual int SizeX() const
  {
    return m_width;
  }
  virtual int SizeY() const
  {
    return m_height;
  }
  virtual Color Map(int x, int y) const
  {
    return ref.buffer[x+y*ref.ydelta];
  }
  virtual void Prepare()
  {
    int width=256, height=256;
    TextureID *txid = id;
    MainLoopEnv e;
    e.in_MV = Matrix::Identity();
    e.in_T = Matrix::Identity();
    e.in_N = Matrix::Identity();
    txid->render(e);
    int id = txid->texture();
    glBindTexture(GL_TEXTURE_2D, id);
  
#ifndef EMSCRIPTEN
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
#endif
    ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  glReadBuffer( GL_COLOR_ATTACHMENT0 );
  glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);
#endif


  int xx = ref.width;
  int yy = ref.height;
  for(int y = 0; y<yy; y++)
    for(int x = 0; x<xx; x++)
      {
	unsigned int col = ref.buffer[x+y*ref.ydelta];
	int r = col &0xff0000;
	int g = col &0xff00;
	int b = col &0xff;
	r >>= 16;
	g >>= 8;
	b <<= 16;
	g <<= 8;
	unsigned int col2 = (col & 0xff000000) + r+g+b;
	ref.buffer[x+y*ref.ydelta] = col2;
      }

  
    m_width = width;
    m_height = height;
  }
  //virtual void handle_event(MainLoopEvent &e) { }
  //virtual void render(MainLoopEnv &e) {
  //  id->render(e);
  //  Prepare();
  // }
private:
  TextureID *id;
  int m_width, m_height;
  BufferRef ref;
};


EXPORT GameApi::BM GameApi::TextureApi::to_bitmap(TXID tx)
{
  TextureID *id = find_txid(e,tx);
  return add_color_bitmap(e, new ToBitmap(id));
}
#endif
#if 1
EXPORT GameApi::BM GameApi::TextureApi::to_bitmap(TXID tx)
{
  //EnvImpl *env = ::EnvImpl::Environment(&e);

  TextureID *txid = find_txid(e, tx);
  int id = txid->texture();
  
  g_low->ogl->glBindTexture(GL_TEXTURE_2D, id);
  int width=256, height=256;
#ifndef EMSCRIPTEN
  g_low->ogl->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  g_low->ogl->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
#endif
  BufferRef ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  g_low->ogl->glReadBuffer( GL_COLOR_ATTACHMENT0 );
  g_low->ogl->glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);
#endif

  int xx = ref.width;
  int yy = ref.height;
  for(int y = 0; y<yy; y++)
    for(int x = 0; x<xx; x++)
      {
	unsigned int col = ref.buffer[x+y*ref.ydelta];
	int r = col &0xff0000;
	int g = col &0xff00;
	int b = col &0xff;
	r >>= 16;
	g >>= 8;
	b <<= 16;
	g <<= 8;
	unsigned int col2 = (col & 0xff000000) + r+g+b;
	ref.buffer[x+y*ref.ydelta] = col2;
      }
  
  Bitmap<Color> *bm = new BitmapFromBuffer(ref);

  //env->deletes.push_back(std::shared_ptr<void>(bm));
  //Bitmap<Color> *bm2 = new FlipColours(*bm);
  return add_color_bitmap2(e, bm);
}
#endif
