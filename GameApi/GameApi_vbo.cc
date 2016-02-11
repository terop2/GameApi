
#include "GameApi_h.hh"

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

