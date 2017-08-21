
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
  
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x300) { data.forward = true; }
  if ((e.ch=='w'||e.ch==26||e.ch==82) && e.type==0x301) { data.forward = false; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x300) { data.backward = true; }
  if ((e.ch=='s'||e.ch==22||e.ch==81) && e.type==0x301) { data.backward = false; }
  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x300) { data.left = true; }
  if ((e.ch=='a'||e.ch==4||e.ch==80) && e.type==0x301) { data.left = false; }
  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x300) { data.right = true; }
  if ((e.ch=='d'||e.ch==7||e.ch==79) && e.type==0x301) { data.right = false; }


  if ((e.ch=='z'||e.ch=='z'-'a'+4) && e.type==0x300) { data.side_left = true; }
  if ((e.ch=='z'||e.ch=='z'-'a'+4) && e.type==0x301) { data.side_left = false; }
  if ((e.ch=='x'||e.ch=='x'-'a'+4) && e.type==0x300) { data.side_right = true; }
  if ((e.ch=='x'||e.ch=='x'-'a'+4) && e.type==0x301) { data.side_right = false; }

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
}
