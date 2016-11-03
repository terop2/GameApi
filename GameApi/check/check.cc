
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

std::vector<std::string> mingw_path = { "c:\\mingw2\\" };
std::vector<std::string> emscripten_path = { "c:\\program files\\emscripten2\\", "c:\\program files\\emscripten" };
std::vector<std::string> git_path = { "c:\\program files (x86)\\git\\" };
std::vector<std::string> sdl_path = { "d:\\cvs\\sdl2-2.0.1\\" };
std::vector<std::string> sdl_image_path = { "d:\\cvs\\sdl2_image-2.0.0\\" };
std::vector<std::string> sdl_mixer_path = { "d:\\cvs\\sdl2_mixer-2.0.0\\" };
std::vector<std::string> freetype_path = { "d:\\cvs\\freetype2\\freetype-2.6\\", "d:\\cvs\\freetype\\" };
std::vector<std::string> glew_path = { "d:\\cvs\\glew-1.10.0\\" };
std::vector<std::string> pthread_path = { "d:\\cvs\\pthread\\" };
std::vector<std::string> gnumake_path = { "d:\\cvs\\gnumake\\" };
std::vector<std::string> possible_emscripten_versions = { "1.35.0" };
void error(std::string err, std::string fail_reason)
{
  std::cout << err << std::endl;
  std::cout << fail_reason << std::endl;
  exit(0);
}
void info(std::string info)
{
  std::cout << info << std::endl;
}
std::vector<std::string> concat_includes(std::vector<std::string> vec,
					 std::string path,
					 std::string postfix)
{
  vec.push_back(path + postfix);
  return vec;
}
std::string choose_path(std::vector<std::string> paths)
{
}
std::string chosen_mingw_path()
{
  return mingw_path[0];
}
std::string chosen_glew_path()
{
  return glew_path[0];
}
std::string chosen_sdl_path()
{
  return sdl_path[0];
}
std::string chosen_freetype_path()
{
  return freetype_path[0];
}
std::string chosen_emscripten_path()
{
  return emscripten_path[0];
}
std::string chosen_sdl_image_path()
{
  return sdl_image_path[0];
}
std::string chosen_pthread_path()
{
  return pthread_path[0];
}
std::vector<std::string> include_directories(std::string platform)
{
  std::vector<std::string> vec;
  if (platform=="win32")
    {
      vec = concat_includes(vec, chosen_mingw_path(), "include\\");
      vec = concat_includes(vec, chosen_mingw_path(), "mingw32\\include\\");
      vec = concat_includes(vec, chosen_mingw_path(), "i686-4.9.2-posix-dwarf-rt_v4-rev2\\mingw32\\i686-w64-mingw32\\include\\");
      vec = concat_includes(vec, chosen_mingw_path(), "mingw32\\lib\\gcc\\mingw32\\4.8.1\\include\\c++\\");
      vec = concat_includes(vec, chosen_glew_path(), "include\\");
      vec = concat_includes(vec, chosen_sdl_path(), "i686-w64-mingw32\\include\\SDL2\\");
      vec = concat_includes(vec, chosen_sdl_path(), "i686-w64-mingw32\\include\\");
      vec = concat_includes(vec, chosen_freetype_path(), "include\\");
      vec = concat_includes(vec, chosen_sdl_image_path(), "i686-w64-mingw32\\include\\SDL2\\");
      vec = concat_includes(vec, chosen_pthread_path(), "Pre-built.2\\include\\");
   }
  if (platform=="linux")
    {
    }
  if (platform=="emscripten")
    {
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.34.1\\system\\include\\emscripten\\");
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.35.0\\system\\include\\emscripten\\");
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.35.0\\system\\include\\");
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.35.0\\system\\include\\libcxx\\");
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.35.0\\system\\include\\SDL\\");
      vec = concat_includes(vec, chosen_emscripten_path(), "emscripten\\1.35.0\\system\\include\\libc\\");
    }
  if (platform=="raspi")
    {
    }
  return vec;
}
std::string check_include_one(std::string platform, std::vector<std::string> dirs, std::string include)
{
  int s = dirs.size();
  std::string paths;
  for(int i=0;i<s;i++)
    {
      std::ifstream f((dirs[i]+include).c_str());
      if (f.good()) { return dirs[i]; }
      paths += dirs[i] + "\n";
    }
  error(platform + ": #include <" + include + "> not found", "Default paths are:\n" + paths);
  return "";
}
std::vector<std::string> concat_vector(std::vector<std::string> v1,
			  std::vector<std::string> v2)
{
  std::vector<std::string> vec;
  int s = v1.size();
  for(int i=0;i<s;i++) vec.push_back(v1[i]);
  int s2 = v2.size();
  for(int j=0;j<s2;j++) vec.push_back(v2[j]);
  return vec;
}
std::string current_platform;
void set_platform(std::string platform)
{
  current_platform = platform;
}
void check_include(std::string platform, std::string include)
{
  std::vector<std::string> plats;
  if (platform == "all")
    {
      plats.push_back("win32");
      plats.push_back("emscripten");
      //plats.push_back("linux");
      //plats.push_back("raspi");
    }
  else
    {
      plats.push_back(platform);
    }
  int s = plats.size();
  for(int i=0;i<s;i++)
    {
      if (current_platform == plats[i])
	{
	  std::vector<std::string> dirs = include_directories(plats[i]);
	  std::string path = check_include_one(plats[i], dirs, include);
	  info(plats[i] + ": #include <" + include + "> found.");
	}
    }
  // return path;
}
std::string check_path(std::string path, std::string file_that_needs_to_be_found)
{
}
std::string find_exe(std::string build_name, std::string name, std::vector<std::string> paths, std::string fail_reason)
{
  int s = paths.size();
  for(int i=0;i<s;i++)
    {
      std::string c = paths[i];
      c+=name;
      std::ifstream f(c.c_str());
      if (f.good()) { 
	info(build_name + ": Found " + name + " at " + paths[i]);
	return c; 
      }
    }
  error(build_name + ": Cannot find " + name,fail_reason);
}
std::string get_emscipten_version()
{
}

void check_includes(std::string platform)
{
  set_platform(platform);
  check_include("all", "set");
  check_include("all", "queue");
  check_include("all", "unordered_set");
  check_include("all", "cstdlib");
  check_include("all", "iostream");
  check_include("all", "GL/glew.h");
  check_include("all", "SDL.h");
  check_include("all", "SDL_opengl.h");
  check_include("all", "GL/gl.h");
  //check_include("all", "SDL/SDL.h");
  check_include("win32", "Windows.h");
  check_include("win32", "ft2build.h");
  //check_include("linux", "ft2build.h");
  check_include("all", "string");
  check_include("all", "vector");
  check_include("all", "cmath");
  check_include("all", "sstream");
  check_include("all", "chrono");
  check_include("emscripten", "emscripten.h");
  check_include("win32", "SDL_image.h");
  //check_include("linux", "SDL_image.h");
  check_include("all", "fstream");
  check_include("all", "algorithm");
  check_include("all", "map");
  check_include("all", "sys/types.h");
  check_include("all", "sys/stat.h");
  check_include("all", "typeinfo");
  check_include("all", "iterator");
  check_include("all", "cassert");
  check_include("all", "pthread.h");
  check_include("all", "functional");
  check_include("all", "memory");
  check_include("all", "utility");
  check_include("all", "assert.h");
  check_include("all", "stdlib.h");
  check_include("all", "math.h");
  check_include("all", "cassert");
}

int main()
{
  // WIN32 compilation
  std::string win32_mingw_make = find_exe("win32", "bin\\mingw32-make.exe",mingw_path, "mingw not found");
  std::string win32_gnu_gcc = find_exe("win32", "bin\\g++.exe", mingw_path, "mingw g++ not found");
  std::string win32_gnu_gcc2 = find_exe("win32", "bin\\gcc.exe", mingw_path, "mingw gcc not found");
  std::string win32_sed = find_exe("win32", "bin\\sed.exe", git_path, "git sed.exe not found");
  std::string win32_rm = find_exe("win32", "bin\\rm.exe", git_path, "git rm.exe not found");

  // Emscripten compilation
  std::string emscripten_make = find_exe("emscripten", "bin\\make.exe", gnumake_path,"gnu make not found");
  std::string emscripten_gcc = find_exe("emscripten", "emscripten\\1.35.0\\em++.bat", emscripten_path, "emscripten compiler not found");
  std::string emscripten_sed = find_exe("emscripten", "bin\\sed.exe", git_path, "git sed.exe not found");
  std::string emscripten_rm = find_exe("emscripten", "bin\\rm.exe", git_path, "git rm.exe not found");

  check_includes("win32");
  check_includes("emscripten");
  check_includes("linux");
  check_includes("raspi");

}
