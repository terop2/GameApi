
#ifndef SERIALIZE_HH
#define SERIALIZE_HH

#include "Graph.hh"

struct FaceCollPolyHandle;
struct BitmapHandle;

struct EventInfo
{
  EventInfo() : time(0.0), activation_event_id(-1), deactivation_event_id(-1), time_origin_id(-1), key_value(-1), timer(false), key_down(false), key_up(false), activate_mouse(false), deactivate_mouse(false) { }
  float time;
  int activation_event_id; // -1 if not used
  int deactivation_event_id; // -1 if not used
  int time_origin_id; // -1 if global
  int key_value; // -1 if not used
  int state_change_id; // -1 if not used
  bool timer;
  bool key_down;
  bool key_up;
  bool activate_mouse;
  bool deactivate_mouse;
  bool state_change;
  bool padding1;
  bool padding2;
};
class MousePlane;
class Sequencer;

EventInfo game_start();
EventInfo event_timer(int activation_id, float time);
EventInfo event_key_down(int key);
EventInfo event_key_up(int key);
EventInfo event_key_down(int key, int start_id, int end_id);
EventInfo event_key_up(int key, int start_id, int end_id);
EventInfo event_ac_mouse_plane(int activation_id, MousePlane &pl);
EventInfo event_deac_mouse_plane(int deactivation_id, MousePlane &pl);
Sequencer *event_single(EventInfo &info);
Sequencer *event_pair(EventInfo *i1, EventInfo *i2);
Sequencer *event_array(EventInfo *array, int size);

struct LinkageInfo
{
  int start_event;
  int end_event;
};

class Sequencer
{
public:
  virtual int NumEvents() const=0;
  virtual EventInfo Event(int i) const=0;
  virtual int NumLinks() const=0;
  virtual LinkageInfo Linkage(int i) const=0;
};

class EmptySequencer : public Sequencer
{
public:
  virtual int NumEvents() const { return 0; }
  virtual EventInfo Event(int i) const { return EventInfo(); }
  virtual int NumLinks() const { return 0; }
  virtual LinkageInfo Linkage(int i) const { return LinkageInfo(); }
};

class NextSequencer : public Sequencer
{
public:
  NextSequencer(Sequencer &seq, EventInfo next) : seq(seq), next(next) { }
  virtual int NumEvents() const { return seq.NumEvents()+1; }
  virtual EventInfo Event(int i) const
  {
    if (i==0) return next;
    return seq.Event(i-1);
  }
  virtual int NumLinks() const { return 0; }
  virtual LinkageInfo Linkage(int i) const { return LinkageInfo(); }
private:
  Sequencer &seq;
  EventInfo next;
};

class PairSequencer : public Sequencer
{
public:
  PairSequencer(Sequencer &seq, EventInfo e1, EventInfo e2) : seq(seq), e1(e1), e2(e2) { }
  virtual int NumEvents() const { return seq.NumEvents()+2; }
  virtual EventInfo Event(int i) const
  {
    if (i==0) return e1;
    if (i==1) return e2;
    return seq.Event(i-2);
  }
  virtual int NumLinks() const { return seq.NumLinks()+1; }
  virtual LinkageInfo Linkage(int i) const 
  {
    if (i==0) { LinkageInfo ii; ii.start_event = 0; ii.end_event=1; return ii; }
    return seq.Linkage(i-1);
  }
  
private:
  Sequencer &seq;
  EventInfo e1,e2;
};

class ArraySequencer : public Sequencer
{
public:
  ArraySequencer(Array<int,EventInfo> &arr) : arr(arr) { }
  virtual int NumEvents() const { return arr.Size(); }
  virtual EventInfo Event(int i) const { return arr.Index(i); }
  virtual int NumLinks() const { return arr.Size()-1; }
  virtual LinkageInfo Linkage(int i) const 
  { 
    LinkageInfo ii;
    ii.start_event = i;
    ii.end_event = i+1;
    return ii;
  }
private:
  Array<int,EventInfo> &arr;
};
struct FunctionImpl;

struct LinkInfo
{ // either start or end is NULL in each event
  LinkInfo() : start_event_id(-1), end_event_id(-1), polygon_start(NULL), polygon_end(NULL)/*, bitmap_start(NULL), bitmap_end(NULL), function1(NULL), scale_func1(false), function2(NULL), scale_func2(false), function3(NULL), scale_func3(false), function4(NULL), scale_func4(false)*/, polygon_posx_func(-1),
	       polygon_posy_func(-1), polygon_posz_func(-1),
	       bitmap_posx_func(-1), bitmap_posy_func(-1),
	       bitmap_posz_func(-1), polygon_xrot_func(-1),
	       polygon_yrot_func(-1), polygon_zrot_func(-1),
	       sound_channel_func(-1) { }
  int start_event_id; // EventInfo
  int end_event_id;
  FaceCollPolyHandle *polygon_start;
  FaceCollPolyHandle *polygon_end;
  // NOTE: PADDING PROBLEMS WITH BOOLS.
  BitmapHandle *bitmap_start;
  BitmapHandle *bitmap_end;
#if 0
  FunctionImpl *function1;
  bool scale_func1; // whether it's between [start,end] or [start,start+funclength]
  FunctionImpl *function2;
  bool scale_func2;
  FunctionImpl *function3;
  bool scale_func3;
  FunctionImpl *function4;
  bool scale_func4;
#endif
  int polygon_posx_func;  
  int polygon_posy_func;
  int polygon_posz_func;
  int bitmap_posx_func;
  int bitmap_posy_func;
  int bitmap_posz_func;
  int polygon_xrot_func;
  int polygon_yrot_func;
  int polygon_zrot_func;
  int sound_channel_func;
};

class SeqWithLinks
{
public:
  virtual int NumEvents() const=0;
  virtual EventInfo Event(int i) const=0;
  virtual int NumLinks() const=0;
  virtual LinkageInfo Linkage(int i) const=0;
  virtual LinkInfo Links(int i) const=0;
};

class MousePlane : public Bitmap<EventInfo>
{
public:
};

class MouseRectanglePlane : public MousePlane
{
public:
  MouseRectanglePlane(MousePlane *next, int sx, int sy) : next(next), sx(sx), sy(sy) { }
  void push_back(int x, int y, int width, int height, EventInfo *info)
  {
    Rect r;
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    r.info = info;
    vec.push_back(r);
  }
public:
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual EventInfo Map(int x, int y) const
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Rect r = vec[i];
	if (x<r.x) continue;
	if (y<r.y) continue;
	    if (x>r.x+r.width) continue;
	    if (y>r.y+r.height) continue;
	    return *r.info;
      }
    return next->Map(x,y);
  }

private:
  struct Rect { int x,y,width,height; EventInfo *info; };
  std::vector<Rect> vec;
  MousePlane *next;
  int sx,sy;
};

struct PointInfo
{
  bool is_start;
  Point p;
};
struct LineInfo
{
  bool is_start;
  Point start_point;
  Point end_point;
  EventInfo *start_event;
  EventInfo *end_event;
};

struct PosInfo_old
{
  bool is_point;
  PointInfo point;
  LineInfo line;
};
struct PosInfo
{
  int link_id;
  //CurveIn3d *curve;
  NDim<float, Point> *curve;
};
struct StateInfo2
{
  Array<int, bool> *enable_obj_array; // dimension = numpositions, int=LL.id
  MousePlane *plane;
};
struct StateRange
{
  int start_range;
  int range_size;
};

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
  virtual ~Sequencer2() { }
};

class EmptySequencer2 : public Sequencer2
{
public:
  virtual int NumEvents() const { return 0; }
  virtual EventInfo Event(int i) const { return EventInfo(); }
  virtual int NumLinks() const { return 0; }
  virtual LinkageInfo Linkage(int i) const { return LinkageInfo(); }
  virtual LinkInfo Links(int i) const { return LinkInfo(); }
  virtual int NumPositions() const { return 0; }
  virtual PosInfo Positions(int i) const { return PosInfo(); }
  // states
  virtual int NumStates() const { return 0; }
  virtual StateInfo2 States(int i) const { return StateInfo2(); }
};

class Sequencer2Impl : public Sequencer2
{
public:
  Sequencer2Impl(Sequencer2 *seq) : seq(seq) { }
  virtual int NumEvents() const { return seq->NumEvents(); }
  virtual EventInfo Event(int i) const { return seq->Event(i); }
  virtual int NumLinks() const { return seq->NumLinks(); }
  virtual LinkageInfo Linkage(int i) const { return seq->Linkage(i); }
  virtual LinkInfo Links(int i) const { return seq->Links(i); }
  virtual int NumPositions() const { return seq->NumPositions(); }
  virtual PosInfo Positions(int i) const { return seq->Positions(i); }
  virtual int NumStates() const { return seq->NumStates(); }
  virtual StateInfo2 States(int i) const { return seq->States(i); }

protected:
  Sequencer2 *seq;
};

class SingleEvent : public Sequencer2Impl
{
public:
  SingleEvent(Sequencer2 *seq2, EventInfo info) : Sequencer2Impl(seq2), seq(seq2), info(info) { }
  int CurrentEventNum() const { return seq->NumEvents(); }
  virtual int NumEvents() const { return seq->NumEvents()+1; }
  virtual EventInfo Event(int i) const
  {
    if (i<seq->NumEvents()) return seq->Event(i);
    return info;
  }
private:
  Sequencer2 *seq;
  EventInfo info;
};

class SingleLink : public Sequencer2Impl
{
public:
  SingleLink(Sequencer2 *seq2, LinkageInfo linkage, LinkInfo info)
    : Sequencer2Impl(seq2), seq(seq2), linkage(linkage), info(info) { }
  int CurrentLinkNum() const { return seq->NumLinks(); }

  virtual int NumLinks() const { return seq->NumLinks()+1; }
  virtual LinkageInfo Linkage(int i) const
  {
    if (i<seq->NumLinks()) return seq->Linkage(i);
    return linkage;
  }
  virtual LinkInfo Links(int i) const
  {
    if (i<seq->NumLinks()) return seq->Links(i);
    return info;
  }
private:
  Sequencer2 *seq;
  LinkageInfo linkage;
  LinkInfo info;
};

class SinglePos : public Sequencer2Impl
{
public:
  SinglePos(Sequencer2 *seq, PosInfo pos)
    : Sequencer2Impl(seq), pos(pos) { }
  int CurrentPosNum() const { return seq->NumPositions(); }
  virtual int NumPositions() const { return Sequencer2Impl::NumPositions()+1; }
  virtual PosInfo Positions(int i) const
  {
    if (i<Sequencer2Impl::NumPositions()) return Sequencer2Impl::Positions(i);
    return pos;
  }
private:
  PosInfo pos;
};
class AllStatesSequencer : public Sequencer2Impl
{
public:
  AllStatesSequencer(Sequencer2 *next, StateInfo2 *array, int size)
    : Sequencer2Impl(next), array(array), size(size) { }
  int NumStates() const { return size; }
  StateInfo2 States(int i) const { return array[i]; }
private:
  StateInfo2 *array;
  int size;
};

#endif
