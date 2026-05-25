
#include "GameApi_h.hh"

EXPORT GameApi::FloatSceneApi::FloatSceneApi(Env &e) : e(e) { }


class FF_Ray : public FloatRay
{
public:
  FF_Ray(Point p1, Point p2) : p1(p1), p2(p2) { }
  Point Ray(float x) const { return p1 + x*Vector(p2-p1); }

private:
  Point p1,p2;
};

class FF_RayField : public Function<float,float>
{
public:
  FF_RayField(FloatRay &ray, const FloatScene &field) : ray(ray), field(field) {}
  float Index(float a) const { return field.Field(ray.Ray(a)); }
private:
  FloatRay &ray;
  const FloatScene &field;
};


class FF_SphereRays : public FF_Ray
{
public:
  FF_SphereRays(Point center, float radius, float alfa, float beta) : FF_Ray(Point(center.x + radius*cos(alfa)*cos(beta),
										   center.y + radius*sin(alfa),
										   center.z + radius*cos(alfa)*sin(beta)),
									     Point(center.x - radius*cos(alfa)*cos(beta),
										   center.y - radius*sin(alfa),
										   center.z - radius*cos(alfa)*sin(beta)))
									     { }
};

class RootFinding_Lipschitz
{
public:
  RootFinding_Lipschitz(Function<float,float> &func, float x_start, float x_end, int maxiter, float c) : func(func), x_start(x_start), x_end(x_end), maxiter(maxiter),c(c)
  {
    if (x_end<x_start) neg=-1.0; else neg=1.0;
  }
  float root(bool &found) const
  {
    //std::cout << "ROOTFINDING: " << x_start << " " << x_end << " " << maxiter << std::endl;
    found = false;
    float x = x_start;
    for(int i=0;i<maxiter;i++) {
      //std::cout << "Step:" << i << std::endl;
      float val = func.Index(x);
      
      if (fabs(val) < 0.0001f) { found = true; /*std::cout << i << "::" << x << " " << val << std::endl;*/ break; }
      //if (neg>0.0 && x >= x_end) { found = false; /*std::cout << i << "::" << "++" << std::endl;*/ break; }
      //if (neg<0.0 && x <= x_end) { found = false; /*std::cout << i << "::" << "--" << std::endl;*/ break; }
      x += neg * c * val; 
    }
    return x;
  }
private:
  Function<float,float> &func;
  float x_start;
  float x_end;
  float neg;
  int maxiter;
  float c;
};

class SphereRaysBitmap : public Bitmap<Color>
{
public:
  SphereRaysBitmap(Point center, float radius,
		   float delta_alfa, float delta_beta,
		   FloatScene &field,
		   int maxiter, float c,
		   unsigned int (FloatScene::*fptr)(Point) const)
    : center(center), radius(radius), delta_alfa(delta_alfa), delta_beta(delta_beta), field(field), maxiter(maxiter), c(c), fptr(fptr) { }
  virtual void Collect(CollectVisitor &vis) { field.Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { field.Prepare(); }

  virtual int SizeX() const { return (int(3.14159265*2.0/delta_alfa)+1); }
  virtual int SizeY() const { return (int(3.14159265/delta_beta)+1); }
  virtual Color Map(int x, int y) const
  {
    int sx = SizeX();
    int sy = SizeY();
    float xx = float(x)/float(sx);
    float yy = float(y)/float(sy);
    bool found;
    Point p = execute(xx,yy,found);
    if (!found) return 0x0;
    return Color((field.*fptr)(p));
  }
  Point execute(float alfa, float beta, bool &found) const
  {
    FF_SphereRays s_rays(center,radius,alfa,beta);
    FF_RayField r_field(s_rays,field);
    RootFinding_Lipschitz root(r_field,0.0f,1.0f,maxiter,c);
    float x = root.root(found);
    Point p = s_rays.Ray(x);
    return p;
  }
private:
  Point center;
  float radius;
  float delta_alfa;
  float delta_beta;
  FloatScene &field;
  int maxiter;
  float c;
  unsigned int (FloatScene::*fptr)(Point) const;
};



GameApi::BM GameApi::BitmapApi::sphere_rays_bitmap(float center_x, float center_y, float center_z, float radius,
						   float delta_alfa, float delta_beta,
						   GameApi::FS field,
						   int maxiter, float c,
						   int fptr_enum)
{
  FloatScene *scene = find_float_scene(e,field);
  unsigned int (FloatScene::*fptr)(Point) const;
  if (fptr_enum==0) fptr = &FloatScene::BaseColor;
  if (fptr_enum==1) fptr = &FloatScene::MetalRoughnessColor;
  if (fptr_enum==2) fptr = &FloatScene::NormalColor;
  if (fptr_enum==3) fptr = &FloatScene::OcculsionColor;
  if (fptr_enum==4) fptr = &FloatScene::EmissiveColor;
  if (fptr_enum==5) fptr = &FloatScene::SheenColor;
  if (fptr_enum==6) fptr = &FloatScene::SpecGlossiColor;
  if (fptr_enum==7) fptr = &FloatScene::DiffuseColor;
  Bitmap<Color> *bm = new SphereRaysBitmap(Point(center_x,center_y,center_z),radius,delta_alfa,delta_beta,*scene,maxiter,c,fptr);
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = bm;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}

class SphereRays : public FaceCollection
{
public:
  SphereRays(Point center, float radius, float delta_alfa, float delta_beta, FloatScene &field, int maxiter, float c, std::vector<Point> vec)
    : center(center), radius(radius), delta_alfa(delta_alfa), delta_beta(delta_beta), field(field), maxiter(maxiter), c(c),vec(vec) { }

  virtual std::string name() const { return "SphereRays"; }
  
  virtual void Collect(CollectVisitor &vis) {
    field.Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    int ss = SizeX()*SizeY();
    clear_facepoints();
    for(int i=0;i<ss;i++)
      {
	add_facepoint(i,0,0.0f,1.0f);
	add_facepoint(i,1,0.0f,1.0f);
	add_facepoint(i,2,0.0f,1.0f);
	add_facepoint(i,3,0.0f,1.0f);

	Point p1 = face_points[face_points.size()-4];
	Point p2 = face_points[face_points.size()-3];
	Point p3 = face_points[face_points.size()-2];
	Point p4 = face_points[face_points.size()-1];
	Vector v1 = p2-p1;
	Vector v2 = p3-p2;
	Vector v3 = p4-p3;
	Vector v4 = p1-p4;
	float delta_x0 = std::max(fabs(v1.dx),fabs(v2.dx));
	float delta_x1 = std::max(fabs(v3.dx),fabs(v4.dx));
	float delta_x2 = std::max(delta_x0,delta_x1);
	float delta_y0 = std::max(fabs(v1.dy),fabs(v2.dy));
	float delta_y1 = std::max(fabs(v3.dy),fabs(v4.dy));
	float delta_y2 = std::max(delta_y0,delta_y1);
	float delta_z0 = std::max(fabs(v1.dz),fabs(v2.dz));
	float delta_z1 = std::max(fabs(v3.dz),fabs(v4.dz));
	float delta_z2 = std::max(delta_z0,delta_z1);

	if (delta_x2 > 35.0 || delta_y2 > 35.0 || delta_z2 > 35.0)
	  { // delete the face
	    face_points.erase(face_points.begin()+(face_points.size()-4),
			      face_points.begin()+(face_points.size()));
	  }
	
	
      }

    int ss2 = SizeX()*SizeY();
    int ss0 = vec.size();
    for(int j=0;j<ss0;j++) {
      Point p = center;
      center+=vec[j];
      for(int i=0;i<ss2;i++)
	{
	  add_facepoint(i,0,0.5f,1.0f);
	  add_facepoint(i,1,0.5f,1.0f);
	  add_facepoint(i,2,0.5f,1.0f);
	  add_facepoint(i,3,0.5f,1.0f);
	  Point p1 = face_points[face_points.size()-4];
	  Point p2 = face_points[face_points.size()-3];
	  Point p3 = face_points[face_points.size()-2];
	  Point p4 = face_points[face_points.size()-1];
	  Vector v1 = p2-p1;
	  Vector v2 = p3-p2;
	  Vector v3 = p4-p3;
	  Vector v4 = p1-p4;
	  float delta_x0 = std::max(fabs(v1.dx),fabs(v2.dx));
	  float delta_x1 = std::max(fabs(v3.dx),fabs(v4.dx));
	  float delta_x2 = std::max(delta_x0,delta_x1);
	  float delta_y0 = std::max(fabs(v1.dy),fabs(v2.dy));
	  float delta_y1 = std::max(fabs(v3.dy),fabs(v4.dy));
	  float delta_y2 = std::max(delta_y0,delta_y1);
	  float delta_z0 = std::max(fabs(v1.dz),fabs(v2.dz));
	  float delta_z1 = std::max(fabs(v3.dz),fabs(v4.dz));
	  float delta_z2 = std::max(delta_z0,delta_z1);
	  
	  if (delta_x2 > 35.0 || delta_y2 > 35.0 || delta_z2 > 35.0)
	    { // delete the face
	      face_points.erase(face_points.begin()+(face_points.size()-4),
				face_points.begin()+(face_points.size()));
	    } 
	}
      center=p;
    }

  }
  virtual void Prepare() { field.Prepare(); HeavyPrepare(); }

  int SizeX() const { return (int(3.14159265*2.0/delta_alfa)+1); }
  int SizeY() const { return (int(3.14159265/delta_beta)+1); }
  
  virtual int NumFaces() const { return face_points.size()/4; }
  virtual int NumPoints(int face) const { return 4; }

  virtual void clear_facepoints()
  {
    face_points.clear();
  }
  virtual void add_facepoint(int face, int point, float start,float end)
  {
    float alfastart = 0.0f;
    float betastart = 0.0f;

    float alfaend = 3.14159265*2.0;
    float betaend = 3.14159265;
    
    int sx = SizeX();
    int sy = SizeY();

    int faceY = face / sx;
    int faceX = face - faceY*sx;

    //std::cout << "FACE:" << face << "::" << faceX << " " << faceY << "::" << sx << " " << sy << std::endl;

    
    if (point==0) { }
    if (point==1) faceX++;
    if (point==2) { faceX++; faceY++; }
    if (point==3) { faceY++; }

    
    float alfa = alfastart + (alfaend-alfastart)*float(faceX)/float(sx);
    float beta = betastart + (betaend-betastart)*float(faceY)/float(sy);

    //std::cout << "AB:" << alfa << " " << beta << "::" << faceX << " " << faceY << "::" << sx << " " << sy << std::endl;
    
    bool found = false;
    Point p = execute(alfa,beta,found, start,end);
    //std::cout << "Found:" << found << std::endl;
    //std::cout << "Center:" << center << std::endl;
    //std::cout << "P:" << p << std::endl;
    if (!found) { face_points.push_back(center); return; }
    face_points.push_back(p);
  }
  
  
  virtual Point FacePoint(int face, int point) const
  {
    int idx = face*4 + point;
    if (idx>=0 && idx<face_points.size())
      return face_points[idx];
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face,0);
    Point p2 = FacePoint(face,1);
    Point p3 = FacePoint(face,2);
    Vector v = Vector::CrossProduct(p2-p1,p3-p1);
    Vector res = v / v.Dist();
    store_face = face;
    store_res = res;
    return res;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const {
    int sx = SizeX();
    int sy = SizeY();

    int faceY = face / sx;
    int faceX = face - faceY*sx;

    Point2d p;
    p.x = 0.0f + float(faceX) * 1.0 /float(sx);
    p.y = 0.0f + float(faceY) * 1.0 /float(sy);
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }

  
  Point execute(float alfa, float beta, bool &found, float start, float end) const
  {
    FF_SphereRays s_rays(center,radius,alfa,beta);
    FF_RayField r_field(s_rays,field);
    RootFinding_Lipschitz root(r_field,start,end,maxiter,c);
    float x = root.root(found);
    Point p = s_rays.Ray(x);
    //std::cout << "ROOT:" << x << " " << p << std::endl;
    return p;
  }
private:
  Point center;
  float radius;
  float delta_alfa;
  float delta_beta;
  FloatScene &field;
  int maxiter;
  float c;
  mutable int store_face;
  mutable Vector store_res;
  std::vector<Point> face_points;
  std::vector<Point> vec;
};
GameApi::P GameApi::PolygonApi::sphere_rays(float center_x, float center_y, float center_z, float radius,
					    float delta_alfa, float delta_beta,
					    GameApi::FS field,
					    int maxiter, float c, std::vector<PT> vec)
{
  FloatScene *scene = find_float_scene(e,field);
  int s = vec.size();
  std::vector<Point> vec2;
  for(int i=0;i<s;i++)
    {
      Point *pt = find_point(e,vec[i]);
      if (pt)
	vec2.push_back(*pt);
    }

  
  FaceCollection *coll = new SphereRays(Point(center_x, center_y, center_z), radius, delta_alfa, delta_beta, *scene, maxiter, c,vec2);
  return add_polygon2(e,coll,1);
}

class FloatSceneWithExecute : public FloatScene
{
public:
  Point execute(float alfa, float beta, bool &found, Point center, float radius, int maxiter, float c) const
  {
    FF_SphereRays s_rays(center,radius,alfa,beta);
    FF_RayField r_field(s_rays,*this);
    RootFinding_Lipschitz root(r_field,0.0f,1.0f,maxiter,c);
    float x = root.root(found);
    Point p = s_rays.Ray(x);
    return p;
  }
};

class FF_FloatFieldColor : public FloatSceneWithExecute
{
public:
  FF_FloatFieldColor(FloatScene &scene, unsigned int color, int fptr_enum) : scene(scene), color(color), fptr_enum(fptr_enum) { }

  virtual void Collect(CollectVisitor &vis) { scene.Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }

  virtual float Field(Point p) const { return scene.Field(p); }
  virtual unsigned int BaseColor(Point p) const { if (fptr_enum==0) return color; else return scene.BaseColor(p); }
  virtual unsigned int MetalRoughnessColor(Point p) const { if (fptr_enum==1) return color; else return scene.MetalRoughnessColor(p); }
  virtual unsigned int NormalColor(Point p) const { if (fptr_enum==2) return color; else return scene.NormalColor(p); }
  virtual unsigned int OcculsionColor(Point p) const { if (fptr_enum==3) return color; else return scene.OcculsionColor(p); }
  virtual unsigned int EmissiveColor(Point p) const { if (fptr_enum==4) return color; else return scene.EmissiveColor(p); }
  virtual unsigned int SheenColor(Point p) const { if (fptr_enum==5) return color; else return scene.SheenColor(p); }
  virtual unsigned int SpecGlossiColor(Point p) const { if (fptr_enum==6) return color; else return scene.SpecGlossiColor(p); }
  virtual unsigned int DiffuseColor(Point p) const { if (fptr_enum==7) return color; else return scene.DiffuseColor(p); }
private:
  FloatScene &scene;
  unsigned int color;
  int fptr_enum;
};
GameApi::FS GameApi::FloatSceneApi::color_scene(FS scene2, unsigned int color, int fptr_enum)
{
  FloatScene *scene = find_float_scene(e,scene2);
  return add_float_scene(e, new FF_FloatFieldColor(*scene, color, fptr_enum));
}
class FF_SphereFloatField : public FloatSceneWithExecute
{
public:
  FF_SphereFloatField(Point center, float radius) : center(center), radius(radius) { }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }
  
  float Field(Point p) const { return (p-center).Dist()-radius; }
  virtual unsigned int BaseColor(Point p) const { return 0xffffffff; }
  virtual unsigned int MetalRoughnessColor(Point p) const { return 0xff000000; }
  virtual unsigned int NormalColor(Point p) const { return 0xff000000; }
  virtual unsigned int OcculsionColor(Point p) const { return 0xff000000; }
  virtual unsigned int EmissiveColor(Point p) const { return 0xff000000; }
  virtual unsigned int SheenColor(Point p) const { return 0xff000000; }
  virtual unsigned int SpecGlossiColor(Point p) const { return 0xff000000; }
  virtual unsigned int DiffuseColor(Point p) const { return 0xff000000; }
private:
  Point center;
  float radius;
};
GameApi::FS GameApi::FloatSceneApi::fs_sphere(float center_x, float center_y, float center_z, float radius)
{
  return add_float_scene(e, new FF_SphereFloatField(Point(center_x,center_y,center_z),radius));
}

class FD_to_FS : public FloatSceneWithExecute
{
public:
  FD_to_FS(GameApi::EveryApi &ev, DistanceRenderable *rend) : ev(ev), rend(rend) { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }

  virtual float Field(Point p) const { return rend->distance(p); }

  virtual unsigned int BaseColor(Point p) const { return rend->color(p); }
  virtual unsigned int MetalRoughnessColor(Point p) const { return 0xff000000; }
  virtual unsigned int NormalColor(Point p) const { return 0xff000000; }
  virtual unsigned int OcculsionColor(Point p) const { return 0xff000000; }
  virtual unsigned int EmissiveColor(Point p) const { return 0xff000000; }
  virtual unsigned int SheenColor(Point p) const { return 0xff000000; }
  virtual unsigned int SpecGlossiColor(Point p) const { return 0xff000000; }
  virtual unsigned int DiffuseColor(Point p) const { return 0xff000000; }
private:
  GameApi::EveryApi &ev;
  DistanceRenderable *rend;
};

GameApi::FS GameApi::FloatSceneApi::fd_to_fs(EveryApi &ev, FD fd)
{
  DistanceRenderable *rend = find_distance(e,fd);
  return add_float_scene(e, new FD_to_FS(ev,rend));
}
