
struct FontPriv
{
};

struct BitmapPriv
{
};
struct AnimPriv
{
};


EXPORT GameApi::FontApi::FontApi(Env &e) : e(e) 
{
  priv = new FontPriv;
}
EXPORT GameApi::FontApi::~FontApi()
{
  delete (FontPriv*)priv;
}

EXPORT GameApi::BitmapApi::BitmapApi(Env &e) : e(e) 
{
  priv = (void*)new BitmapPriv;
}
EXPORT GameApi::BitmapApi::~BitmapApi()
{
  delete (BitmapPriv*)priv;
}
EXPORT GameApi::AnimApi::AnimApi(Env &e) : e(e) 
{
  priv = (void*)new AnimPriv;
}
EXPORT GameApi::AnimApi::~AnimApi()
{
  delete (AnimPriv*)priv;
}

EXPORT GameApi::SpriteApi::~SpriteApi()
{
  delete (SpritePriv*)priv;
}
EXPORT GameApi::VolumeApi::VolumeApi(Env &e) : e(e) { }
EXPORT GameApi::VolumeApi::~VolumeApi() { }

EXPORT GameApi::MainLoopApi::MainLoopApi(Env &e) : frame(0.0), time(0.0), e(e)  
{
  priv = (void*)new MainLoopPriv;
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame = 0;
  p->time = 0;
  p->count = 0;
}

SpritePriv::~SpritePriv()
{
  int s = arrays.size();
  for(int i=0;i<s;i++)
    {
      Sprite** ptr = arrays[i];
      delete [] ptr;
    }
  std::map<int, Sprite*>::iterator it2 = sprites.begin();
  for(;it2!=sprites.end();it2++)
    {
      Sprite *sp = (*it2).second;
      delete sp;
    }
  {
  std::map<int, Sprite*>::iterator it2 = sprites2.begin();
  for(;it2!=sprites2.end();it2++)
    {
      Sprite *sp = (*it2).second;
      delete sp;
    }
  }
}

EXPORT GameApi::PointApi::PointApi(Env &e) : e(e) { }
EXPORT GameApi::VectorApi::VectorApi(Env &e) : e(e) { }
EXPORT GameApi::MatrixApi::MatrixApi(Env &e) : e(e) { }
EXPORT GameApi::SpriteApi::SpriteApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new SpritePriv;
}
EXPORT GameApi::PolygonApi::PolygonApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new PolyPriv;  
} 

int score=0;
int hidden_score=0;
bool g_transparent;
int g_shows_hundred;
int g_event_screen_x=-1;
int g_event_screen_y=-1;
std::string gameapi_homepageurl;
std::vector<int> g_hide_container;
int no_draw_count;

void check_vr_compositor_init() { }
std::string gameapi_seamless_url;
int async_pending_count_previous=0;

int async_is_done=0;
class Splitter;
Splitter *splitter_current =0;

struct FunctionPriv
{
};

EXPORT GameApi::ContinuousBitmapApi::ContinuousBitmapApi(Env &e) : e(e) { }
EXPORT GameApi::EventApi::EventApi(Env &e) : e(e) 
{
  priv = (void*)new EventPriv;
}
EXPORT GameApi::ShaderApi::ShaderApi(Env &e) : e(e)
{
  priv = (void*)new ShaderPriv2;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = 0;
  p->seq = 0;
  p->count=0;
}
EXPORT GameApi::GridApi::GridApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new GridPriv;
}

EXPORT GameApi::StateChangeApi::StateChangeApi(GameApi::Env &e, GameApi::ShaderApi &api)
: e(e),api(api) { }

GameApi::SurfaceApi::SurfaceApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new SurfPriv;
}

class CombineMaterials : public MaterialForward
{
public:
  CombineMaterials(GameApi::EveryApi &ev, Material *mat1, Material *mat2a) : ev(ev), mat1(mat1), mat2a(mat2a) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    int ml1 = mat1->mat(p.id);
    int ml2 = mat2a->mat(p.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{m1,I12});
    return I13;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    int ml1 = mat1->mat_inst(p.id,pts.id);
    int ml2 = mat2a->mat_inst(p.id,pts.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{m1,I12});
    return I13;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {

    int ml1 = mat1->mat_inst2(p.id,pta.id);
    int ml2 = mat2a->mat_inst2(p.id,pta.id);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{m1,I12});
    return I13;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    int ml1 = mat1->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    int ml2 = mat2a->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    GameApi::ML m1 = { ml1 };
    GameApi::ML m2 = { ml2 };
    GameApi::ML I11=ev.mainloop_api.depthfunc(m2,1);
    GameApi::ML I12=ev.mainloop_api.blendfunc(I11,2,3);
    GameApi::ML I13=ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{m1,I12});
    return I13;

  }
private:
  GameApi::EveryApi &ev;
  Material *mat1, *mat2a;
};

LoadObjModelFaceCollection::LoadObjModelFaceCollection(std::vector<unsigned char> file_data, int objcount, std::vector<std::string> material_names) : file_data(file_data), obj_num(objcount), material_names_external(material_names) 
  {
    firsttime = true;
  }

GameApi::P GameApi::PolygonApi::or_array_1(P *p1, int size)
{
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<size;i++)
    {
      FaceCollection *pp1 = find_facecoll(e, p1[i]);
      if (!pp1) { std::cout << "or_array, bad FaceCollection" << std::endl;  continue; }
      vec->push_back(pp1);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>(vec->begin(), vec->end());
  coll->update_faces_cache();
  return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::empty()
{
  return add_polygon(e,new EmptyBoxableFaceCollection, 1);
}

class SpriteTexture : public MeshTextures
{
public:
  SpriteTexture(const Sprite &s) : s(s) 
  { 
    int ss = NumTextures();
    ref = new BufferRef[ss];
    for(int i=0;i<ss;i++)
      {
	ref[i] = BufferRef::NewBuffer(1,1); 
      }
    currentnum = -1; 
  }
  ~SpriteTexture() {
    for(int i=0;i<NumTextures();i++)
      {
	delete [] ref[i].buffer;
      }
    delete [] ref; }
  int NumTextures() const { return s.NumFrames(); }
  void GenTexture(int num)
  {
    BufferRef::FreeBuffer(ref[num]);
    int sx = s.XSize(num);
    int sy = s.YSize(num);
    ref[num] = BufferRef::NewBuffer(sx, sy);

    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  ref[num].buffer[x+y*ref[num].ydelta] = s.Pixel(num, x, y).Pixel();
	}
  }
  BufferRef TextureBuf(int num) const 
  {
    return ref[num]; 
  }
private:
  const Sprite &s;
  mutable BufferRef *ref;
  mutable int currentnum;
};

void TexturePrepare(const Sprite &s, ArrayRender &rend)
{
  int frames = s.NumFrames();
  if (!frames) { std::cout << "TexturePrepare !frames" << std::endl; return; }
  rend.AllocTexture(frames);
  rend.Alloc(2,6, frames,1,1,frames);
  SpriteTexture ss(s);
  for(int i=0;i<frames;i++)
    {
      rend.UpdateTexture(ss, i);
    }  
  rend.Prepare();

}

void TextureEnable(ArrayRender &rend, int frame, bool enable)
{
  if (enable)
    rend.EnableTexture(frame);
  else
    rend.DisableTexture();
}


GameApi::MT GameApi::MaterialsApi::combine_materials(EveryApi &ev, MT mat1, MT mat2)
{
  Material *m1 = find_material(e,mat1);
  Material *m2 = find_material(e,mat2);
  return add_material(e, new CombineMaterials(ev,m1,m2));
}



EXPORT GameApi::TextureApi::TextureApi(GameApi::Env &e) : e(e) { count=0; }
EXPORT GameApi::ColorApi::ColorApi(Env &e) : e(e) { }
EXPORT GameApi::VoxelApi::VoxelApi(Env &e) : e(e) { }

EXPORT GameApi::PlaneApi::PlaneApi(Env &e) : e(e) { }

EXPORT GameApi::FunctionApi::FunctionApi(Env &e) : e(e)
{
  priv = (void*)new FunctionPriv;
}
EXPORT GameApi::FunctionApi::~FunctionApi()
{
  delete (FunctionPriv*)priv;
}
EXPORT GameApi::BoolBitmapApi::BoolBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::BoolBitmapApi::~BoolBitmapApi() { }

EXPORT GameApi::FloatBitmapApi::FloatBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::FloatBitmapApi::~FloatBitmapApi() { }

#if 0
void blocker_iter(void *arg)
{
  Envi_2 *env = (Envi_2*)arg;
  //std::cout << "async: " << async_pending_count << std::endl;
  if (async_pending_count > 0 && !async_is_done) { env->logo_shown = true; }
  if (env->logo_shown)
    {
      bool b=false;
      if (gameapi_seamless_url=="")
	b = env->ev->mainloop_api.logo_iter();
      else
	b = env->ev->mainloop_api.seamless_iter();
      if (b && async_pending_count==0) { env->logo_shown = false;
	env->ev->mainloop_api.reset_time();
	env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
      }
      return;
    }
  async_is_done = true;

    if (env->firsttime) {
      MainLoopItem *item = find_main_loop(env->ev->get_env(),env->mainloop);
      item->Prepare();
      env->firsttime = false;
    }


    env->ev->mainloop_api.clear_3d(0xff000000);

    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { std::cout << "ESC pressed!" << std::endl; env->exit = true; }
#endif

	GameApi::InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e);

      }
    GameApi::InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);

    GameApi::M mat = env->ev->matrix_api.identity();
        if (env->screen_width<600) {
	    mat = env->ev->matrix_api.scale(-1.0,-1.0,1.0);
    }

#ifndef NO_MV
	env->ev->shader_api.use(env->color_sh);
	env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));

	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
#endif
	env->ev->shader_api.use(env->color_sh);

	GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
	GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
	GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

	if (env->fpscounter)
	  env->ev->mainloop_api.fpscounter();
	if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
	  {
	    //std::cout << "Timeout, existing..." << std::endl;
	    //env->exit = true;
	  }

    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    //    g_low->ogl->glGetError();
}
#endif

void splitter_iter2(void *arg)
{
  if (!arg) { std::cout << "FAIL: Splitter_iter2 NULL" << std::endl; return; }
  Splitter *blk2 = (Splitter*)arg;
  int blocker_exit_code = blk2->Iter();
  if (blocker_exit_code!=-1) 
    {
      Splitter *next = blk2->NextState(blocker_exit_code);
      if (next) {
	blk2->EnvTransfer(next);
      }
      blk2->Destroy();
      if (next) {
	next->Init();
      }
#ifdef EMSCRIPTEN
      // TODO, VR ISSUES
      emscripten_set_main_loop_arg(splitter_iter2, (void*)next, 0,1);
#else
      splitter_current = next;
#endif
    }
}
std::vector<std::pair<std::string, int> > prepare_cache_data;

int find_data(std::string data) {
  int s = prepare_cache_data.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string,int> p = prepare_cache_data[i];
      if (p.first==data) return p.second;
    }
  return -1;
}

unsigned int swap_color(unsigned int c)
{
  unsigned int ca = c & 0xff000000;
  unsigned int cr = c & 0x00ff0000;
  unsigned int cg = c & 0x0000ff00;
  unsigned int cb = c & 0x000000ff;
  cr>>=16;
  //cg>>=8;
  cb<<=16;
  //cg<<=8;
  return ca+cr+cg+cb;
}

std::string cache_id(std::string filename, int obj_count)
{
  std::stringstream ss;
  ss << filename << obj_count;
  return ss.str();
}
int filesize(std::string filename)
{
  std::ifstream in(filename, std::ifstream::ate|std::ifstream::binary);
  return in.tellg();
}

bool invalidate(CacheItem *item, std::string filename, int obj_count)
{
  int size = filesize(filename);
  return item->filesize != size || item->obj_count != obj_count;
}

Sprite *sprite_from_handle2(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose)
{
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      int s = ahandle->vec.size();
      Sprite **arr = new Sprite*[s];
      env.arrays.push_back(arr);
      for(int i=0;i<s;i++)
	{
	  BitmapHandle *h = ahandle->vec[i];
	  Sprite *sk = sprite_from_handle(e, env, h);
	  arr[i] = sk;
	}
      ArraySprite *ss = new ArraySprite(arr, s);
      ss->update_cache();
      env.sprites2[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      ArrayRender *rend = env2->renders2[handle->id];
      if (rend)
	{
	  delete rend;
	}
      env2->renders2[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (chandle)
    {
      GameApi::BM bm = { handle->id };
      SpritePosImpl *pos = find_sprite_pos(e, bm);
      Point2d p;
      if (!pos)
	{
	  p.x = 0.0;
	  p.y = 0.0;
	}
      else
	{
	  p.x = pos->x;
	  p.y = pos->y;
	}
      Sprite *ss = new BitmapSprite(*chandle->bm, p);
      env.sprites2[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      ArrayRender *rend = env2->renders2[handle->id];
      if (rend)
	{
	  delete rend;
	}
      env2->renders2[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  std::cout << "Unknown bitmap type in sprite_from_handle" << std::endl;
  BitmapIntHandle *ihandle = dynamic_cast<BitmapIntHandle*>(handle);
  if (ihandle)
    {
      return 0;
    }
  return 0;
}

void PrepareSpriteToVA(const Sprite &s, VertexArraySet &vas)
{
  SpriteMesh spmesh(s);
  SpriteTexCoords texcoords(s);
  
  MeshToTriangles trimesh(spmesh);
  MeshTexCoordsToTriangles tricoords(texcoords, true);
  
  MeshFaceCollection coll(trimesh, tricoords ,0);
  FaceCollectionVertexArray2 va(coll, vas);
  va.copy(0,coll.NumFaces());
}

Sprite *sprite_from_handle(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose)
{
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      int s = ahandle->vec.size();
      Sprite **arr = new Sprite*[s];
      env.arrays.push_back(arr);
      for(int i=0;i<s;i++)
	{
	  BitmapHandle *h = ahandle->vec[i];
	  Sprite *sk = sprite_from_handle(e, env, h);
	  arr[i] = sk;
	}
      ArraySprite *ss = new ArraySprite(arr, s);
      ss->update_cache();
      env.sprites[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (chandle)
    {
      GameApi::BM bm = { handle->id };
      SpritePosImpl *pos = find_sprite_pos(e, bm);
      Point2d p;
      if (!pos)
	{
	  p.x = 0.0;
	  p.y = 0.0;
	}
      else
	{
	  p.x = pos->x;
	  p.y = pos->y;
	}
      Sprite *ss = new BitmapSprite(*chandle->bm, p);
      env.sprites[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  std::cout << "Unknown bitmap type in sprite_from_handle" << std::endl;
  BitmapIntHandle *ihandle = dynamic_cast<BitmapIntHandle*>(handle);
  if (ihandle)
    {
      return 0;
    }
  return 0;
}
void LoadObjModelFaceCollection::Prepare()
  {
    if (firsttime) {
      //std::cout << "Loading: " << filename << " " << obj_num << std::endl;
      check_invalidate();
      Load();
      firsttime = false;
    }
  }
void LoadObjModelFaceCollection::check_invalidate() const { const_cast<LoadObjModelFaceCollection*>(this)->check_invalidate2(); }
void LoadObjModelFaceCollection::check_invalidate2()
  {
    static int filesize_store = -1;
    static int objcount = -1;
    int val = file_data.size(); //filesize(filename);
    if (filesize_store != val) { filesize_store = val; }
    if (objcount != obj_num) { objcount = obj_num; }
  }

  void LoadObjModelFaceCollection::Load() const { const_cast<LoadObjModelFaceCollection*>(this)->Load2(); }
  void LoadObjModelFaceCollection::Load2() {
    //std::ifstream file(filename.c_str());
    std::string s(file_data.begin(), file_data.end());
    std::stringstream file(s);
    std::string line;
    int obj_count = 0;
    int vertex_count = 0;
    int vertex_count2 = 0;
    int face_count=0;
    int normal_count =0;
    int normal_count2 = 0;
    int color_count = 0;
    int color_count2 = 0;
    int tex_count = 0;
    int tex_count2 = 0;

    int obj_base_v = 0;
    int obj_base_t = 0;
    int obj_base_n = 0;
    //int obj_base_c = 0;
    int mtl_material = -1;
    std::string word;
    std::stringstream ss;
    while(std::getline(file, line))
      {
	ss.str(line);
	ss.clear();
	//ss << line;
	ss>>word;
	if (word == "usemtl")
	  {
	    std::string material_name;
	    ss >> material_name;
	    material_names_internal.push_back(material_name);
	    mtl_material++;
	  }
	if (word == "o")
	  {
	    //std::cout << "o" << std::flush;
	    obj_count++;
	    obj_base_v = vertex_count2;
	    obj_base_n = normal_count2;
	    obj_base_t = tex_count2;
	    //obj_base_c = color_count2;
	  }
	if (word == "v" && obj_count==obj_num)
	  {
	    //std::cout << "v" << std::flush;
	    vertex_count++;
	    if (vertex_count % 1000==0) 
	      std::cout << "v" << std::flush;
	    float x,y,z;
	    ss >> x >> y >> z;
	    float cr = 1.0, cg = 1.0, cb = 1.0;
	    bool b1 = false;
	    if (ss >> cr) b1=true;
	    bool b2 = false;
	    if (ss >> cg) b2=true;
	    bool b3 = false;
	    if (ss >> cb) b3=true;
	    //std::cout << "Vertex:" << vertex_data.size() << " " << x << " " << y << " " << z << std::endl;
	    Point p(x,y,z);
	    vertex_data.push_back(p);
	    if (b1&&b2&&b3) {
	    ::Color vc(cr,cg,cb,1.0f);
	    color_data.push_back(vc.clamp().Pixel());
	    }
	  } else if (word =="v") { vertex_count2++; }
	if (word == "vt" && obj_count==obj_num)
	  {	
	    //std::cout << "vt" << std::flush;

	    tex_count++;
	    if (tex_count %1000 == 0)
	      std::cout << "t" << std::flush;
	    //std::cout << "Texture:" << texcoord_data.size() << std::endl;
	    float tx,ty,tz;
	    ss >> tx >> ty >> tz;
	    Point2d p = { tx, ty };
	    texcoord_data.push_back(p);
	    texcoord3_data.push_back(0.0); // TODO, oli 0.0
	  } else if (word=="vt") { tex_count2++; }
	if (word == "vn" && obj_count==obj_num)
	  {
	    //std::cout << "vn" << std::flush;
	    normal_count++;
	    if (normal_count %1000 == 0)
	      std::cout << "n" << std::flush;

	    //std::cout << "Normal:" << normal_data.size() << std::endl;

	    float nx, ny, nz;
	    ss >> nx >> ny >> nz;
	    Vector v(nx,ny,nz);
	    normal_data.push_back(v);
	  } else if (word=="vn") { normal_count2++; }
	if (word == "vc" && obj_count==obj_num)
	  {
	    //std::cout << "vc" << std::flush;
	    color_count++;
	    if (color_count %1000 == 0)
	      std::cout << "c" << std::flush;

	    //std::cout << "Normal:" << normal_data.size() << std::endl;

	    int nr, ng, nb, na;
	    ss >> nr >> ng >> nb >> na;
	    ::Color vc(nr,ng,nb,na);
	    color_data.push_back(vc.clamp().Pixel());
	  } else if (word=="vc") { color_count2++; }
	if (word == "f" && obj_count==obj_num)
	  {
	    //std::cout << "f" << std::flush;
	    face_count++;
	    if (face_count % 1000 == 0)
	      std::cout << "f" << std::flush;

	    int v_index, t_index, n_index;
	    int count = 0;
	    char c;
	    bool t_bool = texcoord_data.size()!=0;
	    bool n_bool = normal_data.size()!=0;
	    
	    //std::cout << "Face:" << face_counts.size() << std::endl;
	    if (t_bool && n_bool) {
	      while(ss>>v_index>> c >>t_index>> c >>n_index)
		{
		  //std::cout << "1" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    if (t_index<0) t_index = texcoord_data.size()+t_index+1;
	    if (n_index<0) n_index = normal_data.size()+n_index+1;
	    v_index -= obj_base_v;
	    t_index -= obj_base_t;
	    n_index -= obj_base_n;
	    vertex_index.push_back(v_index -1);
	    texture_index.push_back(t_index-1);
	    if (int(texcoord3_data.size())>t_index-1)
	      texcoord3_data[t_index-1]=mtl_material+0.3;
	    normal_index.push_back(n_index-1);
		  //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		  count++;
		}
	    }
	    if (t_bool && !n_bool)
	      {
		while(ss>>v_index>> c >>t_index)
		  {
		    //std::cout << "2" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    if (t_index<0) t_index = texcoord_data.size()+t_index+1;
	    //if (n_index<0) n_index = -n_index;
	    v_index -= obj_base_v;
	    t_index -= obj_base_t;
	    vertex_index.push_back(v_index-1);
	    texture_index.push_back(t_index-1);
	    if (int(texcoord3_data.size())>t_index-1)
	      texcoord3_data[t_index-1]=mtl_material+0.3;
	    normal_index.push_back(0);
		    //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		    count++;
		  }
	      }
	    if (!t_bool && n_bool)
	      {

		while(ss>>v_index>> c>> c >>n_index)
		  {
		    //std::cout << "3" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    //if (t_index<0) t_index = -t_index;
	    if (n_index<0) n_index = normal_data.size()+n_index+1;
	    v_index -= obj_base_v;
	    n_index -= obj_base_n;

	    vertex_index.push_back(v_index-1);
	    texture_index.push_back(0);
	    if (int(texcoord3_data.size())>0)
	      texcoord3_data[0]=mtl_material+0.3;
	    normal_index.push_back(n_index-1);
		    //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		    count++;
		  }
	      }
	    if (!t_bool && !n_bool) {
	      while(ss>>v_index)
		{
		  //std::cout << "4" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;

	    v_index -= obj_base_v;

	    //if (t_index<0) t_index = -t_index;
	    //if (n_index<0) n_index = -n_index;

	    vertex_index.push_back(v_index-1);
		  texture_index.push_back(0);
		  if (int(texcoord3_data.size())>0)
		    texcoord3_data[0]=mtl_material+0.3;

		  normal_index.push_back(0);
		  //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		  count++;
		}
	    }

	    face_counts.push_back(count);
	  }
      }
    if (texcoord_data.size()==0) 
      {
	Point2d p;
	p.x = 0.0;
	p.y = 0.0;
	texcoord_data.push_back(p);
      }
    if (texcoord3_data.size()==0) 
      {
	texcoord3_data.push_back(0.0);
      }
    if (normal_data.size()==0)
      {
	Point p(0.0,0.0,0.0);
	normal_data.push_back(p);
      }
    if (vertex_data.size()==0)
      {
	Point p(0.0,0.0,0.0);
	vertex_data.push_back(p);
	vertex_data.push_back(p);
	vertex_data.push_back(p);
      }
    if (color_data.size()==0)
      {
	color_data.push_back(0xffffffff);
      }
  }
  int LoadObjModelFaceCollection::NumFaces() const {
    return face_counts.size()<=0 ? 1 : face_counts.size(); }
  int LoadObjModelFaceCollection::NumPoints(int face) const {
    return face_counts.size()<=0 ? 3 : face_counts[face]; }
  Point LoadObjModelFaceCollection::FacePoint(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)vertex_index.size())
      {
	int index = vertex_index[c];
	if (index>=0 && index<(int)vertex_data.size())
	  return vertex_data[index];
      }
    Point p(0.0,0.0,0.0);
    return p;
  }
  Vector LoadObjModelFaceCollection::PointNormal(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)normal_index.size())
      {
	int index = normal_index[c];
	if (index>=0 && index<(int)normal_data.size())
	  {
	    return normal_data[index];
	  }
      }
    Vector v(0.0,0.0,0.0);
    return v;
  }
  float LoadObjModelFaceCollection::Attrib(int face, int point, int id) const {
    return 0.0; 
  }
  int LoadObjModelFaceCollection::AttribI(int face, int point, int id) const {
    return 0; }
  unsigned int LoadObjModelFaceCollection::Color(int face, int point) const { 
    int c = Count(face,point);
    if (c>=0 && c<(int)vertex_index.size())
      {
	int index = vertex_index[c];
	if (index>=0 && index<(int)color_data.size())
	  return color_data[index];
      }
    unsigned int p = 0xffffffff;
    //Point p(0.0,0.0,0.0);
    return p;
  }
  float LoadObjModelFaceCollection::TexCoord3(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)texture_index.size())
      {
	int index = texture_index[c];
	if (index>=0 && index<(int)texcoord3_data.size())
	  {
	    float i = texcoord3_data[index];
	    int ii = int(i);
	    std::string mat_name = "";
	    int s2 = material_names_internal.size();
	    if (ii>=0 && ii<s2) mat_name = material_names_internal[ii];
	    int s3 = material_names_external.size();
	    for(int i3=0;i3<s3;i3++)
	      {
		//std::cout << "Compare: " << material_names_external[i3] << "==" << mat_name << std::endl;
		if (material_names_external[i3]==mat_name) return float(i3)+0.5;
	      }
	    return i;
	  }
      }
    return 0.0;
  }
  Point2d LoadObjModelFaceCollection::TexCoord(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)texture_index.size())
      {
	int index = texture_index[c];
	if (index>=0 && index<(int)texcoord_data.size())
	  {
	    return texcoord_data[index];
	  }
      }
    Point2d p = { 0.0, 0.0 };
    return p;
  }

  int LoadObjModelFaceCollection::Count(int face, int point) const {
    int s = face_counts.size();
    if (counts.size()>0) { return counts[face]+point; }
    //if (s>0) { return face_counts[0]*face+point; }
    int c = 0;
    counts.push_back(c);
    for(int i=0;i<s;i++)
      {
	c+=face_counts[i];
	counts.push_back(c);
      }
    return counts[face]+point;
  }

Point2d CubeElem::TexCoord(int face, int point) const
{
  {
    Point2d p;
    if (point==0) {
      p.x = 0;
      p.y = 0; }
    if (point==1) {
      p.x = 1;
      p.y = 0;
    }
    if (point==2) {
      p.x = 1;
      p.y = 1;
    }
    if (point==3) {
      p.x = 0;
      p.y = 1;
    }
    return p;
  }

}
Point CubeElem::FacePoint(int face, int point) const
{
  if (face>5) return Point(0.0,0.0,0.0);
  static int face1[] = { 1, 0, 5, 4 };
  static int face2[] = { 0, 7, 6, 5 };
  static int face3[] = { 7, 2, 3, 6 };
  static int face4[] = { 2, 1, 4, 3 };
  static int face5[] = { 0, 1, 2, 7 };
  static int face6[] = { 5, 6, 3, 4 };
  static int *faces[] = { &face1[0], &face2[0], &face3[0], &face4[0], &face5[0], &face6[0] };
  int *array = faces[face]; // face
  int p = array[point];
  //float x=0.0,y=0.0,z=0.0;
  switch(p)
    {
    case 0: return p111; //x=-1.0; y=-1.0; z=-1.0;  break;
    case 1: return p211; // x=1.0; y=-1.0; z=-1.0; break;
    case 2: return p212; //x=1.0; y=-1.0; z=1.0; break;
    case 3: return p222; //x=1.0; y=1.0; z=1.0; break;
    case 4: return p221; //x=1.0; y=1.0; z=-1.0; break;
    case 5: return p121; //x=-1.0; y=1.0; z=-1.0; break;
    case 6: return p122; //x=-1.0; y=1.0; z=1.0; break;
    case 7: return p112; //x=-1.0; y=-1.0; z=1.0; break;
    };
  Point pp = Point(0.0,0.0,0.0);
  return pp;
}

PolyPriv::~PolyPriv()
{
  std::map<int,ArrayRender*>::iterator i = rend.begin();
  for(;i!=rend.end();i++)
    {
      ArrayRender *rend = (*i).second;
      delete rend;
    }
  std::map<int,StateBitmaps*>::iterator i2 = states.begin();
  for(;i2!=states.end();i2++)
    {
      StateBitmaps *st = (*i2).second;
      delete st;
    }
}

SphereElem::SphereElem(Point center, float radius, int numfaces1, int numfaces2) : numfaces(numfaces1*2), numfaces2(numfaces2), center(center), radius(radius) { }

void RenderVoxel(VolumeObject &orv, int size, float wholesize, HandleValue<std::pair<Vector, unsigned int> > &hv)
{
}

SphereVolume::SphereVolume(Point origo_, float radius_)
  : radius(radius_), radius_x_radius(radius_*radius_), origo(origo_)
{
}
bool SphereVolume::Inside(Point v) const
{
  // x^2+y^2+z^2 < d^2
  v -= Vector(origo);
  return v.x*v.x+v.y*v.y+v.z*v.z < radius_x_radius;
}
Vector SphereVolume::Normal(Point v) const
{
  return v-origo;
}

Point SphereVolume::Part(const SphericalPoint &part)
{
  SphericalPoint p = part;
  p.r *= radius;
  return p.ToPoint() + Vector(origo);
}

float SphereVolume::Line(Point p, Point p2) const
{
  Vector origo1 = Vector(origo);
  float x1 = p.x;
  float x2 = p2.x;
  float y1 = p.y;
  float y2 = p2.y;
  float z1 = p.z;
  float z2 = p2.z;
  float x3 = origo1.dx;
  float y3 = origo1.dy;
  float z3 = origo1.dz;
  float a = (x2 - x1)*(x2-x1) + (y2 - y1)*(y2-y1) + (z2 - z1)*(z2-z1);
  float b = 2*( (x2 - x1)*(x1 - x3) + (y2 - y1)*(y1 - y3) + (z2 - z1)*(z1 - z3) );
  float c = x3*x3 + y3*y3 + z3*z3 + x1*x1 + y1*y1 + z1*z1 - 2.0*(x3*x1 + y3*y1 + z3*z1) - radius_x_radius; 
  if (b*b < 4*a*c) return 0.0;
  float sqrts = sqrt(b*b - 4*a*c);
  float res = -b + sqrts;
  float res2 = -b - sqrts;
  float res3 = res2 < res ? res2 : res;
  return res3;
}
int SphereElem::NumFaces() const { return numfaces*numfaces2; }
Point SphereElem::FacePoint(int face, int point) const
{
    int face1 = face / numfaces;
    int face2 = face - (face1*numfaces);

    if (point==1 || point==2) face1++;
    if (point==2 || point==3) face2++;

    float f1 = float(face1)/numfaces2;
    float f2 = float(face2)/numfaces;

    f1*=3.14159;
    f2*=2.0*3.14159;
    float angle1 = f1;
    float angle2 = f2;
    /*
  int face1 = face / (numfaces);
  int face2 = face - face1*(numfaces);
  //int numfaces3 = numfaces*2;
  //Vector p = Vector(0.0, 0.0, radius); 
  float deltaangle1 = 2.0*  3.14159/(numfaces);
  float deltaangle2 =   3.14159/numfaces2; 
  float angle1 = face2*deltaangle1;
  float angle2 = face1*deltaangle2;
  if (point==2||point==1) angle1+=deltaangle1;
  if (point==3||point==2) angle2+=deltaangle2;
    */
  //Matrix m = Matrix::XRotation(angle1)*Matrix::YRotation(angle2);
  Vector pp;
  pp.dx = radius*sin(angle1)*cos(angle2);
  pp.dy = radius*sin(angle1)*sin(angle2);
  pp.dz = radius*cos(angle1);
  return center+pp;
  
}
Point2d SphereElem::TexCoord(int face, int point) const
  {
    int face1 = face / numfaces;
    int face2 = face - (face1*numfaces);

    if (point==1 || point==2) face1++;
    if (point==2 || point==3) face2++;

    float f1 = float(face1)/numfaces2;
    float f2 = float(face2)/numfaces;
    Point2d p = { f1,f2 };
    return p;
    /*
    int face1 = face / (numfaces);
    int face2 = face - face1*(numfaces);
    //int numfaces3 = numfaces*2;
    float deltaangle1 = 1.0/(numfaces+1);
    float deltaangle2 = 1.0/(numfaces2+1);
    float angle1 = face2*deltaangle1;
    float angle2 = face1*deltaangle2;
    if (point==2||point==3) angle1+=deltaangle1;
    if (point==1||point==2) angle2+=deltaangle2;
    Point2d p;
    p.x = angle1;
    p.y = angle2;
    std::cout << "SphereElem TexCoord: " << face << " " << point << " " << face1 << " " << face2 << " " << angle1 << " " << angle2 << std::endl;
    return p;
    */
  }
#if 0
Counts CalcCounts(FaceCollection *coll, int start, int end)
{
  Counts c;
  c.tri_count = 0;
  c.quad_count = 0;
  c.poly_count = 0;
  int s = coll->NumFaces();
  if (start<0) start=0;
  if (end>s) end=s;
  for(int i=start;i<end;i++)
    {
      int p = coll->NumPoints(i);
      if (p==3) c.tri_count++;
      else if (p==4) c.quad_count++;
      else c.poly_count+=p/2*2+2;
    }
  return c;
}

Counts CalcOffsets(FaceCollection *coll, int start)
{
  Counts c;
  c.tri_count = 0;
  c.quad_count = 0;
  c.poly_count = 0;
  int s = coll->NumFaces();
  if (start<0) start=0;
  if (start>s) start=s;
  for(int i=0;i<start;i++)
    {
      int p = coll->NumPoints(i);
      if (p==3) c.tri_count++;
      else if (p==4) c.quad_count++;
      else c.poly_count+=p/2*2+2;
    }
  return c;
}
#endif
