
#include "Triangle.hh"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

void TriangulateEffect::Init()
{
  CubeElem cube;
  cube.SetBox(Matrix::Identity());
  MatrixElem m(cube, Matrix::Scale(100.0,100.0,100.0));
  Triangulate tri(m);
  tri.Gen();
  FaceCollectionFromTriArray faces(tri);
  UpdateVBO(faces, vbostate);
}
bool TriangulateEffect::Frame(float time) 
{
  DrawVBO(vbostate);
  return false;
}

void DecodahedronEffect::Init()
{
}
void DecodahedronEffect::PreFrame(float fr)
{
#ifndef EMSCRIPTEN

  glTranslatef(0.0, 0.0, -500.0);
  glTranslatef(0.0, -100.0, 0.0);

#endif
}
bool DecodahedronEffect::Frame(float time)
{
#if 0
  //StarCurve star(100.0, 200.0, 5);
  Dodecahedron d(200.0);
  PointCollection *coll = &d;
  //FIX RenderOpenGl(*coll);

  ChoosePointByMouseCursor choose(*coll);
  Point pp2 = coll->Index(choose.Choose(pp));
  //std::cout << pp.x << "," << pp.y << " " << pp2 << std::endl;
  SphereElem s(10,10);
  MatrixElem me(s, Matrix::Scale(10.0,10.0,10.0)*Matrix::Translate(pp2.x,pp2.y,pp2.z));
  RenderOpenGl(me);
#endif
  return false;
}
