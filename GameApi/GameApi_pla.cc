
#include "GameApi_h.hh"

class PlanePointsFunction : public PlanePoints2d
{
public:
  PlanePointsFunction( GameApi::EveryApi &e, GameApi::PT (*fptr)(GameApi::EveryApi &e, int idx, void *data),
		       int num_points, void *data, float sx, float sy) : e(e), fptr(fptr), num_points(num_points), data(data),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual int Size() const { return num_points; }
  virtual Point2d Map(int i) const {
    GameApi::PT p = fptr(e, i, data);
    Point2d pp = { e.point_api.pt_x(p), e.point_api.pt_y(p) };
    return pp;
  }
  virtual Point2d Map2(int i) const { Point2d pp = {0.0, 0.0 }; return pp; }
 
private:
  GameApi::EveryApi &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &e, int idx, void *data);
  int num_points; 
  void *data;
  float sx,sy;
};


EXPORT GameApi::PlaneApi::PlaneApi(Env &e) : e(e) { }

EXPORT GameApi::PL GameApi::PlaneApi::function(GameApi::PT (*fptr)(EveryApi &e, int idx, void*data), int num_points, float sx, float sy, void*data)
{
  GameApi::EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_plane(e, new PlanePointsFunction( *ev, fptr, num_points, data, sx,sy ));
}
class PolygonPlane : public PlanePoints2d
{
public:
  PolygonPlane(FaceCollection *coll, Matrix m, float sx, float sy) : coll(coll), m(m),sx(sx),sy(sy) { }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual int Size() const
  {
    int faces = coll->NumFaces();
    int count = 0;
    for(int i=0;i<faces;i++)
      {
	count += coll->NumPoints(i);
      }
    return count;
  }
  virtual Point2d Map2(int i) const { Point2d pp = { 0.0, 0.0 }; return pp; }
  virtual Point2d Map(int i) const
  {
    int faces = coll->NumFaces();
    int count = 0;
    int ii = 0;
    for(;ii<faces;ii++)
      {
	if (i>=count && i<count+coll->NumPoints(ii))
	  {
	    break;
	  }
	count += coll->NumPoints(ii);
      }
    Point p = coll->FacePoint(ii, i-count);
    Point p2 = p * m;
    Point2d p3 = { p2.x, p2.y };
    return p3;
  }
  virtual PlanePointsType Type(int i) const 
  { 
    if (i==0) return EMove;
    int ii=0;
    int count = 0;
    int faces = coll->NumFaces();
    for(;ii<faces;ii++)
      {
	if (i>=count && i<count+coll->NumPoints(ii))
	  {
	    break;
	  }
	count += coll->NumPoints(ii);
      }
    return i==count+coll->NumPoints(ii)-1 ? ELineTo : EMove;
  }
private:
  FaceCollection *coll;
  Matrix m;
  float sx,sy;
};
class BezierCurvePoints : public PointCollection
{
public:
  BezierCurvePoints(Point p1, Point p2, Point p3, Point p4) : p1(p1), p2(p2), p3(p3),p4(p4) { }
  Point Index(int i) const
  {
    switch(i) {
    case 0: return p1;
    case 1: return p2;
    case 2: return p3;
    case 3: return p4;
    };
    return p1;
  }
  int Size() const { return 4; }
private:
  Point p1,p2,p3,p4;
};
class SplineCurve : public CurveIn2d
{
public:
  SplineCurve(Point2d p1, Point2d p2, Point2d p3) : p0(p1), p1(p2), p2(p3) { }
  Point2d Index(float pos) const 
  {
    Point2d p;
    p.x = pos+p0.x;
    p.y = doit(pos+p0.x);
    return p;
  }
  float Size() const 
  {
    return (p1.x-p0.x) + (p2.x-p1.x);
  }
  
  float doit(float t) const {
    float a11 = 2.0/(p1.x-p0.x);
    float a12 = 1.0/(p1.x-p0.x);
    float a21 = 1.0/(p1.x-p0.x);
    float a22 = 2.0*(1.0/(p1.x-p0.x)+1.0/(p2.x-p1.x));
    float a23 = 1.0/(p2.x-p1.x);
    float a32 = 1.0/(p2.x-p1.x);
    float a33 = 2.0/(p2.x-p1.x);
    std::cout << a11 << " " << a12 << " " << a21 << " " << a22 << " " << a23 << " " << a32 << " " << a33 << std::endl;
    float b_1 = 3.0*(p1.y-p0.y)/(p1.x-p0.x)/(p1.x-p0.x);
    float b_2 = 3.0*((p1.y-p0.y)/(p1.x-p0.x)/(p1.x-p0.x) + (p2.y-p1.y)/(p2.x-p1.x)/(p2.x-p1.x));
    float b_3 = 3.0*(p2.y-p1.y)/(p2.x-p1.x)/(p2.x-p1.x);
    std::cout << b_1 << ":" << b_2 << ":" << b_3 << std::endl;

    Matrix m = { { a11, a12, 0.0, 0.0,
		   a21, a22, a23, 0.0,
		   0.0, a32, a33, 0.0,
		   0.0, 0.0, 0.0, 1.0 } };
    Matrix mi = Matrix::Inverse(m);
    Vector v( b_1, b_2, b_3 );
    Vector vv = v * mi;

    std::cout << vv.dx << "?" << vv.dy << "?" << vv.dz << std::endl;
    
    float a1 = vv.dx * (p1.x-p0.x)-(p1.y-p0.y);
    float b1 = -vv.dy * (p1.x-p0.x)+(p1.y-p0.y);
    float a2 = vv.dy * (p2.x-p1.x)-(p2.y-p1.y);
    float b2 = -vv.dz * (p2.x-p1.x)+(p2.y-p1.y);

    std::cout << a1 << "I" << b1 << "I" << a2 << "I" << b2 << std::endl;

    if (t<p1.x) {
      float tt = (t-p0.x)/(p1.x-p0.x);
      float q_1 = (1.0-tt)*p0.y + tt*p1.y + tt*(1.0-tt)*(a1*(1.0-tt)+b1*tt);
      return q_1;
    } else {
      float tt = (t-p1.x)/(p2.x-p1.x);
      float q_2 = (1.0-tt)*p1.y + tt*p2.y + tt*(1.0-tt)*(a2*(1.0-tt)+b2*tt);
      return q_2;
    }

  }

private:
  Point2d p0,p1,p2;
};

class RemoveSplines : public PlanePoints2d
{
public:
  RemoveSplines(PlanePoints2d *plane, float xdelta) : plane(plane), xdelta(xdelta) 
  {
    loop();
  }
  float SizeX() const { return plane->SizeX(); }
  float SizeY() const { return plane->SizeY(); }
  int Size() const { return vec.size(); }
  Point2d Map(int i) const { return vec[i]; }
  PlanePointsType Type(int i) const { return types[i]; }
  void loop() {
    int s = plane->Size();
    for(int i=0;i<s;i++)
      {
	PlanePointsType t = plane->Type(i);
	Point2d p = plane->Map(i);
	switch(t) {
	case EMove:
	  vec.push_back(p);
	  types.push_back(t);
	  break;
	case ELineTo:
	  vec.push_back(p);
	  types.push_back(t);
	  break;
	case ECubic:
	case EConic:
	  {
	    Point2d p1 = p;
	    //Point2d p2 = plane->Map(i+1);
	    Point2d p3 = plane->Map(i+2);
	    
	    if (plane->Type(i+3)!=ECubic&&plane->Type(i+3)!=EConic) { i+=2; break; }
	    Point2d c1= plane->Map(i+3);
	    Point2d c2= plane->Map(i+5);

	    if (t==ECubic) { std::swap(p3,p1); }

	    std::cout << "Bezier:" << p1 << " " << p3 << " " << c1 << " " << c2 << std::endl;

	    Point pp1(p1.x,p1.y,0.0);
	    Point pp2(p3.x,p3.y,0.0);
	    Point pp3(c1.x,c1.y,0.0);
	    Point pp4(c2.x,c2.y,0.0);
	    BezierCurvePoints ps(pp1,pp2,pp3,pp4);
	    BezierCurve c(ps);
	    for(float x = 0.0;x<c.Size()-xdelta;x+=xdelta)
	      {
		Point p = c.Index(x);
		Point2d pp = { p.x,p.y };
		std::cout << x << " " << p << std::endl;
		vec.push_back(pp);
		types.push_back(ELineTo);
	      }
	    Point p = c.Index(c.Size());
	    Point2d pp = { p.x,p.y };
	    vec.push_back(pp);
	    types.push_back(ELineTo);
	  }
	  i+=2;
	  break;
	};
      }
  }
private:
  PlanePoints2d *plane;
  std::vector<Point2d> vec;
  std::vector<PlanePointsType> types;
  float xdelta;
};
EXPORT GameApi::PL GameApi::PlaneApi::remove_splines(GameApi::PL pl, float xdelta)
{
  PlanePoints2d *plane= find_plane(e, pl);
  return add_plane(e, new RemoveSplines(plane, xdelta));
}


int triangulate_find_pos(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj)
{
  //std::cout << "Triangulate_find_pos start: " << std::endl;
  int current_obj = 0;
  int i = 0;
  int s = orig->Size()-1;
  for(;i<s;i++)
    {
      //std::cout << "loop i" << i << std::endl;
      if (orig->Type(i+1)!= PlanePoints2d::ELineTo)
	{
	  current_obj++;
	  continue;
	}
      if (current_obj != obj) { continue; }
      

      Point2d p = orig->Map(i);
      Point2d p2;
      if (i+2>=s)
	{
	  p2 = orig->Map(i+2-s);
	}
      else
	{
	  p2 = orig->Map(i+2);
	}
      bool outside = false;
      GameApi::CBM cbm = ev.plane_api.render_continuous(pl, obj, 0x00000000, 0xffffffff);
      for(float f = 0.1;f<0.9;f+=0.1)
	{
	  Point2d middle = { float(p.x*f+p2.x*(1.0-f)), float(p.y*f+p2.y*(1.0-f)) };
	  
	  unsigned int color = ev.cont_bitmap_api.get_pixel(cbm, middle.x,middle.y);
	  if (color==0x00000000) { outside=true; }
	}
      if (outside==true) continue;
      bool reject = false;
      int ss = orig->Size();
      for(int j=0;j<ss;j++)
	{
	  //std::cout << "loop j" << j << std::endl;
	  Point2d o = orig->Map(j);
	  
	  Point2d o2;
	  if (j+1>=ss)
	    {
	      o2 = orig->Map(j+1-ss);
	    }
	  else
	    {
	      o2 = orig->Map(j+1);
	    }
	  bool parallel = LineLineIntersection_Parallel(p,p2,o,o2);
	  if (parallel) continue;
	  Point2d intersect = LineLineIntersection(p,p2,o,o2);
	  bool bound = IsWithInBoundingBox(intersect, p,p2);
	  bool bound2 = IsWithInBoundingBox(intersect, o,o2);
	  bool comb = bound && bound2;
	  reject = comb;
	  if (reject) break;
	}
      if (reject) continue;
      break;
    }
  //std::cout << "Triangulate pos=" << i+1 << std::endl;
  if (i+1>=s) return i+1-s;
  return i+1;
}


class Triangulate1 : public PlanePoints2d
{
public:
  Triangulate1(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj) : orig(orig), pl(pl), ev(ev), obj(obj) 
  {
    if (orig->Size()<=3) pos=1;
    else
      pos = triangulate_find_pos(ev, pl, orig, obj);
  }
  float SizeX() const { return orig->SizeX(); }
  float SizeY() const 
  {
    return orig->SizeY(); 
  }
  int Size() const { 
    if (orig->Size()<3) return 0;
    return 3; 
  }
  Point2d Map(int i) const
  {
    if (i==0) 
      {
	if (pos-1<0) { return orig->Map(orig->Size()+pos-1); }
	return orig->Map(pos-1); 
      }
    if (i==1) { return orig->Map(pos); }
    if (i==2) {
      if (pos+1>=orig->Size()) return orig->Map(pos+1-orig->Size());
      return orig->Map(pos+1); 
    }
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  PlanePoints2d *orig;
  int pos;
  GameApi::PL pl;
  GameApi::EveryApi &ev;
  int obj;
};
class Triangulate2 : public PlanePoints2d
{
public:
  Triangulate2(GameApi::EveryApi &ev, GameApi::PL pl, PlanePoints2d *orig, int obj) : orig(orig) 
  {
    if (orig->Size()<=3) pos = 1;
    else
      pos = triangulate_find_pos(ev, pl, orig,obj);
  }
  float SizeX() const { return orig->SizeX(); }
  float SizeY() const { return orig->SizeY(); }
  int Size() const {
    int s = orig->Size();
    if (s<=3) return 0;
    return s-1; 
  }
  Point2d Map(int i) const
  {
    if (i<pos) { return orig->Map(i); }
    if (i+1>=orig->Size()) { return orig->Map(i+1-orig->Size()); }
    return orig->Map(i+1);
  }

private:
  PlanePoints2d *orig;
  int pos;
};
class PlanePolygon : public SingleForwardFaceCollection
{
public:
  PlanePolygon(PlanePoints2d *plane, Point pos, Vector u_x, Vector u_y) : plane(plane), pos(pos), u_x(u_x), u_y(u_y) { }
  virtual int NumFaces() const
  {
    int s = plane->Size();
    int face =0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face++;
      }
    return face;
  }
  virtual int NumPoints(int face) const
  {
    int s = plane->Size();
    int face2 =0;
    int points = 0;
    int pos = 0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face2++;
	if (face2==face+1)
	  {
	    break;
	  }
	if (t==PlanePoints2d::EMove) pos=i;
      }
    points = i - pos; 
    return points;
  }
  int PointIndex(int face, int point) const
  {
    if (point==0) { point=0; }
    else if (point==1) { point=2; }
    else if (point==2) { point=1; }

    if (face==0) { return point; }
    int s = plane->Size();
    int face2 =0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::EMove) face2++;
	if (face2==face)
	  {
	    break;
	  }
      }
    return i+point;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int index = PointIndex(face,point);
    Point2d p = plane->Map(index);
    return pos + p.x*u_x + p.y*u_y;
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
  virtual Point2d TexCoord(int face, int point) const { 
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  PlanePoints2d *plane;
  Point pos;
  Vector u_x;
  Vector u_y;
};
EXPORT GameApi::P GameApi::PlaneApi::to_polygon_face(PL pl, PT pos, V u_x, V u_y)
{
  PlanePoints2d *plane = find_plane(e,pl);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  return add_polygon(e, new PlanePolygon(plane, *pos_1, *uu_x, *uu_y),1);
}
class PlanePolygonLines : public SingleForwardFaceCollection
{
public:
  PlanePolygonLines(PlanePoints2d *plane, Point pos, Vector u_x, Vector u_y, Vector u_z, float z_mult) : plane(plane), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), z_mult(z_mult) { }
  virtual int NumFaces() const
  {
    int s = plane->Size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::ELineTo) count++;
      }
    return count;
  }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int s = plane->Size();
    int count = 0;
    int i = 1;
    for(;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	if (t==PlanePoints2d::ELineTo) count++;
	if (count==face) break;
      }
    Point2d p0 = plane->Map(i-1);
    Point2d p1 = plane->Map(i);
    Point pp0 = pos+p0.x*u_x+p0.y*u_y;
    Point pp1 = pos+p1.x*u_x+p1.y*u_y;
    Point pp2 = pos+p0.x*u_x+p0.y*u_y+z_mult*u_z;
    Point pp3 = pos+p1.x*u_x+p1.y*u_y+z_mult*u_z;
    switch(point) {
    case 0: return pp0;
    case 1: return pp1;
    case 2: return pp2;
    case 3: return pp3;
    }
    Point p;
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
  virtual Point2d TexCoord(int face, int point) const { 
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }

private:
  PlanePoints2d *plane;
  Point pos;
  Vector u_x, u_y, u_z;
  float z_mult;
};
EXPORT GameApi::P GameApi::PlaneApi::to_polygon(EveryApi &ev, PL pl, PT pos, V u_x, V u_y, V u_z, float z_mult)
{
  Vector *uu_z = find_vector(e, u_z);
  P face = to_polygon_face(pl, pos, u_x, u_y);
  P face2 = ev.polygon_api.translate(face, uu_z->dx*z_mult, uu_z->dy*z_mult, uu_z->dz*z_mult);
  P lines = to_polygon_lines_1(pl, pos, u_x, u_y, u_z, z_mult);
  P array[] = { face, face2, lines };
  return ev.polygon_api.or_array(&array[0], 3);
}
EXPORT GameApi::P GameApi::PlaneApi::to_polygon_strip(EveryApi &ev, PL pl, PT pos, V u_x, V u_y)
{
  Point *posa = find_point(e,pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);


  PlanePoints2d *plane = find_plane(e,pl);
  int sz = plane->Size();
  std::vector<PT> vec;
  int pos1 = 1;
  int pos2 = 0;
  for(int i=0;i<sz/2;i++)
    {
      Point2d p1 = plane->Map(pos2);
      Point p1a = *posa + p1.x * (*uu_x) + p1.y * (*uu_y);
      vec.push_back(add_point(e, p1a.x,p1a.y,p1a.z));
      Point2d p2 = plane->Map(pos1);
      Point p2a = *posa + p2.x * (*uu_x) + p2.y * (*uu_y);
      vec.push_back(add_point(e, p2a.x,p2a.y,p2a.z));
      pos1++; if (pos1>=sz) { pos1 = 0; }
      pos2--; if (pos2<0) { pos2=sz-1; }
    }
  if (sz%2==1)
    {
      Point2d p1 = plane->Map(pos1);
      Point p1a = *posa + p1.x * (*uu_x) + p1.y * (*uu_y);
      vec.push_back(add_point(e, p1a.x,p1a.y,p1a.z));      
    }
  return ev.polygon_api.tri_strip(&vec[0], vec.size());
}
EXPORT GameApi::P GameApi::PlaneApi::to_polygon_lines(PL pl, PT pos, V u_x, V u_y, V u_z, float z_multiplier)
{
  return to_polygon_lines_1(pl,pos,u_x,u_y,u_z,z_multiplier);
}
 GameApi::P GameApi::PlaneApi::to_polygon_lines_1(PL pl, PT pos, V u_x, V u_y, V u_z, float z_multiplier)
{
  PlanePoints2d *plane = find_plane(e,pl);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_polygon( e, new PlanePolygonLines(plane, *pos_1, *uu_x, *uu_y, *uu_z, z_multiplier), 1);
}
EXPORT std::pair<GameApi::PL, GameApi::PL> GameApi::PlaneApi::triangulate(EveryApi &ev, PL pl, int obj)
{
  PlanePoints2d *plane = find_plane(e,pl);
  PlanePoints2d *plane2 = new Triangulate1(ev, pl, plane,obj);
  PlanePoints2d *plane3 = new Triangulate2(ev, pl, plane,obj);
  PL pl1 = add_plane(e, plane2);
  PL pl2 = add_plane(e, plane3);
  return std::make_pair(pl1,pl2);
}
class EmptyPlane : public PlanePoints2d
{
public:
  EmptyPlane(float sx, float sy) : sx(sx), sy(sy) { }
  float SizeX() const { return sx; }
  float SizeY() const { return sy; }
  int Size() const { return 0; }
  Point2d Map(int i) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
private:
  float sx,sy;
};
EXPORT GameApi::PL GameApi::PlaneApi::empty_plane( float sx, float sy )
{
  return add_plane(e, new EmptyPlane(sx,sy));
}
EXPORT GameApi::PL GameApi::PlaneApi::triangulate_all(GameApi::EveryApi &ev, PL pl, int point_count, int max_obj)
{
  PlanePoints2d *plane = find_plane(e,pl);
  PL res = empty_plane(plane->SizeX(), plane->SizeY());
  for(int obj=0;obj<max_obj;obj++)
    {
      for(int i=0;i<point_count;i++)
	{
	  std::pair<PL,PL> p = triangulate(ev,pl, obj);
	  res = or_plane(res,p.first);
	  pl = p.second;
	}
    }
  return res;
}
EXPORT GameApi::PL GameApi::PlaneApi::render_p(GameApi::P poly, GameApi::M proj_matrix, float sx, float sy)
{
  FaceCollection *coll = find_facecoll(e, poly);
  Matrix m = find_matrix(e, proj_matrix);
  return add_plane(e, new PolygonPlane(coll, m, sx,sy));
}

class ColorContinuousBitmap : public ContinuousBitmap<Color>
{
public:
  ColorContinuousBitmap(ContinuousBitmap<bool> &bm, unsigned int color_0, unsigned int color_1) : bm(bm), color_0(color_0), color_1(color_1) { }
  void Prepare() { bm.Prepare(); }
  float SizeX() const { return bm.SizeX(); }
  float SizeY() const { return bm.SizeY(); }
  Color Map(float x, float y) const
  {
    bool b = bm.Map(x,y);
    if (b) return color_1;
    return color_0;
  }

private:
  ContinuousBitmap<bool> &bm;
  unsigned int color_0;
  unsigned int color_1;
};
EXPORT GameApi::BB GameApi::PlaneApi::render_bool(GameApi::PL pl, int num, int sx, int sy)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  PolygonLines *lines = new PolygonLines(*ptr, num);
  ContinuousBitmap<bool> *fill = new PolygonFill(ptr->SizeX(),ptr->SizeY(), *lines);
  Bitmap<bool> *bm = new BitmapFromContinuousBitmap<bool>(*fill, sx, sy);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(fill));
  return add_bool_bitmap(e, bm);
}
   
GameApi::CBM GameApi::PlaneApi::render_continuous(GameApi::PL pl, int num, unsigned int color_0, unsigned int color_1)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  PolygonLines *lines = new PolygonLines(*ptr, num);
  ContinuousBitmap<bool> *fill = new PolygonFill(ptr->SizeX(),ptr->SizeY(), *lines);
  ContinuousBitmap<Color> *fill2 = new ColorContinuousBitmap(*fill, color_0, color_1);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(lines));
  env->deletes.push_back(std::shared_ptr<void>(fill));
  return add_continuous_bitmap(e, fill2);
}
class FlipYPlane : public PlanePoints2d
{
public:
  FlipYPlane(PlanePoints2d *ptr) : ptr(ptr) { }
  virtual float SizeX() const { return ptr->SizeX(); }
  virtual float SizeY() const { return ptr->SizeY(); }
  virtual int Size() const { return ptr->Size(); }
  virtual Point2d Map(int i) const
  {
    Point2d p = ptr->Map(i);
    Point2d pp = Convert(p);
    return pp;
  }
  Point2d Convert(Point2d p) const
  {
    Point2d pp = { p.x, SizeY()-p.y };
    return pp;
  }
  virtual PlanePointsType Type(int i) const { 
    return ptr->Type(i);
  }


private:
  PlanePoints2d *ptr;
};

class MovePlane : public PlanePoints2d
{
public:
  MovePlane(PlanePoints2d *ptr, float dx, float dy) : ptr(ptr), dx(dx), dy(dy) { }
  virtual float SizeX() const { return ptr->SizeX(); }
  virtual float SizeY() const { return ptr->SizeY(); }
  virtual int Size() const { return ptr->Size(); }
  virtual Point2d Map(int i) const
  {
    Point2d p = ptr->Map(i);
    Point2d pp = Convert(p);
    return pp;
  }
  Point2d Convert(Point2d p) const
  {
    Point2d pp = { p.x+dx, p.y+dy };
    return pp;
  }
  virtual PlanePointsType Type(int i) const { 
    return ptr->Type(i);
  }


private:
  PlanePoints2d *ptr;
  float dx, dy;
};

EXPORT GameApi::PL GameApi::PlaneApi::flip_y(GameApi::PL pl)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  return add_plane(e, new FlipYPlane(ptr));
}
EXPORT GameApi::PL GameApi::PlaneApi::move(GameApi::PL pl, float dx, float dy)
{
  PlanePoints2d *ptr = find_plane(e, pl);
  return add_plane(e, new MovePlane(ptr, dx, dy));
}
class OrPlane : public PlanePoints2d
{
public:
  OrPlane(PlanePoints2d *ptr, PlanePoints2d *ptr2) : p1(ptr), p2(ptr2) { }
  virtual float SizeX() const { return std::max(p1->SizeX(), p2->SizeX()); }
  virtual float SizeY() const { return std::max(p1->SizeY(), p2->SizeY()); }
  virtual int Size() const {return p1->Size()+p2->Size(); }
  virtual Point2d Map(int i) const
  {
    if (i<p1->Size()) { return p1->Map(i); }
    return p2->Map(i-p1->Size());
  }
  virtual PlanePointsType Type(int i) const { 
    if (i<p1->Size()) { return p1->Type(i); }
    return p2->Type(i-p1->Size());
  }
private:
  PlanePoints2d *p1, *p2;
};
GameApi::PL GameApi::PlaneApi::or_plane(GameApi::PL p1, GameApi::PL p2)
{
  PlanePoints2d *ptr = find_plane(e, p1);
  PlanePoints2d *ptr2 = find_plane(e, p2);
  return add_plane(e, new OrPlane(ptr, ptr2));
}
class CirclePlane : public PlanePoints2d
{
public:
  CirclePlane(Point center, float radius, int numpoints) : center(center), radius(radius), numpoints(numpoints) { }
  virtual float SizeX() const { return center.x + radius; }
  virtual float SizeY() const { return center.y + radius; }
  virtual int Size() const {return numpoints+1; }
  virtual Point2d Map(int i) const
  {
    Point2d p = { (float)(radius*cos(i*2.0*3.1415926/numpoints)),
		  (float)(radius*sin(i*2.0*3.1415926/numpoints)) };
    p.x += center.x;
    p.y += center.y;
    return p;
  }
private:
  Point center;
  float radius;
  int numpoints;
};
EXPORT GameApi::PL GameApi::PlaneApi::circle(GameApi::PT center, float radius, int numpoints)
{
  Point *c = find_point(e, center);
  return add_plane(e, new CirclePlane(*c, radius, numpoints));
}

class StarPlane : public PlanePoints2d
{
public:
  StarPlane(Point center, float radius_1, float radius_2, int numpoints) : center(center), radius_1(radius_1), radius_2(radius_2), numpoints(numpoints*2) { }
  virtual float SizeX() const { return center.x + std::max(radius_1, radius_2); }
  virtual float SizeY() const { return center.y + std::max(radius_1, radius_2); }
  virtual int Size() const {return numpoints+1; }
  virtual Point2d Map(int i) const
  {
    float radius = i % 2 == 0 ? radius_1 : radius_2;
    Point2d p = { (float)(radius*cos(i*2.0*3.14159/numpoints)),
		  (float)(radius*sin(i*2.0*3.14159/numpoints)) };
    p.x+=center.x;
    p.y+=center.y;
    return p;
  }
private:
  Point center;
  float radius_1, radius_2;
  int numpoints;
};
EXPORT GameApi::PL GameApi::PlaneApi::star(GameApi::PT center, float radius_1, float radius_2, int numpoints)
{
  Point *c = find_point(e, center);
  return add_plane(e, new StarPlane(*c, radius_1, radius_2, numpoints));
}


EXPORT GameApi::PLA GameApi::PlaneApi::prepare(GameApi::PL pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  PlaneData data;
  PlanePoints2d *ptr = find_plane(e, pl);
  int s = ptr->Size();
  if (GLEW_NV_path_rendering) {
#ifndef EMSCRIPTEN
  //std::cout << "PlaneApi::prepare" << s << std::endl;
  for(int i=0;i<s;i++)
    {
      Point2d p = ptr->Map(i);
      // std::cout << "PlaneApi::prepare" << p.x << " " << p.y << std::endl;
      PlanePoints2d::PlanePointsType b = ptr->Type(i);
      data.array.push_back(p.x);
      data.array.push_back(p.y);
      if (b==PlanePoints2d::EMove) 
	{
	  if (i!=0)
	    data.cmd_array.push_back(GL_CLOSE_PATH_NV);
	  data.cmd_array.push_back(GL_MOVE_TO_NV);
	}
      else if (b==PlanePoints2d::ELineTo)
	{
	  data.cmd_array.push_back(GL_LINE_TO_NV);
	} else if (b==PlanePoints2d::ECubic||b==PlanePoints2d::EConic)
	{
	  data.cmd_array.push_back(GL_CUBIC_CURVE_TO_NV);
	  Point2d p = ptr->Map(i+1);
	  data.array.push_back(p.x);
	  data.array.push_back(p.y);
	  Point2d p2 = ptr->Map(i+2);
	  data.array.push_back(p2.x);
	  data.array.push_back(p2.y);
	  i+=2;
	} else { 
	data.cmd_array.push_back(GL_MOVE_TO_NV);
std::cout << "Type ERROR!" << std::endl; 
      }
    }
  data.cmd_array.push_back(GL_CLOSE_PATH_NV);
  env->plane_array.push_back(data);
#endif
  }
  PLA pla;
  pla.id = env->plane_array.size()-1;
  if (GLEW_NV_path_rendering)
    {
#ifndef EMSCRIPTEN
  glPathCommandsNV(pla.id, data.cmd_array.size(), &data.cmd_array[0], data.array.size(), GL_FLOAT, &data.array[0]);
#endif
    }
  return pla;
}
EXPORT void GameApi::PlaneApi::render(GameApi::PLA pla, float x, float y, float mult_x, float mult_y)
{
  //PlaneData *dt = find_plane_array(e,pla);
  //GLuint pathObj = glGenPathsNV(1);
  //glMatrixPushExt(GL_MODELVIEW);
  if (GLEW_NV_path_rendering)
    {
#ifndef EMSCRIPTEN
  glPushMatrix();
  glMatrixScalefEXT(GL_MODELVIEW, mult_x, mult_y, 1.0);
  glMatrixTranslatefEXT(GL_MODELVIEW, x, y, 0);

  //glStencilFillPathNV(pla.id, GL_COUNT_UP_NV, 0x1F);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 0x0, 0x1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  glColor3f(1,1,1);
  glStencilFillPathNV(pla.id, GL_COUNT_UP_NV, 0x1F);
  glColor3f(1.0,1.0,1.0);
  glCoverStrokePathNV(pla.id, GL_CONVEX_HULL_NV);
  glDisable(GL_STENCIL_TEST);
  glPopMatrix();
#endif
    }
}
#if 0
EXPORT GameApi::PL GameApi::PlaneApi::floodfill_border(GameApi::BB bitmap, int x, int y)
{
  BoolBitmap *bbm = find_bool_bitmap(e, bitmap);
  Bitmap<bool> *bbm2 = bbm->bitmap;
  PlanePoints2d *points = new FloodFillBorder(*bbm, x, y);
  points->run();
  return add_plane(e, points);
}
#endif


//
//
//  NEW PLANE API BELOW
//
class PlaneShapeFunction : public PlaneShape
{
public:
  PlaneShapeFunction(GameApi::Env &e, std::function<GameApi::PT (int idx)> f, int num_points, float px, float py, float sx, float sy) : e(e), f(f), num_points(num_points), px(px), py(py), sx(sx), sy(sy) { }
  virtual int NumShapes() const { return 1; }
  virtual int NumPoints(int shape) const { return num_points; }
  virtual Point2d FacePoint(int shape, int point) const
  {
    GameApi::PT pt = f(point);
    Point *pt1 = find_point(e, pt);
    Point2d p = { pt1->x, pt1->y };
    return p;
  }
  virtual unsigned int Color(int shape, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int shape, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
  virtual float PosX() const { return px; }
  virtual float PosY() const { return py; }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
private:
  GameApi::Env &e;
  std::function<GameApi::PT (int idx)> f;
  int num_points;
  float px,py;
  float sx,sy;
};
GameApi::PP GameApi::NewPlaneApi::function(std::function<PT (int idx)> f, int num_points, float px, float py, float sx, float sy)
{
  return add_plane_shape(e, new PlaneShapeFunction(e,f,num_points,px,py,sx,sy));
}

class ForwardPlaneShape : public PlaneShape
{
public:
  ForwardPlaneShape(PlaneShape *shape) : shape(shape) { }
  virtual int NumShapes() const { return shape->NumShapes(); }
  virtual int NumPoints(int sh) const { return shape->NumPoints(sh); }
  virtual Point2d FacePoint(int sh, int point) const
  {
    return shape->FacePoint(sh,point);
  }
  virtual unsigned int Color(int sh, int point) const
  {
    return shape->Color(sh,point);
  }
  virtual Point2d TexCoord(int sh, int point) const
  {
    return shape->TexCoord(sh,point);
  }
  virtual float PosX() const { return shape->PosX(); }
  virtual float PosY() const { return shape->PosY(); }
  virtual float SizeX() const { return shape->SizeX(); }
  virtual float SizeY() const { return shape->SizeY(); }

private:
  PlaneShape *shape;
};

class PolygonPlaneShape : public PlaneShape
{
public:
  PolygonPlaneShape(std::vector<Point2d> vec, float px, float py, float ex, float ey) : vec(vec),px(px), py(py), ex(ex), ey(ey) { }
  virtual int NumShapes() const { return 1; }
  virtual int NumPoints(int shape) const { return vec.size(); }
  virtual Point2d FacePoint(int shape, int point) const
  {
    return vec[point];
  }
  virtual unsigned int Color(int shape, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int shape, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
  virtual float PosX() const { return px; }
  virtual float PosY() const { return py; }
  virtual float SizeX() const { return ex-px; }
  virtual float SizeY() const { return ey-py; }
private:
  std::vector<Point2d> vec;
  float px,py,ex,ey;
};

GameApi::PP GameApi::NewPlaneApi::polygon(std::vector<PT> vec)
{
  std::vector<Point2d> vec2;
  float min_x = std::numeric_limits<float>::max();
  float max_x = std::numeric_limits<float>::min();
  float min_y = std::numeric_limits<float>::max();
  float max_y = std::numeric_limits<float>::min();
  int s=vec.size();
  for(int i=0;i<s;i++)
    {
      Point *pt = find_point(e, vec[i]);
      Point2d pt2 = { pt->x, pt->y };
      if (pt2.x < min_x) min_x = pt2.x;
      if (pt2.y < min_y) min_y = pt2.y;
      if (pt2.x > max_x) max_x = pt2.x;
      if (pt2.y > max_y) max_y = pt2.y;
      vec2.push_back(pt2);
    }
  return add_plane_shape(e, new PolygonPlaneShape(vec2, min_x, min_y, max_x, max_y));
}

class ColorPlaneShape : public ForwardPlaneShape
{
public:
  ColorPlaneShape(PlaneShape *next, unsigned int color) : ForwardPlaneShape(next), color(color) { }
  virtual unsigned int Color(int shape, int point) const
  {
    return color;
  }
private:
  unsigned int color;
};
class MovePlaneShape : public ForwardPlaneShape
{
public:
  MovePlaneShape(PlaneShape *next, float dx, float dy) : ForwardPlaneShape(next), dx(dx), dy(dy) { }
  virtual Point2d FacePoint(int shape, int point) const
  {
    Point2d p = ForwardPlaneShape::FacePoint(shape,point);
    p.x += dx;
    p.y += dy;
    return p;    
  }

private:
  float dx,dy;
};

GameApi::PP GameApi::NewPlaneApi::color(PP p, unsigned int color)
{
  PlaneShape *next = find_plane_shape(e, p);
  return add_plane_shape(e, new ColorPlaneShape(next, color));
}
GameApi::PP GameApi::NewPlaneApi::trans(PP p, float dx, float dy)
{
  PlaneShape *next = find_plane_shape(e,p);
  return add_plane_shape(e,new MovePlaneShape(next, dx,dy));
}
