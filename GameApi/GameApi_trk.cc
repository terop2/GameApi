#include "GameApi_h.hh"

class EmptyTracker : public Tracker
{
public:
  EmptyTracker(int channels, int timeslots) : channels(channels), timeslots(timeslots) { }
  virtual int NumChannels() const { return channels; }
  virtual int NumTimeSlots() const { return timeslots; }
  virtual int Type(int channel, int timeslot) const { return 0; }
  virtual int Duration(int channel, int timeslot) const { return -1; }
  virtual int Sample(int channel, int timeslot) const { return -1; }
  virtual int NumData(int channel, int timeslot) const { return 0; }
  virtual unsigned int Data(int channel, int timeslot, int data) const { return 0; }
private:
  int channels;
  int timeslots;
};
EXPORT GameApi::TRK GameApi::TrackerApi::empty(int numchannels, int numslots)
{
  return add_tracker(e, new EmptyTracker(numchannels, numslots));
}

class AudioTracker : public Tracker
{
public:
  AudioTracker(Tracker *next, int channel, int timeslot, int duration, int sample) : next(next), channel(channel), timeslot(timeslot), duration(duration), sample(sample) { }
  virtual int NumChannels() const { return next->NumChannels(); }
  virtual int NumTimeSlots() const { return next->NumTimeSlots(); }
  virtual int Type(int cha, int slot) const 
  {
    if (cha==channel && slot == timeslot)
      return 1; 
    return next->Type(cha, slot);
  }
  virtual int Duration(int cha, int slot) const 
  { 
    if (cha==channel && slot==timeslot)
      {
	return duration;
      }
    return next->Duration(cha,slot); 
  }
  virtual int Sample(int cha, int slot) const 
  {
    if (cha==channel && slot==timeslot)
      return sample;
    return next->Sample(cha,slot);
  }
  virtual int NumData(int channel, int timeslot) const { return next->NumData(channel, timeslot); }
  virtual unsigned int Data(int channel, int timeslot, int data) const { return next->Data(channel,timeslot,data); }; 
private:
  Tracker *next;
  int channel;
  int timeslot;
  int duration;
  int sample;
};

EXPORT GameApi::TRK GameApi::TrackerApi::audio_slot(TRK orig, int channel, int slot, int duration, int sample)
{
  Tracker *next = find_tracker(e, orig);
  return add_tracker(e, new AudioTracker(next, channel, slot, duration, sample));
}
class ArrayTracker : public Tracker
{
public:
  ArrayTracker(Tracker **array, int size) : array(array), size(size) { }
  virtual int NumChannels() const 
  {
    int s = size;
    int channels = 0;
    for(int i=0;i<s;i++)
      {
	channels = std::max(channels, array[i]->NumChannels());
      }
    return channels; 
  }
  virtual int NumTimeSlots() const { 
    int s = size;
    int timeslots = 0;
    for(int i=0;i<s;i++)
      {
	timeslots += array[i]->NumTimeSlots();
      }
    return timeslots; 
  }
  virtual int Type(int channel, int timeslot) const { 
    std::pair<int,int> p = find_array_index(timeslot);
    return array[p.first]->Type(channel,p.second);
  }
  virtual int Duration(int channel, int timeslot) const {
    std::pair<int,int> p = find_array_index(timeslot);
    return array[p.first]->Duration(channel,p.second);
  }
  virtual int Sample(int channel, int timeslot) const { 
    std::pair<int,int> p = find_array_index(timeslot);
    return array[p.first]->Sample(channel,p.second);
  }
  virtual int NumData(int channel, int timeslot) const { 
    std::pair<int,int> p = find_array_index(timeslot);
    return array[p.first]->NumData(channel,p.second);

  }
  virtual unsigned int Data(int channel, int timeslot, int data) const { 
    std::pair<int,int> p = find_array_index(timeslot);
    return array[p.first]->Data(channel,p.second,data);
  }; 

  std::pair<int,int> find_array_index(int timeslot) const
  {
    int s = size;
    int pos = 0;
    for(int i=0;i<s;i++)
      {
	int oldpos = pos;
	pos+=array[i]->NumTimeSlots();
	if (pos>=timeslot)
	  {
	    return std::make_pair(i,timeslot-oldpos);
	  }
      }
    return std::make_pair(0,0);
  }
private:
  Tracker **array;
  int size;
};

EXPORT GameApi::TRK GameApi::TrackerApi::array(TRK *array, int size)
{
  std::vector<Tracker*> *vec = new std::vector<Tracker*>;
  for(int i=0;i<size;i++)
    {
      vec->push_back(find_tracker(e,array[i]));
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  return add_tracker(e, new ArrayTracker(&(*vec)[0], size));
}
EXPORT GameApi::TBUF GameApi::TrackerApi::prepare(TRK trk)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Tracker *tracker = find_tracker(e, trk);
  int s1 = tracker->NumChannels();
  int s2 = tracker->NumTimeSlots();
  TBuffer2 *buf = new TBuffer2[s1*s2];
  for(int y=0;y<s2;y++)
    for(int x=0;x<s1;x++)
      {
	TBuffer2 &elem = buf[x+y*s1];
	elem.type = tracker->Type(x,y);
	elem.duration = tracker->Duration(x,y);
	elem.sample = tracker->Sample(x,y);
      }


  TBuffer buf0;
  buf0.numchannels = s1;
  buf0.numslots = s2;
  buf0.buf = buf;
  env->tracker_buf.push_back(buf0);
  TBUF b;
  b.id = env->tracker_buf.size()-1;
  return b;
}

EXPORT void GameApi::TrackerApi::play_song(EveryApi &ev, TBUF buf, WAV samples, int framenum, int speed)
{
  int slot = framenum / speed;
  int delta = framenum - slot*speed;
  if (delta!=0) return;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Wavs &w = env->wavs[samples.id];
  TBuffer &bf = env->tracker_buf[buf.id];
  if (slot>=bf.numslots) return;

  int s = bf.numchannels;
  for(int i=0;i<s;i++)
    {
      TBuffer2 &b = bf.buf[i+bf.numchannels*slot];
      if (b.type==1) {
	std::cout << "play_sample_1" << " " << i <<" " << b.sample << std::endl;
	ev.sample_api.play_sample_1(i, samples, b.sample);
      }
    }
}

void GameApi::TrackerApi::play_mp3(std::string filename)
{
  Mix_Init(MIX_INIT_MP3);
  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
  int c = Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      Mix_GetMusicDecoder(i);
    }
  Mix_Music *mus = Mix_LoadMUS(filename.c_str());
  Mix_PlayMusic(mus, 1);
}