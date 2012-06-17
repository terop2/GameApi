
#include "HierarchyObject.hh"

class HV : public HandleValue<Cube2>
{
public:
  HV(Voxel<bool> &vox, Hierarchy *h, Point p, Vector u_x, Vector u_y, Vector u_z) : h(h), vox(vox), p(p), u_x(u_x), u_y(u_y), u_z(u_z) { vec = new std::vector<FaceCollection*>; }
  void Handle(Cube2 c)
  {
    Point pp = p + float(c.x)*u_x + float(c.y)*u_y + float(c.z)*u_z;
    FaceCollection *coll = h->Obj(pp);
    vec->push_back(coll);
  }
  std::vector<FaceCollection*> *vec;
  Hierarchy *h;
  Voxel<bool> &vox;
  Point p;
  Vector u_x, u_y, u_z;
};

FaceCollection *DoMarching(Hierarchy *h, Voxel<bool> &vox, 
			  Point p, Vector u_x, Vector u_y, Vector u_z)
{
  HV hv(vox, h, p, u_x, u_y, u_z);
  MarchingCubesVoxel v(vox, hv);
  v.Gen();
  std::vector<FaceCollection*> *vec = hv.vec;
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>(vec->begin(), vec->end());
  coll->update_faces_cache();

  FaceCollection *memo = new MemoizeFaces(*coll);
  return memo;
}

FaceCollection *DoMarching2(Hierarchy *h, ContinuousVoxel<bool> &b, int x, int y, int z, Point p, Vector u_x, Vector u_y, Vector u_z)
{
  SampleVoxel<bool> sample(b, x,y,z);
  return DoMarching(h, sample, p, u_x, u_y, u_z);
}
