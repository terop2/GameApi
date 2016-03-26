#include "GameApi.hh"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <cstring>

using namespace GameApi;

struct Envi {
  EveryApi *ev;
  GuiApi *gui;
  PolygonObj *poly;
  LinesObj *lines;

  std::vector<PT> vec;
  bool changed = true;
  P p;
  LI li;
  LLA la;
  int state = 0;

  W buttons;
  SH sh;
  SH sh2;

  bool ctrl = false;
  std::string filename = "test.tri";
};

W buttonbar(Envi &env)
{
  W w1 = env.gui->button(100,50, 0xff884422, 0xff448822);
  W w2 = env.gui->button(100,50, 0xff8844ff, 0xff4488ff);
  W arr[] = { w1,w2 };
  W array_x = env.gui->array_x(arr, 2, 5);
  return array_x;
}

PT func(int linenum, bool id, EveryApi &ev, const std::vector<PT> &vec)
{
  if (id) {linenum++; }
  while (linenum >=vec.size()) linenum-=vec.size();
  if (!id)
    return vec[linenum];
  else
    {
      PT p = vec[linenum];
      float x = ev.point_api.pt_x(p);
      float y = ev.point_api.pt_y(p);
      float z = ev.point_api.pt_z(p);
      x+=2.0;
      y+=2.0;
      return ev.point_api.point(x,y,z);
    }
}

unsigned int color_func(int linenum, bool id, const std::vector<PT> &vec)
{
  if (linenum==vec.size()-1) { return 0xffff0000; }
  return 0xffffffff;
}

void iter(void *arg)
{
  Envi *env = (Envi*)arg;

    env->ev->mainloop_api.clear();



    if (env->changed)
      {
	if (env->vec.size()==1)
	  {
	    env->p = env->ev->polygon_api.triangle( env->vec[0], env->vec[0], env->ev->point_api.move(env->vec[0], 2.0, 2.0, 0.0) );
	  }
	else if (env->vec.size()==2)
	  {
	    env->p = env->ev->polygon_api.triangle( env->vec[0], env->vec[1], env->vec[0] );

	  }
	else
	  {
	    env->p = env->ev->polygon_api.tri_strip( &env->vec[0], env->vec.size() );
	  }
	env->li = env->ev->lines_api.from_polygon(env->p);
	env->li = env->ev->lines_api.color_function(env->li, std::bind(&color_func, _1, _2, env->vec));
	env->la = env->ev->lines_api.prepare(env->li);
	env->changed = false;
      }
    env->ev->lines_api.render(env->la);

    //env->poly->render();
    //env->lines->render();
    env->ev->shader_api.use(env->sh2);
    env->gui->render(env->buttons);
    env->ev->shader_api.use(env->sh);

    env->ev->mainloop_api.fpscounter();
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = env->ev->mainloop_api.get_event();
    PT mouse = e.cursor_pos;
    int button = e.button;
    int ch = e.ch;
    int type = e.type;
    
    //std::cout << ch << " " << type << std::endl;


    if (e.ch==1073742048 && e.type == 0x300)
      {
	env->ctrl = true;
      }
    if (e.ch==1073742048 && e.type != 0x300)
      {
	env->ctrl = false;
      }
    if (env->ctrl && e.ch==115 && e.type==0x300)
      { // Save.
	std::cout << "Saving..." << env->filename << std::endl;
	//env->ev->mod_api.save(env->mod, "mod.txt");
	TS ts = env->ev->ts_api.from_array(&env->vec[0], env->vec.size());
	env->ev->ts_api.save(ts, env->filename);
      }

#ifndef EMSCRIPTEN
    if (e.ch==27) { exit(0); }
#endif
    if (env->state == 0 && e.button==0)
      {
	env->state = 1;
	env->vec.push_back( e.cursor_pos );
	env->changed = true;
      }
    if (e.button!=0) { env->state=0; }


    env->gui->update(env->buttons, mouse, button, ch, type);


}

int main(int argc, char *argv[]) {
  Env e;
  EveryApi ev(e);

  Envi env;

  if (argc==2)
    {
      char *ptr = argv[1];
      int s = strlen(ptr);
      if (ptr[s-3]=='t' && ptr[s-2]=='r' && ptr[s-1]=='i')
	{
	  std::cout << "Filename: " << ptr << std::endl;
	  env.filename = std::string(ptr);
	}
      else
	{
	  std::cout << "Illegal filename... needs to be .tri" << std::endl;
	}
    }


  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load_default();
  SH sh = ev.shader_api.colour_shader();
  SH sh2 = ev.shader_api.texture_shader();

  GuiApi gui(e, ev, sh2);

  // rest of the initializations
  ev.mainloop_api.init(sh2);
  ev.mainloop_api.init(sh);
  ev.shader_api.use(sh);


  PT center = ev.point_api.point(0.0,0.0,0.0);
  P p3 = ev.polygon_api.sphere(center, 100.0, 30, 30);
  PT pos = ev.point_api.point(0.0,0.0,1.0);
  V u_x = ev.vector_api.vector(1.0, 0.0, 0.0);
  V u_y = ev.vector_api.vector(0.0, 1.0, 0.0);
  LI p3_li = ev.lines_api.render_slice_2d(p3, pos, u_x, u_y);
  LinesObj lines(ev, p3_li, sh);
  PolygonObj poly(ev, p3, sh);
  poly.prepare();
  lines.prepare();

  env.ev = &ev;
  env.poly = &poly;
  env.lines = &lines;
  env.gui = &gui;
  env.sh = sh;
  env.sh2 = sh2;
  env.buttons = buttonbar(env);
  
#ifndef EMSCRIPTEN
  while(1) {
    iter(&env);
    ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&env, 60,1);
#endif



}
