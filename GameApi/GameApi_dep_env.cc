
#include "GameApi_h.hh"
#include <stdio.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/fetch.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif

#include <atomic>

#include "Tasks.hh"

#define idb_disabled 1
//#define idb_disabled 0

bool g_disable_polygons=false;
bool g_filter_execute = false;

IMPORT double g_dpr=1.0;


IMPORT extern std::string g_window_href;
IMPORT extern std::string gameapi_homepageurl;
extern int g_pthread_count;

int g_async_pending_count_failures=0;

IMPORT std::string striphomepage(std::string url);
IMPORT std::string stripprefix(std::string s);
IMPORT void InstallProgress(int num, std::string label, int max);
IMPORT void ProgressBar(int num, int val, int max, std::string label);
extern int g_logo_status;


extern task_interface &g_tasks;

void *task_queue_consumer(void *data)
{
  static int task_num = 0;
  task_num++;
  int task_num2 = task_num;
  while(1)
    {
      //std::cout << "Tasks loop" << task_num2 << std::endl;
      if (g_tasks.shutdown_ongoing()) break;
      g_tasks.queue_mutex_start();
      bool b = g_tasks.queue_has_data();
      task_data dt;
      if (b)
	{
	  //std::cout << "Tasks has data" << task_num << std::endl;
	  dt = g_tasks.front();
	//std::cout << "Tasks running heavy" << task_num2 << " " << dt.id << " " << dt.num << std::endl;
	g_tasks.set_task_as_execute(dt);
	g_tasks.pop_from_queue();
	g_tasks.queue_mutex_end();
	dt.fptr(dt.data);
	//std::cout << "Tasks finished heavy" << task_num2 << " " << dt.id << " " << dt.num<< std::endl;
	//g_tasks.queue_mutex_start();
	g_tasks.set_task_as_done(dt);
	//g_tasks.queue_mutex_end();
      } else
	{
	g_tasks.queue_mutex_end();
	  // std::cout << "Tasks waiting" << task_num2 << std::endl;
	  g_tasks.wait_for_push_or_shutdown();
	  //std::cout << "Task_wakeup" << task_num2 << std::endl;
	}
    }  
  return 0;
}

IMPORT void tasks_init()
{
  //std::cout << "Tasks init" << std::endl;
  int s = 8;
  g_tasks.queue_mutex_init();
  g_tasks.mutex_init();
  for(int i=0;i<s;i++)
    {
      g_tasks.spawn_thread();
    }
}
IMPORT void tasks_add(int id, void *(*fptr)(void*), void *data)
{
  //std::cout << "Tasks add "<< id << "::" << (long)fptr << " " << (long)data << std::endl;
  task_data dt = g_tasks.create_work(id,fptr,data);
  g_tasks.push_to_queue(dt);
}
IMPORT void tasks_join(int id)
{
  //std::cout << "Tasks join " << id << std::endl;
  g_tasks.join_id(id);
}

extern GameApi::EveryApi *g_everyapi;
extern int g_disable_draws;

bool disable_opengl_from_get_event=false;


void timeout_getevent()
{
  GameApi::EveryApi *ev = g_everyapi; 
  if (!ev) return;
  GameApi::MainLoopApi::Event e;
  disable_opengl_from_get_event=true;
     while((e = ev->mainloop_api.get_event()).last==true)
       {
	 /* this eats all events from queue */
       }
  disable_opengl_from_get_event=false;

}

int wait_with_timeout(pthread_cond_t *cond, pthread_mutex_t *mutex, int timeout_ms)
{
  struct timespec ts;
  int ret;
  
  // Get current time
  clock_gettime(CLOCK_REALTIME, &ts);
  
  // Add timeout_ms milliseconds to current time for absolute timeout
  ts.tv_sec += timeout_ms / 1000;
  ts.tv_nsec += (timeout_ms % 1000) * 1000000;
  
  // Normalize nanoseconds if > 1 second
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_sec += 1;
    ts.tv_nsec -= 1000000000;
  }
  
  // Wait on the condition variable with timeout
  ret = pthread_cond_timedwait(cond, mutex, &ts);
  
  if (ret == 0) {
    // Condition was signaled before timeout
    return 0;
  } else if (ret == ETIMEDOUT) {
    // Timeout expired
    return 1;
  } else {
    // Some other error occurred
    return -1;
  }
}

class task_implementation : public task_interface
{
public:
  virtual void spawn_thread()
  {
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 4096000);
    //std::cout << "phread_create" << std::endl;
    pthread_t *thread_id = new pthread_t;
    threads.push_back(thread_id);
    pthread_create(thread_id, &attr, &task_queue_consumer, (void*)0);
    //std::cout << "pthread_create_return: " << val << std::endl;
    pthread_attr_destroy(&attr);
  }
  ~task_implementation()
  {
#if 0
    start_shutdown();
    void *res;
    int s = threads.size();
    for(int i=0;i<s;i++)
      pthread_join(*threads[i], &res);
    int s2 = threads.size();
    for(int i=0;i<s2;i++)
      delete threads[i];
    
    
    mutex_destroy();
#endif
  }
  virtual task_data create_work(int id,
				void *(*fptr)(void*),
				void *data)
  {
    static int counter = 0;
    counter++;
    task_data dt;
    dt.id = id;
    dt.num = counter;
    dt.fptr = fptr;
    dt.data = data;
    return dt;
  }
  virtual void queue_mutex_init()
  {
    queue_mutex = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  }
  virtual void queue_mutex_start()
  {
    pthread_mutex_lock(queue_mutex);
  }
  virtual void queue_mutex_end()
  {
    pthread_mutex_unlock(queue_mutex);
  }
  virtual void mutex_init()
  {
    mutex = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
    cond = new pthread_cond_t(PTHREAD_COND_INITIALIZER);

    mutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
    cond2 = new pthread_cond_t(PTHREAD_COND_INITIALIZER);

  }
  virtual void queue_mutex_destroy()
  {
    pthread_mutex_destroy(queue_mutex);
    delete queue_mutex;
    queue_mutex = 0;
  }
  virtual void mutex_destroy()
  {
    pthread_cond_destroy(cond);
    pthread_cond_destroy(cond2);
    pthread_mutex_destroy(mutex);
    pthread_mutex_destroy(mutex2);
    delete mutex;
    delete mutex2;
    delete cond;
    delete cond2;
    mutex=0;
    mutex2=0;
    cond=0;
    cond2=0;
  }
  // this is run in main thread
  virtual void push_to_queue(task_data d)
  {
    //std::cout << "pushing to queue" << d.num << std::endl;
    queue_mutex_start();
    queue.push_back(d);
    pthread_mutex_lock(mutex);
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);
    queue_mutex_end();
  }
  virtual bool queue_has_data()
  {
    bool b = queue.size()!=0;
    return b;
  }
  virtual task_data front()
  {
    task_data dt = *queue.begin();
    return dt;
  }
  virtual void pop_from_queue()
  {
    queue.erase(queue.begin());
  }
  virtual void set_task_as_execute(task_data dt)
  {
    tasks_in_execute.push_back(dt);
  }
  virtual void set_task_as_done(task_data dt)
  {
    queue_mutex_start();
    queue_tasks_done.push_back(dt);
    int s = tasks_in_execute.size();
    for(int i=0;i<s;i++)
      {
	task_data d = tasks_in_execute[i];
	if (d.num==dt.num)
	  {
	    tasks_in_execute.erase(tasks_in_execute.begin()+i);
	  }
      }
    queue_mutex_end();
    if (in_join)
      {
	pthread_mutex_lock(mutex2);
	pthread_cond_signal(cond2);
	pthread_mutex_unlock(mutex2);
      }
  }
  virtual void wait_for_push_or_shutdown()
  {
    pthread_mutex_lock(mutex);
    while(queue.size()==0&&!m_shutdown_ongoing)
      {
	//pthread_cond_wait(cond, mutex);
	int val = wait_with_timeout(cond,mutex,1000);
	if (val==1) // timeout
	  {
	    //std::cout << "Warning: wait timeout!" << std::endl;
	    //timeout_getevent();
	  }
      }
    pthread_mutex_unlock(mutex);
  }
      
  virtual void start_shutdown()
  {
    m_shutdown_ongoing=true;
    pthread_mutex_lock(mutex);
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);
  }
  virtual bool shutdown_ongoing()
  {
    return m_shutdown_ongoing;
  }
  virtual void join_id(int id)
  {
    //std::cout << "join waiting " << id << std::endl;
      in_join=true;
    pthread_mutex_lock(mutex2);
    while(1) {

    queue_mutex_start();
    
    int count=0;
    int s = queue.size();
    for(int i=0;i<s;i++)
      {
	if (queue[i].id == id) count++;
      }
    int s2=tasks_in_execute.size();
    for(int i=0;i<s2;i++)
      {
	if (tasks_in_execute[i].id==id) count++;
      }
    queue_mutex_end();
    if (count==0) break;
    repeat:
    {
	int val = wait_with_timeout(cond2,mutex2,1000);
	if (val==1) // timeout
	  {
	    timeout_getevent();
	    //std::cout << "Warning: wait timeout!" << std::endl;
	    goto repeat;
	  }
    }
    }
    pthread_mutex_unlock(mutex2);

    queue_mutex_start();

    int s3 = queue.size();
    for(int i=0;i<s3;i++)
      {
	if (queue[i].id==id) { queue.erase(queue.begin()+i); i--; s3--; }
      }
    int s4 = tasks_in_execute.size();
    for(int i=0;i<s4;i++)
      {
	if (tasks_in_execute[i].id==id) { tasks_in_execute.erase(tasks_in_execute.begin()+i); i--; s4--; }
      }
    int s5 = queue_tasks_done.size();
    for(int i=0;i<s5;i++)
      {
	if (queue_tasks_done[i].id==id) { queue_tasks_done.erase(queue_tasks_done.begin()+i); i--; s5--; }
      }
    queue_mutex_end();
    
      in_join=false;
    //std::cout << "join exiting " << id << std::endl;
  }
private:
  std::vector<task_data> queue;
  std::vector<task_data> tasks_in_execute;
  std::vector<task_data> queue_tasks_done;
  std::vector<pthread_t*> threads;
  pthread_mutex_t *queue_mutex;
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
  pthread_mutex_t *mutex2;
  pthread_cond_t *cond2;
  std::atomic<bool> m_shutdown_ongoing=false;
  bool in_join=false;
  int in_join_id=-1;
  int m_join_missed=-1;
  int m_join_missed_count=0;
  std::atomic<int> m_join_var=0;
};
task_implementation g_tasks_implementation;
task_interface &g_tasks = g_tasks_implementation;



#ifdef EMSCRIPTEN
void blocks_success(emscripten_fetch_t *fetch);
void blocks_failed(emscripten_fetch_t *fetch);
void chunk_success(emscripten_fetch_t *fetch);
void chunk_failed(emscripten_fetch_t *fetch);
void fetch_download_progress(emscripten_fetch_t *fetch);

class FetchInBlocks;

struct FetchInBlocksUserData
{
  FetchInBlocks *m_this;
  int block_num;
  long long start;
  long long end;
};

extern bool g_concurrent_download;

class FetchInBlocks
{
public:
  FetchInBlocks(std::string url, void(*success)(void*),void(*failed)(void*), void*data) : url(url),success(success),failed(failed),data(data) { }
  ~FetchInBlocks() { delete [] buf; }
  void fetch()
  {
    fetch_size();
  }
  const std::vector<unsigned char, GameApiAllocator<unsigned char> > *get() const { return &result; }
private:
  void fetch_size()
  {

    //std::cout << "fetch_size" << std::endl; 
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.userData = (void*)this;
    attr.attributes = EMSCRIPTEN_FETCH_REPLACE| EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = blocks_success;
    attr.onerror = blocks_failed;

    unsigned int r = rand();
    std::stringstream ss;
    ss << r;

    std::string url0 = gameapi_homepageurl;
    int res=0;
    int ss2 = url0.size();
    for(int i=0;i<ss2;i++) if (url0[i]=='/') res=i;
    std::string url1 = url0.substr(0,res);

    if (url1=="") url1="https://meshpage.org"; // this doesnt really work, since it gives CORS problem
    
    std::string url2 = url1+"/get_file_size.php?" + ss.str() + "&url=" +url; 
    emscripten_fetch(&attr, url2.c_str());

    int val = 0;
    
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);

    int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,val,15*150,"fetch: "+url_only2);
  }

  }
public:
  void size_success(emscripten_fetch_t *fetch)
  {
    //std::cout << "size_success: " << (int)fetch << " " << int(fetch->data) << " " << int(fetch->numBytes) << std::endl;
    if (!fetch || !fetch->data || !fetch->numBytes) { size_failed(fetch); return; } 
    std::string res(&fetch->data[0],&fetch->data[fetch->numBytes]);
    //std::cout << "RES: '" << res << "'" << std::endl;


    
    if (res.size()==0) { size_failed(fetch); return; }
    
    char ch = res[0];
    res = res.substr(1);

    if (ch=='B') { mode=1; }
    
    std::stringstream ss(res);
    totalSize = 0;
    chunkSize = 1048576;
    ss >> totalSize;
    ss >> chunkSize;
    
    int concurrent_tasks = 4;
    //totalSize = fetch->totalBytes;
    //std::cout << "Size Success: " << totalSize << std::endl;
    if (chunkSize==0) { chunkSize=1048576; }
    if (totalSize==0) { size_failed(fetch); return; }    
    else {

      // std::cout << "FETCH_SIZE:" << std::endl;
      if (!g_concurrent_download) {
	//std::cout << "FAILED IMMEDIATELY" << std::endl;
	failed_after_size(fetch,totalSize);
	return;
      }
      //std::cout << "FETCHIND SIZE" << std::endl;

      


      
      result.resize(totalSize+1);
      int t = totalSize/chunkSize+1;
      blocks_ready.resize(t);
      //fetch_block(0);
      int s = t;
      for(int i=0;i<s;i++) {
	blocks_ready[i]=0;
      }
      for(int i=0;i<std::min(concurrent_tasks,s);i++) {
	fetch_block(i);
      }
    }
    emscripten_fetch_close(fetch);

    int val = 1;
    int mult = totalSize/chunkSize;
    if (mult<1) mult=1;
    
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,val,15*mult+1,"fetch: "+url_only2);
  }


  }
  void size_failed(emscripten_fetch_t *fetch)
  {
    std::cout << "FetchInBlocks::size_failed()" << std::endl;
    failed(data);
    emscripten_fetch_close(fetch);
  }
  void failed_after_size(emscripten_fetch_t *fetch, int sz);
private:
  void fetch_block(int id)
  {
    //std::cout << "Fetch Block" << id << std::endl;
    blocks_ready[id]=2;
    //current_id = id;
    start = id*chunkSize;
    end = (id+1)*chunkSize;
    if (end>=totalSize) end=totalSize;
    //std::cout << "START CHUNK" << id << " " << start << " " << end << " " << totalSize << std::endl;
    std::stringstream ss;
    ss << "bytes=" << start << "-" << end;
    std::string res = ss.str();
    delete [] buf;
    buf = new char[res.size()+1];
    std::copy(res.begin(),res.end(),buf);
    buf[res.size()]=0;
    const char *headers[] = {"Range", buf,
       NULL };
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    FetchInBlocksUserData *dt = new FetchInBlocksUserData;
    dt->m_this = this;
    dt->block_num = id;
    dt->start = start;
    dt->end = end;
    attr.userData = (void*)dt;
    attr.attributes = EMSCRIPTEN_FETCH_REPLACE| EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = chunk_success;
    attr.onerror = chunk_failed;
    //attr.onprogress = fetch_download_progress;
    if (mode!=1) { 
      attr.requestHeaders = headers;
    }
    if (mode!=1) {
      char *ptr = new char[url.size()+1];
      std::copy(url.begin(),url.end(),ptr);
      ptr[url.size()]=0;

      
      emscripten_fetch(&attr, ptr);
    } else { // split tool+brotli can be used to split the files beforehand, 1M blocks
      int id1 = id / 26;
      int id2 = id - (id1*26);
      char *pp = "abcdefghijklmnopqrstuvwxyz";
      char c1 = pp[id1];
      char c2 = pp[id2];
      //std::string url2;
      url2 = url;
      url2 += ".";
      url2 += c1;
      url2 += c2;
      char *ptr = new char[url2.size()+1];
      std::copy(url2.begin(),url2.end(),ptr);
      ptr[url2.size()]=0;
      emscripten_fetch(&attr, ptr);
    }
  }
public:
  void fetch_success(emscripten_fetch_t *fetch)
  {
    // std::cout << "fetch_success: " << (int)fetch << " " << int(fetch->data) << " " << int(fetch->numBytes) << " " << fetch->status << std::endl;
    if (!fetch || !fetch->data || !fetch->numBytes) { fetch_failed(fetch); return; } 
    FetchInBlocksUserData *ptr = (FetchInBlocksUserData*)(fetch->userData);
    //std::cout << fetch->statusText << std::endl;
    //std::cout << fetch->status << std::endl;
    //std::cout << "FETCHED BLOCK: " << fetch->dataOffset << " " << fetch->numBytes << std::endl;

    if (fetch->status==200||fetch->status==206) // OK ONE HAS WRONG SIZE BLOCK IN IT, PARTIAL CONTENT PROBABLY ALSO WORKS WITH THIS CODE
      {
	for(int i=0;i<blocks_ready.size();i++)
	  {
	    //std::cout << blocks_ready[i] << " ";
	    if (blocks_ready[i]!=1) {
	      start = i*chunkSize;
	      end = (i+1)*chunkSize;
	      if (start>totalSize) start=totalSize;
	      if (end>totalSize) end=totalSize;
	      int dataOffset = fetch->dataOffset;
	      int numBytes = fetch->numBytes;
	      if (fetch->status==206||mode==1)
		{
		  dataOffset+=ptr->start;
		}
	      //std::cout << "FETCH:" << start << " " << end << " " << dataOffset << " " << numBytes << std::endl;
	      if (dataOffset<=start && dataOffset+numBytes>=end-1)
		{
		  int offset = start-dataOffset;
		  std::copy(&fetch->data[offset], &fetch->data[offset+(end-start)], &result[start]);
		  blocks_ready[i]=1;
		  //std::cout << "BLOCK READY: " << i << std::endl;
		  //} else {
	      //int offset = start-dataOffset;
	      //std::copy(&fetch->data[offset], &fetch->data[offset+std::min(numBytes,end-start)], &result[start]);
	    }
	    
	    }
	  }
	//std::cout << "kk" << std::endl;
  
  }
    //std::cout << "Chunk Success " << current_id << std::endl;
    //assert(fetch->numBytes==0||fetch->numBytes==end-start);
    /*
    if (fetch->status==206) { // Partial Content
      start = ptr->start;
      end = ptr->end;
      std::copy(fetch->data, fetch->data+(end-start), &result[start]);
      int num = ptr->block_num;    
      std::cout << "Fetch Success:" << num << std::endl;
      blocks_ready[num]=1;
      }*/
    int s = blocks_ready.size();
    bool fail = false;
    std::vector<int> next;
    for(int i=0;i<s;i++)
      {
	//std::cout << blocks_ready[i] << " ";
	if (blocks_ready[i]==0) { fail=true; next.push_back(i); continue; }
	if (blocks_ready[i]==2) { fail=true; continue; }
      }
    //std::cout << "kk" << std::endl;
    if (!fail)
      {
	//std::cout << "Exiting via Success" << std::endl;
	success(data);
      }
    else
      {
	if (next.size()!=0) {
	  if (fetch->status==206 && firsttime) {
	    int s = std::min(next.size(),(unsigned long)8);
	    for(int i=0;i<s;i++) {
	      fetch_block(next[i]);
	    }
	    firsttime = false;
	  } else {
	    fetch_block(next[0]);
	  }
	}
      }
    //if (end==totalSize) { success(data); }
    //else {
    //  fetch_block(current_id+1);
    //}
    delete ptr;
    emscripten_fetch_close(fetch);
    //std::cout << "CHUNK" << current_id << " " << start << " " << end << " " << totalSize << std::endl;

    long long mult = totalSize/chunkSize;
    if (mult<1) mult=1;

    long long end2=0;
    int s2 = blocks_ready.size();
    for(int i=0;i<s2;i++)
      {
	if (blocks_ready[i]==1)
	  end2+=(long long)chunkSize;
      }
    
    int val = 1+((long long)(end2))*(long long)15*mult/(long long)(totalSize);
    //std::cout << "VAL=" << val << std::endl;
    if (val<0) val=0;
    if (val>15*mult+1) val=15*mult+1;

  if (g_logo_status==0)
    g_logo_status = 1;

    
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,val,15*mult+1,"fetch: "+url_only2);
  }
    
  }
  void fetch_failed(emscripten_fetch_t *fetch)
  {
    std::cout << "fetch_failed" << std::endl;
    failcount++;
    if (failcount>10) {
      std::cout << "FetchInBlocks::chunk_failed()" << std::endl;
      failed(data);
      emscripten_fetch_close(fetch);
    }
    else
      {
	FetchInBlocksUserData *ptr = (FetchInBlocksUserData*)(fetch->userData);
	int blk = ptr->block_num;
	fetch_block(blk);
      }
  }
public:
  std::string url;
  std::string url2;
  std::vector<unsigned char,GameApiAllocator<unsigned char> > result;
  long long totalSize;
  long long chunkSize;
  void(*success)(void*);
  void(*failed)(void*);
  void *data;
  long long start,end;
  //int current_id;
  char *buf=0;
  std::vector<int> blocks_ready;
  bool firsttime=true;
  int mode = 0;
  int failcount=0;
};
void blocks_success(emscripten_fetch_t *fetch)
{
  FetchInBlocks *ptr = (FetchInBlocks*)(fetch->userData);
  ptr->size_success(fetch);
}
void blocks_failed(emscripten_fetch_t *fetch)
{
  FetchInBlocks *ptr = (FetchInBlocks*)(fetch->userData);
  ptr->size_failed(fetch);
}
void chunk_success(emscripten_fetch_t *fetch)
{
  FetchInBlocksUserData *ptr = (FetchInBlocksUserData*)(fetch->userData);
  ptr->m_this->fetch_success(fetch);
}
void chunk_failed(emscripten_fetch_t *fetch)
{
  FetchInBlocksUserData *ptr = (FetchInBlocksUserData*)(fetch->userData);
  ptr->m_this->fetch_failed(fetch);
}
#endif



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


//#ifdef RASPI
//inline int strlen(const char *ptr) { const char *p = ptr; while(*p) { p++;  } return p-ptr;}
//#endif

IMPORT std::string g_gpu_vendor;
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
void print_dependencies(EnvImpl &impl, int num);
EXPORT void GameApi::Env::print_dependencies(int num)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  ::print_dependencies(*env,num);
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
EXPORT int GameApi::Env::add_to_download_bar(std::string filename)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->add_to_download_bar(filename);
}
EXPORT int GameApi::Env::download_index_mapping(int index)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->download_index_mapping(index);
}
EXPORT int GameApi::Env::download_bar_count() const
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->download_bar_count();
}
EXPORT void GameApi::Env::set_download_data(int i, const std::vector<unsigned char> &file)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->set_download_data(i,file);
}
EXPORT void GameApi::Env::set_download_progress(int i, float percentage)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->set_download_progress(i,percentage);
}
EXPORT void GameApi::Env::set_download_ready(int i)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->set_download_ready(i);
}
EXPORT std::string GameApi::Env::get_download_bar_filename(int i) const
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->get_download_bar_filename(i);
}
EXPORT float GameApi::Env::get_download_bar_progress(int i) const
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->get_download_bar_progress(i);
}
EXPORT bool GameApi::Env::get_download_bar_ready(int i) const
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->get_download_bar_ready(i);
}
EXPORT void GameApi::Env::remove_download_bar_item(int i)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->remove_download_bar_item(i);
}
EXPORT int GameApi::Env::start_async(ASyncTask *task)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->start_async(task);
}
IMPORT int GameApi::Env::async_mapping(int index)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  return env->async_mapping(index);
}
IMPORT void GameApi::Env::remove_async(int i)
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->remove_async(i);
}
EXPORT void GameApi::Env::async_scheduler()
{
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_scheduler();
}

struct DeleterItem
{
  int id;
  void (*fptr)(void*);
  void *data;
};

std::vector<DeleterItem> g_cache_deleter;

EXPORT int register_cache_deleter(void (*fptr)(void*), void *data)
{
  static int ids=0;
  ids++;
  DeleterItem i;
  i.id=ids;
  i.fptr=fptr;
  i.data=data;
  g_cache_deleter.push_back(i);
  return i.id;
}
EXPORT void unregister_cache_deleter(int id)
{
  int s=g_cache_deleter.size();
  for(int i=0;i<s;i++)
    {
      DeleterItem &ii = g_cache_deleter[i];
      if (ii.id==id) {
	g_cache_deleter.erase(g_cache_deleter.begin()+i);
	return;
      }
    }
}
IMPORT void clear_all_caches()
{
  int s=g_cache_deleter.size();
  for(int i=0;i<s;i++)
    {
      DeleterItem &ii = g_cache_deleter[i];
      ii.fptr(ii.data);
    }
}

//EXPORT std::vector<unsigned char> *GameApi::Env::get_download_bar_item(int i) const
//{
//  ::EnvImpl *env = (::EnvImpl*)envimpl;
//  return env->get_download_bar_item(i);
//}




EXPORT void GameApi::Env::async_load_url(std::string url, std::string homepage, bool nosize)
{
  // std::cout << "async_load_url " << url << std::endl;
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  env->async_loader->load_urls(url, homepage, nosize);

}
EXPORT void GameApi::Env::async_load_all_urls(std::vector<std::string> urls, std::string homepage)
{
  if (!envimpl) return;
  ::EnvImpl *env = (::EnvImpl*)envimpl;
  if (!env) return;
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
std::vector<unsigned char,GameApiAllocator<unsigned char> > *load_from_url(std::string url, bool nosize);


#include "GameApi_dep_env_delmap.hh"

struct del_map : public del_map_interface
{
  del_map() { pthread_mutex_init(&lock, NULL); }
  ~del_map() { pthread_mutex_destroy(&lock); }
  void async_cache_clear()
  {
    pthread_mutex_lock(&lock);
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	delete e.second;
      }
    load_url_buffers_async.clear();
    //std::cout << "Map Clear! -> 0" << std::endl;
    pthread_mutex_unlock(&lock);
  }

#ifdef EMSCRIPTEN
  void del_fetch_url(std::string url)
  {
    pthread_mutex_lock(&lock);
    int s = fetches.size();
    for(int i=0;i<s;i++)
      { 
	VECENTRY2 e = fetches[i];
	if (e.first == url)
	  {
	    delete e.second;
	    fetches.erase(fetches.begin()+i);
	    i--;
	    //break;
	  }
      }
    pthread_mutex_unlock(&lock);
  }
#endif
  void del_async_url(std::string url)
  {
    //std::cout << "del_async_url: 1"<< std::endl;
    pthread_mutex_lock(&lock);
    int s = load_url_buffers_async.size();
    //std::cout << "del_async_url: 2"<< std::endl;
    for(int i=0;i<s;i++)
      {
	//std::cout << "del_async_url: 3"<< std::endl;
	VECENTRY &e = load_url_buffers_async[i];
	//std::cout << "del_async_url: 4"<< std::endl;
	if (e.first == url) {
	  delete e.second; e.second=0;
	  //std::cout << "del_async_url: 5"<< std::endl;
	  //std::cout << "del_async_url: 6"<< std::endl;
	  load_url_buffers_async.erase(load_url_buffers_async.begin()+i);
	  //std::cout << "del_async_url: 7"<< std::endl;
	  i--;
	  //std::cout << "del_async_url: 8"<< std::endl;
	  //std::cout << "Map Pop() -> " << load_url_buffers_async.size() << std::endl;

	  break;
	  //std::cout << "del_async_url: 9"<< std::endl;
	}
	//std::cout << "del_async_url: 10"<< std::endl;
      }
    //std::cout << "del_async_url: 11"<< std::endl;
    pthread_mutex_unlock(&lock);
  }
#ifdef EMSCRIPTEN
  void push_fetch_url(std::string url, FetchInBlocks *blk)
  {
    pthread_mutex_lock(&lock);
    VECENTRY2 e;
    e.first = url;
    e.second = blk;
    fetches.push_back(e);
    pthread_mutex_unlock(&lock);
  }
#endif
  void push_async_url(std::string url, const std::vector<unsigned char, GameApiAllocator<unsigned char> > *ptr)
  {
    //std::cout << "Push async url: " << url << " " << std::hex << (long)ptr << std::dec << std::endl;
    pthread_mutex_lock(&lock);
    VECENTRY e;
    e.first = url;
    e.second = const_cast<std::vector<unsigned char,GameApiAllocator<unsigned char> > *>(ptr);
    load_url_buffers_async.push_back(e);
    //std::cout << "Map Push() -> " << load_url_buffers_async.size() << " " << std::hex << (long)ptr << std::dec << std::endl;
    pthread_mutex_unlock(&lock);
  }
  //~del_map() {

  //}
  void print()
  {

  }
  void del_vec(const std::vector<unsigned char, GameApiAllocator<unsigned char> >* vec)
  {
    pthread_mutex_lock(&lock);

    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY &e = load_url_buffers_async[i];
	if (&*e.second == &*vec) {
	  delete e.second; e.second=0;
	  load_url_buffers_async.erase(load_url_buffers_async.begin()+i);
	  i--;
	  //std::cout << "Map Pop2() -> " << load_url_buffers_async.size() << " " << std::endl;
	  
	  break;
	}
      }

    pthread_mutex_unlock(&lock);

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
    pthread_mutex_lock(&lock);
    int s = fetches.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY2 e = fetches[i];
	if (e.first == url)
	  {
	    pthread_mutex_unlock(&lock);
	    return true;
	  }
      }
    pthread_mutex_unlock(&lock);
    return false;
  }
#endif
  bool async_find(std::string url)
{
  //std::cout << "ASYNC FIND:" << url << std::endl;
    pthread_mutex_lock(&lock);
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	//std::cout << "async_find:" << e.first << std::endl;
	if (e.first == url)
	  {
	    pthread_mutex_unlock(&lock);
	    return true; }
      }
    pthread_mutex_unlock(&lock);
    return false;
}

VECENTRY &async_get(std::string url)
{
    pthread_mutex_lock(&lock);
    int s = load_url_buffers_async.size();
    for(int i=0;i<s;i++)
      {
	VECENTRY e = load_url_buffers_async[i];
	if (e.first == url)
	  { ret=e;
	    pthread_mutex_unlock(&lock);

	    return ret; }
      }
std::cout << "You should check if element exists before calling async_get" << std::endl;
 VECENTRY e2;
 e2.first = "";
 e2.second = 0;
 ret = e2;
    pthread_mutex_unlock(&lock);
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
  pthread_mutex_t lock;
};



del_map g_del_map_impl;
del_map_interface &g_del_map = g_del_map_impl;
bool g_del_map_deleter_installed=false;

void delmap_cache_deleter(void *)
{
  g_del_map.async_cache_clear();
#if 0
  g_del_map.load_url_buffers_async.clear();
  g_del_map.fetches.clear();
#endif
  
#if 0
  std::cout << "delmap_cache_deleter is freeing memory" << std::endl;
  std::vector<VECENTRY>::iterator i = g_del_map.load_url_buffers_async.begin();
  for(;i!=g_del_map.load_url_buffers_async.end();i++)
    {
      std::pair<std::string, std::shared_ptr<const std::vector<unsigned char, GameApiAllocator<unsigned char> > > > p = *i;
#ifdef EMSCRIPTEN
 if (g_del_map.fetches.find(p.first)==g_del_map.fetches.end())
#endif
        {
	  delete p.second.get();
        }
    }
  g_del_map.load_url_buffers_async.clear();

#ifdef EMSCRIPTEN
  std::map<std::string,std::shared_ptr<FetchInBlocks> >::iterator i2 = g_del_map.fetches.begin();
  for(;i2!=g_del_map.fetches.end();i2++)
    {
      std::pair<std::string, std::shared_ptr<FetchInBlocks> > p = *i2;
      delete p.second.get();
    }
  g_del_map.fetches.clear();
#endif
#endif
}


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
  g_async_pending_count_failures++; std::cout << "FAIL: async_pending_count_failure" << std::endl;
  std::cout << "ERROR: url loading error! " << std::endl;
  if (!arg) return;
  std::cout << str << std::endl;
  char *url = (char*)arg;
  std::cout << url << std::endl;
    std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  g_del_map.del_async_url(url_only);
  //g_del_map.load_url_buffers_async.erase(url_only);
  //std::cout << "g_del_map remove url: " << url_only << std::endl;
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
  if (s.size()>0 && s[0]=='l' && s[1]=='o' && s[2]=='a' && s[3]=='d') {
    return s.substr(len,s.size()-len);
  } else return s;
}
void onload_async_cb(unsigned int tmp, void *arg, const std::vector<unsigned char, GameApiAllocator<unsigned char> > *buffer)
{
  const char *url = (const char*)arg;
  //std::cout << "onload_async_cb" << std::endl;
  //std::cout << url << std::endl;
  //std::cout << (int)buffer << std::endl;
  //std::cout << "onload_async_cb: " << url << std::endl;

  /*
  unsigned char *dataptr = (unsigned char*)data;
  if (datasize==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
      std::cout << url << std::endl;
  }
  */
  //std::vector<unsigned char> *buffer = new std::vector<unsigned char>(dataptr,dataptr+datasize);
  //unsigned char *dataptr = (unsigned char*)data;
  //for(unsigned int i=0;i<datasize;i++) { buffer->push_back(dataptr[i]); }
  
  //char *url = (char*)arg;
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  /*

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,7,15,url_only2);
  }
  */
  
  //std::cout << "url loading complete! " << url_str << std::endl;
  // THIS WAS url_only, but seems to have not worked.
  //std::cout << "g_del_map " << url_only << " = " << (int)buffer << std::endl;
  //std::cout << "g_del_map add url: " << url_only << std::endl;
    if (!g_del_map_deleter_installed)
      {
	g_del_map_deleter_installed=true;
	register_cache_deleter(delmap_cache_deleter,0);
      }
#ifdef EMSCRIPTEN
    if (!g_del_map.fetch_find(url_only)
	&& g_del_map.async_find(url_only)) {
      //delete g_del_map.load_url_buffers_async[url_only].get();
      g_del_map.del_async_url(url_only);
    }
#endif
    g_del_map.push_async_url(url_only, buffer );
    // g_del_map.load_url_buffers_async[url_only] = std::make_shared<const std::vector<unsigned char, GameApiAllocator<unsigned char> >>(*buffer);
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

  /*
  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url_only2);
  }*/

}

std::vector<unsigned char, GameApiAllocator<unsigned char> > *load_from_url(std::string url, bool noside);

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
  InstallProgress(sum,url,15*150);
  

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
  //pthread_detach(pthread_self());
  system(str->c_str());
  //pthread_exit(0);
  return 0;
}

IMPORT void pthread_system(std::string str)
{
  std::string *ss = new std::string(str);
  //pthread_t thread_id;
  //pthread_attr_t attr;
  //pthread_attr_init(&attr);
  //pthread_attr_setstacksize(&attr, 300000);
  g_pthread_count++;
  //pthread_create(&thread_id, &attr, &process2, (void*)ss);
  tasks_add(3006,&process2,(void*)ss);
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
  //pthread_detach(pthread_self());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > *buf = load_from_url(url,false);
  std::string url2 = "load_url.php?url=" + url ;
  //std::cout << "g_del_map " << url2 << " = " << (int)buf << std::endl;
  //std::cout << "g_del_map add url(process): " << url2 << std::endl;

#ifdef EMSCRIPTEN
  if (!g_del_map.fetch_find(url2)
      && g_del_map.async_find(url2)) {
    g_del_map.del_async_url(url2);
    //   delete g_del_map.load_url_buffers_async[url2].get();
    }
#endif

    g_del_map.push_async_url(url2,buf);
    //g_del_map.load_url_buffers_async[url2] = std::make_shared<const std::vector<unsigned char, GameApiAllocator<unsigned char> >>(*buf); //new std::vector<unsigned char>(buf);  
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
      if (g_del_map.async_find(url2))
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
    //pthread_attr_t attr;
    //pthread_attr_init(&attr);
    //pthread_attr_setstacksize(&attr, 300000);
    g_pthread_count++;
    //int err = pthread_create(&processdata->thread_id, &attr, &process, (void*)processdata);
    //if (err) { std::cout << "pthread_create returned error " << err << std::endl; }

    tasks_add(3007,&process,(void*)processdata);
    
  }
  long long current_size = 0;
  long long last_size = 0;
  for(int j=0;j<s;j++)
    {
      void *res;
      std::string url = urls[u+j];
      std::string url2 = "load_url.php?url=" + url ;
      while (!(g_del_map.async_find(url2)))
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
class FetchInBlocks;
struct LoadData {
  std::string url;
  std::string url3;
  char *buf2;
  char *buf3;
  FetchInBlocks *obj;
  int final_file_size=-1;
};

#ifdef EMSCRIPTEN
void FetchInBlocks::failed_after_size(emscripten_fetch_t *fetch, int sz)
  {
    LoadData *dt = (LoadData*)data;
    dt->final_file_size = sz;
    
    //std::cout << "Concurrent load disabled -> loading normal way" << std::endl;
    failed(data);
    emscripten_fetch_close(fetch);
  }
#endif

void idb_onload_async_cb(void *ptr, void* data, int datasize)
{
  //onload_async_cb(333,ptr,data,datasize);
}
void idb_onerror_async_cb(void *ptr)
{

    onerror_async_cb(0,ptr,0,"");
}

#ifdef EMSCRIPTEN
void fetch_download_succeed(emscripten_fetch_t *fetch) {
  std::cout << "Fetch success: " << fetch->numBytes << std::endl;
  //std::cout << "Fetch data:" << (unsigned char*)fetch->data << std::endl;
  LoadData *data =(LoadData*)fetch->userData;
  const char *url = data->buf3;
  //onload_async_cb(333, (void*)url, (void*)fetch->data, fetch->numBytes+1);
  emscripten_fetch_close(fetch);
}
void fetch_download_failed(emscripten_fetch_t *fetch) {
  std::cout << "Fetch failed: " << fetch->numBytes << std::endl;
  LoadData *data =(LoadData*)fetch->userData;
  const char *url = data->buf3;
  onerror_async_cb(333, (void*)url, 0, "fetch failed!");
  emscripten_fetch_close(fetch);
}


// THis is still used...
void fetch_download_progress(emscripten_fetch_t *fetch) {
  //std::cout << "fetch progress:" << fetch->dataOffset << " " << fetch->totalBytes<< " " << fetch->numBytes << std::endl;
  int val = 7;
  if (fetch->totalBytes) {
    val = fetch->dataOffset * 15 / fetch->totalBytes;
    if (val>15) val=15;
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
      if (strcmp(ptr[0],"uncompressed-length")==0) res = ptr[1];
    }
    int total = 0;
    if (res) {
      std::stringstream ss(res);
      ss >> total;
    }
    //std::cout << "GOT CONTENT LENGTH: " << total << std::endl;
    if (total != 0)
      val = 15*(fetch->dataOffset + fetch->numBytes)/total;
    if (val>15) val=15;
  }
  //std::cout << "logo:" << val << std::endl;
  if (g_logo_status==0)
    g_logo_status = 1;
  
  FetchInBlocks *data =(FetchInBlocks*)fetch->userData;
  std::string url = data->url;
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
  /*
  LoadData *ld = (LoadData*)arg;
  //std::cout << "Exists: " << exists << std::endl;
  if (exists && !idb_disabled) {
    //std::cout << "Loading from idb" << ld->url << std::endl;
    emscripten_idb_async_load("gameapi", ld->url.c_str(), (void*)ld->buf3, &idb_onload_async_cb, &idb_onerror_async_cb);
  } else {
    //std::cout << "Loading from wget" << ld->url << std::endl;
    emscripten_async_wget2_data(ld->buf2, "POST", ld->url3.c_str(), (void*)ld->buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
    }*/
#endif
}
void idb_error(void *arg)
{
  /*
  std::cout << "indexdb error branch, loading wget data again..." << std::endl;
#ifdef EMSCRIPTEN
  LoadData *ld = (LoadData*)arg;
    emscripten_async_wget2_data(ld->buf2, "POST", ld->url3.c_str(), (void*)ld->buf3, 1, &onload_async_cb, &onerror_async_cb, &onprogress_async_cb);
#else
  LoadData *ld = (LoadData*)arg;
    onerror_async_cb(0,(void*)ld->buf3,0,"");
#endif
  */
}
std::vector<std::string> g_currently_loading;
std::string remove_load(std::string s);

extern std::vector<const unsigned char*> g_content;
extern std::vector<const unsigned char*> g_content_end;
extern std::vector<const char*> g_urls;


int CalcUrlIndex(std::string url)
{
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  return sum;
}

#ifdef EMSCRIPTEN
void fetch_success(void *data)
{
  LoadData *dt = (LoadData*)data;
  const char *url = dt->buf3;
  const std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec = dt->obj->get();
  onload_async_cb(333,(void*)url, vec);
#ifdef EMSCRIPTEN
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  g_del_map.push_fetch_url(url_only,dt->obj);
  //g_del_map.fetches[url_only]=std::make_shared<FetchInBlocks>(*dt->obj);
#endif
  // note, the dt->obj is NOT DELETED AT ALL
}
void fetch_2_success(emscripten_fetch_t *fetch)
{
  //std::cout << "2nd attempt successful" << std::endl;
  LoadData *dt = (LoadData*)(fetch->userData);
  const char *url = dt->buf3;
  dt->obj->result = std::vector<unsigned char,GameApiAllocator<unsigned char> >(&fetch->data[0],&fetch->data[fetch->numBytes]);
  // hack to fix the download amounts.
  //std::cout << "FETCH2SUCCESS:" << dt->final_file_size << "==" << fetch->numBytes << std::endl;
  if (dt->final_file_size!=-1 && fetch->numBytes != dt->final_file_size) {
    std::cout << "Size Compare:" << dt->final_file_size << "==" << fetch->numBytes << std::endl;
     dt->obj->result.push_back(' ');
  }
  const std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec = dt->obj->get();  
  onload_async_cb(333,(void*)url,vec);
#ifdef EMSCRIPTEN
  std::string url_str(url);
  std::string url_only(striphomepage(url_str));
  g_del_map.push_fetch_url(url_only,dt->obj);
  //g_del_map.fetches[url_only]=std::make_shared<FetchInBlocks>(*dt->obj);
#endif
  emscripten_fetch_close(fetch);
}
void fetch_2_error(emscripten_fetch_t *fetch)
{
  g_async_pending_count_failures++; std::cout << "FAIL: async_pending_count_failure" << std::endl;
  std::cout << "ERROR: 2nd attempt at loading the data failed" << std::endl;
  emscripten_fetch_close(fetch);
}
void fetch_2_progress(emscripten_fetch_t *fetch)
{

  if (g_logo_status==0)
    g_logo_status = 1;

  
  LoadData *dt = (LoadData*)(fetch->userData);
  float perc=0.0;
  if (fetch->totalBytes) {
    perc = (fetch->dataOffset+fetch->numBytes) * 100.0 / fetch->totalBytes;
  } else {
    perc = float(fetch->dataOffset + fetch->numBytes)*100.0/ float(1024)/float(1024)/float(4);
  }
  //std::cout << "dataoffset = " << fetch->dataOffset << std::endl;
  //std::cout << "numbytes = " << fetch->numBytes << std::endl;
  //std::cout << "totalbytes = " << fetch->totalBytes << std::endl;
  //std::cout << "PROGRESS:" << perc << std::endl;
  std::string url_str(dt->url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);
  int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,15*150*perc/100.0+1,15*150,"fetch: "+url_only2);
  }

  
}
void fetch_failed(void *data)
{
  LoadData *dt = (LoadData*)data;
  //delete dt->obj;

  //std::cout << "Fetching url failed.. trying again... (it's possible get_file_size.php is not available)  " << std::endl;
  //std::cout << dt->url << std::endl;

  std::string url_str(dt->url);
  std::string url_only(striphomepage(url_str));

  { // progressbar
    std::string url_only2 = stripprefix(url_only);

    int s = url_only2.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url_only2[i]);
  sum = sum % 1000;
  //std::cout << "progressbar: " << sum << " " << val << " " <<  url_only2 << std::endl;
  ProgressBar(sum,0,15*150,"fetch: "+url_only2);
  }

  
  // TRY AGAIN... (possibly get_size.php missing)
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes=EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess=fetch_2_success;
  attr.onerror=fetch_2_error;
  attr.onprogress=fetch_2_progress;
  attr.userData=(void*)dt;
  emscripten_fetch(&attr,dt->url.c_str());

}
#endif

void ASyncLoader::load_urls(std::string url, std::string homepage, bool nosize)
  {
    //std::cout << "load_urls:" << url << std::endl;
    if (url=="") return;
    //std::cout << "ASyncLoader::load_urls:" << url << std::endl; 

  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << "ASyncLoader::load_urls::" << remove_load(url) << " " << g_urls[i] << std::endl;
      if (remove_load(url)==g_urls[i]) { 
	//std::vector<unsigned char> *vec = new std::vector<unsigned char>(g_content[i], g_content_end[i]);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	std::string url_only = "load_url.php?url=" + url;

    if (!g_del_map_deleter_installed)
      {
	g_del_map_deleter_installed=true;
	register_cache_deleter(delmap_cache_deleter,0);
      }
    //std::cout << "g_del_map: " << (int)(g_content[i]) << " " << (int)(g_content_end[i]) << " " << (int)(g_content_end[i]-g_content[i]) << std::endl;
#ifdef EMSCRIPTEN
    if (!g_del_map.fetch_find(url_only)
	&& g_del_map.async_find(url_only)) {
      g_del_map.del_async_url(url_only);
      //delete g_del_map.load_url_buffers_async[url_only].get();
    }
#endif
    g_del_map.push_async_url(url_only,new std::vector<unsigned char, GameApiAllocator<unsigned char> >(g_content[i],g_content_end[i]));
			     //g_del_map.load_url_buffers_async[url_only] = std::make_shared<const std::vector<unsigned char, GameApiAllocator<unsigned char> >>(std::vector<unsigned char, GameApiAllocator<unsigned char> >(g_content[i],g_content_end[i]));
	//std::cout << "g_del_map add url: " << url_only << std::endl;

	//async_pending_count--;
	// std::cout << "g_del_map " << url_only << " = " << (int)g_del_map.load_url_buffers_async[url_only] << std::endl;

	
	
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
  if (g_del_map.async_find(url2)) { 
    InstallProgress(sum,url + " (cached)",15*150);
  } else {
  InstallProgress(sum,url,15*150);

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


    const char * headers[] = { /*"Content-type", "application/json; charset=UTF-8",*/ 0};
    
    //std::cout << "url loading started! " << url << std::endl;

  //std::cout << "URL:" << url << std::endl;
    // if we have already loaded the same url, don't load again
    if (/*load_url_buffers_async[url] ||*/g_del_map.async_find(url_only)) {
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

    bool is_same_server = true; //is_urls_from_same_server(oldurl,g_window_href);
    bool is_same_server2 = is_urls_from_same_server(oldurl,homepage+"/");
    
    //emscripten_idb_async_exists("gameapi", oldurl.c_str(), (void*)ld, &idb_exists, &idb_error);
    if (extract_server(oldurl)=="" || is_same_server || is_same_server2) {
      FetchInBlocks *b = new FetchInBlocks(oldurl,fetch_success,fetch_failed,(void*)ld);
      ld->obj = b;
      b->fetch();
      /*
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.userData = (void*)ld;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY| EMSCRIPTEN_FETCH_APPEND;
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
      */
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
  if (g_del_map.async_find(url2)) { 
    ProgressBar(sum,0,15,url+" (cached)");
  } else {
    ProgressBar(sum,0,15,url);
  }
  }

    std::string url2 = "load_url.php?url=" + url ;
    if (g_del_map.async_find(url2)) { 
      { // progressbar
	int s = url.size();
	int sum=0;
	for(int i=0;i<s;i++) sum+=int(url[i]);
	sum = sum % 1000;
	ProgressBar(sum,15,15,url + " (cached)");
      }

      return; }
    std::cout << "Loading url: " << url <<std::endl;
    std::vector<unsigned char, GameApiAllocator<unsigned char> > *buf = load_from_url(url,nosize);
    //std::cout << "Loading url finished: " << url <<std::endl;
    if (buf->size()==0) {
      std::cout << "Empty URL file. Either url is broken or homepage is wrong." << std::endl;
      std::cout << url << std::endl;
    }
    //std::cout << "g_del_map " << url2 << " = " << (int)buf << std::endl;
    //std::cout << "g_del_map add url(loadurls): " << url2 << std::endl;
    if (!g_del_map_deleter_installed)
      {
	g_del_map_deleter_installed=true;
	register_cache_deleter(delmap_cache_deleter,0);
      }
#ifdef EMSCRIPTEN
    if (g_del_map.fetches.find(url2)==g_del_map.fetches.end()
	&& g_del_map.load_url_buffers_async.find(url2)!=g_del_map.load_url_buffers_async.end()) {
      delete g_del_map.load_url_buffers_async[url2].get();
    }
#endif

    g_del_map.push_async_url(url2,buf);
    //g_del_map.load_url_buffers_async[url2] = std::make_shared<const std::vector<unsigned char, GameApiAllocator<unsigned char> >>(*buf); //new std::vector<unsigned char>(buf);
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
  ASyncDataFetcher(const std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec) : vec(vec),buf(0),end2(0),tmp(0) { }
  ASyncDataFetcher(const unsigned char *buf,const unsigned char *end) : vec(0), buf(buf), end2(end),tmp(0) { }
  void del() {
    g_del_map.del_vec(vec);
    
  }
  const unsigned char &operator[](int i) const {
    if (buf && buf+i<end2) return buf[i];
    if (vec) return (*vec)[i];
    //std::cout << "ERROR: AsyncDataFetcher returning null string" << std::endl;
    //throw 1;
    return tmp;
  }
  int size() const { if (buf) return end2-buf; if (vec) return vec->size();
    //std::cout << "ERROR: AsyncDataFetcher returning null string" << std::endl;
    //throw 1;
    return 0;  }
  const unsigned char *begin() const { if (buf) return buf; if (vec) return vec->data();
    //std::cout << "ERROR: AsyncDataFetcher returning null string" << std::endl;
    //throw 1;

    return &tmp; }
  const unsigned char *end() const { if (end2) return end2; if (vec) return vec->data() + vec->size();
    //std::cout << "ERROR: AsyncDataFetcher returning null string" << std::endl;
    //throw 1;
    return &tmp+1; }
private:
  const std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec;
  const unsigned char *buf, *end2;
  mutable unsigned char tmp;
};

GameApi::ASyncVec *g_convert(std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec)
{
  return new ASyncDataFetcher(vec);
}

GameApi::ASyncVec *ASyncLoader::get_loaded_data(std::string url) const
  {
  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      // std::cout << "get_loaded_data:" << remove_load(url) << " " << g_urls[i] << std::endl;

      if (remove_load(url)==std::string(g_urls[i])) { 
	//std::vector<unsigned char> *vec = new std::vector<unsigned char>(g_content[i], g_content_end[i]);
	//std::cout << "load_from_url using memory: " << url << std::endl;
	return new ASyncDataFetcher(g_content[i], g_content_end[i]);
	//	return vec;
      }
    }


    
    url = "load_url.php?url=" + url;
    //std::cout << "url fetch " << url << std::endl;

    /*
    std::map<std::string,const std::vector<unsigned char>*>::iterator i = g_del_map.load_url_buffers_async.begin();
    for(;i!=g_del_map.load_url_buffers_async.end();i++) {
      std::pair<std::string,const std::vector<unsigned char>*> p = *i;
      //std::cout << "DELMAPITEM:" << p.first << std::endl;
    }
    */
    // g_del_map.print();
    if (g_del_map.async_find(url))
      return new ASyncDataFetcher(&*g_del_map.async_get(url).second);
    else
      return 0;
  }


struct ProgressI {
  int num;
  long long value;
  int ticks;
};
void SetTicks(int num, int ticks);


IMPORT std::string g_original_title;
IMPORT Low_SDL_Window *sdl_window;
int g_last_tick=0;
bool g_has_title=false;
std::vector<ProgressI > progress_max;
std::vector<ProgressI > progress_val;
std::vector<std::string> progress_label;

std::vector<int> g_setup;
std::vector<int> g_setup_count;

IMPORT void ClearProgress() { progress_max.clear(); progress_val.clear(); progress_label.clear(); g_setup.clear(); g_setup_count.clear(); }
IMPORT void InstallProgress(int num, std::string label, int max=15)
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
  if (!max_done) {
    p.value = max;
    progress_max.push_back(p);
  }
  if (!val_done) {
    p.value = 0;
    progress_val.push_back(p);
  }
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

int g_val2;
int g_max2;

IMPORT long long FindProgressVal()
{

#if 0
  int res=0;
  std::vector<int> done_vec;
  for(int i=0;i<g_setup.size();i++)
    {
      int s = g_setup[i];
      int s2 = g_setup_count[i];
      
      for(int j=0;j<progress_val.size();j++)
	{
	  if (s == progress_val[j].num) { res+=progress_val[j].value*s2; done_vec.push_back(s); }
	}
    }
  for(int ii=0;ii<g_setup.size();ii++)
    {
      int s = g_setup[ii];
      int s2 = g_setup_count[ii];
      
  if (s==-1) {
    for(int i=0;i<progress_val.size();i++)
      {
	bool skip = false;
	for(int k=0;k<done_vec.size();k++) {
	  if (done_vec[k]==progress_val[i].num) { skip=true; break;}
	}
	if (skip) { continue; }
	res+=progress_val[i].value*s2;
      }
  }
  
    }
#endif  
  
  //return res;
#if 0
  
  int s = progress_val.size();
  float sum = 0.0;
  for(int i=0;i<s;i++) {
    bool skip =false;
    for(int j=0;j<sizeof(progress_remove_list)/sizeof(int);j++)
      if (progress_val[i].num==progress_remove_list[j]) skip=true;
    if (!skip) {
      float mult = 1.0;
      //if (s<sizeof(progress_val_mult)/sizeof(progress_val_mult[0]))
      //	mult = progress_val_mult[s];
      float val = float(progress_val[i].value)/progress_max[i].value;
      val = 1.0-val;
      sum+=val;
      //std::cout << "ProgressVal:" << progress_val[i].num << " " << progress_val[i].value << " " << progress_label[i] << std::endl;
    }
  }
  return (s-sum)*256.0;

#endif
  int s = progress_val.size();
  long long sum = 0;
  for(int i=0;i<s;i++)
    {
      sum+=(long long)progress_val[i].value;
    }
  return sum;
  
}
int g_async_load_count = 0;
long long FindProgressMax()
{

#if 0
  int res=0;
  std::vector<int> done_vec;
  for(int i=0;i<g_setup.size();i++)
    {
      int s = g_setup[i];
      int s2 = g_setup_count[i];
      
      for(int j=0;j<progress_max.size();j++)
	{
	  if (s == progress_max[j].num) { res+=progress_max[j].value*s2; done_vec.push_back(s); }
	}
    }
  for(int ii=0;ii<g_setup.size();ii++)
    {
      int s = g_setup[ii];
      int s2 = g_setup_count[ii];
  if (s==-1) {
    for(int i=0;i<progress_max.size();i++)
      {
	bool skip = false;
	for(int k=0;k<done_vec.size();k++) {
	  if (done_vec[k]==progress_max[i].num) { skip=true; break;}
	}
	if (skip) { continue; }
	res+=progress_max[i].value*s2;
      }
  }
    }
  
#endif
  
  //return res;
  //return g_max2;
#if 0    
  int s = progress_max.size();
  float sum = 0.0;
  for(int i=0;i<s;i++)
    {
    bool skip =false;
    for(int j=0;j<sizeof(progress_remove_list)/sizeof(int);j++)
      if (progress_val[i].num==progress_remove_list[j]) skip=true;
    if (!skip) {
      float val = float(progress_max[i].value)/15.0;
      val = 1.0-val;
      sum+=val;
      //std::cout << "ProgressMax:" << progress_val[i].num << " " << progress_max[i].value << " " << progress_label[i] << std::endl;
    }
    }
  //if (s<4) { sum+=15*(4-s); }
  return (s-sum)*256.0;
#endif

  int s = progress_max.size();
  long long sum = 0;
  for(int i=0;i<s;i++)
    {
      sum+=(long long)progress_max[i].value;
    }
  return sum;

}
IMPORT void FinishProgress()
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
IMPORT std::vector<std::string> g_prog_labels;
IMPORT GameApi::W g_progress_dialog;
IMPORT GameApi::EveryApi *g_everyapi2=0;
IMPORT GameApi::GuiApi *g_everyapi_gui=0;
IMPORT GameApi::FtA g_atlas;
IMPORT GameApi::BM g_atlas_bm;
IMPORT bool g_progress_callback_set=false;
IMPORT void (*g_progress_callback)();

IMPORT void (*update_progress_dialog_cb)(GameApi::W &w, int,int, GameApi::FtA, GameApi::BM, std::vector<std::string>, int val, int max);




void SetupProgress(int num, int count)
{
  g_setup.push_back(num);
  g_setup_count.push_back(count);
}


IMPORT pthread_t g_main_thread_id;
bool g_progress_bar_show_logo=false;
void (*g_progress_bar_logo_cb)(void*);
void *g_progress_bar_logo_cb_data=0;


void ProgressBar(int num, int val, int max, std::string label)
{
#ifndef EMSCRIPTEN
  //  if (getpid()!=gettid()) return; // DO NOT EXECUTE IN PTHREADS
  pthread_t curr = pthread_self();
  if (!pthread_equal(curr, g_main_thread_id)) return;
#endif
  /// std::cout << "ProgressBar: '" << num << " " << label << " " << val << " " << max << "'" << std::endl;

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
	if (num2==num) {
	  progress_val[i].value = val;
	  val_index=i;
	  done = true;
	}
	val_value+=progress_val[i].value;
      }
  }
  if (!done) {
      ProgressI p; p.num = num; p.value=0; p.ticks=0;
      p.value = val;
      progress_val.push_back(p);
      p.value = max;
      progress_max.push_back(p);
      progress_label.push_back(label);
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
      max_value += progress_max[i].value;
    }
  }
  //std::cout << "P3" << std::endl;

  //if (val_index==-1) std::cout << "ProgressError(val): num=" << num << std::endl;
  //if (max_index==-1) std::cout << "ProgressError(max): num=" << num << std::endl;

  //std::cout << "Progress2: " << num << " " << val << " " << label << " " << max << std::endl;
  //std::cout << "Progress3:" << val_value << " " << max_value << std::endl;

  
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

    g_val2 = val2;
    g_max2 = max2;
   
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
    
    

#ifdef EMSCRIPTEN
    if (g_progress_bar_show_logo) {
      g_progress_bar_logo_cb(g_progress_bar_logo_cb_data);
      //g_everyapi->mainloop_api.logo_iter();
    }
#endif
    
#ifndef EMSCRIPTEN
    //std::cout << stream.str() << l << stream3.str() << std::flush;
    if (g_prog_labels.size()>0)
      {
	g_prog_labels[g_prog_labels.size()-1] = stream.str() + l + stream3.str();
	if (g_everyapi2)
	  {
	    
	    update_progress_dialog_cb(g_progress_dialog, 400,200, g_atlas, g_atlas_bm, g_prog_labels,val1,max1);
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
  if (i==s-1) i++;
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

IMPORT bool file_exists(std::string filename)
{
  std::ifstream f(filename.c_str());
  return f.good();
}

std::string upgrade_to_https(std::string url)
{
  if (url.size()>7 && url.substr(0,7)=="http://") { return "https://" + url.substr(7); }
  return url;
}

#ifdef ANDROID
#include <stdio.h>
#include <curl/curl.h>
#include <string>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
  userp->append((char*)contents,size*nmemb);
  return size*nmemb;
}
size_t HeaderCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
  userp->append((char*)contents,size*nmemb);
  return size*nmemb;
}


void popen_curl_init() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

std::string popen_curl_replacement(std::string url, bool headeronly)
{
  //std::cout << "popen_curl_replacement 1" << std::endl;
  CURLcode res;
  std::string headers;
  std::string readBuffer;
  CURL *curl;
  curl = curl_easy_init();
  //std::cout << "popen_curl_replacement 2" << std::endl;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //std::cout << "popen_curl_replacement 3" << std::endl;
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    if (headeronly)
      {
	//std::cout << "popen_curl_replacement 4" << std::endl;
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
	//std::cout << "popen_curl_replacement 5" << std::endl;
      } else {
      //std::cout << "popen_curl_replacement 6" << std::endl;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	//std::cout << "popen_curl_replacement 7" << std::endl;
      }
    //std::cout << "popen_curl_replacement 8" << std::endl;
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    //std::cout << "popen_curl_replacement 9" << std::endl;

    res = curl_easy_perform(curl);
    //std::cout << "popen_curl_replacement 10" << std::endl;

    if (res != CURLE_OK) {
      std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
      //std::cout << "popen_curl_replacement 11" << std::endl;
      curl_easy_cleanup(curl);
      std::cout << headers << std::endl;
      if (headeronly) return headers;
      return readBuffer;
    }
    
  }
  return "";
}


#endif

long long load_size_from_url(std::string url)
{
  //std::cout << "POPEN SIZE" << url << std::endl;
  url = upgrade_to_https(url);
  if (url=="") return 1;
    std::vector<unsigned char, GameApiAllocator<unsigned char> > buffer;
    bool succ=false;
#ifndef ANDROID
#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s --max-time 300 -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    std::string cmd = "curl -s --max-time 300 -N --url " + url;
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
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
    unsigned char c2;
    while(fread(&c2,1,1,f2)==1) {
      vec2.push_back(c2);
    }
    pclose(f2);
    std::string s(vec2.begin(),vec2.end());
#endif
#ifdef ANDROID
    long long num = 1;
    std::string s = popen_curl_replacement(url,true); // headers only
#endif
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
#ifndef ANDROID
    }
#endif
    //std::cout << "POPEN1 END" << url << std::endl;
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
    //std::cout << "POPEN2 " << url << std::endl;
    
    url = upgrade_to_https(url);
    size = load_size_from_url(url);

    InstallProgress(333, "stream load..", 15);
    
    bool succ = false;
#ifdef HAS_POPEN

#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s --max-time 300 -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    //std::cout << "Fetching " << url << std::endl;
    std::string cmd = "curl -s --max-time 300 -N --url " + url;
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
    //std::cout << "POPEN2 END " << url << std::endl;
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
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec;
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
  virtual bool get_line(std::vector<unsigned char, GameApiAllocator<unsigned char> > &line)
  {
#ifdef HAS_POPEN
    if (f) {
      char *buf_line = NULL;
      size_t size2 = 0;
      ssize_t sz = getline(&buf_line,&size2, f);
      if (!buf_line) return false;
      if (sz==-1) return false;
      line.clear();
      line=std::vector<unsigned char, GameApiAllocator<unsigned char> >(buf_line,buf_line+sz);
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
  virtual bool get_file(std::vector<unsigned char, GameApiAllocator<unsigned char> > &file)
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
  LoadStream2(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec) : vec(vec) { }
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
  virtual bool get_line(std::vector<unsigned char, GameApiAllocator<unsigned char> > &line)
  {
    if (pos==vec.end()) return false;
    line.clear();
    char ch = 0;
    while(ch!='\n' && pos!=vec.end()) { ch=*pos; pos++; line.push_back(ch); }
    //std::cout << "LINE: " << std::string(line.begin(),line.end()) << std::endl;
    return true;
  }
  virtual bool get_file(std::vector<unsigned char, GameApiAllocator<unsigned char> > &file)
  {
    file = vec;
    return true;
  }
private:
  std::vector<unsigned char, GameApiAllocator<unsigned char> >::iterator pos;
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec;
};

LoadStream *load_from_vector(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec);

LoadStream *load_from_url_stream(std::string url)
{
#ifndef ANDROID
  LoadStream *stream = new LoadUrlStream(url);
    stream->Prepare();  
  return stream;
#else
  std::string s = popen_curl_replacement(url,false); // read whole file
  std::vector<unsigned char,GameApiAllocator<unsigned char> > vec(s.begin(),s.end());
  LoadStream *stream = load_from_vector(vec);
  return stream;
#endif
}
LoadStream *load_from_vector(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec)
{
  LoadStream *stream = new LoadStream2(vec);
  stream->Prepare();
  return stream;
}

extern std::string g_msg_string;

struct load_url_deleter
{
  std::vector<std::vector<unsigned char, GameApiAllocator<unsigned char> > *> item;
  ~load_url_deleter() {
    int s = item.size();
    for(int i=0;i<s;i++) delete item[i];
  }
};
load_url_deleter load_from_url_del;


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>




std::vector<unsigned char, GameApiAllocator<unsigned char> > *load_from_url(std::string url, bool nosize)
{ // works only in windows currently. Dunno about linux, and definitely doesnt wok in emscripten
  //std::cout << "POPEN3 " << url << std::endl;

  url = upgrade_to_https(url);

    if (url.size()==0) { std::vector<unsigned char, GameApiAllocator<unsigned char> > *b = new std::vector<unsigned char, GameApiAllocator<unsigned char> >(); /*load_from_url_del.item.push_back(b);*/ return b; }
  //std::cout << "load_from_url: @" << url << "@" << std::endl;

  int u = g_urls.size();
  for(int i=0;i<u;i++)
    {
      //std::cout << remove_load(url) << " " << g_urls[i] << std::endl;
      //std::cout << "LOADFROMURL:" << remove_load(url) << " " << g_urls[i] << std::endl;
      if (remove_load(url)==g_urls[i]) { 
	std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec = new std::vector<unsigned char, GameApiAllocator<unsigned char> >(g_content[i], g_content_end[i]);
	//load_from_url_del.item.push_back(vec);
	//std::cout << "load_from_url using memory: " << url << " " << vec.size() << std::endl;
	g_current_size+=vec->size();

	return vec;
      }
    }
  //  std::cout << "load_from_url using network: " << url << std::endl;
  std::vector<unsigned char, GameApiAllocator<unsigned char> > *buffer = new std::vector<unsigned char, GameApiAllocator<unsigned char> >;
  // THIS PUSH_BACK causes problems in other threads, because it accewsses
  // global variable that might not be initialized.
  //load_from_url_del.item.push_back(buffer);
    bool succ=false;
    
#ifdef HAS_POPEN
#ifndef ANDROID

    //std::cout << "NOT IN ANDROID" << std::endl;
    
#ifdef WINDOWS
    std::string cmd = "..\\curl\\curl.exe -s --max-time 300 -N --url " + url;
    std::string cmd2  = "..\\curl\\curl.exe";
    succ = file_exists(cmd2);
    std::string cmdsize = "..\\curl\\curl.exe -sI --url " + url;
#else
    //       std::cout << "Fetching " << url << std::endl;

    std::string cmd;
    if (nosize)
      {
	cmd = "stdbuf -oL curl --fail --silent --show-error --http2 -H \"X-Requested-With: XMLHttpRequest\" -H \"Accept: text/event-stream\" -N --url " + url;
      }
    else
      {
	cmd = "curl -N -s --max-time 300 --url " + url;
      }
    std::string cmdsize = "curl -sI --url " + url;
    succ = true;
#endif
    int num = 100000;
    if (succ) {
    if (!nosize) {

#ifdef __APPLE__
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else
#ifdef LINUX
    FILE *f2 = popen(cmdsize.c_str(), "r");
#else    
    FILE *f2 = popen(cmdsize.c_str(), "rb");
#endif
#endif
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
    unsigned char c2;
    while(fread(&c2,1,1,f2)==1) {
      
      vec2.push_back(c2);
    }
    pclose(f2);
    std::string s(vec2.begin(),vec2.end());
#else // ANDROID
    std::cout << "IS IN ANDROID" << std::endl;
    std::string s = popen_curl_replacement(url,true);
#endif
    //std::cout << "Headers:" << s << std::endl;
    std::stringstream ss(s);
    std::string line;
    while(std::getline(ss,line)) {
      //std::cout << "Line: " << line << std::endl;
      //if (line.size()>20)
      //	std::cout << "Substr: " << line.substr(0,20) << std::endl;
      if (line.size()>15 && line.substr(0,15)=="Content-Length:") {
	std::stringstream ss2(line);
	std::string dummy;
	ss2 >> dummy >> num;
	//std::cout << "Got num: " << num << std::endl;
      }
    }
    }
    //std::cout << "Content-Length: " << num << std::endl;
    
#ifndef ANDROID
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
      std::vector<unsigned char, GameApiAllocator<unsigned char> > *item = new std::vector<unsigned char, GameApiAllocator<unsigned char> >();
      //load_from_url_del.item.push_back(item);
      return item;
    }


    int fd = fileno(f);
    if (fd==-1)
      {
	std::cout << "fileno failed" << std::endl;
      }
    
    
    fd_set fds;
    struct timeval tv;

#ifdef LINUX
    /*
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags==-1) {
	std::cout << "FCNTL F_GETFL failed" << std::endl;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK)==-1)
      {
	std::cout << "FCNTL F_SETFL failed" << std::endl;
      }
    */
#endif
   
    //std::cout<< "FILE: " << std::hex<<(long)f <<std::endl; 
    unsigned char c;
    long long i = 0;
    if (num>0)
      buffer->reserve(num);
#ifdef LINUX

    int cnt=0;
    while(1) {
      FD_ZERO(&fds);
      FD_SET(fd,&fds);
      tv.tv_sec=0;
      tv.tv_usec=5000;

	cnt++;
      if (cnt>5000) {
	cnt=0;
	//g_low->sdl->SDL_PumpEvents();
	  //Low_SDL_Event event;
	  //while (g_low->sdl->SDL_PollEvent(&event)) {
	  //}
	timeout_getevent();
	if (!g_disable_draws) g_low->sdl->SDL_GL_SwapWindow(sdl_window);
      }
      //if (nosize) { std::cout << "while" << std::endl; }
      bool go=true;
      /*
      if (!nosize) {
	if (select(fd+1,&fds,NULL,NULL,&tv) > 0 && FD_ISSET(fd,&fds))
	  {
	    go=true;
	  }
      } else { go=true; }
      */

      if (go)
	{
	  //if (nosize) { std::cout << "select" << std::endl; }
	  
	  char buf[1024];
	  int bytes_read = read(fd,buf,1);
      if (bytes_read<0) {
	    if (errno==EAGAIN || errno==EWOULDBLOCK) { continue; }
	    else {
	      std::cout << "READ ERROR" << std::endl;
	    }
      } else
      if (bytes_read==0)
	{
	  break;
	}
      else {
	//	    if (nosize) { std::cout << "fread" << std::endl; }
      //std::cout << c;
      i+=bytes_read;
      g_current_size+=bytes_read;
      if (!g_progress_already_done && num/15>0 && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int j=0;j<s;j++) sum+=int(url[j]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      for(int ii=0;ii<bytes_read;ii++)
	buffer->push_back(buf[ii]);
      if (nosize) {
	for(int ii=0;ii<bytes_read;ii++)
	  std::cout << buf[ii];
	std::cout << std::flush;
	}
      }

	}
      /*
	} else
	    {
	      break;
	      }*/
    }
    
    pclose(f);
#endif
#ifdef WINDOWS
    while ( fread(&c,1,1,f)==1) {
      i++;
      g_current_size++;
      if (!g_progress_already_done && num/15>0 && i%(num/15)==0) {
	int s = url.size();
	int sum=0;
	for(int j=0;j<s;j++) sum+=int(url[j]);
	sum = sum % 1000;
	ProgressBar(sum,i*15/num,15,url);
      }
      buffer->push_back(c);
      if (nosize) std::cout << c;
    }
    
#endif

    
   }
#else // ANDROID
    std::string s = popen_curl_replacement(url,false);
    buffer = std::vector<unsigned char,GameApiAllocator<unsigned char> >(s.begin(),s.end());
#endif
    
    
    //fseek(f, 0, SEEK_END);
    //long pos = ftell(f);
    //fseek(f, 0, SEEK_SET);
    //buffer.resize(pos);
    //fread(&buffer[0], 1, pos, f);
    //std::cout << "::" << std::string(buffer.begin(),buffer.end()) << "::" << std::endl;
    if (buffer->size()==0)
      {
#ifndef ANDROID
#ifdef WINDOWS
    std::string cmd = ".\\curl\\curl.exe -s --max-time 300 -N --url " + url;
#else
    std::string cmd = "curl -s --max-time 300 -N --url " + url;
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
    //std::cout << "POPEN3 END " << url << std::endl;
    return buffer;
#else // ANDROID
    std::string s = popen_curl_replacement(url,false);
    buffer = std::vector<unsigned char,GameApiAllocator<unsigned char> >(s.begin(),s.end());
    }
    return buffer;
#endif
#else

#ifdef ANDROID
    std::string s = popen_curl_replacement(url,false);
    *buffer = std::vector<unsigned char,GameApiAllocator<unsigned char> >(s.begin(),s.end());
#endif

// no popen
#endif

    //  std::cout << "POPEN3b END " << url << std::endl;
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

std::vector<unsigned char> convert_to_hexdump(std::vector<unsigned char> s)
{
  int ss = s.size();
  std::vector<unsigned char> res;
  for(int i=0;i<ss;i++)
    {
      unsigned char ch = s[i];
      unsigned char ch0 = (ch&0xf0)>>4;
      unsigned char ch1 = (ch&0xf);
      const char *arr = "0123456789abcdef";
      res.push_back(arr[ch0]);
      res.push_back(arr[ch1]);
    }
  return res;
}

std::string add_boundaries(std::string s)
{
  return std::string("\n\n----kdjfkdjhfdkdkjf\nContent-Disposition: form-data; name=\"description\"\nContent-Type: text/plain\n\n") + s + std::string("\n----kdjfkdjhfdkdkjf--\n"); 
}


void send_grab_to_server(std::vector<unsigned char> data, int id, int num)
{
#ifdef EMSCRIPTEN

  int s = data.size();
  for(int i=0;i<s;i+=1024) {
    std::vector<unsigned char> data3 = std::vector<unsigned char>(data.begin()+i,data.begin()+std::min(i+1024,s));
  
  std::vector<unsigned char> data2 = convert_to_hexdump(data3);
  
  emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "PUT");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

    const char *headers[] = { "Content-Type", "undefined", 0};
    std::stringstream ss;
    ss << id << "|" << std::setfill('0') << std::setw(3) << num << "|" << i << "|" << std::min(s-1,i+1023) << "|" << s << std::endl;
    std::string body = ss.str();
    char *buf = new char[body.size()+data2.size()+1];
    std::copy(body.begin(), body.end(), buf);
    std::copy(data2.begin(), data2.end(), buf+body.size());

    //std::string s = std::string(buf,buf+body.size()+data2.size());
    //std::string s2 = add_boundaries(s);
    //char *buf2 = new char[s2.size()+1];
    //std::copy(s2.begin(),s2.end(),buf2);
    
    attr.requestData = buf;
    attr.requestDataSize = body.size()+data2.size();
    attr.requestHeaders = headers;
    emscripten_fetch(&attr, "save_grab.php");
    //delete [] buf;

  }
#endif
}

void remove_spaces(std::string &s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]==' ') s[i]='$';
    }
}

IMPORT void send_post_request(std::string url, std::string headers, std::string data)
{
  std::cout << "POST REQUEST: " << url << " with headers " << headers << " with data: " << data << std::endl;
#ifndef EMSCRIPTEN
#ifdef WINDOWS
  remove_spaces(data);
  remove_spaces(headers);
  std::string cmd = "..\\curl\\curl.exe -N -X POST --url " + url + " -d \"" + data + "\" -H \"" + headers + "\"";
#else
  std::string cmd = "curl -N -X POST --url " + url + " -d \"" + data + "\" -H \"" + headers + "\"";
#endif
  // std::cout << cmd << std::endl;
  

#ifdef __APPLE__
    FILE *f2 = popen(cmd.c_str(), "r");
#else
#ifdef LINUX
    FILE *f2 = popen(cmd.c_str(), "r");
#else    
    FILE *f2 = popen(cmd.c_str(), "rb");
#endif
#endif

    while (pclose(f2)<0);
#endif
    }


std::vector<std::string> g_download_bar_filename;
//std::vector<std::vector<unsigned char>*> g_download_bar_data;
std::vector<int> g_download_bar_index;
std::vector<float> g_download_bar_progress;
std::vector<bool> g_download_bar_ready;

void save_download(std::string filename, const std::vector<unsigned char> *vec)
{
#ifndef EMSCRIPTEN
#ifdef WINDOWS
  system("mkdir %TEMP%\\_gameapi_builder");
  system("mkdir %TEMP%\\_gameapi_builder\\Downloads");
  std::string home = getenv("TEMP");
  std::string filename_with_path = home + std::string("\\_gameapi_builder\\Downloads\\") + filename;
  std::ofstream ss(filename_with_path.c_str());
  std::string val(vec->begin(),vec->end());
  ss << val;
  ss << std::flush;
  ss.close();
#endif
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  if (dockerdir=="") {
    system("mkdir -p ~/.gameapi_builder");
    system("mkdir -p ~/.gameapi_builder/Downloads");
  } else {
    system(("mkdir -p " + dockerdir + ".gameapi_builder").c_str());
    system(("mkdir -p " + dockerdir + ".gameapi_builder/Downloads").c_str());
  }
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  std::string filename_with_path = home + std::string(".gameapi_builder/Downloads/") + filename;
  std::cout << "Saving to " << filename_with_path << std::endl;
  std::ofstream ss(filename_with_path.c_str());
  std::string val(vec->begin(),vec->end());
  ss << val;
  ss << std::flush;
  ss.close();
#endif
#endif
}

int EnvImpl::download_index_mapping(int index)
{
  int s = g_download_bar_index.size();
  for(int i=0;i<s;i++)
    {
      if (g_download_bar_index[i]==index) return i;
    }
  return -1;
}

int EnvImpl::add_to_download_bar(std::string filename)
{
  std::cout << "Saving to " << filename << std::endl;
  static int index = 0;
  index++;
  g_download_bar_index.push_back(index);
  g_download_bar_ready.push_back(false);
  g_download_bar_progress.push_back(0.0);
  g_download_bar_filename.push_back(filename);
  //g_download_bar_data.push_back(new std::vector<unsigned char>(file));

  return index;
}
void EnvImpl::set_download_data(int i, const std::vector<unsigned char> &file)
{
  save_download(g_download_bar_filename[i],&file);
}
int EnvImpl::download_bar_count() const
{
  return g_download_bar_filename.size();
}
//std::vector<unsigned char> *EnvImpl::get_download_bar_item(int i) const
//{
//  return 0;
  //return g_download_bar_data[i];
//}
std::string EnvImpl::get_download_bar_filename(int i) const
{
  return g_download_bar_filename[i];
}
void EnvImpl::remove_download_bar_item(int i)
{
  //delete g_download_bar_data[i];
  g_download_bar_filename.erase(g_download_bar_filename.begin()+i);
  //g_download_bar_data.erase(g_download_bar_data.begin()+i);
  g_download_bar_index.erase(g_download_bar_index.begin()+i);
  g_download_bar_ready.erase(g_download_bar_ready.begin()+i);
  g_download_bar_progress.erase(g_download_bar_progress.begin()+i);
}
void EnvImpl::set_download_progress(int i, float percentage)
{
  g_download_bar_progress[i] = percentage;
}
void EnvImpl::set_download_ready(int i)
{
  g_download_bar_ready[i]=true;
}
float EnvImpl::get_download_bar_progress(int i) const
{
  return g_download_bar_progress[i];
}
bool EnvImpl::get_download_bar_ready(int i) const
{
  return g_download_bar_ready[i];
}
struct TaskData
{
  ASyncTask *task;
  int pos;
  bool finished;
};

std::vector<ASyncTask*> tasks;
std::vector<int> task_location;
std::vector<TaskData*> task_data;
std::vector<int> index_map;
std::vector<Low_SDL_GLContext> context_map;
std::vector<bool> valid_context;

IMPORT extern Low_SDL_Window *sdl_window;
//extern Low_SDL_GLContext g_context;
void *async_process(void *ptr)
{
  /*
  TaskData *dt = (TaskData*)ptr;
  if (valid_context[dt->pos])
    g_low->sdl->SDL_GL_DeleteContext(context_map[dt->pos]);
  context_map[dt->pos] = g_low->sdl->SDL_GL_CreateContext(sdl_window);
  g_low->sdl->SDL_GL_MakeCurrent(sdl_window, context_map[dt->pos]);
  valid_context[dt->pos]=true;
  dt->task->DoTask(dt->pos);
  g_low->sdl->SDL_GL_MakeCurrent(sdl_window, g_context);
  
  dt->finished = true;
  */
}

void start_task(int task, int pos)
{
  tasks[task]->DoTask(pos);
  /*
  ASyncTask *current_task = tasks[task];
  TaskData *ptr = task_data[task]; //new TaskData;
  ptr->task = current_task;
  ptr->pos = pos;
  ptr->finished = false;
  pthread_t thread_id;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 3000);
  pthread_create(&thread_id, &attr, &async_process, (void*)ptr);
  */
}
bool task_finished(int task, int pos)
{
  if (task_location[task]==-1) return true;
  return true;
  //return task_data[task]->finished;
}

int EnvImpl::start_async(ASyncTask *task)
{
  static int index = 0;
  index++;
  index_map.push_back(index);
  tasks.push_back(task);
  task_location.push_back(-1);
  task_data.push_back(new TaskData);
  Low_SDL_GLContext ctx;
  context_map.push_back(ctx);
  valid_context.push_back(false);
  return index;
}
int EnvImpl::async_mapping(int index)
{
  int s = index_map.size();
  for(int i=0;i<s;i++)
    {
      if (index_map[i]==index) return i;
    }
  return -1;
}
void EnvImpl::remove_async(int i)
{
  index_map.erase(index_map.begin()+i);
  tasks.erase(tasks.begin()+i);
  task_location.erase(task_location.begin()+i);
  delete task_data[i];
  task_data.erase(task_data.begin()+i);
  context_map.erase(context_map.begin()+i);
}
void EnvImpl::async_scheduler()
{
  int s = task_location.size();
  for(int i=0;i<s;i++)
    {
      if (!task_finished(i,task_location[i])) continue; // still running
      if (tasks[i]->NumTasks()>task_location[i])
	{
	  task_location[i]++;
	  start_task(i,task_location[i]);
	}
    }
}

