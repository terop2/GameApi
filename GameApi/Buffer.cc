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
// You should have received a copy of the Library GNU General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//


#include "Buffer.hh"
#include <iostream>

//void stackTrace();
BufferRef BufferRef::NewBuffer(int width, int height)
{
  if (width<0) width=0;
  if (height<0) height=0;
  if (width==0) { width++; }
  if (height==0) {height++; }
  unsigned int *buffer = new unsigned int[width*height];
  // std::cout << "BufferRef::NewBuffer(" << width << "*" << height << ")" << std::endl;
  //stackTrace();
  if (!buffer)
    {
      std::cout << "BufferRef::NewBuffer failed" << std::endl;
    }
  BufferRef ref;
  ref.buffer = buffer;
  ref.width = width;
  ref.height = height;
  ref.ydelta = width;
  return ref;
}
BufferRef BufferRef::SubBuffer(BufferRef old, int x, int y, int width, int height)
{
  BufferRef buf = old;
  buf.buffer += x + y*buf.ydelta;
  buf.width = width;
  buf.height = height;
  return buf;
}
// source is smaller than target and xx/yy needs to be inside target
// rect is inside source
void BufferRef::CopyBuffer(BufferRef source, BufferRef target, int xx, int yy, int rx, int ry, int rex, int rey)
{
  //std::cout << "CopyS: " << rx << " " << ry << " " << rex << " " << rey << std::endl;
  if (rx<0) rx = 0;
  if (ry<0) ry = 0;
  if (rex < 0) rex=0;
  if (rey < 0) rey=0;
  if (rex>(int)source.width) rex=source.width;
  if (rey>(int)source.height) rey=source.height;
  if (rx > rex) std::swap(rx,rex);
  if (ry > rey) std::swap(ry,rey);
  if (rx<0) rx = 0;
  if (ry<0) ry = 0;
  if (rex>(int)source.width) rex=source.width;
  if (rey>(int)source.height) rey=source.height;
  int copywidth = rex-rx;
  int copyheight = rey-ry;
  if (rx+copywidth > (int)source.width) copywidth = source.width - rx;
  if (ry+copyheight > (int)source.height) copyheight = source.height - ry;
  //if (source.width+xx > target.width) copywidth = target.width - xx;
  //if (source.height+yy > target.height) copyheight = target.height - yy;
  //std::cout << "Copy: " << copywidth << " " << copyheight << std::endl;
  for(int y=0;y<copyheight;y++)
    for(int x=0;x<copywidth;x++)
      target.buffer[(rx+x+xx)+(ry+y+yy)*target.ydelta] = source.buffer[(rx+x+(ry+y)*source.ydelta)];
}
void BufferRef::FreeBuffer(BufferRef &ref)
{
  delete [] ref.buffer;
  ref.buffer = 0;
}
BufferRefMask BufferRefMask::NewBuffer(int width, int height)
{
  unsigned char *buffer = new unsigned char[width*height];
  BufferRefMask ref;
  ref.buffer = buffer;
  ref.width = width;
  ref.height = height;
  ref.ydelta = width;
  return ref;

}
BufferRefF BufferRefF::NewBuffer(int width, int height)
{
  float *buffer = new float[width*height];
  BufferRefF ref;
  ref.buffer = buffer;
  ref.width = width;
  ref.height = height;
  ref.ydelta = width;
  return ref;
}
BufferRefF BufferRefF::SubBuffer(BufferRefF old, int x, int y, int width, int height)
{
  BufferRefF buf = old;
  buf.buffer += x + y*buf.ydelta;
  buf.width = width;
  buf.height = height;
  return buf;
}
