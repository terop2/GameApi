
#include "GameApi_low.hh"

#define VIRTUAL_REALITY 1
#define SDL2_USED  
#define GAME_API_DEF
#define _SCL_SECURE_NO_WARNINGS
#ifndef EMSCRIPTEN
#define THREADS 1
#endif
#define BATCHING 1

#ifndef EMSCRIPTEN
#define VAO 1
#endif


#define NO_SDL_GLEXT 
#ifndef DEPS
#include <GL/glew.h> 
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
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


#ifdef WINDOWS
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
#undef glUniform2f
#undef glUniform3f
#undef glUniform4f
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

#undef Mix_PlayChannel


class OpenglApi : public OpenglLowApi
{
public:
  virtual void init() { }
  virtual void cleanup() { }

  virtual void glColor4ub(int r, int g, int b, int a) { ::glColor4ub(r,g,b,a); }
  virtual void glColor4f(float r, float g, float b, float a) { ::glColor4f(r,g,b,a); }
  virtual void glClearColor(float r, float g, float b, float a) { ::glClearColor(r,g,b,a); }
  virtual void glClear(int bits) { ::glClear(bits); }
  virtual void glDepthMask(int a) { ::glDepthMask(a); }
  virtual void glStencilMask(int a) { ::glStencilMask(a); }
  virtual void glStencilOp(int keep, int keep2, int repl) { ::glStencilOp(keep,keep2, repl); }
  virtual void glStencilFunc(int notequal, int a, int b) { ::glStencilFunc(notequal, a,b); }
  virtual void glPolygonMode(int front_and_back, int gl_line) { return ::glPolygonMode(front_and_back,gl_line); }
  virtual void glClearStencil(int val) { ::glClearStencil(val); }
  virtual void glTexEnvi(int a, int mode, int rep) { ::glTexEnvi(a,mode,rep); }

  // attribs
  virtual void glEnable(int val) { ::glEnable(val); }
  virtual void glDisable(int val) { ::glDisable(val); }
  virtual void glGetFloatv(int val, float *params) { ::glGetFloatv(val,params); }

  // viewports
  virtual void glViewport(int x, int y, int w, int h) { ::glViewport(x,y,w,h); }

  // textures
  virtual void glGenTextures(int val, unsigned int *tex) { ::glGenTextures(val,tex); }
  virtual void glDeleteTextures(int val, const unsigned int *tex) { ::glDeleteTextures(val,tex); }
  virtual void glBindTexture(int ID, int tex) { ::glBindTexture(ID,tex); }
  virtual void glTexImage2D(int ID, int a, int b,int c,int d, int e, int f, int g, void *ptr) { ::glTexImage2D(ID,a,b,c,d,e,f,g,ptr); }
  virtual void glCopyTexImage2D(int ID, int a,int b,int c,int d, int e,int f, int ptr) { ::glCopyTexImage2D(ID,a,b,c,d,e,f,ptr); }
  virtual void glTexParameteri(int ID, int a,int b) { ::glTexParameteri(ID,a,b); }
  virtual void glHint(int hint, int value) { ::glHint(hint,value); }
  virtual void glActiveTexture(int a) { 
#ifdef GLEW_HACK
#define glActiveTexture GLEW_GET_FUN(__glewActiveTexture)
#endif
    ::glActiveTexture(a); 
  }
  virtual void glClientActiveTexture(int a) { 
#ifdef GLEW_HACK
#define glClientActiveTexture GLEW_GET_FUN(__glewClientActiveTexture)
#endif
::glClientActiveTexture(a); 
  }
  virtual void glTexStorage3D(int arr, int a, int flag, int w, int h, int layer_count) { 
#ifdef GLEW_HACK
#define glTexStorage3D GLEW_GET_FUN(__glewTexStorage3D)
#endif
::glTexStorage3D(arr,a,flag,w,h,layer_count); 
}
  virtual void glTexSubImage3D(int arr, int a,int b,int c,int d,int e,int f, int g, int rgba, int unsig_byte, void *buffer) { 
#ifdef GLEW_HACK
#define glTexSubImage3D GLEW_GET_FUN(__glewTexSubImage3D)
#endif
::glTexSubImage3D(arr,a,b,c,d,e,f,g,rgba,unsig_byte,buffer); 
  }
  virtual void glGetTexLevelParameteriv(int a, int b, int w, int *ptr) { ::glGetTexLevelParameteriv(a,b,w,ptr); }
  virtual void glGetTexImage(int a, int b, int rgba, int unsign_byte, void *ptr) { ::glGetTexImage(a,b,rgba,unsign_byte,ptr); }
  virtual void glReadBuffer(int a) { ::glReadBuffer(a); }
  

  // blend
  virtual void glBlendFunc(int val, int val2) { ::glBlendFunc(val,val2); }
  
  // scissor
  virtual void glScissor(float x, float y, float w, float h) { ::glScissor(x,y,w,h); }

  // vertex arrays
  virtual void glGenBuffers(int i, unsigned int *buffers) { 
#ifdef GLEW_HACK
#define glGenBuffers GLEW_GET_FUN(__glewGenBuffers)
#endif
::glGenBuffers(i,buffers); 
  }
  virtual void glDeleteBuffers(int count, const unsigned int *buffers) { 
#ifdef GLEW_HACK
#define glDeleteBuffers GLEW_GET_FUN(__glewDeleteBuffers)
#endif
::glDeleteBuffers(count,buffers); 
  }
  virtual void glBindBuffer(int a, unsigned int data) { 
#ifdef GLEW_HACK
#define glBindBuffer GLEW_GET_FUN(__glewBindBuffer)
#endif
::glBindBuffer(a,data); 
  }
  virtual void glBufferData(int a, int size, const void *ptr, int static_draw) { 
#ifdef GLEW_HACK
#define glBufferData GLEW_GET_FUN(__glewBufferData)
#endif
    ::glBufferData(a,size,ptr,static_draw); 
  }
  virtual void glBufferSubData(int a, int b, int c, const void* d) { 
#ifdef GLEW_HACK
#define glBufferSubData GLEW_GET_FUN(__glewBufferSubData)
#endif
::glBufferSubData(a,b,c,d);
  }
  virtual void glVertexAttribPointer(int a, int b, int gl_float, int boolean, int c, const void *ptr) { 
#ifdef GLEW_HACK
#define glVertexAttribPointer GLEW_GET_FUN(__glewVertexAttribPointer)
#endif
::glVertexAttribPointer(a,b,gl_float,boolean,c,ptr); 
  }
virtual void glVertexAttribIPointer(int a, int b, int gl_float, int boolean, const void *ptr) {
#ifdef GLEW_HACK
#define glVertexAttribIPointer GLEW_GET_FUN(__glewVertexAttribIPointer)
#endif
::glVertexAttribIPointer(a,b,gl_float,boolean,ptr); 
}
  virtual void glVertexAttribDivisor(int a, int b) { 
#ifdef GLEW_HACK
#define glVertexAttribDivisor GLEW_GET_FUN(__glewVertexAttribDivisor)
#endif
    ::glVertexAttribDivisor(a,b); 
  }
  virtual void glGenVertexArrays(int i, unsigned int *arr) { 
#ifdef GLEW_HACK
#define glGenVertexArrays GLEW_GET_FUN(__glewGenVertexArrays)
#endif
    ::glGenVertexArrays(i,arr); 
  }
  virtual void glDeleteVertexArrays(int count, unsigned int *vao) { 
#ifdef GLEW_HACK
#define glDeleteVertexArrays GLEW_GET_FUN(__glewDeleteVertexArrays)
#endif
::glDeleteVertexArrays(count,vao); 
  }
  virtual void glBindVertexArray(int vao) { 
#ifdef GLEW_HACK
#define glBindVertexArray GLEW_GET_FUN(__glewBindVertexArray)
#endif
::glBindVertexArray(vao); 
  }
  virtual void glEnableVertexAttribArray(int a) { 
#ifdef GLEW_HACK
#define glEnableVertexAttribArray GLEW_GET_FUN(__glewEnableVertexAttribArray)
#endif
::glEnableVertexAttribArray(a); 
  }
  virtual void glDisableVertexAttribArray(int a) { 
#ifdef GLEW_HACK
#define glDisableVertexAttribArray GLEW_GET_FUN(__glewDisableVertexAttribArray)
#endif
::glDisableVertexAttribArray(a); 
  }
  virtual void glDrawArraysInstanced(int tri, int a, int b, int c) { 
#ifdef GLEW_HACK
#define glDrawArraysInstanced GLEW_GET_FUN(__glewDrawArraysInstanced)
#endif
::glDrawArraysInstanced(tri,a,b,c); 
  }
  virtual void glDrawArrays(int tri, int a, int b) { ::glDrawArrays(tri,a,b); }
  
  // bitmaps
  virtual void glReadPixels(int x, int y, int w, int h, int rgba, int mode, void *ptr) { ::glReadPixels(x,y,w,h,rgba,mode,ptr); }
  
  // fbo
  virtual void glGenFramebuffers(int i, unsigned int *fbo_id) {
#ifdef GLEW_HACK
#define glGenFramebuffers GLEW_GET_FUN(__glewGenFramebuffers) 
#endif
::glGenFramebuffers(i,fbo_id); }
  virtual void glGenRenderBuffers(int i, unsigned int *rbo_id) { ::glGenRenderbuffers(i,rbo_id); }
  virtual void glDeleteRenderBuffers(int i, unsigned int *rbo) { ::glDeleteRenderbuffers(i,rbo); }
  virtual void glDeleteFrameBuffers(int i, unsigned int *fbo) { ::glDeleteFramebuffers(i,fbo); }
  virtual void glBindFramebuffer(int a, int fbo_id) { 
#ifdef GLEW_HACK
#define glBindFramebuffer GLEW_GET_FUN(__glewBindFramebuffer)
#endif
::glBindFramebuffer(a,fbo_id); }
  virtual void glBindRenderbuffer(int a, int fbo_id) { 
#ifdef GLEW_HACK
#define glBindRenderbuffer GLEW_GET_FUN(__glewBindRenderbuffer)
#endif
::glBindRenderbuffer(a,fbo_id); }
  virtual void glRenderbufferStorage(int a, int d, int sx, int sy) { 
#ifdef GLEW_HACK
#define glRenderbufferStorage GLEW_GET_FUN(__glewRenderbufferStorage)
#endif
::glRenderbufferStorage(a,d,sx,sy); }
  virtual void glFramebufferRenderbuffer(int a, int d, int r, unsigned int tex) { 
#ifdef GLEW_HACK
#define glFramebufferRenderbuffer GLEW_GET_FUN(__glewFramebufferRenderbuffer)
#endif
::glFramebufferRenderbuffer(a,d,r,tex); }
  
  // shaders
  virtual void glCreateShader(int shader) { 
#ifdef GLEW_HACK
#define glCreateShader GLEW_GET_FUN(__glewCreateShader)
#endif
::glCreateShader(shader); }
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
::glGetShaderiv(handle,gl_compile_status, ptr); }
  virtual void glDeleteShader(int h) { 
#ifdef GLEW_HACK
#define glDeleteShader GLEW_GET_FUN(__glewDeleteShader)
#endif
::glDeleteShader(h); }
  
  // programs
  virtual int glCreateProgram() { return 
#ifdef GLEW_HACK
#define glCreateProgram GLEW_GET_FUN(__glewCreateProgram)
#endif
      ::glCreateProgram(); }
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
  virtual void glGetProgramInfoLog(unsigned int p, int num, int *len, char *buf) { 
#ifdef GLEW_HACK
#define glGetProgramInfoLog GLEW_GET_FUN(__glewGetProgramInfoLog)
#endif
::glGetProgramInfoLog(p,num,len,buf); }
  virtual void glBindFragDataLocation(int p, int num, char *data) { 
#ifdef GLEW_HACK
#define glBindFragDataLocation GLEW_GET_FUN(__glewBindFragDataLocation)
#endif
::glBindFragDataLocation(p,num,data); }
  virtual void glBindAttribLocation(int p, int num, char *data) { 
#ifdef GLEW_HACK
#define glBindAttribLocation GLEW_GET_FUN(__glewBindAttribLocation)
#endif
::glBindAttribLocation(p,num,data); 
  }
  virtual void glProgramParameteriEXT(int p, int geom, int inputtype) { 
#ifdef GLEW_HACK
#define glProgramParameteriEXT GLEW_GET_FUN(__glewProgramParameteriEXT)
#endif
::glProgramParameteriEXT(p,geom,inputtype); }
  
  // uniforms
  virtual int glGetUniformLocation(int p, char *data) { 
#ifdef GLEW_HACK
#define glGetUniformLocation GLEW_GET_FUN(__glewGetUniformLocation)
#endif
return ::glGetUniformLocation(p,data); }
  virtual void glUniform1f(int loc, float val) { 
#ifdef GLEW_HACK
#define glUniform1f GLEW_GET_FUN(__glewUniform1f)
#endif
::glUniform1f(loc,val); }
  virtual void glUniform2f(int loc, float val, float val2) { 
#ifdef GLEW_HACK
#define glUniform2f GLEW_GET_FUN(__glewUniform2f)
#endif
::glUniform2f(loc,val,val2); }
  virtual void glUniform3f(int loc, float val, float val2, float val3) { 
#ifdef GLEW_HACK
#define glUniform3f GLEW_GET_FUN(__glewUniform3f)
#endif
::glUniform3f(loc, val, val2, val3); }
  virtual void glUniform4f(int loc, float val, float val2, float val3, float val4) { 
#ifdef GLEW_HACK
#define glUniform4f GLEW_GET_FUN(__glewUniform4f)
#endif
::glUniform4f(loc,val,val2,val3, val4); }
  virtual void glUniform1iv(int loc, int count, int *array) { 
#ifdef GLEW_HACK
#define glUniform1iv GLEW_GET_FUN(__glewUniform1iv)
#endif
::glUniform1iv(loc,count,array); }
  virtual void glUniform1fv(int loc, int count, float *array) { 
#ifdef GLEW_HACK
#define glUniform1fv GLEW_GET_FUN(__glewUniform1fv)
#endif
::glUniform1fv(loc,count,array); }
  virtual void glUniformMatrix4fv(int loc, int count, int boolean, float *matrix) { 
#ifdef GLEW_HACK
#define glUniformMatrix4fv GLEW_GET_FUN(__glewUniformMatrix4fv)
#endif
::glUniformMatrix4fv(loc,count,boolean,matrix); }
  virtual void glUniform3fv(int loc, int count, float *arr) { 
#ifdef GLEW_HACK
#define glUniform3fv GLEW_GET_FUN(__glewUniform3fv)
#endif
::glUniform3fv(loc,count,arr); }
  


  // Old
  virtual void glEnableClientState(int a) { ::glEnableClientState(a); }
  virtual void glLineWidth(float i) { ::glLineWidth(i); }
  virtual void glPushMatrix() { ::glPushMatrix(); }
  virtual void glPopMatrix() { ::glPopMatrix(); }
  virtual void glMultMatrixf(float *mat) { ::glMultMatrixf(mat); }

};
LowApi *g_low;
 
void initialize_low(int flags)
{
  LowApi *low = new LowApi;
  low->ogl = new OpenglApi;

  g_low = low;
}
