
#include "GameApi_h.hh"

unsigned int swap_color(unsigned int c);

class ColorLineCollection : public LineCollection
{
public:
  ColorLineCollection(LineCollection *coll, unsigned int color1, unsigned int color2) : coll(coll), color1(color1), color2(color2) { }
  virtual int NumLines() const { return coll->NumLines(); }
  virtual Point LinePoint(int line, int point) const { return coll->LinePoint(line,point); }
  virtual unsigned int LineColor(int line, int point) const { if (point==0) return color1; else return color2; }
private:
  LineCollection *coll;
  unsigned int color1;
  unsigned int color2;
};
EXPORT GameApi::LI GameApi::LinesApi::change_color(GameApi::LI li, unsigned int color)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new ColorLineCollection(lines, color, color));
}
EXPORT GameApi::LI GameApi::LinesApi::change_color(GameApi::LI li, unsigned int color1, unsigned int color2)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new ColorLineCollection(lines, color1, color2));
}
class ColorFunctionLines : public LineCollection
{
public:
  ColorFunctionLines(LineCollection *coll, std::function<unsigned int(int linenum, bool id)> f) : coll(coll), f(f) { }
  int NumLines() const { return coll->NumLines(); }
  Point LinePoint(int line, int point) const { return coll->LinePoint(line,point); }
  unsigned int LineColor(int line, int point) const
  {
    return f(line, point==1);
  }
private:
  LineCollection *coll;
  std::function<unsigned int(int linenum, bool id)> f;
};
EXPORT GameApi::LI GameApi::LinesApi::color_function(LI lines, std::function<unsigned int(int linenum, bool id)> f)
{
  LineCollection *lines2 = find_line_array(e, lines);
  return add_line_array(e, new ColorFunctionLines(lines2, f));
}
class LineCollectionFunction : public LineCollection
{
public:
  LineCollectionFunction(GameApi::Env &e, std::function<GameApi::PT (int linenum, bool id)> f, int numlines) :  ev(e), f(f), numlines(numlines) { }
  virtual int NumLines() const { return numlines; }
  virtual Point LinePoint(int line, int point) const
  {
    bool b = point==0 ? 0 : 1;
    GameApi::PT pt = f(line, b);
    float x = ev.point_api.pt_x(pt);
    float y = ev.point_api.pt_y(pt);
    float z = ev.point_api.pt_z(pt);
    return Point(x,y,z);
  }

private:
  //GameApi::Env &e;
  mutable GameApi::EveryApi ev;
  std::function<GameApi::PT (int linenum, bool id)> f;
  int numlines; 
};


EXPORT GameApi::LI GameApi::LinesApi::function(std::function<GameApi::PT (int linenum, bool id)> f, int numlines)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);

  return add_line_array(e, new LineCollectionFunction(e, f, numlines));
}
class LinesFromPlane : public LineCollection
{
public:
  LinesFromPlane(PlanePoints2d *plane) : plane(plane) { }
  virtual int NumLines() const {
    int s = plane->Size();
    int line=0;
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	switch(t) {
	case PlanePoints2d::EMove:
	  break;
	case PlanePoints2d::ELineTo:
	  line++;
	  break;
	case PlanePoints2d::EConic:
	case PlanePoints2d::ECubic:
	  std::cout << "Use remove_splines() first before doing from_plane()" << std::endl;
	  break;
	};
      }
    return line;
  }
  virtual Point LinePoint(int line_num, int point) const 
  {
    int s = plane->Size();
    int line=0;
    Point2d pos = { 0.0, 0.0 };
    for(int i=0;i<s;i++)
      {
	PlanePoints2d::PlanePointsType t = plane->Type(i);
	switch(t) {
	case PlanePoints2d::EMove:
	  pos = plane->Map(i);
	  break;
	case PlanePoints2d::ELineTo:
	  if (line_num == line && point==0) {
	    Point p(pos.x, pos.y, 0.0);
	    return p;
	  }
	  if (line_num == line && point==1) {
	    Point2d pp = plane->Map(i);
	    Point p(pp.x, pp.y, 0.0);
	    return p;
	  }
	  pos = plane->Map(i);
	  line++;
	  break;
	case PlanePoints2d::EConic:
	case PlanePoints2d::ECubic:
	  std::cout << "Use remove_splines() first before doing from_plane()" << std::endl;
	  break;
	};
      }
    Point p(pos.x, pos.y, 0.0);
    return p;
  }
private:
  PlanePoints2d *plane;
};
EXPORT GameApi::LI GameApi::LinesApi::from_plane(GameApi::PL plane)
{
  PlanePoints2d *plane2 = find_plane(e,plane);
  return add_line_array(e, new LinesFromPlane(plane2));
}
EXPORT GameApi::LI GameApi::LinesApi::from_points(GameApi::PC points, bool loops)
{
  PointCollection *point_coll = find_pointcoll_array(e,points);
  return add_line_array(e, new ContinuousLines(point_coll, loops));
}
EXPORT GameApi::LI GameApi::LinesApi::from_polygon2(GameApi::P poly1, GameApi::P poly2)
{
  FaceCollection *p1 = find_facecoll(e, poly1);
  FaceCollection *p2 = find_facecoll(e, poly2);
  return add_line_array(e, new AnimLines(p1, p2));
}
class SliceLineCollection : public LineCollection
{
public:
  SliceLineCollection(FaceCollection *coll, Point pos, Vector u_x, Vector u_y) : coll(coll), pos(pos), u_x(u_x), u_y(u_y) { }
  virtual int NumLines() const { return coll->NumFaces(); }
  virtual Point LinePoint(int line, int point) const 
  {
    Point p1 = coll->FacePoint(line, 0);
    Point p2 = coll->FacePoint(line, 1);
    Point p3 = coll->FacePoint(line, 2);
    Plane pl(pos, u_x, u_y);
    Point2d res1,res2;
    bool b = pl.TriangleIntersection(p1,p2,p3, res1,res2);
    if (b)
      {
	if (point==0) { return Point(res1.x, res1.y, 0.0); }
	if (point==1) { return Point(res2.x, res2.y, 0.0); }
      }
    else
      {
	return Point(0.0,0.0,0.0);
      }
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }
private:
  FaceCollection *coll;
  Point pos;
  Vector u_x, u_y;
};
class NormalsLineCollection : public LineCollection
{
public:
  NormalsLineCollection(FaceCollection *coll, float length) : coll(coll),length(length) { }
  virtual int NumLines() const { return coll->NumFaces(); }
  virtual Point LinePoint(int line, int point) const 
  {
    int s = coll->NumPoints(line);
    Point avg(0.0, 0.0, 0.0);
    for(int i=0;i<s;i++)
      {
	Point p = coll->FacePoint(line, i);
	avg += Vector(p);
      }
    Vector avg2 = avg;
    avg2/=float(s);
    if (point==0)

      {
	return Point(avg2);
      }
    if (point==1)
      {
	Vector v = coll->PointNormal(line, 0);
	v/=v.Dist();
	v*=length;
	return Point(avg2)+v;
      }
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }  
private:
  FaceCollection *coll;
  float length;
};
EXPORT GameApi::LI GameApi::LinesApi::normals_from_polygon(GameApi::P p, float length)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_line_array(e, new NormalsLineCollection(coll, length));
}
class MatrixLineCollection : public LineCollection
{
public:
  MatrixLineCollection(Matrix m, LineCollection &coll) : m(m),coll(coll) { }
  virtual int NumLines() const { return coll.NumLines(); }
  virtual Point LinePoint(int line, int point) const 
  {
    return coll.LinePoint(line,point)*m;
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }  

private:
  Matrix m;
  LineCollection &coll;
};
EXPORT GameApi::LI GameApi::LinesApi::translate(LI lines, float dx, float dy, float dz)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::Translate(dx,dy,dz),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::rotx(LI lines, float angle)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::XRotation(angle),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::roty(LI lines, float angle)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::YRotation(angle),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::rotz(LI lines, float angle)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::ZRotation(angle),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::scale(LI lines, float dx, float dy, float dz)
{
  LineCollection *c = find_line_array(e, lines);
  return add_line_array(e, new MatrixLineCollection(Matrix::Scale(dx,dy,dz),*c));
}
EXPORT GameApi::LI GameApi::LinesApi::render_slice_2d(GameApi::P p, GameApi::PT pos, GameApi::V u_x, GameApi::V u_y)
{
  FaceCollection *coll = find_facecoll(e, p);
  Point *pp1 = find_point(e, pos);
  Vector *v1 = find_vector(e, u_x);
  Vector *v2 = find_vector(e, u_y);
  return add_line_array(e, new SliceLineCollection(coll,*pp1, *v1, *v2));
}

class LineAnim : public ForwardFaceCollection
{
public:
  LineAnim(FaceCollection &p, LineCollection &c, float val) : ForwardFaceCollection(p), p(p), c(c), val(val) { }
  Point FacePoint(int face, int point) const
  {
    int count = 0;
    for(int i=0;i<face;i++) 
      {
	count+=p.NumPoints(i);
      }
    count+=point;
    Point p1 = c.LinePoint(count, 0);
    Point p2 = c.LinePoint(count, 1);
    return Point((1.0-val)*Vector(p1)+val*Vector(p2));
  }
private:
  FaceCollection &p;
  LineCollection &c;
  float val;
};
EXPORT GameApi::P GameApi::LinesApi::line_anim(GameApi::P poly, GameApi::LI lines, float val)
{
  FaceCollection *p = find_facecoll(e, poly);
  LineCollection *c = find_line_array(e, lines);
  return add_polygon(e, new LineAnim(*p, *c, val),1);
}
EXPORT GameApi::LI GameApi::LinesApi::from_polygon(GameApi::P poly)
{
  FaceCollection *poly2 = find_facecoll(e, poly);
  return add_line_array(e, new OutlineFaces(*poly2));
}
class BorderFromBoolBitmap : public LineCollection
{
public:
  BorderFromBoolBitmap(Bitmap<bool> &bm, float start_x, float end_x, float start_y, float end_y, float z) : bm(bm),start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { Store(); }
  virtual int NumLines() const { return p1.size(); }
  virtual Point LinePoint(int line, int point) const
  {
    if (point==0) { return p1[line]; }
    return p2[line];
  }

  void Store() {
    int sx = bm.SizeX();
    int sy = bm.SizeY();
    for(int x=0;x<sx-1;x++)
      {
	for(int y=0;y<sy-1;y++)
	  {
	    bool b00 = bm.Map(x,y);
	    bool b01 = bm.Map(x+1,y);
	    bool b10 = bm.Map(x,y+1);
	    bool b11 = bm.Map(x+1,y+1);

	    float xx0 = start_x + (end_x-start_x)*x/sx;
	    float xx1 = start_x + (end_x-start_x)*(x+1)/sx;
	    float xx2 = start_x + (end_x-start_x)*(x+2)/sx;

	    float yy0 = start_y + (end_y-start_y)*y/sy;
	    float yy1 = start_y + (end_y-start_y)*(y+1)/sy;
	    float yy2 = start_y + (end_y-start_y)*(y+2)/sy;

	    if ((b00 || b01) && !(b00 && b01)) // TOPLEFT-TOPRIGHT
	      {
		p1.push_back(Point(xx1,yy0,z));
		p2.push_back(Point(xx1,yy1,z));
	      }
	    if ((b00 || b10) && !(b00 && b10)) // topleft-bottomleft
	      {
		p1.push_back(Point(xx0,yy1,z));
		p2.push_back(Point(xx1,yy1,z));
	      }
	    if ((b10||b11) && !(b10 && b11)) // bottomleft - bottomright
	      {
		p1.push_back(Point(xx1,yy1,z));
		p2.push_back(Point(xx1,yy2,z));
	      }
	    if ((b01 || b11) && !(b01 && b11)) // topright - bottomright
	      {
		p1.push_back(Point(xx1,yy1,z));
		p2.push_back(Point(xx2,yy1,z));
	      }
		    
	  }
      }
  }

private:
  Bitmap<bool> &bm;
  float start_x, end_x;
  float start_y, end_y;
  float z;
  std::vector<Point> p1;
  std::vector<Point> p2;
};

EXPORT GameApi::LI GameApi::LinesApi::border_from_bool_bitmap(GameApi::BB b, float start_x, float end_x, float start_y, float end_y, float z)
{
  BoolBitmap *bb2 = find_bool_bitmap(e, b);
  Bitmap<bool> *bb = bb2->bitmap;
  return add_line_array(e, new BorderFromBoolBitmap(*bb, start_x, end_x, start_y, end_y, z));
}
class LI_Render : public MainLoopItem
{
public:
  LI_Render(GameApi::LinesApi &api, GameApi::LLA l) : api(api), l(l) { }
  void execute() {
    api.render(l);
  }
private:
  GameApi::LinesApi &api;
  GameApi::LLA l;
};
EXPORT GameApi::ML GameApi::LinesApi::render_ml(LLA l)
{
  return add_main_loop(e, new LI_Render(*this, l));
}
EXPORT void GameApi::LinesApi::render(LLA l)
{
  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);
#ifdef VAO
  glBindVertexArray(array->vao[0]);
#else
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, array->buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, array->buffer);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, array->buffer2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,0);
#endif
  glDrawArrays(GL_LINES, 0, array->numpoints);
#ifndef VAO
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
#endif
}

#if 0
void GameApi::LinesApi::render(float *array, int size)
{
  size/=6;
  GLuint vao = 0;
  GLuint buffer = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, size*2*sizeof(float)*3, array, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, size);

  glDeleteVertexArrays( 1, &vao );
  glDeleteBuffers( 1, &buffer);
}
#endif

#if 0
unsigned int *GameApi::LinesApi::color_access(LLA lines, int line, bool b)
{
  PointArray2 *ee = find_lines_array(e, lines);
  int pos = b ? 1 : 0;
  return &ee->color_array[line*2+pos];
}
int GameApi::LinesApi::line_count(LLA lines)
{
  PointArray2 *ee = find_lines_array(e, lines);
  return ee->numpoints;
}
float *GameApi::LinesApi::line_access(LLA lines, int line, bool b)
{
  PointArray2 *ee = find_lines_array(e, lines);
  int pos = b ? 1 : 0;
  return &ee->array[line*2*3+pos*3];
}
void GameApi::LinesApi::update(LLA lines)
{
  PointArray2 *arr = find_lines_array(e, lines);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, GL_STATIC_DRAW);
}
#endif
class LI_Update : public MainLoopItem
{
public:
  LI_Update(GameApi::LinesApi &api, GameApi::LLA la, GameApi::LI li) : api(api), la(la), li(li) { }
  void execute() { api.update(la,li); }
private:
  GameApi::LinesApi &api;
  GameApi::LLA la;
  GameApi::LI li;
};
EXPORT GameApi::ML GameApi::LinesApi::update_ml(LLA la, LI l)
{
  return add_main_loop(e, new LI_Update(*this, la, l));
}
EXPORT void GameApi::LinesApi::update(LLA la, LI l)
{
  LineCollection *coll = find_line_array(e, l);
  int count = coll->NumLines();
  float *array = 0;
  unsigned int *color_array = 0;
  if (count > 0) {
    array = new float[count*6];
    color_array = new unsigned int[count*2];
    for(int i=0;i<count;i++)
      {
	Point p = coll->LinePoint(i,0);
	Point p2 = coll->LinePoint(i,1);
	unsigned int color1 = coll->LineColor(i,0);
	unsigned int color2 = coll->LineColor(i,1);
	//std::cout << std::hex << color1 << ";" << std::hex << color2 << std::endl;
	array[i*6+0] = p.x;
	array[i*6+1] = p.y;
	array[i*6+2] = p.z;
	array[i*6+3] = p2.x;
	array[i*6+4] = p2.y;
	array[i*6+5] = p2.z;
	color_array[i*2+0] = color1;
	color_array[i*2+1] = color2;
	//std::cout << i << ":" << "(" << p.x << "," << p.y << "," << p.z << ")-(" << p2.x<< "," << p2.y << "," << p2.z << ")" << std::endl;
      }
  }
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->color_array = color_array;
  arr->numpoints = count*2;
#ifdef VAO
  glGenVertexArrays(1, arr->vao);
  glBindVertexArray(arr->vao[0]);
#endif
  glGenBuffers(1, &arr->buffer);
  glGenBuffers(1, &arr->buffer2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, GL_STATIC_DRAW);
#ifdef VAO
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,0);
#endif
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(2);

  return add_update_lines_array(e, la, arr);

}
EXPORT GameApi::LLA GameApi::LinesApi::prepare(LI l)
{
  LineCollection *coll = find_line_array(e, l);
  int count = coll->NumLines();
  float *array = 0;
  unsigned int *color_array = 0;
  if (count > 0) {
    array = new float[count*6];
    color_array = new unsigned int[count*2];
    for(int i=0;i<count;i++)
      {
	Point p = coll->LinePoint(i,0);
	Point p2 = coll->LinePoint(i,1);
	unsigned int color1 = coll->LineColor(i,0);
	unsigned int color2 = coll->LineColor(i,1);
	//std::cout << std::hex << color1 << ";" << std::hex << color2 << std::endl;
	array[i*6+0] = p.x;
	array[i*6+1] = p.y;
	array[i*6+2] = p.z;
	array[i*6+3] = p2.x;
	array[i*6+4] = p2.y;
	array[i*6+5] = p2.z;
	color_array[i*2+0] = swap_color(color1);
	color_array[i*2+1] = swap_color(color2);
	//std::cout << i << ":" << "(" << p.x << "," << p.y << "," << p.z << ")-(" << p2.x<< "," << p2.y << "," << p2.z << ")" << std::endl;
      }
  }
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->color_array = color_array;
  arr->numpoints = count*2;
#ifdef VAO
  glGenVertexArrays(1, arr->vao);
  glBindVertexArray(arr->vao[0]);
#endif
  glGenBuffers(1, &arr->buffer);
  glGenBuffers(1, &arr->buffer2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, GL_STATIC_DRAW);
#ifdef VAO
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,0);
#endif
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(2);

  return add_lines_array(e, arr);
}