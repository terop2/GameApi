
class LBitmap : public Bitmap<Color>
{
public:
};

class LArea
{
public:
  Point2d tl;
  Point2d br;
};
class LLayerRef
{
public:
  int id;
};
class LLayerContainer
{
  std::vector<LLayer*> layers;
};

class LLayer
{
public:

  LBitmap *bitmap;
  std::vector<LArea*> areas;
};

class LView
{
public:
  std::vector<LLayer*> layers;
  std::vector<Point2d> pos_tl;
  std::vector<Point2d> pos_br;
};

