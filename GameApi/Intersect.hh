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


#ifndef INTERSECT_HH
#define INTERSECT_HH

#include "VectorTools.hh"
#include "Effect.hh"
#include <vector>

struct LinePlaneIntersection
{
  Vector tuv; // t is line coords, u,v is plane coords
};

Point IntersectionPoint(LinePlaneIntersection &c, Point line_p1, Point line_p2);

Vector IntersectionNormal(LinePlaneIntersection &c, Vector line_n1, Vector line_n2);

bool InsideLine(LinePlaneIntersection &c);

// Line equation line_p1+(line_p2-line_p1)*t
// Plane equation plane_p1+(plane_p2-plane_p1)*u+(plane_p3-plane_p1)*v

LinePlaneIntersection LinePlaneIntersectionFunc(Point line_p1, Point line_p2,
						Point plane_p1, Point plane_p2, Point plane_p3);

class FaceCollection;

void SplitPolygon(std::vector<Point> &result_points, std::vector<Vector> &result_normals, FaceCollection &c1, int face1,
		  Point plane_point1, Point plane_point2, Point plane_point3, bool start);


#endif
