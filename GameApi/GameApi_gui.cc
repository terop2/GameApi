
#include "GameApi_h.hh"
#include "GameApi_gui.hh"

const float keypress_rot_speed = 6.0;

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


class EmptyWidget : public GuiWidgetForward
{
public:
  EmptyWidget(GameApi::EveryApi &ev) : GuiWidgetForward(ev,{ }) { }
};
class TimedWidget2 : public GuiWidgetForward
{
public:
  TimedWidget2(GameApi::EveryApi &ev, GuiWidget *next, float time) : GuiWidgetForward(ev, { next } ), time(time) 
  {
    old_time = ev.mainloop_api.get_time();
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type);
    float current_time = ev.mainloop_api.get_time();
    float delta = current_time - old_time;
    if (delta > time && vec.size()>0)
      {
	vec.erase(vec.begin());
      }
  }  
private:
  float old_time;
  float time;
};
class TimedWidget : public GuiWidgetForward
{
public:
  TimedWidget(GameApi::EveryApi &ev, GuiWidget *next, GuiWidget *timed, GuiWidgetForward *insert_wid, float start_duration, float duration, float dx) : GuiWidgetForward(ev, {next}), next(next), timed(timed), insert_wid(insert_wid), start_duration(start_duration), duration(duration),dx(dx) 
  {
    uid = unique_id();
    state = 0;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);

  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type);
    Point2d pos = vec[0]->get_pos();
    Vector2d sz = vec[0]->get_size();
    if (state==0 && mouse_pos.x>pos.x && mouse_pos.x<pos.x+sz.dx &&
	mouse_pos.y>pos.y && mouse_pos.y<pos.y+sz.dy)
      {
	state=1;
	state1_time = ev.mainloop_api.get_time();
      }
    float current_time = ev.mainloop_api.get_time();
    if (state==1 && current_time-state1_time > start_duration)
      {
	state=2;
	state2_time = ev.mainloop_api.get_time();
	// this only works with tooltips
	Point2d p = next->get_pos();
	Vector2d ps = next->get_size();
	Point2d pos = { float(p.x+ps.dx-dx), float(p.y+0.0) };
	timed->set_pos(pos);


	GuiWidget *tm = new TimedWidget2(ev, timed, duration);
	tm->set_id(uid);
	insert_wid->vec.push_back(tm);
	index = insert_wid->vec.size()-1;
      }
    if (state==2 && (current_time-state2_time > duration||
	!(mouse_pos.x>pos.x && mouse_pos.x<pos.x+sz.dx &&
	  mouse_pos.y>pos.y && mouse_pos.y<pos.y+sz.dy))
	)
      {
	state=3;
	int s = insert_wid->vec.size();
	for(int i=0;i<s;i++)
	  {
	    GuiWidget* w = insert_wid->vec[i];
	    std::string id = w->get_id();
	    if (id==uid)
	      {
		insert_wid->vec.erase(insert_wid->vec.begin()+i);
		break; 
	      }
	  }
      }    
    if (state==3 && (
	!(mouse_pos.x>pos.x && mouse_pos.x<pos.x+sz.dx &&
	  mouse_pos.y>pos.y && mouse_pos.y<pos.y+sz.dy))
	) {
      state=0;
    }

  }
private:
  std::string uid;
  GuiWidget *next;
  GuiWidget *timed;
  GuiWidgetForward *insert_wid;
  float start_duration;
  float duration;
  int state;
  float state1_time;
  float state2_time;
  int index;
  float dx;
};

class TextGuiWidgetAtlas : public GuiWidgetForward
{
public:
  TextGuiWidgetAtlas(GameApi::EveryApi &ev, std::string label, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::SH sh, int x_gap) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), label(label), atlas(atlas), atlas_bm(atlas_bm), sh(sh), x_gap(x_gap) { firsttime = true; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2); 
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    if (firsttime)
      {
	rendered_bitmap = ev.font_api.font_string_from_atlas(ev, atlas, atlas_bm, label.c_str(), x_gap);
	GameApi::CBM sca = ev.cont_bitmap_api.from_bitmap(rendered_bitmap, 1.0, 1.0);
	int sx = ev.bitmap_api.size_x(rendered_bitmap);
	int sy = ev.bitmap_api.size_y(rendered_bitmap);
	scaled_bitmap = ev.cont_bitmap_api.sample(sca, sx/2, sy/2);
	rendered_bitmap_va = ev.sprite_api.create_vertex_array(scaled_bitmap);
	firsttime = false;
      }
    size.dx = ev.bitmap_api.size_x(scaled_bitmap);
    size.dy = ev.bitmap_api.size_y(scaled_bitmap);
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	ev.sprite_api.render_sprite_vertex_array(rendered_bitmap_va);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
  }
  int render_to_bitmap()
  {
    return rendered_bitmap.id;
  }
private:
  bool firsttime;
  std::string label;
  GameApi::FtA atlas;
  GameApi::BM atlas_bm;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
  GameApi::BM scaled_bitmap;
  GameApi::VA rendered_bitmap_va;
  int x_gap;
};

template<class T>
class EditorGuiWidget : public GuiWidgetForward
{
public:
  EditorGuiWidget(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::Ft font, GameApi::SH sh) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), font(font), sh(sh) { firsttime = true; active=false; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    bool changed = false;
    Point2d pos = get_pos();
    Vector2d sz = get_size();
    if (button == 0 && type==1025 && mouse.x>=pos.x && mouse.x < pos.x+sz.dx
	&& mouse.y>=pos.y && mouse.y<pos.y+sz.dy)
      {
	active = true;
      }
    else if (button==0)
      {
	active = false;

      }

    if (firsttime)
      {
	std::stringstream ss;
	ss << target;
	label = ss.str();
      }

    if (active)
      {
	int s = allowed_chars.size();
	for(int i=0;i<s;i++)
	  {
	    if (allowed_chars[i]==ch)
	      {
		label.push_back(ch);
		changed = true;
	      }
	  }
	if (ch==8 && label.size()>0)
	  {
	    label.erase(label.begin()+(label.size()-1));
	    changed = true;
	  }
      }

    // note, spaces are not allowed.
    if (active) {
      //std::cout << "EDITOR UPDATES: " << label << std::endl;
      std::stringstream ss2(label);
      ss2 >> target;
      //std::cout << "EDITOR UPDATES2: " << target << std::endl;
    }

    if (firsttime || changed)
      {
	rendered_bitmap = ev.font_api.font_string(font, label.c_str(), 2);
	rendered_bitmap_va = ev.sprite_api.create_vertex_array(rendered_bitmap);
	firsttime = false;
      }
    size.dx = ev.bitmap_api.size_x(rendered_bitmap);
    size.dy = ev.bitmap_api.size_y(rendered_bitmap);
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	ev.sprite_api.render_sprite_vertex_array(rendered_bitmap_va);
      }
  }
  int render_to_bitmap()
  {
    return rendered_bitmap.id;
  }
private:
  bool firsttime;
  std::string allowed_chars;
  T &target; 
  std::string label;
  std::string oldlabel;
  GameApi::Ft font;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
  GameApi::VA rendered_bitmap_va;
  bool active;
};

template<class T>
class Conv {
public:
  static void set(T &target, std::string s) { std::stringstream ss(s); ss >> target; }
  static void get(const T &target, std::string &s) { std::stringstream ss; ss << target; s=ss.str(); }
};
template<>
class Conv<std::string>
{
public:
  static void set(std::string &target, std::string s) { target = s; }
  static void get(const std::string &target, std::string &s) { s=target; }
};

template<class T>
class EditorGuiWidgetAtlas : public GuiWidgetForward
{
public:
  EditorGuiWidgetAtlas(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::SH sh, int x_gap) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), atlas(atlas), atlas_bm(atlas_bm), sh(sh), x_gap(x_gap) { firsttime = true; active=false; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    shift=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    bool changed = false;
    Point2d pos = get_pos();
    Vector2d sz = get_size();
    if (button == 0 && type==1025 && mouse.x>=pos.x && mouse.x < pos.x+sz.dx
	&& mouse.y>=pos.y && mouse.y<pos.y+sz.dy)
      {
	active = true;
      }
    else if (button==0 && type==1025)
      {
	active = false;
      }
    //std::cout << type << " " << ch << std::endl;
    if (type==768 && (ch==1073742049||ch==1073742053)) { shift=true; std::cout << "Shift1" << std::endl; }
    if (type==769 && (ch==1073742049||ch==1073742053)) { shift=false; std::cout << "Shift2" << std::endl; }

    if (firsttime)
      {
	Conv<T>::get(target, label);
	//std::stringstream ss;
	//ss << target;
	//label = target; //ss.str();
      }
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (ch==13) { ch='\n'; }
    if (shift) { ch = std::toupper(ch); }
    if (active && type==768)
      {
	int s = allowed_chars.size();
	for(int i=0;i<s;i++)
	  {
	    if (allowed_chars[i]==ch)
	      {
		label.push_back(ch);
		changed = true;
	      }
	  }
	if ((ch==8 ||ch==42) && label.size()>0)
	  {
	    label.erase(label.begin()+(label.size()-1));
	    changed = true;
	  }
      }

    // note, spaces are not allowed.
    if (active) {
      //std::cout << "EDITOR UPDATES: " << label << std::endl;
      //std::stringstream ss2(label);
      //ss2 >> target;
      Conv<T>::set(target, label);
      //std::cout << "EDITOR UPDATES2: " << target << std::endl;
    }

    if (firsttime || changed)
      {
	rendered_bitmap = ev.font_api.font_string_from_atlas(ev, atlas, atlas_bm, label.c_str(), x_gap);
	int sx = ev.bitmap_api.size_x(rendered_bitmap);
	int sy = ev.bitmap_api.size_y(rendered_bitmap);
	GameApi::CBM cbm = ev.cont_bitmap_api.from_bitmap(rendered_bitmap, 1.0,1.0);
	scaled_bitmap = ev.cont_bitmap_api.sample(cbm, sx/2, sy/2);
	rendered_bitmap_va = ev.sprite_api.create_vertex_array(scaled_bitmap);
	firsttime = false;
      }
    size.dx = ev.bitmap_api.size_x(scaled_bitmap);
    size.dy = ev.bitmap_api.size_y(scaled_bitmap);
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	ev.sprite_api.render_sprite_vertex_array(rendered_bitmap_va);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      }
  }
  int render_to_bitmap()
  {
    return rendered_bitmap.id;
  }
private:
  bool firsttime;
  std::string allowed_chars;
  T &target; 
  std::string label;
  GameApi::FtA atlas;
  GameApi::BM atlas_bm;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
  GameApi::BM scaled_bitmap;
  GameApi::VA rendered_bitmap_va;
  bool active;
  int x_gap;
  bool shift;
};


class PolyGuiWidget : public GuiWidgetForward
{
public:
  PolyGuiWidget(GameApi::EveryApi &ev, GameApi::P p, GameApi::SH sh, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), old_sh(old_sh), p(p), obj(ev, p, sh),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (ch=='a' && type==0x300)
      {
	left = true;
      }
    if (ch=='a' && type==0x301)
      {
	left = false;
      }
    if (ch=='d' && type==0x300)
      {
	right=true;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }

    if (firsttime)
      {
      obj.prepare();
      firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_y(rot_y);
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_y(rot_y);
      }
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	glEnable(GL_DEPTH_TEST);
	obj.render();
	glDisable(GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
  }
private:
  GameApi::SH sh;
  GameApi::SH old_sh;
  GameApi::P p;
  GameApi::PolygonObj obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
};

class VAGuiWidget : public GuiWidgetForward
{
public:
  VAGuiWidget(GameApi::EveryApi &ev, GameApi::VA p, GameApi::SH sh, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), old_sh(old_sh), p(p),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    mat = ev.matrix_api.identity();
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (ch=='a' && type==0x300)
      {
	left = true;
      }
    if (ch=='a' && type==0x301)
      {
	left = false;
      }
    if (ch=='d' && type==0x300)
      {
	right = true;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }

    if (firsttime)
      {
	//obj.prepare();
      firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	//obj.set_rotation_y(rot_y);
	mat = ev.matrix_api.yrot(rot_y);
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	//obj.set_rotation_y(rot_y);
	mat = ev.matrix_api.yrot(rot_y);
      }
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	glEnable(GL_DEPTH_TEST);
	//obj.render();
	ev.shader_api.set_var(sh, "in_MV", mat);
	ev.polygon_api.render_vertex_array(p);
	glDisable(GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
  }
private:
  GameApi::SH sh;
  GameApi::SH old_sh;
  GameApi::VA p;
  GameApi::M mat;
  //GameApi::PolygonObj obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
};

class MLGuiWidget : public GuiWidgetForward
{
public:
  MLGuiWidget(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::ML p, GameApi::SH sh, GameApi::SH sh2, GameApi::SH sh_arr, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), env(env), sh(sh), sh2(sh2), sh_arr(sh_arr), old_sh(old_sh), p(p),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    mat = ev.matrix_api.identity();
    e.cursor_pos = ev.point_api.point(0.0,0.0,0.0);
    e.type = -1;
    e.ch = -1;
    e.button = -1;
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    if (type==0x300 || type==0x301)
      {
	e.type = type;
	e.ch = ch;
	e.button = button;
	e.cursor_pos = ev.point_api.point(mouse.x,mouse.y,0.0);
      }
    if (!firsttime)
      {
	ev.mainloop_api.event_ml(p, e);
      }

#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (ch=='a' && type==0x300)
      {
	left=true;
      }
    if (ch=='a' && type==0x301)
      {
	left=false;
      }
    if (ch=='d' && type==0x300)
      {
	right=true;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }

    if (firsttime)
      {
	//obj.prepare();
      firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	//obj.set_rotation_y(rot_y);
	mat = ev.matrix_api.yrot(rot_y);
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	//obj.set_rotation_y(rot_y);
	mat = ev.matrix_api.yrot(rot_y);
      }
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	ev.shader_api.use(sh2);
	ev.mainloop_api.switch_to_3d(true, sh2, screen_x, screen_y);
	ev.shader_api.use(sh_arr);
	ev.mainloop_api.switch_to_3d(true, sh_arr, screen_x, screen_y);
	glEnable(GL_DEPTH_TEST);
	//obj.render();
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "in_MV", mat);
	ev.shader_api.use(sh2);
	ev.shader_api.set_var(sh2, "in_MV", mat);
	ev.shader_api.use(sh_arr);
	ev.shader_api.set_var(sh_arr, "in_MV", mat);
	ev.shader_api.use(sh);
	GameApi::M in_T = ev.mainloop_api.in_T(ev, true);
	GameApi::M in_N = ev.mainloop_api.in_N(ev, true);
	//e.inMV = find_matrix(env, mat);
	ev.mainloop_api.execute_ml(p, sh, sh2, sh_arr,mat, in_T, in_N);
	e.type = -1;
	e.ch = -1;
	e.button = -1;

	//ev.polygon_api.render_vertex_array(p);
	glDisable(GL_DEPTH_TEST);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	ev.shader_api.use(sh2);
	ev.mainloop_api.switch_to_3d(false, sh2, screen_x, screen_y);
	ev.shader_api.use(sh_arr);
	ev.mainloop_api.switch_to_3d(false, sh_arr, screen_x, screen_y);
	glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
  }
private:
  GameApi::Env &env;
  GameApi::SH sh;
  GameApi::SH sh2;
  GameApi::SH sh_arr;
  GameApi::SH old_sh;
  GameApi::ML p;
  GameApi::M mat;
  GameApi::MainLoopApi::Event e;
  //GameApi::PolygonObj obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
};



class ShaderPlaneGuiWidget : public GuiWidgetForward
{
public:
  ShaderPlaneGuiWidget(GameApi::EveryApi &ev, GameApi::SFO p, GameApi::SH sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), p(p),sx(sx),sy(sy),screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    time = ev.mainloop_api.get_time();
    left=false; right=false;

  }
  GameApi::V func(int face, int point, GameApi::EveryApi &ev)
  {
    switch(point)
      {
      case 0: return ev.vector_api.vector(0.0,0.0,0.0);
      case 1: return ev.vector_api.vector(1.0,0.0,0.0);
      case 2: return ev.vector_api.vector(1.0,1.0,0.0);
      case 3: return ev.vector_api.vector(0.0,1.0,0.0);
      };
    return ev.vector_api.vector(0.0,0.0,0.0);
  }
  
  GameApi::PT func2(int face, int point, GameApi::EveryApi &ev)
  {
    switch(point)
      {
      case 0: return ev.point_api.point(0.0,1.0,0.0);
      case 1: return ev.point_api.point(1.0,1.0,0.0);
      case 2: return ev.point_api.point(1.0,0.0,0.0);
      case 3: return ev.point_api.point(0.0,0.0,0.0);
      };
    return ev.point_api.point(0.0,0.0,0.0);
  }
  

  void update(Point2d mouse, int button, int ch, int type)
  {

#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif

    if (ch=='a' && type==0x300)
      {
	left = true;
      }
    if (ch=='a' && type==0x301)
      {
	left = false;
      }
    if (ch=='d' && type==0x300)
      {
	right=true;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }


    using std::placeholders::_1;
    using std::placeholders::_2;
    if (firsttime)
      {
	GameApi::SFO render = ev.sh_api.render(p);
	sh2 = ev.shader_api.get_normal_shader("screen", "screen", "", "", "", false, render);
	ev.mainloop_api.init(sh2, screen_x, screen_y);
	ev.mainloop_api.alpha(true);
	GameApi::P poly_0 = ev.polygon_api.quad_z(0.0, sx,
					 0.0, sy,
					 0.0);
	GameApi::P poly_1 = ev.polygon_api.normal_function(poly_0, std::bind(&ShaderPlaneGuiWidget::func, this, _1, _2, std::ref(ev)));
	GameApi::P poly_2 = ev.polygon_api.texcoord_function(poly_1, std::bind(&ShaderPlaneGuiWidget::func2, this, _1, _2, std::ref(ev)));
	obj = new GameApi::PolygonObj(ev, poly_2, sh2);

	obj->prepare();
	firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
      }
    if (!firsttime)
      {
	float f = (ev.mainloop_api.get_time()-time)/1000.0;
	ev.shader_api.use(sh2);
	ev.shader_api.set_var(sh2, "roty", rot_y);
	ev.shader_api.set_var(sh2, "time", f);
	Point2d pos = get_pos();
	//Vector2d sz = get_size();
	obj->set_pos(pos.x, pos.y, 0.0);
	obj->render();
	ev.shader_api.use(sh);
      }
  }
private:
  GameApi::SH sh;
  GameApi::SH sh2;
  GameApi::SFO p;
  GameApi::PolygonObj *obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  float time;
  float left,right;
};


class PTSGuiWidget : public GuiWidgetForward
{
public:
  PTSGuiWidget(GameApi::EveryApi &ev, GameApi::PTS p, GameApi::SH sh, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), old_sh(old_sh), p(p), obj(ev, p, sh),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (ch=='a' && type==0x300)
      {
	left=true;
      }
    if (ch=='a' && type==0x301)
      {
	left=false;
      }
    if (ch=='d' && type==0x300)
      {
	right=true;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }

    if (firsttime)
      {
      obj.prepare();
      firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	glEnable(GL_DEPTH_TEST);
	//glPointSize(5.0);
	//ev.mainloop_api.clear_3d();
	obj.render();
	glDisable(GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
  }
private:
  GameApi::SH sh;
  GameApi::SH old_sh;
  GameApi::PTS p;
  GameApi::PointsObj obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
};


class LinesGuiWidget : public GuiWidgetForward
{
public:
  LinesGuiWidget(GameApi::EveryApi &ev, GameApi::LI p, GameApi::SH sh, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), old_sh(old_sh), p(p), obj(ev, p, sh),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (ch=='a' && type==0x300)
      {
	left=true;
      }
    if (ch=='d' && type==0x300)
      {
	right=true;
      }
    if (ch=='a' && type==0x301)
      {
	left=false;
      }
    if (ch=='d' && type==0x301)
      {
	right=false;
      }

    if (firsttime)
      {
      obj.prepare();
      firsttime = false;
      }
    size.dx = sx;
    size.dy = sy;
  }  
  void render()
  {
    if (left)
      {
	rot_y+=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (right)
      {
	rot_y-=1.0*3.14159*2.0/360.0*ev.mainloop_api.get_delta_time()*keypress_rot_speed;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	glEnable(GL_DEPTH_TEST);
	obj.render();
	glDisable(GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
  }
private:
  GameApi::SH sh;
  GameApi::SH old_sh;
  GameApi::LI p;
  GameApi::LinesObj obj;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
};

class IconGuiWidget : public GuiWidgetForward
{
public:
  IconGuiWidget(GameApi::EveryApi &ev, GameApi::BM bm, GameApi::SH sh) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), bm(bm) { firsttime=true;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);

  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    if (firsttime)
      {
	bm_va = ev.sprite_api.create_vertex_array(bm);
	firsttime = false;
      }
    size.dx = ev.bitmap_api.size_x(bm);
    size.dy = ev.bitmap_api.size_y(bm);
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
  }
  int render_to_bitmap()
  {
    return bm.id;
  }
private:
  bool firsttime;
  GameApi::SH sh;
  GameApi::BM bm;
  GameApi::VA bm_va;
};
class MarginGuiWidget : public GuiWidgetForward
{
public:
  MarginGuiWidget(GameApi::EveryApi &ev, GuiWidget *w, int l, int t, int r, int b) : GuiWidgetForward(ev, { w }), l(l), t(t), r(r), b(b) 
  {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
    Vector2d delta = { float(l+r), float(t+b) };
    vec[0]->set_size(size + (- delta));
  }
  void set_pos(Point2d p)
  {
    GuiWidgetForward::set_pos(p);
    Vector2d v = { float(l), float(t) };
    vec[0]->set_pos(p+v);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);
    Vector2d v = vec[0]->get_size();
    Vector2d vv = { float(l+r), float(t+b) };
    Vector2d vvv = v+vv;
    size = vvv;
  }
  int chosen_item() const
  {
    int val = vec[0]->chosen_item();
    return val;
  }

private:
  int l,t,r,b;
};
class LayerGuiWidget : public GuiWidgetForward
{
public:
  LayerGuiWidget(GameApi::EveryApi &ev, GuiWidget *w1, GuiWidget *w2) : GuiWidgetForward(ev, { w1, w2 }) 
  { 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    vec[0]->set_pos(pos);
    vec[1]->set_pos(pos);
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
    vec[0]->set_size(size);
    vec[1]->set_size(size);
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type);
    Vector2d v1 = vec[0]->get_size();
    Vector2d v2 = vec[1]->get_size();
    Vector2d mx = { std::max(v1.dx, v2.dx), std::max(v1.dy,v2.dy) };
    size = mx;
  }
  int chosen_item() const
  {
    int val = vec[0]->chosen_item();
    if (val != -1) return val;
    return vec[1]->chosen_item();
  }
};
class ArrayXWidget : public GuiWidgetForward
{
public:
  ArrayXWidget(GameApi::EveryApi &ev, std::vector<GuiWidget*> vec, int x_gap) : GuiWidgetForward(ev, vec), x_gap(x_gap) 
  {
    Point2d pos = { -666.0, -666.0 };
    update(pos, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    int s = vec.size();
    float delta = 0;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	Point2d p = { pos.x + delta, pos.y };
	w->set_pos(p);
	delta += w->get_size().dx + x_gap;
      }
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);
    int s = vec.size();
    float sz = 0;
    float sz_max = 0;
    for(int i=0;i<s; i++)
      {
	sz += vec[i]->get_size().dx + x_gap;
	sz_max = std::max(sz_max, vec[i]->get_size().dy);
      }
    size.dx = sz;
    size.dy = sz_max;
    set_pos(get_pos());
  }
  void render()
  {
    GuiWidgetForward::render();
  }
private:
  int x_gap;
};

class ArrayYWidget : public GuiWidgetForward
{
public:
  ArrayYWidget(GameApi::EveryApi &ev, std::vector<GuiWidget*> vec, int y_gap) : GuiWidgetForward(ev, vec), y_gap(y_gap) 
  {
    Point2d pos = { -666.0, -666.0 };
    update(pos, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    int s = vec.size();
    float delta = 0;
    for(int i=0;i<s;i++)
      {
	GuiWidget *w = vec[i];
	Point2d p = { pos.x, pos.y +delta};
	w->set_pos(p);
	delta += w->get_size().dy + y_gap;
      }
  }
  void set_size(Vector2d size)
  {
    GuiWidgetForward::set_size(size);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);
    int s = vec.size();
    float sz = 0;
    float sz_max = 0;
    for(int i=0;i<s; i++)
      {
	sz += vec[i]->get_size().dy + y_gap;
	sz_max = std::max(sz_max, vec[i]->get_size().dx);
      }
    size.dx = sz_max;
    size.dy = sz;

    selected_item = -1;

    for(int j=0;j<s;j++) 
      {
	GuiWidget *w = vec[j];
	Point2d pos = w->get_pos();
	Vector2d size = w->get_size();
	if (mouse.x>=pos.x && mouse.x<pos.x+size.dx &&
	    mouse.y>=pos.y && mouse.y<pos.y+size.dy)
	  {
	    selected_item = j;
	  }
      }

  }
  int chosen_item() const { return selected_item; }
  void render()
  {
    GuiWidgetForward::render();
  }
private:
  int y_gap;
  int selected_item;
};

class HighlightGuiWidget : public GuiWidgetForward
{
public:
  HighlightGuiWidget(GameApi::EveryApi &ev, GameApi::SH sh, int sx, int sy) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), enabled(false), oldenabled(false), c_changed(false) { 
    firsttime=true; 
    size.dx = sx;
    size.dy = sy;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1);
  }
  void update(Point2d mouse, int button,int ch, int type)
  {
    Point2d p = get_pos();
    Vector2d s = get_size();
    if (mouse.x >= p.x && mouse.x < p.x+s.dx && mouse.y>=p.y && mouse.y< p.y+s.dy)
      {
	//std::cout << "Enabled: " << mouse.x << " " << mouse.y << std::endl;
	//std::cout << p.x << " " << p.y << " " << s.dx << " " << s.dy << std::endl;
	enabled = true;
      }
    else
      {
	enabled = false;
      }
    if (enabled!=oldenabled)
      {
	c_changed = true;
	oldenabled = enabled;
      }
    if (firsttime)
      {
	Vector2d v = get_size();
	GameApi::BB bg = ev.bool_bitmap_api.empty(v.dx, v.dy);
	GameApi::BB bb_1 = ev.bool_bitmap_api.rectangle(bg, 0.0,0.0, 2.0, (float)v.dy);
	GameApi::BB bb_2 = ev.bool_bitmap_api.rectangle(bb_1, 0.0,0.0, (float)v.dx, 2.0);
	GameApi::BB bb_3 = ev.bool_bitmap_api.rectangle(bb_2, v.dx-2.0,0.0, 2.0, v.dy);
	GameApi::BB bb_4 = ev.bool_bitmap_api.rectangle(bb_3, 0.0, v.dy-2, v.dx, 2.0);
	bm = ev.bool_bitmap_api.to_bitmap(bb_4, 255,255,255,255, 0,0,0,0);
	bm_va = ev.sprite_api.create_vertex_array(bm);
	empty_bm = ev.bool_bitmap_api.to_bitmap(bg, 255,255,255,255, 0,0,0,0);
	empty_bm_va = ev.sprite_api.create_vertex_array(empty_bm);
	firsttime = false;
      }
  }
  void render()
  {
    if (!firsttime && enabled)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5, p.y+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
  }
  int render_to_bitmap()
  {
    if (enabled)
      return bm.id;
    else
      return empty_bm.id;
  }
  bool content_changed() const
  {
    return c_changed;
  }
  void clear_content_changed() { c_changed = false; }
private:
  GameApi::BM bm;
  GameApi::BM empty_bm;
  GameApi::VA bm_va;
  GameApi::VA empty_bm_va;
  GameApi::SH sh;
  bool firsttime;
  bool enabled;
  bool oldenabled;
  bool c_changed;
};


class Highlight2GuiWidget : public GuiWidgetForward
{
public:
  Highlight2GuiWidget(GameApi::EveryApi &ev, GuiWidget *wid, GameApi::SH sh) : GuiWidgetForward(ev, {wid} ), sh(sh), enabled(false), oldenabled(false), c_changed(false) { 
    firsttime=true; 
    changed=false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
  }
  void update(Point2d mouse, int button,int ch, int type)
  {
    GuiWidgetForward::update(mouse, button,ch, type);
    Point2d p = get_pos();
    Vector2d s = get_size();
    if (mouse.x >= p.x && mouse.x < p.x+s.dx && mouse.y>=p.y && mouse.y< p.y+s.dy)
      {
	enabled = true;
      }
    else
      {
	enabled = false;
      }
    if (enabled!=oldenabled)
      {
	c_changed = true;
	oldenabled = enabled;
      }
    oldsize = size;
    size = vec[0]->get_size();

    if (size.dx != oldsize.dx ||size.dy!=oldsize.dy)
      {
	
	changed = true;
      }

    if (firsttime ||changed)
      {
	Vector2d v = get_size();
	GameApi::BB bg = ev.bool_bitmap_api.empty(v.dx, v.dy);
	GameApi::BB bb_1 = ev.bool_bitmap_api.rectangle(bg, 0.0,0.0, 2.0, (float)v.dy);
	GameApi::BB bb_2 = ev.bool_bitmap_api.rectangle(bb_1, 0.0,0.0, (float)v.dx, 2.0);
	GameApi::BB bb_3 = ev.bool_bitmap_api.rectangle(bb_2, v.dx-2.0,0.0, 2.0, v.dy);
	GameApi::BB bb_4 = ev.bool_bitmap_api.rectangle(bb_3, 0.0, v.dy-2, v.dx, 2.0);
	bm = ev.bool_bitmap_api.to_bitmap(bb_4, 255,255,255,255, 0,0,0,0);
	bm_va = ev.sprite_api.create_vertex_array(bm);
	empty_bm = ev.bool_bitmap_api.to_bitmap(bg, 255,255,255,255, 0,0,0,0);
	empty_bm_va = ev.sprite_api.create_vertex_array(empty_bm);
	firsttime = false;
	changed = false;
      }
  }
  void render()
  {
    GuiWidgetForward::render();
    if (!firsttime && enabled)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5, p.y+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
  }
  int render_to_bitmap()
  {
    if (enabled)
      return bm.id;
    else
      return empty_bm.id;
  }
  bool content_changed() const
  {
    return c_changed;
  }
  void clear_content_changed() { c_changed = false; }
private:
  GameApi::BM bm;
  GameApi::BM empty_bm;
  GameApi::VA bm_va;
  GameApi::VA empty_bm_va;
  GameApi::SH sh;
  bool firsttime;
  bool changed;
  bool enabled;
  bool oldenabled;
  bool c_changed;
  Vector2d oldsize;
};

class LineWidget : public GuiWidgetForward
{
public:
  LineWidget(GameApi::EveryApi &ev, GameApi::SH sh, GuiWidget *t1, int delta_x, int delta_y,
	     GuiWidget *t2, int delta2_x, int delta2_y, GameApi::SH sh2)
    : GuiWidgetForward(ev, { } ), sh(sh), t1(t1), delta_x(delta_x), delta_y(delta_y),
      t2(t2), delta2_x(delta2_x), delta2_y(delta2_y),sh2(sh2) { firsttime = true; 
    Point2d p = t1->get_pos();
    p.x += delta_x;
    p.y += delta_y;
    Point2d p2 = t2->get_pos();
    p2.x += delta2_x;
    p2.y += delta2_y;
    Point2d pp = { std::min(p.x,p2.x), std::min(p.y,p2.y) };
    Point2d pp2 = { std::max(p.x,p2.x), std::max(p.y,p2.y) };
    Vector2d sz = pp2-pp;
    pos = pp;
    size = sz;
    Vector2d d = { 0.0, 0.0 };
    delta_vec = d;
}
  void set_pos(Point2d p2)
  {
    //Point2d p = pos;
    GuiWidgetForward::set_pos(p2);
    //delta_vec = p2 - p + delta_vec;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);

  }
  GameApi::PT line_func(int linenum, bool id)
  {
    switch(id) {
    case false: 
      {
	Point2d pos = t1->get_pos();
	pos.x += delta_x;
	pos.y += delta_y;
	pos+=delta_vec;
	return ev.point_api.point(pos.x, pos.y, 0.0);
      }
    case true:
      {
	Point2d pos = t2->get_pos();
	pos.x += delta2_x;
	pos.y += delta2_y;
	pos+=delta_vec;
	return ev.point_api.point(pos.x, pos.y, 0.0);
      }
    };
    return ev.point_api.point(0.0,0.0,0.0);
  }
  GameApi::P line_to_poly(int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)
  {
    //std::cout << sx << " " << sy << " " << sz << ":" << ex << " " << ey << " " << ez << std::endl;
    GameApi::PT p1 = ev.point_api.point(sx-2,sy,0.0);
    GameApi::PT p2 = ev.point_api.point(sx,sy+2,0.0);
    GameApi::PT p3 = ev.point_api.point(ex+2,ey,0.0);
    GameApi::PT p4 = ev.point_api.point(ex,ey+2,0.0);
    GameApi:: P poly =  ev.polygon_api.quad(p1,p2,p3,p4);
    //GameApi::P poly = ev.polygon_api.quad_z(sx,ex,sy,ey,0.0);
    return ev.polygon_api.color(poly, 0xffffffff);
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    using std::placeholders::_5;
    using std::placeholders::_6;
    using std::placeholders::_7;
    using std::placeholders::_8;
    using std::placeholders::_9;
    if (firsttime)
      {
	line = ev.lines_api.function([this](int a,bool b){ return line_func(a,b); }, 1);
	line = ev.lines_api.change_color(line, 0xffffffff);
	line_2 = ev.polygon_api.from_lines(line, [this](int a,float b,float c,float d,float e,float f,float g,unsigned int h,unsigned int i) { return line_to_poly(a,b,c,d,e,f,g,h,i); });
	line_3 = ev.polygon_api.create_vertex_array(line_2, true);
	//line_p = ev.lines_api.prepare(line);
	firsttime = false;
      }
    else
      {

	if (old_t1_pos.x != t1->get_pos().x ||old_t2_pos.x != t2->get_pos().x
	    ||old_t1_pos.y != t1->get_pos().y || old_t2_pos.y != t2->get_pos().y)
	  {
	    line = ev.lines_api.function([this](int a,bool b){ return line_func(a,b); }, 1);
	    line = ev.lines_api.change_color(line, 0xffffffff);
	    line_2 = ev.polygon_api.from_lines(line, [this](int a,float b,float c,float d,float e,float f,float g,unsigned int h,unsigned int i) { return line_to_poly(a,b,c,d,e,f,g,h,i); });
	    
	     
	    ev.polygon_api.update_vertex_array_no_memory(line_3, line_2);
	  }
	//ev.lines_api.update(line_p, line);
      }
    old_t1_pos = t1->get_pos();
    old_t2_pos = t2->get_pos();


    Point2d p = t1->get_pos();
    p.x += delta_x;
    p.y += delta_y;
    p+=delta_vec;
    Point2d p2 = t2->get_pos();
    p2.x += delta2_x;
    p2.y += delta2_y;
    p2+=delta_vec;
    Point2d pp = { std::min(p.x,p2.x), std::min(p.y,p2.y) };
    Point2d pp2 = { std::max(p.x,p2.x), std::max(p.y,p2.y) };
    Vector2d sz = pp2-pp;
    pos = pp;
    size = sz;
    

  }
  void render()
  {
    if (!firsttime)
      {
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.identity());
	//ev.lines_api.render(line_p);
	ev.polygon_api.render_vertex_array(line_3);
	ev.shader_api.use(sh2);
      }
  }
private:
  GameApi::SH sh;
  bool firsttime;
  GuiWidget *t1;
  int delta_x, delta_y;
  GuiWidget *t2;
  int delta2_x, delta2_y;
  GameApi::LI line;
  GameApi::P line_2;
  GameApi::VA line_3;
  //GameApi::LLA line_p;
  Point2d old_t1_pos;
  Point2d old_t2_pos;
  GameApi::SH sh2;
  Vector2d delta_vec;
};
EXPORT GameApi::W GameApi::GuiApi::timed_visibility(W orig, W timed_widget, W insert, float start_duration, float duration, float dx)
{
  GuiWidget *o = find_widget(e,orig);
  GuiWidget *t = find_widget(e,timed_widget);
  GuiWidget *i = find_widget(e, insert);
#ifdef EMSCRIPTEN
  GuiWidgetForward *ii = static_cast<GuiWidgetForward*>(i);
#else
  GuiWidgetForward *ii = dynamic_cast<GuiWidgetForward*>(i);
#endif
  return add_widget(e, new TimedWidget(ev, o, t, ii, start_duration, duration, dx));
}
EXPORT GameApi::W GameApi::GuiApi::empty()
{
  return add_widget(e, new EmptyWidget(ev));
}
EXPORT GameApi::W GameApi::GuiApi::tooltip(W orig, W insert, std::string label, FtA atlas, BM atlas_bm, int x_gap, float dx)
{
  W w = text(label, atlas, atlas_bm, x_gap);
  W w1 = margin(w, 5,5,5,5);
  W w2 = button(size_x(w1),size_y(w1), c_tooltip_button, c_tooltip_button2 /*0xff888888, c_dialog_1_2*/);
  W w3 = layer(w2,w1);
  float p_x = pos_x(orig) + size_x(orig) + 10.0;
  float p_y = pos_y(orig) + 10.0;
  set_pos(w3, p_x, p_y);
  W w4 = timed_visibility(orig, w3, insert, 0.3, 400.0, dx);
  return w4;
}
EXPORT GameApi::W GameApi::GuiApi::line(W target1, int delta_x, int delta_y,
					W target2, int delta2_x, int delta2_y, SH sh, SH old_sh)
{
  GuiWidget *t1 = find_widget(e, target1);
  GuiWidget *t2 = find_widget(e, target2);
  return add_widget(e, new LineWidget(ev, sh, t1, delta_x, delta_y,
				      t2, delta2_x, delta2_y, old_sh));
}
EXPORT GameApi::W GameApi::GuiApi::canvas(int sx, int sy)
{
  return add_widget(e, new CanvasWidget(ev, sx, sy));
}
EXPORT GameApi::W GameApi::GuiApi::find_canvas_item(W canvas, std::string id)
{
  GuiWidget *w = find_widget(e, canvas);
#ifndef EMSCRIPTEN
  CanvasWidget *ww = dynamic_cast<CanvasWidget*>(w);
#else
  CanvasWidget *ww = static_cast<CanvasWidget*>(w);
#endif
  GuiWidget *item = ww->find_widget(id);
  if (!item)
    {
      std::cout << "ERROR: find_canvas_item failed to find widget from canvas!" << std::endl;
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int s = env->widgets.size();
  for(int i=0;i<s;i++)
    {
      GuiWidget *w = env->widgets[i];
      if (w==item) { GameApi::W w; w.id = i; return w; }
    }
  std::cout << "ERROR: find_canvas_item failed to find widget!" << std::endl;
  GameApi::W wx;
  wx.id = 0;
  return wx;

}
EXPORT int GameApi::GuiApi::canvas_item(W canvas, W item, int x, int y)
{
  GuiWidget *canvas_1 = find_widget(e, canvas);
  CanvasWidget *canvas_2 = dynamic_cast<CanvasWidget*>(canvas_1);
  if (!canvas_2) return -1;
  GuiWidget *item_1 = find_widget(e, item);
  canvas_2->push_back(item_1, x, y);
  return canvas_2->count()-1;
}
EXPORT void GameApi::GuiApi::del_canvas_item(W canvas, int id)
{
  GuiWidget *canvas_1 = find_widget(e, canvas);
  CanvasWidget *canvas_2 = dynamic_cast<CanvasWidget*>(canvas_1);
  if (!canvas_2) return;
  canvas_2->del(id);
}
EXPORT int GameApi::GuiApi::canvas_item_index(W canvas, W item)
{
  GuiWidget *canvas_1 = find_widget(e, canvas);
  CanvasWidget *canvas_2 = dynamic_cast<CanvasWidget*>(canvas_1);
  if (!canvas_2) return -1;
  GuiWidget *item_1 = find_widget(e, item);
  return canvas_2->find_index(item_1);
}
EXPORT GameApi::W GameApi::GuiApi::list_item_title(int sx, std::string label, FtA atlas, BM atlas_bm)
{
  W node_t = text(label, atlas, atlas_bm);
  W node_t0 = margin(node_t, 5,5,5,5);
  std::cout << "List Item title height: " << size_y(node_t0) << std::endl;
  W node_0 = button(sx, size_y(node_t0), c_list_item_title, c_list_item_title2 /*0xff884422, 0xff442211*/);
  W node_1 = layer(node_0, node_t0);
  W node_2 = highlight(node_1);
  return node_2;
 }

EXPORT GameApi::W GameApi::GuiApi::list_item_opened(int sx, std::string label, FtA atlas, BM atlas_bm, std::vector<std::string> subitems, std::vector<std::string> subitems_tooltip, FtA atlas2, BM atlas_bm2, W insert)
{
  W title = list_item_title(sx, label, atlas, atlas_bm);
  std::vector<W> vec;
  vec.push_back(title);
  int s = subitems.size();
  for(int i=0;i<s;i++)
    {
      std::string label = subitems[i];
      std::string toolt = subitems_tooltip[i];
      W txt_0 = text(label, atlas2, atlas_bm2);
      W txt_1 = margin(txt_0, 5, 2, sx-5-size_x(txt_0), 2);
      W txt_2 = highlight(size_x(txt_1), size_y(txt_1));
      W txt_21 = tooltip(txt_2, insert, toolt, atlas2, atlas_bm2, 2, 40.0);
      W txt_3 = layer(txt_1, txt_21);
      vec.push_back(txt_3);
    }
  W array = array_y(&vec[0], vec.size(),2);
  W array_2 = margin(array, 1,1,1,1);
  return array_2;
}


class MouseMoveWidget : public GuiWidgetForward
{
public:
  MouseMoveWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height) { move_ongoing = false; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);
    if (!move_ongoing && button==0 && type==1025 && mouse.x >= pos.x+area_x && mouse.x < pos.x+area_x+area_width &&
	mouse.y >= pos.y+area_y && mouse.y < pos.y+area_y+area_height)
      {
	move_ongoing = true;
	old_mouse = mouse;
	old_pos = pos;
      }
    if (move_ongoing) {
      Vector2d delta = mouse - old_mouse;
      Point2d p = old_pos + delta;
      set_pos(p);
    }
    if (move_ongoing && button==-1)
      {
	move_ongoing = false;
      }
    size = vec[0]->get_size();
  }
  int chosen_item() const { return vec[0]->chosen_item(); }
private:
  int area_x, area_y;
  int area_width, area_height;
  bool move_ongoing;
  Point2d old_pos;
  Point2d old_mouse;
};

class ClickAreaWidget : public GuiWidgetForward
{
public:
  ClickAreaWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height, int button_id2) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height) { done=false;
    selected = false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
    button_id = button_id2;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);

    if (done) {selected=false; }
    if (button==-1) {done = false; }

    //std::cout << "Update!" << std::endl;
    //std::cout << button << " " << mouse << " " << pos << " " << area_x << " " << area_y << " " << area_width << " " << area_height << std::endl;
    if (!done && button==button_id && type==1025 && mouse.x >= pos.x+area_x && mouse.x < pos.x+area_x+area_width &&
	mouse.y >= pos.y+area_y && mouse.y < pos.y+area_y+area_height)
      {
	//std::cout << "Selected: true" << std::endl;
	selected = true;
	done = true;
      }
    size = vec[0]->get_size();
  }
  int chosen_item() const { if (selected) return 0; else return -1; }
private:
  int area_x, area_y;
  int area_width, area_height;
  bool selected;
  bool done;
  int button_id;
};

class KeyAreaWidget : public GuiWidgetForward
{
public:
  KeyAreaWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height, int key) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height),key(key) { done=false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
    selected=false;
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);

    if (done) {selected=false; }
    if (ch==-1) {done = false; }

    //std::cout << "Update!" << std::endl;
    //std::cout << button << " " << mouse << " " << pos << " " << area_x << " " << area_y << " " << area_width << " " << area_height << std::endl;
    if (!done && ch==key && type==768 && mouse.x >= pos.x+area_x && mouse.x < pos.x+area_x+area_width &&
	mouse.y >= pos.y+area_y && mouse.y < pos.y+area_y+area_height)
      {
	//std::cout << "Selected: true" << std::endl;
	selected = true;
	done = true;
      }
    size = vec[0]->get_size();
  }
  int chosen_item() const { if (selected) return 0; else return -1; }
private:
  int area_x, area_y;
  int area_width, area_height;
  bool selected;
  bool done;
  int key;
};


EXPORT GameApi::W GameApi::GuiApi::click_area(W widget, int area_x, int area_y, int area_width, int area_height, int button_id)
{
  GuiWidget *wid = find_widget(e, widget);
  return add_widget(e, new ClickAreaWidget(ev, wid, area_x, area_y, area_width, area_height,button_id));

}
EXPORT GameApi::W GameApi::GuiApi::key_area(W widget, int area_x, int area_y, int area_width, int area_height, int key)
{
  GuiWidget *wid = find_widget(e, widget);
  return add_widget(e, new KeyAreaWidget(ev, wid, area_x, area_y, area_width, area_height, key));

}

EXPORT GameApi::W GameApi::GuiApi::mouse_move(W widget, int area_x, int area_y, int area_width, int area_height)
{
  GuiWidget *wid = find_widget(e, widget);
  return add_widget(e, new MouseMoveWidget(ev, wid, area_x, area_y, area_width, area_height));
}

EXPORT GameApi::W GameApi::GuiApi::canvas_item_gameapi_node(int sx, int sy, std::string label, std::vector<std::string> param_types, std::vector<std::string> param_tooltip, std::string return_type, FtA atlas, BM atlas_bm, W &connect_click, std::string uid, std::vector<W> &params, std::string symbol, std::string comment)
{
  std::vector<W> vec;
  int s = param_types.size();
  int max_width = 0;
  for(int i=0;i<s;i++)
    {
      std::string p = param_types[i];
      std::string tt = param_tooltip[i];
      W txt_0 = text(p, atlas, atlas_bm);
      W txt_1 = margin(txt_0, 5,5,5,5);
      W txt_11 = tooltip(txt_1, txt_1, tt, atlas, atlas_bm, 2, 0.0);
      int width = size_x(txt_1);
      if (max_width<width) max_width = width;
      W txt_2 = button(size_x(txt_1), size_y(txt_1), c_canvas_item,c_canvas_item2 /*0xff888888, 0xff666666*/);
      W txt_3 = layer(txt_2, txt_11);
      W txt_4 = click_area(txt_3, 0.0,0.0,size_x(txt_3), size_y(txt_3),0);
      
      std::stringstream ii;
      ii << i;
      std::string param_uid = uid + " " + ii.str();

      set_id(txt_4, param_uid);
      params.push_back(txt_4);
      vec.push_back(txt_4);
    }

  W node_2 = text(label, atlas,atlas_bm);
  W node_2a = text(symbol, atlas,atlas_bm);
  //float wd = size_x(node_2) + size_x(node_2a) + 5;
  //if (max_width<wd) max_width = wd;

  //W insert = text(comment,atlas,atlas_bm);
  W node_2b = tooltip(node_2a, node_2a, comment, atlas, atlas_bm,2,0.0);
  W node_2bb = layer(node_2a, node_2b);
  std::vector<W> arr = { node_2, node_2bb };
  W node_2c = array_x(&arr[0],arr.size(),5);
  W node_22 = margin(node_2c, 5,5,5,5);

  W array = array_y(&vec[0], vec.size(), 5);
  int ssy = std::max(sy, size_y(array)+size_y(node_22)+5);
  int ssy0 = std::max(sy-size_y(node_22), size_y(array));
  int ssy2 = ssy0 - size_y(array);
  W array_1 = margin(array, 0, size_y(node_22) + ssy2/2+2, 0, ssy2/2-size_y(node_22)-2);
  
  W txt_0 = text(return_type, atlas,atlas_bm);
  W txt_1 = margin(txt_0, 5,5,5,5);

  int max_width2 = size_x(txt_1);
  W txt_11 = highlight(txt_1);
  W txt_111 = click_area(txt_11, 0.0, 0.0, size_x(txt_11), size_y(txt_11),0);
  set_id(txt_111, uid);
  connect_click = txt_111;
  W txt_2 = button(size_x(txt_1), size_y(txt_1), c_canvas_item_text_button, c_canvas_item_text_button2 /*0xff330033, 0xff880088*/);
  W txt_3 = layer(txt_2, txt_111);

  int ssx_0 = std::max(sx, max_width+4+20+4+max_width2);
  int ssx = std::max(ssx_0,size_x(node_22));
  W node_0 = button(ssx,ssy, c_canvas_item_node_0, c_canvas_item_node_0_2 /*0xffff8844, 0xff884422*/);
  W node_1 = button(ssx,size_y(node_22), c_canvas_item_node_1, c_canvas_item_node_1_2 /*0xff884422, 0xff442211*/);
  W node_12 = highlight(node_1);

  int sy0 = std::max(sy, size_y(array)+size_y(node_22)+5);
  W txt_4 = margin(txt_3, ssx-size_x(txt_3), size_y(node_22)+((sy0-size_y(node_22))-size_y(txt_3))/2, 0,((sy0-size_y(node_22))-size_y(txt_3))/2-size_y(node_22));


  W l_0 = layer(node_0, node_12);
  W l_1 = layer(l_0, node_22);
  W l_2 = layer(l_1, array_1);
  W l_3 = layer(l_2, txt_4);
  return l_3;
}

EXPORT GameApi::W GameApi::GuiApi::text(std::string label, FtA atlas, BM atlas_bm, int x_gap)
{
  return add_widget(e, new TextGuiWidgetAtlas(ev, label, atlas, atlas_bm, sh, x_gap));
}

EXPORT GameApi::W GameApi::GuiApi::icon(BM bm)
{
  return add_widget(e, new IconGuiWidget(ev, bm,sh));
}
EXPORT GameApi::W GameApi::GuiApi::poly(P p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new PolyGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT GameApi::W GameApi::GuiApi::va(VA p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new VAGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT GameApi::W GameApi::GuiApi::ml(ML p, SH sh2, SH sh3, SH sh_arr, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new MLGuiWidget(e, ev, p, sh2, sh3, sh_arr, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT GameApi::W GameApi::GuiApi::shader_plane(SFO p, int sx, int sy, int screen_x, int screen_y)
{
  return add_widget(e, new ShaderPlaneGuiWidget(ev, p, sh, sx,sy,screen_x, screen_y));
}

EXPORT GameApi::W GameApi::GuiApi::lines(LI p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new LinesGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT GameApi::W GameApi::GuiApi::pts(PTS p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new PTSGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}

EXPORT GameApi::W GameApi::GuiApi::string_editor(std::string allowed_chars, std::string &target, FtA atlas, BM atlas_bm, int x_gap)
{
  W e1 = add_widget(e, new EditorGuiWidgetAtlas<std::string>(ev, allowed_chars, target, atlas, atlas_bm, sh, x_gap));
  W e2 = highlight(e1);
  return e2;
}

EXPORT GameApi::W GameApi::GuiApi::float_editor(float &target, FtA atlas, BM atlas_bm, int x_gap)
{
  std::string allowed_chars = "0123456789.-";
  W w = add_widget(e, new EditorGuiWidgetAtlas<float>(ev,allowed_chars, target, atlas, atlas_bm, sh, x_gap));
  W w2 = highlight(w);
  return w2;
}

EXPORT GameApi::W GameApi::GuiApi::int_editor(int &target, FtA atlas, BM atlas_bm, int x_gap)
{
  std::string allowed_chars = "0123456789-";
  W w = add_widget(e, new EditorGuiWidgetAtlas<int>(ev, allowed_chars, target, atlas, atlas_bm, sh, x_gap));
  W w2 = highlight(w);
  return w2;
}

EXPORT GameApi::W GameApi::GuiApi::long_editor(long &target, FtA atlas, BM atlas_bm, int x_gap)
{
  std::string allowed_chars = "0123456789-";
  W w = add_widget(e, new EditorGuiWidgetAtlas<long>(ev, allowed_chars, target, atlas, atlas_bm, sh, x_gap));
  W w2 = highlight(w);
  return w2;
}


EXPORT GameApi::W GameApi::GuiApi::color_editor(std::string &col, FtA atlas, BM atlas_bm, int x_gap)
{
  std::string allowed_chars = "0123456789abcdef";
  W edit = string_editor(allowed_chars, col, atlas, atlas_bm, x_gap);
  W edit2 = highlight(edit);
  return edit2; 
}

EXPORT GameApi::W GameApi::GuiApi::polygon_dialog(P p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  W bm_1 = poly(p, sh, 800,600, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::va_dialog(VA p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  W bm_1 = va(p, sh, 800,600, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::ml_dialog(ML p, SH sh, SH sh2, SH sh_arr, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  W bm_1 = ml(p, sh, sh2, sh_arr, 800,600, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}




EXPORT GameApi::W GameApi::GuiApi::shader_dialog(SFO p, W &close_button, FtA atlas, BM atlas_bm, int screen_x, int screen_y, W &codegen_button)
{
  W bm_1 = shader_plane(p, 800,600*600/800, screen_x, screen_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}


EXPORT GameApi::W GameApi::GuiApi::lines_dialog(LI p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  W bm_1 = lines(p, sh, 800,600, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::pts_dialog(PTS p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  W bm_1 = pts(p, sh, 800,600, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
   
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;


  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}



EXPORT GameApi::W GameApi::GuiApi::bitmap_dialog(BM bm, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button)
{
  ev.bitmap_api.prepare(bm);
  float sx = float(ev.bitmap_api.size_x(bm));
  float sy = float(ev.bitmap_api.size_y(bm));
  if (sx>sy) {
    float change = 400.0/sx;
    sx *= change;
    sy *= change;
  } else {
    float change = 400.0/sy;
    sx *= change;
    sy *= change;
  }

  CBM cbm = ev.cont_bitmap_api.from_bitmap(bm, 1.0,1.0);
  BM  cbm_2 = ev.cont_bitmap_api.sample(cbm, int(sx),int(sy));

  W bm_1 = icon(cbm_2);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4));
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;

  W arr[] = { bm_4, code_6, but_6 };
  W arr_2 = array_y(&arr[0], 3, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::edit_dialog(const std::vector<std::string> &labels, const std::vector<GameApi::GuiApi::EditTypes*> &vec, FtA atlas, BM atlas_bm, const std::vector<std::string> &types, W &cancel_but, W &ok_but)
{
  assert(vec.size()==labels.size());
  assert(labels.size()==types.size());
  std::vector<W> vec2;
  int s = vec.size();
  //std::cout << "edit_dialog: " << s << std::endl;
  int size_x1 = 0;
  for(int j=0;j<s;j++)
    {
      std::string label = labels[j];
      W lab = text(label, atlas,atlas_bm, 8);
      int s = size_x(lab);
      if (size_x1 < s) size_x1 = s;
    }
  for(int i=0;i<s;i++)
    {
      EditTypes *target = vec[i];
      //std::cout << "edit_dialog" << i << " " << target << " " << target->i_value << std::endl;
      std::string type = types[i];
      std::string label = labels[i];
      W edit = generic_editor(*target, atlas, atlas_bm, type, 8);
      W lab = text(label, atlas,atlas_bm, 8);
      W lab_2 = right_align(lab, size_x1);
      W array2[] = { lab_2, edit };
      W array3 = array_x(&array2[0], 2, 5);
      vec2.push_back(array3);
    }
  if (vec2.size()==0)
    {
      W lab0 = text("", atlas, atlas_bm, 8);
      W lab = text("(No data)", atlas, atlas_bm, 8);
      W lab_2 = center_align(lab, 500);
      vec2.push_back(lab0);
      vec2.push_back(lab_2);
      vec2.push_back(lab0);
    }
  W array = array_y(&vec2[0], vec2.size(), 35);
  W array_1 = margin(array, 10,10,10,10);
  W array_1a = center_align(array_1, 500);
  W array_2 = button(500, size_y(array_1), c_dialog_1, c_dialog_1_2 /*0xff884422, 0xff442211*/);
  W array_3 = layer(array_2, array_1a);

  W cancel_button = button(250,50, c_dialog_button_1, c_dialog_button_1 /*0xff884422, 0xff442211*/);
  W cancel_button_1 = text("Cancel", atlas,atlas_bm, 4);
  W cancel_button_11 = center_align(cancel_button_1, 250);
  W cancel_button_111 = center_y(cancel_button_11, 50);
  W cancel_button_2 = layer(cancel_button, cancel_button_111);
  W cancel_button_3 = highlight(size_x(cancel_button_2), size_y(cancel_button_2));
  W cancel_button_4 = layer(cancel_button_2, cancel_button_3);
  W cancel_area = click_area(cancel_button_4, 0,0,250,50,0);
  cancel_but = cancel_area;
  W ok_button = button(250,50, c_dialog_button_1, c_dialog_button_1/*0xff884422, 0xff442211*/);
  W ok_button_1 = text("Ok", atlas,atlas_bm, 4);
  W ok_button_11 = center_align(ok_button_1, 250);
  W ok_button_111 = center_y(ok_button_11, 50);
  W ok_button_2 = layer(ok_button, ok_button_111);
  W ok_button_3 = highlight(size_x(ok_button_2), size_y(ok_button_2));
  W ok_button_4 = layer(ok_button_2, ok_button_3);
  W ok_area = click_area(ok_button_4, 0,0,250,50,0);
  ok_but = ok_area;
  W button_array[] = { cancel_area, ok_area };
  W button_arr = array_x(&button_array[0], 2, 0);
  W combine_array[] = { array_3, button_arr };
  W combine_arr = array_y(&combine_array[0], 2, 0);
  W combine_move = mouse_move(combine_arr, 0,0,size_x(combine_arr), size_y(combine_arr));
  return combine_move;
}

class RightAlignWidget : public GuiWidgetForward
{
public:
  RightAlignWidget(GameApi::EveryApi &ev, GuiWidget *wid, int sx) : GuiWidgetForward(ev, { wid }), sx(sx) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x + sx - size.dx, pos.y };
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type);
    set_pos(get_pos());
    size.dx = sx;
    size.dy = vec[0]->get_size().dy;
  }
private:
  int sx;
};
class CenterAlignWidget : public GuiWidgetForward
{
public:
  CenterAlignWidget(GameApi::EveryApi &ev, GuiWidget *wid, int sx) : GuiWidgetForward(ev, { wid }), sx(sx) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x + (sx - size.dx)/2, pos.y };
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type);
    set_pos(get_pos());
    size.dx = sx;
    size.dy = vec[0]->get_size().dy;
  }
private:
  int sx;
};

class CenterYWidget : public GuiWidgetForward
{
public:
  CenterYWidget(GameApi::EveryApi &ev, GuiWidget *wid, int sy) : GuiWidgetForward(ev, { wid }), sy(sy) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x, pos.y + (sy-size.dy)/2};
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type);
    set_pos(get_pos());
    size.dx = vec[0]->get_size().dx;
    size.dy = sy; 
  }
private:
  int sy;
};


EXPORT GameApi::W GameApi::GuiApi::right_align(W item, int sx)
{
  GuiWidget *wid = find_widget(e, item);
  return add_widget(e, new RightAlignWidget(ev, wid, sx));
}
EXPORT GameApi::W GameApi::GuiApi::center_align(W item, int sx)
{
  GuiWidget *wid = find_widget(e, item);
  return add_widget(e, new CenterAlignWidget(ev, wid, sx));
}
EXPORT GameApi::W GameApi::GuiApi::center_y(W item, int sy)
{
  GuiWidget *wid = find_widget(e, item);
  return add_widget(e, new CenterYWidget(ev, wid, sy));
}
EXPORT void GameApi::GuiApi::string_to_generic(EditTypes &target, std::string type, const std::string &source)
{

  //std::cout << "Source: " << source << std::endl;

  //std::cout << "string_to_generic" << type << std::endl;
  if (type=="EveryApi&")
    {
    } else
  if (type=="long")
    {
      std::stringstream ss(source);
      if (ss >> target.l_value) {
	//std::cout << "Dest: " << target.i_value << std::endl;
      } else { std::cout << "StringStream failed" << std::endl; }

    } else
  if (type=="int")
    {
      std::stringstream ss(source);
      if (ss >> target.i_value) {
	//std::cout << "Dest: " << target.i_value << std::endl;
      } else { std::cout << "StringStream failed" << std::endl; }

    } else
  if (type=="unsigned int")
    {
      target.color = source;
    } else
  if (type=="std::string")
    {
      target.s = source;
    } else
  if (type=="float")
    {
      std::stringstream ss(source);
      ss >> target.f_value;
    } else
    if (type=="bool")
      {
	target.s = source;
      } else
  if (type=="PT")
    {
      std::stringstream ss(source);
      char c;
      ss >> c;
      ss >> target.f_x;
      ss >> c;
      ss >> target.f_y;
      ss >> c;
      ss >> target.f_z;
      ss >> c;
    } else
    {
      std::cout << "Unknown type at string_to_generic" << std::endl;
    }
}
EXPORT void GameApi::GuiApi::generic_to_string(const EditTypes &source, std::string type, std::string &target)
{
  //std::cout << "generic_to_string" << type << std::endl;
  if (type=="EveryApi&")
    {
      target="ev";
    } else
  if (type=="long")
    {
      long val = source.l_value;
      //std::cout << "Source2: " << val << std::endl;
      std::stringstream ss;
      ss << val;
      target = ss.str();

    } else
  if (type=="int")
    {
      int val = source.i_value;
      //std::cout << "Source2: " << val << std::endl;
      std::stringstream ss;
      ss << val;
      target = ss.str();
      //std::cout << "Dest2: " << target << std::endl;
    } else
  if (type=="unsigned int")
    {
      std::string s = source.color;
      target = s;
    } else
  if (type=="std::string")
    {
      std::string s = source.s;
      target = s;
    } else
    if (type=="bool")
      {
	std::string s = source.s;
	target = s;
      } else
  if (type=="float")
    {
      float f = source.f_value;
      std::stringstream ss;
      ss << f;
      target = ss.str();
    } else
  if (type=="PT")
    {
      float x = source.f_x;
      float y = source.f_y;
      float z = source.f_z;
      std::stringstream ss;
      ss << "(" << x << "," << y << "," << z << ")";
      target = ss.str();
    } else
    {
      target = "";
      std::cout << "Unknown type at generic_to_string: " << type << std::endl;
    }
}

EXPORT GameApi::W GameApi::GuiApi::generic_editor(EditTypes &target, FtA atlas, BM atlas_bm, std::string type, int x_gap)
{
  //std::cout << "Generic editor: " << type << std::endl;
  if (type=="EveryApi&")
    {
      W edit = empty();
      return edit;
    }
  if (type=="long")
    {
      W edit = long_editor(target.l_value, atlas, atlas_bm, x_gap);
      return edit;
    }
  if (type=="int")
    {
      //std::cout << "Generic editor << " << target.i_value << std::endl;
      W edit = int_editor(target.i_value, atlas, atlas_bm, x_gap);
      return edit;
    }
  if (type=="unsigned int")
    {
      W edit = color_editor(target.color, atlas, atlas_bm, x_gap);
      return edit;
    }
  if (type=="std::string" || type=="bool")
    {
      std::string allowed = "0123456789abcdefghijklmnopqrstuvwxyz/.ABCDEFGHIJKLMNOPQRSTUVWXYZ*()-#+/*\n";
      W edit = string_editor(allowed, target.s, atlas, atlas_bm, x_gap);
      return edit;
    }
  if (type=="float")
    {
      W edit = float_editor(target.f_value, atlas, atlas_bm, x_gap);
      return edit;
    }
  if (type=="PT")
    {
      W edit = point_editor(target.f_x, target.f_y, target.f_z, atlas, atlas_bm, x_gap);
      return edit;
    }
  std::cout << "TYPE ERROR: " << type << std::endl;
  W dummy = button(30,30, 0xffffffff, c_dialog_1);
  return dummy;
}


EXPORT GameApi::W GameApi::GuiApi::point_editor(float &x, float &y, float &z, FtA atlas, BM atlas_bm, int x_gap)
{
  W l0 = text("(",atlas, atlas_bm);
  W x_edit = float_editor(x, atlas, atlas_bm, x_gap);
  W x_edit_2 = highlight(x_edit);
  W l1 = text(";",atlas, atlas_bm);
  W y_edit = float_editor(y, atlas, atlas_bm, x_gap);
  W y_edit_2 = highlight(y_edit);
  W l2 = text(";",atlas,atlas_bm);
  W z_edit = float_editor(z, atlas,atlas_bm, x_gap);
  W z_edit_2 = highlight(z_edit);
  W l3 = text(")",atlas,atlas_bm);
  W array[] = { l0, x_edit_2, l1, y_edit_2, l2, z_edit_2, l3 };
  return array_x(&array[0], 7, 2);
}

EXPORT GameApi::W GameApi::GuiApi::highlight(int sx, int sy)
{
  return add_widget(e, new HighlightGuiWidget(ev, sh, sx,sy));
}
EXPORT GameApi::W GameApi::GuiApi::highlight(W wid)
{
  GuiWidget *w = find_widget(e, wid);
  return add_widget(e, new Highlight2GuiWidget(ev, w, sh));
}
EXPORT GameApi::W GameApi::GuiApi::margin(W w, int l, int t, int r, int b)
{
  GuiWidget *ww = find_widget(e, w);
  return add_widget(e, new MarginGuiWidget(ev, ww, l, t, r, b));
}
EXPORT GameApi::W GameApi::GuiApi::layer(W w1, W w2)
{
  GuiWidget *ww1 = find_widget(e, w1);
  GuiWidget *ww2 = find_widget(e, w2);
  return add_widget(e, new LayerGuiWidget(ev, ww1, ww2));
}
EXPORT GameApi::W GameApi::GuiApi::array_x(W *arr, int size, int x_gap)
{
  int s = size;
  std::vector<GuiWidget*> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_widget(e, arr[i]));
    }
  return add_widget(e, new ArrayXWidget(ev, vec, x_gap));
}
EXPORT GameApi::W GameApi::GuiApi::array_y(W *arr, int size, int y_gap)
{
  int s = size;
  std::vector<GuiWidget*> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(find_widget(e, arr[i]));
    }
  return add_widget(e, new ArrayYWidget(ev, vec, y_gap));
}

GameApi::W GameApi::GuiApi::main_menu(std::vector<std::string> labels, FtA atlas, BM atlas_bm)
{
  int s = labels.size();
  std::vector<W> vec;
  for(int i=0;i<s;i++)
    {
      std::string s = labels[i];
      W txt_1 = text(s, atlas, atlas_bm);
      W txt_2 = margin(txt_1, 5,5,5,5);
      W txt_3 = highlight(size_x(txt_2), size_y(txt_2));
      W txt_4 = layer(txt_2, txt_3);
      vec.push_back(txt_4);
    }
  W w = array_x(&vec[0], vec.size(), 20);
  //PT pt1 = ev.point_api.point(0.0,0.0,0.0);
  //PT pt2 = ev.point_api.point(0.0, size_y(w), 0.0);
  //W w3 = gradient(size_x(w), size_y(w), pt1, pt2, 0xffffaa88, 0xffff8844);
  W w3 = button(size_x(w), size_y(w), c_list_item_title, c_list_item_title2 /*0xffff8844, 0xff884422*/);

  W w4 = layer(w3,w);
  W w5 = margin(w4, 2,2,2,2);

  return w5;
}

class ScrollBarY : public GuiWidgetForward
{
public:
  ScrollBarY(GameApi::EveryApi &ev, GameApi::SH sh, int sx, int sy, int area_y) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), sx(sx), sy(sy), area_y(area_y) { firsttime=true; current_pos = 0.0; following = false; old_area_y = area_y; changed=false; }
  void update(Point2d mouse, int button, int ch, int type)
  {
    Point2d p = get_pos();
    float size_y = float(sy)/float(area_y)*float(sy-2-2-2-2);
    float pos_y = current_pos * (float(sy) - float(sy)/float(area_y)*float(sy-2-2-2-2));
    if (button==0 && type == 1025 && !following && mouse.x>=p.x+2+2 && mouse.x<p.x+sx-2-2 && mouse.y>=p.y+pos_y+2+2 && mouse.y<p.y+pos_y+2+2+size_y)
      {
	following = true;
	start_pos = mouse;
      }
    if (button==0 && following)
      { // moving this to render() doesnt help since it needs mouse position
	float delta = mouse.y - start_pos.y;
	current_pos = current_pos + delta/float(sy-2-2-2-2);

	if (current_pos < 0.0) current_pos = 0.0;
	if (current_pos > 1.0) current_pos = 1.0;

	start_pos = mouse;
      }

    if (button==-1)
      {
	following = false;
      }
    if (firsttime || changed)
      {
	if (firsttime)
	  {
	GameApi::BB b = ev.bool_bitmap_api.empty(sx,sy);
	GameApi::BB b1 = ev.bool_bitmap_api.rectangle(b, 0.0, 0.0, float(sx), 2.0);
	GameApi::BB b2 = ev.bool_bitmap_api.rectangle(b1, 0.0, 0.0, 2.0, float(sy));
	GameApi::BB b3 = ev.bool_bitmap_api.rectangle(b2, float(sx)-2.0, 0.0, 2.0, float(sy));
	GameApi::BB b4 = ev.bool_bitmap_api.rectangle(b3, 0.0, float(sy)-2.0, float(sx), 2.0);
	bg = ev.bool_bitmap_api.to_bitmap(b4, 255,255,255,255, 0,0,0,0);

	bg_va = ev.sprite_api.create_vertex_array(bg);
	GameApi::BB k = ev.bool_bitmap_api.empty(sx-2-2-2-2, float(sy)/float(area_y)*float(sy-2-2-2-2));
	GameApi::BB k2 = ev.bool_bitmap_api.rectangle(k, 0.0, 0.0, float(sx-2-2-2-2), float(sy)/float(area_y)*float(sy-2-2-2-2));
	thumb = ev.bool_bitmap_api.to_bitmap(k2, 255,255,255,255, 0,0,0,0);
	thumb_va = ev.sprite_api.create_vertex_array(thumb);
	  }

	firsttime = false;
	changed=false;
	old_area_y = area_y;
      }
  }
  void render()
  {

    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x, p.y, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bg_va);

	float pos_y = current_pos * (float(sy) - float(sy)/float(area_y)*float(sy));
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+2+2+0.5, p.y+pos_y+2+2+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(thumb_va);
      }
  }
  float dynamic_param(int id) const
  {
    if (id==0) { return current_pos; }
    return 0.0;
  }
  void set_dynamic_param(int id, float val)
  {
    if (id==0) { 
      area_y = int(val); 
      if (area_y!=old_area_y)
	{
	  changed=true;
	}
    }
  }
private:
  GameApi::SH sh;
  GameApi::BM bg;
  GameApi::VA bg_va;
  GameApi::BM thumb;
  GameApi::VA thumb_va;
  float current_pos;
  int sx,sy,area_y;
  bool firsttime;
  bool following;
  Point2d start_pos;
  int old_area_y;
  bool changed;
  bool button_pressed;
};

class ScrollBarX : public GuiWidgetForward
{
public:
  ScrollBarX(GameApi::EveryApi &ev, GameApi::SH sh, int sx, int sy, int area_x) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), sx(sx), sy(sy), area_x(area_x) { firsttime=true; current_pos = 0.0; following = false;  changed=false; old_area_x = area_x; }
  void update(Point2d mouse, int button, int ch, int type)
  {
    Point2d p = get_pos();
    float size_x = float(sx)/float(area_x)*float(sx-2-2-2-2);
    float pos_x = current_pos * (float(sx) - float(sx)/float(area_x)*float(sx-2-2-2-2));
    if (button==0 && type==1025 && !following && mouse.y>=p.y+2+2 && mouse.y<p.y+sy-2-2 && mouse.x>=p.x+pos_x+2+2 && mouse.x<p.x+pos_x+2+2+size_x)
      {
	following = true;
	start_pos = mouse;
      }
    if (button==0 && following)
      {
	float delta = mouse.x - start_pos.x;
	current_pos = current_pos + delta/float(sx-2-2-2-2);

	if (current_pos < 0.0) current_pos = 0.0;
	if (current_pos > 1.0) current_pos = 1.0;

	start_pos = mouse;
      }
    if (button==-1)
      {
	following = false;
      }

    if (firsttime ||changed)
      {
	if (firsttime)
	  {
	GameApi::BB b = ev.bool_bitmap_api.empty(sx,sy);
	GameApi::BB b1 = ev.bool_bitmap_api.rectangle(b, 0.0, 0.0, float(sx), 2.0);
	GameApi::BB b2 = ev.bool_bitmap_api.rectangle(b1, 0.0, 0.0, 2.0, float(sy));
	GameApi::BB b3 = ev.bool_bitmap_api.rectangle(b2, float(sx)-2.0, 0.0, 2.0, float(sy));
	GameApi::BB b4 = ev.bool_bitmap_api.rectangle(b3, 0.0, float(sy)-2.0, float(sx), 2.0);
	bg = ev.bool_bitmap_api.to_bitmap(b4, 255,255,255,255, 0,0,0,0);

	bg_va = ev.sprite_api.create_vertex_array(bg);
	GameApi::BB k = ev.bool_bitmap_api.empty(float(sx)/float(area_x)*float(sx-2-2-2-2), sy-2-2-2-2);
	GameApi::BB k2 = ev.bool_bitmap_api.rectangle(k, 0.0, 0.0, float(sx)/float(area_x)*float(sx-2-2-2-2), float(sy-2-2-2-2));
	thumb = ev.bool_bitmap_api.to_bitmap(k2, 255,255,255,255, 0,0,0,0);
	thumb_va = ev.sprite_api.create_vertex_array(thumb);
	  }

	firsttime = false;
	changed=false;
	old_area_x = area_x;
      }
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x, p.y, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bg_va);

	float pos_x = current_pos * (float(sx) - float(sx)/float(area_x)*float(sx));
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+pos_x+2+2+0.5, p.y+2+2+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(thumb_va);
      }
  }
  float dynamic_param(int id) const
  {
    if (id==0) { return current_pos; }
    return 0.0;
  }
  void set_dynamic_param(int id, float val)
  {
    if (id==0) { 
      area_x = int(val); 
      if (old_area_x != area_x)
	{
	  changed=true;
	}
    }
  }
private:
  GameApi::SH sh;
  GameApi::BM bg;
  GameApi::VA bg_va;
  GameApi::BM thumb;
  GameApi::VA thumb_va;
  float current_pos;
  int sx,sy,area_x;
  bool firsttime;
  bool following;
  Point2d start_pos;
  int old_area_x;
  bool changed;
};

float wave_transfer(float val, float start_range, std::function<float(float)> f, float time)
{
  val += start_range;
  float v = f(val+time);
  return v;
}
class WaveformWidget : public GuiWidget
{
public:
  WaveformWidget(GameApi::EveryApi &ev, GameApi::SH sh, std::function<float (float)> f, float start_range, float end_range, float min_value, float max_value, int sx, int sy, unsigned int true_color, unsigned int false_color) : ev(ev), sh(sh), f(f), start_range(start_range), end_range(end_range), min_value(min_value), max_value(max_value), sx(sx), sy(sy), true_color(true_color), false_color(false_color) 
  { 
    firsttime = true; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    time = 0.0;
  }
  Point2d get_pos() const { return pos; }
  Vector2d get_size() const { return size; }
  void set_pos(Point2d pos_p) { pos = pos_p; }
  void set_size(Vector2d vec_p) { size = vec_p; }
  void update(Point2d mouse_pos, int button,int ch, int type)
  {
    if (firsttime)
      {
	GameApi::WV wave = ev.waveform_api.function(std::bind(&wave_transfer, GameApi::_1, start_range, f, std::ref(time)), end_range-start_range, min_value, max_value); 
	wave_bm = ev.waveform_api.waveform_bitmap(wave, sx, sy, true_color, false_color);
	wave_bm_va = ev.sprite_api.create_vertex_array(wave_bm);
	firsttime = false;
      } else {
      time+=0.05;
      ev.sprite_api.update_vertex_array(wave_bm_va, wave_bm);
    }
    Vector2d s = { float(sx), float(sy) };
    size = s;
  }
  void render()
  {
    if (!firsttime)
      {
	Point2d pos = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(pos.x+0.5, pos.y+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(wave_bm_va);
      }
  }
  int render_to_bitmap()
  {
    return wave_bm.id;
  }
  void select_item(int item) { }
  int chosen_item() const { return -1; }
  float dynamic_param(int id) const { return 0.0; }
  void set_dynamic_param(int id, float val) { }
  int child_count() const { return 0; }
  GuiWidget *child(int num) const { return 0; }
  bool content_changed() const { return true; }
  void clear_content_changed() { }
private:
  bool firsttime;
  GameApi::BM wave_bm;
  GameApi::VA wave_bm_va;
  Point2d pos;
  Vector2d size;
  GameApi::EveryApi &ev;
  GameApi::SH sh;
  std::function<float (float)> f;
  float start_range, end_range;
  float min_value, max_value;
  int sx, sy;
  unsigned int true_color, false_color;
  float time;
};
class ScrollAreaWidget : public GuiWidgetForward
{
public:
  ScrollAreaWidget(GameApi::EveryApi &ev, GameApi::SH sh, GuiWidget *orig, int sx, int sy, int screen_y) : GuiWidgetForward(ev, { orig }), sh(sh), orig(orig), sx(sx), sy(sy), screen_y(screen_y) { firsttime = true; Vector2d v = { float(sx),float(sy)}; size = v; top=0.0; left=0.0; 
    
}
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d sz = vec[0]->get_size();
    Point2d pos2 = { pos.x-left*(sz.dx-size.dx), pos.y-top*(sz.dy-size.dy) };
    vec[0]->set_pos(pos2);
  }
  void set_size(Vector2d sz)
  {
    GuiWidgetForward::set_size(sz);
    size = sz;
    sx=(int)sz.dx;
    sy=(int)sz.dy;
  }
  void update(Point2d mouse_pos, int button, int ch, int type)
  {
    Point2d mouse = mouse_pos;
    if (mouse.x>=pos.x-80 && mouse.x<pos.x+size.dx+80 &&
	mouse.y>=pos.y-80 && mouse.y<pos.y+size.dy+80)
      {
	//Vector2d sz = vec[0]->get_size();
	//mouse.x -= left*(sz.dx-size.dx);
	//mouse.y -= top*(sz.dy-size.dy);
	orig->update(mouse, button,ch, type);
      }
    else
      {
	Point2d mouse = {-666.0, -666.0 };
	orig->update(mouse, button,ch, type);
      }
  }
  void render()
  {
    glEnable(GL_SCISSOR_TEST);
    glScissor(pos.x, screen_y-pos.y-size.dy, size.dx, size.dy);
    vec[0]->render();
    glDisable(GL_SCISSOR_TEST);
  }

  int chosen_item() const { return orig->chosen_item(); }
  void set_dynamic_param(int id, float val)
  {
    switch(id) {
    case 0: left = val; break;
    case 1: top = val; break;
    }
    Point2d pos = get_pos();
    Vector2d sz = vec[0]->get_size();
    Point2d pos2 = { pos.x-left*(sz.dx-size.dx), pos.y-top*(sz.dy-size.dy) };
    //std::cout << "dyn: " << left << " " << top << std::endl;
    vec[0]->set_pos(pos2);
  }
private:
  bool firsttime;
  GameApi::SH sh;
  GameApi::BM area_bm;
  GameApi::VA area_bm_va;
  GuiWidget *orig;
  int sx,sy;
  float top, left;
  int screen_y;
};
class OrElemGuiWidget : public GuiWidget
{
public:
  OrElemGuiWidget(GuiWidget *w1, GuiWidget *w2) : b(false), w1(w1), w2(w2) { }
  virtual Point2d get_pos() const { if (b) return w1->get_pos(); else return w2->get_pos(); }
  virtual Vector2d get_size() const { if (b) return w1->get_size(); else return w2->get_size(); }
  virtual void set_pos(Point2d pos) { if (b) w1->set_pos(pos); else w2->set_pos(pos); } 
  virtual void set_size(Vector2d size) { if (b) w1->set_size(size); else w2->set_size(size); }
  virtual void update(Point2d mouse_pos, int button, int ch, int type) { if (b) w1->update(mouse_pos, button,ch,type); else w2->update(mouse_pos, button,ch,type); }
  virtual void render() { if (b) w1->render(); else w2->render(); }
  virtual int render_to_bitmap() { if (b) return w1->render_to_bitmap(); else return w2->render_to_bitmap(); }
  virtual void select_item(int item) { if (item==0) b=true; if (item==1) b=false; }
  virtual int chosen_item() const { if (b) return w1->chosen_item(); else return w2->chosen_item(); }
  virtual float dynamic_param(int id) const { if (b) return w1->dynamic_param(id); else return w2->dynamic_param(id); }
  virtual void set_dynamic_param(int id, float val) { if (b) w1->set_dynamic_param(id, val); else w2->set_dynamic_param(id, val); }
  virtual int child_count() const { if (b) { return w1->child_count(); } else { return w2->child_count(); } }
  virtual bool content_changed() const { if (b) { return w1->content_changed(); } else { return w2->content_changed(); } }
  virtual void clear_content_changed() { if (b) { w1->clear_content_changed(); } else { w2->clear_content_changed(); } }
  virtual GuiWidget *child(int num) const
  {
    if (b) { return w1->child(num); }
    else { return w2->child(num); }
  }

private:
  bool b;
  GuiWidget *w1;
  GuiWidget *w2;
};
GameApi::W GameApi::GuiApi::gradient(int sx, int sy, PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2)
{
  //color_2 = color_1;
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, color_1, color_2, sx, sy);
  return icon(bm);
}
GameApi::W GameApi::GuiApi::button(int sx, int sy, unsigned int color_1, unsigned int color_2)
{
  //color_2 = color_1;

  PT pt1 = ev.point_api.point(0.0, 0.0, 0.0);
  PT pt2 = ev.point_api.point(0.0, sy, 0.0);
  BM w = ev.bitmap_api.gradient(pt1, pt2, color_1, color_2, sx,sy);
  unsigned int color_1L = color_1;
  unsigned int color_2L = color_2;
#if 1
  unsigned char *ptr = (unsigned char *)&color_1L;
  unsigned char *ptr2 = (unsigned char *)&color_2L;
  ptr[0] = (unsigned char) (float(ptr[0])*0.8);
  ptr[1] = (unsigned char) (float(ptr[1])*0.8);
  ptr[2] = (unsigned char) (float(ptr[2])*0.8);
  ptr[3] = (unsigned char) (float(ptr[3])*0.8);
  ptr2[0] = (unsigned char) (float(ptr2[0])*0.8);
  ptr2[1] = (unsigned char) (float(ptr2[1])*0.8);
  ptr2[2] = (unsigned char) (float(ptr2[2])*0.8);
  ptr2[3] = (unsigned char) (float(ptr2[3])*0.8);
#endif
  BM w2 = ev.bitmap_api.gradient(pt1, pt2, color_1L, color_2L,sx,sy);

  unsigned int color_1D = color_1;
  unsigned int color_2D = color_2;
#if 1
  unsigned char *ptrD = (unsigned char*)&color_1D;
  unsigned char *ptr2D = (unsigned char*)&color_2D;
  ptrD[0] = (unsigned char) (float(ptrD[0])*0.6);
  ptrD[1] = (unsigned char) (float(ptrD[1])*0.6);
  ptrD[2] = (unsigned char) (float(ptrD[2])*0.6);
  ptrD[3] = (unsigned char) (float(ptrD[3])*0.6);
  ptr2D[0] = (unsigned char) (float(ptr2D[0])*0.6);
  ptr2D[1] = (unsigned char) (float(ptr2D[1])*0.6);
  ptr2D[2] = (unsigned char) (float(ptr2D[2])*0.6);
  ptr2D[3] = (unsigned char) (float(ptr2D[3])*0.6);
#endif
  BM w3 = ev.bitmap_api.gradient(pt1, pt2, color_1D, color_2D,sx,sy);

  BB mask_0 = ev.bool_bitmap_api.empty(sx,sy);
  BB mask_0a = ev.bool_bitmap_api.rectangle(mask_0, 0.0, 0.0, float(sx), 2.0);
  BB mask_0b = ev.bool_bitmap_api.rectangle(mask_0a, 0.0, 0.0, 2.0, float(sy));
  BB mask_1 = mask_0b;
  
  BB mask__0 = ev.bool_bitmap_api.empty(sx,sy);
  BB mask__0a = ev.bool_bitmap_api.rectangle(mask__0, float(sx-2), 0.0, 2.0, sy);
  BB mask__0b = ev.bool_bitmap_api.rectangle(mask__0a, 0.0, float(sy-2), float(sx), 2.0);
  BB mask_2 = mask__0b;

  BM w4 = ev.bitmap_api.blitbitmap(w2, w, 0, 0, mask_1);
  BM w5 = ev.bitmap_api.blitbitmap(w4, w3, 0, 0, mask_2);
  return icon(w5);
}
GameApi::W GameApi::GuiApi::or_elem(W w1, W w2)
{
  GuiWidget *ww1 = find_widget(e, w1);
  GuiWidget *ww2 = find_widget(e, w2);
  return add_widget(e, new OrElemGuiWidget(ww1,ww2));
}
GameApi::W GameApi::GuiApi::scrollbar_y(int sx, int sy, int area_y)
{
  return add_widget(e, new ScrollBarY(ev,sh, sx,sy,area_y));
}
GameApi::W GameApi::GuiApi::scrollbar_x(int sx, int sy, int area_x)
{
  return add_widget(e, new ScrollBarX(ev,sh, sx,sy,area_x));
}
GameApi::W GameApi::GuiApi::scroll_area(W orig, int sx, int sy, int screen_y)
{
  GuiWidget *orig_1 = find_widget(e, orig);
  return add_widget(e, new ScrollAreaWidget(ev, sh, orig_1, sx,sy, screen_y));
}
#if 0
GameApi::W GameApi::GuiApi::canvas_scroll_area(int sx, int sy, int screen_x, int screen_y)
{
}
#endif
GameApi::W GameApi::GuiApi::waveform(std::function<float (float)> f, float start_range, float end_range, float min_value, float max_value, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  return add_widget(e, new WaveformWidget(ev, sh, f, start_range, end_range, min_value, max_value, sx, sy, true_color, false_color));
}
GameApi::W GameApi::GuiApi::menu(W main_menu, int menu_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm)
{
  GuiWidget *w = find_widget(e, main_menu);
  int s = w->child(0)->child(1)->child_count();
  if (menu_id<0 || menu_id>=s) { std::cout << "ERROR: Wrong menu id" << menu_id << " " << s << std::endl; exit(0); }
  GuiWidget *ww = w->child(0)->child(1)->child(menu_id);
  Point2d pos = ww->get_pos();
  Vector2d size = ww->get_size();
  
  int ss = labels.size();
  std::vector<W> vec;
  for(int i=0;i<ss;i++) {
    std::string s = labels[i];
    W txt_1 = text(s, atlas, atlas_bm);
    W txt_2 = margin(txt_1, 2,2,2,2);
    W txt_3 = highlight(size_x(txt_2), size_y(txt_2));
    W txt_4 = layer(txt_2, txt_3);
    vec.push_back(txt_4);
  }
  W w2 = array_y(&vec[0], vec.size(), 2);
  W w22 = margin(w2, 4,4,4,4);
  //PT pt1 = ev.point_api.point(0.0,0.0,0.0);
  //PT pt2 = ev.point_api.point(0.0, size_y(w2), 0.0);
  W w3 = button(size_x(w22), size_y(w22), c_list_item_title, c_list_item_title2 /*0xffff8844, 0xff884422*/);
    //gradient(size_x(w2), size_y(w2), pt1, pt2, 0xffff8844, 0xff884422);
  W w4 = layer(w3,w22);
  set_pos(w4, pos.x, pos.y+size.dy);
  return w4;
}

GameApi::W GameApi::GuiApi::popup_menu(int x, int y, std::vector<std::string> labels, FtA atlas, BM atlas_bm, std::vector<W> &areas)
{
  int ss = labels.size();
  std::vector<W> vec;
  for(int i=0;i<ss;i++) {
    std::string s = labels[i];
    W txt_1 = text(s, atlas, atlas_bm);
    W txt_2 = margin(txt_1, 2,2,2,2);
    W txt_3 = highlight(size_x(txt_2), size_y(txt_2));
    W txt_4 = layer(txt_2, txt_3);
    W txt_5 = click_area(txt_4, 0.0, 0.0, size_x(txt_4), size_y(txt_4),0);
    std::stringstream ss;
    ss << i;
    std::string id = ss.str();
    set_id(txt_5, id);
    vec.push_back(txt_5);
    areas.push_back(txt_5);
  }

  W w2 = array_y(&vec[0], vec.size(), 2);
  W w22 = margin(w2, 4,4,4,4);
  W w3 = button(size_x(w22), size_y(w22), c_list_item_title, c_list_item_title2 /*0xffff8844, 0xff884422*/);
  W w4 = layer(w3,w22);
  set_pos(w4, x, y);
  return w4;
}

int GuiWidgetForward::render_to_bitmap()
{
  GameApi::BM bg = ev.bitmap_api.newbitmap(size.dx, size.dy, 0x0000000000);
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      GuiWidget *w = vec[i];
      int bm_id = w->render_to_bitmap();
      GameApi::BM bm_id_1;
      bm_id_1.id = bm_id;
      Point2d p = w->get_pos();
      p-=pos;
      GameApi::FB fb = ev.float_bitmap_api.from_alpha(bm_id_1);
      GameApi::BB bb3 = ev.bool_bitmap_api.from_float_bitmap(fb, 0.1, 1.2);
      bg = ev.bitmap_api.blitbitmap(bg, bm_id_1, (int)p.x, (int)p.y, bb3);
    }
  return bg.id;
}
int GameApi::GuiApi::pos_x(W w)
{
  GuiWidget *ww = find_widget(e, w);
  return int(ww->get_pos().x);
}
int GameApi::GuiApi::pos_y(W w)
{
  GuiWidget *ww = find_widget(e, w);
  return int(ww->get_pos().y);
}
 int GameApi::GuiApi::size_x(W w)
 {
  GuiWidget *ww = find_widget(e, w);
  return ww->get_size().dx;
 }
 int GameApi::GuiApi::size_y(W w)
 {
   GuiWidget *ww = find_widget(e, w);
   return ww->get_size().dy;
 }
void GameApi::GuiApi::set_pos(W w, float px, float py)
{
  Point2d p = { px, py };
  GuiWidget *ww = find_widget(e, w);
  ww->set_pos(p);
}
void GameApi::GuiApi::set_size(W w, float sx, float sy)
{
  Vector2d v = { sx, sy };
  GuiWidget *ww = find_widget(e, w);
  ww->set_size(v);
}
void GameApi::GuiApi::update(W w, PT mouse, int button,int ch, int type)
{
  GuiWidget *ww = find_widget(e, w);
  Point *pt = find_point(e, mouse);
  Point2d pt2 = { pt->x, pt->y };
  ww->update(pt2, button,ch, type);
}
void GameApi::GuiApi::render(W w)
{
  //std::cout << "GuiApi::render" << w.id << std::endl;
  GuiWidget *ww = find_widget(e, w);
  ww->render();
}
int GameApi::GuiApi::chosen_item(W w)
{
  GuiWidget *ww = find_widget(e, w);
  return ww->chosen_item();
}
void GameApi::GuiApi::select_item(W w, int item)
{
  GuiWidget *ww = find_widget(e, w);
  ww->select_item(item);
}
float GameApi::GuiApi::dynamic_param(W w, int id)
{
  //std::cout << "GuiApi::dynamic_param" << w.id << std::endl;
  GuiWidget *ww = find_widget(e, w);
  return ww->dynamic_param(id);
}
int GameApi::GuiApi::num_childs(W w)
{
  GuiWidget *ww = find_widget(e, w);
#ifdef EMSCRIPTEN
  GuiWidgetForward *ww2 = static_cast<GuiWidgetForward*>(ww);
#else
  GuiWidgetForward *ww2 = dynamic_cast<GuiWidgetForward*>(ww);
#endif
  return ww2->vec.size();
}
GameApi::W GameApi::GuiApi::get_child(W w, int i)
{
  GuiWidget *ww = find_widget(e, w);
#ifdef EMSCRIPTEN
  GuiWidgetForward *ww2 = static_cast<GuiWidgetForward*>(ww);
#else
  GuiWidgetForward *ww2 = dynamic_cast<GuiWidgetForward*>(ww);
#endif
  if (ww2) 
    {
      GuiWidget *child = ww2->vec[i];
      
      EnvImpl *env = ::EnvImpl::Environment(&e);
      int s = env->widgets.size();
      for(int i=0;i<s;i++)
	{
	  GuiWidget *w = env->widgets[i];
	  if (child==w) {
	    GameApi::W wid;
	    wid.id = i;
	    return wid;
	  }
	}
    }
  GameApi::W wid;
  wid.id = -1;
  return wid;
}
void GameApi::GuiApi::set_dynamic_param(W w, int id, float val)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_dynamic_param(id, val);
}
std::string GameApi::GuiApi::get_id(W w)
{
  GuiWidget *ww = find_widget(e,w);
  return ww->get_id();
}
void GameApi::GuiApi::set_id(W w, std::string id)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_id(id);
}

template<class T>
class FromStreamClass
{
public:
  T from_stream(std::string s, GameApi::EveryApi &ev)
  {
  std::cout << "Type using default: " << typeid(T).name() << std::endl;
  T t;
  std::stringstream is(s);
  is >> t;
  return t;
  }
};

template<>
class FromStreamClass<GameApi::EveryApi &>
{
public:
  GameApi::EveryApi &from_stream(std::string s, GameApi::EveryApi &ev)
  {
    return ev;
  }
};

template<>
class FromStreamClass<bool>
{
public:
  bool from_stream(std::string s, GameApi::EveryApi &ev)
  {
  if (s=="false") return false;
  if (s=="true") return true;
  if (s=="1") return true;
  return false;
  }
};

template<>
class FromStreamClass<unsigned int>
{
public:
  unsigned int from_stream(std::string s, GameApi::EveryApi &ev)
  {
  unsigned int bm;
  //char c;
  //is >> c;
  //is >> c;
  std::stringstream is(s);
  is >> std::hex >> bm >> std::dec;
  return bm;
  }
};

template<class T>
class FromStreamClass<std::vector<T>>
{
public:
  std::vector<T> from_stream(std::string s, GameApi::EveryApi &ev)
  {
    std::cout << "Vector:" << s << std::endl;
    std::vector<T> vec;
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass<T> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    T t = cls.from_stream(substr, ev);
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};

template<class T>
void set_empty(GameApi::EveryApi &ev, T &t) { t.id=0; }

#define MACRO2(lab, funccall) \
  void set_empty(GameApi::EveryApi&ev,lab &l) {	\
    l = funccall;\
  }

#define MACRO3(lab, funccall) \
  if (s==#lab) { return #funccall; }

std::string empty_param(std::string s)
{
  if (s.size()>1 && s[0]=='[')
    {
      return "std::vector<"+s+">()";
    }
  if (s=="float") { return "0.0"; }
  if (s=="int") { return "0"; }
  if (s=="std::string") { return ""; }
MACRO3(BM,ev.bitmap_api.newbitmap(10,10,0x00000000))
MACRO3(FD,ev.dist_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO3(BO,ev.bool_api.cube(ev,0.0,0.0,0.0,0.0,0.0,0.0,1,1))
MACRO3(BB,ev.bool_bitmap_api.empty(10,10))
MACRO3(FB,ev.float_bitmap_api.empty(10,10))
MACRO3(PT,ev.point_api.origo())
MACRO3(V,ev.vector_api.null_vector())
MACRO3(FO,ev.float_volume_api.distance())
MACRO3(SFO,ev.sh_api.cube())
MACRO3(O,ev.volume_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO3(CBM,ev.cont_bitmap_api.empty(10.0,10.0))
MACRO3(P,ev.polygon_api.empty())
MACRO3(SH,ev.shader_api.colour_shader())
MACRO3(CO,ev.color_api.u_color(0xffffffff))
MACRO3(LI,ev.lines_api.from_polygon(ev.polygon_api.empty()))
MACRO3(MN,ev.move_api.empty())
MACRO3(MT,ev.materials_api.def(ev))
MACRO3(C,ev.curve_api.linear(std::vector<GameApi::PT>()))
  return "@";
}


MACRO2(GameApi::BM,ev.bitmap_api.newbitmap(10,10,0x00000000))
MACRO2(GameApi::FD,ev.dist_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::BO,ev.bool_api.cube(ev,0.0,0.0,0.0,0.0,0.0,0.0,1,1))
MACRO2(GameApi::BB,ev.bool_bitmap_api.empty(10,10))
MACRO2(GameApi::FB,ev.float_bitmap_api.empty(10,10))
MACRO2(GameApi::PT,ev.point_api.origo())
MACRO2(GameApi::V,ev.vector_api.null_vector())
MACRO2(GameApi::FO,ev.float_volume_api.distance())
MACRO2(GameApi::SFO,ev.sh_api.cube())
MACRO2(GameApi::O,ev.volume_api.cube(0.0,0.0,0.0,0.0,0.0,0.0))
MACRO2(GameApi::CBM,ev.cont_bitmap_api.empty(10.0,10.0))
MACRO2(GameApi::P,ev.polygon_api.empty())
MACRO2(GameApi::SH,ev.shader_api.colour_shader())
MACRO2(GameApi::CO,ev.color_api.u_color(0xffffffff))
MACRO2(GameApi::LI,ev.lines_api.from_polygon(ev.polygon_api.empty()))
MACRO2(GameApi::MN,ev.move_api.empty())
MACRO2(GameApi::MT,ev.materials_api.def(ev))
MACRO2(GameApi::C,ev.curve_api.linear(std::vector<GameApi::PT>()))


#define MACRO(lab) \
template<> \
class FromStreamClass<lab> \
{ \
public:\
  lab from_stream(std::string s, GameApi::EveryApi &ev)\
  {\
  lab bm;\
  if (s=="@") {\
        set_empty(ev,bm);\
      return bm;\
  }\
  std::stringstream is(s);\
  is >> bm.id;\
  return bm;\
  }\
};
MACRO(GameApi::BM)
MACRO(GameApi::FtA)
MACRO(GameApi::FD)
MACRO(GameApi::BO)
MACRO(GameApi::BB)
MACRO(GameApi::FB)
MACRO(GameApi::PT)
MACRO(GameApi::V)
MACRO(GameApi::FO)
MACRO(GameApi::SFO)
MACRO(GameApi::O)
MACRO(GameApi::COV)
MACRO(GameApi::CBM)
MACRO(GameApi::P)
MACRO(GameApi::SH)
MACRO(GameApi::CO)
MACRO(GameApi::Ft)
MACRO(GameApi::VA)
MACRO(GameApi::VX)
MACRO(GameApi::PL)
MACRO(GameApi::TX)
MACRO(GameApi::TXID)
MACRO(GameApi::TXA)
MACRO(GameApi::LI)
MACRO(GameApi::LLA)
MACRO(GameApi::PTS)
MACRO(GameApi::FBO)
MACRO(GameApi::W)
MACRO(GameApi::ML)
MACRO(GameApi::PTA)
MACRO(GameApi::MN)
MACRO(GameApi::VO)
MACRO(GameApi::IM)
MACRO(GameApi::TL)
MACRO(GameApi::T)
MACRO(GameApi::MT)
MACRO(GameApi::C)
MACRO(GameApi::MS)
MACRO(GameApi::MC)
#undef MACRO


template<typename T> T from_stream2(std::stringstream &is, GameApi::EveryApi &ev)
{
  FromStreamClass<T> cls;
  std::string s;
  is >> s;
  return cls.from_stream(s,ev);
}



template<class T, class RT, class... P>
int funccall(GameApi::EveryApi &ev, T (GameApi::EveryApi::*api),
	     RT (T::*fptr)(P...), std::vector<std::string> s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name)
{
  int s3 = s.size();
  for(int i=0;i<s3;i++)
    {
      std::string pn = param_name[i];
      int s4 = e.names.size();
      for(int j=0;j<s4;j++)
	{
	  std::string n = e.names[j];
	  std::string v = e.values[j];
	  if (pn == n)
	    {
	      s[i] = v;
	    }
	}
      
    }


  std::stringstream ss;
  int s2 = s.size();
#ifndef EMSCRIPTEN
  for(int i=s2-1;i>=0;i--)
    {
      ss << s[i] << " ";
    }
#else
  for(int i=0;i<s2;i++)
    {
      ss << s[i] << " ";
    }
#endif
  std::cout << "FuncCall: " << ss.str() << std::endl;

  std::stringstream ss2(ss.str());
  
  T *ptr = &(ev.*api);
  RT val = (ptr->*fptr)(from_stream2<P>(ss2,ev)...);
  std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id;
}

static std::string unique_id_apiitem()
{
  static int id = 0;
  id++;
  std::stringstream ss;
  ss << id;
  return std::string("I") + ss.str();
}

template<class T, class RT, class... P>
class ApiItem : public GameApiItem
{
public:
  ApiItem(T (GameApi::EveryApi::*api), RT (T::*fptr)(P...),
	  std::string name, 
	  std::vector<std::string> param_name,
	  std::vector<std::string> param_type,
	  std::vector<std::string> param_default,
	  std::string ret_type, std::string api_name, std::string func_name,
	  std::string symbols, std::string comment) 
    : api(api), fptr(fptr), name(name), param_name(param_name),
      param_type(param_type), param_default(param_default), 
      return_type(ret_type), api_name(api_name), func_name(func_name), symbols(symbols), comment(comment) { }
  int Count() const { return 1; }
  std::string Name(int i) const { return name; }
  int ParamCount(int i) const { return param_name.size(); }
  std::string ParamName(int i, int p) const { return param_name[p]; }
  std::string ParamType(int i, int p) const { return param_type[p]; }
  std::string ParamDefault(int i, int p) const { return param_default[p]; }
  std::string ReturnType(int i) const { return return_type; }
  std::string Symbols() const { return symbols; }
  std::string Comment() const { return comment; }
  int Execute(GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e)
  {
    return funccall(ev, api, fptr, params, e, param_name); 
  }
  std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)
  {
    std::string s;
    int ss = params.size();
   for(int i=0;i<ss;i++)
      {
	s+= params[i];
      }

    s+= return_type;
    s+= " ";
    std::string id = unique_id_apiitem();
    s+= id;
    s+= "=";
    s+= "ev.";
    s+= api_name;
    s+= ".";
    s+= func_name;
    s+= "(";
    int sk = param_names.size();
    for(int i=0;i<sk;i++)
      {
	if (param_names[i]=="@")
	  {
	    s+=empty_param(param_type[i]);
	  }
	else
	  s+= param_names[i];
	if (i!=int(param_names.size())-1) s+=",";
      }
    s+=");\n";
    return std::make_pair(id, s);
  }
private:
  T (GameApi::EveryApi::*api);
  RT (T::*fptr)(P...);
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> param_type;
  std::vector<std::string> param_default;
  std::string return_type;
  std::string api_name;
  std::string func_name;
  std::string symbols;
  std::string comment;
};
template<class T, class RT, class... P>
GameApiItem* ApiItemF(T (GameApi::EveryApi::*api), RT (T::*fptr)(P...),
		      std::string name,
		      std::vector<std::string> param_name,
		      std::vector<std::string> param_type,
		      std::vector<std::string> param_default,
		      std::string return_type, std::string api_name, std::string func_name, std::string symbols="", std::string comment="")
{
  return new ApiItem<T,RT,P...>(api, fptr, name, param_name, param_type, param_default, return_type, api_name, func_name, symbols,comment);
}
std::vector<GameApiItem*> append_vectors(std::vector<GameApiItem*> vec1, std::vector<GameApiItem*> vec2)
{
  int s = vec2.size();
  for(int i=0;i<s;i++)
    {
      vec1.push_back(vec2[i]);
    }
  return vec1;
}
std::vector<GameApiItem*> textureapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_plane,
			 "tex_plane",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "256", "256" },
			 "TX", "texture_api", "tex_plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_bitmap,
			 "tex_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "TX", "texture_api", "tex_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_assign,
			 "tex_assign",
			 { "tx", "id", "x", "y", "bm" },
			 { "TX", "int", "int", "int", "BM" },
			 { "", "0", "0", "0", "" },
			 "TX", "texture_api", "tex_assign"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_coord,
			 "tex_coord",
			 { "tx", "id", "x", "y", "width", "height" },
			 { "TX", "int", "int", "int", "int", "int" },
			 { "", "0", "0", "0", "100", "100" },
			 "TX", "texture_api", "tex_coord"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::get_tex_coord,
			 "get_tex_coord",
			 { "tx", "id" },
			 { "TX", "int" },
			 { "", "0" },
			 "Q", "texture_api", "get_tex_coord"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare,
			 "tx_prepare",
			 { "tx" },
			 { "TX" },
			 { "" },
			 "TXID", "texture_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::bind,
			 "tx_bind",
			 { "va", "txid" },
			 { "VA", "TXID" },
			 { "", "" },
			 "VA", "texture_api", "bind"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare_arr,
			 "tx_prepare_arr",
			 { "ev", "vec", "sx", "sy" },
			 { "EveryApi&", "[BM]", "int", "int" },
			 { "ev", "", "255", "255"  },
			 "TXA", "texture_api", "prepare_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::bind_arr,
			 "tx_bind_arr",
			 { "va", "txa" },
			 { "VA", "TXA" },
			 { "", "" },
			 "VA", "texture_api", "bind_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::to_bitmap,
			 "tx_to_bitmap",
			 { "txid" },
			 { "TXID" },
			 { "" },
			 "BM", "texture_api", "to_bitmap"));

  return vec;
}
std::vector<GameApiItem*> volumeapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::from_bool_bitmap,
			 "o_from_bool_bitmap",
			 { "b", "dist" },
			 { "BB", "float" },
			 { "", "50.0" },
			 "O", "volume_api", "from_bool_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::sphere,
			 "o_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "O", "volume_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::cube,
			 "o_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "O", "volume_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::cone,
			 "o_cone",
			 { "p1", "p2", "rad1", "rad2" },
			 { "PT", "PT", "float", "float" },
			 { "", "", "100.0", "90.0" },
			 "O", "volume_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::torus,
			 "o_torus",
			 { "center", "u_x", "u_y", "dist1", "dist2" },
			 { "PT", "PT", "PT", "float", "float" },
			 { "", "", "", "100.0", "50.0" },
			 "O", "volume_api", "torus"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::colour,
			 "o_colour",
			 { "object", "color" },
			 { "O", "unsigned int" },
			 { "", "ffffffff" },
			 "O", "volume_api", "colour"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::subset_color,
			 "o_subset_color",
			 { "model", "color_subset", "color" },
			 { "O", "O", "unsigned int" },
			 { "", "", "ffffffff" },
			 "O", "volume_api", "subset_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::move,
			 "o_move",
			 { "obj", "dx", "dy", "dz" },
			 { "O", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "O", "volume_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatex,
			 "o_rotatex",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatey,
			 "o_rotatey",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatey"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatez,
			 "o_rotatez",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O", "volume_api", "rotatez"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::scale,
			 "o_scale",
			 { "object", "sx", "sy", "sz" },
			 { "O", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "O", "volume_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::not_op,
			 "o_notop",
			 { "object" },
			 { "O" },
			 { "" },
			 "O", "volume_api", "not_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::min_op,
			 "o_minop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "min_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::max_op,
			 "o_maxop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "max_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::andnot_op,
			 "o_andnot",
			 { "object1", "not_obj" },
			 { "O", "O" },
			 { "", "" },
			 "O", "volume_api", "andnot_op"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,int,int, float,float, float,float, float,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "30", "30", "30", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));
#if 0
  // keeps crashing
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts2",
			 { "object", "size", "wholesize" },
			 { "O", "int", "float" },
			 { "", "30", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rendercubes3,
			 "o_render",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "100", "100", "100", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "P", "volume_api", "rendercubes3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::sphere,
			 "im_sphere",
			 { "r" },
			 { "float" },
			 { "100.0" },
			 "IM", "implicit_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::blob,
			 "im_blob",
			 { "c", "c_x", "c_y", "cc_x", "cc_y" },
			 { "float", "float", "float", "float", "float" },
			 { "100.0", "0.0", "0.0", "60.0", "0.0" },
			 "IM", "implicit_api", "blob","[B]", "Broken"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::translate,
			 "im_translate",
			 { "obj", "dx", "dy", "dz" },
			 { "IM", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "IM", "implicit_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance,
			 "im_from_distance",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance_cyl,
			 "im_distance_cyl",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance_cyl"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::from_distance_sph,
			 "im_distance_sph",
			 { "fd", "pos_x", "pos_y", "pos_z", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "300.0", "300.0", "300.0", "256.0", "256.0" },
			 "IM", "implicit_api", "from_distance_sph"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_upper,
			 "im_render_upper",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float", "float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "FB", "implicit_api", "render_upper"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_lower,
			 "im_render_lower",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float","float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "FB", "implicit_api", "render_lower"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_upper_color,
			 "im_upper_color",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float", "float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "BM", "implicit_api", "render_upper_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::implicit_api, &GameApi::ImplicitApi::render_lower_color,
			 "im_lower_color",
			 { "obj", "size_x", "size_y", "sx", "sy", "dx", "dy" },
			 { "IM", "float", "float", "int", "int","float","float" },
			 { "", "300.0", "300.0", "255", "255", "0.5", "0.5" },
			 "BM", "implicit_api", "render_lower_color"));

  return vec;
}
std::vector<GameApiItem*> floatvolumeapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::from_volume,
			 "fo_from_volume",
			 { "o", "false_val", "true_val" },
			 { "O", "float", "float" },
			 { "", "0.0", "1.0" },
			 "FO", "float_volume_api", "from_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::from_float_bitmap,
			 "fo_from_fbm",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "FB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "FO", "float_volume_api", "from_float_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::move,
			 "fo_move",
			 { "f1", "dx", "dy", "dz" },
			 { "FO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FO", "float_volume_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::minimum,
			 "fo_min",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO", "float_volume_api", "minimum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::maximum,
			 "fo_max",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO", "float_volume_api", "maximum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::subvolume,
			 "fo_subvolume",
			 { "f", "start_range", "end_range" },
			 { "FO", "float", "float" },
			 { "", "0.5", "1.0" },
			 "O", "float_volume_api", "subvolume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal2,
			 "fo_normal",
			 { "fo", "stepsize" },
			 { "FO", "float" },
			 { "", "0.1" },
			 "VO", "vector_volume_api", "normal2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::setup_normal,
			 "setup_normal",
			 { "orig", "v" },
			 { "P", "VO" },
			 { "", "" },
			 "P", "vector_volume_api", "setup_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::sphere,
			 "fd_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "FD", "dist_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::cube,
			 "fd_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "FD", "dist_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::round_cube,
			 "fd_round_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r" },
			 { "float", "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "10.0" },
			 "FD", "dist_api", "round_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::torus,
			 "fd_torus",
			 { "r_1", "r_2" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "torus"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::cone,
			 "fd_cone",
			 { "c_x", "c_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::plane,
			 "fd_plane",
			 { "n_x", "n_y", "n_z", "n_w" },
			 { "float", "float", "float", "float" },
			 { "0.0", "1.0", "0.0", "1.0" },
			 "FD", "dist_api", "plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::hex_prism,
			 "fd_hex_prism",
			 { "h_x", "h_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "hex_prism"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::tri_prism,
			 "fd_tri_prism",
			 { "h_x", "h_y" },
			 { "float", "float" },
			 { "100.0", "30.0" },
			 "FD", "dist_api", "tri_prism"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::triangle,
			 "fd_triangle",
			 { "a", "b", "c" },
			 { "PT", "PT", "PT" },
			 { "", "", ""},
			 "FD", "dist_api", "triangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::quad,
			 "fd_quad",
			 { "a", "b", "c", "d" },
			 { "PT","PT", "PT", "PT" },
			 { "", "", "", "" },
			 "FD", "dist_api", "quad"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::line,
			 "fd_line",
			 { "start", "end", "dist" },
			 { "PT", "PT", "float" },
			 { "", "", "10.0" },
			 "FD", "dist_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::min,
			 "fd_min",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "min"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::max,
			 "fd_max",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "max"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::and_not,
			 "fd_and_not",
			 { "a1", "a2" },
			 { "FD", "FD" },
			 { "", "" },
			 "FD", "dist_api", "and_not"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::blend,
			 "fd_blend",
			 { "a1", "a2", "k" },
			 { "FD", "FD", "float" },
			 { "", "", "15.0" },
			 "FD", "dist_api", "blend"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::color,
			 "fd_color",
			 { "fd", "r", "g", "b", "a" },
			 { "FD", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "FD", "dist_api", "color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::trans,
			 "fd_trans",
			 { "fd", "dx", "dy", "dz" },
			 { "FD", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FD", "dist_api", "trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::rot_x,
			 "fd_rot_x",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "rot_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::rot_y,
			 "fd_rot_y",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "rot_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::rot_z,
			 "fd_rot_z",
			 { "fd", "angle" },
			 { "FD", "float" },
			 { "", "0.0" },
			 "FD", "dist_api", "rot_z"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal,
			 "fd_normal",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "VO", "vector_volume_api", "normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::recalculate_normals,
			 "fd_recalc_normals",
			 { "fd" },
			 { "FD" },
			 { "" },
			 "FD", "dist_api", "recalculate_normals"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::ambient_occulsion,
			 "fd_amb_occul",
			 { "fd", "d", "i" },
			 { "FD", "float", "float" },
			 { "", "10.2", "30.0" },
			 "FD", "dist_api", "ambient_occulsion","[B]", "Broken"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render2,
			 "fd_render2",
			 { "ev", "obj", "sx", "sy" },
			 { "EveryApi&", "FD", "int", "int" },
			 { "ev", "", "300", "300" },
			 "BM", "dist_api", "render2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::render,
			 "fd_render_bm",
			 { "obj", "pos", "u_x", "u_y", "u_z", "sx", "sy" },
			 { "FD", "PT", "V", "V", "V", "int", "int" },
			 { "", "", "", "", "", "100", "100" },
			 "BM", "dist_api", "render"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly,
			 "fd_render_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "400.0", "400.0", "50", "50", "100.0", "100.0" },
			 "P", "dist_api", "distance_poly","[B]","Sometimes gives bad results"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_cyl,
			 "fd_render_cyl_p",
			 { "ev", "fd", "pos_x", "pos_y", "pos_z", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "0.0", "-600.0", "0.0", "300.0", "1200.0", "300.0", "50", "50", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_cyl","[B]", "Sometimes gives bad result"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::dist_api, &GameApi::DistanceFloatVolumeApi::distance_poly_sph,
			 "fd_render_sph_p",
			 { "ev", "fd", "dx", "dy", "dz", "sx", "sy", "ssx", "ssy", "ssxi", "ssyi", "ssx2", "ssy2" },
			 { "EveryApi&", "FD", "float", "float", "float", "int", "int", "float", "float", "int", "int", "float", "float" },
			 { "ev", "", "400.0", "400.0", "400.0", "256", "256", "40.0", "40.0", "50", "50", "50.0", "50.0" },
			 "P", "dist_api", "distance_poly_sph", "[B]", "Sometimes gives bad result"));

  return vec;
}
std::vector<GameApiItem*> colorvolumeapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_float_volume,
			 "cov_from_fo",
			 { "fo", "color0", "color1" },
			 { "FO", "unsigned int", "unsigned int" },
			 { "", "ff888888", "ffffffff" },
			 "COV", "color_volume_api", "from_float_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_volume,
			 "cov_from_o",
			 { "obj", "col_true", "col_false" },
			 { "O", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "COV", "color_volume_api", "from_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_continuous_bitmap,
			 "cov_from_cbm",
			 { "bm" },
			 { "CBM" },
			 { "" },
			 "COV", "color_volume_api", "from_continuous_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::mix,
			 "cov_mix",
			 { "p1", "p2", "value" },
			 { "COV", "COV", "float" },
			 { "", "", "0.5" },
			 "COV", "color_volume_api", "mix"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::directcolor,
			 "cov_color_from_normal",
			 { "normal" },
			 { "VO" },
			 { "" },
			 "COV", "color_volume_api", "directcolor"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::or_cov,
			 "cov_or",
			 { "p1", "p2" },
			 { "COV", "COV" },
			 { "", "" },
			 "COV", "color_volume_api", "or_cov"));
#endif
  return vec;
}

std::vector<GameApiItem*> vectorapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_api, &GameApi::VectorApi::vector,
			 "vector",
			 { "dx", "dy", "dz" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "V", "vector_api", "vector"));

  return vec;
}
std::vector<GameApiItem*> pointapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, &GameApi::PointApi::point,
			 "point",
			 { "x", "y", "z" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "PT", "point_api", "point"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, (GameApi::PT (GameApi::PointApi::*)(GameApi::PT,float,float,float))&GameApi::PointApi::move,
			 "move",
			 { "p1", "dx", "dy", "dz" },
			 { "PT", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PT", "point_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, &GameApi::PointApi::mix,
			 "mix",
			 { "p1", "p2", "val" },
			 { "PT", "PT", "float" },
			 { "", "", "0.5" },
			 "PT", "point_api", "mix"));

  return vec;
}

std::vector<GameApiItem*> fontapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::newfont,
			 "newfont",
			 { "file", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "FreeSans.ttf", "20", "20" },
			 "Ft", "font_api", "newfont"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph,
			 "glyph",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BM", "font_api", "glyph"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_outline,
			 "glyph_outline",
			 { "font", "idx", "sx", "sy" },
			 { "Ft", "long", "float", "float" },
			 { "", "64", "100.0", "100.0" },
			 "LI", "font_api", "glyph_outline"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_plane,
			 "glyph_plane",
			 { "font", "idx", "sx", "sy", "dx", "dy" },
			 { "Ft", "long", "float", "float", "float", "float" },
			 { "", "64", "100.0", "100.0", "30.0", "30.0" },
			 "PL", "font_api", "glyph_plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string,
			 "font_string",
			 { "font", "str", "x_gap" },
			 { "Ft", "std::string", "int" },
			 { "", "Hello", "5" },
			 "BM", "font_api", "font_string"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_fb,
			 "glyph_fb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "FB", "font_api", "glyph_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_bb,
			 "glyph_bb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BB", "font_api", "glyph_bb"));
#endif
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas_info,
			 "atlas_info",
			 { "ev", "font", "chars", "sx", "sy", "y_delta" },
			 { "EveryApi&", "Ft", "std::string", "float", "float", "int" },
			 { "ev", "", "0123456789", "30", "30", "40" },
			 "FtA", "font_api", "font_atlas_info"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas,
			 "font_atlas",
			 { "ev", "font", "atlas", "sx", "sy" },
			 { "EveryApi&", "Ft", "FtA", "float", "float" },
			 { "ev", "", "", "30", "30" },
			 "BM", "font_api", "font_atlas"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_from_atlas,
			 "font_string2",
			 { "ev", "atlas", "atlas_bm", "str", "x_gap" },
			 { "EveryApi&", "FtA", "BM", "std::string", "int" },
			 { "ev", "", "", "Hello", "5" },
			 "BM", "font_api", "font_string_from_atlas"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::save_atlas,
			 "save_atlas",
			 { "atlas", "filename" },
			 { "FtA", "std::string" },
			 { "", "atlas1.txt" },
			 "()", "font_api", "save_atlas"));
#endif
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_atlas,
			 "load_atlas",
			 { "filename" },
			 { "std::string" },
			 { "atlas1.txt" },
			 "FtA", "font_api", "load_atlas"));
#endif
  return vec;
}
std::vector<GameApiItem*> moveapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::empty,
			 "mn_empty",
			 { },
			 { },
			 { },
			 "MN", "move_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::level,
			 "mn_custom",
			 { "mn" },
			 { "MN" },
			 { "" },
			 "MN", "move_api", "level"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::trans2,
			 "mn_translate",
			 { "next", "dx", "dy", "dz" },
			 { "MN", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "trans2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale2,
			 "mn_scale",
			 { "next", "sx", "sy", "sz" },
			 { "MN", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatex,
			 "mn_rotatex",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatey,
			 "mn_rotatey",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatey"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotatez,
			 "mn_rotatez",
			 { "next", "angle" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "rotatez"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::translate,
			 "anim_translate",
			 { "next", "start_time", "end_time", "dx", "dy", "dz" },
			 { "MN", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale,
			 "anim_scale",
			 { "next", "start_time", "end_time", "sx", "sy", "sz" },
			 { "MN", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotate,
			 "anim_rotate",
			 { "next", "start_time", "end_time", "p_x", "p_y", "p_z",
			     "v_x", "v_y", "v_z", "angle" },
			 { "MN", "float", "float",
			     "float", "float","float",
			     "float", "float","float",
			     "float" },
			 { "", "0.0", "100.0",
			     "0.0", "0.0", "0.0",
			     "0.0", "1.0", "0.0",
			     "3.14159" },
			 "MN", "move_api", "rotate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::compress,
			 "anim_compress",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "compress"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::change_time,
			 "anim_time",
			 { "next", "delta_time" },
			 { "MN", "float" },
			 { "", "0.0" },
			 "MN", "move_api", "change_time"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::time_repeat,
			 "anim_repeat",
			 { "next", "start_time", "repeat_duration" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "time_repeat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml,
			 "move_ml",
			 { "ev", "ml", "mn" },
			 { "EveryApi&", "ML", "MN" },
			 { "ev", "", "" },
			 "ML", "move_api", "move_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enable_ml,
			 "enable_ml",
			 { "ev", "ml", "start_time", "end_time" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "0.0", "100.0" },
			 "ML", "move_api", "enable_ml"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::level,
			 "tree_level",
			 { "vec" },
			 { "[MN]" },
			 { "" },
			 "TL", "tree_api", "level"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree,
			 "tree",
			 { "vec" },
			 { "[TL]" },
			 { "" },
			 "T", "tree_api", "tree"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_ml,
			 "tree_ml",
			 { "ev", "tree", "vec" },
			 { "EveryApi&", "T", "[ML]" },
			 { "ev", "", "" },
			 "ML", "tree_api", "tree_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_p,
			 "tree_p",
			 { "ev", "tree", "vec", "time" },
			 { "EveryApi&", "T", "[P]", "float" },
			 { "ev", "", "", "0.0" },
			 "P", "tree_api", "tree_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::def,
			 "m_def",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "MT", "materials_api", "def"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture,
			 "m_texture",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "MT", "materials_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_arr,
			 "m_texture_arr",
			 { "ev", "vec", "sx", "sy" },
			 { "EveryApi&", "[BM]", "int", "int" },
			 { "ev", "", "256", "256" },
			 "MT", "materials_api", "texture_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::snow,
			 "m_snow",
			 { "ev", "nxt" },
			 { "EveryApi&", "MT" },
			 { "ev", "" },
			 "MT", "materials_api", "snow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::web,
			 "m_web",
			 { "ev", "nxt" },
			 { "EveryApi&", "MT" },
			 { "ev", "" },
			 "MT", "materials_api", "web"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::dist_field_mesh,
			 "m_dist_field",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "","" },
			 "MT", "materials_api", "dist_field_mesh","[B]","Loses animations from SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind,
			 "m_bind",
			 { "p", "mat" },
			 { "P", "MT" },
			 { "", "" },
			 "ML", "materials_api", "bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst,
			 "m_bind_inst",
			 { "p", "pts", "mat" },
			 { "P", "PTS", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst2,
			 "m_bind_inst2",
			 { "p", "pta", "mat" },
			 { "P", "PTA", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst2"));

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_event,
			 "keyevent_ml",
			 { "ev", "ml", "mn", "type", "ch", "button", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "int", "int","float" },
			 { "ev", "", "", "768", "-1", "-1","10.0" },
			 "ML", "move_api", "key_event"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::wasd,
			 "wasd_ml",
			 { "ev", "ml", "w", "a", "s", "d", "duration" },
			 { "EveryApi&", "ML", "MN", "MN", "MN", "MN","float" },
			 { "ev", "",  "", "", "", "", "10.0" },
			 "ML", "move_api", "wasd"));
#endif
  return vec;
}
std::vector<GameApiItem*> polygonapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::empty,
			 "empty",
			 { },
			 { },
			 { },
			 "P", "polygon_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_model,
			 "load_model",
			 { "filename", "obj_num" },
			 { "std::string", "int" },
			 { "test.obj", "0" },
			 "P", "polygon_api", "load_model"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_model_all,
			 "load_model_all",
			 { "filename", "count" },
			 { "std::string", "int" },
			 { "test.obj", "30" },
			 "P", "polygon_api", "load_model_all"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::save_model_ml,
			 "save_model",
			 { "poly", "filename" },
			 { "P", "std::string" },
			 { "", "test.obj" },
			 "ML", "polygon_api", "save_model_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::alt,
			 "p_alt",
			 { "vec", "index" },
			 { "[P]", "int" },
			 { "", "0" },
			 "P", "polygon_api", "alt"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::triangle,
			 "triangle",
			 { "p1", "p2", "p3" },
			 { "PT", "PT", "PT" },
			 { "", "", "" },
			 "P", "polygon_api", "triangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad,
			 "quad",
			 { "p1", "p2", "p3", "p4" },
			 { "PT", "PT", "PT", "PT" },
			 { "", "", "", "" },
			 "P", "polygon_api", "quad"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_x,
			 "quad_x",
			 { "x", "y1", "y2", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "100.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_y,
			 "quad_y",
			 { "x1", "x2", "y", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "100.0" },
			 "P", "polygon_api", "quad_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_z,
			 "quad_z",
			 { "x1", "x2", "y1", "y2", "z" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "P", "polygon_api", "quad_z"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::polygon2,
			 "polygon",
			 { "vec" },
			 { "[PT]" },
			 { "" },
			 "P", "polygon_api", "polygon2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(float,float,float,float,float,float))&GameApi::PolygonApi::cube,
			 "cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "P", "polygon_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rounded_cube,
			 "rounded_cube",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r_radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "20.0" },
			 "P", "polygon_api", "rounded_cube"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere,
			 "sphere",
			 { "center", "radius", "numfaces1", "numfaces2" },
			 { "PT", "float", "int", "int" },
			 { "(0.0,0.0,0.0)",  "100.0", "30", "30" },
			 "P", "polygon_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::cone,
			 "cone",
			 { "numfaces", "p1", "p2", "radius1", "radius2" },
			 { "int", "PT", "PT", "float", "float" },
			 { "30", "(0.0,0.0,0.0)", "(0.0,100.0,0.0)", "30.0", "10.0" },
			 "P", "polygon_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::torus,
			 "torus",
			 { "numfaces1", "numfaces2", "center", "u_x", "u_y", "radius1", "uu_x", "uu_y", "radius2" },
			 { "int", "int", "PT", "V", "V", "float", "V", "V", "float" },
			 { "20", "20", "", "", "", "30.0", "", "", "10.0" },
			 "P", "polygon_api", "torus"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::deform,
			 "deform",
			 { "obj", "bools", "dx", "dy", "dz" },
			 { "P", "O", "float", "float", "float" },
			 { "", "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "deform"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::linear_span,
			 "linear_span",
			 { "ev", "li", "dx", "dy", "dz", "num_steps" },
			 { "EveryApi&", "LI", "float", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "100.0", "1" },
			 "P", "polygon_api", "linear_span"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_map,
			 "color_map",
			 { "bm", "sx", "sy", "z" },
			 { "BM", "float", "float", "float" },
			 { "", "100", "100", "0" },
			 "P", "polygon_api", "color_map"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3,
			 "color_map3",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3", "[B]", "Broken"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3_cyl,
			 "color_map3_cyl",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3_cyl", "[B]", "Broken"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::BM, GameApi::FB,float,float,float))&GameApi::PolygonApi::color_map3_sph,
			 "color_map3_sph",
			 { "bm", "fb", "sx", "sy", "z" },
			 { "BM", "FB", "float", "float", "float" },
			 { "", "", "100", "100", "0" },
			 "P", "polygon_api", "color_map3_sph", "[B]", "Slightly Broken"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color,
			 "color",
			 { "orig", "color" },
			 { "P", "unsigned int" },
			 { "", "ffffffff" },
			 "P", "polygon_api", "color"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mix_color,
			 "p_mix_color",
			 { "orig", "orig2", "val" },
			 { "P", "P", "float" },
			 { "", "", "0.5" },
			 "P", "polygon_api", "mix_color"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_faces,
			 "color_faces",
			 { "orig", "color_1", "color_2", "color_3", "color_4" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ffffffff", "ff888888", "ff888888" },
			 "P", "polygon_api", "color_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_from_normals,
			 "color_from_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "color_from_normals"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_from_texcoord,
			 "color_from_texcoord",
			 { "orig", "color_tl", "color_tr", "color_bl", "color_br" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffff8844", "ff884422", "ffaa8844", "ffffffff" },
			 "P", "polygon_api", "color_from_texcoord"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_range,
			 "color_range",
			 { "orig", "source_upper", "source_lower", "upper_range", "lower_range" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "00000000","ffffffff", "88888888" },
			 "P", "polygon_api", "color_range"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_lambert,
			 "color_lambert",
			 { "orig", "color", "light_dir" },
			 { "P", "unsigned int", "V" },
			 { "", "ffffffff", "" },
			 "P", "polygon_api", "color_lambert"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::recalculate_normals,
			 "recalc_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "recalculate_normals"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_grayscale,
			 "color_grayscale",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "color_grayscale"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::choose_texture,
			 "choose_texture",
			 { "orig", "tex" },
			 { "P", "int" },
			 { "", "0" },
			 "P", "polygon_api", "choose_texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_manual,
			 "texcoord",
			 { "orig", "p1_x", "p1_y", "p2_x", "p2_y", "p3_x", "p3_y", "p4_x", "p4_y" },
			 { "P", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "1.0", "0.0", "1.0", "1.0", "0.0", "1.0" },
			 "P", "polygon_api", "texcoord_manual"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_spherical2,
			 "texcoord_spherical",
			 { "ev", "center", "r", "orig" },
			 { "EveryApi&", "PT", "float", "P" },
			 { "ev", "", "100.0", "" },
			 "P", "polygon_api", "texcoord_spherical2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_elem,
			 "p_or_elem",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_array2,
			 "p_or_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "P", "polygon_api", "or_array2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::translate,
			 "translate",
			 { "orig", "dx", "dy", "dz" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "translate","[S]"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatex,
			 "rotatex",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatex","[S]"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatey,
			 "rotatey",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatey","[S]"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatez,
			 "rotatez",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatez","[S]"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::scale,
			 "scale",
			 { "orig", "sx", "sy", "sz" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "scale","[S]"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow,
			 "shadow",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(1.0,1.0,1.0)" },
			 "P", "polygon_api", "shadow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::reflection,
			 "reflection",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(0.0, 1.0,0.0)" },
			 "P", "polygon_api", "reflection"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::subpoly_change,
			 "subpoly_change",
			 { "p", "p2", "o" },
			 { "P", "P", "O" },
			 { "", "", "" },
			 "P", "polygon_api", "subpoly_change"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::memoize,
			 "memoize",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "memoize"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::persistent_cache,
			 "persistent_cache",
			 { "p", "filename" },
			 { "P", "std::string" },
			 { "", "cache.obj" },
			 "P", "polygon_api", "persistent_cache"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(GameApi::EveryApi&,std::vector<GameApi::P>,std::string,std::string,float,float,int,int))&GameApi::PolygonApi::world_from_bitmap,
			 "world_from_bitmap",
			 { "ev", "pieces", "filename", "chars", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "[P]", "std::string", "std::string", "float", "float", "int", "int" },
			 { "ev", "", "map.txt", ".0123456789", "100.0", "100.0", "5", "5" },
			 "P", "polygon_api", "world_from_bitmap"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_points,
			 "from_points",
			 { "p", "f" },
			 { "PTS", "std::function<P (int,float,float,float,unsigned int)>" },
			 { "", "" },
			 "P", "polygon_api", "from_points"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_lines,
			 "from_lines",
			 { "li", "f" },
			 { "LI", "std::function<P (int,float,float,float,float,float,float,float,unsigned int,unsigned int)>" },
			 { "", "" },
			 "P", "polygon_api", "from_lines"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_polygon,
			 "from_polygon",
			 { "p", "f" },
			 { "P", "std::function<P (int,PT,PT,PT,PT)>" },
			 { "", "" },
			 "P", "polygon_api", "from_polygon"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::renderpolytobitmap,
			 "p_to_bitmap",
			 { "ev", "p", "sh", "x", "y", "z", "sx", "sy" },
			 { "EveryApi&", "P", "SH", "float", "float", "float", "int", "int" },
			 { "ev", "", "", "0.0", "0.0", "0.0", "800", "600" },
			 "BM", "polygon_api", "renderpolytobitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::create_vertex_array,
			 "p_prepare",
			 { "p", "b" },
			 { "P", "bool" },
			 { "", "true" },
			 "VA", "polygon_api", "create_vertex_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_vertex_array_ml,
			 "p_render",
			 { "ev", "va" },
			 { "EveryApi&", "VA" },
			 { "ev", "" },
			 "ML", "polygon_api", "render_vertex_array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml,
			 "p_render_inst",
			 { "ev", "p", "pts" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced2_ml,
			 "p_render_inst2",
			 { "ev", "va", "pta" },
			 { "EveryApi&", "VA", "PTA" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced2_ml","","Can be used for dynamic changes for pta"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shading_shader,
			 "p_shading",
			 { "ev", "mainloop", "level1", "level2", "level3" },
			 { "EveryApi&", "ML", "unsigned int", "unsigned int", "unsigned int" },
			 { "ev", "", "ff442211", "ffff8844", "ffffffff" },
			 "ML", "polygon_api", "shading_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::noise_shader,
			 "p_noise",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "noise_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::light_shader,
			 "p_light",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "light_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::toon_shader,
			 "p_toon",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "toon_shader"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_ml,
			 "array_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "ML", "mainloop_api", "array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::shader_api, &GameApi::ShaderApi::shader_choice,
			 "shader",
			 { "ev", "choose" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "SH", "shader_api", "shader_choice"));
  return vec;
}
std::vector<GameApiItem*> shadermoduleapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::PT,float))&GameApi::ShaderModuleApi::sphere,
			 "sh_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "(0.0,0.0,0.0)", "100.0" },
			 "SFO", "sh_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float, float, float, float,float))&GameApi::ShaderModuleApi::cube,
			 "sh_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO", "sh_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::rounded_cube,
			 "sh_rcube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r" },
			 { "float", "float", "float", "float", "float", "float","float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "20.0" },
			 "SFO", "sh_api", "rounded_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture,
			 "sh_texture",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture_box,
			 "texture_box",
			 { "start_x", "end_x", "start_y", "end_y", "start_x", "end_x" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO", "sh_api", "texture_box"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float,float,float,float,float, float,float))&GameApi::ShaderModuleApi::line,
			 "sh_line",
			 { "start_x", "start_y", "start_z", "end_x", "end_y", "end_z", "width1", "width2" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "100.0", "100.0", "0.0", "15.0", "15.0" },
			 "SFO", "sh_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::plane,
			 "sh_plane",
			 { "center", "u_x", "u_y" },
			 { "PT", "V", "V" },
			 { "", "", "" },
			 "SFO", "sh_api", "plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::torus,
			 "sh_torus",
			 { "radius_1", "radius_2" },
			 { "float", "float" },
			 { "30.0", "10.0" },
			 "SFO", "sh_api", "torus"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color,
			 "sh_color",
			 { "obj", "r", "g", "b", "a" },
			 { "SFO", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "SFO", "sh_api", "color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float))&GameApi::ShaderModuleApi::rot_x,
			 "rot_x",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_y,
			 "rot_y",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_z,
			 "rot_z",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO", "sh_api", "rot_z"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::and_not,
			 "and_not",
			 { "obj", "not_obj" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO", "sh_api", "and_not"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::or_elem,
			 "sh_or_elem",
			 { "obj1", "obj2" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO", "sh_api", "or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::blend,
			 "sh_blend",
			 { "obj1", "obj2", "k" },
			 { "SFO", "SFO", "float" },
			 { "", "", "15.0" },
			 "SFO", "sh_api", "blend"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float,float,float))&GameApi::ShaderModuleApi::trans,
			 "sh_trans",
			 { "obj", "dx", "dy", "dz" },
			 { "SFO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "SFO", "sh_api", "trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bind_arg,
			 "bind_arg",
			 { "obj", "name", "value" },
			 { "SFO", "std::string", "std::string" },
			 { "", "a", "0" },
			 "SFO", "sh_api", "bind_arg","","Allows dynamic anims in the future"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color_from_normal,
			 "color_from_normal",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "color_from_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::stop_generation,
			 "stop_generation",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "stop_generation"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::mix_color,
			 "mix_color",
			 { "col1", "col2", "t" },
			 { "SFO", "SFO", "float" },
			 { "", "", "0.5" },
			 "SFO", "sh_api", "mix_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::grayscale,
			 "sh_grayscale",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "grayscale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::noise,
			 "sh_noise",
			 { "obj", "strength" },
			 { "SFO", "float" },
			 { "", "1.0" },
			 "SFO", "sh_api", "noise"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::ambient_occulsion,
			 "ambient_occulsion",
			 { "obj", "d", "i" },
			 { "SFO", "float", "float" },
			 { "", "1.0", "1.0" },
			 "SFO", "sh_api", "ambient_occulsion"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::soft_shadow,
			 "soft_shadow",
			 { "scene", "light_dir", "mint", "maxt", "k", "strong" },
			 { "SFO", "V", "float", "float", "float", "float" },
			 { "", "", "1.0", "5.0", "1.0", "3.0" },
			 "SFO", "sh_api", "soft_shadow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bounding_primitive,
			 "bounding_prim",
			 { "prim", "inside", "outside" },
			 { "SFO", "SFO", "SFO" },
			 { "", "", "" },
			 "SFO", "sh_api", "bounding_primitive","","Allows huge speedup in rendering of sfo"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::render,
			 "render",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::v_render,
			 "v_render",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO", "sh_api", "v_render"));
  return vec;

}
std::vector<GameApiItem*> linesapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::point_array,
			 "li_points",
			 { "arr" },
			 { "[PT]" },
			 { "" },
			 "LI", "lines_api", "point_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color",
			 { "li", "color" },
			 { "LI", "unsigned int" },
			 { "", "ffffffff" },
			 "LI", "lines_api", "change_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color2",
			 { "li", "color_1", "color_2" },
			 { "LI", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "LI", "lines_api", "change_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::from_polygon,
			 "li_from_polygon",
			 { "poly" },
			 { "P" },
			 { "" },
			 "LI", "lines_api", "from_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::border_from_bool_bitmap,
			 "border_from_bool_bitmap",
			 { "b", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "",   "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "LI", "lines_api", "border_from_bool_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::translate,
			 "li_translate",
			 { "lines", "dx", "dy", "dz" },
			 { "LI", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "LI", "lines_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::scale,
			 "li_scale",
			 { "lines", "m_x", "m_y", "m_z" },
			 { "LI", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "LI", "lines_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::line_product,
			 "li_product",
			 { "li1", "li2" },
			 { "LI", "LI" },
			 { "", "" },
			 "P", "lines_api", "line_product"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::prepare,
			 "li_prepare",
			 { "li" },
			 { "LI" },
			 { "" },
			 "LLA", "lines_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::update_ml,
			 "li_update",
			 { "lla", "li" },
			 { "LLA", "LI" },
			 { "", "" },
			 "ML", "lines_api", "update_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::render_ml,
			 "li_render",
			 { "ev", "lla" },
			 { "EveryApi&", "LLA" },
			 { "ev", "" },
			 "ML", "lines_api", "render_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::line,
			 "c_line",
			 { "p1", "p2" },
			 { "PT", "PT" },
			 { "", "" },
			 "C", "curve_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xy,
			 "c_circle_xy",
			 { "center", "r" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "C", "curve_api", "circle_xy"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xz,
			 "c_circle_xz",
			 { "center", "r" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "C", "curve_api", "circle_xz"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::linear,
			 "c_linear",
			 { "vec" },
			 { "[PT]" },
			 { "" },
			 "C", "curve_api", "linear"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::bezier,
			 "c_bezier",
			 { "vec" },
			 { "[PT]" },
			 { "" },
			 "C", "curve_api", "bezier"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::scale,
			 "c_scale",
			 { "curve", "sx", "sy", "sz" },
			 { "C", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "C", "curve_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::trans,
			 "c_trans",
			 { "curve", "dx", "dy", "dz" },
			 { "C", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "C", "curve_api", "trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::compose,
			 "c_compose",
			 { "vec" },
			 { "[C]" },
			 { "" },
			 "C", "curve_api", "compose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::change_length,
			 "c_change_length",
			 { "curve", "new_legnth" },
			 { "C", "float" },
			 { "", "1.0" },
			 "C", "curve_api", "change_length"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::split,
			 "c_split",
			 { "curve", "start", "end" },
			 { "C", "float", "float" },
			 { "", "0.0", "1.0" },
			 "C", "curve_api", "split"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::sample,
			 "c_sample",
			 { "curve", "num_samples" },
			 { "C", "int" },
			 { "", "10" },
			 "PTS", "curve_api", "sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::to_lines,
			 "c_render",
			 { "curve", "num_lines" },
			 { "C", "int" },
			 { "", "40" },
			 "LI", "curve_api", "to_lines"));

  return vec;
}
std::vector<GameApiItem*> pointsapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::from_float_volume,
			 "from_float_volume",
			 { "volume", "numpoints", "start_x", "start_y", "start_z", "end_x", "end_y", "end_z" },
			 { "FO", "int", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "1000", "0.0", "0.0", "0.0", "100.0", "100.0", "100.0" },
			 "PTS", "points_api", "from_float_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::or_points,
			 "or_points",
			 { "p1", "p2" },
			 { "PTS", "PTS" },
			 { "", "" },
			 "PTS", "points_api", "or_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::move,
			 "move_pts",
			 { "obj", "dx", "dy", "dz" },
			 { "PTS", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PTS", "points_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::scale,
			 "scale_pts",
			 { "obj", "sx", "sy", "sz" },
			 { "PTS", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "PTS", "points_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::color_points,
			 "color_pts",
			 { "p", "color" },
			 { "PTS", "unsigned int" },
			 { "", "ffffffff" },
			 "PTS", "points_api", "color_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::from_volume,
			 "from_volume",
			 { "o", "pos", "u_x", "u_y", "u_z", "sx", "sy", "sz" },
			 { "O", "PT", "V", "V", "V", "int", "int", "int" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,100.0,0.0)", "(0.0,0.0,100.0)", "100", "100", "100" },
			 "PTS", "points_api", "from_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::shadow_points,
			 "shadow_points",
			 { "obj", "pos", "u_x", "u_y", "light_vec" },
			 { "PTS", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,0.0,100.0)", "(1.0,1.0,0.0)" },
			 "PTS", "points_api", "shadow_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_plane,
			 "random_plane",
			 { "pos", "u_x", "u_y", "numpoints" },
			 { "PT", "V", "V", "int" },
			 { "", "", "", "100" },
			 "PTS", "points_api", "random_plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::prepare,
			 "pts_prepare",
			 { "p" },
			 { "PTS" },
			 { "" },
			 "PTA", "points_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::render_ml,
			 "pts_render",
			 { "ev", "a" },
			 { "EveryApi&", "PTA" },
			 { "ev", "" },
			 "ML", "points_api", "render_ml"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_points,
			 "ms_from_points",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "MS", "matrices_api", "from_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::subarray,
			 "ms_subarray",
			 { "ms", "start", "count" },
			 { "MS", "int", "int" },
			 { "", "0", "0" },
			 "MS", "matrices_api", "subarray"));
  return vec;
}
std::vector<GameApiItem*> floatbitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::empty,
			 "emptyfloat",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "FB", "float_bitmap_api", "empty"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::grayscale,
			 "grayscale",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "grayscale"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_red,
			 "from_red",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_red"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_green,
			 "from_green",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_green"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_blue,
			 "from_blue",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_blue"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_alpha,
			 "from_alpha",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB", "float_bitmap_api", "from_alpha"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::min_fb,
			 "min_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api", "mix_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::max_fb,
			 "max_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api","max_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::mix_fb,
			 "mix_fb",
			 { "fb1", "fb2", "val" },
			 { "FB", "FB", "float" },
			 { "", "", "0.5" },
			 "FB", "float_bitmap_api", "mix_fb"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::to_grayscale_color,
			 "to_grayscale_color",
			 { "fb", "r", "g", "b", "a", "r2", "g2", "b2", "a2" },
			 { "FB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "float_bitmap_api", "to_grayscale_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::choose_bitmap,
			 "choose_bitmap",
			 { "fb", "bm1", "bm2" },
			 { "FB", "BM", "BM" },
			 { "", "", "" },
			 "BM", "float_bitmap_api", "choose_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_bool,
			 "from_bool",
			 { "b", "val_true", "val_false" },
			 { "BB", "float", "float" },
			 { "", "1.0", "0.0" },
			 "FB", "float_bitmap_api", "from_bool"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::distance_field,
			 "distance_field",
			 { "bb" },
			 { "BB" },
			 { "" },
			 "FB", "float_bitmap_api", "distance_field","[B]","Broken"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::add_border,
			 "add_border",
			 { "fb" },
			 { "FB" },
			 { "" },
			 "FB", "float_bitmap_api", "add_border"));

  return vec;
}
std::vector<GameApiItem*> boolbitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::empty,
			 "emptybool",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "100", "100" },
			 "BB", "bool_bitmap_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::from_bitmaps_color,
			 "bb_from_bitmap",
			 { "bm", "r", "g", "b" },
			 { "BM", "int", "int", "int" },
			 { "", "255", "255", "255" },
			 "BB", "bool_bitmap_api", "from_bitmaps_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::from_float_bitmap,
			 "from_float_bitmap",
			 { "float_bm", "range_start", "range_end" },
			 { "FB", "float", "float" },
			 { "", "0.5", "1.0" },
			 "BB", "bool_bitmap_api", "from_float_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::line,
			 "line",
			 { "bg", "p_x", "p_y", "p2_x", "p2_y", "line_width1", "line_width2" },
			 { "BB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "100.0", "100.0", "3.0", "3.0" },
			 "BB", "bool_bitmap_api", "line"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::circle,
			 "circle",
			 { "bg", "center_x", "center_y", "radius" },
			 { "BB", "float", "float", "float" },
			 { "", "50.0", "50.0", "50.0" },
			 "BB", "bool_bitmap_api", "circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::rectangle,
			 "rectangle",
			 { "bg", "x", "y", "width", "height" },
			 { "BB", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "30.0", "30.0" },
			 "BB", "bool_bitmap_api", "rectangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::part_circle,
			 "part_circle",
			 { "sx", "sy", "x", "y", "start_angle", "end_angle", "start_rad", "end_rad" },
			 { "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "100", "100", "50.0", "50.0", "0.0", "1.0", "30.0", "50.0" },
			 "BB", "bool_bitmap_api", "part_circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::not_bitmap,
			 "not_bitmap",
			 { "b" },
			 { "BB" },
			 { "" },
			 "BB", "bool_bitmap_api", "not_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::andnot_bitmap,
			 "andnot_bitmap",
			 { "b1", "not_b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "andnot_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::or_bitmap,
			 "or_bitmap",
			 { "b1", "b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "or_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::xor_bitmap,
			 "xor_bitmap",
			 { "b1", "flip_b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB", "bool_bitmap_api", "xor_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::choose_bitmap,
			 "choose_bitmap",
			 { "bools", "true_bm", "false_bm" },
			 { "BB", "BB", "BB" },
			 { "", "", "" },
			 "BB", "bool_bitmap_api", "choose_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::sprite,
			 "bb_sprite",
			 { "bg", "sprite", "x", "y", "mult_x", "mult_y" },
			 { "BB", "BB", "float", "float", "float", "float" },
			 { "", "", "0.0", "0.0", "1.0", "1.0" },
			 "BB", "bool_bitmap_api", "sprite"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::to_bitmap,
			 "to_bitmap",
			 { "bools", "true_r", "true_g", "true_b", "true_a", "false_r", "false_g", "false_b", "false_a" },
			 { "BB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "bool_bitmap_api", "to_bitmap"));
  return vec;
}

std::vector<GameApiItem*> bitmapapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::newbitmap,
			 "new", 
			 { "sx", "sy", "color" },
			 { "int", "int", "unsigned int" },
			 { "100", "100", "00000000" },
			 "BM", "bitmap_api", "newbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::loadbitmap,
			 "load",
			 { "filename" },
			 { "std::string" },
			 { "test.png" },
			 "BM", "bitmap_api", "loadbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::subbitmap,
			 "sub",
			 { "orig", "x", "y", "width", "height" },
			 { "BM", "int", "int", "int", "int" },
			 { "",   "0", "0", "100", "100" },
			 "BM", "bitmap_api", "subbitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::alt,
			 "alt",
			 { "vec", "index" },
			 { "[BM]", "int" },
			 { "",   "0" },
			 "BM", "bitmap_api", "alt"));

  //vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::growbitmap,
  //			 "grow",
  //			 { "orig", "l", "t", "r", "b" },
  //			 { "BM", "int", "int", "int", "int" },
  //			 { "", "2", "2", "2", "2" },
  //			 "BM", "bitmap_api", "growbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int))&GameApi::BitmapApi::blitbitmap,
			 "blit",
			 { "bg", "orig", "x", "y" },
			 { "BM", "BM", "int", "int" },
			 { "", "", "0", "0" },
			 "BM", "bitmap_api", "blitbitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::FB))&GameApi::BitmapApi::blitbitmap,
			 "blitFB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "FB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::BB))&GameApi::BitmapApi::blitbitmap,
			 "blitBB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "BB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::interpolate_bitmap,
			 "interpolate",
			 { "orig1", "orig2", "x" },
			 { "BM", "BM", "float" },
			 { "", "", "0.0" },
			 "BM", "bitmap_api", "interpolate_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::repeat_bitmap,
			 "repeat",
			 { "orig1", "x_count", "y_xount" },
			 { "BM", "int", "int" },
			 { "", "1", "1" },
			 "BM", "bitmap_api", "repeat_bitmap"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap,
			 "world",
			 { "f", "int_bm", "dx", "dy" },
			 { "std::function<BM(int>", "BM", "int", "int" },
			 { "", "", "100", "100" },
			 "BM", "bitmap_api", "world_from_bitmap"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_x,
			 "flip_x",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "flip_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_y,
			 "flip_y",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "flip_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient,
			 "gradient",
			 { "pos_1", "pos_2", "color_1", "color_2", "sx", "sy" },
			 { "PT",    "PT",    "unsigned int", "unsigned int", "int", "int" },
			 { "ev.point_api.point(0.0,0.0,0.0)", "ev.point_api.point(0.0, 100.0, 0.0)", "ffffffff", "ff888888", "100", "100" },
			 "BM", "bitmap_api", "gradient"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chessboard,
			 "chessboard",
			 { "tile_sx", "tile_sy", "count_x", "count_y", "color_1", "color_2" },
			 { "int", "int", "int", "int", "unsigned int", "unsigned int" },
			 { "10", "10", "8", "8", "ffffffff", "ff888888" },
			 "BM", "bitmap_api", "chessboard"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap2,
			 "bm_world_from_bitmap",
			 { "ev", "v", "filename", "chars", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "[BM]", "std::string", "std::string", "int", "int", "int", "int" },
			 { "ev", "", "map.txt", ".01234567", "100", "100", "5", "5" },
			 "BM", "bitmap_api", "world_from_bitmap2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::persistent_cache,
			 "bm_persistent_cache",
			 { "bm", "filename" },
			 { "BM", "std::string" },
			 { "", "cache.ppm" },
			 "BM", "bitmap_api", "persistent_cache"));
 
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::create_vertex_array,
			 "bm_prepare",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "VA", "sprite_api", "create_vertex_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::update_vertex_array_ml,
			 "bm_update",
			 { "va", "bm" },
			 { "VA", "BM" },
			 { "", "" },
			 "ML", "sprite_api", "update_vertex_array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::render_sprite_vertex_array_ml,
			 "bm_render",
			 { "ev", "va" },
			 { "EveryApi&", "VA" },
			 { "ev", "" },
			 "ML", "sprite_api", "render_sprite_vertex_array_ml"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::empty,
			 "cbm_empty",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "1.0", "1.0" },
			 "CBM", "cont_bitmap_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::from_bitmap,
			 "cbm_from_bitmap",
			 { "bm", "xsize", "ysize" },
			 { "BM", "float", "float" },
			 { "", "1.0", "1.0" },
			 "CBM", "cont_bitmap_api", "from_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::to_bitmap,
			 "cbm_to_bitmap",
			 { "cbm", "sx", "sy" },
			 { "CBM", "int", "int" },
			 { "", "200", "200" },
			 "BM", "cont_bitmap_api", "to_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::sample,
			 "cbm_sample",
			 { "cbm", "sx", "sy" },
			 { "CBM", "int", "int" },
			 { "", "200", "200" },
			 "BM", "cont_bitmap_api", "sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::rotate,
			 "cbm_rotate",
			 { "cbm", "center_x", "center_y", "angle" },
			 { "CBM", "float", "float", "float" },
			 { "", "0.5", "0.5", "0.0" },
			 "CBM", "cont_bitmap_api", "rotate"));
  

  return vec;
}

std::vector<GameApiItem*> booleanopsapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::create_bo,
			 "create_bo", 
			 { "mesh", "bools", "fd" },
			 { "P", "O", "FD" },
			 { "", "", "" },
			 "BO", "bool_api", "create_bo"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::cube,
			 "cube_bo", 
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "split_x", "split_y" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "ev", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "18", "18" },
			 "BO", "bool_api", "cube"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::sphere,
			 "sphere_bo", 
			 { "ev", "center", "radius", "numfaces1", "numfaces2" },
			 { "EveryApi&", "PT", "float", "int", "int" },
			 { "ev", "", "100.0", "30", "30" },
			 "BO", "bool_api", "sphere"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::or_elem,
			 "or_elem_bo", 
			 { "ev", "obj", "obj2" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "or_elem"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::and_not,
			 "and_not_bo", 
			 { "ev", "obj", "not_obj" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "and_not"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::intersect,
			 "intersect_bo", 
			 { "ev", "obj", "obj2" },
			 { "EveryApi&", "BO", "BO" },
			 { "ev", "", "" },
			 "BO", "bool_api", "intersect"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_polygon,
			 "to_poly", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "P", "bool_api", "to_polygon"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_volume,
			 "to_volume", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "O", "bool_api", "to_volume"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_api, &GameApi::BooleanOps::to_dist,
			 "to_dist", 
			 { "obj" },
			 { "BO" },
			 { "" },
			 "FD", "bool_api", "to_dist"));

  

  return vec;
}
std::vector<GameApiItem*> global_functions;
std::vector<GameApiItem*> all_functions()
{
  std::vector<GameApiItem*> v1 = bitmapapi_functions();
  std::vector<GameApiItem*> v2 = polygonapi_functions();
  std::vector<GameApiItem*> v3 = boolbitmapapi_functions();
  std::vector<GameApiItem*> v4 = floatbitmapapi_functions();
  std::vector<GameApiItem*> v5 = shadermoduleapi_functions();
  std::vector<GameApiItem*> v6 = linesapi_functions();
  std::vector<GameApiItem*> v7 = pointsapi_functions();
  std::vector<GameApiItem*> v8 = pointapi_functions();
  std::vector<GameApiItem*> v9 = vectorapi_functions();
  std::vector<GameApiItem*> va = volumeapi_functions();
  std::vector<GameApiItem*> vb = floatvolumeapi_functions();
  std::vector<GameApiItem*> vc = colorvolumeapi_functions();
  std::vector<GameApiItem*> vd = fontapi_functions();
  std::vector<GameApiItem*> ve = textureapi_functions();
  std::vector<GameApiItem*> vf = booleanopsapi_functions();
  std::vector<GameApiItem*> vg = global_functions;
  std::vector<GameApiItem*> vh = moveapi_functions();

  std::vector<GameApiItem*> a1 = append_vectors(v1,v2);
  std::vector<GameApiItem*> a2 = append_vectors(v3,v4);
  std::vector<GameApiItem*> a3 = append_vectors(a1,a2);
  std::vector<GameApiItem*> a4 = append_vectors(a3, v5);
  std::vector<GameApiItem*> a5 = append_vectors(a4, v6);
  std::vector<GameApiItem*> a6 = append_vectors(a5, v7);
  std::vector<GameApiItem*> a7 = append_vectors(a6, v8);
  std::vector<GameApiItem*> a8 = append_vectors(a7, v9);
  std::vector<GameApiItem*> a9 = append_vectors(a8, va);
  std::vector<GameApiItem*> aa = append_vectors(a9, vb);
  std::vector<GameApiItem*> ab = append_vectors(aa, vc);
  std::vector<GameApiItem*> ac = append_vectors(ab, vd);
  std::vector<GameApiItem*> ad = append_vectors(ac, ve);
  std::vector<GameApiItem*> ae = append_vectors(ad, vf);
  std::vector<GameApiItem*> af = append_vectors(ae, vg);
  std::vector<GameApiItem*> ag = append_vectors(af, vh);
  return ag;
}
std::string GameApi::GuiApi::bitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = bitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::textureapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = textureapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::fontapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = fontapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::shadermoduleapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = shadermoduleapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::pointsapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = pointsapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::linesapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = linesapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::polygonapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = polygonapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::volumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = volumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::floatvolumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = floatvolumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::colorvolumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = colorvolumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}


std::string GameApi::GuiApi::boolbitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = boolbitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::floatbitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = floatbitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::pointapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = pointapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::vectorapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = vectorapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

std::string GameApi::GuiApi::booleanopsapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = booleanopsapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
std::string GameApi::GuiApi::moveapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = moveapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}




GameApi::W functions_widget(GameApi::GuiApi &gui, std::string label, std::vector<GameApiItem*> vec, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::FtA atlas2, GameApi::BM atlas_bm2, GameApi::W insert)
{
  std::vector<std::string> vec2;
  std::vector<std::string> vec3;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      GameApiItem *item = vec[i];
      vec2.push_back(item->Name(0));
      int sp = item->ParamCount(0);
      std::string typestr = "(";
      for(int j=0;j<sp;j++)
	{
	  std::string type = item->ParamType(0, j);
	  
	  if (type.size()<=4 && type.size()>0 && type[0]>='A' && type[0]<='Z')
	    {
	      if (typestr.size()!=1)
		typestr+=",";
	      typestr+=type;
	    }
	  if (type.size()>2 && type[0]=='[' && type[type.size()-1]==']')
	    {
	      if (type[1]=='[' && type[type.size()-2]==']') type = type.substr(1, type.size()-2);

	      if (typestr.size()!=1)
		typestr+=",";
	      typestr+= type;
	    }
	}
      typestr+=")->";
      typestr+=item->ReturnType(0);
      vec3.push_back(typestr);
    }
  GameApi::W w = gui.list_item_opened(120, label, atlas, atlas_bm, vec2, vec3, atlas2, atlas_bm2, insert);
  GameApi::W w2 = gui.list_item_title(120, label, atlas, atlas_bm);
  GameApi::W or_elem = gui.or_elem(w,w2);
  return or_elem;
}



GameApi::W GameApi::GuiApi::bitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BitmapApi", bitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::textureapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "TextureApi", textureapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::booleanopsapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BooleanOps", booleanopsapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::moveapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "MoveApi", moveapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}



GameApi::W GameApi::GuiApi::fontapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FontApi", fontapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::volumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "VolumeApi", volumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::floatvolumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FVolumeApi", floatvolumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::colorvolumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "CVolumeApi", colorvolumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}


GameApi::W GameApi::GuiApi::shadermoduleapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "ShaderApi", shadermoduleapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::linesapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "LinesApi", linesapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::pointsapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PointsApi", pointsapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::pointapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PointApi", pointapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::vectorapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "VectorApi", vectorapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::boolbitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BoolBitmap", boolbitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
GameApi::W GameApi::GuiApi::floatbitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FloatBitmap", floatbitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

GameApi::W GameApi::GuiApi::polygonapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PolygonApi", polygonapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

class InsertWidget : public GuiWidgetForward
{
public:
  InsertWidget(GameApi::EveryApi &ev,GuiWidget *w, std::function<void(int,int)> f) : GuiWidgetForward(ev, { w }), f(f) { activated = true;  state=false; }
  void activate() { activated = true; }
  void deactivate() { activated = false; }
  void update(Point2d mouse_pos, int button,int ch, int type)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type);
    if (activated)
      {
	set_pos(mouse_pos);
	
	if (!state && button==-1)
	  {
	    state=true;
	  }
	if (button==0 && type==1025 && state)
	  {
	    f(int(mouse_pos.x), int(mouse_pos.y));
	    state = false;
	  }
      }
  }
  void render()
  {
    if (activated)
      {
	vec[0]->render();
      }
  }
private:
  bool activated;
  bool state;
  std::function<void(int,int)> f;
};

GameApi::W GameApi::GuiApi::insert_widget(W item, std::function<void(int,int)> f)
{
  GuiWidget *w = find_widget(e, item);
  return add_widget(e, new InsertWidget(ev,w,f));
}
