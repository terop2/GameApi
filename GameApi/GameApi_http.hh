#ifndef GAMEAPI_HTTP_HH
#define GAMEAPI_HTTP_HH

#include <string>
#include <vector>
#include "GameApi_h.hh"

void start_http_listening(std::vector<std::string> filenames,
			  std::vector<std::string> contents,
			  std::string homepage,
			  std::string script,
			  std::string date);

void join_http();
void send_http_server_shutdown();
bool choose_http_port();
std::string choose_type(std::string filename);
void http_server(std::vector<std::string> filenames,
		 std::vector<std::string> contents,
		 std::string homepage,
		 std::string script,
		 std::string date);
void *http_server_process(void *ptr);

struct HttpDeployResult
{
  std::vector<std::string> orig_urls;
  std::vector<std::string> filenames;
  std::vector<std::string> contents;
};

std::string replace_urls_from_script(std::string script,
				     const HttpDeployResult &data);

HttpDeployResult http_deploy(GameApi::Env &env, std::string h2_script);

void set_http_server_htmlfile(std::string file);

std::string gameapi_example(std::string homepage, std::string script, std::string date);


#endif
