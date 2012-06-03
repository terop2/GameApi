
#include "Graph.hh"

struct UV
{
  float U;
  float V;
};

class FloatVolumeObject
{
public:
  // inside object < 0
  // outside object > 0
  // border ==0
  virtual float Value(Point p) const=0;
  virtual int Id(Point p) const=0;
  virtual UV TexCoords(int id, Point p) const=0;
};

class IdColorMapping
{
public:
  virtual Color MapColor(int id, UV uv, Point p) const=0;
};


class FVOToContinuousVoxel : public ContinuousVoxel<float>
{
public:
  FVOToContinuousVoxel(FloatVolumeObject &fvo, float sx, float sy, float sz): fvo(fvo) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual float SizeZ() const { return sz; }
  virtual float Map(float x, float y, float z) const
  {
    return fvo.Value(Point(x,y,z));
  }
private:
  FloatVolumeObject &fvo;
};

class DistanceFVO : public FloatVolumeObject
{
public:
  virtual float Value(Point p) const { return sqrt(p.x*p.x+p.y*p.y+p.z*p.z); }
};

class SphereFVO : public FloatVolumeObject
{
public:
  SphereFVO(Point center, float radius, int id) : center(center), radius(radius), id(id) { }
  virtual float Value(Point p) const { p-=center; return dist.Value(p)-radius; }

  virtual int Id(Point p) const { return id; }
  virtual UV TexCoords(int id, Point p) const
  {
    Matrix m = Matrix::CartesianToSpherical(p);
    Point p = m * p;

    UV uv;
    uv.U = p.x;
    uv.V = p.y;
    return uv;
  }

private:
  Point center;
  float radius;
  DistanceFVO dist;
  int id;
};

class IntersectFVO : public FloatVolumeObject
{
public:
  OrFVO(FloatVolumeObject &o1, FloatVolumeObject &o2) : o1(o1), o2(o2) { }
  virtual float Value(Point p) const 
  {
    return std::min(o1.Value(p),o2.Value(p));
  }
  virtual int Id(Point p) const 
  {
    float val = o1.Value(p);
    float val2 = o2.Value(p);
    if (val<val2) { return o1.Id(p); }
    return o2.Id(p); 
  }
  
  virtual UV TexCoords(int id, Point p) const
  {
    if (o1.Id(p)==id) { return o1.TexCoords(id,p); }
    if (o2.Id(p)==id) { return o2.TexCoords(id,p); }
    UV uv;
    uv.U = 0.0;
    uv.V = 0.0;
    return uv;
  }

private:
  FloatVolumeObject &o1, &o2;
};

class UnionFVO : public FloatVolumeObject
{
public:
  OrFVO(FloatVolumeObject &o1, FloatVolumeObject &o2) : o1(o1), o2(o2) { }
  virtual float Value(Point p) const 
  {
    return std::max(o1.Value(p),o2.Value(p));
  }  
  virtual int Id(Point p) const 
  {
    float val = o1.Value(p);
    float val2 = o2.Value(p);
    if (val>val2) { return o1.Id(p); }
    return o2.Id(p); 
  }
  virtual UV TexCoords(int id, Point p) const
  {
    if (o1.Id(p)==id) { return o1.TexCoords(id,p); }
    if (o2.Id(p)==id) { return o2.TexCoords(id,p); }
    UV uv;
    uv.U = 0.0;
    uv.V = 0.0;
    return uv;
  }
  
private:
  FloatVolumeObject &o1, &o2;
};
