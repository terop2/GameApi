
#include "GameApi_h.hh"
#include <chrono>

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
EXPORT int GameApi::MainLoopApi::get_framenum()
{
  return frame;
}
EXPORT GameApi::BM GameApi::MainLoopApi::screenshot()
{
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //SDL_Surface *surf = p->screen;
  int w = 800; //surf->w;
  int h = 600; //surf->h;

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
