
#include "ShaderExpr.hh"

int ShaderExpr::unique=0;

#if 0
void testshader()
{
  float val;
  CubeElem cube;
  PointAttribute pa(&cube, AttrCenterX, AttrCenterY, AttrCenterZ, "center");
  Vertex v(&cube);
  MixPoint mix(pa, v, "slide", val);

  std::cout << "Context: " << mix.Context() << "@" << std::endl;
  for(int x=0;x<cube.NumFaces();x++)
    {
      for(int y=0;y<cube.NumPoints(x);y++)
	{
	  ChooseVertex v;
	  v.face = x;
	  v.point = y;
	  mix.SetVertex(v);
	  std::cout << "Value: " << mix.Value() << "@" << std::endl;
	}
    }
  std::cout << "Expr: " << mix.Expr("gl_Position.xyz") << "@" << std::endl;
}
#endif
