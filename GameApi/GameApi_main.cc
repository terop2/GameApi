
#include "GameApi_h.hh"
#include <chrono>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#define NO_MV 1

void *setup_midi(const std::vector<unsigned char> &data, const std::vector<unsigned char> &patchset);
void play_midi(void *ptr);


EXPORT GameApi::MainLoopApi::MainLoopApi(Env &e) : frame(0.0), time(0.0), e(e)  
{
  priv = (void*)new MainLoopPriv;
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame = 0;
  p->time = 0;
  p->count = 0;
  p->previous_frame_time = 0.0;
  p->delta_time = 0.0;
  p->current_time = 0.0;
  p->frame_time = 0.0;
}
EXPORT GameApi::MainLoopApi::~MainLoopApi()
{
  delete (MainLoopPriv*)priv;
}
extern Low_SDL_Window *sdl_window;
extern Low_SDL_GLContext g_context;
EXPORT void GameApi::MainLoopApi::make_current()
{
 int val = g_low->sdl->SDL_GL_MakeCurrent(sdl_window, g_context);
 std::cout << "Makecurrent return:" << val << std::endl;
}
EXPORT void GameApi::MainLoopApi::cursor_visible(bool enabled)
{
  g_low->sdl->SDL_ShowCursor(enabled);
}
extern Low_SDL_Window *sdl_window;

int g_has_wayland = false;
extern int g_display_width;
extern int g_display_height;

EXPORT void GameApi::MainLoopApi::init_window(int screen_width, int screen_height, std::string window_title, bool vr_init)
{
  set_corner(0,0,screen_width, screen_height);
  MainLoopPriv *p = (MainLoopPriv*)priv;
  int screenx = screen_width;
  int screeny = screen_height;
#ifdef SDL2_USED
#ifndef EMSCRIPTEN
  p->screen = InitSDL2(screenx,screeny,false, false, true,vr_init);
#else
  p->screen = InitSDL2(screenx,screeny,false, false, false,vr_init);
#endif
#else
  p->screen = InitSDL(screenx,screeny,false);
#endif
  OpenglLowApi *ogl = g_low->ogl;
  g_low->sdl->SDL_SetWindowTitle(sdl_window, window_title.c_str());

  if (g_has_wayland)
    {
      screenx = g_display_width;
      screeny = g_display_height;
    }
  
  p->screen_width = screenx;
  p->screen_height = screeny;
  time = g_low->sdl->SDL_GetTicks();
  ogl->glDisable(Low_GL_DEPTH_TEST);
    fpscounter_init();

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
  prog->set_var("in_iMV", Matrix::Transpose(Matrix::Inverse(m2)));
  prog->set_var("in_T", m2);
  prog->set_var("in_POS", 0.0f);
  prog->set_var("color_mix", 0.5f);
  prog->set_var("time", 0.0f);
  int s = 8;
  const char *arr[] = { 
"texsampler[0]",
"texsampler[1]",
"texsampler[2]",
"texsampler[3]",
"texsampler[4]",
"texsampler[5]",
"texsampler[6]",
"texsampler[7]",
"texsampler[8]",
"texsampler[9]",
"texsampler[10]",
"texsampler[11]",
"texsampler[12]",
"texsampler[13]",
"texsampler[14]",
"texsampler[15]",
"texsampler[16]",
"texsampler[17]",
"texsampler[18]",
"texsampler[19]",
"texsampler[20]",
"texsampler[21]",
"texsampler[22]",
"texsampler[23]",
"texsampler[24]",
"texsampler[25]",
"texsampler[26]",
"texsampler[27]",
"texsampler[28]",
"texsampler[29]",
"texsampler[30]",
"texsampler[31]",
"texsampler[32]",
"texsampler[33]",
"texsampler[34]",
"texsampler[35]",
"texsampler[36]",
"texsampler[37]",
"texsampler[38]",
"texsampler[39]",
"texsampler[40]",
"texsampler[41]",
"texsampler[42]",
"texsampler[43]",
"texsampler[44]",
"texsampler[45]",
"texsampler[46]",
"texsampler[47]",
"texsampler[48]",
"texsampler[49]",
"texsampler[50]",
"texsampler[51]",
"texsampler[52]",
"texsampler[53]",
"texsampler[54]",
"texsampler[55]",
"texsampler[56]",
"texsampler[57]",
"texsampler[58]",
"texsampler[59]",
"texsampler[60]",
"texsampler[61]",
"texsampler[62]",
"texsampler[63]",
"texsampler[64]"
  };

  const char *arr2[] = { 
"texsampler_cube[0]",
"texsampler_cube[1]",
"texsampler_cube[2]",
"texsampler_cube[3]",
"texsampler_cube[4]",
"texsampler_cube[5]",
"texsampler_cube[6]",
"texsampler_cube[7]",
"texsampler_cube[8]",
"texsampler_cube[9]",
"texsampler_cube[10]",
"texsampler_cube[11]",
"texsampler_cube[12]",
"texsampler_cube[13]",
"texsampler_cube[14]",
"texsampler_cube[15]",
"texsampler_cube[16]",
"texsampler_cube[17]",
"texsampler_cube[18]",
"texsampler_cube[19]",
"texsampler_cube[20]",
"texsampler_cube[21]",
"texsampler_cube[22]",
"texsampler_cube[23]",
"texsampler_cube[24]",
"texsampler_cube[25]",
"texsampler_cube[26]",
"texsampler_cube[27]",
"texsampler_cube[28]",
"texsampler_cube[29]",
"texsampler_cube[30]",
"texsampler_cube[31]",
"texsampler_cube[32]",
"texsampler_cube[33]",
"texsampler_cube[34]",
"texsampler_cube[35]",
"texsampler_cube[36]",
"texsampler_cube[37]",
"texsampler_cube[38]",
"texsampler_cube[39]",
"texsampler_cube[40]",
"texsampler_cube[41]",
"texsampler_cube[42]",
"texsampler_cube[43]",
"texsampler_cube[44]",
"texsampler_cube[45]",
"texsampler_cube[46]",
"texsampler_cube[47]",
"texsampler_cube[48]",
"texsampler_cube[49]",
"texsampler_cube[50]",
"texsampler_cube[51]",
"texsampler_cube[52]",
"texsampler_cube[53]",
"texsampler_cube[54]",
"texsampler_cube[55]",
"texsampler_cube[56]",
"texsampler_cube[57]",
"texsampler_cube[58]",
"texsampler_cube[59]",
"texsampler_cube[60]",
"texsampler_cube[61]",
"texsampler_cube[62]",
"texsampler_cube[63]",
"texsampler_cube[64]"
  };

  for(int i=0;i<s;i++) {
    //std::stringstream ss2; ss2 << i;
    //std::string ss = "texsampler[" + ss2.str() + "]";
    prog->set_var(arr[i] /*ss.c_str()*/, i);
    //std::string ss3 = "texsampler_cube[" + ss2.str() + "]";
    prog->set_var(arr2[i] /*ss3.c_str()*/,i);
  }
  std::string ss = "cubesampler";
  prog->set_var(ss.c_str(), 0);

  alpha_1(false);


}
EXPORT void GameApi::MainLoopApi::transfer_sdl_surface(MainLoopApi &orig)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  MainLoopPriv *p2 = (MainLoopPriv*)orig.priv;
  p->screen = p2->screen;
}
EXPORT void GameApi::MainLoopApi::delay(int ms)
{
  g_low->sdl->SDL_Delay(ms);
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
#ifndef __APPLE__
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
#endif
}
EXPORT void GameApi::MainLoopApi::print_profile()
{
#ifndef __APPLE__
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
#endif
}


int frame_counter = 0;

EXPORT void GameApi::MainLoopApi::fpscounter_init()
{
}

EXPORT void GameApi::MainLoopApi::fpscounter_framestart()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  unsigned long long time = g_low->sdl->SDL_GetPerformanceCounter();
  p->time = time;
}

unsigned long long g_fps_time;
EXPORT void GameApi::MainLoopApi::fpscounter_frameready()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  unsigned long long time = g_low->sdl->SDL_GetPerformanceCounter();
  p->time2 = time;
  g_fps_time = p->time;
}

unsigned long long s_time;

EXPORT void GameApi::MainLoopApi::fpscounter_swapbuffersready()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  unsigned long long time = g_low->sdl->SDL_GetPerformanceCounter();
  s_time = time;
  
}


float collect_time=0.0;
float collect_delta = 0.0;
int collect_frame = 0;

EXPORT float GameApi::MainLoopApi::fpscounter(bool print)
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  //unsigned long long time = g_low->sdl->SDL_GetPerformanceCounter();
  // time = time*1000/g_low->sdl->SDL_GetPerformanceFrequency();
  // unsigned int time = g_low->sdl->SDL_GetTicks();
  //double delta_time = (((long double)(p->time2 - p->time))*1000.0f)/(double)g_low->sdl->SDL_GetPerformanceFrequency(); // in ms

  long long delta_time_ticks = p->time2 - g_fps_time;
  long long delta_time_ticks2 = s_time - g_fps_time;
  if (delta_time_ticks<0) delta_time_ticks=-delta_time_ticks;
  if (delta_time_ticks2<0) delta_time_ticks2=-delta_time_ticks2;
  //std::cout << "delta_ticks: " << delta_time_ticks << std::endl;
  //if (delta_time_ticks<0) delta_time_ticks=1;
  
  float delta_time = delta_time_ticks*1000.0/g_low->sdl->SDL_GetPerformanceFrequency();

  float delta_time2 = delta_time_ticks2*1000.0/g_low->sdl->SDL_GetPerformanceFrequency();

  
  
  //unsigned int f_time = time - p->frame_time;
  //if (p->count==0) { p->avg_time = 0; p->time = time; p->frame=0; }
  //p->avg_time+=f_time;
  // std::cout << "Freq: " << g_low->sdl->SDL_GetPerformanceFrequency() << std::endl;
  //std::cout << "Time2: " << p->time2 << " " << p->time << std::endl;
  //std::cout << "Delta_time: " << delta_time << std::endl;
  //p->frame++;
  //p->count++;
  //if (delta_time<1) delta_time=1;
  collect_delta+=delta_time;
  collect_frame++;
  
  float fps = 1000.0f*collect_frame/collect_delta; // p->frame/(delta_time/1000.0f);



  static float s_fps = 0.0; 
  collect_time+=delta_time2;
  if (collect_time>100.0) {
    collect_time=0.0;
    collect_delta = 0.0;
    collect_frame=0;
    s_fps = fps;
  } else {
    return s_fps;
  }

  //std::cout << "fps: " << fps << std::endl;

  //if (p->count<0) { p->count = 0; }
  //if (p->count>100) {
  //  if (print) {
  //  std::cout << "FPS: " << fps << " delta_time:" << p->avg_time/100.0 << std::endl;
  //  }
  //  p->count = 0;
  //}


  return fps;
}

EXPORT void GameApi::MainLoopApi::start_editor_state()
{
  OpenglLowApi *ogl = g_low->ogl;
  alpha_1(true);
  ogl->glDisable(Low_GL_DEPTH_TEST);
}
EXPORT void GameApi::MainLoopApi::end_editor_state()
{
  OpenglLowApi *ogl = g_low->ogl;
  alpha_1(true);
  //ogl->glEnable(Low_GL_DEPTH_TEST);
}

extern GameApi::M g_view_rot;

EXPORT void GameApi::MainLoopApi::init_3d(SH sh, int screen_width, int screen_height)
{
  OpenglLowApi *ogl = g_low->ogl;
  
  int screenx = screen_width;
  int screeny = screen_height;

  Program *prog = find_shader_program(e, sh);
  prog->use(); // 80.0, 10.1, 60000.0
  Matrix m = Matrix::Perspective(90.0*double(screeny)/double(screenx), (double)screenx/screeny, 10.1, 1600.0); // 10.1 60000.0
  //Matrix m = Matrix::Perspective2(-300.0, 300.0, -300.0, 300.0, 1.0, 610.0);
  prog->set_var("in_P", m);
  Matrix m2 = Matrix::Identity();
  prog->set_var("in_MV", m2);
  prog->set_var("in_iMV", Matrix::Transpose(Matrix::Inverse(m2)));
  prog->set_var("in_View", Matrix::Identity());
  prog->set_var("in_View2", Matrix::Identity());
  g_view_rot=add_matrix2(e,Matrix::Identity());
  Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
  prog->set_var("in_T", m3);
  prog->set_var("in_POS", 0.0f);
  prog->set_var("color_mix", 0.5f); 
  prog->set_var("time",0.0f);
  int s = 64;
  const char *arr[] = { 
"texsampler[0]",
"texsampler[1]",
"texsampler[2]",
"texsampler[3]",
"texsampler[4]",
"texsampler[5]",
"texsampler[6]",
"texsampler[7]",
"texsampler[8]",
"texsampler[9]",
"texsampler[10]",
"texsampler[11]",
"texsampler[12]",
"texsampler[13]",
"texsampler[14]",
"texsampler[15]",
"texsampler[16]",
"texsampler[17]",
"texsampler[18]",
"texsampler[19]",
"texsampler[20]",
"texsampler[21]",
"texsampler[22]",
"texsampler[23]",
"texsampler[24]",
"texsampler[25]",
"texsampler[26]",
"texsampler[27]",
"texsampler[28]",
"texsampler[29]",
"texsampler[30]",
"texsampler[31]",
"texsampler[32]",
"texsampler[33]",
"texsampler[34]",
"texsampler[35]",
"texsampler[36]",
"texsampler[37]",
"texsampler[38]",
"texsampler[39]",
"texsampler[40]",
"texsampler[41]",
"texsampler[42]",
"texsampler[43]",
"texsampler[44]",
"texsampler[45]",
"texsampler[46]",
"texsampler[47]",
"texsampler[48]",
"texsampler[49]",
"texsampler[50]",
"texsampler[51]",
"texsampler[52]",
"texsampler[53]",
"texsampler[54]",
"texsampler[55]",
"texsampler[56]",
"texsampler[57]",
"texsampler[58]",
"texsampler[59]",
"texsampler[60]",
"texsampler[61]",
"texsampler[62]",
"texsampler[63]",
"texsampler[64]"
  };

  const char *arr2[] = { 
"texsampler_cube[0]",
"texsampler_cube[1]",
"texsampler_cube[2]",
"texsampler_cube[3]",
"texsampler_cube[4]",
"texsampler_cube[5]",
"texsampler_cube[6]",
"texsampler_cube[7]",
"texsampler_cube[8]",
"texsampler_cube[9]",
"texsampler_cube[10]",
"texsampler_cube[11]",
"texsampler_cube[12]",
"texsampler_cube[13]",
"texsampler_cube[14]",
"texsampler_cube[15]",
"texsampler_cube[16]",
"texsampler_cube[17]",
"texsampler_cube[18]",
"texsampler_cube[19]",
"texsampler_cube[20]",
"texsampler_cube[21]",
"texsampler_cube[22]",
"texsampler_cube[23]",
"texsampler_cube[24]",
"texsampler_cube[25]",
"texsampler_cube[26]",
"texsampler_cube[27]",
"texsampler_cube[28]",
"texsampler_cube[29]",
"texsampler_cube[30]",
"texsampler_cube[31]",
"texsampler_cube[32]",
"texsampler_cube[33]",
"texsampler_cube[34]",
"texsampler_cube[35]",
"texsampler_cube[36]",
"texsampler_cube[37]",
"texsampler_cube[38]",
"texsampler_cube[39]",
"texsampler_cube[40]",
"texsampler_cube[41]",
"texsampler_cube[42]",
"texsampler_cube[43]",
"texsampler_cube[44]",
"texsampler_cube[45]",
"texsampler_cube[46]",
"texsampler_cube[47]",
"texsampler_cube[48]",
"texsampler_cube[49]",
"texsampler_cube[50]",
"texsampler_cube[51]",
"texsampler_cube[52]",
"texsampler_cube[53]",
"texsampler_cube[54]",
"texsampler_cube[55]",
"texsampler_cube[56]",
"texsampler_cube[57]",
"texsampler_cube[58]",
"texsampler_cube[59]",
"texsampler_cube[60]",
"texsampler_cube[61]",
"texsampler_cube[62]",
"texsampler_cube[63]",
"texsampler_cube[64]"
  };

  for(int i=0;i<s;i++) {
    //std::stringstream ss2; ss2 << i;
    //std::string ss = "texsampler[" + ss2.str() + "]";
    prog->set_var(arr[i] /*ss.c_str()*/, i);
    prog->set_var(arr2[i], i);
  }
  std::string ss = "cubesampler";
  prog->set_var(ss.c_str(),0);
  alpha_1(false);
  ogl->glEnable(Low_GL_DEPTH_TEST);

  //ogl->glAlphaFunc(Low_GL_LESS, 0.5);
  //ogl->glEnable(Low_GL_ALPHA_TEST);
  //glEnable(GL_MULTISAMPLE );
}
EXPORT void GameApi::MainLoopApi::nvidia_init()
{
#if 0
  OpenglLowApi *ogl = g_low->ogl;
  if (GLEW_NV_path_rendering)
    {
  ogl->glMatrixLoadIdentityEXT(Low_GL_PROJECTION);
  ogl->glMatrixLoadIdentityEXT(Low_GL_MODELVIEW);
  ogl->glMatrixOrthoEXT(Low_GL_PROJECTION, 0, 800, 600, 0, -1, 1);
    }
#endif
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
  OpenglLowApi *ogl = g_low->ogl;
  //int screenx = 800;
  //int screeny = 600;
  if (b)
    {
      Program *prog = find_shader_program(e, sh);
      //g_low->ogl->glDisable(Low_GL_LIGHTING);
      ogl->glEnable(Low_GL_DEPTH_TEST);
      // 10.1, 60000.0
      Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1,
				     1600.0);
      Matrix m3 = Matrix::Translate(0.0,0.0,-500.0);
      prog->use();
      prog->set_var("in_P", m);
      prog->set_var("in_T", m3);
      //glMultMatrixf(&mat[0]);
#ifndef EMSCRIPTEN      
      ogl->glMatrixMode( Low_GL_MODELVIEW ); 
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
      prog->use();
      prog->set_var("in_P", m);
      //g_low->ogl->glDisable(Low_GL_LIGHTING);
      ogl->glDisable(Low_GL_DEPTH_TEST);
      //glMatrixMode( GL_PROJECTION ); 
      //glLoadIdentity(); 
      //glOrtho(0, screenx, screeny,0,0,1);
      //glMatrixMode( GL_MODELVIEW ); 
      //glLoadIdentity();
      //glTranslatef(0.375, 0.375, 0.0);
      Matrix m2 = Matrix::Translate(0.375, 0.375, 0.0);
      prog->set_var("in_MV", m2);
      prog->set_var("in_iMV", Matrix::Transpose(Matrix::Inverse(m2)));
      Matrix m3 = Matrix::Identity();
      prog->set_var("in_T", m3);
    }
}
extern std::string gameapi_seamless_url;
EXPORT void GameApi::MainLoopApi::clear(unsigned int col)
{
  OpenglLowApi *ogl = g_low->ogl;
  //glClearColor(255,255,255,255);
  ogl->glClearStencil(0);
  Color c(col);
  ogl->glClearColor(c.rf(),c.gf(),c.bf(),c.af());
  ogl->glStencilMask(~0);
  ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
  //glLoadIdentity();
  //glTranslatef(0.375, 0.375, 0.0);
  //glTranslatef(0.0, 0.0, -260.0);
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}
EXPORT void GameApi::MainLoopApi::clear_3d_transparent()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
}
bool g_no_clear = false;
EXPORT void GameApi::MainLoopApi::clear_3d(unsigned int color)
{
  //glClearColor(255,255,255,255);
  OpenglLowApi *ogl = g_low->ogl;
  
  ogl->glClearStencil(0);

  int r = color & 0x00ff0000;
  int g = color & 0x0000ff00;
  int b = color & 0x000000ff;
  int a = color & 0xff000000;
  a>>=24;
  r>>=16;
  g>>=8;
  ogl->glClearColor(r/256.0,g/256.0,b/256.0,a/256.0);
  ogl->glStencilMask(~0);
  if (g_no_clear) {
    ogl->glClear( /*Low_GL_COLOR_BUFFER_BIT |*/ Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
  } else {
    ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
  }
#ifndef EMSCRIPTEN
  ogl->glLoadIdentity();
  ogl->glTranslatef(0.375, 0.375, 0.0);
  ogl->glTranslatef(0.0, 0.0, -260.0);
#endif
  //float speed = 1.0;
  //glRotatef(speed*time, 0.0,1.0,0.0);
  //glTranslatef(0.0, -100.0, 0.0);

}

EXPORT void GameApi::MainLoopApi::transparency(bool enabled)
{
  OpenglLowApi *ogl = g_low->ogl;

  if (enabled)
    {
      //glBlendFunc(GL_ZERO, GL_SRC_COLOR); 
     ogl->glDepthMask(Low_GL_FALSE);
    }
  else
    {
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     ogl->glDepthMask(Low_GL_TRUE);
    }
}
EXPORT void GameApi::MainLoopApi::depth_test(bool enabled)
{
  OpenglLowApi *ogl = g_low->ogl;
  if (enabled)
    {
      ogl->glEnable(Low_GL_DEPTH_TEST);
    }
  else
    {
      ogl->glDisable(Low_GL_DEPTH_TEST);
    }
}
EXPORT void GameApi::MainLoopApi::alpha(bool enable)
{
  alpha_1(enable);
}
void GameApi::MainLoopApi::alpha_1(bool enable)
{
  OpenglLowApi *ogl = g_low->ogl;
  if (enable)
    {
      ogl->glEnable(Low_GL_BLEND);
      //glBlendFunc(GL_SRC_COLOR /*ONE_MINUS_SRC_COLOR*/, GL_DST_COLOR);
      ogl->glBlendFunc(Low_GL_SRC_ALPHA, Low_GL_ONE_MINUS_SRC_ALPHA);
#ifndef EMSCRIPTEN
      ogl->glTexEnvi(Low_GL_TEXTURE_ENV, Low_GL_TEXTURE_ENV_MODE, Low_GL_REPLACE);
#endif
      //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE); 
    }
  else
    {
      ogl->glDisable(Low_GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
#ifndef EMSCRIPTEN
      ogl->glTexEnvi(Low_GL_TEXTURE_ENV, Low_GL_TEXTURE_ENV_MODE, Low_GL_REPLACE);
#endif
    }
}
EXPORT void GameApi::MainLoopApi::antialias(bool enable)
{
  if (enable)
    {
      g_low->sdl->SDL_GL_SetAttribute( Low_SDL_GL_MULTISAMPLEBUFFERS, 1);
    }
  else
    {
      g_low->sdl->SDL_GL_SetAttribute( Low_SDL_GL_MULTISAMPLEBUFFERS, 0);
    }
}

EXPORT float GameApi::MainLoopApi::get_time()
{
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  return pp->frame_time-time;
  //return g_low->sdl->SDL_GetTicks()-time;
}
EXPORT float GameApi::MainLoopApi::get_delta_time()
{
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  return pp->delta_time;
}
EXPORT void GameApi::MainLoopApi::reset_time()
{
  time = g_low->sdl->SDL_GetTicks();
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
  OpenglLowApi *ogl = g_low->ogl;
  MainLoopPriv *p = (MainLoopPriv*)priv;
  //SDL_Surface *surf = p->screen;
  int w = p->screen_width; //surf->w;
  int h = p->screen_height; //surf->h;

  BufferRef ref = BufferRef::NewBuffer(w,h);

  ogl->glReadPixels(0,0,w,h, Low_GL_RGBA /*GL_BGRA*/, Low_GL_UNSIGNED_BYTE, ref.buffer);

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

extern Low_SDL_Window *sdl_window;

EXPORT void GameApi::MainLoopApi::finish()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glFinish();
}
EXPORT void GameApi::MainLoopApi::check_glerrors(std::string context)
{

  OpenglLowApi *ogl = g_low->ogl;
  int e = -1;
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  int i = 0;
  while(i<100&& ((e=ogl->glGetError()) != 0)) {
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
	pp->last_error = e;
	std::cout << context << ":"<< std::hex << e << ":" << error << std::endl;
      }
  }
  
}
EXPORT void GameApi::MainLoopApi::swapbuffers()
{
#if 0
  OpenglLowApi *ogl = g_low->ogl;
  int e = -1;
  MainLoopPriv *pp = (MainLoopPriv*)priv;
  int i = 0;
  while((e=ogl->glGetError()) != 0) {
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
      ogl->glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &readint );

      int readgpuint = -1;
      ogl->glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &readgpuint);
      std::cout << "MEMORY AVAILABLE: " << readint * 1024 << std::endl;
      std::cout << "GPU MEMORY AVAILABLE: " << readgpuint * 1024 << std::endl;
    }
#endif


  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //glLoadIdentity();
#ifdef SDL2_USED
  g_low->sdl->SDL_GL_SwapWindow(sdl_window);
#else
  SDL_GL_SwapBuffers();
#endif
  //#ifdef EMSCRIPTEN
  // emscripten_webgl_commit_frame();
  //#endif
  fpscounter_swapbuffersready();
  
#if 1
  unsigned int time = g_low->sdl->SDL_GetTicks();
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame_time = time;
  unsigned int delta = time - p->previous_frame_time;
  //std::cout << "delta:" << delta << std::endl;
  p->delta_time = float(delta)/100.0;
  p->current_time = float(time)/100.0;

  p->previous_frame_time = time;
#endif
#if 0
  unsigned long long time = g_low->sdl->SDL_GetPerformanceCounter();
  MainLoopPriv *p = (MainLoopPriv*)priv;
  p->frame_time = time*1000/g_low->sdl->SDL_GetPerformanceFrequency();
  unsigned int delta = time - p->previous_frame_time;
  p->delta_time = float(delta*1000/g_low->sdl->SDL_GetPerformanceFrequency())/100.0*5.0;
  p->current_time = float(time*1000/g_low->sdl->SDL_GetPerformanceFrequency())/100.0*5.0;
  p->previous_frame_time = time;
                    
#endif
  
  
  
  //SDL_Flip(surf);
  frame++;
}
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i);
GameApi::SP GameApi::MainLoopApi::screenspace()
{
  MainLoopPriv *p = (MainLoopPriv*)priv;
  Low_SDL_Surface *surf = p->screen;
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
  OpenglLowApi *ogl = g_low->ogl;
#ifndef EMSCRIPTEN
  ogl->glClearStencil(0);
  ogl->glClear(Low_GL_STENCIL_BUFFER_BIT);
  ogl->glEnable(Low_GL_STENCIL_TEST);
  ogl->glStencilFunc(Low_GL_ALWAYS, 1, -1);
  ogl->glStencilOp(Low_GL_KEEP, Low_GL_KEEP, Low_GL_REPLACE);
  ogl->glColor4f(1.0,1.0,1.0,1.0);
#endif
}
EXPORT void GameApi::MainLoopApi::outline_second()
{
  OpenglLowApi *ogl = g_low->ogl;
#ifndef EMSCRIPTEN
  ogl->glStencilFunc(Low_GL_NOTEQUAL, 1, -1);
  ogl->glStencilOp(Low_GL_KEEP, Low_GL_KEEP, Low_GL_REPLACE);
  ogl->glLineWidth(3);
  ogl->glEnable(Low_GL_LINE_SMOOTH);
  ogl->glPolygonMode(Low_GL_FRONT_AND_BACK, Low_GL_LINE);
#endif
}
EXPORT void GameApi::MainLoopApi::outline_third()
{
  OpenglLowApi *ogl = g_low->ogl;
#ifndef EMSCRIPTEN
  ogl->glPolygonMode(Low_GL_FRONT_AND_BACK, Low_GL_FILL);
  ogl->glDisable(Low_GL_STENCIL_TEST);
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
int g_event_screen_x = -1;
int g_event_screen_y = -1;
extern int g_resize_event_sx;
extern int g_resize_event_sy;

extern Low_SDL_Window *sdl_window;

#if 0
bool g_event_first = true;



bool g_event_override=false;
float g_delta_y = 0;
bool g_event_handled=true;

float current_yy=0.0;
double current_y=0.0;
float g_event_time=0.0;
float g_event_time2=0.0;

int positive_count=0;
int negative_count=0;

bool g_give_stored=false;
#ifdef EMSCRIPTEN
EM_BOOL em_cb(int eventType, const EmscriptenWheelEvent *wheelEvent, void *data)
{
  double deltaX = wheelEvent->deltaX;
  double deltaY = wheelEvent->deltaY;
  double deltaZ = wheelEvent->deltaZ;
  //std::cout << "EM_CB: eventtype=" << eventType << std::endl;
  //std::cout << "EM_CB: deltaX=" << deltaX << std::endl;
  //std::cout << "EM_CB: deltaY=" << deltaY << std::endl;
  //std::cout << "EM_CB: deltaZ=" << deltaZ << std::endl;
  
  int dir = 0;
  if (deltaY > 110.0) dir=1;
  if (deltaY < -110.0) dir=-1;
  if (deltaY > 220.0) dir=2;
  if (deltaY < -220.0) dir=-2;
  if (deltaY > -110.0 && deltaY < 110.0)
    {
      //std::cout << g_event_time << " -- " << g_event_time2 << std::endl;
      //float delta = deltaY-current_yy;
      //current_yy=deltaY;
      current_y += deltaY;
      g_delta_y = current_y;
      g_event_override=true;
      g_event_handled=false;
      return 1;
    }
  if (dir!=0) {
    current_y += dir*110.0;
    g_delta_y = current_y;
    g_event_override=true;
    g_event_handled=false;
  }
  return 1;
}
#endif

bool g_disable_storing=false;

#endif

EXPORT GameApi::MainLoopApi::Event GameApi::MainLoopApi::get_event()
{
#if 0
  static GameApi::MainLoopApi::Event stored;
  if (g_give_stored)
    {
      GameApi::MainLoopApi::Event stored2= stored;
      g_give_stored=false;
      g_disable_storing=true;
      stored=get_event();
      g_disable_storing=false;
      return stored2;
    }
#endif

#if 0
  if (g_event_first)
    {
#ifdef EMSCRIPTEN
      //std::cout << "EM_CB: installing callback" << std::endl;
      emscripten_set_wheel_callback("canvas", 0, false, &em_cb);
#endif
      g_event_first=false;
    }

#endif
  
  OpenglLowApi *ogl = g_low->ogl;
  Low_SDL_Event event;
  Event e2;
  int last = g_low->sdl->SDL_PollEvent(&event);
  e2.last = last!=0;
  //float t = get_time();
  //g_event_time = t;
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //e2.current_time = p->current_time;
  //e2.delta_time = p->delta_time;
  int x,y;
  int mouse_wheel_y = 0;
  int val = g_low->sdl->SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  if (event.type==Low_SDL_KEYDOWN||event.type==Low_SDL_KEYUP)
    e2.ch = event.key.keysym.sym;
  else
    e2.ch=-1;
  if (event.type==256) { exit(0); }
  std::string filename = "";
  if (event.type==Low_SDL_DROPFILE)
    {
      //std::cout << "get_event -- dragdrop event" << std::endl;

      Low_SDL_DropEvent *ptr = &event.drop;
      if (ptr && ptr->file)
	filename = ptr->file;
    }
  e2.drag_drop_filename = filename;

#ifndef __APPLE__
#ifndef EMSCRIPTEN
  if (event.type==Low_SDL_FINGERMOTION||event.type==Low_SDL_FINGERDOWN||event.type==Low_SDL_FINGERUP)
    {
      Low_SDL_TouchFingerEvent *ptr = &event.tfinger;
      float xx = ptr->x / get_screen_width();
      float yy = ptr->y / get_screen_height();
      if (!std::isnormal(xx)) xx=0.0f;
      if (!std::isnormal(yy)) yy=0.0f;
      x = /*get_screen_width()-*/int(xx);
      y = /*get_screen_height()-*/int(yy); 
    }
#endif
#endif


#ifndef EMSCRIPTEN
  if (event.type==Low_SDL_WINDOWEVENT) {
    if (event.window.event == 5) {
      //std::cout << "WINDOWEVENT 5: RESIZE" << std::endl;
      g_event_screen_x = event.window.data1;
      g_event_screen_y = event.window.data2;
      if (g_event_screen_x<320) g_event_screen_x = 320;
      if (g_event_screen_y<200) g_event_screen_y = 200;
#ifndef LINUX
           g_low->sdl->SDL_SetWindowSize(sdl_window,g_event_screen_x, g_event_screen_y);
#endif
           ogl->glViewport(0,0,g_event_screen_x, g_event_screen_y);
    }
  }
#endif

  // This will be activated from gameapi.cc
  if (g_resize_event_sx != -1 && g_resize_event_sy != -1) {
    //std::cout << "WINDOWEVENT 5: RESIZE" << std::endl;
      g_event_screen_x = g_resize_event_sx;
      g_event_screen_y = g_resize_event_sy;
      if (g_event_screen_x<320) g_event_screen_x = 320;
      if (g_event_screen_y<200) g_event_screen_y = 200;
#ifndef LINUX
           g_low->sdl->SDL_SetWindowSize(sdl_window,g_event_screen_x, g_event_screen_y);
#endif
           ogl->glViewport(0,0,g_event_screen_x, g_event_screen_y);
	   g_resize_event_sx = -1;
	   g_resize_event_sy = -1;
  }
  //if (event.type == Low_SDL_WINDOWEVENT
  //    && event.window.event == Low_SDL_WINDOWEVENT_EXPOSED)
  //{
  //  swapbuffers();
  //}
  
  if (event.type==Low_SDL_MOUSEWHEEL)
    {
      Low_SDL_MouseWheelEvent *ptr = &event.wheel;
      mouse_wheel_y = ptr->y;
      //std::cout << "MouseWheel:" << mouse_wheel_y << std::endl;
    }

  bool handled=true;
  

  if (event.type==Low_SDL_JOYAXISMOTION) {
    //if ((event.jaxis.value < -3200) || (event.jaxis.value>3200)) {
      if (event.jaxis.axis == 0) e2.joy0_axis0 = event.jaxis.value;
      if (event.jaxis.axis == 1) e2.joy0_axis1 = event.jaxis.value;
      if (event.jaxis.axis == 2) e2.joy0_axis2 = event.jaxis.value;
      if (event.jaxis.axis == 3) e2.joy0_axis3 = event.jaxis.value;
      if (event.jaxis.axis == 4) e2.joy0_axis4 = event.jaxis.value;
      e2.joy0_current_axis = event.jaxis.axis;
      //} else {
      //  event.type=-1;
      // }
  }
  if (event.type==Low_SDL_JOYBALLMOTION) {
    if (event.jball.ball==0) {
      e2.joy0_ball0 += event.jball.xrel;
      e2.joy0_ball1 += event.jball.yrel;
    }
    if (event.jball.ball==1) {
      e2.joy1_ball0 += event.jball.xrel;
      e2.joy1_ball1 += event.jball.yrel;
    }
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
  float scale_x = 1.0;
  float scale_y = 1.0;
  if (g_event_screen_y != -1) {
    scale_x = float(get_screen_width())/float(g_event_screen_x);
    scale_y = float(get_screen_height())/float(g_event_screen_y);
    
    //delta = get_screen_height()-g_event_screen_y;
  }

  env->pt[id].x = x*scale_x;
  env->pt[id].y = y*scale_y;
  e2.cursor_pos.id = id;
  e2.button = -1;

#ifdef __APPLE__
  SDL_Keymod mod = g_low->sdl->SDL_GetModState();
  if ((val & Low_SDL_BUTTON(1))&&(mod & Low_KMOD_CTRL)) { e2.button = 2; } else
#endif  	  
    //if (val & Low_SDL_BUTTON(1)) { e2.button = 0; }
    //if (val & Low_SDL_BUTTON(2)) { e2.button = 1; }
    //if (val & Low_SDL_BUTTON(3)) { e2.button = 2; }

    static bool button0=false,button1=false,button2=false;
    
    if (event.type==Low_SDL_MOUSEBUTTONDOWN)
      {
	if (event.button.button==1) button0=true;
	if (event.button.button==2) button1=true;
	if (event.button.button==3) button2=true;
      }
  if (event.type==Low_SDL_MOUSEBUTTONUP)
    {
	if (event.button.button==1) button0=false;
	if (event.button.button==2) button1=false;
	if (event.button.button==3) button2=false;
    }
  if (button0) e2.button=0;
  if (button1) e2.button=1;
  if (button2) e2.button=2;
  
  

  static Low_SDL_Joystick *joy1 = g_low->sdl->SDL_JoystickOpen(0);
  //std::cout << "Joystick0:" << joy1 << std::endl;
  unsigned int but1 = g_low->sdl->SDL_JoystickGetButton(joy1, 0);  
  unsigned int but2 = g_low->sdl->SDL_JoystickGetButton(joy1, 1);
  unsigned int but3 = g_low->sdl->SDL_JoystickGetButton(joy1, 2);
  unsigned int but4 = g_low->sdl->SDL_JoystickGetButton(joy1, 3);
  //std::cout << e.type << " " << e.ch << std::endl;
  //std::cout << "Joystick buttons:" << but1 << " " << but2 << " " << but3 << " " << but4 << std::endl;
  e2.joy0_button0 = but1==1;
  e2.joy0_button1 = but2==1;
  e2.joy0_button2 = but3==1;
  e2.joy0_button3 = but4==1;
  e2.mouse_wheel_y = mouse_wheel_y;

  //std::cout << "MOUSE RESULT: " << mouse_wheel_y << std::endl;
  
  static Low_SDL_Joystick *joy2 = g_low->sdl->SDL_JoystickOpen(1);
  //std::cout << "Joystick1:" << joy2 << std::endl;

  static bool firsttime = true;
  if (firsttime) {
    firsttime = false;
    g_low->sdl->SDL_JoystickEventState(Low_SDL_ENABLE);
  }

  
  unsigned int a_but1 = g_low->sdl->SDL_JoystickGetButton(joy2, 0);  
  unsigned int a_but2 = g_low->sdl->SDL_JoystickGetButton(joy2, 1);
  unsigned int a_but3 = g_low->sdl->SDL_JoystickGetButton(joy2, 2);
  unsigned int a_but4 = g_low->sdl->SDL_JoystickGetButton(joy2, 3);
  //std::cout << a_but1 << " " << a_but2 << " " << a_but3 << " " << a_but4 << std::endl;
  e2.joy1_button0 = a_but1==1;
  e2.joy1_button1 = a_but2==1;
  e2.joy1_button2 = a_but3==1;
  e2.joy1_button3 = a_but4==1;



#if 0
  if (g_event_override && !g_disable_storing) {
    static float dy = 0.0;
    static float old_dy = 0.0;
    dy-=g_delta_y;
    float delta = dy-old_dy;
    //mouse_wheel_y = dy;
    g_delta_y=0;
    current_y=0.0;
    //std::cout << "Delta: " << delta << std::endl;

    static int g_mouse_wheel = 0;
    
    if (e2.type==Low_SDL_MOUSEWHEEL) { e2.mouse_wheel_y = g_mouse_wheel; }
    if (delta>120.0) { stored=e2; e2.last=false; e2.type =Low_SDL_MOUSEWHEEL; e2.ch=1; g_mouse_wheel++; old_dy -= 120.0; g_give_stored=true;}
    if (delta<-120.0) { stored=e2; e2.last=false; e2.type =Low_SDL_MOUSEWHEEL; e2.ch=-1; g_mouse_wheel--; old_dy += 120.0; g_give_stored=true; }
    if (e2.type==Low_SDL_MOUSEWHEEL) { e2.mouse_wheel_y = g_mouse_wheel; }
      g_event_handled=true;
  }
#endif
  
  //std::cout << "EVENT:" << e2.type << " " << e2.ch << " " << e2.mouse_wheel_y << std::endl;
  
  return e2;
}
class SeqML : public MainLoopItem
{
public:
  SeqML(std::vector<MainLoopItem*> vec, float time) : vec(vec), time(time) { num2 = -1; firsttime = true; }
  void Collect(CollectVisitor &vis) {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
  OpenglLowApi *ogl = g_low->ogl;
    if (firsttime)
      {
	int s = vec.size();
	for(int i=s-1;i>=0;i--)
	  {
	    vec[i]->execute(e);
	  }
	ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
	
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
  virtual std::vector<int> shader_id() {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      return vec[num2]->shader_id();
    } else { 
      return std::vector<int>(); 
    }
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
  void Collect(CollectVisitor &vis) {
    vec[1]->Collect(vis); vec[0]->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { vec[1]->Prepare(); vec[0]->Prepare(); }
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
  virtual std::vector<int> shader_id() {
    int s = vec.size();
    if (num2>=0 && num2<s) {
      return vec[num2]->shader_id();
    } else { 
      return std::vector<int>(); 
    }
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
  void Collect(CollectVisitor &vis) { curr->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { curr->Prepare(); }
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
  void Collect(CollectVisitor &vis) { curr->Collect(vis); end->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { curr->Prepare(); end->Prepare(); }
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
  void Collect(CollectVisitor &vis) { end->Collect(vis); curr->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { end->Prepare(); curr->Prepare(); }
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
  std::vector<int> shader_id() {
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
  void Collect(CollectVisitor &vis) { normal_game_screen->Collect(vis); gameover_screen->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() { normal_game_screen->Prepare(); gameover_screen->Prepare(); }
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
  virtual std::vector<int> shader_id() {
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
class JoyPrinter : public MainLoopItem
{
public:
  JoyPrinter(MainLoopItem *item) : item(item) { }
  void Collect(CollectVisitor &vis) { item->Collect(vis); }
  void HeavyPrepare() { }

  virtual void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    std::cout << "Joy0: (" << (e.joy0_button0?"true":"false") << "," << (e.joy0_button1?"true":"false") << "," << (e.joy0_button2?"true":"false") << "," << (e.joy0_button3?"true":"false") << "): " << e.joy0_axis0 << " " << e.joy0_axis1 << " " << e.joy0_axis2 << " " << e.joy0_axis3<< " " << e.joy0_axis4 << "::" << e.joy0_ball0 << " " << e.joy0_ball1 << std::endl;
    std::cout << "Joy1: (" << e.joy1_button0 << "," << e.joy1_button1 << "," << e.joy1_button2 << "," << e.joy1_button3 << "): " << e.joy1_axis0 << " " << e.joy1_axis1 << "::" << e.joy1_ball0 << " " << e.joy1_ball1 << std::endl;

  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  MainLoopItem *item;
};

GameApi::ML GameApi::MainLoopApi::joystick_printer(ML ml)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new JoyPrinter(item));
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
extern int g_event_screen_x;
extern int g_event_screen_y;

GameApi::M g_view_rot;
void GameApi::MainLoopApi::execute_ml(GameApi::EveryApi &ev, ML ml, SH color, SH texture, SH texture_2d, SH array_texture, M in_MV, M in_T, M in_N, int screen_size_x, int screen_size_y)
{
  int screenx = screen_size_x;
  int screeny = screen_size_y;

  //float scale_x = 1.0;
  //float scale_y = 1.0;
  //	if (g_event_screen_y!=-1) {
  //	  scale_x = float(g_event_screen_x)/float(screenx);
  //	  scale_y = float(g_event_screen_y)/float(screeny);
  //	}
  ////	screenx=screenx*scale_x;
  //	screeny=screeny*scale_y;
  if (ml.id<0) { std::cout << "execute_ml rejected " << ml.id << std::endl; return; }
  MainLoopItem *item = find_main_loop(e, ml);
  if (!item) { std::cout << "execute_ml rejected item=NULL with ml.id=" << ml.id << std::endl; /*check_main_loop_array(e);*/ return; }
  if (color.id<0||texture_2d.id<0||texture.id<0) { std::cout << "execute_ml shaders rejected" << std::endl; return; }
  MainLoopEnv ek;
  ek.sh_color = color.id;
  ek.sh_texture_2d = texture_2d.id;
  ek.sh_texture = texture.id;
  ek.sh_array_texture = texture.id; //array_texture.id;
  //ek.type = ee.type;
  //ek.ch = ee.ch;
  //ek.cursor_pos = *find_point(e,ee.cursor_pos);
  //ek.button = ee.button;
  ek.env = find_matrix(e,in_MV);
  ek.in_MV = find_matrix(e, in_MV);
  ek.in_T = find_matrix(e, in_T);
  ek.in_N = find_matrix(e, in_N);
  ek.in_P = Matrix::Perspective(90.0*double(screeny)/double(screenx), (double)screenx/screeny, 10.1, 1600.0); // 10.1, 60000.0
  ek.time = get_time()/1000.0;
  ek.delta_time = get_delta_time();
  ek.screen_x = 0;
  ek.screen_y = 0;
  ek.screen_width = screen_size_x;
  ek.screen_height = screen_size_y;

  if (item) {
  std::vector<int> vec = item->shader_id();
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      GameApi::SH sh;
      sh.id = vec[i];
      if (sh.id>=0) {
	GameApi::M mat = ev.matrix_api.identity();
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "in_View", mat);
	ev.shader_api.set_var(sh, "in_View2", mat);
	g_view_rot=mat;
      } else
	{
	  std::cout << "execute_ml rejected SH " << sh.id << "coming from shader_id()" << std::endl; 
	}
    }
  
  item->execute(ek);
  }
}
class PrintStats : public MainLoopItem
{
public:
  PrintStats(FaceCollection *coll) : coll(coll) { }
  void Collect(CollectVisitor &vis) {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    std::cout << "Model statistics:" << std::endl;
    std::cout << "NumFaces()=" << coll->NumFaces() << std::endl;

    std::cout << "NumObjects()=" << coll->NumObjects() << std::endl;
    int s = coll->NumObjects();
    for(int i=0;i<s;i++) {
      std::pair<int,int> p = coll->GetObject(i);
      std::cout << "Obj #" << i << ":" << p.first << ".." << p.second << std::endl;
    }

  }
  void Prepare() {
    coll->Prepare();
    std::cout << "Model statistics:" << std::endl;
    std::cout << "NumFaces()=" << coll->NumFaces() << std::endl;

    std::cout << "NumObjects()=" << coll->NumObjects() << std::endl;
    int s = coll->NumObjects();
    for(int i=0;i<s;i++) {
      std::pair<int,int> p = coll->GetObject(i);
      std::cout << "Obj #" << i << ":" << p.first << ".." << p.second << std::endl;
    }
  }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  FaceCollection *coll;
};

GameApi::ML GameApi::MainLoopApi::print_stats(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_main_loop(e,new PrintStats(coll));
}

void GameApi::MainLoopApi::event_ml(ML ml, const Event &ee)
{
  MainLoopEvent e2;
  e2.type = ee.type;
  if (e2.type==Low_SDL_KEYDOWN||e2.type==Low_SDL_KEYUP)
    e2.ch = ee.ch;
  else e2.ch=-1;
  //std::cout << "Type=" << ee.type << " " << (int)e2.ch << std::endl;
  if (e2.type==Low_SDL_MOUSEWHEEL && e2.ch==-1)
    {
      //std::cout << "MOUSE_WHEEL:" << ee.mouse_wheel_y << std::endl;
      if (ee.mouse_wheel_y<0) e2.ch=-1; else
	if (ee.mouse_wheel_y>0) e2.ch=1; else e2.ch=-666;
    }
  if (ee.cursor_pos.id!=-1) {
    e2.cursor_pos = *find_point(e,ee.cursor_pos);
  } else e2.cursor_pos=Point(0.0,0.0,0.0);
  if (e2.type==Low_SDL_MOUSEBUTTONDOWN||e2.type==Low_SDL_MOUSEBUTTONUP)
    e2.button = ee.button;
  else e2.button=-1;
  if (e2.type==Low_SDL_DROPFILE) {
    e2.drag_drop_filename = ee.drag_drop_filename;
  } else e2.drag_drop_filename="";
  e2.joy0_button0 = ee.joy0_button0;
  e2.joy0_button1 = ee.joy0_button1;
  e2.joy0_button2 = ee.joy0_button2;
  e2.joy0_button3 = ee.joy0_button3;
  e2.joy1_button0 = ee.joy1_button0;
  e2.joy1_button1 = ee.joy1_button1;
  e2.joy1_button2 = ee.joy1_button2;
  e2.joy1_button3 = ee.joy1_button3;
  e2.joy0_current_axis = ee.joy0_current_axis;
  e2.joy0_axis0 = ee.joy0_axis0;
  e2.joy0_axis1 = ee.joy0_axis1;
  e2.joy0_axis2 = ee.joy0_axis2;
  e2.joy0_axis3 = ee.joy0_axis3;
  e2.joy0_axis4 = ee.joy0_axis4;
  e2.joy1_current_axis = ee.joy1_current_axis;
  e2.joy1_axis0 = ee.joy1_axis0;
  e2.joy1_axis1 = ee.joy1_axis1;
  e2.joy1_axis2 = ee.joy1_axis2;
  e2.joy1_axis3 = ee.joy1_axis3;
  e2.joy1_axis4 = ee.joy1_axis4;

  e2.joy0_ball0 = ee.joy0_ball0;
  e2.joy0_ball1 = ee.joy0_ball1;
  e2.joy1_ball0 = ee.joy1_ball0;
  e2.joy1_ball1 = ee.joy1_ball1;
  if (ml.id!=-1) {
  MainLoopItem *item = find_main_loop(e, ml);
  if (item)
    item->handle_event(e2);
    }
}
#if 0
class TimingMainLoop : public MainLoopItem
{
public:
  TimingMainLoop(std::vector<MainLoopItem*> vec, float duration) : vec(vec), duration(duration) { firsttime = true; }
  void Prepare() {
  }
  void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	  MainLoopEnv ee = e;
	  vec[i]->execute(ee);
	}
    }
    firsttime = false;
    float t = e.time*10.0;
    current_time = t;
    float t2 = t/duration;
    int t3 = int(t2);
    if (t3<0||t3>=vec.size()) return;
    MainLoopEnv ee = e;
    vec[t3]->execute(ee);
  }
  void handle_event(MainLoopEvent &e)
  {
    float t = current_time;
    float t2 = t/duration;
    int t3 = int(t2);
    if (t3<0||t2>=vec.size()) return;
    vec[t3]->handle_event(e);
  }

private:
  bool firsttime;
  std::vector<MainLoopItem*> vec;
  float duration;
  float current_time;
};
#endif

extern bool g_filter_execute;
int g_filter_start=0;
int g_filter_end=0;
extern bool g_execute_shows_logo;

class FirstExecuteFilterMainLoop : public MainLoopItem
{
public:
  FirstExecuteFilterMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<MainLoopItem*> vec) : env(env), ev(ev), vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);    
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
  }
  void logoexecute() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->logoexecute();
      }
  }
  void execute(MainLoopEnv &e)
  {
    int s = vec.size();
    int start=0;
    int end= s;
    if (g_filter_execute)
      {
	start = g_filter_start;
	end = g_filter_end;
	if (start<0) start=0;
	if (start>s) start=s;
	if (end<0) end=0;
	if (end>s) end=s;
	if (end==s) {
	  g_execute_shows_logo=false;
	}
      }
    
    for(int i=start;i<end;i++)
      {
	MainLoopEnv ee = e;

#ifndef NO_MV
	// here's a block needed to distribute in_MV to different cases.
	std::vector<int> ids = vec[i]->shader_id();
	int s = ids.size();
	for(int j=0;j<s;j++) {
	  int id = ids[j];
	  if (id!=-1) {
	    GameApi::SH sh;
	    sh.id = id;
	    GameApi::M m = add_matrix2( env, e.in_MV);
	    ev.shader_api.use(sh);
	    ev.shader_api.set_var(sh, "in_MV", m);
	  }
	}
#endif
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
  std::vector<int> shader_id() { 
    int s = vec.size();
    std::vector<int> res;
    for(int i=0;i<s;i++)
      {
	std::vector<int> v = vec[i]->shader_id();
	int ss = v.size();
	for(int j=0;j<ss;j++)
	  {
	    res.push_back(v[j]);
	  }
	//if (vec[i]->shader_id()!=-1) return vec[i]->shader_id();
      }
    return res; 
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> vec;
};

class EnsureFrameRateMainLoop : public MainLoopItem
{
public:
  EnsureFrameRateMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<MainLoopItem*> vec, int ticks) : env(env), ev(ev), vec(vec),ticks(ticks) { firsttime=true; }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);    
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
  }
  void logoexecute() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->logoexecute();
      }
  }
  void execute(MainLoopEnv &e)
  {
    g_no_clear = true;
    int s = vec.size();
	unsigned int t = g_low->sdl->SDL_GetTicks();
	for(int i=0;i<s;i++) {
	MainLoopEnv ee = e;

#ifndef NO_MV
	// here's a block needed to distribute in_MV to different cases.
	std::vector<int> ids = vec[i]->shader_id();
	int s = ids.size();
	for(int j=0;j<s;j++) {
	  int id = ids[j];
	  if (id!=-1) {
	    GameApi::SH sh;
	    sh.id = id;
	    GameApi::M m = add_matrix2( env, e.in_MV);
	    ev.shader_api.use(sh);
	    ev.shader_api.set_var(sh, "in_MV", m);
	  }
	}
#endif
	vec[i]->execute(ee);
	exe_count++;
	unsigned int t2 = g_low->sdl->SDL_GetTicks();
	int delta = t2-t;
	if (firsttime) {
	  // first execute can do 1 second
	  if (delta>ticks*600) break;
	} else {
	  // rest of the frames are limited to frame rate
	  if (delta>ticks) break;
	}
	}
	firsttime = false;
  }
  void handle_event(MainLoopEvent &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->handle_event(e);
      }
  }
  std::vector<int> shader_id() { 
    int s = vec.size();
    std::vector<int> res;
    for(int i=0;i<s;i++)
      {
	std::vector<int> v = vec[i]->shader_id();
	int ss = v.size();
	for(int j=0;j<ss;j++)
	  {
	    res.push_back(v[j]);
	  }
	//if (vec[i]->shader_id()!=-1) return vec[i]->shader_id();
      }
    return res; 
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> vec;
  int exe_count=0;
  int ticks;
  bool firsttime;
};
class ArrayMainLoop : public MainLoopItem
{
public:
  ArrayMainLoop(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<MainLoopItem*> vec) : env(env), ev(ev), vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);    
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();
  }
  void logoexecute() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->logoexecute();
      }
  }
  void execute(MainLoopEnv &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	MainLoopEnv ee = e;

#ifndef NO_MV
	// here's a block needed to distribute in_MV to different cases.
	std::vector<int> ids = vec[i]->shader_id();
	int s = ids.size();
	for(int j=0;j<s;j++) {
	  int id = ids[j];
	  if (id!=-1) {
	    GameApi::SH sh;
	    sh.id = id;
	    GameApi::M m = add_matrix2( env, e.in_MV);
	    ev.shader_api.use(sh);
	    ev.shader_api.set_var(sh, "in_MV", m);
	  }
	}
#endif
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
  std::vector<int> shader_id() { 
    int s = vec.size();
    std::vector<int> res;
    for(int i=0;i<s;i++)
      {
	std::vector<int> v = vec[i]->shader_id();
	int ss = v.size();
	for(int j=0;j<ss;j++)
	  {
	    if (v[j]>0)
	      res.push_back(v[j]);
	  }
	//if (vec[i]->shader_id()!=-1) return vec[i]->shader_id();
      }
    return res; 
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> vec;
};

class FrameBufferArrayMainLoop : public FrameBufferLoop
{
public:
  FrameBufferArrayMainLoop(std::vector<FrameBufferLoop*> vec) : vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Collect(vis);
      }
  }
  void HeavyPrepare() { }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Prepare();
      }
  }
  void frame(DrawLoopEnv &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	DrawLoopEnv ee = e;
	vec[i]->frame(ee);
      }
  }
  void handle_event(FrameLoopEvent &e)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->handle_event(e);
      }
  }
#if 0
  std::vector<int> shader_id() { 
    int s = vec.size();
    for(int i=s-1;i>=0;i--)
      {
	if (vec[i]->shader_id()!=-1) return vec[i]->shader_id();
      }
    return -1; 
  }
#endif
private:
  std::vector<FrameBufferLoop*> vec;
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
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glViewport(x,y,sx,sy);
}
/*
EXPORT GameApi::ML GameApi::MainLoopApi::timing_ml(std::vector<ML> vec, float duration)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new TimingMainLoop(vec2));
}
*/
EXPORT GameApi::ML GameApi::MainLoopApi::or_elem_ml(GameApi::EveryApi &ev, ML m1, ML m2)
{
  std::vector<GameApi::ML> vec;
  vec.push_back(m1);
  vec.push_back(m2);
  return array_ml(ev,vec);
}
EXPORT GameApi::ML GameApi::MainLoopApi::slow_start_array_ml(GameApi::EveryApi &ev, std::vector<ML> vec, int max_ticks)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "array_ml id: " << vec[i].id << std::endl;
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new EnsureFrameRateMainLoop(e,ev,vec2,max_ticks));
}
EXPORT GameApi::ML GameApi::MainLoopApi::filter_execute_array_ml(GameApi::EveryApi &ev, std::vector<ML> vec)
{
  g_execute_shows_logo=true;
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "array_ml id: " << vec[i].id << std::endl;
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new FirstExecuteFilterMainLoop(e,ev,vec2));
}
EXPORT GameApi::ML GameApi::MainLoopApi::array_ml(GameApi::EveryApi &ev, std::vector<ML> vec)
{
  //return filter_execute_array_ml(ev,vec);
  
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "array_ml id: " << vec[i].id << std::endl;
      vec2.push_back(find_main_loop(e,vec[i]));
    }
  return add_main_loop(e, new ArrayMainLoop(e,ev,vec2));
  
}
EXPORT GameApi::FML GameApi::MainLoopApi::array_fml(std::vector<FML> vec)
{
  std::vector<FrameBufferLoop*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      //std::cout << "array_ml id: " << vec[i].id << std::endl;
      vec2.push_back(find_framemainloop(e,vec[i]));
    }
  return add_framemainloop(e, new FrameBufferArrayMainLoop(vec2));
}
void save_raw_bitmap(GameApi::Env &e, GameApi::BM bm, std::string filename);

void GameApi::MainLoopApi::save_logo(EveryApi &ev)
{
  BM I1=ev.bitmap_api.newbitmap(500,300,0x00000000);
  Ft I2=ev.font_api.newfont("http://meshpage.org/assets/FreeSans.ttf",80,80);
  BM I3=ev.font_api.font_string(I2,"qtamoVR",5);
  BM I4=ev.bitmap_api.blitbitmap(I1,I3,0,0);
  Ft I5=ev.font_api.newfont("http://meshpage.org/assets/FreeSans.ttf",18,18);
  BM I6=ev.font_api.font_string(I5,"Loading",5);
  BM I7=ev.bitmap_api.blitbitmap(I4,I6,80,88);
  ev.bitmap_api.savebitmap(I7, "logo.ppm", true);
  save_raw_bitmap(e,I7,"logo.raw");
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
  if (!env) return false;
  /*GameApi::MainLoopApi::Event event =*/ //env->ev->mainloop_api.get_event();
    GameApi::MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
	  env->ev->mainloop_api.event_ml(env->res, e);
      }



  env->ev->mainloop_api.clear_3d();
  M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
  M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
  M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
  env->ev->mainloop_api.execute_ml(*env->ev,env->res, env->color, env->texture, env->texture_2d, env->arr,in_MV, in_T, in_N, get_screen_sx(), get_screen_sy());
  env->ev->mainloop_api.swapbuffers();

  
  frame_count++;
  if (frame_count>2) {
    return true;
  }
  return false;
}
extern int async_pending_count;
bool GameApi::MainLoopApi::seamless_iter()
{
  LogoEnv *env = logo_env;
  env->ev->mainloop_api.clear_3d_transparent();
  env->ev->mainloop_api.swapbuffers();
  frame_count++;
  if (frame_count>300 ||async_pending_count==0) {
    return true;
  }
  return false;

}
void GameApi::MainLoopApi::display_seamless(EveryApi &ev)
{
 SH color = ev.shader_api.colour_shader();  
 SH texture = ev.shader_api.texture_shader();
 SH texture_2d = texture;
#ifdef EMSCRIPTEN
 SH arr = texture;
#else
 SH arr = ev.shader_api.texture_shader();
#endif
 ev.mainloop_api.init_3d(color);
 ev.mainloop_api.init_3d(texture);
 ev.mainloop_api.init_3d(texture_2d);
 ev.mainloop_api.init_3d(arr);
 LogoEnv *env = new LogoEnv;
 env->ev = &ev;
 //env->res = res;
 env->color = color;
 env->texture = texture;
 env->texture_2d = texture_2d;
 env->arr = arr;
 logo_env = env;
 frame_count = 0;

}
bool g_transparent = false;
extern bool g_transparent;
int g_background_mode = 0;
unsigned int g_background_center[] = { 0xff000000, 0xff888888, 0xffffffff, 0xff000088,
				       0xffff0000, 0xff00ff00, 0xffffff00,
				       0xff00ffff, 0xffff00ff };
unsigned int g_background_edge[] = { 0xff000000, 0xff000000, 0xff888888, 0xff000000, 
				     0xff880000, 0xff008800, 0xff888800,
				     0xff008888, 0xff880088 };

class Bg : public MainLoopItem
{
public:
  Bg(GameApi::EveryApi &ev) : ev(ev) { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    ev.mainloop_api.clear_3d(0xff000000);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::EveryApi &ev;
};
GameApi::ML GameApi::MainLoopApi::glClear(GameApi::EveryApi &ev)
{
  return add_main_loop(e,new Bg(ev));
}

extern bool g_transparent_indication;

GameApi::ML GameApi::MainLoopApi::display_background(EveryApi &ev, ML ml)
{
  if (g_transparent) {
    if (g_transparent_indication) {
      BM I1 = ev.bitmap_api.chessboard(4,4,get_screen_sx()/4,get_screen_sy()/4,0xff444444,0xff22222);
    BM I2=ev.bitmap_api.scale_bitmap_fullscreen(ev,I1);
    ML I3=ev.sprite_api.vertex_array_render(ev,I2);
    ML I4=ev.sprite_api.turn_to_2d(ev,I3,0.0,0.0,800.0,600.0);
    ML I5=ev.mainloop_api.array_ml(ev,std::vector<ML>{I4,ml});  
    } else 
      return ml;
  } else {
    //BM I1 = ev.bitmap_api.circular_gradient(256,256, g_background_center[g_background_mode], g_background_edge[g_background_mode]);
    //BM I1=ev.bitmap_api.newbitmap(100,100,0xff000000);
    //BM I2=ev.bitmap_api.scale_bitmap_fullscreen(ev,I1);
    //ML I3=ev.sprite_api.vertex_array_render(ev,I2);
    //ML I4=ev.sprite_api.turn_to_2d(ev,I3,0.0,0.0,800.0,600.0);
    ML I4=ev.mainloop_api.glClear(ev);
    ML I5=ev.mainloop_api.array_ml(ev,std::vector<ML>{I4,ml});
    return I5;
  }
}
bool is_mobile(GameApi::EveryApi &ev);

GameApi::BM load_raw_bitmap(GameApi::Env &e, std::string filename);

extern int g_logo_status;

class LogoStatus : public MainLoopItem
{
public:
  LogoStatus(MainLoopItem *connect, MainLoopItem *download, MainLoopItem *prepare) : connect(connect), download(download), prepare(prepare) { }
  virtual void Collect(CollectVisitor &vis)
  {
    connect->Collect(vis);
    download->Collect(vis);
    prepare->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() { Prepare(); }
  virtual void Prepare()
  {
    connect->Prepare();
    download->Prepare();
    prepare->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (g_logo_status==0) connect->execute(e);
    if (g_logo_status==1) download->execute(e);
    if (g_logo_status==2) prepare->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  MainLoopItem *connect, *download, *prepare;
  
};

GameApi::ML GameApi::MainLoopApi::choose_ml_from_status(ML connect, ML download, ML prepare)
{
  MainLoopItem *connect2 = find_main_loop(e,connect);
  MainLoopItem *download2 = find_main_loop(e,download);
  MainLoopItem *prepare2 = find_main_loop(e,prepare);
  MainLoopItem *res = new LogoStatus(connect2, download2, prepare2);
  return add_main_loop(e,res);
}

GameApi::BM load_raw_bitmap2(GameApi::Env &e, std::string data);

#include "editor/logo-connecting.inc"
#include "editor/logo-preparing.inc"
#include "editor/logo-downloading.inc"

extern bool g_has_fullscreen_button;


void GameApi::MainLoopApi::display_logo(EveryApi &ev)
{
#ifdef EMSCRIPTEN
  //BM I7 = ev.bitmap_api.loadbitmap("web_page/logo.ppm");
  int x = 0;
  int y = 0;
  int width = 500;
  int height = 300;
  float div_x = 1;
  float div_y = 1;
  if (is_mobile(ev)) {
    width=200;
    height=100;
    div_x=2.5;
    div_y=2.2;
  }

  std::string s = std::string(logo_connecting_raw,logo_connecting_raw+logo_connecting_raw_len);
  BM I7A = load_raw_bitmap2(e, s /*"web_page/logo-connecting.raw"*/);
  BM I7aA = I7A; //ev.bitmap_api.flip_x(I7A);
  ML I17A;
  {

BM I2=ev.bitmap_api.scale_bitmap(ev,I7aA,width,height);
ML I3=ev.sprite_api.vertex_array_render(ev,I2);
MN I4=ev.move_api.mn_empty();
 MN I5=ev.move_api.scale2(I4,1.5,1.5,1);
 MN I6=ev.move_api.trans2(I5,(220-40-15+30)/div_x,(250-20+100)/div_y,0);
ML I7=ev.move_api.move_ml(ev,I3,I6,1,10.0);

 ML I19=ev.sprite_api.turn_to_2d(ev,I7,0.0,0.0,800.0,600.0);

    
//    ML I19=ev.sprite_api.vertex_array_render(ev,I18);

    //ML I13 = ev.sprite_api.render_sprite_vertex_array_ml(ev, I7a);
//    P I1=ev.polygon_api.quad_z(x+0,x+width,/*is_mobile(ev)?225:*/y+0,/*is_mobile(ev)?525:*/y+height,0);
//MT I3=ev.materials_api.texture(ev,I7aA,1.0);
//MT I4=ev.materials_api.fade(ev,I3,0,5,3000,4000);
//ML I5=ev.materials_api.bind(I1,I3);
//MN I6=ev.move_api.mn_empty();
//MN I7=ev.move_api.scale2(I6,4,4,4);
//MN I8=ev.move_api.trans2(I7,-800,400,0);
//MN I9=ev.move_api.rotatey(I8,-1.59);
//MN I10=ev.move_api.rotate(I9,0,0.00000,0,0,0,0,1,0,1.59);
//MN I10 = ev.move_api.rotatey(I9,1.59);
//MN I11=ev.move_api.trans2(I10,-800,-1400,0);
 I17A=I19; //ev.move_api.move_ml(ev,I5,I11,1,10.0);
  }

  std::string s2 = std::string(logo_downloading_raw,logo_downloading_raw+logo_downloading_raw_len);

  BM I7B = load_raw_bitmap2(e, s2 /*"web_page/logo-downloading.raw"*/);
  BM I7aB = I7B; //ev.bitmap_api.flip_x(I7B);
  ML I17B;
  {
    
BM I2=ev.bitmap_api.scale_bitmap(ev,I7aB,width,height);
ML I3=ev.sprite_api.vertex_array_render(ev,I2);
MN I4=ev.move_api.mn_empty();
 MN I5=ev.move_api.scale2(I4,1.5,1.5,1);
 MN I6=ev.move_api.trans2(I5,(220-40-15+30)/div_x,(250-20+100)/div_y,0);
ML I7=ev.move_api.move_ml(ev,I3,I6,1,10.0);

 ML I19=ev.sprite_api.turn_to_2d(ev,I7,0.0,0.0,800.0,600.0);

    
    //ML I13 = ev.sprite_api.render_sprite_vertex_array_ml(ev, I7a);
//    P I1=ev.polygon_api.quad_z(x+0,x+width,/*is_mobile(ev)?225:*/y+0,/*is_mobile(ev)?525:*/y+height,0);
//MT I3=ev.materials_api.texture(ev,I7aB,1.0);
//MT I4=ev.materials_api.fade(ev,I3,0,5,3000,4000);
//ML I5=ev.materials_api.bind(I1,I3);
//MN I6=ev.move_api.mn_empty();
//MN I7=ev.move_api.scale2(I6,4,4,4);
//MN I8=ev.move_api.trans2(I7,-800,400,0);
//MN I9=ev.move_api.rotatey(I8,-1.59);
//MN I10=ev.move_api.rotate(I9,0,0.00000,0,0,0,0,1,0,1.59);
// MN I10 = ev.move_api.rotatey(I9,1.59);
//MN I11=ev.move_api.trans2(I10,-800,-1400,0);
//I17B=ev.move_api.move_ml(ev,I5,I11,1,10.0);
 I17B = I19;
  }

  std::string s3 = std::string(logo_preparing_raw,logo_preparing_raw+logo_preparing_raw_len);
  BM I7C = load_raw_bitmap2(e, s3 /*"web_page/logo-preparing.raw"*/);
  BM I7aC = I7C; //ev.bitmap_api.flip_x(I7C);
  ML I17C;
  {

BM I2=ev.bitmap_api.scale_bitmap(ev,I7aC,width,height);
ML I3=ev.sprite_api.vertex_array_render(ev,I2);
MN I4=ev.move_api.mn_empty();
 MN I5=ev.move_api.scale2(I4,1.5,1.5,1);
 MN I6=ev.move_api.trans2(I5,(220-40-15+30)/div_x,(250-20+100)/div_y,0);
ML I7=ev.move_api.move_ml(ev,I3,I6,1,10.0);

 
ML I19=ev.sprite_api.turn_to_2d(ev,I7,0.0,0.0,800.0,600.0);

    //ML I13 = ev.sprite_api.render_sprite_vertex_array_ml(ev, I7a);
//    P I1=ev.polygon_api.quad_z(x+0,x+width,/*is_mobile(ev)?225:*/y+0,/*is_mobile(ev)?525:*/y+height,0);
//MT I3=ev.materials_api.texture(ev,I7aC,1.0);
//MT I4=ev.materials_api.fade(ev,I3,0,5,3000,4000);
//ML I5=ev.materials_api.bind(I1,I3);
//MN I6=ev.move_api.mn_empty();
//MN I7=ev.move_api.scale2(I6,4,4,4);
//MN I8=ev.move_api.trans2(I7,-800,400,0);
//MN I9=ev.move_api.rotatey(I8,-1.59);
//MN I10=ev.move_api.rotate(I9,0,0.00000,0,0,0,0,1,0,1.59);
// MN I10 = ev.move_api.rotatey(I9,1.59);
//MN I11=ev.move_api.trans2(I10,-800,-1400,0);
 I17C=I19; //ev.move_api.move_ml(ev,I5,I11,1,10.0);
  }
  ML I17 = ev.mainloop_api.choose_ml_from_status(I17A,I17B,I17C);
  //ML I18 = ev.mainloop_api.
  ML I18=ev.sprite_api.turn_to_2d(ev,I17,0,0,800,600);
    /*
  MN I14=ev.move_api.empty();
  MN I14a=ev.move_api.scale2(I14, 4,4,4);
  MN I14b=ev.move_api.trans2(I14a,-800,400,0);
  MN I15=ev.move_api.rotatey(I14b,-1.59);
  MN I16=ev.move_api.rotate(I15,0,30,0,0,0,0,1,0,1.59);
  MN I16a=ev.move_api.trans2(I16, -800,-1400,0);
  ML I17=ev.move_api.move_ml(ev,I13,I16a);
  */
  ML I26;
  { // PROGRESS BAR
BB I1=ev.bool_bitmap_api.bb_empty(92,12/2);
BB I2=ev.bool_bitmap_api.rectangle(I1,0,0,92,12/2);
BM I3=ev.bool_bitmap_api.to_bitmap(I2,255,255,255,255,0,0,0,0);
ML I4=ev.sprite_api.vertex_array_render(ev,I3);
MN I5=ev.move_api.mn_empty();
MN I6=ev.move_api.trans2(I5,4,4,0);
MN I7=ev.move_api.scale_progress_max(I6,true,false,false);
ML I8=ev.move_api.move_ml(ev,I4,I7,1,10.0);

BB AI1=ev.bool_bitmap_api.bb_empty(92,12/2);
BB AI2=ev.bool_bitmap_api.rectangle(AI1,0,0,92,12/2);
BM AI3=ev.bool_bitmap_api.to_bitmap(AI2,255,255,255,255,0,0,0,0);
ML AI4=ev.sprite_api.vertex_array_render(ev,AI3);
MN AI5=ev.move_api.mn_empty();
MN AI6=ev.move_api.trans2(AI5,4,4+6,0);
MN AI7=ev.move_api.scale_progress(AI6,true,false,false);
ML AI8=ev.move_api.move_ml(ev,AI4,AI7,1,10.0);

 ML BI8=ev.mainloop_api.array_ml(ev,std::vector<ML>{I8,AI8});
 


 
 MN I9=ev.move_api.mn_empty();
MN I10=ev.move_api.scale2(I9,5,5,1);
MN I11=ev.move_api.trans2(I10,-230,-300,0);
ML I12=ev.move_api.move_ml(ev,BI8,I11,1,10.0);
BB I13=ev.bool_bitmap_api.bb_empty(100,20);
BB I14=ev.bool_bitmap_api.rectangle(I13,1,1,98,1);
BB I15=ev.bool_bitmap_api.rectangle(I14,1,18,98,1);
BB I16=ev.bool_bitmap_api.rectangle(I15,1,1,1,18);
BB I17=ev.bool_bitmap_api.rectangle(I16,98,1,1,18);
BM I18=ev.bool_bitmap_api.to_bitmap(I17,255,255,255,255,0,0,0,0);
ML I19=ev.sprite_api.vertex_array_render(ev,I18);
MN I20=ev.move_api.mn_empty();
MN I21=ev.move_api.scale2(I20,5,5,1);
MN I22=ev.move_api.trans2(I21,-230,-300,0);
ML I23=ev.move_api.move_ml(ev,I19,I22,1,10);
 I26=ev.mainloop_api.array_ml(ev,std::vector<ML>{I12,I23});
  }
  ML I17a = ev.mainloop_api.array_ml(ev,std::vector<ML>{I26, I18});



  I17a = ev.mainloop_api.display_background(ev,I17a);

 ML res = I17a;

 if (g_has_fullscreen_button)
   {
     GameApi::ML ml = ev.mainloop_api.fullscreen_button(ev);
     res = ev.mainloop_api.array_ml(ev,std::vector<ML>{res,ml});
   }



#else
BM I18=ev.bitmap_api.newbitmap(500,300,0x00000000);
 FI I19 = ev.font_api.load_font("http://meshpage.org/assets/FreeSans.ttf", 80,80);
 BM I20 = ev.font_api.draw_text_string(I19, "GameApi", 5, 30);

 //Ft I19=ev.font_api.newfont("http://meshpage.org/assets/FreeSans.ttf",80,80);
 //BM I20=ev.font_api.font_string(I19,"GameApi",5);
BM I21=ev.bitmap_api.blitbitmap(I18,I20,0,0);
//Ft I22=ev.font_api.newfont("http://meshpage.org/assets/FreeSans.ttf",18,18);
//BM I23=ev.font_api.font_string(I22,"Win32Edition",5);
 FI I22 = ev.font_api.load_font("http://meshpage.org/assets/FreeSans.ttf", 18,18);
 BM I23 = ev.font_api.draw_text_string(I22, "Win32Edition", 5, 30);
BM I24=ev.bitmap_api.blitbitmap(I21,I23,160,88);
 P I25=ev.polygon_api.color_map(I24,0.0,500,0.0,300.0,0);
 //P I26=ev.polygon_api.rotatex(I25,3.14159);
P I27=ev.polygon_api.scale(I25,2,-2,2);
P I28=ev.polygon_api.translate(I27,-400,200,0);
VA I29=ev.polygon_api.create_vertex_array(I28,true);
ML I30=ev.polygon_api.render_vertex_array_ml(ev,I29);
MN I31=ev.move_api.mn_empty();
MN I32=ev.move_api.rotatey(I31,-1.59);
MN I33=ev.move_api.rotate(I32,0,30,0,0,0,0,1,0,1.59);
ML I34=ev.move_api.move_ml(ev,I30,I33);
 I34 = ev.mainloop_api.display_background(ev,I34);
 ML res = I34;
#endif
 //ML res2 = ev.mainloop_api.flip_scene_x_if_mobile(ev,res);
 SH color = ev.shader_api.colour_shader();  
 SH texture = ev.shader_api.texture_shader();
 SH texture_2d = texture;
#ifdef EMSCRIPTEN
 SH arr = texture;
#else
 SH arr = ev.shader_api.texture_shader();
#endif
 ev.mainloop_api.init_3d(color);
 ev.mainloop_api.init_3d(texture);
 ev.mainloop_api.init_3d(texture_2d);
 ev.mainloop_api.init_3d(arr);

  MainLoopItem *item = find_main_loop(e,res);
 item->Prepare();
 
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

extern  Matrix g_in_P;
extern bool g_in_P_used;

GameApi::M GameApi::MainLoopApi::in_P(EveryApi &ev, bool is_3d)
{
  if (g_in_P_used)
    {
      return add_matrix2(ev.get_env(),g_in_P);
    }



  int screenx = get_screen_width();
  int screeny = get_screen_height();
  if (is_3d)
    {
      return ev.matrix_api.perspective(80.0, (double)screenx/screeny, 10.1, 600.0);
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
    firsttime2 = true;
    vec=0;
  }
  ~SongML() { delete vec; }
  void Collect(CollectVisitor &vis) { next->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  void Prepare() {
    if (firsttime2) {
    next->Prepare();
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    if (tolower(url[url.size()-3])=='o'&&tolower(url[url.size()-2])=='g'&&tolower(url[url.size()-1])=='g') is_ogg=true; else is_ogg=false;

    if (!is_ogg) { std::cout << "WARNING: MP3 PLAYING DOESN'T WORK. USE OGG FILES." << std::endl; }
    
    GameApi::ASyncVec *ptr = env.get_loaded_async_url(url);
    //std::cout << "SONG SIZE: "<< ptr->size() << std::endl;
    vec = new std::vector<unsigned char>(ptr->begin(),ptr->end());
    firsttime2 = false;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
#ifdef EMSCRIPTEN
    // web browser wants music to start on click events.
    if (e.button==0||e.type==0x300)
      
#endif
    if (firsttime) {
      std::cout << "Playing music..." << std::endl;
      // setup ogg can also play mp3s
      ptr2 = ev.tracker_api.setup_ogg(*vec,is_ogg?1:2);
      //std::ofstream ss("song.ogg", std::ofstream::out | std::ofstream::binary);
      //int s = ptr->size();
      //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
      //ss.close();
      //#ifndef EMSCRIPTEN
      //ev.tracker_api.play_ogg("song.ogg");
      ev.tracker_api.play_ogg(ptr2);
      //#else
	// std::cout << "Warning: ogg playing disabled since it didn't work in emscripten." << std::endl;
      //#endif
      firsttime = false;
    }
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  bool firsttime;
  bool firsttime2;
  MainLoopItem *next;
  std::string homepage;
  std::vector<unsigned char> *vec;
  void *ptr2;
  bool is_ogg=true;
};

GameApi::ML GameApi::MainLoopApi::load_song(EveryApi &ev, ML next, std::string url)
{
  MainLoopItem *nxt = find_main_loop(e, next);
  std::string homepage = ev.mainloop_api.get_homepage_url();
  return add_main_loop(e, new SongML(e,ev, url, nxt, homepage));
}

class MidiML : public MainLoopItem
{
public:
  MidiML(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string url_patchset, MainLoopItem *next, std::string homepage) : env(env), ev(ev), url(url), url_patchset(url_patchset), next(next), homepage(homepage) {
    firsttime = true;
    firsttime2 = true;
    vec=0;
  }
  ~MidiML() { delete vec; }
  void Collect(CollectVisitor &vis) { next->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  void Prepare() {
    if (firsttime2) {
    next->Prepare();
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
    env.async_load_url(url_patchset, homepage);
#endif
    if (url[url.size()-3]=='o'&&url[url.size()-2]=='g'&&url[url.size()-1]=='g') is_ogg=true; else is_ogg=false;
    
    GameApi::ASyncVec *ptr = env.get_loaded_async_url(url);
    vec = new std::vector<unsigned char>(ptr->begin(),ptr->end());

    GameApi::ASyncVec *ptr2 = env.get_loaded_async_url(url_patchset);
    vec2 = new std::vector<unsigned char>(ptr2->begin(),ptr2->end());

    firsttime2 = false;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
#ifdef EMSCRIPTEN
    // web browser wants music to start on click events.
    if (e.button==0||e.type==0x300)
      
#endif
    if (firsttime) {
      // setup ogg can also play mp3s
      ptr2 = setup_midi(*vec,*vec2);
      //std::ofstream ss("song.ogg", std::ofstream::out | std::ofstream::binary);
      //int s = ptr->size();
      //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
      //ss.close();
      //#ifndef EMSCRIPTEN
      //ev.tracker_api.play_ogg("song.ogg");
      play_midi(ptr2);
      //#else
	// std::cout << "Warning: ogg playing disabled since it didn't work in emscripten." << std::endl;
      //#endif
      firsttime = false;
    }
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  std::string url_patchset;
  bool firsttime;
  bool firsttime2;
  MainLoopItem *next;
  std::string homepage;
  std::vector<unsigned char> *vec;
  std::vector<unsigned char> *vec2;
  void *ptr2;
  bool is_ogg=true;
};


GameApi::ML GameApi::MainLoopApi::load_midi(EveryApi &ev, ML next, std::string url, std::string url_patchset)
{
  MainLoopItem *nxt = find_main_loop(e, next);
  std::string homepage = ev.mainloop_api.get_homepage_url();
  return add_main_loop(e, new MidiML(e,ev, url, url_patchset, nxt, homepage));
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
  void Collect(CollectVisitor &vis) { item->Collect(vis); item2->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { item->Prepare(); item2->Prepare(); }
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
  virtual std::vector<int> shader_id() { if (!toggle) return item->shader_id(); else return item2->shader_id(); }

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
#if 0
class PreparePTS : public MainLoopItem
{
public:
  PreparePTS(MainLoopItem *item, PointsApiPoints *points) : item(item), points(points), firsttime(true) {}
  void Collect(CollectVisitor &vis) {
    item->Collect(vis); points->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { item->Prepare(); points->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
      //points->Prepare();
    }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return item->shader_id(); }

private:
  MainLoopItem *item;
  PointsApiPoints *points;
  bool firsttime;
};
#endif
GameApi::ML GameApi::MainLoopApi::prepare_pts(ML ml, PTS pts)
{
#if 0
  MainLoopItem *item = find_main_loop(e,ml);
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_main_loop(e, new PreparePTS(item, points));
#endif
}

class DepthFunc : public MainLoopItem
{
public:
  DepthFunc(MainLoopItem *next, int i) : next(next),i(i) {}
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() {next->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
  OpenglLowApi *ogl = g_low->ogl;
  int val = Low_GL_LEQUAL;
    switch(i) {
    case 0: val=Low_GL_LEQUAL; break;
    case 1: val=Low_GL_EQUAL; break;
    case 2: val=Low_GL_ALWAYS; break;
    case 3: val=Low_GL_LESS; break;
    case 4: val=Low_GL_GREATER; break;
    case 5: val=Low_GL_GEQUAL; break;
    case 6: val=Low_GL_NOTEQUAL; break;
    };
    MainLoopEnv ee = e;
    ee.depthfunc=val;
    ogl->glDepthFunc(val);
    next->execute(ee);
    ogl->glDepthFunc(e.depthfunc);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  int i;
};

class DepthMask : public MainLoopItem
{
public:
  DepthMask(MainLoopItem *next, bool b) : next(next), b(b) { }
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() {next->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
  OpenglLowApi *ogl = g_low->ogl;
    if (b) {
      ogl->glDepthMask(Low_GL_TRUE);
    } else {
      ogl->glDepthMask(Low_GL_FALSE);
    }
    MainLoopEnv ee= e;
    ee.depthmask=b?Low_GL_TRUE:Low_GL_FALSE;
    next->execute(ee);
    ogl->glDepthMask(e.depthmask);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  bool b;  
};

GameApi::ML GameApi::MainLoopApi::depthfunc(ML ml, int val)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new DepthFunc(next,val));
}

GameApi::ML GameApi::MainLoopApi::depthmask(ML ml, bool b)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new DepthMask(next,b));
}


class CullFace2 : public MainLoopItem
{
public:
  CullFace2(MainLoopItem *next, bool b, bool is_gltf) : next(next),b(b),is_gltf(is_gltf) { }
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() {next->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
    OpenglLowApi *ogl = g_low->ogl;
    MainLoopEnv ee = e;
    if (b) {
      ogl->glEnable(Low_GL_CULL_FACE);
      if (is_gltf) {
	ogl->glFrontFace(Low_GL_CCW); // this is ccw because gltf models are odd
      } else {
	ogl->glFrontFace(Low_GL_CW); // this is ccw because gltf models are odd
      }
      ee.cullface=true;
    } else {
      ogl->glDisable(Low_GL_CULL_FACE);
      ogl->glFrontFace(Low_GL_CW);
      ee.cullface=false;
    }
    next->execute(ee);
    ogl->glDisable(Low_GL_CULL_FACE);
      ogl->glFrontFace(Low_GL_CW);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  bool b;
  bool is_gltf;
};


class BlendFunc : public MainLoopItem
{
public:
  BlendFunc(MainLoopItem *next, int i, int i2) : next(next),i(i),i2(i2) {}
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() {next->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
  OpenglLowApi *ogl = g_low->ogl;
    Low_GLenum s = Low_GL_SRC_COLOR;
    Low_GLenum d = Low_GL_ONE_MINUS_SRC_COLOR;
    switch(i) {
    case 0: s=Low_GL_ZERO; break;
    case 1: s=Low_GL_ONE; break;
    case 2: s=Low_GL_SRC_COLOR; break;
    case 3: s=Low_GL_ONE_MINUS_SRC_COLOR; break;
    case 4: s=Low_GL_DST_COLOR; break;
    case 5: s=Low_GL_ONE_MINUS_DST_COLOR; break;
    case 6: s=Low_GL_SRC_ALPHA; break;
    case 7: s=Low_GL_ONE_MINUS_SRC_ALPHA; break;
    case 8: s=Low_GL_DST_ALPHA; break;
    case 9: s=Low_GL_ONE_MINUS_DST_ALPHA; break;
    case 10: s=Low_GL_CONSTANT_COLOR; break;
    case 11: s=Low_GL_ONE_MINUS_CONSTANT_COLOR; break;
    case 12: s=Low_GL_CONSTANT_ALPHA; break;
    };

    switch(i2) {
    case 0: d=Low_GL_ZERO; break;
    case 1: d=Low_GL_ONE; break;
    case 2: d=Low_GL_SRC_COLOR; break;
    case 3: d=Low_GL_ONE_MINUS_SRC_COLOR; break;
    case 4: d=Low_GL_DST_COLOR; break;
    case 5: d=Low_GL_ONE_MINUS_DST_COLOR; break;
    case 6: d=Low_GL_SRC_ALPHA; break;
    case 7: d=Low_GL_ONE_MINUS_SRC_ALPHA; break;
    case 8: d=Low_GL_DST_ALPHA; break;
    case 9: d=Low_GL_ONE_MINUS_DST_ALPHA; break;
    case 10: d=Low_GL_CONSTANT_COLOR; break;
    case 11: d=Low_GL_ONE_MINUS_CONSTANT_COLOR; break;
    case 12: d=Low_GL_CONSTANT_ALPHA; break;
    };
    ogl->glBlendFunc(s,d);
    next->execute(e);
    ogl->glBlendFunc(Low_GL_SRC_ALPHA,Low_GL_ONE_MINUS_SRC_ALPHA);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  int i,i2;
};

GameApi::ML GameApi::MainLoopApi::cullface(ML ml, bool b, bool gltf)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new CullFace2(next,b,gltf));
  
}
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
  MN I15=ev.move_api.mn_empty();
  MN I16=ev.move_api.trans2(I15,150,300,0);
  ML I17=ev.move_api.move_ml(ev,I14,I16,1,10.0);
  ML I18=ev.sprite_api.turn_to_2d(ev,I17,0.0,0.0,800.0,600.0);
  ML I19=ev.mainloop_api.array_ml(ev,std::vector<ML>{I18,ml});
  return I19;
}



struct Store {
  float time;
  int type;
  int ch;
  Point cursor_pos;
  int button;
  std::string drag_drop_filename;
};

class RecordKeyPresses : public MainLoopItem
{
public:
  RecordKeyPresses(MainLoopItem *item, std::string output_filename) : item(item), output_filename(output_filename) { }

  void Collect(CollectVisitor &vis) { item->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() {item->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
    time = e.time;
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (e.ch=='o' && e.type==0x300) {
      save();
    }
    Store s;
    s.time = time;
    s.type=e.type;
    s.ch = e.ch;
    s.cursor_pos = e.cursor_pos;
    s.button = e.button;
    s.drag_drop_filename = e.drag_drop_filename;
    vec.push_back(s);
    item->handle_event(e);    
  }
  virtual std::vector<int> shader_id() { return item->shader_id(); }

  void save() {
    std::ofstream ss(output_filename.c_str());
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Store s = vec[i];
	ss << "{ " << s.time << " , " << s.type << " , " << s.ch << " , " << s.cursor_pos << " , " << s.button << " }" << std::endl;
      }
    ss.close();
  }
private:
  float time;
  MainLoopItem *item;
  std::string output_filename;
  std::vector<Store> vec;
};

GameApi::ML GameApi::MainLoopApi::record_keypresses(ML ml, std::string output_filename)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new RecordKeyPresses(item, output_filename));
}

class PlaybackKeyPresses : public MainLoopItem
{
public:
  PlaybackKeyPresses(GameApi::Env &ee, MainLoopItem *item, std::string url, std::string homepageurl) : ee(ee), item(item), url(url), homepageurl(homepageurl) { current_item = 0; firsttime = true; }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
#ifndef EMSCRIPTEN
    ee.async_load_url(url, homepageurl);
#endif
    GameApi::ASyncVec *vec = ee.get_loaded_async_url(url);
    std::string ss(vec->begin(), vec->end());
    load(ss);

  }
  virtual void execute(MainLoopEnv &e) {
    if (firsttime) {
    firsttime = false;
    }
    time = e.time; item->execute(e);

    int s = vec.size();
    for(int i=current_item;i<s;i++) {
      Store s = vec[i];
      if (time<s.time) { current_item=i; break; }
      if (time>=s.time) {
	MainLoopEvent ee;
	ee.type = s.type;
	ee.ch = s.ch;
	ee.cursor_pos = s.cursor_pos;
	ee.button = s.button;
	ee.drag_drop_filename = s.drag_drop_filename;
	item->handle_event(ee);
	//std::cout << "Event: " << ee.ch << " " << ee.type << std::endl;
      }
    }


  }
  virtual void handle_event(MainLoopEvent &e) {
    // TODO, is this call needed?
    //item->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return item->shader_id(); }

  void load(std::string contents) {
    std::stringstream ss(contents);
    char ch;
    Store s;
    while(ss >> ch >> s.time >> ch >> s.type >> ch >> s.ch >> ch >> s.cursor_pos >> ch >> s.button >> ch) {
      vec.push_back(s);
      //std::cout << s.time << " " << s.type << " " << s.ch << " " << s.cursor_pos << " " << s.button << std::endl;
    }
  }
private:
  GameApi::Env &ee;
  float time;
  bool firsttime;
  MainLoopItem *item;
  std::string url;
  std::string homepageurl;
  std::vector<Store> vec;
  int current_item;
};
GameApi::ML GameApi::MainLoopApi::playback_keypresses(ML ml, std::string url)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new PlaybackKeyPresses(e,item, url, gameapi_homepageurl));
}

class DynamicMainLoop : public MainLoopItem
{
public:
  DynamicMainLoop(DynMainLoop *d) : d(d) { }
  void Collect(CollectVisitor &vis) {

    int s = d->num();
    for(int i=0;i<s;i++) {
      MainLoopItem *item = d->get_mainloop(i);
      item->Collect(vis);
    }

  }
  void HeavyPrepare() { }

  virtual void Prepare() {
    int s = d->num();
    for(int i=0;i<s;i++) {
      MainLoopItem *item = d->get_mainloop(i);
      item->Prepare();
    }
  }
  virtual void execute(MainLoopEnv &e)
  {
    int s = d->num();
    for(int i=0;i<s;i++) {
      MainLoopItem *item = d->get_mainloop(i);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = d->num();
    for(int i=0;i<s;i++) {
      MainLoopItem *item = d->get_mainloop(i);
      item->handle_event(e);
    }
  }

  virtual std::vector<int> shader_id() {
    int s = d->num();
    std::vector<int> vec;
    for(int i=0;i<s;i++) {
      MainLoopItem *item = d->get_mainloop(i);
      std::vector<int> id = item->shader_id();
      std::copy(id.begin(),id.end(),std::back_inserter(vec));
    }
    return vec;
  }
private:
  DynMainLoop *d;
};

GameApi::ML GameApi::MainLoopApi::dyn(D d)
{
  DynMainLoop *d2 = find_dyn(e,d);
  return add_main_loop(e,new DynamicMainLoop(d2));
}

class JoystickToWasd : public MainLoopItem
{
public:
  JoystickToWasd(MainLoopItem *item) : item(item) {

    button_state[0] = false;
    button_state[1] = false;
    button_state[2] = false;
    button_state[3] = false;
  }
  void Collect(CollectVisitor &vis)
  {
    item->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    MainLoopEvent ee = e;
    if (e.type==Low_SDL_JOYAXISMOTION) {
    if (e.joy0_current_axis==0 && e.joy0_axis0 < 0 && button_state[0]==false) { // keydown 'a'
      button_state[0]=true;

      ee.type=768;
      ee.ch='a';
    item->handle_event(ee);
    } else
    if (e.joy0_current_axis==0 && e.joy0_axis0 >= 0 && button_state[0]==true) { // keyup 'a'
      button_state[0]=false;

      ee.type=769;
      ee.ch='a';
    item->handle_event(ee);
    }

    if (e.joy0_current_axis==0 && e.joy0_axis0 > 0 && button_state[1]==false) { // keydown 'd'
      button_state[1]=true;

      ee.type=768;
      ee.ch='d';
    item->handle_event(ee);
    } else
    if (e.joy0_current_axis==0 && e.joy0_axis0 <= 0 && button_state[1]==true) { // keyup 'd'
      button_state[1]=false;

      ee.type=769;
      ee.ch='d';
    item->handle_event(ee);
    }

    if (e.joy0_current_axis==1 && e.joy0_axis1 > 0 && button_state[2]==false) { // keydown 's'
      button_state[2]=true;

      ee.type=768;
      ee.ch='s';
    item->handle_event(ee);
    } else
    if (e.joy0_current_axis==1 && e.joy0_axis1 <= 0 && button_state[2]==true) { // keyup 's'
      button_state[2]=false;

      ee.type=769;
      ee.ch='s';
    item->handle_event(ee);
    }

    if (e.joy0_current_axis==1 && e.joy0_axis1 < 0 && button_state[3]==false) { // keydown 'w'
      button_state[3]=true;

      ee.type=768;
      ee.ch='w';
    item->handle_event(ee);
    } else
    if (e.joy0_current_axis==1 && e.joy0_axis1 >= 0 && button_state[3]==true) { // keyup 'w'
      button_state[3]=false;

      ee.type=769;
      ee.ch='w';
    item->handle_event(ee);
    }
    }
    
    item->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    return item->shader_id();
  }
private:
  MainLoopItem *item;
  bool button_state[4];
};

GameApi::ML GameApi::MainLoopApi::joystick_to_wasd(ML ml)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new JoystickToWasd(item));
}

class DisableZBuffer : public MainLoopItem
{
public:
  DisableZBuffer(MainLoopItem *item) : item(item) { }
  void Collect(CollectVisitor &vis) { item->Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
    OpenglLowApi *ogl = g_low->ogl;
    ogl->glDisable(Low_GL_DEPTH_TEST);
    item->execute(e);
    ogl->glEnable(Low_GL_DEPTH_TEST);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return item->shader_id(); }
private:
  MainLoopItem *item;
};

GameApi::ML GameApi::MainLoopApi::disable_z_buffer(ML ml)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new DisableZBuffer(item));
}

class SendKeyAtTime : public MainLoopItem
{
public:
  SendKeyAtTime(MainLoopItem *item, float time, int key) : item(item), time(time), key(key) { done = false;}
  virtual void logoexecute() { item->logoexecute(); }
  virtual void Collect(CollectVisitor &vis) { item->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { item->Prepare(); }
  virtual void execute(MainLoopEnv &e) {
    if (!done && e.time>time)
      {
	MainLoopEvent ee;
	ee.type = 0x300;
	ee.ch = key;
	ee.cursor_pos = Point(0.0,0.0,0.0);
	ee.button = -1;
	item->handle_event(ee);
	done = true;
      }
    if (e.time<time) done=false;
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }   
  virtual std::vector<int> shader_id() { return item->shader_id(); }
private:
  MainLoopItem *item;
  float time;
  int key;
  bool done;
};

GameApi::ML GameApi::MainLoopApi::send_key_at_time(ML ml, float time, int key)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new SendKeyAtTime(item, time, key));
};

class ScreenSpace : public MainLoopItem
{
public:
  ScreenSpace(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML scene, ScreenSpaceMaterial *mat) : env(e), ev(ev), scene(scene), mat(mat) { firsttime = true; }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis) {
    MainLoopItem *item = find_main_loop(env,scene);
    item->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() { mat->Prepare(); }
  virtual void Prepare() {
    MainLoopItem *item = find_main_loop(env,scene);
    item->Prepare();
    HeavyPrepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
      GameApi::P I1=ev.polygon_api.quad_z(-1024.0,1024.0,-1024.0,1024.0,0.0);
      GameApi::MN mn0 = ev.move_api.mn_empty();
      GameApi::MN mn = ev.move_api.scale2(mn0,((600-40)*2)/2048.0,((600-40)*2)/2048.0,1.0);
      GameApi::ML scene2 = ev.move_api.move_ml(ev,scene,mn,1,10.0);

      
      // POSITION TXID GENERATION
      GameApi::MT aI1=ev.materials_api.m_def(ev);
      GameApi::SHP aI2=ev.mainloop_api.empty_shp();
      GameApi::ARR aI3=ev.mainloop_api.load_shader2("https://meshpage.org/assets/position.vert","https://meshpage.org/assets/position.frag");
      ArrayType *arr=find_array(env,aI3);
      GameApi::SHC o1,o2;
      o1.id = arr->vec[0];
      o2.id = arr->vec[1];
      GameApi::SHI aI4=ev.mainloop_api.generic_anim_shader2(ev,aI2,"position",o1,std::vector<GameApi::SHI>());
      GameApi::SHI aI5=ev.mainloop_api.generic_anim_shader2(ev,aI2,"position",o2,std::vector<GameApi::SHI>());
      //GameApi::MT aI6=ev.materials_api.generic_shader_material00(ev,aI1,aI4,aI5);
      //GameApi::ML aI7=ev.materials_api.bind(I1,aI6);
      GameApi::ML aI7 = ev.mainloop_api.generic_shader(ev,scene2,aI4,aI5,std::vector<GameApi::TXID>());
      GameApi::TXID aI8=ev.fbo_api.fbo_ml(ev,aI7,-1,-1,false);


      GameApi::MT bI1=ev.materials_api.m_def(ev);
      GameApi::SHP bI2=ev.mainloop_api.empty_shp();
      GameApi::ARR bI3=ev.mainloop_api.load_shader2("https://meshpage.org/assets/normal.vert","https://meshpage.org/assets/normal.frag");
      ArrayType *arr2=find_array(env,bI3);
      GameApi::SHC bo1,bo2;
      bo1.id = arr2->vec[0];
      bo2.id = arr2->vec[1];
      GameApi::SHI bI4=ev.mainloop_api.generic_anim_shader2(ev,bI2,"normal",bo1,std::vector<GameApi::SHI>());
      GameApi::SHI bI5=ev.mainloop_api.generic_anim_shader2(ev,bI2,"normal",bo2,std::vector<GameApi::SHI>());
      //GameApi::MT bI6=ev.materials_api.generic_shader_material00(ev,bI1,bI4,bI5);
      //GameApi::ML bI7=ev.materials_api.bind(I1,bI6);
      GameApi::ML bI7 = ev.mainloop_api.generic_shader(ev,scene2,bI4,bI5, std::vector<GameApi::TXID>());
      GameApi::TXID bI8=ev.fbo_api.fbo_ml(ev,bI7,-1,-1,false);

      
      GameApi::TXID I4 = ev.fbo_api.fbo_ml(ev,scene2,2048,2048,false);
      GameApi::TXID I4_depth = ev.fbo_api.depth_ml(ev,scene,2048,2048,false);
      ml = mat->mat(I4.id,I4_depth.id,I1.id,aI8.id,bI8.id);
      MainLoopItem *item2 = find_main_loop(env,ml);
      item2->Prepare();
    }
    MainLoopItem *item = find_main_loop(env,ml);
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }

  }
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      return item->shader_id();
    }
    return std::vector<int>();
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::ML scene;
  ScreenSpaceMaterial *mat;
  GameApi::ML ml = { -1 };
  bool firsttime;
};

GameApi::ML GameApi::MainLoopApi::screenspace_rendering(EveryApi &ev, ML scene, SMT screenspace_material)
{
  ScreenSpaceMaterial *smat = find_screenspace_material(e,screenspace_material);
  return add_main_loop(e, new ScreenSpace(e,ev,scene,smat));
}

extern bool g_inside_ml_widget;
bool g_transparent_indication;
extern std::string gameapi_seamless_url;

class SceneTransparency : public MainLoopItem
{
public:
  SceneTransparency(MainLoopItem *ml) : ml(ml) {
    g_transparent=true;
    if (g_inside_ml_widget) {
      g_transparent_indication=true;
    }
#ifdef EMSCRIPTEN
    gameapi_seamless_url="@";
#endif
  }
  ~SceneTransparency() {
    g_transparent=false;
    g_transparent_indication=false;
#ifdef EMSCRIPTEN
    gameapi_seamless_url="";
#endif
  }
  virtual void Collect(CollectVisitor &vis) { ml->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { ml->Prepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
    ml->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { ml->handle_event(e); }
  virtual std::vector<int> shader_id() { return ml->shader_id(); }
private:
  
  MainLoopItem *ml;
};

GameApi::ML GameApi::MainLoopApi::scene_transparency(ML scene)
{
  MainLoopItem *ml = find_main_loop(e,scene);
  return add_main_loop(e, new SceneTransparency(ml));
}



class Timing2 : public Timing
{
public:
  Timing2(GameApi::Env &env, float duration, GameApi::ML show, GameApi::TT link) : env(env), duration(duration), show(show), link(link),valid(true),is_switch(false)
  {
    Timing *item = find_timing(env,link);
    Timing *timing = item;
    start_time2 = timing->end_time(); //+timing->delta_time();
    end_time2 = start_time2+duration; //+timing->delta_time();
  }
  Timing2(GameApi::Env &env, float duration, GameApi::ML show, GameApi::ML show2, GameApi::TT link, int switch_dir) : env(env), duration(duration), show(show), show2(show2), link(link), valid(true),is_switch(true),switch_dir(switch_dir)
  {
    Timing *item = find_timing(env,link);
    Timing *timing = item;
    start_time2 = timing->end_time(); //+timing->delta_time();
    end_time2 = start_time2+duration; //+timing->delta_time();
  }
  Timing2(GameApi::Env &e) : env(e),valid(false),is_switch(false) { start_time2=0.0; end_time2=0.0; }
  float end_time() const 
  {
    return end_time2;
  }
  float delta_time() const
  {
    if (valid) {
    Timing *item = find_timing(env,link);
    Timing *timing = item;
    //if (is_switch) {
    //  return end_time2-start_time2+timing->delta_time();
    //}
    return timing->delta_time();
    } else return 0.0;
  }
  Timing* clone() const { return new Timing2(env,duration,show,link); }

  virtual void Collect(CollectVisitor &vis)
  {
    if (valid) {
    MainLoopItem *item = find_main_loop(env,show);
    item->Collect(vis);
    if (is_switch) {
      MainLoopItem *item = find_main_loop(env,show2);
      item->Collect(vis);
    }
    Timing *t = find_timing(env,link);
    t->Collect(vis);
    }
  }
  virtual void HeavyPrepare() {}
  virtual void Prepare()
  {
    if (valid) {
    MainLoopItem *item = find_main_loop(env,show);
    item->Prepare();
    if (is_switch) {
      MainLoopItem *item = find_main_loop(env,show2);
      item->Prepare();
    }
    Timing *t = find_timing(env,link);
    t->Prepare();
    }
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (valid) {
    if (e.time>=start_time2 && e.time<=end_time2) {
      in_timerange=true;
      MainLoopItem *item = find_main_loop(env,show);
      //MainLoopEnv ee = e;
      //ee.time = e.time-start_time2;
      item->execute(e);
      if (is_switch) {
	MainLoopItem *item = find_main_loop(env,show2);
	item->execute(e);
      }
    } else {
      in_timerange=false;
      Timing *item = find_timing(env,link);
      MainLoopEnv ee = e;
      ee.time = e.time+delta_time();
      item->execute(ee);
    }
    }
  }
  virtual void handle_event(MainLoopEvent &e)
    {
      if (valid && in_timerange) {
    MainLoopItem *item = find_main_loop(env,show);
    item->handle_event(e);
      } else if (valid && !in_timerange) {
	Timing *item = find_timing(env,link);
	item->handle_event(e);
	
      }
    }
  virtual std::vector<int> shader_id() {
    if (valid && in_timerange) {
    MainLoopItem *item = find_main_loop(env,show);
    return item->shader_id();
    } else if (valid && !in_timerange) {
      Timing *item = find_timing(env,link);
      return item->shader_id();
    } else return std::vector<int>();
  }
private:
 GameApi::Env &env;
 float duration;
 GameApi::ML show;
 GameApi::ML show2;
 GameApi::TT link;
 float start_time2;
 float end_time2;
 bool valid;
 bool in_timerange;
 bool is_switch;
 int switch_dir;
};


GameApi::TT GameApi::MainLoopApi::timing_start()
{
  return add_timing(e, new Timing2(e));
}

GameApi::TT GameApi::MainLoopApi::timing(float duration, TT link, ML show)
{
  return add_timing(e, new Timing2(e,duration,show,link));
}
GameApi::TT GameApi::MainLoopApi::timing_switch(EveryApi &ev, float duration, TT link, ML show, ML show2, int switch_dir)
{
  Timing *t = find_timing(e,link);
  float start_time = t->end_time();

  float move_amount = 1800.0;
  
  GameApi::MN I1 = ev.move_api.mn_empty();
  GameApi::MN I2 = ev.move_api.translate(I1, start_time, (start_time+duration), move_amount, 0.0, 0.0); 

  GameApi::MN AI1 = ev.move_api.mn_empty();
  GameApi::MN AI11 = ev.move_api.trans2(AI1,-move_amount,0.0,0.0);
  GameApi::MN AI2 = ev.move_api.translate(AI11, start_time, (start_time+duration), move_amount, 0.0, 0.0); 

  GameApi::ML new_show = ev.move_api.move_ml(ev,show, I2, 1, 10.0);
  GameApi::ML new_show2 = ev.move_api.move_ml(ev,show2, AI2, 1, 10.0);
  
  return add_timing(e, new Timing2(e,duration,new_show,new_show2,link,switch_dir));
}

GameApi::ML GameApi::MainLoopApi::timing_exit(TT link)
{
  Timing *t = find_timing(e,link)->clone();
  MainLoopItem *item = (MainLoopItem*)t;
  return add_main_loop(e,item);
}

bool g_concurrent_download=false;

class ConcurrentDownload : public MainLoopItem
{
public:
  ConcurrentDownload(MainLoopItem *next) : next(next) {
    g_concurrent_download=true;
  }
  ~ConcurrentDownload() { g_concurrent_download=false; }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};

GameApi::ML GameApi::MainLoopApi::concurrent_download(ML ml)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new ConcurrentDownload(item));
}


#if 0
GameApi::ML GameApi::MainLoopApi::custom_element(EveryApi &ev, std::string name, std::string scene_url, std::string param_names, std::string param_types, std::string param_default_values)
{
}
#endif
