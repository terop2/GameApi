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


#ifndef TREE_HH
#define TREE_HH

#include "Effect.hh"
#include "Event.hh"
#include <assert.h>
#include "VolumeObjects.hh"
#include <typeinfo>
#include <vector>

template<class T>
class Tree
{
public:
  virtual int NumberOfLevels() const=0;
  virtual const Array<int, T> *TreeChilds(const Array<int, int> &path) const=0;
};

class TimeChange
{
public:
  virtual float Time(float time) const = 0;
};
class EmptyTimeChange : public TimeChange
{
public:
  virtual float Time(float time) const { return time; }
};

struct TreeNode
{
  mutable Matrix matrix;
  const BoxableFaceCollection *faces;
  bool quads; // false = triangles, true = quads
  mutable ShaderParameters *params;
  TimeChange *timechange;
  friend std::ostream &operator<<(std::ostream &o, const TreeNode &tn);
};
TreeNode matrix_treenode(Matrix m);
TreeNode faces_treenode(Matrix m, const BoxableFaceCollection *faces);

struct TreeNodeInternal
{
  mutable Matrix matrix;
  const BoxableFaceCollection *faces;
  bool quads;
  ShaderParameters *params;
  Matrix current_level_matrix;
  friend std::ostream &operator<<(std::ostream &o, const TreeNodeInternal &tn);
  friend bool operator==(const TreeNodeInternal &n1, const TreeNodeInternal &n2);
};
class ModifyTree
{
public:
  virtual void Modify(float time)=0;
};

class TreeNodeCollection : public Array<int, TreeNode>, public ModifyTree
{
public:
};


class EventNodes : public TreeNodeCollection
{
public:
};
class MatrixFacesNodes : public TreeNodeCollection
{
public:
  MatrixFacesNodes(BoxCollection &coll, BoxableFaceCollection &faces) : coll(coll), faces(faces) { }
  int Size() const { return coll.Size(); }
  TreeNode Index(int i) const
  {
    TreeNode n = faces_treenode(coll.Index(i), &faces);
    return n;
  }
  void Modify(float time)
  {
  }
private:
  BoxCollection &coll;
  BoxableFaceCollection &faces;
};

class MatrixNodes : public TreeNodeCollection
{
public:
  MatrixNodes(BoxCollection &coll) : coll(coll) { }
  int Size() const { return coll.Size(); }
  TreeNode Index(int i) const
  {
    TreeNode n = matrix_treenode(coll.Index(i));
    return n;
  }
  void Modify(float time)
  {
  }
private:
  BoxCollection &coll;
};
class TimedMatrixNodes : public TreeNodeCollection
{
public:
  TimedMatrixNodes(TimedMatrixPaths &paths) : paths(paths) { }
  int Size() const { return paths.Size(); }
  TreeNode Index(int i) const
  {
    TreeNode n = matrix_treenode(paths.Index(t, i));
    return n;
  }
  void Modify(float time)
  {
    t = time;
  }

private:
  mutable float t;
  TimedMatrixPaths &paths;
};

class EmptyTreeNodeCollection : public TreeNodeCollection
{
public:
  int Size() const { return 0; }
  TreeNode Index(int a) const { TreeNode n; return n; }
  void Modify(float time) { }
};

class MatrixTreeNodeCollection : public TreeNodeCollection
{
public:
  MatrixTreeNodeCollection(TreeNodeCollection &coll) : coll(coll) { }
  int Size() const { return coll.Size(); }
  TreeNode Index(int i) const 
  { 
    TreeNode n = matrix_treenode(coll.Index(i).matrix); 
    return n; 
  }
  void Modify(float time) 
  {
    coll.Modify(time);
  }
private:
  TreeNodeCollection &coll;
};

class FacesTreeNodeCollection : public TreeNodeCollection
{
public:
  FacesTreeNodeCollection(TreeNodeCollection &coll) : coll(coll) { }
  int Size() const { return coll.Size(); }
  TreeNode Index(int i) const 
  { 
    TreeNode n = faces_treenode(Matrix::Identity(), coll.Index(i).faces); 
    return n; 
  }
  void Modify(float time) 
  {
    coll.Modify(time);
  }
private:
  TreeNodeCollection &coll;
};

class SingletonTreeNodeCollection : public TreeNodeCollection
{
public: // not very useful class, only for dynamic treenode generation
  SingletonTreeNodeCollection(TreeNode n) : n(n) { }
  void Modify(float time) { }
  int Size() const { return 1; }
  TreeNode Index(int a) const 
  { 
    return n;
  }
  
private:
  TreeNode n;
};

class RingTreeNodes : public TreeNodeCollection
{
public:
  RingTreeNodes()
    : pvarray(array, array+4),
      pointvector(pvarray),
      ring(pointvector, 100.0, 50),
      ring1(pointvector, 150.0,50),
      ring2(pointvector, 200.0,50),
      ring3(pointvector, 250.0,50)
  {
  }
  int Size() const { return 4; }
  TreeNode Index(int i) const
  {
    TreeNode n;
    switch(i)
      {
      case 0: n = faces_treenode(m,&ring); break;
      case 1: n = faces_treenode(m1,&ring1); break;
      case 2: n = faces_treenode(m2,&ring2); break;
      case 3: n = faces_treenode(m3,&ring3); break;
      default: std::cout << "RingTreeNode: ERROR! " << i << std::endl; break;
      }
    return n;
  }
  void Modify(float time)
  {
    m = Matrix::XRotation(time/100.0);
    m1 = Matrix::ZRotation(time/75.0);
    m2 = Matrix::XRotation(time/50.0);
    m3 = Matrix::ZRotation(time/25.0);
  }
private:
  Matrix m;
  Matrix m1;
  Matrix m2;
  Matrix m3;
  VectorArray<std::pair<Point,Vector> > pvarray;
  PointVectorCollectionConvert pointvector;
  RingElem ring;
  RingElem ring1;
  RingElem ring2;
  RingElem ring3;
  static std::pair<Point,Vector> array[];
};

class ObjectTree
{
public:
  virtual int NumberOfLevels() const=0;
  virtual const TreeNodeCollection *TreeChilds(const Array<int, int> &path) const=0;
};

class ArrayObjectTreeStatic : public ObjectTree
{
public:
  ArrayObjectTreeStatic(TreeNodeCollection &coll) : coll(coll) { }
  int NumberOfLevels() const { return 1; }
  const TreeNodeCollection *TreeChilds(const Array<int, int> &path) const
  {
    if (path.Size()==0)
      {
	return &coll;
      }
    return &empty;
  }
private:
  TreeNodeCollection &coll;
  EmptyTreeNodeCollection empty;
};

class ArrayObjectTreeDynamic : public ObjectTree
{
public:
  ArrayObjectTreeDynamic(TreeNodeCollection &coll) : coll(coll), matrix(coll), faces(coll), singleton(0) { }
  int NumberOfLevels() const { return 2; }
  const TreeNodeCollection *TreeChilds(const Array<int, int> &path) const
  {
    int size = path.Size();
    if (size==0)
      {
	return &matrix;
      }
    if (size==1)
      {
	delete singleton;
	int index = path.Index(0);
	TreeNode n = faces.Index(index);
	singleton = new SingletonTreeNodeCollection(n);
	return singleton;
      }

    return &empty;
  }
  ~ArrayObjectTreeDynamic() { delete singleton; }
private:
  TreeNodeCollection &coll;
  EmptyTreeNodeCollection empty;
  MatrixTreeNodeCollection matrix;
  FacesTreeNodeCollection faces;
  mutable SingletonTreeNodeCollection *singleton;
};


class ObjectTreeForest : public Array<int, ObjectTree*>
{
public:
};
typedef ArrayConvert<ObjectTreeForest, int, ObjectTree*> ObjectTreeForestConvert;


class ForestToTree : public ObjectTree
{
public:
  // Forest and nodes array sizes must match
  ForestToTree(ObjectTreeForest &forest, TreeNodeCollection &nodes) : forest(forest), nodes(nodes) { }
  virtual int NumberOfLevels() const 
  {
    int size = forest.Size();
    int count =0;
    for(int i=0;i<size;i++)
      {
	count = std::max(count, forest.Index(i)->NumberOfLevels());
      }
    return count;
  }
  virtual const TreeNodeCollection *TreeChilds(const Array<int, int> &path) const
  {
    int size = path.Size();
    if (size==0)
      {
	return &nodes;
      }
    int index = path.Index(0);
    ObjectTree *tree = forest.Index(index);
    SubArray<int,int> path2(path, 1, path.Size()-1);
    return tree->TreeChilds(path2);
  }
private:
  ObjectTreeForest &forest;
  TreeNodeCollection &nodes;
};

class AppendArray : public Array<int,int>
{
public:
  AppendArray(const Array<int,int> &arr, int i) : arr(arr), i(i) { }
  int Size() const { return arr.Size()+1; }
  int Index(int ii) const
  {
    if (ii==0) return i;
    return arr.Index(ii-1);
  }
private:
  const Array<int,int> &arr;
  int i;
};

class ObjectSubTree : public ObjectTree
{
public:
  ObjectSubTree(ObjectTree &tree, int index) : tree(tree), index(index) { }
  int NumberOfLevels() const { return tree.NumberOfLevels()-1; }
  const TreeNodeCollection *TreeChilds(const Array<int, int> &path) const
  {
    AppendArray arr(path, index);
    return tree.TreeChilds(arr);
  }
private:
  ObjectTree &tree;
  int index;
};

class TreeSize
{
public:
  virtual int SubTreeSize(VectorArray<int> &path) const=0;
};

class TreeSizeImpl : public TreeSize
{
public:
  TreeSizeImpl(ObjectTree &tree): tree(tree) { }
  int SubTreeSize(VectorArray<int> &path) const
  {
    const TreeNodeCollection *coll = tree.TreeChilds(path);
    int count = 0;
    int num = coll->Size();
    count += num;
    
    for(int i=0;i<num;i++)
      {
	path.push_back(i);
	count += SubTreeSize(path);
	path.pop_back();
      }
    return count;
  }
private:
  ObjectTree &tree;
};

class ObjectRanges
{
public:
  virtual int NumObjects() const=0;
  virtual int StartRange(int obj) const=0;
  virtual int EndRange(int obj) const=0;
  virtual Matrix Transform(int obj) const =0;
};
template<class T>
class RangeFaceCollectionArray : public Array<int, T>
{
public:
  RangeFaceCollectionArray(ObjectRanges &ranges, Array<int,T> &arr, int obj) : ranges(ranges), arr(arr), obj(obj) { }
  int Size() const { return ranges.EndRange(obj)-ranges.StartRange(obj); }
  T Index(int i) const
  {
    int startindex = ranges.StartRange(obj);
    return arr.Index(startindex+i);
  }
private:
  ObjectRanges &ranges;
  Array<int, T> &arr;
  int obj;
};
class SelectFaceCollection : public Array<int,std::pair<const FaceCollection*, bool> >
{
public:
  SelectFaceCollection(Array<int, TreeNodeInternal> &arr) : arr(arr) { }
  int Size() const { return arr.Size(); }
  std::pair<const FaceCollection *, bool> Index(int i) const
  {
    TreeNodeInternal n = arr.Index(i);
    return std::make_pair(n.faces, n.quads);
  }
private:
  Array<int, TreeNodeInternal> &arr;
};

class ApplyMatrix : public Array<int, TreeNodeInternal>
{
public:
  ApplyMatrix(Array<int,TreeNodeInternal> &arr) : arr(arr), elem(0) { }
  int Size() const { return arr.Size(); }
  TreeNodeInternal Index(int i) const
  {
    
    TreeNodeInternal n = arr.Index(i);

    if (!elem)
      {
	elem = new MatrixElem(*n.faces, n.current_level_matrix);
      }
    //delete elem;
    elem->SetMatrix(n.current_level_matrix);
    elem->SetNext(n.faces);

    //elem = new MatrixElem(*n.faces, n.current_level_matrix);
    n.faces = elem;
    return n;
  }
  ~ApplyMatrix() { delete elem; }
private:
  Array<int,TreeNodeInternal> &arr;
  mutable MatrixElem *elem;
};

class QuadTriChooser : public Array<int, FaceCollectionTri*>
{
public:
  QuadTriChooser(Array<int, std::pair<const FaceCollection*,bool> > &arr) : arr(arr), coll(0) { }
  int Size() const { return arr.Size(); }
  FaceCollectionTri *Index(int i) const
  {
    delete coll;
    std::pair<const FaceCollection*, bool> p = arr.Index(i);
    if (p.second)
      { // quads
	coll = new QuadsToTris(*p.first);
      }
    else
      {
	coll = new TrisToTris(*p.first); 
      }
    return coll;
  }
  ~QuadTriChooser() { delete coll; }
private:
  Array<int, std::pair<const FaceCollection*, bool> > &arr;
  mutable FaceCollectionTri *coll;
};

class CompressFaceCollectionArrays : public Array<int, FaceCollectionTri*>
{
public:
  CompressFaceCollectionArrays(ObjectRanges &ranges, Array<int, TreeNodeInternal> &arr) : ranges(ranges), arr(arr), range_arr(0), apply_matrix(0), select(0), chooser(0), iter(0), oa(0)
 { }
  int Size() const
  {
    return ranges.NumObjects();
  }
  FaceCollectionTri *Index(int i) const
  {
    delete oa;
    //delete iter;
    delete chooser;
    delete select;
    delete apply_matrix;
    delete range_arr;

    range_arr = new RangeFaceCollectionArray<TreeNodeInternal>(ranges, arr, i);
    apply_matrix = new ApplyMatrix(*range_arr);
    select = new SelectFaceCollection(*apply_matrix);
    chooser = new QuadTriChooser(*select);
    //comp = new CompressObjectTri(*chooser);
    //iter = new IteratorArray<int, FaceCollectionTri*>(*chooser);
    oa = new ObjectArray2(*chooser);
    return oa;
  }
  ~CompressFaceCollectionArrays() 
  { 
    delete oa;
    //delete iter;
    delete chooser;
    delete select;
    delete apply_matrix;
    delete range_arr;
  }
private:
  ObjectRanges &ranges;
  Array<int, TreeNodeInternal> &arr;
  mutable RangeFaceCollectionArray<TreeNodeInternal> *range_arr;
  mutable ApplyMatrix *apply_matrix;
  mutable SelectFaceCollection *select;
  mutable QuadTriChooser *chooser;
  //CompressObjectTri *comp;
  mutable IteratorArray<int, FaceCollectionTri*> *iter;
  mutable ObjectArray2 *oa;
};
class VBOArrays : public Element<const FaceCollectionVBO*>
{
public:
  VBOArrays(Array<int, FaceCollectionTri*> &arr) : oa(arr), vbo(oa) { }
  const FaceCollectionVBO *Elem() const
  {
    return &vbo;
  }
private:
  //IteratorArray<int, FaceCollectionTri*> iter;
  ObjectArray2 oa;
  VBO vbo;
};

class MemoizeRanges : public ObjectRanges
{
public:
  MemoizeRanges(ObjectRanges &ranges_) : count(ranges_.NumObjects()), ranges(ranges_), start_range(count), start_range_flag(count), end_range(count), end_range_flag(count), transform(count), transform_flag(count) 
  {
    for(int i=0;i<count;i++)
      { 
	start_range_flag[i] = 0;
	end_range_flag[i] = 0;
	transform_flag[i] = 0;
     }
  }
  virtual int NumObjects() const
  {
    return count;
  }
  virtual int StartRange(int obj) const
  {
    if (start_range_flag[obj]==0)
      {
	start_range_flag[obj]=1;
	start_range[obj]=ranges.StartRange(obj);
      }
    return start_range[obj];
  }
  virtual int EndRange(int obj) const
  {
    if (end_range_flag[obj]==0)
      {
	end_range_flag[obj]=1;
	end_range[obj]=ranges.EndRange(obj);
      }
    return end_range[obj];

  }
  virtual Matrix Transform(int obj) const
  {
    if (transform_flag[obj]==0)
      {
	transform_flag[obj]=1;
	transform[obj]=ranges.Transform(obj);
      }
    return transform[obj];

  }

private:
  int count;
  ObjectRanges &ranges;
  mutable std::vector<int> start_range;
  mutable std::vector<char> start_range_flag;
  mutable std::vector<int> end_range;
  mutable std::vector<char> end_range_flag;
  mutable std::vector<Matrix> transform;
  mutable std::vector<char> transform_flag;

};

class SharedArray : public Array<int, TreeNodeInternal>, public ObjectRanges
{
public:
  SharedArray(Array<int, TreeNodeInternal> &arr, ObjectRanges &ranges) : arr(arr), ranges(ranges), enabled_func(this, &SharedArray::Enabled), mapping_range_i(this, &SharedArray::MappingRangeI) { }

public:
  int Size() const
  {
    int c = arr.Size();
    int count = 0;
    for(int i=0;i<c;i++)
      {
	bool b = InsideEnabledRange(i);
	if (b) count++;
      }
    return count;
  }
  TreeNodeInternal Index(int i) const
  {
    return arr.Index(MappingArrayI(i));
  }

  int NumObjects() const
  {
    int c = ranges.NumObjects();
    int count = 0;
    for(int i=0;i<c;i++)
      {
	bool b = Enabled(i)==-1;
	if (b) count++;
      }
    return count;
  }

  int StartRange(int obj) const
  {
    int next = Enabled(obj);
    if (next == -1)
      {
	return MappingArray(ranges.StartRange(MappingRange(obj)));
      }
    else
      {
	return MappingArray(ranges.StartRange(MappingRange(next)));
      }
  }
  int EndRange(int obj) const
  {
    int next = Enabled(obj);
    if (next == -1)
      {
	return MappingArray(ranges.EndRange(MappingRange(obj)));
      }
    else
      {
	return MappingArray(ranges.EndRange(MappingRange(next)));
      }
  }
  Matrix Transform(int obj) const
  {
    return Matrix::Identity();
  }
public:
  // bigger -> smaller
  int MappingArray(int obj) const
  {
    int count = 0;
    int num = arr.Size();
    for(int i=0;i<num;i++)
      {
	if (i==obj) return count;
	bool b = InsideEnabledRange(i);
	if (b)
	  {
	    count++;
	  }
      }
    return 0;
  }

  // smaller -> bigger
  int MappingArrayI(int obj) const
  {
    int count = 0;
    int num = arr.Size();
    for(int i=0;i<num;i++)
      {
	bool b = InsideEnabledRange(i);
	if (b)
	  {
	    if (count==obj) return i;
	    count++;
	  }
      }
    return 0;
  }

  
  bool InsideEnabledRange(int num) const
  {
    int size = ranges.NumObjects();
    for(int i=0;i<size;i++)
      {
	if (InsideRange(num,i))
	  {
	    if (Enabled(i)==-1)
	      {
		return true;
	      }
	    else
	      {
		return false;
	      }
	  }
      }
    return false;
  }

  bool InsideRange(int num, int obj) const
  {
    int start = ranges.StartRange(obj);
    int end = ranges.EndRange(obj);
    return num >=start && num < end;
  }

  // smaller -> bigger
  int MappingRange(int obj) const
  {
    int count = 0;
    int num = ranges.NumObjects();
    for(int i=0;i<num;i++)
      {
	bool b = Enabled(i)==-1;
	if (b)
	  {
	    if (count==obj) return i;
	    count++;
	  }
      }
    return 0;
  }

  // bigger -> smaller
  int MappingRangeI(int obj) const
  {
    int count = 0;
    int num = ranges.NumObjects();
    for(int i=0;i<num;i++)
      {
	if (i==obj) return count;
	bool b = Enabled(i)==-1;
	if (b)
	  {
	    count++;
	  }
      }
    return 0;
  }





  // returns -1 if enabled
  // returns 0..obj-1 if disabled
  int Enabled(int obj) const
  {
    for(int i=0;i<obj;i++)
      {
	if (Equal(obj,i))
	  return i;
      }
    return -1;
  }

  bool Equal(int obj1, int obj2) const
  {
    //if (obj1==obj2) return false;
    int start1 = ranges.StartRange(obj1);
    int end1 = ranges.EndRange(obj1);
    int start2 = ranges.StartRange(obj2);
    int end2 = ranges.EndRange(obj2);
    if (end1-start1 != end2-start2) return false;
    for(int i=0;i<end1-start1;i++)
      {
	TreeNodeInternal n1 = arr.Index(start1+i);
	TreeNodeInternal n2 = arr.Index(start2+i);
	if (n1 == n2) continue;
	return false;
      }
    return true;
  }
private:
  Array<int, TreeNodeInternal> &arr;
  ObjectRanges &ranges;
public:
  ForwardFunction<SharedArray, int, int> enabled_func;
  ForwardFunction<SharedArray, int, int> mapping_range_i;
};

class FlattenTree : public Array<int, TreeNodeInternal>, public ObjectRanges
{
public:
  FlattenTree(ObjectTree &tree) : tree(tree), size(sizeimpl), sizeimpl(tree) { }
  int Size() const 
  {
    VectorArray<int> vec;
    return size.SubTreeSize(vec);
  }
  int NumObjects() const
  {
    int count = 0;
    VectorArray<int> vec;
    const TreeNodeCollection *coll = tree.TreeChilds(vec);
    if (coll->Size() == 0) return 0;
    count += 1;
    int num = coll->Size();
    for(int i=0;i<num;i++)
      {
	ObjectSubTree subtree(tree, i);
	FlattenTree flat(subtree);
	count += flat.NumObjects();
      }
    return count;
  }
  Matrix Transform(int obj) const
  {
    if (obj==0) return Matrix::Identity();
    obj-=1;
    VectorArray<int> vec;
    const TreeNodeCollection *coll = tree.TreeChilds(vec);
    int num = coll->Size();
    int count = 0;
    for(int i=0;i<num;i++)
      {
	int oldcount = count;
	ObjectSubTree subtree(tree, i);
	FlattenTree flat(subtree);
	count += flat.NumObjects();
	if (count > obj)
	  {
	    return  flat.Transform(obj-oldcount) * coll->Index(i).matrix; 
	  }
      }
    std::cout << "Flatten::Transform ERROR! " << std::endl;
    return Matrix::Identity();
  }
  int StartRange(int obj) const
  {
    if (obj==0) return 0;
    obj -= 1;

    VectorArray<int> vec;
    const TreeNodeCollection *coll = tree.TreeChilds(vec);
    int num = coll->Size();
    int count = 0;
    int count2 = 0;
    for(int i=0;i<num;i++)
      {
	int oldcount = count;
	int oldcount2 = count2;
	ObjectSubTree subtree(tree, i);
	FlattenTree flat(subtree);
	count += flat.NumObjects();

	vec.push_back(i);
	count2 += size.SubTreeSize(vec);
	vec.pop_back();

	if (count > obj)
	  {
	    int val = num + oldcount2 + flat.StartRange(obj-oldcount);
	    return val;
	  }
      }
    std::cout << "Flatten::Startrange ERROR! " << std::endl;
    return 0;
  }
  int EndRange(int obj) const
  {
    VectorArray<int> vec;
    const TreeNodeCollection *coll = tree.TreeChilds(vec);
    if (obj==0) return coll->Size();
    obj -= 1;

    int num = coll->Size();
    int count = 0;
    int count2 = 0;
    for(int i=0;i<num;i++)
      {
	int oldcount = count;
	int oldcount2 = count2;
	ObjectSubTree subtree(tree, i);
	FlattenTree flat(subtree);
	count += flat.NumObjects();

	vec.push_back(i);
	count2 += size.SubTreeSize(vec);
	vec.pop_back();

	if (count > obj)
	  {
	    int val = num + oldcount2 + flat.EndRange(obj-oldcount);
	    return val;
	  }
      }
    std::cout << "Flatten::Endrange ERROR! " << std::endl;
    return 0;
  }
  TreeNodeInternal Index(int i) const
  {
    VectorArray<int> vec;
    const TreeNodeCollection *coll = tree.TreeChilds(vec);
    if (i < coll->Size())
      {
	TreeNode n = coll->Index(i);
	TreeNodeInternal nn;
	nn.matrix = n.matrix;
	nn.faces = n.faces;
	nn.quads = n.quads;
	nn.params = n.params;
	nn.current_level_matrix = n.matrix;
	return nn;
      }
    int num = coll->Size();
    i -= num;
    int count = 0;
    for(int ii=0;ii<num;ii++)
      {
	int oldcount = count;
	vec.push_back(ii);
	count += size.SubTreeSize(vec);
	vec.pop_back();
	if (count > i)
	  { // this one
	    ObjectSubTree subtree(tree, ii);
	    FlattenTree subtreeflat(subtree);
	    TreeNodeInternal n = subtreeflat.Index(i-oldcount);
	    n.matrix = coll->Index(ii).matrix * n.matrix;
	    return n;
	  }
      }
    std::cout << "ERROR: FlattenTree: i=" << i+num << std::endl;
    TreeNodeInternal n = { };
    return n;
  }
private:
  ObjectTree &tree;
  TreeSize &size;
  TreeSizeImpl sizeimpl;
};

class TreeObjectData : public ObjectDataArray
{
public:
  TreeObjectData(ObjectRanges &ranges, Function<int,int> &enabled, Function<int,int> &mapping_range_i) : ranges(ranges), enabled(enabled), mapping_range_i(mapping_range_i)  { }
  int Size() const { return ranges.NumObjects(); }
  ObjectData Index(int i) const
  {
    ObjectData d;
    d.matrix = ranges.Transform(i);
    int en = enabled.Index(i);
    if (en == -1) en = i;
    d.object = mapping_range_i.Index(en);
    d.params = 0; // TODO, ShaderParameters (flattentree needs to provide via ObjectRanges)
    return d;
  }
private:
  ObjectRanges &ranges;
  Function<int,int> &enabled;
  Function<int,int> &mapping_range_i;
};
class TreeStateInternal;
class TreeState
{
public:
  TreeState();
  ~TreeState();
  friend void PrepareTree(ObjectTree &tree, TreeState &treestate, VBOUpdate u);
  friend void RenderTree(ObjectTree &tree, float time, TreeState &treestate, VBOUpdate u);
private:
  TreeStateInternal *priv;
};

void PrepareTree(ObjectTree &tree, TreeState &treestate, VBOUpdate u=UpdateVertexNormalIndex);
void RenderTree(ObjectTree &tree, float time, TreeState &treestate, VBOUpdate u=UpdateVertexNormalIndex);

class RenderTreeCmds : public OpenGlCommands
{
public:
  RenderTreeCmds(ObjectTree &tree, float time) : tree(tree), time(time) { }
  void PreCalc();
  void Execute();
private:
  ObjectTree &tree;
  float time;
  TreeState treestate;
};

class PositionTree : public ObjectTree
{
public:
  PositionTree(ObjectTreeForest &forest, TimedMatrixPaths &positions) : nodes(positions), tree(forest, nodes) { }
  int NumberOfLevels() const { return tree.NumberOfLevels(); }
  const TreeNodeCollection *TreeChilds(const Array<int,int> &path) const
  {
    return tree.TreeChilds(path);
  }  
private:
  TimedMatrixNodes nodes;
  ForestToTree tree;
};

class CircleTree : public ObjectTree
{
public:
  CircleTree(ObjectTree &object_tree) : circle(400.0), sample(circle, 5), treenode(sample), forest_array(&object_tree, treenode.Size()), forest(forest_array), tree(forest, treenode) { }
  int NumberOfLevels() const { return tree.NumberOfLevels(); }
  const TreeNodeCollection *TreeChilds(const Array<int,int> &path) const
  {
    return tree.TreeChilds(path);
  }
private:
  CircleMatrixCurve circle;
  SampleMatrixCurve sample;
  MatrixNodes treenode;  
  DuplicateArray<int, ObjectTree*> forest_array;
  ObjectTreeForestConvert forest;
  ForestToTree tree;
};

class RingTreeEffect : public FrameAnim
{
public:
  RingTreeEffect(Render *r);
  void Init();
  bool Frame(float time);
private:
  RingTreeNodes nodes;
  ArrayObjectTreeDynamic tree;
  //CircleTree circle;
  TreeState vbostate;

  SingletonArray<ObjectTree*> forestarray;
  ObjectTreeForestConvert forest;
  CircleMatrixPath cpath;
  PositionTree pos;
};
class ManyCubesEffect : public FrameAnim
{
public:
  ManyCubesEffect(Render *r);
  void Init();
  bool Frame(float time);
private:
  CubeElem cube;
  MatrixElem me;
  RandomPointsInBox box;
  PointsToBoxCollection matrix;
  MatrixFacesNodes nodes;
  ArrayObjectTreeStatic tree;
  TreeState vbostate;

  CircleTree circle;
};

//
// New kind of trees
//
template<class Node, class Link>
class TTree
{
public:
  TTree(Node n) : n(n) { }
  TTree(typename std::vector<TTree<Node,Link> *> &children, 
	Node n, 
	std::vector<Link> links) 
    : children(children), n(n), links(links) { }
    
  int NumNodes() const { return children.size(); }
  Link &GetLink(int i) const { return links[i]; }
  Node &GetNode() const { return n; }
  TTree<Node,Link> *GetChildren(int i) const { }
  ~TTree() 
  {
    int s = children.size();
    for(int i=0;i<s;i++) delete children[i];
  }
private:
  std::vector<TTree*> children;
  Node n;
  std::vector<Link> links;
};



class TValue
{
public:
  virtual void *Value() const=0;
  virtual std::string Type() const=0;
};
template<class T>
class TTValue : public TValue
{
public:
  TTValue(T t) : t(t) { }
  void *Value() const { return &t; }
  std::string Type() const { return typeid(T).name(); }
private:
  T t;
};

class TFunction
{
public:
  virtual void *ret_val() const=0;
  virtual std::string ret_type() const=0;
  virtual int num_params() const=0;
  virtual void set_param(int i, void *val) const=0;
  virtual std::string param_type(int i) const=0;
  virtual void Execute()=0;
  virtual TValue *create_return_value()=0;
};

struct TFunctionArray
{
public:
  std::vector<TFunction*> vec;
};

template<class T, class R>
class TTFunction : public TFunction
{
public:
  TTFunction(Function<T,R> &f) : f(f) { }
  virtual void *ret_val() const { return ret_value; }
  virtual std::string ret_type() const { return typeid(R).name(); }
  virtual int num_params() const { return 1; }
  virtual void set_param(int i, void *val) const 
  {
    if (i==0)
      {
	param1 = val;
      }
  }
  virtual std::string param_type(int i) const { return typeid(T).name(); }
  virtual void Execute()
  {
    T t = *(T*)param1;
    r = f.Index(t);
    ret_value = (void*)&r;
  }
  virtual TValue *create_return_value() { 
    return new TTValue<R>(r);
  }
private:
  Function<T,R> &f;
  void *ret_value;
  R r;
  void *param1;
};

class DynamicTree : public TTree<TFunctionArray, TValue*>
{
};



#endif
