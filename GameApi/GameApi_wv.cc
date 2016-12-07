
#include "GameApi_h.hh"


class WaveformPolygon : public FaceCollection
{
public:
  WaveformPolygon(Waveform *w, float start_angle, float end_angle,
		  float radius, int num_samples, int num_waves)
    : w(w), start_angle(start_angle), end_angle(end_angle),
      radius(radius), num_samples(num_samples), num_waves(num_waves) { }
  virtual void Prepare() { }
  virtual int NumFaces() const { return num_samples*num_waves; }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int sample = face / num_waves;
    int wave = face - sample*num_waves;
    if (point==1 || point==2) { wave++; }
    if (point==2 || point==3) { sample++; }
    float angle = start_angle + (end_angle-start_angle)*(float(wave)/num_waves);
    float x = radius * float(sample)/num_samples * cos(angle);
    float z = radius * float(sample)/num_samples * sin(angle);
    float pos = float(sample)/num_samples*w->Length();
    float y = w->Index(pos);
    Point p(x,y,z);
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    return -Vector::CrossProduct(p2-p1,p3-p1);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const {
    int sample = face / num_waves;
    int wave = face - sample*num_waves;
    if (point==1 || point==2) { wave++; }
    if (point==2 || point==3) { sample++; }
    float val = wave/num_waves;
    float val2 = float(sample)/num_samples;
    Point2d p = { val, val2 };
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }

private:
  Waveform *w;
  float start_angle, end_angle;
  float radius;
  int num_samples, num_waves;
};

EXPORT GameApi::P GameApi::WaveformApi::waveform_rotated_polygon(WV wave, float start_angle, float end_angle, float radius, int num_samples, int num_waves)
{
  Waveform *w = find_waveform(e, wave);
  return add_polygon2(e, new WaveformPolygon(w, start_angle, end_angle, radius, num_samples, num_waves),1);
}


EXPORT GameApi::WV GameApi::WaveformApi::empty(float length)
{
  return add_waveform(e, new ZeroWaveform(length, -1.0,1.0));
}
class SumWaveform : public Waveform
{
public:
  SumWaveform(Waveform *a1, Waveform *a2) : a1(a1), a2(a2) { }
  virtual float Length() const { return std::max(a1->Length(), a2->Length()); }
  virtual float Min() const { return std::min(a1->Min(),a2->Min()); }
  virtual float Max() const { return std::max(a1->Max(), a2->Max()); }
  virtual float Index(float val) const 
  {
    float val1 = a1->Index(val);
    float val2 = a2->Index(val);
    return val1+val2;
  }  
private:
  Waveform *a1, *a2;
};
EXPORT GameApi::WV GameApi::WaveformApi::sum(WV w1, WV w2)
{
  Waveform *ww1 = find_waveform(e, w1);
  Waveform *ww2 = find_waveform(e, w2);
  return add_waveform(e, new SumWaveform(ww1,ww2));
}

class ScaleWaveform : public Waveform
{
public:
  ScaleWaveform(Waveform *w, float scale) : w(w), scale(scale) { }
  virtual float Length() const { return w->Length(); }
  virtual float Min() const { return w->Min()*scale; }
  virtual float Max() const { return w->Max()*scale; }
  virtual float Index(float val) const 
  {
    return w->Index(val)*scale;
  }
private:
  Waveform *w;
  float scale;
};

EXPORT GameApi::WV GameApi::WaveformApi::scale(WV w1, float scale_factor)
{
  Waveform *ww1 = find_waveform(e, w1);
  return add_waveform(e, new ScaleWaveform(ww1, scale_factor));
}

class MoveWaveform : public Waveform
{
public:
  MoveWaveform(Waveform *w, float delta) : w(w), delta(delta) { }
  virtual float Length() const { return w->Length(); }
  virtual float Min() const { return w->Min(); }
  virtual float Max() const { return w->Max(); }
  virtual float Index(float val) const {
    val+=delta;
    if (val<0.0) return w->Index(0);
    if (val>w->Length()) return w->Index(w->Length());
    return w->Index(val);
  }

private:
  Waveform *w;
  float delta;
};

EXPORT GameApi::WV GameApi::WaveformApi::move(WV w1, float delta)
{
  Waveform *ww1 = find_waveform(e, w1);
  return add_waveform(e, new MoveWaveform(ww1, delta));
}


EXPORT GameApi::WV GameApi::WaveformApi::sinwave(float length, float freq)
{
  return add_waveform(e, new SinWaveform(length, freq));
}
class GaussianWaveform : public Waveform
{
public:
  GaussianWaveform(float start_x, float end_x, float start_y, float end_y) : start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y)
  {
  }
  virtual float Length() const { return end_x-start_x; }
  virtual float Min() const { return start_y; }
  virtual float Max() const { return end_y; }
  virtual float Index(float val) const 
  {
    val/=Length();
    val*=6.0;
    val-=3.0;
    return start_y + (end_y-start_y)*exp(-val*val);
  }
private:
  float start_x, end_x, start_y, end_y;
};
EXPORT GameApi::WV GameApi::WaveformApi::gaussian(float start_x, float end_x, float start_y, float end_y)
{
  return add_waveform(e, new GaussianWaveform(start_x, end_x, start_y, end_y));
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
