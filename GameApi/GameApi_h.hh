#ifndef GAMEAPI_H_H
#define GAMEAPI_H_H

#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
#define THREADS 1
#endif

#ifndef EMSCRIPTEN
#define VAO 1
#endif


#include "GameApi.hh"
#include <vector>
#include <map>
#include "Effect.hh"
#include "VectorTools.hh"
#include "Graph.hh"
#include "Physics.hh"
#include "State.hh" 
#include "Serialize.hh"
#include "GameRunner.hh"
#include "RayTracing.hh"
#include "VertexArray.hh"
#include "StateChange.hh"
#include "DistanceObject.hh"
#include "MatrixApi.hh"
#include "Web.hh"
#include "Effect2.hh"
#include <iostream>
#include <pthread.h>
#include <fstream>

#include <memory>
#include <cmath>
#include <chrono>

#define NO_SDL_GLEXT 
#include <GL/glew.h> 
#include <GL/gl.h>
#ifdef SDL2_USED
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h> 
#include <SDL/SDL_opengl.h>
#endif
#include <SDL_mixer.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#undef LoadImage

#include "FreeType.hh"
#include "Parser.hh"

#define SPECIAL_TEX_ID 600000
#define SPECIAL_TEX_IDA 700000

void *operator new( std::size_t count);
void operator delete(void* ptr) noexcept;

std::string unique_id();

struct TexCoordQuad
{
  Point2d p1;
  Point2d p2;
};
Program *find_shader_program(GameApi::Env &e, GameApi::SH sh);
struct EventPriv
{
};

#if 0
struct SpritePriv
{
  //ArrayRender rend;
  std::map<int, ArrayRender*> renders;
  std::map<int, Sprite*> sprites;
  std::vector<Sprite**> arrays;
  ~SpritePriv();
};
#endif

struct GridPriv
{
  ArrayRender rend;
  Bitmap<Color> *blocks;
  std::map<int, int> cellsx;
  std::map<int, int> cellsy;
  Bitmap<Pos> *positions;
  int last_id;
};
#if MOVED_TO_GAMERUNNER_HH
struct PolyPriv
{
  std::map<int, ArrayRender*> rend;
  std::map<int, StateBitmaps*> states;
  ~PolyPriv();
};
#endif

struct SurfPriv
{
};

struct MainLoopPriv
{
  SDL_Surface *screen;
  unsigned int time;
  unsigned int frame_time;
  unsigned int avg_time;
  float current_time;
  float delta_time;
  unsigned int previous_frame_time;
  int count;
  int frame;
#ifndef EMSCRIPTEN
  std::map<std::string, std::chrono::high_resolution_clock::duration > profile_sums;
  std::map<std::string, int> profile_count;
  std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock> > profile_start_time;
#endif
  int screen_width, screen_height;
  int corner_x=0, corner_y=0;
  int screen_rect_sx=800, screen_rect_sy=600;
  int last_error=0;
};

struct ShaderPriv2
{
  ShaderFile *file;
  ShaderSeq *seq;
  std::map<int,int> ids;
  int count;
};

struct FBOPriv
{
  GLuint fbo_name;
  GLuint texture;
  GLuint depthbuffer;
  int sx,sy;
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

class AnimInt 
{
public:
  virtual int Index(int path, float f) const=0;
  virtual float Duration() const=0;
  virtual int NumPaths() const=0;
  virtual ~AnimInt() { }
};
class SingleAnimInt : public AnimInt
{
public:
  SingleAnimInt(int v, float d) : d(d), v(v) { }
  int Index(int path, float t) const { return v; }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  float d;
  int v;
};
class LineAnimInt : public AnimInt
{
public:
  LineAnimInt(int val1, int val2, float d) : val1(val1), val2(val2), d(d) { }
  int Index(int path, float t) const
  {
    t/=d;
    return val1*t + val2*(1.0-t);
  }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  int val1,val2;
  float d;
};


class AnimPoint3d 
{
public:
  virtual Point Index(int path, float f) const=0;
  virtual float Duration() const=0;
  virtual int NumPaths() const=0;
  virtual ~AnimPoint3d() { }
};

class SingleAnimPoint : public AnimPoint3d
{
public:
  SingleAnimPoint(Point v, float d) : d(d), v(v) { }
  Point Index(int path, float t) const { return v; }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  float d;
  Point v;
};
class LineAnimPoint : public AnimPoint3d
{
public:
  LineAnimPoint(Point val1, Point val2, float d) : val1(val1), val2(val2), d(d) { }
  Point Index(int path, float t) const
  {
    t/=d;
    return t*Vector(val1) + (1.0-t)*Vector(val2);
  }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  Point val1,val2;
  float d;
};


class AnimFloat 
{
public:
  virtual float Index(int path, float f) const=0;
  virtual float Duration() const=0;
  virtual int NumPaths() const=0;
  virtual ~AnimFloat() { }
};


class SingleAnimFloat : public AnimFloat
{
public:
  SingleAnimFloat(float v, float d) : d(d), v(v) { }
  float Index(int path, float t) const { return v; }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  float d;
  float v;
};
class LineAnimFloat : public AnimFloat
{
public:
  LineAnimFloat(float val1, float val2, float d) : val1(val1), val2(val2), d(d) { }
  float Index(int path, float t) const
  {
    t/=d;
    return val1*t + val2*(1.0-t);
  }
  float Duration() const { return d; }
  int NumPaths() const { return 1; }
private:
  float val1,val2;
  float d;
};

struct BoolBitmap
{
  Bitmap<bool> *bitmap;
};
struct FloatBitmap
{
  Bitmap<float> *bitmap;
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

class IDInterface
{
public:
  virtual int Size() const=0;
  virtual int Poly_id(int count) const=0;
  virtual int facenum(int count) const=0;
  virtual int pointnum(int count) const=0;
  virtual int type(int count) const=0; // 0=point, 1=normal, 2=color, 3=texcoord
};
class IDInterfaceUnion : public IDInterface
{
public:
  IDInterfaceUnion(IDInterface &id1, IDInterface &id2) : id1(id1), id2(id2) { }
  virtual int Size() const { return id1.Size()+id2.Size(); }
  virtual int Poly_id(int count) const
  {
    if (count < id1.Size()) return id1.Poly_id(count);
    return id2.Poly_id(count-id1.Size());
  }
  virtual int facenum(int count) const
  {
    if (count < id1.Size()) return id1.facenum(count);
    return id2.facenum(count-id1.Size());
  }
  virtual int pointnum(int count) const
  {
    if (count < id1.Size()) return id1.pointnum(count);
    return id2.pointnum(count-id1.Size());
  }
  virtual int type(int count) const // 0=point, 1=normal, 2=color, 3=texcoord
  {
    if (count < id1.Size()) return id1.type(count);
    return id2.type(count-id1.Size());
  }
  
private:
  IDInterface &id1, &id2;
};

#undef interface
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
  GLuint buffer;
  GLuint buffer2_1;
  GLuint buffer2;
  GLuint vao[1];
  GLuint pos_buffer;
};

struct PointArray3
{
  float *array;
  unsigned int *color;
  int numpoints;
  GLuint buffer[2];
  GLuint vao[1];
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

static const int ArrayElements = 2;
template<class T>
class ArrayElem
{
};
template<>
class ArrayElem<GameApi::BM>
{
public:
  static const int i=0;
};
template<>
class ArrayElem<GameApi::P>
{
  static const int i=1;
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
  std::vector<VertexAnimNode*> vertex_anims;
  std::vector<PointTransform*> point_transforms;
  std::vector<CurvePos*> curve_pos;
  std::vector<ArrayType*> arrays2;
  std::vector<Fetcher<int>*> int_fetchers;
  std::vector<Fetcher<std::string>*> string_fetchers;
  std::vector<Fetcher<float>*> float_fetchers;
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
  //std::vector<EventInfo> event_infos;
  Sequencer2 *event_infos; // owned, one level only.
  pthread_mutex_t mutex;
  void lock() { pthread_mutex_lock(&mutex); }
  void unlock() { pthread_mutex_unlock(&mutex); }
  FT_Library lib;
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
  int s5 = surfaces.size();
  for(int i5=0;i5<s5;i5++)
    {
      SurfaceImpl s = surfaces[i5];
      delete s.surf;
      surfaces[i5].surf=0;
    }  
  surfaces.resize(0);
  surfaces.shrink_to_fit();
  delete event_infos;
  event_infos = 0;

#if 0
  int s6 = matrix.size();
  for(int i6=0;i6<s6;i6++)
    {
      MatrixInterface *i = matrix[i6];
      delete i;
    }
#endif
  }
  EnvImpl();
#if 0
  EnvImpl() : event_infos(new EmptySequencer2) 
  {
#ifndef EMSCRIPTEN
    int err = FT_Init_FreeType(&lib);
    //std::cout << "Freetype init error: " << err << std::endl;
#endif
    cursor_pos_point_id.id = -1;
  }
#endif
  ~EnvImpl();
};

#if MOVED_TO_PARSER_HH
struct PolyHandle
{
  int id;
  virtual ~PolyHandle() { }
};
struct FaceCollPolyHandle : public PolyHandle
{
  FaceCollection *coll; 
  bool collowned;
  //FaceCollection **collarray;
  Array<int, FaceCollection*> *collarray;
  bool collarrayowned;
  //int size;
  
  ~FaceCollPolyHandle() { if (collowned) delete coll; if (collarrayowned) delete [] collarray; }

};
#endif

#define ARRMACRO(arr,arr2) \
  arr add_array_ ## arr2 (GameApi::Env &e, ArrayType *type); \
  ArrayType *find_array_ ## arr2 (GameApi::Env &e, arr a);

ARRMACRO(GameApi::PAR,par)
#undef ARRMACRO


//
// add functions
//
GameApi::CBB add_cont_bool(GameApi::Env &e, ContinuousBitmap<bool> *bm);
GameApi::CFB add_cont_float(GameApi::Env &e, ContinuousBitmap<float> *bm);
GameApi::SI add_shader(GameApi::Env &e, ShaderI *shid);
GameApi::TXID add_txid(GameApi::Env &e, TextureID *txid);
GameApi::PLP add_plane_points(GameApi::Env &e, PointsInPlane *pl);
GameApi::PLL add_plane_lines(GameApi::Env &e, LinesInPlane *pl);
GameApi::PLF add_plane_faces(GameApi::Env &e, FacesInPlane *pl);
GameApi::PR add_property_array(GameApi::Env &e, PropertyArray *arr);
GameApi::SP add_space_3d(GameApi::Env &e, Space3d *sp);
GameApi::PA add_patch(GameApi::Env &e, CurvePatch *patch);
GameApi::CMD add_cmds(GameApi::Env &e, CmdExecute *cmds);
GameApi::FI add_font_interface(GameApi::Env &e, FontInterface *fi);
GameApi::GI add_glyph_interface(GameApi::Env &e, GlyphInterface *gi);
GameApi::SD add_string_display(GameApi::Env &e, StringDisplay *sd);
GameApi::FF add_float_fetcher(GameApi::Env &e, Fetcher<float> *f);
GameApi::IF add_int_fetcher(GameApi::Env &e, Fetcher<int> *i);
GameApi::SF add_string_fetcher(GameApi::Env &e, Fetcher<std::string> *str);
template<class T>
GameApi::ARR add_array(GameApi::Env &e, T t) { 
  /* this is because of arrays in builder, template instantiation
     wouldnt be accepted without this version -- real check is in runtime */
  std::cout << "ERROR: Array version used with non-arrays! " << std::endl;
  GameApi::ARR arr;
  arr.id = -1;
  return arr;
}
GameApi::ARR add_array(GameApi::Env &e, ArrayType *arr);
GameApi::CPP add_curve_pos(GameApi::Env &e, CurvePos *pos);
GameApi::PTT add_point_transform(GameApi::Env &e, PointTransform *ptt);
GameApi::KF add_vertex_anim(GameApi::Env &e, VertexAnimNode *node);
GameApi::BLK add_blocker(GameApi::Env &e, Blocker *blk);
GameApi::CC add_color(GameApi::Env &e, ColorChange *cc);
GameApi::MX add_mixed(GameApi::Env &e, MixedI *n);
GameApi::SA add_skeletal(GameApi::Env &e, SkeletalNode *n);
GameApi::PP add_plane_shape(GameApi::Env &e, PlaneShape *shape);
GameApi::MC add_matrix_curve(GameApi::Env &e, Curve<Matrix> *curve);
GameApi::MS add_matrix_array(GameApi::Env &e, MatrixArray *arr);
GameApi::C add_curve(GameApi::Env &e, Curve<Point> *curve);
GameApi::US add_uber(GameApi::Env &e, ShaderCall *call);
GameApi::MT add_material(GameApi::Env &e, Material *mat);
GameApi::TL add_tree_level(GameApi::Env &e, TreeLevel *lvl);
GameApi::T add_tree(GameApi::Env &e, TreeStack *tre);
GameApi::IM add_implicit(GameApi::Env &e, ImplicitFunction3d *imp);
GameApi::MN add_move(GameApi::Env &e, Movement *m);
GameApi::CP add_collision(GameApi::Env &e, Collision *c);
GameApi::TS add_tri_strip(GameApi::Env &e, TriStrip *n);
GameApi::PH add_physics(GameApi::Env &e, PhysicsNode *n);
GameApi::EX add_expr(GameApi::Env &e, ExprNode *n);
template<class T>
GameApi::A<T> add_array(GameApi::Env &e, std::vector<T> *arr);
GameApi::ML add_main_loop(GameApi::Env &e, MainLoopItem *item);
GameApi::FtA add_font_atlas(GameApi::Env &e, FontAtlasInfo *info);
GameApi::W add_widget(GameApi::Env &e, GuiWidget *w);
void add_update_widget(GameApi::Env &e, GameApi::W widget, GuiWidget *w);
GameApi::SFO add_shader_module(GameApi::Env &e, ShaderModule *vol);
GameApi::SM add_sample(GameApi::Env &e, Samples *s);
GameApi::TRK add_tracker(GameApi::Env &e, Tracker *trk);
GameApi::FBO add_fbo(GameApi::Env &e, GLuint fbo_name, GLuint texture, GLuint depthbuffer, int sx, int sy);
GameApi::F add_float(GameApi::Env &e, LazyValue<float> *val);
GameApi::FA add_float_array(GameApi::Env &e, Array<int,float> *arr);
GameApi::FD add_distance(GameApi::Env &e, DistanceRenderable *dist);
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i);
GameApi::S add_surface(GameApi::Env &e, SurfaceImpl i);
GameApi::BM add_bitmap(GameApi::Env &e, BitmapHandle *handle);
GameApi::BM add_color_bitmap(GameApi::Env &e, Bitmap<Color> *bm);
GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm);
GameApi::BMA add_bitmap_array(GameApi::Env &e, BitmapArray2<Color> *arr);
GameApi::BB add_bool_bitmap(GameApi::Env &e, Bitmap<bool> *bitmap);
GameApi::LAY add_layout(GameApi::Env &e, Layout *l);
GameApi::WV add_waveform(GameApi::Env &e, Waveform *bitmap);
GameApi::TX add_texture(GameApi::Env &e, TextureI *i);
GameApi::Q add_tex_quad(GameApi::Env &e, Point2d p1, Point2d p2);
GameApi::FB add_float_bitmap(GameApi::Env &e, Bitmap<float> *bitmap);
GameApi::VA add_vertex_array(GameApi::Env &e, VertexArraySet *va, RenderVertexArray *arr);
void add_update_vertex_array(GameApi::Env &e, GameApi::VA va_h, VertexArraySet *va, RenderVertexArray *arr);
GameApi::CBM add_continuous_bitmap(GameApi::Env &e, ContinuousBitmap<Color> *bitmap);
GameApi::F add_function(GameApi::Env &e, FunctionImpl &f);
GameApi::F add_function(GameApi::Env &e, Function<float,float> *f);
GameApi::SA add_separate(GameApi::Env &e, Separate *sep);
GameApi::PL add_plane(GameApi::Env &e, PlanePoints2d *sep);
GameApi::P add_polygon(GameApi::Env &e, FaceCollPolyHandle *handle);
GameApi::OM add_move(GameApi::Env &e, Array<int, ObjectWithPos> *arr);
GameApi::VAA add_move_array(GameApi::Env &e, std::vector<VertexArrayWithPos> *vec);
GameApi::P add_polygon(GameApi::Env &e, FaceCollection *coll, int size);
GameApi::P add_polygon2(GameApi::Env &e, FaceCollection *coll, int size);
GameApi::M add_matrix2(GameApi::Env &e, Matrix mm);
GameApi::M add_matrix(GameApi::Env &e, MatrixInterface *i);
GameApi::LI add_line_array(GameApi::Env &e, LineCollection *array);
GameApi::PTS add_points_api_points(GameApi::Env &e, PointsApiPoints *pts);
GameApi::PTA add_point_array3(GameApi::Env &e, PointArray3 *array);
GameApi::FOA add_point_array(GameApi::Env &e, PointArray2 *array);
void add_update_lines_array(GameApi::Env &e, GameApi::LLA la, PointArray2 *array);
GameApi::LLA add_lines_array(GameApi::Env &e, PointArray2 *array);
GameApi::PC add_pointcoll_array(GameApi::Env &e, PointCollection *array);
GameApi::PT add_point(GameApi::Env &e, float x, float y);
GameApi::TR add_timerange(GameApi::Env &e, int paths);
GameApi::TR add_timerange(GameApi::Env &e, TROArray *arr);
GameApi::VV add_timerange_vertexarray(GameApi::Env &e, GameApi::TR tr);
GameApi::O add_volume(GameApi::Env &e, VolumeObject *o);
GameApi::FO add_float_volume(GameApi::Env &e, FloatVolumeObject *o);
GameApi::COV add_color_volume(GameApi::Env &e, ColorVolumeObject *o);
GameApi::VO add_vector_volume(GameApi::Env &e, VectorVolumeObject *o);
GameApi::VX add_voxel(GameApi::Env &e, Voxel<unsigned int> *o);
GameApi::VX add_int_voxel(GameApi::Env &e, Voxel<int> *o);
GameApi::CO add_color(GameApi::Env &e, int r, int g, int b, int a);
GameApi::E add_event(GameApi::Env &e, const EventInfo &info);
GameApi::L add_link(GameApi::Env &e, GameApi::E e1, GameApi::E e2, LinkInfo info);
GameApi::MV add_mv_point(GameApi::Env &e, float x, float y, float z);
GameApi::MV add_line(GameApi::Env &e, GameApi::E start, GameApi::E end, 
		     GameApi::MV start_mv, GameApi::MV end_mv, int start_choose, int end_choose);
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z);
GameApi::LL add_pos(GameApi::Env &e, GameApi::L l, GameApi::MV point);

GameApi::V add_vector(GameApi::Env &e, float dx, float dy, float dz);
GameApi::IS add_anim(GameApi::Env &e, const AnimImpl &impl);
GameApi::CT add_cutter(GameApi::Env &e, Cutter *cut);

//
// find() functions
//
ContinuousBitmap<float> *find_cont_float(GameApi::Env &e, GameApi::CFB bm);
ContinuousBitmap<bool> *find_cont_bool(GameApi::Env &e, GameApi::CBB bm);
ShaderI *find_shader(GameApi::Env &e, GameApi::SI id);
TextureID *find_txid(GameApi::Env &e, GameApi::TXID id);
PointsInPlane *find_plane_points(GameApi::Env &e, GameApi::PLP plane);
LinesInPlane *find_plane_lines(GameApi::Env &e, GameApi::PLL plane);
FacesInPlane *find_plane_faces(GameApi::Env &e, GameApi::PLF plane);
PropertyArray *find_prop_array(GameApi::Env &e, GameApi::PR prop);
Space3d *find_space_3d(GameApi::Env &e, GameApi::SP space);
CurvePatch *find_patch(GameApi::Env &e, GameApi::PA patch);
CmdExecute *find_cmds(GameApi::Env &e, GameApi::CMD cmds);
StringDisplay *find_string_display(GameApi::Env &e, GameApi::SD sd);
GlyphInterface *find_glyph_interface(GameApi::Env &e, GameApi::GI gi);
FontInterface *find_font_interface(GameApi::Env &e, GameApi::FI fi);
Fetcher<float> *find_float_fetcher(GameApi::Env &e, GameApi::FF f);
Fetcher<std::string> *find_string_fetcher(GameApi::Env &e, GameApi::SF s);
Fetcher<int> *find_int_fetcher(GameApi::Env &e, GameApi::IF f);
ArrayType *find_array(GameApi::Env &e, GameApi::ARR arr);
CurvePos *find_curve_pos(GameApi::Env &e, GameApi::CP pos);
PointTransform *find_point_transform(GameApi::Env &e, GameApi::PTT pt);
VertexAnimNode *find_vertex_anim(GameApi::Env &e, GameApi::KF kf);
Blocker *find_blocker(GameApi::Env &e, GameApi::BLK blk);
ColorChange *find_color(GameApi::Env &e, GameApi::CC cc);
MixedI *find_mixed(GameApi::Env &e, GameApi::MX m);
PD_Impl find_polydistfield(GameApi::Env &e, GameApi::PD p);
SkeletalNode *find_skeletal(GameApi::Env &e, GameApi::SA n);
PlaneShape *find_plane_shape(GameApi::Env &e, GameApi::PP pp);
Curve<Matrix> *find_matrix_curve(GameApi::Env &e, GameApi::MC m);
MatrixArray *find_matrix_array(GameApi::Env &e, GameApi::MS m);
Curve<Point> *find_curve(GameApi::Env &e, GameApi::C c);
ShaderCall *find_uber(GameApi::Env &e, GameApi::US u);
Material *find_material(GameApi::Env &e, GameApi::MT mat);
TreeLevel *find_tree_level(GameApi::Env &e, GameApi::TL tl);
TreeStack *find_tree(GameApi::Env &e, GameApi::T t);
ImplicitFunction3d *find_implicit(GameApi::Env &e, GameApi::IM m);
Movement *find_move(GameApi::Env &e, GameApi::MN m);
Collision *find_collision(GameApi::Env &e, GameApi::CP p);
TriStrip *find_tri_strip(GameApi::Env &e, GameApi::TS p);
PhysicsNode *find_physics(GameApi::Env &e, GameApi::PH p);
ExprNode *find_expr(GameApi::Env &e, GameApi::EX n);
template<class T>
std::vector<T> *find_array(GameApi::Env &e, GameApi::A<T> arr);
MainLoopItem *find_main_loop(GameApi::Env &e, GameApi::ML ml);
FontAtlasInfo *find_font_atlas(GameApi::Env &e, GameApi::FtA ft);
ShaderModule *find_shader_module(GameApi::Env &e, GameApi::SFO sfo);
GuiWidget *find_widget(GameApi::Env &e, GameApi::W w);
Samples* find_samples(GameApi::Env &e, GameApi::SM sm);
Tracker* find_tracker(GameApi::Env &e, GameApi::TRK trk);
EventInfo find_event_info(GameApi::Env &e, GameApi::E ee);
FBOPriv *find_fbo(GameApi::Env &e, GameApi::FBO f);
LinkageInfo find_linkage(GameApi::Env &e, GameApi::L l);
DistanceRenderable *find_distance(GameApi::Env &e, GameApi::FD fd);
LazyValue<float> *find_float(GameApi::Env &e, GameApi::F f);
Array<int, float> *find_float_array(GameApi::Env &e, GameApi::FA f);
TextureI *find_texture(GameApi::Env &e, GameApi::TX t);
TexCoordQuad find_tex_quad(GameApi::Env &e, GameApi::Q t);
LinkInfo find_link_info(GameApi::Env &e, GameApi::L l);
PosInfo find_pos_info(GameApi::Env &e, GameApi::LL l);
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv);
BitmapHandle *find_bitmap(GameApi::Env &e, GameApi::BM b);
ShaderPriv2 *find_shaderpriv(GameApi::Env &e, GameApi::SH sh);
Program *find_shader_program(GameApi::Env &e, GameApi::SH sh);
BitmapArray2<Color> *find_bitmap_array(GameApi::Env &e, GameApi::BMA b);
BoolBitmap *find_bool_bitmap(GameApi::Env &e, GameApi::BB b);
Layout *find_layout(GameApi::Env &e, GameApi::LAY l);
Waveform *find_waveform(GameApi::Env &e, GameApi::WV b);
ContinuousBitmap<Color> *find_continuous_bitmap(GameApi::Env &e, GameApi::CBM b);
FloatBitmap *find_float_bitmap(GameApi::Env &e, GameApi::FB b);
Voxel<unsigned int> *find_voxel(GameApi::Env &e, GameApi::VX b);
Voxel<int> *find_int_voxel(GameApi::Env &e, GameApi::VX b);
Separate* find_separate(GameApi::Env &e, GameApi::SA p);
PlaneData *find_plane_array(GameApi::Env &e, GameApi::PLA p);
PlanePoints2d* find_plane(GameApi::Env &e, GameApi::PL p);
Array<int,ObjectWithPos> *find_move(GameApi::Env &e, GameApi::OM p);
std::vector<VertexArrayWithPos> *find_move_array(GameApi::Env &e, GameApi::VAA p);
FaceCollPolyHandle *find_poly(GameApi::Env &e, GameApi::P p);
RenderVertexArray *find_vertex_array_render(GameApi::Env &e, GameApi::VA p);
VertexArraySet *find_vertex_array(GameApi::Env &e, GameApi::VA p);
FaceCollection *find_facecoll(GameApi::Env &e, GameApi::P p);
FunctionImpl *find_function(GameApi::Env &e, GameApi::F f);
SurfaceImpl *find_surface(GameApi::Env &e, GameApi::S f);
AnimImpl find_anim(GameApi::Env &e, GameApi::IS i);
Bitmap<Color> *find_color_bitmap(BitmapHandle *handle, int bbm_choose=-1);
Bitmap<Pos> *find_pos_bitmap(BitmapHandle *handle, int bbm_choose=-1);
Sprite *sprite_from_handle2(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose);
Sprite *sprite_from_handle(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose);
PointsApiPoints *find_pointsapi_points(GameApi::Env &e, GameApi::PTS ps);
LineCollection *find_line_array(GameApi::Env &e, GameApi::LI li);
PointArray3 *find_point_array3(GameApi::Env &e, GameApi::PTA pa);
PointArray2 *find_point_array(GameApi::Env &e, GameApi::FOA p);
PointArray2 *find_lines_array(GameApi::Env &e, GameApi::LLA p);
PointCollection *find_pointcoll_array(GameApi::Env &e, GameApi::PC p);
Point *find_point(GameApi::Env &e, GameApi::PT p);
Matrix find_matrix(GameApi::Env &e, GameApi::M m);
TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr);
VArray *find_timerange_vertexarray(GameApi::Env &e, GameApi::VV vv);
VolumeObject *find_volume(GameApi::Env &e, GameApi::O o);
FloatVolumeObject *find_float_volume(GameApi::Env &e, GameApi::FO o);
ColorVolumeObject *find_color_volume(GameApi::Env &e, GameApi::COV o);
VectorVolumeObject *find_vector_volume(GameApi::Env &e, GameApi::VO o);
Color *find_color(GameApi::Env &e, GameApi::CO p);
Vector *find_vector(GameApi::Env &e, GameApi::V p);
SpaceImpl *find_space(GameApi::Env &e, GameApi::SP s);
LineImpl *find_line(GameApi::Env &e, GameApi::LN l);
SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);
Cutter *find_cutter(GameApi::Env &e, GameApi::CT cut);


template<class T>
GameApi::A<T> GameApi::ArrayApi<T>::array(std::vector<T> vec)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  std::vector<int> vec2;
  int s = vec.size;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(vec[i].id);
    }
  env->handle_array.push_back(vec2);
  GameApi::A<T> a;
  a.id = env->handle_array.size()-1;
  return a;
}

void check_world(MainLoopEnv &e);
void check_world_elements(MainLoopEnv &e);
EnemyPiece &get_enemy_piece(MainLoopEnv &e, int index);

#endif
