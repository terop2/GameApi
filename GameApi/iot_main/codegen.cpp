

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

bool find(std::string s, std::string s2)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      int sj = s2.size();
      bool found = true;
      for(int j=0;j<sj;j++)
	{
	  if (s[i+j]!=s2[j]) found=false;
	}
      if (found) { return true; }
    }
  return false;
}

void error(std::string s);
std::string network(std::string url)
{
#ifdef WINDOWS
  std::string cmd = "..\\curl\\curl.exe -s -N --url " + url;
#else
  std::string cmd = "curl -s -N --url " + url;
#endif
#ifdef __APPLE__
  FILE *f = popen(cmd.c_str(), "r");
#else
  FILE *f = popen(cmd.c_str(),"rb");
#endif

  unsigned char c;
  std::vector<unsigned char> buffer;
  while(fread(&c,1,1,f)==1) { buffer.push_back(c); }
  bool is404 = find(std::string(buffer.begin(), buffer.end()), "404 Not Found");
  if (is404) error(std::string("404 not found: " + url));
  std::cout << buffer.size() << std::endl;
  if (buffer.size()==0) { error(std::string("url not found: ") + url); }
  return std::string(buffer.begin(),buffer.end());
}

void error(std::string s)
{
  std::cout << "ERROR: " << s << std::endl;
  std::cout << "Usage: --url <url> --prefix <prefix>" << std::endl;
  exit(0);
}

std::vector<std::string> find_urls(std::string s)
{
  std::vector<std::string> vec;
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='h' && s[i+1]=='t' && s[i+2]=='t' && s[i+3]=='p')
	{
	  std::string chars="abcdefghijklmnopqrstuvwxyz0123456789/+?=%#£:._";
	  int end = i;
	  for(int j=i;j<ss;j++)
	    {
	      int sk = chars.size();
	      bool found = false;
	      for(int k=0;k<sk;k++)
		{
		  if (s[j]==chars[k]) { found=true; break; }
 		}
	      if (!found) { end=j; break; }
	    }
	  std::string url = s.substr(i,end-i);
	  vec.push_back(url);
	}
    }
  return vec;

}

std::string s_table[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };

std::string stringify(std::string s)
{
  int ss = s.size();
  std::string res;
  res+="\"";
  for(int i=0;i<ss;i++)
    {
      unsigned char c = s[i];
      unsigned char c2 = c&0xf;
      unsigned char c3 = (c&0xf0)>>4;
      res+="\\x";
      res+=s_table[c3];
      res+=s_table[c2];
      if (i%19==0) res+="\"\n\"";
    }
  res+="\"";
  return res;
}

void output_file(std::string name, std::string file)
{
  std::string s = "std::string " + name + " = \n";
  int ss = file.size();
}

std::string replace_string(std::string s, std::string rep, std::string subst)
{
  int val = s.find(rep);
  s.replace(s.begin()+val, s.begin()+val+rep.size(),subst);

  return s;
}

bool is_deep_file(std::string url)
{
  bool b1 = find(url, ".mp");
  return b1;
}

int main(int argc, char *argv[])
{
  std::string url = "";
  std::string prefix = "tst";
  if (argc<2) { error("Number of parameters"); }
  for(int i=1;i<argc;i++) {
    if (std::string(argv[i])=="--url") {
      url = argv[i+1];
      url = replace_string(url, "mesh.php", "mesh_pre.php");
      i++;
    } else
      if (std::string(argv[i])=="--prefix") {
      prefix = argv[i+1];
      i++;
    } else
      {
	error("Invalid parameter " + std::string(argv[i]));
      }
  }
  std::cout << "Loading " << url << " ";
  std::vector<std::string> files;
  std::string file = network(url);
  files.push_back(file);
  std::vector<std::string> urls = find_urls(file);
  int s = urls.size();
  for(int i=0;i<s;i++)
    {
      std::cout << "Loading " << urls[i] << " ";
      std::string n = network(urls[i]);
      bool b = is_deep_file(urls[i]);
      if (b) {
	std::vector<std::string> u = find_urls(n);
	int ss = u.size();
	for(int j=0;j<ss;j++) { urls.push_back(u[j]); s++; }
      }
      files.push_back(n);
    }
  urls.insert(urls.begin(), &url, &url + 1);

  std::ofstream out("gen_code.cpp");
  int sf = files.size();
  for(int i=0;i<sf;i++)
    {
      std::stringstream ss;
      ss << i;
      out << "unsigned char c_" + prefix + "_" + ss.str() + "[] = " << std::endl;
      out << stringify(files[i]);
      out << ";" << std::endl;
      
      out << "const char g_" + prefix + "_url_" + ss.str() + "[] = \"";
      out << urls[i];
      out << "\";" << std::endl;
    }
  out << "const unsigned char *g_arr_" << prefix << "[] = {";
  for(int i=0;i<sf;i++)
    {
      std::stringstream ss;
      ss << i;
      out << "c_" + prefix + "_" + ss.str();
      if (i!=sf-1) out << ",";
    }
  out << " };" << std::endl;
  out << "const char *g_url_" << prefix << "[] = { ";
  for(int i=0;i<sf;i++)
    {
      std::stringstream ss;
      ss << i;
      out << "g_" + prefix + "_url_" + ss.str();
      if (i!=sf-1) out<<",";
    }
  out << "};" << std::endl;
  out << "const int g_arr_sizes_" << prefix << "[] = { ";
  for(int i=0;i<sf;i++)
    {
      std::stringstream ss;
      ss << i;
      out << "sizeof(c_" << prefix << "_" << ss.str() << ")/sizeof(unsigned char)";
      if (i!=sf-1) out << ",";
    }
  out << "}; "<< std::endl;
  out << "const int g_arr_size_" << prefix << " = sizeof(g_arr_" << prefix << ")/sizeof(*g_arr_" << prefix << ");" << std::endl;
  out.close();

}
