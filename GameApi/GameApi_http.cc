#include "GameApi_http.hh"
#include "GameApi.hh"
#include "Tasks.hh"
#define CPPHTTPLIB_NO_EXCEPTIONS
#include "httplib.h"
#include "GraphI.hh"
#define MINIZ_HEADER_FILE_ONLY 1
#include "zip_file.hpp"

bool file_exists(std::string url);
std::string remove_quotes(std::string str);
IMPORT extern std::string g_http_server_ip;
IMPORT extern int g_http_server_port;
std::string GetContentInstallDir(bool b);

EXPORT std::string g_http_server_ip = "127.0.0.1";
EXPORT int g_http_server_port=50200;

bool g_wait_ongoing = true;
void *http_server_wait_process(void*);

EXPORT std::string http_server_address() {
  std::stringstream ss;
  ss << g_http_server_port;
  return g_http_server_ip + ":" + ss.str();
}


IMPORT void *http_server_process(void *);
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
  std::string homepage;
  std::string script;
  std::string date;
  bool transparent;
};

void set_cors_headers(httplib::Response &res)
{
  //res.set_header("Access-Control-Allow-Headers", "Range");
}
bool handle_range_request(const httplib::Request &req, httplib::Response &res, const std::vector<std::string> &filenames, const std::vector<std::string> &contents, std::string filename)
{
  std::string range = req.get_header_value("Range");
  bool done = false;

  int s = filenames.size();
  int pos = -1;
  for(int i=0;i<s;i++)
    {
      if (filename == filenames[i]) { pos=i; break; }
    }
  if (pos==-1) return false;
  size_t filesize = contents[pos].size();
  
    if (!range.empty())
    {
        // Example: "bytes=100-199"
        size_t start = 0;
        size_t end = filesize - 1;

	//std::cout << "RANGE is:" << range << std::endl;
	
        if (sscanf(range.c_str(), "bytes=%zu-%zu", &start, &end) < 1)
        {
	  //std::cout << "RETURNING 416:" << start << " " << end << std::endl;
            res.status = 416;
            return true;
        }

        if (end >= (size_t)filesize)
            end = filesize - 1;
	
        size_t len = end - start + 1;

        //std::string data(len, '\0');
	std::vector<unsigned char> data;
	data.resize(len-1);
	
	std::copy(contents[pos].begin()+start,contents[pos].begin()+start+len-1,&data[0]);
	
        //file.seekg(start);
        //file.read(data.data(), len);

        res.status = 206;
        res.set_header("Accept-Ranges", "bytes");
        res.set_header(
            "Content-Range",
            ("bytes " + std::to_string(start) + "-" +
             std::to_string(end) + "/" +
             std::to_string(filesize)).c_str());
	res.set_header( "Content-Length", std::to_string(len));

	//std::cout << "Content-Range header set to: bytes " + std::to_string(start) + "-" + std::to_string(end) + "/" + std::to_string(filesize) << std::endl;

	std::string data2(data.begin(),data.end());
	
        res.set_content(data2,
                        choose_type(filename) /*"application/octet-stream"*/);
	done = true;
	//std::cout << "Range request handled!" << std::endl;
    }
    return done;
}

EXPORT void start_http_listening(std::vector<std::string> filenames,
			  std::vector<std::string> contents,
			  std::string homepage,
			  std::string script,
			  std::string date,
			  bool transparent)
{
  HTTP_files *files = new HTTP_files;
  files->filenames = filenames;
  files->contents = contents;
  files->homepage = homepage;
  files->script = script;
  files->date = date;
  files->transparent = transparent;
  tasks_add(9898, &http_server_process, (void*)files);
  tasks_add(9899, &http_server_wait_process, (void*)0);
}
EXPORT void join_http()
{
  tasks_join(9898);
}

extern httplib::Server *g_http_server;

EXPORT void start_server_shutdown()
{
  if (g_http_server)
    g_http_server->stop();
}

bool g_http_up_and_running=false;

EXPORT void http_sleep()
{

  while(g_http_up_and_running==false) {
#ifdef LINUX
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
#ifdef WINDOWS
    Sleep(100);
    //std::this_thread::sleep_for(std::chrono::milliseconds(3));
#endif
  }
  g_http_up_and_running = false;
}
EXPORT void send_http_server_shutdown()
{
  std::cout << "Note: shutting down http server at " << g_http_server_ip << ":" << g_http_server_port << std::endl;
  std::stringstream ss;
  ss << g_http_server_port;
  std::string url = "http://" + g_http_server_ip + ":" + ss.str() + "/stop";
  std::string headers = "";
  std::string data = "";
  send_post_request(url,headers,data);
}

EXPORT bool choose_http_port()
{
  httplib::Server svr;
  int counter=0;
  bool error = false;
  while (!svr.bind_to_port("0.0.0.0",g_http_server_port))
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
  svr.stop();
  return !error;
}

const char *choose_type(std::string filename)
{
  if (filename.substr(filename.size()-4,4)=="wasm")
    {
      return "application/wasm";
    }
  if (filename.substr(filename.size()-2,2)=="js")
    {
      return "text/javascript";
    }
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

std::string g_http_htmlfile;

httplib::Server *g_http_server = 0;

EXPORT void http_server(std::vector<std::string> filenames,
		 std::vector<std::string> contents,
		 std::string homepage,
		 std::string script,
		 std::string date, bool transparent)
{
  static std::vector<std::string> g_filenames;
  g_filenames = filenames;
  static std::vector<std::string> g_contents;
  g_contents = contents;
  static std::string g_homepage;
  g_homepage = homepage;
  static std::string g_script;
  g_script = script;
  static std::string g_date;
  g_date = date;
  static bool g_transparent;
  g_transparent = transparent;

  

  
  httplib::Server *svr = new httplib::Server;
  g_http_server = svr;
  
  int s = std::min(filenames.size(),contents.size());

  svr->Post("/stop", [&](const httplib::Request &req,
		      httplib::Response &res)
  {
    std::cout << "Server shutdown..." << std::endl;
    res.set_content("ok","text/plain");
    res.set_header("Connection", "close");
    res.status = 200;
    std::thread([&]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      svr->stop();
    }).detach();
  });
    

  svr->Get("/user_data/temp/tmp0.txt",
	   [&](const httplib::Request &req,
	       httplib::Response &res)
	   {
		g_wait_ongoing = false;
		std::cout << "Fetching3: /user_data/temp/tmp0.txt"<< std::endl;
		set_cors_headers(res);
	     res.set_content(g_http_htmlfile,"text/plain");
	     res.status = 200;
	   });
  svr->Get("/gameapi_example.html",
	   [&](const httplib::Request &req,
	       httplib::Response &res)
	   {
		g_wait_ongoing = false;
		std::cout << "Fetching2: /gameapi_example.html"<< std::endl;
		static std::string s;
		set_cors_headers(res);
		s = gameapi_example(homepage,script,date,transparent);
	     res.set_content(s,"text/html");
	     res.status = 200;
	   });

  std::vector<std::string> *fn = new std::vector<std::string>;
  for(int i=0;i<s;i++)
    {
      fn->push_back((std::string("/") + filenames[i]));
      svr->Get(fn->operator[](fn->size()-1).c_str(),
	      [&](const httplib::Request &req,
		 httplib::Response &res)
	      {
		g_wait_ongoing = false;
		//std::cout << "path:" << req.path << std::endl;
		for (int j=0;j<g_filenames.size();j++)
		  {
		    if ((std::string("/")+g_filenames[j])==req.path) {
		      set_cors_headers(res);
		      bool b = handle_range_request(req,res,g_filenames,g_contents,req.path.substr(1));
		      if (!b) {
			res.set_content(g_contents[j],choose_type(g_filenames[j]));
			res.status = 200;

		      }
		    }
		  }
	      });
    }
  g_http_up_and_running = true;
  svr->listen(g_http_server_ip, g_http_server_port, SO_REUSEADDR);
  g_http_server = 0;
}


void *http_server_wait_process(void *ptr)
{
  while(g_wait_ongoing) {
#ifdef LINUX
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
#ifdef WINDOWS
    Sleep(100);
    //std::this_thread::sleep_for(std::chrono::milliseconds(3));
#endif
  }
  
#ifdef LINUX
    std::this_thread::sleep_for(std::chrono::milliseconds(3000000));
#endif
#ifdef WINDOWS
    Sleep(3000000);
    //std::this_thread::sleep_for(std::chrono::milliseconds(3));
#endif
    g_wait_ongoing = true;
    std::cout << "Trying to shutdown http server.." << std::endl;
    start_server_shutdown();
    return 0;
}

EXPORT void *http_server_process(void *ptr)
{
  HTTP_files *files = (HTTP_files*)ptr;
  http_server(files->filenames,files->contents, files->homepage, files->script, files->date, files->transparent);
  return 0;
}


EXPORT std::string replace_urls_from_script(std::string script,
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


EXPORT HttpDeployResult http_deploy(GameApi::Env &env, std::string h2_script)
{
#ifndef EMSCRIPTEN

  int ok=true;

#ifdef WINDOWS

      std::string str1 = "mkdir %TEMP%\\_gameapi_builder";
      if (use_path4(gameapi_temp_dir)!="@")
	{
	  str1 = deploy_replace_string(str1,"%TEMP%",use_path4(gameapi_temp_dir));
	}
      int val1 = system(str1.c_str());

      std::string str2 = "rmdir /Q /S %TEMP%\\_gameapi_builder\\deploy";
      std::string str3 = "mkdir %TEMP%\\_gameapi_builder\\deploy";
      
      if (use_path4(gameapi_temp_dir)!="@")
	{
	  str2 = deploy_replace_string(str2,"%TEMP%",use_path4(gameapi_temp_dir));
	  str3 = deploy_replace_string(str3,"%TEMP%",use_path4(gameapi_temp_dir));
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
	      std::string s2 = use_path4(gameapi_temp_dir);
	      s2 = replace_deploy_url(s2);
	      ii.url = deploy_replace_string(ii.url,"$(tempdir)",s2);
	    } else {
	      ii.url = deploy_replace_string(ii.url,"$(tempdir)",use_path4(gameapi_temp_dir));
	    }
	    //ii.url = deploy_replace_string(ii.url,"$(tempdir)",gameapi_temp_dir);
	    ii.url = deploy_replace_string(ii.url,"$(TEMPDIR)",use_path4(gameapi_temp_dir));
	  }

	  if (ii.url[ii.url.size()-1]=='/') continue; // ignore directories

	  	  std::string dir = find_directory(ii.url);
	  if (dir!="") {
	    std::string str7 = std::string("mkdir %TEMP%\\_gameapi_builder\\deploy\\")+dir;
	    if (use_path3(env,gameapi_temp_dir,PathHandler::ETempDirReplace33)!="@")
	{
	  str7 = deploy_replace_string(str7,"%TEMP%",use_path3(env,gameapi_temp_dir,PathHandler::ETempDirReplace32));
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
	  if (use_path3(env,gameapi_temp_dir,PathHandler::ETempDirReplace31)!="@")
	{
	  curl_string = deploy_replace_string(curl_string,"%TEMP%",remove_quotes(g_path_handler->use_path(env,gameapi_temp_dir,g_path_handler->situ(PathHandler::ETempDirReplace30))));
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
      /*
      int s5 = res.filenames.size();
      for(int i=0;i<s5;i++)
	{
	  std::cout << "DeployRes:" << res.filenames[i] << std::endl;
	}
      */
      return res;
#endif
}

void set_http_server_htmlfile(std::string file)
{
  g_http_htmlfile = file;
}

std::string gameapi_example(std::string homepage, std::string script, std::string date, bool transparent)
{
  std::string example=
"<?php\n"
"  //header(\"Cross-Origin-Opener-Policy: same-origin\");\n"
"  $id = $_GET[\"id\"];\n"
"  $filename = \"./user_data/temp/tmp\" . $id . \".txt\";\n"
"  $script = file_get_contents($filename);\n"
"  $homepage = $_GET[\"homepage\"];\n"
"  $new_script = str_replace(\"@\",\"\n\",$script);\n"
"  $date = $_GET[\"date\"];\n"
"  ?>\n"
"<!DOCTYPE html>\n"
"<html>\n"
"  <head>\n"
"    <meta name=\"description\" content=\"a 3d model created via meshpage.org/gltf_to_zip.php\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\">\n"
"  </head>\n"
"  <body>\n"
"    <!-- you need to copy the following files to transfer 3d models\n"
"         to new web server:\n"
"         https://meshpage.org/gameapi_display.zip\n"
"\n"
"	 unzip the package to some new directory in your web host\n"
"\n"
"	 Then change the homepage url from inside pre tag below\n"
"\n"
"	 Then copy-paste a builder codegen script to the gameapi_script \n"
"	 pre tag below, last line needs to have type RUN.\n"
"      -->\n"
"    <pre id=\"homepage\" style=\"display:none\">\n"
"    <?php echo $homepage ?>\n"
"    </pre>\n"
"    <pre id=\"gameapi_script\" style=\"display:none\">\n"
"<?php echo $new_script ?>\n"
"    </pre>\n"
"    <pre id=\"gameapi_modificationdate\" style=\"display:none\">\n"
"<?php echo $date ?>\n"
"    </pre>\n"
"   <div id=\"container\">\n"
"    <canvas id=\"canvas\"></canvas>\n"
"    </div>\n"
"    <style>\n"
"      #container { display: inline-block;  width: 400px; height: 300px; }\n"
    "      #canvas { position: absolute;\n";
  if (!transparent) {
example += "		border-width:0px;\n"
"		border: 5px solid black;\n"
"		border-radius: 10px;\n"
  "		background-color: #000000;\n";
  }
  example +=
"		margin:0;\n"
"		padding:0;\n"
"		width: 820px;\n"
"		height: 620px;\n"
"	      }\n"
"      </style>\n"
"\n"
"    <!-- TO CHANGE THE PARAMETERS, YOU SHOULD CHANGE:        \n"                                                                         
"         container width/height (to change what area of the page is reserved for the model)                                                             \n"
"         and wd/hd (to change how large the 3d model is)                      \n"
"                                                                         \n"
"         and delta_x and delta_y (to position the model)                      \n" "                                                                         \n"
"    -->\n"
"    <script>\n"
"      let wd = 800-28;\n"
"      let hd = 600-28;\n"
"      let delta_x=0;\n"
"      let delta_y=0;\n"
"      let container_width=800-28;\n"
"      let container_height=600-28;\n"
"      let enable_debug_border=false;\n"
"      if (window.self !== window.top)\n"
"      { // we're inside iframe, use iframe dimensions\n"
"	  wd = window.innerWidth-28;\n"
"	  hd = window.innerHeight-28;\n"
"	  container_width = window.innerWidth-28;\n"
"	  container_height= window.innerHeight-28;\n"
"      }\n"
"\n"
"      \n"
"    </script>\n"
"    <script src=\"gameapi.js?1\"></script>\n"
"    <script>\n"
"function resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border) {\n"
"    return function (event) {\n"
"      if (window.self !== window.top)\n"
"      { // we're inside iframe, use iframe dimensions\n"
"	  wd = window.innerWidth-28;\n"
"	  hd = window.innerHeight-28;\n"
"	  container_width = window.innerWidth-28;\n"
"	  container_height= window.innerHeight-28;\n"
"      }\n"
"	\n"
"          if (Module && g_emscripten_running) {\n"
"      var cc = document.getElementById(\"container\");\n"
"      var c = document.getElementById(\"canvas\");\n"
"          if (enable_debug_border)\n"
"              {\n"
"                  cc.style.borderStyle=\"solid\";\n"
"                  cc.style.borderWidth=\"1px\";\n"
"                  cc.style.borderColor=\"black\";\n"
"                 c.style.borderStyle=\"solid\";\n"
"                  c.style.borderWidth=\"1px\";\n"
"                  c.style.borderColor=\"black\";\n"
"          }\n"
"           try {\n"
"               Module.ccall(\"set_resize_event\", null, [\"number\", \"number\"], [wd,hd], {async:true});\n"
"           } catch(e) {\n"
"               console.log(e);\n"
"           }\n"
"              cc.style.width = (container_width).toString() + \"px\";\n"
"              cc.style.height = (container_height).toString() + \"px\";\n"
"              c.style.width = (wd).toString() + \"px\";\n"
"              c.style.height = (hd).toString() + \"px\";\n"
"              c.style.left = (delta_x + cc.offsetLeft + (cc.clientWidth-wd)/2).toString() + \"px\";\n"
"c.style.top = (delta_y + cc.offsetTop + (cc.clientHeight-hd)/2).toString() + \"px\";\n"
"            const dpr = window.devicePixelRatio || 2;\n"
"            c.width = (wd)*dpr;\n"
"            c.height = (hd)*dpr;\n"
"\n"
"          } else window.setTimeout(function() { resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null); }, 100);\n"
"      }\n"
"}\n"
"window.addEventListener(\"resize\", function() {\n"
"   resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null);\n"
"});\n"
"window.setTimeout(function() { resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null); },10);\n"
"\n"
"    </script>\n"
"  </body>\n"
"</html>\n"
"\n"
"\n"
"<!-- html>\n"
"  <head>\n"
"  </head>\n"
"  <body>\n"
"    <pre id=\"homepage\" style=\"display:none\">\n"
"<?php echo $homepage ?>\n"
"    </pre>\n"
"    <pre id=\"gameapi_script\" style=\"display:none\">\n"
"<?php echo $new_script ?>\n"
"    </pre>\n"
"    <pre id=\"gameapi_modificationdate\" style=\"display:none\">\n"
"<?php echo $date ?>\n"
"    </pre>\n"
"    <div class=\"center\">\n"
    "    <canvas id=\"canvas\" style=\"border-width:0px; ";
  if (!transparent)
    example += "border: 5px solid black; border-radius: 10px; background-color: #000000; ";
example += "margin:0; padding:0; width: 820px; height: 620px;\"></canvas>\n"
"    </div>\n"
"    <style>\n"
"    .center {\n"
"       display: flex;\n"
"       align-items: center;\n"
"       justify-content: center;\n"
"    }\n"
"    </style>\n"
"    <script src=\"gameapi.js?<?php echo $date ?>\"></script>\n"
"      \n"
"  </body>\n"
"</html -->\n"
"\n"
"  \n"
																					     "}\n";

 example= deploy_replace_string(example,"<?php echo $homepage ?>", homepage);
 example= deploy_replace_string(example,"<?php echo $new_script ?>", script);
 example= deploy_replace_string(example,"<?php echo $date ?>", date);
 return example;
}


EXPORT std::vector<unsigned char> find_display_zip_file()
{
#ifdef EMSCRIPTEN
  std::string filename;
#endif
#ifdef WINDOWS
  std::string filename = GetInstallDir2(false) + "/gameapi_display.zip"; 
#endif
#ifdef LINUX
  char path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
  if (count != -1) {
    path[count] = '\0';
  }
  int s = strlen(path);
  int pos = -1;
  for(int i=0;i<s;i++)
    {
      if (path[i]=='/') pos=i;
    }
  std::string filename;
  if (pos!=-1) {
    std::string path2 = path;
    filename = path2.substr(0,pos) + "/gameapi_display.zip";
    if (!file_exists(filename))
      {
	filename = path2.substr(0,pos) + "/../display/gameapi_display.zip";
      }
  }
#endif
  std::vector<unsigned char> res;
  std::ifstream ss(filename.c_str(),std::ios::binary);
  char ch;
  while(ss.get(ch)) { res.push_back(ch); }
  return res;
}


EXPORT std::vector<HttpFileFromZip> decompress_zip_file(std::vector<unsigned char> zip_contents)
{
  std::vector<HttpFileFromZip> result;

  mz_zip_archive pZip;
  std::memset(&pZip,0,sizeof(mz_zip_archive));
  mz_bool b2 = mz_zip_reader_init_mem(&pZip, &zip_contents[0], zip_contents.size(),0);
  mz_uint num = mz_zip_reader_get_num_files(&pZip);
  for(int i=0;i<num;i++)
    {
      mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip,i);
      if (is_dir) {
	char *filename = new char[256];
	for(int j=0;j<256;j++)
	  filename[j]=0;
	mz_uint err = mz_zip_reader_get_filename(&pZip,i,filename,256);
	// TODO, DIRECTORIES
      } else {
	// normal file
	char *filename = new char[256];
	for(int j=0;j<256;j++)
	  filename[j]=0;
	mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	if (strlen(filename)==0) { delete [] filename; continue; }

	std::vector<unsigned char> *data = new std::vector<unsigned char>;
	mz_zip_archive_file_stat file_stat;
	size_t uncompressed_size = 0;
	if (mz_zip_reader_file_stat(&pZip, i, &file_stat))
	  {
	    uncompressed_size = (size_t)file_stat.m_uncomp_size;
	    data->resize(uncompressed_size);
	  }
	else
	  {
	    std::cout << "error at resize";
	    continue;
	  }
	mz_bool b = mz_zip_reader_extract_to_mem(&pZip, i, (void*)&data->operator[](0), uncompressed_size, 0);
	if (!b) { std::cout << "error at extract" << std::endl; continue; }

	HttpFileFromZip file;
	//std::cout << "Zip:'" << filename << "'" << std::endl;
	if (std::string(filename)==std::string("gameapi.js"))
	  file.filename = filename;
	else
	  file.filename = std::string("engine/") + filename;
	file.contents = std::string(data->begin(),data->end());
	result.push_back(file);
	delete [] filename;
	delete data;
      }
    }
  return result;
}
