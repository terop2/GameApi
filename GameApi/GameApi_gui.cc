
#include "GameApi_h.hh"
#include "GameApi_gui.hh"
#include "GameApi_cmd.hh"





#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

extern int g_event_screen_x;
extern int g_event_screen_y;

#if defined(ARM) || defined(RASPI) 
#ifdef FIRST_PART
int strlen(const char *ptr) { const char *p = ptr; while(*p) { p++;  } return p-ptr;}
#else
int strlen(const char *);
#endif
#endif
int ret_type_count(std::string return_type);

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
    //std::cout << "TextGuiWidgetAtlas: " << atlas.id << " " << atlas_bm.id << std::endl;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2); 
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    if (firsttime)
      {
	int c = get_current_block();
	set_current_block(-1);
	rendered_bitmap = ev.font_api.font_string_from_atlas(ev, atlas, atlas_bm, label.c_str(), x_gap);
	
	GameApi::CBM sca = ev.cont_bitmap_api.from_bitmap(rendered_bitmap, 1.0, 1.0);
	//int sx = ev.bitmap_api.size_x(rendered_bitmap);
	//int sy = ev.bitmap_api.size_y(rendered_bitmap);
	//scaled_bitmap = ev.cont_bitmap_api.sample(sca, sx/2, sy/2);
	
	int sx = ev.bitmap_api.size_x(rendered_bitmap);
	int sy = ev.bitmap_api.size_y(rendered_bitmap);
	scaled_bitmap = rendered_bitmap; //ev.bitmap_api.scale_bitmap(ev,rendered_bitmap, sx/2,sy/2);
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
	set_current_block(c);
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
    OpenglLowApi *ogl = g_low->ogl;
    ogl->glAlphaFunc(Low_GL_LESS, 0.9);
    ogl->glEnable(Low_GL_ALPHA_TEST);
	ev.shader_api.set_var(sh, "in_MV", ev.matrix_api.trans(p.x,p.y,0.0));
	ev.sprite_api.render_sprite_vertex_array(rendered_bitmap_va);
	ogl->glDisable(Low_GL_ALPHA_TEST);
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
	scaled_bitmap = rendered_bitmap; //ev.cont_bitmap_api.sample(cbm, sx/2, sy/2);
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
    ctrl=false;
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
    if (type==768 && (ch==1073742049||ch==1073742053)) { shift=true; }
    if (type==769 && (ch==1073742049||ch==1073742053)) { shift=false; }
    if (type==768 && (ch==1073742048||ch==1073742052)) { ctrl = true; }
    if (type==769 && (ch==1073742048||ch==1073742052)) { ctrl = false; }

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
    if (active && ctrl && type==768 && ch==118) // ctrl-v
      {
	char *buf = g_low->sdl->SDL_GetClipboardText();
	std::string buf2 = buf;
	int s = buf2.size();
	std::string buf3;
	for(int i=0;i<s;i++) {
	  int s2 = allowed_chars.size();
	  bool insert = false;
	  for(int j=0;j<s2;j++) {
	    if (allowed_chars[j]==buf2[i]) { insert=true; break; }
	  }
	  if (insert) buf3.push_back(buf2[i]);
	}
			     
	label = buf3;
	changed=true;
      }
    if (active && ctrl && type==768 && ch==99) {
      g_low->sdl->SDL_SetClipboardText(label.c_str());
      changed=true;
    }
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
    if (active && type==768 && !changed)
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
	scaled_bitmap = rendered_bitmap; //ev.cont_bitmap_api.sample(cbm, sx/2, sy/2);
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
  bool ctrl;
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
  ~PolyGuiWidget() { /*std::cout << "PolyGuiWidget destructor" << std::endl;*/ }
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
    OpenglLowApi *ogl = g_low->ogl;

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
	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}
	ogl->glViewport(pos.x*scale_x, screen_y*scale_y-pos.y*scale_y-sz.dy*scale_y, sz.dx*scale_x, sz.dy*scale_y);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	ogl->glEnable(Low_GL_DEPTH_TEST);
	obj.render();
	ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	ogl->glViewport(0,0,screen_x*scale_x, screen_y*scale_y);
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
    OpenglLowApi *ogl = g_low->ogl;
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
	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}
	ogl->glViewport(pos.x*scale_x, screen_y*scale_y-pos.y*scale_y-sz.dy*scale_y, sz.dx*scale_x, sz.dy*scale_y);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
	//obj.render();
	ev.shader_api.set_var(sh, "in_MV", mat);
	ev.polygon_api.render_vertex_array(p);
	ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	ogl->glViewport(0,0,screen_x*scale_x, screen_y*scale_y);
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
    if (type==0x300 || type==0x301 ||type==Low_SDL_JOYAXISMOTION || type==Low_SDL_JOYBALLMOTION)
      {

	//std::cout << "update: " << type << " " << ch << " " << button << std::endl;
	e.type = type;
	e.ch = ch;
	e.button = button;
	e.cursor_pos = ev.point_api.point(mouse.x,mouse.y,0.0);

	// TODO, JOYSTICK AXIS AND BALL EVENTS? (needs changes to widget? :)
	
	static Low_SDL_Joystick *joy1 = g_low->sdl->SDL_JoystickOpen(0);
	unsigned int but1 = g_low->sdl->SDL_JoystickGetButton(joy1, 0);  
	unsigned int but2 = g_low->sdl->SDL_JoystickGetButton(joy1, 1);
	unsigned int but3 = g_low->sdl->SDL_JoystickGetButton(joy1, 2);
	unsigned int but4 = g_low->sdl->SDL_JoystickGetButton(joy1, 3);
	e.joy0_button0 = but1==1;
	e.joy0_button1 = but2==1;
	e.joy0_button2 = but3==1;
	e.joy0_button3 = but4==1;

	// TODO
	e.joy1_button0 = false;
	e.joy1_button1 = false;
	e.joy1_button2 = false;
	e.joy1_button3 = false;
	
	e.joy0_current_axis = -1;
	e.joy0_axis0 = 0;
	e.joy0_axis1 = 0;
	e.joy0_axis2 = 0;
	e.joy0_axis3 = 0;
	e.joy0_axis4 = 0;
	e.joy1_current_axis = -1;
	e.joy1_axis0 = 0;
	e.joy1_axis1 = 0;
	e.joy1_axis2 = 0;
	e.joy1_axis3 = 0;
	e.joy1_axis4 = 0;
	

	e.joy0_ball0 = 0;
	e.joy0_ball1 = 0;
	e.joy1_ball0 = 0;
	e.joy1_ball1 = 0;
	// END OF TODO
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
    OpenglLowApi *ogl = g_low->ogl;

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
	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}
	ogl->glViewport(pos.x*scale_x, screen_y*scale_y-pos.y*scale_y-sz.dy*scale_y, sz.dx*scale_x, sz.dy*scale_y);
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
	ogl->glViewport(0,0,screen_x*scale_x, screen_y*scale_y);
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
    OpenglLowApi *ogl = g_low->ogl;
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

	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}

	ogl->glViewport(pos.x*scale_x, screen_y*scale_y-pos.y*scale_y-sz.dy*scale_y, sz.dx*scale_x, sz.dy*scale_y);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	ogl->glEnable(Low_GL_DEPTH_TEST);
	//glPointSize(5.0);
	//ev.mainloop_api.clear_3d();
	obj.render();
	ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	  ogl->glViewport(0,0,screen_x*scale_x, screen_y*scale_y);
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
    OpenglLowApi *ogl = g_low->ogl;
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
	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}

	ogl->glViewport(pos.x*scale_x, screen_y*scale_y-pos.y*scale_y-sz.dy*scale_y, sz.dx*scale_x, sz.dy*scale_y);
	ev.shader_api.use(sh);
	ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
	ogl->glEnable(Low_GL_DEPTH_TEST);
	obj.render();
	ogl->glDisable(Low_GL_DEPTH_TEST);
	ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
	ogl->glViewport(0,0,screen_x*scale_x, screen_y*scale_y);
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
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", 1.0f);
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
	int c = get_current_block();
	set_current_block(-1);
	
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
	set_current_block(c);
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
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", 1.0f);
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


class IconGuiWidgetSharedNoScale : public GuiWidgetForward
{
public:
  IconGuiWidgetSharedNoScale(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::BM bm, GameApi::SH sh, int key) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), env(env), ev(ev), sh(sh), bm(bm), key(key) { firsttime=true;
    Point2d p = { -666.0, -666.0 };
    update(p, -1,-1, -1,0);
    Point2d p2 = { 0.0, 0.0 };
    set_pos(p2);
    //ev.bitmap_api.prepare(bm);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    #if 1
    //BitmapHandle *handle = find_bitmap(env, bm);
    //::Bitmap<Color> *b2 = find_color_bitmap(handle);
    //b2->Prepare();
    size.dx = 400; //ev.bitmap_api.size_x(bm);
    size.dy = 200; //ev.bitmap_api.size_y(bm);
    if (firsttime)
      {
	int c = get_current_block();
	set_current_block(-1);
	
	//std::map<int,int>::iterator i = shared_sprites.find(key);
	int index = find_shared_sprites(key);
	if (index!=-1) {
	  bm_va.id = shared_sprites[index].value;
	  RenderVertexArray *r = find_vertex_array_render(env, bm_va);
	  if (!r) {
	    GameApi::BM scaled = ev.bitmap_api.scale_bitmap(ev,bm, 400,200);
	    bm_va = ev.sprite_api.create_vertex_array(scaled);
	    A a;
	    a.key = key;
	    a.value = bm_va.id;
	    shared_sprites.push_back(a);
	    //std::cout << "shared_sprites " << shared_sprites.size() << " " << bm_va.id << std::endl; 
	    //shared_sprites[key] = bm_va.id;
	  }
	} else {
	  GameApi::BM scaled = ev.bitmap_api.scale_bitmap(ev,bm, 400,200);
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
	set_current_block(c);
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
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", 1.0f);
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
	  GameApi::BB bg = ev.bool_bitmap_api.bb_empty(v.dx, v.dy);
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
	GameApi::BB bg = ev.bool_bitmap_api.bb_empty(v.dx, v.dy);
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
      // std::cout << "ERROR: find_canvas_item failed to find canvas!" << std::endl;
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
      // std::cout << "ERROR: find_canvas_item failed to find widget from canvas!" << std::endl;
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
  //std::cout << "ERROR: find_canvas_item failed to find widget!" << std::endl;
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

extern Low_SDL_Window *sdl_window;

int g_window_pos_x = 0;
int g_window_pos_y = 0;

class WindowMoveWidget : public GuiWidgetForward
{
public:
  WindowMoveWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height) { move_ongoing = false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1, 0);
  }
  void update(Point2d mouse, int button, int ch, int type, int mouse_wheel_y)
  {
    GuiWidgetForward::update(mouse,button,ch, type, mouse_wheel_y);
    if (!move_ongoing && button==0 && type==1025 && mouse.x >= pos.x+area_x && mouse.x < pos.x+area_x+area_width &&
	mouse.y >= pos.y+area_y && mouse.y < pos.y+area_y+area_height)
      {
	move_ongoing = true;
	old_mouse = mouse;
	int px = g_window_pos_x;
	int py = g_window_pos_y;
	//g_low->sdl->SDL_GetWindowPosition(sdl_window, &px, &py);
	//	std::cout << "GOT WINDOW POS:" << px << " " << py << std::endl;
	Point2d p = { float(px), float(py) };
	old_pos = p;
      }
    if (move_ongoing) {
      Vector2d delta = mouse - old_mouse;
      Point2d p = old_pos + delta;
      //g_low->sdl->SDL_SetWindowPosition(sdl_window, p.x, p.y);
      g_window_pos_x = p.x;
      g_window_pos_y = p.y;
      //std::cout << "Set WINDOW POS:" << p.x << " " << p.y << std::endl;
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
  RectangleWidget() = delete;
  RectangleWidget(GameApi::EveryApi &ev, GameApi::SH sh, int start_x, int end_x, int start_y, int end_y, unsigned int color) : GuiWidgetForward(ev, { } ), sh(sh), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), color(color) {
    firsttime = true;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1,0);
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
	GameApi::BB b = ev.bool_bitmap_api.bb_empty(sx,sy);
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

EXPORT GameApi::W GameApi::GuiApi::window_move(W widget, int area_x, int area_y, int area_width, int area_height)
{
  GuiWidget *wid = find_widget(e, widget);
  return add_widget(e, new WindowMoveWidget(ev, wid, area_x, area_y, area_width, area_height));
}

int ret_type_count(std::string return_type)
{
  if (return_type.size()>1&&return_type[0]=='[') return 1; // filter out array rturns
  int s = return_type.size();
  int count = 1;
  for(int i=0;i<s;i++) {
    if (return_type[i]==',') count++;
  }
  return count;
}
std::string ret_type_index(std::string return_type, int index)
{
  int s = return_type.size();
  int count = 0;
  int beg = 0;
  std::string label;
  int i=0;
  for(;i<s;i++) {
    if (return_type[i]==',') {
      label = return_type.substr(beg,i-beg);
      if (count==index) return label;
      count++;
      beg = i+1;
    }
  }
  label = return_type.substr(beg,i-beg);
  return label;
}



int progress_lock=0;

EXPORT GameApi::W GameApi::GuiApi::progress_dialog(int sx, int sy, FtA atlas, BM atlas_bm, std::vector<std::string> vec)
{
  std::string prog = vec[vec.size()-1].substr(0,18);
  std::string rest = vec[vec.size()-1].substr(18);

  
  W txt_0 = text(prog, atlas, atlas_bm);
  W txt_1 = text(rest, atlas, atlas_bm);
  W arr[2] = { txt_0, txt_1 };
  W array_0 = array_y(&arr[0], 2, 5);

  W array_1 = margin(array_0, (sx-size_x(array_0))/2, (sy-size_y(array_0))/2, (sx-size_x(array_0))/2, (sy-size_y(array_0))/2);
  
  //W rect = highlight(400,200);
  
  //vec2.push_back(txt_1);

  //W array_0 = array_y(&vec2[0], vec2.size(), 5);
  //W array_1 = margin(array_0, 5,5,5,5); 
  W rect = button(sx,sy,c_dialog_1, c_dialog_1_2);

  //BM bm = ev.bitmap_api.loadbitmap("progress_v1.ppm");
  //progress_lock = 1;
  //W pic = icon_shared_noscale(bm,0x7777);
  //progress_lock = 0;
  //set_size(pic,sx,sy);
  //W txt_3 = layer(rect, pic);
  W txt_4 = layer(rect, array_1);
  
  return txt_4;
}
extern GameApi::GuiApi *g_everyapi_gui;


void update_progress_dialog_cb_impl(GameApi::W &w, int x,int y, GameApi::FtA f, GameApi::BM b, std::vector<std::string> v)
{
  g_everyapi_gui->update_progress_dialog(w,x,y,f,b,v);
}

void GameApi::GuiApi::update_progress_dialog(W &w, int sx, int sy, FtA atlas, BM atlas_bm, std::vector<std::string> vec)
{
  if (progress_lock) return;
  static int g_id = -1;
  if (g_id!=-1) clear_block(g_id);
  g_id = add_block();
  int old = get_current_block();
  set_current_block(g_id);
  
  w = progress_dialog(sx,sy,atlas,atlas_bm, vec);
  
  set_current_block(old);
}

EXPORT GameApi::W GameApi::GuiApi::canvas_item_gameapi_node(int sx, int sy, std::string label, std::vector<std::string> param_types, std::vector<std::string> param_tooltip, std::string return_type, FtA atlas, BM atlas_bm, std::vector<W *> connect_click, std::string uid, std::vector<W> &params, std::string symbol, std::string comment)
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


  int rs = ret_type_count(return_type);
  std::vector<W> vec_ret;
  int ret_sx=0;
  for(int j=0;j<rs;j++) {
    std::string ret_type = ret_type_index(return_type,j);
    W txt_0 = text(ret_type, atlas,atlas_bm);
    W txt_1 = margin(txt_0, 5,5,5,5);

    int max_width2 = size_x(txt_1);
    W txt_11 = highlight(txt_1);
    W txt_111 = click_area(txt_11, 0.0, 0.0, size_x(txt_11), size_y(txt_11),0);
    set_id(txt_111, uid);
    set_index(txt_111,j);
    set_size2(txt_111,rs);
    *(connect_click[j]) = txt_111; 
    W txt_2 = button(size_x(txt_1), size_y(txt_1), c_canvas_item_text_button, c_canvas_item_text_button2 /*0xff330033, 0xff880088*/);
    W txt_3 = layer(txt_2, txt_111);

    ret_sx = std::max(size_x(txt_3),ret_sx);
    
    vec_ret.push_back(txt_3);
  }
  for(int j=0;j<rs;j++)
    {
      vec_ret[j] = margin(vec_ret[j], ret_sx-size_x(vec_ret[j]),0.0, 0.0, 0.0);
    }
  int max_width2 = ret_sx;
  W array_ret = array_y(&vec_ret[0], vec_ret.size(), 5);
  int ret_sy = size_y(array_ret);

  W array_1 = margin(array, 0, size_y(node_22) + ssy2/2+2, 0, ssy2/2-size_y(node_22)-2);

  
  ssy = std::max(ssy, size_y(array_ret)+size_y(node_22)+5);
  ssy0 = std::max(ssy0, size_y(array_ret));
  ssy2 = ssy0 - std::max(size_y(array), size_y(array_ret));

  

  int ssx_0 = std::max(sx, max_width+4+20+4+max_width2);
  int ssx = std::max(ssx_0,size_x(node_22));
  W node_0 = button(ssx,ssy, c_canvas_item_node_0, c_canvas_item_node_0_2 /*0xffff8844, 0xff884422*/);
  W node_1 = button(ssx,size_y(node_22), c_canvas_item_node_1, c_canvas_item_node_1_2 /*0xff884422, 0xff442211*/);
  W node_12 = highlight(node_1);

  int sy0 = std::max(sy, std::max(size_y(array),size_y(array_ret))+size_y(node_22)+5);
  //W txt_4 = margin(txt_3, ssx-size_x(txt_3), size_y(node_22)+((sy0-size_y(node_22))-size_y(txt_3))/2, 0,((sy0-size_y(node_22))-size_y(txt_3))/2-size_y(node_22));
  W txt_4 = margin(array_ret, ssx-ret_sx, size_y(node_22)+((sy0-size_y(node_22))-ret_sy)/2, 0, ((sy0-size_y(node_22))-ret_sy)/2-size_y(node_22));

  

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
EXPORT GameApi::W GameApi::GuiApi::icon_shared_noscale(BM bm, int key)
{
#ifndef EMSCRIPTEN
  return add_widget(e, new IconGuiWidgetSharedNoScale(e,ev, bm, sh, key));
#else
  W w;
  w.id = 0;
  return w;
#endif

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
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_size_x);
    float scale_y = float(g_event_screen_y)/float(screen_size_y);
    w*=scale_x;
    h*=scale_y;
  }
  W bm_1 = poly(p, sh, w,h, screen_size_x,screen_size_y);
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
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_size_x);
    float scale_y = float(g_event_screen_y)/float(screen_size_y);
    w*=scale_x;
    h*=scale_y;
  }

  W bm_1 = va(p, sh, w,h, screen_size_x,screen_size_y);
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
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_size_x);
    float scale_y = float(g_event_screen_y)/float(screen_size_y);
    w*=scale_x;
    h*=scale_y;
  }


  W bm_1 = ml(p, sh, sh2, sh_2d, sh_arr, w,h, screen_size_x,screen_size_y);
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
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_x);
    float scale_y = float(g_event_screen_y)/float(screen_y);
    w*=scale_x;
    h*=scale_y;
  }


  W bm_1 = shader_plane(p, w,h*h/w, screen_x, screen_y);
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


EXPORT GameApi::W GameApi::GuiApi::window_decoration(int sx, int sy, std::string label, FtA atlas, BM atlas_bm)
{ // wayland only
  int w = sx;
  int h = sy;
  W contents = rectangle(0,w,0,h,0xff000000);
  
  W txt = text(label, atlas, atlas_bm,3);
  W txt_1 = margin(txt, 5,7,5,5);
  W but_1 = button(w+10,35, 0xff888888, 0xff222222);
  W but_2 = margin(but_1,0,0,0,0);
  W but_3 = window_move(but_2,0,0,w+10,30); 
  W lay_2 = layer(but_3,txt_1);
		   
  W mrg = margin(contents, 5,5+30,5,5);
  W lay_1 = layer(mrg,lay_2);


  W left = button(5,h+5,0xff888888,0xff222222);
  W right = button(5,h+5,0xff888888,0xff222222);
  W l_margin = margin(left,0,30,0,0);
  W r_margin = margin(right,w-4,30,0,0);

  W bottom = button(w+10, 5, 0xff888888,0xff222222);
  W b_margin = margin(bottom, 0, h+5+30, 0,0);

  W lay_3 = layer(l_margin, r_margin);
  W lay_4 = layer(lay_3,b_margin);
  W lay_6 = layer(lay_4,lay_1);
  
  return lay_6;
  
}

EXPORT GameApi::W GameApi::GuiApi::lines_dialog(LI p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm, W &codegen_button, W& collect_button)
{
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_size_x);
    float scale_y = float(g_event_screen_y)/float(screen_size_y);
    w*=scale_x;
    h*=scale_y;
  }

  W bm_1 = lines(p, sh, w,h, screen_size_x,screen_size_y);
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
  int w = 800;
  int h = 600;
  if (g_event_screen_y!=-1) {
    float scale_x = float(g_event_screen_x)/float(screen_size_x);
    float scale_y = float(g_event_screen_y)/float(screen_size_y);
    w*=scale_x;
    h*=scale_y;
  }

  W bm_1 = pts(p, sh, w,h, screen_size_x,screen_size_y);
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
    OpenglLowApi *ogl = g_low->ogl;
    ogl->glDisable(Low_GL_DEPTH_TEST);

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
      W edit = generic_editor(*target, atlas, atlas_bm, type, 2, atlas_tiny, atlas_tiny_bm, size_y(lab));

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
      if (target.s.size()>4 && (target.s.substr(0,4)=="http" ||target.s.substr(0,4)=="file"))
	{
	  W edit = url_editor(target.s, atlas_tiny, atlas_tiny_bm, x_gap);
	  W edit_2 = margin(edit, 0, sy-size_y(edit), 0, 0);
	  return edit_2;
	}
      else 
	{
      std::string allowed = "0123456789abcdefghijklmnopqrstuvwxyz/.ABCDEFGHIJKLMNOPQRSTUVWXYZ*()-#+/*!\"¤%&?\n,:_";
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
	GameApi::BB b = ev.bool_bitmap_api.bb_empty(sx,sy);
	GameApi::BB b1 = ev.bool_bitmap_api.rectangle(b, 0.0, 0.0, float(sx), 2.0);
	GameApi::BB b2 = ev.bool_bitmap_api.rectangle(b1, 0.0, 0.0, 2.0, float(sy));
	GameApi::BB b3 = ev.bool_bitmap_api.rectangle(b2, float(sx)-2.0, 0.0, 2.0, float(sy));
	GameApi::BB b4 = ev.bool_bitmap_api.rectangle(b3, 0.0, float(sy)-2.0, float(sx), 2.0);
	bg = ev.bool_bitmap_api.to_bitmap(b4, 255,255,255,255, 0,0,0,0);

	bg_va = ev.sprite_api.create_vertex_array(bg);
	GameApi::BB k = ev.bool_bitmap_api.bb_empty(sx-2-2-2-2, float(sy)/float(area_y)*float(sy-2-2-2-2));
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
	GameApi::BB b = ev.bool_bitmap_api.bb_empty(sx,sy);
	GameApi::BB b1 = ev.bool_bitmap_api.rectangle(b, 0.0, 0.0, float(sx), 2.0);
	GameApi::BB b2 = ev.bool_bitmap_api.rectangle(b1, 0.0, 0.0, 2.0, float(sy));
	GameApi::BB b3 = ev.bool_bitmap_api.rectangle(b2, float(sx)-2.0, 0.0, 2.0, float(sy));
	GameApi::BB b4 = ev.bool_bitmap_api.rectangle(b3, 0.0, float(sy)-2.0, float(sx), 2.0);
	bg = ev.bool_bitmap_api.to_bitmap(b4, 255,255,255,255, 0,0,0,0);

	bg_va = ev.sprite_api.create_vertex_array(bg);
	GameApi::BB k = ev.bool_bitmap_api.bb_empty(float(sx)/float(area_x)*float(sx-2-2-2-2), sy-2-2-2-2);
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
extern int g_event_screen_y;
extern int g_event_screen_x;
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
    OpenglLowApi *ogl = g_low->ogl;
    if (is_visible())
      {
    ogl->glEnable(Low_GL_SCISSOR_TEST);
    if (g_event_screen_y!=-1) {
      float scale_x = float(g_event_screen_x)/float(ev.mainloop_api.get_screen_width());
      float scale_y = float(g_event_screen_y)/float(ev.mainloop_api.get_screen_height());
      //std::cout << "SCISSOR SIZE: " << size.dx << " " << size.dy << " " << scale_x << " " << scale_y << std::endl;
      ogl->glScissor(pos.x*scale_x, g_event_screen_y-pos.y*scale_y-size.dy*scale_y, (size.dx)*scale_x, size.dy*scale_y);
    } else {
      ogl->glScissor(pos.x, screen_y-pos.y-size.dy, size.dx, size.dy);
    }
    vec[0]->render();
    ogl->glDisable(Low_GL_SCISSOR_TEST);
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

  BB mask_0 = ev.bool_bitmap_api.bb_empty(sx,sy);
  BB mask_0a = ev.bool_bitmap_api.rectangle(mask_0, 0.0, 0.0, float(sx), 2.0);
  BB mask_0b = ev.bool_bitmap_api.rectangle(mask_0a, 0.0, 0.0, 2.0, float(sy));
  BB mask_1 = mask_0b;
  
  BB mask__0 = ev.bool_bitmap_api.bb_empty(sx,sy);
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
  if (ww) {
  ww->set_pos(p);
  }
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
  if (ww) {
  Point *pt = find_point(e, mouse);

  Point2d pt2 = { pt?pt->x:0.0f, pt?pt->y:0.0f };
  ww->update(pt2, button,ch, type, mouse_wheel_y);
  }
}
EXPORT void GameApi::GuiApi::render(W w)
{
  //std::cout << "GuiApi::render" << w.id << std::endl;
  GuiWidget *ww = find_widget(e, w);
  if (ww)
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
EXPORT int GameApi::GuiApi::get_index(W w)
{
  GuiWidget *ww = find_widget(e,w);
  return ww->get_index();
}
EXPORT int GameApi::GuiApi::get_size2(W w)
{
  GuiWidget *ww = find_widget(e,w);
  return ww->get_size2();
}
EXPORT void GameApi::GuiApi::set_size2(W w, int sz)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_size2(sz);
}
EXPORT void GameApi::GuiApi::set_index(W w, int j)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_index(j);
}
EXPORT void GameApi::GuiApi::set_id(W w, std::string id)
{
  GuiWidget *ww = find_widget(e, w);
  ww->set_id(id);
}


int to_int(std::string s);



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
int template_get_id(GameApi::ARR a) { return 0; }

void funccall_1(std::vector<std::string> &s, GameApi::ExecuteEnv &e, std::vector<std::string> param_name);



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
  std::vector<int> j; // -1 = check array
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

  //std::cout << "CodeGenLine: " << line2.api_name << " " << line2.func_name << " " << line2.params << std::endl;
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
    //std::cout << "url loading complete! " << url_only << std::endl;
    
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

ASyncData async_data[] = { 
  { "font_api", "newfont", 0 },
  { "font_api", "load_font", 0 },
  { "mainloop_api", "load_song", 2 },
#ifndef HAS_POPEN
  { "polygon_api", "p_url", 1 },
#endif
  { "polygon_api", "p_url_mtl", 1 },
#ifndef HAS_POPEN
  { "polygon_api", "p_mtl", 1 },
#endif
  { "polygon_api", "p_mtl", 2 },
#ifndef HAS_POPEN
  { "polygon_api", "p_mtl2", 1 },
#endif
  { "polygon_api", "p_mtl2", 2 },
  { "mainloop_api", "fps_display", 2 },
  { "mainloop_api", "score_display", 2 },
  { "mainloop_api", "time_display", 2 },
  { "mainloop_api", "load_P_script", 1 },
  { "mainloop_api", "load_P_script_array", 1 },
  { "mainloop_api", "load_ML_script", 1 },
  { "mainloop_api", "load_MN_script", 1 },
  { "mainloop_api", "load_MT_script", 1 },
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
  { "polygon_api", "load_scene", 1 },
  { "polygon_api", "gltf_load", 2 },
  { "polygon_api", "gltf_load_bitmap", 2 },
  { "materials_api", "gltf_material", 2 },
  { "materials_api", "gltf_material_manual", 2 },
  { "materials_api", "gltf_material_env", 2 },
  { "polygon_api", "bar_chart", 1 },
  { "polygon_api", "bar_chart2", 1 },
  { "polygon_api", "piechart_full", 4 },
  { "mainloop_api", "gltf_mesh", 2 },
  { "mainloop_api", "gltf_mesh_all", 2 },
  { "mainloop_api", "gltf_mesh_all_env", 2 },
  { "mainloop_api", "gltf_node", 2 },
  { "mainloop_api", "gltf_scene", 2 },
  { "mainloop_api", "gltf_anim", 2 },
  { "mainloop_api", "gltf_anim2", 2 },
  { "mainloop_api", "gltf_skeleton", 2 },
  { "mainloop_api", "matrix_range_check", 3 },
  { "font_api", "draw_text_large", 2 },
  { "bitmap_api", "loadbitmapfromurl", 0},
  { "mainloop_api", "parse_areatype", 1},
  { "mainloop_api", "create_landscape", 1},
  { "mainloop_api", "bind_obj_type", 1},
  { "mainloop_api", "read_obj_pos", 0},
  { "polygon_api", "mesh_anim", 6},
  { "materials_api", "many_texture_id_material", 1},
  { "bitmap_api", "script_bitmap", 0},
  { "points_api", "ply_pts", 0 },
  { "points_api", "ply_faces", 0 },
  { "mainloop_api", "memmap_window2", 1 },
  { "mainloop_api", "memmap_window3", 1 },
  { "mainloop_api", "memmap_window3", 2 },
  { "mainloop_api", "memmap_window3", 3 },
  { "mainloop_api", "memmap_window3", 4 },
  { "mainloop_api", "memmap_window3", 5 },
  { "mainloop_api", "memmap_window3", 6 },
  { "polygon_api", "ske_anim", 4 },
  { "polygon_api", "ske_anim2", 4 },
  { "lines_api", "li_url", 0 },
  { "mainloop_api", "gltf_anim_skeleton", 2},
  { "polygon_api", "gltf_split_faces2", 2},
  { "mainloop_api", "gltf_anim4", 2},
  { "mainloop_api", "gltf_scene_anim", 2},
  { "mainloop_api", "async_url", 0},
  { "materials_api", "gltf_anim_material", 2},
  { "materials_api", "gltf_anim_material2", 2},
  { "mainloop_api", "tunnel_tree", 3},
  { "mainloop_api", "async_gltf", 2},
  { "materials_api", "gltf_material_from_file", 1}
};
ASyncData *g_async_ptr = &async_data[0];
int g_async_count = sizeof(async_data)/sizeof(ASyncData);
ASyncData async_data2[] = { 
  { "polygon_api", "p_mtl", 1 },
  { "polygon_api", "p_mtl2", 1 },
  { "polygon_api", "p_url", 1 }
};
ASyncData *g_async_ptr2 = &async_data2[0];
int g_async_count2 = sizeof(async_data2)/sizeof(ASyncData);



std::vector<std::string> g_async_loaded_urls;
bool is_async_loaded_urls_in_vec(std::string url)
{
  int s= g_async_loaded_urls.size();
  for(int i=0;i<s;i++) {
    if (g_async_loaded_urls[i]==url) return true;
  }
  return false;
}
struct ASyncCallback { void (*fptr)(void*); void *data; };
ASyncCallback *rem_async_cb(std::string url);


struct del_map
{
  void del_url(std::string url)
  {
    //std::vector<unsigned char>* v = load_url_buffers_async[url];
    //delete v;
    //load_url_buffers_async[url] = 0;
  }
  ~del_map() {
    //std::map<std::string,std::vector<unsigned char>*>::iterator i = load_url_buffers_async.begin();
    //for(;i!=load_url_buffers_async.end();i++)
    //  {
    //	std::pair<std::string,std::vector<unsigned char>*> p = *i;
    //	std::cout << std::hex << (long)p.second << "::" << p.first << std::endl;
    //	delete p.second;
    //  }
  }
  std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;
};
extern del_map g_del_map;

//extern std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;

void async_cb(void *data)
{
  //ASyncCallback *cb = (ASyncCallback*)data;
  
}
std::vector<ASyncCallback*> g_async_vec;
std::vector<std::string> g_async_vec2;

void LoadUrls_async_cbs()
{
  int s = g_async_vec.size();
  for(int i=0;i<s;i++) {
    ASyncCallback *cb = g_async_vec[i];
    std::string url = g_async_vec2[i];
    
    if (cb && g_del_map.load_url_buffers_async[std::string("load_url.php?url=") + url]!=0) {
      //std::cout << "Callback: " << url << std::endl;
      (*cb->fptr)(cb->data);
      g_async_vec[i] =0 ;
    }
  }
  //g_async_vec.clear();
}



std::vector<std::string> g_extra_async_urls;

bool is_in_vec(std::string url)
{
  int s = g_extra_async_urls.size();
  for(int i=0;i<s;i++) { if (g_extra_async_urls[i]==url) return true; }
  return false;
}
std::vector<std::string> g_registered_urls;
bool is_in_registered(std::string url)
{
  int s = g_registered_urls.size();
  for(int i=0;i<s;i++) if (g_registered_urls[i]==url) return true;
  return false;
}
class RegisterUrl : public MainLoopItem
{
public:
  RegisterUrl(std::string url, MainLoopItem *next) : next(next) {
    g_registered_urls.push_back(url);
  }
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
};
GameApi::ML GameApi::MainLoopApi::async_url(std::string url, ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new RegisterUrl(url, item));
}
extern int g_async_load_count;
void LoadUrls_async(GameApi::Env &e, const CodeGenLine &line, std::string homepage)
{
  int s = sizeof(async_data)/sizeof(ASyncData);
  for(int i=0;i<s;i++)
    {
      ASyncData &dt = async_data[i];
      if (line.api_name == dt.api_name && line.func_name == dt.func_name)
	{
	  g_async_load_count++;
	  int param_num = dt.param_num;
	  std::string url = line.params[param_num];

	  if (!is_async_loaded_urls_in_vec(url)) {

	    e.async_load_url(url,homepage);
	    g_async_loaded_urls.push_back(url);
	  } else {

	    ASyncCallback *cb = rem_async_cb(std::string("load_url.php?url=")+url);
	    g_async_vec.push_back(cb);
	    g_async_vec2.push_back(url);
	      
	  }
	}
    }
}			 
void InstallProgress(int num, std::string label, int max=15);

void LoadUrls(const CodeGenLine &line, std::string homepage)
{
  return;
  //if (line.api_name!="bitmap_api" || line.func_name!="loadbitmapfromurl")
  //  return;
#if 0
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

    LoadData *ld = new LoadData;
    ld->buf2 = buf2;
    ld->buf3 = buf3;
    ld->url = oldurl;
    ld->url3 = url3;
    emscripten_idb_async_exists("gameapi", oldurl.c_str(), (void*)ld, &idb_exists2, &idb_error2);
    //emscripten_async_wget_data(buf2, (void*)buf2 , &onload_cb, &onerror_cb);
    emscripten_async_wget2_data(buf2, "POST", urlend.c_str(), (void*)buf3, 1, &onload_cb, &onerror_cb, &onprogress_cb);
#endif
#endif
}
void LoadUrls_codegen(GameApi::Env &env, std::vector<CodeGenLine> vec, std::string homepage)
{
  int s = vec.size();
  for(int i=0;i<s;i++) {
    CodeGenLine l = vec[i];
      LoadUrls(l, homepage);
      LoadUrls_async(env,l, homepage);
  }
}
std::vector<CodeGenLine> parse_codegen(GameApi::Env &env, GameApi::EveryApi &ev, std::string text, int &error_line_num)
{
  int idx = 0;
  int old_idx = 0;
  int line_num = 0;
  error_line_num = 0;
  std::vector<CodeGenLine> vec;
  static std::vector<GameApiItem*> funcs = all_functions();
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
      //LoadUrls(l, homepage);
      //LoadUrls_async(env,l, homepage);
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
  std::vector<int> j;
};

std::pair<std::string, int> parse_multiple_return(std::string param)
{
  int s = param.size();
  int start = s;
  int end = s;
  for(int i=0;i<s;i++) {
    if (param[i]=='[') {
      start = i;
    }
    if (param[i]==']') {
      end = i;
    }
  }
  if (start==s && end==s) return std::make_pair(param,0);
  std::string res = param.substr(0,start);
  std::string index = param.substr(start+1,end-start-1);
  std::stringstream ss(index);
  int ii = 0;
  ss >> ii;
  return std::make_pair(res,ii);
}

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
	  std::pair<std::string,int> pp = parse_multiple_return(param_value);
	  int jj=0;
	  if (pp.first.size()>0 && pp.first[0]=='I' && line_map.find(pp.first)!=line_map.end())
	    {
	      int linkage = line_map[pp.first];
	      std::stringstream ss;
	      ss << linkage;
	      param_linkage = ss.str();
	      jj = pp.second;
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

		    std::pair<std::string,int> pp2 = parse_multiple_return(param);
		    
		    std::string param_linkage = "";
		    if (pp2.first.size()>0 && pp2.first[0]=='I')
		      {
			int linkage = line_map[pp2.first];
			std::stringstream ss;
			ss << linkage;
			param_linkage = ss.str();
		      } else if (pp2.first.size()>0 && pp2.first[0]=='E')
		      {
			int linkage = env_map[pp2.first];
			std::stringstream ss;
			ss << linkage;
			param_linkage = std::string("E") + ss.str();
		      } else {
		      param_linkage = param;
		    }
		    vec.params.push_back(param_linkage);
		    vec.j.push_back(pp.second);
		    pos = pos2+1;
		    if (param_value[pos2]=='}') break;
		  }
		  vectors.push_back(vec);
		  std::stringstream ss;
		  ss << vectors.size()-1;
		  param_linkage = std::string("%") + ss.str();
		  jj = -1;
		}
	    }
	  lines[i].params_linkage.push_back(param_linkage);
	  lines[i].j.push_back(jj);
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

std::vector<int> execute_api(GameApi::Env &ee, GameApi::EveryApi &ev, const std::vector<CodeGenLine> &vec, std::vector<CodeGenVectors> &vecvec, int /*line_num*/, GameApi::ExecuteEnv &e)
{
  std::vector<std::string> res_vec;
  int s2 = vec.size();
  int jj = 0;
  for(int ij=0;ij<s2;ij++)
    {
      CodeGenLine l = vec[ij];

      int s = l.params_linkage.size();
      std::vector<std::string> params = l.params;
      for(int i=0;i<s;i++)
	{
	  std::string link = l.params_linkage[i];
	  int jj = l.j[i];
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
		  int jj = vecvec[idx].j[i];
		  if (is_num(link)) {
		    std::stringstream ss(link);
		    int num;
		    ss >> num;
		    //std::stringstream ss2;
		    //ss2 << jj;
		    std::string val = res_vec[num];
		    std::stringstream ss2(val);
		    std::string val2;
		    std::vector<std::string> vec;
		    while(ss2>>val2) {
		      //std::cout << "execute_api: " << val2 << std::endl;
		      vec.push_back(val2);
		    }
		    
		    params2.push_back( vec[jj] );
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
	      //std::stringstream ss2;
	      //ss2 << jj;
	      std::string val = res_vec[num];
	      std::stringstream ss2(val);
	      std::string val2;
	      std::vector<std::string> vec;
	      while(ss2>>val2) { vec.push_back(val2); }
	      
	      
	      params[i] = vec[jj]; 
	    }
	}
      
      //std::cout << "Execute: " << params << std::endl;
      std::stringstream sk3;
      std::string ret_type = l.item->ReturnType(0);
      int count = ret_type_count(ret_type);
      std::string res;
      int val = l.item->Execute(sk3, ee,ev, params, e,-1);
      if (count>1) {
	GameApi::ARR arr;
	arr.id = val;
	ArrayType *t = find_array(ee,arr);
	for(int jj=0;jj<count;jj++)
	  {
	    int val2 = t->vec[jj];
	    std::stringstream ss2;
	    ss2 << val2;
	    res+=ss2.str();
	    if (jj!=count-1) res+=" ";
	  }
      } else { std::stringstream ss; ss << val; res=ss.str(); }
      res_vec.push_back(res);
    }
  std::string s = res_vec[res_vec.size()-1];
  std::stringstream ss(s);
  int val = -1;
  std::vector<int> res_vec2;
  while(ss >> val) { res_vec2.push_back(val); }
  return res_vec2;
  
}


std::string ToString(int num)
{
  std::stringstream ss;
  ss << num;
  return ss.str();
}
extern int g_async_load_count;
std::pair<int,std::string> GameApi::execute_codegen(GameApi::Env &env, GameApi::EveryApi &ev, std::string text, GameApi::ExecuteEnv &e)
{
  g_async_load_count = 0;
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
  static std::vector<GameApiItem*> functions = all_functions();
  link_api_items(vec, functions);
  std::vector<int> val = execute_api(env, ev, vec, vecvec, vec.size()-1, e);
  std::string homepage = ev.mainloop_api.get_homepage_url();
  LoadUrls_codegen(env,vec,homepage);
  CodeGenLine last = vec[vec.size()-1];
  if (last.return_type=="BLK") return std::make_pair(val[0],"OK");
  return std::make_pair(val[0],last.return_type);
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
std::pair<std::string,std::string> CodeGen_1(GameApi::EveryApi &ev, std::vector<std::string> params, std::vector<std::string> param_names, std::vector<std::string> param_type, std::string return_type, std::string api_name, std::string func_name,int j);



#if USE_CHAISCRIPT
#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/language/chaiscript_common.hpp>
#endif



DelApiItemF g_api_item_deleter;
extern DelApiItemF g_api_item_deleter;


std::vector<GameApiItem*> append_vectors(std::vector<GameApiItem*> vec1, std::vector<GameApiItem*> vec2);

std::vector<GameApiItem*> append_vectors(std::vector<GameApiItem*> vec1, std::vector<GameApiItem*> vec2)
{
  int s = vec2.size();
  for(int i=0;i<s;i++)
    {
      vec1.push_back(vec2[i]);
    }
  return vec1;
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
  return functions_widget(*this, "MaterialsApi", shadermoduleapi_functions(), atlas1, atlas_bm1, atlas2, atlas_bm2, insert);
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

  static std::vector<GameApiItem*> funcs = all_functions();
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
