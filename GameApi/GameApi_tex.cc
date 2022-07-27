#include "GameApi_h.hh"
#include "GameApi_low.hh"
#include <iomanip>


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
  //RenderVertexArray *arr = find_vertex_array_render(e,va);
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
  s->free_memory(); // experimental (move semantics s->ns)
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

void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max=15);

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
  OpenglLowApi *ogl = g_low->ogl;

  Low_GLuint id;
  GameApi::TXID txid;
  ogl->glGenTextures(1, &id);
  txid.id = id;
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);

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
  InstallProgress(767, "cubetexture", s*4);
  for(int i=0;i<s;i++)
    {
      BitmapHandle *handle = find_bitmap(e, vec[i]);
      Bitmap<Color> *bm = find_color_bitmap(handle);


      if (sizex==-1) sizex=bm->SizeX();
      if (sizey==-1) sizey=bm->SizeY();
      if (sizex!=sizey) {std::cout << "Warning: Cubemap textures dimensions need to be the same" << std::endl;
	if (sizex>sizey) { sizex=sizey; } else { sizey=sizex; }
      }
	bm = new SubBitmap<Color>(*bm, 0, 0, sizex, sizey);


      
      FlipColours flip(*bm);
      BufferFromBitmap buf(flip);
      //      buf.Gen();
#ifndef THREADS
  buf.Gen();
  ProgressBar(767, i*4, s*4, "cubetexture");
#else
  buf.GenPrepare();

  int numthreads = 8;
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
      ProgressBar(767, i*4+k, s*4, "cubetexture");
    }
#endif

      
      if (bm->SizeX() != sizex) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }
      if (bm->SizeY() != sizey) { std::cout << "Warning: Cubemap textures need to be same size" << std::endl; }


      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, id);
      ogl->glTexImage2D(Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,Low_GL_RGBA,sizex,sizey, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
    }
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MIN_FILTER,Low_GL_LINEAR);      
  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE); 
  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_R, Low_GL_CLAMP_TO_EDGE); 

  return txid;
}

struct TXIDCACHE
{
  std::string label;
  GameApi::TXID id;
};
std::vector<TXIDCACHE> idcache;
GameApi::TXID find_txid(std::string label)
{
  int s = idcache.size();
  for(int i=0;i<s;i++)
    {
      if (label==idcache[i].label) return idcache[i].id;
    }
  GameApi::TXID id;
  id.id = -1;
  return id;
}

EXPORT std::vector<GameApi::TXID> GameApi::TextureApi::prepare_many(EveryApi &ev, std::vector<BM> vec, std::vector<int> types, bool mipmaps, std::vector<std::string> id_labels)
{

  mipmaps = false;
  
  OpenglLowApi *ogl = g_low->ogl;
  //mipmaps = false;

  std::vector<Low_GLuint> ids;
  std::vector<GameApi::TXID> txidvec;
  ids.resize(vec.size());
  ogl->glGenTextures(vec.size(), &ids[0]);
  int s = vec.size();
  //InstallProgress(768, "texturemany", s*4);

  for(int i=0;i<s;i++)
    {
      //std::cout << "I=" << i << std::endl;
      if (i<id_labels.size()) {
	TXID id = find_txid(id_labels[i]);
	if (id.id!=-1) {
	  txidvec.push_back(id);
	  continue;
	}
      }
      
      int type = 0;
      int s3 = types.size();
      if (i>=0 && i<s3) {
	type = types[i];
      }
      //std::cout << "type=" << type << std::endl;
      if (type!=-1) {
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+i);
      }
  
      if (type==1) {
	  ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, ids[i]);

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
	  //ProgressBar(768, i*4+4,s*4, "texturemany"); 
#else
	  buf.GenPrepare();
  
	  int numthreads = 8;
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
	      //ProgressBar(768, i*4+t,s*4, "texturemany"); 
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
	  ogl->glTexImage2D(Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X+j,0,Low_GL_RGBA,bm->SizeX(),bm->SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
	  if (mipmaps&&power_of_two)
	    ogl->glGenerateMipmap(Low_GL_TEXTURE_2D);

	  ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MIN_FILTER,mipmaps&&power_of_two?Low_GL_LINEAR_MIPMAP_LINEAR:Low_GL_LINEAR);      
	ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
	ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
	ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE); 
	ogl->glTexParameteri(Low_GL_TEXTURE_CUBE_MAP,Low_GL_TEXTURE_WRAP_R, Low_GL_CLAMP_TO_EDGE); 

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
  //ProgressBar(768, i*4+4,s*4, "texturemany"); 
#else
  buf.GenPrepare();

  int numthreads = 8;
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
  int ii = i;
  for(int i=0;i<ss;i++)
    {
      threads.join(ids2[i]);
      //ProgressBar(768, ii*4+i,s*4, "texturemany"); 

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
      if (!power_of_two) { /*std::cout << "Warning: texture not in power_of_two, mipmapping is disabled" << std::endl;*/ }
      
	ogl->glBindTexture(Low_GL_TEXTURE_2D, ids[i]);
	ogl->glTexImage2D(Low_GL_TEXTURE_2D,0,Low_GL_RGBA,bm->SizeX(),bm->SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
#ifdef EMSCRIPTEN
	if (mipmaps&&power_of_two)
#endif
	    ogl->glGenerateMipmap(Low_GL_TEXTURE_2D);

	int filter = mipmaps&&power_of_two?Low_GL_LINEAR_MIPMAP_LINEAR:Low_GL_LINEAR;
#if defined(WINDOWS) || defined(LINUX)
	filter = Low_GL_LINEAR_MIPMAP_LINEAR;
#endif
	
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,filter);      
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
#if defined(LINUX)||defined(WINDOWS)
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_REPEAT); // GL_REPEAT
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_REPEAT); // these cause power-of-two texture requirement in emscripten.
#else
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE); // GL_REPEAT
	ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE); // these cause power-of-two texture requirement in emscripten.
#endif	
	
    }
      //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
      GameApi::TXID id2;
      id2.id = ids[i];
      txidvec.push_back(id2);
      if (i<id_labels.size()) {
      TXIDCACHE idcacheitem;
      idcacheitem.label = id_labels[i];
      idcacheitem.id = id2;
      idcache.push_back(idcacheitem);
      }
    }
  assert(txidvec.size()==vec.size());
  return txidvec;
}
EXPORT GameApi::TXA GameApi::TextureApi::prepare_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy)
{
  OpenglLowApi *ogl = g_low->ogl;
  Low_GLuint id;
  ogl->glGenTextures(1, &id); 

  if (vec.size()==0) { GameApi::TXA i; i.id=id; return i; }

  Low_GLsizei width = sx;
  Low_GLsizei height = sy;
  Low_GLsizei layercount = vec.size();

  ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, id);
#ifndef EMSCRIPTEN
  ogl->glTexStorage3D(Low_GL_TEXTURE_2D_ARRAY, 1, Low_GL_RGBA8, width, height, layercount);
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
      ogl->glTexSubImage3D(Low_GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);
#endif
    }
  ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);	
  ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D_ARRAY,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);

  GameApi::TXA i; 
  i.id=id; 
  return i;
}
EXPORT GameApi::TXID GameApi::TextureApi::prepare(TX tx)
{
  OpenglLowApi *ogl = g_low->ogl;
  TextureI *tex = find_texture(e, tx);
  TextureIBitmap bm(*tex);
  FlipColours flip(bm);
  BufferFromBitmap buf(flip);
  //  buf.Gen();
  InstallProgress(555,"texture",4);
#ifndef THREADS
  buf.Gen();
  ProgressBar(555,4,4,"texture");
#else
  buf.GenPrepare();

  int numthreads = 8;
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
  ProgressBar(555,i,4,"texture");
    }
#endif

  
  Low_GLuint id;
  ogl->glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, bm.SizeX(),bm.SizeY(), 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.Buffer().buffer);

  int ssx = bm.SizeX();
  int ssy = bm.SizeY();
  bool mipmaps = false;
  bool power_of_two = true;
      if (!(ssx==1 ||ssx==2||ssx==4||ssx==8||ssx==16||ssx==32||ssx==64||ssx==128||ssx==256||ssx==512||ssx==1024||ssx==2048||ssx==4096||ssx==8192||ssx==16384))
	power_of_two = false;
      if (!(ssy==1 ||ssy==2||ssy==4||ssy==8||ssy==16||ssy==32||ssy==64||ssy==128||ssy==256||ssy==512||ssy==1024||ssy==2048||ssy==4096||ssy==8192||ssy==16384))
	power_of_two = false;
      if (!power_of_two) { /*std::cout << "Warning: texture not power of two, mipmapping is disabled" << std::endl; */}
  
  if (mipmaps&&power_of_two)
    ogl->glGenerateMipmap(Low_GL_TEXTURE_2D);

  
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,mipmaps&&power_of_two?Low_GL_LINEAR_MIPMAP_LINEAR:Low_GL_LINEAR);      
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;
}

EXPORT void GameApi::TextureApi::use(TXID tx, int i)
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+i);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, tx.id);
}
EXPORT void GameApi::TextureApi::use_many(std::vector<TXID> tx, int i)
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glEnable(Low_GL_TEXTURE_2D);
  int s = tx.size();
  for(int i=0;i<s;i++)
    {
    ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
    ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
    ogl->glBindTexture(Low_GL_TEXTURE_2D, tx[i].id);
    }
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif

}
EXPORT void GameApi::TextureApi::unuse(TXID tx)
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glDisable(Low_GL_TEXTURE_2D);
}

EXPORT void GameApi::TextureApi::delete_texid(TXID tx)
{
  OpenglLowApi *ogl = g_low->ogl;
    Low_GLuint id;
    id = tx.id;
    ogl->glDeleteTextures(1, &id);
}
EXPORT void GameApi::TextureApi::delete_texid(std::vector<TXID> vec)
{

  int s = vec.size();
  for(int i=0;i<s;i++) {
    delete_texid(vec[i]);
  }
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

class GrabSplitter : public Splitter
{
public:
  GrabSplitter(Splitter *next, int sx, int sy) : next(next), sx(sx), sy(sy) {
    ref = BufferRef::NewBuffer(sx, sy);
    bitmap = new BitmapFromBuffer(ref);    
    firsttime = true; }
  void Init() {
    next->e = e; next->ev = ev;
    next->Init();
  }
  int Iter() { if (firsttime) {
      next->Iter();
      OpenglLowApi *ogl = g_low->ogl;
      ogl->glReadPixels(0,0,sx,sy,Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
      firsttime = false;
    }
    return 0;
  }
  void Destroy() { next->Destroy(); }
  Splitter *NextState(int code) { return this; }
  void EnvTransfer(Splitter *next) { next->e = e; next->ev = ev; }
  Bitmap<Color> *get_bitmap() const { return bitmap; }
private:
  Splitter *next;
  Bitmap<Color> *bitmap=0;
  int sx, sy;
  bool firsttime;
  BufferRef ref;
};

class CombineScreens : public Splitter
{
public:
  CombineScreens(Splitter *s1, Splitter *s2) : s1(s1), s2(s2) {  }
  void Init() { s1->e = e; s1->ev = ev; s2->e = e; s2->ev = ev; s1->Init(); s2->Init(); }
  int Iter() { s1->Iter(); return s2->Iter(); }
  void Destroy() { s1->Destroy(); s2->Destroy(); }
  Splitter *NextState(int code) { return this; }
  void EnvTransfer(Splitter *next) { next->e = e; next->ev = ev; }
private:
  Splitter *s1, *s2;
};

EXPORT GameApi::RUN GameApi::TextureApi::combine_screens(RUN r1, RUN r2)
{
  Splitter *s1 = find_splitter(e,r1);
  Splitter *s2 = find_splitter(e,r2);
  return add_splitter(e, new CombineScreens(s1,s2));
}

EXPORT GameApi::ARR GameApi::TextureApi::grab_screen(EveryApi &ev, RUN r)
{
  float screen_x = ev.mainloop_api.get_screen_sx();
  float screen_y = ev.mainloop_api.get_screen_sy();
  
  Splitter *old = find_splitter(e,r);
  GrabSplitter *run_new = new GrabSplitter(old,screen_x, screen_y);
  
  RUN run = add_splitter(e, run_new);

  BM bm = add_color_bitmap(e, run_new->get_bitmap());
  
  ArrayType *arr = new ArrayType;
  arr->type = 2;
  arr->vec.push_back(run.id);
  arr->vec.push_back(bm.id);
  return add_array(e, arr);
}



EXPORT GameApi::BM GameApi::TextureApi::to_bitmap(TXID tx)
{
  OpenglLowApi *ogl = g_low->ogl;
  //EnvImpl *env = ::EnvImpl::Environment(&e);

  TextureID *txid = find_txid(e, tx);
  int id = txid->texture();

  MainLoopEnv e2;
  e2.in_MV = Matrix::Identity();
  e2.in_T = Matrix::Identity();
  e2.in_N = Matrix::Identity();
  txid->render(e2);

  int id2=0;
  ogl->glGetIntegerv(Low_GL_TEXTURE_BINDING_2D, &id2);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  int width=256, height=256;
#ifndef EMSCRIPTEN
  ogl->glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_WIDTH, &width);
  ogl->glGetTexLevelParameteriv(Low_GL_TEXTURE_2D, 0, Low_GL_TEXTURE_HEIGHT, &height);
#endif
  BufferRef ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  //ogl->glReadBuffer( Low_GL_COLOR_ATTACHMENT0 );
  //ogl->glReadPixels(0,0,width,height, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
  ogl->glGetTexImage( Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
#endif
  ogl->glBindTexture(Low_GL_TEXTURE_2D, id2);

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
  OpenglLowApi *ogl = g_low->ogl;
      int sx = buf.width;
      int sy = buf.height;

      bool mipmaps = false;
      bool power_of_two = true;
      if (!(sx==1 ||sx==2||sx==4||sx==8||sx==16||sx==32||sx==64||sx==128||sx==256||sx==512||sx==1024||sx==2048||sx==4096||sx==8192||sx==16384))
	power_of_two = false;
      if (!(sy==1 ||sy==2||sy==4||sy==8||sy==16||sy==32||sy==64||sy==128||sy==256||sy==512||sy==1024||sy==2048||sy==4096||sy==8192||sy==16384))
	power_of_two = false;
      if (!power_of_two) { /*std::cout << "Warning: textures not power_of_two, mipmapping disabled" << std::endl;*/ }

  //std::cout << "bufferref_to_txid:" << buf.width << "x" << buf.height << ":" << buf.buffer << std::endl;
  Low_GLuint id;
  bool newbuffer = false;
  if (old.id==-1 || old.id==0) {
    ogl->glGenTextures(1, &id);
    //newbuffer = true;
  }
  else
    id=old.id;
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, id);
  //std::cout << "buf.width=" << buf.width << " " << "buf.height=" << buf.height << " " << (int)buf.buffer << std::endl;
  //if (newbuffer) {

    ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, buf.width,buf.height, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.buffer);
    // } else {
    //  g_low->ogl->glTexSubImage2D(Low_GL_TEXTURE_2D, 0, 0,0, buf.width,buf.height, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, buf.buffer);
    // }
    if (mipmaps&&power_of_two)
      ogl->glGenerateMipmap(Low_GL_TEXTURE_2D);

    
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, power_of_two?Low_GL_REPEAT:Low_GL_CLAMP_TO_EDGE);
  //g_low->ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;  
}


class KeyPrepare : public MainLoopItem
{
public:
  KeyPrepare(int key, MainLoopItem *next, MainLoopItem *keyed) : key(key), next(next), keyed(keyed) { prepare_done=false; do_prepare=false; }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    next->execute(e);
    if (do_prepare) {
      keyed->Prepare();
      prepare_done = true;
      do_prepare=false;
    }
    if (prepare_done) keyed->execute(e);

    

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int ch = e.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (e.type==0x300 && ch==key) {
      do_prepare=true;
    }
    next->handle_event(e);
    if (prepare_done) keyed->handle_event(e);
  }
private:
  int key;
  bool prepare_done;
  bool do_prepare;
  MainLoopItem *next, *keyed;
};
GameApi::ML GameApi::TextureApi::prepare_key(ML next, ML keyed, int key)
{
  MainLoopItem *next_ = find_main_loop(e,next);
  MainLoopItem *keyed_ = find_main_loop(e,keyed);
  return add_main_loop(e, new KeyPrepare(key, next_, keyed_));
}

class KeyPrepareAnim : public MainLoopItem
{
public:
  KeyPrepareAnim(int key, MainLoopItem *next, std::vector<MainLoopItem *> keyed, float time_delta, int num, std::vector<std::string> filenames) : key(key), next(next), keyed(keyed),time_delta(time_delta), num(num), filenames(filenames) { for(int i=0;i<num;i++) prepare_done.push_back(false); do_prepare=false;
    current=0;
    
  }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); current=0; }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); current=0; }
  virtual void execute(MainLoopEnv &e)
  {
    current_time = e.time;
    next->execute(e);
    if (do_prepare && e.time>=key_time+current*time_delta/100.0 && current<num) {
      std::cout << filenames[current] << std::endl;
      keyed[current]->Prepare();
      prepare_done[current] = true;
      current++;
      if (current>=num)
	do_prepare=false;
    }
    if (current>=1 && current-1<num)
    if (prepare_done[current-1]) keyed[current-1]->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int ch = e.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (e.type==0x300 && ch==key) {
      key_time = current_time;
      do_prepare=true;
    }
    next->handle_event(e);
    if (current>=1&&current-1<num)
    if (prepare_done[current-1]) keyed[current-1]->handle_event(e);
  }
private:
  int key;
  std::vector<bool> prepare_done;
  bool do_prepare;
  MainLoopItem *next;
  std::vector<MainLoopItem*> keyed;
  int current=0;
  float key_time=0.0;
  float time_delta=1.0;
  int num;
  float current_time=0.0;
  std::vector<std::string> filenames;
};

GameApi::ML GameApi::TextureApi::prepare_key_anim(ML next, std::vector<ML> keyed, int key, float time_delta, std::vector<std::string> filenames)
{
  MainLoopItem *next_ = find_main_loop(e,next);
  int s = keyed.size();
  std::vector<MainLoopItem*> vec;
  for(int i=0;i<s;i++) {
    MainLoopItem *keyed_ = find_main_loop(e,keyed[i]);
    vec.push_back(keyed_);
  }
  return add_main_loop(e, new KeyPrepareAnim(key, next_, vec,time_delta,s, filenames));
}


class Prepare_grabscreen : public Bitmap<Color>
{
public:
  Prepare_grabscreen(GameApi::EveryApi &ev, int sx, int sy) : ev(ev), m_sx(sx), m_sy(sy) {
    if (m_sx==-1) m_sx=ev.mainloop_api.get_screen_sx();
    if (m_sy==-1) m_sy=ev.mainloop_api.get_screen_sy();
    //std::cout << "sx=" << m_sx << "sy=" << m_sy << std::endl;
    ref=BufferRef::NewBuffer(m_sx,m_sy);
  }
  virtual int SizeX() const { return m_sx; }
  virtual int SizeY() const { return m_sy; }
  virtual Color Map(int x, int y) const
  {
    unsigned int col = ref.buffer[x+y*ref.ydelta];
    unsigned int b = col &0xff0000;
    unsigned int g = col &0xff00;
    unsigned int r = col &0xff;
    unsigned int a = col &0xff000000;
    b >>= 16;
    g >>= 8;
    a >>= 24;

    // THIS CONVERTS TRANSPARENT PIXELS INTO BLACK
    float mult = a/255.0;
    b*=mult;
    g*=mult;
    r*=mult;
    a=255;
    
    //r >>= 8;
    r <<= 16;
    g <<= 8;
    a <<= 24;
    unsigned int col2 = a + r+g+b;
    return col2;
  }
  virtual void Prepare()
  {
    OpenglLowApi *ogl = g_low->ogl;
    std::cout << "Grabbing.." << std::endl;
    ogl->glReadBuffer( Low_GL_COLOR_ATTACHMENT0 );
    GameApi::SH shader;
    shader.id = 0;
    ev.shader_api.unuse(shader);
    ev.mainloop_api.swapbuffers();
    while(ogl->glCheckFramebufferStatus(Low_GL_FRAMEBUFFER)!=Low_GL_FRAMEBUFFER_COMPLETE) { }
    //int err = ogl->glGetError(); std::cout << "err1=" << err << std::endl;
    ogl->glReadPixels(0,0,m_sx,m_sy,Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, (void*)ref.buffer);
    //err = ogl->glGetError(); std::cout << "err2=" << err << std::endl;
    ogl->glFinish();
    //err = ogl->glGetError(); std::cout << "err3=" << err << std::endl;
  }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }

private:
  GameApi::EveryApi &ev;
  int m_sx,m_sy;
  BufferRef ref;
};
GameApi::BM GameApi::TextureApi::grab_screen_bitmap(EveryApi &ev)
{
  Bitmap<Color> *b = new Prepare_grabscreen(ev,-1,-1);
  BitmapColorHandle *handle3 = new BitmapColorHandle;
  handle3->bm = b;
  BM bm = add_bitmap(e, handle3);
  return bm;
}

GameApi::ML GameApi::TextureApi::save_screenshot_via_key(EveryApi &ev, GameApi::ML ml3, int key, std::string filename)
{
  GameApi::BM bm = grab_screen_bitmap(ev);
  GameApi::BM bm2 = ev.bitmap_api.flip_y(bm);
  GameApi::ML ml2 = ev.bitmap_api.save_png_ml(ev, bm2, filename);

  GameApi::ML ml = prepare_key(ml3, ml2, key);
  return ml;
}

void send_grab_to_server(std::vector<unsigned char> data, int id, int num);

void SaveImage(std::vector<unsigned char> &res, BufferRef ref);

class GrabToServer : public MainLoopItem
{
public:
  GrabToServer(Bitmap<Color> *bm, int id, int num) : bm(bm),id(id),num(num) { }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() { Prepare(); }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  void Prepare()
  {
    //bm->Prepare();
    std::vector<unsigned char> res;
    BufferFromBitmap buf(*bm);
    buf.Gen();
    BufferRef ref = buf.Buffer();
    SaveImage(res,ref);
    send_grab_to_server(res, id, num);
  }
private:
  MainLoopItem *next;
  Bitmap<Color> *bm;
  int id;
  int num;
};

GameApi::ML GameApi::TextureApi::grab_to_server(BM bm, int id, int num)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  return add_main_loop(e, new GrabToServer(b2, id,num));
}

extern int gameapi_id;

GameApi::ML GameApi::TextureApi::send_screenshots_via_key_to_server(EveryApi &ev, GameApi::ML ml3, int key, float time_delta, int num)
{
  int s = num;
  std::vector<std::string> vec2;
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
  GameApi::BM bm = grab_screen_bitmap(ev);
  GameApi::BM bm2 = ev.bitmap_api.scale_bitmap(ev, bm, 280, 208);
  GameApi::BM bm3 = ev.bitmap_api.flip_y(bm2);
  std::stringstream ss;
  ss<< "screenshot" << std::setfill('0') << std::setw(3) << i+1 << ".png";
  //GameApi::ML ml2 = ev.bitmap_api.save_png_ml(ev, bm3, ss.str());
  GameApi::ML ml2 = ev.texture_api.grab_to_server(bm3,gameapi_id,i);
  vec.push_back(ml2);
  vec2.push_back( ss.str() );
  }
  GameApi::ML ml = prepare_key_anim(ml3, vec, key, time_delta, vec2);
  return ml;
}

GameApi::ML GameApi::TextureApi::save_screenshots_via_key(EveryApi &ev, GameApi::ML ml3, int key, float time_delta, int num)
{
  int s = num;
  std::vector<std::string> vec2;
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
  GameApi::BM bm = grab_screen_bitmap(ev);
  GameApi::BM bm2 = ev.bitmap_api.scale_bitmap(ev, bm, 280, 208);
  GameApi::BM bm3 = ev.bitmap_api.flip_y(bm2);
  std::stringstream ss;
  ss<< "screenshot" << std::setfill('0') << std::setw(3) << i+1 << ".png";
  GameApi::ML ml2 = ev.bitmap_api.save_png_ml(ev, bm3, ss.str());
  vec.push_back(ml2);
  vec2.push_back(ss.str());
  }
  GameApi::ML ml = prepare_key_anim(ml3, vec, key, time_delta, vec2);
  return ml;
}
