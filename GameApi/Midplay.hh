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


#include <string>
#include <vector>

struct Chunk
{
  unsigned char Id[4];
  unsigned char Len[4];
  
  unsigned long Length() { return long(Len[0])*256*256*256 +
			     long(Len[1])*256*256+
			     long(Len[2])*256+
			     long(Len[3]); }
};

/*
struct Event
{
  long delta_time;
  int event_type;
  int channel;
  int param1;
  int param2;
};

class MidFileLoader
{
public:
  MidFileLoader(std::string filename);
  int NumberOfChunks() const;
  Chunk *ChunkHeader(int chunk) const;
  unsigned char *ChunkData(int chunk) const;
  std::vector<Event> GetEvents(int chunk) const;
  long VarLength(unsigned char *&data) const;
private:
  std::string contents;
  std::vector<Chunk*> chunks;
};
*/
