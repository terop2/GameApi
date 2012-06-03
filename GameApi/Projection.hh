
class LineRange
{
  float start;
  float end;
};

class PlaneRange
{
  LineRange x;
  LineRange y;
};

class CubeRange
{
  LineRange x;
  LineRange y;
  LineRange z;
};

float LineRangePoint(const LineRange &r, float m)
{
  return r.start + m*(r.end-r.start);
}
Point2d PlaneRangePoint(const PlaneRange &r, float mx, float my)
{
  float x = LineRangePoint(r.x, mx);
  float y = LineRangePoint(r.y, my);
  Point2d p;
  p.x = x;
  p.y = y;
  return p;
}
Point CubeRangePoint(const CubeRange &r, float mx, float my, float mz)
{
  float x = LineRangePoint(r.x, mx);
  float y = LineRangePoint(r.y, my);
  float z = LineRangePoint(r.z, mz);
  Point p(x,y,z);
  return p;
}

std::pair<Point2d, Point2d> Projection(Point p)
{ // equation (x,y1=y2, z)
  Point2d p1;
  p1.x = p.x;
  p1.y = p.y;
  Point2d p2;
  p2.x = p.z;
  p2.y = p.y;
  return std::make_pair(p1,p2);
}

Point IntersectCurve(Function<float, float> &f1, Function<float,float> &f2, float y) // f1 :: y->x, f2 :: y->z
{
  Point p1;
  p1.x = f1.Index(y);
  p1.y = y;
  p1.z = f2.Index(y);
  return p1; // (x,z)
}



class Curve2dFunction : public Function<float,float>
{
  Curve2dFunction(Curve<Point2d> &c, float y, float num) : c(c), y(y), num(num) { }
  // TODO, NEEDS solving equations for floats
  float Index(float x) const
  {
    ChooseX2d xx(c);
    ChooseY2d yy(c);
    float pos = FindRoot(xx, num);
    
  }
private:
  Curve<Point2d> &c;
  float y;
  float num;
};
