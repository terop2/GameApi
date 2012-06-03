
class PercentageLine : public Function<int, int>
{
public:
  void SetWidth(int width) { m_width = width; }
  void SetCount(int count) { m_percentage.resize(count); }
  void SetPercentage(int item, float percentage)
  {
    m_percentage[item] = percentage;
  }
public:
  int Index(int i) const
  {
    return m_percentages[i]*m_width;
  }
private:
  int m_width;
  std::vector<float> m_percentages;
};

//
// Layout
//
//
class Screen : public Bitmap<Color>
{
public:
  Screen(int sx, int sy);
};

class LayoutElem;

class Area : public Bitmap<Color>
{
public:
  Area(int sx, int sy);
  void Reset();
  void PushBack(const LayoutElem &e);
};

struct Rect
{
  float x,y,width,height;
};

bool Inside(Rect r, Point2d p)
{
  if (p.x < r.x) return false;
  if (p.y < r.y) return false;
  if (p.x >= r.x+r.width) return false;
  if (p.y >= r.y+r.height) return false;
  return true;
}

class CmdBuf
{
};

struct ContentElem
{
  ContinuousBitmap<Color> *bitmap;
  CmdBuf *cmdbuf;
};

class LayoutCmdBuf : public CmdBuf
{
public:
  
};

class LayoutElem
{
public:
  virtual Rect GetRect() const=0;
  virtual ContentElem *Bitmap() const=0;
};

class LayoutBitmap : public ContinuousBitmap<Color>
{
public:
  LayoutBitmap(float sx, float sy) : sx(sx), sy(sy) { }
  void PushBack(LayoutElem *elem) { vec.push_back(elem); }
  virtual float SizeX() const { return sx; }
  virtual float SizeY() const { return sy; }
  virtual Color Map(float x, float y) const
  {
    Point2d p = { x,y };
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	LayoutElem *elem = vec[i];
	Rect r = elem->GetRect();
	if (Inside(r, p))
	  {
	    x-=r.x;
	    y-=r.y;
	    ContentElem *celem = elem->Bitmap();
	    ContinuousBitmap<Color> *bm = celem->bitmap;
	    return bm->Map(x,y);
	  }
      }
    return Color(255,255,255,0);
  }
  ~LayoutBitmap()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	LayoutElem *elem = vec[i];
	delete elem;
      }
  }
private:
  float sx,sy;
  std::vector<LayoutElem*> vec;  
};
