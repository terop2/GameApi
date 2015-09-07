
#include "Serialize.hh"
#include "Graph.hh"
#include "GameApi.hh"
#include "State.hh"

ArrayRender *FindRender(GameApi::Env &e, int bm_id);

struct SpritePriv;

Sprite *sprite_from_handle(GameApi::Env &e, SpritePriv &env, BitmapHandle *handle, int bbm_choose=-1);

struct PolyPriv
{
  std::map<int, ArrayRender*> rend;
  std::map<int, StateBitmaps*> states;
  ~PolyPriv();
};


struct SpritePriv
{
  //ArrayRender rend;
  //MOVED To env: std::map<int, ArrayRender*> renders;
  std::map<int, Sprite*> sprites;
  std::map<int, Sprite*> sprites2;
  std::vector<Sprite**> arrays;
  ~SpritePriv();
};


#if 0
class Sequencer2
{
  // ranges [0..x] are described by two events, 0, and x.
public:
  // time
  virtual int NumEvents() const=0;
  virtual EventInfo Event(int i) const=0;

  virtual int NumLinks() const=0;
  virtual LinkageInfo Linkage(int i) const=0;
  virtual LinkInfo Links(int i) const=0;
  // positions
  virtual int NumPositions() const=0;
  virtual PosInfo Positions(int i) const=0;
  // states
  virtual int NumStates() const=0;
  virtual StateInfo2 States(int i) const=0;
};
#endif

StateBitmaps * PrepareFaceCollPolyHandle(FaceCollPolyHandle *handle, int bbm_choose);


class GameRunner
{
public:
  GameRunner(GameApi::Env &e, Sequencer2 &seq, int start_state) :  seq(seq), start_state(start_state), current_time(0.0),e(e) { }
  void run();
private: 
  int MoveToNextState(int state, float time)
  {
    return state;
  }
  void Alloc()
  {
    int s = seq.NumStates();
    for(int st=0;st<s;st++)
      {
	StateInfo2 si = seq.States(st);
	std::vector<ArrayRender*> *vec = new std::vector<ArrayRender*>;
	state_rends.push_back(vec);
	if (si.enable_obj_array)
	  {
	    int c = si.enable_obj_array->Size();
	    for(int i=0;i<c;i++)
	      {
		vec->push_back(new ArrayRender);
	      }
	  }
	PrepareState(st);
      }
  }
  void PrepareState(int state)
  {
    StateInfo2 st = seq.States(current_state);
    if (st.enable_obj_array)
      for(int i=0;i<st.enable_obj_array->Size();i++)
	{
	  if (st.enable_obj_array->Index(i))
	    { // object number i is enabled in this state
	      PosInfo p = seq.Positions(i);
	      Point pos(0.0,0.0,0.0);
	      int link_id = p.link_id;
	      LinkInfo linkinfo = seq.Links(link_id);
	      FaceCollPolyHandle *polygon_start = linkinfo.polygon_start;
	      //FaceCollPolyHandle *polygon_end = linkinfo.polygon_end;
	      BitmapHandle *bitmap_start = linkinfo.bitmap_start;
	      //BitmapHandle *bitmap_end = linkinfo.bitmap_end;
	      if (polygon_start)
		     {
		       StateBitmaps *state_bm = PrepareFaceCollPolyHandle(polygon_start, 0); // TODO bbm_choose
		       polys.states[i] = state_bm;
		     }
	      else if (bitmap_start)
		{
		  ::Sprite *sprite = sprite_from_handle(e, sprites, bitmap_start, -1);
		  if (!sprite) { std::cout << "preparesprite in running the game\n" << std::endl; return; }
		  PrepareSprite(*sprite, *FindRender(e,i));
		}
	    }
	  
	}
  }
  bool BetweenEvents(const LinkageInfo &li)
  {
    int start_event = li.start_event;
    int end_event = li.end_event;
    EventInfo e = seq.Event(start_event);
    EventInfo e2 = seq.Event(end_event);
    if (e.time < current_time && current_time < e2.time) 
      return true;
    return false;
  }

  void UpdateActivatedEvents(float time_delta, std::vector<int> key_pressed, std::vector<int> key_released)
  {
    //float newtime = time + time_delta;
    int s = seq.NumEvents();
    for(int i=0;i<s;i++)
      {
	//EventInfo ei = seq.Event(i);
	
      }
  }
  float EventInterpolatePos(LinkageInfo li)
  {
    int start_event = li.start_event;
    int end_event = li.end_event;
    EventInfo e = seq.Event(start_event);
    EventInfo e2 = seq.Event(end_event);
    float time = current_time - e.time;
    float pos = time / (e2.time - e.time); // 0.0 .. 1.0
    return pos;

  }

  void RenderState(int state)
  {
    StateInfo2 st = seq.States(current_state);
    if (st.enable_obj_array)
      for(int i=0;i<st.enable_obj_array->Size();i++)
	{
	  if (st.enable_obj_array->Index(i))
	    { // object number i is enabled in this state
	      PosInfo p = seq.Positions(i);
	      Point pos(0.0,0.0,0.0);
	      int link_id = p.link_id;
	      LinkageInfo linkage = seq.Linkage(link_id);
	      if (BetweenEvents(linkage))
		{
		  float val = EventInterpolatePos(linkage);
		  if (p.curve)
		    {
		      std::vector<float> c = p.curve->Count();
		      switch(c.size())
			{
			case 0: // Point
			  {
			pos = p.curve->Map(NULL, 0);
			break;
			  }
			case 1: // Curve : float->Point
			  {
			    pos = p.curve->Map(&val, 1);
			    break;
			  }
			default:
			  {
			    std::cout << "ERROR: bad curve dimension\n" << std::endl;
			    break;
			  }
			};

		    }
		   LinkInfo linkinfo = seq.Links(link_id);
		   FaceCollPolyHandle *polygon_start = linkinfo.polygon_start;
		   //FaceCollPolyHandle *polygon_end = linkinfo.polygon_end;
		   BitmapHandle *bitmap_start = linkinfo.bitmap_start;
		   //BitmapHandle *bitmap_end = linkinfo.bitmap_end;
		   if (polygon_start)
		     {
		       StateBitmaps *state_bm = polys.states[i];
		       state_bm->Render(0); // TODO 0..
		     }
		   else if (bitmap_start)
		     {
		       ::Sprite *sprite = sprites.sprites[i];
		       Point2d p = { pos.x, pos.y };
		       float z = 0.0;
		       Program *prog = 0;
		       RenderSprite(*sprite, 0, p, z, *FindRender(e,i), prog);
		     }
		}
	    }
	}
  }

private:
  Sequencer2 &seq;
  int start_state;

  std::map<int, EventInfo> activated_events;
  //float time;

  float current_time;
  int current_state;
  std::vector< std::vector<ArrayRender*>* > state_rends;
  SpritePriv sprites;
  PolyPriv polys;
  GameApi::Env &e;
};
