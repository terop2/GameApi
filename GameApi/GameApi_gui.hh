
#include "GameApi_h.hh"


#define ORIGINAL_COLORS 1
#ifdef ORIGINAL_COLORS
const unsigned int c_tooltip_button = 0xff888888;
const unsigned int c_tooltip_button2 = 0xff444444;
const unsigned int c_list_item_title = 0xff884422;  // light
const unsigned int c_list_item_title2 = 0xff442211; // dark
const unsigned int c_canvas_item = 0xff888888; // light
const unsigned int c_canvas_item2 = 0xff666666; // dark
const unsigned int c_canvas_item_text_button = 0xff330033; // dark
const unsigned int c_canvas_item_text_button2 = 0xff880088; // light
const unsigned int c_canvas_item_node_0 = 0xffff8844;
const unsigned int c_canvas_item_node_0_2 = 0xff884422;
const unsigned int c_canvas_item_node_1 = 0xff884422;
const unsigned int c_canvas_item_node_1_2 = 0xff442211;
const unsigned int c_dialog_1 = 0xff888888;
const unsigned int c_dialog_1_2 = 0xff444444;
const unsigned int c_dialog_button_1 = 0xff00ff00;
const unsigned int c_dialog_button_2 = 0xff008800;
#endif
#ifdef NEW_COLORS
//const unsigned int c_dark = 0xff7b1fa2;
//const unsigned int c_prim = 0xff9c27b0;
//const unsigned int c_light = 0xffe1bee7;

const unsigned int c_dark = 0xff388e3c;
const unsigned int c_prim = 0xff4caf50;
const unsigned int c_light = 0xffc8e6c9;

const unsigned int c_tooltip_button = c_dark; //0xffe1bee7;
const unsigned int c_tooltip_button2 = c_prim;
const unsigned int c_list_item_title = c_dark; //0xff7b1fa2;  // light
const unsigned int c_list_item_title2 = c_prim; // dark
const unsigned int c_canvas_item = c_dark; //0xff7b1fa2; // light
const unsigned int c_canvas_item2 = c_prim; // dark
const unsigned int c_canvas_item_text_button = c_dark; //0xff7b1fa2; // dark
const unsigned int c_canvas_item_text_button2 = c_prim; // light
const unsigned int c_canvas_item_node_0 = c_prim; //0xff9c27b0;
const unsigned int c_canvas_item_node_0_2 = c_dark;
const unsigned int c_canvas_item_node_1 = c_dark; //0xff7b1fa2;
const unsigned int c_canvas_item_node_1_2 = c_prim;
const unsigned int c_dialog_1 = c_dark;
const unsigned int c_dialog_1_2 = c_prim;
const unsigned int c_dialog_button_1 = c_prim;
const unsigned int c_dialog_button_2 = c_dark;

#endif


bool WidgetCompare(GuiWidget *w1, GuiWidget *w2);


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
    isVisible = true;
    //old_mouse.x = 0.0;
    //old_mouse.y = 0.0;
    sort_widgets();
  }
  void hide() { 
    isVisible=false; 
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	w->hide();
      }
  }
  void show() {
    isVisible=true; 
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	w->show();
      }
  }
  virtual bool is_visible() const
  {
    return isVisible;
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
    sort_widgets();
  }
  virtual void set_size(Vector2d size_p)
  {
    size = size_p;
    // derived widget must override and implement to set child widgets.
  }
  virtual void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    if (vec.size()!=sorted.size()) sort_widgets();
    
    
    int s = vec.size();
    int selected_item = -1;
    int i2 = 0;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = sorted[i];
	if (mouse_pos.y <= w->get_pos().y + w->get_size().dy + 80) { i2=i; break; }
      }

    
    for(int i=i2;i<s;i++)
      {
	GuiWidget *w = sorted[i];
	if (firsttime>0) 
	  w->update(mouse_pos, button,ch, type, mouse_wheel_y);
	
	Point2d p = w->get_pos();
	Vector2d s = w->get_size();
	/*
	if ((mouse_pos.x >= p.x-80 && mouse_pos.x < p.x+s.dx+80 &&
	     mouse_pos.y >= p.y-80 && mouse_pos.y < p.y+s.dy+80)
	   ||
	    (old_mouse.x >= p.x-80 && old_mouse.x < p.x+s.dx+80 &&
	    old_mouse.y >= p.y-80 && old_mouse.y < p.y+s.dy+80))*/
	  {
	    w->update(mouse_pos, button,ch, type, mouse_wheel_y);
	  }
	if (mouse_pos.x >= p.x && mouse_pos.x < p.x+s.dx &&
	    mouse_pos.y >= p.y && mouse_pos.y < p.y+s.dy)
	  {
	    selected_item = i;
	  }
	if (mouse_pos.y<p.y-80) break;
      }
    if (firsttime>0)
      firsttime--;
    if (button==0 && type==1025) {
      current_selected_item = selected_item;
    }
    if (button==-1) {
      current_selected_item = -1;
    }
    old_mouse = mouse_pos;
  }
  virtual void render()
  {
    if (is_visible()) {
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	  GuiWidget *w = vec[i];
	  Point2d p0 = get_pos();
	  Vector2d s0 = get_size();
	  Point2d p = w->get_pos();
	  Vector2d s = w->get_size();
	  /*
	  static float p_max=0.0;
	  static float p_min=100000.0;
	  static float s_max=0.0;
	  static float s_min=100000.0;
	  if (p.y+s.dy>p_max) p_max=p.x+s.dy;
	  if (p.y<p_min) p_min=p.y;
	  if (p.y+s.dy>s_max) s_max=p.y+s.dy;
	  if (p.y<s_min) s_min=p.y;

	  std::cout << p_min << " " << p_max << " " << s_min << " " << s_max << std::endl; 
	  */
	  // check if widget is inside its parent (DOESNT WORK)
	  //if (p.x+s.dx<p0.x || p.x>p0.x+s0.dx || p.y+s.dy<p0.y || p.y>p0.y+s0.dy) continue;
	  
	  if (p.x + s.dx>=0.0 && p.y+s.dy>=0.0 && p.x<float(ev.mainloop_api.get_screen_width()) && p.y<float(ev.mainloop_api.get_screen_height()))
	    { // inside screen.
	      w->render();
	    }
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
	GuiWidgetForward *forw = (GuiWidgetForward*)(vec[val]);
	if (!forw) { return 0; }
	v = &forw->vec;
      }
    return v;
  }
  void sort_widgets()
  {
    sorted=std::vector<GuiWidget*>(vec.begin(),vec.end());
    std::sort(sorted.begin(),sorted.end(),&WidgetCompare);
  }
protected:
  GameApi::EveryApi &ev;
  Point2d pos;
  Vector2d size;
  int current_selected_item;
  int firsttime;
  Point2d prev_pos;
  Point2d old_mouse;
public:
  std::vector<GuiWidget *> vec;
  std::vector<GuiWidget *> sorted;
  bool isVisible;
};


class GuiWidgetRestrict : public GuiWidgetForward
{
public:
  GuiWidgetRestrict(GameApi::EveryApi &ev, std::vector<GuiWidget*> vec) : GuiWidgetForward(ev,vec) { }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
	      GuiWidgetForward::update(mouse_pos,button,ch,type,mouse_wheel_y);
	      /*	Point2d p = get_pos();
	Vector2d s = get_size();
	if (mouse_pos.x>=p.x && mouse_pos.x<p.x+s.dx)
	  if (mouse_pos.y>=p.y && mouse_pos.y<p.y+s.dy)
	    {
	    }*/
  }
};


class CanvasWidget : public GuiWidgetRestrict
{
public:
  CanvasWidget(GameApi::EveryApi &ev, int sx, int sy) : GuiWidgetRestrict(ev, std::vector<GuiWidget*>())
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
