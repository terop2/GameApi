
#include "GameApi_h.hh"


class FF_Ray : public FloatFieldRay
{
public:
  FF_Ray(Point p1, Point p2) : p1(p1), p2(p2) { }
  Point Ray(float x) const { return p1 + x*(p2-p1); }

private:
  Point p1,p2;
};

class FF_RayField : public Function<float,float>
{
public:
  FF_RayField(FloatFieldRay &ray, FloatField &field) : ray(ray), field(field) {}
  float Index(float a) const { return field.Field(ray.Ray(a)); }
private:
  FloatFieldRay &ray;
  FloatField &field;
};

class FF_SphereFloatField : public FloatField
{
public:
  FF_SphereFloatField(Point center, float radius) : center(center), radius(radius) { }
  float Field(Point p) const { return (p-center).Dist()-radius; }
private:
  Point center;
  float radius;
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
    found = false;
    float x = x_start;
    for(int i=0;i<maxiter;i++) {
      float val = func.Index(x);
      if (fabs(val) < 0.0001) { found = true; break; }
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
		   FloatField &field,
		   int maxiter, float c,
		   unsigned int (FloatField::*fptr)(Point))
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
    return Color(fptr(p));
  }
  virtual void Prepare() { }

  Point execute(float alfa, float beta, bool &found)
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
  FloatField &field;
  int maxiter;
  float c;
  unsigned int (FloatField::*fptr)(Point);
};

class SphereRays : public FaceCollection
{
public:
  SphereRays(Point center, float radius, float delta_alfa, float delta_beta, FloatField &field, int maxiter, float c)
    : center(center), radius(radius), delta_alfa(delta_alfa), delta_beta(delta_beta), field(field) { }

  virtual void Collect(CollectVisitor &vis) { field.Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { field.Prepare(); }


  int SizeX() const { return (int(3.14159265*2.0/delta_alfa)+1); }
  int SizeY() const { return (int(3.14159265/delta_beta)+1); }
  
  virtual int NumFaces() const { return SizeX()*SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
 
  virtual Point FacePoint(int face, int point) const
  {
    float alfastart = 0.0f;
    float betastart = 0.0f;

    float alfaend = 3.14159265*2.0;
    float betaend = 3.14159265;
    
    int sx = SizeX();
    int sy = SizeY();

    int faceY = face / sx;
    int faceX = face - faceY*sx;

    if (point==0) { }
    if (point==1) faceX++;
    if (point==2) { faceX++; faceY++; }
    if (point==3) { faceY++; }

    
    float alfa = alfastart + (delta_alfa)*float(faceX)/float(sx);
    float beta = betastart + (delta_beta)*float(faceY)/float(sy);

    bool found;
    Point p = execute(alfa,beta,found);
    if (!found) return center;
    return p;
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

  
  Point execute(float alfa, float beta, bool &found)
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
  FloatField &field;
  int maxiter;
  float c;
  mutable int store_face;
  mutable Vector store_res;
};
