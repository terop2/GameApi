
#include "GameApi_h.hh"
#include <SDL_mixer.h>

class EmptySamples : public Samples
{
public:
  EmptySamples() { }
  virtual int NumWaves() const { return 0; }
  virtual int SampleRate(int wave) const { return 1; }
  virtual int Id(int wave) const { return 0; }
  virtual float Length(int wave) const { return 1.0; }
  virtual float Index(int wave, float val) const { return 0.0; }
};

EXPORT GameApi::SM GameApi::SampleCollectionApi::empty()
{
  return add_sample(e, new EmptySamples);
}
class AddSamples : public Samples
{
public:
  AddSamples(Samples *next, Waveform *wave, int sample_rate, int id) 
    : next(next), waveform(wave), sample_rate(sample_rate), id(id)
  { }
  virtual int NumWaves() const { return 1+next->NumWaves(); }
  virtual int SampleRate(int wave) const 
  {
    if (wave<next->NumWaves()) { return next->SampleRate(wave); }
    return sample_rate;
  }
  virtual int Id(int wave) const 
  {
    if (wave<next->NumWaves()) { return next->Id(wave); }
    return id;
  }
  virtual float Length(int wave) const { 
    if (wave<next->NumWaves()) { return next->Length(wave); }
    return waveform->Length();
  }
  virtual float Index(int wave, float val) const {
    if (wave<next->NumWaves()) { return next->Index(wave,val); }
    return waveform->Index(val);
  }
private:
  Samples *next;
  Waveform *waveform;
  int sample_rate;
  int id;
};

EXPORT GameApi::SM GameApi::SampleCollectionApi::add(SM orig, WV wave, int sample_rate, int id)
{
  Samples *sample = find_samples(e, orig);
  Waveform *wv = find_waveform(e, wave);
  return add_sample(e, new AddSamples(sample, wv, sample_rate, id));
}
EXPORT void GameApi::SampleCollectionApi::init_audio()
{
  Mix_Init(0);
  int err = Mix_OpenAudio(22050, AUDIO_U8, 4, 1024);
  if (err==-1)
    {
      std::cout << "Mix_OpenAudio error: " << Mix_GetError() << std::endl;
    }
}
EXPORT void GameApi::SampleCollectionApi::play_sample(int channel, WAV w, int id) {
  play_sample_1(channel, w, id);
}
void GameApi::SampleCollectionApi::play_sample_1(int channel, WAV w, int id)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Wavs *wav = &env->wavs[w.id];
  int i = 0;
  int s = wav->ids.size();
  for(;i<s;i++)
    {
      if (wav->ids[i]==id) { break; }
    }
  std::cout << "Play_Sample: " << i << std::endl;
  int err = 0;
  if (i!=s)
    err = Mix_PlayChannel(channel, (Mix_Chunk*)wav->chunks[i], 0);
  if (err == -1)
    {
      std::cout << "Mix_PlayChannel error: " << Mix_GetError() << std::endl;
    }
}

EXPORT GameApi::WAV GameApi::SampleCollectionApi::prepare(SM samples)
{
  std::cout << "SampleCollectionApi::prepare" << std::endl;
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  Wavs wav;
  env->wavs.push_back(wav);
  int wav_id = env->wavs.size()-1;

  Samples *sample = find_samples(e, samples);
  int s = sample->NumWaves();
  for(int i=0;i<s;i++)
    {
      float len = sample->Length(i);
      int rate = sample->SampleRate(i);
      float step = 100.0 / rate;
      int ss = int(len/step);
      //if (ss%1==1) { ss++; }
      unsigned char *buffer = new unsigned char[ss];
      //int *buffer2 = (int*)buffer;
      env->wavs[wav_id].data.push_back(buffer);
      env->wavs[wav_id].size.push_back(11*4+ss*2);
      env->wavs[wav_id].rate.push_back(rate);
      for(int ii = 0; ii<ss; ii++)
	{
	  buffer[ii] = (unsigned char)(256.0*sample->Index(i,step*ii));
	}
      //SDL_RWops *bf = SDL_RWFromMem((void*)buffer, 11*4+ss*2);
      //Mix_Chunk *sample2 = Mix_LoadWAV_RW(bf, 1);
      Mix_Chunk *sample2 = Mix_QuickLoad_RAW(buffer, ss);
      if (!sample2)
	{
	  std::cout << "Mix_QuickLoad_Raw error: " << Mix_GetError() << std::endl;
	}
      else
	{
	  //std::cout << "Mix_QuickLoad_RAW success!" << std::endl;
	}
      env->wavs[wav_id].chunks.push_back((void*)sample2);
      env->wavs[wav_id].ids.push_back(sample->Id(i));
    }
  GameApi::WAV w;
  w.id = wav_id;
  return w;
}
