
class OpenglLowApi
{
public:
  virtual void init()=0;
  virtual void cleanup()=0;
  virtual void glColor4ub(int r, int g, int b, int a)=0;
  virtual void glColor4f(float r, float g, float b, float a)=0;
  virtual void glClearColor(float r, float g, float b, float a)=0;
  virtual void glClear(int bits)=0;
  virtual void glDepthMask(int a)=0;
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
  virtual void glViewport(int x, int y, int w, int h)=0;

  // textures
  virtual void glGenTextures(int val, int *tex)=0;
  virtual void glDeleteTextures(int val, int *tex)=0;
  virtual void glBindTexture(int ID, int tex)=0;
  virtual void glTexImage2D(int ID, int, int,int,int, int, int, int, void *ptr)=0;
  virtual void glCopyTexImage2D(int ID, int,int,int,int, int,int, void* ptr)=0;
  virtual void glTexParameteri(int ID, int,int)=0;
  virtual void glHint(int hint, int value)=0;
  virtual void glActiveTexture(int a)=0;
  virtual void glClientActiveTexture(int a)=0;
  virtual void glTexStorage3d(int arr, int, int flag, int w, int h, int layer_count)=0;
  virtual void glTexSubImage3d(int arr, int,int,int,int,int,int, int, int rgba, int unsig_byte, void *buffer)=0;
  virtual void glGetTexLevelParameteriv(int a, int, int w, int *ptr)=0;
  virtual void glGetTexImage(int a, int, int rgba, int unsign_byte, void *ptr)=0;
  virtual void glReadBuffer(int a)=0;
  

  // blend
  virtual void glBlendFunc(int val, int val2)=0;
  
  // scissor
  virtual void glScissor(float x, float y, float w, float h)=0;

  // vertex arrays
  virtual void glGenBuffers(int i, int *buffers)=0;
  virtual void glDeleteBuffers(int count, int *buffers)=0;
  virtual void glBindBufer(int a, void *data)=0;
  virtual void glBufferData(int a, int size, void *ptr, int static_draw)=0;
  virtual void glBufferSubData(int a, int, int, void*)=0;
  virtual void glVertexAttribPointer(int, int, int gl_float, int boolean, int, void *ptr)=0;
  virtual void glVertexAttribDivisor(int, int)=0;
  virtual void glGenVertexArrays(int i, int *arr)=0;
  virtual void glDeleteVertexArrays(int count, int *vao)=0;
  virtual void glBindVertexArray(int vao)=0;
  virtual void glEnableVertexAttribArray(int a)=0;
  virtual void glDisableVertexAttribArray(int a)=0;
  virtual void glDrawArraysInstanced(int tri, int, int, int)=0;
  virtual void glDrawArrays(int tri, int, int)=0;
  
  // bitmaps
  virtual void glReadPixels(int x, int y, int w, int h, int rgba, int mode, void *ptr)=0;
  
  // fbo
  virtual void glGenFramebuffers(int i, int *fbo_id)=0;
  virtual void glGenRenderBuffers(int i, int *rbo_id)=0;
  virtual void glDeleteRenderBuffers(int i, int *rbo)=0;
  virtual void glDeleteFrameBuffers(int i, int *fbo)=0;
  virtual void glBindFramebuffer(int a, int fbo_id)=0;
  virtual void glBindRenderbuffer(int a, int fbo_id)=0;
  virtual void glRenderbufferStorage(int a, int d, int sx, int sy)=0;
  virtual void glFramebufferRenderbuffer(int a, int d, int r, void *tex)=0;
  
  // shaders
  virtual void glCreateShader(int shader)=0;
  virtual void glShaderSource(int h, int c, char **strings, int *lengths)=0;
  virtual void glCompileShader(int h)=0;
  virtual void glGetShaderInfoLog(int h, int val, int *length, char *buf)=0;
  virtual void glGetShaderiv(int handle, int gl_compile_status, int *ptr)=0;
  virtual void glDeleteShader(int h)=0;
  
  // programs
  virtual int glCreateProgram()=0;
  virtual void glDeleteProgram(int p)=0;
 virtual  void glAttachShader(int p, int h)=0; 
  virtual void glDetachShader(int p, int h)=0;
  virtual void glLinkProgram(int p)=0;
  virtual void glUseProgram(int p)=0;
  virtual void glGetProgramInfoLog(int p, int num, int len, char *buf)=0;
  virtual void glBindFragDataLocation(int p, int num, char *data)=0;
  virtual void glBindAttribLocation(int p, int num, char *data)=0;
  virtual void glProgramParameteriEXT(int p, int geom, int inputtype)=0;
  
  // uniforms
  virtual int glGetUniformLocation(int p, char *data)=0;
  virtual void glUniform1f(int loc, float val)=0;
  virtual void glUniform2f(int loc, float val, float val2)=0;
  virtual void glUniform3f(int loc, float val, float val2, float val3)=0;
  virtual void glUniform4f(int loc, float val, float val2, float val3, float val4)=0;
  virtual void glUniform1iv(int loc, int count, int *array)=0;
  virtual void glUniform1fv(int loc, int count, float *array)=0;
  virtual void glUniformMatrix4fv(int loc, int count, int boolean, float *matrix)=0;
  virtual void glUniform3fv(int loc, int count, float *arr)=0;
  virtual void glBindAttribLocation(int p, int index, char *data)=0;
  


  // Old
  virtual void glEnableClientState(int a)=0;
  virtual void glLineWidth(float i)=0;
  virtual void glPushMatrix()=0;
  virtual void glPopMatrix()=0;
  virtual void glMultMatrixf(float *mat)=0;
  
};

class FontLowApi
{
};

class OpenVRLowApi
{
};

class EmscriptenLowApi
{
public:
  virtual void emscripten_set_main_loop_arg(void (*)(void*), void*, int, int)=0;
  virtual void emscripten_async_wget_data(void *buf2, void *data, void (*cb_sucess)(void*), void (*cb_fail)(void*))=0;
  virtual bool emscripten_has_threading_support()=0;
  virtual void emscripten_async_call(void (*)(void*), void *data, int val)=0;
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
  virtual void emscripten_set_click_callback(char *c, int a, bool true_, void (*)())=0;
  virtual void emscripten_set_touchend_callback(char *c, int a, bool true_, void (*)())=0;
  virtual void emscripten_vr_get_eye_parameters(int d, int left, void *ptr)=0;
};

struct LowApi
{
  OpenglLowApi *ogl;
  FontLowApi *fnt;
  OpenVRLowApi *ovr;
  EmscriptenLowApi *ems;
};

LowApi *gameapi_low;
