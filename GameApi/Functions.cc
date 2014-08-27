#define NO_SDL_GLEXT
#define _SCL_SECURE_NO_WARNINGS
#include <SDL_opengl.h>

#include "Functions.hh"
#include "Effect.hh"
#include "Equalizer.hh"

void cube(Matrix m, FaceStore &s)
{
  CubeElem c;
  MatrixElem mm(c, m);
  s.Add(mm);
}
void sphere(Matrix m, int numfaces, int numfaces2, FaceStore &s)
{
  SphereElem ss(numfaces, numfaces2);
  MatrixElem mm(ss,m);
  s.Add(mm);
}
void cone(Matrix m, int numfaces, Point p1, Point p2, float rad1, float rad2, FaceStore &s)
{
  ConeElem cn(numfaces, p1,p2,rad1, rad2);
  MatrixElem mm(cn,m);
  s.Add(mm);
}
void texture_rectangle(Matrix m, TexturePlugin *plugin, FaceStore &s)
{
  RectangleElem rect(1.0,1.0);
  SimpleTexCoords coords(rect, plugin->Texture());
  TextureElem texture(rect,coords);
  s.Add(texture);
}
void cylinder(Matrix m, int numfaces, FaceStore &s)
{
  CylinderElem cyl(numfaces);
  MatrixElem mm(cyl, m);
  s.Add(mm);
}
void disk(Matrix m, int numfaces, float disk_pos, FaceStore &s)
{
  DiskElem dsk(numfaces, disk_pos);
  MatrixElem mm(dsk, m);
  s.Add(mm);
}
void color(BoxableFaceCollection &coll, unsigned int color, FaceStore &s)
{
  ColorElem cl(coll, color);
  s.Add(cl);
}

void matrix(BoxableFaceCollection &coll, Matrix m, FaceStore &s)
{
  MatrixElem mm(coll,m);
  s.Add(mm);
}
void andnot(BoxableFaceCollection &coll1, BoxableFaceCollection &coll2, FaceStore &s)
{
  AndNotElem ae(coll1,coll2);
  s.Add(ae);
}
void rectangle(Matrix m, float x, float y, FaceStore &s)
{
  RectangleElem r(x,y);
  MatrixElem mm(r,m);
  s.Add(mm);
}

void ring(Matrix m, const PointVectorCollection &points, float x, int steps, FaceStore &s)
{
  RingElem r(points, x, steps);
  MatrixElem mm(r,m);
  s.Add(mm);
}

void color_poly(Point2d p1, Point2d p2, Point2d p3, Color c1, Color c2, int x, int y, BitmapStore<Color> &s)
{
  ColorTriangle ct(x,y,p1,p2,p3,c1,c2);
  SampleBitmap sample(ct, x, y);
  s.Set(sample);
}

void plane_cube_blit(const FPlane &pl, FaceStore &s)
{
  QuadElem quad(pl.p1,pl.p2,pl.p3,pl.p4);
  s.Add(quad);
}

void floor(const FCube &pos, FLine &l1, FLine &l2, FLine &l3, FLine &l4, FPlane &pl, FaceStore &s)
{
  Point p1 = pos.p + pos.u_x*0.5 - pos.u_z*0.5;
  Point p2 = pos.p + pos.u_x*0.5 + pos.u_z*0.5;
  Point p3 = pos.p - pos.u_x*0.5 + pos.u_z*0.5;
  Point p4 = pos.p - pos.u_x*0.5 - pos.u_z*0.5;
  l1.p1 = p1;
  l1.p2 = p2;
  l1.u_x = pos.u_x;
  l1.u_y = pos.u_y;
  l2.p1 = p2;
  l2.p2 = p3;
  l2.u_x = pos.u_z;
  l2.u_y = pos.u_y;
  l3.p1 = p3;
  l3.p2 = p4;
  l3.u_x = -pos.u_x;
  l3.u_y = pos.u_y;
  l4.p1 = p4;
  l4.p2 = p1;
  l4.u_x = -pos.u_z;
  l4.u_y = pos.u_y;
  pl.p1 = p1;
  pl.p2 = p2;
  pl.p3 = p3;
  pl.p4 = p4;
  pl.u = pos.u_y;
  rectangle(Matrix::XRotation(3.14159/2.0), 100.0,100.0,s);
}

void floorlinetocube(const FLine &line, FCube &cube)
{
  cube.p = Vector(line.p1+line.p2)*0.5 + line.u_x/2;
  cube.u_x = line.u_x;
  cube.u_y = line.u_y;
  cube.u_z = line.p2 - line.p1;
}
void flexcube(const FPlane &pl, float dist_left, float dist_right, float left_angle, float right_angle, FCube2 &res)
{
  FFlexCube c;
  c.dist_left = dist_left;
  c.dist_right = dist_right;
  c.left_angle = left_angle;
  c.right_angle = right_angle;
  flexcube(pl, c, res);
}

void flexcube(const FPlane &pl, 
	      const FFlexCube &args,
	      FCube2 &res)
{
  res.tlb = pl.p1;
  res.trb = pl.p2;
  res.brb = pl.p3;
  res.blb = pl.p4;

  Point p1 = pl.u * args.dist_left;
  Point p2 = pl.u * args.dist_right;
  Matrix m1 = Matrix::YRotation(args.left_angle*2.0*3.14159/360.0);
  Matrix m2 = Matrix::YRotation(args.right_angle*2.0*3.14159/360.0);
  Point pp1 = p1 * m1;
  Point pp2 = p2 * m2;
  
  Point p1a = pp1 + pl.p1;
  Point p2a = pp2 + pl.p2;
  Point p3a = pp2 + pl.p3;
  Point p4a = pp1 + pl.p4;

  res.tlf = p1a;
  res.trf = p2a;
  res.brf = p3a;
  res.blf = p4a;
}

void cubeplane_t(const FCube2 &res, FPlane &pl)
{
  pl.p1 = res.tlb;
  pl.p2 = res.trb;
  pl.p3 = res.trf;
  pl.p4 = res.tlf;
  pl.u = res.tlb - res.blb;
}

void cubeplane_l(const FCube2 &res, FPlane &pl)
{
  pl.p1 = res.tlb;
  pl.p2 = res.tlf;
  pl.p3 = res.blf;
  pl.p4 = res.blb;
  pl.u = res.tlb-res.trb;
}
void cubeplane_r(const FCube2 &res, FPlane &pl)
{
  pl.p1 = res.trf;
  pl.p2 = res.trb;
  pl.p3 = res.brb;
  pl.p4 = res.brf;
  pl.u = res.trf - res.tlf;
}
void cubeplane_f(const FCube2 &res, FPlane &pl)
{
  pl.p1 = res.tlf;
  pl.p2 = res.trf;
  pl.p3 = res.brf;
  pl.p4 = res.blf;
  pl.u = res.tlf-res.tlb;
}
FPlane plane_init()
{
  FPlane p;
  p.p1 = Point(-100.0,-100.0,0.0);
  p.p2 = Point(100.0,-100.0,0.0);
  p.p3 = Point(100.0,100.0,0.0);
  p.p4 = Point(-100.0,100.0,0.0);
  p.u = Vector(0.0,0.0,200.0);
  return p;
}

void tunnel_end(const FPlane &pl, FaceStore &s)
{
  FCube2 cube;
  flexcube(pl, 1.0, 1.0, 0.0, 0.0, cube);
  FPlane p1,p2,p3;
  cubeplane_l(cube, p1);
  cubeplane_r(cube, p2);
  cubeplane_f(cube, p3);
  plane_cube_blit(p1, s);
  plane_cube_blit(p2, s);
  plane_cube_blit(p3, s);
}
void plane_dup(const FPlane &pl, FPlane &pl1)
{
  pl1.p1 = pl.p2;
  pl1.p2 = pl.p1;
  pl1.p3 = pl.p4;
  pl1.p4 = pl.p3;
  pl1.u = -pl.u;
}
FuncEffect::FuncEffect(Render *render) : FrameAnimPlugins(render) 
{
  //tex_index = chessboard_bitmap(this); 
}

void FuncEffect::Init()
{
  FaceCollectionStore store;
  store.SetTextures(true);
  FPlane p = plane_init();
  tunnel_end(p, store);
  FPlane p2;
  plane_dup(p,p2);
  //tunnel_end(p2, store);
  chessboard(p2, store);
  TextureGeneration(store, *this);
  
  FrameAnimPlugins::Init();
}
bool FuncEffect::Frame(float time)
{
  FaceCollectionStore store;
  FPlane p = plane_init();
  tunnel_end(p, store);
  FPlane p2;
  plane_dup(p,p2);
  //tunnel_end(p2, store);
  chessboard(p2, store);
  //glEnable(GL_TEXTURE_2D);
  GetRender()->RenderFacesTexture(store);
  //glDisable(GL_TEXTURE_2D);
  return false;
}
int chessboard_bitmap(FrameAnimPlugins *fa)
{
  //TexturePlugin *plugin = new TexturePlugin(buf);
  //fa->plugins.push_back(plugin);
  return fa->plugins.size()-1;
}

void chessboard(const FPlane &pl, FaceStore &s)
{
  Color c(0,0,0);
  Color c2(255,255,255);
  ChessBoardBitmap bm(c,c2,256/8,256/8,8,8);
  BufferFromBitmap buffer(bm);
  //buffer.Gen();
  //BufferRef buf = buffer.Buffer();
  Quad q = { pl.p1, pl.p2, pl.p3, pl.p4 };
  SingletonArray<Quad> array(q);
  QuadArrayElem rect(array);
  SimpleTexCoords coords(rect, 1);
  TextureElem2 te(rect, buffer, coords);
  s.Add(te);
}


void FunctionPointerFrameAnim::Init()
{
  FaceCollectionStore store;
  store.SetTextures(true);
  fptr(store);
  TextureGeneration(store, *this);
  FrameAnimPlugins::Init();
}
void FunctionPointerFrameAnim::Cleanup()
{
  FrameAnimPlugins::Cleanup();
  //FaceCollectionStore store;
  //fptr(store);
}
bool FunctionPointerFrameAnim::Frame(float time)
{
  FaceCollectionStore store;
  fptr(store);
  //glEnable(GL_TEXTURE_2D);
  GetRender()->RenderFacesTexture(store);
  //glDisable(GL_TEXTURE_2D);
  return false;
}

void chessboard_with_hole(const FPlane &pl, FaceStore &s)
{
  Color c(0,0,0);
  Color c2(255,255,255);
  ChessBoardBitmap bm(c,c2,256/8,256/8,8,8);
  Point2d p1 = { 100.0, 50.0 };
  Point2d p2 = { 50.0, 100.0 };
  Point2d p3 = { 200.0, 200.0 };
  TriPointCollection2d coll(p1,p2,p3);
  TextureWithHole tex(bm, coll);
  BufferFromBitmap buffer(tex);
  //buffer.Gen();
  //BufferRef buf = buffer.Buffer();
  Quad q = { pl.p1, pl.p2, pl.p3, pl.p4 };
  SingletonArray<Quad> array(q);
  QuadArrayElem rect(array);
  SimpleTexCoords coords(rect, 1);
  TextureElem2 te(rect, buffer, coords);
  s.Add(te);
}


void chessboard_test(FaceStore &s)
{
  FPlane p = plane_init();
  chessboard_with_hole(p, s);
}
