
#include "GameApi_h.hh"

#include "GameApi_low.hh"

unsigned int swap_color(unsigned int c);
bool is_mobile(GameApi::EveryApi &ev);


class ColorLineCollection : public LineCollection
{
public:
  ColorLineCollection(LineCollection *coll, unsigned int color1, unsigned int color2) : coll(coll), color1(color1), color2(color2) { }
  void Collect(CollectVisitor &vis) {
    coll->Collect(vis);
  }
  void HeavyPrepare() { }

  virtual void Prepare() { coll->Prepare(); }
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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { coll->Prepare(); }

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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual void Prepare() {}

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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  virtual void Prepare() { }
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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  virtual void Prepare() { coll->Prepare(); }
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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { coll->Prepare(); }
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
  void Collect(CollectVisitor &vis) { coll.Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { coll.Prepare(); }

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
EXPORT GameApi::LI GameApi::LinesApi::li_matrix(LI lines, M matrix)
{
  LineCollection *c = find_line_array(e, lines);
  Matrix m = find_matrix(e, matrix);
  return add_line_array(e, new MatrixLineCollection(m, *c));
}
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
  void Collect(CollectVisitor &vis) { c.Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { c.Prepare(); }
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
  BorderFromBoolBitmap(Bitmap<bool> &bm, float start_x, float end_x, float start_y, float end_y, float z) : bm(bm),start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { }
  void Collect(CollectVisitor &vis) { bm.Collect(vis); vis.register_obj(this); }
  void HeavyPrepare() { Store(); }
  virtual void Prepare() { bm.Prepare(); Store(); }
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
  LI_Render(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LLA l, float lineWidth) : env(e), ev(ev), api(api), l(l), linewidth(lineWidth) { shader.id = -1;}
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() {}
  void execute(MainLoopEnv &e) {
    GameApi::SH sh;
    sh.id = e.sh_color;
    shader = sh;
    ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);

	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
    ev.shader_api.set_var(sh, "in_POS", e.in_POS);
  OpenglLowApi *ogl = g_low->ogl;
    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);

    api.render(l);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return shader.id; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LLA l;
  GameApi::SH shader;
  float linewidth;
};


class LI_Render2 : public MainLoopItem
{
public:
  LI_Render2(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LI l, float linewidth) : env(e), ev(ev), api(api), l(l),linewidth(linewidth) {firsttime=true; shader.id=-1; }
  void handle_event(MainLoopEvent &e)
  {
  }
    void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
  void execute(MainLoopEnv &e) {
    GameApi::SH sh;
    sh.id = e.sh_color;
    shader = sh;
    ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);

	if (firsttime)
	  {
	    l2 = ev.lines_api.prepare(l);
	    firsttime = false;
	  }
    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
  OpenglLowApi *ogl = g_low->ogl;
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);
	
    api.render(l2);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return shader.id; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LI l;
  GameApi::LLA l2;
  bool firsttime;
  GameApi::SH shader;
  float linewidth;
};


class LI_Render_Inst : public MainLoopItem
{
public:
  LI_Render_Inst(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LLA l, GameApi::PTA pta, float linewidth) : env(env), ev(ev), api(api), l(l),pta(pta),linewidth(linewidth) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
    void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
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
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);
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
    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
  OpenglLowApi *ogl = g_low->ogl;
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);

    api.render_inst(l,pta);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LLA l;
  GameApi::PTA pta;
  bool firsttime;
  GameApi::SH shader;
  float linewidth;
};

class LI_Render_Inst2 : public MainLoopItem
{
public:
  LI_Render_Inst2(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LI l0, GameApi::PTA pta, float linewidth) : env(env), ev(ev), api(api), l0(l0),pta(pta),linewidth(linewidth) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
    void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
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
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);

	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	sh = shader;
      }

    ev.shader_api.use(sh);
    if (firsttime)
      {
	l=api.prepare(l0);
	api.prepare_inst(l,pta);
	firsttime=false;
      }
    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
  OpenglLowApi *ogl = g_low->ogl;
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);

    api.render_inst(l,pta);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LI l0;
  GameApi::LLA l;
  GameApi::PTA pta;
  bool firsttime;
  GameApi::SH shader;
  float linewidth;
};



class LI_Render_Inst3 : public MainLoopItem
{
public:
  LI_Render_Inst3(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LI l0, GameApi::PTS pts, float linewidth) : env(env), ev(ev), api(api), l0(l0),pts(pts),linewidth(linewidth) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
    void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
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
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);

	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	sh = shader;
      }

    ev.shader_api.use(sh);
    if (firsttime)
      {
	pta = ev.points_api.prepare(pts);
	
	l=api.prepare(l0);
	api.prepare_inst(l,pta);
	firsttime=false;
      }

    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
  OpenglLowApi *ogl = g_low->ogl;
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);

    api.render_inst(l,pta);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LI l0;
  GameApi::LLA l;
  GameApi::PTS pts;
  GameApi::PTA pta;
  bool firsttime;
  GameApi::SH shader;
  float linewidth;
};


class LI_empty : public LineCollection
{
public:
  LI_empty() { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  virtual void Prepare() { }
  virtual int NumLines() const { return 0; }
  virtual Point LinePoint(int line, int point) const { return Point(0.0,0.0,0.0); }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }

};
GameApi::LI GameApi::LinesApi::li_empty()
{
  return add_line_array(e, new LI_empty);
}

class LI_Render_Inst3_matrix : public MainLoopItem
{
public:
  LI_Render_Inst3_matrix(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LinesApi &api, GameApi::LI l0, GameApi::MS pts, float linewidth) : env(env), ev(ev), api(api), l0(l0),pts(pts),linewidth(linewidth) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {}
  void HeavyPrepare() { }
  void Prepare() { }
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
	GameApi::US vertex2 = ev.uber_api.v_inst_matrix(vertex);
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
	//GameApi::M m3 = ev.matrix_api.trans(0.0,0.0,-1.1);
	//m = ev.matrix_api.mult(m3,m);

	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);
	sh = shader;
      }

    ev.shader_api.use(sh);
    if (firsttime)
      {
	pta = ev.matrices_api.prepare(pts);
	
	l=api.prepare(l0);
	api.prepare_inst_matrix(l,pta);
	firsttime=false;
      }

    float mult = 1.0;
    if (is_mobile(ev)) {
      mult = 0.2;
    }
  OpenglLowApi *ogl = g_low->ogl;
    if (is_platform_linux())
      ogl->glLineWidth(linewidth*mult/2);
    else
      ogl->glLineWidth(linewidth*mult);

    api.render_inst_matrix(l,pta);
    ev.shader_api.unuse(sh);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
  //int shader_id() { return -1; }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LinesApi &api;
  GameApi::LI l0;
  GameApi::LLA l;
  GameApi::MS pts;
  GameApi::MSA pta;
  bool firsttime;
  GameApi::SH shader;
  float linewidth;
};



EXPORT GameApi::ML GameApi::LinesApi::render_ml(EveryApi &ev, LLA l, float linewidth)
{
  return add_main_loop(e, new LI_Render(e, ev, *this, l,linewidth));
}
EXPORT GameApi::ML GameApi::LinesApi::render_ml2(EveryApi &ev, LI l,float linewidth)
{
  return add_main_loop(e, new LI_Render2(e, ev, *this, l,linewidth));
}
EXPORT GameApi::ML GameApi::LinesApi::render_inst_ml(EveryApi &ev, LLA l, PTA pta, float linewidth)
{
  return add_main_loop(e, new LI_Render_Inst(e, ev, *this, l, pta, linewidth));
}
EXPORT GameApi::ML GameApi::LinesApi::render_inst_ml2(EveryApi &ev, LI l, PTA pta, float linewidth)
{
  return add_main_loop(e, new LI_Render_Inst2(e, ev, *this, l, pta, linewidth));
}
EXPORT GameApi::ML GameApi::LinesApi::render_inst_ml3(EveryApi &ev, LI l, PTS pts, float linewidth)
{
  return add_main_loop(e, new LI_Render_Inst3(e, ev, *this, l, pts, linewidth));
}
EXPORT GameApi::ML GameApi::LinesApi::render_inst_ml3_matrix(EveryApi &ev, LI l, MS pts, float linewidth)
{
  return add_main_loop(e, new LI_Render_Inst3_matrix(e, ev, *this, l, pts, linewidth));
}

EXPORT void GameApi::LinesApi::prepare_inst(LLA l, PTA instances)
{
  OpenglLowApi *ogl = g_low->ogl;

  PointArray2 *array = find_lines_array(e,l);
  PointArray3 *arr = find_point_array3(e, instances);

  float *positions = arr->array;
  int size = arr->numpoints;

#ifdef VAO
  ogl->glBindVertexArray(array->vao[0]);
#endif
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );

  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Point) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  ogl->glBindVertexArray(0);
#endif

}

EXPORT void GameApi::LinesApi::prepare_inst_matrix(LLA l, MSA instances)
{
  OpenglLowApi *ogl = g_low->ogl;

  PointArray2 *array = find_lines_array(e,l);
  MatrixArray3 *arr = find_matrix_array3(e, instances);

  float *positions = arr->array;
  int size = arr->numpoints;

#ifdef VAO
  ogl->glBindVertexArray(array->vao[0]);
#endif
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );

  ogl->glBufferData( Low_GL_ARRAY_BUFFER, sizeof(Matrix) * size, positions, Low_GL_DYNAMIC_DRAW);

#ifdef VAO
  ogl->glBindVertexArray(0);
#endif

}

std::map<int,bool> g_render_inst_map;

EXPORT void GameApi::LinesApi::render_inst(LLA l, PTA instances)
{
  OpenglLowApi *ogl = g_low->ogl;

  PointArray3 *arr = find_point_array3(e, instances);
  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);

  float *positions = arr->array;
  int size = arr->numpoints;
  //ss INSTANCED RENDERING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );
  if (g_render_inst_map[instances.id]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  }
  ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glVertexAttribDivisor(5, 1);
  ogl->glEnableVertexAttribArray(5);
  // END OF INSTANCED RENDERING
#ifdef VAO
  ogl->glBindVertexArray(array->vao[0]);
#else
  ogl->glEnableVertexAttribArray(0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer);
  ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2_1);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0,0);
#endif

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );
  if (g_render_inst_map[instances.id]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Point) * size, positions);
  }
  ogl->glVertexAttribPointer(5, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glVertexAttribDivisor(5, 1);
  ogl->glEnableVertexAttribArray(5);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);


  ogl->glDrawArraysInstanced(Low_GL_LINES, 0, array->numpoints, size);
#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(4);
  ogl->glDisableVertexAttribArray(2);
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif


    g_render_inst_map[instances.id]=true;

}

EXPORT void GameApi::LinesApi::render_inst_matrix(LLA l, MSA instances)
{
  OpenglLowApi *ogl = g_low->ogl;

  MatrixArray3 *arr = find_matrix_array3(e, instances);
  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);

  float *positions = arr->array;
  int size = arr->numpoints;
  //ss INSTANCED RENDERING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );
  if (g_render_inst_map[instances.id]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  }
  ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
  ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  ogl->glVertexAttribDivisor(7, 1);
  ogl->glVertexAttribDivisor(8, 1);
  ogl->glVertexAttribDivisor(9, 1);
  ogl->glVertexAttribDivisor(10, 1);
  ogl->glEnableVertexAttribArray(7);
  ogl->glEnableVertexAttribArray(8);
  ogl->glEnableVertexAttribArray(9);
  ogl->glEnableVertexAttribArray(10);
  // END OF INSTANCED RENDERING
#ifdef VAO
  ogl->glBindVertexArray(array->vao[0]);
#else
  ogl->glEnableVertexAttribArray(0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer);
  ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2_1);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0,0);
#endif

  // INSTANCED DRAWING
  ogl->glBindBuffer( Low_GL_ARRAY_BUFFER, array->pos_buffer );
  if (g_render_inst_map[instances.id]==false) {
    ogl->glBufferSubData( Low_GL_ARRAY_BUFFER, 0, sizeof(Matrix) * size, positions);
  }
  ogl->glVertexAttribPointer(7, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, 0);
  ogl->glVertexAttribPointer(8, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*4));
  ogl->glVertexAttribPointer(9, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*8));
  ogl->glVertexAttribPointer(10, 4, Low_GL_FLOAT, Low_GL_FALSE, sizeof(float)*4*4, (void*)(sizeof(float)*12));
  ogl->glVertexAttribDivisor(7, 1);
  ogl->glVertexAttribDivisor(8, 1);
  ogl->glVertexAttribDivisor(9, 1);
  ogl->glVertexAttribDivisor(10, 1);
  ogl->glEnableVertexAttribArray(7);
  ogl->glEnableVertexAttribArray(8);
  ogl->glEnableVertexAttribArray(9);
  ogl->glEnableVertexAttribArray(10);
  // END INSTANCED

  ogl->glVertexAttribDivisor(0, 0);
  ogl->glVertexAttribDivisor(1, 0);
  ogl->glVertexAttribDivisor(2, 0);
  ogl->glVertexAttribDivisor(3, 0);
  ogl->glVertexAttribDivisor(4, 0);


  ogl->glDrawArraysInstanced(Low_GL_LINES, 0, array->numpoints, size);
#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(4);
  ogl->glDisableVertexAttribArray(2);
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif


    g_render_inst_map[instances.id]=true;

}

EXPORT void GameApi::LinesApi::render(LLA l)
{
  OpenglLowApi *ogl = g_low->ogl;

  PointArray2 *array = find_lines_array(e,l);
  //glEnableClientState(GL_VERTEX_ARRAY);
#ifdef VAO
  ogl->glBindVertexArray(array->vao[0]);
#else
  ogl->glEnableVertexAttribArray(0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer);
  ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2_1);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, array->buffer2);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0,0);
#endif
  ogl->glDrawArrays(Low_GL_LINES, 0, array->numpoints);
#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(4);
  ogl->glDisableVertexAttribArray(2);
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
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
  glBindBuffer(Low_GL_ARRAY_BUFFER, buffer);
  glBufferData(Low_GL_ARRAY_BUFFER, size*2*sizeof(float)*3, array, Low_GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(Low_GL_ARRAY_BUFFER, buffer);
  glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);


  glBindVertexArray(vao);
  glDrawArrays(Low_GL_LINES, 0, size);

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
  glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer);
  glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, Low_GL_STATIC_DRAW);
  glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2_1);
  glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array2_1, Low_GL_STATIC_DRAW);
  glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2);
  glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, Low_GL_STATIC_DRAW);
}
#endif
class LI_Update : public MainLoopItem
{
public:
  LI_Update(GameApi::LinesApi &api, GameApi::LLA la, GameApi::LI li) : api(api), la(la), li(li) { }
    void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() {}
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e) { api.update(la,li); }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }


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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { coll->Prepare(); }
  bool Update(MainLoopEnv &e) {
    return false;
  }
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
  LineAnim3(LineCollection *coll, std::function<float(int)> f) : coll(coll), f(f) { firsttime = true; }
    void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() {coll->Prepare(); }
  bool Update(MainLoopEnv &e) {
    return false;
  }
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
  bool firsttime;
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
  OpenglLowApi *ogl = g_low->ogl;

  LineCollection *coll = find_line_array(e, l);
  int count = coll->NumLines();
  float *array = 0;
  float *array2_1 = 0;
  unsigned int *color_array = 0;
  if (count > 0) {
    array = new float[count*6];
    array2_1 = new float[count*6];
    color_array = new unsigned int[count*2];
    for(int i=0;i<count;i++)
      {
	Point p = coll->LinePoint(i,0);
	Point p2 = coll->LinePoint(i,1);
	Point k = coll->EndLinePoint(i,0);
	Point k2 = coll->EndLinePoint(i,1);
	unsigned int color1 = coll->LineColor(i,0);
	unsigned int color2 = coll->LineColor(i,1);
	//std::cout << std::hex << color1 << ";" << std::hex << color2 << std::endl;
	array[i*6+0] = p.x;
	array[i*6+1] = p.y;
	array[i*6+2] = p.z;
	array[i*6+3] = p2.x;
	array[i*6+4] = p2.y;
	array[i*6+5] = p2.z;

	array2_1[i*6+0] = k.x;
	array2_1[i*6+1] = k.y;
	array2_1[i*6+2] = k.z;
	array2_1[i*6+3] = k2.x;
	array2_1[i*6+4] = k2.y;
	array2_1[i*6+5] = k2.z;

	color_array[i*2+0] = color1;
	color_array[i*2+1] = color2;


	//std::cout << i << ":" << "(" << p.x << "," << p.y << "," << p.z << ")-(" << p2.x<< "," << p2.y << "," << p2.z << ")" << std::endl;
      }
  }
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->array2_1 = array2_1;
  arr->color_array = color_array;
  arr->numpoints = count*2;
#ifdef VAO
  ogl->glGenVertexArrays(1, arr->vao);
  ogl->glBindVertexArray(arr->vao[0]);
#endif
  ogl->glGenBuffers(1, &arr->buffer);
  ogl->glGenBuffers(1, &arr->buffer2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2_1);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array2_1, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, Low_GL_STATIC_DRAW);
#ifdef VAO
  ogl->glEnableVertexAttribArray(0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer);
  ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2_1);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0,0);
#endif

#ifdef VAO
  ogl->glBindVertexArray(0);
#endif


  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(2);

  return add_update_lines_array(e, la, arr);

}
EXPORT GameApi::LLA GameApi::LinesApi::prepare(LI l)
{
  OpenglLowApi *ogl = g_low->ogl;

  LineCollection *coll = find_line_array(e, l);
  coll->Prepare();
  int count = coll->NumLines();
  float *array = 0;
  float *array2_1 = 0;
  unsigned int *color_array = 0;
  if (count > 0) {
    array = new float[count*6];
    array2_1 = new float[count*6];
    color_array = new unsigned int[count*2];
    for(int i=0;i<count;i++)
      {
	Point p = coll->LinePoint(i,0);
	Point p2 = coll->LinePoint(i,1);
	Point k = coll->EndLinePoint(i,0);
	Point k2 = coll->EndLinePoint(i,1);
	//if (p.x==p2.x && p.y==p2.y && p.z==p2.z)
	//  p2.x+=1.0;
	unsigned int color1 = coll->LineColor(i,0);
	unsigned int color2 = coll->LineColor(i,1);
	//std::cout << std::hex << color1 << ";" << std::hex << color2 << std::endl;
	array[i*6+0] = p.x;
	array[i*6+1] = p.y;
	array[i*6+2] = p.z;
	array[i*6+3] = p2.x;
	array[i*6+4] = p2.y;
	array[i*6+5] = p2.z;

	array2_1[i*6+0] = k.x;
	array2_1[i*6+1] = k.y;
	array2_1[i*6+2] = k.z;
	array2_1[i*6+3] = k2.x;
	array2_1[i*6+4] = k2.y;
	array2_1[i*6+5] = k2.z;

	color_array[i*2+0] = swap_color(color1);
	color_array[i*2+1] = swap_color(color2);
	//std::cout << i << ":" << "(" << p.x << "," << p.y << "," << p.z << ")-(" << p2.x<< "," << p2.y << "," << p2.z << ")" << std::endl;
      }
  }
  PointArray2 *arr = new PointArray2;
  arr->array = array;
  arr->array2_1 = array2_1;
  arr->color_array = color_array;
  arr->numpoints = count*2;
#ifdef VAO
  ogl->glGenVertexArrays(1, arr->vao);
  ogl->glBindVertexArray(arr->vao[0]);
  // INSTANCED RENDERING
  ogl->glGenBuffers(1, &arr->pos_buffer);
  // END OF INSTANCED RENDERING
#endif
  ogl->glGenBuffers(1, &arr->buffer);
  ogl->glGenBuffers(1, &arr->buffer2_1);
  ogl->glGenBuffers(1, &arr->buffer2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2_1);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array2_1, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color_array, Low_GL_STATIC_DRAW);
#ifdef VAO
  ogl->glEnableVertexAttribArray(0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer);
  ogl->glVertexAttribPointer(0, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2_1);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0, 0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer2);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0,0);
#endif
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(2);
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  return add_lines_array(e, arr);
}
class FromPoints2 : public LineCollection
{
public:
  FromPoints2(PointsApiPoints *start, PointsApiPoints *end) : start(start), end(end) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); coll2->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { coll->Prepare(); coll2->Prepare(); }
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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
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
{ // this code is optimized for memory usage. Previous version did
  // eat too much memory.
public:
  SplitLines(LineCollection *coll, float dist) : m_coll(coll), dist(dist) {
  }
  void Collect(CollectVisitor &vis) { m_coll->Collect(vis); vis.register_obj(this); }
  void HeavyPrepare() {

    int s = m_coll->NumLines();
    for(int i=0;i<s;i++)
      {
	Point p = m_coll->LinePoint(i, 0);
	Point p2 = m_coll->LinePoint(i, 1);
	float d = Dist(p,p2);
	if (d<dist) { indexes.push_back(1); continue; }
	int count = d/dist;
	indexes.push_back(count);
      }

  }

  void Prepare() {
    m_coll->Prepare();
    int s = m_coll->NumLines();
    for(int i=0;i<s;i++)
      {
	Point p = m_coll->LinePoint(i, 0);
	Point p2 = m_coll->LinePoint(i, 1);
	float d = Dist(p,p2);
	if (d<dist) { indexes.push_back(1); continue; }
	int count = d/dist;
	indexes.push_back(count);
      }
  }
  int NumLines() const {
    int s = indexes.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	count+=indexes[i];
      }
    return count;
  }
  Point LinePoint(int line, int point) const {
    int s = indexes.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	count+=indexes[i];
	if (count > line) {
	  // i is the 1st index
	  int j = line - oldcount;
	  // j is the 2nd index
	  Point p = m_coll->LinePoint(i, 0);
	  Point p2 = m_coll->LinePoint(i, 1);

	  float d = Dist(p,p2);
	  if (d<dist) {
	    if (point==0) return p;
	    if (point==1) return p2;
	    Point p0(0.0,0.0,0.0);
	    return p0;
	  }
	  float delta = dist/d;
	  float pos = delta*j;
	  
	  if (point == 0)
	    {
	      return Point::Interpolate(p,p2,pos);
	    }
	  else
	    {
	      return Point::Interpolate(p,p2,pos+delta);
	    }
	}
      }
    Point p0(0.0,0.0,0.0);
    return p0;
  }
  unsigned int LineColor(int line, int point) const { 
    int s = indexes.size();
    int count = 0;
    for(int i=0;i<s;i++)
      {
	int oldcount = count;
	count+=indexes[i];
	if (count > line) {
	  // i is the 1st index
	  int j = line - oldcount;
	  // j is the 2nd index
	  Point p = m_coll->LinePoint(i, 0);
	  Point p2 = m_coll->LinePoint(i, 1);

	  float d = Dist(p,p2);
	  if (d<dist) {
	    unsigned int cp = m_coll->LineColor(i, 0);
	    unsigned int cp2 = m_coll->LineColor(i, 1);
	    if (point==0) return cp;
	    if (point==1) return cp2;
	    unsigned int p0 = 0x0;
	    return p0;
	  }
	  float delta = dist/d;
	  float pos = delta*j;
	  
	  unsigned int cp = m_coll->LineColor(i, 0);
	  unsigned int cp2 = m_coll->LineColor(i, 1);
	  if (point == 0)
	    {
	      return Color::Interpolate(cp,cp2,pos);
	    }
	  else
	    {
	      return Color::Interpolate(cp,cp2,pos+delta);
	    }
	}
      }
    unsigned int p0 = 0x0;
    return p0;
  }
private:
  LineCollection *m_coll;
  float dist;
  std::vector<int> indexes;
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
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { coll->Prepare(); }
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

class Fur2 : public LineCollection
{
public:
  Fur2(PointsApiPoints *pts, Point center, float dist) : pts(pts), center(center), dist(dist) { }
  void Collect(CollectVisitor &vis) { if (pts) pts->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { if (pts) pts->Prepare(); }
  virtual int NumLines() const { if (pts) return pts->NumPoints(); else return 0; }
  virtual Point LinePoint(int line, int point) const
  {
    if (!pts) { return Point(0.0,0.0,0.0); }
    if (point==0) { return pts->Pos(line); }
    Point p = pts->Pos(line);
    Vector dir = p - center;
    dir/=dir.Dist();
    dir*=dist;
    return p + dir;
  }
  virtual unsigned int LineColor(int line, int point) const {
    if (!pts) return 0x0;
    return pts->Color(line);
  }
  
private:
  PointsApiPoints *pts;
  Point center;
  float dist;
};

class MeshLines : public LineCollection
{
public:
  MeshLines(std::vector<Point> points, std::vector<unsigned int> color) : points(points), color(color) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }
  virtual int NumLines() const { return points.size()/2; }
  virtual Point LinePoint(int line, int point) const
  {
    line*=2;
    if (point==0) return points[line];
    return points[line+1];
  }
  virtual unsigned int LineColor(int line, int point) const { 
    line*=2;
    if (point==0) return color[line];
    return color[line+1];
  }

private:
  std::vector<Point> points;
  std::vector<unsigned int> color;
};

GameApi::LI GameApi::LinesApi::fur(PTS points, PT center, float dist)
{
  PointsApiPoints *pts = find_pointsapi_points(e, points);
  Point *pt = find_point(e, center);
  return add_line_array(e, new Fur2(pts, *pt, dist));
}

GameApi::LI GameApi::LinesApi::random_mesh_quad_lines(EveryApi &ev, P p, int count)
{
  FaceCollection *coll = find_facecoll(e, p);
  coll->Prepare();
  std::vector<Point> *points = new std::vector<Point>;
  std::vector<unsigned int> *color2 = new std::vector<unsigned int>;
  for(int i=0;i<count*2;i++)
    {
      Random r;
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      float zp = double(r.next())/r.maximum();
      xp*=2.0;
      yp*=2.0;
      xp-=1.0;
      yp-=1.0;
      zp*=float(coll->NumFaces());
      int zpi = int(zp);
      if (zpi<0) zpi = 0;
      if (zpi>=coll->NumFaces()) zpi = coll->NumFaces()-1;
      int num = coll->NumPoints(zpi);
      if (num != 4 && num != 3) { /*std::cout << "Error quad: " << num << std::endl;*/ }
      if (num==4) {
	Point p1 = coll->FacePoint(zpi, 0);
	Point p2 = coll->FacePoint(zpi, 1);
	Point p3 = coll->FacePoint(zpi, 2);
	Point p4 = coll->FacePoint(zpi, 3);
	Point p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(p1) + (1.0f+xp)*(1.0f-yp)*Vector(p2) + (1.0f+xp)*(1.0f+yp)*Vector(p3) + (1.0f-xp)*(1.0f+yp)*Vector(p4));
	if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	points->push_back(p);
	color2->push_back(0xffffffff);
      } else if (num==3)
	{
	  Point p1 = coll->FacePoint(zpi, 0);
	  Point p2 = coll->FacePoint(zpi, 1);
	  Point p3 = coll->FacePoint(zpi, 2);
	  float r1 = double(r.next())/r.maximum();
	  float r2 = double(r.next())/r.maximum();
	  Point p = Point((1.0-sqrt(r1))*Vector(p1) + (sqrt(r1)*(1.0-r2))*Vector(p2) + (r2*sqrt(r1))*Vector(p3));
	  if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	  points->push_back(p);
	  color2->push_back(0xffffffff);
	  
	}
    }
  return add_line_array(e, new MeshLines(*points, *color2));
}

class RandomAngleLines : public LineCollection
{
public:
  RandomAngleLines(LineCollection *coll, float max_angle) : coll(coll), max_angle(max_angle) { }
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { coll->Prepare(); }
  virtual int NumLines() const { return coll->NumLines(); }
  virtual Point LinePoint(int line, int point) const
  {
    if (point==0) return coll->LinePoint(line, 0);
    Random r;
    float angle1 = double(r.next())/r.maximum();
    float angle2 = double(r.next())/r.maximum();
    angle1*=max_angle;
    angle2*=2.0*3.14159;

    Point p1 = coll->LinePoint(line,0);
    Point p2 = coll->LinePoint(line,1);
    Vector v = p2-p1;
    Vector v2 = { 0.0, 1.0, 0.0 };
    Vector xx = Vector::CrossProduct(v,v2);
    Matrix m = Matrix::RotateAroundAxis(xx, angle1);
    Vector v3 = v*m;
    Matrix m2 = Matrix::RotateAroundAxis(v, angle2);
    Vector v4 = v3*m2;
    Point p3 = p1 + v4;
    return p3;
  }
  virtual unsigned int LineColor(int line, int point) const { return coll->LineColor(line,point); }

private:
  LineCollection *coll;
  float max_angle;
};

GameApi::LI GameApi::LinesApi::random_angle(LI lines, float max_angle)
{
  LineCollection *lines2 = find_line_array(e, lines);
  return add_line_array(e, new RandomAngleLines(lines2, max_angle));

}
class LI_or_array : public LineCollection
{
public:
  LI_or_array(std::vector<LineCollection*> vec) : vec(vec)
  {
  }
  void Collect(CollectVisitor &vis) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { update_cache(); }

  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
    update_cache();
  }
  void update_cache()
  {
    int s2 = NumLines();
    lines_cache.reserve(s2);
    lines_num.reserve(s2);
    int count = 0;
    int sum = 0;
    int f = 0;
    //int k = 0;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	int oldsum = sum;
	count = vec[i]->NumLines();
	sum+=count;
	for(int j=0;j<count;j++)
	  {
	    lines_cache.push_back(f-oldsum);
	    lines_num.push_back(i);
	    f++;
	  }
      }
  }

  
  int NumLines() const
  {
    int s = vec.size();
    int count=0;
    for(int i=0;i<s;i++)
      {
	count+=vec[i]->NumLines();
      }
    return count;
  }

  Point LinePoint(int line, int point) const
  {
    return vec[lines_num[line]]->LinePoint(lines_cache[line], point);
  }
  unsigned int LineColor(int line, int point) const
  {
    return vec[lines_num[line]]->LineColor(lines_cache[line], point);
    
  }
  Point EndLinePoint(int line, int point) const
  {
    return vec[lines_num[line]]->EndLinePoint(lines_cache[line], point);
  }

  unsigned int EndLineColor(int line, int point) const
  {
    return vec[lines_num[line]]->EndLineColor(lines_cache[line], point);
  }
  
private:
  std::vector<LineCollection*> vec;
  std::vector<int> lines_cache;
  std::vector<int> lines_num;
};
GameApi::LI GameApi::LinesApi::li_or_array(std::vector<LI> vec)
{
  std::vector<LineCollection*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_line_array(e, vec[i]));
    }
  return add_line_array(e, new LI_or_array(vec2));
}
#if 0
class IFCImport : public LineCollection
{
public:
  IFCImport(GameApi::Env &e, GameApi::EveryApi &ev, std::string url, std::string homepage) : e(e), ev(ev), url(url), homepage(homepage) {
    current=0;
  }

  virtual void Prepare() {
    if (current == 0) {
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepage);
#endif
      std::vector<unsigned char> *ptr = e.get_loaded_async_url(url);
      if (!ptr) {
	std::cout << "ifc_url async not ready yet, failing..." << std::endl;
	return;
      }
      current = 1;
      std::string s(ptr->begin(), ptr->end());
      std::stringstream ss(s);
      std::string line;
      char *array[] = { "IFCCARTESIANPOINT((",
			"IFCFACE",
			"IFCPOLYLOOP",
			"IFCCOLOURRGB($,"
       };
      int counter = 300000;
      unsigned int colour = 0xffffffff;
      vec.push_back(std::vector<Point>());
      color.push_back(std::vector<unsigned int>());
      while(std::getline(ss, line)) {
	int s = sizeof(array)/sizeof(char*);
	for(int i=0;i<s;i++) {
	  std::string::iterator ii = std::search(line.begin(), line.end(),
						array[i], array[i]+strlen(array[i]));
	  if (ii!=line.end()) {
	    std::advance(ii, strlen(array[i]));
	    std::string s2(ii, line.end());
	    if (i==0) { // point
	      std::stringstream ss3(s2);
	      double x,y,z;
	      char ch;
	      bool err = false;
	      ss3 >> x;
	      do {
	      err = ss3 >> ch;
	      } while(!err && ch!=',');
	      ss3 >> y;
	      do {
	      err = ss3 >> ch;
	      } while(!err && ch!=',');
	      ss3 >> z;
	      std::cout << "(" << x << "," << y << "," << z << ")" << std::endl;
	      vec[vec.size()-1].push_back(Point(float(x),float(y),float(z)));
	      color[color.size()-1].push_back(colour);
	      counter--;
	      if (counter<1) return;
	    } else if (i==1) { // face
	      
	    } else if (i==2) { // polyloop
	      vec.push_back(std::vector<Point>());
	      color.push_back(std::vector<unsigned int>());
	    } else if (i==3) { // colour
	      std::stringstream ss3(s2);
	      float r,g,b;
	      char ch;
	      ss3 >> r >> ch >> g >> ch >> b;
	      Color c(r,g,b);
	      colour = c.Pixel();
	    }
	  }
	}
      }
    }
  }
  virtual int NumLines() const {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++) {
      count+=vec[i].size()-1;
    }
    return count;
  }
  virtual Point LinePoint(int line, int point) const
  {
    int s = vec.size();
    int count = 0;
    for(int i=0;i<s;i++) {
      int oldcount = count;
      count+=vec[i].size()-1;
      if (count>line) {
	int i2 = line-oldcount;
	if (point==0) {
	  Point p = vec[i][i2];
	  return p;
	} else {
	  Point p = vec[i][(i2+1)%vec[i].size()];
	  return p;
	}
      }
    }
    Point p(0.0,0.0,0.0);
    return p;
  }
  virtual unsigned int LineColor(int line, int point) const { 
    int s = color.size();
    int count = 0;
    for(int i=0;i<s;i++) {
      int oldcount = count;
      count+=color[i].size()-1;
      if (count>line) {
	int i2 = line-oldcount;
	if (point==0) {
	  unsigned int p = color[i][i2];
	  return p;
	} else {
	  unsigned int p = color[i][(i2+1)%vec[i].size()];
	  return p;
	}
      }
    }

    return 0xffffffff;
  }  
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  int filled;
  LineCollection *empty;
  int current;
  std::vector<std::vector<Point> > vec;
  std::vector<std::vector<unsigned int> > color;
};
  
extern std::string gameapi_homepageurl;
  
GameApi::LI GameApi::LinesApi::import_ifc(EveryApi &ev, std::string url)
{
  return add_line_array(e, new IFCImport(e, ev, url, gameapi_homepageurl));
}
#endif

class LinePosMult : public LineCollection
{
public:
  LinePosMult(float val, LineCollection *coll) : val(val), coll(coll) {}
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { coll->Prepare(); }
  virtual int NumLines() const { return coll->NumLines(); }
  virtual Point LinePoint(int line, int point) const { 
    Point p = coll->LinePoint(line,point);
    p.x*=val;
    p.y*=val;
    p.z*=val;
    return p; }
  virtual unsigned int LineColor(int line, int point) const { return coll->LineColor(line,point);}

private:
  float val;
  LineCollection *coll;
};
GameApi::LI GameApi::LinesApi::line_pos_mult(float val, LI li)
{
  LineCollection *lines = find_line_array(e, li);
  return add_line_array(e, new LinePosMult(val, lines));
}

class LIOrElem : public LineCollection
{
public:
  LIOrElem(LineCollection *lines1, LineCollection *lines2) : lines1(lines1), lines2(lines2) { s=lines1->NumLines(); s2=lines2->NumLines(); }
  void Collect(CollectVisitor &vis) {
    lines1->Collect(vis);
    lines2->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    s=lines1->NumLines();
    s2=lines2->NumLines();
  }
  virtual void Prepare() {
    lines1->Prepare();
    lines2->Prepare();
    s=lines1->NumLines();
    s2=lines2->NumLines();
  }
  int get_index(int line) const {
    //std::cout << "get_index: " << line << " " << s << std::endl;
    if (line<s) return line;
    return line-s;
  }
  LineCollection *get_elem(int line) const {
    //std::cout << "get_elem: " << line << " " << s << std::endl;
    if (line<s) return lines1;
    return lines2;
  }
  virtual int NumLines() const { return lines1->NumLines()+lines2->NumLines(); }
  virtual Point LinePoint(int line, int point) const
  {
    return get_elem(line)->LinePoint(get_index(line),point);
  }
  virtual unsigned int LineColor(int line, int point) const {
    return get_elem(line)->LineColor(get_index(line),point);
  }
private:
  LineCollection *lines1;
  LineCollection *lines2;
  int s,s2;
};

GameApi::LI GameApi::LinesApi::li_or_elem(LI li1, LI li2)
{
  LineCollection *lines1 = find_line_array(e, li1);
  LineCollection *lines2 = find_line_array(e, li2);
  return add_line_array(e, new LIOrElem(lines1,lines2));
}

class Bevel : public LineCollection
{
public:
  Bevel(LineCollection *lines, FaceCollection *faces, float mix) : lines(lines), faces(faces), mix(mix) { }
  void Collect(CollectVisitor &vis) { lines->Collect(vis); faces->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { lines->Prepare(); faces->Prepare(); }
  int NumLines() const { if (lines) return lines->NumLines(); return 0; }
  Point LinePoint(int line, int point) const { if (lines) return lines->LinePoint(line,point); return Point(0.0,0.0,0.0); }
  unsigned int LineColor(int line, int point) const {
    Point p1 = lines->LinePoint(line,point);
    //Point p2 = Point(0.9708737864*Vector(p1));
    int count = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    int s = faces->NumFaces();
    for(int i=0;i<s;i++) {
      int s2 = faces->NumPoints(i);
      for(int j=0;j<s2;j++) {
	Vector v = faces->FacePoint(i,j)-p1;
	if (v.Dist()<10.00) {
	  count++;
	  unsigned int cc = faces->Color(i,j);
	  Color cc2(cc);
	  r+=cc2.r;
	  g+=cc2.g;
	  b+=cc2.b;
	}
      }
    }
    if (count>0) {
      r/=count;
      g/=count;
      b/=count;
    }
    unsigned int col = Color(r,g,b).Pixel();
    unsigned int col2 = lines->LineColor(line,point);
    return Color::Interpolate(col,col2,mix);
  }

private:
  LineCollection *lines;
  FaceCollection *faces;
  float mix;
};

GameApi::LI GameApi::LinesApi::li_bevel(LI li, P p, float mix)
{
  LineCollection *lines = find_line_array(e,li);
  FaceCollection *faces = find_facecoll(e,p);
  return add_line_array(e, new Bevel(lines, faces, mix));
}

class TowardsNormal : public ForwardFaceCollection
{
public:
  TowardsNormal(FaceCollection *coll, float amount) : ForwardFaceCollection(*coll), coll(coll), amount(amount) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  Point FacePoint(int face, int point) const
  {
    Vector v = coll->PointNormal(face,point);
    v/=v.Dist();
    Point p = coll->FacePoint(face,point);
    float d = p.Dist();
    d-=350.0;
    d/=(1200.0-350.0);
    if (d<0.0) d=0.0;
    if (d>1.0) d=1.0;
    float amount2 = (1.0-d)*3.0 + d*3.0; // was 40.0
    return coll->FacePoint(face,point)+amount*amount2*v;
  }
private:
  FaceCollection *coll;
  float amount;
};

GameApi::P GameApi::LinesApi::p_towards_normal(P p, float amount)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new TowardsNormal(coll,amount),1);
}


struct LIStore
{
  Point p1,p2;
  unsigned int c1,c2;
};

extern std::string gameapi_homepageurl;


class LoadLI : public LineCollection
{
public:
  LoadLI(GameApi::Env &env, std::string url, std::string homepage) : env(env), url(url), homepage(homepage) { pos.x=0.0; pos.y=0.0; pos.z=0.0; }
  void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  virtual void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = env.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    std::stringstream ss(code);
    std::string line;
    vec2 = std::vector<LIStore>();
    while(std::getline(ss,line)) {
      std::string word;
      std::stringstream ss2(line);
      ss2 >> word;
      if (word=="end_alias") {
	alias_map[current_alias] = alias_store;
	alias_store=std::vector<std::string>();
	current_alias='\0';
      }
      if (current_alias!='\0') { alias_store.push_back(line); continue; } 
      if (word=="color") {
	ss2 >> std::hex >> c1 >> std::hex >> c2 >> std::dec;
      }
      if (word=="line") {
	LIStore store;
	ss2 >> store.p1.x >> store.p1.y >> store.p1.z;
	ss2 >> store.p2.x >> store.p2.y >> store.p2.z;
	store.c1 = c1;
	store.c2 = c2;
	vec2.push_back(store);
      }
      if (word=="point_at") {
	ss2>> pos.x >> pos.y >> pos.z;
      }
      if (word=="line_to") {
	LIStore store;
	store.p1 = pos;
	ss2 >> store.p2.x >> store.p2.y >> store.p2.z;
	pos = store.p2;
	store.c1 = c1;
	store.c2 = c2;
	vec2.push_back(store);
      }
      if (word=="line_delta") {
	LIStore store;
	store.p1 = pos;
	store.p2 = pos;
	Vector delta;
	ss2 >> delta.dx >> delta.dy >> delta.dz;
	store.p2+=delta;
	pos = store.p2;
	store.c1 = c1;
	store.c2 = c2;
	vec2.push_back(store);	
      }
      if (word=="start_alias") {
	ss2 >> current_alias;
      }
      if (word=="execute_alias") {
	std::string chars;
	ss2 >> chars;
	int s = chars.size();
	for(int i=0;i<s;i++) {
	  std::vector<std::string> alias = alias_map[chars[i]];
	  int s2 = alias.size();
	  for(int i=0;i<s2;i++) {
	    std::string line = alias[i];
	    handle_line(line);
	  }
	}
      }
    }
  }
  void handle_line(std::string line) {

    std::string word;
    std::stringstream ss2(line);
    ss2 >> word;
    
    if (word=="color") {
      ss2 >> std::hex >> c1 >> std::hex >> c2 >> std::dec;
    }
    if (word=="line") {
      LIStore store;
      ss2 >> store.p1.x >> store.p1.y >> store.p1.z;
      ss2 >> store.p2.x >> store.p2.y >> store.p2.z;
      store.c1 = c1;
      store.c2 = c2;
      vec2.push_back(store);
    }
    if (word=="point_at") {
      ss2>> pos.x >> pos.y >> pos.z;
    }
    if (word=="line_to") {
      LIStore store;
      store.p1 = pos;
      ss2 >> store.p2.x >> store.p2.y >> store.p2.z;
      pos = store.p2;
      store.c1 = c1;
      store.c2 = c2;
      vec2.push_back(store);
    }
    if (word=="line_delta") {
      LIStore store;
      store.p1 = pos;
      store.p2 = pos;
      Vector delta;
      ss2 >> delta.dx >> delta.dy >> delta.dz;
      store.p2+=delta;
      pos = store.p2;
      store.c1 = c1;
      store.c2 = c2;
      vec2.push_back(store);	
    }
    if (word=="execute_alias") {
	std::string chars;
	ss2 >> chars;
	int s = chars.size();
	for(int i=0;i<s;i++) {
	  std::vector<std::string> alias = alias_map[chars[i]];
	  int s2 = alias.size();
	  for(int i=0;i<s2;i++) {
	    std::string line = alias[i];
	    handle_line(line);
	  }
	}
    }

  }
  virtual int NumLines() const { return vec2.size(); }
  virtual Point LinePoint(int line, int point) const
  {
    int sz = vec2.size();
    if (line>=0 && line<sz) {
      LIStore s = vec2[line];
      if (point==0) return s.p1;
      if (point==1) return s.p2;
    }
    Point p(0.0,0.0,0.0);
    return p;
  }
  virtual unsigned int LineColor(int line, int point) const {
    int sz = vec2.size();
    if (line>=0 && line<sz) {
      LIStore s = vec2[line];
      if (point==0) return s.c1;
      if (point==1) return s.c2;
    }
    return 0xffffffff;
  }
private:
  GameApi::Env &env;
  std::string url;
  std::string homepage;
  unsigned int c1=0xffffffff, c2 = 0xffffffff;
  std::vector<LIStore> vec2;
  Point pos;
  char current_alias = '\0';
  std::vector<std::string> alias_store;
  std::map<char, std::vector<std::string> > alias_map;
};

GameApi::LI GameApi::LinesApi::li_url(std::string url)
{
  return add_line_array(e, new LoadLI(e,url, gameapi_homepageurl));
}
