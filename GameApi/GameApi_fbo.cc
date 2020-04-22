
#include "GameApi_h.hh"

EXPORT GameApi::TXID GameApi::FrameBufferApi::tex_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->texture;
  return id;
}
EXPORT GameApi::TXID GameApi::FrameBufferApi::depth_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->depthbuffer;
  return id;
}
EXPORT GameApi::FBO GameApi::FrameBufferApi::create_fbo(int sx, int sy)
{
  OpenglLowApi *ogl = g_low->ogl;
  
  Low_GLuint fbo_name;
  ogl->glGenFramebuffers(1, &fbo_name);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, fbo_name);

  Low_GLuint texture;
  ogl->glGenTextures(1, &texture);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, texture);
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, sx,sy, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, 0);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MIN_FILTER, Low_GL_LINEAR);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MAG_FILTER, Low_GL_LINEAR);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);

  Low_GLuint depth_texture2;
  Low_GLuint depth_texture;
  ogl->glGenTextures(1, &depth_texture2);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, depth_texture2);
  //ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MIN_FILTER, Low_GL_NEAREST);
  //ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MAG_FILTER, Low_GL_NEAREST);
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_DEPTH_COMPONENT, sx,sy, 0, Low_GL_DEPTH_COMPONENT, Low_GL_UNSIGNED_INT,0);
  //ogl->glGenRenderbuffers(1, &depth_texture);
  //ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, depth_texture);
  //ogl->glRenderbufferStorage(Low_GL_RENDERBUFFER, Low_GL_DEPTH_COMPONENT16, sx, sy);
  ogl->glFramebufferRenderbuffer(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_RENDERBUFFER, depth_texture2);

  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_TEXTURE_2D, depth_texture2, 0);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, 0);

  return add_fbo(e, fbo_name, texture, depth_texture2, sx,sy);
}
EXPORT void GameApi::FrameBufferApi::config_fbo(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;

  FBOPriv *priv = find_fbo(e, buffer);

  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0, Low_GL_TEXTURE_2D, priv->texture, 0);
  //ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_TEXTURE_2D, priv->depthbuffer,0);
  
  //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  //glDrawBuffers(1, DrawBuffers);
  //glReadBuffer( GL_COLOR_ATTACHMENT0 );
  
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
}
EXPORT GameApi::FrameBufferApi::vp GameApi::FrameBufferApi::bind_fbo(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;
  FBOPriv *priv = find_fbo(e, buffer);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, priv->depthbuffer);
  FrameBufferApi::vp viewport;
  ogl->glGetIntegerv(Low_GL_VIEWPORT, viewport.viewport);
  ogl->glViewport(0,0,priv->sx,priv->sy);
  return viewport;
}
EXPORT void GameApi::FrameBufferApi::bind_screen(vp viewport)
{
  OpenglLowApi *ogl = g_low->ogl;

  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER,0);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, 0);
  
  ogl->glViewport(viewport.viewport[0],viewport.viewport[1],viewport.viewport[2],viewport.viewport[3]);
}
EXPORT bool GameApi::FrameBufferApi::fbo_status(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;
  FBOPriv *priv = find_fbo(e, buffer);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  int val = ogl->glCheckFramebufferStatus(Low_GL_FRAMEBUFFER);
  //std::cout << "CheckFrameBuffer: " << val << "== " << Low_GL_FRAMEBUFFER_COMPLETE << std::endl;
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
  return val == Low_GL_FRAMEBUFFER_COMPLETE;
}
EXPORT GameApi::BM GameApi::FrameBufferApi::fbo_to_bitmap(EveryApi &ev, FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;
  FBOPriv *priv = find_fbo(e, buffer);
  
  int width = priv->sx;
  int height = priv->sy;

  ogl->glBindTexture( Low_GL_TEXTURE_2D, priv->texture);

  BufferRef ref = BufferRef::NewBuffer(width, height);
#ifndef EMSCRIPTEN
  ogl->glGetTexImage( Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
#endif


  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  return add_color_bitmap2(e, bm);
}
class FBOTextureID : public TextureID
{
public:
  FBOTextureID(GameApi::EveryApi &ev, MainLoopItem *item, int sx, int sy, bool translate, bool is_depth) : ev(ev), item(item), sx(sx), sy(sy),translate(translate), is_depth(is_depth)
  {
    if (sx == -1) { sx=ev.mainloop_api.get_screen_width(); }
    if (sy == -1) { sy=ev.mainloop_api.get_screen_height(); }
    fbo = ev.fbo_api.create_fbo(sx,sy);
    ev.fbo_api.config_fbo(fbo);
    if (is_depth) {
      id = ev.fbo_api.depth_id(fbo);
    } else {
      id = ev.fbo_api.tex_id(fbo);
    }
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  void render(MainLoopEnv &e) {
  OpenglLowApi *ogl = g_low->ogl;
    int id2=0;
    ogl->glGetIntegerv(Low_GL_TEXTURE_BINDING_2D, &id2);
    ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
    GameApi::FrameBufferApi::vp viewport = ev.fbo_api.bind_fbo(fbo);
    ogl->glClearColor(0.0,0.0,0.0,0.0);
    ogl->glClear( Low_GL_COLOR_BUFFER_BIT | Low_GL_DEPTH_BUFFER_BIT | Low_GL_STENCIL_BUFFER_BIT);
    ogl->glEnable(Low_GL_DEPTH_TEST);
    ogl->glDepthMask(Low_GL_TRUE);
    MainLoopEnv ee = e;
    ee.v_shader_functions = "";
    ee.f_shader_functions = "";
    ee.v_shader_funcnames.clear();
    ee.f_shader_funcnames.clear();
    ee.us_vertex_shader=-1;
    ee.us_fragment_shader=-1;
    ee.screen_x = 0;
    ee.screen_y = 0;
    //ee.screen_width = sx;
    //ee.screen_height = sy;
    
    int old_x = viewport.viewport[0];
    int old_y = viewport.viewport[1];
    //int oldwidth = viewport.viewport[2]-viewport.viewport[0];
    //int oldheight = viewport.viewport[3]-viewport.viewport[1];
    
    if (translate) {
      ee.in_MV = Matrix::Translate(old_x,old_y,0.0); //*Matrix::Scale(1.0/oldwidth, 1.0/oldheight,1.0)*Matrix::Scale(sx,sy,1.0);
      ee.env = Matrix::Translate(old_x,old_y,0.0); //*Matrix::Scale(1.0/oldwidth, 1.0/oldheight,1.0)*Matrix::Scale(sx,sy,1.0);
    } else {
      ee.in_MV = Matrix::Identity();
      ee.env = Matrix::Identity();
    }
    if (firsttime) {
      item->Prepare();
      firsttime = false;
    }

      item->execute(ee);
    ev.fbo_api.bind_screen(viewport);
    ogl->glBindTexture(Low_GL_TEXTURE_2D, id2);
    ogl->glDisable(Low_GL_DEPTH_TEST);
		  
  }
  int texture() const
  {
    if (id.id==-1) return 0;
    return id.id;
  }
private:
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  GameApi::FBO fbo;
  GameApi::TXID id;
  int sx,sy;
  bool firsttime;
  bool translate;
  bool is_depth;
};
GameApi::TXID GameApi::FrameBufferApi::fbo_ml(EveryApi &ev, GameApi::ML mainloop, int sx, int sy, bool translate)
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_txid(e, new FBOTextureID(ev,ml,sx,sy, translate, false));
}
GameApi::TXID GameApi::FrameBufferApi::depth_ml(EveryApi &ev, GameApi::ML mainloop, int sx, int sy, bool translate)
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_txid(e, new FBOTextureID(ev,ml,sx,sy, translate, true));
}
class FBOML : public MainLoopItem
{
public:
  FBOML(GameApi::Env &env,GameApi::EveryApi &ev, TextureID *id, float start_x, float end_x, float start_y, float end_y, float z) : env(env), ev(ev), id(id), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { firsttime = true; }
  void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    id->render(e);
    int ii = id->texture();
    GameApi::TXID iid;
    iid.id = ii;
    if (firsttime)
      {
	//GameApi::BM bm = ev.texture_api.to_bitmap(iid);
	GameApi::P p = ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
	va = ev.polygon_api.create_vertex_array(p, true);
	//va = ev.sprite_api.create_vertex_array(bm);
	va2 = ev.texture_api.bind(va, iid);
	firsttime = false;
      }
    GameApi::US us_v;
    us_v.id = -1;
    GameApi::US us_f;
    us_f.id = -1;
    if (e.us_vertex_shader!=-1)
      {
	us_v.id = e.us_vertex_shader;
      }
    if (e.us_fragment_shader!=-1)
      {
	us_f.id = e.us_fragment_shader;
      }
    if (sh.id==-1 && us_v.id!=-1 && us_f.id!=-1)
      {
	sh = ev.shader_api.get_normal_shader("comb", "comb", "", us_v, us_f,e.v_shader_functions, e.f_shader_functions);
	ev.mainloop_api.init_3d(sh);
	ev.mainloop_api.alpha(true);
      }
    if (sh.id==-1) {
      sh.id = e.sh_texture;
    }


    ev.shader_api.use(sh);

    
    GameApi::M m2 = add_matrix2(env, e.env);
    GameApi::M m2i = ev.matrix_api.transpose(ev.matrix_api.inverse(m2));
    ev.shader_api.set_var(sh, "in_MV", m2);
    ev.shader_api.set_var(sh, "in_iMV", m2i);
    //ev.texture_api.use(iid,0);
    //ev.sprite_api.render_sprite_vertex_array(va);
    ev.polygon_api.render_vertex_array(va2);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    id->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  TextureID *id;
  GameApi::VA va;
  GameApi::VA va2;
  bool firsttime;
  float start_x, end_x;
  float start_y, end_y;
  float z;
  GameApi::SH sh;
};
GameApi::ML GameApi::FrameBufferApi::fbo_ml_blit(EveryApi &ev, GameApi::TXID id, float start_x, float end_x, float start_y, float end_y, float z)
{
  TextureID *txid = find_txid(e, id);
  return add_main_loop(e, new FBOML(e,ev,txid, start_x, end_x, start_y, end_y, z));
}
