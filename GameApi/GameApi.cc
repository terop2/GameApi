        
#define SDL2_USED  
#define GAME_API_DEFS
#define _SCL_SECURE_NO_WARNINGS

#include "GameApi.hh" 
#include "Graph.hh"  
#include "Physics.hh"
#include "State.hh" 
#include "Serialize.hh"
 
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

#include <memory>
#include <cmath>

#include "Parser.hh"
#ifndef EMSCRIPTEN
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include "FreeType.hh"
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


#undef LoadImage

GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm);

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

struct SurfPriv
{
};

struct MainLoopPriv
{
  SDL_Surface *screen;
  unsigned int time;
  int count;
  int frame;
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


EXPORT GameApi::MainLoopApi::MainLoopApi(Env &e) : frame(0.0), time(0.0), e(e)  
{
  priv = (void*)new MainLoopPriv;
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame = 0;
  p->time = 0;
  p->count = 0;
}
EXPORT GameApi::MainLoopApi::~MainLoopApi()
{
  delete (MainLoopPriv*)priv;
}
EXPORT void GameApi::MainLoopApi::cursor_visible(bool enabled)
{
  SDL_ShowCursor(enabled);
}
EXPORT void GameApi::MainLoopApi::init_window(int screen_width, int screen_height)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
  p->screen = InitSDL2(screenx,screeny,false);
#else
  p->screen = InitSDL(screenx,screeny,false);
#endif
  time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
}
EXPORT void GameApi::MainLoopApi::init(SH sh, int screen_width, int screen_height)
{
  int screenx = screen_width;
  int screeny = screen_height;
  //glColor4f(1.0,1.0,1.0, 0.5);
  Program *prog = find_shader_program(e, sh);
  prog->use();
  Matrix m = Matrix::Ortho(0,screenx, screeny, 0,0,1);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  prog->set_var("in_T", m2);
  prog->set_var("in_POS", 0.0f);
  alpha_1(false);

#if 0
  glMatrixLoadIdentityEXT(GL_PROJECTION);
  glMatrixLoadIdentityEXT(GL_MODELVIEW);
  glMatrixOrthoEXT(GL_PROJECTION, 0, 800, 600, 0, -1, 1);
#endif
}
EXPORT void GameApi::MainLoopApi::transfer_sdl_surface(MainLoopApi &orig)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  MainLoopPriv *p2 = (MainLoopPriv*)orig.priv;
  p->screen = p2->screen;
}
EXPORT void GameApi::MainLoopApi::delay(int ms)
{
  SDL_Delay(ms);
}
EXPORT unsigned int GameApi::MainLoopApi::rand_max()
{
  Random r;
  return r.maximum();
}
EXPORT unsigned int GameApi::MainLoopApi::random()
{
  Random r;
  return r.next();
}
EXPORT void GameApi::MainLoopApi::fpscounter()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  unsigned int time = SDL_GetTicks();
  unsigned int delta_time = time - p->time;
  //p->time = time;
  if (p->count==0) { p->time = time; p->frame=0; }
  p->frame++;
  p->count++;
  if (delta_time<1) delta_time=1;
  float fps = p->frame/(delta_time/1000.0f);
  if (p->count<0) { p->count = 0; }
  if (p->count>100) {
    std::cout << "FPS: " << fps << std::endl;
    p->count = 0;
  }
}

EXPORT void GameApi::MainLoopApi::init_3d(SH sh, int screen_width, int screen_height)
{
  int screenx = screen_width;
  int screeny = screen_height;
#if 0
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
  p->screen = InitSDL2(screenx,screeny,true);
#else
  p->screen = InitSDL(screenx,screeny,true);
#endif
  //glColor4f(1.0,1.0,1.0, 0.5);
  time = SDL_GetTicks();
#endif
  glDisable(GL_LIGHTING);
  //glDisable(GL_DEPTH_TEST);
  //glMatrixMode( GL_PROJECTION ); 
  // glLoadIdentity(); 
  //glOrtho(0, screenx, screeny,0,0,1);
  /*
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 99.1, 60000.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  glMultMatrixf(&mat[0]);
  */
  Program *prog = find_shader_program(e, sh);
  prog->use();
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
  prog->set_var("in_T", m3);
  prog->set_var("in_POS", 0.0f);
  alpha_1(false);
  glEnable(GL_DEPTH_TEST);
}
EXPORT void GameApi::MainLoopApi::nvidia_init()
{
  if (GLEW_NV_path_rendering)
    {
  glMatrixLoadIdentityEXT(GL_PROJECTION);
  glMatrixLoadIdentityEXT(GL_MODELVIEW);
  glMatrixOrthoEXT(GL_PROJECTION, 0, 800, 600, 0, -1, 1);
    }
}

EXPORT void GameApi::MainLoopApi::switch_to_3d(bool b, SH sh)
{
  int screenx = 800;
  int screeny = 600;
  if (b)
    {
      Program *prog = find_shader_program(e, sh);
      glDisable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
      Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
      Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
      prog->set_var("in_P", m);
      prog->set_var("in_T", m3);
      //glMultMatrixf(&mat[0]);
#ifndef EMSCRIPTEN      
      glMatrixMode( GL_MODELVIEW ); 
#endif
      //Matrix m2 = Matrix::Translate(0.0, 0.0, -500.0);
      Matrix m2 = Matrix::Identity();
      prog->set_var("in_MV", m2);
      //glMatrixLoadIdentityEXT(GL_PROJECTION);
      //glMatrixLoadIdentityEXT(GL_MODELVIEW);
      //glMatrixOrthoEXT(GL_MODELVIEW, 0, screenx, screeny, 0, 0, 1);

    }
  else
    {
      Program *prog = find_shader_program(e, sh);
      Matrix m = Matrix::Ortho(0, screenx, screeny, 0,0,1);
      //m = m*Matrix::Translate(-1.0/2, -1.0/2, 0.0);
      //m = m * Matrix::Scale(1.0, -1.0, 0.0);
      //m = m*Matrix::Translate(-1, -1, 0.0);
      prog->set_var("in_P", m);
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      //glMatrixMode( GL_PROJECTION ); 
      //glLoadIdentity(); 
      //glOrtho(0, screenx, screeny,0,0,1);
      //glMatrixMode( GL_MODELVIEW ); 
      //glLoadIdentity();
      //glTranslatef(0.375, 0.375, 0.0);
      Matrix m2 = Matrix::Translate(0.375, 0.375, 0.0);
      prog->set_var("in_MV", m2);
      Matrix m3 = Matrix::Identity();
      prog->set_var("in_T", m3);
    }
}

EXPORT void GameApi::MainLoopApi::clear()
{
  //glClearColor(255,255,255,255);
  glClearStencil(0);
  glClearColor(0,0,0,0);
  glStencilMask(~0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  //glLoadIdentity();
  //glTranslatef(0.375, 0.375, 0.0);
  //glTranslatef(0.0, 0.0, -260.0);
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}
EXPORT void GameApi::MainLoopApi::clear_3d()
{
  //glClearColor(255,255,255,255);
  glClearStencil(0);
  glClearColor(0,0,0,0);
  glStencilMask(~0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#ifndef EMSCRIPTEN
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);
  glTranslatef(0.0, 0.0, -260.0);
#endif
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}

EXPORT void GameApi::MainLoopApi::transparency(bool enabled)
{
  if (enabled)
    {
      //glBlendFunc(GL_ZERO, GL_SRC_COLOR); 
     glDepthMask(GL_FALSE);
    }
  else
    {
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(GL_TRUE);
    }
}
EXPORT void GameApi::MainLoopApi::depth_test(bool enabled)
{
  if (enabled)
    {
      glEnable(GL_DEPTH_TEST);
    }
  else
    {
      glDisable(GL_DEPTH_TEST);
    }
}
EXPORT void GameApi::MainLoopApi::alpha(bool enable)
{
  alpha_1(enable);
}
void GameApi::MainLoopApi::alpha_1(bool enable)
{
  if (enable)
    {
      glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_COLOR /*ONE_MINUS_SRC_COLOR*/, GL_DST_COLOR);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef EMSCRIPTEN
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
      //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE); 
    }
  else
    {
      glDisable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
#ifndef EMSCRIPTEN
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
    }
}
EXPORT void GameApi::MainLoopApi::antialias(bool enable)
{
  if (enable)
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
    }
  else
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0);
    }
}

EXPORT float GameApi::MainLoopApi::get_time()
{
  return SDL_GetTicks()-time;
}
EXPORT int GameApi::MainLoopApi::get_framenum()
{
  return frame;
}
EXPORT GameApi::BM GameApi::MainLoopApi::screenshot()
{
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //SDL_Surface *surf = p->screen;
  int w = 800; //surf->w;
  int h = 600; //surf->h;

  BufferRef ref = BufferRef::NewBuffer(w,h);
  glReadPixels(0,0,w,h, GL_BGRA, GL_UNSIGNED_BYTE, ref.buffer);
  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  return add_color_bitmap2(e, bm);
}
extern SDL_Window *sdl_window;

EXPORT void GameApi::MainLoopApi::swapbuffers()
{
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //glLoadIdentity();
#ifdef SDL2_USED
  SDL_GL_SwapWindow(sdl_window);
#else
  SDL_GL_SwapBuffers();
#endif


  //SDL_Flip(surf);
  frame++;
}
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i);
GameApi::SP GameApi::MainLoopApi::screenspace()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  SDL_Surface *surf = p->screen;
  int w = surf->w;
  int h = surf->h;
  SpaceImpl sp;
  Point tl( 0.0, 0.0, 0.0 );
  Point tr(float(w), 0.0, 0.0);
  Point bl(0.0, float(h),0.0);
  Point br(float(w), float(h),0.0);
  Point z(0.0,0.0,550.0);
  Point origo(0.0, 0.0,0.0);
  sp.tl = tl;
  sp.tr = tr;
  sp.bl = bl;
  sp.br = br;
  sp.z = z;
  sp.origo = origo;
  return add_space(e, sp);
}

#if MOVED_TO_PARSER_HH
struct BitmapHandle
{
  int id;
  virtual ~BitmapHandle() { }
};
#endif
struct BoolBitmap
{
  Bitmap<bool> *bitmap;
};
struct FloatBitmap
{
  Bitmap<float> *bitmap;
};

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
  FontGlyphBitmap *bm;
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
  unsigned int *color_array;
  int numpoints;
  GLuint buffer;
  GLuint buffer2;
  GLuint vao[1];
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
  //std::vector<EventInfo> event_infos;
  Sequencer2 *event_infos; // owned, one level only.
#ifndef EMSCRIPTEN
  FT_Library lib;
#endif
  std::vector<Font> fonts;
  static ::EnvImpl *Environment(GameApi::Env *e) { return (EnvImpl*)e->envimpl; }
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

  int vv1 = volumes.size();
  for(int i_v=0;i_v<vv1;i_v++)
    {
      delete volumes[i_v];
      volumes[i_v] =0;
    }
  volumes.resize(0);
  volumes.shrink_to_fit();
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
  EnvImpl() : event_infos(new EmptySequencer2) 
  {
#ifndef EMSCRIPTEN
    FT_Init_FreeType(&lib);
#endif
    cursor_pos_point_id.id = -1;
  }
  ~EnvImpl();
};
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

template<class T>
void ArrayDelete(T *ptr)
{
  delete [] ptr;
}
GameApi::PT add_point(GameApi::Env &e, float x, float y);
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z);



EXPORT GameApi::MainLoopApi::Event GameApi::MainLoopApi::get_event()
{
  SDL_Event event;
  Event e2;
  SDL_PollEvent(&event);
  int x,y;
  int val = SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  e2.ch = event.key.keysym.sym;

  if (event.type==SDL_FINGERMOTION||event.type==SDL_FINGERDOWN||event.type==SDL_FINGERUP)
    {
      SDL_TouchFingerEvent *ptr = &event.tfinger;
      x = (int)ptr->x;
      y = (int)ptr->y;
    }

  int id = 0;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (env->cursor_pos_point_id.id==-1)
    {
      GameApi::PT pt = add_point(e, x,y);
      id = pt.id;
    }
  else
    {
      id = env->cursor_pos_point_id.id;
    }
  env->pt[id].x = x;
  env->pt[id].y = y;
  e2.cursor_pos.id = id;
  e2.button = -1;
  if (val & SDL_BUTTON(1)) { e2.button = 0; }
  if (val & SDL_BUTTON(2)) { e2.button = 1; }
  if (val & SDL_BUTTON(3)) { e2.button = 2; }
  SDL_Joystick *joy1 = SDL_JoystickOpen(0);
  SDL_JoystickEventState(SDL_ENABLE);
  unsigned int but1 = SDL_JoystickGetButton(joy1, 0);  
  unsigned int but2 = SDL_JoystickGetButton(joy1, 1);
  unsigned int but3 = SDL_JoystickGetButton(joy1, 2);
  unsigned int but4 = SDL_JoystickGetButton(joy1, 3);
  //std::cout << e.type << " " << e.ch << std::endl;
  e2.joy0_button0 = but1==1;
  e2.joy0_button1 = but2==1;
  e2.joy0_button2 = but3==1;
  e2.joy0_button3 = but4==1;

  SDL_Joystick *joy2 = SDL_JoystickOpen(1);
  unsigned int a_but1 = SDL_JoystickGetButton(joy2, 0);  
  unsigned int a_but2 = SDL_JoystickGetButton(joy2, 1);
  unsigned int a_but3 = SDL_JoystickGetButton(joy2, 2);
  unsigned int a_but4 = SDL_JoystickGetButton(joy2, 3);
  //std::cout << a_but1 << " " << a_but2 << " " << a_but3 << " " << a_but4 << std::endl;
  e2.joy1_button0 = a_but1==1;
  e2.joy1_button1 = a_but2==1;
  e2.joy1_button2 = a_but3==1;
  e2.joy1_button3 = a_but4==1;

  return e2;
}

EnvImpl::~EnvImpl()
{
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
GameApi::P add_polygon2(GameApi::Env &e, FaceCollection *coll, int size)
{
  FaceCollPolyHandle *h = new FaceCollPolyHandle;
  h->coll = coll;
  h->collowned = true;
  h->collarray=0;
  h->collarrayowned = false;
  //h->size = size;
  return add_polygon(e,h);
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
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
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

GameApi::MV GameApi::EventApi::point(float x, float y, float z)
{
  return add_mv_point(e, x,y,z);
}
GameApi::MV GameApi::EventApi::line(GameApi::E start, GameApi::E end, GameApi::MV start_mv, GameApi::MV end_mv, int start_choose, int end_choose)
{
  return add_line(e, start, end, start_mv, end_mv, start_choose, end_choose); 
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
GameApi::LL GameApi::EventApi::link(GameApi::L obj, GameApi::MV move)
{
  return add_pos(e, obj, move);
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

std::string GameApi::EventApi::Serialize(ST states, int start_state)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StateRange r = env->state_ranges[states.id];
  
  AllStatesSequencer seq(env->event_infos, &env->states[r.start_range], r.range_size);
  SequencerParser parser;
  return parser.Create(&seq);
}

#if 0
ST GameApi::EventApi::UnSerialize(std::string s)
{
  SequencerParser parser;
  bool success = true;
  Sequencer2 *seq = parser.Parse(s, success);
  std::vector<E> vec;
  int c = seq->NumEvents();
  for(int i=0;i<c;i++)
    {
      E event = { 0 };
      EventInfo info = seq->Event(i);
      if (info.timer) { 
	E activation_event = { info.activation_event_id };
	event = timer(info.activation_event, time);
      }
      if (info.key_down) {
	
      }
      if (info.key_up) {
      }
      if (info.activate_mouse) {
      }
      if (info.deactivate_mouse) {
      }
      if (info.state_change) {
      }
      vec.push_back(event);
    }
  
  int c2 = seq->NumLinks();
  std::vector<L> vec;
  for(int a=0;a<c2;a++)
    {
      LinkageInfo linkage = seq->Linkage(a);
      LinkInfo link = seq->Links(a);
      E start = vec[linkage.start_event];
      E end = vec[linkage.end_event];
      L link = { 0 };
      if (link.polygon_start || link.polygon_end)
	{
	  link = polygon(start, end, polygon_start, polygon_end);
	}
      if (link.bitmap_start || link.bitmap_end)
	{
	  link = bitmap(start, end, add_bitmap(e,link.bitmap_start), add_bitmap(link.bitmap_end));
	}
      vec.push_back(link);
    }
  int c3 = seq->NumPositions();
  for(int b=0;b<c3;b++)
    {
      PosInfo pos = seq->Positions(b);
      // TODO, seems curve is not possible to unserialize using this api
    }

}
#endif
EXPORT GameApi::EventApi::~EventApi() 
{
  delete (EventPriv*)priv;
}

GameApi::ST GameApi::EventApi::states(int count_states)
{
	::EnvImpl *env =  ::EnvImpl::Environment(&e);
  int sz = count_states;
  StateInfo2 info;
  info.enable_obj_array = new DuplicateArray<int, bool>(1,false);
  info.plane = 0;
  int start = env->states.size();
  for(int i=0;i<sz;i++)
    {
      env->states.push_back(info);
    }
  int end = env->states.size();
  int count = end - start;
  StateRange r;
  r.start_range = start;
  r.range_size = count;
  env->state_ranges.push_back(r);
  GameApi::ST st;
  st.id = env->state_ranges.size()-1;
  return st;
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
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
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
Bitmap<Color> *find_color_bitmap(BitmapHandle *handle, int bbm_choose=-1)
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
Bitmap<Pos> *find_pos_bitmap(BitmapHandle *handle, int bbm_choose=-1)
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
      env2->renders2[handle->id] = new ArrayRender;
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
      env2->renders2[handle->id] = new ArrayRender;
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
      env2->renders[handle->id] = new ArrayRender;
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
      env2->renders[handle->id] = new ArrayRender;
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

template<class I, class T>
class AnimArray : public I
{
public:
  AnimArray(GameApi::Env &e, GameApi::IS *array, int size)
    : size(size), e(e)
  {
    array2 = new GameApi::IS[size];
    std::copy(array, array+size, array2);
  }
  ~AnimArray() { delete [] array2; }
  T Index(int path, float t) const
  {
    int dd = 0.0;
    for(int i=0;i<size;i++)
      {
	GameApi::IS ee = array2[i];
	AnimImpl k = find_anim(e,ee);
	const I *ptr = get(k);
	if (!ptr) continue;
	float d = ptr->Duration();
	int olddd = dd;
	dd+=d;
	if (dd > t)
	  {
	    return ptr->Index(path, t-olddd);
	  }
      }
    return T();
  }
  virtual I *get(const AnimImpl &e) const=0;
  int NumPaths() const { return 1; }
  float Duration() const
  {
    float dd = 0.0;
    for(int i=0;i<size;i++)
      {
	GameApi::IS ee = array2[i];
	AnimImpl k = find_anim(e,ee);
	const I *ptr = get(k);
	if (!ptr) continue;
	float d = ptr->Duration();
	dd+=d;
      }
    return dd;
  }
private:
  GameApi::IS *array2;
  int size;
  GameApi::Env &e;
};

template<class I, class T>
class AnimRepeat : public I
{
public:
  AnimRepeat(GameApi::Env &e, GameApi::IS val, int count=-1) : e(e), val(val),c(count) { }
  T Index(int path, float t) const
  {
    GameApi::IS ee = val;
    AnimImpl k = find_anim(e,ee);
    const I *ptr = get(k);
    if (!ptr) return T();
    float d = ptr->Duration();
    int count = int(t / d);
    float pos = count * d;
    t-=pos;
    if (c!=-1 && count > c)
      {
	t = d;
      }
    return ptr->Index(path, t);
  }
  virtual I *get(const AnimImpl &e) const=0;
  int NumPaths() const { return 1; }
  float Duration() const
  {
    GameApi::IS ee = val;
    AnimImpl k = find_anim(e,ee);
    const I *ptr = get(k);
    if (!ptr) return 0.0;
    float d = ptr->Duration();
    return d;
  }
private:
  GameApi::Env &e;
  GameApi::IS val;
  int c;
};


EXPORT GameApi::GridApi::GridApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new GridPriv;
}
EXPORT GameApi::GridApi::~GridApi()
{
  delete (GridPriv*)priv;
}

EXPORT GameApi::SpriteApi::SpriteApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new SpritePriv;
}
EXPORT GameApi::SpriteApi::~SpriteApi()
{
  delete (SpritePriv*)priv;
}
EXPORT void GameApi::SpriteApi::spritepos(BM bm, float x, float y)
{
  SpritePosImpl *i = find_sprite_pos(e,bm);
  i->x = x;
  i->y = y;
}

EXPORT void GameApi::SpriteApi::render_sprite_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  //SpritePriv &spriv = *(SpritePriv*)priv;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  if (s->texture_id!=-1 && s->texture_id<6000)
    {
      TextureEnable(*env->renders2[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders2[s->texture_id], 0, false);
    }
  else if(s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-6000);

      rend->render(0);
      //      RenderVertexArray arr(*s);
      //arr.render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else
    {
      std::cout << "SpriteApi::render_sprite_vertex_array, texture not found!" << std::endl;
    }
}
EXPORT GameApi::VA GameApi::SpriteApi::create_vertex_array(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  ::Sprite *sprite = sprite_from_handle2(e,spriv, handle, -1);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; GameApi::VA va; va.id = 0; return va; }
  VertexArraySet *s = new VertexArraySet;
  PrepareSpriteToVA(*sprite, *s);
  TexturePrepare(*sprite, *env->renders2[bm.id]);
  s->texture_id = bm.id;
  RenderVertexArray *arr = new RenderVertexArray(*s); 
  arr->prepare(0);
  s->free_memory();
  return add_vertex_array(e, s, arr); 
}

EXPORT void GameApi::SpriteApi::preparesprite(BM bm, int bbm_choose)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  //Bitmap<Color> *cbm = find_color_bitmap(handle, bbm_choose);
  //if (!cbm) return;
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *sprite = sprite_from_handle(e,spriv, handle, bbm_choose);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; return; }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  PrepareSprite(*sprite, *env->renders[bm.id]);
}
EXPORT void GameApi::SpriteApi::rendersprite(BM bm, SH sh, float x, float y, float mult_x, float mult_y)
{
  rendersprite3_1(bm, 0, sh, x, y, mult_x, mult_y);
}
EXPORT void GameApi::SpriteApi::rendersprite2(BM bm, SH sh, PT pos)
{
  rendersprite4(bm, 0, sh, pos);
}
#if 0
void GameApi::SpriteApi::rendersprite(BM bm, SH sh, float x, float y, float x1, float y1, float inside_x, float inside_y, float inside_x1, float inside_y1)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x, y };
  Point2d pos3 = { x1, y1 };
  Point2d inside_2 = { inside_x, inside_y };
  Point2d inside_3 ={ inside_x1, inside_y1 }; 
  float z = 0.0;
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, pos3, inside_2, inside_3, z, *env->renders[bm.id], prog);
}
#endif
EXPORT void GameApi::SpriteApi::rendersprite3(GameApi::BM bm, int bm_choose, GameApi::SH sh, float x, float y, float mult_x, float mult_y)
{
  rendersprite3_1(bm,bm_choose,sh,x,y,mult_x, mult_y);
}
void GameApi::SpriteApi::rendersprite3_1(GameApi::BM bm, int bm_choose, GameApi::SH sh, float x, float y, float mult_x, float mult_y)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call preparesprite() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x, y };
  float z = 0.0;
  //glPushMatrix();
  //glScalef(mult_x, mult_y, 1.0);
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id], mult_x, mult_y, prog);

  //glPopMatrix();
}
void GameApi::SpriteApi::rendersprite4(BM bm, int bm_choose, SH sh, PT pos)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point *p = find_point(e, pos);
  Point2d pos2 = { p->x, p->y };
  float z = 0.0; //p->z;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id], prog);
}

EXPORT void GameApi::SpriteApi::rendersprite5(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, float x, float y)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }
  SpaceImpl *move = find_space(e, move_space);
  SpaceImpl *spr = find_space(e, sprite_space);
  
  Point2d p1 = spr->conv(spr->tl);
  Point2d o = spr->conv(spr->origo);
  Vector2d v1 = p1 - o;

  Vector2d u_x = move->conv(move->tr) - move->conv(move->tl);
  Vector2d u_y = move->conv(move->bl) - move->conv(move->tl);
  Point2d u = move->conv(move->tl);
  Point2d pp = u + Point2d::FromVector(u_x * x) + Point2d::FromVector(u_y * y);
  pp += v1;

  Point2d pos = { pp.x, pp.y };
  float z = 0.0;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos, z, *env->renders[bm.id], prog);
}
EXPORT void GameApi::SpriteApi::rendersprite6(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, PT pos)
{
}
EXPORT GameApi::SP GameApi::SpriteApi::spritespace(BM bm)
{
  SP s;
  s.id = 0;
  return s;
}
EXPORT GameApi::PT GameApi::SpriteApi::pixelpos(BM bm, int x, int y)
{
  PT pt;
  pt.id = 0;
  return pt;
}

struct BitmapPriv
{
};

struct FunctionPriv
{
};

struct AnimPriv
{
};

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
EXPORT GameApi::ShaderApi::ShaderApi(Env &e) : e(e)
{
  priv = (void*)new ShaderPriv2;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = 0;
  p->seq = 0;
}
EXPORT GameApi::ShaderApi::~ShaderApi()
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  delete p->file;
  delete p->seq;
  delete (ShaderPriv2*)priv;
}

class ColorMap : public Function<bool,Color>
{
public:
  Color Index(bool b) const
  {
    if (!b) return Color(255,0,0,0);
    return Color(255,255,255,255);
  }
};
class ColorMap2 : public Function<bool,Pos>
{
public:
  Pos Index(bool b) const
  {
    Pos p1 = { 0,0 };
    Pos p2 = { 0,1 };
    if (!b) return p1;
    return p2;
  }
};
class AlphaColorBitmap : public Bitmap<Color>
{
public:
  AlphaColorBitmap(Bitmap<Color> &bm, unsigned int key) : bm(bm), key(key) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    unsigned int cc = c.Pixel();
    if (cc == key) cc = 0x00000000;
    return Color(cc);
  }

private:
  Bitmap<Color> &bm;
  unsigned int key;
};

EXPORT GameApi::BMA GameApi::BitmapApi::empty_array()
{
  return array(NULL, 0);
}
class BitmapArrayImpl : public BitmapArray2<Color>
{
public:
  BitmapArrayImpl(GameApi::BitmapApi *bmapi, GameApi::BM *array, int size) : bmapi(bmapi),
    m_size(size) 
  {
    if (size > 0)
      {
	m_array = new GameApi::BM[size];
	for(int i=0;i<size;i++)
	  {
	    m_array[i] = array[i];
	  }
      }
    else
      {
	m_array = 0;
      }
  }
  virtual int Size() const { return m_size; }
  virtual int SizeX(int i) const { return bmapi->size_x(m_array[i]); }
  virtual int SizeY(int i) const { return bmapi->size_y(m_array[i]); }
  virtual Color Map(int i, int x, int y) const
  {
    return bmapi->colorvalue(m_array[i], x, y);
  }

  ~BitmapArrayImpl() { delete [] m_array; }
private:
  GameApi::BitmapApi *bmapi;
  GameApi::BM *m_array;
  int m_size;
};
GameApi::BMA GameApi::BitmapApi::array(BM *array, int size)
{
  return add_bitmap_array(e, new BitmapArrayImpl(this, array, size));
}

class BitmapArrayElem : public Bitmap<Color>
{
public:
  BitmapArrayElem(BitmapArray2<Color> *arr, int i) : arr(arr), i(i) { }
  virtual int SizeX() const { return arr->SizeX(i); }
  virtual int SizeY() const { return arr->SizeY(i); }
  virtual Color Map(int x, int y) const
  {
    return arr->Map(i, x, y);
  }

private:
  BitmapArray2<Color> *arr;
  int i;
};

EXPORT GameApi::BM GameApi::BitmapApi::array_elem(BMA array, int i)
{
  BitmapArray2<Color> *arr = find_bitmap_array(e, array);
  return add_color_bitmap2(e, new BitmapArrayElem(arr, i));
}

EXPORT GameApi::BM GameApi::BitmapApi::alpha_color(BM orig, unsigned int color_key)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  ::Bitmap<Color> *b = new AlphaColorBitmap(*b2, color_key);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
}
class GradientBitmap2 : public Bitmap<Color>
{
public:
  GradientBitmap2(Point2d pos_1, Point2d pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy) : pos_1(pos_1), pos_2(pos_2), color_1(color_1), color_2(color_2), sx(sx), sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    Vector u = Point(x, y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    Vector u_x = Point(pos_2.x, pos_2.y, 0.0)-Point(pos_1.x, pos_1.y, 0.0);
    float val = Vector::DotProduct(u, u_x);
    val /= u_x.Dist();
    val /= u_x.Dist(); 
    // now [0.0 .. 1.0]
    if (val<0.0) return 0;
    if (val>1.0) return 0;
    return Color(Color::CubicInterpolate(color_1, color_2, val));
  }

private:
  Point2d pos_1;
  Point2d pos_2;
  unsigned int color_1;
  unsigned int color_2;
  int sx,sy;
};
EXPORT GameApi::BM GameApi::BitmapApi::gradient(PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2, int sx, int sy)
{
  Point *pos_1a = find_point(e, pos_1);
  Point *pos_2a = find_point(e, pos_2);
  Point2d pos_1b = { pos_1a->x, pos_1a->y };
  Point2d pos_2b = { pos_2a->x, pos_2a->y };
  return add_color_bitmap2(e, new GradientBitmap2(pos_1b, pos_2b, color_1, color_2, sx,sy));
}

class RadialGradient : public Bitmap<Color>
{
public:
  RadialGradient(int sx, int sy, Point2d pos, float r1, float r2, unsigned int color_1, unsigned int color_2) : sx(sx),sy(sy), pos(pos), r1(r1), r2(r2), color_1(color_1), color_2(color_2) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx-=pos.x;
    yy-=pos.y;
    float dist = sqrt(xx*xx+yy*yy);
    dist -= r1;
    dist /= (r2-r1);
    // now [0..1]
    if (dist<0.0) return 0;
    if (dist>1.0) return 0;
    return Color(Color::CubicInterpolate(color_1, color_2, dist));
  }
private:
  int sx,sy;
  Point2d pos;
  float r1,r2;
  unsigned int color_1, color_2;
};

EXPORT GameApi::BM GameApi::BitmapApi::radial_gradient(int sx, int sy, PT pos, float r1, float r2, unsigned int color_1, unsigned int color_2)
{
  Point *pos_p1 = find_point(e, pos);
  Point2d pos_p = { pos_p1->x, pos_p1->y };
   ::Bitmap<Color> *b = new RadialGradient(sx,sy,pos_p, r1,r2, color_1, color_2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
  return bm;
}

EXPORT GameApi::BM GameApi::BitmapApi::newbitmap(int sx, int sy, unsigned int color)
{
  ::Bitmap<Color> *b = new ConstantBitmap<Color>(Color(color), sx,sy);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

template<class T>
class BitmapTransformFromFunction : public Bitmap<T>
{
public:
  BitmapTransformFromFunction(Bitmap<T> &bm, std::function<T (int,int,T)> f) : bm(bm),  f(f) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual T Map(int x, int y) const
  {
    return f(x,y,bm.Map(x,y));
  }
private:
  Bitmap<T> &bm;
  std::function<T (int,int,T)> f; 
};

template<class T>
class BitmapFromFunction : public Bitmap<T>
{
public:
  BitmapFromFunction(std::function< T(int,int) > f, int sx, int sy) : f(f), sx(sx),sy(sy) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    return f(x,y);
  }
private:
  std::function<T (int,int)> f;
  int sx,sy;
};


EXPORT GameApi::BM GameApi::BitmapApi::transform(BM orig, std::function<unsigned int (int,int, unsigned int)> f)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  BitmapHandle *handle = find_bitmap(e, orig);
  Bitmap<Color> *c = find_color_bitmap(handle);
  UnsignedIntFromBitmap *bm1 = new UnsignedIntFromBitmap(*c);
  env->deletes.push_back(std::shared_ptr<void>(bm1));
  BitmapTransformFromFunction<unsigned int> *trans = new BitmapTransformFromFunction<unsigned int>(*bm1, f);
  env->deletes.push_back(std::shared_ptr<void>(trans));

  BitmapFromUnsignedInt *bm2 = new BitmapFromUnsignedInt(*trans);
  return add_color_bitmap(e, bm2);
}
EXPORT GameApi::BM GameApi::BitmapApi::function(std::function<unsigned(int,int)> f, int sx, int sy)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  Bitmap<unsigned int> *bm = new BitmapFromFunction<unsigned int>(f,sx,sy);  
  return add_color_bitmap(e, new BitmapFromUnsignedInt(*bm));
}

EXPORT GameApi::BM GameApi::BitmapApi::memoize(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  return add_color_bitmap(e, new MemoizeBitmap(*bitmap));
}

EXPORT GameApi::BM GameApi::BitmapApi::memoize_all(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  MemoizeBitmap *membitmap = new MemoizeBitmap(*bitmap);
  membitmap->MemoizeAll();
  return add_color_bitmap(e, membitmap);
}

EXPORT GameApi::BM GameApi::WaveformApi::waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  Waveform *m_wave = find_waveform(e, wave);
  return add_color_bitmap(e, new WaveformBitmap(*m_wave, 0.0, m_wave->Length(), m_wave->Min(), m_wave->Max(), sx, sy, Color(true_color), Color(false_color)));
}
class ChessBoardBitmap2 : public Bitmap<Color>
{
public:
  ChessBoardBitmap2(int tile_sx, int tile_sy, int count_x, int count_y, Color c1, Color c2) : tile_sx(tile_sx), tile_sy(tile_sy), count_x(count_x), count_y(count_y), c1(c1), c2(c2) { }
  int SizeX() const { return tile_sx*count_x; }
  int SizeY() const { return tile_sy*count_y; }
  Color Map(int x, int y) const
  {
    bool bb = false;
    int xx = x / count_x;
    int yy = y / count_y;
    if (xx & 1) { bb = !bb; }
    if (yy & 1) { bb = !bb; }
    if (bb) { return c1; }
    return c2;
  }
private:
  int tile_sx, tile_sy;
  int count_x, count_y;
  Color c1,c2;
};
EXPORT GameApi::BM GameApi::BitmapApi::chessboard(int tile_sx, int tile_sy, int count_x, int count_y, unsigned int c1, unsigned int c2)
{
  Bitmap<Color> *m = new ChessBoardBitmap2(tile_sx, tile_sy, count_x, count_y, Color(c1), Color(c2));
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = m;
  BM bm = add_bitmap(e, handle);
  return bm;
}
EXPORT GameApi::BM GameApi::BitmapApi::mandelbrot(bool julia,
		float start_x, float end_x,
		float start_y, float end_y,
		float xx, float yy,
		int sx, int sy,
		int count)
{
  ::Bitmap<int> *b = new Mandelbrot(julia, start_x, end_x,
				      start_y, end_y,
				      sx,sy,
				      count,
				      xx,yy);
  ::Bitmap<Color> *b2 = new MapBitmapToColor(0, count, Color(255,255,255), Color(0,128,255), *b);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  env->deletes.push_back(std::shared_ptr<void>(b));
  env->deletes.push_back(std::shared_ptr<void>(b2));
  ::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = m;
  BM bm = add_bitmap(e, handle);
  return bm;  
}

EXPORT GameApi::BM GameApi::BitmapApi::mandelbrot2(bool julia,
		float start_x, float end_x,
		float start_y, float end_y,
		float xx, float yy,
		int sx, int sy,
		int count)
{
  ::Bitmap<int> *b = new Mandelbrot(julia, start_x, end_x,
				      start_y, end_y,
				      sx,sy,
				      count,
				      xx,yy);
  ::Bitmap<Color> *b2 = new MapBitmapToColor(0, count, Color(255,255,255), Color(0,128,255), *b);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  env->deletes.push_back(std::shared_ptr<void>(b));
  //env->deletes.push_back(std::shared_ptr<void>(b2));
  //::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b2;
  BM bm = add_bitmap(e, handle);
  return bm;  
}

EXPORT GameApi::BM GameApi::BitmapApi::newtilebitmap(int sx, int sy, int tile_sx, int tile_sy)
{
  ::Bitmap<Color> *b = new ConstantBitmap<Color>(Color(0,0,0), sx,sy);
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = b;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

BufferRef LoadImage(std::string filename, bool &success);

EXPORT void GameApi::BitmapApi::savebitmap(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);
  PpmFile file(filename, *bm2);
  std::string pngcontents = file.Contents();
  std::ofstream filehandle(filename.c_str(), std::ios_base::out);
  filehandle << pngcontents;
  filehandle.close();
}

EXPORT GameApi::BM GameApi::BitmapApi::loadbitmap(std::string filename)
{

  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  if (b==false) {
    img = BufferRef::NewBuffer(10,10);
    for(int x=0;x<10;x++)
      for(int y=0;y<10;y++)
	{
	  img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	}
    std::cout << "ERROR: File not found: " << filename << std::endl;
  }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));

  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = buf;
  BM bm = add_bitmap(e, handle);
  return bm;
}
EXPORT GameApi::BM GameApi::BitmapApi::loadtilebitmap(std::string filename, int sx, int sy)
{

  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));
  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = buf;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
}


EXPORT GameApi::BM GameApi::BitmapApi::loadposbitmap(std::string filename)
{
  BitmapCircle *circle = new BitmapCircle(Point2d::NewPoint(30.0,30.0), 30.0, 60,60);
  ColorMap2 *f = new ColorMap2;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(f));
  env->deletes.push_back(std::shared_ptr<void>(circle));
  MapBitmap<bool, Pos> *map = new MapBitmap<bool,Pos>(*circle,*f);
  BitmapPosHandle *handle = new BitmapPosHandle;
  handle->bm = map;
  BM bm = add_bitmap(e, handle);
  return bm;
  
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


EXPORT void GameApi::GridApi::preparegrid(GameApi::BM tile_bitmap, int tile_choose)
{
  GridPriv *p = (GridPriv*)priv;
  p->last_id = tile_bitmap.id;
  BitmapHandle *handle = find_bitmap(e, tile_bitmap);
  int sx = 1;
  int sy = 1;
  BitmapTileHandle *chandle = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle)
    {
      sx = chandle->tile_sx;
      sy = chandle->tile_sy;
    }
  
  p->cellsx[tile_bitmap.id] = sx;
  p->cellsy[tile_bitmap.id] = sy;

  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  if (!bitmap) { std::cout << "preparegrid bitmap==NULL" << std::endl; return; }

  ArrayRender *rend = &((GridPriv*)priv)->rend;
  PrepareGrid(*bitmap, sx, sy, *rend);
}

EXPORT void GameApi::GridApi::rendergrid(GameApi::BM grid, int grid_choose, float top_x, float top_y)
{
  GridPriv *p = (GridPriv*)priv;
  int cellsx = p->cellsx[p->last_id];
  int cellsy = p->cellsy[p->last_id];
  
  BitmapHandle *handle = find_bitmap(e, grid);
  ::Bitmap<Pos> *bitmap = find_pos_bitmap(handle);
  if (!bitmap) { std::cout << "rendergrid bitmap==NULL" << std::endl; return; }
  RenderGrid(*bitmap, top_x, top_y, cellsx, cellsy, p->rend, 0,0, 10,10);
}

EXPORT GameApi::BM GameApi::BitmapApi::findtile(GameApi::BM tile_bitmap, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, tile_bitmap);
  BitmapTileHandle *chandle = dynamic_cast<BitmapTileHandle*>(handle);
  int sx = chandle->tile_sx;
  int sy = chandle->tile_sy;
  return subbitmap(tile_bitmap, sx*x, sy*y, sx, sy);
}
template<class T>
Bitmap<T> *subbitmap_t(Bitmap<T> *bm, int x, int y, int width, int height)
{
  Bitmap<T> *sub = new SubBitmap<T>(*bm, x,y,width, height);
  return sub;
}

BitmapHandle *subbitmap_h(BitmapHandle *handle, int x, int y, int width, int height)
{
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle1)
    { // color
      Bitmap<Color> *c = subbitmap_t(chandle1->bm, x,y,width,height);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle2)
    { // int
      Bitmap<int> *c = subbitmap_t(chandle2->bm, x,y,width,height);
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = subbitmap_h(h, x,y,width,height);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }
  if (chandle4)
    { // pos
      Bitmap<Pos> *c = subbitmap_t(chandle4->bm, x,y,width,height);
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle5)
    { // tile
      Bitmap<Color> *c = subbitmap_t(chandle5->bm, x,y,width,height);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  std::cout << "subbitmap" << std::endl;
  return 0;
}

GameApi::BM GameApi::BitmapApi::subbitmap(GameApi::BM orig, int x, int y, int width, int height)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = subbitmap_h(handle, x,y,width,height);
  BM bm = add_bitmap(e, handle2);
  return bm;

}
template<class T>
Bitmap<T> *growbitmap_t(Bitmap<T> *bm, int l, int t, int r, int b,
			T tt)
{
  Bitmap<T> *grow = new GrowBitmap<T>(*bm, l,t,r,b, tt);
  return grow;
} 
BitmapHandle *growbitmap_h(BitmapHandle *handle, int l, int t, int r, int b)
{
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle1)
    { // color
      Color elem(0,0,0,0);
      Color *c = (Color*)&elem;
      Bitmap<Color> *cbm = growbitmap_t(chandle1->bm, l,t,r,b,*c);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle2)
    { // int
      int elem = 0;
      int *c = (int*)&elem;
      Bitmap<int> *cbm = growbitmap_t(chandle2->bm, l,t,r,b,*c);
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = growbitmap_h(h, l,t,r,b);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }
  if (chandle4)
    { // pos
      Pos elem = { 0,0 };
      Pos *c = (Pos*)&elem;
      Bitmap<Pos> *cbm = growbitmap_t(chandle4->bm, l,t,r,b,*c);
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = cbm;
      return handle;
    }
  if (chandle5)
    { // tile
      Color elem(0,0,0,0);
      Color *c = (Color*)&elem;
      Bitmap<Color> *cbm = growbitmap_t(chandle5->bm, l,t,r,b,*c);
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = cbm;
      return handle;
    }
  std::cout << "growbitmap" << std::endl;
  return 0;
}

EXPORT GameApi::BM GameApi::BitmapApi::growbitmap(GameApi::BM orig, int l, int t, int r, int b)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = growbitmap_h(handle, l,t,r,b);
  BM bm = add_bitmap(e, handle2);
  return bm;

}

BitmapHandle *modify_bitmap_h(BitmapHandle *orig, BitmapHandle *bm, int x, int y)
{
  BitmapColorHandle *chandle1 = dynamic_cast<BitmapColorHandle*>(orig);
  BitmapIntHandle *chandle2 = dynamic_cast<BitmapIntHandle*>(orig);
  BitmapArrayHandle *chandle3 = dynamic_cast<BitmapArrayHandle*>(orig);
  BitmapPosHandle *chandle4 = dynamic_cast<BitmapPosHandle*>(orig);
  //BitmapTileHandle *chandle5 = dynamic_cast<BitmapTileHandle*>(handle);
  if (chandle1)
    { // color
      BitmapColorHandle *ahandle1 = dynamic_cast<BitmapColorHandle*>(bm);
      Bitmap<Color> *c = new ModifyBitmap<Color>(*chandle1->bm, *ahandle1->bm, x,y); 
      BitmapColorHandle *handle = new BitmapColorHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle2)
    { // color
      BitmapIntHandle *ahandle1 = dynamic_cast<BitmapIntHandle*>(bm);
      Bitmap<int> *c = new ModifyBitmap<int>(*chandle2->bm, *ahandle1->bm, x,y); 
      BitmapIntHandle *handle = new BitmapIntHandle;
      handle->bm = c;
      return handle;
    }
  if (chandle3)
    { // array
      int size = chandle3->vec.size();
      BitmapArrayHandle *nhandle = new BitmapArrayHandle;
      for(int i=0;i<size;i++)
	{
	  BitmapHandle *h = chandle3->vec[i];
	  BitmapHandle *h2 = modify_bitmap_h(h, bm, x,y);
	  nhandle->vec.push_back(h2);
	  nhandle->owned.push_back(1);
	}
      return nhandle;
    }

  if (chandle4)
    { // pos
      BitmapPosHandle *ahandle1 = dynamic_cast<BitmapPosHandle*>(bm);
      Bitmap<Pos> *c = new ModifyBitmap<Pos>(*chandle4->bm, *ahandle1->bm, x,y); 
      BitmapPosHandle *handle = new BitmapPosHandle;
      handle->bm = c;
      return handle;
    }
  std::cout << "modify_bitmap_h" << std::endl;
  return 0;
}

EXPORT GameApi::BM GameApi::BitmapApi::modify_bitmap(GameApi::BM orig, BM bm, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = find_bitmap(e, bm);
  BitmapHandle *h = modify_bitmap_h(handle, handle2, x,y);
  BM bbm = add_bitmap(e,h);
  return bbm;
}
EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  return add_color_bitmap(e, new BlitBitmapClass(*bm1, *bm2, x,y));
}

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, FB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  FloatBitmap *handle3 = find_float_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<float> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked(*bm1, *bm2, x,y, *bm3));
}

EXPORT GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, BB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  BoolBitmap *handle3 = find_bool_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<bool> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked2(*bm1, *bm2, x,y, *bm3));
}

EXPORT int GameApi::BitmapApi::intvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapIntHandle *handle2 = dynamic_cast<BitmapIntHandle*>(handle);
  if (handle2)
    {
      return handle2->bm->Map(x,y);
    }
  return 0;
}

EXPORT int GameApi::BitmapApi::size_x(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *handle3 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *handle4 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *handle5 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *handle6 = dynamic_cast<BitmapTileHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeX();
  if (handle3)
    return handle3->bm->SizeX();
  if (handle4)
    return dynamic_cast<BitmapColorHandle*>(handle4->vec[0])->bm->SizeX();
  if (handle5)
    return handle5->bm->SizeX();
  if (handle6)
    return handle6->bm->SizeX();
  return 0;
}
EXPORT int GameApi::BitmapApi::size_y(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);

  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  BitmapIntHandle *handle3 = dynamic_cast<BitmapIntHandle*>(handle);
  BitmapArrayHandle *handle4 = dynamic_cast<BitmapArrayHandle*>(handle);
  BitmapPosHandle *handle5 = dynamic_cast<BitmapPosHandle*>(handle);
  BitmapTileHandle *handle6 = dynamic_cast<BitmapTileHandle*>(handle);
  if (handle2)
    return handle2->bm->SizeY();
  if (handle3)
    return handle3->bm->SizeY();
  if (handle4)
    return dynamic_cast<BitmapColorHandle*>(handle4->vec[0])->bm->SizeY();
  if (handle5)
    return handle5->bm->SizeY();
  if (handle6)
    return handle6->bm->SizeY();
  return 0;

}

EXPORT unsigned int GameApi::BitmapApi::colorvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  if (handle2)
    {
      return handle2->bm->Map(x,y).Pixel();
    }
  return 0;
}


EXPORT GameApi::BM GameApi::BitmapApi::interpolate_bitmap(GameApi::BM orig1, GameApi::BM orig2, float x)
{
  BitmapHandle *handle1 = find_bitmap(e, orig1);
  BitmapHandle *handle2 = find_bitmap(e, orig2);
  BitmapColorHandle *newhandle = new BitmapColorHandle;
  BitmapColorHandle *h1 = dynamic_cast<BitmapColorHandle*>(handle1);
  BitmapColorHandle *h2 = dynamic_cast<BitmapColorHandle*>(handle2);
  newhandle->bm = 0;
  if (h1&&h2)
    {
      newhandle->bm = new InterpolateBitmap(*h1->bm, *h2->bm, x);
    }
  else
    {
      std::cout << "Only COlorbitmaps work" << std::endl;
    }
  BM bbm = add_bitmap(e,newhandle);
  return bbm;
}

template<class T>
class BitmapFromString : public Bitmap<T>
{
public:
  BitmapFromString(char *array, int sx, int sy, std::function<T (char)> f) : array(array), sx(sx), sy(sy),  f(f) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  T Map(int x, int y) const
  {
    return f(array[x+y*sx]);
  }
private:
  char *array;
  int sx;
  int sy;
  std::function<T (char)> f;
};


EXPORT GameApi::BM GameApi::BitmapApi::newintbitmap(char *array, int sx, int sy, std::function<int (char ch)> f )
{
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  BitmapIntHandle *handle = new BitmapIntHandle;
  handle->bm = new BitmapFromString<int>(array, sx, sy, f);
  return add_bitmap(e, handle);
}

EXPORT GameApi::BM GameApi::BitmapApi::newcolorbitmap(char *array, int sx, int sy, std::function<unsigned int (char)> f)
{
  //EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  BitmapColorHandle *handle = new BitmapColorHandle;
  ::Bitmap<unsigned int> *bm = new BitmapFromString<unsigned int>(array, sx, sy, f);
  env->deletes.push_back(std::shared_ptr<void>(bm));
  handle->bm = new BitmapFromUnsignedInt(*bm);
  return add_bitmap(e, handle);
}



EXPORT GameApi::BM GameApi::BitmapApi::anim_array(GameApi::BM *bitmaparray, int size)
{
  int s = size;
  BitmapArrayHandle *handle = new BitmapArrayHandle;
  for(int i=0;i<s;i++)
    {
      GameApi::BM bm = bitmaparray[i];
      BitmapHandle *h = find_bitmap(e, bm);
      //std::cout << "bitmaparray: " << h << std::endl;
      handle->vec.push_back(h);
      handle->owned.push_back(0);

    }
  BM bm = add_bitmap(e, handle);
  return bm;
}

#if 0
EXPORT GameApi::BM GameApi::BitmapApi::bitmapandtypes(BM bm, BM (*fptr)(GameApi::EveryApi &ev,int))
{
  BitmapHandle *h = find_bitmap(e, bm);
  BitmapIntHandle *hh = dynamic_cast<BitmapIntHandle*>(h);
  if (!hh) { std::cout << "bm for bitmapandtypes() is not bitmap of integers" << std::endl; return; }
  
}
#endif
EXPORT GameApi::IS GameApi::AnimApi::single(int val, float duration)
{
  AnimImpl i;
  i.wave_int = new SingleAnimInt(val, duration);
  i.wave_point = 0;
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

EXPORT GameApi::IS GameApi::AnimApi::single(PT point, float duration)
{
  Point *p = find_point(e,point);
  Point pp(p->x, p->y, p->z);

  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = new SingleAnimPoint(pp, duration);
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;  
}
EXPORT GameApi::IS GameApi::AnimApi::single(float val, float duration)
{
  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = 0;
  i.wave_float = new SingleAnimFloat(val, duration);
  IS is = add_anim(e,i);
  return is;
}

EXPORT GameApi::IS GameApi::AnimApi::line(int val1, int val2, float duration)
{
  AnimImpl i;
  i.wave_int = new LineAnimInt(val1,val2, duration);
  i.wave_point = 0;
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

EXPORT GameApi::IS GameApi::AnimApi::line(PT p1, PT p2, float duration)
{
  Point *p = find_point(e,p1);
  Point pp(p->x, p->y, p->z);
  Point *p2x = find_point(e,p2);
  Point pp2(p2x->x, p2x->y, p2x->z);

  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = new LineAnimPoint(pp, pp2, duration);
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;  
}
EXPORT GameApi::IS GameApi::AnimApi::line(float val1, float val2, float duration)
{
  AnimImpl i;
  i.wave_int =0;
  i.wave_point = 0;
  i.wave_float = new LineAnimFloat(val1, val2, duration);
  IS is = add_anim(e,i);
  return is;
}


class AnimArrayInt : public AnimArray<AnimInt, int>
{
public:
  AnimArrayInt(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimInt,int>(e, array, size) { }
  AnimInt *get(const AnimImpl &c) const { return c.wave_int; }
};
class AnimArrayPoint : public AnimArray<AnimPoint3d, Point>
{
public:
  AnimArrayPoint(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimPoint3d,Point>(e, array, size) { }
  AnimPoint3d *get(const AnimImpl &c) const { return c.wave_point; }
};
class AnimArrayFloat : public AnimArray<AnimFloat, float>
{
public:
  AnimArrayFloat(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimFloat,float>(e, array, size) { }
  AnimFloat *get(const AnimImpl &c) const { return c.wave_float; }
};


class AnimRepeatInt : public AnimRepeat<AnimInt, int>
{
public:
  AnimRepeatInt(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimInt,int>(e, val, count) { }
  AnimInt *get(const AnimImpl &c) const { return c.wave_int; }
};
class AnimRepeatPoint : public AnimRepeat<AnimPoint3d, Point>
{
public:
  AnimRepeatPoint(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimPoint3d,Point>(e, val, count) { }
  AnimPoint3d *get(const AnimImpl &c) const { return c.wave_point; }
};
class AnimRepeatFloat : public AnimRepeat<AnimFloat, float>
{
public:
  AnimRepeatFloat(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimFloat,float>(e, val, count) { }
  AnimFloat *get(const AnimImpl &c) const { return c.wave_float; }
};




EXPORT GameApi::IS GameApi::AnimApi::seq_line(IS *array, int size)
{
  AnimImpl i;
  i.wave_int = new AnimArrayInt(e, array, size);
  i.wave_point = new AnimArrayPoint(e, array, size);
  i.wave_float = new AnimArrayFloat(e, array, size);
  IS is = add_anim(e,i);
  return is;
} 
EXPORT GameApi::IS GameApi::AnimApi::repeat(IS i, int count)
{
  AnimImpl ii;
  ii.wave_int = new AnimRepeatInt(e, i, count);
  ii.wave_point = new AnimRepeatPoint(e, i, count);
  ii.wave_float = new AnimRepeatFloat(e, i, count);
  IS is = add_anim(e,ii);
  return is;
}
EXPORT GameApi::IS GameApi::AnimApi::repeat_infinite(IS i)
{
  AnimImpl ii;
  ii.wave_int = new AnimRepeatInt(e, i, -1);
  ii.wave_point = new AnimRepeatPoint(e, i, -1);
  ii.wave_float = new AnimRepeatFloat(e, i, -1);
  IS is = add_anim(e,ii);
  return is;
}


GameApi::PT GameApi::SpaceApi::pos(SP spa, float x, float y, float z)
{
  SpaceImpl *sp = find_space(e, spa);
  x /= (sp->tr - sp->tl).Dist();
  y /= (sp->bl - sp->tl).Dist();
  z /= (sp->z - sp->tl).Dist();
  Vector u_x = sp->tr - sp->tl;
  Vector u_y = sp->bl - sp->tl;
  Vector u_z = sp->z - sp->tl;
  Point p = sp->tl + x*u_x + y*u_y + z*u_z;
  return add_point(e, p.x, p.y, p.z);
}
GameApi::PT GameApi::SpaceApi::posN(SP spa, float x, float y, float z)
{
  SpaceImpl *sp = find_space(e, spa);
  Vector u_x = sp->tr - sp->tl;
  Vector u_y = sp->bl - sp->tl;
  Vector u_z = sp->z - sp->tl;
  Point p = sp->tl + x*u_x + y*u_y + z*u_z;
  return add_point(e, p.x, p.y, p.z);
}
float GameApi::SpaceApi::pt_x(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->x;
}
float GameApi::SpaceApi::pt_y(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->y;
}
float GameApi::SpaceApi::pt_z(PT p)
{
  Point *pp = find_point(e,p);
  if (!pp) return 0.0;
  return pp->z;
}

EXPORT int GameApi::AnimApi::timed_value_repeat_num(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimInt *wave = ii.wave_int;
  AnimPoint3d *wave2 = ii.wave_point;
  AnimFloat *wave3 = ii.wave_float;
  if (wave && wave->Duration()>0.01) 
    {
      int val = time / wave->Duration();
      return val;
    }
  if (wave2 && wave2->Duration()>0.01) 
    {
      int val = time / wave2->Duration();
      return val;
    }
  if (wave3 && wave3->Duration()>0.01)
    {
      int val = time / wave3->Duration();
      return val;
    }
  std::cout << "timed_value_repeat_num ERROR" << std::endl;
  return 0;
}

EXPORT int GameApi::AnimApi::timed_value(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimInt *wave = ii.wave_int;
  if (!wave) return 0;
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  return wave->Index(0, time);
}

EXPORT GameApi::PT GameApi::AnimApi::timed_value_point(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimPoint3d *wave = ii.wave_point;
  if (!wave) return add_point(e, 0.0, 0.0);
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  Point p = wave->Index(0, time); 
  return add_point(e, p.x, p.y,p.z);
}
EXPORT float GameApi::AnimApi::timed_value_float(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimFloat *wave = ii.wave_float;
  if (!wave) return 0.0;
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  float p = wave->Index(0, time); 
  return p;
}


EXPORT GameApi::TextApi::~TextApi()
{
  delete(std::vector<BM>*)priv;
}

EXPORT void GameApi::TextApi::load_font(std::string filename, int sx, int sy, int x, int y, char start_char, char end_char)
{
  this->sx = sx;
  this->sy = sy;
  this->start_char = start_char;
  this->end_char = end_char;
  std::vector<BM> *bms = new std::vector<BM>();
  priv = (void*)bms;
  BM bm_all = bm.loadtilebitmap(filename, sx,sy);
  for(int yy=0;yy<y;yy++)
    {
      for(int xx=0;xx<x;xx++)
	{
	  BM o = bm.findtile(bm_all, xx,yy);
	  //std::cout << "Text" << o.id << std::endl;
	  sp.preparesprite(o);
	  bms->push_back(o);
	}
    }
}
EXPORT void GameApi::TextApi::draw_text(std::string text, int x, int y, SH sh)
{
  int xpos = x;
  int ypos = y;
  int s = text.size();
  for(int i=0;i<s;i++)
    {
      char c = text[i];
      if (c!='\n' && c>=start_char && c<=end_char)
	{
	  BM b = ((std::vector<BM>*)priv)->operator[](c-start_char);
	  //std::cout << "draw_text" << b.id << std::endl;
	  sp.rendersprite(b, sh, xpos, ypos, 1.0, 1.0);
	}
      xpos += sx;
      if (c=='\n') { xpos=x; ypos+=sy; }
    }
}

EXPORT GameApi::PolygonApi::PolygonApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new PolyPriv;
}

EXPORT GameApi::PolygonApi::~PolygonApi()
{
  delete(PolyPriv*)priv;
}
GameApi::SurfaceApi::SurfaceApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new SurfPriv;
}
GameApi::SurfaceApi::~SurfaceApi()
{
  delete(PolyPriv*)priv;
}
GameApi::S GameApi::SurfaceApi::bitmapsphere(PT center, float radius0, float radius1, BM height)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  Point *pp1 = find_point(e,center);
  BitmapHandle *handle = find_bitmap(e, height);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  ContinuousBitmapFromBitmap<Color> *contbm = new ContinuousBitmapFromBitmap<Color>(*bitmap, 1.0,1.0);
  ContinuousColorBitmapToFloatBitmap *floatbm = new ContinuousColorBitmapToFloatBitmap(*contbm);
  env->deletes.push_back(std::shared_ptr<void>(floatbm));
  env->deletes.push_back(std::shared_ptr<void>(contbm));
  BitmapSphere *sphere = new BitmapSphere(*floatbm, *pp1, radius0, radius1);
  SurfaceImpl i;
  i.surf = sphere;
  return add_surface(e, i);
}
EXPORT GameApi::P GameApi::PolygonApi::from_polygon(P p, std::function<P (int face, float p1_x, float p1_y, float p1_z, float p2_x, float p2_y, float p2_z, float p3_x, float p3_y, float p3_z, float p4_x, float p4_y, float p4_z)> f)
{
  return from_polygon_1(p,f);
}
GameApi::P GameApi::PolygonApi::from_polygon_1(P p, std::function<P (int face, float p1_x, float p1_y, float p1_z, float p2_x, float p2_y, float p2_z, float p3_x, float p3_y, float p3_z, float p4_x, float p4_y, float p4_z)> f)
{
  FaceCollPolyHandle *handle = find_poly(e, p);
  FaceCollection *coll = handle->coll;
  int s = coll->NumFaces();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point p1 = coll->FacePoint(i, 0);
      Point p2 = coll->FacePoint(i, 1);
      Point p3 = coll->FacePoint(i, 2);
      Point p4 = coll->NumPoints(i)>3 ? coll->FacePoint(i, 3) : coll->FacePoint(i,2);
      P p = f(i, p1.x,p1.y,p1.z,
	      p2.x,p2.y,p2.z,
	      p3.x,p3.y,p3.z,
	      p4.x,p4.y,p4.z);
      vec.push_back(p);
    }
  P pp = or_array(&vec[0], s);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_lines(LI li, std::function<P (int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)> f)
{
  LineCollection *coll = find_line_array(e, li);
  int count = coll->NumLines();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point p1 = coll->LinePoint(i, 0);
      Point p2 = coll->LinePoint(i, 1);
      unsigned int c1 = coll->LineColor(i, 0);
      unsigned int c2 = coll->LineColor(i, 1);
      P item = f(i,p1.x,p1.y,p1.z, p2.x,p2.y,p2.z,c1,c2);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_points(PTS pts, std::function<P (int i, float x, float y, float z, unsigned int color)> f)
{
  PointsApiPoints *p = find_pointsapi_points(e, pts);
  int count = p->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point pos = p->Pos(i);
      unsigned int color = p->Color(i);
      P item = f(i, pos.x,pos.y,pos.z,color);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_voxel(std::function<P (unsigned int c)> f, VX voxel, float dx, float dy, float dz)
{
  Voxel<unsigned int> *vox = find_voxel(e, voxel);
  int sx = vox->SizeX();
  int sy = vox->SizeY();
  int sz = vox->SizeZ();
  std::vector<P> vec_x;
  vec_x.reserve(sx);
  for(int x=0;x<sx;x++)
    {
      std::vector<P> vec_y;
      vec_y.reserve(sy);
      for(int y=0;y<sy;y++)
	{
	  std::vector<P> vec_z;
	  vec_z.reserve(sz);
	  for(int z=0;z<sz;z++)
	    {
	      unsigned int c = vox->Map(x,y,z);
	      unsigned int i = c;
	      P p = f(i);
	      P p2 = translate_1(p, 0.0,0.0,z*dz);
	      vec_z.push_back(p2);
	    }
	  P p = or_array_1(&vec_z[0], sz);
	  P p2 = translate_1(p, 0.0,y*dy,0.0);
	  vec_y.push_back(p2);
	}
      P p = or_array_1(&vec_y[0], sy);
      P p2 = translate_1(p, x*dx,0.0,0.0);
      vec_x.push_back(p2);
    }
  P p = or_array_1(&vec_x[0],sx);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap2(EveryApi &ev, std::function<P (int c, PT tl, PT tr, PT bl, PT br)> f, BM int_bm, FB float_bm, float dx, float dz, float height)
{
  BitmapIntHandle *handle = dynamic_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  Bitmap<int> *bm = handle->bm;
  Bitmap<float> *fbm = find_float_bitmap(e, float_bm)->bitmap;
  std::vector<P> vec;
  int sx = std::min(fbm->SizeX(),bm->SizeX());
  int sy = std::min(fbm->SizeY(),bm->SizeY());
  for(int y=0;y<sy-1;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx-1;x++)
	{
	  float height_1 = fbm->Map(x,y);
	  float height_2 = fbm->Map(x+1,y);
	  float height_3 = fbm->Map(x,y+1);
	  float height_4 = fbm->Map(x+1,y+1);

	  PT pos_1 = ev.point_api.point(0.0, height_1*height, 0.0);
	  PT pos_2 = ev.point_api.point(dx, height_2*height, 0.0);
	  PT pos_3 = ev.point_api.point(0.0, height_3*height, dz);
	  PT pos_4 = ev.point_api.point(dx, height_4*height, dz);

	  P p = f(bm->Map(x,y), pos_1, pos_2, pos_3, pos_4);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap(std::function<P (int c)> f, BM int_bm, float dx, float dz)
{
  BitmapIntHandle *handle = dynamic_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  Bitmap<int> *bm = handle->bm;
  std::vector<P> vec;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  P p = f(bm->Map(x,y));
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}

EXPORT GameApi::P GameApi::PolygonApi::triangle(PT p1, PT p2, PT p3)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  FaceCollection *coll = new TriElem(*pp1, *pp2, *pp3);
  return add_polygon(e, coll,1);
}
class UnitFaceColl : public ForwardFaceCollection
{
public:
  UnitFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};
EXPORT GameApi::P GameApi::PolygonApi::unit_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1), 1);
}
class UnitToFaceColl : public ForwardFaceCollection
{
public:
  UnitToFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitToCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};

EXPORT GameApi::P GameApi::PolygonApi::unit_to_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitToFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1),1);
}

class UnitToFlexClass : public ForwardFaceCollection
{
public:
  UnitToFlexClass(FaceCollection *coll, Point bTL, Point bTR, Point bBL, Point bBR,
	     Point fTL, Point fTR, Point fBL, Point fBR) 
    : ForwardFaceCollection(*coll), bTL(bTL), bTR(bTR), bBL(bBL), bBR(bBR),
      fTL(fTL), fTR(fTR), fBL(fBL), fBR(fBR)
  { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face, point);
    Point pp = UnitToFlex(p, bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR);
    return pp;
  }
private:
  Point bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR;
};

EXPORT GameApi::P GameApi::PolygonApi::unit_to_flex(P orig, 
					     PT bTL, PT bTR, PT bBL, PT bBR,
					     PT fTL, PT fTR, PT fBL, PT fBR)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *bTL1 = find_point(e, bTL);
  Point *bTR1 = find_point(e, bTR);
  Point *bBL1 = find_point(e, bBL);
  Point *bBR1 = find_point(e, bBR);

  Point *fTL1 = find_point(e, fTL);
  Point *fTR1 = find_point(e, fTR);
  Point *fBL1 = find_point(e, fBL);
  Point *fBR1 = find_point(e, fBR);
  return add_polygon(e, new UnitToFlexClass(coll, *bTL1, *bTR1, *bBL1, *bBR1,
				       *fTL1, *fTR1, *fBL1, *fBR1),1);
}


EXPORT GameApi::P GameApi::PolygonApi::empty()
{
  return add_polygon(e,new EmptyBoxableFaceCollection, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::load_model(std::string filename, int num)
{
  return add_polygon2(e, new LoadObjModelFaceCollection(filename, num), 1);
} 
EXPORT void GameApi::PolygonApi::save_model(GameApi::P poly, std::string filename)
{
  FaceCollection *face = find_facecoll(e, poly);
  SaveObjModelFaceCollection save(face);
  save.save(filename);
}
   
#if 0
EXPORT GameApi::P GameApi::PolygonApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p2);
  FaceCollection *coll = new TriElem(*pp1, *pp2, (*pp3)+Vector(1.0,1.0,1.0));
  return add_polygon(e, coll,1);
}
#endif
class TexCoordQuadFaceCollection : public ForwardFaceCollection
{
public:
  TexCoordQuadFaceCollection(FaceCollection &coll, TexCoordQuad q_, int sx, int sy) : ForwardFaceCollection(coll), q(q_) //,sx(sx),sy(sy) 
  {
    q.p1.x /= float(sx);
    q.p1.y /= float(sy);
    q.p2.x /= float(sx);
    q.p2.y /= float(sy);
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
  }
  virtual Point2d TexCoord(int face, int point) const 
  { 
    //std::cout << "TexCoord" << face << " " << point << std::endl;
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
    if (point==0) {
      return q.p1;
    }
    if (point==1) {
      Point2d p;
      p.x = q.p2.x;
      p.y = q.p1.y;
      return p;
    }
    if (point==2) {
      return q.p2;
    }
    if (point==3) {
      Point2d p;
      p.x = q.p1.x;
      p.y = q.p2.y;
      return p;
    }
    Point2d pp;
    pp.x = 0.0;
    pp.y = 0.0;
    return pp;
  }
private:
  TexCoordQuad q;
  //int sx,sy;
};

class TexCoordSpherical : public ForwardFaceCollection
{
public:
  TexCoordSpherical(Point center, FaceCollection *coll) : ForwardFaceCollection(*coll), center(center), coll(coll) { }
    virtual Point2d TexCoord(int face, int point) const { 
      Point p = ForwardFaceCollection::FacePoint(face,point);
      p-=Vector(center);
      float r = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
      float alfa = acos(p.z/r);
      float beta = atan2(p.y,p.x);
      alfa/=2.0*3.14159;
      beta/=2.0*3.14159;
      Point2d pp;
      pp.x = alfa;
      pp.y = beta;
      return pp;
    }
private:
  Point center;
  FaceCollection *coll;
};

class TexCoordCylindar : public ForwardFaceCollection
{
public:
  TexCoordCylindar(FaceCollection *coll, float start_y, float end_y) : ForwardFaceCollection(*coll), coll(coll), start_y(start_y), end_y(end_y) { }
    virtual Point2d TexCoord(int face, int point) const { 
      Point p = ForwardFaceCollection::FacePoint(face,point);
      //float r = sqrt(p.x*p.x+p.z*p.z);
      float alfa = atan2(p.z, p.x);
      alfa /= 2.0*3.14159;
      float z = p.y - start_y;
      z/=(end_y-start_y);
      Point2d pp;
      pp.x = alfa;
      pp.y = z;
      return pp;
    }
private:
  FaceCollection *coll;
  float start_y, end_y;
};
class TexCoordManual : public ForwardFaceCollection
{
public:
  TexCoordManual(FaceCollection *coll,
		 float p1_x, float p1_y,
		 float p2_x, float p2_y,
		 float p3_x, float p3_y,
		 float p4_x, float p4_y) : ForwardFaceCollection(*coll),
					   p1_x(p1_x), p1_y(p1_y),
					   p2_x(p2_x), p2_y(p2_y),
					   p3_x(p3_x), p3_y(p3_y),
					   p4_x(p4_x), p4_y(p4_y) { }

  Point2d TexCoord(int face, int point) const
  {
    switch(point) {
    case 0:
      {
      Point2d p1 = { p1_x, p1_y };
      return p1;
      }
    case 1:
      {
      Point2d p2 = { p2_x, p2_y };
      return p2;
      }
    case 2:
      {
      Point2d p3 = { p3_x, p3_y };
      return p3;
      }
    case 3:
      {
      Point2d p4 = { p4_x, p4_y };
      return p4;
      }
    };
    Point2d pp = { 0.0, 0.0 };
    return pp;
  }
private:
  float p1_x, p1_y;
  float p2_x, p2_y;
  float p3_x, p3_y;
  float p4_x, p4_y;
					   
};
EXPORT GameApi::P GameApi::PolygonApi::texcoord_manual(P orig,
						float p1_x, float p1_y,
						float p2_x, float p2_y,
						float p3_x, float p3_y,
						float p4_x, float p4_y)
{
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon(e, new TexCoordManual(face, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_spherical(PT center, P orig)
{
  Point *center_1 = find_point(e, center);
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon(e, new TexCoordSpherical(*center_1, face),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_cylindar(P orig, float start_y, float end_y)
{
  FaceCollection *face = find_facecoll(e, orig);
  return add_polygon(e, new TexCoordCylindar(face,start_y, end_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, TX tx, int id)
{
  TextureApi t(e);
  Q q = t.get_tex_coord_1(tx, id);
  return sprite_bind_1(p, q, tx);
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, Q q, TX tx)
{
  return sprite_bind_1(p,q,tx);
}
GameApi::P GameApi::PolygonApi::sprite_bind_1(P p, Q q, TX tx)
{
  TextureI *texture = find_texture(e, tx);
  int sx = texture->SizeX();
  int sy = texture->SizeY();
  FaceCollection *pp1 = find_facecoll(e, p);
  TexCoordQuad qq = find_tex_quad(e, q);
  return add_polygon(e, new TexCoordQuadFaceCollection(*pp1, qq, sx,sy),1);
}

EXPORT GameApi::P GameApi::PolygonApi::quad(PT p1, PT p2, PT p3, PT p4)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  Point *pp4 = find_point(e, p4);
  FaceCollection *coll = new QuadElem(*pp1, *pp2, *pp3, *pp4);
  return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_x(float x,
		  float y1, float y2,
		  float z1, float z2)
{
  Point pp1 = Point(x,y1,z1);
  Point pp2 = Point(x,y2,z1);
  Point pp3 = Point(x,y2,z2);
  Point pp4 = Point(x,y1,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_y(float x1, float x2,
		  float y,
		  float z1, float z2)
{
  Point pp1 = Point(x1,y,z1);
  Point pp2 = Point(x2,y,z1);
  Point pp3 = Point(x2,y,z2);
  Point pp4 = Point(x1,y,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_z(float x1, float x2,
		  float y1, float y2,
		  float z)
{
  Point pp1 = Point(x1,y1,z);
  Point pp2 = Point(x2,y1,z);
  Point pp3 = Point(x2,y2,z);
  Point pp4 = Point(x1,y2,z);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}



EXPORT GameApi::P GameApi::PolygonApi::cube(float start_x, float end_x,
				  float start_y, float end_y,
				  float start_z, float end_z)
{
  Point p111(start_x, start_y, start_z);
  Point p112(start_x, start_y, end_z);
  Point p121(start_x, end_y, start_z);
  Point p122(start_x, end_y, end_z);
  Point p211(end_x, start_y, start_z);
  Point p212(end_x, start_y, end_z);
  Point p221(end_x, end_y, start_z);
  Point p222(end_x, end_y, end_z);
  
  FaceCollection *coll = new CubeElem(p111,p112,p121,p122,
				      p211,p212,p221,p222);
  return add_polygon2(e, coll,1);  
}

EXPORT GameApi::P GameApi::PolygonApi::cube(PT *p)
{
  Point *p111 = find_point(e,p[0]);
  Point *p112 = find_point(e,p[1]);
  Point *p121 = find_point(e,p[2]);
  Point *p122 = find_point(e,p[3]);
  Point *p211 = find_point(e,p[4]);
  Point *p212 = find_point(e,p[5]);
  Point *p221 = find_point(e,p[6]);
  Point *p222 = find_point(e,p[7]);
  
  FaceCollection *coll = new CubeElem(*p111,*p112,*p121,*p122,
				      *p211,*p212,*p221,*p222);
  return add_polygon(e, coll,1);  
}
EXPORT GameApi::P GameApi::PolygonApi::sphere(PT center, float radius, int numfaces1, int numfaces2)
{
    Point *p = find_point(e,center);
    FaceCollection *coll = new SphereElem(*p, radius, numfaces1, numfaces2);
    return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2)
{
  Point *cent = find_point(e, center);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);
  Vector *uu_x1 = find_vector(e, uu_x);
  Vector *uu_y1 = find_vector(e, uu_y);
  FaceCollection *coll = new TorusElem(numfaces1, numfaces2, *cent, *u_x1, *u_y1, radius1, *uu_x1, *uu_y1, radius2);
  return add_polygon(e, coll, 1); 
}
EXPORT GameApi::P GameApi::PolygonApi::cone(int numfaces, PT p1, PT p2, float rad1, float rad2)
{
    Point *pp1 = find_point(e,p1);
    Point *pp2 = find_point(e,p2);
    FaceCollection *coll = new ConeElem(numfaces, *pp1, *pp2, rad1, rad2);
    return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::ring(float sx, float sy, float x, int steps)
{
  std::pair<Point, Vector> array[] = 
    { 
      std::make_pair(Point(-sx,-sy,0.0), Vector(-1.0,-1.0,0.0)),
      std::make_pair(Point(sx,-sy,0.0), Vector(1.0, -1.0,0.0)),
      std::make_pair(Point(sx,sy,0.0), Vector(1.0, 1.0,0.0)),
      std::make_pair(Point(-sx,sy,0.0), Vector(-1.0, 1.0,0.0))
    };
  VectorArray<std::pair<Point,Vector> > *pvarray = new VectorArray<std::pair<Point,Vector> >(array, array+4);
  PointVectorCollectionConvert *pointvector = new PointVectorCollectionConvert(*pvarray);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(pvarray));
  env->deletes.push_back(std::shared_ptr<void>(pointvector));
  RingElem *ring = new RingElem(*pointvector, x, steps);
  return add_polygon(e, ring,1);
}

EXPORT GameApi::P GameApi::PolygonApi::shadow(P p, PT pos, V u_x, V u_y, V light_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *light = find_vector(e, light_vec);
  return add_polygon(e, new ShadowFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *light), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::reflection(P p, PT pos, V u_x, V u_y, V ref_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *ref = find_vector(e, ref_vec);
  return add_polygon(e, new ReflectFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *ref), 1);
}


EXPORT GameApi::P GameApi::PolygonApi::or_elem(P p1, P p2)
{
  FaceCollection *pp1 = find_facecoll(e, p1);
  FaceCollection *pp2 = find_facecoll(e, p2);
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>;
  coll->push_back(pp1);
  coll->push_back(pp2);
  coll->update_faces_cache();
  return add_polygon2(e, coll,1);
}
 

EXPORT GameApi::P GameApi::PolygonApi::texture(P orig, BM bm, int choose)
{
  FaceCollection *coll = find_facecoll(e, orig);
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle, choose);
  BufferFromBitmap *req = new BufferFromBitmap(*bitmap);
  SimpleTexCoords *coords = new SimpleTexCoords(*coll, 0);
  BoxableFaceCollection *coll2 = new TextureElem2(*coll, *req, *coords);
  return add_polygon(e, coll2, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::or_array(P *p1, int size)
{
  return or_array_1(p1,size);
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




EXPORT GameApi::P GameApi::PolygonApi::color(P next, unsigned int color)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorElem(*convert, color);
  return add_polygon(e, coll,1);
}
class ColorGrayScale : public ForwardFaceCollection
{
public:
  ColorGrayScale(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int a = col&0xff000000;
    unsigned int r = col&0xff0000;
    unsigned int g = col&0xff00;
    unsigned int b = col&0xff;
    r>>=16;
    g>>=8;
    unsigned int c = (r+g+b)/3;
    unsigned int cc = (c<<16)+(c<<8)+c;
    return a+cc;
  }
};
class ColorFromNormals : public ForwardFaceCollection
{
public:
  ColorFromNormals(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual unsigned int Color(int face, int point) const
  {
    Vector v = ForwardFaceCollection::PointNormal(face,point);
    v/=v.Dist();
    int r = v.dx*127.0+127.0;
    int g = v.dy*127.0+127.0;
    int b = v.dz*127.0+127.0;
    r<<=16;
    g<<=8;
    return 0xff000000+r+g+b;
  }
  
};
class ColorRangeFaceCollection : public ForwardFaceCollection
{
public: 
  ColorRangeFaceCollection(FaceCollection *coll, unsigned int upper_range, unsigned int lower_range) : ForwardFaceCollection(*coll), coll(coll), upper_range(upper_range), lower_range(lower_range) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int col2 = Color::RangeChange(col, upper_range, lower_range);
    return col2;
  }
private:
  FaceCollection *coll;
  unsigned int upper_range, lower_range;
};
EXPORT GameApi::P GameApi::PolygonApi::color_range(P orig, unsigned int upper_range, unsigned int lower_range)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorRangeFaceCollection(c,upper_range,lower_range);
  return add_polygon2(e, c2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_grayscale(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorGrayScale(c);
  return add_polygon2(e, c2, 1);
}
class QuadsToTris2 : public FaceCollection
{
public:
  QuadsToTris2(FaceCollection *coll) : coll(coll) { Iterate(); }
  void Iterate()
  {
    int counter = 0;
    int s = coll->NumFaces();
    for(int f=0;f<s;f++)
      {
	int count = coll->NumPoints(f);
	if (count==4)
	  {
	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,1));
	    vec.push_back(coll->FacePoint(f,2));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,1));
	    norm.push_back(coll->PointNormal(f,2));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,1));
	    color.push_back(coll->Color(f,2));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,1));	    
	    texcoord.push_back(coll->TexCoord(f,2));

	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,2));
	    vec.push_back(coll->FacePoint(f,3));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,2));
	    norm.push_back(coll->PointNormal(f,3));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,2));
	    color.push_back(coll->Color(f,3));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,2));
	    texcoord.push_back(coll->TexCoord(f,3));

	  }
	else
	  {
	    counts.push_back(count);
	    counts2.push_back(counter); counter+=count;
	    for(int i=0;i<count;i++)
	      {
		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i));
		texcoord.push_back(coll->TexCoord(f,i));
	      }
	  }
      }
  }
  virtual int NumFaces() const { return counts.size(); }
  virtual int NumPoints(int face) const { return counts[face]; }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[counts2[face]+point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return norm[counts2[face]+point];
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
        return color[counts2[face]+point];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
       return texcoord[counts2[face]+point]; 
  }

private:
  FaceCollection *coll;
  std::vector<int> counts;
  std::vector<int> counts2;
  std::vector<Point> vec;
  std::vector<Vector> norm;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
};
class FlipPolygonOrder : public ForwardFaceCollection
{
public:
  FlipPolygonOrder(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }

  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(face,Order(point, NumPoints(face)));
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(face,Order(point, NumPoints(face)));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(face,Order(point, NumPoints(face)),id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(face,Order(point, NumPoints(face)),id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(face,Order(point, NumPoints(face)));
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(face,Order(point, NumPoints(face)));
  }

  int Order(int a, int count) const
  {
    return count-a-1;
  }
private:
  FaceCollection *coll;
};
EXPORT GameApi::P GameApi::PolygonApi::flip_polygon_order(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new FlipPolygonOrder(c);
  return add_polygon(e,c2,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quads_to_triangles(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new QuadsToTris2(c);
  return add_polygon(e,c2,1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_from_normals(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorFromNormals(c);
  return add_polygon2(e, c2, 1);
}
class ColorCubeElem : public ForwardFaceCollection
{
public:
  ColorCubeElem(FaceCollection *next, Point p_0, Point p_x, Point p_y, Point p_z,
		unsigned int c_0, unsigned int c_x, unsigned int c_y, unsigned int c_z) : ForwardFaceCollection(*next), p_0(p_0), p_x(p_x), p_y(p_y), p_z(p_z), c_0(c_0), c_x(c_x), c_y(c_y), c_z(c_z) { }
  virtual unsigned int Color(int face, int point) const 
  {
    Point pos = ForwardFaceCollection::FacePoint(face,point);
    Coords c;
    c.center = p_0;
    c.u_x = p_x-p_0;
    c.u_y = p_y-p_0;
    c.u_z = p_z-p_0;
    Point pos2 = c.FindInternalCoords(pos);
    pos2.x /= c.u_x.Dist();
    pos2.y /= c.u_y.Dist();
    pos2.z /= c.u_z.Dist();

    // now [0..1]
    unsigned int cc_x_a = pos2.x*((c_x-c_0)&0xff000000);
    unsigned int cc_x_r = pos2.x*((c_x-c_0)&0xff0000);
    unsigned int cc_x_g = pos2.x*((c_x-c_0)&0xff00);
    unsigned int cc_x_b = pos2.x*((c_x-c_0)&0xff);
    unsigned int cc_y_a = pos2.y*((c_y-c_0)&0xff000000);
    unsigned int cc_y_r = pos2.y*((c_y-c_0)&0xff0000);
    unsigned int cc_y_g = pos2.y*((c_y-c_0)&0xff00);
    unsigned int cc_y_b = pos2.y*((c_y-c_0)&0xff);
    unsigned int cc_z_a = pos2.z*((c_z-c_0)&0xff000000);
    unsigned int cc_z_r = pos2.z*((c_z-c_0)&0xff0000);
    unsigned int cc_z_g = pos2.z*((c_z-c_0)&0xff00);
    unsigned int cc_z_b = pos2.z*((c_z-c_0)&0xff);
    
    cc_x_a &= 0xff000000;
    cc_y_a &= 0xff000000;
    cc_z_a &= 0xff000000;

    cc_x_r &= 0xff0000;
    cc_y_r &= 0xff0000;
    cc_z_r &= 0xff0000;

    cc_x_g &= 0xff00;
    cc_y_g &= 0xff00;
    cc_z_g &= 0xff00;

    cc_x_b &= 0xff;
    cc_y_b &= 0xff;
    cc_z_b &= 0xff;

    unsigned int cc_x = cc_x_a+cc_x_r+cc_x_g+cc_x_b;
    unsigned int cc_y = cc_y_a+cc_y_r+cc_y_g+cc_y_b;
    unsigned int cc_z = cc_z_a+cc_z_r+cc_z_g+cc_z_b;

    return c_0+cc_x+cc_y+cc_z;
  }

private:
  Point p_0, p_x, p_y, p_z;
  unsigned int c_0, c_x, c_y, c_z;
};
EXPORT GameApi::P GameApi::PolygonApi::color_cube(P next,
					   PT p_0,
					   PT p_x,
					   PT p_y,
					   PT p_z,
					   unsigned int c_0,
					   unsigned int c_x,
					   unsigned int c_y,
					   unsigned int c_z)
{
  Point *pp_0 = find_point(e,p_0);
  Point *pp_x = find_point(e,p_x);
  Point *pp_y = find_point(e,p_y);
  Point *pp_z = find_point(e,p_z);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorCubeElem(convert, *pp_0, *pp_x, *pp_y, *pp_z, c_0, c_x, c_y, c_z);
  return add_polygon2(e, coll,1);

}
EXPORT GameApi::P GameApi::PolygonApi::color_faces(P next, 
					 unsigned int color_1, 
					 unsigned int color_2,
					 unsigned int color_3, 
					 unsigned int color_4)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorFaceElem(*convert, color_1,color_2,color_3,color_4);
  return add_polygon2(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::translate(P orig, float dx, float dy, float dz)
{
  return translate_1(orig, dx,dy,dz);
}
GameApi::P GameApi::PolygonApi::translate_1(P orig, float dx, float dy, float dz)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Translate(dx,dy,dz));
  return add_polygon2(e, coll,1);
}
 
EXPORT GameApi::P GameApi::PolygonApi::rotatex(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::XRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatey(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::YRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatez(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::ZRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotate(P orig, PT point, V axis, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Point *pp = find_point(e, point);
  Vector *ax = find_vector(e, axis);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::RotateAroundAxisPoint(*pp, *ax, angle));
  return add_polygon(e, coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::scale(P orig, float sx, float sy, float sz)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);  
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Scale(sx,sy,sz));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::move(P orig, PT obj_0, V obj_x, V obj_y, V obj_z,
				     PT world_0, V world_x, V world_y, V world_z)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *obj0 = find_point(e, obj_0);
  Vector *objx = find_vector(e, obj_x);
  Vector *objy = find_vector(e, obj_y);
  Vector *objz = find_vector(e, obj_z);

  Point *world0 = find_point(e, world_0);
  Vector *worldx = find_vector(e, world_x);
  Vector *worldy = find_vector(e, world_y);
  Vector *worldz = find_vector(e, world_z);
  
  Coords obj;
  obj.center = *obj0;
  obj.u_x = *objx;
  obj.u_y = *objy;
  obj.u_z = *objz;
  Coords world;
  world.center = *world0;
  world.u_x = *worldx;
  world.u_y = *worldy;
  world.u_z = *worldz;
  FaceCollection *coll2 = new CoordChangeFaceColl(coll, false, obj);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(coll2));  
  FaceCollection *coll3 = new CoordChangeFaceColl(coll2, true, world);
  return add_polygon(e, coll3, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::splitquads(P orig, int x_count, int y_count)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *next = new SplitQuads(*coll, x_count, y_count);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(next));  
  MemoizeFaces *next2 = new MemoizeFaces(*next);
  next2->Reset();
  return add_polygon(e, next2, 1);
}

class GameApiPointFunction2 : public Function<Point,Point>
{
public:
  GameApiPointFunction2(GameApi::Env &e, GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data), void *data) : e(e), fptr(fptr), data(data) { }
  Point Index(Point p) const
  {
    GameApi::EveryApi ev(e);
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = fptr(ev, pt, data);
    Point *pp = find_point(e, pt2);
    return *pp;
  }
  
private:
  GameApi::Env &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data);
  void *data;
};
class GameApiPointFunction : public Function<Point,Point>
{
public:
  GameApiPointFunction(GameApi::Env &e, GameApi::FunctionCb<GameApi::PT,GameApi::PT> *cb) : e(e), cb(cb){ }
  Point Index(Point p) const
  {
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = cb->Map(pt);
    Point *pp = find_point(e, pt2);
    if (!pp) return Point(0.0,0.0,0.0);
    return *pp;
  }
private:
  GameApi::Env &e;
  GameApi::FunctionCb<GameApi::PT, GameApi::PT> *cb;
};
#if 0
void GameApi::PolygonApi::del_cb_later(GameApi::FunctionCb<PT,PT> *cb)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(cb));
}
#endif

class ChangePoints2 : public ForwardFaceCollection
{
public:
  ChangePoints2(FaceCollection &coll, std::function<Point (Point, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual Point FacePoint(int face, int point) const 
  {
    return f(ForwardFaceCollection::FacePoint(face,point),
		face, point);
  }
private:
  std::function<Point (Point, int,int)> f;
};
struct ChangePositions_data
{
  GameApi::Env *env;
  std::function<GameApi::PT (GameApi::PT p, int face, int point)> f;
};

Point ChangePositions_Func(Point p, int face,int point, void* data)
{
  ChangePositions_data *dt = (ChangePositions_data*)data;
  GameApi::PT pt = add_point(*dt->env, p.x, p.y, p.z);
  GameApi::PT pt2 = dt->f(pt, face, point);
  Point *pp = find_point(*dt->env, pt2);
  if (!pp) return Point(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_positions(P orig, std::function<PT (PT p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
  FaceCollection *coll = find_facecoll(e, orig);
  ChangePositions_data dt;
  dt.env = &e;
  dt.f = f;
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  FaceCollection *coll2 = new ChangePoints2(*coll, std::bind(&ChangePositions_Func, _1,_2,_3,(void*)&dt));
  return add_polygon(e, coll2, 1);
#endif
}



class ChangeNormal2 : public ForwardFaceCollection
{
public:
  ChangeNormal2(FaceCollection &coll, std::function<Vector (Vector, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual Vector PointNormal(int face, int point) const 
  {
    return f(ForwardFaceCollection::PointNormal(face,point),
		face, point);
  }
private:
  std::function<Vector (Vector, int,int)> f;
};
struct ChangeNormal_data
{
  GameApi::Env *env;
  std::function<GameApi::V (GameApi::V p, int face, int point)> f;
};

Vector ChangeNormals_Func(Vector p, int face,int point,void* data)
{
  ChangeNormal_data *dt = (ChangeNormal_data*)data;
  GameApi::V pt = add_vector(*dt->env, p.dx, p.dy, p.dz);
  GameApi::V pt2 = dt->f(pt, face, point);
  Vector *pp = find_vector(*dt->env, pt2);
  if (!pp) return Vector(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_normals(P orig, std::function<V (V p, int face, int point)> f)
{
#ifndef EMSCRIPTEN

  FaceCollection *coll = find_facecoll(e, orig);
  ChangeNormal_data dt;
  dt.env = &e;
  dt.f = f;

  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;

  FaceCollection *coll2 = new ChangeNormal2(*coll, std::bind(&ChangeNormals_Func, _1, _2, _3, &dt));
  return add_polygon(e, coll2, 1);
#endif
}


class ChangeColor2 : public ForwardFaceCollection
{
public:
  ChangeColor2(FaceCollection &coll, std::function<unsigned int (unsigned int, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual unsigned int Color(int face, int point) const 
  {
    return f(ForwardFaceCollection::Color(face,point),
		face, point);
  }
private:
  //GameApi::EveryApi &ev;
  std::function<unsigned int ( unsigned int, int,int)> f;
  //void *data;
};
struct ChangeColor_data
{
  GameApi::Env *env;
  std::function<unsigned int (unsigned int p, int face, int point)> f;
  //void *data;
};

unsigned int ChangeColor_Func(unsigned int p, int face,int point,void* data)
{
  ChangeColor_data *dt = (ChangeColor_data*)data;
  return dt->f(p,face,point);
}

EXPORT GameApi::P GameApi::PolygonApi::change_colors(P orig, std::function<unsigned int (unsigned int p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeColor_data *dt = new ChangeColor_data;
  dt->env = &e;
  dt->f = f;
  //dt.data = data;

  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(dt));

  FaceCollection *coll2 = new ChangeColor2(*coll, std::bind(ChangeColor_Func, _1,_2,_3,(void*)dt));
  return add_polygon(e, coll2, 1);
#endif
}

class ChangeTexture : public ForwardFaceCollection
{
public:
  ChangeTexture(FaceCollection &coll, std::function<int (int)> f, ::Bitmap< ::Color> **array, int size)
    : ForwardFaceCollection(coll), f(f), array(array) /*, size(size)*/ { temp = new BufferFromBitmap*[size]; }
  void GenTexture(int num) 
  {
    delete temp[num];
    temp[num] = new BufferFromBitmap(*(array[num]));
    temp[num]->Gen();
  }
  virtual BufferRef TextureBuf(int num) const 
  { 
    return temp[num]->Buffer();
  }
  virtual int FaceTexture(int face) const { return f(face); }
private:
  //GameApi::EveryApi &ev;
  std::function<int (int)> f;
  //void *data;
  Bitmap< ::Color> **array;
  //int size;
  mutable BufferFromBitmap **temp;
};


EXPORT GameApi::P GameApi::PolygonApi::change_texture(P orig, std::function<int (int face)> f, BM *array, int size)
{
  FaceCollection *coll = find_facecoll(e, orig);
  std::vector<Bitmap<Color>*> *vec = new std::vector<Bitmap<Color>*>;
  for(int i=0;i<size;i++)
    {
      BitmapHandle *handle = find_bitmap(e, array[i]);
      ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
      vec->push_back(bitmap);
    }
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  ChangeTexture *tex = new ChangeTexture(*coll, f, &(*vec)[0], size);
  return add_polygon(e, tex, 1);
}




EXPORT GameApi::P GameApi::PolygonApi::recalculate_normals(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new RecalculateNormals(*coll);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::memoize(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::memoize_all(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  coll2->MemoizeAll();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::heightmap(FB bm,
					  std::function<P (float)> f, float dx, float dz)
{
  FloatBitmap *fb = find_float_bitmap(e, bm);
  int sx = fb->bitmap->SizeX();
  int sy = fb->bitmap->SizeY();
  std::vector<P> vec;
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  float val = fb->bitmap->Map(x,y);
	  P p = f(val);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::heightmap(BM bm,
					  HeightMapType t,
				       float min_x, float max_x,
				       float min_y, float max_y,
				       float min_z, float max_z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  HeightMap3DataImpl *data = new HeightMap3DataImpl(*bitmap);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(data));
  Point p(min_x, max_y, min_z);
  Vector u_x(max_x-min_x, 0.0, 0.0);
  Vector u_y(0.0, -(max_y-min_y),0.0);
  Vector u_z(0.0,0.0,max_z-min_z);

  HeightMap3 *heightmap = new HeightMap3(*data, 0.0, 1.0,
					 p, u_x, u_y, u_z);
  env->deletes.push_back(std::shared_ptr<void>(heightmap));
  MeshFaceCollection *coll = new MeshFaceCollection(*heightmap, 0);
  return add_polygon(e, coll, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::anim_array(P *array, int size)
{
  int s = size;
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<s;i++)
    {
      P *ptr = array+i;
      FaceCollection *c = find_facecoll(e, *ptr);
      //if (!c) { std::cout << "anim_array: dynamic cast failed" << std::endl; exit(0); }
      vec->push_back(c);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec->begin(), vec->end());
  handle->collarrayowned = true;
  //handle->size = size;
  return add_polygon(e, handle);
}

EXPORT void GameApi::PolygonApi::render(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "Need to call prepare() before render() call(do not put prepare() to renderloop)" << std::endl; }
  state_bm->Render(choose); 
}

StateBitmaps * PrepareFaceCollPolyHandle(FaceCollPolyHandle *h2, int bbm_choose)
{
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
    
  FaceCollection * const *array2 = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      VectorArray<FaceCollection*> *ptr = dynamic_cast<VectorArray<FaceCollection*>*>(h2->collarray);
      //std::cout << "Using collarray" << std::endl;
      array2 = ptr->get_whole_array();
      size = ptr->Size();
    }
  if (bbm_choose != -1)
    {
      array2 = &array2[bbm_choose];
      size = 1;
    }
  FaceCollection **array = const_cast<FaceCollection**>(array2);
  FaceArrayMesh *mesh = new FaceArrayMesh(array, size);
  FaceArrayMeshNormals *normal = new FaceArrayMeshNormals(array, size);
  FaceArrayMeshTexCoords *coord = new FaceArrayMeshTexCoords(array, size);
  FaceArrayMeshColors *color = new FaceArrayMeshColors(array, size);
  FaceArrayMeshTextures *textures = new FaceArrayMeshTextures(array, size, 0);

  StateBitmaps *state_bm = new StateBitmaps(mesh, normal, coord, color, textures, *textures);
  state_bm->Prepare();
  return state_bm;
}

EXPORT void GameApi::PolygonApi::prepare(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  StateBitmaps *state_bm = PrepareFaceCollPolyHandle(h2, bbm_choose);
  pp->states[p.id] = state_bm;
}

EXPORT void GameApi::PolygonApi::preparepoly(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = new ArrayRender;
  pp->rend[p.id] = r;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
  
  FaceCollection **array = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      std::cout << "Using collarray" << std::endl;
      //array = h2->collarray;
      AllocToNativeArray(*h2->collarray, &array);
    }
  if (bbm_choose != -1)
    {
      array = &array[bbm_choose];
      size = 1;
    }
  FaceArrayMesh mesh(array, size);
  FaceArrayMeshNormals normal(array, size);
  FaceArrayMeshTexCoords coord(array, size);
  FaceArrayMeshColors color(array, size);

  MeshToTriangles trimesh(mesh);
  MeshNormalsToTriangles trinormal(normal, mesh.NumPoints()==4);
  MeshTexCoordsToTriangles tricoord(coord, mesh.NumPoints()==4);
  MeshColorsToTriangles tricolor(color, mesh.NumPoints()==4);

  //std::cout << "preparepoly:" <<  h2->size << std::endl;
  //std::cout << mesh.NumFaces(0) << " " << mesh.NumPoints() << std::endl;
  r->Alloc(trimesh.NumFaces(0), trimesh.NumFaces(0)*trimesh.NumPoints(), size, size, size, size);
  r->InsertAll(trimesh, trinormal, tricoord, tricolor);
#if 0
  int texcount = array[0]->NumTextures();
  if (texcount)
    {
      r->AllocTexture(texcount);
      MeshTextures *texture = 0;
      for(int i=0;i<texcount;i++)
	{
	  array[0]->GenTexture(i);
	  BufferRef buf = array[0]->TextureBuf(i);
	  BitmapFromBuffer *bitmap = new BitmapFromBuffer(buf);
	  if (texture)
	    {
	      texture = new MeshTexturesImpl(*bitmap, *texture);
	    }
	  else
	    {
	      texture = new MeshTexturesImpl(*bitmap);
	    }
	}
      r->UpdateTexture(*texture, 0);
    }
#endif
  //for(int i=0;i<h2->size;i++)
  //  {
  //    std::pair<int,int> p = r->InsertMesh(mesh, i);
  //  }
}
void GameApi::PolygonApi::renderpoly(P p, PT pos)
{
  Point *pp = find_point(e, pos);
  renderpoly(p, 0, pp->x, pp->y, pp->z);
}
void GameApi::PolygonApi::renderpoly(P p, float x, float y, float z)
{
  renderpoly(p, 0, x,y,z);
}
void GameApi::PolygonApi::renderpoly(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = pp->rend[p.id];
  if (!r) { std::cout << "To use renderpoly() you should first call preparepoly(do not put it to frame loop)" << std::endl; return; }
  glPushMatrix();
#ifndef EMSCRIPTEN
  glTranslatef(x,y,z);
#endif
  //std::cout << "renderpoly: " << r->used_vertex_count << std::endl;

  //PolyHandle *handle = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
#if 0
  if (r->TextureCount())
    {
      r->EnableTexture(0);
    }
#endif
  r->Render(choose, choose, choose, choose, 0, r->used_vertex_count[choose]);
#if 0
  if (r->TextureCount())
    {
      r->DisableTexture();
    }
#endif
  glPopMatrix();
}

class CountsFaceCollection : public ForwardFaceCollection
{
public:
  CountsFaceCollection(int numfaces, FaceCollection *next) : ForwardFaceCollection(*next), numfaces(numfaces) { }
  int NumFaces() const { return numfaces; }
private:
  int numfaces;
};

class CountFuncFaceCollection : public ForwardFaceCollection
{
public:
  CountFuncFaceCollection(FaceCollection *next, std::function<int (int face)> f) : ForwardFaceCollection(*next),  f(f) { }
  virtual int NumPoints(int face) const
  {
    return f(face);
  }
  
private:
  //GameApi::EveryApi &ev;
  std::function<int (int face)> f; 
  //void *data;
};

class PointFaceCollection : public ForwardFaceCollection
{
public:
  PointFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Point FacePoint(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};


class NormalFaceCollection : public ForwardFaceCollection
{
public:
  NormalFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::V (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Vector PointNormal(int face, int point) const
  {
    GameApi::V pp = f(face, point);
    Vector *p = find_vector(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::V (int face, int point)> f;
  //void *data;
};


class ColorFaceCollection : public ForwardFaceCollection
{
public:
  ColorFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<unsigned int (int face, int point)> f) : ForwardFaceCollection(*next), /*e(e),*/  f(f) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int pp = f(face, point);
    return pp;
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<unsigned int (int face, int point)> f;
  void *data;
};


class TexFaceCollection : public ForwardFaceCollection
{
public:
  TexFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Point2d TexCoord(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    Point2d px = { p->x, p->y };
    return px;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};

class AttribFaceCollection : public ForwardFaceCollection
{
public:
  AttribFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<float (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/ f(f), idx(idx) { }
  virtual float Attrib(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::Attrib(face,point,id);
  }

private:
  std::function<float (int face, int point, int id)> f;
  int idx;
};

class AttribIFaceCollection : public ForwardFaceCollection
{
public:
  AttribIFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<int (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/  f(f), idx(idx) { }
  virtual int AttribI(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::AttribI(face,point,id);
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<int (int face, int point, int id)> f;
  int idx;
};



EXPORT GameApi::P GameApi::PolygonApi::counts(P p1, int numfaces)
{
  FaceCollection *poly = find_facecoll(e, p1);
  return add_polygon(e, new CountsFaceCollection(numfaces, poly),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::count_function(P p1, std::function<int (int face)> f)
{ 
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new CountFuncFaceCollection(poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::point_function(P p1, std::function<PT (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new PointFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::normal_function(P p1, std::function<V (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new NormalFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::color_function(P p1, std::function<unsigned int (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new ColorFaceCollection(e, poly,f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_function(P p1, std::function<PT (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new TexFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::attrib_function(P p1, std::function<float (int face, int point, int idx)> f, int idx)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribFaceCollection(e, poly, f, idx),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::attribi_function(P p1, std::function<int (int face, int point, int idx)> f, int idx)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribIFaceCollection(e, poly, f, idx),1);  
}


EXPORT void GameApi::ShaderApi::load(std::string filename)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = new ShaderFile(filename);
  ShaderSeq *seq = new ShaderSeq(*p->file);
  p->seq = seq;
  p->count = 0;
}
EXPORT void GameApi::ShaderApi::load_default()
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = new ShaderFile;
  ShaderSeq *seq = new ShaderSeq(*p->file);
  p->seq = seq;
  p->count = 0;
}

EXPORT void GameApi::ShaderApi::set_default_projection(SH shader, std::string name)
{
  set_default_projection_1(shader, name);
}
void GameApi::ShaderApi::set_default_projection_1(SH shader, std::string name)
{
  std::cout << "SetDefaultProjection:" << std::endl;
  Matrix m = Matrix::Perspective(80.0, (double)800/600, 10.1, 60000.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, m);  
}
EXPORT void GameApi::ShaderApi::set_y_rotation(SH shader, std::string name, float angle)
{
  Matrix m = Matrix::YRotation(angle); //*Matrix::Translate(0.0,0.0,-300.0);
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, m);  
}
EXPORT void GameApi::ShaderApi::link(GameApi::SH shader)
{
  link_1(shader);
}
void GameApi::ShaderApi::link_1(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->link(shader.id);
}
EXPORT GameApi::SH GameApi::ShaderApi::texture_shader()
{
  return get_normal_shader_1("comb", "comb", "", "texture:light:light", "texture:light:light");
}
EXPORT GameApi::SH GameApi::ShaderApi::colour_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:light:light", "colour:light:light");
}
EXPORT GameApi::SH GameApi::ShaderApi::colour_texture_shader()
{
  return get_normal_shader_1("comb", "comb", "","colour:texture:light:light", "colour:texture:light:light");
}
EXPORT GameApi::SH GameApi::ShaderApi::get_normal_shader(std::string v_format,
						  std::string f_format,
						  std::string g_format,
						  std::string v_comb,
						  std::string f_comb)
{
  return get_normal_shader_1(v_format, f_format, g_format, v_comb, f_comb);
}
GameApi::SH GameApi::ShaderApi::get_normal_shader_1(std::string v_format,
						  std::string f_format,
						  std::string g_format,
						  std::string v_comb,
						  std::string f_comb)
{
  SH sh = get_shader_1(v_format, f_format, g_format, v_comb, f_comb);
  bind_attrib_1(sh, 0, "in_Position");
  bind_attrib_1(sh, 1, "in_Normal");
  bind_attrib_1(sh, 2, "in_Color");
  bind_attrib_1(sh, 3, "in_TexCoord");
  bind_attrib_1(sh, 4, "in_Position2");
  link_1(sh);
  use_1(sh);
  set_default_projection_1(sh, "in_P");
  return sh;
}

void combparse(std::string comb, std::vector<std::string> &vec)
{
  std::string s;
  int ss = comb.size();
  for(int i=0;i<ss;i++)
    {
      char c = comb[i];
      if (c==':')
	{
	  vec.push_back(s);
	  s="";
	}
      else
	{
	  s+=c;
	}
    }
  if (s!="")
    {
      vec.push_back(s);
    }
}

EXPORT GameApi::SH GameApi::ShaderApi::get_shader(std::string v_format,
					std::string f_format,
					   std::string g_format, 
					   std::string v_comb,
					   std::string f_comb)
{
  return get_shader_1(v_format, f_format, g_format, v_comb, f_comb);
}

GameApi::SH GameApi::ShaderApi::get_shader_1(std::string v_format,
					std::string f_format,
					   std::string g_format, 
					   std::string v_comb,
					   std::string f_comb)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  if (!p->file)
    {
      std::cout << "ERROR: Call Shader::load before get_shader()" << std::endl;
      SH sh;
      sh.id = -1;
      return sh;
    }
  std::vector<std::string> v_vec;
  std::vector<std::string> f_vec;
  combparse(v_comb, v_vec);
  combparse(f_comb, f_vec);
  p->ids[p->count] = p->seq->GetShader(v_format, f_format, g_format, v_vec, f_vec);
  p->count++;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_privs[p->count-1] = p;
  GameApi::SH sh;
  sh.id = p->count-1;
  return sh;
}
EXPORT void GameApi::ShaderApi::use(GameApi::SH shader)
{
  use_1(shader);
}
void GameApi::ShaderApi::use_1(GameApi::SH shader)
{
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->use(p->ids[shader.id]);
}

EXPORT void GameApi::ShaderApi::unuse(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->unuse(p->ids[shader.id]);
}
EXPORT void GameApi::ShaderApi::bind_attrib(GameApi::SH shader, int num, std::string name)
{
  return bind_attrib_1(shader,num,name);
}
EXPORT void GameApi::ShaderApi::bind_frag(GameApi::SH shader, int attachment_num, std::string name)
{
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_frag(attachment_num, name);
}
void GameApi::ShaderApi::bind_attrib_1(GameApi::SH shader, int num, std::string name)
{
  std::cout << "SHADER: " << shader.id << std::endl;
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->bind_attrib(num, name);
}
EXPORT GameApi::M GameApi::ShaderApi::get_matrix_var(GameApi::SH shader, std::string name)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Matrix m = prog->get_matrix_var(name);
  return add_matrix(e, new SimpleMatrix(m));

}
EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float val)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float x, float y, float z)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Point px(x, y, z);
  prog->set_var(name, px);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float x, float y, float z, float k)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, x,y,z,k);
}


EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, int val)
{
  //std::cout << "Set var int" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

EXPORT void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, M matrix)
{
  Matrix mat = find_matrix(e, matrix);

  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, mat);
}

EXPORT void GameApi::ShaderApi::bindnames(GameApi::SH shader, 
				   std::string s_vertex,
				   std::string s_normal,
				   std::string s_color,
				   std::string s_texcoord)
{
  //ShaderPriv2 *p = (ShaderPriv2*)priv;
  //ShaderSeq *seq = p->seq;
  //Program *prog = seq->prog(p->ids[shader.id]);
  //prog->attr_loc(s_vertex, 10);
  //prog->attr_loc(s_normal, 11);
  //prog->attr_loc(s_color, 12);
  //prog->attr_loc(s_texcoord, 13);
}


EXPORT GameApi::FunctionApi::FunctionApi(Env &e) : e(e)
{
  priv = (void*)new FunctionPriv;
}
EXPORT GameApi::FunctionApi::~FunctionApi()
{
  delete (FunctionPriv*)priv;
}
float GameApi::FunctionApi::get_value(GameApi::F f, float val)
{
  FunctionImpl *impl = find_function(e,f);
  if (!impl) return 0.0;
  if (!impl->func) return 0.0;
  return impl->func->Index(val);
}

GameApi::F GameApi::FunctionApi::constant(float y)
{
  return add_function(e, new ConstantFloatFunction(y));
}
GameApi::F GameApi::FunctionApi::sin()
{
  return add_function(e, new NativeFunction<float,float>(&std::sin));
}
GameApi::F GameApi::FunctionApi::cos()
{
  return add_function(e, new NativeFunction<float,float>(&std::cos));
}

GameApi::PT GameApi::SpaceApi::plus(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point res = (*pp1) + Vector(*pp2);
  return add_point(e, res.x, res.y, res.z);
}

GameApi::BM GameApi::SurfaceApi::render(S surf, int sx, int sy,
				     PT ray_0, PT ray_x, PT ray_y, PT ray_z)
{
  SurfaceImpl *s = find_surface(e, surf);
  Point *r_0 = find_point(e, ray_0);
  Point *r_x = find_point(e, ray_x);
  Point *r_y = find_point(e, ray_y);
  Point *r_z = find_point(e, ray_z);
  //std::cout << "Render: " << *r_0 << " " << *r_x << " " << *r_y << " " << *r_z << std::endl;
  if (!r_0 || !r_x || !r_y || !r_z) return add_bitmap(e,0);
  //BitmapHandle *handle = find_bitmap(e, tex);
  //if (!handle) return add_bitmap(e,0);
  //Bitmap<Color> *texture = find_color_bitmap(handle);
  //if (!texture) return add_bitmap(e,0);
  RenderingEquationBitmap *bm = new RenderingEquationBitmap(*r_0, *r_x-*r_0, *r_y-*r_0, *r_z-*r_0, *s->surf, sx,sy);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(bm));
  TextureBitmap *bm2 = new TextureBitmap(*bm, *s->surf);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm2;
  return add_bitmap(e, handle);
}
GameApi::S GameApi::SurfaceApi::plane(PT pos, V u_x, V u_y)
{
  Point *p = find_point(e,pos);
  Vector *u_x1 = find_vector(e,u_x);
  Vector *u_y1 = find_vector(e,u_y);
  Plane pl(*p, *u_x1, *u_y1);
  SurfaceImpl i;
  i.surf = new PlaneSurfaceIn3d(pl);
  return add_surface(e,i);
}

GameApi::S GameApi::SurfaceApi::sphere(PT center, float radius)
{
  Point *p = find_point(e, center);
  SphereSurfaceIn3d *surf = new SphereSurfaceIn3d(*p, radius);
  SurfaceImpl i;
  i.surf = surf;
  return add_surface(e,i);
}
GameApi::S GameApi::SurfaceApi::texture(S orig, BM texture)
{
  SurfaceImpl *surf = find_surface(e, orig);
  if (!surf) return add_surface(e,SurfaceImpl());
  BitmapHandle *handle = find_bitmap(e, texture);
  if (!handle) return add_surface(e,SurfaceImpl());
  ::Bitmap<Color> *bm = find_color_bitmap(handle);
  if (!bm) return add_surface(e,SurfaceImpl());
  BitmapTextureSurfaceIn3d *surface = new BitmapTextureSurfaceIn3d(*surf->surf, *bm);
  SurfaceImpl impl;
  impl.surf = surface;
  return add_surface(e, impl);
}

class FlipBitmap : public Bitmap<Color>
{
public:
  FlipBitmap(Bitmap<Color> &bm, bool x, bool y) : bm(bm), flip_x(x), flip_y(y) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    int xx = x;
    int yy = y;
    if (flip_x) {
      xx = SizeX()-x-1;
    }
    if (flip_y) {
      yy = SizeY()-y-1;
    }
    return bm.Map(xx,yy);
  }

private:
  Bitmap<Color> &bm;
  bool flip_x, flip_y;
};
class DupXBitmap : public Bitmap<Color>
{
public:
  DupXBitmap(Bitmap<Color> &orig) : bm(orig) { }
  virtual int SizeX() const { return bm.SizeX()*2; }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    x/=2;
    return bm.Map(x,y);
  }
private:
  Bitmap<Color> &bm;
};
EXPORT GameApi::BM GameApi::BitmapApi::dup_x(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new DupXBitmap(*chandle->bm);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}

EXPORT GameApi::BM GameApi::BitmapApi::flip_x(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, true, false);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::flip_y(BM orig)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  Bitmap<Color> *rep = new FlipBitmap(*chandle->bm, false, true);

  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::repeat_bitmap(BM orig, int xcount, int ycount)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (!chandle) return add_bitmap(e,0);
  RepeatBitmap<Color> *rep = new RepeatBitmap<Color>(*chandle->bm, xcount, ycount);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}
EXPORT GameApi::BM GameApi::BitmapApi::world_from_bitmap(std::function<BM(int)> f, BM int_bm, int dx, int dy)
{
  int sx = size_x(int_bm);
  int sy = size_y(int_bm);
  BM current = newbitmap(sx*dx,sy*dy);
  for(int y=0;y<sy;y++)
    {
      for(int x=0;x<sx;x++)
	{
	  int index = intvalue(int_bm, x,y);
	  BM bm = f(index);
	  current = blitbitmap(current, bm, x*dx, y*dy);
	}
    }
  return current;
}
EXPORT GameApi::BM GameApi::FloatBitmapApi::subfloatbitmap(FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  GameApi::BB b = to_bool(fb, range_start, range_end);
  Color c(true_color);
  Color c2(false_color);
  GameApi::BM bm = ev->bool_bitmap_api.to_bitmap_1(b, c.r, c.g, c.b, c.alpha,
						 c2.r, c2.g, c2.b, c2.alpha);
  return bm;
}

struct CurrentState
{
public:
  int vertex;
  int normal;
  int texcoord;
  int color;
};

struct VBOImpl
{
  std::vector<CurrentState> objects;
};

GameApi::VBOApi::VBOApi(Env &e) : e(e) 
{ 
  priv = new VBOImpl;
}
GameApi::VBOApi::~VBOApi()
{
  delete (VBOImpl*)priv;
}

GameApi::Vb GameApi::VBOApi::alloc(int obj_count)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VBOObjects *objs = new VBOObjects;
  objs->SetObjectCount(obj_count);
  env->vbos.push_back(objs);
  Vb v;
  v.id = env->vbos.size()-1;
  return v;
#endif
  Vb v;
  v.id = 0;
  return v;
}

void GameApi::VBOApi::sprite(Vb v, int obj_num, BM bm, float x, float y)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];

  BitmapHandle *bm_handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(bm_handle);

  Point2d pos = { x,y };
  BitmapSprite s(*bitmap, pos);

  SpriteMesh spmesh(s);
  SpriteTexCoords texcoords(s);
  MeshToTriangles trimesh(spmesh);
  MeshTexCoordsToTriangles tricoords(texcoords, true);
  objs->MeshToOrig(obj_num, trimesh);
  objs->MeshTexCoordsToOrig(obj_num, trimesh, tricoords);
#endif
}
void GameApi::VBOApi::polygon(Vb v, int obj_num, P p, float x, float y, float z)
{
}
void GameApi::VBOApi::grid(Vb v, int obj_num, BM tile_bitmap, BM grid, float top_x, float top_y)
{
}

void GameApi::VBOApi::prepare(Vb v)
{
  //VBOImpl *impl = (VBOImpl*)priv;
  //CurrentState *state = &impl->objects[obj_num];
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->OrigToAdjusted(true, true, true);
  objs->AllocWholeBuffer(true, true, true);
  objs->CopyPartsToWholeBuffer(0, -1, -1, 0);
  objs->BlitWholeBufferToGPU();
#endif
}

void GameApi::VBOApi::swapframe(Vb v, int obj_num, int type, int frame)
{
#if 0
  VBOImpl *impl = (VBOImpl*)priv;
  CurrentState *state = &impl->objects[obj_num];
  if (type == 0)
    state->vertex = frame;
  if (type == 1)
    state->normal = frame;
  if (type == 2)
    state->color = frame;
  if (type == 3)
    state->texcoord = frame;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->CopyPartToGPU(obj_num, state->vertex, state->normal, state->color, state->texcoord);
#endif
}

void GameApi::VBOApi::move(Vb v, int obj_num, float x, float y, float z)
{
}

void GameApi::VBOApi::render(Vb v)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->BlitGPUToScreen();
#endif
}


EXPORT GameApi::EventApi::EventApi(Env &e) : e(e) 
{
  priv = (void*)new EventPriv;
}

GameApi::E GameApi::EventApi::root_event()
{
  EventInfo i = ::game_start();
  return add_event(e, i);
}
GameApi::E GameApi::EventApi::timer(GameApi::E activation_event, float time)
{
  EventInfo i = ::event_timer(activation_event.id, time);
  return add_event(e, i);  
}

GameApi::L GameApi::EventApi::polygon(GameApi::E start, GameApi::E end, GameApi::P poly)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = env->poly[poly.id];
  link.polygon_end = env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
#endif
  GameApi::L l;
  l.id = 0;
  return l;
}
GameApi::L GameApi::EventApi::bitmap(GameApi::E start, GameApi::E end, GameApi::BM start_bitmap, BM end_bitmap)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
#if 0
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = NULL; //env->poly[poly.id];
  link.polygon_end = NULL; //env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
#endif
  GameApi::L l;
  l.id = 0;
  return l;
}

enum Classify { EBefore, ECurrent, EFuture };
float event_time(Sequencer2 &seq, int e1)
{
  EventInfo i = seq.Event(e1);
  float time = 0.0;
  if (i.time_origin_id != -1)
    time = event_time(seq, i.time_origin_id);
  return i.time + time;
}
struct DynEventInfo
{
  bool currently_active;
};

Classify classify_event(Sequencer2 &seq, int e1, int e2, float time, int last_down_key, int last_up_key, std::vector<DynEventInfo> &vec)
{
  EventInfo i = seq.Event(e1);
  if (i.activation_event_id != -1 && i.deactivation_event_id != -1)
    {
      Classify ac_class = classify_event(seq, i.activation_event_id, i.deactivation_event_id, time, last_down_key, last_up_key, vec);
      if (ac_class == EBefore) return EBefore;
      if (ac_class == EFuture) return EFuture;
    }
  if (i.timer)
    {
      float event1_time = event_time(seq,e1);
      float event2_time = event_time(seq,e2);
      if (time < event1_time) return EBefore;
      if (time > event2_time) return EFuture;
      return ECurrent;
    }
  if (i.key_down)
    {
      if (last_down_key == i.key_value)
	{
	}
    }
  if (i.key_up)
    {
    }
  if (i.activate_mouse)
    {
    }
  if (i.deactivate_mouse)
    {
    }
  return EBefore;
}



void GameApi::EventApi::run_game(GameApi::ST st, int start_state)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StateRange r = env->state_ranges[st.id];
  
  AllStatesSequencer seq(env->event_infos, &env->states[r.start_range], r.range_size);
  GameRunner game(e,seq, start_state);
  game.run();

#if 0
  EventInfo ei = find_event_info(e, ee);
  
  LinkageInfo li = find_linkage(e, l);
  LinkInfo link = find_link_info(e, l);
  PosInfo pos = find_pos_info(e, l);

#endif
}

EXPORT GameApi::P GameApi::PolygonApi::create_static_geometry(GameApi::P *array, int size)
{
  if (size==0) { std::cout << "Empty array in remove_changing" << std::endl; }
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(find_facecoll(e,array[i]));
    }
  IsChangingFace *func = new IsChangingFace(vec);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(func));
  FilterFaces *coll2 = new FilterFaces(*(vec[0]), *func);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::create_dynamic_geometry(GameApi::P *array, int size)
{
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      FaceCollection *coll = find_facecoll(e,array[i]);
      vec.push_back(coll);
    }
  IsChangingFace *ch = new IsChangingFace(vec);
  for(int i=0;i<size;i++)
    {
      vec[i] = new FilterFaces(*(vec[i]), *ch);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec.begin(), vec.end());
  handle->collarrayowned = true;

  return add_polygon(e, handle);
}
EXPORT GameApi::P GameApi::PolygonApi::tri_vertex_array(float *v_array, int v_size,
						 float *n_array, int n_size,
						 unsigned int *c_array, int c_size,
						 float *tex_array, int tex_size,
						 float **attrib_array, int a_size1, int a_size2)
{
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = new VertexArrayFaceCollection(v_array, v_size,
					       n_array, n_size,
					       c_array, c_size,
					       tex_array, tex_size,
					       attrib_array, a_size1, a_size2);
  handle->collowned = true;
  handle->collarray = NULL;
  handle->collarrayowned = false;
  return add_polygon(e, handle);
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_frames(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_frames" << std::endl; }

  FaceCollPolyHandle *h2 = find_poly(e,p);
  if (!h2->collarray) { return 1; }
  return h2->collarray->Size();
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_rows(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_rows" << std::endl; }
  return state_bm->bitmap->SizeY();
}

EXPORT void GameApi::PolygonApi::get_tri_vertex_array(P p, int choose, int row,
					       int *v_size, float **v_array,
					       int *n_size, float **n_array,
					       int *c_size, unsigned int **c_array,
					       int *tex_size, float **tex_array,
					       int *attrib_size1, int *attrib_size2, float ***attrib_array)
{

  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array" << std::endl; }
  StateRow &r = state_bm->bitmap->DynRow(row);
  int vertex_pos = 0;
  int vertex_size = r.rend->used_vertex_count[choose];
  int v_choose = choose;
  int n_choose = choose;
  int c_choose = choose;
  int t_choose = choose;
  ArrayRender *rend = state_bm->GetRender(row);
  //Matrix m = state_bm->GetMatrix(choose);
  int v_offset3 = vertex_pos*3+v_choose*rend->vertex_array_size*3;
  int n_offset3 = vertex_pos*3+n_choose*rend->vertex_array_size*3;
  int c_offset =  vertex_pos+c_choose*rend->vertex_array_size;
  int tex_offset2 = vertex_pos*2+t_choose*rend->vertex_array_size*2;
  *v_array = rend->vertex_array+v_offset3;
  *n_array = rend->normal_array+n_offset3;
  *c_array = (unsigned int*)rend->color_array+c_offset;
  *tex_array = rend->tex_coord_array+tex_offset2;
  
  *v_size = vertex_size*3;
  *n_size = vertex_size*3;
  *c_size = vertex_size;
  *tex_size = vertex_size*2;
}

EXPORT GameApi::P GameApi::PolygonApi::polygon(PT *array, int size)
{
  PolygonElem *coll = new PolygonElem;
  int sz = size;
  for(int i=0;i<sz;i++)
    {
      PT p = array[i];
      Point *pp = find_point(e,p);
      coll->push_back(*pp);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = coll;
  handle->collowned = false;
  handle->collarray = NULL; 
  handle->collarrayowned = false;

  return add_polygon(e, handle);
  
}

EXPORT void GameApi::PolygonApi::render_dynamic(GameApi::P p, int array_elem, bool textures)
{
  FaceCollPolyHandle *handle = find_poly(e,p);
  FaceCollection *coll = handle->collarray->Index(array_elem);
  if (textures)
    {
      RenderOpenGlTextures(*coll);
    }
  else
    {
      RenderOpenGl(*coll);
    }
}

struct FontPriv
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

EXPORT GameApi::Ft GameApi::FontApi::newfont(const char *filename, int sx, int sy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
#ifndef EMSCRIPTEN
  Font fnt;
  fnt.bm = new FontGlyphBitmap((void*)&env->lib,filename, sx,sy);
  env->fonts.push_back(fnt);
#endif
  GameApi::Ft font;
  font.id = env->fonts.size()-1;
  return font;
}
EXPORT GameApi::LI GameApi::FontApi::glyph_outline(GameApi::Ft font, long idx, float sx, float sy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->fonts[font.id].bm->load_glyph_outline(idx, sx, sy);
  LineCollection *coll = env->fonts[font.id].bm;
  LineCollection *coll2 = new ForwardLineCollection(coll);
  return add_line_array(e, coll2);
}
EXPORT GameApi::PL GameApi::FontApi::glyph_plane(GameApi::Ft font, long idx, float sx, float sy, float dx, float dy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->fonts[font.id].bm->load_glyph_outline(idx,sx,sy);
  LineCollection *coll = env->fonts[font.id].bm;
  FontGlyphBitmap *bm = (FontGlyphBitmap*)coll;
  PlanePoints2d *plane = new FontLineCollectionWrapper(coll, bm->Types(), sx, sy, dx,dy);
  return add_plane(e, plane);
}
EXPORT GameApi::BM GameApi::FontApi::glyph(GameApi::Ft font, long idx)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->fonts[font.id].bm->load_glyph(idx);
  Bitmap<int> *bm = env->fonts[font.id].bm;
  Bitmap<Color> *cbm = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(0,0,0,0), *bm);
  MemoizeBitmap *mbm = new MemoizeBitmap(*cbm);
  mbm->MemoizeAll();
  env->deletes.push_back(std::shared_ptr<void>(cbm)); 
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = mbm;
 return add_bitmap(e,chandle2);
}

EXPORT GameApi::BM GameApi::FontApi::font_string(Ft font, const char *str, int x_gap)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int sz = strlen(str);
  FontCharacterString<Color> *array = new FontCharacterString<Color>(Color(0.0,0.0,0.0,0.0), x_gap);
  for(int i=0;i<sz;i++)
    {
      char ch = str[i];
      BM bm = glyph(font, ch);
      int top = env->fonts[font.id].bm->bitmap_top(ch);
      BitmapHandle *handle = find_bitmap(e,bm);
      Bitmap<Color> *col = find_color_bitmap(handle);
      array->push_back(col, top);
    }
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = array;
  return add_bitmap(e,chandle2);
}

EXPORT GameApi::PT GameApi::PointApi::origo()
{
  return add_point(e, 0.0,0.0,0.0);
}
EXPORT GameApi::PT GameApi::PointApi::point(float x, float y, float z)
{
  return add_point(e, x,y,z);
}
EXPORT GameApi::PT GameApi::PointApi::move(PT p1, float dx, float dy, float dz)
{
  Point *p = find_point(e,p1);
  return add_point(e, p->x+dx,p->y+dy,p->z+dz);
}
EXPORT GameApi::PT GameApi::PointApi::mix(PT p1, PT p2, float val)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Point res = *pp1+val*Vector((*pp2)-(*pp1));
  return add_point(e,res.x,res.y,res.z);
}
EXPORT float GameApi::PointApi::pt_x(PT p)
{
  return find_point(e,p)->x;
}
EXPORT float GameApi::PointApi::pt_y(PT p)
{
  return find_point(e,p)->y;
}
EXPORT float GameApi::PointApi::pt_z(PT p)
{
  return find_point(e,p)->z;
}
EXPORT float GameApi::PointApi::dist3d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  float d = (*pp1-*pp2).Dist();
  return d;
}
EXPORT float GameApi::PointApi::dist2d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  Point2d pa = { pp1->x,pp1->y };
  Point2d pb = { pp2->x,pp2->y };
  Vector2d v = pb-pa;
  float d = v.Dist();
  return d;
}
EXPORT GameApi::V GameApi::PointApi::minus(PT p1, PT p2)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Vector v = *pp1 - *pp2;
  return add_vector(e,v.dx,v.dy,v.dz);
}
EXPORT GameApi::ColorApi::ColorApi(Env &e) : e(e) { }
EXPORT GameApi::VectorApi::VectorApi(Env &e) : e(e) { }
EXPORT GameApi::PointApi::PointApi(Env &e) : e(e) { }

EXPORT GameApi::CO GameApi::ColorApi::u_color(unsigned int color)
{
  return add_color(e, 
		   (color&0xff0000) >> 16,
		   (color &0xff00) >> 8,
		   color &0xff,
		   (color&0xff000000)>>24);
}
EXPORT GameApi::CO GameApi::ColorApi::rgb_color(int r, int g, int b, int a)
{
  return add_color(e, r,g,b,a);
}
EXPORT GameApi::CO GameApi::ColorApi::rgbf_color(float r, float g, float b, float a)
{
  return add_color(e,int(r*255.0),int(g*255.0),int(b*255.0),int(a*255.0));
}
EXPORT GameApi::V GameApi::VectorApi::null_vector()
{
  return add_vector(e,0.0,0.0,0.0);
}
EXPORT GameApi::V GameApi::VectorApi::vector(float dx, float dy, float dz)
{
  return add_vector(e,dx,dy,dz);
}
EXPORT GameApi::V GameApi::VectorApi::sum(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector res = *vv1+*vv2;
  return add_vector(e,res.dx,res.dy,res.dz);
}
EXPORT GameApi::V GameApi::VectorApi::mul(V v1, float scalar)
{
  Vector *vv1 = find_vector(e,v1);
  Vector res = scalar * (*vv1);
  return add_vector(e,res.dx,res.dy,res.dz);
}
EXPORT float GameApi::VectorApi::dot(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  return Vector::DotProduct(*vv1,*vv2);
}
EXPORT GameApi::V GameApi::VectorApi::cross(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector v = Vector::CrossProduct(*vv1,*vv2);
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT float GameApi::VectorApi::projection_length(V v1, V u_x)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vu_x = find_vector(e,u_x);
  return Vector::FindProjectionLength(*vv1, *vu_x);
}

EXPORT GameApi::V GameApi::VectorApi::projection_1(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.first;
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT GameApi::V GameApi::VectorApi::projection_2(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.second;
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT GameApi::V GameApi::VectorApi::neg(V v) 
{
  Vector *vv1 = find_vector(e,v);
  Vector vx = -(*vv1);
  return add_vector(e,vx.dx, vx.dy, vx.dz);
}


EXPORT GameApi::PT GameApi::PointApi::from_angle(float radius, float angle)
{
  Point p = Point(0.0,0.0,0.0)+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}
EXPORT GameApi::PT GameApi::PointApi::from_angle(PT center, float radius, float angle)
{
  Point *cen = find_point(e,center);
  Point p = *cen+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}
EXPORT GameApi::O GameApi::VolumeApi::from_bool_bitmap(BB b, float dist)
{
  BoolBitmap *c = find_bool_bitmap(e,b);
  Bitmap<bool> *bm = c->bitmap;
  return add_volume(e, new BitmapVolume(bm, dist));
}
class SubsetColorVolume : public VolumeObject
{
public:
  SubsetColorVolume(VolumeObject *model, VolumeObject *subset, unsigned int col) : model(model), subset(subset), col(col) { }
  bool Inside(Point p) const { return model->Inside(p); }
  Color ColorValue(Point p) const
  {
    if (subset->Inside(p)) return Color(col);
    return model->ColorValue(p);
  }

private:
  VolumeObject *model;
  VolumeObject *subset;
  unsigned int col;
};
EXPORT GameApi::O GameApi::VolumeApi::subset_color(O model, O color_subset, unsigned int color)
{
  VolumeObject *model_1 = find_volume(e, model);
  VolumeObject *color_subset_1 = find_volume(e, color_subset);
  return add_volume(e, new SubsetColorVolume(model_1, color_subset_1, color));
}
EXPORT GameApi::O GameApi::VolumeApi::rotatex(O obj, float angle)
{
  VolumeObject *obj1 = find_volume(e,obj);
  return add_volume(e, new MatrixVolumeObject(obj1, Matrix::XRotation(-angle)));
}
EXPORT GameApi::O GameApi::VolumeApi::rotatey(O obj, float angle)
{
  VolumeObject *obj1 = find_volume(e,obj);
  return add_volume(e, new MatrixVolumeObject(obj1, Matrix::YRotation(-angle)));
}
EXPORT GameApi::O GameApi::VolumeApi::rotatez(O obj, float angle)
{
  VolumeObject *obj1 = find_volume(e,obj);
  return add_volume(e, new MatrixVolumeObject(obj1, Matrix::ZRotation(-angle)));
}
EXPORT GameApi::O GameApi::VolumeApi::move(O obj, float dx, float dy, float dz)
{
  VolumeObject *obj1 = find_volume(e,obj);
  return add_volume(e, new MatrixVolumeObject(obj1, Matrix::Translate(-dx,-dy,-dz)));  
}

EXPORT GameApi::O GameApi::BoolBitmapApi::to_volume(BB b, float dist)
{
  BoolBitmap *c = find_bool_bitmap(e,b);
  Bitmap<bool> *bm = c->bitmap;
  return add_volume(e, new BitmapVolume(bm, dist));
}
class ChooseBitmap3 : public Bitmap<Color>
{
public:
  ChooseBitmap3(Bitmap<bool> &bools, Bitmap<Color> &true_bitmap, Bitmap<Color> &false_bitmap) : bools(bools), true_bitmap(true_bitmap), false_bitmap(false_bitmap) { }
  virtual int SizeX() const { return std::min(std::min(bools.SizeX(), true_bitmap.SizeX()), false_bitmap.SizeX()); }
  virtual int SizeY() const { return std::min(std::min(bools.SizeY(), true_bitmap.SizeY()), false_bitmap.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    bool b = bools.Map(x,y);
    if (b) { return true_bitmap.Map(x,y); }
    return false_bitmap.Map(x,y);
  }
private:
  Bitmap<bool> &bools;
  Bitmap<Color> &true_bitmap;
  Bitmap<Color> &false_bitmap;
};

class ChooseBitmap4 : public Bitmap<Color>
{
public:
  ChooseBitmap4(Bitmap<float> &floats, Bitmap<Color> &bitmap_0, Bitmap<Color> &bitmap_1) : floats(floats), bitmap_0(bitmap_0), bitmap_1(bitmap_1) { }
  virtual int SizeX() const { return std::min(std::min(floats.SizeX(), bitmap_0.SizeX()), bitmap_1.SizeX()); }
  virtual int SizeY() const { return std::min(std::min(floats.SizeY(), bitmap_0.SizeY()), bitmap_1.SizeY()); }
  virtual Color Map(int x, int y) const
  {
    float b = floats.Map(x,y);
    return Color::Interpolate(bitmap_0.Map(x,y), bitmap_1.Map(x,y), b);
  }
private:
  Bitmap<float> &floats;
  Bitmap<Color> &bitmap_0;
  Bitmap<Color> &bitmap_1;
};

class PerlinNoise : public Bitmap<float>
{
public:
  PerlinNoise(Bitmap<float> &grad_1, Bitmap<float> &grad_2) : grad_1(grad_1), grad_2(grad_2) { }
  int SizeX() const { return grad_1.SizeX(); }
  int SizeY() const { return grad_1.SizeY(); }
  float Map(int x, int y) const
  {
    float xx = float(x)/SizeX();
    float yy = float(y)/SizeY();

    int x0 = (xx>0.0 ? (int)xx : (int)xx-1);
    int x1 = x0+1;
    int y0 = (yy>0.0 ? (int)yy : (int)yy -1);
    int y1 = y0+1;
    float sx = x-(double)x0;
    float sy = y-(double)y0;
    float n0,n1,ix0,ix1, value;
    n0 = dotGridGradient(x0,y0,xx,yy);
    n1 = dotGridGradient(x1,y0,xx,yy);
    ix0 = lerp(n0,n1,sx);
    n0 = dotGridGradient(x0,y1,xx,yy);
    n1 = dotGridGradient(x1,y1,xx,yy);
    ix1 = lerp(n0,n1,sx);
    value = lerp(ix0, ix1, sy);
    return value;
  }
  float dotGridGradient(int ix, int iy, float x, float y) const {
    float dx = x - (double)ix;
    float dy = y - (double)iy;
    return dx*grad_1.Map(ix,iy) + dy*grad_2.Map(ix,iy);
  }

  float lerp(float a_0, float a_1, float w) const
  {
    return (1.0-w)*a_0+w*a_1;
  }
private:
  Bitmap<float> & grad_1;
  Bitmap<float> & grad_2;
};
EXPORT GameApi::FB GameApi::FloatBitmapApi::perlin_noise(FB grad_1, FB grad_2)
{
  Bitmap<float> *g_1 = find_float_bitmap(e, grad_1)->bitmap;
  Bitmap<float> *g_2 = find_float_bitmap(e, grad_2)->bitmap;
  return add_float_bitmap(e, new PerlinNoise(*g_1, *g_2));
}
EXPORT GameApi::BM GameApi::FloatBitmapApi::choose_bitmap(FB fb, BM bitmap_0, BM bitmap_1)
{
  Bitmap<float> *bools2 = find_float_bitmap(e, fb)->bitmap;
  BitmapHandle *handle = find_bitmap(e, bitmap_0);
  Bitmap<Color> *true2 = find_color_bitmap(handle);
  BitmapHandle *handle2 = find_bitmap(e, bitmap_1);
  Bitmap<Color> *false2 = find_color_bitmap(handle2);
  Bitmap<Color> *bm = new ChooseBitmap4(*bools2, *true2, *false2);
  return add_color_bitmap2(e, bm);
}
EXPORT GameApi::BM GameApi::BoolBitmapApi::choose_bitmap(BB bools, BM true_bitmap, BM false_bitmap)
{
  Bitmap<bool> *bools2 = find_bool_bitmap(e, bools)->bitmap;
  BitmapHandle *handle = find_bitmap(e, true_bitmap);
  Bitmap<Color> *true2 = find_color_bitmap(handle);
  BitmapHandle *handle2 = find_bitmap(e, false_bitmap);
  Bitmap<Color> *false2 = find_color_bitmap(handle2);
  Bitmap<Color> *bm = new ChooseBitmap3(*bools2, *true2, *false2);
  return add_color_bitmap2(e, bm);
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::transform(BB orig, std::function<bool (int,int, bool)> f)
{
  BoolBitmap *c = find_bool_bitmap(e,orig);
  Bitmap<bool> *bm = c->bitmap;
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));

  BitmapTransformFromFunction<bool> *trans = new BitmapTransformFromFunction<bool>(*bm, f);

  return add_bool_bitmap(e, trans);
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::empty(int sx, int sy)
{
  return add_bool_bitmap(e, new ConstantBitmap<bool>(false, sx,sy));
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::empty(int sx, int sy)
{
  return add_float_bitmap(e, new ConstantBitmap<float>(0.0, sx,sy));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color(BM bm, int r, int g, int b)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  return add_bool_bitmap(e,new EquivalenceClassColorBitmap(*color_bm, Color(r,g,b)));
}

template<class T>
class EquivalenceClassFromArea : public Bitmap<bool>
{
public:
  EquivalenceClassFromArea(Bitmap<Color> &bm, T f) : bm(bm),  f(f) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    return f( c.r,c.g,c.b,c.alpha);
  }
  
private:
  Bitmap<Color> &bm;
  T f;
};

EXPORT GameApi::BB GameApi::BoolBitmapApi::from_float_bitmap(FB float_bm, float range_start, float range_end)
{
  FloatBitmap *fb = find_float_bitmap(e, float_bm);
  Bitmap<float> *bm = fb->bitmap;
  return add_bool_bitmap(e, new BoolBitmapFromFloatBitmap2(*bm, range_start, range_end));
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, std::function<bool (int, int, int,int)> f)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_bool_bitmap(e,new EquivalenceClassFromArea<std::function<bool(int,int,int,int)> >(*color_bm, f));
}

struct RangeData
{
  int r_start, r_end;
  int g_start, g_end;
  int b_start, b_end;
  int a_start, a_end;
};

bool range_select_color_area(int r, int g, int b,int a, void* dt)
{
  RangeData *data = (RangeData*)dt;
  if (r<data->r_start || r>data->r_end) { return false; }
  if (g<data->g_start || g>data->g_end) { return false; }
  if (b<data->b_start || b>data->b_end) { return false; }
  if (a<data->a_start || a>data->a_end) { return false; }
  return true;
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, int r_start, int r_end, int g_start, int g_end, int b_start, int b_end, int a_start, int a_end)
{
#ifndef EMSCRIPTEN

  RangeData data = { r_start, r_end, g_start, g_end, b_start, b_end, a_start, a_end };
  RangeData *dt2 = new RangeData;
  *dt2 = data;
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;
  return from_bitmaps_color_area(bm, std::bind(range_select_color_area, _1,_2,_3,_4,(void*)dt2));
#endif
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::circle(BB bg, float center_x, float center_y, float radius)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d center = { center_x, center_y };
  Bitmap<bool> *circle = new BitmapCircle(center, radius, bm->SizeX(), bm->SizeY());
  Bitmap<bool> *orbitmap = new OrBitmap(*bm, *circle);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(circle));
  return add_bool_bitmap(e, orbitmap);
}

struct Rectangle_data
{
  float start_x, end_x;
  float start_y, end_y;
};

bool Rectangle_func(int x, int y, void* data)
{
  Rectangle_data *dt = (Rectangle_data*)data;
  if (x<dt->start_x) return false;
  if (x>=dt->end_x) return false;
  if (y<dt->start_y) return false;
  if (y>=dt->end_y) return false;
  return true;
}

EXPORT int GameApi::BoolBitmapApi::size_x(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
EXPORT int GameApi::BoolBitmapApi::size_y(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
EXPORT bool GameApi::BoolBitmapApi::boolvalue(BB bm, int x, int y)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}   
  
EXPORT GameApi::BM GameApi::BitmapApi::conical_gradient(int sx, int sy, float x, float y, float angle1, float angle2, unsigned int color_1, unsigned int color_2)
{
  return add_color_bitmap2(e, new ConicalGradientBitmap(sx,sy,x,y, angle1,angle2,color_1,color_2));
}

EXPORT GameApi::BB GameApi::BoolBitmapApi::sections(int sx, int sy, float x, float y, std::function<bool (float angle)> f)
{
  return add_bool_bitmap(e, new SectionsBoolBitmap(sx,sy,x,y,f));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::part_circle(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad)
{
  return add_bool_bitmap(e, new PartCircleBoolBitmap(sx,sy, x,y,start_angle,end_angle, start_rad, end_rad));
}

EXPORT int GameApi::FloatBitmapApi::size_x(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
EXPORT int GameApi::FloatBitmapApi::size_y(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
EXPORT float GameApi::FloatBitmapApi::floatvalue(FB bm, int x, int y)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}


EXPORT GameApi::BB GameApi::BoolBitmapApi::rectangle(BB bg, float x, float y, float width, float height)
{
#ifndef EMSCRIPTEN

  Rectangle_data *d = new Rectangle_data;
  d->start_x = x;
  d->start_y = y;
  d->end_x = x+width;
  d->end_y = y+height;
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(d));

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  return or_bitmap(bg, function(std::bind(Rectangle_func,_1, _2,(void*)d), size_x(bg), size_y(bg)));
#endif  
}

class BoolBitmapSprite : public Bitmap<bool>
{
public:
  BoolBitmapSprite(Bitmap<bool> &bg, Bitmap<bool> &sprite, float x, float y, float mult_x, float mult_y) : bg(bg), sprite(sprite), x(x), y(y), mult_x(mult_x), mult_y(mult_y) { }
  int SizeX() const {return bg.SizeX(); }
  int SizeY() const {return bg.SizeY(); }
  bool Map(int mx, int my) const {
    float xx = mx - x;
    float yy = my - y;
    xx /= mult_x;
    yy /= mult_y; 
    if (xx >= 0.0 && xx < sprite.SizeX())
      if (yy >= 0.0 && yy < sprite.SizeY())
	{
	  bool b = sprite.Map(xx,yy);
	  if (!b) { return bg.Map(mx,my); }
	  return true;
	}
    return bg.Map(mx,my);
  }
private:
  Bitmap<bool> &bg;
  Bitmap<bool> &sprite;
  float x,y,mult_x, mult_y;
};

EXPORT GameApi::BB GameApi::BoolBitmapApi::sprite(BB bg, BB sprite, float x, float y, float mult_x, float mult_y)
{
  Bitmap<bool> *bg_1 = find_bool_bitmap(e,bg)->bitmap;
  Bitmap<bool> *sprite_1 = find_bool_bitmap(e,sprite)->bitmap;
  return add_bool_bitmap(e, new BoolBitmapSprite(*bg_1, *sprite_1, x, y, mult_x, mult_y));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::not_bitmap(BB b)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, b)->bitmap;
  return add_bool_bitmap(e, new NotBitmap(*bm));
}

GameApi::BB GameApi::BoolBitmapApi::or_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new OrBitmap(*bm1,*bm2));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::andnot_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new AndNotBitmap(*bm1,*bm2));
}
EXPORT GameApi::BM GameApi::BoolBitmapApi::to_bitmap(BB bools,
					      int true_r, int true_g, int true_b, int true_a,
					      int false_r, int false_g, int false_b, int false_a)
{
  return to_bitmap_1(bools, true_r, true_g, true_b, true_a,
		     false_r, false_g, false_b, false_a);
}
GameApi::BM GameApi::BoolBitmapApi::to_bitmap_1(BB bools,
					      int true_r, int true_g, int true_b, int true_a,
					      int false_r, int false_g, int false_b, int false_a)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, bools)->bitmap;
  return add_color_bitmap2(e, new ChooseTBitmap<Color>(*bm1, Color(false_r, false_g, false_b, false_a), Color(true_r, true_g, true_b, true_a)));
}

EXPORT GameApi::BoolBitmapApi::BoolBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::BoolBitmapApi::~BoolBitmapApi() { }

EXPORT GameApi::FloatBitmapApi::FloatBitmapApi(GameApi::Env &e) : e(e) { }
EXPORT GameApi::FloatBitmapApi::~FloatBitmapApi() { }

EXPORT GameApi::FB GameApi::FloatBitmapApi::function(std::function<float (int,int)> f, int sx, int sy)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_float_bitmap(e, new BitmapFromFunction<float>(f,sx,sy));
}

class BitmapFromRed : public Bitmap<float>
{
public:
  BitmapFromRed(Bitmap<Color> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.r;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_red(BM bm)
{
  BitmapHandle *handle = find_bitmap(e,bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromRed(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromGreen : public Bitmap<float>
{
public:
  BitmapFromGreen(Bitmap<Color> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.g;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};
class SpaceFillFloatBitmap : public Bitmap<float>
{
public:
  SpaceFillFloatBitmap(Point *pt, float *values, int size, int sx, int sy) : pt(pt), values(values), size(size), sx(sx), sy(sy) { }
  ~SpaceFillFloatBitmap() { delete [] pt; }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float dist = 50000.0*50000.0;
    int pos = 0;
    for(int i=0;i<size;i++)
      {
	float dx = x-pt[i].x;
	float dy = y-pt[i].y;
	if (dx*dx+dy*dy<dist)
	  {
	    dist = dx*dx+dy*dy;
	    pos = i;
	  }
      }
    return values[pos];
  }
private:
  Point *pt;
  float *values;
  int size;
  int sx,sy;
};
EXPORT GameApi::FB GameApi::FloatBitmapApi::space_fill(PT *array, float *array2, int size, int sx, int sy)
{
  Point *array3 = new Point[size];
  for(int i=0;i<size;i++)
    array3[i] = *find_point(e, array[i]);
  return add_float_bitmap(e, new SpaceFillFloatBitmap(array3, array2, size, sx,sy));
}
EXPORT GameApi::FB GameApi::FloatBitmapApi::from_green(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromGreen(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromBlue : public Bitmap<float>
{
public:
  BitmapFromBlue(Bitmap<Color> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.b;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_blue(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromBlue(*bmc);
  return add_float_bitmap(e, bm2);
}

class BitmapFromAlpha : public Bitmap<float>
{
public:
  BitmapFromAlpha(Bitmap<Color> &bm) : bm(bm) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    int val = c.alpha;
    return float(val)/255.0;
  }

private:
  Bitmap<Color> &bm;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_alpha(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromAlpha(*bmc);
  return add_float_bitmap(e, bm2);
}
class FromBoolBitmap : public Bitmap<float>
{
public:
  FromBoolBitmap(Bitmap<bool> &bm, float val_true, float val_false) : bm(bm), val_true(val_true), val_false(val_false) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual float Map(int x, int y) const
  {
    bool b = bm.Map(x,y);
    if (b) return val_true;
    return val_false;
  }

private:
  Bitmap<bool> &bm;
  float val_true;
  float val_false;
};

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_bool(GameApi::BB b, float val_true, float val_false)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, b)->bitmap;
  return add_float_bitmap(e, new FromBoolBitmap(*bm, val_true, val_false));
}

class BitmapFromRGBA : public Bitmap<Color>
{
public:
  BitmapFromRGBA(Bitmap<float> &r, Bitmap<float> &g, Bitmap<float> &b, Bitmap<float> &a) : r(r), g(g), b(b), a(a) { }
  virtual int SizeX() const { return r.SizeX(); }
  virtual int SizeY() const { return r.SizeY(); }
  virtual Color Map(int x, int y) const
  {
    float fr = r.Map(x,y);
    float fg = g.Map(x,y);
    float fb = b.Map(x,y);
    float fa = a.Map(x,y);
    int r = fr*255.0;
    int g = fg*255.0;
    int b = fb*255.0;
    int a = fa*255.0;
    return Color(r,g,b,a);

  }
private:
  Bitmap<float> &r;
  Bitmap<float> &g;
  Bitmap<float> &b;
  Bitmap<float> &a;
};

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_color(FB r, FB g, FB b, FB a)
{
  Bitmap<float> *rb = find_float_bitmap(e,r)->bitmap;
  Bitmap<float> *gb = find_float_bitmap(e,g)->bitmap;
  Bitmap<float> *bb = find_float_bitmap(e,b)->bitmap;
  Bitmap<float> *ab = find_float_bitmap(e,a)->bitmap;
  Bitmap<Color> *bm2 = new BitmapFromRGBA(*rb,*gb,*bb,*ab);
  return add_color_bitmap2(e, bm2);
}
GameApi::BB GameApi::FloatBitmapApi::to_bool(FB fb, float true_range_start, float true_range_end)
{
  Bitmap<float> *f = find_float_bitmap(e,fb)->bitmap;
  return add_bool_bitmap(e, new FloatRangeBitmap(*f, true_range_start, true_range_end));
}

EXPORT GameApi::FB GameApi::FloatBitmapApi::from_bool_bitmap(BB bm, int csx, int csy)
{
  Bitmap<bool> *bm2 = find_bool_bitmap(e,bm)->bitmap;
  return add_float_bitmap(e, new FloatBitmapFromBoolBitmap(*bm2, csx, csy));
}

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_grayscale(FB fb)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_color_bitmap2(e, new GrayScaleBitmapFromFloatBitmap(*bm, Color(0,0,0,0), Color(255,255,255,255)));
}

EXPORT GameApi::BM GameApi::FloatBitmapApi::to_grayscale_color(FB fb, int r, int g, int b, int a,
							int r2, int g2, int b2, int a2)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_color_bitmap2(e, new GrayScaleBitmapFromFloatBitmap(*bm, Color(r,g,b,a), Color(r2,g2,b2,a2)));
}

struct PointArray
{
  Point2d *array;
  int size;
  ~PointArray() { delete[] array; }
};

GameApi::BB GameApi::BoolBitmapApi::function(std::function<bool (int,int)> f, int sx, int sy)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  
  return add_bool_bitmap(e, new BitmapFromFunction<bool>(f, sx,sy));
}
EXPORT GameApi::BB GameApi::BoolBitmapApi::polygon(BB bg2, PT *points, int size)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  Bitmap<bool> *bg = find_bool_bitmap(e, bg2)->bitmap;
  PointArray *pointarray = new PointArray;
  pointarray->array = new Point2d[size];
  pointarray->size = size;
  for(int i=0;i<size;i++)
    {
      Point p = *find_point(e,points[i]);
      Point2d p2d = { p.x, p.y };
      pointarray->array[i] = p2d;
    }
  NativeArray<Point2d> *array = new NativeArray<Point2d>(pointarray->array, size);
  PointCollection2dConvert *conv = new PointCollection2dConvert(*array);
  ContinuousLines2d *lines = new ContinuousLines2d(*conv, true);
  ContinuousBitmap<bool> *bm = new PolygonFill(bg->SizeX(), bg->SizeY(), *lines);
  BitmapFromContinuousBitmap<bool> *sbm = new BitmapFromContinuousBitmap<bool>(*bm, bg->SizeX(), bg->SizeY());
  OrBitmap *sbm2 = new OrBitmap(*bg, *sbm);
  env->deletes.push_back(std::shared_ptr<void>(pointarray));
  env->deletes.push_back(std::shared_ptr<void>(array));
  env->deletes.push_back(std::shared_ptr<void>(conv));
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(bm));
  env->deletes.push_back(std::shared_ptr<void>(sbm));

  return add_bool_bitmap(e, sbm2);
}

EXPORT GameApi::VolumeApi::VolumeApi(Env &e) : e(e) { }
EXPORT GameApi::VolumeApi::~VolumeApi() { }

EXPORT GameApi::O GameApi::VolumeApi::sphere(PT center, float radius)
{
  Point *p = find_point(e, center);
  return add_volume(e, new SphereVolume(*p, radius));
}
EXPORT GameApi::O GameApi::VolumeApi::cone(PT p1, PT p2, float rad1, float rad2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  return add_volume(e, new ConeVolume(*pp1, *pp2-*pp1, rad1, rad2));
}
EXPORT GameApi::O GameApi::VolumeApi::cube(float start_x, float end_x,
				    float start_y, float end_y,
				    float start_z, float end_z)
{
  if (start_x>end_x) std::swap(start_x, end_x);
  if (start_y>end_y) std::swap(start_y, end_y);
  if (start_z>end_z) std::swap(start_z, end_z);
  return add_volume(e, new CubeVolume(start_x, end_x,
				      start_y, end_y,
				      start_z, end_z));
}

EXPORT GameApi::O GameApi::VolumeApi::torus(PT center, PT u_x, PT u_y, float dist1, float dist2)
{
  Point *centerp = find_point(e, center);
  Point *u_xp = find_point(e, u_x);
  Point *u_yp = find_point(e, u_y);
  return add_volume(e, new TorusVolume(*u_xp-*centerp, *u_yp-*centerp, dist1, dist2, *centerp));
}
EXPORT GameApi::O GameApi::VolumeApi::colour(GameApi::O o1, unsigned int col)
{
  VolumeObject *oo1 = find_volume(e,o1);
  return add_volume(e, new ColorSpecVolume(*oo1, Color(col)));
}
EXPORT GameApi::O GameApi::VolumeApi::not_op(GameApi::O o1)
{
  VolumeObject *oo1 = find_volume(e,o1);
  return add_volume(e, new NotVolume(*oo1));
}

EXPORT GameApi::O GameApi::VolumeApi::min_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new AndVolume(*oo1,*oo2));

}

EXPORT GameApi::O GameApi::VolumeApi::max_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new OrVolume(*oo1,*oo2));

}

EXPORT GameApi::BB GameApi::VolumeApi::plane(GameApi::O o, int sx, int sy,
				      PT u_p, V u_x, V u_y,
				      float start_x, float end_x,
				      float start_y, float end_y,
				      float start_z, float end_z)
{
  VolumeObject *volume = find_volume(e,o);
  Point *u_p_1 = find_point(e,u_p);
  Vector *u_x_1 = find_vector(e,u_x);
  Vector *u_y_1 = find_vector(e,u_y);
  Plane pl(*u_p_1, *u_x_1, *u_y_1);
  Range<float> *x = new Range<float>(start_x, end_x);
  Range<float> *y = new Range<float>(start_y, end_y);
  Range<float> *z = new Range<float>(start_z, end_z);
  VolumeVoxel *voxel = new VolumeVoxel(*volume, *x,*y,*z);
  PlaneBitmap<bool> *plane = new PlaneBitmap<bool>(*voxel, pl, float(sx), float(sy));
  BitmapFromContinuousBitmap<bool> *bm = new BitmapFromContinuousBitmap<bool>(*plane, sx,sy);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(x));
  env->deletes.push_back(std::shared_ptr<void>(y));
  env->deletes.push_back(std::shared_ptr<void>(z));
  env->deletes.push_back(std::shared_ptr<void>(voxel));
  env->deletes.push_back(std::shared_ptr<void>(plane));
  return add_bool_bitmap(e, bm);
}
EXPORT GameApi::O GameApi::VolumeApi::andnot_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new AndNotVolume(*oo1,*oo2));

}
EXPORT GameApi::O GameApi::VolumeApi::scale(GameApi::O obj, float x, float y, float z)
{
  VolumeObject *o1 = find_volume(e, obj);
  return add_volume(e, new ScaleVolume(o1,x,y,z)); 
}

EXPORT GameApi::O GameApi::VolumeApi::mandelbulb(float n, float p_x, float p_y, float p_z,
					  float c_x, float c_y, float c_z,
					  float radius,
					  int iterations)
{
  return add_volume(e, new MandelBulb(n, Point(p_x,p_y,p_z), Point(c_x,c_y,c_z), radius, iterations));
}


class AppendHandleValue : public HandleValue<std::pair<Vector, unsigned int> >
{
public:
  AppendHandleValue( GameApi::EveryApi &api, std::vector<GameApi::P> &vec, float sx, float sy, float sz,
		     std::function<GameApi::P (float, float, float, float, float, float, unsigned int)> f)
    : api(api), vec(vec), f(f), sx(sx), sy(sy), sz(sz) { }
  void Handle(std::pair<Vector, unsigned int> p)
  {
    vec.push_back(f(p.first.dx,p.first.dx+sx,
		       p.first.dy,p.first.dy+sy,
		       p.first.dz,p.first.dz+sz, p.second));
  }
  GameApi::P get_all() const { return api.polygon_api.memoize( api.polygon_api.or_array(&vec[0], vec.size()) ); }
private:
  GameApi::EveryApi &api;
  std::vector<GameApi::P> &vec;

  std::function<GameApi::P (float,float,float, float,float,float, unsigned int)> f;
  float sx,sy,sz;
};

class FunctionVolume : public VolumeObject
{
public:
  FunctionVolume(std::function<bool (float x, float y, float z)> f)
    : f(f)
  {
  }
  virtual bool Inside(Point v) const { return f(v.x,v.y,v.z); }
private:
  std::function<bool (float x, float y, float z)> f; 
};


EXPORT GameApi::O GameApi::VolumeApi::boolfunction(std::function<bool (float x, float y, float z)> f)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  VolumeObject *o = new FunctionVolume(f);
  return add_volume(e,o);
}

class FunctionFloatVolumeObject : public FloatVolumeObject {
public:
  FunctionFloatVolumeObject(std::function<float (float x, float y, float z)> f) :  f(f) { }
  virtual float FloatValue(Point p) const
  {
    return f(p.x,p.y,p.z);
  }
  
private:
  std::function<float (float x, float y, float z)> f;
};

class FunctionColorVolumeObject : public ColorVolumeObject {
public:
  FunctionColorVolumeObject(std::function<unsigned int (float x, float y, float z)> f) : f(f) { }
  virtual unsigned int ColorValue(Point p) const
  {
    return f(p.x,p.y,p.z);
  }
  
private:
  std::function<unsigned int (float x, float y, float z)> f;
};


class IntersectionPoint : public VolumeObject
{
public:
  IntersectionPoint(GameApi::EveryApi &ev, 
		    std::function<float (float x, float y, float z)> ff1, float start_range1, float end_range1,
		    std::function<float (float x, float y, float z)> ff2, float start_range2, float end_range2,
		    std::function<float (float x, float y, float z)> ff3, float start_range3, float end_range3)
    : /*ev(ev),*/ f1(ff1), f2(ff2), f3(ff3),
      oo1(f1,start_range1, end_range1),
      oo2(f2,start_range2, end_range2),
      oo3(f3, start_range3, end_range3),
      intersect1(oo1,oo2), intersect_12(intersect1, oo3)
  {
  }
  virtual bool Inside(Point v) const { return intersect_12.Inside(v); }

private:
  //GameApi::EveryApi &ev;
  FunctionFloatVolumeObject f1,f2,f3;
  SubVolume oo1,oo2,oo3;
  AndVolume intersect1, intersect_12;
};


EXPORT GameApi::O GameApi::VolumeApi::subvolume(std::function<float (float x, float y, float z)> f, float start_range, float end_range)
{
  FunctionFloatVolumeObject *ff = new FunctionFloatVolumeObject(f);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ff));
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_volume(e, new SubVolume(*ff, start_range, end_range));
}
class FromPolygonVolumeObject : public VolumeObject
{
public:
  FromPolygonVolumeObject(GameApi::EveryApi &ev, FaceCollection *coll) : ev(ev), coll(coll) { }
  bool Inside(Point p) const { return false; } // TODO
private:
  GameApi::EveryApi &ev;
  FaceCollection *coll;
};
EXPORT GameApi::O GameApi::VolumeApi::from_polygon(GameApi::P p, float x, float y, float z)
{
  FaceCollection *coll = find_facecoll(e, p);
  Point px(x,y,z);
  return add_volume(e, new FaceCollectionVolume(coll,px));
}

EXPORT void GameApi::VolumeApi::find_surface(O object, PT p1, PT p2, PT *res1, PT *res2, int level)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  VolumeObject *volume = find_volume(e,object);
  Range<Point> rr(*pp1,*pp2);

  Range<Point> r = FindSurface(rr, *volume, level);

  *res1 = add_point(e, r.start.x,r.start.y,r.start.z);
  *res2 = add_point(e, r.end.x,r.end.y,r.end.z);
}

class RenderCubes3 : public FaceCollection
{
public:
  RenderCubes3(GameApi::Env &e, GameApi::O o, 
	       int sx, int sy, int sz, 
	       float start_x, float end_x,
	       float start_y, float end_y,
	       float start_z, float end_z
	       ) : e(e), o(o), sx(sx), sy(sy), sz(sz), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z) { Iterate(); }
  void Iterate()
  {
    float world_x = end_x-start_x;
    float world_y = end_y-start_y;
    float world_z = end_z-start_z;
    float step_x = world_x/sx;
    float step_y = world_y/sy;
    float step_z = world_z/sz;
    float half_step_x = step_x/2.0;
    float half_step_y = step_y/2.0;
    float half_step_z = step_z/2.0;
    VolumeObject *volume = find_volume(e,o);
    for(float z = start_z;z<start_z+world_z;z+=step_z) {
      std::cout << z << "/" << start_z+world_z << std::endl;
      for(float y = start_y;y<start_y+world_y;y+=step_y)
	for(float x =start_x;x<start_x+world_x;x+=step_x)
	  {
	    Point p000(x, y, z);
	    Point p100(x+step_x, y, z);
	    Point p010(x, y+step_y, z);
	    Point p001(x, y, z+step_z);
	    Point pn00(x-step_x, y, z);
	    Point p0n0(x, y-step_y, z);
	    Point p00n(x, y, z-step_z);

	    bool b000 = volume->Inside(p000);
	    bool b100 = volume->Inside(p100);
	    bool b010 = volume->Inside(p010);
	    bool b001 = volume->Inside(p001);
	    bool bn00 = volume->Inside(pn00);
	    bool b0n0 = volume->Inside(p0n0);
	    bool b00n = volume->Inside(p00n);
	    
	    if (b000 != b100)
	      {
		Point p0(x+half_step_x, y-half_step_y, z-half_step_z);
		Point p1(x+half_step_x, y+half_step_y, z-half_step_z);
		Point p2(x+half_step_x, y+half_step_y, z+half_step_z);
		Point p3(x+half_step_x, y-half_step_y, z+half_step_z);
		bool flip = false;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else
		  {
		    cubes.push_back(p0);
		    cubes.push_back(p1);
		    cubes.push_back(p2);
		    cubes.push_back(p3);
		  }
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (b100) { color = volume->ColorValue(p100).Pixel(); }
		colours.push_back(color);
	      }
	    if (b000 != bn00)
	      {
		Point p0(x-half_step_x, y-half_step_y, z-half_step_z);
		Point p1(x-half_step_x, y+half_step_y, z-half_step_z);
		Point p2(x-half_step_x, y+half_step_y, z+half_step_z);
		Point p3(x-half_step_x, y-half_step_y, z+half_step_z);
		bool flip = true;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else {
		  cubes.push_back(p0);
		  cubes.push_back(p1);
		  cubes.push_back(p2);
		  cubes.push_back(p3);
		}
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (bn00) { color = volume->ColorValue(pn00).Pixel(); }
		colours.push_back(color);

	      }

	    if (b000 != b010)
	      {
		Point p0(x-half_step_x, y+half_step_y, z-half_step_z);
		Point p1(x+half_step_x, y+half_step_y, z-half_step_z);
		Point p2(x+half_step_x, y+half_step_y, z+half_step_z);
		Point p3(x-half_step_x, y+half_step_y, z+half_step_z);
		bool flip = false;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else
		  {
		    cubes.push_back(p0);
		    cubes.push_back(p1);
		    cubes.push_back(p2);
		    cubes.push_back(p3);
		  }
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (b010) { color = volume->ColorValue(p010).Pixel(); }
		colours.push_back(color);

	      }
	    if (b000 != b0n0)
	      {
		Point p0(x-half_step_x, y-half_step_y, z-half_step_z);
		Point p1(x+half_step_x, y-half_step_y, z-half_step_z);
		Point p2(x+half_step_x, y-half_step_y, z+half_step_z);
		Point p3(x-half_step_x, y-half_step_y, z+half_step_z);
		bool flip = true;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else
		  {
		    cubes.push_back(p0);
		    cubes.push_back(p1);
		    cubes.push_back(p2);
		    cubes.push_back(p3);
		  }
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (b0n0) { color = volume->ColorValue(p0n0).Pixel(); }
		colours.push_back(color);
	      }


	    if (b000 != b001)
	      {
		Point p0(x-half_step_x, y-half_step_y, z+half_step_z);
		Point p1(x+half_step_x, y-half_step_y, z+half_step_z);
		Point p2(x+half_step_x, y+half_step_y, z+half_step_z);
		Point p3(x-half_step_x, y+half_step_y, z+half_step_z);
		bool flip = false;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else
		  {
		    cubes.push_back(p0);
		    cubes.push_back(p1);
		    cubes.push_back(p2);
		    cubes.push_back(p3);
		  }
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (b001) { color = volume->ColorValue(p001).Pixel(); }
		colours.push_back(color);

	      }
	    if (b000 != b00n)
	      {
		Point p0(x-half_step_x, y-half_step_y, z-half_step_z);
		Point p1(x+half_step_x, y-half_step_y, z-half_step_z);
		Point p2(x+half_step_x, y+half_step_y, z-half_step_z);
		Point p3(x-half_step_x, y+half_step_y, z-half_step_z);
		bool flip = true;
		if (flip)
		  {
		    cubes.push_back(p3);
		    cubes.push_back(p2);
		    cubes.push_back(p1);
		    cubes.push_back(p0);
		  }
		else
		  {
		    cubes.push_back(p0);
		    cubes.push_back(p1);
		    cubes.push_back(p2);
		    cubes.push_back(p3);
		  }
		unsigned int color=0x0;
		if (b000) { color = volume->ColorValue(p000).Pixel(); }
		if (b00n) { color = volume->ColorValue(p00n).Pixel(); }
		colours.push_back(color);

	      }
	  }
    }
  }
  virtual int NumFaces() const { return cubes.size()/4; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    return cubes[face*4+point];
  }

  virtual Vector PointNormal(int face, int point) const 
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    return -Vector::CrossProduct(p2-p1,p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const  { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return colours[face];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p0 = { 0.0, 0.0 };
    Point2d p1 = { 1.0, 0.0 };
    Point2d p2 = { 1.0, 1.0 };
    Point2d p3 = { 0.0, 1.0 };
    switch(point)
      {
      case 0: return p0;
      case 1: return p1;
      case 2: return p2;
      case 3: return p3;
      }
    return p0;
  }
  GameApi::Env &e;
  GameApi::O o;
  int sx,sy,sz;
  float start_x, end_x, start_y, end_y, start_z, end_z;
  std::vector<Point> cubes;
  std::vector<unsigned int> colours;
};

EXPORT GameApi::P GameApi::VolumeApi::rendercubes3(O o, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  return add_polygon(e, new RenderCubes3(e, o, sx, sy, sz, start_x,end_x, start_y,end_y, start_z, end_z), 1);
}

EXPORT GameApi::P GameApi::VolumeApi::rendercubes2(EveryApi &ev, O o, fptrtype f, int sx, int sy, int sz, float world_x, float world_y, float world_z)
{
  float step_x = world_x/sx;
  float step_y = world_y/sy;
  float step_z = world_z/sz;
  VolumeObject *volume = find_volume(e,o);
  std::vector<P> vec3;
  for(int z=0;z<sz;z++) {
    std::vector<P> vec2;
    for(int y=0;y<sy;y++) {
      std::vector<P> vec1;
      for(int x=0;x<sx;x++)
	{
	  Point p(x*step_x, y*step_y, z*step_z);
	  bool inside = volume->Inside(p);
	  if (inside)
	    {
	      Color color = volume->ColorValue(p);
	      P pp = f(x*step_x, x*step_x+step_x,
		       y*step_y, y*step_y+step_y,
		       z*step_z, z*step_z+step_z,
		       color.Pixel());
	      vec1.push_back(pp);
	    }
	}
      vec2.push_back(ev.polygon_api.or_array(&vec1[0], vec1.size()));
    }
    vec3.push_back(ev.polygon_api.or_array(&vec2[0], vec2.size()));
  }
  return ev.polygon_api.or_array(&vec3[0], vec3.size());
}
EXPORT GameApi::P GameApi::VolumeApi::rendercubes(O o, fptrtype f, int size, float wholesize)
{
  float s = wholesize/size;
  EveryApi api(e);
  std::vector<P> vec;
  AppendHandleValue hv(api, vec, s, s, s,
		       f);

  VolumeObject *volume = find_volume(e,o);
  RenderVoxel(*volume, size, wholesize, hv);
  return hv.get_all();
}

EXPORT GameApi::ContinuousBitmapApi::ContinuousBitmapApi(Env &e) : e(e) { }

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::empty(float x, float y)
{
  return constant(0x00000000, x, y);
}
class DistanceRenderContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  DistanceRenderContinuousBitmap(DistanceRenderable *dist, ColorVolumeObject *colours, float sx, float sy) : dist(dist), colours(colours),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    int count = 0; 
    Point p(x,y,0.0);
    while(1) {
      float d = dist->distance(p);
      if (d<1.0) break;
      if (count>30) return Color(0x0000000);
      p+=Vector(0.0,0.0,d);
      count++;
    }
    unsigned int col = colours->ColorValue(p);
    return Color(col);
  }
private:
  DistanceRenderable *dist;
  ColorVolumeObject *colours;
  float sx,sy;
};
GameApi::CBM GameApi::ContinuousBitmapApi::distance_render(FD obj, COV colors, float sx, float sy)
{
  DistanceRenderable *dist = find_distance(e, obj);
  ColorVolumeObject *colorsI = find_color_volume(e, colors);
  return add_continuous_bitmap(e, new DistanceRenderContinuousBitmap(dist, colorsI, sx,sy));
}
GameApi::CBM GameApi::ContinuousBitmapApi::constant(unsigned int color, float x, float y)
{
  return add_continuous_bitmap(e, new ConstantContinuousBitmap<Color>(x,y,Color(color)));
}

class FunctionContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  FunctionContinuousBitmap( std::function<unsigned int (float, float)> f, float sx, float sy) : f(f), sx(sx), sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    return Color(f( x,y));
  }
public:
  std::function<unsigned int (float, float)> f; 
  float sx; float sy;
};
EXPORT unsigned int GameApi::ContinuousBitmapApi::get_pixel(CBM bitmap, float x, float y)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, bitmap);
  return cbm->Map(x,y).Pixel();
}

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::function(std::function<unsigned int (float,float)> f, float sx, float sy)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));

  return add_continuous_bitmap(e, new FunctionContinuousBitmap( f, sx, sy));
}
EXPORT GameApi::BM GameApi::ContinuousBitmapApi::sample(CBM c_bitmap, int sx, int sy) // SampleBitmap(CB<Color, int sx,int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_color_bitmap(e, new SampleBitmap(*cbm, sx, sy));
}
EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::rotate(CBM c_bitmap, float center_x, float center_y, float angle)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_continuous_bitmap(e, new RotateContinuousBitmap<Color>(cbm, center_x, center_y, angle));
  
}

EXPORT GameApi::BM GameApi::ContinuousBitmapApi::to_bitmap(CBM bm, int sx, int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, bm);  
  return add_color_bitmap(e, new BitmapFromContinuousBitmap<Color>(*cbm, sx,sy));
}

class ComposeSurfaceColor : public ContinuousBitmap<Color>
{
public:
  ComposeSurfaceColor(SurfaceImpl *impl, ColorVolumeObject *obj) : impl(impl), obj(obj) { }
  virtual float SizeX() const { return impl->surf->XSize(); }
  virtual float SizeY() const { return impl->surf->YSize(); }
  virtual Color Map(float x, float y) const
  {
    Point p = impl->surf->Index(x,y);
    unsigned int c = obj->ColorValue(p);
    return Color(c);
  }

private:
  SurfaceImpl *impl;
  ColorVolumeObject *obj;
};

EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::surfacecolor(S s, COV cov)
{
  SurfaceImpl *impl = find_surface(e, s);
  ColorVolumeObject *obj = find_color_volume(e, cov);
  return add_continuous_bitmap(e, new ComposeSurfaceColor(impl, obj));
}
EXPORT GameApi::CBM GameApi::ContinuousBitmapApi::from_bitmap(BM bm, float xsize, float ysize)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);

  return add_continuous_bitmap(e, new ContinuousBitmapFromBitmap<Color>(*bm2, xsize, ysize));
}



class VoxelFunction : public Voxel<unsigned int>
{
public:
  VoxelFunction(GameApi::EveryApi &ev, unsigned int (*fptr)(GameApi::EveryApi &ev,int x, int y, int z, void *data), int sx, int sy, int sz, void*data) : ev(ev), fptr(fptr), sx(sx), sy(sy), sz(sz), data(data) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual unsigned int Map(int x, int y, int z) const
  {
    return fptr(ev, x,y,z,data);
  }

private:
  GameApi::EveryApi &ev;
  unsigned int (*fptr)(GameApi::EveryApi &ev,int x, int y, int z, void *data);
  int sx; 
  int sy; 
  int sz; 
  void *data;
};

EXPORT GameApi::VoxelApi::VoxelApi(Env &e) : e(e) { }
EXPORT GameApi::VX GameApi::VoxelApi::function(unsigned int (*fptr)(EveryApi &ev, int x, int y, int z, void *data), int sx, int sy, int sz, void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));

  return add_voxel(e, new VoxelFunction(*ev, fptr, sx, sy,sz, data));
}

EXPORT unsigned int GameApi::VoxelApi::get_pixel(VX v, int x, int y, int z)
{
  Voxel<unsigned int> *c = find_voxel(e, v);
  return c->Map(x,y,z);
}

typedef Voxel<unsigned int> VoxelColor;

class VoxelBoxes : public BoxableFaceCollection
{
public:
  VoxelBoxes(VoxelColor *c, float ssx, float ssy, float ssz) : c(c),  cube(Point(0.0,0.0,0.0),
													  Point(size_x(),0.0,0.0),
													  Point(0.0,size_y(),0.0),
													  Point(size_x(),size_y(),0.0),
													  Point(0.0,0.0,size_z()),
													  Point(size_x(),0.0,size_z()),
													  Point(0.0,size_y(), size_z()),
									   Point(size_x(),size_y(),size_z())), ssx(ssx), ssy(ssy), ssz(ssz)
  {
    browse();
  }
  void browse()
  {
    int sx = c->SizeX();
    int sy = c->SizeY();
    int sz = c->SizeZ();
    for(int xx=0;xx<sx;xx++)
      for(int yy=0;yy<sy;yy++)
	for(int zz=0;zz<sz;zz++)
	  {
	    if (Enabled(xx,yy,zz))
	      {
		vec.push_back(Point(xx*ssx/sx, yy*ssy/sy, zz*ssz/sz));
	      }
	  }
  }
  virtual int NumFaces() const
  {
    return vec.size()*cube.NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    return cube.NumPoints(face%cube.NumFaces());
  }
  virtual Point FacePoint(int face, int point) const
  {
    int v = face_to_vec(face);
    Point p = cube.FacePoint(face_to_face(face), point);
    p+=vec[v];
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return cube.PointNormal(face_to_face(face), point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return cube.Attrib(face_to_face(face), point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return cube.AttribI(face_to_face(face), point, id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return cube.Color(face_to_face(face), point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return cube.TexCoord(face_to_face(face), point);
  }

  int face_to_face(int face) const
  {
    return face % cube.NumFaces();
  }
  int face_to_vec(int face) const
  {
    return face/cube.NumFaces();
  }


  float size_x() const { 
    int sx = c->SizeX();
    return ssx/sx; }
  float size_y() const { 
    int sy = c->SizeY();
    return ssy/sy; }
  float size_z() const { 
    int sz = c->SizeZ();
    return ssz/sz; }

  bool Enabled(int x, int y, int z) const {
    return c->Map(x,y,z)!=0x00000000;
  }
private:
  VoxelColor *c;
  CubeElem cube;
  std::vector<Point> vec;
  float ssx,ssy,ssz;
};

EXPORT GameApi::P GameApi::VoxelApi::render_boxes(VX v, float sx, float sy, float sz)
{
  Voxel<unsigned int> *vv = find_voxel(e, v);  
  return add_polygon2(e, new VoxelBoxes(vv, sx, sy, sz), 1);
}

#if 0
EXPORT GameApi::BM GameApi::VoxelApi::sw_rays(O volume, VX colours, int sx, int sy, float vx, float vy, float vz, float z)
{
  ContinuousVoxel<Color> *color = find_voxel(e, colours);
  Vector v(vx,vy,vz);
  Function<Point2d, Point> *ray_func = new RayTracingFunction(floatvoxel, v, z);
  Bitmap<Color> *bm = new RayTracingBitmap(ray_func, *color, sx,sy); 
  return add_color_bitmap(e, bm);
}
#endif

class ColorVoxelFaceCollection : public ForwardFaceCollection
{
public:
  ColorVoxelFaceCollection(FaceCollection &coll, Voxel<unsigned int> &c, Point p, Vector v_x, Vector v_y, Vector v_z) : ForwardFaceCollection(coll), c(c), pp(p) { cc.center = v_x; cc.u_x = v_x; cc.u_y = v_y; cc.u_z = v_z; }
  virtual unsigned int Color(int face, int point) const { 
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p2 = cc.FindInternalCoords(p);
    return c.Map(p2.x,p2.y,p2.z);
  }
private:
  Voxel<unsigned int> &c;
  mutable Coords cc;
  Point pp;
};

EXPORT GameApi::P GameApi::PolygonApi::color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z)
{
  Point *pp = find_point(e, p);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);

  FaceCollection *coll = find_facecoll(e, orig);
  Voxel<unsigned int> *v = find_voxel(e, colours);
  return add_polygon(e, new ColorVoxelFaceCollection(*coll, *v, *pp, *uu_x, *uu_y, *uu_z), 1);
}

EXPORT GameApi::VA GameApi::PolygonApi::create_vertex_array(GameApi::P p, bool keep)
{
  FaceCollection *faces = find_facecoll(e, p);
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy();  
  RenderVertexArray *arr2 = new RenderVertexArray(*s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  return add_vertex_array(e, s, arr2);
}
#if 0
EXPORT int GameApi::PolygonApi::access_point_count(VA va, bool triangle)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      return s->tri_count(0);
    }
  else
    {
      return s->quad_count(0);
    }
}
EXPORT float *GameApi::PolygonApi::access_points(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point *p = s->tri_polys(0);
      return &((float*)p)[face*3*3+point*3];
    }
  else
    {
      const Point *p = s->quad_polys(0);
      return &((float*)p)[face*4*3+point*3];
    }
  return 0;
}
EXPORT float *GameApi::PolygonApi::access_color(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle) {
    const float *ptr = s->tri_color_polys(0);
    return &((float*)ptr)[face*3*4+point*4];
  }
  else
    {
    const float *ptr = s->quad_color_polys(0);
    return &((float*)ptr)[face*4*4+point*4];      
    }
}
EXPORT float *GameApi::PolygonApi::access_normals(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Vector *ptr = s->tri_normal_polys(0);
      return &((float*)ptr)[face*3*3+point*3];
    }
  else
    {
      const Vector *ptr = s->quad_normal_polys(0);
      return &((float*)ptr)[face*4*3+point*3];
    }
}
EXPORT float *GameApi::PolygonApi::access_texcoord(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point2d *ptr = s->tri_texcoord_polys(0);
      return &((float*)ptr)[face*3*2+point*2];
    }
  else
    {
      const Point2d *ptr = s->quad_texcoord_polys(0);
      return &((float*)ptr)[face*4*2+point*2];
    }
}
EXPORT void GameApi::PolygonApi::update(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *s2 = find_vertex_array_render(e, va);
  s2->update(0);
}
#endif
EXPORT void GameApi::PolygonApi::render_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_id!=-1 && s->texture_id<6000)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-6000);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
}

class AnimFace : public ForwardFaceCollection
{
public:
  AnimFace(FaceCollection &coll, Vector v) : ForwardFaceCollection(coll), /*coll(coll),*/ v(v) { }
  Point EndFacePoint(int face, int point) const 
  { return ForwardFaceCollection::FacePoint(face, point) + v; 
  }

private:
  //FaceCollection &coll;
  Vector v;
};

EXPORT GameApi::P GameApi::PolygonApi::anim_target_vector(P p, V v)
{
  FaceCollection *i = find_facecoll(e, p);
  Vector *vv = find_vector(e,v);
  FaceCollection *coll = new AnimFace(*i, *vv);
  return add_polygon(e, coll, 1);
}
class AnimColl : public ForwardFaceCollection
{
public:
  AnimColl(FaceCollection *i1, FaceCollection *i2) : ForwardFaceCollection(*i1), i1(i1), i2(i2) { }
  virtual Point EndFacePoint(int face, int point) const { return i2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return i2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return i2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return i2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return i2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return i2->TexCoord(face,point); }
private:
  FaceCollection *i1;
  FaceCollection *i2;
};
EXPORT GameApi::P GameApi::PolygonApi::anim_endpoints(P p1, P p2)
{
  FaceCollection *i1 = find_facecoll(e,p1);
  FaceCollection *i2 = find_facecoll(e,p2);
  FaceCollection *coll = new AnimColl(i1, i2);
  return add_polygon(e, coll, 1);
}
class AnimInterpolate : public FaceCollection
{
public:
  AnimInterpolate(FaceCollection *coll, float val) : coll(coll), val(val) { }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const { return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = coll->FacePoint(face, point);
    Point p2 = coll->EndFacePoint(face, point);
    return Point::Interpolate(p1, p2, val);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v1 = coll->PointNormal(face,point);
    Vector v2 = coll->EndPointNormal(face, point);
    return Vector(Point::Interpolate(Point(v1),Point(v2), val));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    float val1 = coll->Attrib(face,point,id);
    float val2 = coll->EndAttrib(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    int val1 = coll->AttribI(face,point,id);
    int val2 = coll->EndAttribI(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int c1 = coll->Color(face,point);
    unsigned int c2 = coll->EndColor(face,point);
    return Color::CubicInterpolate(c1,c2, val);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p1 = coll->TexCoord(face,point);
    Point2d p2 = coll->EndTexCoord(face,point);
    Point2d pos = { float(p1.x*val+p2.x*(1.0-val)), float(p1.y*val+p2.y*(1.0-val)) };
    return pos;
  }
private:
  FaceCollection *coll;
  float val;
};
EXPORT GameApi::P GameApi::PolygonApi::anim_interpolate(P p, float val)
{
  FaceCollection *i = find_facecoll(e,p);
  FaceCollection *coll = new AnimInterpolate(i, val);
  return add_polygon(e, coll, 1);
}

class AnimFaceScale : public ForwardFaceCollection
{
public:
  AnimFaceScale(FaceCollection &coll, Point p, float scale_x, float scale_y, float scale_z) : ForwardFaceCollection(coll), /*coll(coll),*/ p(p), scale_x(scale_x), scale_y(scale_y), scale_z(scale_z) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * Matrix::Translate(-p.x,-p.y,-p.z)
      * Matrix::Scale(scale_x, scale_y, scale_z)
      * Matrix::Translate(p.x,p.y,p.z);
  }

private:
  //FaceCollection &coll;
  Point p;
  float scale_x, scale_y, scale_z;
};


EXPORT GameApi::P GameApi::PolygonApi::anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z)
{
  FaceCollection *i = find_facecoll(e, p);
  Point *pp = find_point(e, center);
  FaceCollection *coll = new AnimFaceScale(*i, *pp, scale_x, scale_y, scale_z);
  return add_polygon(e, coll, 1);
}
#if 0
class AnimFaceMatrix : public ForwardFaceCollection
{
public:
  AnimFaceMatrix(FaceCollection &coll, Matrix m) : ForwardFaceCollection(coll), coll(coll), m(m) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * m;
  }

private:
  FaceCollection &coll;
  Matrix m;
};

EXPORT GameApi::P GameApi::PolygonApi::anim_target_matrix(P p, M matrix)
{
  FaceCollection *i = find_facecoll(e, p);
  Matrix *mm = find_matrix(e,matrix);
  FaceCollection *coll = new AnimFaceMatrix(*i, *mm);
  
  return add_polygon(e, coll, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p1, PT p2, PT p3, PT p4)
{
}
EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p, float mul_x, float mul_y)
{
}
#endif

EXPORT GameApi::TR GameApi::StateChangeApi::init(int paths)
{
  return add_timerange(e, paths);
}
class PFloatRenderer : public Renderer<float>
{
public:
  PFloatRenderer(std::function<GameApi::P (float)> f, GameApi::Env &e) : f(f),e(e) { }
  FaceCollection *Index(float t) const
  {
    GameApi::P p = f(t);
    return find_facecoll(e, p);
  }
private:
  std::function<GameApi::P (float)> f;
  GameApi::Env &e;
};

EXPORT GameApi::VV GameApi::StateChangeApi::prepareloop(float *array, int arraysize,
						 std::function<P (float val)> f, float step_duration)
{
  if (arraysize<2) { std::cout << "Error: arraysize<2" << std::endl; GameApi::VV v; v.id = 0; return v; }
  TR t = init(1);
  for(int i=0;i<arraysize-1;i++)
    {
      float val1 = array[i];
      float val2 = array[i+1];
      t = linear_1(t, 0, f, val1, val2, step_duration);
    }
  VV v = prepare_1(t);
  return v;
}

EXPORT GameApi::TR GameApi::StateChangeApi::linear(TR tr, int path_num, std::function<P (float val)> f, float start_v, float end_v, float duration)
{
  return linear_1(tr,path_num, f, start_v, end_v, duration);
}
GameApi::TR GameApi::StateChangeApi::linear_1(TR tr, int path_num, std::function<P (float val)> f, float start_v, float end_v, float duration)

{
  TROArray *arr = find_timerange(e, tr);
  TROArray *arr2 = arr->copy();
  arr2->push_back(path_num, 
		 new DefaultTimeRange<float>(start_v, end_v, duration),
		 new PFloatRenderer(f,e));
  return add_timerange(e, arr2);
}

EXPORT GameApi::VV GameApi::StateChangeApi::prepare(TR sc)
{
  return prepare_1(sc);
}
GameApi::VV GameApi::StateChangeApi::prepare_1(TR sc)
{
  return add_timerange_vertexarray(e, sc);
}
EXPORT void GameApi::StateChangeApi::render(VV v, float time, SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog);
}
EXPORT void GameApi::StateChangeApi::render(VV v, float time, SH shader, float (*fptr)(int path, std::string name))
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog, fptr);
}

EXPORT GameApi::StateChangeApi::StateChangeApi(GameApi::Env &e, GameApi::ShaderApi &api)
: e(e),api(api) { }


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
  ns->texture_id = 6000+tx.id;
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
EXPORT GameApi::TXID GameApi::TextureApi::prepare(TX tx)
{
  TextureI *tex = find_texture(e, tx);
  TextureIBitmap bm(*tex);
  BufferFromBitmap buf(bm);
  buf.Gen();

  GLuint id;
  glGenTextures(1, &id); 
#ifndef EMSCRIPTEN
  glClientActiveTexture(GL_TEXTURE0+0);
#endif
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.SizeX(),bm.SizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.Buffer().buffer);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);      
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  GameApi::TXID id2;
  id2.id = id;
  return id2;
}

EXPORT void GameApi::TextureApi::use(TXID tx, int i)
{
  glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
  glClientActiveTexture(GL_TEXTURE0+i);
#endif
  glActiveTexture(GL_TEXTURE0+i);
  glBindTexture(GL_TEXTURE_2D, tx.id);
}
EXPORT void GameApi::TextureApi::unuse(TXID tx)
{
  glDisable(GL_TEXTURE_2D);
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

class PlanePointsFunction : public PlanePoints2d
{
public:
  PlanePointsFunction( GameApi::EveryApi &e, GameApi::PT (*fptr)(GameApi::EveryApi &e, int idx, void *data),
		       int num_points, void *data, float sx, float sy) : e(e), fptr(fptr), num_points(num_points), data(data),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual int Size() const { return num_points; }
  virtual Point2d Map(int i) const {
    GameApi::PT p = fptr(e, i, data);
    Point2d pp = { e.point_api.pt_x(p), e.point_api.pt_y(p) };
    return pp;
  }
  virtual Point2d Map2(int i) const { Point2d pp = {0.0, 0.0 }; return pp; }
 
private:
  GameApi::EveryApi &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &e, int idx, void *data);
  int num_points; 
  void *data;
  float sx,sy;
};


EXPORT GameApi::PlaneApi::PlaneApi(Env &e) : e(e) { }

EXPORT GameApi::PL GameApi::PlaneApi::function(GameApi::PT (*fptr)(EveryApi &e, int idx, void*data), int num_points, float sx, float sy, void*data)
{
  GameApi::EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_plane(e, new PlanePointsFunction( *ev, fptr, num_points, data, sx,sy ));
}
class PolygonPlane : public PlanePoints2d
{
public:
  PolygonPlane(FaceCollection *coll, Matrix m, float sx, float sy) : coll(coll), m(m),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual int Size() const
  {
    int faces = coll->NumFaces();
    int count = 0;
    for(int i=0;i<faces;i++)
      {
	count += coll->NumPoints(i);
      }
    return count;
  }
  virtual Point2d Map2(int i) const { Point2d pp = { 0.0, 0.0 }; return pp; }
  virtual Point2d Map(int i) const
  {
    int faces = coll->NumFaces();
    int count = 0;
    int ii = 0;
    for(;ii<faces;ii++)
      {
	if (i>=count && i<count+coll->NumPoints(ii))
	  {
	    break;
	  }
	count += coll->NumPoints(ii);
      }
    Point p = coll->FacePoint(ii, i-count);
    Point p2 = p * m;
    Point2d p3 = { p2.x, p2.y };
    return p3;
  }
  virtual PlanePointsType Type(int i) const 
  { 
    if (i==0) return EMove;
    int ii=0;
    int count = 0;
    int faces = coll->NumFaces();
    for(;ii<faces;ii++)
      {
	if (i>=count && i<count+coll->NumPoints(ii))
	  {
	    break;
	  }
	count += coll->NumPoints(ii);
      }
    return i==count+coll->NumPoints(ii)-1 ? ELineTo : EMove;
  }
private:
  FaceCollection *coll;
  Matrix m;
  float sx,sy;
};
class BezierCurvePoints : public PointCollection
{
public:
  BezierCurvePoints(Point p1, Point p2, Point p3, Point p4) : p1(p1), p2(p2), p3(p3),p4(p4) { }
  Point Index(int i) const
  {
    switch(i) {
    case 0: return p1;
    case 1: return p2;
    case 2: return p3;
    case 3: return p4;
    };
    return p1;
  }
  int Size() const { return 4; }
private:
  Point p1,p2,p3,p4;
};
class SplineCurve : public CurveIn2d
{
public:
  SplineCurve(Point2d p1, Point2d p2, Point2d p3) : p0(p1), p1(p2), p2(p3) { }
  Point2d Index(float pos) const 
  {
    Point2d p;
    p.x = pos+p0.x;
    p.y = doit(pos+p0.x);
    return p;
  }
  float Size() const 
  {
    return (p1.x-p0.x) + (p2.x-p1.x);
  }
  
  float doit(float t) const {
    float a11 = 2.0/(p1.x-p0.x);
    float a12 = 1.0/(p1.x-p0.x);
    float a21 = 1.0/(p1.x-p0.x);
    float a22 = 2.0*(1.0/(p1.x-p0.x)+1.0/(p2.x-p1.x));
    float a23 = 1.0/(p2.x-p1.x);
    float a32 = 1.0/(p2.x-p1.x);
    float a33 = 2.0/(p2.x-p1.x);
    std::cout << a11 << " " << a12 << " " << a21 << " " << a22 << " " << a23 << " " << a32 << " " << a33 << std::endl;
    float b_1 = 3.0*(p1.y-p0.y)/(p1.x-p0.x)/(p1.x-p0.x);
    float b_2 = 3.0*((p1.y-p0.y)/(p1.x-p0.x)/(p1.x-p0.x) + (p2.y-p1.y)/(p2.x-p1.x)/(p2.x-p1.x));
    float b_3 = 3.0*(p2.y-p1.y)/(p2.x-p1.x)/(p2.x-p1.x);
    std::cout << b_1 << ":" << b_2 << ":" << b_3 << std::endl;

    Matrix m = { { a11, a12, 0.0, 0.0,
		   a21, a22, a23, 0.0,
		   0.0, a32, a33, 0.0,
		   0.0, 0.0, 0.0, 1.0 } };
    Matrix mi = Matrix::Inverse(m);
    Vector v( b_1, b_2, b_3 );
    Vector vv = v * mi;

    std::cout << vv.dx << "?" << vv.dy << "?" << vv.dz << std::endl;
    
    float a1 = vv.dx * (p1.x-p0.x)-(p1.y-p0.y);
    float b1 = -vv.dy * (p1.x-p0.x)+(p1.y-p0.y);
    float a2 = vv.dy * (p2.x-p1.x)-(p2.y-p1.y);
    float b2 = -vv.dz * (p2.x-p1.x)+(p2.y-p1.y);

    std::cout << a1 << "I" << b1 << "I" << a2 << "I" << b2 << std::endl;

    if (t<p1.x) {
      float tt = (t-p0.x)/(p1.x-p0.x);
      float q_1 = (1.0-tt)*p0.y + tt*p1.y + tt*(1.0-tt)*(a1*(1.0-tt)+b1*tt);
      return q_1;
    } else {
      float tt = (t-p1.x)/(p2.x-p1.x);
      float q_2 = (1.0-tt)*p1.y + tt*p2.y + tt*(1.0-tt)*(a2*(1.0-tt)+b2*tt);
      return q_2;
    }

  }

private:
  Point2d p0,p1,p2;
};

class RemoveSplines : public PlanePoints2d
{
public:
  RemoveSplines(PlanePoints2d *plane, float xdelta) : plane(plane), xdelta(xdelta) 
  {
    loop();
  }
  float SizeX() const { return plane->SizeX(); }
  float SizeY() const { return plane->SizeY(); }
  int Size() const { return vec.size(); }
  Point2d Map(int i) const { return vec[i]; }
  PlanePointsType Type(int i) const { return types[i]; }
  void loop() {
    int s = plane->Size();
    for(int i=0;i<s;i++)
      {
	PlanePointsType t = plane->Type(i);
	Point2d p = plane->Map(i);
	switch(t) {
	case EMove:
	  vec.push_back(p);
	  types.push_back(t);
	  break;
	case ELineTo:
	  vec.push_back(p);
	  types.push_back(t);
	  break;
	case ECubic:
	case EConic:
	  {
	    Point2d p1 = p;
	    //Point2d p2 = plane->Map(i+1);
	    Point2d p3 = plane->Map(i+2);
	    
	    if (plane->Type(i+3)!=ECubic&&plane->Type(i+3)!=EConic) { i+=2; break; }
	    Point2d c1= plane->Map(i+3);
	    Point2d c2= plane->Map(i+5);

	    if (t==ECubic) { std::swap(p3,p1); }

	    std::cout << "Bezier:" << p1 << " " << p3 << " " << c1 << " " << c2 << std::endl;

	    Point pp1(p1.x,p1.y,0.0);
	    Point pp2(p3.x,p3.y,0.0);
	    Point pp3(c1.x,c1.y,0.0);
	    Point pp4(c2.x,c2.y,0.0);
	    BezierCurvePoints ps(pp1,pp2,pp3,pp4);
	    BezierCurve c(ps);
	    for(float x = 0.0;x<c.Size()-xdelta;x+=xdelta)
	      {
		Point p = c.Index(x);
		Point2d pp = { p.x,p.y };
		std::cout << x << " " << p << std::endl;
		vec.push_back(pp);
		types.push_back(ELineTo);
	      }
	    Point p = c.Index(c.Size());
	    Point2d pp = { p.x,p.y };
	    vec.push_back(pp);
	    types.push_back(ELineTo);
	  }
	  i+=2;
	  break;
	};
      }
  }
private:
  PlanePoints2d *plane;
  std::vector<Point2d> vec;
  std::vector<PlanePointsType> types;
  float xdelta;
};
EXPORT GameApi::PL GameApi::PlaneApi::remove_splines(GameApi::PL pl, float xdelta)
{
  PlanePoints2d *plane= find_plane(e, pl);
  return add_plane(e, new RemoveSplines(plane, xdelta));
}


int triangulate_find_pos(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj)
{
  //std::cout << "Triangulate_find_pos start: " << std::endl;
  int current_obj = 0;
  int i = 0;
  int s = orig->Size()-1;
  for(;i<s;i++)
    {
      //std::cout << "loop i" << i << std::endl;
      if (orig->Type(i+1)!= PlanePoints2d::ELineTo)
	{
	  current_obj++;
	  continue;
	}
      if (current_obj != obj) { continue; }
      

      Point2d p = orig->Map(i);
      Point2d p2;
      if (i+2>=s)
	{
	  p2 = orig->Map(i+2-s);
	}
      else
	{
	  p2 = orig->Map(i+2);
	}
      bool outside = false;
      GameApi::CBM cbm = ev.plane_api.render_continuous(pl, obj, 0x00000000, 0xffffffff);
      for(float f = 0.1;f<0.9;f+=0.1)
	{
	  Point2d middle = { float(p.x*f+p2.x*(1.0-f)), float(p.y*f+p2.y*(1.0-f)) };
	  
	  unsigned int color = ev.cont_bitmap_api.get_pixel(cbm, middle.x,middle.y);
	  if (color==0x00000000) { outside=true; }
	}
      if (outside==true) continue;
      bool reject = false;
      int ss = orig->Size();
      for(int j=0;j<ss;j++)
	{
	  //std::cout << "loop j" << j << std::endl;
	  Point2d o = orig->Map(j);
	  
	  Point2d o2;
	  if (j+1>=ss)
	    {
	      o2 = orig->Map(j+1-ss);
	    }
	  else
	    {
	      o2 = orig->Map(j+1);
	    }
	  bool parallel = LineLineIntersection_Parallel(p,p2,o,o2);
	  if (parallel) continue;
	  Point2d intersect = LineLineIntersection(p,p2,o,o2);
	  bool bound = IsWithInBoundingBox(intersect, p,p2);
	  bool bound2 = IsWithInBoundingBox(intersect, o,o2);
	  bool comb = bound && bound2;
	  reject = comb;
	  if (reject) break;
	}
      if (reject) continue;
      break;
    }
  //std::cout << "Triangulate pos=" << i+1 << std::endl;
  if (i+1>=s) return i+1-s;
  return i+1;
}


class Triangulate1 : public PlanePoints2d
{
public:
  Triangulate1(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj) : orig(orig), pl(pl), ev(ev), obj(obj) 
  {
    if (orig->Size()<=3) pos=1;
    else
      pos = triangulate_find_pos(ev, pl, orig, obj);
  }
  float SizeX() const { return orig->SizeX(); }
  float SizeY() const 
  {
    return orig->SizeY(); 
  }
  int Size() const { 
    if (orig->Size()<3) return 0;
    return 3; 
  }
  Point2d Map(int i) const
  {
    if (i==0) 
      {
	if (pos-1<0) { return orig->Map(orig->Size()+pos-1); }
	return orig->Map(pos-1); 
      }
    if (i==1) { return orig->Map(pos); }
    if (i==2) {
      if (pos+1>=orig->Size()) return orig->Map(pos+1-orig->Size());
      return orig->Map(pos+1); 
    }
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  PlanePoints2d *orig;
  int pos;
  GameApi::PL pl;
  GameApi::EveryApi &ev;
  int obj;
};
class Triangulate2 : public PlanePoints2d
{
public:
  Triangulate2(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj) : orig(orig) 
  {
    if (orig->Size()<=3) pos = 1;
    else
      pos = triangulate_find_pos(ev, pl, orig,obj);
  }
  float SizeX() const { return orig->SizeX(); }
  float SizeY() const { return orig->SizeY(); }
  int Size() const {
    int s = orig->Size();
    if (s<=3) return 0;
    return s-1; 
  }
  Point2d Map(int i) const
  {
    if (i<pos) { return orig->Map(i); }
    if (i+1>=orig->Size()) { return orig->Map(i+1-orig->Size()); }
    return orig->Map(i+1);
  }

private:
  PlanePoints2d *orig;
  int pos;
};
class PlanePolygon : public FaceCollection
{
public:
  PlanePolygon(PlanePoints2d *plane, Point pos, Vector u_x, Vector u_y) : plane(plane), pos(pos), u_x(u_x), u_y(u_y) { }
  virtual int NumFaces() const
  {
    int s = plane->Size();
    int face =0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face++;
      }
    return face;
  }
  virtual int NumPoints(int face) const
  {
    int s = plane->Size();
    int face2 =0;
    int points = 0;
    int pos = 0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face2++;
	if (face2==face+1)
	  {
	    break;
	  }
	if (t==PlanePoints2d::EMove) pos=i;
      }
    points = i - pos; 
    return points;
  }
  int PointIndex(int face, int point) const
  {
    if (face==0) { return point; }
    int s = plane->Size();
    int face2 =0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face2++;
	if (face2==face)
	  {
	    break;
	  }
      }
    return i+point;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int index = PointIndex(face,point);
    Point2d p = plane->Map(index);
    return pos + p.x*u_x + p.y*u_y;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    return -Vector::CrossProduct(p2-p1,p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const { 
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  PlanePoints2d *plane;
  Point pos;
  Vector u_x;
  Vector u_y;
};
EXPORT GameApi::P GameApi::PlaneApi::to_polygon_face(PL pl, PT pos, V u_x, V u_y)
{
  PlanePoints2d *plane = find_plane(e,pl);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  return add_polygon(e, new PlanePolygon(plane, *pos_1, *uu_x, *uu_y),1);
}
class PlanePolygonLines : public FaceCollection
{
public:
  PlanePolygonLines(PlanePoints2d *plane, Point pos, Vector u_x, Vector u_y, Vector u_z, float z_mult) : plane(plane), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), z_mult(z_mult) { }
  virtual int NumFaces() const
  {
    int s = plane->Size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::ELineTo) count++;
      }
    return count;
  }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int s = plane->Size();
    int count = 0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::ELineTo) count++;
	if (count==face) break;
      }
    Point2d p0 = plane->Map(i-1);
    Point2d p1 = plane->Map(i);
    Point pp0 = pos+p0.x*u_x+p0.y*u_y;
    Point pp1 = pos+p1.x*u_x+p1.y*u_y;
    Point pp2 = pos+p0.x*u_x+p0.y*u_y+z_mult*u_z;
    Point pp3 = pos+p1.x*u_x+p1.y*u_y+z_mult*u_z;
    switch(point) {
    case 0: return pp0;
    case 1: return pp1;
    case 2: return pp2;
    case 3: return pp3;
    }
    Point p;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    return -Vector::CrossProduct(p2-p1,p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const { 
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }

private:
  PlanePoints2d *plane;
  Point pos;
  Vector u_x, u_y, u_z;
  float z_mult;
};
EXPORT GameApi::P GameApi::PlaneApi::to_polygon(EveryApi &ev, PL pl, PT pos, V u_x, V u_y, V u_z, float z_mult)
{
  Vector *uu_z = find_vector(e, u_z);
  P face = to_polygon_face(pl, pos, u_x, u_y);
  P face2 = ev.polygon_api.translate(face, uu_z->dx*z_mult, uu_z->dy*z_mult, uu_z->dz*z_mult);
  P lines = to_polygon_lines_1(pl, pos, u_x, u_y, u_z, z_mult);
  P array[] = { face, face2, lines };
  return ev.polygon_api.or_array(&array[0], 3);
}
EXPORT GameApi::P GameApi::PlaneApi::to_polygon_lines(PL pl, PT pos, V u_x, V u_y, V u_z, float z_multiplier)
{
  return to_polygon_lines_1(pl,pos,u_x,u_y,u_z,z_multiplier);
}
 GameApi::P GameApi::PlaneApi::to_polygon_lines_1(PL pl, PT pos, V u_x, V u_y, V u_z, float z_multiplier)
{
  PlanePoints2d *plane = find_plane(e,pl);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_polygon( e, new PlanePolygonLines(plane, *pos_1, *uu_x, *uu_y, *uu_z, z_multiplier), 1);
}
EXPORT std::pair<GameApi::PL, GameApi::PL> GameApi::PlaneApi::triangulate(EveryApi &ev, PL pl, int obj)
{
  PlanePoints2d *plane = find_plane(e,pl);
  PlanePoints2d *plane2 = new Triangulate1(ev, pl, plane,obj);
  PlanePoints2d *plane3 = new Triangulate2(ev, pl, plane,obj);
  PL pl1 = add_plane(e, plane2);
  PL pl2 = add_plane(e, plane3);
  return std::make_pair(pl1,pl2);
}
class EmptyPlane : public PlanePoints2d
{
public:
  EmptyPlane(float sx, float sy) : sx(sx), sy(sy) { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  int Size() const { return 0; }
  Point2d Map(int i) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  float sx,sy;
};
EXPORT GameApi::PL GameApi::PlaneApi::empty_plane( float sx, float sy )
{
  return add_plane(e, new EmptyPlane(sx,sy));
}
EXPORT GameApi::PL GameApi::PlaneApi::triangulate_all(GameApi::EveryApi &ev, PL pl, int point_count, int max_obj)
{
  PlanePoints2d *plane = find_plane(e,pl);
  PL res = empty_plane(plane->SizeX(), plane->SizeY());
  for(int obj=0;obj<max_obj;obj++)
    {
      for(int i=0;i<point_count;i++)
	{
	  std::pair<PL,PL> p = triangulate(ev,pl, obj);
	  res = or_plane(res,p.first);
	  pl = p.second;
	}
    }
  return res;
}
EXPORT GameApi::PL GameApi::PlaneApi::render_p(GameApi::P poly, GameApi::M proj_matrix, float sx, float sy)
{
  FaceCollection *coll = find_facecoll(e, poly);
  Matrix m = find_matrix(e, proj_matrix);
  return add_plane(e, new PolygonPlane(coll, m, sx,sy));
}

class ColorContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  ColorContinuousBitmap(ContinuousBitmap<bool> &bm, unsigned int color_0, unsigned int color_1) : bm(bm), color_0(color_0), color_1(color_1) { }
  float SizeX() const { return bm.SizeX(); }
  float SizeY() const { return bm.SizeY(); }
  Color Map(float x, float y) const
  {
    bool b = bm.Map(x,y);
    if (b) return color_1;
    return color_0;
  }

private:
  ContinuousBitmap<bool> &bm;
  unsigned int color_0;
  unsigned int color_1;
};
EXPORT GameApi::BB GameApi::PlaneApi::render_bool(GameApi::PL pl, int num, int sx, int sy)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  PolygonLines *lines = new PolygonLines(*ptr, num);
  ContinuousBitmap<bool> *fill = new PolygonFill(ptr->SizeX(),ptr->SizeY(), *lines);
  Bitmap<bool> *bm = new BitmapFromContinuousBitmap<bool>(*fill, sx, sy);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(fill));
  return add_bool_bitmap(e, bm);
}
   
GameApi::CBM GameApi::PlaneApi::render_continuous(GameApi::PL pl, int num, unsigned int color_0, unsigned int color_1)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  PolygonLines *lines = new PolygonLines(*ptr, num);
  ContinuousBitmap<bool> *fill = new PolygonFill(ptr->SizeX(),ptr->SizeY(), *lines);
  ContinuousBitmap<Color> *fill2 = new ColorContinuousBitmap(*fill, color_0, color_1);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(fill));
  return add_continuous_bitmap(e, fill2);
}
class FlipYPlane : public PlanePoints2d
{
public:
  FlipYPlane(PlanePoints2d *ptr) : ptr(ptr) { }
  virtual float SizeX() const { return ptr->SizeX(); }
  virtual float SizeY() const { return ptr->SizeY(); }
  virtual int Size() const { return ptr->Size(); }
  virtual Point2d Map(int i) const
  {
    Point2d p = ptr->Map(i);
    Point2d pp = Convert(p);
    return pp;
  }
  Point2d Convert(Point2d p) const
  {
    Point2d pp = { p.x, SizeY()-p.y };
    return pp;
  }
  virtual PlanePointsType Type(int i) const { 
    return ptr->Type(i);
  }


private:
  PlanePoints2d *ptr;
};

class MovePlane : public PlanePoints2d
{
public:
  MovePlane(PlanePoints2d *ptr, float dx, float dy) : ptr(ptr), dx(dx), dy(dy) { }
  virtual float SizeX() const { return ptr->SizeX(); }
  virtual float SizeY() const { return ptr->SizeY(); }
  virtual int Size() const { return ptr->Size(); }
  virtual Point2d Map(int i) const
  {
    Point2d p = ptr->Map(i);
    Point2d pp = Convert(p);
    return pp;
  }
  Point2d Convert(Point2d p) const
  {
    Point2d pp = { p.x+dx, p.y+dy };
    return pp;
  }
  virtual PlanePointsType Type(int i) const { 
    return ptr->Type(i);
  }


private:
  PlanePoints2d *ptr;
  float dx, dy;
};

EXPORT GameApi::PL GameApi::PlaneApi::flip_y(GameApi::PL pl)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  return add_plane(e, new FlipYPlane(ptr));
}
EXPORT GameApi::PL GameApi::PlaneApi::move(GameApi::PL pl, float dx, float dy)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  return add_plane(e, new MovePlane(ptr, dx, dy));
}
class OrPlane : public PlanePoints2d
{
public:
  OrPlane(PlanePoints2d *ptr, PlanePoints2d *ptr2) : p1(ptr), p2(ptr2) { }
  virtual float SizeX() const { return std::max(p1->SizeX(), p2->SizeX()); }
  virtual float SizeY() const { return std::max(p1->SizeY(), p2->SizeY()); }
  virtual int Size() const {return p1->Size()+p2->Size(); }
  virtual Point2d Map(int i) const
  {
    if (i<p1->Size()) { return p1->Map(i); }
    return p2->Map(i-p1->Size());
  }
  virtual PlanePointsType Type(int i) const { 
    if (i<p1->Size()) { return p1->Type(i); }
    return p2->Type(i-p1->Size());
  }
private:
  PlanePoints2d *p1, *p2;
};
GameApi::PL GameApi::PlaneApi::or_plane(GameApi::PL p1, GameApi::PL p2)
{
  PlanePoints2d *ptr = find_plane(e, p1);
  PlanePoints2d *ptr2 = find_plane(e, p2);
  return add_plane(e, new OrPlane(ptr, ptr2));
}
class CirclePlane : public PlanePoints2d
{
public:
  CirclePlane(Point center, float radius, int numpoints) : center(center), radius(radius), numpoints(numpoints) { }
  virtual float SizeX() const { return center.x + radius; }
  virtual float SizeY() const { return center.y + radius; }
  virtual int Size() const {return numpoints+1; }
  virtual Point2d Map(int i) const
  {
    Point2d p = { (float)(radius*cos(i*2.0*3.1415926/numpoints)),
		  (float)(radius*sin(i*2.0*3.1415926/numpoints)) };
    p.x += center.x;
    p.y += center.y;
    return p;
  }
private:
  Point center;
  float radius;
  int numpoints;
};
EXPORT GameApi::PL GameApi::PlaneApi::circle(GameApi::PT center, float radius, int numpoints)
{
  Point *c = find_point(e, center);
  return add_plane(e, new CirclePlane(*c, radius, numpoints));
}

class StarPlane : public PlanePoints2d
{
public:
  StarPlane(Point center, float radius_1, float radius_2, int numpoints) : center(center), radius_1(radius_1), radius_2(radius_2), numpoints(numpoints*2) { }
  virtual float SizeX() const { return center.x + std::max(radius_1, radius_2); }
  virtual float SizeY() const { return center.y + std::max(radius_1, radius_2); }
  virtual int Size() const {return numpoints+1; }
  virtual Point2d Map(int i) const
  {
    float radius = i % 2 == 0 ? radius_1 : radius_2;
    Point2d p = { (float)(radius*cos(i*2.0*3.14159/numpoints)),
		  (float)(radius*sin(i*2.0*3.14159/numpoints)) };
    p.x+=center.x;
    p.y+=center.y;
    return p;
  }
private:
  Point center;
  float radius_1, radius_2;
  int numpoints;
};
EXPORT GameApi::PL GameApi::PlaneApi::star(GameApi::PT center, float radius_1, float radius_2, int numpoints)
{
  Point *c = find_point(e, center);
  return add_plane(e, new StarPlane(*c, radius_1, radius_2, numpoints));
}


EXPORT GameApi::PLA GameApi::PlaneApi::prepare(GameApi::PL pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  PlaneData data;
  PlanePoints2d *ptr = find_plane(e, pl);
  int s = ptr->Size();
  if (GLEW_NV_path_rendering) {
#ifndef EMSCRIPTEN
  //std::cout << "PlaneApi::prepare" << s << std::endl;
  for(int i=0;i<s;i++)
    {
      Point2d p = ptr->Map(i);
      // std::cout << "PlaneApi::prepare" << p.x << " " << p.y << std::endl;
      PlanePoints2d::PlanePointsType b = ptr->Type(i);
      data.array.push_back(p.x);
      data.array.push_back(p.y);
      if (b==PlanePoints2d::EMove) 
	{
	  if (i!=0)
	    data.cmd_array.push_back(GL_CLOSE_PATH_NV);
	  data.cmd_array.push_back(GL_MOVE_TO_NV);
	}
      else if (b==PlanePoints2d::ELineTo)
	{
	  data.cmd_array.push_back(GL_LINE_TO_NV);
	} else if (b==PlanePoints2d::ECubic||b==PlanePoints2d::EConic)
	{
	  data.cmd_array.push_back(GL_CUBIC_CURVE_TO_NV);
	  Point2d p = ptr->Map(i+1);
	  data.array.push_back(p.x);
	  data.array.push_back(p.y);
	  Point2d p2 = ptr->Map(i+2);
	  data.array.push_back(p2.x);
	  data.array.push_back(p2.y);
	  i+=2;
	} else { 
	data.cmd_array.push_back(GL_MOVE_TO_NV);
std::cout << "Type ERROR!" << std::endl; 
      }
    }
  data.cmd_array.push_back(GL_CLOSE_PATH_NV);
  env->plane_array.push_back(data);
#endif
  }
  PLA pla;
  pla.id = env->plane_array.size()-1;
  if (GLEW_NV_path_rendering)
    {
#ifndef EMSCRIPTEN
  glPathCommandsNV(pla.id, data.cmd_array.size(), &data.cmd_array[0], data.array.size(), GL_FLOAT, &data.array[0]);
#endif
    }
  return pla;
}
EXPORT void GameApi::PlaneApi::render(GameApi::PLA pla, float x, float y, float mult_x, float mult_y)
{
  //PlaneData *dt = find_plane_array(e,pla);
  //GLuint pathObj = glGenPathsNV(1);
  //glMatrixPushExt(GL_MODELVIEW);
  if (GLEW_NV_path_rendering)
    {
#ifndef EMSCRIPTEN
  glPushMatrix();
  glMatrixScalefEXT(GL_MODELVIEW, mult_x, mult_y, 1.0);
  glMatrixTranslatefEXT(GL_MODELVIEW, x, y, 0);

  //glStencilFillPathNV(pla.id, GL_COUNT_UP_NV, 0x1F);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 0x0, 0x1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  glColor3f(1,1,1);
  glStencilFillPathNV(pla.id, GL_COUNT_UP_NV, 0x1F);
  glColor3f(1.0,1.0,1.0);
  glCoverStrokePathNV(pla.id, GL_CONVEX_HULL_NV);
  glDisable(GL_STENCIL_TEST);
  glPopMatrix();
#endif
    }
}
#if 0
EXPORT GameApi::PL GameApi::PlaneApi::floodfill_border(GameApi::BB bitmap, int x, int y)
{
  BoolBitmap *bbm = find_bool_bitmap(e, bitmap);
  Bitmap<bool> *bbm2 = bbm->bitmap;
  PlanePoints2d *points = new FloodFillBorder(*bbm, x, y);
  points->run();
  return add_plane(e, points);
}
#endif
EXPORT GameApi::WV GameApi::WaveformApi::empty(float length)
{
  return add_waveform(e, new ZeroWaveform(length, -1.0,1.0));
}
EXPORT GameApi::WV GameApi::WaveformApi::sinwave(float length, float freq)
{
  return add_waveform(e, new SinWaveform(length, freq));
}
EXPORT GameApi::WV GameApi::WaveformApi::sample(float *array, int length, float samplelength)
{
  return add_waveform(e, new ArrayWaveform2(array, length, samplelength, -1.0, 1.0));
}
EXPORT GameApi::WV GameApi::WaveformApi::int_sample(int *array, int length, float samplelength, int min_value, int max_value)
{
  GameApi::WV w;
  w.id = 0;
  return w;
}
EXPORT GameApi::WV GameApi::WaveformApi::mix(GameApi::WV orig, float pos, GameApi::WV sample)
{
  Waveform *m_orig = find_waveform(e, orig);
  Waveform *m_sample = find_waveform(e, sample);
  return add_waveform(e, new MixWaveform(m_orig, pos, m_sample));
}
EXPORT GameApi::WV GameApi::WaveformApi::volume_ramp(GameApi::WV orig, float old_y_value, float x_pos1, float x_pos2, float y_pos1, float y_pos2)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new VolumeRampWaveform(m_orig, old_y_value, x_pos1, x_pos2, y_pos1, y_pos2));
}
EXPORT GameApi::WV GameApi::WaveformApi::freq_change(GameApi::WV orig, float old_freq, float new_freq)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new FreqChangeWaveform(m_orig, old_freq, new_freq));
}

EXPORT float GameApi::WaveformApi::get_value(WV orig, float val)
{
  Waveform *m_orig = find_waveform(e, orig);
  return m_orig->Index(val);
}
EXPORT float GameApi::WaveformApi::length(GameApi::WV orig)
{
  Waveform *m_orig = find_waveform(e, orig);
  return m_orig->Length();
}

class LengthChangeWaveform : public Waveform
{
public:
  LengthChangeWaveform(Waveform *wv, float new_length) : wv(wv), new_length(new_length) { }
  virtual float Length() const { 
    return new_length; 
  }
  virtual float Min() const { return wv->Min(); }
  virtual float Max() const { return wv->Max(); }
  virtual float Index(float val) const 
  {
    if (val>=0.0 && val<new_length && val<wv->Length()) { return wv->Index(val); }
    return 0.0;
  }
private:  
  Waveform *wv;
  float new_length;
};

EXPORT GameApi::WV GameApi::WaveformApi::length_change(WV orig, float new_length)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new LengthChangeWaveform(m_orig, new_length));
}


class FunctionWaveform : public Waveform
{
public:
  FunctionWaveform(std::function<float (float)> f, float length, float min_value, float max_value) :  f(f), length(length), min_value(min_value), max_value(max_value) { }
  virtual float Length() const { return length; }
  virtual float Min() const { return min_value; }
  virtual float Max() const { return max_value; }
  virtual float Index(float val) const 
  {
    return f(val);
  }

private:
  std::function<float (float)> f; 
  float length;
  float min_value; 
  float max_value; 
}; 

EXPORT GameApi::WV GameApi::WaveformApi::function(std::function<float (float)> f, float length, float min_value, float max_value)
{
  //GameApi::EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_waveform(e, new FunctionWaveform(f, length, min_value, max_value));
}

class HeightMapPoints : public PointsApiPoints
{
public:
  HeightMapPoints(Bitmap<::Color> &color, Bitmap<float> &height, Point pos, Vector u_x, Vector u_y, Vector u_z, int sx, int sy) : color(color), height(height), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx), sy(sy) { }
  virtual int NumPoints() const { return sx*sy; }
  virtual Point Pos(int i) const
  {
    int yy = i / sx;
    int xx = i - (yy*sx);
    float h = height.Map(xx*height.SizeX()/sx,yy*height.SizeY()/sy);
    Point p = pos + u_x*xx/sx + u_y*yy/sy + h*u_z;
    return p;
  }
  virtual unsigned int Color(int i) const
  {
    int yy = i / sx;
    int xx = i - (yy*sx);
    ::Color c = color.Map(xx*color.SizeX()/sx,yy*color.SizeY()/sy);
    return c.Pixel();
  }
private:
  Bitmap<::Color> &color;
  Bitmap<float> &height;
  Point pos;
  Vector u_x;
  Vector u_y;
  Vector u_z;
  int sx,sy;
};
class OrPoints : public PointsApiPoints
{
public:
  OrPoints(PointsApiPoints *pts1, PointsApiPoints *pts2): pts1(pts1), pts2(pts2) { }
  virtual int NumPoints() const { return pts1->NumPoints()+pts2->NumPoints(); }
  virtual Point Pos(int i) const
  {
    if (i<pts1->NumPoints())
      {
	return pts1->Pos(i);
      }
    return pts2->Pos(i-pts1->NumPoints());
  }
  virtual unsigned int Color(int i) const
  {
    if (i<pts1->NumPoints())
      {
	return pts1->Color(i);
      }
    return pts2->Color(i-pts1->NumPoints());
  }

private:
  PointsApiPoints *pts1;
  PointsApiPoints *pts2;
};
EXPORT GameApi::PTS GameApi::PointsApi::heightmap(BM colour, FB height, PT pos, V u_x, V u_y, V u_z, int sx, int sy)
{
  BitmapHandle *h = find_bitmap(e, colour);
  Bitmap<Color> *colour_bm = find_color_bitmap(h);
  FloatBitmap *fbm = find_float_bitmap(e, height);
  Bitmap<float> *height_bm = fbm->bitmap;
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_points_api_points(e, new HeightMapPoints(*colour_bm, *height_bm, *pt, *uu_x, *uu_y, *uu_z, sx, sy));
}
EXPORT GameApi::PTS GameApi::PointsApi::from_volume(GameApi::O o, GameApi::PT pos,
					     GameApi::V u_x, GameApi::V u_y, GameApi::V u_z, int sx, int sy, int sz)
{
  VolumeObject *obj = find_volume(e, o);
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_points_api_points(e, new SpacePoints(*obj, *pt, *uu_x, *uu_y, *uu_z, sx, sy, sz));
}
EXPORT GameApi::PTS GameApi::PointsApi::or_points(GameApi::PTS p1, GameApi::PTS p2)
{
  PointsApiPoints *pts1 = find_pointsapi_points(e, p1);
  PointsApiPoints *pts2 = find_pointsapi_points(e, p2);
  return add_points_api_points(e,new OrPoints(pts1, pts2));
  
}
class ShadowPoints : public PointsApiPoints
{
public:
  ShadowPoints(PointsApiPoints *pts, Point pos, Vector u_x, Vector u_y, Vector light_vec) : pts(pts), pos(pos), u_x(u_x), u_y(u_y), light_vec(light_vec) { }
  virtual int NumPoints() const
  {
    return pts->NumPoints();
  }
  virtual Point Pos(int i) const
  {
    Point p = pts->Pos(i);
    Vector v = light_vec;
    LinePlaneIntersection sect = LinePlaneIntersectionFunc(p, p+v,
							   pos, pos+u_x, pos+u_y);
    return p+v*sect.tuv.dx;
  }
  virtual unsigned int Color(int i) const
  {
    return pts->Color(i);
  }

private:
  PointsApiPoints *pts;
  Point pos;
  Vector u_x;
  Vector u_y;
  Vector light_vec;
};
EXPORT GameApi::PTS GameApi::PointsApi::shadow_points(GameApi::PTS obj,
					       PT pos,
					       V u_x, V u_y,
					       V light_vec)
{
  PointsApiPoints *pts = find_pointsapi_points(e, obj);
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *light = find_vector(e, light_vec);
  return add_points_api_points(e, new ShadowPoints(pts, *pt, *uu_x, *uu_y, *light));
}
#if 0
float *GameApi::PointsApi::point_access(GameApi::PTA pta, int pointnum)
{
  PointArray3 *arr = find_point_array3(e, pta);
  return &arr->array[pointnum*3];
}
unsigned int *GameApi::PointsApi::color_access(GameApi::PTA pta, int pointnum)
{
  PointArray3 *arr = find_point_array3(e, pta);
  return &arr->color[pointnum];
}
void GameApi::PointsApi::update(GameApi::PTA pta)
{
  PointArray3 *arr = find_point_array3(e, pta);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(float)*3, arr->array);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(unsigned int), arr->color);
}
#endif
EXPORT GameApi::PTA GameApi::PointsApi::prepare(GameApi::PTS p)
{
  PointsApiPoints *pts = find_pointsapi_points(e, p);
  int numpoints = pts->NumPoints();
  float *array = new float[numpoints*3];
  unsigned int *color = new unsigned int[numpoints];

  for(int i=0;i<numpoints;i++)
    {
      Point p = pts->Pos(i);
      unsigned int c = pts->Color(i);
      array[i*3+0] = p.x;
      array[i*3+1] = p.y;
      array[i*3+2] = p.z;
      color[i] = c;
    }
  PointArray3 *arr = new PointArray3;
  arr->array = array;
  arr->color = color;
  arr->numpoints = numpoints;
  glGenBuffers(2, &arr->buffer[0]);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color, GL_STATIC_DRAW);
  
  return add_point_array3(e,arr);
}
class PTSFromFloatVolume : public PointsApiPoints
{
public:
  PTSFromFloatVolume(FloatVolumeObject *fo, int numpoints, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z) 
    : fo(fo), array(0), numpoints(numpoints), 
      start_x(start_x), start_y(start_y), start_z(start_z), 
      end_x(end_x), end_y(end_y), end_z(end_z) 
  { prepare(); }
  ~PTSFromFloatVolume() { delete [] array; }
  virtual int NumPoints() const { return numpoints; }
  virtual Point Pos(int i) const
  {
    return Point(array[i*3+0],
		 array[i*3+1],
		 array[i*3+2]);
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
  void prepare()
  {
    array = new float[numpoints*3];
    int index = 0;
    for(;index/3<numpoints;)
      {
	while(1) {
	  Random r;
	  float xp = double(r.next())/r.maximum()*(end_x-start_x)+start_x;
	  float yp = double(r.next())/r.maximum()*(end_y-start_y)+start_y;
	  float zp = double(r.next())/r.maximum()*(end_z-start_z)+start_z;
	  float val = double(r.next())/r.maximum();
	  if (fo->FloatValue(Point(xp,yp,zp))>val)
	    {
	      array[index+0] = xp;
	      array[index+1] = yp;
	      array[index+2] = zp;
	      index+=3;
	      break;
	    }
	}
      }
  }
private:
  FloatVolumeObject *fo;
  float *array;
  int numpoints;
  float start_x, start_y, start_z;
  float end_x, end_y, end_z;
};
EXPORT GameApi::PTS GameApi::PointsApi::from_float_volume(GameApi::FO object,
						   int numpoints,
						   float start_x, float start_y, float start_z,
						   float end_x, float end_y, float end_z)
{
  FloatVolumeObject *fo = find_float_volume(e, object);
  return add_points_api_points(e, new PTSFromFloatVolume(fo, numpoints, start_x, start_y, start_z, end_x, end_y, end_z));
}
EXPORT GameApi::FOA GameApi::FloatVolumeApi::prepare(GameApi::FO object,
					      int numpoints,
					      float start_x, float start_y, float start_z,
					      float end_x, float end_y, float end_z)
{
  FloatVolumeObject *fo = find_float_volume(e, object);
  float *array = new float[numpoints*3];
  int index = 0;
  for(;index/3<numpoints;)
    {
      while(1) {
      Random r;
      float xp = double(r.next())/r.maximum()*(end_x-start_x)+start_x;
      float yp = double(r.next())/r.maximum()*(end_y-start_y)+start_y;
      float zp = double(r.next())/r.maximum()*(end_z-start_z)+start_z;
      float val = double(r.next())/r.maximum();
      if (fo->FloatValue(Point(xp,yp,zp))>val)
	{
	  array[index+0] = xp;
	  array[index+1] = yp;
	  array[index+2] = zp;
	  index+=3;
	  break;
	}
      }
    }
  index/=3;
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->numpoints = index;
  glGenBuffers(1, &arr->buffer);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);

  return add_point_array(e, arr);
}
EXPORT void GameApi::PointsApi::render(GameApi::PTA array)
{
  PointArray3 *arr = find_point_array3(e, array);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  glDrawArrays(GL_POINTS, 0, arr->numpoints);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
  
}
EXPORT void GameApi::FloatVolumeApi::render(FOA array)
{
  PointArray2 *arr = find_point_array(e, array);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_POINTS, 0, arr->numpoints);
  glDisableVertexAttribArray(0);
}

class SubVolumeObject : public VolumeObject
{
public:
  SubVolumeObject(FloatVolumeObject &ff, float start, float end) : ff(ff), start(start), end(end) { }
  bool Inside(Point p) const
  {
    float v = ff.FloatValue(p);
    return v>=start&&v<=end;
  }
private:
  FloatVolumeObject &ff;
  float start, end;
};

EXPORT GameApi::O GameApi::FloatVolumeApi::subvolume(FO f, float start_range, float end_range)
{
  FloatVolumeObject *ff = find_float_volume(e, f);
  return add_volume(e, new SubVolumeObject(*ff, start_range, end_range));
}
EXPORT GameApi::FO GameApi::FloatVolumeApi::function(std::function<float (float x, float y, float z)> f)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  FunctionFloatVolumeObject *ff = new FunctionFloatVolumeObject(f);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_float_volume(e, ff);
}
EXPORT GameApi::COV GameApi::ColorVolumeApi::function(std::function<unsigned int (float x, float y, float z)> f)
{
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  FunctionColorVolumeObject *ff = new FunctionColorVolumeObject(f);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_color_volume(e, ff);
  
}

class ColorVolumeFromFloatVolume : public ColorVolumeObject
{
public:
  ColorVolumeFromFloatVolume(FloatVolumeObject *obj, unsigned int col0, unsigned int col1) : obj(obj), col0(col0), col1(col1) { }
  virtual unsigned int ColorValue(Point p) const
  {
    float val = obj->FloatValue(p);
    return Color::CubicInterpolate(col0, col1, val);
  }

private:
  FloatVolumeObject *obj;
  unsigned int col0;
  unsigned int col1;
};

EXPORT GameApi::COV GameApi::ColorVolumeApi::from_float_volume(FO obj, unsigned int col0, unsigned int col1)
{
  FloatVolumeObject *obj2 = find_float_volume(e, obj);
  return add_color_volume(e, new ColorVolumeFromFloatVolume(obj2, col0, col1));
}

class ColorVolumeFromContinuousBitmap : public ColorVolumeObject
{
public:
  ColorVolumeFromContinuousBitmap(ContinuousBitmap<Color> *bm) : bm(bm) { }
  unsigned int ColorValue(Point p) const
  {
    Color c = bm->Map(p.x,p.y);
    return c.Pixel();
  }
private:
  ContinuousBitmap<Color> *bm;
};

EXPORT GameApi::COV GameApi::ColorVolumeApi::from_continuous_bitmap(CBM bm)
{
  ContinuousBitmap<Color> *bbm = find_continuous_bitmap(e, bm);
  return add_color_volume(e, new ColorVolumeFromContinuousBitmap(bbm));
}

class ColorVolumeFromVolumeObject : public ColorVolumeObject
{
public:
  ColorVolumeFromVolumeObject(VolumeObject *obj, unsigned int col_true, unsigned int col_false) : obj(obj), col_true(col_true), col_false(col_false) { }
  virtual unsigned int ColorValue(Point p) const
  {
    bool b = obj->Inside(p);
    if (b) return col_true;
    return col_false;
  }
private:
  VolumeObject *obj;
  unsigned int col_true;
  unsigned int col_false;
};

class TextureBitmapFromColorVolume : public Bitmap<Color>
{
public:
  TextureBitmapFromColorVolume(FaceCollection *coll, 
			       ColorVolumeObject *colors,
			       int face,
			       int sx, int sy)
    : colors(colors),  sx(sx), sy(sy) 
  {
    p00 = coll->FacePoint(face, 0);
    p01 = coll->FacePoint(face, 1);
    p11 = coll->FacePoint(face, 2);
    p10 = coll->FacePoint(face, 3);
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float vx = float(x)/float(sx);
    float vy = float(y)/float(sy);
    Point pt = Point(Vector(p00)*vx+Vector(p01)*(1.0-vx));
    Point pb = Point(Vector(p10)*vx+Vector(p11)*(1.0-vx));
    Point pm = Point(Vector(pt)*vy+Vector(pb)*(1.0-vy));
    unsigned int colour = colors->ColorValue(pm);
    return Color(colour);
  }

private:
  ColorVolumeObject *colors;
  int sx;
  int sy;
  Point p00, p01, p10, p11;
};

EXPORT GameApi::BM GameApi::ColorVolumeApi::texture_bm(GameApi::P obj, GameApi::COV colors, int face, int sx, int sy)
{
  FaceCollection *coll = find_facecoll(e, obj);
  ColorVolumeObject *colors2 = find_color_volume(e, colors);
  Bitmap<Color> *bm = new TextureBitmapFromColorVolume(coll, colors2, face, sx, sy);
  return add_color_bitmap2(e, bm);
}

EXPORT GameApi::COV GameApi::ColorVolumeApi::from_volume(O obj, unsigned int col_true, unsigned int col_false)
{
  VolumeObject *obj2 = find_volume(e, obj);
  return add_color_volume(e, new ColorVolumeFromVolumeObject(obj2, col_true, col_false));
}

class NormalVectorVolume : public VectorVolumeObject
{
public:
  NormalVectorVolume(DistanceRenderable *r) : r(r) { }
  Vector VectorValue(Point p) const
  {
    float b = r->distance(p);
    float ax = r->distance(p+Vector(0.01,0.0,0.0)) -b;
    float ay = r->distance(p+Vector(0.0,0.01,0.0)) -b;
    float az = r->distance(p+Vector(0.0,0.0,0.01)) -b;
    return Vector(ax,ay,az);
  }
  
private:
  DistanceRenderable *r;
};

class DirectColor : public ColorVolumeObject
{
public:
  DirectColor(VectorVolumeObject *normal) : normal(normal) { }
  unsigned int ColorValue(Point p) const
  {
    Vector v = normal->VectorValue(p);
    v/= v.Dist();
    float r = v.dx;
    float g = v.dy;
    float b = v.dz;
    unsigned int rr = int(r *128.0)+0x80;
    unsigned int gg = int(g *128.0)+0x80;
    unsigned int bb = int(b *128.0)+0x80;
    return (rr<<16) + (gg<<8) + bb + 0xff000000;
  }

private:
  VectorVolumeObject *normal;
};

class Phong : public ColorVolumeObject
{
public:
  Phong(VectorVolumeObject *normal, Point lightpos, Color i_s, Color i_d, Color i_a, float k_s, float k_d, float k_a, float alfa) : normal(normal), L_p(lightpos), i_s(i_s), i_d(i_d), i_a(i_a), k_s(k_s), k_d(k_d), k_a(k_a), alfa(alfa) { }
  unsigned int ColorValue(Point p) const
  {
    Vector v = normal->VectorValue(p);
    v/= v.Dist();
    Vector L_m = L_p - p;
    L_m /= L_m.Dist();
    Vector r_m = 2.0*(Vector::DotProduct(L_m, v))*v-L_m;

    Color Amb = k_a*i_a;
    float DiffA = k_d*Vector::DotProduct(L_m,v);
    if (DiffA < 0.0) DiffA=0.0;
    if (DiffA > 1.0) DiffA=1.0;
    Color Diff = DiffA*i_d;
    float SpecA = k_s*std::pow((Vector::DotProduct(r_m,Vector(0.0,0.0,-1.0))),alfa); 
    if (SpecA < 0.0) SpecA=0.0;
    if (SpecA > 1.0) SpecA=1.0;
    Color Spec = SpecA*i_s;

    return (Amb+Diff+Spec).Pixel();
  }

private:
  VectorVolumeObject *normal;
  Point L_p; // light position
  Color i_s;
  Color i_d;
  Color i_a;
  float k_s;
  float k_d;
  float k_a;
  float alfa;
};

class Shadow : public FloatVolumeObject
{
public:
  Shadow(DistanceRenderable* rend, Vector rd, float mint, float maxt, float k) : rend(rend), rd(rd), mint(mint), maxt(maxt), k(k) { }
  float FloatValue(Point ro) const
  {
    float res = 1.0;
    for(float t = mint; t<maxt; )
      {
	float h = rend->distance(ro+rd*t);
	if (h < 0.001)
	  return 0.0;
	res = std::min(res, k*h/t);
	t += h;
      }
    return res;
  }
private:
  DistanceRenderable *rend;
  Vector rd;
  float mint, maxt;
  float k;
};

EXPORT GameApi::FO GameApi::FloatVolumeApi::shadow(FD fd, V light_dir, float mint, float maxt, float k)
{
  DistanceRenderable *dist = find_distance(e, fd);
  Vector *vv = find_vector(e, light_dir);
  return add_float_volume(e, new Shadow(dist, *vv, mint, maxt, k));
}

class MixColorVolume : public ColorVolumeObject
{
public:
  MixColorVolume(ColorVolumeObject *o1, ColorVolumeObject *o2, float val) : o1(o1), o2(o2),val(val) { }
  
  unsigned int ColorValue(Point p) const
  {
    unsigned int c1 = o1->ColorValue(p);
    unsigned int c2 = o2->ColorValue(p);
    return Color::CubicInterpolate(c1,c2,val);
  }
private:
  ColorVolumeObject *o1, *o2;
  float val;
};
EXPORT GameApi::COV GameApi::ColorVolumeApi::mix(COV c1, COV c2, float val)
{
  ColorVolumeObject *cc1 = find_color_volume(e, c1);
  ColorVolumeObject *cc2 = find_color_volume(e, c2);
  return add_color_volume(e, new MixColorVolume(cc1,cc2,val));
}

EXPORT GameApi::COV GameApi::ColorVolumeApi::phong(VO fd, PT light_pos, CO i_s, CO i_d, CO i_a, float k_s, float k_d, float k_a, float alfa)
{
  Point *light_pos2 = find_point(e, light_pos);
  Color *i_s2 = find_color(e, i_s);
  Color *i_d2 = find_color(e, i_d);
  Color *i_a2 = find_color(e, i_a);
  VectorVolumeObject *dist = find_vector_volume(e, fd);
  return add_color_volume(e, new Phong(dist, *light_pos2, *i_s2, *i_d2, *i_a2, k_s,k_d,k_a,alfa));
}
EXPORT GameApi::COV GameApi::ColorVolumeApi::directcolor(VO fd)
{
  VectorVolumeObject *dist = find_vector_volume(e, fd);
  return add_color_volume(e, new DirectColor(dist));
}
EXPORT GameApi::VO GameApi::VectorVolumeApi::normal(FD fd)
{
  DistanceRenderable *dist = find_distance(e, fd);
  return add_vector_volume(e, new NormalVectorVolume(dist));
}

class FloatVolumeFromVolume : public FloatVolumeObject
{
public:
  FloatVolumeFromVolume(VolumeObject &obj, float false_val, float true_val) : obj(obj),false_val(false_val), true_val(true_val) { }
  float FloatValue(Point p) const
  {
    if (obj.Inside(p)) { return true_val; }
    return false_val;
  }
private:
  VolumeObject &obj;
  float false_val, true_val;
};

EXPORT GameApi::FO GameApi::FloatVolumeApi::from_volume(GameApi::O obj, float false_val, float true_val)
{
  VolumeObject *obj_ = find_volume(e, obj);
  FloatVolumeObject *obj2 = new FloatVolumeFromVolume(*obj_, false_val, true_val);
  return add_float_volume(e, obj2);
}
class FloatVolumeFromBitmap : public FloatVolumeObject
{
public:
  FloatVolumeFromBitmap(FloatBitmap *bm, float start_x, float end_x,
			float start_y, float end_y,
			float start_z, float end_z) : bm(bm),
						      start_x(start_x),
						      end_x(end_x),
						      start_y(start_y),
						      end_y(end_y),
						      start_z(start_z),
						      end_z(end_z) { }
  float FloatValue(Point p) const
  {
    if (p.x<start_x) return 0.0;
    if (p.x>end_x) return 0.0;
    if (p.y<start_y) return 0.0;
    if (p.y>end_y) return 0.0;
    if (p.z<start_z) return 0.0;
    if (p.z>end_z) return 0.0;
    float x = p.x-start_x;
    x/=end_x-start_x;
    x*=bm->bitmap->SizeX();
    float y = p.y-start_y;
    y/=end_y-start_y;
    y*=bm->bitmap->SizeY();
    return bm->bitmap->Map((int)x,(int)y);
  }
private:
  FloatBitmap *bm;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};
EXPORT GameApi::FO GameApi::FloatVolumeApi::from_float_bitmap(GameApi::FB bm, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  FloatBitmap *bm2 = find_float_bitmap(e, bm);
  FloatVolumeObject *obj = new FloatVolumeFromBitmap(bm2, start_x, end_x, start_y, end_y, start_z, end_z);
  return add_float_volume(e,obj);
}

class FloatVolumeObjectFunction : public Function<Point, float>
{
public:
  FloatVolumeObjectFunction(FloatVolumeObject *obj, float value) : obj(obj), value(value) { }
  float Index(Point p) const
  {
    float val = obj->FloatValue(p);
    val -= value;
    //std::cout << "Index: " << p << ":" << val << std::endl;
    return val;
  }
private:
  FloatVolumeObject *obj;
  float value;
};

class RayTrace3 : public Bitmap<Color>
{
public:
  RayTrace3(FloatVolumeObject *obj,
	    int sx, int sy,
	    Point pos,
	    Vector u_x,
	    Vector u_y,
	    Vector u_z,
	    float surface_value) : obj(obj), sx(sx), sy(sy), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), surf_val(surface_value) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    Point pos_0 = pos + float(x)/float(sx)*u_x + float(y)/float(sy)*u_y;
    Vector vec_0 = u_z;
    //std::cout << pos_0 << " " << vec_0 << ":" << std::endl;
    Ray r(pos_0, vec_0); // float->Point
    FloatVolumeObjectFunction func(obj, surf_val); // Point -> float
    SolvableCompose<Point> compose(r, func);
    bool success = false;
    float val = SolveWithFailure(compose, 0.0, 200.0, success);
    //std::cout << x << " " << y << ":" << val << std::endl;
    return Color(int(val*255.0),int(val*255.0), int(val*255.0), int(val*255.0));    
  }
public:
  FloatVolumeObject *obj;
  int sx;
  int sy;
  Point pos;
  Vector u_x;
  Vector u_y;
  Vector u_z;
  float surf_val;
};

float distance2(float x, float y, float z)
{
  return sqrt(x*x+y*y+z*z);
}

EXPORT GameApi::FO GameApi::FloatVolumeApi::distance()
{
  return function(distance2);
}

struct TorusData {
  Point center;
  Vector u_x;
  Vector u_y;
  float radius;
};

float torus_distance(float x, float y, float z, void *data)
{
  TorusData *dt = (TorusData*)data;
  Point p( x,y,z );
  Plane pl(dt->center, dt->u_x, dt->u_y);
  float xx = pl.CoordsX(p);
  float yy = pl.CoordsY(p);
  float r = sqrt(xx*xx+yy*yy);
  xx/=r;
  yy/=r;
  xx*=dt->radius;
  yy*=dt->radius;
  Point2d p2d = { xx,yy };
  Point pp = pl.Navigate(p2d);
  Vector v = pp-p;
  return v.Dist();
}

EXPORT GameApi::FO GameApi::FloatVolumeApi::torusdistance(PT center, V u_x, V u_y, float radius)
{
#ifndef EMSCRIPTEN

  TorusData *dt = new TorusData;
  dt->center = *find_point(e,center);
  dt->u_x = *find_vector(e,u_x);
  dt->u_y = *find_vector(e,u_y);
  dt->radius = radius;
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  return function(std::bind(torus_distance, _1,_2,_3,(void*)dt));
#endif  
}
EXPORT GameApi::FO GameApi::FloatVolumeApi::minimum(FO f1, FO f2)
{
  FloatVolumeObject *obj = find_float_volume(e, f1);
  FloatVolumeObject *obj2 = find_float_volume(e, f2);
  return add_float_volume(e, new MinFloatVolumeObject(obj, obj2));
}
EXPORT GameApi::FO GameApi::FloatVolumeApi::maximum(FO f1, FO f2)
{
  FloatVolumeObject *obj = find_float_volume(e, f1);
  FloatVolumeObject *obj2 = find_float_volume(e, f2);
  return add_float_volume(e, new MaxFloatVolumeObject(obj, obj2));
}
#if 0
GameApi::FO GameApi::FloatVolumeApi::plus(FO f1, FO f2)
{
  FloatVolumeObject *obj = find_float_volume(e, f1);
  FloatVolumeObject *obj2 = find_float_volume(e, f2);
  return add_float_volume(e, new PlusFloatVolumeObject(obj, obj2));
}
#endif
EXPORT GameApi::FO GameApi::FloatVolumeApi::move(FO f1, float dx, float dy, float dz)
{
  FloatVolumeObject *next = find_float_volume(e, f1);
  return add_float_volume(e, new MoveFloatVolumeObject(next, dx,dy,dz));
}

EXPORT GameApi::BM GameApi::FloatVolumeApi::raytrace(GameApi::FO object, 
					      int sx, int sy,
					      PT ray_0, PT ray_x, PT ray_y, PT ray_z, float surface_value)
{
  FloatVolumeObject *obj = find_float_volume(e, object);
  Point *pos = find_point(e, ray_0);
  Point *pos_x = find_point(e, ray_x);
  Point *pos_y = find_point(e, ray_y);
  Point *pos_z = find_point(e, ray_z);
  Vector u_x = *pos_x-*pos;
  Vector u_y = *pos_y-*pos;
  Vector u_z = *pos_z-*pos;
  return add_color_bitmap(e, new RayTrace3(obj, sx,sy, *pos, u_x, u_y, u_z, surface_value));
}

EXPORT GameApi::MatrixApi::MatrixApi(Env &e) : e(e) { }

EXPORT GameApi::M GameApi::MatrixApi::identity()
{
  return add_matrix(e, new SimpleMatrix(Matrix::Identity()));
}
EXPORT GameApi::M GameApi::MatrixApi::xrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::XRotation(rot)));
}
EXPORT GameApi::M GameApi::MatrixApi::yrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::YRotation(rot)));
}
EXPORT GameApi::M GameApi::MatrixApi::zrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::ZRotation(rot)));
}
EXPORT GameApi::M GameApi::MatrixApi::trans(float x, float y, float z)
{
  return add_matrix(e, new SimpleMatrix(Matrix::Translate(x,y,z)));
}
EXPORT GameApi::M GameApi::MatrixApi::scale(float sx, float sy, float sz)
{
  return add_matrix(e, new SimpleMatrix(Matrix::Scale(sx,sy,sz)));
}
EXPORT GameApi::M GameApi::MatrixApi::inverse(M mat)
{
  Matrix m = find_matrix(e, mat);
  return add_matrix(e, new SimpleMatrix(Matrix::Inverse(m)));
}
EXPORT GameApi::M GameApi::MatrixApi::mult(M m1, M m2)
{
  Matrix ma = find_matrix(e, m1);
  Matrix mb = find_matrix(e, m2);
  return add_matrix(e, new SimpleMatrix(ma * mb));
}
EXPORT GameApi::M GameApi::MatrixApi::perspective(float fovy, float aspect, float near0, float far0)
{
  Matrix m = Matrix::Perspective(fovy, aspect, near0, far0);
  return add_matrix(e, new SimpleMatrix(m));
}
EXPORT GameApi::PT GameApi::MatrixApi::mult(PT point, M matrix)
{
  Matrix ma = find_matrix(e, matrix);
  Point *pt = find_point(e, point);
  
  Point p2 = (*pt) * ma;
  return add_point(e, p2.x,p2.y,p2.z);
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

GameApi::LAY GameApi::LayoutApi::root(int sx, int sy)
{
  return add_layout(e, new RootLayout(sx,sy));
}
GameApi::LAY GameApi::LayoutApi::split_y(LAY l, int id, int num)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutY(*ll, id, num));
}
GameApi::LAY GameApi::LayoutApi::split_x(LAY l, int id, int num)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitLayoutX(*ll, id, num));
}
GameApi::LAY GameApi::LayoutApi::split_xy(LAY l, int id, int num_x, int num_y)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new SplitXYLayout(*ll, id, num_x, num_y));
}
GameApi::LAY GameApi::LayoutApi::margin(LAY l, int id, int lx, int rx, int ty, int by)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new MarginLayout(*ll, id, lx,rx,ty,by));  
}
GameApi::LAY GameApi::LayoutApi::center(LAY l, int id, int cx, int cy)
{
  Layout *ll = find_layout(e, l);
  return add_layout(e, new CenterLayout(*ll, id, cx, cy));
}
GameApi::LAY GameApi::LayoutApi::array(LAY *array, int *id, int size)
{
  std::vector<Layout*> *vec = new std::vector<Layout*>;
  for(int i=0;i<size;i++)
    {
      vec->push_back(find_layout(e,array[i]));
    }
  return add_layout(e, new ArrayLayout(&(*vec)[0], id, size));
}

int GameApi::LayoutApi::count(LAY l)
{
  Layout *ll = find_layout(e, l);
  return ll->count();
}
int GameApi::LayoutApi::pos_x(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).x;
}
int GameApi::LayoutApi::pos_y(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).y;
}

int GameApi::LayoutApi::size_x(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).width;
}
int GameApi::LayoutApi::size_y(LAY l, int id)
{
  Layout *ll = find_layout(e, l);
  return ll->get(id).height;
}

#if 0
GameApi::DR GameApi::DrawApi::label(LAY l, int id, std::string str, Ft font)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(0, tl, br);
  Draw *dr1 = new DrawProp(*dr, "label", str);
  Draw *dr2 = new DrawProp(*dr1, "font", font.id);
  return add_draw(e, dr2);
}

GameApi::DR GameApi::DrawApi::icon(LAY l, int id, BM bm)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(1, tl, br);
  Draw *dr1 = new DrawProp(*dr, "bitmap", bm.id);
  return add_draw(e, dr1);
}

GameApi::DR GameApi::DrawApi::rect(LAY l, int id, unsigned int color)
{
  Layout *ll = find_layout(e, l);
  TRect r = ll->get(id);
  Point2d tl = { r.x, r.y };
  Point2d br = { r.x+r.width, r.y+r.height };
  Draw *dr = new DrawEmpty(2, tl, br);
  Draw *dr1 = new DrawProp(*dr, "rect", color);
  return add_draw(e, dr1);
}
GameApi::DR GameApi::DrawApi::scroll(DR dr, int delta_x, int delta_y)
{
  Draw *ddr = find_draw(e, dr);
  Point2d p = { 0.0, 0.0 };
  Draw *drx = new DrawEmpty(3, p,p); 
  Draw *dr1 = new DrawProp(*dr, "delta_x", delta_x);
  Draw *dr2 = new DrawProp(*dr1, "delta_y", delta_y);
  Draw *dr3 = new DrawProp(*dr2, "draw", ddr);
  return add_draw(e, dr3);
}
GameApi::DR GameApi::DrawApi::scroll_area(DR dr, LAY l, int id);
GameApi::DR GameApi::DrawApi::cliprect(DR cmds, LAY l, int id);
#endif

class PointApiPointFunction : public PointsApiPoints
{
public:
  PointApiPointFunction(GameApi::Env &e, std::function<GameApi::PT(int pointnum)> f, int numpoints) : e(e), f(f), numpoints(numpoints) { }
  virtual int NumPoints() const
  {
    return numpoints;
  }
  virtual Point Pos(int i) const
  {
    GameApi::PT pt = f(i);
    Point *pts = find_point(e, pt);
    return *pts;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  GameApi::Env &e;
  std::function<GameApi::PT(int pointnum)> f;
  int numpoints;
};

class PointsApiColorFunction : public PointsApiPoints
{
public:
  PointsApiColorFunction(GameApi::Env &e, PointsApiPoints *orig, std::function<unsigned int(int pointnum, GameApi::PT pos)> f) : e(e), orig(orig), f(f) { }
  virtual int NumPoints() const { return orig->NumPoints(); }
  virtual Point Pos(int i) const { return orig->Pos(i); }
  virtual unsigned int Color(int i) const
  {
    Point p = Pos(i);
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    unsigned int color = f(i, pt);
    return color;
  }
private:
  GameApi::Env &e;
  PointsApiPoints *orig;
  std::function<unsigned int(int pointnum, GameApi::PT pos)> f;
};

class LineCollectionFunction : public LineCollection
{
public:
  LineCollectionFunction(GameApi::Env &e, std::function<GameApi::PT (int linenum, bool id)> f, int numlines) :  ev(e), f(f), numlines(numlines) { }
  virtual int NumLines() const { return numlines; }
  virtual Point LinePoint(int line, int point) const
  {
    bool b = point==0 ? 0 : 1;
    GameApi::PT pt = f(line, b);
    float x = ev.point_api.pt_x(pt);
    float y = ev.point_api.pt_y(pt);
    float z = ev.point_api.pt_z(pt);
    return Point(x,y,z);
  }

private:
  //GameApi::Env &e;
  mutable GameApi::EveryApi ev;
  std::function<GameApi::PT (int linenum, bool id)> f;
  int numlines; 
};

EXPORT GameApi::PTS GameApi::PointsApi::function(std::function<GameApi::PT(int pointnum)> f, int numpoints)
{
  return add_points_api_points(e, new PointApiPointFunction(e, f, numpoints));
}
EXPORT GameApi::PTS GameApi::PointsApi::color_function(PTS orig, std::function<unsigned int(int pointnum, PT pos)> f)
{
  PointsApiPoints *pts = find_pointsapi_points(e, orig);
  return add_points_api_points(e, new PointsApiColorFunction(e, pts, f));
}
class ColorLineCollection : public LineCollection
{
public:
  ColorLineCollection(LineCollection *coll, unsigned int color1, unsigned int color2) : coll(coll), color1(color1), color2(color2) { }
  virtual int NumLines() const { return coll->NumLines(); }
  virtual Point LinePoint(int line, int point) const { return coll->LinePoint(line,point); }
  virtual unsigned int LineColor(int line, int point) const { if (point==0) return color1; else return color2; }
private:
  LineCollection *coll;
  unsigned int color1;
  unsigned int color2;
};
EXPORT GameApi::LI GameApi::LinesApi::change_color(GameApi::LI li, unsigned int color)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new ColorLineCollection(lines, color, color));
}
EXPORT GameApi::LI GameApi::LinesApi::change_color(GameApi::LI li, unsigned int color1, unsigned int color2)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new ColorLineCollection(lines, color1, color2));
}
EXPORT GameApi::LI GameApi::LinesApi::function(std::function<GameApi::PT (int linenum, bool id)> f, int numlines)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);

  return add_line_array(e, new LineCollectionFunction(e, f, numlines));
}
class LinesFromPlane : public LineCollection
{
public:
  LinesFromPlane(PlanePoints2d *plane) : plane(plane) { }
  virtual int NumLines() const {
    int s = plane->Size();
    int line=0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	switch(t) {
	case PlanePoints2d::EMove:
	  break;
	case PlanePoints2d::ELineTo:
	  line++;
	  break;
	case PlanePoints2d::EConic:
	case PlanePoints2d::ECubic:
	  std::cout << "Use remove_splines() first before doing from_plane()" << std::endl;
	  break;
	};
      }
    return line;
  }
  virtual Point LinePoint(int line_num, int point) const 
  {
    int s = plane->Size();
    int line=0;
    Point2d pos = { 0.0, 0.0 };
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	switch(t) {
	case PlanePoints2d::EMove:
	  pos = plane->Map(i);
	  break;
	case PlanePoints2d::ELineTo:
	  if (line_num == line && point==0) {
	    Point p(pos.x, pos.y, 0.0);
	    return p;
	  }
	  if (line_num == line && point==1) {
	    Point2d pp = plane->Map(i);
	    Point p(pp.x, pp.y, 0.0);
	    return p;
	  }
	  pos = plane->Map(i);
	  line++;
	  break;
	case PlanePoints2d::EConic:
	case PlanePoints2d::ECubic:
	  std::cout << "Use remove_splines() first before doing from_plane()" << std::endl;
	  break;
	};
      }
    Point p(pos.x, pos.y, 0.0);
    return p;
  }
private:
  PlanePoints2d *plane;
};
EXPORT GameApi::LI GameApi::LinesApi::from_plane(GameApi::PL plane)
{
  PlanePoints2d *plane2 = find_plane(e,plane);
  return add_line_array(e, new LinesFromPlane(plane2));
}
EXPORT GameApi::LI GameApi::LinesApi::from_points(GameApi::PC points, bool loops)
{
  PointCollection *point_coll = find_pointcoll_array(e,points);
  return add_line_array(e, new ContinuousLines(point_coll, loops));
}
EXPORT GameApi::LI GameApi::LinesApi::from_polygon2(GameApi::P poly1, GameApi::P poly2)
{
  FaceCollection *p1 = find_facecoll(e, poly1);
  FaceCollection *p2 = find_facecoll(e, poly2);
  return add_line_array(e, new AnimLines(p1, p2));
}
class SliceLineCollection : public LineCollection
{
public:
  SliceLineCollection(FaceCollection *coll, Point pos, Vector u_x, Vector u_y) : coll(coll), pos(pos), u_x(u_x), u_y(u_y) { }
  virtual int NumLines() const { return coll->NumFaces(); }
  virtual Point LinePoint(int line, int point) const 
  {
    Point p1 = coll->FacePoint(line, 0);
    Point p2 = coll->FacePoint(line, 1);
    Point p3 = coll->FacePoint(line, 2);
    Plane pl(pos, u_x, u_y);
    Point2d res1,res2;
    bool b = pl.TriangleIntersection(p1,p2,p3, res1,res2);
    if (b)
      {
	if (point==0) { return Point(res1.x, res1.y, 0.0); }
	if (point==1) { return Point(res2.x, res2.y, 0.0); }
      }
    else
      {
	return Point(0.0,0.0,0.0);
      }
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  FaceCollection *coll;
  Point pos;
  Vector u_x, u_y;
};
class NormalsLineCollection : public LineCollection
{
public:
  NormalsLineCollection(FaceCollection *coll, float length) : coll(coll),length(length) { }
  virtual int NumLines() const { return coll->NumFaces(); }
  virtual Point LinePoint(int line, int point) const 
  {
    int s = coll->NumPoints(line);
    Point avg(0.0, 0.0, 0.0);
    for(int i=0;i<s;i++)
      {
	Point p = coll->FacePoint(line, i);
	avg += Vector(p);
      }
    Vector avg2 = avg;
    avg2/=float(s);
    if (point==0)

      {
	return Point(avg2);
      }
    if (point==1)
      {
	Vector v = coll->PointNormal(line, 0);
	v/=v.Dist();
	v*=length;
	return Point(avg2)+v;
      }
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }  
private:
  FaceCollection *coll;
  float length;
};
EXPORT GameApi::LI GameApi::LinesApi::normals_from_polygon(GameApi::P p, float length)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_line_array(e, new NormalsLineCollection(coll, length));
}
class MatrixLineCollection : public LineCollection
{
public:
  MatrixLineCollection(Matrix m, LineCollection &coll) : m(m),coll(coll) { }
  virtual int NumLines() const { return coll.NumLines(); }
  virtual Point LinePoint(int line, int point) const 
  {
    return coll.LinePoint(line,point)*m;
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }  

private:
  Matrix m;
  LineCollection &coll;
};
EXPORT GameApi::LI GameApi::LinesApi::translate(LI lines, float dx, float dy, float dz)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::Translate(dx,dy,dz),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::scale(LI lines, float dx, float dy, float dz)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::Scale(dx,dy,dz),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::render_slice_2d(GameApi::P p, GameApi::PT pos, GameApi::V u_x, GameApi::V u_y)
{
  FaceCollection *coll = find_facecoll(e, p);
  Point *pp1 = find_point(e, pos);
  Vector *v1 = find_vector(e, u_x);
  Vector *v2 = find_vector(e, u_y);
  return add_line_array(e, new SliceLineCollection(coll,*pp1, *v1, *v2));
}

class LineAnim : public ForwardFaceCollection
{
public:
  LineAnim(FaceCollection &p, LineCollection &c, float val) : ForwardFaceCollection(p), p(p), c(c), val(val) { }
  Point FacePoint(int face, int point) const
  {
    int count = 0;
    for(int i=0;i<face;i++) 
      {
	count+=p.NumPoints(i);
      }
    count+=point;
    Point p1 = c.LinePoint(count, 0);
    Point p2 = c.LinePoint(count, 1);
    return Point((1.0-val)*Vector(p1)+val*Vector(p2));
  }
private:
  FaceCollection &p;
  LineCollection &c;
  float val;
};
EXPORT GameApi::P GameApi::LinesApi::line_anim(GameApi::P poly, GameApi::LI lines, float val)
{
  FaceCollection *p = find_facecoll(e, poly);
  LineCollection *c = find_line_array(e, lines);
  return add_polygon(e, new LineAnim(*p, *c, val),1);
}
EXPORT GameApi::LI GameApi::LinesApi::from_polygon(GameApi::P poly)
{
  FaceCollection *poly2 = find_facecoll(e, poly);
  return add_line_array(e, new OutlineFaces(*poly2));
}
class BorderFromBoolBitmap : public LineCollection
{
public:
  BorderFromBoolBitmap(Bitmap<bool> &bm, float start_x, float end_x, float start_y, float end_y, float z) : bm(bm),start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { Store(); }
  virtual int NumLines() const { return p1.size(); }
  virtual Point LinePoint(int line, int point) const
  {
    if (point==0) { return p1[line]; }
    return p2[line];
  }

  void Store() {
    int sx = bm.SizeX();
    int sy = bm.SizeY();
    for(int x=0;x<sx-1;x++)
      {
	for(int y=0;y<sy-1;y++)
	  {
	    bool b00 = bm.Map(x,y);
	    bool b01 = bm.Map(x+1,y);
	    bool b10 = bm.Map(x,y+1);
	    bool b11 = bm.Map(x+1,y+1);

	    float xx0 = start_x + (end_x-start_x)*x/sx;
	    float xx1 = start_x + (end_x-start_x)*(x+1)/sx;
	    float xx2 = start_x + (end_x-start_x)*(x+2)/sx;

	    float yy0 = start_y + (end_y-start_y)*y/sy;
	    float yy1 = start_y + (end_y-start_y)*(y+1)/sy;
	    float yy2 = start_y + (end_y-start_y)*(y+2)/sy;

	    if ((b00 || b01) && !(b00 && b01)) // TOPLEFT-TOPRIGHT
	      {
		p1.push_back(Point(xx1,yy0,z));
		p2.push_back(Point(xx1,yy1,z));
	      }
	    if ((b00 || b10) && !(b00 && b10)) // topleft-bottomleft
	      {
		p1.push_back(Point(xx0,yy1,z));
		p2.push_back(Point(xx1,yy1,z));
	      }
	    if ((b10||b11) && !(b10 && b11)) // bottomleft - bottomright
	      {
		p1.push_back(Point(xx1,yy1,z));
		p2.push_back(Point(xx1,yy2,z));
	      }
	    if ((b01 || b11) && !(b01 && b11)) // topright - bottomright
	      {
		p1.push_back(Point(xx1,yy1,z));
		p2.push_back(Point(xx2,yy1,z));
	      }
		    
	  }
      }
  }

private:
  Bitmap<bool> &bm;
  float start_x, end_x;
  float start_y, end_y;
  float z;
  std::vector<Point> p1;
  std::vector<Point> p2;
};

EXPORT GameApi::LI GameApi::LinesApi::border_from_bool_bitmap(GameApi::BB b, float start_x, float end_x, float start_y, float end_y, float z)
{
  BoolBitmap *bb2 = find_bool_bitmap(e, b);
  Bitmap<bool> *bb = bb2->bitmap;
  return add_line_array(e, new BorderFromBoolBitmap(*bb, start_x, end_x, start_y, end_y, z));
}
EXPORT void GameApi::LinesApi::render(LLA l)
{
  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);
  glBindVertexArray(array->vao[0]);
  glDrawArrays(GL_LINES, 0, array->numpoints);
  //glDisableVertexAttribArray(0);
}

class SphereDistanceRenderable : public DistanceRenderable
{
public:
  SphereDistanceRenderable(Point center, float radius) : center(center), radius(radius) { }
  float distance(Point p) const {
    p-=center;
    float dist = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    dist -= radius;
    return dist;
  }
  std::string shader() const { return ""; }
  int varnum() const { return 0; }
private:
  Point center;
  float radius;
};

class LineDistance : public DistanceRenderable
{
public:
  LineDistance(Point start, Point end, float dist) : start(start), end(end), dist(dist) { }

  float distance(Point p) const {
    Vector n = end-start;
    n/=n.Dist();
    Vector vv = (start-p)-(Vector::DotProduct((start-p),n)*n);
    float v = vv.Dist();
    return v-dist;
  }
  std::string shader() const { return ""; }
  int varnum() const { return 0; }
private:
  Point start, end;
  float dist;
};

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::sphere(PT center, float radius)
{
  Point *cent = find_point(e, center);
  return add_distance(e, new SphereDistanceRenderable(*cent, radius));
}
EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::line(PT start, PT end, float dist)
{
  Point *st = find_point(e, start);
  Point *en = find_point(e, end);
  return add_distance(e, new LineDistance(*st, *en, dist));
}
class MinDistance2 : public DistanceRenderable
{
public:
  MinDistance2(DistanceRenderable &r1, DistanceRenderable &r2) : r1(r1), r2(r2) { }
  float distance(Point p) const 
  { 
    float d1 = r1.distance(p);
    float d2 = r2.distance(p);
    return std::min(d1,d2);
  }
  std::string shader() const { return ""; }
  int varnum() const { return 0; }
private:
  DistanceRenderable &r1;
  DistanceRenderable &r2;
};

class AndNotDistance : public DistanceRenderable
{
public:
  AndNotDistance(DistanceRenderable &r1, DistanceRenderable &r2) : r1(r1), r2(r2) { }
  float distance(Point p) const 
  { 
    float d1 = r1.distance(p);
    float d2 = r2.distance(p);
    return std::max(d1,-d2);
  }
  std::string shader() const { return ""; }
  int varnum() const { return 0; }
private:
  DistanceRenderable &r1;
  DistanceRenderable &r2;
};

class RoundCubeDistance : public DistanceRenderable
{
public:
  RoundCubeDistance(Point start, Point end, float r) : start(start), end(end), r(r) {}
  float distance(Point p) const
  {
    p-=Vector(start);
    Vector b = end-start;
    if (p.x<0.0) p.x=-p.x;
    if (p.y<0.0) p.y=-p.y;
    if (p.z<0.0) p.z=-p.z;
    p-=Vector(b);
    Point bb(std::max(p.x,0.0f), std::max(p.y,0.0f), std::max(p.z,0.0f));
    float dist = Vector(bb).Dist();
    dist -= r;
    return dist;
  }
private:
  Point start, end;
  float r;
};

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::round_cube(float start_x, float end_x,
							float start_y, float end_y,
							float start_z, float end_z, float r)
{
  Point start(start_x, start_y, start_z);
  Point end(end_x, end_y, end_z);
  return add_distance(e, new RoundCubeDistance(start, end, r));
}

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::min(FD fd1, FD fd2)
{
  DistanceRenderable *ff1 = find_distance(e, fd1);
  DistanceRenderable *ff2 = find_distance(e, fd2);
  return add_distance(e, new MinDistance2(*ff1, *ff2));
}

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::and_not(FD fd1, FD fd2)
{
  DistanceRenderable *ff1 = find_distance(e, fd1);
  DistanceRenderable *ff2 = find_distance(e, fd2);
  return add_distance(e, new AndNotDistance(*ff1, *ff2));
}

class RenderDistance : public Bitmap<Color>
{
public:
  RenderDistance(Point pos, Vector u_x, Vector u_y, Vector u_z, DistanceRenderable &dist, ColorVolumeObject &colors, int sx, int sy) 
    : pos(pos), u_x(u_x), u_y(u_y), m_u_z(u_z), dist(dist), colors(colors), sx(sx), sy(sy) { 
    m_u_z/=m_u_z.Dist();
  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    Point p = pos + u_x*x/sx+u_y*y/sy;
    int count = 0;
    while(1) {
      float d = dist.distance(p);
      if (d<1.0) break;
      if (count >30) return Color(0x00000000);
      p+=m_u_z*d;
      count ++;
    }
    unsigned int col = colors.ColorValue(p);
    return Color(col);
  }
private:
  Point pos;
  Vector u_x,u_y, m_u_z;
  DistanceRenderable &dist;
  ColorVolumeObject &colors;
  int sx,sy;

};

EXPORT GameApi::BM GameApi::DistanceFloatVolumeApi::render(FD obj, COV colors, PT pos, V u_x, V u_y, V u_z, int sx, int sy)
{
  DistanceRenderable *dist = find_distance(e, obj);
  ColorVolumeObject *colorsI = find_color_volume(e, colors);
  Point *posI = find_point(e, pos);
  Vector *u_xI = find_vector(e, u_x);
  Vector *u_yI = find_vector(e, u_y);
  Vector *u_zI = find_vector(e, u_z);
  Bitmap<Color> *bm = new RenderDistance(*posI, *u_xI, *u_yI, *u_zI, *dist, *colorsI, sx,sy);
  return add_color_bitmap2(e, bm);
}

#if 0
class BezierArray : public PointCollection
{
public:
  BezierArray() { }
  void RunRecurse(Point p1, Point p1c, Point p2, Point p2c, int count) 
  {
    if (count==0) 
      {
	vec.push_back(p1);
	vec2.push_back(p2);
	return;
      }    
    
  }
private:
  std::vector<Point> vec;
  std::vector<Point> vec2;
};



GameApi::PC GameApi::PointCollectionApi::bezier(PT *array, PT *control_array, int size, int iteration_count)
{
}
#endif

class PointCollectionFunction : public PointCollection
{
public:
  PointCollectionFunction(GameApi::Env &e, std::function<GameApi::PT (int)> f, int count) : env(e), f(f), count(count) { }
  int Size() const { return count; }
  Point Index(int i) const {
    GameApi::PT p = f(i);
    Point pp = *(find_point(env, p));
    return pp;
  }
private:
  GameApi::Env &env;
  std::function<GameApi::PT (int)> f;
  int count;
};

GameApi::FloatApi::FloatApi(Env &e) : e(e) { }

class LazySingle : public LazyValue<float>
{
public:
  LazySingle(float v) : v(v) { }
  float get() const { return v; }
private:
  float v;
};
class LazyRef : public LazyValue<float>
{
public:
  LazyRef(float* v) : v(v) { }
  float get() const { return *v; }
private:
  float *v;
};
class LazyIndex : public LazyValue<float>
{
public:
  LazyIndex(float* v, int pos) : v(v), pos(pos) { }
  float get() const { return v[pos]; }
private:
  float *v;
  int pos;
};

GameApi::F GameApi::FloatApi::value(float v)
{
  return add_float(e, new LazySingle(v));
}
GameApi::F GameApi::FloatApi::ref(float *v)
{
  return add_float(e, new LazyRef(v));
}
GameApi::F GameApi::FloatApi::array_index(float *array, int pos)
{
  return add_float(e, new LazyIndex(array, pos));
}
float GameApi::FloatApi::get_value(F f)
{
  LazyValue<float> *value = find_float(e, f);
  return value->get();
}
GameApi::FloatArrayApi::FloatArrayApi(Env &e) : e(e) { }
GameApi::FA GameApi::FloatArrayApi::array(float *array, int size)
{
  return add_float_array(e, new NativeArray<float>(array, size));
}
template<class T> 
class RefArray : public Array<int,T>
{
public:
  RefArray(std::vector<LazyValue<T>*> vec) : vec(vec) {}
  int Size() const { return vec.size(); }
  T Index(int i) const { return vec[i]->get(); }
private:
  std::vector<LazyValue<T>*> vec;
};

class RampArray : public Array<int,float>
{
public:
  RampArray(float start_value, float end_value, int steps)
    : start(start_value), end(end_value), steps(steps)
  {
  }
  int Size() const { return steps; }
  float Index(int i) const {
    float val = start + i*(end-start)/steps;
    return val;
  }
private:
  float start;
  float end;
  int steps;
};
class SpanArrays : public Bitmap<Color>
{
public:
  SpanArrays(Array<int,float> &a1, Array<int,float> &a2, ContinuousBitmap<Color> *f) : a1(a1) ,a2(a2), f(f) { } 
  int SizeX() const { return a1.Size(); }
  int SizeY() const { return a2.Size(); }
  Color Map(int x, int y) const
  {
    float val1 = a1.Index(x);
    float val2 = a2.Index(y);
    return f->Map(val1, val2);
  }
private:
  Array<int,float> &a1;
  Array<int,float> &a2;
  ContinuousBitmap<Color> *f;
};
EXPORT GameApi::TXID GameApi::FrameBufferApi::tex_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->texture;
  return id;
}
EXPORT GameApi::TXID GameApi::FrameBufferApi::depth_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->depthbuffer;
  return id;
}
EXPORT GameApi::FBO GameApi::FrameBufferApi::create_fbo(int sx, int sy)
{
  GLuint fbo_name;
  glGenFramebuffers(1, &fbo_name);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_name);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sx,sy, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  GLuint depth_texture;
  glGenRenderbuffers(1, &depth_texture);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_texture);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, sx, sy);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  return add_fbo(e, fbo_name, texture, depth_texture, sx,sy);
}
EXPORT void GameApi::FrameBufferApi::config_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, priv->texture, 0);
  GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, DrawBuffers);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
EXPORT void GameApi::FrameBufferApi::bind_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glBindRenderbuffer(GL_RENDERBUFFER, priv->depthbuffer);
  glViewport(0,0,priv->sx,priv->sy);
}
EXPORT void GameApi::FrameBufferApi::bind_screen(int sx, int sy)
{
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glViewport(0,0,sx,sy);
}
GameApi::BM GameApi::FloatArrayApi::span_arrays(FA fa1, FA fa2, CBM f)
{
  Array<int,float> *arr1 = find_float_array(e, fa1);
  Array<int,float> *arr2 = find_float_array(e, fa2);
  ContinuousBitmap<Color> *bm = find_continuous_bitmap(e, f);
  return add_color_bitmap2(e, new SpanArrays(*arr1, *arr2, bm));
}
GameApi::FA GameApi::FloatArrayApi::ramp(float start_value, float end_value, int steps)
{
  return add_float_array(e, new RampArray(start_value, end_value, steps));
}
GameApi::FA GameApi::FloatArrayApi::f_array(F *array, int size)
{
  std::vector<LazyValue<float>*> arr;
  for(int i=0;i<size;i++)
    {
      arr.push_back(find_float(e, array[i]));
    }
  return add_float_array(e, new RefArray<float>(arr));
}
GameApi::FA GameApi::FloatArrayApi::duparray(float value, int size)
{
  return add_float_array(e, new DuplicateArray<int,float>(value, size));
}
GameApi::FA GameApi::FloatArrayApi::duparray(FA fa, int count)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float_array(e, new DupArray<float>(*arr, count));
}
GameApi::FA GameApi::FloatArrayApi::subarray(FA fa, int start_index, int length)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float_array(e, new SubArray<int, float>(*arr, start_index, length));
}
class LazyIndex2 : public LazyValue<float>
{
public:
  LazyIndex2(Array<int,float> *arr, int index) : arr(arr), index(index) { }
  float get() const { return arr->Index(index); }
private:
  Array<int,float> *arr;
  int index;
};
GameApi::F GameApi::FloatArrayApi::array_index(FA fa, int index)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float(e, new LazyIndex2(arr, index));
}
EXPORT GameApi::PC GameApi::PointCollectionApi::function(std::function<GameApi::PT (int)> f, int count)
{
  return add_pointcoll_array(e, new PointCollectionFunction(e, f, count));
}
#if 0
void GameApi::LinesApi::render(float *array, int size)
{
  size/=6;
  GLuint vao = 0;
  GLuint buffer = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, size*2*sizeof(float)*3, array, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, size);

  glDeleteVertexArrays( 1, &vao );
  glDeleteBuffers( 1, &buffer);
}
#endif

#if 0
unsigned int *GameApi::LinesApi::color_access(LLA lines, int line, bool b)
{
  PointArray2 *ee = find_lines_array(e, lines);
  int pos = b ? 1 : 0;
  return &ee->color_array[line*2+pos];
}
int GameApi::LinesApi::line_count(LLA lines)
{
  PointArray2 *ee = find_lines_array(e, lines);
  return ee->numpoints;
}
float *GameApi::LinesApi::line_access(LLA lines, int line, bool b)
{
  PointArray2 *ee = find_lines_array(e, lines);
  int pos = b ? 1 : 0;
  return &ee->array[line*2*3+pos*3];
}
void GameApi::LinesApi::update(LLA lines)
{
  PointArray2 *arr = find_lines_array(e, lines);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, GL_STATIC_DRAW);
}
#endif
EXPORT GameApi::LLA GameApi::LinesApi::prepare(LI l)
{
  LineCollection *coll = find_line_array(e, l);
  int count = coll->NumLines();
  float *array = 0;
  unsigned int *color_array = 0;
  if (count > 0) {
    array = new float[count*6];
    color_array = new unsigned int[count*2];
    for(int i=0;i<count;i++)
      {
	Point p = coll->LinePoint(i,0);
	Point p2 = coll->LinePoint(i,1);
	unsigned int color1 = coll->LineColor(i,0);
	unsigned int color2 = coll->LineColor(i,1);
	//std::cout << std::hex << color1 << ";" << std::hex << color2 << std::endl;
	array[i*6+0] = p.x;
	array[i*6+1] = p.y;
	array[i*6+2] = p.z;
	array[i*6+3] = p2.x;
	array[i*6+4] = p2.y;
	array[i*6+5] = p2.z;
	color_array[i*2+0] = color1;
	color_array[i*2+1] = color2;
	//std::cout << i << ":" << "(" << p.x << "," << p.y << "," << p.z << ")-(" << p2.x<< "," << p2.y << "," << p2.z << ")" << std::endl;
      }
  }
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->color_array = color_array;
  arr->numpoints = count*2;
  glGenVertexArrays(1, arr->vao);
  glBindVertexArray(arr->vao[0]);
  glGenBuffers(1, &arr->buffer);
  glGenBuffers(1, &arr->buffer2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,0);
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(2);

  return add_lines_array(e, arr);
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

