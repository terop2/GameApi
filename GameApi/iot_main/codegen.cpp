

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
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

int find_last_char(std::string s, char c)
{
  int ss = s.size();
  int pos = -1;
  for(int i=0;i<ss;i++)
    {
      if (s[i]==c) pos=i;
    }
  return pos;
}

bool is_mtl(std::string url)
{
  if (url.size()<3) return false;
  if (url[url.size()-1]=='l' &&
      url[url.size()-2]=='t' &&
      url[url.size()-3]=='m' &&
      url[url.size()-4]=='.') { return true; }
  return false;
}
std::vector<int> find_jpg(std::string s)
{
  int ss = s.size();
  std::vector<int> vec;
  for(int i=0;i<ss-3;i++)
    {
      if (s[i]=='.' && s[i+1]=='j' && s[i+2]=='p' && s[i+3]=='g') vec.push_back(i);
    }
  return vec;
}
std::vector<int> find_png(std::string s)
{
  int ss = s.size();
  std::vector<int> vec;
  for(int i=0;i<ss-3;i++)
    {
      if (s[i]=='.' && s[i+1]=='p' && s[i+2]=='n' && s[i+3]=='g') vec.push_back(i);
    }
  return vec;
}

int go_backwards(std::string s, int pos)
{
  std::string chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+?=%#£:._\\";
  int ss = chars.size();
  for(;pos>=0;pos--)
    {
      int found = false;
      for(int i=0;i<ss;i++) {
	if (s[pos]==chars[i]) found=true;
      }
      if (!found) return pos+1;
    }
  return 0;
}
void replace_char(std::string &s, char c1, char c2)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]==c1) s[i]=c2;
    }
}


std::vector<std::string> find_mtl_urls(std::string mtlfile, std::string mtlurl)
{
  int p = find_last_char(mtlurl, '/');
  std::string prefix = mtlurl.substr(0,p);
  std::vector<std::string> urls;

  std::vector<int> jpg_pos = find_jpg(mtlfile);
  std::vector<int> png_pos = find_png(mtlfile);
  int sj = jpg_pos.size();
  for(int i=0;i<sj;i++)
    {
      int pos = jpg_pos[i];
      int start = go_backwards(mtlfile,pos);
      int end = pos+4;
      std::string url_end = mtlfile.substr(start, end-start);
      std::string url = prefix+"/"+url_end;
      replace_char(url,'\\','/');
      std::cout << "mtl url: " << url << std::endl;
      urls.push_back(url);
    }

  int sp = png_pos.size();
  for(int i=0;i<sp;i++)
    {
      int pos = png_pos[i];
      int start = go_backwards(mtlfile,pos);
      int end = pos+4;
      std::string url_end = mtlfile.substr(start, end-start);
      std::string url = prefix+"/"+url_end;
      replace_char(url,'\\','/');
      std::cout << "mtl url: " << url << std::endl;
      urls.push_back(url);
    }
  return urls;
}

std::vector<std::string> find_urls(std::string s)
{
  std::vector<std::string> vec;
  int ss = s.size();
  for(int i=0;i<ss-3;i++)
    {
      if (s[i]=='h' && s[i+1]=='t' && s[i+2]=='t' && s[i+3]=='p')
	{
	  std::string chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+?=%#£:._";
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

bool is_present(std::vector<std::string> vec, std::string ss)
{
  int s=vec.size();
  for(int i=0;i<s;i++)
    {
      if (vec[i]==ss) return true;
    }
  return false;
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
  std::sort(urls.begin(), urls.end());
  std::unique(urls.begin(), urls.end());

  int s = urls.size();
  for(int i=0;i<s;i++)
    {
      std::cout << "Loading " << urls[i] << " ";
      std::string n = network(urls[i]);
      bool b = is_deep_file(urls[i]);
      if (b) {
	std::vector<std::string> u = find_urls(n);
	int ss = u.size();
	for(int j=0;j<ss;j++) { if (!is_present(urls,u[j])) { urls.push_back(u[j]); s++;} }
      }
      bool b2 = is_mtl(urls[i]);
      if (b2) {
	std::vector<std::string> u = find_mtl_urls(n, urls[i]);
	int ss = u.size();
	for(int j=0;j<ss;j++) { if (!is_present(urls,u[j])) {urls.push_back(u[j]); s++;} }	
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
