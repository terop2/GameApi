// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//

#include "VectorTools.hh"

class VectorField
{
public:
  virtual Vector Field(Point p, float time) const=0;
};


class ScalarField
{
public:
  virtual float Field(Point p, float time) const=0;
};

class GradientField : public VectorField
{
public:
  GradientField(ScalarField &field) : field(field) {  }
  Vector Field(Point p, float time) const
  {
    Vector dx = Vector(0.001,0.0,0.0);
    Vector dy = Vector(0.0,0.001,0.0);
    Vector dz = Vector(0.0,0.0,0.001);
    
    Vector v;
    v.x = field.Field(p+dx, time)-field.Field(p, time);
    v.y = field.Field(p+dy, time)-field.Field(p, time);
    v.z = field.Field(p+dz, time)-field.Field(p, time);
    v.x /= 0.001;
    v.y /= 0.001;
    v.z /= 0.001;
    return v;
  }
private:
  ScalarField &field;
};

VectorField *Gradient(ScalarField *field)
{
  return new GradientField(*field);
}

class DivergenceField : public ScalarField
{
public:
  DivergenceField(VectorField &field) : field(field) {  }
  float Field(Point p, float time) const
  {
    Point px = p + Vector(0.001,0.0,0.0);
    Point py = p + Vector(0.0, 0.001, 0.0);
    Point pz = p + Vector(0.0,0.0,0.001);
    Vector vx = field.Field(px, time)-field.Field(p, time);
    Vector vy = field.Field(py, time)-field.Field(p, time);
    Vector vz = field.Field(pz, time)-field.Field(p, time);
    return vx.x/0.001+vy.y/0.001+vz.z/0.01;
  }
private:
  VectorField &field;
};

ScalarField *Divergence(VectorField *field)
{
  return new DivergenceField(*field);
}
class TensorDivergence : public Vector
{
public:
  TensorDivergence(Tensor &t) : t(t) { }
  // tr(del v)
private:
  Tensor &t;
};

class Curl : public XXXX
{
public:
  Curl(VectorField &v) : v(v) {  }
};

class VectorLaplacian : public Vector
{
public:
  VectorLaplacian(VectorField &field) 
    : field(field),
      d(field),g(d), c1(field), c2(c1)
  { }
  void Update(Point p, float time)
  {
    return g.Field(p, time) - c2.Field(p, time);
  }
private:
  VectorField &field;
  DivergenceField d; // (D(_D_.A))
  GradientField g; //(_D_(D.A))
  Curl c1;      // D x (_D_ x A)
  Curl c2;      // _D_ x (D x A)  
};


class TensorDerivate : public Vector
{
public:
  TensorDerivate(VectorField &field, Vector u, Point P) : field(field), u(u), P(P) { }
  void Update(float time)
  {
    Vector v1 = field.Field(P, time);
    Vector v2 = field.Field(P+0.001*u, time);
    Vector v = v2-v1;
    v /= 0.001;
    dx = v.dx;
    dy = v.dy;
    dz = v.dz;
  }
private:
  VectorField &field;
  Vector u;
  Point P;
};

class Derivative_T : public VectorField
{
public:
  Derivative(VectorField &v) : v(v) { }
  Vector Field(Point p, float time) const
  {
    Vector vv = v.Field(p, time+0.01)-v.Field(p, time);
    return vv;
  }
private:
  VectorField &v;
};

class Advection : public Vector
{
public:
  Advection(Vector v, VectorField &field) : v(v), field(field) { }
  void Update(float time)
  {
    Vector ux = field.Field(p+Vector(0.001,0.0,0.0), time)-field.Field(p,time);
    Vector uy = field.Field(p+Vector(0.0,0.001,0.0), time)-field.Field(p,time);
    Vector uz = field.Field(p+Vector(0.0,0.0,0.001), time)-field.Field(p,time);
    ux *= v.dx;
    uy *= v.dy;
    uz *= v.dz;
    Vector vv = ux+uy+uz;
    dx = vv.dx;
    dy = vv.dy;
    dz = vv.dz;
  }
 private:
  Vector v;
  VectorField &field;
};

template<class T>
class Equation
{
public:
  T left_side() const=0;
  T right_side() const=0;
  bool compare(T,T) const=0;
};

class NavierStokesEquation : public Equation<Vector>
{
public:
  NavierStokesEquation(float density, 
		       VectorField &flow_velocity, 
		       ScalarField &pressure, 
		       VectorField &stress_tensor,
		       float time,
		       Point point,
		       float viscosity,
		       Vector body_forces)
    : density(density),
      flow_velocity(flow_velocity), 
      pressure(pressure), 
      time(time),
      point(point),
      viscosity(viscosity),
      body_forces(body_forces),
      stress_tensor(stress_tensor), der(flow_velocity), adv(flow_velocity.Field(point, time), flow_velocity) { }

  Vector left_side() const
  {
    return density*(der.Field(point, time)+adv);
  }
  Vector right_side() const
  {
    return -grad_p.Field(point, time)+viscosity*lap + body_forces;
  }
  bool compare(Vector a, Vector b) const
  {
    return a.dx==b.dx && a.dy==b.dy && a.dz == b.dz;
  }
private:
  Derivative_T der;
  Advection adv;
  GradientField grad_p;
  DivergenceField div_T;
  float viscosity;
  Vector body_forces;
};
