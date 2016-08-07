
#include "GameApi_h.hh"

class CircleImplicitFunction : public ImplicitFunction2d
{
public:
  CircleImplicitFunction(float r) : r(r) { }
  float f(float x, float y) const { return x*x+y*y-r*r; }
  float f2_u(float x) const {
    return sqrt(r*r-x*x);
  }
  float f2_l(float x) const {
    return -sqrt(r*r-x*x);
  }
private:
  float r;
};

class SphereImplicitFunction : public ImplicitFunction3d
{
public:
  SphereImplicitFunction(float r) : r(r) { }
  float f(float x, float y, float z) const
  {
    return x*x+y*y+z*z - r*r;
  }
  float f_u(float x, float y) const
  {
    return sqrt(r*r - x*x - y*y);
  }
  float f_l(float x, float y) const
  {
    return -sqrt(r*r - x*x - y*y);
  }
  float pos_x() const { return -r; }
  float pos_y() const { return -r; }
  float size_x() const { return r*2.0; }
  float size_y() const { return r*2.0; }
private:
  float r;
};
EXPORT GameApi::IM GameApi::ImplicitApi::sphere(float r)
{
  return add_implicit(e, new SphereImplicitFunction(r));
}
class BlobImplicitFunction : public ImplicitFunction3d
{
public:
  BlobImplicitFunction(float c, Point center_1, Point center_2) : c(c), center_1(center_1), center_2(center_2) { }
  float f(float x, float y, float z) const
  {
    Point p(x,y,z);
    Vector v1 = p-center_1;
    Vector v2 = p-center_2;
    float d1 = v1.Dist();
    float d2 = v2.Dist();
    return (d1+d2) - c;
  }
  float f_u(float x, float y) const
  {
    return sqrt((c*c
		 -(x-center_1.x)*(x-center_1.x)
		 -(y-center_1.y)*(y-center_1.y)
		 -(x-center_2.x)*(x-center_2.x)
		 -(y-center_2.y)*(y-center_2.y))/2.0);
  }
  float f_l(float x, float y) const
  {
    return -sqrt((c*c
		 -(x-center_1.x)*(x-center_1.x)
		 -(y-center_1.y)*(y-center_1.y)
		 -(x-center_2.x)*(x-center_2.x)
		 -(y-center_2.y)*(y-center_2.y))/2.0);
  }
  float pos_x() const { return -c; }
  float pos_y() const { return -c; }
  float size_x() const { return c*2.0; }
  float size_y() const { return c*2.0; }
private:
  float c;
  Point center_1, center_2;
};
EXPORT GameApi::IM GameApi::ImplicitApi::blob(float c, 
					      float c_x, float c_y,
					      float cc_x, float cc_y)
{
  return add_implicit(e, new BlobImplicitFunction(c,Point(c_x,c_y,0.0), Point(cc_x, cc_y, 0.0)));
}

class TranslateImplicitFunction3d : public ImplicitFunction3d
{
public:
  TranslateImplicitFunction3d(ImplicitFunction3d *next, float dx, float dy, float dz) : next(next), dx(dx), dy(dy), dz(dz) { }
  float f(float x, float y, float z) const
  {
    x-=dx;
    y-=dy;
    z-=dz;
    return next->f(x,y,z);
  }
  float f_u(float x, float y) const
  {
    x-=dx; y-=dy;
    float z = next->f_u(x,y);
    z+=dz;
    return z;
  }
  float f_l(float x, float y) const
  {
    x-=dx; y-=dy;
    float z = next->f_l(x,y);
    z+=dz;
    return z;
  }
  float pos_x() const { return dx+next->pos_x(); }
  float pos_y() const { return dy+next->pos_y(); }
  float size_x() const { return next->size_x(); }
  float size_y() const { return next->size_y(); }

private:
  ImplicitFunction3d *next;
  float dx,dy,dz;
};
EXPORT GameApi::IM GameApi::ImplicitApi::translate(IM obj, float dx, float dy, float dz)
{
  ImplicitFunction3d *imp = find_implicit(e, obj);
  return add_implicit(e, new TranslateImplicitFunction3d(imp, dx,dy,dz));
}

class LowerColor : public Bitmap<Color>
{
public:
  LowerColor(ImplicitFunction3d *imp, int sx, int sy, float dx, float dy) : imp(imp), sx(sx), sy(sy), dx(dx),dy(dy) { }
  void Prepare() { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx/=float(sx);
    yy/=float(sy);
    xx-=dx;
    yy-=dy;
    xx*=imp->size_x();
    yy*=imp->size_y();
    xx+=imp->pos_x();
    yy+=imp->pos_y();
    return imp->f_color_l(xx,yy);
  }
private:
  ImplicitFunction3d *imp;
  int sx,sy;
  //float size_x, size_y;
  float dx,dy;
};

class UpperColor : public Bitmap<Color>
{
public:
  UpperColor(ImplicitFunction3d *imp, int sx, int sy, float dx, float dy) : imp(imp), sx(sx), sy(sy), dx(dx),dy(dy) { }
  void Prepare() { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx/=float(sx);
    yy/=float(sy);
    xx-=dx;
    yy-=dy;
    xx*=imp->size_x();
    yy*=imp->size_y();
    xx+=imp->pos_x();
    yy+=imp->pos_y();
    return imp->f_color_u(xx,yy);
  }
private:
  ImplicitFunction3d *imp;
  int sx,sy;
  //float size_x, size_y;
  float dx,dy;
};

class RenderLower : public Bitmap<float>
{
public:
  RenderLower(ImplicitFunction3d *imp, int sx, int sy, float dx, float dy) : imp(imp), sx(sx), sy(sy), dx(dx),dy(dy) { }
  void Prepare() { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx/=float(sx);
    yy/=float(sy);
    xx-=dx;
    yy-=dy;
    xx*=imp->size_x();
    yy*=imp->size_y();
    xx+=imp->pos_x();
    yy+=imp->pos_y();
    float m= imp->f_l(xx,yy);
    if (m<0.0) { m = -m; }
    return m;
  }
private:
  ImplicitFunction3d *imp;
  int sx,sy;
  //float size_x, size_y;
  float dx,dy;
};



class RenderUpper : public Bitmap<float>
{
public:
  RenderUpper(ImplicitFunction3d *imp, int sx, int sy, float dx, float dy) : imp(imp), sx(sx), sy(sy), dx(dx),dy(dy) { }
  void Prepare() { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const
  {
    float xx = x;
    float yy = y;
    xx/=float(sx);
    yy/=float(sy);
    xx-=dx;
    yy-=dy;
    xx*=imp->size_x();
    yy*=imp->size_y();
    xx+=imp->pos_x();
    yy+=imp->pos_y();
    float m = imp->f_u(xx,yy);
    if (m<0.0) { m = -m; }
    //std::cout << xx << " " << yy << " = " << m << std::endl;
    return m;
  }
private:
  ImplicitFunction3d *imp;
  int sx,sy;
  //float size_x, size_y;
  float dx,dy;
};


EXPORT GameApi::FB GameApi::ImplicitApi::render_lower(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy)
{
  ImplicitFunction3d *imp = find_implicit(e, obj);
  return add_float_bitmap(e, new RenderLower(imp,sx,sy,dx,dy));
}
EXPORT GameApi::FB GameApi::ImplicitApi::render_upper(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy)
{
  ImplicitFunction3d *imp = find_implicit(e, obj);
  return add_float_bitmap(e, new RenderUpper(imp,sx,sy,dx,dy));
}

EXPORT GameApi::BM GameApi::ImplicitApi::render_lower_color(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy)
{
  ImplicitFunction3d *imp = find_implicit(e, obj);
  return add_color_bitmap(e, new LowerColor(imp,sx,sy, dx,dy));
}
EXPORT GameApi::BM GameApi::ImplicitApi::render_upper_color(IM obj, float size_x, float size_y, int sx, int sy, float dx, float dy)
{
  ImplicitFunction3d *imp = find_implicit(e, obj);
  return add_color_bitmap(e, new UpperColor(imp,sx,sy,dx,dy));
}

class FromDistanceImplicit : public ImplicitFunction3d
{
public:
  FromDistanceImplicit(DistanceRenderable *dist,
		       Point pos, float u_x, float u_y, float u_z, float sx, float sy) : dist(dist), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx),sy(sy) { }
  float f(float x, float y, float z) const
  {
    return 0.0;
  }
  float f_u(float x, float y) const
  {
    return render(x,y,true);
  }

  float f_l(float x, float y) const
  {
    return render(x,y,false);
  }
  unsigned int f_color_u(float x, float y) const
  {
    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);

    float z = f_u(x,y);
    Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    return dist->color(p);
  }
  unsigned int f_color_l(float x, float y) const
  {
    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);

    float z = f_l(x,y);
    Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    return dist->color(p);
  }
  float render(float x, float y, bool flip) const
  {
    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);
    uu_z/=uu_z.Dist();
    if (flip) uu_z.dz = -uu_z.dz;
    Point p = pos + uu_x*x/sx + uu_y*y/sy;
    float val = 0.0;
    int count = 0;
    while(1) {
      float d = dist->distance(p);
      if (d>-0.01 && d<0.01) break;
      if (count>30) return std::numeric_limits<float>::quiet_NaN();
      p+=uu_z*d;
      val+=d;
      count++;
    }
    if (flip) { return -val; }
    return val;
  }
  float pos_x() const { return -float(sx); }
  float pos_y() const { return -float(sy); }
  float size_x() const { return 2.0*float(sx); }
  float size_y() const { return 2.0*float(sy); }

private:
  DistanceRenderable *dist;
  Point pos;
  float u_x;
  float u_y;
  float u_z;
  int sx;
  int sy;
};

class FromDistanceImplicit_cyl : public ImplicitFunction3d
{
public:
  FromDistanceImplicit_cyl(DistanceRenderable *dist,
		       Point pos, float u_x, float u_y, float u_z, float sx, float sy) : dist(dist), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx),sy(sy) { }
  float f(float x, float y, float z) const
  {
    return 0.0;
  }
  float f_u(float x, float y) const
  {
    return render(x,y,true);
  }

  float f_l(float x, float y) const
  {
    return render(x,y,false);
  }
  unsigned int f_color_u(float x, float y) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 *= 2.0;

    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);
    Matrix m = Matrix::YRotation(2.0*3.14159*x1/float(sx));
    uu_x = uu_x*m;
    uu_z = uu_z*m;

    float d_x = u_x;
    float d_z = u_z;
    float d = sqrt(d_x*d_x+d_z*d_z);
    uu_x/=d;
    uu_z/=d;

    float z = f_u(x,y);
    //Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    Point p = pos + uu_y*y/float(sy);
    p+=(z*uu_x + z * uu_z);
    return dist->color(p);
  }
  unsigned int f_color_l(float x, float y) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 *= 2.0;

    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);
    Matrix m = Matrix::YRotation(2.0*3.14159*x1/float(sx));
    uu_x = uu_x*m;
    uu_z = uu_z*m;

    float d_x = u_x;
    float d_z = u_z;
    float d = sqrt(d_x*d_x+d_z*d_z);
    uu_x/=d;
    uu_z/=d;

    float z = f_l(x,y);

    Point p = pos + uu_y*y/float(sy);
    p+=z*uu_x + z * uu_z;
    //Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    return dist->color(p);
  }
  float render(float x, float y, bool flip) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 *= 2.0;


    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);
    Matrix m = Matrix::YRotation(2.0*3.14159*x1/float(sx));
    uu_x = uu_x*m;
    uu_z = uu_z*m;
    float x_d = u_x;
    float z_d = u_z;
    float d = sqrt(x_d*x_d+z_d*z_d);
    uu_x/=d;
    uu_z/=d;
    //uu_z/=uu_z.Dist();
    //if (flip) uu_z.dz = -uu_z.dz;
    Point p = pos + uu_y*y/sy;
    float val = 0.0;
    int count = 0;
    while(1) {
      float d = dist->distance(p);
      if (d>-0.01 && d<0.01) break;
      if (count>30) return std::numeric_limits<float>::quiet_NaN();
      p+=uu_x*d+uu_z*d;
      val+=d;
      count++;
    }
    //if (flip) { return -val; }
    return val;
  }
  float pos_x() const { return 0.0; }
  float pos_y() const { return 0.0; }
  float size_x() const { return float(sx); }
  float size_y() const { return float(sy); }
private:
  DistanceRenderable *dist;
  Point pos;
  float u_x;
  float u_y;
  float u_z;
  int sx;
  int sy;
};

class FromDistanceImplicit_sph : public ImplicitFunction3d
{
public:
  FromDistanceImplicit_sph(DistanceRenderable *dist,
		       Point pos, float u_x, float u_y, float u_z, float sx, float sy) : dist(dist), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx),sy(sy) { }
  float f(float x, float y, float z) const
  {
    return 0.0;
  }
  float f_u(float x, float y) const
  {
    return render(x,y,true);
  }

  float f_l(float x, float y) const
  {
    return render(x,y,false);
  }
  unsigned int f_color_u(float x, float y) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 /= 2.0;
    float y1 = y;
    //y1 += float(sy);
    //y1 /= 2.0;


    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);

    float angle1 = 3.14159*x1/float(sx);
    float angle2 = 2.0*3.14159*y1/float(sy);

    Vector pp;
    pp.dx = u_x*sin(angle1)*cos(angle2);
    pp.dy = u_y*sin(angle1)*sin(angle2);
    pp.dz = u_z*cos(angle1);
    pp/=pp.Dist();

    float z = f_u(x,y);
    //Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    Point p = pos;
    p+=z*pp;
    return dist->color(p);
  }
  unsigned int f_color_l(float x, float y) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 /= 2.0;
    float y1 = y;
    //y1 += float(sy);
    //y1 /= 2.0;

    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);

    float angle1 = 3.14159*x1/float(sx);
    float angle2 = 2.0*3.14159*y1/float(sy);

    Vector pp;
    pp.dx = u_x*sin(angle1)*cos(angle2);
    pp.dy = u_y*sin(angle1)*sin(angle2);
    pp.dz = u_z*cos(angle1);
    pp/=pp.Dist();

    float z = f_l(x,y);

    Point p = pos;
    p+=z*pp;
    //Point p = pos + uu_x*x/float(sx) + uu_y*y/float(sy) + Vector(0.0,0.0,1.0)*z;
    return dist->color(p);
  }
  float render(float x, float y, bool flip) const
  {
    float x1 = x;
    //x1 += float(sx);
    //x1 /= 2.0;
    float y1 = y;
    //y1 += float(sy);
    //y1 /= 2.0;

    Vector uu_x(u_x,0.0,0.0);
    Vector uu_y(0.0,u_y,0.0);
    Vector uu_z(0.0,0.0,u_z);

    float angle1 = 3.14159*x1/float(sx);
    float angle2 = 2.0*3.14159*y1/float(sy);

    Vector pp;
    pp.dx = u_x*sin(angle1)*cos(angle2);
    pp.dy = u_y*sin(angle1)*sin(angle2);
    pp.dz = u_z*cos(angle1);
    pp/=pp.Dist();


    Point p = pos;
    float val = 0.0;
    int count = 0;
    while(1) {
      float d = dist->distance(p);
      if (d>-0.01 && d<0.01) break;
      if (count>30) return std::numeric_limits<float>::quiet_NaN();
      p+=pp*d;
      val+=d;
      count++;
    }
    //if (flip) { return -val; }
    return val;
  }
  float pos_x() const { return 0.0; }
  float pos_y() const { return 0.0; }
  float size_x() const { return float(sx); }
  float size_y() const { return float(sy); }
private:
  DistanceRenderable *dist;
  Point pos;
  float u_x;
  float u_y;
  float u_z;
  int sx;
  int sy;
};


EXPORT GameApi::IM GameApi::ImplicitApi::from_distance(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy)
{
  DistanceRenderable *dist = find_distance(e, fd);
  return add_implicit(e, new FromDistanceImplicit(dist, Point(pos_x,pos_y,pos_z), u_x, u_y,u_z, sx,sy));
}
EXPORT GameApi::IM GameApi::ImplicitApi::from_distance_cyl(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy)
{
  DistanceRenderable *dist = find_distance(e, fd);
  return add_implicit(e, new FromDistanceImplicit_cyl(dist, Point(pos_x,pos_y,pos_z), u_x, u_y,u_z, sx,sy));
}

EXPORT GameApi::IM GameApi::ImplicitApi::from_distance_sph(FD fd, float pos_x, float pos_y, float pos_z, float u_x, float u_y, float u_z, float sx, float sy)
{
  DistanceRenderable *dist = find_distance(e, fd);
  return add_implicit(e, new FromDistanceImplicit_sph(dist, Point(pos_x,pos_y,pos_z), u_x, u_y,u_z, sx,sy));
}
