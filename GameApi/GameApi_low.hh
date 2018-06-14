
// DO NOT PUT ANY INCLUDES HERE.

#ifndef GAMEAPI_LOW_H
#define GAMEAPI_LOW_H

typedef unsigned int Low_GLuint;
typedef void Low_GLvoid;
typedef int Low_GLenum;
typedef float Low_GLfloat;
typedef unsigned int Low_GLsizei;
typedef int Low_GLint;
// old
typedef char Low_GLbyte;
typedef short Low_GLshort;
enum
  {
    Low_GL_ARRAY_BUFFER=555, // glBindBuffer 1st arg, glBufferData 1st arg, glBufferSubData 1st arg
    Low_GL_FLOAT, // glVertexAttribPointer 3rd arg
    Low_GL_INT, // glVertexAttribIPointer 3rd arg
    Low_GL_FALSE, // glVertexAttribPointer 4th arg
    Low_GL_TRIANGLES, // glDrawArrays / glDrawArraysInstanced 1st arg
    Low_GL_TRIANGLE_STRIP, // glDrawArrays 1st arg
    Low_GL_UNSIGNED_BYTE, // glVertexAttribPointer 3rd arg, glReadPixels 6tth param
    Low_GL_QUADS, // glDrawArrays
    Low_GL_DEPTH_TEST, // glEnable / glDisable
    Low_GL_STATIC_DRAW, // glBufferData 4th param
    Low_GL_DYNAMIC_DRAW, // glBufferData 4th param
    Low_GL_TEXTURE0, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE1, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE2, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE3, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE4, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE5, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE6, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE7, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE8, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE9, // glClientActiveTexture 1st param (+n)
    Low_GL_TEXTURE10, // glClientActiveTexture 1st param (+n)

    Low_GL_TEXTURE_2D, // glBindTexture, glTexImage2D
    Low_GL_TEXTURE_2D_ARRAY,
    Low_GL_TEXTURE_CUBE_MAP,
    Low_GL_LINEAR, // glTexParameteri 3th param
    Low_GL_TEXTURE_MIN_FILTER, // glTexParameteri 2nd param
    Low_GL_TEXTURE_MAG_FILTER, // glTexParameteri 2nd param
    Low_GL_TEXTURE_WRAP_S, // glTexParameteri 2nd param
    Low_GL_CLAMP_TO_EDGE, // glTexParameteri 3rd param
    Low_GL_TEXTURE_WRAP_T, // glTexParamteri 2nd param
    Low_GL_PERSPECTIVE_CORRECTION_HINT, // glHint 1st param
    Low_GL_NICEST, // hlHint 2nd param
    Low_GL_RED, // glTexImage2D 3rd param
    Low_GL_RENDERBUFFER, // glBindRenderBuffer, glRenderbufferStorage, glFramebufferRenderbuffer
    Low_GL_FRAMEBUFFER, // ..., glCheckFramebufferStatus
    Low_GL_TRUE, // glDepthMask
    Low_GL_TEXTURE_BINDING_2D, // glGetIntegerv
    Low_GL_RGBA, // glGetTexImage 3rd param, glReadPixels 5th param
    Low_GL_SCISSOR_TEST, // glEnable/glDisable
    Low_GL_FRAMEBUFFER_COMPLETE, // glCheckFramebufferStatus return value
    Low_GL_NEAREST, // glTexParameteri 3rd param
    Low_GL_DEPTH_COMPONENT16, // glRenderbufferStorage 2nd param
    Low_GL_DEPTH_ATTACHMENT, // glFramebufferRenderbuffer 2nd param
    Low_GL_COLOR_ATTACHMENT0, // glFramebufferTexture2D 2nd param
    Low_GL_VIEWPORT, // glGetIntegerv
    Low_GL_SRC_ALPHA, // glBlendFunc 1st param
    Low_GL_ONE_MINUS_SRC_ALPHA, // glBlendFunc 2nd param
    Low_GL_ZERO,
    Low_GL_ONE,
    Low_GL_SRC_COLOR,
    Low_GL_ONE_MINUS_SRC_COLOR,
    Low_GL_DST_COLOR,
    Low_GL_ONE_MINUS_DST_COLOR,
    //GL_ONE_MINUS_SRC_ALPHA,
    Low_GL_DST_ALPHA,
    Low_GL_ONE_MINUS_DST_ALPHA,
    Low_GL_CONSTANT_COLOR,
    Low_GL_ONE_MINUS_CONSTANT_COLOR,
    Low_GL_CONSTANT_ALPHA, // glBlendFunc 1st AND 2ND ARG
    Low_GL_LIGHTING, // glDisable/glEnable
    Low_GL_MODELVIEW, // glMatrixMode
    Low_GL_TEXTURE_ENV, // glTexEnvi
    Low_GL_BLEND, // glEnable
    Low_GL_ALWAYS, // glStencilFunc
    Low_GL_KEEP, // glStencilOp 1,2
    Low_GL_REPLACE, // glStencilOp
    Low_GL_NOTEQUAL, // glStencilFunc
    Low_GL_FRONT_AND_BACK, // glPolygonMode 1st param
    Low_GL_LINE,  // glPolygonMode 2nd param
    Low_GL_FILL, // glPolygonMode 2nd param
    Low_GL_STENCIL_TEST, // glDisable 1st
    Low_GL_LEQUAL, // glDepthFunc
    Low_GL_EQUAL,
    //GL_ALWAYS,
    Low_GL_LESS,
    Low_GL_GREATER,
    Low_GL_GEQUAL,
    Low_GL_TEXTURE_ENV_MOVE,
    Low_GL_LINE_SMOOTH,
    Low_GL_LINES,
    Low_GL_POINTS,
    Low_GL_TEXTURE_ENV_MODE,
    Low_GL_TEXTURE_WIDTH,
    Low_GL_TEXTURE_HEIGHT,
    Low_GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    Low_GL_TEXTURE_WRAP_R,
    Low_GL_REPEAT,
    Low_GL_RGBA8,
    Low_GL_MULTISAMPLE,
    Low_GL_GEOMETRY_SHADER, // todo
    Low_GL_VERTEX_SHADER,
    Low_GL_FRAGMENT_SHADER,
    Low_GL_COMPILE_STATUS,
    Low_GL_LINES_ADJACENCY_EXT,
    Low_GL_TRIANGLES_ADJACENCY_EXT,
    Low_GL_LINE_STRIP,
    Low_GL_GEOMETRY_INPUT_TYPE_EXT,
    Low_GL_GEOMETRY_VERTICES_OUT_EXT,
    Low_GL_NO_ERROR,
    Low_GL_GEOMETRY_OUTPUT_TYPE_EXT,
    //
    //  OLD STUFF
    // 
    Low_GL_VERTEX_ARRAY,
    Low_GL_NORMAL_ARRAY,
    Low_GL_COLOR_ARRAY,
    Low_GL_TEXTURE_COORD_ARRAY,
    Low_GL_POLYGON,
    Low_GL_SHORT,
    Low_GL_COMPILE,
  };
enum {
    Low_GL_COLOR_BUFFER_BIT=1, // glClear
    Low_GL_DEPTH_BUFFER_BIT=2, // glClear
    Low_GL_STENCIL_BUFFER_BIT=4, // glClear
};

#undef glActiveTexture
#undef glClientActiveTexture
#undef glTexStorage3d
#undef glTexSubImage3d
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
#undef glGetAttribLocation
#undef glDepthFunc
#undef glDrawArrays
#undef glTexImage2D

#undef Mix_PlayChannel


class OpenglLowApi
{
public:
  virtual void init()=0;
  virtual void cleanup()=0;

  virtual int glGetError()=0;
  
  virtual void glColor4ub(int r, int g, int b, int a)=0;
  virtual void glColor4f(float r, float g, float b, float a)=0;
  virtual void glClearColor(float r, float g, float b, float a)=0;
  virtual void glClear(int bits)=0;
  virtual void glDepthMask(int a)=0;
  virtual void glDepthFunc(int i)=0;
  virtual void glStencilMask(int a)=0;
  virtual void glStencilOp(int keep, int keep2, int repl)=0;
  virtual void glStencilFunc(int notequal, int, int)=0;
  virtual void glPolygonMode(int front_and_back, int gl_line)=0;
  virtual void glClearStencil(int val)=0;
  virtual void glTexEnvi(int a, int mode, int rep)=0;

  // attribs
  virtual void glEnable(int val)=0;
  virtual void glDisable(int val)=0;
  virtual void glGetFloatv(int val, float *params)=0;

  // viewports
  virtual void glViewport(int x, int y, unsigned int w, unsigned int h)=0;

  // textures
  virtual void glGenTextures(int val, unsigned int *tex)=0;
  virtual void glDeleteTextures(int val, const unsigned int *tex)=0;
  virtual void glBindTexture(int ID, int tex)=0;
  virtual void glTexImage2D(int ID, int, int,unsigned int,unsigned int, int, int, int, void *ptr)=0;
  virtual void glCopyTexImage2D(int ID, int,int,int,int, int,int, int ptr)=0;
  virtual void glTexParameteri(int ID, int,int)=0;
  virtual void glHint(int hint, int value)=0;
  virtual void glActiveTexture(int a)=0;
  virtual void glClientActiveTexture(int a)=0;
  virtual void glTexStorage3D(int arr, int, int flag, int w, int h, int layer_count)=0;
  virtual void glTexSubImage3D(int arr, int,int,int,int,int,int, int, int rgba, int unsig_byte, void *buffer)=0;
  virtual void glGetTexLevelParameteriv(int a, int, int w, int *ptr)=0;
  virtual void glGetTexImage(int a, int, int rgba, int unsign_byte, void *ptr)=0;
  virtual void glReadBuffer(int a)=0;
  

  // blend
  virtual void glBlendFunc(int val, int val2)=0;
  
  // scissor
  virtual void glScissor(float x, float y, float w, float h)=0;

  // fbo
  //virtual void glGenRenderbuffers(int i, unsigned int *ptr)=0;
  virtual void glFramebufferTexture2D(int i, int atth, int tex2d, int tex, int ptr)=0;

  // vertex arrays
  virtual void glGenBuffers(int i, unsigned int *buffers)=0;
  virtual void glDeleteBuffers(int count, const unsigned int *buffers)=0;
  virtual void glBindBuffer(int a, unsigned int data)=0;
  virtual void glBufferData(int a, int size, const void *ptr, int static_draw)=0;
  virtual void glBufferSubData(int a, int, int, const void*)=0;
  virtual void glVertexAttribPointer(int, int, int gl_float, int boolean, unsigned int, const void *ptr)=0;
  virtual void glVertexAttribIPointer(int, int, int gl_float, int boolean, const void *ptr)=0;
  virtual void glVertexAttribDivisor(int, int)=0;
  virtual void glGenVertexArrays(int i, unsigned int *arr)=0;
  virtual void glDeleteVertexArrays(int count, unsigned int *vao)=0;
  virtual void glBindVertexArray(int vao)=0;
  virtual void glEnableVertexAttribArray(int a)=0;
  virtual void glDisableVertexAttribArray(int a)=0;
  virtual void glDrawArraysInstanced(int tri, int, unsigned int, unsigned int)=0;
  virtual void glDrawArrays(int tri, int, unsigned int)=0;
  
  // bitmaps
  virtual void glReadPixels(int x, int y, int w, int h, int rgba, int mode, void *ptr)=0;
  
  // fbo
  virtual void glGenFramebuffers(int i, unsigned int *fbo_id)=0;
  virtual void glGenRenderbuffers(int i, unsigned int *rbo_id)=0;
  virtual void glDeleteRenderBuffers(int i, unsigned int *rbo)=0;
  virtual void glDeleteFrameBuffers(int i, unsigned int *fbo)=0;
  virtual void glBindFramebuffer(int a, int fbo_id)=0;
  virtual void glBindRenderbuffer(int a, int fbo_id)=0;
  virtual void glRenderbufferStorage(int a, int d, int sx, int sy)=0;
  virtual void glFramebufferRenderbuffer(int a, int d, int r, unsigned int tex)=0;
  
  // shaders
  virtual unsigned int glCreateShader(int shader)=0;
  virtual void glShaderSource(int h, int c, const char **strings, int *lengths)=0;
  virtual void glCompileShader(int h)=0;
  virtual void glGetShaderInfoLog(unsigned int h, int val, int *length, char *buf)=0;
  virtual void glGetShaderiv(int handle, int gl_compile_status, int *ptr)=0;
  virtual void glDeleteShader(int h)=0;
  
  // programs
  virtual int glCreateProgram()=0;
  virtual void glDeleteProgram(int p)=0;
 virtual  void glAttachShader(int p, int h)=0; 
  virtual void glDetachShader(int p, int h)=0;
  virtual void glLinkProgram(int p)=0;
  virtual void glUseProgram(int p)=0;
  virtual void glGetProgramInfoLog(unsigned int p, int num, int *len, char *buf)=0;
  virtual void glBindFragDataLocation(int p, int num, const char *data)=0;
  virtual void glBindAttribLocation(int p, int num, const char *data)=0;
  virtual void glProgramParameteriEXT(int p, int geom, int inputtype)=0;
  
  // uniforms
  virtual int glGetUniformLocation(int p, const char *data)=0;
  virtual int glGetAttribLocation(int p, const char *data)=0;
  virtual void glUniform1f(int loc, float val)=0;
  virtual void glUniform1i(int loc, int val)=0;
  virtual void glUniform2f(int loc, float val, float val2)=0;
  virtual void glUniform3f(int loc, float val, float val2, float val3)=0;
  virtual void glUniform4f(int loc, float val, float val2, float val3, float val4)=0;
  virtual void glUniform1iv(int loc, int count, int *array)=0;
  virtual void glUniform1fv(int loc, int count, float *array)=0;
  virtual void glUniformMatrix4fv(int loc, unsigned int count, int boolean, const float *matrix)=0;
  virtual void glUniform3fv(int loc, int count, float *arr)=0;
  virtual void glGetUniformfv(int p, int loc, float *arr)=0;
  virtual void glGetIntegerv(int p, int *ptr)=0;
  virtual int glCheckFramebufferStatus(int id)=0;

  virtual void glFinish()=0;

  // Old
  virtual void glEnableClientState(int a)=0;
  virtual void glLineWidth(float i)=0;
  virtual void glPushMatrix()=0;
  virtual void glPopMatrix()=0;
  virtual void glMultMatrixf(float *mat)=0;
  virtual void glMatrixLoadIdentityEXT(int i)=0;
  virtual void glMatrixMode(int i)=0;
  virtual void glLoadIdentity()=0;
  virtual void glTranslatef(float, float, float)=0;
  virtual void glDisableClientState(int) { }
  virtual void glTexCoordPointer(int,int,int, void*) { }
  virtual void glColorPointer(int,int,int,void*) { }
  virtual void glVertexPointer(int,int,int,void*) { }
  virtual void glNormalPointer(int,int,void*) { }
  virtual void glRotatef(float,float,float,float) {}
  //virtual void glEnableClientState(int) { }
  //virtual void glDisableClientState(int) { }
  virtual void glColor3f(float,float,float) { }
  virtual void glCallList(int) { }
  virtual void glEndList() { }
  virtual void glEnd() { }
  virtual void glVertex3f(float,float,float) { }
  virtual void glNormal3f(float,float,float) { }
  virtual void glTexCoord2f(float,float) { }
  virtual void glVertexAttrib1s(int loc, short s) {}
  virtual void glVertexAttrib1f(int loc, float s) {}
  virtual void glBegin(int) { }
  virtual void glNewList(int i, int k) { }
  virtual int glGenLists(int) { return 0; }
  virtual void glDeleteLists(int,int) {}
};

class FontLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
};

class OpenVRLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
};

class EmscriptenLowApi
{
public:
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void emscripten_set_main_loop_arg(void (*)(void*), void*, int, int)=0;
  virtual void emscripten_async_wget_data(void *buf2, void *data, void (*cb_sucess)(void*), void (*cb_fail)(void*))=0;
  virtual bool emscripten_has_threading_support()=0;
  virtual void emscripten_async_call(void (*)(void*), void *data, int val)=0;
  virtual void emscripten_set_click_callback(char *c, int a, bool true_, void (*)())=0;
  virtual void emscripten_set_touchend_callback(char *c, int a, bool true_, void (*)())=0;

};
class EmscriptenVRLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void emscripten_vr_init(void (*)(void*), void*)=0;
  virtual void emscripten_vr_submit_frame(int disp)=0;
  virtual void emscripten_vr_get_frame_data(int disp, void *data)=0;
  virtual bool emscripten_vr_display_presenting(int disp)=0;
  virtual void emscripten_vr_set_display_render_loop_arg(int disp, void (*)(void*), void* data)=0;
  virtual void emscripten_vr_exit_present(int disp)=0;
  virtual void emscripten_vr_cancel_display_render_loop(int disp)=0;
  virtual void emscripten_vr_request_present(int disp, void *init, int, void (*cb)(void*), void *data)=0;
  virtual bool emscripten_vr_ready()=0;
  virtual int emscripten_vr_count_displays()=0;
  virtual int emscripten_vr_get_display_handle(int i)=0;
  virtual char *emscripten_vr_get_display_name( int d ) =0;
  virtual int emscripten_vr_get_display_capabilities( int d, void *caps )=0;
  virtual int emscripten_vr_display_connected( int d) =0;
  virtual void emscripten_vr_get_eye_parameters(int d, int left, void *ptr)=0;
};

class FileSystemLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void popen(char *name, char *flags);
};

class ImageLoadLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;

};

class SDLLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void SDL_Init(int flags)=0;
  virtual void SDL_GL_SetAttribute(int flag, int val)=0;
  virtual void* SDL_GL_GetProcAddress(char *name)=0;
  virtual void SDL_Quit()=0;
  virtual void SDL_ConvertSurface(void *surf, void *format, int val)=0;
  virtual void SDL_FreeSurface(void *surf)=0;
  virtual void SDL_LockSurface(void *surf)=0;
  virtual void SDL_UnlockSurface(void *surf)=0;
  virtual void SDL_ShowCursor(bool b)=0;
  virtual void SDL_PollEvent(void *event)=0;
  virtual unsigned int SDL_GetTicks()=0;
  virtual void SDL_Delay(int ms)=0;
  virtual void* SDL_CreateWindow(char *title, int flags, int width, int height, int flags2)=0;
  virtual void* SDL_GL_CreateContext(void *window)=0;
  virtual void SDL_GL_SwapBuffers()=0;
  virtual void SDL_GL_SwapWindow(void *window)=0;
  virtual void SDL_SetWindowTitle(void *window, char *title)=0;
  virtual void SDL_GetMouseState(int *x, int *y)=0;
  virtual void* SDL_GetModState()=0;
  virtual void* SDL_JoystickOpen(int i)=0;
  virtual unsigned int SDL_JoystickGetButton(void*joy, int i)=0;
  virtual void* SDL_RWFromMem(void *buffer, int size)=0;
};

class SDLMixerLowApi
{
public:
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void* Mix_LoadWav_RW(void *buf, int s)=0;
  virtual int Mix_OpenAudio(int rate, int flags, int val, int hup)=0;
  virtual int Mix_PlayChannel(int channel, void *mix_chunk, int val)=0;
  virtual void Mix_Init(int flags)=0;
  virtual void Mix_GetNumMusicDecoders()=0;
  virtual void Mix_GetMusicDecider(int i)=0;
  virtual void Mix_AllocateChannels(int i)=0;
};

class SDLImageLowApi
{
  virtual void init()=0;
  virtual void cleanup()=0;
};

struct LowApi
{
  LowApi() : ogl(0), fnt(0), ems(0), fs(0), img(0), sdl(0), sdl_image(0), ovr(0), ems_vr(0) { }
  OpenglLowApi *ogl;
  FontLowApi *fnt;
  EmscriptenLowApi *ems;
  FileSystemLowApi *fs;
  ImageLoadLowApi *img;
  SDLLowApi *sdl;
  SDLImageLowApi *sdl_image;
  OpenVRLowApi *ovr;
  EmscriptenVRLowApi *ems_vr;
};

extern LowApi *g_low;

enum { EStandard, EWeb };
void initialize_low(int flags);

#endif
