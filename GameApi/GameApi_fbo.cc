
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
  GLuint fbo_name;
  glGenFramebuffers(1, &fbo_name);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_name);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sx,sy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  GLuint depth_texture;
  glGenRenderbuffers(1, &depth_texture);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_texture);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, sx, sy);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  return add_fbo(e, fbo_name, texture, depth_texture, sx,sy);
}
EXPORT void GameApi::FrameBufferApi::config_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, priv->texture, 0);
  
  //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  //glDrawBuffers(1, DrawBuffers);
  //glReadBuffer( GL_COLOR_ATTACHMENT0 );
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
EXPORT GameApi::FrameBufferApi::vp GameApi::FrameBufferApi::bind_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glBindRenderbuffer(GL_RENDERBUFFER, priv->depthbuffer);
  FrameBufferApi::vp viewport;
  glGetIntegerv(GL_VIEWPORT, viewport.viewport);
  glViewport(0,0,priv->sx,priv->sy);
  return viewport;
}
EXPORT void GameApi::FrameBufferApi::bind_screen(vp viewport)
{

  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glViewport(viewport.viewport[0],viewport.viewport[1],viewport.viewport[2],viewport.viewport[3]);
}
EXPORT bool GameApi::FrameBufferApi::fbo_status(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  int val = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  std::cout << "CheckFrameBuffer: " << val << "== " << GL_FRAMEBUFFER_COMPLETE << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return val == GL_FRAMEBUFFER_COMPLETE;
}
EXPORT GameApi::BM GameApi::FrameBufferApi::fbo_to_bitmap(EveryApi &ev, FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  
  int width = priv->sx;
  int height = priv->sy;

  glBindTexture( GL_TEXTURE_2D, priv->texture);

  BufferRef ref = BufferRef::NewBuffer(width, height);
  glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);


  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  return add_color_bitmap2(e, bm);
}
class FBOTextureID : public TextureID
{
public:
  FBOTextureID(GameApi::EveryApi &ev, MainLoopItem *item, int sx, int sy, bool translate) : ev(ev), item(item), sx(sx), sy(sy),translate(translate)
  {
    if (sx == -1) { sx=ev.mainloop_api.get_screen_width(); }
    if (sy == -1) { sy=ev.mainloop_api.get_screen_height(); }
    fbo = ev.fbo_api.create_fbo(sx,sy);
    ev.fbo_api.config_fbo(fbo);
    id = ev.fbo_api.tex_id(fbo);

    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  void render(MainLoopEnv &e) {
    if (firsttime) {
      firsttime = false;
    }
    int id=0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
    glBindTexture(GL_TEXTURE_2D,0);
    GameApi::FrameBufferApi::vp viewport = ev.fbo_api.bind_fbo(fbo);
    glClearColor(0.0,0.0,0.0,0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    MainLoopEnv ee = e;
    // seems to break suomi100 flag.
    //ee.v_shader_functions = "";
    //ee.f_shader_functions = "";
    //ee.v_shader_funcnames.clear();
    //ee.f_shader_funcnames.clear();
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
      item->execute(ee);
    ev.fbo_api.bind_screen(viewport);
    glBindTexture(GL_TEXTURE_2D, id);
		  
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
};
GameApi::TXID GameApi::FrameBufferApi::fbo_ml(EveryApi &ev, GameApi::ML mainloop, int sx, int sy, bool translate)
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_txid(e, new FBOTextureID(ev,ml,sx,sy, translate));
}
class FBOML : public MainLoopItem
{
public:
  FBOML(GameApi::Env &env,GameApi::EveryApi &ev, TextureID *id, float start_x, float end_x, float start_y, float end_y, float z) : env(env), ev(ev), id(id), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { firsttime = true; }
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
  virtual int shader_id() { return -1; }  
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
