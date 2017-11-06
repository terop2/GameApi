#include "GameApi_h.hh"

#ifndef EMSCRIPTEN
#define VAO 1
#endif


class HeightMapPoints : public PointsApiPoints
{
public:
  HeightMapPoints(Bitmap<::Color> &color, Bitmap<float> &height, Point pos, Vector u_x, Vector u_y, Vector u_z, int sx, int sy) : color(color), height(height), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx), sy(sy) { }
  void Prepare() { color.Prepare(); height.Prepare(); }
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
  void Prepare() { pts1->Prepare(); pts2->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) {
    pts1->HandleEvent(event);
    pts2->HandleEvent(event);
  }
  virtual bool Update(MainLoopEnv &e) {
    bool b1 = pts1->Update(e);
    bool b2 = pts2->Update(e);
    return b1 || b2;
  }

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
  void Prepare() { }
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
  void Prepare() { pts->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { pts->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return pts->Update(e); }

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
class AnimMixPTS : public PointsApiPoints
{
public:
  AnimMixPTS(PointsApiPoints *o1, PointsApiPoints *o2, float start_val, float end_val, float start_time, float end_time) : o1(o1), o2(o2), start_val(start_val), end_val(end_val), start_time(start_time), end_time(end_time)
  {
    pos = 0.0;
  }
  void Prepare() { o1->Prepare(); o2->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { o1->HandleEvent(event); o2->HandleEvent(event); }
  bool Update(MainLoopEnv &e) {
    float time = e.time*10.0;
    float current_time = time;

    float t = current_time;
    pos = 0.0;
    if (t<start_time) pos = start_val;
    else if (t>=end_time) pos= end_val;
    else {
      float d = t-start_time;
      d/=(end_time-start_time);
      d*=(end_val-start_val);
      d+=start_val;
      pos = d;
    }
    o1->Update(e);
    o2->Update(e);
    return true;
  }
  int NumPoints() const { return std::min(o1->NumPoints(), o2->NumPoints()); }
  Point Pos(int i) const
  {
    Point p1 = o1->Pos(i);
    Point p2 = o2->Pos(i);
    return Point::Interpolate(p1,p2,1.0-pos);
  }
  unsigned int Color(int i) const
  { 
    unsigned int p1 = o1->Color(i);
    unsigned int p2 = o2->Color(i);
    return Color::Interpolate(p1,p2,1.0-pos);
  }
private:
  PointsApiPoints *o1, *o2;
  float start_val, end_val;
  float start_time, end_time;
  float pos;
};
EXPORT GameApi::PTS GameApi::PointsApi::anim_mix(GameApi::PTS obj1, GameApi::PTS obj2, float start_val, float end_val, float start_time, float end_time)
{
  PointsApiPoints *o1 = find_pointsapi_points(e, obj1);
  PointsApiPoints *o2 = find_pointsapi_points(e, obj2);
  return add_points_api_points(e, new AnimMixPTS(o1, o2, start_val, end_val, start_time, end_time));
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
  void Prepare() {pts->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { pts->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return pts->Update(e); }

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

void GameApi::PointsApi::update_from_data(GameApi::PTA pta, GameApi::PTS p)
{
  PointsApiPoints *pts = find_pointsapi_points(e, p);
  int numpoints = pts->NumPoints();

  PointArray3 *arr = find_point_array3(e, pta);
  float *array = arr->array;
  unsigned int *color = arr->color;
  for(int i=0;i<numpoints;i++)
    {
      Point p = pts->Pos(i);
      unsigned int c = pts->Color(i);
      array[i*3+0] = p.x;
      array[i*3+1] = p.y;
      array[i*3+2] = p.z;
      color[i] = swap_color(c);
    }

  
  update(pta);
}
void GameApi::PointsApi::update(GameApi::PTA pta)
{
  PointArray3 *arr = find_point_array3(e, pta);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(float)*3, arr->array);
  glBindBuffer(GL_ARRAY_BUFFER, arr->buffer[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(unsigned int), arr->color);
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
  pts->Prepare();
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
  glDisableVertexAttribArray(vertex_id);
#ifdef VAO
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(2);
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
  { }
  void Prepare() { prepare(); }
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
  glDisableVertexAttribArray(4);
#endif
  
}


class PointApiPointFunction : public PointsApiPoints
{
public:
  PointApiPointFunction(GameApi::Env &e, std::function<GameApi::PT(int pointnum)> f, int numpoints) : e(e), f(f), numpoints(numpoints) { }
  void Prepare() { }
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
  void Prepare() {orig->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { orig->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return orig->Update(e); }

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
  void Prepare() { }
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
  void Prepare() { next->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { next->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { return next->Update(e); }

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
	shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
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
  ev.shader_api.unuse(sh);
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
  Bitmap<bool> *bb = find_bool_bitmap(e, bm)->bitmap;
  bb->Prepare();
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
class StandardBox : public PointsApiPoints
{
public:
  StandardBox(int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : sx(sx), sy(sy), sz(sz), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z) { }
  void Prepare() {}
  int NumPoints() const { return sx*sy*sz; }
  Point Pos(int i) const {
    int x = i/(sy*sz);
    int x_left = i - x*sy*sz;
    int y = x_left/sy;
    int y_left = x_left - y*sy;
    int z = y_left;

    // these are [0..1]
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    float zz = float(z)/sz;

    float pos_x = start_x + xx*(end_x-start_x);
    float pos_y = start_y + yy*(end_y-start_y);
    float pos_z = start_z + zz*(end_z-start_z);
    Point p = { pos_x, pos_y, pos_z };
    return p;
  }
  unsigned int Color(int i) const { return 0xffffffff; }
private:
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};
GameApi::PTS GameApi::PointsApi::standard_box(int sx, int sy, int sz,
					      float start_x, float end_x,
					      float start_y, float end_y,
					      float start_z, float end_z
					      )
{
  return add_points_api_points(e, new StandardBox(sx,sy,sz,
						  start_x, end_x,
						  start_y, end_y,
						  start_z, end_z
						  ));
}
GameApi::PTS GameApi::PointsApi::matrix_points(PTS orig, M matrix)
{
  Matrix m = find_matrix(e, matrix);
  PointsApiPoints *pts = find_pointsapi_points(e, orig);
  return add_points_api_points(e, new MatrixPoints(pts, m));
}
GameApi::LI GameApi::PointsApi::matrix_display(EveryApi &ev, M matrix)
{
  PTS pts = standard_box(10,10,10, -300.0,300.0, -300.0,300.0, -300.0,300.0);
  PTS pts2 = matrix_points(pts, matrix);
  LI li = ev.lines_api.from_points2(pts, pts2);
  return li;
}
GameApi::LI GameApi::PointsApi::matrix2_display(EveryApi &ev, M matrix1, M matrix2, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  PTS pts = standard_box(sx,sy,sz, start_x, end_x, start_y, end_y, start_z,end_z);
  PTS pts1 = matrix_points(pts, matrix1);
  PTS pts2 = matrix_points(pts, matrix2);
  LI li = ev.lines_api.from_points2(pts1, pts2);
  return li;
}
class MovementDisplay : public MainLoopItem
{
public:
  MovementDisplay(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *item, GameApi::MN mn, int count, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : env(env), ev(ev), item(item), mn(mn),sx(sx),sy(sy),sz(sz),start_x(start_x), end_x(end_x),start_y(start_y), end_y(end_y),start_z(start_z), end_z(end_z), count(count)
  {
    prev_time.push_back(0.0);
  }
  virtual void execute(MainLoopEnv &e)
  {
    float start_time = prev_time.front();
    float end_time = e.time;

    float s_time = start_time*10.0;
    float e_time = end_time*10.0;
    GameApi::M mat = ev.move_api.get_matrix(mn, s_time, ev.mainloop_api.get_delta_time());
    GameApi::M mat2 = ev.move_api.get_matrix(mn, e_time, ev.mainloop_api.get_delta_time());
    GameApi::LI li = ev.points_api.matrix2_display(ev, mat, mat2,sx,sy,sz,start_x,end_x,start_y,end_y,start_z,end_z);
    GameApi::LLA lip = ev.lines_api.prepare(li);
    GameApi::ML ml = ev.lines_api.render_ml(ev,lip);
    MainLoopItem *item2 = find_main_loop(env, ml);
    item2->execute(e);
    
    prev_time.push_back(e.time);
    int s = prev_time.size();
    if (s>=count) {
      prev_time.erase(prev_time.begin());
    }
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  GameApi::MN mn;
  std::vector<float> prev_time;
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int count;
};
GameApi::ML GameApi::PointsApi::movement_display(EveryApi &ev, ML ml, MN mn, int count, int sx, int sy, int sz, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e, new MovementDisplay(e,ev, item, mn,count, sx,sy,sz,start_x,end_x,start_y,end_y,start_z,end_z));
}
class MeshQuad : public PointsApiPoints
{
public:
  MeshQuad(FaceCollection *coll, int count) : coll(coll), count(count) { firsttime = true; points=0; color2=0;}
  ~MeshQuad() { delete[] points; delete[] color2; }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) {
    return false; }
  virtual int NumPoints() const { if (points) return points->size(); else { const_cast<MeshQuad*>(this)->Prepare(); return points->size(); } }
  virtual Point Pos(int i) const { if (points) return (*points)[i]; else { const_cast<MeshQuad*>(this)->Prepare(); return (*points)[i]; } }
  virtual unsigned int Color(int i) const { if (color2) return (*color2)[i]; else { const_cast<MeshQuad*>(this)->Prepare(); return (*color2)[i];}  }

  void Prepare()
  {
    coll->Prepare();
    if (!points)
      points = new std::vector<Point>;
    if (!color2)
      color2 = new std::vector<unsigned int>;
    points->clear();
    color2->clear();
    
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
	int num = coll->NumPoints(zpi);
	if (num != 4 && num != 3) { std::cout << "Error quad: " << num << std::endl; }
	if (num==4) {
	  Point p1 = coll->FacePoint(zpi, 0);
	  Point p2 = coll->FacePoint(zpi, 1);
	  Point p3 = coll->FacePoint(zpi, 2);
	  Point p4 = coll->FacePoint(zpi, 3);
	  Point p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(p1) + (1.0f+xp)*(1.0f-yp)*Vector(p2) + (1.0f+xp)*(1.0f+yp)*Vector(p3) + (1.0f-xp)*(1.0f+yp)*Vector(p4));
	  if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	  points->push_back(p);
	  color2->push_back(0xffffffff);
	} else if (num==3)
	  {
	    Point p1 = coll->FacePoint(zpi, 0);
	    Point p2 = coll->FacePoint(zpi, 1);
	    Point p3 = coll->FacePoint(zpi, 2);
	    float r1 = double(r.next())/r.maximum();
	    float r2 = double(r.next())/r.maximum();
	    Point p = Point((1.0-sqrt(r1))*Vector(p1) + (sqrt(r1)*(1.0-r2))*Vector(p2) + (r2*sqrt(r1))*Vector(p3));
	    if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	    points->push_back(p);
	    color2->push_back(0xffffffff);
	    
	  }
      }
    
  }
private:
  FaceCollection *coll;
  int count;
  std::vector<Point> *points;
  std::vector<unsigned int> *color2;
  bool firsttime;
};
GameApi::PTS GameApi::PointsApi::random_mesh_quad_instancing(EveryApi &ev, P p, int count)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_points_api_points(e, new MeshQuad(coll, count));
  /*
  coll->Prepare();
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
      int num = coll->NumPoints(zpi);
      if (num != 4 && num != 3) { std::cout << "Error quad: " << num << std::endl; }
      if (num==4) {
	Point p1 = coll->FacePoint(zpi, 0);
	Point p2 = coll->FacePoint(zpi, 1);
	Point p3 = coll->FacePoint(zpi, 2);
	Point p4 = coll->FacePoint(zpi, 3);
	Point p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(p1) + (1.0f+xp)*(1.0f-yp)*Vector(p2) + (1.0f+xp)*(1.0f+yp)*Vector(p3) + (1.0f-xp)*(1.0f+yp)*Vector(p4));
	if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	points->push_back(p);
	color2->push_back(0xffffffff);
      } else if (num==3)
	{
	Point p1 = coll->FacePoint(zpi, 0);
	Point p2 = coll->FacePoint(zpi, 1);
	Point p3 = coll->FacePoint(zpi, 2);
      float r1 = double(r.next())/r.maximum();
      float r2 = double(r.next())/r.maximum();
	Point p = Point((1.0-sqrt(r1))*Vector(p1) + (sqrt(r1)*(1.0-r2))*Vector(p2) + (r2*sqrt(r1))*Vector(p3));
	if (std::isnan(p.x) || std::isnan(p.y) ||std::isnan(p.z)) continue;
	points->push_back(p);
	color2->push_back(0xffffffff);

	}
    }
   return add_points_api_points(e, new SurfacePoints(points, color2));
  */
}
/*
GameApi::PTS GameApi::PointsApi::random_bitmap_edge_instancing(BB bm, float start_x, float end_x, float start_z, float end_z, float y)
{
}
*/

class PTSGrid : public PointsApiPoints
{
public:
  PTSGrid(Bitmap<::Color> &bm, float start_x, float end_x, float start_y, float end_y, float z) : bm(bm), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { bm.Prepare(); }
  void Prepare() { bm.Prepare(); }
  virtual int NumPoints() const { return bm.SizeX()*bm.SizeY(); }
  virtual Point Pos(int i) const
  {
    int y = i / bm.SizeX();
    int x = i - y*bm.SizeX();
    float yy = float(y)/bm.SizeY();
    float xx = float(x)/bm.SizeX();
    xx*=(end_x-start_x);
    yy*=(end_y-start_y);
    xx+=start_x;
    yy+=start_y;
    Point p(xx,yy,z);
    return p;
  }
  virtual unsigned int Color(int i) const
  {
    int y = i / bm.SizeX();
    int x = i - y*bm.SizeX();
    ::Color c = bm.Map(x,y);
    return c.Pixel();
  }

private:
  Bitmap<::Color> &bm;
  float start_x, end_x, start_y, end_y, z;
};

GameApi::PTS GameApi::PointsApi::pts_grid(BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  return add_points_api_points(e, new PTSGrid(*b2, start_x, end_x, start_y, end_y, z));
}

class PTSGridBB : public PointsApiPoints
{
public:
  PTSGridBB(Bitmap<bool> &bb, float start_x, float end_x, float start_y, float end_y, float z) : bb(bb), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z)
  {
  }
  void Prepare() {bb.Prepare(); collect_pixels(); }
  void collect_pixels()
  {
    int sx = bb.SizeX();
    int sy = bb.SizeY();
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  bool b = bb.Map(x,y);
	  if (b) {
	    vec_x.push_back(x);
	    vec_y.push_back(y);
	  }
	}
  }
  virtual int NumPoints() const { return vec_x.size(); }
  virtual Point Pos(int i) const
  {
    int x = vec_x[i];
    int y = vec_y[i];
    float xx = float(x)/bb.SizeX();
    float yy = float(y)/bb.SizeY();
    xx *= (end_x-start_x);
    yy *= (end_y-start_y);
    xx+=start_x;
    yy+=start_y;
    Point p(xx,yy,z);
    return p;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  Bitmap<bool> &bb;
  std::vector<int> vec_x;
  std::vector<int> vec_y;
  float start_x, end_x, start_y, end_y, z;
};

GameApi::PTS GameApi::PointsApi::pts_grid_bb(BB bb, float start_x, float end_x, float start_y, float end_y, float z)
{
  Bitmap<bool> *bb2 = find_bool_bitmap(e, bb)->bitmap;
  return add_points_api_points(e, new PTSGridBB(*bb2, start_x, end_x, start_y, end_y, z));
}
class SinglePTS : public PointsApiPoints
{
public:
  void Prepare() {}
  virtual int NumPoints() const { return 1; }
  virtual Point Pos(int i) const { Point p; p.x=0.0; p.y=0.0; p.z=0.0; return p; }
  virtual unsigned int Color(int i) const { return 0xffffffff; }
};
GameApi::PTS GameApi::PointsApi::single_pts()
{
  return add_points_api_points(e, new SinglePTS);
}

class LIFromPTS : public LineCollection
{
public:
  LIFromPTS(PointsApiPoints *pk, float dx, float dy, float dz) : pk(pk),dx(dx),dy(dy),dz(dz) { }
  void Prepare() { pk->Prepare(); }
  virtual int NumLines() const { return pk->NumPoints(); }
  virtual Point LinePoint(int line, int point) const
  {
    Point p = pk->Pos(line);
    if (point==1) p=p+Vector(dx,dy,dz);
    return p;
  }
  virtual unsigned int LineColor(int line, int point) const {
    return pk->Color(line);
  }

private:
  PointsApiPoints *pk;
  float dx,dy,dz;
};

GameApi::LI GameApi::PointsApi::li_from_pts(PTS pts, float dx, float dy, float dz)
{
  PointsApiPoints *p1 = find_pointsapi_points(e, pts);
  return add_line_array(e, new LIFromPTS(p1,dx,dy,dz));
}

GameApi::ML GameApi::PointsApi::pts_render(EveryApi &ev, PTS pts)
{
  LI I3=ev.points_api.li_from_pts(pts,1,1,1);
  LLA I4=ev.lines_api.prepare(I3);
  ML I5=ev.lines_api.render_ml(ev,I4);
  return I5;
}

class MemoizePTS : public PointsApiPoints
{
public:
  MemoizePTS(PointsApiPoints *next) : next(next) {}
  void Prepare() { next->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { }
  bool Update(MainLoopEnv &e) { return false; }
  int NumPoints() const { return next->NumPoints(); }
  Point Pos(int i) const
  {
    std::map<int,Point>::const_iterator ii = mymap.find(i);
    if (ii!=mymap.end())
      {
	return (*ii).second;
      }
    Point p = next->Pos(i);
    mymap[i] = p;
    return p;
  }
  unsigned int Color(int i) const
  {
    std::map<int,unsigned int>::const_iterator ii = mymap_color.find(i);
    if (ii!=mymap_color.end())
      {
	return (*ii).second;
      }
    unsigned int p = next->Color(i);
    mymap_color[i] = p;
    return p;

  }
private:
  PointsApiPoints *next;
  mutable std::map<int, Point> mymap;
  mutable std::map<int, unsigned int> mymap_color;
};

GameApi::PTS GameApi::PointsApi::memoize_pts(PTS pts)
{
  PointsApiPoints *p = find_pointsapi_points(e, pts);
  return add_points_api_points(e, new MemoizePTS(p));
}

class CollisionPoints : public PointsApiPoints
{
public:
  CollisionPoints(float start_x, float end_x,
		  float start_y, float end_y,
		  float start_z, float end_z) :
    start_x(start_x), end_x(end_x),
    start_y(start_y), end_y(end_y),
    start_z(start_z), end_z(end_z) {}

  int NumPoints() const { return 8; }
  Point Pos(int i) const
  {
    switch(i) {
    case 0: return Point(start_x, start_y, start_z);
    case 1: return Point(end_x, start_y, start_z);
    case 2: return Point(start_x, end_y, start_z);
    case 3: return Point(start_x, start_y, end_z);
    case 4: return Point(end_x, end_y, start_z);
    case 5: return Point(end_x, start_y, end_z);
    case 6: return Point(start_x, end_y, end_z);
    case 7: return Point(end_x, end_y, end_z);
    };
    return Point(0.0,0.0,0.0);
  }
  unsigned int Color(int i) const { return 0xffffffff; }
private:
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
};
GameApi::PTS GameApi::PointsApi::collision_points(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  return add_points_api_points(e, new CollisionPoints(start_x, end_x, start_y, end_y, start_z, end_z));
}
void check_world(MainLoopEnv &e);

class CollisionBindInst : public MainLoopItem
{
public:
  CollisionBindInst(PointsApiPoints *pts, PointsApiPoints *pts2, std::string name) : pts(pts), pts2(pts2), name(name) {}
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    World *w = e.current_world;
    std::map<std::string, CollisionData*> &coll_map = w->collision_data;
    int ss = pts2->NumPoints();
    for(int j=0;j<ss;j++)
      {

	Point inst_pos = pts2->Pos(j);
	CollisionData *d = new CollisionData;
	int s = pts->NumPoints();
	for(int i=0;i<s;i++)
	  {
	    Point p = pts->Pos(i);
	    Point p2 = p * e.in_MV;
	    p2.x += inst_pos.x;
	    p2.y += inst_pos.y;
	    p2.z += inst_pos.z;
	    d->bounding_box.push_back(p2);
	  }
	std::stringstream ss;
	ss << j;
	std::string name2 = name + ss.str();;
	delete coll_map[name2];
	coll_map[name2]= d;
      }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
private:
  PointsApiPoints *pts, *pts2;
  std::string name;
};

class CollisionBind : public MainLoopItem
{
public:
  CollisionBind(PointsApiPoints *pts, std::string name) : pts(pts),name(name) { }
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    World *w = e.current_world;
    std::map<std::string, CollisionData*> &coll_map = w->collision_data;
    CollisionData *d = new CollisionData;
    int s = pts->NumPoints();
    for(int i=0;i<s;i++)
      {
	Point p = pts->Pos(i);
	Point p2 = p * e.in_MV;
	d->bounding_box.push_back(p2);
      }
    delete coll_map[name];
    coll_map[name] = d;
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual int shader_id() { return -1; }
private:
  PointsApiPoints *pts;
  std::string name;
};
GameApi::ML GameApi::PointsApi::collision_bind(PTS bounding_box, std::string name)
{
  PointsApiPoints *pt = find_pointsapi_points(e, bounding_box);
  return add_main_loop(e, new CollisionBind(pt, name));
}
GameApi::ML GameApi::PointsApi::collision_bind_inst(PTS bounding_box, PTS inst_points, std::string name)
{
  PointsApiPoints *pt1 = find_pointsapi_points(e, bounding_box);
  PointsApiPoints *pt2 = find_pointsapi_points(e, inst_points);
  return add_main_loop(e, new CollisionBindInst(pt1, pt2, name));
}

float AxisPos(Point p1, Point p2, Point p)
{
  Vector v1 = p-p1;
  Vector v2 = p2-p1;
  return Vector::FindProjectionLength(v1,v2);
}

std::pair<float,float> FindMinMaxAxis(Point p1, Point p2, std::vector<Point> vec)
{
  int s = vec.size();
  float Min = 3000000.0;
  float Max = -3000000.0;
  for(int i=0;i<s;i++)
    {
      Point p = vec[i];
      float val = AxisPos(p1,p2,p);
      if (val>Max) Max=val;
      if (val<Min) Min=val;
    }
  return std::make_pair(Min,Max);
}

bool FindCollideSeparated(Point p1, Point p2, std::vector<Point> vec1, std::vector<Point> vec2)
{
  std::pair<float,float> pp1 = FindMinMaxAxis(p1,p2,vec1);
  std::pair<float,float> pp2 = FindMinMaxAxis(p1,p2,vec2);

  if (pp1.first > pp1.second) std::swap(pp1.first,pp1.second);
  if (pp2.first > pp2.second) std::swap(pp2.first,pp2.second);

  if (pp1.first < pp2.first)
    {
      if (pp1.second < pp2.first) { return true; }
    }
  else
    {
      if (pp2.second < pp1.first) { return true; }
    }
  return false;
}

int axis_data[] = { 0,1,
		    1,0,
		    0,2,
		    2,0,
		    0,3,
		    3,0,
		    1,4,
		    4,1,
		    1,5,
		    5,1,
		    2,4,
		    4,2,
		    2,6,
		    6,2,
		    3,5,
		    5,3,
		    3,6,
		    6,3,
		    4,7,
		    7,4,
		    5,7,
		    7,5,
		    6,7,
		    7,6
		   
};


bool Collide(CollisionData *d1, CollisionData *d2)
{
  int s = sizeof(axis_data)/sizeof(int)/2;
  CollisionData *d[2] = { d1,d2 };
  for(int j=0;j<1;j++)
    {
      CollisionData *dd = d[j];
      for(int i=0;i<s;i++)
	{
	  int d1_ = axis_data[i*2];
	  int d2_ = axis_data[i*2+1];
	  Point p1 = dd->bounding_box[d1_];
	  Point p2 = dd->bounding_box[d2_];
	  bool b = FindCollideSeparated(p1,p2,d1->bounding_box, d2->bounding_box);
	  if (b) return false;
	}
    }
  return true;
}

class CollisionCollect : public MainLoopItem
{
public:
  CollisionCollect(MainLoopItem *next) : next(next) {}
  virtual void execute(MainLoopEnv &e)
  {
    check_world(e);
    World *w = e.current_world;
    std::map<std::string, CollisionData*> &coll_map = w->collision_data;
    coll_map.clear();

    next->execute(e);
    //World *w = e.current_world;
    //d::map<std::string, CollisionData*> &coll_map = w->collision_data;
    std::vector<std::pair<std::string,std::string> > &coll_res = w->collisions;
    coll_res.clear();
    int jj = 0;
    std::map<std::string, CollisionData*>::const_iterator j = coll_map.begin();
    for(;j!=coll_map.end();j++,jj++) {
      std::map<std::string, CollisionData*>::const_iterator i = coll_map.begin();
      std::pair<std::string, CollisionData*> p_j = *j;
      int ii = 0;
      for(;i!=coll_map.end();i++,ii++)
	{
	  if (ii>jj) continue;
	  std::pair<std::string, CollisionData*> p_i = *i;
	  bool b = Collide(p_j.second, p_i.second);
	  if (b==true && jj!=ii) {
	    coll_res.push_back(std::make_pair(p_j.first, p_i.first));
	    //std::cout << "Collide: " << p_j.first << " " << p_i.first << std::endl;
	  }
	}
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual int shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};

GameApi::ML GameApi::PointsApi::collision_collect(ML mainloop)
{
  MainLoopItem *next = find_main_loop(e, mainloop);
  return add_main_loop(e, new CollisionCollect(next));
}

class PTArray : public PointsApiPoints
{
public:
  PTArray(std::vector<Point> vec) : vec(vec) {}
  void Prepare() { }
  void HandleEvent(MainLoopEvent &event) { }
  bool Update(MainLoopEnv &e) { return false; }
  int NumPoints() const { return vec.size(); }
  Point Pos(int i) const { return vec[i]; }
  unsigned int Color(int i) const { return 0xffffffff; }
private:
  std::vector<Point> vec;
};

GameApi::PTS GameApi::PointsApi::pt_array(EveryApi &ev, std::vector<PT> vec)
{
  int s = vec.size();
  std::vector<Point> vec2;
  for(int i=0;i<s;i++) {
    vec2.push_back(*find_point(e,vec[i]));
  }
  return add_points_api_points(e, new PTArray(vec2));
}
