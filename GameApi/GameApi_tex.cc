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
  RenderVertexArray *arr = new RenderVertexArray(g_low, *ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT GameApi::VA GameApi::TextureApi::bind_cubemap(GameApi::VA va, GameApi::TXID tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = SPECIAL_TEX_ID_CUBEMAP+tx.id;
  RenderVertexArray *arr = new RenderVertexArray(g_low, *ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT GameApi::VA GameApi::TextureApi::bind_many(GameApi::VA va, std::vector<GameApi::TXID> vec, std::vector<int> types)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  int s1 = vec.size();
  for(int i=0;i<s1;i++) {
    int type = 0;
    if (i>=0 && i<types.size()) {
      type = types[i];
    }
    TextureID *id = find_txid(e,vec[i]);
    int txid = id->texture();
    if (type==1) txid+=SPECIAL_TEX_ID_CUBEMAP;
    ns->texture_many_ids.push_back(txid);
  }
  RenderVertexArray *arr = new RenderVertexArray(g_low,*ns);
  arr->prepare(0);
  return add_vertex_array(e, ns, arr);
}
EXPORT GameApi::VA GameApi::TextureApi::bind_arr(GameApi::VA va, GameApi::TXA tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = SPECIAL_TEX_IDA+tx.id;
  RenderVertexArray *arr = new RenderVertexArray(g_low,*ns);
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
  Low_GLuint id;
  GameApi::TXID txid;
  g_low->ogl->glGenTextures(1, &id);
  txid.id = id;
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+0);

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
      //      buf.Gen();
#ifndef THREADS
  buf.Gen();
#else
  buf.GenPrepare();

  int numthreads = 4;
  ThreadedUpdateTexture threads;
  int sx = flip.SizeX();
  int sy = flip.SizeY();
  int dsy = sy/numthreads + 1;
  std::vector<int> ids;
  for(int j=0;j<numthreads;j++)
    {
      int start_x = 0;
      int end_x = sx;
      int start_y = j*dsy;
      int end_y = (j+1)*dsy;
      if (start_y>sy) { start_y = sy; }
      if (end_y>sy) end_y = sy;
      
      if (end_y-start_y > 0)
	ids.push_back(threads.push_thread(&buf, start_x, end_x, start_y, end_y));
    }
  int ss = ids.size();
  for(int k=0;k<ss;k++)
    {
      threads.join(ids[k]);
    }
#endif

      
      if (sizex==-1) sizex=bm->SizeX();
      if (sizey==-1) sizey=bm->SizeY();
      if (sizex!=sizey) {std::cout << "Warning: Cubemap textures dimensions need to be the same" << std::endl; }

      if (bm->SizeX() != sizex) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }
      if (bm->SizeY() != sizey) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }


      g_low->ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, id);
      g_low->ogl->glTexImage2D(Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,Low_GL_RGBA,bm->SizeX(),bm->SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
    }
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MIN_FILTER,Low_GL_LINEAR);      
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE); 
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_R, Low_GL_CLAMP_TO_EDGE); 

  return txid;
}
EXPORT std::vector<GameApi::TXID> GameApi::TextureApi::prepare_many(EveryApi &ev, std::vector<BM> vec, std::vector<int> types, bool mipmaps)
{
  std::vector<Low_GLuint> ids;
  std::vector<GameApi::TXID> txidvec;
  ids.resize(vec.size());
  g_low->ogl->glGenTextures(vec.size(), &ids[0]);
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "I=" << i << std::endl;

      int type = 0;
      int s3 = types.size();
      if (i>=0 && i<s3) {
	type = types[i];
      }
      //std::cout << "type=" << type << std::endl;
      if (type!=-1) {
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+i);
      }
  
      if (type==1) {
	  g_low->ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, ids[i]);

	int sizex=-1;
	int sizey=-1;
	ARR arr = ev.bitmap_api.cubemap(vec[i]);
	ArrayType *array = find_array(e, arr);
	std::vector<int> vec2 = array->vec;	
	int sj = vec2.size();
	for(int j=0;j<sj;j++) {
	  //std::cout << "J=" << j << std::endl;

	  BM bmj;
	  bmj.id = vec2[j];
	  //BM bmj2 = ev.bitmap_api.scale_bitmap(ev, bmj, 128,128);
	  BitmapHandle *handle = find_bitmap(e, bmj);
	  if (!handle) {
	    continue; }
	  Bitmap<Color> *bm = find_color_bitmap(handle);
	  if (!bm) {
	    continue; }
	  bm->Prepare();


      if (sizex==-1) sizex=bm->SizeX();
      if (sizey==-1) sizey=bm->SizeY();
      if (sizex!=sizey) {std::cout << "Warning: Cubemap textures dimensions need to be the same" << std::endl; }

      if (bm->SizeX() != sizex) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }
      if (bm->SizeY() != sizey) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }

	  FlipColours flip(*bm);
	  BufferFromBitmap buf(flip);
#ifndef THREADS
	  buf.Gen();
#else
	  buf.GenPrepare();
  
	  int numthreads = 4;
	  ThreadedUpdateTexture threads;
	  int ssx = flip.SizeX();
	  int ssy = flip.SizeY();
	  int dsy = ssy/numthreads + 1;
	  std::vector<int> ids2;
	  for(int k=0;k<numthreads;k++)
	    {
	      int start_x = 0;
	      int end_x = ssx;
	      int start_y = k*dsy;
	      int end_y = (k+1)*dsy;
	      if (start_y>ssy) { start_y = ssy; }
	      if (end_y>ssy) end_y = ssy;
	      
	      if (end_y-start_y > 0)
		ids2.push_back(threads.push_thread(&buf, start_x, end_x, start_y, end_y));
	    }
	  int ss = ids2.size();
	  for(int t=0;t<ss;t++)
	    {
	      threads.join(ids2[t]);
	    }
#endif

	  
	  //      buf.Gen();
	  
	  int sx = bm->SizeX();
	  int sy = bm->SizeY();
	  bool power_of_two = true;
	  if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	    power_of_two = false;
	  if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	    power_of_two = false;
	  if (!power_of_two) std::cout << "Warning: cubemaps not power of two size" << std::endl;
	  //std::cout << "Size:" << bm->SizeX() << " " << bm->SizeY() << std::endl;
	  g_low->ogl->glTexImage2D(Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X+j,0,Low_GL_RGBA,bm->SizeX(),bm->SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
	  if (mipmaps)
	    g_low->ogl->glGenerateMipmap(Low_GL_TEXTURE_2D);

	  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MIN_FILTER,mipmaps?Low_GL_LINEAR_MIPMAP_LINEAR:Low_GL_LINEAR);      
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE); 
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_R, Low_GL_CLAMP_TO_EDGE); 

	//std::cout << "ids[i]=" << ids[i] << std::endl;

	}

	
      }

      if (type==0) {
      BitmapHandle *handle = find_bitmap(e, vec[i]);
      if (!handle) {
	GameApi::TXID id2;
	id2.id = ids[i];
	txidvec.push_back(id2);

	continue;
      }
      Bitmap<Color> *bm = find_color_bitmap(handle);
      if (!bm) {
	GameApi::TXID id2;
	id2.id = ids[i];
	txidvec.push_back(id2);
	continue;
      }
      bm->Prepare();
      FlipColours flip(*bm);
      BufferFromBitmap buf(flip);
#ifndef THREADS
  buf.Gen();
#else
  buf.GenPrepare();

  int numthreads = 4;
  ThreadedUpdateTexture threads;
  int ssx = flip.SizeX();
  int ssy = flip.SizeY();
  int dsy = ssy/numthreads + 1;
  std::vector<int> ids2;
  for(int i=0;i<numthreads;i++)
    {
      int start_x = 0;
      int end_x = ssx;
      int start_y = i*dsy;
      int end_y = (i+1)*dsy;
      if (start_y>ssy) { start_y = ssy; }
      if (end_y>ssy) end_y = ssy;
      
      if (end_y-start_y > 0)
	ids2.push_back(threads.push_thread(&buf, start_x, end_x, start_y, end_y));
    }
  int ss = ids2.size();
  for(int i=0;i<ss;i++)
    {
      threads.join(ids2[i]);
    }
#endif

      
  //      buf.Gen();

      int sx = bm->SizeX();
      int sy = bm->SizeY();
      bool power_of_two = true;
      if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	power_of_two = false;
      if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	power_of_two = false;
      
	g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, ids[i]);
	g_low->ogl->glTexImage2D(Low_GL_TEXTURE_2D,0,Low_GL_RGBA,bm->SizeX(),bm->SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_LINEAR);      
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
	g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE); // these cause power-of-two texture requirement in emscripten.

      }
      //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
      GameApi::TXID id2;
      id2.id = ids[i];
      txidvec.push_back(id2);
    }
  assert(txidvec.size()==vec.size());
  return txidvec;
}
EXPORT GameApi::TXA GameApi::TextureApi::prepare_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy)
{
  Low_GLuint id;
  g_low->ogl->glGenTextures(1, &id); 

  if (vec.size()==0) { GameApi::TXA i; i.id=id; return i; }

  Low_GLsizei width = sx;
  Low_GLsizei height = sy;
  Low_GLsizei layercount = vec.size();

  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, id);
#ifndef EMSCRIPTEN
  g_low->ogl->glTexStorage3D(Low_GL_TEXTURE_2D_ARRAY, 1, Low_GL_RGBA8, width, height, layercount);
#endif

  int s = layercount;
  for(int i=0;i<s;i++)
    {
      BM bm = vec[i];
      BM bm2 = ev.bitmap_api.scale_bitmap(ev, bm, sx,sy);

      BitmapHandle *handle = find_bitmap(e, bm2);
      ::Bitmap<Color> *b2 = find_color_bitmap(handle);
      
      Low_GLsizei twidth = b2->SizeX();
      Low_GLsizei theight = b2->SizeY();
      assert(twidth==width);
      assert(theight==height);

      BufferFromBitmap buf(*b2);
      buf.Gen();
#ifndef EMSCRIPTEN
      g_low->ogl->glTexSubImage3D(Low_GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
#endif
    }
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);	
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);

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
  //  buf.Gen();

#ifndef THREADS
  buf.Gen();
#else
  buf.GenPrepare();

  int numthreads = 4;
  ThreadedUpdateTexture threads;
  int sx = flip.SizeX();
  int sy = flip.SizeY();
  int dsy = sy/numthreads + 1;
  std::vector<int> ids;
  for(int i=0;i<numthreads;i++)
    {
      int start_x = 0;
      int end_x = sx;
      int start_y = i*dsy;
      int end_y = (i+1)*dsy;
      if (start_y>sy) { start_y = sy; }
      if (end_y>sy) end_y = sy;
      
      if (end_y-start_y > 0)
	ids.push_back(threads.push_thread(&buf, start_x, end_x, start_y, end_y));
    }
  int ss = ids.size();
  for(int i=0;i<ss;i++)
    {
      threads.join(ids[i]);
    }
#endif

  
  Low_GLuint id;
  g_low->ogl->glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  g_low->ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, bm.SizeX(),bm.SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);	
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;
}

EXPORT void GameApi::TextureApi::use(TXID tx, int i)
{
  g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+i);
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, tx.id);
}
EXPORT void GameApi::TextureApi::use_many(std::vector<TXID> tx, int i)
{
  g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
  int s = tx.size();
  for(int i=0;i<s;i++)
    {
    g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
    g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
    g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, tx[i].id);
    }
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+0);
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif

}
EXPORT void GameApi::TextureApi::unuse(TXID tx)
{
  g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
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
    glBindTexture(Low_GL_TEXTURE_2D, id);
  
#ifndef EMSCRIPTEN
    glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_HEIGHT, &height);
#endif
    ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  glReadBuffer( Low_GL_COLOR_ATTACHMENT0 );
  glGetTexImage( Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
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

  MainLoopEnv e2;
  e2.in_MV = Matrix::Identity();
  e2.in_T = Matrix::Identity();
  e2.in_N = Matrix::Identity();
  txid->render(e2);

  
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  int width=256, height=256;
#ifndef EMSCRIPTEN
  g_low->ogl->glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_WIDTH, &width);
  g_low->ogl->glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_HEIGHT, &height);
#endif
  BufferRef ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  g_low->ogl->glReadBuffer( Low_GL_COLOR_ATTACHMENT0 );
  g_low->ogl->glGetTexImage( Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
#endif
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, 0);

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

GameApi::TXID GameApi::TextureApi::bufferref_to_txid(GameApi::TXID old, const BufferRef &buf)
{
      int sx = buf.width;
      int sy = buf.height;
      bool power_of_two = true;
      if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	power_of_two = false;
      if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	power_of_two = false;


  //std::cout << "bufferref_to_txid:" << buf.width << "x" << buf.height << ":" << buf.buffer << std::endl;
  Low_GLuint id;
  bool newbuffer = false;
  if (old.id==-1 || old.id==0) {
    g_low->ogl->glGenTextures(1, &id);
    //newbuffer = true;
  }
  else
    id=old.id;
#ifndef EMSCRIPTEN
  g_low->ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  g_low->ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  //std::cout << "buf.width=" << buf.width << " " << "buf.height=" << buf.height << " " << (int)buf.buffer << std::endl;
  //  if (newbuffer) {

    g_low->ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, buf.width,buf.height, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.buffer);
    // } else {
    //   g_low->ogl->glTexSubImage2D(Low_GL_TEXTURE_2D, 0, 0,0, buf.width,buf.height, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.buffer);
    // }
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  g_low->ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;  
}
