#include "GameApi.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <math.h>

#if 0
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <SDL_mixer.h>

using namespace GameApi;

V func(int face, int point, EveryApi &ev)
{
  switch(point)
    {
    case 0: return ev.vector_api.vector(0.0,0.0,0.0);
    case 1: return ev.vector_api.vector(1.0,0.0,0.0);
    case 2: return ev.vector_api.vector(1.0,1.0,0.0);
    case 3: return ev.vector_api.vector(0.0,1.0,0.0);
    };
  return ev.vector_api.vector(0.0,0.0,0.0);
}

PT func2(int face, int point, EveryApi &ev)
{
  switch(point)
    {
    case 0: return ev.point_api.point(0.0,1.0,0.0);
    case 1: return ev.point_api.point(1.0,1.0,0.0);
    case 2: return ev.point_api.point(1.0,0.0,0.0);
    case 3: return ev.point_api.point(0.0,0.0,0.0);
    };
  return ev.point_api.point(0.0,0.0,0.0);
}

BB choose_flash(int id, EveryApi &ev)
{
  switch(id)
    {
    case 0:
    case 4:
      {
      BB flash = ev.bool_bitmap_api.empty(800,600);
      for(int i=0;i<10;i++)
	{
	  flash = ev.bool_bitmap_api.rectangle(flash, i*80, 0, 40, 600);
	}
      return flash;
      }
    case 1:
    case 5:
      {
      BB flash = ev.bool_bitmap_api.empty(800,600);
      for(int i=0;i<10;i++)
	{
	  flash = ev.bool_bitmap_api.rectangle(flash, 0, i*60, 800, 30);
	}
      return flash;
      }
    case 2:
    case 6:
      {
      BM chess = ev.bitmap_api.chessboard(10,10,80,60,0xffffffff, 0);
      BB chess_1 = ev.bool_bitmap_api.from_bitmaps_color(chess,255,255,255);
      return chess_1;
      }
    case 3:
    case 7:
      {
	BB empty = ev.bool_bitmap_api.empty(800,600);
	BB flash = ev.bool_bitmap_api.empty(800,600);
      for(int i=10;i>1;i--)
	{
	  flash = ev.bool_bitmap_api.circle(flash, 400,300, i*40);
	  BB flash2 = ev.bool_bitmap_api.circle(empty, 400,300, i*40-20);
	  flash = ev.bool_bitmap_api.andnot_bitmap(flash, flash2);
	}
      return flash;
      }
    };
}

struct Envi
{
  EveryApi *ev;
  float f;
  float f_start=0;
  int f_start_time=0;
  bool anim_stop = false;
  SH sh;
  SH sh2;
  PolygonObj *poly;
  FBO fbo;
  PolygonObj *poly2;
  bool forward=false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool click = false;
  float pos_x=0.0;
  float pos_z=0.0;
  float rotangle=0.0;
  float speed = 185.0;
  float speed2 = 85.0;
  float rotspeed = 15.0*3.14159*2.0/360.0;
  float time=0.0;
  float old_time=0.0;
  float target_x=0.0;
  float target_y=0.0;
  float target_z=0.0;
  float dir_x = 2.0;
  float dir_y = 0.0;
  float dir_z = 1.0;
  VA score;
  int score_val;
  int score_width;
  std::vector<VA> digits;
  std::vector<int> digit_width;
  std::vector<VA> flash_0;
  std::vector<VA> flash_1;
  int flash_count=0;
  bool flash_0_bool;
  bool flash_1_bool;
  int flash_0_time;
  int flash_1_time;
  bool level_completed=false;
  int completed_time=0;
  VA completed;
  int start_time=0;
  bool logo_shown=true;
};

void render_score(int score, int x, Envi &e)
{
  int xx = x;
  for(int i=0;i<6;i++)
    {
      int sc = score/(pow(10,5-i));
      if (sc<0) sc = -sc;
      int sc2 = sc % 10;
      e.ev->shader_api.set_var(e.sh, "in_MV", e.ev->matrix_api.trans(xx,0.0,0.0));
      e.ev->sprite_api.render_sprite_vertex_array(e.digits[sc2]);
      e.ev->shader_api.set_var(e.sh, "in_MV", e.ev->matrix_api.trans(0.0,0.0,0.0));
      xx+=e.digit_width[sc2];
    }

}

void iter(void *data)
{
  Envi &envi = *(Envi*)data;

  if (envi.logo_shown)
    {
      bool b = envi.ev->mainloop_api.logo_iter();
      if (b) { 
	envi.logo_shown = false; 
#if 1
  envi.ev->tracker_api.play_mp3(".\\Deep_Blue_Sea_Blues.mp3");
#endif
      }
      return;
    }


  envi.f = (envi.ev->mainloop_api.get_time()-envi.start_time)/1000.0;

  envi.time = (envi.ev->mainloop_api.get_time()-envi.start_time)/10.0;
  //envi.f += 1.0/10.0;
  //envi.time +=100.0/10.0;
  envi.speed = envi.time - envi.old_time;
  envi.speed2 = envi.speed*2.0;
  envi.old_time = envi.time;

  //envi.ev->fbo_api.bind_fbo(envi.fbo);
    envi.ev->mainloop_api.clear();
    envi.ev->shader_api.use(envi.sh);
    if (envi.anim_stop)
      {
	envi.ev->shader_api.set_var(envi.sh, "time", 32.0f);
      } else
      {
	envi.ev->shader_api.set_var(envi.sh, "time", envi.f-envi.f_start);
      }
    envi.ev->shader_api.set_var(envi.sh, "time2", envi.pos_x+envi.pos_z);
    envi.ev->shader_api.set_var(envi.sh, "time3", envi.f);
    envi.ev->shader_api.set_var(envi.sh, "rotangle", envi.rotangle);
    envi.ev->shader_api.set_var(envi.sh, "player_pos", envi.pos_x, 0.0, envi.pos_z);
    envi.ev->shader_api.set_var(envi.sh, "target_pos", envi.target_x, envi.target_y, envi.target_z);
    envi.poly->render();

    envi.ev->shader_api.use(envi.sh2);
    envi.ev->sprite_api.render_sprite_vertex_array(envi.score);
    render_score(envi.score_val, envi.score_width, envi);
    if (envi.flash_0_bool)
      {
	//std::cout << "Flash_0" << std::endl;
	envi.ev->sprite_api.render_sprite_vertex_array(envi.flash_0[envi.flash_count]);
	envi.flash_0_time--;
	if (envi.flash_0_time<=0)
	  {
	    envi.flash_0_bool = false;
	  }
      }
    if (envi.flash_1_bool)
      {
	//std::cout << "Flash_1" << std::endl;
	envi.ev->sprite_api.render_sprite_vertex_array(envi.flash_1[envi.flash_count]);
	envi.flash_1_time--;
	if (envi.flash_1_time<=0)
	  {
	    envi.flash_1_bool = false;
	  }
      }
    if (envi.level_completed)
      {
	envi.ev->shader_api.set_var(envi.sh2, "in_MV", envi.ev->matrix_api.trans(100.0, 100.0, 0.0));
	envi.ev->sprite_api.render_sprite_vertex_array(envi.completed);
	envi.ev->shader_api.set_var(envi.sh2, "in_MV", envi.ev->matrix_api.trans(0.0, 0.0, 0.0));
	envi.completed_time--;
	if (envi.completed_time<0) { envi.level_completed = false; envi.start_time += envi.time*10.0; envi.old_time = 0; }

      }
    envi.ev->shader_api.use(envi.sh);


    envi.target_x += envi.dir_x*envi.speed;
    envi.target_z += envi.dir_z*envi.speed;
    envi.target_y = -50.0*cos((envi.target_x-100.0)/300.0*2.0*3.14159/4.0);

    if (envi.target_x>400.0) { envi.dir_x = -2.0; }
    if (envi.target_x<-300.0) { envi.dir_x = 2.0; envi.score_val-=10; 
	      envi.flash_0_time = 20;
	      envi.flash_0_bool = true;
	      //envi.flash_count--; if (envi.flash_count<0) envi.flash_count=0;

    }

    if (envi.target_z>250.0) { envi.dir_z = -1.0; }
    if (envi.target_z<-250.0) { envi.dir_z = 1.0; }

    if (envi.pos_x <-300.0) { envi.pos_x = -300.0; }
    if (envi.pos_x >300.0) { envi.pos_x = 300.0; }
    if (envi.pos_z <-100.0) { envi.pos_z = -100.0; }
    if (envi.pos_z >300.0) { envi.pos_z = 300.0; }
    


    envi.ev->mainloop_api.swapbuffers();
    envi.ev->mainloop_api.finish();

    //std::cout << envi.time << std::endl;
    if (envi.time > 2.0*8000.0 && envi.time < 2.0*8020.0)
      {
	envi.level_completed = true;
	envi.completed_time = 20;
      }


    // handle esc event
    MainLoopApi::Event e;
    while((e = envi.ev->mainloop_api.get_event()).last==true)
      { 
#ifndef EMSCRIPTEN
	if (e.ch==27 && e.type==0x300) { exit(0); }
#endif
	if (e.type==0x300 ||e.type==0x301)
	  {
	    //std::cout << e.ch << " " << e.type << std::endl;
	  }

	if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { envi.forward = true; }
	if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { envi.forward = false; }
	if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { envi.backward = true; }
	if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { envi.backward = false; }
	if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { envi.left = true; }
	if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { envi.left = false; }
	if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { envi.right = true; }
	if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { envi.right = false; }
	if ((e.ch==32) && e.type==0x300) { envi.click = true; }
      }
    envi.f_start_time--;
    if (envi.click)
      {
	//std::cout << "Click" << std::endl;
	//std::cout << "X:" << envi.target_x << " " << -envi.pos_z << std::endl;
	//std::cout << "Z:" << envi.target_z << " " << envi.pos_x << std::endl;
	if (fabs(envi.target_x+envi.pos_z)<120.0)
	  if (fabs(envi.target_z-envi.pos_x)<120.0)
	    {
	      envi.dir_x = 2.0;
	      envi.score_val += 30;
	      envi.flash_1_time = 20;
	      envi.flash_1_bool = true;
	      envi.flash_count++; if (envi.flash_count>5) envi.flash_count=0;
	      //std::cout << "HIT" << std::endl;
	    }
	envi.click = false;
	envi.f_start = envi.f;
	envi.f_start_time = 8;
	envi.anim_stop=false;
      }
    else if (envi.f_start_time<4)
      {
	envi.anim_stop = true;
      }
    if (envi.forward)
      {
	envi.pos_x += envi.speed2*cos(-envi.rotangle+3.14159/2.0);
	envi.pos_z += envi.speed2*sin(-envi.rotangle+3.14159/2.0);
      }
    if (envi.backward)
      {
	envi.pos_x -= envi.speed2*cos(-envi.rotangle+3.14159/2.0);
	envi.pos_z -= envi.speed2*sin(-envi.rotangle+3.14159/2.0);
      }
    if (envi.left)
      {
	envi.rotangle-=envi.rotspeed;
      }
    if (envi.right)
      {
	envi.rotangle+=envi.rotspeed;
      }

}
SFO shader_character(EveryApi &ev, SFO outside)
{
  float k = 15.0;
  PT center = ev.point_api.point(0.0,-30.0,0.0);
  PT hand_center = ev.point_api.point(40.0, 10.0, 0.0);
  PT hand_center2= ev.point_api.point(-40.0, 10.0, 0.0);
  SFO head_1 = ev.sh_api.sphere(center, 30.0);
  SFO head_2 = ev.sh_api.line(0.0, -30.0, 28.0,
			      0.0, -30.0, 50.0,
			      10.0, 3.0);
  SFO head_3 = ev.sh_api.blend(head_1, head_2,k); // or_elem
  PT eye_center1 = ev.point_api.point(10.0, -40.0, 25.0);
  SFO head_4 = ev.sh_api.sphere(eye_center1, 10.0);
  PT eye_center2 = ev.point_api.point(-10.0, -40.0,25.0);
  SFO head_5 = ev.sh_api.sphere(eye_center2, 10.0);
  SFO head_6 = ev.sh_api.blend(head_4, head_5,k);
  SFO head_7 = ev.sh_api.and_not(head_3, head_6);
  SFO head_8 = ev.sh_api.color(head_7, 1.0, 0.0, 1.0, 1.0);
  SFO hat_1 = ev.sh_api.line( 0.0, -50.0, -10.0,
			      0.0, -80.0, -30.0,
			      20.0, 2.0);
  SFO hat_2 = ev.sh_api.blend(head_8, hat_1,k);


  SFO shoulders_1 = ev.sh_api.rounded_cube(-30.0, 30.0, 0.0, 20.0, -20.0, 20.0, 5.0);

  SFO hand_1 = ev.sh_api.rounded_cube(30.0, 50.0, 0.0, 50.0, -10.0, 10.0, 5.0);
  SFO hand_2 = ev.sh_api.rounded_cube(-50.0, -30.0, 0.0, 50.0, -10.0, 10.0, 5.0);
  SFO hand_tip_1 = ev.sh_api.rounded_cube(30.0,50.0, 50.0, 70.0, -10.0, 10.0, 5.0);
  SFO hand_tip_2 = ev.sh_api.rounded_cube(-50.0,-30.0, 50.0, 70.0, -10.0, 10.0, 5.0);

  SFO body_1 = ev.sh_api.rounded_cube(-30.0, 30.0, 20.0, 70.0, -20.0, 20.0, 5.0);
  SFO legs_1 = ev.sh_api.rounded_cube(-30.0, -5.0, 70.0, 100.0, -10.0, 10.0, 5.0);
  SFO legs_2 = ev.sh_api.rounded_cube(5.0,30.0, 70.0, 100.0, -10.0, 10.0, 5.0);

  SFO tip_1 = ev.sh_api.cube(-30.0, -5.0, 90.0, 100.0, -10.0+5.0, 20.0);
  SFO tip_2 = ev.sh_api.cube(5.0,30.0, 90.0, 100.0, -10.0+5.0, 20.0);
  
  SFO all_1 = ev.sh_api.blend(hat_2, shoulders_1,k);
  SFO all_2 = hand_1;
  SFO all_2a = ev.sh_api.blend(all_2, hand_tip_1,k);
  SFO all_2b = ev.sh_api.rot_x(all_2a, 1.5, hand_center);
  SFO all_2c = ev.sh_api.bind_arg(all_2b, "angle", "1.5*cos(time*3.0+3.14159)");
  SFO all_2d = ev.sh_api.color(all_2c, 1.0,0.5,0.0, 1.0);
  SFO all_3 = hand_2;
  SFO all_3a = ev.sh_api.blend(all_3, hand_tip_2,k);
  SFO all_3b = ev.sh_api.rot_x(all_3a, -1.5, hand_center2);
  SFO all_3c = ev.sh_api.bind_arg(all_3b, "angle", "1.5*cos(time*3.0)");
  SFO all_3d = ev.sh_api.color(all_3c, 1.0,0.5,0.0, 1.0);
  SFO all_4 = ev.sh_api.blend(all_3d, body_1,k);
  SFO all_4a = ev.sh_api.blend(all_4, all_1,k);
  SFO all_4b = ev.sh_api.blend(all_4a, all_2d,k);
  SFO all_5 = legs_1;
  SFO all_6 = ev.sh_api.blend(all_5, tip_1,k);
  SFO all_6a = ev.sh_api.trans(all_6, 0.0, 0.0, 10.0);
  SFO all_6b = ev.sh_api.bind_arg(all_6a, "delta", "vec3(0.0,0.0,10.0*cos(time2*3.0))");
  SFO all_6c = ev.sh_api.color(all_6b, 0.0, 1.0, 1.0, 1.0);
  SFO all_7 = legs_2;
  SFO all_8 = ev.sh_api.blend(all_7, tip_2,k);
  SFO all_8a = ev.sh_api.trans(all_8, 0.0, 0.0, 10.0);
  SFO all_8b = ev.sh_api.bind_arg(all_8a, "delta", "vec3(0.0,0.0,10.0*cos(time2*3.0+3.14159))");
  SFO all_8c = ev.sh_api.color(all_8b, 0.0, 1.0, 1.0, 1.0);
  SFO all_9 = ev.sh_api.blend(all_8c, all_4b,k);
  SFO all_10 = ev.sh_api.blend(all_9, all_6c,k);

  PT pt_center = ev.point_api.point(0.0,10.0,0.0);
  SFO bounding_sphere = ev.sh_api.sphere(pt_center, 100.0);
  SFO char_2a = ev.sh_api.bounding_primitive(bounding_sphere, all_10, outside);

  
  SFO all_11 = ev.sh_api.rot_y(all_10, 3.14159/2.0);
  SFO all_12 = ev.sh_api.bind_arg(all_11, "angle", "rotangle");
  SFO all_13 = ev.sh_api.trans(all_12, 0.0,0.0,0.0);
  SFO all_14 = ev.sh_api.bind_arg(all_13, "delta", "player_pos");
  return all_14;
}

SFO background(EveryApi &ev)
{
  SFO tex_cube_1 =  
    ev.sh_api.texture_box(-200.0+0.0,-200.0+800.0,
			  90.0, 105.0,
			  -100.0,100.0);
  SFO tex_cube_11 = ev.sh_api.color(tex_cube_1, 0.0,1.0,1.0,1.0);
  SFO tex_cube_1a = ev.sh_api.rot_x(tex_cube_11, 90.0*3.14159*2.0/360.0);
  SFO tex_cube_1b = ev.sh_api.trans(tex_cube_1a, -100.0, 0.0, -100.0);

  return tex_cube_1a;
}

int main(int argc, char *argv[]) {

  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window(1024,768);

  if (argc==2 && std::string(argv[1])=="--generate-logo")
    {
      std::cout << "Generating Logo" << std::endl;
      ev.mainloop_api.save_logo(ev);
      exit(0);
    }

#if 0
  Mix_Init(MIX_INIT_MP3);
  std::cout << Mix_GetError() << std::endl;
  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
  std::cout << Mix_GetError() << std::endl;
  int c = Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      std::cout << Mix_GetMusicDecoder(i) << std::endl;
    }

  //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
  Mix_Music *mus = Mix_LoadMUS(".\\Deep_Blue_Sea_Blues.mp3");
  std::cout << Mix_GetError() << std::endl;
  Mix_PlayMusic(mus, 1);
  std::cout << Mix_GetError() << std::endl;

#endif
  //return 0;


  FILE *ctt = fopen("CON", "w");
  freopen( "CON", "w", stdout );
  freopen( "CON", "w", stderr );
  printf("HEllo world!\n");


  // shader initialization
  ev.shader_api.load_default(); 
  
  PT center = ev.point_api.point(0.0,0.0,0.0);
  
  SFO skybox = ev.sh_api.sphere(center, 1000.0);
  SFO skybox_2 = ev.sh_api.sphere(center, 990.0);
  SFO skybox_3 = ev.sh_api.and_not(skybox, skybox_2);
  SFO skybox_4 = ev.sh_api.color(skybox_3, 0.7,0.8,1.0, 0.0);




  SFO plane_1 = ev.sh_api.plane(ev.point_api.point(0.0,100.0,0.0),
				ev.vector_api.vector(4.0,0.0,0.0),
				ev.vector_api.vector(0.0,0.0,4.0));
  SFO plane_11 = ev.sh_api.grayscale(plane_1);
  SFO plane_2 = ev.sh_api.or_elem(plane_11, skybox_4);

  SFO plane_2a = ev.sh_api.stop_generation(plane_2);
  SFO char_1 = shader_character(ev, plane_2a);
  SFO char_2 = ev.sh_api.or_elem(plane_2, char_1);

  SFO char_2w = background(ev);
  SFO char_12 = ev.sh_api.or_elem(char_2, char_2w);


  SFO rot_y = ev.sh_api.rot_y(char_12, 0.0);
  SFO rot_y_2 = ev.sh_api.bind_arg(rot_y, "angle", "3.141*1.8/1.25");




  PT ball_center = ev.point_api.point(0.0,0.0,0.0);
  SFO ball = ev.sh_api.sphere(ball_center, 15.0);
  SFO ball_2 = ev.sh_api.trans(ball, 0.0, 0.0, 0.0);
  SFO ball_3 = ev.sh_api.bind_arg(ball_2, "delta", "target_pos");
  SFO ball_comb = ev.sh_api.or_elem(rot_y_2, ball_3);


  PT grow_center = ev.point_api.point(300.0, 0.0, 0.0);
  SFO grow_ball = ev.sh_api.sphere(grow_center, 15.0);
  SFO grow_ball2 = ev.sh_api.bind_arg(grow_ball, "radius", "time3*2.0");
  SFO balls = ev.sh_api.or_elem(ball_comb, grow_ball2);

  SFO rot_y_22 = ev.sh_api.trans(balls, 0.0, 0.0, 0.0);
  SFO rot_y_23 = ev.sh_api.bind_arg(rot_y_22, "delta", "-player_pos.zyx/2.0*vec3(-1.0,1.0,1.0)");
  SFO rot_y_3 = ev.sh_api.stop_generation(rot_y_23);

  SFO color = ev.sh_api.color_from_normal(rot_y_3 );
  SFO mix_color = ev.sh_api.mix_color(rot_y_23, color, 1.0-0.4);



  V light_dir = ev.vector_api.vector(-20.0, -40.0, -20.0);
  SFO soft_shadow = ev.sh_api.soft_shadow(mix_color, light_dir, 0.0, 1.0, 2.0, 8.0);
  SFO ao = ev.sh_api.ambient_occulsion(soft_shadow, 6.2, 50.0);


  SFO sphere_render = ev.sh_api.render(ao);

  SH sh = ev.shader_api.get_normal_shader("screen", "screen", "", "", "", false, sphere_render);
  SH sh2 = ev.shader_api.texture_shader();

  // rest of the initializations
  ev.mainloop_api.init(sh);
  ev.mainloop_api.init(sh2);
  ev.mainloop_api.alpha(true);


  P p = ev.polygon_api.quad_z(0.0, 800.0,
			      0.0, 600.0,
			      0.0);

  P p2 = ev.polygon_api.normal_function(p, std::bind(func,_1,_2,std::ref(ev)));
  P p3 = ev.polygon_api.texcoord_function(p2, std::bind(func2,_1,_2,std::ref(ev)));
 

  // Texture loading
  //Ft font = ev.font_api.newfont("FreeSans.ttf", 240,240);
  //BM bitmap = ev.font_api.font_string(font, "Powerset Functor", 20);
  BM bitmap = ev.bitmap_api.loadbitmap("house.png");
  BB bb = ev.bool_bitmap_api.from_bitmaps_color(bitmap, 255,255,255);
  BB bb2 = ev.bool_bitmap_api.not_bitmap(bb);
  FB dist = ev.float_bitmap_api.distance_field(bb2);
  BM dist_bm = ev.float_bitmap_api.to_grayscale_color(dist, 255,255,255,255, 0,0,0,0);

  TX tex = ev.texture_api.tex_bitmap(dist_bm);
  TXID tex_id = ev.texture_api.prepare(tex);

  PolygonObj poly(ev, p2, sh);
  poly.bind_texture(0,tex_id);
  poly.prepare();
  float f = 0.0;


  Ft font = ev.font_api.newfont("FreeSans.ttf", 30,30);
  BM score = ev.font_api.font_string(font, "Score:", 3);
  int score_width = ev.bitmap_api.size_x(score);
  VA score_va = ev.sprite_api.create_vertex_array(score);
  std::vector<VA> digits;
  std::vector<int> digit_width;
  for(int i=0;i<10;i++)
    {
      BM bm = ev.font_api.glyph(font, '0'+i);
      digits.push_back(ev.sprite_api.create_vertex_array(bm));
      digit_width.push_back(ev.bitmap_api.size_x(bm));
    }

  BM completed = ev.font_api.font_string(font, "Level Completed!", 3);
  VA completed_va = ev.sprite_api.create_vertex_array(completed);

  std::vector<VA> flash_0_vec;
  std::vector<VA> flash_1_vec;
  for(int i=0;i<7;i++)
    {
      BB flash = choose_flash(i, ev);
      BM flash_0 = ev.bool_bitmap_api.to_bitmap(flash, 255,0,0,255,
						0,0,0,0);
      BB not_flash = ev.bool_bitmap_api.not_bitmap(flash);
      BM flash_1 = ev.bool_bitmap_api.to_bitmap(not_flash, 0,255,0,255,
						0,0,0,0);
      
      //CBM flash_0a = ev.cont_bitmap_api.from_bitmap(flash_0, 1.0, 1.0);
      //BM flash_0b = ev.cont_bitmap_api.sample(flash_0a, 800,600);
      //CBM flash_1a = ev.cont_bitmap_api.from_bitmap(flash_1, 1.0, 1.0);
      //BM flash_1b = ev.cont_bitmap_api.sample(flash_1a, 800,600);


      
      VA flash_0_va = ev.sprite_api.create_vertex_array(flash_0);
      VA flash_1_va = ev.sprite_api.create_vertex_array(flash_1);
      flash_0_vec.push_back(flash_0_va);
      flash_1_vec.push_back(flash_1_va);
    }

  Envi envi;
  //envi.fbo = fbo;
  envi.poly = &poly;
  // envi.poly2 = &poly2;
  envi.f = f ;
  envi.sh = sh;
  envi.sh2 = sh2;
  envi.ev = &ev;
  envi.score = score_va;
  envi.score_width = score_width;
  envi.score_val = 1000;
  envi.digits = digits;
  envi.digit_width = digit_width;
  envi.flash_0 = flash_0_vec;
  envi.flash_1 = flash_1_vec;
  envi.flash_0_bool = false;
  envi.flash_1_bool = false;
  envi.completed = completed_va;


#if 1
  ev.mainloop_api.reset_time();
  ev.mainloop_api.display_logo(ev);
  ev.mainloop_api.alpha(true);
  ev.shader_api.use(sh2);
  ev.mainloop_api.switch_to_3d(false, sh, 800,600);
#endif
#ifndef EMSCRIPTEN
  while(1) {
    iter(&envi);
    // ev.mainloop_api.delay(10);
  }
#else
  emscripten_set_main_loop_arg(iter, (void*)&envi, 0,1);
#endif


}
