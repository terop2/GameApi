
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
extern SDL_Window *sdl_window;

EXPORT void GameApi::MainLoopApi::init_window(int screen_width, int screen_height, std::string window_title)
{
  set_corner(0,0,screen_width, screen_height);
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
  p->screen = InitSDL2(screenx,screeny,false);
#else
  p->screen = InitSDL(screenx,screeny,false);
#endif
  SDL_SetWindowTitle(sdl_window, window_title.c_str());

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
  prog->set_var("in_iMV", Matrix::Inverse(m2));
  prog->set_var("in_T", m2);
  prog->set_var("in_POS", 0.0f);
  prog->set_var("color_mix", 0.5f);
  prog->set_var("time", 0.0f);
  int s = 64;
  for(int i=0;i<s;i++) {
    std::stringstream ss2; ss2 << i;
    std::string ss = "texsampler[" + ss2.str() + "]";
    prog->set_var(ss, i);
  }
  std::string ss = "cubesampler";
  prog->set_var(ss, 0);

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
EXPORT GameApi::PT GameApi::MainLoopApi::random_point_3d(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  float x = float(random())/float(rand_max());
  float y = float(random())/float(rand_max());
  float z = float(random())/float(rand_max());
  
  float xx = start_x + x*(end_x-start_x);
  float yy = start_y + y*(end_y-start_y);
  float zz = start_z + z*(end_z-start_z);
  return add_point(e, xx,yy,zz);
}

EXPORT GameApi::PT GameApi::MainLoopApi::random_point_2d(float start_x, float end_x, float start_y, float end_y, float z)
{
  float x = float(random())/float(rand_max());
  float y = float(random())/float(rand_max());
   
  float xx = start_x + x*(end_x-start_x);
  float yy = start_y + y*(end_y-start_y);
  float zz = z;
  return add_point(e, xx,yy,zz);
}

EXPORT GameApi::V GameApi::MainLoopApi::random_dir_vector_2d_xy(float length)
{
  float x = float(random())/float(rand_max());

  float alfa = 2.0*3.14159*x;

  float xx = length*cos(alfa);
  float yy = length*sin(alfa);
  return add_vector(e, xx,yy,0.0);
  
}
EXPORT GameApi::V GameApi::MainLoopApi::random_dir_vector_2d_xz(float length)
{

  float x = float(random())/float(rand_max());

  float alfa = 2.0*3.14159*x;

  float xx = length*cos(alfa);
  float yy = length*sin(alfa);
  return add_vector(e, xx,0.0,yy);

}
EXPORT GameApi::V GameApi::MainLoopApi::random_dir_vector_3d(float length)
{
  float x = float(random())/float(rand_max());
  float y = float(random())/float(rand_max());
  
  float alfa = 3.14159*y;
  float beta = 2.0*3.14159*x;

  float xx = length * sin(alfa) * cos(beta);
  float yy = length * sin(alfa) * sin(beta);
  float zz = length * cos(alfa);
  return add_vector(e,xx,yy,zz);
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
EXPORT float GameApi::MainLoopApi::fpscounter(bool print)
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
    if (print) {
    std::cout << "FPS: " << fps << " delta_time:" << p->avg_time/100.0 << std::endl;
    }
    p->count = 0;
  }
  return fps;
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
  prog->use(); // 80.0, 10.1, 60000.0
  Matrix m = Matrix::Perspective(90.0*double(screeny)/double(screenx), (double)screenx/screeny, 10.1, 60000.0);
  //Matrix m = Matrix::Perspective2(-300.0, 300.0, -300.0, 300.0, 1.0, 610.0);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  prog->set_var("in_iMV", Matrix::Inverse(m2));
  Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
  prog->set_var("in_T", m3);
  prog->set_var("in_POS", 0.0f);
  prog->set_var("color_mix", 0.5f); 
  prog->set_var("time",0.0f);
  int s = 64;
  for(int i=0;i<s;i++) {
    std::stringstream ss2; ss2 << i;
    std::string ss = "texsampler[" + ss2.str() + "]";
    prog->set_var(ss, i);
  }
  std::string ss = "cubesampler";
  prog->set_var(ss,0);
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

EXPORT void GameApi::MainLoopApi::set_corner(int x, int y, int screen_sx, int screen_sy)
{
    MainLoopPriv *p = (MainLoopPriv*)priv;
    p->corner_x = x;
    p->corner_y = y;
    p->screen_rect_sx = screen_sx;
    p->screen_rect_sy = screen_sy;
}
EXPORT int GameApi::MainLoopApi::get_corner_x()
{
    MainLoopPriv *p = (MainLoopPriv*)priv;
    return p->corner_x;
}
EXPORT int GameApi::MainLoopApi::get_corner_y()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->corner_y;
}
EXPORT int GameApi::MainLoopApi::get_screen_rect_sx()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_rect_sx;
}
EXPORT int GameApi::MainLoopApi::get_screen_rect_sy()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_rect_sy;
}
EXPORT void GameApi::MainLoopApi::set_screen_size(int sx, int sy)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->screen_width = sx;
  p->screen_height = sy;
}
EXPORT int GameApi::MainLoopApi::get_screen_sx()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_width;
}
EXPORT int GameApi::MainLoopApi::get_screen_sy()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  return p->screen_height;
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
      prog->set_var("in_iMV", Matrix::Inverse(m2));
      Matrix m3 = Matrix::Identity();
      prog->set_var("in_T", m3);
    }
}

extern std::string gameapi_seamless_url;
EXPORT void GameApi::MainLoopApi::clear(unsigned int col)
{
  if (gameapi_seamless_url=="") {
  //glClearColor(255,255,255,255);
  glClearStencil(0);
  Color c(col);
  glClearColor(c.rf(),c.gf(),c.bf(),c.af());
  glStencilMask(~0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  } else {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
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
  
  if (gameapi_seamless_url=="") {
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
  } else {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
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
EXPORT float GameApi::MainLoopApi::get_delta_time()
{
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  return pp->delta_time;
}
EXPORT void GameApi::MainLoopApi::reset_time()
{
  time = SDL_GetTicks();
}
EXPORT void GameApi::MainLoopApi::advance_time(float val)
{
  time+=val;
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

  glReadPixels(0,0,w,h, GL_RGBA /*GL_BGRA*/, GL_UNSIGNED_BYTE, ref.buffer);

  for(int y=0;y<h;y++)
    for(int x=0;x<w;x++)
      {
	unsigned int pix = ref.buffer[x+y*ref.ydelta];
	// opengl format
	unsigned int cr = pix & 0xff000000;
	unsigned int cg = pix & 0x00ff0000;
	unsigned int cb = pix & 0x0000ff00;
	unsigned int ca = pix & 0x000000ff;
	cr>>=24;
	cg>>=16;
	cb>>=8;
	ca<<=24;
	cr<<=16;
	cg<<=8;
	ref.buffer[x+y*ref.ydelta] = cr+cg+cb+ca;
      }

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
  int e = -1;
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  int i = 0;
  while((e=glGetError()) != 0) {
    i++;
    if (e!=0 && e != pp->last_error)
      {
	std::string error="";
	switch(e) {
	case 0x500: error="GL_INVALID_ENUM"; break;
	case 0x501: error="GL_INVALID_VALUE"; break;
	case 0x502: error="GL_INVALID_OPERATION"; break;
	case 0x503: error="GL_STACK_OVERFLOW"; break;
	case 0x504: error="GL_STACK_UNDERFLOW"; break;
	case 0x505: error="GL_OUT_OF_MEMORY"; break;
	case 0x506: error="GL_INVALID_FRAMEBUFFER_OPERATION"; break;
	case 0x507: error="GL_CONTEXT_LOST"; break;
	case 0x8031: error="GL_TABLE_TOO_LARGE"; break;
      };
	std::cout << "swapbuffers GL error: "<< i << ":"  << e << " " << error << std::endl;
  pp->last_error = e;
      }
  }
#endif

#if 0
  if (GLEW_NVX_gpu_memory_info ) // seems I don't have extension available
    {
      int readint = -1;
      glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &readint );

      int readgpuint = -1;
      glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &readgpuint);
      std::cout << "MEMORY AVAILABLE: " << readint * 1024 << std::endl;
      std::cout << "GPU MEMORY AVAILABLE: " << readgpuint * 1024 << std::endl;
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
  unsigned int delta = time - p->previous_frame_time;
  p->delta_time = float(delta)/100.0;
  p->current_time = float(time)/100.0;

  p->previous_frame_time = time;
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
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //e2.current_time = p->current_time;
  //e2.delta_time = p->delta_time;
  int x,y;
  int mouse_wheel_y = 0;
  int val = SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  e2.ch = event.key.keysym.sym;
  if (event.type==256) { exit(0); }

  if (event.type==SDL_FINGERMOTION||event.type==SDL_FINGERDOWN||event.type==SDL_FINGERUP)
    {
      SDL_TouchFingerEvent *ptr = &event.tfinger;
      x = int(ptr->x * get_screen_width());
      y = int(ptr->y * get_screen_height());
    }
  if (event.type==SDL_MOUSEWHEEL)
    {
      SDL_MouseWheelEvent *ptr = &event.wheel;
      mouse_wheel_y = ptr->y;
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
  e2.mouse_wheel_y = mouse_wheel_y;

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
class SeqML : public MainLoopItem
{
public:
  SeqML(std::vector<MainLoopItem*> vec, float time) : vec(vec), time(time) { num2 = -1; firsttime = true; }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime)
      {
	int s = vec.size();
	for(int i=s-1;i>=0;i--)
	  {
	    vec[i]->execute(e);
	  }
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
      firsttime = false;
      return;
      }
    
    float num = e.time*10.0/time;
    num2 = (int)num;
    //float newtime = fmod(e.time,time/10.0);
    MainLoopEnv ee = e;
    // ee.time trick doesn't work with moveml
    //ee.time = newtime;
    int s = vec.size();
    if (num2>=0 && num2<s) {
      vec[num2]->execute(ee);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      vec[num2]->handle_event(e);
    }
  }
  virtual int shader_id() {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      return vec[num2]->shader_id();
    } else { return -1; }
  }
private:
  std::vector<MainLoopItem*> vec;
  float time;
  int num2;
  bool firsttime;
};

extern int score;
extern int hidden_score;

class SeqMLScore : public MainLoopItem
{
public:
  SeqMLScore(MainLoopItem *ml1, MainLoopItem *ml2, int target_score) : target_score(target_score) {
    vec.push_back(ml1);
    vec.push_back(ml2);
    num2 = -1; firsttime = true;
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime)
      {
	vec[1]->execute(e);
	vec[0]->execute(e);
      firsttime = false;
      return;
      }
    
    //float num = e.time*10.0/time;
    num2 = hidden_score>target_score?1:0;
    //float newtime = fmod(e.time,time/10.0);
    MainLoopEnv ee = e;
    // ee.time trick doesn't work with moveml
    //ee.time = newtime;
    int s = vec.size();
    if (num2>=0 && num2<s) {
      vec[num2]->execute(ee);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      vec[num2]->handle_event(e);
    }
  }
  virtual int shader_id() {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      return vec[num2]->shader_id();
    } else { return -1; }
  }
private:
  std::vector<MainLoopItem*> vec;
  int target_score;
  int num2;
  bool firsttime;
};


class CollisionGenKey : public MainLoopItem
{
public:
  CollisionGenKey(MainLoopItem *curr, std::string obj1, std::string obj2, int key, float keypress_duration) : curr(curr), obj1(obj1), obj2(obj2), key(key), keypress_duration(keypress_duration) 
  {
    collision_detected=false;
    keypress_ongoing=false;
  }
  void handle_event(MainLoopEvent &e)
  {
    curr->handle_event(e);
  }
  void execute(MainLoopEnv &e)
  {
    curr->execute(e);
 
    check_world(e);
    World *w = e.current_world;
    std::vector<std::pair<std::string,std::string> > &coll_res = w->collisions;
    int s = coll_res.size();
    collision_detected = false;
    if (!keypress_ongoing)
    for(int i=0;i<s;i++)
      {
	if ((coll_res[i].first.find(obj1)!=std::string::npos && coll_res[i].second.find(obj2)!=std::string::npos) ||
	    (coll_res[i].second.find(obj1)!=std::string::npos && coll_res[i].first.find(obj2)!=std::string::npos)) {
	  collision_detected = true;
	}
      }
    if (collision_detected && !keypress_ongoing)
      {
	keypress_starttime = e.time;
	MainLoopEvent ev2;
	ev2.type=768;
	ev2.ch=key;
	ev2.cursor_pos=Point(0.0,0.0,0.0);
	ev2.button=-1;
	curr->handle_event(ev2);
	keypress_ongoing = true;
      }
    if (keypress_ongoing)
      {
	float delta = e.time - keypress_starttime;
	if (delta*10.0>keypress_duration) {
	  MainLoopEvent ev2;
	  ev2.type=769;
	  ev2.ch=key;
	  ev2.cursor_pos=Point(0.0,0.0,0.0);
	  ev2.button=-1;
	  curr->handle_event(ev2);
	  keypress_ongoing = false;
	}
      }

  }
private:
  MainLoopItem *curr;
  std::string obj1;
  std::string obj2;
  int key;
  bool collision_detected;
  float keypress_duration;
  float keypress_starttime;
  bool keypress_ongoing;
};

class CollisionSeqML : public MainLoopItem
{
public:
  CollisionSeqML(MainLoopItem *curr, MainLoopItem *end, std::string obj1, std::string obj2, float show_duration) : curr(curr), end(end), obj1(obj1), obj2(obj2), show_duration(show_duration) { 
    collision_detected=false; 
    firsttime = true; 
    old_collision_detected=false; 
    current_item = false; 
  }
  void handle_event(MainLoopEvent &e)
  {
    if (current_item) {
      end->handle_event(e);
    } else {
      curr->handle_event(e);
    }
  }

  void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      end->execute(e);
      curr->execute(e);
      firsttime = false;
      return;
    }
    if (current_item) {
      MainLoopEnv ee = e;
      float tt = e.time - chosen_time;
      if (tt*10.0 > show_duration) { current_item = false; }
      end->execute(ee);
    } else {
      curr->execute(e);
    }

    check_world(e);
    World *w = e.current_world;
    std::vector<std::pair<std::string,std::string> > &coll_res = w->collisions;
    int s = coll_res.size();
    collision_detected = false;
    for(int i=0;i<s;i++)
      {
	if ((coll_res[i].first.find(obj1)!=std::string::npos && coll_res[i].second.find(obj2)!=std::string::npos) ||
	    (coll_res[i].second.find(obj1)!=std::string::npos && coll_res[i].first.find(obj2)!=std::string::npos)) {
	  collision_detected = true;
	}
      }
    if (!current_item) {
	if (!old_collision_detected && collision_detected) {
	  current_item = true;
	  chosen_time = e.time;
	}
    }

    old_collision_detected = collision_detected;

  }
private:
  MainLoopItem *curr, *end;
  std::string obj1, obj2;
  float show_duration;
  float chosen_time;
  bool current_item;
  bool collision_detected;
  bool firsttime;
  bool old_collision_detected;
};

class TimedTmpSeqML : public MainLoopItem
{
public:
  TimedTmpSeqML(MainLoopItem *curr, MainLoopItem *end, float start_time, float end_time, float show_duration, int key) : curr(curr), end(end), start_time(start_time), end_time(end_time), show_duration(show_duration), current_item(false), key_pressed(false), key(key) { firsttime = true; old_key_pressed = false; }
  void handle_event(MainLoopEvent &e)
  {
    if (e.type==0x300 && e.ch==key)
      {
	key_pressed = true;
      }
    if (e.type==0x301 && e.ch==key)
      {
	key_pressed = false;
      }
    if (current_item) {
      end->handle_event(e);
    } else {
      curr->handle_event(e);
    }
	   
  }
  void execute(MainLoopEnv &e)
  {
    #if 1
    if (firsttime)
      {
      end->execute(e);
      curr->execute(e);
      firsttime = false;
      return;
      }
    #endif
    if (e.time*10.0>=start_time && e.time*10.0 <end_time && !current_item)
      {
	if (old_key_pressed && !key_pressed) {
	  current_item = true;
	  chosen_time = e.time;
	}
	
      }

    if (current_item) {
      MainLoopEnv ee = e;
      float tt = e.time - chosen_time;
      if (tt*10.0 > show_duration) { current_item = false; }
      end->execute(ee);
    } else {
      curr->execute(e);
    }
    old_key_pressed = key_pressed;
  }
  int shader_id() {
    if (current_item) {
      return end->shader_id();
    } else {
      return curr->shader_id();
    }
  }
  
private:
  MainLoopItem *curr;
  MainLoopItem *end;
  float start_time;
  float end_time;
  float show_duration;
  float chosen_time;
  bool current_item;
  bool key_pressed;
  int key;
  bool firsttime;
  bool old_key_pressed;
};

class CollisionDetection : public MainLoopItem
{
public:
  CollisionDetection(GameApi::EveryApi &ev, float player_size, float enemy_size,
		     MainLoopItem* normal_game_screen, MainLoopItem *gameover_screen)
    : ev(ev), player_size(player_size), enemy_size(enemy_size), normal_game_screen(normal_game_screen), gameover_screen(gameover_screen)
  {
  }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    Point p = e.current_world->player_pos;
    Matrix m = e.current_world->player_matrix;
    Point pl_pos = p * m;

    int s = e.current_world->enemy_pieces.size();
    for(int i=0;i<s;i++)
      {
	EnemyPiece &piece = get_enemy_piece(e, i);
	Point p2 = piece.enemy_position;
	Matrix m2 = e.current_world->enemy_matrix;
	Point en_pos = p2 * m2;

	Vector v = pl_pos - en_pos;
	float d = v.Dist();
	if (d<player_size+enemy_size)
	  { // hit
	    state = 1;  
	  }
      }

    
    if (state==0) { normal_game_screen->execute(e); }
    else { gameover_screen->execute(e); }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (state==0) { normal_game_screen->handle_event(e); }
    else { gameover_screen->handle_event(e); }
  }
  virtual int shader_id() {
    if (state==0) { return normal_game_screen->shader_id(); }
    else { return gameover_screen->shader_id(); }
  }

private:
  GameApi::EveryApi &ev;
  float player_size;
  Point *player_pos;
  Movement *player_trans;
  float enemy_size;
  PointsApiPoints *enemy_pos;
  Movement *enemy_trans;
  MainLoopItem *normal_game_screen;
  MainLoopItem *gameover_screen;
private:
  int state=0;
};
GameApi::ML GameApi::MainLoopApi::timed_tmp_seq_ml(ML curr, ML end, float start_time, float end_time, float show_duration, int key)
{
  MainLoopItem *curr_item = find_main_loop(e, curr);
  MainLoopItem *end_item = find_main_loop(e, end);
  return add_main_loop(e, new TimedTmpSeqML(curr_item, end_item, start_time, end_time, show_duration, key));
}
GameApi::ML GameApi::MainLoopApi::collision_seq_ml(ML curr, ML end, std::string obj1, std::string obj2, float show_duration)
{
  MainLoopItem *curr_item = find_main_loop(e, curr);
  MainLoopItem *end_item = find_main_loop(e, end);
  return add_main_loop(e, new CollisionSeqML(curr_item, end_item, obj1, obj2, show_duration));
}
GameApi::ML GameApi::MainLoopApi::collision_gen_key(ML curr, std::string obj1, std::string obj2, int key, float duration)
{
  MainLoopItem *curr_item = find_main_loop(e, curr);
  return add_main_loop(e, new CollisionGenKey(curr_item, obj1, obj2, key, duration));
}

GameApi::ML GameApi::MainLoopApi::collision_detection(EveryApi &ev,
						      float player_size, 
						      float enemy_size, 
						      ML normal_game_screen, ML gameover_screen)
{
  MainLoopItem *game_screen = find_main_loop(e, normal_game_screen);
  MainLoopItem *gameover = find_main_loop(e, gameover_screen);
  return add_main_loop(e, new CollisionDetection(ev,player_size, 
						 enemy_size,
						 game_screen, gameover));
}
GameApi::ML GameApi::MainLoopApi::seq_ml(std::vector<ML> vec, float time)
{
  int s = vec.size();
  std::vector<MainLoopItem*> vec2;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = find_main_loop(e, vec[i]);
      vec2.push_back(item);
    }
  return add_main_loop(e, new SeqML(vec2, time));
}
GameApi::ML GameApi::MainLoopApi::seq_ml_score(ML ml1, ML ml2, int score)
{
  MainLoopItem *main1 = find_main_loop(e,ml1);
  MainLoopItem *main2 = find_main_loop(e,ml2);
  return add_main_loop(e, new SeqMLScore(main1,main2, score));
}
void GameApi::MainLoopApi::execute_ml(ML ml, SH color, SH texture, SH texture_2d, SH array_texture, M in_MV, M in_T, M in_N, int screen_size_x, int screen_size_y)
{
  MainLoopItem *item = find_main_loop(e, ml);
  MainLoopEnv ek;
  ek.sh_color = color.id;
  ek.sh_texture_2d = texture_2d.id;
  ek.sh_texture = texture.id;
  ek.sh_array_texture = array_texture.id;
  //ek.type = ee.type;
  //ek.ch = ee.ch;
  //ek.cursor_pos = *find_point(e,ee.cursor_pos);
  //ek.button = ee.button;
  ek.env = find_matrix(e,in_MV);
  ek.in_MV = find_matrix(e, in_MV);
  ek.in_T = find_matrix(e, in_T);
  ek.in_N = find_matrix(e, in_N);
  ek.time = get_time()/1000.0;
  ek.delta_time = get_delta_time();
  ek.screen_x = 0;
  ek.screen_y = 0;
  ek.screen_width = screen_size_x;
  ek.screen_height = screen_size_y;
  item->execute(ek);
}
void GameApi::MainLoopApi::event_ml(ML ml, const Event &ee)
{
  MainLoopEvent e2;
  e2.type = ee.type;
  e2.ch = ee.ch;
  e2.cursor_pos = *find_point(e,ee.cursor_pos);
  e2.button = ee.button;
  MainLoopItem *item = find_main_loop(e, ml);
  item->handle_event(e2);
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
	MainLoopEnv ee = e;
	vec[i]->execute(ee);
      }
  }
  void handle_event(MainLoopEvent &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->handle_event(e);
      }
  }
#if 0
  int shader_id() { 
    int s = vec.size();
    for(int i=s-1;i>=0;i--)
      {
	if (vec[i]->shader_id()!=-1) return vec[i]->shader_id();
      }
    return -1; 
  }
#endif
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
void GameApi::MainLoopApi::set_viewport(int x, int y, int sx, int sy)
{
  glViewport(x,y,sx,sy);
}

EXPORT GameApi::ML GameApi::MainLoopApi::array_ml(std::vector<ML> vec)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "array_ml id: " << vec[i].id << std::endl;
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new ArrayMainLoop(vec2));
}
void GameApi::MainLoopApi::save_logo(EveryApi &ev)
{
  BM I1=ev.bitmap_api.newbitmap(500,300,0x00000000);
  Ft I2=ev.font_api.newfont("http://tpgames.org/FreeSans.ttf",80,80);
  BM I3=ev.font_api.font_string(I2,"qtamoVR",5);
  BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
  Ft I5=ev.font_api.newfont("http://tpgames.org/FreeSans.ttf",18,18);
  BM I6=ev.font_api.font_string(I5,"Loading",5);
  BM I7=ev.bitmap_api.blitbitmap(I4,I6,80,88);
  ev.bitmap_api.savebitmap(I7, "logo.ppm", true);
}
int frame_count=0;
struct LogoEnv
{
  GameApi::EveryApi *ev;
  GameApi::ML res;
  GameApi::SH color;
  GameApi::SH texture;
  GameApi::SH texture_2d;
  GameApi::SH arr;
};
LogoEnv *logo_env = 0;
bool GameApi::MainLoopApi::logo_iter()
{
  LogoEnv *env = logo_env;
  /*GameApi::MainLoopApi::Event event =*/ env->ev->mainloop_api.get_event();
  env->ev->mainloop_api.clear_3d();
  M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
  M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
  M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
  env->ev->mainloop_api.execute_ml(env->res, env->color, env->texture, env->texture_2d, env->arr,in_MV, in_T, in_N, get_screen_sx(), get_screen_sy());
  env->ev->mainloop_api.swapbuffers();
  frame_count++;
  if (frame_count>300) {
    return true;
  }
  return false;
}
bool GameApi::MainLoopApi::seamless_iter()
{
  LogoEnv *env = logo_env;
  env->ev->mainloop_api.clear_3d();
  env->ev->mainloop_api.swapbuffers();
  frame_count++;
  if (frame_count>300) {
    return true;
  }
  return false;

}
void GameApi::MainLoopApi::display_seamless(EveryApi &ev)
{
}
void GameApi::MainLoopApi::display_logo(EveryApi &ev)
{
#ifdef EMSCRIPTEN
  BM I7 = ev.bitmap_api.loadbitmap("web_page/logo.ppm");
  BM I7a = ev.bitmap_api.flip_y(I7);
  //P I8=ev.polygon_api.color_map(I7,500,300,0);
  //P I9=ev.polygon_api.rotatex(I8,3.14159);
  //P I10=ev.polygon_api.scale(I9,2,2,2);
  //P I11=ev.polygon_api.translate(I10,-400,200,0);
  //VA I12=ev.polygon_api.create_vertex_array(I11,true);
  //ML I13=ev.polygon_api.render_vertex_array_ml(ev,I12);
  //VA va = ev.sprite_api.create_vertex_array(I7a);
  ML I13 = ev.sprite_api.render_sprite_vertex_array_ml(ev, I7a);
  MN I14=ev.move_api.empty();
  MN I14a=ev.move_api.scale2(I14, 4,4,4);
  MN I14b=ev.move_api.trans2(I14a,-800,400,0);
  MN I15=ev.move_api.rotatey(I14b,-1.59);
  MN I16=ev.move_api.rotate(I15,0,30,0,0,0,0,1,0,1.59);
  MN I16a=ev.move_api.trans2(I16, -800,-1400,0);
  ML I17=ev.move_api.move_ml(ev,I13,I16a);

  ML I26;
  { // PROGRESS BAR
BB I1=ev.bool_bitmap_api.empty(92,12);
BB I2=ev.bool_bitmap_api.rectangle(I1,0,0,92,12);
BM I3=ev.bool_bitmap_api.to_bitmap(I2,255,255,255,255,0,0,0,0);
ML I4=ev.sprite_api.vertex_array_render(ev,I3);
MN I5=ev.move_api.empty();
MN I6=ev.move_api.trans2(I5,4,4,0);
MN I7=ev.move_api.scale_progress(I6,true,false,false);
ML I8=ev.move_api.move_ml(ev,I4,I7,1,10.0);
MN I9=ev.move_api.empty();
MN I10=ev.move_api.scale2(I9,5,5,1);
MN I11=ev.move_api.trans2(I10,-230,-300,0);
ML I12=ev.move_api.move_ml(ev,I8,I11,1,10.0);
BB I13=ev.bool_bitmap_api.empty(100,20);
BB I14=ev.bool_bitmap_api.rectangle(I13,0,0,100,2);
BB I15=ev.bool_bitmap_api.rectangle(I14,0,18,100,2);
BB I16=ev.bool_bitmap_api.rectangle(I15,0,0,2,20);
BB I17=ev.bool_bitmap_api.rectangle(I16,98,0,2,20);
BM I18=ev.bool_bitmap_api.to_bitmap(I17,255,255,255,255,0,0,0,0);
ML I19=ev.sprite_api.vertex_array_render(ev,I18);
MN I20=ev.move_api.empty();
MN I21=ev.move_api.scale2(I20,5,5,1);
MN I22=ev.move_api.trans2(I21,-230,-300,0);
ML I23=ev.move_api.move_ml(ev,I19,I22,1,10);
I26=ev.mainloop_api.array_ml(std::vector<ML>{I12,I23});
  }
  ML I17a = ev.mainloop_api.array_ml(std::vector<ML>{I17,I26});

 ML res = I17a;
#else
BM I18=ev.bitmap_api.newbitmap(500,300,0x00000000);
 FI I19 = ev.font_api.load_font("http://tpgames.org/FreeSans.ttf", 80,80);
 BM I20 = ev.font_api.draw_text_string(I19, "GameApi", 5, 30);

 //Ft I19=ev.font_api.newfont("http://tpgames.org/FreeSans.ttf",80,80);
 //BM I20=ev.font_api.font_string(I19,"GameApi",5);
BM I21=ev.bitmap_api.blitbitmap(I18,I20,0,0);
//Ft I22=ev.font_api.newfont("http://tpgames.org/FreeSans.ttf",18,18);
//BM I23=ev.font_api.font_string(I22,"Win32Edition",5);
 FI I22 = ev.font_api.load_font("http://tpgames.org/FreeSans.ttf", 18,18);
 BM I23 = ev.font_api.draw_text_string(I22, "Win32Edition", 5, 30);
BM I24=ev.bitmap_api.blitbitmap(I21,I23,160,88);
 P I25=ev.polygon_api.color_map(I24,0.0,500,0.0,300.0,0);
 //P I26=ev.polygon_api.rotatex(I25,3.14159);
P I27=ev.polygon_api.scale(I25,2,-2,2);
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
 SH texture_2d = texture;
#ifdef EMSCRIPTEN
 SH arr = texture;
#else
 SH arr = ev.shader_api.texture_array_shader();
#endif
 ev.mainloop_api.init_3d(color);
 ev.mainloop_api.init_3d(texture);
 ev.mainloop_api.init_3d(texture_2d);
 ev.mainloop_api.init_3d(arr);
 LogoEnv *env = new LogoEnv;
 env->ev = &ev;
 env->res = res;
 env->color = color;
 env->texture = texture;
 env->texture_2d = texture_2d;
 env->arr = arr;
 logo_env = env;
 frame_count = 0;
}
GameApi::M GameApi::MainLoopApi::in_P(EveryApi &ev, bool is_3d)
{
  int screenx = get_screen_width();
  int screeny = get_screen_height();
  if (is_3d)
    {
      return ev.matrix_api.perspective(80.0, (double)screenx/screeny, 10.1, 60000.0);
    }
  else
    {
      return ev.matrix_api.ortho(0,screenx, screeny,0,0,1);
    }

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
std::string gameapi_homepageurl;
std::string GameApi::MainLoopApi::get_homepage_url()
{
  return gameapi_homepageurl;
}
std::string gameapi_seamless_url;
void GameApi::MainLoopApi::set_seamless_url(std::string url)
{
  gameapi_seamless_url = url;
}
void GameApi::MainLoopApi::set_homepage_url(std::string url)
{
  std::cout << "Homepage set to: " << url << std::endl;
  gameapi_homepageurl = url;
}
class SongML : public MainLoopItem
{
public:
  SongML(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, MainLoopItem *next, std::string homepage) : env(env), ev(ev), url(url), next(next), homepage(homepage) {
    firsttime = true;
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
#ifndef EMSCRIPTEN
  env.async_load_url(url, homepage);
#endif      
      std::vector<unsigned char> *ptr = env.get_loaded_async_url(url);
      std::ofstream ss("song.ogg", std::ofstream::out | std::ofstream::binary);
      int s = ptr->size();
      for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
      ss.close();
      ev.tracker_api.play_ogg("song.ogg");
      firsttime = false;
    }
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  bool firsttime;
  MainLoopItem *next;
  std::string homepage;
};

GameApi::ML GameApi::MainLoopApi::load_song(EveryApi &ev, ML next, std::string url)
{
  MainLoopItem *nxt = find_main_loop(e, next);
  std::string homepage = ev.mainloop_api.get_homepage_url();
  return add_main_loop(e, new SongML(e,ev, url, nxt, homepage));
}

// note, BM's need to be 100x100 bitmaps
GameApi::ML GameApi::MainLoopApi::skybox(EveryApi &ev, BM I9_land, BM I15_sky)
{
  BM I9 = ev.bitmap_api.scale_bitmap(ev, I9_land, 100, 100);
  BM I15 = ev.bitmap_api.scale_bitmap(ev, I15_sky, 100,100);
  
  PT I1=ev.point_api.point(0.0,0.0,0.0);
  P I2=ev.polygon_api.sphere(I1,10000,30,30);
  P I3=ev.polygon_api.rotatex(I2,1.59);
  BM I4=ev.bitmap_api.newbitmap(200,100,00000000);
  BM I10=ev.bitmap_api.blitbitmap(I4,I9,0,0);
  BM I16=ev.bitmap_api.blitbitmap(I10,I15,100,0);
  MT I17=ev.materials_api.texture(ev,I16,1.0);
  ML I18=ev.materials_api.bind(I3,I17);
  return I18;
}

class KeyboardToggle : public MainLoopItem
{
public:
  KeyboardToggle(MainLoopItem *item, MainLoopItem *item2, int key) : key(key), item(item), item2(item2) { hold=false; toggle=false; firsttime=true; }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) { item->execute(e); item2->execute(e); firsttime=false; return; }
    if (!toggle) { item->execute(e); } else { item2->execute(e); } 
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (e.type==0x300 && e.ch==key && !hold) { hold=true; toggle=!toggle; }
    if (e.type==0x301 && e.ch==key) { hold=false; }
    if (!toggle) { item->handle_event(e); } else { item2->handle_event(e); } 
  }
  virtual int shader_id() { if (!toggle) return item->shader_id(); else return item2->shader_id(); }

private:
  int key;
  MainLoopItem *item;
  MainLoopItem *item2;
  bool toggle;
  bool hold;
  bool firsttime;
};
GameApi::ML GameApi::MainLoopApi::keyboard_toggle(ML m1, ML m2, int key)
{
  MainLoopItem *item = find_main_loop(e, m1);
  MainLoopItem *item2 = find_main_loop(e, m2);
  return add_main_loop(e, new KeyboardToggle(item, item2, key));
}
class PreparePTS : public MainLoopItem
{
public:
  PreparePTS(MainLoopItem *item, PointsApiPoints *points) : item(item), points(points), firsttime(true) {}
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
      points->Prepare();
    }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  MainLoopItem *item;
  PointsApiPoints *points;
  bool firsttime;
};
GameApi::ML GameApi::MainLoopApi::prepare_pts(ML ml, PTS pts)
{
  MainLoopItem *item = find_main_loop(e,ml);
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_main_loop(e, new PreparePTS(item, points));
}

class DepthFunc : public MainLoopItem
{
public:
  DepthFunc(MainLoopItem *next, int i) : next(next),i(i) {}
  virtual void execute(MainLoopEnv &e) {
    switch(i) {
    case 0: glDepthFunc(GL_LEQUAL); break;
    case 1: glDepthFunc(GL_EQUAL); break;
    case 2: glDepthFunc(GL_ALWAYS); break;
    case 3: glDepthFunc(GL_LESS); break;
    case 4: glDepthFunc(GL_GREATER); break;
    case 5: glDepthFunc(GL_GEQUAL); break;
    };
    next->execute(e);
    glDepthFunc(GL_LEQUAL);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  int i;
};

GameApi::ML GameApi::MainLoopApi::depthfunc(ML ml, int val)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new DepthFunc(next,val));
}

class BlendFunc : public MainLoopItem
{
public:
  BlendFunc(MainLoopItem *next, int i, int i2) : next(next),i(i),i2(i2) {}
  virtual void execute(MainLoopEnv &e) {
    GLenum s = GL_SRC_COLOR;
    GLenum d = GL_ONE_MINUS_SRC_COLOR;
    switch(i) {
    case 0: s=GL_ZERO; break;
    case 1: s=GL_ONE; break;
    case 2: s=GL_SRC_COLOR; break;
    case 3: s=GL_ONE_MINUS_SRC_COLOR; break;
    case 4: s=GL_DST_COLOR; break;
    case 5: s=GL_ONE_MINUS_DST_COLOR; break;
    case 6: s=GL_SRC_ALPHA; break;
    case 7: s=GL_ONE_MINUS_SRC_ALPHA; break;
    case 8: s=GL_DST_ALPHA; break;
    case 9: s=GL_ONE_MINUS_DST_ALPHA; break;
    case 10: s=GL_CONSTANT_COLOR; break;
    case 11: s=GL_ONE_MINUS_CONSTANT_COLOR; break;
    case 12: s=GL_CONSTANT_ALPHA; break;
    };

    switch(i2) {
    case 0: d=GL_ZERO; break;
    case 1: d=GL_ONE; break;
    case 2: d=GL_SRC_COLOR; break;
    case 3: d=GL_ONE_MINUS_SRC_COLOR; break;
    case 4: d=GL_DST_COLOR; break;
    case 5: d=GL_ONE_MINUS_DST_COLOR; break;
    case 6: d=GL_SRC_ALPHA; break;
    case 7: d=GL_ONE_MINUS_SRC_ALPHA; break;
    case 8: d=GL_DST_ALPHA; break;
    case 9: d=GL_ONE_MINUS_DST_ALPHA; break;
    case 10: d=GL_CONSTANT_COLOR; break;
    case 11: d=GL_ONE_MINUS_CONSTANT_COLOR; break;
    case 12: d=GL_CONSTANT_ALPHA; break;
    };
    glBlendFunc(s,d);
    next->execute(e);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  int i,i2;
};


GameApi::ML GameApi::MainLoopApi::blendfunc(ML ml, int val, int val2)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new BlendFunc(next,val,val2));
}

GameApi::ML GameApi::MainLoopApi::restart_screen(EveryApi &ev, ML ml, std::string fontname)
{
  BM I1=ev.bitmap_api.newbitmap(500,30,00000000);
  FI I2=ev.font_api.load_font(fontname,30,30);
  BM I3=ev.font_api.draw_text_string(I2,"Press",5,30);
  BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
  FI I5=ev.font_api.load_font(fontname,30,30);
  BM I6=ev.font_api.draw_text_string(I5,"R",5,30);
  BM I7=ev.bitmap_api.blitbitmap(I4,I6,160,0);
  FI I8=ev.font_api.load_font(fontname,30,30);
  BM I9=ev.font_api.draw_text_string(I8,"to",5,30);
  BM I10=ev.bitmap_api.blitbitmap(I7,I9,210,0);
  FI I11=ev.font_api.load_font(fontname,30,30);
  BM I12=ev.font_api.draw_text_string(I11,"Restart",5,30);
  BM I13=ev.bitmap_api.blitbitmap(I10,I12,285,0);
  ML I14=ev.sprite_api.vertex_array_render(ev,I13);
  MN I15=ev.move_api.empty();
  MN I16=ev.move_api.trans2(I15,150,300,0);
  ML I17=ev.move_api.move_ml(ev,I14,I16,1,10.0);
  ML I18=ev.sprite_api.turn_to_2d(ev,I17,0.0,0.0,800.0,600.0);
  ML I19=ev.mainloop_api.array_ml(std::vector<ML>{I18,ml});
  return I19;
}
