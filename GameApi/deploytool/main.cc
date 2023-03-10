
#include <iostream>
#include <fstream>

void usage() {
  std::cout << "./deploytool --file input_filename.txt -o output_filename.txt" << std::endl;
  exit(0);
}

int main(int argc, char *argv[])
{
  std::string input_filename="@";
  std::string output_filename="@";
  for(int i=1;i<argc;i++)
    {
      if (std::string(argv[i])==std::string("--file")) { input_filename = argv[i+1]; i++; }
      if (std::string(argv[i])==std::string("-o")) { output_filename = argv[i+1]; i++; }
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
}
