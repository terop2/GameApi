#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


bool find_str(std::string contents, std::string label)
{
  int s = contents.size();
  int s2 = label.size();
  for(int i=0;i<s-s2+1;i++)
    {
      int j = 0;
      for(;j<s2;j++)
	{
	  if (contents[i+j]!=label[j]) break;
	}
      if (j==s2) { return true; }
    }
  return false;
}


std::string find_find(std::string content, std::string addname, bool is_find)
{
  if (is_find) {
    addname = "*" + addname + "(";
  } else {
    addname = " " + addname + "(";
  }
  std::stringstream ss(content);
  std::string line;
  int keep = 0;
  std::string result;
  while(std::getline(ss,line))
    {
      bool res = find_str(line, addname);
      if (res) { keep++;
	bool res3 = find_str(line,"return");
	if (res3) keep--; else {
	  bool res4 = find_str(line,"=");
	  if (res4) keep--; else {
	  bool res5 = find_str(line,"A<T>");
	  if (res5) keep--; else {
	  bool res6 = find_str(line,"push_back");
	  if (res6) keep--; else {
	  bool res7 = find_str(line,";");
	  if (res7) keep--;
	  } } } }
	}

      if (keep>0) { result+=line+"\n"; }
      bool res2 = find_str(line,"}");
      if (res2) keep--;
    }
  return result;
}

int main(int argc, char *argv[])
{
  if (argc!=3) {std::cout << "./addfindparser.exe find_name add_name" << std::endl; }
  std::ifstream ss("..\\GameApi.cc");
  std::string line;
  std::string contents;
  while(std::getline(ss,line)) {
    contents+=line+"\n";
  }
  std::string find = find_find(contents,argv[1],true);
  std::string add = find_find(contents,argv[2],false);
  std::cout << find << "\n" << add << "\n";
}
