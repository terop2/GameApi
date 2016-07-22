
#include "GameApi_h.hh"
#include "GameApi_vo.hh"

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
EXPORT GameApi::PTS GameApi::VoxelApi::instanced_positions(VX vx, float sx, float sy, float sz, unsigned int value)
{
  Voxel<unsigned int> *c = find_voxel(e, vx);
  int ssx = c->SizeX();
  int ssy = c->SizeY();
  int ssz = c->SizeZ();
  float *arr = new float[ssx*ssy*ssz*3];
  float *t_arr = arr;
  for(int x=0;x<ssx;x++)
    for(int y=0;y<ssy;y++)
      for(int z=0;z<ssz;z++)
	{
	  if (c->Map(x,y,z)==value) {
	    *t_arr = sx*x; t_arr++;
	    *t_arr = sy*y; t_arr++;
	    *t_arr = sz*z; t_arr++;
	  }
	}
  int size = t_arr-arr;
  return add_points_api_points(e, new Instanced_Points(arr, size));
  //return arr;
}

typedef Voxel<unsigned int> VoxelColor;

class VoxelBoxes : public SingleForwardBoxableFaceCollection
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
