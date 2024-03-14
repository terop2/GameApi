
#include "GameApi_low.hh"

#ifndef ARM
#include <cassert>
#include <iostream>
#include <sstream>


#define VIRTUAL_REALITY 1
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
#ifndef NO_THREADS
#define THREADS 1
#endif
#endif
#define BATCHING 1

#ifndef EMSCRIPTEN
#define VAO 1
#endif

#ifndef RASPI
#define USE_MIX 1
#define USE_TEXTURE_READ 1
#endif



#define NO_SDL_GLEXT 
#ifndef DEPS
//#ifndef RASPI
//#ifndef LINUX
#ifndef ANDROID
#include <GL/glew.h> 
#endif
//#endif
//#endif
#ifdef __APPLE__
#define GLEW_HACK
#include <OpenGL/gl.h>
#else
#ifndef ANDROID
#ifndef RASPI
//#ifndef LINUX
#include <GL/gl.h>
//#endif
#else
//#define USE_GLES2 1
//#include <GLES2/gl2.h>
#include <GL/gl.h>
#define GLEW_HACK
//#include <GL/glext.h>
#endif
#else // android
#include <GLES2/gl2.h>
#endif

#endif
#endif
#ifndef DEPS
#ifdef SDL2_USED
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h> 
#include <SDL/SDL_opengl.h>
#endif
#endif
#include "GameApi_h.hh"
//#include <SDL_mixer.h>

#ifdef USE_MIX
#include <SDL_mixer.h>
#endif
#if 0
#include <AudioService.h>
#endif


#ifdef WAYLAND
#include <SDL_syswm.h>
#endif


#ifdef WINDOWS
#define GLEW_HACK
#endif

#ifdef LINUX
#define GLEW_HACK
#endif

#undef glBindAttribLocation
#undef glActiveTexture
#undef glClientActiveTexture
#undef glTexStorage3D
#undef glTexSubImage3D
#undef glGenBuffers
#undef glDeleteBuffers
#undef glBindBuffer
#undef glBufferData
#undef glBufferSubData
#undef glVertexAttribPointer
#undef glVertexAttribIPointer
#undef glVertexAttribDivisor
#undef glGenVertexArrays
#undef glDeleteVertexArrays
#undef glBindVertexArray
#undef glEnableVertexAttribArray
#undef glDisableVertexAttribArray
#undef glTexStorate3d
#undef glTexSubImage3d
#undef glDrawArraysInstanced
#undef glDrawArrays
#undef glDrawElements
#undef glGenFramebuffers
#undef glBindFramebuffer
#undef glBindRenderbuffer
#undef glFramebufferRenderbuffer
#undef glCreateShader
#undef glShaderSource
#undef glCompileShader
#undef glGetShaderInfoLog
#undef glGetShaderiv
#undef glDeleteShader
#undef glCreaeProgram
#undef glDeleteProgram
#undef glAttachhader
#undef glDetachShader
#undef glLinkProgram
#undef glUseProgram
#undef glGetProgramInfoLog
#undef glBindFragDataLocation
#undef glProgramParameteriEXT
#undef glGetUniformLocation
#undef glUniform1f
#undef glUniform1i
#undef glUniform2f
#undef glUniform3f
#undef glUniform4f
#undef glGetUniformfv
#undef glRenderbufferStorage
#undef glShaderSource
#undef glGetShaderInfoLog
#undef glCreateProgram
#undef glAttachShader
#undef glGetProgramInfoLog
#undef glUniform1iv
#undef glUniform1fv
#undef glUniformMatrix4fv
#undef glUniform3fv
#undef glTexStorage3D
#undef glTexSubImage3D
#undef glGetAttribLocation
#undef glGenerateMipmap
#undef glMapBuffer
#undef glUnmapBuffer

#undef glDepthFunc
#undef glGenRenderbuffers
#undef glFramebufferTexture2D
#undef glGetIntergerv
#undef glCheckFramebufferStatus
#undef glFinish
#undef glMatrixLoadIdentityEXT
#undef glMatrixMode
#undef glLoadIdentity
#undef glTranslatef
#undef glDrawBuffers
#undef SDL_OpenAudio

#undef Mix_PlayChannel
#undef Mix_HaltChannel

std::string to_str(int val)
{
  std::stringstream ss;
  ss << std::hex << val;
  return ss.str();
}

struct SDL_EVENT_HACK
{
  SDL_Event e;
  unsigned int cc; // HACK BECAUSE OF SDL EVENT UNION IS HANDLED BADLY IN EMSCRIOPTEN... EMSCRIPTEN WRITES ONE BYTE OUTSIDE OF SDL_Event structure.
};

void check_err(const char *name)
{
#if 0  
  GLenum e = 0;
  while((e=glGetError())!=GL_NO_ERROR)
    {
      std::cout << "Opengl error: " << name << ":" << std::hex << e << std::dec << std::endl;
    }
#endif
}

void map_enums_sdl(unsigned int &i) {
  switch(i) {
#if 0
    case Low_SAMPLE_FREQ: i=SAMPLE_FREQ; break;
    case Low_AUDIO_F32LSB: i=AUDIO_F32LSB; break;
    case Low_SAMPLE_BUF_SIZE: i=SAMPLE_BUF_SIZE; break;
#endif
      
  case Low_SDL_WINDOW_SHOWN: i=SDL_WINDOW_SHOWN; break;
  case Low_SDL_WINDOW_OPENGL_SHOWN: i=SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN; break;
  case Low_SDL_WINDOW_OPENGL_SHOWN_RESIZEABLE: i=SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |SDL_WINDOW_RESIZABLE; break;

  };
}
void map_enums_sdl(int &i) {
  switch(i) {
  case Low_SDL_WINDOW_FULLSCREEN: i=SDL_WINDOW_FULLSCREEN; break;
  case Low_SDL_WINDOWPOS_CENTERED: i=SDL_WINDOWPOS_CENTERED; break;
  case Low_SDL_WINDOWPOS_CENTERED_DISPLAY: i=SDL_WINDOWPOS_CENTERED_DISPLAY(1); break;
#ifndef EMSCRIPTEN
  case Low_SDL_GL_SHARE_WITH_CURRENT_CONTEXT: i=SDL_GL_SHARE_WITH_CURRENT_CONTEXT; break;
#endif
  case Low_SDL_WINDOW_SHOWN: i=SDL_WINDOW_SHOWN; break;
  case Low_SDL_WINDOW_OPENGL_SHOWN: i=SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN; break;
  case Low_SDL_WINDOW_OPENGL_SHOWN_RESIZEABLE: i=SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |SDL_WINDOW_RESIZABLE; break;
#ifdef LINUX
  case Low_SDL_INIT_VIDEO_NOPARACHUTE_JOYSTICK: i=SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE; break;
#else
  case Low_SDL_INIT_VIDEO_NOPARACHUTE_JOYSTICK: i=SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE; break;
#endif
#ifdef EMSCRIPTEN
  case  Low_SDL_GL_CONTEXT_PROFILE_MASK: i=21; /*::SDL_GL_CONTEXT_PROFILE_MASK;*/ break;
  case   Low_SDL_GL_CONTEXT_PROFILE_CORE: i=0x0001; /*::SDL_GL_CONTEXT_PROFILE_CORE;*/ break;
  case Low_SDL_GL_CONTEXT_PROFILE_ES: i=0x0004; /*::SDL_GL_CONTEXT_PROFILE_ES;*/ break;
  case   Low_SDL_GL_CONTEXT_MINOR_VERSION: i=18 /*::SDL_GL_CONTEXT_MINOR_VERSION;*/; break; 
  case   Low_SDL_GL_CONTEXT_MAJOR_VERSION: i=17; /*::SDL_GL_CONTEXT_MAJOR_VERSION;*/ break;
  case Low_SDL_GL_CONTEXT_FLAGS: i=20; /*::SDL_GL_CONTEXT_FLAGS;*/ break;
  case Low_SDL_GL_CONTEXT_DEBUG_FLAG: i=1; /*::SDL_GL_CONTEXT_DEBUG_FLAG;*/ break;
#else
  case  Low_SDL_GL_CONTEXT_PROFILE_MASK: i=SDL_GL_CONTEXT_PROFILE_MASK; break;
  case   Low_SDL_GL_CONTEXT_PROFILE_CORE: i=SDL_GL_CONTEXT_PROFILE_CORE; break;
  case Low_SDL_GL_CONTEXT_PROFILE_ES: i=SDL_GL_CONTEXT_PROFILE_ES; break;
  case   Low_SDL_GL_CONTEXT_MINOR_VERSION: i=SDL_GL_CONTEXT_MINOR_VERSION; break; 
  case   Low_SDL_GL_CONTEXT_MAJOR_VERSION: i=SDL_GL_CONTEXT_MAJOR_VERSION; break;
  case Low_SDL_GL_CONTEXT_FLAGS: i=SDL_GL_CONTEXT_FLAGS; break;
  case Low_SDL_GL_CONTEXT_DEBUG_FLAG: i=SDL_GL_CONTEXT_DEBUG_FLAG; break;
#endif
  case   Low_SDL_GL_STENCIL_SIZE: i=SDL_GL_STENCIL_SIZE; break;
  case   Low_SDL_GL_DOUBLEBUFFER: i=SDL_GL_DOUBLEBUFFER; break;
  case   Low_SDL_GL_DEPTH_SIZE: i=SDL_GL_DEPTH_SIZE; break;
  case   Low_SDL_GL_ALPHA_SIZE: i=SDL_GL_ALPHA_SIZE; break;
  case   Low_SDL_GL_BLUE_SIZE: i=SDL_GL_BLUE_SIZE; break;
  case   Low_SDL_GL_GREEN_SIZE: i=SDL_GL_GREEN_SIZE; break;
  case   Low_SDL_GL_RED_SIZE: i=SDL_GL_RED_SIZE; break;
  case Low_SDL_GL_MULTISAMPLEBUFFERS: i = SDL_GL_MULTISAMPLEBUFFERS; break;
  case Low_SDL_GL_MULTISAMPLESAMPLES: i = SDL_GL_MULTISAMPLESAMPLES; break;
  case Low_SDL_ENABLE: i=SDL_ENABLE; break;
    //  case Low_SDL_DROPFILE: i=SDL_DROPFILE; break;
  };
}

void map_enums(int &i)
{
  switch(i) {
  case Low_GL_READ_BUFFER: i=GL_READ_BUFFER; break;
  case  Low_GL_BYTE: i=GL_BYTE; break;
  case  Low_GL_DOUBLE: i=GL_DOUBLE; break;
  case Low_GL_RGBA32F: i=GL_RGBA32F; break;
  case Low_GL_NONE: i=GL_NONE; break;
  case Low_GL_LINK_STATUS: i=GL_LINK_STATUS; break;
    //case Low_GL_COMPILE_STATUS: i=GL_COMPILE_STATUS; break;
  case Low_GL_CW: i=GL_CW; break;
  case Low_GL_CCW: i=GL_CCW; break;
  case Low_GL_PIXEL_PACK_BUFFER: i = GL_PIXEL_PACK_BUFFER; break;
  case Low_GL_PIXEL_UNPACK_BUFFER: i=GL_PIXEL_UNPACK_BUFFER; break;
  case Low_GL_STREAM_DRAW: i=GL_STREAM_DRAW; break;
  case Low_GL_WRITE_ONLY: i=GL_WRITE_ONLY; break;
  case Low_GL_READ_ONLY: i=GL_READ_ONLY; break;

  case Low_GL_VENDOR: i=GL_VENDOR; break;
  case Low_GL_RENDERER: i=GL_RENDERER; break;
  case Low_GL_VERSION: i=GL_VERSION; break;
  case Low_GL_CULL_FACE: i=GL_CULL_FACE; break;
  case Low_GL_ARRAY_BUFFER: i=GL_ARRAY_BUFFER; break;
  case Low_GL_ELEMENT_ARRAY_BUFFER: i=GL_ELEMENT_ARRAY_BUFFER; break;
  case Low_GL_FLOAT: i=GL_FLOAT; break;
  case Low_GL_UNSIGNED_SHORT: i=GL_UNSIGNED_SHORT; break;
  case Low_GL_INT: i=GL_INT; break;
  case Low_GL_FALSE: i=GL_FALSE; break;
  case Low_GL_TRIANGLES: i=GL_TRIANGLES; break; 
  case Low_GL_TRIANGLE_STRIP: i=GL_TRIANGLE_STRIP; break;
  case Low_GL_UNSIGNED_BYTE: i=GL_UNSIGNED_BYTE; break;
  case Low_GL_UNSIGNED_INT: i=GL_UNSIGNED_INT; break;
  case   Low_GL_QUADS: i=GL_QUADS; break;
  case   Low_GL_DEPTH_TEST: i=GL_DEPTH_TEST; break;
  case Low_GL_ALPHA_TEST: i=GL_ALPHA_TEST; break;
  case   Low_GL_STATIC_DRAW: i=GL_STATIC_DRAW; break;
  case   Low_GL_DYNAMIC_DRAW: i=GL_DYNAMIC_DRAW; break;
  case   Low_GL_TEXTURE0: i=GL_TEXTURE0; break;
  case   Low_GL_TEXTURE1: i=GL_TEXTURE1; break;
  case   Low_GL_TEXTURE2: i=GL_TEXTURE2; break;
  case   Low_GL_TEXTURE3: i=GL_TEXTURE3; break;
  case   Low_GL_TEXTURE4: i=GL_TEXTURE4; break;
  case   Low_GL_TEXTURE5: i=GL_TEXTURE5; break;
  case   Low_GL_TEXTURE6: i=GL_TEXTURE6; break;
  case   Low_GL_TEXTURE7: i=GL_TEXTURE7; break;
  case   Low_GL_TEXTURE8: i=GL_TEXTURE8; break;
  case   Low_GL_TEXTURE9: i=GL_TEXTURE9; break;
  case   Low_GL_TEXTURE10: i=GL_TEXTURE10; break;
  case   Low_GL_TEXTURE11: i=GL_TEXTURE11; break;
  case   Low_GL_TEXTURE12: i=GL_TEXTURE12; break;
  case   Low_GL_TEXTURE13: i=GL_TEXTURE13; break;
  case   Low_GL_TEXTURE14: i=GL_TEXTURE14; break;
  case   Low_GL_TEXTURE15: i=GL_TEXTURE15; break;
  case   Low_GL_TEXTURE16: i=GL_TEXTURE16; break;
  case   Low_GL_TEXTURE17: i=GL_TEXTURE17; break;
  case   Low_GL_TEXTURE18: i=GL_TEXTURE18; break;
  case   Low_GL_TEXTURE19: i=GL_TEXTURE19; break;
  case   Low_GL_TEXTURE20: i=GL_TEXTURE20; break;

  case    Low_GL_TEXTURE_2D: i=GL_TEXTURE_2D; break;
#ifndef USE_GLES2
  case    Low_GL_TEXTURE_2D_ARRAY: i=GL_TEXTURE_2D_ARRAY; break;
#endif
  case    Low_GL_TEXTURE_CUBE_MAP: i=GL_TEXTURE_CUBE_MAP; break;
  case    Low_GL_LINEAR: i=GL_LINEAR; break;
  case    Low_GL_LINEAR_MIPMAP_LINEAR: i=GL_LINEAR_MIPMAP_LINEAR; break;
  case    Low_GL_TEXTURE_MIN_FILTER: i=GL_TEXTURE_MIN_FILTER; break;
  case    Low_GL_TEXTURE_MAG_FILTER: i=GL_TEXTURE_MAG_FILTER; break;
  case    Low_GL_TEXTURE_WRAP_S: i=GL_TEXTURE_WRAP_S; break;
  case    Low_GL_CLAMP_TO_EDGE: i=GL_CLAMP_TO_EDGE; break;
  case    Low_GL_TEXTURE_WRAP_T: i=GL_TEXTURE_WRAP_T; break;
  case    Low_GL_PERSPECTIVE_CORRECTION_HINT: i=GL_PERSPECTIVE_CORRECTION_HINT; break;
  case    Low_GL_NICEST: i=GL_NICEST; break;
  case    Low_GL_RED: i=GL_RED; break;
  case    Low_GL_RENDERBUFFER: i=GL_RENDERBUFFER; break;
  case    Low_GL_FRAMEBUFFER:  i=GL_FRAMEBUFFER; break;
  case    Low_GL_TRUE: i=GL_TRUE; break;
  case    Low_GL_TEXTURE_BINDING_2D:  i=GL_TEXTURE_BINDING_2D; break;
  case    Low_GL_RGBA: i=GL_RGBA; break;
  case    Low_GL_SCISSOR_TEST:  i=GL_SCISSOR_TEST; break;
  case    Low_GL_FRAMEBUFFER_COMPLETE: i=GL_FRAMEBUFFER_COMPLETE; break;
  case    Low_GL_NEAREST: i=GL_NEAREST; break;
  case    Low_GL_DEPTH_COMPONENT16: i=GL_DEPTH_COMPONENT16; break;
  case Low_GL_DEPTH_COMPONENT32F: i=GL_DEPTH_COMPONENT32F; break;
  case Low_GL_DEPTH_COMPONENT: i=GL_DEPTH_COMPONENT; break;
  case    Low_GL_DEPTH_ATTACHMENT:  i=GL_DEPTH_ATTACHMENT; break;
  case    Low_GL_COLOR_ATTACHMENT0: i=GL_COLOR_ATTACHMENT0; break;
  case    Low_GL_VIEWPORT:  i=GL_VIEWPORT; break;
  case   Low_GL_SRC_ALPHA: i=GL_SRC_ALPHA; break;
  case   Low_GL_ONE_MINUS_SRC_ALPHA: i=GL_ONE_MINUS_SRC_ALPHA; break;
  case   Low_GL_ZERO: i=GL_ZERO; break;
  case   Low_GL_ONE: i=GL_ONE; break;
  case   Low_GL_SRC_COLOR: i=GL_SRC_COLOR; break;
  case   Low_GL_ONE_MINUS_SRC_COLOR: i=GL_ONE_MINUS_SRC_COLOR; break;
  case   Low_GL_DST_COLOR: i=GL_DST_COLOR; break;
  case   Low_GL_ONE_MINUS_DST_COLOR: i=GL_ONE_MINUS_DST_COLOR; break;
    //GL_ONE_MINUS_SRC_ALPHA,
  case   Low_GL_DST_ALPHA: i=GL_DST_ALPHA; break;
  case   Low_GL_ONE_MINUS_DST_ALPHA: i=GL_ONE_MINUS_DST_ALPHA; break;
  case   Low_GL_CONSTANT_COLOR: i=GL_CONSTANT_COLOR; break;
  case   Low_GL_ONE_MINUS_CONSTANT_COLOR: i=GL_ONE_MINUS_CONSTANT_COLOR; break;
  case   Low_GL_CONSTANT_ALPHA:  i=GL_CONSTANT_ALPHA; break;
  case   Low_GL_LIGHTING:  i=GL_LIGHTING; break;
  case   Low_GL_MODELVIEW: i=GL_MODELVIEW; break;
  case   Low_GL_TEXTURE_ENV: i=GL_TEXTURE_ENV; break;
  case   Low_GL_BLEND:  i=GL_BLEND; break;
  case Low_GL_ALWAYS:  i=GL_ALWAYS; break;
  case Low_GL_KEEP:  i=GL_KEEP; break;
  case Low_GL_REPLACE:  i=GL_REPLACE; break;
  case Low_GL_NOTEQUAL:  i=GL_NOTEQUAL; break;
  case Low_GL_FRONT_AND_BACK: i=GL_FRONT_AND_BACK; break;
  case Low_GL_FRONT: i=GL_FRONT; break;
  case Low_GL_BACK: i=GL_BACK; break;
  case Low_GL_LINE:  i=GL_LINE; break;
  case Low_GL_FILL:  i=GL_FILL; break;
  case Low_GL_STENCIL_TEST: i=GL_STENCIL_TEST; break;
  case Low_GL_LEQUAL:  i=GL_LEQUAL; break;
  case Low_GL_EQUAL: i=GL_EQUAL; break;
    //GL_ALWAYS,
  case Low_GL_LESS: i=GL_LESS; break;
  case Low_GL_GREATER: i=GL_GREATER; break; 
  case Low_GL_GEQUAL: i= GL_GEQUAL; break;
    //case Low_GL_TEXTURE_ENV_MOVE: i=GL_TEXTURE_ENV_MOVE; break;
  case Low_GL_LINE_SMOOTH: i=GL_LINE_SMOOTH; break;
  case Low_GL_LINES: i=GL_LINES; break;
  case Low_GL_POINTS: i=GL_POINTS; break;
  case Low_GL_TEXTURE_ENV_MODE: i=GL_TEXTURE_ENV_MODE; break;
  case Low_GL_TEXTURE_WIDTH: i=GL_TEXTURE_WIDTH; break;
  case Low_GL_TEXTURE_HEIGHT: i=GL_TEXTURE_HEIGHT; break;
  case Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X: i=GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
  case Low_GL_TEXTURE_CUBE_MAP_NEGATIVE_X: i=GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
  case Low_GL_TEXTURE_CUBE_MAP_POSITIVE_Y: i=GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
  case Low_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: i=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
  case Low_GL_TEXTURE_CUBE_MAP_POSITIVE_Z: i=GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
  case Low_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: i=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
  case Low_GL_TEXTURE_WRAP_R: i=GL_TEXTURE_WRAP_R; break;
  case Low_GL_REPEAT: i=GL_REPEAT; break;
  case Low_GL_RGBA8: i=GL_RGBA8; break;
  case Low_GL_MULTISAMPLE: i=GL_MULTISAMPLE; break;
#ifndef USE_GLES2
  case  Low_GL_GEOMETRY_SHADER: i=GL_GEOMETRY_SHADER; break;
#endif
  case   Low_GL_VERTEX_SHADER: i=GL_VERTEX_SHADER; break;
  case   Low_GL_FRAGMENT_SHADER: i=GL_FRAGMENT_SHADER; break;
  case   Low_GL_COMPILE_STATUS: i=GL_COMPILE_STATUS; break;
#ifndef USE_GLES2
  case   Low_GL_LINES_ADJACENCY_EXT: i=GL_LINES_ADJACENCY_EXT; break;
  case   Low_GL_TRIANGLES_ADJACENCY_EXT: i=GL_TRIANGLES_ADJACENCY_EXT; break;
#endif
  case   Low_GL_LINE_STRIP: i=GL_LINE_STRIP; break;
#ifndef USE_GLES2
  case   Low_GL_GEOMETRY_INPUT_TYPE_EXT: i=GL_GEOMETRY_INPUT_TYPE_EXT; break;
  case   Low_GL_GEOMETRY_VERTICES_OUT_EXT: i=GL_GEOMETRY_VERTICES_OUT_EXT; break;
#endif
  case   Low_GL_NO_ERROR: i=GL_NO_ERROR; break;
#ifndef USE_GLES2
  case   Low_GL_GEOMETRY_OUTPUT_TYPE_EXT: i=GL_GEOMETRY_OUTPUT_TYPE_EXT; break;
#endif

  default: break;
  };
  // this assert is broken since the values are already in opengl ones,
  // which are arbitrary values, instead of nice enum
  //assert(i<Low_GL_ARRAY_BUFFER ||i>Low_GL_VERSION);
}


void GLAPIENTRY MessageCallback( GLenum source,
				 GLenum type,
				 GLuint id,
				 GLenum severity,
				 GLsizei length,
				 const GLchar *message,
				 const void *userParam)
{
  std::cout << "GL CALLBACK: " << (type==GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << std::hex << "type=" << type << " severity=" << severity << " message=" << message << std::dec << std::endl;
}

class OpenglApi : public OpenglLowApi
{
public:
  virtual void init() {
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);
    
  }
  virtual void cleanup() { }

  virtual void glDrawBuffers(int n, const unsigned int *bufs) {
#ifdef GLEW_HACK
#define glDrawBuffers GLEW_GET_FUN(__glewDrawBuffers)
#endif
    ::glDrawBuffers((GLsizei)n, bufs);
  }


  virtual int glGetError() {
    //return Low_GL_NO_ERROR;
    int i = ::glGetError(); 
    if (i==GL_NO_ERROR) i=Low_GL_NO_ERROR;
    else
      {
	if (i==GL_INVALID_ENUM) std::cout << "GL_INVALID_ENUM" << std::endl;
	if (i==GL_INVALID_VALUE) std::cout << "GL_INVALID_VALUE" << std::endl;
	if (i==GL_INVALID_OPERATION) std::cout << "GL_INVALID_OPERATION" << std::endl;
	if (i==GL_INVALID_FRAMEBUFFER_OPERATION) std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
	if (i==GL_OUT_OF_MEMORY) std::cout << "GL_OUT_OF_MEMORY" << std::endl;
	if (i==GL_STACK_UNDERFLOW) std::cout << "GL_STACK_UNDERFLOW" << std::endl;
	if (i==GL_STACK_OVERFLOW) std::cout << "GL_STACK_OVERFLOW" << std::endl;
      }
    return i;
  }
  
  virtual void glColor4ub(int r, int g, int b, int a) { 
    //::glColor4ub(r,g,b,a);    check_err("glColor4ub");
 }
  virtual void glColor4f(float r, float g, float b, float a) { 
    //::glColor4f(r,g,b,a);    check_err("glColor4f");
 }
  virtual void glClearColor(float r, float g, float b, float a) { ::glClearColor(r,g,b,a);    check_err("glClearColor");
 }
  virtual void glClear(int bits) { 
    GLenum e = 0;
    if (bits & Low_GL_COLOR_BUFFER_BIT) e|=GL_COLOR_BUFFER_BIT;
    if (bits & Low_GL_DEPTH_BUFFER_BIT) e|=GL_DEPTH_BUFFER_BIT;
    if (bits & Low_GL_STENCIL_BUFFER_BIT) e|=GL_STENCIL_BUFFER_BIT;
    ::glClear(e);    check_err("glClear");
 }
  virtual void glDepthMask(int a) { map_enums(a); ::glDepthMask(a);    check_err("glDepthMask");
 }
  virtual void glStencilMask(int a) { map_enums(a); ::glStencilMask(a);    check_err("glStencilMask");
 }
  virtual void glStencilOp(int keep, int keep2, int repl) {
    map_enums(keep);
    map_enums(keep2);
    map_enums(repl);
    ::glStencilOp(keep,keep2, repl); 
    check_err("glStencilOp");

}
  virtual void glStencilFunc(int notequal, int a, int b) {
    map_enums(notequal);
    map_enums(a);
    map_enums(b);
    ::glStencilFunc(notequal, a,b); 
    check_err("glStencilFunc");

}
  virtual void glPolygonMode(int front_and_back, int gl_line) { 
#ifdef USE_TEXTURE_READ
    map_enums(front_and_back);
    map_enums(gl_line);
    //return ::glPolygonMode(front_and_back,gl_line); 
#endif
}
  virtual void glClearStencil(int val) { 
    map_enums(val);
    ::glClearStencil(val); 
    check_err("glClearStencil");

}
  virtual void glTexEnvi(int a, int mode, int rep) { 
    map_enums(a);
    map_enums(mode);
    map_enums(rep);
    check_err("before glTexEnvi");
    //::glTexEnvi(a,mode,rep);
    check_err("glTexEnvi");

 }

  // attribs
  virtual void glEnable(int val) { 
    map_enums(val);
    check_err("before glEnable");
    ::glEnable(val); 
    check_err("glEnable");

}
  virtual void glDisable(int val) { 
    map_enums(val);
    //check_err("before glDisable" + to_str(val));
    ::glDisable(val);
    //check_err("glDisable" + to_str(val));

 }
  virtual void glGetFloatv(int val, float *params) { 
    map_enums(val);
    ::glGetFloatv(val,params);
    check_err("glGetFloatv");

 }

  // viewports
  virtual void glViewport(int x, int y, unsigned int w, unsigned int h) { ::glViewport(x,y,w,h); }

  // textures
  virtual void glGenTextures(int val, unsigned int *tex) { 
    ::glGenTextures(val,tex);
    check_err("glGenTexture");

 }
  virtual void glDeleteTextures(int val, const unsigned int *tex) { ::glDeleteTextures(val,tex); 
    check_err("glDeleteTextures");

}
  virtual void glBindTexture(int ID, int tex) { 
    map_enums(ID);
    ::glBindTexture(ID,tex); 
    check_err("glBindTexture");

}
  virtual void glTexImage2D(int ID, int a, int b,unsigned int c, unsigned int d, int e, int f, int g, void *ptr) {
    map_enums(ID);
    map_enums(b);
    map_enums(f);
    map_enums(g);
    ::glTexImage2D(ID,a,b,c,d,e,f,g,ptr); 
    check_err("glTexImage2D");

}
  virtual void glTexSubImage2D(int ID, int a, int b,int bb, unsigned int c, unsigned int d, int e, int f, void *ptr) {
    map_enums(ID);
    map_enums(e);
    map_enums(f);
    ::glTexSubImage2D(ID,a,b,bb,c,d,e,f,ptr); 
    check_err("glTexSubImage2D");

}
  virtual void glCopyTexImage2D(int ID, int a,int b,int c,int d, int e,int f, int ptr) { 
    map_enums(ID);
    map_enums(b);
    ::glCopyTexImage2D(ID,a,b,c,d,e,f,ptr); 
    check_err("glCopyTexImage2D");
}
  virtual void glTexParameteri(int ID, int a,int b) {
    map_enums(ID);
    map_enums(a);
    map_enums(b);
    ::glTexParameteri(ID,a,b); 
    check_err("glTexParameteri");
}
  virtual void glHint(int hint, int value) { 
    map_enums(hint);
    map_enums(value);
    ::glHint(hint,value); 
    check_err("glHint");

}
  virtual void glActiveTexture(int a) { 
#ifdef GLEW_HACK
#define glActiveTexture GLEW_GET_FUN(__glewActiveTexture)
#endif
    map_enums(a);
    ::glActiveTexture(a); 
    check_err("glActiveTexture");
  }
  virtual void glClientActiveTexture(int a) { 
#ifdef GLEW_HACK
#define glClientActiveTexture GLEW_GET_FUN(__glewClientActiveTexture)
#endif
    map_enums(a);
    check_err("before glClientActiveTexture");

    //::glClientActiveTexture(a); 
    check_err("glClientActiveTexture");
  }
  virtual void glTexStorage3D(int arr, int a, int flag, int w, int h, int layer_count) {
#ifndef USE_GLES2    
#ifdef GLEW_HACK
#define glTexStorage3D GLEW_GET_FUN(__glewTexStorage3D)
#endif
    map_enums(arr);
    map_enums(flag);
    //::glTexStorage3D(arr,a,flag,w,h,layer_count); 
    check_err("glTexStorage3D");
 #endif
  }
  virtual void glTexSubImage3D(int arr, int a,int b,int c,int d,int e,int f, int g, int rgba, int unsig_byte, void *buffer) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glTexSubImage3D GLEW_GET_FUN(__glewTexSubImage3D)
#endif
    map_enums(arr);
    map_enums(rgba);
    map_enums(unsig_byte);
    //::glTexSubImage3D(arr,a,b,c,d,e,f,g,rgba,unsig_byte,buffer); 
    check_err("glTexSubImage3D");
#endif
  }
  virtual void glGetTexLevelParameteriv(int a, int b, int w, int *ptr) { 
    map_enums(a);
    map_enums(w);
    ::glGetTexLevelParameteriv(a,b,w,ptr); 
    check_err("glGetTexLevelParameteriv");
}
  virtual void glGetTexImage(int a, int b, int rgba, int unsign_byte, void *ptr) { 
#ifdef USE_TEXTURE_READ
    map_enums(a);
    map_enums(rgba);
    map_enums(unsign_byte);
    //::glGetTexImage(a,b,rgba,unsign_byte,ptr); 
    check_err("glGetTexImage");
#endif
}
  virtual void glReadBuffer(int a) { 
#ifdef USE_TEXTURE_READ
    map_enums(a);
    //::glReadBuffer(a); 
    check_err("glReadBuffer");
#endif
}

  virtual void glDrawBuffer(int s)
  {
    map_enums(s);
    ::glDrawBuffer(s); 
  }
  


  // blend
  virtual void glBlendFunc(int val, int val2) { 
    map_enums(val);
    map_enums(val2);
::glBlendFunc(val,val2); 
    check_err("glBlendFunc");

}
  
  // scissor
  virtual void glScissor(float x, float y, float w, float h) { 
    ::glScissor(x,y,w,h); }

  // vertex arrays
  virtual void glGenBuffers(int i, unsigned int *buffers) { 
#ifdef GLEW_HACK
#define glGenBuffers GLEW_GET_FUN(__glewGenBuffers)
#endif
::glGenBuffers(i,buffers); 
    check_err("glGenBuffers");
  }
  virtual void glDeleteBuffers(int count, const unsigned int *buffers) { 
#ifdef GLEW_HACK
#define glDeleteBuffers GLEW_GET_FUN(__glewDeleteBuffers)
#endif
::glDeleteBuffers(count,buffers); 
    check_err("glDeleteBuffers");
  }
  virtual void glBindBuffer(int a, unsigned int data) { 
#ifdef GLEW_HACK
#define glBindBuffer GLEW_GET_FUN(__glewBindBuffer)
#endif
    map_enums(a);
::glBindBuffer(a,data); 
    check_err("glBindBuffer");
  }
  virtual void glBufferData(int a, int size, const void *ptr, int static_draw) { 
#ifdef GLEW_HACK
#define glBufferData GLEW_GET_FUN(__glewBufferData)
#endif
    map_enums(a);
    map_enums(static_draw);
    ::glBufferData(a,size,ptr,static_draw); 
    check_err("glBufferData");
  }
  virtual void glBufferSubData(int a, int b, int c, const void* d) { 
#ifdef GLEW_HACK
#define glBufferSubData GLEW_GET_FUN(__glewBufferSubData)
#endif
    map_enums(a);
::glBufferSubData(a,b,c,d);
    check_err("glBufferSubData");
  }
  virtual void glVertexAttribPointer(int a, int b, int gl_float, int boolean, unsigned int c, const void *ptr) { 
#ifdef GLEW_HACK
#define glVertexAttribPointer GLEW_GET_FUN(__glewVertexAttribPointer)
#endif
    map_enums(gl_float);
    map_enums(boolean);
::glVertexAttribPointer(a,b,gl_float,boolean,c,ptr); 
    check_err("glVertexAttribPointer");
  }
virtual void glVertexAttribIPointer(int a, int b, int gl_float, int boolean, const void *ptr) {
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glVertexAttribIPointer GLEW_GET_FUN(__glewVertexAttribIPointer)
#endif
  map_enums(gl_float);
  map_enums(boolean);
  //::glVertexAttribIPointer(a,b,gl_float,boolean,ptr); 
    check_err("glVertexAttribIPointer");
#endif
}
  virtual void glVertexAttribDivisor(int a, int b) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glVertexAttribDivisor GLEW_GET_FUN(__glewVertexAttribDivisor)
#endif
    ::glVertexAttribDivisor(a,b); 
    check_err("glVertexAttribDivisor");
#endif
  }
  virtual void glGenVertexArrays(int i, unsigned int *arr) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glGenVertexArrays GLEW_GET_FUN(__glewGenVertexArrays)
#endif
    ::glGenVertexArrays(i,arr); 
    check_err("glGenVertexArrays");
#endif
  }
  virtual void glDeleteVertexArrays(int count, unsigned int *vao) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glDeleteVertexArrays GLEW_GET_FUN(__glewDeleteVertexArrays)
#endif
::glDeleteVertexArrays(count,vao);
#endif
  }
  virtual void glBindVertexArray(int vao) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glBindVertexArray GLEW_GET_FUN(__glewBindVertexArray)
#endif
::glBindVertexArray(vao); 
    check_err("glBindVertexArray");
#endif
  }
  virtual void glEnableVertexAttribArray(int a) { 
#ifdef GLEW_HACK
#define glEnableVertexAttribArray GLEW_GET_FUN(__glewEnableVertexAttribArray)
#endif
::glEnableVertexAttribArray(a); 
    check_err("glEnableVertexAttribArray");
  }
  virtual void glDisableVertexAttribArray(int a) { 
#ifdef GLEW_HACK
#define glDisableVertexAttribArray GLEW_GET_FUN(__glewDisableVertexAttribArray)
#endif
::glDisableVertexAttribArray(a); 
    check_err("glDisableVertexAttribArray");
  }
  virtual void glDrawArraysInstanced(int tri, int a, unsigned int b, unsigned int c) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glDrawArraysInstanced GLEW_GET_FUN(__glewDrawArraysInstanced)
#endif
    map_enums(tri);
::glDrawArraysInstanced(tri,a,b,c); 
    check_err("glDrawArraysInstanced");
#else
    glDrawArrays(tri,a,b);
#endif
  }
  virtual void glDrawArrays(int tri, int a, unsigned int b) { 
    //#ifdef GLEW_HACK
    //#define glDrawArrays GLEW_GET_FUN(__glewDrawArraysEXT)
    //#endif
    map_enums(tri);
    unsigned int tri2 = (unsigned int)tri;
    ::glDrawArrays(tri2,a,b); 
    check_err("glDrawArrays");
}
  virtual void glDrawElements(int tri, int a, int type, void *indices) {
    map_enums(tri);
    map_enums(type);
    unsigned int tri2 = (unsigned int)tri;
    ::glDrawElements(tri2,a,type,indices);
    check_err("glDrawElements");
  }
  // bitmaps
  virtual void glReadPixels(int x, int y, int w, int h, int rgba, int mode, void *ptr) { 
    map_enums(rgba);
    map_enums(mode);
::glReadPixels(x,y,w,h,rgba,mode,ptr); 
    check_err("glReadPixels");

}
  
  // fbo
  virtual void glGenFramebuffers(int i, unsigned int *fbo_id) {
#ifdef GLEW_HACK
#define glGenFramebuffers GLEW_GET_FUN(__glewGenFramebuffers) 
#endif
::glGenFramebuffers(i,fbo_id); 
    check_err("glGenFramebuffers");
}
  virtual void glGenRenderbuffers(int i, unsigned int *rbo_id) { 
#ifdef GLEW_HACK
#define glGenRenderbuffers GLEW_GET_FUN(__glewGenRenderbuffers)
#endif
::glGenRenderbuffers(i,rbo_id); 
    check_err("glGenRenderbuffers");
}
  virtual void glDeleteRenderBuffers(int i, unsigned int *rbo) { ::glDeleteRenderbuffers(i,rbo); check_err("glDeleteRenderBuffers"); }
  virtual void glDeleteFrameBuffers(int i, unsigned int *fbo) { ::glDeleteFramebuffers(i,fbo); check_err("glDeleteFrameBuffers"); }
  virtual void glBindFramebuffer(int a, int fbo_id) { 
#ifdef GLEW_HACK
#define glBindFramebuffer GLEW_GET_FUN(__glewBindFramebuffer)
#endif
    map_enums(a);
    ::glBindFramebuffer(a,fbo_id); check_err("glBindFrameBuffer"); }
  virtual void glBindRenderbuffer(int a, int fbo_id) { 
#ifdef GLEW_HACK
#define glBindRenderbuffer GLEW_GET_FUN(__glewBindRenderbuffer)
#endif
    map_enums(a);
    ::glBindRenderbuffer(a,fbo_id); check_err("glBindRenderbuffer"); }
  virtual void glRenderbufferStorage(int a, int d, int sx, int sy) { 
#ifdef GLEW_HACK
#define glRenderbufferStorage GLEW_GET_FUN(__glewRenderbufferStorage)
#endif
    map_enums(a);
    map_enums(d);
    ::glRenderbufferStorage(a,d,sx,sy); check_err("glRenderbufferStorage"); }
  virtual void glFramebufferRenderbuffer(int a, int d, int r, unsigned int tex) { 
#ifdef GLEW_HACK
#define glFramebufferRenderbuffer GLEW_GET_FUN(__glewFramebufferRenderbuffer)
#endif
    map_enums(a);
    map_enums(d);
    map_enums(r);
    ::glFramebufferRenderbuffer(a,d,r,tex); check_err("glFramebufferRenderbuffer"); }
  
  // shaders
  virtual unsigned int glCreateShader(int shader) { 
#ifdef GLEW_HACK
#define glCreateShader GLEW_GET_FUN(__glewCreateShader)
#endif
    map_enums(shader);
return ::glCreateShader(shader); }
  virtual void glShaderSource(int h, int c, const char **strings, int *lengths) { 
#ifdef GLEW_HACK
#define glShaderSource GLEW_GET_FUN(__glewShaderSource)
#endif
::glShaderSource(h,c,strings,lengths); }
  virtual void glCompileShader(int h) { 
#ifdef GLEW_HACK
#define glCompileShader GLEW_GET_FUN(__glewCompileShader)
#endif
::glCompileShader(h); }
  virtual void glGetShaderInfoLog(unsigned int h, int val, int *length, char *buf) { 
#ifdef GLEW_HACK
#define glGetShaderInfoLog GLEW_GET_FUN(__glewGetShaderInfoLog)
#endif
::glGetShaderInfoLog(h,val,length,buf); }
  virtual void glGetShaderiv(int handle, int gl_compile_status, int *ptr) { 
#ifdef GLEW_HACK
#define glGetShaderiv GLEW_GET_FUN(__glewGetShaderiv)
#endif
    map_enums(gl_compile_status);
::glGetShaderiv(handle,gl_compile_status, ptr); }
  virtual void glDeleteShader(int h) { 
#ifdef GLEW_HACK
#define glDeleteShader GLEW_GET_FUN(__glewDeleteShader)
#endif
::glDeleteShader(h); }
  
  // programs
  virtual int glCreateProgram() {
#ifdef GLEW_HACK
#define glCreateProgram GLEW_GET_FUN(__glewCreateProgram)
#endif
      return ::glCreateProgram(); }
  virtual void glDeleteProgram(int p) { 
#ifdef GLEW_HACK
#define glDeleteProgram GLEW_GET_FUN(__glewDeleteProgram)
#endif
::glDeleteProgram(p); }
  virtual  void glAttachShader(int p, int h) { 
#ifdef GLEW_HACK
#define glAttachShader GLEW_GET_FUN(__glewAttachShader)
#endif
::glAttachShader(p,h); }
  virtual void glDetachShader(int p, int h) { 
#ifdef GLEW_HACK
#define glDetachShader GLEW_GET_FUN(__glewDetachShader)
#endif
::glDetachShader(p,h); }
  virtual void glLinkProgram(int p) { 
#ifdef GLEW_HACK
#define glLinkProgram GLEW_GET_FUN(__glewLinkProgram)
#endif
::glLinkProgram(p); }
  virtual void glUseProgram(int p) { 
#ifdef GLEW_HACK
#define glUseProgram GLEW_GET_FUN(__glewUseProgram)
#endif
    ::glUseProgram(p); }

  virtual void glGetProgramiv(unsigned int p, int pname, int *params) {
#ifdef GLEW_HACK
#define glGetProgramiv GLEW_GET_FUN(__glewGetProgramiv)
#endif
    map_enums(pname);
    ::glGetProgramiv(p,pname,params); 
  }

  virtual void glGetProgramInfoLog(unsigned int p, int num, int *len, char *buf) { 
#ifdef GLEW_HACK
#define glGetProgramInfoLog GLEW_GET_FUN(__glewGetProgramInfoLog)
#endif
::glGetProgramInfoLog(p,num,len,buf); }
  virtual void glBindFragDataLocation(int p, int num, const char *data) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glBindFragDataLocation GLEW_GET_FUN(__glewBindFragDataLocation)
#endif
    //::glBindFragDataLocation(p,num,data);
#endif
  }
  virtual void glBindAttribLocation(int p, int num, const char *data) { 
#ifdef GLEW_HACK
#define glBindAttribLocation GLEW_GET_FUN(__glewBindAttribLocation)
#endif
::glBindAttribLocation(p,num,data); 
  }
  virtual void glProgramParameteriEXT(int p, int geom, int inputtype) { 
#ifndef USE_GLES2
#ifdef GLEW_HACK
#define glProgramParameteriEXT GLEW_GET_FUN(__glewProgramParameteriEXT)
#endif
    map_enums(geom);
    map_enums(inputtype);
    //::glProgramParameteriEXT(p,geom,inputtype);
#endif
  }
  
  // uniforms
  virtual int glGetUniformLocation(int p, const char *data) { 
#ifdef GLEW_HACK
#define glGetUniformLocation GLEW_GET_FUN(__glewGetUniformLocation)
#endif

    return ::glGetUniformLocation(p,data); }
virtual int glGetAttribLocation(int program, const char *data) {
#ifdef GLEW_HACK
#define glGetAttribLocation GLEW_GET_FUN(__glewGetAttribLocation)
#endif
  return ::glGetAttribLocation(program, data);
}
virtual void glUniform1f(int loc, float val) { 
#ifdef GLEW_HACK
#define glUniform1f GLEW_GET_FUN(__glewUniform1f)
#endif
  ::glUniform1f(loc,val);
}
virtual void glUniform1i(int loc, int val) { 
#ifdef GLEW_HACK
#define glUniform1i GLEW_GET_FUN(__glewUniform1i)
#endif
  ::glUniform1i(loc,val);
}
  virtual void glUniform2f(int loc, float val, float val2) { 
#ifdef GLEW_HACK
#define glUniform2f GLEW_GET_FUN(__glewUniform2f)
#endif
    ::glUniform2f(loc,val,val2);
  }
  virtual void glUniform3f(int loc, float val, float val2, float val3) { 
#ifdef GLEW_HACK
#define glUniform3f GLEW_GET_FUN(__glewUniform3f)
#endif
    ::glUniform3f(loc, val, val2, val3);
  }
  virtual void glUniform4f(int loc, float val, float val2, float val3, float val4) { 
#ifdef GLEW_HACK
#define glUniform4f GLEW_GET_FUN(__glewUniform4f)
#endif
    ::glUniform4f(loc,val,val2,val3, val4);
  }
  virtual void glUniform1iv(int loc, int count, int *array) { 
#ifdef GLEW_HACK
#define glUniform1iv GLEW_GET_FUN(__glewUniform1iv)
#endif
    ::glUniform1iv(loc,count,array);
  }
  virtual void glUniform1fv(int loc, int count, float *array) { 
#ifdef GLEW_HACK
#define glUniform1fv GLEW_GET_FUN(__glewUniform1fv)
#endif
    ::glUniform1fv(loc,count,array); }
  virtual void glUniformMatrix4fv(int loc, unsigned int count, int boolean, const float *matrix) { 
#ifdef GLEW_HACK
#define glUniformMatrix4fv GLEW_GET_FUN(__glewUniformMatrix4fv)
#endif
    map_enums(boolean);
    ::glUniformMatrix4fv(loc,count,boolean,matrix);
  }
  virtual void glUniform3fv(int loc, int count, float *arr) { 
#ifdef GLEW_HACK
#define glUniform3fv GLEW_GET_FUN(__glewUniform3fv)
#endif
    ::glUniform3fv(loc,count,arr);
  }
  
virtual void glGetUniformfv(int p, int loc, float *arr) {
#ifdef GLEW_HACK
#define glGetUniformfv GLEW_GET_FUN(__glewGetUniformfv)
#endif
  ::glGetUniformfv(p,loc,arr);
}

  virtual void glGenerateMipmap(int p) {
#ifdef GLEW_HACK
#define glGenerateMipmap GLEW_GET_FUN(__glewGenerateMipmap)
#endif
  map_enums(p);
  ::glGenerateMipmap(p);
  }


  // Old
  virtual void glEnableClientState(int a) { /*::glEnableClientState(a);*/ }
  virtual void glLineWidth(float i) { ::glLineWidth(i); }
  virtual void glPushMatrix() { /*::glPushMatrix();*/ }
  virtual void glPopMatrix() { /*::glPopMatrix();*/ }
  virtual void glMultMatrixf(float *mat) { /*::glMultMatrixf(mat);*/ }

  // rest
  void glDepthFunc(int i) { 
    map_enums(i);
    ::glDepthFunc(i); }
  void glAlphaFunc(int i, float val) {
    map_enums(i);
    ::glAlphaFunc(i,val);
  }
  void glCullFace(int i)
  {
    map_enums(i);
    ::glCullFace(i);
  }
  void glFrontFace(int i)
  {
    map_enums(i);
    ::glFrontFace(i);
  }

  void* glMapBuffer(int a, int b)
  {
#ifdef GLEW_HACK
#define glMapBuffer GLEW_GET_FUN(__glewMapBuffer)
#endif
    std::cout << "glMapBuffer(" << a << "," << b << ")" << std::endl;
    map_enums(a);
    map_enums(b);
    std::cout << "glMapBuffer[mapped](" << a << "," << b << ")" << std::endl;
    return ::glMapBuffer(a,b);
  }
  void glUnmapBuffer(int a)
  {
#ifdef GLEW_HACK
#define glUnmapBuffer GLEW_GET_FUN(__glewUnmapBuffer)
#endif
    map_enums(a);
    ::glUnmapBuffer(a);
  }

  
  void glFramebufferTexture2D(int a, int b, int c, int d, int ptr) { 
#ifdef GLEW_HACK
#define glFramebufferTexture2D GLEW_GET_FUN(__glewFramebufferTexture2D)
#endif
    map_enums(a);
    map_enums(b);
    map_enums(c);
    ::glFramebufferTexture2D(a,b,c,d,ptr); 
}
void glGetIntegerv(int i, int *ptr) { 
    map_enums(i);
    ::glGetIntegerv(i,ptr); }
  int glCheckFramebufferStatus(int i) { 
#ifdef GLEW_HACK
#define glCheckFramebufferStatus GLEW_GET_FUN(__glewCheckFramebufferStatus)
#endif
    map_enums(i);
    int val = ::glCheckFramebufferStatus(i); 
    if (val==GL_FRAMEBUFFER_COMPLETE) val=Low_GL_FRAMEBUFFER_COMPLETE;
    return val;

      }
  void glFinish() { ::glFinish(); }
  const unsigned char *glGetString( int name ) { 
    map_enums(name);
    return ::glGetString(name); }

  void glMatrixLoadIdentityEXT(int i) { }
  void glMatrixMode(int i) { }
  void glLoadIdentity() {  }
  void glTranslatef(float,float,float) { }

};

SDLLowApi::~SDLLowApi() { }

class SDLApi : public SDLLowApi
{
  virtual void init() { }
  virtual void cleanup() {}
  virtual const char* SDL_GetError() { return ::SDL_GetError(); }
  virtual int SDL_NumJoysticks() { return ::SDL_NumJoysticks(); }
  virtual void SDL_SetHint(const char *flag, const char *str)
  {
    ::SDL_SetHint(flag,str);
  }

  virtual void SDL_GetCurrentDisplayMode(int index, Low_SDL_DisplayMode *mode)
  {
    SDL_DisplayMode m;
    ::SDL_GetCurrentDisplayMode(index, &m);
    mode->w = m.w;
    mode->h = m.h;
  }
  virtual void SDL_PauseAudio(int pause_on) {
    //::SDL_PauseAudio(pause_on);
  }
  virtual int SDL_OpenAudio(Low_SDL_AudioSpec *desired,
			    Low_SDL_AudioSpec *obtained)
  {
    int val = desired->format;
    map_enums_sdl(val);
    SDL_AudioSpec des;
    SDL_AudioSpec obt;
    des.freq = desired->freq;
    des.format = val;
    des.channels = desired->channels;
    des.silence = desired->silence;
    des.samples = desired->samples;
    des.size = desired->size;
    des.callback = desired->callback;
    des.userdata = desired->userdata;
    int val2 = ::SDL_OpenAudio(&des,&obt);
    obtained->freq = obt.freq;
    obtained->format = obt.format==AUDIO_F32LSB?Low_AUDIO_F32LSB:-1;
    obtained->channels = obt.channels;
    obtained->silence = obt.silence;
    obtained->samples = obt.samples;
    obtained->size = obt.size;
    obtained->callback = obt.callback;
    obtained->userdata = obt.userdata;
    return val2;
  }

  virtual void SDL_SetWindowSize(Low_SDL_Window *window, int w, int h)
  {
    ::SDL_SetWindowSize((SDL_Window*)(window->ptr), w,h);
  }
  virtual void SDL_SetWindowPosition(Low_SDL_Window *window, int x, int y)
  {
    ::SDL_SetWindowPosition((SDL_Window*)(window->ptr),x,y);
  }
  virtual void SDL_GetWindowSize(Low_SDL_Window *window, int *w, int *h)
    {
      ::SDL_GetWindowSize((SDL_Window*)(window->ptr),w,h);
    }
								       
  virtual void SDL_GetWindowPosition(Low_SDL_Window *window, int *x, int *y)
  {
    ::SDL_GetWindowPosition((SDL_Window*)(window->ptr),x,y);
  }
  virtual void SDL_SetWindowFullscreen(Low_SDL_Window *window, int val)
  {
    std::cout << "ERROR: SDL_SetWindowFullscreen disabled" << std::endl;
    //map_enums_sdl(val);
    //::SDL_SetWindowFullscreen((SDL_Window*)(window->ptr),val);
  }
  
  virtual char *SDL_GetClipboardText() {
    return ::SDL_GetClipboardText();
  }
  virtual int SDL_SetClipboardText(const char *text)
  {
    return ::SDL_SetClipboardText(text);
  }
  virtual void SDL_Init(int flags) { map_enums_sdl(flags); ::SDL_Init(flags);
    //std::cout << "SDL_Init:" << SDL_GetError() << std::endl;
  }
  virtual void SDL_GL_SetAttribute(int flag, int val) { map_enums_sdl(flag); map_enums_sdl(val); ::SDL_GL_SetAttribute((SDL_GLattr)flag,val); }
  virtual void* SDL_GL_GetProcAddress(char *name) { return ::SDL_GL_GetProcAddress(name); }
  virtual void SDL_Quit() { ::SDL_Quit(); }
  virtual void SDL_ConvertSurface(Low_SDL_Surface *surf, void *format, int val){ ::SDL_ConvertSurface((SDL_Surface*)surf->ptr,(SDL_PixelFormat*)format,val); }
  virtual void SDL_FreeSurface(Low_SDL_Surface *surf) { ::SDL_FreeSurface((SDL_Surface*)surf->ptr); }
  virtual void SDL_LockSurface(Low_SDL_Surface *surf) { ::SDL_LockSurface((SDL_Surface*)surf->ptr); }
  virtual void SDL_UnlockSurface(Low_SDL_Surface *surf) { ::SDL_UnlockSurface((SDL_Surface*)surf->ptr); }
  virtual void SDL_ShowCursor(bool b) { ::SDL_ShowCursor(b); }
  virtual int SDL_PollEvent(Low_SDL_Event *event) {
    if (!event) { std::cout << "LOW::SDL_PollEvent called with null pointer" << std::endl; return 0; }
    //SDL_Event e;
    SDL_EVENT_HACK h;
      h.cc=0x55555555;
    SDL_Event &e = h.e;
    int val = ::SDL_PollEvent(&e);
    if (h.cc!=0x55555555) { static bool firsttime=true; if (firsttime) { std::cout << "LOW::SDL_PollEvent write outside of Event struct (maybe you're missing -s USE_SDL=2 in compiling)" << std::endl; firsttime=false; }  }

    if (!val) { return 0; }

	if (event) {
	event->type = e.type;
   if (e.type==SDL_KEYDOWN||e.type==SDL_KEYUP) {
      event->key.keysym.sym = e.key.keysym.sym;
      if (e.key.repeat!=0) { return 0; }
    }
    if (e.type==SDL_MOUSEWHEEL) {
      event->wheel.y = e.wheel.y;
    }
    if (e.type==SDL_FINGERDOWN||e.type==SDL_FINGERUP||e.type==SDL_FINGERMOTION) {
      event->tfinger.x = e.tfinger.x;
      event->tfinger.y = e.tfinger.y;
    }
#ifndef EMSCRIPTEN
    if (e.type==SDL_DROPFILE)
      event->drop.file = e.drop.file;
#endif
    if (e.type==SDL_MOUSEBUTTONDOWN || e.type==SDL_MOUSEBUTTONUP)
      {
	event->button.button = e.button.button;
      }
    if (e.type==SDL_WINDOWEVENT)
      {
	event->window.event = e.window.event;
	event->window.data1 = e.window.data1;
	event->window.data2 = e.window.data2;
      }
    if (e.type==SDL_JOYAXISMOTION) {
        event->jaxis.value = e.jaxis.value;
	event->jaxis.axis = e.jaxis.axis;
    }
    if (e.type==SDL_JOYBALLMOTION) {
      event->jball.ball = e.jball.ball;
      event->jball.xrel = e.jball.xrel;
      event->jball.yrel = e.jball.yrel;
    }
    if (e.type==SDL_JOYBUTTONDOWN || e.type==SDL_JOYBUTTONUP) {
      event->jbutton.button = e.jbutton.button;
    }
	}
    return val;
  }
  virtual unsigned int SDL_GetTicks() { return ::SDL_GetTicks(); }
  virtual unsigned long long SDL_GetPerformanceCounter() { return ::SDL_GetPerformanceCounter(); }
  virtual unsigned long long SDL_GetPerformanceFrequency() { return ::SDL_GetPerformanceFrequency(); }

  virtual void SDL_Delay(int ms) { ::SDL_Delay(ms); }

  virtual bool SDL_GetWindowWMInfo(Low_SDL_Window *window, Low_SDL_SysWMinfo *info)
  {
    bool b = false;
#ifdef WAYLAND
    SDL_Window *win = (SDL_Window*)(window->ptr);
    SDL_SysWMinfo info2;
    SDL_VERSION(&info2.version);
    b = ::SDL_GetWindowWMInfo(win, &info2);
    info->display = info2.info.wl.display;
    info->surface = info2.info.wl.surface;
    info->shell_surface = info2.info.wl.shell_surface;
#endif
    return b;
  }

  
  virtual Low_SDL_Window* SDL_CreateWindow(const char *title, int x, int y, int width, int height, unsigned int flags2) { 
    map_enums_sdl(x);
    map_enums_sdl(y);
    map_enums_sdl(flags2);
    //std::cout << "Low_SDL_CreateWindow: " << x << " " << y << " " << flags2 << std::endl;
    Low_SDL_Window *w = new Low_SDL_Window;
    w->ptr = ::SDL_CreateWindow(title,x,y,width,height,flags2);
    return w;
  }
  virtual Low_SDL_GLContext SDL_GL_CreateContext(Low_SDL_Window *window) { 
    SDL_GLContext ctx = ::SDL_GL_CreateContext((SDL_Window*)(window->ptr));
    return ctx; 
  }
  virtual void SDL_GL_DeleteContext(Low_SDL_GLContext ctx)
  {
    SDL_GLContext ctx2 = (SDL_GLContext)ctx;
    ::SDL_GL_DeleteContext(ctx2);
  }
  virtual void SDL_UpdateWindowSurface(Low_SDL_Window *window)
  {
    ::SDL_UpdateWindowSurface((SDL_Window*)(window->ptr));
  }
  virtual void SDL_DestroyWindow(Low_SDL_Window *window)
  {
    ::SDL_DestroyWindow((SDL_Window*)(window->ptr));
  }

  virtual Low_SDL_Surface *SDL_GetWindowSurface(Low_SDL_Window *win)
  {
    SDL_Window *w = (SDL_Window*)(win->ptr);
    SDL_Surface *s = ::SDL_GetWindowSurface(w);
    if (!s) { std::cout << "Error at sdl_getwindowsurface:" << SDL_GetError() << std::endl; return 0; }
    Low_SDL_Surface *ss = new Low_SDL_Surface;
    ss->ptr = s;
    ss->w = s->w;
    ss->h = s->h;
    ss->pixels = s->pixels;
    ss->pitch = s->pitch;
    return ss;
  }
  //virtual void SDL_GL_SwapBuffers() { ::SDL_GL_SwapBuffers(); }
  virtual void SDL_GL_SwapWindow(Low_SDL_Window *window) { ::SDL_GL_SwapWindow((SDL_Window*)(window->ptr)); }
  virtual void SDL_SetWindowTitle(Low_SDL_Window *window, const char *title) { ::SDL_SetWindowTitle((SDL_Window*)(window->ptr), title); }
  virtual unsigned int SDL_GetMouseState(int *x, int *y) { return ::SDL_GetMouseState(x,y); }
  virtual unsigned int SDL_GetModState() { return (unsigned int)::SDL_GetModState(); }
  virtual int SDL_GL_MakeCurrent(Low_SDL_Window *window, Low_SDL_GLContext context)
  {
    SDL_GLContext ctx = (SDL_GLContext)context;
    return ::SDL_GL_MakeCurrent((SDL_Window*)(window->ptr), ctx);
  }
  virtual Low_SDL_Joystick* SDL_JoystickOpen(int i) { 
    Low_SDL_Joystick *data = new Low_SDL_Joystick;
    data->data = ::SDL_JoystickOpen(i); 
    return data; 
  }
  virtual void SDL_JoystickEventState(int i) 
  {
    map_enums_sdl(i);
    ::SDL_JoystickEventState(i);
  }
  virtual unsigned int SDL_JoystickGetButton(Low_SDL_Joystick *joy, int i) { return ::SDL_JoystickGetButton((SDL_Joystick*)(joy->data),i); }


  virtual Low_SDL_RWops* SDL_RWFromFile(const char *file, const char *mode)
  {
     Low_SDL_RWops *ops = new Low_SDL_RWops;
    ops->ptr = ::SDL_RWFromFile(file,mode);
    return ops;
   }

  virtual Low_SDL_RWops* SDL_RWFromMem(void *buffer, int size) { 
    Low_SDL_RWops *ops = new Low_SDL_RWops;
    ops->ptr = ::SDL_RWFromMem(buffer,size);
    return ops; 
  }
};

void map_enums_mix(int &i)
{
  switch(i) {
#ifdef USE_MIX    
  case Low_MIX_INIT_MP3: i=MIX_INIT_MP3; break;
  case Low_MIX_INIT_OGG: i=MIX_INIT_OGG; break;
  case Low_MIX_DEFAULT_FORMAT: i=MIX_DEFAULT_FORMAT; break;
  case Low_AUDIO_U8: i=AUDIO_U8; break;
#endif
  };
}

class SDLMixerApi : public SDLMixerLowApi
{
public:
  virtual void init() { }
  virtual void cleanup() { }

  virtual Low_Mix_Chunk* Mix_LoadWAV_RW(Low_SDL_RWops *buf, int s)
  {
#ifdef USE_MIX
    Mix_Chunk *m = ::Mix_LoadWAV_RW((SDL_RWops*)(buf->ptr),s);
    Low_Mix_Chunk *c = new Low_Mix_Chunk;
    c->ptr = m;
    return c;
#endif
    return 0;
  }

  virtual int Mix_OpenAudio(int rate, int flags, int val, int hup)
  {
#ifdef USE_MIX
    map_enums_mix(flags);
    return ::Mix_OpenAudio(rate,flags,val,hup);
#endif
    return 0;
  }
  virtual int Mix_PlayChannel(int channel, Low_Mix_Chunk *mix_chunk, int val)
  {
#ifdef USE_MIX
    return ::Mix_PlayChannelTimed(channel, (Mix_Chunk*)(mix_chunk->ptr), val,-1);
#endif
    return 0;
  }
  virtual void Mix_HaltChannel(int channel)
  {
#ifdef USE_MIX
    // doesn't work but halts the engine...
    //::Mix_HaltChannel(channel);
#endif
  }
  virtual Low_Mix_Chunk *Mix_QuickLoad_RAW(unsigned char *mem, int len)
  {
#ifdef USE_MIX
    Low_Mix_Chunk *c = new Low_Mix_Chunk;
    c->ptr = ::Mix_QuickLoad_RAW(mem,len);
    return c;
#endif
    return 0;
  }
  virtual void Mix_Init(int flags)
  {
#ifdef USE_MIX
    map_enums_mix(flags);
    ::Mix_Init(flags);
#endif
  }
  virtual Low_Mix_Music *Mix_LoadMUS(const char *filename)
  {
#ifdef USE_MIX
    Low_Mix_Music *m = new Low_Mix_Music;
    m->ptr = ::Mix_LoadMUS(filename);
    return m;
#endif
    return 0;
  }
  virtual void Mix_PlayMusic(Low_Mix_Music *mus, int val)
  {
#ifdef USE_MIX
    ::Mix_PlayMusic((Mix_Music*)(mus->ptr), val);
#endif
  }
  virtual int Mix_GetNumMusicDecoders()
  {
#ifdef USE_MIX
#ifndef EMSCRIPTEN
    return ::Mix_GetNumMusicDecoders();
#endif
#endif
    return 0;
  }
  virtual void Mix_GetMusicDecoder(int i)
  {
#ifdef USE_MIX
#ifndef EMSCRIPTEN
    ::Mix_GetMusicDecoder(i);
#endif
#endif
  }
  virtual void Mix_AllocateChannels(int i)
  {
#ifdef USE_MIX
     ::Mix_AllocateChannels(i);
#endif
  }

};
#endif // ndef ARM

LowApi *g_low;

void initialize_stub(int flags); 
void initialize_low(int flags)
{
  //#ifndef ARM
  LowApi *low = new LowApi;
  low->ogl = new OpenglApi;
  low->sdl = new SDLApi;
  low->sdl_mixer = new SDLMixerApi;
  g_low = low;
  //#else
  //  initialize_stub(flags);
  //#endif
  
}
