#ifndef GAMEAPI_HTTP_HH
#define GAMEAPI_HTTP_HH

#include <string>
#include <vector>
#include "GameApi.hh"

IMPORT void start_http_listening(std::vector<std::string> filenames,
			  std::vector<std::string> contents,
			  std::string homepage,
			  std::string script,
			  std::string date,
			  bool transparent);

IMPORT std::string http_server_address();
IMPORT void http_sleep();
IMPORT void join_http();
IMPORT void send_http_server_shutdown();
IMPORT void start_server_shutdown();
IMPORT bool choose_http_port();
IMPORT const char * choose_type(std::string filename);
IMPORT void http_server(std::vector<std::string> filenames,
		 std::vector<std::string> contents,
		 std::string homepage,
		 std::string script,
		 std::string date, bool transparent);
IMPORT void *http_server_process(void *ptr);

struct HttpDeployResult
{
  std::vector<std::string> orig_urls;
  std::vector<std::string> filenames;
  std::vector<std::string> contents;
};

IMPORT std::string replace_urls_from_script(std::string script,
				     const HttpDeployResult &data);

IMPORT HttpDeployResult http_deploy(GameApi::Env &env, std::string h2_script);

void set_http_server_htmlfile(std::string file);

std::string gameapi_example(std::string homepage, std::string script, std::string date, bool transparent);

struct HttpFileFromZip
{
  std::string filename;
  std::string contents;
};

IMPORT std::vector<unsigned char> find_display_zip_file();
IMPORT std::vector<HttpFileFromZip> decompress_zip_file(std::vector<unsigned char> zip_contents);




#endif
