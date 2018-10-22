       
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
//#define THREADS 1
#endif
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "GameApi_h.hh"
#include "FreeType2.hh"

#include <cstring>

#if 01
void *operator new( std::size_t count)
{
  static int counter = 0;
  counter++;
  if (counter % 10000==0) {
    std::printf("Large number of allocs: %d\n", counter);
  }
  if (count >1000000) {
    std::printf("global op new called, size= %zu\n", count);
  }
  return malloc(count);
}
void operator delete(void* ptr) noexcept
{
  free(ptr);
}
#endif

int array_type_to_int(GameApi::BM b) { return E_BM; }
int array_type_to_int(GameApi::P b) { return E_P; }


std::string funccall_to_string(ShaderModule *mod);
std::string color_funccall_to_string(ShaderModule *mod);
std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);
std::string color_funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);

EnvImpl::EnvImpl() : event_infos(new EmptySequencer2), mutex(PTHREAD_MUTEX_INITIALIZER)
{
#ifndef EMSCRIPTEN
#ifdef HAS_FREETYPE
    FT_Init_FreeType(&lib);
    //std::cout << "Freetype init error: " << err << std::endl;
    //std::cout << &lib << std::endl;
#endif
#else
#ifdef HAS_FREETYPE
    FT_Init_FreeType(&lib);
#endif
#endif 
    cursor_pos_point_id.id = -1;
    async_loader = new ASyncLoader;
}

GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm);

#if MOVED_TO_PARSER_HH
struct BitmapHandle
{
  int id;
  virtual ~BitmapHandle() { }
};
#endif

#if MOVED_TO_PARSER_HH
struct BitmapColorHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  ~BitmapColorHandle() { delete bm; }
};
struct BitmapIntHandle : public BitmapHandle
{
  Bitmap<int> *bm;
  ~BitmapIntHandle() { delete bm; }
};
struct BitmapArrayHandle : public BitmapHandle
{
  std::vector<BitmapHandle*> vec;
  std::vector<int> owned;
  ~BitmapArrayHandle() 
  {
    // NOT deleted here, because bitmaparray() uses find_bitmap() to find the
    // handles, and thus we don't have ownership of them.
    if (vec.size() != owned.size())
      {
	std::cout << "BitmapArrayHandle destructor: vector sizes do not match" << std::endl;
      }
    int s = vec.size();
    for(int i=0;i<s;i++) 
      if (owned[i]==1)
	delete vec[i];
  }
};
struct BitmapPosHandle : public BitmapHandle
{
  Bitmap<Pos> *bm;
  ~BitmapPosHandle() { delete bm; }
};
struct BitmapTileHandle : public BitmapHandle
{
  Bitmap<Color> *bm;
  int tile_sx, tile_sy;
  ~BitmapTileHandle() { delete bm; }
};
#endif



ArrayRender *FindRender(GameApi::Env &e, int bm_i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->renders[bm_i];
}
ArrayRender *FindRender2(GameApi::Env &e, int bm_i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->renders2[bm_i];
}

GameApi::PT add_point(GameApi::Env &e, float x, float y);
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z);


EnvImpl::~EnvImpl()
{
  int tt4a = phys.size();
  for(int ui4=0;ui4<tt4a;ui4++)
    {
      delete phys[ui4];
    }

  int tt3 = samples.size();
  for(int iu3=0;iu3<tt3;iu3++)
    {
      delete samples[iu3];
    }

  int tt4 = trackers.size();
  for(int iu4=0;iu4<tt4;iu4++)
    {
      delete trackers[iu4];
    }

  int tt1 = floats.size();
  for(int iu1=0;iu1<tt1;iu1++)
    {
      delete floats[iu1];
    }
  int tt2 = float_array.size();
  for(int iu2=0;iu2<tt2;iu2++)
    {
      delete float_array[iu2];
    }

  int sk6 = textures.size();
  for(int ii6=0;ii6<sk6;ii6++)
    {
      TextureI *ptr = textures[ii6];
      delete ptr;
    }
  int sk5 = pointsapi_points.size();
  for(int ii5=0;ii5<sk5;ii5++)
    {
      PointsApiPoints *p = pointsapi_points[ii5];
      delete p;
    }
  int sk4 = plane_points.size();
  for(int ii4=0;ii4<sk4;ii4++)
    {
      PlanePoints2d *p = plane_points[ii4];
      delete p;
    }
  int sk3 = continuous_bitmaps.size();
  for(int ii3=0;ii3<sk3;ii3++)
    {
      ContinuousBitmap<Color> *p = continuous_bitmaps[ii3];
      delete p;
    }

  int sk2 = vertex_array_render.size();
  for(int ii2=0;ii2<sk2;ii2++)
    {
      RenderVertexArray *s = vertex_array_render[ii2];
      delete s;
    }

  int sk1 = vertex_array.size();
  for(int ii1=0;ii1<sk1;ii1++)
    {
      VertexArraySet *s = vertex_array[ii1];
      delete s;
    }


  std::map<int, ArrayRender*>::iterator it = renders.begin();
  for(;it!=renders.end();it++)
    {
      ArrayRender *rend = (*it).second;
      delete rend;
    }
  std::map<int, ArrayRender*>::iterator it2 = renders2.begin();
  for(;it2!=renders2.end();it2++)
    {
      ArrayRender *rend = (*it2).second;
      delete rend;
    }

  int vv1 = volumes.size();
  for(int i_v=0;i_v<vv1;i_v++)
    {
      delete volumes[i_v];
    }
  int vv2 = floatvolumes.size();
  for(int i_vv2=0;i_vv2<vv2;i_vv2++)
    {
      delete floatvolumes[i_vv2];
    }
  int vv3 = pointarray.size();
  for(int i_vv3=0;i_vv3<vv3;i_vv3++)
    {
      if (pointarray[i_vv3]!=0)
	{
	  delete [] pointarray[i_vv3]->array;
	  delete [] pointarray[i_vv3]->color_array;
	}
      delete pointarray[i_vv3];
    }
  int vv3a = pointarray3.size();
  for(int i_vv3a=0;i_vv3a<vv3a;i_vv3a++)
    { 
      if (pointarray3[i_vv3a]!=0)
	{
	  delete [] pointarray3[i_vv3a]->array;
	  delete [] pointarray3[i_vv3a]->color;
	}
      delete pointarray3[i_vv3a];
    }
  int vv4 = linearray.size();
  for(int i_vv4=0;i_vv4<vv4;i_vv4++)
    {
      delete linearray[i_vv4];
    }
  int vv5 = pointcollarray.size();
  for(int i_vv5=0;i_vv5<vv5;i_vv5++)
    {
      delete pointcollarray[i_vv5];
    }

  int ss1 = bool_bm.size();
  for(int i_1=0;i_1<ss1;i_1++)
    {
      BoolBitmap &bm = bool_bm[i_1];
      delete bm.bitmap;
    }
  int ss1a = waveforms.size();
  for(int i_1a=0;i_1a<ss1a;i_1a++)
    {
      Waveform *bm = waveforms[i_1a];
      delete bm;
    }
  int ss2 = float_bm.size();
  for(int i_2=0;i_2<ss2;i_2++)
    {
      FloatBitmap &bm = float_bm[i_2];
      delete bm.bitmap;
    }

  int s0 = fonts.size();
  for(int i0=0;i0<s0;i0++)
    {
      Font f = fonts[i0];
      delete f.bm;
    }
#ifndef EMSCRIPTEN
#ifdef HAS_FREETYPE
  std::cout << "FREE FREETYPE!" << std::endl;
  FT_Done_FreeType(lib);
#endif
#endif
  int s1 = bm.size();
  for(int i1=0;i1<s1;i1++)
    {
      BitmapHandle *handle = bm[i1];
      //std::cout << "EnvImpl destructor: " << handle << std::endl;
      delete handle;
    }
  int s2 = anim.size();
  for(int i2=0;i2<s2;i2++)
    {
      AnimImpl *impl = &anim[i2];
      delete impl->wave_int;
      delete impl->wave_point;
      delete impl->wave_float;
    }

  int s3 = poly.size();
  for(int i3=0;i3<s3;i3++)
    {
      FaceCollPolyHandle *handle = poly[i3];
      delete handle;
    }
  int s4 = func.size();
  for(int i4=0;i4<s4;i4++)
    {
      FunctionImpl f = func[i4];
      delete f.func;
    }
  int s5 = surfaces.size();
  for(int i5=0;i5<s5;i5++)
    {
      SurfaceImpl s = surfaces[i5];
      delete s.surf;
    }  
  delete event_infos;

#if 0
  int s6 = matrix.size();
  for(int i6=0;i6<s6;i6++)
    {
      MatrixInterface *i = matrix[i6];
      delete i;
    }
#endif
}

GameApi::Env *Global_latest_env = 0;

EXPORT GameApi::Env *GameApi::Env::Latest_Env()
{
  return Global_latest_env;
}
EXPORT GameApi::Env::Env()
{
  Global_latest_env = this;
  envimpl = (void*)new ::EnvImpl;
}
EXPORT void GameApi::Env::free_memory()
{ 
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_memory();
}
EXPORT std::vector<int> GameApi::Env::store_counts()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 return env->store_counts();
}
EXPORT void GameApi::Env::free_to_counts(std::vector<int> vec)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_to_counts(vec);
}
EXPORT void GameApi::Env::free_temp_memory()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_temp_memory();

}

void InstallProgress(int num,std::string label, int max=15);
EXPORT void GameApi::Env::async_load_url(std::string url, std::string homepage)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->load_urls(url, homepage);

}
EXPORT void GameApi::Env::async_load_callback(std::string url, void (*fptr)(void*), void *data)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->set_callback(url, fptr, data);
}
void ProgressBar(int num, int val, int max, std::string label);
EXPORT std::vector<unsigned char> *GameApi::Env::get_loaded_async_url(std::string url)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;

  return env->async_loader->get_loaded_data(url);
}

EXPORT GameApi::Env::~Env()
{
  delete (::EnvImpl*)envimpl;
}

SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);

#define ARRMACRO2(arr,arr2) \
  arr add_array_ ## arr2 (GameApi::Env &e, ArrayType *type) {	\
    ::EnvImpl *env = ::EnvImpl::Environment(&e);	    \
  env->arrays2.push_back(type); \
  arr im;\
  im.id = env->arrays2.size()-1; \
  return im;\
  } \
  ArrayType *find_array_ ## arr2(GameApi::Env &e, arr a) \
  { \
  ::EnvImpl *env = ::EnvImpl::Environment(&e);\
  return env->arrays2[a.id];\
  }

ARRMACRO2(GameApi::PAR,par)
#undef ARRMACRO2
GameApi::PN add_polynomial(GameApi::Env &e, std::vector<float> *pn)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polynomials.push_back(pn);
  GameApi::PN im;
  im.id = env->polynomials.size()-1;
  return im;

}

GameApi::FBU add_framebuffer(GameApi::Env &e, FrameBuffer *buf)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->frame_buffer.push_back(buf);
  GameApi::FBU im;
  im.id = env->frame_buffer.size()-1;
  return im;

}
GameApi::FML add_framemainloop(GameApi::Env &e, FrameBufferLoop *loop)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->frame_loop.push_back(loop);
  GameApi::FML im;
  im.id = env->frame_loop.size()-1;
  return im;
}


GameApi::DS add_disk_store(GameApi::Env &e, DiskStore *ds)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->disk_store.push_back(ds);
  GameApi::DS im;
  im.id = env->disk_store.size()-1;
  return im;
}
GameApi::TXID add_txid(GameApi::Env &e, TextureID *txid)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  int id = txid->texture();
  env->txids[id] = txid;
  GameApi::TXID tx;
  tx.id = id;
  return tx;
}

GameApi::PA add_patch(GameApi::Env &e, CurvePatch *patch)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_patches.push_back(patch);
  GameApi::PA im;
  im.id = env->curve_patches.size()-1;
  return im;

}
GameApi::SD add_string_display(GameApi::Env &e, StringDisplay *sd)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->string_displays.push_back(sd);
  GameApi::SD im;
  im.id = env->string_displays.size()-1;
  return im;
}
GameApi::GI add_glyph_interface(GameApi::Env &e, GlyphInterface *gi)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->glyph_interfaces.push_back(gi);
  GameApi::GI im;
  im.id = env->glyph_interfaces.size()-1;
  return im;
}
GameApi::FI add_font_interface(GameApi::Env &e, FontInterface *fi)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->font_interfaces.push_back(fi);
  GameApi::FI im;
  im.id = env->font_interfaces.size()-1;
  return im;
}
GameApi::FF add_float_fetcher(GameApi::Env &e, Fetcher<float> *f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->float_fetchers.push_back(f);
  GameApi::FF im;
  im.id = env->float_fetchers.size()-1;
  return im;
}
GameApi::PF add_point_fetcher(GameApi::Env &e, Fetcher<Point> *f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->point_fetchers.push_back(f);
  GameApi::PF im;
  im.id = env->point_fetchers.size()-1;
  return im;
}
GameApi::IF add_int_fetcher(GameApi::Env &e, Fetcher<int> *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->int_fetchers.push_back(i);
  GameApi::IF im;
  im.id = env->int_fetchers.size()-1;
  return im;
}
GameApi::SF add_string_fetcher(GameApi::Env &e, Fetcher<std::string> *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->string_fetchers.push_back(i);
  GameApi::SF im;
  im.id = env->string_fetchers.size()-1;
  return im;
}
GameApi::ARR add_array(GameApi::Env &e, ArrayType *type)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->arrays2.push_back(type);
  GameApi::ARR im;
  im.id = env->arrays2.size()-1;
  return im;
}
GameApi::PTT add_point_transform(GameApi::Env &e, PointTransform *trans)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->point_transforms.push_back(trans);
  GameApi::PTT im;
  im.id = env->point_transforms.size()-1;
  return im;
}
GameApi::KF add_vertex_anim(GameApi::Env &e, VertexAnimNode *node)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vertex_anims.push_back(node);
  GameApi::KF im;
  im.id = env->vertex_anims.size()-1;
  return im;
}
GameApi::CPP add_curve_pos(GameApi::Env &e, CurvePos *pos)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_pos.push_back(pos);
  GameApi::CPP im;
  im.id = env->curve_pos.size()-1;
  return im;
}
GameApi::BLK add_blocker(GameApi::Env &e, Blocker *blk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->blockers.push_back(blk);
  GameApi::BLK im;
  im.id = env->blockers.size()-1;
  return im;

}
GameApi::RUN add_splitter(GameApi::Env &e, Splitter *blk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->splitters.push_back(blk);
  GameApi::RUN im;
  im.id = env->splitters.size()-1;
  return im;

}
GameApi::CC add_color(GameApi::Env &e, ColorChange *cc)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->color_change.push_back(cc);
  GameApi::CC im;
  im.id = env->color_change.size()-1;
  return im;

}
GameApi::PP add_plane_shape(GameApi::Env &e, PlaneShape *sh)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_shapes.push_back(sh);
  GameApi::PP im;
  im.id = env->plane_shapes.size()-1;
  return im;

}
GameApi::MX add_mixed(GameApi::Env &e, MixedI *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->mixed.push_back(m);
  GameApi::MX im;
  im.id = env->mixed.size()-1;
  return im;
}
GameApi::SA add_skeletal(GameApi::Env &e, SkeletalNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->skeletals.push_back(n);
  GameApi::SA im;
  im.id = env->skeletals.size()-1;
  return im;

}
GameApi::MC add_matrix_curve(GameApi::Env &e, Curve<Matrix> *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix_curves.push_back(m);
  GameApi::MC im;
  im.id = env->matrix_curves.size()-1;
  return im;

}
GameApi::MS add_matrix_array(GameApi::Env &e, MatrixArray *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix_arrays.push_back(m);
  GameApi::MS im;
  im.id = env->matrix_arrays.size()-1;
  return im;

}
GameApi::C add_curve(GameApi::Env &e, Curve<Point> *curve)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curves.push_back(curve);
  GameApi::C im;
  im.id = env->curves.size()-1;
  return im;
}
GameApi::US add_uber(GameApi::Env &e, ShaderCall *call)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shadercalls.push_back(call);
  GameApi::US im;
  im.id = env->shadercalls.size()-1;
  return im;
}
GameApi::MT add_material(GameApi::Env &e, Material *mat)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->materials.push_back(mat);
  GameApi::MT im;
  im.id = env->materials.size()-1;
  return im;

}
GameApi::TL add_tree_level(GameApi::Env &e, TreeLevel *lvl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->tree_levels.push_back(lvl);
  GameApi::TL im;
  im.id = env->tree_levels.size()-1;
  return im;
}
GameApi::T add_tree(GameApi::Env &e, TreeStack *tre)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->trees.push_back(tre);
  GameApi::T im;
  im.id = env->trees.size()-1;
  return im;

}
GameApi::IM add_implicit(GameApi::Env &e, ImplicitFunction3d *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->implicit.push_back(m);
  GameApi::IM im;
  im.id = env->implicit.size()-1;
  return im;
}
GameApi::MN add_move(GameApi::Env &e, Movement *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->movement_array.push_back(m);
  GameApi::MN a;
  a.id = env->movement_array.size()-1;
  return a;

}
GameApi::CP add_collision(GameApi::Env &e, Collision *c)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->collision_array.push_back(c);
  GameApi::CP a;
  a.id = env->collision_array.size()-1;
  return a;
}
GameApi::TS add_tri_strip(GameApi::Env &e, TriStrip *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->tri_strip.push_back(n);
  GameApi::TS a;
  a.id = env->tri_strip.size()-1;
  return a;

}
GameApi::PH add_physics(GameApi::Env &e, PhysicsNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->phys.push_back(n);
  GameApi::PH a;
  a.id = env->phys.size()-1;
  return a;

}
GameApi::EX add_expr(GameApi::Env &e, ExprNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->exprs.push_back(n);
  GameApi::EX a;
  a.id = env->exprs.size()-1;
  return a;
}
template<class T>
GameApi::A<T> add_array(GameApi::Env &e, std::vector<T> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  std::vector<int> vec2;
  int s = arr->size();
  for(int i=0;i<s;i++)
    {
      T t = arr->operator[](i);
      int val = t.id;
      vec2.push_back(val);
    }

  env->arrays.push_back(new std::vector<int>(vec2));
  GameApi::A<T> a;
  a.id = env->arrays.size()-1;
  return a;
}
GameApi::ML add_main_loop(GameApi::Env &e, MainLoopItem *item)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->main_loop.push_back(item);
  GameApi::ML ml;
  ml.id = env->main_loop.size()-1;
  return ml;
}
GameApi::FtA add_font_atlas(GameApi::Env &e, FontAtlasInfo *info)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->font_atlas.push_back(info);
  GameApi::FtA ft;
  ft.id = env->font_atlas.size()-1;
  return ft;
}
GameApi::W add_widget(GameApi::Env &e, GuiWidget *w)
{
  if (!w)
    {
      std::cout << "add_widget failed" << std::endl;
      assert(0);
    }

  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->widgets.push_back(w);
  GameApi::W wid;
  wid.id = env->widgets.size()-1;
  return wid;
}
GameApi::SFO add_shader_module(GameApi::Env &e, ShaderModule *vol)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_module.push_back(vol);
  GameApi::SFO sfo;
  sfo.id = env->shader_module.size()-1;
  return sfo;
}
GameApi::SM add_sample(GameApi::Env &e, Samples *s)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->samples.push_back(s);
  GameApi::SM sm;
  sm.id = env->samples.size()-1;
  return sm;
}
GameApi::TRK add_tracker(GameApi::Env &e, Tracker *trk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->trackers.push_back(trk);
  GameApi::TRK sm;
  sm.id = env->trackers.size()-1;
  return sm;
}
GameApi::FBO add_fbo(GameApi::Env &e, Low_GLuint fbo_name, Low_GLuint texture, Low_GLuint depthbuffer, int sx, int sy)
{
  FBOPriv p;
  p.fbo_name = fbo_name;
  p.texture = texture;
  p.depthbuffer = depthbuffer;
  p.sx = sx;
  p.sy = sy;
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->fbos.push_back(p);
  GameApi::FBO f;
  f.id = env->fbos.size()-1;
  return f;
}

GameApi::F add_float(GameApi::Env &e, LazyValue<float> *val)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->floats.push_back(val);
  GameApi::F f;
  f.id = env->floats.size()-1;
  return f;
}
GameApi::FA add_float_array(GameApi::Env &e, Array<int,float> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->float_array.push_back(arr);
  GameApi::FA f;
  f.id = env->float_array.size()-1;
  return f;
}
GameApi::FD add_distance(GameApi::Env &e, DistanceRenderable *dist)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->distvolume.push_back(dist);
  GameApi::FD fd;
  fd.id = env->distvolume.size()-1;
  return fd;
}
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->sp.push_back(i);
  GameApi::SP sp;
  sp.id = env->sp.size()-1;
  //sp.type = 0;
  return sp;
}
GameApi::S add_surface2(GameApi::Env &e, SurfaceIn3d *s)
{
  SurfaceImpl i;
  i.surf = s;
  return add_surface(e,i);
}
GameApi::S add_surface(GameApi::Env &e, SurfaceImpl i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->surfaces.push_back(i);
  GameApi::S s;
  s.id = env->surfaces.size()-1;
  //sp.type = 0;
  return s;
}

GameApi::BM add_bitmap(GameApi::Env &e, BitmapHandle *handle)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->bm.push_back(handle);
  GameApi::BM bm;
  bm.id = env->bm.size()-1;

  //std::cout << "add_bitmap: " << bm.id << std::endl;
  //bm.type = 0;
  handle->id = bm.id;
  return bm;
}

GameApi::BM add_color_bitmap(GameApi::Env &e, Bitmap<Color> *bm)
{
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  GameApi::BitmapApi api(e);
  return add_bitmap(e,handle);
}
GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm)
{
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  return add_bitmap(e,handle);
}
GameApi::BMA add_bitmap_array(GameApi::Env &e, BitmapArray2<Color> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->bm_array.push_back(arr);
  GameApi::BMA bma;
  bma.id = env->bm_array.size()-1;
  return bma;
}
GameApi::BB add_bool_bitmap(GameApi::Env &e, Bitmap<bool> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  BoolBitmap handle;
  handle.bitmap = bitmap;
  env->bool_bm.push_back(handle);
  GameApi::BB bm;
  bm.id = env->bool_bm.size()-1;
  //bm.type = 0;
  return bm;
}

GameApi::LAY add_layout(GameApi::Env &e, Layout *l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->layouts.push_back(l);
  GameApi::LAY ll;
  ll.id = env->layouts.size()-1;
  return ll;
}

GameApi::WV add_waveform(GameApi::Env &e, Waveform *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->waveforms.push_back(bitmap);
  GameApi::WV bm;
  bm.id = env->waveforms.size()-1;
  //bm.type = 0;
  return bm;
}


GameApi::TX add_texture(GameApi::Env &e, TextureI *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->textures.push_back(i);
  GameApi::TX tx;
  tx.id = env->textures.size()-1;
  return tx;
}
GameApi::Q add_tex_quad(GameApi::Env &e, Point2d p1, Point2d p2)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  TexCoordQuad q;
  q.p1 = p1;
  q.p2 = p2;
  env->tex_quads.push_back(q);
  GameApi::Q qq;
  qq.id = env->tex_quads.size()-1;
  return qq;
}

GameApi::FB add_float_bitmap(GameApi::Env &e, Bitmap<float> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  FloatBitmap handle;
  handle.bitmap = bitmap;
  env->float_bm.push_back(handle);
  GameApi::FB bm;
  bm.id = env->float_bm.size()-1;
  //bm.type = 0;
  return bm;
}

GameApi::VA add_vertex_array(GameApi::Env &e, VertexArraySet *va, RenderVertexArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vertex_array.push_back(va);
  env->vertex_array_render.push_back(arr);
  GameApi::VA bm;
  bm.id = env->vertex_array.size()-1;
  return bm;
}
void add_update_widget(GameApi::Env &e, GameApi::W widget, GuiWidget *w)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  delete env->widgets[widget.id];
  env->widgets[widget.id] = w;
}
void add_update_vertex_array(GameApi::Env &e, GameApi::VA va_h, VertexArraySet *va, RenderVertexArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  delete env->vertex_array[va_h.id];
  delete env->vertex_array_render[va_h.id];
  env->vertex_array[va_h.id] = va;
  env->vertex_array_render[va_h.id] = arr;
}

GameApi::CBM add_continuous_bitmap(GameApi::Env &e, ContinuousBitmap<Color> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->continuous_bitmaps.push_back(bitmap);
  GameApi::CBM bm;
  bm.id = env->continuous_bitmaps.size()-1;
  return bm;
}

GameApi::F add_function(GameApi::Env &e, FunctionImpl &f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->func.push_back(f);
  GameApi::F func;
  func.id = env->func.size()-1;
  return func;
  
}
GameApi::F add_function(GameApi::Env &e, Function<float,float> *f)
{
  FunctionImpl ff;
  ff.func = f;
  return add_function(e, ff);
}
GameApi::SA add_separate(GameApi::Env &e, Separate *sep)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->separates.push_back(sep);
  GameApi::SA p;
  p.id = env->separates.size()-1;
  return p;
}

GameApi::PL add_plane(GameApi::Env &e, PlanePoints2d *sep)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_points.push_back(sep);
  GameApi::PL p;
  p.id = env->plane_points.size()-1;
  return p;
}

// takes ownership of PolyHandle*
GameApi::P add_polygon(GameApi::Env &e, FaceCollPolyHandle *handle)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->poly.push_back(handle);
  GameApi::P p;
  p.id = env->poly.size()-1;

  //std::cout << "add_polygon returning" << p.id << std::endl;
  //bm.type = 0;
  handle->id = p.id;
  return p;
}
GameApi::OM add_move(GameApi::Env &e, Array<int, ObjectWithPos> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->object_move.push_back(arr);
  GameApi::OM om;
  om.id = env->object_move.size()-1;
  return om;
}

GameApi::VAA add_move_array(GameApi::Env &e, std::vector<VertexArrayWithPos> *vec)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->object_move_vertex_array.push_back(vec);
  GameApi::VAA om;
  om.id = env->object_move_vertex_array.size()-1;
  return om;
}
// takes ownership of FaceCollection*
GameApi::P add_polygon(GameApi::Env &e, FaceCollection *coll, int size)
{
  FaceCollPolyHandle *h = new FaceCollPolyHandle;
  h->coll = coll;
  h->collowned = true;
  h->collarray=0;
  h->collarrayowned = false;
  //h->size = size;
  //GameApi::PolygonApi api(e);
  return /*api.memoize(*/add_polygon(e,h)/*)*/;
}
GameApi::P add_polygon2(GameApi::Env &e, FaceCollection *coll, int size=1)
{
  FaceCollPolyHandle *h = new FaceCollPolyHandle;
  h->coll = coll;
  h->collowned = true;
  h->collarray=0;
  h->collarrayowned = false;
  //h->size = size;
  return add_polygon(e,h);
}
GameApi::M add_matrix2(GameApi::Env &e, Matrix mm)
{
  GameApi::M m;
  std::copy(&mm.matrix[0], &mm.matrix[0]+16, &m.mat[0]);
  return m;
}
GameApi::M add_matrix(GameApi::Env &e, MatrixInterface *i)
{
  GameApi::M m;
  Matrix mm = i->get_matrix();
  std::copy(&mm.matrix[0], &mm.matrix[0]+16, &m.mat[0]);
  delete i;
  return m;
#if 0
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix.push_back(i);
  GameApi::M m;
  m.id = env->matrix.size()-1;
  return m;
#endif
}

GameApi::LI add_line_array(GameApi::Env &e, LineCollection *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->linearray.push_back(array);
  GameApi::LI pt;
  pt.id = env->linearray.size()-1;
  return pt;
}
GameApi::PTS add_points_api_points(GameApi::Env &e, PointsApiPoints *pts)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointsapi_points.push_back(pts);
  GameApi::PTS ptsa;
  ptsa.id = env->pointsapi_points.size()-1;
  return ptsa;
}
GameApi::PTA add_point_array3(GameApi::Env &e, PointArray3 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray3.push_back(array);
  GameApi::PTA pt;
  pt.id = env->pointarray3.size()-1;
  return pt;
}
GameApi::FOA add_point_array(GameApi::Env &e, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray.push_back(array);
  GameApi::FOA pt;
  pt.id = env->pointarray.size()-1;
  return pt;
}
void add_update_lines_array(GameApi::Env &e, GameApi::LLA la, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  PointArray2 *arr = env->pointarray[la.id];
  g_low->ogl->glDeleteBuffers(1, &arr->buffer);
  g_low->ogl->glDeleteBuffers(1, &arr->buffer2);
#ifdef VAO
  g_low->ogl->glDeleteVertexArrays( 1, arr->vao );
#endif
  delete [] env->pointarray[la.id]->array;
  delete [] env->pointarray[la.id]->color_array;
  delete env->pointarray[la.id];
  env->pointarray[la.id] = array;
}
GameApi::LLA add_lines_array(GameApi::Env &e, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray.push_back(array);
  GameApi::LLA pt;
  pt.id = env->pointarray.size()-1;
  return pt;
}
GameApi::PC add_pointcoll_array(GameApi::Env &e, PointCollection *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointcollarray.push_back(array);
  GameApi::PC pt;
  pt.id = env->pointcollarray.size()-1;
  return pt;
}

GameApi::PT add_point(GameApi::Env &e, float x, float y)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Point p = Point(x,y,0.0);
#ifndef EMSCRIPTEN
  env->lock();
#endif
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
#ifndef EMSCRIPTEN
  env->unlock();
#endif
  //pt.type = 0;
  return pt;
}



GameApi::TR add_timerange(GameApi::Env &e, int paths)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->timeranges.push_back(new TROArray(paths));
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}
GameApi::TR add_timerange(GameApi::Env &e, TROArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->timeranges.push_back(arr);
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr);

GameApi::VV add_timerange_vertexarray(GameApi::Env &e, GameApi::TR tr)
{
  TROArray *arr = find_timerange(e, tr);
  
  EnvImpl *env = ::EnvImpl::Environment(&e);
  VArray *varr = new VArray(*arr);
  varr->copy2();
  env->timerange_vertexarrays.push_back(varr);
  GameApi::VV vv;
  vv.id = env->timerange_vertexarrays.size()-1;
  return vv;
}

GameApi::O add_volume(GameApi::Env &e, VolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->volumes.push_back(o);
  GameApi::O pt;
  pt.id = env->volumes.size()-1;
  return pt;
}


GameApi::FO add_float_volume(GameApi::Env &e, FloatVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->floatvolumes.push_back(o);
  GameApi::FO pt;
  pt.id = env->floatvolumes.size()-1;
  return pt;
}

GameApi::COV add_color_volume(GameApi::Env &e, ColorVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->colorvolume.push_back(o);
  GameApi::COV pt;
  pt.id = env->colorvolume.size()-1;
  return pt;
}


GameApi::VO add_vector_volume(GameApi::Env &e, VectorVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vectorvolume.push_back(o);
  GameApi::VO pt;
  pt.id = env->vectorvolume.size()-1;
  return pt;
}

GameApi::VX add_int_voxel(GameApi::Env &e, Voxel<int> *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->voxels2.push_back(o);
  GameApi::VX pt;
  pt.id = env->voxels2.size()-1;
  return pt;
}
GameApi::VX add_voxel(GameApi::Env &e, Voxel<unsigned int> *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->voxels.push_back(o);
  GameApi::VX pt;
  pt.id = env->voxels.size()-1;
  return pt;
}


GameApi::CO add_color(GameApi::Env &e, int r, int g, int b, int a)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Color p = Color(r,g,b,a);
  env->colors.push_back(p);
  GameApi::CO pt;
  pt.id = env->colors.size()-1;
  //pt.type = 0;
  return pt;
}


GameApi::E add_event(GameApi::Env &e, const EventInfo &info)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  SingleEvent *event = new SingleEvent(env->event_infos, info);
  env->event_infos = event;

  GameApi::E ee;
  ee.id = event->CurrentEventNum();
  return ee;
}
GameApi::L add_link(GameApi::Env &e, GameApi::E e1, GameApi::E e2, LinkInfo info)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  LinkageInfo linkage;
  linkage.start_event = e1.id;
  linkage.end_event = e2.id;
  SingleLink *link = new SingleLink(env->event_infos, linkage, info);
  env->event_infos = link;

  GameApi::L ee;
  ee.id = link->CurrentLinkNum();
  return ee;  
}
GameApi::MV add_mv_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->dims.push_back(new PointNDim(Point(x,y,z)));
  GameApi::MV mv;
  mv.id = env->dims.size()-1;
  return mv;
}
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv);
EventInfo find_event_info(GameApi::Env &e, GameApi::E ee);


GameApi::MV add_line(GameApi::Env &e, GameApi::E start, GameApi::E end, 
		     GameApi::MV start_mv, GameApi::MV end_mv, int start_choose, int end_choose)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  NDim<float,Point> *start_dim = find_dim(e,start_mv);
  NDim<float,Point> *end_dim = find_dim(e, end_mv);
  std::vector<float> count_start = start_dim->Count();
  if (count_start.size()>2) { std::cout << "line() dimension error!" << std::endl; }
  std::vector<float> count_end = end_dim->Count();
  if (count_end.size()>2) { std::cout << "line() dimension error!" << std::endl; }
  EventInfo ei1 = find_event_info(e, start);
  EventInfo ei2 = find_event_info(e, end);
  float delta = ei2.time - ei1.time;
  env->dims.push_back(new NextNDim<float,Point>(*start_dim,*end_dim, delta));
  GameApi::MV mv;
  mv.id = env->dims.size()-1;
  return mv;
}


GameApi::LL add_pos(GameApi::Env &e, GameApi::L l, GameApi::MV point)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  PosInfo pos;
  pos.link_id = l.id;
  pos.curve = env->dims[point.id];
  SinglePos *spos = new SinglePos(env->event_infos, pos);
  env->event_infos = spos;
  GameApi::LL ee;
  ee.id = spos->CurrentPosNum();
  return ee;
}
class EnableLinkArray : public Array<int, bool>
{
public:
  EnableLinkArray(Array<int,bool> *next, int num) : next(next), num(num) { }
  int Size() const {
    if (!next) return num;
    //std::cout << "EnableLinkArray: " << this << " " << next << " " << num << std::endl;
    return std::max(num, next->Size()); }
  bool Index(int i) const {
    if (i==num) return true;
    return next->Index(i);
  }
private:
  Array<int,bool> *next;
  int num;
};

GameApi::ST GameApi::EventApi::enable_obj(ST states, int state, LL link)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int state_range_id = states.id;
  int pos_id = link.id;
  int range_start = env->state_ranges[state_range_id].start_range;
  int range_size = env->state_ranges[state_range_id].range_size;
  if (state > range_size-1) { std::cout << "Enable obj, state range error!" << std::endl; }
  std::cout << "Enable_obj:" << range_start << " " << state << " " << range_size << std::endl;
  StateInfo2 &info = env->states[range_start + state];
  Array<int,bool> *enable = info.enable_obj_array;
  info.enable_obj_array = new EnableLinkArray(enable, pos_id);
  return states;
}
class TexIDEmpty : public TextureID {
public:
  TexIDEmpty(int id) : id(id) { }
  int texture() const { return id; }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual void render(MainLoopEnv &e) { }
private:
  int id;
};

FrameBuffer *find_framebuffer(GameApi::Env &e, GameApi::FBU fb)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->frame_buffer[fb.id];
}

FrameBufferLoop *find_framemainloop(GameApi::Env &e, GameApi::FML ml)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->frame_loop[ml.id];
}

MeshAnim *find_mesh_anim(GameApi::Env &e, GameApi::MA ma)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->mesh_anim[ma.id];
}

CurveGroup *find_curve_group(GameApi::Env &e, GameApi::CG cg)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_group[cg.id];
}

InputForMoving *find_move_input(GameApi::Env &e, GameApi::INP im)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->move_input[im.id];
}
Bitmap<int> *find_int_bitmap(GameApi::Env &e, GameApi::IBM bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->int_bitmaps[bm.id];
}

DiskStore *find_disk_store(GameApi::Env &e, GameApi::DS ds)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->disk_store[ds.id];

}
ShaderBitmap *find_shader_bitmap(GameApi::Env &e, GameApi::SBM sbm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shaderbitmaps[sbm.id];
}
std::vector<float> *find_polynomial(GameApi::Env &e, GameApi::PN pn)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->polynomials[pn.id];
}
DynamicChange *find_dyn_change(GameApi::Env &e, GameApi::DC dc)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->dyn_change[dc.id];
}
ContinuousBitmap<float> *find_cont_float(GameApi::Env &e, GameApi::CFB bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cont_floats[bm.id];

}
ContinuousBitmap<bool> *find_cont_bool(GameApi::Env &e, GameApi::CBB bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cont_bools[bm.id];

}
ShaderI *find_shader(GameApi::Env &e, GameApi::SI si)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shader_interface[si.id];
}
TextureID *find_txid(GameApi::Env &e, GameApi::TXID id)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  TextureID *txid = env->txids[id.id];
  if (txid==0) {
    GameApi::TXID id2 = add_txid(e, new TexIDEmpty(id.id));
    txid = env->txids[id2.id];
  }
  return txid;
}
PointsInPlane *find_plane_points(GameApi::Env &e, GameApi::PLP pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_points2[pl.id];
}
LinesInPlane *find_plane_lines(GameApi::Env &e, GameApi::PLL pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_lines[pl.id];
}
FacesInPlane *find_plane_faces(GameApi::Env &e, GameApi::PLF pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_faces[pl.id];  
}
PropertyArray *find_property_array(GameApi::Env &e, GameApi::PR pr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->prop_array[pr.id];
}
Space3d *find_space_3d(GameApi::Env &e, GameApi::SP sp)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->space_3d[sp.id];
}
CurvePatch *find_patch(GameApi::Env &e, GameApi::PA patch)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_patches[patch.id];
}
CmdExecute *find_cmds(GameApi::Env &e, GameApi::CMD cmds2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cmds[cmds2.id];
}
GlyphInterface *find_glyph_interface(GameApi::Env &e, GameApi::GI gi)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->glyph_interfaces[gi.id];
}

FontInterface *find_font_interface(GameApi::Env &e, GameApi::FI fi)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->font_interfaces[fi.id];
}
StringDisplay *find_string_display(GameApi::Env &e, GameApi::SD sd)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->string_displays[sd.id];
}
Fetcher<float> *find_float_fetcher(GameApi::Env &e, GameApi::FF f)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->float_fetchers[f.id];
}
Fetcher<Point> *find_point_fetcher(GameApi::Env &e, GameApi::PF f)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->point_fetchers[f.id];
}
Fetcher<int> *find_int_fetcher(GameApi::Env &e, GameApi::IF i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->int_fetchers[i.id];
}
Fetcher<std::string> *find_string_fetcher(GameApi::Env &e, GameApi::SF s)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->string_fetchers[s.id];
}
ArrayType *find_array(GameApi::Env &e, GameApi::ARR arr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->arrays2[arr.id];
}
CurvePos *find_curve_pos(GameApi::Env &e, GameApi::CPP cp)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_pos[cp.id];
}
PointTransform *find_point_transform(GameApi::Env &e, GameApi::PTT ptt)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->point_transforms[ptt.id];
}
VertexAnimNode *find_vertex_anim(GameApi::Env &e, GameApi::KF kf)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->vertex_anims[kf.id];
}
Blocker *find_blocker(GameApi::Env &e, GameApi::BLK blk)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->blockers[blk.id];
}
Splitter *find_splitter(GameApi::Env &e, GameApi::RUN spl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->splitters[spl.id];
}
PD_Impl find_polydistfield(GameApi::Env &e, GameApi::PD p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->polydistfield[p.id];
}
ColorChange *find_color(GameApi::Env &e, GameApi::CC cc)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->color_change[cc.id];
}
MixedI *find_mixed(GameApi::Env &e, GameApi::MX m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->mixed[m.id];
}
SkeletalNode *find_skeletal(GameApi::Env &e, GameApi::SA n)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->skeletals[n.id];

}
PlaneShape *find_plane_shape(GameApi::Env &e, GameApi::PP p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_shapes[p.id];

}
Curve<Matrix> *find_matrix_curve(GameApi::Env &e, GameApi::MC m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->matrix_curves[m.id];
}
MatrixArray *find_matrix_array(GameApi::Env &e, GameApi::MS m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->matrix_arrays[m.id];
}
Curve<Point> *find_curve(GameApi::Env &e, GameApi::C c)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curves[c.id];
}
ShaderCall *find_uber(GameApi::Env &e, GameApi::US m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shadercalls[m.id];
}
Material *find_material(GameApi::Env &e, GameApi::MT m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->materials[m.id];
}
TreeStack *find_tree(GameApi::Env &e, GameApi::T m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->trees[m.id];
}
TreeLevel *find_tree_level(GameApi::Env &e, GameApi::TL m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->tree_levels[m.id];
}
ImplicitFunction3d *find_implicit(GameApi::Env &e, GameApi::IM m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->implicit[m.id];
}
Movement *find_move(GameApi::Env &e, GameApi::MN m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->movement_array[m.id];  
}
Collision *find_collision(GameApi::Env &e, GameApi::CP p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->collision_array[p.id];  
}
TriStrip *find_tri_strip(GameApi::Env &e, GameApi::TS p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->tri_strip[p.id];  
}
PhysicsNode *find_physics(GameApi::Env &e, GameApi::PH p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  //std::cout << "find_physics: " << p.id << std::endl;
  return env->phys[p.id];  
}
ExprNode *find_expr(GameApi::Env &e, GameApi::EX n)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->exprs[n.id];  
}
template<class T>
std::vector<T> find_array(GameApi::Env &e, GameApi::A<T> arr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  std::vector<T> vec2;
  std::vector<int> vec = env->arrays[arr.id];
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      T t;
      t.id = vec[i];
      vec2.push_back(t);
    }
  return vec2;
}
MainLoopItem *find_main_loop(GameApi::Env &e, GameApi::ML ml)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->main_loop[ml.id];
}
FontAtlasInfo *find_font_atlas(GameApi::Env &e, GameApi::FtA ft)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->font_atlas[ft.id];
}
ShaderModule *find_shader_module(GameApi::Env &e, GameApi::SFO sfo)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shader_module[sfo.id];
}
GuiWidget *find_widget(GameApi::Env &e, GameApi::W w)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int s = env->widgets.size();
  if (w.id>=0 && w.id<s)
    return env->widgets[w.id];
  //std::cout << "find_widget failed!" << w.id << std::endl;
  //assert(0);
  return 0;
}
Samples* find_samples(GameApi::Env &e, GameApi::SM sm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->samples[sm.id];
}
Tracker* find_tracker(GameApi::Env &e, GameApi::TRK trk)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->trackers[trk.id];
}
EventInfo find_event_info(GameApi::Env &e, GameApi::E ee)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Event(ee.id);
}
FBOPriv *find_fbo(GameApi::Env &e, GameApi::FBO f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->fbos.size())
    return &env->fbos[f.id];
  return 0;
}
LinkageInfo find_linkage(GameApi::Env &e, GameApi::L l)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Linkage(l.id);
}
DistanceRenderable *find_distance(GameApi::Env &e, GameApi::FD fd)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (fd.id >=0 && fd.id < (int)env->distvolume.size())
    return env->distvolume[fd.id];
  return 0;
}
LazyValue<float> *find_float(GameApi::Env &e, GameApi::F f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->floats.size())
    return env->floats[f.id];
  return 0;
}
Array<int, float> *find_float_array(GameApi::Env &e, GameApi::FA f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->float_array.size())
    return env->float_array[f.id];
  return 0;
}
TextureI *find_texture(GameApi::Env &e, GameApi::TX t)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (t.id >=0 && t.id < (int)env->textures.size())
    return env->textures[t.id];
  return 0;
}
TexCoordQuad find_tex_quad(GameApi::Env &e, GameApi::Q t)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (t.id >=0 && t.id < (int)env->tex_quads.size())
    return env->tex_quads[t.id];
  TexCoordQuad q;
  Point2d ex = { 0.0, 0.0 };
  q.p1 = ex;
  q.p2 = ex;
  return q;
}


LinkInfo find_link_info(GameApi::Env &e, GameApi::L l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Links(l.id);
}
PosInfo find_pos_info(GameApi::Env &e, GameApi::LL l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Positions(l.id);
}
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->dims[mv.id];
}

GameApi::MA add_mesh_anim(GameApi::Env &e, MeshAnim *ma)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->mesh_anim.push_back(ma);
  GameApi::MA c;
  c.id = env->mesh_anim.size()-1;
  return c;

}

GameApi::CG add_curve_group(GameApi::Env &e, CurveGroup *cg)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_group.push_back(cg);
  GameApi::CG c;
  c.id = env->curve_group.size()-1;
  return c;

}

GameApi::INP add_move_input(GameApi::Env &e, InputForMoving *im)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->move_input.push_back(im);
  GameApi::INP c;
  c.id = env->move_input.size()-1;
  return c;
}

GameApi::IBM add_int_bitmap(GameApi::Env &e, Bitmap<int> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->int_bitmaps.push_back(bm);
  GameApi::IBM c;
  c.id = env->int_bitmaps.size()-1;
  return c;

}
GameApi::SBM add_shader_bitmap(GameApi::Env &e, ShaderBitmap *sbm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shaderbitmaps.push_back(sbm);
  GameApi::SBM c;
  c.id = env->shaderbitmaps.size()-1;
  return c;
}
GameApi::DC add_dyn_change(GameApi::Env &e, DynamicChange *dc)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->dyn_change.push_back(dc);
  GameApi::DC c;
  c.id = env->dyn_change.size()-1;
  return c;
}
GameApi::CFB add_cont_float(GameApi::Env &e, ContinuousBitmap<float> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cont_floats.push_back(bm);
  GameApi::CFB c;
  c.id = env->cont_floats.size()-1;
  return c;

}
GameApi::CBB add_cont_bool(GameApi::Env &e, ContinuousBitmap<bool> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cont_bools.push_back(bm);
  GameApi::CBB c;
  c.id = env->cont_bools.size()-1;
  return c;

}
GameApi::SI add_shader(GameApi::Env &e, ShaderI *sid)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_interface.push_back(sid);
  GameApi::SI c;
  c.id = env->shader_interface.size()-1;
  return c;

}
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Point p = Point(x,y,z);
#ifndef EMSCRIPTEN
  env->lock();
#endif
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
#ifndef EMSCRIPTEN
  env->unlock();
#endif
  return pt;
}

GameApi::V add_vector(GameApi::Env &e, float dx, float dy, float dz)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Vector p = Vector(dx,dy,dz);
  env->vectors.push_back(p);
  GameApi::V pt;
  pt.id = env->vectors.size()-1;
  //pt.type = 0;
  return pt;
}
GameApi::PLP add_plane_points(GameApi::Env &e, PointsInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_points2.push_back(pl);
  GameApi::PLP c;
  c.id = env->plane_points2.size()-1;
  return c;
}
GameApi::PLL add_plane_lines(GameApi::Env &e, LinesInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_lines.push_back(pl);
  GameApi::PLL c;
  c.id = env->plane_lines.size()-1;
  return c;
}
GameApi::PLF add_plane_faces(GameApi::Env &e, FacesInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_faces.push_back(pl);
  GameApi::PLF c;
  c.id = env->plane_faces.size()-1;
  return c;
}

GameApi::PR add_property_array(GameApi::Env &e, PropertyArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->prop_array.push_back(arr);
  GameApi::PR c;
  c.id = env->prop_array.size()-1;
  return c;
}
GameApi::SP add_space_3d(GameApi::Env &e, Space3d *sp)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->space_3d.push_back(sp);
  GameApi::SP c;
  c.id = env->space_3d.size()-1;
  return c;
}

GameApi::CMD add_cmds(GameApi::Env &e, CmdExecute *cmds2)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cmds.push_back(cmds2);
  GameApi::CMD c;
  c.id = env->cmds.size()-1;
  return c;

}
GameApi::CT add_cutter(GameApi::Env &e, Cutter *cut)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cutters.push_back(cut);
  GameApi::CT c;
  c.id = env->cutters.size()-1;
  return c;
}
GameApi::IS add_anim(GameApi::Env &e, const AnimImpl &impl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->anim.push_back(impl);
  GameApi::IS pt;
  pt.id = env->anim.size()-1;
  //pt.type = 0;
  return pt;
}

BitmapHandle *find_bitmap(GameApi::Env &e, GameApi::BM b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BitmapHandle *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bm.size())
    handle = ee->bm[b.id];
  return handle;
}

ShaderPriv2 *find_shaderpriv(GameApi::Env &e, GameApi::SH sh)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  ShaderPriv2 *p = ee->shader_privs[sh.id];
  return p;
}
Program *find_shader_program(GameApi::Env &e, GameApi::SH sh)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  
  ShaderPriv2 *p = ee->shader_privs[sh.id];
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[sh.id]);
  return prog;
}
BitmapArray2<Color> *find_bitmap_array(GameApi::Env &e, GameApi::BMA b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BitmapArray2<Color> *h = 0;
  if (b.id>=0 && b.id<(int)ee->bm_array.size())
    {
      h = ee->bm_array[b.id];
    }
  return h;
}
BoolBitmap *find_bool_bitmap(GameApi::Env &e, GameApi::BB b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BoolBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bool_bm.size())
    handle = &ee->bool_bm[b.id];
  return handle;
}

Layout *find_layout(GameApi::Env &e, GameApi::LAY l)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Layout *handle = 0;
  if (l.id>=0 && l.id < (int)ee->layouts.size())
    handle = ee->layouts[l.id];
  return handle;
}

Waveform *find_waveform(GameApi::Env &e, GameApi::WV b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Waveform *handle = 0;

  if (b.id >=0 && b.id < (int)ee->waveforms.size())
    handle = ee->waveforms[b.id];
  return handle;
}


ContinuousBitmap<Color> *find_continuous_bitmap(GameApi::Env &e, GameApi::CBM b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  ContinuousBitmap<Color> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->continuous_bitmaps.size())
    handle = ee->continuous_bitmaps[b.id];
  return handle;
}

FloatBitmap *find_float_bitmap(GameApi::Env &e, GameApi::FB b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FloatBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->float_bm.size())
    handle = &ee->float_bm[b.id];
  return handle;
}

Voxel<unsigned int> *find_voxel(GameApi::Env &e, GameApi::VX b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Voxel<unsigned int> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->voxels.size())
    handle = ee->voxels[b.id];
  return handle;
}

Voxel<int> *find_int_voxel(GameApi::Env &e, GameApi::VX b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Voxel<int> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->voxels2.size())
    handle = ee->voxels2[b.id];
  return handle;
}


Separate* find_separate(GameApi::Env &e, GameApi::SA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Separate *sep = 0;
  if (p.id >=0 && p.id<(int)ee->separates.size())
    sep = ee->separates[p.id];
  return sep;
}

PlaneData *find_plane_array(GameApi::Env &e, GameApi::PLA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  PlaneData *data = 0;
  if (p.id>=0 && p.id<(int)ee->plane_array.size())
    data = &ee->plane_array[p.id];
  return data;
}
PlanePoints2d* find_plane(GameApi::Env &e, GameApi::PL p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  PlanePoints2d *sep = 0;
  if (p.id >=0 && p.id<(int)ee->plane_points.size())
    sep = ee->plane_points[p.id];
  return sep;
}

Array<int,ObjectWithPos> *find_move(GameApi::Env &e, GameApi::OM p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Array<int,ObjectWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move.size())
    handle = ee->object_move[p.id];
  return handle;
}

std::vector<VertexArrayWithPos> *find_move_array(GameApi::Env &e, GameApi::VAA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  std::vector<VertexArrayWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move_vertex_array.size())
    handle = ee->object_move_vertex_array[p.id];
  return handle;
}

FaceCollPolyHandle *find_poly(GameApi::Env &e, GameApi::P p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FaceCollPolyHandle *handle = 0;

  if (p.id >=0 && p.id < (int)ee->poly.size())
    handle = ee->poly[p.id];
  //std::cout << "find_poly:" << handle << std::endl;
  return handle;
}
RenderVertexArray *find_vertex_array_render(GameApi::Env &e, GameApi::VA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  RenderVertexArray *handle = 0;

  if (p.id >=0 && p.id < (int)ee->vertex_array_render.size())
    handle = ee->vertex_array_render[p.id];
  return handle;
}
VertexArraySet *find_vertex_array(GameApi::Env &e, GameApi::VA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  VertexArraySet *handle = 0;

  if (p.id >=0 && p.id < (int)ee->vertex_array.size())
    handle = ee->vertex_array[p.id];
  return handle;
}


FaceCollection *find_facecoll(GameApi::Env &e, GameApi::P p)
{
  FaceCollPolyHandle *hh = find_poly(e,p);
  //FaceCollPolyHandle *hh = dynamic_cast<FaceCollPolyHandle*>(h);
  if (hh->coll)
    return hh->coll;
  else
    return 0;
}
FunctionImpl *find_function(GameApi::Env &e, GameApi::F f)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FunctionImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->func.size())
    handle = &ee->func[f.id];
  return handle;
}

SurfaceIn3d *find_surface2(GameApi::Env &e, GameApi::S s)
{
  return find_surface(e,s)->surf;
}
SurfaceImpl *find_surface(GameApi::Env &e, GameApi::S f)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  SurfaceImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->surfaces.size())
    handle = &ee->surfaces[f.id];
  return handle;
}



AnimImpl find_anim(GameApi::Env &e, GameApi::IS i)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (i.id >=0 && i.id < (int)ee->anim.size())
    {
      return ee->anim[i.id];
    }
  std::cout << "find_anim: anim not found: " << i.id << std::endl;
  AnimImpl ii;
  ii.wave_int = 0;
  ii.wave_point = 0;
  ii.wave_float = 0;
  return ii;
}
Bitmap<Color> *find_color_bitmap(BitmapHandle *handle, int bbm_choose)
{

  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      if (bbm_choose != -1 && bbm_choose >=0 && bbm_choose < (int)ahandle->vec.size())
	  handle = ahandle->vec[bbm_choose];
    }

   BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
   BitmapTileHandle *chandle2 = dynamic_cast<BitmapTileHandle*>(handle);
   if (!chandle && !chandle2) return 0;
   if (chandle)
     return chandle->bm;
   return chandle2->bm;
}
Bitmap<Pos> *find_pos_bitmap(BitmapHandle *handle, int bbm_choose)
{

  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      if (bbm_choose != -1 && bbm_choose >=0 && bbm_choose < (int)ahandle->vec.size())
	  handle = ahandle->vec[bbm_choose];
    }

   BitmapPosHandle *chandle = dynamic_cast<BitmapPosHandle*>(handle);
   if (!chandle) return 0;
   return chandle->bm;
}



PointsApiPoints *find_pointsapi_points(GameApi::Env &e, GameApi::PTS ps)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (ps.id >=0 && ps.id < (int)ee->pointsapi_points.size())
    return ee->pointsapi_points[ps.id];
  return 0;
}
LineCollection *find_line_array(GameApi::Env &e, GameApi::LI li)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (li.id >=0 && li.id < (int)ee->linearray.size())
    return ee->linearray[li.id];
  return 0;
}
PointArray3 *find_point_array3(GameApi::Env &e, GameApi::PTA pa)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (pa.id >=0 && pa.id < (int)ee->pointarray3.size())
    return ee->pointarray3[pa.id];
  return 0;
}
PointArray2 *find_point_array(GameApi::Env &e, GameApi::FOA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointarray.size())
    return ee->pointarray[p.id];
  return 0;
}

PointArray2 *find_lines_array(GameApi::Env &e, GameApi::LLA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointarray.size())
    return ee->pointarray[p.id];
  return 0;
}


PointCollection *find_pointcoll_array(GameApi::Env &e, GameApi::PC p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointcollarray.size())
    return ee->pointcollarray[p.id];
  return 0;
}


Point *find_point(GameApi::Env &e, GameApi::PT p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pt.size())
    return &ee->pt[p.id];
  return 0;
}
Matrix find_matrix(GameApi::Env &e, GameApi::M m)
{
  Matrix mm;
  std::copy(&m.mat[0], &m.mat[0]+16, &mm.matrix[0]);
  mm.is_identity=false;
  return mm;
#if 0
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->matrix.size())
    return ee->matrix[p.id];
  return 0;
#endif
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (tr.id >=0 && tr.id < (int)ee->timeranges.size())
    return ee->timeranges[tr.id];
  return 0;
}
VArray *find_timerange_vertexarray(GameApi::Env &e, GameApi::VV vv)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (vv.id >=0 && vv.id < (int)ee->timerange_vertexarrays.size())
    return ee->timerange_vertexarrays[vv.id];
  return 0;
}

VolumeObject *find_volume(GameApi::Env &e, GameApi::O o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->volumes.size())
    return ee->volumes[o.id];
  return 0;
}



FloatVolumeObject *find_float_volume(GameApi::Env &e, GameApi::FO o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->floatvolumes.size())
    return ee->floatvolumes[o.id];
  return 0;
}

ColorVolumeObject *find_color_volume(GameApi::Env &e, GameApi::COV o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->colorvolume.size())
    return ee->colorvolume[o.id];
  return 0;
}

VectorVolumeObject *find_vector_volume(GameApi::Env &e, GameApi::VO o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->vectorvolume.size())
    return ee->vectorvolume[o.id];
  return 0;
}


Color *find_color(GameApi::Env &e, GameApi::CO p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->colors.size())
    return &ee->colors[p.id];
  return 0;
}


Vector *find_vector(GameApi::Env &e, GameApi::V p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->vectors.size())
    return &ee->vectors[p.id];
  return 0;
}


SpaceImpl *find_space(GameApi::Env &e, GameApi::SP s)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (s.id >=0 && s.id < (int)ee->sp.size())
    return &ee->sp[s.id];
  return 0;
}
LineImpl *find_line(GameApi::Env &e, GameApi::LN l)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (l.id >=0 && l.id < (int)ee->ln.size())
    return &ee->ln[l.id];
  return 0;
}
SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (bm.id >=0 && bm.id < (int)ee->spr_pos.size())
    return &ee->spr_pos[bm.id];
  return 0;
}
Cutter *find_cutter(GameApi::Env &e, GameApi::CT cut)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  return ee->cutters[cut.id];
}


EXPORT GameApi::GridApi::GridApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new GridPriv;
}
EXPORT GameApi::GridApi::~GridApi()
{
  delete (GridPriv*)priv;
}

EXPORT GameApi::BM GameApi::WaveformApi::waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  Waveform *m_wave = find_waveform(e, wave);
  return add_color_bitmap(e, new WaveformBitmap(*m_wave, 0.0, m_wave->Length(), m_wave->Min(), m_wave->Max(), sx, sy, Color(true_color), Color(false_color)));
}

GameApi::SpaceApi::SpaceApi(Env &e) : e(e) { }

GameApi::SpaceApi::~SpaceApi() { }

#if 0
GameApi::PT GameApi::MovementApi::pos(float x, float y)
{
  return add_point(e, x,y);
}
#endif


std::map<int, void (*)(GameApi::EveryApi&) > gamefunctions;
std::map<int, int> gamemapping;

EXPORT GameApi::GamesApi::GamesApi(Env &e) : e(e)
{
}
EXPORT GameApi::GamesApi::~GamesApi()
{
}
EXPORT void GameApi::GamesApi::register_game(int game_id, void (*fptr)(GameApi::EveryApi &e))
{
  gamefunctions[game_id] = fptr;
}
EXPORT void GameApi::GamesApi::modify_map(int event, int game_id)
{
  gamemapping[event] = game_id;
}
EXPORT void GameApi::GamesApi::start_game(int event)
{
  int id = gamemapping[event];
  void (*fptr)(GameApi::EveryApi &) = gamefunctions[id];
  if (fptr)
    {
      GameApi::EveryApi ea(e);
      fptr(ea);
    }
}





































GameApi::SV GameApi::SpaceVectorApi::from_points(GameApi::PC pc) {
  // choose polygon via hit test
  // find value of vector via interpolation
  // return it.
  // SV = PT->V
  // PC = int->PT
  // algo fills the space with polygons, finds vector.
  GameApi::SV sv;
  sv.id = 0;
  return sv;
}
GameApi::PT GameApi::SpaceVectorApi::flow_next_point(SV v, PT p, float mult)
{
  GameApi::PT pt;
  pt.id = 0;
  return pt;
}

GameApi::SA GameApi::SeparateApi::empty()
{
  return add_separate(e, new SeparateEmpty);
}

GameApi::SA GameApi::SeparateApi::u_sep(SA orig, float (*sep_x)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateX(sep, sep_x, data));
}
GameApi::SA GameApi::SeparateApi::v_sep(SA orig, float (*sep_y)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateY(sep, sep_y, data));
}
GameApi::SA GameApi::SeparateApi::surf_sep(SA orig, float (*sep_dist)(float x, float y, float z, void *data), void *data)
{
  Separate *sep = find_separate(e, orig);
  return add_separate(e, new SeparateDist(sep, sep_dist, data));
}

GameApi::P GameApi::SeparateApi::create_quads(GameApi::SA object, float dist, float dist_accuracy,
			float x_range_start, float x_range_end, float x_step,
			float y_range_start, float y_range_end, float y_step,
			float z_range_start, float z_range_end, float z_step,
			float u_range_start, float u_range_end, float u_step,
			float v_range_start, float v_range_end, float v_step
)
{
  Separate *sep = find_separate(e, object);
  return add_polygon(e, new SeparateRender(*sep, dist, dist_accuracy,
					   x_range_start, x_range_end, x_step,
					   y_range_start, y_range_end, y_step,
					   z_range_start, z_range_end, z_step,
					   u_range_start, u_range_end, u_step,
					   v_range_start, v_range_end, v_step), 1);
}



class EmptyMove : public Array<int, ObjectWithPos>
{
public:
  EmptyMove(int s) : s(s) { }
  ObjectWithPos Index(int i) const
  {
    ObjectWithPos p;
    GameApi::P pp;
    GameApi::M m;
    pp.id = 0;
    for(int i=0;i<16;i++)
      {
	m.mat[i] = 0.0f;
      }
    p.obj = pp;
    p.m = m;
    return p;
  }
  int Size() const { return s; }
private:
  int s;
};

class AddObjectMove : public Array<int,ObjectWithPos>
{
public:
  AddObjectMove(Array<int,ObjectWithPos> &arr, int index, GameApi::P coll) : arr(arr), index(index), coll(coll) { }
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
    if (i==index) { 
      ObjectWithPos p;
      p.obj = coll;
      p.m = arr.Index(i).m;
      return p;
    }
    return arr.Index(i);
  }  
private:
  Array<int,ObjectWithPos> &arr;
  int index;
  GameApi::P coll;
};
class PosMove : public Array<int,ObjectWithPos>
{
public:
  PosMove(Array<int,ObjectWithPos> &arr, int index, GameApi::M m) : arr(arr), index(index), m(m) { }
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
    if (i==index) { 
      ObjectWithPos p;
      p.obj = arr.Index(i).obj;
      p.m = m;
      return p;
    }
    return arr.Index(i);
  }
private:
  Array<int, ObjectWithPos> &arr;
  int index;
  GameApi::M m;
};

#if 0
class VertexMove : public Array<int, ObjectWithPos>
{
public:
  int Size() const { return arr.Size(); }
  ObjectWithPos Index(int i) const
  {
  }
};
#endif

//  std::vector<Array<ObjectWithPos> *> object_move;
GameApi::OM GameApi::ObjectMoveApi::empty(int numobjects)
{
  return add_move(e, new EmptyMove(numobjects));
}
GameApi::OM GameApi::ObjectMoveApi::add_object(OM orig, int index, P obj)
{
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new AddObjectMove(*arr, index, obj));
}
GameApi::OM GameApi::ObjectMoveApi::clear_object(OM orig, int index)
{
  P p;
  p.id = 0;
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new AddObjectMove(*arr, index, p));
}
GameApi::OM GameApi::ObjectMoveApi::change_pos(OM orig, int index, M mat)
{
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  return add_move(e, new PosMove(*arr, index, mat));
}
GameApi::VAA GameApi::ObjectMoveApi::prepare_all(OM orig)
{
  PolygonApi poly(e);
  Array<int, ObjectWithPos> *arr = find_move(e, orig);
  int s = arr->Size();
  std::vector<VertexArrayWithPos> *vec = new std::vector<VertexArrayWithPos>;
  for(int i=0;i<s;i++)
    {
      ObjectWithPos p = arr->Index(i);
      GameApi::VA va = poly.create_vertex_array(p.obj);
      VertexArrayWithPos pos;
      pos.va = va;
      pos.m = p.m;
      vec->push_back(pos);
    }
  return add_move_array(e, vec);
}
void GameApi::ObjectMoveApi::render_all(GameApi::VAA va)
{
  PolygonApi poly(e);
  std::vector<VertexArrayWithPos> *vec = find_move_array(e, va);
  int s = vec->size();
  for(int i=0;i<s;i++)
    {
      VertexArrayWithPos p = (*vec)[i];
      Matrix mm = find_matrix(e, p.m);
#ifndef EMSCRIPTEN
      g_low->ogl->glPushMatrix();
      float mat[16] = { mm.matrix[0], mm.matrix[4], mm.matrix[8], mm.matrix[12],
			mm.matrix[1], mm.matrix[5], mm.matrix[9], mm.matrix[13],
			mm.matrix[2], mm.matrix[6], mm.matrix[10], mm.matrix[14],
			mm.matrix[3], mm.matrix[7], mm.matrix[11], mm.matrix[15] };
      
      g_low->ogl->glMultMatrixf(&mat[0]);

      poly.render_vertex_array(p.va);
      g_low->ogl->glPopMatrix();
#endif
    }
}




void *Thread_Call(void *data)
{
  GameApi::RenderObject *o = (GameApi::RenderObject*)data;
  o->prepare();
#ifndef ARM
  pthread_exit(NULL);
#endif
  return 0;
}
#ifndef ARM
pthread_t thread;
#endif
void GameApi::prepare(GameApi::RenderObject &o)
{
#ifndef ARM 
  pthread_create(&thread, NULL, Thread_Call, (void*)&o);
#endif
}

GameApi::Pa GameApi::PolygonArrayApi::split_p(EveryApi &ev, P p, int max_chunk)
{
  FaceCollection *f = find_facecoll(e, p);
  f->Prepare();
  std::vector<P> vec;
  int num = f->NumFaces();
  int start = 0;
  std::cout << "split_p" << std::endl;
  while(num > max_chunk)
    { 
      std::cout << "Range: " << start << " " << max_chunk << std::endl;
      vec.push_back(ev.polygon_api.split_p(p, start, start+max_chunk));
      start += max_chunk;
      num -= max_chunk;
    }
  std::cout << "LastRange: " << start << " " << num << std::endl;
  vec.push_back(ev.polygon_api.split_p(p, start, start+num));
  Pa_Impl impl;
  impl.vec = vec;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polygon_array.push_back(impl);
  Pa pa;
  pa.id = env->polygon_array.size()-1;
  return pa;  
}
GameApi::Va GameApi::PolygonArrayApi::create_vertex_array(EveryApi &ev, Pa p, bool keep)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Pa_Impl &impl = env->polygon_array[p.id];
  int s = impl.vec.size();
  Va_Impl va_impl;
  for(int i=0;i<s;i++)
    {
      P pp = impl.vec[i];
      VA va = ev.polygon_api.create_vertex_array(pp, keep);
      va_impl.vec.push_back(va);
    }
  env->va_array.push_back(va_impl);
  Va va;
  va.id = env->va_array.size()-1;
  return va;
}
void GameApi::PolygonArrayApi::render_vertex_array(EveryApi &ev, Va p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[p.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.render_vertex_array(va);
    }
}
void GameApi::PolygonArrayApi::prepare_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.prepare_vertex_array_instanced(sha, va, pta, sh);
    }
}
void GameApi::PolygonArrayApi::render_vertex_array_instanced(EveryApi &ev, ShaderApi &sha, Va va, PTA pta, SH sh)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.render_vertex_array_instanced(sha, va, pta, sh);
    }
}
void GameApi::PolygonArrayApi::delete_vertex_array(EveryApi &ev, Va va)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Va_Impl &impl = env->va_array[va.id];
  int s = impl.vec.size();
  for(int i=0;i<s;i++)
    {
      VA va = impl.vec[i];
      ev.polygon_api.delete_vertex_array(va);
    }
}

EXPORT GameApi::BO GameApi::BooleanOps::create_bo(P mesh, O bools, FD fd)
{
  BO_Impl bo;
  bo.mesh = mesh;
  bo.bools = bools;
  bo.fd = fd;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->boolean_ops.push_back(bo);
  BO bo2;
  bo2.id = env->boolean_ops.size()-1;
  return bo2;
  
}

EXPORT GameApi::BO GameApi::BooleanOps::cube(GameApi::EveryApi &ev, 
					 float start_x, float end_x,
					 float start_y, float end_y,
				      float start_z, float end_z,
				      int split_x, int split_y)
{
  P mesh = ev.polygon_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  P mesh2 = ev.polygon_api.splitquads(mesh, split_x, split_y);

  O bools = ev.volume_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  FD fd = ev.dist_api.cube(start_x, end_x, start_y, end_y, start_z, end_z);
  //CT cutter = ev.cutter_api.distance_cut(fd);
  return create_bo(mesh2, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::sphere(GameApi::EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
{
  P mesh = ev.polygon_api.sphere(center, radius, numfaces1, numfaces2);
  O bools = ev.volume_api.sphere(center, radius);
  FD fd = ev.dist_api.sphere(center, radius);
  return create_bo(mesh, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::or_elem(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];
  P mesh = ev.polygon_api.or_elem(obj_i.mesh, obj2_i.mesh);
  O bools = ev.volume_api.max_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.min(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}
EXPORT GameApi::BO GameApi::BooleanOps::and_not(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];

  CT cutter = ev.cutter_api.distance_cut(obj_i.fd);
  CT cutter2 = ev.cutter_api.distance_cut(obj2_i.fd);
  P mesh = ev.polygon_api.and_not_elem(ev, obj2_i.mesh, obj_i.mesh,
				       obj2_i.bools, obj_i.bools,
				       cutter2, cutter);
  //P mesh2 = ev.polygon_api.tri_to_quad(mesh);
  O bools = ev.volume_api.andnot_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.and_not(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}

EXPORT GameApi::BO GameApi::BooleanOps::intersect(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];

  CT cutter = ev.cutter_api.distance_cut(obj_i.fd);
  CT cutter2 = ev.cutter_api.distance_cut(obj2_i.fd);
  P mesh = ev.polygon_api.intersect(ev, obj_i.mesh, obj2_i.mesh,
				       obj_i.bools, obj2_i.bools,
				       cutter, cutter2);
  O bools = ev.volume_api.min_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.max(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}

EXPORT GameApi::P GameApi::BooleanOps::to_polygon(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.mesh;
}
EXPORT GameApi::O GameApi::BooleanOps::to_volume(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.bools;
}
EXPORT GameApi::FD GameApi::BooleanOps::to_dist(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.fd;
}

// Explicit instantiation of array api
namespace GameApi {
#ifndef EMSCRIPTEN
  //class template ArrayApi<GameApi::BM>;
  //class template ArrayApi<GameApi::P>;
#endif
};
class RectangleCollision : public Collision
{
public:
  RectangleCollision(int id, float x, float y, float sx, float sy) : Collision(id), x(x),y(y),sx(sx),sy(sy) { }
  bool check_collision(float xx,float yy) const
  {
    xx-=m_x;
    yy-=m_y;
    if (xx<x) return false;
    if (yy<y) return false;
    if (xx>x+sx) return false;
    if (yy>y+sy) return false;
    return true;
  }
private:
  float x,y,sx,sy;
};
class OrElemCollision : public Collision
{
public:
  OrElemCollision(int id, std::vector<Collision*> vec) : Collision(id), vec(vec) { }
  bool check_collision(float x, float y) const
  {
    x-=m_x;
    y-=m_y;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	bool b = vec[i]->check_collision(x,y);
	if (b) { return true; }
      }
    return false;
  }
  void set_pos(int id, float x, float y)
  {
    if (m_id == id) { m_x = x; m_y = y; }
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Collision *c = vec[i];
	c->set_pos(id, x,y);
      }
  }
private:
  std::vector<Collision*> vec;
};
class CircleCollision : public Collision
{
public:
  CircleCollision(int id, float radius) : Collision(id), radius(radius) { }
  bool check_collision(float x, float y) const
  {
    x-=m_x;
    y-=m_y;
    float d = x*x+y*y;
    return d < radius*radius;
  }
private:
  float radius;
};
GameApi::CP GameApi::CollisionPlane::rectangle(int id, float x, float y, float sx, float sy)
{
  return add_collision(e, new RectangleCollision(id, x,y,sx,sy));
}
GameApi::CP GameApi::CollisionPlane::circle(int id, float radius)
{
  return add_collision(e, new CircleCollision(id, radius));
}
GameApi::CP GameApi::CollisionPlane::or_elem(int id, std::vector<CP> vec)
{
  std::vector<Collision*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_collision(e, vec[i]));
    }
  return add_collision(e, new OrElemCollision(id, vec2));
}
void GameApi::CollisionPlane::set_pos(CP plane, int id, float x, float y)
{
  Collision *c = find_collision(e, plane);
  c->set_pos(id, x,y);
}
bool GameApi::CollisionPlane::check_collision(CP plane, float x, float y)
{
  Collision *c = find_collision(e, plane);
  return c->check_collision(x,y);
}

class EmptyMovement : public Movement
{
public:
  EmptyMovement() : m_m(Matrix::Identity()) { }
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }

  void set_matrix(Matrix m) { m_m = m; }
  Matrix get_whole_matrix(float time, float delta_time) const { return m_m; }
private:
  Matrix m_m;
};
EXPORT GameApi::MN GameApi::MovementNode::empty()
{
  return add_move(e, new EmptyMovement);
}
class ColorStart : public ColorChange
{
public:
  ColorStart(unsigned int color) : color(color) { }
  Color get_whole_color(float time, float delta_time) const
  {
    return color;
  }
private:
  unsigned int color;
};
class ColorInterpolate2 : public ColorChange
{
public:
  ColorInterpolate2(ColorChange *next, unsigned int color, unsigned int color2, float start_time, float end_time) : next(next), color(color), color2(color2), start_time(start_time), end_time(end_time) { }
  Color get_whole_color(float time, float delta_time) const
  {
    if (time < start_time) return next->get_whole_color(time, delta_time);
    if (time >= end_time) return next->get_whole_color(time, delta_time);
    float d =  time - start_time;
    d/=(end_time-start_time);
    return Color::Interpolate(color, color2, d);
  }
private:
  ColorChange *next;
  unsigned int color;
  unsigned int color2;
  float start_time;
  float end_time;
};

EXPORT GameApi::CC GameApi::MovementNode::color_start(unsigned int color)
{
  return add_color(e, new ColorStart(color));
}
EXPORT GameApi::CC GameApi::MovementNode::color_interpolate(CC next, unsigned int color, unsigned int color2, float start_time, float end_time)
{
  ColorChange *cc = find_color(e, next);
  return add_color(e, new ColorInterpolate2(cc, color, color2, start_time, end_time));
}
class LevelMovement : public Movement
{
public:
  LevelMovement(Movement *m) : m(m) { }
  virtual void event(MainLoopEvent &e) { m->event(e); }
  virtual void frame(MainLoopEnv &e) { m->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { m->draw_frame(e); }
  void set_matrix(Matrix m) { m_m = m; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return m_m * m->get_whole_matrix(time, delta_time);
  }
private:
  Movement *m;
  Matrix m_m;
};
EXPORT GameApi::MN GameApi::MovementNode::level(MN next)
{
  Movement *move = find_move(e, next);
  return add_move(e, new LevelMovement(move));
}
class TranslateMovement : public Movement
{
public:
  TranslateMovement(Movement *next, float start_time, float end_time,
		    float dx, float dy, float dz)
    : next(next), start_time(start_time), end_time(end_time),
      dx(dx), dy(dy), dz(dz) { }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }

  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { dx=ddx; dy=ddy; dz=ddz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { Matrix m=Matrix::Identity(); return next?next->get_whole_matrix(time, delta_time):m; }
    if (time>=end_time) { Matrix m=Matrix::Identity(); return Matrix::Translate(dx,dy,dz)*(next?next->get_whole_matrix(time,delta_time):m); }
    float d = time - start_time;
    //if (fabs(end_time-start_time)>0.01)
      d/=(end_time-start_time);
    return Matrix::Translate(dx*d,dy*d,dz*d)*(next?next->get_whole_matrix(time, delta_time):Matrix::Identity());
  }
private:
  Movement *next;
  float start_time, end_time;
  float dx,dy,dz;
};

extern float debug_pos_x, debug_pos_y, debug_pos_z;

class DebugTranslateMovement : public Movement
{
public:
  DebugTranslateMovement(Movement *next)
    : next(next){ }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }

  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return Matrix::Translate(debug_pos_x, debug_pos_y, debug_pos_z);
  }
private:
  Movement *next;
  float start_time, end_time;
  float dx,dy,dz;
};

EXPORT GameApi::MN GameApi::MovementNode::debug_translate(MN next)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new DebugTranslateMovement(nxt));
}

EXPORT GameApi::MN GameApi::MovementNode::translate(MN next, 
					     float start_time, float end_time,
					     float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TranslateMovement(nxt,start_time, end_time,
					   dx,dy,dz));
}
extern int async_pending_count;
int FindProgressVal();
int FindProgressMax();
class ScaleProgress : public Movement
{
public:
  ScaleProgress(Movement *next, bool is_x, bool is_y, bool is_z) : next(next), is_x(is_x), is_y(is_y), is_z(is_z) { 
    max_async_pending = 0;
  }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    float x = is_x ? val() : 1.0;
    float y = is_y ? val() : 1.0;
    float z = is_z ? val() : 1.0;
    return Matrix::Scale(x,y,z)*next->get_whole_matrix(time,delta_time);
  }
  float val() const {
    //if (async_pending_count>max_async_pending) max_async_pending=async_pending_count;
    //float val1 = 1.0-(async_pending_count/max_async_pending);
    //if (val1<0.1) val1=0.1;
    //if (val1>1.0) val1=1.0;
    float val2 = float(FindProgressVal())/float(FindProgressMax());
    if (val2<0.1) val2=0.1;
    if (val2>1.0) val2=1.0;
    return val2;
  }
private:
  Movement *next;
  mutable int max_async_pending;
  bool is_x, is_y, is_z;
};

GameApi::MN GameApi::MovementNode::scale_progress(MN next, bool is_x, bool is_y, bool is_z)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new ScaleProgress(nxt,is_x,is_y,is_z));
}
class MN_Fetcher : public Movement
{
public:
  MN_Fetcher(Fetcher<Point> *pf) : pf(pf) {}
  virtual void event(MainLoopEvent &e) { pf->event(e); }
  virtual void frame(MainLoopEnv &e) { pf->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { std::cout << "MN_fetcher not supporting draw_frame" << std::endl; }
  virtual void set_matrix(Matrix m) { }
  virtual Matrix get_whole_matrix(float time, float delta_time) const
  {
    Point p = pf->get();
    return Matrix::Translate(p.x,p.y,p.z);
  }
private:
  Fetcher<Point> *pf;
};
EXPORT GameApi::MN GameApi::MovementNode::mn_fetcher(PF pf)
{
  Fetcher<Point> *ppf = find_point_fetcher(e, pf);
  return add_move(e, new MN_Fetcher(ppf));
}
class ScaleMovement : public Movement
{
public:
  ScaleMovement(Movement *next, float start_time, float end_time,
		    float sx, float sy, float sz)
    : next(next), start_time(start_time), end_time(end_time),
      sx(sx), sy(sy), sz(sz) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  void set_matrix(Matrix m) { }
  void set_scale(float ssx, float ssy, float ssz) { sx=ssx; sy=ssy; sz=ssz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { return Matrix::Scale(sx,sy,sz)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    float ssx = (1.0-d)*1.0 + d*sx;
    float ssy = (1.0-d)*1.0 + d*sy;
    float ssz = (1.0-d)*1.0 + d*sz;
    return Matrix::Scale(ssx,ssy,ssz)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  float sx,sy,sz;
};

EXPORT GameApi::MN GameApi::MovementNode::scale(MN next,
					 float start_time, float end_time,
					 float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new ScaleMovement(nxt, start_time, end_time,
				       sx,sy,sz));
}
class RotateMovement : public Movement
{
public:
public:
  RotateMovement(Movement *next, float start_time, float end_time,
		 float p_x, float p_y, float p_z,
		float v_x, float v_y, float v_z, float angle)
    : next(next), start_time(start_time), end_time(end_time),
      p_x(p_x), p_y(p_y), p_z(p_z),
      v_x(v_x), v_y(v_y), v_z(v_z), angle(angle) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), angle)*next->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time); // [0..1]
    return Matrix::RotateAroundAxisPoint(Point(p_x,p_y,p_z),Vector(v_x,v_y,v_z), d*angle)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  float p_x,p_y,p_z;
  float v_x,v_y,v_z;
  float angle;
};
EXPORT GameApi::MN GameApi::MovementNode::rotate(MN next, float start_time, float end_time,
					  float p_x, float p_y, float p_z,
					  float v_x, float v_y, float v_z, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new RotateMovement(nxt, start_time, end_time,
					p_x,p_y,p_z,v_x, v_y, v_z, angle));
}

class CompressMovement : public Movement
{
public:
  CompressMovement(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) 
  { 
    zero = next->get_whole_matrix(start_time, 1.0);
    one = next->get_whole_matrix(end_time,1.0);
  }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return zero; }
    if (time>end_time) { return one; }
    return next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  Matrix zero;
  Matrix one;
};

class AnimEnable : public Movement
{
public:
  AnimEnable(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return Matrix::Zero(); }
    if (time>end_time) { return Matrix::Zero(); }
    return next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time,end_time;
};

class AnimDisable : public Movement
{
public:
  AnimDisable(Movement *next, float start_time, float end_time) : next(next), start_time(start_time), end_time(end_time) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time,delta_time); }
    if (time>end_time) { return next->get_whole_matrix(time,delta_time); }
    return Matrix::Zero(); 
  }
private:
  Movement *next;
  float start_time,end_time;
};
class AnimChoose : public Movement
{
public:
  AnimChoose(std::vector<Movement*> vec, float start_time, float duration) : vec(vec), start_time(start_time), duration(duration) { }
  virtual void event(MainLoopEvent &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->event(e);
  }
  virtual void frame(MainLoopEnv &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->frame(e);
  }
  virtual void draw_frame(DrawLoopEnv &e) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->draw_frame(e);
  }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (vec.size()==0) return Matrix::Identity();
    if (time < start_time) return vec[0]->get_whole_matrix(time,delta_time);
    if (time > start_time+duration*vec.size()) return vec[vec.size()-1]->get_whole_matrix(time,delta_time);
    int val = (time-start_time)/duration;
    return vec[val]->get_whole_matrix(time,delta_time);
  }
private:
  std::vector<Movement*> vec;
  float start_time;
  float duration;
};

EXPORT GameApi::MN GameApi::MovementNode::anim_enable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimEnable(next2, start_time, end_time));
}
EXPORT GameApi::MN GameApi::MovementNode::anim_disable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimDisable(next2, start_time, end_time));
}
EXPORT GameApi::MN GameApi::MovementNode::anim_choose(std::vector<MN> vec, float start_time, float duration)
{
  std::vector<Movement*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_move(e,vec[i]));
    }

  return add_move(e, new AnimChoose(vec2, start_time, duration));
}


EXPORT GameApi::MN GameApi::MovementNode::compress(MN next, float start_time, float end_time)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new CompressMovement(nxt, start_time, end_time));  
}
class TimeChangeMovement : public Movement
{
public:
  TimeChangeMovement(Movement *nxt, float d_time) : nxt(nxt), d_time(d_time) { }
  virtual void event(MainLoopEvent &e) { nxt->event(e); }
  virtual void frame(MainLoopEnv &e) { nxt->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { nxt->draw_frame(e); }

  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    time -= d_time;
    return nxt->get_whole_matrix(time, delta_time);
  }
  
private:
  Movement *nxt;
  float d_time;
};
class MatrixMovement : public Movement
{
public:
  MatrixMovement(Movement *next, Matrix m) : next(next), m(m) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }

  void set_matrix(Matrix mm) { m = mm; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return next->get_whole_matrix(time, delta_time)*m;
  }
private:
  Movement *next;
  Matrix m;
};
class EventActivateMovement : public Movement
{
public:
  EventActivateMovement(Movement *next, Movement *event2, float activate_time, float duration) : next(next), event2(event2), activate_time(activate_time), duration(duration) { 
    event_activated=false;
    collect = Matrix::Identity();
  }
  virtual void event(MainLoopEvent &e) { next->event(e); event2->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); event2->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); event2->draw_frame(e); }

  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    Matrix n = next->get_whole_matrix(time, delta_time);
    
    if (time > activate_time && time - delta_time < activate_time)
      {
	event_activated = true;
      }
    if (event_activated) { n = n * event2->get_whole_matrix(time-activate_time, delta_time) * collect; }
    else { n = n * collect; }
    if (event_activated && time - activate_time > duration) {
      event_activated = false;
      collect = collect * event2->get_whole_matrix(duration,delta_time);
    }
    return n;
  }
private:
  Movement *next;
  Movement *event2;
  float activate_time;
  mutable bool event_activated;
  float duration;
  mutable Matrix collect;
};
EXPORT GameApi::MN GameApi::MovementNode::event_activate(MN next, MN event, float activate_time, float duration)
{
  Movement *nxt = find_move(e, next);
  Movement *eve = find_move(e, event);
  return add_move(e, new EventActivateMovement(nxt,eve,activate_time, duration));
}
EXPORT GameApi::MN GameApi::MovementNode::trans2(MN next, float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Translate(dx,dy,dz)));  
}
EXPORT GameApi::MN GameApi::MovementNode::scale2(MN next, float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Scale(sx,sy,sz)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatex(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::XRotation(angle)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatey(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::YRotation(angle)));  
}
EXPORT GameApi::MN GameApi::MovementNode::rotatez(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::ZRotation(angle)));  
}
class TimeRepeatMovement : public Movement
{
public:
  TimeRepeatMovement(Movement *m, float start_time, float repeat_duration) : m(m), start_time(start_time), repeat_duration(repeat_duration) { }
  virtual void event(MainLoopEvent &e) { m->event(e); }
  virtual void frame(MainLoopEnv &e) { m->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { m->draw_frame(e); }

  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time < start_time) { return m->get_whole_matrix(time, delta_time); }
    float d = time - start_time;
    float dd = fmod(d,repeat_duration);
    return m->get_whole_matrix(start_time+dd, delta_time);
  }
private:
  Movement *m;
  float start_time;
  float repeat_duration;
};
EXPORT GameApi::MN GameApi::MovementNode::time_repeat(MN next, float start_time,
					       float repeat_duration)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TimeRepeatMovement(nxt, start_time, repeat_duration));
}

EXPORT GameApi::MN GameApi::MovementNode::change_time(MN next, float d_time)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TimeChangeMovement(nxt, d_time));  
}
void GameApi::MovementNode::set_matrix(MN n, M m)
{
  Movement *nn = find_move(e, n);
  Matrix mm = find_matrix(e, m);
  nn->set_matrix(mm);
}
EXPORT GameApi::M GameApi::MovementNode::get_matrix(MN n, float time, float delta_time)
{
  Movement *nn = find_move(e, n);
  Matrix m = nn->get_whole_matrix(time, delta_time);
  return add_matrix2(e, m);
}
class KeyEventML : public MainLoopItem
{
public:
  KeyEventML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int type, int ch, int button, float duration) : e(e), ev(ev), next(next), mn(mn), type(type), ch(ch), button(button), duration(duration) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    b=false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  bool compare(MainLoopEvent &e)
  {
    //std::cout << "Compare1: " << type << " " << ch << " " << button << std::endl;
    //std::cout << "Compare2: " << e.type << " " << e.ch << " " << e.button << std::endl;
    if (type!=-1 && e.type!=type) return false;
    if (ch!=-1 && e.ch != ch) return false;
    if (button!=-1 && e.button != button) return false;
    //std::cout << "COMPARE true" << std::endl;
    return true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    bool b = compare(env);
    if (b) {
      start_times.push_back(ev.mainloop_api.get_time());
    }

    next->handle_event(env);
  }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    
    int s = start_times.size();
    GameApi::M res = ev.matrix_api.identity();
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M res2 = ev.matrix_api.mult(m2,res);
    for(int i=0;i<s;i++)
      {
	float time = (ev.mainloop_api.get_time()-start_times[i])/100.0;
	GameApi::M mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
	res2 = ev.matrix_api.mult(res2, mat);
      }
    for(int i=0;i<s;i++)
      {
	float time = (ev.mainloop_api.get_time()-start_times[i])/100.0;
	if (time>duration)
	  {
	    start_times.erase(start_times.begin()+i);
	    i--; s--;
	  }
      }
    GameApi::M res2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res2);
    ev.shader_api.set_var(s1, "in_iMV", res2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res2);
    ev.shader_api.set_var(s2, "in_iMV", res2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res2);
    ev.shader_api.set_var(s3, "in_iMV", res2i);
    env.in_MV = find_matrix(e, res2);
    Matrix old_env = env.env;
    env.env = env.env * find_matrix(e,res2);
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;

  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int type, ch, button;
  bool b;
  std::vector<float> start_times;
  float duration;
};
class KeyPrinter : public MainLoopItem
{
public:
  KeyPrinter(MainLoopItem *next) : next(next) { }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    std::cout << "type: " << eve.type << " char: " << eve.ch << " button: " << eve.button << std::endl;
    next->handle_event(eve);
  }
  void execute(MainLoopEnv &env)
  {
    next->execute(env);
   }
private:
  MainLoopItem *next;
};

class JumpML : public MainLoopItem
{
public:
  JumpML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int key_jump, float height, float jump_duration) : e(e), ev(ev), next(next), key_jump(key_jump), height(height), jump_duration(jump_duration) { pos=0.0; jump_position=0.0; jump_going_up = false; jump_going_down=false; }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (eve.type==0x300 && ch==key_jump && !jump_going_up && !jump_going_down) { jump_going_up=true; }

  }

  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;

    if (jump_going_down)
      {
      pos-=height/(jump_duration/2.0)*env.delta_time;
      jump_position += env.delta_time;
      if (jump_position > jump_duration) {
	jump_going_down = false;
	pos = 0.0;
	jump_position = 0.0;
      }
      }
    if (jump_going_up) {
      pos+=height/(jump_duration/2.0)*env.delta_time;
      jump_position += env.delta_time;
      if (jump_position > jump_duration/2.0) {
	jump_going_down = true;
	jump_going_up = false;
      }
    }
    
    Matrix m = Matrix::Identity();
    m = Matrix::Translate(0.0,pos,0.0);

    GameApi::M mat = add_matrix2(e, m);
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);
    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2);/* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int key_jump;
  float height;
  float jump_duration;
  bool jump_going_up;
  bool jump_going_down;
  float pos;
  float jump_position;
};

class KeyMoveML : public MainLoopItem
{
public:
  KeyMoveML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int key_forward, int key_backward, float speed, float rot_speed, bool x, bool y, bool z, bool rot_x, bool rot_y, bool rot_z, float start, float end) : e(e), ev(ev), next(next),forward(key_forward), backward(key_backward),speed(speed),rot_speed(rot_speed), x(x),y(y),z(z),rot_x(rot_x), rot_y(rot_y), rot_z(rot_z), start(start),end(end) { 
    move_forward=false;
    move_backward=false;
    pos = 0.0;
    angle=0.0;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif


    if (eve.type==0x300 && ch==forward) { move_forward=true; }
    if (eve.type==0x301 && ch==forward) { move_forward=false; }
    if (eve.type==0x300 && ch==backward) { move_backward=true; }
    if (eve.type==0x301 && ch==backward) { move_backward=false; }
    next->handle_event(eve);
  }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;


    if (x||y||z)
      {
    if (move_forward) { pos+=speed;
      if (pos>end) {pos=end; }
    }
    if (move_backward) { pos-=speed; 
      if (pos<start) { pos=start; }
    }
      }
    if (rot_x||rot_y||rot_z)
      {
	if (move_forward) { angle+=rot_speed; }
	if (angle>end) { angle=end; }
	if (move_backward) { angle-=rot_speed; }
	if (angle<start) { angle=start; }
      }
    
    Matrix m = Matrix::Identity();
    if (x) {
      m = Matrix::Translate(pos,0.0,0.0);
      //e.in_MV = e.in_MV * m;
    }
    if (y) {
      m = Matrix::Translate(0.0,pos,0.0);
      //e.in_MV = e.in_MV * m;

    }
    if (z) {
      m = Matrix::Translate(0.0,0.0,pos);
      //e.in_MV = e.in_MV * m;
    }
    if (rot_x) {
      m = Matrix::XRotation(angle);
    }
    if (rot_y) {
      m = Matrix::YRotation(angle);
    }
    if (rot_z) {
      m = Matrix::ZRotation(angle);
    }
    GameApi::M mat = add_matrix2(e, m);
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);
    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2);/* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int forward;
  int backward;
  bool move_forward;
  bool move_backward;
  float pos;
  float angle;
  float speed;
  float rot_speed;
  bool x,y,z;
  bool rot_x, rot_y, rot_z;
  float start,end;
};

class MoveML : public MainLoopItem
{
public:
  MoveML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int clone_count, float time_delta) : e(e), ev(ev), next(next), mn(mn), clone_count(clone_count), time_delta(time_delta) 
  {
    firsttime = true;
    firsttime2 = false;
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    next->handle_event(env);
    Movement *move = find_move(e,mn);
    move->event(env);
  }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e,mn);
    move->frame(env);

    if (firsttime) {
      firsttime2 = true;
    }
    if (!firsttime && firsttime2)
      {
	reset_time();
	firsttime2 = false;
      }
    firsttime = false;
    for(int i=0;i<clone_count;i++)
      {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s11;
    s11.id = env.sh_texture_2d;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    float time = (env.time*1000.0-start_time)/100.0+i*time_delta;
    GameApi::M mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
    //Matrix mat_i = find_matrix(e, mat);
    //std::cout << mat_i << std::endl;
    GameApi::M m2 = add_matrix2(e, env.env);
    //std::cout << env.env << std::endl;
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
    //Matrix old_in_MV = env.in_MV;
    MainLoopEnv ee = env;
    ee.in_MV = find_matrix(e, mat2);

    //Matrix old_env = env.env;
    //float old_time = env.time;
    ee.env = find_matrix(e,mat2); /* * env.env*/;
    ee.time = env.time + i*time_delta/10.0;
    next->execute(ee);
    ev.shader_api.unuse(s3);
    //env.env = old_env;
    //env.time = old_time;
    //env.in_MV = old_in_MV;
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int clone_count;
  float time_delta;
  bool firsttime;
  bool firsttime2;
};


class ColorML : public MainLoopItem
{
public:
  ColorML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, int color_num, ColorChange *cc) : e(e), ev(ev), next(next), color_num(color_num), cc(cc) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    std::stringstream ss;
    ss << color_num;
    color_num2 = "color_array[" + ss.str() + "]";
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    next->handle_event(env);
  }
  void execute(MainLoopEnv &env)
  {
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    Color c2 = cc->get_whole_color(time, env.delta_time);
    //std::cout << color_num2 << " " << c2 << std::endl;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, color_num2, c2.rf(),c2.gf(), c2.bf(), c2.af());

    next->execute(env);
    ev.shader_api.unuse(s3);
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  int color_num;
  ColorChange *cc;
  std::string color_num2;
};


class KeyActivateML : public MainLoopItem
{
public:
  KeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int key, float duration) : e(e), ev(ev), next(next), mn(mn),key(key), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    key_pressed = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    bool start_anim = false;
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x300 && ch == key && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x301 && ch == key) { key_pressed = false; }

    if (start_anim) {
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
    }

    next->handle_event(eve);
  }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);


    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	anim_ongoing = false;
	collect = ev.matrix_api.mult(collect, ev.move_api.get_matrix(mn, duration, env.delta_time));
	
	if (key_pressed) { // repeat animation if key is being pressed down when anim stops
	  anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
	}

      }

    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  bool anim_ongoing;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
};



class CombKeyActivateML : public MainLoopItem
{
public:
  CombKeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN mn, int key, int key_2, float duration) : e(e), ev(ev), next(next), mn(mn),key(key), key_2(key_2), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    key_pressed = false;
    key2_pressed = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    bool start_anim = false;
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x300 && ch == key_2) { key2_pressed = true; }
    if (eve.type==0x300 && key2_pressed && ch == key && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x300 && key_pressed && ch == key_2 && !anim_ongoing) { start_anim = true; }
    if (eve.type==0x301 && ch == key) { key_pressed = false; }
    if (eve.type==0x301 && ch == key_2) { key2_pressed = false; }

    if (start_anim) {
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
    }

    next->handle_event(eve);
  }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);

    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	anim_ongoing = false;
	collect = ev.matrix_api.mult(collect, ev.move_api.get_matrix(mn, duration, env.delta_time));
	
	if (key_pressed && key2_pressed) { // repeat animation if key is being pressed down when anim stops
	  anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
	}

      }

    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  int key_2;
  bool anim_ongoing;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
  bool key2_pressed;
};




class TempKeyActivateML : public MainLoopItem
{
public:
  TempKeyActivateML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::MN nm, int key, float duration) : e(e), ev(ev), next(next), mn(nm),key(key), duration(duration)
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
    start_time2 = 0.0;
    anim_pos = 0.0;
    collect = ev.matrix_api.identity();
    anim_ongoing = false;
    anim_ongoing2 = false;
    key_pressed = false;
    key_canceled=false;
    start_anim_chain = false;
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &eve)
  {
    Movement *move = find_move(e, mn);
    move->event(eve);

    bool start_anim = false;
    bool start_anim2 = false;
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (eve.type==0x300 && ch == key && !anim_ongoing && !anim_ongoing2 && !key_pressed) { start_anim = true; } else if (eve.type==0x300 && ch==key && !key_pressed)
      {
	key_canceled=true;
      }
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x301 && ch == key && !anim_ongoing2 && !key_canceled) { key_pressed = false;  start_anim2 = true; } 
    if (eve.type==0x301 && ch==key) { key_canceled=false;
      //if (!anim_ongoing && !anim_ongoing2 && anim_pos>0.0) {
      //	anim_pos = duration;
      //	start_anim2 = true;
      //}
    }

    if (start_anim) {
      start_anim_chain = false;
      //std::cout << "start_anim" << std::endl;
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
      start_anim = false;
    }
    if (start_anim2 && !start_anim_chain) {
      start_anim_chain = true;
      //std::cout << "start_anim2" << std::endl;
      anim_ongoing = false;
      anim_ongoing2 = true; 
      start_time2 = ev.mainloop_api.get_time();  
      anim_pos_at_change = anim_pos;
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      time_at_change = std::min(time,duration);

      //      collect = ev.move_api.get_matrix(mn, anim_pos_at_change, ev.mainloop_api.get_delta_time());
      start_anim2 = false;
    }

    
    next->handle_event(eve);
  }
  void execute(MainLoopEnv &env)
  {
    Movement *move = find_move(e, mn);
    move->frame(env);


    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
    GameApi::M mat,m2,mat2;
    if (anim_ongoing) {
      float time = (ev.mainloop_api.get_time()-start_time)/100.0;
      mat = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      //mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos += env.delta_time;
      if (anim_pos > duration-env.delta_time) {
	//std::cout << "anim_ongoing=false" << std::endl;
	anim_ongoing = false;
	collect = ev.move_api.get_matrix(mn, time, ev.mainloop_api.get_delta_time());

	if (0)
	{
	  start_anim_chain = true;
	  //std::cout << "start_anim2" << std::endl;
	  anim_ongoing = false;
	  anim_ongoing2 = true; 
	  start_time2 = ev.mainloop_api.get_time();  
	  anim_pos_at_change = anim_pos;
	  float time = (ev.mainloop_api.get_time()-start_time)/100.0;
	  time_at_change = std::min(time,duration);
	}

	//anim_ongoing2 = true;
	//start_time2 = ev.mainloop_api.get_time();  
	//anim_pos_at_change = anim_pos;
      }
    } else if (anim_ongoing2) {
      float time = (ev.mainloop_api.get_time()-start_time2)/100.0;
      mat = ev.move_api.get_matrix(mn, time_at_change-time, ev.mainloop_api.get_delta_time());
      m2 = add_matrix2(e, env.env);
      mat2 = mat;
      //mat2 = ev.matrix_api.mult(mat2, collect);
      mat2 = ev.matrix_api.mult(mat2,m2);

      anim_pos -= env.delta_time;
      if (time_at_change - time - ev.mainloop_api.get_delta_time() < 0.0) {
	//std::cout << "anim_ongoing2=false" << std::endl;
	anim_ongoing2 = false;
	collect = ev.matrix_api.identity();
      }
    } else {
      m2 = add_matrix2(e, env.env);
      mat2 = ev.matrix_api.mult(collect,m2);
    }
    GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    ev.shader_api.set_var(s3, "in_iMV", mat2i);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    next->execute(env);
    ev.shader_api.unuse(s3);
    env.env = old_env;
    env.in_MV = old_in_MV;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  float start_time2;
  MainLoopItem *next;
  GameApi::MN mn;
  int key;
  bool anim_ongoing;
  bool anim_ongoing2;
  float anim_pos;
  float duration;
  GameApi::M collect;
  bool key_pressed;
  float anim_pos_at_change;
  float time_at_change;
  bool key_canceled;
  bool start_anim_chain;
};




class EnableML : public MainLoopItem
{
public:
  EnableML(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next, float start_time2, float end_time2) : e(e), ev(ev), next(next), start_time2(start_time2), end_time2(end_time2) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    if (time>=start_time2 && time<end_time2)
      {
	next->handle_event(env);
      }
  }
  void execute(MainLoopEnv &env)
  {
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    if (time>=start_time2 && time<end_time2)
      {
	next->execute(env);
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  MainLoopItem *next;
  float start_time2, end_time2;
};


class MoveMLArray : public MainLoopItem
{
public:
  MoveMLArray(GameApi::Env &e, GameApi::EveryApi &ev, std::vector<MainLoopItem *> next, std::vector<GameApi::MN> mn) : e(e), ev(ev), next(next), mn(mn) 
  { 
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }
  int shader_id() { return next[0]->shader_id(); }
  void handle_event(MainLoopEvent &env)
  {
    int s = std::min(next.size(), mn.size());
    for(int i=0;i<s;i++)
      {
	next[i]->handle_event(env);
      }
  }
  void execute(MainLoopEnv &env)
  {
    int s = std::min(next.size(), mn.size());
    for(int i=0;i<s;i++)
      {
	GameApi::SH s1;
	s1.id = env.sh_texture;
	GameApi::SH s2;
	s2.id = env.sh_array_texture;
	GameApi::SH s3;
	s3.id = env.sh_color;
	float time = (ev.mainloop_api.get_time()-start_time)/100.0;
	GameApi::M mat = ev.move_api.get_matrix(mn[i], time, ev.mainloop_api.get_delta_time());
	GameApi::M m2 = add_matrix2(e, env.env);
	GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
	GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1, "in_MV", mat2);
	ev.shader_api.set_var(s1, "in_iMV", mat2i);
	ev.shader_api.use(s2);
	ev.shader_api.set_var(s2, "in_MV", mat2);
	ev.shader_api.set_var(s2, "in_iMV", mat2i);
	ev.shader_api.use(s3);
	ev.shader_api.set_var(s3, "in_MV", mat2);
	ev.shader_api.set_var(s3, "in_iMV", mat2i);
	env.in_MV = find_matrix(e, mat2);
	
	Matrix old_env = env.env;
	env.env = find_matrix(e,mat2) * env.env;
	next[i]->execute(env);
	ev.shader_api.unuse(s3);
	env.env = old_env;
      }
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float start_time;
  std::vector<MainLoopItem *> next;
  std::vector<GameApi::MN> mn;
};

EXPORT GameApi::ML GameApi::MovementNode::key_printer_ml(GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyPrinter(item));
}
class RepeatML : public MainLoopItem
{
public:
  RepeatML(MainLoopItem *next, float duration) : next(next), duration(duration) { }
  virtual void execute(MainLoopEnv &e)
  {
    float time = e.time;
    float newtime = fmod(time, duration);
    MainLoopEnv ee = e;
    ee.time = newtime;
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e) { return next->handle_event(e); }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  float duration;

};
EXPORT GameApi::ML GameApi::MovementNode::repeat_ml(EveryApi &ev, GameApi::ML ml, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new RepeatML(item, duration/10.0));  
}
EXPORT GameApi::ML GameApi::MovementNode::move_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int clone_count, float time_delta)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new MoveML(e,ev,item, move, clone_count, time_delta));
}
EXPORT GameApi::ML GameApi::MovementNode::color_ml(EveryApi &ev, int color_num, ML ml, CC cc)
{
  MainLoopItem *item = find_main_loop(e, ml);
  ColorChange *cc2 = find_color(e, cc);
  return add_main_loop(e, new ColorML(e,ev,item, color_num, cc2));
}
EXPORT GameApi::ML GameApi::MovementNode::key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyActivateML(e,ev,item, move, key, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::comb_key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, int key_2, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new CombKeyActivateML(e,ev,item, move, key, key_2, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::temp_key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new TempKeyActivateML(e, ev, item, move, key, duration));
}
EXPORT GameApi::ML GameApi::MovementNode::move_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,true,false,false,false,false,false,start_x, end_x));
}
EXPORT GameApi::ML GameApi::MovementNode::jump_ml(EveryApi &ev, GameApi::ML ml, int key_jump, float height, float jump_duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new JumpML(e,ev,item,key_jump, height, jump_duration));
}
EXPORT GameApi::ML GameApi::MovementNode::move_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0, false,true,false,false,false,false,start_y,end_y));
}
EXPORT GameApi::ML GameApi::MovementNode::move_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,false,false,true,false,false,false,start_z,end_z));
}

EXPORT GameApi::ML GameApi::MovementNode::rot_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,true,false,false,start_x, end_x));
}
EXPORT GameApi::ML GameApi::MovementNode::rot_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,true,false,false,true,false,start_y,end_y));
}
EXPORT GameApi::ML GameApi::MovementNode::rot_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,false,false,true,start_z,end_z));
}

EXPORT GameApi::ML GameApi::MovementNode::enable_ml(EveryApi &ev, GameApi::ML ml, float start_time, float end_time)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new EnableML(e,ev,item, start_time, end_time));

}
EXPORT GameApi::ML GameApi::MovementNode::move_ml_array(EveryApi &ev, std::vector<GameApi::ML> ml, std::vector<GameApi::MN> move)
{
  int s = ml.size();
  std::vector<MainLoopItem*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_main_loop(e, ml[i]));
    }
  return add_main_loop(e, new MoveMLArray(e,ev, vec2, move));
}
EXPORT GameApi::ML GameApi::MovementNode::key_event(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int type, int ch, int button, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyEventML(e,ev,item, move,type,ch,button, duration));
}

EXPORT GameApi::ML GameApi::MovementNode::wasd(EveryApi &ev, GameApi::ML ml,
					GameApi::MN w, GameApi::MN a,
					GameApi::MN s, GameApi::MN d,
					float duration)
{
  GameApi::ML m1 = key_event(ev, ml, w, 0x300, 'w',-1, duration);
  GameApi::ML m2 = key_event(ev, m1, a, 0x300, 'a',-1, duration);
  GameApi::ML m3 = key_event(ev, m2, s, 0x300, 's',-1, duration);
  GameApi::ML m4 = key_event(ev, m3, d, 0x300, 'd',-1, duration);
  return m4;
}

GameApi::BB GameApi::PickingApi::pick_area(GameApi::EveryApi &ev, float mouse_x, float mouse_y, float radius, int scr_size_x, int scr_size_y)
{
  BB empty = ev.bool_bitmap_api.empty(scr_size_x, scr_size_y);
  BB circle = ev.bool_bitmap_api.circle(empty, mouse_x, mouse_y, radius);
  return circle;
}
class MatrixVolume : public VolumeObject
{
public:
  MatrixVolume(Bitmap<bool> &b, Matrix m) : b(b),m(m) { }
  bool Inside(Point v) const
  {
    std::cout << "P: " << v.x << " " << v.y << " " << v.z << std::endl;
    Point p2 = v * m;
    p2.x *=800.0;
    p2.y *=600.0;
    p2.x /= p2.z;
    p2.y /= p2.z;
    p2.x += 800/2;
    p2.y += 600/2;
    std::cout << "pick_pos " << p2.x << " " << p2.y << " " << p2.z << std::endl;
    if (p2.x>0.0 && p2.y>0.0 && p2.x<b.SizeX() && p2.y<b.SizeY())
      {
	bool bb = b.Map(p2.x,p2.y);
	return bb;
      }
    return false;
  }
private:
  Bitmap<bool> &b;
  Matrix m;
};
GameApi::O GameApi::PickingApi::pick_volume(M in_P, BB pick)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, pick)->bitmap;
  Matrix m = find_matrix(e, in_P);
  for(int i=0;i<16;i++) std::cout << m.matrix[i] << " ";
  std::cout << std::endl;
  return add_volume(e, new MatrixVolume(*bm, m));
}
bool GameApi::PickingApi::picked(O o, float x, float y, float z)
{
  VolumeObject *obj = find_volume(e, o);
  Point p(x,y,z);
  bool b = obj->Inside(p);
  return b;
}

class TreeLevelImpl : public TreeLevel
{
public:
  TreeLevelImpl(std::vector<Movement*> vec) : vec(vec) { }
  int num_childs() const { return vec.size(); }
  Matrix get_child(int i, float time) const
  {
    // TODO tree cannot use delta_time properly
    Matrix m = vec[i]->get_whole_matrix(time, 1.0);
    return m;
  }
private:
  std::vector<Movement*> vec;
};

GameApi::TL GameApi::TreeApi::level(std::vector<MN> vec)
{
  std::vector<Movement*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_move(e, vec[i]));
    }
  return add_tree_level(e, new TreeLevelImpl(vec2));
}
class TreeImpl : public TreeStack
{
public:
  TreeImpl(std::vector<TreeLevel*> vec) : vec(vec) { }
  int num_levels() const
  {
    return vec.size();
  }
  TreeLevel *get_level(int i) const
  {
    return vec[i];
  }
private:
  std::vector<TreeLevel*> vec;
};
GameApi::T GameApi::TreeApi::tree(std::vector<TL> vec)
{
  std::vector<TreeLevel*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_tree_level(e, vec[i]));
    }
  return add_tree(e, new TreeImpl(vec2));
}

class TreeMainLoop : public MainLoopItem
{
public:
  TreeMainLoop(GameApi::Env &e, GameApi::EveryApi &ev, TreeStack *t, std::vector<MainLoopItem*> vec) : e(e), ev(ev), next(vec), tree2(t) 
  {
    start_time = 0.0; //ev.mainloop_api.get_time();
  }
  int shader_id() { return -1; }
  void execute_one(MainLoopEnv &env, Matrix m, int level)
  {
	  GameApi::M m2 = add_matrix2(e, m);
	  GameApi::SH s1;
	  s1.id = env.sh_texture;
	  GameApi::SH s2;
	  s2.id = env.sh_array_texture;
	  GameApi::SH s3;
	  s3.id = env.sh_color;
	  //GameApi::M mat = ev.move_api.get_matrix(mn, time);
	  //GameApi::M m2 = add_matrix2(e, env.env);
	  GameApi::M mat2 = m2; //ev.matrix_api.mult(mat,m2);
	  GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
	  ev.shader_api.use(s1);
	  ev.shader_api.set_var(s1, "in_MV", mat2);
	  ev.shader_api.set_var(s1, "in_iMV", mat2i);
	  ev.shader_api.use(s2);
	  ev.shader_api.set_var(s2, "in_MV", mat2);
	  ev.shader_api.set_var(s2, "in_iMV", mat2i);
	  ev.shader_api.use(s3);
	  ev.shader_api.set_var(s3, "in_MV", mat2);
	  ev.shader_api.set_var(s3, "in_iMV", mat2i);
	  env.in_MV = find_matrix(e, mat2);
	  
	  Matrix old_env = env.env;
	  env.env = find_matrix(e,mat2);
	  int ss = next.size();
	  if (level<ss) {
	    next[level]->execute(env);
	  }
	  ev.shader_api.unuse(s3);
	  env.env = old_env;
  }
  void execute_recurse(MainLoopEnv &e, Matrix mm, int current_level)
  {
    execute_one(e, mm, current_level);
    if (current_level >= tree2->num_levels()-1) return;
    TreeLevel *lvl = tree2->get_level(current_level);
    int s2 = lvl->num_childs();
    float time = (ev.mainloop_api.get_time()-start_time)/100.0;
    for(int j=0;j<s2;j++)
      {
	Matrix m = lvl->get_child(j, time);
	Matrix m2 = m * mm;
	execute_recurse(e, m2, current_level+1);
      }    
  }
  void handle_event(MainLoopEvent &e)
  {
    int s = next.size();
    for(int i=0;i<s;i++)
      {
	next[i]->handle_event(e);
      }
  }
  void execute(MainLoopEnv &e)
  {
    execute_recurse(e, e.env, 0);
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> next;
  TreeStack *tree2;
  float start_time;
};

GameApi::ML GameApi::TreeApi::tree_ml(EveryApi &ev, T tree, std::vector<ML> vec)
{
  TreeStack *tree2 = find_tree(e, tree);
  std::vector<MainLoopItem*> vec2;
  int s=vec.size();
  for(int i=0;i<s;i++)
    {
      MainLoopItem* ii = find_main_loop(e, vec[i]);
      vec2.push_back(ii);
    }

  return add_main_loop(e, new TreeMainLoop(e,ev,tree2, vec2));
}

GameApi::P execute_one(GameApi::Env &e, GameApi::EveryApi &ev, const std::vector<GameApi::P> &v, Matrix m, int level)
{
  int s = v.size();
  if (level<s)
    return ev.polygon_api.matrix(v[level], add_matrix2(e,m));
  return ev.polygon_api.empty();
}
GameApi::P execute_recurse(GameApi::Env &e, GameApi::EveryApi &ev, const std::vector<GameApi::P> &v, Matrix mm, int current_level, TreeStack *tree2, float time)
{
  GameApi::P p0 = execute_one(e, ev, v, mm, current_level);
  if (current_level>=tree2->num_levels()-1) return p0;
  TreeLevel *lvl = tree2->get_level(current_level);
  int s2 = lvl->num_childs();
  std::vector<GameApi::P> vec;
  vec.push_back(p0);
  for(int j=0;j<s2;j++)
    {
      Matrix m = lvl->get_child(j,time);
      Matrix m2 = m * mm;
      GameApi::P p2 = execute_recurse(e, ev, v, m2, current_level+1, tree2, time);
      vec.push_back(p2);
    }
  return ev.polygon_api.or_array2(vec);
}

GameApi::P GameApi::TreeApi::tree_p(EveryApi &ev, T tree, std::vector<P> vec, float time)
{
  TreeStack *tree2 = find_tree(e, tree);
  return execute_recurse(e, ev, vec, Matrix::Identity(), 0, tree2, time); 
}

class MaterialForward : public Material
{
public:
  GameApi::ML call(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = mat(p.id);
    return ml;
  }
  GameApi::ML call_inst(GameApi::P p, GameApi::PTS pts)
  {
    GameApi::ML ml;
    ml.id = mat_inst(p.id,pts.id);
    return ml;
  }
  int mat(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::ML ml = mat2(p2);
    return ml.id;
  }
  int mat_inst(int p, int pts) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst(p2,p3);
    return ml.id;
  }
  int mat_inst2(int p, int pta) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTA p3;
    p3.id = pta;
    GameApi::ML ml = mat2_inst2(p2,p3);
    return ml.id;

  }
  int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat_inst_fade(p2,p3, flip, start_time, end_time);
    return ml.id;

  }
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};
class DefaultMaterial : public MaterialForward
{
public:
  DefaultMaterial(GameApi::EveryApi &ev) : ev(ev) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::PTS pts = ev.points_api.single_pts();
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev,p,pts);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev, p, pts);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml = ev.materials_api.render_instanced_ml_fade(ev, p, pts, flip, start_time, end_time);
    return ml;
  }

private:
  GameApi::EveryApi &ev;
};

class ShadowMaterial : public MaterialForward
{
public:
  ShadowMaterial(GameApi::EveryApi &ev, GameApi::P objs, std::vector<GameApi::BM> vec, Point pos, int sx, int sy, unsigned int dark_color, float mix, float mix2) : ev(ev), objs(objs), vec(vec), pos(pos), sx(sx), sy(sy), dark_color(dark_color), mix(mix), mix2(mix2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::BM bm2 = bm; //ev.bitmap_api.flip_y(bm);
    std::vector<GameApi::BM> vec2 = vec;
    vec2.push_back(bm2);
    //GameApi::P I10=ev.polygon_api.or_elem(objs,p); 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,p,vec2);

    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, I17, vec2.size()-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, ml2, mix);

    GameApi::ML I17a=ev.polygon_api.render_vertex_array_ml2_texture(ev,objs,vec);

    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a, mix);

    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml});
    return arr;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    std::vector<GameApi::BM> vec2 = vec;
    vec2.push_back(bm);
    //GameApi::P p2 = ev.polygon_api.or_elem(objs,p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,vec2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, I17,mix);

    GameApi::ML I17a=ev.materials_api.render_instanced_ml_texture(ev,objs,pts,vec);
    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a,mix);

    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, ml, vec2.size()-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml2});
    return arr;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst2(p.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  GameApi::P objs;
  std::vector<GameApi::BM> vec;
  Point pos;
  int sx, sy;
  unsigned int dark_color;
  float mix;
  float mix2;
};

class ShadowMaterial2 : public MaterialForward
{
public:
  ShadowMaterial2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P objs, Point pos, int sx, int sy, unsigned int dark_color, float mix, float mix2, int numtextures) : e(e), ev(ev), objs(objs), pos(pos), sx(sx), sy(sy), dark_color(dark_color), mix(mix), mix2(mix2), numtextures(numtextures) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {

    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::P p2 = ev.polygon_api.texture_add(p, bm);
    //GameApi::BM bm2 = bm; //ev.bitmap_api.flip_y(bm);
    //std::vector<GameApi::BM> vec2 = vec;
    //vec2.push_back(bm2);
    //GameApi::P I10=ev.polygon_api.or_elem(objs,p); 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture2(ev,p2);

    FaceCollection *coll = find_facecoll(e, p2);
    int count = coll->NumTextures();
    if (numtextures>count) count=numtextures;
    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, I17, count-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, ml2, mix);

    GameApi::ML I17a=ev.polygon_api.render_vertex_array_ml2_texture2(ev,objs);

    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a, mix);

    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml});
    return arr;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM bm = ev.polygon_api.shadow_map(ev, objs, pos.x,pos.y,pos.z, sx,sy);
    GameApi::P p2 = ev.polygon_api.texture_add(p, bm);
    //std::vector<GameApi::BM> vec2 = vec;
    //vec2.push_back(bm);
    //GameApi::P p2 = ev.polygon_api.or_elem(objs,p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture2(ev,p,pts);
    GameApi::ML ml=ev.polygon_api.texture_many_shader(ev, I17,mix);

    GameApi::ML I17a=ev.materials_api.render_instanced_ml_texture2(ev,objs,pts);
    GameApi::ML mla=ev.polygon_api.texture_many_shader(ev, I17a,mix);

    FaceCollection *coll = find_facecoll(e, p2);
    int count = coll->NumTextures();
    if (numtextures>count) count=numtextures;
    GameApi::ML ml2 = ev.polygon_api.shadow_shader(ev, ml, count-1, pos.x,pos.y,pos.z,dark_color,mix2);
    GameApi::ML arr = ev.mainloop_api.array_ml(std::vector<GameApi::ML>{mla,ml2});
    return arr;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst2(p.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    // TODO
    GameApi::ML ml;
    ml.id=0;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::P objs;
  std::vector<GameApi::BM> vec;
  Point pos;
  int sx, sy;
  unsigned int dark_color;
  float mix;
  float mix2;
  int numtextures;
};


class FogMaterial : public MaterialForward
{
public:
  FogMaterial(GameApi::EveryApi &ev, Material *next, float fog_dist, unsigned int dark_color, unsigned int light_color) : ev(ev), next(next),fog_dist(fog_dist), dark_color(dark_color), light_color(light_color) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    ml = ev.polygon_api.fog_shader(ev, ml,fog_dist, dark_color, light_color);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    ml = ev.polygon_api.fog_shader(ev, ml, fog_dist, dark_color, light_color);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  float fog_dist;
  unsigned int dark_color, light_color;
};

class SkeletalMaterial : public MaterialForward
{
public:
  SkeletalMaterial(GameApi::EveryApi &ev) : ev(ev) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    // these are in skeletal_bind_materiaal
    //GameApi::P pp = ev.polygon_api.or_array2(vec);
    //GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);

    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(),vecattribs);
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    //GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //std::cout << "mat2_inst" << std::endl;
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //std::cout << "mat2_inst" << std::endl;
    std::vector<int> vecattribs;
    vecattribs.push_back(AttrPart);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(p,false,std::vector<int>(), vecattribs);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml_fade(ev, va, pta, flip, start_time, end_time);
    return ml;
  }

private:
  GameApi::EveryApi &ev;
};


class TextureArrayMaterial : public MaterialForward
{
public:
  TextureArrayMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, int sx, int sy, float mix) : ev(ev), vec(vec),sx(sx),sy(sy), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;
    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;

    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);

    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta,flip,start_time,end_time);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> vec;
  int sx,sy;
  float mix;
};

class ForwardRenderToTextureId : public MainLoopItem
{
public:
  ForwardRenderToTextureId(MainLoopItem *next, TextureID *id) : next(next), id(id) { }
  virtual void execute(MainLoopEnv &e)
  {
    id->render(e);
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    id->handle_event(e);
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  TextureID *id;
};
GameApi::ML GameApi::TextureApi::forward_to_txid(ML mainloop, TXID id)
{
  MainLoopItem *next = find_main_loop(e, mainloop);
  TextureID *txid = find_txid(e, id);
  return add_main_loop(e, new ForwardRenderToTextureId(next, txid));
}

class TextureIDMaterial : public MaterialForward
{
public:
  TextureIDMaterial(GameApi::EveryApi &ev, GameApi::TXID id, float mix) : ev(ev), txid(id), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {

    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    //GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    // GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    //GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::TXID I15 = txid;
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta, flip, start_time, end_time);
    GameApi::ML I17a = ev.texture_api.forward_to_txid(I17,I15);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17a,mix);
    return I18;
  }
private:
  GameApi::EveryApi &ev;
  GameApi::TXID txid;
  float mix;
};

class TextureMaterial : public MaterialForward
{
public:
  TextureMaterial(GameApi::EveryApi &ev, GameApi::BM bm, float mix) : ev(ev), bm(bm), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    GameApi::BM I13=bm; //ev.bitmap_api.chessboard(20,20,2,2,ffffffff,ff888888);
    GameApi::TX I14=ev.texture_api.tex_bitmap(I13);
    GameApi::TXID I15=ev.texture_api.prepare(I14);
    GameApi::VA I16=ev.texture_api.bind(I12,I15);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml_fade(ev,I16,pta, flip, start_time, end_time);
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  GameApi::BM bm;
  float mix;
};


class ManyTextureMaterial : public MaterialForward
{
public:
  ManyTextureMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> bm, float mix) : ev(ev), bm(bm), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    // END OF NOT WORKING
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> bm;
  float mix;
};

class CubemapTextureMaterial : public MaterialForward
{
public:
  CubemapTextureMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> bm, float mix, float mix2) : ev(ev), bm(bm), mix(mix),mix2(mix2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_cubemap(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.texture_cubemap_shader(ev, I17, mix,mix2);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_cubemap(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.texture_cubemap_shader(ev, I17,mix,mix2);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    // END OF NOT WORKING
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> bm;
  float mix;
  float mix2;
};


class ManyTextureMaterial2 : public MaterialForward
{
public:
  ManyTextureMaterial2(GameApi::EveryApi &ev, float mix) : ev(ev), mix(mix) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture2(ev,I10);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17, mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture2(ev,p,pts);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
    return I18;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    // NOT WORKING
#if 0
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    std::vector<GameApi::BM> I13=bm;
    std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    GameApi::ML I17=ev.materials_api.render_instanced2_ml(ev,I16,pta);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    // END OF NOT WORKING
    GameApi::ML I18;
    I18.id = 0;
    return I18;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
#if 0
    GameApi::P I10=p; //ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
    //GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    //GameApi::VA I12=ev.polygon_api.create_vertex_array(I10,true);
    //std::vector<GameApi::BM> I13=bm;
    //std::vector<GameApi::TXID> I15 = ev.texture_api.prepare_many(ev, I13);
    //GameApi::VA I16=ev.texture_api.bind_many(I12,I15);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_fade_texture(ev,I10,pts, flip, start_time, end_time,bm);
    GameApi::ML I18=ev.polygon_api.texture_many_shader(ev, I17,mix);
#endif
    GameApi::ML I18;
    I18.id =0;
    return I18;
  }

private:
  GameApi::EveryApi &ev;
  float mix;
};



class BrashMetal : public MaterialForward
{
public:
  BrashMetal(GameApi::EveryApi &ev, Material *next, int count=80000, bool web=true) : ev(ev), next(next), count(count),web(web) { }

  virtual GameApi::ML mat2(GameApi::P I4) const
  {
    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst(I2,I5,I7);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I4 = ev.polygon_api.static_instancing(ev, p, pts);

    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst(I2,I5,I7);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;



    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    //return ml;

    // TODO how to implement instancing
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    return ml;
    // TODO how to implement instancing
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I4 = ev.polygon_api.static_instancing(ev, p, pts);

    GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
    GameApi::P I2=ev.polygon_api.sphere(I1,5,2,2);
    //PT I3=ev.point_api.point(0.0,0.0,0.0);
    //P I4=ev.polygon_api.torus2(ev,80,18,I3,400,80);
    GameApi::PTS I5=ev.points_api.random_mesh_quad_instancing(ev,I4,count);
    GameApi::MT I6=ev.materials_api.def(ev);
    GameApi::MT I7=ev.materials_api.snow(ev,I6);
    if (web) {
      I7=ev.materials_api.web(ev,I7);
    }
    GameApi::ML I9=ev.materials_api.bind_inst_fade(I2,I5,I7, flip, start_time, end_time);
    //PT I10=ev.point_api.point(0.0,0.0,0.0);
    //P I11=ev.polygon_api.torus2(ev,80,18,I10,400,80);
    GameApi::MT I12=ev.materials_api.def(ev);
    GameApi::MT I13=ev.materials_api.snow(ev,I12);
    GameApi::MT I14=ev.materials_api.web(ev,I13);
    GameApi::ML I15=ev.materials_api.bind(I4,I14);
    GameApi::ML I16=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I9,I15});
    return I16;



    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    //return ml;

    // TODO how to implement instancing
  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  int count;
  bool web;
};

class ShadingMaterial1 : public MaterialForward
{
public:
  ShadingMaterial1(GameApi::EveryApi &ev, Material *next, float mix_val, float mix_val2) : ev(ev), next(next), mix_val(mix_val), mix_val2(mix_val2) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat(p4.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat_inst(p4.id, pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p4.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    GameApi::P p4 = ev.polygon_api.mix_color(p3,p,mix_val2);

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p4.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  float mix_val;
  float mix_val2;
};

class ShadingMaterial2 : public MaterialForward
{
public:
  ShadingMaterial2(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3) : ev(ev), next(next), color1(color1), color2(color2), color3(color3) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
};


class SnowMaterial : public MaterialForward
{
public:
  SnowMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3, float mix_val) : ev(ev), next(next), color1(color1), color2(color2), color3(color3), mix_val(mix_val) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat(p3.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst(p3.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p3.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,mix_val);
    //GameApi::P p4 = ev.polygon_api.max_color(p3,p);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p3.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
  float mix_val;
};

class PhongMaterial : public MaterialForward
{
public:
  PhongMaterial(GameApi::EveryApi &ev, Material *next, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow) : ev(ev), next(next), light_dir_x(light_dir_x), light_dir_y(light_dir_y), light_dir_z(light_dir_z), ambient(ambient), highlight(highlight), pow(pow) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat(p1.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst(p1.id, pts.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p1.id, pta.id);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p1.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.phong_shader(ev, ml, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float light_dir_x, light_dir_y, light_dir_z;
  unsigned int ambient, highlight;
  float pow;
};



class DynLightsMaterial : public MaterialForward
{
public:
  DynLightsMaterial(GameApi::EveryApi &ev, Material *next, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point) : ev(ev), next(next), light_pos_x(light_pos_x), light_pos_y(light_pos_y), light_pos_z(light_pos_z), dist(dist), dyn_point(dyn_point) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    //GameApi::P p00 = ev.polygon_api.smooth_normals2(p0);
    //GameApi::P p1 = ev.polygon_api.color(p0, 0xff000000);

    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML sh = ev.polygon_api.dyn_lights_shader(ev, ml, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float light_pos_x, light_pos_y, light_pos_z;
  float dist;
  int dyn_point;
};



class FlatMaterial : public MaterialForward
{
public:
  FlatMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color1, unsigned int color2, unsigned int color3) : ev(ev), next(next), color1(color1), color2(color2), color3(color3) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat(p1.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst(p1.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p1.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p1.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, color1, color2, color3);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color1, color2, color3;
};

class BloomMaterial : public MaterialForward
{
public:
  BloomMaterial(GameApi::EveryApi &ev, Material *next, GameApi::BM bm, float r_cut, float g_cut, float b_cut, float pixel_x, float pixel_y) : ev(ev), next(next),bm(bm),r_cut(r_cut), g_cut(g_cut), b_cut(b_cut), pixel_x(pixel_x), pixel_y(pixel_y) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    // TODO next material support.
    // requires adding m_bind/m_bind_inst/m_bind_inst2/m_bind_inst_fade to Material class.
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind(p,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
/*
   GameApi::P I1=ev.polygon_api.fullscreen_quad(ev);
   GameApi::P I2=ev.polygon_api.texcoord_manual(I1,0.0,0.0,1.0,0.0,1.0,1.0,0.0,1.0);
   GameApi::P I3=p; 
   //GameApi::P I4=ev.polygon_api.color_from_normals(I3);
   //GameApi::ML I5=ev.polygon_api.render_vertex_array_ml2(ev,I3);
   GameApi::ML I5;
   I5.id = next->mat(I3.id);
   GameApi::ML II1 = ev.move_api.move_ml(ev, I5, move, 1,10.0);
   GameApi::TXID I1a=ev.fbo_api.fbo_ml(ev,II1,800,600,false);
   GameApi::MT I9=ev.materials_api.textureid(ev,I1a,0.9);
   GameApi::ML I10=ev.materials_api.bind(I2,I9);
   GameApi::MN I11=ev.move_api.empty();
   GameApi::MN I12=ev.move_api.trans2(I11,0,0,-0.001);
   GameApi::ML I13=ev.move_api.move_ml(ev,I10,I12,1,10.0);
   GameApi::ML I14=ev.sprite_api.turn_to_2d(ev,I13,0.0,0.0,800.0,600.0);
   GameApi::P I15=ev.polygon_api.fullscreen_quad(ev);
   GameApi::P I16=ev.polygon_api.texcoord_manual(I15,0.0,0.0,1.0,0.0,1.0,1.0,0.0,1.0);
   GameApi::P I17=p;
   //GameApi::ML I19=ev.polygon_api.render_vertex_array_ml2(ev,I17);
   GameApi::ML I19;
   I19.id = next->mat(I17.id);
   GameApi::TXID I2a=ev.fbo_api.fbo_ml(ev,I19,800,600,false);
   GameApi::MT I23=ev.materials_api.textureid(ev,I2a,0);
   GameApi::ML I24=ev.materials_api.bind(I16,I23);
   GameApi::ML I25=ev.polygon_api.bloom1_shader(ev,I24,0.4,0.4,0.4);
   GameApi::ML I26=ev.polygon_api.blur_shader(ev,I25,5,0);
   GameApi::ML I27=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I14,I26});
   return I27;
*/
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst(p,pts,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst2(p,pta,I10);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::BM I9 = bm;
    GameApi::MT I10=ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{I9},0);
    GameApi::ML I11=ev.materials_api.bind_inst_fade(p,pts,I10,flip,start_time,end_time);
    GameApi::SBM I15=ev.polygon_api.texture_sbm();
    GameApi::SBM I16=ev.polygon_api.texture_sbm();
    GameApi::SBM I17=ev.polygon_api.bloom_cut_sbm(I16,r_cut,g_cut,b_cut);
    GameApi::SBM I18=ev.polygon_api.blur_sbm(I17,pixel_x,0);
    GameApi::SBM I19=ev.polygon_api.blur_sbm(I18,0,pixel_y);
    GameApi::SBM I20=ev.polygon_api.combine_sbm(I15,I19);
    GameApi::ML I21=ev.polygon_api.sbm_texture(ev,I11,I20);
    return I21;

  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::BM bm;
  float r_cut, g_cut, b_cut;
  float pixel_x, pixel_y;
};

class FurMaterial : public MaterialForward
{
public:
  FurMaterial(GameApi::EveryApi &ev, Material *next, GameApi::PT center, float dist, float max_angle, int count, float size, int cone_numfaces) : ev(ev), next(next), center(center), dist(dist), max_angle(max_angle), count(count), size(size), cone_numfaces(cone_numfaces) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::PTS pts = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat(pl2.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::PTS pts2 = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts2, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst(pl2.id, pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::PTS pts = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst2(pl2.id, pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::PTS pts2 = ev.points_api.random_mesh_quad_instancing(ev, p, count);
    GameApi::LI li = ev.lines_api.fur(pts2, center, dist);
    GameApi::LI li2 = ev.lines_api.random_angle(li, max_angle);
    GameApi::P  pl = ev.polygon_api.line_to_cone(ev, li2, size, cone_numfaces);
    GameApi::P  pl2 = ev.polygon_api.or_elem(pl, p);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(pl2.id, pts.id, flip, start_time, end_time);
    return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  GameApi::PT center;
  float dist;
  float max_angle;
  int count;
  float size;
  int cone_numfaces;
};
class Marble : public MaterialForward
{
public:
  Marble(GameApi::EveryApi &ev, Material *next, int count, float cubesize) : ev(ev), next(next), count(count), cubesize(cubesize) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);
    
        GameApi::ML ml;
    ml.id = next->mat(I7.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst(I7.id, pts.id);
    return ml;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst2(I7.id, pta.id);

    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P I1=ev.polygon_api.cube(-cubesize,cubesize,-cubesize,cubesize,-cubesize,cubesize);
    GameApi::MS I2=ev.matrices_api.ms_random_rot(0,1,0,count);
    GameApi::P I3=p;
    GameApi::PTS I4=ev.points_api.random_mesh_quad_instancing(ev,I3,count);
    GameApi::MS I5=ev.matrices_api.from_points(I4);
    GameApi::MS I6=ev.matrices_api.mult_array(I2,I5);
    GameApi::P I7=ev.polygon_api.static_instancing_matrix(ev,I1,I6);

    GameApi::ML ml;
    ml.id = next->mat_inst_fade(I7.id, pts.id, flip, start_time, end_time);

    return ml;
  }
  
private:
  GameApi::EveryApi &ev;
  Material *next;
  int count;
  float cubesize;
};

class ChooseColorMaterial : public MaterialForward
{
public:
  ChooseColorMaterial(GameApi::EveryApi &ev, Material *next, unsigned int color, float mix_val) : ev(ev), next(next),color(color), mix_val(mix_val) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.choose_color_shader(ev, ml, color, mix_val);
    return sh;

  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  unsigned int color;
  float mix_val;
};

EXPORT GameApi::MT GameApi::MaterialsApi::def(EveryApi &ev)
{
  return add_material(e, new DefaultMaterial(ev));
}
EXPORT GameApi::MT GameApi::MaterialsApi::skeletal(EveryApi &ev)
{
  return add_material(e, new SkeletalMaterial(ev));
}
EXPORT GameApi::MT GameApi::MaterialsApi::fur(EveryApi &ev, MT nxt, PT center, float dist, float max_angle, int count, float size, int cone_numfaces)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FurMaterial(ev, mat, center, dist, max_angle, count, size, cone_numfaces));
}

EXPORT GameApi::MT GameApi::MaterialsApi::bloom(EveryApi &ev, MT nxt, BM bm, float r_cut, float g_cut, float b_cut, float pixel_x, float pixel_y)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new BloomMaterial(ev, mat, bm, r_cut, g_cut, b_cut, pixel_x, pixel_y));
}

				      
EXPORT GameApi::MT GameApi::MaterialsApi::texture(EveryApi &ev, BM bm, float mix)
{
  return add_material(e, new TextureMaterial(ev, bm,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::textureid(EveryApi &ev, TXID txid, float mix)
{
  return add_material(e, new TextureIDMaterial(ev, txid,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_many(EveryApi &ev, std::vector<BM> vec, float mix)
{
  return add_material(e, new ManyTextureMaterial(ev, vec,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_cubemap(EveryApi &ev, std::vector<BM> vec, float mix, float mix2)
{
  return add_material(e, new CubemapTextureMaterial(ev, vec,mix,mix2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::texture_many2(EveryApi &ev, float mix)
{
  return add_material(e, new ManyTextureMaterial2(ev,mix));
}


EXPORT GameApi::MT GameApi::MaterialsApi::texture_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy, float mix)
{
  return add_material(e, new TextureArrayMaterial(ev, vec, sx,sy,mix));
}
EXPORT GameApi::MT GameApi::MaterialsApi::snow(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3, float mix_val)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new SnowMaterial(ev, mat, color1, color2, color3, mix_val));
}
EXPORT GameApi::MT GameApi::MaterialsApi::phong(EveryApi &ev, MT nxt, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new PhongMaterial(ev, mat, light_dir_x, light_dir_y, light_dir_z, ambient, highlight, pow));
}
EXPORT GameApi::MT GameApi::MaterialsApi::fog(EveryApi &ev, MT nxt, float fog_dist, unsigned int dark_color, unsigned int light_color)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FogMaterial(ev, mat, fog_dist, dark_color, light_color));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shadow(EveryApi &ev, GameApi::P p, std::vector<BM> vec, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2)
{
  return add_material(e, new ShadowMaterial(ev, p, vec, Point(p_x,p_y,p_z),sx,sy , dark_color,mix,mix2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shadow2(EveryApi &ev, GameApi::P p, float p_x, float p_y, float p_z, int sx, int sy, unsigned int dark_color, float mix, float mix2, int numtextures)
{
  return add_material(e, new ShadowMaterial2(e,ev, p, Point(p_x,p_y,p_z),sx,sy , dark_color,mix,mix2,numtextures));
}
EXPORT GameApi::MT GameApi::MaterialsApi::dyn_lights(EveryApi &ev, MT nxt, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new DynLightsMaterial(ev, mat, light_pos_x, light_pos_y, light_pos_z, dist, dyn_point));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shading1(EveryApi &ev, MT nxt, float mix_val, float mix_val2)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ShadingMaterial1(ev, mat, mix_val,mix_val2));
}
EXPORT GameApi::MT GameApi::MaterialsApi::shading2(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ShadingMaterial2(ev, mat, color1, color2, color3));
}
EXPORT GameApi::MT GameApi::MaterialsApi::flat(EveryApi &ev, MT nxt, unsigned int color1, unsigned int color2, unsigned int color3)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new FlatMaterial(ev, mat, color1, color2, color3));
}
EXPORT GameApi::MT GameApi::MaterialsApi::choose_color(EveryApi &ev, MT nxt, unsigned int color, float mix_val)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new ChooseColorMaterial(ev, mat,color,mix_val));
}
EXPORT GameApi::MT GameApi::MaterialsApi::brashmetal(EveryApi &ev, MT nxt, int count, bool web)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new BrashMetal(ev, mat, count, web));
}
EXPORT GameApi::MT GameApi::MaterialsApi::marble(EveryApi &ev, MT nxt, int count, float cubesize)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new Marble(ev, mat, count, cubesize));
}

#if 0
EXPORT GameApi::ML GameApi::MaterialsApi::snow(EveryApi &ev, P p)
{
  P p0 = ev.polygon_api.recalculate_normals(p);
  P p1 = ev.polygon_api.color_from_normals(p0);
  P p2 = ev.polygon_api.color_grayscale(p1);
  P p3 = ev.polygon_api.mix_color(p1,p2,0.5);
  VA va = ev.polygon_api.create_vertex_array(p3,false);
  ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  ML sh = ev.polygon_api.shading_shader(ev, ml, 0xffaaaaaa, 0xffeeeeee, 0xffffffff);
  return sh;
}
#endif

class WebMaterial : public MaterialForward
{
public:
  WebMaterial(GameApi::EveryApi &ev, Material *next, float val, float linewidth, unsigned int color) : ev(ev),next(next),val(val),linewidth(linewidth),color(color) {}
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    g_low->ogl->glLineWidth(linewidth);
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_ml2(ev,I5);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat(I8.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    g_low->ogl->glLineWidth(linewidth);
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml3(ev,I5,pts);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst(I8.id,pts.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    //GameApi::ML I12=ev.mainloop_api.prepare_pts(I11,pts);
    return I11;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    g_low->ogl->glLineWidth(linewidth);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);
    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml2(ev,I5,pta);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst2(I8.id,pta.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    g_low->ogl->glLineWidth(linewidth);
    GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,color);
    GameApi::LI I5=ev.lines_api.line_pos_mult(val,I4);

    //GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml2(ev,I5,pta);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst_fade(I8.id,pts.id, flip, start_time, end_time);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

private:
  GameApi::EveryApi &ev;
  Material *next;
  float val;
  float linewidth;
  unsigned int color;
};

EXPORT GameApi::MT GameApi::MaterialsApi::web(EveryApi &ev, MT nxt, float val, float linewidth, unsigned int color)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new WebMaterial(ev,mat,val,linewidth,color));
}


#if 0
EXPORT GameApi::ML GameApi::MaterialsApi::web(EveryApi &ev, P p)
{

  // TODO: this might require std::function<ML(ev,P)> parameter, but
  // builder can't do it yet.

  P I2=p;
LI I3=ev.lines_api.from_polygon(I2);
LI I4=ev.lines_api.change_color(I3,0xff000000);
LLA I5=ev.lines_api.prepare(I4);
ML I6=ev.lines_api.render_ml(ev,I5);
 P I8=p; 
VA I9=ev.polygon_api.create_vertex_array(I8,true);
ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
ML I11=ev.mainloop_api.array_ml(std::vector<ML>{I6,I10});
 return I11;
}

#endif

EXPORT GameApi::ML GameApi::MaterialsApi::bind(P p, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat(p.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst2(P p, PTA pta, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat_inst2(p.id,pta.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst(P p, PTS pts, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat_inst(p.id,pts.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
EXPORT GameApi::ML GameApi::MaterialsApi::bind_inst_fade(P p, PTS pts, MT mat, bool flip, float start_time, float end_time)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat_inst_fade(p.id,pts.id, flip, start_time, end_time);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
class RenderInstanced : public MainLoopItem
{
public:
  RenderInstanced(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    if (firsttime) { obj2->Prepare(); }
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,false);
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (firsttime || shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	//ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
      }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    fade = false;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;

};

class RenderInstancedTex : public MainLoopItem
{
public:
  RenderInstancedTex(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time, std::vector<GameApi::BM> bm) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time),bm(bm)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,true);
	std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev,bm);
	va = ev.texture_api.bind_many(va, id);
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  std::vector<GameApi::BM> bm;
};


class RenderInstancedCubemap : public MainLoopItem
{
public:
  RenderInstancedCubemap(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time, std::vector<GameApi::BM> bm) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time),bm(bm)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,true);

	GameApi::BM right,left,top,bottom,back,front;
	right.id = 0;
	left.id = 0;
	top.id = 0;
	bottom.id = 0;
	back.id = 0;
	front.id = 0;
	if (bm.size()>0) right = bm[0];
	if (bm.size()>1) left = bm[1];
	if (bm.size()>2) top = bm[2];
	if (bm.size()>3) bottom = bm[3];
	if (bm.size()>4) back = bm[4];
	if (bm.size()>5) front = bm[5];
	if (left.id==0) left=right;
	if (top.id==0) top=right;
	if (bottom.id==0) bottom=right;
	if (back.id==0) back=right;
	if (front.id==0) front=right;
	GameApi::TXID id = ev.texture_api.prepare_cubemap(ev, right,left,top,bottom,back,front);
	va = ev.texture_api.bind_cubemap(va, id);
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
  std::vector<GameApi::BM> bm;
};



class RenderInstancedTex2 : public MainLoopItem
{
public:
  RenderInstancedTex2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), p(p), pts(pts), fade(fade), flip(flip), start_time(start_time), end_time(end_time)  { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    obj2->HandleEvent(e);
  }
  void execute(MainLoopEnv &e)
  {
    PointsApiPoints *obj2 = find_pointsapi_points(env, pts);
    bool changed = obj2->Update(e);
    // MainLoopEnv ee = e;
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,true);
	std::vector<GameApi::BM> bm;
	int s = ev.polygon_api.p_num_textures(p);
	for(int i=0;i<s;i++) {
	  ev.polygon_api.p_gen_texture(p,i);
	  bm.push_back(ev.polygon_api.p_texture(p,i));
	}
	std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev,bm);
	va = ev.texture_api.bind_many(va, id);
      }
    if (changed)
      {
	ev.points_api.update_from_data(pta, pts);
      }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced::USESHADER" << std::endl;
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime || changed) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.NumPoints(pts);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }

    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
};



class RenderInstanced2 : public MainLoopItem
{
public:
  RenderInstanced2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::VA va, GameApi::PTA pta, bool fade, bool flip, float start_time, float end_time) : env(e), ev(ev), va(va), pta(pta), fade(fade), flip(flip), start_time(start_time), end_time(end_time) { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	    u_f = ev.uber_api.f_texture(u_f);
	  }

	if (ev.polygon_api.is_array_texture(va))

	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
	    if (e.us_vertex_shader==-1)
	    		u_v = ev.uber_api.v_texture_arr(u_v);
	    if (e.us_fragment_shader==-1)
	    		u_f = ev.uber_api.f_texture_arr(u_f);
	       }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_colour(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_colour(u_f);
	  }
      }
#endif
    if (shader.id==-1)
      {
	//std::cout << "RenderInstanced2::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	if (e.sfo_id==-1)
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//std::cout << "RenderInstanced2::USESHADER" << std::endl;
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "in_POS", e.in_POS);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime) {
      firsttime = false;
      //pta = ev.points_api.prepare(pts);
      //va = ev.polygon_api.create_vertex_array(p,false);
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    int hide_n = -1;
    if (fade)
      {
	int start_n = ev.points_api.num_points(pta);
	int end_n = 0;
	if (flip) { std::swap(start_n, end_n); } 

	float time = e.time*10.0;
	if (time < start_time) { hide_n = start_n; }
	else if (time > end_time) { hide_n = end_n; }
	else {
	  float d = time - start_time;
	  d/=end_time-start_time;
	  d*=float(end_n-start_n);
	  d+=float(start_n);
	  hide_n = (int)d;
	}
      }
    
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh, hide_n);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  //GameApi::P p;
  //GameApi::PTS pts;
  bool firsttime;
  GameApi::VA va;
  GameApi::PTA pta;
  GameApi::SH shader;
  bool fade, flip;
  float start_time, end_time;
};

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml(GameApi::EveryApi &ev, P p, PTS pts)
{
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts, false,false,0.0,0.0));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_texture(GameApi::EveryApi &ev, P p, PTS pts, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedTex(e, ev, p,pts, false,false,0.0,0.0,bm));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_cubemap(GameApi::EveryApi &ev, P p, PTS pts, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedCubemap(e, ev, p,pts, false,false,0.0,0.0,bm));
}

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_texture2(GameApi::EveryApi &ev, P p, PTS pts)
{
  return add_main_loop(e, new RenderInstancedTex2(e, ev, p,pts, false,false,0.0,0.0));
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_fade(GameApi::EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time)
{
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts, true,flip,start_time,end_time));  
}
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_fade_texture(GameApi::EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time, std::vector<BM> bm)
{
  return add_main_loop(e, new RenderInstancedTex(e, ev, p,pts, true,flip,start_time,end_time,bm));  
}


EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced2_ml(GameApi::EveryApi &ev, VA va, PTA pta)
{
  return add_main_loop(e, new RenderInstanced2(e, ev, va,pta,false,false,0.0,0.0));
}

EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced2_ml_fade(GameApi::EveryApi &ev, VA va, PTA pta, bool flip, float start_time, float end_time)
{
  return add_main_loop(e, new RenderInstanced2(e, ev, va,pta, true,flip,start_time,end_time));  
}


class V_ShaderCallFunction : public ShaderCall
{
public:
  V_ShaderCallFunction(std::string funcname, ShaderCall *next, std::string defines) : funcname(funcname), next(next),defines(defines) { id=-1; }
  int index(int base) const {
    id = next->index(base)+1; 
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 pos";
    out+=ss.str();
    out+=" = ";
    out+=funcname;
    out+="(pos";
    out+=ss2.str();
    out+=");\n";
    return out;
  }
  std::string define_strings() const { 
    std::string s1 = defines;
    std::string s2 = next->define_strings();
    std::string m = (s1=="" ||s2=="") ? "" : " ";
    return s1 + m + s2; 
  }
private:
  std::string funcname;
  ShaderCall *next;
  mutable int id;
  std::string defines;
};

class F_ShaderCallFunction : public ShaderCall
{
public:
  F_ShaderCallFunction(std::string funcname, ShaderCall *next, std::string defines) : funcname(funcname), next(next),defines(defines) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=funcname;
    out+="(rgb";
    out+=ss2.str();
    out+=");\n";
    return out;
  }
  std::string define_strings() const { 
    std::string s1 = defines;
    std::string s2 = next->define_strings();
    std::string m = (s1=="" ||s2=="") ? "" : " ";
    return s1 + m + s2; 
  }
private:
  std::string funcname;
  ShaderCall *next;
  mutable int id;
  std::string defines;
};
class EmptyV : public ShaderCall
{
public:
  EmptyV() : id(-1) { }
  int index(int base) const { id = base; return base; }
  std::string func_call() const
  {
    std::string out;
    out+="vec3 p = mix(in_Position, in_Position2, in_POS);\n";
    std::stringstream ss;
    ss << id+1;
    out+="vec4 pos" + ss.str() + " = vec4(p,1.0);\n";
    return out;
  }
  std::string define_strings() const { return ""; }
private:
  mutable int id;
};

class EmptyF : public ShaderCall
{
public:
  EmptyF(bool transparent) : transparent(transparent) { id = -1; }
  int index(int base) const { id = base; return base; }
  std::string func_call() const
  {
    std::string out;
    std::stringstream ss;
    ss << id+1;
    if (transparent)
      out+="vec4 rgb" + ss.str() + " = vec4(0.0,0.0,0.0,0.0);\n";
    else
      out+="vec4 rgb" + ss.str() + " = vec4(0.0,0.0,0.0,1.0);\n";
    return out;
  }
  std::string define_strings() const { return ""; }
private:
  mutable int id;
  bool transparent;
};

GameApi::US GameApi::UberShaderApi::v_empty()
{
  return add_uber(e, new EmptyV());
}
GameApi::US GameApi::UberShaderApi::v_color_from_normals(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("color_from_normals", next,""));
}
GameApi::US GameApi::UberShaderApi::v_recalc_normal(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("recalc_normal", next, "EX_NORMAL IN_NORMAL IN_N"));
}

GameApi::US GameApi::UberShaderApi::v_inst(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("inst", next, "INST"));
}

GameApi::US GameApi::UberShaderApi::v_ambient(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("ambient", next,""));
}
GameApi::US GameApi::UberShaderApi::v_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("phong", next,"EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}
GameApi::US GameApi::UberShaderApi::v_fog(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("fog", next,"EX_POSITION IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_shadow(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("shadow", next,"EX_POSITION IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_dyn_lights(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("dyn_lights", next,"IN_NORMAL EX_NORMAL2 IN_POSITION EX_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_diffuse(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("diffuse", next, "EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_specular(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("specular", next, "EX_NORMAL2 EX_LIGHTPOS2 LIGHTDIR IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_passall(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("passall", next,"EX_COLOR EX_TEXCOORD EX_NORMAL EX_POSITION IN_COLOR IN_TEXCOORD IN_NORMAL IN_POSITION"));
}
GameApi::US GameApi::UberShaderApi::v_pass_position(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("passpos", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_point_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("point_light", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_snoise(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("snoise", next,"EX_POSITION IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("light", next,""));
}

GameApi::US GameApi::UberShaderApi::v_ref(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("ref", next,"EX_POSITION IN_POSITION EX_NORMAL IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_wave(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("wave", next, "IN_POSITION"));
}

GameApi::US GameApi::UberShaderApi::v_toon(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("toon", next,"EX_NORMAL IN_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::v_texture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("texture", next,"EX_TEXCOORD IN_TEXCOORD"));
}
GameApi::US GameApi::UberShaderApi::v_manytexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("manytextures", next,"MANYTEXTURES EX_TEXCOORD IN_TEXCOORD"));
}
GameApi::US GameApi::UberShaderApi::v_cubemaptexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("cubemaptextures", next,"CUBEMAPTEXTURES EX_TEXCOORD IN_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::v_texture_arr(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("texture_arr", next,"EX_TEXCOORD IN_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::v_colour(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("colour", next,"EX_COLOR IN_COLOR"));
}

GameApi::US GameApi::UberShaderApi::v_blur(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("blur", next, "EX_TEXCOORD IN_TEXCOORD"));
}
class V_DistFieldMesh : public ShaderCall
{
public:
  V_DistFieldMesh(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec4 pos";
    out+=ss.str();
    out+=" = ";
    out+=funccall_to_string_with_replace(mod, "p1", "pos" + ss2.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};


class F_MeshColor : public ShaderCall
{
public:
  F_MeshColor(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec3 pos";
    out+=ss.str();
    out+=" = ";
    out+="ex_Position;\n";
    //out+="pos" + ss.str() + ".x/=gl_FragCoord.w;\n";
    //out+="pos" + ss.str() + ".y/=gl_FragCoord.w;\n";
    //out+="pos" + ss.str() + ".z/=gl_FragCoord.w;\n";
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=color_funccall_to_string_with_replace(mod, "pt", "pos" + ss.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return "EX_POSITION " + next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};

class F_Sandbox : public ShaderCall
{
public:
  F_Sandbox(ShaderCall *next, ShaderModule *mod) : next(next), mod(mod) { }
  int index(int base) const {
    id = next->index(base)+1;
    return id;
  }
  std::string func_call() const
  {
    std::string out;
    out+=next->func_call();
    std::stringstream ss;
    int i = id;
    ss << i+1;
    std::stringstream ss2;
    ss2 << i;
    out+="vec3 pos";
    out+=ss.str();
    out+=" = ";
    out+="ex_TexCoord;\n";
    out+="pos" + ss.str() + ".x*=300.0;\n";
    out+="pos" + ss.str() + ".y*=300.0;\n";
    //out+="pos" + ss.str() + ".z/=gl_FragCoord.w;\n";
    out+="vec4 rgb";
    out+=ss.str();
    out+=" = ";
    out+=color_funccall_to_string_with_replace(mod, "pt", "pos" + ss.str());
    out+=";\n";
    //out+=funcname;
    //out+="(pos";
    //out+=ss2.str();
    //out+=");\n";
    return out;
  }
  std::string define_strings() const { return "EX_TEXCOORD " + next->define_strings(); } // TODO, MAYBE ASK DEFINE STRINGS FROM SFO
private:
  ShaderCall *next;
  ShaderModule *mod;
  mutable int id;
};


GameApi::US GameApi::UberShaderApi::v_dist_field_mesh(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e,us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new V_DistFieldMesh(next,mod));
}
GameApi::US GameApi::UberShaderApi::f_mesh_color(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e, us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new F_MeshColor(next,mod));
}
GameApi::US GameApi::UberShaderApi::f_sandbox(US us, SFO sfo)
{
  ShaderCall *next = find_uber(e, us);
  ShaderModule *mod = find_shader_module(e, sfo);
  return add_uber(e, new F_Sandbox(next,mod));
}
GameApi::US GameApi::UberShaderApi::v_skeletal(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction("skeletal", next, "SKELETAL"));
}


GameApi::US GameApi::UberShaderApi::f_empty(bool transparent)
{
  return add_uber(e, new EmptyF(transparent));
}

GameApi::US GameApi::UberShaderApi::f_diffuse(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("diffuse", next,"EX_NORMAL2 EX_LIGHTPOS2 LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_phong(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("phong", next,"EX_NORMAL2 EX_LIGHTPOS2 LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_fog(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("fog", next,"EX_POSITION"));
}
GameApi::US GameApi::UberShaderApi::f_shadow(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("shadow", next,"EX_POSITION MANYTEXTURES COLOR_MIX"));
}
GameApi::US GameApi::UberShaderApi::f_dyn_lights(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("dyn_lights", next,"EX_NORMAL2 EX_POSITION"));

}

GameApi::US GameApi::UberShaderApi::f_ambient(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("ambient", next,"LEVELS"));
}
GameApi::US GameApi::UberShaderApi::f_color_from_id(US us, int id)
{
  ShaderCall *next = find_uber(e, us);
  std::stringstream ss;
  ss << id;
  return add_uber(e, new F_ShaderCallFunction("color_from_id_" + ss.str(), next, "COLOR_ARRAY"));
}

GameApi::US GameApi::UberShaderApi::f_specular(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("specular", next,"EX_NORMAL2 EX_LIGHTPOS2 SPECULAR_SIZE LEVELS"));
}

GameApi::US GameApi::UberShaderApi::f_color_from_normals(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("color_from_normals", next,"EX_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::f_point_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  std::string def = next->define_strings();
  def+="EX_POSITION LIGHTPOS";
  std::string cur_def = "LIGHTPOS";
  std::vector<std::string> str = replace_c_template_unique_ids(def, cur_def);
  std::string id = str[str.size()-1];
  return add_uber(e, new F_ShaderCallFunction("point_light" + id, next,"EX_POSITION LIGHTPOS"));
}

GameApi::US GameApi::UberShaderApi::f_bands(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("bands", next,""));
}

GameApi::US GameApi::UberShaderApi::f_snoise(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("snoise", next,"EX_POSITION"));
}

GameApi::US GameApi::UberShaderApi::f_blur(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("blur", next,"EX_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::f_ref(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("ref", next,"EX_POSITION EX_NORMAL"));
}
GameApi::US GameApi::UberShaderApi::v_custom(US us, std::string v_funcname)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new V_ShaderCallFunction(v_funcname, next,"MANYTEXTURES IN_POSITION EX_POSITION IN_TEXCOORD EX_TEXCOORD COLOR_MIX"));

}
GameApi::US GameApi::UberShaderApi::f_custom(US us, std::string f_funcname)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction(f_funcname, next,"MANYTEXTURES EX_POSITION IN_TEXCOORD EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_light(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("light", next,""));
}

GameApi::US GameApi::UberShaderApi::f_toon(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("toon", next,"EX_NORMAL"));
}

GameApi::US GameApi::UberShaderApi::f_texture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("texture", next,"EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_manytexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("manytextures", next,"MANYTEXTURES EX_TEXCOORD COLOR_MIX"));
}
GameApi::US GameApi::UberShaderApi::f_cubemaptexture(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("cubemaptextures", next,"CUBEMAPTEXTURES EX_TEXCOORD COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_texture_arr(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("texture_arr", next,"EX_TEXCOORD TEXTURE_ARRAY COLOR_MIX"));
}

GameApi::US GameApi::UberShaderApi::f_colour(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("colour", next,"EX_COLOR"));
}
GameApi::US GameApi::UberShaderApi::f_choose_color(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("choose_color", next,"CHOOSE_COLOR"));
}

class LineCurve : public Curve<Point>
{
public:
  LineCurve(Point p1, Point p2) : p1(p1), p2(p2) {}
  float Size() const {
    return 1.0;
  }
  Point Index(float p) const
  {
    return Point((1.0-p)*Vector(p1) + p*Vector(p2));
  }
private:
  Point p1,p2;
};

class CircleCurveXY : public Curve<Point>
{
public:
  CircleCurveXY(Point center, float r) : center(center), r(r) { }
  float Size() const { return 2.0*3.14159; }
  Point Index(float p) const
  {
    Point pp;
    pp.x = r*cos(p);
    pp.y = r*sin(p);
    pp.z = 0.0;
    return pp;
  }
private:
  Point center;
  float r;
};

class CircleCurveXZ : public Curve<Point>
{
public:
  CircleCurveXZ(Point center, float r) : center(center), r(r) { }
  float Size() const { return 2.0*3.14159; }
  Point Index(float p) const
  {
    Point pp;
    pp.x = r*cos(p);
    pp.y = 0.0;
    pp.z = r*sin(p);
    return pp;
  }
private:
  Point center;
  float r;
};

class XYSumCurvePos : public CurvePos
{
public:
  virtual float FindPos(Point p, float curve_length) const { return  (p.x+p.y+p.z)/(300.0+300.0+300.0)*curve_length; }
};
EXPORT GameApi::CPP GameApi::CurveApi::xy_sum()
{
  return add_curve_pos(e, new XYSumCurvePos);
}

class XYSumCurvePos2 : public CurvePos
{
public:
  XYSumCurvePos2(float xmult, float ymult, float zmult) : xmult(xmult), ymult(ymult), zmult(zmult) { }
  virtual float FindPos(Point p, float curve_length) const { return  (p.x*xmult+p.y*ymult+p.z*zmult)/(300.0+300.0+300.0)*curve_length; }
private:
  float xmult,ymult,zmult;
};
EXPORT GameApi::CPP GameApi::CurveApi::xy_sum2(float xmult, float ymult, float zmult)
{
  return add_curve_pos(e, new XYSumCurvePos2(xmult,ymult,zmult));
}

EXPORT GameApi::C GameApi::CurveApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  return add_curve(e, new LineCurve(*pp1, *pp2));
}
class WaveCircleCurve : public Curve<Point>
{
public:
  WaveCircleCurve(Waveform *wave, float start_r, float end_r, float length) : wave(wave), start_r(start_r), end_r(end_r), length(length) { }
  float Size() const { return length; }
  Point Index(float p) const {
    float pos = p;
    pos/=length; // [0..1]
    pos*=wave->Length(); // [0..wave->Length()]
    float r = wave->Index(pos); // [min..max]
    //r/=wave->Max()-wave->Min();  // [min..min+1]
    //r-=wave->Min();  // [0..1]
    r*=end_r-start_r; // [0..(end_r-start_r)]
    r+=start_r; // [start_r..end_r]
    float x = r*cos(p);
    float y = r*sin(p);
    Point p2 = { x,y,0.0f };
    return p2;
  }
private:
  Waveform *wave;
  float start_r, end_r;
  float length;
};
EXPORT GameApi::C GameApi::CurveApi::circle_xy_wave(float start_r, float end_r, WV wave, float length)
{
  Waveform *wv = find_waveform(e, wave);
  return add_curve(e, new WaveCircleCurve(wv, start_r, end_r, length));
}
EXPORT GameApi::C GameApi::CurveApi::circle_xy(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXY(*c, r));
}
EXPORT GameApi::C GameApi::CurveApi::circle_xz(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXZ(*c, r));
}
EXPORT GameApi::C GameApi::CurveApi::linear(std::vector<PT> vec)
{
  if (vec.size()==0) { GameApi::C c; c.id = -1; return c; }
  if (vec.size()==1) { GameApi::PT pt = vec[0]; return line(pt,pt); }
  int s = vec.size();
  std::vector<C> vec2;
  for(int i=0;i<s-1;i++)
    {
      GameApi::C c = line(vec[i],vec[i+1]);
      vec2.push_back(c);
    }
  return compose(vec2);
}
class BezierPoints : public PointCollection
{
public:
  BezierPoints(std::vector<Point> vec) : vec(vec) { }
  int Size() const { return vec.size(); }
  Point Index(int i) const { return vec[i]; }

private:
  std::vector<Point> vec;

};
EXPORT GameApi::C GameApi::CurveApi::bezier(std::vector<PT> vec)
{
  int s = vec.size();
  std::vector<Point> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(*find_point(e,vec[i]));
    }
  BezierPoints *pt = new BezierPoints(vec2);
  return add_curve(e, new BezierCurve(*pt));
}

class MatrixCurve3 : public Curve<Point>
{
public:
  MatrixCurve3(Curve<Point> *c, Matrix m) : c(c), m(m) {}
  float Size() const { return c->Size(); }
  Point Index(float pos) const
  {
    Point p = c->Index(pos);
    Point p2 = p*m; 
    return p2;
  }
private:
  Curve<Point> *c;
  Matrix m;
};
EXPORT GameApi::C GameApi::CurveApi::scale(C curve, float mx, float my, float mz)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new MatrixCurve3(c, Matrix::Scale(mx, my, mz)));
}

EXPORT GameApi::C GameApi::CurveApi::trans(C curve, float dx, float dy, float dz)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new MatrixCurve3(c, Matrix::Translate(dx, dy, dz)));
}

class ComposeCurve2 : public Curve<Point>
{
public:
  ComposeCurve2(std::vector<Curve<Point>*> vec) : vec(vec) { }
  Point Index(float p) const
  {
    float div = fmod(p, Size());
    int pos = int(p);
    Curve<Point> *c = vec[pos];
    div*=c->Size();
    return c->Index(div);
  }
  float Size() const {
    return float(vec.size());
  }
  
private:
  std::vector<Curve<Point>*> vec;
};

EXPORT GameApi::C GameApi::CurveApi::compose(std::vector<C> vec)
{
  int s = vec.size();
  std::vector<Curve<Point>*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_curve(e,vec[i]));
    }
  return add_curve(e, new ComposeCurve2(vec2));
}

class LengthCurve : public Curve<Point>
{
public:
  LengthCurve(Curve<Point> *c, float new_length) : c(c), new_length(new_length) { }
  float Size() const {
    return new_length;
  }
  Point Index(float p) const
  {
    p/=new_length;
    p*=c->Size();
    return c->Index(p);
  }
private:
  Curve<Point> *c;
  float new_length;
};
EXPORT GameApi::C GameApi::CurveApi::change_length(C curve, float new_length)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new LengthCurve(c, new_length));
}

class SplitCurve : public Curve<Point>
{
public:
  SplitCurve(Curve<Point> *c, float start, float end) : c(c), start(start), end(end) { }
  float Size() const { return end-start; }
  Point Index(float p) const
  {
    return c->Index(start+p);
  }
private:
  Curve<Point> *c;
  float start, end;
};

EXPORT GameApi::C GameApi::CurveApi::split(C curve, float start_var, float end_var)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new SplitCurve(c, start_var, end_var));
}

EXPORT GameApi::PT GameApi::CurveApi::pos(C curve, float p)
{
  Curve<Point> *c = find_curve(e, curve);
  Point pp = c->Index(p);
  return add_point(e, pp.x,pp.y,pp.z);
}

class SampleCurve : public PointsApiPoints
{
public:
  SampleCurve(Curve<Point> *c, int num_points) : c(c), num_points(num_points) { }
  int NumPoints() const { return num_points; }
  Point Pos(int i) const
  {
    float val = float(i);
    val/=float(num_points);
    val*=c->Size();
    return c->Index(val);
  }
  unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Curve<Point> *c;
  int num_points;
  float start,end;
};
EXPORT GameApi::PTS GameApi::CurveApi::sample(C curve, int num_samples)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_points_api_points(e, new SampleCurve(c,num_samples));
}
class CurveLineCollection : public LineCollection
{
public:
  CurveLineCollection(Curve<Point> *c, int num_lines) : c(c), num_lines(num_lines) { }
  int NumLines() const { return num_lines; }
  Point LinePoint(int line, int point) const
  {
    int pos = line+point;
    float pos2 = float(pos);
    pos2/=float(num_lines+1);
    pos2*=c->Size();
    return c->Index(pos2);
  }
private:
  Curve<Point> *c;
  int num_lines;
};

EXPORT GameApi::LI GameApi::CurveApi::to_lines(C curve, int num_lines)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_line_array(e, new CurveLineCollection(c, num_lines));
}

class ProductPatch : public CurvePatch
{
public:
  ProductPatch(Curve<Point> &c1, Curve<Point> &c2, Point c2_center) : c1(c1), c2(c2), c2_center(c2_center) { }
  virtual float start_x() const { return 0.0; }
  virtual float end_x() const { return c1.Size(); }
  virtual float start_y() const { return 0.0; }
  virtual float end_y() const { return c2.Size(); }
  virtual Point Map(float x, float y) const
  {
    Point p = c1.Index(x);
    Point p2 = c2.Index(y);
    Vector v2 = p2-c2_center;
    p+=v2;
    return p;
  }
private:
  Curve<Point> &c1;
  Curve<Point> &c2;
  Point c2_center;
};

EXPORT GameApi::PA GameApi::CurveApi::curve_product(C c1, C c2, PT c2_center)
{
  Point *pt = find_point(e, c2_center);
  Curve<Point> *p1 = find_curve(e, c1);
  Curve<Point> *p2 = find_curve(e, c2);
  return add_patch(e, new ProductPatch(*p1, *p2, *pt));
}
class PatchSample : public FaceCollection
{
public:
  PatchSample(CurvePatch &patch, int sx, int sy) : patch(patch), sx(sx), sy(sy) { }
  virtual void Prepare() { }
  virtual int NumFaces() const { return sx*sy; }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int y = face/sx;
    int x = face - y*sx;
    float dx = (patch.end_x() - patch.start_x())/sx;
    float dy = (patch.end_y() - patch.start_y())/sy;
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    float pos_x = patch.start_x() + xx * dx;
    float pos_y = patch.start_y() + yy * dy;
    if (point==1 ||point==2) pos_x+=dx;
    if (point==2 ||point==3) pos_y+=dy;
    return patch.Map(pos_x, pos_y);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face,0);
    Point p2 = FacePoint(face,1);
    Point p3 = FacePoint(face,2);
    return -Vector::CrossProduct(p2-p1, p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const {
    int y = face/sx;
    int x = face - y*sx;
    float dx = (1.0 - 0.0)/sx;
    float dy = (1.0 - 0.0)/sy;
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    float pos_x = xx * dx;
    float pos_y = yy * dy;
    Point2d p;
    p.x = pos_x;
    p.y = pos_y;
    return p;
    
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
private:
  CurvePatch &patch;
  int sx,sy;
};
EXPORT GameApi::P GameApi::CurveApi::patch_sample(PA patch, int sx, int sy)
{
  CurvePatch *p = find_patch(e, patch);
  return add_polygon2(e, new PatchSample(*p, sx,sy));
}

class Patch_X_Curve : public Curve<Point>
{
public:
  Patch_X_Curve(CurvePatch &p, float y) : p(p), y(y) { }
  float Size() const { return p.end_x() - p.start_x(); }
  Point Index(float x) const
  {
    Point pp = p.Map(p.start_x()+x,y);
    return pp;
  }
private:
  CurvePatch &p;
  float y;
};
class Patch_Y_Curve : public Curve<Point>
{
public:
  Patch_Y_Curve(CurvePatch &p, float x) : p(p), x(x) { }
  float Size() const { return p.end_y() - p.start_y(); }
  Point Index(float y) const
  {
    Point pp = p.Map(x, p.start_y()+y);
    return pp;
  }
private:
  CurvePatch &p;
  float x;
};
EXPORT GameApi::C GameApi::CurveApi::x_curve(PA patch, float y)
{
  CurvePatch *p = find_patch(e, patch);
  return add_curve(e, new Patch_X_Curve(*p, y));
}
EXPORT GameApi::C GameApi::CurveApi::y_curve(PA patch, float x)
{
  CurvePatch *p = find_patch(e, patch);
  return add_curve(e, new Patch_Y_Curve(*p, x));
}

class FromPointsMatrices : public MatrixArray
{
public:
  FromPointsMatrices(PointsApiPoints *p) : p(p) { }
  int Size() const {
    return p->NumPoints();
  }
  Matrix Index(int i) const
  {
    Point pp = p->Pos(i);
    return Matrix::Translate(pp.x, pp.y, pp.z);
  }
private:
  PointsApiPoints *p;
};

class MSRandomRot : public MatrixArray
{
public:
  MSRandomRot(Point p, int count) : p(p), count(count) { }
  int Size() const { return count; }
  Matrix Index(int i) const
  {
    Random r;
    float val = double(r.next())/r.maximum();
    val*=2.0*3.14159;
    return Matrix::RotateAroundAxis(Vector(p), val);
  }
private:
  Point p;
  int count;
};

EXPORT GameApi::MS GameApi::MatricesApi::ms_random_rot(float px, float py, float pz, int count)
{
  return add_matrix_array(e, new MSRandomRot(Point(px,py,pz), count));
}

EXPORT GameApi::MS GameApi::MatricesApi::from_points(PTS pts)
{
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_matrix_array(e, new FromPointsMatrices(points));
}

class From2dLinesMatrices : public MatrixArray
{
public:
  From2dLinesMatrices(LineCollection *coll) : coll(coll) { }
  int Size() const {
    return coll->NumLines();
  }
  Matrix Index(int i) const
  {
    Point p1 = coll->LinePoint(i, 0);
    Point p2 = coll->LinePoint(i, 1);
    LineProperties lprop(p1,p2);
    Matrix m0 = Matrix::Scale(1.0/300.0, 1.0, 1.0);
    Matrix m1 = lprop.translate_to_p1();
    Matrix m2 = lprop.rotate_z_from_p1_in_2d();
    Matrix m3 = lprop.scale_length_in_2d();
    return m0*m3*m2*m1;
  }
private:
  LineCollection *coll;
};

// requires x=0..300 3d object
EXPORT GameApi::MS GameApi::MatricesApi::from_lines_2d(LI li)
{
  LineCollection *lines = find_line_array(e,li);
  return add_matrix_array(e, new From2dLinesMatrices(lines));
}

class MultArray1 : public MatrixArray
{
public:
  MultArray1(MatrixArray *arr, Matrix m) : arr(arr), m(m) { }
  int Size() const { return arr->Size(); }
  Matrix Index(int i) const { return arr->Index(i)*m; }
private:
  MatrixArray *arr;
  Matrix m;
};

class MultArray2 : public MatrixArray
{
public:
  MultArray2(Matrix m, MatrixArray *arr) : m(m), arr(arr) { }
  int Size() const { return arr->Size(); }
  Matrix Index(int i) const { return m*arr->Index(i); }
private:
  Matrix m;
  MatrixArray *arr;
};

EXPORT GameApi::MS GameApi::MatricesApi::mult(MS m, M mat)
{
  MatrixArray *arr = find_matrix_array(e, m);
  Matrix mm = find_matrix(e, mat);
  return add_matrix_array(e, new MultArray1(arr,mm));
}
class MultArray3 : public MatrixArray
{
public:
  MultArray3(MatrixArray *arr1, MatrixArray *arr2) : arr1(arr1), arr2(arr2) { }
  int Size() const { return std::min(arr1->Size(),arr2->Size()); }
  Matrix Index(int i) const { return arr1->Index(i)*arr2->Index(i); }
private:
  MatrixArray *arr1;
  MatrixArray *arr2;
};
EXPORT GameApi::MS GameApi::MatricesApi::mult_array(MS m1, MS m2)
{
  MatrixArray *arr1 = find_matrix_array(e, m1);
  MatrixArray *arr2 = find_matrix_array(e, m2);
  return add_matrix_array(e, new MultArray3(arr1,arr2));
}
EXPORT GameApi::MS GameApi::MatricesApi::mult(M mat, MS m)
{
  Matrix mm = find_matrix(e,mat);
  MatrixArray *arr = find_matrix_array(e,m);
  return add_matrix_array(e, new MultArray2(mm,arr));
}
class SubArrayMatrices : public MatrixArray
{
public:
  SubArrayMatrices(MatrixArray *m, int start, int count) : m(m) { }
  int Size() const { return std::min(count, m->Size()-start); }
  Matrix Index(int i) const { return m->Index(start+i); }
private:
  MatrixArray *m;
  int start;
  int count;
};
EXPORT GameApi::MS GameApi::MatricesApi::subarray(MS m, int start, int count)
{
  MatrixArray *arr = find_matrix_array(e,m);
  return add_matrix_array(e, new SubArrayMatrices(arr,start,count));
}

class FromCurveMatrices : public Curve<Matrix>
{
public:
  FromCurveMatrices(Curve<Point> *c) : c(c) { }
  float Size() const { return c->Size(); }
  Matrix Index(float i) const {
    Point p = c->Index(i);
    Matrix m = Matrix::Translate(p.x,p.y,p.z);
    return m;
  }
private:
  Curve<Point> *c;
};
EXPORT GameApi::MC GameApi::MatrixCurveApi::from_curve(C curve)
{
  Curve<Point> *c = find_curve(e,curve);
  return add_matrix_curve(e, new FromCurveMatrices(c));
}
class CircleXYMatrix : public Curve<Matrix>
{
public:
  CircleXYMatrix(float radius) : r(radius) { }
  float Size() const { return 2.0*3.14159; }
  Matrix Index(float x) const
  {
    Matrix m = Matrix::Translate(r,0.0,0.0);
    Matrix m2 = Matrix::ZRotation(x);
    return m * m2;
  }
private:
  float r;
};

class CircleXZMatrix : public Curve<Matrix>
{
public:
  CircleXZMatrix(float radius) : r(radius) { }
  float Size() const { return 2.0*3.14159; }
  Matrix Index(float x) const
  {
    Matrix m = Matrix::Translate(r,0.0,0.0);
    Matrix m2 = Matrix::YRotation(x);
    return m * m2;
  }
private:
  float r;
};
EXPORT GameApi::MC GameApi::MatrixCurveApi::circle_xy(float radius)
{
  return add_matrix_curve(e, new CircleXYMatrix(radius));
}
EXPORT GameApi::MC GameApi::MatrixCurveApi::circle_xz(float radius)
{
  return add_matrix_curve(e, new CircleXZMatrix(radius));
}
class SampleMatrixCurve2 : public MatrixArray
{
public:
  SampleMatrixCurve2(Curve<Matrix> *c, int num_points) : c(c), num_points(num_points) { }
  int Size() const { return num_points; }
  Matrix Index(int i) const
  {
    float val = float(i);
    val/=float(num_points);
    val*=c->Size();
    return c->Index(val);
  }
  unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Curve<Matrix> *c;
  int num_points;
};

EXPORT GameApi::MS GameApi::MatrixCurveApi::sample(MC m_curve, int num)
{
  Curve<Matrix> *c = find_matrix_curve(e, m_curve);
  return add_matrix_array(e, new SampleMatrixCurve2(c,num));
}

class DistanceFieldMesh : public MaterialForward
{
public:
  DistanceFieldMesh(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo),next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id,flip,start_time, end_time);
    GameApi::ML df = ev.polygon_api.dist_field_mesh_shader(ev, ml,sfo);
    return df;
  }

private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

class SFO_Sandbox : public MaterialForward
{
public:
  SFO_Sandbox(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo), next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const 
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML df = ev.polygon_api.sfo_sandbox_shader(ev, ml,sfo);
    return df;

  }

private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

class MeshColorFromSfo : public MaterialForward
{
public:
  MeshColorFromSfo(GameApi::EveryApi &ev, GameApi::SFO sfo, Material *next) : ev(ev), sfo(sfo),next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    //GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    //GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML df = ev.polygon_api.mesh_color_shader(ev, ml,sfo);
    return df;
  }

  
private:
  GameApi::EveryApi &ev;
  GameApi::SFO sfo;
  Material *next;
};

GameApi::MT GameApi::MaterialsApi::dist_field_mesh(EveryApi &ev, SFO sfo, MT next)
{
  SFO sfo2 = ev.sh_api.v_render(sfo);
  Material *nxt = find_material(e, next);
  return add_material(e, new DistanceFieldMesh(ev,sfo2,nxt));
}
GameApi::MT GameApi::MaterialsApi::sfo_sandbox(EveryApi &ev, SFO sfo, MT next)
{
  Material *nxt = find_material(e, next);
  return add_material(e, new SFO_Sandbox(ev, sfo, nxt));
}
GameApi::MT GameApi::MaterialsApi::mesh_color_from_sfo(EveryApi &ev, SFO sfo, MT next)
{
  Material *nxt = find_material(e, next);
  return add_material(e, new MeshColorFromSfo(ev, sfo, nxt));
}

class SkeletalRoot : public SkeletalNode
{
public:
  SkeletalRoot(Point p) : p(p) { }
  Point pos(float time) const { return p; }
  Matrix mat(float time) const { return Matrix::Identity(); }
private:
  Point p;
};

GameApi::SA GameApi::Skeletal::root(PT pos)
{
  Point *pt = find_point(e, pos);
  return add_skeletal(e, new SkeletalRoot(*pt));
}
class SkeletalImpl : public SkeletalNode
{
public:
  SkeletalImpl(SkeletalNode *sk, Movement *mn, Point pt) : sk(sk), mn(mn), pt(pt) { }
  virtual Matrix mat(float time) const
  {
    // TODO: skeletal can't use delta_time properly.
    return mn->get_whole_matrix(time, 1.0);
  }
  virtual Point pos(float time) const
  {
    Point pt0 = sk->pos(time);
    Matrix m = mat(time);
    return pt0 + pt * m;
  }
private:
  SkeletalNode *sk;
  Movement *mn;
  Point pt;
}; 
GameApi::SA GameApi::Skeletal::node(SA parent, MN matrix, PT point_offset)
{
  SkeletalNode *sk = find_skeletal(e, parent);
  Movement *mn = find_move(e, matrix);
  Point *pt = find_point(e, point_offset);
  return add_skeletal(e, new SkeletalImpl(sk, mn, *pt));
}
GameApi::ML GameApi::Skeletal::skeletal_bind(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> savec)
{
  GameApi::P pp = ev.polygon_api.or_array2(vec);
  GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);

  std::vector<int> vecattribs;
  vecattribs.push_back(AttrPart);
  GameApi::VA va = ev.polygon_api.create_vertex_array_attribs(pp2,false,std::vector<int>(), vecattribs);
  GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
  
  GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
  return ml2;
}
GameApi::ML GameApi::Skeletal::skeletal_bind_material(EveryApi &ev, std::vector<P> vec, std::vector<PT> vec2, std::vector<SA> savec, MT material)
{
    GameApi::P pp = ev.polygon_api.or_array2(vec);
    GameApi::P pp2 = ev.polygon_api.build_offsets(pp, vec2);
    Material *mat2 = find_material(e, material);
    int val = mat2->mat(pp2.id);
    GameApi::ML ml;
    ml.id = val;
    GameApi::ML ml2 = ev.polygon_api.skeletal_shader(ev, ml, savec);
    return ml2;
}

#if 0
class SA_Bind : public MainLoopItem
{
public:
  SA_Bind(std::vector<SkeletalNode*> sk, std::vector<>);
  void execute(MainLoopEnv &e)
  {
  }
};
  GameApi::ML GameApi::Skeletal::sa_bind(EveryApi &ev, std::vector<SA> pos, std::vector<P> vec)
{
  GameApi::P obj = ev.polygon_api.or_array();
}
#endif
EXPORT GameApi::PD GameApi::PolygonDistanceField::empty(EveryApi &ev)
{
  PD_Impl pd;
  pd.mesh = ev.polygon_api.empty();
  pd.distance_field = ev.sh_api.cube();
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polydistfield.push_back(pd);
  PD pd2;
  pd2.id = env->polydistfield.size()-1;
  return pd2;
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::create_pd(P mesh, SFO distance_field)
{
  PD_Impl pd;
  pd.mesh = mesh;
  pd.distance_field = distance_field;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polydistfield.push_back(pd);
  PD pd2;
  pd2.id = env->polydistfield.size()-1;
  return pd2;
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::cube(EveryApi &ev, float start_x, float end_x,
				       float start_y, float end_y,
				       float start_z, float end_z)
{
  SFO cube_sfo = ev.sh_api.cube(start_x, end_x,
				start_y, end_y,
				start_z, end_z);
  P cube_p = ev.polygon_api.cube(start_x, end_x,
				 start_y, end_y,
				 start_z, end_z);
  return create_pd(cube_p, cube_sfo);
}

EXPORT GameApi::PD GameApi::PolygonDistanceField::rounded_cube(EveryApi &ev,
					       float start_x, float end_x,
					       float start_y, float end_y,
					       float start_z, float end_z,
					       float radius)
{
  SFO c_sfo = ev.sh_api.rounded_cube(start_x, end_x,
				     start_y, end_y,
				     start_z, end_z,
				     radius);
  P c_p = ev.polygon_api.rounded_cube(ev,start_x, end_x,
				      start_y, end_y,
				      start_z, end_z,
				      radius);
  return create_pd(c_p, c_sfo);
}

EXPORT GameApi::PD GameApi::PolygonDistanceField::sphere(EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
{
  SFO sfo = ev.sh_api.sphere(center, radius);
  P p = ev.polygon_api.sphere(center, radius, numfaces1, numfaces2);
  return create_pd(p,sfo);
}
#if 0
GameApi::PD GameApi::PolygonDistanceField::cone(EveryApi &ev, int numfaces, PT p1, PT p2, float rad1, float rad2)
{
}
#endif

EXPORT GameApi::PD GameApi::PolygonDistanceField::or_array(EveryApi &ev, std::vector<PD> vec)
{
  if (vec.size()==0) { GameApi::PD p; p.id = 0; return p; }
  PD p = vec[0];
  PD_Impl pi = find_polydistfield(e, p);
  SFO pi_sfo = pi.distance_field;
  P pi_p = pi.mesh;
  std::vector<P> vec2;
  vec2.push_back(pi_p);
  int s = vec.size();
  for(int i=1;i<s;i++)
    {
      PD pp = vec[i];
      PD_Impl ppi = find_polydistfield(e,pp);
      pi_sfo = ev.sh_api.or_elem(pi_sfo, ppi.distance_field);
      vec2.push_back(ppi.mesh);
    }
  P res_p = ev.polygon_api.or_array2(vec2);
  return create_pd(res_p, pi_sfo);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::color_from_normal(EveryApi &ev, PD obj)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.color_from_normal(sfo);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::ambient_occulsion_sfo(EveryApi &ev,
						       PD obj, float d, float i)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.ambient_occulsion(sfo, d,i);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
EXPORT GameApi::PD GameApi::PolygonDistanceField::colormod_from_position(EveryApi &ev, PD obj, float px, float py, float pz, float sx, float sy, float sz)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.colormod_from_position(sfo, px,py,pz,sx,sy,sz);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);

}
EXPORT GameApi::SFO GameApi::PolygonDistanceField::get_distance_field(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.distance_field;
}
EXPORT GameApi::P GameApi::PolygonDistanceField::get_polygon(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.mesh;
}

EXPORT GameApi::MT GameApi::PolygonDistanceField::mesh_color_from_sfo(EveryApi &ev, PD orig, MT next)
{
  PD_Impl impl = find_polydistfield(e,orig);
  return ev.materials_api.mesh_color_from_sfo(ev, impl.distance_field, next);
}
EXPORT GameApi::ML GameApi::PolygonDistanceField::render_scene(EveryApi &ev, PD pd, PD world)
{
  // TODO
  //P p = get_poly(pd);
  //SFO sfo = get_distance_field(world);
  GameApi::ML ml;
  ml.id = 0;
  return ml;
}

EXPORT std::vector<GameApi::ML> GameApi::PolygonDistanceField::render_scene_array(EveryApi &ev, std::vector<PD> vec)
{
  int s = vec.size();
  std::vector<ML> mls;
  std::vector<PD> world_vec;
  for(int i=0;i<s;i++)
    {
      PD pd = vec[i];
      world_vec.push_back(pd);
    }
  PD world = or_array(ev,world_vec);
  for(int i=0;i<s;i++)
    {
      PD pd = vec[i];
      ML ml = render_scene(ev, pd, world);
      mls.push_back(ml);
    }
  return mls;
}



#include "GameApi_gui.hh"

class ClickHide : public GuiWidgetForward
{
public:
  ClickHide(GameApi::EveryApi &ev, GuiWidget *wid) : GuiWidgetForward(ev, {wid}) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    //state=0;
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos,button, ch, type, mouse_wheel_y);
    //std::cout << "clickhide button=" << button << " " << type << std::endl;
    if (button==0 && type==1025) {
      if (vec.size() >0) {
	Point2d pos = vec[0]->get_pos();
	Vector2d sz = vec[0]->get_size();
	//std::cout << "clickhide:" << pos.x << " " << pos.y << " " << sz.dx << " " << sz.dy << std::endl;

	if (mouse_pos.x > pos.x && mouse_pos.x< pos.x+sz.dx &&
	    mouse_pos.y > pos.y && mouse_pos.y< pos.y+sz.dy)
	  {
	    vec[0]->hide();
	    //state=1;
	  }
      }
    }
    //if (state==1 && button==-1) state=0;
    Vector2d sz = vec[0]->get_size();
    size.dx = sz.dx;
    size.dy = sz.dy;

  }
private:
  //int state;
};

class ClickVisibilityWidget : public GuiWidgetForward
{
public:
  ClickVisibilityWidget(GameApi::EveryApi &ev, GuiWidget *area, GuiWidget *hidden) : GuiWidgetForward(ev, {area,hidden}), area(area), hidden(hidden) { 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    hidden->hide();
    //state = 0;
  }

  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos,button, ch, type, mouse_wheel_y);
    //std::cout << "clickvisibility button=" << button << " " << type << std::endl;
    if (button==0 && type==1025) {
      Point2d pos = area->get_pos();
      Vector2d sz = area->get_size();
      //Point2d pos2 = hidden->get_pos();
      //Vector2d sz2 = hidden->get_size();
      //std::cout << "vis: " << pos.x << " " << pos.y << " " << sz.dx << " " << sz.dy << std::endl;
      //std::cout << "hid: " << pos2.x << " " << pos2.y << " " << sz2.dx << " " << sz2.dy << std::endl;
      if (mouse_pos.x > pos.x && mouse_pos.x< pos.x+sz.dx &&
	  mouse_pos.y > pos.y && mouse_pos.y< pos.y+sz.dy)
	{
	  if (hidden->is_visible()) { hidden->hide(); } else { hidden->show(); }
	}
      //state=1;
    }
    //if (state==1 && button==-1) { state=0; }
    Vector2d sz = area->get_size();
    size.dx = sz.dx;
    size.dy = sz.dy;
  }

private:
  GuiWidget *area;
  GuiWidget *hidden;
  //int state;
};

GameApi::W GameApi::GuiApi::click_visibility(W area_widget, W hidden_widget)
{
  GuiWidget *w1 = find_widget(e, area_widget);
  GuiWidget *w2 = find_widget(e, hidden_widget);
  return add_widget(e, new ClickVisibilityWidget(ev, w1, w2));
}


GameApi::W GameApi::GuiApi::click_hide(W widget)
{
  GuiWidget *w1 = find_widget(e, widget);
  return add_widget(e, new ClickHide(ev, w1));
}


GameApi::W GameApi::GuiApi::popup_box(std::string label, std::vector<std::string> options, FtA atlas, BM atlas_bm)
{
  W txt_1 = text(label, atlas, atlas_bm);
  W txt_2 = margin(txt_1, 5,5,5,5);
  W txt_2a = center_y(txt_2, 70.0);
  //W but_1 = button(size_x(txt_2)+30.0, size_y(txt_2), c_list_item_title, c_list_item_title2);
  W but_2 = button(20.0, 20.0, c_canvas_item_node_1, c_canvas_item_node_1_2);
  W but_2a = center_align(but_2, 70.0);
  W but_2b = center_y(but_2a, 70.0);
  //W but_2a = margin(but_2,5,5,5,5);
  std::vector<W> vec;
  vec.push_back(txt_2a);
  vec.push_back(but_2b);
  W x_arr = array_x(&vec[0], vec.size(), 3);

  W mar = margin(x_arr, 2,2,2,2);
  W but_1 = button(size_x(mar), size_y(mar), c_list_item_title, c_list_item_title2);
  W lay = layer(but_1, mar);

  W menu = popup_box_menu(options, atlas, atlas_bm);
  W w6 = top_right_corner_match(lay,menu);
  W menu_hide = click_hide(w6);
  W menu_visibility = click_visibility(lay, menu_hide);
  return menu_visibility;
}

class TopRightBottomRight : public GuiWidgetForward
{
public:
  TopRightBottomRight(GameApi::EveryApi &ev, GuiWidget *wid, GuiWidget *floating) : GuiWidgetForward(ev, {floating } ),wid(wid), floating(floating) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2); 
  }
  void set_pos(Point2d pos)
  {
    //wid->set_pos(pos);
    Point2d p = wid->get_pos();
    Vector2d v1 = { wid->get_size().dx -floating->get_size().dx,wid->get_size().dy };
    p += v1;
    GuiWidgetForward::set_pos(p);
    floating->set_pos(p);
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
    floating->set_size(size);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type, mouse_wheel_y);
    Vector2d v1 = floating->get_size();
    //Vector2d v2 = vec[1]->get_size();
    size = v1;

  }
private:
  GuiWidget *wid;
  GuiWidget *floating;
};

GameApi::W GameApi::GuiApi::top_right_corner_match(W wid, W floating)
{
  GuiWidget *wid2 = find_widget(e, wid);
  GuiWidget *floating2 = find_widget(e, floating);
  return add_widget(e, new TopRightBottomRight(ev, wid2, floating2));
}

GameApi::W GameApi::GuiApi::popup_box_menu(std::vector<std::string> options, FtA atlas, BM atlas_bm)
{
  std::vector<W> vec;
  int s = options.size();
  int width = 0;
  for(int i=0;i<s;i++)
    {
      std::string s = options[i];
      W txt_1 = text(s, atlas, atlas_bm);
      if (width<size_x(txt_1)) { width = size_x(txt_1); }
      vec.push_back(txt_1);
    }
  int s2 = options.size();
  for(int i=0;i<s2;i++)
    {
      W txt_1 = vec[i];
      W txt_1a = left_align(txt_1, width);
      W txt_2 = margin(txt_1a, 5,5,5,5);
      W txt_3 = highlight(size_x(txt_2), size_y(txt_2));
      W txt_4 = layer(txt_2, txt_3);
      vec[i] = txt_4;
    }
  W w = array_y(&vec[0], vec.size(), 3);
  W w2 = button(size_x(w), size_y(w), c_list_item_title, c_list_item_title2);
  W w3 = layer(w2,w);
  W w5 = margin(w3,2,2,2,2);
  return w5;
}

template<class T>
class GenericMixed : public MixedI
{
public:
  GenericMixed(T val, std::string type) : val(val), type(type) { }
  virtual std::string Type() const { return type; }
  virtual int NumItems() const { return 1; }
  virtual std::string Name(int i) const { std::stringstream ss; ss << i; return ss.str(); }
  virtual std::string Value(int i) const { std::stringstream ss; ss << val; return ss.str(); }
  virtual std::string Print() const { return Value(0); }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new GenericMixed<T>(val,type); }
private:
  T val;
  std::string type;
};

class PairMixed : public MixedI
{
public:
  PairMixed(std::string name, MixedI &mx) : name(name), mx(mx) { }
  virtual std::string Type() const { return "pair"; }
  virtual int NumItems() const { return 1; }
  virtual std::string Name(int i) const { return name; }
  virtual std::string Value(int i) const { return mx.Print(); }
  virtual std::string Print() const
  {
    return " ( " + Name(0) + " => " + Value(0) + " ) "; 
  }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new PairMixed(name,mx); }
private:
  std::string name;
  MixedI &mx;
};

class ArrayMixed : public MixedI
{
public:
  ArrayMixed(std::vector<MixedI*> vec) : vec(vec) { }
  virtual std::string Type() const { return "array"; }
  virtual int NumItems() const { return vec.size(); }
  virtual std::string Name(int i) const 
  {
    std::stringstream ss;
    ss << i;
    return ss.str();
  }
  virtual std::string Value(int i) const {
    return vec[i]->Print();
  }
  virtual std::string Print() const
  {
    std::string res;
    res += " [ ";
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	if (i!=0) { res+=","; }
	res+=vec[i]->Print();
      }
    res+= " ] ";
    return res;
  }
  MixedI *Access(int i) const { return vec[i]; }
  MixedI *Clone() const { return new ArrayMixed(vec); }
private:
  std::vector<MixedI*> vec;
};

GameApi::MX GameApi::MixedApi::mx_float(float val)
{
  return add_mixed(e, new GenericMixed<float>(val,"float"));
}
GameApi::MX GameApi::MixedApi::mx_int(int val)
{
  return add_mixed(e, new GenericMixed<int>(val,"int"));
}
GameApi::MX GameApi::MixedApi::mx_bool(bool val)
{
  return add_mixed(e, new GenericMixed<bool>(val, "bool"));
}
GameApi::MX GameApi::MixedApi::mx_point(float x, float y, float z)
{
  Point val(x,y,z);
  return add_mixed(e, new GenericMixed<Point>(val, "point"));
}
GameApi::MX GameApi::MixedApi::mx_vector(float dx, float dy, float dz)
{
  Vector val(dx,dy,dz);
  return add_mixed(e, new GenericMixed<Vector>(val, "vector"));
}
GameApi::MX GameApi::MixedApi::mx_color(int r, int g, int b, int a)
{
  return add_mixed(e, new GenericMixed<Color>(Color(r,g,b,a), "color"));
}
GameApi::MX GameApi::MixedApi::mx_pair(std::string name, MX val)
{
  MixedI *val2 = find_mixed(e, val);
  return add_mixed(e, new PairMixed(name,*val2));
}
GameApi::MX GameApi::MixedApi::mx_string(std::string value)
{
  return add_mixed(e, new GenericMixed<std::string>(value,"string"));
}
GameApi::MX GameApi::MixedApi::mx_array(std::vector<MX> vec)
{
  std::vector<MixedI*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_mixed(e,vec[i]));
    }
  return add_mixed(e, new ArrayMixed(vec2));
}

int GameApi::MixedApi::mx_size(MX arr)
{
  MixedI *arr2 = find_mixed(e, arr);
  return arr2->NumItems();
}
class EmptyMixed : public MixedI 
{
public:
  virtual std::string Type() const { return "empty"; }
  virtual int NumItems() const { return 0; }
  virtual std::string Name(int i) const { return ""; }
  virtual std::string Value(int i) const { return ""; }
  virtual std::string Print() const { return "()"; }
  virtual MixedI *Access(int i) const { return 0; }
  virtual MixedI *Clone() const { return new EmptyMixed; }
};
GameApi::MX GameApi::MixedApi::mx_index(MX arr, int idx)
{
  MixedI *arr2 = find_mixed(e, arr);
  MixedI *item = arr2->Access(idx);
  if (!item) { return add_mixed(e, new EmptyMixed); }
  return add_mixed(e, item);
}
class CloneMixed : public MixedI
{
public:
  CloneMixed(MixedI *val) : val(val) { }
  virtual std::string Type() const { return val->Type(); }
  virtual int NumItems() const { return val->NumItems(); }
  virtual std::string Name(int i) const { return val->Name(i); }
  virtual std::string Value(int i) const { return val->Value(i); }
  virtual std::string Print() const { return val->Print(); }
  virtual MixedI *Access(int i) const { return val->Access(i); }
  virtual MixedI *Clone() const { return val->Clone(); }
private:
  MixedI *val;
};

GameApi::MX GameApi::MixedApi::mx_find(MX arr, std::string name)
{
  MixedI *arr2 = find_mixed(e, arr);
  int s = arr2->NumItems();
  for(int i=0;i<s;i++)
    {
      MixedI *item = arr2->Access(i);
      if (item->Type()!="pair") { continue; }
      if (name==item->Name(0))
	{
	  return add_mixed(e, item);
	}
    }
  std::stringstream ss(name);
  int idx;
  if (ss >> idx) {
    if (idx>=0 && idx<s) {
      MixedI *val = arr2->Access(idx);
      return add_mixed(e,new CloneMixed(val));
    }
  }
  return add_mixed(e, new EmptyMixed);
}

float GameApi::MixedApi::mx_to_float(MX val, float def)
{
  MixedI *item = find_mixed(e, val);
  if (item->Type()=="float") {
    std::stringstream ss(item->Value(0));
    float val=def;
    ss >> val;
    return val;
  }
  return def;
}
int GameApi::MixedApi::mx_to_int(MX val, int def)
{
  MixedI *item = find_mixed(e, val);
  if (item->Type()=="int") {
    std::stringstream ss(item->Value(0));
    int val=def;
    ss >> val;
    return val;
  }
  return def;
}

std::vector<std::string> GameApi::MixedApi::mx_names(MX val)
{
  MixedI *item = find_mixed(e, val);
  std::vector<std::string> names;
  int s = item->NumItems();
  for(int i=0;i<s;i++) names.push_back(item->Name(i));
  return names;
}
std::vector<GameApi::MX> GameApi::MixedApi::mx_values(MX val)
{
  MixedI *item = find_mixed(e, val);
  std::vector<GameApi::MX> values;
  int s = item->NumItems();
  for(int i=0;i<s;i++) values.push_back(add_mixed(e,new CloneMixed(item->Access(i))));
  return values;
}

class EmptyEvent : public Event
{
public:
  void execute(MainLoopEnv &e) { }
  void handle_event(MainLoopEvent &e) { }
  bool event_triggered() const { return false; }
};
class KeyEvent : public Event
{
public:
  KeyEvent(Event *next, int key_id, int key_type) : next(next), key_id(key_id), key_type(key_type) { 
    trigger = false;
  }
  void execute(MainLoopEnv &e) { return next->execute(e); }
  void handle_event(MainLoopEvent &e)
  {
    if (e.ch == key_id && e.type == key_type)
      {
	trigger = true;
      }
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger ||next->event_triggered();
    trigger = false;
    return t;
  }
private:
  Event *next;
  int key_id;
  int key_type;
  mutable bool trigger;
};
class TimerEvent : public Event
{
public:
  TimerEvent(Event *next, float time) : next(next), time(time) { 
    trigger = false;
  }
  
  void execute(MainLoopEnv &e) { 
    if (time > e.time*10.0 && time < e.time*10.0 + e.delta_time)
      {
	trigger = true;
      }
    return next->execute(e); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger||next->event_triggered();
    trigger = false;
    return t;
  }
private:
  Event *next;
  float time;
  mutable bool trigger;
};

class DeltaTimerEvent : public Event
{
public:
  DeltaTimerEvent(Event *next, Event *ev, float delta_time) : next(next), ev(ev), delta_time(delta_time) { 
    state = 0;
    trigger = false;
  }
  void execute(MainLoopEnv &e) {
    if (ev->event_triggered())
      {
	start_time = e.time*10.0;
	state = 1;
      }
    if (state == 1 && e.time*10.0 > start_time + delta_time)
      {
	state = 0;
	trigger = true;
      }
    return next->execute(e); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  bool event_triggered() const {
    bool t = trigger||next->event_triggered();
    trigger = false;
    return t;
  }
  
private:
  Event *next;
  Event *ev;
  float delta_time;
  float start_time;
  int state;
  mutable bool trigger;
};

class EmptyAction : public Action
{
public:
  MainLoopItem *get_main_loop(MainLoopItem *next) { return next; }
  void trigger() { }
};

class ChooseMLAction : public Action
{
public:
  ChooseMLAction(Action *next, MainLoopItem *alt) : next(next), alt(alt) { 
    triggered = false;
  }
  MainLoopItem *get_main_loop(MainLoopItem *nxt)
  {
    if (triggered) { return next->get_main_loop(alt); }
    return next->get_main_loop(nxt);
  }
  void trigger() {
    next->trigger();
    triggered = true;
  }
private:
  Action *next;
  MainLoopItem *alt;
  bool triggered;
};

class MoveMLAction : public Action, private MainLoopItem
{
public:
  MoveMLAction(Action *next, Movement *mov, float start_time, float end_time) : next(next), mov(mov), start_time(start_time), end_time(end_time) { 
    next_item = 0;
    current_time = 0.0;
  }
  virtual MainLoopItem *get_main_loop(MainLoopItem *next)
  {
    next_item = next;
    return this;
  }
  virtual void trigger()=0;

  virtual void execute(MainLoopEnv &e)
  {
    current_time = e.time*10.0;
    if (next_item) { next_item->execute(e); }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (next_item) { next_item->handle_event(e); }
  }
  virtual int shader_id() { 
    if (next_item) { return next_item->shader_id(); }
    return -1;
  }
private:
  Action *next;
  Movement *mov;
  float start_time;
  float end_time;
  MainLoopItem *next_item;
  float current_time;
};



struct Envi_2 {
  GameApi::EveryApi *ev;
  GameApi::MN move;
  GameApi::ML mainloop;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  GameApi::InteractionApi::Quake_data data;
  bool logo_shown = true;
  bool fpscounter = false;
  float start_time = 0.0f;
  GameApi::SH color_sh;
  GameApi::SH texture_sh;
  GameApi::SH arr_texture_sh;
  bool exit = false;
  float timeout = 100000.0f;
  int screen_width=800;
  int screen_height=600;
};
extern int async_pending_count;
extern std::string gameapi_seamless_url;
void blocker_iter(void *arg)
{
  Envi_2 *env = (Envi_2*)arg;
  //std::cout << "async: " << async_pending_count << std::endl;
  if (async_pending_count > 0) { env->logo_shown = true; }
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
    env->ev->mainloop_api.clear_3d(0xff000000);

    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { env->exit = true; }
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
	env->ev->shader_api.use(env->color_sh);
	env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));

	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	//env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->color_sh);

	GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
	GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
	GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

	if (env->fpscounter)
	  env->ev->mainloop_api.fpscounter();
	if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
	  {
	    env->exit = true;
	  }

    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    g_low->ogl->glGetError();
}
extern int async_pending_count;
int async_pending_count_previous=-1;

extern int score;
extern int hidden_score;

class MainLoopSplitter_win32_and_emscripten : public Splitter
{
public:
  MainLoopSplitter_win32_and_emscripten(GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
  }
  virtual void set_env(GameApi::Env *ei)
  {
    e = ei;
  }
  virtual void set_everyapi(GameApi::EveryApi *evi)
  {
    ev = evi;
  }
  virtual void Init()
  {
    score = 0;
    hidden_score = 0;

    Envi_2 &env = envi;
    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;
    env.ev = ev;
    
    GameApi::SH sh = env.ev->shader_api.colour_shader();
    GameApi::SH sh2 = env.ev->shader_api.texture_shader();
    GameApi::SH sh3 = env.ev->shader_api.texture_array_shader();
    
    // rest of the initializations
    env.ev->mainloop_api.init_3d(sh);
    env.ev->mainloop_api.init_3d(sh2);
    env.ev->mainloop_api.init_3d(sh3);
    env.ev->shader_api.use(sh);
    
    GameApi::ML ml = mainloop(*env.ev);
    if (async_pending_count > 0) { env.logo_shown = true; }
    
    env.mainloop = ml;
    
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    env.ev->mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url == "") {
	env.ev->mainloop_api.display_logo(*env.ev);
      } else {
	env.ev->mainloop_api.display_seamless(*env.ev);
      }
    } else {
	env.ev->mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     env.ev->mainloop_api.alpha(true);
     g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
     GameApi::MainLoopApi::Event e;
     while((e = env.ev->mainloop_api.get_event()).last==true)
       {
	 /* this eats all events from queue */
       }
  }
  virtual int Iter()
  {
    Envi_2 *env = (Envi_2*)&envi;
    //std::cout << "async: " << async_pending_count << std::endl;
    if (async_pending_count > 0) { env->logo_shown = true; }
    if (async_pending_count != async_pending_count_previous)
      {
	std::cout << "ASync pending count=" << async_pending_count << std::endl;
	async_pending_count_previous = async_pending_count;
      }
    if (env->logo_shown)
      {
	bool b = false;
	if (gameapi_seamless_url=="") {
	  b = env->ev->mainloop_api.logo_iter();
	} else {
	  b = env->ev->mainloop_api.seamless_iter();
	}
	if (b && async_pending_count==0) { env->logo_shown = false;
	  env->ev->mainloop_api.reset_time();
	  env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
	}
	return -1;
      }

    env->ev->mainloop_api.clear_3d(0xff000000);
    
    // handle esc event
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { env->exit = true; return 0; }
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
    env->ev->shader_api.use(env->color_sh);
    env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->texture_sh);
    env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    
    env->ev->shader_api.use(env->arr_texture_sh);
    env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->color_sh);
    
    GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
    GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
    GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
    
    env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

    if (env->fpscounter)
      env->ev->mainloop_api.fpscounter();
    if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
      {
	env->exit = true;
      }
    
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    g_low->ogl->glGetError();
    return -1;
  }
  virtual void Destroy()
  {
    // this is needed for win32 build in editor
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
  }
  virtual Splitter* NextState(int code)
  {
    return 0;
  }
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }

private:
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
  Envi_2 envi;
};

extern int score;
extern int hidden_score;

class MainLoopBlocker_win32_and_emscripten : public Blocker
{
public:
  MainLoopBlocker_win32_and_emscripten(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : e(e), ev(ev), code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
    score = 0;
    hidden_score=0;
  }
  void SetTimeout(float duration) {
  }
  void Execute()
  {
    
    Envi_2 env;

    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;

    GameApi::SH sh = ev.shader_api.colour_shader();
    GameApi::SH sh2 = ev.shader_api.texture_shader();
    GameApi::SH sh3 = ev.shader_api.texture_array_shader();
    
    // rest of the initializations
    ev.mainloop_api.init_3d(sh);
    ev.mainloop_api.init_3d(sh2);
    ev.mainloop_api.init_3d(sh3);
    ev.shader_api.use(sh);
    
    GameApi::ML ml = mainloop(ev);
    if (async_pending_count > 0) { env.logo_shown = true; }

    //GameApi::MN mn0 = ev.move_api.empty();
    //GameApi::MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, -400.0);
    //GameApi::ML ml2 = ev.move_api.move_ml(ev, ml, mn);
    env.mainloop = ml;
    
    env.ev = &ev;
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    ev.mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url=="") {
	ev.mainloop_api.display_logo(ev);
      } else {
	ev.mainloop_api.display_seamless(ev);
      }
    } 
    else 
      {
	ev.mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     ev.mainloop_api.alpha(true);
     g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
    GameApi::MainLoopApi::Event e;
    while((e = env.ev->mainloop_api.get_event()).last==true)
      {
	/* this eats all events from queue */
      }

#ifndef EMSCRIPTEN
    while(!env.exit) {
      blocker_iter(&env);
      //ev.mainloop_api.delay(10);
    }
#else
      emscripten_set_main_loop_arg(blocker_iter, (void*)&env, 0,1);
#endif 
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
  }
  private:
  
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
};

EXPORT GameApi::BLK GameApi::BlockerApi::game_window(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{
  Blocker *blk = new MainLoopBlocker_win32_and_emscripten(e,ev,ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_blocker(e, blk);
}
EXPORT GameApi::RUN GameApi::BlockerApi::game_window2(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{
  Splitter *spl = new MainLoopSplitter_win32_and_emscripten(ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_splitter(e, spl);
}

class SplitterSeq : public Splitter
{
public:
  SplitterSeq(std::vector<Splitter*> vec) : vec(vec) { current_seq=0; }
  virtual void Init() {
    vec[current_seq]->e = e;
    vec[current_seq]->ev = ev;
    vec[current_seq]->Init();
  }
  virtual int Iter() { 
    int exit = vec[current_seq]->Iter(); return exit; 
  }
  virtual void Destroy() { vec[current_seq]->Destroy(); }
  virtual Splitter* NextState(int code) {
    current_seq++;
    int s = vec.size();
    if (current_seq>=s) { return 0; }
    return vec[current_seq];
  }
private:
  std::vector<Splitter*> vec;
  int current_seq;
};

EXPORT GameApi::RUN GameApi::BlockerApi::run_seq(GameApi::EveryApi &ev, std::vector<RUN> vec)
{
  std::vector<Splitter*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      Splitter *spl = find_splitter(e, vec[i]);
      vec2.push_back(spl);
    }
  return add_splitter(e, new SplitterSeq(vec2));
}

class BlockerSeq : public Blocker
{
public:
  BlockerSeq(GameApi::EveryApi &ev, std::vector<Blocker*> vec) : ev(ev), vec(vec) { }
  void SetTimeout(float duration) {
  }
  void Execute() 
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Blocker *blk = vec[i];
	blk->Execute();
      }
  }
private:
  GameApi::EveryApi &ev;
  std::vector<Blocker*> vec;
};
EXPORT GameApi::BLK GameApi::BlockerApi::game_seq(GameApi::EveryApi &ev,std::vector<BLK> vec)
{
  std::vector<Blocker*> blks;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      Blocker *blk = find_blocker(e,vec[i]);
      blks.push_back(blk);
    }
  return add_blocker(e, new BlockerSeq(ev,blks));
}
EXPORT void GameApi::BlockerApi::run(BLK blk)
{
  Blocker *blk2 = find_blocker(e, blk);
  blk2->Execute();
}


Splitter *splitter_current = 0;
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

#ifdef VIRTUAL_REALITY
void vr_run2(Splitter *spl2);
#endif
EXPORT void GameApi::BlockerApi::run2(EveryApi &ev, RUN spl)
{
  Splitter *spl2 = find_splitter(e, spl);
  spl2->e = &e;
  spl2->ev = &ev;
  spl2->Init();
  splitter_current = spl2;
#ifdef EMSCRIPTEN
#ifdef VIRTUAL_REALITY
  vr_run2(spl2);
#else
  emscripten_set_main_loop_arg(splitter_iter2, (void*)spl2, 0,1);
#endif
#else
  while(splitter_current) {
    splitter_iter2((void*)splitter_current);
  }
#endif
}

class KeyFrameMesh : public VertexAnimNode
{
public:
  KeyFrameMesh(int coll) : coll(coll) { }
  int NumKeyFrames() { return 2; }
  virtual float StepDuration(int keyframe_gap) const { return 0.001; }
  int FaceColl(int keyframe_gap) const
  {
    return coll;
  }
  int Lines(int keyframe_gap) const
  {
    return -1;
  }
  int Points(int keyframe_gap) const
  {
    return -1;
  }
  Point KeyFramePoint(Point p, int keyframe_gap, int point_num) const
  {
    return p;
  }
private:
  int coll;
};

class KeyFrameLines : public VertexAnimNode
{
public:
  KeyFrameLines(int coll) : coll(coll) { }
  int NumKeyFrames() { return 2; }
  virtual float StepDuration(int keyframe_gap) const { return 0.001; }
  int FaceColl(int keyframe_gap) const
  {
    return -1;
  }
  int Lines(int keyframe_gap) const
  {
    return coll;
  }
  int Points(int keyframe_gap) const
  {
    return -1;
  }
  Point KeyFramePoint(Point p, int keyframe_gap, int point_num) const
  {
    return p;
  }
private:
  int coll;
};

EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_mesh(P part)
{
  return add_vertex_anim(e, new KeyFrameMesh(part.id));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_lines(LI part)
{
  return add_vertex_anim(e, new KeyFrameLines(part.id));
}
class ChangePosLI : public LineCollection
{
public:
  ChangePosLI(LineCollection &coll, LineCollection &orig, PointTransform &trans, float delta_time, bool dif) : coll(coll), orig(orig), trans(trans), delta_time(delta_time),dif(dif) {}

  virtual int NumLines() const { return coll.NumLines(); }
  virtual Point LinePoint(int line, int point) const
  {
    Point p;
    if (!dif) {
      p = coll.EndLinePoint(line,point);
    } else {
      p = orig.LinePoint(line,point);
      return trans.Map(p, 0.0);
    }
    return p;

  }
  virtual Point EndLinePoint(int line, int point) const {
    Point p;
    if (!dif) {
      p = coll.EndLinePoint(line,point);
    } else {
      p = orig.LinePoint(line,point);
    }
    return trans.Map(p, delta_time);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

  
private:
  LineCollection &coll;
  LineCollection &orig;
  PointTransform &trans;
  float delta_time;
  bool dif;
};

class ChangePos : public ForwardFaceCollection
{
public:
  ChangePos(FaceCollection &coll, FaceCollection &orig, PointTransform &trans, float delta_time, bool dif) : ForwardFaceCollection(coll), orig(orig), trans(trans), delta_time(delta_time), dif(dif) { }
  virtual Point FacePoint(int face, int point) const
  {
    // this FacePoint/EndFacePoint logic looks odd, but is needed
    // because of chaining of facecollections.
    Point p;
    if (!dif) {
      p = ForwardFaceCollection::EndFacePoint(face,point);
    } else {
      p = orig.FacePoint(face,point);
      return trans.Map(p, 0.0);
    }
    return p;
  }
  virtual Point EndFacePoint(int face, int point) const
  {
    Point p;
    if (!dif) {
      p = ForwardFaceCollection::EndFacePoint(face,point);
    } else {
      p = orig.FacePoint(face,point);
    }
    return trans.Map(p, delta_time);
  }
private:
  FaceCollection &orig;
  PointTransform &trans;
  float delta_time;
  bool dif;
};

EXPORT GameApi::LI GameApi::VertexAnimApi::change_pos_li(LI p, LI orig, PTT transform, float delta_time, bool different_pos)
{
  LineCollection *li = find_line_array(e, p);
  LineCollection *orig2 = find_line_array(e, orig);
  PointTransform *trans = find_point_transform(e, transform);
  return add_line_array(e, new ChangePosLI(*li, *orig2, *trans, delta_time, different_pos));
}

EXPORT GameApi::P GameApi::VertexAnimApi::change_pos(P p, P orig, PTT transform, float delta_time, bool different_pos)
{
  FaceCollection *coll = find_facecoll(e, p);
  FaceCollection *orig2 = find_facecoll(e, orig);
  PointTransform *trans = find_point_transform(e, transform);
  return add_polygon2(e, new ChangePos(*coll, *orig2, *trans, delta_time, different_pos));
}

class CurveAccessor : public PointTransform
{
public:
  CurveAccessor(float start_time, Curve<Point> &curve, CurvePos &pos, float time_multiplier) : start_time(start_time), curve(curve), pos(pos), time_mult(time_multiplier) { }
  Point Map(Point p, float delta_time) const {
    float val = pos.FindPos(p, curve.Size());
    Point pp = p+curve.Index(start_time*time_mult+val+time_mult*(delta_time));
    return pp;
  }
private:
  float start_time;
  Curve<Point> &curve;
  CurvePos &pos;
  float time_mult;
};
class RotAccessor : public PointTransform
{
public:
  RotAccessor(float start_time, float time_mult, float nx, float ny, float nz, float dist_angle) : start_time(start_time), time_mult(time_mult), nx(nx), ny(ny), nz(nz), dist_angle(dist_angle) { }
  Point Map(Point p, float delta_time) const {
    float progress = start_time*time_mult + time_mult*delta_time;
    Matrix m = Matrix::RotateAroundAxis(Vector(nx,ny,nz), progress);
    return p*m;
  }
private:
  float start_time;
  float time_mult;
  float nx,ny,nz;
  float dist_angle;
};
EXPORT GameApi::PTT GameApi::VertexAnimApi::curve_accessor(C curve, CPP pos, float start_time, float time_mult)
{
  Curve<Point> *cur = find_curve(e, curve);
  CurvePos *cur_pos = find_curve_pos(e, pos);
  return add_point_transform(e, new CurveAccessor(start_time, *cur, *cur_pos, time_mult));
}


EXPORT GameApi::PTT GameApi::VertexAnimApi::rot_accessor(float start_time, float time_mult, float nx, float ny, float nz, float dist_angle)
{
  return add_point_transform(e, new RotAccessor(start_time, time_mult, nx, ny,nz, dist_angle));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::curve_trans(EveryApi &ev, KF kf, C curve, CPP pos, int numsamples, float duration)
{
  Curve<Point> *cur = find_curve(e, curve);
  float curve_length = cur->Size();
  int s = numsamples;
  //float delta_pos = curve_length/numsamples;
  float delta_time = duration/numsamples;
  for(int i=0;i<s;i++)
    {
      PTT pt = curve_accessor(curve, pos, i*delta_time, curve_length/duration);
      kf = keyframe_bind2(ev, kf, pt, delta_time, true);
    }
  return kf;
}
EXPORT GameApi::KF GameApi::VertexAnimApi::sample_rot(EveryApi &ev, KF kf, float nx, float ny, float nz, float angle, int numsamples, float duration)
{
  //Curve<Point> *cur = find_curve(e, curve);
  float curve_length = angle; //cur->Size();
  int s = numsamples;
  //float delta_pos = curve_length/numsamples;
  float delta_time = duration/numsamples;
  for(int i=0;i<s;i++)
    {
      //PTT empty = empty_trans();
      //PTT pt = rotate_trans2(empty, delta_time, nx,ny,nz, angle/numsamples);
      PTT pt = rot_accessor(i*delta_time, curve_length/duration, nx,ny,nz, angle);
      kf = keyframe_bind2(ev, kf, pt, delta_time, true);
    }
  return kf;
}

// change pos needs to be different.
class KeyFrameBind : public VertexAnimNode
{
public:
  KeyFrameBind(GameApi::EveryApi &ev, VertexAnimNode *va, GameApi::PTT pt, float delta_time, bool dif) : ev(ev), va(va), pt(pt), delta_time(delta_time), dif(dif) { temp = 0; }
  int NumKeyFrames() { return va->NumKeyFrames() + 1; }
  virtual float StepDuration(int keyframe_gap) const { 
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->StepDuration(keyframe_gap); }
    return delta_time;
  }
  int FaceColl(int keyframe_gap) const
  {
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->FaceColl(keyframe_gap); }
    int coll = va->FaceColl(prev_count-1);
    int orig = va->FaceColl(0);
    GameApi::P p = { coll };
    GameApi::P orig2 = { orig };
    if (coll==-1 || orig==-1) {
      return -1;
    }
    GameApi::P p2 = ev.vertex_anim_api.change_pos(p, orig2, pt, delta_time, dif);
    return p2.id;
  }
  int Lines(int keyframe_gap) const
  {
    int prev_count = va->NumKeyFrames();
    if (keyframe_gap < prev_count) { return va->Lines(keyframe_gap); }
    int lines = va->Lines(prev_count-1);
    int orig = va->Lines(0);
    GameApi::LI p = { lines };
    GameApi::LI orig2 = { orig };
    if (lines==-1||orig==-1) {
      return -1;
    }
    GameApi::LI p2 = ev.vertex_anim_api.change_pos_li(p, orig2, pt, delta_time,dif);
    return p2.id;
  }
private:
  GameApi::EveryApi &ev;
  VertexAnimNode *va;
  GameApi::PTT pt;
  float delta_time;
  mutable FaceCollection *temp;
  bool dif;
};


EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_bind(EveryApi &ev, KF keyframe, PTT transform, float delta_time)
{
  VertexAnimNode *va = find_vertex_anim(e, keyframe);
  return add_vertex_anim(e, new KeyFrameBind(ev,va,transform,delta_time, false));
}
EXPORT GameApi::KF GameApi::VertexAnimApi::keyframe_bind2(EveryApi &ev, KF keyframe, PTT transform, float delta_time, bool dif)
{
  VertexAnimNode *va = find_vertex_anim(e, keyframe);
  return add_vertex_anim(e, new KeyFrameBind(ev,va,transform,delta_time, dif));
}

class EmptyTransform : public PointTransform
{
public:
  EmptyTransform() { }
  Point Map(Point p, float delta_time) const { return p; }
};
class TransformSpeed : public PointTransform
{
public:
  TransformSpeed(PointTransform *prev) : prev(prev) { }
  virtual Matrix Mat() const=0;
  Point Map(Point p, float delta_time) const
  {
    Matrix m = Mat();
    delta_time /= 10.0;
    m*=delta_time;
    return prev->Map(p,delta_time)*m;
  }
private:
  PointTransform *prev;
};
class TransformDist : public PointTransform
{
public:
  TransformDist(PointTransform *prev, float duration) : prev(prev), duration(duration) { 
    if (duration==0.0f) duration=10.0f;
  }
  virtual Matrix Mat() const=0;
  Point Map(Point p, float delta_time) const
  {
    Matrix m = Mat();
    m*=1.0/duration;
    m*=delta_time;
    return prev->Map(p,delta_time)*m;
  }
private:
  PointTransform *prev;
  float duration;
};

GameApi::PTT GameApi::VertexAnimApi::empty_trans()
{
  return add_point_transform(e, new EmptyTransform);
}

class TranslateSpeed : public TransformSpeed
{
public:
  TranslateSpeed(PointTransform *prev, float speed_x, float speed_y, float speed_z) : TransformSpeed(prev), speed_x(speed_x), speed_y(speed_y), speed_z(speed_z) { }
  virtual Matrix Mat() const { return Matrix::Translate(speed_x, speed_y, speed_z); }
private:
  float speed_x, speed_y, speed_z;
};
class TranslateDist : public TransformDist
{
public:
  TranslateDist(PointTransform *prev, float duration, float dist_x, float dist_y, float dist_z) : TransformDist(prev, duration), dist_x(dist_x), dist_y(dist_y), dist_z(dist_z) { }
  virtual Matrix Mat() const { return Matrix::Translate(dist_x, dist_y, dist_z); }
private:
  float dist_x, dist_y, dist_z;
};
class RotateSpeed : public TransformSpeed
{
public:
  RotateSpeed(PointTransform *prev, float nx, float ny, float nz, float angle) : TransformSpeed(prev), nx(nx), ny(ny), nz(nz), angle(angle) { }
  virtual Matrix Mat() const { return Matrix::RotateAroundAxis(Vector(nx,ny,nz),angle); }
private:
  float nx,ny,nz;
  float angle;
};
class RotateDist : public TransformDist
{
public:
  RotateDist(PointTransform *prev, float duration, float nx, float ny, float nz, float angle) : TransformDist(prev, duration), nx(nx), ny(ny), nz(nz) { }
  virtual Matrix Mat() const { return Matrix::RotateAroundAxis(Vector(nx,ny,nz), angle); }
private:
  float nx,ny,nz;
  float angle;
};


class ScaleSpeed : public TransformSpeed
{
public:
  ScaleSpeed(PointTransform *prev, float sx, float sy, float sz) : TransformSpeed(prev), sx(sx), sy(sy), sz(sz) { }
  virtual Matrix Mat() const { return Matrix::Scale(sx,sy,sz); }
private:
  float sx,sy,sz;
};
class ScaleDist : public TransformDist
{
public:
  ScaleDist(PointTransform *prev, float duration, float sx, float sy, float sz) : TransformDist(prev, duration), sx(sx), sy(sy), sz(sz) { }
  virtual Matrix Mat() const { return Matrix::Scale(sx,sy,sz); }
private:
  float sx,sy,sz;
};


EXPORT GameApi::PTT GameApi::VertexAnimApi::translate_trans(PTT prev, float speed_x, float speed_y, float speed_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new TranslateSpeed(prev2, speed_x, speed_y, speed_z));
}
EXPORT GameApi::PTT GameApi::VertexAnimApi::translate_trans2(PTT prev, float duration, float dist_x, float dist_y, float dist_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new TranslateDist(prev2, duration, dist_x, dist_y, dist_z));

}

EXPORT GameApi::PTT GameApi::VertexAnimApi::rotate_trans(PTT prev, float nx, float ny, float nz, float speed_angle)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new RotateSpeed(prev2, nx, ny, nz, speed_angle));
}
EXPORT GameApi::PTT GameApi::VertexAnimApi::rotate_trans2(PTT prev, float duration, float nx, float ny, float nz, float dist_angle)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new RotateDist(prev2, duration, nx, ny, nz, dist_angle));

}
EXPORT GameApi::PTT GameApi::VertexAnimApi::scale_trans(PTT prev, float scale_speed_x, float scale_speed_y, float scale_speed_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new ScaleSpeed(prev2, scale_speed_x, scale_speed_y, scale_speed_z));

}

EXPORT GameApi::PTT GameApi::VertexAnimApi::scale_trans2(PTT prev, float duration, float scale_dist_x, float scale_dist_y, float scale_dist_z)
{
  PointTransform *prev2 = find_point_transform(e, prev);
  return add_point_transform(e, new ScaleDist(prev2, duration, scale_dist_x, scale_dist_y, scale_dist_z));

}

GameApi::P linear_func(float val, GameApi::P c_start, GameApi::P c_end, float time_start, float time_end)
{
  float time_avg = (time_start + time_end)/2.0f;
  if (val < time_avg) { return c_start; }
  return c_end;
}

class AnimFaceCollection : public ForwardFaceCollection
{
public:
  AnimFaceCollection(FaceCollection *coll1, FaceCollection *coll2) : ForwardFaceCollection(*coll1), coll2(coll2) { }
  
  virtual Point EndFacePoint(int face, int point) const { return coll2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return coll2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return coll2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return coll2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return coll2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return coll2->TexCoord(face,point); }
  virtual float EndTexCoord3(int face, int point) const { return TexCoord3(face,point); }

private:
  FaceCollection *coll2;
};

class VertexAnimMainLoop : public MainLoopItem
{
public:
  VertexAnimMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, VertexAnimNode *va) : env(env), ev(ev), va(va) 
  {
    Prepare_1();
    Prepare_2();
    shader.id=-1;
    firsttime = true;
  }
  void Prepare_1() {
    std::vector<GameApi::P> vec;
    int s = va->NumKeyFrames();
    for(int i=0;i<s;i++)
      {
        int coll = va->FaceColl(i);
	GameApi::P p = { coll };
	vec.push_back(p);
      }
    ranges = vec;

    std::vector<GameApi::LI> vec2;
    int s2 = va->NumKeyFrames();
    for(int i=0;i<s2;i++)
      {
	int coll = va->Lines(i);
	GameApi::LI li = { coll };
	vec2.push_back(li);
      }
    li_ranges = vec2;

  }
  void Prepare_2()
  {
    std::vector<GameApi::VA> vas;
    int s = ranges.size();
    for(int i=0;i<s;i++)
      {
	GameApi::P p1 = ranges[i];
	if (p1.id==-1) {
	  GameApi::VA va;
	  va.id = -1;
	  vas.push_back(va);
	} else {
	  GameApi::VA va = ev.polygon_api.create_vertex_array(p1);
	  vas.push_back(va);
	}
      }
    ranges2 = vas;

    std::vector<GameApi::LLA> vas2;
    int s2 = li_ranges.size();
    for(int i=0;i<s2;i++)
      {
	GameApi::LI li = li_ranges[i];
	if (li.id==-1) {
	  GameApi::LLA lla;
	  lla.id = -1;
	  vas2.push_back(lla);
	} else {
	  GameApi::LLA lla = ev.lines_api.prepare(li);
	  vas2.push_back(lla);
	}
      }
    li_ranges2 = vas2;

  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e) 
  {
    if (ranges2.size()<1) { return; }
    GameApi::SH sh = { e.sh_color };
    if (ranges2[0].id!=-1) {
    if (ev.polygon_api.is_texture(ranges2[0]))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(ranges2[0]))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }
    }

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ranges2[0].id!=-1) {
    if (ev.polygon_api.is_texture(ranges2[0]))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(ranges2[0]))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
    }
#endif
    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	//ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);


    //std::cout << e.time << " " << start_time << std::endl;
    float time = e.time*10.0;
    int s = va->NumKeyFrames();
    float currenttime = 0.0f;
    int choose = -1;
    float step_size = 1.0f;
    for(int i=0;i<s;i++)
      {
	float val = va->StepDuration(i);
	if (currenttime + val > time && currenttime < time)
	  {
	    step_size = val;
	    choose=i;
	    break;
	  }
	currenttime += val;
      }
    bool end = false;
    if (choose==-1)
      {
	choose = ranges2.size()-1;
	end = true;
      }
    if (choose!=-1) {
      //std::cout << "choose: " << choose << ": " << (time-currenttime)/step_size << std::endl;
      ev.shader_api.use(sh);
      if (!end)
	ev.shader_api.set_var(sh, "in_POS", (time - currenttime)/step_size); 
      else
	ev.shader_api.set_var(sh, "in_POS", 1.0f);
      if (ranges2[choose].id != -1)
	ev.polygon_api.render_vertex_array(ranges2[choose]);
      if (li_ranges2[choose].id != -1)
	ev.lines_api.render(li_ranges2[choose]);
    }
    ev.shader_api.unuse(sh);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  VertexAnimNode *va;
  std::vector<GameApi::P> ranges;
  std::vector<GameApi::VA> ranges2;
  std::vector<GameApi::LI> li_ranges;
  std::vector<GameApi::LLA> li_ranges2;
  bool firsttime;
  //GameApi::SH sh;
  GameApi::SH shader;

  float start_time;
};

EXPORT GameApi::ML GameApi::VertexAnimApi::vertex_anim_render(EveryApi &ev, KF kf)
{
  VertexAnimNode *va = find_vertex_anim(e, kf);
  return add_main_loop(e, new VertexAnimMainLoop(e,ev, va));
  
}
class RepeatVertexAnimNode : public VertexAnimNode
{
public:
  RepeatVertexAnimNode(VertexAnimNode *next, int repeat_count) : next(next), repeat_count(repeat_count) { }
  virtual int NumKeyFrames() { return next->NumKeyFrames()*repeat_count; } // n
  virtual float StepDuration(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->StepDuration(keyframe_gap);
  }
  virtual int FaceColl(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->FaceColl(keyframe_gap);
  }
  virtual int Lines(int keyframe_gap) const
  {
    int s = next->NumKeyFrames();
    while(keyframe_gap >= s) keyframe_gap-=s;
    return next->Lines(keyframe_gap);
  }
private:
  VertexAnimNode *next;
  int repeat_count;
};
EXPORT GameApi::KF GameApi::VertexAnimApi::repeat_keyframes(KF rep, int count)
{
  VertexAnimNode *va = find_vertex_anim(e, rep);
  return add_vertex_anim(e, new RepeatVertexAnimNode(va,count));
}

std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;
struct ASyncCallback { void (*fptr)(void*); void *data; };
struct ASyncCallback2 { std::string url; ASyncCallback *cb; };
//std::map<std::string, ASyncCallback*> load_url_callbacks;
std::vector<ASyncCallback2> load_url_callbacks;
void add_async_cb(std::string url, ASyncCallback *cb)
{
  ASyncCallback2 cb2;
  cb2.url = url;
  cb2.cb = cb;
  load_url_callbacks.push_back(cb2);
}
ASyncCallback *rem_async_cb(std::string url)
{
  int s = load_url_callbacks.size();
  int i = 0;
  for(;i<s;i++)
    {
      if (load_url_callbacks[i].url==url) break;
    }
  if (i==s) return 0;
  ASyncCallback *cb = load_url_callbacks[i].cb;
  load_url_callbacks.erase(load_url_callbacks.begin()+i);
  return cb;
}


std::string striphomepage(std::string);
void onprogress_async_cb(unsigned int tmp, void *, int, int) { }
void onerror_async_cb(unsigned int tmp, void *arg, int, const char*)
{
  std::cout << "ERROR: url loading error! " << std::endl;
    char *url = (char*)arg;
    std::string url_str(url);
  std::string url_only(striphomepage(url_str));
    load_url_buffers_async[url_only] = (std::vector<unsigned char>*)-1;
    async_pending_count--;
    std::cout << "ASync pending dec (onerror_async_cb) -->" << async_pending_count << std::endl;
    
    ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  if (cb) {
    std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
  }

}
std::string striphomepage(std::string);
std::string stripprefix(std::string s)
{
  int len = strlen("load_url.php?url=");
  return s.substr(len,s.size()-len);
}
void onload_async_cb(unsigned int tmp, void *arg, void *data, unsigned int datasize)
{

  if (datasize==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
  }
  std::vector<unsigned char> buffer;
  unsigned char *dataptr = (unsigned char*)data;
  for(unsigned int i=0;i<datasize;i++) { buffer.push_back(dataptr[i]); }
  
  char *url = (char*)arg;
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,7,15,url_only2);
  }
  
  //std::cout << "url loading complete! " << url_str << std::endl;
  load_url_buffers_async[url_only] = new std::vector<unsigned char>(buffer);
  async_pending_count--;
  //std::cout << "ASync pending dec (onload_async_cb) -->" << async_pending_count<< std::endl;
  
  //std::cout << "Async cb!" << url_only << std::endl;
  ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  if (cb) {
    //std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
  }

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url_only2);
  }

}

std::vector<unsigned char> load_from_url(std::string url);

void ASyncLoader::set_callback(std::string url, void (*fptr)(void*), void *data)
{
  // progress bar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url);


  url = "load_url.php?url=" + url;
  ASyncCallback* cb = new ASyncCallback;
  cb->fptr = fptr;
  cb->data = data;
  //load_url_callbacks[url] = cb;
  add_async_cb(url,cb);



  //std::cout << "async set callback" << url << std::endl;
}
void ASyncLoader::load_urls(std::string url, std::string homepage)
  {
  // progress bar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url);

#ifdef EMSCRIPTEN
  std::string url2 = "load_url.php";
  std::string urlend = "url=" + url;
  std::string url3 = urlend + "&homepage=" + homepage;

  url = "load_url.php?url=" + url + "&homepage=" + homepage;

    //std::cout << "url loading started! " << url << std::endl;

    // if we have already loaded the same url, don't load again
    if (load_url_buffers_async[url]) { 
      ASyncCallback *cb = rem_async_cb(url); //load_url_callbacks[url];
      if (cb) {
	//std::cout << "Load cb!" << url << std::endl;
	(*cb->fptr)(cb->data);
      }

      { // progressbar
	std::string url_plain = stripprefix(url);
	int s = url_plain.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url_plain[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url_plain);
      }

      return; 
    }
    char *buf2 = new char[url2.size()+1];
    std::copy(url2.begin(), url2.end(), buf2);
    buf2[url2.size()]=0;

    char *buf3 = new char[url.size()+1];
    std::copy(url.begin(), url.end(), buf3);
    buf3[url.size()]=0;
    
    async_pending_count++;
    //    std::cout << "ASync pending inc (load_urls) -->" << async_pending_count << std::endl;

    //emscripten_async_wget_data(buf2, (void*)buf2 , &onload_async_cb, &onerror_async_cb);
    emscripten_async_wget2_data(buf2, "POST", url3.c_str(), (void*)buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
#else
  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,7,15,url);
  }


    std::string url2 = "load_url.php?url=" + url ;
    if (load_url_buffers_async[url2]) { return; }
    std::vector<unsigned char> buf = load_from_url(url);
    if (buf.size()==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
    }
    load_url_buffers_async[url2] = new std::vector<unsigned char>(buf);
    //std::cout << "Async cb!" << url2 << std::endl;
    ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
    if (cb) {
      //std::cout << "Load cb!" << url2 << std::endl;
      (*cb->fptr)(cb->data);
    }

  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url);
  }


#endif
  }
std::vector<unsigned char> *ASyncLoader::get_loaded_data(std::string url) const
  {
    url = "load_url.php?url=" + url;
    //std::cout << "url fetch " << url << std::endl;
    return load_url_buffers_async[url];
  }


class StringDisplayToBitmap : public Bitmap<int>
{
public:
  StringDisplayToBitmap(StringDisplay &sd, int def) : sd(sd),def(def) {}
  void Prepare() { }
  int SizeX() const
  {
    int s = sd.Count();
    int maxx = 0;
    for(int i=0;i<s;i++)
      {
	int x = sd.X(i) + sd.SX(i);
	if (x>maxx) maxx = x;
      }
    return maxx;
  }
  int SizeY() const
  {
    int s = sd.Count();
    int maxy = 0;
    for(int i=0;i<s;i++)
      {
	int y = sd.Y(i) + sd.SY(i);
	if (y>maxy) maxy = y;
      }
    return maxy;

  }
  int Map(int x, int y) const
  {
    int s = sd.Count();
    //int maxy = 0;
    for(int i=0;i<s;i++)
      {
	int xx = sd.X(i);
	if (x<xx) continue;
	int yy = sd.Y(i);
	if (y<yy) continue;
	if (x>xx+sd.SX(i)) continue;
	if (y>yy+sd.SY(i)) continue;
	return sd.Map(i, x-xx, y-yy);
      }
    return def;
  }
private:
  StringDisplay &sd;
  int def;
};
GameApi::BM GameApi::FontApi::string_display_to_bitmap(SD sd, int def)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StringDisplay *sdd = find_string_display(e, sd);
  Bitmap<int> *bm1 = new StringDisplayToBitmap(*sdd, def);
  env->deletes.push_back(std::shared_ptr<void>(bm1)); 
  Bitmap<Color> *bm2 = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(0,0,0,0), *bm1);
  return add_color_bitmap(e, bm2);
}

class ChooseGlyphFromFont : public GlyphInterface
{
public:
  ChooseGlyphFromFont(FontInterface &fi, long idx) : fi(fi), idx(idx) { }
  virtual int Top() const { return fi.Top(idx); }
  virtual int SizeX() const { return fi.SizeX(idx); }
  virtual int SizeY() const { return fi.SizeY(idx); }
  virtual int Map(int x, int y) const
  {
    return fi.Map(idx,x,y);
  }
private:
  FontInterface &fi;
  long idx;
};
GameApi::GI GameApi::FontApi::choose_glyph_from_font(FI font, long idx)
{
  FontInterface *fi = find_font_interface(e, font);
  return add_glyph_interface(e, new ChooseGlyphFromFont(*fi, idx));
}

class StringDisplayFromGlyphs : public StringDisplay
{
public:
  StringDisplayFromGlyphs(std::vector<GlyphInterface*> vec, std::string str, int x_gap, int empty_line_height) : vec(vec), x_gap(x_gap), str(str), empty_line_height(empty_line_height) { }
  virtual int Count() const { return vec.size(); }
  virtual int X(int c) const
  {
    int s = vec.size();
    int x = 0;
    for(int i=0;i<s;i++)
      {
	if (i==c) { return x; }
	GlyphInterface *gi = vec[i];
	x+=gi->SizeX();
	x+=x_gap;
	if (str[i]=='\n') x=0;
      }
    return 0;
  }
  virtual int Y(int c) const
  {
    //int s = str.size();
    int y = 0;
    int prev_pos = 0;
    for(int i=0;i<c;i++)
      {
	if (str[i]=='\n') { 
	  int maxy = 0;
	  if (prev_pos+1 >= c) { maxy = empty_line_height; }
	  for(int j=prev_pos+1;j<c;j++)
	    {
	      maxy = std::max(maxy, vec[j]->Top() + vec[j]->SizeY());
	    }
	  y+=maxy;
	  prev_pos = i;
	}
      }
    return y+PositiveDelta()+vec[c]->Top();
  }
  virtual int SX(int c) const
  {
    return vec[c]->SizeX();
  }
  virtual int SY(int c) const
  {
    return vec[c]->SizeY();
  }
  virtual int Map(int c, int x, int y) const
  {
    return vec[c]->Map(x,y);
  }
  int PositiveDelta() const
  {
    int sz = vec.size();
    int min_delta=0;
    for(int i=0;i<sz;i++)
      {
	int delta = vec[i]->Top();
	if (delta<min_delta) min_delta=delta;
      }
    return -min_delta;
  }
private:
  std::vector<GlyphInterface*> vec;
  int x_gap;
  std::string str;
  int empty_line_height;
};

GameApi::SD GameApi::FontApi::draw_text_string_sd(std::vector<GI> glyphs, std::string str, int gap_x, int empty_line_height)
{
  int s = glyphs.size();
  std::vector<GlyphInterface*> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_glyph_interface(e, glyphs[i]));
    }
  return add_string_display(e, new StringDisplayFromGlyphs(vec, str, gap_x, empty_line_height));
}

extern std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
int bitmap_find_data(std::string data);

class BitmapPrepareCache2 : public Bitmap<Color>
{
public:
  BitmapPrepareCache2(GameApi::Env &e, std::string id, Bitmap<Color> *bm) : e(e), id(id), bm(bm) { }
  void Prepare()
  {
    if (bitmap_find_data(id)!=-1) {
      return;
    }
    bm->Prepare();
    GameApi::BM num = add_color_bitmap2(e, bm);
    bitmap_prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  Bitmap<Color> *get_bm() const
  {
    int num = bitmap_find_data(id);
    if (num==-1) { const_cast<BitmapPrepareCache2*>(this)->Prepare(); num=bitmap_find_data(id); }
    GameApi::BM bm2;
    bm2.id = num;
    BitmapHandle *handle = find_bitmap(e, bm2);
    Bitmap<Color> *bbm = find_color_bitmap(handle);
    return bbm;
  }
  virtual int SizeX() const { return get_bm()->SizeX(); }
  virtual int SizeY() const { return get_bm()->SizeY(); }
  virtual Color Map(int x, int y) const
  {
    return get_bm()->Map(x,y);
  }
private:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
};


GameApi::BM GameApi::FontApi::draw_text_string(FI font, std::string str, int x_gap, int empty_line_height)
{
  std::cout << "draw_text_string: " << str << std::endl;
  int s = str.size();
  std::vector<GI> glyphs;
  for(int i=0;i<s;i++)
    {
      GI glyph = choose_glyph_from_font(font, (long)(str[i]));
      glyphs.push_back(glyph);
    }
  SD sd = draw_text_string_sd(glyphs, str, x_gap, empty_line_height);
  BM bm = string_display_to_bitmap(sd,0);
  //BitmapHandle *bbm = find_bitmap(e, bm);
  //Bitmap<Color> *bbm1 = find_color_bitmap(bbm);
  //Bitmap<Color> *bbm2 = new BitmapPrepareCache2(e, str, bbm1);
  //BitmapColorHandle *handle = new BitmapColorHandle;
  //handle->bm = bbm2;
  //BM bm2 = add_bitmap(e,handle);

  return bm;
}


extern std::string gameapi_homepageurl;
GameApi::FI GameApi::FontApi::load_font(std::string ttf_filename, int sx, int sy)
{
#ifdef HAS_FREETYPE
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  void *priv_ = (void*)&env->lib;
  return add_font_interface(e, new FontInterfaceImpl(e, priv_, ttf_filename, gameapi_homepageurl, sx,sy));
#else
#endif
}

class RotateCmds : public CmdExecute
{
public:
  RotateCmds(CmdExecute *cmds, float v_x, float v_y, float v_z, float angle, float delta_angle) : cmds(cmds),v_x(v_x), v_y(v_y), v_z(v_z), old_angle(0.0), angle(angle), delta_angle(delta_angle) { do_changes(angle); }
  Point pos() const { return cmds->pos(); }
  int obj_type() const { return cmds->obj_type(); }
  void set_hooks(std::function<void (char)> f) { cmds->set_hooks(f); }
  void Execute(char c) {
    switch(c) {
    case 'r': 
      angle+=delta_angle; 
      setup_angle(); 
      break;
    case 't': 
      angle-=delta_angle; 
      setup_angle();
      break;
    default:
      cmds->Execute(c);
      break;
    }
  }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z) { return cmds->set_units(v_x,v_y,v_z); }
  virtual Vector get_unit_x() const { return cmds->get_unit_x(); }
  virtual Vector get_unit_y() const { return cmds->get_unit_y(); }
  virtual Vector get_unit_z() const { return cmds->get_unit_z(); }

  void setup_angle()
  {
    undo_changes();
    do_changes(angle);
  }
  void undo_changes()
  {
    Vector a_x = cmds->get_unit_x();
    Vector a_y = cmds->get_unit_y();
    Vector a_z = cmds->get_unit_z();
    Matrix m = Matrix::RotateAroundAxis(Vector(v_x,v_y,v_z), -old_angle);
    cmds->set_units(a_x*m,a_y*m,a_z*m);
    old_angle = 0.0f;
  }
  void do_changes(float newangle)
  {
    Vector a_x = cmds->get_unit_x();
    Vector a_y = cmds->get_unit_y();
    Vector a_z = cmds->get_unit_z();
    Matrix m = Matrix::RotateAroundAxis(Vector(v_x,v_y,v_z), newangle);
    cmds->set_units(a_x*m, a_y*m, a_z*m);
    old_angle = newangle;
  }
private:
  CmdExecute *cmds;
  float v_x, v_y, v_z;
  float old_angle;
  float angle;
  float delta_angle;
};
class RepeatCmds : public CmdExecute
{
public:
  RepeatCmds(CmdExecute *cmds, std::string repeat, float dx, float dy, float dz) : cmds(cmds),repeat(repeat),dx(Vector(dx,0.0,0.0)),dy(Vector(0.0,dy,0.0)),dz(Vector(0.0,0.0,dz)) 
  {
    type_delta=0;
  }
  Point pos() const { return cmds->pos() + Vector(pos_delta.x,pos_delta.y,pos_delta.z); }
  int obj_type() const { return type_delta+cmds->obj_type(); }
  void set_hooks(std::function<void (char)> f) { cmds->set_hooks(f); }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z) { cmds->set_units(v_x, v_y, v_z); }
  virtual Vector get_unit_x() const { return dx; }
  virtual Vector get_unit_y() const { return dy; }
  virtual Vector get_unit_z() const { return dz; }

  void Execute(char c) {
    switch(c) {
    case 'w': pos_delta-=dy; break;
    case 'a': pos_delta-=dx; break;
    case 's': pos_delta+=dy; break;
    case 'd': pos_delta+=dx; break;
    case 'n': pos_delta-=dz; break;
    case 'm': pos_delta+=dz; break;
    case 'u': type_delta++; break;
    case 'i': type_delta--; break;
    case '.':
      {
	int s = repeat.size();
	for(int i=0;i<s;i++) {
	  char c = repeat[i];
	  cmds->Execute(c);
	}
	break;
      }
    default:
      cmds->Execute(c);
      break;
    };
  }  
private:
  CmdExecute *cmds;
  std::string repeat;
  Point pos_delta;
  int type_delta;
  Vector dx,dy,dz;
};
class DefaultCmds : public CmdExecute
{
public:
  DefaultCmds(float dx, float dy, float dz) : ddx(dx),ddy(dy),ddz(dz), p_x(0), p_y(0), p_z(0),type(0),dx(Vector(dx,0.0,0.0)),dy(Vector(0.0,dy,0.0)),dz(Vector(0.0,0.0,dz)),point(0.0,0.0,0.0) { }
  Point pos() const { return point; }
  int obj_type() const { return type; }
  void set_hooks(std::function<void (char)> f) { m_f = f; }
  virtual void set_units(Vector v_x, Vector v_y, Vector v_z)
  {
    dx = v_x;
    dy = v_y;
    dz = v_z;
  }
  virtual Vector get_unit_x() const
  {
    return dz;
  }
  virtual Vector get_unit_y() const
  {
    return dy;
  }
  virtual Vector get_unit_z() const
  {
    return dz;
  }
  void Execute(char c) {
    switch(c) {
    case 'w': point-=dy; break;
    case 'a': point-=dx; break;
    case 's': point+=dy; break;
    case 'd': point+=dx; break;
    case 'n': point-=dz; break;
    case 'm': point+=dz; break;
    case 'u': type++; break;
    case 'i': type--; break;
    case '#': point=Point(0.0,0.0,0.0); 
      dx=Vector(ddx,0.0,0.0);
      dy=Vector(0.0,ddy,0.0);
      dz=Vector(0.0,0.0,ddz);
      break;

    case '.': m_f('.'); break;
    case ':': m_f(':'); break;
    };
  }

private:
  float ddx,ddy,ddz;
  int p_x, p_y, p_z;
  int type;
  Vector dx,dy,dz;
  Point point;
  std::function<void (char)> m_f;
};

GameApi::CMD GameApi::MovementNode::default_cmds(float dx, float dy, float dz)
{
  return add_cmds(e, new DefaultCmds(dx,dy,dz));
}
GameApi::CMD GameApi::MovementNode::cmd_repeat(CMD cmds2, std::string repeat, float dx, float dy, float dz)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_cmds(e, new RepeatCmds(cmds, repeat,dx,dy,dz));
}
GameApi::CMD GameApi::MovementNode::cmd_rotate(CMD cmds2, float v_x, float v_y, float v_z, float angle, float delta_angle)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_cmds(e, new RotateCmds(cmds,v_x,v_y,v_z, angle, delta_angle));
}

class CmdPoints : public PointsApiPoints
{
public:
  CmdPoints(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdPoints::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      cmds->Execute(commands[i]);
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      vec.push_back(p);
    }
  }
  virtual int NumPoints() const { return vec.size(); }
  virtual Point Pos(int i) const { return vec[i]; }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  CmdExecute *cmds;
  std::vector<Point> vec;
};

GameApi::PTS GameApi::MovementNode::cmd_to_pts(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_points_api_points(e, new CmdPoints(cmds, commands));
}

class CmdLines : public LineCollection
{
public:
  CmdLines(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdLines::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      cmds->Execute(commands[i]);
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      vec.push_back(old_point);
      vec.push_back(p);
      old_point = p;
    }
    if (c==':') {
      old_point = cmds->pos();
    }
  }
  virtual int NumLines() const { return vec.size()/2; }
  virtual Point LinePoint(int line, int point) const
  {
    line*=2;
    if (point==1) line++;
    return vec[line];
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  CmdExecute *cmds;
  std::vector<Point> vec;
  Point old_point;
};

class CmdPolygon : public FacesInPlane
{
public:
  CmdPolygon(CmdExecute *cmds, std::string commands) : cmds(cmds) {
    cmds->set_hooks(std::bind(&CmdPolygon::insert, this, GameApi::_1));
    int s = commands.size();
    for(int i=0;i<s;i++)
      {
	cmds->Execute(commands[i]);
      }
  }
  void insert(char c) {
    if (c=='.') {
      Point p = cmds->pos();
      Point2d pp = { p.x, p.y };
      vec.push_back(pp);
    }
  }
  virtual int Size() const { return 1; }
  virtual int NumPoints(int face) const { return vec.size(); }
  virtual Point2d Map(int face, int point) const
  {
    return vec[point];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
private:
  CmdExecute *cmds;
  std::vector<Point2d> vec;
};


GameApi::LI GameApi::MovementNode::cmd_to_li(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_line_array(e, new CmdLines(cmds, commands));
}
GameApi::PLF GameApi::MovementNode::cmd_to_plf(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_plane_faces(e, new CmdPolygon(cmds, commands));
}


void check_world(MainLoopEnv &e)
{
  if (e.current_world) return;
  e.current_world = new World;
}
void check_world_elements(MainLoopEnv &e)
{
  World *w = e.current_world;
  if (!w) return;
  if (w->world) return;
  w->world = new WorldElement[w->world_sx*w->world_sy*w->world_sz];
}

class Player : public MainLoopItem
{
public:
  Player(MainLoopItem *next) : next(next) { }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->player_matrix = e.in_MV;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
};


class Enemy : public MainLoopItem
{
public:
  Enemy(MainLoopItem *next) : next(next) { }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->enemy_matrix = e.in_MV;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
};

GameApi::ML GameApi::MovementNode::player(ML prev)
{
  MainLoopItem *item = find_main_loop(e, prev);
  return add_main_loop(e, new Player(item));
}
GameApi::ML GameApi::MovementNode::enemy(ML prev)
{
  MainLoopItem *item = find_main_loop(e, prev);
  return add_main_loop(e, new Enemy(item));
}

class PlayerPos : public MainLoopItem
{
public:
  PlayerPos(MainLoopItem *next, Point p) : next(next),p(p) { }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    e.current_world->player_pos = p;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  Point p;
};

GameApi::ML GameApi::MovementNode::player_pos(ML prev, PT pos)
{
  MainLoopItem *item = find_main_loop(e, prev);
  Point *pt = find_point(e, pos);
  return add_main_loop(e, new PlayerPos(item, *pt));
}
EnemyPiece &get_enemy_piece(MainLoopEnv &e, int index)
{
  check_world(e);
  World *w = e.current_world;
  while (index >= (int)w->enemy_pieces.size()) {
    EnemyPiece piece;
    w->enemy_pieces.push_back(piece);
  }
  EnemyPiece &piece = w->enemy_pieces[index];
  return piece;
}

class EnemyPos : public MainLoopItem
{
public:
  EnemyPos(MainLoopItem *next, PointsApiPoints *pts) : next(next), pts(pts) { firsttime = true; }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    if (firsttime)
      {
	int s = pts->NumPoints();
	for(int i=0;i<s;i++)
	  {
	    Point pt = pts->Pos(i);
	    EnemyPiece &p = get_enemy_piece(e, i);
	    p.enemy_position = pt;
	  }
	firsttime = false;
      }
    
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  PointsApiPoints *pts;
  bool firsttime;
};

GameApi::ML GameApi::MovementNode::enemy_pos(ML prev, PTS pos)
{
  MainLoopItem *item = find_main_loop(e, prev);
  PointsApiPoints *pts = find_pointsapi_points(e, pos);
  return add_main_loop(e, new EnemyPos(item, pts));
}

class BlitVoxel_voxel : public Voxel<int>
{
public:
  BlitVoxel_voxel(Voxel<int> *bg, Voxel<int> *obj, int p_x, int p_y, int p_z) : bg(bg), obj(obj),p_x(p_x), p_y(p_y), p_z(p_z) { }
  void Prepare() { bg->Prepare(); obj->Prepare(); }
  virtual int SizeX() const { return bg->SizeX(); }
  virtual int SizeY() const { return bg->SizeY(); }
  virtual int SizeZ() const { return bg->SizeZ(); }
  virtual int Map(int x, int y, int z) const
  {
    int val = obj->Map(x-p_x,y-p_y,z-p_z);
    if (val==-1) return bg->Map(x,y,z);
    return val;
  }
private:
  Voxel<int> *bg;
  Voxel<int> *obj;
  int p_x, p_y, p_z;
};
class BlitVoxel : public Voxel<int>
{
public:
  BlitVoxel(VolumeObject *obj,
	    int sx, int sy, int sz,
	    float start_x, float end_x,
	    float start_y, float end_y,
	    float start_z, float end_z,
	    int false_value, int true_value) : obj(obj),
					       sx(sx), sy(sy), sz(sz),
					       start_x(start_x), end_x(end_x),
					       start_y(start_y), end_y(end_y),
					       start_z(start_z), end_z(end_z),
					       false_value(false_value), true_value(true_value) { 
    int ss = sx*sy*sz;
    while (sx>600 || sy>600 ||sz>600 || ss>600*600*600) { sx/=2; sy/=2; sz/=2; ss=sx*sy*sz; std::cout << "Warning: Voxel size too large (" << sx << "x" << sy << "x" << sz << ")" << std::endl; }
}
  void Prepare() {}
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    float xx = float(x)/sx*(end_x-start_x)+start_x;
    float yy = float(y)/sy*(end_y-start_y)+start_y;
    float zz = float(z)/sz*(end_z-start_z)+start_z;
    Point p(xx,yy,zz);
    bool b = obj->Inside(p);
    if (b) {
      return true_value;
    }
    return false_value;
  }

private:
  Voxel<int> *next;
  VolumeObject *obj;
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int false_value, true_value;
};
class EmptyVoxel : public Voxel<int>
{
public:
  EmptyVoxel(int sx, int sy, int sz) : sx(sx), sy(sy), sz(sz) { }
  void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    return -1;
  }
private:
  int sx,sy,sz;
};
GameApi::VX GameApi::VoxelApi::empty_voxel(int sx, int sy, int sz)
{
  return add_int_voxel(e, new EmptyVoxel(sx,sy,sz));
}
class VoxelLandscape : public Voxel<int>
{
public:
  VoxelLandscape(Bitmap<float> &bm, int height, int false_value, int true_value) : bm(bm), height(height), false_value(false_value), true_value(true_value) {}
  void Prepare() { bm.Prepare(); }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return height; }
  virtual int SizeZ() const { return bm.SizeY(); }
  virtual int Map(int x, int y, int z) const
  {
    float val = bm.Map(x,z);
    float val0 = val*height;
    if (val0 < float(y)) { return false_value; }
    float val1 = bm.Map(x+1,z);
    float val2 = bm.Map(x,z+1);
    float val3 = bm.Map(x-1,z);
    float val4 = bm.Map(x,z-1);
    float m = std::min(std::min(val1,val2), std::min(val3,val4));
    float val00 = m*height - 1;
    if (val00 > float(y)) { return false_value; }
    return true_value;
  }
private:
  Bitmap<float> &bm;
  int height;
  int false_value;
  int true_value;
};
class SubVoxel : public Voxel<int>
{
public:
  SubVoxel(Voxel<int> *vx, int start_x, int end_x,
	   int start_y, int end_y,
	   int start_z, int end_z) : vx(vx), start_x(start_x), end_x(end_x),
				     start_y(start_y), end_y(end_y),
				     start_z(start_z), end_z(end_z) { }
  void Prepare() { vx->Prepare(); }

  virtual int SizeX() const { return end_x-start_x; }
  virtual int SizeY() const { return end_y-start_y; }
  virtual int SizeZ() const { return end_z-start_z; }
  virtual int Map(int x, int y, int z) const
  {
    // clip to specified area
    if (x<start_x) return -1;
    if (x>=end_x) return -1;
    if (y<start_y) return -1;
    if (y>=end_y) return -1;
    if (z<start_z) return -1;
    if (x>=end_z) return -1;
    // clip to available space
    int xx = x+start_x;
    int yy = y+start_y;
    int zz = z+start_z;
    if (xx>=vx->SizeX()) return -1;
    if (yy>=vx->SizeY()) return -1;
    if (zz>=vx->SizeZ()) return -1;
    if (xx<0) return -1;
    if (yy<0) return -1;
    if (zz<0) return -1;
    return vx->Map(xx, yy, zz);
  }
private:
  Voxel<int> *vx;
  int start_x, end_x;
  int start_y, end_y;
  int start_z, end_z;
};
GameApi::VX GameApi::VoxelApi::subvoxel(VX voxel, int start_x, int end_x,
					int start_y, int end_y,
					int start_z, int end_z)
{
  Voxel<int> *vx = find_int_voxel(e, voxel);
  return add_int_voxel(e, new SubVoxel(vx, start_x, end_x, start_y, end_y, start_z, end_z));
}
GameApi::VX GameApi::VoxelApi::voxel_landscape_from_fbm(FB bitmap, int height, int false_value, int true_value)
{
  Bitmap<float> *fbm = find_float_bitmap(e, bitmap)->bitmap;
  return add_int_voxel(e, new VoxelLandscape(*fbm, height, false_value, true_value));
}
GameApi::VX GameApi::VoxelApi::blit_voxel2(VX voxel, VX voxel2, int p_x, int p_y, int p_z)
{
  Voxel<int> *v0 = find_int_voxel(e, voxel);
  Voxel<int> *v1 = find_int_voxel(e, voxel2);
  return add_int_voxel(e, new BlitVoxel_voxel(v0,v1, p_x,p_y,p_z));
}
GameApi::VX GameApi::VoxelApi::blit_voxel(O object,
					  int sx, int sy, int sz,
					  float start_x, float end_x,
					  float start_y, float end_y,
					  float start_z, float end_z,
					  int false_value, int true_value)
{
  VolumeObject *obj = find_volume(e, object);
  return add_int_voxel(e,new BlitVoxel(obj, sx,sy,sz,
				       start_x, end_x,
				       start_y, end_y,
				       start_z, end_z,
				       false_value, true_value));
}


class VoxelToPTS 
{
public:
  VoxelToPTS(Voxel<int> *vx, int count, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : vx(vx),count(count), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z)
  {
    prepared=false;
  }
  void Prepare() {
    if (!prepared) {
    vx->Prepare();
    int sx = vx->SizeX();
    int sy = vx->SizeY();
    int sz = vx->SizeZ();
    if (sx && sy && sz)  prepared=true;
    Point p;
    float dx = end_x-start_x;
    float dy = end_y-start_y;
    float dz = end_z-start_z;
    pts.resize(count);
    for(int i=0;i<sx;i++) {
      p.x = float(i)/sx*dx+start_x;
      for(int j=0;j<sy;j++) {
	p.y = float(j)/sy*dy+start_y;
	for(int k=0;k<sz;k++)
	  {
	    int val = vx->Map(i,j,k);
	    if (val>=0 && val<count) {
	      p.z = float(k)/sz*dz+start_z;
	      int val_mx = vx->Map(i-1,j,k);
	      bool b_mx = val_mx>=0 && val_mx<count;
	      if (!b_mx) { pts[val].push_back(p); continue; }
	      int val_px = vx->Map(i+1,j,k);
	      bool b_px = val_px>=0 && val_px<count;
	      if (!b_px) { pts[val].push_back(p); continue; }
	      int val_my = vx->Map(i,j-1,k);
	      bool b_my = val_my>=0 && val_my<count;
	      if (!b_my) { pts[val].push_back(p); continue; }
	      int val_py = vx->Map(i,j+1,k);
	      bool b_py = val_py>=0 && val_py<count;
	      if (!b_py) { pts[val].push_back(p); continue; }
	      int val_mz = vx->Map(i,j,k-1);
	      bool b_mz = val_mz>=0 && val_mz<count;
	      if (!b_mz) { pts[val].push_back(p); continue; }
	      int val_pz = vx->Map(i,j,k+1);
	      bool b_pz = val_pz>=0 && val_pz<count;
	      if (!b_pz) { pts[val].push_back(p); continue; }
	    }
	  }
      }
    }
    }
  }
  PointsApiPoints *get(int val);
public:
  std::vector<std::vector<Point> > pts;
  Voxel<int> *vx;
  bool prepared;
  int count;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};

class VPTS : public PointsApiPoints
{
public:
  VPTS(VoxelToPTS &pts, int val, Voxel<int> *vx) : pts(pts), val(val),vx(vx) { }
  void Prepare() {
    pts.Prepare();
    /*vx->Prepare();*/ }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return pts.pts[val].size();
  }
  virtual Point Pos(int i) const
  {
    return pts.pts[val][i];
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  VoxelToPTS &pts;
  int val;
  Voxel<int> *vx;
};

PointsApiPoints *VoxelToPTS::get(int val)
{
  return new VPTS(*this, val,vx);
}

std::vector<GameApi::PTS> GameApi::arr_to_pts_arr(GameApi::EveryApi &ev, GameApi::ARR a)
{
  std::vector<GameApi::PTS> vec;
  ArrayType *t = find_array(ev.get_env(), a);
  int s = t->vec.size();
  for(int i=0;i<s;i++)
    {
      int val = t->vec[i];
      GameApi::PTS pts;
      pts.id = val;
      vec.push_back(pts);
    }
  return vec;
}

// ARR = array of PTS
GameApi::ARR GameApi::VoxelApi::voxel_instancing(VX voxel,
						 int count,
						 float start_x, float end_x,
						 float start_y, float end_y,
						 float start_z, float end_z)
{
  Voxel<int> *vx = find_int_voxel(e, voxel);
  VoxelToPTS *pts = new VoxelToPTS(vx, count, start_x, end_x, start_y, end_y, start_z, end_z);
  int s = count;
  std::vector<int> vec;
  ArrayType *array = new ArrayType;
  array->type = 2;
  for(int i=0;i<s;i++)
    {
      PointsApiPoints *p = pts->get(i);
      PTS pts = add_points_api_points(e, p);
      array->vec.push_back(pts.id);
    }
  return add_array(e,array);
}
GameApi::ML GameApi::VoxelApi::voxel_render(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<ML> mls;
  for(int i=0;i<s;i++)
    {
      ML o = ev.materials_api.render_instanced_ml(ev, objs[i], vec[i]);
      mls.push_back(o);
    }
  return ev.mainloop_api.array_ml(mls);
}
GameApi::ML GameApi::VoxelApi::voxel_bind(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec, MT mt)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<ML> mls;
  for(int i=0;i<s;i++)
    {
      ML o = ev.materials_api.bind_inst(objs[i], vec[i], mt);
      mls.push_back(o);
    }
  return ev.mainloop_api.array_ml(mls);
}
GameApi::P GameApi::VoxelApi::voxel_static(EveryApi &ev, std::vector<P> objs, std::vector<PTS> vec)
{
  int s = std::min(objs.size(), vec.size());
  std::vector<P> mls;
  for(int i=0;i<s;i++)
    {
      P o = ev.polygon_api.static_instancing(ev, objs[i], vec[i]);
      mls.push_back(o);
    }
  return ev.polygon_api.or_array2(mls);
}

GameApi::ML GameApi::MovementNode::all_cursor_keys(EveryApi &ev, ML ml, float speed, float duration)
{
  ML ml1 = cursor_keys(ev,ml,119,97,115,100, speed, duration);
  ML ml2 = cursor_keys(ev,ml1, 1073741906, 1073741904, 1073741905, 1073741903, speed, duration);
  ML ml3 = cursor_keys(ev,ml2, 82, 80, 81, 79, speed, duration);
  return ml3;
}

GameApi::ML GameApi::MovementNode::cursor_keys(EveryApi &ev, ML I10, int key_up, int key_left, int key_down, int key_right, float speed, float duration)
{
  int k_100 = key_right;
  int k_119 = key_up;
  int k_115 = key_down;
  int k_97 = key_left;

  float s_8 = speed;
  float s_234 = speed*sqrt(2) - speed;
  
  float s_1 = duration;
MN I11=ev.move_api.empty();
MN I12=ev.move_api.translate(I11,0,s_1,s_8,0,0);
ML I13=ev.move_api.key_activate_ml(ev,I10,I12,k_100,s_1);
MN I14=ev.move_api.empty();
MN I15=ev.move_api.translate(I14,0,s_1,0,-s_8,0);
ML I16=ev.move_api.key_activate_ml(ev,I13,I15,k_119,s_1);
MN I17=ev.move_api.empty();
MN I18=ev.move_api.translate(I17,0,s_1,0,s_8,0);
ML I19=ev.move_api.key_activate_ml(ev,I16,I18,k_115,s_1);
MN I20=ev.move_api.empty();
MN I21=ev.move_api.translate(I20,0,s_1,-s_8,0,0);
ML I22=ev.move_api.key_activate_ml(ev,I19,I21,k_97,s_1);
MN I23=ev.move_api.empty();
MN I24=ev.move_api.translate(I23,0,s_1,-s_234,s_234,0);
ML I25=ev.move_api.comb_key_activate_ml(ev,I22,I24,k_100,k_119,s_1);
MN I26=ev.move_api.empty();
MN I27=ev.move_api.translate(I26,0,s_1,-s_234,-s_234,0);
ML I28=ev.move_api.comb_key_activate_ml(ev,I25,I27,k_115,k_100,s_1);
MN I29=ev.move_api.empty();
MN I30=ev.move_api.translate(I29,0,s_1,s_234,-s_234,0);
ML I31=ev.move_api.comb_key_activate_ml(ev,I28,I30,k_115,k_97,s_1);
MN I32=ev.move_api.empty();
MN I33=ev.move_api.translate(I32,0,s_1,s_234,s_234,0);
ML I34=ev.move_api.comb_key_activate_ml(ev,I31,I33,k_119,k_97,s_1);
 return I34;
}

float quake_pos_x, quake_pos_y;
float quake_rot_y;

class QuakeML : public MainLoopItem
{
public:
  QuakeML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float speed, float rot_speed) : env(env), ev(ev), next(next), speed(speed), rot_speed(rot_speed) { }
  virtual void execute(MainLoopEnv &e)
  {
    GameApi::InteractionApi::quake_movement_frame(ev, pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    quake_pos_x = pos_x;
    quake_pos_y = pos_y;
    quake_rot_y = rot_y;
    MainLoopEnv eee = e;
    GameApi::M env_m = add_matrix2(env, e.in_MV);
    GameApi::M rot_y2 = ev.matrix_api.yrot(rot_y);
    GameApi::M trans = ev.matrix_api.trans(pos_x, 0.0, pos_y+400);
    GameApi::M trans2 = ev.matrix_api.trans(0.0,0.0,-400.0);
    GameApi::M scale = ev.matrix_api.scale(1.0,1.0,-1.0);
    GameApi::M res = ev.matrix_api.mult(env_m, ev.matrix_api.mult(ev.matrix_api.mult(ev.matrix_api.mult(trans,rot_y2),trans2),scale));

    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(res));
    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;

    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);

    
    eee.in_MV = find_matrix(env, res); 
    eee.env = find_matrix(env, res);

    next->execute(eee);
    ev.shader_api.unuse(s3);

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    GameApi::MainLoopApi::Event ee;
    ee.type = e.type;
    ee.ch = e.ch;
    ee.button = e.button;
    GameApi::InteractionApi::quake_movement_event(ev,ee,pos_x, pos_y, rot_y, dt, speed_x, speed_y, speed, rot_speed);
    next->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  float pos_x=0.0, pos_y=0.0, rot_y=0.0;
  GameApi::InteractionApi::Quake_data dt;
  float speed_x=0.0, speed_y=0.0;
  float speed, rot_speed;
};

GameApi::ML GameApi::MovementNode::quake_ml(EveryApi &ev, ML ml,float speed, float rot_speed)
{
  MainLoopItem *mml = find_main_loop(e,ml);
  return add_main_loop(e, new QuakeML(e,ev, mml, speed, rot_speed));
}
class LocalMove : public MainLoopItem
{
public:
  LocalMove(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *inner, PointsApiPoints *o) : env(env), ev(ev), inner(inner), o(o) { }
  virtual void execute(MainLoopEnv &e)
  {
    o->Update(e);
    int s = o->NumPoints();
    for(int i=0;i<s;i++)
      {
	Point p = o->Pos(i);
	GameApi::M m = ev.matrix_api.trans(p.x,p.y,p.z);
	GameApi::M m2 = add_matrix2(env, e.in_MV);
	MainLoopEnv ee = e;

	GameApi::SH s1;
	s1.id = e.sh_texture;
	GameApi::SH s11;
	s11.id = e.sh_texture_2d;
	GameApi::SH s2;
	s2.id = e.sh_array_texture;
	GameApi::SH s3;
	s3.id = e.sh_color;
	
	GameApi::M mat2 = ev.matrix_api.mult(m,m2);
	//GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1, "in_MV", mat2);
	//ev.shader_api.set_var(s1, "in_iMV", mat2i);
	ev.shader_api.use(s11);
	ev.shader_api.set_var(s11, "in_MV", mat2);
	//ev.shader_api.set_var(s11, "in_iMV", mat2i);
	ev.shader_api.use(s2);
	ev.shader_api.set_var(s2, "in_MV", mat2);
	//ev.shader_api.set_var(s2, "in_iMV", mat2i);
	ev.shader_api.use(s3);
	ev.shader_api.set_var(s3, "in_MV", mat2);
	//ev.shader_api.set_var(s3, "in_iMV", mat2i);
	
	
	ee.in_MV = find_matrix(env, m) * e.in_MV;
	ee.env = find_matrix(env, m) * e.in_MV;
	inner->execute(ee);
	ev.shader_api.unuse(s3);
      }	  
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    inner->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *inner;
  PointsApiPoints *o;
};
GameApi::ML GameApi::MovementNode::local_move(EveryApi &ev, ML inner_ml, PTS center_points)
{
  MainLoopItem *item = find_main_loop(e, inner_ml);
  PointsApiPoints *o = find_pointsapi_points(e, center_points);
  return add_main_loop(e, new LocalMove(e, ev, item, o));
}


#if 0
class ModifyWorld : public MainLoopItem, public WorldSpec
{
public:
  ModifyWorld(Cursor &c) : c(c) { }
  virtual int NumItems() const { return item.size(); }
  virtual int PosX(int i) const { return pos_x[i]; }
  virtual int PosY(int i) const { return pos_y[i]; }
  virtual int Item(int i) const { return item[i]; }

  virtual void execute(MainLoopEnv &e)
  {
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    c.update();
    static const char *letters = "qwertyuiopasdfghjklzxcvbnm";
    int s = strlen(letters);
    int value = -1;
    for(int i=0;i<s;i++)
      {
	if (e.ch==letters[i]) {
	  value = i;
	}
      }
    if (value != -1 && e.type==0x300)
      {
	int p_x = c.PosX();
	int p_y = c.PosY();
	pos_x.push_back(p_x);
	pos_y.push_back(p_y);
	item.push_back(value);
      }
  }
private:
  std::vector<int> pos_x;
  std::vector<int> pos_y;
  std::vector<int> item;
};
#endif

class ExplosionChange : public DynamicChange
{
public:
  ExplosionChange(Point center, float start_val, float end_val, float start_time, float end_time) : center(center), start_val(start_val), end_val(end_val), start_time(start_time), end_time(end_time) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    float time = e.time*10.0;
    float d = start_val;
    if (time < start_time) { d=start_val; }
    else if (time>end_time) { d=end_val; }
    else
      {
	d = start_val + (end_val-start_val)*(time-start_time)/(end_time-start_time);
      }
    int s = size/3;
    for(int i=0;i<s;i++)
      {
	float x = source[i*3];
	float y = source[i*3+1];
	float z = source[i*3+2];
	Point p(x,y,z);
	Vector v=p-center;
	v*=d;
	p+=v;
	target[i*3] = p.x;
	target[i*3+1] = p.y;
	target[i*3+2] = p.z;
      }
    
  }
private:
  Point center;
  float start_val, end_val;
  float start_time, end_time;
};

class SphWaveChange : public DynamicChange
{
public:
  SphWaveChange(float r1, float fr_1, float r2, float fr_2, float t_1, float t_2) : r1(r1), r2(r2), fr_1(fr_1), fr_2(fr_2), t_1(t_1), t_2(t_2) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    int s = size/3;
    SphericalPoint sp(Point(0.0,0.0,0.0));
    for(int i=0;i<s;i++)
      {
	Point p(source[0],source[1],source[2]);
	sp.FromPoint(p);
	sp.r = sp.r + r1*cos(fr_1*sp.alfa+t_1*e.time) + r2*cos(fr_2*sp.beta+t_2*e.time);
	Point p2 = sp.ToPoint();
	target[0] = p2.x;
	target[1] = p2.y;
	target[2] = p2.z;
	source+=3;
	target+=3;
      }
  }
private:
  float r1,r2;
  float fr_1, fr_2;
  float t_1, t_2;
};

class WaveChange : public DynamicChange
{
public:
  WaveChange(float r, float speed1,float speed2, float dist1, float dist2, int sx, int sy) : r(r),speed1(speed1), speed2(speed2), dist1(dist1), dist2(dist2), sx(sx), sy(sy) { }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    //std::cout << "applychange: " << size << std::endl;
    int s = size/3;
    float s1 = speed1*e.time;
    float s2 = speed2*e.time;
    float delta1 = dist1/sx;
    float delta2 = dist2/sy;
    float x,y,z;
    for(int i=0;i<s;i++)
      {
	if (i%sx==0) { s2+=delta2; s1=speed1*e.time; }
	x = source[i*3];
	y = source[i*3+1];
	z = source[i*3+2];
	x+=r*sin(s1)*cos(s2);
	y+=r*sin(s1)*sin(s2);
	z+=r*cos(s1);
	target[i*3] = x;
	target[i*3+1] = y;
	target[i*3+2] = z;
	s1+=delta1;
      }
  }
private:
  float r;
  float speed1,speed2;
  float dist1, dist2;
  int sx,sy;
};
class IdentityChange : public DynamicChange
{
public:
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    for(int i=0;i<size;i++)
      {
	*target++ = *source++;
      }     
  }
};

GameApi::DC GameApi::MovementNode::identity()
{
  return add_dyn_change(e, new IdentityChange);
}
GameApi::DC GameApi::MovementNode::explosion(float center_x, float center_y, float center_z, float start_val, float end_val, float start_time, float end_time)
{
  return add_dyn_change(e, new ExplosionChange(Point(center_x, center_y, center_z), start_val, end_val, start_time, end_time));
}
GameApi::DC GameApi::MovementNode::wave(float r, float speed1, float speed2, float dist1, float dist2, int sx, int sy)
{
  return add_dyn_change(e, new WaveChange(r,speed1, speed2, dist1, dist2, sx,sy));
}
GameApi::DC GameApi::MovementNode::sphwave(float r1, float fr_1, float t_1, float r2, float fr_2, float t_2)
{
  return add_dyn_change(e, new SphWaveChange(r1,fr_1,r2,fr_2,t_1,t_2));
}
class SplitChange : public DynamicChange
{
public:
  SplitChange(DynamicChange *d1, DynamicChange *d2, float val) : d1(d1),d2(d2), val(val) {
    if (val<0.0) val=0.0;
    if (val>1.0) val=1.0;
  }
  void applychange(float *source, float *target, int size, MainLoopEnv &e)
  {
    int s = size/3;
    float v = val*s;
    int ss = (int)v;
    ss*=3;
    d1->applychange(source,target,ss,e);
    d2->applychange(source+ss,target+ss,size-ss,e);
  }
private:
  DynamicChange *d1;
  DynamicChange *d2;
  float val;
};
GameApi::DC GameApi::MovementNode::split(DC d1, DC d2, float val)
{
  DynamicChange *dd1 = find_dyn_change(e, d1);
  DynamicChange *dd2 = find_dyn_change(e, d2);
  return add_dyn_change(e, new SplitChange(dd1,dd2,val));
}

class WavePoints : public PointsApiPoints
{
public:
  WavePoints(Waveform &wv, int num_samples, Point pos, Vector u_x, Vector u_y) : wv(wv), num_samples(num_samples),pos(pos), u_x(u_x), u_y(u_y) { }
  virtual void HandleEvent(MainLoopEvent &event) { wv.HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return wv.Update(e); }
  virtual int NumPoints() const
  {
    return num_samples;
  }
  virtual Point Pos(int i) const
  {
    float val = float(i)/num_samples*wv.Length();
    float v = wv.Index(val);
    return pos + val*u_x + v*u_y;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Waveform &wv;
  int num_samples;
  Point pos;
  Vector u_x, u_y;
};
GameApi::PTS GameApi::PointsApi::wave_points(WV wave, int num_samples,
					     float pos_x, float pos_y, float pos_z,
					     float u_x_x, float u_x_y, float u_x_z,
					     float u_y_x, float u_y_y, float u_y_z)
{
  Waveform *wv = find_waveform(e, wave);
  return add_points_api_points(e, new WavePoints(*wv, num_samples, Point(pos_x, pos_y, pos_z), Vector(u_x_x, u_x_y, u_x_z), Vector(u_y_x, u_y_y, u_y_z)));
}

class FilterComponent : public PointsApiPoints
{
public:
  FilterComponent(PointsApiPoints *pts, int comp, float val) : pts(pts), comp(comp), val(val) { }
  void HandleEveent(MainLoopEvent &event) { pts->HandleEvent(event); }
  bool Update(MainLoopEnv &e) { return pts->Update(e); }
  int NumPoints() const { return pts->NumPoints(); }
  Point Pos(int i) const
  {
    Point p = pts->Pos(i);
    if (comp==0) { p.x=val; }
    if (comp==1) { p.y=val; }
    if (comp==2) { p.z=val; }
    return p;
  }
  unsigned int Color(int i) const {return pts->Color(i); }
private:
  PointsApiPoints *pts;
  int comp;
  float val;
};

GameApi::PTS GameApi::PointsApi::filter_component(PTS pts, int comp, float val)
{
  PointsApiPoints *pts2 = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new FilterComponent(pts2, comp, val));
}

class AnimRotPTS : public PointsApiPoints
{
public:
  AnimRotPTS(PointsApiPoints *next, float start_time, float end_time, Vector v, float rotate_amount) : next(next), start_time(start_time), end_time(end_time), v(v), rotate_amount(rotate_amount) {
    m = Matrix::Identity();
    firsttime = true;
  }
  void HandleEvent(MainLoopEvent &event) { next->HandleEvent(event); }
  bool Update(MainLoopEnv &e) {
    if (firsttime) { firsttime = false; return true; }
    float time = e.time*10.0;
    if (time < start_time) { m = Matrix::Identity(); return false; }
    if (time > end_time) { m = Matrix::RotateAroundAxis(v,rotate_amount); return false; }
    
    float val = (time-start_time)/(end_time-start_time);
    val*=rotate_amount;
    m = Matrix::RotateAroundAxis(v, val);
    
    return true;
  }
  int NumPoints() const { return next->NumPoints(); }
  Point Pos(int i) const
  {
    return next->Pos(i)*m;
  }
  unsigned int Color(int i) const { return next->Color(i); }
private:
  PointsApiPoints *next;
  float start_time, end_time;
  Vector v;
  float rotate_amount;
  Matrix m;
  bool firsttime;
};

GameApi::PTS GameApi::PointsApi::anim_rot_pts(PTS pts, float start_time, float end_time, float v_x, float v_y, float v_z, float rotate_amount)
{
  PointsApiPoints *next = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new AnimRotPTS(next, start_time, end_time, Vector(v_x,v_y,v_z), rotate_amount));
}

class Scale2dScreen : public MainLoopItem
{
public:
  Scale2dScreen(GameApi::EveryApi &ev, MainLoopItem *item, float sx, float sy) : ev(ev), item(item), sx(sx), sy(sy) {}
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    float source_sx = sx;
    float source_sy = sy;
    float target_sx = ev.mainloop_api.get_screen_sx();
    float target_sy = ev.mainloop_api.get_screen_sy();
    //float target_sx = 200;
    //float target_sy = 200;
    Matrix m1 = Matrix::Scale(target_sx/source_sx,target_sy/source_sy,1.0);
    ee.in_MV = e.in_MV * m1;
    ee.env = e.env * m1;
    item->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  float sx,sy;
};

GameApi::ML GameApi::MainLoopApi::scale_2d_screen(GameApi::EveryApi &ev, ML orig, float sx, float sy)
{
  MainLoopItem *item = find_main_loop(e, orig);
  return add_main_loop(e, new Scale2dScreen(ev, item,sx,sy));
}

GameApi::PN GameApi::WaveformApi::std_polynomial(float x_5, float x_4, float x_3, float x_2, float x_1, float c)
{
  std::vector<float> *vec = new std::vector<float>;
  vec->push_back(c);
  vec->push_back(x_1);
  vec->push_back(x_2);
  vec->push_back(x_3);
  vec->push_back(x_4);
  vec->push_back(x_5);
  return add_polynomial(e, vec);
}
class PolynomialWave : public Waveform
{
public:
  PolynomialWave(std::vector<float> *vec, float start_x, float end_x, float start_y, float end_y) : vec(vec), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
  void HandleEvent(MainLoopEvent &event) { }
  bool Update(MainLoopEnv &e) { return false; }
  float Length() const {
    return end_x-start_x;
  }
  float Min() const { return start_y; }
  float Max() const { return end_y; }
  float Index(float val) const
  {
    val+=start_x;
    std::vector<float> basis = Polynomial::basis_vector(vec->size(), val);
    float v = Polynomial::mul(*vec, basis);
    return v;
  }
private:
  std::vector<float> *vec;
  float start_x, end_x;
  float start_y, end_y;
};

GameApi::WV GameApi::WaveformApi::polynomial_wave(PN pn, float start_x, float end_x, float start_y, float end_y)
{
  std::vector<float> *poly = find_polynomial(e, pn);
  return add_waveform(e, new PolynomialWave(poly, start_x, end_x, start_y, end_y));
}

GameApi::PN GameApi::WaveformApi::df_dx(PN poly)
{
  std::vector<float> *vec = find_polynomial(e, poly);
  int sx = vec->size();
  int sy = vec->size();
  std::vector<float> vec2 = Polynomial::mul_matrix(PolyMatrix::df_per_dx(sx,sy), *vec);
  std::vector<float> *vec3 = new std::vector<float>(vec2);
  return add_polynomial(e, vec3);
}


class TouchRotate : public MainLoopItem
{
public:
  TouchRotate(GameApi::Env &e2, GameApi::EveryApi &ev, MainLoopItem *next, bool leftright, bool topdown, float x_speed, float y_speed) : e2(e2), ev(ev), next(next), leftright(leftright), topdown(topdown), x_speed(x_speed), y_speed(y_speed) { mousedown=false; fixed=Point(0.0,0.0,0.0); 
    if (!leftright) x_speed=0.0;
    if (!topdown) y_speed=0.0;
    accumulated=Matrix::Identity();
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e) 
  {
    if ((e.type==1025 && e.button==0) || e.type==Low_SDL_FINGERDOWN) {
      mousedown=true;
      mousedown_pos = e.cursor_pos;
    }
    if ((e.type==1026 && e.button==-1) || e.type==Low_SDL_FINGERUP) {
      mousedown=false;
      fixed+=mouse_delta;
      mouse_delta=Vector(0.0,0.0,0.0);
    }
    if ((e.type==1024||e.type==Low_SDL_FINGERMOTION) && mousedown)
      {
	mouse_delta = e.cursor_pos - mousedown_pos;
        mousedown_pos = e.cursor_pos;
      }
    next->handle_event(e);
  }
  void execute(MainLoopEnv &e)
  {
    Matrix curr1 = Matrix::YRotation(mouse_delta.dx*x_speed);
    Matrix curr2 = Matrix::XRotation(mouse_delta.dy*y_speed);
    Matrix curr = curr1 * curr2;
    
    accumulated *= curr;
    mouse_delta=Vector(0.0,0.0,0.0);

    MainLoopEnv ee = e;
    ee.in_MV = ee.in_MV * accumulated; //Matrix::XRotation((fixed.y+mouse_delta.dy)*y_speed) * Matrix::YRotation((fixed.x+mouse_delta.dx)*x_speed);
    ee.env = ee.env * accumulated; //Matrix::XRotation((fixed.y+mouse_delta.dy)*y_speed) * Matrix::YRotation((fixed.x+mouse_delta.dx)*x_speed);

    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s11;
    s11.id = e.sh_texture_2d;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;


    GameApi::M mat2 = add_matrix2(e2,ee.env);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);


    next->execute(ee);
    ev.shader_api.unuse(s3);
  }
private:
  GameApi::Env &e2;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  bool leftright;
  bool topdown;
  
  bool mousedown;
  Point mousedown_pos;
  Vector mouse_delta;
  Point fixed;
  float x_speed, y_speed;

  Matrix accumulated;
  
};

GameApi::ML GameApi::MainLoopApi::touch_rotate(GameApi::EveryApi &ev, ML ml, bool leftright, bool topdown, float x_speed, float y_speed)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new TouchRotate(e,ev,item, leftright, topdown,x_speed,y_speed));
}

GameApi::ML GameApi::MainLoopApi::score_display(EveryApi &ev, ML ml, std::string font)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);

  IF I21=ev.font_api.score_fetcher(ev);
  SF I22=ev.font_api.int_to_string_fetcher(I21);

  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);

  IF I25=ev.font_api.char_fetcher_from_string(I22,"0123456789",1);
  ML I26=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I25,0,0);

  IF I27=ev.font_api.char_fetcher_from_string(I22,"0123456789",2);
  ML I28=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I27,0,0);

  IF I29=ev.font_api.char_fetcher_from_string(I22,"0123456789",3);
  ML I30=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I29,0,0);

  IF I31=ev.font_api.char_fetcher_from_string(I22,"0123456789",4);
  ML I32=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I31,0,0);

  MN I49a=ev.move_api.empty();
  MN I50a=ev.move_api.trans2(I49a,35,0,0);
  ML I51a=ev.move_api.move_ml(ev,I26,I50a,1,10.0);

  MN I49b=ev.move_api.empty();
  MN I50b=ev.move_api.trans2(I49b,35+35,0,0);
  ML I51b=ev.move_api.move_ml(ev,I28,I50b,1,10.0);

  MN I49c=ev.move_api.empty();
  MN I50c=ev.move_api.trans2(I49c,35+35+35,0,0);
  ML I51c=ev.move_api.move_ml(ev,I30,I50c,1,10.0);

  MN I49d=ev.move_api.empty();
  MN I50d=ev.move_api.trans2(I49d,35+35+35+35,0,0);
  ML I51d=ev.move_api.move_ml(ev,I32,I50d,1,10.0);


  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51a,I51b,I51c,I51d});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;

}

GameApi::ML GameApi::MainLoopApi::time_display(EveryApi &ev, ML ml, std::string font, float start_time)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);

  IF I21=ev.font_api.time_fetcher(ev, start_time);
  SF I22=ev.font_api.int_to_string_fetcher(I21);

  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);

  IF I25=ev.font_api.char_fetcher_from_string(I22,"0123456789",1);
  ML I26=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I25,0,0);

  IF I27=ev.font_api.char_fetcher_from_string(I22,"0123456789",2);
  ML I28=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I27,0,0);

  IF I29=ev.font_api.char_fetcher_from_string(I22,"0123456789",3);
  ML I30=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I29,0,0);

  IF I31=ev.font_api.char_fetcher_from_string(I22,"0123456789",4);
  ML I32=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I31,0,0);

  MN I49a=ev.move_api.empty();
  MN I50a=ev.move_api.trans2(I49a,35,0,0);
  ML I51a=ev.move_api.move_ml(ev,I26,I50a,1,10.0);

  MN I49b=ev.move_api.empty();
  MN I50b=ev.move_api.trans2(I49b,35+35,0,0);
  ML I51b=ev.move_api.move_ml(ev,I28,I50b,1,10.0);

  MN I49c=ev.move_api.empty();
  MN I50c=ev.move_api.trans2(I49c,35+35+35,0,0);
  ML I51c=ev.move_api.move_ml(ev,I30,I50c,1,10.0);

  MN I49d=ev.move_api.empty();
  MN I50d=ev.move_api.trans2(I49d,35+35+35+35,0,0);
  ML I51d=ev.move_api.move_ml(ev,I32,I50d,1,10.0);


  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51a,I51b,I51c,I51d});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;

}


GameApi::ML GameApi::MainLoopApi::fps_display(EveryApi &ev, ML ml, std::string font)
{
  FI I1=ev.font_api.load_font(font,30,30);
  BM I2=ev.font_api.draw_text_string(I1,"0",5,30);
  FI I3=ev.font_api.load_font(font,30,30);
  BM I4=ev.font_api.draw_text_string(I3,"1",5,30);
  FI I5=ev.font_api.load_font(font,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"2",5,30);
  FI I7=ev.font_api.load_font(font,30,30);
  BM I8=ev.font_api.draw_text_string(I7,"3",5,30);
  FI I9=ev.font_api.load_font(font,30,30);
  BM I10=ev.font_api.draw_text_string(I9,"4",5,30);
  FI I11=ev.font_api.load_font(font,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"5",5,30);
  FI I13=ev.font_api.load_font(font,30,30);
  BM I14=ev.font_api.draw_text_string(I13,"6",5,30);
  FI I15=ev.font_api.load_font(font,30,30);
  BM I16=ev.font_api.draw_text_string(I15,"7",5,30);
  FI I17=ev.font_api.load_font(font,30,30);
  BM I18=ev.font_api.draw_text_string(I17,"8",5,30);
  FI I19=ev.font_api.load_font(font,30,30);
  BM I20=ev.font_api.draw_text_string(I19,"9",5,30);
  FF I21=ev.font_api.fps_fetcher(ev);
  SF I22=ev.font_api.float_to_string_fetcher(I21);
  IF I23=ev.font_api.char_fetcher_from_string(I22,"0123456789",0);
  ML I24=ev.font_api.dynamic_character(ev,std::vector<BM>{I2,I4,I6,I8,I10,I12,I14,I16,I18,I20},I23,0,0);
  FI I25=ev.font_api.load_font(font,30,30);
  BM I26=ev.font_api.draw_text_string(I25,"0",5,30);
  FI I27=ev.font_api.load_font(font,30,30);
  BM I28=ev.font_api.draw_text_string(I27,"1",5,30);
  FI I29=ev.font_api.load_font(font,30,30);
  BM I30=ev.font_api.draw_text_string(I29,"2",5,30);
  FI I31=ev.font_api.load_font(font,30,30);
  BM I32=ev.font_api.draw_text_string(I31,"3",5,30);
  FI I33=ev.font_api.load_font(font,30,30);
  BM I34=ev.font_api.draw_text_string(I33,"4",5,30);
  FI I35=ev.font_api.load_font(font,30,30);
  BM I36=ev.font_api.draw_text_string(I35,"5",5,30);
  FI I37=ev.font_api.load_font(font,30,30);
  BM I38=ev.font_api.draw_text_string(I37,"6",5,30);
  FI I39=ev.font_api.load_font(font,30,30);
  BM I40=ev.font_api.draw_text_string(I39,"7",5,30);
  FI I41=ev.font_api.load_font(font,30,30);
  BM I42=ev.font_api.draw_text_string(I41,"8",5,30);
  FI I43=ev.font_api.load_font(font,30,30);
  BM I44=ev.font_api.draw_text_string(I43,"9",5,30);
  FF I45=ev.font_api.fps_fetcher(ev);
  SF I46=ev.font_api.float_to_string_fetcher(I45);
  IF I47=ev.font_api.char_fetcher_from_string(I46,"0123456789",1);
  ML I48=ev.font_api.dynamic_character(ev,std::vector<BM>{I26,I28,I30,I32,I34,I36,I38,I40,I42,I44},I47,0,0);
  MN I49=ev.move_api.empty();
  MN I50=ev.move_api.trans2(I49,35,0,0);
  ML I51=ev.move_api.move_ml(ev,I48,I50,1,10.0);
  ML I52=ev.mainloop_api.array_ml(std::vector<ML>{I24,I51});
  ML I53=ev.sprite_api.turn_to_2d(ev,I52,0.0,0.0,800.0,600.0);
  ML I54=ev.mainloop_api.array_ml(std::vector<ML>{ml,I53});
  return I54;
}

int find_str(std::string val, std::string repl)
{
  std::size_t pos = val.find(repl);
  if (pos==std::string::npos) return -1;
  return pos;
}

std::string replace_str(std::string val, std::string repl, std::string subst)
{
  std::string s = "";
  int p = 0;
  while(1) {
    int pos = find_str(val,repl);
    if (pos==-1) { s+=val.substr(p,val.size()-p); return s; }
    s+=val.substr(p,pos-p);
    s+=subst;
    val[pos]='X'; // remove %N
    p=pos+repl.size();
  }

  while(1) {
  int pos = find_str(val, repl);
  if (pos==-1) return val;
  val.replace(pos,pos+repl.size()-1,subst);
  }
  return "ERROR";
}
void P_cb(void *data);

class P_script : public FaceCollection
{
public:
  P_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), coll(0) {
    e.async_load_callback(url, &P_cb, this);
#ifdef EMSCRIPTEN
    async_pending_count++; async_taken = true;
#endif
       std::cout << "async_pending_count inc (P_sctipr) " << async_pending_count << std::endl;
       std::cout << "P_script url: " << url << std::endl;
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    std::string code(vec->begin(), vec->end());
    code = replace_str(code, "%1", p1);
    code = replace_str(code, "%2", p2);
    code = replace_str(code, "%3", p3);
    code = replace_str(code, "%4", p4);
    code = replace_str(code, "%5", p5);
    GameApi::ExecuteEnv e2;
    std::pair<int,std::string> p = GameApi::execute_codegen(e,ev,code,e2);
    if (p.second=="P") {
      GameApi::P pp;
      pp.id = p.first;
      p_data = pp;
      coll = find_facecoll(e,p_data);
#ifdef EMSCRIPTEN
      if (async_taken)
      	async_pending_count--;
#endif
      async_taken=false;
       std::cout << "async_pending_count inc (P_sctipt) " << async_pending_count << std::endl;

      return;
    }
#ifdef EMSCRIPTEN
    if (async_taken)
      async_pending_count--;
#endif
    async_taken = false;
       std::cout << "async_pending_count inc (P_sctipt2) " << async_pending_count << std::endl;
    
  }
  void Prepare() { 
    if (!coll) { 
#ifdef EMSCRIPTEN
      std::cout << "P_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2(); }
    if (coll) {
      coll->Prepare();
      return;
    }
    GameApi::P pp;
    pp.id = -1;
    p_data = pp;
    coll = 0;
  }
  virtual int NumFaces() const { if (!coll) return 0; return coll->NumFaces(); }
  virtual int NumPoints(int face) const { if (!coll) return 3; return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { if (!coll) return Point(0.0,0.0,0.0); return coll->FacePoint(face,point); }
  virtual Vector PointNormal(int face, int point) const { if (!coll) return Vector(0.0,0.0,0.0); return coll->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { if (!coll) return 0.0; return coll->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { if (!coll) return 0; return coll->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { if (!coll) return 0xffffffff; return coll->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { if (!coll) { Point2d p; p.x = 0.0; p.y = 0.0; return p; } return coll->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { if (!coll) return 0.0; return coll->TexCoord3(face,point); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  GameApi::P p_data;
  FaceCollection *coll;
  bool async_taken;
};

void P_cb(void *data)
{
  std::cout << "P_cb" << std::endl;
  P_script *script = (P_script*)data;
  script->Prepare2();
}


GameApi::P GameApi::MainLoopApi::load_P_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  return add_polygon2(e, new P_script(e,ev,url, p1,p2,p3,p4,p5));
}

std::vector<std::string> parse_sep(std::string s, char sep)
{
  int ss = s.size();
  int pos = 0;
  std::vector<std::string> vec;
  for(int i=0;i<ss;i++)
    {
      if (s[i]==sep) {
	std::string beg = s.substr(pos, i-pos);
	vec.push_back(beg);
	pos = i+1;
      }
    }
  std::string beg = s.substr(pos,s.size()-pos);
  vec.push_back(beg);
  std::cout << "Parse sep: ";
  int s2 = vec.size();
  for(int i=0;i<s2;i++) std::cout << vec[i] << "::";
  std::cout << std::endl;
  return vec;
}

GameApi::ARR GameApi::MainLoopApi::load_P_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }
     std::stringstream ss;
     ss << i;
     
     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     FaceCollection *coll = new P_script(e,ev,url, k1,k2,k3,k4,k5);
     GameApi::P coll_p = add_polygon2(e, coll);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}



void ML_cb(void* data);


class ML_script : public MainLoopItem
{
public:
  ML_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url),p1(p1), p2(p2), p3(p3), p4(p4), p5(p5) , main2(0) { firsttime = true; 
       e.async_load_callback(url, &ML_cb, this); 
#ifdef EMSCRIPTEN
       async_pending_count++; async_taken=true;
#endif
       std::cout << "async_pending_count inc (ML_sctipr) " << async_pending_count << std::endl;
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepage);
#endif
      std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
      std::string code(vec->begin(), vec->end());
      code = replace_str(code, "%1", p1);
      code = replace_str(code, "%2", p2);
      code = replace_str(code, "%3", p3);
      code = replace_str(code, "%4", p4);
      code = replace_str(code, "%5", p5);
      GameApi::ExecuteEnv e2;
      std::pair<int,std::string> p = GameApi::execute_codegen(e,ev,code,e2);
      if (p.second=="ML") {
	GameApi::ML pp;
	pp.id = p.first;
	main2 = find_main_loop(e,pp);
#ifdef EMSCRIPTEN
	if (async_taken)
	  async_pending_count--;
#endif
	std::cout << "async_pending_count dec (ML_sctipr) " << async_pending_count << std::endl;
	async_taken = false;
	//main2->execute(e3);
	//firsttime = false;
	return;
      }
      //GameApi::P pp;
      //pp.id = -1;
      main2 = 0;
#ifdef EMSCRIPTEN
      if (async_taken)
      async_pending_count--;
#endif
      async_taken = false;
       std::cout << "async_pending_count dec (ML_sctipr2) " << async_pending_count << std::endl;

  }
  virtual void execute(MainLoopEnv &e3)
  {
    if (firsttime) {
      if (!main2) {
#ifdef EMSCRIPTEN
      std::cout << "ML_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2();
      }
      firsttime = false;
    }
    if (main2)
      main2->execute(e3);
  }

  virtual void handle_event(MainLoopEvent &e)
  {
    if (main2) {
      main2->handle_event(e);
    }
  }
  virtual int shader_id() { 
    if (main2) {
      return main2->shader_id();
    }
    return -1;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  bool firsttime;
  MainLoopItem *main2;
  bool async_taken;
};

void ML_cb(void *data)
{
  std::cout << "ML_cb" << std::endl;
  ML_script *script = (ML_script*)data;
  script->Prepare2();
}


GameApi::ML GameApi::MainLoopApi::load_ML_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  return add_main_loop(e, new ML_script(e,ev,url,p1,p2,p3,p4,p5));
}

GameApi::ARR GameApi::MainLoopApi::load_ML_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }

     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     MainLoopItem *coll = new ML_script(e,ev,url, k1,k2,k3,k4,k5);
     GameApi::ML coll_p = add_main_loop(e, coll);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}



void BM_cb(void *data);

class BM_script : public Bitmap<Color>
{
public:
  BM_script(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5) : e(e), ev(ev), url(url), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), bitmap(0) 
  {
    e.async_load_callback(url, &BM_cb, this);
#ifdef EMSCRIPTEN
    async_pending_count++; async_taken = true;
#endif
  }
  void Prepare2() {
    std::string homepage = gameapi_homepageurl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    std::string code(vec->begin(), vec->end());
    //std::cout << "BM_scriptA: " << code << std::endl;
    code = replace_str(code, "%1", p1);
    code = replace_str(code, "%2", p2);
    code = replace_str(code, "%3", p3);
    code = replace_str(code, "%4", p4);
    code = replace_str(code, "%5", p5);
    //std::cout << "BM_script: " << code << std::endl;
    GameApi::ExecuteEnv e2;
    std::pair<int, std::string> p = GameApi::execute_codegen(e,ev,code,e2);
    if (p.second=="BM") {
      GameApi::BM bm;
      bm.id = p.first;
      BitmapHandle *handle = find_bitmap(e, bm);
      bitmap = find_color_bitmap(handle);
#ifdef EMSCRIPTEN
      if (async_taken)
      	async_pending_count--;
#endif
      async_taken=false;
      return;
    }
    bitmap=0;
#ifdef EMSCRIPTEN
    if (async_taken)
      async_pending_count--;
#endif
    async_taken=false;

  }
  void Prepare() {
    if (bitmap)
     bitmap->Prepare();
    else {
#ifdef EMSCRIPTEN
      std::cout << "BM_script: script not ready at Prepare()" << std::endl;
#endif
      Prepare2();
      if (bitmap)
	bitmap->Prepare();
    }
  }
  virtual int SizeX() const { if (bitmap) return bitmap->SizeX(); return 1; }
  virtual int SizeY() const { if (bitmap) return bitmap->SizeY(); return 1; }
  virtual Color Map(int x, int y) const
  {
    if (bitmap) {
      return bitmap->Map(x,y);
    }
    return Color();
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string p1,p2,p3,p4,p5;
  Bitmap<Color> *bitmap;
  bool async_taken;
};

void BM_cb(void *data)
{
  std::cout << "BM_cb" << std::endl;

  BM_script *script = (BM_script*)data;
  script->Prepare2();
}


GameApi::BM GameApi::MainLoopApi::load_BM_script(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  Bitmap<Color> *bm = new BM_script(e,ev,url,p1,p2,p3,p4,p5);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = bm;
  return add_bitmap(e, handle2);
}

GameApi::ARR GameApi::MainLoopApi::load_BM_script_array(EveryApi &ev, std::string url, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5)
{
  std::vector<std::string> p1vec = parse_sep(p1,'&');
  std::vector<std::string> p2vec = parse_sep(p2,'&');
  std::vector<std::string> p3vec = parse_sep(p3,'&');
  std::vector<std::string> p4vec = parse_sep(p4,'&');
  std::vector<std::string> p5vec = parse_sep(p5,'&');

 ArrayType *array = new ArrayType;
 array->type=2;
 int s1 = p1vec.size();
 int s2 = p2vec.size();
 int s3 = p3vec.size();
 int s4 = p4vec.size();
 int s5 = p5vec.size();
 int s12 = std::max(s1,s2);
 int s123 = std::max(s12,s3);
 int s1234 = std::max(s123,s4);
 int s12345 = std::max(s1234, s5);
 for(int i=0;i<s12345;i++)
   {
     std::string k1 = "";
     if (i<s1) { k1 = p1vec[i]; }
     std::string k2 = "";
     if (i<s2) { k2 = p2vec[i]; }
     std::string k3 = "";
     if (i<s3) { k3 = p3vec[i]; }
     std::string k4 = "";
     if (i<s4) { k4 = p4vec[i]; }
     std::string k5 = "";
     if (i<s5) { k5 = p5vec[i]; }
     if (i!=0) { e.async_load_url(url,gameapi_homepageurl); }
     Bitmap<Color> *coll = new BM_script(e,ev,url, k1,k2,k3,k4,k5);
     BitmapColorHandle *handle2 = new BitmapColorHandle;
     handle2->bm = coll;
     GameApi::BM coll_p = add_bitmap(e, handle2);
     array->vec.push_back(coll_p.id);
   }
 return add_array(e,array);
}


struct FileHeader
{
  char d;
  char s;
  int type;
  int numblocks;
};
struct FileBlock
{
  int block_type;
  int block_size_in_bytes;
  int block_offset_from_beginning_of_file;
};

class LoadDS : public DiskStore
{
public:
  LoadDS(std::vector<unsigned char> file) : vec(file), valid(false) { }
  void Prepare() {
    header = (FileHeader*)&vec[0];
    blocks = (FileBlock*)&vec[sizeof(FileHeader)];
    wholefile = (unsigned char*)&vec[0];
    valid = header->d=='d' && header->s=='s';
    std::cout << "LoadDS: valid=" << valid << " " << header->d << " " << header->s << std::endl;
  }
  int Type() const { return header->type; }
  int NumBlocks() const { return valid ? header->numblocks : 0; }
  int BlockType(int block) const { return blocks[block].block_type; }
  int BlockSizeInBytes(int block) const { return blocks[block].block_size_in_bytes; }
  unsigned char *Block(int block) const { return wholefile + blocks[block].block_offset_from_beginning_of_file; }
private:
  std::vector<unsigned char> vec;
  FileHeader *header;
  FileBlock *blocks;
  unsigned char *wholefile;
  bool valid;
};

GameApi::DS GameApi::MainLoopApi::load_ds_from_disk(std::string filename)
{
  std::ifstream t(filename,std::ios::in | std::ios::binary);
  std::string str;
  if (t) {
    t.seekg(0,std::ios::end);
    str.resize(t.tellg());
    t.seekg(0,std::ios::beg);
    t.read(&str[0],str.size());
    t.close();
  }
  std::vector<unsigned char> vec(str.begin(),str.end());
  return add_disk_store(e, new LoadDS(vec));
}
GameApi::DS GameApi::MainLoopApi::load_ds_from_mem(std::vector<unsigned char> vec)
{
   return add_disk_store(e, new LoadDS(vec));
}

void GameApi::MainLoopApi::save_ds(std::string output_filename, DS ds)
{
  std::ofstream ff(output_filename, std::ios::out | std::ios::binary);

  int offset = 0;
  DiskStore *dds = find_disk_store(e, ds);
  dds->Prepare();
  FileHeader h;
  h.d = 'd';
  h.s = 's';
  h.type = dds->Type();
  h.numblocks = dds->NumBlocks();
  std::cout << "Writing Header:" << sizeof(FileHeader) << std::endl;
  ff.write((char*)&h, (int)sizeof(FileHeader));
  offset += sizeof(FileHeader);
  int s = h.numblocks;
  offset += sizeof(FileBlock)*s;
  for(int i=0;i<s;i++)
    {
      FileBlock b;
      b.block_type = dds->BlockType(i);
      b.block_size_in_bytes = dds->BlockSizeInBytes(i);
      b.block_offset_from_beginning_of_file = offset;
      offset += b.block_size_in_bytes;
      std::cout << "Writing FileBlock:" << sizeof(FileBlock) << std::endl;
      ff.write((char*)&b,(int)sizeof(FileBlock));
    }
  for(int i=0;i<s;i++)
    {
      int s = dds->BlockSizeInBytes(i);
      unsigned char *ptr = dds->Block(i);
      std::cout << "Writing Block:" << s << std::endl;
      ff.write((char*)ptr, s);
    }
  std::cout << "Closing file" << std::endl;
  ff.close();
}

class RandomIntBitmap : public Bitmap<int>
{
public:
  RandomIntBitmap(int sx, int sy, int min_value, int max_value) : sx(sx), sy(sy), min_value(min_value), max_value(max_value) {}
  void Prepare() { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  int Map(int x, int y) const {
    Random r;
    float val = double(r.next())/r.maximum();
    val*=(max_value-min_value);
    val+=min_value;
    return int(val);
  }
private:
  int sx,sy;
  int min_value, max_value;
};

GameApi::IBM GameApi::BitmapApi::random_int_bitmap(int sx, int sy, int min_value, int max_value)
{
  return add_int_bitmap(e,new RandomIntBitmap(sx,sy,min_value,max_value));
}

class NormalMove : public InputForMoving
{
public:
  NormalMove() : up(false), down(false), left(false), right(false),x(0.0), y(0.0) { }
  void execute(MainLoopEnv &e) { calc(); }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }

  }
  float XPos() const { 
    return x;
  }

  float YPos() const {
    return y;
  }
  float ZPos() const { return 0.0; }
  void calc() {
    x=0.0;
    y=0.0;
    if (up && !down && !left && !right) y=1.0;
    if (!up && down && !left && !right) y=-1.0;

    if (!up && !down && left && !right) x=-1.0;
    if (!up && !down && !left && right) x=1.0;

    if (up && !down && left && !right) { y=1.0/sqrt(2.0); x=-1.0/sqrt(2.0); }
    if (!up && down && left && !right) { y=-1.0/sqrt(2.0); x=-1.0/sqrt(2.0); }

    if (up && !down && !left && right) { y=1.0/sqrt(2.0); x=+1.0/sqrt(2.0); }
    if (!up && down && !left && right) { y=-1.0/sqrt(2.0); x=+1.0/sqrt(2.0); }
  }
private:
  bool up;
  bool down;
  bool left;
  bool right;
  float x;
  float y;
};

GameApi::INP GameApi::MainLoopApi::cursor_keys_normal_move()
{
  return add_move_input(e, new NormalMove);
}

class MoveSequence : public InputForMoving
{
public:
  MoveSequence(float time_per_char, std::string str, float start_time) : time(time_per_char), str(str), start_time(start_time) { 
  }
  void execute(MainLoopEnv &env)
  {
    float t = (env.time*1000.0-start_time)/100.0;
    int val = int(t/time);
    int s = str.size();
    x=0.0;
    y=0.0;
    z=0.0;
    if (val>=0 && val<s) {
      char ch = str[val];
      if (ch=='w') { y=-1.0; }
      if (ch=='s') { y=1.0; }
      if (ch=='a') { x=-1.0; }
      if (ch=='d') { x=1.0; }
      if (ch=='z') { z=-1.0; }
      if (ch=='x') { z=1.0; }
    }
  }  
  void handle_event(MainLoopEvent &e)
  {
  }
  float XPos() const { 
    return x;
  }

  float YPos() const {
    return y;
  }
  float ZPos() const { return z; }
private:
  float time;
  std::string str;
  float start_time;
  float x,y,z;
};

class RotateMove : public InputForMoving
{
public:
  RotateMove(float speed_rot, float r_forward, float r_normal, float r_backward) : speed_rot(speed_rot), r_forward(r_forward), r_normal(r_normal), r_backward(r_backward) { }
  void execute(MainLoopEnv &e) { 
    calc();
  }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }
  }
  void calc() { 
    if (left) current_angle-=speed_rot;
    if (right) current_angle+=speed_rot;
    if (up) current_radius=r_forward; 
    else
      if (down) current_radius=r_backward; 
      else
	current_radius=r_normal;
  }
  float XPos() const { 
    return current_radius*cos(current_angle);
  }

  float YPos() const {
    return current_radius*sin(current_angle);
  }
  float ZPos() const { return 0.0; }

private:
  float speed_rot, r_forward, r_normal, r_backward;
  float current_angle;
  float current_radius;
  bool up,down,left,right;
};

GameApi::INP GameApi::MainLoopApi::cursor_keys_rotate_move(float speed_rot, float r_forward, float r_normal, float r_backward)
{
  return add_move_input(e, new RotateMove(speed_rot, r_forward, r_normal, r_backward));
}
GameApi::INP GameApi::MainLoopApi::move_sequence(float start_time, float time_per_char, std::string str)
{
  return add_move_input(e, new MoveSequence(time_per_char, str, start_time));
}

class CollisionMove : public InputForMoving
{
public:
  CollisionMove(Bitmap<int> *scene, float start_x, float end_x, float start_y, float end_y, float s_x, float m_x, float e_x, float s_y, float m_y, float e_y, float speed_up, float speed_down, float speed_left, float speed_right, float speed_gravity) : scene(scene), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y),up(false),down(false),left(false), right(false),s_x(s_x), m_x(m_x), e_x(e_x), s_y(s_y), m_y(m_y), e_y(e_y), speed_up(speed_up), speed_down(speed_down), speed_left(speed_left), speed_right(speed_right), speed_gravity(speed_gravity) { firsttime = true; xpos=0.0; ypos=0.0; zpos=0.0; up=false; down=false; left=false; right=false; }
  void execute(MainLoopEnv &e) { 
    if (firsttime) { firsttime = false; scene->Prepare(); }
    Point p(xpos+e_x,ypos+m_y,zpos);
    Point2d pp = to_bm(p);
    int val = scene->Map(int(pp.x),int(pp.y));
    if (right && val==0) { xpos+=speed_right*e.delta_time; }

    Point p2(xpos+s_x,ypos+m_y,zpos);
    Point2d pp2 = to_bm(p2);
    int val2 = scene->Map(int(pp2.x),int(pp2.y));
    if (left && val2==0) { xpos+=speed_left*e.delta_time; }

    Point p3(xpos+m_x,ypos+s_y,zpos);
    Point2d pp3 = to_bm(p3);
    int val3 = scene->Map(int(pp3.x),int(pp3.y));
    if (up && val3==0) { ypos+=speed_up*e.delta_time; }

    Point p4(xpos+m_x,ypos+e_y,zpos);
    Point2d pp4 = to_bm(p4);
    int val4 = scene->Map(int(pp4.x),int(pp4.y));
    if (down && val4==0) { ypos+=speed_down*e.delta_time; }
    if (val4==0) { ypos+=speed_gravity*e.delta_time; }
  }
  Point2d to_bm(Point p) const
  {
    p-=Vector(start_x,start_y,0.0);
    p.x /= (end_x-start_x);
    p.y /= (end_y-start_y);
    if (p.x<0.0 || p.x>1.0) { } else
      if (p.y<0.0 || p.y>1.0) { } else
	{
	  p.x *= scene->SizeX();
	  p.y *= scene->SizeY();
	}
    Point2d pp = { p.x,p.y };
    return pp;
  }
  void handle_event(MainLoopEvent &e)
  {
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x300) { up=true; }
    if ((e.ch=='w'||e.ch==26 || e.ch==82) && e.type==0x301) { up=false; }

    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x300) { down=true; }
    if ((e.ch=='s'||e.ch==22 || e.ch==81) && e.type==0x301) { down=false; }
    
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { left=true; }
    if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { left=false; }

    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { right=true; }
    if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { right=false; }

  }
  float XPos() const {
    return xpos;
  }
  float YPos() const {
    return ypos;
  }
  float ZPos() const
  {
    return zpos;
  }

private:
  Bitmap<int> *scene;
  float start_x, end_x;
  float start_y, end_y;
  bool up,down,left,right;
  float xpos,ypos,zpos;
  float s_x,m_x,e_x,s_y,m_y,e_y;
  float speed_up, speed_down, speed_left, speed_right;
  bool firsttime;
  float speed_gravity;
};

class MoveSpeed : public InputForMoving
{
public:
  MoveSpeed(InputForMoving *next, float speed) : next(next), speed(speed),x(0.0),y(0.0),z(0.0) { }
  void execute(MainLoopEnv &e) { 
    next->execute(e);
    x+=speed*next->XPos()*e.delta_time;
    y+=speed*next->YPos()*e.delta_time;
    z+=speed*next->ZPos()*e.delta_time;
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  float XPos() const {
    return x;
  }
  float YPos() const {
    return y;
  }
  float ZPos() const
  {
    return z;
  }
private:
  InputForMoving *next;
  float speed;
  float x,y,z;
};
GameApi::INP GameApi::MainLoopApi::move_collision(IBM scene, float start_x, float end_x, float start_y, float end_y, float s_x, float m_x, float e_x, float s_y, float m_y, float e_y, float speed_up, float speed_down, float speed_left, float speed_right, float speed_gravity)
{
  Bitmap<int> *bm = find_int_bitmap(e, scene);
  return add_move_input(e, new CollisionMove(bm, start_x, end_x, start_y, end_y, s_x, m_x, e_x, s_y, m_y, e_y, speed_up, speed_down, speed_left, speed_right, speed_gravity));
}
GameApi::INP GameApi::MainLoopApi::move_speed(INP orig, float speed)
{
  InputForMoving *o = find_move_input(e, orig);
  return add_move_input(e, new MoveSpeed(o, speed));
};


class MoveIN : public MainLoopItem
{
public:
  MoveIN(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *item, InputForMoving *in) : e(e), ev(ev), item(item), in(in) { }
  virtual void execute(MainLoopEnv &env)
  {
    GameApi::M mat = ev.matrix_api.trans(in->XPos(), in->YPos(), in->ZPos());
    GameApi::M m2 = add_matrix2(e,env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
 
    GameApi::SH s1;
    s1.id = env.sh_texture;
    GameApi::SH s11;
    s11.id = env.sh_texture_2d;
    GameApi::SH s2;
    s2.id = env.sh_array_texture;
    GameApi::SH s3;
    s3.id = env.sh_color;
 
   ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);
 

    MainLoopEnv ee = env;
    ee.in_MV = find_matrix(e, mat2);
    ee.env = find_matrix(e,mat2);
    item->execute(ee);
    in->execute(ee);
    ev.shader_api.unuse(s3);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
    in->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  InputForMoving *in;
};
GameApi::ML GameApi::MainLoopApi::move_in(EveryApi &ev, ML ml, INP in)
{
  MainLoopItem *item = find_main_loop(e,ml);
  InputForMoving *in2 = find_move_input(e,in);
  return add_main_loop(e, new MoveIN(e,ev,item, in2));
}

class Gravity2 : public InputForMoving
{
public:
  Gravity2(InputForMoving *mv, Bitmap<int> *bm, float start_x, float end_x, float start_y, float end_y, float speed) : mv(mv), bm(bm), speed(speed), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { ypos=0.0; firsttime = true; }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
      bm->Prepare();
    }
    Point p(mv->XPos(), mv->YPos()+ypos, mv->ZPos());
    p-=Vector(start_x,start_y,0.0);
    p.x /= (end_x-start_x);
    p.y /= (end_y-start_y);
    if (p.x<0.0 || p.x>1.0) { } else
      if (p.y<0.0 || p.y>1.0) { } else
	{
	  p.x *= bm->SizeX();
	  p.y *= bm->SizeY();
	  int val = bm->Map(int(p.x),int(p.y));
	  if (val==0) { ypos+=speed*e.delta_time; }
	}
    mv->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { mv->handle_event(e); }
  virtual float XPos() const { return mv->XPos(); }
  virtual float YPos() const { return mv->YPos()+ypos; }
  virtual float ZPos() const { return mv->ZPos(); }
private:
  InputForMoving *mv;
  Bitmap<int> *bm;
  float speed;
  float start_x, end_x;
  float start_y, end_y;
  float ypos;
  bool firsttime;
};

GameApi::INP GameApi::MainLoopApi::gravity(INP im, IBM bitmap, float start_x, float end_x, float start_y, float end_y, float speed)
{
  InputForMoving *mv = find_move_input(e, im);
  Bitmap<int> *bm = find_int_bitmap(e, bitmap);
  return add_move_input(e, new Gravity2(mv, bm, start_x, end_x, start_y, end_y, speed));
}


std::vector<float> dyn_points_global_x;
std::vector<float> dyn_points_global_y;
std::vector<float> dyn_points_global_z;


class DynPoints : public MainLoopItem
{
public:
  DynPoints(GameApi::Env &ee,GameApi::EveryApi &ev, MainLoopItem *next, Movement *mn, int pointnum, Point start_pos) : ee(ee), ev(ev), next(next), mn(mn), pointnum(pointnum), start_pos(start_pos) {
    start_time = 0.0; //ev.mainloop_api.get_time();
    firsttime = true;
    firsttime2 = false;
  }
  void reset_time()
  {
    start_time = ev.mainloop_api.get_time();
  }

  virtual void execute(MainLoopEnv &e)
  {
    if (!mn ||!next) return;
    MainLoopEnv ee = e; 
   mn->frame(ee);
    if (firsttime) {
      firsttime2 = true;
    }
    if (!firsttime && firsttime2)
      {
	reset_time();
	firsttime2 = false;
      }
    firsttime = false;

    float time = (e.time*1000.0-start_time) / 100.0;
    Matrix m = mn->get_whole_matrix(time,ev.mainloop_api.get_delta_time());
    Point pp = start_pos*m*e.env;
    int s = dyn_points_global_x.size();
    while (pointnum>s-1) { 
      dyn_points_global_x.push_back(pp.x); 
      dyn_points_global_y.push_back(pp.y); 
      dyn_points_global_z.push_back(pp.z); 
      s=dyn_points_global_x.size(); 
    }
    if (pointnum>=0) {
      dyn_points_global_x[pointnum] = pp.x;
      dyn_points_global_y[pointnum] = pp.y;
      dyn_points_global_z[pointnum] = pp.z;
    }
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
    mn->event(e);
  }
  virtual int shader_id() { return next?next->shader_id():-1; }

private:
  GameApi::Env &ee;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Movement *mn;
  int pointnum;
  bool firsttime;
  bool firsttime2;
  float start_time;
  Point start_pos;
};

GameApi::ML GameApi::MainLoopApi::dyn_points(EveryApi &ev, ML ml, MN move, int pointnum, float pos_x, float pos_y, float pos_z)
{
  MainLoopItem *ma = find_main_loop(e,ml);
  Movement *mn = find_move(e, move);
  Point pos(pos_x,pos_y,pos_z);
  return add_main_loop(e, new DynPoints(e,ev, ma, mn, pointnum, pos));
}
extern std::map<std::string, int> number_map;
int score=0; // remember to add to number_map["score"]
int hidden_score=0;

class ScoreAdder : public MainLoopItem
{
public:
  ScoreAdder(GameApi::EveryApi &ev,MainLoopItem *item, VolumeObject *obj, Movement *move, int enter_score, int leave_score, int dyn_point, float timeout) : ev(ev), item(item), obj(obj), move(move), enter_score(enter_score), leave_score(leave_score), dyn_point(dyn_point),timeout(timeout) {
    prev_p = false;
    prev_time = 0.0;
    prev_time2 = 0.0;
}

  void enter_event(float time) {
    if (time>prev_time+timeout) {
      prev_time = time;
      score+=enter_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  void leave_event(float time) {
    if (time>prev_time2+timeout) {
      prev_time2 = time;
      score+=leave_score;
      if (score<0) score=0;
      if (score>99999) score=99999;
      number_map["score"] = score;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {
    Point p = Point(0, 0, -400);
    //Matrix m = Matrix::YRotation(-quake_rot_y);
    //p = p * m;
    //p.x*=1.2;
    //p.y*=1.2;
    //p.z*=1.2;
    //Matrix m2 = Matrix::YRotation(-quake_rot_y);
    //p = p * m2;
    p.x-=quake_pos_x;
    p.z-=quake_pos_y;

    debug_pos_x = p.x;
    debug_pos_y = p.y;
    debug_pos_z = p.z;

    int s = dyn_points_global_x.size();
    if (dyn_point>=0 && dyn_point<s)
      {
	p = Point(dyn_points_global_x[dyn_point],dyn_points_global_y[dyn_point],dyn_points_global_z[dyn_point]);
      }
    p = p * Matrix::Inverse(move->get_whole_matrix(e.time*10.0, ev.mainloop_api.get_delta_time()));


    bool b = obj->Inside(p);
    if (b && b!=prev_p) { prev_p = b; enter_event(e.time*10.0); } else
    if (!b && b!=prev_p) { prev_p = b; leave_event(e.time*10.0); }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  VolumeObject *obj;
  Movement *move;
  int enter_score, leave_score;
  int dyn_point;
  float timeout;
  bool prev_p;
  float prev_time, prev_time2;
};

float debug_pos_x=0.0, debug_pos_y=0.0, debug_pos_z=0.0;

GameApi::ML GameApi::MainLoopApi::debug_obj(EveryApi &ev)
{  
  P p = ev.polygon_api.cube(-3.0,3.0, -3.0, 3.0, -3.0,3.0);
  MT mat0 = ev.materials_api.def(ev);
  MT mat = ev.materials_api.snow(ev,mat0,0xffaaaaaa, 0xffeeeeee, 0xffffffff, 0.95);
  ML ml1 = ev.materials_api.bind(p,mat);
  MN mn2 = ev.move_api.empty();
  MN mn = ev.move_api.debug_translate(mn2);
  ML ml2 = ev.move_api.move_ml(ev,ml1,mn,1,10.0);
  return ml2;
}

GameApi::ML GameApi::MainLoopApi::score_adder(EveryApi &ev,ML ml, O o, MN transform, int enter_score, int leave_score, int dyn_point, float timeout)
{
  MainLoopItem *item = find_main_loop(e,ml);
  VolumeObject *obj = find_volume(e, o);
  Movement *mv = find_move(e, transform);
  return add_main_loop(e, new ScoreAdder(ev,item, obj, mv, enter_score, leave_score, dyn_point,timeout));
}

class ScoreHidder : public MainLoopItem
{
public:
  ScoreHidder(GameApi::EveryApi &ev, MainLoopItem *item, VolumeObject *obj, int max_count) : ev(ev), item(item), obj(obj), max_count(max_count) { prev_p=false; }

  void enter_event(float time) {
      int transfer = score>max_count?max_count:score;
      if (transfer<0) transfer=0;
      hidden_score+=transfer;
      score-=transfer;
      number_map["score"] = score;
  }
  void leave_event(float time) {
      int transfer = score>max_count?max_count:score;
      if (transfer<0) transfer=0;
      hidden_score+=transfer;
      score-= transfer;
      number_map["score"] = score;
  }

  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
  virtual void execute(MainLoopEnv &e)
  {
    Point p = Point(0, 0, -400);
    p.x-=quake_pos_x;
    p.z-=quake_pos_y;

    bool b = obj->Inside(p);
    if (b && b!=prev_p) { prev_p = b; enter_event(e.time*10.0); } else
    if (!b && b!=prev_p) { prev_p = b; leave_event(e.time*10.0); }
    item->execute(e);
  }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  VolumeObject *obj;
  int max_count;
  bool prev_p;
};

GameApi::ML GameApi::MainLoopApi::score_hidder(EveryApi &ev, ML ml, O o, int max_count)
{
  MainLoopItem *item = find_main_loop(e,ml);
  VolumeObject *obj = find_volume(e, o);
  return add_main_loop(e, new ScoreHidder(ev,item,obj,max_count));
}

GameApi::MT GameApi::MaterialsApi::noise(EveryApi &ev, int sx, int sy, int r, int g, int b, int a,
					 int r2, int g2, int b2, int a2, float mix)
{
  BM I1=ev.bitmap_api.noise_vectors(sx,sy);
  FB I2=ev.float_bitmap_api.from_red(I1);
  BM I3=ev.float_bitmap_api.to_grayscale_color(I2,r,g,b,a,r2,g2,b2,a2);
  MT I4=ev.materials_api.texture_many(ev,std::vector<BM>{I3},mix);
  return I4;
}

class LinesCurveGroup : public CurveGroup
{
public:
  LinesCurveGroup(LineCollection *coll) : coll(coll) { }
  void Prepare() { coll->Prepare(); }
  int NumCurves() const { return coll->NumLines(); }
  Point Pos(int num, float t) const
  {
    Point p1 = coll->LinePoint(num, 0);
    Point p2 = coll->LinePoint(num, 1);
    LineIn3d line(p1,p2);
    return line.Index(t);
  }
  unsigned int Color(int num, float t) const
  {
    unsigned int c1 = coll->LineColor(num,0);
    unsigned int c2 = coll->LineColor(num,1);
    return Color::Interpolate(c1,c2,t);
  }
  int Shape(int num, float t) const { return 0; }
private:
  LineCollection *coll;
};

GameApi::CG GameApi::CurveApi::curve_group_from_lines(LI li)
{
  LineCollection *coll = find_line_array(e, li);
  return add_curve_group(e, new LinesCurveGroup(coll));
}

class LinesFromQuads : public LineCollection
{
public:
  LinesFromQuads(FaceCollection *coll, int sx, int sy) : coll(coll), sx(sx), sy(sy) { }
  void Prepare() { coll->Prepare(); }
  int NumLines() const { return coll->NumFaces()*(sx+sy+1); }
  Point LinePoint(int line, int point) const
  {
    int s = coll->NumFaces();
    int a1_r = line/s;
    int a1 = line-a1_r*s;
    int a2 = a1_r;
    bool b = false;
    if (a2>=sx) { b=true; a2-=sx; } 
    float t = float(a2);
    if (b) { t/=float(sy); } else { t/=float(sx); }

    if (!b) {
      if (point==0) {
	Point p1 = coll->FacePoint(a1,0);
	Point p2 = coll->FacePoint(a1,1);
	LineIn3d line(p1,p2);
	Point pp1 = line.Index(t);
	return pp1;
      } else {
	Point p3 = coll->FacePoint(a1,2);
	Point p4 = coll->FacePoint(a1,3%coll->NumPoints(a1));
	LineIn3d line2(p4,p3);
	Point pp2 = line2.Index(t);
	return pp2; 
      }
    } else {
      if (point==0) {
	Point p1 = coll->FacePoint(a1,0);
	Point p4 = coll->FacePoint(a1,3%coll->NumPoints(a1));
	LineIn3d line(p1,p4);
	Point pp1 = line.Index(t);
	return pp1;
      } else {
	Point p2 = coll->FacePoint(a1,1);
	Point p3 = coll->FacePoint(a1,2);
	LineIn3d line2(p2,p3);
	Point pp2 = line2.Index(t);
	return pp2;
      }
    }
  }
  unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  FaceCollection *coll;
  int sx;
  int sy;
};

GameApi::LI GameApi::LinesApi::lines_from_quads(P p, int sx, int sy)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_line_array(e, new LinesFromQuads(coll,sx,sy));
}

class AnimCurveGroup : public CurveGroup
{
public:
  AnimCurveGroup(MeshAnim *ma, float start_time, float end_time) : ma(ma), start_time(start_time), end_time(end_time) { }
  void Prepare() { ma->Prepare(); numfaces = ma->NumFaces(); }
  int NumCurves() const { return numfaces*ma->NumPoints(0); }
  Point Pos(int num, float t) const
  {
    int s = ma->NumFaces();
    int p = num/s;
    int p0 = num - p*s;
    return ma->Vertex(p0,p,(1.0-t)*start_time + t*end_time);
  }
  unsigned int Color(int num, float t) const
  {
    int s = ma->NumFaces();
    int p = num/s;
    int p0 = num - p*s;
    return ma->Color(p0,p,(1.0-t)*start_time + t*end_time);
  }
  int Shape(int num, float t) const { return 0; }
private:
  MeshAnim *ma;
  int numfaces;
  float start_time, end_time;
};
GameApi::CG GameApi::PolygonApi::curve_group_from_anim(MA ma, float start_time, float end_time)
{
  MeshAnim *maa = find_mesh_anim(e, ma);
  return add_curve_group(e, new AnimCurveGroup(maa, start_time, end_time));
}

class LineFromCurveGroup : public LineCollection
{
public:
  LineFromCurveGroup(CurveGroup *cg, int split) : cg(cg), split(split) {}
  void Prepare() { cg->Prepare(); }
  int NumLines() const { return split*cg->NumCurves(); }
  Point LinePoint(int line, int point) const
  {
    int k = line/split;
    int k2 = line-k*split;
    float t = float(k2)/float(split);
    float t0 = float(1.0)/float(split);
    switch(point) { 
    case 0: return cg->Pos(k, t);
    case 1: return cg->Pos(k, t+t0);
    }
    Point p(0.0,0.0,0.0);
    return p;
  }
  unsigned int LineColor(int line, int point) const
  {
    int k = line/split;
    int k2 = line-k*split;
    float t = float(k2)/float(split);
    float t0 = float(1.0)/float(split);
    switch(point) {
    case 0: return cg->Color(k,t);
    case 1: return cg->Color(k,t+t0);
      };
    return 0xffffffff;
  }
private:
  CurveGroup *cg;
  int split;
};

GameApi::LI GameApi::CurveApi::lines_from_curve_group(CG curvegroup, int split)
{
  CurveGroup *cg = find_curve_group(e, curvegroup);
  return add_line_array(e, new LineFromCurveGroup(cg, split));
}

class FromImplicitVoxel : public Voxel<int>
{
public:
  FromImplicitVoxel(ImplicitFunction3d *im, int sx, int sy, int sz,
		    float start_x, float end_x,
		    float start_y, float end_y,
		    float start_z, float end_z, int value) : im(im),sx(sx),sy(sy),sz(sz),
						  start_x(start_x), end_x(end_x),
start_y(start_y), end_y(end_y),
							     start_z(start_z), end_z(end_z), value(value)
  { }
  virtual void Prepare() { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Map(int x, int y, int z) const
  {
    float xx = float(x)/float(sx)*(end_x-start_x)+start_x;
    float yy = float(y)/float(sy)*(end_y-start_y)+start_y;
    float zz = float(z)/float(sz)*(end_z-start_z)+start_z;
    float low_val = im->f_l(xx,yy);
    float high_val = im->f_u(xx,yy);
    if (zz > low_val && zz < high_val) return value;
    return -1;
  }
private:
  ImplicitFunction3d *im;
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int value;
};

GameApi::VX GameApi::VoxelApi::from_implicit(IM i, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value)
{
  ImplicitFunction3d *im = find_implicit(e,i);
  return add_int_voxel(e, new FromImplicitVoxel(im, sx,sy,sz, start_x, end_x, start_y, end_y, start_z, end_z, value));
}


class SourceBitmap
{
public:
  SourceBitmap(DrawBufferFormat fmt, int depth) : m_data(0), fmt(fmt), m_depth(depth),m_owned(false),m_owned2(false)
  {
  }
  void set_data(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 1 " << std::endl;
    unsigned int *buf = new unsigned int[width*height];
    std::memset(buf, 0, width*height);
    m_data = buf;
    std::memcpy(buf, data, width*height*sizeof(unsigned int));
    m_width=width;
    m_height=height;
    m_ydelta=ydelta;
    m_owned2 = true;
  }
  void set_data_mono1(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 2 " << std::endl;
    unsigned char *buf = new unsigned char[width*height/8];
    std::memset(buf, 0, width*height/8);
    m_data = buf;
    m_width=width;
    m_height=height;
    m_ydelta =width/8;
    for(int y=0;y<height;y++)
      for(int x=0;x<width;x++)
	{
	  unsigned int color = ((unsigned int*)data)[x+y*ydelta];
	  bool b = false;
	  //if ((color & 0xff000000)>0x80000000) b=true;
	  if ((color & 0x00ff0000)>0x00800000) b=true;
	  if ((color & 0x0000ff00)>0x00008000) b=true;
	  if ((color & 0x000000ff)>0x00000080) b=true;
	  if (b)
	    buf[x/8+y*m_ydelta] |= 1 << (7-(x%8));
	}
    m_owned = true;
  }
  ~SourceBitmap() { 
    if (m_owned) delete[]((unsigned char*)m_data); 
    if (m_owned2) delete[]((unsigned int*)m_data);
  }
public:
  void *m_data;
  DrawBufferFormat fmt;
  int m_width;
  int m_height;
  int m_ydelta;
  int m_depth;
  bool m_owned;
  bool m_owned2;
};


class LowFrameBuffer : public FrameBuffer
{
public:
  LowFrameBuffer(FrameBufferLoop *loop, int format, int width, int height, int depth) : loop(loop),m_format(format), width(width), height(height), depth(depth) { firsttime = true;  }
  virtual void Prepare()
  {
    loop->Prepare();
    // 
    FrameBufferFormat fmt = (FrameBufferFormat)m_format;
    switch(fmt) {
    case FrameBufferFormat::F_Mono1:
      buffer = new unsigned char[width*height/8];
      size = width*height/8;
      break;
    case FrameBufferFormat::F_Mono8:
      buffer = new unsigned char[width*height];
      size = width*height;
      break;
    case FrameBufferFormat::F_RGB565:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGB888:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGBA8888:
      buffer = new unsigned int[width*height];
      size=width*height*sizeof(unsigned int);
      break;
    }
    firsttime = true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    loop->handle_event(e);
  }
  virtual void frame()
  {
    // clear the buffer
    std::memset(buffer,0, size);
    if (firsttime) {
      auto p2 = std::chrono::system_clock::now();
      auto dur_in_seconds = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p2.time_since_epoch()));
      start_time = dur_in_seconds.count();
      //start_time/=1000.0;
      start_time_epoch = start_time;
      firsttime = false;
    }
    DrawLoopEnv e;
    e.format = D_RGBA8888;
    e.drawbuffer = this;
    //e.drawbuffer_width = width;
    //e.drawbuffer_height = height;
    //e.drawbuffer_depth = depth;
    auto p3 = std::chrono::system_clock::now();
    auto dur_in_seconds3 = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p3.time_since_epoch()));
    double val = dur_in_seconds3.count();  
    //val/=1000.0;
    // std::cout << "FTime: " << val << " " << start_time_epoch << std::endl;
    
    //std::chrono::time_point p = std::chrono::high_resolution_clock::now();
    e.time = float(val-start_time_epoch);
    e.delta_time = float(val-start_time);
    start_time = val;
    loop->frame(e);

    // TODO submit to iot platform
  }
  virtual void *Buffer() const { return buffer; }
  virtual int Width() const { return width; }
  virtual int Height() const { return height; }
  virtual int Depth() const { return depth; }
  virtual FrameBufferFormat format() const { return (FrameBufferFormat)m_format; }

  virtual void draw_sprite(SourceBitmap *bm, int pos_x, int pos_y)
  {
    int sp_width=bm->m_width;
    int sp_height = bm->m_height;
    int sp_ydelta = bm->m_ydelta;
    void *buf = bm->m_data;
    switch(m_format) {
    case FrameBufferFormat::F_Mono1:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      int s_pos = (x/8+y*sp_ydelta);
	      int t_pos = ((pos_x+x)/8+(y+pos_y)*(width));
	      int s_bit = x&0x7;
	      int t_bit = (x+pos_x)&0x7;
	      unsigned char b = ((unsigned char*)buf)[s_pos];
	      b>>=7-s_bit;
	      b&=1;
	      if (b) {
		((unsigned char*)buffer)[t_pos]|=1<<(7-t_bit);
	      } else {
		((unsigned char*)buffer)[t_pos]&=~(1<<(7-t_bit));
	      }
	      //((unsigned char*)buffer)[(pos_x+x)/8+(y+pos_y)*width/8] = ((unsigned char*)buf)[x/8+y*sp_ydelta];

	    }
	}
	//std::cout << "draw_sprite, mono1->mono1 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono1 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_Mono8:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->mono8 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono8 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB565:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb565 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb565 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb888 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb888 not implemented" << std::endl;
	break;
      };
      break;
    case FrameBufferFormat::F_RGBA8888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:

	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      //std::cout << "(" << x << "," << y << ") " << buffer << " " << buf << std::endl;
	      int pos = (x/8+y*sp_ydelta);
	      int bit = x&0x7;
	      unsigned char b = ((unsigned char*)buf)[pos];
	      b>>=7-bit;
	      b&=1;

	      ((unsigned int*)buffer)[pos_x+x+(y+pos_y)*width] = b?0xffffffff:0xff000000;
	    }
	} 

	//std::cout << "draw_sprite, mono1->rgba8888 not implemented" << std::endl;
	break;
      case DrawBufferFormat::D_RGBA8888:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      //std::cout << "(" << x << "," << y << ") " << buffer << " " << buf << std::endl;
	      ((unsigned int*)buffer)[pos_x+x+(y+pos_y)*width] = ((unsigned int*)buf)[x+y*sp_ydelta];
	    }
	}
	break;
      };
      break;
    }

  }
private:
  bool firsttime;
  FrameBufferLoop *loop;
  int m_format;
  void *buffer;
  int size;
  int width;
  int height;
  int depth;
  double start_time;
  double start_time_epoch;
};

GameApi::FBU GameApi::LowFrameBufferApi::low_framebuffer(GameApi::FML ml, int format, int width, int height, int depth)
{
  FrameBufferLoop *loop = find_framemainloop(e,ml);
  return add_framebuffer(e, new LowFrameBuffer(loop, format,width,height,depth));
}

void CopyFrameToSurface(FrameBuffer *buf, Low_SDL_Surface *surf)
{
  g_low->sdl->SDL_LockSurface(surf);
  int width = buf->Width();
  int height = buf->Height();
  //int depth = buf->Depth();
  void *buffer = buf->Buffer();
  FrameBufferFormat f = buf->format();
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	// GETPIXEL
	unsigned int val = 0xffff00ff;
#if 1
	switch(f) {
	case FrameBufferFormat::F_Mono1:
	  {
	  int byte = (y*width + x)/8;
	  int bit = (y*width+x)&0x7;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  v>>=7-bit;
	  v&=1;
	  val = v?0xffffffff:0xff000000;
	  //std::cout << "F_Mono1 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_Mono8:
	  {
	  int byte = y*width + x;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  unsigned int vv = v;
	  vv <<= 8;
	  unsigned int vvv = v;
	  vvv <<= 16;
	  val = 0xff000000 + v + vv + vvv;
	  //std::cout << "F_Mono8 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_RGB565:
	  //std::cout << "F_RGB565 not supported\n" << std::endl;
	  {
	  unsigned short *ptr = ((unsigned short*)buffer) + y*width+x;
	  unsigned short v = *ptr;
	  unsigned int r = v&0x001f;
	  float rv = r/16.0;
	  rv*=255;
	  unsigned int g = v&(((4+2+1)+(8+4+2))*0x10);
	  g>>=5;
	  float gv = g/32.0;
	  gv*=255;
	  unsigned int b = v&(((8+4+2+1)+(8))*(1024));
	  b>>=11;
	  float bv = b/16.0;
	  bv*=255;
	  unsigned int rr = (unsigned int)rv;
	  unsigned int gg = (unsigned int)gv;
	  unsigned int bb = (unsigned int)bv;
	  rr<<=16;
	  gg<<=8;
	  val=0xff000000 + rr+gg+bb;
	  break;
	  }
	case FrameBufferFormat::F_RGB888:
	  //std::cout << "F_RGB888 not supported\n" << std::endl;
	  {
	  unsigned int *b = ((unsigned int*)buffer) + y*width+x;
	  val = *b;
	  val |= 0xff000000;
	  break;
	  }
	case FrameBufferFormat::F_RGBA8888:
	  {
	  unsigned int *b = ((unsigned int*) buffer) + y*width + x;
	  val = *b;
	  break;
	  }
	};
#endif
	
	// PUTPIXEL
	unsigned int *target_pixel = (unsigned int*)(((unsigned char*) surf->pixels) + y*surf->pitch + x*sizeof(*target_pixel));
	*target_pixel = val;
      }
  g_low->sdl->SDL_UnlockSurface(surf);

}

void clear_sdl_surface(Low_SDL_Surface *surf, int width, int height, unsigned int val)
{
#if 0
  SDL_LockSurface(surf);
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	unsigned int *target_pixel = (unsigned int*)(((unsigned char*) surf->pixels) + y*surf->pitch + x*sizeof(*target_pixel));
	*target_pixel = val;
      }
  SDL_UnlockSurface(surf);
#endif
}
extern Low_SDL_Window *sdl_framebuffer_window;

Low_SDL_Surface *init_sdl_surface_framebuffer(int width, int height);


void get_iot_event(const GameApi::MainLoopApi::Event &e, bool *array);
class FBU_run : public Splitter
{
public:
  FBU_run(GameApi::Env &env, GameApi::EveryApi &ev, FrameBuffer *buf, int mode, int scr_x, int scr_y) : env(env), ev(ev), buf(buf), scr_x(scr_x), scr_y(scr_y) { exit=false;}
  virtual void Init() {
    surf = init_sdl_surface_framebuffer(scr_x, scr_y);
    buf->Prepare();
  }
  virtual Splitter* NextState(int code) { return 0; }
  virtual int Iter() {
    //std::cout << "FBU_run::Iter" << std::endl;
    // TODO clear the screen
    clear_sdl_surface(surf,scr_x,scr_y,0xffffff00);
    // TODO event loop.
    // 
    buf->frame();
    CopyFrameToSurface(buf, surf);
    g_low->sdl->SDL_UpdateWindowSurface(sdl_framebuffer_window);

    GameApi::MainLoopApi::Event e;
    while((e = ev.mainloop_api.get_event()).last==true)
      {
    	if (e.ch==27 && e.type==0x300) { exit=true; }

	FrameLoopEvent event;
	event.type = e.type;
	event.ch = e.ch;
	event.cursor_pos = *find_point(env,e.cursor_pos);
	event.button = e.button;
	//event.pin = 
	get_iot_event(e,&event.pin[0]);
	buf->handle_event(event);
     }

    g_low->sdl->SDL_Delay(16);
    if (exit) return 0;
    return -1;
  }
  virtual void Destroy()
  {
    g_low->sdl->SDL_FreeSurface(surf);
    g_low->sdl->SDL_DestroyWindow(sdl_framebuffer_window);
    sdl_framebuffer_window = 0;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  FrameBuffer *buf;
  Low_SDL_Surface *surf;
  int scr_x, scr_y;
  bool exit;
};

GameApi::RUN GameApi::LowFrameBufferApi::low_framebuffer_run(EveryApi &ev, GameApi::FBU buf, int mode, int scr_x, int scr_y)
{
  FrameBuffer *buf2 = find_framebuffer(e, buf);
  return add_splitter(e, new FBU_run(e,ev,buf2,mode,scr_x,scr_y));
}


void BufferRefToSourceBitmap(BufferRef &ref, SourceBitmap &target, DrawBufferFormat fmt)
{
  unsigned int *buffer = ref.buffer;
  unsigned int width = ref.width;
  unsigned int height = ref.height;
  unsigned int ydelta = ref.ydelta;
  switch(fmt) {
  case D_Mono1:
    //std::cout << "D_Mono1 not supported in BufferRefToSourceBitmap" << std::endl;
    target.set_data_mono1(buffer, width, height, ydelta);
    break;
  case D_RGBA8888:
    target.set_data(buffer, width, height, ydelta);
    break;
  default:
    std::cout << "Default not supported in BufferRefToSourceBitmap" << std::endl;
    break;
  };
}

void BitmapToSourceBitmap(Bitmap<Color> &bm, SourceBitmap &target, DrawBufferFormat fmt)
{
  //std::cout << "::" << bm.SizeX() << " " << bm.SizeY() << std::endl;
  BufferFromBitmap buf(bm);
  buf.Gen();
  BufferRef buf2 = buf.Buffer();
  BufferRefToSourceBitmap(buf2, target, fmt);
}


class SpriteDraw : public FrameBufferLoop
{
public:
  SpriteDraw(Bitmap<Color> &bm, Movement *move, int x, int y, int fmt, float start_time) : bm(bm), move(move), x(x),y(y), src(fmt?D_RGBA8888:D_Mono1,0), fmt(fmt?D_RGBA8888:D_Mono1), start_time(start_time) { 
    prepared=false;
  }
  ~SpriteDraw() { }

  virtual void Prepare()
  {
    bm.Prepare();
    BitmapToSourceBitmap(bm,src, fmt);
    prepared=true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
  }
  virtual void frame(DrawLoopEnv &e)
  {
    if (prepared) {
    move->draw_frame(e);
    Point p = {float(x),float(y),0.0};
    //std::cout << "Time: " << e.time << " " << e.delta_time << std::endl;
    Matrix m = move->get_whole_matrix(e.time*10.0, (e.delta_time*10.0));
    Point p2 = p*m;
    //std::cout << "Pos: " << p2 << std::endl;
    e.drawbuffer->draw_sprite(&src, p2.x,p2.y);
    } else { std::cout << "frame before prepare in SpriteDraw" << std::endl; }
  }
private:
  Bitmap<Color> &bm;
  Movement *move;
  int x, y;
  SourceBitmap src;
  DrawBufferFormat fmt;
  float start_time;
  bool prepared;
};

GameApi::FML GameApi::LowFrameBufferApi::low_sprite_draw(BM bm, MN mn, int x, int y, int fmt, float start_time)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  Movement *move = find_move(e, mn);
  return add_framemainloop(e,new SpriteDraw(*b2, move,x,y,fmt,start_time));

}

int last_io_pin = -1;
bool iot_firsttime = true;

void get_iot_event(const GameApi::MainLoopApi::Event &e, bool *array)
{
  static bool but[10];
  if (iot_firsttime) {
    for(int i=0;i<9;i++) but[i]=false;
    iot_firsttime = false;
  }
  if (e.type==768 && e.ch>='0' && e.ch<='9') {
    int num = e.ch-'1';
    but[num] = true;
  }
  if (e.type==769 && e.ch>='0' && e.ch<='9') {
    int num = e.ch-'1';
    but[num] = false;
  }

#ifdef IOT_EVENTS
  auto mIO = (sf::IOPin*)sf::pmap::myIO);
  mIO->setEvent(sf::IOPinEvent::IN_Rising, [](auto pin, auto pol) {
      if (pin>=0 && pin<=9)
	but[pin] = true;
    });
  mIO->setEvent(sf::IOPinEvent::IN_Falling, [](auto pin, auto pol) {
      if (pin>=0 && pin<=9)
	but[pin] = false;
    }
#endif
  for(int i=0;i<9;i++) {
    array[i] = but[i];
  }
}
