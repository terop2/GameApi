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


//#define GL_GLEXT_PROTOTYPES
#define _SCL_SECURE_NO_WARNINGS
#include "Graph.hh"
#include "Event.hh"
#define NO_SDL_GLEXT
//#include <GL/glew.h>
//#include <SDL_opengl.h>
#include "Shader.hh"
#include "Effect2.hh"
#include "Font.hh"
#include "VertexArray.hh"
#include "GameApi_low.hh"

#ifndef ARM
#ifndef NO_THREADS
#define THREADS 1
#endif
#endif
void RenderVoxel(VolumeObject &orv, int size, float wholesize, HandleValue<std::pair<Vector, unsigned int> > &hv)
{
  float step = wholesize/size;
  float length = size * step;
  //float box = (step - 0.0)/2.0;
  

  VolumeVoxel vox(orv, 
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0));
  SampleVoxel<bool> samp(vox, size,size,size);

  ColorHandleValue color(hv, length, length, length);
  CubeHandleValue cubehv(Point(-length/2.0, -length/2.0, -length/2.0), 
			 Vector(step,0.0,0.0),
			 Vector(0.0,step,0.0),
			 Vector(0.0,0.0,step),
			 color);
      //FilterHandleValue<Cube2> filter(cubehv);
      //GenerateVoxel<bool> gen(samp, filter);
  MarchingCubesVoxel gen(samp, cubehv);
  gen.Gen();
}
#undef LoadImage
VoxelEffect::VoxelEffect(Render *r)
  : FrameAnimPlugins(r)
{
#if 0
  bool b;
  BufferRef buf1 = LoadImage("./textures/texture_carpet2.jpeg", b);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();
  plugins.push_back(new TexturePlugin(buf));
#endif
}
void VoxelEffect::Init()
{
  FrameAnimPlugins::Init();
  CubeElem cube;
  cube.SetBox(Matrix::Identity());
  UpdateVBO(cube, vbostate, UpdateAll);
  first = true;

  shader.push_back(&globpiece);
  shader.push_back(&texturepiece);
  shader.push_back(&phongpiece);
  shader.push_back(&normalpiece);
}
VoxelEffect::~VoxelEffect()
{
  shader.unuse();
}
bool VoxelEffect::Frame(float time)
{
#if 0
  OpenglLowApi *ogl = g_low->ogl;
  shader.use();

  GlobalTextureParameters params3(Point(0,0,0), Vector(2400.0,0.0,0.0), Vector(0.0, 2400.0, 0.0), Vector(0.0, 0.0, 2400.0));
  shader.set_params(params3);
  TextureParameters params4(0);
  shader.set_params(params4);

  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,250,240);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);
  NormalParameters np(Point(0.0,0.0,0.0),0.1);
  shader.set_params(np);

  ogl->glEnable(Low_GL_TEXTURE_2D);

  // todo, first prevents anim
  if (first)
    {
      SphereVolume vol(Point(0.0,0.0,0.0), 1.8);
      SphereVolume vol2(Point(1.0,0.0,0.0), 1.8);
      AndNotVolume orv(vol, vol2);
      RenderVoxel(orv, 40, 800.0, hv1a);

      TorusVolume tvol(Vector(1.0,0.0,0.0), Vector(0.0,1.0,0.0),
      		  1.4, 0.6, Point(0.0,0.0,0.0));
      RenderVoxel(tvol, 40, 800.0, hv2a);
      int maxsize = std::max(hv1a.Size(), hv2a.Size());
      ResizeArray<std::pair<Vector, unsigned int> > hv1(hv1a, std::make_pair(Vector(0,0,0),0), maxsize);
      ResizeArray<std::pair<Vector, unsigned int> > hv2(hv2a, std::make_pair(Vector(0,0,0),0), maxsize);
      
      
      SndArray<int, Vector, unsigned int> sndhv1(hv1);
      FstArray<int, Vector, unsigned int> fsthv1(hv1); // Array<int, Vector>
      FstArray<int, Vector, unsigned int> fsthv2(hv2); // Array<int, Vector>
      VectorPointConversion conv;
      ComposeArrays<int, Vector, Point> comphv1(fsthv1, conv); // Array<int, Point>
      ComposeArrays<int, Vector, Point> comphv2(fsthv2, conv); // Array<int, Point>
      Array<int,Point>* array[] = { &comphv1, &comphv2 };
      VectorArray<Array<int,Point>*> vec(array, array+2); // Array<int, Array<int,Point> *>
      SwapIndexesArray<int, int, Point> swapped(vec); // Array<int, Array<int, Point>* >
      PointCollectionConvertArray conv2(swapped); // Array<int, PointCollection*>
      // todo...
      float fvec[] = { 0.0, 1.0 };
      VectorArray<float> floatvec(fvec, fvec+2);
      FloatArrayConvert floatarr(floatvec);
      SplineCurves curves(conv2, floatarr); // Array<int, CurveIn3d*>
      CurveArrayConvert curveconv(curves);
      ApplyParameterArray<float, Point> params(curveconv, time); // Array<int, Point>
      PointVectorConversion pointvec;
      ComposeArrays<int, Point, Vector> params2(params, pointvec);
      ArrayPair<int, Vector, unsigned int> pair(params2, sndhv1);
      
      

      int size = 40;
      float wholesize = 800.0;
      float step = wholesize/size;
      float box = (step - 0.0)/2.0;

      ScaleTranslateHandleValue<unsigned int> trans2(hv,box, box, box); 
      HandleValueSource<std::pair<Vector, unsigned int> > source(hv2a /*pair*/, trans2);
      source.Gen();
    }
      first = false;

#if 0
      
      int size = 20;
      float wholesize = 800.0;
      float step = wholesize/size;
      float length = size * step;
      float box = (step - 0.0)/2.0;
      
      //TorusVolume vol(Vector(1.0,0.0,0.0), Vector(0.0,1.0,0.0),
      //		  1.4, 0.6, Point(0.0,0.0,0.0));
      VolumeVoxel vox(orv, 
		      Range<float>(-2.0,2.0),
		      Range<float>(-2.0,2.0),
		      Range<float>(-2.0,2.0));
      SampleVoxel<bool> samp(vox, size,size,size);
      //
      //FaceCollectionHandleValueDynamic hv;
      ScaleTranslateHandleValue<unsigned int> trans(hv,box, box, box); 
      ColorHandleValue color(trans, length, length, length);
      CubeHandleValue cubehv(Point(-length/2.0, -length/2.0, -length/2.0), 
			     Vector(step,0.0,0.0),
			     Vector(0.0,step,0.0),
			     Vector(0.0,0.0,step),
			     color);
      //FilterHandleValue<Cube2> filter(cubehv);
      //GenerateVoxel<bool> gen(samp, filter);
      MarchingCubesVoxel gen(samp, cubehv);
      gen.Gen();
#endif
      //    }
  hv.DrawVBO(vbostate, UpdateAll);
  ogl->glDisable(Low_GL_TEXTURE_2D);

  return false;
#endif
}


void FaceCollectionHandleValueDynamic::DrawVBO(VBOState &vbostate, VBOUpdate u)
{
  OpenglLowApi *ogl = g_low->ogl;
  for(int i=0;i<vec.Size();i++)
    {
      Node n = vec.Index(i);
      //Matrix m = p.first;
      ogl->glPushMatrix();
      //float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
      //			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
      //			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
      //			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#ifndef EMSCRIPTEN

      ogl->glMultMatrixf(&n.mat[0]);
#endif
      unsigned int color = n.color;
      ogl->glColor4ub(color, color>>8, color>>16, color>>24);
      ::DrawVBO(vbostate, u);
      ogl->glPopMatrix();
    }
}

MapEffect::MapEffect(Render *r) : FrameAnimPlugins(r), mapfunc("./maps/map1.txt", mapfunc1, 5,6),
			 colorfunc("./maps/map1.txt", colorfunc1, 5,6) 
{ 
#if 0
  bool b;
  BufferRef buf1 = LoadImage("./textures/texture_carpet2.jpeg", b);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();
  plugins.push_back(new TexturePlugin(buf));

  //shader.push_back(&globpiece);
  shader.push_back(&texturepiece);
  //shader.push_back(&phongpiece);
  //shader.push_back(&normalpiece);
  shader.config();
#endif
}
MapEffect::~MapEffect()
{
  shader.unuse();
}
void MapEffect::Init()
{
#if 0
  FrameAnimPlugins::Init();

  RectangleElem rect(1500.0,1500.0);
  //CubePlane rect(5,5,1500.0,1500.0, 50.0);
  rect.SetBox(Matrix::Identity());
  SimpleTexCoords coords(rect, ((TexturePlugin*)plugins[0])->Texture());
  TextureElem texture(rect, coords);
  //MemoizeFaces memoize(texture);
  UpdateVBO(texture, vbostate, UpdateAllTexture);
  first = true;
#endif
}
bool MapEffect::Frame(float time)
{
#if 0
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glEnable(Low_GL_TEXTURE_2D);
  ogl->glColor4f(1.0,1.0,1.0,0.0);
  shader.use();

  GlobalTextureParameters params3(Point(0,0,0), Vector(2400.0,0.0,0.0), Vector(0.0, 2400.0, 0.0), Vector(0.0, 0.0, 2400.0));
  shader.set_params(params3);
  TextureParameters params4(0);
  shader.set_params(params4);

  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,250,240);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);
  NormalParameters np(Point(300000.0*cos(time/6.0),300000.0*sin(time/3.0),400000.0*sin(time/5.0)),0.9);
  shader.set_params(np);


  if (first)
    {
      first = false;
      WallMatrix wallmatrix(hv, 1500.0);
      WallsFromMap walls(mapfunc, colorfunc, wallmatrix, 1,1);
      walls.Gen();
    }
  ogl->glPushMatrix();
  Point array[] = { Point(2.0, -1.0, 2.0),
		    Point(1.0, 1.0, 4.0),
		    Point(1.0, -1.0, 8.0),
		    Point(4.0, 1.0, 8.0),
		    Point(9.0, -1.0, 7.0),
		    Point(4.0, 1.0, 7.0),
		    Point(4.0, 1.0, 2.0),
		    Point(8.0, 1.0, 2.0),
		    Point(2.0, -1.0, 2.0)
  };
  VectorArray<Point> arr(array, array+9);
  PointCollectionConvert conv(arr);
  LinearInterpolation bezier(conv);
  time /= 3.0;
  time /= 2.0;
  time /= 3.14159;
  while(time > bezier.Size())
    time -= bezier.Size();
  Point p = bezier.Index(time);
  float rot = 0.0; //time/30.0;
  float x = /*2.0 +*/ p.z-0.5; //cos(-time/30.0);
  float y = 0.07 + 0.4*p.y; //sin(-time/10.0);
  float z = /*2.0 +*/ p.x-0.5; //sin(-time/30.0);
  Matrix m = Matrix::Translate(-1500.0/2 - x * 1500.0, y*1500.0 ,-1500.0/2 - z * 1500.0)*Matrix::YRotation(rot);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#ifndef EMSCRIPTEN
  ogl->glMultMatrixf(&mat[0]);
#endif

  hv.DrawVBO(vbostate, UpdateAll);
  ogl->glDisable(Low_GL_TEXTURE_2D);

  ogl->glPopMatrix();
  return false;
#endif
}

SuperParameters odd_1 = { 1.00, 1.00, 7.00, 0.20, 1.70, 1.70 };
SuperParameters odd_2 = {1.00, 1.00, 7.00, 0.20, 1.70, 1.70 };

SuperParameters curved_1 = { 1.00, 1.00, 5.00, 60.0, 100, 30 };
SuperParameters curved_2 = { 1.00, 2.00, 2.00, 10.0, 10, 10 };

bool SuperShapeEffect::Frame(float time)
{
  //                  a      b     m    n1    n2    n3
  SuperShape3d shape(odd_1,
		     odd_2);
  SampleSurfaceIn3d surf(shape, 0, 60,60);
  BoxableFaceCollectionConvert conv(surf);
  MatrixElem elem(conv, Matrix::Scale(400.0,400.0,400.0));
  RenderOpenGl(elem);
  return false;
}

RenderToTextureEffect::RenderToTextureEffect(Render *r)
  : FrameAnimPlugins(r), e(600.0,600.0),
    tex(e,0),
    render(e, tex),
    tex2(render),
    trans(tex2),
    col(trans, Color(255,255,255,255)),
    disp(col)
{
#if 0
  bool b;
  BufferRef buf1 = LoadImage("./textures/texture_carpet2.jpeg", b);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();

  plugins.push_back(new TexturePlugin(buf));
  tex.SetTexture(((TexturePlugin*)plugins[0])->Texture());
#endif
}
void RenderToTextureEffect::Init()
{
  FrameAnimPlugins::Init();
  //SuperShapeEffect fa;
  //FrameAnimCmds cmds(fa, 0.0);
  //RenderToTexture rend((TexturePlugin*)plugins[0], cmds, true);
  //TransparencyCmds rend(cmds);
  //PrecalcRenderCmds(rend);
  e.SetBox(Matrix::Identity());
  PrecalcCmds(col);
}
bool RenderToTextureEffect::Frame(float time)
{
  RenderCmds(col);
  return false;
}
void InterpolateMovementEffect::Init()
{
  float wholesize = 1000.0;
  int size = 40;
  float step = wholesize/size;
  float length = size * step;

  SphereVolume vol(Point(0.0,0.0,0.0), 1.8);
  SphereVolume vol2(Point(1.0,0.0,0.0), 1.8);
  AndNotVolume orv(vol, vol2);
  VolumeVoxel vox(orv, 
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0));
  SampleVoxel<bool> samp(vox, size,size,size);

  VectorPointConversion conv;
  HandleValueFunction<Vector, Point> func(hv, conv);
  CubeHandleValue cubehv(Point(-length/2.0, -length/2.0, -length/2.0), 
			 Vector(step,0.0,0.0),
			 Vector(0.0,step,0.0),
			 Vector(0.0,0.0,step),
			 func);
  MarchingCubesVoxel gen(samp, cubehv);
  gen.Gen();

  //
  // Object 2
  //

  TorusVolume orv2(Vector(1.0,0.0,0.0), Vector(0.0,1.0,0.0),
		   1.4, 0.6, Point(0.0,0.0,0.0));
  VolumeVoxel vox2(orv2, 
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0),
		  Range<float>(-2.0,2.0));
  SampleVoxel<bool> samp2(vox2, size,size,size);

  VectorPointConversion conv2;
  HandleValueFunction<Vector, Point> func2(hv2, conv2);
  CubeHandleValue cubehv2(Point(-length/2.0, -length/2.0, -length/2.0), 
			 Vector(step,0.0,0.0),
			 Vector(0.0,step,0.0),
			 Vector(0.0,0.0,step),
			 func2);
  MarchingCubesVoxel gen2(samp2, cubehv2);
  gen2.Gen();

  CubeElem cube;
  //SphereElem cube(3,3);
  //RectangleElem cube(50.0,50.0);
  cube.SetBox(Matrix::Identity());
  MatrixElem m(cube, Matrix::Scale(wholesize/size/2,wholesize/size/2,wholesize/size/2));
  UpdateVBO(m, vbostate, UpdateAll);

  shader.push_back(&linshader);
  shader.push_back(&phong);
}
InterpolateMovementEffect::~InterpolateMovementEffect()
{
  shader.unuse();
}
bool InterpolateMovementEffect::Frame(float time)
{
#if 0
  shader.use();
  
  Point viewer(0.0, 0.0, 0.0);
  Point light(1.0,1.0,3.0);
  Color light_color(255,250,240);
  Color spec_color(128,64,0);
  PhongParameters pp(viewer, light, light_color, spec_color);
  shader.set_params(pp);


  PointCollectionConvert pc(hv); // OK
  PointCollectionConvert pc2(hv2); // OK
  PointCollection *array[] = { &pc, &pc, &pc2, &pc2, &pc };
  float farray[]           = { 0.0, 200.0, 400.0, 600.0, 800.0 };
  VectorArray<float> farr(farray, farray+5);
  FloatArrayConvert fconv(farr);
  VectorArray<PointCollection*> arr(array, array+5);
  SizeCollections sized(arr);
  DrawVBO(sized, fconv, time, vbostate, UpdateAll, shader);
  //LinearMovement move(sized, fconv); // TimedPointCloud
  //TranslatePaths trans(move); // TimedMatrixPaths
  //DrawVBO(trans, time, vbostate, UpdateAll);
  return false;
#endif
}

void GraphEffect::Init()
{
#if 0
  MapFunction mapfunc;
  BitmapFromFile<bool> bitmap("./maps/map1.txt", mapfunc, 10,11);
  PointGrid pos(3,3, Point(0.0,0.0,0.0), Vector(200.0,0.0,0.0), Vector(0.0,200.0,0.0), 10,11);
  GraphFromMap graph(bitmap, pos, false, 50.0);
  SphereDataRep sphere(10, 10);
  ConeDataRep cone(30);
  RenderGraph<VertexData3d, EdgeData3d> render(graph, graph, sphere, cone);
  FaceCollection *faces = &render;
  UpdateVBO(*faces, vbostate);
#endif
  //FacesFromMap mymap(bitmap, pos);
  //UpdateVBO(mymap, vbostate2);
}
bool GraphEffect::Frame(float time)
{
  DrawVBO(vbostate);
  //DrawVBO(vbostate2);
  return false;
}

void ShaderEffect::Init()
{
#if 0
  s = seq.GetShader("sphere:surfacemain:crossproduct", "colormain:phong", "");
  seq.use(s);
  Program *prog = seq.prog(s);
  Attrib a = prog->find_attr("u_x", AttrInternalX);
  Attrib b = prog->find_attr("u_y", AttrInternalY);
  std::vector<Attrib> vec;
  vec.push_back(a);
  vec.push_back(b);
  Plane pl(Point(0.0,0.0,0.0), Vector(500.0,0.0,0.0), Vector(0.0,500.0,0.0));
  PlaneSurfaceIn3d surf(pl);
  RectangleElem rect(500.0,500.0);
  QuadsToSurfaces quads(rect);
  //SampleSurfaceForGrid grid(surf, 2, 20);
  SampleSurfaceIn3d faces(*quads.Index(0), 0, 15,15);
  //SampleGrid faces(grid, 0);
  UpdateVBO(faces, vbostate, UpdateAll , vec);
#endif
}

bool ShaderEffect::Frame(float time)
{
  seq.use(s);
  Program *prog = seq.prog(s);
  prog->set_var("lightcolor", Color(255,255,255));
  prog->set_var("speccolor", Color(128,128,128));
  Attrib a = prog->find_attr("u_x", AttrInternalX);
  Attrib b = prog->find_attr("u_y", AttrInternalY);
  std::vector<Attrib> vec;
  vec.push_back(a);
  vec.push_back(b);
  Point p = Point(-2*250.0, -2*250.0, -2*250.0);

  for(int x=0;x<4;x++)
    for(int y=0;y<4;y++)
      for(int z=0;z<4;z++)
	{
	  Point m = Point(x*250.0, y*250.0, z*250.0);
	  prog->set_var("center", p+m);
	  prog->set_var("radius", 100.0f);
	  //prog->set_var("up", p+m+Point(0.0,0.0,0.0));
	  //prog->set_var("ux", p+m+Point(100.0,0.0,0.0));
	  //prog->set_var("uy", p+m+Point(0.0,100.0,0.0));
	  DrawVBO(vbostate, UpdateAll , vec);
	}
  seq.unuse(s);
  return false;
}

void Effect2dEffect::Init()
{
  s = seq.GetShader("texture_main", "texture_main:electric_color:interference", "");
  seq.use(s);
  Program *prog = seq.prog(s);
  Attrib a = prog->find_attr("x", AttrInternalX);
  Attrib b = prog->find_attr("y", AttrInternalY);
  RectangleElem rect(500.0,500.0);
  std::vector<Attrib> vec;
  vec.push_back(a);
  vec.push_back(b);
  UpdateVBO(rect, vbostate, UpdateAll, vec);
}
bool Effect2dEffect::Frame(float time)
{
  seq.use(s);
  Program *prog = seq.prog(s);
  Attrib a = prog->find_attr("x", AttrInternalX);
  Attrib b = prog->find_attr("y", AttrInternalY);
  RectangleElem rect(100.0,100.0);
  std::vector<Attrib> vec;
  vec.push_back(a);
  vec.push_back(b);
  Point2d p1;
  p1.x = cos(time/20.0);
  p1.y = sin(time/10.0);
  prog->set_var("p1", p1.x,p1.y);
  Point2d p2;
  p2.x = cos(time/60.0);
  p2.y = sin(time/90.0);
  prog->set_var("p2", p2.x,p2.y);
  prog->set_var("time", float(-time/3.0));
  DrawVBO(vbostate, UpdateAll, vec);
  return false;
}

void Effect3dEffect::Init()
{
#if 0
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  s = seq.GetShader("coords_main", "render_main:red_color:blob", "");
  seq.use(s);
  Program *prog = seq.prog(s);
  Attrib a = prog->find_attr("x", AttrInternalX);
  Attrib b = prog->find_attr("y", AttrInternalY);
  Attrib c = prog->find_attr("z", AttrInternalZ);
  RenderBox box(20,20,20,300.0,300.0,300.0);
  std::vector<Attrib> vec;
  vec.push_back(a);
  vec.push_back(b);
  vec.push_back(c);
  UpdateVBO(box, vbostate, UpdateAll, vec);
#endif
}

bool Effect3dEffect::Frame(float time)
{
  Program *prog = seq.prog(s);
  prog->set_var("dist", float(0.8+0.3*cos(time/30.0)));
  DrawVBO(vbostate, UpdateAll);
  return false;
}
Effect3dEffect::~Effect3dEffect()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glDisable(Low_GL_BLEND);
  ogl->glEnable(Low_GL_DEPTH_TEST);
  
}


Cube3 CubeVoxelArea(Voxel<Cube3> &cubes, PointI p1, PointI p2)
{
  Cube3 c = cubes.Map(p1.x,p1.y,p1.z);
  Cube3 c2 = cubes.Map(p2.x,p2.y,p2.z);
  Cube3 cc;
  cc.minx = c.minx;
  cc.miny = c.miny;
  cc.minz = c.minz;
  cc.maxx = c2.maxx;
  cc.maxy = c2.maxy;
  cc.maxz = c2.maxz;
  return cc;
}
Color GradientBitmap::Map(float x, float y) const
  {
    Point2d p = { x,y };
    Vector2d A = p-p1;
    Vector2d B = p2-p1;
    //B /= B.Length();
    float r;
    r = A.dx*B.dx;
    r+= A.dy*B.dy;
    //r /= A.Length();
    //r /= A.Length();
    r /= B.Length();
    r /= B.Length();
    //r /= A.Length();
    if (r < 0.0) return c1;
    if (r > 1.0) return c2;
    return Color::Interpolate(c2,c1,r);
  }

static float det(float a, float b, float c, float d)
{
  return a*d-b*c;
}

static Point2d I( float x1, float x2, float x3, float x4,
		 float y1, float y2, float y3, float y4)
{
  float x = det(det(x1,y1,x2,y2),det(x1,1.0,x2,1.0), det(x3,y3,x4,y4), det(x3,1.0,x4,1.0)) / det(det(x1,1.0,x2,1.0),det(y1,1.0,y2,1.0),det(x3,1.0,x4,1.0), det(y3,1.0,y4,1.0));
  
  float y = det(det(x1,y1,x2,y2),det(y1,1.0,y2,1.0), det(x3,y3,x4,y4), det(y3,1.0,y4,1.0)) / det(det(x1,1.0,x2,1.0),det(y1,1.0,y2,1.0),det(x3,1.0,x4,1.0),det(y3,1.0,y4,1.0));

  Point2d p;
  p.x = x;
  p.y = y;
  return p;
}

Point2d IntersectLines(const Line2d &l1, const Line2d &l2)
{
  return I(l1.p1.x,l1.p1.y,l1.p2.x,l1.p2.y,
	   l2.p1.x,l2.p1.y,l2.p2.x,l2.p2.y);
}
float AngleLines(const Point2d &intersect, const Point2d &l1, const Point2d &l2)
{
  float dx1 = l1.x-intersect.x;
  float dy1 = l1.y-intersect.y;
  float dx2 = l2.x-intersect.x;
  float dy2 = l2.y-intersect.y;
  float angle1 = atan2f(dy1, dx1);
  float angle2 = atan2f(dy2, dx2);
  return fabs(angle1-angle2);
}

Quad QuadIterpolation(const Quad &q1, const Quad &q2, float val)
{
  Quad q;
  q.p1 = Vector(q1.p1) * (1.0-val) + Vector(q2.p1)*val;
  q.p2 = Vector(q1.p2) * (1.0-val) + Vector(q2.p2)*val;
  q.p3 = Vector(q1.p3) * (1.0-val) + Vector(q2.p3)*val;
  q.p4 = Vector(q1.p4) * (1.0-val) + Vector(q2.p4)*val;
  return q;
}
void BlitBitmap(BufferRef ref, int xx, int yy, Bitmap<Color> &color)
{
  int sx = color.SizeX();
  int sy = color.SizeY();
  int pos = xx+yy*ref.ydelta;
  for(int y=0;y<sy;y++)
    {
    for(int x=0;x<sx;x++)
      {
	ref.buffer[pos+x]=color.Map(x,y).Pixel(); 
      }
    pos+=ref.ydelta;
    }
}

class RectangleMesh : public Mesh
{
public:
  RectangleMesh(int sx, int sy) : sx(sx), sy(sy) { }
  virtual int NumStates() const { return 0; }
  virtual int StateNum(int frame, int face) const { return 0; }

  int NumFrames() const { return 1; }
  int NumFaces(int framenum) const { return 1; }
  int NumPoints() const { return 4; }
  Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    switch(point)
      {
      case 0: return Point(0.0,0.0, 0.0);
      case 1: return Point(sx, 0.0, 0.0);
      case 2: return Point(sx,sy, 0.0);
      case 3: return Point(0.0,sy, 0.0);
      };
    return Point(0.0,0.0,0.0);
  }
private:
  int sx,sy;
};
class HexagonMesh : public Mesh
{
public:
  HexagonMesh(int len) : len(len) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  
  virtual int NumStates() const { return 0; }
  virtual int StateNum(int frame, int face) const { return 0; }

  int NumFrames() const { return 1; }
  int NumFaces(int framenum) const { return 6; }
  int NumPoints() const { return 3; }
  Point FacePoint(int framenum, int statenum, int face, int point) const
  {
    Point p(len, 0.0,0.0);
    Matrix m = Matrix::ZRotation(60.0*2*3.14159/360.0);
    Point p2 = p * m;
    Point p3(0.0,0.0,0.0);

    Matrix m2 = Matrix::ZRotation(60.0*2*3.14159/360.0*face);
    p = p * m2;
    p2 = p2 * m2;

    switch(point)
      {
      case 0: return p3;
      case 1: return p2;
      case 2: return p;
      };
    return Point(0.0,0.0,0.0);
  }

private:
  int len;
};

class HexagonSteps : public Bitmap<Point>
{
public:
  HexagonSteps(int len, Point firstpos, int sx, int sy) : sx(sx), sy(sy)
  {
    Point p(len*2,0,0);
    Matrix m = Matrix::ZRotation(30*2*3.14159/360.0);
    Point pa = p*m;
    Matrix m2 = Matrix::ZRotation(-30*2*3.14159/360.0);
    Point p2a = p*m2;
    u_x_1 = pa-p;
    u_x_2 = p2a-p;
    Point b(0,len*2,0);
    Point pb = b*m;
    Point pb2 = b*m2;
    u_y_1 = pb - p;
    u_y_2 = pb2 - p;    
  }
  void Collect(CollectVisitor &vis){ }
  void HeavyPrepare() { }
  void Prepare() { }

  int SizeX() const { return sx; }
  int SizeY() const { return sy; }
  Point Map(int x, int y) const 
  {
    Point p = firstPoint + (x/2)*(u_x_1+u_x_2) + (y/2)*(u_y_1+u_y_2);
    if (x&0x1) { p += u_x_1; }
    if (y&0x1) { p += u_y_1; }
    return p;
  }
private:
  int sx, sy;
  Point firstPoint;
  Vector u_x_1;
  Vector u_x_2;
  Vector u_y_1;
  Vector u_y_2;
};

class HexagonMeshTexCoords : public MeshTexCoords
{
public:
  HexagonMeshTexCoords(int sx, int sy, int len, Point2d firstpos) : sx(sx), sy(sy), mesh(len), steps(len, Point(firstpos.x, firstpos.y, 0.0), sx,sy) { }
  int NumFrames() const { return sx*sy; }
  Point2d TexCoord(int framenum, int statenum, int face, int point) const
  {
    int frame_y = framenum / sx;
    int frame_x = framenum - frame_y*sx;

    Point p = mesh.FacePoint(0, 0, face, point);
    p += Vector(steps.Map(frame_x, frame_y));
    Point2d pp;
    pp.x = p.x;
    pp.y = p.y;
    return pp;
  }

private:
  int sx,sy;
  HexagonMesh mesh;
  HexagonSteps steps;
};

class RectangleMeshTexCoords : public MeshTexCoords
{
public:
  RectangleMeshTexCoords(int index_sx, int index_sy, int cell_sx, int cell_sy, int texture_sx, int texture_sy) : sx(index_sx), sy(index_sy), cellsx(cell_sx), cellsy(cell_sy), texture_sx(texture_sx), texture_sy(texture_sy) { }
  int NumFrames() const { return sx*sy; }
  Point2d TexCoord(int framenum, int statenum, int face, int point) const
  {
    int frame_y = framenum / sx;
    int frame_x = framenum - frame_y*sx;

    Point2d p = { float(frame_x * cellsx), float(frame_y * cellsy) }; 
    switch(point)
      {
      case 0: break;
      case 1: p.x += cellsx; break;
      case 2: p.x += cellsx; p.y+=cellsy; break;
      case 3: p.y += cellsy; break;
      };
    p.x /= texture_sx;
    p.y /= texture_sy;
    return p;
  }
private:
  int sx,sy;
  int cellsx, cellsy;
  int texture_sx, texture_sy;
};


void PrepareGrid(Bitmap<Color> &bm, int cellsx, int cellsy, ArrayRender &rend)
{
  rend.AllocTexture(1);
  MeshTexturesImpl t(bm);
  rend.UpdateTexture(t, 0);
  BufferRef r = t.TextureBuf(0);
  int index_count_x = bm.SizeX()/cellsx;
  int index_count_y = bm.SizeY()/cellsy;
  RectangleMesh r_mesh(cellsx, cellsy);
  rend.Alloc(r_mesh.NumFaces(0), r_mesh.NumFaces(0)*r_mesh.NumPoints(), 1, 0, 0, index_count_x*index_count_y);
  RectangleMeshTexCoords r_texcoord(bm.SizeX()/cellsx, bm.SizeY()/cellsy, cellsx, cellsy, r.width, r.height);
  MeshNormals normal;
  MeshColors color;
  rend.InsertAll(r_mesh, normal, r_texcoord, color);
  rend.Prepare();
}
void RenderGrid(Bitmap<Pos> &bm, float x, float y, int sx, int sy, ArrayRender &rend, int start_x, int start_y, int size_x, int size_y)
{
  OpenglLowApi *ogl = g_low->ogl;
  rend.EnableTexture(0);
  int sizex = bm.SizeX();
  int sizey = bm.SizeY();
  ogl->glPushMatrix();
  Matrix m = Matrix::Translate(x, y, 0.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  
#ifndef EMSCRIPTEN
  ogl->glMultMatrixf(&mat[0]);
#endif

  for(int y=start_y;y<sizey&&y<start_y+size_y;y++)
    for(int x=start_x;x<sizex&&x<start_x+size_x;x++)
      {
	ogl->glPushMatrix();

	Matrix m = Matrix::Translate(x*sx, y*sy, 0.0);
	float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };

#ifndef EMSCRIPTEN
	ogl->glMultMatrixf(&mat[0]);
#endif

	Pos p = bm.Map(x,y);
	rend.Render(0, -1, -1, p.x+p.y*sizex, 0, rend.used_vertex_count[0]);
	ogl->glPopMatrix();
      }
  ogl->glPopMatrix();
  rend.DisableTexture();
}

class ColorFunction : public Function<char, Color>
{
public:
  Color Index(char c) const
  {
    switch(c)
      {
      case 'a': return Color(0,0,0);
      };
    return Color(255,255,255);
  }
};

class BoolPos : public Function<bool,Pos>
{
public:
  Pos Index(bool b) const 
  {
    Pos p = { 0,0 };
    Pos p2 = { 1,0 };
    if (!b) return p;
    return p2;
  }
};

void GridEffect2::Init()
{
  ColorFunction c;
  BitmapFromFile<Color> bm("./maps/map1.txt", c, 20, 11);
  PrepareGrid(bm, 10,11, rend);
  scr.SetTargetPos(scr.TargetRangeX(), scr.TargetRangeY());
}
bool GridEffect2::Frame(float time)
{
  OpenglLowApi *ogl = g_low->ogl;
  scr.Frame();
  ogl->glPushMatrix();
  Matrix m = Matrix::Scale(10.0, 10.0, 0.0)*Matrix::Translate(0.0,0.0,0.0);
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  
#ifndef EMSCRIPTEN
  ogl->glMultMatrixf(&mat[0]);
#endif

  //Pos p = { 1,0 };
  Point2d center = { 25.0,25.0 };
  BitmapCircle circle(center, 20.0, 500,500);
  BoolPos px;
  MapBitmap<bool,Pos> bm(circle, px);
  //ConstantBitmap<Pos> bm(p, 300, 300);
  
  //std::cout << scr.PosX() << " " << scr.PosY() << std::endl;
  RenderGrid(bm, -scr.PosX()-1024/2/10,-scr.PosY()-768/2/10, 10,11, rend,
	     scr.PosX()/10-3,scr.PosY()/11-1,30,30);
	     //scr.PosX()/10/10,scr.PosY()/11/10,1024/10/10+1,768/11/10+1);
  ogl->glPopMatrix();
  return false;
}
 
void ProgressBar(int num, int val, int max, std::string label);
void InstallProgress(int num, std::string label, int max=15);

struct ThreadInfo_sprite
{
  pthread_t thread_id;
  const Sprite *s;
  BufferRef *ref;
  int start_y;
  int end_y;
  int sx;
  int num;
};

int g_sprite_count=0;
int g_sprite_count2 = 0;

void *thread_func_sprite(void *data)
{
  ThreadInfo_sprite *ti = (ThreadInfo_sprite*)data;
  for(int y=ti->start_y;y<ti->end_y;y++)
    for(int x=0;x<ti->sx;x++)
	{
	  ti->ref[ti->num].buffer[x+y*ti->ref[ti->num].ydelta] = ti->s->Pixel(ti->num, x, y).Pixel();
	}
#if 0
  g_sprite_count++;
    std::string s = "bitmap#";
    std::stringstream ss;
    ss << g_sprite_count2;
    s+=ss.str();

  ProgressBar(434, g_sprite_count, 4, s);
#endif

  //pthread_exit(NULL);

  return 0;
}
std::string MB(long i);

class SpriteTexture : public MeshTextures
{
public:
  SpriteTexture(const Sprite &s, bool progress) : s(s), progress(progress) 
  { 
    int ss = NumTextures();
    ref = new BufferRef[ss];
    for(int i=0;i<ss;i++)
      {
	ref[i] = BufferRef::NewBuffer(1,1); 
      }
    currentnum = -1; 
  }
  ~SpriteTexture() {
    for(int i=0;i<NumTextures();i++)
      {
	delete [] ref[i].buffer;
      }
    delete [] ref; }
  int NumTextures() const { return s.NumFrames(); }
  void GenTexture(int num)
  {
    BufferRef::FreeBuffer(ref[num]);
    int sx = s.XSize(num);
    int sy = s.YSize(num);
    // std::cout << "SpriteTexture::GenTexture(" << sx << "," << sy << ")" << std::endl;
    if (sx<1 ||sy<1) { /*std::cout << "ERROR: SpriteTexture::GenTexture, too small bitmap ("<< sx << "," << sy << ")" << std::endl;*/ sx=1; sy=1; }
    if (sx>10000 || sy>10000) { /*std::cout << "ERROR: SpriteTexture::GenTexture, too large bitmap ("<< sx << "," << sy << ")" << std::endl;*/ sx=1; sy=1; }
    ref[num] = BufferRef::NewBuffer(sx, sy);
    //std::cout << "SpriteTexture:" << sx << "x" << sy << "=" << MB(sx*sy*sizeof(unsigned int)) << std::endl;
#ifdef THREADS
    int numthreads = 4;
    g_sprite_count=0;
    //if (progress)
    //  InstallProgress(434, "bitmap", 15);
    std::vector<ThreadInfo_sprite*> vec;
    int dsy = sy/numthreads + 1;
    for(int t=0;t<numthreads;t++) {
      ThreadInfo_sprite *info = new ThreadInfo_sprite;
      info->s = &s;
      info->ref = ref;
      info->start_y = t*dsy;
      info->end_y = (t+1)*dsy;
      if (info->start_y>sy) { info->start_y=sy; }
      if (info->end_y>sy) { info->end_y = sy; }
      if (info->end_y-info->start_y<1) { delete info; continue; }
      info->sx = sx;
      info->num = num;
      vec.push_back(info);
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setstacksize(&attr, 300000);
      pthread_create(&info->thread_id, &attr, &thread_func_sprite, (void*)info);
      pthread_attr_destroy(&attr);
    }
    int s = vec.size();
    for(int t=0;t<s;t++) {
      ThreadInfo_sprite *info = vec[t];
      void *res;
      pthread_join(info->thread_id, &res);
      //if (progress)
      //ProgressBar(434, t*15/s, 15, "bitmap");
      delete info;
    }
    //if (progress)
    //  ProgressBar(434, 15, 15, "bitmap");

#else
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
	{
	  ref[num].buffer[x+y*ref[num].ydelta] = s.Pixel(num, x, y).Pixel();
	}
#endif
  }
  BufferRef TextureBuf(int num) const 
  {
    return ref[num]; 
  }
private:
  const Sprite &s;
  mutable BufferRef *ref;
  mutable int currentnum;
  bool progress;
};

void TexturePrepare(const Sprite &s, ArrayRender &rend, bool progress)
{
  int frames = s.NumFrames();
  if (!frames) { std::cout << "TexturePrepare !frames" << std::endl; return; }
  rend.AllocTexture(frames);
  rend.Alloc(2,6, frames,1,1,frames);
  SpriteTexture ss(s,progress);
  for(int i=0;i<frames;i++)
    {
      rend.UpdateTexture(ss, i);
    }  
  rend.Prepare();

}
void TextureEnable(ArrayRender &rend, int frame, bool enable)
{
  if (&rend) {
  if (enable)
    rend.EnableTexture(frame);
  else
    rend.DisableTexture();
  }
}
void PrepareSprite(const Sprite &s, ArrayRender &rend)
{
  int frames = s.NumFrames();
  if (!frames) { std::cout << "PrepareSprite !frames" << std::endl; return; }
  rend.AllocTexture(frames);
  rend.Alloc(2, 6, frames, 1, 1, frames);
  SpriteTexture ss(s,true);
  for(int i=0;i<frames;i++)
    {
      rend.UpdateTexture(ss, i);
    }
  SpriteMesh spmesh(s);
  SpriteTexCoords texcoords(s);

  MeshToTriangles trimesh(spmesh);
  MeshTexCoordsToTriangles tricoords(texcoords, true);

  for(int i2=0;i2<frames;i2++)
    {
      std::pair<int,int> p = rend.InsertMesh(trimesh, i2);
      rend.UpdateTexCoord(trimesh, tricoords, p, i2);
    }
   rend.Prepare();
 
}

void PrepareSpriteToVA(const Sprite &s, VertexArraySet &vas)
{
  SpriteMesh spmesh(s);
  SpriteTexCoords texcoords(s);
  
  MeshToTriangles trimesh(spmesh);
  MeshTexCoordsToTriangles tricoords(texcoords, true);
  
  MeshFaceCollection coll(trimesh, tricoords ,0);
  FaceCollectionVertexArray2 va(coll, vas);
  va.reserve(0);
  va.copy(0,coll.NumFaces());
}

void RenderSprite(const Sprite &s, int frame, Point2d pos, float z, ArrayRender &rend, Program *prog)
{
  //std::cout << "SpriteFrame: " << frame << std::endl;
  rend.EnableTexture(frame);
  //glPushMatrix();
  Point2d p = s.Pos(frame);

  Matrix m = Matrix::Translate(pos.x+p.x, pos.y+p.y, z);
  //float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
  //		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
  //		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
  //		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  //
  prog->set_var("in_MV", m);
  //prog->use();
  //glMultMatrixf(&mat[0]);
  rend.Render(frame, -1, -1, frame, 0, rend.used_vertex_count[0]);
  //prog->unuse();
  //glPopMatrix();
  rend.DisableTexture();
}

void RenderSprite(const Sprite &s, int frame, Point2d pos, float z, ArrayRender &rend, float mult_x, float mult_y, Program *prog)
{
  //std::cout << "SpriteFrame: " << frame << std::endl;
  rend.EnableTexture(frame);
  //glPushMatrix();
  Point2d p = s.Pos(frame);

  Matrix m = Matrix::Translate(pos.x+p.x, pos.y+p.y, z);
  //float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
  //		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
  //		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
  //		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  //
  //glMultMatrixf(&mat[0]);
  //glScalef(mult_x, mult_y, 1.0);
  m = Matrix::Scale(mult_x, mult_y, 1.0) * m;
  //prog->use();
  prog->set_var("in_MV", m);
  rend.Render(frame, -1, -1, frame, 0, rend.used_vertex_count[0]);
  //prog->unuse();
  //glPopMatrix();
  rend.DisableTexture();
}


#if 0
void RenderSprite(const Sprite &s, int frame, Point2d pos1, Point2d pos2, Point2d pos1_inside, Point2d pos2_inside, float z, ArrayRender &rend)
{
  OpenglLowApi *ogl = g_low->ogl;
  //std::cout << "SpriteFrame: " << frame << std::endl;
  rend.EnableTexture(frame);
  ogl->glPushMatrix();
  Point2d p = s.Pos(frame);

  Matrix me = Matrix::Translate(pos1_inside.x+p.x, pos1_inside.y+p.y, z);
  float xx = pos2_inside.x-pos1_inside.x;
  float yy = pos2_inside.y-pos1_inside.y;
  Matrix mr = Matrix::RotateZ(atan2(yy,xx));
  float d = sqrt(xx*xx+yy*yy);
  Matrix ms = Matrix::Scale(1.0/d,1.0/d,1.0/d);
  Matrix ma = me*mr*ms;
  Matrix mi = Matrix::Inverse(ma);

  Matrix wm = Matrix::Translate(pos1.x+p.x, pos1.y+p.y, z);
  float wxx = pos2.x-pos1.x;
  float wyy = pos2.y-pos1.y;
  Matrix wmr = Matrix::RotateZ(atan2(wyy,wxx));
  float wd = sqrt(wxx*wxx+wyy*wyy);
  Matrix wms = Matrix::Scale(1.0/wd,1.0/wd,1.0/wd);
  Matrix wma = wm*wmr*wms;

  Matrix m = mi * wma;
			    
  float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		    m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		    m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		    m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
  
#ifndef EMSCRIPTEN
  ogl->glMultMatrixf(&mat[0]);
#endif
  rend.Render(frame, -1, -1, frame, 0, rend.used_vertex_count[0]);
  ogl->glPopMatrix();
  rend.DisableTexture();
}
#endif

#if 0
float Parser2::ParseFloat(std::string s_, bool &success)
{
  std::cout << "ParseFloat: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  std::stringstream ss(s);
  float val;
  if (ss >> val) success = true;
  else { std::cout << "ParseFloat fail" << std::endl; success = false; }
  return val;
}
int Parser2::ParseInt(std::string s_, bool &success)
{
  std::cout << "ParseInt: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  std::stringstream ss(s);
  int val = 0;
  if (ss >> val) success = true;
  else { std::cout << "ParseInt fail" << std::endl; success = false; }
  return val;
}

bool Parser2::ParseBool(std::string s_, bool &success)
{
  std::cout << "ParseBool: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  bool val = false;
  if (s=="true") val = true;
  else if (s=="false") val = false;
  else { std::cout << "ParseBool fail" << std::endl; success = false; return val; }
  success = true;
  return val;
}



Point2d Parser2::ParsePoint2d(std::string s_, bool &success)
{
  std::cout << "ParsePoint2d: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);

  if (s.length()<2) { std::cout << "ParsePoint2d: length<2" << std::endl; success=false; return Point2d(); }
  if (s[0]!='(' || s[s.length()-1]!=')') { std::cout << "ParsePoint2d () fail" << std::endl; success = false; return Point2d(); }
  std::string s2 = s.substr(1, s.length()-2);
  VectorArray<float> *array = ParseArray2<float>(s2, success);
  if (!success) { std::cout << "ParsePoint2d: ParseArray fail" << std::endl; return Point2d(); }
  if (array->Size()!=2) { std::cout << "ParsePoint2d: dimensions!=2" << std::endl; success = false; return Point2d(); }
  Point2d p;
  p.x = array->Index(0);
  p.y = array->Index(1);
  return p;
}

int Parser2::ParseEnum(std::string *strings, int size,
		      std::string s, bool &success)
{
  for(int i=0;i<size;i++)
    {
      if (s==strings[i]) { success = true; return i; }
    }
  success = false;
  return -1;
}

Point Parser2::ParsePoint(std::string s_, bool &success)
{
  std::cout << "ParsePoint: '" << s_ << "'" << std::endl;

  std::string s = WhiteSpace(s_);
  if (s.length()<2) { std::cout << "ParsePoint: length<2" << std::endl; success=false; return Point(); }
  if (s[0]!='(' || s[s.length()-1]!=')') { std::cout << "ParsePoint () fail" << std::endl; success = false; return Point(); }
  std::string s2 = s.substr(1, s.length()-2);
  VectorArray<float> *array = ParseArray2<float>(s2, success);
  if (!success) { std::cout << "ParsePoint array fail" << std::endl; return Point(); }
  if (array->Size()!=3) { std::cout << "ParsePoint: dimensions!=3" << std::endl; success = false; return Point(); }
  Point p;
  p.x = array->Index(0);
  p.y = array->Index(1);
  p.z = array->Index(2);
  return p;
}
#endif

std::string WhiteSpace(std::string s)
{
  //std::cout << "WhiteSpace: " << s << std::endl;
  int size = s.length();
  int i;
  for(i=0;i<size;i++)
    {
      if (s[i]!=' '&&s[i]!='\n'&&s[i]!='\t') break;
    }
  int k;
  for(k=size-1;k>=0;k--)
    {
      if (s[k]!=' '&&s[k]!='\n'&&s[k]!='\t') { k++; break; }
    }
  if (k<=i) return std::string("");
  std::string sk = s.substr(i,s.length()-i-(s.length()-k));
  //std::cout << "WhiteSpaceRes: " << i << " " << k << " " << sk << std::endl;
  return sk;
}

void DumpMem(const char *c, int size)
{
#if 0
  for(int i=0;i<size;i++)
    {
      std::cout << std::hex << (int)(c[i]) << std::dec;
    }
  std::cout << std::endl;
#endif
}

#if 0
void FontParseTestCases()
{
  std::ifstream str("./fonts/font.txt");
  std::string s;
  char c; while(str>>c) s+=c;
  std::cout << s;
  std::cout << "Parsing starts:" << std::endl;
  FontCharacterImpl impl(s);
  int count = impl.NumLoops();
  std::cout << "NumLoops: " << count << std::endl; 
  for(int l=0;l<count;l++)
    {
      int points = impl.NumPoints(l);
      std::cout << "NumPoints: " << points << std::endl; 
      std::cout << "{" << std::endl;
      for(int p=0;p<points;p++)
	{
	  Point2d point = impl.LoopPoint(l, p);
	  std::cout << "(" << point.x << "," << point.y << "), ";
	}
      std::cout << "}" << std::endl;
    }
}
#endif

void Loop::visit(Visitor &v)
{
    v.visit(inverse);
    v.visit(points);
}

#if 0
TypeTraits<int>::fptrtype TypeTraits<int>::fptr = &Parser2::ParseInt;
TypeTraits<bool>::fptrtype TypeTraits<bool>::fptr = &Parser2::ParseBool;

TypeTraits<float>::fptrtype TypeTraits<float>::fptr = &Parser2::ParseFloat;

TypeTraits<Point2d>::fptrtype TypeTraits<Point2d>::fptr = &Parser2::ParsePoint2d;
TypeTraits<Loop>::fptrtype TypeTraits<Loop>::fptr = &Parser2::ParseLoop;

#endif

void Write(Parser2 *p, char *c, int size, int type, std::string s, bool &success)
{
#if 0
  std::cout << "Write: '" << s << "'" << type << std::endl;

  switch(type)
    {
    case 0: 
      {
      int i = (p->* (TypeTraits<int>::fptr))(s, success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 1:
      {
      float i = p->ParseFloat(s,success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 2:
      {
      Point2d i = p->ParsePoint2d(s,success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 3:
      {
      Point i = p->ParsePoint(s,success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 4:
      {
      VectorArray<Loop> *i = p->ParseArray<Loop>(s,success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 5:
      {
      VectorArray<Point2d> *i = p->ParseArray<Point2d>(s,success);
      std::copy((char*)&i, ((char*)&i)+size, c);
      break;
      }
    case 6:
      {
	bool i = (p->* (TypeTraits<bool>::fptr))(s, success);
	std::copy((char*)&i, ((char*)&i)+size, c);
	break;
      }
    default:
      std::cout << "Write type fail" << std::endl;
      success = false;
      return;
    };
  std::cout << "Write result: ";
  DumpMem(c,size);
  success = true;
#endif
}

int Find(std::string s, char c)
{
  int ss = s.length();
  int level = 0;
  for(int i=0;i<ss;i++)
    {
      if (!level && s[i]==c) return i;
      if (s[i]=='{'||s[i]=='(') level++;
      if (s[i]=='}'||s[i]==')') level--;
    }
  return -1;
}
#if 0
Path3d ParsePath3d(std::string s, bool &success)
{
  Path3d result;
  std::vector<float> stack;
  Point position(0.0,0.0,0.0);
  success = true;
  if (s.length()==0) { Path3d p; return p; }
  int pos = 0;
  while(1)
    {
      if (pos>=s.size()) break;
      char c = s[pos];
      switch(c)
	{
	case '(':
	  for(int i=pos+1;i<s.size();i++)
	    {
	      char cc = s[i];
	      if (cc==')') break;
	    }
	  std::string ss = s.substr(1,i-1);
	  position = ParsePoint(ss, success);
	  if (!success) { Path3d p; return p; }
	  pos = i+1;
	  break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  {
	  for(int i=pos;i<s.size();i++)
	    {
	      char c = s[i];
	      if (c<'0' || c>'9') break;
	    }
	  std::string s = s.substr(pos,i-pos);
	  float val = ParseFloat(s, success);
	  if (!success) { Path3d p; return p; }
	  stack.push_back(val);
	  pos = i;
	  break;
	  }
	case 'u': // up
	case 'd': // down
	case 'l': // left
	case 'r': // right
	case 'f': // forward
	case 'b': // backward
	  {
	    float mx = 0.0;
	    float my = 0.0;
	    float mz = 0.0;
	    if (c=='u') my=-1.0;
	    if (c=='d') my=1.0;
	    if (c=='l') mx=-1.0;
	    if (c=='r') mx=1.0;
	    if (c=='f') mz=1.0;
	    if (c=='b') mz=-1.0;
	    float val = stack.top();
	    stack.pop_back();
	    position.x += mx*val;
	    position.y += my*val;
	    position.z += mz*val;
	    pos++;
	    break;
	  }
	case 'x':
	  {
	    result.vec.push_back(position);
	    pos++;
	    break;
	  }
	default:
	  {
	  success=false;
	  return result;
	  }
	};
    }
  return result;
      
}
Path2d ParsePath2d(std::string s, bool &success)
{
}



FBO::FBO(int sx, int sy) :sx(sx), sy(sy) { 
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glGenFrameBuffers(1, ids); 
  ogl->glGenRenderBuffers(1, rbo);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, rbo[0]);
  ogl->glRenderbufferStorage(Low_GL_RENDERBUFFER, Low_GL_RGBA,
			sx,sy);
  ogl->glBindRenderbuffer(Low_GL_RENDERBUFFER, 0);
  
  ref = BufferRef::NewBuffer(sx,sy);
}
FBO::~FBO() { 
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glDeleteRenderBuffers(1,rbo);
  ogl->glDeleteFrameBuffers(1, ids); 
  FreeBuffer(ref);
}
void FBO::bind()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glBindFrameBuffer(Low_GL_FRAMEBUFFER, ids[0]);
  ogl->glFramebufferRenderbuffer(Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0,
			    Low_GL_RENDERBUFFER, rbo);
}
void FBO::unbind()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glBindFrameBuffer(Low_GL_FRAMEBUFFER, 0);
  ogl->glFramebufferRenderbuffer(Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0,
			    Low_GL_RENDERBUFFER, 0);
}
void FBO::update()
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glReadPixels(0,0,sx,sy, Low_G_RGBA, Low_GL_UNSIGNED_INT_8_8_8_8, (void*)ref.buffer);
}


#endif
