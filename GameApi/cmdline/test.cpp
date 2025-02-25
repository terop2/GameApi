
#include "GameApi.hh"
#include "Tasks.hh"
#include <sstream>
#include <fstream>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

#ifdef ANDROID
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <SDL2/SDL.h>
#include <jni.h>
#endif
Low_SDL_Surface *InitSDL2(int scr_x, int scr_y, bool vblank, bool antialias, bool resize, bool vr_init);

#ifdef ANDROID
GameApi::RUN android_run = { -1 };
GameApi::BLK android_blk = { -1 };

#endif

void splitter_iter2(void *arg);

#ifdef ANDROID
extern GameApi::RUN android_run;
extern GameApi::BLK android_blk;
extern GameApi::EveryApi *g_everyapi;
extern "C" void SDL_Android_Init(JNIEnv*env, jclass cls)
{
        SDL_Log("ANDROID Init called");
}
extern "C"    void SDL_Android_GLES_Init() {
        SDL_Log("GLES Init called");
    }
    
    // Try these variants too in case SDL version difference
extern "C"    void Android_GLES_RenderFrame() {
        SDL_Log("Alternative render frame called");
    }

 extern "C"   void Android_GLES_Init() {
        SDL_Log("Alternative GLES init called");
    }

extern "C" void Android_RenderFrame();

extern "C" void SDL_Android_GLES_RenderFrame() {
  Android_RenderFrame();
}
#endif


void onload(unsigned int, void*, const char* var)
{
  std::cout << "POST RESULT: " << var << std::endl;
  std::ifstream ss(var);
  std::string data;
  char ch;
  while(ss>>ch) { data+=ch; }
  //std::cout << "POST FILE: " << data << std::endl;
}
void onerror(unsigned int, void*, int)
{
  std::cout << "POST ERROR: " << std::endl;
}
void onprogress(unsigned int, void*, int)
{
}

bool find_string(std::string s, std::string s2)
{
  //std::cout << "FIND_STRING:" << s << "==" << s2 << std::endl;
  int ss=s.size()-s2.size();
  int sk = s2.size();
  for(int i=0;i<ss;i++)
    {
      bool found = true;
      for(int j=0;j<sk;j++) {
	if (s[i+j]!=s2[j]) found=false;
      }
      if (found) return true;
    }
  return false;
}

std::string hexify5(std::string s)
{
  std::string res;
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      unsigned char c = s[i];
      unsigned char c2 = c & 0xf;
      unsigned char c3 = c & 0xf0;
      c3>>=4;
      const char *chrs = "0123456789ABCDEF";
      res+=chrs[c3];
      res+=chrs[c2];
    }
  return res;
}


std::vector<std::string> screenshot_filenames;
std::vector<float> screenshot_times;
std::vector<bool> screenshot_done;

using namespace GameApi;
struct Envi {
  EveryApi *ev;
  //PolygonObj *poly;
  MN move;
  ML mainloop;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  InteractionApi::Quake_data data;
  bool logo_shown = false;
  SH color_sh;
  SH texture_sh;
  SH arr_texture_sh;
  int screen_width = 1280;
  int screen_height = 900;
  BLK blk;
};

std::string code=
"BM I1=ev.bitmap_api.chessboard(20,20,8,8,ffffffff,ff880088);\n"
"FB I2=ev.float_bitmap_api.from_green(I1);\n"
"S I3=ev.polygon_api.s_spherical(0,0,0,I2,300,310,0,1);\n"
"BM I4=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/map.jpg);\n"
"BM I5=ev.bitmap_api.rot90(I4);\n"
"S I6=ev.surface_api.texture(I3,I5);\n"
				      "P I7=ev.polygon_api.s_sample(I6,200,200);\n"
				      "MT I8=ev.materials_api.def(ev);\n"
				      "MT I9=ev.materials_api.web(ev,I8,1.03,2.0,ff000000);\n"
				      "ML I10=ev.materials_api.bind(I7,I9);\n"
				      "MN I11=ev.move_api.empty();\n"
				      "MN I12=ev.move_api.rotatex(I11,3.141);\n"
				      "MN I13=ev.move_api.rotate(I12,0,10000,0,0,0,0,0,1,628);\n"
				      "MN I14=ev.move_api.rotatex(I13,2.5);\n"
				      "ML I15=ev.move_api.move_ml(ev,I10,I14,1,10.0);\n"
				      "ML I16=ev.mainloop_api.touch_rotate(ev,I15,true,true,0.01,0.01);\n"
				      "RUN I17=ev.blocker_api.game_window2(ev,I16,false,false,0.0,100000.0);\n"
  /*  "PT I1=ev.point_api.point(0.0,0.0,0.0);\n"
  "P I2=ev.polygon_api.torus2(ev,7,9,I1,250,100);\n"
  "MT I3=ev.materials_api.def(ev);\n"
  "MT I4=ev.materials_api.snow(ev,I3);\n"
  "ML I5=ev.materials_api.bind(I2,I4);\n"
  "MN I6=ev.move_api.empty();\n"
  "MN I7=ev.move_api.anim_enable(I6,0,100);\n"
  "MN I8=ev.move_api.time_repeat(I7,0,200);\n"
  "ML I9=ev.move_api.move_ml(ev,I5,I8);\n"
  "PT I10=ev.point_api.point(0.0,0.0,0.0);\n"
  "P I11=ev.polygon_api.torus2(ev,7,9,I10,250,100);\n"
  "MT I12=ev.materials_api.def(ev);\n"
  "MT I13=ev.materials_api.brashmetal(ev,I12,80000,false);\n"
  "ML I14=ev.materials_api.bind(I11,I13);\n"
  "MN I15=ev.move_api.empty();\n"
  "MN I16=ev.move_api.anim_enable(I15,100,200);\n"
  "MN I17=ev.move_api.time_repeat(I16,0,200);\n"
  "ML I18=ev.move_api.move_ml(ev,I14,I17);\n"
  "ML I19=ev.mainloop_api.array_ml(std::vector<ML>{I9,I18});\n"
  "MN I20=ev.move_api.empty();\n"
  "MN I21=ev.move_api.rotate(I20,0,10000,0,0,0,0,1,0,600);\n"
  "ML I22=ev.move_api.move_ml(ev,I19,I21);\n"
  "ML I23=ev.move_api.move_x_ml(ev,I22,100,97,5,-100,100);\n"
  "ML I24=ev.move_api.move_y_ml(ev,I23,119,115,5.0,-100.0,100.0);\n"
  "PT I25=ev.point_api.point(0.0,0.0,0.0);\n"
  "P I26=ev.polygon_api.torus2(ev,7,8,I25,25,15);\n"
  "P I27=ev.polygon_api.recalculate_normals(I26);\n"
  "P I28=ev.polygon_api.color_from_normals(I27);\n"
  "BB I29=ev.bool_bitmap_api.empty(100,100);\n"
  "BB I30=ev.bool_bitmap_api.circle(I29,50,50,50);\n"
  "PTS I31=ev.points_api.random_bitmap_instancing(ev,I30,300,-300,300,-300,300,0);\n"
  "PTS I32=ev.points_api.rot_x(I31,1.5708);\n"
  "PTS I33=ev.points_api.scale(I32,3,3,3);\n"
  "BB I34=ev.bool_bitmap_api.empty(100,100);\n"
  "BB I35=ev.bool_bitmap_api.circle(I34,50,50,50);\n"
  "PTS I36=ev.points_api.random_bitmap_instancing(ev,I35,300,-300,300,-300,300,0);\n"
  "PTS I37=ev.points_api.rot_x(I36,1.5708);\n"
  "PTS I38=ev.points_api.scale(I37,3,3,3);\n"
  "PTS I39=ev.points_api.move(I38,2500,0,0);\n"
  "PTS I40=ev.points_api.or_points(I33,I39);\n"
  "BB I41=ev.bool_bitmap_api.empty(100,100);\n"
  "BB I42=ev.bool_bitmap_api.circle(I41,50,50,50);\n"
  "PTS I43=ev.points_api.random_bitmap_instancing(ev,I42,300,-300,300,-300,300,0);\n"
  "PTS I44=ev.points_api.rot_x(I43,1.5708);\n"
  "PTS I45=ev.points_api.scale(I44,3,3,3);\n"
  "PTS I46=ev.points_api.move(I45,-2500,0,0);\n"
  "PTS I47=ev.points_api.or_points(I40,I46);\n"
  "MT I48=ev.materials_api.def(ev);\n"
  "MT I49=ev.materials_api.web(ev,I48);\n"
  "ML I50=ev.materials_api.bind_inst(I28,I47,I49);\n"
  "MN I51=ev.move_api.empty();\n"
  "MN I52=ev.move_api.trans2(I51,5000,0,0);\n"
  "MN I53=ev.move_api.translate(I52,0,100,-10000,0,0);\n"
  "MN I54=ev.move_api.time_repeat(I53,0.0,100.0);\n"
  "ML I55=ev.move_api.move_ml(ev,I50,I54);\n"
  "ML I56=ev.mainloop_api.array_ml(std::vector<ML>{I24,I55});\n"
  */
;

std::string homepageurl = "";

#ifdef ANDROID


class SDLStreamBuf : public std::streambuf {
protected:
    std::string buffer;

    virtual int_type overflow(int_type c) {
        if (c != EOF) {
            if (c == '\n') {
                SDL_Log("%s", buffer.c_str());
                buffer.clear();
            } else {
                buffer += static_cast<char>(c);
            }
        }
        return c;
    }

    virtual int sync() {
        if (!buffer.empty()) {
            SDL_Log("%s", buffer.c_str());
            buffer.clear();
        }
        return 0;
    }
};


AAssetManager* getAssetManager();

void copyAssetToInternal(const char* filename) {
    AAssetManager* assetManager = getAssetManager();
    if (!assetManager) {
        SDL_Log("Failed to initialize AssetManager!");
        return;
    }

    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
    if (!asset) {
        SDL_Log("Failed to open asset: %s", filename);
        return;
    }

    const char* internalStoragePath = SDL_AndroidGetInternalStoragePath();
    std::string outputPath = std::string(internalStoragePath) + "/" + filename;

    FILE* outputFile = fopen(outputPath.c_str(), "wb");
    if (!outputFile) {
        SDL_Log("Failed to open output file: %s", outputPath.c_str());
        AAsset_close(asset);
        return;
    }

    const void* buffer = AAsset_getBuffer(asset);
    size_t size = AAsset_getLength(asset);
    fwrite(buffer, 1, size, outputFile);

    fclose(outputFile);
    AAsset_close(asset);

    SDL_Log("Successfully copied %s to internal storage at %s", filename, outputPath.c_str());
}

AAssetManager* getAssetManager() {
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManager = env->CallObjectMethod(activity, getAssetsMethod);
    jobject glob = env->NewGlobalRef(assetManager);
    
    return AAssetManager_fromJava(env, glob);
}

std::string loadAsset(const char* filename) {
    AAssetManager* assetManager = getAssetManager();
    if (!assetManager) {
        SDL_Log("Failed to obtain AssetManager!");
        return "";
    }
    SDL_Log("Tero: Trying to open:%s", filename);
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
    if (!asset) {
        SDL_Log("Failed to open asset: %s", filename);
        return "";
    }

    size_t assetLength = AAsset_getLength(asset);
    const void* buffer = AAsset_getBuffer(asset);
    const char *buf = (const char*)buffer;
    // Do something with the asset's content
    SDL_Log("Asset size: %zu bytes", assetLength);
    return std::string(buf);
   
    // Clean up
    //AAsset_close(asset);
}
#endif

std::string strip_spaces(std::string data)
{
  std::string res;
  int s = data.size();
  for(int i=0;i<s-1;i++)
    {
      char c1 = data[i];
      char c2 = data[i+1];
      if (c1==' ' && c2==' ') { }
      else { res+=c1; }
    }
  char c3 = data[s-1];
  res+=c3;
  while(res[0]==' ') res = res.substr(1);
  while(res[res.size()-1]==' ') res = res.substr(0,res.size()-1);
  return res;
}
std::pair<int,std::string> mainloop(Env &e2, EveryApi &ev)
{
  ExecuteEnv e;
  std::pair<int,std::string> p = GameApi::execute_codegen(e2, ev, code, e);
  return p;
  /*  BLK I6;
  I6.id = p.first;
  return I6;*/
}
std::string decode(std::string s)
{
  int ss = s.size();
  std::string str;
  for(int i=0;i<ss;i++)
    {
      char c = s[i];
      if (c=='@') c='\n';
      str+=c;
    }
  return str;
}
std::string insert_enter(std::string s)
{
  if (s.length()>0)
    {
      char c = s[s.size()-1];
      if (c>='0'&&c<='9') s+=");";
      if (c==')') s+=";";
      if (c!='\n') s+='\n';
    }
  return s;
}
bool check_count(std::vector<std::string> cmd_args, int current_arg, int count)
{
  return cmd_args.size()-current_arg >= count;
}

void set_status(int val, int val_max) {
  //#ifdef EMSCRIPTEN
  //  EM_ASM_({
  //      Module.setStatus('Running...(' + $0 + '/' + $1 + ')');
  //    },val,val_max);
  //#endif
}

void initialize_low(int flags);
std::string replace_str(std::string s, std::string repl, std::string subst);

int call_count=0;

extern bool g_transparent;
extern int g_event_screen_x;
extern int g_event_screen_y;
extern std::string g_platform;

extern bool g_deploy_phase;
extern std::string g_window_href;
extern GameApi::EveryApi *g_everyapi;
void ClearProgress();
#if 1
extern pthread_t g_main_thread_id;
extern Low_SDL_Window *sdl_window;

extern int sprite_screen_width;
extern int sprite_screen_height;

#ifdef ANDROID

static void handle_cmd(struct android_app *app, int32_t cmd) {
  switch(cmd) {
  case APP_CMD_INIT_WINDOW:
    //InitSDL2(800,600,false,false,false,false);
    break;
  case APP_CMD_TERM_WINDOW:
    break;
  }
}

extern "C" void android_main(struct android_app *app) {


  app->onAppCmd = handle_cmd;

#define SDL_MAIN_HANDLED

    // Pass dummy argc and argv to SDL main
  char* argv[] = {"SDL_App", "--file", "script.txt"};
    SDL_SetMainReady();
    SDL_main(3, argv);
}
#endif

#ifdef ANDROID
void popen_curl_init();
#endif

extern std::string g_store_directory;


#ifdef ANDROID
int main(int ac, char *ag[]) {
#else
int main(int argc, char *argv[]) {
#endif
#ifdef ANDROID
  int argc=3;
  char *argv[] = { "./SDLAPP", "--file", "script.txt" };
#endif

#ifdef ANDROID
  popen_curl_init();
#endif

  
#ifdef ANDROID
  static SDLStreamBuf sdlbuf;
  std::cout.rdbuf(&sdlbuf);
#endif
  
#ifdef ANDROID
  SDL_Log("Tero1");  
#endif
  g_main_thread_id = pthread_self();
#ifdef ANDROID
  SDL_Log("Tero2");  
#endif
    
  tasks_init();
#ifdef ANDROID
  SDL_Log("Tero3");  
#endif  
  call_count++;
  //std::cout << "CALL COUNT" << call_count << std::endl;
#ifndef ANDROID
  if (call_count>1) return 0;
#endif
  set_status(1,6);
  //std::cout << "COMMANDLINE ARGS: " << std::endl;
  int s = argc;
  std::vector<std::string> cmd_args;
  for(int i=0;i<s;i++)
    {
      //std::cout << "Arg #" << i << ":" << argv[i] << std::endl;
      cmd_args.push_back(argv[i]);
    }
#ifdef ANDROID
  SDL_Log("Tero4");  
#endif
  Env *e2 = new Env;
  Env &e = *e2;
  EveryApi *ev2 = new EveryApi(e);
  EveryApi &ev = *ev2;
  //EveryApi ev(e);

  g_everyapi = &ev;
  
  int w_width = 800;
  int w_height = 600;
  sprite_screen_width=1200;
  sprite_screen_height=900;
  std::string seamless_url="";
  int current_arg = 1; // start after the current filename
#ifdef ANDROID
  SDL_Log("Tero5");  
#endif
  while(cmd_args.size()-current_arg > 0)
    {
      if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--transparent") {
	if (std::string(cmd_args[current_arg+1])=="1") g_transparent = true;
	current_arg+=2;
      } else
      if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--seamless")
	{
	  seamless_url = cmd_args[current_arg+1];
	  current_arg+=2;
	} else
	if (check_count(cmd_args, current_arg,2) && cmd_args[current_arg]=="--store")
	  {
	    g_store_directory = cmd_args[current_arg+1];
	  }
      if (check_count(cmd_args, current_arg, 3) && cmd_args[current_arg]=="--size")
	{
	  std::string width = cmd_args[current_arg+1];
	  std::string height = cmd_args[current_arg+2];
	  std::stringstream ss(width);
	  ss >> w_width;
	  std::stringstream ss2(height);
	  ss2 >> w_height;
	  std::cout << "Choose size: " << w_width << "x" << w_height << std::endl;
	  current_arg+=3;
	  continue;
	} else
	if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--file")
	  {
#ifdef ANDROID
  SDL_Log("Tero6");  
#endif
  std::string arg = cmd_args[current_arg+1];
#ifdef ANDROID
	    copyAssetToInternal(arg.c_str());
#endif
	    //std::string arg2 = SDL_AndroidGetExternalStoragePath();
	    //SDL_Log("Tero:%s", arg2.c_str());
#ifdef ANDROID
	    SDL_RWops* file = SDL_RWFromFile(arg.c_str(), "r");
	    if (!file)
	      {
		SDL_Log("Tero file==NULL");
	      }
	    Sint64 fileSize = SDL_RWsize(file);

	    std::vector<unsigned char> vec(fileSize+1);
	    SDL_RWread(file, &vec[0],1,fileSize);
	    vec[fileSize]='\0';

	   
	    
	    std::string content = std::string(vec.begin(),vec.end()); //loadAsset(arg.c_str());
	      // Use content...
	      
	      std::stringstream file2(content);
	      std::string res;
	      std::string line;
	      std::string start;
	      int pos =0;
	      while(std::getline(file2,line)) {
		SDL_Log("Tero: %s",line.c_str());
		std::stringstream ss(line);
		ss >> start;
		res+=line+"\n";
		if (start=="RUN" || start=="BLK") {
		  pos = res.size();
		}
	      }
	      if (pos==0) { std::cout << "RUN not found from the script" << std::endl; }
    
  SDL_Log("Tero7");  

#else
	    std::ifstream file(arg.c_str());
	    std::string res;
	    std::string line;
	    std::string start;
	    int pos =0;
	    while(std::getline(file,line)) {
	      std::stringstream ss(line);
	      ss >> start;
	      res+=line+"\n";
	      if (start=="RUN" || start=="BLK") {
		pos = res.size();
	      }
	    }
#ifndef ANDROID
	    if (pos==0) { std::cout << "RUN not found from the script" << std::endl; return 0; }
#endif
#endif

	    
	    res=res.substr(0,pos);
	    code = insert_enter(strip_spaces(decode(res)));
	  code = replace_str(code, "&lt;", "<");
	  code = replace_str(code, "&gt;", ">");
	  code = replace_str(code, "&quot;", "\"");
	  code = replace_str(code, "&apos;", "\'");
	  code = replace_str(code, "&amp;", "&");

	    current_arg+=2;

	    
	  } else
      if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--code")
	{
	  std::cout << "Choosing code!" << std::endl;
	  code = insert_enter(strip_spaces(decode(cmd_args[current_arg+1])));
	  code = replace_str(code, "&lt;", "<");
	  code = replace_str(code, "&gt;", ">");
	  code = replace_str(code, "&quot;", "\"");
	  code = replace_str(code, "&apos;", "\'");
	  code = replace_str(code, "&amp;", "&");


	  current_arg+=2;
	  continue;
	} else
	if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--homepage") {
	  homepageurl = strip_spaces(cmd_args[current_arg+1]);
	  current_arg+=2;
	}
	else
	  if (check_count(cmd_args, current_arg, 1) && cmd_args[current_arg]=="--deploy") {
	    g_deploy_phase = true;
	    current_arg+=1;
	  }
      else
      if (check_count(cmd_args, current_arg, 3) && cmd_args[current_arg]=="--screenshot")
	{
#if 0
	  std::cout << "Screenshot filename: " << cmd_args[current_arg+1] << " at " << cmd_args[current_arg+2] << std::endl;
	  std::string screenshot_filename = cmd_args[current_arg+1];
	  float screenshot_time = 0.0;
	  std::stringstream ss_time(cmd_args[current_arg+2]);
	  ss_time >> screenshot_time;
	  screenshot_filenames.push_back(screenshot_filename);
	  screenshot_times.push_back(screenshot_time);
	  screenshot_done.push_back(false);
#endif
	  current_arg+=3;
	  continue;
	} else
      
      if (cmd_args[current_arg]=="--generate-logo")
	{
	  std::cout << "Generating Logo" << std::endl;
	  ev.mainloop_api.save_logo(ev);
	  exit(0);
	} else
	if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--platform")
	  {
	    std::cout << "Using platform:" << cmd_args[current_arg+1] << std::endl;
	    g_platform = cmd_args[current_arg+1];
	  current_arg+=2;
	  } else
	  if (check_count(cmd_args, current_arg,2) && cmd_args[current_arg]=="--href")
	      {
		std::cout << "Page from server: " << cmd_args[current_arg+1] << std::endl;
		g_window_href = cmd_args[current_arg+1];
		current_arg+=2;
	      }
	else
	{
	  std::cout << "Invalid commandline args" << std::endl;
	  std::cout << "Alternatives are: " << std::endl;
	  std::cout << "   --size 100 200" << std::endl;
	  std::cout << "   --code data" << std::endl;
	  std::cout << "   --generate-logo" << std::endl;
	  std::cout << "   --store dir (for AI features)" << std::endl;
	  exit(0);
	}
    }
#ifdef ANDROID
  SDL_Log("Tero7");  
#endif
  // initialize window
  if (!find_string(code, "low_framebuffer_run") && !find_string(code, "webgpu_window"))
    {
      bool vr_init=true;
      if (!find_string(code, "vr_window")) {
	vr_init=false;
      }
      set_status(2,6);
      ev.mainloop_api.init_window(w_width,w_height,"GameApi", vr_init);
      set_status(3,6);
      g_event_screen_x = w_width; // was -1
      g_event_screen_y = w_height;
#ifdef ANDROID      
  SDL_Log("Tero8");  
#endif
    }
  else
    {
      initialize_low(0);
#ifdef ANDROID      
  SDL_Log("Tero8_fail");  
#endif
    }
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
#ifdef ANDROID      
  SDL_Log("Tero9");  
#endif
  

#ifndef LINUX
           g_low->sdl->SDL_SetWindowSize(sdl_window,g_event_screen_x, g_event_screen_y);
#endif
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glViewport(0,0,g_event_screen_x, g_event_screen_y);

  
  if (!find_string(code, "low_framebuffer_run") && !find_string(code, "webgpu_window"))
    {
      ev.shader_api.load_default();
      set_status(4,6);
    }
  ClearProgress();
  std::pair<int,std::string> blk = mainloop(e, ev);
  set_status(5,6);
#ifdef EMSCRIPTEN
  emscripten_run_script("g_ready_bit=1");
#endif
  if (blk.second == "RUN") {
    RUN r;
    r.id = blk.first;
    ev.blocker_api.run2(ev,r);
  } else if (blk.second=="OK") {
    BLK b;
    b.id = blk.first;
    ev.blocker_api.run(b);
  } else {
  std::cout << blk.second << std::endl;
    std::cout << "ERROR: internal error" << std::endl;
  }
#ifdef ANDROID
  SDL_Log("Tero10 done");  
#endif
}



#endif

