
#include "GameApi.hh"
#include "GameApi_h.hh"

void GameApi::InteractionApi::wasd_movement(MainLoopApi::Event &e,
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

  if (data.up) { pos_y-=speed_y; }
  if (data.down) { pos_y+=speed_y; }
  if (data.left) { pos_x-=speed_x; }
  if (data.right) { pos_y+=speed_x; } 
}

void GameApi::InteractionApi::quake_movement(MainLoopApi::Event &e,
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
  
  if (data.backward) { pos_y += speed_y; pos_x+=speed_x; }
  if (data.forward) { pos_y -= speed_y; pos_x-=speed_x; }
  if (data.left) { rot_y -= rot_speed; }
  if (data.right) { rot_y += rot_speed; }

  speed_x = speed*cos(rot_y+3.14159/2.0);
  speed_y = speed*sin(rot_y+3.14159/2.0);
}