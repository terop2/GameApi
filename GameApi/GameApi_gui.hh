
#include "GameApi_h.hh"

class GuiWidgetForward : public GuiWidget
{
public:
  GuiWidgetForward(GameApi::EveryApi &ev, std::vector<GuiWidget*> vec) : ev(ev), vec(vec) { pos=Point2d::NewPoint(0.0, 0.0); size.dx=0.0; size.dy=0.0; current_selected_item=-1; firsttime = 10;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	assert(w);
      }
    prev_pos.x = 0.0;
    prev_pos.y = 0.0;

  }
  virtual Point2d get_pos() const { return pos; }
  virtual Vector2d get_size() const { return size; }
  virtual void set_pos(Point2d new_pos) {
    if (new_pos.x != prev_pos.x || new_pos.y != prev_pos.y)
      {
	Point2d old_pos = pos;
	int s = vec.size();
	for(int i=0;i<s;i++)
	  {
	    GuiWidget *w = vec[i];
	    Point2d child_old_pos = w->get_pos();
	    Point2d child_new_pos = child_old_pos;
	    child_new_pos -= old_pos;
	    child_new_pos += new_pos;
	    w->set_pos(child_new_pos);
	  }
	pos = new_pos;
	prev_pos = new_pos;
      }
  }
  virtual void set_size(Vector2d size_p)
  {
    size = size_p;
    // derived widget must override and implement to set child widgets.
  }
  virtual void update(Point2d mouse_pos, int button, int ch, int type)
  {
    int s = vec.size();
    int selected_item = -1;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	if (firsttime>0) 
	  w->update(mouse_pos, button,ch, type);
	
	Point2d p = w->get_pos();
	Vector2d s = w->get_size();
	if (mouse_pos.x >= p.x-80 && mouse_pos.x < p.x+s.dx+80 &&
	    mouse_pos.y >= p.y-80 && mouse_pos.y < p.y+s.dy+80)
	  {
	    w->update(mouse_pos, button,ch, type);
	  }
	if (mouse_pos.x >= p.x && mouse_pos.x < p.x+s.dx &&
	    mouse_pos.y >= p.y && mouse_pos.y < p.y+s.dy)
	  {
	    selected_item = i;
	  }
      }
    if (firsttime>0)
      firsttime--;
    if (button==0 && type==1025) {
      current_selected_item = selected_item;
    }
    if (button==-1) {
      current_selected_item = -1;
    }
  }
  virtual void render()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	Point2d p = w->get_pos();
	Vector2d s = w->get_size();
	if (p.x + s.dx>=0.0 && p.y+s.dy>=0.0 && p.x<1200.0 && p.y<1000.0)
	  { // inside screen.
	    w->render();
	  }
      }
  }
  virtual int render_to_bitmap();

  virtual void select_item(int item) {
    current_selected_item=item; 
  }
  virtual int chosen_item() const
  {
    return current_selected_item;
  }
  virtual float dynamic_param(int id) const { return 0.0; }
  virtual void set_dynamic_param(int id, float val)
  {
  }
  virtual bool content_changed() const
  {
    int s = vec.size();
    bool changed = false;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	bool b = w->content_changed();
	changed |= b;
      }
    return changed;
  }
  virtual void clear_content_changed()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	w->clear_content_changed();
      }
  }
  virtual int child_count() const { return vec.size(); }
  virtual GuiWidget *child(int num) const { return vec[num]; }
  virtual std::vector<GuiWidget*> *child_from_path(std::string path)
  {
    std::vector<GuiWidget*> *v = &vec;
    int s = 0;
    for(int i=0;i<s;i++)
      {
	char c = path[i];
	int val = int(c - '0');
	GuiWidgetForward *forw = dynamic_cast<GuiWidgetForward*>(vec[val]);
	if (!forw) { return 0; }
	v = &forw->vec;
      }
    return v;
  }
protected:
  GameApi::EveryApi &ev;
  Point2d pos;
  Vector2d size;
  int current_selected_item;
  int firsttime;
  Point2d prev_pos;
public:
  std::vector<GuiWidget *> vec;
};


class CanvasWidget : public GuiWidgetForward
{
public:
  CanvasWidget(GameApi::EveryApi &ev, int sx, int sy) : GuiWidgetForward(ev, std::vector<GuiWidget*>())
  {
    size.dx = sx;
    size.dy = sy;
  }
  int count() const { return vec.size(); }
  int find_index(GuiWidget *w)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	if (vec[i]==w) { return i; }
      }
    return -1;
  }
  GuiWidget *find_widget(int i) const { return vec[i]; }
  GuiWidget *find_widget(std::string id) const
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	std::string w_id = w->get_id();
	if (id==w_id) return w;
      }
    return 0;
  }
  int push_back(GuiWidget *widget, int x, int y)
  {
    vec.push_back(widget);
    pos_x.push_back(x);
    pos_y.push_back(y);
    Vector2d v = { float(x),float(y) };
    widget->set_pos(get_pos() + v);
    return vec.size()-1;
  }
  void del(int id)
  {
    vec.erase(vec.begin()+id);
  }
private:
  std::vector<int> pos_x;
  std::vector<int> pos_y;
};