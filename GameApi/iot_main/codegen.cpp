
#include <cstdio>


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
  return std::string(buffer.begin(),buffer.end());
}

void error(std::string s)
{
  std::cout << "ERROR: " << s << std::endl;
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
	  std::string chars="abcdefghijklmnopqrstuvwxyz0123456789/+?=%#£";
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
	  std::string url = s.substr(i,end-i-1);
	  vec.push_back(url);
	}
    }
  return vec;

}


void output_file(std::string name, std::string file)
{
  std::string s = "std::string " << name << " = \n";
  int ss = file.size();
}

int main(int argc, char *argv[])
{
  if (argv[1]!="--url") error("Should use --url parameter");
  if (argc!=2) error("Number of parameters(argc) should be 2");
  std::string url = argv[2];
  std::string file = network(url);
  std::vector<std::string> urls = find_urls(file);
  int s = urls.size();
  std::vector<std::string> files;
  for(int i=0;i<s;i++)
    {
      files.push_back(network(urls[i]));
    }
  
}
