
#include "GameApi_h.hh"
#include <stdio.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

int load_size_from_url(std::string url);


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
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->set_callback(url, fptr, data);
}
EXPORT void GameApi::Env::async_rem_callback(std::string url)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->rem_callback(url);
}
EXPORT std::vector<unsigned char> *GameApi::Env::get_loaded_async_url(std::string url)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;

  return env->async_loader->get_loaded_data(url);
}

EXPORT GameApi::Env::~Env()
{
  delete (::EnvImpl*)envimpl;
}
std::string remove_load(std::string s);
std::vector<unsigned char> load_from_url(std::string url);
std::string striphomepage(std::string url);
std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;
struct ASyncCallback { void (*fptr)(void*); void *data; };
struct ASyncCallback2 { std::string url; ASyncCallback *cb; };
//std::map<std::string, ASyncCallback*> load_url_callbacks;
std::vector<ASyncCallback2> load_url_callbacks;
void add_async_cb(std::string url, ASyncCallback *cb)
{
  ASyncCallback2 cb2;
  cb2.url = url;
  cb2.cb = cb;
  load_url_callbacks.push_back(cb2);
}
ASyncCallback *rem_async_cb(std::string url)
{
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
void onerror_async_cb(unsigned int tmp, void *arg, int, const char*)
{
  std::cout << "ERROR: url loading error! " << std::endl;
  if (!arg) return;
  char *url = (char*)arg;
    std::string url_str(url);
  std::string url_only(striphomepage(url_str));
    load_url_buffers_async[url_only] = (std::vector<unsigned char>*)-1;
    async_pending_count--;
    std::cout << "ASync pending dec (onerror_async_cb) -->" << async_pending_count << std::endl;
    
    ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  if (cb) {
    std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
  }

}
std::string striphomepage(std::string);
std::string stripprefix(std::string s)
{
  int len = strlen("load_url.php?url=");
  return s.substr(len,s.size()-len);
}
void onload_async_cb(unsigned int tmp, void *arg, void *data, unsigned int datasize)
{
  char *url = (char*)arg;
  unsigned char *dataptr = (unsigned char*)data;
  if (datasize==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
  }
  std::vector<unsigned char> buffer;
  //unsigned char *dataptr = (unsigned char*)data;
  for(unsigned int i=0;i<datasize;i++) { buffer.push_back(dataptr[i]); }
  
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
  load_url_buffers_async[url_only] = new std::vector<unsigned char>(buffer);
  async_pending_count--;
  //std::cout << "ASync pending dec (onload_async_cb) -->" << async_pending_count<< std::endl;
  
  //std::cout << "Async cb!" << url_only << std::endl;
  ASyncCallback *cb = rem_async_cb(url_only); //load_url_callbacks[url_only];
  if (cb) {
    //std::cout << "Load cb!" << url_only << std::endl;
    (*cb->fptr)(cb->data);
  }

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url_only2);
  }

}

std::vector<unsigned char> load_from_url(std::string url);

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
  cb->fptr = fptr;
  cb->data = data;
  //load_url_callbacks[url] = cb;
  add_async_cb(url,cb);



  //std::cout << "async set callback" << url << std::endl;
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
  std::vector<unsigned char> buf = load_from_url(url);
  std::string url2 = "load_url.php?url=" + url ;
  load_url_buffers_async[url2] = new std::vector<unsigned char>(buf);  
  pthread_exit(0);
}

bool g_progress_already_done = false;
int g_current_size=0;
void ASyncLoader::load_all_urls(std::vector<std::string> urls, std::string homepage)
{
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


  std::vector<std::string> urls2;
  for (int e=0;e<s;e++)
    {
      std::string url = urls[e];
      std::string url2 = "load_url.php?url=" + url ;
      if (load_url_buffers_async[url2])
	{
      ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
      if (cb) {
	//std::cout << "Load cb!" << url2 << std::endl;
	//std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
	(*cb->fptr)(cb->data);
      } else {
	//std::cout << "ASyncLoadUrl::CB failed" << std::endl;
      }

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
  
  int total_size = 0;
  std::vector<int> sizes;
  for(int d=0;d<s;d++)
    {
      std::string url = urls[d];
      int sz = load_size_from_url(url);
      sizes.push_back(sz);
      total_size+=sz;
    }
  g_current_size = 0;
  
  std::vector<ProcessData*> vec;

    
    {
	InstallProgress(444,"loading assets",15);
    }

  for(int i=0;i<s;i++) {
    std::string url = urls[i];

    ProcessData *processdata = new ProcessData;
    processdata->url = url;
    vec.push_back(processdata);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 300000);
    pthread_create(&processdata->thread_id, &attr, &process, (void*)processdata);
  }
  int current_size = 0;
  int last_size = 0;
  for(int j=0;j<s;j++)
    {
      void *res;
      std::string url = urls[j];
      std::string url2 = "load_url.php?url=" + url ;
      while (!load_url_buffers_async[url2])
	{
	  if (g_current_size > last_size+(total_size/15))
	  {
	    int s = url.size();
	    int sum=0;
	    for(int i=0;i<s;i++) sum+=int(url[i]);
	    sum = sum % 1000;
	    ProgressBar(444,g_current_size*15/total_size,15,"loading assets");
	  last_size=g_current_size;
	  }

	}
      // pthread_join(vec[j]->thread_id,&res);
      ASyncCallback *cb = rem_async_cb(url2); //load_url_callbacks[url2];
      if (cb) {
	//std::cout << "Load cb!" << url2 << std::endl;
	//std::cout << "ASyncLoader::cb:" << url2 << std::endl; 
	(*cb->fptr)(cb->data);
      } else {
	//std::cout << "ASyncLoadUrl::CB failed" << std::endl;
      }
      current_size+=sizes[j];

      //{
      //int s = url.size();
      //int sum=0;
      //for(int i=0;i<s;i++) sum+=int(url[i]);
      //sum = sum % 1000;
      //ProgressBar(444,current_size*15/total_size,15,"loading assets");
      //}
      
    }

#endif
  g_progress_already_done = false;

}
void ASyncLoader::load_urls(std::string url, std::string homepage)
  {
    //std::cout << "ASyncLoader::load_urls:" << url << std::endl; 

  // progress bar
    {
    std::string url2 = "load_url.php?url=" + url ;
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  if (load_url_buffers_async[url2]) { 
    InstallProgress(sum,url + " (cached)",15);
  } else {
  InstallProgress(sum,url,15);

  }
    }

#ifdef EMSCRIPTEN
  std::string url2 = "load_url.php";
  std::string urlend = "url=" + url;
  std::string url3 = urlend + "&homepage=" + homepage;

  url = "load_url.php?url=" + url + "&homepage=" + homepage;

    //std::cout << "url loading started! " << url << std::endl;

    // if we have already loaded the same url, don't load again
    if (load_url_buffers_async[url]) { 
      ASyncCallback *cb = rem_async_cb(url); //load_url_callbacks[url];
      if (cb) {
	//std::cout << "Load cb!" << url << std::endl;
	(*cb->fptr)(cb->data);
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
    char *buf2 = new char[url2.size()+1];
    std::copy(url2.begin(), url2.end(), buf2);
    buf2[url2.size()]=0;

    char *buf3 = new char[url.size()+1];
    std::copy(url.begin(), url.end(), buf3);
    buf3[url.size()]=0;
    
    async_pending_count++;
    //    std::cout << "ASync pending inc (load_urls) -->" << async_pending_count << std::endl;

    //emscripten_async_wget_data(buf2, (void*)buf2 , &onload_async_cb, &onerror_async_cb);
    emscripten_async_wget2_data(buf2, "POST", url3.c_str(), (void*)buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
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
  if (load_url_buffers_async[url2]) { 
    ProgressBar(sum,0,15,url+" (cached)");
  } else {
    ProgressBar(sum,0,15,url);
  }
  }

    std::string url2 = "load_url.php?url=" + url ;
    if (load_url_buffers_async[url2]) { 
      { // progressbar
	int s = url.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url + " (cached)");
      }

      return; }
    //std::cout << "Loading url: " << url <<std::endl;
    std::vector<unsigned char> buf = load_from_url(url);
    //std::cout << "Loading url finished: " << url <<std::endl;
    if (buf.size()==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
    }
    load_url_buffers_async[url2] = new std::vector<unsigned char>(buf);
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

std::vector<unsigned char> *ASyncLoader::get_loaded_data(std::string url) const
  {
    url = "load_url.php?url=" + url;
    //std::cout << "url fetch " << url << std::endl;
    return load_url_buffers_async[url];
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
void InstallProgress(int num, std::string label, int max=15)
{
  //std::cout << "InstallProgress: " << num << " " << label << " " << max << std::endl;
  //std::cout << "InstallProgress: '" << label << "'" << std::endl;
  //std::cout << "IB: " << num << std::endl;
  ProgressI p; p.num = num;
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

int FindProgressVal()
{
  int s = progress_val.size();
  int sum = 0;
  for(int i=0;i<s;i++)
    sum+=progress_val[i].value;
  return sum;
}
int FindProgressMax()
{
  int s = progress_max.size();
  int sum = 0;
  for(int i=0;i<s;i++)
    {
      sum+=progress_max[i].value;
    }
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
void ProgressBar(int num, int val, int max, std::string label)
{
  //std::cout << "Progress: " << num << " " << val << " " << label << " " << max << std::endl;
  //std::cout << "ProgressBar: '" << label << "'" << std::endl;

  //std::cout << "PB: " << num << std::endl;
  int val_index = -1;
  int max_index = -1;
  //std::cout << "P1" << std::endl;
  {
  int s = progress_val.size();
  for(int i=0;i<s;i++)
    {
      int num2 = progress_val[i].num;
      if (num2==num) {
	progress_val[i].value = val;
	val_index=i;
      }
    }
  }
  //std::cout << "P2" << std::endl;

  {
  int s = progress_max.size();
  for(int i=0;i<s;i++)
    {
      int num2 = progress_max[i].num;
      if (num2==num) {
	progress_max[i].value = max;
	max_index=i;
      }
    }
  }
  //std::cout << "P3" << std::endl;

  if (val_index==-1) std::cout << "ProgressError(val): num=" << num << std::endl;
  if (max_index==-1) std::cout << "ProgressError(max): num=" << num << std::endl;

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
    if (old_label != label) { old_label = label; stream << std::endl << "["; }
  else
    stream << "\r[";
  for(int i=0;i<val2;i++) {
    stream << "#";
  }
  for(int i=val2;i<max2;i++) {
    stream << "-";
  }
  int s = label.size();
  int pos = -1;
  for(int i=0;i<s;i++) {
    if (label[i]=='/') pos=i+1;
  }
  if (pos!=-1) label = label.substr(pos);

  stream << "] "
    //<< val1 << "/" << max1 << ") (" << val << "/" << max << ") " << num << " " 
	    << " " << label ;
  std::string l = stream.str();
#ifndef EMSCRIPTEN
	{
	  std::cout << l.c_str() << std::flush;
	  //g_low->sdl->SDL_SetWindowTitle(sdl_window, l.c_str());
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

int load_size_from_url(std::string url)
{
  if (url=="") return 100000;
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

std::vector<unsigned char> load_from_url(std::string url)
{ // works only in windows currently. Dunno about linux, and definitely doesnt wok in emscripten
  if (url.size()==0) return std::vector<unsigned char>();
  //std::cout << "load_from_url: @" << url << "@" << std::endl;

  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      if (remove_load(url)==g_urls[i]) { 
	std::vector<unsigned char> vec(g_content[i], g_content_end[i]);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	return vec;
      }
    }
  //  std::cout << "load_from_url using network: " << url << std::endl;
    std::vector<unsigned char> buffer;
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
    //    std::cout << "Content-Length: " << num << std::endl;


#ifdef __APPLE__
    FILE *f = popen(cmd.c_str(), "r");
#else
#ifdef LINUX
    FILE *f = popen(cmd.c_str(), "r");
#else    
    FILE *f = popen(cmd.c_str(), "rb");
#endif
#endif
    //std::cout<< "FILE: " << std::hex<<(long)f <<std::endl; 
    unsigned char c;
    int i = 0;
    while(fread(&c,1,1,f)==1) {
      i++;
      g_current_size++;
      if (!g_progress_already_done && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url[i]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      buffer.push_back(c); }
    pclose(f);

    }
    //fseek(f, 0, SEEK_END);
    //long pos = ftell(f);
    //fseek(f, 0, SEEK_SET);
    //buffer.resize(pos);
    //fread(&buffer[0], 1, pos, f);
    //std::cout << "::" << std::string(buffer.begin(),buffer.end()) << "::" << std::endl;
    if (buffer.size()==0)
      {
#ifdef WINDOWS
    std::string cmd = ".\\curl\\curl.exe -s -N --url " + url;
#else
    std::string cmd = "curl -s -N --url " + url;
#endif
    FILE *f = popen(cmd.c_str(), "rb");
    unsigned char c;
    //std::vector<unsigned char> buffer;
    int i = 0;
    while(fread(&c,1,1,f)==1) {
      i++;
      g_current_size++;
      if (!g_progress_already_done && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url[i]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      buffer.push_back(c); }
    pclose(f);
      }
    return buffer;
#else
    // no popen
#endif
    return buffer;
}

