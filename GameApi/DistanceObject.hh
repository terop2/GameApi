
#include "VectorTools.hh"
#include "Effect.hh"

class DistanceObject
{
public:
  virtual int NumDistances() const=0;
  virtual float Distance(int idx, Point ps) const=0;
  virtual bool Inside(Point p) const=0;
  virtual int Idx(Point p) const=0;
  virtual Color ColorValue(int idx, Point p) const=0;
};

class ArrayDistanceObject : public DistanceObject
{
public:
  ArrayDistanceObject(DistanceObject *array, int size) : array(array), size(size) { }
  virtual int NumDistances() const
  {
    int s = size;
    int num = 0;
    for(int i=0;i<s;i++)
      {
	DistanceObject *obj = &array[i];
	num += obj->NumDistances();
      }
    return num;
  }
  virtual bool Inside(Point p) const
  {
    int s = size;
    //int num = 0;
    for(int i=0;i<s;i++)
      {
	DistanceObject *obj = &array[i];
	if (obj->Inside(p)) return true;
      }
    return false;
  }
  virtual int idx(Point p) const
  {
    int s = size;
    //int num = 0;
    for(int i=0;i<s;i++)
      {
	DistanceObject *obj = &array[i];
	if (obj->Inside(p)) return i;
      }
    return -1;
  }
  virtual float Distance(int idx, Point ps) const
  {
    int s = size;
    int num = 0;
    int oldnum=0;
    for(int i=0;i<s;i++)
      {
	DistanceObject *obj = &array[i];
	for(int j=0;j<obj->NumDistances();j++)
	  {
	    if (num==idx) return obj->Distance(num-oldnum, ps);
	    num++;
	  }
         oldnum = num;
      }
    return 0.0;
  }
  virtual Color ColorValue(int idx, Point p) const
  {
    int s = size;
    int num = 0;
    int oldnum=0;
    for(int i=0;i<s;i++)
      {
	DistanceObject *obj = &array[i];
	for(int j=0;j<obj->NumDistances();j++)
	  {
	    if (num==idx) return obj->ColorValue(num-oldnum, p);
	    num++;
	  }
         oldnum = num;
      }

  }
  
private:
  DistanceObject *array;
  int size;
};

class DistanceObjectFunction : public Function<float,float>
{
public:
  DistanceObjectFunction(const DistanceObject &o, float val, Ray &r) : o(o), val(val),r(r) { }
  float Index(float val) const
  {
    return o.Distance(0,r.Index(val));
  }
private:
  const DistanceObject &o;
  float val;
  Ray &r;
};

Point find_pos(const DistanceObject &o, float val, Ray &r, float raydist);
Color find_color(const DistanceObject &o, float val, Ray &r, float raydist);


class MinDistance : public DistanceObject
{
public:
  MinDistance(DistanceObject &array) : array(array) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    int mn = 9999999;
    int s = array.NumDistances();
    for(int i=0;i<s;i++)
      {
	float d = array.Distance(i, ps);
	if (d<mn) mn=d;

      }
    return mn;
  }
private:
  DistanceObject &array;
};

class SphereDistance : public DistanceObject
{
public:
  SphereDistance(Point center) : center(center) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    ps-=center;
    return sqrt(ps.x*ps.x+ps.y*ps.y+ps.z*ps.z);
  }
private:
  Point center;
};

class PlaneDistance : public DistanceObject
{
public:
  PlaneDistance(Point p, Vector dir) : p(p), dir(dir) { }
  virtual int NumDistances() const { return 1; }
  virtual float Distance(int idx, Point ps) const
  {
    std::pair<Vector,Vector> split = Vector::SplitToComponents(ps-p, dir);
    return split.first.Dist();
  }
private:
  Point p;
  Vector dir;
};

//
// Separate objects
//

class Separate
{
public:
  virtual float x_sep(Point p) const=0;
  virtual float y_sep(Point p) const=0;
  virtual float dist_sep(Point p) const=0;
};

class SeparateEmpty : public Separate
{
public:
  virtual float x_sep(Point p) const { return 0.0; }
  virtual float y_sep(Point p) const { return 0.0; }
  virtual float dist_sep(Point p) const { return 0.0; }
};
class SeparateX : public Separate
{
public:
  SeparateX(Separate *next, float (*fptr)(float,float,float,void*), void *data) : next(next), fptr(fptr), data(data) { }
  virtual float x_sep(Point p) const { return fptr(p.x,p.y,p.z,data); }
  virtual float y_sep(Point p) const { return next->y_sep(p); }
  virtual float dist_sep(Point p) const { return next->dist_sep(p); }
private:
  Separate *next;
  float (*fptr)(float,float,float,void*); 
  void *data;
};

class SeparateY : public Separate
{
public:
  SeparateY(Separate *next, float (*fptr)(float,float,float,void*), void *data) : next(next), fptr(fptr), data(data) { }
  virtual float x_sep(Point p) const { return next->x_sep(p); }
  virtual float y_sep(Point p) const { return fptr(p.x,p.y,p.z,data); }
  virtual float dist_sep(Point p) const { return next->dist_sep(p); }
private:
  Separate *next;
  float (*fptr)(float,float,float,void*); 
  void *data;
};

class SeparateDist : public Separate
{
public:
  SeparateDist(Separate *next, float (*fptr)(float,float,float,void*), void *data) : next(next), fptr(fptr), data(data) { }
  virtual float x_sep(Point p) const { return next->x_sep(p); }
  virtual float y_sep(Point p) const { return next->y_sep(p); }
  virtual float dist_sep(Point p) const { return fptr(p.x,p.y,p.z,data); }
private:
  Separate *next;
  float (*fptr)(float,float,float,void*); 
  void *data;
};

class SeparateRender : public FaceCollection
{
public:
  struct Key { int kx; int ky; 
    friend bool operator<(const Key &k1, const Key &k2);
  };
  SeparateRender(Separate &s, float dist, float dist_accuracy,
		 float x_range_start, float x_range_end, float x_step,
		 float y_range_start, float y_range_end, float y_step,
		 float z_range_start, float z_range_end, float z_step,
		 float u_range_start, float u_range_end, float u_step,
		 float v_range_start, float v_range_end, float v_step
		 ) : s(s), dist(dist), dist_accuracy(dist_accuracy/2.0),
		     x_range_start(x_range_start), x_range_end(x_range_end), x_step(x_step),
		     y_range_start(y_range_start), y_range_end(y_range_end), y_step(y_step),
		     z_range_start(z_range_start), z_range_end(z_range_end), z_step(z_step)
  {
    for(float x=x_range_start;x<x_range_end;x+=x_step)
      {
	for(float y=y_range_start;y<y_range_end;y+=y_step)
	  {
	    for(float z=z_range_start;z<z_range_end;z+=z_step)
	      {
		Point p(x,y,z);
		float sz = s.dist_sep(p);
		if (sz >= dist-dist_accuracy && sz <= dist+dist_accuracy)
		  {
		    
		    float u = s.x_sep(p);
		    float v = s.y_sep(p);

		    int ssx = int((u-u_range_start)/u_step);
		    int ssy = int((v-v_range_start)/v_step);
		    Key k;
		    k.kx = ssx;
		    k.ky = ssy;
		    points[k] = p;
		    //std::cout << "Key: " << k.kx << " " << k.ky << ":= " << p << std::endl;
		  }
	      }
	  }
      }
    
  }
  int X_Count() const { return (x_range_end-x_range_start)/x_step; }
  int Y_Count() const { return (y_range_end-y_range_start)/y_step; }
  virtual int NumFaces() const
  {
    return X_Count()*Y_Count();
  }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    // std::cout << face << " " << point << std::endl;
    Point p;
   switch(point)
      {
      case 0: p=points[Face_to_key(face)]; break;
      case 1: {
	Key k = Face_to_key(face);
	k.kx+=1;
	p=points[k]; break;}
      case 2: {
	Key k = Face_to_key(face);
	k.kx+=1;
	k.ky+=1;
	p=points[k];break; }
      case 3: {
	Key k = Face_to_key(face);
	k.ky+=1;
	p=points[k];break;}
      }
   //std::cout << "Point: " << p << std::endl;
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
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
  Key Face_to_key(int face) const
  {
    Key k;
    k.kx = face/X_Count();
    k.ky = face-k.kx*X_Count();
    //std::cout << "FaceToKey" << face << ":" << k.kx << " " << k.ky << std::endl;
    return k;
  }
private:
  Separate &s;
  float dist;
  float dist_accuracy;
  float x_range_start; float x_range_end; float x_step;
  float y_range_start; float y_range_end; float y_step;
  float z_range_start; float z_range_end; float z_step;
  float u_range_start; float u_range_end; float u_step;
  float v_range_start; float v_range_end; float v_step;
  mutable std::map<Key, Point> points;
};

// (x,y,z)|- r
// 
// (alfa,beta) |- (x,y,z)


// Intersection

