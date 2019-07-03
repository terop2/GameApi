
#include "GameApi_h.hh"
#include "GameApi_gui.hh"

#ifdef RASP_PI_COMPILE_HACK_1
#define FIRST_PART 1
#else
#ifdef RASP_PI_COMPILE_HACK_2
#define SECOND_PART 1
#else
#ifdef RASP_PI_COMPILE_HACK_3
#define THIRD_PART 1
#else
#define FIRST_PART 1
#define SECOND_PART 1
#define THIRD_PART 1
#endif
#endif
#endif

#ifdef RASP_PI_PART_1
#define SECTION_1 1
#else
#ifdef RASP_PI_PART_2
#define SECTION_2 1
#else
#ifdef RASP_PI_PART_3
#define SECTION_3 1
#else
#define SECTION_1 1
#define SECTION_2 1
#define SECTION_3 1
#endif
#endif
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#if defined(ARM) || defined(RASPI) 
#ifdef FIRST_PART
int strlen(const char *ptr) { const char *p = ptr; while(*p) { p++;  } return p-ptr;}
#else
int strlen(const char *);
#endif
#endif

std::string unique_id_apiitem();
std::vector<GameApiItem*> all_functions();
std::vector<GameApiItem*> polydistfield_functions();
std::vector<GameApiItem*> waveform_functions();
std::vector<GameApiItem*> blocker_functions();
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> volumeapi_functions();
std::vector<GameApiItem*> floatvolumeapi_functions();
std::vector<GameApiItem*> colorvolumeapi_functions();
std::vector<GameApiItem*> vectorapi_functions();
std::vector<GameApiItem*> pointapi_functions();
std::vector<GameApiItem*> fontapi_functions();
std::vector<GameApiItem*> moveapi_functions();
std::vector<GameApiItem*> polygonapi_functions();
std::vector<GameApiItem*> shadermoduleapi_functions();
std::vector<GameApiItem*> framebuffermoduleapi_functions();
std::vector<GameApiItem*> textureapi_functions();
std::vector<GameApiItem*> booleanopsapi_functions();
std::vector<GameApiItem*> polygonapi_functions1();
std::vector<GameApiItem*> polygonapi_functions2();
std::vector<GameApiItem*> linesapi_functions();
std::vector<GameApiItem*> pointsapi_functions();
std::vector<GameApiItem*> floatbitmapapi_functions();
std::vector<GameApiItem*> boolbitmapapi_functions();
std::vector<GameApiItem*> bitmapapi_functions();

extern int async_pending_count;


class EmptyWidget : public GuiWidgetForward
{
public:
  EmptyWidget(GameApi::EveryApi &ev) : GuiWidgetForward(ev,{ }) { }
};


#ifdef FIRST_PART
#ifdef SECTION_1
const float keypress_rot_speed = 6.0;


class TimedWidget2 : public GuiWidgetForward
{
public:
  TimedWidget2(GameApi::EveryApi &ev, GuiWidget *next, float time) : GuiWidgetForward(ev, { next } ), time(time) 
  {
    old_time = ev.mainloop_api.get_time();
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type, mouse_wheel_y);
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
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);

  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type,mouse_wheel_y);
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

#ifndef EMSCRIPTEN
std::map<std::string, int> shared_text;
class TextGuiWidgetAtlas : public GuiWidgetForward
{
public:
  TextGuiWidgetAtlas(GameApi::EveryApi &ev, std::string label, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::SH sh, int x_gap) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), label(label), atlas(atlas), atlas_bm(atlas_bm), sh(sh), x_gap(x_gap) { firsttime = true; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2); 
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    if (firsttime)
      {
	rendered_bitmap = ev.font_api.font_string_from_atlas(ev, atlas, atlas_bm, label.c_str(), x_gap);
	GameApi::CBM sca = ev.cont_bitmap_api.from_bitmap(rendered_bitmap, 1.0, 1.0);
	int sx = ev.bitmap_api.size_x(rendered_bitmap);
	int sy = ev.bitmap_api.size_y(rendered_bitmap);
	scaled_bitmap = ev.cont_bitmap_api.sample(sca, sx/2, sy/2);

	std::stringstream ss;
	ss << sx << " " << sy;
	std::string key = label + ss.str();
	std::map<std::string,int>::iterator i = shared_text.find(key);
	if (i!=shared_text.end())
	  {

	    rendered_bitmap_va.id = (*i).second;
	  } else {
	  rendered_bitmap_va = ev.sprite_api.create_vertex_array(scaled_bitmap);
	  shared_text[key]=rendered_bitmap_va.id;
	}
	firsttime = false;
      }
    size.dx = ev.bitmap_api.size_x(scaled_bitmap);
    size.dy = ev.bitmap_api.size_y(scaled_bitmap);
  }
  void render()
  {
    if (is_visible())
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
  std::string label;
  GameApi::FtA atlas;
  GameApi::BM atlas_bm;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap; 
  GameApi::BM scaled_bitmap;
 GameApi::VA rendered_bitmap_va;
  int x_gap;
};
#endif
template<class T>
class EditorGuiWidget : public GuiWidgetForward
{
public:
  EditorGuiWidget(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::Ft font, GameApi::SH sh) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), font(font), sh(sh) { firsttime = true; active=false; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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

std::string FloatExprEval(std::string s);

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
template<>
class Conv<float>
{
public:
  static void set(float &target, std::string s) { s = FloatExprEval(s);  std::stringstream ss(s); ss >> target; }
  static void get(const float &target, std::string &s) { std::stringstream ss; ss << target; s=ss.str(); }
};
template<class T>
class MultilineEditor : public GuiWidgetForward
{
public:
  MultilineEditor(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::FI font, GameApi::SH sh, int x_gap, int line_height) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), font(font), sh(sh), x_gap(x_gap), line_height(line_height) {
    firsttime = true; active=false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1, 0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    shift=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
      }
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (ch==13) { ch='\n'; }
    if (shift) { 
#ifdef EMSCRIPTEN
      const char *chars1 = "";
      const char *chars2 = "";
#else
      const char *chars1 = "§1234567890+',.-abcdefghijklmnopqrstuvwxyz";
      const char *chars2 = "½!\"#¤%&/()=?*;:_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#endif
      int s = strlen(chars1);
      for(int i=0;i<s;i++)
	{
	  if (ch == chars1[i]) ch=chars2[i];
	}
      if (ch==45) ch='_';
    }
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
      Conv<T>::set(target, label);
    }

    if (firsttime || changed)
      {
	rendered_bitmap = ev.font_api.draw_text_string(font, label, x_gap, line_height);
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
    if (is_visible())
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	ev.sprite_api.render_sprite_vertex_array(rendered_bitmap_va);

      }
  }
private:
  bool firsttime;
  std::string allowed_chars;
  T &target;
  std::string label;
  GameApi::FI font;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
  GameApi::BM scaled_bitmap;
  GameApi::VA rendered_bitmap_va;
  bool active;
  int x_gap;
  int line_height;
  bool shift;
};


template<class T>
class EditorGuiWidgetAtlas : public GuiWidgetForward
{
public:
  EditorGuiWidgetAtlas(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::SH sh, int x_gap) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), atlas(atlas), atlas_bm(atlas_bm), sh(sh), x_gap(x_gap) { firsttime = true; active=false; 
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    shift=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (shift) { 
#ifdef EMSCRIPTEN
      const char *chars1 = "";
      const char *chars2 = "";
#else
      const char *chars1 = "§1234567890+',.-abcdefghijklmnopqrstuvwxyz";
      const char *chars2 = "½!\"#¤%&/()=?*;:_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#endif
      int s = strlen(chars1);
      for(int i=0;i<s;i++)
	{
	  if (ch == chars1[i]) ch=chars2[i];
	}
      if (ch==45) ch='_';
      //ch = std::toupper(ch); 
    }
    if (active && type==768)
      {
	int s = allowed_chars.size();
	for(int i=0;i<s;i++)
	  {
	    if (allowed_chars[i]==ch)
	      {
		label.push_back(ch);
		changed = true;
		break;
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
    if (is_visible())
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
    update(p3, -1,-1,-1,0);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    left=false; right=false;
  }
  ~PolyGuiWidget() { std::cout << "PolyGuiWidget destructor" << std::endl; }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible()) {
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
	g_low->ogl->glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	obj.render();
	g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	g_low->ogl->glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
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
    update(p3, -1,-1,-1,0);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    mat = ev.matrix_api.identity();
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
	g_low->ogl->glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	//obj.render();
	ev.shader_api.set_var(sh, "in_MV", mat);
	ev.polygon_api.render_vertex_array(p);
	g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	g_low->ogl->glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
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

extern bool g_is_quake;

class MLGuiWidget : public GuiWidgetForward
{
public:
  MLGuiWidget(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::ML p, GameApi::SH sh, GameApi::SH sh2,GameApi::SH sh_2d, GameApi::SH sh_arr, GameApi::SH old_sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), env(env), sh(sh), sh2(sh2), sh_2d(sh_2d), sh_arr(sh_arr), old_sh(old_sh), p(p),sx(sx),sy(sy), screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1,0);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    mat = ev.matrix_api.identity();
    e.cursor_pos = ev.point_api.point(0.0,0.0,0.0);
    e.type = -1;
    e.ch = -1;
    e.button = -1;
    left=false; right=false;
    keys_enabled=true;
    g_is_quake = false;
    firsttime2=true;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    if (firsttime)
      {
	ev.mainloop_api.reset_time();
      }
    if (type==0x300 || type==0x301)
      {
	//std::cout << "update: " << type << " " << ch << " " << button << std::endl;
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
    if (ch=='p' && type==0x300)
      {
	keys_enabled = false;
      }
    if (ch=='o' && type==0x300)
      {
	keys_enabled = true;
      }


    if (keys_enabled && !g_is_quake){
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

    if (is_visible())
      {

    if (firsttime2) {
      // firsttime
      MainLoopItem *item = find_main_loop(env, p);
      item->Prepare();
      firsttime2 = false;
    }


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
	g_low->ogl->glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	int c_x = ev.mainloop_api.get_corner_x();
	int c_y = ev.mainloop_api.get_corner_y();
	int c_sx = ev.mainloop_api.get_screen_rect_sx();
	int c_sy = ev.mainloop_api.get_screen_rect_sy();
	ev.mainloop_api.set_corner(pos.x, pos.y, sz.dx,sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	ev.shader_api.use(sh2);
	ev.mainloop_api.switch_to_3d(true, sh2, screen_x, screen_y);
	ev.shader_api.use(sh_2d);
	ev.mainloop_api.switch_to_3d(false, sh_2d, screen_x, screen_y);
	ev.shader_api.use(sh_arr);
	ev.mainloop_api.switch_to_3d(true, sh_arr, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	//obj.render();
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "in_MV", mat);
	ev.shader_api.use(sh2);
	ev.shader_api.set_var(sh2, "in_MV", mat);
	ev.shader_api.use(sh_2d);
	ev.shader_api.set_var(sh_2d, "in_MV", mat);
	ev.shader_api.use(sh_arr);
	ev.shader_api.set_var(sh_arr, "in_MV", mat);
	ev.shader_api.use(sh);
	GameApi::M in_T = ev.mainloop_api.in_T(ev, true);
	GameApi::M in_N = ev.mainloop_api.in_N(ev, true);
	//e.inMV = find_matrix(env, mat);
	ev.mainloop_api.execute_ml(p, sh, sh2, sh2, sh_arr,mat, in_T, in_N, sz.dx, sz.dy);
	e.type = -1;
	e.ch = -1;
	e.button = -1;

	//ev.polygon_api.render_vertex_array(p);
	g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	ev.shader_api.use(sh2);
	ev.mainloop_api.switch_to_3d(false, sh2, screen_x, screen_y);
	ev.shader_api.use(sh_arr);
	ev.mainloop_api.switch_to_3d(false, sh_arr, screen_x, screen_y);
	ev.shader_api.use(sh_2d);
	ev.mainloop_api.switch_to_3d(false, sh_2d, screen_x, screen_y);
	ev.mainloop_api.set_corner(c_x,c_y,c_sx,c_sy);
	g_low->ogl->glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      } 

      }
  }
private:
  GameApi::Env &env;
  GameApi::SH sh;
  GameApi::SH sh2;
  GameApi::SH sh_2d;
  GameApi::SH sh_arr;
  GameApi::SH old_sh;
  GameApi::ML p;
  GameApi::M mat;
  GameApi::MainLoopApi::Event e;
  //GameApi::PolygonObj obj;
  bool keys_enabled;
  bool firsttime;
  int sx,sy;
  int screen_x, screen_y;
  float rot_y;
  bool left,right;
  bool firsttime2;
};



class ShaderPlaneGuiWidget : public GuiWidgetForward
{
public:
  ShaderPlaneGuiWidget(GameApi::EveryApi &ev, GameApi::SFO p, GameApi::SH sh, int sx, int sy, int screen_x, int screen_y) : GuiWidgetForward(ev, { }), sh(sh), p(p),sx(sx),sy(sy),screen_x(screen_x), screen_y(screen_y) { firsttime = true; 
    Point2d p3 = {-666.0, -666.0 };
    update(p3, -1,-1,-1,0);
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
  

  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
    update(p3, -1,-1,-1,0);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    left=false; right=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
	g_low->ogl->glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	//glPointSize(5.0);
	//ev.mainloop_api.clear_3d();
	obj.render();
	g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	g_low->ogl->glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
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
    update(p3, -1,-1,-1,0);
    Point2d p2 = { 0.0,0.0 };
    set_pos(p2);
    rot_y = 0.0;
    left = false;
    right=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
	g_low->ogl->glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	obj.render();
	g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	g_low->ogl->glViewport(0,0,screen_x, screen_y);
	ev.shader_api.use(old_sh);
      }
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
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    ev.bitmap_api.prepare(bm);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    #if 1
    size.dx = ev.bitmap_api.size_x(bm);
    size.dy = ev.bitmap_api.size_y(bm);
    if (firsttime)
      {
        bm_va = ev.sprite_api.create_vertex_array(bm);
	//ev.sprite_api.clear_arrays(bm_va);
	firsttime = false;
      }
    #endif
  }
  void render()
  {
    #if 1
    if (is_visible())
      {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0));
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
      }
    #endif
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
  //static std::map<int, int> arrs;
};

#ifndef EMSCRIPTEN
//std::map<int, int> shared_sprites;
//int find_shared_sprites(int key) {
//  std::map<int,int>::iterator it = shared_sprites.find(key);
//  if (it!=shared_sprites.end()) return (*it).second;
//  return -1;
//}
#if 1
struct A { int key; int value; };
std::vector<A> shared_sprites;
int find_shared_sprites(int key) {
  int s = shared_sprites.size();
  for(int i=0;i<s;i++) { if (shared_sprites[i].key==key) return i; }
  return -1;
}
#endif
class IconGuiWidgetShared : public GuiWidgetForward
{
public:
  IconGuiWidgetShared(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::BM bm, GameApi::SH sh, int key) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), env(env), ev(ev), sh(sh), bm(bm), key(key) { firsttime=true;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    //ev.bitmap_api.prepare(bm);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    #if 1
    size.dx = ev.bitmap_api.size_x(bm);
    size.dy = ev.bitmap_api.size_y(bm);
    if (firsttime)
      {
	//std::map<int,int>::iterator i = shared_sprites.find(key);
	int index = find_shared_sprites(key);
	if (index!=-1) {
	  bm_va.id = shared_sprites[index].value;
	  RenderVertexArray *r = find_vertex_array_render(env, bm_va);
	  if (!r) {
	    GameApi::BM scaled = ev.bitmap_api.scale_bitmap(ev,bm, 100,100);
	    bm_va = ev.sprite_api.create_vertex_array(scaled);
	    A a;
	    a.key = key;
	    a.value = bm_va.id;
	    shared_sprites.push_back(a);
	    //std::cout << "shared_sprites " << shared_sprites.size() << " " << bm_va.id << std::endl; 
	    //shared_sprites[key] = bm_va.id;
	  }
	} else {
	  GameApi::BM scaled = ev.bitmap_api.scale_bitmap(ev,bm, 100,100);
	  bm_va = ev.sprite_api.create_vertex_array(scaled);
	  A a;
	  a.key = key;
	  a.value = bm_va.id;
	  shared_sprites.push_back(a);
	  // std::cout << "shared_sprites2 " << shared_sprites.size() << " " << bm_va.id << std::endl; 
	  //shared_sprites[key] = bm_va.id;
	  //shared_sprites[a.key] = a.value;
	  // shared_sprites[key] = bm_va.id;
	}
	firsttime = false;
      }
    #endif
  }
  void render()
  {
    #if 1
    if (is_visible())
      {
    if (!firsttime)
      {
	Point2d p = get_pos();
	GameApi::M m = ev.matrix_api.trans(p.x+0.5,p.y+0.5,0.0);
	GameApi::M ms = ev.matrix_api.scale(size.dx/100.0, size.dy/100.0, 1.0);
	GameApi::M mc = ev.matrix_api.mult(ms,m);
	ev.shader_api.set_var(sh, "in_MV", mc);
	ev.sprite_api.render_sprite_vertex_array(bm_va);
	ev.shader_api.use(sh);
      }
      }
    #endif
  }
  int render_to_bitmap()
  {
    return bm.id;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  bool firsttime;
  GameApi::SH sh;
  GameApi::BM bm;
  GameApi::VA bm_va;
  //static std::map<int, int> arrs;
  int key;
};
#endif

//std::map<int,int> IconGuiWidget::arrs;
class MarginGuiWidget : public GuiWidgetForward
{
public:
  MarginGuiWidget(GameApi::EveryApi &ev, GuiWidget *w, int l, int t, int r, int b) : GuiWidgetForward(ev, { w }), l(l), t(t), r(r), b(b) 
  {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1,0);
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
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
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
    update(p, -1,-1, -1,0);
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
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type, mouse_wheel_y);
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
    update(pos, -1,-1, -1,0);
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
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
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
    if (is_visible())
      {
    GuiWidgetForward::render();
      }
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
    update(pos, -1,-1, -1,0);
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
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
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
    if (is_visible())
      {
    GuiWidgetForward::render();
      }
  }
private:
  int y_gap;
  int selected_item;
};

int highlight_bitmap = -1;
int highlight_border = -1;
class HighlightGuiWidget : public GuiWidgetForward
{
public:
  HighlightGuiWidget(GameApi::EveryApi &ev, GameApi::SH sh, int sx, int sy) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), enabled(false), oldenabled(false), c_changed(false) { 
    firsttime=true; 
    size.dx = sx;
    size.dy = sy;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1,-1,0);
  }
  void update(Point2d mouse, int button,int ch, int type, int mouse_wheel_y)
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
	if (highlight_bitmap != -1)
	  {
	    empty_bm_va.id = highlight_bitmap;
	    bm_va.id = highlight_border;
	  }
	else {
	  Vector2d v;
	  v.dx = 100;
	  v.dy = 100;
	  GameApi::BB bg = ev.bool_bitmap_api.empty(v.dx, v.dy);
	  GameApi::BB bb_1 = ev.bool_bitmap_api.rectangle(bg, 0.0,0.0, 2.0, (float)v.dy);
	  GameApi::BB bb_2 = ev.bool_bitmap_api.rectangle(bb_1, 0.0,0.0, (float)v.dx, 2.0);
	  GameApi::BB bb_3 = ev.bool_bitmap_api.rectangle(bb_2, v.dx-2.0,0.0, 2.0, v.dy);
	  GameApi::BB bb_4 = ev.bool_bitmap_api.rectangle(bb_3, 0.0, v.dy-2, v.dx, 2.0);
	  bm = ev.bool_bitmap_api.to_bitmap(bb_4, 255,255,255,255, 0,0,0,0);
	  bm_va = ev.sprite_api.create_vertex_array(bm);
	  empty_bm = ev.bool_bitmap_api.to_bitmap(bg, 255,255,255,255, 0,0,0,0);
	  empty_bm_va = ev.sprite_api.create_vertex_array(empty_bm);
	  highlight_bitmap = empty_bm_va.id;
	  highlight_border = bm_va.id;
	}
	firsttime = false;
      }
  }
  void render()
  {
    if (is_visible())
      {
    if (!firsttime && enabled)
      {
	Vector2d v = get_size();
	Point2d p = get_pos();
	GameApi::M m = ev.matrix_api.trans(p.x+0.5, p.y+0.5, 0.0);
	GameApi::M ms = ev.matrix_api.scale(v.dx/100.0, v.dy/100.0, 1.0);
	GameApi::M mc = ev.matrix_api.mult(ms,m);
	ev.shader_api.set_var(sh, "in_MV", mc);
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
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
    update(p, -1, -1, -1,0);
  }
  void update(Point2d mouse, int button,int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse, button,ch, type, mouse_wheel_y);
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
    if (is_visible())
      {
    GuiWidgetForward::render();
    if (!firsttime && enabled)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x+0.5, p.y+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bm_va);
      }
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
    update(p, -1, -1, -1,0);

  }
  GameApi::PT line_func(int linenum, bool id)
  {
    if (id==false) {
	Point2d pos = t1->get_pos();
	pos.x += delta_x;
	pos.y += delta_y;
	pos+=delta_vec;
	return ev.point_api.point(pos.x, pos.y, 0.0);
      } else {
	Point2d pos = t2->get_pos();
	pos.x += delta2_x;
	pos.y += delta2_y;
	pos+=delta_vec;
	return ev.point_api.point(pos.x, pos.y, 0.0);
      }
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
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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

class LineWidget2 : public GuiWidgetForward
{
public:
  LineWidget2(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::SH sh, GuiWidget *t1, int delta_x, int delta_y,
	     GuiWidget *t2, int delta2_x, int delta2_y, GameApi::SH sh2)
    : GuiWidgetForward(ev, { } ), env(env), sh(sh), t1(t1), delta_x(delta_x), delta_y(delta_y),
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
    update(p, -1, -1, -1,0);

  }
  GameApi::PT line_func(int linenum, bool id)
  {
    if (id)
      return ev.point_api.point(100.0,0.0,0.0);
    return ev.point_api.point(0.0,0.0,0.0);
  }
  GameApi::P line_to_poly(int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)
  {
    //std::cout << sx << " " << sy << " " << sz << ":" << ex << " " << ey << " " << ez << std::endl;
    int width = 200;
    GameApi::PT p1 = ev.point_api.point(sx,sy,0.0);
    GameApi::PT p2 = ev.point_api.point(sx,sy+width,0.0);
    GameApi::PT p3 = ev.point_api.point(ex,ey,0.0);
    GameApi::PT p4 = ev.point_api.point(ex,ey+width,0.0);
    GameApi:: P poly =  ev.polygon_api.quad(p1,p2,p3,p4);
    //GameApi::P poly = ev.polygon_api.quad_z(sx,ex,sy,ey,0.0);
    return ev.polygon_api.color(poly, 0xffffffff);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
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
#if 0
	if (old_t1_pos.x != t1->get_pos().x ||old_t2_pos.x != t2->get_pos().x
	    ||old_t1_pos.y != t1->get_pos().y || old_t2_pos.y != t2->get_pos().y)
	  {
	    line = ev.lines_api.function([this](int a,bool b){ return line_func(a,b); }, 1);
	    line = ev.lines_api.change_color(line, 0xffffffff);
	    line_2 = ev.polygon_api.from_lines(line, [this](int a,float b,float c,float d,float e,float f,float g,unsigned int h,unsigned int i) { return line_to_poly(a,b,c,d,e,f,g,h,i); });
	    
	     
	    ev.polygon_api.update_vertex_array_no_memory(line_3, line_2);
	  }
	//ev.lines_api.update(line_p, line);
#endif
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
    if (is_visible())
      {
    if (!firsttime)
      {
	ev.shader_api.use(sh);
	Point2d p = t1->get_pos();
	p.x += delta_x;
	p.y += delta_y;
	p+=delta_vec;
	Point2d p2 = t2->get_pos();
	p2.x += delta2_x;
	p2.y += delta2_y;
	p2+=delta_vec;
	LineProperties prop(Point(p.x,p.y,0.0), Point(p2.x,p2.y,0.0));
	Matrix m0 = Matrix::Scale(1.0/100.0, 1.0/100.0,1.0);
	Matrix m1 = prop.translate_to_p1();
	Matrix m2 = prop.rotate_z_from_p1_in_2d();
	Matrix m3 = prop.scale_length_in_2d();
	Matrix mm = m0*m3*m2*m1;
	GameApi::M mm2 = add_matrix2(env, mm);
	ev.shader_api.set_var(sh, "in_MV", mm2);
	//ev.lines_api.render(line_p);
	ev.polygon_api.render_vertex_array(line_3);
	ev.shader_api.use(sh2);
      }
      }
  }
private:
  GameApi::Env &env;
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
GameApi::W GameApi::GuiApi::tooltip(W orig, W insert, std::string label, FtA atlas, BM atlas_bm, int x_gap, float dx)
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
  return add_widget(e, new LineWidget2(e,ev, sh, t1, delta_x, delta_y,
				      t2, delta2_x, delta2_y, old_sh));
}
EXPORT GameApi::W GameApi::GuiApi::canvas(int sx, int sy)
{
  return add_widget(e, new CanvasWidget(ev, sx, sy));
}
EXPORT GameApi::W GameApi::GuiApi::find_canvas_item(W canvas, std::string id)
{
  GuiWidget *w = find_widget(e, canvas);
  if (!w) {
    {
      std::cout << "ERROR: find_canvas_item failed to find canvas!" << std::endl;
      GameApi::W wx;
      wx.id = -1;
      return wx;
    }
  }
#ifndef EMSCRIPTEN
  CanvasWidget *ww = dynamic_cast<CanvasWidget*>(w);
#else
  CanvasWidget *ww = static_cast<CanvasWidget*>(w);
#endif
  GuiWidget *item = ww->find_widget(id);
  if (!item)
    {
      std::cout << "ERROR: find_canvas_item failed to find widget from canvas!" << std::endl;
      GameApi::W wx;
      wx.id = -1;
      return wx;
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
  wx.id = -1;
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
  //std::cout << "List Item title height: " << size_y(node_t0) << std::endl;
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
    update(p, -1, -1, -1,0);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
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

class RectangleWidget : public GuiWidgetForward
{
public:
  RectangleWidget(GameApi::EveryApi &ev, GameApi::SH sh, int start_x, int end_x, int start_y, int end_y, unsigned int color) : GuiWidgetForward(ev, { } ), sh(sh), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), color(color) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1,0);
    firsttime = true;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
    Vector2d sz = { float(end_x-start_x), float(end_y-start_y) };
    size = sz;
    if (firsttime)
      {
	sx = size.dx;
	sy = size.dy;
	GameApi::BB b = ev.bool_bitmap_api.empty(sx,sy);
	GameApi::BB b1 = ev.bool_bitmap_api.rectangle(b, 0.0, 0.0, float(sx),float(sy));
	Color c(color);
	bg = ev.bool_bitmap_api.to_bitmap(b1, c.r,c.g,c.b,c.alpha,0,0,0,0);
	bg_va = ev.sprite_api.create_vertex_array(bg);
	firsttime = false;
      }

  }
  void render() {
    if (is_visible())
      {
    if (!firsttime)
      {
	Point2d p = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x, p.y, 0.0));
	ev.sprite_api.render_sprite_vertex_array(bg_va);
      }
      }

  }
private:
  int sx,sy;
  GameApi::SH sh;
  GameApi::BM bg;
  GameApi::VA bg_va;
  int start_x, end_x;
  int start_y, end_y;
  bool firsttime;
  unsigned int color;
};

EXPORT GameApi::W GameApi::GuiApi::rectangle(int start_x, int end_x, int start_y, int end_y, unsigned int color)
{
  return add_widget(e, new RectangleWidget(ev,sh,start_x, end_x, start_y, end_y, color));
}

class ClickAreaWidget : public GuiWidgetForward
{
public:
  ClickAreaWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height, int button_id2) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height) { done=false;
    selected = false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1,0);
    button_id = button_id2;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);

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
    update(p, -1, -1, -1,0);
    selected=false;
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);

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
#ifndef EMSCRIPTEN
  return add_widget(e, new TextGuiWidgetAtlas(ev, label, atlas, atlas_bm, sh, x_gap));
#else
  GameApi::W w;
  w.id = 0;
  return w;
#endif
  
}

EXPORT GameApi::W GameApi::GuiApi::icon(BM bm)
{
  return add_widget(e, new IconGuiWidget(ev, bm,sh));
}
EXPORT GameApi::W GameApi::GuiApi::icon_shared(BM bm, int key)
{
#ifndef EMSCRIPTEN
  return add_widget(e, new IconGuiWidgetShared(e,ev, bm, sh, key));
#else
  W w;
  w.id = 0;
  return w;
#endif
}
EXPORT GameApi::W GameApi::GuiApi::poly(P p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new PolyGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT GameApi::W GameApi::GuiApi::va(VA p, SH sh2, int sx, int sy, int screen_size_x, int screen_size_y)
{
  return add_widget(e, new VAGuiWidget(ev, p, sh2, sh, sx,sy, screen_size_x, screen_size_y));
}
EXPORT  GameApi::W GameApi::GuiApi::ml(ML p, SH sh2, SH sh3, SH sh_2d, SH sh_arr, int sx, int sy, int screen_size_x, int screen_size_y)
{
  
  return add_widget(e, new MLGuiWidget(e, ev, p, sh2, sh3, sh_2d, sh_arr, sh, sx,sy, screen_size_x, screen_size_y));
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
EXPORT GameApi::W GameApi::GuiApi::multiline_string_editor(std::string allowed_chars, std::string &target, FI font, int x_gap, int line_height)
{
  W e1 = add_widget(e, new MultilineEditor<std::string>(ev, allowed_chars, target, font, sh, x_gap, line_height));
  W e2 = highlight(e1);
  return e2;
}

EXPORT GameApi::W GameApi::GuiApi::float_editor(float &target, FtA atlas, BM atlas_bm, int x_gap)
{
  std::string allowed_chars = "0123456789.-+*/";
  W w = add_widget(e, new EditorGuiWidgetAtlas<float>(ev,allowed_chars, target, atlas, atlas_bm, sh, x_gap));
  W w2 = highlight(w);
  return w2;
}
EXPORT GameApi::W GameApi::GuiApi::url_editor(std::string &target, FtA atlas, BM atlas_bm, int x_gap)
{
#ifdef EMSCRIPTEN
  std::string allowed_chars="";
#else
  std::string allowed_chars = "0123456789.-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!\"#¤%&/()=?+\\*^.,-<>|§½;:_";
#endif
  W w = add_widget(e, new EditorGuiWidgetAtlas<std::string>(ev,allowed_chars, target, atlas, atlas_bm, sh, x_gap));
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
EXPORT GameApi::W GameApi::GuiApi::copy_paste_dialog(SH sh, W &close_button,FI font, FtA atlas, BM atlas_bm, std::string &edit)
{
#ifdef EMSCRIPTEN
  std::string allowed_chars= "";
#else
  std::string allowed_chars= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#¤%&/()=?\\|<>,.-;:_^~*'åöäÅÖÄ+";
#endif
  W w = multiline_string_editor( allowed_chars, edit, font, 5, 30);

  W bm_2 = margin(w, 10,10,10,10);
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

  //W code_1 = text("CodeGen", atlas, atlas_bm);
  //W code_2 = center_align(code_1, size_x(bm_4));
  //W code_3 = center_y(code_2, 60.0);
  //W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  //W code_41 = highlight(code_4);
  //W code_5 = layer(code_41, code_3);
  //W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  //codegen_button = code_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;

}

EXPORT GameApi::W GameApi::GuiApi::polygon_dialog(P p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button, W &mem)
{
  W bm_1 = poly(p, sh, 800,600, screen_size_x,screen_size_y);
  mem = bm_1;
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), c_dialog_1, c_dialog_1_2);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4)/3);
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), c_dialog_button_1, c_dialog_button_2);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5),0);
  close_button = but_6;

  W code_1 = text("CodeGen", atlas, atlas_bm);
  W code_2 = center_align(code_1, size_x(bm_4)/3);
  W code_3 = center_y(code_2, 60.0);
  W code_4 = button(size_x(code_3), size_y(code_3), c_dialog_button_1, c_dialog_button_2);
  W code_41 = highlight(code_4);
  W code_5 = layer(code_41, code_3);
  W code_6 = click_area(code_5, 0,0,size_x(code_5), size_y(code_5),0);
  codegen_button = code_6;

  W coll_1 = text("Edit", atlas, atlas_bm);
  W coll_2 = center_align(coll_1, size_x(bm_4)/3);
  W coll_3 = center_y(coll_2, 60.0);
  W coll_4 = button(size_x(coll_3), size_y(coll_3), c_dialog_button_1, c_dialog_button_2);
  W coll_41 = highlight(coll_4);
  W coll_5 = layer(coll_41, coll_3);
  W coll_6 = click_area(coll_5, 0,0,size_x(coll_5), size_y(coll_5),0);
  collect_button = coll_6;


  W arr_x[] = { code_6, coll_6, but_6 };
  W arr_x2 = array_x(&arr_x[0], 3, 0);

  //W popup = popup_box("Popup", { "Test1", "Test2", "Test3" }, atlas, atlas_bm);

  W arr[] = { bm_4, arr_x2 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::va_dialog(VA p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button)
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

EXPORT GameApi::W GameApi::GuiApi::ml_dialog(ML p, SH sh, SH sh2, SH sh_2d, SH sh_arr, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button)
{
  W bm_1 = ml(p, sh, sh2, sh_2d, sh_arr, 800,600, screen_size_x,screen_size_y);
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




EXPORT GameApi::W GameApi::GuiApi::shader_dialog(SFO p, W &close_button, FtA atlas, BM atlas_bm, int screen_x, int screen_y, W &codegen_button, W &collect_button)
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


EXPORT GameApi::W GameApi::GuiApi::lines_dialog(LI p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W& collect_button)
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

EXPORT GameApi::W GameApi::GuiApi::pts_dialog(PTS p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W &collect_button)
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



EXPORT GameApi::W GameApi::GuiApi::bitmap_dialog(BM bm, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W&collect_button)
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

EXPORT GameApi::W GameApi::GuiApi::edit_dialog(const std::vector<std::string> &labels, const std::vector<GameApi::GuiApi::EditTypes*> &vec, FtA atlas, BM atlas_bm, const std::vector<std::string> &types, W &cancel_but, W &ok_but, FtA atlas_tiny, BM atlas_tiny_bm)
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
      W lab = text(label, atlas,atlas_bm, 8);
      W lab_2 = right_align(lab, size_x1);
      W edit = generic_editor(*target, atlas, atlas_bm, type, 8, atlas_tiny, atlas_tiny_bm, size_y(lab));

      W array2[] = { lab_2, edit };
      W array3 = array_x(&array2[0], 2, 5);
      vec2.push_back(array3);
    }
  int button_width = 350;
  if (vec2.size()==0)
    {
      W lab0 = text("", atlas, atlas_bm, 8);
      W lab = text("(No data)", atlas, atlas_bm, 8);
      W lab_2 = center_align(lab, button_width*2);
      vec2.push_back(lab0);
      vec2.push_back(lab_2);
      vec2.push_back(lab0);
    }
  W array = array_y(&vec2[0], vec2.size(), 35);
  W array_1 = margin(array, 10,10,10,10);
  W array_1a = center_align(array_1, button_width*2);
  W array_2 = button(button_width*2, size_y(array_1), c_dialog_1, c_dialog_1_2 /*0xff884422, 0xff442211*/);
  W array_3 = layer(array_2, array_1a);


  W cancel_button = button(button_width,50, c_dialog_button_1, c_dialog_button_1 /*0xff884422, 0xff442211*/);
  W cancel_button_1 = text("Cancel", atlas,atlas_bm, 4);
  W cancel_button_11 = center_align(cancel_button_1, button_width);
  W cancel_button_111 = center_y(cancel_button_11, 50);
  W cancel_button_2 = layer(cancel_button, cancel_button_111);
  W cancel_button_3 = highlight(size_x(cancel_button_2), size_y(cancel_button_2));
  W cancel_button_4 = layer(cancel_button_2, cancel_button_3);
  W cancel_area = click_area(cancel_button_4, 0,0,button_width,50,0);
  cancel_but = cancel_area;
  W ok_button = button(button_width,50, c_dialog_button_1, c_dialog_button_1/*0xff884422, 0xff442211*/);
  W ok_button_1 = text("Ok", atlas,atlas_bm, 4);
  W ok_button_11 = center_align(ok_button_1, button_width);
  W ok_button_111 = center_y(ok_button_11, 50);
  W ok_button_2 = layer(ok_button, ok_button_111);
  W ok_button_3 = highlight(size_x(ok_button_2), size_y(ok_button_2));
  W ok_button_4 = layer(ok_button_2, ok_button_3);
  W ok_area = click_area(ok_button_4, 0,0,button_width,50,0);
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
    update(p, -1, -1, -1,0);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x + sx - size.dx, pos.y };
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type, mouse_wheel_y);
    set_pos(get_pos());
    size.dx = sx;
    size.dy = vec[0]->get_size().dy;
  }
private:
  int sx;
};
class LeftAlignWidget : public GuiWidgetForward
{
public:
  LeftAlignWidget(GameApi::EveryApi &ev, GuiWidget *wid, int sx) : GuiWidgetForward(ev, { wid }), sx(sx) {
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1,0);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    //Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x, pos.y };
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type, mouse_wheel_y);
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
    update(p, -1, -1, -1,0);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x + (sx - size.dx)/2, pos.y };
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type, mouse_wheel_y);
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
    update(p, -1, -1, -1,0);

  }
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d size = vec[0]->get_size();
    Point2d p = { pos.x, pos.y + (sy-size.dy)/2};
    vec[0]->set_pos(p);
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button, ch, type, mouse_wheel_y);
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
EXPORT GameApi::W GameApi::GuiApi::left_align(W item, int sx)
{
  GuiWidget *wid = find_widget(e, item);
  return add_widget(e, new LeftAlignWidget(ev, wid, sx));
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
std::string FloatExprEval(std::string s);

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
      std::string source2 = FloatExprEval(source);
      std::stringstream ss(source2);
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

EXPORT GameApi::W GameApi::GuiApi::generic_editor(EditTypes &target, FtA atlas, BM atlas_bm, std::string type, int x_gap, FtA atlas_tiny, BM atlas_tiny_bm, int sy)
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
      if (target.s.size()>4 && target.s.substr(0,4)=="http")
	{
	  W edit = url_editor(target.s, atlas_tiny, atlas_tiny_bm, x_gap);
	  W edit_2 = margin(edit, 0, sy-size_y(edit), 0, 0);
	  return edit_2;
	}
      else 
	{
      std::string allowed = "0123456789abcdefghijklmnopqrstuvwxyz/.ABCDEFGHIJKLMNOPQRSTUVWXYZ*()-#+/*!\"¤%&?\n,";
      W edit = string_editor(allowed, target.s, atlas_tiny, atlas_tiny_bm, x_gap);
      W edit_2 = margin(edit, 0, sy-size_y(edit), 0, 0);
      return edit_2;
	}
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

EXPORT GameApi::W GameApi::GuiApi::main_menu(std::vector<std::string> labels, FtA atlas, BM atlas_bm)
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
#endif // SECTION_1
#ifdef SECTION_2


class ScrollBarY : public GuiWidgetForward
{
public:
  ScrollBarY(GameApi::EveryApi &ev, GameApi::SH sh, int sx, int sy, int area_y) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), sh(sh), sx(sx), sy(sy), area_y(area_y) { firsttime=true; current_pos = 0.0; following = false; old_area_y = area_y; changed=false; }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
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
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    time = 0.0;
  }
  bool is_visible() const { return true; }
  Point2d get_pos() const { return pos; }
  Vector2d get_size() const { return size; }
  void set_pos(Point2d pos_p) { pos = pos_p; }
  void set_size(Vector2d vec_p) { size = vec_p; }
  void update(Point2d mouse_pos, int button,int ch, int type, int mouse_wheel_y)
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
    if (is_visible())
      {
    if (!firsttime)
      {
	Point2d pos = get_pos();
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(pos.x+0.5, pos.y+0.5, 0.0));
	ev.sprite_api.render_sprite_vertex_array(wave_bm_va);
      }
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
    mouse_wheel_move=0.0;
}
  void set_pos(Point2d pos)
  {
    GuiWidgetForward::set_pos(pos);
    Vector2d sz = vec[0]->get_size();
    Point2d pos2 = { pos.x-left*(sz.dx-size.dx), pos.y-top*(sz.dy-size.dy)+mouse_wheel_move };
    vec[0]->set_pos(pos2);
  }
  void set_size(Vector2d sz)
  {
    GuiWidgetForward::set_size(sz);
    size = sz;
    sx=(int)sz.dx;
    sy=(int)sz.dy;
  }
  void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y)
  {
    Point2d mouse = mouse_pos;
    if (mouse.x>=pos.x-80 && mouse.x<pos.x+size.dx+80 &&
	mouse.y>=pos.y-80 && mouse.y<pos.y+size.dy+80)
      {
	//Vector2d sz = vec[0]->get_size();
	//mouse.x -= left*(sz.dx-size.dx);
	//mouse.y -= top*(sz.dy-size.dy);
	orig->update(mouse, button,ch, type, mouse_wheel_y);
      }
    else
      {
	Point2d mouse = {-666.0, -666.0 };
	orig->update(mouse, button,ch, type, mouse_wheel_y);
      }
    if (type==1027 && mouse.x>pos.x && mouse.x<pos.x+size.dx &&
	mouse.y>pos.y && mouse.y<pos.y+size.dy)
      { // mouse wheel
	//std::cout << "Mouse wheel" << mouse_wheel_y << std::endl;
	mouse_wheel_move+=mouse_wheel_y*80.0;
	Point2d pos = get_pos();
	Vector2d sz = vec[0]->get_size();
	Point2d pos2 = { pos.x-left*(sz.dx-size.dx), pos.y-top*(sz.dy-size.dy)+mouse_wheel_move };
	vec[0]->set_pos(pos2);

      }
  }
  void render()
  {
    if (is_visible())
      {
    g_low->ogl->glEnable(Low_GL_SCISSOR_TEST);
    g_low->ogl->glScissor(pos.x, screen_y-pos.y-size.dy, size.dx, size.dy);
    vec[0]->render();
    g_low->ogl->glDisable(Low_GL_SCISSOR_TEST);
      }
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
    Point2d pos2 = { pos.x-left*(sz.dx-size.dx), pos.y-top*(sz.dy-size.dy)+mouse_wheel_move };
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
  float mouse_wheel_move;
  int screen_y;
};
class OrElemGuiWidget : public GuiWidget
{
public:
  OrElemGuiWidget(GuiWidget *w1, GuiWidget *w2) : b(false), w1(w1), w2(w2) { isVisible=true; }
  virtual bool is_visible() const { return isVisible; }
  virtual Point2d get_pos() const { if (b) return w1->get_pos(); else return w2->get_pos(); }
  virtual Vector2d get_size() const { if (b) return w1->get_size(); else return w2->get_size(); }
  virtual void set_pos(Point2d pos) { if (b) w1->set_pos(pos); else w2->set_pos(pos); } 
  virtual void set_size(Vector2d size) { if (b) w1->set_size(size); else w2->set_size(size); }
  virtual void update(Point2d mouse_pos, int button, int ch, int type, int mouse_wheel_y) { if (b) w1->update(mouse_pos, button,ch,type,mouse_wheel_y); else w2->update(mouse_pos, button,ch,type,mouse_wheel_y); }
  virtual void render() { if (is_visible()) {if (b) w1->render(); else w2->render();}  }
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
  bool isVisible;
};
EXPORT GameApi::W GameApi::GuiApi::gradient(int sx, int sy, PT pos_1, PT pos_2, unsigned int color_1, unsigned int color_2)
{
  //color_2 = color_1;
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, color_1, color_2, sx, sy);
  int key = color_1+color_2;
  return icon_shared(bm,key);
}
EXPORT GameApi::W GameApi::GuiApi::button(int sx, int sy, unsigned int color_1, unsigned int color_2)
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

  BM w4 = ev.bitmap_api.blitbitmap_bb(w2, w, 0, 0, mask_1);
  BM w5 = ev.bitmap_api.blitbitmap_bb(w4, w3, 0, 0, mask_2);
  int key = sx+sy+color_1+color_2;
  return icon_shared(w5,key);
}
EXPORT GameApi::W GameApi::GuiApi::or_elem(W w1, W w2)
{
  GuiWidget *ww1 = find_widget(e, w1);
  GuiWidget *ww2 = find_widget(e, w2);
  return add_widget(e, new OrElemGuiWidget(ww1,ww2));
}
EXPORT GameApi::W GameApi::GuiApi::scrollbar_y(int sx, int sy, int area_y)
{
  return add_widget(e, new ScrollBarY(ev,sh, sx,sy,area_y));
}
EXPORT GameApi::W GameApi::GuiApi::scrollbar_x(int sx, int sy, int area_x)
{
  return add_widget(e, new ScrollBarX(ev,sh, sx,sy,area_x));
}
EXPORT GameApi::W GameApi::GuiApi::scroll_area(W orig, int sx, int sy, int screen_y)
{
  GuiWidget *orig_1 = find_widget(e, orig);
  return add_widget(e, new ScrollAreaWidget(ev, sh, orig_1, sx,sy, screen_y));
}
#if 0
EXPORT GameApi::W GameApi::GuiApi::canvas_scroll_area(int sx, int sy, int screen_x, int screen_y)
{
}
#endif
EXPORT GameApi::W GameApi::GuiApi::waveform(std::function<float (float)> f, float start_range, float end_range, float min_value, float max_value, int sx, int sy, unsigned int true_color, unsigned int false_color)
{
  return add_widget(e, new WaveformWidget(ev, sh, f, start_range, end_range, min_value, max_value, sx, sy, true_color, false_color));
}
EXPORT GameApi::W GameApi::GuiApi::menu(W main_menu, int menu_id, std::vector<std::string> labels, FtA atlas, BM atlas_bm)
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

EXPORT GameApi::W GameApi::GuiApi::popup_menu(int x, int y, std::vector<std::string> labels, FtA atlas, BM atlas_bm, std::vector<W> &areas)
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
      bg = ev.bitmap_api.blitbitmap_bb(bg, bm_id_1, (int)p.x, (int)p.y, bb3);
    }
  return bg.id;
}
EXPORT int GameApi::GuiApi::pos_x(W w)
{
  GuiWidget *ww = find_widget(e, w);
  return int(ww->get_pos().x);
}
EXPORT int GameApi::GuiApi::pos_y(W w)
{
  GuiWidget *ww = find_widget(e, w);
  return int(ww->get_pos().y);
}
EXPORT  int GameApi::GuiApi::size_x(W w)
 {
  GuiWidget *ww = find_widget(e, w);
  return ww->get_size().dx;
 }
EXPORT  int GameApi::GuiApi::size_y(W w)
 {
   GuiWidget *ww = find_widget(e, w);
   return ww->get_size().dy;
 }
EXPORT void GameApi::GuiApi::set_pos(W w, float px, float py)
{
  Point2d p = { px, py };
  GuiWidget *ww = find_widget(e, w);
  ww->set_pos(p);
}
EXPORT void GameApi::GuiApi::set_size(W w, float sx, float sy)
{
  Vector2d v = { sx, sy };
  GuiWidget *ww = find_widget(e, w);
  ww->set_size(v);
}
EXPORT void GameApi::GuiApi::update(W w, PT mouse, int button,int ch, int type, int mouse_wheel_y)
{
  GuiWidget *ww = find_widget(e, w);
  Point *pt = find_point(e, mouse);
  Point2d pt2 = { pt->x, pt->y };
  ww->update(pt2, button,ch, type, mouse_wheel_y);
}
EXPORT void GameApi::GuiApi::render(W w)
{
  //std::cout << "GuiApi::render" << w.id << std::endl;
  GuiWidget *ww = find_widget(e, w);
  ww->render();
}
EXPORT int GameApi::GuiApi::chosen_item(W w)
{
  GuiWidget *ww = find_widget(e, w);
  if (!ww) { return -1; }
  return ww->chosen_item();
}
EXPORT void GameApi::GuiApi::select_item(W w, int item)
{
  GuiWidget *ww = find_widget(e, w);
  ww->select_item(item);
}
EXPORT float GameApi::GuiApi::dynamic_param(W w, int id)
{
  //std::cout << "GuiApi::dynamic_param" << w.id << std::endl;
  GuiWidget *ww = find_widget(e, w);
  if (!ww) { return 0.0; }
  return ww->dynamic_param(id);
}
EXPORT int GameApi::GuiApi::num_childs(W w)
{
  GuiWidget *ww = find_widget(e, w);
#ifdef EMSCRIPTEN
  GuiWidgetForward *ww2 = static_cast<GuiWidgetForward*>(ww);
#else
  GuiWidgetForward *ww2 = dynamic_cast<GuiWidgetForward*>(ww);
#endif
  return ww2->vec.size();
}
EXPORT GameApi::W GameApi::GuiApi::get_child(W w, int i)
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
EXPORT void GameApi::GuiApi::set_dynamic_param(W w, int id, float val)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_dynamic_param(id, val);
}
EXPORT std::string GameApi::GuiApi::get_id(W w)
{
  GuiWidget *ww = find_widget(e,w);
  return ww->get_id();
}
EXPORT void GameApi::GuiApi::set_id(W w, std::string id)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_id(id);
}
#endif
#endif // SECTION_2

template<class T>
class FromStreamClass
{
public:
  T from_stream(std::string s, GameApi::EveryApi &ev)
  {
    //std::cout << "Type using default: " << typeid(T).name() << std::endl;
    bool neg = false;
    if (s.size()>0 && s[0]=='-')
      {
	neg = true;
	s = s.substr(1);
      }
    
  T t;
  std::stringstream is(s);
  is >> t;
  if (neg) t = -t;

  //std::cout << "Default: " << t << std::endl;
  return t;
  }
};

std::string FloatExprEval(std::string s);
int find_float_ch(std::string s, char ch);



template<>
class FromStreamClass<float>
{
public:
  float from_stream(std::string s, GameApi::EveryApi &ev)
  {
    //std::cout << "Type using default: " << typeid(T).name() << std::endl;
    bool neg = false;
    if (s.size()>0 && s[0]=='-')
      {
	neg = true;
	s = s.substr(1);
      }
    s=FloatExprEval(s);

  float t;
  std::stringstream is(s);
  is >> t;
  if (neg) t = -t;

  //std::cout << "Default: " << t << std::endl;
  return t;
  }
};


template<>
class FromStreamClass<std::string>
{
public:
  std::string from_stream(std::string s, GameApi::EveryApi &ev)
  {
    return s;
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

template<>
class FromStreamClass<std::vector<std::string>>
{
public:
  std::vector<std::string> from_stream(std::string s, GameApi::EveryApi &ev)
  {
    std::vector<std::string> vec;
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass<std::string> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    std::string t = cls.from_stream(substr, ev);
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};

template<class T>
class FromStreamClass<std::vector<T>>
{
public:
  std::vector<T> from_stream(std::string s, GameApi::EveryApi &ev)
  {
    //std::cout << "Vector:" << s << std::endl;
    std::vector<T> vec;
    if (s.size()>0 && s[0]!='[')
      { // ARR version
	//std::cout << "Arr version" << s << std::endl;
	GameApi::ARR arr;
	std::stringstream ss(s);
	ss >> arr.id;
	ArrayType *t = find_array(ev.get_env(), arr);
	int s = t->vec.size();
	for(int i=0;i<s;i++)
	  {
	    T tt;
	    tt.id = t->vec[i];
	    vec.push_back(tt);
	  }
	return vec;
      }
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

std::string empty_param(std::string s);
#ifdef FIRST_PART
#ifdef SECTION_3
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
MACRO3(PD,ev.polygon_dist_api.empty(ev))
MACRO3(WV,ev.waveform_api.empty(1.0))
  return "@";
}
#endif
#endif

#ifdef FIRST_PART
#ifdef SECTION_3

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
MACRO2(GameApi::PD,ev.polygon_dist_api.empty(ev))
MACRO2(GameApi::WV,ev.waveform_api.empty(1.0))
#endif
#endif

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
MACRO(GameApi::SA)
MACRO(GameApi::PD)
MACRO(GameApi::WV)
MACRO(GameApi::CC)
MACRO(GameApi::BLK)
MACRO(GameApi::RUN)
MACRO(GameApi::PTT)
MACRO(GameApi::KF)
MACRO(GameApi::CPP)
MACRO(GameApi::IF)
MACRO(GameApi::SF)
MACRO(GameApi::SD)
MACRO(GameApi::FI)
MACRO(GameApi::GI)
MACRO(GameApi::ARR)
MACRO(GameApi::CMD)
MACRO(GameApi::PA)
MACRO(GameApi::PLF)
MACRO(GameApi::PLL)
MACRO(GameApi::PLP)
MACRO(GameApi::CBB)
MACRO(GameApi::CFB)
MACRO(GameApi::PH)
MACRO(GameApi::DC)
MACRO(GameApi::PN)
MACRO(GameApi::PF)
MACRO(GameApi::FF)
MACRO(GameApi::S)
MACRO(GameApi::SBM)
MACRO(GameApi::IBM)
MACRO(GameApi::INP)
MACRO(GameApi::MA)
MACRO(GameApi::CG)
MACRO(GameApi::FML)
MACRO(GameApi::FBU)
MACRO(GameApi::AV)
#undef MACRO


template<typename T> T from_stream2(std::stringstream &is, GameApi::EveryApi &ev)
{
  FromStreamClass<T> cls;
  std::string s;
  is >> s;
  //std::cout << "FromStreamClass: " << s << " " << typeid(T).name() << std::endl;
  return cls.from_stream(s,ev);
}

class GameApiEditNode_float_coord : public GameApi::EditNode
{
public:
  GameApiEditNode_float_coord(int count, std::string param_type, std::string param_value, std::string param_name) : source(0), mode(0), count(count), param_type(param_type), param_value(param_value), param_name(param_name) { move=false; mod=false; source=0; mode=0; }
  void SetSource(int i) { source=i; }
  void SetMode(int i) { mode=i; }
  void CurrentPos(Point &p) const
  {
    if (param_type=="float") {
      std::stringstream ss(param_value);
      if (mode==1) { ss >> p.x; }
      if (mode==2) { ss >> p.y; }
      if (mode==3) { ss >> p.z; }
    }
  }
  void HandleMouseMove(int x, int y) 
  {
    mod=false;
    if (source==1) { res = x; mod = true;}
    if (source==2) { res = y; mod = true; }
    if (mod && move && param_type=="float") {
      std::stringstream ss;
      ss << res;
      param_value = ss.str();
    }
  }
  // type==0 no event
  // type==1 mouse down
  // type==2 mouse up
  void HandleMousePress(int type) {
    if (type==1) { move=true; }
    if (type==2) { move=false; }
  }
  std::string GetValue() const { return param_value; }
private:
  int source; // 0=no source, 1=mouse_x, 2=mouse_y
  int mode; // 0=no edit, 1=edit_x, 2=edit_y, 3=edit_z
  int count;
  std::string param_type;
  std::string param_value;
  std::string param_name;
  int res;
  bool mod;
  bool move;
};

#if 0
int GameApi::collect_counter(int mode)
{
  static int count=0;
  if (mode==0) { count=0; }
  if (mode==1) { count++; }
  return count;
}
#endif
#if 0
std::vector<GameApi::EditNode*> collectnodes(std::string name, std::vector<std::string> param_type, std::vector<std::string> param_names, std::vector<std::string> params, std::vector<std::string> param_name)
{
  int s=param_type.size();
  std::cout << "Params: " << s << std::endl;
  for(int i=0;i<s;i++)
    {
      int count = GameApi::collect_counter(1);
      std::cout << count << " " << name << ":" << param_type[i] << " " << param_names[i] << ": " << param_name[i] << std::endl;
    }
  return std::vector<GameApi::EditNode*>();
}
#endif

template<class T>
int template_get_id(T t) { return t.id; }
#ifdef FIRST_PART
#ifdef SECTION_3
int template_get_id(GameApi::ARR a) { return 0; }
#endif
#endif

void funccall_1(std::vector<std::string> &s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name);

#ifdef FIRST_PART
#ifdef SECTION_3

void funccall_1(std::vector<std::string> &s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name)
{
  int s3 = s.size();
  if (s.size()!=param_name.size()) { std::cout << "funccall: param_names and parameter values std::vectors different size" << std::endl; }
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
}
#endif
#endif
template<class T, class RT, class... P>
int funccall(GameApi::Env &ee, GameApi::EveryApi &ev, T (GameApi::EveryApi::*api),
	     RT (T::*fptr)(P...), std::vector<std::string> s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name, std::string return_type)
{
  funccall_1(s,e,param_name);
#if 0
  int s3 = s.size();
  if (s.size()!=param_name.size()) { std::cout << "funccall: param_names and parameter values std::vectors different size" << std::endl; }
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
#endif

  std::stringstream ss;
  int s2 = s.size();
#ifndef EMSCRIPTEN
#ifndef ANDROID
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
#else
  for(int i=0;i<s2;i++)
    {
      ss << s[i] << " ";
    }
#endif
  //std::cout << "FuncCall: " << ss.str() << std::endl;

  std::stringstream ss2(ss.str());
  T *ptr = &(ev.*api);
  RT val = (ptr->*fptr)(from_stream2<P>(ss2,ev)...);

#if 0
  if (return_type.size()>2 && return_type[0]=='[' && return_type[return_type.size()-1]==']')
    { // array return type
      GameApi::ARR arr = add_array(ee, val);
      return arr.id;
    }
#endif
  
  //std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id; //template_get_id(val);
}



#ifdef FIRST_PART
#ifdef SECTION_3
int find_char(const std::string &line, int start_char, char ch, bool braces=true)
{
  int s = line.size();
  int level=0;
  for(int i=start_char;i<s;i++)
    {
      if (braces && (line[i]=='(' ||line[i]=='{')) { level++; }
      if (braces && ((line[i]==')' ||line[i]=='}') && level>0)) { level--; }

      if (line[i]==ch && level==0) return i;
    }
  return -1;
}
int find_one(std::string line, int start_char, std::string chars, bool braces=true)
{
  int s = line.size();
  int level=0;
  for(int i=start_char;i<s;i++)
    {
      if (braces && (line[i]=='('||line[i]=='{')) { level++; }
      if (braces && ((line[i]==')'||line[i]=='}') && level>0)) { level--; }
      int ss = chars.size();
      bool found = false;
      for(int j=0;j<ss;j++)
	{
	  if (line[i]==chars[j])
	    {
	      if (level==0) {
		found = true;
	      }
	    }
	}
      if (found) { return i; }
    }
  return -1;
}
int extend_until(std::string line, int start_char, std::string chars)
{
  int s = line.size();
  for(int i=start_char;i<s;i++)
    {
      int ss = chars.size();
      bool b = false;
      for(int j=0;j<ss;j++)
	if (line[i]==chars[j]) { b=true; }
      if (b) continue;
      return i;
    }
  return -1;
}
class GameApiItem;
struct CodeGenLine {
  std::string return_type;
  std::string label_num;
  std::string api_name;
  std::string func_name;
  std::vector<std::string> params;
  std::vector<std::string> params_linkage;
  GameApiItem *item;
};
void CodeGenLineErrorCheck(const CodeGenLine &line, std::vector<GameApiItem*> functions)
{
  int s = functions.size();
  bool found = false;
  for(int i=0;i<s;i++)
    {
      GameApiItem *item = functions[i];
      if (item->ApiName(0) == line.api_name && item->FuncName(0)==line.func_name)
	{
	  found = true;
	  if (item->ParamCount(0) != int(line.params.size()))
	    {
	      std::cout << "ERROR: Param arity problem" << std::endl;
	      std::cout << item->ApiName(0) << "::" << item->FuncName(0) << "(";
	      int ss = item->ParamCount(0);
	      for(int i=0;i<ss;i++)
		{
		  std::cout << item->ParamType(0,i);
		  if (i!=ss-1) std::cout << " ";
		}
	      std::cout << ");" << std::endl;
	      int ss2 = line.params.size();
	      std::cout << line.api_name << "::" << line.func_name << "(";
	      for(int j=0;j<ss2;j++)
		{
		  std::cout << line.params[j];
		  if (j!=ss2-1) std::cout << " ";
		}
	      std::cout << ");" << std::endl;
	    }
	}
    }
  if (!found) {
    std::cout << "ERROR: function not found! " << std::endl;
    int ss2 = line.params.size();
    std::cout << line.api_name << "::" << line.func_name << "(";
    for(int j=0;j<ss2;j++)
      {
	std::cout << line.params[j];
	if (j!=ss2-1) std::cout << " ";
      }
    std::cout << ");" << std::endl;
  }
}
template<class T>
std::ostream &operator<<(std::ostream &o, const std::vector<T> &v)
{
  int s = v.size();
  for(int i=0;i<s;i++)
    {
    o << v[i];
    if (i!=s-1) { o << ","; }
    }
  return o;
}

CodeGenLine parse_codegen_line(std::string line)
{
  CodeGenLine error = { "@", "@", "@", "@", { } };
  int first_space = find_char(line, 0, ' ');
  if (first_space==-1) { std::cout << "parse_codegen_line: first space error: " << line << std::endl; return error; }
  std::string return_type = line.substr(0,first_space);
  if (line[first_space+1]!='I') { std::cout << "parse_codegen_line: I error:"  << line << std::endl; return error; }
  int num_end_iterator = extend_until(line, first_space+2, "0123456789");
  if (num_end_iterator==-1) { std::cout << "parse_codegen_line: 012345689:"  << line<< std::endl; return error; }
  std::string num = line.substr(first_space+2, num_end_iterator-first_space-2);
  if (line[num_end_iterator]!='=') { std::cout << "parse_codegen_line: '=' error:" << line << std::endl; return error; }
  if (line[num_end_iterator+1]!='e') { std::cout << "parse_codegen_line: 'e' error:" << line << std::endl; return error; }
  if (line[num_end_iterator+2]!='v') { std::cout << "parse_codegen_line: 'v' error:" << line << std::endl; return error; }
  if (line[num_end_iterator+3]!='.') { std::cout << "parse_codegen_line: '.' error:"  << line<< std::endl; return error; }
  int end = find_char(line, num_end_iterator+4, '.');
  if (end==-1) { std::cout << "parse_codegen_line2: '.' error:"  << line<< std::endl; return error; }
  std::string api_name = line.substr(num_end_iterator+4, end-num_end_iterator-4);
  int end2 = find_char(line,end+1,'(',false);
  if (end2==-1) { std::cout << "parse_codegen_line2: '(' error:"  << line<< std::endl; return error; }
  std::string func_name = line.substr(end+1, end2-end-1);
  std::vector<std::string> params;
  while(1) {
    int end3 = find_one(line, end2+1, "),");
    if (end3==-1) { std::cout << "parse_codegen_line2: '),' error:"  << line<< std::endl; return error; }
    if (end3==end2+1 && line[end3]==')') { break; } // empty array
    params.push_back(line.substr(end2+1, end3-end2-1));
    end2 = end3;
    if (line[end3]==')') { break; }
  }
  CodeGenLine line2;
  line2.return_type = return_type;
  line2.label_num = "I" + num;
  line2.api_name = api_name;
  line2.func_name = func_name;
  line2.params = params;

  std::cout << "CodeGenLine: " << line2.api_name << " " << line2.func_name << " " << line2.params << std::endl;
  return line2;
}

std::string striphomepage(std::string url);

std::map<std::string, std::vector<unsigned char>*> load_url_buffers;
void onerror_cb(unsigned int tmp, void *arg, int, const char*)
{
  std::cout << "ERROR: onerror_cb" << std::endl; 
    char *url = (char*)arg;
    std::string url_str(url);
      std::string url_only(striphomepage(url_str));

    load_url_buffers[url_only] = (std::vector<unsigned char>*)-1;
    async_pending_count--;
    std::cout << "async_pending_count dec (onerror_cb) -->" << async_pending_count << std::endl;
}
std::string stripprefix(std::string s);
void ProgressBar(int num, int val, int max, std::string label);
void onprogress_cb(unsigned int tmp, void *, int, int) { }
void onload_cb(unsigned int tmp, void *arg, void *data, unsigned int datasize)
{
    std::vector<unsigned char> buffer;
    unsigned char *dataptr = (unsigned char*)data;
    int dt = datasize;
    for(int i=0;i<dt;i++) { buffer.push_back(dataptr[i]); }

    char *url = (char*)arg;
    std::string url_str(url);
    std::string url_only(striphomepage(url_str));
    std::cout << "url loading complete! " << url_only << std::endl;
    
    load_url_buffers[url_only] = new std::vector<unsigned char>(buffer);
    async_pending_count--;
    std::cout << "async_pending_count dec (onlosd_cb) -->" << async_pending_count << std::endl;
  { // progressbar
    std::string url_plain(stripprefix(url_only));
  int s = url_plain.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_plain[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url_plain);
  }

}
struct ASyncData
{
  std::string api_name;
  std::string func_name;
  int param_num;
};
ASyncData async_data[] = { 
  { "font_api", "newfont", 0 },
  { "font_api", "load_font", 0 },
  { "mainloop_api", "load_song", 2 },
  { "polygon_api", "p_url", 1 },
  { "polygon_api", "p_url_mtl", 1 },
  { "polygon_api", "p_mtl", 1 },
  { "polygon_api", "p_mtl", 2 },
  { "mainloop_api", "fps_display", 2 },
  { "mainloop_api", "score_display", 2 },
  { "mainloop_api", "time_display", 2 },
  { "mainloop_api", "load_P_script", 1 },
  { "mainloop_api", "load_P_script_array", 1 },
  { "mainloop_api", "load_ML_script", 1 },
  { "mainloop_api", "load_ML_script_array", 1 },
  { "mainloop_api", "load_BM_script", 1 },
  { "mainloop_api", "load_BM_script_array", 1 },
  { "polygon_api", "p_ds_url", 1 },
  { "bitmap_api", "intbitmap_loader", 0 },
  { "mainloop_api", "restart_screen", 2 },
  { "tracker_api", "play_wave_via_keypress", 2 },
  { "mainloop_api", "playback_keypresses", 1 },
  { "bitmap_api", "world_from_bitmap3", 2 },
  //{ "bitmap_api", "chai_bm", 0 }
  { "mainloop_api", "state_int_fetcher", 0 },
  { "mainloop_api", "state_speed_movement", 1 },
  // Note, this is function name, not user interface name.
  { "low_frame_api", "low_sprite_array", 1 }, 
  { "low_frame_api", "low_build_world", 1 },
  { "low_frame_api", "low_enemy_draw", 1 },
  { "low_frame_api", "low_enemy_draw2", 1 },
  { "polygon_api", "stl_load", 0 },
  { "font_api", "load_font_dump", 0 },
  { "polygon_api", "load_scene", 1 }
};

void LoadUrls_async(GameApi::Env &e, const CodeGenLine &line, std::string homepage)
{
  int s = sizeof(async_data)/sizeof(ASyncData);
  for(int i=0;i<s;i++)
    {
      ASyncData &dt = async_data[i];
      if (line.api_name == dt.api_name && line.func_name == dt.func_name)
	{
	  int param_num = dt.param_num;
	  std::string url = line.params[param_num];
	  e.async_load_url(url,homepage);
	}
    }
}			 
void InstallProgress(int num, std::string label, int max=15);

void LoadUrls(const CodeGenLine &line, std::string homepage)
{
  if (line.api_name!="bitmap_api" || line.func_name!="loadbitmapfromurl")
    return;
#ifdef EMSCRIPTEN
  
  std::string url = line.params[0];
  std::cout << "loading url: " << url << std::endl;

  // progress bar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url);


  std::string url2 = "load_url.php";
  std::string urlend = "url=" + url + "&homepage=" + homepage;
  url = "load_url.php?url=" + url + "&homepage=" + homepage;

    char *buf2 = new char[url2.size()+1];
    std::copy(url2.begin(), url2.end(), buf2);
    buf2[url2.size()]=0;

    char *buf3 = new char[url.size()+1];
    std::copy(url.begin(), url.end(), buf3);
    buf3[url.size()]=0;
    
    async_pending_count++;
    std::cout << "async_pending_count inc (LoadUrls) -->" << async_pending_count << std::endl;



    //emscripten_async_wget_data(buf2, (void*)buf2 , &onload_cb, &onerror_cb);
    emscripten_async_wget2_data(buf2, "POST", urlend.c_str(), (void*)buf3, 1, &onload_cb, &onerror_cb, &onprogress_cb);
#endif
}
std::vector<CodeGenLine> parse_codegen(GameApi::Env &env, GameApi::EveryApi &ev, std::string text, int &error_line_num)
{
  int idx = 0;
  int old_idx = 0;
  int line_num = 0;
  error_line_num = 0;
  std::vector<CodeGenLine> vec;
  std::vector<GameApiItem*> funcs = all_functions();
  std::string homepage = ev.mainloop_api.get_homepage_url();
  while((idx=find_char(text, idx, '\n'))!= -1)
    {
      std::string line = text.substr(old_idx, idx-old_idx-1);
      //std::cout << "Line:" << line << std::endl;
      if (line.size()<10) { 
	line_num++;
	old_idx = idx+1;
	idx++;
	continue; 
      }
      CodeGenLine l = parse_codegen_line(line);
      CodeGenLineErrorCheck(l, funcs);
      if (l.return_type=="@") {
	std::cout << "ERROR:" << line << std::endl;
	error_line_num = line_num; return std::vector<CodeGenLine>(); }
      LoadUrls(l, homepage);
      LoadUrls_async(env,l, homepage);
      vec.push_back(l);
      line_num++;
#if 1
      if (line_num > 1800) {
	// quick exit if line_num is larger than 400, for line number misuse.
	std::cout << "Number of lines in CodeGen is limited to 1800 lines" << std::endl;
	exit(0);
      }
#endif
      old_idx = idx+1;
      idx++;
    }
    return vec;
}

struct CodeGenVectors {
  std::vector<std::string> params;
};

void add_params_linkage(std::vector<CodeGenLine> &lines, std::vector<CodeGenVectors> &vectors, bool &error, std::map<std::string, int> env_map)
{
  error = false;
  // create line_map
  std::map<std::string, int> line_map;
  int s = lines.size();
  for(int i=0;i<s;i++)
    {
      CodeGenLine l = lines[i];
      line_map[l.label_num] = i;
      //std::cout << "Map: " << l.label_num << "=" << i << std::endl;
    }

  // iterate all parameters
  int s2 = lines.size();
  for(int i=0;i<s2;i++)
    {
      CodeGenLine l = lines[i];
      int ss = l.params.size();
      for(int j=0;j<ss;j++)
	{
	  std::string param_value = l.params[j];
	  std::string param_linkage = "";
	  // TODO std::vector types
	  if (param_value.size()>0 && param_value[0]=='I' && line_map.find(param_value)!=line_map.end())
	    {
	      int linkage = line_map[param_value];
	      std::stringstream ss;
	      ss << linkage;
	      param_linkage = ss.str();
	    }
	  if (param_value.size()>strlen("std::vector<"))
	    {
	      //std::cout << "Check Array" << param_value << std::endl;
	      std::string substr = param_value.substr(0,strlen("std::vector<"));
	      if (substr=="std::vector<")
		{ // array
		  //std::cout << "Is Array" << param_value << std::endl;
		  int pos = find_char(param_value, 13, '>');
		  if (pos==-1) { error = true; return; }
		  std::string type = param_value.substr(13,pos);
		  if (param_value[pos+1]!='{') { error=true; return; }
		  CodeGenVectors vec;
		  pos+=2;
		  while(1) {
		    int pos2 = find_one(param_value, pos, ",}");
		    if (pos2==pos+1) break;
		    if (pos2==-1) { std::cout << "Error=true" << std::endl; error=true; return; }
		    std::string param = param_value.substr(pos,pos2-pos);
		    //std::cout << "ArrParam: " << param << std::endl;

		    std::string param_linkage = "";
		    if (param.size()>0 && param[0]=='I')
		      {
			int linkage = line_map[param];
			std::stringstream ss;
			ss << linkage;
			param_linkage = ss.str();
		      } else if (param.size()>0 && param[0]=='E')
		      {
			int linkage = env_map[param];
			std::stringstream ss;
			ss << linkage;
			param_linkage = std::string("E") + ss.str();
		      } else {
		      param_linkage = param;
		    }
		    vec.params.push_back(param_linkage);
		    pos = pos2+1;
		    if (param_value[pos2]=='}') break;
		  }
		  vectors.push_back(vec);
		  std::stringstream ss;
		  ss << vectors.size()-1;
		  param_linkage = std::string("%") + ss.str();
		}
	    }
	  lines[i].params_linkage.push_back(param_linkage);
	}
    }
}

void link_api_items(std::vector<CodeGenLine> &vec, std::vector<GameApiItem*> functions)
{
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      CodeGenLine &line = vec[i];
      int ss = functions.size();
      bool found = false;
      for(int j=0;j<ss;j++)
	{
	  GameApiItem* item = functions[j];
	  //std::cout << "Compare: " << line.func_name << " " << item->FuncName(0) << std::endl;
	  //std::cout << "Compare2: " << line.api_name << " " << item->ApiName(0) << std::endl;
	  if (line.func_name == item->FuncName(0) &&
	      line.api_name == item->ApiName(0))
	    {
	      line.item = item;
	      found=true; break;
	    }
	}
      if (found==false) { std::cout << "NOT FOUND: " << line.api_name << " " << line.func_name<< std::endl; }
    }
}


bool is_num(std::string s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]<'0' || s[i]>'9') return false;
    }
  return true;
}

int execute_api(GameApi::Env &ee, GameApi::EveryApi &ev, const std::vector<CodeGenLine> &vec, std::vector<CodeGenVectors> &vecvec, int /*line_num*/, GameApi::ExecuteEnv &e)
{
  std::vector<std::string> res_vec;
  int s2 = vec.size();
  for(int ij=0;ij<s2;ij++)
    {
      CodeGenLine l = vec[ij];

      int s = l.params_linkage.size();
      std::vector<std::string> params = l.params;
      for(int i=0;i<s;i++)
	{
	  std::string link = l.params_linkage[i];
	  if (link.size()>0 && link[0]=='E')
	    {
	      std::string sub = link.substr(1);
	      std::stringstream ss(sub);
	      int idx = 0;
	      ss >> idx;
	      int val = e.env[idx];
	      std::stringstream ss2;
	      ss2 << val;
	      params[i] = ss2.str();
	    }
	  else if (link.size()>0 && link[0]=='%')
	    {
	      int j = i;
	      std::string sub = link.substr(1);
	      std::stringstream ss(sub);
	      int idx = 0;
	      ss >> idx;
	      int s = vecvec[idx].params.size();
	      std::vector<std::string> params2;
	      for(int i=0;i<s;i++)
		{
		  std::string link = vecvec[idx].params[i];
		  if (is_num(link)) {
		    std::stringstream ss(link);
		    int num;
		    ss >> num;
		    std::string val = res_vec[num]; 
		    params2.push_back( val );
		  } else {
		    params2.push_back(link);
		  }
		}
	      std::string res = "[";
	      for(int i=0;i<s;i++)
		{
		  if (i!=0) { res+=","; }
		  std::string p = params2[i];
		  res+=p;
		}
	      res+="]";
	      params[j] = res;
	    }
	  else if (link!="")
	    {
	      std::stringstream ss(link);
	      int num;
	      ss >> num;
	      std::string val = res_vec[num]; 
	      params[i] = val; 
	    }
    }
      
      //std::cout << "Execute: " << params << std::endl;
      int val = l.item->Execute(ee,ev, params, e);
      std::stringstream ss2;
      ss2 << val;
      res_vec.push_back(ss2.str());
    }
  std::string s = res_vec[res_vec.size()-1];
  std::stringstream ss(s);
  int val = -1;
  ss >> val;
  return val;
  
}


std::string ToString(int num)
{
  std::stringstream ss;
  ss << num;
  return ss.str();
}
std::pair<int,std::string> GameApi::execute_codegen(GameApi::Env &env, GameApi::EveryApi &ev, std::string text, GameApi::ExecuteEnv &e)
{
  int error_line_num = 0;
  std::vector<CodeGenLine> vec = parse_codegen(env, ev, text, error_line_num);
  if (vec.size()==0) {
    return std::make_pair(0,std::string("Error at line ") + ToString(error_line_num));
  }
  std::vector<CodeGenVectors> vecvec;
  std::map<std::string, int> envmap = e.envmap; 
  bool err2 = false;
  add_params_linkage(vec,vecvec,err2, envmap);
  if (err2) { return std::make_pair(0, std::string("Error at params_linkage")); }
  link_api_items(vec, all_functions());
  int val = execute_api(env, ev, vec, vecvec, vec.size()-1, e);
  CodeGenLine last = vec[vec.size()-1];
  if (last.return_type=="BLK") return std::make_pair(val,"OK");
  return std::make_pair(val,last.return_type);
  //if (last.return_type=="RUN") {
  //  return std::make_pair(val,"RUN");
  //}

  // return std::make_pair(val, "OK");
}

template<class T>
class CodeGenBuilderValue : public BuilderValue<T>
{
public:
  CodeGenBuilderValue(GameApi::Env &env, GameApi::EveryApi &ev, std::string text, GameApi::ExecuteEnv &e) : env(env), ev(ev), text(text),e(e) { }
  T get() const
  {
    std::pair<int, std::string> p = execute_codegen(env, ev, text, e);
    T t;
    t.id = p.first;
    return t;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string text;
  GameApi::ExecuteEnv &e;
};
#endif // SECTION_1
#endif
std::pair<std::string,std::string> CodeGen_1(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, std::vector<std::string> param_type, std::string return_type, std::string api_name, std::string func_name);


#ifdef FIRST_PART
#ifdef SECTION_2
std::pair<std::string,std::string> CodeGen_1(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, std::vector<std::string> param_type, std::string return_type, std::string api_name, std::string func_name)
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
#endif
#endif
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
  std::string ApiName(int i) const { return api_name; }
  std::string FuncName(int i) const { return func_name; }
  std::string Symbols() const { return symbols; }
  std::string Comment() const { return comment; }
  int Execute(GameApi::Env &ee, GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e)
  {
    if (params.size()!=param_name.size()) {
      std::cout << "Error: param vectors different size: " << ApiName(0) << "::" << FuncName(0) << std::endl;
    }
    return funccall(ee, ev, api, fptr, params, e, param_name, return_type); 
  }
#if 0
  std::vector<GameApi::EditNode*> CollectNodes(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)
  {
    return collectnodes(name,param_type, param_names, params, param_name);
  }
#endif
  std::pair<std::string,std::string> CodeGen(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names)
  {
    std::pair<std::string,std::string> p = CodeGen_1(ev,params, param_names, param_type, return_type,api_name,func_name);
    return p;
#if 0
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
#endif
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

std::vector<GameApiItem*> append_vectors(std::vector<GameApiItem*> vec1, std::vector<GameApiItem*> vec2);

#ifdef FIRST_PART
#ifdef SECTION_2
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
#if 1
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
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::to_bitmap,
			 "tx_to_bitmap",
			 { "txid" },
			 { "TXID" },
			 { "" },
			 "BM", "texture_api", "to_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::fbo_api, &GameApi::FrameBufferApi::fbo_ml,
			 "fbo_id",
			 { "ev", "mainloop", "sx", "sy", "translate" },
			 { "EveryApi&", "ML", "int", "int", "bool" },
			 { "ev", "", "-1", "-1", "false" },
			 "TXID", "fbo_api", "fbo_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::position_based_on_screen,
			 "fbo_pos",
			 { "obj" },
			 { "ML" },
			 { "" },
			 "ML", "polygon_api", "position_based_on_screen"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::fbo_api, &GameApi::FrameBufferApi::fbo_ml_blit,
			 "fbo_ml",
			 { "ev", "txid", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "TXID", "float", "float", "float", "float", "float" },
			 { "ev", "", "0.0", "800", "0.0", "600", "0.0" },
			 "ML", "fbo_api", "fbo_ml_blit"));

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
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::mandelbrot_volume,
			 "o_mandelbrot",
			 { "julia", "count", "yy" },
			 { "bool", "int", "float" },
			 { "false", "64", "0.0" },
			 "O", "volume_api", "mandelbrot_volume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::mandelbulb,
			 "o_mandelbulb",
			 { "n", "p_x", "p_y", "p_z", "c_x", "c_y", "c_z", "radius", "iterations" },
			 { "float", "float", "float", "float", "float", "float", "float", "float", "int" },
			 { "2.5,", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "100.0", "64" },
			 "O", "volume_api", "mandelbulb"));
			   
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::instancing_volume,
			 "o_inst",
			 { "volume", "pts" },
			 { "O", "PTS" },
			 { "", "" },
			 "O", "volume_api", "instancing_volume"));
#if 0
  // keeps crashing
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts2",
			 { "object", "size", "wholesize" },
			 { "O", "int", "float" },
			 { "", "30", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));
#endif
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::waveform_sphere,
			 "wv_sphere",
			 { "wave", "r" },
			 { "WV", "float" },
			 { "", "200.0" },
			 "FO", "float_volume_api", "waveform_sphere"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::min_distance,
			 "fo_min_distance",
			 { "curve" },
			 { "C" },
			 { "" },
			 "FO", "float_volume_api", "min_distance"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::subvolume,
			 "fo_subvolume",
			 { "f", "start_range", "end_range" },
			 { "FO", "float", "float" },
			 { "", "0.5", "1.0" },
			 "O", "float_volume_api", "subvolume"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::interpolate,
			 "fo_interpolate",
			 { "f1", "f2", "val" },
			 { "FO", "FO", "float" },
			 { "", "", "0.5" },
			 "FO", "float_volume_api", "interpolate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::smooth,
			 "fo_smooth",
			 { "vec", "val" },
			 { "[FO]", "float" },
			 { "", "0.01" },
			 "FO", "float_volume_api", "smooth"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vector_volume_api, &GameApi::VectorVolumeApi::normal2,
			 "fo_normal",
			 { "fo", "stepsize" },
			 { "FO", "float" },
			 { "", "0.1" },
			 "VO", "vector_volume_api", "normal2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::integrate_render,
			 "fo_itg_render",
			 { "obj", "sx", "sy", "numsamples" },
			 { "FO", "int", "int", "int" },
			 { "", "800", "600", "3" },
			 "FB", "float_volume_api", "integrate_render"));
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::player,
			 "player",
			 { "prev" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "player"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enemy,
			 "enemy",
			 { "prev" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "enemy"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::player_pos,
			 "player_pos",
			 { "prev", "pos" },
			 { "ML", "PT" },
			 { "", "" },
			 "ML", "move_api", "player_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enemy_pos,
			 "enemy_pos",
			 { "prev", "pos" },
			 { "ML", "PTS" },
			 { "", "" },
			 "ML", "move_api", "enemy_pos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::to_polygon,
			 "plf_poly",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "P", "newplane_api", "to_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::reverse_faces,
			 "plf_reversefaces",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "PLF", "newplane_api", "reverse_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::newplane_api, &GameApi::NewPlaneApi::triangulate,
			 "triangulate2d",
			 { "faces" },
			 { "PLF" },
			 { "" },
			 "PLF", "newplane_api", "triangulate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::empty,
			 "phy_empty",
			 { },
			 { },
			 { },
			 "PH", "physics_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::anchor_point2,
			 "phy_anchor",
			 { "phy", "pos" },
			 { "PH", "PT" },
			 { "", "" },
			 "PH", "physics_api", "anchor_point2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::ext_force,
			 "phy_force",
			 { "phy", "point", "dir" },
			 { "PH", "int", "V" },
			 { "", "0", "" },
			 "PH", "physics_api", "ext_force"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::ext_force_all,
			 "phy_force_all",
			 { "phy", "dir" },
			 { "PH", "V" },
			 { "", "" },
			 "PH", "physics_api", "ext_force_all"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::anchor_link,
			 "phy_link",
			 { "phy", "p1", "p2", "dist" },
			 { "PH", "int", "int", "float" },
			 { "", "0", "1", "100" },
			 "PH", "physics_api", "anchor_link"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::force_obj,
			 "phy_object",
			 { "phy", "obj", "dir" },
			 { "PH", "O", "V" },
			 { "", "", "" },
			 "PH", "physics_api", "force_obj"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::phy_from_p,
			 "phy_p",
			 { "p" },
			 { "P" },
			 { "" },
			 "PH", "physics_api", "phy_from_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::physics_api, &GameApi::PhysicsApi::physics_action,
			 "phy_points",
			 { "ev", "phy" },
			 { "EveryApi&", "PH" },
			 { "ev", "" },
			 "PTS", "physics_api", "physics_action"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::identity,
			 "dc_identity",
			 { },
			 { },
			 { },
			 "DC", "move_api", "identity"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::wave,
			 "dc_wave",
			 { "r", "speed1", "speed2", "dist1", "dist2", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "int", "int" },
			 { "30.0", "1.0", "1.0", "30.0", "30.0", "30", "30" },
			 "DC", "move_api", "wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::sphwave,
			 "dc_sphwave",
			 { "r1", "fr_1", "t_1", "r2", "fr_2", "t_2" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "30.0", "3.0", "1.0", "30.0", "5.0", "2.0" },
			 "DC", "move_api", "sphwave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::explosion,
			 "dc_explosion",
			 { "center_x", "center_y", "center_z", "start_val", "end_val", "start_time", "end_time" },
			 { "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "1.0", "10.0", "0.0", "30.0" },
			 "DC", "move_api", "explosion"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::split,
			 "dc_split",
			 { "d1", "d2", "val" },
			 { "DC", "DC", "float" },
			 { "", "", "0.5" },
			 "DC", "move_api", "split"));
  
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::default_cmds,
			 "cmd_def",
			 { "dx", "dy", "dz" },
			 { "float", "float", "float" },
			 { "10.0", "10.0", "10.0" },
			 "CMD", "move_api", "default_cmds"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_repeat,
			 "cmd_repeat",
			 { "cmds", "repeat", "dx", "dy", "dz" },
			 { "CMD", "std::string", "float", "float", "float" },
			 { "", ".dd.", "10.0", "10.0", "10.0" },
			 "CMD", "move_api", "cmd_repeat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_rotate,
			 "cmd_rotate",
			 { "cmds", "v_x", "v_y", "v_z", "angle", "delta_angle" },
			 { "CMD", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "1.0", "0.0", "1.57" },
			 "CMD", "move_api", "cmd_rotate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_pts,
			 "cmd_to_pts",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "PTS", "move_api", "cmd_to_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_li,
			 "cmd_to_li",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "LI", "move_api", "cmd_to_li"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::cmd_to_plf,
			 "cmd_to_plf",
			 { "cmds", "commands" },
			 { "CMD", "std::string" },
			 { "", "." },
			 "PLF", "move_api", "cmd_to_plf"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::empty_voxel,
			 "vx_empty",
			 { "sx", "sy", "sz" },
			 { "int", "int", "int" },
			 { "30", "30", "30" },
			 "VX", "voxel_api", "empty_voxel"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::subvoxel,
			 "vx_sub",
			 { "voxel", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "VX", "int", "int", "int", "int", "int", "int" },
			 { "", "0", "100", "0", "100", "0", "100" },
			 "VX", "voxel_api", "subvoxel"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_landscape_from_fbm,
			 "vx_landscape_bm",
			 { "bitmap", "height", "false_val", "true_val" },
			 { "FB", "int", "int", "int" },
			 { "", "30", "-1", "0" },
			 "VX", "voxel_api", "voxel_landscape_from_fbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::blit_voxel2,
			 "vx_or_elem",
			 { "v1", "v2", "p_x", "p_y", "p_z" },
			 { "VX", "VX", "int", "int", "int" },
			 { "", "", "0", "0", "0" },
			 "VX", "voxel_api", "blit_voxel2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::blit_voxel,
			 "vx_blit",
			 { "object", "sx","sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "false_value", "true_value" },
			 { "O", "int", "int", "int", "float","float", "float", "float", "float", "float", "int", "int" },
			 { "", "30", "30", "30", "-200.0", "200.0", "-200.0", "200.0", "-200.0", "200.0", "-1", "0" },
			 "VX", "voxel_api", "blit_voxel"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_instancing,
			 "vx_inst",
			 { "voxel", "count", "start_x", "end_x", "start_y","end_y", "start_z", "end_z" },
			 { "VX", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "1", "-200.0","200.0", "-200.0", "200.0", "-200.0", "200.0" },
			 "[PTS]", "voxel_api", "voxel_instancing"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_render,
			 "vx_render",
			 { "ev", "objs", "ptss" },
			 { "EveryApi&", "[P]", "[PTS]" },
			 { "ev", "", "" },
			  "ML", "voxel_api", "voxel_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_bind,
			 "vx_bind",
			 { "ev", "objs", "ptss", "mt" },
			 { "EveryApi&", "[P]", "[PTS]", "MT" },
			 { "ev", "", "", "" },
			 "ML", "voxel_api", "voxel_bind"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::voxel_static,
			 "vx_static",
			 { "ev", "objs", "ptss" },
			 { "EveryApi&", "[P]", "[PTS]" },
			 { "ev", "", "" },
			 "P", "voxel_api", "voxel_static"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::convert_p_to_vx,
			 "vx_p",
			 { "p", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "value" },
			 { "P", "int", "int", "int", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "0" },
			 "VX", "voxel_api", "convert_p_to_vx"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::voxel_api, &GameApi::VoxelApi::from_implicit,
			 "vx_im",
			 { "im", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "value" },
			 { "IM", "int", "int", "int", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "0" },
			 "VX", "voxel_api", "from_implicit"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::pts_to_voxel,
			 "av_from_pts",
			 { "pts", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy", "sz" },
			 { "PTS", "float", "float", "float", "float", "float", "float", "int", "int", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "30", "30", "30" },
			 "AV", "polygon_api", "pts_to_voxel"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::voxelarray_to_pts,
			 "av_to_pts",
			 { "att", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "AV", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "polygon_api", "voxelarray_to_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::av_unique,
			 "av_unique",
			 { "arr" },
			 { "AV" },
			 { "" },
			 "AV", "polygon_api", "av_unique"));
  return vec;
}

std::vector<GameApiItem*> fontapi_functions()
{
  std::vector<GameApiItem*> vec;
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::newfont,
			 "newfont",
			 { "file", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/FreeSans.ttf", "20", "20" },
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_string,
			 "dyn_string",
			 { "ev", "font", "alternative_chars", "fetcher", "x", "y", "numchars" },
			 { "EveryApi&", "Ft", "std::string", "SF", "int", "int", "int" },
			 { "ev", "", "0123456789:", "", "0", "0", "5" },
			 "ML", "font_api", "dynamic_string"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_array,
			 "fnt_chars",
			 { "font", "string", "x_gap" },
			 { "Ft", "std::string", "int" },
			 { "", "0123456789", "2" },
			 "ARR", "font_api", "font_string_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_array2,
			 "fnt_chars2",
			 { "font", "string" },
			 { "FI", "std::string" },
			 { "", "0123456789" },
			 "[BM]", "font_api", "font_string_array2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_string_fetcher,
			 "fnt_time",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "SF", "font_api", "time_string_fetcher"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::score_string_fetcher,
			 "fnt_score",
			 { "id", "label", "numdigits" },
			 { "std::string", "std::string", "int" },
			 { "score", "Score: ", "5" },
			 "SF", "font_api", "score_string_fetcher"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_adder,
			 "score_adder",
			 { "ev", "ml", "o", "mn", "enter_score", "leave_score", "dyn_point", "timeout" },
			 { "EveryApi&", "ML", "O", "MN", "int", "int", "int", "float" },
			 { "ev", "", "", "", "1", "0", "-1", "5.0" },
			 "ML", "mainloop_api", "score_adder"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_adder2,
			 "score_adder2",
			 { "ev", "ml", "o", "points", "mn", "enter_score", "leave_score", "dyn_point", "timeout", "fetcher" },
			 { "EveryApi&", "ML", "O", "PTS", "MN", "int", "int", "int", "float", "IF" },
			 { "ev", "", "", "", "", "1", "0", "-1", "5.0", "" },
			 "ML", "mainloop_api", "score_adder2"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_hidder,
			 "score_hidder",
			 { "ev", "ml", "o","max_count" },
			 { "EveryApi&", "ML", "O", "int" },
			 { "ev", "", "", "50" },
			 "ML", "mainloop_api", "score_hidder"));

  
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_font,
			 "FI_load",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/Chunkfive.otf", "200", "200" },
			 "FI", "font_api", "load_font"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::draw_text_string,
			 "FI_drawtext",
			 { "font", "str", "x_gap", "line_height" },
			 { "FI", "std::string", "int", "int" },
			 { "", "Hello", "5", "30" },
			 "BM", "font_api", "draw_text_string"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::save_font_dump,
			 "FI_save_dump",
			 { "font", "chars", "filename" },
			 { "FI", "std::string", "std::string" },
			 { "", "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!\"#¤%&/()=?+\\*^.,-<>|§½;:[]_ ", "font.txt" },
			 "ML", "font_api", "save_font_dump"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_font_dump,
			 "FI_load_dump",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/font.txt" },
			 "FI", "font_api", "load_font_dump"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::bm_array_id,
			 "bm_array_id",
			 { "vec" },
			 { "[BM]" },
			 { "" },
			 "[BM]", "font_api", "bm_array_id"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_character,
			 "bm_chooser",
			 { "ev", "vec", "fetcher", "x", "y" },
			 { "EveryApi&", "[BM]", "IF", "int", "int" },
			 { "ev", "", "", "0","0" },
			 "ML", "font_api", "dynamic_character"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_polygon,
			 "p_chooser",
			 { "ev", "vec", "mat", "fetcher" },
			 { "EveryApi&", "[P]", "MT", "IF" },
			 { "ev", "", "", "" },
			 "ML", "font_api", "dynamic_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::ml_chooser,
			 "ml_chooser",
			 { "vec", "fetcher" },
			 { "[ML]", "IF" },
			 { "", "" },
			 "ML", "font_api", "ml_chooser"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::toggle_button_fetcher,
			 "if_toggle",
			 { "start_x", "end_x", "start_y", "end_y" },
			 { "float", "float", "float", "float" },
			 { "0.0", "800.0", "0.0", "600.0" },
			 "IF", "font_api", "toggle_button_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::timed_int_fetcher,
			 "if_timed",
			 { "ev", "start", "end", "start_time", "end_time" },
			 { "EveryApi&", "int", "int", "float", "float" },
			 { "ev", "0", "10", "0.0", "30.0" },
			 "IF", "font_api", "timed_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::movement_int_fetcher,
			 "if_move",
			 { "count", "x_mult", "y_mult", "z_mult" },
			 { "int", "float", "float", "float" },
			 { "10", "0.1", "0.1", "0.1" },
			 "IF", "font_api", "movement_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::repeat_int_fetcher,
			 "if_repeat",
			 { "fetcher", "duration" },
			 { "IF", "float" },
			 { "", "30.0" },
			 "IF", "font_api", "repeat_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::keypress_int_fetcher,
			 "if_keypress",
			 { "key", "key_down_value", "key_up_value" },
			 { "int", "int", "int" },
			 { "32", "1", "0" },
			 "IF", "font_api", "keypress_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::state_int_fetcher,
			 "if_statemachine",
			 { "url", "states" },
			 { "std::string", "std::string" },
			 { "http://tpgames.org/move.sm", "s0&s1&s2&s3&s4" },
			 "IF", "mainloop_api", "state_int_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::hmd_state_fetcher,
			 "if_hmd",
			 { },
			 { },
			 { },
			 "IF", "font_api", "hmd_state_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::choose_float_fetcher,
			 "ff_choose",
			 { "int_fetcher", "a_0", "a_1", "a_2", "a_3", "a_4", "a_5", "a_6" },
			 { "IF", "float", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0" },
			 "FF", "font_api", "choose_float_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::time_fetcher2,
			 "ff_time",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "FF", "font_api", "time_fetcher2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::point_fetcher_constant,
			 "pf_constant",
			 { "x", "y", "z" },
			 { "float", "float", "float" },
			 { "0.0", "0.0", "0.0" },
			 "PF", "font_api", "point_fetcher_constant"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::mouse_fetcher,
			 "pf_mouse",
			 { },
			 { },
			 { },
			 "PF", "font_api", "mouse_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::point_fetcher_part,
			 "pf_component",
			 { "point_fetcher", "component", "float_fetcher" },
			 { "PF", "int", "FF" },
			 { "", "0", "" },
			 "PF", "font_api", "point_fetcher_part"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::fps_fetcher,
			 "fnt_fps",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "FF", "font_api", "fps_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::float_to_string_fetcher,
			 "fnt_float_to_string",
			 { "fetcher" },
			 { "FF" },
			 { "" },
			 "SF", "font_api", "float_to_string_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::char_fetcher_from_string,
			 "fnt_char_idx",
			 { "string_fetcher", "alternatives", "idx" },
			 { "SF", "std::string", "int" },
			 { "", "0123456789", "0" },
			 "IF", "font_api", "char_fetcher_from_string"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::x_comp,
			 "pf_x",
			 { "point_fetcher", "start_x", "end_x", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "x_comp"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::y_comp,
			 "pf_y",
			 { "point_fetcher", "start_y", "end_y", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "y_comp"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::z_comp,
			 "pf_z",
			 { "point_fetcher", "start_z", "end_z", "numsteps" },
			 { "PF", "float", "float", "int" },
			 { "", "0.0", "300.0", "8" },
			 "IF", "font_api", "z_comp"));

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
  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_p,
			 "tree_p",
			 { "ev", "tree", "vec", "time" },
			 { "EveryApi&", "T", "[P]", "float" },
			 { "ev", "", "", "0.0" },
			 "P", "tree_api", "tree_p"));
  
  return vec;
}
#endif // SECTION_3
#endif


#ifdef SECOND_PART
#ifdef SECTION_1
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
#ifdef VIRTUAL_REALITY
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::pose,
			 "mn_hmd_pose",
			 { "next", "pose_in_screen" },
			 { "MN", "bool" },
			 { "", "false" },
			 "MN", "move_api", "pose"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::state_speed_movement,
			 "mn_statemachine",
			 { "mn", "url", "states", "x_speeds", "y_speeds", "z_speeds" },
			 { "MN", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "", "http://tpgames.org/move.sm", "s0&s1&s2&s3&s4", "0.0&-1.0&0.0&0.0&1.0", "0.0&0.0&-1.0&1.0&0.0", "0.0&0.0&0.0&0.0&0.0" },
			 "MN", "mainloop_api", "state_speed_movement"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale_progress,
			 "mn_scale_progress",
			 { "next", "is_x", "is_y", "is_z" },
			 { "MN", "bool", "bool", "bool" },
			 { "", "true", "false", "false" },
			 "MN", "move_api", "scale_progress"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::mn_fetcher,
			 "mn_pos_fetcher",
			 { "point_fetcher" },
			 { "PF" },
			 { "" },
			 "MN", "move_api", "mn_fetcher"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::local_move,
			 "local_move",
			 { "ev", "inner_ml", "center_points" },
			 { "EveryApi&", "ML", "PTS" },
			 { "ev", "", "" },
			 "ML", "move_api", "local_move"));
  
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
			     "6.28318" },
			 "MN", "move_api", "rotate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::translate_wave,
			 "wave_translate",
			 { "next", "start_time", "end_time", "wave", "dx", "dy", "dz" },
			 { "MN", "float", "float", "WV", "float", "float", "float" },
			 { "", "0.0", "100.0", "", "0.0", "0.0", "0.0" },
			 "MN", "move_api", "translate_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::scale_wave,
			 "wave_scale",
			 { "next", "start_time", "end_time", "wave", "sx", "sy", "sz" },
			 { "MN", "float", "float", "WV", "float", "float", "float" },
			 { "", "0.0", "100.0", "", "1.0", "1.0", "1.0" },
			 "MN", "move_api", "scale_wave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rotate_wave,
			 "wave_rotate",
			 { "next", "start_time", "end_time", "wave", "p_x", "p_y", "p_z",
			     "v_x", "v_y", "v_z", "angle" },
			 { "MN", "float", "float", "WV",
			     "float", "float","float",
			     "float", "float","float",
			     "float" },
			 { "", "0.0", "100.0", "",
			     "0.0", "0.0", "0.0",
			     "0.0", "1.0", "0.0",
			     "6.28318" },
			 "MN", "move_api", "rotate_wave"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_enable,
			 "anim_enable",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_enable"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_disable,
			 "anim_disable",
			 { "next", "start_time", "end_time" },
			 { "MN", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_disable"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::anim_choose,
			 "anim_choose",
			 { "vec", "start_time", "duration" },
			 { "[MN]", "float", "float" },
			 { "", "0.0", "100.0" },
			 "MN", "move_api", "anim_choose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::event_activate,
			 "anim_event",
			 { "next", "event", "event_time", "event_duration" },
			 { "MN", "MN", "float", "float" },
			 { "", "", "10.0", "100.0" },
			 "MN", "move_api", "event_activate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_start,
			 "color_start",
			 { "color" },
			 { "unsigned int" },
			 { "ffffffff" },
			 "CC", "move_api", "color_start"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_interpolate,
			 "color_interpolate",
			 { "next", "start_color", "end_color", "start_time", "end_time" },
			 { "CC", "unsigned int", "unsigned int", "float", "float" },
			 { "", "ffffffff", "ff888888", "0.0", "100.0" },
			 "CC", "move_api", "color_interpolate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::cursor_keys_normal_move,
			 "move_normal",
			 { },
			 { },
			 { },
			 "INP", "mainloop_api", "cursor_keys_normal_move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::cursor_keys_rotate_move,
			 "move_rotate",
			 { "speed_rot", "r_forward", "r_normal", "r_backward" },
			 { "float", "float", "float", "float" },
			 { "0.3141", "1.0", "0.0", "-1.0" },
			 "INP", "mainloop_api", "cursor_keys_rotate_move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_speed,
			 "move_speed",
			 { "orig", "speed" },
			 { "INP", "float" },
			 { "", "1.0" },
			 "INP", "mainloop_api", "move_speed"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_collision,
			 "move_coll",
			 { "scene", "start_x", "end_x", "start_y", "end_y", "s_x", "m_x", "e_x", "s_y", "m_y", "e_y", "speed_up", "speed_down", "speed_left", "speed_right", "speed_gravity" },
			 { "IBM", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "","-200.0", "200.0", "-200.0", "200.0", "-5.0", "0.0", "5.0", "5.0", "0.0", "-5.0", "20.0", "-10.0", "-10.0", "10.0", "-10.0" },
			 "INP", "mainloop_api", "move_collision"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_sequence,
			 "move_sequence",
			 { "start_time", "time_per_char", "string" },
			 { "float", "float", "std::string" },
			 { "0.0", "3.0", "wwddssaa" },
			 "INP", "mainloop_api", "move_sequence"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::gravity,
			 "move_gravity",
			 { "inp", "scene", "start_x", "end_x", "start_y", "end_y","speed" },
			 { "INP", "IBM", "float", "float", "float", "float","float" },
			 { "", "", "0.0", "800.0", "0.0", "600.0", "10.0" },
			 "INP", "mainloop_api", "gravity"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::root,
			 "sa_root",
			 { "points" },
			 { "PT" },
			 { "" },
			 "SA", "skeletal_api", "root"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::node,
			 "sa_node",
			 { "parent", "matrix", "point_offset" },
			 { "SA", "MN", "PT" },
			 { "", "", "" },
			 "SA", "skeletal_api", "node"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_x_ml,
			 "move_x_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed", "start_x", "end_x" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "5.0","-100.0", "100.0" },
			 "ML", "move_api", "move_x_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_y_ml,
			 "move_y_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_y", "end_y" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "119", "115", "5.0","-100.0","100.0" },
			 "ML", "move_api", "move_y_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_z_ml,
			 "move_z_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_z", "end_z" },
			 { "EveryApi&", "ML", "int", "int", "float","float","float" },
			 { "ev", "", "107", "109", "5.0","-100.0","100.0" },
			 "ML", "move_api", "move_z_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::jump_ml,
			 "jump_ml",
			 { "ev", "ml", "key_jump", "height", "jump_duration" },
			 { "EveryApi&", "ML", "int", "float", "float" },
			 { "ev", "", "32", "300.0", "10.0" },
			 "ML", "move_api", "jump_ml"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::def,
			 "m_def",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "MT", "materials_api", "def"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::colour_material,
			 "m_colour",
			 { "ev", "mix" },
			 { "EveryApi&", "float" },
			 { "ev", "0.5" },
			 "MT", "materials_api", "colour_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture,
			 "m_texture",
			 { "ev", "bm", "mix" },
			 { "EveryApi&", "BM","float" },
			 { "ev", "","1.0" },
			 "MT", "materials_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::textureid,
			 "m_texture_id",
			 { "ev", "txid", "mix" },
			 { "EveryApi&", "TXID", "float" },
			 { "ev", "", "1.0" },
			 "MT", "materials_api", "textureid"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_many,
			 "m_texture_many",
			 { "ev", "vec", "mix" },
			 { "EveryApi&", "[BM]", "float" },
			 { "ev", "", "1.0" },
			 "MT", "materials_api", "texture_many"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_many2,
			 "m_texture_many_p",
			 { "ev", "mix" },
			 { "EveryApi&", "float" },
			 { "ev", "0.5" },
			 "MT", "materials_api", "texture_many2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::many_texture_id_material,
			 "m_mtl_many",
			 { "ev", "mtl_url", "url_prefix", "mix", "start_range", "end_range" },
			 { "EveryApi&", "std::string", "std::string", "float", "int", "int" },
			 { "ev", "http://tpgames.org/sponza/sponza.mtl", "http://tpgames.org/sponza", "1", "0", "15" },
			 "MT", "materials_api", "many_texture_id_material"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_cubemap,
			 "m_texture_cubemap",
			 { "ev", "vec", "mix", "mix2" },
			 { "EveryApi&", "[BM]", "float","float" },
			 { "ev", "", "1.0", "1.0" },
			 "MT", "materials_api", "texture_cubemap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::texture_arr,
			 "m_texture_arr",
			 { "ev", "vec", "sx", "sy", "mix" },
			 { "EveryApi&", "[BM]", "int", "int", "float" },
			 { "ev", "", "256", "256", "1.0" },
			 "MT", "materials_api", "texture_arr"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::skeletal,
			 "m_skeletal",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "MT", "materials_api", "skeletal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shading1,
			 "m_shading1",
			 { "ev", "nxt", "mix_val1", "mix_val2" },
			 { "EveryApi&", "MT", "float", "float" },
			 { "ev", "", "0.95", "0.5" },
			 "MT", "materials_api", "shading1"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shading2,
			 "m_shading2",
			 { "ev", "nxt", "color1", "color2", "color3" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int" },
			 { "ev", "", "ffaaaaaa", "ffeeeeee", "ffffffff" },
			 "MT", "materials_api", "shading2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::phong,
			 "m_phong",
			 { "ev", "nxt", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow" },
			 { "EveryApi&", "MT", "float", "float", "float", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "-0.3", "0.3", "-1.0", "ffff8800", "ff666666", "5.0" },
			 "MT", "materials_api", "phong"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bump_phong,
			 "m_bump_phong",
			 { "ev", "light_dir_x", "light_dir_y", "light_dir_z", "ambient", "highlight", "pow", "fb", "bump_width" },
			 { "EveryApi&", "float", "float", "float", "unsigned int", "unsigned int", "float", "FB", "float" },
			 { "ev", "-0.3", "0.3", "-1.0", "ffff8800", "ffffffff", "10.0", "", "5.0" },
			 "MT", "materials_api", "bump_phong"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::fog,
			 "m_fog",
			 { "ev", "nxt", "dist", "dark_color", "light_color" },
			 { "EveryApi&", "MT", "float", "unsigned int", "unsigned int" },
			 { "ev", "", "300.0", "ff000000", "ffffffff" },
			 "MT", "materials_api", "fog"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::gi,
			 "m_gi",
			 { "ev", "nxt", "pts", "obj_size" },
			 { "EveryApi&", "MT", "PTS", "float" },
			 { "ev", "", "", "100.0" },
			 "MT", "materials_api", "gi"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::combine_materials,
			 "m_or_elem",
			 { "ev", "mat1", "mat2" },
			 { "EveryApi&", "MT", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "combine_materials"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shadow,
			 "m_shadow",
			 { "ev", "p", "vec", "p_x", "p_y", "p_z", "sx", "sy", "dark_color", "mix", "mix2" },
			 { "EveryApi&", "P", "[BM]", "float", "float", "float", "int", "int", "unsigned int", "float", "float" },
			 { "ev", "", "", "0.0", "0.0", "0.0", "512", "512", "ff000000", "1.0", "0.5" },
			 "MT", "materials_api", "shadow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::shadow2,
			 "m_shadow2",
			 { "ev", "p", "p_x", "p_y", "p_z", "sx", "sy", "dark_color", "mix", "mix2", "numtextures" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int", "unsigned int", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "512", "512", "ff000000", "1.0", "0.5", "0" },
			 "MT", "materials_api", "shadow2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::dyn_lights,
			 "m_dyn_lights",
			 { "ev", "nxt", "light_pos_x", "light_pos_y", "light_pos_z", "dist", "dyn_point" },
			 { "EveryApi&", "MT", "float", "float", "float", "float", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "500.0", "-1" },
			 "MT", "materials_api", "dyn_lights"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::snow,
			 "m_snow",
			 { "ev", "nxt", "color1", "color2", "color3", "mix_val" },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int", "float" },
			 { "ev", "", "ffaaaaaa", "ffeeeeee", "ffffffff", "0.95" },
			 "MT", "materials_api", "snow"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::flat,
			 "m_flat",
			 { "ev", "nxt", "color1", "color2", "color3"  },
			 { "EveryApi&", "MT", "unsigned int", "unsigned int", "unsigned int" },
			 { "ev", "", "ff8888ff", "ffff4422", "ffffffff" },
			 "MT", "materials_api", "flat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::fur,
			 "m_fur",
			 { "ev", "nxt", "center", "dist", "max_angle", "count", "size", "cone_numfaces" },
			 { "EveryApi&", "MT", "PT", "float", "float", "int", "float", "int" },
			 { "ev", "", "", "60.0", "1.59", "1500", "2.0", "4" },
			 "MT", "materials_api", "fur"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::noise,
			 "m_noise",
			 { "ev", "sx", "sy", "r", "g", "b", "a", "r2", "g2", "b2", "a2", "mix" },
			 { "EveryApi&", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "float" },
			 { "ev", "1024", "1024", "255", "255", "255", "255", "0", "0", "0", "255", "0.5" },
			 "MT", "materials_api", "noise"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bloom,
			 "m_bloom",
			 { "ev", "nxt", "bm","r_cut", "g_cut", "b_cut", "pixel_x", "pixel_y" },
			 { "EveryApi&", "MT", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "0.7", "0.7", "0.7", "0.01", "0.01" },
			 "MT", "materials_api", "bloom"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::choose_color,
			 "m_choose_color",
			 { "ev", "nxt", "color", "mix_val" },
			 { "EveryApi&", "MT", "unsigned int", "float" },
			 { "ev", "", "ffff8844", "0.5" },
			 "MT", "materials_api", "choose_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::brashmetal,
			 "m_brashmetal",
			 { "ev", "nxt", "count", "web" },
			 { "EveryApi&", "MT", "int", "bool" },
			 { "ev", "", "80000", "true" },
			 "MT", "materials_api", "brashmetal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::marble,
			 "m_marble",
			 { "ev", "nxt", "count", "cubesize" },
			 { "EveryApi&", "MT", "int", "float" },
			 { "ev", "", "300", "10" },
			 "MT", "materials_api", "marble"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::web,
			 "m_web",
			 { "ev", "nxt", "mult", "linewidth", "linecolor" },
			 { "EveryApi&", "MT", "float", "float","unsigned int" },
			 { "ev", "", "1.03", "2.0","ff000000" },
			 "MT", "materials_api", "web"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::dist_field_mesh,
			 "m_dist_field",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "","" },
			 "MT", "materials_api", "dist_field_mesh","[B]","Loses animations from SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::mesh_color_from_sfo,
			 "m_dist_field_color",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "mesh_color_from_sfo"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::sfo_sandbox,
			 "m_sandbox",
			 { "ev", "sfo", "mt" },
			 { "EveryApi&", "SFO", "MT" },
			 { "ev", "", "" },
			 "MT", "materials_api", "sfo_sandbox"));
#endif

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
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_mesh,
			 "kf_mesh",
			 { "part" },
			 { "P" },
			 { "" },
			 "KF", "vertex_anim_api", "keyframe_mesh"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_lines,
			 "kf_lines",
			 { "part" },
			 { "LI" },
			 { "" },
			 "KF", "vertex_anim_api", "keyframe_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::keyframe_bind,
			 "kf_bind",
			 { "ev", "keyframe", "transform", "delta_time" },
			 { "EveryApi&", "KF", "PTT", "float" },
			 { "ev", "", "", "10.0" },
			 "KF", "vertex_anim_api", "keyframe_bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::repeat_keyframes,
			 "kf_repeat",
			 { "keyframe", "count" },
			 { "KF", "int" },
			 { "", "100" },
			 "KF", "vertex_anim_api", "repeat_keyframes"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::curve_trans,
			 "kf_curve",
			 { "ev", "keyframe", "curve", "pos", "numsamples", "duration" },
			 { "EveryApi&", "KF", "C", "CPP", "int", "float" },
			 { "ev", "", "", "", "10", "30.0" },
			 "KF", "vertex_anim_api", "curve_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::sample_rot,
			 "kf_rot",
			 { "ev", "kf", "nx", "ny", "nz", "angle", "numsamples", "duration" },
			 { "EveryApi&", "KF", "float", "float", "float", "float", "int", "float" },
			 { "ev", "", "0.0", "1.0", "0.0", "6.282", "15", "30.0" },
			 "KF", "vertex_anim_api", "sample_rot"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::empty_trans,
			 "ptt_empty",
			 { },
			 { },
			 { },
			 "PTT", "vertex_anim_api", "empty_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::translate_trans,
			 "ptt_trans_s",
			 { "prev", "speed_x", "speed_y", "speed_z" },
			 { "PTT", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PTT", "vertex_anim_api", "translate_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::translate_trans2,
			 "ptt_trans_d",
			 { "prev", "duration", "dist_x", "dist_y", "dist_z" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "10.0", "0.0", "0.0", "0.0" },
			 "PTT", "vertex_anim_api", "translate_trans2"));
#if 0
  // rotation doesnt work in vertex anim, since vertex anim
  // uses time ranges + linear interpolation inside the range
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::rotate_trans,
			 "ptt_rotate_s",
			 { "prev", "nx", "ny", "nz", "speed_angle" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "0.0", "1.0", "0.0", "6.282" },
			 "PTT", "vertex_anim_api", "rotate_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::rotate_trans2,
			 "ptt_rotate_d",
			 { "prev", "duration", "nx", "ny", "nz", "dist_angle" },
			 { "PTT", "float", "float", "float", "float", "float" },
			 { "", "10.0", "0.0", "1.0", "0.0", "6.282" },
			 "PTT", "vertex_anim_api", "rotate_trans2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::scale_trans,
			 "ptt_scale_s",
			 { "prev", "speed_x", "speed_y", "speed_z" },
			 { "PTT", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "PTT", "vertex_anim_api", "scale_trans"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::scale_trans2,
			 "ptt_scale_d",
			 { "prev", "duration", "dist_x", "dist_y", "dist_z" },
			 { "PTT", "float", "float", "float", "float" },
			 { "", "10.0", "1.0", "1.0", "1.0" },
			 "PTT", "vertex_anim_api", "scale_trans2"));
  return vec;
}
#endif // SECTION_1
#ifdef SECTION_2
std::vector<GameApiItem*> blocker_functions()
{

  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script,
			 "bm_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/tiiliseina_bm.mp", "a", "b", "c", "d", "e" },
			 "BM", "mainloop_api", "load_BM_script"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script,
			 "p_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/blob_p.mp", "a", "b", "c", "d", "e" },
			 "P", "mainloop_api", "load_P_script"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script,
			 "ml_script",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/marble_cube_ml.mp", "a", "b", "c", "d", "e" },
			 "ML", "mainloop_api", "load_ML_script"));




  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_BM_script_array,
			 "bm_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/tiiliseina_bm.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[BM]", "mainloop_api", "load_BM_script_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_P_script_array,
			 "p_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/blob_p.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[P]", "mainloop_api", "load_P_script_array"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_ML_script_array,
			 "ml_script_arr",
			 { "ev", "url", "%1", "%2", "%3", "%4", "%5" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "std::string", "std::string", "std::string" },
			 { "ev", "http://tpgames.org/marble_cube_ml.mp", "a&a", "b&b", "c&c", "d&d", "e&e" },
			 "[ML]", "mainloop_api", "load_ML_script_array"));

#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chai_bm,
			 "chai_bm",
			 { "url", "sx", "sy" },
			 { "std::string", "int", "int" },
			 { "http://tpgames.org/test_bm.chai", "100", "100" },
			 "BM", "bitmap_api", "chai_bm"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_scene,
			 "scene_ml",
			 { "ev", "url", "sx", "sy" },
			 { "EveryApi&", "std::string", "int", "int" },
			 { "ev", "http://tpgames.org/landscape.scn", "600", "600" },
			 "ML", "polygon_api", "load_scene"));
		     
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::skybox,
			 "skybox_ml",
			 { "ev", "land", "sky" },
			 { "EveryApi&", "BM", "BM" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "skybox"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::alt_ml_array,
			 "alt_ml",
			 { "ev", "vec", "start_time", "time_delta", "repeat" },
			 { "EveryApi&", "[ML]", "float", "float", "bool" },
			 { "ev", "", "0.0", "10.0", "true" },
			 "ML", "sprite_api", "alt_ml_array"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_ml,
			 "array_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "ML", "mainloop_api", "array_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_array,
			 "activate_array_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "ML", "mainloop_api", "activate_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::activate_arr_arr,
			 "ac_arr_ml",
			 { "arr" },
			 { "[ML]" },
			 { "" },
			 "[ML]", "mainloop_api", "activate_arr_arr"));
  /*
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timing_ml,
			 "timing_ml",
			 { "arr", "duration" },
			 { "[ML]", "float" },
			 { "", "10.0" },
			 "ML", "mainloop_api", "timing_ml"));*/
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml,
			 "seq_ml",
			 { "vec", "time" },
			 { "[ML]", "float" },
			 { "", "30.0" },
			 "ML", "mainloop_api", "seq_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::seq_ml_score,
			 "seq_ml_score",
			 { "ml1", "ml2", "target_score" },
			 { "ML", "ML", "int" },
			 { "", "", "100" },
			 "ML", "mainloop_api", "seq_ml_score"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::timed_tmp_seq_ml,
			 "timed_tmp_seq_ml",
			 { "curr", "end", "start_time", "end_time", "show_duration", "key" },
			 { "ML", "ML", "float", "float", "float", "int" },
			 { "", "", "0.0", "100.0", "30.0", "32" },
			 "ML", "mainloop_api", "timed_tmp_seq_ml"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_detection,
			 "collision_ml",
			 { "ev", "player_size", "enemy_size", "normal_game", "gameover_screen" },
			 { "EveryApi&", "float", "float",  "ML", "ML" },
			 { "ev", "100.0", "10.0", "", "" },
			 "ML", "mainloop_api", "collision_detection"));
#endif  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::move_ml,
			 "move_ml",
			 { "ev", "ml", "mn", "clone_count", "time_delta" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "1", "10.0" },
			 "ML", "move_api", "move_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::move_in,
			 "move_in",
			 { "ev", "ml", "in" },
			 { "EveryApi&", "ML", "INP" },
			 { "ev", "", "" },
			 "ML", "mainloop_api", "move_in"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::dyn_points,
			 "dyn_points_ml",
			 { "ev", "ml", "move", "pointnum", "pos_x", "pos_y", "pos_z" },
			 { "EveryApi&", "ML", "MN", "int", "float", "float", "float" },
			 { "ev", "", "", "0", "0.0", "0.0", "0.0" },
			 "ML", "mainloop_api", "dyn_points"));
		     
#if 0
  // This doesnt work since it eats too much memory.
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::movement_display,
			 "move_display_ml",
			 { "ev", "ml", "mn", "frames", "sx","sy","sz","s_x","e_x", "s_y", "e_y", "s_z", "e_z" },
			 { "EveryApi&", "ML", "MN","int", "int","int","int","float","float", "float","float","float","float" },
			 { "ev", "", "","30", "10","10","10","-300.0","300.0","-300.0","300.0", "-300.0","300.0" },
			 "ML", "points_api", "movement_display"));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::repeat_ml,
			 "repeat_ml",
			 { "ev", "ml", "duration" },
			 { "EveryApi&", "ML", "float" },
			 { "ev", "", "100.0" },
			 "ML", "move_api", "repeat_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::vertex_array_render,
			 "bm_render",
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "sprite_api", "vertex_array_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::turn_to_2d,
			 "bm_2d",
			 { "ev", "ml", "tl_x", "tl_y", "br_x", "br_y" },
			 { "EveryApi&", "ML","float", "float", "float", "float" },
			 { "ev", "", "0.0", "0.0", "800.0", "600.0" },
			 "ML", "sprite_api", "turn_to_2d"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::scale_2d_screen,
			 "bm_2d_screen_scale",
			 { "ev", "orig", "sx", "sy" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "1024", "768" },
			 "ML", "mainloop_api", "scale_2d_screen"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render,
			 "sp_render",
			 { "ev", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_inst,
			 "sp_render_inst",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "ML", "polygon_api", "sprite_render_inst"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sprite_render_fade,
			 "sp_render_fade",
			 { "ev", "bm", "pts", "start_x", "end_x", "start_y", "end_y", "z", "flip", "start_time", "end_time" },
			 { "EveryApi&", "BM", "PTS", "float", "float", "float", "float", "float", "bool", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0", "false", "10.0", "40.0" },
			 "ML", "polygon_api", "sprite_render_fade"));
			 
  
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::create_vertex_array,
			 "p_prepare",
			 { "p", "b" },
			 { "P", "bool" },
			 { "", "false" },
			 "VA", "polygon_api", "create_vertex_array"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_vertex_array_ml2,
			 "p_render",
			 { "ev", "p" },
			 { "EveryApi&", "P" },
			 { "ev", "" },
			 "ML", "polygon_api", "render_vertex_array_ml2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml,
			 "p_render_inst",
			 { "ev", "p", "pts" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_dynamic_ml,
			 "p_render_dyn",
			 { "ev", "p", "dyn" },
			 { "EveryApi&", "P", "DC" },
			 { "ev", "", "" },
			 "ML", "polygon_api", "render_dynamic_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced_ml_fade,
			 "p_render_inst_fade",
			 { "ev", "p", "pts", "flip", "start_time", "end_time" },
			 { "EveryApi&", "P", "PTS", "bool", "float", "float" },
			 { "ev", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "render_instanced_ml_fade"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::render_instanced2_ml,
			 "p_render_inst2",
			 { "ev", "va", "pta" },
			 { "EveryApi&", "VA", "PTA" },
			 { "ev", "", "" },
			 "ML", "materials_api", "render_instanced2_ml","","Can be used for dynamic changes for pta"));
#if 0
  // doesnt work in emscripten

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

#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_render,
			 "pts_render",
			 { "ev", "pts" },
			 { "EveryApi&", "PTS" },
			 { "ev", "" },
			 "ML", "points_api", "pts_render"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::prepare,
			 "li_prepare",
			 { "li" },
			 { "LI" },
			 { "" },
			 "LLA", "lines_api", "prepare"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::render_ml,
			 "li_render",
			 { "ev", "lla" },
			 { "EveryApi&", "LLA" },
			 { "ev", "" },
			 "ML", "lines_api", "render_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::sfo_to_ml,
			 "sfo_render",
			 { "ev", "sfo", "sx", "sy" },
			 { "EveryApi&", "SFO", "float", "float" },
			 { "ev", "", "-1.0", "-1.0" },
			 "ML", "sh_api", "sfo_to_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, (GameApi::PTS (GameApi::VolumeApi::*)(GameApi::O,int,int,int, float,float, float,float, float,float))&GameApi::VolumeApi::instanced_positions,
			 "o_to_pts",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "30", "30", "30", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "volume_api", "instanced_positions"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rendercubes3,
			 "o_render",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "100", "100", "100", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "volume_api", "rendercubes3"));

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
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::m_bind_inst_many,
			 "m_bind_inst_many",
			 { "ev", "vec", "mat", "pts" },
			 { "EveryApi&", "[P]", "[MT]", "PTS" },
			 { "ev", "", "", "" },
			 "ML", "polygon_api", "m_bind_inst_many"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst_fade,
			 "m_fade_inst",
			 { "p", "pts", "mat", "flip", "start_time", "end_time" },
			 { "P", "PTS", "MT", "bool", "float", "float" },
			 { "", "", "", "false", "10.0", "40.0" },
			 "ML", "materials_api", "bind_inst_fade"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_x_ml,
			 "rot_x_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed", "start_angle", "end_angle"},
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100000.0", "100000.0" },
			 "ML", "move_api", "rot_x_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_y_ml,
			 "rot_y_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_y", "end_y" },
			 { "EveryApi&", "ML", "int", "int", "float","float", "float" },
			 { "ev", "", "100", "97", "0.01","-100.0","100.0" },
			 "ML", "move_api", "rot_y_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::rot_z_ml,
			 "rot_z_ml",
			 { "ev", "ml", "key_forward", "key_backward", "speed","start_z", "end_z" },
			 { "EveryApi&", "ML", "int", "int", "float","float","float" },
			 { "ev", "", "100", "97", "0.01","-100000.0","100000.0" },
			 "ML", "move_api", "rot_z_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::enable_ml,
			 "enable_ml",
			 { "ev", "ml", "start_time", "end_time" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "0.0", "100.0" },
			 "ML", "move_api", "enable_ml"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::color_ml,
			 "color_ml",
			 { "ev", "color_num", "ml", "cc" },
			 { "EveryApi&", "int", "ML", "CC" },
			 { "ev", "0", "", "" },
			 "ML", "move_api", "color_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::all_cursor_keys,
			 "wasd_ml",
			 { "ev", "ml", "speed", "duration" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "8.0", "1.0" },
			 "ML", "move_api", "all_cursor_keys"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml,
			 "quake_ml",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml2,
			 "quake_ml2",
			 { "ev", "ml", "speed", "rot_speed" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "20.0", "0.03" },
			 "ML", "move_api", "quake_ml2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::quake_ml3,
			 "fly_ml",
			 { "ev", "ship", "world", "speed", "rot_speed", "p_x", "p_y", "p_z" },
			 { "EveryApi&", "ML", "ML", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "20.0", "0.03", "0.0", "0.0", "0.0" },
			 "ML", "move_api", "quake_ml3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::debug_obj, 
			 "debug_obj",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "ML", "mainloop_api", "debug_obj"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_activate_ml,
			 "key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::temp_key_activate_ml,
			 "tmp_key_activate_ml",
			 { "ev", "ml", "mn", "key", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "float" },
			 { "ev", "", "", "32", "10.0" },
			 "ML", "move_api", "temp_key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::comb_key_activate_ml,
			 "comb_key_activate_ml",
			 { "ev", "ml", "mn", "key", "key_2", "duration" },
			 { "EveryApi&", "ML", "MN", "int", "int", "float" },
			 { "ev", "", "", "32", "32", "10.0" },
			 "ML", "move_api", "comb_key_activate_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::keyboard_toggle,
			 "key_toggle",
			 { "ml1", "ml2", "key" },
			 { "ML", "ML", "int" },
			 { "", "", "32" },
			 "ML", "mainloop_api", "keyboard_toggle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::touch_rotate,
			 "touch_rotate",
			 { "ev", "ml", "leftright", "topdown", "x_speed", "y_speed" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "true", "true", "0.01", "0" },
			 "ML", "mainloop_api", "touch_rotate"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::record_keypresses,
			 "key_record_ml",
			 { "ml", "output_filename" },
			 { "ML", "std::string" },
			 { "", "key_record.txt" },
			 "ML", "mainloop_api", "record_keypresses"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::playback_keypresses,
			 "key_playback_ml",
			 { "ml", "input_url" },
			 { "ML", "std::string" },
			 { "", "http://tpgames.org/key_record.txt" },
			 "ML", "mainloop_api", "playback_keypresses" ));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::move_api, &GameApi::MovementNode::key_printer_ml,
			 "key_printer_ml",
			 { "ml" },
			 { "ML" },
			 { "" },
			 "ML", "move_api", "key_printer_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::fps_display,
			 "fps_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "fps_display"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_display,
			 "score_display",
			 { "ev", "ml", "font" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "score_display"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::time_display,
			 "time_display",
			 { "ev", "ml", "font", "time" },
			 { "EveryApi&", "ML", "std::string", "float" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf", "1000.0" },
			 "ML", "mainloop_api", "time_display"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::restart_screen,
			 "restart_screen",
			 { "ev", "ml", "fontname" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/Chunkfive.otf" },
			 "ML", "mainloop_api", "restart_screen"));
			

  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind,
			 "skeletal_bind",
			 { "ev", "model", "points", "movement" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]" },
			 { "ev", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::skeletal_api, &GameApi::Skeletal::skeletal_bind_material,
			 "skeletal_material",
			 { "ev", "model", "points", "movement", "material" },
			 { "EveryApi&", "[P]", "[PT]", "[SA]", "MT" },
			 { "ev", "", "", "", "" },
			 "ML", "skeletal_api", "skeletal_bind_material"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::tree_api, &GameApi::TreeApi::tree_ml,
			 "tree_ml",
			 { "ev", "tree", "vec" },
			 { "EveryApi&", "T", "[ML]" },
			 { "ev", "", "" },
			 "ML", "tree_api", "tree_ml"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::materials_api, &GameApi::MaterialsApi::bind_inst2,
			 "m_bind_inst2",
			 { "p", "pta", "mat" },
			 { "P", "PTA", "MT" },
			 { "", "", "" },
			 "ML", "materials_api", "bind_inst2"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::vertex_anim_api, &GameApi::VertexAnimApi::vertex_anim_render,
			 "kf_render",
			 { "ev", "keyframe" },
			 { "EveryApi&",  "KF" },
			 { "ev",  "" },
			 "ML", "vertex_anim_api", "vertex_anim_render"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::depthfunc,
			 "ogl_depthfunc",
			 { "ml", "val" },
			 { "ML", "int" },
			 { "", "1" },
			 "ML", "mainloop_api", "depthfunc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::blendfunc,
			 "ogl_blendfunc",
			 { "ml", "val", "val2" },
			 { "ML", "int", "int" },
			 { "", "2", "3" },
			 "ML", "mainloop_api", "blendfunc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::load_song,
			 "song_ml",
			 { "ev", "next", "url" },
			 { "EveryApi&", "ML", "std::string" },
			 { "ev", "", "http://tpgames.org/piano_variations.ogg" },
			 "ML", "mainloop_api", "load_song"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::tracker_api, &GameApi::TrackerApi::play_wave_via_keypress,
			 "sound_ml",
			 { "ev", "ml", "url", "key" },
			 { "EveryApi&", "ML", "std::string", "int" },
			 { "ev", "", "http://tpgames.org/Clap.wav", "32" },
			 "ML", "tracker_api", "play_wave_via_keypress"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window, 
			 "blk_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "BLK", "blocker_api", "game_window"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window2, 
			 "run_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML","bool","bool", "float", "float" },
			 { "ev", "","false","false", "0.0", "100000.0" },
			 "RUN", "blocker_api", "game_window2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::small_window,
			 "sml_window",
			 { "ev", "ml", "x", "y", "sx", "sy" },
			 { "EveryApi&", "ML", "int", "int", "int", "int" },
			 { "ev", "", "100", "100", "320", "200" },
			 "ML", "mainloop_api", "small_window"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass,
  //			 "run_looking_glass",
  //			 { "ev", "ml" },
  //			 { "EveryApi&", "ML" },
  //			 { "ev", "" },
  //			 "ML", "mainloop_api", "looking_glass"));
#if 1
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_window_2nd_display,
			 "looking_glass_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float" },
			 { "ev", "", "false", "false", "0.0", "1000000.0" },
			 "RUN", "blocker_api", "game_window_2nd_display"));
#endif
  //vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_lib,
  //			 "looking_glass_lib",
  //			 { "txid", "sx", "sy", "x", "y" },
  //			 { "TXID", "int", "int", "int", "int" },
  //			 { "", "512", "256", "4", "8" },
  //			 "ML", "mainloop_api", "looking_glass_lib"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::looking_glass_full,
			 "looking_glass_full",
			 { "ev", "ml", "amount", "mode", "start", "end" },
			 { "EveryApi&", "ML", "float", "int", "int", "int" },
			 { "ev", "", "150.0", "0", "0", "45" },
			 "ML", "mainloop_api", "looking_glass_full"));
#ifdef VIRTUAL_REALITY
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_window,
			 "vr_window",
			 { "ev", "ml", "logo", "fpscounter", "start_time", "duration", "invert", "translate" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float", "bool", "bool" },
			 { "ev", "", "false", "false", "0.0", "100000.0", "true", "true" },
			 "RUN", "blocker_api", "vr_window"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_submit,
			 "vr_submit",
			 { "ev", "left_eye", "right_eye" },
			 { "EveryApi&", "TXID", "TXID" },
			 { "ev", "", "" },
			 "ML", "blocker_api", "vr_submit"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::vr_submit_ml,
			 "vr_submit_ml",
			 { "ev", "ml", "left_eye", "right_eye", "invert", "translate" },
			 { "EveryApi&", "ML", "TXID", "TXID", "bool", "bool" },
			 { "ev", "", "", "", "false", "true" },
			 "ML", "blocker_api", "vr_submit_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::setup_hmd_projection,
			 "vr_projection",
			 { "ev", "ml", "eye", "is_std", "near", "far","translate" },
			 { "EveryApi&", "ML", "bool", "bool", "float", "float","bool" },
			 { "ev", "", "false", "true", "10.1", "60000.0", "true" },
			 "ML", "mainloop_api", "setup_hmd_projection"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::run_seq,
			 "run_seq",
			 { "ev", "vec" },
			 { "EveryApi&", "[RUN]" },
			 { "ev", "" },
			 "RUN", "blocker_api", "run_seq"));
  
#if 0
  // doesnt work in emscripten
  vec.push_back(ApiItemF(&GameApi::EveryApi::blocker_api, &GameApi::BlockerApi::game_seq,
			 "blk_seq",
			 { "ev", "vec" },
			 { "EveryApi&", "[BLK]" },
			 { "ev", "" },
			 "BLK", "blocker_api", "game_seq"));

#endif
  return vec;
}
#endif // SECTION_2
#ifdef SECTION_3
std::vector<GameApiItem*> waveform_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::empty,
			 "wv_empty",
			 { "length" },
			 { "float" },
			 { "1.0" },
			 "WV", "waveform_api", "empty"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::step,
			 "wv_step",
			 { "flip" },
			 { "bool" },
			 { "false" },
			 "WV", "waveform_api", "step"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::sinwave,
			 "wv_sin",
			 { "length", "freq" },
			 { "float", "float" },
			 { "6.28318", "1.0" },
			 "WV", "waveform_api", "sinwave"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::gaussian,
			 "wv_gaussian",
			 { "start_x", "end_x", "start_y", "end_y" },
			 { "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-1.0", "1.0" },
			 "WV", "waveform_api", "gaussian"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::sum,
			 "wv_sum",
			 { "w1", "w2" },
			 { "WV", "WV" },
			 { "", "" },
			 "WV", "waveform_api", "sum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::move,
			 "wv_move",
			 { "w1", "delta" },
			 { "WV", "float" },
			 { "", "0.0" },
			 "WV", "waveform_api", "move"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::wave_move_y,
			 "wv_move_y",
			 { "wave", "delta" },
			 { "WV", "float" },
			 { "", "0.0" },
			 "WV", "float_volume_api", "wave_move_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::scale,
			 "wv_scale",
			 { "w1", "scale" },
			 { "WV", "float" },
			 { "", "1.0" },
			 "WV", "waveform_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::waveform_rotated_polygon,
			 "wv_rot_poly",
			 { "wave", "start_angle", "end_angle", "radius", "num_samples", "num_waves" },
			 { "WV", "float", "float", "float", "int", "int" },
			 { "", "0.0", "6.28318", "300.0", "40", "40" },
			 "P", "waveform_api", "waveform_rotated_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::cubic,
			 "wv_cubic",
			 { "f_0", "f_1", "df_0", "df_1", "min_y", "max_y" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.5", "-0.5", "-5.0", "5.0" },
			 "WV", "waveform_api", "cubic"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::repeat,
			 "wv_repeat",
			 { "wave", "num" },
			 { "WV", "int" },
			 { "", "2" },
			 "WV", "waveform_api", "repeat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::length_change,
			 "wv_set_length",
			 { "wave", "new_length" },
			 { "WV", "float" },
			 { "", "1.0" },
			 "WV", "waveform_api", "length_change"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::waveform_bitmap,
			 "wv_bitmap",
			 { "wave", "sx", "sy", "true_color", "false_color" },
			 { "WV", "int", "int", "unsigned int", "unsigned int" },
			 { "", "100", "100", "ffffffff", "00000000" },
			 "BM", "waveform_api", "waveform_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::std_polynomial,
			 "pn_std",
			 { "x_5", "x_4", "x_3", "x_2", "x_1", "c" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "0.0", "0.0", "0.0" },
			 "PN", "waveform_api", "std_polynomial"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::df_dx,
			 "pn_df_dx",
			 { "poly" },
			 { "PN" },
			 { "" },
			 "PN", "waveform_api", "df_dx"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::waveform_api, &GameApi::WaveformApi::polynomial_wave,
			 "wv_polynomial",
			 { "pn", "start_x", "end_x", "start_y", "end_y" },
			 { "PN", "float", "float", "float", "float" },
			 { "", "-5.0", "5.0", "5.0", "-5.0" },
			 "WV", "waveform_api", "polynomial_wave"));
  return vec;
}
#endif // SECTION_3
#ifdef SECTION_1
 
std::vector<GameApiItem*> polydistfield_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::create_pd,
			 "create_pd",
			 { "mesh", "dist_field" },
			 { "P", "SFO" },
			 { "", "" },
			 "PD", "polygon_dist_api", "create_pd"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::cube,
			 "cube_pd",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float" },
			 { "ev", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "PD", "polygon_dist_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::rounded_cube,
			 "rounded_cube_pd",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "20.0" },
			 "PD", "polygon_dist_api", "rounded_cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::sphere,
			 "sphere_pd",
			 { "ev", "center", "radius", "numfaces1", "numfaces2" },
			 { "EveryApi&", "PT", "float", "int", "int" },
			 { "ev", "", "100.0", "30", "30" },
			 "PD", "polygon_dist_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::or_array,
			 "or_array_pd",
			 { "ev", "vec" },
			 { "EveryApi&", "[PD]" },
			 { "ev", "" },
			 "PD", "polygon_dist_api", "or_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::color_from_normal,
			 "pd_color_from_normal",
			 { "ev", "obj" },
			 { "EveryApi&", "PD" },
			 { "ev", "" },
			 "PD", "polygon_dist_api", "color_from_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::colormod_from_position,
			 "pd_colormod",
			 { "ev", "obj", "px", "py", "pz", "sx", "sy", "sz" },
			 { "EveryApi&", "PD", "float", "float", "float", "float", "float", "float" },
			 { "ev", "", "0.0", "0.0", "0.0", "100.0", "100.0", "100.0" },
			 "PD", "polygon_dist_api", "colormod_from_position"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::ambient_occulsion_sfo,
			 "ambient_occulsion_sfo",
			 { "ev", "obj", "d", "i" },
			 { "EveryApi&", "PD", "float", "float" },
			 { "ev", "", "10.2", "30.0" },
			 "PD", "polygon_dist_api", "ambient_occulsion_sfo"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::get_distance_field,
			 "to_sfo",
			 { "pd" },
			 { "PD" },
			 { "" },
			 "SFO", "poly_dist_api", "get_distance_field"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::get_polygon,
			 "to_p",
			 { "pd" },
			 { "PD" },
			 { "" },
			 "P", "poly_dist_api", "get_polygon"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_dist_api, &GameApi::PolygonDistanceField::mesh_color_from_sfo,
			 "mesh_color_from_sfo",
			 { "ev", "pd", "mat" },
			 { "EveryApi&", "PD", "MT" },
			 { "ev", "", "" },
			 "MT", "poly_dist_api", "mesh_color_from_sfo"));
 
  return vec;
}
#endif // SECTION_1
 
#ifdef SECTION_2
std::vector<GameApiItem*> polygonapi_functions()
{
  std::vector<GameApiItem*> i1 = polygonapi_functions1();
  std::vector<GameApiItem*> i2 = polygonapi_functions2();
  std::copy(i2.begin(),i2.end(),std::back_inserter(i1));
  return i1;
}
std::vector<GameApiItem*> polygonapi_functions1()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::empty,
			 "empty",
			 { },
			 { },
			 { },
			 "P", "polygon_api", "empty"));
#if 0
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
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::save_model_ml,
			 "save_model",
			 { "poly", "filename" },
			 { "P", "std::string" },
			 { "", "test.obj" },
			 "ML", "polygon_api", "save_model_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::save_ds_ml,
			 "save_ds",
			 { "ev", "out_filename", "poly" },
			 { "EveryApi&", "std::string", "P" },
			 { "ev", "test.ds", "" },
			 "ML", "mainloop_api", "save_ds_ml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url,
			 "p_url",
			 { "ev", "url", "count" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://tpgames.org/example.obj", "10" },
			 "P", "polygon_api", "p_url"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_mtl,
			 "p_mtl",
			 { "ev", "obj_url", "mtl_url", "url_prefix", "count" },
			 { "EveryApi&", "std::string", "std::string", "std::string", "int" },
			 { "ev", "http://tpgames.org/sponza/sponza.obj", "http://tpgames.org/sponza/sponza.mtl", "http://tpgames.org/sponza", "1" },
			 "P", "polygon_api", "p_mtl"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_url_mtl,
			 "p_url_mtl",
			 { "ev", "url", "count", "material_names" },
			 { "EveryApi&", "std::string", "int", "[std::string]" },
			 { "ev", "http://tpgames.org/example.obj", "10", "" },
			 "P", "polygon_api", "p_url_mtl"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::p_ds_url,
			 "p_ds_url",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/sponza.ds" },
			 "P", "polygon_api", "p_ds_url"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::stl_load,
			 "p_stl",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/teapot.stl" },
			 "P", "polygon_api", "stl_load"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::alt,
			 "p_alt",
			 { "vec", "index" },
			 { "[P]", "int" },
			 { "", "0" },
			 "P", "polygon_api", "alt"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::poly_array,
			 "p_array",
			 { "vec" },
			 { "[P]" },
			 { "" },
			 "[P]", "polygon_api", "poly_array"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::triangle,
			 "triangle",
			 { "p1", "p2", "p3" },
			 { "PT", "PT", "PT" },
			 { "", "", "" },
			 "P", "polygon_api", "triangle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quads_to_triangles,
			 "to_triangles",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "quads_to_triangles"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fullscreen_quad,
			 "quad_fullscreen",
			 { "ev" },
			 { "EveryApi&" },
			 { "ev" },
			 "P", "polygon_api", "fullscreen_quad"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::vr_fullscreen_quad,
			 "vr_fullscreen_quad",
			 { "ev", "is_right_eye" },
			 { "EveryApi&", "bool" },
			 { "ev", "false" },
			 "P", "polygon_api", "vr_fullscreen_quad"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::polygon3,
			 "polygon",
			 { "vec" },
			 { "PTS" },
			 { "" },
			 "P", "polygon_api", "polygon3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(float,float,float,float,float,float))&GameApi::PolygonApi::cube,
			 "cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "P", "polygon_api", "cube"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rounded_cube,
			 "rounded_cube",
			 { "ev", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r_radius" },
			 { "EveryApi&", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0", "20.0" },
			 "P", "polygon_api", "rounded_cube"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere,
			 "sphere",
			 { "center", "radius", "numfaces1", "numfaces2" },
			 { "PT", "float", "int", "int" },
			 { "(0.0,0.0,0.0)",  "100.0", "30", "30" },
			 "P", "polygon_api", "sphere"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::plane_map,
			 "plane_map",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "start_values", "end_values", "fb", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "float", "float", "FB", "int", "int" },
			 { "-200.0", "200.0", "-200.0", "200.0", "0.0", "70.0", "0.0", "1.0", "", "256", "256" },
			 "P", "polygon_api", "plane_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere_map,
			 "sphere_map",
			 { "c_x", "c_y", "c_z", "fb", "start_radius", "end_radius", "start_values", "end_values", "sx", "sy" },
			 { "float", "float", "float", "FB", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "0.0", "0.0", "", "200.0", "250.0", "0.0", "1.0", "30", "30" },
			 "P", "polygon_api", "sphere_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map,
			 "shadow_map",
			 { "ev", "p", "p_x", "p_y", "p_z", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "0.0", "512", "512" },
			 "BM", "polygon_api", "shadow_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map2,
			 "shadow_map2",
			 { "p", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "P", "float", "float", "float", "int", "int", "P" },
			 { "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BB", "polygon_api", "shadow_map2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow_map3,
			 "shadow_map3",
			 { "ev", "objs", "p_x", "p_y", "p_z", "sx", "sy", "quad" },
			 { "EveryApi&", "P", "float", "float", "float", "int", "int", "P" },
			 { "ev", "", "0.0", "-300.0", "0.0", "100", "100", "" },
			 "BM", "polygon_api", "shadow_map3"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::disc,
			 "disc",
			 { "ev", "numfaces", "center_x", "center_y", "center_z", "normal_x", "normal_y", "normal_z", "radius" },
			 { "EveryApi&", "int", "float", "float", "float", "float", "float", "float", "float" },
			 { "ev", "30", "0.0", "0.0" ,"0.0", "0.0", "0.0", "1.0", "100.0" },
			 "P", "polygon_api", "disc"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::cone,
			 "cone",
			 { "numfaces", "p1", "p2", "radius1", "radius2" },
			 { "int", "PT", "PT", "float", "float" },
			 { "30", "(0.0,0.0,0.0)", "(0.0,100.0,0.0)", "30.0", "10.0" },
			 "P", "polygon_api", "cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::torus2,
			 "torus",
			 { "ev", "numfaces1", "numfaces2", "center", "radius1", "radius2" },
			 { "EveryApi&", "int", "int", "PT", "float", "float" },
			 { "ev", "20", "20", "", "250.0", "50.0" },
			 "P", "polygon_api", "torus2"));
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
			 "P", "polygon_api", "translate"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatex,
			 "rotatex",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatex"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatey,
			 "rotatey",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatey"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatez,
			 "rotatez",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "rotatez"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::scale,
			 "scale",
			 { "orig", "sx", "sy", "sz" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "scale"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_choose,
			 "lod_choose",
			 { "vec", "name" },
			 { "[P]", "std::string" },
			 { "", "lod" },
			 "P", "polygon_api", "lod_choose"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_set,
			 "lod_set",
			 { "p", "name", "value" },
			 { "P", "std::string", "int" },
			 { "", "lod", "0" },
			 "P", "polygon_api", "lod_set"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::lod_select,
			 "lod_select",
			 { "start_dist", "dist_step", "max_value" },
			 { "float", "float", "int" },
			 { "0.0", "300.0", "3" },
			 "IF", "polygon_api", "lod_select"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::mesh_resize,
			 "mesh_resize",
			 { "p", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "P", "float", "float", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "polygon_api", "mesh_resize"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fix_vertex_order,
			 "fix_vertex_order",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "fix_vertex_order"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::filter_invisible,
			 "filter_invisible",
			 { "p", "size" },
			 { "P", "float" },
			 { "", "1.0" },
			 "P", "polygon_api", "filter_invisible"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xy_p,
			 "p_repeat_xy",
			 { "ev", "p", "start_x", "start_y", "dx", "dy", "sx", "sy" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xy_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_xz_p,
			 "p_repeat_xz",
			 { "ev", "p", "start_x", "start_z", "dx", "dz", "sx", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_xz_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::repeat_yz_p,
			 "p_repeat_yz",
			 { "ev", "p", "start_y", "start_z", "dy", "dz", "sy", "sz" },
			 { "EveryApi&", "P", "float", "float", "float", "float", "int", "int" },
			 { "ev", "", "0.0", "0.0", "30.0", "30.0", "10", "10" },
			 "P", "polygon_api", "repeat_yz_p"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::line_to_cone,
			 "li_to_cone",
			 { "ev", "li", "size", "numfaces" },
			 { "EveryApi&", "LI", "float", "int" },
			 { "ev", "", "10.0", "15" },
			 "P", "polygon_api", "line_to_cone"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::curve_to_poly,
			 "curve_to_poly",
			 { "curve", "start_x", "end_x", "start_y", "end_y", "start_angle", "end_angle", "numinstances" },
			 { "C", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "-4.0", "4.0", "-2.0", "2.0", "1.2", "7.48318", "120" },
			 "P", "polygon_api", "curve_to_poly"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing,
			 "static_instancing",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_vertex_color,
			 "inst_vertex_color",
			 { "ev", "obj", "pos" },
			 { "EveryApi&", "P", "PTS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing_vertex_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_with_color,
			 "static_inst_color",
			 { "ev", "obj", "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "P", "BM", "float", "float", "float", "float", "float" },
			 { "ev", "", "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "P", "polygon_api", "static_instancing_with_color"));
  return vec;
}
#endif // SECTION_2
#ifdef SECTION_3
std::vector<GameApiItem*> polygonapi_functions2()
{
  std::vector<GameApiItem*> vec;

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_map,
			 "color_map",
			 { "bm", "statt_x", "end_x", "start_y", "end_y", "z" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "-200", "200", "-200", "200", "0" },
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::min_color,
			 "p_min_color",
			 { "orig", "orig2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "min_color"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::max_color,
			 "p_max_color",
			 { "orig", "orig2" },
			 { "P", "P" },
			 { "", "" },
			 "P", "polygon_api", "max_color"));



  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_faces,
			 "color_faces",
			 { "orig", "color_1", "color_2", "color_3", "color_4" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ffffffff", "ff888888", "ff888888" },
			 "P", "polygon_api", "color_faces"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_alpha,
			 "color_alpha",
			 { "orig", "alpha" },
			 { "P", "unsigned int" },
			 { "", "ff" },
			 "P", "polygon_api", "color_alpha"));
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_distance,
			 "color_distance",
			 { "model", "center_x", "center_y", "center_z", "color_center", "color_dist", "dist_center", "dist_dist" },
			 { "P", "float", "float", "float", "unsigned int", "unsigned int", "float", "float" },
			 { "", "0.0", "0.0", "0.0", "ffffffff", "ff000000", "10.0", "300.0" },
			 "P", "polygon_api", "color_distance"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_range,
			 "color_range",
			 { "orig", "source_upper", "source_lower", "upper_range", "lower_range" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "00000000","ffffffff", "88888888" },
			 "P", "polygon_api", "color_range"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_lambert,
			 "color_lambert",
			 { "orig", "color", "light_dir_x", "light_dir_y", "light_dir_z", "pow", "intensity" },
			 { "P", "unsigned int", "float", "float", "float", "float","float" },
			 { "", "ff884422", "-2.0", "1.0", "2.0","1.0","0.3" },
			 "P", "polygon_api", "color_lambert"));


  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::recalculate_normals,
			 "recalc_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "recalculate_normals"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::smooth_normals2,
			 "smooth_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "smooth_normals2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spherical_normals,
			 "spherical_normals",
			 { "p", "p_x", "p_y", "p_z" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P", "polygon_api", "spherical_normals"));
#if 0
  // doesnt work too well
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::average_normals,
			 "average_normals",
			 { "orig", "sx", "sy" },
			 { "P", "int", "int" },
			 { "", "30", "30" },
			 "P", "polygon_api", "average_normals"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::flip_normals,
			 "flip_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "flip_normals"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::build_offsets,
			 "p_offsets",
			 { "orig", "vec" },
			 { "P", "[PT]" },
			 { "", "" },
			 "P", "polygon_api", "build_offsets"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_plane,
			 "texcoord_plane",
			 { "orig", "start_x", "end_x", "start_y", "end_y" },
			 { "P", "float", "float", "float", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0" },
			 "P", "polygon_api", "texcoord_plane"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_normal_to_texcoord,
			 "texcoord_from_normal",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "from_normal_to_texcoord"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_subarea,
			 "texcoord_subarea",
			 { "p", "start_x", "end_x", "start_y", "end_y" },
			 { "P", "float", "float", "float", "float" },
			 { "", "0.0", "0.5", "0.0", "0.5" },
			 "P", "polygon_api", "texcoord_subarea"));
#if 0
  // doesnt work since wrapping at edge of texture works wrong
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_from_normal,
			 "texcoord_from_normal",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "texcoord_from_normal"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::fix_texcoord,
			 "texcoord_fix",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "fix_texcoord"));
#endif
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texcoord_spherical2,
			 "texcoord_spherical",
			 { "ev", "center", "r", "orig" },
			 { "EveryApi&", "PT", "float", "P" },
			 { "ev", "", "100.0", "" },
			 "P", "polygon_api", "texcoord_spherical2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_splitter,
			 "p_tex_splitter",
			 { "obj", "start_index", "end_index" },
			 { "P", "int", "int" },
			 { "", "0", "16" },
			 "P", "polygon_api", "texture_splitter"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_storage,
			 "p_tex_storage",
			 { "obj", "texture_sx", "texture_sy" },
			 { "P", "int", "int" },
			 { "", "1024", "1024" },
			 "P", "polygon_api", "texture_storage"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::light_transport,
			 "p_tex_light",
			 { "obj", "num", "light_dir_x", "light_dir_y", "light_dir_z" },
			 { "P", "int", "float", "float", "float" },
			 { "", "10000", "1.0", "1.0", "1.0" },
			 "P", "polygon_api", "light_transport"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_from_p,
			 "texture_from_p",
			 { "obj", "num" },
			 { "P", "int" },
			 { "", "0" },
			 "BM", "polygon_api", "texture_from_p"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::replace_texture,
			 "p_tex_replace",
			 { "obj", "bm", "num" },
			 { "P", "BM", "int" },
			 { "", "", "0" },
			 "P", "polygon_api", "replace_texture"));

  #if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::log_coords,
			 "log_coords",
			 { "p" },
			 { "P" },
			 { "" },
			 "P", "polygon_api", "log_coords"));
  #endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::log_coords2,
			 "log_coords",
			 { "p", "x_count", "y_count", "sx", "sy", "sz" },
			 { "P", "int", "int", "float", "float", "float" },
			 { "", "5", "5", "30.0", "30.0", "30.0" },
			 "P", "polygon_api", "log_coords2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spherical_wave,
			 "sph_wave",
			 { "p", "r1", "freq_1", "r2", "freq_2" },
			 { "P", "float", "float", "float", "float" },
			 { "", "30.0", "1.0", "30.0", "3.0" },
			 "P", "polygon_api", "spherical_wave"));
			
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::splitquads,
			 "splitquads",
			 { "p", "x_count", "y_count" },
			 { "P", "int", "int" },
			 { "", "5", "5" },
			 "P", "polygon_api", "splitquads"));
			 
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shading_shader,
			 "p_shading",
			 { "ev", "mainloop", "level1", "level2", "level3", "spec_size", "ambient", "diffuse", "specular" },
			 { "EveryApi&", "ML", "unsigned int", "unsigned int", "unsigned int", "float", "bool", "bool", "bool" },
			 { "ev", "", "ff442211", "ffff8844", "ffffffff", "15.0", "true", "true", "true" },
			 "ML", "polygon_api", "shading_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::choose_color_shader,
			 "p_color",
			 { "ev", "mainloop", "color", "mix_val" },
			 { "EveryApi&", "ML", "unsigned int", "float" },
			 { "ev", "", "ffaa9988", "0.5" },
			 "ML", "polygon_api", "choose_color_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::spotlight_shader,
			 "p_spotlight",
			 { "ev", "mainloop", "light_color_id", "move" },
			 { "EveryApi&", "ML", "int", "MN" },
			 { "ev", "", "0", "" },
			 "ML", "polygon_api", "spotlight_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::noise_shader,
			 "p_noise",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "noise_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::dither_shader,
			 "p_dither",
			 { "ev", "mainloop" },
			 { "EveryApi&", "ML" },
			 { "ev", "" },
			 "ML", "polygon_api", "dither_shader"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::blur_shader,
			 "p_blur",
			 { "ev", "mainloop", "x_val", "y_val" },
			 { "EveryApi&", "ML", "float", "float" },
			 { "ev", "", "5", "0" },
			 "ML", "polygon_api", "blur_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bloom1_shader,
			 "p_bloom_cut",
			 { "ev", "mainloop", "r", "g", "b" },
			 { "EveryApi&", "ML", "float", "float", "float" },
			 { "ev", "", "0.5", "0.5", "0.5" },
			 "ML", "polygon_api", "bloom1_shader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::wave_shader,
			 "p_wave",
			 { "ev", "mainloop", "radius", "t_mult", "x_mult", "y_mult" },
			 { "EveryApi&", "ML", "float", "float", "float", "float" },
			 { "ev", "", "0.02", "1.5", "17", "9" },
			 "ML", "polygon_api", "wave_shader"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::shader_api, &GameApi::ShaderApi::shader_choice,
			 "shader",
			 { "ev", "choose" },
			 { "EveryApi&", "int" },
			 { "ev", "0" },
			 "SH", "shader_api", "shader_choice"));
  return vec;
}
#endif // SECTION_3
#ifdef SECTION_1
 std::vector<GameApiItem*> framebuffermoduleapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_sprite_draw,
			 "fr_sprite_draw",
			 { "name", "bm", "move", "x", "y", "fmt", "start_time" },
			 { "std::string", "BM", "MN", "int", "int", "int", "float" },
			 { "s_tmp1", "", "", "0", "0", "1", "0.0" },
			 "FML", "low_frame_api", "low_sprite_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_sprite_array,
			 "fr_sprite_array",
			 { "name", "url", "bms", "mn", "x", "y", "fmt", "start_time" },
			 { "std::string", "std::string", "[BM]", "MN", "int", "int", "int", "float" },
			 { "a_tmp1", "http://tpgames.org/pos_lst.sp", "", "", "0", "0", "1", "0.0" },
			 "FML", "low_frame_api", "low_sprite_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::dynamic_character_frame,
			 "fr_sprite_choose",
			 { "ev", "vec", "fetcher", "x", "y", "fmt", "mn" },
			 { "EveryApi&", "[BM]", "IF", "int", "int", "int", "MN" },
			 { "ev", "", "", "0", "0", "1", "" },
			 "FML", "font_api", "dynamic_character_frame"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_poly_draw,
			 "fr_poly_draw",
			 { "name", "p", "mn" },
			 { "std::string", "P", "MN" },
			 { "p_tmp1", "", "" },
			 "FML", "low_frame_api", "low_poly_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_render_world,
			 "fr_world_draw",
			 { "blocks", "screen_width", "screen_height", "fmt" },
			 { "[BM]", "int", "int", "int" },
			 { "", "800", "600", "1" },
			 "FML", "low_frame_api", "low_render_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_scroll_world,
			 "fr_scroll_world",
			 { "ml", "speed_x", "speed_y", "p_x", "p_y", "left_offset","right_offset", "height", "height2", "mode", "jump_frames" },
			 { "FML", "float", "float", "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "", "1.0", "1.0", "400.0", "300.0", "10.0", "10.0", "5.0", "30.0", "0", "15" },
			 "FML", "low_frame_api", "low_scroll_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_build_world,
			 "fr_build_world",
			 { "ml", "url", "chars", "x", "y" },
			 { "FML", "std::string", "std::string", "int", "int" },
			 { "", "http://tpgames.org/map.txt", ".#", "0", "0" },
			 "FML", "low_frame_api", "low_build_world"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_enemy_draw,
			 "fr_enemy_draw",
			 { "bm", "url", "fmt", "speed" },
			 { "BM", "std::string", "int", "float" },
			 { "", "http://tpgames.org/enemy0.txt", "1", "0.03" },
			 "FML", "low_frame_api", "low_enemy_draw"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_enemy_draw2,
			 "fr_enemy_draw2",
			 { "bm", "url", "fmt", "speed", "time_delta", "time_duration" },
			 { "[BM]", "std::string", "int", "float", "int", "int" },
			 { "", "http://tpgames.org/enemy0.txt", "1", "0.03", "1", "8" },
			 "FML", "low_frame_api", "low_enemy_draw2"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::array_fml,
			 "array_fml",
			 { "arr" },
			 { "[FML]" },
			 { "" },
			 "FML", "mainloop_api", "array_fml"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_frame_bitmap,
			 "fr_bitmap",
			 { "ml", "sx", "sy" },
			 { "FML", "int", "int" },
			 { "", "800", "600" },
			 "BM", "low_frame_api", "low_frame_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_collision,
			 "fr_collision",
			 { "ml", "start_x", "end_x", "start_y", "end_y", "key" },
			 { "FML", "float", "float", "float", "float", "int" },
			 { "", "100.0", "120.0", "100.0", "120.0", "32" },
			 "FML", "low_frame_api", "low_collision"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_activate_snapshot,
			 "fr_snapshot",
			 { "ev", "ml", "key", "move", "duration", "next" },
			 { "EveryApi&","FML", "int", "MN", "float", "FML" },
			 { "ev", "", "32", "", "30.0", "" },
			 "FML", "low_frame_api", "low_activate_snapshot"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::qml_print,
  //			 "fr_print_qml",
  //			 { "url" },
  //			 { "std::string" },
  //			 { "http://tpgames.org/wearable.qml" },
  //			 "FML", "low_frame_api", "qml_print"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::qml_create_node,
  //			 "fr_create_qml",
  //			 { "url" },
  //			 { "std::string" },
  //			 { "http://tpgames.org/wearable.qml" },
  //			 "FML", "low_frame_api", "qml_create_node"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_framebuffer,
			 "fr_framebuffer",
			 { "mainloop", "format", "width", "height", "depth" },
			 { "FML", "int", "int", "int", "int" },
			 { "", "4", "128", "144", "0" },
			 "FBU", "low_frame_api", "low_framebuffer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::low_framebuffer_run,
			 "fr_run",
			 { "ev", "buffer", "mode", "scr_x", "scr_y" },
			 { "EveryApi&", "FBU", "int", "int", "int" },
			 { "ev", "", "0", "128", "144" },
			 "RUN", "low_frame_api", "low_framebuffer_run"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_root,
			 "w_root",
			 { "ev", "wd" },
			 { "EveryApi&", "W" },
			 { "ev", "" },
			 "FML", "low_frame_api", "w_root"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_layout,
			 "w_layout",
			 { "vec", "url" },
			 { "[W]", "std::string" },
			 { "", "http://tpgames.org/testlayout.lay" },
			 "W", "low_frame_api", "w_layout"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_rect,
			 "w_rect",
			 { "color" },
			 { "unsigned int" },
			 { "ffffffff" },
			 "W", "low_frame_api", "w_rect"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::low_frame_api, &GameApi::LowFrameBufferApi::w_bitmap,
			 "w_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "W", "low_frame_api", "w_bitmap"));
  return vec;
}
#endif // SECTION_1
#ifdef SECTION_2
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

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::texture_sbm,
			 "sbm_many_texture",
			 { },
			 { },
			 { },
			 "SBM", "polygon_api", "texture_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sbm_circle,
			 "sbm_circle",
			 { "start_time", "end_time", "start_pos_x", "start_pos_y", "end_pos_x", "end_pos_y", "start_radius", "end_radius" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "0.0", "0.0", "100.0", "120.0" },
			 "SBM", "polygon_api", "sbm_circle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::blur_sbm,
			 "sbm_blur",
			 { "texture", "pixel_x", "pixel_y" },
			 { "SBM", "float", "float" },
			 { "", "0.01", "0.01" },
			 "SBM", "polygon_api", "blur_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::bloom_cut_sbm,
			 "sbm_bloom_cut",
			 { "texture","r_cut", "g_cut", "b_cut" },
			 { "SBM", "float", "float", "float" },
			 { "", "0.7", "0.7", "0.7" },
			 "SBM", "polygon_api", "bloom_cut_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::combine_sbm,
			 "sbm_combine",
			 { "texture1", "texture2" },
			 { "SBM", "SBM" },
			 { "", "" },
			 "SBM", "polygon_api", "combine_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sfo_sbm,
			 "sfo_sbm",
			 { "ev", "sfo" },
			 { "EveryApi&", "SFO" },
			 { "ev", "" },
			 "SBM", "polygon_api", "sfo_sbm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sbm_texture,
			 "p_sbm",
			 { "ev", "mainloop", "bitmap" },
			 { "EveryApi&", "ML", "SBM" },
			 { "ev", "", "" },
			 "ML", "polygon_api", "sbm_texture"));


  return vec;

}
#endif // SECTION_2
#endif
#ifdef THIRD_PART
#ifdef SECTION_1
std::vector<GameApiItem*> linesapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::point_array,
			 "li_points",
			 { "arr" },
			 { "[PT]" },
			 { "" },
			 "LI", "lines_api", "point_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_from_pts,
			 "li_from_pts",
			 { "pts1", "dx", "dy", "dz" },
			 { "PTS", "float", "float", "float" },
			 { "", "0.0","0.01","0.0" },
			 "LI", "points_api", "li_from_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_pts,
			 "li_to_pts",
			 { "li", "pos" },
			 { "LI", "float" },
			 { "", "0.0" },
			 "PTS", "points_api", "li_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::li_pts2,
			 "li_to_pts2",
			 { "li" },
			 { "LI" },
			 { "" },
			 "PTS", "points_api", "li_pts2"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::fur,
			 "li_fur",
			 { "pts", "center", "dist" },
			 { "PTS", "PT", "float" },
			 { "", "", "10.0" },
			 "LI", "lines_api", "fur"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::random_angle,
			 "li_rand_angle",
			 { "lines", "max_angle" },
			 { "LI", "float" },
			 { "", "0.3" },
			 "LI", "lines_api", "random_angle"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::random_mesh_quad_lines,
			 "li_from_quads",
			 { "ev", "p", "count" },
			 { "EveryApi&", "P", "int" },
			 { "ev", "", "1000" },
			 "LI", "lines_api", "random_mesh_quad_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::lines_from_quads,
			 "li_grid_from_quads",
			 { "p", "sx", "sy" },
			 { "P", "int", "int" },
			 { "", "5", "5" },
			 "LI", "lines_api", "lines_from_quads"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::border_from_bool_bitmap,
			 "border_from_bool_bitmap",
			 { "b", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "",   "-300.0", "300.0", "-300.0", "300.0", "0.0" },
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::li_or_elem,
			 "li_or_elem",
			 { "li1", "li2" },
			 { "LI", "LI" },
			 { "", "" },
			 "LI", "lines_api", "li_or_elem"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::line_product,
			 "li_product",
			 { "li1", "li2" },
			 { "LI", "LI" },
			 { "", "" },
			 "P", "lines_api", "line_product"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::split_lines,
			 "li_split",
			 { "li", "dist" },
			 { "LI", "float" },
			 { "", "5.0" },
			 "LI", "lines_api", "split_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::twist_y,
			 "li_twist",
			 { "li", "y_0", "angle_per_y_unit" },
			 { "LI", "float", "float" },
			 { "", "0.0", "0.031459" },
			 "LI", "lines_api", "twist_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::update_ml,
			 "li_update",
			 { "lla", "li" },
			 { "LLA", "LI" },
			 { "", "" },
			 "ML", "lines_api", "update_ml"));


#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::import_ifc,
			 "li_import_icf",
			 { "ev", "url" },
			 { "EveryApi&", "std::string" },
			 { "ev", "http://tpgames.org/" },
			 "LI", "lines_api", "import_ifc"));
#endif
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::circle_xy_wave,
			 "c_circle_wave",
			 { "start_r", "end_r", "wave", "length" },
			 { "float", "float", "WV", "float" },
			 { "200.0", "220.0", "", "6.283" },
			 "C", "curve_api", "circle_xy_wave"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::curve_product,
			 "c_product",
			 { "c1", "c2", "point" },
			 { "C", "C", "PT" },
			 { "", "", "" },
			 "PA", "curve_api", "curve_product"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::curve_group_from_lines,
			 "cg_lines",
			 { "li" },
			 { "LI" },
			 { "" },
			 "CG", "curve_api", "curve_group_from_lines"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::lines_from_curve_group,
			 "cg_to_li",
			 { "curvegroup", "split" },
			 { "CG", "int" },
			 { "", "3" },
			 "LI", "curve_api", "lines_from_curve_group"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::meshanim,
			 "ma_p",
			 { "vec", "start_time", "end_time"},
			 { "[P]", "float", "float" },
			 { "", "0.0", "10.0" },
			 "MA", "polygon_api", "meshanim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::meshanim_mesh,
			 "ma_to_p",
			 { "anim", "time" },
			 { "MA", "float" },
			 { "", "0.0" },
			 "P", "polygon_api", "meshanim_mesh"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::anim,
			 "ma_render",
			 { "ev", "next", "anim", "start_time", "end_time", "count" },
			 { "EveryApi&", "ML", "MA", "float", "float", "int" },
			 { "ev", "", "", "0.0", "10.0", "3" },
			 "ML", "polygon_api", "anim"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::anim_bind,
			 "ma_bind",
			 { "ev", "next", "anim", "material", "start_time", "end_time", "count" },
			 { "EveryApi&", "ML", "MA", "MT", "float", "float", "int" },
			 { "ev", "", "", "", "0.0", "10.0", "3" },
			 "ML", "polygon_api", "anim_bind"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::curve_group_from_anim,
			 "ma_to_cg",
			 { "meshanim", "start_time", "end_time" },
			 { "MA", "float", "float" },
			 { "", "0.0", "100.0" },
			 "CG", "polygon_api", "curve_group_from_anim"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::patch_sample,
			 "pa_sample",
			 { "patch", "sx", "sy" },
			 { "PA", "int", "int" },
			 { "", "10", "10" },
			 "P", "curve_api", "patch_sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::x_curve,
			 "pa_x",
			 { "patch", "y" },
			 { "PA", "float" },
			 { "", "0.0" },
			 "C", "curve_api", "x_curve"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::y_curve,
			 "pa_y",
			 { "patch", "x" },
			 { "PA", "float" },
			 { "", "0.0" },
			 "C", "curve_api", "y_curve"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::xy_sum,
			 "cpp_sum",
			 { },
			 { },
			 { },
			 "CPP", "curve_api", "xy_sum"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::curve_api, &GameApi::CurveApi::xy_sum2,
			 "cpp_sum2",
			 { "xmult", "ymult", "zmult" },
			 { "float", "float", "float" },
			 { "1.0", "1.0", "1.0" },
			 "CPP", "curve_api", "xy_sum2"));

  return vec;
}
#endif // SECTION_1
#ifdef SECTION_2
 std::vector<GameApiItem*> pointsapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pt_array,
			 "pt_array",
			 { "ev", "vec" },
			 { "EveryApi&", "[PT]" },
			 { "ev", "" },
			 "PTS", "points_api", "pt_array"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::anim_mix,
			 "anim_mix_pts",
			 { "obj1", "obj2", "start_val", "end_val", "start_time", "end_time" },
			 { "PTS", "PTS", "float", "float", "float", "float" },
			 { "", "", "0.0", "1.0", "10.0", "40.0" },
			 "PTS", "points_api", "anim_mix"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_hide_pts,
			 "score_hide_pts",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "PTS", "mainloop_api", "score_hide_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::score_set_hide,
			 "score_set_hide",
			 { },
			 { },
			 { },
			 "IF", "mainloop_api", "score_set_hide"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::sort_pts,
			 "sort_pts",
			 { "points" },
			 { "PTS" },
			 { "" },
			 "PTS", "points_api", "sort_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_x,
			 "rot_x_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_y,
			 "rot_y_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_y"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::rot_z,
			 "rot_z_pts",
			 { "obj", "angle" },
			 { "PTS", "float" },
			 { "", "1.570795" },
			 "PTS", "points_api", "rot_z"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::anim_rot_pts,
			 "anim_rot_pts",
			 { "pts", "start_time", "end_time", "v_x", "v_y", "v_z", "rotate_amount" },
			 { "PTS", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "1.0", "0.0", "6.282" },
			 "PTS", "points_api", "anim_rot_pts"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::polygon_face_center_points,
			 "face_center",
			 { "p" },
			 { "P" },
			 { "" },
			 "PTS", "points_api", "polygon_face_center_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::hemisphere_points,
			 "hemisphere_pts",
			 { "points", "normal", "r", "numpoints" },
			 { "PT", "V", "float", "int" },
			 { "", "", "100.0", "100" },
			 "PTS", "points_api", "hemisphere_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::wave_points,
			 "wave_pts",
			 { "wave", "num_samples", "pos_x", "pos_y", "pos_z",
			     "u_x_x", "u_x_y", "u_x_z",
			     "u_y_x", "u_y_y", "u_y_z" },
			 { "WV", "int", "float", "float", "float",
			     "float", "float", "float",
			     "float", "float", "float" },
			 { "", "50", "-300.0", "-300.0", "0.0", "600.0", "0.0", "0.0", "0.0", "600.0", "0.0" },
			 "PTS", "points_api", "wave_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::filter_component,
			 "filter_pts",
			 { "pts", "comp", "val" },
			 { "PTS", "int", "float" },
			 { "", "2", "0.0" },
			 "PTS", "points_api", "filter_component"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_bitmap_instancing,
			 "random_bitmap",
			 { "ev", "bb", "count", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "EveryApi&", "BB", "int", "float", "float", "float", "float", "float" },
			 { "ev", "", "300", "-300.0", "300.0", "-300.0", "300.0", "0.0"},
			 "PTS", "points_api", "random_bitmap_instancing"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::random_mesh_quad_instancing,
			 "random_quad",
			 { "ev", "p", "count" },
			 { "EveryApi&", "P", "int" },
			 { "ev", "", "300" },
			 "PTS", "points_api", "random_mesh_quad_instancing"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::random_vol_object,
			 "random_volume",
			 { "o", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "numpoints" },
			 { "O", "float", "float", "float", "float", "float", "float", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0", "10000" },
			 "PTS", "volume_api", "random_vol_object"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_grid,
			 "pts_grid",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "PTS", "points_api", "pts_grid"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::pts_grid_bb,
			 "pts_grid_bb",
			 { "bb", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "", "-200.0", "200.0", "-200.0", "200.0", "0.0" },
			 "PTS", "points_api", "pts_grid_bb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::memoize_pts,
			 "pts_memoize",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "PTS", "points_api", "memoize_pts"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_points,
			 "pts_bounding_box",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "-300.0", "300.0", "-300.0", "300.0", "-300.0", "300.0" },
			 "PTS", "points_api", "collision_points"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_bind,
			 "pts_collision_bind",
			 { "ev","bounding_box", "name" },
			 { "EveryApi&","PTS", "std::string" },
			 { "ev", "", "obj1" },
			 "ML", "points_api", "collision_bind"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_bind_inst,
			 "pts_coll_bind_inst",
			 { "ev","bounding_box", "inst_points", "name" },
			 { "EveryApi&","PTS", "PTS", "std::string" },
			 { "ev", "", "", "obj1" },
			 "ML", "points_api", "collision_bind_inst"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::collision_collect,
			 "collidion_collect",
			 { "mainloop" },
			 { "ML" },
			 { "" },
			 "ML", "points_api", "collision_collect"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_seq_ml,
			 "collision_seq_ml",
			 { "curr", "end", "obj1", "obj2", "show_duration" },
			 { "ML", "ML", "std::string", "std::string", "float" },
			 { "", "", "obj1", "obj2", "100.0" },
			 "ML", "mainloop_api", "collision_seq_ml"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::mainloop_api, &GameApi::MainLoopApi::collision_gen_key,
			 "collision_gen_key",
			 { "curr", "obj1", "obj2", "key", "keypress_duration" },
			 { "ML", "std::string", "std::string", "int", "float" },
			 { "", "obj1", "obj2", "32", "10.0" },
			 "ML", "mainloop_api", "collision_gen_key"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_points,
			 "ms_from_points",
			 { "pts" },
			 { "PTS" },
			 { "" },
			 "MS", "matrices_api", "from_points"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::mult_array,
			 "ms_mult",
			 { "m1", "m2" },
			 { "MS", "MS" },
			 { "","" },
			 "MS", "matrices_api", "mult_array"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::ms_random_rot,
			 "ms_random_rot",
			 { "px", "py", "pz", "count" },
			 { "float", "float", "float", "int" },
			 { "0.0", "1.0", "0.0", "30" },
			 "MS", "matrices_api", "ms_random_rot"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::subarray,
			 "ms_subarray",
			 { "ms", "start", "count" },
			 { "MS", "int", "int" },
			 { "", "0", "0" },
			 "MS", "matrices_api", "subarray"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::matrices_api, &GameApi::MatricesApi::from_lines_2d,
			 "ms_from_lines_2d",
			 { "li" },
			 { "LI" },
			 { "" },
			 "MS", "matrices_api", "from_lines_2d"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::static_instancing_matrix,
			 "ms_static_inst",
			 { "ev", "obj", "matrices" },
			 { "EveryApi&", "P", "MS" },
			 { "ev", "", "" },
			 "P", "polygon_api", "static_instancing_matrix"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::li_static_instancing_matrix,
			 "ms_lines_inst",
			 { "ev", "obj", "matrices" },
			 { "EveryApi&", "LI", "MS" },
			 { "ev", "", "" },
			 "LI", "polygon_api", "li_static_instancing_matrix"));
  return vec;
}
#endif // SECTION_2
#ifdef SECTION_3
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient_fb,
			 "gradient_fb",
			 { "sx", "sy", "val", "val2", "flip" },
			 { "int", "int", "float", "float", "bool" },
			 { "100", "100", "0.0", "1.0", "false" },
			 "FB", "bitmap_api", "gradient_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::radial_fb,
			 "radial_fb",
			 { "sx", "sy", "x", "y", "r", "val_at_zero", "val_at_r" },
			 { "int", "int", "float", "float", "float", "float", "float" },
			 { "100", "100", "50", "50", "50", "1.0", "0.0" },
			 "FB", "bitmap_api", "radial_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::sin_fb,
			 "sin_fb",
			 { "gradient" },
			 { "FB" },
			 { "" },
			 "FB", "bitmap_api", "sin_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::plus_fb,
			 "plus_fb",
			 { "f1", "f2" },
			 { "FB", "FB" },
			 { "","" },
			 "FB", "bitmap_api", "plus_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::mul_fb,
			 "mul_fb",
			 { "f", "mul" },
			 { "FB", "float" },
			 { "", "1.0" },
			 "FB", "bitmap_api", "mul_fb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::dist,
			 "bm_dist",
			 { "sx", "sy", "p_x", "p_y" },
			 { "int", "int", "float", "float" },
			 { "100", "100", "50.0", "50.0" },
			 "FB", "float_bitmap_api", "dist"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::blur_bitmap,
			 "bm_blur",
			 { "fb", "d" },
			 { "FB", "float" },
			 { "", "0.5" },
			 "FB", "float_bitmap_api", "blur_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::gaussian,
			 "bm_gaussian",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "sx", "sy" },
			 { "float", "float", "float", "float", "float", "float", "int", "int" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "1.0", "100", "100" },
			 "FB", "float_bitmap_api", "gaussian"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::perlin_noise,
			 "bm_perlin",
			 { "grad_1", "grad_2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB", "float_bitmap_api", "perlin_noise"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::to_grayscale_color,
			 "to_grayscale_color",
			 { "fb", "r", "g", "b", "a", "r2", "g2", "b2", "a2" },
			 { "FB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM", "float_bitmap_api", "to_grayscale_color"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::choose_bitmap,
			 "fb_choose_bitmap",
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
			 { "fb", "max_value" },
			 { "FB", "float" },
			 { "", "100.0" },
			 "FB", "float_bitmap_api", "distance_field"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::add_border,
			 "add_border",
			 { "fb" },
			 { "FB" },
			 { "" },
			 "FB", "float_bitmap_api", "add_border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_loader,
			 "load_map",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/examplemap.txt" },
			 "IBM", "bitmap_api", "intbitmap_loader"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::intbitmap_bm,
			 "ibm_to_bm",
			 { "ibm" },
			 { "IBM" },
			 { "" },
			 "BM", "bitmap_api", "intbitmap_bm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_fb_to_ibm_bitmap,
			 "fb_to_ibm",
			 { "fb", "start", "d" },
			 { "FB", "float", "float" },
			 { "", "0.0", "0.3" },
			 "IBM", "bitmap_api", "convert_fb_to_ibm_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::convert_ibm_to_vx,
			 "ibm_to_vx",
			 { "bm" },
			 { "IBM" },
			 { "" },
			 "VX", "bitmap_api", "convert_ibm_to_vx"));

  return vec;
}
#endif // SECTION_3
#ifdef SECTION_1
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::tri,
			 "tri",
			 { "orig", "p1_x", "p1_y", "p2_x", "p2_y", "p3_x", "p3_y" },
			 { "BB", "float", "float", "float", "float", "float", "float" },
			 { "", "0", "0", "100", "30", "50", "100" },
			 "BB", "bool_bitmap_api", "tri"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::rings,
			 "rings",
			 { "sx", "sy", "center_x_start", "center_y_start", "center_x_end", "center_y_end", "start_radius", "end_radius", "start_thickness", "end_thickness", "numrings" },
			 { "int", "int", "float", "float", "float", "float", "float", "float", "float", "float", "int" },
			 { "300", "300", "150", "150", "150", "150", "10", "180", "30", "30", "20" },
			 "BB", "bool_bitmap_api", "rings"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::black_white_dithering,
			 "fb_dither",
			 { "float_bitmap" },
			 { "FB" },
			 { "" },
			 "BB", "bool_bitmap_api", "black_white_dithering"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::create_ibm,
			 "bb_ibm",
			 { "vec" },
			 { "[BB]" },
			 { "" },
			 "IBM", "bitmap_api", "create_ibm"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_bool,
			 "ibm_bb",
			 { "bm", "val" },
			 { "IBM", "int" },
			 { "", "1" },
			 "BB", "bitmap_api", "choose_bool"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_ints,
			 "ibm_bbs",
			 { "bm", "count" },
			 { "IBM", "int" },
			 { "", "2" },
			 "[BB]", "bitmap_api", "choose_ints"));
  return vec;
}
#endif // SECTION_1
#ifdef SECTION_2
 
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::save_png_ml,
			 "save_png",
			 { "ev", "bm", "filename" },
			 { "EveryApi&", "BM", "std::string" },
			 { "ev", "", "test.png" },
			 "ML", "bitmap_api", "save_png_ml"));
#if 1
  // doesnt work in emscripten, all solutions seem to fail miserably,
  // with emscripten_async_wget didn't work fine.
  // and emscripten_wget had simlar problems
  // also it would make emscripten build alot larger for no good reason
  // when you had to use -s ASYNCIFY and other flags
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::loadbitmapfromurl,
			 "bm_url",
			 { "url" },
			 { "std::string" },
			 { "http://tpgames.org/gameapi_logo.png" },
			 "BM", "bitmap_api", "loadbitmapfromurl"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::dyn_fetch_bitmap,
			 "bm_fetch",
			 { "ev", "url", "reload_frame" },
			 { "EveryApi&", "std::string", "int" },
			 { "ev", "http://tpgames.org/gameapi_logo.png", "300000" },
			 "TXID", "bitmap_api", "dyn_fetch_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_x,
			 "bm_atlas_x",
			 { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_x", "count" },
			 { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int" },
			 { "ev", "", "0", "10", "0", "10", "10", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_x"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_y,
			 "bm_atlas_y",
			 { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_y", "count" },
			 { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int" },
			 { "ev", "", "0", "10", "0", "10", "10", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_y"));
    vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::sprite_atlas_xy,
			 "bm_atlas_xy",
			   { "ev", "orig", "start_x", "end_x", "start_y", "end_y", "delta_x", "delta_y", "count_x", "count_y" },
			   { "EveryApi&", "BM", "int", "int", "int", "int", "int", "int", "int", "int" },
			   { "ev", "", "0", "10", "0", "10", "10", "10", "2", "2" },
			 "[BM]", "sprite_api", "sprite_atlas_xy"));
    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::cubemap,
			   "bm_cubemap",
			   { "bm" },
			   { "BM" },
			   { "" },
			   "[BM]", "bitmap_api", "cubemap"));

    vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::fix_edges,
			   "edge_fix",
			   { "bm" },
			   { "BM" },
			   { "" },
			   "BM", "bitmap_api", "fix_edges"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::border,
			 "border",
			 { "bm", "left", "right", "top", "bottom" },
			 { "BM", "int", "int", "int", "int" },
			 { "", "5", "5", "5", "5" },
			 "BM", "bitmap_api", "border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::add_shape_border,
			 "add_shape_border",
			 { "ev", "bm", "dist_field_size", "start_range", "end_range", "r", "g", "b", "a", "border_size" },
			 { "EveryApi&", "BM", "float", "float", "float", "int", "int", "int", "int", "int" },
			 { "ev", "", "7.0", "0.5", "0.9", "0", "0", "0", "255", "5" },
			 "BM", "bitmap_api", "add_shape_border"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::choose_color,
			 "bm_choose_color",
			 { "bm", "c1", "c2" },
			 { "BM", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "00888888" },
			 "BB", "bitmap_api", "choose_color"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::FB))&GameApi::BitmapApi::blitbitmap_fb,
			 "blitFB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "FB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap_fb"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::BB))&GameApi::BitmapApi::blitbitmap_bb,
			 "blitBB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "BB" },
			 { "", "", "0", "0", "" },
			 "BM", "bitmap_api", "blitbitmap_bb"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_bitmap,
			 "bm_scale",
			 { "ev", "orig", "sx", "sy" },
			 { "EveryApi&", "BM", "int", "int" },
			 { "ev", "", "800", "600" },
			 "BM", "bitmap_api", "scale_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::scale_bitmap_fullscreen,
			 "bm_fullscreen",
			 { "ev", "orig" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "BM", "bitmap_api", "scale_bitmap_fullscreen"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::interpolate_bitmap,
			 "interpolate",
			 { "orig1", "orig2", "x" },
			 { "BM", "BM", "float" },
			 { "", "", "0.0" },
			 "BM", "bitmap_api", "interpolate_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::color_bm,
			 "bm_color",
			 { "bm", "color" },
			 { "BM", "unsigned int" },
			 { "", "fff8844" },
			 "BM", "bitmap_api", "color_bm"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::plus_bitmap,
			 "bm_plus",
			 { "bm1", "bm2" },
			 { "BM", "BM" },
			 { "", "" },
			 "BM", "bitmap_api", "plus_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::median_filter,
			 "bm_medianfilter",
			 { "bm", "sx", "sy" },
			 { "BM", "int", "int" },
			 { "", "1", "1" },
			 "BM", "bitmap_api", "median_filter"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::rot90,
			 "rot90",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "rot90"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::Indicator,
			 "indicator",
			 { "sx", "sy", "g_ind" },
			 { "int", "int", "int" },
			 { "256", "256", "0" },
			 "BM", "bitmap_api", "Indicator"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::noise_vectors,
			 "noise_vec",
			 { "sx", "sy" },
			 { "int", "int" },
			 { "256", "256" },
			 "BM", "bitmap_api", "noise_vectors"));
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
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::mandelbrot,
			 "mandelbrot",
			 { "is_julia", "start_x", "end_x", "start_y", "end_y", "xx", "yy", "sx", "sy", "count" },
			 { "bool", "float", "float", "float", "float", "float", "float", "int", "int", "int" },
			 { "false", "-2.0", "1.0", "-1.0", "1.0", "0.0", "0.0", "256", "256", "64" },
			 "BM", "bitmap_api", "mandelbrot"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::simple_blur,
			 "blur",
			 { "bm", "center", "left", "right", "top", "bottom" },
			 { "BM", "float", "float", "float", "float", "float" },
			 { "", "0.5", "0.125", "0.125", "0.125", "0.125" },
			 "BM", "bitmap_api", "simple_blur"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::bump_map,
			 "bm_bumpmap",
			 { "fb", "h" },
			 { "FB", "float" },
			 { "", "10.0" },
			 "BM", "bitmap_api", "bump_map"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::memoize,
			 "bm_memoize",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "BM", "bitmap_api", "memoize"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap3,
			 "bm_world_from_bitmap",
			 { "ev", "v", "url", "chars", "dx", "dy" },
			 { "EveryApi&", "[BM]", "std::string", "std::string", "int", "int" },
			 { "ev", "", "http://tpgames.org/map.txt", ".#", "100", "100" },
			 "BM", "bitmap_api", "world_from_bitmap3"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::persistent_cache,
			 "bm_persistent_cache",
			 { "bm", "filename" },
			 { "BM", "std::string" },
			 { "", "cache.ppm" },
			 "BM", "bitmap_api", "persistent_cache"));
#if 0 
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
			 { "ev", "bm" },
			 { "EveryApi&", "BM" },
			 { "ev", "" },
			 "ML", "sprite_api", "render_sprite_vertex_array_ml"));
#endif


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
  vec.push_back(ApiItemF(&GameApi::EveryApi::cont_bitmap_api, &GameApi::ContinuousBitmapApi::bicubic,
			 "cbm_bicubic",
			 { "f_0", "f_1", "df_0", "df_1", "ff_0", "ff_1", "dff_0", "dff_1" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.5", "-0.5", "1.0", "1.0", "0.5", "-0.5" },
			 "CBM", "cont_bitmap_api", "bicubic"));

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

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::s_spherical,
			 "s_spherical",
			 { "c_x", "c_y", "c_z", "fb", "start_radius", "end_radius", "start_values", "end_values" },
			 { "float", "float", "float", "FB", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "", "300", "350", "0.0", "1.0" },
			 "S", "polygon_api", "s_spherical"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::surface_api, &GameApi::SurfaceApi::texture,
			 "s_bitmap",
			 { "orig", "texture" },
			 { "S", "BM" },
			 { "", "" },
			 "S", "surface_api", "texture"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::s_sample,
			 "s_sample",
			 { "surf", "sx", "sy" },
			 { "S", "int", "int" },
			 { "", "300", "300" },
			 "P", "polygon_api", "s_sample"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::C_bitmap,
			 "cfb_c",
			 { "sx", "sy", "C" },
			 { "float","float", "float" },
			 { "100.0", "100.0", "1.0" },
			 "CFB", "float_bitmap_api", "C_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::X_bitmap,
			 "cfb_x",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "300.0", "300.0" },
			 "CFB", "float_bitmap_api", "X_bitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Y_bitmap,
			 "cfb_y",
			 { "sx", "sy" },
			 { "float", "float" },
			 { "300.0", "300.0" },
			 "CFB", "float_bitmap_api", "Y_bitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::AddBitmap,
			 "cfb_add",
			 { "vec" },
			 { "[CFB]" },
			 { "" },
			 "CFB", "float_bitmap_api", "AddBitmap"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::MulBitmap,
			 "cfb_mul",
			 { "vec" },
			 { "[CFB]" },
			 { "" },
			 "CFB", "float_bitmap_api", "MulBitmap"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SqrtContFloat,
			 "cfb_sqrt",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "SqrtContFloat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Sin,
			 "cfb_sin",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Sin"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Cos,
			 "cfb_cos",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Cos"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Tan,
			 "cfb_tan",
			 { "val" },
			 { "CFB" },
			 { "" },
			 "CFB", "float_bitmap_api", "Tan"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SampleContFloat,
			 "cfb_sample",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "sx", "sy", "mult" },
			 { "CFB", "float", "float", "float", "float", "int", "int", "float" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "100", "100", "1.0" },
			 "FB", "float_bitmap_api", "SampleContFloat"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::Equalizer,
			 "cfb_equalizer",
			 { "a1", "a2" },
			 { "CFB", "CFB" },
			 { "", "" },
			 "CBB", "float_bitmap_api", "Equalizer"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::SampleContBool,
			 "cbb_sample",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "sx", "sy" },
			 { "CBB", "float", "float", "float", "float", "int", "int" },
			 { "", "-300.0", "300.0", "-300.0", "300.0", "100", "100" },
			 "BB", "float_bitmap_api", "SampleContBool"));
  
  return vec;
}
#endif // SECTION_2
#ifdef SECTION_3
 
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
EXPORT std::vector<GameApiItem*> global_functions;
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
  std::vector<GameApiItem*> vi = polydistfield_functions();
  std::vector<GameApiItem*> vj = waveform_functions();
  std::vector<GameApiItem*> vk = blocker_functions();
  std::vector<GameApiItem*> vl = framebuffermoduleapi_functions();
  
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
  std::vector<GameApiItem*> ah = append_vectors(ag, vi);
  std::vector<GameApiItem*> ai = append_vectors(ah, vj);
  std::vector<GameApiItem*> aj = append_vectors(ai, vk);
  std::vector<GameApiItem*> ak = append_vectors(aj, vl);
  return ak;
}
#endif // SECTION_3
#ifdef SECTION_1
EXPORT std::string GameApi::GuiApi::bitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = bitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::waveformapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = waveform_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::blockerapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = blocker_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::textureapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = textureapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::fontapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = fontapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::shadermoduleapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = shadermoduleapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::framebuffermoduleapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = framebuffermoduleapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::pointsapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = pointsapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::linesapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = linesapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::polygonapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = polygonapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::polygondistapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = polydistfield_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::volumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = volumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::floatvolumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = floatvolumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::colorvolumeapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = colorvolumeapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}


EXPORT std::string GameApi::GuiApi::boolbitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = boolbitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::floatbitmapapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = floatbitmapapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::pointapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = pointapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::vectorapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = vectorapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}

EXPORT std::string GameApi::GuiApi::booleanopsapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = booleanopsapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}
EXPORT std::string GameApi::GuiApi::moveapi_functions_item_label(int i)
{
  std::vector<GameApiItem*> funcs = moveapi_functions();
  GameApiItem *item = funcs[i];
  std::string name = item->Name(0);
  return name;
}




EXPORT GameApi::W functions_widget(GameApi::GuiApi &gui, std::string label, std::vector<GameApiItem*> vec, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::FtA atlas2, GameApi::BM atlas_bm2, GameApi::W insert)
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
  GameApi::W w = gui.list_item_opened(140-5, label, atlas, atlas_bm, vec2, vec3, atlas2, atlas_bm2, insert);
  GameApi::W w2 = gui.list_item_title(140-5, label, atlas, atlas_bm);
  GameApi::W or_elem = gui.or_elem(w,w2);
  return or_elem;
}



EXPORT GameApi::W GameApi::GuiApi::bitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BitmapApi", bitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::waveformapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "WaveformApi", waveform_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::blockerapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "MainLoopApi", blocker_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::textureapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "TextureApi", textureapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::booleanopsapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BooleanOps", booleanopsapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::moveapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "MoveApi", moveapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}



EXPORT GameApi::W GameApi::GuiApi::fontapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FontApi", fontapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::volumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "VolumeApi", volumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::floatvolumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FVolumeApi", floatvolumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::colorvolumeapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "CVolumeApi", colorvolumeapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}


EXPORT GameApi::W GameApi::GuiApi::shadermoduleapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "ShaderApi", shadermoduleapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::framebuffermoduleapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FrameBufferApi", framebuffermoduleapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::linesapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "LinesApi", linesapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::pointsapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PointsApi", pointsapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::pointapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PointApi", pointapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::vectorapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "VectorApi", vectorapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::boolbitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "BoolBitmap", boolbitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::floatbitmapapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "FloatBitmap", floatbitmapapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

EXPORT GameApi::W GameApi::GuiApi::polygonapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PolygonApi", polygonapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}
EXPORT GameApi::W GameApi::GuiApi::polygondistapi_functions_list_item(FtA atlas1, BM atlas_bm1, FtA atlas2, BM atlas_bm2, W insert)
{
  return functions_widget(*this, "PolyDistApi", polydistfield_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
}

class InsertWidget : public GuiWidgetForward
{
public:
  InsertWidget(GameApi::EveryApi &ev,GuiWidget *w, std::function<void(int,int)> f) : GuiWidgetForward(ev, { w }), f(f) { activated = true;  state=false; }
  void activate() { activated = true; }
  void deactivate() { activated = false; }
  void update(Point2d mouse_pos, int button,int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse_pos, button,ch, type, mouse_wheel_y);
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
    if (is_visible())
      {
    if (activated)
      {
	vec[0]->render();
      }
      }
  }
private:
  bool activated;
  bool state;
  std::function<void(int,int)> f;
};

EXPORT GameApi::W GameApi::GuiApi::insert_widget(W item, std::function<void(int,int)> f)
{
  GuiWidget *w = find_widget(e, item);
  return add_widget(e, new InsertWidget(ev,w,f));
}

EXPORT void GameApi::GuiApi::delete_widget(W w)
{
  GuiWidget *ww = new EmptyWidget(ev);
  add_update_widget(e, w, ww);
}

std::vector<std::pair<std::string,std::string> > GameApi::GuiApi::get_functions_mapping()
{
  std::vector<std::pair<std::string,std::string> > vec;

  std::vector<GameApiItem*> funcs = all_functions();
  int s = funcs.size();
  for(int i=0;i<s;i++)
    {
      GameApiItem *i2 = funcs[i];
      std::string apiname = i2->ApiName(0);
      std::string funcname = i2->FuncName(0);
      std::string name = i2->Name(0);
      vec.push_back(std::make_pair(apiname + "." + funcname,name));
    }
  return vec;
}

std::string unique_id_apiitem()
{
  static int id = 0;
  id++;
  std::stringstream ss;
  ss << id;
  return std::string("I") + ss.str();
}

#endif // SECTION_1
#endif

#ifdef FIRST_PART
#ifdef SECTION_3


int find_float_ch(std::string s, char ch) {
  int ss = s.size();
  int level = 0;
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='(') level++;
      if (s[i]==')') level--;
      if (level==0 && s[i]==ch) return i;
    }
  return -1;
}

std::string FloatExprEval(std::string s)
{
  //std::cout << "FloatExprEval: " << s << std::endl;
  if (s.size()>0 && s[0]==' ') return FloatExprEval(s.substr(1,s.size()-1));
  if (s.size()>0 && s[s.size()-1]==' ') return FloatExprEval(s.substr(0,s.size()-1));
  if (s.size()>2 && s[0]=='(' && s[s.size()-1]==')')
    {
      return FloatExprEval(s.substr(1,s.size()-2));
    }
  int vala = find_float_ch(s, '+');
  if (vala!=-1) {
    std::string start = s.substr(0,vala);
    std::string end = s.substr(vala+1);
    std::string res1 = FloatExprEval(start);
    std::string res2 = FloatExprEval(end);
    std::stringstream ss(res1);
    std::stringstream ss2(res2);
    float val1=0.0, val2=0.0;
    ss >> val1;
    ss2 >> val2;
    std::stringstream res;
    res << val1+val2;
    //std::cout << "FloatExprEval(result): " << res.str() << std::endl;

    return res.str();
  }
  int valb = find_float_ch(s, '-');
  if (valb!=-1) {
    std::string start = s.substr(0,valb);
    std::string end = s.substr(valb+1);
    std::string res1 = FloatExprEval(start);
    std::string res2 = FloatExprEval(end);
    std::stringstream ss(res1);
    std::stringstream ss2(res2);
    float val1=0.0, val2=0.0;
    ss >> val1;
    ss2 >> val2;
    std::stringstream res;
    res << val1-val2;
    //std::cout << "FloatExprEval(result): " << res.str() << std::endl;
    return res.str();
  }

  int valc = find_float_ch(s, '*');
  if (valc!=-1) {
    std::string start = s.substr(0,valc);
    std::string end = s.substr(valc+1);
    std::string res1 = FloatExprEval(start);
    std::string res2 = FloatExprEval(end);
    std::stringstream ss(res1);
    std::stringstream ss2(res2);
    float val1=0.0, val2=0.0;
    ss >> val1;
    ss2 >> val2;
    std::stringstream res;
    res << val1*val2;
    //std::cout << "FloatExprEval(result): " << res.str() << std::endl;
    return res.str();
  }

  int vald = find_float_ch(s, '/');
  if (vald!=-1) {
    std::string start = s.substr(0,vald);
    std::string end = s.substr(vald+1);
    std::string res1 = FloatExprEval(start);
    std::string res2 = FloatExprEval(end);
    std::stringstream ss(res1);
    std::stringstream ss2(res2);
    float val1=0.0, val2=0.0;
    ss >> val1;
    ss2 >> val2;
    std::stringstream res;
    res << val1/val2;
    //std::cout << "FloatExprEval(result): " << res.str() << std::endl;
    return res.str();
  }

  //std::cout << "FloatExprEval(result): " << s << std::endl;
  return s;

}
#endif
#endif
