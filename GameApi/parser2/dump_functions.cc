
#include "GameApi_h.hh"

int main(int argc, char **argv)
{
  if (argc==0) { std::cout << "USAGE: ./dump_functions 1" << std::endl; exit(0); }
  std::string s(argv[1]);
  std::stringstream ss(s);
  int value;
  ss>>value;


  GameApi::Env env;
  GameApi::EveryApi ev(env);
  ev.wmod_api.dump_functions_for_docs(ev,value);
}
