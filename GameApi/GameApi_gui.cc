
#include "GameApi_h.hh"
#include "GameApi_gui.hh"

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
  std::string label;
  GameApi::FtA atlas;
  GameApi::BM atlas_bm;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
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
class EditorGuiWidgetAtlas : public GuiWidgetForward
{
public:
  EditorGuiWidgetAtlas(GameApi::EveryApi &ev, std::string allowed_chars, T &target_m, GameApi::FtA atlas, GameApi::BM atlas_bm, GameApi::SH sh, int x_gap) : GuiWidgetForward(ev, std::vector<GuiWidget*>()), allowed_chars(allowed_chars), target(target_m), atlas(atlas), atlas_bm(atlas_bm), sh(sh), x_gap(x_gap) { firsttime = true; active=false; 
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
    else if (button==0 && type==1025)
      {
	active = false;
      }

    if (firsttime)
      {
	std::stringstream ss;
	ss << target;
	label = ss.str();
      }
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
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
	if ((ch==8 ||ch==42) && label.size()>0)
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
	rendered_bitmap = ev.font_api.font_string_from_atlas(ev, atlas, atlas_bm, label.c_str(), x_gap);
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
  GameApi::FtA atlas;
  GameApi::BM atlas_bm;
  GameApi::SH sh;
  GameApi::BM rendered_bitmap;
  GameApi::VA rendered_bitmap_va;
  bool active;
  int x_gap;
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
	rot_y+=1.0*3.14159*2.0/360.0;
	obj.set_rotation_y(rot_y);
      }
    if (ch=='d' && type==0x300)
      {
	rot_y-=1.0*3.14159*2.0/360.0;
	obj.set_rotation_y(rot_y);
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
	rot_y+=1.0*3.14159*2.0/360.0;
      }
    if (ch=='d' && type==0x300)
      {
	rot_y-=1.0*3.14159*2.0/360.0;
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
	rot_y+=1.0*3.14159*2.0/360.0;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (ch=='d' && type==0x300)
      {
	rot_y-=1.0*3.14159*2.0/360.0;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
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
    if (!firsttime)
      {
	Point2d pos = get_pos();
	Vector2d sz = get_size();
	glViewport(pos.x, screen_y-pos.y-sz.dy, sz.dx, sz.dy);
	//ev.mainloop_api.switch_to_3d(true, sh);
	glEnable(GL_DEPTH_TEST);
	obj.render();
	glDisable(GL_DEPTH_TEST);
	//ev.mainloop_api.switch_to_3d(false, sh);
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
	rot_y+=1.0*3.14159*2.0/360.0;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
      }
    if (ch=='d' && type==0x300)
      {
	rot_y-=1.0*3.14159*2.0/360.0;
	obj.set_rotation_matrix(ev.matrix_api.yrot(rot_y));
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
	    
	    
	    ev.polygon_api.update_vertex_array(line_3, line_2, true);
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
  W w2 = button(size_x(w1),size_y(w1), 0xff888888, 0xff444444);
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
  W node_0 = button(sx, size_y(node_t0), 0xffff88ff, 0xff8844ff);
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
  ClickAreaWidget(GameApi::EveryApi &ev, GuiWidget *wid, int area_x, int area_y, int area_width, int area_height) : GuiWidgetForward(ev, { wid } ), area_x(area_x), area_y(area_y), area_width(area_width), area_height(area_height) { done=false;
    selected = false;
    Point2d p = { -666.0, -666.0 };
    update(p, -1, -1, -1);
  }
  void update(Point2d mouse, int button, int ch, int type)
  {
    GuiWidgetForward::update(mouse,button,ch, type);

    if (done) {selected=false; }
    if (button==-1) {done = false; }

    //std::cout << "Update!" << std::endl;
    //std::cout << button << " " << mouse << " " << pos << " " << area_x << " " << area_y << " " << area_width << " " << area_height << std::endl;
    if (!done && button==0 && type==1025 && mouse.x >= pos.x+area_x && mouse.x < pos.x+area_x+area_width &&
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


EXPORT GameApi::W GameApi::GuiApi::click_area(W widget, int area_x, int area_y, int area_width, int area_height)
{
  GuiWidget *wid = find_widget(e, widget);
  return add_widget(e, new ClickAreaWidget(ev, wid, area_x, area_y, area_width, area_height));

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

EXPORT GameApi::W GameApi::GuiApi::canvas_item_gameapi_node(int sx, int sy, std::string label, std::vector<std::string> param_types, std::vector<std::string> param_tooltip, std::string return_type, FtA atlas, BM atlas_bm, W &connect_click, std::string uid, std::vector<W> &params)
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
      W txt_2 = button(size_x(txt_1), size_y(txt_1), 0xff888888, 0xff666666);
      W txt_3 = layer(txt_2, txt_11);
      W txt_4 = click_area(txt_3, 0.0,0.0,size_x(txt_3), size_y(txt_3));
      
      std::stringstream ii;
      ii << i;
      std::string param_uid = uid + " " + ii.str();

      set_id(txt_4, param_uid);
      params.push_back(txt_4);
      vec.push_back(txt_4);
    }

  W node_2 = text(label, atlas,atlas_bm);
  W node_22 = margin(node_2, 5,5,5,5);

  W array = array_y(&vec[0], vec.size(), 5);
  int ssy = std::max(sy, size_y(array)+size_y(node_22)+5);
  W array_1 = margin(array, 0, ssy-size_y(array), 0, 0);
  
  W txt_0 = text(return_type, atlas,atlas_bm);
  W txt_1 = margin(txt_0, 5,5,5,5);

  int max_width2 = size_x(txt_1);
  W txt_11 = highlight(txt_1);
  W txt_111 = click_area(txt_11, 0.0, 0.0, size_x(txt_11), size_y(txt_11));
  set_id(txt_111, uid);
  connect_click = txt_111;
  W txt_2 = button(size_x(txt_1), size_y(txt_1), 0xff330033, 0xff880088);
  W txt_3 = layer(txt_2, txt_111);

  int ssx_0 = std::max(sx, max_width+4+20+4+max_width2);
  int ssx = std::max(ssx_0,size_x(node_22));
  W node_0 = button(ssx,ssy, 0xffff8844, 0xff884422);
  W node_1 = button(ssx,size_y(node_22), 0xffff88ff, 0xff8844ff);
  W node_12 = highlight(node_1);

  W txt_4 = margin(txt_3, ssx-size_x(txt_3), (sy-size_y(txt_3))/2, 0,(sy-size_y(txt_3))/2);


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

EXPORT GameApi::W GameApi::GuiApi::polygon_dialog(P p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm)
{
  W bm_1 = poly(p, sh, 400,400, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), 0xff888888, 0xff444444);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), 0xff00ff00, 0xff008800);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5));
  close_button = but_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::shader_dialog(SFO p, W &close_button, FtA atlas, BM atlas_bm, int screen_x, int screen_y)
{
  W bm_1 = shader_plane(p, 400,400*600/800, screen_x, screen_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), 0xff888888, 0xff444444);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), 0xff00ff00, 0xff008800);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5));
  close_button = but_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}


EXPORT GameApi::W GameApi::GuiApi::lines_dialog(LI p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm)
{
  W bm_1 = lines(p, sh, 400,400, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), 0xff888888, 0xff444444);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), 0xff00ff00, 0xff008800);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5));
  close_button = but_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}

EXPORT GameApi::W GameApi::GuiApi::pts_dialog(PTS p, SH sh, int screen_size_x, int screen_size_y, W &close_button, FtA atlas, BM atlas_bm)
{
  W bm_1 = pts(p, sh, 400,400, screen_size_x,screen_size_y);
  W bm_2 = margin(bm_1, 10,10,10,10);
  W bm_3 = button(size_x(bm_2), size_y(bm_2), 0xff888888, 0xff444444);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), 0xff00ff00, 0xff008800);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5));
  close_button = but_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

  W arr_3 = mouse_move(arr_2, 0,0, size_x(arr_2), size_y(arr_2));
  return arr_3;
}



EXPORT GameApi::W GameApi::GuiApi::bitmap_dialog(BM bm, W &close_button, FtA atlas, BM atlas_bm)
{
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
  W bm_3 = button(size_x(bm_2), size_y(bm_2), 0xff888888, 0xff444444);
  W bm_4 = layer(bm_3, bm_2);
  
  W but_1 = text("Close", atlas, atlas_bm);
  W but_2 = center_align(but_1, size_x(bm_4));
  W but_3 = center_y(but_2, 60.0);
  W but_4 = button(size_x(but_3), size_y(but_3), 0xff00ff00, 0xff008800);
  W but_41 = highlight(but_4);
  W but_5 = layer(but_41, but_3);
  W but_6 = click_area(but_5, 0,0,size_x(but_5), size_y(but_5));
  close_button = but_6;


  W arr[] = { bm_4, but_6 };
  W arr_2 = array_y(&arr[0], 2, 0);

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
  W array = array_y(&vec2[0], vec2.size(), 35);
  W array_1 = margin(array, 10,10,10,10);
  W array_1a = center_align(array_1, 500);
  W array_2 = button(500, size_y(array_1), 0xff224488, 0xff112244);
  W array_3 = layer(array_2, array_1a);

  W cancel_button = button(250,50, 0xff884422, 0xff442211);
  W cancel_button_1 = text("Cancel", atlas,atlas_bm, 4);
  W cancel_button_11 = center_align(cancel_button_1, 250);
  W cancel_button_111 = center_y(cancel_button_11, 50);
  W cancel_button_2 = layer(cancel_button, cancel_button_111);
  W cancel_button_3 = highlight(size_x(cancel_button_2), size_y(cancel_button_2));
  W cancel_button_4 = layer(cancel_button_2, cancel_button_3);
  W cancel_area = click_area(cancel_button_4, 0,0,250,50);
  cancel_but = cancel_area;
  W ok_button = button(250,50, 0xff884422, 0xff442211);
  W ok_button_1 = text("Ok", atlas,atlas_bm, 4);
  W ok_button_11 = center_align(ok_button_1, 250);
  W ok_button_111 = center_y(ok_button_11, 50);
  W ok_button_2 = layer(ok_button, ok_button_111);
  W ok_button_3 = highlight(size_x(ok_button_2), size_y(ok_button_2));
  W ok_button_4 = layer(ok_button_2, ok_button_3);
  W ok_area = click_area(ok_button_4, 0,0,250,50);
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
      std::string allowed = "0123456789abcdefghijklmnopqrstuvwxyz/.ABCDEFGHIJKLMNOPQRSTUVWXYZ*()-#+/*";
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
  W dummy = button(30,30, 0xffffffff, 0xff888888);
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
  W w3 = button(size_x(w), size_y(w), 0xffff8844, 0xff884422);

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
      {
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
  BM bm = ev.bitmap_api.gradient(pos_1, pos_2, color_1, color_2, sx, sy);
  return icon(bm);
}
GameApi::W GameApi::GuiApi::button(int sx, int sy, unsigned int color_1, unsigned int color_2)
{
  PT pt1 = ev.point_api.point(0.0, 0.0, 0.0);
  PT pt2 = ev.point_api.point(0.0, sy, 0.0);
  BM w = ev.bitmap_api.gradient(pt1, pt2, color_1, color_2, sx,sy);
  unsigned int color_1L = color_1;
  unsigned int color_2L = color_2;
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
  BM w2 = ev.bitmap_api.gradient(pt1, pt2, color_1L, color_2L,sx,sy);

  unsigned int color_1D = color_1;
  unsigned int color_2D = color_2;
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
  W w3 = button(size_x(w22), size_y(w22), 0xffff8844, 0xff884422);
    //gradient(size_x(w2), size_y(w2), pt1, pt2, 0xffff8844, 0xff884422);
  W w4 = layer(w3,w22);
  set_pos(w4, pos.x, pos.y+size.dy);
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


template<typename T> T from_stream(std::stringstream &is)
{
  std::cout << "Type using default: " << typeid(T).name() << std::endl;
  T t;
  is >> t;
  return t;
}
template<> unsigned int from_stream<unsigned int>(std::stringstream &is)
{
  unsigned int bm;
  //char c;
  //is >> c;
  //is >> c;
  is >> std::hex >> bm >> std::dec;
  return bm;
}


template<> GameApi::BM from_stream<GameApi::BM>(std::stringstream &is)
{
  GameApi::BM bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::BB from_stream<GameApi::BB>(std::stringstream &is)
{
  GameApi::BB bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::FB from_stream<GameApi::FB>(std::stringstream &is)
{
  GameApi::FB bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::PT from_stream<GameApi::PT>(std::stringstream &is)
{
  GameApi::PT bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::V from_stream<GameApi::V>(std::stringstream &is)
{
  GameApi::V bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::FO from_stream<GameApi::FO>(std::stringstream &is)
{
  GameApi::FO bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::SFO from_stream<GameApi::SFO>(std::stringstream &is)
{
  GameApi::SFO bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::O from_stream<GameApi::O>(std::stringstream &is)
{
  GameApi::O bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::COV from_stream<GameApi::COV>(std::stringstream &is)
{
  GameApi::COV bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::CBM from_stream<GameApi::CBM>(std::stringstream &is)
{
  GameApi::CBM bm;
  is >> bm.id;
  return bm;
}

template<> GameApi::P from_stream<GameApi::P>(std::stringstream &is)
{
  GameApi::P bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::SH from_stream<GameApi::SH>(std::stringstream &is)
{
  GameApi::SH bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::CO from_stream<GameApi::CO>(std::stringstream &is)
{
  GameApi::CO bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::Ft from_stream<GameApi::Ft>(std::stringstream &is)
{
  GameApi::Ft bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::VA from_stream<GameApi::VA>(std::stringstream &is)
{
  GameApi::VA bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::VX from_stream<GameApi::VX>(std::stringstream &is)
{
  GameApi::VX bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::PL from_stream<GameApi::PL>(std::stringstream &is)
{
  GameApi::PL bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::TX from_stream<GameApi::TX>(std::stringstream &is)
{
  GameApi::TX bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::TXID from_stream<GameApi::TXID>(std::stringstream &is)
{
  GameApi::TXID bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::LI from_stream<GameApi::LI>(std::stringstream &is)
{
  GameApi::LI bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::LLA from_stream<GameApi::LLA>(std::stringstream &is)
{
  GameApi::LLA bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::PTS from_stream<GameApi::PTS>(std::stringstream &is)
{
  GameApi::PTS bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::FBO from_stream<GameApi::FBO>(std::stringstream &is)
{
  GameApi::FBO bm;
  is >> bm.id;
  return bm;
}
template<> GameApi::W from_stream<GameApi::W>(std::stringstream &is)
{
  GameApi::W bm;
  is >> bm.id;
  return bm;
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
  RT val = (ptr->*fptr)(from_stream<P>(ss2)...);
  std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id;
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
	  std::string ret_type) 
    : api(api), fptr(fptr), name(name), param_name(param_name),
      param_type(param_type), param_default(param_default), 
      return_type(ret_type) { }
  int Count() const { return 1; }
  std::string Name(int i) const { return name; }
  int ParamCount(int i) const { return param_name.size(); }
  std::string ParamName(int i, int p) const { return param_name[p]; }
  std::string ParamType(int i, int p) const { return param_type[p]; }
  std::string ParamDefault(int i, int p) const { return param_default[p]; }
  std::string ReturnType(int i) const { return return_type; }
  int Execute(GameApi::EveryApi &ev, std::vector<std::string> params, GameApi::ExecuteEnv &e)
  {
    return funccall(ev, api, fptr, params, e, param_name); 
  }
private:
  T (GameApi::EveryApi::*api);
  RT (T::*fptr)(P...);
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> param_type;
  std::vector<std::string> param_default;
  std::string return_type;
};
template<class T, class RT, class... P>
GameApiItem* ApiItemF(T (GameApi::EveryApi::*api), RT (T::*fptr)(P...),
		      std::string name,
		      std::vector<std::string> param_name,
		      std::vector<std::string> param_type,
		      std::vector<std::string> param_default,
		      std::string return_type)
{
  return new ApiItem<T,RT,P...>(api, fptr, name, param_name, param_type, param_default, return_type);
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
			 "TX"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_bitmap,
			 "tex_bitmap",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "TX"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_assign,
			 "tex_assign",
			 { "tx", "id", "x", "y", "bm" },
			 { "TX", "int", "int", "int", "BM" },
			 { "", "0", "0", "0", "" },
			 "TX"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::tex_coord,
			 "tex_coord",
			 { "tx", "id", "x", "y", "width", "height" },
			 { "TX", "int", "int", "int", "int", "int" },
			 { "", "0", "0", "0", "100", "100" },
			 "TX"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::get_tex_coord,
			 "get_tex_coord",
			 { "tx", "id" },
			 { "TX", "int" },
			 { "", "0" },
			 "Q"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::prepare,
			 "tx_prepare",
			 { "tx" },
			 { "TX" },
			 { "" },
			 "TXID"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::texture_api, &GameApi::TextureApi::bind,
			 "tx_bind",
			 { "va", "txid" },
			 { "VA", "TXID" },
			 { "", "" },
			 "VA"));
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
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::sphere,
			 "o_sphere",
			 { "center", "radius" },
			 { "PT", "float" },
			 { "", "100.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::cube,
			 "o_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::cone,
			 "o_cone",
			 { "p1", "p2", "rad1", "rad2" },
			 { "PT", "PT", "float", "float" },
			 { "", "", "100.0", "90.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::torus,
			 "o_torus",
			 { "center", "u_x", "u_y", "dist1", "dist2" },
			 { "PT", "PT", "PT", "float", "float" },
			 { "", "", "", "100.0", "50.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::colour,
			 "o_colour",
			 { "object", "color" },
			 { "O", "unsigned int" },
			 { "", "ffffffff" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::subset_color,
			 "o_subset_color",
			 { "model", "color_subset", "color" },
			 { "O", "O", "unsigned int" },
			 { "", "", "ffffffff" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::move,
			 "o_move",
			 { "obj", "dx", "dy", "dz" },
			 { "O", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatex,
			 "o_rotatex",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatey,
			 "o_rotatey",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rotatez,
			 "o_rotatez",
			 { "object", "angle" },
			 { "O", "float" },
			 { "", "0.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::scale,
			 "o_scale",
			 { "object", "sx", "sy", "sz" },
			 { "O", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::not_op,
			 "o_notop",
			 { "object" },
			 { "O" },
			 { "" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::min_op,
			 "o_minop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::max_op,
			 "o_maxop",
			 { "object1", "object2" },
			 { "O", "O" },
			 { "", "" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::andnot_op,
			 "o_andnot",
			 { "object1", "not_obj" },
			 { "O", "O" },
			 { "", "" },
			 "O"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::volume_api, &GameApi::VolumeApi::rendercubes3,
			 "o_render",
			 { "object", "sx", "sy", "sz", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "O", "int", "int", "int", "float", "float", "float", "float", "float", "float" },
			 { "", "100", "100", "100", "-100.0", "100.0", "-100.0", "100.0", "-100.0", "100.0" },
			 "P"));

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
			 "FO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::from_float_bitmap,
			 "fo_from_fbm",
			 { "bm", "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "FB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "FO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::move,
			 "fo_move",
			 { "f1", "dx", "dy", "dz" },
			 { "FO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "FO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::minimum,
			 "fo_min",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::maximum,
			 "fo_max",
			 { "f1", "f2" },
			 { "FO", "FO" },
			 { "", "" },
			 "FO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_volume_api, &GameApi::FloatVolumeApi::subvolume,
			 "fo_subvolume",
			 { "f", "start_range", "end_range" },
			 { "FO", "float", "float" },
			 { "", "0.5", "1.0" },
			 "O"));
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
			 "COV"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_volume,
			 "cov_from_o",
			 { "obj", "col_true", "col_false" },
			 { "O", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "COV"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::from_continuous_bitmap,
			 "cov_from_cbm",
			 { "bm" },
			 { "CBM" },
			 { "" },
			 "COV"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::mix,
			 "cov_mix",
			 { "p1", "p2", "value" },
			 { "COV", "COV", "float" },
			 { "", "", "0.5" },
			 "COV"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::color_volume_api, &GameApi::ColorVolumeApi::or_cov,
			 "cov_or",
			 { "p1", "p2" },
			 { "COV", "COV" },
			 { "", "" },
			 "COV"));
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
			 "V"));

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
			 "PT"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, (GameApi::PT (GameApi::PointApi::*)(GameApi::PT,float,float,float))&GameApi::PointApi::move,
			 "move",
			 { "p1", "dx", "dy", "dz" },
			 { "PT", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "PT"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::point_api, &GameApi::PointApi::mix,
			 "mix",
			 { "p1", "p2", "val" },
			 { "PT", "PT", "float" },
			 { "", "", "0.5" },
			 "PT"));
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
			 "Ft"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph,
			 "glyph",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_outline,
			 "glyph_outline",
			 { "font", "idx", "sx", "sy" },
			 { "Ft", "long", "float", "float" },
			 { "", "64", "100.0", "100.0" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_plane,
			 "glyph_plane",
			 { "font", "idx", "sx", "sy", "dx", "dy" },
			 { "Ft", "long", "float", "float", "float", "float" },
			 { "", "64", "100.0", "100.0", "30.0", "30.0" },
			 "PL"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string,
			 "font_string",
			 { "font", "str", "x_gap" },
			 { "Ft", "std::string", "int" },
			 { "", "Hello", "5" },
			 "BM"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_fb,
			 "glyph_fb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::glyph_bb,
			 "glyph_bb",
			 { "font", "idx" },
			 { "Ft", "long" },
			 { "", "64" },
			 "BB"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas_info,
			 "atlas_info",
			 { "ev", "font", "chars", "sx", "sy", "y_delta" },
			 { "EveryApi&", "Ft", "std::string", "float", "float", "int" },
			 { "ev", "", "0123456789", "30", "30", "40" },
			 "FtA"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_atlas,
			 "font_atlas",
			 { "ev", "font", "atlas", "sx", "sy" },
			 { "EveryApi&", "Ft", "FtA", "float", "float" },
			 { "ev", "", "", "30", "30" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::font_string_from_atlas,
			 "font_string2",
			 { "ev", "atlas", "atlas_bm", "str", "x_gap" },
			 { "EveryApi&", "FtA", "BM", "const char*", "int" },
			 { "ev", "", "", "Hello", "5" },
			 "BM"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::save_atlas,
			 "save_atlas",
			 { "atlas", "filename" },
			 { "FtA", "std::string" },
			 { "", "atlas1.txt" },
			 "()"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::font_api, &GameApi::FontApi::load_atlas,
			 "load_atlas",
			 { "filename" },
			 { "std::string" },
			 { "atlas1.txt" },
			 "FtA"));
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
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::load_model,
			 "load_model",
			 { "filename", "obj_num" },
			 { "std::string", "int" },
			 { "test.obj", "0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::save_model_ml,
			 "save_model",
			 { "poly", "filename" },
			 { "P", "std::string" },
			 { "", "test.obj" },
			 "ML"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::triangle,
			 "triangle",
			 { "p1", "p2", "p3" },
			 { "PT", "PT", "PT" },
			 { "", "", "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad,
			 "quad",
			 { "p1", "p2", "p3", "p4" },
			 { "PT", "PT", "PT", "PT" },
			 { "", "", "", "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_x,
			 "quad_x",
			 { "x", "y1", "y2", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "100.0", "0.0", "100.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_y,
			 "quad_y",
			 { "x1", "x2", "y", "z1", "z2" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "0.0", "100.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::quad_z,
			 "quad_z",
			 { "x1", "x2", "y1", "y2", "z" },
			 { "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, (GameApi::P (GameApi::PolygonApi::*)(float,float,float,float,float,float))&GameApi::PolygonApi::cube,
			 "cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::sphere,
			 "sphere",
			 { "center", "radius", "numfaces1", "numfaces2" },
			 { "PT", "float", "int", "int" },
			 { "(0.0,0.0,0.0)",  "100.0", "30", "30" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::cone,
			 "cone",
			 { "numfaces", "p1", "p2", "radius1", "radius2" },
			 { "int", "PT", "PT", "float", "float" },
			 { "30", "(0.0,0.0,0.0)", "(0.0,100.0,0.0)", "30.0", "10.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::torus,
			 "torus",
			 { "numfaces1", "numfaces2", "center", "u_x", "u_y", "radius1", "uu_x", "uu_y", "radius2" },
			 { "int", "int", "PT", "V", "V", "float", "V", "V", "float" },
			 { "20", "20", "", "", "", "30.0", "", "", "10.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color,
			 "color",
			 { "orig", "color" },
			 { "P", "unsigned int" },
			 { "", "ffffffff" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_faces,
			 "color_faces",
			 { "orig", "color_1", "color_2", "color_3", "color_4" },
			 { "P", "unsigned int", "unsigned int", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ffffffff", "ff888888", "ff888888" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_from_normals,
			 "color_from_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::recalculate_normals,
			 "recalc_normals",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::color_grayscale,
			 "color_grayscale",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::or_elem,
			 "p_or_elem",
			 { "p1", "p2" },
			 { "P", "P" },
			 { "", "" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::translate,
			 "translate",
			 { "orig", "dx", "dy", "dz" },
			 { "P", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatex,
			 "rotatex",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatey,
			 "rotatey",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::rotatez,
			 "rotatez",
			 { "orig", "angle" },
			 { "P", "float" },
			 { "", "0.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::scale,
			 "scale",
			 { "orig", "sx", "sy", "sz" },
			 { "P", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::shadow,
			 "shadow",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(1.0,1.0,1.0)" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::reflection,
			 "reflection",
			 { "orig", "pos", "u_x", "u_y", "light_vec" },
			 { "P", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(1.0,0.0,0.0)", "(0.0,0.0,1.0)", "(0.0, 1.0,0.0)" },
			 "P"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::memoize,
			 "memoize",
			 { "orig" },
			 { "P" },
			 { "" },
			 "P"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::world_from_bitmap,
			 "world_from_bitmap",
			 { "f", "int_bm", "dx", "dy", "max_c" },
			 { "std::function<P(int c)>", "BM", "float", "float", "int" },
			 { "", "", "100.0", "100.0", "1" },
			 "P"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_points,
			 "from_points",
			 { "p", "f" },
			 { "PTS", "std::function<P (int,float,float,float,unsigned int)>" },
			 { "", "" },
			 "P"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_lines,
			 "from_lines",
			 { "li", "f" },
			 { "LI", "std::function<P (int,float,float,float,float,float,float,float,unsigned int,unsigned int)>" },
			 { "", "" },
			 "P"));
#endif
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::from_polygon,
			 "from_polygon",
			 { "p", "f" },
			 { "P", "std::function<P (int,PT,PT,PT,PT)>" },
			 { "", "" },
			 "P"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::create_vertex_array,
			 "p_prepare",
			 { "p", "b" },
			 { "P", "bool" },
			 { "", "false" },
			 "VA"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::polygon_api, &GameApi::PolygonApi::render_vertex_array_ml,
			 "p_render",
			 { "va" },
			 { "VA" },
			 { "" },
			 "ML"));
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
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float, float, float, float,float))&GameApi::ShaderModuleApi::cube,
			 "sh_cube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::rounded_cube,
			 "sh_rcube",
			 { "start_x", "end_x", "start_y", "end_y", "start_z", "end_z", "r" },
			 { "float", "float", "float", "float", "float", "float","float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0", "20.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture,
			 "sh_texture",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::texture_box,
			 "texture_box",
			 { "start_x", "end_x", "start_y", "end_y", "start_x", "end_x" },
			 { "float", "float", "float", "float", "float", "float" },
			 { "0.0", "100.0", "0.0", "100.0", "0.0", "100.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(float,float,float,float,float,float, float,float))&GameApi::ShaderModuleApi::line,
			 "sh_line",
			 { "start_x", "start_y", "start_z", "end_x", "end_y", "end_z", "width1", "width2" },
			 { "float", "float", "float", "float", "float", "float", "float", "float" },
			 { "0.0", "0.0", "0.0", "100.0", "100.0", "0.0", "15.0", "15.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::plane,
			 "sh_plane",
			 { "center", "u_x", "u_y" },
			 { "PT", "V", "V" },
			 { "", "", "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::torus,
			 "sh_torus",
			 { "radius_1", "radius_2" },
			 { "float", "float" },
			 { "30.0", "10.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color,
			 "sh_color",
			 { "obj", "r", "g", "b", "a" },
			 { "SFO", "float", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0", "1.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float))&GameApi::ShaderModuleApi::rot_x,
			 "rot_x",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_y,
			 "rot_y",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO, float))&GameApi::ShaderModuleApi::rot_z,
			 "rot_z",
			 { "obj", "angle" },
			 { "SFO", "float" },
			 { "", "0.0" },
			 "SFO"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::and_not,
			 "and_not",
			 { "obj", "not_obj" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::or_elem,
			 "sh_or_elem",
			 { "obj1", "obj2" },
			 { "SFO", "SFO" },
			 { "", "" },
			 "SFO"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, (GameApi::SFO (GameApi::ShaderModuleApi::*)(GameApi::SFO,float,float,float))&GameApi::ShaderModuleApi::trans,
			 "sh_trans",
			 { "obj", "dx", "dy", "dz" },
			 { "SFO", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bind_arg,
			 "bind_arg",
			 { "obj", "name", "value" },
			 { "SFO", "std::string", "std::string" },
			 { "", "a", "0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::color_from_normal,
			 "color_from_normal",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::stop_generation,
			 "stop_generation",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::mix_color,
			 "mix_color",
			 { "col1", "col2", "t" },
			 { "SFO", "SFO", "float" },
			 { "", "", "0.5" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::grayscale,
			 "sh_grayscale",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::noise,
			 "sh_noise",
			 { "obj", "strength" },
			 { "SFO", "float" },
			 { "", "1.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::ambient_occulsion,
			 "ambient_occulsion",
			 { "obj", "d", "i" },
			 { "SFO", "float", "float" },
			 { "", "1.0", "1.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::soft_shadow,
			 "soft_shadow",
			 { "scene", "light_dir", "mint", "maxt", "k", "strong" },
			 { "SFO", "V", "float", "float", "float", "float" },
			 { "", "", "1.0", "5.0", "1.0", "3.0" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::bounding_primitive,
			 "bounding_prim",
			 { "prim", "inside", "outside" },
			 { "SFO", "SFO", "SFO" },
			 { "", "", "" },
			 "SFO"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sh_api, &GameApi::ShaderModuleApi::render,
			 "render",
			 { "obj" },
			 { "SFO" },
			 { "" },
			 "SFO"));
  return vec;

}
std::vector<GameApiItem*> linesapi_functions()
{
  std::vector<GameApiItem*> vec;
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color",
			 { "li", "color" },
			 { "LI", "unsigned int" },
			 { "", "ffffffff" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, (GameApi::LI (GameApi::LinesApi::*)(GameApi::LI, unsigned int, unsigned int))&GameApi::LinesApi::change_color,
			 "change_color2",
			 { "li", "color_1", "color_2" },
			 { "LI", "unsigned int", "unsigned int" },
			 { "", "ffffffff", "ff888888" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::from_polygon,
			 "li_from_polygon",
			 { "poly" },
			 { "P" },
			 { "" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::border_from_bool_bitmap,
			 "border_from_bool_bitmap",
			 { "b", "start_x", "end_x", "start_y", "end_y", "z" },
			 { "BB", "float", "float", "float", "float", "float" },
			 { "",   "0.0", "100.0", "0.0", "100.0", "0.0" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::translate,
			 "li_translate",
			 { "lines", "dx", "dy", "dz" },
			 { "LI", "float", "float", "float" },
			 { "", "0.0", "0.0", "0.0" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::scale,
			 "li_scale",
			 { "lines", "m_x", "m_y", "m_z" },
			 { "LI", "float", "float", "float" },
			 { "", "1.0", "1.0", "1.0" },
			 "LI"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::prepare,
			 "li_prepare",
			 { "li" },
			 { "LI" },
			 { "" },
			 "LLA"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::update_ml,
			 "li_update",
			 { "lla", "li" },
			 { "LLA", "LI" },
			 { "", "" },
			 "ML"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::lines_api, &GameApi::LinesApi::render_ml,
			 "li_render",
			 { "lla" },
			 { "LLA" },
			 { "" },
			 "ML"));
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
			 "PTS"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::or_points,
			 "or_points",
			 { "p1", "p2" },
			 { "PTS", "PTS" },
			 { "", "" },
			 "PTS"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::from_volume,
			 "from_volume",
			 { "o", "pos", "u_x", "u_y", "u_z", "sx", "sy", "sz" },
			 { "O", "PT", "V", "V", "V", "int", "int", "int" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,100.0,0.0)", "(0.0,0.0,100.0)", "100", "100", "100" },
			 "PTS"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::points_api, &GameApi::PointsApi::shadow_points,
			 "shadow_points",
			 { "obj", "pos", "u_x", "u_y", "light_vec" },
			 { "PTS", "PT", "V", "V", "V" },
			 { "", "(0.0,0.0,0.0)", "(100.0,0.0,0.0)", "(0.0,0.0,100.0)", "(1.0,1.0,0.0)" },
			 "PTS"));
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
			 "FB"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::grayscale,
			 "grayscale",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_red,
			 "from_red",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_green,
			 "from_green",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_blue,
			 "from_blue",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_alpha,
			 "from_alpha",
			 { "color_bm" },
			 { "BM" },
			 { "" },
			 "FB"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::min_fb,
			 "min_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::max_fb,
			 "max_fb",
			 { "fb1", "fb2" },
			 { "FB", "FB" },
			 { "", "" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::mix_fb,
			 "mix_fb",
			 { "fb1", "fb2", "val" },
			 { "FB", "FB", "float" },
			 { "", "", "0.5" },
			 "FB"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::to_grayscale_color,
			 "to_grayscale_color",
			 { "fb", "r", "g", "b", "a", "r2", "g2", "b2", "a2" },
			 { "FB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::choose_bitmap,
			 "choose_bitmap",
			 { "fb", "bm1", "bm2" },
			 { "FB", "BM", "BM" },
			 { "", "", "" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::from_bool,
			 "from_bool",
			 { "b", "val_true", "val_false" },
			 { "BB", "float", "float" },
			 { "", "1.0", "0.0" },
			 "FB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::float_bitmap_api, &GameApi::FloatBitmapApi::distance_field,
			 "distance_field",
			 { "bb" },
			 { "BB" },
			 { "" },
			 "FB"));

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
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::from_float_bitmap,
			 "from_float_bitmap",
			 { "float_bm", "range_start", "range_end" },
			 { "FB", "float", "float" },
			 { "", "0.5", "1.0" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::line,
			 "line",
			 { "bg", "p_x", "p_y", "p2_x", "p2_y", "line_width1", "line_width2" },
			 { "BB", "float", "float", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "100.0", "100.0", "3.0", "3.0" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::circle,
			 "circle",
			 { "bg", "center_x", "center_y", "radius" },
			 { "BB", "float", "float", "float" },
			 { "", "50.0", "50.0", "50.0" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::rectangle,
			 "rectangle",
			 { "bg", "x", "y", "width", "height" },
			 { "BB", "float", "float", "float", "float" },
			 { "", "0.0", "0.0", "30.0", "30.0" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::not_bitmap,
			 "not_bitmap",
			 { "b" },
			 { "BB" },
			 { "" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::andnot_bitmap,
			 "andnot_bitmap",
			 { "b1", "not_b2" },
			 { "BB", "BB" },
			 { "", "" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::choose_bitmap,
			 "choose_bitmap",
			 { "bools", "true_bm", "false_bm" },
			 { "BB", "BB", "BB" },
			 { "", "", "" },
			 "BB"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bool_bitmap_api, &GameApi::BoolBitmapApi::to_bitmap,
			 "to_bitmap",
			 { "bools", "true_r", "true_g", "true_b", "true_a", "false_r", "false_g", "false_b", "false_a" },
			 { "BB", "int", "int", "int", "int", "int", "int", "int", "int" },
			 { "", "255", "255", "255", "255", "0", "0", "0", "0" },
			 "BM"));
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
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::loadbitmap,
			 "load",
			 { "filename" },
			 { "std::string" },
			 { "test.png" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::subbitmap,
			 "sub",
			 { "orig", "x", "y", "width", "height" },
			 { "BM", "int", "int", "int", "int" },
			 { "",   "0", "0", "100", "100" },
			 "BM"));
  //vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::growbitmap,
  //			 "grow",
  //			 { "orig", "l", "t", "r", "b" },
  //			 { "BM", "int", "int", "int", "int" },
  //			 { "", "2", "2", "2", "2" },
  //			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int))&GameApi::BitmapApi::blitbitmap,
			 "blit",
			 { "bg", "orig", "x", "y" },
			 { "BM", "BM", "int", "int" },
			 { "", "", "0", "0" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::FB))&GameApi::BitmapApi::blitbitmap,
			 "blitFB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "FB" },
			 { "", "", "0", "0", "" },
			 "BM"));
			 
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, (GameApi::BM (GameApi::BitmapApi::*)(GameApi::BM,GameApi::BM,int,int,GameApi::BB))&GameApi::BitmapApi::blitbitmap,
			 "blitBB",
			 { "bg", "orig", "x", "y", "mask" },
			 { "BM", "BM", "int", "int", "BB" },
			 { "", "", "0", "0", "" },
			 "BM"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::interpolate_bitmap,
			 "interpolate",
			 { "orig1", "orig2", "x" },
			 { "BM", "BM", "float" },
			 { "", "", "0.0" },
			 "BM"));

  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::repeat_bitmap,
			 "repeat",
			 { "orig1", "x_count", "y_xount" },
			 { "BM", "int", "int" },
			 { "", "1", "1" },
			 "BM"));
#if 0
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::world_from_bitmap,
			 "world",
			 { "f", "int_bm", "dx", "dy" },
			 { "std::function<BM(int>", "BM", "int", "int" },
			 { "", "", "100", "100" },
			 "BM"));
#endif
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_x,
			 "flip_x",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::flip_y,
			 "flip_y",
			 { "orig" },
			 { "BM" },
			 { "" },
			 "BM"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::gradient,
			 "gradient",
			 { "pos_1", "pos_2", "color_1", "color_2", "sx", "sy" },
			 { "PT",    "PT",    "unsigned int", "unsigned int", "int", "int" },
			 { "ev.point_api.point(0.0,0.0,0.0)", "ev.point_api.point(0.0, 100.0, 0.0)", "ffffffff", "ff888888", "100", "100" },
			 "BM"));
  
  vec.push_back(ApiItemF(&GameApi::EveryApi::bitmap_api, &GameApi::BitmapApi::chessboard,
			 "chessboard",
			 { "tile_sx", "tile_sy", "count_x", "count_y", "color_1", "color_2" },
			 { "int", "int", "int", "int", "unsigned int", "unsigned int" },
			 { "10", "10", "8", "8", "ffffffff", "ff888888" },
			 "BM"));
 
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::create_vertex_array,
			 "bm_prepare",
			 { "bm" },
			 { "BM" },
			 { "" },
			 "VA"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::update_vertex_array_ml,
			 "bm_update",
			 { "va", "bm" },
			 { "VA", "BM" },
			 { "", "" },
			 "ML"));
  vec.push_back(ApiItemF(&GameApi::EveryApi::sprite_api, &GameApi::SpriteApi::render_sprite_vertex_array_ml,
			 "bm_render",
			 { "va" },
			 { "VA" },
			 { "" },
			 "ML"));


  return vec;
}
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
  return ad;
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