
void ReadData(int *data, int *dataend, WorldWalls &walls, std::map<int, bool> &states)
{
  if (!data) return;
  if (!dataend) return;
  std::map<int, int*> group_start;
  std::map<int, int*> group_end;
  for(;data<dataend;)
    {
      int c = *data;
      if (!c) break;
      data+=1;
      if (c & n) walls.Jump(-1,0);
      if (c & s) walls.Jump(1,0);
      if (c & e) walls.Jump(0,1);
      if (c & w) walls.Jump(0,-1);
      if (c & jump) { int dx = *data++; int dy = *data++; walls.Jump(dx,dy); }
      if (c == d) walls.SetYDelta(1);
      if (c == dd) walls.SetYDelta(2);
      if (c == u) walls.SetWallHeight(1);
      if (c == uu) walls.SetWallHeight(2);
      if (c == gs) { int num = *data++; group_start[num] = data; }
      if (c == ge) { int num = *data++; group_end[num]=data-2; }
      if (c == g) 
	{ 
	  int num = *data++; 
	  ReadData(group_start[num], group_end[num], walls); 
	}
      if (c == w0) walls.SetWallType(0);
      if (c == w1) walls.SetWallType(1);
      if (c == w2) walls.SetWallType(2);
      if (c == w3) walls.SetWallType(3);
      if (c == skip) 
	{ 
	  int state=*data++;
	  int num = *data++; 
	  if (states[state]) 
	    data+=num; 
	}
    }
}
