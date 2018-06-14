#if 0

#define NO_SDL_GLEXT
#include "KeyFrameEditor.hh"
#include "VectorTools.hh"
#include "Effect.hh"
#include <gtkmm.h>
#include <gtkmm/socket.h>
//#include <GL/glew.h>
//#include <GL/gl.h>
//#include <SDL/SDL.h>
//#include <SDL/SDL_opengl.h>

class ScaleArea
{
public:
  ScaleArea(int window_sx, int window_sy,
	    int coords_sx, int coords_sy) : window_sx(window_sx),
					    window_sy(window_sy),
					    coords_sx(coords_sx),
					    coords_sy(coords_sy),
					    mm(Matrix::Identity())
  { }
  void SetMatrix(Matrix m) { mm = m; }
  Point convert_to_coords(Point2d window_pos, float z)
  {
    Point p;
    p.x = window_pos.x*coords_sx/window_sx;
    p.y = window_pos.y*coords_sy/window_sy;
    p.z = z;
    p = p * Matrix::Inverse(mm);
    return p;
  }
  std::pair<Point2d,float> convert_to_window(Point coords_pos)
  {
    coords_pos = coords_pos * mm;
    Point2d p;
    p.x = coords_pos.x*window_sx/coords_sx;
    p.y = coords_pos.y*window_sy/coords_sy;
    return std::make_pair(p, coords_pos.z);
  }
private:
  int window_sx, window_sy, coords_sx, coords_sy;
  Matrix mm;
};

class MoveChanges : public LineCollection
{
public:
  MoveChanges(LineCollection *old) : old(old) { }
  void push_back(int num, Point start_newpos, Point end_newpos)
  {
    change_num.push_back(num);
    change_pos.push_back(start_newpos);
    change_pos2.push_back(end_newpos);
  }
  virtual int NumLines() const
  {
    return old->NumLines();
  }
  virtual Point LinePoint(int line, int point) const
  {
    int s = change_num.size();
    for(int i=0;i<s;i++)
      {
	int val = change_num[i];
	Point p = change_pos[i];
	Point p2 = change_pos2[i];
	if (val == line && point==0) { return p; }
	if (val == line && point==1) { return p2; }
      }
    return old->LinePoint(line, point);
  }
  LineCollection *Undo() { return old; }
private:
  std::vector<int> change_num;
  std::vector<Point> change_pos;
  std::vector<Point> change_pos2;
  LineCollection *old;
};

class LineDrawingArea : public Gtk::DrawingArea
{
public:
  LineDrawingArea() : changes(0), mm(Matrix::Identity()), shownum(-1), motion_event(false), xrot(0), yrot(0), zrot(0), rot_matrix(Matrix::Identity())
  {
    set_flags(Gtk::CAN_FOCUS);
    set_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::POINTER_MOTION_MASK|Gdk::KEY_PRESS_MASK|Gdk::KEY_RELEASE_MASK);
    signal_button_press_event().connect( sigc::mem_fun(this, &LineDrawingArea::on_button_press) );    
    signal_button_release_event().connect( sigc::mem_fun(this, &LineDrawingArea::on_button_release) );
    signal_motion_notify_event().connect( sigc::mem_fun( this, &LineDrawingArea::on_motion_event) );
    signal_key_press_event().connect( sigc::mem_fun( this, &LineDrawingArea::on_key_press_event), false );
    signal_key_release_event().connect( sigc::mem_fun( this, &LineDrawingArea::on_key_release_event), false );

    Glib::signal_timeout().connect( sigc::mem_fun(this, &LineDrawingArea::on_timer_event), 1000/60);
  }
  void set_matrix(Matrix m) { mm = m; }
  void set_area(int sx2, int sy2) { sx = sx2; sy=sy2; }
  void set_coll(LineCollection *coll2) { coll = coll2; add_to_undo_stack(); }
  void set_lines(PointCollection *coll2, bool loops) { coll = new ContinuousLines(coll2, loops); add_to_undo_stack(); }
  void set_polys(FaceCollection *coll2) { coll = new OutlineFaces(*coll2); add_to_undo_stack(); original = coll2; }
  FaceCollection *get_polys() { return new InverseOutlineFaces(*original, *changes); }

  void move_point(int pointnum, Point2d p, bool first)
  {
    if (first)
      {
	window_pos[pointnum] = p;
      }
    else
      {
	window_pos2[pointnum] = p;
      }
  }
  int find_pointnum(int size, Point2d p)
  {
    int s = window_pos.size();
    for(int i=0;i<s;i++)
      {
	Point2d wp = window_pos[i];
	Vector2d v = p-wp;
	float d = v.Dist();
	if (d<size) return i;
      }
    int s2 = window_pos2.size();
    for(int i=0;i<s2;i++)
      {
	Point2d wp = window_pos2[i];
	Vector2d v = p-wp;
	float d = v.Dist();
	if (d<size) return i;
      }
    return -1;
  }
  bool find_bool(int size, Point2d p)
  {
    int s = window_pos.size();
    for(int i=0;i<s;i++)
      {
	Point2d wp = window_pos[i];
	Vector2d v = p-wp;
	float d = v.Dist();
	if (d<size) return true;
      }
    int s2 = window_pos2.size();
    for(int i=0;i<s2;i++)
      {
	Point2d wp = window_pos2[i];
	Vector2d v = p-wp;
	float d = v.Dist();
	if (d<size) return false;
      }
    return false;
  }
  void calc_rot_matrix()
  {
    rot_matrix=Matrix::XRotation(xangle)*Matrix::YRotation(yangle)*Matrix::ZRotation(zangle);
  }
  void move_point2(Point2d old_pos, Point2d new_pos)
  {
    motion_event=false;
    move_to_be_drawn = true;
    Glib::RefPtr<Gdk::Window> win = get_window();
    int oldnum = -1;
    while(1)
      {
    int num = find_pointnum(4, old_pos);
    //std::cout << "move point" << num << std::endl;
    if (oldnum==num) break;
    oldnum =num;
    if (num==-1) break;
    bool b = find_bool(4, old_pos);
    move_point(num, new_pos, b);
    Point p1 = changes->LinePoint(num, 0);
    Point p2 = changes->LinePoint(num, 1);
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    ScaleArea area(width, height, sx,sy);
    area.SetMatrix(rot_matrix*mm);
    Point p;
    if (b) 
      { 
	p = area.convert_to_coords(window_pos[num], z_pos[num]);
	p1 = p;
      }
    else
      {
	p = area.convert_to_coords(window_pos2[num], z_pos2[num]);
	p2 = p;
      }
    MoveChanges *changes2 = (MoveChanges*)changes;
    changes2->push_back(num, p1, p2);
    refresh();
      }
    add_to_undo_stack();
    Gdk::Rectangle r(0,0,get_allocation().get_width(),get_allocation().get_height());
    win->invalidate_rect(r, true);
    win->process_updates(true);

  }
  void add_to_undo_stack()
  {
    if (!changes)
      {
	changes = new MoveChanges(coll);
      }
    else
      {
	changes = new MoveChanges(changes);
      }
  }
  void undo()
  {
    MoveChanges *ch = dynamic_cast<MoveChanges*>(changes);
    if (ch)
      changes = ch->Undo();
  }
protected:
  bool on_key_press_event(GdkEventKey *event)
  {
    std::cout << "key: " << event->keyval << std::endl;
    if (event->keyval == 117 && !undo_ongoing) // u
      {
	undo_ongoing = true;
	undo();
      }
    if (event->keyval == 120) // z
      {
	xrot=-1;
      }
    if (event->keyval == 122) // x
      {
	xrot=1;
      }
    if (event->keyval==97) // a
      {
	yrot=-1;
      }
    if (event->keyval==115) // s
      {
	yrot=1;
      }
    if (event->keyval==113) // q
      {
	zrot=-1;
      }
    if (event->keyval==119) // w
      {
	zrot=1;
      }
    return true;
  }
  bool on_key_release_event(GdkEventKey *event)
  {
    std::cout << "rkey: " << event->keyval << std::endl;
    if (event->keyval==120 && xrot==-1) xrot=0;
    if (event->keyval==122 && xrot==1) xrot=0;
    if (event->keyval==97 && yrot==-1) yrot=0;
    if (event->keyval==115 && yrot==1) yrot=0;
    if (event->keyval==113 && zrot==-1) zrot=0;
    if (event->keyval==119 && zrot==1) zrot=0;
    if (event->keyval == 117 && undo_ongoing) // u
      {
	undo_ongoing = false;
      }

    return true;
  }
  bool on_timer_event()
  {
    xangle += 2.0*3.14159*xrot/100.0;
    yangle += 2.0*3.14159*yrot/100.0;
    zangle += 2.0*3.14159*zrot/100.0;
    calc_rot_matrix();
    if (xrot!=0 || yrot!=0 || zrot!=0)
      {
	motion_event=false;
	move_to_be_drawn = true;

	Glib::RefPtr<Gdk::Window> win = get_window();
	Gdk::Rectangle r(0,0,get_allocation().get_width(),get_allocation().get_height());
	win->invalidate_rect(r, true);
	win->process_updates(true);
      }
    return true;
  }
  bool on_motion_event(GdkEventMotion *event)
  {
    if (move_to_be_drawn) return true;
    //std::cout << "Motion" << std::endl;
    gdouble xx = event->x;
    gdouble yy = event->y;
    if (fabs(lastxx-xx)<4 && fabs(lastyy-yy)<4) return true;
    lastxx = xx;
    lastyy = yy;
    Point2d p = { float(xx), float(yy) };
    int num = find_pointnum(4, p);
    if (num!=-1)
      {
	shownum=num;
	showbool = find_bool(4, p);
	//std::cout << "shownum:" << num << "," << showbool << std::endl;
	Point2d p = window_pos[shownum];
	Point2d p2 = window_pos2[shownum];
	Point2d pp;
	if (showbool) { 
	  pp = p;
	}
	else
	  {
	    pp = p2;
	  }
    motion_event=true;
	Gdk::Rectangle r(pp.x-2,pp.y-2,4,
			 4);
	Glib::RefPtr<Gdk::Window> win = get_window();
	win->invalidate_rect(r, true);
        win->process_updates(true);
      }
    else
      {
	if (shownum!=-1)
	  {
	    Point2d p = window_pos[shownum];
	    Point2d p2 = window_pos2[shownum];
	    Point2d pp;
	    if (showbool) { 
	      pp = p;
	    }
	    else
	      {
		pp = p2;
	      }
	    shownum=-1;
    motion_event=true;
	    Gdk::Rectangle r(pp.x-2,pp.y-2,4,
			     4);
	    Glib::RefPtr<Gdk::Window> win = get_window();
	    win->invalidate_rect(r, true);
	    win->process_updates(true);
	  }
      }
    return false;
  }
  virtual bool on_button_press(GdkEventButton *event)
  {
    //std::cout << "on_button_press_event" << std::endl;
    //Gtk::DrawingArea::on_button_press_event(event);
    if (!event) { std::cout << "No event!" << std::endl; return false; }
    gdouble xx = event->x;
    gdouble yy = event->y;
    drag_start_pos.x = xx;
    drag_start_pos.y = yy;
    return false;
  }
  virtual bool on_button_release(GdkEventButton *event)
  {
    //std::cout << "on_button_release_event" << std::endl;
    //Gtk::DrawingArea::on_button_release_event(event);
    if (!event) { std::cout << "No event!" << std::endl; return false; }
    gdouble xx = event->x;
    gdouble yy = event->y;
    //std::cout << "on button release: " << xx << " " << yy << std::endl;
    Point2d pos;
    pos.x = xx;
    pos.y = yy;
    move_point2(drag_start_pos, pos);
    return false;
  }
  void refresh()
  {
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    ScaleArea area(width,height, sx,sy);
    area.SetMatrix(rot_matrix*mm);
    window_pos = std::vector<Point2d>();
    z_pos = std::vector<float>();
    window_pos2 = std::vector<Point2d>();
    z_pos2 = std::vector<float>();
    int s = changes->NumLines();
    for(int i=0;i<s;i++)
      {
	Point p1 = changes->LinePoint(i, 0);
	Point p2 = changes->LinePoint(i, 1);
	std::pair<Point2d,float> pk1 = area.convert_to_window(p1);
	std::pair<Point2d,float> pk2 = area.convert_to_window(p2);

	window_pos.push_back(pk1.first);
	z_pos.push_back(pk1.second);
	window_pos2.push_back(pk2.first);
	z_pos2.push_back(pk2.second);
      }
  }
  virtual bool on_expose_event(GdkEventExpose* event)
  {
    move_to_be_drawn=false;
    std::cout << "expose" << motion_event << std::endl;
    Glib::RefPtr<Gdk::Window> window = get_window();
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    ScaleArea area(width,height, sx,sy);
    area.SetMatrix(rot_matrix*mm);

    if (!motion_event)
      {
	std::cout << "Motion Draw" << std::endl;
    int s = changes->NumLines();
    cr->set_line_width(1.0);
    cr->set_source_rgb(0.5,0.5,0.5);
    cr->rectangle(event->area.x, event->area.y, event->area.x+event->area.width, event->area.y+event->area.height);
    cr->fill();
    cr->set_source_rgb(0.0,0.0,0.0);
    refresh();
    for(int i=0;i<s;i++)
      {
	Point p1 = changes->LinePoint(i, 0);
	Point p2 = changes->LinePoint(i, 1);

	//Point2d pp1 = { p1.x, p1.y };
	//Point2d pp2 = { p2.x, p2.y };

	std::pair<Point2d,float> pk1 = area.convert_to_window(p1);
	std::pair<Point2d,float> pk2 = area.convert_to_window(p2);

	//std::cout << pk1.first.x << " " << pk1.first.y << std::endl;
	//std::cout << pk2.first.x << " " << pk2.first.y << std::endl;

	cr->move_to(pk1.first.x, pk1.first.y);
	cr->line_to(pk2.first.x, pk2.first.y);
	cr->stroke();
      }
      }
    if (shownum != -1)
      {
	//std::cout << "Draw" << std::endl;
	Point2d p = window_pos[shownum];
	Point2d p2 = window_pos2[shownum];
	Point2d pp;
	if (showbool) pp = p; else pp=p2;
	cr->set_source_rgb(1.0,1.0,1.0);
	cr->set_line_width(4.0);
	cr->move_to(pp.x-4,pp.y-4);
	cr->line_to(pp.x+4.0,pp.y+4.0);
	cr->stroke();
      }
    motion_event=false;
   return true;
  }
private:
  LineCollection *coll;
  LineCollection *changes;
  int sx,sy;
  std::vector<Point2d> window_pos;
  std::vector<float> z_pos;
  std::vector<Point2d> window_pos2;
  std::vector<float> z_pos2;
  Point2d drag_start_pos;
  Matrix mm;
  int shownum;
  bool showbool;
  gfloat lastxx,lastyy;
  bool motion_event;
  bool move_to_be_drawn;
  int xrot;
  int yrot;
  int zrot;
  float xangle;
  float yangle;
  float zangle;
  Matrix rot_matrix;
  FaceCollection *original;
  bool undo_ongoing;
};

void ShowDrawingArea_polys(int argc, char **argv, FaceCollection *coll)
{
  Gtk::Main kit(argc, argv);
  Gtk::Window win;
  win.set_title("Test");
  win.set_default_size(600,600);
  LineDrawingArea area;
  area.set_area(100,100);
  area.set_matrix(Matrix::Translate(100.0,100.0,0.0)*Matrix::Scale(0.5,0.5,0.5));
  area.set_polys(coll);
  win.add(area);
  area.show();
  Gtk::Main::run(win);
}

void ShowDrawingArea_lines(int argc, char **argv, LineCollection *lines)
{
  Gtk::Main kit(argc, argv);
  Gtk::Window win;
  win.set_title("Test");
  win.set_default_size(100,100);
  LineDrawingArea area;
  area.set_area(100,100);
  area.set_coll(lines);
  win.add(area);
  area.show();
  Gtk::Main::run(win);
}
#endif
