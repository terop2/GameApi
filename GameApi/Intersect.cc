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



#include "Intersect.hh"
#include "Effect.hh"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL/SDL_opengl.h>

Point IntersectionPoint(LinePlaneIntersection &c, Point line_p1, Point line_p2)
{
  //Point p = line_p1+Vector(line_p2-line_p1)*(c.tuv.dx);
  Point p = Vector(line_p1)*(1.0-c.tuv.dx)+ Vector(line_p2)*(c.tuv.dx);
  //std::cout << "Intersection: " << line_p1 << p << line_p2 << std::endl;
  return p;
}

Vector IntersectionNormal(LinePlaneIntersection &c, Vector line_n1, Vector line_n2)
{
  return line_n1*c.tuv.dx+line_n2*(1.0-c.tuv.dx);
}


LinePlaneIntersection LinePlaneIntersectionFunc(Point line_p1, Point line_p2,
			    Point plane_p1, Point plane_p2, Point plane_p3)
{
  Matrix m = { { line_p1.x-line_p2.x, plane_p2.x-plane_p1.x, plane_p3.x-plane_p1.x, 0.0,
	       line_p1.y-line_p2.y, plane_p2.y-plane_p1.y, plane_p3.y-plane_p1.y, 0.0,
	       line_p1.z-line_p2.z, plane_p2.z-plane_p1.z, plane_p3.z-plane_p1.z, 0.0,
	       0.0,                 0.0,                   0.0,                   1.0 } };
  
  //std::cout << "LinePlaneIntersectionFunc" << m << std::endl;
  Matrix mi = Matrix::Inverse(m);
  Vector v;
  v.dx = line_p1.x - plane_p1.x;
  v.dy = line_p1.y - plane_p1.y;
  v.dz = line_p1.z - plane_p1.z;
  Vector tuv = v*mi;
  //tuv.Normalize();
  LinePlaneIntersection sec;
  //tuv.dx = 0.5; 
  sec.tuv = tuv;
  //std::cout << tuv.dx << std::endl;
  return sec;

}

bool InsideLine(LinePlaneIntersection &c)
{
  if (isnan(c.tuv.dx)) return false;
  return c.tuv.dx>=0.0 && c.tuv.dx<=1.0;
}

void SplitPolygon(std::vector<Point> &result_points, std::vector<Vector> &result_normals, FaceCollection &c1, int face1,
			   Point plane_point1, Point plane_point2, Point plane_point3, bool start)
{
  std::vector<Point> &vec = result_points;
  std::vector<Vector> &normal = result_normals;
  int count=c1.NumPoints(face1);
  Point p = c1.FacePoint(face1,0); 
  Point pn = c1.PointNormal(face1, 0);
  int draw = start?1:0;
  //std::cout << "Start poly " << std::endl;
  
  if (draw==1) 
    { 
      vec.push_back(p); 
      normal.push_back(pn); 
      //std::cout << p << std::endl; 
    }

  Point store;
  for(int i=1;i<count+1;i++)
    {
      int ii = i % count;
      Point pp = c1.FacePoint(face1, ii);
      Vector ppn = c1.PointNormal(face1, ii);
      //std::cout << "isect: " << p << pp << plane_point1 << plane_point2 << plane_point3 << std::endl;

      LinePlaneIntersection c = LinePlaneIntersectionFunc(p,pp, plane_point1, plane_point2, plane_point3);
      if (InsideLine(c))
	{
	  Point interp = IntersectionPoint(c, p,pp);
	  Vector interpn = IntersectionNormal(c, pn, ppn);

	  //Point pa = interp;
	  //Point pb = interp+interpn;
	  //glBegin(GL_LINES);
	  //glVertex3f(pa.x, pa.y, pa.z);
	  //glVertex3f(pb.x, pb.y, pb.z);
	  //glEnd();


	vec.push_back(interp);
	normal.push_back(interpn);
	//std::cout << "i " << interp << std::endl;
	  store = interp;
	  if (draw==0) 
	    { 
	      vec.push_back(pp); 
	      normal.push_back(ppn); 
	      //std::cout << pp << std::endl;
	    }
	  draw^=1;
	}
      else
	{
	  if (draw==1) 
	    { 
	      vec.push_back(pp); 
	      normal.push_back(ppn); 
	      //std::cout << pp << std::endl;
	    }
	}
      p = pp;
      pn = ppn;
    }
  if (draw==1)
    {
      vec.pop_back();
      normal.pop_back();
    }
}
