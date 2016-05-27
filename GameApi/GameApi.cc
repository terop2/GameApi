       
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
//#define THREADS 1
#endif


#include "GameApi_h.hh"


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

EXPORT GameApi::Env::Env()
{
  envimpl = (void*)new ::EnvImpl;
}
EXPORT void GameApi::Env::free_memory()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->free_memory();
}

EXPORT GameApi::Env::~Env()
{
  delete (::EnvImpl*)envimpl;
}

SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);

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
class template ArrayApi<GameApi::BM>;
class template ArrayApi<GameApi::P>;
};
