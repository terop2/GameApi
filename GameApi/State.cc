
#include "State.hh"

#define NO_SDL_GLEXT
//#include <GL/glew.h>
//#include <SDL_opengl.h>
//#include <GL/glut.h>

#include "GameApi_low.hh"

void StateBitmaps::Render(int framenum)
  {
    int sy = bitmap->SizeY();
    //StateColumn &col = bitmap->DynColumn(framenum);
    //Matrix m = col.m;
#if 0
    float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		      m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		      m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		      m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
#endif
    g_low->ogl->glPushMatrix();
#ifndef EMSCRIPTEN
    g_low->ogl->glTranslatef(0.0,0.0,0.0);
#endif
    //glMultMatrixf(&mat[0]);
    //bool b = tex->NumTextures() != 0;
    for(int y=0;y<sy;y++)
      {
	//glActiveTexture(GL_TEXTURE0+y);
	StateRow &row = bitmap->DynRow(y);
	//std::cout << "Render: " << y << " " << framenum << std::endl;
	if (row.texture)
	  {
	    row.rend->EnableTexture(0);
	  }
	row.rend->Render(framenum, framenum, framenum, framenum, 0, row.rend->used_vertex_count[framenum]);
	if (row.texture)
	  {
	    row.rend->DisableTexture();
	  }
      }
    g_low->ogl->glPopMatrix();
  }
