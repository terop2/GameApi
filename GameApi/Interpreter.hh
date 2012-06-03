
char cmda[] = "ca";
char cmdb[] = { 1,2,3,4,5,0 };
char cmdc[] = { 1,2,3,4,5,0 };
char *cmds[] = { &cmda, &cmdb, &cmdc };

class PointCollectionInterpret : public PointCollection2d
{
public:
  PointCollectionInterpret(char** cmds) : cmds(cmds) { current_point.x = 0.0; current_point.y = 0.0; current_vector.dx = 0.0; current_vector.dy = -1.0; }
  int Size() const { return vec.size(); }
  Point2d Index(int i) const
  {
    return vec[i];
  }

  void execute(char cmd)
  {
    char *cmdptr = cmds[cmd];
    while(1)
      {
	switch(*cmdptr++)
	  {
	  case '\0': return;
	  case 'c': // funccall: (1, cmdnum)
	    {
	      char cmdtoexecute = *cmdptr++ - 'a';
	      execute(cmdtoexecute);
	    }
	    break;
	  case 'p': // Point
	    {
	      vec.push_back(current_point);
	    }
	    break;
	  case 'f': // Forward
	    {
	      current_point += current_vector;
	      break;
	    }
	    break;
	  case 'b': // Backward
	    {
	      current_point += current_vector;
	      break;
	    }
	    break;
	  case 'l': // rotate left
	    {
	      Vector2d v;
	      v.dx = current_vector.dx * cos(45.0/2.0*2.0*3.14159/360.0)
		- current_vector.dy * sin(45/2.0*2.0*3.14159/360.0);
	      v.dy = current_vector.dx * sin(45.0/2.0*2.0*3.14159/360.0)
		+ current_vector.dy * cos(45.0/2.0*2.0*3.14159/360.0);
	      current_vector = v;
	    }
	    break;
	  case 'r': // rotate right
	    {
	      Vector2d v;
	      v.dx = current_vector.dx * cos(-45.0/2.0*2.0*3.14159/360.0)
		- current_vector.dy * sin(-45/2.0*2.0*3.14159/360.0);
	      v.dy = current_vector.dx * sin(-45.0/2.0*2.0*3.14159/360.0)
		+ current_vector.dy * cos(-45.0/2.0*2.0*3.14159/360.0);
	      current_vector = v;
	    }
	    break;
	  case 's': // scale down *2
	    {
	      current_vector /= 2.0;
	      break;
	    }
	  case 'S': // scale up *2
	    {
	      current_vector *= 2.0;
	      break;
	    }
	  };
      }
  }
private:
  char **cmds;
  std::vector<Point2d> vec;
  Point2d current_point;
  Vector2d current_vector;
};

int main()
{
  PointCollectionInterpret d(cmds);
  d.execute(0);
}
