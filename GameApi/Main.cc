// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//

// this is also in GameApi_h.hh
#ifndef LINUX
#ifndef EMSCRIPTEN
#ifndef ANDROID
#define VIRTUAL_REALITY 1
#endif
#endif
#endif
//#define VIRTUAL_REALITY_OVERLAY 1
#define SDL2_USED
#define NO_SDL_GLEXT
//#define GL3_PROTOTYPES 1
//#include <GL3/gl3.h>
//#define GLEW_STATIC
#ifndef ARM
//#ifndef LINUX
//#ifndef RASPI
#ifndef ANDROID
#define USE_GLEW 1
#include <GL/glew.h>
#endif
//#endif
//#endif
#endif
#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef BD_CALVIN_COMPUTER
#include 
#endif

#if 0
//#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
#endif

// this flag needs to be changes also from
// 1) GameApi_h.hh
// 2) Main.cc
// 3) Shader.cc
//#define OPENGL_ES 1
#ifdef RASPI
#define OPENGL_ES 1
#endif

#ifdef LINUX
//#define OPENGL_ES 1
#endif

#ifdef WINDOWS
//#define OPENGL_ES 1
#endif

#ifdef AMD
#define OPENGL_ES 1
#endif

#ifdef EMSCRIPTEN
#define OPENGL_ES 1
#endif

#ifdef ANDROID
#define OPENGL_ES 1
#endif


//#define WAYLAND 1

#ifdef WAYLAND
//#define OPENGL_ES 1
#endif


//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_image.h>
//#include <sys/soundcard.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
#include <iostream>
#include "Pieces.hh"
#include "Map.hh"
#include "Effect.hh"
#include "Midplay.hh"
#include <fstream>
//#include <GL/glut.h>
//extern "C" void __gxx_personality_v0() { }
#include "Language.hh"
#include "VolumeObjects.hh"
#include "Tree.hh"
#include "Widgets.hh"
#include "Category.hh"
#include "Coordinates.hh"
#include "Shader.hh"
#include "Triangle.hh"
#include "Game.hh"
#include "Editor.hh"
#include "Equalizer.hh"
#include "Web.hh"
#include "Functions.hh"
#include "ShaderExpr.hh"
#include "Font.hh"
#include "FontEditor.hh"
#include "GameApi.hh"
#include "KeyFrameEditor.hh"
#include "Parser.hh"

#include "GameApi_low.hh"

#ifdef WAYLAND
#include <wayland-client.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"
#include "stb_image_write.h"


struct wl_display;
struct wl_surface;
struct wl_shell_surface;

wl_display *g_wl_display = 0; 
wl_surface *g_wl_surface = 0;
wl_shell_surface *g_wl_shell_surface = 0;

extern bool g_vr_enable;
//#pragma comment (lib, "glew32s.lib") 

BufferRef CopyFromSDLSurface(Low_SDL_Surface *surf);
GLTFModelInterface *find_gltf(GameApi::Env &e, GameApi::TF tf);


IMPORT extern Low_SDL_Window *sdl_window;



//void *__dso_handle=NULL;
class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;
    
    //The ticks stored when the timer was paused
    int pausedTicks;
    
    //The timer status
    bool paused;
    bool started;
    
    public:
    //Initializes variables
    Timer();
    
    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();
    
    //Gets the timer's time
    int get_ticks();
    
    //Checks the status of the timer
    bool is_started();
    bool is_paused();    
};
Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;    
}

void Timer::start()
{
    //Start the timer
    started = true;
    
    //Unpause the timer
    paused = false;
    
    //Get the current clock time
    //TODO    startTicks = SDL_GetTicks();    
}

void Timer::stop()
{
    //Stop the timer
    started = false;
    
    //Unpause the timer
    paused = false;    
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;
    
        //Calculate the paused ticks
	//        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;
    
        //Reset the starting ticks
	//        startTicks = SDL_GetTicks() - pausedTicks;
        
        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
	  //            return SDL_GetTicks() - startTicks;
        }    
    }
    
    //If the timer isn't running
    return 0;    
}

bool Timer::is_started()
{
    return started;    
}

bool Timer::is_paused()
{
    return paused;    
}
bool exit2 = false;
//GLfloat light_position[] = { -1.0, 0.5, 6.0, 0.0 };
#if 0
GLfloat light_position[] = { 1.0, 1.0, 0.0, -200.0 };
GLfloat light_position2[] = { -0.0, -200.0, 0.0, 0.0 };

GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 20.0 };
#endif
//static bool lock1=false;
//static bool lock2=false;
//static bool exit_process=false;
#ifdef MUSIC
void *audio(void *arg)
{
  int audio_fd,i;
  audio_fd = open("/dev/dsp", O_WRONLY, 0);
  i=AFMT_S16_LE;ioctl(audio_fd,SNDCTL_DSP_SETFMT,&i);
  i=2; ioctl(audio_fd,SNDCTL_DSP_CHANNELS,&i);
  i=44100; ioctl(audio_fd,SNDCTL_DSP_SPEED,&i);

  {
  std::ifstream f1("m8.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m4.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m2.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  int size1 = end_pos-begin_pos;
  f1.seekg(0, std::ios_base::beg);
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m1.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  int size1 = end_pos-begin_pos;
  f1.seekg(0, std::ios_base::beg);
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m5.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  int size1 = end_pos-begin_pos;
  f1.seekg(0, std::ios_base::beg);
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m6.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m7.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m8.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  {
  std::ifstream f1("m9.wav");
  f1.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f1.tellg();
  f1.seekg(0, std::ios_base::end);
  std::ifstream::pos_type end_pos = f1.tellg();
  f1.seekg(0, std::ios_base::beg);
  int size1 = end_pos-begin_pos;
  char *audio_buffer = new char[size1];
  f1.read((char*)audio_buffer, size1);
  for(int i=0;i<size1&&!exit2;i+=100)
    write(audio_fd,audio_buffer+i,100);
  lock1=true;
  delete [] audio_buffer;
  }

  //pthread_exit(0);
  return 0;

}
#endif


Low_SDL_GLContext g_context;

extern std::string g_gpu_vendor;

IMPORT int g_display_width = 1200;
IMPORT int g_display_height = 1024;


void initialize_low(int flags);

IMPORT void check_vr_compositor_init();
IMPORT void    check_vr_overlay_init();


void *thread_func_bitmap(void *data)
{
#ifdef THREADS
  ThreadInfo_bitmap *ti = (ThreadInfo_bitmap*)data;
  ti->buffer->Gen(ti->start_x, ti->end_x, ti->start_y, ti->end_y);
  //pthread_exit(NULL);
  return 0;
#endif
}

extern void *(*g_thread_func_bitmap)(void *data);

Low_SDL_Surface *InitSDL2(int scr_x, int scr_y, bool vblank, bool antialias, bool resize, bool vr_init)
{
  
#ifdef THREADS
  g_thread_func_bitmap=&thread_func_bitmap;
#endif
  
  initialize_low(0);

#ifdef EMSCRIPTEN
  EmscriptenWebGLContextAttributes attr;
  emscripten_webgl_init_context_attributes(&attr);
  attr.majorVersion = 3; attr.minorVersion = 0;
  //attr.antialias=false;
  
  
  attr.enableExtensionsByDefault = false;


  
  //attr.desynchronized = true;
  //attr.premultipliedAlpha = true;
  //attr.antialias = true;
  //attr.depth = true;
  //attr.alpha = true;
  
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
  emscripten_webgl_make_context_current(ctx);
#endif
  
  
#ifdef SDL2_USED
  int screenx = scr_x, screeny = scr_y;


  //g_low->sdl->SDL_SetHint("SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS","1");
  g_low->sdl->SDL_Init(Low_SDL_INIT_VIDEO_NOPARACHUTE_JOYSTICK);

  Low_SDL_DisplayMode current;
  g_low->sdl->SDL_GetCurrentDisplayMode(0, &current);
  g_display_width = current.w;
  g_display_height = current.h;
  std::cout << "DISPLAY SIZE:" << g_display_width << " " << g_display_height << std::endl;
  
  if (screenx ==-1) { scr_x = g_display_width; screenx = g_display_width; }
  if (screeny ==-1) { scr_y = g_display_height; screeny = g_display_height; }
  
    std::cout << g_low->sdl->SDL_GetError() << std::endl;
    //std::cout << "NumJoysticks:"  << g_low->sdl->SDL_NumJoysticks() << std::endl;

    //   std::cout << "TEST:" << std::endl;

    
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_RED_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_GREEN_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_BLUE_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_ALPHA_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_DEPTH_SIZE, 24);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_DOUBLEBUFFER, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_STENCIL_SIZE, 1);
#ifdef WAYLAND
  
  if (getenv("XDG_SESSION_TYPE") && std::string(getenv("XDG_SESSION_TYPE"))=="wayland")
    {
      g_low->sdl->SDL_SetHint("SDL_VIDEODRIVER", "wayland");
    }
  
#endif

#ifdef BD_CALVIN_COMPUTER
  std::cout << "Choosing BD_CALVIN computer setup" << std::endl;
  g_low->sdl->SDL_SetHint("SDL_VIDEODRIVER", "wayland");
  g_low->sdl->SDL_SetHint("SDL_OPENGL_ES_DRIVER", "1");
#else

#ifndef LINUX
#ifndef EMSCRIPTEN
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLEBUFFERS, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLESAMPLES, 32);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_FLAGS, Low_SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
#endif
#ifndef EMSCRIPTEN
#ifdef LINUX
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLEBUFFERS, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLESAMPLES, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_FLAGS, Low_SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
#endif
#ifdef EMSCRIPTEN
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLEBUFFERS, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_MULTISAMPLESAMPLES, 8);

#endif
  
  
#ifndef EMSCRIPTEN
#ifndef LINUX
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
#endif
#endif
#endif
  //SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4);

#ifndef ANDROID
#if !defined(OPENGL_ES) && !defined(EMSCRIPTEN) && !defined(BD_CALVIN_COMPUTER)
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MINOR_VERSION, 3);

  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_PROFILE_MASK, Low_SDL_GL_CONTEXT_PROFILE_CORE);
#else 
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MAJOR_VERSION, 2); // 2
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MINOR_VERSION, 0); // 0

  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_PROFILE_MASK, Low_SDL_GL_CONTEXT_PROFILE_ES);

#endif
#else
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MAJOR_VERSION, 2); // 2
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MINOR_VERSION, 0); // 0

  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_PROFILE_MASK, Low_SDL_GL_CONTEXT_PROFILE_ES);
#endif
  
  //g_low->sdl->SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#ifdef VIRTUAL_REALITY
  if (vr_init||g_vr_enable) {
    check_vr_compositor_init();
  }
#endif  
#ifdef VIRTUAL_REALITY_OVERLAY
  //if (vr_init) {
    check_vr_overlay_init();
    //}
#endif

    //scr_x = 800; scr_y=600;
    //std::cout << "Trying to create window" << scr_x << "," << scr_y << std::endl;
    
  if (resize)
    sdl_window = g_low->sdl->SDL_CreateWindow("Program", Low_SDL_WINDOWPOS_CENTERED, Low_SDL_WINDOWPOS_CENTERED, scr_x, scr_y, Low_SDL_WINDOW_OPENGL_SHOWN_RESIZEABLE);
  else
    sdl_window = g_low->sdl->SDL_CreateWindow("Program", Low_SDL_WINDOWPOS_CENTERED, Low_SDL_WINDOWPOS_CENTERED, scr_x, scr_y, Low_SDL_WINDOW_OPENGL_SHOWN);

  //std::cout << "window created!" << std::endl;
  std::cout << g_low->sdl->SDL_GetError() << std::endl;

  //std::cout << sdl_window << " " << sdl_window->ptr << std::endl;

  //std::cout << "Trying to create context" << std::endl;
  g_context = g_low->sdl->SDL_GL_CreateContext(sdl_window);
  if (!g_context) { 
    std::cout << "Could not create Opengl3.2 context" << std::endl; 
    std::cout << g_low->sdl->SDL_GetError() << std::endl;
  }

  
  //std::cout << "context created" << std::endl;

  //g_low->ogl->glEnable(Low_GL_MULTISAMPLE);
  
#ifdef WAYLAND
  Low_SDL_SysWMinfo info;
  if (g_low->sdl->SDL_GetWindowWMInfo(sdl_window, &info))
    {
      g_wl_display = info.display;
      g_wl_surface = info.surface;
      g_wl_shell_surface = info.shell_surface;
    }
#endif
  
#ifdef USE_GLEW
  glewExperimental=true;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      std::cout << "Glew init failed!" << std::endl;
      std::cout << "ERROR:" << err << std::endl;
      //return 0;
    }

#endif
  OpenglLowApi *ogl = g_low->ogl;
  ogl->init();

#ifdef ANDROID
  g_gpu_vendor = std::string("ANDROID");
#endif
  
#ifndef ANDROID
  const unsigned char *ptr = ogl->glGetString(Low_GL_VENDOR);
  if (ptr && strlen((const char*)ptr)>2) {
    g_gpu_vendor = std::string(ptr,ptr+3);
  }
  if (ptr && strlen((const char*)ptr)>3) {
  g_gpu_vendor = std::string(ptr,ptr+4);
  }
  std::cout << "GPU Vendor: " << ogl->glGetString(Low_GL_VENDOR) << "(" << g_gpu_vendor << ")" << std::endl;
  std::cout << "GPU Renderer:" << ogl->glGetString(Low_GL_RENDERER)<< std::endl;
  std::cout << "GPU Version:" << ogl->glGetString(Low_GL_VERSION) << std::endl;
#endif  

  vblank = true;
#ifdef EMSCRIPTEN
  g_low->sdl->SDL_GL_SetSwapInterval(0);
#endif
  //#ifdef LINUX
  // g_low->sdl->SDL_GL_SetSwapInterval(0);
  //#endif
  
#if 0
  if (vblank)
    {
      int (APIENTRY *SwapInterval)(int);
      SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("glXSwapInterval");
      if (!SwapInterval)
	SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("wglSwapIntervalEXT");
      if (!SwapInterval)
	SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("glXSwapIntervalEXT");
      if (!SwapInterval)
	SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("glXSwapIntervalSGI");
      if (!SwapInterval)
	SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("wglSwapInterval");
      if (!SwapInterval)
	SwapInterval = (int(APIENTRY *)(int))g_low->sdl->SDL_GL_GetProcAddress("wglSwapIntervalSGI");
      // actual vsync activation
      if (SwapInterval)
	SwapInterval(0);
      else
	std::cout << "Cannot get VBLANK" << std::endl;
      //SDL_GL_SetSwapInterval(int interval);
    }
#endif
#ifdef WINDOWS
   ogl->glEnable(Low_GL_DEPTH_TEST);
   //ogl->glDepthMask(Low_GL_FALSE);
  ogl->glDepthFunc(Low_GL_LEQUAL);
  ogl->glEnable(Low_GL_BLEND);
#endif
#ifdef LINUX
   ogl->glEnable(Low_GL_DEPTH_TEST);
   //ogl->glDepthMask(Low_GL_FALSE);
  ogl->glDepthFunc(Low_GL_LEQUAL);
  ogl->glEnable(Low_GL_BLEND);
#endif
#ifdef EMSCRIPTEN
   ogl->glEnable(Low_GL_DEPTH_TEST);
   //ogl->glDepthMask(Low_GL_FALSE); // this does disable writing to depth buffer, which isn't right
   ogl->glDepthFunc(Low_GL_LEQUAL);
  ogl->glEnable(Low_GL_BLEND);
#endif
  
  ogl->glClearColor( 0, 0, 0, 0 );
  ogl->glViewport(0,0,screenx, screeny);
  ogl->glDisable(Low_GL_CULL_FACE);
#endif
  return 0;
}

Low_SDL_Surface *InitSDL(int scr_x, int scr_y, bool vblank, bool antialias)
{
  Low_SDL_Surface *screen = 0;
#ifndef SDL2_USED
  int screenx = scr_x, screeny = scr_y;
  //screenx = 1024;
  //screeny = 768;

  //screenx = 320;
  //screeny = 200;
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE|SDL_INIT_JOYSTICK|SDL_INIT_AUDIO);
  //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
  screen = SDL_SetVideoMode(screenx,screeny,32,SDL_OPENGL | SDL_DOUBLEBUF|SDL_RESIZABLE/*|SDL_FULLSCREEN*/);
  SDL_ShowCursor(SDL_DISABLE);
  //glutInit(&argc, argv);
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      std::cout << "Glew init failed!" << std::endl;
      return 0;
    }
#if 0
  if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader &&
      GL_EXT_geometry_shader4)
    {
      std::cout << "Shaders OK" << std::endl;
    }
  else
    {
      std::cout << "NO SHADERS" << std::endl;
      exit(1);
    }
#endif

  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  Rect scr;
  scr.tl.x = 0;
  scr.tl.y = 0;
  scr.br.x = screenx;
  scr.br.y = screeny;

  vblank = false;
  if (vblank)
    {
      int (*SwapInterval)(int);
      SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("glXSwapInterval");
      if (!SwapInterval)
	SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("wglSwapIntervalEXT");
      if (!SwapInterval)
	SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("glXSwapIntervalEXT");
      if (!SwapInterval)
	SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("glXSwapIntervalSGI");
      if (!SwapInterval)
	SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("wglSwapInterval");
      if (!SwapInterval)
	SwapInterval = (int(*)(int))SDL_GL_GetProcAddress("wglSwapIntervalSGI");
      // actual vsync activation
      if (SwapInterval)
	SwapInterval(1);
      else
	std::cout << "Cannot get VBLANK" << std::endl;
      //SDL_GL_SetSwapInterval(int interval);
    }

  //if (antialias)
  //  {
  //    glEnable(GL_BLEND);
  //    glEnable(GL_POLYGON_SMOOTH);
  //    glDisable(GL_DEPTH_TEST);
  //  }
  //else
  //  {
  //    glDisable(GL_BLEND);
  //    glDisable(GL_POLYGON_SMOOTH);
  //    glEnable(GL_DEPTH_TEST);
  //  }
  glEnable(GL_DEPTH_TEST);
  glEnable ( GL_NORMALIZE );
  glDepthMask(GL_TRUE);
#ifndef EMSCRIPTEN
  glShadeModel(GL_SMOOTH);
#endif
  //glEnable(GL_LIGHTING);

  //glEnable(GL_POLYGON_SMOOTH);
  //glEnable(GL_CULL_FACE);
  //glMaterialf( GL_FRONT_AND_BACK,
  //	       GL_SHININESS, 0.8);
  // glColorMaterial ( GL_FRONT_AND_BACK, GL_EMISSION ) ;
   glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  //glEnable(GL_MULTISAMPLE_ARB);
  

  //glEnable(GL_LIGHT1);
  //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
  //glEnable(GL_AUTONORMALS);
  glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
  glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
  glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
  glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess );
  //glFrontFace(0 ? GL_CCW : GL_CW);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, mat_diffuse);
  //glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
  glClearColor( 0, 0, 0, 0 );
  glViewport(0,0,screenx, screeny);
#ifndef EMSCRIPTEN
  glMatrixMode( GL_PROJECTION ); 
  glLoadIdentity(); 
#endif
  //glOrtho( -screenx, screenx, screeny, -screeny, -1000, 1000 ); 
  //double r = 0.05;
  //double ks = (double)screenx/screeny;
  //glFrustum( -r*ks, r*ks, -r, r, 0.1, 100.0 );
  //gluLookAt(0.0, 0.0, -100.0, // eye
  //    0.0, 0.0, 0.0,  // center
  //	    0.0, -100.0, 0.0);

  // ORIGINAL
  //gluPerspective(80.0, (double)screenx/screeny, 10.1, 6000.0);
  Matrix m = Matrix::Perspective(80.0, (double)screenx/screeny, 10.1, 1600.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#ifndef EMSCRIPTEN
  glMultMatrixf(&mat[0]);

  glMatrixMode( GL_MODELVIEW ); 
  glLoadIdentity();
#endif
#endif

  return screen;
}


void GameApiTest(GameApi::EveryApi &e);
void GameApiTest2(GameApi::EveryApi &e);
void GameApiTest3(GameApi::EveryApi &e);
void Game(GameApi::EveryApi &e);
//void GameApiWorldFunc(GameApi::EveryApi &e);

//extern "C" void _start()
int mainxxx(int argc, char *argv[])
{
#if 0
#if 0
  if (argc>1 && (std::string(argv[1])=="-h" || std::string(argv[1])=="--help"))
    {
      std::cout << "./main [-h] [-edit filename] [-test3]" << std::endl;
      exit(0);
    }
  if (argc>1 && std::string(argv[1])=="-world")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameApiWorldFunc);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  if (argc>1 && std::string(argv[1])=="-test4")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &Game);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  if (argc>1 && std::string(argv[1])=="-test3")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameApiTest3);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }

  if (argc>2 && std::string(argv[1])=="-edit")
    {
      SphereElem elem(Point(0.0,0.0,0.0), 100.0, 10,10);

      FaceCollectionParser parser(0);
      std::string s = parser.Create(&elem);
      std::cout << s << std::endl;
      bool success = false;
      FaceCollection *coll = parser.Parse(s,success);
      std::cout << "Succes:" << success << std::endl;
      /*
      CubeElem elem(Point(-100.0,-100.0,-100.0),
		    Point(-100.0,-100.0,100.0),
		    Point(-100.0,100.0,-100.0),
		    Point(-100.0,100.0,100.0),
		    Point(100.0,-100.0,-100.0),
		    Point(100.0,-100.0,100.0),
		    Point(100.0,100.0,-100.0),
		    Point(100.0,100.0,100.0));
      */
      ShowDrawingArea_polys(argc,argv,coll);
      exit(0);
#endif
    }

  //FontParseTestCases();
  //return 0;
#if 0
  GameApi::Games g;
  g.register_game(0, &GameApiTest);
  g.modify_map(0,0);
  g.start_game(0);
  return 0;
#endif
#if 0
  GameApi::Env e;
  GameApi::GamesApi g(e);
  g.register_game(0, &GameApiTest2);
  g.modify_map(0,0);
  g.start_game(0);
  return 0;
#endif

  //testshader();
  //return 0;
  //testlang();
  //return 0;
  //CreateHtml();
  //return 0;

#if 0
  DynamicFont::Data d;
  d.character = (int)'a';
  d.ch.AddLoop();
  Point2d p = { 10.0, 5.0 };
  Point2d p2 = { 5.0, 10.0 };
  Point2d p3 = { 20.0,20.0 };
  Point2d p4 = { 14.0,20.0 };
  Point2d p5 = { 20.0,15.0 };
  d.ch.AddPoint(0, p);
  d.ch.AddPoint(0, p2);
  d.ch.AddPoint(0, p3);
  d.ch.AddPoint(0, p4);
  d.ch.AddPoint(0, p5);
  d.ch.SetAdvance(30.0);
  Point2d pp = { 0.0, 0.0 };
  Point2d pp2 = { 30.0, 30.0 };
  d.ch.SetTL(pp);
  d.ch.SetBR(pp2);
  DynamicFont font;
  font.height = 30.0;
  font.chars.push_back(d);
  ExecuteFontEditor(argc, argv, &font);
  return 0;
#endif
  //CreationFunctionArray *array = creation_array();
  //ExpressionTree<ExpressionNode> *tree = create_expr_tree_node(array->Index(0));
  ////tree->children.push_back(create_expr_tree_node(array->Index(0)));

  //ExecuteGtkmm(argc, argv, tree);
  //return 0;
  srand(0);
#if 0
  std::string s = "123,1";
  Integer lang;
  //StringRef ss(s);
  //bool b = lang.Match(ss);
  CharLanguage ch(',');
  ArrayLanguage arr(lang, ch);
  bool b = arr.Match(s);
  std::cout << b << std::endl;
  int i = arr.MatchResultCount();
  std::cout << i << std::endl;
  for (int ii=0;ii<i;ii++)
    std::cout << arr.MatchResult(ii) << " ";
  std::cout << std::endl;

  return 0;
#endif
  /*
  Matrix am = Matrix::XRotation(2.0*3.14159/16.0);
  Matrix im = Matrix::Inverse(am);
  Matrix mm = im*am;
  for(int y=0;y<4;y++)
    {
    for(int x=0;x<4;x++)
      std::cout << mm.matrix[x+y*4] << " ";
    std::cout << std::endl;
    }
  return 0;
  */
  //Map m;
  //m.Load("level0.txt");
#ifdef MUSIC  
  std::cout << "Please wait while loading" <<  std::flush;
  {
  char c;
  std::ifstream f1("m1.wav");
  while(f1>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f2("m2.wav");
  while(f2>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f3("m3.wav");
  while(f3>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f4("m4.wav");
  while(f4>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f5("m5.wav");
  while(f5>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f6("m6.wav");
  while(f6>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f7("m7.wav");
  while(f7>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f8("m8.wav");
  while(f8>>c);
  std::cout << "." <<  std::flush;
  std::ifstream f9("m9.wav");
  while(f9>>c);
  std::cout << "." <<  std::endl;
  }

  //MidFileLoader load("357.mid");
  //std::vector<Event> events = load.GetEvents(1);

  //pthread_t thread;
  g_pthread_count++;
  //pthread_create(&thread, 0 ,&audio, 0);
  tasks_add(3012,&audio,(void*)0);
#endif
  int screenx, screeny;
  screenx = 1024;
  screeny = 768;
  InitSDL(screenx, screeny, true); // true = vsync

  Low_SDL_Surface *screen; // = SDL_GetVideoSurface();


  ShaderFile shader("./Shader.txt");
  SDLRender rr;
  SDLArrayRender rr2;
  Render *rend = &rr;
  {
    GridEffect2 g(rend);
    Execute(g,screen);

    //ExecuteEffect(tree, screen);
    //ArrayEffect e(rend);
    //PreCalcExecute(rend, e, 100.0, 100);
    //Execute(e, screen);
    //rend.Show();
  }
  {
    FunctionPointerFrameAnim fa(rend, &chessboard_test);
    Execute(fa, screen);
  }
  {
    FuncEffect fe(rend);
    //PreCalcExecute(*rend, fe, 1000.0, 100);
    Execute(fe, screen);
    //rend->Show();
  }

  {
    //CubeAnim a;
    IntersectAnim a;
    AnimFrameAnim afa(a,shader,100.0);
    Execute(afa, screen);
  }

  {
    Position2d p; // this is modified by MovePointEventSurface, and used by GameEffect
    float speed = 15.0;
    Point2d tl = { float(-screenx/2.0), float(-screeny/2.0) };
    Point2d br = { float(screenx/2.0), float(screeny/2.0) };
    MovePointEventSurface mp(p, tl, br, speed);
    //DecodahedronEffect deco;
    //GraphEffect ge;
    GameEffect game(p,rend);
    Execute2(game,mp);
  }
  {
    //DecodahedronEffect deco(rend);
    //Execute2(deco, deco);
  }

  {
    //TriangulateEffect trie(rend);
    //Execute(trie, screen);
  }
  {
    FractalEffect frac(rend);
    //PreCalcExecute(*rend, frac, 100.0, 100);
    //Execute(e, screen);
    //rend->Show();
    Execute(frac, screen);
  }

  {
    Effect3dEffect e3de(shader, rend);
    Execute(e3de, screen);
  }

  {
    Effect2dEffect e2de(shader, rend);
    Execute(e2de, screen);
  }

  {
    ShaderEffect se(shader, rend);
    Execute(se, screen);
  }

  {
    TransformEffect trans(rend);
    Execute(trans, screen);
  }
  {
    GraphEffect ge(rend);
    Execute(ge, screen);
  }
  {
    InterpolateMovementEffect eff(rend);
    Execute(eff, screen);
  }

  {
    RenderToTextureEffect rend2(rend);
    Execute(rend2, screen);
  }

  {
    SuperShapeEffect effect(rend);
    Execute(effect, screen);
  }

  {
    MapEffect mapper(rend);
    Execute(mapper, screen);
  }

  {
    CategoryEffect cat(rend);
    Execute(cat, screen);
    
  }

  //{
  //  ManyCubesEffect cubes;
  //  Execute(cubes, screen);
  //}
  {
#if 1
    SinWaveform wave(3.14159*2.0, 1.0);
    FitWaveform fit(wave, 100.0, -0.5, 0.5);
    SampleWaveform initial_val_array(fit, 20);
    //FunctionArray<int, float> initial_val_array(2.0*3.14159, func);
    
    Range<float> range;
    range.start = wave.Min();
    range.end = wave.Max();
    CopyArray<float> vectorarray(initial_val_array);
    ArrayDataType<float> dt(vectorarray, range);
    BufferRef buffer = BufferRef::NewBuffer(screenx/8, screeny/8);
    DataTypeTextureEvents events(dt, screenx/8, screeny/8);
    TextureWidget widget(buffer, events, screenx, screeny, rend);
    RotateFrameAnim fwidget(widget, rend);
    Execute2(fwidget, widget);
#endif

    
#if 0
    Rect r;
    r.tl.x = 0.0;
    r.tl.y = 0.0;
    r.br.x = 1.0;
    r.br.y = 1.0;

    VectorArray<Point2d> vectorarray;
    Point2dArrayDataType dt(vectorarray, r);
    BufferRef buffer = BufferRef::NewBuffer(screenx/8, screeny/8);
    DataTypeTextureEvents events(dt, screenx/8, screeny/8);
    TextureWidget widget(buffer, events, screenx, screeny);
    RotateFrameAnim fwidget(widget);
    Execute2(fwidget, widget);
#endif
  }
#if 0
  {
    TunnelEffect2 te2;
    RingTreeEffect ring;
    RotateFrameAnim rte2(te2);
    RotateFrameAnim rring(ring);
    RectFrameAnim te2r(scr.SplitX().first, rte2, scr);
    RectFrameAnim ringr(scr.SplitX().second, rring, scr);
    FrameAnim *array[] = { &te2r, &ringr };
    VectorFrameAnim vec(array, array+2);
    Execute2(vec);
  }
  { 
    TunnelEffect2 te2;
    Execute(te2, screen);
  }
#endif
 {
   VoxelEffect ve(rend);
   Execute(ve, screen);
 }
#if 0
  {
    WalkCubeEffect wce;
    Execute(wce, screen);
  }
#endif
  {
    RingTreeEffect ring(rend);
    Execute(ring, screen);
   }
#if 0
  {
    SinusEffect sin(rend);
    Execute(sin, screen);
  }
#endif
#if 0
  {
    MovementEffect mov;
    Execute(mov, screen);
  }
#endif
  {
    VolumeEffect vol(rend);
    Execute(vol, screen);
  }

  {
    RingEffect ring(rend);
    Execute(ring, screen);
  }

  {
    SurfaceEffect surf(rend);
    Execute(surf, screen);
  }

  {
    BlobEffect blobe(rend);
    Execute(blobe, screen);
  }

  {
    RoboEffect re(rend);
  Execute(re, screen);
   }
  {
    CircleEffect cie(rend);
    //PreCalcExecute(*rend, cie, 1000.0, 100);
    //rend->Show();
    Execute(cie, screen);
   }
  {
    VBOEffect vboe(rend);
    //PreCalcExecute(*rend, vboe, 1000.0,100);
    //rend->Show();
    Execute(vboe, screen);
  }
  //PathEffect pe;
  //Execute(pe, screen);
  {
    MoleculeEffect me(rend);
  Execute(me, screen);
  }
  {
    HeightMapEffect hme(rend);
  Execute(hme, screen);
  }
  {
    TunnelEffect te(rend);
  Execute(te, screen);
  }
  {
    MoleculeInsideEffect mie(rend);
  Execute(mie, screen);
  }
  //TestEffect te2;
  //Execute(te2, screen);
  //TitleEffect te3;
  //Execute(te3, screen);
  {
    SphereEffect se(rend);
  Execute(se, screen);
  }
  {
    CylinderEffect ce3(rend);
  Execute(ce3, screen);
  }
  {
    CubeEffect ce(rend);
  Execute(ce, screen);  
  }
  {
    CubeEffect ce2(rend);
    //SDL_Surface *scr = SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 768, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  SaveFrameAnimToFile(ce2, screen, "./output/cube.ppm", 0.1, 0.3);
  }
  SDL_Quit();
  exit2 = true;
  /*asm ( \
    //  "movl $1,%eax\n" \
    // "xor %ebx,%ebx\n" \
    //"int $128\n" \
    //);*/
#endif
  return 0;
}
void Execute2(FrameAnim &f)
{
  EmptyEventSurface s;
  Execute2(f,s);
}
void Execute2(FrameAnim &f, EventSurface &surf)
{
#if 0
  SDL_Event event;
  SDL_Event prev;

  //int x=0;
  //int tick;
 
  Timer fps;
  Timer update;
  int frame = 0;
  f.Init();
  //Start the update timer 
  update.start(); 
  //Start the frame timer 
  fps.start();
  glColor3f(1.0,1.0,1.0);
  bool exita = false;
  int state = 0;
  SDL_ShowCursor(true);
  do
  {
    //tick = SDL_GetTicks();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //x=int(fps.get_ticks()/30.0);
  //SawWaveform w;
  //FitWaveform fit(w, 150.0, 0.0, 150.0);
  //RepeatWaveform rep(fit, 50);
  //x = rep.Index(x)+x;
  f.PreFrame(fps.get_ticks()/30.0);
  surf.HandleFrame(fps.get_ticks()/30.0);
  f.Frame(fps.get_ticks()/30.0);
  exita=lock1;
  f.PostFrame();
  glLoadIdentity();

  //SDL_GL_SwapBuffers();
  //SDL_Flip(screen);
  frame++;

 //If a second has passed since the caption was last updated 
  if( update.get_ticks() > 1000 ) { //The frame rate as a 
    //Calculate the frames per second and create the 
    std::cout << "FPS: " << frame / ( fps.get_ticks() / 1000.f )<< ", " << frame << std::endl;
    //Reset the caption 

    //SDL_WM_SetCaption( caption.str().c_str(), NULL ); 
    //Restart the update timer 
    update.start();
  
  }
    SDL_PollEvent(&event);
    //if (event.type==prev.type && event.key.keysym.sym == prev.key.keysym.sym &&
    //event.button.button == prev.button.button)
    //{
    //continue;
	//}
    prev = event;

    if (event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && state == 0)
      {
	Point2d p;
	p.x = event.button.x;
	p.y = event.button.y;
	surf.HandlePointerMove(p);
	surf.HandlePointer(0);
      }
    if (event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT && state==0)
      {
	Point2d p;
	p.x = event.button.x;
	p.y = event.button.y;
	surf.HandlePointerMove(p);
	surf.HandlePointer(1);
	state++;
      }
    if (event.type==SDL_KEYDOWN)
      {
	surf.HandleKeyDown(event.key.keysym.sym);
	surf.HandleKey(event.key.keysym.sym);
      }
    if (event.type==SDL_MOUSEMOTION)
      {
	Point2d p;
	p.x = event.button.x;
	p.y = event.button.y;
	surf.HandlePointerMove(p);
      }
    if (event.type==SDL_KEYUP) 
      {  
	surf.HandleKeyUp(event.key.keysym.sym);
      }
    if (event.type==SDL_QUIT || (event.type==SDL_KEYDOWN && event.key.keysym.sym==27)) { SDL_Quit(); exit(0); }
    if (event.type==SDL_KEYDOWN && event.key.keysym.sym=='p') 
      { 
	for(;;) {
	  SDL_PollEvent(&event);
	  if (event.type==SDL_KEYDOWN && event.key.keysym.sym=='o')
	    break;
	}
      }
  } while (!(event.type==SDL_KEYDOWN && event.key.keysym.sym == ' ') && !exita);
  do {
    SDL_PollEvent(&event);
  } while(event.type!=SDL_KEYUP && !exita);

  f.Cleanup();
  lock1=false;
#endif
}
//void ExecuteRend(FrameAnim &f, SDL_Surface *screen, Render *rend)
//{
//  rend->Execute(f,screen);
//}

void Execute(FrameAnim &f, Low_SDL_Surface *screen)
{
#if 0
  SDL_Event event;
  int x=0;
  //int tick;
 
  Timer fps;
  Timer update;
  int frame = 0;
  f.Init();
  //Start the update timer 
  update.start(); 
  //Start the frame timer 
  fps.start();
  glColor3f(1.0,1.0,1.0);
  bool exita = false;
  do
  {
    //tick = SDL_GetTicks();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  x=int(fps.get_ticks()/30.0);
  //SawWaveform w;
  //FitWaveform fit(w, 150.0, 0.0, 150.0);
  //RepeatWaveform rep(fit, 50);
  //x = rep.Index(x)+x;

  f.PreFrame(fps.get_ticks()/30.0);
#ifndef EMSCRIPTEN
  glTranslatef(0.0, 0.0, -500.0);
#endif
  float speed = f.RotSpeed();
  glRotatef(speed*x, f.XRot(),f.YRot(),f.ZRot());
#ifndef EMSCRIPTEN

  glTranslatef(0.0, -100.0, 0.0);
#endif
  f.Frame(fps.get_ticks()/30.0);
  exita=lock1;
  f.PostFrame();
  glLoadIdentity();

  //SDL_GL_SwapBuffers();
  //SDL_Flip(screen);
  frame++;

 //If a second has passed since the caption was last updated 
  if( update.get_ticks() > 1000 ) { //The frame rate as a 
    //Calculate the frames per second and create the 
    std::cout << "FPS: " << frame / ( fps.get_ticks() / 1000.f )<< ", " << frame << std::endl;
    //Reset the caption 

    //SDL_WM_SetCaption( caption.str().c_str(), NULL ); 
    //Restart the update timer 
    update.start();
  
  }
    SDL_PollEvent(&event);
    if (event.type==SDL_QUIT || (event.type==SDL_KEYDOWN && event.key.keysym.sym==27)) { SDL_Quit(); exit(0); }
    if (event.type==SDL_KEYDOWN && event.key.keysym.sym=='p') 
      { 
	for(;;) {
	  SDL_PollEvent(&event);
	  if (event.type==SDL_KEYDOWN && event.key.keysym.sym=='o')
	    break;
	}
      }
  } while (!(event.type==SDL_KEYDOWN && event.key.keysym.sym == ' ') && !exita);
  do {
    SDL_PollEvent(&event);
  } while(event.type!=SDL_KEYUP && !exita);

  f.Cleanup();
  lock1=false;
#endif
}

void InitFrameAnim(FrameAnim &f, Low_SDL_Surface *screen)
{
  //f.Init();
  //glColor3f(1.0,1.0,1.0);

}

void DisplayFrame(FrameAnim &f, Low_SDL_Surface *screen, float time)
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT );

  //SawWaveform w;
  //FitWaveform fit(w, 150.0, 0.0, 150.0);
  //RepeatWaveform rep(fit, 50);
  //x = rep.Index(x)+x;

  f.PreFrame(time/30.0);
#ifndef EMSCRIPTEN
  ogl->glTranslatef(0.0, 0.0, -500.0);
#endif
  float speed = f.RotSpeed();
  ogl->glRotatef(speed*time/30.0, f.XRot(),f.YRot(),f.ZRot());
#ifndef EMSCRIPTEN
  ogl->glTranslatef(0.0, -100.0, 0.0);
#endif
  f.Frame(time/30.0);
  f.PostFrame();
  ogl->glLoadIdentity();

  //SDL_GL_SwapBuffers();
}
void CleanupFrameAnim(FrameAnim &f, Low_SDL_Surface *surf)
{
  f.Cleanup();
}

void PreCalcExecute(Render &rend, FrameAnim &f, float duration, int numframes)
{
  f.Init();
  float time = 0.0;
  for(int i = 0; i<numframes; i++, time+=duration/numframes)
    {
      rend.SetTime(time);
      f.PreFrame(time/30.0);
      f.Frame(time/30.0);
      f.PostFrame();  
    }
}




GameApi::ASyncVec *g_convert(std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec);

void g_png_store_write_func(void *context, void *data, int size)
{
  std::string *write_buf = (std::string*)context;
  *write_buf = std::string((unsigned char*)data,((unsigned char*)data)+size);
}

Bitmap<Color> *find_bitmap2(GameApi::Env &e, GameApi::BM bm);


void GameApi::MainLoopApi::save_png_store(std::string output_filename, BM bm)
{
  std::stringstream ss;
  Bitmap<Color> *bbm = find_bitmap2(e,bm);
  bbm->Prepare();
  FlipColours flip(*bbm);
  BufferFromBitmap bm2(flip);
  bm2.Gen();
  BufferRef ref = bm2.Buffer();
  
  stbi_write_func *func = &g_png_store_write_func;
  std::string write_buf;
  void *context = (void*)&write_buf;
  void *data = ref.buffer;
  int stride = ref.ydelta*sizeof(unsigned int); // in bytes
  stbi_write_png_to_func(func, context, bbm->SizeX(), bbm->SizeY(), 4, data, stride);

  
  std::vector<unsigned char,GameApiAllocator<unsigned char> > vec(write_buf.begin(),write_buf.end());
  e.store_file(output_filename,g_convert(&vec));
}


#undef LoadImage
BufferRef LoadImageFromString(const std::vector<unsigned char> &buffer, bool &success)
{
  //std::vector<unsigned char> mem;
  //int s = buffer.size();
  //for(int i=0;i<s;i++) mem.push_back(buffer[i]);
  // std::cout << "LoadImageFromString:" << buffer.size() << std::endl;
  
  int x=0,y=0;
  int comp=0;
  stbi_uc * ptr = stbi_load_from_memory(&buffer[0], buffer.size(), &x, &y, &comp, 4);
  //std::cout << "ImageSize: " << x << " " << y << " " << comp << std::endl;
  int sz = x*y*sizeof(int);
  // this is required because stbi allocs it with malloc(), and we should use new.
  unsigned char *buf = new unsigned char[sz];
  std::copy(ptr,ptr+sz,buf);
  free(ptr);
  BufferRef ref;
  ref.buffer = (unsigned int *)buf;
  ref.width = x;
  ref.height = y;
  ref.ydelta = x;

#if 1
  if (comp==3||comp==4)
  for(int yy=0;yy<y;yy++)
    for(int xx=0;xx<x;xx++)
      {
	unsigned int val = ref.buffer[xx+yy*ref.ydelta];
	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b; 
	ref.buffer[xx+yy*ref.ydelta] = val;
      }
#endif
  success = true;
  return ref;
  

}
BufferRef LoadImage(std::string filename, bool &success)
{
  std::cout << "Loading: " << filename << std::endl;
  //SDL_Surface *surf = IMG_Load(filename.c_str()); 
  std::vector<unsigned char> mem;
  ifstream ss(filename.c_str(), ios_base::binary|ios_base::in);
  char c;
  while(ss.get(c))
    {
      mem.push_back((unsigned char)c);
    }
  std::cout << "FileSize: " << mem.size() << std::endl;
  if (mem.size()==0) { success=false;
      std::cout << "Cannot load " << filename << std::endl;
      BufferRef r;
      return r;
  }
  
  int x,y;
  int comp;
  stbi_uc * ptr = stbi_load_from_memory(&mem[0], mem.size(), &x, &y, &comp, 4);
  std::cout << "ImageSize: " << x << " " << y << " " << comp << std::endl;
  BufferRef ref;
  ref.buffer = (unsigned int *)ptr;
  ref.width = x;
  ref.height = y;
  ref.ydelta = x;
#if 1
  if (comp==3 ||comp==4)
  for(int yy=0;yy<y;yy++)
    for(int xx=0;xx<x;xx++)
      {
	unsigned int val = ref.buffer[xx+yy*ref.ydelta];
	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b;  
	ref.buffer[xx+yy*ref.ydelta] = val;
      }
#endif
#if 0
  //SDL_RWops *src = SDL_RWFromMem(&mem[0], mem.size());
  //SDL_Surface *surf = IMG_Load_RW(src, 0);
  if (!surf) 
    {
      success = false;
      std::cout << "Cannot load " << filename << std::endl;
      //char *err = IMG_GetError();
      //std::cout << "Error: " << err << std::endl;
      BufferRef r;
      return r;
    }
  //SDL_Surface *surf2 = SDL_DisplayFormat(surf);
  SDL_PixelFormat format;
  format.palette = 0;
  format.BitsPerPixel = 32;
  format.BytesPerPixel = 4;
  format.Rloss = 0;
  format.Gloss = 0;
  format.Bloss = 0;
  format.Aloss = 0;
  format.Rshift = 24;
  format.Gshift = 16;
  format.Bshift = 8;
  format.Ashift = 0;
  format.Rmask = 0x00ff0000;
  format.Gmask = 0x0000ff00;
  format.Bmask = 0x000000ff;
  format.Amask = 0xff000000;
  //format.colorkey = 0;
  //format.alpha = 255;
  Low_SDL_Surface *surf2 = g_low->sdl->SDL_ConvertSurface(surf, &format, 0);
  BufferRef ref = CopyFromSDLSurface(surf2);
  SDL_FreeSurface(surf);  
#endif
  //SDL_FreeSurface(surf2);  
  success = true;
  return ref;
}
BufferRef CopyFromSDLSurface(Low_SDL_Surface *surf)
{
  //std::cout << "Copy Size: " << surf->w << " " << surf->h << std::endl;
  BufferRef buf = BufferRef::NewBuffer(surf->w, surf->h);
  //std::cout << buf.buffer << " " << buf.width << " " << buf.height << " " << buf.ydelta << std::endl;
  g_low->sdl->SDL_LockSurface(surf);
  for(int y=0;y<surf->h;y++)
    for(int x=0;x<surf->w;x++)
      {
	unsigned int *ptr = (unsigned int*)surf->pixels;
	int color = ptr[(/*surf->format->BytesPerPixel**/ x+y*surf->pitch)/sizeof(unsigned int)];
	//std::cout << x << " " << y << " " << buf.ydelta << std::endl;
#if 0
	int r = (color & 0xff000000) >> 24 ;
	int g = (color & 0xff0000) >> 16;
	int b = (color & 0xff00) >> 8;
	int a = color & 0xff;
	//std::cout << r << " " << g << " " << b << " " << a << std::endl;
	int col = r + (g<<8) + (b << 16) + (a << 24);
#endif
	buf.buffer[x+buf.ydelta*y] = color;
      }
  g_low->sdl->SDL_UnlockSurface(surf);
  return buf;
}
struct savecontext 
{
  std::ofstream *stream;
  std::vector<unsigned char> *res;
};
void save_write_func(void *context, void *data, int size)
{
  savecontext *ctx = (savecontext*)context;
  (*ctx->stream).write((const char*)data, size);
}
void save_write_func2(void *context, void *data, int size)
{
  savecontext *ctx = (savecontext*)context;
  for(int i=0;i<size;i++)
    ctx->res->push_back(((const unsigned char*)data)[i]);
  //(*ctx->res) += std::string((const char*)data, ((const char *)data)+size);
  //(*ctx->stream).write((const char*)data, size);
}

void SaveImage(std::vector<unsigned char> &res, BufferRef ref)
{
  int x = ref.width;
  int y = ref.height;

  for(int yy=0;yy<y;yy++)
    for(int xx=0;xx<x;xx++)
      {
	unsigned int val = ref.buffer[xx+yy*ref.ydelta];
	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b;  
	ref.buffer[xx+yy*ref.ydelta] = val;
      }


  savecontext ctx;
  ctx.res = &res;
  //ctx.stream = new std::ofstream(filename.c_str(),ios_base::out|ios_base::binary);
  stbi_write_png_to_func(&save_write_func2, &ctx, ref.width, ref.height,4,ref.buffer,ref.ydelta*sizeof(unsigned int));

  //ctx.stream->close();
  //delete ctx.stream;
  //ctx.stream = 0;

}


void SaveImage(BufferRef ref, std::string filename)
{
  int x = ref.width;
  int y = ref.height;

  for(int yy=0;yy<y;yy++)
    for(int xx=0;xx<x;xx++)
      {
	unsigned int val = ref.buffer[xx+yy*ref.ydelta];
	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b;  
	ref.buffer[xx+yy*ref.ydelta] = val;
      }


  savecontext ctx;
  ctx.stream = new std::ofstream(filename.c_str(),ios_base::out|ios_base::binary);
  stbi_write_png_to_func(&save_write_func, &ctx, ref.width, ref.height,4,ref.buffer,ref.ydelta*sizeof(unsigned int));

  ctx.stream->close();
  delete ctx.stream;
  ctx.stream = 0;
}

Low_SDL_Window *sdl_framebuffer_window = 0;
Low_SDL_Surface *sdl_framebuffer = 0;

Low_SDL_Surface *init_sdl_surface_framebuffer(int scr_x, int scr_y)
{
#ifdef SDL2_USED
  g_low->sdl->SDL_Init(Low_SDL_INIT_VIDEO_NOPARACHUTE_JOYSTICK);
  //std::cout << "SDL_Init: " << g_low->sdl->SDL_GetError() << std::endl;
  sdl_framebuffer_window = g_low->sdl->SDL_CreateWindow("Framebuffer", Low_SDL_WINDOWPOS_CENTERED, Low_SDL_WINDOWPOS_CENTERED, scr_x, scr_y, Low_SDL_WINDOW_SHOWN);
  if (!sdl_framebuffer_window) {
    std::cout << "ERROR ON WINDOW CREATION!" << std::endl;
    std::cout << g_low->sdl->SDL_GetError() << std::endl;
  }
  sdl_framebuffer = g_low->sdl->SDL_GetWindowSurface(sdl_framebuffer_window);
#endif
  if (!sdl_framebuffer) { std::cout << "sdl_framebuffer NULL" << std::endl; exit(0); }
  return sdl_framebuffer;
}

Low_SDL_Surface *init_iot_surface_framebuffer(int scr_x, int scr_y)
{
  return 0;
  // TODO
  // sdl_framebuffer = ... ;
}

Low_SDL_Window *sdl_display2_window = 0;
Low_SDL_Surface *sdl_display2_framebuffer = 0;
Low_SDL_GLContext context_display2;


Low_SDL_Surface *init_2nd_display(int scr_x, int scr_y)
{
  g_low->sdl->SDL_Init(Low_SDL_INIT_VIDEO_NOPARACHUTE_JOYSTICK);


  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_RED_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_GREEN_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_BLUE_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_ALPHA_SIZE, 8);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_DEPTH_SIZE, 24);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_DOUBLEBUFFER, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_STENCIL_SIZE, 1);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  //SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4);

#ifndef EMSCRIPTEN
#ifndef OPENGL_ES
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MINOR_VERSION, 3);

  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_PROFILE_MASK, Low_SDL_GL_CONTEXT_PROFILE_CORE);
#else 
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_MINOR_VERSION, 0);

  g_low->sdl->SDL_GL_SetAttribute(Low_SDL_GL_CONTEXT_PROFILE_MASK, Low_SDL_GL_CONTEXT_PROFILE_ES);

#endif
#endif


  sdl_display2_window = g_low->sdl->SDL_CreateWindow("Framebuffer", Low_SDL_WINDOWPOS_CENTERED_DISPLAY, Low_SDL_WINDOWPOS_CENTERED_DISPLAY, scr_x, scr_y, Low_SDL_WINDOW_OPENGL_SHOWN);
  //sdl_display2_framebuffer = g_low->sdl->SDL_GetWindowSurface(sdl_display2_window);
  //context_display2 = g_low->sdl->SDL_GL_CreateContext(sdl_display2_window);
  //    g_low->sdl->SDL_GL_MakeCurrent(sdl_window, NULL);
  //    g_low->sdl->SDL_GL_MakeCurrent(sdl_display2_window, context_display2);

  //g_low->ogl->glEnable(Low_GL_DEPTH_TEST);
  //g_low->ogl->glDepthMask(Low_GL_TRUE);

  //g_low->ogl->glClearColor( 0, 0, 0, 0 );
  //g_low->ogl->glViewport(0,0,scr_x, scr_y);
  //g_low->ogl->glDisable(Low_GL_CULL_FACE);
  //   g_low->sdl->SDL_GL_MakeCurrent(sdl_display2_window, NULL);
  //    g_low->sdl->SDL_GL_MakeCurrent(sdl_window, g_context);

  return 0;
}
