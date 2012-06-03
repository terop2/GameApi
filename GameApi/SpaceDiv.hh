
class SpaceDivisionDecision
{
public:
  virtual bool Div(Point p) const=0;
};

class SpaceDivisionArray
{
public:
  std::vector<SpaceDivisionDecision*> vec;
};

class DecisionArray
{
public:
  std::vector<bool> vec;
};

struct Objects
{
  std::vector<Point> *center_points;
  std::vector<FaceCollection*> *objects;
};

class ObjectsFromVolume
{
public:
  virtual Objects Get(float start_x, float end_x,
		      float start_y, float end_y,
		      float start_z, float end_z, 
		      float time) const=0;
};

struct Dim
{
  float dx;
  float dy;
};

struct World
{
  Bitmap<int> *world_bitmap[30]; // 30 = levels
  Dim world_level_accuracy[30];
  int world_piece_count;
  Bitmap<bool> *world_pieces;
  FaceCollection **true_pieces;
  FaceCollection **false_pieces;
};
