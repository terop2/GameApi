
#include "GameApi_h.hh"


SpritePriv::~SpritePriv()
{
  int s = arrays.size();
  for(int i=0;i<s;i++)
    {
      Sprite** ptr = arrays[i];
      delete [] ptr;
    }
  std::map<int, Sprite*>::iterator it2 = sprites.begin();
  for(;it2!=sprites.end();it2++)
    {
      Sprite *sp = (*it2).second;
      delete sp;
    }
  {
  std::map<int, Sprite*>::iterator it2 = sprites2.begin();
  for(;it2!=sprites2.end();it2++)
    {
      Sprite *sp = (*it2).second;
      delete sp;
    }
  }
}

PolyPriv::~PolyPriv()
{
  std::map<int,ArrayRender*>::iterator i = rend.begin();
  for(;i!=rend.end();i++)
    {
      ArrayRender *rend = (*i).second;
      delete rend;
    }
  std::map<int,StateBitmaps*>::iterator i2 = states.begin();
  for(;i2!=states.end();i2++)
    {
      StateBitmaps *st = (*i2).second;
      delete st;
    }
}

Sprite *sprite_from_handle2(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose)
{
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      int s = ahandle->vec.size();
      Sprite **arr = new Sprite*[s];
      env.arrays.push_back(arr);
      for(int i=0;i<s;i++)
	{
	  BitmapHandle *h = ahandle->vec[i];
	  Sprite *sk = sprite_from_handle(e, env, h);
	  arr[i] = sk;
	}
      ArraySprite *ss = new ArraySprite(arr, s);
      ss->update_cache();
      env.sprites2[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      ArrayRender *rend = env2->renders2[handle->id];
      if (rend)
	{
	  delete rend;
	}
      env2->renders2[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (chandle)
    {
      GameApi::BM bm = { handle->id };
      SpritePosImpl *pos = find_sprite_pos(e, bm);
      Point2d p;
      if (!pos)
	{
	  p.x = 0.0;
	  p.y = 0.0;
	}
      else
	{
	  p.x = pos->x;
	  p.y = pos->y;
	}
      Sprite *ss = new BitmapSprite(*chandle->bm, p);
      env.sprites2[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      ArrayRender *rend = env2->renders2[handle->id];
      if (rend)
	{
	  delete rend;
	}
      env2->renders2[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  std::cout << "Unknown bitmap type in sprite_from_handle" << std::endl;
  BitmapIntHandle *ihandle = dynamic_cast<BitmapIntHandle*>(handle);
  if (ihandle)
    {
      return 0;
    }
  return 0;
}

Sprite *sprite_from_handle(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose)
{
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      int s = ahandle->vec.size();
      Sprite **arr = new Sprite*[s];
      env.arrays.push_back(arr);
      for(int i=0;i<s;i++)
	{
	  BitmapHandle *h = ahandle->vec[i];
	  Sprite *sk = sprite_from_handle(e, env, h);
	  arr[i] = sk;
	}
      ArraySprite *ss = new ArraySprite(arr, s);
      ss->update_cache();
      env.sprites[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
  if (chandle)
    {
      GameApi::BM bm = { handle->id };
      SpritePosImpl *pos = find_sprite_pos(e, bm);
      Point2d p;
      if (!pos)
	{
	  p.x = 0.0;
	  p.y = 0.0;
	}
      else
	{
	  p.x = pos->x;
	  p.y = pos->y;
	}
      Sprite *ss = new BitmapSprite(*chandle->bm, p);
      env.sprites[handle->id] = ss;
      EnvImpl *env2 = ::EnvImpl::Environment(&e);
      env2->renders[handle->id] = new ArrayRender(g_low);
      //env2->renders2[handle->id] = new ArrayRender;
      return ss;
    }
  std::cout << "Unknown bitmap type in sprite_from_handle" << std::endl;
  BitmapIntHandle *ihandle = dynamic_cast<BitmapIntHandle*>(handle);
  if (ihandle)
    {
      return 0;
    }
  return 0;
}

EXPORT GameApi::SpriteApi::SpriteApi(GameApi::Env &e) : e(e)
{
  priv = (void*) new SpritePriv;
}

extern int g_event_screen_x;
extern int g_event_screen_y;
class TurnTo2d : public MainLoopItem
{
public:
  TurnTo2d(GameApi::EveryApi &ev, MainLoopItem *next, Point2d tl, Point2d br) : ev(ev), next(next), tl(tl), br(br) {
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    OpenglLowApi *ogl = g_low->ogl;
    static int inside_it = false;
    if (inside_it) { 
    ogl->glDisable(Low_GL_DEPTH_TEST);
      next->execute(e); 
    ogl->glEnable(Low_GL_DEPTH_TEST);
      return; 
    }

    inside_it = true;
    tl.x = e.screen_x;
    tl.y = e.screen_y;
    br.x = e.screen_width;
    br.y = e.screen_height;
    screen_x = ev.mainloop_api.get_screen_sx();
    screen_y = ev.mainloop_api.get_screen_sy();
    corner_x = ev.mainloop_api.get_corner_x();
    corner_y = ev.mainloop_api.get_corner_y();
    rect_sx = ev.mainloop_api.get_screen_rect_sx();
    rect_sy = ev.mainloop_api.get_screen_rect_sy();


  	float scale_x = 1.0;
	float scale_y = 1.0;
	if (g_event_screen_y!=-1) {
	  scale_x = float(g_event_screen_x)/float(screen_x);
	  scale_y = float(g_event_screen_y)/float(screen_y);
	}


    GameApi::SH sh = { e.sh_texture_2d };
    ev.shader_api.use(sh);
    ev.mainloop_api.switch_to_3d(false, sh, screen_x, screen_y);
    ogl->glViewport((corner_x+tl.x)*scale_x,(screen_y-corner_y-(br.y-tl.y))*scale_y, (br.x-tl.x)*scale_x, (br.y-tl.y)*scale_y);

    ogl->glDisable(Low_GL_DEPTH_TEST);
    //int old_sh = e.sh_texture;
    MainLoopEnv ee = e;
    ee.sh_texture = e.sh_texture_2d;
    ee.in_MV = Matrix::Identity();
    //ee.env = e.env * Matrix::Translate(0.0,0.0,400.0);
    ee.is_2d = true;
    next->execute(ee);
    //e.sh_texture = old_sh;
    ogl->glEnable(Low_GL_DEPTH_TEST);

    ev.mainloop_api.switch_to_3d(true, sh, screen_x, screen_y);
    ogl->glViewport(corner_x*scale_x,(screen_y-corner_y-rect_sy)*scale_y,rect_sx*scale_x, rect_sy*scale_y);
    inside_it = false;
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Point2d tl,br;
  int screen_x, screen_y;
  int corner_x, corner_y;
  int rect_sx, rect_sy;
};

class AltMLArray : public MainLoopItem
{
public:
  AltMLArray(GameApi::EveryApi &ev, std::vector<MainLoopItem*> vec, float s_time, float time_delta, bool repeat) : ev(ev), vec(vec),s_time(s_time), time_delta(time_delta), repeat(repeat) { 
    start_time = 0.0; //ev.mainloop_api.get_time();

  }
  void reset_time() {
    start_time = ev.mainloop_api.get_time();
  }

  void handle_event(MainLoopEvent &e)
  {
  }
  void Prepare() { int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    if (vec.size()==0) { return; }
    MainLoopItem *item = vec[0];
    float time = (e.time*1000.0-start_time)/100.0;
    if (time >= s_time+ time_delta*vec.size())
      {
	// repeat
	if (repeat) {
	  time = fmod(time-s_time, time_delta*vec.size())+s_time;
	}
	else
	{
	  item = vec[vec.size()-1];
	}
      }

    if (time<s_time) {
      item = vec[0];
    }
    if (time>=s_time && time < s_time + time_delta*vec.size())
      {
	int c = (time - s_time)/time_delta;
	item = vec[c];
      }
    item->execute(e);
  }  
private:
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> vec;
  float start_time;
  float s_time;
  float time_delta;
  bool repeat;
};

EXPORT GameApi::ML GameApi::SpriteApi::alt_ml_array(EveryApi &ev, std::vector<ML> vec, float start_time, float time_delta, bool repeat)
{
  std::vector<MainLoopItem*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = find_main_loop(e, vec[i]);
      vec2.push_back(item);
    }
  return add_main_loop(e, new AltMLArray(ev, vec2, start_time, time_delta, repeat));
}


EXPORT GameApi::ML GameApi::SpriteApi::turn_to_2d(EveryApi &ev, ML ml, float tl_x, float tl_y, float br_x, float br_y)
{
  MainLoopItem *next = find_main_loop(e, ml);
  Point2d tl = { tl_x, tl_y };
  Point2d br = { br_x, br_y };
  return add_main_loop(e, new TurnTo2d(ev,next,tl,br));
}

EXPORT GameApi::SpriteApi::~SpriteApi()
{
  delete (SpritePriv*)priv;
}
EXPORT void GameApi::SpriteApi::spritepos(BM bm, float x, float y)
{
  SpritePosImpl *i = find_sprite_pos(e,bm);
  i->x = x;
  i->y = y;
}

void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max=15);


class RenderVertexArray4 : public MainLoopItem
{
public:
  RenderVertexArray4(GameApi::Env &ee, GameApi::EveryApi &ev, GameApi::SpriteApi &sp, GameApi::BM bm) : ee(ee), ev(ev), sp(sp), bm(bm) 
  {
    firsttime = true;
    va.id = -1;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Prepare() { }
  void execute(MainLoopEnv &e)
  {
    if (firsttime)
      {
	// this is inside frame loop because url loading
	// requires some time before rendering can happen
	// this can be done only after url loading finished
	// url loading happens while frames are being updated.
	//InstallProgress(222,"bitmap",1);
	va = ev.sprite_api.create_vertex_array(bm,true);
	//ProgressBar(222,1,1,"bitmap");
	firsttime = false;
      }

    GameApi::SH sh;
    sh.id = e.sh_texture;
    ev.shader_api.use(sh);
    //std::cout << e.in_MV << std::endl;
    ev.shader_api.set_var(sh, "color_mix", 1.0f);
    ev.shader_api.set_var(sh, "in_MV", add_matrix2(ee,e.in_MV));
    sp.render_sprite_vertex_array(va);
  }
  std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &ee;
  GameApi::EveryApi &ev;
  GameApi::SpriteApi &sp;
  GameApi::VA va;
  GameApi::BM bm;
  bool firsttime;
};


EXPORT GameApi::ML GameApi::SpriteApi::render_sprite_vertex_array_ml(EveryApi &ev, BM bm)
{
  return add_main_loop(e, new RenderVertexArray4(e, ev, *this, bm));
}

EXPORT void GameApi::SpriteApi::render_sprite_vertex_array(VA va)
{
    OpenglLowApi *ogl = g_low->ogl;

  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  //if (!s || ((int)s)<0x100) { std::cout << "render_sprite_vertex_array ignored!" << std::endl; return; }
  //if (!rend || ((int)rend)<0x100){ std::cout << "render_sprite_vertex_array ignored!" << std::endl;  return; }
  //SpritePriv &spriv = *(SpritePriv*)priv;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders2[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders2[s->texture_id], 0, false);
    }
  else if(s->texture_id!=-1)
    {
      ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      rend->render(0);
      //      RenderVertexArray arr(*s);
      //arr.render(0);

      ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else
    {
      std::cout << "SpriteApi::render_sprite_vertex_array, texture not found!" << std::endl;
    }
}
EXPORT void GameApi::SpriteApi::clipping_sprite(VA va, int sx, int sy, float tex_l, float tex_t, float tex_r, float tex_b)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  Point pa1 = { 0.0, 0.0, 0.0 };
  Point pa2 = { float(sx), 0.0, 0.0 };
  Point pa3 = { float(sx), float(sy), 0.0 };
  Point pa4 = { 0.0, float(sy), 0.0 };
  Point p1 = { tex_l, tex_t,0.0 };
  Point p2 = { tex_r, tex_t,0.0 };
  Point p3 = { tex_r, tex_b,0.0 };
  Point p4 = { tex_l, tex_b,0.0 };
  Point *ptr = s->tri_texcoord_polys(0);
  Point *ptr2 = s->tri_polys(0);
  ptr2[0] = pa1;
  ptr2[1] = pa3;
  ptr2[2] = pa4;
  ptr2[3] = pa1;
  ptr2[4] = pa2;
  ptr2[5] = pa3;
  ptr[0] = p1;
  ptr[1] = p3; 
  ptr[2] = p4;
  ptr[3] = p1;
  ptr[4] = p2;
  ptr[5] = p3;
  rend->update(0);
}
class UpdateVertexArray4 : public MainLoopItem
{
public:
  UpdateVertexArray4(GameApi::SpriteApi &api, GameApi::VA va, GameApi::BM bm) : api(api), va(va), bm(bm) { }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Prepare() { }
  void execute(MainLoopEnv &e)
  {
    api.update_vertex_array(va,bm);
  }
  std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::SpriteApi &api;
  GameApi::VA va;
  GameApi::BM bm;
};
EXPORT GameApi::ML GameApi::SpriteApi::update_vertex_array_ml(VA va, BM bm)
{
  return add_main_loop(e, new UpdateVertexArray4(*this, va,bm));
}
EXPORT void GameApi::SpriteApi::update_vertex_array(VA va, BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  VertexArraySet *ss = find_vertex_array(e,va);

  delete env->renders2[ss->get_bm_id()];
  env->renders2[ss->get_bm_id()] = 0;

  ::Sprite *sprite = sprite_from_handle2(e,spriv, handle, -1);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; return; }
  VertexArraySet *s = new VertexArraySet;
  s->set_bm_id(bm.id);
  PrepareSpriteToVA(*sprite, *s);
  TexturePrepare(*sprite, *env->renders2[bm.id], true);
  s->texture_id = bm.id;
  RenderVertexArray *arr = new RenderVertexArray(g_low, *s); 
  arr->prepare(0);
  //s->free_memory();
  add_update_vertex_array(e, va, s, arr);
}
EXPORT GameApi::ML GameApi::SpriteApi::vertex_array_render(EveryApi &ev, BM bm)
{
  //GameApi::VA va = create_vertex_array(bm);
  GameApi::ML ml = render_sprite_vertex_array_ml(ev, bm);
  return ml;
}
EXPORT GameApi::VA GameApi::SpriteApi::create_vertex_array(BM bm, bool progress)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);

  ::Sprite *sprite = sprite_from_handle2(e,spriv, handle, -1);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; GameApi::VA va; va.id = 0; return va; }
  VertexArraySet *s = new VertexArraySet;
  s->set_bm_id(bm.id);
  PrepareSpriteToVA(*sprite, *s);
  TexturePrepare(*sprite, *env->renders2[bm.id],progress);
  s->texture_id = bm.id;
  RenderVertexArray *arr = new RenderVertexArray(g_low, *s); 
  arr->prepare(0);
  //s->free_memory();
  //std::cout << "add vertex array: " << (int)s << " " << (int)arr << std::endl;
  return add_vertex_array(e, s, arr); 
}
EXPORT void GameApi::SpriteApi::clear_arrays(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  //RenderVertexArray *rend = find_vertex_array_render(e, va);
  s->clear_arrays();
}

EXPORT void GameApi::SpriteApi::preparesprite(BM bm, int bbm_choose)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  //Bitmap<Color> *cbm = find_color_bitmap(handle, bbm_choose);
  //if (!cbm) return;
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *sprite = sprite_from_handle(e,spriv, handle, bbm_choose);
  if (!sprite) { std::cout << "preparesprite's sprite==NULL?" << std::endl; return; }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  PrepareSprite(*sprite, *env->renders[bm.id]);
}
EXPORT void GameApi::SpriteApi::rendersprite(BM bm, SH sh, float x, float y, float mult_x, float mult_y)
{
  rendersprite3_1(bm, 0, sh, x, y, mult_x, mult_y);
}
EXPORT void GameApi::SpriteApi::rendersprite2(BM bm, SH sh, PT pos)
{
  rendersprite4(bm, 0, sh, pos);
}
#if 0
void GameApi::SpriteApi::rendersprite(BM bm, SH sh, float x, float y, float x1, float y1, float inside_x, float inside_y, float inside_x1, float inside_y1)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x, y };
  Point2d pos3 = { x1, y1 };
  Point2d inside_2 = { inside_x, inside_y };
  Point2d inside_3 ={ inside_x1, inside_y1 }; 
  float z = 0.0;
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, pos3, inside_2, inside_3, z, *env->renders[bm.id], prog);
}
#endif
EXPORT void GameApi::SpriteApi::rendersprite3(GameApi::BM bm, int bm_choose, GameApi::SH sh, float x, float y, float mult_x, float mult_y)
{
  rendersprite3_1(bm,bm_choose,sh,x,y,mult_x, mult_y);
}
void GameApi::SpriteApi::rendersprite3_1(GameApi::BM bm, int bm_choose, GameApi::SH sh, float x, float y, float mult_x, float mult_y)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call preparesprite() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point2d pos2 = { x+0.5f, y+0.5f };
  float z = 0.0;
  //glPushMatrix();
  //glScalef(mult_x, mult_y, 1.0);
  //std::cout << "rendersprite: " << bm_choose << std::endl;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id], mult_x, mult_y, prog);

  //glPopMatrix();
}
void GameApi::SpriteApi::rendersprite4(BM bm, int bm_choose, SH sh, PT pos)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }

  Point *p = find_point(e, pos);
  Point2d pos2 = { p->x, p->y };
  float z = 0.0; //p->z;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos2, z, *env->renders[bm.id], prog);
}

EXPORT void GameApi::SpriteApi::rendersprite5(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, float x, float y)
{
  SpritePriv &spriv = *(SpritePriv*)priv;
  ::Sprite *s = spriv.sprites[bm.id];
  if (!s) { std::cout << "rendersprite sprite==NULL (maybe you need to call prepare() for every object before rendering happens. (do not put it to frame loop or you lose frame rates))" << std::endl; return; }
  SpaceImpl *move = find_space(e, move_space);
  SpaceImpl *spr = find_space(e, sprite_space);
  
  Point2d p1 = spr->conv(spr->tl);
  Point2d o = spr->conv(spr->origo);
  Vector2d v1 = p1 - o;

  Vector2d u_x = move->conv(move->tr) - move->conv(move->tl);
  Vector2d u_y = move->conv(move->bl) - move->conv(move->tl);
  Point2d u = move->conv(move->tl);
  Point2d pp = u + Point2d::FromVector(u_x * x) + Point2d::FromVector(u_y * y);
  pp += v1;

  Point2d pos = { pp.x, pp.y };
  float z = 0.0;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Program *prog = find_shader_program(e, sh);
  RenderSprite(*s, bm_choose, pos, z, *env->renders[bm.id], prog);
}
EXPORT void GameApi::SpriteApi::rendersprite6(BM bm, int bm_choose, SH sh, SP move_space, SP sprite_space, PT pos)
{
}
EXPORT GameApi::SP GameApi::SpriteApi::spritespace(BM bm)
{
  SP s;
  s.id = 0;
  return s;
}
EXPORT GameApi::PT GameApi::SpriteApi::pixelpos(BM bm, int x, int y)
{
  PT pt;
  pt.id = 0;
  return pt;
}

EXPORT GameApi::ARR GameApi::SpriteApi::sprite_atlas_x(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_x, int count)
{
  //ev.bitmap_api.prepare(orig);
  int s=count;
  ArrayType *arr = new ArrayType;
  arr->type=1;
  for(int i=0;i<s;i++)
    {
      BM sub = ev.bitmap_api.subbitmap(orig, start_x+delta_x*i, start_y, end_x-start_x, end_y-start_y);
      arr->vec.push_back(sub.id);
    }
  return add_array(e, arr);
}
EXPORT GameApi::ARR GameApi::SpriteApi::sprite_atlas_y(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_y, int count)
{
  //ev.bitmap_api.prepare(orig);
  int s=count;
  ArrayType *arr = new ArrayType;
  arr->type=1;
  for(int i=0;i<s;i++)
    {
      BM sub = ev.bitmap_api.subbitmap(orig, start_x, start_y+delta_y*i, end_x-start_x, end_y-start_y);
      arr->vec.push_back(sub.id);
    }
  return add_array(e, arr);
}

EXPORT GameApi::ARR GameApi::SpriteApi::sprite_atlas_xy(EveryApi &ev, BM orig, int start_x, int end_x, int start_y, int end_y, int delta_x, int delta_y, int count_x, int count_y)
{
  //ev.bitmap_api.prepare(orig);
  ArrayType *arr = new ArrayType;
  arr->type=1;
  for(int i=0;i<count_y;i++)
    for(int j=0;j<count_x;j++)
    {
      BM sub = ev.bitmap_api.subbitmap(orig, start_x+delta_x*j, start_y+delta_y*i, end_x-start_x, end_y-start_y);
      arr->vec.push_back(sub.id);
    }
  return add_array(e, arr);
}
