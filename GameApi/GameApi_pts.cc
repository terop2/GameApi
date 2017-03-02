#include "GameApi_h.hh"

#ifndef EMSCRIPTEN
#define VAO 1
#endif


class HeightMapPoints : public PointsApiPoints
{
public:
  HeightMapPoints(Bitmap<::Color> &color, Bitmap<float> &height, Point pos, Vector u_x, Vector u_y, Vector u_z, int sx, int sy) : color(color), height(height), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx), sy(sy) { }
  virtual int NumPoints() const { return sx*sy; }
  virtual Point Pos(int i) const
  {
    int yy = i / sx;
    int xx = i - (yy*sx);
    float h = height.Map(xx*height.SizeX()/sx,yy*height.SizeY()/sy);
    Point p = pos + u_x*xx/sx + u_y*yy/sy + h*u_z;
    return p;
  }
  virtual unsigned int Color(int i) const
  {
    int yy = i / sx;
    int xx = i - (yy*sx);
    ::Color c = color.Map(xx*color.SizeX()/sx,yy*color.SizeY()/sy);
    return c.Pixel();
  }
private:
  Bitmap<::Color> &color;
  Bitmap<float> &height;
  Point pos;
  Vector u_x;
  Vector u_y;
  Vector u_z;
  int sx,sy;
};
class OrPoints : public PointsApiPoints
{
public:
  OrPoints(PointsApiPoints *pts1, PointsApiPoints *pts2): pts1(pts1), pts2(pts2) { }
  virtual int NumPoints() const { return pts1->NumPoints()+pts2->NumPoints(); }
  virtual Point Pos(int i) const
  {
    if (i<pts1->NumPoints())
      {
	return pts1->Pos(i);
      }
    return pts2->Pos(i-pts1->NumPoints());
  }
  virtual unsigned int Color(int i) const
  {
    if (i<pts1->NumPoints())
      {
	return pts1->Color(i);
      }
    return pts2->Color(i-pts1->NumPoints());
  }

private:
  PointsApiPoints *pts1;
  PointsApiPoints *pts2;
};
class SurfacePoints : public PointsApiPoints
{
public:
  SurfacePoints(std::vector<Point> *points, std::vector<unsigned int> *color) :points(points), color(color) { }
  virtual int NumPoints() const { return points->size(); }
  virtual Point Pos(int i) const { return points->operator[](i); }
  virtual unsigned int Color(int i) const
  {
    return color->operator[](i);
  }
  ~SurfacePoints() { delete points; delete color; }
private:
  std::vector<Point> *points;
  std::vector<unsigned int> *color;
};
EXPORT GameApi::PTS GameApi::PointsApi::surface(std::function<PT (float,float)> surf,
						std::function<unsigned int (PT,float,float)> color,
						float start_u, float end_u,
						float start_v, float end_v,
						float step_u, float step_v)
{
  std::vector<Point> *points = new std::vector<Point>;
  std::vector<unsigned int> *color2 = new std::vector<unsigned int>;
  for(float u = start_u;u<end_u;u+=step_u)
    {
      for(float v = start_v; v<end_v; v+=step_v)
	{
	  PT pos = surf(u,v);
	  unsigned int col = color(pos, u,v);
	  Point *pos_1 = find_point(e, pos);
	  points->push_back(*pos_1);
	  color2->push_back(col);
	}
    }
  return add_points_api_points(e, new SurfacePoints(points, color2));
}
EXPORT GameApi::PTS GameApi::PointsApi::random_plane(PT pos, V u_x, V u_y, int numpoints)
{
  Point *pos_1 = find_point(e,pos);
  Vector *ux_1 = find_vector(e,u_x);
  Vector *uy_1 = find_vector(e,u_y);
  std::vector<Point> *points = new std::vector<Point>;
  std::vector<unsigned int> *color2 = new std::vector<unsigned int>;
  for(int i=0;i<numpoints;i++)
    {
      Random r;
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      
      Point p = *pos_1 + xp* (*ux_1) + yp*(*uy_1);
      points->push_back(p);
      color2->push_back(0xffffffff);
    }


  return add_points_api_points(e, new SurfacePoints(points, color2));  
}
class MatrixPoints : public PointsApiPoints
{
public:
  MatrixPoints(PointsApiPoints *pts, Matrix m) : pts(pts), m(m) { }
  int NumPoints() const { return pts->NumPoints(); }
  Point Pos(int i) const {
    Point p = pts->Pos(i);
    return p*m;
  }
  unsigned int Color(int i) const { return pts->Color(i); }
private:
  PointsApiPoints *pts;
  Matrix m;
};
EXPORT GameApi::PTS GameApi::PointsApi::scale(PTS obj, float sx, float sy, float sz)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, obj);

  return add_points_api_points(e, new MatrixPoints(obj2, Matrix::Scale(sx,sy,sz)));

}
EXPORT GameApi::PTS GameApi::PointsApi::heightmap(BM colour, FB height, PT pos, V u_x, V u_y, V u_z, int sx, int sy)
{
  BitmapHandle *h = find_bitmap(e, colour);
  Bitmap<Color> *colour_bm = find_color_bitmap(h);
  FloatBitmap *fbm = find_float_bitmap(e, height);
  Bitmap<float> *height_bm = fbm->bitmap;
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_points_api_points(e, new HeightMapPoints(*colour_bm, *height_bm, *pt, *uu_x, *uu_y, *uu_z, sx, sy));
}
EXPORT GameApi::PTS GameApi::PointsApi::from_volume(GameApi::O o, GameApi::PT pos,
					     GameApi::V u_x, GameApi::V u_y, GameApi::V u_z, int sx, int sy, int sz)
{
  VolumeObject *obj = find_volume(e, o);
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);
  return add_points_api_points(e, new SpacePoints(*obj, *pt, *uu_x, *uu_y, *uu_z, sx, sy, sz));
}
EXPORT GameApi::PTS GameApi::PointsApi::move(GameApi::PTS obj, float dx, float dy, float dz)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, obj);

  return add_points_api_points(e, new MatrixPoints(obj2, Matrix::Translate(dx,dy,dz)));
}
EXPORT GameApi::PTS GameApi::PointsApi::rot_x(GameApi::PTS obj, float angle)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, obj);

  return add_points_api_points(e, new MatrixPoints(obj2, Matrix::XRotation(angle)));
}
EXPORT GameApi::PTS GameApi::PointsApi::rot_y(GameApi::PTS obj, float angle)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, obj);

  return add_points_api_points(e, new MatrixPoints(obj2, Matrix::YRotation(angle)));
}
EXPORT GameApi::PTS GameApi::PointsApi::rot_z(GameApi::PTS obj, float angle)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, obj);

  return add_points_api_points(e, new MatrixPoints(obj2, Matrix::ZRotation(angle)));
}

EXPORT GameApi::PTS GameApi::PointsApi::or_points(GameApi::PTS p1, GameApi::PTS p2)
{
  PointsApiPoints *pts1 = find_pointsapi_points(e, p1);
  PointsApiPoints *pts2 = find_pointsapi_points(e, p2);
  return add_points_api_points(e,new OrPoints(pts1, pts2));
  
}
class ShadowPoints : public PointsApiPoints
{
public:
  ShadowPoints(PointsApiPoints *pts, Point pos, Vector u_x, Vector u_y, Vector light_vec) : pts(pts), pos(pos), u_x(u_x), u_y(u_y), light_vec(light_vec) { }
  virtual int NumPoints() const
  {
    return pts->NumPoints();
  }
  virtual Point Pos(int i) const
  {
    Point p = pts->Pos(i);
    Vector v = light_vec;
    LinePlaneIntersection sect = LinePlaneIntersectionFunc(p, p+v,
							   pos, pos+u_x, pos+u_y);
    return p+v*sect.tuv.dx;
  }
  virtual unsigned int Color(int i) const
  {
    return pts->Color(i);
  }

private:
  PointsApiPoints *pts;
  Point pos;
  Vector u_x;
  Vector u_y;
  Vector light_vec;
};
EXPORT GameApi::PTS GameApi::PointsApi::shadow_points(GameApi::PTS obj,
					       PT pos,
					       V u_x, V u_y,
					       V light_vec)
{
  PointsApiPoints *pts = find_pointsapi_points(e, obj);
  Point *pt = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *light = find_vector(e, light_vec);
  return add_points_api_points(e, new ShadowPoints(pts, *pt, *uu_x, *uu_y, *light));
}
int GameApi::PointsApi::num_points(PTA pta)
{
  PointArray3 *arr = find_point_array3(e, pta);
  return arr->numpoints;
}
float *GameApi::PointsApi::point_access(GameApi::PTA pta, int pointnum)
{
  PointArray3 *arr = find_point_array3(e, pta);
  return &arr->array[pointnum*3];
}
EXPORT void GameApi::PointsApi::set_point(PTA pta, int pointnum, float x, float y, float z)
{
  float *ptr = point_access(pta, pointnum);
  ptr[0] = x;
  ptr[1] = y;
  ptr[2] = z;
}
#if 0
unsigned int *GameApi::PointsApi::color_access(GameApi::PTA pta, int pointnum)
{
  PointArray3 *arr = find_point_array3(e, pta);
  return &arr->color[pointnum];
}
void GameApi::PointsApi::update(GameApi::PTA pta)
{
  PointArray3 *arr = find_point_array3(e, pta);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(float)*3, arr->array);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(unsigned int), arr->color);
}
#endif
unsigned int swap_color(unsigned int c)
{
  unsigned int ca = c & 0xff000000;
  unsigned int cr = c & 0x00ff0000;
  unsigned int cg = c & 0x0000ff00;
  unsigned int cb = c & 0x000000ff;
  cr>>=16;
  //cg>>=8;
  cb<<=16;
  //cg<<=8;
  return ca+cr+cg+cb;
}


struct ThreadInfo_pts
{
  pthread_t thread_id;
  PointArray3 *arr;
  PointsApiPoints *pts;
  int start_range;
  int end_range;
};
void *thread_func_pts(void *data);

class ThreadedPrepare_pts
{
public:
  ThreadedPrepare_pts(PointsApiPoints *pts) : pts(pts) { }
  int push_thread(int start_range, int end_range)
  {
    PointArray3 *arr = new PointArray3;
    arr->array = new float[(end_range-start_range)*3];
    arr->color = new unsigned int[end_range-start_range];
    arr->numpoints = end_range-start_range;
    sets.push_back(arr);

    ThreadInfo_pts *info = new ThreadInfo_pts;
    info->arr = arr;
    info->start_range = start_range;
    info->end_range = end_range;
    info->pts = pts;
    ti.push_back(info);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 3000000);

    pthread_create(&info->thread_id, &attr, &thread_func_pts, (void*)info);
    
    pthread_attr_destroy(&attr);
    return sets.size()-1;
  }
  void join(int id)
  {
    void *res;
    pthread_join(ti[id]->thread_id, &res);
  }
  PointArray3* collect(int num_items)
  {
    PointArray3 *arr = new PointArray3;
    arr->array = new float[(num_items)*3];
    arr->color = new unsigned int[num_items];
    arr->numpoints = num_items;
    int s = sets.size();
    float *array = arr->array;
    unsigned int *color = arr->color;
    for(int i=0;i<s;i++)
      {
	PointArray3 *src = sets[i];
	int ss = src->numpoints;
	for(int j=0;j<ss;j++)
	  {
	    *array++ = src->array[j*3+0];
	    *array++ = src->array[j*3+1];
	    *array++ = src->array[j*3+2];
	    *color++ = src->color[j];
	  }
      }
    return arr;
  }
  ~ThreadedPrepare_pts()
  {
    int s = sets.size();
    for(int i=0;i<s;i++)
      {
	delete sets[i]->array;
	delete sets[i]->color;
	delete sets[i];
	delete ti[i];
      }
  }
private:
  PointsApiPoints *pts;
  std::vector<PointArray3*> sets;
  std::vector<ThreadInfo_pts*> ti;
};



EXPORT GameApi::PTA GameApi::PointsApi::prepare(GameApi::PTS p)
{
  PointsApiPoints *pts = find_pointsapi_points(e, p);
  int numpoints = pts->NumPoints();
#if 1
  //ndef THREADS
  //ndef THREADS
  //ifndef THREADS0
  float *array = new float[numpoints*3];
  unsigned int *color = new unsigned int[numpoints];

  for(int i=0;i<numpoints;i++)
    {
      Point p = pts->Pos(i);
      unsigned int c = pts->Color(i);
      array[i*3+0] = p.x;
      array[i*3+1] = p.y;
      array[i*3+2] = p.z;
      color[i] = swap_color(c);
    }
  PointArray3 *arr = new PointArray3;
  arr->array = array;
  arr->color = color;
  arr->numpoints = numpoints;
#else
  int num_threads = 4;
  ThreadedPrepare_pts prep(pts);
  int s = numpoints;
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  for(int i=0;i<num_threads;i++)
    {
      int start_range = i*delta_s;
      int end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; }
      if (i==num_threads-1) { end_range = s; }
      vec.push_back(prep.push_thread(start_range, end_range));
    }
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
    }
  PointArray3 *arr = prep.collect(s);
#endif
#ifdef VAO
  glGenVertexArrays(1, &arr->vao[0]);
  glBindVertexArray(arr->vao[0]);
  const int vertex_id = 0;
#endif
#ifndef VAO
  const int vertex_id = 0;
#endif
  glGenBuffers(2, &arr->buffer[0]);
  glEnableVertexAttribArray(vertex_id);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glBufferData(GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color, GL_STATIC_DRAW);
#ifdef VAO
  glEnableVertexAttribArray(vertex_id);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(vertex_id, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
#endif
#ifdef VAO
  glBindVertexArray(0);
#endif
  return add_point_array3(e,arr);
}
class PTSFromFloatVolume : public PointsApiPoints
{
public:
  PTSFromFloatVolume(FloatVolumeObject *fo, int numpoints, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z) 
    : fo(fo), array(0), numpoints(numpoints), 
      start_x(start_x), start_y(start_y), start_z(start_z), 
      end_x(end_x), end_y(end_y), end_z(end_z) 
  { prepare(); }
  ~PTSFromFloatVolume() { delete [] array; }
  virtual int NumPoints() const { return numpoints; }
  virtual Point Pos(int i) const
  {
    return Point(array[i*3+0],
		 array[i*3+1],
		 array[i*3+2]);
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
  void prepare()
  {
    array = new float[numpoints*3];
    int index = 0;
    for(;index/3<numpoints;)
      {
	while(1) {
	  Random r;
	  float xp = double(r.next())/r.maximum()*(end_x-start_x)+start_x;
	  float yp = double(r.next())/r.maximum()*(end_y-start_y)+start_y;
	  float zp = double(r.next())/r.maximum()*(end_z-start_z)+start_z;
	  float val = double(r.next())/r.maximum();
	  if (fo->FloatValue(Point(xp,yp,zp))>val)
	    {
	      array[index+0] = xp;
	      array[index+1] = yp;
	      array[index+2] = zp;
	      index+=3;
	      break;
	    }
	}
      }
  }
private:
  FloatVolumeObject *fo;
  float *array;
  int numpoints;
  float start_x, start_y, start_z;
  float end_x, end_y, end_z;
};
EXPORT GameApi::PTS GameApi::PointsApi::from_float_volume(GameApi::FO object,
						   int numpoints,
						   float start_x, float start_y, float start_z,
						   float end_x, float end_y, float end_z)
{
  FloatVolumeObject *fo = find_float_volume(e, object);
  return add_points_api_points(e, new PTSFromFloatVolume(fo, numpoints, start_x, start_y, start_z, end_x, end_y, end_z));
}


EXPORT void GameApi::PointsApi::explode(GameApi::PTA array, float x, float y, float z, float dist)
{
  PointArray3 *arr = find_point_array3(e, array);
  float *arr2 = arr->array;
  Point p2(x,y,z);
  for(int i=0;i<arr->numpoints;i++)
    {
      Point p(arr2[0], arr2[1], arr2[2]);
      Vector v = p-p2;
      float d = v.Dist();
      v/=d;
      v*=d+dist;
      Point pp = p2+v;
      arr2[0] = pp.x;
      arr2[1] = pp.y;
      arr2[2] = pp.z;
      arr2+=3;
    }
}
EXPORT void GameApi::PointsApi::render(GameApi::PTA array)
{
  PointArray3 *arr = find_point_array3(e, array);
#ifdef VAO
  glBindVertexArray(arr->vao[0]);
#endif
#ifndef VAO
  const int vertex_id = 0;
  glEnableVertexAttribArray(vertex_id);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(vertex_id, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
  glDrawArrays(GL_POINTS, 0, arr->numpoints);
#ifndef VAO
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
#endif
  
}


class PointApiPointFunction : public PointsApiPoints
{
public:
  PointApiPointFunction(GameApi::Env &e, std::function<GameApi::PT(int pointnum)> f, int numpoints) : e(e), f(f), numpoints(numpoints) { }
  virtual int NumPoints() const
  {
    return numpoints;
  }
  virtual Point Pos(int i) const
  {
    GameApi::PT pt = f(i);
    Point *pts = find_point(e, pt);
    return *pts;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  GameApi::Env &e;
  std::function<GameApi::PT(int pointnum)> f;
  int numpoints;
};

class PointsApiColorFunction : public PointsApiPoints
{
public:
  PointsApiColorFunction(GameApi::Env &e, PointsApiPoints *orig, std::function<unsigned int(int pointnum, GameApi::PT pos)> f) : e(e), orig(orig), f(f) { }
  virtual int NumPoints() const { return orig->NumPoints(); }
  virtual Point Pos(int i) const { return orig->Pos(i); }
  virtual unsigned int Color(int i) const
  {
    Point p = Pos(i);
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    unsigned int color = f(i, pt);
    return color;
  }
private:
  GameApi::Env &e;
  PointsApiPoints *orig;
  std::function<unsigned int(int pointnum, GameApi::PT pos)> f;
};


EXPORT GameApi::PTS GameApi::PointsApi::function(std::function<GameApi::PT(int pointnum)> f, int numpoints)
{
  return add_points_api_points(e, new PointApiPointFunction(e, f, numpoints));
}
EXPORT GameApi::PTS GameApi::PointsApi::color_function(PTS orig, std::function<unsigned int(int pointnum, PT pos)> f)
{
  PointsApiPoints *pts = find_pointsapi_points(e, orig);
  return add_points_api_points(e, new PointsApiColorFunction(e, pts, f));
}

class SurfacePoints3 : public PointsApiPoints
{
public:
  SurfacePoints3(SurfaceIn3d *surf, int sx, int sy) : surf(surf), sx(sx), sy(sy) { }
  int NumPoints() const {
    return sx*sy;
  }
  Point Pos(int i) const
  {
    int x = i/sy;
    int y = i - x*sy;
    float xx = x * surf->XSize() / sx;
    float yy = y * surf->YSize() / sy;
    return surf->Index(xx,yy);
  }
  unsigned int Color(int i) const
  {
    int x = i/sy;
    int y = i - x*sy;
    float xx = x * surf->XSize() / sx;
    float yy = y * surf->YSize() / sy;
    return surf->Color(xx,yy); 
  }
private:
  SurfaceIn3d *surf;
  int sx;
  int sy;
};

EXPORT GameApi::PTS GameApi::PointsApi::surface(S surf, int sx, int sy)
{
  SurfaceImpl *s = find_surface(e, surf);
  return add_points_api_points(e, new SurfacePoints3(s->surf, sx,sy));
}

EXPORT int GameApi::PointsApi::NumPoints(PTS p)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, p);
  return obj2->NumPoints();
}
EXPORT float GameApi::PointsApi::pos_x(PTS p, int index)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, p);
  return obj2->Pos(index).x;
}
EXPORT float GameApi::PointsApi::pos_y(PTS p, int index)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, p);
  return obj2->Pos(index).y;
}
EXPORT float GameApi::PointsApi::pos_z(PTS p, int index)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, p);
  return obj2->Pos(index).z;
}

void *thread_func_pts(void *data)
{
  ThreadInfo_pts *ti = (ThreadInfo_pts*)data;

  int jj=0;
  for(int i=ti->start_range;i<ti->end_range;i++)
    {
      Point p = ti->pts->Pos(i);
      unsigned int c = ti->pts->Color(i);
      ti->arr->array[jj*3+0] = p.x;
      ti->arr->array[jj*3+1] = p.y;
      ti->arr->array[jj*3+2] = p.z;
      ti->arr->color[jj] = swap_color(c);
      jj++;
    }
  //  std::cout << "Thread finished" << jj << " " << ti->arr->numpoints << std::endl;
  return 0;
}
class ColorPoints : public PointsApiPoints
{
public:
  ColorPoints(PointsApiPoints *next, unsigned int color) : next(next), color(color) { }
  int NumPoints() const { return next->NumPoints(); }
  Point Pos(int i) const { return next->Pos(i); }
  unsigned int Color(int i) const {return color; }
private:
  PointsApiPoints *next;
  unsigned int color;
};
GameApi::PTS GameApi::PointsApi::color_points(PTS p, unsigned int color)
{
  PointsApiPoints *obj = find_pointsapi_points(e, p);
  return add_points_api_points(e, new ColorPoints(obj, color));
}
class PointsApiRender : public MainLoopItem
{
public:
  PointsApiRender(GameApi::Env &env, GameApi::EveryApi &ev, PointArray3 *arr) : env(env), ev(ev), arr(arr) { }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    GameApi::SH sh;
    sh.id = e.sh_color;

    ev.shader_api.use(sh);
    Matrix mat2 = e.in_MV;
    GameApi::M mat = add_matrix2(env, mat2);
    ev.shader_api.set_var(sh, "in_MV", mat);

#if 1
    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = e.us_vertex_shader;
	fragment.id = e.us_fragment_shader;
	shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment);
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 
      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_N", m2);

	sh = shader;
      }
#endif
    ev.shader_api.use(sh);
#ifdef VAO
    glBindVertexArray(arr->vao[0]);
#endif
#ifndef VAO
  const int vertex_id = 0;
  glEnableVertexAttribArray(vertex_id);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(vertex_id, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

    glDrawArrays(GL_POINTS, 0, arr->numpoints);

#ifndef VAO
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(4);
#endif

  }
  int shader_id() { return -1; }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  PointArray3 *arr;
  GameApi::SH shader;
};
GameApi::ML GameApi::PointsApi::render_ml(EveryApi &ev, PTA array)
{
  PointArray3 *arr = find_point_array3(e, array);
  return add_main_loop(e, new PointsApiRender(e, ev,arr));
}


GameApi::PTS GameApi::PointsApi::random_bitmap_instancing(EveryApi &ev, BB bm, int count, float start_x, float end_x, float start_z, float end_z, float y)
{
  std::vector<Point> *points = new std::vector<Point>;
  std::vector<unsigned int> *color2 = new std::vector<unsigned int>;
  int sx = ev.bool_bitmap_api.size_x(bm);
  int sy = ev.bool_bitmap_api.size_y(bm);
  for(int i=0;i<count;i++)
    {
      Random r;
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      float xx = xp*sx;
      float yy = yp*sy;
      bool b = ev.bool_bitmap_api.boolvalue(bm, xx,yy);
      if (b) {
	Point p(start_x + (end_x-start_x)*xp, start_z+(end_z-start_z)*yp, y);
	points->push_back(p);
	color2->push_back(0xffffffff);
      }
    }
   return add_points_api_points(e, new SurfacePoints(points, color2));  
 
}
GameApi::PTS GameApi::PointsApi::random_mesh_quad_instancing(EveryApi &ev, P p, int count)
{
  FaceCollection *coll = find_facecoll(e, p);
  std::vector<Point> *points = new std::vector<Point>;
  std::vector<unsigned int> *color2 = new std::vector<unsigned int>;

  for(int i=0;i<count;i++)
    {
      Random r;
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      float zp = double(r.next())/r.maximum();
      xp*=2.0;
      yp*=2.0;
      xp-=1.0;
      yp-=1.0;
      zp*=float(coll->NumFaces());
      int zpi = int(zp);
      if (zpi<0) zpi = 0;
      if (zpi>=coll->NumFaces()) zpi = coll->NumFaces()-1;
      Point p1 = coll->FacePoint(zpi, 0);
      Point p2 = coll->FacePoint(zpi, 1);
      Point p3 = coll->FacePoint(zpi, 2);
      Point p4 = coll->FacePoint(zpi, 3);
      Point p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(p1) + (1.0f+xp)*(1.0f-yp)*Vector(p2) + (1.0f+xp)*(1.0f+yp)*Vector(p3) + (1.0f-xp)*(1.0f+yp)*Vector(p4));
      points->push_back(p);
      color2->push_back(0xffffffff);
    }
   return add_points_api_points(e, new SurfacePoints(points, color2));
}
/*
GameApi::PTS GameApi::PointsApi::random_bitmap_edge_instancing(BB bm, float start_x, float end_x, float start_z, float end_z, float y)
{
}
*/
