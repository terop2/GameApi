
void GameApiTest();

#include "Effect.hh"
#include "Graph.hh"

class BitmapMod
{
public:
  virtual void Origo()=0;
  virtual void SetPos(int x, int y)=0;
  virtual void Move(int dx, int dy)=0;
  virtual void Change(int value)=0;
  virtual int Value() const=0;
  virtual void SavePos(int id)=0;
  virtual void LoadPos(int id)=0;
  virtual Pos CurrentPos() const=0;
};

class DepMod
{
public:
  virtual void Value(int i)=0;
  virtual void Func(int i1, int i2)=0;
};
void FloodFill(BitmapMod &m, int old_tile, int new_tile)
{
  std::vector<Pos> p;
  Pos curr = m.CurrentPos();
  p.push_back(curr);
  while(!p.empty()) {
    Pos pos = p.back();
    p.pop_back();
    m.SetPos(pos.x, pos.y);
    if (m.Value() == old_tile)
      {
	m.Change(new_tile);
	Pos px1 = { pos.x-1, pos.y };
	Pos px2 = { pos.x+1, pos.y };
	Pos py1 = { pos.x, pos.y-1 };
	Pos py2 = { pos.x, pos.y+1 };
	p.push_back(px1);
	p.push_back(px2);
	p.push_back(py1);
	p.push_back(py2);
      }
  }
}


void Square(BitmapMod &m, 
	    int corner_tl, int middle_t, int corner_tr,
	    int middle_l, int middle_r,
	    int corner_bl, int middle_b, int corner_br,
	    int width, int height)
{
  m.Change(corner_tl);
  m.SavePos(0);
  for(int i=1;i<width-1;i++)
    {
      m.Move(1,0);
      m.Change(middle_t);
    }
  m.Move(1,0);
  m.Change(corner_tr);
  m.LoadPos(0);
  for(int i=1;i<height-1;i++)
    {
      m.Move(0,1);
      m.Change(middle_l);
      m.Move(width,0);
      m.Change(middle_r);
      m.Move(-width,0);
    }
  m.Move(0,1);
  m.Change(corner_bl);
  for(int i=1;i<width-1;i++)
    {
      m.Move(1,0);
      m.Change(middle_t);
    }
  m.Move(1,0);
  m.Change(corner_br);
}
	    
void Square(DepMod &m, int v)
{
  m.Value(v+0);
  m.Value(v+1);
  m.Value(v+2);
  m.Value(v+3);
  m.Func(v+0,v+1);
  m.Func(v+1,v+2);
  m.Func(v+0,v+3);
  m.Func(v+3,v+2);
}
