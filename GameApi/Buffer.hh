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


#ifndef BUFFER_HH
#define BUFFER_HH


struct BufferRef
{
  unsigned int *buffer;
  unsigned int width;
  unsigned int height;
  unsigned int ydelta;
  static BufferRef SubBuffer(BufferRef old, int x, int y, int width, int height)
;
  static BufferRef NewBuffer(int width, int height);
  static void FreeBuffer(BufferRef ref);
  static void CopyBuffer(BufferRef source, BufferRef target, int x, int y, int rx, int ry, int rex, int rey);
};
struct BufferRefF
{
  float *buffer;
  unsigned int width;
  unsigned int height;
  unsigned int ydelta;
  static BufferRefF SubBuffer(BufferRefF old, int x, int y, int width, int height)
;
  static BufferRefF NewBuffer(int width, int height);

};

struct BufferRefMask
{
  unsigned char *buffer;
  unsigned int width;
  unsigned int height;
  unsigned int ydelta;
  static BufferRefMask NewBuffer(int width, int height);
};

class BufferRefReq
{
public:
  virtual void Gen() const=0;
  virtual BufferRef Buffer() const=0;
};


template<class T>
struct Buffer
{
  Buffer(int count) { buffer = new T[count]; }
  Buffer(const Buffer<T> &buf) : buffer(new T[buf.count]), count(buf.count)
  {
    memcpy(buffer, buf.buffer, count*sizeof(T));  
  }
  void set(int index, T val)
  {
    buffer[index] = val;
  }
  T &operator[](int idx) { return buffer[idx]; }
  void operator=(const Buffer<T> &buf) 
  { 
    buffer = new T[buf.count];
    count = buf.count;
    memcpy(buffer, buf.buffer, buf.count*sizeof(T));  
  }
  ~Buffer() { delete [] buffer; }
  T *Array() const { return buffer; }
  int Count() const { return count; }
  static Buffer<T> Compose(const Buffer<T> &buf, const Buffer<T> &buf2)
  {
    Buffer<T> res(buf.count+buf2.count);
    for(int i=0;i<buf.count;i++)
      {
	res[i]=buf[i];
      }
    for(int k=0;k<buf2.count;k++)
      {
	res[buf.count+k]=buf2[k];
      }
    return res;
  }
  int NumTris() const { return count/3/3; }
  int NumQuads() const { return count/6/3; }
  int NumTexCoordTris() const { return count/2/3; }
  int NumColorTris() const { return count/1/3; }
  int NumTexCoordQuads() const { return count/2/6; }
  int NumColorQuads() const { return count/1/6; }
  friend void AnimBuffer(const Buffer<float> &buf, const Buffer<float> &buf2, Buffer<float> &res, float val);
private:
  T *buffer;
  int count;
};
inline void AnimBuffer(const Buffer<float> &buf, const Buffer<float> &buf2, Buffer<float> &res, float val)
{
  float m = 1.0-val;
  float *resbuf = res.buffer;
  float *bufbuf = buf.buffer;
  float *buf2buf = buf2.buffer;
  for(int i=0;i<buf.count;i++)
    {
      resbuf[i]=bufbuf[i]*val+buf2buf[i]*m;
    }
}



#endif
