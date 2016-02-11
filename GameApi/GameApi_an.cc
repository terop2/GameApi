
#include "GameApi_h.hh"

template<class I, class T>
class AnimArray : public I
{
public:
  AnimArray(GameApi::Env &e, GameApi::IS *array, int size)
    : size(size), e(e)
  {
    array2 = new GameApi::IS[size];
    std::copy(array, array+size, array2);
  }
  ~AnimArray() { delete [] array2; }
  T Index(int path, float t) const
  {
    int dd = 0.0;
    for(int i=0;i<size;i++)
      {
	GameApi::IS ee = array2[i];
	AnimImpl k = find_anim(e,ee);
	const I *ptr = get(k);
	if (!ptr) continue;
	float d = ptr->Duration();
	int olddd = dd;
	dd+=d;
	if (dd > t)
	  {
	    return ptr->Index(path, t-olddd);
	  }
      }
    return T();
  }
  virtual I *get(const AnimImpl &e) const=0;
  int NumPaths() const { return 1; }
  float Duration() const
  {
    float dd = 0.0;
    for(int i=0;i<size;i++)
      {
	GameApi::IS ee = array2[i];
	AnimImpl k = find_anim(e,ee);
	const I *ptr = get(k);
	if (!ptr) continue;
	float d = ptr->Duration();
	dd+=d;
      }
    return dd;
  }
private:
  GameApi::IS *array2;
  int size;
  GameApi::Env &e;
};

template<class I, class T>
class AnimRepeat : public I
{
public:
  AnimRepeat(GameApi::Env &e, GameApi::IS val, int count=-1) : e(e), val(val),c(count) { }
  T Index(int path, float t) const
  {
    GameApi::IS ee = val;
    AnimImpl k = find_anim(e,ee);
    const I *ptr = get(k);
    if (!ptr) return T();
    float d = ptr->Duration();
    int count = int(t / d);
    float pos = count * d;
    t-=pos;
    if (c!=-1 && count > c)
      {
	t = d;
      }
    return ptr->Index(path, t);
  }
  virtual I *get(const AnimImpl &e) const=0;
  int NumPaths() const { return 1; }
  float Duration() const
  {
    GameApi::IS ee = val;
    AnimImpl k = find_anim(e,ee);
    const I *ptr = get(k);
    if (!ptr) return 0.0;
    float d = ptr->Duration();
    return d;
  }
private:
  GameApi::Env &e;
  GameApi::IS val;
  int c;
};



EXPORT GameApi::IS GameApi::AnimApi::single(PT point, float duration)
{
  Point *p = find_point(e,point);
  Point pp(p->x, p->y, p->z);

  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = new SingleAnimPoint(pp, duration);
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;  
}
EXPORT GameApi::IS GameApi::AnimApi::single(float val, float duration)
{
  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = 0;
  i.wave_float = new SingleAnimFloat(val, duration);
  IS is = add_anim(e,i);
  return is;
}

EXPORT GameApi::IS GameApi::AnimApi::line(int val1, int val2, float duration)
{
  AnimImpl i;
  i.wave_int = new LineAnimInt(val1,val2, duration);
  i.wave_point = 0;
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;
}

EXPORT GameApi::IS GameApi::AnimApi::line(PT p1, PT p2, float duration)
{
  Point *p = find_point(e,p1);
  Point pp(p->x, p->y, p->z);
  Point *p2x = find_point(e,p2);
  Point pp2(p2x->x, p2x->y, p2x->z);

  AnimImpl i;
  i.wave_int = 0;
  i.wave_point = new LineAnimPoint(pp, pp2, duration);
  i.wave_float = 0;
  IS is = add_anim(e, i);
  return is;  
}
EXPORT GameApi::IS GameApi::AnimApi::line(float val1, float val2, float duration)
{
  AnimImpl i;
  i.wave_int =0;
  i.wave_point = 0;
  i.wave_float = new LineAnimFloat(val1, val2, duration);
  IS is = add_anim(e,i);
  return is;
}


class AnimArrayInt : public AnimArray<AnimInt, int>
{
public:
  AnimArrayInt(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimInt,int>(e, array, size) { }
  AnimInt *get(const AnimImpl &c) const { return c.wave_int; }
};
class AnimArrayPoint : public AnimArray<AnimPoint3d, Point>
{
public:
  AnimArrayPoint(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimPoint3d,Point>(e, array, size) { }
  AnimPoint3d *get(const AnimImpl &c) const { return c.wave_point; }
};
class AnimArrayFloat : public AnimArray<AnimFloat, float>
{
public:
  AnimArrayFloat(GameApi::Env &e, GameApi::IS *array, int size) : AnimArray<AnimFloat,float>(e, array, size) { }
  AnimFloat *get(const AnimImpl &c) const { return c.wave_float; }
};


class AnimRepeatInt : public AnimRepeat<AnimInt, int>
{
public:
  AnimRepeatInt(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimInt,int>(e, val, count) { }
  AnimInt *get(const AnimImpl &c) const { return c.wave_int; }
};
class AnimRepeatPoint : public AnimRepeat<AnimPoint3d, Point>
{
public:
  AnimRepeatPoint(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimPoint3d,Point>(e, val, count) { }
  AnimPoint3d *get(const AnimImpl &c) const { return c.wave_point; }
};
class AnimRepeatFloat : public AnimRepeat<AnimFloat, float>
{
public:
  AnimRepeatFloat(GameApi::Env &e, GameApi::IS val, int count=-1) : AnimRepeat<AnimFloat,float>(e, val, count) { }
  AnimFloat *get(const AnimImpl &c) const { return c.wave_float; }
};




EXPORT GameApi::IS GameApi::AnimApi::seq_line(IS *array, int size)
{
  AnimImpl i;
  i.wave_int = new AnimArrayInt(e, array, size);
  i.wave_point = new AnimArrayPoint(e, array, size);
  i.wave_float = new AnimArrayFloat(e, array, size);
  IS is = add_anim(e,i);
  return is;
} 
EXPORT GameApi::IS GameApi::AnimApi::repeat(IS i, int count)
{
  AnimImpl ii;
  ii.wave_int = new AnimRepeatInt(e, i, count);
  ii.wave_point = new AnimRepeatPoint(e, i, count);
  ii.wave_float = new AnimRepeatFloat(e, i, count);
  IS is = add_anim(e,ii);
  return is;
}
EXPORT GameApi::IS GameApi::AnimApi::repeat_infinite(IS i)
{
  AnimImpl ii;
  ii.wave_int = new AnimRepeatInt(e, i, -1);
  ii.wave_point = new AnimRepeatPoint(e, i, -1);
  ii.wave_float = new AnimRepeatFloat(e, i, -1);
  IS is = add_anim(e,ii);
  return is;
}
EXPORT int GameApi::AnimApi::timed_value_repeat_num(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimInt *wave = ii.wave_int;
  AnimPoint3d *wave2 = ii.wave_point;
  AnimFloat *wave3 = ii.wave_float;
  if (wave && wave->Duration()>0.01) 
    {
      int val = time / wave->Duration();
      return val;
    }
  if (wave2 && wave2->Duration()>0.01) 
    {
      int val = time / wave2->Duration();
      return val;
    }
  if (wave3 && wave3->Duration()>0.01)
    {
      int val = time / wave3->Duration();
      return val;
    }
  std::cout << "timed_value_repeat_num ERROR" << std::endl;
  return 0;
}

EXPORT int GameApi::AnimApi::timed_value(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimInt *wave = ii.wave_int;
  if (!wave) return 0;
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  return wave->Index(0, time);
}

EXPORT GameApi::PT GameApi::AnimApi::timed_value_point(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimPoint3d *wave = ii.wave_point;
  if (!wave) return add_point(e, 0.0, 0.0);
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  Point p = wave->Index(0, time); 
  return add_point(e, p.x, p.y,p.z);
}
EXPORT float GameApi::AnimApi::timed_value_float(IS i, float time)
{
  AnimImpl ii = find_anim(e, i);
  AnimFloat *wave = ii.wave_float;
  if (!wave) return 0.0;
  int val = timed_value_repeat_num(i, time);
  float x = val * wave->Duration();
  time -= x;
  float p = wave->Index(0, time); 
  return p;
}
