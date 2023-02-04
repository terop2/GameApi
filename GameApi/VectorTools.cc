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
#include "Graph.hh"
#include <cmath>
#include <iostream>
#include <ostream>
#include <algorithm>
#include <vector>
std::ostream &operator<<(std::ostream &o, const Point &p)
{
  o << "(" << p.x << "," << p.y << "," << p.z << ")";
  return o;
}
std::istream &operator>>(std::istream &i, Point &p)
{
  char ch;
  i >> ch >> p.x >> ch >> p.y >> ch >> p.z >> ch;
  return i;
}
std::ostream &operator<<(std::ostream &o, const Point2d &p)
{
  o << "(" << p.x << "," << p.y << ")";
  return o;
}
std::istream &operator>>(std::istream &i, Point2d &p)
{
  char ch;
  i >> ch >> p.x >> ch >> p.y >> ch;
  return i;
}


std::ostream &operator<<(std::ostream &o, const Vector &v)
{
  o << "(" << v.dx << "," << v.dy << "," << v.dz << ")";
  return o;
}

std::istream &operator>>(std::istream &i, Vector &v)
{
  char ch;
  i >> ch >> v.dx >> ch >> v.dy >> ch >> v.dz >> ch;
  return i;
}


std::ostream &operator<<(std::ostream &o, const Matrix &m)
{
  o << "[" << m.matrix[0] << "," << m.matrix[1] << "," << m.matrix[2] << "," << m.matrix[3] << "]" << std::endl;
  o << "[" << m.matrix[4] << "," << m.matrix[5] << "," << m.matrix[6] << "," << m.matrix[7] << "]" << std::endl;
  o << "[" << m.matrix[8] << "," << m.matrix[9] << "," << m.matrix[10] << "," << m.matrix[11] << "]" << std::endl;
  o << "[" << m.matrix[12] << "," << m.matrix[13] << "," << m.matrix[14] << "," << m.matrix[15] << "]" << std::endl;
  return o;
}


float Dist(const Point &a, const Point &b) { return (b-Vector(a)).Dist(); }


float Vector::Angle(const Vector &aVec, const Vector &aVec2)
{
  return acos(DotProduct(aVec, aVec2)/(aVec.Dist()*aVec2.Dist()));
}


Point SphericalPoint::ToPoint()
{
  Point p;
  p.x = r*sin(alfa)*cos(beta);
  p.y = r*sin(alfa)*sin(beta);
  p.z = r*cos(alfa);
  p += center;
  return p;
}

void SphericalPoint::FromPoint(Point v)
{
  v-=center;
  r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  alfa = acos(v.z/r);
  beta = atan2(v.y,v.x);
}


Point::Point(const Vector &v)
  : x(v.dx), y(v.dy), z(v.dz) { }


void Point::operator+=(const Vector &v)
{
  //Vector vv = v;
  //vv.Normalize();
  //Normalize();
  x += v.dx;
  y += v.dy;
  z += v.dz;
  //s = 1.0;
}
Point operator+(const Point &p, const Vector &v)
{
  Point pp = p;
  pp += v;
  return pp;
}

void Point::operator-=(const Vector &v)
{
 x -= v.dx;
 y -= v.dy;
 z -= v.dz;
}
Point operator-(const Point &p, const Vector &v)
{
  Point pp = p;
  pp -= v;
  return pp;
}


float Vector::Dist() const
{
  float r = dx*dx;
  r+= dy*dy;
  r+= dz*dz;
  float r2 = sqrt(r);
  return r2;
}
float Point::Dist() const
{
  return sqrt(x*x+y*y+z*z);
}

Vector Vector::UnitX()
{
  Vector v;
  v.dx = 1.0;
  v.dy = 0.0;
  v.dz = 0.0;
  return v;
}
Vector Vector::UnitY()
{
  Vector v;
  v.dx = 0.0;
  v.dy = 1.0;
  v.dz = 0.0;
  return v;
}
Vector Vector::UnitZ()
{
  Vector v;
  v.dx = 0.0;
  v.dy = 0.0;
  v.dz = 1.0;
  return v;
}

Vector Vector::UnitConvert(const Vector &u_x, const Vector &u_y, const Vector &u_z)
{
  // r = u_x*x + u_y*y + u_z*z
  Vector u_x_x = u_x * dx;
  Vector u_y_y = u_y * dy;
  Vector u_z_z = u_z * dz;
  Vector r = u_x_x + u_y_y + u_z_z;
  return r;
}


Point operator*(const Point &p, const Matrix &m)
{
  Vector v(p);
  Vector r = v * m;
  Point pp(r);
  return pp;
}

Vector operator*(const Vector &v, const Matrix &m)
{
  Vector r;
  r.dx = v.dx*m.matrix[0] + v.dy*m.matrix[1] + v.dz*m.matrix[2] + m.matrix[3];
  r.dy = v.dx*m.matrix[4] + v.dy*m.matrix[5] + v.dz*m.matrix[6] + m.matrix[7];
  r.dz = v.dx*m.matrix[8] + v.dy*m.matrix[9] + v.dz*m.matrix[10] + m.matrix[11];
  
  float val = v.dx*m.matrix[12] + v.dy*m.matrix[13] + v.dz*m.matrix[14] + m.matrix[15];
  r.dx /= val;
  r.dy /= val;
  r.dz /= val;
  //r.ds = 
  return r;
}

Matrix operator*(const Matrix &m1, const Matrix &m2)
{
  Matrix r;
  //if (m1.is_identity && m2.is_identity)
  //  {
  //    r = Matrix::Identity();
  //    return r;
  //  }
  for(int x=0;x<4;x++)
    for(int y=0;y<4;y++) {
      int i = x+y*4;
      int yy = y*4;
      r.matrix[i]=0;
      for(int z=0;z<4;z++)
	r.matrix[i] += m1.matrix[x+z*4]*m2.matrix[z+yy];
    }
  return r;
}

Matrix Matrix::Zero()
{
  Matrix r = { { 0.0, 0.0, 0.0, 0.0,
	       0.0, 0.0, 0.0, 0.0,
	       0.0, 0.0, 0.0, 0.0,
	       0.0, 0.0, 0.0, 1.0 } };
  //r.is_identity = false;
  return r;
}

Matrix Matrix::Identity()
{
  Matrix r = { { 1.0, 0.0, 0.0, 0.0,
	       0.0, 1.0, 0.0, 0.0,
	       0.0, 0.0, 1.0, 0.0,
	       0.0, 0.0, 0.0, 1.0 } };
  //r.is_identity = true;
  return r;
}


Matrix Matrix::XRotation(float rot)
{
  float sin_r = sin(rot);
  float cos_r = cos(rot);
  Matrix r = { { 1.0, 0.0, 0.0, 0.0,
	       0.0, cos_r, -sin_r, 0.0,
	       0.0, sin_r, cos_r, 0.0,
	       0.0, 0.0, 0.0, 1.0 } };
  return r;
}

Matrix Matrix::KeepRotation(const Matrix &m)
{
  Matrix r = { { m.matrix[0], m.matrix[1], m.matrix[2], 0.0,
	       m.matrix[4], m.matrix[5], m.matrix[6], 0.0,
	       m.matrix[8], m.matrix[9], m.matrix[10], 0.0,
	       0.0, 0.0, 0.0, 1.0 } };
  return r;
}


Matrix Matrix::YRotation(float rot)
{
  float sin_r = sin(rot);
  float cos_r = cos(rot);
  Matrix r = { {cos_r, 0.0, sin_r, 0.0,
	       0.0, 1.0, 0.0, 0.0,
	       -sin_r, 0.0, cos_r, 0.0,
	       0.0, 0.0, 0.0, 1.0} };
  return r;
}

Matrix Matrix::ZRotation(float rot)
{
  float sin_r = sin(rot);
  float cos_r = cos(rot);
  Matrix r = { { cos_r, -sin_r, 0.0, 0.0,
	       sin_r, cos_r, 0.0, 0.0,
	       0.0, 0.0, 1.0, 0.0,
	       0.0, 0.0, 0.0, 1.0} };
  return r;
}
Matrix Matrix::Translate(float x, float y, float z)
{
  Matrix r = { { 1.0, 0.0, 0.0, x,
	       0.0, 1.0, 0.0, y,
	       0.0, 0.0, 1.0, z,
	       0.0, 0.0, 0.0, 1.0} };
  return r;
}

Matrix Matrix::Scale(float x, float y, float z)
{
  Matrix r = { {x, 0.0, 0.0, 0.0,
	       0.0, y, 0.0, 0.0,
	       0.0, 0.0, z, 0.0,
	       0.0, 0.0, 0.0, 1.0} };
  return r;
}

Matrix Matrix::ProjectionTrans(float z_min)
{
  Matrix r = { {1.0, 0.0, 0.0, 0.0,
	       0.0, 1.0, 0.0, 0.0,
	       0.0, 0.0, float(1.0/(1.0+z_min)), float(-z_min/(1.0+z_min)),
	       0.0, 0.0, -1.0, 0.0} };
  return r;
}

Matrix Matrix::PerspectiveProjection(float dist)
{
  //float F = dist;
  //float B = 2000.0;
  Matrix r = { {1.0, 0.0, 0.0, 0.0,
	       0.0, 1.0, 0.0, 0.0,
	       0.0, 0.0, 0.0/*(B+F)/(B-F)*/, 0.0/*-2.0*B*F/(B-F)*/,
	       0.0, 0.0, float(1.0/dist), 1.0} };
  return r;
}

#undef far
#undef near

Matrix Matrix::Perspective2(float l, float r,
			    float b, float t,
			    float n, float f)
{
  float c00 = 2.0*n/(r-l);
  float c11 = 2.0*n/(t-b);
  float c22 = -(f+n)/(f-n);
  //float c33 = 0.0;
  float c20 = (r+l)/(r-l);
  float c21 = (t+b)/(t-b);
  float c32 = -2.0*(f+n)/(f-n);
  float c23 = -1.0;

  Matrix m = { { c00, 0.0, c20, 0.0,
		 0.0, c11, c21, 0.0,
		 0.0, 0.0, c22, c32,
		 0.0, 0.0, c23, 0.0 } };
  return m;
}

Matrix Matrix::Perspective(float fovy, float aspect, float near, float far)
{
  float f = -1.0/tan(fovy/2.0);
  float ff = (far+near)/(near-far);
  float fff = 2.0*far*near/(near-far);
  Matrix r = { {f/aspect, 0.0, 0.0, 0.0,
  	0.0, f, 0.0, 0.0,
  	0.0, 0.0, ff, fff,
  	0.0, 0.0, -1.0, 0.0 } };

  //Matrix r = { {f/aspect, 0.0, 0.0, 0.0,
  //		0.0, f, 0.0, 0.0,
  //		0.0, 0.0, (far+near)/(near-far), -1.0,
  //		0.0, 0.0, 2.0*far*near/(near-far), 0.0 }, false };
  return r;
}

Matrix Matrix::Ortho(float left, float right, float bottom, float top, float near, float far)
{
  float t_x = (right+left)/(right-left);
  float t_y = (top+bottom)/(top-bottom);
  float t_z = (far+near)/(far-near);
  //std::cout << "Ortho:" << t_x << " " << t_y << " " << t_z << std::endl;
  Matrix m = { { 2.0f/(right-left), 0.0f, 0.0f, -t_x,
		 0.0f, 2.0f/(top-bottom), 0.0f, -t_y,
		 0.0f, 0.0f, -2.0f/(far-near),-t_z,
		 0.0f, 0.0f, 0.0f, 1.0f } };
  return m;
}


Matrix Matrix::SphericalToCartesian(SphericalPoint p)
{
  float alfa = p.alfa;
  float beta = p.beta;
  float cosalfa = cos(alfa);
  float sinalfa = sin(alfa);
  float cosbeta = cos(beta);
  float sinbeta = sin(beta);
  float r = p.r;
  Matrix rr = { {sinalfa*cosbeta, r*cosalfa*cosbeta, -r*sinalfa*sinbeta, 0,
	       sinalfa*sinbeta, r*cosalfa*sinbeta, r*sinalfa*cosbeta, 0,
	       cosalfa, -r*sinalfa, 0.0, 0.0,
	       0.0,0.0,0.0,1.0} };
  return rr;
}

Matrix Matrix::CartesianToSpherical(Point p)
{
  float x = p.x;
  float y = p.y;
  float z = p.z;
  float x_x_y_y = x*x+y*y;
  float sqrt_x_x_y_y = sqrt(x_x_y_y);
  float r = sqrt(x_x_y_y+z*z);
  Matrix rr = { {x/r, y/r, z/r, 0,
	       x*z/r/sqrt_x_x_y_y, y*z/r/sqrt_x_x_y_y, -(x_x_y_y)/r/sqrt_x_x_y_y, 0
	       -y/sqrt_x_x_y_y, x/sqrt_x_x_y_y, 0.0, 0.0,
	       0, 0, 0, 1.0} };
  return rr;
}

Matrix Matrix::MultiplyMatrix(const Matrix &m, int count)
{
  Matrix mm = m;
  for(int i=0;i<count;i++)
    {
      mm = mm * m;
    }
  return mm;
}

Matrix Matrix::RotateAroundAxisPoint(const Point &p,
				     const Vector &v,
				     float angle)
{
  return Matrix::Translate(-p.x,-p.y,-p.z)*RotateAroundAxis(v,angle)*Matrix::Translate(p.x,p.y,p.z);
}

Matrix Matrix::RotateAroundAxis(const Vector &v, float angle)
{
  float cosa = cos(angle);
  float sina = sin(angle);
  Vector vv = v;
  vv /= vv.Dist();
  float x = vv.dx;
  float y = vv.dy;
  float z = vv.dz;
  Matrix r = { { float(cosa+(1.0-cosa)*x*x), float((1.0-cosa)*x*y-sina*z), float((1.0-cosa)*x*z+sina*y), 0.0,
	       float((1.0-cosa)*y*x+sina*z), float(cosa+(1.0-cosa)*y*y), float((1.0-cosa)*y*z-sina*x), 0.0,
	       float((1.0-cosa)*z*x-sina*y), float((1.0-cosa)*z*y+sina*x), float(cosa+(1.0-cosa)*z*z), 0.0,
	       0.0, 0.0, 0.0, 1.0 } };
  return r;
}

struct Mat2 { float m[4]; };

std::ostream &operator<<(std::ostream &o, const Mat2 &m)
{
  o << m.m[0] << " " << m.m[1] << std::endl;
  o << m.m[2] << " " << m.m[3] << std::endl;
  return o;
}

//static float det2(const float *m[])
//{
//  return (*m[0])*(*m[3]) - (*m[1])*(*m[2]);
//}
static float det2(const Mat2 &m)
{
  float f = m.m[0]*m.m[3] - (m.m[1]*m.m[2]); 
  return f;
}
static Mat2 mult(const Mat2 &m, const Mat2 &m2)
{
  Mat2 r;
  r.m[0] = m.m[0]*m2.m[0] + m.m[1]*m2.m[2];
  r.m[1] = m.m[0]*m2.m[1] + m.m[1]*m2.m[3];
  r.m[2] = m.m[2]*m2.m[0] + m.m[3]*m2.m[2];
  r.m[3] = m.m[2]*m2.m[1] + m.m[3]*m2.m[3];

  return r;
}
static Mat2 ref(const float *m[])
{
  Mat2 r;
  r.m[0] = *m[0];
  r.m[1] = *m[1];
  r.m[2] = *m[2];
  r.m[3] = *m[3];

  return r;
}
static Mat2 minus(const Mat2 &m, const Mat2 &m2)
{
  Mat2 r;
  r.m[0] = m.m[0]-m2.m[0];
  r.m[1] = m.m[1]-m2.m[1];
  r.m[2] = m.m[2]-m2.m[2];
  r.m[3] = m.m[3]-m2.m[3];
  return r;
}
static Mat2 inv(const Mat2 &m)
{
  float a = m.m[0];
  float b = m.m[1];
  float c = m.m[2];
  float d = m.m[3];
  float dd = 1.0/(a*d-b*c);
  if (a*d-b*c < 0.00001 && a*d-b*c > -0.00001) 
    dd = 1.0;
  Mat2 r;
  r.m[0]= d*dd;
  r.m[1]= -b*dd;
  r.m[2]= -c*dd;
  r.m[3]= a*dd;
  return r;
}
static float det(const Matrix &m)
{
  const float *A[] = { &m.matrix[0], &m.matrix[1],
	       &m.matrix[4], &m.matrix[5] };
  const float *B[] = { &m.matrix[2], &m.matrix[3],
	       &m.matrix[6], &m.matrix[7] };

  const float *C[] = { &m.matrix[8], &m.matrix[9],
	       &m.matrix[12], &m.matrix[13] };

  const float *D[] = { &m.matrix[10], &m.matrix[11],
	       &m.matrix[14], &m.matrix[15] };

  return det2(ref(A))*det2(minus(ref(D),mult(mult(ref(C),inv(ref(A))), ref(B))));

}
float Matrix::Determinant(const Matrix &m) {
  return det(m);
}

static float det(const float *m[])
{
  int a=0;
  int b=1;
  int c=2;
  int d=3;
  int e=4;
  int f=5;
  int g=6;
  int h=7;
  int i=8;
  return ((*m[a])*(*m[e])*(*m[i]) +
	  (*m[b])*(*m[f])*(*m[g]) +
	  (*m[c])*(*m[d])*(*m[h])) 
    -
    ((*m[g])*(*m[e])*(*m[c]) +
     (*m[h])*(*m[f])*(*m[a]) +
     (*m[i])*(*m[d])*(*m[b])) ;
}

/*
static float det(const float *m)
{
  int a=0;
  int b=1;
  int c=2;
  int d=3;
  int e=4;
  int f=5;
  int g=6;
  int h=7;
  int i=8;
  return ((m[a])*(m[e])*(m[i]) +
	  (m[b])*(m[f])*(m[g]) +
	  (m[c])*(m[d])*(m[h])) 
    -
    ((m[g])*(m[e])*(m[c]) +
     (m[h])*(m[f])*(m[a]) +
     (m[i])*(m[d])*(m[b])) ;
}
*/

Matrix Matrix::Inverse(const Matrix &m)
{
  const float *m11[] = { &m.matrix[5], &m.matrix[6], &m.matrix[7],
		   &m.matrix[9], &m.matrix[10], &m.matrix[11],
		   &m.matrix[13], &m.matrix[14], &m.matrix[15] };
  const float *m21[] = { &m.matrix[1], &m.matrix[2], &m.matrix[3],
		   &m.matrix[9], &m.matrix[10], &m.matrix[11],
		   &m.matrix[13], &m.matrix[14], &m.matrix[15] };
  const float *m31[] = { &m.matrix[1], &m.matrix[2], &m.matrix[3],
		   &m.matrix[5], &m.matrix[6], &m.matrix[7],
		   &m.matrix[13], &m.matrix[14], &m.matrix[15] };
  const float *m41[] = { &m.matrix[1], &m.matrix[2], &m.matrix[3],
		   &m.matrix[5], &m.matrix[6], &m.matrix[7],
		   &m.matrix[9], &m.matrix[10], &m.matrix[11] };


  const float *m12[] = { &m.matrix[4], &m.matrix[6], &m.matrix[7],
		   &m.matrix[8], &m.matrix[10], &m.matrix[11],
		   &m.matrix[12], &m.matrix[14], &m.matrix[15] };
  const float *m22[] = { &m.matrix[0], &m.matrix[2], &m.matrix[3],
		   &m.matrix[8], &m.matrix[10], &m.matrix[11],
		   &m.matrix[12], &m.matrix[14], &m.matrix[15] };
  const float *m32[] = { &m.matrix[0], &m.matrix[2], &m.matrix[3],
		   &m.matrix[4], &m.matrix[6], &m.matrix[7],
		   &m.matrix[12], &m.matrix[14], &m.matrix[15] };
  const float *m42[] = { &m.matrix[0], &m.matrix[2], &m.matrix[3],
		   &m.matrix[4], &m.matrix[6], &m.matrix[7],
		   &m.matrix[8], &m.matrix[10], &m.matrix[11] };


  const float *m13[] = { &m.matrix[4], &m.matrix[5], &m.matrix[7],
		   &m.matrix[8], &m.matrix[9], &m.matrix[11],
		   &m.matrix[12], &m.matrix[13], &m.matrix[15] };
  const float *m23[] = { &m.matrix[0], &m.matrix[1], &m.matrix[3],
		   &m.matrix[8], &m.matrix[9], &m.matrix[11],
		   &m.matrix[12], &m.matrix[13], &m.matrix[15] };
  const float *m33[] = { &m.matrix[0], &m.matrix[1], &m.matrix[3],
		   &m.matrix[4], &m.matrix[5], &m.matrix[7],
		   &m.matrix[12], &m.matrix[13], &m.matrix[15] };
  const float *m43[] = { &m.matrix[0], &m.matrix[1], &m.matrix[3],
		   &m.matrix[4], &m.matrix[5], &m.matrix[7],
		   &m.matrix[8], &m.matrix[9], &m.matrix[11] };


  const float *m14[] = { &m.matrix[4], &m.matrix[5], &m.matrix[6],
		   &m.matrix[8], &m.matrix[9], &m.matrix[10],
		   &m.matrix[12], &m.matrix[13], &m.matrix[14] };
  const float *m24[] = { &m.matrix[0], &m.matrix[1], &m.matrix[2],
		   &m.matrix[8], &m.matrix[9], &m.matrix[10],
		   &m.matrix[12], &m.matrix[13], &m.matrix[14] };
  const float *m34[] = { &m.matrix[0], &m.matrix[1], &m.matrix[2],
		   &m.matrix[4], &m.matrix[5], &m.matrix[6],
		   &m.matrix[12], &m.matrix[13], &m.matrix[14] };
  const float *m44[] = { &m.matrix[0], &m.matrix[1], &m.matrix[2],
		   &m.matrix[4], &m.matrix[5], &m.matrix[6],
		   &m.matrix[8], &m.matrix[9], &m.matrix[10] };

  float M11 = det(m11);
  float M12 = det(m12);
  float M13 = det(m13);
  float M14 = det(m14);

  float M21 = det(m21);
  float M22 = det(m22);
  float M23 = det(m23);
  float M24 = det(m24);

  float M31 = det(m31);
  float M32 = det(m32);
  float M33 = det(m33);
  float M34 = det(m34);

  float M41 = det(m41);
  float M42 = det(m42);
  float M43 = det(m43);
  float M44 = det(m44);



  float C11 = pow(-1, 1+1)*M11;
  float C12 = pow(-1, 1+2)*M12;
  float C13 = pow(-1, 1+3)*M13;
  float C14 = pow(-1, 1+4)*M14;

  float C21 = pow(-1, 2+1)*M21;
  float C22 = pow(-1, 2+2)*M22;
  float C23 = pow(-1, 2+3)*M23;
  float C24 = pow(-1, 2+4)*M24;

  float C31 = pow(-1, 3+1)*M31;
  float C32 = pow(-1, 3+2)*M32;
  float C33 = pow(-1, 3+3)*M33;
  float C34 = pow(-1, 3+4)*M34;

  float C41 = pow(-1, 4+1)*M41;
  float C42 = pow(-1, 4+2)*M42;
  float C43 = pow(-1, 4+3)*M43;
  float C44 = pow(-1, 4+4)*M44;
  float A = det(m);
  if (A < 0.000000001f && A>=0.0f) 
    A = 0.000000001f;
  if ( A > -0.000000001f && A<0.0f)
    A = -0.000000001f;
  Matrix inv = { { C11/A, C21/A, C31/A, C41/A,
		 C12/A, C22/A, C32/A, C42/A,
		 C13/A, C23/A, C33/A, C43/A,
		 C14/A, C24/A, C34/A, C44/A } };
  //std::cout << inv << std::endl;
  //return Matrix::Transpose(inv);
  return inv;
}

#if 0
static float TransformFloat_s(const float &f1, const float &f2,
		     float starttime, float endtime, float time)
{
  if (time <= starttime) return f1;
  if (time >= endtime) return f2;
  float delta = time - starttime;
  float scale = endtime-starttime;
  float mult = delta/scale; // 0.0 .. 1.0
  return f1*(1.0-mult) + f2*(mult);
}
#endif
Matrix Matrix::Interpolate(const Matrix &m1, const Matrix &m2, float t) // t= [0.0..1.0]
{
  Vector v(m1.matrix[3], m1.matrix[7], m1.matrix[11]);
  Vector v2(m2.matrix[3], m2.matrix[7], m2.matrix[11]);
 
  Quarternion q = Quarternion::MatrixToQuar(m1);
  Quarternion q2 = Quarternion::MatrixToQuar(m2);
  
  Quarternion qm = Quarternion::NLerp(q,q2, t);
  Vector vm = v*t + v2*(1.0-t);
  Matrix m = Quarternion::QuarToMatrix(qm);
  //Matrix m = Matrix::Identity();
  m.matrix[3]=vm.dx;
  m.matrix[7]=vm.dy;
  m.matrix[11]=vm.dz;
  return m;
}

AxisAngle Matrix::FindAxisAngle(const Matrix &m)
{
  AxisAngle a;
  float trace = m.matrix[0]+m.matrix[5]+m.matrix[10];
  a.angle = acos((trace-1.0)/2.0);
  a.axis = 1.0/2.0/sin(a.angle)*Vector(m.matrix[6]-m.matrix[9],
				       m.matrix[8]-m.matrix[2],
				       m.matrix[1]-m.matrix[4]);
  return a;
}
bool Plane::WhichSideOfPlane(Point p) const
{
  Vector normal = Vector::CrossProduct(u_x, u_y);
  Vector pos = p - u_p;
  float dot = Vector::DotProduct(normal, pos);
  return dot<0.0;
}

float Plane::Dist(Point p) const
{
  Vector normal = Vector::CrossProduct(u_x, u_y);
  Vector vpa = p - u_p;
  float d = -Vector::DotProduct(normal, vpa);
  float d2 = Vector::DotProduct(normal, normal);
  float d3 = d / d2;
  Point pp = (normal*d3).ApplyPoint(p);
  Vector v = p - pp;
  return v.Dist();
}

float Plane::CoordsX(Point p) const
{
  Vector v = p - u_p;
  float vx = Vector::DotProduct(v, u_x/u_x.Dist());
  return vx/u_x.Dist();
}

float Plane::CoordsY(Point p) const
{
  Vector v = p - u_p;
  float vy = Vector::DotProduct(v, u_y/u_y.Dist());
  return vy/u_y.Dist();
}


Color::Color(int r_, int g_, int b_)
  : r(r_), g(g_), b(b_), alpha(0xff)
{
  check();
}

Color::Color(int r_, int g_, int b_, int alpha_)
  : r(r_), g(g_), b(b_), alpha(alpha_)
{
  check();
}


Color::Color(Vector v)
{
  r = int(v.dx*255.0)&0xff;
  g = int(v.dy*255.0)&0xff;
  b = int(v.dz*255.0)&0xff;
  alpha = 255;
  check();
}
Color::Color(unsigned int color)
  : r((color&0xff0000u)>>16),
    g((color&0xff00u)>>8),
    b((color&0xffu)>>0),
        alpha((color&0xff000000u)>>24)
{
  check();
}
unsigned int Color::Average(std::vector<unsigned int> vec)
{
  if (vec.size()==0) return 0x0;
  float r=0.0;
  float g=0.0;
  float b=0.0;
  float a=0.0;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      unsigned int aa = vec[i]&0xff000000;
      unsigned int rr = vec[i]&0x00ff0000;
      unsigned int gg = vec[i]&0x0000ff00;
      unsigned int bb = vec[i]&0x000000ff;
      aa>>=24;
      rr>>=16;
      gg>>=8;
      bb>>=0;
      
      r+=rr;
      g+=gg;
      b+=bb;
      a+=aa;
    }
  r/=s;
  g/=s;
  b/=s;
  a/=s;
  unsigned int rrr = (unsigned int)r;
  unsigned int ggg = (unsigned int)g;
  unsigned int bbb = (unsigned int)b;
  unsigned int aaa = (unsigned int)a;
  aaa<<=24;
  rrr<<=16;
  ggg<<=8;
  bbb<<=0;
  return aaa+rrr+ggg+bbb;
}
void Color::check()
{
    bool bb = (r>255) ||(g>255)||(b>255)||(alpha>255)||(r<0)||(g<0)||(b<0)||(alpha<0);
    if(bb) {
      r = r %511;
      g = g %511;
      b = b %511;
      alpha = alpha %511;
      if (r>255) { r=255-(r-256); }
      if (g>255) { g=255-(g-256); }
      if (b>255) { b=255-(b-256); }
      if (alpha>255) { alpha=255-(alpha-256); }
    if (r<0) { r=-r; }
    if (g<0) { g=-g; }
    if (b<0) { b=-b; }
    if (alpha<0) { alpha=-alpha;  }
    }
}

Matrix Matrix::Transpose(const Matrix &m)
{
  Matrix mm = { { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] } };
  return mm;
  
}

Matrix Quarternion::QuarToMatrix(const Quarternion &q)
{
  float ww = q.w*q.w;
  float xx = q.x*q.x;
  float yy = q.y*q.y;
  float zz = q.z*q.z;
  float i = 1.0/(xx+yy+zz+ww);

  float xy = q.x*q.y;
  float zw = q.z*q.w;
  float xz = q.x*q.z;
  float yw = q.y*q.w;
  float yz = q.y*q.z;
  float xw = q.x*q.w;

  Matrix m = { { (xx-yy-zz+ww)*i, float(2.0*(xy+zw)*i), float(2.0*(xz-yw)*i), 0.0,
		 float(2.0*(xy-zw)*i), (-xx+yy-zz+ww)*i, float(2.0*(yz+xw)*i), 0.0,
		 float(2.0*(xz+yw)*i), float(2.0*(yz-xw)*i), (-xx-yy+zz+ww)*i, 0.0,
		 0.0, 0.0, 0.0, 1.0 } };
  m = Matrix::Transpose(m);
  return m;
}

float cs(float x, float m)
{
  if (x > 0.0)
    {
      if (m > 0.0)
	{
	  return x;
	}
      else
	{
	  return -x;
	}
    }
  else
    {
      if (m > 0.0)
	{
	  return -x;
	}
      else
	{
	  return x;
	}
    }
}
#undef min
#undef max
Quarternion Quarternion::MatrixToQuar(const Matrix &m)
{
  Matrix mm = Matrix::Transpose(m);
  Quarternion q;
  q.w = sqrt(std::max(0.0, 1.0 + mm.matrix[0] + mm.matrix[5] + mm.matrix[10])) / 2.0;
  q.x = sqrt(std::max(0.0, 1.0 + mm.matrix[0] - mm.matrix[5] - mm.matrix[10])) / 2.0;
  q.y = sqrt(std::max(0.0, 1.0 - mm.matrix[0] + mm.matrix[5] - mm.matrix[10])) / 2.0;
  q.z = sqrt(std::max(0.0, 1.0 - mm.matrix[0] - mm.matrix[5] + mm.matrix[10])) / 2.0;
  q.x = cs(q.x, mm.matrix[6] - mm.matrix[9]);
  q.y = cs(q.y, mm.matrix[2] - mm.matrix[8]);
  q.z = cs(q.z, mm.matrix[1] - mm.matrix[4]);
  return q;
}

Quarternion operator+(const Quarternion &a, const Quarternion &b)
{
  Quarternion q;
  q.x = a.x + b.x;
  q.y = a.y + b.y;
  q.z = a.z + b.z;
  q.w = a.w + b.w;
  return q;
}
Quarternion operator*(const Quarternion &a, float x)
{
  Quarternion q;
  q.x = a.x*x;
  q.y = a.y*x;
  q.z = a.z*x;
  q.w = a.w*x;
  return q;
}

Quarternion Quarternion::NLerp(const Quarternion &q,
			       const Quarternion &q2, 
			       float t)
{
  return q*t+q2*(1.0-t);
}
Plane LineProperties::PlaneFromLine(float x, float angle, float dist)
  {
    Point p = MiddlePoint(x);
    Vector u_x = PerpendicularVector(angle, 1.0);
    Vector normal = p2-p1;
    normal /= normal.Dist();
    Vector u_y = Vector::CrossProduct(normal, u_x);
    Plane pl(p, dist*u_x, dist*u_y);
    return pl;
  }
float LineProperties::LineCoords(Point p) const
{
  Vector B = p2-p1;
  Vector A = p-p1;
  Vector unit = B / B.Dist();
  float proj = Vector::DotProduct(A, unit);
  float proj2 = proj / B.Dist();
  return proj2;
}
Point LineProperties::MiddlePoint(float x) const
{
    Point p = Point(x*Vector(p1)+(1.0-x)*Vector(p2));
    return p;
}

Vector LineProperties::PerpendicularVector(float angle, float length)
{
    Vector normal = p2-p1;
    normal /= normal.Dist();
    float y_2 = sin(angle);
    float z_2 = cos(angle);
    if (normal.dx < 0.001 && normal.dx >-0.001) normal.dx += 0.002;
    float x_2 = - ( normal.dy*y_2+normal.dz*z_2 ) / normal.dx;
    Vector u_x(x_2, y_2, z_2);
    return length*u_x;
}
static Vector proj(Vector u, Vector v)
{
  return Vector::DotProduct(u,v)/Vector::DotProduct(u,u)*u;
}

Point Coords::FindInternalCoords(Point external_coords) const
{
  Vector v = external_coords-center;
  float x = Vector::FindProjectionLength(v, u_x);
  float y = Vector::FindProjectionLength(v, u_y);
  float z = Vector::FindProjectionLength(v, u_z);
  return Point(x,y,z);
}
Point Coords::FindExternalCoords(Point internal_coords) const
{
  return center + internal_coords.x*u_x + internal_coords.y*u_y + internal_coords.z*u_z;
}

void Coords::OrthoNormalize()
{
  Vector *v[3] = { &u_x, &u_y, &u_z };
  for(int j=0;j<3;j++)
    {
      for(int i=0;i<j-1;i++)
	{
	  *v[j] = (*v[j]) - proj(*v[i], *v[j]);
	}
      *v[j] = (*v[j]) / (*v[j]).Dist();
    }
}

void UnitVectors::OrthoNormalize()
{
  Vector *v[3] = { &u_x, &u_y, &u_z };
  for(int j=0;j<3;j++)
    {
      for(int i=0;i<j-1;i++)
	{
	   *v[j] = (*v[j]) - proj(*v[i], *v[j]);
	}
      *v[j] = (*v[j]) / (*v[j]).Dist();
    }
}
void UnitVectors::Scale(float x)
{
  u_x *= x;
  u_y *= x;
  u_z *= x;
}
void Coords::Scale(float x)
{
  u_x *= x;
  u_y *= x;
  u_z *= x;
}

Point UnitCube(Point p, Point pos, Vector u_x, Vector u_y, Vector u_z)
{
  Coords c;
  c.center = pos;
  c.u_x = u_x;
  c.u_y = u_y;
  c.u_z = u_z;
  return c.FindInternalCoords(p);
}
Point UnitToCube(Point p, Point pos, Vector u_x, Vector u_y, Vector u_z)
{
  Coords c;
  c.center = pos;
  c.u_x = u_x;
  c.u_y = u_y;
  c.u_z = u_z;
  return c.FindExternalCoords(p);  
}
Point UnitToFlex(Point p, 
		 Point bTL, Point bTR, Point bBL, Point bBR,
		 Point fTL, Point fTR, Point fBL, Point fBR)
{
  Point px1 = Vector(bTL)*p.x+Vector(bTR)*(1.0-p.x);
  Point px2 = Vector(bBL)*p.x+Vector(bBR)*(1.0-p.x);
  Point px3 = Vector(fTL)*p.x+Vector(fTR)*(1.0-p.x);
  Point px4 = Vector(fBL)*p.x+Vector(fBR)*(1.0-p.x);

  Point py1 = Vector(px1)*p.y + Vector(px2)*(1.0-p.y);
  Point py2 = Vector(px3)*p.y + Vector(px4)*(1.0-p.y);

  Point pz = Vector(py1)*p.z + Vector(py2)*(1.0-p.z);
  return pz;
}


PlaneIntersection::PlaneIntersection(const Plane &p1, const Plane &p2)
  : p1(p1), p2(p2)
{
  l = PlanePlaneIntersection(p1,p2);
}
Line PlaneIntersection::Intersection() const
{
  return l;
}
float PlaneIntersection::ProjectP1(Point2d p_in_plane1)
{
  Point p = p1.u_p + p_in_plane1.x*p1.u_x+p_in_plane1.y*p1.u_y;
  float val = Vector::DotProduct(p-l.p, l.dir)/l.dir.Dist();
  return val;
}
float PlaneIntersection::ProjectP2(Point2d p_in_plane2)
{
  Point p = p2.u_p + p_in_plane2.x*p2.u_x+p_in_plane2.y*p2.u_y;
  float val = Vector::DotProduct(p-l.p, l.dir)/l.dir.Dist();
  return val;
}

float LineProperties::Dist(Point p)
{
  Vector v = (p2-p1);
  //v.Normalize();
  v/=v.Dist();
  return (p1+Vector::DotProduct((p-p1),v)*v-p).Dist();
}

float Vector2d::Length() const { return sqrt(dx*dx+dy*dy); }

void Plane::RotateAroundNormal(float angle_in_rad)
{
  Vector normal = Vector::CrossProduct(u_x, u_y);
  Matrix m = Matrix::RotateAroundAxis(normal, angle_in_rad);
  Point p_x = Point(u_x)*m;
  Point p_y = Point(u_y)*m;
  u_x = p_x;
  u_y = p_y;
}
Vector Plane::Normal(float length) const
{
  Vector v = Vector::CrossProduct(u_x,u_y);
  v /= v.Dist();
  v *= length;
  return v;
}

Point Projection(Point2d p1, Point2d p2, Point corner, Vector u_x, Vector u_y, Vector u_z)
{
  // p1 = u_x, u_y
  // p2 = u_z, u_y
  return Point(0.0,0.0,0.0);
}

Point Plane::Navigate(const Point2d &p) { return u_p + p.x*u_x+p.y*u_y; }
float Plane::Dist2(Point p)
{
  Vector n = Normal(1.0);
  p-=Vector(u_p);
  float dot = Vector::DotProduct(n,Vector(p));
  return dot;
  
}
float det2(float *arr)
{
  float a = arr[0];
  float b = arr[1];
  float c = arr[2];
  float d = arr[3];
  return a*d-b*c;
}
float det3(float *arr)
{
  float a = arr[0];
  float b = arr[1];
  float c = arr[2];
  float d = arr[3];
  float e = arr[4];
  float f = arr[5];
  float g = arr[6];
  float h = arr[7];
  float i = arr[8];
  float d2a[] = { e,f,h,i };
  float d2b[] = { d,f,g,i };
  float d2c[] = { d,e,g,h };
  return a*det2(d2a)-b*det2(d2b)+c*det2(d2c);
}
float det4(float *arr)
{
  float a = arr[0];
  float b = arr[1];
  float c = arr[2];
  float d = arr[3];
  float e = arr[4];
  float f = arr[5];
  float g = arr[6];
  float h = arr[7];
  float i = arr[8];
  float j = arr[9];
  float k = arr[10];
  float l = arr[11];
  float m = arr[12];
  float n = arr[13];
  float o = arr[14];
  float p = arr[15];

  float d3a[] = { f,g,h,j,k,l,n,o,p };
  float d3b[] = { e,g,h,i,k,l,m,o,p };
  float d3c[] = { e,f,h,i,j,l,m,n,p };
  float d3d[] = { e,f,g,i,j,k,m,n,o };
  return a*det3(d3a)-b*det3(d3b)+c*det3(d3c)-d*det3(d3d);
}


bool Plane::LineSegmentIntersection(Point p1, Point p2, Point2d &outP)
{
  float arr[] = { 1.0,1.0,1.0,1.0, 
		  u_p.x,(u_p+u_x).x,(u_p+u_y).x, p1.x,
		  u_p.y,(u_p+u_x).y,(u_p+u_y).y, p1.y,
		  u_p.z,(u_p+u_x).z,(u_p+u_y).z, p1.z };
  float arr2[]= { 1.0, 1.0, 1.0, 0.0,
		  u_p.x, (u_p+u_x).x, (u_p+u_y).x, p2.x-p1.x,
		  u_p.y, (u_p+u_x).y, (u_p+u_y).y, p2.y-p1.y,
		  u_p.z, (u_p+u_x).z, (u_p+u_y).z, p2.z-p1.z };
  float t = -det4(arr)/det4(arr2);
  if (t<0.0 ||t>1.0) return false;

  //float d1 = Dist2(p1); // TODO, these dists might not work ok for this algo.
  //float d2 = Dist2(p2);
  //if (d1*d2 > 0) { return false; }
  //float t = d1/(d1-d2);
  Point p = p1+t*Vector(p2-p1);
  float coord_x = CoordsX(p);
  float coord_y = CoordsY(p);
  outP.x = coord_x;
  outP.y = coord_y;
  return true;
}

bool Plane::TriangleIntersection(Point p1, Point p2, Point p3, Point2d &res1, Point2d &res2)
{
  std::vector<Point2d> vec;
  Point2d point;
  if (LineSegmentIntersection(p1,p2,point))
    {
      vec.push_back(point);
    }
  if (LineSegmentIntersection(p1,p3,point))
    {
      vec.push_back(point);
    }
  if (LineSegmentIntersection(p2,p3,point))
    {
      vec.push_back(point);
    }
  if (vec.size()==2)
    {
      res1 = vec[0];
      res2 = vec[1];
    }
  else
    {
      return false; 
    }
  return true;
}

FlexibleCube box(Point p1, float sx, float sy, float sz)
{
  FlexibleCube cube;
  cube.front_plane.line1_p1 = p1;
  cube.front_plane.line1_p2 = p1+Vector(sx,0,0);
  cube.front_plane.line2_p1 = p1+Vector(0,sy,0);
  cube.front_plane.line2_p2 = p1+Vector(sx,sy,0);

  cube.back_plane.line1_p1 = p1+Vector(0,0,sz);
  cube.back_plane.line1_p2 = p1+Vector(sx,0,sz);
  cube.back_plane.line2_p1 = p1+Vector(0,sy,sz);
  cube.back_plane.line2_p2 = p1+Vector(sx,sy,sz);
  return cube;  
}

float det_2(float x1, float x2, float x3, float x4)
{
  return x1*x4 - x2*x3;
}

bool LineLineIntersection_Parallel(Point2d l1_p1, Point2d l1_p2,
				   Point2d l2_p1, Point2d l2_p2)
{
  float x1 = l1_p1.x;
  float y1 = l1_p1.y;
  float x2 = l1_p2.x;
  float y2 = l1_p2.y;
  float x3 = l2_p1.x;
  float y3 = l2_p1.y;
  float x4 = l2_p2.x;
  float y4 = l2_p2.y;
  
  float val = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4); 
  return val>-0.001 && val<0.001;
}
Point2d LineLineIntersection(Point2d l1_p1, Point2d l1_p2,
			     Point2d l2_p1, Point2d l2_p2)
{
  float x1 = l1_p1.x;
  float y1 = l1_p1.y;
  float x2 = l1_p2.x;
  float y2 = l1_p2.y;
  float x3 = l2_p1.x;
  float y3 = l2_p1.y;
  float x4 = l2_p2.x;
  float y4 = l2_p2.y;

  float P_x = ((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
  float P_y = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)) / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

  Point2d p = {P_x,P_y};
  return p;

}
bool IsWithInBoundingBox(Point2d p, Point2d top_left, Point2d bottom_right)
{
  if (p.x>=top_left.x && p.y>=top_left.y &&
      p.x<=bottom_right.x && p.y<=bottom_right.y)
    {
      return true;
    }
  return false;
}
bool LineProperties::QuadIntersection(Point a1, Point a2, Point a3, Point a4, float &t)
{
  float t1,t2;
  bool b = TriangleIntersection(a1,a2,a3,t1);
  bool b2 = TriangleIntersection(a1,a3,a4,t2);
  if (b2) t=t2;
  if (b) t=t1;
  return b||b2;
}
bool LineProperties::BoxIntersection(BOX &b, float &tmin, float &tmax)
{
  Vector v1 = p2-p1;
  tmin = -10000000.0; //std::numeric_limits<float>::infinity(); //10000000.0;
  tmax = 10000000.0; //std::numeric_limits<float>::infinity(); //10000000.0;
  if (v1.dx!=0.0) {
    float tx1 = (b.start_x-p1.x)/v1.dx;
    float tx2 = (b.end_x -p1.x)/v1.dx;
    tmin = std::max(tmin, std::min(tx1,tx2));
    tmax = std::min(tmax, std::max(tx1,tx2));
  }
  if (v1.dy!=0.0) {
    float ty1 = (b.start_y-p1.y)/v1.dy;
    float ty2 = (b.end_y -p1.y)/v1.dy;
    tmin = std::max(tmin, std::min(ty1,ty2));
    tmax = std::min(tmax, std::max(ty1,ty2));
  }
  if (v1.dz!=0.0) {
    float tz1 = (b.start_z-p1.z)/v1.dz;
    float tz2 = (b.end_z -p1.z)/v1.dz;
    tmin = std::max(tmin, std::min(tz1,tz2));
    tmax = std::min(tmax, std::max(tz1,tz2));
  }
  return tmax>=tmin;

}
bool LineProperties::TriangleIntersection(Point v1, Point v2, Point v3, float &u, float &v, float &t)
{
  Vector O = p1;
  Vector D = p2-p1;
  Vector e1 = v2-v1;
  Vector e2 = v3-v1;
  Vector P = Vector::CrossProduct(D, e2);
  float det = Vector::DotProduct(e1, P);
  if (det > -0.00001 && det < 0.00001) return false;
  float inv_det = 1.f / det;
  Vector T = O-v1;
  u = Vector::DotProduct(T,P)*inv_det;
  if (u<0.f || u>1.f) return false;
  Vector Q = Vector::CrossProduct(T,e1);
  v = Vector::DotProduct(D,Q)*inv_det;
  if (v<0.f || u+v>1.f) return false;
  t = Vector::DotProduct(e2,Q)*inv_det;
  //if (t>0.00001) {
    //*ipoint = t;
    return true;
}
bool LineProperties::TriangleIntersection(Point v1, Point v2, Point v3, float &t2)
{
#if 1
  Vector O = p1;
  Vector D = p2-p1;
  Vector e1 = v2-v1;
  Vector e2 = v3-v1;
  Vector P = Vector::CrossProduct(D, e2);
  float det = Vector::DotProduct(e1, P);
  if (det > -0.00001 && det < 0.00001) return false;
  float inv_det = 1.f / det;
  Vector T = O-v1;
  float u = Vector::DotProduct(T,P)*inv_det;
  if (u<0.f || u>1.f) return false;
  Vector Q = Vector::CrossProduct(T,e1);
  float v = Vector::DotProduct(D,Q)*inv_det;
  if (v<0.f || u+v>1.f) return false;
  float t = Vector::DotProduct(e2,Q)*inv_det;
  t2 = t;
  //if (t>0.00001) {
    //*ipoint = t;
    return true;
    //}
    // return false;			  
#endif

#if 0
    Vector u = v2-v1;
    Vector v = v3-v1;
    Vector n = Vector::CrossProduct(u,v);
    if (n.Dist()<0.001) return false;
    Vector dir = p2-p1;
    Vector w0 = p1-v1;
    float  a = -Vector::DotProduct(n,w0);
    float  b = Vector::DotProduct(n,dir);
    if (fabs(b)<0.0001) {
      if (a==0) return false;
      else return false;
    }
    float r = a/b;
    if (r<0.0) return false;
    if (r>1.0) return false;
    //return true;
    Vector i = p1 + r*dir;
    
    float uu = Vector::DotProduct(u,u);
    float uv = Vector::DotProduct(u,v);
    float vv = Vector::DotProduct(v,v);
    Vector w = i - v1;
    float wu = Vector::DotProduct(w,u);
    float wv = Vector::DotProduct(w,v);
    float D = uv * uv - uu*vv;
    float s = (uv*wv - vv*wu) / D;
    if (s<0.0 || s>1.0) return false;
    //return true;
    float t = (uv*wu - uu*wv) / D;
    if (t<0.0 || (s+t)>1.0)
      return false;
    t2 = t;
    return true;
#endif

#if 0
    Vector v0v1 = v2-v1;
    Vector v0v2 = v3-v1;
    Vector N = Vector::CrossProduct(v0v1,v0v2);
    float area2 = N.Dist();
    Vector dir = p2-p1;
    float NdotRayDirection = Vector::DotProduct(N,dir);
    if (fabs(NdotRayDirection) < 0.01) return false;
    
    float d = Vector::DotProduct(N,v1);
    
    t2 = (Vector::DotProduct(N,Vector(p1)) + d)/ NdotRayDirection;
    if (t2<0.0) return false;
    
    Vector P = p1 + t2*dir;
    Vector C;
    Vector edge0 = v2-v1;
    Vector vp0 = P-v1;
    C=Vector::CrossProduct(edge0, vp0);
    if (Vector::DotProduct(N,C) < 0.0) return false;
    
    Vector edge1 = v3-v2;
    Vector vp1 = P-v2;
    C=Vector::CrossProduct(edge1,vp1);
    if (Vector::DotProduct(N,C) <0.0) return false;
    
    Vector edge2 = v1-v3;
    Vector vp2 = P-v3;
    C=Vector::CrossProduct(edge2,vp2);
    if (Vector::DotProduct(N,C)<0.0) return false;
    
    return true;
#endif
}

bool TriangleProperties::is_inside_circum_sphere(Point p) const
{
  Point center = circum_center();
  float radius = circum_radius();
  SphereProperties2 prop(center, radius);
  return prop.InsideSphere(p);
}

std::vector<float> Polynomial::basis_vector(int count, float val)
{
  std::vector<float> vec;
  float mul = 1.0;
  for(int i=0;i<count;i++)
    {
      vec.push_back(mul);
      mul*=val;
    }
  return vec;
}
float Polynomial::mul(std::vector<float> v1,
		      std::vector<float> v2)
{
  int s = std::min(v1.size(), v2.size());
  float val = 0.0;
  for(int i=0;i<s;i++)
    {
      val+=v1[i]*v2[i];
    }
  return val;
}

std::vector<float> Polynomial::mul_matrix(const PolyMatrix &m, std::vector<float> &v)
{
  int s = v.size();
  int sx = std::min(m.sx,s);
  int sy = m.sy;
  std::vector<float> res;
  for(int y=0;y<sy;y++) {
    float val = 0.0;
    for(int x=0;x<sx;x++)
      {
	val+=m.grid[x+y*sx]*v[x];
      }
    res.push_back(val);
  }
  return res;
}
std::vector<float> Polynomial::scale(std::vector<float> v, float c)
{
  int s = v.size();
  for(int i=0;i<s;i++)
    v[i]*=c;
  return v;
}

std::vector<float> Polynomial::add(std::vector<float> v1, std::vector<float> v2)
{
  std::vector<float> res;
  int s = std::min(v1.size(), v2.size());
  for(int i=0;i<s;i++)
    {
      res.push_back(v1[i]+v2[i]);
    }
  int s1 = v1.size();
  int s2 = v2.size();
  if (s1>s2) {
    for(int i=s;i<s1;i++)
      res.push_back(v1[i]);
  } else if (s2>s1) {
    for(int i=s;i<s2;i++)
      res.push_back(v2[i]);
  }
  return res;
}
PolyMatrix PolyMatrix::ZeroMatrix(int sx, int sy)
{
  PolyMatrix m(sx,sy);
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
      {
	m.grid[x+y*sx] = 0.0f;
      }
  return m;
}
PolyMatrix PolyMatrix::IdentityMatrix(int sx, int sy)
{
  PolyMatrix m(sx,sy);
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
      {
	m.grid[x+y*sx] = x==y?1.0:0.0f;
      }
  return m;
}
PolyMatrix PolyMatrix::df_per_dx(int sx, int sy)
{
  PolyMatrix m(sx,sy);
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
      {
	float val = 0.0;
	if (x-1==y) val=float(y+1);
	m.grid[x+y*sx] = val;
      }
  return m;
}

float AreaTools::TriArea(Point p1, Point p2, Point p3)
{
  Vector v1 = p2-p1;
  Vector v2 = p3-p1;
  float dot = Vector::DotProduct(v1,v2)/v1.Dist()/v2.Dist();
  float angle = acos(dot);
  return 1.0/2.0*v1.Dist()*v2.Dist()*sin(angle);
}

float AreaTools::QuadArea(Point p1, Point p2, Point p3, Point p4)
{
  return TriArea(p1,p2,p3) + TriArea(p1,p3,p4);
}

Line PlanePlaneIntersection(const Plane &p1, const Plane &p2)
{
    // Equation: x*u_x - x1*u_x2 + y*u_y - y1*u_y2 = -u_p + u_p2;
  Vector u_x = p1.u_x;
  Vector u_x2 = -p2.u_x;
  Vector u_y = p1.u_y;
  Vector u_y2 = -p2.u_y;
  Point u_p = p1.u_p;
  Point u_p2 = p2.u_p;

  FloatPointBitmap u_xb(u_x);
  FloatPointBitmap u_x2b(u_x2);
  FloatPointBitmap u_yb(u_y);
  FloatPointBitmap u_y2b(u_y2);
  FloatPointBitmap u_pb(u_p);
  FloatPointBitmap u_p2b(u_p2);

  CombineBitmapX<float> u_x_cb(u_xb, u_x2b);
  CombineBitmapX<float> u_y_cb(u_yb, u_y2b);
  CombineBitmapX<float> u_p_cb(u_pb, u_p2b);

  CombineBitmapX<float> u_xy_cb(u_x_cb, u_y_cb);
  CombineBitmapX<float> u_xyp_cb(u_xy_cb, u_p_cb);
  
  NodeToOne n1(u_xyp_cb, 0);
  NodeToOne n2(n1, 1);
  NodeToOne n3(n2, 3);

  NodeToZero a01(n3, 0,1); 
  NodeToZero a02(a01, 0,2); 
  NodeToZero a12(a02, 1,2); 
  NodeToZero a10(a12, 1,0); 
  NodeToZero a20(a10, 2,0); 
  NodeToZero a21(a20, 2,1); 
  /*
  for(int y=0;y<a21.SizeY();y++)
    {
    for(int x=0;x<a21.SizeX();x++)
      {
	std::cout << a21.Map(x,y) << " ";
      }
    std::cout << std::endl;
    }
  */
  Point p(a21.Map(3,0), a21.Map(3,1), a21.Map(3,2));
  Point u1(a21.Map(4,0), a21.Map(4,1), a21.Map(4,2));
  Point u2(a21.Map(5,0), a21.Map(5,1), a21.Map(5,2));
  Point u = u1+Vector(u2);
  
  Line l;
  l.p = u;
  l.dir = Vector(p);
  return l;
}

Pid::Pid(double p, double i, double d){
	init();
	P=p; I=i; D=d;
}
Pid::Pid(double p, double i, double d, double f){
	init();
	P=p; I=i; D=d; F=f;
}
void Pid::init(){
	P=0;
	I=0;
	D=0;
	F=0;

	maxIOutput=0;
	maxError=0;
	errorSum=0;
	maxOutput=0; 
	minOutput=0;
	setpoint=0;
	lastActual=0;
	firstRun=true;
	reversed=false;
	outputRampRate=0;
	lastOutput=0;
	outputFilter=0;
	setpointRange=0;
}

void Pid::setP(double p){
	P=p;
	checkSigns();
}
void Pid::setI(double i){
	if(I!=0){
		errorSum=errorSum*I/i;
		}
	if(maxIOutput!=0){
		maxError=maxIOutput/i;
	}
	I=i;
	checkSigns();

} 

void Pid::setD(double d){
	D=d;
	checkSigns();
}
void Pid::setF(double f){
	F=f;
	checkSigns();
}

void Pid::setPID(double p, double i, double d){
	P=p;I=i;D=d;
	checkSigns();
}

void Pid::setPID(double p, double i, double d,double f){
	P=p;I=i;D=d;F=f;
	checkSigns();
}

void Pid::setMaxIOutput(double maximum){
	maxIOutput=maximum;
	if(I!=0){
		maxError=maxIOutput/I;
	}
}

void Pid::setOutputLimits(double output){ setOutputLimits(-output,output);}

void Pid::setOutputLimits(double minimum,double maximum){
	if(maximum<minimum)return;
	maxOutput=maximum;
	minOutput=minimum;

	if(maxIOutput==0 || maxIOutput>(maximum-minimum) ){
		setMaxIOutput(maximum-minimum);
	}
}

void Pid::setDirection(bool reversed){
	this->reversed=reversed;
}

void Pid::setSetpoint(double setpoint){
	this->setpoint=setpoint;
} 

double Pid::getOutput(double actual, double setpoint){
	double output;
	double Poutput;
	double Ioutput;
	double Doutput;
	double Foutput;

	this->setpoint=setpoint;

	if(setpointRange!=0){
		setpoint=clamp(setpoint,actual-setpointRange,actual+setpointRange);
	}

	double error=setpoint-actual;

	Foutput=F*setpoint;

	Poutput=P*error;	 

	if(firstRun){
		lastActual=actual;
		lastOutput=Poutput+Foutput;
		firstRun=false;
	}


	Doutput= -D*(actual-lastActual);
	lastActual=actual;




	Ioutput=I*errorSum;
	if(maxIOutput!=0){
		Ioutput=clamp(Ioutput,-maxIOutput,maxIOutput); 
	}	

	output=Foutput + Poutput + Ioutput + Doutput;

	if(minOutput!=maxOutput && !bounded(output, minOutput,maxOutput) ){
		errorSum=error; 
	}
	else if(outputRampRate!=0 && !bounded(output, lastOutput-outputRampRate,lastOutput+outputRampRate) ){
		errorSum=error; 
	}
	else if(maxIOutput!=0){
		errorSum=clamp(errorSum+error,-maxError,maxError);
	}
	else{
		errorSum+=error;
	}

	if(outputRampRate!=0){
		output=clamp(output, lastOutput-outputRampRate,lastOutput+outputRampRate);
	}
	if(minOutput!=maxOutput){ 
		output=clamp(output, minOutput,maxOutput);
		}
	if(outputFilter!=0){
		output=lastOutput*outputFilter+output*(1-outputFilter);
	}

	lastOutput=output;
	return output;
}

double Pid::getOutput(){
	return getOutput(lastActual,setpoint);
}

double Pid::getOutput(double actual){
	return getOutput(actual,setpoint);
}
	
void Pid::reset(){
	firstRun=true;
	errorSum=0;
}

void Pid::setOutputRampRate(double rate){
	outputRampRate=rate;
}

void Pid::setSetpointRange(double range){
	setpointRange=range;
}

void Pid::setOutputFilter(double strength){
	if(strength==0 || bounded(strength,0,1)){
		outputFilter=strength;
	}
}

double Pid::clamp(double value, double min, double max){
	if(value > max){ return max;}
	if(value < min){ return min;}
	return value;
}

bool Pid::bounded(double value, double min, double max){
		return (min<value) && (value<max);
}

void Pid::checkSigns(){
	if(reversed){	//all values should be below zero
		if(P>0) P*=-1;
		if(I>0) I*=-1;
		if(D>0) D*=-1;
		if(F>0) F*=-1;
	}
	else{	//all values should be above zero
		if(P<0) P*=-1;
		if(I<0) I*=-1;
		if(D<0) D*=-1;
		if(F<0) F*=-1;
	}
}
