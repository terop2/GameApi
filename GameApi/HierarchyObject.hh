
#include "Graph.hh"

class Hierarchy
{
public:
  virtual FaceCollection* Obj(Point p) const=0;
};

FaceCollection *DoMarching(Hierarchy *h, Voxel<bool> &vox, 
			   Point p, Vector u_x, Vector u_y, Vector u_z);

FaceCollection *DoMarching2(Hierarchy *h, ContinuousVoxel<bool> &b, 
			    int x, int y, int z, 
			    Point p, Vector u_x, Vector u_y, Vector u_z);
