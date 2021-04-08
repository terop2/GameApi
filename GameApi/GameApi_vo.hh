
class Instanced_Points : public PointsApiPoints
{
public:
  Instanced_Points(float *arr, int size) : arr(arr), size(size) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  int NumPoints() const { return size/3; }
  Point Pos(int i) const { return Point(arr[i*3+0], arr[i*3+1], arr[i*3+2]); }
  unsigned int Color(int i) const { return 0xffffffff; }
  ~Instanced_Points() { delete [] arr; }
private:
  float *arr;
  int size;
};
