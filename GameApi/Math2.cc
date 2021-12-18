#include <iostream>
#include <cmath>
#include <vector>
//#include "GameApi_h.hh"


template<class I, class T>
class MArray
{
public:
  virtual I size() const=0;
  virtual T map(I i) const=0;
};

template<class T>
class DiffArray : public MArray<int,T>
{
public:
  DiffArray() : arr(0) {

  }
  DiffArray(MArray<int,T> &arr) : arr(&arr) {

    
  }
  void preparecache() const
  {
  }
  void set_arr(MArray<int,T>* p_arr) { arr= p_arr; }
  int size() const { return arr->size()-1; }
  T map(int i) const {
    if (stored==i) { return arr->map(i+1)-val; }
    T val2 = arr->map(i);
    T val1 = arr->map(i+1);
    stored=i+1;
    val=val1;
    return val1-val2;
  }
private:
  MArray<int,T> *arr;
  mutable int stored;
  mutable T val;
};

template<class T>
class Derivatives : public MArray<int,T>
{
public:
  Derivatives(MArray<int,T> &arr) : arr(&arr) { }
  int size() const {
    return arr->size();
  }
  T map(int i) const {
    if (ptr==0) {
      int s = size();
      ptr = new DiffArray<T>[s];
      ptr[0].set_arr(arr);
      for(int j=1;j<s;j++) {
	ptr[j].set_arr(&ptr[j-1]);
      }
      // cache = new T[s];
      //ava = new bool[s];
      //for(int i=0;i<s;i++) ava[i]=false;
    }
    if (i==0) return arr->map(0);
    //if (ava[i]) return cache[i];
   
    T val = ptr[i-1].map(0);
    //cache[i]=val;
    //ava[i]=true;
    return val;
  }
private:
  MArray<int,T> *arr;
  mutable DiffArray<T> *ptr=0;
  //mutable T *cache;
  //mutable bool *ava;
};

template<class T>
class GenerateSequence2 : public MArray<float,T>
{
public:
  GenerateSequence2(MArray<int,T> &derivates, int cut) : arr(derivates),cut(cut) { }
  float size() const { return std::min(cut,arr.size()); }
  T map(float n) const
  {
    int s = std::min(cut,arr.size());
    T sum = 0;
    for(int i=0;i<s;i++) {
      T d = arr.map(i);
      //std::cout << "d=" << d << " value=" << value(n,i+1) << std::endl;
      /*if (fabs(d)>0.001 && n>=i+1)*/ {
	T val = value(n,i);
	if (!std::isnan(val) && !std::isinf(val))
	  sum+=d*val;
	else std::cout << "NAN:" << val << "::" << n << " " << i << std::endl;
      }
    }
    return sum;
  }
  long long factorial(int val) const
  {
   if (val<1) return 1;
    if (val==1) return 1;
    long long val2 = val;
    return val2*factorial(val-1);
  }
  //float factorial(float val) const
  //{
  //  if (fabs(val)<0.01) return 0.0;
  //  return sqrtf(2.0*3.14159265*val)*powf(val/exp(1.0),val)*exp(1.0/12.0/val-1.0/360.0/val/val/val);
  //  
  // }
  long long value(int n, int k) const
  {
    long long res = 1.0;
    for(int i=0;i<k;i++) res*=n-i;
    res/=factorial(k);
    return res;
  }
  //float value(float n, int k) const
 //{
  //  float nf = factorial(n);
  //  float kf = factorial(k);
  //  float nkf = factorial(n-k);
    //std::cout << "Fac1: " << n << " " << nf << std::endl;
    //std::cout << "Fac2: " << k << " " << kf << std::endl;
    //std::cout << "Fac3: " << n-k << " " << nkf << std::endl;
  //  return float(nf/kf/nkf);
  // }
private:
  MArray<int,T> &arr;
  int cut;
};

float test_arr[] =
  {
    7,7,7,7,9,9,9,9,9,9,11,11,11,11,11,13,13,13,15,15,17,17,19,21,21,23,27,29,33,37,43,51,55,66,77,88,99,100,150
  };

class TestSeq : public MArray<int,long long>
{
public:
  TestSeq() { }
  int size() const { return sizeof(test_arr)/sizeof(test_arr[0]); }
  long long map(int i) const { if (i<size()) return test_arr[i]; return 0.0;}
};

template<class T>
void print(const MArray<int,T> &arr)
{
  int s = arr.size();
  for(int i=0;i<s;i++) { std::cout << arr.map(i) << ","; }
}

template<class T>
void print(const MArray<float,T> &arr)
{
  float s = arr.size();
  for(float i=0;i<s;i+=1.0) { std::cout << arr.map(i) << ","; }
}

#if 0

class XBitmapMArray : public MArray<int,long double>
{
public:
  XBitmapMArray(Bitmap<::Color> &bm, int y, bool red, bool green, bool blue, bool alpha) : bm(bm), y(y), red(red), green(green), blue(blue), alpha(alpha) { }
  int size() const { return bm.SizeX(); }
  long double map(int i) const {
    unsigned int c = bm.Map(i,y).Pixel();
    if (red) return (c&0xff0000)>>16;
    if (green) return (c&0xff00)>>8;
    if (blue) return c&0xff;
    if (alpha) return (c&0xff000000)>>24;
    return 0.0f;
  }
private:
  Bitmap<::Color> &bm;
  int y;
  bool red, green, blue, alpha;  
};

class YBitmapMArray : public MArray<int,long double>
{
public:
  YBitmapMArray(Bitmap<Color> &bm, int x, bool red, bool green, bool blue, bool alpha) : bm(bm), x(x), red(red), green(green), blue(blue), alpha(alpha) { }
  int size() const { return bm.SizeY(); }
  long double map(int i) const {
    unsigned int c = bm.Map(x,i).Pixel();
    if (red) return (c&0xff0000)>>16;
    if (green) return (c&0xff00)>>8;
    if (blue) return c&0xff;
    if (alpha) return (c&0xff000000)>>24;
    return 0.0f;
  }
private:
  Bitmap<Color> &bm;
  int x;
  bool red, green, blue, alpha;
};

#endif

#if 0

class ScaleBitmap_slow : public ContinuousBitmap<Color>
{
public:
  ScaleBitmap_slow(Bitmap<Color> *bm) : bm(bm) { }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int sx = SizeX();
    int sy = SizeY();
    for(int i=0;i<sx;i++) PrepareY(i);
    for(int i=0;i<sy;i++) PrepareX(i);
  }
  void Prepare() { bm->Prepare(); HeavyPrepare(); }
  float SizeX() const { return bm->SizeX(); }
  float SizeY() const { return bm->SizeY(); }
  void PrepareX(int y)
  {
    XBitmapMArray *xbm_r= new XBitmapMArray(*bm,y, true, false, false, false);
    XBitmapMArray *xbm_g= new XBitmapMArray(*bm,y, false, true, false, false);
    XBitmapMArray *xbm_b= new XBitmapMArray(*bm,y, false, false, true, false);
    XBitmapMArray *xbm_a= new XBitmapMArray(*bm,y, false, false, false, true);

    Derivatives<long double> *d_x_r = new Derivatives<long double>(*xbm_r);
    GenerateSequence2<long double> *gs_x_r= new GenerateSequence2<long double> (*d_x_r,20);

    Derivatives<long double> *d_x_g= new Derivatives<long double>(*xbm_g);
    GenerateSequence2<long double> *gs_x_g= new GenerateSequence2<long double>(*d_x_g,20);

    Derivatives<long double> *d_x_b= new Derivatives<long double>(*xbm_b);
    GenerateSequence2<long double> *gs_x_b= new GenerateSequence2<long double>(*d_x_b,20);

    Derivatives<long double> *d_x_a= new Derivatives<long double>(*xbm_a);
    GenerateSequence2<long double> *gs_x_a= new GenerateSequence2<long double>(*d_x_a,20);

    if (y==5) {
      std::cout << "ORIG:" << std::endl;
      print(*xbm_r);
      std::cout << "DERIV:" << std::endl;
      print(*d_x_r);
      std::cout << "OUTPUT:" << std::endl;
      print(*gs_x_r);
      std::cout << std::endl; }
    
    x_r_vec.push_back(gs_x_r);
    x_g_vec.push_back(gs_x_g);
    x_b_vec.push_back(gs_x_b);
    x_a_vec.push_back(gs_x_a);
    
  }
  void PrepareY(int x)
  {
    YBitmapMArray *ybm_r = new YBitmapMArray(*bm,x, true, false, false,false);
    YBitmapMArray *ybm_g= new YBitmapMArray(*bm,x, false,true,false,false);
    YBitmapMArray *ybm_b= new YBitmapMArray(*bm,x, false,false,true,false);
    YBitmapMArray *ybm_a= new YBitmapMArray(*bm,x, false,false,false,true);

    Derivatives<long double> *d_y_r = new Derivatives<long double>(*ybm_r);
    GenerateSequence2<long double> *gs_y_r = new GenerateSequence2<long double>(*d_y_r,20);

    Derivatives<long double> *d_y_g= new Derivatives<long double>(*ybm_g);
    GenerateSequence2<long double> *gs_y_g= new GenerateSequence2<long double>(*d_y_g,20);

    Derivatives<long double> *d_y_b= new Derivatives<long double>(*ybm_b);
    GenerateSequence2<long double> *gs_y_b= new GenerateSequence2<long double>(*d_y_b,20);

    Derivatives<long double> *d_y_a= new Derivatives<long double>(*ybm_a);
    GenerateSequence2<long double> *gs_y_a= new GenerateSequence2<long double>(*d_y_a,20);
    y_r_vec.push_back(gs_y_r);
    y_g_vec.push_back(gs_y_g);
    y_b_vec.push_back(gs_y_b);
    y_a_vec.push_back(gs_y_a);

    
  }
  Color Map(float x, float y) const
  {
    if (y>x_r_vec.size()) return Color(255,255,255,255);
    if (x>y_r_vec.size()) return Color(255,0,0,255);
    if (y>y_r_vec[int(x)]->size()) return Color(255,255,0,255);
    if (x>x_r_vec[int(y)]->size()) return Color(255,0,255,255);
    
    //std::cout << "x=" << x << " y=" << y << std::endl;
    long double val_r = x_r_vec[int(y)]->map(x);
    //long double val_r2 = y_r_vec[int(x)]->map(y);
    long double avg_r = val_r; //(val_r+val_r2)/2.0;    

    long double val_g = x_g_vec[int(y)]->map(x);
    //long double val_g2 =y_g_vec[int(x)]->map(y);
    long double avg_g = val_g; //(val_g+val_g2)/2.0;    

    long double val_b = x_b_vec[int(y)]->map(x);
    //long double val_b2 = y_b_vec[int(x)]->map(y);
    long double avg_b = val_b; //(val_b+val_b2)/2.0;    

    long double val_a = x_a_vec[int(y)]->map(x);
    //long double val_a2 = y_a_vec[int(x)]->map(y);
    long double avg_a = val_a; //(val_a+val_a2)/2.0;
    if (avg_r<0.0L) avg_r = 0.0L;
    if (avg_r>255.0L) avg_r=255.0L;
    if (avg_g<0.0L) avg_g = 0.0L;
    if (avg_g>255.0L) avg_g=255.0L;
    if (avg_b<0.0L) avg_b = 0.0L;
    if (avg_b>255.0L) avg_b=255.0L;
    if (avg_a<0.0L) avg_a = 0.0L;
    if (avg_a>255.0L) avg_a=255.0L;
    //std::cout << avg_r << " " << avg_g << " " << avg_b << " " << avg_a << std::endl;
    return Color(int(avg_r),int(avg_g),int(avg_b), int(avg_a));   
  }
private:
  Bitmap<Color> *bm;
  std::vector<GenerateSequence2<long double> *> x_r_vec;
  std::vector<GenerateSequence2<long double> *> x_g_vec;
  std::vector<GenerateSequence2<long double> *> x_b_vec;
  std::vector<GenerateSequence2<long double> *> x_a_vec;
  std::vector<GenerateSequence2<long double> *> y_r_vec;
  std::vector<GenerateSequence2<long double> *> y_g_vec;
  std::vector<GenerateSequence2<long double> *> y_b_vec;
  std::vector<GenerateSequence2<long double> *> y_a_vec;
};

GameApi::CBM GameApi::BitmapApi::scale_bitmap2(GameApi::BM bm)
{

  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);

  
  //BitmapColorHandle *handle2 = new BitmapColorHandle;
  ContinuousBitmap<Color> *cbm = new ScaleBitmap_slow(b2);
  return add_continuous_bitmap(e, cbm);
}
#endif

#if 0
int main()
{
  TestSeq s;
  Derivatives<long long> d(s);
  GenerateSequence2<long long> gs(d,60);
  print(s);
  std::cout << std::endl;
  print(d);
  std::cout << std::endl;
  print(gs);
}
#endif

//
//
// 2nd attempt
//

std::vector<float> diff_init(float value)
{
  std::vector<float> vec;
  vec.push_back(value);
  return vec;
}

std::vector<float> diff_array(std::vector<float> vec, float value)
{
  std::vector<float> res;
  res.push_back(value);
  int s = vec.size();
  float val = value;
  for(int i=0;i<s;i++)
    {
      val-=vec[i];
      res.push_back(val);
    }
  return res;
}
std::vector<std::vector<float> > collect_diffs(std::vector<float> orig_seq)
{
  std::vector<std::vector<float> > res;
  std::vector<float> vec0 = diff_init(orig_seq[0]);
  res.push_back(vec0);
  int s = orig_seq.size();
  for(int i=1;i<s;i++)
    {
      vec0 = diff_array(vec0, orig_seq[i]);
      res.push_back(vec0);
    }
  return res;
}
std::vector<float> diff_array_ends(std::vector<std::vector<float> > vec)
{
  std::vector<float> res;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    res.push_back(vec[i][vec[i].size()-1]);
  }
  return res;
}

std::ostream &operator<<(std::ostream &o, std::vector<float> vec)
{
  int s = vec.size();
  for(int i=0;i<s;i++) if (i!=s-1) o << vec[i] << ","; else o << vec[i];
  return o;
}

long double select(long double n, int k)
{
  //if (n>=0 && n<k) return 0L;
  double res=1;
  for(int i=0;i<k;i++) { res*=(n-i); }
  for(int i=1;i<=k;i++) { res/=(long double)i; }
  return res;
}
long double select_sum(int n, std::vector<float> diff_end)
{
  int s = diff_end.size();
  long double res=0;
  for(int i=0;i<std::min(n+1,s);i++) {
    //std::cout << "n=" << n << "i=" << i << " diff=" << diff_end[i] << " select=" << select(n,i) << std::endl;
    long double r = (long double)select(n,i);
    if (r>0.0)
      res+=diff_end[i]*r;
  }
  return res;
}


#if 0
float test_arr2[] = { 0.5, 1.5, 2.0, 3.5, 4.5 };
int main()
{
  int sz = sizeof(test_arr)/sizeof(test_arr[0]);
  //if (sz>21) sz=21;
  std::vector<float> orig = std::vector<float>(test_arr,test_arr+sz);
  std::vector<std::vector<float> > vec = collect_diffs(orig);
  std::vector<float> diff = diff_array_ends(vec);
  std::cout << diff << std::endl;
  int s = diff.size();
  for(int i=0;i<s;i++)
    std::cout << select_sum(i, diff) << ",";
}
#endif


//
//
// Navier Stokes equations
//
//

struct NS_Point
{
  float x,y,z;
};

class NS_StateSpace
{
public:
  virtual NS_Point u(NS_Point x, float t) const=0;
  virtual float p(NS_Point x, float t) const=0;
};

class NS_StateSpaceInitial
{
public:
  virtual NS_Point u_0(NS_Point x) const=0;
};

class NS_InitialConditions : public NS_StateSpace
{
public:
  NS_InitialConditions(NS_StateSpaceInitial *init, NS_StateSpace *next) : init(init), next(next) { }
  virtual NS_Point u(NS_Point x, float t) const {
    if (t==0.0f) { return init->u_0(x); }
    return next->u(x,t);
  }
  virtual float p(NS_Point x, float t) const { return next->p(x,t); }
private:
  NS_StateSpaceInitial *init;
  NS_StateSpace *next;
};

// Integration and derivatives

// lim dx->1 (f(x+dx)-f(x))/dx

float derivative(float f_x_plus_dx, float f_x, float dx=1.0f)
{
  return (f_x_plus_dx-f_x)/dx;
}
float next_item(float f_x, float df_dx_at_x, float dx=1.0f)
{
  return f_x+df_dx_at_x*dx;
}
float prev_item(float f_x_plus_dx, float df_dx_at_x, float dx=1.0f)
{
  return f_x_plus_dx-df_dx_at_x*dx;
}
