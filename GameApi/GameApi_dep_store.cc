
#include "GameApi.hh"
#include "GameApi_h.hh"
#include "GraphI.hh"

bool file_exists(std::string filename);


bool GameApi::Env::store_file_exists(std::string filename)
{
  std::string start="";
  
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder" + "/store";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  start+="/";
  std::string store_file_dir = start + filename;
  return file_exists(store_file_dir);
#endif

#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\_gameapi_builder\\" + "store";
  std::string cmd = "mkdir " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  
  start+="\\";
  std::string store_file_dir = start + filename;
  return file_exists(store_file_dir);
#endif
#ifdef EMSCRIPTEN
  std::cout << "CANNOT STORE_exists IN EMSCRIPTEN" << std::endl;
#endif
#ifdef ANDROID
  std::cout << "CANNOT STORE_exists IN ANDROID" << std::endl;
#endif
  
  return false;
}

void GameApi::Env::store_file(std::string filename, ASyncVec *vec)
{
  std::string start="";

#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder" + "/store";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  start+="/";
  std::string store_file_dir = start + filename;
  std::ofstream ss(store_file_dir.c_str());
  std::string data(vec->begin(),vec->end());
  ss << data;
  ss.close();

#endif
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\_gameapi_builder\\" + "store";
  std::string cmd = "mkdir " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  
  start+="\\";
  std::string store_file_dir = start + filename;
  std::ofstream ss(store_file_dir.c_str(),std::ios_base::out|std::ios_base::binary);
  std::string data(vec->begin(),vec->end());
  ss << data;
  ss.close();
#endif
#ifdef EMSCRIPTEN
  std::cout << "CANNOT STORE IN EMSCRIPTEN" << std::endl;
#endif
#ifdef ANDROID
  std::cout << "CANNOT STORE IN ANDROID" << std::endl;
#endif
}

struct PENDING_LOADS
{
  GameApi::Env *env;
  std::string filename;
  std::string store_filename;
  void (*fptr)(void*);
  void *data;
};
struct FINISHED_LOADS
{
  std::string filename;
  GameApi::ASyncVec *vec;
};
std::vector<PENDING_LOADS*> g_loads;
std::vector<FINISHED_LOADS> g_finished;

void g_load_cb(void *data)
{
  PENDING_LOADS *pl = (PENDING_LOADS*)data;
  GameApi::ASyncVec *vec = pl->env->get_loaded_async_url(pl->store_filename);
  //std::vector<unsigned char, GameApi::GameApiAllocator<unsigned char> > *vec = new std::vector<unsigned char,GameApi::GameApiAllocator<unsigned char> >(vec->begin(),vec->end());
  FINISHED_LOADS ld;
  ld.filename = pl->filename;
  ld.vec = vec;
  g_finished.push_back(ld);

  
  int s = g_loads.size();
  for(int i=0;i<s;i++)
    {
      if (pl->filename == g_loads[i]->filename)
	{
	  g_loads.erase(g_loads.begin()+i);
	  break;
	}
    }

  pl->fptr(pl->data);
  delete pl;
}

extern std::string gameapi_homepageurl;

GameApi::ASyncVec *g_convert(std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec);

void GameApi::Env::load_file(std::string filename, void (*fptr)(void*), void *data, bool &success)
{
#ifndef EMSCRIPTEN
  if (!store_file_exists(filename)) { std::cout << "Filename: " << filename << " does not exists!" << std::endl; success=false; return; }
#endif
  std::string start="";
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder" + "/store";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  start+="/";
  std::string store_file_dir = start + filename;
  std::ifstream ss(store_file_dir.c_str());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec = new std::vector<unsigned char, GameApiAllocator<unsigned char> >();
  char ch;
  while(ss.get(ch))
    {
      vec->push_back(ch);
    }
  FINISHED_LOADS ld;
  ld.filename = filename;
  ld.vec = g_convert(vec);
  g_finished.push_back(ld);
  success=true;
  fptr(data);
#endif
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\_gameapi_builder\\" + "store";
  std::string cmd = "mkdir " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  
  start+="\\";
  std::string store_file_dir = start + filename;
  std::ifstream ss(store_file_dir.c_str(),std::ios_base::in|std::ios_base::binary);
  std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec = new std::vector<unsigned char,GameApiAllocator<unsigned char> >();
  char ch;
  while(ss.get(ch))
    {
      vec->push_back(ch);
    }
  FINISHED_LOADS ld;
  ld.filename = filename;
  ld.vec = g_convert(vec);
  g_finished.push_back(ld);
  success=true;
  fptr(data);
#endif
#ifdef EMSCRIPTEN
  std::string store_file_dir = "./store/" + filename;
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec;
  PENDING_LOADS *pl = new PENDING_LOADS;
  pl->filename = filename;
  pl->store_filename = store_file_dir;
  pl->fptr = fptr;
  pl->data = data;
  g_loads.push_back(pl);
  success=true;
  async_load_callback(store_file_dir,&g_load_cb,(void*)pl);
  async_load_url(store_file_dir,gameapi_homepageurl);
#endif
#ifdef ANDROID
#endif

}
  
GameApi::ASyncVec *GameApi::Env::load_file_result(std::string filename)
{
  int s = g_finished.size();
  for(int i=0;i<s;i++)
    {
      if (g_finished[i].filename == filename)
	{
	  return g_finished[i].vec;
	}
    }
  return 0;
}

void GameApi::Env::load_file_clean(std::string filename)
{
  int s = g_finished.size();
  for(int i=0;i<s;i++)
    {
      if (g_finished[i].filename == filename)
	{
	  delete g_finished[i].vec;
	  g_finished.erase(g_finished.begin()+i);
	  break;
	}
    }
  
}
