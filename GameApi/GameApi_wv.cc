
#include "GameApi_h.hh"


EXPORT GameApi::WV GameApi::WaveformApi::empty(float length)
{
  return add_waveform(e, new ZeroWaveform(length, -1.0,1.0));
}
EXPORT GameApi::WV GameApi::WaveformApi::sinwave(float length, float freq)
{
  return add_waveform(e, new SinWaveform(length, freq));
}
EXPORT GameApi::WV GameApi::WaveformApi::sample(float *array, int length, float samplelength)
{
  return add_waveform(e, new ArrayWaveform2(array, length, samplelength, -1.0, 1.0));
}
EXPORT GameApi::WV GameApi::WaveformApi::int_sample(int *array, int length, float samplelength, int min_value, int max_value)
{
  GameApi::WV w;
  w.id = 0;
  return w;
}
EXPORT GameApi::WV GameApi::WaveformApi::mix(GameApi::WV orig, float pos, GameApi::WV sample)
{
  Waveform *m_orig = find_waveform(e, orig);
  Waveform *m_sample = find_waveform(e, sample);
  return add_waveform(e, new MixWaveform(m_orig, pos, m_sample));
}
EXPORT GameApi::WV GameApi::WaveformApi::volume_ramp(GameApi::WV orig, float old_y_value, float x_pos1, float x_pos2, float y_pos1, float y_pos2)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new VolumeRampWaveform(m_orig, old_y_value, x_pos1, x_pos2, y_pos1, y_pos2));
}
EXPORT GameApi::WV GameApi::WaveformApi::freq_change(GameApi::WV orig, float old_freq, float new_freq)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new FreqChangeWaveform(m_orig, old_freq, new_freq));
}

EXPORT float GameApi::WaveformApi::get_value(WV orig, float val)
{
  Waveform *m_orig = find_waveform(e, orig);
  return m_orig->Index(val);
}
EXPORT float GameApi::WaveformApi::length(GameApi::WV orig)
{
  Waveform *m_orig = find_waveform(e, orig);
  return m_orig->Length();
}

class LengthChangeWaveform : public Waveform
{
public:
  LengthChangeWaveform(Waveform *wv, float new_length) : wv(wv), new_length(new_length) { }
  virtual float Length() const { 
    return new_length; 
  }
  virtual float Min() const { return wv->Min(); }
  virtual float Max() const { return wv->Max(); }
  virtual float Index(float val) const 
  {
    if (val>=0.0 && val<new_length && val<wv->Length()) { return wv->Index(val); }
    return 0.0;
  }
private:  
  Waveform *wv;
  float new_length;
};

EXPORT GameApi::WV GameApi::WaveformApi::length_change(WV orig, float new_length)
{
  Waveform *m_orig = find_waveform(e, orig);
  return add_waveform(e, new LengthChangeWaveform(m_orig, new_length));
}


class FunctionWaveform : public Waveform
{
public:
  FunctionWaveform(std::function<float (float)> f, float length, float min_value, float max_value) :  f(f), length(length), min_value(min_value), max_value(max_value) { }
  virtual float Length() const { return length; }
  virtual float Min() const { return min_value; }
  virtual float Max() const { return max_value; }
  virtual float Index(float val) const 
  {
    return f(val);
  }

private:
  std::function<float (float)> f; 
  float length;
  float min_value; 
  float max_value; 
}; 

EXPORT GameApi::WV GameApi::WaveformApi::function(std::function<float (float)> f, float length, float min_value, float max_value)
{
  //GameApi::EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_waveform(e, new FunctionWaveform(f, length, min_value, max_value));
}

class RepeatWaveform2 : public Waveform
{
public:
  RepeatWaveform2(Waveform *wv, int num) : wv(wv), num(num) { }
  virtual float Length() const { 
    return wv->Length()*num; 
  }
  virtual float Min() const { return wv->Min(); }
  virtual float Max() const { return wv->Max(); }
  virtual float Index(float val) const 
  {
    float len = wv->Length();
    val = fmod(val, len);
    return wv->Index(val);
  }

private:
  Waveform *wv;
  int num;
};

class StepWaveform : public Waveform
{
public:
  StepWaveform(bool flip) : flip(flip) { }
  virtual float Length() const { 
    return 1.0;
  }
  virtual float Min() const { return 0.0; }
  virtual float Max() const { return 1.0; }
  virtual float Index(float pos) const 
  {
    float val=0.0;
    if (pos>0.5) { val=1.0; }
    if (flip) { val = 1.0-val; }
    return val;
  }
private:
  bool flip;
};

EXPORT GameApi::WV GameApi::WaveformApi::repeat(WV wave, int num)
{
  Waveform *wave2 = find_waveform(e, wave);

  return add_waveform(e, new RepeatWaveform2(wave2, num));
}
EXPORT GameApi::WV GameApi::WaveformApi::step(bool flip)
{
  return add_waveform(e, new StepWaveform(flip));
}


class CubicInterpolateWaveform : public Waveform
{
public:
  CubicInterpolateWaveform(float f_0, float f_1, float df_0, float df_1, float min_y, float max_y) : cb(f_0, f_1, df_0, df_1), min_y(min_y), max_y(max_y) { }
  virtual float Length() const { 
    return 1.0;
  }
  virtual float Min() const { return min_y; }
  virtual float Max() const { return max_y; }
  virtual float Index(float pos) const 
  {
    return cb.Index(pos);
  }
private:
  CubicInterpolate cb;
  float min_y, max_y;
};
GameApi::WV GameApi::WaveformApi::cubic(float f_0, float f_1, float df_0, float df_1, float min_y, float max_y)
{
  return add_waveform(e, new CubicInterpolateWaveform(f_0, f_1, df_0, df_1, min_y, max_y));
}
