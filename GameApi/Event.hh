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


#ifndef EVENT_HH
#define EVENT_HH

#include "Effect.hh"

template<class T>
class Time
{
public:
  float time;
  T o;
};
typedef Time<Point> TimedPoint;
typedef Time<Vector> TimedVector;
typedef Time<Matrix> TimedMatrix;

class TimeLine : public Curve<Point>
{
};

class TimedPointCollection : public Array<int, TimedPoint>
{
};

template<class T>
class Paths
{
public:
  virtual ~Paths() { }
  virtual float Length() const=0;
  virtual int Size() const=0;
  virtual T Index(float time, int p) const=0;
};

class TimedPointCloud : public Paths<Point>
{
public:
};

class TimedMatrixPaths : public Paths<Matrix>
{
public:
};

class TranslatePaths : public TimedMatrixPaths
{
public:
  TranslatePaths(TimedPointCloud &cloud) : cloud(cloud) { }
  float Length() const { return cloud.Length(); }
  int Size() const { return cloud.Size(); }
  Matrix Index(float time, int p) const
  {
    Point pp = cloud.Index(time, p);
    //std::cout << p << ":" << pp << std::endl;
    return Matrix::Translate(pp.x,pp.y,pp.z);
  }
private:
  TimedPointCloud &cloud;
};

class TimedWaveforms : public Paths<float>
{
};

class TimeLineSequence : public TimeLine
{
public:
  TimeLineSequence(Array<int, TimeLine*> &arr) : arr(arr) { }
  float Size()
  {
    int size = arr.Size();
    float length=0.0;
    for(int i=0;i<size;i++)
      {
	length += arr.Index(i)->Size();
      }
    return length;
  }
  Point Index(float time) const
  {
    int size = arr.Size();
    float length = 0.0;
    for(int i=0;i<size;i++)
      {
	int oldlength = length;
	length += arr.Index(i)->Size();
	if (length > time)
	  {
	    return arr.Index(i)->Index(time-oldlength);
	  }
      }
    return Point(0.0,0.0,0.0);    

  }
private:
  Array<int, TimeLine*> &arr;
};
class PointCloudSequence : public TimedPointCloud
{
public:
  PointCloudSequence(Array<int, TimedPointCloud*> &arr) : arr(arr) { }
  float Length() const
  {
    int size = arr.Size();
    float length = 0.0;
    for(int i=0;i<size;i++)
      {
	length += arr.Index(i)->Length();
      }
    return length;
  }
  int Size() const
  {
    int size = arr.Size();
    int num = 999999;
    for(int i=0;i<size;i++)
      {
	num = std::min(num, arr.Index(i)->Size());
      }
    return num;    
  }
  Point Index(float time, int p) const
  {
    int size = arr.Size();
    float length = 0.0;
    Point pp(0.0,0.0,0.0);
    for(int i=0;i<size;i++)
      {
	int oldlength = length;
	length += arr.Index(i)->Length();
	if (length > time)
	  {
	    return pp+arr.Index(i)->Index(time-oldlength, p);
	  }
	pp += arr.Index(i)->Index(arr.Index(i)->Length(), p);
      }
    return pp;
  }
private:
  Array<int, TimedPointCloud*> &arr;
};

class TimeLineFromPointCloud : public TimeLine
{
public:
  TimeLineFromPointCloud(TimedPointCloud &cloud, int p) : cloud(cloud), p(p) { }
  float Size() const { return cloud.Length(); }
  Point Index(float time) const
  {
    return cloud.Index(time, p);
  }
private:
  TimedPointCloud &cloud;
  int p;
};
class BoxCollectionFromPaths : public BoxCollection
{
public:
  BoxCollectionFromPaths(TimedMatrixPaths &paths, float time) : paths(paths), time(time) { }
  int Size() const 
  {
    return paths.Size();
  }
  Matrix Index(int i) const
  {
    return paths.Index(time, i);
  }
private:
  TimedMatrixPaths &paths;
  float time;
};

class InterpolatePoints : public TimedPointCloud
{
public:
  InterpolatePoints(PointCollection &c1, PointCollection &c2, float length) : c1(c1), c2(c2), length(length) { }
  int NumPoints() const { return std::min(c1.Size(), c2.Size()); }
  float Length() const { return length; }
  Point Index(float time, int p) const
  {
    Point p1 = c1.Index(p);
    Point p2 = c2.Index(p);
    float val = time / length;
    return val*Vector(p1)+(1.0-val)*Vector(p2);
  }
private:
  PointCollection &c1;
  PointCollection &c2;
  float length;
};

class InterpolateMatrixes : public TimedMatrixPaths
{
public:
  InterpolateMatrixes(BoxCollection &c1, BoxCollection &c2, float length) : c1(c1), c2(c2), length(length) { }
  float Length() const { return length; }
  int Size() const { return std::min(c1.Size(), c2.Size()); }
  Matrix Index(float time, int p) const
  {
    Matrix m1 = c1.Index(p);
    Matrix m2 = c2.Index(p);
    //return m2;
    return Matrix::Interpolate(m1,m2,time/length);
  }
private:
  BoxCollection &c1, &c2;
  float length;
};


class TimedMatrixSequence : public TimedMatrixPaths
{
public:
  TimedMatrixSequence(Array<int, TimedMatrixPaths*> &arr) : arr(arr) { }
  float Length() const
  {
    int size = arr.Size();
    float length = 0.0;
    for(int i=0;i<size;i++)
      {
	length += arr.Index(i)->Length();
      }
    return length;
  }
  int Size() const
  {
    int size = arr.Size();
    int num = 999999;
    for(int i=0;i<size;i++)
      {
	num = std::min(num, arr.Index(i)->Size());
      }
    return num;    
  }
  Matrix Index(float time, int p) const
  {
    int size = arr.Size();
    float length = 0.0;
    Matrix m = Matrix::Identity();
    for(int i=0;i<size;i++)
      {
	int oldlength = length;
	length += arr.Index(i)->Length();
	if (length > time)
	  {
	    return m*arr.Index(i)->Index(time-oldlength, p);
	  }
	m = m * arr.Index(i)->Index(arr.Index(i)->Length(),p);
      }
    return m;
  }
private:
  Array<int, TimedMatrixPaths*> &arr;
};

class ParallelCurves : public TimedMatrixPaths
{
public:
  ParallelCurves(Array<int, MatrixCurve*> &curves) : curves(curves) { }
  float Length() const 
  {
    int size = curves.Size();
    float num = 999999.0;
    for(int i=0;i<size;i++)
      {
	num = std::min(num, curves.Index(i)->Size());
      }
    return num;    
  }
  int Size() const 
  {
    return curves.Size();
  }
  Matrix Index(float time, int p) const
  {
    return curves.Index(p)->Index(time);
  }
private:
  Array<int, MatrixCurve*> &curves;
};

class ConnectMatrixPaths : public TimedMatrixPaths
{
public:
  ConnectMatrixPaths(TimedMatrixPaths &c1, TimedMatrixPaths &c2, float length) : c1(c1), c2(c2), b1(c1, c1.Length()), b2(c2, 0.0), inter(b1,b2, length), seq(arr) 
  {
    arr.push_back(&c1);
    arr.push_back(&inter);
    arr.push_back(&c2);
  }
  float Length() const { return seq.Length(); }
  int Size() const { return seq.Size(); }
  Matrix Index(float time, int p) const { return seq.Index(time,p); }

private:
  TimedMatrixPaths &c1, &c2;
  BoxCollectionFromPaths b1,b2;
  InterpolateMatrixes inter;
  VectorArray<TimedMatrixPaths*> arr;
  TimedMatrixSequence seq;
};

class SplineFunctions : public Array<int, const Function<float, float>* >, private Function<float,float>
{
public:
  SplineFunctions(const PointCollection2d &points, float z_0) : points(points), z_0(z_0) { } 
  int Size() const { return points.Size()-1; }
  const Function<float,float>* Index(int i) const
  {
    func = i;
    return this;
  }
  float Index(float x) const
  {
    Point2d p = points.Index(func);
    float y_i = p.y;
    float x_i = p.x;
    float z_i = z_i_f(z_0, func);
    Point2d p2 = points.Index(func+1);
    //float y_i_1 = p2.y;
    float x_i_1 = p2.x;
    float z_i_1 = z_i_1_f(z_i, func);
    float S_i = y_i + z_i*(x-x_i) + (z_i_1 - z_i)/2.0/(x_i_1-x_i) * (x-x_i)*(x-x_i);
    return S_i;
  }
  float z_i_1_f(float z_i, int i) const
  {
    Point2d p = points.Index(i);
    Point2d p2 = points.Index(i+1);
    return -z_i + 2.0*(p2.y-p.y)/(p2.x-p.x);
  }
  float z_i_f(float z_0, int ii) const
  {
    float z= z_0;
    for(int i=1;i<ii;i++)
      {
	z = z_i_1_f(z, i);
      }
    return z;
  }

private:
  const PointCollection2d &points;
  mutable int func;
  float z_0;
};

class SplineInterpolation : public Function<float, float>
{
public:
  SplineInterpolation(PointCollection2d &coll) : coll(coll), func(coll, 0.5) { }
  float Index(float x) const
  {
    int size=coll.Size()-1;
    for(int i=0;i<size;i++)
      {
	if (x>=coll.Index(i).x && x<coll.Index(i+1).x)
	  {
	    return func.Index(i)->Index(x);
	  }
      }
    return 0.0;
  }
private:
  PointCollection2d &coll;
  SplineFunctions func;
};

class ChooseXYZ : public PointCollection2d
{
public:
  ChooseXYZ(PointCollection &coll, FloatArray &arr, int xyz) : coll(coll), arr(arr), xyz(xyz) { }
  int Size() const { return std::min(coll.Size(), arr.Size()); }
  Point2d Index(int i) const 
  {
    Point2d p;
    p.x = arr.Index(i);
    if (xyz == 0)
      p.y = coll.Index(i).x;
    else if (xyz==1)
      p.y = coll.Index(i).y;
    else if (xyz==2)
      p.y = coll.Index(i).z;
    else
      p.y = 0.0;
    return p;
  }
private:
  PointCollection &coll;
  FloatArray &arr;
  int xyz;
};

class SplineCurve : public CurveIn3d
{ // arr should be increasing float numbers.
  // and there needs to be same amount of points as floats.
public:
  SplineCurve(PointCollection &coll, FloatArray &arr) : coll(coll), arr(arr), x(coll, arr, 0), y(coll,arr,1), z(coll,arr,2), sx(x), sy(y), sz(z) { }
  float Size() const { return arr.Index(arr.Size()-1) - arr.Index(0); }
  Point Index(float t) const
  {
    t += arr.Index(0);
    Point p;
    p.x = sx.Index(t);
    p.y = sy.Index(t);
    p.z = sz.Index(t);
    return p;
  }
private:
  PointCollection &coll;
  FloatArray &arr;
  ChooseXYZ x;
  ChooseXYZ y;
  ChooseXYZ z;
  SplineInterpolation sx;
  SplineInterpolation sy;
  SplineInterpolation sz;
};

class CircleMatrixPath : public TimedMatrixPaths
{
public:
  CircleMatrixPath(float x1, float x2, int count, float time) : circle(x1), circle2(x2), sample(circle, count), sample2(circle2, count), im(sample, sample2, time) { }
  float Length() const { return im.Length(); }
  int Size() const { return im.Size(); }
  Matrix Index(float time, int p) const 
  {
    Matrix m = im.Index(time,p);  
    //std::cout << m << ":" << time << ", " << p << std::endl;
    return m;
  }

private:
  CircleMatrixCurve circle;
  CircleMatrixCurve circle2;
  SampleMatrixCurve sample;
  SampleMatrixCurve sample2;
  InterpolateMatrixes im;
};

class SplineCurves : public Array<int, CurveIn3d*>
{
public:
  SplineCurves(Array<int, PointCollection*> &arr, FloatArray &arr2) : c(0), arr(arr), arr2(arr2){ }
  int Size() const { return arr.Size(); }
  CurveIn3d *Index(int i) const
  {
    delete c;
    PointCollection *coll = arr.Index(i);
    c = new SplineCurve(*coll, arr2);
    return c;
  }

private:
  mutable SplineCurve *c;
  Array<int, PointCollection*> &arr;
  FloatArray &arr2;
};
class InterpolationDistance
{
public:
  virtual float Dist(int p1, int p2) const=0;
};
class Dist1 : public InterpolationDistance
{
public:
  Dist1(PointCollection &coll) : coll(coll) { }
  float Dist(int p1, int p2) const
  {
    Point r = coll.Index(p1);
    Point r2 = coll.Index(p2);
    return (r2-r).Dist();
  }
private:
  PointCollection &coll;
};
class Dist2 : public InterpolationDistance
{
public:
  Dist2(FloatArray &arr) : arr(arr) { }
  float Dist(int p1, int p2) const
  {
    return arr.Index(p2)-arr.Index(p1);
  }
private:
  FloatArray &arr;
};

class LinearInterpolation : public CurveIn3d
{
public:
  LinearInterpolation(PointCollection &coll) : coll(coll), d(coll), dist(d) { }
  LinearInterpolation(PointCollection &coll, InterpolationDistance &dist) : coll(coll), d(coll), dist(dist) { }
  Point Index(float pos) const
  {
    float length = 0;
    int i;
    float d = 0.0;
    int size = coll.Size()-1;
    for(i=0;i<size;i++)
      {
	d = dist.Dist(i,i+1);
	length += d;
	if (length>pos) break;
      }
    length -= d;
    pos -= length;
    pos /= d;
    Point px = Point((1.0-pos)*Vector(coll.Index(i)) + pos*Vector(coll.Index(i+1)));
    return px;
  }
  float Size() const
  {
    float length = 0;
    for(int i=0;i<coll.Size()-1;i++)
      {
	length += dist.Dist(i,i+1);
      }
    return length;
  }

  //float Distance(int p, int p2) const
  //{
  //  Point r = coll.Index(p);
  //  Point r2 = coll.Index(p2);
  //  return (r2-r).Dist();
  //}

private:
  PointCollection &coll;
  Dist1 d;
  InterpolationDistance &dist;
};

class ParallelCurvesPoint : public TimedPointCloud
{
public:
  ParallelCurvesPoint(Array<int, CurveIn3d*> &arr) : arr(arr) { }
  float Length() const { return arr.Index(0)->Size(); }
  int Size() const { return arr.Size(); }
  Point Index(float time, int p) const
  {
    return arr.Index(p)->Index(time);
  }
private:
  Array<int, CurveIn3d*> &arr;
};

class SelectPoints : public PointCollection
{
public:
  SelectPoints(Array<int, PointCollection*> &arr, int select) : arr(arr), select(select) { }
  int Size() const { return arr.Size(); }
  Point Index(int index) const
  {
    return arr.Index(index)->Index(select);
  }  
private:
  Array<int, PointCollection*> &arr;
  int select;
};

class LinearMovement : public TimedPointCloud
{ // All PointCollections to have same number of elements.
  // arr and FloatArray should have same number of elements.
public:
  LinearMovement(Array<int, PointCollection*> &arr, FloatArray &time) : arr(arr), time(time), lm(*this), points(0), dist(time), interpol(0), parallel(lm) { }
public:
  float Length() const { return parallel.Length(); }
  int Size() const { return parallel.Size(); }
  Point Index(float time, int p) const
  {
    return parallel.Index(time,p);
  }
  
private:
  struct LM : public Array<int, CurveIn3d*>
  {
    LM(LinearMovement &mv) : mv(mv) { }
    int Size() const { 
      int size = mv.arr.Index(0)->Size(); 
      //std::cout << "size2: " << size << std::endl;
      return size;
    }
    CurveIn3d *Index(int i) const 
    {
      //if (i>=Size()) i=Size();
      delete mv.interpol;
      delete mv.points;
      mv.points = new SelectPoints(mv.arr, i);
      mv.interpol = new LinearInterpolation(*mv.points, mv.dist);
      return mv.interpol;
    }
    LinearMovement &mv;
  };
  friend class LM;
private:
  Array<int, PointCollection*> &arr;
  FloatArray &time;
  LM lm;
  SelectPoints *points;          // (Array<int,PointCollection*>, int) => PointCollection 
  Dist2 dist;                     // FloatArray => InterpolationDistance
  LinearInterpolation *interpol; // PointCollection, InterpolationDistance => CurveIn3d
  ParallelCurvesPoint parallel; // array<int, curvein3d*> => timedpointcloud
};

class SizeCollections : public Array<int, PointCollection*>
{ // makes all pointcollections the same size by adding new 0.0,0.0,0.0 points.
public:
  SizeCollections(Array<int, PointCollection*> &arr) : arr(arr) 
  { 
    pc.size = MaxSize();
  }
  int Size() const { return arr.Size(); }
  PointCollection *Index(int i) const
  {
    if (i>=Size()) i--;
    //delete pc;
    //pc = new PC(arr.Index(i), MaxSize());
    pc.coll = arr.Index(i);
    return &pc;
  }
  int MaxSize() const
  {
    int size = arr.Size();
    int maxsize = 0;
    for(int i=0;i<size;i++)
      {
	maxsize = std::max(maxsize, arr.Index(i)->Size());
      }
    return maxsize;
  }
  struct PC : public PointCollection
  {
    int Size() const { return size; }
    Point Index(int i) const 
    {
      if (i < coll->Size())
	return coll->Index(i);
      else
	return coll->Index(0);
    }
    PointCollection *coll;
    int size;
  };
private:
  Array<int, PointCollection*> &arr;
  mutable PC pc;
};
void DrawVBO(TimedMatrixPaths &paths, float time, VBOState &vbostate, VBOUpdate u);
void DrawVBO(Array<int, PointCollection*> &arr, FloatArray &t, float time, VBOState &vbostate, VBOUpdate u, PieceShader &shader);






#endif
