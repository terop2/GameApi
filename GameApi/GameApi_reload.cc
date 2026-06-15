
#include "GameApi_h.hh"

class MaterialForward : public Material
{
public:
  GameApi::ML call(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = mat(p.id);
    return ml;
  }
  GameApi::ML call_inst(GameApi::P p, GameApi::PTS pts)
  {
    GameApi::ML ml;
    ml.id = mat_inst(p.id,pts.id);
    return ml;
  }
  GameApi::ML call_inst_matrix(GameApi::P p, GameApi::MS ms)
  {
    GameApi::ML ml;
    ml.id = mat_inst_matrix(p.id,ms.id);
    return ml;
  }
  int mat(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::ML ml = mat2(p2);
    return ml.id;
  }
  int mat_inst(int p, int pts) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst(p2,p3);
    return ml.id;
  }
  int mat_inst_va_prepare(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::VA va = mat2_inst_va_prepare(p2);
    return va.id;
  }
  int mat_inst_va(int va, int pts) const
  {
    GameApi::VA p2;
    p2.id = va;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst_va(p2,p3);
    return ml.id;
  }
  int mat_inst_matrix(int p, int ms) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::MS p3;
    p3.id = ms;
    GameApi::ML ml = mat2_inst_matrix(p2,p3);
    return ml.id;
  }

  int mat_inst2(int p, int pta) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTA p3;
    p3.id = pta;
    GameApi::ML ml = mat2_inst2(p2,p3);
    return ml.id;

  }
  int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat_inst_fade(p2,p3, flip, start_time, end_time);
    return ml.id;

  }
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::VA mat2_inst_va_prepare(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst_va(GameApi::VA va, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};


std::vector<MainLoopItem*> g_reload_instances;
std::vector<std::string> g_reload_names;
std::vector<int> g_reload_phase;

void reload_exe_cb(void *data);

class ReloadMainLoop : public MainLoopItem
{
public:
  ReloadMainLoop(GameApi::Env &env, Material *next, GameApi::P p, std::string name, int phase, GameApi::P default_p, Material *default_material, GameApi::PTS pts, GameApi::MS ms) : env(env), next(next), p(p), name(name), phase(phase), default_p(default_p), default_material(default_material), pts(pts),ms(ms)
  {
    g_reload_instances.push_back(this);
    g_reload_names.push_back(name);
    g_reload_phase.push_back(phase);
  }
  virtual void Collect(CollectVisitor &vis)
  {
    //if (use_ml.id != -1) {

    if (pts.id!=-1)
      {
	PointsApiPoints *pt = find_pointsapi_points(env,pts);
	pt->Prepare();
      }
    if (ms.id!=-1)
      {
	MatrixArray *msa = find_matrix_array(env,ms);
	msa->Prepare();
      }
    
      FaceCollection *coll1 = find_facecoll(env,default_p);
      coll1->Collect(vis);
      FaceCollection *coll2 = find_facecoll(env,p);
      coll2->Collect(vis);
      vis.register_obj(this);
      //}
  }
  virtual void HeavyPrepare()
  {
    std::cout << "HeavyPrepare" << std::endl;
    if (pts.id!=-1)
      {
	PointsApiPoints *pt = find_pointsapi_points(env,pts);
	pt->Prepare();
      }
    if (ms.id!=-1)
      {
	MatrixArray *msa = find_matrix_array(env,ms);
	msa->Prepare();
      }

      FaceCollection *coll1 = find_facecoll(env,default_p);
      coll1->Prepare();
      FaceCollection *coll2 = find_facecoll(env,p);
      coll2->Prepare();
    reload_internal(default_p,true);
  }
  virtual void Prepare()
  {
    std::cout << "Prepare" << std::endl;
    HeavyPrepare();
    //if (use_ml.id != -1) {
    //  MainLoopItem *item = find_main_loop(env,use_ml);
    //  item->Prepare();
    //}
  }
  virtual void FirstFrame() { }
  void execute()
  {
    FaceCollection *p_coll = find_facecoll(env,p);
    if (!was_ready && p_coll->Ready())
      {
	std::cout << "Model ready: reloading" << std::endl;
	reload();
      }

  }
  virtual void execute(MainLoopEnv &e)
  {
    //std::cout << "execute" << std::endl;
    FaceCollection *p_coll = find_facecoll(env,p);
    std::cout << p_coll->Ready() << "//" << was_ready << "--" << def_counter << "@@" << use_ml_real.id << "::" << use_ml_def.id << std::endl;

    execute(); 
    
    //std::cout << "USEMLid=" << use_ml.id << std::endl;
    //if (was_ready) {

    def_counter++;
    
      if (use_ml_real.id != -1 && def_counter > 30) {
	MainLoopItem *item = find_main_loop(env,use_ml_real);
	item->execute(e);
      }
      if (use_ml_def.id != -1 && def_counter <30) {
	MainLoopItem *item = find_main_loop(env,use_ml_def);
	item->execute(e);
      }
      //}
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (use_ml_def.id != -1) {
      MainLoopItem *item = find_main_loop(env,use_ml_def);
      item->handle_event(e);
    }
    if (use_ml_real.id != -1) {
      MainLoopItem *item = find_main_loop(env,use_ml_real);
      item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    if (use_ml_real.id != -1) {
      MainLoopItem *item = find_main_loop(env,use_ml_real);
      return item->shader_id();
    }
    if (use_ml_def.id != -1) {
      MainLoopItem *item = find_main_loop(env,use_ml_def);
      return item->shader_id();
    }
    return std::vector<int>();
  }

  GameApi::ML reload_fetch(GameApi::P p, GameApi::PTS pts, GameApi::MS ms, bool is_default) const
  {
    std::cout << "reload_fetch" << phase << " " << is_default << " " << p.id << std::endl; 
    GameApi::ML res;
    if (is_default)
      {

    switch(phase) {
    case 0: // mat2
      res.id = default_material->mat(p.id);
      std::cout << "res=" << res.id << std::endl;
      break;
    case 1: // mat2_inst
      res.id = default_material->mat_inst(p.id,pts.id);
      break;
    case 2: // mat2_inst_va_prepare
      //res = default_material->mat(p.id);
      break;
    case 3: // mat2_inst_va
      break;
    case 4: // mat2_inst_matrix
      res.id = default_material->mat_inst_matrix(p.id,ms.id);
      break;
    case 5: // mat2_inst2
      break;
    case 6: // mat_inst_fade
      break;
    };

	
      } else { // !is_default
      switch(phase) {
      case 0: // mat2
      res.id = next->mat(p.id);
	break;
      case 1: // mat2_inst
      res.id = next->mat_inst(p.id,pts.id);
	break;
      case 2: // mat2_inst_va_prepare
	break;
      case 3: // mat2_inst_va
	break;
      case 4: // mat2_inst_matrix
	res.id = next->mat_inst_matrix(p.id,ms.id);
	break;
      case 5: // mat2_inst2
	break;
      case 6: // mat_inst_fade
	break;
    };
    }
    return res;
  }

  void reload()
  {
    reload_internal(p,false);
  }
private:
  void reload_internal(GameApi::P p0, bool is_default)
  {
    if (is_default)
      use_ml_def = reload_fetch(p0,pts,ms,is_default);
    else
      use_ml_real = reload_fetch(p0,pts,ms,is_default);
    has_ml=true;
    if (is_default) {
      MainLoopItem *item = find_main_loop(env,use_ml_def);
      item->Prepare();
    }
    if (!is_default) {
      MainLoopItem *item = find_main_loop(env,use_ml_real);
      item->Prepare();
    }
    if (!is_default)
      was_ready = true;
  }
  
private:
  GameApi::Env &env;
  Material *next;
  Material *default_material;
  GameApi::P p = { -1 };
  GameApi::P default_p = { -1 };
  GameApi::PTS pts = { -1 };
  GameApi::MS ms = { -1 };
  std::string name;
  int phase;
  bool has_ml = false;
  mutable GameApi::ML use_ml_def = { -1 };
  mutable GameApi::ML use_ml_real = { -1 };
  bool was_ready=false;
  int def_counter=0;
};
void reload_exe_cb(void *data)
{
}


GameApi::ML GameApi::MainLoopApi::reload_ml(void *next, P p, std::string name, int phase, GameApi::P default_p, void *default_p_material, GameApi::PTS pts, GameApi::MS ms)
{
  Material *m = (Material*)next;
  return add_main_loop(e,new ReloadMainLoop(e,(Material*)next,p,name,phase,default_p,(Material*)default_p_material,pts,ms));
}


class ReloadMaterial : public MaterialForward
{
public:
  ReloadMaterial(GameApi::EveryApi &ev,Material *next, Material *default_p_material, std::string name, GameApi::P default_p) : ev(ev), next(next),name(name), default_p_material(default_p_material), default_p(default_p) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::PTS pts = { -1 };
    GameApi::MS ms = { -1 };
    return ev.mainloop_api.reload_ml((void*)next,p,name,0,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat(p.id);
    //return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::MS ms  = { -1 };
    return ev.mainloop_api.reload_ml((void*)next,p,name,1,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst(p.id, pts.id);
    //return ml;
  }
  virtual GameApi::VA mat2_inst_va_prepare(GameApi::P p) const
  {
    //GameApi::VA va;
    //return va;
    //return ev.mainloop_api.reload_ml((void*)next,p,name,2,default_p,default_p_material);
    return ev.polygon_api.create_vertex_array(p);
  }
  virtual GameApi::ML mat2_inst_va(GameApi::VA va, GameApi::PTS pts) const
  {
    
    //return ev.mainloop_api.reload_ml((void*)next,p,name,3,default_p,default_p_material);
    GameApi::ML ml;
    ml.id = next->mat_inst_va(va.id, pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    GameApi::PTS pts = { -1 };
    return ev.mainloop_api.reload_ml((void*)next,p,name,4,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst_matrix(p.id, ms.id);
    //return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::PTS pts = { -1 };
    GameApi::MS ms = { -1 };
    return ev.mainloop_api.reload_ml((void*)next,p,name,5,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst2(p.id, pta.id);
    //return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::MS ms = { -1 };
    return ev.mainloop_api.reload_ml((void*)next,p,name,6,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //return ml;
  }
private:
  GameApi::EveryApi &ev;
  Material *next;
  Material *default_p_material;
  std::string name;
  GameApi::P default_p;
  
};
GameApi::MT GameApi::MaterialsApi::reload_material(EveryApi &ev, MT next, std::string name, P default_p, MT default_mat)
{
  Material *nxt = find_material(e,next);
  FaceCollection *coll = find_facecoll(e,default_p);
  Material *def_nxt = find_material(e,default_mat);
  return add_material(e, new ReloadMaterial(ev,nxt,def_nxt,name,default_p));
}
