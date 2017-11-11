 
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
void GameApi::PolygonApi::print_data(P p)
{
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_data" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_data" << std::endl;  return; }
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      std::cout << "Face " << i << ": ";
      int ss = coll->NumPoints(i);
      for(int j=0;j<ss;j++)
	{
	  Point p = coll->FacePoint(i,j);
	  std::cout << p << " ";
	}
      std::cout << std::endl;
    }

}
void GameApi::PolygonApi::print_data2(P p)
{
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_data" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_data" << std::endl;  return; }
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      std::cout << "Face2 " << i << ": ";
      int ss = coll->NumPoints(i);
      for(int j=0;j<ss;j++)
	{
	  Point p = coll->EndFacePoint(i,j);
	  std::cout << p << " ";
	}
      std::cout << std::endl;
    }

}
void GameApi::PolygonApi::print_stat(P p)
{
  //FaceCollPolyHandle *handle = find_poly(e, p);
  //FaceCollection *coll = handle->coll;
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_stat" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_stat" << std::endl;  return; }
  int faces = coll->NumFaces();
  int points = faces>0 ? coll->NumPoints(0) : 0;
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

class DistFromLines : public SingleForwardFaceCollection
{
public:
  DistFromLines(LineCollection *coll, float d1, float d2, Point center) : coll(coll), d1(d1), d2(d2), center(center) { }
  void Prepare() { coll->Prepare(); }
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
EXPORT GameApi::P GameApi::PolygonApi::load_model_all_no_cache(std::string filename, int count)
{
  int s=count;
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(filename, i), 1);
      vec.push_back(model);
    }
  GameApi::P obj = or_array2(vec);
  GameApi::P resize = resize_to_correct_size(obj);
  return resize;

}

EXPORT GameApi::P GameApi::PolygonApi::load_model_all(std::string filename, int count)
{
  int s=count;
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(filename, i), 1);
      GameApi::P cache = file_cache(model, filename,i);
      vec.push_back(cache);
    }
  GameApi::P obj = or_array2(vec);
  GameApi::P resize = resize_to_correct_size(obj);
  return resize;
}

std::vector<std::pair<std::string, int> > prepare_cache_data;

int find_data(std::string data) {
  int s = prepare_cache_data.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string,int> p = prepare_cache_data[i];
      if (p.first==data) return p.second;
    }
  return -1;
}

class PrepareCache : public FaceCollection
{
public:
  PrepareCache(GameApi::Env &e, std::string id, FaceCollection *coll) : e(e), id(id), coll(coll) {}
  void Prepare()
  {
    //std::cout << "PrepareCache: " << id << std::endl;
    if (find_data(id)!=-1)
      {
	//std::cout << "PrepareCache: SKIPPED!" << std::endl;
	return;
      }
    coll->Prepare();
    GameApi::P num = add_polygon2(e, coll,1);
    prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  FaceCollection *get_coll() const
  {
    int num = find_data(id);
    if (num==-1) {const_cast<PrepareCache*>(this)->Prepare(); num=find_data(id); }
    GameApi::P p;
    p.id = num;
    FaceCollection *coll = find_facecoll(e, p);
    return coll;
  }

  virtual int NumFaces() const 
  {
    return get_coll()->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    return get_coll()->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    return get_coll()->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return get_coll()->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return get_coll()->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return get_coll()->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return get_coll()->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return get_coll()->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const {
    return get_coll()->TexCoord3(face,point);
  }
private:
  GameApi::Env &e;
  std::string id;
  FaceCollection *coll;
};


class NetworkedFaceCollection : public FaceCollection
{
public:
  NetworkedFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *empty, std::string url, std::string homepage, int count) : e(e), ev(ev), url(url), homepage(homepage), count(count), empty(empty)
  {
    current = empty;
    filled = 0;
  }
  void Prepare()
  {
    if (current == empty) {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif

    std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }
    static int num=0;
    num++;
    std::stringstream ss2;
    ss2 << "p_url_file" << num << ".obj";
    std::string filename = ss2.str();
    std::fstream ss(filename.c_str(), std::ios_base::binary |std::ios_base::out);
    int s = ptr->size();
    for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    ss.close();
    GameApi::P p = ev.polygon_api.load_model_all_no_cache(filename, count);
    FaceCollection *coll = find_facecoll(e, p);
    filled = coll;
    current = filled;
    current->Prepare();
    }
  }

  virtual int NumFaces() const { return current->NumFaces(); }
  virtual int NumPoints(int face) const { return current->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return current->FacePoint(face,point); }
  virtual Point EndFacePoint(int face, int point) const { return current->EndFacePoint(face, point); }

  virtual Vector PointNormal(int face, int point) const { return current->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return current->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return current->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return current->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return current->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return current->TexCoord3(face,point); }
  virtual int NumTextures() const { return current->NumTextures(); }
  virtual void GenTexture(int num) { current->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return current->TextureBuf(num); }
  virtual int FaceTexture(int face) const { return current->FaceTexture(face); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  int count;
  FaceCollection *empty;
  FaceCollection *filled;
  FaceCollection *current;
};
extern std::string gameapi_homepageurl;
EXPORT GameApi::P GameApi::PolygonApi::p_url(EveryApi &ev, std::string url, int count)
{
  P p = empty();
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollection(e,ev, emp, url, gameapi_homepageurl, count),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll),1);
  return p2;
}
EXPORT GameApi::P GameApi::PolygonApi::load_model(std::string filename, int num)
{
  GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(filename, num), 1);
  GameApi::P cache = file_cache(model, filename,num);
  GameApi::P resize = resize_to_correct_size(cache);
  return resize;
} 
class SaveModel : public MainLoopItem
{
public:
  SaveModel(GameApi::PolygonApi &api, GameApi::P poly, std::string filename) : api(api), poly(poly), filename(filename)
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      std::cout << "Saving " << filename << std::endl;
      api.save_model(poly, filename);
      firsttime = false;
    }
  }
  int shader_id() { return -1; }

private:
  GameApi::PolygonApi &api;
  GameApi::P poly;
  std::string filename;
  bool firsttime;
};
EXPORT GameApi::ML GameApi::PolygonApi::save_model_ml(GameApi::P poly, std::string filename)
{
  return add_main_loop(e, new SaveModel(*this, poly, filename));
}
EXPORT void GameApi::PolygonApi::save_model(GameApi::P poly, std::string filename)
{
  FaceCollection *face = find_facecoll(e, poly);
  face->Prepare();
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
      std::cout << "texcoord: " << pp.x << " " << pp.y << std::endl;
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
  
  return add_polygon2(e, new TexCoordSpherical(*center_1, face),1);
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
EXPORT GameApi::P GameApi::PolygonApi::fullscreen_quad(EveryApi &ev)
{
  int sx = 800-40; //ev.mainloop_api.get_screen_sx();
  int sy = 600-40; //ev.mainloop_api.get_screen_sy();
  return quad_z(-sx,sx, -sy, sy, 0.0);
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
EXPORT GameApi::P GameApi::PolygonApi::torus2(EveryApi &ev, int numfaces1, int numfaces2, PT center, float radius1, float radius2)
{
  V u_x = ev.vector_api.vector(1.0,0.0,0.0);
  V u_y = ev.vector_api.vector(0.0,1.0,0.0);
  V uu_x = ev.vector_api.vector(1.0,0.0,0.0);
  V uu_y = ev.vector_api.vector(0.0,0.0,1.0);
  return torus(numfaces1, numfaces2, center, u_x, u_y, radius1, uu_x, uu_y, radius2);
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
  //OrElem<FaceCollection> *coll = new OrElem<FaceCollection>;
  //coll->push_back(pp1);
  //coll->push_back(pp2);
  //coll->update_faces_cache();
  FaceCollection *coll = new OrElem2(pp1,pp2);
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
  void Prepare() { c1->Prepare(); c2->Prepare(); }
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
  Lambert(FaceCollection *coll, unsigned int color, Vector light_dir, float pow, float intensity) : ForwardFaceCollection(*coll), color(color), light_dir(light_dir),pow(pow), intensity(intensity) 
  {
    light_dir /= light_dir.Dist();
  }
  unsigned int Color(int face, int point) const
  {
    Vector n = ForwardFaceCollection::PointNormal(face,point);
    n /= n.Dist();
   
    //n /= n.Dist();
    float dot = Vector::DotProduct(n,light_dir);
    //if (dot<0.0) dot=0.0;
    //if (dot>1.0) dot=1.0;
    dot = std::pow(dot,pow);
    // dot = fabs(dot);
    dot*=intensity;
    if (dot<0.0) dot=0.0;
    //if (dot>1.0) dot=1.0;
    ::Color c(color);
    c.r*=dot;
    c.g*=dot;
    c.b*=dot;
    return c.clamp().Pixel();
  }
private:
  unsigned int color;
  Vector light_dir;
  float pow;
  float intensity;
};
EXPORT GameApi::P GameApi::PolygonApi::color_lambert(P orig, unsigned int color, float light_dir_x, float light_dir_y, float light_dir_z, float pow, float intensity)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *coll = new Lambert(c, color, Vector(light_dir_x, light_dir_y, light_dir_z), pow, intensity);
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
class ColorAlpha : public ForwardFaceCollection
{
public:
  ColorAlpha(FaceCollection *coll, unsigned int alpha) : ForwardFaceCollection(*coll), alpha(alpha) { }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int c = ForwardFaceCollection::Color(face,point);
    c = c & 0x00ffffff;
    c = c | (alpha << 24);
    return c;
  }
private:
  unsigned int alpha;
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
class QuadsToTris2 : public ForwardFaceCollection
{
public:
  QuadsToTris2(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { Iterate(); }
  void Iterate()
  {
    int counter = 0;
    int s = coll->NumFaces();
    for(int f=0;f<s;f++)
      {
	int count = coll->NumPoints(f);
	if (count==3) { 
	  counts.push_back(3);
	  counts2.push_back(counter); counter+=3;
	  for(int i=0;i<3;i++) { 
	    vec.push_back(coll->FacePoint(f,i));
	    norm.push_back(coll->PointNormal(f,i));
	    color.push_back(coll->Color(f,i));
	    texcoord.push_back(coll->TexCoord(f,i));
	  }
					      
	}
	else if (count==4)
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
	    int j = count;
	    int prev = j;
	    int prev2 = 1;
	    for(int i=2;i<count/2+1;i++,j--)
	      {
		counts.push_back(3);
		counts2.push_back(counter); counter+=3;

		vec.push_back(coll->FacePoint(f, prev%count));
		norm.push_back(coll->PointNormal(f,prev%count));
		color.push_back(coll->Color(f,prev%count));
		texcoord.push_back(coll->TexCoord(f,prev%count));

		vec.push_back(coll->FacePoint(f,prev2));
		norm.push_back(coll->PointNormal(f,prev2));
		color.push_back(coll->Color(f,prev2));
		texcoord.push_back(coll->TexCoord(f,prev2));

		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i+2));
		texcoord.push_back(coll->TexCoord(f,i));


		//i++; j--;
		//if (i>=count/2) break;

		counts.push_back(3);
		counts2.push_back(counter); counter+=3;

		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i));
		texcoord.push_back(coll->TexCoord(f,i));


		vec.push_back(coll->FacePoint(f,j-1));
		norm.push_back(coll->PointNormal(f,j-1));
		color.push_back(coll->Color(f,j-1));
		texcoord.push_back(coll->TexCoord(f,j-1));


		vec.push_back(coll->FacePoint(f,j%count));
		norm.push_back(coll->PointNormal(f,j%count));
		color.push_back(coll->Color(f,j%count));
		texcoord.push_back(coll->TexCoord(f,j%count));


		prev--; prev2++;

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
EXPORT GameApi::P GameApi::PolygonApi::color_alpha(P orig, unsigned int alpha)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorAlpha(c, alpha);
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
GameApi::P GameApi::PolygonApi::matrix(P orig, M mat)
{
  Matrix mat2 = find_matrix(e, mat);
  //::EnvImpl *env = ::EnvImpl::Environment(&e); 
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, mat2);
  return add_polygon2(e, coll,1);

}
GameApi::P GameApi::PolygonApi::translate_1(P orig, float dx, float dy, float dz)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::Translate(dx,dy,dz));
  return add_polygon2(e, coll,1);
}
 
EXPORT GameApi::P GameApi::PolygonApi::rotatex(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::XRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatey(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::YRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatez(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::ZRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotate(P orig, PT point, V axis, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  Point *pp = find_point(e, point);
  Vector *ax = find_vector(e, axis);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::RotateAroundAxisPoint(*pp, *ax, angle));
  return add_polygon(e, coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::scale(P orig, float sx, float sy, float sz)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);  
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *coll = new MatrixElem(*c, Matrix::Scale(sx,sy,sz));
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
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(next));  
  //MemoizeFaces *next2 = new MemoizeFaces(*next);
  //next2->Reset();
  return add_polygon(e, next, 1);
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
  GameApi::P p;
  p.id = 0;
  return p;
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
  GameApi::P p;
  p.id = 0;
  return p;
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
  GameApi::P p;
  p.id = 0;
  return p;
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
EXPORT GameApi::P GameApi::PolygonApi::average_normals(P orig, int sx, int sy)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new AverageNormals(coll, sx,sy);
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
class Span : public SingleForwardFaceCollection
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
  void Prepare() { lines->Prepare(); }
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
EXPORT GameApi::P GameApi::PolygonApi::linear_span(EveryApi &ev, LI li,
						   float dx, float dy, float dz,
						   int num_steps)
{
  return span(li, ev.matrix_api.trans(dx,dy,dz), num_steps);
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

class TriStripFaceCollection : public SingleForwardFaceCollection
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
EXPORT GameApi::P GameApi::PolygonApi::polygon2(std::vector<PT> vec)
{
  return polygon(&vec[0], vec.size());
}
EXPORT GameApi::P GameApi::PolygonApi::polygon3(PTS points)
{
  PointsApiPoints *p = find_pointsapi_points(e, points);
  PolygonElem *coll = new PolygonElem;
  int sz = p->NumPoints();
  for(int i=0;i<sz;i++)
    {
      Point pp = p->Pos(i);
      coll->push_back(pp);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = coll;
  handle->collowned = false;
  handle->collarray = NULL; 
  handle->collarrayowned = false;

  return add_polygon(e, handle);

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
  void handle_event(MainLoopEvent &e)
  {
  }
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
EXPORT void GameApi::PolygonApi::update_vertex_array_no_memory(GameApi::VA va, GameApi::P p)
{
  FaceCollection *faces = find_facecoll(e, p);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  VertexArraySet *set = find_vertex_array(e, va);
  set->clear_arrays();
  FaceCollectionVertexArray2 faces2(*faces, *set);
  faces2.copy(0,faces->NumFaces());
  rend->update(0);
}
EXPORT void GameApi::PolygonApi::delete_vertex_array(GameApi::VA va)
{
  GameApi::P p = empty();
  update_vertex_array(va,p);
}
void ProgressBar(int val, int max)
{
  std::cout << "\r[";
  for(int i=0;i<val;i++) {
    std::cout << "#";
  }
  for(int i=val;i<max-1;i++) {
    std::cout << "-";
  }
  std::cout << "]";
}
extern ThreadInfo *ti_global;
extern int thread_counter;


EXPORT void GameApi::PolygonApi::update_vertex_array(GameApi::VA va, GameApi::P p, bool keep)
{
  if (keep) {
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(*s);
    arr2->prepare(0); 
    if (!keep)
      s->free_memory();
    add_update_vertex_array(e, va, s, arr2);

  }


#ifdef THREADS


#ifndef BATCHING
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
#else // BATCHING
  int num_threads = 4;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();

  ThreadedPrepare prep(faces);  
  int s = faces->NumFaces();   
  //std::cout << "NumFaces: " << s << std::endl;
  if (s<100) { num_threads=1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "NumThreads2: " << num_threads << std::endl;
  Counts ct = CalcCounts(faces, 0, faces->NumFaces());
  VertexArraySet *set = new VertexArraySet;
  RenderVertexArray *arr2 = new RenderVertexArray(*set);
  arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));
  pthread_mutex_t *mutex1 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex3 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  thread_counter = 0;
  pthread_mutex_lock(mutex3); // LOCK mutex3
  pthread_mutex_lock(mutex2);
  for(int i=0;i<num_threads;i++)
    {  
      int start_range = i*delta_s; 
      int  end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; } 
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread2(start_range, end_range,arr2, mutex1, mutex2,mutex3));
    }
  int progress = 0;
  while(1) {
    pthread_mutex_lock(mutex3); // WAIT FOR mutex3 to open.
    progress++;
    ProgressBar(progress/num_threads,10);

    // now ti_global is available
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 0, ti_global->ct2_offsets.tri_count*3, ti_global->ct2_offsets.tri_count*3 + ti_global->ct2_counts.tri_count*3); 
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 1, ti_global->ct2_offsets.quad_count*6, ti_global->ct2_offsets.quad_count*6 + ti_global->ct2_counts.quad_count*6);
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 2, std::max(ti_global->ct2_offsets.poly_count-1,0), std::max(ti_global->ct2_offsets.poly_count-1,0) + (ti_global->ct2_offsets.poly_count?ti_global->ct2_counts.poly_count:ti_global->ct2_counts.poly_count-1));
    ti_global = 0;
    pthread_mutex_unlock(mutex2); // release other process
    if (thread_counter==num_threads) break;
  }
 
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
     }
  pthread_mutex_destroy(mutex1);
  pthread_mutex_destroy(mutex2);
  pthread_mutex_destroy(mutex3);
  //VertexArraySet *set = new VertexArraySet;
  //RenderVertexArray *arr2 = new RenderVertexArray(*set);
  //arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
      //::EnvImpl *env = ::EnvImpl::Environment(&e);
      //env->temp_deletes.push_back(std::shared_ptr<void>( arr2 ) );
    }
  add_update_vertex_array(e, va, set, arr2);
  
#endif

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
  if (keep) {
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(*s);
    arr2->prepare(0); 
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
  }

#ifdef THREADS
#ifdef EMSCRIPTEN
#ifdef __EMSCRIPTEN_PTHREADS__
  if (emscripten_has_threading_support()) {
#else
  if (0) {
#endif // END OF __EMSCRIPTEN_PTHREADS
#endif // END OF EMSCRIPTEN

#ifndef BATCHING
  int num_threads = 4;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
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
      //::EnvImpl *env = ::EnvImpl::Environment(&e);
      //env->temp_deletes.push_back(std::shared_ptr<void>( arr2 ) );
    }
#else // BATCHING
  int num_threads = 4;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();

  ThreadedPrepare prep(faces);  
  int s = faces->NumFaces();   
  //std::cout << "NumFaces: " << s << std::endl;
  if (s<100) { num_threads=1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "NumThreads2: " << num_threads << std::endl;
  Counts ct = CalcCounts(faces, 0, faces->NumFaces());
  VertexArraySet *set = new VertexArraySet;
  RenderVertexArray *arr2 = new RenderVertexArray(*set);
  arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));
  pthread_mutex_t *mutex1 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex3 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  thread_counter = 0;
  pthread_mutex_lock(mutex3); // LOCK mutex3
  pthread_mutex_lock(mutex2);
  for(int i=0;i<num_threads;i++)
    {  
      int start_range = i*delta_s; 
      int  end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; } 
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread2(start_range, end_range,arr2, mutex1, mutex2,mutex3));
    }
  int progress = 0;
  while(1) {
    pthread_mutex_lock(mutex3); // WAIT FOR mutex3 to open.
    progress++;
    ProgressBar(progress/num_threads,10);

    // now ti_global is available
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 0, ti_global->ct2_offsets.tri_count*3, ti_global->ct2_offsets.tri_count*3 + ti_global->ct2_counts.tri_count*3); 
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 1, ti_global->ct2_offsets.quad_count*6, ti_global->ct2_offsets.quad_count*6 + ti_global->ct2_counts.quad_count*6);
    ti_global->prep->transfer_to_gpu_mem(ti_global->set, *ti_global->r, 0, 2, std::max(ti_global->ct2_offsets.poly_count-1,0), std::max(ti_global->ct2_offsets.poly_count-1,0) + (ti_global->ct2_offsets.poly_count?ti_global->ct2_counts.poly_count:ti_global->ct2_counts.poly_count-1));
    ti_global = 0;
    pthread_mutex_unlock(mutex2); // release other process
    if (thread_counter==num_threads) break;
  }
 
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
     }
  pthread_mutex_destroy(mutex1);
  pthread_mutex_destroy(mutex2);
  pthread_mutex_destroy(mutex3);
  //VertexArraySet *set = new VertexArraySet;
  //RenderVertexArray *arr2 = new RenderVertexArray(*set);
  //arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
      //::EnvImpl *env = ::EnvImpl::Environment(&e);
      //env->temp_deletes.push_back(std::shared_ptr<void>( arr2 ) );
    }

  
#endif // BATCHING


#ifdef EMSCRIPTEN
  return add_vertex_array(e, set, arr2);
  } else {
#ifndef BATCHING
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(*s);
    arr2->prepare(0); 
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
#else // BATCHING
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    int total_faces = faces->NumFaces();
    int batch_count = 10;
    if (total_faces<100) batch_count=1;
    int batch_faces = faces->NumFaces()/batch_count+1;
    Counts ct = CalcCounts(faces, 0, faces->NumFaces());
    VertexArraySet *s = new VertexArraySet;
    RenderVertexArray *arr2 = new RenderVertexArray(*s);
    //std::cout << "Counts: " << ct.tri_count << " " <<  ct.quad_count << " " << ct.poly_count << std::endl;
    arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));  // SIZES MUST BE KNOWN
    for(int i=0;i<batch_count;i++) {
      ProgressBar(i,batch_count);
      int start = i*batch_faces;
      int end = (i+1)*batch_faces;
      if (start>total_faces) { start=total_faces; }
      if (end>total_faces) { end=total_faces; }
      Counts ct2_counts = CalcCounts(faces, start, end);
      Counts ct2_offsets = CalcOffsets(faces, start);
      FaceCollectionVertexArray2 arr(*faces, *s);
      //arr.reserve(0);
      arr.copy(start,end);  
      arr2->update_tri(0, 0, ct2_offsets.tri_count*3, ct2_offsets.tri_count*3 + ct2_counts.tri_count*3);
      arr2->update_tri(0, 1, ct2_offsets.quad_count*6, ct2_offsets.quad_count*6 + ct2_counts.quad_count*6);
      arr2->update_tri(0, 2, std::max(ct2_offsets.poly_count-1,0), std::max(ct2_offsets.poly_count-1,0) + (ct2_offsets.poly_count?ct2_counts.poly_count:ct2_counts.poly_count-1));
      s->free_reserve(0);
    }
    //std::cout << "\n";
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
#endif // BATCHING
  }
#else // EMSCRIPTEN
  return add_vertex_array(e, set, arr2);
#endif // end of EMSCRIPTEN

#else // THREADS
#ifndef BATCHING
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces());  
  RenderVertexArray *arr2 = new RenderVertexArray(*s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  return add_vertex_array(e, s, arr2);

#else // BATCHING
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    int total_faces = faces->NumFaces();
    int batch_count = 10;
    if (total_faces<100) batch_count=1;
    int batch_faces = faces->NumFaces()/batch_count+1;
    Counts ct = CalcCounts(faces, 0, faces->NumFaces());
    VertexArraySet *s = new VertexArraySet;
    RenderVertexArray *arr2 = new RenderVertexArray(*s);
    //std::cout << "Counts: " << ct.tri_count << " " <<  ct.quad_count << " " << ct.poly_count << std::endl;
    arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));  // SIZES MUST BE KNOWN
    for(int i=0;i<batch_count;i++) {
      ProgressBar(i,batch_count); 
      int start = i*batch_faces;
      int end = (i+1)*batch_faces;
      if (start>total_faces) { start=total_faces; }
      if (end>total_faces) { end=total_faces; }
      Counts ct2_counts = CalcCounts(faces, start, end);
      Counts ct2_offsets = CalcOffsets(faces, start);
      FaceCollectionVertexArray2 arr(*faces, *s);
      //arr.reserve(0);
      arr.copy(start,end);  
      arr2->update_tri(0, 0, ct2_offsets.tri_count*3, ct2_offsets.tri_count*3 + ct2_counts.tri_count*3);
      arr2->update_tri(0, 1, ct2_offsets.quad_count*6, ct2_offsets.quad_count*6 + ct2_counts.quad_count*6);
      arr2->update_tri(0, 2, std::max(ct2_offsets.poly_count-1,0), std::max(ct2_offsets.poly_count-1,0) + (ct2_offsets.poly_count?ct2_counts.poly_count:ct2_counts.poly_count-1));
      s->free_reserve(0);
    }
    //std::cout << "\n";
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
#endif // end of BATCHING
#endif // end of THREADS
}

EXPORT GameApi::VA GameApi::PolygonApi::create_vertex_array_attribs(GameApi::P p, bool keep, std::vector<int> attribs, std::vector<int> attribi)
{ 
#ifdef THREADS
  int num_threads = 4;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
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
      vec.push_back(prep.push_thread(start_range, end_range,attribs,attribi));
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
  faces->Prepare();
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces(),attribs,attribi);  
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
    return float(val)+0.5f;
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
  RenderVA(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::VA va) : env(env), ev(ev), api(api), va(va)
  {
    shader.id = -1;
    firsttime = true;
  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
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

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::SH shader;
};

class RenderP : public MainLoopItem
{
public:
  RenderP(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p) : env(env), ev(ev), api(api), p(p)
  {
    shader.id = -1;
    firsttime = true;
  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	va = ev.polygon_api.create_vertex_array(p,false);
      }

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

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (firsttime && shader.id==-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
#if 1
    //ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	//ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
#endif
	}
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    ev.shader_api.set_var(sh, "in_POS", 0.0f);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
};


class RenderPTex : public MainLoopItem
{
public:
  RenderPTex(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, std::vector<GameApi::BM> bm) : env(env), ev(ev), api(api), p(p), bm(bm)
  {
    shader.id = -1;
    firsttime = true;
  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	va = ev.polygon_api.create_vertex_array(p, true);
	std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev, bm);
	va = ev.texture_api.bind_many(va, id);
      }


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

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
  std::vector<GameApi::BM> bm;
};


class RenderDynP : public MainLoopItem
{
public:
  RenderDynP(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, GameApi::DC dc) : env(env), ev(ev), api(api), p(p), dc(dc)
  {
    shader.id = -1;
    firsttime = true;
  }
  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	va = ev.polygon_api.create_vertex_array(p,true);
	ev.polygon_api.clone(va);
      }

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

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array_dyn(va,dc,e);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::DC dc;
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
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="snoise";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="snoise";
#endif
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_snoise(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_snoise(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};

class CustomShaderML : public MainLoopItem
{
public:
  CustomShaderML(GameApi::EveryApi &ev, MainLoopItem *next, std::string v_shaderstring, std::string f_shaderstring, std::string v_funcname, std::string f_funcname) : ev(ev), next(next), v_shaderstring(v_shaderstring), f_shaderstring(f_shaderstring), v_funcname(v_funcname), f_funcname(f_funcname) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_custom(vertex, v_funcname);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_custom(fragment, f_funcname);
    ee.us_fragment_shader = a2f.id;
    }

    int s = ee.v_shader_funcnames.size();
    bool v_add=true;
    for(int i=0;i<s;i++)
      {
	if (ee.v_shader_funcnames[i] == v_funcname) v_add=false;
      }
    int s2 = ee.f_shader_funcnames.size();
    bool f_add=true;
    for(int i=0;i<s2;i++)
      {
	if (ee.f_shader_funcnames[i] == f_funcname) f_add=false;
      }
    if (v_add)
      {
      ee.v_shader_functions += v_shaderstring;
      ee.v_shader_funcnames.push_back(v_funcname);
      }
    if (f_add)
      {
      ee.f_shader_functions += f_shaderstring;
      ee.f_shader_funcnames.push_back(f_funcname);
      }
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  std::string v_shaderstring, f_shaderstring;
  std::string v_funcname, f_funcname;
};


class TextureShaderML : public MainLoopItem
{
public:
  TextureShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_texture(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_texture(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
      }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};



class TextureManyShaderML : public MainLoopItem
{
public:
  TextureManyShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_manytexture(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_manytexture(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
      }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};



class TextureArrShaderML : public MainLoopItem
{
public:
  TextureArrShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_texture_arr(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_texture_arr(fragment);
    ee.us_fragment_shader = a2f.id;
    }
    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
      }

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};


class LightShaderML : public MainLoopItem
{
public:
  LightShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="light";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="light";
#endif
    if (firsttime) {
      firsttime = false; 
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_light(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_light(fragment);
    ee.us_fragment_shader = a2f.id;
    }
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};

class ToonShaderML : public MainLoopItem
{
public:
  ToonShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="toon";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="toon";
#endif
    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_toon(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_toon(fragment);
    ee.us_fragment_shader = a2f.id;
      }
    next->execute(ee);
  }
  int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};

class SkeletalShader : public MainLoopItem
{
public:
  SkeletalShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, std::vector<SkeletalNode*> vec) : env(env), ev(ev), next(next), vec(vec) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_skeletal(vertex);
    //ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
      }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.use(sh);
    ev.shader_api.set_var(sh, "in_MV", m);
    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_N", m2);
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);

    std::vector<GameApi::M> vec1;
    std::vector<GameApi::PT> vec2;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	//std::cout << time << std::endl;
	Matrix mm = vec[i]->mat(time);
	//std::cout << mm << std::endl;
	GameApi::M m = add_matrix2(env, mm);
	Point ppt = vec[i]->pos(time);
	//std::cout << "Point: " << ppt << std::endl;
	GameApi::PT pt = add_point(env, ppt.x, ppt.y, ppt.z); // TODO MEM LEAK ON POINTS
	vec1.push_back(m);
	vec2.push_back(pt); 
      }

    ev.shader_api.set_var(sh, "bones", vec1,50);
    ev.shader_api.set_var(sh, "bone_pos", vec2);

    next->execute(ee);
    ev.shader_api.unuse(sh);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  std::vector<SkeletalNode*> vec;
};

class DistFieldMeshShader : public MainLoopItem
{
public:
  DistFieldMeshShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_dist_field_mesh(vertex, sfo);
    ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
      }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_N", m2);
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};


class MeshColorShader : public MainLoopItem
{
public:
  MeshColorShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_pass_position(vertex);
    ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_mesh_color(fragment, sfo);
    ee.us_fragment_shader = a2f.id;
      }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_N", m2);
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};

class SFOSandboxShader : public MainLoopItem
{
public:
  SFOSandboxShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    //GameApi::US a2v = ev.uber_api.v_pass_position(vertex);
    ee.sfo_id = sfo.id;
    //ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_sandbox(fragment, sfo);
    ee.us_fragment_shader = a2f.id;
      }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_N", m2);
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};


class ChooseColorShaderML : public MainLoopItem
{
public:
  ChooseColorShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, unsigned int color, float mix_val) : env(env), ev(ev), next(next), color(color), mix_val(mix_val) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_choose_color(fragment);
    ee.us_fragment_shader = a2f.id;
      }

    int sh_id = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_N", m2);
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    Color cc(color);
    ev.shader_api.set_var(sh, "color_choice", cc.rf(), cc.gf(), cc.bf(), cc.af());
    ev.shader_api.set_var(sh, "mix_val", mix_val);

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  unsigned int color;
  float mix_val;
};

class SpotlightShaderML : public MainLoopItem
{
public:
  SpotlightShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, int light_color_id, Movement *change) : env(env), ev(ev), next(next), light_color_id(light_color_id), change(change) 
  {
    firsttime = true;
  }
  int shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
	GameApi::US vertex;
	vertex.id = ee.us_vertex_shader;
	if (vertex.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.v_empty();
	    GameApi::US a1 = ev.uber_api.v_colour(a0);
	    ee.us_vertex_shader = a1.id;
	  }
	vertex.id = ee.us_vertex_shader;
	GameApi::US a2v = ev.uber_api.v_point_light(vertex);
	ee.us_vertex_shader = a2v.id;

	GameApi::US fragment;
	fragment.id = ee.us_fragment_shader;
	if (fragment.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.f_empty(false);
	    GameApi::US a1 = ev.uber_api.f_colour(a0);
	    ee.us_fragment_shader = a1.id;
	  }
	fragment.id = ee.us_fragment_shader;
	//GameApi::US a2f = ev.uber_api.f_color_from_id(fragment, light_color_id);
	GameApi::US a3f = ev.uber_api.f_point_light(fragment);
	ee.us_fragment_shader = a3f.id;
      }
    int sh_id = next->shader_id();
    if (sh_id != -1)
      {
	sh.id = sh_id;
      }
    {
	ev.shader_api.use(sh);
	Point p(0.0,0.0,0.0);
	Matrix m = change->get_whole_matrix(e.time*10.0, e.delta_time);
	Point p2 = p * m;
	std::stringstream ss;
	ss << e.spotlight_id;
	ev.shader_api.set_var(sh, "lightpos" + ss.str(), p2.x,p2.y,p2.z);
    }
    GameApi::M m = add_matrix2( env, e.in_MV );
    GameApi::M m1 = add_matrix2( env, e.in_T );
    GameApi::M m2 = add_matrix2( env, e.in_N );
    ev.shader_api.set_var( sh, "in_MV", m);
    ev.shader_api.set_var( sh, "in_T", m1);
    ev.shader_api.set_var( sh, "in_N", m2);
    ev.shader_api.set_var( sh, "time", e.time);

    ee.spotlight_id = e.spotlight_id+1;
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int light_color_id;
  Movement *change;
  bool firsttime;
  GameApi::SH sh;
};

class ShaderParamML : public MainLoopItem
{
public:
  ShaderParamML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir,
		unsigned int level1, unsigned int level2, unsigned int level3, float spec_size, bool ambient, bool diffuse, bool specular) : env(env), ev(ev), next(next), light_dir(light_dir), level1(level1), level2(level2), level3(level3), spec_size(spec_size), ambient(ambient), diffuse(diffuse), specular(specular) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  int shader_id() { if (sh.id != -1) return sh.id; return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_ambient(vertex);
    GameApi::US a3v = ev.uber_api.v_diffuse(a2v);
    GameApi::US a4v = ev.uber_api.v_specular(a3v);

    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = a4v.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient)
      fragment = ev.uber_api.f_ambient(fragment);
    if (diffuse)
      fragment = ev.uber_api.f_diffuse(fragment);
    if (specular)
      fragment = ev.uber_api.f_specular(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

    int sh_id = next->shader_id();
    sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
	ev.shader_api.set_var(sh, "specular_size", spec_size);
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

	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", 0.0f);

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  unsigned int level1, level2, level3;
  GameApi::SH sh;
  bool firsttime;
  float spec_size;
  bool ambient, diffuse, specular;
};
EXPORT GameApi::ML GameApi::PolygonApi::noise_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NoiseShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::custom_shader(EveryApi &ev, ML mainloop, std::string v_shaderstring, std::string f_shaderstring, std::string v_funcname, std::string f_funcname)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new CustomShaderML(ev, item,v_shaderstring,f_shaderstring,v_funcname,f_funcname));
}
EXPORT GameApi::ML GameApi::PolygonApi::spotlight_shader(EveryApi &ev, ML mainloop, int light_color_id, MN move)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  Movement *change = find_move(e, move);
  return add_main_loop(e, new SpotlightShaderML(e,ev, item, light_color_id, change));
}
#if 0
EXPORT GameApi::ML GameApi::PolygonApi::ambient_shader(EveryApi &ev, ML mainloop, int ambient_color_id, float ambient_level)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new AmbientShaderML(ev, item, ambient_color_id, ambient_level));
}
#endif
EXPORT GameApi::ML GameApi::PolygonApi::choose_color_shader(EveryApi &ev, ML mainloop, unsigned int color, float mix_val)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ChooseColorShaderML(e, ev, item, color, mix_val));
}
EXPORT GameApi::ML GameApi::PolygonApi::texture_shader(EveryApi &ev, ML mainloop, float mix=0.5)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new TextureShaderML(ev, item, mix));
}
 EXPORT GameApi::ML GameApi::PolygonApi::texture_many_shader(EveryApi &ev, ML mainloop, float mix=0.5)
 {
   MainLoopItem *item = find_main_loop(e, mainloop);
   return add_main_loop(e, new TextureManyShaderML(ev, item, mix));
 }
EXPORT GameApi::ML GameApi::PolygonApi::texture_arr_shader(EveryApi &ev, ML mainloop, float mix=0.5)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new TextureArrShaderML(ev, item, mix));
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
 EXPORT GameApi::ML GameApi::PolygonApi::dist_field_mesh_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new DistFieldMeshShader(e,ev,item,sfo));
}
 EXPORT GameApi::ML GameApi::PolygonApi::mesh_color_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new MeshColorShader(e,ev,item,sfo));
}
 EXPORT GameApi::ML GameApi::PolygonApi::sfo_sandbox_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new SFOSandboxShader(e,ev,item,sfo));
}
EXPORT GameApi::ML GameApi::PolygonApi::skeletal_shader(EveryApi &ev, ML mainloop, std::vector<SA> vec)
{
  std::vector<SkeletalNode*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_skeletal(e,vec[i]));
    }
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new SkeletalShader(e,ev,item,vec2));
}

EXPORT GameApi::ML GameApi::PolygonApi::shading_shader(EveryApi &ev, ML mainloop,
						      unsigned int level1,
						      unsigned int level2,
						       unsigned int level3, float spec_size, bool ambient, bool diffuse, bool specular)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  Vector light_dir(1.0,1.0,1.0);
  return add_main_loop(e, new ShaderParamML(e, ev, item, light_dir, level1, level2, level3, spec_size, ambient, diffuse, specular));
}
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml(EveryApi &ev, VA va)
{
  return add_main_loop(e, new RenderVA(e, ev, *this, va));
}
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2(EveryApi &ev, P p)
{
    GameApi::PTS pts = ev.points_api.single_pts();
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev,p,pts);
    return ml;
    // return add_main_loop(e, new RenderP(e, ev, *this, p));
}
 EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2_texture(EveryApi &ev, P p, std::vector<BM> bm)
 {
   return add_main_loop(e, new RenderPTex(e, ev, *this, p, bm));
 }
EXPORT GameApi::ML GameApi::PolygonApi::render_dynamic_ml(EveryApi &ev, P p, DC dyn)
{
  return add_main_loop(e, new RenderDynP(e, ev, *this, p, dyn));
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
  if (s->texture_many_ids.size()!=0) {
    glEnable(GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	glActiveTexture(GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        glClientActiveTexture(GL_TEXTURE0+i);
#endif
	glBindTexture(GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(0);

      glDisable(GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
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
EXPORT void GameApi::PolygonApi::clone(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  s->clone(0,1);
}
EXPORT void GameApi::PolygonApi::render_vertex_array_dyn(VA va, DC dc, MainLoopEnv &ee)
{
  DynamicChange *ddc = find_dyn_change(e, dc);
  VertexArraySet *s = find_vertex_array(e, va);
  s->apply_change(ddc, 0, 1, ee);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  rend->update(1);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    glEnable(GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	glActiveTexture(GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        glClientActiveTexture(GL_TEXTURE0+i);
#endif
	glBindTexture(GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(1);

      glDisable(GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);
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
      rend->render(1);

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
      rend->render(1);
      glDisable(GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);
    }
}
EXPORT void GameApi::PolygonApi::prepare_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);
  rend->prepare_instanced(0, (Point*)arr->array, arr->numpoints);
}
EXPORT void GameApi::PolygonApi::render_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh, int hide_n)
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
  if (s->texture_many_ids.size()!=0) {
    glEnable(GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	glActiveTexture(GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        glClientActiveTexture(GL_TEXTURE0+i);
#endif
	glBindTexture(GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(0);

      glDisable(GL_TEXTURE_2D);

  } 
  else
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
  if (s->texture_many_ids.size()!=0) {
    glEnable(GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	glActiveTexture(GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        glClientActiveTexture(GL_TEXTURE0+i);
#endif
	glBindTexture(GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced(0, (Point*)arr->array, show_num);

      glDisable(GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced(0, (Point*)arr->array, show_num);
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
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array, show_num);
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

      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array, show_num);
      glDisable(GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array, show_num);
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
class AnimInterpolate : public ForwardFaceCollection
{
public:
  AnimInterpolate(FaceCollection *coll, float val) : ForwardFaceCollection(*coll), coll(coll), val(val) { }
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
class CutFaces : public ForwardFaceCollection
{
public:
  CutFaces(FaceCollection *i, VolumeObject *oo, Cutter *cut) : ForwardFaceCollection(*i), i(i), oo(oo), cut(cut) { cut_them(); compress(); }

  void cut_them()
  {
    int f = i->NumFaces();
    for(int ii=0;ii<f; ii++)
      {
	faces.push_back(std::vector<Point>());
	int p = i->NumPoints(ii);
	//std::cout << p << std::endl;
	std::vector<Point> &ref = faces[faces.size()-1];
	//std::cout << "start" << std::endl;
	for(int jj=0;jj<p;jj++)
	  {
	    int jjj1 = jj;
	    while(jjj1<0) jjj1+=p;
	    while(jjj1>=p) jjj1-=p;
	    int jjj2 = jjj1+1;
	    while(jjj2<0) jjj2+=p;
	    while(jjj2>=p) jjj2-=p;
	    Point p1 = i->FacePoint(ii, jjj1);
	    Point p2 = i->FacePoint(ii, jjj2);
	    bool b1 = oo->Inside(p1);
	    bool b2 = oo->Inside(p2);

	    if (jj==0) { ref.push_back(p1); }

	    if (!b1 && !b2) {
	      //std::cout << "!b1 && !b2" << std::endl;
	      //ref.push_back(p1);
		  ref.push_back(p2);	      
	    }
	    if (!b1 && b2) {
	      //std::cout << "!b1 && b2" << std::endl;
	      Point prev = p1;
	      Point curr = p2;
	      for(int h=0;h<p;h++)
		{
		  int jjj3 = jjj2+h;
		  while(jjj3<0) jjj3+=p;
		  while(jjj3>=p) jjj3-=p;

		  curr = i->FacePoint(ii, jjj3);
		  bool b3 = oo->Inside(curr);
		  if (!b3) break;
		  prev = curr;
		}
	      std::vector<Point> c0 = cut->cut(p1,p2);
	      std::vector<Point> c1 = cut->cut(prev,curr);
	      //ref.push_back(p1);
	      ref.push_back(c0[0]);
	      ref.push_back(c1[0]);	      
	      ref.push_back(curr);
	    }
#if 1
	    if (b1 && !b2) {
	      //std::cout << "b1 && !b2" << std::endl;
	      std::vector<Point> c0 = cut->cut(p1,p2);
	      //ref.push_back(c0[0]);
	      ref.push_back(p2);	      
	    }
#endif
	    //if (b1 && b2)
	    //  {
	    //	  ref.push_back(p1);
	    //	  ref.push_back(p2);	      
	    //  }
	    
	    

	  }
      }
  }
  void compress()
  {
    int f = faces.size();
    int count = 0;
    for(int ii=0;ii<f; ii++)
      {
	int p = faces[ii].size();
	if (p==0 ||p==1 ||p==2) continue;
	faces2.push_back(std::vector<Point>());
#if 1
	Point prev;
	for(int i=0;i<p;i++)
	  {
	    if ((prev-faces[ii][i]).Dist()>0.001)
	      faces2[count].push_back(faces[ii][i]);
	    prev = faces[ii][i];
	  }
#endif
	//			faces2[count].push_back(faces2[count][0]);
	count++;
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
class TriToQuad : public ForwardFaceCollection
{
public:
  TriToQuad(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
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

class ColorMapPoly : public SingleForwardFaceCollection
{
public:
  ColorMapPoly(Bitmap<::Color> *bm, Point pos, Vector u_x, Vector u_y) : bm(bm), pos(pos), u_x(u_x), u_y(u_y) { prepared = false; }
    void Prepare() { bm->Prepare(); prepared = true; }
  virtual int NumFaces() const { if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare(); return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare(); return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
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
if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
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
  bool prepared;
};
#if 0
class ColorMapPoly4 : public FaceCollection
{
public:
  ColorMapPoly4(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) { 
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist();
  }
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

    return p;
  }
};
#endif
class ColorMapPoly2 : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm),fb(fb), pos(pos), u_x(u_x), u_y(u_y) { u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); }
  void Prepare() { bm->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(),yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val)) { 
      float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
      float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
      float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
      float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
      if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	{
	  val = val0;
	}
      else
	val=0.0; 
    }
    p+=val*u_z;
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
class ColorMapPoly2_cyl : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2_cyl(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) {
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); 
  }
  void Prepare() { bm->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Matrix m = Matrix::YRotation(xx*3.14159*2.0/bm->SizeX());
    Vector uu_x = u_x*m;
    Vector uu_y = u_y;
    Vector uu_z = u_z*m;
    Point p = pos + yy*uu_y/bm->SizeY();
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(), yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val))
      {
	float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	  {
	    val = val0;
	  }
	else
	  val=0.0; 
      }
    p+=val*uu_x + val*uu_z;
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


class ColorMapPoly2_sph : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2_sph(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) {
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); 
    u_z *= u_x.Dist();
  }
  void Prepare() { bm->Prepare(); fb->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }

    float angle1 = 3.14159*xx/bm->SizeX();
    float angle2 = 2.0*3.14159*yy/bm->SizeY();

    Vector uu;
    uu.dx = u_x.Dist() * sin(angle1)*cos(angle2);
    uu.dy = u_y.Dist() * sin(angle1)*sin(angle2);
    uu.dz = u_z.Dist() * cos(angle1);

    //Matrix m = Matrix::YRotation(xx*3.14159*2.0/bm->SizeX());
    //Vector uu_x = u_x*m;
    //Vector uu_y = u_y;
    //Vector uu_z = u_z*m;
    Point p = pos;
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(), yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val))
      {
	float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	  {
	    val = val0;
	  }
	else
	  val=0.0; 
      }
    p+=val*uu;
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

EXPORT GameApi::ML GameApi::PolygonApi::sprite_render(EveryApi &ev, BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  GameApi::P I1=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  GameApi::BM I2=bm; 
  GameApi::MT I3=ev.materials_api.texture(ev,I2,1.0);
  GameApi::ML I4=ev.materials_api.bind(I1,I3);
  return I4;
}
EXPORT GameApi::ML GameApi::PolygonApi::sprite_render_inst(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z)
{
  P I5=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  BM I6=bm;
  MT I7=ev.materials_api.texture(ev,I6,1.0);
  ML I8=ev.materials_api.bind_inst(I5,pts,I7);
  return I8;
}
EXPORT GameApi::ML GameApi::PolygonApi::sprite_render_fade(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z, bool flip, float start_time, float end_time)
{
  P I9=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  BM I10=bm; //ev.bitmap_api.chessboard(10,10,8,8,ffffffff,ff888888);
  MT I11=ev.materials_api.texture(ev,I10,1.0);
  ML I12=ev.materials_api.bind_inst_fade(I9,pts,I11,flip,start_time,end_time);
  return I12;
}
							  
EXPORT GameApi::P GameApi::PolygonApi::color_map(BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);

  Point pos(start_x, start_y, z);
  Vector u_x(end_x-start_x, 0.0, 0.0);
  Vector u_y(0.0, end_y-start_y, 0.0);
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

EXPORT GameApi::P GameApi::PolygonApi::color_map3_cyl(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2_cyl(bm1, fb, *pos1, *u_x1, *u_y1), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_map3_sph(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2_sph(bm1, fb, *pos1, *u_x1, *u_y1), 1);
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

EXPORT GameApi::P GameApi::PolygonApi::color_map3_cyl(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2_cyl(bm1, fb, pos, u_x, u_y), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3_sph(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2_sph(bm1, fb, pos, u_x, u_y), 1);
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
  void Prepare() { coll1->Prepare(); coll2->Prepare(); }
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
    pp.x = alfa/3.14159265;
    pp.y = 0.5+beta/3.14159265/2.0;
    // std::cout << "texcoord: " << pp.x << " " << pp.y << std::endl;
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
  //int screen_width = ev.mainloop_api.get_screen_width();
  //int screen_height = ev.mainloop_api.get_screen_height();
  //Point2d pos = { 0.0, 0.0 };
  //Vector2d sz = { float(sx), float(sy) };
  //glViewport(pos.x, screen_height-pos.y-sz.dy, sz.dx, sz.dy);
  //ev.shader_api.use(sh);
  //ev.mainloop_api.switch_to_3d(true, sh, screen_width, screen_height);
  glEnable(GL_DEPTH_TEST);
 
 FBO fbo = ev.fbo_api.create_fbo(sx,sy);
  ev.fbo_api.config_fbo(fbo);
  GameApi::FrameBufferApi::vp viewport = ev.fbo_api.bind_fbo(fbo);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  //glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
  //		       GL_ONE, GL_ONE);
  ev.mainloop_api.clear_3d(0x00000000);
  obj.render();
  glEnable(GL_BLEND);
  while(!ev.fbo_api.fbo_status(fbo));
  //BM bm = ev.mainloop_api.screenshot();
  ev.fbo_api.bind_screen(viewport);
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
struct CacheItem
{
public:
  std::string filename;
  GameApi::P obj;  
  int filesize;
  int obj_count;
};
int filesize(std::string filename)
{
  std::ifstream in(filename, std::ifstream::ate|std::ifstream::binary);
  return in.tellg();
}
bool invalidate(CacheItem *item, std::string filename, int obj_count)
{
  int size = filesize(filename);
  return item->filesize != size || item->obj_count != obj_count;
}

class PrepareCut : public ForwardFaceCollection
{
public:
  PrepareCut(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  void Prepare() { }
};
GameApi::P GameApi::PolygonApi::prepare_cut(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new PrepareCut(coll), 1);
}

std::string cache_id(std::string filename, int obj_count)
{
  std::stringstream ss;
  ss << filename << obj_count;
  return ss.str();
}

GameApi::P GameApi::PolygonApi::file_cache(P model, std::string filename, int obj_count)
{
  static std::map<std::string, CacheItem*> cache_map;
  CacheItem *item = cache_map[cache_id(filename,obj_count)];
  if (item && !invalidate(item,filename,obj_count)) {
    std::cout << "From Cache" << std::endl;
    return item->obj;
  }
  std::cout << "From File" << std::endl;
  CacheItem *item2 = new CacheItem;
  item2->filename = filename;
  item2->obj = memoize(prepare_cut(model));
  item2->filesize = filesize(filename);
  item2->obj_count = obj_count;
  cache_map[cache_id(filename,obj_count)]=item2;
  return model;
}
class ResizeFaceCollection : public ForwardFaceCollection
{
public:
  ResizeFaceCollection(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll)
  {
  }
  void Prepare()
  {
    coll->Prepare();

    find_bounding_box();
    print_bounding_box();
    calc_center();
    calc_size();
    calc_matrix();
  }
  Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    return p*m_m;
  }
public:
  Matrix get_matrix() const { return m_m; }

private:
  void find_bounding_box()
  {
    start_x = std::numeric_limits<float>::max();
    start_y = std::numeric_limits<float>::max();
    start_z = std::numeric_limits<float>::max();
    end_x =  std::numeric_limits<float>::min();
    end_y =  std::numeric_limits<float>::min();
    end_z =  std::numeric_limits<float>::min();

    int s = coll->NumFaces();
    for(int i=0;i<s;i++)
      {
	int p = coll->NumPoints(i);
	for(int j=0;j<p;j++)
	  {
	    Point p2 = coll->FacePoint(i,j);
	    //std::cout << p2 << std::endl;
	    handlepoint(p2);
	  }
      }
  }
  void handlepoint(Point p)
  {
    if (p.x<start_x) start_x = p.x;
    if (p.y<start_y) start_y = p.y;
    if (p.z<start_z) start_z = p.z;
    if (p.x>end_x) end_x = p.x;
    if (p.y>end_y) end_y = p.y;
    if (p.z>end_z) end_z = p.z;    
  }
  void print_bounding_box()
  {
    std::cout << start_x << " " << end_x << " " << start_y << " " << end_y << " " << start_z << " " << end_z << std::endl;
  }
  void calc_center()
  {
    center_x = start_x+(end_x-start_x)/2.0;
    center_y = start_y+(end_y-start_y)/2.0;
    center_z = start_z+(end_z-start_z)/2.0;
  }
  void calc_size()
  {
    size_x = end_x-start_x;
    size_y = end_y-start_y;
    size_z = end_z-start_z;
  }
  void calc_matrix()
  {
    Matrix m = Matrix::Translate(-center_x, -center_y, -center_z);
    
    float val = std::max(std::max(size_x, size_y), size_z);
    float val2 = 1.0/val;
    val2*= 400.0;
    Matrix m2 = Matrix::Scale(val2, val2, val2);
    Matrix mm = m * m2;
    m_m= mm;
  }

private:
  FaceCollection *coll;
  float start_x, start_y, start_z;
  float end_x, end_y, end_z;

  float center_x, center_y, center_z;
  float size_x, size_y, size_z;
  Matrix m_m;
};
GameApi::P GameApi::PolygonApi::resize_to_correct_size(P model)
{
  FaceCollection *coll = find_facecoll(e, model);
  return add_polygon2(e, new ResizeFaceCollection(coll),1);
}

class PersistentCachePoly : public FaceCollection
{
public:
  PersistentCachePoly(FaceCollection &c, std::string filename) : c(c), filename(filename),res(0) { }
  virtual void Prepare()
  {
    if (!res) {
      std::ifstream ss(filename.c_str());
      ss.tie(nullptr);
      int count = 0;
      char c2;
      while(ss.get(c2)) { count++; }
      if (count==0)
	{
	  std::cout << "Saving " << filename << std::endl;
	  SaveObjModelFaceCollection save(&c);
	  save.save(filename);
	}
      std::cout << "Loading " << filename << std::endl;
      res = new LoadObjModelFaceCollection(filename,0);
      res->Prepare();
    }
  }
  virtual int NumFaces() const 
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) return c.NumFaces();
    return res->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.NumPoints(face); }
    return res->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.FacePoint(face,point); }
    return res->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.PointNormal(face,point); }
    return res->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.Attrib(face,point,id); }
    return res->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.AttribI(face,point,id); }
    return res->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.Color(face,point); }
    return res->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.TexCoord(face,point); }
    return res->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const 
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.TexCoord3(face,point); }
    return res->TexCoord3(face,point);
  }
private:
  FaceCollection &c;
  std::string filename;
  FaceCollection *res;
};
GameApi::P GameApi::PolygonApi::persistent_cache(P p, std::string filename)
{
  FaceCollection *coll = find_facecoll(e, p);
  GameApi::P p2 = add_polygon2(e, new PersistentCachePoly(*coll, filename),1);
  GameApi::P cache = file_cache(p2, filename, 0);
  return cache;
}

class BuildOffsets : public ForwardFaceCollection
{
public:
  BuildOffsets(FaceCollection *coll, std::vector<Point> vec) : ForwardFaceCollection(*coll), coll(coll), vec(vec) { }
  Point FacePoint(int face, int point) const
  {
    int part = ForwardFaceCollection::AttribI(face,point, AttrPart);
    int s = vec.size();
    if (part>=0 && part<s)
      {
	Point p = vec[part];
	Point p2 = ForwardFaceCollection::FacePoint(face,point);
	Vector v = p2-p;
	Point p3 = v;
	return p3;
      }
    Point p0;
    p0.x = 0.0;
    p0.y = 0.0;
    p0.z = 0.0;
    return p0;
  }
private:
  FaceCollection *coll;
  std::vector<Point> vec;
};

GameApi::P GameApi::PolygonApi::build_offsets(P p, std::vector<PT> points)
{
  FaceCollection *coll = find_facecoll(e, p);
  int s = points.size();
  std::vector<Point> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(*find_point(e, points[i]));
    }
  return add_polygon2(e, new BuildOffsets(coll, vec),1);  
}


class SplitterFaceCollection : public FaceCollection
{
public:
  SplitterFaceCollection(FaceCollection &coll, int start, int end) : coll(coll), start(start), end(end) { }
  virtual void Prepare() { return coll.Prepare(); }
  virtual int NumFaces() const { return std::min(end-start,coll.NumFaces()-start); }
  virtual int NumPoints(int face) const
  {
    return coll.NumPoints(face+start);
  }
  virtual Point FacePoint(int face, int point) const { return coll.FacePoint(face+start,point); }
  virtual Vector PointNormal(int face, int point) const { return coll.PointNormal(face+start, point); }
  virtual float Attrib(int face, int point, int id) const { return coll.Attrib(face+start, point, id); }
  virtual int AttribI(int face, int point, int id) const { return coll.AttribI(face+start, point, id); }
  virtual unsigned int Color(int face, int point) const { return coll.Color(face+start, point); }
  virtual Point2d TexCoord(int face, int point) const { return coll.TexCoord(face+start,point); }
  virtual float TexCoord3(int face, int point) const { return coll.TexCoord3(face+start,point); }

private:
  FaceCollection &coll;
  int start;
  int end;
};
class TexCoordFromNormal : public ForwardFaceCollection
{
public:
  TexCoordFromNormal(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  Point2d TexCoord(int face, int point) const
  { 
    Vector n = coll->PointNormal(face,point);
    int nxt = point+2;
    nxt = nxt % coll->NumPoints(face);
    Vector n2 = coll->PointNormal(face,nxt);
    n/=n.Dist();
    n2/=n2.Dist();
    float r = sqrt(n.dx*n.dx+n.dy*n.dy+n.dz*n.dz);
    float r2 = sqrt(n2.dx*n2.dx+n2.dy*n2.dy+n2.dz*n2.dz);
    float alfa = acos(n.dz/r);
    float alfa2 = acos(n2.dz/r2);
    float beta = atan2(n.dy,n.dx);
    float beta2 = atan2(n2.dy,n2.dx);
    alfa/=3.14159265;
    beta/=2.0*3.14159265;
    alfa2/=3.14159265;
    beta2/=2.0*3.14159265;
#if 1
    while (alfa<0.0) alfa+=1.0;
    while (beta<0.0) beta+=1.0;
    while (alfa>1.0) alfa-=1.0;
    while (beta>1.0) beta-=1.0;

    while (alfa2<0.0) alfa2+=1.0;
    while (beta2<0.0) beta2+=1.0;
    while (alfa2>1.0) alfa2-=1.0;
    while (beta2>1.0) beta2-=1.0;
#endif
    if (alfa-alfa2 > 0.5) { alfa-=1.0; }
    if (beta-beta2 > 0.5) { beta-=1.0; }
    if (alfa-alfa2 < -0.5) { alfa+=1.0; }
    if (beta-beta2 < -0.5) { beta+=1.0; }
    
    Point2d p;
    p.x = alfa;
    p.y = beta;
    return p;
  }
private:
  FaceCollection *coll;
};
GameApi::P GameApi::PolygonApi::texcoord_from_normal(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new TexCoordFromNormal(coll),1);
}
class FixTexCoord : public ForwardFaceCollection
{
public:
  FixTexCoord(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) {}

  Point2d TexCoord(int face, int point) const
  {
    int prev = point - 1;
    int next = point + 1;
    if (coll->NumPoints(face)==4) { prev--; next++; }
    prev = prev % coll->NumPoints(face);
    next = next % coll->NumPoints(face);

    Point2d tx_prev = coll->TexCoord(face, prev);
    Point2d tx_curr = coll->TexCoord(face, point);
    //Point2d tx_next = coll->TexCoord(face, next);

    Vector2d v1 = tx_curr - tx_prev;
    //Vector2d v2 = tx_next - tx_curr;
     
    float dist1 = v1.Dist();
    //float dist2 = v2.Dist();
    if (dist1>0.5) {
      Point2d tx_test1 = tx_curr;
      tx_test1.x -= 1.0;
      Point2d tx_test2 = tx_curr;
      tx_test2.y -= 1.0;
      Point2d tx_test3 = tx_curr;
      tx_test3.x += 1.0;
      Point2d tx_test4 = tx_curr;
      tx_test4.y += 1.0;

      float d0 = (tx_curr - tx_prev).Dist();
      float d1 = (tx_test1 - tx_prev).Dist();
      float d2 = (tx_test2 - tx_prev).Dist();
      float d3 = (tx_test3 - tx_prev).Dist();
      float d4 = (tx_test4 - tx_prev).Dist();
      if (d0 < d1 && d0 < d2 && d0 < d3 && d0 < d4) { return tx_curr; }
      if (d1 < d2 && d1 < d3 && d1 < d4) { return tx_test1; }
      if (d2 < d1 && d2 < d3 && d2 < d4) { return tx_test2; }
      if (d3 < d1 && d3 < d2 && d3 < d4) { return tx_test3; }
      if (d4 < d1 && d4 < d2 && d4 < d3) { return tx_test4; }
    }
    return tx_curr;
  }
private:
  FaceCollection *coll;
};
GameApi::P GameApi::PolygonApi::fix_texcoord(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new FixTexCoord(coll),1);
}

GameApi::P GameApi::PolygonApi::split_p(P p, int start_face, int end_face)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SplitterFaceCollection(*coll, start_face,end_face),1);
}

class LineToCone : public FaceCollection
{
public:
  LineToCone(GameApi::Env &e, GameApi::EveryApi &ev, LineCollection *coll2, float size, int numfaces) : e(e), ev(ev), lines(coll2),size(size),numfaces(numfaces) { coll=0; }
  void Prepare() {
    lines->Prepare();
    int s = lines->NumLines();
    std::vector<GameApi::P> vec;
    for(int i=0;i<s;i++)
      {
	Point p1 = lines->LinePoint(i, 0);
	Point p2 = lines->LinePoint(i, 1);
	GameApi::PT pp1 = ev.point_api.point(p1.x,p1.y,p1.z);
	GameApi::PT pp2 = ev.point_api.point(p2.x,p2.y,p2.z);
	GameApi::P ct = ev.polygon_api.cone(numfaces, pp1, pp2, size, size);
	vec.push_back(ct);
    }
    GameApi::P res = ev.polygon_api.or_array2(vec);
    coll = find_facecoll(e,res);
  }
  virtual int NumFaces() const { if (!coll) return 0; return coll->NumFaces(); }
  virtual int NumPoints(int face) const { if (!coll) return 3; return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    if (!coll) return Point(0.0,0.0,0.0);
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (!coll) return Vector(0.0,0.0,0.0);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (!coll) return 0.0;

    return coll->Attrib(face,point,id);
  }

  virtual int AttribI(int face, int point, int id) const
  {
    if (!coll) return 0;
    return coll->AttribI(face,point,id);
  }

  virtual unsigned int Color(int face, int point) const
  {
    if (!coll) return 0x00000000;
    return coll->Color(face,point);
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    if (!coll) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    return coll->TexCoord(face,point);
  }

  virtual float TexCoord3(int face, int point) const
  {
    if (!coll) { return 0.0; }
    return coll->TexCoord3(face,point);
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  LineCollection *lines;
  FaceCollection *coll;
  float size;
  int numfaces;
};


GameApi::P GameApi::PolygonApi::line_to_cone(EveryApi &ev, LI li, float size, int numfaces)
{
  LineCollection *lines = find_line_array(e, li);
  return add_polygon2(e, new LineToCone(e,ev,lines,size,numfaces),1);
}
GameApi::P GameApi::PolygonApi::static_instancing_matrix(EveryApi &ev, P obj, MS matrix_array)
{
  MatrixArray *arr = find_matrix_array(e, matrix_array);
  int s = arr->Size();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Matrix m = arr->Index(i);
      M m2 = add_matrix2(e, m);
      P trans = matrix(obj, m2);
      vec.push_back(trans);
    }
  return or_array2(vec);
}
EXPORT GameApi::LI GameApi::PolygonApi::li_static_instancing_matrix(EveryApi &ev, LI obj, MS matrix_array)
{
  MatrixArray *arr = find_matrix_array(e, matrix_array);
  int s = arr->Size();
  std::vector<LI> vec;
  for(int i=0;i<s;i++)
    {
      Matrix m = arr->Index(i);
      M m2 = add_matrix2(e, m);
      LI trans = ev.lines_api.li_matrix(obj, m2);
      vec.push_back(trans);
    }
  return ev.lines_api.li_or_array(vec);
}

GameApi::P GameApi::PolygonApi::static_instancing(EveryApi &ev, P obj, PTS pos)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, pos);
  int s = obj2->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point pp = obj2->Pos(i);
      P trans = translate(obj, pp.x,pp.y,pp.z);
      vec.push_back(trans);
    }
  return or_array2(vec);
}
GameApi::P GameApi::PolygonApi::static_instancing_with_color(EveryApi &ev, P obj, BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  b2->Prepare();
  int sx = b2->SizeX();
  int sy = b2->SizeY();
  
  //int s = obj2->NumPoints();
  std::vector<P> vec;
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
    {
      Color c = b2->Map(x,y);
      float xx = float(x)/sx;
      float yy = float(y)/sy;
      xx*=(end_x-start_x);
      yy*=(end_y-start_y);
      xx+=start_x;
      yy+=start_y;
      Point pp(xx,yy,z);
      P trans = translate(obj, pp.x,pp.y,pp.z);
      P cc = color(trans, c.Pixel());
      vec.push_back(cc);
    }
  return or_array2(vec);  
}

GameApi::P GameApi::PolygonApi::flip_normals(P obj)
{
  FaceCollection *obj2 = find_facecoll(e, obj);
  return add_polygon2(e, new FlipNormals(*obj2),1);
}
GameApi::ARR GameApi::PolygonApi::poly_array(std::vector<P> vec)
{
  ArrayType *arr = new ArrayType;
  arr->type = 0;
  int s = vec.size();
  for(int i=0;i<s;i++) arr->vec.push_back(vec[i].id);
  return add_array(e, arr);
}
int GameApi::PolygonApi::poly_size(ARR arr)
{
  ArrayType *t = find_array(e, arr);
  return t->vec.size();
}
GameApi::P GameApi::PolygonApi::poly_index(ARR arr, int idx)
{
  ArrayType *t = find_array(e, arr);
  int s = t->vec.size();
  if (idx>=0 && idx<s)
    {
      GameApi::P p;
      p.id = t->vec[idx];
      return p;
    }
  std::cout << "Array index error!" << std::endl;
  GameApi::P p = empty();
  return p;
}

GameApi::ARR GameApi::PolygonApi::poly_execute(EveryApi &ev, ARR arr, std::string gameapi_script)
{
  ArrayType *t = find_array(e, arr);
  int s = t->vec.size();
  ArrayType *t2 = new ArrayType;
  t2->type = 0;
  for(int i=0;i<s;i++)
    {
      ExecuteEnv env;
      env.envmap["E0"] = 0;
      env.env.push_back( t->vec[i] );
      std::pair<int, std::string> p = execute_codegen(e, ev, gameapi_script, env);
      t2->vec.push_back(p.first);
    }
  return add_array(e, t2);
}

GameApi::P GameApi::PolygonApi::disc(EveryApi &ev, int numfaces, float center_x, float center_y, float center_z, float normal_x, float normal_y, float normal_z, float radius)
{
  PT center = ev.point_api.point(center_x, center_y, center_z);
  V normal = ev.vector_api.vector(normal_x, normal_y, normal_z);
  V tiny_normal = ev.vector_api.mul(normal, 0.001);
  PT center_2 = ev.point_api.move(center, tiny_normal);
  return ev.polygon_api.cone(numfaces, center, center_2, radius, 0.0001);
}

class SphNormals : public ForwardFaceCollection
{
public:
  SphNormals(FaceCollection *coll, Point pt) : ForwardFaceCollection(*coll), coll(coll), pt(pt) { }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Vector v = p - pt;
    v/=v.Dist();
    return v;
  }
private:
  FaceCollection *coll;
  Point pt;
};

GameApi::P GameApi::PolygonApi::spherical_normals(P p, float p_x, float p_y, float p_z)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SphNormals(coll, Point(p_x,p_y,p_z)),1);
}

std::string Num(float val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

// 3d position: gl_FragCoord
std::string dither_shader_string_v =
	"vec4 dither(vec4 pos)\n"
	"{\n"
	"   ex_Position = in_Position;\n"
	"   return pos;\n"
	"}\n";
std::string dither_shader_string_f =
	"vec4 dither(vec4 rgb)\n"
	"{\n"
	"   float v = gl_FragDepth;\n"
	"   return vec4(v,v,v,1.0);\n"
	"}\n";				     				 
GameApi::ML GameApi::PolygonApi::dither_shader(EveryApi &ev, ML mainloop)
{
  return custom_shader(ev,mainloop,dither_shader_string_v,dither_shader_string_f,"dither","dither");
}

std::string ConvF(float val)
{
  std::stringstream ss;
  ss << val;
  std::string s = ss.str();
  int sk = s.size();
  bool val2 = false;
  for(int i=0;i<sk;i++)
    {
      if (s[i]=='.') { val2=true; break; }
    }
  if (!val2) { s+=".0"; }
  return s;
}

GameApi::ML GameApi::PolygonApi::wave_shader(EveryApi &ev, ML mainloop, float radius, float t_mult, float x_mult, float y_mult)
{

  std::string r = ConvF(radius);
  std::string t = ConvF(t_mult);
  std::string x = ConvF(x_mult);
  std::string y = ConvF(y_mult);
  
std::string wave_v =
	"vec4 wave2(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
  std::string wave_f =
    "vec4 wave2(vec4 rgb)\n"
    "{\n"
    "  float r = " + r + ";\n"
    "  float t = " + t + "*time + ex_TexCoord.x*" + x +" + ex_TexCoord.y*" + y + ";\n"
    "  vec2 t_mx = ex_TexCoord.xy + vec2(r*cos(t),r*sin(t));\n"
    "  t_mx.x = clamp(t_mx.x,0.0,1.0);\n"
    "  t_mx.y = clamp(t_mx.y,0.0,1.0);\n"
    "   vec4 tex_mx = texture2D(tex, t_mx);	\n"
    "   return tex_mx;\n"
    "}\n";


  return custom_shader(ev, mainloop, wave_v, wave_f, "wave2", "wave2");

}

GameApi::ML GameApi::PolygonApi::blur_shader(EveryApi &ev, ML mainloop, float x_val, float y_val)
{
  //  std::stringstream ss; ss<<val;
  std::string x_ss = ConvF(x_val);
  std::string y_ss = ConvF(y_val);


std::string blur_v =
	"vec4 blur2(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
#if 0
  std::string blur_f =
	"vec4 blur2(vec4 rgb)\n"
	"{\n"
    "  vec2 t_mx = ex_TexCoord.xy + vec2(-" + ss + ",0.0);\n"
    "  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-" + ss + ");\n"
    "  vec2 t_px = ex_TexCoord.xy + vec2("+ss+ ",0.0);\n"
    "  vec2 t_py = ex_TexCoord.xy + vec2(0.0," + ss + ");\n"
	"\n"
	"   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
	"   vec4 tex_mx = texture2D(tex, t_mx);	\n"
	"   vec4 tex_my = texture2D(tex, t_my);	\n"
	"   vec4 tex_px = texture2D(tex, t_px);	\n"
	"   vec4 tex_py = texture2D(tex, t_py);\n"
	"   vec3 t1 = mix(tex_mx.rgb, tex_px.rgb, 0.5);\n"
	"   vec3 t2 = mix(tex_my.rgb, tex_py.rgb, 0.5);\n"
	"   vec3 t12 = mix(t1,t2,0.5);\n"
	"   vec3 t12t = mix(t12,tex2.rgb,0.2);\n"   	
        "   float a1 = tex2.a;\n"
        "   float a_mx = tex_mx.a;\n"
        "   float a_my = tex_my.a;\n"
        "   float a_px = tex_px.a;\n"
        "   float a_py = tex_py.a;\n"
        "   float a = max(max(max(max(a1,a_mx),a_my),a_px), a_py);\n"
	"   return vec4(t12t,a);\n"
	"}\n";
#endif

  std::string blur_f =
    "   const int stepCount = 2;\n"
    "vec4 blur2(vec4 rgb)\n"
    "{\n"
    "     float gWeights[stepCount];\n"
    "    float gOffsets[stepCount];\n"
    "    gWeights[0] = 0.44908;\n"
    "    gWeights[1] = 0.05092;\n"
    "    gOffsets[0] = 0.53805;\n"
    "    gOffsets[1] = 2.06278;\n"
    "   vec2 pixelOffset = vec2(" + x_ss + "/800.0," + y_ss + "/600.0); \n"
    "   vec3 colOut = vec3(0,0,0);\n"
    "   float alphaOut = 0.0;\n"
    "    for( int i=0; i < stepCount; i++) {\n"
    "      vec2 texCoordOffset = gOffsets[i] * pixelOffset;\n"
    "      vec4 tex1 = texture2D( tex, ex_TexCoord.xy + texCoordOffset );\n"
    "      vec4 tex2 = texture2D( tex, ex_TexCoord.xy - texCoordOffset );\n"
    "      float alpha = tex1.a + tex2.a;\n"
    "      vec3 col = tex1.xyz + \n"
    "                 tex2.xyz; \n"
    "      colOut += gWeights[i] * col; \n"
    "      alphaOut += gWeights[i] * alpha;\n"
    "     }\n"
    "     return vec4(colOut,alphaOut);\n"
    "}\n";    

  return custom_shader(ev, mainloop, blur_v, blur_f, "blur2", "blur2");
}

 static std::string shader_unique_id()
 {
   static int id = 0;
   id++;
   std::stringstream ss;
   ss << id;
   return ss.str();
 }

 class TextureManySBM : public ShaderBitmap
 {
 public:
   TextureManySBM() { m_id = shader_unique_id(); }
   virtual std::string bitmapname() const { return "sbm" + m_id; }
   virtual std::string bitmapbody() const { 
     return "vec4 sbm" + m_id + "(vec3 pos)\n"
       "{\n"
       "  if (pos.z<0.7)\n"
       "  return texture2D(texsampler[0],pos.xy);\n"
       "  if (pos.z<1.7)\n"
       "  return texture2D(texsampler[1],pos.xy);\n"
       "  if (pos.z<2.7)\n"
       "  return texture2D(texsampler[2],pos.xy);\n"
       "  if (pos.z<3.7)\n"
       "  return texture2D(texsampler[3],pos.xy);\n"
       "  if (pos.z<4.7)\n"
       "  return texture2D(texsampler[4],pos.xy);\n"
       "  if (pos.z<5.7)\n"
       "  return texture2D(texsampler[5],pos.xy);\n"
       "  if (pos.z<6.7)\n"
       "  return texture2D(texsampler[6],pos.xy);\n"
       "  if (pos.z<7.7)\n"
       "  return texture2D(texsampler[7],pos.xy);\n"
       "  if (pos.z<8.7)\n"
       "  return texture2D(texsampler[8],pos.xy);\n"
       "  if (pos.z<9.7)\n"
       "  return texture2D(texsampler[9],pos.xy);\n"
       "  if (pos.z<10.7)\n"
       "  return texture2D(texsampler[10],pos.xy);\n"
       "  return vec4(0.0,0.0,0.0,0.0);\n"
       "}";
   }
   virtual std::string bitmapbody_v_init() const { return ""; }
   virtual std::string bitmapbody_v_body() const { return "ex_TexCoord = in_TexCoord;\nreturn pos;"; }
   virtual std::string bitmapbody_f_init() const { return ""; /*uniform sampler2D texsampler[16];\n";*/ }
   virtual std::string bitmapbody_f_body() const { return ""; }
 private:
   std::string m_id;
 };


GameApi::SBM GameApi::PolygonApi::texture_sbm()
{
  return add_shader_bitmap(e, new TextureManySBM);
}

class BloomCutSbm : public ShaderBitmap
{
public:
  BloomCutSbm(ShaderBitmap *tex, float r, float g, float b) : tex(tex),r(r), g(g), b(b) { m_id = shader_unique_id(); }
  virtual std::string bitmapname() const { return "bloom_cut" + m_id; }
  virtual std::string bitmapbody() const
  {
    std::string rr = ConvF(r);
    std::string gg = ConvF(g);
    std::string bb = ConvF(b);

    return tex->bitmapbody() +
      "vec4 bloom_cut" + m_id + "(vec3 pos)\n"
      "{\n"
      "   vec4 tex2 = " + tex->bitmapname() + "(pos);\n"
      "   if (tex2.r < " + rr + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   if (tex2.g < " + gg + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   if (tex2.b < " + bb + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   return tex2;\n"
      "}\n";
  }
  virtual std::string bitmapbody_v_init() const
  {
    return tex->bitmapbody_v_init() + "";
  }
  virtual std::string bitmapbody_v_body() const
  {
    return tex->bitmapbody_v_body() + "";
  }
  virtual std::string bitmapbody_f_init() const
  {
    return tex->bitmapbody_f_init() + "";
  }
  virtual std::string bitmapbody_f_body() const
  {
    return tex->bitmapbody_f_body() + "";
  }

private:
  ShaderBitmap *tex;
  std::string m_id;
  float r,g,b;
};

 GameApi::SBM GameApi::PolygonApi::bloom_cut_sbm(SBM texture, float r, float g, float b)
{
  ShaderBitmap *tex = find_shader_bitmap(e, texture);
  return add_shader_bitmap(e, new BloomCutSbm(tex,r,g,b));
}

class BlurSbm : public ShaderBitmap
{
public:
  BlurSbm(ShaderBitmap *tex, float pixel_x, float pixel_y) : tex(tex), pixel_x(pixel_x), pixel_y(pixel_y) { m_id = shader_unique_id(); }
  virtual std::string bitmapname() const { return "blur_sbm" + m_id; }
  virtual std::string bitmapbody() const
  {
    std::string x_ss = ConvF(pixel_x);
    std::string y_ss = ConvF(pixel_y);

    return tex->bitmapbody() + 
    "vec4 blur_sbm" + m_id + "(vec3 pos)\n"
    "{\n"
    "     float gWeights[stepCount" + m_id + "];\n"
    "    float gOffsets[stepCount" + m_id + "];\n"
    "    gWeights[0] = 0.44908;\n"
    "    gWeights[1] = 0.05092;\n"
    "    gOffsets[0] = 0.53805;\n"
    "    gOffsets[1] = 2.06278;\n"
    "   vec2 pixelOffset = vec2(" + x_ss + "," + y_ss + "); \n"
    "   vec3 colOut = vec3(0,0,0);\n"
    "   float alphaOut = 0.0;\n"
    "    for( int i=0; i < stepCount" + m_id + "; i++) {\n"
    "      vec2 texCoordOffset = gOffsets[i] * pixelOffset;\n"
      "      vec4 tex1 = " + tex->bitmapname() + "( vec3(pos.xy + texCoordOffset,pos.z) );\n"
      "      vec4 tex2 = " + tex->bitmapname() + "( vec3(pos.xy - texCoordOffset,pos.z) );\n"
    "      float alpha = tex1.a + tex2.a;\n"
    "      vec3 col = tex1.xyz + \n"
    "                 tex2.xyz; \n"
    "      colOut += gWeights[i] * col; \n"
    "      alphaOut += gWeights[i] * alpha;\n"
    "     }\n"
    "     return vec4(colOut,alphaOut);\n"
    "}\n";    

  }
  virtual std::string bitmapbody_v_init() const
  {
    return tex->bitmapbody_v_init() + "";
  }
  virtual std::string bitmapbody_v_body() const
  {
    return tex->bitmapbody_v_body() + "";
  }
  virtual std::string bitmapbody_f_init() const
  {
    return tex->bitmapbody_f_init() + "   const int stepCount" + m_id + " = 2;\n";
  }
  virtual std::string bitmapbody_f_body() const
  {
    return tex->bitmapbody_f_body() + "";
  }

private:
  ShaderBitmap *tex;
  float pixel_x, pixel_y;
  std::string m_id;
};

GameApi::SBM GameApi::PolygonApi::blur_sbm(SBM texture, float pixel_x, float pixel_y)
{
  ShaderBitmap *tex = find_shader_bitmap(e, texture);
  return add_shader_bitmap(e, new BlurSbm(tex, pixel_x, pixel_y));
}
 
class CombineSbm : public ShaderBitmap
{
public:
  CombineSbm(ShaderBitmap *sbm1, ShaderBitmap *sbm2) : sbm1(sbm1), sbm2(sbm2) { m_id=shader_unique_id(); }
  virtual std::string bitmapname() const { return "combine" + m_id; }
  virtual std::string bitmapbody() const
  {
    return sbm1->bitmapbody() + sbm2->bitmapbody() +
      "vec4 combine" + m_id + "(vec3 pos)\n"
      "{\n"
      "  vec4 tex1 = " + sbm1->bitmapname() + "(pos);\n"
      "  vec4 tex2 = " + sbm2->bitmapname() + "(pos);\n"
      "  return clamp(tex1+tex2,vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));\n"
      "}\n"
      ;
  }
  virtual std::string bitmapbody_v_init() const
  {
    return sbm1->bitmapbody_v_init() + sbm2->bitmapbody_v_init();
  }
  virtual std::string bitmapbody_v_body() const
  {
    return sbm1->bitmapbody_v_body() + sbm2->bitmapbody_v_body();
  }
  virtual std::string bitmapbody_f_init() const
  {
    return sbm1->bitmapbody_f_init() + sbm2->bitmapbody_f_init();
  }
  virtual std::string bitmapbody_f_body() const
  {
    return sbm1->bitmapbody_f_body() + sbm2->bitmapbody_f_body();
  }
private:
  ShaderBitmap *sbm1, *sbm2;
  std::string m_id;
};

GameApi::SBM GameApi::PolygonApi::combine_sbm(SBM texture1, SBM texture2)
{
  ShaderBitmap *tex1 = find_shader_bitmap(e, texture1);
  ShaderBitmap *tex2 = find_shader_bitmap(e, texture2);
  return add_shader_bitmap(e, new CombineSbm(tex1, tex2));
}

GameApi::ML GameApi::PolygonApi::sbm_texture(EveryApi &ev, ML mainloop, SBM bitmap)
{
  ShaderBitmap *sbm = find_shader_bitmap(e, bitmap);
  std::string v = sbm->bitmapbody_v_init() +
    "vec4 sbmtex(vec4 pos)\n"
    "{\n" + sbm->bitmapbody_v_body() + " }\n";
  std::string f = sbm->bitmapbody_f_init() +
    sbm->bitmapbody() +
    "vec4 sbmtex(vec4 rgb) {\n"
    "  " + sbm->bitmapbody_f_body() + " "
    "  vec4 col = " + sbm->bitmapname() + "(ex_TexCoord.xyz);\n"
    "  return col;\n"
    "}\n";
  return custom_shader(ev, mainloop, v, f, "sbmtex", "sbmtex");
}

GameApi::ML GameApi::PolygonApi::bloom1_shader(EveryApi &ev, ML mainloop, float r_val, float g_val, float b_val)
{
  std::string ss_r = ConvF(r_val);
  std::string ss_g = ConvF(g_val);
  std::string ss_b = ConvF(b_val);

std::string bloom1_v =
	"vec4 bloom1(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
  std::string bloom1_f =
    "vec4 bloom1(vec4 rgb)\n"
    "{\n"
    "   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
    "   vec3 v = vec3(" + ss_r + "," + ss_g + "," + ss_b + ");\n"
    "   float val = dot(tex2.rgb, v);\n"
    "   if (val <= 1.0) rgb = vec4(0,0,0,0.0);\n"
    "   return rgb;\n"
    "}\n";


  return custom_shader(ev, mainloop, bloom1_v, bloom1_f, "bloom1", "bloom1");
}

class LogCoordsFaceCollection : public ForwardFaceCollection
{
public:
  LogCoordsFaceCollection(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    SphericalPoint sp(Point(0.0,0.0,0.0));
    sp.FromPoint(p);
    sp.r = log(sp.r);
    Point p2 = sp.ToPoint();
    return p2;
  }
};

GameApi::P GameApi::PolygonApi::log_coords(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new LogCoordsFaceCollection(coll),1);
}

GameApi::P GameApi::PolygonApi::log_coords2(P p, int x_count, int y_count, float sx, float sy, float sz)
{
  P p1 = splitquads(p, x_count,y_count);
  P p2 = log_coords(p1);
  P p3 = scale(p2,sx,sy,sz);
  return p3;
}

class SphericalWave : public ForwardFaceCollection
{
public:
  SphericalWave(FaceCollection *coll, float r1, float fr_1, float r2, float fr_2) : ForwardFaceCollection(*coll), r1(r1), r2(r2), fr_1(fr_1), fr_2(fr_2) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    SphericalPoint sp(Point(0.0,0.0,0.0));
    sp.FromPoint(p);
    sp.r = sp.r + r1*cos(fr_1*sp.alfa) + r2*cos(fr_2*sp.beta);
    Point p2 = sp.ToPoint();
    return p2;
  }
private:
  FaceCollection *coll;
  float r1,r2;
  float fr_1, fr_2;
};

GameApi::P GameApi::PolygonApi::spherical_wave(P p, float r1, float freq_1, float r2, float freq_2)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SphericalWave(coll, r1, freq_1, r2, freq_2), 1);
}

class MainLoopPosition : public MainLoopItem
{
public:
  MainLoopPosition(MainLoopItem *next) : next(next) { }
  virtual void execute(MainLoopEnv &e)
  {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    MainLoopEnv ee = e;
    ee.in_MV = e.in_MV * Matrix::Translate(viewport[0], viewport[1], 0.0);
    ee.env = e.env * Matrix::Translate(viewport[0], viewport[1], 0.0);
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};
 
GameApi::ML GameApi::PolygonApi::position_based_on_screen(ML obj)
{
  MainLoopItem *next = find_main_loop(e, obj);
  return add_main_loop(e, new MainLoopPosition(next));
}

class PlaneMap2 : public SurfaceIn3d
{
public:
  PlaneMap2(Bitmap<float> &fb, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float start_values, float end_values) : fb(fb), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), start_values(start_values), end_values(end_values) {
  }
  virtual void Prepare() { fb.Prepare(); }
  virtual Point Index(float x, float y) const
  {
    float cx = x*fb.SizeX();
    float cy = y*fb.SizeY();
    float zval = fb.Map(int(cx),int(cy));
    zval-=start_values;
    zval/=(end_values-start_values);
    zval*=(end_z-start_z);
    zval+=start_z;

    float xval = start_x+x*(end_x-start_x);
    float yval = start_y+y*(end_y-start_y);
    Point p(xval,yval,zval);
    return p;
  }
  virtual Point2d Texture(float x, float y) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
  virtual float Attrib(float x, float y, int id) const { return 0.0; }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float x, float y) const
  {
    Vector v;
    v.dx = 0.0;
    v.dy = 0.0;
    v.dz = end_z-start_z;
    return v;
  }
  virtual unsigned int Color(float x, float y) const { return 0xffffffff; }
  virtual float XSize() const { return 1.0; }
  virtual float YSize() const { return 1.0; }
private:
  Bitmap<float> &fb;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  float start_values, end_values;
};

class SphereMap2 : public SurfaceIn3d
{
public:
  SphereMap2(float p_x, float p_y, float p_z, Bitmap<float> &fb, float start_radius, float end_radius, float start_values, float end_values) : p_x(p_x), p_y(p_y), p_z(p_z), fb(fb), start_radius(start_radius), end_radius(end_radius), start_values(start_values), end_values(end_values) { }
  virtual void Prepare() { fb.Prepare(); }
  virtual Point Index(float x, float y) const
  {
    float cx = x*(fb.SizeX()-1.0);
    float cy = y*(fb.SizeY()-1.0);
    float r = fb.Map(int(cx),int(cy));
    r-=start_values;
    r/=(end_values-start_values);
    r*=(end_radius-start_radius);
    r+=start_radius;
    float angle1 = x*3.14159265;
    float angle2 = y*3.14159265*2.0;
    
    Point p;
    p.x = p_x + r*sin(angle1)*cos(angle2);
    p.y = p_y + r*sin(angle1)*sin(angle2);
    p.z = p_z + r*cos(angle1);
    return p;
  }
  virtual Point2d Texture(float x, float y) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
  virtual float Attrib(float x, float y, int id) const { return 0.0; }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float x, float y) const
  {
    Point center(p_x,p_y,p_z);
    Point p = Index(x,y);
    Vector v = p - center;
    v/=v.Dist();
    return v;
  }
  virtual unsigned int Color(float x, float y) const { return 0xffffffff; }
  virtual float XSize() const { return 1.0; }
  virtual float YSize() const { return 1.0; }
private:
  float p_x,p_y,p_z;
  Bitmap<float> &fb;
  float start_radius, end_radius;
  float start_values, end_values;
};

GameApi::S GameApi::PolygonApi::s_spherical(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values)
{
  Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
  SurfaceIn3d *surf = new SphereMap2(c_x,c_y,c_z, *fb2, start_radius, end_radius, start_values, end_values);
  SurfaceImpl impl;
  impl.surf = surf;
  return add_surface(e, impl);
}

GameApi::P GameApi::PolygonApi::s_sample(S surf, int sx, int sy)
{
  SurfaceImpl *surf2 = find_surface(e, surf);
  SurfaceIn3d *surf3 = surf2->surf;
  return add_polygon2(e, new SampleSurfaceIn3d(*surf3, 0, sx,sy),1);
}

GameApi::P GameApi::PolygonApi::sphere_map(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values, int sx, int sy)
{
  Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
  SurfaceIn3d *surf = new SphereMap2(c_x, c_y, c_z, *fb2, start_radius, end_radius, start_values, end_values);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(surf));
  FaceCollection *coll = new SampleSurfaceIn3d(*surf, 0, sx,sy);
  return add_polygon2(e, coll, 1);
}

 GameApi::P GameApi::PolygonApi::plane_map(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float start_values, float end_values, FB fb, int sx, int sy)
 {
   Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
   SurfaceIn3d *surf = new PlaneMap2(*fb2, start_x, end_x, start_y, end_y, start_z, end_z, start_values, end_values);
   ::EnvImpl *env = ::EnvImpl::Environment(&e);
   env->deletes.push_back(std::shared_ptr<void>(surf));
   FaceCollection *coll = new SampleSurfaceIn3d(*surf, 0, sx,sy);
   return add_polygon2(e, coll, 1);
 }

class CurveToPoly : public FaceCollection
{
public:
  CurveToPoly(Curve<Point> *curve, float start_x, float end_x, float start_y, float end_y, float start_angle, float end_angle, int numinstances) : curve(curve), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_angle(start_angle), end_angle(end_angle), numinstances(numinstances) {}
  void Prepare() { }
  int NumFaces() const { return numinstances; }
  int NumPoints(int face) const { return 4; }
  Point FacePoint(int face, int point) const
  {
    float p = float(face)/numinstances;
    Point2d p11 = { start_x, start_y };
    Point2d p21 = { end_x, start_y };
    Point2d p12 = { start_x, end_y };
    Point2d p22 = { end_x, end_y };
    float angle = start_angle+p*(end_angle-start_angle);
    Point2d rot_p11 = { float(cos(angle)*p11.x) -float(sin(angle)*p11.y), float(sin(angle)*p11.x)+float(cos(angle)*p11.y) };
    Point2d rot_p12 = { float(cos(angle)*p12.x -sin(angle)*p12.y), float(sin(angle)*p12.x+cos(angle)*p12.y) };
    Point2d rot_p21 = { float(cos(angle)*p21.x -sin(angle)*p21.y), float(sin(angle)*p21.x+cos(angle)*p21.y) };
    Point2d rot_p22 = { float(cos(angle)*p22.x -sin(angle)*p22.y), float(sin(angle)*p22.x+cos(angle)*p22.y) };
    Point curve_pos = curve->Index(p*curve->Size());
    switch(point) {
    case 0: return curve_pos+Vector(rot_p11.x, rot_p11.y, 0.0);
    case 1: return curve_pos+Vector(rot_p12.x, rot_p12.y, 0.0);
    case 2: return curve_pos+Vector(rot_p22.x, rot_p22.y, 0.0);
    case 3: return curve_pos+Vector(rot_p21.x, rot_p21.y, 0.0);
    };
    return curve_pos;
  }
  Vector PointNormal(int face, int point) const { return Point(0.0,0.0,1.0); }
  float Attrib(int face, int point, int id) const { return 0.0; }
  int AttribI(int face, int point, int id) const { return 0; }
  unsigned int Color(int face, int point) const { return 0xffffffff; }
  Point2d TexCoord(int face, int point) const { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
  float TexCoord3(int face, int point) const { return 0.0; }
private:
  Curve<Point> *curve;
  float start_x, end_x;
  float start_y, end_y;
  float start_angle, end_angle;
  int numinstances;
};

GameApi::P GameApi::PolygonApi::curve_to_poly(C c, float start_x, float end_x, float start_y, float end_y, float start_angle, float end_angle, int numinstances)
{
  Curve<Point> *curve = find_curve(e, c);
  return add_polygon2(e, new CurveToPoly(curve, start_x, end_x, start_y, end_y, start_angle, end_angle, numinstances),1);
}
