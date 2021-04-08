
#include "GameApi_h.hh"
#include "GameApi_vo.hh"

class VoxelFunction : public Voxel<unsigned int>
{
public:
  VoxelFunction(GameApi::EveryApi &ev, unsigned int (*fptr)(GameApi::EveryApi &ev,int x, int y, int z, void *data), int sx, int sy, int sz, void*data) : ev(ev), fptr(fptr), sx(sx), sy(sy), sz(sz), data(data) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }
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
  c->Prepare();
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
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    return cube.PointNormal(face_to_face(face), point);
  }
  bool has_attrib() const { return true; }
  virtual float Attrib(int face, int point, int id) const
  {
    return cube.Attrib(face_to_face(face), point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return cube.AttribI(face_to_face(face), point, id);
  }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    return cube.Color(face_to_face(face), point);
  }
  bool has_texcoord() const { return true; }
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

class P_to_VX : public Voxel<int>
{
public:
  P_to_VX(FaceCollection *coll, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value) : coll(coll),sx(sx),sy(sy),sz(sz),bounds({start_x,end_x,start_y,end_y,start_z,end_z}), value(value) { }
  void Collect(CollectVisitor &vis) { coll->Collect(vis); vis.register_obj(this); }
  void HeavyPrepare() {

    p1.resize(sz);
    p2.resize(sz);
    p3.resize(sz);
    p4.resize(sz);
    for(int z=0;z<sz;z++) {
      float zz = bounds.start_z + z*(bounds.end_z-bounds.start_z)/sz;
      Plane pl(Point(bounds.start_x,bounds.start_y,zz), Vector(bounds.end_x-bounds.start_x,0.0,0.0), Vector(0.0,bounds.end_y-bounds.start_y,0.0));
      int s = coll->NumFaces();
      for(int i=0;i<s;i++) {
	Point p1a = coll->FacePoint(i, 0);
	Point p2a = coll->FacePoint(i, 1);
	Point p3a = coll->FacePoint(i, 2);
	Point p4a = coll->FacePoint(i, 3);

	Point2d pp1,pp2;
	Point2d pp3,pp4;
	bool b = pl.TriangleIntersection(p1a,p2a,p3a, pp1,pp2);
	bool b2 = pl.TriangleIntersection(p1a,p3a,p4a,pp3,pp4);
	if (b) p1[z].push_back(pp1);
	if (b) p2[z].push_back(pp2);
	if (b2) p3[z].push_back(pp3);
	if (b2) p4[z].push_back(pp4);
      }
    }

  }

  void Prepare() { 
    coll->Prepare();
    p1.resize(sz);
    p2.resize(sz);
    p3.resize(sz);
    p4.resize(sz);
    for(int z=0;z<sz;z++) {
      float zz = bounds.start_z + z*(bounds.end_z-bounds.start_z)/sz;
      Plane pl(Point(bounds.start_x,bounds.start_y,zz), Vector(bounds.end_x-bounds.start_x,0.0,0.0), Vector(0.0,bounds.end_y-bounds.start_y,0.0));
      int s = coll->NumFaces();
      for(int i=0;i<s;i++) {
	Point p1a = coll->FacePoint(i, 0);
	Point p2a = coll->FacePoint(i, 1);
	Point p3a = coll->FacePoint(i, 2);
	Point p4a = coll->FacePoint(i, 3);

	Point2d pp1,pp2;
	Point2d pp3,pp4;
	bool b = pl.TriangleIntersection(p1a,p2a,p3a, pp1,pp2);
	bool b2 = pl.TriangleIntersection(p1a,p3a,p4a,pp3,pp4);
	if (b) p1[z].push_back(pp1);
	if (b) p2[z].push_back(pp2);
	if (b2) p3[z].push_back(pp3);
	if (b2) p4[z].push_back(pp4);
      }
    }
  }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  int SizeZ() const { return sz; }
  int Map(int x, int y, int z) const
  {
    if(z<0||z>=sz) return -1;
    BOX b = find_box(x,y,z);
    float middle_z = (b.start_z+b.end_z)/2.0;

    {
    int s = p1[z].size();
    for(int i=0;i<s;i++) {
      Point2d p1a = p1[z][i];
      Point2d p2a = p2[z][i];
      LineProperties pl(Point(p1a.x,p1a.y,middle_z), Point(p2a.x,p2a.y,middle_z));
      float tmin, tmax;
      bool intersect = pl.BoxIntersection(b,tmin,tmax);
      if (intersect && ((tmin>=0.0 && tmin<=1.0) ||(tmax>=0.0 && tmax<=1.0))) return value;
    }
    }
    {
    int s = p3[z].size();
    for(int i=0;i<s;i++) {
      Point2d p3a = p3[z][i];
      Point2d p4a = p4[z][i];
      LineProperties pl(Point(p3a.x,p3a.y,middle_z), Point(p4a.x,p4a.y,middle_z));
      float tmin, tmax;
      bool intersect = pl.BoxIntersection(b,tmin,tmax);
      if (intersect && ((tmin>=0.0 && tmin<=1.0)||(tmax>=0.0&&tmax<=1.0))) return value;
    }
    }
    return -1;
  }
  BOX find_box(int x, int y, int z) const
  {
    float xx = (float(x)-0.5)/sx;
    float yy = (float(y)-0.5)/sy;
    float zz = (float(z)-0.5)/sz;
    float xxx = xx + 0.5/sx;
    float yyy = yy + 0.5/sy;
    float zzz = zz + 0.5/sz;
#if 0
    float xx = bounds.start_x + x*(bounds.end_x-bounds.start_x)/sx;
    float yy = bounds.start_y + y*(bounds.end_y-bounds.start_y)/sy;
    float zz = bounds.start_z + z*(bounds.end_z-bounds.start_z)/sz;
    float xxx = xx + (bounds.end_x-bounds.start_x)/sx;
    float yyy = yy + (bounds.end_y-bounds.start_y)/sy;
    float zzz = zz + (bounds.end_z-bounds.start_z)/sz;
#endif
    BOX b = { xx,xxx,yy,yyy,zz,zzz };
    return b;
  }
private:
  FaceCollection *coll;
  int sx,sy,sz;
  BOX bounds;
  std::vector<std::vector<Point2d> > p1;
  std::vector<std::vector<Point2d> > p2;
  std::vector<std::vector<Point2d> > p3;
  std::vector<std::vector<Point2d> > p4;
  int value;
};

EXPORT GameApi::VX GameApi::VoxelApi::convert_p_to_vx(P p, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int value)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_int_voxel(e, new P_to_VX(coll, sx,sy,sz, start_x,end_x, start_y,end_y, start_z,end_z,value));
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
