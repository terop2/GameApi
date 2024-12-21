#include "GameApi_h.hh"

class FetchInBlocks;

  struct VECENTRY {
    std::string first;
    const std::vector<unsigned char, GameApiAllocator<unsigned char> > * second;
  };
#ifdef EMSCRIPTEN
  struct VECENTRY2
  {
    std::string first;
    FetchInBlocks* second;
  };
#endif

struct del_map
{
  void async_cache_clear()
  {
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	delete e.second;
      }
    load_url_buffers_async.clear();
  }

#ifdef EMSCRIPTEN
  void del_fetch_url(std::string url)
  {
    int s = fetches.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY2 e = fetches[i];
	if (e.first == url) { delete e.second; fetches.erase(fetches.begin()+i); }
      }
  }
#endif
  void del_async_url(std::string url)
  {
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	if (e.first == url) {
	  delete e.second;
	  load_url_buffers_async.erase(load_url_buffers_async.begin()+i);
	}
      }
  }
#ifdef EMSCRIPTEN
  void push_fetch_url(std::string url, FetchInBlocks *blk)
  {
    VECENTRY2 e;
    e.first = url;
    e.second = blk;
    fetches.push_back(e);
  }
#endif
  void push_async_url(std::string url, const std::vector<unsigned char, GameApiAllocator<unsigned char> > *ptr)
  {
    VECENTRY e;
    e.first = url;
    e.second = ptr;
    load_url_buffers_async.push_back(e);
  }
  ~del_map() {

  }
  void print()
  {

  }
  void del_vec(const std::vector<unsigned char, GameApiAllocator<unsigned char> >* vec)
  {

    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	if (&*e.second == &*vec) { delete e.second; load_url_buffers_async.erase(load_url_buffers_async.begin()+i); }
      }


    /*
    std::map<std::string,std::shared_ptr<const std::vector<unsigned char, GameApiAllocator<unsigned char> >> >::iterator it=load_url_buffers_async.begin();
    for(;it!=load_url_buffers_async.end();it++)
      {
	const std::vector<unsigned char, GameApiAllocator<unsigned char> > * ptr = (*it).second.get();
	if (ptr==vec) {
	  load_url_buffers_async.erase(it);
	  delete ptr;
	  return;
	}
      }
    */
  }
#ifdef EMSCRIPTEN
  bool fetch_find(std::string url)
  {
    int s = fetches.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY2 e = fetches[i];
	if (e.first == url)
	  { return true; }
      }
    return false;
  }
#endif
  bool async_find(std::string url)
{
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	if (e.first == url)
	  { return true; }
      }
    return false;
}

VECENTRY &async_get(std::string url)
{
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	if (e.first == url)
	  { ret=e; return ret; }
      }
std::cout << "You should check if element exists before calling async_get" << std::endl;
 VECENTRY e2;
 e2.first = "";
 e2.second = 0;
 ret = e2;
    return ret;  
}
  
  std::vector<VECENTRY> load_url_buffers_async;
  //std::map<std::string, std::shared_ptr<const std::vector<unsigned char, GameApiAllocator<unsigned char> > > > load_url_buffers_async;
#ifdef EMSCRIPTEN
    std::vector<VECENTRY2> fetches;
  //std::map<std::string, std::shared_ptr<FetchInBlocks> > fetches;
#endif
  VECENTRY ret;
  VECENTRY ret2;
};
extern del_map g_del_map;
