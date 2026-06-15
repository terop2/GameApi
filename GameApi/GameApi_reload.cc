
#include "GameApi_h.hh"
#include "MaterialI.hh"

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
  void find_ml_def() {
    if (use_ml_def.id == -1) {
      use_ml_def = reload_fetch(default_p,pts,ms,true);
    }
  }
  void find_ml_real() {
    if (use_ml_real.id == -1) {
      use_ml_real = reload_fetch(p,pts,ms,false);
    }
  }
  
  virtual void Collect(CollectVisitor &vis)
  {
    find_ml_def();
    MainLoopItem *item = find_main_loop(env,use_ml_def);
    if (item)
      item->Collect(vis);

   find_ml_real();
    MainLoopItem *item2 = find_main_loop(env,use_ml_real);
    if (item2)
      item2->Collect(vis);
    // FaceCollection *item2 = find_facecoll(env,p);
    //item2->Collect(vis);
    
    vis.register_obj(this);
    
    firsttime = false;
  }
  virtual void HeavyPrepare()
  {
    /*
    if (firsttime2) {
      find_ml_real();
      MainLoopItem *item3 = find_main_loop(env,use_ml_real);
      if (item3)
	item3->Prepare();
      firsttime2=false;
    }
    */

  }
  virtual void Prepare()
  {
    if (firsttime2) {
      find_ml_real();
      MainLoopItem *item3 = find_main_loop(env,use_ml_real);
      if (item3)
      	item3->Prepare();

      //FaceCollection *item3 = find_facecoll(env,p);
      //if (item3)
      //item3->Prepare();
      
      firsttime2=false;
    }

  }
  void logoexecute() {
    next->logoexecute();
    default_material->logoexecute();
  }
  virtual void FirstFrame() { }
  void execute()
  {
  }
  virtual void execute(MainLoopEnv &e)
  {
    FaceCollection *coll = find_facecoll(env,p);
    bool is_ready = coll->Ready();
    bool is_ready_orig = is_ready;
    if (def_counter<20) is_ready=false;
    if (def_counter<30) std::cout << "IsReady:" << is_ready_orig << " " << is_ready << std::endl;
    if (def_counter>40 && is_ready==false && firsttime2) {
      
    }
    def_counter++;
    if (!is_ready) {
    if (firsttime) {
      find_ml_def();
      MainLoopItem *item1 = find_main_loop(env,use_ml_def);
      if (item1)
	item1->Prepare();
      firsttime=false;
    }
    find_ml_def();
    MainLoopItem *item2 = find_main_loop(env,use_ml_def);
    if (item2)
      {
	item2->execute(e);
      }
    }


    if (is_ready) {
    if (firsttime2==false) {
      find_ml_real();
      MainLoopItem *item4 = find_main_loop(env,use_ml_real);
      if (item4)
	{
	  item4->execute(e);
	}
    }
    }
    
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
    find_ml_real();
    if (use_ml_real.id != -1) {
      MainLoopItem *item = find_main_loop(env,use_ml_real);
      return item->shader_id();
    }
    find_ml_def();
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
#if 0
    reload_internal(p,false);
#endif
  }
private:
  void reload_internal(GameApi::P p0, bool is_default)
  {
#if 0
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
#endif
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
  bool firsttime = true;
  bool firsttime2 = true;
};
void reload_exe_cb(void *data)
{
}


GameApi::ML reload_ml(GameApi::Env &e, Material *next, GameApi::P p, std::string name, int phase, GameApi::P default_p, Material *default_p_material, GameApi::PTS pts, GameApi::MS ms)
{
  Material *m = (Material*)next;
  return add_main_loop(e,new ReloadMainLoop(e,next,p,name,phase,default_p,default_p_material,pts,ms));
}


class ReloadMaterial : public MaterialForward
{
public:
  ReloadMaterial(GameApi::Env &e, GameApi::EveryApi &ev,Material *next, Material *default_p_material, std::string name, GameApi::P default_p) : e(e), ev(ev), next(next),name(name), default_p_material(default_p_material), default_p(default_p) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    GameApi::PTS pts = { -1 };
    GameApi::MS ms = { -1 };
    return reload_ml(e,next,p,name,0,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat(p.id);
    //return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    GameApi::MS ms  = { -1 };
    return reload_ml(e,next,p,name,1,default_p,default_p_material,pts,ms);
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
    return reload_ml(e,next,p,name,4,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst_matrix(p.id, ms.id);
    //return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    GameApi::PTS pts = { -1 };
    GameApi::MS ms = { -1 };
    return reload_ml(e,next,p,name,5,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst2(p.id, pta.id);
    //return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    GameApi::MS ms = { -1 };
    return reload_ml(e,next,p,name,6,default_p,default_p_material,pts,ms);
    //GameApi::ML ml;
    //ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    //return ml;
  }
private:
  GameApi::Env &e;
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
  return add_material(e, new ReloadMaterial(e,ev,nxt,def_nxt,name,default_p));
}
