
#include "GameApi_h.hh"

EXPORT GameApi::EventApi::EventApi(Env &e) : e(e) 
{
  priv = (void*)new EventPriv;
}

GameApi::E GameApi::EventApi::root_event()
{
  EventInfo i = ::game_start();
  return add_event(e, i);
}
GameApi::E GameApi::EventApi::timer(GameApi::E activation_event, float time)
{
  EventInfo i = ::event_timer(activation_event.id, time);
  return add_event(e, i);  
}

GameApi::L GameApi::EventApi::polygon(GameApi::E start, GameApi::E end, GameApi::P poly)
{
#if 0
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = env->poly[poly.id];
  link.polygon_end = env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
#endif
  GameApi::L l;
  l.id = 0;
  return l;
}
GameApi::L GameApi::EventApi::bitmap(GameApi::E start, GameApi::E end, GameApi::BM start_bitmap, BM end_bitmap)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
#if 0
  LinkInfo link;
  link.start_event_id = start.id;
  link.end_event_id = end.id;
  link.polygon_start = NULL; //env->poly[poly.id];
  link.polygon_end = NULL; //env->poly[poly.id];
  //PosInfo pos;
  return add_link(e, start, end, link);
#endif
  GameApi::L l;
  l.id = 0;
  return l;
}

enum Classify { EBefore, ECurrent, EFuture };
float event_time(Sequencer2 &seq, int e1)
{
  EventInfo i = seq.Event(e1);
  float time = 0.0;
  if (i.time_origin_id != -1)
    time = event_time(seq, i.time_origin_id);
  return i.time + time;
}
struct DynEventInfo
{
  bool currently_active;
};

Classify classify_event(Sequencer2 &seq, int e1, int e2, float time, int last_down_key, int last_up_key, std::vector<DynEventInfo> &vec)
{
  EventInfo i = seq.Event(e1);
  if (i.activation_event_id != -1 && i.deactivation_event_id != -1)
    {
      Classify ac_classa = classify_event(seq, i.activation_event_id, i.deactivation_event_id, time, last_down_key, last_up_key, vec);
      if (ac_classa == EBefore) return EBefore;
      if (ac_classa == EFuture) return EFuture;
    }
  if (i.timer)
    {
      float event1_time = event_time(seq,e1);
      float event2_time = event_time(seq,e2);
      if (time < event1_time) return EBefore;
      if (time > event2_time) return EFuture;
      return ECurrent;
    }
  if (i.key_down)
    {
      if (last_down_key == i.key_value)
	{
	}
    }
  if (i.key_up)
    {
    }
  if (i.activate_mouse)
    {
    }
  if (i.deactivate_mouse)
    {
    }
  return EBefore;
}



void GameApi::EventApi::run_game(GameApi::ST st, int start_state)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StateRange r = env->state_ranges[st.id];
  
  AllStatesSequencer seq(env->event_infos, &env->states[r.start_range], r.range_size);
  GameRunner game(e,seq, start_state);
  game.run();

#if 0
  EventInfo ei = find_event_info(e, ee);
  
  LinkageInfo li = find_linkage(e, l);
  LinkInfo link = find_link_info(e, l);
  PosInfo pos = find_pos_info(e, l);

#endif
}

#if 0
GameApi::MV GameApi::EventApi::point(float x, float y, float z)
{
  return add_mv_point(e, x,y,z);
}
GameApi::MV GameApi::EventApi::line(GameApi::E start, GameApi::E end, GameApi::MV start_mv, GameApi::MV end_mv, int start_choose, int end_choose)
{
  return add_line(e, start, end, start_mv, end_mv, start_choose, end_choose); 
}
GameApi::LL GameApi::EventApi::link(GameApi::L obj, GameApi::MV move)
{
  return add_pos(e, obj, move);
}
std::string GameApi::EventApi::Serialize(ST states, int start_state)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  StateRange r = env->state_ranges[states.id];
  
  AllStatesSequencer seq(env->event_infos, &env->states[r.start_range], r.range_size);
  SequencerParser parser;
  return parser.Create(&seq);
}
#endif

#if 0
ST GameApi::EventApi::UnSerialize(std::string s)
{
  SequencerParser parser;
  bool success = true;
  Sequencer2 *seq = parser.Parse(s, success);
  std::vector<E> vec;
  int c = seq->NumEvents();
  for(int i=0;i<c;i++)
    {
      E event = { 0 };
      EventInfo info = seq->Event(i);
      if (info.timer) { 
	E activation_event = { info.activation_event_id };
	event = timer(info.activation_event, time);
      }
      if (info.key_down) {
	
      }
      if (info.key_up) {
      }
      if (info.activate_mouse) {
      }
      if (info.deactivate_mouse) {
      }
      if (info.state_change) {
      }
      vec.push_back(event);
    }
  
  int c2 = seq->NumLinks();
  std::vector<L> vec;
  for(int a=0;a<c2;a++)
    {
      LinkageInfo linkage = seq->Linkage(a);
      LinkInfo link = seq->Links(a);
      E start = vec[linkage.start_event];
      E end = vec[linkage.end_event];
      L link = { 0 };
      if (link.polygon_start || link.polygon_end)
	{
	  link = polygon(start, end, polygon_start, polygon_end);
	}
      if (link.bitmap_start || link.bitmap_end)
	{
	  link = bitmap(start, end, add_bitmap(e,link.bitmap_start), add_bitmap(link.bitmap_end));
	}
      vec.push_back(link);
    }
  int c3 = seq->NumPositions();
  for(int b=0;b<c3;b++)
    {
      PosInfo pos = seq->Positions(b);
      // TODO, seems curve is not possible to unserialize using this api
    }

}
#endif
EXPORT GameApi::EventApi::~EventApi() 
{
  delete (EventPriv*)priv;
}

GameApi::ST GameApi::EventApi::states(int count_states)
{
	::EnvImpl *env =  ::EnvImpl::Environment(&e);
  int sz = count_states;
  StateInfo2 info;
  info.enable_obj_array = new DuplicateArray<int, bool>(1,false);
  info.plane = 0;
  int start = env->states.size();
  for(int i=0;i<sz;i++)
    {
      env->states.push_back(info);
    }
  int end = env->states.size();
  int count = end - start;
  StateRange r;
  r.start_range = start;
  r.range_size = count;
  env->state_ranges.push_back(r);
  GameApi::ST st;
  st.id = env->state_ranges.size()-1;
  return st;
}
