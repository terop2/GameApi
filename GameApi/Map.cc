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



#include "Map.hh"
#include <iostream>
#include <string>
#include <fstream>
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "Pieces.hh"
#include "Effect.hh"



void error(std::string s) {
  std::cout << s << std::endl;
  exit(0);
}

void Map::Load(std::string filename)
{
  std::ifstream file(filename.c_str());
  file >> dimx >> dimy;
  for(int y=0;y<dimy;y++)
    {
      for(int x=0;x<dimx;x++)
	{
	  char c=0;
	  if (!(file >> c)) error("File format");
	  type.push_back((int)(c-'a'));
	}
    }
}
void Map::Render(float time)
{
#if 0
  glTranslatef(-100.0*dimy/2,0.0,-100.0*dimx/2);
  for(int y=0;y<dimy;y++)
    {
      for(int x=0;x<dimx;x++)
	{
	  //Color c = *colors[type[x+y*dimx]];
	  //Color c2 = white;
	  //Color cc = TransformColor(c, c2, 0.0, 100.0, time);
	  Color2 cc = AnimColor(red_green, red_green_size,
			       time);
	  glNormal3f( 0.0, 1.0, 0.0 );
	  PlainPiece(zero, cc);
	  glTranslatef(0.0, 0.0, 100.0);
	}
      glTranslatef(100.0, 0.0, 0.0);
      glTranslatef(0.0,0.0,-dimx*100.0);
    }
#endif
#if 0
  for(int x=0;x<5;x++)
    for(int y=0;y<5;y++)
      for(int z=0;z<5;z++) 
	{
	  Point p =  Point(-x*150.0, y*160.0, z*150.0);
	  Cube(p, unit_dir, white, size100);
	}
#endif
  glColor3f(1.0,0.0,0.0);
  

}
