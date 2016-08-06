
#ifndef FONTEDITOR_HH
#define FONTEDITOR_HH

#include "Effect.hh"
#include "Graph.hh"

class Font;
void ExecuteFontEditor(int argc, char **argv, Font *font);


void DrawDot(BufferRef ref, Point2d p);
void DrawDots(BufferRef ref, PointCollection2d &p);

int FindNearestPoint(Point2d p, PointCollection2d &points);

class Zoom : public Bitmap<Color>
{
public:
  Zoom(Bitmap<Color> &bm, int level) : bm(bm), level(level) { }
  void Prepare() { bm.Prepare(); }

  int SizeX() const { return level*bm.SizeX(); }
  int SizeY() const { return level*bm.SizeY(); }
  Color Map(int x, int y) const
  {
    int xx = x/level;
    int yy = y/level;
    if (xx==oldx && yy==oldy)
      {
	return oldColor;
      }
    oldColor = bm.Map(xx,yy);
    oldx = xx; oldy=yy;
    return oldColor;
  }

  static Point2d ZoomToSmall(Point2d p, int level) { p.x /= level; p.y/=level; return p; }
  static Point2d SmallToZoom(Point2d p, int level) { p.x *= level; p.y*=level; return p; }


private:
  Bitmap<Color> &bm;
  int level;

  mutable int oldx, oldy;
  mutable Color oldColor;
};

class ZoomPoints : public PointCollection2d
{
public:
  ZoomPoints(PointCollection2d &coll, int level) : coll(coll), level(level) { }
  int Size() const { return coll.Size(); }
  Point2d Index(int i) const { return Point2d::FromVector(Vector2d::FromPoint(coll.Index(i))*float(level)); }
private:
  PointCollection2d &coll;
  int level;
};

#endif
