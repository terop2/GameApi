 
#include "GameApi_h.hh"


EXPORT GameApi::PolygonApi::PolygonApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new PolyPriv;
}

EXPORT GameApi::PolygonApi::~PolygonApi()
{
  delete(PolyPriv*)priv;
}

EXPORT GameApi::P GameApi::PolygonApi::from_polygon(P p, std::function<P (int face, 
									  PT p1, PT p2, PT p3, PT p4)> f)

{
  return from_polygon_1(p,f);
}
void GameApi::PolygonApi::print_stat(P p)
{
  FaceCollPolyHandle *handle = find_poly(e, p);
  FaceCollection *coll = handle->coll;
  int faces = coll->NumFaces();
  int points = coll->NumPoints(0);
  std::cout << "Faces: " << faces << std::endl;
  std::cout << "Points: " << points << std::endl;

}

GameApi::P GameApi::PolygonApi::from_polygon_1(P p, std::function<P (int face,
								     PT p1, PT p2, PT p3, PT p4)> f)
{
  FaceCollPolyHandle *handle = find_poly(e, p);
  FaceCollection *coll = handle->coll;
  int s = coll->NumFaces();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point p1 = coll->FacePoint(i, 0);
      Point p2 = coll->FacePoint(i, 1);
      Point p3 = coll->FacePoint(i, 2);
      Point p4 = coll->NumPoints(i)>3 ? coll->FacePoint(i, 3) : coll->FacePoint(i,2);
      PT pp1 = add_point(e, p1.x, p1.y, p1.z);
      PT pp2 = add_point(e, p2.x, p2.y, p2.z);
      PT pp3 = add_point(e, p3.x, p3.y, p3.z);
      PT pp4 = add_point(e, p4.x, p4.y, p4.z);
      P p = f(i, pp1, pp2, pp3, pp4);
      vec.push_back(p);
    }
  P pp = or_array(&vec[0], s);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_lines(LI li, std::function<P (int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)> f)
{
  LineCollection *coll = find_line_array(e, li);
  int count = coll->NumLines();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point p1 = coll->LinePoint(i, 0);
      Point p2 = coll->LinePoint(i, 1);
      unsigned int c1 = coll->LineColor(i, 0);
      unsigned int c2 = coll->LineColor(i, 1);
      P item = f(i,p1.x,p1.y,p1.z, p2.x,p2.y,p2.z,c1,c2);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_points(PTS pts, std::function<P (int i, float x, float y, float z, unsigned int color)> f)
{
  PointsApiPoints *p = find_pointsapi_points(e, pts);
  int count = p->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point pos = p->Pos(i);
      unsigned int color = p->Color(i);
      P item = f(i, pos.x,pos.y,pos.z,color);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_voxel(std::function<P (unsigned int c)> f, VX voxel, float dx, float dy, float dz, int max_c)
{
  std::vector<P> pieces;
  for(int i=0;i<max_c;i++)
    {
      pieces.push_back(f(i));
    }


  Voxel<unsigned int> *vox = find_voxel(e, voxel);
  int sx = vox->SizeX();
  int sy = vox->SizeY();
  int sz = vox->SizeZ();
  std::vector<P> vec_x;
  vec_x.reserve(sx);
  for(int x=0;x<sx;x++)
    {
      std::vector<P> vec_y;
      vec_y.reserve(sy);
      for(int y=0;y<sy;y++)
	{
	  std::vector<P> vec_z;
	  vec_z.reserve(sz);
	  for(int z=0;z<sz;z++)
	    {
	      unsigned int c = vox->Map(x,y,z);
	      unsigned int i = c;
	      P p = pieces[i];
	      P p2 = translate_1(p, 0.0,0.0,z*dz);
	      vec_z.push_back(p2);
	    }
	  P p = or_array_1(&vec_z[0], sz);
	  P p2 = translate_1(p, 0.0,y*dy,0.0);
	  vec_y.push_back(p2);
	}
      P p = or_array_1(&vec_y[0], sy);
      P p2 = translate_1(p, x*dx,0.0,0.0);
      vec_x.push_back(p2);
    }
  P p = or_array_1(&vec_x[0],sx);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap2(EveryApi &ev, std::function<P (int c, PT tl, PT tr, PT bl, PT br)> f, BM int_bm, FB float_bm, float dx, float dz, float height)
{
  BitmapIntHandle *handle = dynamic_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  Bitmap<int> *bm = handle->bm;
  Bitmap<float> *fbm = find_float_bitmap(e, float_bm)->bitmap;
  std::vector<P> vec;
  int sx = std::min(fbm->SizeX(),bm->SizeX());
  int sy = std::min(fbm->SizeY(),bm->SizeY());
  for(int y=0;y<sy-1;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx-1;x++)
	{
	  float height_1 = fbm->Map(x,y);
	  float height_2 = fbm->Map(x+1,y);
	  float height_3 = fbm->Map(x,y+1);
	  float height_4 = fbm->Map(x+1,y+1);

	  PT pos_1 = ev.point_api.point(0.0, height_1*height, 0.0);
	  PT pos_2 = ev.point_api.point(dx, height_2*height, 0.0);
	  PT pos_3 = ev.point_api.point(0.0, height_3*height, dz);
	  PT pos_4 = ev.point_api.point(dx, height_4*height, dz);

	  P p = f(bm->Map(x,y), pos_1, pos_2, pos_3, pos_4);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx-1);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy-1);
  return p;
}
int index_from_string(char c, std::string s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]==c) { return i; }
    }
  return 0;
}

EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap(EveryApi &ev, std::vector<P> pieces, std::string filename, std::string chars, float dx, float dz, int ssx, int ssy)
{
  if (pieces.size()==0) return empty();
  char *array = new char[ssx*ssy];
  std::ifstream ss(filename.c_str());
  for(int i=0;i<ssx*ssy;i++)
    {
      ss >> array[i];
    }

  BM int_bm = ev.bitmap_api.newintbitmap(array, ssx,ssy, [&chars](char c) { return index_from_string(c,chars); });

  BitmapIntHandle *handle = dynamic_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }

  Bitmap<int> *bm = handle->bm;
  std::vector<P> vec;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  int val = bm->Map(x,y);
	  int sk = pieces.size();
	  if (val<0) val=0;
	  if (val>=sk) val=0;
	  P p = pieces[val];
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;

}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap(std::function<P (int c)> f, BM int_bm, float dx, float dz, int max_c)
{
  BitmapIntHandle *handle = dynamic_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  std::vector<P> pieces;
  for(int i=0;i<max_c;i++)
    {
      pieces.push_back(f(i));
    }

  Bitmap<int> *bm = handle->bm;
  std::vector<P> vec;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  P p = pieces[bm->Map(x,y)];
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}

class DistFromLines : public FaceCollection
{
public:
  DistFromLines(LineCollection *coll, float d1, float d2, Point center) : coll(coll), d1(d1), d2(d2), center(center) { }
  virtual int NumFaces() const { return coll->NumLines(); }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = coll->LinePoint(face, 0);
    Point p2 = coll->LinePoint(face, 1);
    
    Vector v1 = p1 - center;
    Vector v2 = p2 - center;
    float dist1 = v1.Dist();
    float dist2 = v2.Dist();
    v1/=dist1;
    v2/=dist2;
    Point pp1a = center + v1*(dist1+d1);
    Point pp1b = center + v1*(dist1+d2);
    Point pp2a = center + v2*(dist2+d1);
    Point pp2b = center + v2*(dist2+d2);
    if (point==0) return pp1a;
    if (point==1) return pp2a;
    if (point==2) return pp2b;
    if (point==3) return pp1b;
    return pp1a;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    return v / v.Dist();
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p1 = { 0.0, 0.0 };
    Point2d p2 = { 1.0, 0.0 };
    Point2d p3 = { 1.0, 1.0 };
    Point2d p4 = { 0.0, 1.0 };
    if (point==0) return p1;
    if (point==1) return p2;
    if (point==2) return p3;
    if (point==3) return p4;
    return p1;
  }

private:
  LineCollection *coll;
  float d1,d2;
  Point center;
};
EXPORT GameApi::P GameApi::PolygonApi::dist_from_lines(LI li, float d1, float d2, PT center)
{
  Point *pt = find_point(e, center);
  LineCollection *lines = find_line_array(e, li);
  return add_polygon2(e, new DistFromLines(lines, d1, d2, *pt), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::triangle(PT p1, PT p2, PT p3)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  FaceCollection *coll = new TriElem(*pp1, *pp2, *pp3);
  return add_polygon(e, coll,1);
}
class UnitFaceColl : public ForwardFaceCollection
{
public:
  UnitFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};
EXPORT GameApi::P GameApi::PolygonApi::unit_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{ 
  FaceCollection *coll = find_facecoll(e, orig);
 Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1), 1);
}
class UnitToFaceColl : public ForwardFaceCollection
{
public:
  UnitToFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitToCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};

EXPORT GameApi::P GameApi::PolygonApi::unit_to_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitToFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1),1);
}

class UnitToFlexClass : public ForwardFaceCollection
{
public:
  UnitToFlexClass(FaceCollection *coll, Point bTL, Point bTR, Point bBL, Point bBR,
	     Point fTL, Point fTR, Point fBL, Point fBR) 
    : ForwardFaceCollection(*coll), bTL(bTL), bTR(bTR), bBL(bBL), bBR(bBR),
      fTL(fTL), fTR(fTR), fBL(fBL), fBR(fBR)
  { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face, point);
    Point pp = UnitToFlex(p, bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR);
    return pp;
  }
private:
  Point bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR;
};

EXPORT GameApi::P GameApi::PolygonApi::unit_to_flex(P orig, 
					     PT bTL, PT bTR, PT bBL, PT bBR,
					     PT fTL, PT fTR, PT fBL, PT fBR)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *bTL1 = find_point(e, bTL);
  Point *bTR1 = find_point(e, bTR);
  Point *bBL1 = find_point(e, bBL);
  Point *bBR1 = find_point(e, bBR);

  Point *fTL1 = find_point(e, fTL);
  Point *fTR1 = find_point(e, fTR);
  Point *fBL1 = find_point(e, fBL);
  Point *fBR1 = find_point(e, fBR);
  return add_polygon(e, new UnitToFlexClass(coll, *bTL1, *bTR1, *bBL1, *bBR1,
				       *fTL1, *fTR1, *fBL1, *fBR1),1);
}


EXPORT GameApi::P GameApi::PolygonApi::empty()
{
  return add_polygon(e,new EmptyBoxableFaceCollection, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::load_model(std::string filename, int num)
{
  return add_polygon2(e, new LoadObjModelFaceCollection(filename, num), 1);
} 
class SaveModel : public MainLoopItem
{
public:
  SaveModel(GameApi::PolygonApi &api, GameApi::P poly, std::string filename) : api(api), poly(poly), filename(filename)
  {
  }
  void execute(MainLoopEnv &e)
  {
    api.save_model(poly, filename);
  }
  int shader_id() { return -1; }

private:
  GameApi::PolygonApi &api;
  GameApi::P poly;
  std::string filename;
};
EXPORT GameApi::ML GameApi::PolygonApi::save_model_ml(GameApi::P poly, std::string filename)
{
  return add_main_loop(e, new SaveModel(*this, poly, filename));
}
EXPORT void GameApi::PolygonApi::save_model(GameApi::P poly, std::string filename)
{
  FaceCollection *face = find_facecoll(e, poly);
  SaveObjModelFaceCollection save(face);
  save.save(filename);
}
   
#if 0
EXPORT GameApi::P GameApi::PolygonApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p2);
  FaceCollection *coll = new TriElem(*pp1, *pp2, (*pp3)+Vector(1.0,1.0,1.0));
  return add_polygon(e, coll,1);
}
#endif
class TexCoordQuadFaceCollection : public ForwardFaceCollection
{
public:
  TexCoordQuadFaceCollection(FaceCollection &coll, TexCoordQuad q_, int sx, int sy) : ForwardFaceCollection(coll), q(q_) //,sx(sx),sy(sy) 
  {
    q.p1.x /= float(sx);
    q.p1.y /= float(sy);
    q.p2.x /= float(sx);
    q.p2.y /= float(sy);
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
  }
  virtual Point2d TexCoord(int face, int point) const 
  { 
    //std::cout << "TexCoord" << face << " " << point << std::endl;
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
    if (point==0) {
      return q.p1;
    }
    if (point==1) {
      Point2d p;
      p.x = q.p2.x;
      p.y = q.p1.y;
      return p;
    }
    if (point==2) {
      return q.p2;
    }
    if (point==3) {
      Point2d p;
      p.x = q.p1.x;
      p.y = q.p2.y;
      return p;
    }
    Point2d pp;
    pp.x = 0.0;
    pp.y = 0.0;
    return pp;
  }
private:
  TexCoordQuad q;
  //int sx,sy;
};

class TexCoordSpherical : public ForwardFaceCollection
{
public:
  TexCoordSpherical(Point center, FaceCollection *coll) : ForwardFaceCollection(*coll), center(center), coll(coll) { }
    virtual Point2d TexCoord(int face, int point) const { 
      Point p = ForwardFaceCollection::FacePoint(face,point);
      p-=Vector(center);
      float r = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
      float alfa = acos(p.z/r);
      float beta = atan2(p.y,p.x);
      alfa/=2.0*3.14159;
      beta/=2.0*3.14159;
      Point2d pp;
      pp.x = alfa;
      pp.y = beta;
      return pp;
    }
private:
  Point center;
  FaceCollection *coll;
};

class TexCoordCylindar : public ForwardFaceCollection
{
public:
  TexCoordCylindar(FaceCollection *coll, float start_y, float end_y) : ForwardFaceCollection(*coll), coll(coll), start_y(start_y), end_y(end_y) { }
    virtual Point2d TexCoord(int face, int point) const { 
      Point p = ForwardFaceCollection::FacePoint(face,point);
      //float r = sqrt(p.x*p.x+p.z*p.z);
      float alfa = atan2(p.z, p.x);
      alfa /= 2.0*3.14159;
      float z = p.y - start_y;
      z/=(end_y-start_y);
      Point2d pp;
      pp.x = alfa;
      pp.y = z;
      return pp;
    }
private:
  FaceCollection *coll;
  float start_y, end_y;
};
class TexCoordManual : public ForwardFaceCollection
{
public:
  TexCoordManual(FaceCollection *coll,
		 float p1_x, float p1_y,
		 float p2_x, float p2_y,
		 float p3_x, float p3_y,
		 float p4_x, float p4_y) : ForwardFaceCollection(*coll),
					   p1_x(p1_x), p1_y(p1_y),
					   p2_x(p2_x), p2_y(p2_y),
					   p3_x(p3_x), p3_y(p3_y),
					   p4_x(p4_x), p4_y(p4_y) { }

  Point2d TexCoord(int face, int point) const
  {
    switch(point) {
    case 0:
      {
      Point2d p1 = { p1_x, p1_y };
      return p1;
      }
    case 1:
      {
      Point2d p2 = { p2_x, p2_y };
      return p2;
      }
    case 2:
      {
      Point2d p3 = { p3_x, p3_y };
      return p3;
      }
    case 3:
      {
      Point2d p4 = { p4_x, p4_y };
      return p4;
      }
    };
    Point2d pp = { 0.0, 0.0 };
    return pp;
  }
private:
  float p1_x, p1_y;
  float p2_x, p2_y;
  float p3_x, p3_y;
  float p4_x, p4_y;
					   
};
EXPORT GameApi::P GameApi::PolygonApi::texcoord_manual(P orig,
						float p1_x, float p1_y,
						float p2_x, float p2_y,
						float p3_x, float p3_y,
						float p4_x, float p4_y)
{
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon(e, new TexCoordManual(face, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_spherical(PT center, P orig)
{
  Point *center_1 = find_point(e, center);
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon(e, new TexCoordSpherical(*center_1, face),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_cylindar(P orig, float start_y, float end_y)
{
  FaceCollection *face = find_facecoll(e, orig);
  return add_polygon(e, new TexCoordCylindar(face,start_y, end_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, TX tx, int id)
{
  TextureApi t(e);
  Q q = t.get_tex_coord_1(tx, id);
  return sprite_bind_1(p, q, tx);
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, Q q, TX tx)
{
  return sprite_bind_1(p,q,tx);
}
GameApi::P GameApi::PolygonApi::sprite_bind_1(P p, Q q, TX tx)
{
  TextureI *texture = find_texture(e, tx);
  int sx = texture->SizeX();
  int sy = texture->SizeY();
  FaceCollection *pp1 = find_facecoll(e, p);
  TexCoordQuad qq = find_tex_quad(e, q);
  return add_polygon(e, new TexCoordQuadFaceCollection(*pp1, qq, sx,sy),1);
}

EXPORT GameApi::P GameApi::PolygonApi::quad(PT p1, PT p2, PT p3, PT p4)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  Point *pp4 = find_point(e, p4);
  FaceCollection *coll = new QuadElem(*pp1, *pp2, *pp3, *pp4);
  return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_x(float x,
		  float y1, float y2,
		  float z1, float z2)
{
  Point pp1 = Point(x,y1,z1);
  Point pp2 = Point(x,y2,z1);
  Point pp3 = Point(x,y2,z2);
  Point pp4 = Point(x,y1,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_y(float x1, float x2,
		  float y,
		  float z1, float z2)
{
  Point pp1 = Point(x1,y,z1);
  Point pp2 = Point(x2,y,z1);
  Point pp3 = Point(x2,y,z2);
  Point pp4 = Point(x1,y,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_z(float x1, float x2,
		  float y1, float y2,
		  float z)
{
  Point pp1 = Point(x1,y1,z);
  Point pp2 = Point(x2,y1,z);
  Point pp3 = Point(x2,y2,z);
  Point pp4 = Point(x1,y2,z);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::rounded_cube(EveryApi &ev, float start_x, float end_x,
						    float start_y, float end_y,
						    float start_z, float end_z,
						    float r)
{
  P p110 = cube(start_x+r, end_x-r, start_y+r, end_y-r, start_z, end_z);
  P p101 = cube(start_x+r, end_x-r, start_y, end_y, start_z+r, end_z-r);
  P p011 = cube(start_x, end_x, start_y+r, end_y-r, start_z+r, end_z-r);

  PT cen111 = ev.point_api.point(start_x+r, start_y+r, start_z+r);
  P c111 = sphere(cen111, r, 60,30);

  PT cen211 = ev.point_api.point(end_x-r, start_y+r, start_z+r);
  P c211 = sphere(cen211, r, 60,30);

  PT cen121 = ev.point_api.point(start_x+r, end_y-r, start_z+r);
  P c121 = sphere(cen121, r, 60,30);

  PT cen112 = ev.point_api.point(start_x+r, start_y+r, end_z-r);
  P c112 = sphere(cen112, r, 60,30);

  PT cen122 = ev.point_api.point(start_x+r, end_y-r, end_z-r);
  P c122 = sphere(cen122, r, 60,30);

  PT cen221 = ev.point_api.point(end_x-r, end_y-r, start_z+r);
  P c221 = sphere(cen221, r, 60,30);

  PT cen212 = ev.point_api.point(end_x-r, start_y+r, end_z-r);
  P c212 = sphere(cen212, r, 60,30);

  PT cen222 = ev.point_api.point(end_x-r, end_y-r, end_z-r);
  P c222 = sphere(cen222, r, 60,30);



  P L111_211 = cone(30, cen111, cen211, r, r);
  P L111_121 = cone(30, cen111, cen121, r, r);
  P L111_112 = cone(30, cen111, cen112, r, r);

  P L222_221 = cone(30, cen222, cen221, r, r);
  P L222_122 = cone(30, cen222, cen122, r, r);
  P L222_212 = cone(30, cen222, cen212, r, r);

  P L211_221 = cone(30, cen211, cen221, r,r);
  P L211_212 = cone(30, cen211, cen212, r,r);
  P L121_221 = cone(30, cen121, cen221, r,r);
  P L121_122 = cone(30, cen121, cen122, r,r);
  P L112_212 = cone(30, cen112, cen212, r,r);
  P L112_122 = cone(30, cen112, cen122, r,r);
  

  P array[] = { p110,p101,p011, c111, c211, c121, c112, c122, c221, c212, c222,
		L111_211, L111_121, L111_112, L222_221, L222_122, L222_212,
		L211_221, L211_212, L121_221, L121_122, L112_212, L112_122
  };

  P arr = or_array(array,17+6);
  return arr;
}

EXPORT GameApi::P GameApi::PolygonApi::cube(float start_x, float end_x,
				  float start_y, float end_y,
				  float start_z, float end_z)
{
  Point p111(start_x, start_y, start_z);
  Point p112(start_x, start_y, end_z);
  Point p121(start_x, end_y, start_z);
  Point p122(start_x, end_y, end_z);
  Point p211(end_x, start_y, start_z);
  Point p212(end_x, start_y, end_z);
  Point p221(end_x, end_y, start_z);
  Point p222(end_x, end_y, end_z);
  
  FaceCollection *coll = new CubeElem(p111,p112,p121,p122,
				      p211,p212,p221,p222);
  return add_polygon2(e, coll,1);  
}

EXPORT GameApi::P GameApi::PolygonApi::cube(PT *p)
{
  Point *p111 = find_point(e,p[0]);
  Point *p112 = find_point(e,p[1]);
  Point *p121 = find_point(e,p[2]);
  Point *p122 = find_point(e,p[3]);
  Point *p211 = find_point(e,p[4]);
  Point *p212 = find_point(e,p[5]);
  Point *p221 = find_point(e,p[6]);
  Point *p222 = find_point(e,p[7]);
  
  FaceCollection *coll = new CubeElem(*p111,*p112,*p121,*p122,
				      *p211,*p212,*p221,*p222);
  return add_polygon(e, coll,1);  
}
EXPORT GameApi::P GameApi::PolygonApi::sphere(PT center, float radius, int numfaces1, int numfaces2)
{
    Point *p = find_point(e,center);
    FaceCollection *coll = new SphereElem(*p, radius, numfaces1, numfaces2);
    return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2)
{
  Point *cent = find_point(e, center);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);
  Vector *uu_x1 = find_vector(e, uu_x);
  Vector *uu_y1 = find_vector(e, uu_y);
  FaceCollection *coll = new TorusElem(numfaces1, numfaces2, *cent, *u_x1, *u_y1, radius1, *uu_x1, *uu_y1, radius2);
  return add_polygon(e, coll, 1); 
}
EXPORT GameApi::P GameApi::PolygonApi::cone(int numfaces, PT p1, PT p2, float rad1, float rad2)
{
    Point *pp1 = find_point(e,p1);
    Point *pp2 = find_point(e,p2);
    FaceCollection *coll = new ConeElem(numfaces, *pp1, *pp2, rad1, rad2);
    return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::ring(float sx, float sy, float x, int steps)
{
  std::pair<Point, Vector> array[] = 
    { 
      std::make_pair(Point(-sx,-sy,0.0), Vector(-1.0,-1.0,0.0)),
      std::make_pair(Point(sx,-sy,0.0), Vector(1.0, -1.0,0.0)),
      std::make_pair(Point(sx,sy,0.0), Vector(1.0, 1.0,0.0)),
      std::make_pair(Point(-sx,sy,0.0), Vector(-1.0, 1.0,0.0))
    };
  VectorArray<std::pair<Point,Vector> > *pvarray = new VectorArray<std::pair<Point,Vector> >(array, array+4);
  PointVectorCollectionConvert *pointvector = new PointVectorCollectionConvert(*pvarray);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(pvarray));
  env->deletes.push_back(std::shared_ptr<void>(pointvector));
  RingElem *ring = new RingElem(*pointvector, x, steps);
  return add_polygon(e, ring,1);
}

EXPORT GameApi::P GameApi::PolygonApi::shadow(P p, PT pos, V u_x, V u_y, V light_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *light = find_vector(e, light_vec);
  return add_polygon(e, new ShadowFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *light), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::reflection(P p, PT pos, V u_x, V u_y, V ref_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *ref = find_vector(e, ref_vec);
  return add_polygon(e, new ReflectFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *ref), 1);
}


EXPORT GameApi::P GameApi::PolygonApi::or_elem(P p1, P p2)
{
  FaceCollection *pp1 = find_facecoll(e, p1);
  FaceCollection *pp2 = find_facecoll(e, p2);
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>;
  coll->push_back(pp1);
  coll->push_back(pp2);
  coll->update_faces_cache();
  return add_polygon2(e, coll,1);
}
 

EXPORT GameApi::P GameApi::PolygonApi::texture(P orig, BM bm, int choose)
{
  FaceCollection *coll = find_facecoll(e, orig);
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle, choose);
  BufferFromBitmap *req = new BufferFromBitmap(*bitmap);
  SimpleTexCoords *coords = new SimpleTexCoords(*coll, 0);
  BoxableFaceCollection *coll2 = new TextureElem2(*coll, *req, *coords);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::or_array2(std::vector<P> vec)
{
  if (vec.size()>0)
    return or_array(&vec[0],vec.size());
  return empty();
}
EXPORT GameApi::P GameApi::PolygonApi::or_array(P *p1, int size)
{
  return or_array_1(p1,size);
}
GameApi::P GameApi::PolygonApi::or_array_1(P *p1, int size)
{
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<size;i++)
    {
      FaceCollection *pp1 = find_facecoll(e, p1[i]);
      if (!pp1) { std::cout << "or_array, bad FaceCollection" << std::endl;  continue; }
      vec->push_back(pp1);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>(vec->begin(), vec->end());
  coll->update_faces_cache();
  return add_polygon(e, coll,1);
}

class MixColorFaceColl : public ForwardFaceCollection
{
public:
  MixColorFaceColl(FaceCollection *c1, FaceCollection *c2, float val) : ForwardFaceCollection(*c1), c1(c1), c2(c2),val(val) { }
  unsigned int Color(int face, int point) const
  {
    unsigned int col1 = c1->Color(face,point);
    unsigned int col2 = c2->Color(face,point);
    return Color::Interpolate(col1, col2, val);
  }

private:
  FaceCollection *c1;
  FaceCollection *c2;
  float val;
};

EXPORT GameApi::P GameApi::PolygonApi::mix_color(P orig, P orig2, float val)
{
  FaceCollection *c1 = find_facecoll(e, orig);
  FaceCollection *c2 = find_facecoll(e, orig2);
  return add_polygon(e, new MixColorFaceColl(c1,c2,val), 1);
}
class Lambert : public ForwardFaceCollection
{
public:
  Lambert(FaceCollection *coll, unsigned int color, Vector light_dir) : ForwardFaceCollection(*coll), color(color), light_dir(light_dir) 
  {
    //light_dir /= light_dir.Dist();
  }
  unsigned int Color(int face, int point) const
  {
    Vector n = ForwardFaceCollection::PointNormal(face,point);
    //n /= n.Dist();
    float dot = Vector::DotProduct(n,light_dir);
    dot/=n.Dist();
    dot/=light_dir.Dist();
    //if (dot<0.0) dot=0.0;
    //if (dot>1.0) dot=1.0;
    return Color::Interpolate(color,0xffffffff, dot);
  }
private:
  unsigned int color;
  Vector light_dir;
};
EXPORT GameApi::P GameApi::PolygonApi::color_lambert(P orig, unsigned int color, V light_dir)
{
  FaceCollection *c = find_facecoll(e, orig);
  Vector *light_dir_v = find_vector(e, light_dir);
  FaceCollection *coll = new Lambert(c, color, *light_dir_v);
  return add_polygon(e, coll,1);
  
}
class ColorElem2 : public ForwardFaceCollection
{
public:
  ColorElem2(FaceCollection *coll, unsigned int col) : ForwardFaceCollection(*coll), col(col) { }
  unsigned int Color(int face, int point) const {return col; }
private:
  unsigned int col;
};

EXPORT GameApi::P GameApi::PolygonApi::color(P next, unsigned int color)
{
  FaceCollection *c = find_facecoll(e, next);
  FaceCollection *coll = new ColorElem2(c, color);
  return add_polygon(e, coll,1);
}
class ColorGrayScale : public ForwardFaceCollection
{
public:
  ColorGrayScale(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int a = col&0xff000000;
    unsigned int r = col&0xff0000;
    unsigned int g = col&0xff00;
    unsigned int b = col&0xff;
    r>>=16;
    g>>=8;
    unsigned int c = (r+g+b)/3;
    unsigned int cc = (c<<16)+(c<<8)+c;
    return a+cc;
  }
};
class ColorFromNormals : public ForwardFaceCollection
{
public:
  ColorFromNormals(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual unsigned int Color(int face, int point) const
  {
    Vector v = ForwardFaceCollection::PointNormal(face,point);
    v/=v.Dist();
    int r = v.dx*127.5+127.5;
    int g = v.dy*127.5+127.5;
    int b = v.dz*127.5+127.5;
    r<<=16;
    g<<=8;
    return 0xff000000+r+g+b;
  }
  
};
class ColorRangeFaceCollection : public ForwardFaceCollection
{
public: 
  ColorRangeFaceCollection(FaceCollection *coll, unsigned int source_upper, unsigned int source_lower, unsigned int upper_range, unsigned int lower_range) : ForwardFaceCollection(*coll), coll(coll), source_upper(source_upper), source_lower(source_lower), upper_range(upper_range), lower_range(lower_range) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int col2 = Color::RangeChange(col, source_upper, source_lower, upper_range, lower_range);
    return col2;
  }
private:
  FaceCollection *coll;
  unsigned int source_upper, source_lower;
  unsigned int upper_range, lower_range;
};
EXPORT GameApi::P GameApi::PolygonApi::color_range(P orig, unsigned int source_upper, unsigned int source_lower, unsigned int upper_range, unsigned int lower_range)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorRangeFaceCollection(c,source_upper, source_lower, upper_range,lower_range);
  return add_polygon2(e, c2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_grayscale(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorGrayScale(c);
  return add_polygon2(e, c2, 1);
}
class QuadsToTris2 : public FaceCollection
{
public:
  QuadsToTris2(FaceCollection *coll) : coll(coll) { Iterate(); }
  void Iterate()
  {
    int counter = 0;
    int s = coll->NumFaces();
    for(int f=0;f<s;f++)
      {
	int count = coll->NumPoints(f);
	if (count==4)
	  {
	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,1));
	    vec.push_back(coll->FacePoint(f,2));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,1));
	    norm.push_back(coll->PointNormal(f,2));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,1));
	    color.push_back(coll->Color(f,2));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,1));	    
	    texcoord.push_back(coll->TexCoord(f,2));

	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,2));
	    vec.push_back(coll->FacePoint(f,3));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,2));
	    norm.push_back(coll->PointNormal(f,3));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,2));
	    color.push_back(coll->Color(f,3));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,2));
	    texcoord.push_back(coll->TexCoord(f,3));

	  }
	else
	  {
	    counts.push_back(count);
	    counts2.push_back(counter); counter+=count;
	    for(int i=0;i<count;i++)
	      {
		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i));
		texcoord.push_back(coll->TexCoord(f,i));
	      }
	  }
      }
  }
  virtual int NumFaces() const { return counts.size(); }
  virtual int NumPoints(int face) const { return counts[face]; }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[counts2[face]+point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return norm[counts2[face]+point];
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
        return color[counts2[face]+point];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
       return texcoord[counts2[face]+point]; 
  }

private:
  FaceCollection *coll;
  std::vector<int> counts;
  std::vector<int> counts2;
  std::vector<Point> vec;
  std::vector<Vector> norm;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
};
class FlipPolygonOrder : public ForwardFaceCollection
{
public:
  FlipPolygonOrder(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }

  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(face,Order(point, NumPoints(face)));
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(face,Order(point, NumPoints(face)));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(face,Order(point, NumPoints(face)),id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(face,Order(point, NumPoints(face)),id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(face,Order(point, NumPoints(face)));
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(face,Order(point, NumPoints(face)));
  }

  int Order(int a, int count) const
  {
    return count-a-1;
  }
private:
  FaceCollection *coll;
};
EXPORT GameApi::P GameApi::PolygonApi::flip_polygon_order(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new FlipPolygonOrder(c);
  return add_polygon(e,c2,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quads_to_triangles(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new QuadsToTris2(c);
  return add_polygon(e,c2,1);
}
class SkeletalAnim : public ForwardFaceCollection
{
public:
  SkeletalAnim(FaceCollection *coll, Point p0, Point p1, Point n0, Point n1)
    : ForwardFaceCollection(*coll), prop(p0, p1), prop2(n0, n1) { }

  Point FacePoint(int face, int point) const
  {
    Point pp = ForwardFaceCollection::FacePoint(face,point);
    float val = prop.LineCoords(pp);
    Point pp1 = prop.MiddlePoint(val);
    Point pp2 = prop2.MiddlePoint(val);
    Vector v = pp2-pp1;
    return pp + v;
  }
private:
  FaceCollection *coll;
  LineProperties prop;
  LineProperties prop2;
};
EXPORT GameApi::P GameApi::PolygonApi::skeletal_anim(P p, PT p_0, PT p_1,
						     PT n_0, PT n_1)
{
  FaceCollection *coll = find_facecoll(e, p);
  Point *pp_0 = find_point(e,p_0);
  Point *pp_1 = find_point(e,p_1);
  Point *nn_0 = find_point(e,n_0);
  Point *nn_1 = find_point(e,n_1);
  FaceCollection *coll2 = new SkeletalAnim(coll, *pp_0, *pp_1, *nn_0, *nn_1);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_from_normals(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorFromNormals(c);
  return add_polygon2(e, c2, 1);
}
class ColorCubeElem : public ForwardFaceCollection
{
public:
  ColorCubeElem(FaceCollection *next, Point p_0, Point p_x, Point p_y, Point p_z,
		unsigned int c_0, unsigned int c_x, unsigned int c_y, unsigned int c_z) : ForwardFaceCollection(*next), p_0(p_0), p_x(p_x), p_y(p_y), p_z(p_z), c_0(c_0), c_x(c_x), c_y(c_y), c_z(c_z) { }
  virtual unsigned int Color(int face, int point) const 
  {
    Point pos = ForwardFaceCollection::FacePoint(face,point);
    Coords c;
    c.center = p_0;
    c.u_x = p_x-p_0;
    c.u_y = p_y-p_0;
    c.u_z = p_z-p_0;
    Point pos2 = c.FindInternalCoords(pos);
    pos2.x /= c.u_x.Dist();
    pos2.y /= c.u_y.Dist();
    pos2.z /= c.u_z.Dist();

    // now [0..1]
    unsigned int cc_x_a = pos2.x*((c_x-c_0)&0xff000000);
    unsigned int cc_x_r = pos2.x*((c_x-c_0)&0xff0000);
    unsigned int cc_x_g = pos2.x*((c_x-c_0)&0xff00);
    unsigned int cc_x_b = pos2.x*((c_x-c_0)&0xff);
    unsigned int cc_y_a = pos2.y*((c_y-c_0)&0xff000000);
    unsigned int cc_y_r = pos2.y*((c_y-c_0)&0xff0000);
    unsigned int cc_y_g = pos2.y*((c_y-c_0)&0xff00);
    unsigned int cc_y_b = pos2.y*((c_y-c_0)&0xff);
    unsigned int cc_z_a = pos2.z*((c_z-c_0)&0xff000000);
    unsigned int cc_z_r = pos2.z*((c_z-c_0)&0xff0000);
    unsigned int cc_z_g = pos2.z*((c_z-c_0)&0xff00);
    unsigned int cc_z_b = pos2.z*((c_z-c_0)&0xff);
    
    cc_x_a &= 0xff000000;
    cc_y_a &= 0xff000000;
    cc_z_a &= 0xff000000;

    cc_x_r &= 0xff0000;
    cc_y_r &= 0xff0000;
    cc_z_r &= 0xff0000;

    cc_x_g &= 0xff00;
    cc_y_g &= 0xff00;
    cc_z_g &= 0xff00;

    cc_x_b &= 0xff;
    cc_y_b &= 0xff;
    cc_z_b &= 0xff;

    unsigned int cc_x = cc_x_a+cc_x_r+cc_x_g+cc_x_b;
    unsigned int cc_y = cc_y_a+cc_y_r+cc_y_g+cc_y_b;
    unsigned int cc_z = cc_z_a+cc_z_r+cc_z_g+cc_z_b;

    return c_0+cc_x+cc_y+cc_z;
  }

private:
  Point p_0, p_x, p_y, p_z;
  unsigned int c_0, c_x, c_y, c_z;
};
EXPORT GameApi::P GameApi::PolygonApi::color_cube(P next,
					   PT p_0,
					   PT p_x,
					   PT p_y,
					   PT p_z,
					   unsigned int c_0,
					   unsigned int c_x,
					   unsigned int c_y,
					   unsigned int c_z)
{
  Point *pp_0 = find_point(e,p_0);
  Point *pp_x = find_point(e,p_x);
  Point *pp_y = find_point(e,p_y);
  Point *pp_z = find_point(e,p_z);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorCubeElem(convert, *pp_0, *pp_x, *pp_y, *pp_z, c_0, c_x, c_y, c_z);
  return add_polygon2(e, coll,1);

}
EXPORT GameApi::P GameApi::PolygonApi::color_faces(P next, 
					 unsigned int color_1, 
					 unsigned int color_2,
					 unsigned int color_3, 
					 unsigned int color_4)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorFaceElem(*convert, color_1,color_2,color_3,color_4);
  return add_polygon2(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::translate(P orig, float dx, float dy, float dz)
{
  return translate_1(orig, dx,dy,dz);
}
GameApi::P GameApi::PolygonApi::translate_1(P orig, float dx, float dy, float dz)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Translate(dx,dy,dz));
  return add_polygon2(e, coll,1);
}
 
EXPORT GameApi::P GameApi::PolygonApi::rotatex(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::XRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatey(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::YRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatez(P orig, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::ZRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotate(P orig, PT point, V axis, float angle)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Point *pp = find_point(e, point);
  Vector *ax = find_vector(e, axis);
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::RotateAroundAxisPoint(*pp, *ax, angle));
  return add_polygon(e, coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::scale(P orig, float sx, float sy, float sz)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);  
  FaceCollection *c = find_facecoll(e, orig);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*convert, Matrix::Scale(sx,sy,sz));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::move(P orig, PT obj_0, V obj_x, V obj_y, V obj_z,
				     PT world_0, V world_x, V world_y, V world_z)
{
  FaceCollection *coll = find_facecoll(e, orig);
  Point *obj0 = find_point(e, obj_0);
  Vector *objx = find_vector(e, obj_x);
  Vector *objy = find_vector(e, obj_y);
  Vector *objz = find_vector(e, obj_z);

  Point *world0 = find_point(e, world_0);
  Vector *worldx = find_vector(e, world_x);
  Vector *worldy = find_vector(e, world_y);
  Vector *worldz = find_vector(e, world_z);
  
  Coords obj;
  obj.center = *obj0;
  obj.u_x = *objx;
  obj.u_y = *objy;
  obj.u_z = *objz;
  Coords world;
  world.center = *world0;
  world.u_x = *worldx;
  world.u_y = *worldy;
  world.u_z = *worldz;
  FaceCollection *coll2 = new CoordChangeFaceColl(coll, false, obj);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(coll2));  
  FaceCollection *coll3 = new CoordChangeFaceColl(coll2, true, world);
  return add_polygon(e, coll3, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::splitquads(P orig, int x_count, int y_count)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *next = new SplitQuads(*coll, x_count, y_count);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(next));  
  MemoizeFaces *next2 = new MemoizeFaces(*next);
  next2->Reset();
  return add_polygon(e, next2, 1);
}


class GameApiPointFunction2 : public Function<Point,Point>
{
public:
  GameApiPointFunction2(GameApi::Env &e, GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data), void *data) : e(e), fptr(fptr), data(data) { }
  Point Index(Point p) const
  {
    GameApi::EveryApi ev(e);
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = fptr(ev, pt, data);
    Point *pp = find_point(e, pt2);
    return *pp;
  }
  
private:
  GameApi::Env &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data);
  void *data;
};
class GameApiPointFunction : public Function<Point,Point>
{
public:
  GameApiPointFunction(GameApi::Env &e, GameApi::FunctionCb<GameApi::PT,GameApi::PT> *cb) : e(e), cb(cb){ }
  Point Index(Point p) const
  {
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = cb->Map(pt);
    Point *pp = find_point(e, pt2);
    if (!pp) return Point(0.0,0.0,0.0);
    return *pp;
  }
private:
  GameApi::Env &e;
  GameApi::FunctionCb<GameApi::PT, GameApi::PT> *cb;
};
#if 0
void GameApi::PolygonApi::del_cb_later(GameApi::FunctionCb<PT,PT> *cb)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(cb));
}
#endif

class ChangePoints2 : public ForwardFaceCollection
{
public:
  ChangePoints2(FaceCollection &coll, std::function<Point (Point, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual Point FacePoint(int face, int point) const 
  {
    return f(ForwardFaceCollection::FacePoint(face,point),
		face, point);
  }
private:
  std::function<Point (Point, int,int)> f;
};
struct ChangePositions_data
{
  GameApi::Env *env;
  std::function<GameApi::PT (GameApi::PT p, int face, int point)> f;
};

Point ChangePositions_Func(Point p, int face,int point, void* data)
{
  ChangePositions_data *dt = (ChangePositions_data*)data;
  GameApi::PT pt = add_point(*dt->env, p.x, p.y, p.z);
  GameApi::PT pt2 = dt->f(pt, face, point);
  Point *pp = find_point(*dt->env, pt2);
  if (!pp) return Point(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_positions(P orig, std::function<PT (PT p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
  FaceCollection *coll = find_facecoll(e, orig);
  ChangePositions_data dt;
  dt.env = &e;
  dt.f = f;
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  FaceCollection *coll2 = new ChangePoints2(*coll, std::bind(&ChangePositions_Func, _1,_2,_3,(void*)&dt));
  return add_polygon(e, coll2, 1);
#endif
}



class ChangeNormal2 : public ForwardFaceCollection
{
public:
  ChangeNormal2(FaceCollection &coll, std::function<Vector (Vector, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual Vector PointNormal(int face, int point) const 
  {
    return f(ForwardFaceCollection::PointNormal(face,point),
		face, point);
  }
private:
  std::function<Vector (Vector, int,int)> f;
};
struct ChangeNormal_data
{
  GameApi::Env *env;
  std::function<GameApi::V (GameApi::V p, int face, int point)> f;
};

Vector ChangeNormals_Func(Vector p, int face,int point,void* data)
{
  ChangeNormal_data *dt = (ChangeNormal_data*)data;
  GameApi::V pt = add_vector(*dt->env, p.dx, p.dy, p.dz);
  GameApi::V pt2 = dt->f(pt, face, point);
  Vector *pp = find_vector(*dt->env, pt2);
  if (!pp) return Vector(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_normals(P orig, std::function<V (V p, int face, int point)> f)
{
#ifndef EMSCRIPTEN

  FaceCollection *coll = find_facecoll(e, orig);
  ChangeNormal_data dt;
  dt.env = &e;
  dt.f = f;

  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;

  FaceCollection *coll2 = new ChangeNormal2(*coll, std::bind(&ChangeNormals_Func, _1, _2, _3, &dt));
  return add_polygon(e, coll2, 1);
#endif
}


class ChangeColor2 : public ForwardFaceCollection
{
public:
  ChangeColor2(FaceCollection &coll, std::function<unsigned int (unsigned int, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual unsigned int Color(int face, int point) const 
  {
    return f(ForwardFaceCollection::Color(face,point),
		face, point);
  }
private:
  //GameApi::EveryApi &ev;
  std::function<unsigned int ( unsigned int, int,int)> f;
  //void *data;
};
struct ChangeColor_data
{
  GameApi::Env *env;
  std::function<unsigned int (unsigned int p, int face, int point)> f;
  //void *data;
};

unsigned int ChangeColor_Func(unsigned int p, int face,int point,void* data)
{
  ChangeColor_data *dt = (ChangeColor_data*)data;
  return dt->f(p,face,point);
}

EXPORT GameApi::P GameApi::PolygonApi::change_colors(P orig, std::function<unsigned int (unsigned int p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeColor_data *dt = new ChangeColor_data;
  dt->env = &e;
  dt->f = f;
  //dt.data = data;

  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(dt));

  FaceCollection *coll2 = new ChangeColor2(*coll, std::bind(ChangeColor_Func, _1,_2,_3,(void*)dt));
  return add_polygon(e, coll2, 1);
#endif
}

class ChangeTexture : public ForwardFaceCollection
{
public:
  ChangeTexture(FaceCollection &coll, std::function<int (int)> f, ::Bitmap< ::Color> **array, int size)
    : ForwardFaceCollection(coll), f(f), array(array) /*, size(size)*/ { temp = new BufferFromBitmap*[size]; }
  void GenTexture(int num) 
  {
    delete temp[num];
    temp[num] = new BufferFromBitmap(*(array[num]));
    temp[num]->Gen();
  }
  virtual BufferRef TextureBuf(int num) const 
  { 
    return temp[num]->Buffer();
  }
  virtual int FaceTexture(int face) const { return f(face); }
private:
  //GameApi::EveryApi &ev;
  std::function<int (int)> f;
  //void *data;
  Bitmap< ::Color> **array;
  //int size;
  mutable BufferFromBitmap **temp;
};


EXPORT GameApi::P GameApi::PolygonApi::change_texture(P orig, std::function<int (int face)> f, BM *array, int size)
{
  FaceCollection *coll = find_facecoll(e, orig);
  std::vector<Bitmap<Color>*> *vec = new std::vector<Bitmap<Color>*>;
  for(int i=0;i<size;i++)
    {
      BitmapHandle *handle = find_bitmap(e, array[i]);
      ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
      vec->push_back(bitmap);
    }
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  ChangeTexture *tex = new ChangeTexture(*coll, f, &(*vec)[0], size);
  return add_polygon(e, tex, 1);
}



//EXPORT GameApi::P GameApi::PolygonApi::smooth_normals(P orig)
//{
//  FaceCollection *coll = find_facecoll(e, orig);
//  FaceCollection *coll2 = new SmoothNormals(*coll);
//  return add_polygon(e, coll2, 1);
//}
EXPORT GameApi::P GameApi::PolygonApi::recalculate_normals(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new RecalculateNormals(*coll);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::memoize(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::memoize_all(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  coll2->MemoizeAll();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::circular_span(EveryApi &ev, LI li,
						     float delta_angle,
						     int num_steps)
{
  M m = ev.matrix_api.yrot(delta_angle);
  return span(li, m, num_steps);
}
class Span : public FaceCollection
{
public:
  Span(LineCollection *lines, Matrix m, int num_steps) : lines(lines), m(m), num_steps(num_steps) 
  {
    Matrix mm = Matrix::Identity();
    arr.push_back(mm);
    for(int i=0;i<num_steps;i++)
      {
	mm = mm * m; 
	arr.push_back(mm);
      }
  }
  virtual int NumFaces() const { return lines->NumLines() * num_steps; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int line = face / num_steps;
    int step = face - line*num_steps;
    Matrix m1 = arr[step];
    Point p = lines->LinePoint(line, 0);
    Point py = lines->LinePoint(line, 1);
    p = p * m1;
    py = py * m1;
    Point px = p*m;
    Point pxy = py*m;
    switch(point)
      {
      case 0: return p;
      case 1: return px;
      case 2: return pxy;
      case 3: return py;
      }
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    return v / v.Dist();
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int line = face / num_steps;
    //int step = face - line*num_steps;
    unsigned int p = lines->LineColor(line, 0);
    unsigned int py = lines->LineColor(line, 1);
    switch(point)
      {
      case 0: return p;
      case 1: return p;
      case 2: return py;
      case 3: return py;
      };
    return p;

  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p00 = { 0.0, 0.0 };
    Point2d p10 = { 1.0, 0.0 };
    Point2d p11 = { 1.0, 1.0 };
    Point2d p01 = { 0.0, 1.0 };
    switch(point)
      {
      case 0: return p00;
      case 1: return p10;
      case 2: return p11;
      case 3: return p01;
      }
    return p00;
  }

private:
  LineCollection *lines;
  Matrix m;
  int num_steps;
  std::vector<Matrix> arr;
};
EXPORT GameApi::P GameApi::PolygonApi::span(LI li,
					    M matrix,
					    int num_steps)
{
  LineCollection *lines = find_line_array(e, li);
  Matrix m = find_matrix(e, matrix);
  FaceCollection *span = new Span(lines, m, num_steps);
  return add_polygon2(e, span, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::heightmap(FB bm,
					  std::function<P (float)> f, float dx, float dz)
{
  FloatBitmap *fb = find_float_bitmap(e, bm);
  int sx = fb->bitmap->SizeX();
  int sy = fb->bitmap->SizeY();
  std::vector<P> vec;
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  float val = fb->bitmap->Map(x,y);
	  P p = f(val);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::heightmap(BM bm,
					  HeightMapType t,
				       float min_x, float max_x,
				       float min_y, float max_y,
				       float min_z, float max_z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  HeightMap3DataImpl *data = new HeightMap3DataImpl(*bitmap);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(data));
  Point p(min_x, max_y, min_z);
  Vector u_x(max_x-min_x, 0.0, 0.0);
  Vector u_y(0.0, -(max_y-min_y),0.0);
  Vector u_z(0.0,0.0,max_z-min_z);

  HeightMap3 *heightmap = new HeightMap3(*data, 0.0, 1.0,
					 p, u_x, u_y, u_z);
  env->deletes.push_back(std::shared_ptr<void>(heightmap));
  MeshFaceCollection *coll = new MeshFaceCollection(*heightmap, 0);
  return add_polygon(e, coll, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::anim_array(P *array, int size)
{
  int s = size;
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<s;i++)
    {
      P *ptr = array+i;
      FaceCollection *c = find_facecoll(e, *ptr);
      //if (!c) { std::cout << "anim_array: dynamic cast failed" << std::endl; exit(0); }
      vec->push_back(c);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec->begin(), vec->end());
  handle->collarrayowned = true;
  //handle->size = size;
  return add_polygon(e, handle);
}

EXPORT void GameApi::PolygonApi::render(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "Need to call prepare() before render() call(do not put prepare() to renderloop)" << std::endl; }
  state_bm->Render(choose); 
}

StateBitmaps * PrepareFaceCollPolyHandle(FaceCollPolyHandle *h2, int bbm_choose)
{
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
    
  FaceCollection * const *array2 = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      VectorArray<FaceCollection*> *ptr = dynamic_cast<VectorArray<FaceCollection*>*>(h2->collarray);
      //std::cout << "Using collarray" << std::endl;
      array2 = ptr->get_whole_array();
      size = ptr->Size();
    }
  if (bbm_choose != -1)
    {
      array2 = &array2[bbm_choose];
      size = 1;
    }
  FaceCollection **array = const_cast<FaceCollection**>(array2);
  FaceArrayMesh *mesh = new FaceArrayMesh(array, size);
  FaceArrayMeshNormals *normal = new FaceArrayMeshNormals(array, size);
  FaceArrayMeshTexCoords *coord = new FaceArrayMeshTexCoords(array, size);
  FaceArrayMeshColors *color = new FaceArrayMeshColors(array, size);
  FaceArrayMeshTextures *textures = new FaceArrayMeshTextures(array, size, 0);

  StateBitmaps *state_bm = new StateBitmaps(mesh, normal, coord, color, textures, *textures);
  state_bm->Prepare();
  return state_bm;
}

EXPORT void GameApi::PolygonApi::prepare(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  StateBitmaps *state_bm = PrepareFaceCollPolyHandle(h2, bbm_choose);
  pp->states[p.id] = state_bm;
}

EXPORT void GameApi::PolygonApi::preparepoly(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = new ArrayRender;
  pp->rend[p.id] = r;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
  
  FaceCollection **array = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      std::cout << "Using collarray" << std::endl;
      //array = h2->collarray;
      AllocToNativeArray(*h2->collarray, &array);
    }
  if (bbm_choose != -1)
    {
      array = &array[bbm_choose];
      size = 1;
    }
  FaceArrayMesh mesh(array, size);
  FaceArrayMeshNormals normal(array, size);
  FaceArrayMeshTexCoords coord(array, size);
  FaceArrayMeshColors color(array, size);

  MeshToTriangles trimesh(mesh);
  MeshNormalsToTriangles trinormal(normal, mesh.NumPoints()==4);
  MeshTexCoordsToTriangles tricoord(coord, mesh.NumPoints()==4);
  MeshColorsToTriangles tricolor(color, mesh.NumPoints()==4);

  //std::cout << "preparepoly:" <<  h2->size << std::endl;
  //std::cout << mesh.NumFaces(0) << " " << mesh.NumPoints() << std::endl;
  r->Alloc(trimesh.NumFaces(0), trimesh.NumFaces(0)*trimesh.NumPoints(), size, size, size, size);
  r->InsertAll(trimesh, trinormal, tricoord, tricolor);
#if 0
  int texcount = array[0]->NumTextures();
  if (texcount)
    {
      r->AllocTexture(texcount);
      MeshTextures *texture = 0;
      for(int i=0;i<texcount;i++)
	{
	  array[0]->GenTexture(i);
	  BufferRef buf = array[0]->TextureBuf(i);
	  BitmapFromBuffer *bitmap = new BitmapFromBuffer(buf);
	  if (texture)
	    {
	      texture = new MeshTexturesImpl(*bitmap, *texture);
	    }
	  else
	    {
	      texture = new MeshTexturesImpl(*bitmap);
	    }
	}
      r->UpdateTexture(*texture, 0);
    }
#endif
  //for(int i=0;i<h2->size;i++)
  //  {
  //    std::pair<int,int> p = r->InsertMesh(mesh, i);
  //  }
}
void GameApi::PolygonApi::renderpoly(P p, PT pos)
{
  Point *pp = find_point(e, pos);
  renderpoly(p, 0, pp->x, pp->y, pp->z);
}
void GameApi::PolygonApi::renderpoly(P p, float x, float y, float z)
{
  renderpoly(p, 0, x,y,z);
}
void GameApi::PolygonApi::renderpoly(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = pp->rend[p.id];
  if (!r) { std::cout << "To use renderpoly() you should first call preparepoly(do not put it to frame loop)" << std::endl; return; }
  glPushMatrix();
#ifndef EMSCRIPTEN
  glTranslatef(x,y,z);
#endif
  //std::cout << "renderpoly: " << r->used_vertex_count << std::endl;

  //PolyHandle *handle = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
#if 0
  if (r->TextureCount())
    {
      r->EnableTexture(0);
    }
#endif
  r->Render(choose, choose, choose, choose, 0, r->used_vertex_count[choose]);
#if 0
  if (r->TextureCount())
    {
      r->DisableTexture();
    }
#endif
  glPopMatrix();
}

class CountsFaceCollection : public ForwardFaceCollection
{
public:
  CountsFaceCollection(int numfaces, FaceCollection *next) : ForwardFaceCollection(*next), numfaces(numfaces) { }
  int NumFaces() const { return numfaces; }
private:
  int numfaces;
};

class CountFuncFaceCollection : public ForwardFaceCollection
{
public:
  CountFuncFaceCollection(FaceCollection *next, std::function<int (int face)> f) : ForwardFaceCollection(*next),  f(f) { }
  virtual int NumPoints(int face) const
  {
    return f(face);
  }
  
private:
  //GameApi::EveryApi &ev;
  std::function<int (int face)> f; 
  //void *data;
};

class PointFaceCollection : public ForwardFaceCollection
{
public:
  PointFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Point FacePoint(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};


class NormalFaceCollection : public ForwardFaceCollection
{
public:
  NormalFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::V (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Vector PointNormal(int face, int point) const
  {
    GameApi::V pp = f(face, point);
    Vector *p = find_vector(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::V (int face, int point)> f;
  //void *data;
};


class ColorFaceCollection : public ForwardFaceCollection
{
public:
  ColorFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<unsigned int (int face, int point)> f) : ForwardFaceCollection(*next), /*e(e),*/  f(f) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int pp = f(face, point);
    return pp;
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<unsigned int (int face, int point)> f;
  void *data;
};


class TexFaceCollection : public ForwardFaceCollection
{
public:
  TexFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Point2d TexCoord(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    Point2d px = { p->x, p->y };
    return px;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};

class AttribFaceCollection : public ForwardFaceCollection
{
public:
  AttribFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<float (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/ f(f), idx(idx) { }
  virtual float Attrib(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::Attrib(face,point,id);
  }

private:
  std::function<float (int face, int point, int id)> f;
  int idx;
};

class AttribIFaceCollection : public ForwardFaceCollection
{
public:
  AttribIFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<int (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/  f(f), idx(idx) { }
  virtual int AttribI(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::AttribI(face,point,id);
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<int (int face, int point, int id)> f;
  int idx;
};



EXPORT GameApi::P GameApi::PolygonApi::counts(P p1, int numfaces)
{
  FaceCollection *poly = find_facecoll(e, p1);
  return add_polygon(e, new CountsFaceCollection(numfaces, poly),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::count_function(P p1, std::function<int (int face)> f)
{ 
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new CountFuncFaceCollection(poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::point_function(P p1, std::function<PT (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new PointFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::normal_function(P p1, std::function<V (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new NormalFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::color_function(P p1, std::function<unsigned int (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new ColorFaceCollection(e, poly,f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_function(P p1, std::function<PT (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new TexFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::attrib_function(P p1, std::function<float (int face, int point, int idx)> f, int idx)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribFaceCollection(e, poly, f, idx),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::attribi_function(P p1, std::function<int (int face, int point, int idx)> f, int idx)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribIFaceCollection(e, poly, f, idx),1);  
}

EXPORT GameApi::P GameApi::PolygonApi::create_static_geometry(GameApi::P *array, int size)
{
  if (size==0) { std::cout << "Empty array in remove_changing" << std::endl; }
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(find_facecoll(e,array[i]));
    }
  IsChangingFace *func = new IsChangingFace(vec);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(func));
  FilterFaces *coll2 = new FilterFaces(*(vec[0]), *func);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::create_dynamic_geometry(GameApi::P *array, int size)
{
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      FaceCollection *coll = find_facecoll(e,array[i]);
      vec.push_back(coll);
    }
  IsChangingFace *ch = new IsChangingFace(vec);
  for(int i=0;i<size;i++)
    {
      vec[i] = new FilterFaces(*(vec[i]), *ch);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec.begin(), vec.end());
  handle->collarrayowned = true;

  return add_polygon(e, handle);
}
EXPORT GameApi::P GameApi::PolygonApi::tri_vertex_array(float *v_array, int v_size,
						 float *n_array, int n_size,
						 unsigned int *c_array, int c_size,
						 float *tex_array, int tex_size,
						 float **attrib_array, int a_size1, int a_size2)
{
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = new VertexArrayFaceCollection(v_array, v_size,
					       n_array, n_size,
					       c_array, c_size,
					       tex_array, tex_size,
					       attrib_array, a_size1, a_size2);
  handle->collowned = true;
  handle->collarray = NULL;
  handle->collarrayowned = false;
  return add_polygon(e, handle);
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_frames(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_frames" << std::endl; }

  FaceCollPolyHandle *h2 = find_poly(e,p);
  if (!h2->collarray) { return 1; }
  return h2->collarray->Size();
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_rows(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_rows" << std::endl; }
  return state_bm->bitmap->SizeY();
}

EXPORT void GameApi::PolygonApi::get_tri_vertex_array(P p, int choose, int row,
					       int *v_size, float **v_array,
					       int *n_size, float **n_array,
					       int *c_size, unsigned int **c_array,
					       int *tex_size, float **tex_array,
					       int *attrib_size1, int *attrib_size2, float ***attrib_array)
{

  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array" << std::endl; }
  StateRow &r = state_bm->bitmap->DynRow(row);
  int vertex_pos = 0;
  int vertex_size = r.rend->used_vertex_count[choose];
  int v_choose = choose;
  int n_choose = choose;
  int c_choose = choose;
  int t_choose = choose;
  ArrayRender *rend = state_bm->GetRender(row);
  //Matrix m = state_bm->GetMatrix(choose);
  int v_offset3 = vertex_pos*3+v_choose*rend->vertex_array_size*3;
  int n_offset3 = vertex_pos*3+n_choose*rend->vertex_array_size*3;
  int c_offset =  vertex_pos+c_choose*rend->vertex_array_size;
  int tex_offset2 = vertex_pos*2+t_choose*rend->vertex_array_size*2;
  *v_array = rend->vertex_array+v_offset3;
  *n_array = rend->normal_array+n_offset3;
  *c_array = (unsigned int*)rend->color_array+c_offset;
  *tex_array = rend->tex_coord_array+tex_offset2;
  
  *v_size = vertex_size*3;
  *n_size = vertex_size*3;
  *c_size = vertex_size;
  *tex_size = vertex_size*2;
}

class TriStripFaceCollection : public FaceCollection
{
public:
  TriStripFaceCollection(std::vector<Point> vec) : vec(vec) { }
  virtual int NumFaces() const { return vec.size()-2; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[face+point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    return v / v.Dist();
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
private:
  std::vector<Point> vec;
};
EXPORT GameApi::P GameApi::PolygonApi::tri_strip(PT *array, int size)
{
  std::vector<Point> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(*find_point(e, array[i]));
    }
  return add_polygon2(e, new TriStripFaceCollection(vec),1);
  
}
EXPORT GameApi::P GameApi::PolygonApi::polygon(PT *array, int size)
{
  PolygonElem *coll = new PolygonElem;
  int sz = size;
  for(int i=0;i<sz;i++)
    {
      PT p = array[i];
      Point *pp = find_point(e,p);
      coll->push_back(*pp);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = coll;
  handle->collowned = false;
  handle->collarray = NULL; 
  handle->collarrayowned = false;

  return add_polygon(e, handle);
  
}

EXPORT void GameApi::PolygonApi::render_dynamic(GameApi::P p, int array_elem, bool textures)
{
  FaceCollPolyHandle *handle = find_poly(e,p);
  FaceCollection *coll = handle->collarray->Index(array_elem);
  if (textures)
    {
      RenderOpenGlTextures(*coll);
    }
  else
    {
      RenderOpenGl(*coll);
    }
}


class ColorVoxelFaceCollection : public ForwardFaceCollection
{
public:
  ColorVoxelFaceCollection(FaceCollection &coll, Voxel<unsigned int> &c, Point p, Vector v_x, Vector v_y, Vector v_z) : ForwardFaceCollection(coll), c(c), pp(p) { cc.center = v_x; cc.u_x = v_x; cc.u_y = v_y; cc.u_z = v_z; }
  virtual unsigned int Color(int face, int point) const { 
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p2 = cc.FindInternalCoords(p);
    return c.Map(p2.x,p2.y,p2.z);
  }
private:
  Voxel<unsigned int> &c;
  mutable Coords cc;
  Point pp;
};

EXPORT GameApi::P GameApi::PolygonApi::color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z)
{
  Point *pp = find_point(e, p);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);

  FaceCollection *coll = find_facecoll(e, orig);
  Voxel<unsigned int> *v = find_voxel(e, colours);
  return add_polygon(e, new ColorVoxelFaceCollection(*coll, *v, *pp, *uu_x, *uu_y, *uu_z), 1);
}
class UpdateVA : public MainLoopItem
{
public:
  UpdateVA(GameApi::PolygonApi &api, GameApi::VA va, GameApi::P p, bool keep) : api(api), va(va), p(p), keep(keep) { }
  void execute(MainLoopEnv &e)
  {
    api.update_vertex_array(va,p,keep);
  }
  int shader_id() { return -1; }
private:
  GameApi::PolygonApi &api;
  GameApi::VA va;
  GameApi::P p;
  bool keep;
};
EXPORT GameApi::ML GameApi::PolygonApi::update_vertex_array_ml(GameApi::VA va, GameApi::P p, bool keep)
{
  return add_main_loop(e, new UpdateVA(*this, va, p, keep));
}
EXPORT void GameApi::PolygonApi::update_vertex_array(GameApi::VA va, GameApi::P p, bool keep)
{
#ifdef THREADS
  int num_threads = 1;
  FaceCollection *faces = find_facecoll(e, p);
  ThreadedPrepare prep(faces);
  int s = faces->NumFaces();
  if (s<100) { num_threads = 1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "Numthreads: " << num_threads << std::endl;
  for(int i=0;i<num_threads;i++)
    {
      int start_range = i*delta_s;
      int end_range = (i+1)*delta_s;
      if (end_range>s) end_range = s;
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread(start_range, end_range));
    }
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
    }
  VertexArraySet *set = prep.collect();
  RenderVertexArray *arr2 = new RenderVertexArray(*set);
  arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
    }
  add_update_vertex_array(e, va, set, arr2);
#else
  FaceCollection *faces = find_facecoll(e, p);
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces());  
  RenderVertexArray *arr2 = new RenderVertexArray(*s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  add_update_vertex_array(e, va, s, arr2);
#endif

}
EXPORT void GameApi::PolygonApi::explode(VA va, PT pos, float dist)
{
  VertexArraySet *s = find_vertex_array(e, va);
  Point *pt = find_point(e, pos);
  s->explode(0, *pt, dist);
}
 
EXPORT GameApi::VA GameApi::PolygonApi::create_vertex_array(GameApi::P p, bool keep)
{ 
#ifdef THREADS
  int num_threads = 4;
  FaceCollection *faces = find_facecoll(e, p);
  //std::cout << "FaceColl: " << faces << " " << faces->NumFaces() << std::endl; 
  ThreadedPrepare prep(faces);  
  int s = faces->NumFaces();   
  //std::cout << "NumFaces: " << s << std::endl;
  if (s<100) { num_threads=1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "NumThreads2: " << num_threads << std::endl;
  for(int i=0;i<num_threads;i++)
    {  
      int start_range = i*delta_s; 
      int  end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; } 
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread(start_range, end_range));
    }
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
    }
  VertexArraySet *set = prep.collect();
  RenderVertexArray *arr2 = new RenderVertexArray(*set);
  arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
    }
  return add_vertex_array(e, set, arr2);
#else
  FaceCollection *faces = find_facecoll(e, p);
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces());  
  RenderVertexArray *arr2 = new RenderVertexArray(*s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  return add_vertex_array(e, s, arr2);
#endif
}
#if 0
EXPORT int GameApi::PolygonApi::access_point_count(VA va, bool triangle)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      return s->tri_count(0);
    }
  else
    {
      return s->quad_count(0);
    }
}
EXPORT float *GameApi::PolygonApi::access_points(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point *p = s->tri_polys(0);
      return &((float*)p)[face*3*3+point*3];
    }
  else
    {
      const Point *p = s->quad_polys(0);
      return &((float*)p)[face*4*3+point*3];
    }
  return 0;
}
EXPORT float *GameApi::PolygonApi::access_color(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle) {
    const float *ptr = s->tri_color_polys(0);
    return &((float*)ptr)[face*3*4+point*4];
  }
  else
    {
    const float *ptr = s->quad_color_polys(0);
    return &((float*)ptr)[face*4*4+point*4];      
    }
}
EXPORT float *GameApi::PolygonApi::access_normals(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Vector *ptr = s->tri_normal_polys(0);
      return &((float*)ptr)[face*3*3+point*3];
    }
  else
    {
      const Vector *ptr = s->quad_normal_polys(0);
      return &((float*)ptr)[face*4*3+point*3];
    }
}
EXPORT float *GameApi::PolygonApi::access_texcoord(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point2d *ptr = s->tri_texcoord_polys(0);
      return &((float*)ptr)[face*3*2+point*2];
    }
  else
    {
      const Point2d *ptr = s->quad_texcoord_polys(0);
      return &((float*)ptr)[face*4*2+point*2];
    }
}
#endif
class ChooseTex : public ForwardFaceCollection
{
public:
  ChooseTex(FaceCollection *coll, int val) : ForwardFaceCollection(*coll),val(val) { }
  virtual float TexCoord3(int face, int point) const { 
    return float(val);
  }
private:
  int val;
};
class DefaultTex : public ForwardFaceCollection
{
public:
  DefaultTex(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  Point2d TexCoord(int face, int point) const
  {
    if (point==0) {
      Point2d p0 = { 0.0, 0.0 }; return p0; }
    if (point==1) {
      Point2d p1 = { 1.0, 0.0 }; return p1; }
    if (point==2) {
      Point2d p2 = { 1.0, 1.0 }; return p2; }
    if (point==3) {
      Point2d p3 = { 0.0, 1.0 }; return p3; }
    Point2d p4;
    return p4;
    
  }
};
EXPORT GameApi::P GameApi::PolygonApi::texcoord_default(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new DefaultTex(coll), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::choose_texture(P orig, int num)
{
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new ChooseTex(coll, num), 1);
}
EXPORT void GameApi::PolygonApi::update(VA va)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *s2 = find_vertex_array_render(e, va);
  s2->update(0);
}
class RenderVA : public MainLoopItem
{
public:
  RenderVA(GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::VA va) : ev(ev), api(api), va(va) 
  {
    shader.id = -1;
  }
  int shader_id() { return shader.id; }
  void execute(MainLoopEnv &e)
  { 

    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }
    if (shader.id==-1 && e.vertex_shader!="" && e.fragment_shader!="")
      {
	shader = ev.shader_api.get_normal_shader("comb", "comb", "", e.vertex_shader, e.fragment_shader);
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
  }
private:
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  GameApi::VA va;
  GameApi::SH shader;
};
class NoiseShaderML : public MainLoopItem
{
public:
  NoiseShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="snoise";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="snoise";
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
};

class LightShaderML : public MainLoopItem
{
public:
  LightShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="light";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="light";
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
};

class ToonShaderML : public MainLoopItem
{
public:
  ToonShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="toon";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="toon";
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
};


class ShaderParamML : public MainLoopItem
{
public:
  ShaderParamML(GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir,
		unsigned int level1, unsigned int level2, unsigned int level3) : ev(ev), next(next), light_dir(light_dir), level1(level1), level2(level2), level3(level3) 
  { 
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:passall", "colour:ambient:diffuse:specular:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true);
  }
  int shader_id() { return next->shader_id(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="passall";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="ambient:diffuse:specular";


    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
	ev.shader_api.set_var(sh, "level1_color",
			      ((level1&0xff0000)>>16)/255.0,
			      ((level1&0xff00)>>8)/255.0,
			      ((level1&0xff))/255.0,
			      ((level1&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level2_color",
			      ((level2&0xff0000)>>16)/255.0,
			  ((level2&0xff00)>>8)/255.0,
			      ((level2&0xff))/255.0,
			  ((level2&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level3_color",
			      ((level3&0xff0000)>>16)/255.0,
			      ((level3&0xff00)>>8)/255.0,
			      ((level3&0xff))/255.0,
			      ((level3&0xff000000)>>24)/255.0);
      }
    next->execute(ee);
  }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  unsigned int level1, level2, level3;
  GameApi::SH sh;
};
EXPORT GameApi::ML GameApi::PolygonApi::noise_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NoiseShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::light_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new LightShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::toon_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ToonShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::shading_shader(EveryApi &ev, ML mainloop,
						      unsigned int level1,
						      unsigned int level2,
						      unsigned int level3)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  Vector light_dir(1.0,1.0,1.0);
  return add_main_loop(e, new ShaderParamML(ev, item, light_dir, level1, level2, level3));
}
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml(EveryApi &ev, VA va)
{
  return add_main_loop(e, new RenderVA(ev, *this, va));
}
EXPORT void GameApi::PolygonApi::print_stat(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  s->print_stat(0);
}
EXPORT bool GameApi::PolygonApi::is_texture(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  return s->is_texture();
}
EXPORT bool GameApi::PolygonApi::is_array_texture(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  return s->is_array_texture();
}
EXPORT void GameApi::PolygonApi::render_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render(0);
      glDisable(GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
}
EXPORT void GameApi::PolygonApi::prepare_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);
  rend->prepare_instanced(0, (Point*)arr->array, arr->numpoints);
}
EXPORT void GameApi::PolygonApi::render_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh)
{
#if 0
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);

  
  int ss = arr->numpoints;
  for(int i=0;i<ss;i++)
    {
      float x = arr->array[i*3];
      float y = arr->array[i*3+1];
      float z = arr->array[i*3+2];
      shapi.set_var(sh, "in_InstPos", x,y,z);

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render(0);
      glDisable(GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
    }
#else
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render_instanced(0, (Point*)arr->array, arr->numpoints);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      glEnable(GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render_instanced(0, (Point*)arr->array, arr->numpoints);
      //rend->render(0);

      glDisable(GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      glEnable(GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      glClientActiveTexture(GL_TEXTURE0+0);
#endif
      glActiveTexture(GL_TEXTURE0+0);
      glBindTexture(GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render_instanced(0, (Point*)arr->array, arr->numpoints);
      glDisable(GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render_instanced(0, (Point*)arr->array, arr->numpoints);
      //rend->render(0);
    }
#endif
}


class AnimFace : public ForwardFaceCollection
{
public:
  AnimFace(FaceCollection &coll, Vector v) : ForwardFaceCollection(coll), /*coll(coll),*/ v(v) { }
  Point EndFacePoint(int face, int point) const 
  { return ForwardFaceCollection::FacePoint(face, point) + v; 
  }

private:
  //FaceCollection &coll;
  Vector v;
};

EXPORT GameApi::P GameApi::PolygonApi::anim_target_vector(P p, V v)
{
  FaceCollection *i = find_facecoll(e, p);
  Vector *vv = find_vector(e,v);
  FaceCollection *coll = new AnimFace(*i, *vv);
  return add_polygon(e, coll, 1);
}
class AnimColl : public ForwardFaceCollection
{
public:
  AnimColl(FaceCollection *i1, FaceCollection *i2) : ForwardFaceCollection(*i1), i1(i1), i2(i2) { }
  virtual Point EndFacePoint(int face, int point) const { return i2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return i2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return i2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return i2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return i2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return i2->TexCoord(face,point); }
private:
  FaceCollection *i1;
  FaceCollection *i2;
};
EXPORT GameApi::P GameApi::PolygonApi::anim_endpoints(P p1, P p2)
{
  FaceCollection *i1 = find_facecoll(e,p1);
  FaceCollection *i2 = find_facecoll(e,p2);
  FaceCollection *coll = new AnimColl(i1, i2);
  return add_polygon(e, coll, 1);
}
class AnimInterpolate : public FaceCollection
{
public:
  AnimInterpolate(FaceCollection *coll, float val) : coll(coll), val(val) { }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const { return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = coll->FacePoint(face, point);
    Point p2 = coll->EndFacePoint(face, point);
    return Point::Interpolate(p1, p2, val);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v1 = coll->PointNormal(face,point);
    Vector v2 = coll->EndPointNormal(face, point);
    return Vector(Point::Interpolate(Point(v1),Point(v2), val));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    float val1 = coll->Attrib(face,point,id);
    float val2 = coll->EndAttrib(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    int val1 = coll->AttribI(face,point,id);
    int val2 = coll->EndAttribI(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int c1 = coll->Color(face,point);
    unsigned int c2 = coll->EndColor(face,point);
    return Color::CubicInterpolate(c1,c2, val);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p1 = coll->TexCoord(face,point);
    Point2d p2 = coll->EndTexCoord(face,point);
    Point2d pos = { float(p1.x*val+p2.x*(1.0-val)), float(p1.y*val+p2.y*(1.0-val)) };
    return pos;
  }
private:
  FaceCollection *coll;
  float val;
};
EXPORT GameApi::P GameApi::PolygonApi::anim_interpolate(P p, float val)
{
  FaceCollection *i = find_facecoll(e,p);
  FaceCollection *coll = new AnimInterpolate(i, val);
  return add_polygon(e, coll, 1);
}

class AnimFaceScale : public ForwardFaceCollection
{
public:
  AnimFaceScale(FaceCollection &coll, Point p, float scale_x, float scale_y, float scale_z) : ForwardFaceCollection(coll), /*coll(coll),*/ p(p), scale_x(scale_x), scale_y(scale_y), scale_z(scale_z) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * Matrix::Translate(-p.x,-p.y,-p.z)
      * Matrix::Scale(scale_x, scale_y, scale_z)
      * Matrix::Translate(p.x,p.y,p.z);
  }

private:
  //FaceCollection &coll;
  Point p;
  float scale_x, scale_y, scale_z;
};


EXPORT GameApi::P GameApi::PolygonApi::anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z)
{
  FaceCollection *i = find_facecoll(e, p);
  Point *pp = find_point(e, center);
  FaceCollection *coll = new AnimFaceScale(*i, *pp, scale_x, scale_y, scale_z);
  return add_polygon(e, coll, 1);
}
class CutFaces : public FaceCollection
{
public:
  CutFaces(FaceCollection *i, VolumeObject *oo, Cutter *cut) : i(i), oo(oo), cut(cut) { cut_them(); compress(); }

  void cut_them()
  {
    int f = i->NumFaces();
    for(int ii=0;ii<f; ii++)
      {
	faces.push_back(std::vector<Point>());
	int p = i->NumPoints(ii);
	//std::cout << p << std::endl;
	std::vector<Point> &ref = faces[faces.size()-1];
	for(int jj=0;jj<p;jj++)
	  {
	    int jjj1 = jj;
	    int jjj2 = (jjj1+1)%p;
	    Point p1 = i->FacePoint(ii, jjj1);
	    Point p2 = i->FacePoint(ii, jjj2);
	    bool b1 = oo->Inside(p1);
	    bool b2 = oo->Inside(p2);
	    if (b1!=b2)
	      {

		//std::cout << "CUTTER: " << p1 << " " << p2 << " ";
		Point c1 = cut->cut(p1,p2);
		//std::cout << c1 << std::endl;
		if (!b1)
		  {
		    ref.push_back(p1);
		    ref.push_back(c1);
		  }
		if (!b2)
		  {
		    ref.push_back(c1);
		    ref.push_back(p2);
		  }
	      }
	    else
	      {
		if (!b1) {
		  ref.push_back(p1);
		  ref.push_back(p2);
		}
	      }
	  }
      }
  }
  void compress()
  {
    int f = faces.size();
    for(int ii=0;ii<f; ii++)
      {
	int p = faces[ii].size();
	if (p==0 ||p==1 ||p==2) continue;
	faces2.push_back(faces[ii]);
      }
  }

  virtual int NumFaces() const { return faces2.size(); }
  virtual int NumPoints(int face) const { return faces2[face].size(); }
  virtual Point FacePoint(int face, int point) const
  {
    return faces2[face][point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0, 0.0, 0.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }


private:
  FaceCollection *i;
  VolumeObject *oo;
  Cutter *cut;
  std::vector<std::vector<Point> > faces;
  std::vector<std::vector<Point> > faces2;
};


#if 0
class AnimFaceMatrix : public ForwardFaceCollection
{
public:
  AnimFaceMatrix(FaceCollection &coll, Matrix m) : ForwardFaceCollection(coll), coll(coll), m(m) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * m;
  }

private:
  FaceCollection &coll;
  Matrix m;
};

EXPORT GameApi::P GameApi::PolygonApi::anim_target_matrix(P p, M matrix)
{
  FaceCollection *i = find_facecoll(e, p);
  Matrix *mm = find_matrix(e,matrix);
  FaceCollection *coll = new AnimFaceMatrix(*i, *mm);
  
  return add_polygon(e, coll, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p1, PT p2, PT p3, PT p4)
{
}
EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p, float mul_x, float mul_y)
{
}
#endif


EXPORT GameApi::P GameApi::PolygonApi::cut_faces(P p, O o, CT cutter)
{
  FaceCollection *i = find_facecoll(e, p);
  VolumeObject *oo = find_volume(e, o);
  Cutter *cut = find_cutter(e, cutter);
  FaceCollection *coll = new CutFaces(i, oo, cut);
  return add_polygon(e, coll, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::intersect(EveryApi &ev, P p1, P p2,
						 O o1, O o2,
						 CT cutter1, CT cutter2)
{
  O o1_n = ev.volume_api.not_op(o1);
  O o2_n = ev.volume_api.not_op(o2);

  P p1_cut = cut_faces(p1, o2_n, cutter2);
  TS ts_1 = ev.ts_api.from_poly(p1_cut);
  P p_1 = ev.ts_api.to_poly(ts_1);

  P p2_cut = cut_faces(p2, o1_n, cutter1);
  TS ts_2 = ev.ts_api.from_poly(p2_cut);
  P p_2_ = ev.ts_api.to_poly(ts_2);

  P or_1 = or_elem(p_1, p_2_);
  return or_1;

}
class TriToQuad : public FaceCollection
{
public:
  TriToQuad(FaceCollection *coll) : coll(coll) { }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (coll->NumPoints(face)==3)
      {
	if (point==3) { return coll->FacePoint(face,2)+Vector(1.0,0.0,0.0); }
      }
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (point==3) point=2;
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const {
    if (point==3) point=2;
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (point==3) point=2;
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (point==3) point=2;
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (point==3) point=2;
    return coll->TexCoord(face,point);
  }

private:
  FaceCollection *coll;
};
EXPORT GameApi::P GameApi::PolygonApi::tri_to_quad(P p)
{
  FaceCollection *poly = find_facecoll(e, p);
  return add_polygon2(e, new TriToQuad(poly),1);
}

class ColorMapPoly : public FaceCollection
{
public:
  ColorMapPoly(Bitmap<::Color> *bm, Point pos, Vector u_x, Vector u_y) : bm(bm), pos(pos), u_x(u_x), u_y(u_y) { }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    //p.x = xx*sx/bm->SizeX();
    //p.y = yy*sy/bm->SizeY();
    //p.z = z;
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }
private:
  Bitmap<::Color> *bm;
  Point pos;
  Vector u_x, u_y;
};

class ColorMapPoly2 : public FaceCollection
{
public:
  ColorMapPoly2(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm),fb(fb), pos(pos), u_x(u_x), u_y(u_y) { u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    p+=fb->Map(xx*fb->SizeX()/bm->SizeX(),yy*fb->SizeY()/bm->SizeY())*u_z;
    //p.x = xx*sx/bm->SizeX();
    //p.y = yy*sy/bm->SizeY();
    //p.z = z;
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }
private:
  Bitmap<::Color> *bm;
  Bitmap<float> *fb;
  Point pos;
  Vector u_x, u_y;
  Vector u_z;
};

 
EXPORT GameApi::P GameApi::PolygonApi::color_map(BM bm, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly(bm1, pos, u_x, u_y), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map2(BM bm, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  return add_polygon2(e, new ColorMapPoly(bm1, *pos1, *u_x1, *u_y1), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2(bm1, fb, *pos1, *u_x1, *u_y1), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2(bm1, fb, pos, u_x, u_y), 1);
}

#if 0
EXPORT GameApi::P GameApi::PolygonApi::cube_map(EveryApi &ev, float start_x, float end_x,
						float start_y, float end_y,
						float start_z, float end_z,
						BM bm_front, BM bm_back,
						BM bm_left, BM bm_right,
						BM bm_top, BM bm_bottom)
{
  PT pos_1 = ev.point_api.point(start_x, start_y, start_z);
  V u_x1 = ev.vector_api.vector(end_x-start_x, 0.0, 0.0);
  V u_y1 = ev.vector_api.vector(0.0, end_y-start_y, 0.0);
  P cube1 = color_map2(bm_front, pos_1, u_x1, u_y1);

  


}
#endif
EXPORT GameApi::P GameApi::PolygonApi::alt(std::vector<P> v, int index)
{
  if (v.size()==0) return empty();
  int s = v.size();
  if (index<0 ||index>=s) return empty();
  return v[index];
}
class SubPolyChange : public FaceCollection
{
public:
  SubPolyChange(FaceCollection *coll1, FaceCollection *coll2, VolumeObject *vol)
    : coll1(coll1), coll2(coll2), vol(vol) 
  {
    int s = coll1->NumFaces();
    for(int i=0;i<s;i++)
      {
	if (side(true, i)) { choose.push_back(0); faces.push_back(i); }
      }
    int s2 = coll2->NumFaces();
    for(int i2=0;i2<s2;i2++)
      {
	if (side(false, i2)) { choose.push_back(1); faces.push_back(i2); }
      }
  }
  bool side(bool which, int face) const
  {
    FaceCollection *coll = 0;
    if (which)
      {
	coll = coll1;
      }
    else
      {
	coll = coll2;
      }
    int s = coll->NumPoints(face);
    Point p(0.0,0.0,0.0);
    for(int i=0;i<s;i++)
      {
	p+=Vector(coll->FacePoint(face,i));
      }
    Vector v = p;
    v/=float(s);
    Point pp = v;
    bool b = vol->Inside(pp);
    if (which) b = !b;
    return b;
  }

  virtual int NumFaces() const { return choose.size(); }
  virtual int NumPoints(int face) const {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->NumPoints(faces[face]);
  }
  virtual Point FacePoint(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->FacePoint(faces[face], point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->PointNormal(faces[face], point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->Color(faces[face], point);

  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->TexCoord(faces[face], point);
  }
private:
  FaceCollection *coll1;
  FaceCollection *coll2;
  VolumeObject *vol;
  std::vector<int> choose;
  std::vector<int> faces;
};
EXPORT GameApi::P GameApi::PolygonApi::subpoly_change(P p, P p2, O choose)
{
  FaceCollection *pa = find_facecoll(e, p);
  FaceCollection *pa2 = find_facecoll(e, p2);
  VolumeObject *choose_a = find_volume(e, choose);

  return add_polygon2(e, new SubPolyChange(pa, pa2, choose_a),1);
}
class TexCoordSpherical2 : public ForwardFaceCollection
{
public:
  TexCoordSpherical2(GameApi::Env &e, GameApi::EveryApi &ev, Point center, float r, FaceCollection *coll) : ForwardFaceCollection(*coll), e(e), ev(ev), center(center), r(r), coll(coll) 
  {
    GameApi::PT center2 = ev.point_api.point(center.x, center.y, center.z);
    fd = ev.dist_api.sphere(center2, r);
  }
  Point2d TexCoord(int face, int point) const
  {
    Point p = coll->FacePoint(face, point);
    Vector n = coll->PointNormal(face,point);
    GameApi::PT p2 = ev.point_api.point(p.x, p.y, p.z);
    GameApi::V v2 = ev.vector_api.vector(n.dx, n.dy, n.dz);
    GameApi::PT ip = ev.dist_api.ray_shape_intersect(fd, p2, v2);
    GameApi::PT sph = ev.point_api.spherical_coords(ip);
    float alfa = ev.point_api.pt_x(sph);
    float beta = ev.point_api.pt_y(sph);
    Point2d pp;
    pp.x = alfa/3.14159;
    pp.y = beta/3.14159/2.0;
    return pp;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Point center;
  float r;
  FaceCollection *coll;
  GameApi::FD fd;
};
EXPORT GameApi::P GameApi::PolygonApi::texcoord_spherical2(EveryApi &ev,
							  PT center,
							  float r,
							  P orig)
{
  FaceCollection *pa = find_facecoll(e, orig);
  Point *pt = find_point(e, center);
  return add_polygon2(e, new TexCoordSpherical2(e, ev, *pt, r, pa), 1);

}
class ColorFromTexcoord : public ForwardFaceCollection
{
public:
  ColorFromTexcoord(FaceCollection *coll, unsigned int color_tl, unsigned int color_tr,
		    unsigned int color_bl, unsigned int color_br)
    : ForwardFaceCollection(*coll), color_tl(color_tl), color_tr(color_tr),
      color_bl(color_bl), color_br(color_br) 
  {
  }
  unsigned int Color(int face, int point) const
  {
    Point2d pos = ForwardFaceCollection::TexCoord(face,point);
    unsigned int c1 = Color::Interpolate(color_tl, color_tr, pos.x);
    unsigned int c2 = Color::Interpolate(color_bl, color_br, pos.x);
    unsigned int c3 = Color::Interpolate(c1,c2, pos.y);
    //std::cout << "ColorFromTexCoord: " << pos.x << " " << pos.y << ":" << std::hex << c1 << " " << c2 << " " << c3 << std::dec << std::endl;
    return c3;
  }
private:
  unsigned int color_tl, color_tr;
  unsigned int color_bl, color_br;
};
EXPORT GameApi::P GameApi::PolygonApi::color_from_texcoord(P orig,
							   unsigned int color_tl, unsigned int color_tr,
							   unsigned int color_bl, unsigned int color_br)
{
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new ColorFromTexcoord(coll, color_tl, color_tr, color_bl, color_br), 1);  
}
class DeformFaceCollection : public ForwardFaceCollection
{
public:
  DeformFaceCollection(FaceCollection *obj, VolumeObject *bools, Vector v) : ForwardFaceCollection(*obj), obj(obj), bools(bools), v(v) {}
  Point FacePoint(int face, int point) const
  {
    Point p = obj->FacePoint(face,point);
    bool b = bools->Inside(p);
    if (b) { p+=v; }
    return p;
  }
private:
  FaceCollection *obj;
  VolumeObject *bools;
  Vector v;
};
EXPORT GameApi::P GameApi::PolygonApi::deform(P obj, O bools, float dx, float dy, float dz)
{
  FaceCollection *obj_ = find_facecoll(e,obj);
  VolumeObject *bools_ = find_volume(e,bools);
  return add_polygon2(e, new DeformFaceCollection(obj_, bools_, Vector(dx,dy,dz)),1);
}
EXPORT GameApi::P GameApi::PolygonApi::and_not_elem(EveryApi &ev, P p1, P p_not,
						    O o1, O o_not,
						    CT cutter1, CT cutter_not)
{
  O o1_n = o1; //ev.volume_api.not_op(o1);
  O onot_n = ev.volume_api.not_op(o_not);

  P p1_cut = cut_faces(p1, onot_n, cutter_not);
  TS ts_1 = ev.ts_api.from_poly(p1_cut);
  P p_1 = ev.ts_api.to_poly(ts_1);

  P p_not_cut = cut_faces(p_not, o1_n, cutter1);
  TS ts_not = ev.ts_api.from_poly(p_not_cut);
  P p_not_ = ev.ts_api.to_poly(ts_not);


  P or_1 = or_elem(p_1, p_not_);
  return or_1;
  //return or_1;
}
GameApi::BM GameApi::PolygonApi::renderpolytobitmap(EveryApi &ev, P p, SH sh, float x, float y, float z, int sx, int sy)
{
  GameApi::PolygonObj obj(ev, p, sh);
  obj.set_pos(x,y,z);
  obj.prepare();
  int screen_width = ev.mainloop_api.get_screen_width();
  int screen_height = ev.mainloop_api.get_screen_height();
  //Point2d pos = { 0.0, 0.0 };
  //Vector2d sz = { float(sx), float(sy) };
  //glViewport(pos.x, screen_height-pos.y-sz.dy, sz.dx, sz.dy);
  //ev.shader_api.use(sh);
  //ev.mainloop_api.switch_to_3d(true, sh, screen_width, screen_height);
  glEnable(GL_DEPTH_TEST);
 
 FBO fbo = ev.fbo_api.create_fbo(sx,sy);
  ev.fbo_api.config_fbo(fbo);
  ev.fbo_api.bind_fbo(fbo);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  //glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
  //		       GL_ONE, GL_ONE);
  ev.mainloop_api.clear_3d(0x00000000);
  obj.render();
  glEnable(GL_BLEND);
  while(!ev.fbo_api.fbo_status(fbo));
  //BM bm = ev.mainloop_api.screenshot();
  ev.fbo_api.bind_screen(screen_width, screen_height);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST);
  //ev.mainloop_api.switch_to_3d(false, sh, screen_width, screen_height);

  //return ev.fbo_api.fbo_to_bitmap(ev, fbo);
  TXID id = ev.fbo_api.tex_id(fbo);
  //glDisable(GL_DEPTH_TEST);
  //glViewport(0,0,screen_width, screen_height);
  BM bm = ev.texture_api.to_bitmap(id);
  BM bm2 = ev.bitmap_api.color_range(bm, 0x90ffffff, 0x00000000, 0xffffffff, 0x00000000);
  return bm2; 
}
