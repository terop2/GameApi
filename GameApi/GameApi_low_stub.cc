
#include "GameApi_low.hh"

class OpenglStub : public OpenglLowApi
{
public:
  virtual void init() { }
  virtual void cleanup() { }

  virtual int glGetError() { return 0; }
  
  virtual void glColor4ub(int r, int g, int b, int a) {}
  virtual void glColor4f(float r, float g, float b, float a) { }
  virtual void glClearColor(float r, float g, float b, float a) { }
  virtual void glClear(int bits) { }
  virtual void glDepthMask(int a) { }
  virtual void glDepthFunc(int i) { }
  virtual void glStencilMask(int a) { }
  virtual void glStencilOp(int keep, int keep2, int repl) { }
  virtual void glStencilFunc(int notequal, int, int) { }
  virtual void glPolygonMode(int front_and_back, int gl_line) { }
  virtual void glClearStencil(int val) { }
  virtual void glTexEnvi(int a, int mode, int rep) { }

  // attribs
  virtual void glEnable(int val) { }
  virtual void glDisable(int val) { }
  virtual void glGetFloatv(int val, float *params) { }

  // viewports
  virtual void glViewport(int x, int y, unsigned int w, unsigned int h) { }

  // textures
  virtual void glGenTextures(int val, unsigned int *tex) { }
  virtual void glDeleteTextures(int val, const unsigned int *tex) { }
  virtual void glBindTexture(int ID, int tex) { }
  virtual void glTexImage2D(int ID, int, int,unsigned int,unsigned int, int, int, int, void *ptr) { }
  virtual void glTexSubImage2D(int ID, int, int,int, unsigned int,unsigned int, int, int, void *ptr) { }
  virtual void glCopyTexImage2D(int ID, int,int,int,int, int,int, int ptr) { }
  virtual void glTexParameteri(int ID, int,int) { }
  virtual void glHint(int hint, int value) { }
  virtual void glActiveTexture(int a) { }
  virtual void glClientActiveTexture(int a) { }
  virtual void glTexStorage3D(int arr, int, int flag, int w, int h, int layer_count) { }
  virtual void glTexSubImage3D(int arr, int,int,int,int,int,int, int, int rgba, int unsig_byte, void *buffer) { }
  virtual void glGetTexLevelParameteriv(int a, int, int w, int *ptr) { }
  virtual void glGetTexImage(int a, int, int rgba, int unsign_byte, void *ptr) { }
  virtual void glReadBuffer(int a) { }
  

  // blend
  virtual void glBlendFunc(int val, int val2) { }
  
  // scissor
  virtual void glScissor(float x, float y, float w, float h) { }

  // fbo
  //virtual void glGenRenderbuffers(int i, unsigned int *ptr)=0;
  virtual void glFramebufferTexture2D(int i, int atth, int tex2d, int tex, int ptr) { }

  // vertex arrays
  virtual void glGenBuffers(int i, unsigned int *buffers) { }
  virtual void glDeleteBuffers(int count, const unsigned int *buffers) { }
  virtual void glBindBuffer(int a, unsigned int data) { }
  virtual void glBufferData(int a, int size, const void *ptr, int static_draw) { }
  virtual void glBufferSubData(int a, int, int, const void*) { }
  virtual void glVertexAttribPointer(int, int, int gl_float, int boolean, unsigned int, const void *ptr) { }
  virtual void glVertexAttribIPointer(int, int, int gl_float, int boolean, const void *ptr) { }
  virtual void glVertexAttribDivisor(int, int) { }
  virtual void glGenVertexArrays(int i, unsigned int *arr) { }
  virtual void glDeleteVertexArrays(int count, unsigned int *vao) { }
  virtual void glBindVertexArray(int vao) { }
  virtual void glEnableVertexAttribArray(int a) { }
  virtual void glDisableVertexAttribArray(int a) { }
  virtual void glDrawArraysInstanced(int tri, int, unsigned int, unsigned int) { }
  virtual void glDrawArrays(int tri, int, unsigned int) { }
  
  // bitmaps
  virtual void glReadPixels(int x, int y, int w, int h, int rgba, int mode, void *ptr) { }
  
  // fbo
  virtual void glGenFramebuffers(int i, unsigned int *fbo_id) { }
  virtual void glGenRenderbuffers(int i, unsigned int *rbo_id) { }
  virtual void glDeleteRenderBuffers(int i, unsigned int *rbo) { }
  virtual void glDeleteFrameBuffers(int i, unsigned int *fbo) { }
  virtual void glBindFramebuffer(int a, int fbo_id) { }
  virtual void glBindRenderbuffer(int a, int fbo_id) { }
  virtual void glRenderbufferStorage(int a, int d, int sx, int sy) { }
  virtual void glFramebufferRenderbuffer(int a, int d, int r, unsigned int tex) { }
  
  // shaders
  virtual unsigned int glCreateShader(int shader) { return 0; }
  virtual void glShaderSource(int h, int c, const char **strings, int *lengths) { }
  virtual void glCompileShader(int h) { }
  virtual void glGetShaderInfoLog(unsigned int h, int val, int *length, char *buf) { }
  virtual void glGetShaderiv(int handle, int gl_compile_status, int *ptr) { }
  virtual void glDeleteShader(int h) { }
  
  // programs
  virtual int glCreateProgram() { return 0; }
  virtual void glDeleteProgram(int p) { }
 virtual  void glAttachShader(int p, int h) { } 
  virtual void glDetachShader(int p, int h) { }
  virtual void glLinkProgram(int p) { }
  virtual void glUseProgram(int p) { }
  virtual void glGetProgramInfoLog(unsigned int p, int num, int *len, char *buf) { }
  virtual void glBindFragDataLocation(int p, int num, const char *data) { }
  virtual void glBindAttribLocation(int p, int num, const char *data) { }
  virtual void glProgramParameteriEXT(int p, int geom, int inputtype) { }
  
  // uniforms
  virtual int glGetUniformLocation(int p, const char *data) { return 0; }
  virtual int glGetAttribLocation(int p, const char *data) { return 0; }
  virtual void glUniform1f(int loc, float val) { }
  virtual void glUniform1i(int loc, int val) { }
  virtual void glUniform2f(int loc, float val, float val2) { }
  virtual void glUniform3f(int loc, float val, float val2, float val3) { }
  virtual void glUniform4f(int loc, float val, float val2, float val3, float val4) { }
  virtual void glUniform1iv(int loc, int count, int *array) { }
  virtual void glUniform1fv(int loc, int count, float *array) { }
  virtual void glUniformMatrix4fv(int loc, unsigned int count, int boolean, const float *matrix) { }
  virtual void glUniform3fv(int loc, int count, float *arr) { }
  virtual void glGetUniformfv(int p, int loc, float *arr) { }
  virtual void glGetIntegerv(int p, int *ptr) { }
  virtual int glCheckFramebufferStatus(int id) { return 0; }

  virtual void glFinish() { }

  // Old
  virtual void glEnableClientState(int a) { }
  virtual void glLineWidth(float i) { }
  virtual void glPushMatrix() { }
  virtual void glPopMatrix() { }
  virtual void glMultMatrixf(float *mat) { }
  virtual void glMatrixLoadIdentityEXT(int i) { }
  virtual void glMatrixMode(int i) { }
  virtual void glLoadIdentity() { }
  virtual void glTranslatef(float, float, float) { }
  virtual void glDisableClientState(int) { }
  virtual void glTexCoordPointer(int,int,int, void*) { }
  virtual void glColorPointer(int,int,int,void*) { }
  virtual void glVertexPointer(int,int,int,void*) { }
  virtual void glNormalPointer(int,int,void*) { }
  virtual void glRotatef(float,float,float,float) {}
  virtual const unsigned char *glGetString( int name ) { return 0; }

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
class FontLowApiStub : public FontLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
};
 

class OpenVRLowApiStub : public OpenVRLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
};

class EmscriptenLowApiStub : public EmscriptenLowApi
{
public:
  virtual void init() { }
  virtual void cleanup() { }
  virtual void emscripten_set_main_loop_arg(void (*)(void*), void*, int, int) { }
  virtual void emscripten_async_wget_data(void *buf2, void *data, void (*cb_sucess)(void*), void (*cb_fail)(void*)) {}
  virtual bool emscripten_has_threading_support() { return false; }
  virtual void emscripten_async_call(void (*)(void*), void *data, int val) { }
  virtual void emscripten_set_click_callback(char *c, int a, bool true_, void (*)()) { }
  virtual void emscripten_set_touchend_callback(char *c, int a, bool true_, void (*)()) { }

};
class EmscriptenVRLowApiStub : public EmscriptenVRLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
  virtual void emscripten_vr_init(void (*)(void*), void*) { }
  virtual void emscripten_vr_submit_frame(int disp) { }
  virtual void emscripten_vr_get_frame_data(int disp, void *data) { }
  virtual bool emscripten_vr_display_presenting(int disp) { return false; }
  virtual void emscripten_vr_set_display_render_loop_arg(int disp, void (*)(void*), void* data) { }
  virtual void emscripten_vr_exit_present(int disp) { }
  virtual void emscripten_vr_cancel_display_render_loop(int disp) { }
  virtual void emscripten_vr_request_present(int disp, void *init, int, void (*cb)(void*), void *data) { }
  virtual bool emscripten_vr_ready() { return false; }
  virtual int emscripten_vr_count_displays() { return 0; }
  virtual int emscripten_vr_get_display_handle(int i) { return 0; }
  virtual char *emscripten_vr_get_display_name( int d ) { return const_cast<char*>("stub display");}
  virtual int emscripten_vr_get_display_capabilities( int d, void *caps ) { return 0; }
  virtual int emscripten_vr_display_connected( int d) {return 0; }
  virtual void emscripten_vr_get_eye_parameters(int d, int left, void *ptr) { }
};

class FileSystemLowApiStub : public FileSystemLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
  virtual void popen(char *name, char *flags) { }
};

class ImageLoadLowApiStub : public ImageLoadLowApi
{
  virtual void init() { }
  virtual void cleanup() { }

};

class SDLLowApiStub : public SDLLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
  virtual const char* SDL_GetError() {}
  virtual char *SDL_GetClipboardText() { return ""; }
  virtual int SDL_SetClipboardText(const char *) { return -1; }

  virtual void SDL_Init(int flags) { }
  virtual void SDL_GL_SetAttribute(int flag, int val) { }
  virtual void* SDL_GL_GetProcAddress(char *name) { return 0; }
  virtual void SDL_Quit() { }
  virtual void SDL_ConvertSurface(Low_SDL_Surface *surf, void *format, int val) { }
  virtual void SDL_FreeSurface(Low_SDL_Surface *surf) { }
  virtual void SDL_LockSurface(Low_SDL_Surface *surf) { }
  virtual void SDL_UnlockSurface(Low_SDL_Surface *surf) { }
  virtual void SDL_ShowCursor(bool b) { }
  virtual int SDL_PollEvent(Low_SDL_Event *event) { return 0; }
  virtual unsigned int SDL_GetTicks() { return 0; }
  virtual void SDL_Delay(int ms) { }
  virtual Low_SDL_Surface* SDL_GetWindowSurface(Low_SDL_Window *win) { return 0; }
  virtual Low_SDL_Window* SDL_CreateWindow(const char *title, int x, int y, int width, int height, unsigned int flags2) { return 0; }
  virtual Low_SDL_GLContext SDL_GL_CreateContext(Low_SDL_Window *window) { Low_SDL_GLContext ctx; return ctx; }
  virtual int SDL_GL_MakeCurrent(Low_SDL_Window *window, Low_SDL_GLContext context) { return 0; }

  virtual void SDL_GL_SwapBuffers() { }
  virtual void SDL_GL_SwapWindow(Low_SDL_Window *window) { }
  virtual void SDL_UpdateWindowSurface(Low_SDL_Window *window) { }
  virtual void SDL_DestroyWindow(Low_SDL_Window *window) { }
  virtual void SDL_SetWindowTitle(Low_SDL_Window *window, const char *title) { }
  virtual unsigned int SDL_GetMouseState(int *x, int *y) { return 0; }
  virtual unsigned int SDL_GetModState() { return 0; }
  virtual Low_SDL_Joystick* SDL_JoystickOpen(int i) { return 0; }
  virtual void SDL_JoystickEventState(int i) { }
  virtual unsigned int SDL_JoystickGetButton(Low_SDL_Joystick *joy, int i) { return 0; }
  virtual Low_SDL_RWops* SDL_RWFromMem(void *buffer, int size) { return 0; }
  virtual void SDL_GL_DeleteContext(Low_SDL_GLContext ctx) { }
  virtual void SDL_SetWindowSize(Low_SDL_Window *window, int w, int h) { }
};

class SDLMixerLowApiStub : public SDLMixerLowApi
{
public:
  virtual void init() { }
  virtual void cleanup() { }
  virtual Low_Mix_Chunk* Mix_LoadWAV_RW(Low_SDL_RWops *buf, int s) { return 0; }
  virtual int Mix_OpenAudio(int rate, int flags, int val, int hup) { return 0; }
  virtual int Mix_PlayChannel(int channel, Low_Mix_Chunk *mix_chunk, int val) { return 0; }
  virtual Low_Mix_Chunk *Mix_QuickLoad_RAW(unsigned char *mem, int len) { return 0; }

  virtual void Mix_Init(int flags) {}
  virtual Low_Mix_Music *Mix_LoadMUS(const char *filename) { return 0; }
  virtual void Mix_PlayMusic(Low_Mix_Music *mus, int val) { }
  virtual int Mix_GetNumMusicDecoders() { return 0; }
  virtual void Mix_GetMusicDecoder(int i) { }
  virtual void Mix_AllocateChannels(int i) { }
};

class SDLImageLowApiStub : public SDLImageLowApi
{
  virtual void init() { }
  virtual void cleanup() { }
};
extern LowApi *g_low;

void initialize_stub(int flags)
{
  LowApi *low = new LowApi;
  low->ogl = new OpenglStub;
  low->sdl = new SDLLowApiStub;
  low->sdl_mixer = new SDLMixerLowApiStub;
  g_low = low;
}
