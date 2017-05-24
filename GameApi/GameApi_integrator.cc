
#include "GameApi_h.hh"

template<class T>
class PathIntegrator
{
public:
  PathIntegrator(PathValues<T> &curve, int numsamples) : curve(curve), numsamples(numsamples) { }
  T sum() const
  {
    int s = numsamples;
    Random r;
    int ss = curve.Size();
    T all = 0;
    for(int i=0;i<s;i++)
      {
	float p = double(r.next())/r.maximum();
	p*=ss;
	T val = curve.get(p);
	all+=val;
      }
    return all;
  }
  T avg() { return sum()/numsamples; }
  T integrate() {
    float s = curve.Size();
    return s*avg();
  }
private:
  PathValues<T> &curve;
  int numsamples;
};

template<class T>
class AreaIntegrator
{
public:
  
};

template<class T>
class VolumeIntegrator
{
public:
  VolumeIntegrator(VolumeValues<T> &curve, int numsamples) : curve(curve), numsamples(numsamples) { }
  T sum() const
  {
    int s = numsamples;
    Random r;
    float ss1 = curve.SizeX();
    float ss2 = curve.SizeY();
    float ss3 = curve.SizeZ();
    T all = 0;
    for(int i=0;i<s;i++)
      {
	float p1 = double(r.next())/r.maximum();
	float p2 = double(r.next())/r.maximum();
	float p3 = double(r.next())/r.maximum();
	p1*=ss1;
	p2*=ss2;
	p3*=ss3;
	T val = curve.get(p1,p2,p3);
	all+=val;
      }
    return all;
  }
  T avg() {
    int s = numsamples;
    Random r;
    float ss1 = curve.SizeX();
    float ss2 = curve.SizeY();
    float ss3 = curve.SizeZ();
    T all = 0;
    int count = 0;
    for(int i=0;i<s;i++)
      {
	float p1 = double(r.next())/r.maximum();
	float p2 = double(r.next())/r.maximum();
	float p3 = double(r.next())/r.maximum();
	p1*=ss1;
	p2*=ss2;
	p3*=ss3;
	T val = curve.get(p1,p2,p3);
	if (val>0.1) {
	  count++;
	  all+=val;
	}
      }
    return all/count;
  }
  T integrate() {
    float s1 = curve.SizeX();
    float s2 = curve.SizeY();
    float s3 = curve.SizeZ();
    return s1*s2*s3*avg();
  }
private:
  VolumeValues<T> &curve;
  int numsamples;
};

class ViewFustrum
{
public:
  ViewFustrum(Point screen_tl, Point screen_tr,
	      Point screen_bl, Point screen_br,

	      Point far_tl, Point far_tr,
	      Point far_bl, Point far_br) : s_tl(screen_tl), s_tr(screen_tr),
					    s_bl(screen_bl), s_br(screen_br),
					    f_tl(far_tl), f_tr(far_tr),
					    f_bl(far_bl), f_br(far_br)
  {
    d_tr_tl = s_tr-s_tl;
    f_tr_tl = f_tr-f_tl;
    d_bl_tl = s_bl-s_tl;
    f_bl_tl = f_bl-f_tl;
    fd_tl_tl = f_tl-s_tl;
  }
  float SizeX() const { return 1.0; }
  float SizeY() const { return 1.0; }
  float SizeZ() const { return 1.0; }
  Point get(float x, float y, float z) const
  {
    Vector v_x = (1.0-z)*x*(d_tr_tl) + z*x*(f_tr_tl);
    Vector v_y = (1.0-z)*y*(d_bl_tl) + z*y*(f_bl_tl);
    Vector v_z = z*(fd_tl_tl);
    Point p = s_tl + v_x + v_y + v_z;
    return p;
  }
private:
  Point s_tl, s_tr;
  Point s_bl, s_br;
  Point f_tl, f_tr;
  Point f_bl, f_br;

  Vector d_tr_tl;
  Vector f_tr_tl;
  Vector d_bl_tl;
  Vector f_bl_tl;
  Vector fd_tl_tl;
};

class ViewGrid
{
public:
  ViewGrid(float screen_width, float screen_height,
	   float screen_z, int scr_x,int scr_y,
	   float far_width, float far_height,
	   float far_z)
    :
    screen_x(scr_x), screen_y(scr_y),
    sx(screen_width), sy(screen_height), sz(screen_z),
    fwidth(far_width), fheight(far_height),
    fz(far_z), view(0)
  {
  }
  ViewFustrum *get(int x, int y) const
  {
    float xx = float(x)/float(screen_x);
    float yy = float(y)/float(screen_y);
    float xx1 = float(x+1)/float(screen_x);
    float yy1 = float(y+1)/float(screen_y);
    xx-=0.5;
    yy-=0.5;
    xx1-=0.5;
    yy1-=0.5;
    float fx = xx;
    float fy = yy;
    float fx1 = xx1;
    float fy1 = yy1;
    xx*=sx;
    yy*=sy;
    fx*=fwidth;
    fy*=fheight;
    xx1*=sx;
    yy1*=sy;
    fx1*=fwidth;
    fy1*=fheight;

    Point s_tl(xx,yy,sz);
    Point s_tr(xx1,yy,sz);
    Point s_bl(xx,yy1,sz);
    Point s_br(xx1,yy1,sz);

    Point f_tl(fx,fy,fz);
    Point f_tr(fx1,fy,fz);
    Point f_bl(fx,fy1,fz);
    Point f_br(fx1,fy1,fz);

    
    delete view;
    view = new ViewFustrum(s_tl,s_tr,s_bl,s_br,
			   f_tl,f_tr,f_bl,f_br);
    return view;
  }
  ~ViewGrid() { delete view; }
private:
  int screen_x, screen_y;
  float sx,sy,sz;
  float fwidth, fheight;
  float fz;
  mutable ViewFustrum *view;
};

class VValues : public VolumeValues<float>
{
public:
  VValues(FloatVolumeObject &vox, int x, int y,
	  float screen_width, float screen_height,
	  float screen_z, int scr_x, int scr_y,
	  float far_width, float far_height,
	  float far_z
	  )
    : vox(vox)
  {
    grid = new ViewGrid(screen_width, screen_height,
			screen_z, scr_x, scr_y,
			far_width, far_height, far_z);
    fustrum = grid->get(x,y);
  }
  ~VValues() { delete grid; }
  float SizeX() const { return 1.0; }
  float SizeY() const { return 1.0; }
  float SizeZ() const { return 1.0; }
  float get(float x, float y, float z) const
  {
    Point p = fustrum->get(x,y,z);
    float val = vox.FloatValue(p);
    return val;
  }
private:
  FloatVolumeObject &vox;
  ViewGrid *grid;
  ViewFustrum *fustrum;
};

class RenderVolume : public Bitmap<float>
{
public:
  RenderVolume(FloatVolumeObject &vox, int sx, int sy, int numsamples) : vox(vox),sx(sx),sy(sy), numsamples(numsamples)
  {
    values = 0;
  }
  ~RenderVolume() { delete values; }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  float Map(int x, int y) const {
    delete values;
    values = new VValues(vox, x,y, 800.0,600.0, 0.0,
			 sx,sy,
			 8000.0,6000.0, 1200.0);
    
    VolumeIntegrator<float> i(*values,numsamples);
    float avg = i.avg();
    return avg;
  }
  void Prepare() { }
private:
  FloatVolumeObject &vox;
  mutable VValues *values;
  int sx,sy;
  int numsamples;
};
GameApi::FB GameApi::FloatVolumeApi::integrate_render(FO obj, int sx, int sy, int numsamples)
{
  FloatVolumeObject *ff = find_float_volume(e, obj);
  return add_float_bitmap(e, new RenderVolume(*ff, sx,sy,numsamples));
}

#if 0
float hemisphere_integrator(Point p, Vector n, float radius, FloatVolumeObject *objs, int numsamples)
{
  Random r;
  int s = numsamples;
  for(int i=0;i<s;i++)
    {
      float r = radius * double(r.next())/r.maximum();
      float alfa = 
    }
}
#endif

class WaveformSphere : public FloatVolumeObject
{
public:
  WaveformSphere(Waveform *wv, float r) : wv(wv), r(r) { }
  float FloatValue(Point p) const
  {
    float rr = p.Dist();
    rr/=r;
    if (rr<0.0) return 0.0;
    if (rr>1.0) return 0.0;
    rr*=wv->Length();
    float val = wv->Index(rr);
    return val;
  }

private:
  Waveform *wv;
  float r;
};

GameApi::FO GameApi::FloatVolumeApi::waveform_sphere(WV wav, float r)
{
  Waveform *wv = find_waveform(e, wav);
  return add_float_volume(e, new WaveformSphere(wv,r));
}

class WVMoveY : public Waveform
{
public:
  WVMoveY(Waveform *next, float delta) : next(next), delta(delta) { }
  float Length() const { return next->Length(); }
  float Min() const { return delta + next->Min(); }
  float Max() const { return delta + next->Max(); }
  float Index(float val) const
  {
    return delta + next->Index(val);
  }
private:
  Waveform *next;
  float delta;
};
GameApi::WV GameApi::FloatVolumeApi::wave_move_y(WV wav, float delta)
{
  Waveform *wv = find_waveform(e, wav);
  return add_waveform(e, new WVMoveY(wv, delta));
}
