
#include "GameApi_h.hh"

EXPORT GameApi::MatrixApi::MatrixApi(Env &e) : e(e) { }

EXPORT GameApi::M GameApi::MatrixApi::identity()
{
  return add_matrix2(e, Matrix::Identity());
}
EXPORT GameApi::M GameApi::MatrixApi::transpose(M mat)
{
  Matrix m = find_matrix(e, mat);
  return add_matrix2(e, Matrix::Transpose(m));
}
EXPORT GameApi::M GameApi::MatrixApi::xrot(float rot)
{
  return add_matrix2(e, Matrix::XRotation(rot));
}
EXPORT GameApi::M GameApi::MatrixApi::yrot(float rot)
{
  return add_matrix2(e, Matrix::YRotation(rot));
}
EXPORT GameApi::M GameApi::MatrixApi::zrot(float rot)
{
  return add_matrix2(e, Matrix::ZRotation(rot));
}
EXPORT GameApi::M GameApi::MatrixApi::trans(float x, float y, float z)
{
  return add_matrix2(e, Matrix::Translate(x,y,z));
}
EXPORT GameApi::M GameApi::MatrixApi::scale(float sx, float sy, float sz)
{
  return add_matrix2(e, Matrix::Scale(sx,sy,sz));
}
EXPORT GameApi::M GameApi::MatrixApi::inverse(M mat)
{
  Matrix m = find_matrix(e, mat);
  return add_matrix2(e, Matrix::Inverse(m));
}
EXPORT GameApi::M GameApi::MatrixApi::keep_rotation(M mat)
{
  Matrix m = find_matrix(e, mat);
  Matrix m2 = Matrix::KeepRotation(m);
  return add_matrix2(e, m2);
}
EXPORT GameApi::M GameApi::MatrixApi::mult(M m1, M m2)
{
  Matrix ma = find_matrix(e, m1);
  Matrix mb = find_matrix(e, m2);
  return add_matrix2(e, ma * mb);
}
EXPORT GameApi::M GameApi::MatrixApi::perspective(float fovy, float aspect, float near0, float far0)
{
  Matrix m = Matrix::Perspective(fovy, aspect, near0, far0);
  return add_matrix(e, new SimpleMatrix(m));
}
EXPORT GameApi::PT GameApi::MatrixApi::mult(PT point, M matrix)
{
  Matrix ma = find_matrix(e, matrix);
  Point *pt = find_point(e, point);
  
  Point p2 = (*pt) * ma;
  return add_point(e, p2.x,p2.y,p2.z);
}
