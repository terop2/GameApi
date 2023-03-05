
#include "GameApi_h.hh"


bool is_inside_triangle(Point2d tri_1, Point2d tri_2, Point2d tri_3, Point2d test_point)
{
  Point2d p1 = tri_1;
  Point2d p2 = tri_2;
  Point2d p3 = tri_3;
  Point2d p = test_point;

  float alpha = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
        ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float beta = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
    ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float gamma = 1.0f - alpha - beta;

  if (alpha < 0.0) return false;
  if (beta < 0.0) return false;
  if (gamma< 0.0) return false;
  return true;
}

class Lines : public Line2dCollection
{
public:
  Lines(FacesInPlane *coll, int face, bool *enable_array) : coll(coll), face(face), enable_array(enable_array) { }
  int Size() const {
    int s = coll->NumPoints(face);
    int count = 0;
    for(int i=0;i<s;i++)
      {
	if (enable_array[i]==true) count++;
      }
    return count;
  }
  Line2d Index(int i) const {
    int s = coll->NumPoints(face);
    int count = 0;
    int ii=0;
    for(;ii<s;i++)
      {
	if (count==i) break;
	if (enable_array[ii]==true) count++;
      }
    int j = ii+1;
    if (j>=s) j-=s;
    while(enable_array[j]==false)
      {
      j++;
      if (j>=s) j-=s;
      }
    Line2d l;
    l.p1 = coll->Map(face,ii);
    l.p2 = coll->Map(face,j);
    return l;
  }
private:
  FacesInPlane *coll;
  int face;
  bool *enable_array;
};

bool is_point_inside_polygon(FacesInPlane *coll, int face, Point2d p, bool *enable_array)
{
#if 0
  Lines ll(coll, face,enable_array);
  PolygonFill pf(10000.0,10000.0, ll);
  bool b = pf.Map(p.x,p.y);
  //std::cout << "PolygonFill: " << b << std::endl;
  return b;
#endif
}

bool is_line_segment_inside_polygon(FacesInPlane *coll, int face, Point2d line_1, Point2d line_2, bool *enable_array)
{
  for(int i=0;i<80;i++)
    {
      float v = i/80.0;
      Point2d val = (1.0-v)*line_1 + v*line_2;
      bool b = is_point_inside_polygon(coll, face, val,enable_array);
      if (!b) { return false; }
    }
  return true;
}

bool is_ear(FacesInPlane *coll, int face, int point, bool *enable_array)
{
#if 0
  int s = coll->NumPoints(face);
  int prev = point;
  prev--;
  prev = prev % s;
  while(enable_array[prev]==false) {
    prev--;
    prev = prev % s;
  }
  int next = point;
  next++;
  next = next % s;
  while(enable_array[next]==false) {
    next++;
    next=next % s;
  }
#endif

  int s = coll->NumPoints(face);
  int prev = point;
  prev--;
  while(prev<0)
    prev+=s;
  while(enable_array[prev]==false) {
    prev--;
    while(prev<0)
      prev+=s;
  }
  int next = point;
  next++;
  while(next>=s)
    next-=s;
  while(enable_array[next]==false) {
    next++;
    while(next>=s)
      next-=s;
  }

  std::swap(next,prev);

  //int prev = point-1;
  //int next = point+1;
  //int s = coll->NumPoints(face);
  //prev = prev % s;
  //next = next % s;
  Point2d p_prev = coll->Map(face,prev);
  Point2d p_curr = coll->Map(face,point);
  Point2d p_next = coll->Map(face,next);

  float val = (p_curr.x - p_prev.x) * (p_next.y - p_prev.y) - (p_curr.y-p_prev.y) * (p_next.x-p_prev.x);
  if (0.0 >= val) return false;

  
#if 0
  //Vector2d v1 = p_prev - p_curr;
  //Vector2d v2 = p_next - p_curr;
  //float d = Vector2d::DotProduct(v1,v2);
  //d/=v1.Dist()*v2.Dist();
  //float angle = acos(d);
  if (angle<0.0 || angle>3.14159265)
    {
      return false;
    }
#endif

#if 0
  if (!is_line_segment_inside_polygon(coll, face, p_prev, p_next,enable_array))
    {
      return false;
    }
#endif
  if (enable_array[point]==false)
    {
      return false;
    }

  //return true;
  bool ear = true;
  for(int i=0;i<s;i++) {
    if (enable_array[i]==true)
    if (i!=prev && i!=next && i!=point)
      {
	bool is_inside = is_inside_triangle(p_prev, p_curr, p_next, coll->Map(face,i));

	if (is_inside) { ear = false; break; }
      }
  }
  return ear;
}

int find_ear(FacesInPlane *coll, int face, bool *enable_array)
{
  int s = coll->NumPoints(face);
  for(int i=0;i<s;i++)
    {
      if (enable_array[i]) {
	bool b = is_ear(coll, face,i, enable_array);
	if (b) return i;
      }
    }
  // should never happen
  std::cout << "find_ear failed! code broken" << std::endl;
  return 0;
}

void append_tri(std::vector<int> &vec, FacesInPlane *coll, int face, int point, bool *enable_array)
{
  int s = coll->NumPoints(face);
  int prev = point;
  prev--;
  while(prev<0)
    prev+=s;
  while(enable_array[prev]==false) {
    prev--;
    while(prev<0)
      prev+=s;
  }
  int next = point;
  next++;
  while(next>=s)
    next-=s;
  while(enable_array[next]==false) {
    next++;
    while(next>=s)
      next-=s;
  }
  
  vec.push_back(prev);
  vec.push_back(point);
  vec.push_back(next);
  std::cout << prev << ":" << point << ":" << next << std::endl;
}

std::vector<int> triangulate(FacesInPlane *coll, int face)
{
  int s = coll->NumPoints(face);
  if (!s) return std::vector<int>();
  std::vector<int> vec;
  bool *enable_array = new bool[s];
  for(int i=0;i<s;i++) enable_array[i] = true;
  int count = 0;
  while(1) {
    int val = find_ear(coll,face, enable_array);
    append_tri(vec, coll, face, val, enable_array);
    count++;
    if (count > s-2) break;
    enable_array[val]=false;
  }
  delete [] enable_array;
  return vec;
}
bool is_curve_ccw(FacesInPlane &f, int face)
{
  int s = f.NumPoints(face);
  int c = 0;
  for(int i=0;i<s;i++)
    {
      Point2d p = f.Map(face,i);
      int ii = i+1;
      if (ii>=s) ii-=s;
      Point2d p2 = f.Map(face,ii);
      Point pp1 = { p.x, p.y,0.0 };
      Point pp2 = { p2.x,p2.y,0.0 };
      Matrix m = Matrix::ZRotation(0.3); // this deals with 90 degrees angles
      Point ppp1 = pp1*m;
      Point ppp2 = pp2*m;
      c+=(ppp2.x-ppp1.x)*(ppp2.y+ppp1.y);
    }
  return c<0.0;
}

class ReverseFaces : public FacesInPlane
{
public:
  ReverseFaces(FacesInPlane *faces) : faces(faces) { }
  virtual int Size() const { return faces->Size(); }
  virtual int NumPoints(int face) const
  {
    return faces->NumPoints(face);
  }
  virtual Point2d Map(int face, int point) const
  {
    return faces->Map(face,NumPoints(face)-point-1);
  }
  
  virtual Point2d TexCoord(int face, int point) const
  {
    return faces->TexCoord(face,NumPoints(face)-point-1);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return faces->Color(face,NumPoints(face)-point-1);
  }

private:
  FacesInPlane *faces;
};


class Triangulate_plane : public FacesInPlane
{
public:
  Triangulate_plane(FacesInPlane *coll) : coll(coll)
  {
    int s = coll->Size();
    for(int i=0;i<s;i++)
      {
	//bool b = is_curve_ccw(*coll,i);
	FacesInPlane *c = coll;
	//ReverseFaces c2(c);
	//if (b) { c = &c2; }
	vec.push_back(triangulate(c,i));
      }
  }
  virtual int Size() const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++) count+=vec[i].size()/3;
    return count;
  }
  virtual int NumPoints(int face) const
  {
    return 3;
  }
  virtual Point2d Map(int face, int point) const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	if (face<int(count+vec[i].size()/3))
	  {
	    int index = (face-count)*3;
	    int idx = vec[i][index+point];
	    return coll->Map(face,idx);
	  }
	count+=vec[i].size()/3;
      }
    std::cout << "Bad index!" << std::endl;
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	if (face<int(count+vec[i].size()/3))
	  {
	    int index = (face-count)*3;
	    int idx = vec[i][index+point];
	    return coll->TexCoord(face,idx);
	  }
	count+=vec[i].size()/3;
      }
    std::cout << "Bad index!" << std::endl;
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;

  }
  virtual unsigned int Color(int face, int point) const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	if (face<int(count+vec[i].size()/3))
	  {
	    int index = (face-count)*3;
	    int idx = vec[i][index+point];
	    return coll->Color(face,idx);
	  }
	count+=vec[i].size()/3;
      }
    std::cout << "Bad index!" << std::endl;

    return 0xffffffff;

  }
private:
  FacesInPlane *coll;
  std::vector<std::vector<int> > vec;
};


GameApi::PLF GameApi::NewPlaneApi::triangulate(PLF faces)
{
  FacesInPlane *face = find_plane_faces(e, faces);


  return add_plane_faces(e, new Triangulate_plane(face));
}

class PLFaces : public FaceCollection
{
public:
  PLFaces(FacesInPlane *pl) : pl(pl) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  virtual void Prepare() { }
  virtual int NumFaces() const { return pl->Size(); }
  virtual int NumPoints(int face) const { return pl->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point2d p = pl->Map(face,point); 
    return Point(p.x,p.y,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return pl->Normal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return pl->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return pl->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }

private:
  FacesInPlane *pl;
  
};



GameApi::PLF GameApi::NewPlaneApi::reverse_faces(PLF faces)
{
  FacesInPlane *pl = find_plane_faces(e, faces);
  return add_plane_faces(e, new ReverseFaces(pl));
}

GameApi::P GameApi::NewPlaneApi::to_polygon(PLF faces)
{
  FacesInPlane *pl = find_plane_faces(e, faces);
  return add_polygon2(e, new PLFaces(pl),1);
}
