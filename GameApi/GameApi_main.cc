
#include "GameApi_h.hh"
#include <chrono>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

EXPORT GameApi::MainLoopApi::MainLoopApi(Env &e) : frame(0.0), time(0.0), e(e)  
{
  priv = (void*)new MainLoopPriv;
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame = 0;
  p->time = 0;
  p->count = 0;
}
EXPORT GameApi::MainLoopApi::~MainLoopApi()
{
  delete (MainLoopPriv*)priv;
}
EXPORT void GameApi::MainLoopApi::cursor_visible(bool enabled)
{
  SDL_ShowCursor(enabled);
}
EXPORT void GameApi::MainLoopApi::init_window(int screen_width, int screen_height)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
  p->screen = InitSDL2(screenx,screeny,false);
#else
  p->screen = InitSDL(screenx,screeny,false);
#endif
  p->screen_width = screenx;
  p->screen_height = screeny;
  time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
}
EXPORT void GameApi::MainLoopApi::init(SH sh, int screen_width, int screen_height)
{
  int screenx = screen_width;
  int screeny = screen_height;
  //glColor4f(1.0,1.0,1.0, 0.5);
  Program *prog = find_shader_program(e, sh);
  prog->use();
  Matrix m = Matrix::Ortho(0,screenx, screeny, 0,0,1);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  prog->set_var("in_T", m2);
  prog->set_var("in_POS", 0.0f);
  alpha_1(false);

#if 0
  glMatrixLoadIdentityEXT(GL_PROJECTION);
  glMatrixLoadIdentityEXT(GL_MODELVIEW);
  glMatrixOrthoEXT(GL_PROJECTION, 0, 800, 600, 0, -1, 1);
#endif
  //glEnable(GL_MULTISAMPLE );

}
EXPORT void GameApi::MainLoopApi::transfer_sdl_surface(MainLoopApi &orig)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  MainLoopPriv *p2 = (MainLoopPriv*)orig.priv;
  p->screen = p2->screen;
}
EXPORT void GameApi::MainLoopApi::delay(int ms)
{
  SDL_Delay(ms);
}
EXPORT unsigned int GameApi::MainLoopApi::rand_max()
{
  Random r;
  return r.maximum();
}
EXPORT unsigned int GameApi::MainLoopApi::random()
{
  Random r;
  return r.next();
}
EXPORT void GameApi::MainLoopApi::profile(std::string label, bool start)
{
#ifndef EMSCRIPTEN
  MainLoopPriv *p = (MainLoopPriv*)priv;
  std::chrono::time_point<std::chrono::high_resolution_clock> time;
  time = std::chrono::system_clock::now();
  //unsigned int time = SDL_GetTicks();
  if (start)
    {
      //if (p->profile_start_time[label]!=0)
      //	{
      //	  std::cout << "Error in profile nesting:" << label << std::endl;
      //	}
      p->profile_start_time[label] = time;
    }
  else
    {
      //      if (p->profile_start_time[label]==0)
      //{
      //  std::cout << "Profile error: Not proper nesting:" << label << std::endl;
      //	}
      std::chrono::high_resolution_clock::duration count = time - p->profile_start_time[label];
      //p->profile_start_time[label] = 0;
      p->profile_count[label]++;
      p->profile_sums[label]+=count;
    }
#endif
}
EXPORT void GameApi::MainLoopApi::print_profile()
{
#ifndef EMSCRIPTEN
  MainLoopPriv *p = (MainLoopPriv*)priv;
  auto i1 = p->profile_sums.begin();
  auto i2 = p->profile_count.begin();
  while(1)
    {
      std::pair<std::string,std::chrono::duration<double> > pp = *i1;
      std::pair<std::string,int> pp2 = *i2;
      std::cout << pp.first << ": " << std::chrono::duration_cast<std::chrono::microseconds>(pp.second).count()/double(pp2.second) << std::endl;
      i1++;
      i2++;
      if (i1==p->profile_sums.end()) break;
    }
#endif
}
EXPORT void GameApi::MainLoopApi::fpscounter()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  unsigned int time = SDL_GetTicks();
  unsigned int delta_time = time - p->time;
  unsigned int f_time = time - p->frame_time;
  //p->time = time;
  if (p->count==0) { p->avg_time = 0; p->time = time; p->frame=0; }
  p->avg_time+=f_time;
  p->frame++;
  p->count++;
  if (delta_time<1) delta_time=1;
  float fps = p->frame/(delta_time/1000.0f);
  if (p->count<0) { p->count = 0; }
  if (p->count>100) {
    std::cout << "FPS: " << fps << " delta_time:" << p->avg_time/100.0 << std::endl;
    p->count = 0;
  }
}

EXPORT void GameApi::MainLoopApi::init_3d(SH sh, int screen_width, int screen_height)
{
  int screenx = screen_width;
  int screeny = screen_height;
#if 0
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
  p->screen = InitSDL2(screenx,screeny,true);
#else
  p->screen = InitSDL(screenx,screeny,true);
#endif
  //glColor4f(1.0,1.0,1.0, 0.5);
  time = SDL_GetTicks();
#endif
  glDisable(GL_LIGHTING);
  //glDisable(GL_DEPTH_TEST);
  //glMatrixMode( GL_PROJECTION ); 
  // glLoadIdentity(); 
  //glOrtho(0, screenx, screeny,0,0,1);
  /*
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 99.1, 60000.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  glMultMatrixf(&mat[0]);
  */
  Program *prog = find_shader_program(e, sh);
  prog->use();
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
  prog->set_var("in_T", m3);
  prog->set_var("in_POS", 0.0f);
  alpha_1(false);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_MULTISAMPLE );
}
EXPORT void GameApi::MainLoopApi::nvidia_init()
{
  if (GLEW_NV_path_rendering)
    {
  glMatrixLoadIdentityEXT(GL_PROJECTION);
  glMatrixLoadIdentityEXT(GL_MODELVIEW);
  glMatrixOrthoEXT(GL_PROJECTION, 0, 800, 600, 0, -1, 1);
    }
}

EXPORT void GameApi::MainLoopApi::switch_to_3d(bool b, SH sh, int screenx, int screeny)
{
  //int screenx = 800;
  //int screeny = 600;
  if (b)
    {
      Program *prog = find_shader_program(e, sh);
      glDisable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
      Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
      Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
      prog->set_var("in_P", m);
      prog->set_var("in_T", m3);
      //glMultMatrixf(&mat[0]);
#ifndef EMSCRIPTEN      
      glMatrixMode( GL_MODELVIEW ); 
#endif
      //Matrix m2 = Matrix::Translate(0.0, 0.0, -500.0);
      Matrix m2 = Matrix::Identity();
      prog->set_var("in_MV", m2);
      //glMatrixLoadIdentityEXT(GL_PROJECTION);
      //glMatrixLoadIdentityEXT(GL_MODELVIEW);
      //glMatrixOrthoEXT(GL_MODELVIEW, 0, screenx, screeny, 0, 0, 1);

    }
  else
    {
      Program *prog = find_shader_program(e, sh);
      Matrix m = Matrix::Ortho(0, screenx, screeny, 0,0,1);
      //m = m*Matrix::Translate(-1.0/2, -1.0/2, 0.0);
      //m = m * Matrix::Scale(1.0, -1.0, 0.0);
      //m = m*Matrix::Translate(-1, -1, 0.0);
      prog->set_var("in_P", m);
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      //glMatrixMode( GL_PROJECTION ); 
      //glLoadIdentity(); 
      //glOrtho(0, screenx, screeny,0,0,1);
      //glMatrixMode( GL_MODELVIEW ); 
      //glLoadIdentity();
      //glTranslatef(0.375, 0.375, 0.0);
      Matrix m2 = Matrix::Translate(0.375, 0.375, 0.0);
      prog->set_var("in_MV", m2);
      Matrix m3 = Matrix::Identity();
      prog->set_var("in_T", m3);
    }
}

EXPORT void GameApi::MainLoopApi::clear()
{
  //glClearColor(255,255,255,255);
  glClearStencil(0);
  glClearColor(0,0,0,0);
  glStencilMask(~0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  //glLoadIdentity();
  //glTranslatef(0.375, 0.375, 0.0);
  //glTranslatef(0.0, 0.0, -260.0);
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}
EXPORT void GameApi::MainLoopApi::clear_3d(unsigned int color)
{
  //glClearColor(255,255,255,255);
  glClearStencil(0);

  int r = color & 0x00ff0000;
  int g = color & 0x0000ff00;
  int b = color & 0x000000ff;
  int a = color & 0xff000000;
  a>>=24;
  r>>=16;
  g>>=8;
  glClearColor(r/256.0,g/256.0,b/256.0,a/256.0);
  glStencilMask(~0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#ifndef EMSCRIPTEN
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);
  glTranslatef(0.0, 0.0, -260.0);
#endif
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}

EXPORT void GameApi::MainLoopApi::transparency(bool enabled)
{
  if (enabled)
    {
      //glBlendFunc(GL_ZERO, GL_SRC_COLOR); 
     glDepthMask(GL_FALSE);
    }
  else
    {
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(GL_TRUE);
    }
}
EXPORT void GameApi::MainLoopApi::depth_test(bool enabled)
{
  if (enabled)
    {
      glEnable(GL_DEPTH_TEST);
    }
  else
    {
      glDisable(GL_DEPTH_TEST);
    }
}
EXPORT void GameApi::MainLoopApi::alpha(bool enable)
{
  alpha_1(enable);
}
void GameApi::MainLoopApi::alpha_1(bool enable)
{
  if (enable)
    {
      glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_COLOR /*ONE_MINUS_SRC_COLOR*/, GL_DST_COLOR);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef EMSCRIPTEN
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
      //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE); 
    }
  else
    {
      glDisable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
#ifndef EMSCRIPTEN
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
    }
}
EXPORT void GameApi::MainLoopApi::antialias(bool enable)
{
  if (enable)
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
    }
  else
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0);
    }
}

EXPORT float GameApi::MainLoopApi::get_time()
{
  return SDL_GetTicks()-time;
}
EXPORT void GameApi::MainLoopApi::reset_time()
{
  time = SDL_GetTicks();
}
EXPORT int GameApi::MainLoopApi::get_framenum()
{
  return frame;
}
EXPORT GameApi::BM GameApi::MainLoopApi::screenshot()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  //SDL_Surface *surf = p->screen;
  int w = p->screen_width; //surf->w;
  int h = p->screen_height; //surf->h;

  BufferRef ref = BufferRef::NewBuffer(w,h);
  glReadPixels(0,0,w,h, GL_BGRA, GL_UNSIGNED_BYTE, ref.buffer);
  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  return add_color_bitmap2(e, bm);
}

extern SDL_Window *sdl_window;

EXPORT void GameApi::MainLoopApi::finish()
{
  glFinish();
}
EXPORT void GameApi::MainLoopApi::swapbuffers()
{
#if 0
  GLenum e = glGetError();
  if (e!=GL_NO_ERROR)
    {
      std::cout << "swapbuffers GL error: " << e << std::endl;
    }
#endif
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //glLoadIdentity();
#ifdef SDL2_USED
  SDL_GL_SwapWindow(sdl_window);
#else
  SDL_GL_SwapBuffers();
#endif

  unsigned int time = SDL_GetTicks();
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame_time = time;
  //SDL_Flip(surf);
  frame++;
}
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i);
GameApi::SP GameApi::MainLoopApi::screenspace()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  SDL_Surface *surf = p->screen;
  int w = surf->w;
  int h = surf->h;
  SpaceImpl sp;
  Point tl( 0.0, 0.0, 0.0 );
  Point tr(float(w), 0.0, 0.0);
  Point bl(0.0, float(h),0.0);
  Point br(float(w), float(h),0.0);
  Point z(0.0,0.0,550.0);
  Point origo(0.0, 0.0,0.0);
  sp.tl = tl;
  sp.tr = tr;
  sp.bl = bl;
  sp.br = br;
  sp.z = z;
  sp.origo = origo;
  return add_space(e, sp);
}
EXPORT void GameApi::MainLoopApi::outline_first()
{
#ifndef EMSCRIPTEN
  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, -1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glColor4f(1.0,1.0,1.0,1.0);
#endif
}
EXPORT void GameApi::MainLoopApi::outline_second()
{
#ifndef EMSCRIPTEN
  glStencilFunc(GL_NOTEQUAL, 1, -1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glLineWidth(3);
  glEnable(GL_LINE_SMOOTH);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}
EXPORT void GameApi::MainLoopApi::outline_third()
{
#ifndef EMSCRIPTEN
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_STENCIL_TEST);
  //glDisable(GL_DEPTH_TEST);
#endif
}
EXPORT void GameApi::MainLoopApi::outline_disable()
{
  //glEnable(GL_DEPTH_TEST);
}

EXPORT bool GameApi::MainLoopApi::ch_doubletap_detect(Event &e, int expire_timer_count, int ch, DoubleTapState &state)
{
  bool doubletap = false;
  if (e.type==0x300 && e.ch == ch)
    { // down press
      if (state.in_between==true && frame - state.start_frame < expire_timer_count)
	{
	  doubletap = true;
	}
      state.start_frame = frame;
      state.in_between = false;
    }
  if (e.type==0x301 && e.ch == ch)
    {
      state.in_between = true;
    }
  return doubletap;
}
EXPORT GameApi::MainLoopApi::Event GameApi::MainLoopApi::get_event()
{
  SDL_Event event;
  Event e2;
  int last = SDL_PollEvent(&event);
  e2.last = last!=0;
  int x,y;
  int val = SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  e2.ch = event.key.keysym.sym;
  if (event.type==256) { exit(0); }

  if (event.type==SDL_FINGERMOTION||event.type==SDL_FINGERDOWN||event.type==SDL_FINGERUP)
    {
      SDL_TouchFingerEvent *ptr = &event.tfinger;
      x = (int)ptr->x;
      y = (int)ptr->y;
    }

  int id = 0;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (env->cursor_pos_point_id.id==-1)
    {
      GameApi::PT pt = add_point(e, x,y);
      id = pt.id;
    }
  else
    {
      id = env->cursor_pos_point_id.id;
    }
  env->pt[id].x = x;
  env->pt[id].y = y;
  e2.cursor_pos.id = id;
  e2.button = -1;
  if (val & SDL_BUTTON(1)) { e2.button = 0; }
  if (val & SDL_BUTTON(2)) { e2.button = 1; }
  if (val & SDL_BUTTON(3)) { e2.button = 2; }
  SDL_Joystick *joy1 = SDL_JoystickOpen(0);
  SDL_JoystickEventState(SDL_ENABLE);
  unsigned int but1 = SDL_JoystickGetButton(joy1, 0);  
  unsigned int but2 = SDL_JoystickGetButton(joy1, 1);
  unsigned int but3 = SDL_JoystickGetButton(joy1, 2);
  unsigned int but4 = SDL_JoystickGetButton(joy1, 3);
  //std::cout << e.type << " " << e.ch << std::endl;
  e2.joy0_button0 = but1==1;
  e2.joy0_button1 = but2==1;
  e2.joy0_button2 = but3==1;
  e2.joy0_button3 = but4==1;

  SDL_Joystick *joy2 = SDL_JoystickOpen(1);
  unsigned int a_but1 = SDL_JoystickGetButton(joy2, 0);  
  unsigned int a_but2 = SDL_JoystickGetButton(joy2, 1);
  unsigned int a_but3 = SDL_JoystickGetButton(joy2, 2);
  unsigned int a_but4 = SDL_JoystickGetButton(joy2, 3);
  //std::cout << a_but1 << " " << a_but2 << " " << a_but3 << " " << a_but4 << std::endl;
  e2.joy1_button0 = a_but1==1;
  e2.joy1_button1 = a_but2==1;
  e2.joy1_button2 = a_but3==1;
  e2.joy1_button3 = a_but4==1;

  return e2;
}
void GameApi::MainLoopApi::execute_ml(ML ml, SH color, SH texture, SH array_texture, const Event &ee, M in_MV, M in_T, M in_N)
{
  MainLoopItem *item = find_main_loop(e, ml);
  MainLoopEnv ek;
  ek.sh_color = color.id;
  ek.sh_texture = texture.id;
  ek.sh_array_texture = array_texture.id;
  ek.type = ee.type;
  ek.ch = ee.ch;
  ek.cursor_pos = *find_point(e,ee.cursor_pos);
  ek.button = ee.button;
  ek.env = find_matrix(e,in_MV);
  ek.in_MV = find_matrix(e, in_MV);
  ek.in_T = find_matrix(e, in_T);
  ek.in_N = find_matrix(e, in_N);
  item->execute(ek);
}

class ArrayMainLoop : public MainLoopItem
{
public:
  ArrayMainLoop(std::vector<MainLoopItem*> vec) : vec(vec) { }
  void execute(MainLoopEnv &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->execute(e);
      }
  }
  int shader_id() { return -1; }
private:
  std::vector<MainLoopItem*> vec;
};
int GameApi::MainLoopApi::get_screen_width()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_width;
}
int GameApi::MainLoopApi::get_screen_height()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_height;
}

GameApi::ML GameApi::MainLoopApi::array_ml(std::vector<ML> vec)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new ArrayMainLoop(vec2));
}
void GameApi::MainLoopApi::save_logo(EveryApi &ev)
{
  BM I1=ev.bitmap_api.newbitmap(500,300,0x00000000);
  Ft I2=ev.font_api.newfont("FreeSans.ttf",80,80);
  BM I3=ev.font_api.font_string(I2,"GameApi",5);
  BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
  Ft I5=ev.font_api.newfont("FreeSans.ttf",18,18);
  BM I6=ev.font_api.font_string(I5,"EmscriptenEdition",5);
  BM I7=ev.bitmap_api.blitbitmap(I4,I6,90,88);
  ev.bitmap_api.savebitmap(I7, "logo.ppm", true);
}
int frame_count=0;
struct LogoEnv
{
  GameApi::EveryApi *ev;
  GameApi::ML res;
  GameApi::SH color;
  GameApi::SH texture;
  GameApi::SH arr;
};
LogoEnv *logo_env = 0;
bool GameApi::MainLoopApi::logo_iter()
{
  LogoEnv *env = logo_env;
  GameApi::MainLoopApi::Event event = env->ev->mainloop_api.get_event();
  env->ev->mainloop_api.clear_3d();
  M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
  M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
  M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
  env->ev->mainloop_api.execute_ml(env->res, env->color, env->texture, env->arr, event,in_MV, in_T, in_N);
  env->ev->mainloop_api.swapbuffers();
  frame_count++;
  if (frame_count>300) {
    return true;
  }
  return false;
}
void GameApi::MainLoopApi::display_logo(EveryApi &ev)
{
#ifdef EMSCRIPTEN
  BM I7 = ev.bitmap_api.loadbitmap("logo.ppm");
  BM I7a = ev.bitmap_api.flip_y(I7);
  //P I8=ev.polygon_api.color_map(I7,500,300,0);
  //P I9=ev.polygon_api.rotatex(I8,3.14159);
  //P I10=ev.polygon_api.scale(I9,2,2,2);
  //P I11=ev.polygon_api.translate(I10,-400,200,0);
  //VA I12=ev.polygon_api.create_vertex_array(I11,true);
  //ML I13=ev.polygon_api.render_vertex_array_ml(ev,I12);
  VA va = ev.sprite_api.create_vertex_array(I7a);
  ML I13 = ev.sprite_api.render_sprite_vertex_array_ml(ev, va);
MN I14=ev.move_api.empty();
 MN I14a=ev.move_api.scale2(I14, 4,4,4);
 MN I14b=ev.move_api.trans2(I14a,-800,400,0);
MN I15=ev.move_api.rotatey(I14b,-1.59);
MN I16=ev.move_api.rotate(I15,0,30,0,0,0,0,1,0,1.59);
 MN I16a=ev.move_api.trans2(I16, -800,-1400,0);
ML I17=ev.move_api.move_ml(ev,I13,I16a);
 ML res = I17;
#else
BM I18=ev.bitmap_api.newbitmap(500,300,0x00000000);
Ft I19=ev.font_api.newfont("FreeSans.ttf",80,80);
BM I20=ev.font_api.font_string(I19,"GameApi",5);
BM I21=ev.bitmap_api.blitbitmap(I18,I20,0,0);
Ft I22=ev.font_api.newfont("FreeSans.ttf",18,18);
BM I23=ev.font_api.font_string(I22,"Win32Edition",5);
BM I24=ev.bitmap_api.blitbitmap(I21,I23,160,88);
P I25=ev.polygon_api.color_map(I24,500,300,0);
P I26=ev.polygon_api.rotatex(I25,3.14159);
P I27=ev.polygon_api.scale(I26,2,2,2);
P I28=ev.polygon_api.translate(I27,-400,200,0);
VA I29=ev.polygon_api.create_vertex_array(I28,true);
ML I30=ev.polygon_api.render_vertex_array_ml(ev,I29);
MN I31=ev.move_api.empty();
MN I32=ev.move_api.rotatey(I31,-1.59);
MN I33=ev.move_api.rotate(I32,0,30,0,0,0,0,1,0,1.59);
ML I34=ev.move_api.move_ml(ev,I30,I33);
 ML res = I34;
#endif
 SH color = ev.shader_api.colour_shader();  
 SH texture = ev.shader_api.texture_shader();
#ifdef EMSCRIPTEN
 SH arr = texture;
#else
 SH arr = ev.shader_api.texture_array_shader();
#endif
 ev.mainloop_api.init_3d(color);
 ev.mainloop_api.init_3d(texture);
 ev.mainloop_api.init_3d(arr);
 LogoEnv *env = new LogoEnv;
 env->ev = &ev;
 env->res = res;
 env->color = color;
 env->texture = texture;
 env->arr = arr;
 logo_env = env;
}
GameApi::M GameApi::MainLoopApi::in_N(EveryApi &ev, bool is_3d)
{
  if (is_3d)
    {
      return ev.matrix_api.identity();      
    }
  else
    {
      return ev.matrix_api.identity();      
    }
}
GameApi::M GameApi::MainLoopApi::in_T(EveryApi &ev, bool is_3d)
{
  if (is_3d)
    {
      return ev.matrix_api.trans(0.0,0.0,-500.0);      
    }
  else
    {
      return ev.matrix_api.identity();      
    }
}
GameApi::M GameApi::MainLoopApi::in_MV(EveryApi &ev, bool is_3d)
{
  if (is_3d)
    {
      return ev.matrix_api.identity();      
    }
  else
    {
      return ev.matrix_api.identity();      
    }
}
