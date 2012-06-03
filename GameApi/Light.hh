

class LightingEnvironment
{
public:
  virtual Color Env(Vector v) const=0;
};

class LightBitmap : public Bitmap<Color>
{
public:
  LightBitmap(LightingEnvironment &env,
	      int sx, int sy,
	      Point p, Vector u_x, Vector u_y,
	      Point center) : sx(sx), sy(sy), env(env), p(p), u_x(u_x), u_y(u_y), center(center) { }
  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Color Map(int x, int y) const
  {
    Point pp = p + (float(x)/sx)*u_x + (float(y)/sy)*u_y;
    pp -= center;
    return env.Env(Vector(pp));
  }
private:
  int sx,sy;
  LightingEnvironment &env;
  Point p;
  Vector u_x, u_y;
  Point center;
};

class LightDistribution
{
public:
  virtual Color Dist(Vector v, Vector vv) const=0;
};

class RadialLightDistribution : public LightDistribution
{
public:
  RadialLightDistribution(Color c1, Color c2, float dist) : c1(c1), c2(c2), dist(dist) { }
  Color Dist(Vector v, Vector vv) const
  {
    float d = Vector::Dist(v,vv);
    d/=dist;
    if (d>=0.0 && d<=1.0)
      return Color::Interpolate(c1,c2,d); 
    if (d<0.0)
      return c1;
    return c2;
  }
};

class ArrayLightingEnvironment : public LightingEnvironment
{
public:
  void push_back(Vector v, LightDistribution *next)
  {
    Node n;
    n.vec = v/v.Dist();
    n.color = c;
    arr.push_back(n);
  }
  Color Env(Vector v) const
  {
    v/=v.Dist();
    int s = arr.Size();
    float min_dist = 100000.0;
    Color result(255,255,255);
    for(int i=0;i<s;i++)
      {
	Node n = arr.Index(i);
	Vector vv = n.vec;
	vv/=vv.Dist();
	float val = Vector::Dist(vv,v);
	if (val < min_dist)
	  {
	    min_dist = val;
	    result = n.color->Dist(v,vv);
	  }
      }
    return result;
  }
  struct Node
  {
    Vector vec;
    LightDistribution *color;
  };
private:
    VectorArray<Node> arr;
};
