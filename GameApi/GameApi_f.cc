
#include "GameApi_h.hh"

GameApi::FloatApi::FloatApi(Env &e) : e(e) { }

class LazySingle : public LazyValue<float>
{
public:
  LazySingle(float v) : v(v) { }
  float get() const { return v; }
private:
  float v;
};
class LazyRef : public LazyValue<float>
{
public:
  LazyRef(float* v) : v(v) { }
  float get() const { return *v; }
private:
  float *v;
};
class LazyIndex : public LazyValue<float>
{
public:
  LazyIndex(float* v, int pos) : v(v), pos(pos) { }
  float get() const { return v[pos]; }
private:
  float *v;
  int pos;
};

GameApi::F GameApi::FloatApi::value(float v)
{
  return add_float(e, new LazySingle(v));
}
GameApi::F GameApi::FloatApi::ref(float *v)
{
  return add_float(e, new LazyRef(v));
}
GameApi::F GameApi::FloatApi::array_index(float *array, int pos)
{
  return add_float(e, new LazyIndex(array, pos));
}
float GameApi::FloatApi::get_value(F f)
{
  LazyValue<float> *value = find_float(e, f);
  return value->get();
}
GameApi::FloatArrayApi::FloatArrayApi(Env &e) : e(e) { }
GameApi::FA GameApi::FloatArrayApi::array(float *array, int size)
{
  return add_float_array(e, new NativeArray<float>(array, size));
}
template<class T> 
class RefArray : public Array<int,T>
{
public:
  RefArray(std::vector<LazyValue<T>*> vec) : vec(vec) {}
  int Size() const { return vec.size(); }
  T Index(int i) const { return vec[i]->get(); }
private:
  std::vector<LazyValue<T>*> vec;
};

class RampArray : public Array<int,float>
{
public:
  RampArray(float start_value, float end_value, int steps)
    : start(start_value), end(end_value), steps(steps)
  {
  }
  int Size() const { return steps; }
  float Index(int i) const {
    float val = start + i*(end-start)/steps;
    return val;
  }
private:
  float start;
  float end;
  int steps;
};

class SpanArrays : public Bitmap<Color>
{
public:
  SpanArrays(Array<int,float> &a1, Array<int,float> &a2, ContinuousBitmap<Color> *f) : a1(a1) ,a2(a2), f(f) { } 
  int SizeX() const { return a1.Size(); }
  int SizeY() const { return a2.Size(); }
  Color Map(int x, int y) const
  {
    float val1 = a1.Index(x);
    float val2 = a2.Index(y);
    return f->Map(val1, val2);
  }
private:
  Array<int,float> &a1;
  Array<int,float> &a2;
  ContinuousBitmap<Color> *f;
};
GameApi::BM GameApi::FloatArrayApi::span_arrays(FA fa1, FA fa2, CBM f)
{
  Array<int,float> *arr1 = find_float_array(e, fa1);
  Array<int,float> *arr2 = find_float_array(e, fa2);
  ContinuousBitmap<Color> *bm = find_continuous_bitmap(e, f);
  return add_color_bitmap2(e, new SpanArrays(*arr1, *arr2, bm));
}
GameApi::FA GameApi::FloatArrayApi::ramp(float start_value, float end_value, int steps)
{
  return add_float_array(e, new RampArray(start_value, end_value, steps));
}
GameApi::FA GameApi::FloatArrayApi::f_array(F *array, int size)
{
  std::vector<LazyValue<float>*> arr;
  for(int i=0;i<size;i++)
    {
      arr.push_back(find_float(e, array[i]));
    }
  return add_float_array(e, new RefArray<float>(arr));
}
GameApi::FA GameApi::FloatArrayApi::duparray(float value, int size)
{
  return add_float_array(e, new DuplicateArray<int,float>(value, size));
}
GameApi::FA GameApi::FloatArrayApi::duparray(FA fa, int count)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float_array(e, new DupArray<float>(*arr, count));
}
GameApi::FA GameApi::FloatArrayApi::subarray(FA fa, int start_index, int length)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float_array(e, new SubArray<int, float>(*arr, start_index, length));
}
class LazyIndex2 : public LazyValue<float>
{
public:
  LazyIndex2(Array<int,float> *arr, int index) : arr(arr), index(index) { }
  float get() const { return arr->Index(index); }
private:
  Array<int,float> *arr;
  int index;
};
GameApi::F GameApi::FloatArrayApi::array_index(FA fa, int index)
{
  Array<int,float> *arr = find_float_array(e, fa);
  return add_float(e, new LazyIndex2(arr, index));
}
