
#include "GameApi_h.hh"

class EmptyPhysics : public PhysicsNode
{
public:
  EmptyPhysics() { }
  virtual int NumAnchors() const { return 0; }
  virtual Point AnchorPoint(int i) const { Point p(0.0, 0.0, 0.0); return p; }
  virtual int NumForces(int i) const { return 0; }
  virtual Vector Force(int i, int f) const { Vector v(0.0, 0.0, 0.0); return v; }
  virtual int NumLinks() const { return 0; }
  virtual std::pair<int,int> Link(int i) const { return std::make_pair(0,0); }
  virtual float LinkDistance(int i) const { return 0.0; }
  virtual int NumForceVolumes() const { return 0; }
  virtual Vector ForceVolume(int v, Point p) const { Vector v2(0.0,0.0,0.0); return v2; }

};

GameApi::PH GameApi::PhysicsApi::empty()
{
  return add_physics(e, new EmptyPhysics);
}
class AnchorPhysics : public ForwardPhysics
{
public:
  AnchorPhysics(PhysicsNode *node, Point p) : ForwardPhysics(*node), node(node),p(p) { }
  int NumAnchors() const { return 1 + node->NumAnchors(); }
  Point AnchorPoint(int i) const { 
    int s = NumAnchors();
    if (i<s-1) { return node->AnchorPoint(i); }
    return p;
  }
private:
  PhysicsNode *node;
  Point p;
};
GameApi::PhysicsApi::PHI GameApi::PhysicsApi::anchor_point(PH phy, PT pos)
{
  PhysicsNode *node = find_physics(e, phy);
  Point *pt = find_point(e, pos);
  PHI i;
  PhysicsNode *n = new AnchorPhysics(node, *pt);
  i.phy = add_physics(e, n); 
  i.id = n->NumAnchors()-1;
  return i;
}

class ForcePhysics : public ForwardPhysics
{
public:
  ForcePhysics(PhysicsNode *node, int point, Vector dir) : ForwardPhysics(*node), node(node), point(point), dir(dir) { }
  int NumForces(int i) const
  {
    if (i==point) { return 1 + node->NumForces(i); }
    return node->NumForces(i);
  }
  Vector Force(int i, int f) const
  {
    int s = NumForces(i);
    int ss = s;
    if (i==point) ss= s-1;
    if (f < ss) { return node->Force(i,f); }
    return dir;
  }
private:
  PhysicsNode *node;
  int point;
  Vector dir;
};
GameApi::PH GameApi::PhysicsApi::ext_force(PH phy, int point, V dir)
{
  PhysicsNode *node = find_physics(e, phy);
  Vector *v = find_vector(e, dir);
  return add_physics(e, new ForcePhysics(node, point, *v));
}

class ForcePhysics2 : public ForwardPhysics
{
public:
  ForcePhysics2(PhysicsNode *node, Vector dir) : ForwardPhysics(*node), node(node), dir(dir) { }
  int NumForces(int i) const
  {
    return 1 + node->NumForces(i);
  }
  Vector Force(int i, int f) const
  {
    int s = NumForces(i);
    if (f < s-1) { return node->Force(i,f); }
    return dir;
  }
private:
  PhysicsNode *node;
  Vector dir;
};


GameApi::PH GameApi::PhysicsApi::ext_force_all(PH phy, V dir)
{
  PhysicsNode *node = find_physics(e, phy);
  Vector *v = find_vector(e, dir);
  return add_physics(e, new ForcePhysics2(node, *v));
}

class AnchorLinkPhysics : public ForwardPhysics
{
public:
  AnchorLinkPhysics(PhysicsNode *node, int p1, int p2, float dist) : ForwardPhysics(*node), node(node), p1(p1), p2(p2), dist(dist) { }
  int NumLinks() const {
    return 1 + node->NumLinks();
  }
  std::pair<int,int> Link(int i) const
  {
    int s = NumLinks();
    if (i<s-1) { return node->Link(i); }
    return std::make_pair(p1,p2);
  }
  float LinkDistance(int i) const
  {
    int s = NumLinks();
    if (i<s-1) { return node->LinkDistance(i); }
    return dist;
  }
private:
  PhysicsNode *node;
  int p1,p2;
  float dist;
};

GameApi::PH GameApi::PhysicsApi::anchor_link(PH phy, int p1, int p2, float dist)
{
  PhysicsNode *node = find_physics(e, phy);
  return add_physics(e, new AnchorLinkPhysics(node, p1, p2, dist));
}

class ForceObjPhysics : public ForwardPhysics
{
public:
  ForceObjPhysics(PhysicsNode *node, VolumeObject *obj2, Vector v) : ForwardPhysics(*node), node(node), obj(obj2), v(v) { }
  int NumForceVolumes() const { return 1+node->NumForceVolumes(); }
  Vector ForceVolume(int v1, Point p) const
  {
    int s = NumForceVolumes();
    if (v1 < s-1) { return node->ForceVolume(v1,p); }
    bool b = obj->Inside(p);
    if (b) { return v; }
    Vector v2(0.0,0.0,0.0);
    return v2;
  }
private:
  PhysicsNode *node;
  VolumeObject *obj;
  Vector v;
};

GameApi::PH GameApi::PhysicsApi::force_obj(PH phy, O obj, V dir)
{
  PhysicsNode *node = find_physics(e, phy);
  VolumeObject *obj2 = find_volume(e, obj);
  Vector *vec = find_vector(e, dir);
  return add_physics(e, new ForceObjPhysics(node, obj2, *vec));
}

//GameApi::PH GameApi::PhysicsApi::array(PH *arr, int size)
//{
//}

int GameApi::PhysicsApi::num_anchors(PH phy)
{
  PhysicsNode *node = find_physics(e, phy);
  return node->NumAnchors();

}
int GameApi::PhysicsApi::num_forces(PH phy, int anchor)
{
  PhysicsNode *node = find_physics(e, phy);
  return node->NumForces(anchor);
}
int GameApi::PhysicsApi::num_links(PH phy)
{
  PhysicsNode *node = find_physics(e, phy);
  return node->NumLinks();
}
int GameApi::PhysicsApi::num_force_volumes(PH phy)
{
  PhysicsNode *node = find_physics(e, phy);
  return node->NumForceVolumes();
}

class PointsPhysics : public PointsApiPoints
{
public:
  PointsPhysics(PhysicsNode *node) : node(node) { }
  int NumPoints() const { return node->NumAnchors(); }
  Point Pos(int i) const { return node->AnchorPoint(i); }
  unsigned int Color(int i) const { return 0xffffffff; }
private:
  PhysicsNode *node;
};

GameApi::PTS GameApi::PhysicsApi::init_points(PH phy)
{
  PhysicsNode *node = find_physics(e, phy);
  PointsApiPoints *pts = new PointsPhysics(node);
  return add_points_api_points(e, pts);
}

void GameApi::PhysicsApi::step_points(PH phy, PTA prev_frame, float timestep)
{
  PhysicsNode *node = find_physics(e, phy);
  PointArray3 *arr = find_point_array3(e, prev_frame);
  
  std::vector<Vector> vec;
  for(int i=0;i<arr->numpoints;i++)
    {
      Vector sum(0.0, 0.0, 0.0);
      int fn = node->NumForces(i);
      for(int f=0;f<fn;f++)
	{
	  sum += node->Force(i,f);
	}
      Point p(arr->array[i*3+0], arr->array[i*3+1], arr->array[i*3+2]);
      //std::cout << "Physics begin: " << p << std::endl;
      int fnn = node->NumForceVolumes();
      for(int ff=0;ff<fnn;ff++)
	{
	  //std::cout << "ForceVolume " << std::endl;
	  sum += node->ForceVolume(ff, p);
	}
      vec.push_back(sum);
    }

  int s = node->NumLinks();
  for(int i=0;i<s;i++)
    {
      std::pair<int,int> link = node->Link(i);
      Point p1(arr->array[link.first*3+0], arr->array[link.first*3+1], arr->array[link.first*3+2]);
      Point p2(arr->array[link.second*3+0], arr->array[link.second*3+1], arr->array[link.second*3+2]);
      Vector f1 = vec[link.first];
      Vector f2 = vec[link.second];
      std::pair<Vector,Vector> pp1 = Vector::SplitToComponents(f1, p2-p1);
      std::pair<Vector,Vector> pp2 = Vector::SplitToComponents(f2, p1-p2);
      vec[link.first] += pp2.first;
      vec[link.second] += pp1.first;
    }

  for(int i=0;i<arr->numpoints;i++)
    {
      Point p1(arr->array[i*3+0], arr->array[i*3+1], arr->array[i*3+2]);
      p1 += vec[i]*timestep;
      //std::cout << "Physics point: " << p1 << std::endl;
      arr->array[i*3+0] = p1.x;
      arr->array[i*3+1] = p1.y;
      arr->array[i*3+2] = p1.z;
    }
}