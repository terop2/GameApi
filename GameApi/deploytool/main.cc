
#include <iostream>
#include <fstream>

#include "GameApi.hh"

using namespace GameApi;

void usage() {
  std::cout << "./deploytool --file input_filename.txt" << std::endl;
  std::cout << "             -o output_filename.zip" << std::endl;
  std::cout << "             --homepage url" << std::endl;
  exit(0);
}
void start_async_deploy(GameApi::Env &e, std::string script, std::string output_filename, std::string homepage);

int main(int argc, char *argv[])
{
  std::string input_filename="@";
  std::string output_filename="@";
  std::string homepage="@";
  for(int i=1;i<argc;i++)
    {
      if (std::string(argv[i])==std::string("--file")) { input_filename = argv[i+1]; i++; }
      if (std::string(argv[i])==std::string("-o")) { output_filename = argv[i+1]; i++; }
      if (std::string(argv[i])==std::string("--homepage")) { homepage = argv[i+1]; i++; }
    }
  if (input_filename=="@") usage();
  if (output_filename=="@") usage();


  std::string script;
  std::ifstream ss(input_filename.c_str());
  char ch;
  while(ss.get(ch)) { script+=ch; }
  ss.close();

  std::cout << script << std::endl;
  std::cout << "->" << std::endl;
  std::cout << output_filename << std::endl;
  std::cout << " with " << std::endl;
  std::cout << homepage << std::endl;

  Env *e2 = new Env;
  Env &e = *e2;
  EveryApi ev(*e2);

  start_async_deploy(e,script,output_filename, homepage);
  for(int i=0;i<100;i++)
    e.async_scheduler();
  std::cout << "EXITING.." << std::endl;
}
