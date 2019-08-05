
#include <thread>
#include <sstream>
#include "GameApi.hh"

using namespace GameApi;

#include "gen_code.cpp"

#ifdef LINUX
void check_vr_compositor_init() { }
#endif

#ifdef RASPI

__asm__(".symver __fdelt_chk,__fdelt_chk@GLIBC_2.15");
extern "C" unsigned long int __fdelt_chk(unsigned long int d) { return d; }

__asm__(".symver clock_gettime,clock_gettime@GLIBC_2.17");
extern "C"
int clock_gettime(clockid_t, timespec*) { return 0; }

#endif

#if 0
const char g_url[] = "http://tpgames.org/";
const char g_url2[] = "http://tpgames.org/aaa";
const unsigned char g_ptr[] = "";
const unsigned char g_ptr2[] = "";
const unsigned char *g_arr[] = { g_ptr, g_ptr2 };
const char *g_url[] = { g_url, g_url2 };
const int g_arr_sizes[] = { sizeof(g_ptr)/sizeof(unsigned char),
			    sizeof(g_ptr2)/sizeof(unsigned char) };
const int g_arr_size = sizeof(arr)/sizeof(*arr);
#endif

std::string code;

std::pair<int,std::string> mainloop(Env &e2, EveryApi &ev)
{
  ExecuteEnv e;
  std::pair<int,std::string> p = GameApi::execute_codegen(e2,ev, code, e);
  return p;
}

std::string insert_enter(std::string s)
{
  if (s.length()>0)
    {
      char c = s[s.size()-1];
      if (c!='\n') s+='\n';
    }
  return s;
}

std::string strip_spaces(std::string data)
{
  std::string res;
  int s = data.size();
  for(int i=0;i<s-1;i++)
    {
      char c1 = data[i];
      char c2 = data[i+1];
      if (c1==' ' && c2==' ') { }
      else { res+=c1; }
    }
  char c3 = data[s-1];
  res+=c3;
  while(res[0]==' ') res = res.substr(1);
  while(res[res.size()-1]==' ') res = res.substr(0,res.size()-1);
  return res;
}
std::string strip_empty_lines(std::string data)
{
  std::string res;
  std::stringstream ss(data);
  std::string line;
  while(std::getline(ss,line))
    {
      const char chars[] = "01234567890abcdefghijklmnopqrstuvwxyzI";
      bool has_chars = false;
      int s = line.size();
      for(int i=0;i<s;i++)
	{
	  char c = line[i];
	  int sk = sizeof(chars);
	  for(int j=0;j<sk;j++)
	    {
	      if (c==chars[j]) {has_chars = true; break; } 
	    }
	}
      if (has_chars) { res+=line; res+='\n'; }
    }
  return res;
}


void gameapi_main()
{
  // TODO, where to get code2 from?
  code = insert_enter(strip_empty_lines(strip_spaces(std::string(g_arr_tst[0],g_arr_tst[0]+g_arr_sizes_tst[0]))));

  std::cout << code << std::endl;

  Env e;
  EveryApi ev(e);

  int s = g_arr_size_tst;
  for(int i=0;i<s;i++)
    {
      const unsigned char *ptr = g_arr_tst[i];
      const int size = g_arr_sizes_tst[i];
      const char *url = g_url_tst[i];
      append_url_map(url, ptr, ptr+size);
    }
  

#ifdef WINDOWS
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800*1.8; // ;1920;
  int w_height = 600*1.8; // 1080;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif
#ifdef LINUX
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800;
  int w_height = 600;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif
  std::pair<int,std::string> blk = mainloop(e,ev);
  if (blk.second=="RUN") {
  RUN r;
  r.id = blk.first;
  ev.blocker_api.run2(ev,r);

  } else if (blk.second=="OK") {
    BLK b;
    b.id = blk.first;
    ev.blocker_api.run(b);

  } else 
    {
      std::cout << "ERROR: internal error: " << blk.second << std::endl;
    }


}

void gameapi_init()
{
  // TODO how to set stack size
  new std::thread(gameapi_main);
}

int main()
{
  gameapi_main();
  //  gameapi_init();
  //  while(1);
}
