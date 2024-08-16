
#include <cstdio>
#include <sstream>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>

std::string cut(std::string s)
{
  std::string res;
  int sz = s.size();
  for(int i=0;i<sz;i++)
    {
      if (s[i]!=' ') res+=s[i];
    }
  return res;
}

int main(int argc, char **argv)
{
  //std::cout << "argc=" << argc << std::endl;
  bool menu=false;
  bool menu2 = false;
  if (argc<2) { std::cout << "./dump_docs 0" << std::endl;
    std::cout << "./dump_docs 0 0" << std::endl; 
    std::cout << "./dump_docs 0 0 select" << std::endl;
    exit(0);
  }
  
  int count=0;
  int count2=0;
  bool select=false;
  if (argc>=2) { std::string num(argv[1]); menu=true; std::stringstream ss(num); ss >> count;   
}
  if (argc>=3) {
    std::string num2(argv[2]);
    menu2 = true;
    std::stringstream ss(num2);
    ss>>count2;
  }
  if (argc>=4)
    {
      std::string sel(argv[3]);
      if (sel=="select") { select=true; }
    }
    
  std::ifstream ss("./dump.txt");
  std::string line;
  int menu_line=0;
  int menu_line2=0;
  while(std::getline(ss,line)) {
    if (line.size()>2 && line[0]=='/' && line[1]=='/')
      {
	std::string label = line.substr(3);
	if (menu && !menu2 && !select) {
	  if (menu_line==count) {
	    std::cout << "<b>";
	  }
	  std::stringstream ss3;
	  ss3 << menu_line << "," << count2;
	  std::cout << "<a href=\"#\" onclick=\"menu_click(" + ss3.str() + ")\">";
	  std::cout << label;
	  std::cout << "</a>";
	  if (menu_line==count) {
	    std::cout << "</b>";
	  }
	  std::cout << std::endl;
	}
	menu_line++;
	menu_line2=0;
      } else {
      std::stringstream ss(line);
      std::string name;
      std::string api;
      std::string func;
      ss >> name >> api >> func;
      if (menu2 && count==menu_line-1 && !select) {
	if (menu_line2 == count2) { std::cout << "<b>"; }
	  std::stringstream ss3;
	  ss3 << menu_line-1 << "," << menu_line2;
	  std::cout << "<a href=\"#\" onclick=\"menu_click2(" + ss3.str() + ")\">";
	std::cout << name;
	  std::cout << "</a>";
	if (menu_line2 == count2) { std::cout << "</b>"; }
	std::cout << std::endl;
      }
      if (menu_line-1==count && menu_line2 == count2 && select)
	{
	  std::string cmd = "./parser " + cut(api) + " " + cut(func);
	  char *ptr = new char[cmd.size()+1];
	  std::copy(cmd.begin(),cmd.end(),ptr);
	  ptr[cmd.size()]=0;
	  FILE *f = popen(ptr,"r");
	  int c;
	  while((c=fgetc(f))!=EOF) {
	    if (c=='<') std::cout << "&lt;"; else
	    if (c=='>') std::cout << "&gt;"; else
	    std::cout << (char)c;
	  }
	  pclose(f);
	}
      
      menu_line2++;
    } 
  }
  std::string random;
  std::stringstream r;
  srand(time(0));
  r << rand();
  std::string script = 
    "<script>\n"
    "function menu_click(num,num2) {\n"
    "    window.location.href=\"https://meshpage.org/mesh_doc.php?menu=\" + num.toString() + \"&submenu=\" + num2.toString() + \"&select=select&" + r.str() +"\" ;"
    "}\n"
    "function menu_click2(num,num2) {\n"
    "    window.location.href=\"https://meshpage.org/mesh_doc.php?menu=\" + num.toString() + \"&submenu=\" + num2.toString()+\"&select=select&" + r.str() +"\";\n"
    "}\n"
    "</script>"
    ;
  std::cout << script << std::endl;
}
