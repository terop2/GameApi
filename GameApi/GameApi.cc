        
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

#if 0
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
    FT_Init_FreeType(&lib);
    //std::cout << "Freetype init error: " << err << std::endl;
    //std::cout << &lib << std::endl;
#else
    FT_Init_FreeType(&lib);
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
  std::cout << "FREE FREETYPE!" << std::endl;
  FT_Done_FreeType(lib);
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

EXPORT void GameApi::Env::async_load_url(std::string url, std::string homepage)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->load_urls(url, homepage);
}
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
GameApi::FBO add_fbo(GameApi::Env &e, GLuint fbo_name, GLuint texture, GLuint depthbuffer, int sx, int sy)
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
  glDeleteBuffers(1, &arr->buffer);
  glDeleteBuffers(1, &arr->buffer2);
#ifdef VAO
  glDeleteVertexArrays( 1, arr->vao );
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
  env->lock();
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  env->unlock();
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

GameApi::PT add_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Point p = Point(x,y,z);
  env->lock();
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
  env->unlock();
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
      glPushMatrix();
      float mat[16] = { mm.matrix[0], mm.matrix[4], mm.matrix[8], mm.matrix[12],
			mm.matrix[1], mm.matrix[5], mm.matrix[9], mm.matrix[13],
			mm.matrix[2], mm.matrix[6], mm.matrix[10], mm.matrix[14],
			mm.matrix[3], mm.matrix[7], mm.matrix[11], mm.matrix[15] };
      
      glMultMatrixf(&mat[0]);

      poly.render_vertex_array(p.va);
      glPopMatrix();
#endif
    }
}




void *Thread_Call(void *data)
{
  GameApi::RenderObject *o = (GameApi::RenderObject*)data;
  o->prepare();
  pthread_exit(NULL);
  return 0;
}
pthread_t thread;

void GameApi::prepare(GameApi::RenderObject &o)
{
  pthread_create(&thread, NULL, Thread_Call, (void*)&o);
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
  void set_matrix(Matrix m) { }
  void set_pos(float ddx, float ddy, float ddz) { dx=ddx; dy=ddy; dz=ddz; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (time<start_time) { return next->get_whole_matrix(time, delta_time); }
    if (time>=end_time) { return Matrix::Translate(dx,dy,dz)*next->get_whole_matrix(time,delta_time); }
    float d = time - start_time;
    d/=(end_time-start_time);
    return Matrix::Translate(dx*d,dy*d,dz*d)*next->get_whole_matrix(time, delta_time);
  }
private:
  Movement *next;
  float start_time, end_time;
  float dx,dy,dz;
};
EXPORT GameApi::MN GameApi::MovementNode::translate(MN next, 
					     float start_time, float end_time,
					     float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TranslateMovement(nxt,start_time, end_time,
					   dx,dy,dz));
}
class ScaleMovement : public Movement
{
public:
  ScaleMovement(Movement *next, float start_time, float end_time,
		    float sx, float sy, float sz)
    : next(next), start_time(start_time), end_time(end_time),
      sx(sx), sy(sy), sz(sz) { }
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
  EventActivateMovement(Movement *next, Movement *event, float activate_time, float duration) : next(next), event(event), activate_time(activate_time), duration(duration) { 
    event_activated=false;
    collect = Matrix::Identity();
  }
  void set_matrix(Matrix mm) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    Matrix n = next->get_whole_matrix(time, delta_time);
    
    if (time > activate_time && time - delta_time < activate_time)
      {
	event_activated = true;
      }
    if (event_activated) { n = n * event->get_whole_matrix(time-activate_time, delta_time) * collect; }
    else { n = n * collect; }
    if (event_activated && time - activate_time > duration) {
      event_activated = false;
      collect = collect * event->get_whole_matrix(duration,delta_time);
    }
    return n;
  }
private:
  Movement *next;
  Movement *event;
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
  }
  void execute(MainLoopEnv &env)
  {
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
    float newtime = fmod(time, duration/10.0);
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
  return add_main_loop(e, new RepeatML(item, duration));  
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
				      
EXPORT GameApi::MT GameApi::MaterialsApi::texture(EveryApi &ev, BM bm, float mix)
{
  return add_material(e, new TextureMaterial(ev, bm,mix));
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
  WebMaterial(GameApi::EveryApi &ev, Material *next) : ev(ev),next(next) {}
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,0xff000000);
    GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_ml(ev,I5);
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
    GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,0xff000000);
    GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml(ev,I5,pta);
    GameApi::P I8=p; 
    //VA I9=ev.polygon_api.create_vertex_array(I8,true);
    //ML I10=ev.polygon_api.render_vertex_array_ml(ev,I9);
    GameApi::ML I10;
    I10.id = next->mat_inst(I8.id,pts.id);
    GameApi::ML I11=ev.mainloop_api.array_ml(std::vector<GameApi::ML>{I6,I10});
    return I11;
  }

  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,0xff000000);
    GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml(ev,I5,pta);
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
    GameApi::PTA pta = ev.points_api.prepare(pts);
    
    GameApi::P I2=p;
    GameApi::LI I3=ev.lines_api.from_polygon(I2);
    GameApi::LI I4=ev.lines_api.change_color(I3,0xff000000);
    GameApi::LLA I5=ev.lines_api.prepare(I4);
    GameApi::ML I6=ev.lines_api.render_inst_ml(ev,I5,pta);
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
};

EXPORT GameApi::MT GameApi::MaterialsApi::web(EveryApi &ev, MT nxt)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new WebMaterial(ev,mat));
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
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment, false, sfo);
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

    ev.shader_api.set_var(sh, "in_POS", 0.0f);

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
	  shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment);
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader=ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment, false, sfo);
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
	ev.shader_api.set_var(shader, "in_POS", 0.0f);

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
EXPORT GameApi::ML GameApi::MaterialsApi::render_instanced_ml_fade(GameApi::EveryApi &ev, P p, PTS pts, bool flip, float start_time, float end_time)
{
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts, true,flip,start_time,end_time));  
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
  return add_uber(e, new F_ShaderCallFunction("diffuse", next,"EX_NORMAL2 EX_LIGHTPOS2"));
}

GameApi::US GameApi::UberShaderApi::f_ambient(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("ambient", next,""));
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
  return add_uber(e, new F_ShaderCallFunction("specular", next,"EX_NORMAL2 EX_LIGHTPOS2 SPECULAR_SIZE"));
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
void blocker_iter(void *arg)
{
  Envi_2 *env = (Envi_2*)arg;
  //std::cout << "async: " << async_pending_count << std::endl;
  if (async_pending_count > 0) { env->logo_shown = true; }
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false;
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
	env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));

	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
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

}
extern int async_pending_count;

class MainLoopBlocker_win32_and_emscripten : public Blocker
{
public:
  MainLoopBlocker_win32_and_emscripten(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : e(e), ev(ev), code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
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
      ev.mainloop_api.display_logo(ev);
    } else {
	ev.mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     ev.mainloop_api.alpha(true);
     glEnable(GL_DEPTH_TEST);
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
     glDisable(GL_DEPTH_TEST);
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
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment, false, sfo);
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
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
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

std::string striphomepage(std::string);
void onerror_async_cb(void *arg)
{
  std::cout << "ERROR: url loading error! " << std::endl;
    char *url = (char*)arg;
    std::string url_str(url);
  std::string url_only(striphomepage(url_str));
    load_url_buffers_async[url_only] = (std::vector<unsigned char>*)-1;
    async_pending_count--;
}
std::string striphomepage(std::string);
void onload_async_cb(void *arg, void *data, int datasize)
{
  std::vector<unsigned char> buffer;
  unsigned char *dataptr = (unsigned char*)data;
  for(int i=0;i<datasize;i++) { buffer.push_back(dataptr[i]); }
  
  char *url = (char*)arg;
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  
  std::cout << "url loading complete! " << url_str << std::endl;
  load_url_buffers_async[url_only] = new std::vector<unsigned char>(buffer);
  async_pending_count--;
}

std::vector<unsigned char> load_from_url(std::string url);

void ASyncLoader::load_urls(std::string url, std::string homepage)
  {
#ifdef EMSCRIPTEN
    url = "load_url.php?url=" + url;
    std::string url3 = url + "&homepage=" + homepage;

    std::cout << "url loading started! " << url << std::endl;

    // if we have already loaded the same url, don't load again
    if (load_url_buffers_async[url]) { return; }
    
    char *buf2 = new char[url3.size()+1];
    std::copy(url3.begin(), url3.end(), buf2);
    buf2[url3.size()]=0;
    
    async_pending_count++;
    emscripten_async_wget_data(buf2, (void*)buf2 , &onload_async_cb, &onerror_async_cb);
#else
    std::string url2 = "load_url.php?url=" + url ;
    if (load_url_buffers_async[url2]) { return; }
    std::vector<unsigned char> buf = load_from_url(url);
    load_url_buffers_async[url2] = new std::vector<unsigned char>(buf);
#endif
  }
std::vector<unsigned char> *ASyncLoader::get_loaded_data(std::string url) const
  {
    url = "load_url.php?url=" + url;
    std::cout << "url fetch " << url << std::endl;
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
  Bitmap<Color> *bm2 = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(255,255,255,0), *bm1);
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
  return bm;
}
extern std::string gameapi_homepageurl;
GameApi::FI GameApi::FontApi::load_font(std::string ttf_filename, int sx, int sy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  void *priv_ = (void*)&env->lib;
  return add_font_interface(e, new FontInterfaceImpl(e, priv_, ttf_filename, gameapi_homepageurl, sx,sy));
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


GameApi::LI GameApi::MovementNode::cmd_to_li(CMD cmds2, std::string commands)
{
  CmdExecute *cmds = find_cmds(e, cmds2);
  return add_line_array(e, new CmdLines(cmds, commands));
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
					       false_value(false_value), true_value(true_value) { }
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
  VoxelToPTS(Voxel<int> *vx, int count, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
  {
    int sx = vx->SizeX();
    int sy = vx->SizeY();
    int sz = vx->SizeZ();
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
	      pts[val].push_back(p);
	    }
	  }
      }
    }
  }
  PointsApiPoints *get(int val);
public:
  std::vector<std::vector<Point> > pts;
};

class VPTS : public PointsApiPoints
{
public:
  VPTS(VoxelToPTS &pts, int val) : pts(pts), val(val) { }
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
};

PointsApiPoints *VoxelToPTS::get(int val)
{
  return new VPTS(*this, val);
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
