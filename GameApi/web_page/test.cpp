#include "GameApi.hh"
#include <sstream>
#include <fstream>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cmath>

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
};

std::string code=
  "PT I1=ev.point_api.point(0.0,0.0,0.0);\n"
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
  "ML I56=ev.mainloop_api.array_ml(std::vector<ML>{I24,I55});\n";

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
ML mainloop(EveryApi &ev, MN &move)
{
  ExecuteEnv e;
  std::pair<int,std::string> p = GameApi::execute_codegen(ev, code, e);
#if 0
  P I1=ev.polygon_api.cube(0.0,100.0,0.0,100.0,0.0,100.0);
  VA I2=ev.polygon_api.create_vertex_array(I1,true);
  ML I3=ev.polygon_api.render_vertex_array_ml(ev,I2);
  MN I4=ev.move_api.empty();
  MN I5=ev.move_api.trans2(I4,0.0,0.0,0.0);
  move = I5;
  ML I6=ev.move_api.move_ml(ev,I3,I5);
#endif
  ML I6;
  I6.id = p.first;
  return I6;
}

void iter(void *arg)
{
  Envi *env = (Envi*)arg;
  if (env->logo_shown)
    {
      bool b = env->ev->mainloop_api.logo_iter();
      if (b) { env->logo_shown = false; }
      return;
    }

    env->ev->mainloop_api.clear_3d(0xff000000);

    //M a_m = env->ev->matrix_api.yrot(env->rot_y+3.14159);
    M a_m2 = env->ev->matrix_api.trans(env->pos_x,0.0,-env->pos_y);
    M a_m3 = env->ev->matrix_api.trans(0.0,0.0,0.0);
    M a_mm = env->ev->matrix_api.mult(a_m3 /*env->ev->matrix_api.mult(a_m3,a_m)*/,a_m2);

    //env->ev->move_api.set_matrix(env->move, a_mm);
    //env->poly->set_rotation_matrix2(a_mm);
    //env->poly->set_pos(env->pos_x, 0.0, env->pos_y);
    //env->poly->render();

    


    // handle esc event
    MainLoopApi::Event e;
    while((e = env->ev->mainloop_api.get_event()).last==true)
      {
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif

	InteractionApi::quake_movement_event(*env->ev,e, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e);

      }

    InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
				   env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);

    M mat = env->ev->matrix_api.identity();
	env->ev->shader_api.use(env->color_sh);
	env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
	env->ev->shader_api.use(env->texture_sh);
	env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->arr_texture_sh);
	env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
	env->ev->shader_api.use(env->color_sh);

	M in_MV = env->ev->mainloop_api.in_MV(*env->ev, true);
	M in_T = env->ev->mainloop_api.in_MV(*env->ev, true);
	M in_N = env->ev->mainloop_api.in_MV(*env->ev, true);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N);


    env->ev->mainloop_api.fpscounter();
    // swapbuffers

    env->ev->mainloop_api.swapbuffers();


    int s = screenshot_done.size();
    for(int i=0;i<s;i++)
      {
	if (screenshot_done[i] == false && env->ev->mainloop_api.get_time() > screenshot_times[i])
	  {
	    BM img = env->ev->mainloop_api.screenshot();
	    env->ev->bitmap_api.savebitmap(img, screenshot_filenames[i], false);
	    screenshot_done[i] = true;
	    std::ifstream ss(screenshot_filenames[i], std::ios::binary);
	    char ch;
	    std::string data;
	    while(ss.get(ch)) { data+=ch; }
	    std::string hex_data = hexify5(data);
	    std::stringstream ss2;
	    ss2 << i;
	    std::string request_params = "user=terop&key=" + ss2.str() + "&data=" + hex_data;
#ifdef EMSCRIPTEN
	    emscripten_async_wget2("save_screenshot.php", "tmp.txt", "POST", request_params.c_str(), 0, &onload, &onerror, &onprogress);
#endif
	  }
      }


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
      if (c!='\n') s+='\n';
    }
  return s;
}
bool check_count(std::vector<std::string> cmd_args, int current_arg, int count)
{
  return cmd_args.size()-current_arg >= count;
}

int main(int argc, char *argv[]) {
  std::cout << "COMMANDLINE ARGS: " << std::endl;
  int s = argc;
  std::vector<std::string> cmd_args;
  for(int i=0;i<s;i++)
    {
      std::cout << "Arg #" << i << ":" << argv[i] << std::endl;
      cmd_args.push_back(argv[i]);
    }
  Env e;
  EveryApi ev(e);

  Envi env;
  int w_width = 800;
  int w_height = 600;
  int current_arg = 1; // start after the current filename
  while(cmd_args.size()-current_arg > 0)
    {
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
      if (check_count(cmd_args, current_arg, 2) && cmd_args[current_arg]=="--code")
	{
	  std::cout << "Choosing code!" << std::endl;
	  code = insert_enter(strip_spaces(decode(cmd_args[current_arg+1])));
	  current_arg+=2;
	  continue;
	} else
      if (check_count(cmd_args, current_arg, 3) && cmd_args[current_arg]=="--screenshot")
	{
	  std::cout << "Screenshot filename: " << cmd_args[current_arg+1] << " at " << cmd_args[current_arg+2] << std::endl;
	  std::string screenshot_filename = cmd_args[current_arg+1];
	  float screenshot_time = 0.0;
	  std::stringstream ss_time(cmd_args[current_arg+2]);
	  ss_time >> screenshot_time;
	  screenshot_filenames.push_back(screenshot_filename);
	  screenshot_times.push_back(screenshot_time);
	  screenshot_done.push_back(false);
	  current_arg+=3;
	  continue;
	} else
      
      if (cmd_args[current_arg]=="--generate-logo")
	{
	  std::cout << "Generating Logo" << std::endl;
	  ev.mainloop_api.save_logo(ev);
	  exit(0);
	} else
	{
	  std::cout << "Invalid commandline args" << std::endl;
	  std::cout << "Alternatives are: " << std::endl;
	  std::cout << "   --size 100 200" << std::endl;
	  std::cout << "   --code data" << std::endl;
	  std::cout << "   --generate-logo" << std::endl;
	  exit(0);
	}
    }

  // initialize window
  ev.mainloop_api.init_window(w_width,w_height);

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.texture_shader();
  SH sh3 = ev.shader_api.texture_array_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);
  ev.mainloop_api.init_3d(sh2);
  ev.mainloop_api.init_3d(sh3);
  ev.shader_api.use(sh);



  //P p3 = ev.polygon_api.cube(0.0, 100.0, 0.0, 100.0, 0.0, 100.0);
#if 0
  PT center1 = ev.point_api.point(0.0,0.0,0.0);
  PT center2 = ev.point_api.point(30.0,0.0,50.0);
  //P p1 = ev.polygon_api.sphere(center1, 50.0, 30*2*2, 15*2*2);
  float cs = 40.0;

  BO p1 = ev.bool_api.cube(ev, -cs,cs, -cs,cs, -cs,cs, 8, 8);
  BO p2 = ev.bool_api.sphere(ev, center1, 50.0, 30*2*2, 30*2*2);
  BO p3bo = ev.bool_api.and_not(ev, p1, p2);
  BO p22 = ev.bool_api.sphere(ev, center2, 40.0, 30*2, 30*2);
  BO b22bo = ev.bool_api.and_not(ev, p3bo, p22);
  P p3 = ev.bool_api.to_polygon(b22bo);
  //O p3o = ev.bool_api.to_volume(b22bo);
  //P p3 = ev.volume_api.rendercubes3(p3o, 80, 80, 80,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0,
  // 				    -100.0, 100.0);

  P p31 = ev.polygon_api.recalculate_normals(p3);
  P p32 = ev.polygon_api.color_from_normals(p31);

  PolygonObj poly(ev, p32, sh);
  poly.prepare();

#endif
  ML ml = mainloop(ev, env.move);
  MN mn0 = ev.move_api.empty();
  MN mn = ev.move_api.trans2(mn0, 0.0, 0.0, -400.0);
  ML ml2 = ev.move_api.move_ml(ev, ml, mn);
  env.mainloop = ml2;

  env.ev = &ev;
  //env.poly = &poly;
  env.color_sh = sh;
  env.texture_sh = sh2;
  env.arr_texture_sh = sh3;

  ev.mainloop_api.reset_time();
  if (env.logo_shown)
    ev.mainloop_api.display_logo(ev);
  ev.mainloop_api.alpha(true);

#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    //ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 0,1);
#endif



}
