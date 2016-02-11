
#include "GameApi_h.hh"


EXPORT GameApi::VectorApi::VectorApi(Env &e) : e(e) { }

EXPORT GameApi::V GameApi::VectorApi::null_vector()
{
  return add_vector(e,0.0,0.0,0.0);
}
EXPORT GameApi::V GameApi::VectorApi::vector(float dx, float dy, float dz)
{
  return add_vector(e,dx,dy,dz);
}
EXPORT GameApi::V GameApi::VectorApi::sum(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector res = *vv1+*vv2;
  return add_vector(e,res.dx,res.dy,res.dz);
}
EXPORT GameApi::V GameApi::VectorApi::mul(V v1, float scalar)
{
  Vector *vv1 = find_vector(e,v1);
  Vector res = scalar * (*vv1);
  return add_vector(e,res.dx,res.dy,res.dz);
}
EXPORT float GameApi::VectorApi::dot(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  return Vector::DotProduct(*vv1,*vv2);
}
EXPORT GameApi::V GameApi::VectorApi::cross(V v1, V v2)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vv2 = find_vector(e,v2);
  Vector v = Vector::CrossProduct(*vv1,*vv2);
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT float GameApi::VectorApi::projection_length(V v1, V u_x)
{
  Vector *vv1 = find_vector(e,v1);
  Vector *vu_x = find_vector(e,u_x);
  return Vector::FindProjectionLength(*vv1, *vu_x);
}

EXPORT GameApi::V GameApi::VectorApi::projection_1(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.first;
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT GameApi::V GameApi::VectorApi::projection_2(V u, V u_x)
{
  Vector *vv1 = find_vector(e,u);
  Vector *vu_x = find_vector(e,u_x);
  std::pair<Vector,Vector> res = Vector::SplitToComponents(*vv1, *vu_x);
  Vector v = res.second;
  return add_vector(e, v.dx, v.dy, v.dz);
}

EXPORT GameApi::V GameApi::VectorApi::neg(V v) 
{
  Vector *vv1 = find_vector(e,v);
  Vector vx = -(*vv1);
  return add_vector(e,vx.dx, vx.dy, vx.dz);
}
