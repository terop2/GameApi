#include "Game.hh"
#include "Widgets.hh"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL/SDL_opengl.h>


void GameEffect::Init()
{
  FrameAnimPlugins::Init();
  SphereElem sphere(20,20);
  MatrixElem me(sphere, Matrix::Scale(30.0,30.0,30.0));
  FaceCollectionPoly poly(2,&me);

  TunnelWall wall(0, "./textures/texture_carpet2.jpeg", &plugins);
  TunnelWall wall2(1, "./textures/texture_carpet3.jpeg", &plugins);

  int sx = 15;
  int sy = 15;
  float blockx = 1024*2/sx;
  float blocky = 768*2/sy;
  float sizex = blockx*sx;
  float sizey = blocky*sy;

  CubeElem cube;
  FaceCollectionPoly cubep(3, &cube);
  ConstantBitmap<Type> bm(3, sx,sy);
  GridPoly gridp(3, bm, cubep, Point(-sizex/2.0,-sizey/2.0,0.0), Vector(blockx,0.0,0.0), Vector(0.0,blocky,0.0), (blockx+blocky)/2.0/2.0);
  


  VectorArray<TypeToPoly*> vec;
  vec.push_back(&poly); // 0
  vec.push_back(&wall); // 1
  vec.push_back(&wall2); // 2
  vec.push_back(&gridp); // 3
  TypeToPolyArrayConvert conv(vec);
  TypeToPolyArrayFunction func(conv);
  UpdateVBOTypeToPoly(func, vbostate, UpdateAllTexture);
}
void GameEffect::PreFrame(float time)
{
  FrameAnimPlugins::PreFrame(time);
  glTranslatef(0.0, 0.0, -400.0);
}
bool GameEffect::Frame(float time)
{

  {
  Matrix mm = Matrix::Translate(p.p.x, -p.p.y, 0);
  ScopeMatrix m(mm);
  DrawVBO(0, vbostate);
  }


  float speed = 80.0;
  int count = time / (1024*2/speed);

  float delta = count * 1024*2;

  {
    Matrix mkmm = Matrix::Translate(-p.p.x, p.p.y, time*speed - delta);
    ScopeMatrix mk(mkmm);


    //glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    DrawVBO(1,vbostate, UpdateAllTexture);
    {
      Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
      ScopeMatrix m(mm);
      DrawVBO(2,vbostate, UpdateAllTexture);
      DrawVBO(3, vbostate);

      {
	Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
	ScopeMatrix m(mm);
	DrawVBO(2,vbostate, UpdateAllTexture);
	{
	  Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
	  ScopeMatrix m(mm);
	  DrawVBO(2,vbostate, UpdateAllTexture);
	{
	  Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
	  ScopeMatrix m(mm);
	  DrawVBO(2,vbostate, UpdateAllTexture);
	{
	  Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
	  ScopeMatrix m(mm);
	  DrawVBO(2,vbostate, UpdateAllTexture);
	{
	  Matrix mm = Matrix::Translate(0.0, 0.0, -1024*2);
	  ScopeMatrix m(mm);
	  DrawVBO(2,vbostate, UpdateAllTexture);
	}

	}

	}

	}
      }

    }
    glDisable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHTING);
  }
  return false;
}

void TunnelWall::Init()
{
  BufferRef buf1 = LoadImage(filename);
  BitmapFromBuffer buf2(buf1);
  ContinuousBitmapFromBitmap<Color> buf3(buf2, 1.0, 1.0);
  BitmapFromContinuousBitmap<Color> buf4(buf3, 512, 512);
  BufferFromBitmap buf5(buf4);
  buf5.Gen();
  BufferRef buf = buf5.Buffer();
  TexturePlugin *plugin2 = new TexturePlugin(buf);
  plugin->push_back(plugin2);
  plugin2->Init();
  r = new RectangleElem(1024*2,1024*2);
  coords = new SimpleTexCoords(*r, plugin2->Texture());
  te = new TextureElem(*r, *coords);
  
  left = new MatrixElem(*te, Matrix::YRotation(3.14159/2.0)*Matrix::Translate(-1024,0.0,0.0));
  right = new MatrixElem(*te, Matrix::YRotation(-3.14159/2.0)*Matrix::Translate(1024, 0.0,0.0));
  top = new MatrixElem(*te, Matrix::XRotation(3.14159/2.0)*Matrix::Translate(0.0,-768,0.0));
  bottom = new MatrixElem(*te, Matrix::XRotation(-3.14159/2.0)*Matrix::Translate(0.0,768, 0.0));
  vec = new VectorArray<const FaceCollection*>;
  vec->push_back(top);
  vec->push_back(bottom);
  vec->push_back(left);
  vec->push_back(right);
  convert = new FaceCollectionArrayConvert(*vec);
  compress=new CompressObject(*convert);
  boxable = new BoxableFaceCollectionConvert(*compress);
}
void UpdateVBOTypeToPoly(const TypeToPoly &tp, VBOState &vbostate, VBOUpdate update)
{
  std::vector<FaceCollectionTri*> vec;
  int s = tp.NumTypes();
  for(int t=0;t<s;t++)
    {
      Type tt = tp.TypeIndex(t);
      const FaceCollection *faces = tp.Index(tt);
      std::cout << "TypeToPoly: " << faces->NumFaces() << std::endl;
      //UpdateVBO(*faces, vbostate, update);
      QuadsToTris *q = new QuadsToTris(*faces);
      vec.push_back(q);
    }
  ObjectArray oa(vec.begin(), vec.end());
  VBO vbo(oa);
  UpdateVBO(vbo, vbostate, update);
}
