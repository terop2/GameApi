
#include <thread>
#include "GameApi.hh"

#include "gen_code.cpp"

const char g_url[] = "http://tpgames.org/";
const char g_url2[] = "http://tpgames.org/aaa";
const unsigned char g_ptr[] = "";
const unsigned char g_ptr2[] = "";
const unsigned char *g_arr[] = { g_ptr, g_ptr2 };
const char *g_url[] = { g_url, g_url2 };
const int g_arr_sizes[] = { sizeof(g_ptr)/sizeof(unsigned char),
			    sizeof(g_ptr2)/sizeof(unsigned char) };
const int g_arr_size = sizeof(arr)/sizeof(*arr);

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

std::string code2;

void gameapi_main()
{
  // TODO, where to get code2 from?
  code = insert_enter(strip_spaces(code2));

  Env e;
  EveryApi ev(e);

  int s = g_arr_size;
  for(int i=0;i<s;i++)
    {
      const unsigned char *ptr = g_arr[i];
      const int size = g_arr_sizes[i];
      const char *url = g_url[i];
      append_url_map(url, ptr, ptr+size);
    }
  
  std::pair<int,std::string> blk = mainloop(e,ev);

  RUN r;
  r.id = blk.first;
  ev.blocker_api.run2(ev,r);

}

void gameapi_init()
{
  // TODO how to set stack size
  new std::thread(gameapi_main);
}

int main()
{
  gameapi_init();
  while(1);
}
