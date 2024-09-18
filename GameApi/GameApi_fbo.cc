
#include "GameApi_h.hh"

void confirm_texture_usage(VertexArraySet *set);

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
EXPORT GameApi::FBO GameApi::FrameBufferApi::create_fbo(EveryApi &ev, int sx, int sy)
{
  OpenglLowApi *ogl = g_low->ogl;
  
  Low_GLuint fbo_name;
    ev.mainloop_api.check_glerrors("create_fbo_before");
  ogl->glGenFramebuffers(1, &fbo_name);
    ev.mainloop_api.check_glerrors("create_fbo0");
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, fbo_name);
    ev.mainloop_api.check_glerrors("create_fbo1");

  Low_GLuint texture;
  ogl->glGenTextures(1, &texture);
    ev.mainloop_api.check_glerrors("create_fbo2");
  ogl->glBindTexture(Low_GL_TEXTURE_2D, texture);
    ev.mainloop_api.check_glerrors("create_fbo3");
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGB, sx,sy, 0, Low_GL_RGB, Low_GL_UNSIGNED_BYTE, 0);
    ev.mainloop_api.check_glerrors("create_fbo4");
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MIN_FILTER, Low_GL_NEAREST);
    ev.mainloop_api.check_glerrors("create_fbo5");
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MAG_FILTER, Low_GL_NEAREST);
    ev.mainloop_api.check_glerrors("create_fbo6");
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
    ev.mainloop_api.check_glerrors("create_fbo7");
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
    ev.mainloop_api.check_glerrors("create_fbo8");

  //Low_GLuint depth_texture2=0;
  
  Low_GLuint depth_texture;
  ogl->glGenTextures(1, &depth_texture);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, depth_texture);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MIN_FILTER, Low_GL_NEAREST);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D, Low_GL_TEXTURE_MAG_FILTER, Low_GL_NEAREST);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);

  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_DEPTH_COMPONENT32F, sx,sy, 0, Low_GL_DEPTH_COMPONENT, Low_GL_FLOAT,0);
  /*  
  ogl->glGenRenderbuffers(1, &depth_texture);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, texture);
  ogl->glRenderbufferStorage(Low_GL_RENDERBUFFER, Low_GL_RGBA, sx, sy);
  
  
  ogl->glFramebufferRenderbuffer(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_RENDERBUFFER, depth_texture);

  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_RGBA, Low_GL_TEXTURE_2D, texture, 0);
  */
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
  //ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, 0);

  return add_fbo(e, fbo_name, texture, depth_texture, sx,sy);
}
void map_enums(int &i);
EXPORT void GameApi::FrameBufferApi::config_fbo(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;

  FBOPriv *priv = find_fbo(e, buffer);

  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0, Low_GL_TEXTURE_2D, priv->texture, 0);
  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_TEXTURE_2D, priv->depthbuffer,0);
  
  /*
  int ii = Low_GL_COLOR_ATTACHMENT0;
  map_enums(ii);
  unsigned int DrawBuffers[1] = { (unsigned int)ii };
  ogl->glDrawBuffers(1, DrawBuffers);
  ogl->glReadBuffer( Low_GL_COLOR_ATTACHMENT0 );
  */
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
}
EXPORT void GameApi::FrameBufferApi::unconfig_fbo(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;

  FBOPriv *priv = find_fbo(e, buffer);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0, Low_GL_TEXTURE_2D, 0, 0);
  ogl->glFramebufferTexture2D(Low_GL_FRAMEBUFFER, Low_GL_DEPTH_ATTACHMENT, Low_GL_TEXTURE_2D, 0,0);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);

}

EXPORT GameApi::FrameBufferApi::vp GameApi::FrameBufferApi::bind_fbo(FBO buffer)
{
  OpenglLowApi *ogl = g_low->ogl;
  FBOPriv *priv = find_fbo(e, buffer);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, priv->fbo_name);
  //ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, priv->depthbuffer);
  FrameBufferApi::vp viewport;
  ogl->glGetIntegerv(Low_GL_VIEWPORT, viewport.viewport);
  ogl->glViewport(0,0,priv->sx,priv->sy);
  return viewport;
}
EXPORT void GameApi::FrameBufferApi::bind_screen(vp viewport)
{
  OpenglLowApi *ogl = g_low->ogl;

  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER,0);
  //ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, 0);
  
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


class CollectInterfaceImpl2 : public CollectVisitor
{
public:
  void register_obj(CollectInterface *i) {    
    vec.push_back(i);
  }
  void register_first_frame(CollectInterface *i) {
    vec2.push_back(i);
  }
  void execute(int i) {
    int sz = vec.size();
    if (i>=0 && i<sz && vec[i])
      vec[i]->HeavyPrepare();
  }
  void first_frame(int i) {
    int sz = vec2.size();
    if (i>=0 && i<sz && vec2[i])
      vec2[i]->FirstFrame();
  }
			   
  int count(int i) const
  {
    int sz = vec.size();
    if (i>=0 && i<sz && vec[i])
      return vec[i]->NumBlocks();
    return 1;
  }
public:
  std::vector<CollectInterface*> vec;
  std::vector<CollectInterface*> vec2;
  //int count=0;
};


class FBOTextureID : public TextureID
{
public:
  FBOTextureID(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *item, int sx, int sy, bool translate, bool is_depth) : env(env), ev(ev), item(item), sx(sx), sy(sy),translate(translate), is_depth(is_depth)
  {
    firsttime2=true;
    firsttime=true;
  }
  bool is_fbo() const { return true; }
  void Collect(CollectVisitor &vis)
  {
    item->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { item->Prepare(); }
  void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  std::vector<int> shader_id() { return item->shader_id(); }
  void render(MainLoopEnv &e) {
    if (firsttime) {
      /*
      CollectInterfaceImpl2 *vis = new CollectInterfaceImpl2;
      item->Collect(*vis);
	int counter = 0;
	int s = vis->vec.size();
	for(int i=0;i<s;i++) counter+=vis->count(i);
	int vis_counter=0;
      for(int i=0;i<counter;i++) {
	vis->execute(vis_counter);
	vis_counter++;
      }
      */
      //item->Prepare();
      firsttime = false;
    }
    if (firsttime2) {
      firsttime2=false;
    if (sx == -1) { sx=512; }
    if (sy == -1) { sy=512; }

    if (sx!=1 && sx!=2 && sx!=4 && sx!=8 && sx!=16 && sx!=32 && sx!=64 && sx!=128 && sx!=256 && sx!=512 && sx!=1024 && sx!=2048 && sx!=4096) {
      std::cout << "Warning: power of two texture sizes needed in fbo_ml/fbo_id (doesnt work in webgl)" << std::endl;
    }
    if (sy!=1 && sy!=2 && sy!=4 && sy!=8 && sy!=16 && sy!=32 && sy!=64 && sy!=128 && sy!=256 && sy!=512 && sy!=1024 && sy!=2048 && sy!=4096) {
      std::cout << "Warning: power of two texture sizes needed in fbo_ml/fbo_id (doesnt work in webgl)" << std::endl;
    }

    
    // std::cout << sx << " " << sy << std::endl;
    fbo = ev.fbo_api.create_fbo(ev,sx,sy);
    ev.mainloop_api.check_glerrors("create_fbo");
    ev.fbo_api.config_fbo(fbo);
    ev.mainloop_api.check_glerrors("config_fbo");
    if (is_depth) {
      id = ev.fbo_api.depth_id(fbo);
    ev.mainloop_api.check_glerrors("depth_id");
    } else {
      id = ev.fbo_api.tex_id(fbo);
    ev.mainloop_api.check_glerrors("tex_id");
    }
    int count = 30000;
    while(count>0&&!ev.fbo_api.fbo_status(fbo)) { count--; }
    ev.mainloop_api.check_glerrors("fbo_status");

    if (count==0) std::cout << "ERROR, framebuffer status incomplete after delay" << std::endl;
    //firsttime = true;
    }
    
    
  OpenglLowApi *ogl = g_low->ogl;
    int id2=0;
    // These two are causing black screens when drawing via p_render
    //ogl->glGetIntegerv(Low_GL_TEXTURE_BINDING_2D, &id2);
    //ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
    GameApi::FrameBufferApi::vp viewport = ev.fbo_api.bind_fbo(fbo);
    ogl->glClearColor(0.0,0.0,0.0,1.0);
    ogl->glClear( Low_GL_COLOR_BUFFER_BIT| Low_GL_DEPTH_BUFFER_BIT);
    ogl->glEnable(Low_GL_DEPTH_TEST);
    //ogl->glDepthFunc(Low_GL_LESS);
    ogl->glDisable(Low_GL_BLEND);
    //ogl->glDepthMask(Low_GL_TRUE);
    MainLoopEnv ee = e;

    /*  
    ee.v_shader_functions = "";
    ee.f_shader_functions = "";
    ee.v_shader_funcnames.clear();
    ee.f_shader_funcnames.clear();
    
    ee.us_vertex_shader=0;
    ee.us_fragment_shader=0;
    */
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
      ee.in_iMV = Matrix::Translate(-old_x,-old_y,0.0);
      ee.env = Matrix::Translate(old_x,old_y,0.0); //*Matrix::Scale(1.0/oldwidth, 1.0/oldheight,1.0)*Matrix::Scale(sx,sy,1.0);
    } else {
      ee.in_MV = Matrix::Identity();
      ee.in_iMV = Matrix::Identity();
      ee.env = Matrix::Identity();
      }


    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = e.us_vertex_shader;
	fragment.id = e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    
    if (shader.id!=-1)
      {
    GameApi::M m = add_matrix2( env, ee.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, ee.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, ee.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	GameApi::M m3 = add_matrix2(env, ee.in_P);
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	if (ee.has_inverse) {
	  GameApi::M m4 = add_matrix2(env, ee.in_iMV);	  
	  ev.shader_api.set_var(shader, "in_iMV", m4);
	} else
	  ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_P", m3);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);
      }
    
    //#ifndef EMSCRIPTEN
    //ogl->glDisable(Low_GL_MULTISAMPLE);
    //#endif
    item->execute(ee);
      //#ifndef EMSCRIPTEN
      //ogl->glEnable(Low_GL_MULTISAMPLE);
      //#endif
      ev.fbo_api.bind_screen(viewport);
      //ogl->glBindTexture(Low_GL_TEXTURE_2D, id2);
    ////ogl->glDisable(Low_GL_DEPTH_TEST);
    ogl->glEnable(Low_GL_DEPTH_TEST);
    ogl->glEnable(Low_GL_BLEND);

    ev.fbo_api.unconfig_fbo(fbo);
    
  }
  int texture() const
  {
  OpenglLowApi *ogl = g_low->ogl;

    FBOPriv *priv = find_fbo(ev.get_env(), fbo);
    if (priv==0) return 0;
    //std::cout << "ID=" << std::dec << priv->texture << std::endl;
  ogl->glBindTexture(Low_GL_TEXTURE_2D, priv->texture);
    return priv->texture;
  /*
  if (id.id==-1) return 0;
    return id.id;
  */
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  GameApi::FBO fbo;
  GameApi::TXID id;
  int sx,sy;
  bool firsttime;
  bool firsttime2;
  bool translate;
  bool is_depth;
  GameApi::SH shader = { -1 };
};
GameApi::TXID GameApi::FrameBufferApi::fbo_ml(EveryApi &ev, GameApi::ML mainloop, int sx, int sy, bool translate)
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_txid(e, new FBOTextureID(e,ev,ml,sx,sy, translate, false));
}
GameApi::TXID GameApi::FrameBufferApi::depth_ml(EveryApi &ev, GameApi::ML mainloop, int sx, int sy, bool translate)
{
  MainLoopItem *ml = find_main_loop(e, mainloop);
  return add_txid(e, new FBOTextureID(e,ev,ml,sx,sy, translate, true));
}
class FBOML : public MainLoopItem
{
public:
  FBOML(GameApi::Env &env,GameApi::EveryApi &ev, TextureID *id, float start_x, float end_x, float start_y, float end_y, float z) : env(env), ev(ev), id(id), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { firsttime = true; }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
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
	VertexArraySet *s = find_vertex_array(env, va2);
	confirm_texture_usage(s);
	//VertexArraySet *find_vertex_array(GameApi::Env &e, GameApi::VA p)

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
P I1=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
//P I1=ev.polygon_api.fullscreen_quad(ev);
  MT I6=ev.materials_api.textureid(ev,id,1.0);
  ML I7=ev.materials_api.bind(I1,I6);
  return I7;
  //TextureID *txid = find_txid(e, id);
  //return add_main_loop(e, new FBOML(e,ev,txid, start_x, end_x, start_y, end_y, z));
}
