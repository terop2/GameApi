
#include "GameApi.hh"
#include <vector>
#include <utility>

using namespace GameApi;

int main()
{
  Env e;
  EveryApi ev(e);
  SH sh;
  sh.id = -1;
  GuiApi g(e,ev,sh);
  std::vector<std::pair<std::string, std::string>> vec = g.get_functions_mapping();
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      std::cout << vec[i].first << " " << vec[i].second << std::endl;
    }
}
