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


#ifndef PIECES_HH
#define PIECES_HH

#include "VectorTools.hh"

struct Heights
{
  float tl;
  float tr;
  float bl;
  float br;
};
struct Color2
{
  float r,g,b,t;
};
struct TimedColor
{
  float time;
  const Color2 *c;
};
struct Dir
{
  Vector u_x, u_y, u_z;
};
struct Size2
{
  float x,y,z;
};

static const Heights zero = { 0.0, 0.0, 0.0, 0.0 };
static const Color2 red = { 1.0, 0.0, 0.0, 1.0 };
static const Color2 green = { 0.0, 1.0, 0.0, 1.0 };
static const Color2 blue = { 0.0, 0.0, 1.0, 1.0 };
static const Color2 white = { 1.0, 1.0, 1.0, 1.0 };

//static const Color *colors[] = { &red, &green, &blue, &white }; 

static const TimedColor red_green[] =
  {
    { 0.0, &red },
    { 100.0, &green },
    { 200.0, &red }
  };
static const int red_green_size = sizeof(red_green) / sizeof(TimedColor);
//static const Dir unit_dir =
//  {
//    Vector(1.0, 0.0, 0.0),
//    Vector(0.0, 1.0, 0.0),
//    Vector(0.0, 0.0, 1.0)
//  };
static const Size2 size100 =
  {
    100.0, 100.0, 100.0
  };
void PlainPiece(const Heights &h, const Color2 &c);
Color2 TransformColor(const Color2 &c1, const Color2 &c2,
		    float starttime, float endtime, float time);
Color2 AnimColor(const TimedColor *color, int count,  float time);
void Cube(const Point &center, const Dir &d, const Color2 &c, const Size2 &s);
Point TransformPoint(const Point &p1, const Point &p2,
		     float starttime, float endtime, float time);

Vector TransformVector(const Vector &p1, const Vector &p2,
		     float starttime, float endtime, float time);

float TransformFloat(const float &f1, const float &f2,
		     float starttime, float endtime, float time);


#endif
