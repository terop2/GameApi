
#include "Serialize.hh"

EventInfo game_start()
{
  EventInfo info;
  info.time = 0.0;
  info.timer = true;
  return info;
}
EventInfo event_timer(int num, float time)
{
  EventInfo info;
  info.time = time;
  info.activation_event_id = num;
  info.timer = true;
  return info;
}
