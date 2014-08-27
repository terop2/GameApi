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

#include "Event.hh"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL_opengl.h>

void DrawVBO(TimedMatrixPaths &paths, float time, VBOState &vbostate, VBOUpdate u)
{
  int size = paths.Size();
  //std::cout << "Size: " << size << std::endl;
  float t = time;
  while(t > paths.Length())
    t -= paths.Length();
  for(int i=0;i<size;i++)
    {
      Matrix m = paths.Index(t, i);
      glPushMatrix();
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
      glMultMatrixf(&mat[0]);
      DrawVBO(vbostate, u);
      glPopMatrix();      
    }
}
void DrawVBO(Array<int, PointCollection*> &arr, FloatArray &t, float time, VBOState &vbostate, VBOUpdate u, PieceShader &shader)
{
  int size = t.Size();
  float last_time = t.Index(t.Size()-1);
  while(time > last_time)
    time -= last_time;
  int ii;
  float start_time = time-1.0;
  float end_time = time+1.0;
  for(ii=0;ii<size-1;ii++)
    {
      start_time = t.Index(ii);
      end_time = t.Index(ii+1);
      //std::cout << start_time << " " << end_time << " " << time << std::endl;
      if (time >=start_time && time <= end_time) break;
    }
  //std::cout << ii << std::endl;
  float timedist = (end_time-start_time);
  float tt = time / timedist;
  start_time /= timedist;
  end_time /= timedist;

  int startpos_id = shader.get_loc("startpos");
  int endpos_id = shader.get_loc("endpos");
  int time_id = shader.get_loc("time");

  int count = arr.Index(0)->Size();
  for(int i=0;i<count;i++)
    {
      PointCollection *start = arr.Index(ii);
      Point p1 = start->Index(i);
      PointCollection *end = arr.Index(ii+1);
      Point p2 = end->Index(i);
      LinearInterpolationParameters p(startpos_id, endpos_id, time_id, p1, p2, start_time, end_time, tt);
      shader.set_params(p);
      DrawVBO(vbostate, u);
    }
}
