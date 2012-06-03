
#include "Effect.hh"
#include "Graph.hh"
#include <typeinfo>

struct FCube
{
  Point p;
  Vector u_x;
  Vector u_y;
  Vector u_z;
};

struct FLine
{
public:
  Point p1,p2;
  Vector u_x,u_y;
};

struct FPlane
{
public:
  Point p1,p2,p3,p4;
  Vector u;
};

struct FFlexCube
{
public:
  float dist_left;
  float dist_right;
  //float height;
  float left_angle; 
  float right_angle; 
  //float top_angle; 
  //float bottom_angle; 
};

struct FCube2
{
  Point tlb;
  Point trb;
  Point blb;
  Point brb;
  Point tlf;
  Point trf;
  Point blf;
  Point brf;
};


void cube(Matrix m, FaceStore &s);
void sphere(Matrix m, int numfaces, int numfaces2, FaceStore &s);
void cone(Matrix m, int numfaces, Point p1, Point p2, float rad1, float rad2, FaceStore &s);
void rectangle(Matrix m, float x, float y, FaceStore &s);
void texture_rectangle(Matrix m, TexturePlugin *plugin, FaceStore &s);

void cylinder(Matrix m, int numfaces, FaceStore &s);
void disk(Matrix m, int numfaces, float disk_pos, FaceStore &s);
void color(BoxableFaceCollection &coll, unsigned int color, FaceStore &s);
void matrix(BoxableFaceCollection &coll, Matrix m, FaceStore &s);
void andnot(BoxableFaceCollection &coll, BoxableFaceCollection &coll2, FaceStore &s);
void ring(Matrix m, const PointVectorCollection &points, float x, int steps, FaceStore &s);

void color_poly(Point2d p1, Point2d p2, Point2d p3, Color c1, Color c2, int x, int y, BitmapStore<Color> &s);


void floor(const FCube &pos, FLine &l1, FLine &l2, FLine &l3, FLine &l4, FPlane &pl, FaceStore &s);

void linetocube(const FLine &line, FCube &cube);

void flexcube(const FPlane &pl, 
	      const FFlexCube &args,
	      FCube2 &res);

void flexcube(const FPlane &pl, float dist_left, float dist_right, float left_angle, float right_angle, FCube2 &res);

void plane_cube_blit(const FPlane &pl, FaceStore &s);

void cubeplane_t(const FCube2 &res, FPlane &pl);
void cubeplane_l(const FCube2 &res, FPlane &pl);
void cubeplane_r(const FCube2 &res, FPlane &pl);
void cubeplane_f(const FCube2 &res, FPlane &pl);

FPlane plane_init();
void plane_dup(const FPlane &pl, FPlane &pl1);

void tunnel_end(const FPlane &pl, FaceStore &s);

int chessboard_bitmap(FrameAnimPlugins *fa);
void chessboard(const FPlane &pl, FaceStore &s);

class FuncEffect : public FrameAnimPlugins
{
public:
  FuncEffect(Render *render);
  void Init();
  bool Frame(float time);
private:
  int tex_index;
};

class FunctionPointerFrameAnim : public FrameAnimPlugins
{
public:
  FunctionPointerFrameAnim(Render *render, void (*fptr)(FaceStore &s)) : FrameAnimPlugins(render), fptr(fptr) { }
  void Init();
  void Cleanup();
  bool Frame(float time);
private:
  void (*fptr)(FaceStore &s);
};

void chessboard_test(FaceStore &s);

class PolyContainer
{
public:
  void Alloc(int bytescount, int objcount2) { buffer = new char[bytescount]; ptrs=new char*[objcount2]; widths = new int[objcount2]; bufferbytes = bytescount; objcount = objcount2; }
  void ResizeObjs(int newobjcount)
  {
    char **p = new char*[newobjcount];
    int *w = new int[newobjcount];
    std::type_info **i = new std::type_info*[newobjcount];
    std::copy(ptrs,ptrs+objcount,p);
    std::copy(widths, widths+objcount, w);
    std::copy(infos, infos+objcount, i);
    delete [] ptrs;
    delete [] widths;
    delete [] infos;
    ptrs = p;
    widths = w;
    infos = i;
    objcount = newobjcount;
  }
  void ResizeBytes(int newcount)
  {
    char *buf = new char[newcount];
    std::copy(buffer, buffer+bufferbytes, buf);
    delete [] buffer;
    buffer = buf;
    bufferbytes = newcount;
  }


  template<class T>
  T *GetObj(int obj)
  {
    if (*(infos[obj])!=typeid(T)) return 0;
    return (T*)ptrs[obj];
  }

  template<class T>
  void AllocObj(int obj)
  {
    if (obj > objcount) ResizeObjs(obj+20);
    if (ptrs[obj] != 0)
      {
	T *ptr = (T*)&ptrs[obj];
	ptr->~T();
      }
    if (allocated_bytes + sizeof(T) >= bufferbytes) ResizeBytes(allocated_bytes + sizeof(T)*20);
    T* ptr = new(buffer+allocated_bytes)T;
    ptrs[obj] = ptr;
    widths[obj] = sizeof(T);
    infos[obj] = new std::type_info(typeid(T));
    allocated_bytes+=sizeof(T);
  }
private:
  char *buffer;
  int bufferbytes;
  char **ptrs;
  int *widths;
  std::type_info **infos;
  int objcount;

  int allocated_bytes;
};
