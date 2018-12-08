#include <memory>
#include "../GameApi_low.hh"
#include "../Shader.hh"


struct FaceCollPolyHandle
{
  FaceCollPolyHandle() : coll(0), collarray(0), collowned(false), collarrayowned(false) { }
  int id;
  FaceCollection *coll; 
  //FaceCollection **collarray;
  Array<int, FaceCollection*> *collarray;
  bool collowned;
  bool collarrayowned;
  //int size;
  bool padding1;
  bool padding2;
  ~FaceCollPolyHandle() { if (collowned) delete coll; if (collarrayowned) delete collarray; }

};

struct BitmapHandle
{
  int id;
  virtual ~BitmapHandle() { }
};


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



class BooleanOps
{
public:
  virtual ~BooleanOps() { }
  virtual bool Inside(Point p) const { return false; }
};
class BoxableFaceCollection : public FaceCollection, public BooleanOps
{
};


class SingleForwardFaceCollection : public FaceCollection
{
public:
  void Prepare() { }
};
class SingleForwardBoxableFaceCollection : public BoxableFaceCollection
{
public:
  void Prepare() { }
};
class MousePlane;
template<class T, class K> class Array;
struct StateInfo2
{
  Array<int, bool> *enable_obj_array; // dimension = numpositions, int=LL.id
  MousePlane *plane;
};
struct StateRange
{
  int start_range;
  int range_size;
};

struct BoolBitmap
{
  Bitmap<bool> *bitmap;
};
struct FloatBitmap
{
  Bitmap<float> *bitmap;
};

struct SpaceImpl
{
  Point tl, tr, bl, br, z;
  Point origo;
  Point2d conv(Point p) { Point2d pp; pp.x = p.x; pp.y = p.y; return pp; }
};

struct LineImpl
{
  Point2d p1,p2;
  float duration;
};
struct SpritePosImpl
{
  float x,y;
};
struct TexCoordQuad
{
  Point2d p1;
  Point2d p2;
};

struct FBOPriv
{
  Low_GLuint fbo_name;
  Low_GLuint texture;
  Low_GLuint depthbuffer;
  int sx,sy;
};

class AnimInt;
class AnimPoint3d;
class AnimFloat;
class VolumeObject;
class FloatVolumeObject;
class BitmapHandle;
class FaceCollPolyHandle;
class VertexArraySet;
class RenderVertexArray;
class TROArray;
class VArray;
class ShaderPriv2;
class TextureI;
class Separate;
class PlanePoints2d;
class Waveform;
class ArrayRender;
class Layout;
class PointArray2;
class PointArray3;
class PointCollection;
class ColorVolumeObject;
class DistanceRenderable;
class VectorVolumeObject;
template<class T> class LazyValue;
class FontAtlasInfo;
class Sequencer2;
class SurfaceIn3d;
class MousePlane;
class BitmapArrayHandle;
class IDInterface
{
public:
  virtual int Size() const=0;
  virtual int Poly_id(int count) const=0;
  virtual int facenum(int count) const=0;
  virtual int pointnum(int count) const=0;
  virtual int type(int count) const=0; // 0=point, 1=normal, 2=color, 3=texcoord
};


struct IDImpl
{
  int id_num;
  IDInterface *interface;
  //int poly_id;
  //int facenum;
  //int pointnum;
  //int type; 
};

struct Font
{
  Bitmap<int> *bm;
};

struct ObjectWithPos
{
  GameApi::P obj;
  GameApi::M m;
};
struct VertexArrayWithPos
{
  GameApi::VA va;
  GameApi::M m;
};

struct PointArray2
{
  float *array; // 3*numpoints items
  float *array2_1;
  unsigned int *color_array;
  int numpoints;
  Low_GLuint buffer;
  Low_GLuint buffer2_1;
  Low_GLuint buffer2;
  Low_GLuint vao[1];
  Low_GLuint pos_buffer;
};

struct PointArray3
{
  float *array;
  unsigned int *color;
  int numpoints;
  Low_GLuint buffer[2];
  Low_GLuint vao[1];
};

struct PlaneData
{
  std::vector<float> array;
  std::vector<unsigned char> cmd_array;
};

struct TBuffer2
{
  int type;
  int duration;
  int sample;
};

struct TBuffer
{
  int numchannels;
  int numslots;
  TBuffer2 *buf; // 2d buffer
};

struct FontAtlasGlyphInfo
{
  int sx, sy;
  int x,y;
  int top;
};
struct FontAtlasInfo
{
  std::map<int, FontAtlasGlyphInfo> char_map;
  int atlas_sx;
  int atlas_sy;
};


struct BO_Impl
{
  GameApi::P mesh;
  GameApi::O bools;
  GameApi::FD fd;
};
struct PD_Impl
{
  GameApi::P mesh;
  GameApi::SFO distance_field;
};
struct Pa_Impl
{
  std::vector<GameApi::P> vec;
};
struct Va_Impl
{
  std::vector<GameApi::VA> vec;
};


struct AnimImpl
{
  AnimInt *wave_int;
  AnimPoint3d *wave_point;
  AnimFloat *wave_float;
};
struct FunctionImpl
{
  Function<float,float> *func;
  float size;
};

struct SurfaceImpl
{
  SurfaceIn3d *surf;
};


struct EnvImpl
{
  std::vector<Point> pt;
  //std::vector<MatrixInterface*> matrix;
  GameApi::PT cursor_pos_point_id;
  std::vector<Vector> vectors;
  std::vector<Color> colors;
  std::vector<VolumeObject*> volumes;
  std::vector<FloatVolumeObject*> floatvolumes;
  std::vector<BitmapHandle*> bm;
  std::vector<BitmapArray2<Color> *> bm_array;
  std::vector<BoolBitmap> bool_bm;
  std::vector<FloatBitmap> float_bm;
  std::vector<SpaceImpl> sp;
  std::vector<LineImpl> ln;
  std::vector<SpritePosImpl> spr_pos;
  std::vector<AnimImpl> anim;
  std::vector<FaceCollPolyHandle*> poly;
  std::vector<std::shared_ptr<void> > deletes;
  std::vector<FunctionImpl> func;
  std::vector<SurfaceImpl> surfaces;
  //std::vector<VBOObjects*> vbos;
  std::vector<IDImpl> ids;
  std::vector<NDim<float,Point>*> dims;
  std::vector<StateInfo2> states;
  std::vector<StateRange> state_ranges; // ST type points to this array
  std::vector<ContinuousBitmap<Color>*> continuous_bitmaps;
  std::vector<VertexArraySet*> vertex_array;
  std::vector<RenderVertexArray*> vertex_array_render;
  std::vector<int> texture_id;
  std::vector<std::string> shader_float_parameter;
  std::vector<std::string> shader_int_parameter;
  
  std::vector<Voxel<unsigned int>*> voxels;
  std::vector<Voxel<int>*> voxels2;
  std::vector<TROArray*> timeranges; // statechange time ranges
  std::vector<VArray*> timerange_vertexarrays; // statechange vertex arrays
  std::vector<ShaderPriv2*> shaders;
  std::vector<TextureI*> textures;
  std::vector<TexCoordQuad> tex_quads;
  std::vector<Separate*> separates;
  std::vector<PlanePoints2d*> plane_points;
  std::vector<PlaneData> plane_array;
  std::vector<Waveform*> waveforms;
  std::vector<Array<int, ObjectWithPos> * > object_move;
  std::vector<std::vector<VertexArrayWithPos> *> object_move_vertex_array;
  std::vector<Bitmap<float>*> layout_data;
  std::map<int, ArrayRender*> renders; // BM.id -> arrayrender
  std::map<int, ArrayRender*> renders2; // BM.id -> arrayrender
  std::vector<Layout*> layouts;
  std::vector<PointArray2*> pointarray;
  std::vector<PointArray3*> pointarray3;
  std::vector<PointCollection*> pointcollarray;
  std::vector<LineCollection*> linearray;
  std::vector<ColorVolumeObject*> colorvolume;
  std::map<int, ShaderPriv2*> shader_privs;
  std::vector<DistanceRenderable*> distvolume;
  std::vector<VectorVolumeObject*> vectorvolume;
  std::vector<PointsApiPoints*> pointsapi_points;
  std::vector<LazyValue<float>*> floats;
  std::vector<Array<int,float>*> float_array;
  std::vector<FBOPriv> fbos;
  std::vector<Samples*> samples;
  std::vector<Tracker*> trackers;
  std::vector<TBuffer> tracker_buf;
  std::vector<Wavs> wavs;
  std::vector<ShaderModule*> shader_module;
  std::vector<GuiWidget*> widgets;
  std::vector<GameApiModule *> gameapi_modules;
  std::vector<FontAtlasInfo*> font_atlas;
  std::vector<MainLoopItem*> main_loop;
  std::vector<std::vector<int>*> arrays;
  std::vector<ExprNode*> exprs;
  std::vector<PhysicsNode*> phys;
  std::vector<TriStrip*> tri_strip;
  std::vector<Cutter*> cutters;
  std::vector<BO_Impl> boolean_ops;
  std::vector<PD_Impl> polydistfield;
  std::vector<std::vector<int> > handle_array;
  std::vector<std::vector<int> > template_array;
  std::vector<Collision*> collision_array;
  std::vector<Movement*> movement_array;
  std::vector<ImplicitFunction3d*> implicit;
  std::vector<TreeLevel*> tree_levels;
  std::vector<TreeStack*> trees;
  std::vector<Material*> materials;
  std::vector<ShaderCall*> shadercalls;
  std::vector<Curve<Point>*> curves;
  std::vector<MatrixArray*> matrix_arrays;
  std::vector<Curve<Matrix>*> matrix_curves;
  std::vector<PlaneShape*> plane_shapes;
  std::vector<SkeletalNode*> skeletals;
  std::vector<std::shared_ptr<void> > temp_deletes;
  std::vector<Pa_Impl> polygon_array;
  std::vector<Va_Impl> va_array;
  std::vector<MixedI*> mixed;
  std::vector<ColorChange*> color_change;
  std::vector<Blocker*> blockers;
  std::vector<Splitter*> splitters;
  std::vector<VertexAnimNode*> vertex_anims;
  std::vector<PointTransform*> point_transforms;
  std::vector<CurvePos*> curve_pos;
  std::vector<ArrayType*> arrays2;
  std::vector<Fetcher<int>*> int_fetchers;
  std::vector<Fetcher<std::string>*> string_fetchers;
  std::vector<Fetcher<float>*> float_fetchers;
  std::vector<Fetcher<Point>*> point_fetchers;
  ASyncLoader *async_loader;
  std::vector<GlyphInterface*> glyph_interfaces;
  std::vector<FontInterface*> font_interfaces;
  std::vector<StringDisplay*> string_displays;
  std::vector<CmdExecute*> cmds;
  std::vector<CurvePatch*> curve_patches;
  std::vector<PropertyArray*> prop_array;
  std::vector<Space3d*> space_3d;
  std::vector<LinesInPlane*> plane_lines;
  std::vector<PointsInPlane*> plane_points2;
  std::vector<FacesInPlane*> plane_faces;
  std::map<int, TextureID*> txids;
  std::vector<ShaderI*> shader_interface;
  std::vector<ContinuousBitmap<float>*> cont_floats;
  std::vector<ContinuousBitmap<bool>*> cont_bools;
  std::vector<DynamicChange*> dyn_change;
  std::vector<std::vector<float> *> polynomials;
  std::vector<ShaderBitmap*> shaderbitmaps;
  std::vector<DiskStore*> disk_store;
  std::vector<Bitmap<int>*> int_bitmaps;
  std::vector<InputForMoving*> move_input;
  std::vector<CurveGroup*> curve_group;
  std::vector<MeshAnim*> mesh_anim;
  std::vector<FrameBufferLoop*> frame_loop;
  std::vector<FrameBuffer*> frame_buffer;
  std::vector<HeavyOperation*> heavys;
  //std::vector<EventInfo> event_infos;
  Sequencer2 *event_infos; // owned, one level only.
  pthread_mutex_t mutex;
  void lock() { pthread_mutex_lock(&mutex); }
  void unlock() { pthread_mutex_unlock(&mutex); }
#ifdef HAS_FREETYPE
  FT_Library lib;
#endif
  std::vector<Font> fonts;
  static ::EnvImpl *Environment(GameApi::Env *e) { return (EnvImpl*)e->envimpl; }
  EXPORT void free_temp_memory()
  {
    temp_deletes.resize(0);
    temp_deletes.shrink_to_fit();
  }
  EXPORT std::vector<int> store_counts() const
  {
    std::vector<int> vec;
    vec.push_back(vertex_array_render.size());
    vec.push_back(vertex_array.size());
    vec.push_back(renders.size());
    vec.push_back(renders2.size());
    return vec;
  }
  EXPORT void free_to_counts(std::vector<int> vec)
  {
      int sk2 = vertex_array_render.size();
      int start0=vec[0];
   for(int ii2=start0;ii2<sk2;ii2++)
    {
      RenderVertexArray *s = vertex_array_render[ii2];
      delete s;
    }
   int start1 = vec[1];
  int sk1 = vertex_array.size();
  for(int ii1=start1;ii1<sk1;ii1++)
    {
      VertexArraySet *s = vertex_array[ii1];
      delete s;
    }

  int start2 = vec[2];
  std::map<int, ArrayRender*>::iterator it = renders.begin();
  std::advance(it,start2);
  for(;it!=renders.end();it++)
    {
      ArrayRender *rend = (*it).second;
      delete rend;
    }
  int start3 = vec[3];
  std::map<int, ArrayRender*>::iterator it2 = renders2.begin();
  std::advance(it,start3);
  for(;it2!=renders2.end();it2++)
    {
      ArrayRender *rend = (*it2).second;
      delete rend;
    }
  
  }
  EXPORT void free_memory()
  {
    deletes.resize(0);
    deletes.shrink_to_fit();
  int sk6 = textures.size();
  for(int ii6=0;ii6<sk6;ii6++)
    {
      TextureI *ptr = textures[ii6];
      delete ptr;
      textures[ii6]=0;
    }
  textures.resize(0);
  textures.shrink_to_fit();
  int sk5 = pointsapi_points.size();
  for(int ii5=0;ii5<sk5;ii5++)
    {
      PointsApiPoints *p = pointsapi_points[ii5];
      delete p;
      pointsapi_points[ii5] = 0;
    }
  pointsapi_points.resize(0);
  pointsapi_points.shrink_to_fit();
  int sk4 = plane_points.size();
  for(int ii4=0;ii4<sk4;ii4++)
    {
      PlanePoints2d *p = plane_points[ii4];
      delete p;
      plane_points[ii4]=0;
    }
  plane_points.resize(0);
  plane_points.shrink_to_fit();
  int sk3 = continuous_bitmaps.size();
  for(int ii3=0;ii3<sk3;ii3++)
    {
      ContinuousBitmap<Color> *p = continuous_bitmaps[ii3];
      delete p;
      continuous_bitmaps[ii3]=0;
    }
  continuous_bitmaps.resize(0);
  continuous_bitmaps.shrink_to_fit();
#if 0
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
#endif
#if 1
  int vv1 = volumes.size();
  for(int i_v=0;i_v<vv1;i_v++)
    {
      delete volumes[i_v];
      volumes[i_v] =0;
    }
  volumes.resize(0);
  volumes.shrink_to_fit();
#endif
  int vv2 = floatvolumes.size();
  for(int i_vv2=0;i_vv2<vv2;i_vv2++)
    {
      delete floatvolumes[i_vv2];
      floatvolumes[i_vv2]=0;
    }
  floatvolumes.resize(0);
  floatvolumes.shrink_to_fit();
#if 0
  int vv3 = pointarray.size();
  for(int i_vv3=0;i_vv3<vv3;i_vv3++)
    {
      delete [] pointarray[i_vv3]->array;
      delete [] pointarray[i_vv3]->color_array;
      delete pointarray[i_vv3];
      pointarray[i_vv3]=0;
    }
  pointarray.resize(0);
  pointarray.shrink_to_fit();
#endif
#if 0
  int vv3a = pointarray3.size();
  for(int i_vv3a=0;i_vv3a<vv3a;i_vv3a++)
    { 
      delete [] pointarray3[i_vv3a]->array;
      delete [] pointarray3[i_vv3a]->color;
      delete pointarray3[i_vv3a];
      pointarray3[i_vv3a]=0;
    }
  pointarray3.resize(0);
  pointarray3.shrink_to_fit();
#endif
  int vv4 = linearray.size();
  for(int i_vv4=0;i_vv4<vv4;i_vv4++)
    {
      delete linearray[i_vv4];
      linearray[i_vv4]=0;
    }
  linearray.resize(0);
  linearray.shrink_to_fit();
  int vv5 = pointcollarray.size();
  for(int i_vv5=0;i_vv5<vv5;i_vv5++)
    {
      delete pointcollarray[i_vv5];
      pointcollarray[i_vv5]=0;
    }
  pointcollarray.resize(0);
  pointcollarray.shrink_to_fit();
  int ss1 = bool_bm.size();
  for(int i_1=0;i_1<ss1;i_1++)
    {
      BoolBitmap &bm = bool_bm[i_1];
      delete bm.bitmap;
      bm.bitmap=0;
    }
  bool_bm.resize(0);
  bool_bm.shrink_to_fit();
  int ss1a = waveforms.size();
  for(int i_1a=0;i_1a<ss1a;i_1a++)
    {
      Waveform *bm = waveforms[i_1a];
      delete bm;
      waveforms[i_1a]=0;
    }
  waveforms.resize(0);
  waveforms.shrink_to_fit();
  int ss2 = float_bm.size();
  for(int i_2=0;i_2<ss2;i_2++)
    {
      FloatBitmap &bm = float_bm[i_2];
      delete bm.bitmap;
      bm.bitmap=0;
    }
  float_bm.resize(0);
  float_bm.shrink_to_fit();
  int s0 = fonts.size();
  for(int i0=0;i0<s0;i0++)
    {
      Font f = fonts[i0];
      delete f.bm;
      fonts[i0].bm=0;
    }
  fonts.resize(0);
  fonts.shrink_to_fit();
  int s1 = bm.size();
  for(int i1=0;i1<s1;i1++)
    {
      BitmapHandle *handle = bm[i1];
      //std::cout << "EnvImpl destructor: " << handle << std::endl;
      delete handle;
      bm[i1]=0;
    }
  bm.resize(0);
  bm.shrink_to_fit();
  int s2 = anim.size();
  for(int i2=0;i2<s2;i2++)
    {
      AnimImpl *impl = &anim[i2];
      delete impl->wave_int;
      delete impl->wave_point;
      delete impl->wave_float;
    }
  anim.resize(0);
  anim.shrink_to_fit();
  int s3 = poly.size();
  for(int i3=0;i3<s3;i3++)
    {
      FaceCollPolyHandle *handle = poly[i3];
      delete handle;
      poly[i3]=0;
    }
  poly.resize(0);
  poly.shrink_to_fit();
  int s4 = func.size();
  for(int i4=0;i4<s4;i4++)
    {
      FunctionImpl f = func[i4];
      delete f.func;
      func[i4].func=0;
    }
  func.resize(0);
  func.shrink_to_fit();
#if 0
  int s5 = surfaces.size();
  for(int i5=0;i5<s5;i5++)
    {
      SurfaceImpl s = surfaces[i5];
      delete s.surf;
      surfaces[i5].surf=0;
    }  
  surfaces.resize(0);
  surfaces.shrink_to_fit();
#endif
  delete event_infos;
  event_infos = 0;

  }
  EnvImpl();
  ~EnvImpl();
};
struct PolyHandle
{
  int id;
  virtual ~PolyHandle() { }
};
//struct FaceCollPolyHandle : public PolyHandle
//{
//  FaceCollection *coll; 
//  bool collowned;
//  //FaceCollection **collarray;
//  Array<int, FaceCollection*> *collarray;
//  bool collarrayowned;
  //int size;
  
//  ~FaceCollPolyHandle() { if (collowned) delete coll; if (collarrayowned) delete [] collarray; }

//};


FaceCollPolyHandle *find_poly(GameApi::Env &e, GameApi::P p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FaceCollPolyHandle *handle = 0;

  if (p.id >=0 && p.id < (int)ee->poly.size())
    handle = ee->poly[p.id];
  //std::cout << "find_poly:" << handle << std::endl;
  return handle;
}

namespace GameApi {

struct EveryApi
{
	EveryApi(Env &e)
	  : mainloop_api(e), point_api(e), vector_api(e), matrix_api(e), sprite_api(e), grid_api(e), bitmap_api(e), polygon_api(e), bool_bitmap_api(e), float_bitmap_api(e), cont_bitmap_api(e),
	    font_api(e), anim_api(e), event_api(e), /*curve_api(e),*/ function_api(e), volume_api(e), float_volume_api(e), color_volume_api(e), dist_api(e), vector_volume_api(e), shader_api(e), /*state_change_api(e, shader_api),*/ texture_api(e), separate_api(e), waveform_api(e),  color_api(e), lines_api(e), plane_api(e), points_api(e), voxel_api(e), fbo_api(e), sample_api(e), tracker_api(e), sh_api(e), mod_api(e), physics_api(e), ts_api(e), cutter_api(e), bool_api(e), collision_api(e), move_api(e), implicit_api(e), picking_api(e), tree_api(e), materials_api(e), uber_api(e), curve_api(e), matrices_api(e), skeletal_api(e), polygon_arr_api(e),polygon_dist_api(e), blocker_api(e), vertex_anim_api(e), newplane_api(e), surface_api(e), low_frame_api(e),

	    env(e)
  { }
  Env &get_env() { return env; }
  
  MainLoopApi mainloop_api;
  PointApi point_api;
  VectorApi vector_api;
  MatrixApi matrix_api;
  SpriteApi sprite_api;
  GridApi grid_api;
  BitmapApi bitmap_api;
  PolygonApi polygon_api;
  BoolBitmapApi bool_bitmap_api;
  FloatBitmapApi float_bitmap_api;
  ContinuousBitmapApi cont_bitmap_api;
  FontApi font_api;
  AnimApi anim_api;
  EventApi event_api;
  //CurveApi curve_api;
  FunctionApi function_api;
  VolumeApi volume_api;
  FloatVolumeApi float_volume_api;
  ColorVolumeApi color_volume_api;
  DistanceFloatVolumeApi dist_api;
  VectorVolumeApi vector_volume_api;
  ShaderApi shader_api;
  //StateChangeApi state_change_api;
  TextureApi texture_api;
  SeparateApi separate_api;
  WaveformApi waveform_api;
  ColorApi color_api;
  LinesApi lines_api;
  PlaneApi plane_api;
  PointsApi points_api;
  VoxelApi voxel_api;
  FrameBufferApi fbo_api;
  SampleCollectionApi sample_api;
  TrackerApi tracker_api;
  ShaderModuleApi sh_api;
  WModApi mod_api;
  PhysicsApi physics_api;
  TriStripApi ts_api;
  CutterApi cutter_api;
  BooleanOps bool_api;
  CollisionPlane collision_api;
  MovementNode move_api;
  ImplicitApi implicit_api;
  PickingApi picking_api;
  TreeApi tree_api;
  MaterialsApi materials_api;
  UberShaderApi uber_api;
  CurveApi curve_api;
  MatricesApi matrices_api;
  Skeletal skeletal_api;
  PolygonArrayApi polygon_arr_api;
  PolygonDistanceField polygon_dist_api;
  BlockerApi blocker_api;
  VertexAnimApi vertex_anim_api;
  NewPlaneApi newplane_api;
  SurfaceApi surface_api;
  LowFrameBufferApi low_frame_api;
private:
  Env &env;
  EveryApi(const EveryApi&);
  void operator=(const EveryApi&);

};








} // namespace


class BitmapFromBuffer : public ColorBitmap
{
public:
  BitmapFromBuffer(BufferRef buf) : buf(buf) { }
  void Prepare() { }
  int SizeX() const { if (buf.buffer!=0) return buf.width; return 0; }
  int SizeY() const { if (buf.buffer!=0) return buf.height; return 0; }
  Color Map(int x, int y) const
  {
    if (x >= 0 && x <int(buf.width))
      if (y >= 0 && y < int(buf.height))
	return Color(buf.buffer[x+y*buf.ydelta]);
    return Color();
  }
  unsigned int &Map2(int x, int y) const
  {
    if (x >= 0 && x <int(buf.width))
      if (y >= 0 && y < int(buf.height))
	return buf.buffer[x+y*buf.ydelta];
    throw "error";
  }


public:
  BufferRef buf;
};
int bitmap_find_data(std::string data);
extern std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
std::vector<unsigned char> load_from_url(std::string url);
void ProgressBar(int num, int val, int max, std::string label);
BufferRef LoadImageFromString(std::vector<unsigned char> buffer, bool &success);

BitmapHandle *find_bitmap(GameApi::Env &e, GameApi::BM b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BitmapHandle *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bm.size())
    handle = ee->bm[b.id];
  return handle;
}

Bitmap<Color> *find_color_bitmap(BitmapHandle *handle, int bbm_choose=-1);

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
Bitmap<Color> *find_bitmap2(GameApi::Env &e, GameApi::BM bm) {
  BitmapHandle *h = find_bitmap(e,bm);
  return find_color_bitmap(h,-1);
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

BoolBitmap *find_bool_bitmap(GameApi::Env &e, GameApi::BB b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BoolBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bool_bm.size())
    handle = &ee->bool_bm[b.id];
  return handle;
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



std::string gameapi_homepageurl;
int async_pending_count;
