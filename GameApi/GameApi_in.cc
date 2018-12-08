
#include "GameApi.hh"
#include "GameApi_h.hh"

void GameApi::InteractionApi::wasd_movement_event(MainLoopApi::Event &e,
				   float &pos_x, float &pos_y, Wasd_data &data, float speed_x, float speed_y)
{
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { data.up = true; }
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { data.up = false; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { data.down = true; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { data.down = false; }
  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { data.left = true; }
  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { data.left = false; }
  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { data.right = true; }
  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { data.right = false; }
}

void GameApi::InteractionApi::wasd_movement_frame(MainLoopApi::Event &e,
				   float &pos_x, float &pos_y, Wasd_data &data, float speed_x, float speed_y)
{

  if (data.up) { pos_y-=speed_y; }
  if (data.down) { pos_y+=speed_y; }
  if (data.left) { pos_x-=speed_x; }
  if (data.right) { pos_x+=speed_x; } 
}


void GameApi::InteractionApi::quake_movement_event(EveryApi &ev, MainLoopApi::Event &e,
				    float &pos_x, float &pos_y, float &rot_y, 
					     Quake_data &data, float &speed_x, float &speed_y, float speed, float rot_speed)
{
  GameApi::Env &ee = ev.get_env();
  GameApi::PT cursor_0 = e.cursor_pos;
  Point *cursor_1 = find_point(ee,cursor_0);
  Point cursor = cursor_1?*cursor_1:Point(-1.0,-1.0,-1.0);

  //static Point cursor_old;
  //if (cursor.x>1.0 && cursor.y>1.0) cursor_old=cursor;

  //cursor = cursor_old;
  std::cout << "Quake: " << cursor.x << "," << cursor.y << "," << cursor.z << "::" << e.type << " " << e.button << std::endl;
  int screen_x = ev.mainloop_api.get_screen_width();
  int screen_y = ev.mainloop_api.get_screen_height();
  int scr_x_0 = 0.0;
  int scr_x_1 = screen_x/3.0;
  int scr_x_2 = screen_x*2.0/3.0;
  int scr_x_3 = screen_x;
  int scr_y_0 = 0.0;
  int scr_y_1 = screen_y/3.0;
  int scr_y_2 = screen_y*2.0/3.0;
  int scr_y_3 = screen_y;
  //static bool mv_mode = false;
  if (e.type==1025 && e.button ==0 && cursor.x>scr_x_1 && cursor.x<scr_x_2 && cursor.y>scr_y_1 && cursor.y<scr_y_2) {
    //data.mv_mode = true;
    //std::cout << "mv mode true" << std::endl;
  }
  if (e.type==1026 && e.button==-1) {
    //std::cout << "mv mode false" << std::endl;

    data.mv_mode = false; }
  //data.mv_mode = mv_mode;
  
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { data.forward = true; }
  if ((e.type==1025||e.type==1792) && e.button==0 && cursor.x>scr_x_1 && cursor.x<scr_x_2 && cursor.y>scr_y_0 && cursor.y<scr_y_1) data.forward=true;
  if ((e.type==1026||e.type==1794) && e.button==-1) data.forward=false;
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { data.forward = false; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { data.backward = true; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { data.backward = false; }
  if ((e.type==1025||e.type==1792) && e.button==0 && cursor.x>scr_x_1 && cursor.x<scr_x_2 && cursor.y>scr_y_2 && cursor.y<scr_y_3) data.backward=true;
  if ((e.type==1026||e.type==1794) && e.button==-1) data.backward=false;

  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { data.left = true; }
  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { data.left = false; }

  if (e.type==1025 && e.button==0 && cursor.x>scr_x_0 && cursor.x<scr_x_1 && cursor.y>scr_y_1 && cursor.y<scr_y_2) data.left=true;
  if (e.type==1026 && e.button==-1) data.left=false;


  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { data.right = true; }
  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { data.right = false; }

  if ((e.type==1025||e.type==1792) && e.button==0 && cursor.x>scr_x_2 && cursor.x<scr_x_3 && cursor.y>scr_y_1 && cursor.y<scr_y_2) data.right=true;
  if ((e.type==1026||e.type==1794) && e.button==-1) data.right=false;

  
  if ((e.ch=='z'||e.ch==29) && e.type==0x300) { data.side_left = true; }
  if ((e.ch=='z'||e.ch==29) && e.type==0x301) { data.side_left = false; }

  if ((e.type==1025||e.type==1792) && e.button==0 && cursor.x>scr_x_0 && cursor.x<scr_x_1 && cursor.y>scr_y_2 && cursor.y<scr_y_3) data.side_left=true;
  if ((e.type==1026||e.type==1794) && e.button==-1) data.side_left=false;


  if ((e.ch=='x'||e.ch==27) && e.type==0x300) { data.side_right = true; }
  if ((e.ch=='x'||e.ch==27) && e.type==0x301) { data.side_right = false; }

  if ((e.type==1025||e.type==1792) && e.button==0 && cursor.x>scr_x_2 && cursor.x<scr_x_3 && cursor.y>scr_y_2 && cursor.y<scr_y_3) data.side_right=true;
  if ((e.type==1026||e.type==1794) && e.button==-1) data.side_right=false;


  if (data.mv_mode && e.type==1024)
    {
      //std::cout << "mv mode 1024 event" << std::endl;
    
    if (data.old_pos_x>5.0) {
      //std::cout << "mv mode pos.x>5.0" << std::endl;

	Vector v = cursor-Point(data.old_pos_x,data.old_pos_y,0.0);
	//if (fabs(v.dx)>0.01 || fabs(v.dy)>0.01)
	  {
	  }

	  //std::cout << "mv mode v=" << v.dx << " " << v.dy << std::endl;
	if (v.dx<-0.5) data.left=true;
	if (v.dx>0.5) data.right=true;
	if (v.dy<-0.5) data.forward = true;
	if (v.dy>0.5) data.backward = true;
      }
      data.old_pos_x = cursor.x;
      data.old_pos_y = cursor.y;
      
    }

  //std::cout << data.left << " " << data.right << " " << data.backward << " " << data.forward << std::endl;
  
  
}
void GameApi::InteractionApi::quake_movement_frame(EveryApi &ev, float &pos_x, float &pos_y, float &rot_y, Quake_data &data, float &speed_x, float &speed_y, float speed, float rot_speed)
{
 float delta = ev.mainloop_api.get_delta_time()*7.0;
  if (data.backward) { pos_y += delta*speed_y; pos_x+=delta*speed_x; }
  if (data.forward) { pos_y -= delta*speed_y; pos_x-=delta*speed_x; }
  if (data.left) { rot_y += delta*rot_speed; }
  if (data.right) { rot_y -= delta*rot_speed; }

  if (data.side_left) { pos_y += delta*speed*sin(rot_y); pos_x += delta*speed*cos(rot_y); }
  if (data.side_right) { pos_y += delta*speed*sin(rot_y+3.14159); pos_x += delta*speed*cos(rot_y+3.14159); }

  speed_x = speed*cos(rot_y+3.14159/2.0);
  speed_y = speed*sin(rot_y+3.14159/2.0);
  if (data.mv_mode) {
    data.left=false;
    data.right=false;
    data.forward=false;
    data.backward=false;
  }
}
