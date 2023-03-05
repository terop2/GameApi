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



#include <vector>
#include "VectorTools.hh"
#include "Effect.hh"
#include "Graph.hh"

//
// Objects
//
struct OneObject
{
};
struct MatrixObject
{
  Matrix m;
};
struct PointObject
{
  Point p;
};
struct SizeObject
{
  float sx;
  float sy;
  float sz;
};
struct SizeObject2
{
  float sx;
  float sy;
  float sz;
};
struct Point2Object
{
  Point p;
};
struct BoolObject
{
  bool b;
};
struct VectorObject
{
  Vector v;
};
struct ColorObject
{
  unsigned int color;
};
struct IntObject
{
  int num;
};
struct ObjectNumObject
{
  int num;
};
struct IndexObject
{
  int num;
};
struct Index2Object
{
  int num;
};
struct FloatObject
{
  float num;
};
struct RenderObject
{
  Matrix m;
  unsigned int color;
  int object;
};
template<class I, class T>
struct FunctionObject
{
  FunctionObject() : func(0) { }
  Function<I,T> *func;
};
template<class I, class T>
struct ArrayObject
{
  ArrayObject() : array(0) { }
  Array<I,T> *array; // owned
};

struct FaceCollectionObject
{
  FaceCollectionObject() : obj(0) { }
  FaceCollection *obj;
};
struct FaceCollectionListObject
{
  std::vector<FaceCollection*> vec;
};
struct IndexListObject
{
  std::vector<int> vec;
};
template<class T>
struct BitmapObject
{
  BitmapObject() : bitmap(0) { }
  Bitmap<T> *bitmap; // owned
};
template<class T>
struct VoxelObject
{
  VoxelObject() : voxel(0) { }
  Voxel<T> *voxel; // owned
};
template<class T>
struct ContinuousVoxelObject
{
  ContinuousVoxelObject() : voxel(0) { }
  ContinuousVoxel<T> *voxel;
};
struct VBOStateObject
{
  VBOState vbostate;
};
struct CubeObject
{
  Cube2 cube;
};
struct CubeListObject 
{ 
  CubeListObject() : array(0) { }
  Array<int,Cube2> *array; // owned

};
struct VolumePosListObject
{
  VolumePosListObject() : array(0) { }
  Array<int, std::pair<VolumeObject*, Point> > *array;
};
struct VolumeObjectObject
{
  VolumeObjectObject() : volume(0) { }
  VolumeObject *volume;
};
struct RangeProductObject
{
  Range<float> rx;
  Range<float> ry;
  Range<float> rz;
};
struct UnitVectorsObject
{
  Vector u_x;
  Vector u_y;
  Vector u_z;
};

void Print(CubeListObject &cube);


//
// Arrows
//
// (UnitVectors, Cube, Point2) -> Point
struct CubeArrow : public virtual Point2Object, public virtual UnitVectorsObject, public virtual CubeObject, public virtual PointObject
{
  void doit() 
  {
    PointObject::p = Point2Object::p + cube.x*u_x + cube.y*u_y + cube.z*u_z;
  }
};

// (Vec) -> IndexObject && Arrow 
template<class Vec, class Arrow>
struct IterateArrow : public virtual Vec, public virtual IndexObject, public Arrow
{
  void doit()
  {
    int size = Vec::array->Size();
    for(int i=0;i<size;i++)
      {
	IndexObject::num = i;
	Arrow::doit();
      }
  }
};

template<class Vec, class Arrow>
struct Iterate2Arrow : public virtual Vec, public virtual Index2Object, public Arrow
{
  void doit()
  {
    int size = Vec::array->Size();
    for(int i=0;i<size;i++)
      {
	Index2Object::num = i;
	Arrow::doit();
      }
  }
};

// Top -> Matrix
struct IdentityMatrixArrow : public virtual MatrixObject
{
  void doit() { m = Matrix::Identity(); }
};

// (Size, Point) -> Matrix -> Matrix
struct ScaleTranslateArrow : public virtual SizeObject, public virtual PointObject, public virtual MatrixObject
{
  void doit() { 
    //std::cout << p.x << " " << p.y << " " << p.z << std::endl;

    m *= Matrix::Scale(sx,sy,sz)*Matrix::Translate(p.x,p.y,p.z); 
  }
};
// (Matrix, Color, ObjectNum) -> Render
struct MatrixColorArrow : public virtual MatrixObject, 
			  public virtual ColorObject, 
			  public virtual ObjectNumObject,
			  public virtual RenderObject
{
  void doit() 
  {
    RenderObject::m = MatrixObject::m;
    RenderObject::color = ColorObject::color;
    RenderObject::object = ObjectNumObject::num;
  }
};
// (Render,VBOState) -> ()
struct RenderArrow : public virtual RenderObject, public virtual VBOStateObject
{
  void doit();
};
// List -> IndexList
template<class T>
struct IndexesArrow : public virtual T, public virtual IndexListObject
{
  void doit()
  {
    IndexListObject::vec.clear();
    for(int i=0;i<T::vec.size();i++)
      {
	IndexListObject::vec.push_back(i);
      }
  }
};
// List<FaceCollection> -> IndexList
class FaceCollectionIndexes : public IndexesArrow<FaceCollectionListObject>
{
};
// () -> CubeList
class SingleCubeArrow : public virtual CubeListObject
{
public:
  void doit()
  {
    delete CubeListObject::array;
    Cube2 c;
    c.x = 5;
    c.y = 5;
    c.z = 5;
    CubeListObject::array = new SingletonArray<Cube2>(c);
  }
};
// voxel -> CubeList
class MarchingCubesArrow : public virtual VoxelObject<bool>, public virtual CubeListObject
{
public:
  void doit()
  {
    delete CubeListObject::array;
    GenerateArray<Cube2> *ptr = new GenerateArray<Cube2>;
    ptr->Init();
    CubeListObject::array = ptr;
    MarchingCubesVoxel vox(*voxel, *ptr);
    vox.Gen();
  }
};

// (RangeProduct, VolumeObject)->ContinuousVoxel
class VolumeObjectVoxelArrow : public virtual RangeProductObject, public virtual VolumeObjectObject, public virtual ContinuousVoxelObject<bool>
{
public:
  void doit()
  {
    //delete voxel;
    //voxel = new VolumeVoxel(*volume, rx,ry,rz);
  }
};
// (Cube, ContinuousVoxel) -> Voxel
template<class T>
class SampleVoxelArrow : public virtual CubeObject, public virtual ContinuousVoxelObject<T>, public virtual VoxelObject<T>
{
public:
  void doit()
  {
    delete VoxelObject<T>::voxel;
    VoxelObject<T>::voxel = new SampleVoxel<T>(*ContinuousVoxelObject<T>::voxel, cube.x, cube.y, cube.z);
  }
};

// (VolumePosList, Index2)->(VolumeObject, Point2)
class DerefVolumePosArrow : public virtual VolumePosListObject, public virtual Index2Object, public virtual VolumeObjectObject, public virtual Point2Object
{
public:
  void doit() 
  {
    std::pair<VolumeObject*, Point> pp = VolumePosListObject::array->Index(Index2Object::num);
    delete VolumeObjectObject::volume;
    VolumeObjectObject::volume = pp.first;
    Point2Object::p = pp.second;
  }
};

// (CubeList, Index) -> Cube
class DerefCubeArrow : public virtual CubeListObject, public virtual IndexObject, public virtual CubeObject
{
public:
  void doit() { CubeObject::cube = CubeListObject::array->Index(IndexObject::num); }
};
// (Point, Size2) -> Color
class ColorCubeArrow : public virtual PointObject, public virtual SizeObject2, public virtual ColorObject
{
public:
  void doit() { ColorObject::color = Color::ColorCube(p.x, p.y, p.z, sx, sy, sz); }
};
//
// Compositions
//

// 
// (UnitVectors, CubeList, Index, Point2, Size) -> Matrix
// Modifies: Cube, Point, Matrix
class DerefCubeTranslate : public DerefCubeArrow, public CubeArrow, public IdentityMatrixArrow, public ScaleTranslateArrow
{
public:
  void doit()
  {
    DerefCubeArrow::doit(); // CubeList, Index -> Cube
    CubeArrow::doit();  // UnitVectors, Cube, Point2 -> Point
    IdentityMatrixArrow::doit(); // Top -> Matrix
    ScaleTranslateArrow::doit(); // (Size, Point) -> Matrix -> Matrix
  }
};
// (UnitVectors, CubeList, Index, Color, ObjectNum, VBOState, Point2, Size) -> ()
// Modifies: Cube, Point, Matrix, Render

class DerefCubePointArrow : public DerefCubeArrow, public CubeArrow, public IdentityMatrixArrow, public ScaleTranslateArrow, public MatrixColorArrow, public RenderArrow
{
public:
  void doit() 
  { 
    DerefCubeArrow::doit(); 
    CubeArrow::doit(); 
    //std::cout << PointObject::p << std::endl;
    IdentityMatrixArrow::doit();
    ScaleTranslateArrow::doit(); 
    MatrixColorArrow::doit();
    RenderArrow::doit();
  }
};
// (UnitVectors, CubeList, Index, ObjectNum, VBOState, Point2, Size) -> ()
// Modifies: Cube, Point, Matrix, Render, Color

class ColorRender : public DerefCubeTranslate, public ColorCubeArrow, public MatrixColorArrow, public RenderArrow
{
public:
  void doit()
  { 
    DerefCubeTranslate::doit(); // (UnitVectors, CubeList, Index, Point2, Size) -> (Cube, Point, Matrix)
    ColorCubeArrow::doit(); // (Point, Size2) -> Color
    MatrixColorArrow::doit(); // (Matrix, Color, ObjectNum) -> Render
    RenderArrow::doit(); // Render, VBOState -> ()
  }
};
// (Cube, RangeProduct, VolumeObject, UnitVectors, Color, ObjectNum, VBOState, Point2, Size) -> ()
// Modifies: ContinuousVoxel, Voxel, CubeList, Index, Cube, Point, Matrix, Render
class VoxelRenderArrow : public VolumeObjectVoxelArrow, public SampleVoxelArrow<bool>, public MarchingCubesArrow, public IterateArrow<CubeListObject, ColorRender>
{
public:
  void doit()
  {
    VolumeObjectVoxelArrow::doit(); // RangeProduct, VolumeObject->COntinuousVoxel
    SampleVoxelArrow<bool>::doit(); // Cube, COntinuousVoxel -> Voxel
    MarchingCubesArrow::doit(); // Voxel -> CubeList
    IterateArrow<CubeListObject, ColorRender>::doit(); // CubeList -> IndexObject && (UnitVectors, CubeList, Index, ObjectNum, VBOState, Point2, Size) -> ()
  }
};

class DerefVoxelRenderArrow : public DerefVolumePosArrow, public VoxelRenderArrow
{
public:
  void doit() 
  {
    DerefVolumePosArrow::doit(); // (VolumePosList, Index2) -> (VolumeObject, Point2)
    VoxelRenderArrow::doit(); // (Cube, RangeProduct, VolumeObject, UnitVectors, Color, ObjectNum, VBOState, Point2, Size) -> ()
  }
  
};

// (VolumePosList, Cube, RangeProduct, UnitVectors, Color, ObjectNum, VBOState, Size) -> ()
// Modifies: ContinuousVoxel, Voxel, CubeList, Index, Cube, Point, Matrix, Render, VolumeObject, Point2 
class VolumeCityArrow : public Iterate2Arrow<VolumePosListObject, DerefVoxelRenderArrow>
{
public:
  void doit() 
  {
    Iterate2Arrow<VolumePosListObject, DerefVoxelRenderArrow>::doit(); // VolumePosList -> Index2 && (VolumePosList, Index2, Cube, RangeProduct, UnitVectors, Color, ObjectNum, VBOState, Size) -> ()
  }
};

//
// Effects
//
class CategoryEffect : public FrameAnim
{
public:
  CategoryEffect(Render *r) : FrameAnim(r) { }
  void Init();
  bool Frame(float time);
private:
  VolumeCityArrow vox;
};


//
//
// Categories
//
//
// Category of relations
#if 0
template<class I>
class Rel
{
public:
  template<class A, class B>
  struct Arrow
  {
    virtual I Size() const=0;
    virtual std::pair<A,B> Subset(I i) const=0;
    virtual bool Inside(A a, B b) const=0;
  };
  template<class A>
  struct IndentityArrow : public Arrow<A,A>
  {
    IdentityArrow(A a) : a(a) { }
    I Size() const { return I(1); }
    std::pair<A,A> Subset(I i) const { return std::make_pair(a, a); }
    bool Inside(A a1, A a2) const { return a1==a2; }
    A a;
  };
  template<class A, class B, class C>
  struct Composition : public Arrow<A,C>
  {
  public:
    Composition(Arrow<A,B> &arr, Arrow<B,C> &arr2) : arr(arr), arr2(arr2),b(b) { }
    I Size() const { return I(1); }
    std::pair<A,C> Subset(I i) const 
    {
      return std::make_pair(arr.Subset(i).first, arr2.Subset(i).second);
    }
    bool Inside(A a, C c) const
    {
      std::pair<A,B> p = Subset(0);
      B b = p.second;
      return arr.Inside(a,b) && arr2.Inside(b,c);
    }
  private:
    Arrow<A,B> &arr;
    Arrow<B,C> &arr2;
  };
  // Converse
  template<class A, class B>
  struct Converse : public Arrow<B,A>
  {
    Converse(Arrow<A,B> &f) : f(f) { }
    I Size() const { return I(1); }
    std::pair<B,A> Subset(I i) const 
    {
      std::pair<A,B> p = f.Subset(i);
      return std::make_pair(p.second, p.first);
    }
    bool Inside(B b, A a) const
    {
      return f.Inside(a,b);
    }
    Arrow<A,B> &f;
  };
};

template<class A, class B, class I>
class SetInRel : public Rel<I>::Arrow<A, B>
{
public:
  SetInRel(Function<A,B> &func) : func(func) { }
  std::pair<A,B> Subset(I i) const
  {
    A a = A::Index(i);
    return std::make_pair(a, func.Index(a));
  }
  bool Inside(A a, B b) const
  {
    return func.Index(a)==b;
  }
private:
  Function<A,B> &func;
};

template<class A, class B, class I>
class ConcreteRelCategoryArrow : public Function<A,B>
{
public:
  ConcreteRelCategoryArrow(Rel<I>::Arrow<A,B> &arr) : arr(arr) { }
  B Index(A a) const
  {
    for(int i=0;i<arr.Size();i++)
      {
	if (a==arr.Subset(i).first)
	  return arr.Subset(i).second;
      }
    return B();
  }
private:
  Rel<I>::Arrow<A,B> &arr;
};
#endif
