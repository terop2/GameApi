
#include "GameApi_h.hh"
#include "GameApi_vo.hh"

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
#if 0
  Point *pp1 = find_point(e,p1);
  Point *pp2 = find_point(e,p2);
  VolumeObject *volume = find_volume(e,object);
  Range<Point> rr(*pp1,*pp2);

  Range<Point> r = FindSurface(rr, *volume, level);

  *res1 = add_point(e, r.start.x,r.start.y,r.start.z);
  *res2 = add_point(e, r.end.x,r.end.y,r.end.z);
#endif
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


EXPORT GameApi::PTS GameApi::VolumeApi::instanced_positions(O object,
							    int sx, int sy, int sz,
							    float start_x, float end_x,
							    float start_y, float end_y,
							    float start_z, float end_z)
{
  VolumeObject *volume = find_volume(e,object);
  int ssx = sx;
  int ssy = sy;
  int ssz = sz;
  float *arr = new float[ssx*ssy*ssz*3];
  float *t_arr = arr;
  for(int x=0;x<ssx;x++)
    for(int y=0;y<ssy;y++)
      for(int z=0;z<ssz;z++)
	{
	  Point p(start_x + x*(end_x-start_x)/ssx,
		  start_y + y*(end_y-start_y)/ssy,
		  start_z + z*(end_z-start_z)/ssz);
	  if (volume->Inside(p)) {
	    *t_arr = p.x; t_arr++;
	    *t_arr = p.y; t_arr++;
	    *t_arr = p.z; t_arr++;
	  }
	}
  int size = t_arr-arr;
  return add_points_api_points(e, new Instanced_Points(arr, size));

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

class HV : public HandleValue<std::pair<Vector,unsigned int> >
{
public:
  HV(std::vector<float> &points, std::vector<unsigned int> &colors) : points(points), colors(colors) { }
  void Handle(std::pair<Vector,unsigned int> p)
  {
    points.push_back(p.first.dx);
    points.push_back(p.first.dy);
    points.push_back(p.first.dz);
    colors.push_back(p.second);
  }
private:
  std::vector<float> &points;
  std::vector<unsigned int> &colors;
};
EXPORT GameApi::PTS GameApi::VolumeApi::instanced_positions(O o, int size, float wholesize)
{
  //float s = wholesize/size;
  EveryApi api(e);
  std::vector<float> vec;
  std::vector<unsigned int> vec2;
  HV hv(vec,vec2);

  VolumeObject *volume = find_volume(e,o);
  RenderVoxel(*volume, size, wholesize, hv);

  float *arr = new float[vec.size()];
  std::copy(vec.begin(), vec.end(), arr);

  return add_points_api_points(e, new Instanced_Points(arr, vec.size()));
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
class ArrayBM : public Bitmap<Color>
{
public:
  ArrayBM(ColorVolumeObject *col, int sx, int sy, float ssx, float ssy, float z) : col(col) { }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    float xx = float(x)/sx*ssx;
    float yy = float(y)/sy*ssy;
    Point p(xx,yy,z);
    unsigned int c = col->ColorValue(p);
    return Color(c);
  }

private:
  ColorVolumeObject *col;
  int sx,sy;
  float ssx,ssy;
  float z;
};
EXPORT GameApi::BM GameApi::ColorVolumeApi::array_bm(COV colours, int sx, int sy, float ssx, float ssy, float z)
{
  ColorVolumeObject *col = find_color_volume(e, colours);
  Bitmap<Color> *bm = new ArrayBM(col, sx,sy,ssx,ssy,z);
  return add_color_bitmap2(e,bm);
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
class CubeDistanceRenderable : public DistanceRenderable
{
public:
  CubeDistanceRenderable(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z) { }
  float distance(Point p) const {
    p-=Vector(start_x, start_y, start_z);
    Vector s = Vector(end_x, end_y, end_z) - Vector(start_x, start_y, start_z);
    s/=2.0;
    p-=s;
    float pp_x = fabs(p.x);
    float pp_y = fabs(p.y);
    float pp_z = fabs(p.z);
    pp_x-=s.dx;
    pp_y-=s.dy;
    pp_z-=s.dz;
    pp_x = std::max(pp_x, 0.0f);
    pp_y = std::max(pp_y, 0.0f);
    pp_z = std::max(pp_z, 0.0f);
    return sqrt(pp_x*pp_x+pp_y*pp_y+pp_z*pp_z);
  }
private:
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  return add_distance(e, new CubeDistanceRenderable(start_x, end_x, start_y, end_y, start_z, end_z));
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

class MaxDistance2 : public DistanceRenderable
{
public:
  MaxDistance2(DistanceRenderable &r1, DistanceRenderable &r2) : r1(r1), r2(r2) { }
  float distance(Point p) const 
  { 
    float d1 = r1.distance(p);
    float d2 = r2.distance(p);
    return std::max(d1,d2);
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

EXPORT GameApi::FD GameApi::DistanceFloatVolumeApi::max(FD fd1, FD fd2)
{
  DistanceRenderable *ff1 = find_distance(e, fd1);
  DistanceRenderable *ff2 = find_distance(e, fd2);
  return add_distance(e, new MaxDistance2(*ff1, *ff2));
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