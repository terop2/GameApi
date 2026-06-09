
#include "GameApi.hh"
#include "Tasks.hh"
#define CPPHTTPLIB_NO_EXCEPTIONS
#include "httplib.h"
#include "GraphI.hh"

extern std::string g_http_server_ip;
extern int g_http_server_port;


void *http_server_process(void *);
IMPORT void send_post_request(std::string url, std::string headers, std::string data);
std::string remove_str_after_char(std::string s, char ch);
std::string deploy_truncate(std::string s);
std::string http_to_https(std::string url);
std::string deploy_curl_cmd();
IMPORT std::string remove_prefix(std::string url);
std::string find_directory(std::string url);
IMPORT extern GameApi::PAT gameapi_temp_dir;
std::string replace_deploy_url(std::string home);
IMPORT int find_str(std::string val, std::string repl);
std::string deploy_replace_string(std::string val, std::string repl, std::string subst);
std::string convert_spaces_to_url_encoding(std::string url);
std::string take_prefix(std::string cd, std::string path);
IMPORT std::string replace_str(std::string val, std::string repl, std::string subst);


extern std::string g_mod_path;
struct UrlItem
{
  int index;
  std::string url;
  std::string url_orig;
  bool is_license=false;
  std::string licensed_filename;
  std::string author;
  std::string license_contents;
};
std::vector<UrlItem> find_url_items(GameApi::Env &e, std::string s);
void find_url_items2(GameApi::Env &e, std::string s, std::vector<UrlItem> &result);
void find_url_items3(GameApi::Env &e, std::vector<UrlItem> &result);



struct HTTP_files
{
  std::vector<std::string> filenames;
  std::vector<std::string> contents;
};

void start_http_listening(std::vector<std::string> filenames,
			  std::vector<std::string> contents)
{
  HTTP_files *files = new HTTP_files;
  files->filenames = filenames;
  files->contents = contents;
  tasks_add(9898, &http_server_process, (void*)files);
}
void join_http()
{
  tasks_join(9898);
}
void send_http_server_shutdown()
{
  std::cout << "Note: shutting down http server at " << g_http_server_ip << ":" << g_http_server_port << std::endl;
  std::stringstream ss;
  ss << g_http_server_port;
  std::string url = "http://" + g_http_server_ip + ":" + ss.str() + "/stop";
  std::string headers = "";
  std::string data = "";
  send_post_request(url,headers,data);
}

bool choose_http_port()
{
  httplib::Server svr;
  int counter=0;
  bool error = false;
  while (!svr.bind_to_port(g_http_server_ip,g_http_server_port))
    {
      counter++;
      g_http_server_port++;
      if (counter>40) { error=true; break; }
    }
  if (error) {
    std::cout << "ERROR: couldn't find local ip port: " << g_http_server_ip << ": 50000-" << g_http_server_port << " -> failing" << std::endl;
  } else {
    std::cout << "NOTE: setting up http server at: " << g_http_server_ip << ":" << g_http_server_port << std::endl;
  }
  return !error;
}

std::string choose_type(std::string filename)
{
  if (filename.substr(filename.size()-3,3)=="txt")
    {
      return "text/plain";
    }
  if (filename.substr(filename.size()-4,4)=="gltf")
    {
      return "model/gltf+json";
    }
  if (filename.substr(filename.size()-3,3)=="glb")
    {
      return "model/gltf-binary";
    }
  if (filename.substr(filename.size()-3,3)=="png")
    {
      return "image/png";
    }
  if (filename.substr(filename.size()-3,3)=="jpg")
    {
      return "image/jpeg";
    }
  if (filename.substr(filename.size()-4,4)=="jpeg")
    {
      return "image/jpeg";
    }
  if (filename.substr(filename.size()-3,3)=="mp4")
    {
      return "video/mp4";
    }
  if (filename.substr(filename.size()-3,3)=="ogg")
    {
      return "audio/ogg";
    }
    if (filename.substr(filename.size()-3,3)=="mp3")
    {
      return "audio/mpeg";
    }
  if (filename.substr(filename.size()-3,3)=="zip")
    {
      return "application/zip";
    }
  return "text/plain";
}


httplib::Server *g_http_server = 0;

void http_server(std::vector<std::string> filenames,
		 std::vector<std::string> contents)
{
  httplib::Server svr;
  g_http_server = &svr;
  
  int s = std::min(filenames.size(),contents.size());

  svr.Post("/stop", [&](const httplib::Request &req,
		      httplib::Response &res)
  {
    res.set_content("ok","text/plain");
    svr.stop();
  });
    
  
  for(int i=0;i<s;i++)
    {
      svr.Get((std::string("/") + filenames[i]).c_str(),
	      [&](const httplib::Request &req,
		 httplib::Response &res)
	      {
		res.set_content(contents[i],choose_type(filenames[i]));
	      });
    }
  svr.listen(g_http_server_ip, g_http_server_port);
  g_http_server = 0;
}

void *http_server_process(void *ptr)
{
  HTTP_files *files = (HTTP_files*)ptr;
  http_server(files->filenames,files->contents);
  return 0;
}

struct HttpDeployResult
{
  std::vector<std::string> orig_urls;
  std::vector<std::string> filenames;
  std::vector<std::string> contents;
};

std::string replace_urls_from_script(std::string script,
				     const HttpDeployResult &data)
{
  int s = std::min(data.orig_urls.size(),data.filenames.size());
  for(int i=0;i<s;i++)
    {
      std::string orig_url = data.orig_urls[i];
      std::string filename = data.filenames[i];
      std::stringstream ss;
      ss << g_http_server_port;
      script = deploy_replace_string(script, orig_url, std::string("http://") + g_http_server_ip + ":" + ss.str() + std::string("/") + filename);
    }
  return script;
}


HttpDeployResult http_deploy(GameApi::Env &env, std::string h2_script)
{
#ifndef EMSCRIPTEN

  int ok=true;

#ifdef WINDOWS

      std::string str1 = "mkdir %TEMP%\\_gameapi_builder";
      if (gameapi_temp_dir!="@")
	{
	  str1 = deploy_replace_string(str1,"%TEMP%",gameapi_temp_dir);
	}
      int val1 = system(str1.c_str());

      std::string str2 = "rmdir /Q /S %TEMP%\\_gameapi_builder\\deploy";
      std::string str3 = "mkdir %TEMP%\\_gameapi_builder\\deploy";
      
      if (gameapi_temp_dir!="@")
	{
	  str2 = deploy_replace_string(str2,"%TEMP%",gameapi_temp_dir);
	  str3 = deploy_replace_string(str3,"%TEMP%",gameapi_temp_dir);
	}
      int val2=system(str2.c_str());
      int val3=system(str3.c_str());

  
  
	std::string s = h2_script; //h2->script_file();
      s = replace_str(s, "@", "\n");
      s = replace_str(s, "&", "&amp;");
      s = replace_str(s, ">", "&gt;");
      s = replace_str(s, "<", "&lt;");
      s = replace_str(s, "\"", "&quot;");
      s = replace_str(s, "\'", "&apos;");

      std::vector<UrlItem> items = find_url_items(env,s);
      find_url_items2(e,s,items);
      find_url_items3(e,items);
      std::vector<std::string> http_orig_urls;
      std::vector<std::string> http_filenames;
      std::vector<std::string> http_contents;
      
      int si=items.size();
      for(int i=si-1;i>=0;i--)
	{
	  UrlItem ii = items[i];

	  http_orig_urls.push_back(ii.url_orig);
	  
	  ii.url = replace_str(ii.url,"&amp;","&");

	  char buffer3[MAX_PATH];
	  if (_getcwd(buffer3,sizeof(buffer3))) {
	    std::string cd = buffer3;
	    std::string cd2 = GetContentInstallDir(false);
	    if (g_mod_path!="") cd=take_prefix(cd,g_mod_path);
	    cd = convert_spaces_to_url_encoding(cd);
	    cd2 = convert_spaces_to_url_encoding(cd2);
	    ii.url = deploy_replace_string(ii.url,"%CD%",cd);
	    ii.url = deploy_replace_string(ii.url,"%cd%",cd);
	    if (find_str(ii.url,"$(pwd)") != -1) {
	      std::string s2 = cd;
	      s2 = replace_deploy_url(s2);
	      ii.url = deploy_replace_string(ii.url,"$(pwd)",s2);
	    }
	    //url = deploy_replace_string(url,"$(pwd)",cd);
	    if (find_str(ii.url,"$(PWD)") != -1) {
	      std::string s2 = cd;
	      s2 = replace_deploy_url(s2);
	      ii.url = deploy_replace_string(ii.url,"$(PWD)",s2);
	    }
	    ii.url = deploy_replace_string(ii.url,"$(instdir)",cd2);
	    ii.url = deploy_replace_string(ii.url,"$(INSTDIR)",cd2);
	    if (find_str(ii.url,"$(tempdir)") != -1) {
	      std::string s2 = gameapi_temp_dir;
	      s2 = replace_deploy_url(s2);
	      ii.url = deploy_replace_string(ii.url,"$(tempdir)",s2);
	    } else {
	      ii.url = deploy_replace_string(ii.url,"$(tempdir)",gameapi_temp_dir);
	    }
	    //ii.url = deploy_replace_string(ii.url,"$(tempdir)",gameapi_temp_dir);
	    ii.url = deploy_replace_string(ii.url,"$(TEMPDIR)",gameapi_temp_dir);
	  }

	  if (ii.url[ii.url.size()-1]=='/') continue; // ignore directories

	  	  std::string dir = find_directory(ii.url);
	  if (dir!="") {
	    std::string str7 = std::string("mkdir %TEMP%\\_gameapi_builder\\deploy\\")+dir;
      if (gameapi_temp_dir!="@")
	{
	  str7 = deploy_replace_string(str7,"%TEMP%",gameapi_temp_dir);
	}
	    
	    int val=system(str7.c_str());
	    if (val!=0) { std::cout << "ERROR: mkdir returned error " << val << std::endl; ok=false; }
	  }
	  s = deploy_replace_string(s,ii.url_orig,remove_prefix(ii.url));
	  std::string curl="..\\curl\\curl.exe";
	    std::string curl_string;
	  if (file_exists(curl))
	    curl_string= "..\\curl\\curl.exe \"" + convert_spaces_to_url_encoding(deploy_truncate(http_to_https(ii.url))) + "\" --output \"" + "%TEMP%\\_gameapi_builder\\deploy\\" + dir + (dir!=""?"/":"") + deploy_truncate(remove_prefix(remove_str_after_char(ii.url,'?'))) + "\"";
	  else
	    curl_string=".\\curl\\curl.exe \"" + convert_spaces_to_url_encoding(deploy_truncate(http_to_https(ii.url))) + "\" --output \"" + "%TEMP%\\_gameapi_builder\\deploy\\" + dir + (dir!=""?"/":"") + deploy_truncate(remove_prefix(remove_str_after_char(ii.url,'?'))) + "\"";
	  std::cout << curl_string << std::endl;
      if (gameapi_temp_dir!="@")
	{
	  curl_string = deploy_replace_string(curl_string,"%TEMP%",remove_quotes(gameapi_temp_dir));
	}


      //int s7=ii.url.size();
      //int pos=-1;
      //for(int i=0;i<s7;i++)
	//if (ii.url[i]=='/') pos=i;
      
      //std::string id = ii.url.substr(pos+1);
      
      //int s8=m_persistent.size();
      //    bool flag=false;
      //    for(int i=0;i<s8;i++)
      //      {
      //	if (id==m_persistent[i]) flag=true;
      //      }
	    
      //if (!flag) {
	      int val = system(curl_string.c_str());
	      if (val!=0) { std::cout << "ERROR: " << curl_string << " RETURNED ERROR " << val << std::endl; ok=false; }

	      std::string filename = "%TEMP%\\_gameapi_builder\\deploy\\" + dir + (dir!=""?"/":"") + deploy_truncate(remove_prefix(remove_str_after_char(ii.url,'?')));
	      std::ifstream file(filename.c_str(), std::ios_base::binary);
	      std::string contents;
	      char ch;
	      while(file.get(ch)) {
		contents += ch;
	      }
	      int s = filename.size();
	      for(int i=0;i<s;i++)
		if (filename[i]=='\\') filename[i]='/';

	      size_t last_slash = filename.find_last_of('/');
	      std::string short_filename;
	      if (last_slash != std::string::npos)
		{
		  short_filename = filename.substr(last_slash+1);
		}
	      else
		{
		  short_filename="@";
		}
	      http_filenames.push_back(short_filename);
	      http_contents.push_back(contents);
	      //    }
	}
#endif

#ifdef LINUX


      std::string home = getenv("HOME")?getenv("HOME"):"/home/www-data";
      std::string cmd1 = "mkdir -p "+home+"/.gameapi_builder";
      std::string cmd2 = "chmod a+rwx " + home + "/.gameapi_builder";
      
      int val1 = system(cmd1.c_str());
      int val2 = system(cmd2.c_str());
      if (val1!=0) {std::cout << "ERROR: mkdir or chmod returned error" << val1 << " " << val2 << std::endl; ok=false;}
      
      //std::string home = getenv("HOME")?getenv("HOME"):"/home/www-data";
      std::string a_cmd1 = std::string("rm -rf ") + home + std::string("/.gameapi_builder/deploy");
      std::string a_cmd2 = "mkdir -p " + home + "/.gameapi_builder/deploy/store";
      int a_val1= system(a_cmd1.c_str());
      int a_val2=system(a_cmd2.c_str());
      if (a_val1!=0 || a_val2!=0) { std::cout << "ERROR: rm or mkdir returned error!" << a_val1 << " " << a_val2 << std::endl; }
      
      
      std::string s = h2_script; //h2->script_file();
      s = replace_str(s, "@", "\n");
      s = replace_str(s, "&", "&amp;");
      s = replace_str(s, ">", "&gt;");
      s = replace_str(s, "<", "&lt;");
      s = replace_str(s, "\"", "&quot;");
      s = replace_str(s, "\'", "&apos;");
      
      std::vector<UrlItem> items = find_url_items(env,s);
      find_url_items2(env,s,items);
      find_url_items3(env,items);
      
      std::vector<std::string> http_orig_urls;
      std::vector<std::string> http_filenames;
      std::vector<std::string> http_contents;
      
      int si=items.size();
      for(int i=si-1;i>=0;i--)
	{
	  UrlItem ii = items[i];
	  http_orig_urls.push_back(ii.url_orig);
	  
	  ii.url = replace_str(ii.url,"&amp;","&");
	  
	  char buffer3[PATH_MAX];
	  getcwd(buffer3, PATH_MAX);
	  std::string cd = buffer3;
	  if (g_mod_path!="") cd=take_prefix(cd,g_mod_path);
	  cd = convert_spaces_to_url_encoding(cd);
	  ii.url = deploy_replace_string(ii.url,"%CD%",cd);
	  ii.url = deploy_replace_string(ii.url,"%cd%",cd);
	  if (find_str(ii.url,"$(pwd)") != -1) {
	    std::string s2 = cd;
	    s2 = replace_deploy_url(s2);
	    ii.url = deploy_replace_string(ii.url,"$(pwd)",s2);
	  }
	  //url = deploy_replace_string(url,"$(pwd)",cd);
	  if (find_str(ii.url,"$(PWD)") != -1) {
	    std::string s2 = cd;
	    s2 = replace_deploy_url(s2);
	    ii.url = deploy_replace_string(ii.url,"$(PWD)",s2);
	  }
	  //url = deploy_replace_string(url,"$(PWD)",cd);
	  
	  //ii.url = deploy_replace_string(ii.url,"$(pwd)",cd);
	  //ii.url = deploy_replace_string(ii.url,"$(PWD)",cd);
	  if (find_str(ii.url,"$(tempdir)") != -1) {
	    std::string s2 = g_path_handler->use_path(env,gameapi_temp_dir,g_path_handler->situ(PathHandler::ETempDirReplace9));
	    s2 = replace_deploy_url(s2);
	    ii.url = deploy_replace_string(ii.url,"$(tempdir)",s2);
	  } else {
	    ii.url = deploy_replace_string(ii.url,"$(tempdir)",g_path_handler->use_path(env,gameapi_temp_dir,g_path_handler->situ(PathHandler::ETempDirReplace10)));
	  }
	  //ii.url = deploy_replace_string(ii.url,"$(tempdir)",gameapi_temp_dir);
	  ii.url = deploy_replace_string(ii.url,"$(TEMPDIR)",g_path_handler->use_path(env,gameapi_temp_dir,g_path_handler->situ(PathHandler::ETempDirReplace11)));

	  if (ii.url[ii.url.size()-1]=='/') continue; // ignore directories

	  std::string dir = find_directory(ii.url);

	  if (dir!="") {
	    std::string home = getenv("HOME")?getenv("HOME"):"/home/www-data";
	    
	    int val = system((std::string("mkdir -p ") + home + std::string("/.gameapi_builder/deploy/") + dir).c_str());
	    if (val!=0) { std::cout << "ERROR: mkdir returned error: " << val << std::endl; ok=false; }
	  }
	  s = deploy_replace_string(s,ii.url_orig,remove_prefix(ii.url));
	  std::string home = getenv("HOME")?getenv("HOME"):"/home/www-data";
	  
	  std::string curl_cmd = deploy_curl_cmd();
	  
	  std::string curl_string = std::string("(cd " + home + "/.gameapi_builder/deploy/") + dir + (dir!=""?"/":"") + std::string(";") + curl_cmd + " \"" + convert_spaces_to_url_encoding(deploy_truncate(http_to_https(ii.url))) + "\" --output \"" + deploy_truncate(remove_prefix(remove_str_after_char(ii.url,'?'))) + "\")";
	  //std::cout << curl_string << std::endl;
	  int val = system(curl_string.c_str());
	  if (val!=0) { std::cout << "ERROR: " << curl_string << " RETURNED ERROR " << val << std::endl; ok=false; }
	  
	  std::string fn = home + "/.gameapi_builder/deploy/" + deploy_truncate(remove_prefix(remove_str_after_char(ii.url,'?')));

	  std::string filename = fn;
	  std::ifstream file(filename.c_str());
	  std::string contents;
	  char ch;
	  while(file.get(ch)) {
	    contents += ch;
	  }
	  int s = filename.size();
	  for(int i=0;i<s;i++)
	    if (filename[i]=='\\') filename[i]='/';
	  size_t last_slash = filename.find_last_of('/');
	  std::string short_filename;
	  if (last_slash != std::string::npos)
	    {
	      short_filename = filename.substr(last_slash+1);
	    }
	  else
	    {
	      short_filename="@";
	    }
	    
	  http_filenames.push_back(short_filename);
	  http_contents.push_back(contents);
	}
#endif

      HttpDeployResult res;
      res.orig_urls = http_orig_urls;
      res.filenames = http_filenames;
      res.contents = http_contents;
      return res;
#endif
}
