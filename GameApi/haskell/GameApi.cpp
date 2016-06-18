#include <GameApi.hh>
#include "GameApi.h"
using namespace GameApi;

extern "C" {
  int newbitmap(c_BitmapApi *api, int sx,int sy, unsigned int color)
  {
    BitmapApi *bm = (BitmapApi*)api->bmApi;
    GameApi::BM res = bm->newbitmap(sx,sy,color);
    return res.id;
  }
  int loadbitmap(c_BitmapApi *api, const char *filename)
  {
    BitmapApi *bm = (BitmapApi*)api->bmApi;
    GameApi::BM res = bm->loadbitmap(filename);
    return res.id;
  }
  c_EveryApi *geteveryApi(c_Env *api)
  {
    GameApi::Env *envptr = (GameApi::Env*)api->env;
    GameApi::EveryApi *every = new EveryApi(*envptr);
    c_EveryApi *res = new c_EveryApi;
    res->everyApi = (void*)every;
    return res;
  }
  c_MainloopApi *getmainloopapi(c_EveryApi *eapi)
  {
    GameApi::EveryApi *every = (GameApi::EveryApi*)eapi->everyApi;
    GameApi::MainLoopApi *api = &every->mainloop_api;
    c_MainloopApi *capi = new c_MainloopApi;
    capi->mainLoopApi = (void*)api;
    return capi;
  }
  c_Env *createEnv()
  {
    GameApi::Env *theEnv = new GameApi::Env;
    c_Env *env = new c_Env;
    env->env = (void*)theEnv;
    return env;
  }
  c_BitmapApi *getBitmapApi(c_EveryApi *api)
  {
    GameApi::EveryApi *every = (GameApi::EveryApi*)api->everyApi;
    GameApi::BitmapApi *bmapi = &every->bitmap_api;
    c_BitmapApi *cbmapapi = new c_BitmapApi;
    cbmapapi->bmApi = (void*)bmapi;
    return cbmapapi;
  }
  void init_window(c_MainloopApi *api, int wid, int hei)
  {
    MainLoopApi *mapi = (MainLoopApi*)api->mainLoopApi;
    mapi->init_window(wid,hei);
  }
}
