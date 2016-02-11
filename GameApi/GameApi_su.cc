#include "GameApi_h.hh"

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
