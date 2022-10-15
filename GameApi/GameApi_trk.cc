#include "GameApi_h.hh"
#include <fstream>
#include <ios>
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
#ifdef USE_SDL_MIXER
  g_low->sdl_mixer->Mix_Init(Low_MIX_INIT_MP3);
  g_low->sdl_mixer->Mix_OpenAudio(22050, Low_MIX_DEFAULT_FORMAT, 2, 4096);
#ifndef EMSCRIPTEN
  int c = g_low->sdl_mixer->Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      g_low->sdl_mixer->Mix_GetMusicDecoder(i);
    }
#endif
  Low_Mix_Music *mus = g_low->sdl_mixer->Mix_LoadMUS(filename.c_str());
  g_low->sdl_mixer->Mix_PlayMusic(mus, 1);
#else
  std::cout << "TrackerApi::play_mp3... SDL_MIXER is disabled" << std::endl;
#endif
}

void GameApi::TrackerApi::play_ogg(std::string filename)
{
#ifdef USE_SDL_MIXER
  g_low->sdl_mixer->Mix_Init(Low_MIX_INIT_OGG);
  g_low->sdl_mixer->Mix_OpenAudio(22050, Low_MIX_DEFAULT_FORMAT, 2, 4096);
#ifndef EMSCRIPTEN
  int c = g_low->sdl_mixer->Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      g_low->sdl_mixer->Mix_GetMusicDecoder(i);
    }
#endif
  char *ptr = new char[filename.size()+1];
  std::copy(filename.begin(), filename.end(), ptr);
  //#ifndef EMSCRIPTEN
  Low_Mix_Music *mus = g_low->sdl_mixer->Mix_LoadMUS(ptr);
  g_low->sdl_mixer->Mix_PlayMusic(mus, 1);
  /*
#else
  //std::ifstream ss(filename.c_str(), std::ios::binary|std::ios::in);
  //char ch;
  //std::vector<unsigned char> s;
  //while(ss.get(ch)) { s.push_back((unsigned char)ch); }
  //Low_SDL_RWops *ops = g_low->sdl->SDL_RWFromMem((void*)&s[0], s.size());
  Low_SDL_RWops *ops = g_low->sdl->SDL_RWFromFile(filename.c_str(),"rb");
  Low_Mix_Chunk *chunk = g_low->sdl_mixer->Mix_LoadWAV_RW(ops, 0);
  g_low->sdl_mixer->Mix_PlayChannel(-1, chunk, 0); 
#endif
  */
#else
  std::cout << "TrackerApi::play_ogg... SDL_MIXER is disabled" << std::endl;
#endif
}

//void *g_ogg_chunk;
void *GameApi::TrackerApi::setup_ogg(const std::vector<unsigned char> &data, int type)
{
#ifdef USE_SDL_MIXER
  //g_ogg_ops = ops;
  if (type==0)
    g_low->sdl_mixer->Mix_Init(Low_MIX_INIT_OGG);
  else if (type==1)
    g_low->sdl_mixer->Mix_Init(Low_MIX_INIT_MP3);
    
  g_low->sdl_mixer->Mix_OpenAudio(22050, Low_MIX_DEFAULT_FORMAT, 2, 4096);

#ifndef EMSCRIPTEN
  int c = g_low->sdl_mixer->Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      g_low->sdl_mixer->Mix_GetMusicDecoder(i);
    }
#endif

    Low_SDL_RWops *ops = g_low->sdl->SDL_RWFromMem((void*)&data[0], data.size());
  Low_Mix_Chunk *chunk = g_low->sdl_mixer->Mix_LoadWAV_RW(ops, 0);

  //g_ogg_chunk = chunk;
  return (void*)chunk;
#else
  std::cout << "TrackerApi::setup_ogg... SDL_MIXER is disabled" << std::endl;
#endif
  return 0;
}
void GameApi::TrackerApi::play_ogg(void *ptr)
{
  // const std::vector<unsigned char> *data = (const std::vector<unsigned char> *)g_ogg_ops;

#ifdef USE_SDL_MIXER
  Low_Mix_Chunk *chunk = (Low_Mix_Chunk*)ptr;

  
  g_low->sdl_mixer->Mix_PlayChannel(-1, chunk, 0); 
#else
  std::cout << "TrackerApi::play_ogg(void*)... SDL_MIXER is disabled" << std::endl;
#endif  
}
void GameApi::TrackerApi::play_ogg(const std::vector<unsigned char> &data)
{
#ifdef USE_SDL_MIXER
    g_low->sdl_mixer->Mix_Init(Low_MIX_INIT_OGG);
  g_low->sdl_mixer->Mix_OpenAudio(22050, Low_MIX_DEFAULT_FORMAT, 2, 4096);
#ifndef EMSCRIPTEN
  int c = g_low->sdl_mixer->Mix_GetNumMusicDecoders();
  for(int i=0;i<c;i++)
    {
      g_low->sdl_mixer->Mix_GetMusicDecoder(i);
    }
#endif
  //std::ifstream ss(filename.c_str(), std::ios::binary|std::ios::in);
  //char ch;
  //std::vector<unsigned char> s;
  //while(ss.get(ch)) { s.push_back((unsigned char)ch); }
  //Low_SDL_RWops *ops = g_low->sdl->SDL_RWFromFile(filename.c_str(),"rb");
  Low_SDL_RWops *ops = g_low->sdl->SDL_RWFromMem((void*)&data[0], data.size());
  Low_Mix_Chunk *chunk = g_low->sdl_mixer->Mix_LoadWAV_RW(ops, 0);
  g_low->sdl_mixer->Mix_PlayChannel(-1, chunk, 0); 
#else
  std::cout << "TrackerApi::play_ogg(data)... SDL_MIXER is disabled" << std::endl;
#endif
}
extern "C" int HTML5_Mix_HaltMusic();
void GameApi::TrackerApi::stop_music_playing()
{
#ifndef USE_SDL_MIXER_HACK
#ifdef USE_SDL_MIXER
  g_low->sdl_mixer->Mix_HaltChannel(-1);
#else
  std::cout << "TrackerApi::stop_music_playing... SDL_MIXER is disabled" << std::endl;
#endif
#else
  g_low->sdl_mixer->Mix_HaltChannel(-1);
  //HTML5_Mix_HaltMusic();
#endif
}

int music_initialized = 0;

class PlayWavViaKeypress : public MainLoopItem
{
public:
  void init_music()
  {
    if (!music_initialized) {
#ifdef USE_SDL_MIXER
      std::cout << g_low->sdl->SDL_GetError() << std::endl;
      int val = g_low->sdl_mixer->Mix_OpenAudio(44100, Low_MIX_DEFAULT_FORMAT, 2, 4096);
      std::cout << "init_music: openaudio returned: " << val << std::endl;
      std::cout << g_low->sdl->SDL_GetError() << std::endl;
#ifndef EMSCRIPTEN
      int c = g_low->sdl_mixer->Mix_GetNumMusicDecoders();
      std::cout << g_low->sdl->SDL_GetError() << std::endl;
      for(int i=0;i<c;i++)
	{
	  g_low->sdl_mixer->Mix_GetMusicDecoder(i);
	  std::cout << g_low->sdl->SDL_GetError() << std::endl;
	}
#endif
      g_low->sdl_mixer->Mix_AllocateChannels(16);
      std::cout << g_low->sdl->SDL_GetError() << std::endl;


#endif      
      music_initialized=1;
    }
  }
  PlayWavViaKeypress(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, std::string url, std::string homepage, int key) : env(env), ev(ev), next(next), url(url), homepage(homepage), key(key) { firsttime=true; initialized=false; chunk = 0; initialized2 = false;
#ifdef USE_SDL_MIXER
    if (!music_initialized)
      g_low->sdl_mixer->Mix_Init(0);
#endif
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    vec = env.get_loaded_async_url(url);

    /*
    filename = "audioA.wav";
    static int index = 0;
    index++;
    filename[5]+=index;
    std::ofstream ss(filename.c_str(),std::ios_base::out|std::ios_base::binary);
    //ss.write((const char*)&vec->operator[](0), vec->size());
    int s = vec->size();
    for(int i=0;i<s;i++) {
      ss.put(vec->operator[](i));
    }
    ss.close();
    */

    
    initialized = true;

  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
    }
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (initialized) { 
      int ch = e.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) ch='0';
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
    if (ch==26) ch='w';
    if (ch==22) ch='s';
    if (ch==4) ch='a';
    if (ch==7) ch='d';
    if (ch==82) ch='w';
    if (ch==81) ch='s';
    if (ch==80) ch='a';
    if (ch==79) ch='d';
    if (ch==44) ch=32;
#endif

      if (ch == key && e.type==0x300) {
	if (!music_initialized) init_music();
	if (!initialized2) {
#ifdef USE_SDL_MIXER

	  chunk = g_low->sdl_mixer->Mix_LoadWAV_RW(/*g_low->sdl->SDL_RWFromFile(filename.c_str(), "rb")*/ g_low->sdl->SDL_RWFromMem((void*)&vec->operator[](0), vec->size()),0);
#endif
	  // std::cout << g_low->sdl_mixer->Mix_GetError() << std::endl;
	  if (!chunk || !chunk->ptr) {
	    std::cout << "Invalid wav file/Mix_QuickLoad_WAV failed" << std::endl;
	  }
	  initialized2 = true;
	}
	  
#ifdef USE_SDL_MIXER
	g_low->sdl_mixer->Mix_PlayChannel(-1,chunk, 0);
#endif
	//std::cout << g_low->sdl_mixer->Mix_GetError() << std::endl;
      }
    }
    return next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  std::string url;
  std::string homepage;
  int key;
  bool firsttime;
  bool initialized;
  bool initialized2;
  Low_Mix_Chunk *chunk;
  std::string filename;
  GameApi::ASyncVec *vec;
};

extern std::string gameapi_homepageurl;

GameApi::ML GameApi::TrackerApi::play_wave_via_keypress(EveryApi &ev, ML ml, std::string url, int key)
{
  MainLoopItem *item = find_main_loop(e, ml);
  std::string homepage = gameapi_homepageurl;
  return add_main_loop(e, new PlayWavViaKeypress(e,ev,item, url, homepage, key));
}
