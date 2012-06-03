
class FaceCollection;
template<class T>
class Voxel;
class PointCollection;

struct Combine
{
public:
  FaceCollection *faces;
  PointCollection *positions;
  Voxel<Cube3> *areas;
  Graph *graph;
  GraphData<VertexData3d, EdgeData3d> *graphdata;
  GraphData<PointI, int> *graphareas;
  int current_graph;
  Bitmap<int> *graphs;
};

struct Path
{
  int bitmap_x, bitmap_y;
  int graph_vertex;  
};
