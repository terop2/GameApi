       
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
//#define THREADS 1
#endif


#include "GameApi_h.hh"

std::string funccall_to_string(ShaderModule *mod);
std::string color_funccall_to_string(ShaderModule *mod);
std::string funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);
std::string color_funccall_to_string_with_replace(ShaderModule *mod, std::string name, std::string val);

EnvImpl::EnvImpl() : event_infos(new EmptySequencer2), mutex(PTHREAD_MUTEX_INITIALIZER)
{
#ifndef EMSCRIPTEN
    int err = FT_Init_FreeType(&lib);
    std::cout << "Freetype init error: " << err << std::endl;
    std::cout << &lib << std::endl;
#endif 
    cursor_pos_point_id.id = -1;
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
EXPORT void GameApi::Env::free_temp_memory()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_temp_memory();

}

EXPORT GameApi::Env::~Env()
{
  delete (::EnvImpl*)envimpl;
}

SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);

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
  std::cout << "find_widget failed!" << w.id << std::endl;
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

GameApi::BO GameApi::BooleanOps::create_bo(P mesh, O bools, FD fd)
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

GameApi::BO GameApi::BooleanOps::cube(GameApi::EveryApi &ev, 
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
GameApi::BO GameApi::BooleanOps::sphere(GameApi::EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
{
  P mesh = ev.polygon_api.sphere(center, radius, numfaces1, numfaces2);
  O bools = ev.volume_api.sphere(center, radius);
  FD fd = ev.dist_api.sphere(center, radius);
  return create_bo(mesh, bools, fd);
}
GameApi::BO GameApi::BooleanOps::or_elem(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  BO_Impl obj2_i = env->boolean_ops[obj2.id];
  P mesh = ev.polygon_api.or_elem(obj_i.mesh, obj2_i.mesh);
  O bools = ev.volume_api.max_op(obj_i.bools, obj2_i.bools);
  FD fd = ev.dist_api.min(obj_i.fd, obj2_i.fd);
  return create_bo(mesh, bools, fd);
}
GameApi::BO GameApi::BooleanOps::and_not(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
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

GameApi::BO GameApi::BooleanOps::intersect(GameApi::EveryApi &ev, GameApi::BO obj, GameApi::BO obj2)
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

GameApi::P GameApi::BooleanOps::to_polygon(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.mesh;
}
GameApi::O GameApi::BooleanOps::to_volume(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.bools;
}
GameApi::FD GameApi::BooleanOps::to_dist(BO obj)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BO_Impl obj_i = env->boolean_ops[obj.id];
  return obj_i.fd;
}

// Explicit instantiation of array api
namespace GameApi {
#ifndef EMSCRIPTEN
class template ArrayApi<GameApi::BM>;
class template ArrayApi<GameApi::P>;
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
GameApi::MN GameApi::MovementNode::empty()
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

GameApi::CC GameApi::MovementNode::color_start(unsigned int color)
{
  return add_color(e, new ColorStart(color));
}
GameApi::CC GameApi::MovementNode::color_interpolate(CC next, unsigned int color, unsigned int color2, float start_time, float end_time)
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
GameApi::MN GameApi::MovementNode::level(MN next)
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
GameApi::MN GameApi::MovementNode::translate(MN next, 
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

GameApi::MN GameApi::MovementNode::scale(MN next,
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
GameApi::MN GameApi::MovementNode::rotate(MN next, float start_time, float end_time,
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

GameApi::MN GameApi::MovementNode::anim_enable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimEnable(next2, start_time, end_time));
}
GameApi::MN GameApi::MovementNode::anim_disable(MN next, float start_time, float end_time)
{
  Movement *next2 = find_move(e, next);
  return add_move(e, new AnimDisable(next2, start_time, end_time));
}
GameApi::MN GameApi::MovementNode::anim_choose(std::vector<MN> vec, float start_time, float duration)
{
  std::vector<Movement*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_move(e,vec[i]));
    }

  return add_move(e, new AnimChoose(vec2, start_time, duration));
}


GameApi::MN GameApi::MovementNode::compress(MN next, float start_time, float end_time)
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
GameApi::MN GameApi::MovementNode::event_activate(MN next, MN event, float activate_time, float duration)
{
  Movement *nxt = find_move(e, next);
  Movement *eve = find_move(e, event);
  return add_move(e, new EventActivateMovement(nxt,eve,activate_time, duration));
}
GameApi::MN GameApi::MovementNode::trans2(MN next, float dx, float dy, float dz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Translate(dx,dy,dz)));  
}
GameApi::MN GameApi::MovementNode::scale2(MN next, float sx, float sy, float sz)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::Scale(sx,sy,sz)));  
}
GameApi::MN GameApi::MovementNode::rotatex(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::XRotation(angle)));  
}
GameApi::MN GameApi::MovementNode::rotatey(MN next, float angle)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new MatrixMovement(nxt, Matrix::YRotation(angle)));  
}
GameApi::MN GameApi::MovementNode::rotatez(MN next, float angle)
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
GameApi::MN GameApi::MovementNode::time_repeat(MN next, float start_time,
					       float repeat_duration)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new TimeRepeatMovement(nxt, start_time, repeat_duration));
}

GameApi::MN GameApi::MovementNode::change_time(MN next, float d_time)
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
GameApi::M GameApi::MovementNode::get_matrix(MN n, float time, float delta_time)
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
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", res2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", res2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", res2);
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
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
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
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
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
    GameApi::M m2 = add_matrix2(e, env.env);
    GameApi::M mat2 = ev.matrix_api.mult(mat,m2);
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    Matrix old_in_MV = env.in_MV;
    env.in_MV = find_matrix(e, mat2);

    Matrix old_env = env.env;
    float old_time = env.time;
    env.env = find_matrix(e,mat2); /* * env.env*/;
    env.time = env.time + i*time_delta/10.0;
    next->execute(env);
    env.env = old_env;
    env.time = old_time;
    env.in_MV = old_in_MV;
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
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
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
    int ch = eve.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    bool start_anim = false;
    bool start_anim2 = false;
    if (eve.type==0x300 && ch == key && !anim_ongoing && !anim_ongoing2 && !key_pressed) { start_anim = true; } else if (eve.type==0x300 && ch==key && !key_pressed)
      {
	key_canceled=true;
      }
    if (eve.type==0x300 && ch == key) { key_pressed = true; }
    if (eve.type==0x301 && ch == key && !anim_ongoing2 && !key_canceled) { key_pressed = false;  start_anim2 = true; } 
    if (eve.type==0x301 && ch==key) { key_canceled=false; }

    if (start_anim) {
      start_anim_chain = false;
      //std::cout << "start_anim" << std::endl;
      anim_ongoing = true; start_time = ev.mainloop_api.get_time(); anim_pos = 0.0; 
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
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
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
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1, "in_MV", mat2);
	ev.shader_api.use(s2);
	ev.shader_api.set_var(s2, "in_MV", mat2);
	ev.shader_api.use(s3);
	ev.shader_api.set_var(s3, "in_MV", mat2);
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

GameApi::ML GameApi::MovementNode::key_printer_ml(GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyPrinter(item));
}
GameApi::ML GameApi::MovementNode::move_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int clone_count, float time_delta)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new MoveML(e,ev,item, move, clone_count, time_delta));
}
GameApi::ML GameApi::MovementNode::color_ml(EveryApi &ev, int color_num, ML ml, CC cc)
{
  MainLoopItem *item = find_main_loop(e, ml);
  ColorChange *cc2 = find_color(e, cc);
  return add_main_loop(e, new ColorML(e,ev,item, color_num, cc2));
}
GameApi::ML GameApi::MovementNode::key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyActivateML(e,ev,item, move, key, duration));
}
GameApi::ML GameApi::MovementNode::temp_key_activate_ml(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int key, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new TempKeyActivateML(e, ev, item, move, key, duration));
}
GameApi::ML GameApi::MovementNode::move_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,true,false,false,false,false,false,start_x, end_x));
}
GameApi::ML GameApi::MovementNode::jump_ml(EveryApi &ev, GameApi::ML ml, int key_jump, float height, float jump_duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new JumpML(e,ev,item,key_jump, height, jump_duration));
}
GameApi::ML GameApi::MovementNode::move_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0, false,true,false,false,false,false,start_y,end_y));
}
GameApi::ML GameApi::MovementNode::move_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,speed,0.0,false,false,true,false,false,false,start_z,end_z));
}

GameApi::ML GameApi::MovementNode::rot_x_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_x, float end_x)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,true,false,false,start_x, end_x));
}
GameApi::ML GameApi::MovementNode::rot_y_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_y, float end_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,true,false,false,true,false,start_y,end_y));
}
GameApi::ML GameApi::MovementNode::rot_z_ml(EveryApi &ev, GameApi::ML ml, int key_forward, int key_backward, float speed, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyMoveML(e,ev,item,key_forward, key_backward,0.0,speed,false,false,false,false,false,true,start_z,end_z));
}

GameApi::ML GameApi::MovementNode::enable_ml(EveryApi &ev, GameApi::ML ml, float start_time, float end_time)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new EnableML(e,ev,item, start_time, end_time));

}
GameApi::ML GameApi::MovementNode::move_ml_array(EveryApi &ev, std::vector<GameApi::ML> ml, std::vector<GameApi::MN> move)
{
  int s = ml.size();
  std::vector<MainLoopItem*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_main_loop(e, ml[i]));
    }
  return add_main_loop(e, new MoveMLArray(e,ev, vec2, move));
}
GameApi::ML GameApi::MovementNode::key_event(EveryApi &ev, GameApi::ML ml, GameApi::MN move, int type, int ch, int button, float duration)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new KeyEventML(e,ev,item, move,type,ch,button, duration));
}

GameApi::ML GameApi::MovementNode::wasd(EveryApi &ev, GameApi::ML ml,
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
	  ev.shader_api.use(s1);
	  ev.shader_api.set_var(s1, "in_MV", mat2);
	  ev.shader_api.use(s2);
	  ev.shader_api.set_var(s2, "in_MV", mat2);
	  ev.shader_api.use(s3);
	  ev.shader_api.set_var(s3, "in_MV", mat2);
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
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
};
class DefaultMaterial : public MaterialForward
{
public:
  DefaultMaterial(GameApi::EveryApi &ev) : ev(ev) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //std::cout << "mat2_inst" << std::endl;
    GameApi::PTA pta = ev.points_api.prepare(pts);
    GameApi::VA va = ev.polygon_api.create_vertex_array(p,false);
    GameApi::ML ml = ev.materials_api.render_instanced2_ml(ev, va, pta);
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
private:
  GameApi::EveryApi &ev;
};


class TextureArrayMaterial : public MaterialForward
{
public:
  TextureArrayMaterial(GameApi::EveryApi &ev, std::vector<GameApi::BM> vec, int sx, int sy) : ev(ev), vec(vec),sx(sx),sy(sy) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P I10=p; 
    GameApi::P I11=ev.polygon_api.texcoord_manual(I10,0,0,1,0,1,1,0,1);
    GameApi::VA I12=ev.polygon_api.create_vertex_array(I11,true);
    //GameApi::BM I13=bm;
    GameApi::TXA I14 = ev.texture_api.prepare_arr(ev, vec, sx,sy);
    GameApi::VA I16 = ev.texture_api.bind_arr(I12, I14);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml(ev,I16);
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17);
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
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17);
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
    GameApi::ML I18=ev.polygon_api.texture_arr_shader(ev, I17);
    return I18;
  }


private:
  GameApi::EveryApi &ev;
  std::vector<GameApi::BM> vec;
  int sx,sy;
};

class TextureMaterial : public MaterialForward
{
public:
  TextureMaterial(GameApi::EveryApi &ev, GameApi::BM bm) : ev(ev), bm(bm) { }
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
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17);
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
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17);
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
    GameApi::ML I18=ev.polygon_api.texture_shader(ev, I17);
    return I18;
  }
private:
  GameApi::EveryApi &ev;
  GameApi::BM bm;
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
private:
  GameApi::EveryApi &ev;
  Material *next;
  int count;
  bool web;
};

class SnowMaterial : public MaterialForward
{
public:
  SnowMaterial(GameApi::EveryApi &ev, Material *next) : ev(ev), next(next) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,0.5);
    GameApi::ML ml;
    ml.id = next->mat(p3.id);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, 0xffaaaaaa, 0xffeeeeee, 0xffffffff);
    return sh;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,0.5);
    GameApi::ML ml;
    ml.id = next->mat_inst(p3.id, pts.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, 0xffaaaaaa, 0xffeeeeee, 0xffffffff);
    return sh;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::P p0 = ev.polygon_api.recalculate_normals(p);
    GameApi::P p1 = ev.polygon_api.color_from_normals(p0);
    GameApi::P p2 = ev.polygon_api.color_grayscale(p1);
    GameApi::P p3 = ev.polygon_api.mix_color(p1,p2,0.5);
    GameApi::ML ml;
    ml.id = next->mat_inst2(p3.id, pta.id);
    //VA va = ev.polygon_api.create_vertex_array(p3,false);
    //ML ml = ev.polygon_api.render_vertex_array_ml(ev, va);
    GameApi::ML sh = ev.polygon_api.shading_shader(ev, ml, 0xffaaaaaa, 0xffeeeeee, 0xffffffff);
    return sh;

  }
private:
  GameApi::EveryApi &ev;
  Material *next;
};
GameApi::MT GameApi::MaterialsApi::def(EveryApi &ev)
{
  return add_material(e, new DefaultMaterial(ev));
}
GameApi::MT GameApi::MaterialsApi::skeletal(EveryApi &ev)
{
  return add_material(e, new SkeletalMaterial(ev));
}
GameApi::MT GameApi::MaterialsApi::texture(EveryApi &ev, BM bm)
{
  return add_material(e, new TextureMaterial(ev, bm));
}
GameApi::MT GameApi::MaterialsApi::texture_arr(EveryApi &ev, std::vector<BM> vec, int sx, int sy)
{
  return add_material(e, new TextureArrayMaterial(ev, vec, sx,sy));
}
GameApi::MT GameApi::MaterialsApi::snow(EveryApi &ev, MT nxt)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new SnowMaterial(ev, mat));
}
GameApi::MT GameApi::MaterialsApi::brashmetal(EveryApi &ev, MT nxt, int count, bool web)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new BrashMetal(ev, mat, count, web));
}

#if 0
GameApi::ML GameApi::MaterialsApi::snow(EveryApi &ev, P p)
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


private:
  GameApi::EveryApi &ev;
  Material *next;
};

GameApi::MT GameApi::MaterialsApi::web(EveryApi &ev, MT nxt)
{
  Material *mat = find_material(e, nxt);
  return add_material(e, new WebMaterial(ev,mat));
}


#if 0
GameApi::ML GameApi::MaterialsApi::web(EveryApi &ev, P p)
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

GameApi::ML GameApi::MaterialsApi::bind(P p, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat(p.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
GameApi::ML GameApi::MaterialsApi::bind_inst2(P p, PTA pta, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat_inst2(p.id,pta.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
GameApi::ML GameApi::MaterialsApi::bind_inst(P p, PTS pts, MT mat)
{
  Material *mat2 = find_material(e, mat);
  int val = mat2->mat_inst(p.id,pts.id);
  GameApi::ML ml;
  ml.id = val;
  return ml;
}
class RenderInstanced : public MainLoopItem
{
public:
  RenderInstanced(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::PTS pts) : env(e), ev(ev), p(p), pts(pts) { firsttime = true; shader.id=-1; }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    // MainLoopEnv ee = e;
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	va = ev.polygon_api.create_vertex_array(p,false);
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
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    if (firsttime) {
      firsttime = false;
      //std::cout << "RenderInstanced::PREPARE" << std::endl;
      ev.polygon_api.prepare_vertex_array_instanced(ev.shader_api, va, pta, sh);
    }

    ev.shader_api.set_var(sh, "in_POS", 0.0f);
    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh);
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
};

class RenderInstanced2 : public MainLoopItem
{
public:
  RenderInstanced2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::VA va, GameApi::PTA pta) : env(e), ev(ev), va(va), pta(pta) { firsttime = true; shader.id=-1; }
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

    ev.polygon_api.render_vertex_array_instanced(ev.shader_api, va, pta, sh);
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
};

GameApi::ML GameApi::MaterialsApi::render_instanced_ml(GameApi::EveryApi &ev, P p, PTS pts)
{
  return add_main_loop(e, new RenderInstanced(e, ev, p,pts));
}
GameApi::ML GameApi::MaterialsApi::render_instanced2_ml(GameApi::EveryApi &ev, VA va, PTA pta)
{
  return add_main_loop(e, new RenderInstanced2(e, ev, va,pta));
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
  return add_uber(e, new F_ShaderCallFunction("specular", next,"EX_NORMAL2 EX_LIGHTPOS2"));
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
  return add_uber(e, new F_ShaderCallFunction("texture", next,"EX_TEXCOORD"));
}

GameApi::US GameApi::UberShaderApi::f_texture_arr(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("texture_arr", next,"EX_TEXCOORD TEXTURE_ARRAY"));
}

GameApi::US GameApi::UberShaderApi::f_colour(US us)
{
  ShaderCall *next = find_uber(e, us);
  return add_uber(e, new F_ShaderCallFunction("colour", next,"EX_COLOR"));
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

GameApi::C GameApi::CurveApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  return add_curve(e, new LineCurve(*pp1, *pp2));
}
GameApi::C GameApi::CurveApi::circle_xy(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXY(*c, r));
}
GameApi::C GameApi::CurveApi::circle_xz(PT center, float r)
{
  Point *c = find_point(e, center);
  return add_curve(e, new CircleCurveXZ(*c, r));
}
GameApi::C GameApi::CurveApi::linear(std::vector<PT> vec)
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
GameApi::C GameApi::CurveApi::bezier(std::vector<PT> vec)
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
GameApi::C GameApi::CurveApi::scale(C curve, float mx, float my, float mz)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new MatrixCurve3(c, Matrix::Scale(mx, my, mz)));
}

GameApi::C GameApi::CurveApi::trans(C curve, float dx, float dy, float dz)
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

GameApi::C GameApi::CurveApi::compose(std::vector<C> vec)
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
GameApi::C GameApi::CurveApi::change_length(C curve, float new_length)
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

GameApi::C GameApi::CurveApi::split(C curve, float start_var, float end_var)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_curve(e, new SplitCurve(c, start_var, end_var));
}

GameApi::PT GameApi::CurveApi::pos(C curve, float p)
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
GameApi::PTS GameApi::CurveApi::sample(C curve, int num_samples)
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
GameApi::LI GameApi::CurveApi::to_lines(C curve, int num_lines)
{
  Curve<Point> *c = find_curve(e, curve);
  return add_line_array(e, new CurveLineCollection(c, num_lines));
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

GameApi::MS GameApi::MatricesApi::from_points(PTS pts)
{
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_matrix_array(e, new FromPointsMatrices(points));
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

GameApi::MS GameApi::MatricesApi::mult(MS m, M mat)
{
  MatrixArray *arr = find_matrix_array(e, m);
  Matrix mm = find_matrix(e, mat);
  return add_matrix_array(e, new MultArray1(arr,mm));
}
GameApi::MS GameApi::MatricesApi::mult(M mat, MS m)
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
GameApi::MS GameApi::MatricesApi::subarray(MS m, int start, int count)
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
GameApi::MC GameApi::MatrixCurveApi::from_curve(C curve)
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
GameApi::MC GameApi::MatrixCurveApi::circle_xy(float radius)
{
  return add_matrix_curve(e, new CircleXYMatrix(radius));
}
GameApi::MC GameApi::MatrixCurveApi::circle_xz(float radius)
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

GameApi::MS GameApi::MatrixCurveApi::sample(MC m_curve, int num)
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
GameApi::PD GameApi::PolygonDistanceField::empty(EveryApi &ev)
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
GameApi::PD GameApi::PolygonDistanceField::create_pd(P mesh, SFO distance_field)
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
GameApi::PD GameApi::PolygonDistanceField::cube(EveryApi &ev, float start_x, float end_x,
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

GameApi::PD GameApi::PolygonDistanceField::rounded_cube(EveryApi &ev,
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

GameApi::PD GameApi::PolygonDistanceField::sphere(EveryApi &ev, PT center, float radius, int numfaces1, int numfaces2)
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

GameApi::PD GameApi::PolygonDistanceField::or_array(EveryApi &ev, std::vector<PD> vec)
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
GameApi::PD GameApi::PolygonDistanceField::color_from_normal(EveryApi &ev, PD obj)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.color_from_normal(sfo);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
GameApi::PD GameApi::PolygonDistanceField::ambient_occulsion_sfo(EveryApi &ev,
						       PD obj, float d, float i)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.ambient_occulsion(sfo, d,i);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);
}
GameApi::PD GameApi::PolygonDistanceField::colormod_from_position(EveryApi &ev, PD obj, float px, float py, float pz, float sx, float sy, float sz)
{
  SFO sfo = get_distance_field(obj);
  SFO sfo2 = ev.sh_api.colormod_from_position(sfo, px,py,pz,sx,sy,sz);
  PD_Impl impl = find_polydistfield(e,obj);
  return create_pd(impl.mesh,sfo2);

}
GameApi::SFO GameApi::PolygonDistanceField::get_distance_field(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.distance_field;
}
GameApi::P GameApi::PolygonDistanceField::get_polygon(PD pd)
{
  PD_Impl impl = find_polydistfield(e,pd);
  return impl.mesh;
}

GameApi::MT GameApi::PolygonDistanceField::mesh_color_from_sfo(EveryApi &ev, PD orig, MT next)
{
  PD_Impl impl = find_polydistfield(e,orig);
  return ev.materials_api.mesh_color_from_sfo(ev, impl.distance_field, next);
}
GameApi::ML GameApi::PolygonDistanceField::render_scene(EveryApi &ev, PD pd, PD world)
{
  // TODO
  //P p = get_poly(pd);
  //SFO sfo = get_distance_field(world);
  GameApi::ML ml;
  ml.id = 0;
  return ml;
}

std::vector<GameApi::ML> GameApi::PolygonDistanceField::render_scene_array(EveryApi &ev, std::vector<PD> vec)
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
  GameApi::SH color_sh;
  GameApi::SH texture_sh;
  GameApi::SH arr_texture_sh;
  bool exit = false;
};


void blocker_iter(void *arg)
{
  Envi_2 *env = (Envi_2*)arg;
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; }
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
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->color_sh);

	GameApi::M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
	GameApi::M in_T = env->ev->mainloop_api.in_MV(*env->ev, true);
	GameApi::M in_N = env->ev->mainloop_api.in_MV(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N);

    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

}

class MainLoopBlocker_win32_and_emscripten : public Blocker
{
public:
  MainLoopBlocker_win32_and_emscripten(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML code) : e(e), ev(ev), code(code) 
  {
  }
  void Execute()
  {
    
    Envi_2 env;
    

    GameApi::SH sh = ev.shader_api.colour_shader();
    GameApi::SH sh2 = ev.shader_api.texture_shader();
    GameApi::SH sh3 = ev.shader_api.texture_array_shader();
    
    // rest of the initializations
    ev.mainloop_api.init_3d(sh);
    ev.mainloop_api.init_3d(sh2);
    ev.mainloop_api.init_3d(sh3);
    ev.shader_api.use(sh);
    
    GameApi::ML ml = mainloop(ev);
    GameApi::MN mn0 = ev.move_api.empty();
    GameApi::MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, -400.0);
    GameApi::ML ml2 = ev.move_api.move_ml(ev, ml, mn);
    env.mainloop = ml2;
    
    env.ev = &ev;
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    ev.mainloop_api.reset_time();
    if (env.logo_shown)
      ev.mainloop_api.display_logo(ev);
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
};

GameApi::BLK GameApi::BlockerApi::game_window(GameApi::EveryApi &ev, ML ml)
{
  Blocker *blk = new MainLoopBlocker_win32_and_emscripten(e,ev,ml);
  return add_blocker(e, blk);
}
void GameApi::BlockerApi::run(BLK blk)
{
  Blocker *blk2 = find_blocker(e, blk);
  blk2->Execute();
}
