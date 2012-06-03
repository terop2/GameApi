
#include "Effect.hh"

class Derivative : public Function<float,float>
{
public:
  Derivative(Function<float,float> &f, float dx) : f(f), dx(dx) { }
  float Index(float x) const
  {
    return (f.Index(x+dx)-f.Index(x))/dx;
  }
private:
  Function<float, float> &f;
  float dx;
};

class Integrate : public Function<float,float>
{
public:
  Integrate(Function<float,float> &f, float start_x, float end_x, float dx) : f(f), start_x(start_x), end_x(end_x), dx(dx) { }
  float Index(float x) const
  {
    float val = 0.0;
    for(float x = start_x; x<end_x;x+=dx)
      {
	val += f.Index(x)*dx;
      }
    return val;
  }
private:
  Function<float, float> &f;
  float start_x;
  float end_x;
  float dx;
};

struct P
{
  float x;
  float y;
  float z;
  float t;
};

class Derivative_x : public Function<P, float>
{
public:
  Derivative_x(Function<P, float> &f, float dx) : f(f) , dx(dx) {}
  float Index(P p) const
  {
    P pp;
    pp.x = p.x + dx;
    pp.y = p.y;
    pp.z = p.z;
    pp.t = p.t;
    float val = f.Index(pp);
    pp.x = p.x;
    float val2 = f.Index(pp);
    return (val-val2)/dx;
  }
private:
  Function<P, float> &f;
  float dx;
};
class Derivative_xx : public Function<P, float>
{
public:
  Derivative_xx(Function<P, float> &f, float dx1, float dx2) : d1(f, dx1), d2(d1, dx2) { }
  float Index(P p) const { return d2.Index(p); }
private:
  Derivative_x d1;
  Derivative_x d2;
};

class Derivative_y : public Function<P, float>
{
public:
  Derivative_x(Function<P, float> &f, float dy) : f(f) , dy(dy) {}
  float Index(P p) const
  {
    P pp;
    pp.x = p.x;
    pp.y = p.y + dy;
    pp.z = p.z;
    pp.t = p.t;
    float val = f.Index(pp);
    pp.y = p.y;
    float val2 = f.Index(pp);
    return (val-val2)/dy;
  }
private:
  Function<P, float> &f;
  float dy;
};

class Derivative_yy : public Function<P, float>
{
public:
  Derivative_yy(Function<P, float> &f, float dx1, float dx2) : d1(f, dx1), d2(d1, dx2) { }
  float Index(P p) const { return d2.Index(p); }
private:
  Derivative_y d1;
  Derivative_y d2;
};


class Derivative_z : public Function<P, float>
{
public:
  Derivative_z(Function<P, float> &f, float dz) : f(f) , dz(dz) {}
  float Index(P p) const
  {
    P pp;
    pp.x = p.x;
    pp.y = p.y;
    pp.z = p.z + dz;
    pp.t = p.t;
    float val = f.Index(pp);
    pp.z = p.z;
    float val2 = f.Index(pp);
    return (val-val2)/dz;
  }
private:
  Function<P, float> &f;
  float dz;
};

class Derivative_zz : public Function<P, float>
{
public:
  Derivative_zz(Function<P, float> &f, float dx1, float dx2) : d1(f, dx1), d2(d1, dx2) { }
  float Index(P p) const { return d2.Index(p); }
private:
  Derivative_z d1;
  Derivative_z d2;
};



class Derivative_t : public Function<P, float>
{
public:
  Derivative_t(Function<P, float> &f, float dt) : f(f) , dt(dt) {}
  float Index(P p) const
  {
    P pp;
    pp.x = p.x;
    pp.y = p.y;
    pp.z = p.z;
    pp.t = p.t + dt;
    float val = f.Index(pp);
    pp.t = p.t;
    float val2 = f.Index(pp);
    return (val-val2)/dt;
  }
private:
  Function<P, float> &f;
  float dt;
};

class Derivative_tt : public Function<P, float>
{
public:
  Derivative_tt(Function<P, float> &f, float dx1, float dx2) : d1(f, dx1), d2(d1, dx2) { }
  float Index(P p) const { return d2.Index(p); }
private:
  Derivative_t d1;
  Derivative_t d2;
};


template<class A, class B>
class Equation
{
public:
  virtual B f(A a) const=0;
  virtual B g(A a) const=0;
};

class Bools : public Function<std::pair<float,float>, bool>
{
public:
  bool Index(std::pair<float,float> p) const
  {
    return p.first < p.second;
  }
};

class Intersection : public Function<std::pair<bool,bool>, bool>
{
public:
  bool Index(std::pair<bool,bool> p) const
  {
    return p.first && p.second;
  }
};

struct NavierStokesVariables
{
public:
  float p;
  Function<P,float> *u;
  Function<P,float> *v;
  Function<P,float> *w;
  Function<P,float> *pp;
  float my;
  float g_x;
  float g_y;
  float g_z;
  P pos;
};

class NavierStokesEquation_1 : public Equation<NavierStokesVariables, float>
{
public:
  float f(NavierStokesVariables v) const
  {
    Derivative_t Dt(*v.u, 0.01);
    Derivative_x Dx(*v.u, 0.01);
    Derivative_y Dy(*v.u, 0.01);
    Derivative_z Dz(*v.u, 0.01);
    return v.p*(Dt.Index(v.pos) + v.u->Index(v.pos) * Dx.Index(v.pos) + v.v->Index(v.pos) * Dy.Index(v.pos) + v.w->Index(v.pos) * Dz.Index(v.pos)); 
  }
  float g(NavierStokesVariables v) const
  {
    Derivative_x Dpx(*v.pp, 0.01);
    Derivative_xx Dux(*v.u, 0.01, 0.01);
    Derivative_yy Duy(*v.u, 0.01, 0.01);
    Derivative_zz Duz(*v.u, 0.01, 0.01);
    return -Dpx.Index(v.pos)+v.my*(Dux.Index(v.pos)+Duy.Index(v.pos)+Duz.Index(v.pos)) + v.p*v.g_x;
  }
};

class NavierStokesEquation_2 : public Equation<NavierStokesVariables, float>
{
public:
  float f(NavierStokesVariables v) const
  {
    Derivative_t Dt(*v.u, 0.01);
    Derivative_x Dx(*v.u, 0.01);
    Derivative_y Dy(*v.u, 0.01);
    Derivative_z Dz(*v.u, 0.01);
    return v.p*(Dt.Index(v.pos) + v.u->Index(v.pos) * Dx.Index(v.pos) + v.v->Index(v.pos) * Dy.Index(v.pos) + v.w->Index(v.pos) * Dz.Index(v.pos)); 
  }
  float g(NavierStokesVariables v) const
  {
    Derivative_x Dpx(*v.pp, 0.01);
    Derivative_xx Dux(*v.u, 0.01, 0.01);
    Derivative_yy Duy(*v.u, 0.01, 0.01);
    Derivative_zz Duz(*v.u, 0.01, 0.01);
    return -Dpx.Index(v.pos)+v.my*(Dux.Index(v.pos)+Duy.Index(v.pos)+Duz.Index(v.pos)) + v.p*v.g_y;
  }
};

class NavierStokesEquation_3 : public Equation<NavierStokesVariables, float>
{
public:
  float f(NavierStokesVariables v) const
  {
    Derivative_t Dt(*v.u, 0.01);
    Derivative_x Dx(*v.u, 0.01);
    Derivative_y Dy(*v.u, 0.01);
    Derivative_z Dz(*v.u, 0.01);
    return v.p*(Dt.Index(v.pos) + v.u->Index(v.pos) * Dx.Index(v.pos) + v.v->Index(v.pos) * Dy.Index(v.pos) + v.w->Index(v.pos) * Dz.Index(v.pos)); 
  }
  float g(NavierStokesVariables v) const
  {
    Derivative_x Dpx(*v.pp, 0.01);
    Derivative_xx Dux(*v.u, 0.01, 0.01);
    Derivative_yy Duy(*v.u, 0.01, 0.01);
    Derivative_zz Duz(*v.u, 0.01, 0.01);
    return -Dpx.Index(v.pos)+v.my*(Dux.Index(v.pos)+Duy.Index(v.pos)+Duz.Index(v.pos)) + v.p*v.g_z;
  }
};

class ContinuityEquation : public Equation<NavierStokesVariables, float>
{
public:
  float f(NavierStokesVariables v) const
  {
    Derivative_x Dx(*v.u, 0.01);
    Derivative_y Dy(*v.u, 0.01);
    Derivative_z Dz(*v.u, 0.01);
    return Dx.Index(v.pos)+Dy.Index(v.pos)+Dz.Index(v.pos);
  }
  float g(NavierStokesVariables v) const
  {
    return 0.0;
  }
};
