
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
  LI_Render(GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LLA l) : ev(ev), api(api), l(l) { }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e) {
    GameApi::SH sh;
    sh.id = e.sh_color;
    ev.shader_api.use(sh);
    api.render(l);
  }
  int shader_id() { return -1; }

private:
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LLA l;
};

class LI_Render_Inst : public MainLoopItem
{
public:
  LI_Render_Inst(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LLA l, GameApi::PTA pta) : env(env), ev(ev), api(api), l(l),pta(pta) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e) {
    GameApi::SH sh;
    sh.id = e.sh_color;
    GameApi::US u_v,u_f;
    u_v.id = 0;
    u_f.id = 0;

    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
   
	u_v = ev.uber_api.v_colour(u_v);
	u_v = ev.uber_api.v_light(u_v);
	u_f = ev.uber_api.f_colour(u_f);
	u_f = ev.uber_api.f_light(u_f);
      }

    if (firsttime)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = ev.uber_api.v_inst(vertex);
	//GameApi::US fragment2 = ev.uber_api.f_inst(fragment);
	shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment);
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
	sh = shader;
      }

    ev.shader_api.use(sh);
    if (firsttime)
      {
	api.prepare_inst(l,pta);
	firsttime=false;
      }
    api.render_inst(l,pta);
  }
  int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LLA l;
  GameApi::PTA pta;
  bool firsttime;
  GameApi::SH shader;
};

EXPORT GameApi::ML GameApi::LinesApi::render_ml(EveryApi &ev, LLA l)
{
  return add_main_loop(e, new LI_Render(ev, *this, l));
}
EXPORT GameApi::ML GameApi::LinesApi::render_inst_ml(EveryApi &ev, LLA l, PTA pta)
{
  return add_main_loop(e, new LI_Render_Inst(e, ev, *this, l, pta));
}
EXPORT void GameApi::LinesApi::prepare_inst(LLA l, PTA instances)
{
  PointArray2 *array = find_lines_array(e,l);
  PointArray3 *arr = find_point_array3(e, instances);

  float *positions = arr->array;
  int size = arr->numpoints;

#ifdef VAO
  glBindVertexArray(array->vao[0]);
#endif
  glBindBuffer( GL_ARRAY_BUFFER, array->pos_buffer );

  glBufferData( GL_ARRAY_BUFFER, sizeof(Point) * size, positions, GL_DYNAMIC_DRAW);

}
EXPORT void GameApi::LinesApi::render_inst(LLA l, PTA instances)
{
  PointArray3 *arr = find_point_array3(e, instances);
  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);

  float *positions = arr->array;
  int size = arr->numpoints;
  // INSTANCED RENDERING
  glBindBuffer( GL_ARRAY_BUFFER, array->pos_buffer );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);

  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);
  // END OF INSTANCED RENDERING
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

  // INSTANCED DRAWING
  glBindBuffer( GL_ARRAY_BUFFER, array->pos_buffer );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(5);
  // END INSTANCED

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);


  glDrawArraysInstanced(GL_LINES, 0, array->numpoints, size);
#ifndef VAO
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
#endif
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
  glEnableVertexAttribArray(4);
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
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e) { api.update(la,li); }
  int shader_id() { return -1; }

private:
  GameApi::LinesApi &api;
  GameApi::LLA la;
  GameApi::LI li;
};
EXPORT GameApi::ML GameApi::LinesApi::update_ml(LLA la, LI l)
{
  return add_main_loop(e, new LI_Update(*this, la, l));
}
class LineAnim2 : public PointsApiPoints
{
public:
  LineAnim2(LineCollection *coll, float val) : coll(coll), val(val) { }
  virtual int NumPoints() const { return coll->NumLines(); }
  virtual Point Pos(int i) const
  {
    Point p1 = coll->LinePoint(i, 0);
    Point p2 = coll->LinePoint(i, 1);
    return Point(Vector(p1)*val + Vector(p2)*(1.0-val));
  }
  virtual unsigned int Color(int i) const
  {
    unsigned int c1 = coll->LineColor(i, 0);
    unsigned int c2 = coll->LineColor(i, 1);
    return Color::Interpolate(c1, c2, val);
  }
private:
  LineCollection *coll;
  float val;
};

class LineAnim3 : public PointsApiPoints
{
public:
  LineAnim3(LineCollection *coll, std::function<float(int)> f) : coll(coll), f(f) { }
  virtual int NumPoints() const { return coll->NumLines(); }
  virtual Point Pos(int i) const
  {
    Point p1 = coll->LinePoint(i, 0);
    Point p2 = coll->LinePoint(i, 1);
    float val = f(i);
    return Point(Vector(p1)*val + Vector(p2)*(1.0-val));
  }
  virtual unsigned int Color(int i) const
  {
    unsigned int c1 = coll->LineColor(i, 0);
    unsigned int c2 = coll->LineColor(i, 1);
    float val = f(i);
    return Color::Interpolate(c1, c2, val);
  }
private:
  LineCollection *coll;
  std::function<float (int)> f;
};

EXPORT GameApi::PTS GameApi::LinesApi::pts_line_anim(LI lines, float val)
{
  LineCollection *coll = find_line_array(e, lines);
  return add_points_api_points(e, new LineAnim2(coll, val));
}

EXPORT GameApi::PTS GameApi::LinesApi::pts_line_anim2(LI lines, std::function<float (int)> f)
{
  LineCollection *coll = find_line_array(e, lines);
  return add_points_api_points(e, new LineAnim3(coll, f));
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
  // INSTANCED RENDERING
  glGenBuffers(1, &arr->pos_buffer);
  // END OF INSTANCED RENDERING
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
class FromPoints2 : public LineCollection
{
public:
  FromPoints2(PointsApiPoints *start, PointsApiPoints *end) : start(start), end(end) { }
  virtual int NumLines() const { return std::min(start->NumPoints(), end->NumPoints()); }
  virtual Point LinePoint(int line, int point) const 
  {
    if (point==0) { return start->Pos(line); }
    return end->Pos(line);
  }
  virtual unsigned int LineColor(int line, int point) const { 
    if (point==0) { return start->Color(line); }
    return end->Color(line);
  }
private:
  PointsApiPoints *start;
  PointsApiPoints *end;
};
GameApi::LI GameApi::LinesApi::from_points2(PTS start_points, PTS end_points)
{
  PointsApiPoints *pts1 = find_pointsapi_points(e, start_points);
  PointsApiPoints *pts2 = find_pointsapi_points(e, end_points);
  return add_line_array(e, new FromPoints2(pts1, pts2));  
}
class LineProduct : public SingleForwardFaceCollection
{
public:
  LineProduct(LineCollection *coll, LineCollection *coll2) : coll(coll), coll2(coll2) { }
  virtual int NumFaces() const { return coll->NumLines()*coll2->NumLines(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int l2 = face / coll->NumLines();
    int l1 = face - l2*coll->NumLines();
    
    Point p1 = coll->LinePoint(l1,0);
    Vector vx = coll->LinePoint(l1,1)-p1;
    Vector vy = coll2->LinePoint(l2,1) - coll2->LinePoint(l2,0);
    switch(point)
      {
      case 0: return p1;
      case 1: return p1+vx;
      case 2: return p1+vx+vy;
      case 3: return p1+vy;
      };
    return p1;
  }
  virtual Vector PointNormal(int face, int point) const { Vector v; return v; }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int l2 = face / coll->NumLines();
    int l1 = face - l2*coll->NumLines();
    
    unsigned int c1 = coll->LineColor(l1,0);
    unsigned int c2 = coll->LineColor(l1,1);
    unsigned int c3 = coll2->LineColor(l2,1);
    unsigned int c4 = coll2->LineColor(l2,0);
    switch(point)
      {
      case 0: return c1;
      case 1: return c2;
      case 2: return c3;
      case 3: return c4;
      };
    return c1;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }

private:
  LineCollection *coll;
  LineCollection *coll2;
};
GameApi::P GameApi::LinesApi::line_product(LI lines1, LI lines2)
{
  LineCollection *l1 = find_line_array(e, lines1);
  LineCollection *l2 = find_line_array(e, lines2);
  return add_polygon2(e, new LineProduct(l1,l2),1);
}
class PointArrayLineCollection : public LineCollection
{
public:
  PointArrayLineCollection(std::vector<Point> vec) : vec(vec) { }
  int NumLines() const { return vec.size()-1; }
  Point LinePoint(int line, int point) const
  {
    int i = line + point;
    return vec[i];
  }
  unsigned int LineColor(int line, int point) const
  {
    return 0xffffffff;
  }
private:
  std::vector<Point> vec;
};
GameApi::LI GameApi::LinesApi::point_array(std::vector<PT> vec)
{
  int s = vec.size();
  std::vector<Point> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(*find_point(e, vec[i]));
    }
  return add_line_array(e, new PointArrayLineCollection(vec2));
}


class SplitLines : public LineCollection
{
public:
  SplitLines(LineCollection *coll, float dist) {
    int s = coll->NumLines();
    for(int i=0;i<s;i++)
      {
	Point p = coll->LinePoint(i, 0);
	Point p2 = coll->LinePoint(i, 1);
	unsigned int c = coll->LineColor(i, 0);
	unsigned int c2 = coll->LineColor(i, 1);
	float d = Dist(p,p2);
	if (d<dist) { vec.push_back(p); vec.push_back(p2); continue; }
	int count = d/dist;
	float delta = dist/d;
	float pos = 0.0;
      	for(int i=0;i<count;i++)
	  {
	    vec.push_back(Point::Interpolate(p,p2,pos));
	    vec.push_back(Point::Interpolate(p,p2,pos+delta));
	    color.push_back(Color::Interpolate(c,c2,pos));
	    color.push_back(Color::Interpolate(c,c2,pos+delta));
	    pos+=delta;
	  }
      }
  }
  int NumLines() const { return vec.size()/2; }
  Point LinePoint(int line, int point) const { return vec[line*2+point]; }
  unsigned int LineColor(int line, int point) const { return color[line*2+point]; }
private:
  float dist;
  std::vector<Point> vec;
  std::vector<unsigned int> color;
};

GameApi::LI GameApi::LinesApi::split_lines(LI li, float dist)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new SplitLines(lines, dist));
}

class TwistLines : public LineCollection
{
public:
  TwistLines(LineCollection *coll, float y_0, float angle_per_y_unit) : coll(coll), y_0(y_0), angle(angle_per_y_unit) { }
  int NumLines() const { return coll->NumLines(); }
  Point LinePoint(int line, int point) const
  {
    Point p = coll->LinePoint(line,point);
    float yy = p.y;
    yy-=y_0;
    float ang = yy*angle;
    Matrix m = Matrix::YRotation(ang);
    return p*m;
  }
private:
  LineCollection *coll;
  float y_0;
  float angle;
};

GameApi::LI GameApi::LinesApi::twist_y(LI li, float y_0, float angle_per_y_unit)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new TwistLines(lines, y_0, angle_per_y_unit));  
}
