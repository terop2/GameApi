#define GAME_API_DEFS

#include "GameApi.hh"
#include "Graph.hh"
#include "Physics.hh"
#include "State.hh"
#include "Serialize.hh"

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <tr1/memory>
#include <cmath>

#include "Parser.hh"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "FreeType.hh"
#include "GameRunner.hh"
#include "RayTracing.hh"
#include "VertexArray.hh"
#include "StateChange.hh"
#include "DistanceObject.hh"
#include "MatrixApi.hh"
#include "Web.hh"
#include <iostream>


GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm);

struct TexCoordQuad
{
  Point2d p1;
  Point2d p2;
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
}

struct SurfPriv
{
};

struct MainLoopPriv
{
  SDL_Surface *screen;
};

struct ShaderPriv2
{
  ShaderFile *file;
  ShaderSeq *seq;
  std::map<int,int> ids;
  int count;
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

GameApi::MainLoopApi::MainLoopApi(Env &e) : frame(0.0), time(0.0), e(e)  
{
  priv = (void*)new MainLoopPriv;
}
GameApi::MainLoopApi::~MainLoopApi()
{
  delete (MainLoopPriv*)priv;
}
void GameApi::MainLoopApi::cursor_visible(bool enabled)
{
  SDL_ShowCursor(enabled);
}
void GameApi::MainLoopApi::init(int screen_width, int screen_height)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
  p->screen = InitSDL(screenx,screeny,true);
  //glColor4f(1.0,1.0,1.0, 0.5);
  time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
  glMatrixMode( GL_PROJECTION ); 
  glLoadIdentity(); 
  glOrtho(0, screenx, screeny,0,0,1);
  /*
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 99.1, 60000.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  glMultMatrixf(&mat[0]);
  */
  glMatrixMode( GL_MODELVIEW ); 
  glLoadIdentity();
  alpha(false);
}
void GameApi::MainLoopApi::transfer_sdl_surface(MainLoopApi &orig)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  MainLoopPriv *p2 = (MainLoopPriv*)orig.priv;
  p->screen = p2->screen;
}


void GameApi::MainLoopApi::init_3d(int screen_width, int screen_height)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
  p->screen = InitSDL(screenx,screeny,true);
  //glColor4f(1.0,1.0,1.0, 0.5);
  time = SDL_GetTicks();
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
  glMatrixMode( GL_MODELVIEW ); 
  glLoadIdentity();
  alpha(false);
}

void GameApi::MainLoopApi::switch_to_3d(bool b)
{
  int screenx = 800;
  int screeny = 600;
  if (b)
    {
      glDisable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
      glMatrixMode( GL_PROJECTION ); 
      glLoadIdentity(); 
      Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
      glMultMatrixf(&mat[0]);
      
      glMatrixMode( GL_MODELVIEW ); 
      glTranslatef(0.0, 0.0, -500.0);

    }
  else
    {
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      glMatrixMode( GL_PROJECTION ); 
      glLoadIdentity(); 
      glOrtho(0, screenx, screeny,0,0,1);
      glMatrixMode( GL_MODELVIEW ); 
      glLoadIdentity();
      glTranslatef(0.375, 0.375, 0.0);
    }
}

void GameApi::MainLoopApi::clear()
{
  //glClearColor(255,255,255,255);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);
  //glTranslatef(0.0, 0.0, -260.0);
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}
void GameApi::MainLoopApi::clear_3d()
{
  //glClearColor(255,255,255,255);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);
  glTranslatef(0.0, 0.0, -260.0);
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}


void GameApi::MainLoopApi::alpha(bool enable)
{
  if (enable)
    {
      glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_COLOR /*ONE_MINUS_SRC_COLOR*/, GL_DST_COLOR);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE); 
    }
  else
    {
      glDisable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
}
void GameApi::MainLoopApi::antialias(bool enable)
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

float GameApi::MainLoopApi::get_time()
{
  return SDL_GetTicks()-time;
}
int GameApi::MainLoopApi::get_framenum()
{
  return frame;
}
GameApi::BM GameApi::MainLoopApi::screenshot()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  SDL_Surface *surf = p->screen;
  int w = surf->w;
  int h = surf->h;

  BufferRef ref = BufferRef::NewBuffer(w,h);
  glReadPixels(0,0,w,h, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);
  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  return add_color_bitmap2(e, bm);
}
void GameApi::MainLoopApi::swapbuffers()
{
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  glLoadIdentity();
  SDL_GL_SwapBuffers();
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
  int numpoints;
};


struct EnvImpl
{
  std::vector<Point> pt;
  std::vector<MatrixInterface*> matrix;
  GameApi::PT cursor_pos_point_id;
  std::vector<Vector> vectors;
  std::vector<Color> colors;
  std::vector<VolumeObject*> volumes;
  std::vector<FloatVolumeObject*> floatvolumes;
  std::vector<BitmapHandle*> bm;
  std::vector<BoolBitmap> bool_bm;
  std::vector<FloatBitmap> float_bm;
  std::vector<SpaceImpl> sp;
  std::vector<LineImpl> ln;
  std::vector<SpritePosImpl> spr_pos;
  std::vector<AnimImpl> anim;
  std::vector<FaceCollPolyHandle*> poly;
  std::vector<std::tr1::shared_ptr<void> > deletes;
  std::vector<FunctionImpl> func;
  std::vector<SurfaceImpl> surfaces;
  std::vector<VBOObjects*> vbos;
  std::vector<IDImpl> ids;
  std::vector<NDim<float,Point>*> dims;
  std::vector<StateInfo2> states;
  std::vector<StateRange> state_ranges; // ST type points to this array
  std::vector<ContinuousBitmap<Color>*> continuous_bitmaps;
  std::vector<VertexArraySet*> vertex_array;
  std::vector<int> texture_id;
  std::vector<std::string> shader_float_parameter;
  std::vector<std::string> shader_int_parameter;
  
  std::vector<Voxel<Color>*> voxels;
  std::vector<TROArray*> timeranges; // statechange time ranges
  std::vector<VArray*> timerange_vertexarrays; // statechange vertex arrays
  std::vector<ShaderPriv2*> shaders;
  std::vector<TextureI*> textures;
  std::vector<TexCoordQuad> tex_quads;
  std::vector<Separate*> separates;
  std::vector<PlanePoints2d*> plane_points;
  std::vector<Waveform*> waveforms;
  std::vector<Array<int, ObjectWithPos> * > object_move;
  std::vector<std::vector<VertexArrayWithPos> *> object_move_vertex_array;
  std::vector<Bitmap<float>*> layout_data;
  std::map<int, ArrayRender*> renders; // BM.id -> arrayrender
  std::vector<Layout*> layouts;
  std::vector<PointArray2*> pointarray;
  //std::vector<EventInfo> event_infos;
  Sequencer2 *event_infos; // owned, one level only.
  FT_Library lib;
  std::vector<Font> fonts;
  static EnvImpl *Environment(GameApi::Env *e) { return (EnvImpl*)e->envimpl; }
  EnvImpl() : event_infos(new EmptySequencer2) 
  {
    FT_Init_FreeType(&lib);
    cursor_pos_point_id.id = -1;
  }
  ~EnvImpl();
};
ArrayRender *FindRender(GameApi::Env &e, int bm_i)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->renders[bm_i];
}

template<class T>
void ArrayDelete(T *ptr)
{
  delete [] ptr;
}
GameApi::PT add_point(GameApi::Env &e, float x, float y);



GameApi::MainLoopApi::Event GameApi::MainLoopApi::get_event()
{
  SDL_Event event;
  Event e2;
  SDL_PollEvent(&event);
  int x,y;
  int val = SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  e2.ch = event.key.keysym.sym;
  int id = 0;
  EnvImpl *env = EnvImpl::Environment(&e);
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
  std::map<int, ArrayRender*>::iterator it = renders.begin();
  for(;it!=renders.end();it++)
    {
      ArrayRender *rend = (*it).second;
      delete rend;
    }

  int vv1 = volumes.size();
  for(int i_v=0;i_v<vv1;i_v++)
    {
      delete volumes[i_v];
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
  FT_Done_FreeType(lib);
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
}

GameApi::Env::Env()
{
  envimpl = (void*)new EnvImpl;
}

GameApi::Env::~Env()
{
  delete (EnvImpl*)envimpl;
}

SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm);

GameApi::SP add_space(GameApi::Env &e, SpaceImpl i)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->sp.push_back(i);
  GameApi::SP sp;
  sp.id = env->sp.size()-1;
  //sp.type = 0;
  return sp;
}
GameApi::S add_surface(GameApi::Env &e, SurfaceImpl i)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->surfaces.push_back(i);
  GameApi::S s;
  s.id = env->surfaces.size()-1;
  //sp.type = 0;
  return s;
}

GameApi::BM add_bitmap(GameApi::Env &e, BitmapHandle *handle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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

GameApi::BB add_bool_bitmap(GameApi::Env &e, Bitmap<bool> *bitmap)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->layouts.push_back(l);
  GameApi::LAY ll;
  ll.id = env->layouts.size()-1;
  return ll;
}

GameApi::WV add_waveform(GameApi::Env &e, Waveform *bitmap)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->waveforms.push_back(bitmap);
  GameApi::WV bm;
  bm.id = env->waveforms.size()-1;
  //bm.type = 0;
  return bm;
}


GameApi::TX add_texture(GameApi::Env &e, TextureI *i)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->textures.push_back(i);
  GameApi::TX tx;
  tx.id = env->textures.size()-1;
  return tx;
}
GameApi::Q add_tex_quad(GameApi::Env &e, Point2d p1, Point2d p2)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  FloatBitmap handle;
  handle.bitmap = bitmap;
  env->float_bm.push_back(handle);
  GameApi::FB bm;
  bm.id = env->float_bm.size()-1;
  //bm.type = 0;
  return bm;
}

GameApi::VA add_vertex_array(GameApi::Env &e, VertexArraySet *va)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->vertex_array.push_back(va);
  GameApi::VA bm;
  bm.id = env->vertex_array.size()-1;
  return bm;
}


GameApi::CBM add_continuous_bitmap(GameApi::Env &e, ContinuousBitmap<Color> *bitmap)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->continuous_bitmaps.push_back(bitmap);
  GameApi::CBM bm;
  bm.id = env->continuous_bitmaps.size()-1;
  return bm;
}

GameApi::F add_function(GameApi::Env &e, FunctionImpl &f)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->separates.push_back(sep);
  GameApi::SA p;
  p.id = env->separates.size()-1;
  return p;
}

GameApi::PL add_plane(GameApi::Env &e, PlanePoints2d *sep)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->plane_points.push_back(sep);
  GameApi::PL p;
  p.id = env->plane_points.size()-1;
  return p;
}

// takes ownership of PolyHandle*
GameApi::P add_polygon(GameApi::Env &e, FaceCollPolyHandle *handle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->poly.push_back(handle);
  GameApi::P p;
  p.id = env->poly.size()-1;
  //bm.type = 0;
  handle->id = p.id;
  return p;
}
GameApi::OM add_move(GameApi::Env &e, Array<int, ObjectWithPos> *arr)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->object_move.push_back(arr);
  GameApi::OM om;
  om.id = env->object_move.size()-1;
  return om;
}

GameApi::VAA add_move_array(GameApi::Env &e, std::vector<VertexArrayWithPos> *vec)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  GameApi::PolygonApi api(e);
  return api.memoize(add_polygon(e,h));
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->matrix.push_back(i);
  GameApi::M m;
  m.id = env->matrix.size()-1;
  return m;
}

GameApi::FOA add_point_array(GameApi::Env &e, PointArray2 *array)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->pointarray.push_back(array);
  GameApi::FOA pt;
  pt.id = env->pointarray.size()-1;
  return pt;
}

GameApi::PT add_point(GameApi::Env &e, float x, float y)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Point p = Point(x,y,0.0);
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
  return pt;
}



GameApi::TR add_timerange(GameApi::Env &e, int paths)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->timeranges.push_back(new TROArray(paths));
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}
GameApi::TR add_timerange(GameApi::Env &e, TROArray *arr)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->timeranges.push_back(arr);
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr);

GameApi::VV add_timerange_vertexarray(GameApi::Env &e, GameApi::TR tr)
{
  TROArray *arr = find_timerange(e, tr);
  
  EnvImpl *env = EnvImpl::Environment(&e);
  VArray *varr = new VArray(*arr);
  varr->copy2();
  env->timerange_vertexarrays.push_back(varr);
  GameApi::VV vv;
  vv.id = env->timerange_vertexarrays.size()-1;
  return vv;
}

GameApi::O add_volume(GameApi::Env &e, VolumeObject *o)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->volumes.push_back(o);
  GameApi::O pt;
  pt.id = env->volumes.size()-1;
  return pt;
}

GameApi::FO add_float_volume(GameApi::Env &e, FloatVolumeObject *o)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->floatvolumes.push_back(o);
  GameApi::FO pt;
  pt.id = env->floatvolumes.size()-1;
  return pt;
}


GameApi::VX add_voxel(GameApi::Env &e, Voxel<Color> *o)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->voxels.push_back(o);
  GameApi::VX pt;
  pt.id = env->voxels.size()-1;
  return pt;
}

GameApi::CO add_color(GameApi::Env &e, int r, int g, int b, int a)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Color p = Color(r,g,b,a);
  env->colors.push_back(p);
  GameApi::CO pt;
  pt.id = env->colors.size()-1;
  //pt.type = 0;
  return pt;
}


GameApi::E add_event(GameApi::Env &e, const EventInfo &info)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(env->event_infos));
  SingleEvent *event = new SingleEvent(env->event_infos, info);
  env->event_infos = event;

  GameApi::E ee;
  ee.id = event->CurrentEventNum();
  return ee;
}
GameApi::L add_link(GameApi::Env &e, GameApi::E e1, GameApi::E e2, LinkInfo info)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(env->event_infos));
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
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(env->event_infos));
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
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
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
GameApi::EventApi::~EventApi() { }

GameApi::ST GameApi::EventApi::states(int count_states)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->event_infos->Event(ee.id);
}
LinkageInfo find_linkage(GameApi::Env &e, GameApi::L l)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->event_infos->Linkage(l.id);
}
TextureI *find_texture(GameApi::Env &e, GameApi::TX t)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  if (t.id >=0 && t.id < (int)env->textures.size())
    return env->textures[t.id];
  return 0;
}
TexCoordQuad find_tex_quad(GameApi::Env &e, GameApi::Q t)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->event_infos->Links(l.id);
}
PosInfo find_pos_info(GameApi::Env &e, GameApi::LL l)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->event_infos->Positions(l.id);
}
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  return env->dims[mv.id];
}

GameApi::PT add_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Point p = Point(x,y,z);
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
  return pt;
}

GameApi::V add_vector(GameApi::Env &e, float dx, float dy, float dz)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Vector p = Vector(dx,dy,dz);
  env->vectors.push_back(p);
  GameApi::V pt;
  pt.id = env->vectors.size()-1;
  //pt.type = 0;
  return pt;
}



GameApi::IS add_anim(GameApi::Env &e, const AnimImpl &impl)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->anim.push_back(impl);
  GameApi::IS pt;
  pt.id = env->anim.size()-1;
  //pt.type = 0;
  return pt;
}

BitmapHandle *find_bitmap(GameApi::Env &e, GameApi::BM b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  BitmapHandle *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bm.size())
    handle = ee->bm[b.id];
  return handle;
}

BoolBitmap *find_bool_bitmap(GameApi::Env &e, GameApi::BB b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  BoolBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bool_bm.size())
    handle = &ee->bool_bm[b.id];
  return handle;
}

Layout *find_layout(GameApi::Env &e, GameApi::LAY l)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  Layout *handle = 0;
  if (l.id>=0 && l.id < (int)ee->layouts.size())
    handle = ee->layouts[l.id];
  return handle;
}

Waveform *find_waveform(GameApi::Env &e, GameApi::WV b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  Waveform *handle = 0;

  if (b.id >=0 && b.id < (int)ee->waveforms.size())
    handle = ee->waveforms[b.id];
  return handle;
}


ContinuousBitmap<Color> *find_continuous_bitmap(GameApi::Env &e, GameApi::CBM b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  ContinuousBitmap<Color> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->continuous_bitmaps.size())
    handle = ee->continuous_bitmaps[b.id];
  return handle;
}

FloatBitmap *find_float_bitmap(GameApi::Env &e, GameApi::FB b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  FloatBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->float_bm.size())
    handle = &ee->float_bm[b.id];
  return handle;
}

Voxel<Color> *find_voxel(GameApi::Env &e, GameApi::VX b)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  Voxel<Color> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->voxels.size())
    handle = ee->voxels[b.id];
  return handle;
}

Separate* find_separate(GameApi::Env &e, GameApi::SA p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  Separate *sep = 0;
  if (p.id >=0 && p.id<(int)ee->separates.size())
    sep = ee->separates[p.id];
  return sep;
}

PlanePoints2d* find_plane(GameApi::Env &e, GameApi::PL p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  PlanePoints2d *sep = 0;
  if (p.id >=0 && p.id<(int)ee->plane_points.size())
    sep = ee->plane_points[p.id];
  return sep;
}

Array<int,ObjectWithPos> *find_move(GameApi::Env &e, GameApi::OM p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  Array<int,ObjectWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move.size())
    handle = ee->object_move[p.id];
  return handle;
}

std::vector<VertexArrayWithPos> *find_move_array(GameApi::Env &e, GameApi::VAA p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  std::vector<VertexArrayWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move_vertex_array.size())
    handle = ee->object_move_vertex_array[p.id];
  return handle;
}

FaceCollPolyHandle *find_poly(GameApi::Env &e, GameApi::P p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  FaceCollPolyHandle *handle = 0;

  if (p.id >=0 && p.id < (int)ee->poly.size())
    handle = ee->poly[p.id];
  //std::cout << "find_poly:" << handle << std::endl;
  return handle;
}
VertexArraySet *find_vertex_array(GameApi::Env &e, GameApi::VA p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
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
  EnvImpl *ee = EnvImpl::Environment(&e);
  FunctionImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->func.size())
    handle = &ee->func[f.id];
  return handle;
}

SurfaceImpl *find_surface(GameApi::Env &e, GameApi::S f)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  SurfaceImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->surfaces.size())
    handle = &ee->surfaces[f.id];
  return handle;
}



AnimImpl find_anim(GameApi::Env &e, GameApi::IS i)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
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
      EnvImpl *env2 = EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender;
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
      EnvImpl *env2 = EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender;
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

PointArray2 *find_point_array(GameApi::Env &e, GameApi::FOA p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointarray.size())
    return ee->pointarray[p.id];
  return 0;
}

Point *find_point(GameApi::Env &e, GameApi::PT p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pt.size())
    return &ee->pt[p.id];
  return 0;
}
MatrixInterface *find_matrix(GameApi::Env &e, GameApi::M p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->matrix.size())
    return ee->matrix[p.id];
  return 0;
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (tr.id >=0 && tr.id < (int)ee->timeranges.size())
    return ee->timeranges[tr.id];
  return 0;
}
VArray *find_timerange_vertexarray(GameApi::Env &e, GameApi::VV vv)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (vv.id >=0 && vv.id < (int)ee->timerange_vertexarrays.size())
    return ee->timerange_vertexarrays[vv.id];
  return 0;
}

VolumeObject *find_volume(GameApi::Env &e, GameApi::O o)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->volumes.size())
    return ee->volumes[o.id];
  return 0;
}

FloatVolumeObject *find_float_volume(GameApi::Env &e, GameApi::FO o)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->floatvolumes.size())
    return ee->floatvolumes[o.id];
  return 0;
}

Color *find_color(GameApi::Env &e, GameApi::CO p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->colors.size())
    return &ee->colors[p.id];
  return 0;
}


Vector *find_vector(GameApi::Env &e, GameApi::V p)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->vectors.size())
    return &ee->vectors[p.id];
  return 0;
}


SpaceImpl *find_space(GameApi::Env &e, GameApi::SP s)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (s.id >=0 && s.id < (int)ee->sp.size())
    return &ee->sp[s.id];
  return 0;
}
LineImpl *find_line(GameApi::Env &e, GameApi::LN l)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
  if (l.id >=0 && l.id < (int)ee->ln.size())
    return &ee->ln[l.id];
  return 0;
}
SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm)
{
  EnvImpl *ee = EnvImpl::Environment(&e);
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


GameApi::GridApi::GridApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new GridPriv;
}
GameApi::GridApi::~GridApi()
{
  delete (GridPriv*)priv;
}

GameApi::SpriteApi::SpriteApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new SpritePriv;
}
GameApi::SpriteApi::~SpriteApi()
{
  delete (SpritePriv*)priv;
}
void GameApi::SpriteApi::spritepos(BM bm, float x, float y)
{
  SpritePosImpl *i = find_sprite_pos(e,bm);
  i->x = x;
  i->y = y;
}

void GameApi::SpriteApi::render_sprite_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  //SpritePriv &spriv = *(SpritePriv*)priv;
  EnvImpl *env = EnvImpl::Environment(&e);

  if (s->texture_id!=-1 && s->texture_id<6000)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      RenderVertexArray arr(*s);
      arr.render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if(s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D);
      glClientActiveTexture(GL_TEXTURE0+0);
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-6000);

      RenderVertexArray arr(*s);
      arr.render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else
    {
      std::cout << "SpriteApi::render_sprite_vertex_array, texture not found!" << std::endl;
    }
}
GameApi::VA GameApi::SpriteApi::create_vertex_array(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  SpritePriv &spriv = *(SpritePriv*)priv;
  EnvImpl *env = EnvImpl::Environment(&e);

  ::Sprite *sprite = sprite_from_handle(e,spriv, handle, -1);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; GameApi::VA va; va.id = 0; return va; }
  VertexArraySet *s = new VertexArraySet;
  PrepareSpriteToVA(*sprite, *s);
  TexturePrepare(*sprite, *env->renders[bm.id]);
  s->texture_id = bm.id;
  return add_vertex_array(e, s);
}

void GameApi::SpriteApi::preparesprite(BM bm, int bbm_choose)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  //Bitmap<Color> *cbm = find_color_bitmap(handle, bbm_choose);
  //if (!cbm) return;
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *sprite = sprite_from_handle(e,spriv, handle, bbm_choose);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; return; }
  EnvImpl *env = EnvImpl::Environment(&e);
  PrepareSprite(*sprite, *env->renders[bm.id]);
}
void GameApi::SpriteApi::rendersprite(BM bm, float x, float y, float mult_x, float mult_y)
{
  rendersprite(bm, 0, x, y, mult_x, mult_y);
}
void GameApi::SpriteApi::rendersprite(BM bm, PT pos)
{
  rendersprite(bm, 0, pos);
}
#if 0
void GameApi::SpriteApi::rendersprite(BM bm, float x, float y, float x1, float y1, float inside_x, float inside_y, float inside_x1, float inside_y1)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  EnvImpl *env = EnvImpl::Environment(&e);
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x, y };
  Point2d pos3 = { x1, y1 };
  Point2d inside_2 = { inside_x, inside_y };
  Point2d inside_3 ={ inside_x1, inside_y1 }; 
  float z = 0.0;
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  RenderSprite(*s, bm_choose, pos2, pos3, inside_2, inside_3, z, *env->renders[bm.id]);
}
#endif
void GameApi::SpriteApi::rendersprite(BM bm, int bm_choose, float x, float y, float mult_x, float mult_y)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call preparesprite() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x, y };
  float z = 0.0;
  //glPushMatrix();
  //glScalef(mult_x, mult_y, 1.0);
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  EnvImpl *env = EnvImpl::Environment(&e);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id], mult_x, mult_y);

  //glPopMatrix();
}
void GameApi::SpriteApi::rendersprite(BM bm, int bm_choose, PT pos)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point *p = find_point(e, pos);
  Point2d pos2 = { p->x, p->y };
  float z = 0.0; //p->z;
  EnvImpl *env = EnvImpl::Environment(&e);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id]);
}

void GameApi::SpriteApi::rendersprite(BM bm, int bm_choose, SP move_space, SP sprite_space, float x, float y)
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
  EnvImpl *env = EnvImpl::Environment(&e);
  RenderSprite(*s, bm_choose, pos, z, *env->renders[bm.id]);
}
void GameApi::SpriteApi::rendersprite(BM bm, int bm_choose, SP move_space, SP sprite_space, PT pos)
{
}
GameApi::SP GameApi::SpriteApi::spritespace(BM bm)
{
  SP s;
  return s;
}
GameApi::PT GameApi::SpriteApi::pixelpos(BM bm, int x, int y)
{
  PT pt;
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

GameApi::BitmapApi::BitmapApi(Env &e) : e(e) 
{
  priv = (void*)new BitmapPriv;
}
GameApi::BitmapApi::~BitmapApi()
{
  delete (BitmapPriv*)priv;
}
GameApi::AnimApi::AnimApi(Env &e) : e(e) 
{
  priv = (void*)new AnimPriv;
}
GameApi::AnimApi::~AnimApi()
{
  delete (AnimPriv*)priv;
}
GameApi::ShaderApi::ShaderApi(Env &e) : e(e)
{
  priv = (void*)new ShaderPriv2;
}
GameApi::ShaderApi::~ShaderApi()
{
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

GameApi::BM GameApi::BitmapApi::newbitmap(int sx, int sy)
{
  ::Bitmap<Color> *b = new ConstantBitmap<Color>(Color(0,0,0,0), sx,sy);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b;
  BM bm = add_bitmap(e, handle);
  return bm;
  
}

template<class T>
class BitmapTransformFromFunction : public Bitmap<T>
{
public:
  BitmapTransformFromFunction(GameApi::EveryApi &ev, Bitmap<T> &bm, T (*fptr)(GameApi::EveryApi &, int,int,T,void*), void *data) : ev(ev), bm(bm),  fptr(fptr), data(data) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual T Map(int x, int y) const
  {
    return fptr(ev,x,y,bm.Map(x,y), data);
  }
private:
  GameApi::EveryApi &ev;
  Bitmap<T> &bm;
  T (*fptr)(GameApi::EveryApi &ev, int,int,T,void*); 
  void *data;
};

template<class T>
class BitmapFromFunction : public Bitmap<T>
{
public:
  BitmapFromFunction(GameApi::EveryApi &ev, T (*fptr)(GameApi::EveryApi &ev, int,int, void*), int sx, int sy, void *data) : ev(ev), fptr(fptr), sx(sx),sy(sy),data(data) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual T Map(int x, int y) const
  {
    return fptr(ev, x,y,data);
  }
private:
  GameApi::EveryApi &ev;
  T (*fptr)(GameApi::EveryApi &, int,int, void*);
  int sx,sy;
  void *data;
};


GameApi::BM GameApi::BitmapApi::transform(BM orig, unsigned int (*fptr)(GameApi::EveryApi &ev, int,int, unsigned int, void *), void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  BitmapHandle *handle = find_bitmap(e, orig);
  Bitmap<Color> *c = find_color_bitmap(handle);
  UnsignedIntFromBitmap *bm1 = new UnsignedIntFromBitmap(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(bm1));
  BitmapTransformFromFunction<unsigned int> *trans = new BitmapTransformFromFunction<unsigned int>(*ev, *bm1, fptr, data);
  env->deletes.push_back(std::tr1::shared_ptr<void>(trans));

  BitmapFromUnsignedInt *bm2 = new BitmapFromUnsignedInt(*trans);
  return add_color_bitmap(e, bm2);
}
GameApi::BM GameApi::BitmapApi::function(unsigned int (*fptr)(GameApi::EveryApi &ev,int,int, void*), int sx, int sy, void* data)
{
  GameApi::EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  Bitmap<unsigned int> *bm = new BitmapFromFunction<unsigned int>(*ev, fptr,sx,sy,data);
  
  
  return add_color_bitmap(e, new BitmapFromUnsignedInt(*bm));
}

GameApi::BM GameApi::BitmapApi::memoize(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  return add_color_bitmap(e, new MemoizeBitmap(*bitmap));
}

GameApi::BM GameApi::BitmapApi::memoize_all(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bitmap = find_color_bitmap(handle);
  MemoizeBitmap *membitmap = new MemoizeBitmap(*bitmap);
  membitmap->MemoizeAll();
  return add_color_bitmap(e, membitmap);
}

GameApi::BM GameApi::WaveformApi::waveform_bitmap(WV wave, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  Waveform *m_wave = find_waveform(e, wave);
  return add_color_bitmap(e, new WaveformBitmap(*m_wave, 0.0, m_wave->Length(), m_wave->Min(), m_wave->Max(), sx, sy, Color(true_color), Color(false_color)));
}

GameApi::BM GameApi::BitmapApi::mandelbrot(bool julia,
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
  EnvImpl *env = EnvImpl::Environment(&e);

  env->deletes.push_back(std::tr1::shared_ptr<void>(b));
  env->deletes.push_back(std::tr1::shared_ptr<void>(b2));
  ::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = m;
  BM bm = add_bitmap(e, handle);
  return bm;  
}

GameApi::BM GameApi::BitmapApi::mandelbrot2(bool julia,
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
  EnvImpl *env = EnvImpl::Environment(&e);

  env->deletes.push_back(std::tr1::shared_ptr<void>(b));
  //env->deletes.push_back(std::tr1::shared_ptr<void>(b2));
  //::Bitmap<Color> *m = new MemoizeBitmap(*b2);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = b2;
  BM bm = add_bitmap(e, handle);
  return bm;  
}

GameApi::BM GameApi::BitmapApi::newtilebitmap(int sx, int sy, int tile_sx, int tile_sy)
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

void GameApi::BitmapApi::savebitmap(BM bm, std::string filename)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);
  PpmFile file(filename, *bm2);
  std::string pngcontents = file.Contents();
  std::ofstream filehandle(filename, std::ios_base::out);
  filehandle << pngcontents;
  filehandle.close();
}

GameApi::BM GameApi::BitmapApi::loadbitmap(std::string filename)
{

  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  if (b==false) { GameApi::BM bm; bm.id = -1; return bm; }
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));

  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = buf;
  BM bm = add_bitmap(e, handle);
  return bm;
}
GameApi::BM GameApi::BitmapApi::loadtilebitmap(std::string filename, int sx, int sy)
{

  //ChessBoardBitmap *bmp = new ChessBoardBitmap(Color(255,0.0,0.0), Color(255,255,255), 8, 8, 30, 30);
  bool b = false;
  BufferRef img = LoadImage(filename, b);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(img.buffer, &ArrayDelete<unsigned int>));
  BitmapFromBuffer *buf = new BitmapFromBuffer(img);  
  BitmapTileHandle *handle = new BitmapTileHandle;
  handle->bm = buf;
  handle->tile_sx = sx;
  handle->tile_sy = sy;
  BM bm = add_bitmap(e, handle);
  return bm;
}


GameApi::BM GameApi::BitmapApi::loadposbitmap(std::string filename)
{
  BitmapCircle *circle = new BitmapCircle(Point2d::NewPoint(30.0,30.0), 30.0, 60,60);
  ColorMap2 *f = new ColorMap2;
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(f));
  env->deletes.push_back(std::tr1::shared_ptr<void>(circle));
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

GameApi::GamesApi::GamesApi(Env &e) : e(e)
{
}
GameApi::GamesApi::~GamesApi()
{
}
void GameApi::GamesApi::register_game(int game_id, void (*fptr)(GameApi::EveryApi &e))
{
  gamefunctions[game_id] = fptr;
}
void GameApi::GamesApi::modify_map(int event, int game_id)
{
  gamemapping[event] = game_id;
}
void GameApi::GamesApi::start_game(int event)
{
  int id = gamemapping[event];
  void (*fptr)(GameApi::EveryApi &) = gamefunctions[id];
  if (fptr)
    {
      GameApi::EveryApi ea(e);
      fptr(ea);
    }
}


void GameApi::GridApi::preparegrid(GameApi::BM tile_bitmap, int tile_choose)
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

void GameApi::GridApi::rendergrid(GameApi::BM grid, int grid_choose, float top_x, float top_y)
{
  GridPriv *p = (GridPriv*)priv;
  int cellsx = p->cellsx[p->last_id];
  int cellsy = p->cellsy[p->last_id];
  
  BitmapHandle *handle = find_bitmap(e, grid);
  ::Bitmap<Pos> *bitmap = find_pos_bitmap(handle);
  if (!bitmap) { std::cout << "rendergrid bitmap==NULL" << std::endl; return; }
  RenderGrid(*bitmap, top_x, top_y, cellsx, cellsy, p->rend, 0,0, 10,10);
}

GameApi::BM GameApi::BitmapApi::findtile(GameApi::BM tile_bitmap, int x, int y)
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

GameApi::BM GameApi::BitmapApi::growbitmap(GameApi::BM orig, int l, int t, int r, int b)
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

GameApi::BM GameApi::BitmapApi::modify_bitmap(GameApi::BM orig, BM bm, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapHandle *handle2 = find_bitmap(e, bm);
  BitmapHandle *h = modify_bitmap_h(handle, handle2, x,y);
  BM bbm = add_bitmap(e,h);
  return bbm;
}
GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  return add_color_bitmap(e, new BlitBitmapClass(*bm1, *bm2, x,y));
}

GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, FB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  FloatBitmap *handle3 = find_float_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<float> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked(*bm1, *bm2, x,y, *bm3));
}

GameApi::BM GameApi::BitmapApi::blitbitmap(BM bg, BM orig, int x, int y, BB mask)
{
  BitmapHandle *handle = find_bitmap(e, bg);
  BitmapHandle *handle2 = find_bitmap(e, orig);
  BoolBitmap *handle3 = find_bool_bitmap(e, mask);
  Bitmap<Color> *bm1 = find_color_bitmap(handle);
  Bitmap<Color> *bm2 = find_color_bitmap(handle2);
  Bitmap<bool> *bm3 = handle3->bitmap;
  return add_color_bitmap(e, new BlitBitmapClassMasked2(*bm1, *bm2, x,y, *bm3));
}

int GameApi::BitmapApi::intvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapIntHandle *handle2 = dynamic_cast<BitmapIntHandle*>(handle);
  if (handle2)
    {
      return handle2->bm->Map(x,y);
    }
  return 0;
}

int GameApi::BitmapApi::size_x(BM bm)
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
int GameApi::BitmapApi::size_y(BM bm)
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

unsigned int GameApi::BitmapApi::colorvalue(GameApi::BM orig, int x, int y)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *handle2 = dynamic_cast<BitmapColorHandle*>(handle);
  if (handle2)
    {
      return handle2->bm->Map(x,y).Pixel();
    }
  return 0;
}


GameApi::BM GameApi::BitmapApi::interpolate_bitmap(GameApi::BM orig1, GameApi::BM orig2, float x)
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
  BitmapFromString(GameApi::EveryApi &ev, char *array, int sx, int sy, T (*fptr)(GameApi::EveryApi &ev, char)) : ev(ev),array(array), sx(sx), sy(sy),  fptr(fptr) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  T Map(int x, int y) const
  {
    return fptr(ev, array[x+y*sx]);
  }
private:
  GameApi::EveryApi &ev;
  char *array;
  int sx;
  int sy;
  T (*fptr)(GameApi::EveryApi &ev, char);
};


GameApi::BM GameApi::BitmapApi::newintbitmap(char *array, int sx, int sy, int (*fptr)(EveryApi &ev, char ch))
{
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  BitmapIntHandle *handle = new BitmapIntHandle;
  handle->bm = new BitmapFromString<int>(*ev, array, sx, sy, fptr);
  return add_bitmap(e, handle);
}

GameApi::BM GameApi::BitmapApi::newcolorbitmap(char *array, int sx, int sy, unsigned int (*fptr)(GameApi::EveryApi &ev,char))
{
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  BitmapColorHandle *handle = new BitmapColorHandle;
  ::Bitmap<unsigned int> *bm = new BitmapFromString<unsigned int>(*ev, array, sx, sy, fptr);
  env->deletes.push_back(std::tr1::shared_ptr<void>(bm));
  handle->bm = new BitmapFromUnsignedInt(*bm);
  return add_bitmap(e, handle);
}



GameApi::BM GameApi::BitmapApi::anim_array(GameApi::BM *bitmaparray, int size)
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
GameApi::BM GameApi::BitmapApi::bitmapandtypes(BM bm, BM (*fptr)(GameApi::EveryApi &ev,int))
{
  BitmapHandle *h = find_bitmap(e, bm);
  BitmapIntHandle *hh = dynamic_cast<BitmapIntHandle*>(h);
  if (!hh) { std::cout << "bm for bitmapandtypes() is not bitmap of integers" << std::endl; return; }
  
}
#endif
GameApi::IS GameApi::AnimApi::single(int val, float duration)
{
  AnimImpl i;
  i.wave_int = new SingleAnimInt(val, duration);
  i.wave_point = 0;
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

GameApi::IS GameApi::AnimApi::single(PT point, float duration)
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
GameApi::IS GameApi::AnimApi::single(float val, float duration)
{
  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = 0;
  i.wave_float = new SingleAnimFloat(val, duration);
  IS is = add_anim(e,i);
  return is;
}

GameApi::IS GameApi::AnimApi::line(int val1, int val2, float duration)
{
  AnimImpl i;
  i.wave_int = new LineAnimInt(val1,val2, duration);
  i.wave_point = 0;
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

GameApi::IS GameApi::AnimApi::line(PT p1, PT p2, float duration)
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
GameApi::IS GameApi::AnimApi::line(float val1, float val2, float duration)
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




GameApi::IS GameApi::AnimApi::seq_line(IS *array, int size)
{
  AnimImpl i;
  i.wave_int = new AnimArrayInt(e, array, size);
  i.wave_point = new AnimArrayPoint(e, array, size);
  i.wave_float = new AnimArrayFloat(e, array, size);
  IS is = add_anim(e,i);
  return is;
} 
GameApi::IS GameApi::AnimApi::repeat(IS i, int count)
{
  AnimImpl ii;
  ii.wave_int = new AnimRepeatInt(e, i, count);
  ii.wave_point = new AnimRepeatPoint(e, i, count);
  ii.wave_float = new AnimRepeatFloat(e, i, count);
  IS is = add_anim(e,ii);
  return is;
}
GameApi::IS GameApi::AnimApi::repeat_infinite(IS i)
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

int GameApi::AnimApi::timed_value_repeat_num(IS i, float time)
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

int GameApi::AnimApi::timed_value(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimInt *wave = ii.wave_int;
  if (!wave) return 0;
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  return wave->Index(0, time);
}

GameApi::PT GameApi::AnimApi::timed_value_point(IS i, float time)
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
float GameApi::AnimApi::timed_value_float(IS i, float time)
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


GameApi::TextApi::~TextApi()
{
  delete(std::vector<BM>*)priv;
}

void GameApi::TextApi::load_font(std::string filename, int sx, int sy, int x, int y, char start_char, char end_char)
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
void GameApi::TextApi::draw_text(std::string text, int x, int y)
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
	  sp.rendersprite(b, xpos, ypos, 1.0, 1.0);
	}
      xpos += sx;
      if (c=='\n') { xpos=x; ypos+=sy; }
    }
}

GameApi::PolygonApi::PolygonApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new PolyPriv;
}

GameApi::PolygonApi::~PolygonApi()
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
  EnvImpl *env = EnvImpl::Environment(&e);

  Point *pp1 = find_point(e,center);
  BitmapHandle *handle = find_bitmap(e, height);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  ContinuousBitmapFromBitmap<Color> *contbm = new ContinuousBitmapFromBitmap<Color>(*bitmap, 1.0,1.0);
  ContinuousColorBitmapToFloatBitmap *floatbm = new ContinuousColorBitmapToFloatBitmap(*contbm);
  env->deletes.push_back(std::tr1::shared_ptr<void>(floatbm));
  env->deletes.push_back(std::tr1::shared_ptr<void>(contbm));
  BitmapSphere *sphere = new BitmapSphere(*floatbm, *pp1, radius0, radius1);
  SurfaceImpl i;
  i.surf = sphere;
  return add_surface(e, i);
}

GameApi::P GameApi::PolygonApi::triangle(PT p1, PT p2, PT p3)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  FaceCollection *coll = new TriElem(*pp1, *pp2, *pp3);
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::empty()
{
  return add_polygon(e,new EmptyBoxableFaceCollection, 1);
}

GameApi::P GameApi::PolygonApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p2);
  FaceCollection *coll = new TriElem(*pp1, *pp2, (*pp3)+Vector(1.0,1.0,1.0));
  return add_polygon(e, coll,1);
}

class TexCoordQuadFaceCollection : public ForwardFaceCollection
{
public:
  TexCoordQuadFaceCollection(FaceCollection &coll, TexCoordQuad q_, int sx, int sy) : ForwardFaceCollection(coll), q(q_),sx(sx),sy(sy) 
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
  int sx,sy;
};

GameApi::P GameApi::PolygonApi::sprite_bind(P p, TX tx, int id)
{
  TextureApi t(e);
  Q q = t.get_tex_coord(tx, id);
  return sprite_bind(p, q, tx);
}
GameApi::P GameApi::PolygonApi::sprite_bind(P p, Q q, TX tx)
{
  TextureI *texture = find_texture(e, tx);
  int sx = texture->SizeX();
  int sy = texture->SizeY();
  FaceCollection *pp1 = find_facecoll(e, p);
  TexCoordQuad qq = find_tex_quad(e, q);
  return add_polygon(e, new TexCoordQuadFaceCollection(*pp1, qq, sx,sy),1);
}

GameApi::P GameApi::PolygonApi::quad(PT p1, PT p2, PT p3, PT p4)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  Point *pp4 = find_point(e, p4);
  FaceCollection *coll = new QuadElem(*pp1, *pp2, *pp3, *pp4);
  return add_polygon(e, coll,1);
}
GameApi::P GameApi::PolygonApi::quad_x(float x,
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
GameApi::P GameApi::PolygonApi::quad_y(float x1, float x2,
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
GameApi::P GameApi::PolygonApi::quad_z(float x1, float x2,
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



GameApi::P GameApi::PolygonApi::cube(float start_x, float end_x,
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
  return add_polygon(e, coll,1);  
}

GameApi::P GameApi::PolygonApi::cube(PT *p)
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
GameApi::P GameApi::PolygonApi::sphere(PT center, float radius, int numfaces1, int numfaces2)
{
    Point *p = find_point(e,center);
    FaceCollection *coll = new SphereElem(*p, radius, numfaces1, numfaces2);
    return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::cone(int numfaces, PT p1, PT p2, float rad1, float rad2)
{
    Point *pp1 = find_point(e,p1);
    Point *pp2 = find_point(e,p2);
    FaceCollection *coll = new ConeElem(numfaces, *pp1, *pp2, rad1, rad2);
    return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::ring(float sx, float sy, float x, int steps)
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(pvarray));
  env->deletes.push_back(std::tr1::shared_ptr<void>(pointvector));
  RingElem *ring = new RingElem(*pointvector, x, steps);
  return add_polygon(e, ring,1);
}

GameApi::P GameApi::PolygonApi::or_elem(P p1, P p2)
{
  FaceCollection *pp1 = find_facecoll(e, p1);
  FaceCollection *pp2 = find_facecoll(e, p2);
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>;
  coll->push_back(pp1);
  coll->push_back(pp2);
  coll->update_faces_cache();
  return add_polygon(e, coll,1);
}


GameApi::P GameApi::PolygonApi::texture(P orig, BM bm, int choose)
{
  FaceCollection *coll = find_facecoll(e, orig);
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle, choose);
  BufferFromBitmap *req = new BufferFromBitmap(*bitmap);
  SimpleTexCoords *coords = new SimpleTexCoords(*coll, 0);
  BoxableFaceCollection *coll2 = new TextureElem2(*coll, *req, *coords);
  return add_polygon(e, coll2, 1);
}

GameApi::P GameApi::PolygonApi::or_array(P *p1, int size)
{
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<size;i++)
    {
      FaceCollection *pp1 = find_facecoll(e, p1[i]);
      if (!pp1) { std::cout << "or_array, bad FaceCollection" << std::endl;  continue; }
      vec->push_back(pp1);
    }
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(vec));
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>(vec->begin(), vec->end());
  coll->update_faces_cache();
  return add_polygon(e, coll,1);
}




GameApi::P GameApi::PolygonApi::color(P next, unsigned int color)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorElem(*convert, color);
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::color_faces(P next, 
					 unsigned int color_1, 
					 unsigned int color_2,
					 unsigned int color_3, 
					 unsigned int color_4)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorFaceElem(*convert, color_1,color_2,color_3,color_4);
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::translate(P orig, float dx, float dy, float dz)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Translate(dx,dy,dz));
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::rotatex(P orig, float angle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::XRotation(angle));
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::rotatey(P orig, float angle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::YRotation(angle));
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::rotatez(P orig, float angle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::ZRotation(angle));
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::rotate(P orig, PT point, V axis, float angle)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Point *pp = find_point(e, point);
  Vector *ax = find_vector(e, axis);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::RotateAroundAxisPoint(*pp, *ax, angle));
  return add_polygon(e, coll,1);
}


GameApi::P GameApi::PolygonApi::scale(P orig, float sx, float sy, float sz)
{
  EnvImpl *env = EnvImpl::Environment(&e);  
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::tr1::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Scale(sx,sy,sz));
  return add_polygon(e, coll,1);
}

GameApi::P GameApi::PolygonApi::move(P orig, PT obj_0, V obj_x, V obj_y, V obj_z,
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(coll2));  
  FaceCollection *coll3 = new CoordChangeFaceColl(coll2, true, world);
  return add_polygon(e, coll3, 1);
}

GameApi::P GameApi::PolygonApi::splitquads(P orig, int x_count, int y_count)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *next = new SplitQuads(*coll, x_count, y_count);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(next));  
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(cb));
}
#endif

class ChangePoints2 : public ForwardFaceCollection
{
public:
  ChangePoints2(GameApi::EveryApi &ev, FaceCollection &coll, Point (*fptr)(GameApi::EveryApi &,Point, int,int,void*), void *data)
    : ForwardFaceCollection(coll), ev(ev), fptr(fptr),data(data) { }
  virtual Point FacePoint(int face, int point) const 
  {
    return fptr(ev, ForwardFaceCollection::FacePoint(face,point),
		face, point, data);
  }
private:
  GameApi::EveryApi &ev;
  Point (*fptr)(GameApi::EveryApi &ev, Point, int,int,void*);
  void *data;
};
struct ChangePositions_data
{
  GameApi::Env *env;
  GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, int face, int point, void* data);
  void *data;
};

Point ChangePositions_Func(GameApi::EveryApi &ev, Point p, int face,int point,void* data)
{
  ChangePositions_data *dt = (ChangePositions_data*)data;
  GameApi::PT pt = add_point(*dt->env, p.x, p.y, p.z);
  GameApi::PT pt2 = dt->fptr(ev, pt, face, point, data);
  Point *pp = find_point(*dt->env, pt2);
  if (!pp) return Point(0.0,0.0,0.0);
  return *pp;
}

GameApi::P GameApi::PolygonApi::change_positions(P orig, PT (*fptr)(GameApi::EveryApi &ev, PT p, int face, int point, void* data), void *data)
{
  FaceCollection *coll = find_facecoll(e, orig);
  ChangePositions_data dt;
  dt.env = &e;
  dt.fptr = fptr;
  dt.data = data;
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  FaceCollection *coll2 = new ChangePoints2(*ev, *coll, &ChangePositions_Func, &dt);
  return add_polygon(e, coll2, 1);
}



class ChangeNormal2 : public ForwardFaceCollection
{
public:
  ChangeNormal2(GameApi::EveryApi &ev, FaceCollection &coll, Vector (*fptr)(GameApi::EveryApi &ev,Vector, int,int,void*), void *data)
    : ForwardFaceCollection(coll), ev(ev), fptr(fptr),data(data) { }
  virtual Vector PointNormal(int face, int point) const 
  {
    return fptr(ev, ForwardFaceCollection::PointNormal(face,point),
		face, point, data);
  }
private:
  GameApi::EveryApi &ev;
  Vector (*fptr)(GameApi::EveryApi &ev,Vector, int,int,void*);
  void *data;
};
struct ChangeNormal_data
{
  GameApi::Env *env;
  GameApi::V (*fptr)(GameApi::EveryApi &ev, GameApi::V p, int face, int point, void* data);
  void *data;
};

Vector ChangeNormals_Func(GameApi::EveryApi &ev, Vector p, int face,int point,void* data)
{
  ChangeNormal_data *dt = (ChangeNormal_data*)data;
  GameApi::V pt = add_vector(*dt->env, p.dx, p.dy, p.dz);
  GameApi::V pt2 = dt->fptr(ev, pt, face, point, dt->data);
  Vector *pp = find_vector(*dt->env, pt2);
  if (!pp) return Vector(0.0,0.0,0.0);
  return *pp;
}

GameApi::P GameApi::PolygonApi::change_normals(P orig, V (*fptr)(GameApi::EveryApi &ev, V p, int face, int point, void* data), void *data)
{
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeNormal_data dt;
  dt.env = &e;
  dt.fptr = fptr;
  dt.data = data;

  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));

  FaceCollection *coll2 = new ChangeNormal2(*ev, *coll, &ChangeNormals_Func, &dt);
  return add_polygon(e, coll2, 1);
}


class ChangeColor2 : public ForwardFaceCollection
{
public:
  ChangeColor2(GameApi::EveryApi &ev, FaceCollection &coll, unsigned int (*fptr)(GameApi::EveryApi &ev, unsigned int, int,int,void*), void *data)
    : ForwardFaceCollection(coll), ev(ev), fptr(fptr),data(data) { }
  virtual unsigned int Color(int face, int point) const 
  {
    return fptr(ev, ForwardFaceCollection::Color(face,point),
		face, point, data);
  }
private:
  GameApi::EveryApi &ev;
  unsigned int (*fptr)(GameApi::EveryApi &ev, unsigned int, int,int,void*);
  void *data;
};
struct ChangeColor_data
{
  GameApi::Env *env;
  unsigned int (*fptr)(GameApi::EveryApi &ev, unsigned int p, int face, int point, void* data);
  void *data;
};

unsigned int ChangeColor_Func(GameApi::EveryApi &ev, unsigned int p, int face,int point,void* data)
{
  ChangeColor_data *dt = (ChangeColor_data*)data;
  return dt->fptr(ev, p,face,point,dt->data);
}

GameApi::P GameApi::PolygonApi::change_colors(P orig, unsigned int (*fptr)(GameApi::EveryApi &ev, unsigned int p, int face, int point, void* data), void *data)
{
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeColor_data dt;
  dt.env = &e;
  dt.fptr = fptr;
  dt.data = data;

  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));

  FaceCollection *coll2 = new ChangeColor2(*ev, *coll, &ChangeColor_Func, &dt);
  return add_polygon(e, coll2, 1);
}

class ChangeTexture : public ForwardFaceCollection
{
public:
  ChangeTexture(GameApi::EveryApi &ev, FaceCollection &coll, int (*fptr)(GameApi::EveryApi &ev, int,void*), ::Bitmap< ::Color> **array, int size, void *data)
    : ForwardFaceCollection(coll), ev(ev), fptr(fptr),data(data), array(array), size(size) { temp = new BufferFromBitmap*[size]; }
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
  virtual int FaceTexture(int face) const { return fptr(ev, face, data); }
private:
  GameApi::EveryApi &ev;
  int (*fptr)(GameApi::EveryApi &ev,int,void*);
  void *data;
  Bitmap< ::Color> **array;
  int size;
  mutable BufferFromBitmap **temp;
};


GameApi::P GameApi::PolygonApi::change_texture(P orig, int (*fptr)(GameApi::EveryApi &ev,int face, void* data), BM *array, int size, void *data)
{
  FaceCollection *coll = find_facecoll(e, orig);
  std::vector<Bitmap<Color>*> *vec = new std::vector<Bitmap<Color>*>;
  for(int i=0;i<size;i++)
    {
      BitmapHandle *handle = find_bitmap(e, array[i]);
      ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
      vec->push_back(bitmap);
    }
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  ChangeTexture *tex = new ChangeTexture(*ev, *coll, fptr, &(*vec)[0], size, data);
  return add_polygon(e, tex, 1);
}




GameApi::P GameApi::PolygonApi::recalculate_normals(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new RecalculateNormals(*coll);
  return add_polygon(e, coll2, 1);
}
GameApi::P GameApi::PolygonApi::memoize(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  return add_polygon2(e, coll2, 1);
}
GameApi::P GameApi::PolygonApi::memoize_all(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  coll2->MemoizeAll();
  return add_polygon2(e, coll2, 1);
}
GameApi::P GameApi::PolygonApi::heightmap(BM bm,
					  HeightMapType t,
				       float min_x, float max_x,
				       float min_y, float max_y,
				       float min_z, float max_z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  HeightMap3DataImpl *data = new HeightMap3DataImpl(*bitmap);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(data));
  Point p(min_x, max_y, min_z);
  Vector u_x(max_x-min_x, 0.0, 0.0);
  Vector u_y(0.0, -(max_y-min_y),0.0);
  Vector u_z(0.0,0.0,max_z-min_z);

  HeightMap3 *heightmap = new HeightMap3(*data, 0.0, 1.0,
					 p, u_x, u_y, u_z);
  env->deletes.push_back(std::tr1::shared_ptr<void>(heightmap));
  MeshFaceCollection *coll = new MeshFaceCollection(*heightmap, 0);
  return add_polygon(e, coll, 1);
}

GameApi::P GameApi::PolygonApi::anim_array(P *array, int size)
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(vec));
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec->begin(), vec->end());
  handle->collarrayowned = true;
  //handle->size = size;
  return add_polygon(e, handle);
}

void GameApi::PolygonApi::render(P p, int choose, float x, float y, float z)
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

void GameApi::PolygonApi::prepare(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  StateBitmaps *state_bm = PrepareFaceCollPolyHandle(h2, bbm_choose);
  pp->states[p.id] = state_bm;
}

void GameApi::PolygonApi::preparepoly(P p, int bbm_choose)
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
  glTranslatef(x,y,z);
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
  CountFuncFaceCollection(GameApi::EveryApi &ev, FaceCollection *next, int (*fptr)(GameApi::EveryApi &e, int face, void *data), void *data) : ForwardFaceCollection(*next), ev(ev), fptr(fptr), data(data) { }
  virtual int NumPoints(int face) const
  {
    return fptr(ev, face, data);
  }
  
private:
  GameApi::EveryApi &ev;
  int (*fptr)(GameApi::EveryApi &e, int face, void *data); 
  void *data;
};

class PointFaceCollection : public ForwardFaceCollection
{
public:
  PointFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, GameApi::PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data) : ForwardFaceCollection(*next), e(e), ev(ev), fptr(fptr), data(data) { }
  virtual Point FacePoint(int face, int point) const
  {
    GameApi::PT pp = fptr(ev, face, point, data);
    Point *p = find_point(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data);
  void *data;
};


class NormalFaceCollection : public ForwardFaceCollection
{
public:
  NormalFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, GameApi::V (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data) : ForwardFaceCollection(*next), e(e), ev(ev), fptr(fptr), data(data) { }
  virtual Vector PointNormal(int face, int point) const
  {
    GameApi::V pp = fptr(ev, face, point, data);
    Vector *p = find_vector(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::V (*fptr)(GameApi::EveryApi &e, int face, int point, void *data);
  void *data;
};


class ColorFaceCollection : public ForwardFaceCollection
{
public:
  ColorFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, unsigned int (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data) : ForwardFaceCollection(*next), e(e), ev(ev), fptr(fptr), data(data) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int pp = fptr(ev, face, point, data);
    return pp;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  unsigned int (*fptr)(GameApi::EveryApi &e, int face, int point, void *data);
  void *data;
};


class TexFaceCollection : public ForwardFaceCollection
{
public:
  TexFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, GameApi::PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data) : ForwardFaceCollection(*next), e(e), ev(ev), fptr(fptr), data(data) { }
  virtual Point2d TexCoord(int face, int point) const
  {
    GameApi::PT pp = fptr(ev, face, point, data);
    Point *p = find_point(e, pp);
    Point2d px = { p->x, p->y };
    return px;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data);
  void *data;
};

class AttribFaceCollection : public ForwardFaceCollection
{
public:
  AttribFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, float (*fptr)(GameApi::EveryApi &e, int face, int point, int id, void *data), void *data, int idx) : ForwardFaceCollection(*next), e(e),ev(ev), fptr(fptr), data(data), idx(idx) { }
  virtual float Attrib(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = fptr(ev, face, point, id, data);
      return pp;
    }
    return ForwardFaceCollection::Attrib(face,point,id);
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float (*fptr)(GameApi::EveryApi &e, int face, int point, int id, void *data);
  void *data;
  int idx;
};

class AttribIFaceCollection : public ForwardFaceCollection
{
public:
  AttribIFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *next, int (*fptr)(GameApi::EveryApi &e, int face, int point, int id, void *data), void *data, int idx) : ForwardFaceCollection(*next), e(e), ev(ev), fptr(fptr), data(data), idx(idx) { }
  virtual int AttribI(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = fptr(ev, face, point, id, data);
      return pp;
    }
    return ForwardFaceCollection::AttribI(face,point,id);
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  int (*fptr)(GameApi::EveryApi &e, int face, int point, int id, void *data);
  void *data;
  int idx;
};



GameApi::P GameApi::PolygonApi::counts(P p1, int numfaces)
{
  FaceCollection *poly = find_facecoll(e, p1);
  return add_polygon(e, new CountsFaceCollection(numfaces, poly),1);  
}
GameApi::P GameApi::PolygonApi::count_function(P p1, int (*numpoints)(GameApi::EveryApi &e, int face, void *data), void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new CountFuncFaceCollection(*ev, poly, numpoints, data),1);  
}
GameApi::P GameApi::PolygonApi::point_function(P p1, PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new PointFaceCollection(e, *ev, poly, fptr, data),1);  
}
GameApi::P GameApi::PolygonApi::normal_function(P p1, V (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new NormalFaceCollection(e, *ev, poly, fptr, data),1);  
}
GameApi::P GameApi::PolygonApi::color_function(P p1, unsigned int (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new ColorFaceCollection(e, *ev, poly,fptr, data),1);  
}
GameApi::P GameApi::PolygonApi::texcoord_function(P p1, PT (*fptr)(GameApi::EveryApi &e, int face, int point, void *data), void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new TexFaceCollection(e, *ev, poly, fptr, data),1);  
}
GameApi::P GameApi::PolygonApi::attrib_function(P p1, float (*fptr)(GameApi::EveryApi &e, int face, int point, int idx, void *data), int idx, void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new AttribFaceCollection(e, *ev, poly, fptr, data, idx),1);  
}
GameApi::P GameApi::PolygonApi::attribi_function(P p1, int (*fptr)(GameApi::EveryApi &e, int face, int point, int idx, void *data), int idx, void *data)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_polygon(e, new AttribIFaceCollection(e, *ev, poly, fptr, data, idx),1);  
}


void GameApi::ShaderApi::load(std::string filename)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  p->file = new ShaderFile(filename);
  ShaderSeq *seq = new ShaderSeq(*p->file);
  p->seq = seq;
}
GameApi::SH GameApi::ShaderApi::get_shader(std::string v_format,
					std::string f_format,
					std::string g_format)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  if (!p->file)
    {
      std::cout << "ERROR: Call Shader::load before get_shader()" << std::endl;
      SH sh;
      sh.id = -1;
      return sh;
    }

  p->ids[p->count] = p->seq->GetShader(v_format, f_format, g_format);
  p->count++;
  GameApi::SH sh;
  sh.id = p->count-1;
  return sh;
}
void GameApi::ShaderApi::use(GameApi::SH shader)
{
  if (shader.id==-1) return;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->use(p->ids[shader.id]);
}

void GameApi::ShaderApi::unuse(GameApi::SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  seq->unuse(p->ids[shader.id]);
}
void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float val)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, float x, float y, float z)
{
  //std::cout << "Set var float" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  Point px(x, y, z);
  prog->set_var(name, px);
}

void GameApi::ShaderApi::set_var(GameApi::SH shader, std::string name, int val)
{
  //std::cout << "Set var int" << std::endl;
  ShaderPriv2 *p = (ShaderPriv2*)priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  prog->set_var(name, val);
}

void GameApi::ShaderApi::bindnames(GameApi::SH shader, 
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


GameApi::FunctionApi::FunctionApi(Env &e) : e(e)
{
  priv = (void*)new FunctionPriv;
}
GameApi::FunctionApi::~FunctionApi()
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(bm));
  TextureBitmap *bm2 = new TextureBitmap(*bm, *s->surf);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm2;
  return add_bitmap(e, handle);
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
GameApi::BM GameApi::BitmapApi::repeat_bitmap(BM orig, int xcount, int ycount)
{
  BitmapHandle *handle = find_bitmap(e, orig);
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (!chandle) return add_bitmap(e,0);
  RepeatBitmap<Color> *rep = new RepeatBitmap<Color>(*chandle->bm, xcount, ycount);
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = rep;
  return add_bitmap(e,chandle2);
}

GameApi::BM GameApi::FloatBitmapApi::subfloatbitmap(FB fb, float range_start, float range_end, unsigned int true_color, unsigned int false_color)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  GameApi::BB b = to_bool(fb, range_start, range_end);
  Color c(true_color);
  Color c2(false_color);
  GameApi::BM bm = ev->bool_bitmap_api.to_bitmap(b, c.r, c.g, c.b, c.alpha,
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
  EnvImpl *env = EnvImpl::Environment(&e);
  VBOObjects *objs = new VBOObjects;
  objs->SetObjectCount(obj_count);
  env->vbos.push_back(objs);
  Vb v;
  v.id = env->vbos.size()-1;
  return v;
}

void GameApi::VBOApi::sprite(Vb v, int obj_num, BM bm, float x, float y)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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

  EnvImpl *env = EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->OrigToAdjusted(true, true, true);
  objs->AllocWholeBuffer(true, true, true);
  objs->CopyPartsToWholeBuffer(0, -1, -1, 0);
  objs->BlitWholeBufferToGPU();
}

void GameApi::VBOApi::swapframe(Vb v, int obj_num, int type, int frame)
{
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
  EnvImpl *env = EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->CopyPartToGPU(obj_num, state->vertex, state->normal, state->color, state->texcoord);
}

void GameApi::VBOApi::move(Vb v, int obj_num, float x, float y, float z)
{
}

void GameApi::VBOApi::render(Vb v)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  VBOObjects *objs = env->vbos[v.id];
  objs->BlitGPUToScreen();
}

struct EventPriv
{
};

GameApi::EventApi::EventApi(Env &e) : e(e) 
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
  EnvImpl *env = EnvImpl::Environment(&e);
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = env->poly[poly.id];
  link.polygon_end = env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
}
GameApi::L GameApi::EventApi::bitmap(GameApi::E start, GameApi::E end, GameApi::BM start_bitmap, BM end_bitmap)
{
  //EnvImpl *env = EnvImpl::Environment(&e);
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = NULL; //env->poly[poly.id];
  link.polygon_end = NULL; //env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
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
  EnvImpl *env = EnvImpl::Environment(&e);
  StateRange r = env->state_ranges[st.id];
  
  AllStatesSequencer seq(env->event_infos, &env->states[r.start_range], r.range_size);
  GameRunner game(seq, start_state);
  game.run();

#if 0
  EventInfo ei = find_event_info(e, ee);
  
  LinkageInfo li = find_linkage(e, l);
  LinkInfo link = find_link_info(e, l);
  PosInfo pos = find_pos_info(e, l);

#endif
}

GameApi::P GameApi::PolygonApi::create_static_geometry(GameApi::P *array, int size)
{
  if (size==0) { std::cout << "Empty array in remove_changing" << std::endl; }
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(find_facecoll(e,array[i]));
    }
  IsChangingFace *func = new IsChangingFace(vec);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(func));
  FilterFaces *coll2 = new FilterFaces(*(vec[0]), *func);
  return add_polygon(e, coll2, 1);
}
GameApi::P GameApi::PolygonApi::create_dynamic_geometry(GameApi::P *array, int size)
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
GameApi::P GameApi::PolygonApi::tri_vertex_array(float *v_array, int v_size,
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

int GameApi::PolygonApi::get_tri_vertex_array_frames(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_frames" << std::endl; }

  FaceCollPolyHandle *h2 = find_poly(e,p);
  if (!h2->collarray) { return 1; }
  return h2->collarray->Size();
}

int GameApi::PolygonApi::get_tri_vertex_array_rows(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_rows" << std::endl; }
  return state_bm->bitmap->SizeY();
}

void GameApi::PolygonApi::get_tri_vertex_array(P p, int choose, int row,
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

GameApi::P GameApi::PolygonApi::polygon(PT *array, int size)
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

void GameApi::PolygonApi::render_dynamic(GameApi::P p, int array_elem, bool textures)
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

GameApi::FontApi::FontApi(Env &e) : e(e) 
{
  priv = new FontPriv;
}
GameApi::FontApi::~FontApi()
{
}

GameApi::Ft GameApi::FontApi::newfont(const char *filename, int sx, int sy)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  Font fnt;
  fnt.bm = new FontGlyphBitmap((void*)&env->lib,filename, sx,sy);
  env->fonts.push_back(fnt);
  GameApi::Ft font;
  font.id = env->fonts.size()-1;
  return font;
}
GameApi::BM GameApi::FontApi::glyph(GameApi::Ft font, long idx)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  env->fonts[font.id].bm->load_glyph(idx);
  Bitmap<int> *bm = env->fonts[font.id].bm;
  Bitmap<Color> *cbm = new MapBitmapToColor(0,255,Color(255,255,255,255), Color(0,0,0,0), *bm);
  MemoizeBitmap *mbm = new MemoizeBitmap(*cbm);
  mbm->MemoizeAll();
  env->deletes.push_back(std::tr1::shared_ptr<void>(cbm)); 
  BitmapColorHandle *chandle2 = new BitmapColorHandle;
  chandle2->bm = mbm;
 return add_bitmap(e,chandle2);
}

GameApi::BM GameApi::FontApi::font_string(Ft font, const char *str, int x_gap)
{
  EnvImpl *env = EnvImpl::Environment(&e);
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

GameApi::PT GameApi::PointApi::origo()
{
  return add_point(e, 0.0,0.0,0.0);
}
GameApi::PT GameApi::PointApi::point(float x, float y, float z)
{
  return add_point(e, x,y,z);
}
GameApi::PT GameApi::PointApi::move(PT p1, float dx, float dy, float dz)
{
  Point *p = find_point(e,p1);
  return add_point(e, p->x+dx,p->y+dy,p->z+dz);
}
GameApi::PT GameApi::PointApi::mix(PT p1, PT p2, float val)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Point res = *pp1+val*Vector((*pp2)-(*pp1));
  return add_point(e,res.x,res.y,res.z);
}
float GameApi::PointApi::pt_x(PT p)
{
  return find_point(e,p)->x;
}
float GameApi::PointApi::pt_y(PT p)
{
  return find_point(e,p)->y;
}
float GameApi::PointApi::pt_z(PT p)
{
  return find_point(e,p)->z;
}
float GameApi::PointApi::dist3d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  float d = (*pp1-*pp2).Dist();
  return d;
}
float GameApi::PointApi::dist2d(PT p, PT p2)
{
  Point *pp1 = find_point(e,p);
  Point *pp2 = find_point(e,p2);
  Point2d pa = { pp1->x,pp1->y };
  Point2d pb = { pp2->x,pp2->y };
  Vector2d v = pb-pa;
  float d = v.Dist();
  return d;
}
GameApi::V GameApi::PointApi::minus(PT p1, PT p2)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  Vector v = *pp1 - *pp2;
  return add_vector(e,v.dx,v.dy,v.dz);
}
GameApi::ColorApi::ColorApi(Env &e) : e(e) { }
GameApi::VectorApi::VectorApi(Env &e) : e(e) { }
GameApi::PointApi::PointApi(Env &e) : e(e) { }

GameApi::CO GameApi::ColorApi::u_color(unsigned int color)
{
  return add_color(e, 
		   (color&0xff0000) >> 16,
		   (color &0xff00) >> 8,
		   color &0xff,
		   (color&0xff000000)>>24);
}
GameApi::CO GameApi::ColorApi::rgb_color(int r, int g, int b, int a)
{
  return add_color(e, r,g,b,a);
}
GameApi::CO GameApi::ColorApi::rgbf_color(float r, float g, float b, float a)
{
  return add_color(e,int(r*255.0),int(g*255.0),int(b*255.0),int(a*255.0));
}
GameApi::V GameApi::VectorApi::null_vector()
{
  return add_vector(e,0.0,0.0,0.0);
}
GameApi::V GameApi::VectorApi::vector(float dx, float dy, float dz)
{
  return add_vector(e,dx,dy,dz);
}
GameApi::V GameApi::VectorApi::sum(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector res = *vv1+*vv2;
  return add_vector(e,res.dx,res.dy,res.dz);
}
GameApi::V GameApi::VectorApi::mul(V v1, float scalar)
{
  Vector *vv1 = find_vector(e,v1);
  Vector res = scalar * (*vv1);
  return add_vector(e,res.dx,res.dy,res.dz);
}
float GameApi::VectorApi::dot(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  return Vector::DotProduct(*vv1,*vv2);
}
GameApi::V GameApi::VectorApi::cross(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector v = Vector::CrossProduct(*vv1,*vv2);
  return add_vector(e, v.dx, v.dy, v.dz);
}

float GameApi::VectorApi::projection_length(V v1, V u_x)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vu_x = find_vector(e,u_x);
  return Vector::FindProjectionLength(*vv1, *vu_x);
}

GameApi::V GameApi::VectorApi::projection_1(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.first;
  return add_vector(e, v.dx, v.dy, v.dz);
}

GameApi::V GameApi::VectorApi::projection_2(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.second;
  return add_vector(e, v.dx, v.dy, v.dz);
}

GameApi::V GameApi::VectorApi::neg(V v) 
{
  Vector *vv1 = find_vector(e,v);
  Vector vx = -(*vv1);
  return add_vector(e,vx.dx, vx.dy, vx.dz);
}


GameApi::PT GameApi::PointApi::from_angle(float radius, float angle)
{
  Point p = Point(0.0,0.0,0.0)+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}
GameApi::PT GameApi::PointApi::from_angle(PT center, float radius, float angle)
{
  Point *cen = find_point(e,center);
  Point p = *cen+Vector(radius*cos(angle),radius*sin(angle),0.0);
  return add_point(e,p.x,p.y,p.z);
}

GameApi::O GameApi::BoolBitmapApi::to_volume(BB b, float dist)
{
  BoolBitmap *c = find_bool_bitmap(e,b);
  Bitmap<bool> *bm = c->bitmap;
  return add_volume(e, new BitmapVolume(bm, dist));
}
GameApi::BB GameApi::BoolBitmapApi::transform(BB orig, bool (*fptr)(EveryApi &ev, int,int, bool, void *), void *data)
{
  BoolBitmap *c = find_bool_bitmap(e,orig);
  Bitmap<bool> *bm = c->bitmap;
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));

  BitmapTransformFromFunction<bool> *trans = new BitmapTransformFromFunction<bool>(*ev, *bm, fptr, data);

  return add_bool_bitmap(e, trans);
}

GameApi::BB GameApi::BoolBitmapApi::empty(int sx, int sy)
{
  return add_bool_bitmap(e, new ConstantBitmap<bool>(false, sx,sy));
}
GameApi::FB GameApi::FloatBitmapApi::empty(int sx, int sy)
{
  return add_float_bitmap(e, new ConstantBitmap<float>(0.0, sx,sy));
}
GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color(BM bm, int r, int g, int b)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  return add_bool_bitmap(e,new EquivalenceClassColorBitmap(*color_bm, Color(r,g,b)));
}

template<class T>
class EquivalenceClassFromArea : public Bitmap<bool>
{
public:
  EquivalenceClassFromArea(GameApi::EveryApi &ev, Bitmap<Color> &bm, T fptr, void *ptr) : ev(ev), bm(bm),  fptr(fptr), ptr(ptr) { }
  virtual int SizeX() const { return bm.SizeX(); }
  virtual int SizeY() const { return bm.SizeY(); }
  virtual bool Map(int x, int y) const
  {
    Color c = bm.Map(x,y);
    return fptr(ev, c.r,c.g,c.b,c.alpha, ptr);
  }
  
private:
  GameApi::EveryApi &ev;
  Bitmap<Color> &bm;
  T fptr;
  void *ptr;
};

GameApi::BB GameApi::BoolBitmapApi::from_float_bitmap(FB float_bm, float range_start, float range_end)
{
  FloatBitmap *fb = find_float_bitmap(e, float_bm);
  Bitmap<float> *bm = fb->bitmap;
  return add_bool_bitmap(e, new BoolBitmapFromFloatBitmap2(*bm, range_start, range_end));
}

GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, bool (*fptr)(GameApi::EveryApi &ev, int, int, int,int, void*), void *ptr)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *color_bm = find_color_bitmap(handle);
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_bool_bitmap(e,new EquivalenceClassFromArea<bool(*)(GameApi::EveryApi &ev, int,int,int,int, void*)>(*ev, *color_bm, fptr, ptr));
}

struct RangeData
{
  int r_start, r_end;
  int g_start, g_end;
  int b_start, b_end;
  int a_start, a_end;
};

bool range_select_color_area(GameApi::EveryApi &ev, int r, int g, int b,int a, void* dt)
{
  RangeData *data = (RangeData*)dt;
  if (r<data->r_start || r>data->r_end) { return false; }
  if (g<data->g_start || g>data->g_end) { return false; }
  if (b<data->b_start || b>data->b_end) { return false; }
  if (a<data->a_start || a>data->a_end) { return false; }
  return true;
}
GameApi::BB GameApi::BoolBitmapApi::from_bitmaps_color_area(BM bm, int r_start, int r_end, int g_start, int g_end, int b_start, int b_end, int a_start, int a_end)
{
  RangeData data = { r_start, r_end, g_start, g_end, b_start, b_end, a_start, a_end };
  RangeData *dt2 = new RangeData;
  *dt2 = data;
  return from_bitmaps_color_area(bm, range_select_color_area, (void*)dt2);
}

GameApi::BB GameApi::BoolBitmapApi::circle(BB bg, float center_x, float center_y, float radius)
{
  Bitmap<bool> *bm = find_bool_bitmap(e, bg)->bitmap;
  Point2d center = { center_x, center_y };
  Bitmap<bool> *circle = new BitmapCircle(center, radius, bm->SizeX(), bm->SizeY());
  Bitmap<bool> *orbitmap = new OrBitmap(*bm, *circle);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(circle));
  return add_bool_bitmap(e, orbitmap);
}

struct Rectangle_data
{
  float start_x, end_x;
  float start_y, end_y;
};

bool Rectangle_func(GameApi::EveryApi &ev, int x, int y, void* data)
{
  Rectangle_data *dt = (Rectangle_data*)data;
  if (x<dt->start_x) return false;
  if (x>=dt->end_x) return false;
  if (y<dt->start_y) return false;
  if (y>=dt->end_y) return false;
  return true;
}

int GameApi::BoolBitmapApi::size_x(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
int GameApi::BoolBitmapApi::size_y(BB bm)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
bool GameApi::BoolBitmapApi::boolvalue(BB bm, int x, int y)
{
  Bitmap<bool> *b = find_bool_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}

GameApi::BB GameApi::BoolBitmapApi::part_circle(int sx, int sy, float x, float y, float start_angle, float end_angle, float start_rad, float end_rad)
{
  return add_bool_bitmap(e, new PartCircleBoolBitmap(sx,sy, x,y,start_angle,end_angle, start_rad, end_rad));
}

int GameApi::FloatBitmapApi::size_x(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeX();
}
int GameApi::FloatBitmapApi::size_y(FB bm)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->SizeY();
}
float GameApi::FloatBitmapApi::floatvalue(FB bm, int x, int y)
{
  Bitmap<float> *b = find_float_bitmap(e,bm)->bitmap;
  return b->Map(x,y);
}


GameApi::BB GameApi::BoolBitmapApi::rectangle(BB bg, float x, float y, float width, float height)
{
  Rectangle_data d;
  d.start_x = x;
  d.start_y = y;
  d.end_x = x+width;
  d.end_y = y+height;
  return or_bitmap(bg, function(&Rectangle_func, size_x(bg), size_y(bg), &d));
  
}

GameApi::BB GameApi::BoolBitmapApi::not_bitmap(BB b)
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
GameApi::BB GameApi::BoolBitmapApi::andnot_bitmap(BB b1, BB b2)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, b1)->bitmap;
  Bitmap<bool> *bm2 = find_bool_bitmap(e, b2)->bitmap;
  return add_bool_bitmap(e, new AndNotBitmap(*bm1,*bm2));
}
GameApi::BM GameApi::BoolBitmapApi::to_bitmap(BB bools,
					      int true_r, int true_g, int true_b, int true_a,
					      int false_r, int false_g, int false_b, int false_a)
{
  Bitmap<bool> *bm1 = find_bool_bitmap(e, bools)->bitmap;
  return add_color_bitmap2(e, new ChooseTBitmap<Color>(*bm1, Color(false_r, false_g, false_b, false_a), Color(true_r, true_g, true_b, true_a)));
}

GameApi::BoolBitmapApi::BoolBitmapApi(GameApi::Env &e) : e(e) { }
GameApi::BoolBitmapApi::~BoolBitmapApi() { }

GameApi::FloatBitmapApi::FloatBitmapApi(GameApi::Env &e) : e(e) { }
GameApi::FloatBitmapApi::~FloatBitmapApi() { }

GameApi::FB GameApi::FloatBitmapApi::function(float (*fptr)(EveryApi &ev,int,int,void*), int sx, int sy, void* data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_float_bitmap(e, new BitmapFromFunction<float>(*ev, fptr,sx,sy,data));
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

GameApi::FB GameApi::FloatBitmapApi::from_red(BM bm)
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

GameApi::FB GameApi::FloatBitmapApi::from_green(BM bm)
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

GameApi::FB GameApi::FloatBitmapApi::from_blue(BM bm)
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

GameApi::FB GameApi::FloatBitmapApi::from_alpha(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  Bitmap<float> *bm2 = new BitmapFromAlpha(*bmc);
  return add_float_bitmap(e, bm2);
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

GameApi::BM GameApi::FloatBitmapApi::to_color(FB r, FB g, FB b, FB a)
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

GameApi::FB GameApi::FloatBitmapApi::from_bool_bitmap(BB bm, int csx, int csy)
{
  Bitmap<bool> *bm2 = find_bool_bitmap(e,bm)->bitmap;
  return add_float_bitmap(e, new FloatBitmapFromBoolBitmap(*bm2, csx, csy));
}
GameApi::BM GameApi::FloatBitmapApi::to_grayscale(FB fb)
{
  Bitmap<float> *bm = find_float_bitmap(e,fb)->bitmap;
  return add_color_bitmap2(e, new GrayScaleBitmapFromFloatBitmap(*bm, Color(0,0,0,0), Color(255,255,255,255)));
}

GameApi::BM GameApi::FloatBitmapApi::to_grayscale_color(FB fb, int r, int g, int b, int a,
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

GameApi::BB GameApi::BoolBitmapApi::function(bool (*fptr)(EveryApi &ev,int,int, void*), int sx, int sy, void* data)
{
  EnvImpl *env = EnvImpl::Environment(&e);
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  
  return add_bool_bitmap(e, new BitmapFromFunction<bool>(*ev, fptr, sx,sy,data));
}
GameApi::BB GameApi::BoolBitmapApi::polygon(BB bg2, PT *points, int size)
{
  EnvImpl *env = EnvImpl::Environment(&e);

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
  env->deletes.push_back(std::tr1::shared_ptr<void>(pointarray));
  env->deletes.push_back(std::tr1::shared_ptr<void>(array));
  env->deletes.push_back(std::tr1::shared_ptr<void>(conv));
  env->deletes.push_back(std::tr1::shared_ptr<void>(lines));
  env->deletes.push_back(std::tr1::shared_ptr<void>(bm));
  env->deletes.push_back(std::tr1::shared_ptr<void>(sbm));

  return add_bool_bitmap(e, sbm2);
}

GameApi::VolumeApi::VolumeApi(Env &e) : e(e) { }
GameApi::VolumeApi::~VolumeApi() { }

GameApi::O GameApi::VolumeApi::sphere(PT center, float radius)
{
  Point *p = find_point(e, center);
  return add_volume(e, new SphereVolume(*p, radius));
}
GameApi::O GameApi::VolumeApi::cone(PT p1, PT p2, float rad1, float rad2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  return add_volume(e, new ConeVolume(*pp1, *pp2-*pp1, rad1, rad2));
}
GameApi::O GameApi::VolumeApi::cube(float start_x, float end_x,
				    float start_y, float end_y,
				    float start_z, float end_z)
{
  return add_volume(e, new CubeVolume(start_x, end_x,
				      start_y, end_y,
				      start_z, end_z));
}

GameApi::O GameApi::VolumeApi::torus(PT center, PT u_x, PT u_y, float dist1, float dist2)
{
  Point *centerp = find_point(e, center);
  Point *u_xp = find_point(e, u_x);
  Point *u_yp = find_point(e, u_y);
  return add_volume(e, new TorusVolume(*u_xp-*centerp, *u_yp-*centerp, dist1, dist2, *centerp));
}

GameApi::O GameApi::VolumeApi::min_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new AndVolume(*oo1,*oo2));

}

GameApi::O GameApi::VolumeApi::max_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new OrVolume(*oo1,*oo2));

}

GameApi::BB GameApi::VolumeApi::plane(GameApi::O o, int sx, int sy,
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
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(x));
  env->deletes.push_back(std::tr1::shared_ptr<void>(y));
  env->deletes.push_back(std::tr1::shared_ptr<void>(z));
  env->deletes.push_back(std::tr1::shared_ptr<void>(voxel));
  env->deletes.push_back(std::tr1::shared_ptr<void>(plane));
  return add_bool_bitmap(e, bm);
}
GameApi::O GameApi::VolumeApi::andnot_op(GameApi::O o1, GameApi::O o2)
{
  VolumeObject *oo1 = find_volume(e, o1);
  VolumeObject *oo2 = find_volume(e, o2);
  return add_volume(e, new AndNotVolume(*oo1,*oo2));

}



class AppendHandleValue : public HandleValue<std::pair<Vector, unsigned int> >
{
public:
  AppendHandleValue(GameApi::EveryApi &api, std::vector<GameApi::P> &vec, float sx, float sy, float sz,
		    GameApi::P (*fptr)(GameApi::EveryApi &, float, float, float, float, float, float, unsigned int, void*), void *data)
    : api(api), vec(vec), fptr(fptr), data(data), sx(sx), sy(sy), sz(sz) { }
  void Handle(std::pair<Vector, unsigned int> p)
  {
    vec.push_back(fptr(api, p.first.dx,p.first.dx+sx,
		       p.first.dy,p.first.dy+sy,
		       p.first.dz,p.first.dz+sz, p.second, data));
  }
  GameApi::P get_all() const { return api.polygon_api.memoize( api.polygon_api.or_array(&vec[0], vec.size()) ); }
private:
  GameApi::EveryApi &api;
  std::vector<GameApi::P> &vec;
  GameApi::P (*fptr)(GameApi::EveryApi &e, float,float,float, float,float,float, unsigned int, void*);
  void *data;
  float sx,sy,sz;
};

class FunctionVolume : public VolumeObject
{
public:
  FunctionVolume(GameApi::EveryApi &ev, bool (*fptr)(GameApi::EveryApi &ev, float x, float y, float z, void *data), void *data)
    : ev(ev), fptr(fptr), data(data)
  {
  }
  virtual bool Inside(Point v) const { return fptr(ev, v.x,v.y,v.z,data); }
private:
  GameApi::EveryApi &ev;
  bool (*fptr)(GameApi::EveryApi &ev, float x, float y, float z, void *data); 
  void *data;
};


GameApi::O GameApi::VolumeApi::boolfunction(bool (*fptr)(GameApi::EveryApi &ev, float x, float y, float z, void *data), void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  VolumeObject *o = new FunctionVolume(*ev, fptr, data);
  return add_volume(e,o);
}

class FunctionFloatVolumeObject : public FloatVolumeObject {
public:
  FunctionFloatVolumeObject(GameApi::EveryApi &ev, float (*fptr)(GameApi::EveryApi &ev, float x, float y, float z, void *data), void *data) : ev(ev), fptr(fptr), data(data) { }
  virtual float FloatValue(Point p) const
  {
    return fptr(ev, p.x,p.y,p.z,data);
  }
  
private:
  GameApi::EveryApi &ev;
  float (*fptr)(GameApi::EveryApi &ev, float x, float y, float z, void *data);
  void *data;
};

class IntersectionPoint : public VolumeObject
{
public:
  IntersectionPoint(GameApi::EveryApi &ev, float (*fptr1)(GameApi::EveryApi &ev, float x, float y, float z, void *data), void *data1, float start_range1, float end_range1,
		    float (*fptr2)(GameApi::EveryApi &ev,float x, float y, float z, void *data), void *data2,float start_range2, float end_range2,
		    float (*fptr3)(GameApi::EveryApi &ev,float x, float y, float z, void *data), void *data3,float start_range3, float end_range3)
    : ev(ev), f1(ev, fptr1, data1), f2(ev, fptr2,data2), f3(ev, fptr3,data3),
      oo1(f1,start_range1, end_range1),
      oo2(f2,start_range2, end_range2),
      oo3(f3, start_range3, end_range3),
      intersect1(oo1,oo2), intersect_12(intersect1, oo3)
  {
  }
  virtual bool Inside(Point v) const { return intersect_12.Inside(v); }

private:
  GameApi::EveryApi &ev;
  FunctionFloatVolumeObject f1,f2,f3;
  SubVolume oo1,oo2,oo3;
  AndVolume intersect1, intersect_12;
};


GameApi::O GameApi::VolumeApi::subvolume(float (*fptr)(GameApi::EveryApi &ev,float x, float y, float z, void *data), void *data, float start_range, float end_range)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  FunctionFloatVolumeObject *ff = new FunctionFloatVolumeObject(*ev, fptr, data);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ff));
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_volume(e, new SubVolume(*ff, start_range, end_range));
}


void GameApi::VolumeApi::find_surface(O object, PT p1, PT p2, PT *res1, PT *res2, int level)
{
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  VolumeObject *volume = find_volume(e,object);
  Range<Point> rr(*pp1,*pp2);

  Range<Point> r = FindSurface(rr, *volume, level);

  *res1 = add_point(e, r.start.x,r.start.y,r.start.z);
  *res2 = add_point(e, r.end.x,r.end.y,r.end.z);
}

GameApi::P GameApi::VolumeApi::rendercubes(O o, P (*fptr)(EveryApi &api, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, unsigned int color, void* data), void *data, int size, float wholesize)
{
  float s = wholesize/size;
  EveryApi api(e);
  std::vector<P> vec;
  AppendHandleValue hv(api, vec, s, s, s,
		       fptr, data);

  VolumeObject *volume = find_volume(e,o);
  RenderVoxel(*volume, size, wholesize, hv);
  return hv.get_all();
}

GameApi::ContinuousBitmapApi::ContinuousBitmapApi(Env &e) : e(e) { }

GameApi::CBM GameApi::ContinuousBitmapApi::empty(float x, float y)
{
  return constant(0x00000000, x, y);
}
GameApi::CBM GameApi::ContinuousBitmapApi::constant(unsigned int color, float x, float y)
{
  return add_continuous_bitmap(e, new ConstantContinuousBitmap<Color>(x,y,Color(color)));
}

class FunctionContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  FunctionContinuousBitmap(GameApi::EveryApi &ev, unsigned int (*fptr)(GameApi::EveryApi &ev, float, float, void*), float sx, float sy, void *data) : ev(ev), fptr(fptr), sx(sx), sy(sy), data(data) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    return Color(fptr(ev, x,y, data));
  }
public:
  GameApi::EveryApi &ev;
  unsigned int (*fptr)(GameApi::EveryApi &ev, float, float, void*); 
  float sx; float sy;
  void *data;
};


GameApi::CBM GameApi::ContinuousBitmapApi::function(unsigned int (*fptr)(EveryApi &ev, float,float, void*), float sx, float sy, void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));

  return add_continuous_bitmap(e, new FunctionContinuousBitmap(*ev, fptr, sx, sy, data));
}
GameApi::BM GameApi::ContinuousBitmapApi::sample(CBM c_bitmap, int sx, int sy) // SampleBitmap(CB<Color, int sx,int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_color_bitmap(e, new SampleBitmap(*cbm, sx, sy));
}
GameApi::CBM GameApi::ContinuousBitmapApi::rotate(CBM c_bitmap, float center_x, float center_y, float angle)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, c_bitmap);
  return add_continuous_bitmap(e, new RotateContinuousBitmap<Color>(cbm, center_x, center_y, angle));
  
}

GameApi::BM GameApi::ContinuousBitmapApi::to_bitmap(CBM bm, int sx, int sy)
{
  ContinuousBitmap<Color> *cbm = find_continuous_bitmap(e, bm);  
  return add_color_bitmap(e, new BitmapFromContinuousBitmap<Color>(*cbm, sx,sy));
}

GameApi::CBM GameApi::ContinuousBitmapApi::from_bitmap(BM bm, float xsize, float ysize)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bm2 = find_color_bitmap(handle);

  return add_continuous_bitmap(e, new ContinuousBitmapFromBitmap<Color>(*bm2, xsize, ysize));
}

class VoxelFunction : public Voxel<Color>
{
public:
  VoxelFunction(GameApi::EveryApi &ev, unsigned int (*fptr)(GameApi::EveryApi &ev,int x, int y, int z, void *data), int sx, int sy, int sz, void*data) : ev(ev), fptr(fptr), sx(sx), sy(sy), sz(sz), data(data) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual Color Map(int x, int y, int z) const
  {
    return Color(fptr(ev, x,y,z,data));
  }

private:
  GameApi::EveryApi &ev;
  unsigned int (*fptr)(GameApi::EveryApi &ev,int x, int y, int z, void *data);
  int sx; 
  int sy; 
  int sz; 
  void *data;
};

GameApi::VoxelApi::VoxelApi(Env &e) : e(e) { }
GameApi::VX GameApi::VoxelApi::function(unsigned int (*fptr)(EveryApi &ev, int x, int y, int z, void *data), int sx, int sy, int sz, void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));

  return add_voxel(e, new VoxelFunction(*ev, fptr, sx, sy,sz, data));
}

unsigned int GameApi::VoxelApi::get_pixel(VX v, int x, int y, int z)
{
  Voxel<Color> *c = find_voxel(e, v);
  return c->Map(x,y,z).Pixel();
}

typedef Voxel<Color> VoxelColor;

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
    return c->Map(x,y,z).Pixel()!=0x00000000;
  }
private:
  VoxelColor *c;
  CubeElem cube;
  std::vector<Point> vec;
  float ssx,ssy,ssz;
};

GameApi::P GameApi::VoxelApi::render_boxes(VX v, float sx, float sy, float sz)
{
  Voxel<Color> *vv = find_voxel(e, v);  
  return add_polygon2(e, new VoxelBoxes(vv, sx, sy, sz), 1);
}

#if 0
GameApi::BM GameApi::VoxelApi::sw_rays(O volume, VX colours, int sx, int sy, float vx, float vy, float vz, float z)
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
  ColorVoxelFaceCollection(FaceCollection &coll, Voxel< ::Color> &c, Point p, Vector v_x, Vector v_y, Vector v_z) : ForwardFaceCollection(coll), c(c), pp(p) { cc.center = v_x; cc.u_x = v_x; cc.u_y = v_y; cc.u_z = v_z; }
  virtual unsigned int Color(int face, int point) const { 
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p2 = cc.FindInternalCoords(p);
    return c.Map(p2.x,p2.y,p2.z).Pixel();
  }
private:
  Voxel< ::Color> &c;
  mutable Coords cc;
  Point pp;
};

GameApi::P GameApi::PolygonApi::color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z)
{
  Point *pp = find_point(e, p);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);

  FaceCollection *coll = find_facecoll(e, orig);
  Voxel<Color> *v = find_voxel(e, colours);
  return add_polygon(e, new ColorVoxelFaceCollection(*coll, *v, *pp, *uu_x, *uu_y, *uu_z), 1);
}

GameApi::VA GameApi::PolygonApi::create_vertex_array(GameApi::P p)
{
  FaceCollection *faces = find_facecoll(e, p);
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.copy();  
  return add_vertex_array(e, s);
}
void GameApi::PolygonApi::render_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  EnvImpl *env = EnvImpl::Environment(&e);
  if (s->texture_id!=-1 && s->texture_id<6000)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      RenderVertexArray arr(*s);
      arr.render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D);
      glClientActiveTexture(GL_TEXTURE0+0);
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-6000);

      RenderVertexArray arr(*s);
      arr.render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else
    {
      RenderVertexArray arr(*s);
      arr.render(0);
    }
}

class AnimFace : public ForwardFaceCollection
{
public:
  AnimFace(FaceCollection &coll, Vector v) : ForwardFaceCollection(coll), coll(coll), v(v) { }
  Point EndFacePoint(int face, int point) const 
  { return ForwardFaceCollection::FacePoint(face, point) + v; 
  }

private:
  FaceCollection &coll;
  Vector v;
};

GameApi::P GameApi::PolygonApi::anim_target_vector(P p, V v)
{
  FaceCollection *i = find_facecoll(e, p);
  Vector *vv = find_vector(e,v);
  FaceCollection *coll = new AnimFace(*i, *vv);
  return add_polygon(e, coll, 1);
}

class AnimFaceScale : public ForwardFaceCollection
{
public:
  AnimFaceScale(FaceCollection &coll, Point p, float scale_x, float scale_y, float scale_z) : ForwardFaceCollection(coll), coll(coll), p(p), scale_x(scale_x), scale_y(scale_y), scale_z(scale_z) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * Matrix::Translate(-p.x,-p.y,-p.z)
      * Matrix::Scale(scale_x, scale_y, scale_z)
      * Matrix::Translate(p.x,p.y,p.z);
  }

private:
  FaceCollection &coll;
  Point p;
  float scale_x, scale_y, scale_z;
};


GameApi::P GameApi::PolygonApi::anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z)
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

GameApi::P GameApi::PolygonApi::anim_target_matrix(P p, M matrix)
{
  FaceCollection *i = find_facecoll(e, p);
  Matrix *mm = find_matrix(e,matrix);
  FaceCollection *coll = new AnimFaceMatrix(*i, *mm);
  return add_polygon(e, coll, 1);
}

GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p1, PT p2, PT p3, PT p4)
{
}
GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p, float mul_x, float mul_y)
{
}
#endif

GameApi::TR GameApi::StateChangeApi::init(int paths)
{
  return add_timerange(e, paths);
}
class PFloatRenderer : public Renderer<float>
{
public:
  PFloatRenderer(GameApi::P (*fptr)(GameApi::EveryApi &e, float, void *cb), GameApi::Env &e, void *data) : fptr(fptr),e(e),data(data) { }
  FaceCollection *Index(float t) const
  {
    GameApi::EveryApi ee(e);
    GameApi::P p = fptr(ee, t, data);
    return find_facecoll(e, p);
  }
private:
  GameApi::P (*fptr)(GameApi::EveryApi &e, float, void*);
  GameApi::Env &e;
  void *data;
};

GameApi::VV GameApi::StateChangeApi::prepareloop(float *array, int arraysize,
						P (*fptr)(EveryApi &e, float val, void *cb), void *cb, float step_duration)
{
  if (arraysize<2) { std::cout << "Error: arraysize<2" << std::endl; GameApi::VV v; v.id = 0; return v; }
  TR t = init(1);
  for(int i=0;i<arraysize-1;i++)
    {
      float val1 = array[i];
      float val2 = array[i+1];
      t = linear(t, 0, fptr, val1, val2, step_duration, cb);
    }
  VV v = prepare(t);
  return v;
}

GameApi::TR GameApi::StateChangeApi::linear(TR tr, int path_num, P (*fptr)(EveryApi &e, float val, void *cb), float start_v, float end_v, float duration, void *cb)
{
  TROArray *arr = find_timerange(e, tr);
  TROArray *arr2 = arr->copy();
  arr2->push_back(path_num, 
		 new DefaultTimeRange<float>(start_v, end_v, duration),
		 new PFloatRenderer(fptr,e, cb));
  return add_timerange(e, arr2);
}

GameApi::VV GameApi::StateChangeApi::prepare(TR sc)
{
  return add_timerange_vertexarray(e, sc);
}
void GameApi::StateChangeApi::render(VV v, float time, SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog);
}
void GameApi::StateChangeApi::render(VV v, float time, SH shader, float (*fptr)(int path, std::string name))
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog, fptr);
}

GameApi::StateChangeApi::StateChangeApi(GameApi::Env &e, GameApi::ShaderApi &api)
: e(e),api(api) { }


GameApi::TextureApi::TextureApi(GameApi::Env &e) : e(e) { count=0; }

GameApi::TX GameApi::TextureApi::tex_plane(int sx, int sy)
{
  return add_texture(e, new TexPlane(sx,sy));
}
GameApi::TX GameApi::TextureApi::tex_bitmap(GameApi::BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  return add_texture(e, new TexBitmap(*bmc));
}
GameApi::VA GameApi::TextureApi::bind(GameApi::VA va, GameApi::TXID tx)
{
  VertexArraySet *s = find_vertex_array(e, va);
  VertexArraySet *ns = new VertexArraySet(*s);
  ns->texture_id = 6000+tx.id;
  return add_vertex_array(e, ns);
}
int GameApi::TextureApi::unique_id()
{
  count++;
  return count;
}
GameApi::TX GameApi::TextureApi::tex_assign(GameApi::TX tex, int id, int x, int y, GameApi::BM bm)
{
  TextureI *texture = find_texture(e, tex);
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<Color> *bmc = find_color_bitmap(handle);
  return add_texture(e, new TexAssign(*texture, id, x, y, *bmc));
}
GameApi::TX GameApi::TextureApi::tex_coord(GameApi::TX tex, int id, int x, int y, int width, int height)
{
  TextureI *texture = find_texture(e, tex);
  return add_texture(e, new TextureITexCoord(*texture, id, x,y,width,height));
}
GameApi::Q GameApi::TextureApi::get_tex_coord(TX tx, int id)
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
GameApi::TXID GameApi::TextureApi::prepare(TX tx)
{
  TextureI *tex = find_texture(e, tx);
  TextureIBitmap bm(*tex);
  BufferFromBitmap buf(bm);
  buf.Gen();

  GLuint id;
  glGenTextures(1, &id); 
  glClientActiveTexture(GL_TEXTURE0+0);
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

void GameApi::TextureApi::use(TXID tx, int i)
{
  glEnable(GL_TEXTURE_2D);
  glClientActiveTexture(GL_TEXTURE0+i);
  glActiveTexture(GL_TEXTURE0+i);
  glBindTexture(GL_TEXTURE_2D, tx.id);
}
void GameApi::TextureApi::unuse(TXID tx)
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
  return sv;
}
GameApi::PT GameApi::SpaceVectorApi::flow_next_point(SV v, PT p, float mult)
{
  GameApi::PT pt;
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
		       int num_points, void *data) : e(e), fptr(fptr), num_points(num_points), data(data) { }
  virtual int Size() const { return num_points; }
  virtual Point2d Map(int i) const {
    GameApi::PT p = fptr(e, i, data);
    Point2d pp = { e.point_api.pt_x(p), e.point_api.pt_y(p) };
    return pp;
  }
 
private:
  GameApi::EveryApi &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &e, int idx, void *data);
  int num_points; 
  void *data;
};


GameApi::PlaneApi::PlaneApi(Env &e) : e(e) { }

GameApi::PL GameApi::PlaneApi::function(GameApi::PT (*fptr)(EveryApi &e, int idx, void*data), int num_points, void*data)
{
  GameApi::EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_plane(e, new PlanePointsFunction( *ev, fptr, num_points, data ));
}

GameApi::WV GameApi::WaveformApi::empty(float length)
{
  return add_waveform(e, new ZeroWaveform(length, -1.0,1.0));
}
GameApi::WV GameApi::WaveformApi::sinwave(float length, float freq)
{
  return add_waveform(e, new SinWaveform(length, freq));
}
GameApi::WV GameApi::WaveformApi::sample(float *array, int length, float samplelength)
{
  return add_waveform(e, new ArrayWaveform2(array, length, samplelength, -1.0, 1.0));
}
GameApi::WV GameApi::WaveformApi::int_sample(int *array, int length, float samplelength, int min_value, int max_value)
{
  GameApi::WV w;
  return w;
}
GameApi::WV GameApi::WaveformApi::mix(GameApi::WV orig, float pos, GameApi::WV sample)
{
  Waveform *m_orig = find_waveform(e, orig);
  Waveform *m_sample = find_waveform(e, sample);
  return add_waveform(e, new MixWaveform(m_orig, pos, m_sample));
}
GameApi::WV GameApi::WaveformApi::volume_ramp(GameApi::WV orig, float old_y_value, float x_pos1, float x_pos2, float y_pos1, float y_pos2)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new VolumeRampWaveform(m_orig, old_y_value, x_pos1, x_pos2, y_pos1, y_pos2));
}
GameApi::WV GameApi::WaveformApi::freq_change(GameApi::WV orig, float old_freq, float new_freq)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new FreqChangeWaveform(m_orig, old_freq, new_freq));
}

float GameApi::WaveformApi::get_value(WV orig, float val)
{
  Waveform *m_orig = find_waveform(e, orig);
  return m_orig->Index(val);
}
float GameApi::WaveformApi::length(GameApi::WV orig)
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

GameApi::WV GameApi::WaveformApi::length_change(WV orig, float new_length)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new LengthChangeWaveform(m_orig, new_length));
}


class FunctionWaveform : public Waveform
{
public:
  FunctionWaveform(GameApi::EveryApi &ev, float (*fptr)(GameApi::EveryApi &ev, float, void*), float length, float min_value, float max_value, void *data) : ev(ev), fptr(fptr), length(length), min_value(min_value), max_value(max_value), data(data) { }
  virtual float Length() const { return length; }
  virtual float Min() const { return min_value; }
  virtual float Max() const { return max_value; }
  virtual float Index(float val) const 
  {
    return fptr(ev, val, data);
  }

private:
  GameApi::EveryApi &ev;
  float (*fptr)(GameApi::EveryApi &ev, float, void*); 
  float length;
  float min_value; 
  float max_value; 
  void *data;
};

GameApi::WV GameApi::WaveformApi::function(float (*fptr)(EveryApi &ev, float, void*), float length, float min_value, float max_value, void *data)
{
  GameApi::EveryApi *ev = new EveryApi(e);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_waveform(e, new FunctionWaveform(*ev, fptr, length, min_value, max_value, data));
}

GameApi::FOA GameApi::FloatVolumeApi::prepare(GameApi::FO object,
					      int numpoints,
					      float start_x, float start_y, float start_z,
					      float end_x, float end_y, float end_z)
{
  FloatVolumeObject *fo = find_float_volume(e, object);
  float *array = new float[numpoints*3];
  int index = 0;
  for(int i=0;i<numpoints;i++)
    {
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
	}
    }
  index/=3;
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->numpoints = index;
  return add_point_array(e, arr);
}

void GameApi::FloatVolumeApi::render(FOA array)
{
  PointArray2 *arr = find_point_array(e, array);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, arr->array);
  glDrawArrays(GL_POINTS, 0, arr->numpoints);
  glDisableClientState(GL_VERTEX_ARRAY);
}

GameApi::FO GameApi::FloatVolumeApi::function(float (*fptr)(EveryApi &ev, float x, float y, float z, void *data), void *data)
{
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  FunctionFloatVolumeObject *ff = new FunctionFloatVolumeObject(*ev, fptr, data);
  EnvImpl *env = EnvImpl::Environment(&e);
  env->deletes.push_back(std::tr1::shared_ptr<void>(ev));
  return add_float_volume(e, ff);
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

GameApi::FO GameApi::FloatVolumeApi::from_volume(GameApi::O obj, float false_val, float true_val)
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
GameApi::FO GameApi::FloatVolumeApi::from_float_bitmap(GameApi::FB bm, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
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

float distance2(GameApi::EveryApi &ev, float x, float y, float z, void *data)
{
  return sqrt(x*x+y*y+z*z);
}

GameApi::FO GameApi::FloatVolumeApi::distance()
{
  return function(distance2, NULL);
}

struct TorusData {
  Point center;
  Vector u_x;
  Vector u_y;
  float radius;
};

float torus_distance(GameApi::EveryApi &ev, float x, float y, float z, void *data)
{
  TorusData *dt = (TorusData*)data;
  Point p = { x,y,z };
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

GameApi::FO GameApi::FloatVolumeApi::torusdistance(PT center, V u_x, V u_y, float radius)
{
  TorusData *dt = new TorusData;
  dt->center = *find_point(e,center);
  dt->u_x = *find_vector(e,u_x);
  dt->u_y = *find_vector(e,u_y);
  dt->radius = radius;
  return function(torus_distance, (void*)dt);
  
}
GameApi::FO GameApi::FloatVolumeApi::minimum(FO f1, FO f2)
{
  FloatVolumeObject *obj = find_float_volume(e, f1);
  FloatVolumeObject *obj2 = find_float_volume(e, f2);
  return add_float_volume(e, new MinFloatVolumeObject(obj, obj2));
}
GameApi::FO GameApi::FloatVolumeApi::maximum(FO f1, FO f2)
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
GameApi::FO GameApi::FloatVolumeApi::move(FO f1, float dx, float dy, float dz)
{
  FloatVolumeObject *next = find_float_volume(e, f1);
  return add_float_volume(e, new MoveFloatVolumeObject(next, dx,dy,dz));
}

GameApi::BM GameApi::FloatVolumeApi::raytrace(GameApi::FO object, 
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

GameApi::MatrixApi::MatrixApi(Env &e) : e(e) { }

GameApi::M GameApi::MatrixApi::identity()
{
  return add_matrix(e, new SimpleMatrix(Matrix::Identity()));
}
GameApi::M GameApi::MatrixApi::xrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::XRotation(rot)));
}
GameApi::M GameApi::MatrixApi::yrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::YRotation(rot)));
}
GameApi::M GameApi::MatrixApi::zrot(float rot)
{
  return add_matrix(e, new SimpleMatrix(Matrix::ZRotation(rot)));
}
GameApi::M GameApi::MatrixApi::trans(float x, float y, float z)
{
  return add_matrix(e, new SimpleMatrix(Matrix::Translate(x,y,z)));
}
GameApi::M GameApi::MatrixApi::scale(float sx, float sy, float sz)
{
  return add_matrix(e, new SimpleMatrix(Matrix::Scale(sx,sy,sz)));
}
GameApi::M GameApi::MatrixApi::inverse(M mat)
{
  MatrixInterface *mat2 = find_matrix(e, mat);
  return add_matrix(e, new SimpleMatrix(Matrix::Inverse(mat2->get_matrix())));
}
GameApi::M GameApi::MatrixApi::mult(M m1, M m2)
{
  MatrixInterface *mat2a = find_matrix(e, m1);
  MatrixInterface *mat2b = find_matrix(e, m2);
  Matrix ma = mat2a->get_matrix();
  Matrix mb = mat2b->get_matrix();
  return add_matrix(e, new SimpleMatrix(ma * mb));
}
GameApi::PT GameApi::MatrixApi::mult(PT point, M matrix)
{
  MatrixInterface *mat2a = find_matrix(e, matrix);
  Matrix ma = mat2a->get_matrix();
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
    m.id = 0;
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
      MatrixInterface *m = find_matrix(e, p.m);
      Matrix mm = m->get_matrix();
      glPushMatrix();
      float mat[16] = { mm.matrix[0], mm.matrix[4], mm.matrix[8], mm.matrix[12],
			mm.matrix[1], mm.matrix[5], mm.matrix[9], mm.matrix[13],
			mm.matrix[2], mm.matrix[6], mm.matrix[10], mm.matrix[14],
			mm.matrix[3], mm.matrix[7], mm.matrix[11], mm.matrix[15] };
      
      glMultMatrixf(&mat[0]);

      poly.render_vertex_array(p.va);
      glPopMatrix();
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
