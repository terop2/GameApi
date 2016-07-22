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


#include "Tree.hh"
#include "VolumeObjects.hh"

#if 0

std::ostream &operator<<(std::ostream &o, const TreeNode &tn)
{
  o << "(" << tn.matrix << ")";
  return o;
}
std::ostream &operator<<(std::ostream &o, const TreeNodeInternal &tn)
{
  o << "(" << tn.faces->NumFaces() << ")";
  return o;
}
bool operator==(const TreeNodeInternal &n1, const TreeNodeInternal &n2)
{
  return n1.faces == n2.faces || (n1.faces->NumFaces()==0 && n2.faces->NumFaces()==0);
}


void PrepareRecurseLevel(int level, ObjectTree &tree, VectorArray<int> &vec, VBOState &vbostate, VBOUpdate u)
{
}
void RecurseModifyTree(ObjectTree &tree, float time)
{
  VectorArray<int> vec;
  TreeNodeCollection *coll = const_cast<TreeNodeCollection*>(tree.TreeChilds(vec));
  coll->Modify(time);
  int num = coll->Size();
  for(int i=0;i<num;i++)
    {
      //TimeChange *tc = coll->Index(i).timechange;
      //float t = tc->Time(time);
      ObjectSubTree subtree(tree, i);
      RecurseModifyTree(subtree, time);
    }
}

class TreeStateInternal
{
public:
  VBOState vbostate;
  FlattenTree *flat;
  SharedArray *arr;
  MemoizeFunction<int> *mem_enabled;
  MemoizeFunction<int> *mem_mapping;
};

TreeState::TreeState()
{
  priv = new TreeStateInternal;
  priv->flat = 0;
}
TreeState::~TreeState()
{
  delete priv;
}
void PrepareTree(ObjectTree &tree, TreeState &treestate, VBOUpdate u)
{
  RecurseModifyTree(tree, 0.0);

  FlattenTree flat(tree);
  std::cout << flat << std::endl;
  MemoizeArray<TreeNodeInternal> memoize(flat.Size(), flat);
  MemoizeRanges memoize_ranges(flat);
  SharedArray shared(memoize, memoize_ranges);
  std::cout << "Shared: " << shared << std::endl;
  MemoizeArray<TreeNodeInternal> memoize2(shared.Size(), shared);
  MemoizeRanges memoize_ranges2(shared);

  CompressFaceCollectionArrays compress(memoize_ranges2, memoize2);

  VBOArrays vbo(compress);
  UpdateVBO(*vbo.Elem(), treestate.priv->vbostate, u);
}

void RenderTree(ObjectTree &tree, float time, TreeState &treestate, VBOUpdate u)
{
  RecurseModifyTree(tree, time);

  TreeStateInternal *p = treestate.priv;
  if (!p->flat)
    {
      p->flat = new FlattenTree(tree);
      int num = p->flat->NumObjects();
      p->arr = new SharedArray(*p->flat,*p->flat);
      p->mem_enabled = new MemoizeFunction<int>(num, p->arr->enabled_func);
      p->mem_mapping = new MemoizeFunction<int>(num, p->arr->mapping_range_i);
      std::cout << "enabled: " << FunctionArray<int,int>(num, *p->mem_enabled) << std::endl;
      std::cout << "mapping: " << FunctionArray<int,int>(num, *p->mem_mapping) << std::endl;
    }

  FlattenTree flat(tree);
  TreeObjectData objectdata(flat, *p->mem_enabled, *p->mem_mapping);
  //std::cout << objectdata << std::endl;
  RenderOpenGlObjects(objectdata, treestate.priv->vbostate, u, 0); // TODO shader
}

TreeNode matrix_treenode(Matrix m)
{
  TreeNode n;
  n.matrix = m;
  //BoxableFaceCollection *cube = new CubeElem;
  //cube->SetBox(Matrix::Identity());
  //BoxableFaceCollection *mm = new MatrixElem(*cube, Matrix::Scale(100.0,100.0,100.0));
  n.faces = new EmptyBoxableFaceCollection;
  n.quads = true;
  n.params = 0;
  n.timechange = new EmptyTimeChange;
  return n;
}

TreeNode faces_treenode(Matrix m, const BoxableFaceCollection *faces)
{
  TreeNode n;
  n.matrix = m;
  n.faces = faces;
  n.quads = true;
  n.params = 0;
  n.timechange = new EmptyTimeChange;
  return n;
}

std::pair<Point, Vector> RingTreeNodes::array[] = 
  { 
    std::make_pair(Point(-30.0,-10.0,0.0), Vector(-1.0,-1.0,0.0)),
    std::make_pair(Point(30.0,-10.0,0.0), Vector(1.0, -1.0,0.0)),
    std::make_pair(Point(30.0,10.0,0.0), Vector(1.0, 1.0,0.0)),
    std::make_pair(Point(-30.0,10.0,0.0), Vector(-1.0, 1.0,0.0))
  };


RingTreeEffect::RingTreeEffect(Render *r)
  : FrameAnim(r), nodes(), tree(nodes), /*circle(tree),*/ forestarray(&tree), forest(forestarray), cpath(500.0, 5050.0, 5, 200.0), pos(forest, cpath)
{
}

void RingTreeEffect::Init()
{
  PrepareTree(pos, vbostate);
}
bool RingTreeEffect::Frame(float time)
{
  RenderTree(pos, time, vbostate);
  return false;
}
ManyCubesEffect::ManyCubesEffect(Render *r) : FrameAnim(r), me(cube, Matrix::Scale(10.0,10.0,10.0)), box(400, Point(0.0,0.0,0.0), 500.0,500.0,500.0), matrix(box), nodes(matrix, me), tree(nodes), circle(tree) { cube.SetBox(Matrix::Identity()); }

void ManyCubesEffect::Init()
{
  PrepareTree(circle, vbostate);
}
bool ManyCubesEffect::Frame(float time)
{
  RenderTree(circle, time, vbostate);
  return false;
}

void RenderTreeCmds::PreCalc()
{
  PrepareTree(tree, treestate);
}
void RenderTreeCmds::Execute()
{
  RenderTree(tree, time, treestate);
}

#endif
