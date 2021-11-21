
#include "GameApi_h.hh"
#include <stdio.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/fetch.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif

#define idb_disabled 1
//#define idb_disabled 0


std::string g_window_href;

long long load_size_from_url(std::string url);


std::vector<FaceCollection*> g_confirm;
std::vector<VertexArraySet*> g_confirm2;
void confirm_texture_usage(GameApi::Env &e, GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  g_confirm.push_back(coll);
}
void confirm_texture_usage(FaceCollection *coll)
{
  g_confirm.push_back(coll);
}
void confirm_texture_usage(VertexArraySet *set)
{
  g_confirm2.push_back(set);
}
void clear_texture_confirms()
{
  g_confirm.clear();
  g_confirm2.clear();
}
bool is_texture_usage_confirmed(const FaceCollection *p)
{
  int s = g_confirm.size();
  for(int i=0;i<s;i++) if (g_confirm[i]==p) return true;
  return false;
}
bool is_texture_usage_confirmed(VertexArraySet *set)
{
  int s = g_confirm2.size();
  for(int i=0;i<s;i++) if (g_confirm2[i]==set) return true;
  return false;
  
}


std::string extract_server(std::string url)
{
  int s = url.size();
  bool found = false;
  int i = 0;
  for(;i<s;i++) { if (url[i]==':') { break; } }
  i++;
  for(;i<s;i++) { if (url[i]=='/') { break; } }
  i++;
  for (;i<s;i++) { if (url[i]=='/') { break; } }
  i++;
  int pos = i;
  for(;i<s;i++) { if (url[i]=='/') { break; } }
  i++;
  int pos2 = i;
  if (pos<s && pos2<=s) return url.substr(pos,pos2-pos);
  return "";
}

bool is_urls_from_same_server(std::string url, std::string url2)
{
  //std::cout << "URL COMPARE: " << url << "==" << url2 << std::endl;
  std::string s1 = extract_server(url);
  std::string s2 = extract_server(url2);
  //std::cout << "is_urls_from_same_server::Compare:" << s1 << "==" << s2 << std::endl;
  return s1==s2;
}


#ifdef RASPI
inline int strlen(const char *ptr) { const char *p = ptr; while(*p) { p++;  } return p-ptr;}
#endif

std::string g_gpu_vendor;
extern int async_pending_count;
EnvImpl::EnvImpl() : event_infos(new EmptySequencer2), mutex(PTHREAD_MUTEX_INITIALIZER)
{
#ifndef EMSCRIPTEN
#ifdef HAS_FREETYPE
    FT_Init_FreeType(&lib);
    //std::cout << "Freetype init error: " << err << std::endl;
    //std::cout << &lib << std::endl;
#endif
#else
#ifdef HAS_FREETYPE
    FT_Init_FreeType(&lib);
#endif
#endif 
    cursor_pos_point_id.id = -1;
    async_loader = new ASyncLoader;
}

EnvImpl::~EnvImpl()
{
#if 0
  int tt4a = phys.size();
  for(int ui4=0;ui4<tt4a;ui4++)
    {
      delete phys[ui4];
    }

  int tt3 = samples.size();
  for(int iu3=0;iu3<tt3;iu3++)
    {
      delete samples[iu3];
    }

  int tt4 = trackers.size();
  for(int iu4=0;iu4<tt4;iu4++)
    {
      delete trackers[iu4];
    }

  int tt1 = floats.size();
  for(int iu1=0;iu1<tt1;iu1++)
    {
      delete floats[iu1];
    }
  int tt2 = float_array.size();
  for(int iu2=0;iu2<tt2;iu2++)
    {
      delete float_array[iu2];
    }

  int sk6 = textures.size();
  for(int ii6=0;ii6<sk6;ii6++)
    {
      TextureI *ptr = textures[ii6];
      delete ptr;
    }
  int sk5 = pointsapi_points.size();
  for(int ii5=0;ii5<sk5;ii5++)
    {
      PointsApiPoints *p = pointsapi_points[ii5];
      delete p;
    }
  int sk4 = plane_points.size();
  for(int ii4=0;ii4<sk4;ii4++)
    {
      PlanePoints2d *p = plane_points[ii4];
      delete p;
    }
  int sk3 = continuous_bitmaps.size();
  for(int ii3=0;ii3<sk3;ii3++)
    {
      ContinuousBitmap<Color> *p = continuous_bitmaps[ii3];
      delete p;
    }

  int sk2 = vertex_array_render.size();
  for(int ii2=0;ii2<sk2;ii2++)
    {
      RenderVertexArray *s = vertex_array_render[ii2];
      delete s;
    }

  int sk1 = vertex_array.size();
  for(int ii1=0;ii1<sk1;ii1++)
    {
      VertexArraySet *s = vertex_array[ii1];
      delete s;
    }


  std::map<int, ArrayRender*>::iterator it = renders.begin();
  for(;it!=renders.end();it++)
    {
      ArrayRender *rend = (*it).second;
      delete rend;
    }
  std::map<int, ArrayRender*>::iterator it2 = renders2.begin();
  for(;it2!=renders2.end();it2++)
    {
      ArrayRender *rend = (*it2).second;
      delete rend;
    }

  int vv1 = volumes.size();
  for(int i_v=0;i_v<vv1;i_v++)
    {
      delete volumes[i_v];
    }
  int vv2 = floatvolumes.size();
  for(int i_vv2=0;i_vv2<vv2;i_vv2++)
    {
      delete floatvolumes[i_vv2];
    }
  int vv3 = pointarray.size();
  for(int i_vv3=0;i_vv3<vv3;i_vv3++)
    {
      if (pointarray[i_vv3]!=0)
	{
	  delete [] pointarray[i_vv3]->array;
	  delete [] pointarray[i_vv3]->color_array;
	}
      delete pointarray[i_vv3];
    }
  int vv3a = pointarray3.size();
  for(int i_vv3a=0;i_vv3a<vv3a;i_vv3a++)
    { 
      if (pointarray3[i_vv3a]!=0)
	{
	  delete [] pointarray3[i_vv3a]->array;
	  delete [] pointarray3[i_vv3a]->color;
	}
      delete pointarray3[i_vv3a];
    }
  int vv4 = linearray.size();
  for(int i_vv4=0;i_vv4<vv4;i_vv4++)
    {
      delete linearray[i_vv4];
    }
  int vv5 = pointcollarray.size();
  for(int i_vv5=0;i_vv5<vv5;i_vv5++)
    {
      delete pointcollarray[i_vv5];
    }

  int ss1 = bool_bm.size();
  for(int i_1=0;i_1<ss1;i_1++)
    {
      BoolBitmap &bm = bool_bm[i_1];
      delete bm.bitmap;
    }
  int ss1a = waveforms.size();
  for(int i_1a=0;i_1a<ss1a;i_1a++)
    {
      Waveform *bm = waveforms[i_1a];
      delete bm;
    }
  int ss2 = float_bm.size();
  for(int i_2=0;i_2<ss2;i_2++)
    {
      FloatBitmap &bm = float_bm[i_2];
      delete bm.bitmap;
    }

  int s0 = fonts.size();
  for(int i0=0;i0<s0;i0++)
    {
      Font f = fonts[i0];
      delete f.bm;
    }
#ifndef EMSCRIPTEN
#ifdef HAS_FREETYPE
  //std::cout << "FREE FREETYPE!" << std::endl;
  FT_Done_FreeType(lib);
#endif
#endif
  int s1 = bm.size();
  for(int i1=0;i1<s1;i1++)
    {
      BitmapHandle *handle = bm[i1];
      //std::cout << "EnvImpl destructor: " << handle << std::endl;
      delete handle;
    }
  int s2 = anim.size();
  for(int i2=0;i2<s2;i2++)
    {
      AnimImpl *impl = &anim[i2];
      delete impl->wave_int;
      delete impl->wave_point;
      delete impl->wave_float;
    }

  int s3 = poly.size();
  for(int i3=0;i3<s3;i3++)
    {
      FaceCollPolyHandle *handle = poly[i3];
      delete handle;
    }
  int s4 = func.size();
  for(int i4=0;i4<s4;i4++)
    {
      FunctionImpl f = func[i4];
      delete f.func;
    }
  int s5 = surfaces.size();
  for(int i5=0;i5<s5;i5++)
    {
      SurfaceImpl s = surfaces[i5];
      delete s.surf;
    }  
  delete event_infos;

#if 0
  int s6 = matrix.size();
  for(int i6=0;i6<s6;i6++)
    {
      MatrixInterface *i = matrix[i6];
      delete i;
    }
#endif
#endif
}

GameApi::Env *Global_latest_env = 0;

EXPORT GameApi::Env *GameApi::Env::Latest_Env()
{
  return Global_latest_env;
}
EXPORT GameApi::Env::Env()
{
  Global_latest_env = this;
  envimpl = (void*)new ::EnvImpl;
}
EXPORT void GameApi::Env::free_memory()
{ 
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_memory();
}
EXPORT std::vector<int> GameApi::Env::store_counts()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 return env->store_counts();
}
EXPORT void GameApi::Env::free_to_counts(std::vector<int> vec)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_to_counts(vec);
}
EXPORT void GameApi::Env::free_temp_memory()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
 env->free_temp_memory();

}

void InstallProgress(int num, std::string label, int max);
void ProgressBar(int num, int val, int max, std::string label);


EXPORT void GameApi::Env::async_load_url(std::string url, std::string homepage)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->load_urls(url, homepage);

}
EXPORT void GameApi::Env::async_load_all_urls(std::vector<std::string> urls, std::string homepage)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->load_all_urls(urls, homepage);

}
EXPORT void GameApi::Env::async_load_callback(std::string url, void (*fptr)(void*), void *data)
{
  //std::cout << "async_load_callback: " << url << std::endl;
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->set_callback(url, fptr, data);
}
EXPORT void GameApi::Env::async_rem_callback(std::string url)
{
  //std::cout << "async_load_rem_callback: " << url << std::endl;
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->rem_callback(url);
}
EXPORT GameApi::ASyncVec *GameApi::Env::get_loaded_async_url(std::string url)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;

  return env->async_loader->get_loaded_data(url);
}

EXPORT GameApi::Env::~Env()
{
  delete (::EnvImpl*)envimpl;
}
std::string remove_load(std::string s);
std::vector<unsigned char> *load_from_url(std::string url);
std::string striphomepage(std::string url);

struct del_map
{
  void del_url(std::string url)
  {
    //std::vector<unsigned char>* v = load_url_buffers_async[url];
    //delete v;
    //load_url_buffers_async[url] = 0;
  }
  ~del_map() {
    //std::map<std::string,std::vector<unsigned char>*>::iterator i = load_url_buffers_async.begin();
    //for(;i!=load_url_buffers_async.end();i++)
    //  {
    //	std::pair<std::string,std::vector<unsigned char>*> p = *i;
    //	std::cout << std::hex << (long)p.second << "::" << p.first << std::endl;
    //delete p.second;
    // }
  }
  std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;
};
del_map g_del_map;

struct ASyncCallback { void (*fptr)(void*); void *data; };
struct ASyncCallback2 { std::string url; ASyncCallback *cb; };
//std::map<std::string, ASyncCallback*> load_url_callbacks;
std::vector<ASyncCallback2> load_url_callbacks;
void add_async_cb(std::string url, ASyncCallback *cb)
{
  //std::cout << "add_async_cb:" << url << std::endl;
  ASyncCallback2 cb2;
  cb2.url = url;
  cb2.cb = cb;
  load_url_callbacks.push_back(cb2);
}
ASyncCallback *rem_async_cb(std::string url)
{
  //std::cout << "rem_async_cb:" << url << std::endl;
  int s = load_url_callbacks.size();
  int i = 0;
  for(;i<s;i++)
    {
      //std::cout << "rem_async_cb: " << url << " <=> " << load_url_callbacks[i].url << std::endl;
      if (load_url_callbacks[i].url==url) break;
    }
  if (i==s) { /*std::cout << "rem_async_cb failure!" << std::endl;*/ return 0; }
  ASyncCallback *cb = load_url_callbacks[i].cb;
  load_url_callbacks.erase(load_url_callbacks.begin()+i);
  return cb;
}


std::string striphomepage(std::string);
void onprogress_async_cb(unsigned int tmp, void *, int, int) { }
void onerror_async_cb(unsigned int tmp, void *arg, int, const char*str)
{
  std::cout << "ERROR: url loading error! " << std::endl;
  if (!arg) return;
  std::cout << str << std::endl;
  char *url = (char*)arg;
  std::cout << url << std::endl;
    std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  g_del_map.load_url_buffers_async.erase(url_only);
  //load_url_buffers_async[url_only] = (std::vector<unsigned char>*)-1;
    async_pending_count--;
    //std::cout << "ASync pending dec (onerror_async_cb) -->" << async_pending_count << std::endl;
    
    ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  if (cb) {
    //std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
  }

}
void dummy_cb(void*) { }
std::string striphomepage(std::string);
std::string stripprefix(std::string s)
{
  //std::cout << "stripprefix:" << s << std::endl;
  int len = strlen("load_url.php?url=");
  if (s.size()>0 && s[0]=='l') {
    return s.substr(len,s.size()-len);
  } else return s;
}
void onload_async_cb(unsigned int tmp, void *arg, void *data, unsigned int datasize)
{
  char *url = (char*)arg;
  //std::cout << "onload_async_cb: " << url << std::endl;

  unsigned char *dataptr = (unsigned char*)data;
  if (datasize==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
      std::cout << url << std::endl;
  }
  std::vector<unsigned char> *buffer = new std::vector<unsigned char>(dataptr,dataptr+datasize);
  //unsigned char *dataptr = (unsigned char*)data;
  //for(unsigned int i=0;i<datasize;i++) { buffer->push_back(dataptr[i]); }
  
  //char *url = (char*)arg;
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,7,15,url_only2);
  }
  
  //std::cout << "url loading complete! " << url_str << std::endl;
  // THIS WAS url_only, but seems to have not worked.
  g_del_map.load_url_buffers_async[url_only] = buffer;
  async_pending_count--;
  //std::cout << "ASync pending dec (onload_async_cb) -->" << async_pending_count<< std::endl;
  
  //std::cout << "Async cb!" << url_only << std::endl;
  ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  while (cb) {
    //std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
    cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  }
  ASyncCallback *cb2 = rem_async_cb(url);
  while(cb2) {
    (*cb2->fptr)(cb2->data);
    cb2 = rem_async_cb(url); //load_url_callbacks[url_only];
  }
    std::string url2 = stripprefix(url_only);
  ASyncCallback *cb3 = rem_async_cb(url2);
  while(cb3) {
    (*cb3->fptr)(cb3->data);
    cb3 = rem_async_cb(url2); //load_url_callbacks[url_only];
  }

  std::string url3 = striphomepage(url2);
  ASyncCallback *cb4 = rem_async_cb(url3);
  while(cb4) {
    (*cb4->fptr)(cb4->data);
    cb4 = rem_async_cb(url3); //load_url_callbacks[url_only];
  }
  
#ifdef EMSCRIPTEN
  if (tmp!=333) {
#if 0
    std::string url_store = stripprefix(url_only);
    //std::cout << "Store:" << url_store << std::endl;
    emscripten_idb_async_store("gameapi", url_store.c_str(), &g_del_map.load_url_buffers_async[url_only]->operator[](0), g_del_map.load_url_buffers_async[url_only]->size(), 0, &dummy_cb, &dummy_cb);
#endif
  }
#endif
  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url_only2);
  }

}

std::vector<unsigned char> *load_from_url(std::string url);

struct CallbackDel
{
  std::vector<ASyncCallback*> vec;
  ~CallbackDel() { int s = vec.size(); for(int i=0;i<s;i++) delete vec[i]; }
};
CallbackDel cb_del;

void ASyncLoader::rem_callback(std::string url)
{
  rem_async_cb(url);
}
void ASyncLoader::set_callback(std::string url, void (*fptr)(void*), void *data)
{
  // progress bar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url,15);


  url = "load_url.php?url=" + url;
  ASyncCallback* cb = new ASyncCallback;
  cb_del.vec.push_back(cb);
  cb->fptr = fptr;
  cb->data = data;
  //load_url_callbacks[url] = cb;
  add_async_cb(url,cb);



  //std::cout << "async set callback" << url << std::endl;
}

void *process2(void *ptr)
{
  std::string *str = (std::string*)ptr;
  pthread_detach(pthread_self());
  system(str->c_str());
  //pthread_exit(0);
  return 0;
}

void pthread_system(std::string str)
{
  std::string *ss = new std::string(str);
  pthread_t thread_id;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 300000);
  pthread_create(&thread_id, &attr, &process2, (void*)ss);
}


#ifdef WINDOWS
#ifdef THREADS
pthread_t g_main_thread;
#endif
#endif

struct ProcessData
{
  pthread_t thread_id;
  std::string url;
};

void* process(void *ptr)
{
  ProcessData *dt = (ProcessData*)ptr;
  std::string url = dt->url;
  pthread_detach(pthread_self());
  std::vector<unsigned char> *buf = load_from_url(url);
  std::string url2 = "load_url.php?url=" + url ;
  g_del_map.load_url_buffers_async[url2] = buf; //new std::vector<unsigned char>(buf);  
  //pthread_exit(0);
  return 0;
}

bool g_progress_already_done = false;
long long g_current_size=0;
void ASyncLoader::load_all_urls(std::vector<std::string> urls, std::string homepage)
{
  //std::cout<< "URLS_SIZE:" << urls.size() << std::endl;
#if 0
  int s2 = urls.size();
  for(int i=0;i<s2;i++)
    {
      std::string url = urls[i];
      load_urls(url,homepage);
    }
  return;
#endif

  g_progress_already_done = true;
#ifndef EMSCRIPTEN
  int s = urls.size();
  std::vector<std::string> urls1 = urls;

  std::vector<std::string> urls2;
  for (int e=0;e<s;e++)
    {
      std::string url = urls[e];
      std::string url2 = "load_url.php?url=" + url ;
      //std::cout << url2 << std::endl;
      if (g_del_map.load_url_buffers_async[url2])
	{
	  
	  //ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
	  //if (cb) {
	//std::cout << "Load cb!" << url2 << std::endl;
	//std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
	//(*cb->fptr)(cb->data);
	  //} else {
	//std::cout << "ASyncLoadUrl::CB failed" << std::endl;
	  //}
	  
	}
      else
	{
	  urls2.push_back(url);
	}
    }
  urls=urls2;
  s = urls.size();
  if (!s)
    {
	InstallProgress(444,"loading assets (cached)",15);
	ProgressBar(444,15,15,"loading assets (cached)");

    }
  //int sk = s;
  //for(int u=0;u<sk;u+=10) { 
  int u=0;

    {
	InstallProgress(444,"loading assets",15);
	ProgressBar(444,0,15,"loading assets");
    }

  
  long long total_size = 0;
  std::vector<long long> sizes;
  //s=std::min(10,s-u);
  for(int d=0;d<s;d++)
    {
      std::string url = urls[u+d];
      long long sz = load_size_from_url(url);
      //std::cout << "SZ:" << url << "=" << sz << std::endl;
      sizes.push_back(sz);
      total_size+=sz;
    }
  g_current_size = 0;
  
  std::vector<ProcessData*> vec;

    

    std::vector<ProcessData*> dt;
  for(int i=0;i<s;i++) {
    std::string url = urls[u+i];

    ProcessData *processdata = new ProcessData;
    dt.push_back(processdata);
    processdata->url = url;
    vec.push_back(processdata);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 300000);
    int err = pthread_create(&processdata->thread_id, &attr, &process, (void*)processdata);
    if (err) { std::cout << "pthread_create returned error " << err << std::endl; }
  }
  long long current_size = 0;
  long long last_size = 0;
  for(int j=0;j<s;j++)
    {
      void *res;
      std::string url = urls[u+j];
      std::string url2 = "load_url.php?url=" + url ;
      while (!g_del_map.load_url_buffers_async[url2])
	{
	  if (g_current_size > last_size+(total_size/15))
	  {
	    //int s = url.size();
	    //int sum=0;
	    //for(int i=0;i<s;i++) sum+=int(url[u+i]);
	    //sum = sum % 1000;
	    //std::cout << g_current_size << "*15/" << total_size << std::endl; 
	    ProgressBar(444,g_current_size*15/total_size,15,"loading assets");
	  last_size=g_current_size;
	  }
#ifdef LINUX
	  //usleep(100000);
#endif
	}
      // pthread_join(vec[j]->thread_id,&res);
      //ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
      //if (cb) {
	//std::cout << "Load cb!" << url2 << std::endl;
	//std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
      //	(*cb->fptr)(cb->data);
      //} else {
	//std::cout << "ASyncLoadUrl::CB failed" << std::endl;
      //}
      current_size+=sizes[u+j];

      
      
      //{
      //int s = url.size();
      //int sum=0;
      //for(int i=0;i<s;i++) sum+=int(url[i]);
      //sum = sum % 1000;
      //ProgressBar(444,current_size*15/total_size,15,"loading assets");
      //}
      
    }
  //  }

  int sr = dt.size();
  for(int i=0;i<sr;i++)
    {
      delete dt[i];
    }
  
  int ssk = urls1.size();
  for(int j=0;j<ssk;j++)
    {
      void *res;
      std::string url = urls1[u+j];
      std::string url2 = "load_url.php?url=" + url ;

      ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
      if (cb) {
	//std::cout << "Load cb!" << url2 << std::endl;
	//std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
	(*cb->fptr)(cb->data);
      } else {
	//std::cout << "ASyncLoadUrl::CB failed" << std::endl;
      }

    }
	ProgressBar(444,15,15,"loading assets");
  
#endif
  g_progress_already_done = false;

}
struct LoadData {
  std::string url;
  std::string url3;
  char *buf2;
  char *buf3;
};

void idb_onload_async_cb(void *ptr, void* data, int datasize)
{
  onload_async_cb(333,ptr,data,datasize);
}
void idb_onerror_async_cb(void *ptr)
{

    onerror_async_cb(0,ptr,0,"");
}

#ifdef EMSCRIPTEN
void fetch_download_succeed(emscripten_fetch_t *fetch) {
  //std::cout << "Fetch success: " << fetch->numBytes << std::endl;
  //std::cout << "Fetch data:" << (unsigned char*)fetch->data << std::endl;
  LoadData *data =(LoadData*)fetch->userData;
  const char *url = data->buf3;
  onload_async_cb(333, (void*)url, (void*)fetch->data, fetch->numBytes+1);
  emscripten_fetch_close(fetch);
}
void fetch_download_failed(emscripten_fetch_t *fetch) {
  //std::cout << "Fetch failed: " << fetch->numBytes << std::endl;
  LoadData *data =(LoadData*)fetch->userData;
  const char *url = data->buf3;
  onerror_async_cb(333, (void*)url, 0, "fetch failed!");
  emscripten_fetch_close(fetch);
}
extern int g_logo_status;
void fetch_download_progress(emscripten_fetch_t *fetch) {
  //std::cout << "fetch progress:" << fetch->dataOffset << " " << fetch->totalBytes << std::endl;
  int val = 7;
  if (fetch->totalBytes) {
    val = fetch->dataOffset * 15 / fetch->totalBytes;
  } else {

    size_t sz = emscripten_fetch_get_response_headers_length(fetch);
    char *buf = new char[sz+1];
    size_t sz2 = emscripten_fetch_get_response_headers(fetch, buf, sz+1);
    //std::cout << "HEADERS:" << buf << std::endl;

    char**headers = emscripten_fetch_unpack_response_headers(buf);
    char**ptr = headers;
    char *res = 0;
    for(;*ptr;ptr+=2) {
      //std::cout << ptr[0] << "::" << ptr[1] << std::endl;
      if (strcmp(ptr[0],"content-length")==0) res = ptr[1];
    }
    int total = 0;
    if (res) {
      std::stringstream ss(res);
      ss >> total;
    }
    //std::cout << "GOT CONTENT LENGTH: " << total << std::endl;
    if (total != 0)
      val = 15*(fetch->dataOffset + fetch->numBytes)/total;
  }
  //std::cout << "logo:" << val << std::endl;
  if (g_logo_status==0)
    g_logo_status = 1;
  
  LoadData *data =(LoadData*)fetch->userData;
  const char *url = data->buf3;
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,val,15,"fetch: "+url_only2);
  }

  
}
#endif

void idb_exists(void *arg, int exists)
{
#ifdef EMSCRIPTEN
  LoadData *ld = (LoadData*)arg;
  //std::cout << "Exists: " << exists << std::endl;
  if (exists && !idb_disabled) {
    //std::cout << "Loading from idb" << ld->url << std::endl;
    emscripten_idb_async_load("gameapi", ld->url.c_str(), (void*)ld->buf3, &idb_onload_async_cb, &idb_onerror_async_cb);
  } else {
    //std::cout << "Loading from wget" << ld->url << std::endl;
    emscripten_async_wget2_data(ld->buf2, "POST", ld->url3.c_str(), (void*)ld->buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
  }
#endif
}
void idb_error(void *arg)
{
  std::cout << "indexdb error branch, loading wget data again..." << std::endl;
#ifdef EMSCRIPTEN
  LoadData *ld = (LoadData*)arg;
    emscripten_async_wget2_data(ld->buf2, "POST", ld->url3.c_str(), (void*)ld->buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
#else
  LoadData *ld = (LoadData*)arg;
    onerror_async_cb(0,(void*)ld->buf3,0,"");
#endif
}
std::vector<std::string> g_currently_loading;
std::string remove_load(std::string s);

extern std::vector<const unsigned char*> g_content;
extern std::vector<const unsigned char*> g_content_end;
extern std::vector<const char*> g_urls;


void ASyncLoader::load_urls(std::string url, std::string homepage)
  {
    //std::cout << "load_urls:" << url << std::endl;
    if (url=="") return;
    //std::cout << "ASyncLoader::load_urls:" << url << std::endl; 

  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      if (remove_load(url)==g_urls[i]) { 
	//std::vector<unsigned char> *vec = new std::vector<unsigned char>(g_content[i], g_content_end[i]);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	std::string url_only = "load_url.php?url=" + url;

	g_del_map.load_url_buffers_async[url_only] = new std::vector<unsigned char>(g_content[i],g_content_end[i]);
	//async_pending_count--;

	
	std::string oldurl = url;
	url = "load_url.php?url=" + url + "&homepage=" + homepage;

	ASyncCallback *cb = rem_async_cb(oldurl); //load_url_callbacks[url];
	if (cb) {
	  //std::cout << "Load cb!" << url << std::endl;
	(*cb->fptr)(cb->data);
	}
      ASyncCallback *cb2 = rem_async_cb(url); //load_url_callbacks[url];
      if (cb2) {
	//std::cout << "Load cb!2" << url << std::endl;
	(*cb2->fptr)(cb2->data);
      }
      ASyncCallback *cb3 = rem_async_cb(url_only); //load_url_callbacks[url];
      if (cb3) {
	//std::cout << "Load cb!3" << url << std::endl;
	(*cb3->fptr)(cb3->data);
      }

      { // progressbar
	std::string url_plain = stripprefix(url);
	int s = url_plain.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url_plain[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url_plain);
      }
	
	return;
      }
    }
    
    std::string oldurl = url;

    //int s = g_currently_loading.size();
    //for(int i=0;i<s;i++) {
    //  if (g_currently_loading[i]==oldurl) {
    //	return;
    //  }
    // }

    
  // progress bar
    {
    std::string url2 = "load_url.php?url=" + url ;
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  //std::cout << "InstallProgress:" << sum << " " << url << std::endl;
  if (g_del_map.load_url_buffers_async[url2]) { 
    InstallProgress(sum,url + " (cached)",15);
  } else {
  InstallProgress(sum,url,15);

  }
    }

#ifdef EMSCRIPTEN
    std::string olderurl = url;
  std::string url2 = "load_url.php";
  std::string urlend = "url=" + url;
  std::string url3 = urlend + "&homepage=" + homepage;
  std::string url_only = "load_url.php?url=" + url;
  url = "load_url.php?url=" + url + "&homepage=" + homepage;


  std::string body = olderurl + "|" + homepage;
  //std::cout << "Request body: " << body << std::endl;
    char *body_buf = new char[body.size()+1];
    std::copy(body.begin(), body.end(), body_buf);
    body_buf[body.size()]=0;


    const char * headers[] = { "Content-type", "application/json; charset=UTF-8", 0};
    
    //std::cout << "url loading started! " << url << std::endl;

  //std::cout << "URL:" << url << std::endl;
    // if we have already loaded the same url, don't load again
  if (/*load_url_buffers_async[url] ||*/g_del_map.load_url_buffers_async[url_only]) {
    //std::cout << "URL FROM CACHE" << std::endl;
      ASyncCallback *cb = rem_async_cb(oldurl); //load_url_callbacks[url];
      if (cb) {
	//std::cout << "Load cb!" << url << std::endl;
	(*cb->fptr)(cb->data);
      }
      ASyncCallback *cb2 = rem_async_cb(url); //load_url_callbacks[url];
      if (cb2) {
	//std::cout << "Load cb!" << url << std::endl;
	(*cb2->fptr)(cb2->data);
      }
      ASyncCallback *cb3 = rem_async_cb(url_only); //load_url_callbacks[url];
      if (cb3) {
	//std::cout << "Load cb!" << url << std::endl;
	(*cb3->fptr)(cb3->data);
      }

      { // progressbar
	std::string url_plain = stripprefix(url);
	int s = url_plain.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url_plain[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url_plain);
      }

      return; 
    }
  //std::cout << "URL LOADED REALLY..." << std::endl;
      //g_currently_loading.push_back(oldurl);
    char *buf2 = new char[url2.size()+1];
    std::copy(url2.begin(), url2.end(), buf2);
    buf2[url2.size()]=0;

    char *buf3 = new char[url.size()+1];
    std::copy(url.begin(), url.end(), buf3);
    buf3[url.size()]=0;
    
    async_pending_count++;
    //std::cout << "ASync pending inc (load_urls) -->" << async_pending_count << std::endl;

    //emscripten_async_wget_data(buf2, (void*)buf2 , &onload_async_cb, &onerror_async_cb);

    //std::cout << "Fetch start" << std::endl;

    
    LoadData *ld = new LoadData;
    ld->buf2 = buf2;
    ld->buf3 = buf3;
    ld->url = oldurl;
    ld->url3 = url3;

    bool is_same_server = is_urls_from_same_server(oldurl,g_window_href);
    bool is_same_server2 = is_urls_from_same_server(oldurl,homepage+"/");
    
    //emscripten_idb_async_exists("gameapi", oldurl.c_str(), (void*)ld, &idb_exists, &idb_error);
    if (extract_server(oldurl)=="" || is_same_server || is_same_server2) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.userData = (void*)ld;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY|256;
    attr.onsuccess = fetch_download_succeed;
    attr.onerror = fetch_download_failed;
    attr.onprogress = fetch_download_progress;
    //attr.onreadystatechange = fetch_headers_received;
    if (!is_same_server) {
      attr.requestData = (char*)body_buf;
      attr.requestDataSize = body.size()+1;
    }
    attr.requestHeaders = headers;
    if (!is_same_server) {
      emscripten_fetch(&attr, "load_url.php");
    } else {
      emscripten_fetch(&attr, oldurl.c_str());
    }
    //emscripten_async_wget2_data(buf2, "POST", url3.c_str(), (void*)buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);

    //std::cout << "Fetch end" << std::endl;

    } else { std::cout << "WARNING: Security violation, trying to access urls that are outside allowed ownership area:" << oldurl << " isn't in domains '" << extract_server(g_window_href) << "' or '" << extract_server(homepage) << "'" << std::endl; }
#if 0
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = &onload_async_cb;
    attr.onerror = &onerror_async_cb;
    //    attr.onprogress = &onprogress_async_cb;
    emscripten_fetch(&attr,buf3);
#endif
#else
  { // progressbar
    std::string url2 = "load_url.php?url=" + url ;
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  if (g_del_map.load_url_buffers_async[url2]) { 
    ProgressBar(sum,0,15,url+" (cached)");
  } else {
    ProgressBar(sum,0,15,url);
  }
  }

    std::string url2 = "load_url.php?url=" + url ;
    if (g_del_map.load_url_buffers_async[url2]) { 
      { // progressbar
	int s = url.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url + " (cached)");
      }

      return; }
    //std::cout << "Loading url: " << url <<std::endl;
    std::vector<unsigned char> *buf = load_from_url(url);
    //std::cout << "Loading url finished: " << url <<std::endl;
    if (buf->size()==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
    }
    g_del_map.load_url_buffers_async[url2] = buf; //new std::vector<unsigned char>(buf);
    //std::cout << "Async cb!" << url2 << std::endl;
    ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
    if (cb) {
      //std::cout << "Load cb!" << url2 << std::endl;
      //std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
      (*cb->fptr)(cb->data);
    } else {
      //std::cout << "ASyncLoadUrl::CB failed" << std::endl;
    }

      {

    // REASON, ProgressBar cannot be called from other threads.
#if 1
  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url);
  }
#endif
      }


#endif
  }

class ASyncDataFetcher : public GameApi::ASyncVec
{
public:
  ASyncDataFetcher(std::vector<unsigned char> *vec) : vec(vec),buf(0),end2(0) { }
  ASyncDataFetcher(const unsigned char *buf,const unsigned char *end) : buf(buf), end2(end) { }
  const unsigned char &operator[](int i) const {
    if (buf && buf+i<end2) return buf[i];
    return (*vec)[i];
  }
  int size() const { if (buf) return end2-buf; return vec->size(); }
  const unsigned char *begin() const { if (buf) return buf; return &(*vec->begin()); }
  const unsigned char *end() const { if (end2) return end2; return &(*vec->end()); }
private:
  std::vector<unsigned char> *vec;
  const unsigned char *buf, *end2;
};

GameApi::ASyncVec *ASyncLoader::get_loaded_data(std::string url) const
  {
  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;

      if (remove_load(url)==g_urls[i]) { 
	//std::vector<unsigned char> *vec = new std::vector<unsigned char>(g_content[i], g_content_end[i]);
	return new ASyncDataFetcher(g_content[i], g_content_end[i]);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	//	return vec;
      }
    }


    
    url = "load_url.php?url=" + url;
    //std::cout << "url fetch " << url << std::endl;
    return new ASyncDataFetcher(g_del_map.load_url_buffers_async[url]);
  }


struct ProgressI {
  int num;
  int value;
  int ticks;
};
void SetTicks(int num, int ticks);


std::string g_original_title;
Low_SDL_Window *sdl_window;
int g_last_tick=0;
bool g_has_title=false;
std::vector<ProgressI > progress_max;
std::vector<ProgressI > progress_val;
std::vector<std::string> progress_label;
void ClearProgress() { progress_max.clear(); progress_val.clear(); progress_label.clear(); }
void InstallProgress(int num, std::string label, int max=15)
{
  //std::cout << "InstallProgress: " << num << " " << label << " " << max << std::endl;
  //std::cout << "InstallProgress: '" << label << "'" << std::endl;
  //std::cout << "IB: " << num << std::endl;
  ProgressI p; p.num = num; p.value=0; p.ticks=0;
  //p.ticks = g_low->sdl->SDL_GetTicks();
  //g_last_tick = p.ticks;
  int s = progress_max.size();
  int s2 = progress_val.size();
  bool max_done = false;
  bool val_done = false;
  for(int i=0;i<std::min(s,s2);i++) {
    if (progress_max[i].num==num) max_done=true;
    if (progress_val[i].num==num) val_done=true;
  }
  if (!max_done)
    progress_max.push_back(p);
  if (!val_done)
    progress_val.push_back(p);
  if (!max_done)
    progress_label.push_back(label);
  SetTicks(num, p.ticks);
  ProgressBar(num,0,max,"installprogress");
#ifndef EMSCRIPTEN
    // g_low->sdl->SDL_SetWindowTitle(sdl_window, l.c_str());
  // std::cout << std::endl;
#endif

}

void SetTicks(int num, int ticks) {
  int s = progress_val.size();
  for(int i=0;i<s;i++) {
    if (progress_val[i].num == num) {
      progress_val[i].ticks = ticks;
    }
  }
}

int progress_remove_list[] = { 1, 434, 555 };

float progress_val_mult[] = { 0.1, 0.3, 0.5, 0.8, 1.0 };

int FindProgressVal()
{
  int s = progress_val.size();
  int sum = 0;
  for(int i=0;i<s;i++) {
    bool skip =false;
    for(int j=0;j<sizeof(progress_remove_list)/sizeof(int);j++)
      if (progress_val[i].num==progress_remove_list[j]) skip=true;
    if (!skip) {
      float mult = 1.0;
      if (s<sizeof(progress_val_mult)/sizeof(progress_val_mult[0]))
	mult = progress_val_mult[s];
      sum+=progress_val[i].value*mult;
      //std::cout << "ProgressVal:" << progress_val[i].num << " " << progress_val[i].value << " " << progress_label[i] << std::endl;
    }
  }
  return sum;
}
int g_async_load_count = 0;
int FindProgressMax()
{
  int s = progress_max.size();
  int sum = 0;
  for(int i=0;i<s;i++)
    {
    bool skip =false;
    for(int j=0;j<sizeof(progress_remove_list)/sizeof(int);j++)
      if (progress_val[i].num==progress_remove_list[j]) skip=true;
    if (!skip) {
      sum+=progress_max[i].value;
      //std::cout << "ProgressMax:" << progress_val[i].num << " " << progress_max[i].value << " " << progress_label[i] << std::endl;
    }
    }
  if (s<4) { sum+=15*(4-s); }
  return sum;
}
void FinishProgress()
{
  if (g_has_title) {
	//int tick = g_low->sdl->SDL_GetTicks();
	//int delta = tick-g_last_tick;
	//if (delta>2000 && g_has_title) {
	//std::string l = g_original_title;
#ifndef EMSCRIPTEN
    // g_low->sdl->SDL_SetWindowTitle(sdl_window, l.c_str());
    //std::cout << std::endl;
#endif
    g_has_title = false;
    //}
  }
}
std::vector<std::string> g_prog_labels;
GameApi::W g_progress_dialog;
GameApi::EveryApi *g_everyapi2=0;
GameApi::GuiApi *g_everyapi_gui=0;
GameApi::FtA g_atlas;
GameApi::BM g_atlas_bm;
bool g_progress_callback_set=false;
void (*g_progress_callback)();

void (*update_progress_dialog_cb)(GameApi::W &w, int,int, GameApi::FtA, GameApi::BM, std::vector<std::string>);


void ProgressBar(int num, int val, int max, std::string label)
{
  //std::cout << "Progress2: " << num << " " << val << " " << label << " " << max << std::endl;
  //std::cout << "ProgressBar: '" << label << "'" << std::endl;

  //std::cout << "PB: " << num << std::endl;
  int val_index = -1;
  int max_index = -1;
  //std::cout << "P1" << std::endl;
  int val_value = 0;
  int max_value = 0;
  bool done = false;
  {
    int s = progress_val.size();
    for(int i=0;i<s;i++)
      {
	int num2 = progress_val[i].num;
	val_value+=num2;
	if (num2==num) {
	  progress_val[i].value = val;
	  val_index=i;
	  done = true;
	}
      }
  }
  if (!done) {
      ProgressI p; p.num = num; p.value=0; p.ticks=0;
      p.value = val;
      progress_max.push_back(p);
      p.value = max;
      progress_val.push_back(p);
      progress_label.push_back(label);
  }
  //std::cout << "P2" << std::endl;

  {
  int s = progress_max.size();
  for(int i=0;i<s;i++)
    {
      int num2 = progress_max[i].num;
      max_value += num2;
      if (num2==num) {
	progress_max[i].value = max;
	max_index=i;
      }
    }
  }
  //std::cout << "P3" << std::endl;

  //if (val_index==-1) std::cout << "ProgressError(val): num=" << num << std::endl;
  //if (max_index==-1) std::cout << "ProgressError(max): num=" << num << std::endl;

  int val1 = val_index!=-1?progress_val[val_index].value:0; //FindProgressVal();
  int max1 = max_index!=-1?progress_max[max_index].value:1; //FindProgressMax();
  //int ticks1 = progress_val[val_index].ticks;
  //int ticks2 = g_low->sdl->SDL_GetTicks();
  //g_last_tick = ticks2;
  //int ticks = ticks2-ticks1;
  float v = float(val1)/float(max1);
  v*=15.0;
  int val2 = int(v)+1;
  if (val2<0) val2=0;
  if (val2>15) val2=15;
  float vv = 1.0;
  vv*=15.0;
  int max2 = int(vv);
  if (max2<1) max2=1;
  if (max2>15) max2=15;
  static std::string old_label = "";
  if (label!="installprogress" /*&& (ticks>40||val==max)*/) {

    std::stringstream stream;
    std::stringstream stream2;
    std::stringstream stream3;
    if (old_label != label) {
      g_prog_labels.push_back("");
      old_label = label; stream << std::endl << "["; }
  else
    stream << "\r[";
  for(int i=0;i<val2;i++) {
    stream2 << "#";
  }
  for(int i=val2;i<max2;i++) {
    stream2 << "_";
  }
  int s = label.size();
  int pos = -1;
  for(int i=0;i<s;i++) {
    if (label[i]=='/') pos=i+1;
  }
  if (pos!=-1) label = label.substr(pos);

  stream3 << "] "
    //<< val1 << "/" << max1 << ") (" << val << "/" << max << ") " << num << " " 
	    << " " << label ;
  std::string l = stream2.str();

    std::stringstream sk;
    sk << val2 << "/" << max2;
    std::string kk = sk.str();
    int center = l.size()/2;
    int center_kk = kk.size()/2;
    int pos2 = center-center_kk;
    for(int i=0;i<kk.size();i++) { l[pos2+i] = kk[i]; }


    //std::string start = l.substr(0,std::max(0,std::min(val2,int(l.size()))));
    //std::string end = l.substr(std::max(1,std::min(val2+1,int(l.size()))));
    
    

    
#ifndef EMSCRIPTEN
    //std::cout << stream.str() << l << stream3.str() << std::flush;
    if (g_prog_labels.size()>0)
      {
	g_prog_labels[g_prog_labels.size()-1] = stream.str() + l + stream3.str();
	if (g_everyapi2)
	  {
	    
	    update_progress_dialog_cb(g_progress_dialog, 400,200, g_atlas, g_atlas_bm, g_prog_labels);
	    if (g_progress_callback_set) {
	      g_progress_callback();
	    }
	  }
	
      }
#endif
  g_has_title = true;
  }


  //std::cout << "P4" << std::endl;
}

int async_pending_count = 0;

std::string striphomepage(std::string url)
{ 
  int s = url.size();
  int i = 0;
  for(;i<s-1;i++) if (url[i]=='&'&&url[i+1]=='h') break;
  std::string res = url.substr(0,i);
  //std::cout << "Url without homepage: " << res << std::endl;
  return res;
}

std::vector<const unsigned char*> g_content;
std::vector<const unsigned char*> g_content_end;
std::vector<const char*> g_urls;

std::string remove_load(std::string s)
{
  int ss = std::string("load_url.php?url=").size();
  if (s.substr(0,ss)=="load_url.php?url=") return stripprefix(s);
  return s;
}

bool file_exists(std::string filename)
{
  std::ifstream f(filename.c_str());
  return f.good();
}

long long load_size_from_url(std::string url)
{
  if (url=="") return 1;
    std::vector<unsigned char> buffer;
    bool succ=false;
#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    std::string cmd = "curl -s -N --url " + url;
    std::string cmdsize = "curl -sI --url " + url;
    succ = true;
#endif
    long long num = 1;
    if (succ) {

#ifdef __APPLE__
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else
#ifdef LINUX
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else    
    FILE *f2 = popen(cmdsize.c_str(), "rb");
#endif
#endif
    std::vector<unsigned char> vec2;
    unsigned char c2;
    while(fread(&c2,1,1,f2)==1) {
      vec2.push_back(c2);
    }
    pclose(f2);
    std::string s(vec2.begin(),vec2.end());
    //std::cout << "Headers:" << s << std::endl;
    std::stringstream ss(s);
    std::string line;
    while(std::getline(ss,line)) {
      //std::cout << "Line: " << line << std::endl;
      //if (line.size()>15)
	//std::cout << "Substr: " << line.substr(0,15) << std::endl;
      if (line.size()>15 && line.substr(0,15)=="Content-Length:") {
	std::stringstream ss2(line);
	std::string dummy;
	ss2 >> dummy >> num;
	//std::cout << "Got num: " << num << std::endl;
      }
    }
    }
    return num;
}

class LoadUrlStream : public LoadStream
{
public:
  LoadUrlStream(std::string url) : url(url),f(0) { }
  ~LoadUrlStream() {
#ifdef HAS_POPEN
    if (f) { pclose(f); }
#endif
  }
  virtual LoadStream *Clone()
  {
    return new LoadUrlStream(url);
  }

  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }

  virtual void Prepare()
  {
    size = load_size_from_url(url);

    InstallProgress(333, "stream load..", 15);
    
    bool succ = false;
#ifdef HAS_POPEN

#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    std::string cmd = "curl -s -N --url " + url;
    std::string cmdsize = "curl -sI --url " + url;
    succ = true;
#endif

#ifdef __APPLE__
    f = popen(cmd.c_str(), "r");
#else
#ifdef LINUX
    f = popen(cmd.c_str(), "r");
#else    
    f = popen(cmd.c_str(), "rb");
#endif
#endif

    
#endif

    if (!f) { std::cout << "popen failed" << std::endl;
      //std::cout << errno << std::endl;
      //std::cout << url << std::endl;
    }
  }
  virtual bool get_ch(unsigned char &ch)
  {
#ifdef HAS_POPEN
    if (f) {
      return fread(&ch,1,1,f)==1;
    } else return false;
#else
    return false;
#endif
  }
#ifdef WINDOWS
  ssize_t getline(char** line, size_t*sz, FILE *f)
  {
    //std::cout << "getline" << std::endl;
    std::vector<unsigned char> vec;
    unsigned char ch='a';
    long long i = 0;
    bool succ = false;
    while((succ = get_ch(ch))&&ch!='\n') {
	  vec.push_back(ch);
	  i++;
	  currentpos++;
	  if (size/15>0 && currentpos % (size/15)==0) {
	    ProgressBar(333,currentpos*15/size,15,"stream load..");
			
	  }
    }
    
    //std::cout << "getline end " << i << std::endl;
    *line =(char*) malloc(vec.size()+1);
    std::copy(vec.begin(),vec.end(),*line);
    (*line)[i]=0;
    if (!succ) {
      *sz=-1;
    } else {
      *sz = i;
    }
    return *sz;
  }
#endif
  virtual bool get_line(std::vector<unsigned char> &line)
  {
#ifdef HAS_POPEN
    if (f) {
      char *buf_line = NULL;
      size_t size2 = 0;
      ssize_t sz = getline(&buf_line,&size2, f);
      if (!buf_line) return false;
      if (sz==-1) return false;
      line.clear();
      line=std::vector<unsigned char>(buf_line,buf_line+sz);
      for(int i=0;i<sz;i++) {
	currentpos++;
      	  if (size/15>0 && currentpos % (size/15)==0) {
	    ProgressBar(333,currentpos*15/size,15,"stream load..");
	  }
      }
      
      free(buf_line);
      return sz>0;
    } else return false;
#else
    return false;
#endif
  }
  virtual bool get_file(std::vector<unsigned char> &file)
  {
#ifdef HAS_POPEN
    if (f) {
      file.clear();
      unsigned char c2;
      while(fread(&c2,1,1,f)==1) {
	file.push_back(c2);
      }
      return true;
    } else return false;
#else
    return false;
#endif
  }
private:
  std::string url;
  FILE *f;
  long long size=0;
  long long currentpos=0;
};

class LoadStream2 : public LoadStream
{
public:
  LoadStream2(std::vector<unsigned char> vec) : vec(vec) { }
  virtual LoadStream *Clone()
  {
    return new LoadStream2(vec);
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { pos=vec.begin(); }

  virtual void Prepare() {
    pos = vec.begin();
  }
  virtual bool get_ch(unsigned char &ch)
  {
    if (pos!=vec.end()) {
      ch = *pos;
      pos++;
      return true;
    } else { return false; }
  }
  virtual bool get_line(std::vector<unsigned char> &line)
  {
    if (pos==vec.end()) return false;
    line.clear();
    char ch = 0;
    while(ch!='\n' && pos!=vec.end()) { ch=*pos; pos++; line.push_back(ch); }
    //std::cout << "LINE: " << std::string(line.begin(),line.end()) << std::endl;
    return true;
  }
  virtual bool get_file(std::vector<unsigned char> &file)
  {
    file = vec;
    return true;
  }
private:
  std::vector<unsigned char>::iterator pos;
  std::vector<unsigned char> vec;
};

LoadStream *load_from_url_stream(std::string url)
{
  LoadStream *stream = new LoadUrlStream(url);
    stream->Prepare();  
  return stream;
}
LoadStream *load_from_vector(std::vector<unsigned char> vec)
{
  LoadStream *stream = new LoadStream2(vec);
  stream->Prepare();
  return stream;
}

extern std::string g_msg_string;

struct load_url_deleter
{
  std::vector<std::vector<unsigned char> *> item;
  ~load_url_deleter() {
    int s = item.size();
    for(int i=0;i<s;i++) delete item[i];
  }
};
load_url_deleter load_from_url_del;

std::vector<unsigned char> *load_from_url(std::string url)
{ // works only in windows currently. Dunno about linux, and definitely doesnt wok in emscripten
  if (url.size()==0) { std::vector<unsigned char> *b = new std::vector<unsigned char>(); load_from_url_del.item.push_back(b); return b; }
  //std::cout << "load_from_url: @" << url << "@" << std::endl;

  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      if (remove_load(url)==g_urls[i]) { 
	std::vector<unsigned char> *vec = new std::vector<unsigned char>(g_content[i], g_content_end[i]);
	//load_from_url_del.item.push_back(vec);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	g_current_size+=vec->size();

	return vec;
      }
    }
  //  std::cout << "load_from_url using network: " << url << std::endl;
  std::vector<unsigned char> *buffer = new std::vector<unsigned char>;
  load_from_url_del.item.push_back(buffer);
    bool succ=false;
#ifdef HAS_POPEN

#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    std::string cmd = "curl -s -N --url " + url;
    std::string cmdsize = "curl -sI --url " + url;
    succ = true;
#endif
    int num = 100000;
    if (succ) {

#ifdef __APPLE__
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else
#ifdef LINUX
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else    
    FILE *f2 = popen(cmdsize.c_str(), "rb");
#endif
#endif
    std::vector<unsigned char> vec2;
    unsigned char c2;
    while(fread(&c2,1,1,f2)==1) {
      
      vec2.push_back(c2);
    }
    pclose(f2);
    std::string s(vec2.begin(),vec2.end());
    //std::cout << "Headers:" << s << std::endl;
    std::stringstream ss(s);
    std::string line;
    while(std::getline(ss,line)) {
      //std::cout << "Line: " << line << std::endl;
      //if (line.size()>15)
	//std::cout << "Substr: " << line.substr(0,15) << std::endl;
      if (line.size()>15 && line.substr(0,15)=="Content-Length:") {
	std::stringstream ss2(line);
	std::string dummy;
	ss2 >> dummy >> num;
	//std::cout << "Got num: " << num << std::endl;
      }
    }
    //std::cout << "Content-Length: " << num << std::endl;


#ifdef __APPLE__
    FILE *f = popen(cmd.c_str(), "r");
#else
#ifdef LINUX
    FILE *f = popen(cmd.c_str(), "r");
#else    
    FILE *f = popen(cmd.c_str(), "rb");
#endif
#endif
    if (!f) {
      g_msg_string = "popen failed!";
      std::cout << "popen failed#2" << std::endl;
      std::cout << errno << std::endl;
      std::cout << url << std::endl;
      std::vector<unsigned char> *item = new std::vector<unsigned char>();
      load_from_url_del.item.push_back(item);
      return item;
    }


    //std::cout<< "FILE: " << std::hex<<(long)f <<std::endl; 
    unsigned char c;
    long long i = 0;
    if (num>0)
      buffer->reserve(num);
    while(fread(&c,1,1,f)==1) {
      i++;
      g_current_size++;
      if (!g_progress_already_done && num/15>0 && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int j=0;j<s;j++) sum+=int(url[j]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      buffer->push_back(c); }
    pclose(f);

    }
    //fseek(f, 0, SEEK_END);
    //long pos = ftell(f);
    //fseek(f, 0, SEEK_SET);
    //buffer.resize(pos);
    //fread(&buffer[0], 1, pos, f);
    //std::cout << "::" << std::string(buffer.begin(),buffer.end()) << "::" << std::endl;
    if (buffer->size()==0)
      {
#ifdef WINDOWS
    std::string cmd = ".\\curl\\curl.exe -s -N --url " + url;
#else
    std::string cmd = "curl -s -N --url " + url;
#endif
#ifdef LINUX
    FILE *f = popen(cmd.c_str(), "r");
#else
    FILE *f = popen(cmd.c_str(), "rb");
#endif

    if (!f) { g_msg_string = "popen failed!"; std::cout << "popen failed!" << std::endl; }
    
    unsigned char c;
    //std::vector<unsigned char> buffer;
    long long i = 0;
    while(fread(&c,1,1,f)==1) {
      i++;
      g_current_size++;
      if (!g_progress_already_done && num/15>0 && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int j=0;j<s;j++) sum+=int(url[j]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      buffer->push_back(c); }
    pclose(f);
      }
    return buffer;
#else
    // no popen
#endif
    return buffer;
}


#if 0
struct Req
{
  std::string url;
  std::string params;
  void (*fptr)(std::string);
};

void web_request_onload(unsigned handle, void *arg, void *buffer, unsigned size)
{
  unsigned char *buf = (unsigned char*) buffer;
  std::string buf2(buf,buf+size);
  Req *r = (Req*)arg;
  r->fptr(buf2);
}
void web_request_onerror(unsigned handle, void *arg, int http_error, const char *status)
{
  std::cout << "[" << http_error << "]:" << status << std::endl;
}
void web_request_onprogress(unsigned handle, void *arg, int numbytes, int size)
{
}
void web_request(std::string url, std::string params, void (*fptr)(std::string))
{
  Req *r = new Req;
  r->url = url;
  r->params = params;
  r->fptr = fptr;
#ifdef EMSCRIPTEN
  emscripten_async_wget2_data(url.c_str(), "POST", params.c_str(), (void*)r, false, &web_request_onload, &web_request_onerror, &web_request_onprogress);
#else
  
#endif
}
#endif
