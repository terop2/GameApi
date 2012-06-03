
class WorldWalls
{
public:
  virtual void Jump(int dx, int dy)=0;
  virtual void SetWallType(int i)=0;
  virtual void SetWallHeight(int height)=0;
  virtual void SetWallYDelta(int delta)=0;
};
static const int n = 0x01; // movement
static const int s = 0x02;
static const int e = 0x04;
static const int w = 0x08;
static const int jump = 0x8000; // jump(dx,dy)
static const int d = 0x10; // height
static const int dd = 0x20;
static const int u = 0x40;
static const int uu = 0x80;
static const int gs = 0x100; // groupstart(num)
static const int ge = 0x200; // groupend(num)
static const int g = 0x400;  // group(num)
static const int w0 = 0x10000; // wall types
static const int w1 = 0x800; 
static const int w2 = 0x1000;
static const int w3 = 0x2000;
static const int skip = 0x4000; // if(state, skipcount)
void ReadData(int *data, WorldWalls &walls);
