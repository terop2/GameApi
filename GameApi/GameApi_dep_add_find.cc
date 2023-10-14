
#include "GameApi_h.hh"
#include <memory>


void InstallProgress(int num, std::string label, int max);
void ProgressBar(int num, int val, int max, std::string label);
void stackTrace();


struct Block
{
  std::vector<std::shared_ptr<void> > vec;
#if 0
  ~Block()
  {
    int s = vec.size();
    //if (s)
    //  InstallProgress(667, "Cleanup", 15);
    for(int i=0;i<s;i++)
      {
	//if (i%10==0) {
	//  ProgressBar(667,i*15/s,15,"Cleanup");
	//}
	//std::cout << typeid(decltype(*(vec[i]))).name() << std::endl;
      vec[i].reset();
      }
  //if (s)
  //    ProgressBar(667,15,15,"Cleanup");
    //std::cout << std::endl;
    vec.clear();
  }
#endif
};

struct G_BLOCK
{
std::vector<Block*> g_blocks;
  ~G_BLOCK() {
    int s = g_blocks.size();
    for(int i=0;i<s;i++) delete g_blocks[i];
  }
} g_blocks;

struct Rest {
  std::vector<std::shared_ptr<void> > g_rest;
  void delete_item_from_rest(void *ptr)
  {
    int s = g_rest.size();
    for(int i=0;i<s;i++) {
      void *ptr2 = g_rest[i].get();
      if (ptr==ptr2) {
	g_rest[i].reset();
      }
    }
    
  }
  ~Rest()
  {
    int s = g_rest.size();
    // doesnt work because global destructors have already been ran
    // and installprogress uses global variables.
    //InstallProgress(666, "cleanup", 15);
    for(int i=0;i<s;i++)
      {
	//if (i%10==0) {
	//  ProgressBar(666,i*15/s,15,"cleanup");
	//}
      g_rest[i].reset();
      }
    //ProgressBar(666,15,15,"cleanup");
    //std::cout << std::endl;
    g_rest.clear();
  }
};
Rest g_rest;
int g_current_block=-1;
int add_block()
{
  g_blocks.g_blocks.push_back(new Block);
  return g_blocks.g_blocks.size()-1;
}
void recreate_block(int id)
{
  g_blocks.g_blocks[id]=new Block;
}
void set_current_block(int id)
{
  g_current_block = id;
}
void delete_item_from_block(void *ptr, Block *blk)
{
  int s = blk->vec.size();
  for(int i=0;i<s;i++) {
    void *ptr2 = blk->vec[i].get();
    if (ptr==ptr2) {
      blk->vec[i].reset();
    }
  }
}
void delete_item(void *ptr)
{
  int ks = g_blocks.g_blocks.size();
  for(int i=0;i<ks;i++) {
    Block *blk = g_blocks.g_blocks[i];
    if (blk)
    delete_item_from_block(ptr, blk);
  }
  g_rest.delete_item_from_rest(ptr);
}
int get_current_block()
{
  return g_current_block;
}
void clear_block(int id)
{
  delete g_blocks.g_blocks[id];
  g_blocks.g_blocks[id]=0;
}
void add_b(std::shared_ptr<void> ptr)
{
  if (g_current_block!=-1)
    g_blocks.g_blocks[g_current_block]->vec.push_back(ptr);
  else
    g_rest.g_rest.push_back(ptr); // these will never be released
}

GameApi::TT add_timing(GameApi::Env &e, Timing *t)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->timings.push_back(t);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(t));
  GameApi::TT im;
  im.id = env->timings.size()-1;
  return im;
}

GameApi::PL add_platform(GameApi::Env &e, Platform *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->platforms.push_back(pl);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(pl));
  GameApi::PL im;
  im.id = env->platforms.size()-1;
  return im;
}

GameApi::PBO add_pbo(GameApi::Env &e, PixelBufferObject *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pbo.push_back(i);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(i));
  GameApi::PBO im;
  im.id = env->pbo.size()-1;
  return im;
}

GameApi::SHP add_shp(GameApi::Env &e, ShaderParameterI* i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shp.push_back(i);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(i));
  GameApi::SHP im;
  im.id = env->shp.size()-1;
  return im;
}
GameApi::SHI add_shaderI(GameApi::Env &e, ShaderI2* i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shi.push_back(i);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(i));
  GameApi::SHI im;
  im.id = env->shi.size()-1;
  return im;
}
GameApi::TF add_gltf(GameApi::Env &e, GLTFModelInterface *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->tf.push_back(i);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(i));
  GameApi::TF im;
  im.id = env->tf.size()-1;
  return im;
}
GameApi::SHC add_shader_code(GameApi::Env &e, ShaderCode *code)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shc.push_back(code);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(code));
  GameApi::SHC im;
  im.id = env->shc.size()-1;
  return im;
}
GameApi::GC add_gc(GameApi::Env &e, GraphicsContext *gc)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->gc.push_back(gc);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(gc));
  GameApi::GC im;
  im.id = env->gc.size()-1;
  return im;
}

GameApi::ATT add_attach(GameApi::Env &e, Attach *att)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->attach.push_back(att);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(att));
  GameApi::ATT im;
  im.id = env->attach.size()-1;
  return im;
}

GameApi::MB add_memblock(GameApi::Env &e, MemoryBlock *b)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->memblock.push_back(b);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(b));
  GameApi::MB im;
  im.id = env->memblock.size()-1;
  return im;
}

GameApi::UV add_uv(GameApi::Env &e, Fetcher<FaceID> *f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->uv.push_back(f);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(f));
  GameApi::UV im;
  im.id = env->uv.size()-1;
  return im;

}
GameApi::AV add_voxel_array(GameApi::Env &e, VoxelArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->voxel_array.push_back(arr);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(arr));
  GameApi::AV im;
  im.id = env->voxel_array.size()-1;
  return im;

}

GameApi::HML add_html(GameApi::Env &e, Html *file)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->html.push_back(file);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(file));
  GameApi::HML im;
  im.id = env->html.size()-1;
  return im;
}

GameApi::W add_frm_widget(GameApi::Env &e, FrmWidget *w)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->frm_widgets.push_back(w);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(w));
  GameApi::W im;
  im.id = env->frm_widgets.size()-1;
  return im;

}

GameApi::PN add_polynomial(GameApi::Env &e, std::vector<float> *pn)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->polynomials.push_back(pn);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(pn));
  GameApi::PN im;
  im.id = env->polynomials.size()-1;
  return im;

}

GameApi::FBU add_framebuffer(GameApi::Env &e, FrameBuffer *buf)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->frame_buffer.push_back(buf);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(buf));
  GameApi::FBU im;
  im.id = env->frame_buffer.size()-1;
  return im;

}
GameApi::FML add_framemainloop(GameApi::Env &e, FrameBufferLoop *loop)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->frame_loop.push_back(loop);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(loop));
  GameApi::FML im;
  im.id = env->frame_loop.size()-1;
  return im;
}


GameApi::DS add_disk_store(GameApi::Env &e, DiskStore *ds)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->disk_store.push_back(ds);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(ds));
  GameApi::DS im;
  im.id = env->disk_store.size()-1;
  return im;
}
GameApi::TXID add_txid(GameApi::Env &e, TextureID *txid)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  int id = txid->texture();
  env->txids[id] = txid;
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(txid));
  GameApi::TXID tx;
  tx.id = id;
  return tx;
}

GameApi::PA add_patch(GameApi::Env &e, CurvePatch *patch)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_patches.push_back(patch);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(patch));
  GameApi::PA im;
  im.id = env->curve_patches.size()-1;
  return im;

}
GameApi::SD add_string_display(GameApi::Env &e, StringDisplay *sd)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->string_displays.push_back(sd);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(sd));
  GameApi::SD im;
  im.id = env->string_displays.size()-1;
  return im;
}
GameApi::GI add_glyph_interface(GameApi::Env &e, GlyphInterface *gi)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->glyph_interfaces.push_back(gi);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(gi));
  GameApi::GI im;
  im.id = env->glyph_interfaces.size()-1;
  return im;
}
GameApi::FI add_font_interface(GameApi::Env &e, FontInterface *fi)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->font_interfaces.push_back(fi);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(fi));
  GameApi::FI im;
  im.id = env->font_interfaces.size()-1;
  return im;
}
GameApi::FF add_float_fetcher(GameApi::Env &e, Fetcher<float> *f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->float_fetchers.push_back(f);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(f));
  GameApi::FF im;
  im.id = env->float_fetchers.size()-1;
  return im;
}
GameApi::PF add_point_fetcher(GameApi::Env &e, Fetcher<Point> *f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->point_fetchers.push_back(f);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(f));
  GameApi::PF im;
  im.id = env->point_fetchers.size()-1;
  return im;
}
GameApi::IF add_int_fetcher(GameApi::Env &e, Fetcher<int> *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->int_fetchers.push_back(i);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(i));
  GameApi::IF im;
  im.id = env->int_fetchers.size()-1;
  return im;
}
GameApi::SF add_string_fetcher(GameApi::Env &e, Fetcher<std::string> *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->string_fetchers.push_back(i);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(i));
  GameApi::SF im;
  im.id = env->string_fetchers.size()-1;
  return im;
}
GameApi::ARR add_array(GameApi::Env &e, ArrayType *type)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->arrays2.push_back(type);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(type));
  GameApi::ARR im;
  im.id = env->arrays2.size()-1;
  return im;
}
GameApi::PTT add_point_transform(GameApi::Env &e, PointTransform *trans)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->point_transforms.push_back(trans);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(trans));
  GameApi::PTT im;
  im.id = env->point_transforms.size()-1;
  return im;
}
GameApi::KF add_vertex_anim(GameApi::Env &e, VertexAnimNode *node)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vertex_anims.push_back(node);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(node));
  GameApi::KF im;
  im.id = env->vertex_anims.size()-1;
  return im;
}
GameApi::CPP add_curve_pos(GameApi::Env &e, CurvePos *pos)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_pos.push_back(pos);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(pos));
  GameApi::CPP im;
  im.id = env->curve_pos.size()-1;
  return im;
}
GameApi::BLK add_blocker(GameApi::Env &e, Blocker *blk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->blockers.push_back(blk);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(blk));
  GameApi::BLK im;
  im.id = env->blockers.size()-1;
  return im;

}
GameApi::RUN add_splitter(GameApi::Env &e, Splitter *blk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->splitters.push_back(blk);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(blk));
  GameApi::RUN im;
  im.id = env->splitters.size()-1;
  return im;

}
GameApi::CC add_color(GameApi::Env &e, ColorChange *cc)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->color_change.push_back(cc);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(cc));
  GameApi::CC im;
  im.id = env->color_change.size()-1;
  return im;

}
GameApi::PP add_plane_shape(GameApi::Env &e, PlaneShape *sh)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_shapes.push_back(sh);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(sh));
  GameApi::PP im;
  im.id = env->plane_shapes.size()-1;
  return im;

}
GameApi::MX add_mixed(GameApi::Env &e, MixedI *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->mixed.push_back(m);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(m));
  GameApi::MX im;
  im.id = env->mixed.size()-1;
  return im;
}
GameApi::SA add_skeletal(GameApi::Env &e, SkeletalNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->skeletals.push_back(n);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(n));
  GameApi::SA im;
  im.id = env->skeletals.size()-1;
  return im;

}
GameApi::MC add_matrix_curve(GameApi::Env &e, Curve<Matrix> *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix_curves.push_back(m);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(m));
  GameApi::MC im;
  im.id = env->matrix_curves.size()-1;
  return im;

}
GameApi::MS add_matrix_array(GameApi::Env &e, MatrixArray *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix_arrays.push_back(m);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(m));
  GameApi::MS im;
  im.id = env->matrix_arrays.size()-1;
  return im;

}
GameApi::C add_curve(GameApi::Env &e, Curve<Point> *curve)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curves.push_back(curve);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(curve));
  GameApi::C im;
  im.id = env->curves.size()-1;
  return im;
}
GameApi::US add_uber(GameApi::Env &e, ShaderCall *call)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shadercalls.push_back(call);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(call));
  GameApi::US im;
  im.id = env->shadercalls.size()-1;
  return im;
}
GameApi::MT add_material(GameApi::Env &e, Material *mat)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->materials.push_back(mat);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(mat));
  GameApi::MT im;
  im.id = env->materials.size()-1;
  return im;

}
GameApi::TL add_tree_level(GameApi::Env &e, TreeLevel *lvl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->tree_levels.push_back(lvl);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(lvl));
  GameApi::TL im;
  im.id = env->tree_levels.size()-1;
  return im;
}
GameApi::T add_tree(GameApi::Env &e, TreeStack *tre)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->trees.push_back(tre);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(tre));
  GameApi::T im;
  im.id = env->trees.size()-1;
  return im;

}
GameApi::IM add_implicit(GameApi::Env &e, ImplicitFunction3d *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->implicit.push_back(m);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(m));
  GameApi::IM im;
  im.id = env->implicit.size()-1;
  return im;
}
GameApi::MN add_move(GameApi::Env &e, Movement *m)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->movement_array.push_back(m);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(m));
  GameApi::MN a;
  a.id = env->movement_array.size()-1;
  return a;

}
GameApi::CP add_collision(GameApi::Env &e, Collision *c)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->collision_array.push_back(c);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(c));
  GameApi::CP a;
  a.id = env->collision_array.size()-1;
  return a;
}
GameApi::TS add_tri_strip(GameApi::Env &e, TriStrip *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->tri_strip.push_back(n);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(n));
  GameApi::TS a;
  a.id = env->tri_strip.size()-1;
  return a;

}
GameApi::PH add_physics(GameApi::Env &e, PhysicsNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->phys.push_back(n);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(n));
  GameApi::PH a;
  a.id = env->phys.size()-1;
  return a;

}
GameApi::EX add_expr(GameApi::Env &e, ExprNode *n)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->exprs.push_back(n);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(n));
  GameApi::EX a;
  a.id = env->exprs.size()-1;
  return a;
}
template<class T>
GameApi::A<T> add_array(GameApi::Env &e, std::vector<T> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  std::vector<int> vec2;
  int s = arr->size();
  for(int i=0;i<s;i++)
    {
      T t = arr->operator[](i);
      int val = t.id;
      vec2.push_back(val);
    }

  env->arrays.push_back(new std::vector<int>(vec2));
  GameApi::A<T> a;
  a.id = env->arrays.size()-1;
  return a;
}
GameApi::ML add_main_loop_wgpu(GameApi::Env &e, MainLoopItemWGPU *item)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->wgpu_main_loop.push_back(item);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(item));
  GameApi::ML ml;
  ml.id = env->wgpu_main_loop.size()-1;
  return ml;
}
GameApi::ML add_main_loop(GameApi::Env &e, MainLoopItem *item)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->main_loop.push_back(item);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(item));
  GameApi::ML ml;
  ml.id = env->main_loop.size()-1;
  return ml;
}

GameApi::FtA add_font_atlas(GameApi::Env &e, FontAtlasInfo *info)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->font_atlas.push_back(info);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(info));
  GameApi::FtA ft;
  ft.id = env->font_atlas.size()-1;
  return ft;
}
GameApi::W add_widget(GameApi::Env &e, GuiWidget *w)
{
  // stackTrace();
  if (!w)
    {
      std::cout << "add_widget failed" << std::endl;
      assert(0);
    }

  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->widgets.push_back(w);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(w));
  GameApi::W wid;
  wid.id = env->widgets.size()-1;
  return wid;
}
GameApi::SFO add_shader_module(GameApi::Env &e, ShaderModule *vol)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_module.push_back(vol);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(vol));
  GameApi::SFO sfo;
  sfo.id = env->shader_module.size()-1;
  return sfo;
}
GameApi::SM add_sample(GameApi::Env &e, Samples *s)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->samples.push_back(s);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(s));
  GameApi::SM sm;
  sm.id = env->samples.size()-1;
  return sm;
}
GameApi::TRK add_tracker(GameApi::Env &e, Tracker *trk)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->trackers.push_back(trk);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(trk));

  GameApi::TRK sm;
  sm.id = env->trackers.size()-1;
  return sm;
}
GameApi::FBO add_fbo(GameApi::Env &e, Low_GLuint fbo_name, Low_GLuint texture, Low_GLuint depthbuffer, int sx, int sy)
{
  FBOPriv p;
  p.fbo_name = fbo_name;
  p.texture = texture;
  p.depthbuffer = depthbuffer;
  p.sx = sx;
  p.sy = sy;
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->fbos.push_back(p);
  GameApi::FBO f;
  f.id = env->fbos.size()-1;
  return f;
}

GameApi::F add_float(GameApi::Env &e, LazyValue<float> *val)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->floats.push_back(val);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(val));
  GameApi::F f;
  f.id = env->floats.size()-1;
  return f;
}
GameApi::FA add_float_array(GameApi::Env &e, Array<int,float> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->float_array.push_back(arr);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(arr));
  GameApi::FA f;
  f.id = env->float_array.size()-1;
  return f;
}
GameApi::FD add_distance(GameApi::Env &e, DistanceRenderable *dist)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->distvolume.push_back(dist);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(dist));
  GameApi::FD fd;
  fd.id = env->distvolume.size()-1;
  return fd;
}
GameApi::SP add_space(GameApi::Env &e, SpaceImpl i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->sp.push_back(i);
  //add_b(std::shared_ptr<void>(i));
  GameApi::SP sp;
  sp.id = env->sp.size()-1;
  //sp.type = 0;
  return sp;
}
GameApi::S add_surface2(GameApi::Env &e, SurfaceIn3d *s)
{
  SurfaceImpl i;
  i.surf = s;
  return add_surface(e,i);
}
GameApi::S add_surface(GameApi::Env &e, SurfaceImpl i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->surfaces.push_back(i);
  GameApi::S s;
  s.id = env->surfaces.size()-1;
  //sp.type = 0;
  return s;
}

GameApi::BM add_bitmap3(GameApi::Env &e, BitmapHandle *handle)
{
  //stackTrace();
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->bm.push_back(handle);
  GameApi::BM bm;
  bm.id = env->bm.size()-1;

  Bitmap<Color> *bm2 = ((BitmapColorHandle*)handle)->bm;
  
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bm2));

  
  //std::cout << "add_bitmap: " << bm.id << std::endl;
  //bm.type = 0;
  handle->id = bm.id;
  return bm;

}

GameApi::BM add_bitmap(GameApi::Env &e, BitmapHandle *handle)
{
  //stackTrace();
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->bm.push_back(handle);
  GameApi::BM bm;
  bm.id = env->bm.size()-1;

  //std::cout << "add_bitmap: " << bm.id << std::endl;
  //bm.type = 0;
  handle->id = bm.id;
  return bm;
}

GameApi::BM add_color_bitmap(GameApi::Env &e, Bitmap<Color> *bm)
{
#if 1
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bm));
  return add_bitmap(e,handle);
#endif
  
#if 0
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  GameApi::BitmapApi api(e);
  return add_bitmap(e,handle);
#endif
}
GameApi::BM add_color_bitmap2(GameApi::Env &e, Bitmap<Color> *bm)
{
  //stackTrace();
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bm;
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bm));
  return add_bitmap(e,handle);
}
GameApi::BMA add_bitmap_array(GameApi::Env &e, BitmapArray2<Color> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->bm_array.push_back(arr);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(arr));
  GameApi::BMA bma;
  bma.id = env->bm_array.size()-1;
  return bma;
}
GameApi::BB add_bool_bitmap(GameApi::Env &e, Bitmap<bool> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  BoolBitmap handle;
  handle.bitmap = bitmap;
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bitmap));
  env->bool_bm.push_back(handle);
  GameApi::BB bm;
  bm.id = env->bool_bm.size()-1;
  //bm.type = 0;
  return bm;
}

GameApi::LAY add_layout(GameApi::Env &e, Layout *l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->layouts.push_back(l);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(l));
  GameApi::LAY ll;
  ll.id = env->layouts.size()-1;
  return ll;
}

GameApi::WV add_waveform(GameApi::Env &e, Waveform *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->waveforms.push_back(bitmap);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bitmap));
  GameApi::WV bm;
  bm.id = env->waveforms.size()-1;
  //bm.type = 0;
  return bm;
}


GameApi::TX add_texture(GameApi::Env &e, TextureI *i)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->textures.push_back(i);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(i));
  GameApi::TX tx;
  tx.id = env->textures.size()-1;
  return tx;
}
GameApi::Q add_tex_quad(GameApi::Env &e, Point2d p1, Point2d p2)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  TexCoordQuad q;
  q.p1 = p1;
  q.p2 = p2;
  env->tex_quads.push_back(q);
  GameApi::Q qq;
  qq.id = env->tex_quads.size()-1;
  return qq;
}

GameApi::FB add_float_bitmap(GameApi::Env &e, Bitmap<float> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  FloatBitmap handle;
  handle.bitmap = bitmap;
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bitmap));
  env->float_bm.push_back(handle);
  GameApi::FB bm;
  bm.id = env->float_bm.size()-1;
  //bm.type = 0;
  return bm;
}

GameApi::VA add_vertex_array(GameApi::Env &e, VertexArraySet *va, RenderVertexArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vertex_array.push_back(va);
  env->vertex_array_render.push_back(arr);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(va));
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(arr));

  GameApi::VA bm;
  bm.id = env->vertex_array.size()-1;
  return bm;
}
void add_update_widget(GameApi::Env &e, GameApi::W widget, GuiWidget *w)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  //int s = g_rest.g_rest.size();
  //for(int i=0;i<s;i++) {
  //  if (g_rest.g_rest[i].get()==env->widgets[widget.id]) {
  //    g_rest.g_rest[i].reset();
  //  }
  //}
  //delete env->widgets[widget.id];
  delete_item(env->widgets[widget.id]);
  env->widgets[widget.id] = w;
}
void add_update_vertex_array(GameApi::Env &e, GameApi::VA va_h, VertexArraySet *va, RenderVertexArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  delete_item(env->vertex_array[va_h.id]);
  delete_item(env->vertex_array_render[va_h.id]);
  //delete env->vertex_array[va_h.id];
  //delete env->vertex_array_render[va_h.id];
  env->vertex_array[va_h.id] = va;
  env->vertex_array_render[va_h.id] = arr;
}

GameApi::CBM add_continuous_bitmap(GameApi::Env &e, ContinuousBitmap<Color> *bitmap)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->continuous_bitmaps.push_back(bitmap);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(bitmap));
  GameApi::CBM bm;
  bm.id = env->continuous_bitmaps.size()-1;
  return bm;
}

GameApi::F add_function(GameApi::Env &e, FunctionImpl &f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->func.push_back(f);
  GameApi::F func;
  func.id = env->func.size()-1;
  return func;
  
}
GameApi::F add_function(GameApi::Env &e, Function<float,float> *f)
{
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(f));

  FunctionImpl ff;
  ff.func = f;
  return add_function(e, ff);
}
GameApi::SA add_separate(GameApi::Env &e, Separate *sep)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->separates.push_back(sep);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(sep));
  GameApi::SA p;
  p.id = env->separates.size()-1;
  return p;
}

GameApi::PL add_plane(GameApi::Env &e, PlanePoints2d *sep)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_points.push_back(sep);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(sep));
  GameApi::PL p;
  p.id = env->plane_points.size()-1;
  return p;
}

// takes ownership of PolyHandle*
GameApi::P add_polygon(GameApi::Env &e, FaceCollPolyHandle *handle)
{
  //stackTrace();
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (g_current_block != -2)
    add_b(std::shared_ptr<void>(handle));
  env->poly.push_back(handle);
  GameApi::P p;
  p.id = env->poly.size()-1;

  //std::cout << "add_polygon returning" << p.id << std::endl;
  //bm.type = 0;
  handle->id = p.id;
  return p;
}
GameApi::OM add_move(GameApi::Env &e, Array<int, ObjectWithPos> *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->object_move.push_back(arr);
  GameApi::OM om;
  om.id = env->object_move.size()-1;
  return om;
}

GameApi::VAA add_move_array(GameApi::Env &e, std::vector<VertexArrayWithPos> *vec)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->object_move_vertex_array.push_back(vec);
  GameApi::VAA om;
  om.id = env->object_move_vertex_array.size()-1;
  return om;
}
// takes ownership of FaceCollection*
GameApi::P add_polygon(GameApi::Env &e, FaceCollection *coll, int size)
{
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(coll));

  FaceCollPolyHandle *h = new FaceCollPolyHandle;
  h->coll = coll;
  h->collowned = true;
  h->collarray=0;
  h->collarrayowned = false;
  //h->size = size;
  //GameApi::PolygonApi api(e);
  return /*api.memoize(*/add_polygon(e,h)/*)*/;
}
GameApi::P add_polygon2(GameApi::Env &e, FaceCollection *coll, int size=1)
{
  //stackTrace();
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(coll));
  FaceCollPolyHandle *h = new FaceCollPolyHandle;
  h->coll = coll;
  h->collowned = true;
  h->collarray=0;
  h->collarrayowned = false;
  //h->size = size;
  return add_polygon(e,h);
}
GameApi::M add_matrix2(GameApi::Env &e, Matrix mm)
{
  GameApi::M m;
  std::copy(&mm.matrix[0], &mm.matrix[0]+16, &m.mat[0]);
  return m;
}
GameApi::M add_matrix(GameApi::Env &e, MatrixInterface *i)
{
  GameApi::M m;
  Matrix mm = i->get_matrix();
  std::copy(&mm.matrix[0], &mm.matrix[0]+16, &m.mat[0]);
  delete i;
  return m;
#if 0
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrix.push_back(i);
  GameApi::M m;
  m.id = env->matrix.size()-1;
  return m;
#endif
}

GameApi::LI add_line_array(GameApi::Env &e, LineCollection *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->linearray.push_back(array);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(array));

  GameApi::LI pt;
  pt.id = env->linearray.size()-1;
  return pt;
}
GameApi::PTS add_points_api_points(GameApi::Env &e, PointsApiPoints *pts)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointsapi_points.push_back(pts);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(pts));

  GameApi::PTS ptsa;
  ptsa.id = env->pointsapi_points.size()-1;
  return ptsa;
}
GameApi::PTA add_point_array3(GameApi::Env &e, PointArray3 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray3.push_back(array);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(array));
  GameApi::PTA pt;
  pt.id = env->pointarray3.size()-1;
  return pt;
}
GameApi::MSA add_matrix_array3(GameApi::Env &e, MatrixArray3 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->matrixarray3.push_back(array);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(array));
  GameApi::MSA pt;
  pt.id = env->matrixarray3.size()-1;
  return pt;
}
GameApi::FOA add_point_array(GameApi::Env &e, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray.push_back(array);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(array));
  GameApi::FOA pt;
  pt.id = env->pointarray.size()-1;
  return pt;
}
void add_update_lines_array(GameApi::Env &e, GameApi::LLA la, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  PointArray2 *arr = env->pointarray[la.id];
  g_low->ogl->glDeleteBuffers(1, &arr->buffer);
  g_low->ogl->glDeleteBuffers(1, &arr->buffer2);
#ifdef VAO
  g_low->ogl->glDeleteVertexArrays( 1, arr->vao );
#endif
  delete [] env->pointarray[la.id]->array;
  delete [] env->pointarray[la.id]->color_array;
  delete_item( env->pointarray[la.id] );
  env->pointarray[la.id] = array;
}
GameApi::LLA add_lines_array(GameApi::Env &e, PointArray2 *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointarray.push_back(array);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(array));
  GameApi::LLA pt;
  pt.id = env->pointarray.size()-1;
  return pt;
}
GameApi::PC add_pointcoll_array(GameApi::Env &e, PointCollection *array)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->pointcollarray.push_back(array);
   if (g_current_block != -2)
 add_b(std::shared_ptr<void>(array));
  GameApi::PC pt;
  pt.id = env->pointcollarray.size()-1;
  return pt;
}

GameApi::PT add_point(GameApi::Env &e, float x, float y)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Point p = Point(x,y,0.0);
#ifndef EMSCRIPTEN
  env->lock();
#endif
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
#ifndef EMSCRIPTEN
  env->unlock();
#endif
  //pt.type = 0;
  return pt;
}



GameApi::TR add_timerange(GameApi::Env &e, int paths)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->timeranges.push_back(new TROArray(paths));
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}
GameApi::TR add_timerange(GameApi::Env &e, TROArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->timeranges.push_back(arr);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(arr));
  GameApi::TR tr;
  tr.id = env->timeranges.size()-1;
  return tr;
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr);

GameApi::VV add_timerange_vertexarray(GameApi::Env &e, GameApi::TR tr)
{
#if 0
  TROArray *arr = find_timerange(e, tr);
  
  EnvImpl *env = ::EnvImpl::Environment(&e);
  VArray *varr = new VArray(*arr);
  varr->copy2();
  env->timerange_vertexarrays.push_back(varr);
  GameApi::VV vv;
  vv.id = env->timerange_vertexarrays.size()-1;
  return vv;
#endif
}

GameApi::O add_volume(GameApi::Env &e, VolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->volumes.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::O pt;
  pt.id = env->volumes.size()-1;
  return pt;
}


GameApi::FO add_float_volume(GameApi::Env &e, FloatVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->floatvolumes.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::FO pt;
  pt.id = env->floatvolumes.size()-1;
  return pt;
}

GameApi::COV add_color_volume(GameApi::Env &e, ColorVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->colorvolume.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::COV pt;
  pt.id = env->colorvolume.size()-1;
  return pt;
}


GameApi::VO add_vector_volume(GameApi::Env &e, VectorVolumeObject *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vectorvolume.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::VO pt;
  pt.id = env->vectorvolume.size()-1;
  return pt;
}

GameApi::VX add_int_voxel(GameApi::Env &e, Voxel<int> *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->voxels2.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::VX pt;
  pt.id = env->voxels2.size()-1;
  return pt;
}
GameApi::VX add_voxel(GameApi::Env &e, Voxel<unsigned int> *o)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->voxels.push_back(o);
  if (g_current_block != -2)
  add_b(std::shared_ptr<void>(o));
  GameApi::VX pt;
  pt.id = env->voxels.size()-1;
  return pt;
}


GameApi::CO add_color(GameApi::Env &e, int r, int g, int b, int a)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Color p = Color(r,g,b,a);
  env->colors.push_back(p);
  GameApi::CO pt;
  pt.id = env->colors.size()-1;
  //pt.type = 0;
  return pt;
}


GameApi::E add_event(GameApi::Env &e, const EventInfo &info)
{
#if 0
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  SingleEvent *event = new SingleEvent(env->event_infos, info);
  env->event_infos = event;

  GameApi::E ee;
  ee.id = event->CurrentEventNum();
  return ee;
#endif
}
GameApi::L add_link(GameApi::Env &e, GameApi::E e1, GameApi::E e2, LinkInfo info)
{
#if 0
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  LinkageInfo linkage;
  linkage.start_event = e1.id;
  linkage.end_event = e2.id;
  SingleLink *link = new SingleLink(env->event_infos, linkage, info);
  env->event_infos = link;

  GameApi::L ee;
  ee.id = link->CurrentLinkNum();
  return ee;  
#endif
}
#if 0
GameApi::MV add_mv_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->dims.push_back(new PointNDim(Point(x,y,z)));
  GameApi::MV mv;
  mv.id = env->dims.size()-1;
  return mv;
}
#endif
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv);
EventInfo find_event_info(GameApi::Env &e, GameApi::E ee);

#if 0
GameApi::MV add_line(GameApi::Env &e, GameApi::E start, GameApi::E end, 
		     GameApi::MV start_mv, GameApi::MV end_mv, int start_choose, int end_choose)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  NDim<float,Point> *start_dim = find_dim(e,start_mv);
  NDim<float,Point> *end_dim = find_dim(e, end_mv);
  std::vector<float> count_start = start_dim->Count();
  if (count_start.size()>2) { std::cout << "line() dimension error!" << std::endl; }
  std::vector<float> count_end = end_dim->Count();
  if (count_end.size()>2) { std::cout << "line() dimension error!" << std::endl; }
  EventInfo ei1 = find_event_info(e, start);
  EventInfo ei2 = find_event_info(e, end);
  float delta = ei2.time - ei1.time;
  env->dims.push_back(new NextNDim<float,Point>(*start_dim,*end_dim, delta));
  GameApi::MV mv;
  mv.id = env->dims.size()-1;
  return mv;
}
#endif

GameApi::LL add_pos(GameApi::Env &e, GameApi::L l, GameApi::MV point)
{
#if 0
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(env->event_infos));
  PosInfo pos;
  pos.link_id = l.id;
  pos.curve = env->dims[point.id];
  SinglePos *spos = new SinglePos(env->event_infos, pos);
  env->event_infos = spos;
  GameApi::LL ee;
  ee.id = spos->CurrentPosNum();
  return ee;
#endif
}

GameApi::GlobalIlluminationData *find_gi(GameApi::Env &e, GameApi::GI gi)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->globalillumination[gi.id];  
}

Timing *find_timing(GameApi::Env &e, GameApi::TT tm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->timings[tm.id];  
}

Platform *find_platform(GameApi::Env &e, GameApi::PL pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->platforms[pl.id];  
}

ICache *find_cache(GameApi::Env &e, GameApi::CX c)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->caches[c.id];  
}

IWorld *find_world(GameApi::Env &e, GameApi::W w)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->worlds[w.id];  
}
ScreenSpaceMaterial *find_screenspace_material(GameApi::Env &e, GameApi::SMT mat)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->ss_mat[mat.id];
}
ShaderCode *find_shader_code(GameApi::Env &e, GameApi::SHC code)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shc[code.id];
}
PixelBufferObject *find_pbo(GameApi::Env &e, GameApi::PBO p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->pbo[p.id];
}
ShaderI2 *find_shaderI(GameApi::Env &e, GameApi::SHI s)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shi[s.id];
}

ShaderParameterI *find_shp(GameApi::Env &e, GameApi::SHP s)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shp[s.id];
}

GLTFModelInterface *find_gltf(GameApi::Env &e, GameApi::TF tf)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->tf[tf.id];
}

GraphicsContext *find_gc(GameApi::Env &e, GameApi::GC gc)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->gc[gc.id];
}

GameState *find_game_state(GameApi::Env &e, GameApi::GS gs)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->game_state[gs.id];
}

Attach *find_attach(GameApi::Env &e, GameApi::ATT att)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->attach[att.id];
}

UrlMemoryMap *find_urlmemmap(GameApi::Env &e, GameApi::PKG urlmemmap)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->urlmemmap[urlmemmap.id];
}

MemoryBlock *find_memblock(GameApi::Env &e, GameApi::MB b)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->memblock[b.id];
}

FloatField *find_float_field(GameApi::Env &e, GameApi::FFi field)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->ffi[field.id];
}
VelocityField *find_velocity_field(GameApi::Env &e, GameApi::VFi field)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->vfi[field.id];
}
FloatArray2 *find_float_array2(GameApi::Env &e, GameApi::FA arr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->farray[arr.id];
}

DynMainLoop *find_dyn(GameApi::Env &e, GameApi::D d)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->dyn[d.id];
}

Html *find_html(GameApi::Env &e, GameApi::HML u)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->html[u.id];
}

Fetcher<FaceID> *find_uv(GameApi::Env &e, GameApi::UV u)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->uv[u.id];
}

VoxelArray *find_voxel_array(GameApi::Env &e, GameApi::AV a)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->voxel_array[a.id];
}

FrmWidget *find_frm_widget(GameApi::Env &e, GameApi::W w)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->frm_widgets[w.id];

}

HeavyOperation *find_heavy(GameApi::Env &e, GameApi::H h)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->heavys[h.id];
}

FrameBuffer *find_framebuffer(GameApi::Env &e, GameApi::FBU fb)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->frame_buffer[fb.id];
}

FrameBufferLoop *find_framemainloop(GameApi::Env &e, GameApi::FML ml)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->frame_loop[ml.id];
}

MeshAnim *find_mesh_anim(GameApi::Env &e, GameApi::MA ma)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->mesh_anim[ma.id];
}

CurveGroup *find_curve_group(GameApi::Env &e, GameApi::CG cg)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_group[cg.id];
}

InputForMoving *find_move_input(GameApi::Env &e, GameApi::INP im)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->move_input[im.id];
}
Bitmap<int> *find_int_bitmap(GameApi::Env &e, GameApi::IBM bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->int_bitmaps[bm.id];
}

DiskStore *find_disk_store(GameApi::Env &e, GameApi::DS ds)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->disk_store[ds.id];

}
ShaderBitmap *find_shader_bitmap(GameApi::Env &e, GameApi::SBM sbm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shaderbitmaps[sbm.id];
}
std::vector<float> *find_polynomial(GameApi::Env &e, GameApi::PN pn)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->polynomials[pn.id];
}
DynamicChange *find_dyn_change(GameApi::Env &e, GameApi::DC dc)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->dyn_change[dc.id];
}
ContinuousBitmap<float> *find_cont_float(GameApi::Env &e, GameApi::CFB bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cont_floats[bm.id];

}
ContinuousBitmap<bool> *find_cont_bool(GameApi::Env &e, GameApi::CBB bm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cont_bools[bm.id];

}
ShaderI *find_shader(GameApi::Env &e, GameApi::SI si)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shader_interface[si.id];
}

class TexIDEmpty : public TextureID {
public:
  TexIDEmpty(int id) : id(id) { }
  int texture() const { return id; }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual void render(MainLoopEnv &e) { }
private:
  int id;
};


TextureID *find_txid(GameApi::Env &e, GameApi::TXID id)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  TextureID *txid = env->txids[id.id];
  if (txid==0) {
    GameApi::TXID id2 = add_txid(e, new TexIDEmpty(id.id));
    txid = env->txids[id2.id];
  }
  return txid;
}
PointsInPlane *find_plane_points(GameApi::Env &e, GameApi::PLP pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_points2[pl.id];
}
LinesInPlane *find_plane_lines(GameApi::Env &e, GameApi::PLL pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_lines[pl.id];
}
FacesInPlane *find_plane_faces(GameApi::Env &e, GameApi::PLF pl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_faces[pl.id];  
}
PropertyArray *find_property_array(GameApi::Env &e, GameApi::PR pr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->prop_array[pr.id];
}
Space3d *find_space_3d(GameApi::Env &e, GameApi::SP sp)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->space_3d[sp.id];
}
CurvePatch *find_patch(GameApi::Env &e, GameApi::PA patch)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_patches[patch.id];
}
CmdExecute *find_cmds(GameApi::Env &e, GameApi::CMD cmds2)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->cmds[cmds2.id];
}
GlyphInterface *find_glyph_interface(GameApi::Env &e, GameApi::GI gi)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->glyph_interfaces[gi.id];
}

FontInterface *find_font_interface(GameApi::Env &e, GameApi::FI fi)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->font_interfaces[fi.id];
}
StringDisplay *find_string_display(GameApi::Env &e, GameApi::SD sd)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->string_displays[sd.id];
}
Fetcher<float> *find_float_fetcher(GameApi::Env &e, GameApi::FF f)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->float_fetchers[f.id];
}
Fetcher<Point> *find_point_fetcher(GameApi::Env &e, GameApi::PF f)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->point_fetchers[f.id];
}
Fetcher<int> *find_int_fetcher(GameApi::Env &e, GameApi::IF i)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->int_fetchers[i.id];
}
Fetcher<std::string> *find_string_fetcher(GameApi::Env &e, GameApi::SF s)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->string_fetchers[s.id];
}
ArrayType *find_array(GameApi::Env &e, GameApi::ARR arr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->arrays2[arr.id];
}
CurvePos *find_curve_pos(GameApi::Env &e, GameApi::CPP cp)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curve_pos[cp.id];
}
PointTransform *find_point_transform(GameApi::Env &e, GameApi::PTT ptt)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->point_transforms[ptt.id];
}
VertexAnimNode *find_vertex_anim(GameApi::Env &e, GameApi::KF kf)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->vertex_anims[kf.id];
}
Blocker *find_blocker(GameApi::Env &e, GameApi::BLK blk)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->blockers[blk.id];
}
Splitter *find_splitter(GameApi::Env &e, GameApi::RUN spl)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->splitters[spl.id];
}
PD_Impl find_polydistfield(GameApi::Env &e, GameApi::PD p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->polydistfield[p.id];
}
ColorChange *find_color(GameApi::Env &e, GameApi::CC cc)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->color_change[cc.id];
}
MixedI *find_mixed(GameApi::Env &e, GameApi::MX m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->mixed[m.id];
}
SkeletalNode *find_skeletal(GameApi::Env &e, GameApi::SA n)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->skeletals[n.id];

}
PlaneShape *find_plane_shape(GameApi::Env &e, GameApi::PP p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->plane_shapes[p.id];

}
Curve<Matrix> *find_matrix_curve(GameApi::Env &e, GameApi::MC m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->matrix_curves[m.id];
}
MatrixArray *find_matrix_array(GameApi::Env &e, GameApi::MS m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->matrix_arrays[m.id];
}
Curve<Point> *find_curve(GameApi::Env &e, GameApi::C c)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->curves[c.id];
}
ShaderCall *find_uber(GameApi::Env &e, GameApi::US m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shadercalls[m.id];
}
Material *find_material(GameApi::Env &e, GameApi::MT m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->materials[m.id];
}
TreeStack *find_tree(GameApi::Env &e, GameApi::T m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->trees[m.id];
}
TreeLevel *find_tree_level(GameApi::Env &e, GameApi::TL m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->tree_levels[m.id];
}
ImplicitFunction3d *find_implicit(GameApi::Env &e, GameApi::IM m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->implicit[m.id];
}
Movement *find_move(GameApi::Env &e, GameApi::MN m)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->movement_array[m.id];  
}
Collision *find_collision(GameApi::Env &e, GameApi::CP p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->collision_array[p.id];  
}
TriStrip *find_tri_strip(GameApi::Env &e, GameApi::TS p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->tri_strip[p.id];  
}
PhysicsNode *find_physics(GameApi::Env &e, GameApi::PH p)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  //std::cout << "find_physics: " << p.id << std::endl;
  return env->phys[p.id];  
}
ExprNode *find_expr(GameApi::Env &e, GameApi::EX n)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->exprs[n.id];  
}
template<class T>
std::vector<T> find_array(GameApi::Env &e, GameApi::A<T> arr)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  std::vector<T> vec2;
  std::vector<int> vec = env->arrays[arr.id];
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      T t;
      t.id = vec[i];
      vec2.push_back(t);
    }
  return vec2;
}
MainLoopItem *find_main_loop(GameApi::Env &e, GameApi::ML ml)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->main_loop[ml.id];
}
MainLoopItemWGPU *find_main_loop_wgpu(GameApi::Env &e, GameApi::GML ml)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->wgpu_main_loop[ml.id];
}
FontAtlasInfo *find_font_atlas(GameApi::Env &e, GameApi::FtA ft)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->font_atlas[ft.id];
}
ShaderModule *find_shader_module(GameApi::Env &e, GameApi::SFO sfo)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->shader_module[sfo.id];
}
GuiWidget *find_widget(GameApi::Env &e, GameApi::W w)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  int s = env->widgets.size();
  if (w.id>=0 && w.id<s)
    return env->widgets[w.id];
  //std::cout << "find_widget failed!" << w.id << std::endl;
  //assert(0);
  return 0;
}
Samples* find_samples(GameApi::Env &e, GameApi::SM sm)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->samples[sm.id];
}
Tracker* find_tracker(GameApi::Env &e, GameApi::TRK trk)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->trackers[trk.id];
}
EventInfo find_event_info(GameApi::Env &e, GameApi::E ee)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Event(ee.id);
}
FBOPriv *find_fbo(GameApi::Env &e, GameApi::FBO f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->fbos.size())
    return &env->fbos[f.id];
  return 0;
}
LinkageInfo find_linkage(GameApi::Env &e, GameApi::L l)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Linkage(l.id);
}
DistanceRenderable *find_distance(GameApi::Env &e, GameApi::FD fd)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (fd.id >=0 && fd.id < (int)env->distvolume.size())
    return env->distvolume[fd.id];
  return 0;
}
LazyValue<float> *find_float(GameApi::Env &e, GameApi::F f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->floats.size())
    return env->floats[f.id];
  return 0;
}
Array<int, float> *find_float_array(GameApi::Env &e, GameApi::FA f)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (f.id >=0 && f.id < (int)env->float_array.size())
    return env->float_array[f.id];
  return 0;
}
TextureI *find_texture(GameApi::Env &e, GameApi::TX t)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (t.id >=0 && t.id < (int)env->textures.size())
    return env->textures[t.id];
  return 0;
}
TexCoordQuad find_tex_quad(GameApi::Env &e, GameApi::Q t)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  if (t.id >=0 && t.id < (int)env->tex_quads.size())
    return env->tex_quads[t.id];
  TexCoordQuad q;
  Point2d ex = { 0.0, 0.0 };
  q.p1 = ex;
  q.p2 = ex;
  return q;
}


LinkInfo find_link_info(GameApi::Env &e, GameApi::L l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Links(l.id);
}
PosInfo find_pos_info(GameApi::Env &e, GameApi::LL l)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->event_infos->Positions(l.id);
}
NDim<float,Point> *find_dim(GameApi::Env &e, GameApi::MV mv)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  return env->dims[mv.id];
}

GameApi::GI add_gi(GameApi::Env &e, GameApi::GlobalIlluminationData *dt)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->globalillumination.push_back(dt);
  GameApi::GI c2;
  c2.id = env->globalillumination.size()-1;
  return c2;
}

GameApi::CX add_cache(GameApi::Env &e, ICache *c)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->caches.push_back(c);
  GameApi::CX c2;
  c2.id = env->caches.size()-1;
  return c2;
}
GameApi::W add_world(GameApi::Env &e, IWorld *w)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->worlds.push_back(w);
  GameApi::W c;
  c.id = env->worlds.size()-1;
  return c;
}

GameApi::SMT add_screenspace_material(GameApi::Env &e, ScreenSpaceMaterial *mat)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->ss_mat.push_back(mat);
  GameApi::SMT c;
  c.id = env->ss_mat.size()-1;
  return c;
}

GameApi::FFi add_float_field(GameApi::Env &e, FloatField *field)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->ffi.push_back(field);
  GameApi::FFi c;
  c.id = env->ffi.size()-1;
  return c;
}

GameApi::VFi add_velocity_field(GameApi::Env &e, VelocityField *field)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->vfi.push_back(field);
  GameApi::VFi c;
  c.id = env->vfi.size()-1;
  return c;
}
GameApi::FA add_float_array2(GameApi::Env &e, FloatArray2 *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->farray.push_back(arr);
  GameApi::FA c;
  c.id = env->farray.size()-1;
  return c;
}


GameApi::PKG add_urlmemmap(GameApi::Env &e, UrlMemoryMap *map)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->urlmemmap.push_back(map);
  GameApi::PKG c;
  c.id = env->urlmemmap.size()-1;
  return c;
}

GameApi::GS add_game_state(GameApi::Env &e, GameState *gs)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->game_state.push_back(gs);
  GameApi::GS c;
  c.id = env->game_state.size()-1;
  return c;
}

GameApi::D add_dyn(GameApi::Env &e, DynMainLoop *d2)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->dyn.push_back(d2);
  GameApi::D c;
  c.id = env->dyn.size()-1;
  return c;
}

GameApi::H add_heavy(GameApi::Env &e, HeavyOperation *h)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->heavys.push_back(h);
  GameApi::H c;
  c.id = env->heavys.size()-1;
  return c;

}

GameApi::MA add_mesh_anim(GameApi::Env &e, MeshAnim *ma)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->mesh_anim.push_back(ma);
  GameApi::MA c;
  c.id = env->mesh_anim.size()-1;
  return c;

}

GameApi::CG add_curve_group(GameApi::Env &e, CurveGroup *cg)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->curve_group.push_back(cg);
  GameApi::CG c;
  c.id = env->curve_group.size()-1;
  return c;

}

GameApi::INP add_move_input(GameApi::Env &e, InputForMoving *im)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->move_input.push_back(im);
  GameApi::INP c;
  c.id = env->move_input.size()-1;
  return c;
}

GameApi::IBM add_int_bitmap(GameApi::Env &e, Bitmap<int> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->int_bitmaps.push_back(bm);
  GameApi::IBM c;
  c.id = env->int_bitmaps.size()-1;
  return c;

}
GameApi::SBM add_shader_bitmap(GameApi::Env &e, ShaderBitmap *sbm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shaderbitmaps.push_back(sbm);
  GameApi::SBM c;
  c.id = env->shaderbitmaps.size()-1;
  return c;
}
GameApi::DC add_dyn_change(GameApi::Env &e, DynamicChange *dc)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->dyn_change.push_back(dc);
  GameApi::DC c;
  c.id = env->dyn_change.size()-1;
  return c;
}
GameApi::CFB add_cont_float(GameApi::Env &e, ContinuousBitmap<float> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cont_floats.push_back(bm);
  GameApi::CFB c;
  c.id = env->cont_floats.size()-1;
  return c;

}
GameApi::CBB add_cont_bool(GameApi::Env &e, ContinuousBitmap<bool> *bm)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cont_bools.push_back(bm);
  GameApi::CBB c;
  c.id = env->cont_bools.size()-1;
  return c;

}
GameApi::SI add_shader(GameApi::Env &e, ShaderI *sid)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->shader_interface.push_back(sid);
  GameApi::SI c;
  c.id = env->shader_interface.size()-1;
  return c;

}
GameApi::PT add_point(GameApi::Env &e, float x, float y, float z)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Point p = Point(x,y,z);
#ifndef EMSCRIPTEN
  env->lock();
#endif
  env->pt.push_back(p);
  GameApi::PT pt;
  pt.id = env->pt.size()-1;
  //pt.type = 0;
#ifndef EMSCRIPTEN
  env->unlock();
#endif
  return pt;
}

GameApi::V add_vector(GameApi::Env &e, float dx, float dy, float dz)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  Vector p = Vector(dx,dy,dz);
  env->vectors.push_back(p);
  GameApi::V pt;
  pt.id = env->vectors.size()-1;
  //pt.type = 0;
  return pt;
}
GameApi::PLP add_plane_points(GameApi::Env &e, PointsInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_points2.push_back(pl);
  GameApi::PLP c;
  c.id = env->plane_points2.size()-1;
  return c;
}
GameApi::PLL add_plane_lines(GameApi::Env &e, LinesInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_lines.push_back(pl);
  GameApi::PLL c;
  c.id = env->plane_lines.size()-1;
  return c;
}
GameApi::PLF add_plane_faces(GameApi::Env &e, FacesInPlane *pl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->plane_faces.push_back(pl);
  GameApi::PLF c;
  c.id = env->plane_faces.size()-1;
  return c;
}

GameApi::PR add_property_array(GameApi::Env &e, PropertyArray *arr)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->prop_array.push_back(arr);
  GameApi::PR c;
  c.id = env->prop_array.size()-1;
  return c;
}
GameApi::SP add_space_3d(GameApi::Env &e, Space3d *sp)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->space_3d.push_back(sp);
  GameApi::SP c;
  c.id = env->space_3d.size()-1;
  return c;
}

GameApi::CMD add_cmds(GameApi::Env &e, CmdExecute *cmds2)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cmds.push_back(cmds2);
  GameApi::CMD c;
  c.id = env->cmds.size()-1;
  return c;

}
GameApi::CT add_cutter(GameApi::Env &e, Cutter *cut)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->cutters.push_back(cut);
  GameApi::CT c;
  c.id = env->cutters.size()-1;
  return c;
}
GameApi::IS add_anim(GameApi::Env &e, const AnimImpl &impl)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  env->anim.push_back(impl);
  GameApi::IS pt;
  pt.id = env->anim.size()-1;
  //pt.type = 0;
  return pt;
}
Bitmap<Color> *find_bitmap2(GameApi::Env &e, GameApi::BM bm) {
  BitmapHandle *h = find_bitmap(e,bm);
  return find_color_bitmap(h);
}

BitmapHandle *find_bitmap(GameApi::Env &e, GameApi::BM b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BitmapHandle *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bm.size())
    handle = ee->bm[b.id];
  return handle;
}

ShaderPriv2 *find_shaderpriv(GameApi::Env &e, GameApi::SH sh)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  ShaderPriv2 *p = ee->shader_privs[sh.id];
  return p;
}
Program *find_shader_program(GameApi::Env &e, GameApi::SH sh)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  
  ShaderPriv2 *p = ee->shader_privs[sh.id];
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[sh.id]);
  return prog;
}
BitmapArray2<Color> *find_bitmap_array(GameApi::Env &e, GameApi::BMA b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BitmapArray2<Color> *h = 0;
  if (b.id>=0 && b.id<(int)ee->bm_array.size())
    {
      h = ee->bm_array[b.id];
    }
  return h;
}
BoolBitmap *find_bool_bitmap(GameApi::Env &e, GameApi::BB b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  BoolBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->bool_bm.size())
    handle = &ee->bool_bm[b.id];
  return handle;
}

Layout *find_layout(GameApi::Env &e, GameApi::LAY l)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Layout *handle = 0;
  if (l.id>=0 && l.id < (int)ee->layouts.size())
    handle = ee->layouts[l.id];
  return handle;
}

Waveform *find_waveform(GameApi::Env &e, GameApi::WV b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Waveform *handle = 0;

  if (b.id >=0 && b.id < (int)ee->waveforms.size())
    handle = ee->waveforms[b.id];
  return handle;
}


ContinuousBitmap<Color> *find_continuous_bitmap(GameApi::Env &e, GameApi::CBM b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  ContinuousBitmap<Color> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->continuous_bitmaps.size())
    handle = ee->continuous_bitmaps[b.id];
  return handle;
}

FloatBitmap *find_float_bitmap(GameApi::Env &e, GameApi::FB b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FloatBitmap *handle = 0;

  if (b.id >=0 && b.id < (int)ee->float_bm.size())
    handle = &ee->float_bm[b.id];
  return handle;
}

Voxel<unsigned int> *find_voxel(GameApi::Env &e, GameApi::VX b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Voxel<unsigned int> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->voxels.size())
    handle = ee->voxels[b.id];
  return handle;
}

Voxel<int> *find_int_voxel(GameApi::Env &e, GameApi::VX b)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Voxel<int> *handle = 0;

  if (b.id >=0 && b.id < (int)ee->voxels2.size())
    handle = ee->voxels2[b.id];
  return handle;
}


Separate* find_separate(GameApi::Env &e, GameApi::SA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Separate *sep = 0;
  if (p.id >=0 && p.id<(int)ee->separates.size())
    sep = ee->separates[p.id];
  return sep;
}

PlaneData *find_plane_array(GameApi::Env &e, GameApi::PLA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  PlaneData *data = 0;
  if (p.id>=0 && p.id<(int)ee->plane_array.size())
    data = &ee->plane_array[p.id];
  return data;
}
PlanePoints2d* find_plane(GameApi::Env &e, GameApi::PL p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  PlanePoints2d *sep = 0;
  if (p.id >=0 && p.id<(int)ee->plane_points.size())
    sep = ee->plane_points[p.id];
  return sep;
}

Array<int,ObjectWithPos> *find_move(GameApi::Env &e, GameApi::OM p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  Array<int,ObjectWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move.size())
    handle = ee->object_move[p.id];
  return handle;
}

std::vector<VertexArrayWithPos> *find_move_array(GameApi::Env &e, GameApi::VAA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  std::vector<VertexArrayWithPos> *handle = NULL;
  if (p.id >=0 && p.id < (int)ee->object_move_vertex_array.size())
    handle = ee->object_move_vertex_array[p.id];
  return handle;
}

FaceCollPolyHandle *find_poly(GameApi::Env &e, GameApi::P p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FaceCollPolyHandle *handle = 0;

  if (p.id >=0 && p.id < (int)ee->poly.size())
    handle = ee->poly[p.id];
  //std::cout << "find_poly:" << handle << std::endl;
  return handle;
}
RenderVertexArray *find_vertex_array_render(GameApi::Env &e, GameApi::VA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  RenderVertexArray *handle = 0;

  if (p.id >=0 && p.id < (int)ee->vertex_array_render.size())
    handle = ee->vertex_array_render[p.id];
  return handle;
}
VertexArraySet *find_vertex_array(GameApi::Env &e, GameApi::VA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  VertexArraySet *handle = 0;

  if (p.id >=0 && p.id < (int)ee->vertex_array.size())
    handle = ee->vertex_array[p.id];
  return handle;
}


FaceCollection *find_facecoll(GameApi::Env &e, GameApi::P p)
{
  FaceCollPolyHandle *hh = find_poly(e,p);
  //FaceCollPolyHandle *hh = dynamic_cast<FaceCollPolyHandle*>(h);
  if (!hh) return 0;
  if (hh->coll)
    return hh->coll;
  else
    return 0;
}
FunctionImpl *find_function(GameApi::Env &e, GameApi::F f)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  FunctionImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->func.size())
    handle = &ee->func[f.id];
  return handle;
}

SurfaceIn3d *find_surface2(GameApi::Env &e, GameApi::S s)
{
  return find_surface(e,s)->surf;
}
SurfaceImpl *find_surface(GameApi::Env &e, GameApi::S f)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  SurfaceImpl *handle = 0;

  if (f.id >=0 && f.id < (int)ee->surfaces.size())
    handle = &ee->surfaces[f.id];
  return handle;
}



AnimImpl find_anim(GameApi::Env &e, GameApi::IS i)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (i.id >=0 && i.id < (int)ee->anim.size())
    {
      return ee->anim[i.id];
    }
  std::cout << "find_anim: anim not found: " << i.id << std::endl;
  AnimImpl ii;
  ii.wave_int = 0;
  ii.wave_point = 0;
  ii.wave_float = 0;
  return ii;
}
Bitmap<Color> *find_color_bitmap(BitmapHandle *handle, int bbm_choose)
{
#ifndef EMSCRIPTEN
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      if (bbm_choose != -1 && bbm_choose >=0 && bbm_choose < (int)ahandle->vec.size())
	  handle = ahandle->vec[bbm_choose];
    }
#endif
#ifdef EMSCRIPTEN
   BitmapColorHandle *chandle = static_cast<BitmapColorHandle*>(handle);
#else
   BitmapColorHandle *chandle = dynamic_cast<BitmapColorHandle*>(handle);
#endif
#ifndef EMSCRIPTEN
   BitmapTileHandle *chandle2 = dynamic_cast<BitmapTileHandle*>(handle);
   if (!chandle && !chandle2) return 0;
#endif
   if (chandle)
     return chandle->bm;
#ifndef EMSCRIPTEN
   return chandle2->bm;
#else
   return 0;
#endif
}
Bitmap<Pos> *find_pos_bitmap(BitmapHandle *handle, int bbm_choose)
{
#ifndef EMSCRIPTEN
  BitmapArrayHandle *ahandle = dynamic_cast<BitmapArrayHandle*>(handle);
  if (ahandle)
    {
      if (bbm_choose != -1 && bbm_choose >=0 && bbm_choose < (int)ahandle->vec.size())
	  handle = ahandle->vec[bbm_choose];
    }
#endif
   BitmapPosHandle *chandle = static_cast<BitmapPosHandle*>(handle);
   if (!chandle) return 0;
   return chandle->bm;
}



PointsApiPoints *find_pointsapi_points(GameApi::Env &e, GameApi::PTS ps)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (ps.id >=0 && ps.id < (int)ee->pointsapi_points.size())
    return ee->pointsapi_points[ps.id];
  return 0;
}
LineCollection *find_line_array(GameApi::Env &e, GameApi::LI li)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (li.id >=0 && li.id < (int)ee->linearray.size())
    return ee->linearray[li.id];
  return 0;
}
PointArray3 *find_point_array3(GameApi::Env &e, GameApi::PTA pa)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (pa.id >=0 && pa.id < (int)ee->pointarray3.size())
    return ee->pointarray3[pa.id];
  return 0;
}
MatrixArray3 *find_matrix_array3(GameApi::Env &e, GameApi::MSA pa)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (pa.id >=0 && pa.id < (int)ee->matrixarray3.size())
    return ee->matrixarray3[pa.id];
  return 0;
}
PointArray2 *find_point_array(GameApi::Env &e, GameApi::FOA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointarray.size())
    return ee->pointarray[p.id];
  return 0;
}

PointArray2 *find_lines_array(GameApi::Env &e, GameApi::LLA p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointarray.size())
    return ee->pointarray[p.id];
  return 0;
}


PointCollection *find_pointcoll_array(GameApi::Env &e, GameApi::PC p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pointcollarray.size())
    return ee->pointcollarray[p.id];
  return 0;
}


Point *find_point(GameApi::Env &e, GameApi::PT p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->pt.size())
    return &ee->pt[p.id];
  return 0;
}
Matrix find_matrix(GameApi::Env &e, GameApi::M m)
{
  Matrix mm;
  std::copy(&m.mat[0], &m.mat[0]+16, &mm.matrix[0]);
  //mm.is_identity=false;
  return mm;
#if 0
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->matrix.size())
    return ee->matrix[p.id];
  return 0;
#endif
}

TROArray *find_timerange(GameApi::Env &e, GameApi::TR tr)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (tr.id >=0 && tr.id < (int)ee->timeranges.size())
    return ee->timeranges[tr.id];
  return 0;
}
VArray *find_timerange_vertexarray(GameApi::Env &e, GameApi::VV vv)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (vv.id >=0 && vv.id < (int)ee->timerange_vertexarrays.size())
    return ee->timerange_vertexarrays[vv.id];
  return 0;
}

VolumeObject *find_volume(GameApi::Env &e, GameApi::O o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->volumes.size())
    return ee->volumes[o.id];
  return 0;
}



FloatVolumeObject *find_float_volume(GameApi::Env &e, GameApi::FO o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->floatvolumes.size())
    return ee->floatvolumes[o.id];
  return 0;
}

ColorVolumeObject *find_color_volume(GameApi::Env &e, GameApi::COV o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->colorvolume.size())
    return ee->colorvolume[o.id];
  return 0;
}

VectorVolumeObject *find_vector_volume(GameApi::Env &e, GameApi::VO o)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (o.id >=0 && o.id < (int)ee->vectorvolume.size())
    return ee->vectorvolume[o.id];
  return 0;
}


Color *find_color(GameApi::Env &e, GameApi::CO p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->colors.size())
    return &ee->colors[p.id];
  return 0;
}


Vector *find_vector(GameApi::Env &e, GameApi::V p)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (p.id >=0 && p.id < (int)ee->vectors.size())
    return &ee->vectors[p.id];
  return 0;
}


SpaceImpl *find_space(GameApi::Env &e, GameApi::SP s)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (s.id >=0 && s.id < (int)ee->sp.size())
    return &ee->sp[s.id];
  return 0;
}
LineImpl *find_line(GameApi::Env &e, GameApi::LN l)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (l.id >=0 && l.id < (int)ee->ln.size())
    return &ee->ln[l.id];
  return 0;
}
SpritePosImpl *find_sprite_pos(GameApi::Env &e, GameApi::BM bm)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  if (bm.id >=0 && bm.id < (int)ee->spr_pos.size())
    return &ee->spr_pos[bm.id];
  return 0;
}
Cutter *find_cutter(GameApi::Env &e, GameApi::CT cut)
{
  EnvImpl *ee = ::EnvImpl::Environment(&e);
  return ee->cutters[cut.id];
}

struct Dep
{
  std::string name;
  int count;
};
bool operator<(const Dep &a1, const Dep &a2)
{
  return a1.count>a2.count;
}
std::vector<Dep> g_dep;
template<class T>
void add_dep(std::vector<T> vec, std::string name) {
  Dep d;
  d.name = name;
  d.count=vec.size();
  g_dep.push_back(d);
}




#define DEP(name) \
  add_dep(impl.name,#name)


void print_dependencies(EnvImpl &impl, int num)
{
  g_dep=std::vector<Dep>();
  
  
  DEP(pt);
  DEP(vectors);
  DEP(colors);
  DEP(volumes);
  DEP(floatvolumes);
  DEP(bm);
  DEP(bm_array);
  DEP(bool_bm);
  DEP(float_bm);
  DEP(sp);
  DEP(ln);
  DEP(spr_pos);
  DEP(anim);
  DEP(poly);
  DEP(func);
  DEP(surfaces);
  DEP(ids);
  DEP(dims);
  DEP(states);
  DEP(state_ranges); // ST type points to this array
  DEP(continuous_bitmaps);
  DEP(vertex_array);
  DEP(vertex_array_render);
  DEP(texture_id);
  DEP(shader_float_parameter);
  DEP(shader_int_parameter);
  
  DEP(voxels);
  DEP(voxels2);
  DEP(timeranges); // statechange time ranges
  DEP(timerange_vertexarrays); // statechange vertex arrays
  DEP(shaders);
  DEP(textures);
  DEP(tex_quads);
  DEP(separates);
  DEP(plane_points);
  DEP(plane_array);
  DEP(waveforms);
  DEP(object_move);
  DEP(object_move_vertex_array);
  DEP(layout_data);
  DEP(layouts);
  DEP(pointarray);
  DEP(pointarray3);
  DEP(pointcollarray);
  DEP(linearray);
  DEP(colorvolume);
  DEP(distvolume);
  DEP(vectorvolume);
  DEP(pointsapi_points);
  DEP(floats);
  DEP(float_array);
  DEP(fbos);
  DEP(samples);
  DEP(trackers);
  DEP(tracker_buf);
  DEP(wavs);
  DEP(shader_module);
  DEP(widgets);
  DEP(gameapi_modules);
  DEP(font_atlas);
  DEP(main_loop);
  DEP(arrays);
  DEP(exprs);
  DEP(phys);
  DEP(tri_strip);
  DEP(cutters);
  DEP(boolean_ops);
  DEP(polydistfield);
  DEP(handle_array);
  DEP(template_array);
  DEP(collision_array);
  DEP(movement_array);
  DEP(implicit);
  DEP(tree_levels);
  DEP(trees);
  DEP(materials);
  DEP(shadercalls);
  DEP(curves);
  DEP(matrix_arrays);
  DEP(matrix_curves);
  DEP(plane_shapes);
  DEP(skeletals);
  DEP(temp_deletes);
  DEP(polygon_array);
  DEP(va_array);
  DEP(mixed);
  DEP(color_change);
  DEP(blockers);
  DEP(splitters);
  DEP(vertex_anims);
  DEP(point_transforms);
  DEP(curve_pos);
  DEP(arrays2);
  DEP(int_fetchers);
  DEP(string_fetchers);
  DEP(float_fetchers);
  DEP(point_fetchers);
  DEP(glyph_interfaces);
  DEP(font_interfaces);
  DEP(string_displays);
  DEP(cmds);
  DEP(curve_patches);
  DEP(prop_array);
  DEP(space_3d);
  DEP(plane_lines);
  DEP(plane_points2);
  DEP(plane_faces);
  DEP(shader_interface);
  DEP(cont_floats);
  DEP(cont_bools);
  DEP(dyn_change);
  DEP(polynomials);
  DEP(shaderbitmaps);
  DEP(disk_store);
  DEP(int_bitmaps);
  DEP(move_input);
  DEP(curve_group);
  DEP(mesh_anim);
  DEP(frame_loop);
  DEP(frame_buffer);
  DEP(heavys);
  DEP(frm_widgets);
  DEP(voxel_array);
  DEP(uv);
  DEP(matrixarray3);
  DEP(html);
  DEP(dyn);
  DEP(vfi);
  DEP(ffi);
  DEP(farray);
  DEP(memblock);
  DEP(urlmemmap);
  DEP(attach);
  DEP(game_state);
  DEP(gc);
  DEP(tf);
  DEP(shi);
  DEP(shp);
  //SSSSDSSEP(pbo);
  //DssEP(shc);
  //DssEP(ss_mat);

  std::sort(g_dep.begin(),g_dep.end());
  int s=g_dep.size();
  for(int i=0;i<std::min(s,num);i++)
    {
      std::cout << g_dep[i].name << ":" << g_dep[i].count << std::endl;
    }
}
#undef DEP
