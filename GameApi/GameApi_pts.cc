#include "GameApi_h.hh"

#ifndef EMSCRIPTEN
#define VAO 1
#endif

#include "GameApi_low.hh"

class HeightMapPoints : public PointsApiPoints
{
public:
  HeightMapPoints(Bitmap<::Color> &color, Bitmap<float> &height, Point pos, Vector u_x, Vector u_y, Vector u_z, int sx, int sy) : color(color), height(height), pos(pos), u_x(u_x), u_y(u_y), u_z(u_z), sx(sx), sy(sy) { }
  void Collect(CollectVisitor &vis) { color.Collect(vis); height.Collect(vis); }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { pts1->Collect(vis); pts2->Collect(vis); }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { pts->Collect(vis); }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { o1->Collect(vis); o2->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { o1->Prepare(); o2->Prepare(); }
  void HandleEvent(MainLoopEvent &event) { o1->HandleEvent(event); o2->HandleEvent(event); }
  bool Update(MainLoopEnv &e) {
    float time = e.time*10.0;
    float current_time = time;
    m_time = current_time;
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
  int NumPoints() const { 
    if (m_time<start_time ||m_time>=end_time) return 0;
    return std::min(o1->NumPoints(), o2->NumPoints()); }
  Point Pos(int i) const
  {
    Point p1 = o1->Pos(i);
    Point p2 = o2->Pos(i);
    return Point::Interpolate(p1,p2,pos);
  }
  unsigned int Color(int i) const
  { 
    unsigned int p1 = o1->Color(i);
    unsigned int p2 = o2->Color(i);
    return Color::Interpolate(p1,p2,pos);
  }
private:
  PointsApiPoints *o1, *o2;
  float start_val, end_val;
  float start_time, end_time;
  float pos;
  float m_time;
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
  void Collect(CollectVisitor &vis) { pts->Collect(vis); }
  void HeavyPrepare() { }

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
int GameApi::PointsApi::num_points(MSA pta)
{
  MatrixArray3 *arr = find_matrix_array3(e, pta);
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

void GameApi::PointsApi::update_from_data(GameApi::MSA pta, GameApi::MS p)
{
  MatrixArray *pts = find_matrix_array(e, p);
  int numpoints = pts->Size();
  
  MatrixArray3 *arr = find_matrix_array3(e, pta);
  float *array = arr->array;
  unsigned int *color = arr->color;
  for(int i=0;i<numpoints;i++) 
    {
      Matrix m = pts->Index(i);
      float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
			m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
			m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
			m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };
      
      unsigned int c = pts->Color(i);
      for(int j=0;j<16;j++)
	array[i*16+j] = m.matrix[j];
      color[i]=swap_color(c);
    }
  // todo, update vertex array?
}
void GameApi::PointsApi::update_from_data(GameApi::PTA pta, GameApi::PTS p)
{
  PointsApiPoints *pts = find_pointsapi_points(e, p);
  int numpoints = pts->NumPoints();


  PointArray3 *arr = find_point_array3(e, pta);

  if (numpoints>arr->numpoints) {
    float *array = arr->array;
    unsigned int *color = arr->color;
    delete [] array;
    delete [] color;
    arr->array = new float[numpoints*3];
    arr->color = new unsigned int[numpoints];
  }
  arr->numpoints = numpoints;

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
  OpenglLowApi *ogl = g_low->ogl;

  PointArray3 *arr = find_point_array3(e, pta);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  if (arr->numpoints>0)
    ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(float)*3, arr->array);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[1]);
  if (arr->numpoints>0)
    ogl->glBufferSubData(Low_GL_ARRAY_BUFFER, 0, arr->numpoints*sizeof(unsigned int), arr->color);
}


#ifndef ARM

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
#endif

EXPORT GameApi::MSA GameApi::MatricesApi::prepare(GameApi::MS p)
{
  MatrixArray *arr2 = find_matrix_array(e, p);
  arr2->Prepare();
  int num = arr2->Size();
  float *array = new float[num*16];
  unsigned int *color = new unsigned int[num];
  for(int i=0;i<num;i++) {
    Matrix m = arr2->Index(i);
    float mat[16] = { m.matrix[0], m.matrix[4], m.matrix[8], m.matrix[12],
		      m.matrix[1], m.matrix[5], m.matrix[9], m.matrix[13],
		      m.matrix[2], m.matrix[6], m.matrix[10], m.matrix[14],
		      m.matrix[3], m.matrix[7], m.matrix[11], m.matrix[15] };

    unsigned int c = arr2->Color(i);
    for(int j=0;j<16;j++) {
      array[i*16+j] = m.matrix[j];
    }
    color[i] = swap_color(c);
  }
  // note, this isnt done like pts, i.e. its not sent to gpu buffers.
  MatrixArray3 *arr = new MatrixArray3;
  arr->array = array;
  arr->color = color;
  arr->numpoints = num;
  return add_matrix_array3(e,arr);
}

EXPORT GameApi::PTA GameApi::PointsApi::prepare(GameApi::PTS p)
{
  OpenglLowApi *ogl = g_low->ogl;

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
  ogl->glGenVertexArrays(1, &arr->vao[0]);
  ogl->glBindVertexArray(arr->vao[0]);
  const int vertex_id = 0;
#endif
#ifndef VAO
  const int vertex_id = 0;
#endif
  ogl->glGenBuffers(2, &arr->buffer[0]);
  ogl->glEnableVertexAttribArray(vertex_id);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(float)*3, arr->array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[1]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, arr->numpoints*sizeof(unsigned int), arr->color, Low_GL_STATIC_DRAW);
#ifdef VAO
  ogl->glEnableVertexAttribArray(vertex_id);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(vertex_id, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[1]);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0, 0);
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
  ogl->glDisableVertexAttribArray(vertex_id);
#ifdef VAO
  ogl->glDisableVertexAttribArray(4);
  ogl->glDisableVertexAttribArray(2);
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
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }

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
  OpenglLowApi *ogl = g_low->ogl;
  PointArray3 *arr = find_point_array3(e, array);
#ifdef VAO
  ogl->glBindVertexArray(arr->vao[0]);
#endif
#ifndef VAO
  const int vertex_id = 0;
  ogl->glEnableVertexAttribArray(vertex_id);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(vertex_id, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[1]);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0, 0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
  ogl->glDrawArrays(Low_GL_POINTS, 0, arr->numpoints);
#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(2);
  ogl->glDisableVertexAttribArray(4);
#endif
  
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif


}


class PointApiPointFunction : public PointsApiPoints
{
public:
  PointApiPointFunction(GameApi::Env &e, std::function<GameApi::PT(int pointnum)> f, int numpoints) : e(e), f(f), numpoints(numpoints) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { orig->Collect(vis);}
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
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
EXPORT int GameApi::PointsApi::NumPoints(MS p)
{
  MatrixArray *obj2 = find_matrix_array(e, p);
  return obj2->Size();
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

#ifndef ARM

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
  //pthread_exit(NULL);

  return 0;
}
#endif
class ColorPoints : public PointsApiPoints
{
public:
  ColorPoints(PointsApiPoints *next, unsigned int color) : next(next), color(color) { }
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }
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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }
  void execute(MainLoopEnv &e)
  {
  OpenglLowApi *ogl = g_low->ogl;

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
    ogl->glBindVertexArray(arr->vao[0]);
#endif
#ifndef VAO
  const int vertex_id = 0;
  ogl->glEnableVertexAttribArray(vertex_id);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(vertex_id, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[0]);
  ogl->glVertexAttribPointer(4, 3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glEnableVertexAttribArray(2);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, arr->buffer[1]);
  ogl->glVertexAttribPointer(2, 4, Low_GL_UNSIGNED_BYTE, Low_GL_TRUE, 0, 0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

    ogl->glDrawArrays(Low_GL_POINTS, 0, arr->numpoints);

#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(2);
  ogl->glDisableVertexAttribArray(4);
#endif
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif

  ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return std::vector<int>{shader.id}; }
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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) { item->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { item->Prepare(); }
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
    GameApi::ML ml = ev.lines_api.render_ml(ev,lip,1.0);
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
  virtual std::vector<int> shader_id() { return item->shader_id(); }

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
struct Cont {
  int count;
  std::vector<Point> *points;
  std::vector<unsigned int> *colours;
};
std::vector<Cont> g_meshquad_data;
Cont *find_meshquad(int count)
{
  int s = g_meshquad_data.size();
  for(int i=0;i<s;i++) {
    Cont *c = &g_meshquad_data[i];
    if (c->count == count) return c;
  }
  return 0;
}
int meshquad_calculate_id(FaceCollection *coll, int count)
{
  Point p = coll->FacePoint(0,0);
  int val = int(p.x)+int(p.y)+int(p.z);
  return coll->NumFaces()+ count + val;
}


class MeshQuad : public PointsApiPoints
{
public:
  MeshQuad(FaceCollection *coll, int count) : coll(coll), count(count) { firsttime = true; points=0; color2=0;  }
  ~MeshQuad() { /*delete points; delete color2; g_meshquad_data.clear();*/ }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) {
    return false; }
  virtual int NumPoints() const { if (points) return points->size(); else { const_cast<MeshQuad*>(this)->Prepare(); return points->size(); } }
  virtual Point Pos(int i) const { if (points) return (*points)[i]; else { const_cast<MeshQuad*>(this)->Prepare(); return (*points)[i]; } }
  virtual unsigned int Color(int i) const { if (color2) return (*color2)[i]; else { const_cast<MeshQuad*>(this)->Prepare(); return (*color2)[i];}  }

  void Collect(CollectVisitor &vis) { coll->Collect(vis); vis.register_obj(this); }
  void HeavyPrepare() {
    int id = meshquad_calculate_id(coll, count);
    Cont *c = find_meshquad(id);
    if (c) {
      points = c->points;
      color2 = c->colours;
      return;
    }
    if (firsttime) 
      {
	firsttime = false;
    if (!points)
      points = new std::vector<Point>;
    if (!color2)
      color2 = new std::vector<unsigned int>;
    points->clear();
    color2->clear();

    int numfaces = coll->NumFaces();
    
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
	zp*=float(numfaces);
	int zpi = int(zp);
	if (zpi<0) zpi = 0;
	if (zpi>=numfaces) zpi = numfaces-1;
	int num = coll->NumPoints(zpi);
	if (num != 4 && num != 3) { /*std::cout << "Error quad: " << num << std::endl;*/ }
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
    int id = meshquad_calculate_id(coll, count);
    Cont c;
    c.count = id;
    c.points = points;
    c.colours = color2;
    g_meshquad_data.push_back(c);
      }

  }

  void Prepare()
  {
    coll->Prepare();
    int id = meshquad_calculate_id(coll, count);
    Cont *c = find_meshquad(id);
    if (c) {
      points = c->points;
      color2 = c->colours;
      return;
    }
    if (firsttime) 
      {
	firsttime = false;
    if (!points)
      points = new std::vector<Point>;
    if (!color2)
      color2 = new std::vector<unsigned int>;
    points->clear();
    color2->clear();

    int numfaces = coll->NumFaces();
    
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
	zp*=float(numfaces);
	int zpi = int(zp);
	if (zpi<0) zpi = 0;
	if (zpi>=numfaces) zpi = numfaces-1;
	int num = coll->NumPoints(zpi);
	if (num != 4 && num != 3) { /*std::cout << "Error quad: " << num << std::endl;*/ }
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
    int id = meshquad_calculate_id(coll, count);
    Cont c;
    c.count = id;
    c.points = points;
    c.colours = color2;
    g_meshquad_data.push_back(c);
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
}

class PTSGrid : public PointsApiPoints
{
public:
  PTSGrid(Bitmap<::Color> &bm, float start_x, float end_x, float start_y, float end_y, float z) : bm(bm), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), z(z) { bm.Prepare(); }
  void Collect(CollectVisitor &vis) {
    bm.Collect(vis);
  }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) {
    bb.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { collect_pixels(); }

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
  void Collect(CollectVisitor &vis) {
  }
  void HeavyPrepare() { }

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
  void Collect(CollectVisitor &vis) {
    pk->Collect(vis);
  }
  void HeavyPrepare() { }

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
  ML I5=ev.lines_api.render_ml(ev,I4,1.0);
  return I5;
}

class MemoizePTS : public PointsApiPoints
{
public:
  MemoizePTS(PointsApiPoints *next) : next(next) {}
  void Collect(CollectVisitor &vis) {
    next->Collect(vis);
  }
  void HeavyPrepare() { }

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

  void Collect(CollectVisitor &vis) {
  }
  void HeavyPrepare() { }

  
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
  void Collect(CollectVisitor &vis) {
  }
  void HeavyPrepare() { }
  void Prepare() { }
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
  void Collect(CollectVisitor &vis) {
  }
  void HeavyPrepare() { }

  void Prepare() { }
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
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  PointsApiPoints *pts;
  std::string name;
};
GameApi::ML GameApi::PointsApi::collision_bind(EveryApi &ev,PTS bounding_box, std::string name)
{
  PointsApiPoints *pt = find_pointsapi_points(e, bounding_box);
  GameApi::ML ml = add_main_loop(e, new CollisionBind(pt, name));
#ifndef EMSCRIPTEN
  GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
  GameApi::P I2=ev.polygon_api.sphere(I1,5,8,8);
  GameApi::ML I3=ev.materials_api.render_instanced_ml(ev,I2,bounding_box);
#endif
  GameApi::ML res = ev.mainloop_api.array_ml(ev,{ml
#ifndef EMSCRIPTEN
	,I3
#endif
	});

  return res;
}
GameApi::ML GameApi::PointsApi::collision_bind_inst(EveryApi &ev, PTS bounding_box, PTS inst_points, std::string name)
{
  PointsApiPoints *pt1 = find_pointsapi_points(e, bounding_box);
  PointsApiPoints *pt2 = find_pointsapi_points(e, inst_points);
  GameApi::ML ml = add_main_loop(e, new CollisionBindInst(pt1, pt2, name));

#ifndef EMSCRIPTEN
  GameApi::PT I1=ev.point_api.point(0.0,0.0,0.0);
  GameApi::P I2=ev.polygon_api.sphere(I1,5,8,8);
  GameApi::P I2a=ev.polygon_api.static_instancing(ev,I2,bounding_box);
  GameApi::ML I3=ev.materials_api.render_instanced_ml(ev,I2a,inst_points);
#endif
  GameApi::ML res = ev.mainloop_api.array_ml(ev,{ml
#ifndef EMSCRIPTEN
	,I3
#endif
	});
  return res;
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
  void Collect(CollectVisitor &vis) { next->Collect(vis); }
  void HeavyPrepare() { }

  void Prepare() {next->Prepare(); }
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
  virtual std::vector<int> shader_id() { return next->shader_id(); }
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
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

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

class LIPTS : public PointsApiPoints
{
public:
  LIPTS(LineCollection *coll, float pos) : coll(coll),pos(pos) { }
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  virtual void Prepare() { coll->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return coll->NumLines();
  }
  virtual Point Pos(int i) const
  {
    Point p1 = coll->LinePoint(i,0);
    Point p2 = coll->LinePoint(i,1);
    return Point::Interpolate(p1,p2,pos);
  }
  virtual unsigned int Color(int i) const
  {
    unsigned int c1 = coll->LineColor(i,0);
    unsigned int c2 = coll->LineColor(i,1);
    return Color::Interpolate(c1,c2,pos);
  }
private:
  LineCollection *coll;
  float pos;
};

GameApi::PTS GameApi::PointsApi::li_pts(LI li, float pos)
{
  LineCollection *lines = find_line_array(e,li);
  return add_points_api_points(e, new LIPTS(lines, pos));
}
GameApi::PTS GameApi::PointsApi::li_pts2(LI li)
{
  PTS p1 = li_pts(li,0.0f);
  PTS p2 = li_pts(li,1.0f);
  return or_points(p1,p2);
}

class CompareCC {
public:
  CompareCC(PointsApiPoints *p, MainLoopEnv *e) : p(p), e(e) {}
  bool operator()(int p1, int p2) 
  {
    Point pp1 = p->Pos(p1);
    Point pp2 = p->Pos(p2);
    Matrix m = e->in_MV;
    pp1 = pp1*m;
    pp2 = pp2*m;
    return pp1.z < pp2.z;
  }
private:
  PointsApiPoints *p;
  MainLoopEnv *e;
  MainLoopItem *item;
};

class SortPTS : public PointsApiPoints
{
public:
  SortPTS(PointsApiPoints *pts) : pts(pts) { }
  void Collect(CollectVisitor &vis) { pts->Collect(vis); }
  void HeavyPrepare() { }

  virtual void Prepare() { pts->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { pts->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) { 
    vec = std::vector<int>();
    int s = pts->NumPoints();
    for(int i=0;i<s;i++) vec.push_back(i);
    CompareCC c(pts,&e);
    std::sort(vec.begin(),vec.end(),c);

    return pts->Update(e); 
  }
  virtual int NumPoints() const
  {
    return pts->NumPoints();
  }
  virtual Point Pos(int i) const
  {
    return pts->Pos(vec[i]);
  }
  virtual unsigned int Color(int i) const
  {
    return pts->Color(vec[i]);
  }

private:
  PointsApiPoints *pts;
  std::vector<int> vec;
};

GameApi::PTS GameApi::PointsApi::sort_pts(PTS points)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, points);
  return add_points_api_points(e, new SortPTS(obj2));
}

class HemiSphere_points : public PointsApiPoints
{
public:
  HemiSphere_points(Point p, Vector n, float r, int numpoints) : p(p), n(n), r(r), numpoints(numpoints) { }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }

  void Prepare() {
    vec=std::vector<Point>();
    int s = numpoints;
    for(int i=0;i<s;i++)
      {
	Random rr;
	float xp = double(rr.next())/rr.maximum();
	float yp = double(rr.next())/rr.maximum();
	xp*=2.0*3.14159;
	yp*=3.14159;
	float angle = yp;
	float angle2 = xp;
	float x = r*sin(angle)*cos(angle2);
        float y = r*sin(angle)*sin(angle2);
	float z = r*cos(angle);
	Point p = { x,y,z };
	
	float d = Vector::DotProduct(Vector(p),n);
	if (d<0) { p.x=-p.x; p.y=-p.y; p.z=-p.z; }
	
	vec.push_back(p);
      }
  }
  int NumPoints() const { return numpoints; }
  Point Pos(int i) const {
    if (i<0||i>=vec.size()) return Point(0.0,0.0,0.0);
    return vec[i];
  }
  unsigned int Color(int i) const { return 0xffffffff; }
private:
  Point p;
  Vector n;
  float r;
  int numpoints;
  std::vector<Point> vec;
};

GameApi::PTS GameApi::PointsApi::hemisphere_points(PT points, V normal, float r, int numpoints)
{
  Point *pt = find_point(e,points);
  Vector *vt = find_vector(e,normal);
  return add_points_api_points(e, new HemiSphere_points(*pt,*vt,r,numpoints));
}

class FaceCenterPoints2 : public PointsApiPoints
{
public:
  FaceCenterPoints2(FaceCollection *coll) : coll(coll) { }
  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }

  virtual void Prepare() { coll->Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return coll->NumFaces();
  }
  virtual Point Pos(int i) const
  {
    int k = coll->NumPoints(i);
    if (k==3)
      { // triangle
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,1);
	Point p3 = coll->FacePoint(i,2);
	Point p = p1+Vector(p2)+Vector(p3);
	p.x/=3.0;
	p.y/=3.0;
	p.z/=3.0;
	return p;
      }
    else if (k==4)
      { // quad
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,1);
	Point p3 = coll->FacePoint(i,2);
	Point p4 = coll->FacePoint(i,3);
	Point p = p1+Vector(p2)+Vector(p3)+Vector(p4);
	p.x/=4.0;
	p.y/=4.0;
	p.z/=4.0;
	return p;

      }
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int Color(int i) const
  {
    int k = coll->NumPoints(i);
    if (k==3)
      { // triangle
	unsigned int p1 = coll->Color(i,0);
	unsigned int p2 = coll->Color(i,1);
	unsigned int p3 = coll->Color(i,2);
	::Color c1(p1);
	::Color c2(p2);
	::Color c3(p3);
	c1*=1.0/3.0;
	c2*=1.0/3.0;
	c3*=1.0/3.0;
	return (c1+c2+c3).Pixel();
      }
    else if (k==4)
      { // quad

	unsigned int p1 = coll->Color(i,0);
	unsigned int p2 = coll->Color(i,1);
	unsigned int p3 = coll->Color(i,2);
	unsigned int p4 = coll->Color(i,3);
	::Color c1(p1);
	::Color c2(p2);
	::Color c3(p3);
	::Color c4(p4);
	c1*=1.0/4.0;
	c2*=1.0/4.0;
	c3*=1.0/4.0;
	c4*=1.0/4.0;
	return (c1+c2+c3+c4).Pixel();

      }
    return 0x00;

  }
private:
  FaceCollection *coll;
};

GameApi::PTS GameApi::PointsApi::polygon_face_center_points(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_points_api_points(e, new FaceCenterPoints2(coll));
}

extern bool g_restart_ongoing;

class Bullet : public PointsApiPoints
{
public:
  Bullet(int key, float vx, float vy, float vz, Point px, float dist, int max_count, float repeat_time) : key(key), pt(vx,vy,vz), px(px), dist(dist), max_count(max_count), repeat_time(repeat_time) 
  { 
    arr = new Point[max_count];
    //inverse = new Matrix[max_count];
    in_MV = new Matrix[max_count];
    for(int i=0;i<max_count;i++) {
      arr[i]=Point(-20000.0,0.0,0.0);
      //inverse[i]=Matrix::Identity();
      in_MV[i]=Matrix::Identity();
    }
    lock=false;
    changed=false;
    current_time = 0.0;
  }

  int find_free() const
  {
    for(int i=0;i<max_count;i++) {
      if (arr[i].x<-10000.0) return i;
    }
    return -1;
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void HandleEvent(MainLoopEvent &event) { 
    if (g_restart_ongoing) {
      for(int i=0;i<max_count;i++) arr[i]=Point(-20000.0,0.0,0.0);
      lock=false;
      current_time = 0.0;
      changed=true;
    }

    int ch = event.ch;
#ifdef EMSCRIPTEN
    if (ch>=4 && ch<=29) { ch = ch - 4; ch=ch+'a'; }
    if (ch==39) { ch='0'; }
    if (ch>=30 && ch<=38) { ch = ch-30; ch=ch+'1'; }
#endif
    if (event.type==768 && ch==key && lock==false)
      {
        lock=true;
        int val = find_free();
        if (val!=-1) { arr[val]=px; in_MV[val]=mv; }
      }
    if (event.type==769 && ch==key) lock=false;
  }
  virtual bool Update(MainLoopEnv &e) { 
    inv = Matrix::Inverse(e.in_MV);
    mv = e.in_MV;

    if (lock) {
      current_time+=e.delta_time;
      if (current_time > repeat_time && repeat_time>0.01) {
	int val = find_free();
	if (val!=-1) { arr[val]=px; in_MV[val]=mv; current_time = 0.0; }
      }
    }

    bool update = false;
    for(int i=0;i<max_count;i++) {
      if (arr[i].x>=-10000.0) { 

      arr[i]+=pt*e.delta_time; update=true; 
      if ((arr[i]-px).Dist()>dist) { arr[i]=Point(-20000.0,0.0,0.0); }
  }
    }
    if (changed) { changed=false; return true; }
    return update; 
  }
  virtual int NumPoints() const { return max_count; }
  virtual Point Pos(int i) const
  {
    return arr[i] * in_MV[i] * inv;
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }

private:
  int key;
  Vector pt;
  Point px;
  float dist;
  int max_count;
  Point *arr;
  bool lock;
  float current_time;
  float repeat_time;
  Matrix *in_MV;
  //Matrix *inverse;
  Matrix inv;
  Matrix mv;
  bool changed;
};

GameApi::PTS GameApi::PointsApi::bullet(int key, float vx, float vy, float vz, float px, float py, float pz,float dist, int max_count, float repeat_time)
{
  return add_points_api_points(e, new Bullet(key,vx,vy,vz,Point(px,py,pz),dist,max_count,repeat_time/100.0));
}


class IteratePoints : public PointsApiPoints
{
public:
  IteratePoints(PointsApiPoints *pts, FloatArray2 *speed, VelocityField *field) : pts(pts), speed(speed), field(field) { cache = 0; numpoints =0;}
  void Collect(CollectVisitor &vis) {
    pts->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    numpoints = std::min(pts->NumPoints(),speed->Num());
    if (numpoints) 
      cache = new Point[numpoints];
    else
      cache = 0;
    for(int i=0;i<numpoints;i++)
      cache[i]=pts->Pos(i);

  }
  virtual void Prepare() {
    pts->Prepare();
    numpoints = std::min(pts->NumPoints(),speed->Num());
    if (numpoints) 
      cache = new Point[numpoints];
    else
      cache = 0;
    for(int i=0;i<numpoints;i++)
      cache[i]=pts->Pos(i);
  }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) {
    if (cache) {
      int s = numpoints;
      for(int i=0;i<s;i++) {
	Point p = cache[i];
	Vector v = field->Map(p);
	//std::cout << "IteratePoints:" << cache[i] << "::" << speed->Map(i) << "::" << v << std::endl;
	cache[i] = cache[i] + speed->Map(i) * v;
      }
    }
    return true;
  }
  virtual int NumPoints() const { return numpoints; }
  virtual Point Pos(int i) const {
    if (cache && i>=0 && i<numpoints)
      return cache[i];
    else
      {
	Point p = { 0.0, 0.0, 0.0 };
	return p;
      }
  }
  virtual unsigned int Color(int i) const
  {
    return 0xffffffff;
  }
private:
  PointsApiPoints *pts;
  VelocityField *field;
  FloatArray2 *speed;
  Point *cache;
  int numpoints;
};

GameApi::PTS GameApi::PointsApi::iterate_points(PTS points, VFi field, FA speed)
{
  FloatArray2 *fa = find_float_array2(e, speed);
  PointsApiPoints *pts = find_pointsapi_points(e,points);
  VelocityField *vfi = find_velocity_field(e,field);
  return add_points_api_points(e, new IteratePoints(pts,fa,vfi));
}
class RandomSpeeds : public FloatArray2
{
public:
  RandomSpeeds(float start_speed, float end_speed, int num) : start_speed(start_speed), end_speed(end_speed), num(num) {

    array = new float[num];
    for(int i=0;i<num;i++) {
    Random r;
    float xp = double(r.next())/r.maximum();
    //std::cout << "RandomSpeeds:" << xp << std::endl;
    array[i]=start_speed + xp*(end_speed-start_speed);
    }
  }
  int Num() const { return num; }
  float Map(int i) const
  {
    if (i>=0 && i<num)
      return array[i];
    else
      return 0.0;
  }
private:
  float start_speed, end_speed;
  int num;
  float *array;
};
GameApi::FA GameApi::PointsApi::random_speeds(float start_speed, float end_speed, int num)
{
  return add_float_array2(e, new RandomSpeeds(start_speed,end_speed,num));
}

class PressureGradient : public VelocityField
{
public:
  PressureGradient(FloatField *pressure) : pressure(pressure) { }

  Vector Map(Point p) const
  {
    float delta = 0.01;
    Point p_x = p + Vector(delta,0.0,0.0);
    Point p_y = p + Vector(0.0,delta,0.0);
    Point p_z = p + Vector(0.0,0.0,delta);
    float f = pressure->Map(p);
    float f_x = pressure->Map(p_x);
    float f_y = pressure->Map(p_y);
    float f_z = pressure->Map(p_z);
    return Vector((f_x-f)/delta,(f_y-f)/delta,(f_z-f)/delta);
  }
private:
  FloatField *pressure;
};

GameApi::VFi GameApi::PointsApi::pressure_gradient(FFi pressure)
{
  FloatField *field = find_float_field(e,pressure);
  return add_velocity_field(e, new PressureGradient(field));
}

class MatrixField : public VelocityField
{
public:
  MatrixField(Movement *start, Movement *end) : start(start),end(end) { }
  Vector Map(Point p) const {
    Matrix m_start = start->get_whole_matrix(0.0, 1.0);
    Matrix m_end = end->get_whole_matrix(0.0,1.0);
    Point p_start = p * m_start;
    Point p_end = p * m_end;
    return p_end-p_start;
  }
private:
  Movement *start;
  Movement *end;
};

GameApi::VFi GameApi::PointsApi::matrix_field(MN start, MN end)
{
  Movement *st = find_move(e,start);
  Movement *en = find_move(e,end);
  return add_velocity_field(e, new MatrixField(st,en));
}

class PlyLoader : public CollectInterface
{
public:
  PlyLoader(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url), homepage(homepage) { }
  float get_float(std::stringstream &ss, std::string type)
  {
    if (type=="float") {
      char ch[4];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
	ss.get(ch[2]);
	ss.get(ch[3]);
      } else {
	ss.get(ch[3]);
	ss.get(ch[2]);
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      float *ptr = (float*)&ch[0];
      return *ptr;
    }
    if (type=="double") {
      char ch[8];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
	ss.get(ch[2]);
	ss.get(ch[3]);
	ss.get(ch[4]);
	ss.get(ch[5]);
	ss.get(ch[6]);
	ss.get(ch[7]);
      } else {
	ss.get(ch[7]);
	ss.get(ch[6]);
	ss.get(ch[5]);
	ss.get(ch[4]);
	ss.get(ch[3]);
	ss.get(ch[2]);
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      double *ptr = (double*)&ch[0];
      return float(*ptr);
    }
    return 0.0f;
  }
  int get_int(std::stringstream &ss, std::string type)
  {
    if (type=="char") {
      char ch;
      ss.get(ch);
      return int(ch);
    }
    if (type=="uchar") {
      char ch;
      ss.get(ch);
      return int(ch);
    }
    if (type=="short") {
      char ch[2];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
      } else {
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      short *sh = (short*)&ch[0];
      return int(*sh);
    }
    if (type=="ushort") {
      char ch[2];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
      } else {
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      unsigned short *sh = (unsigned short *)&ch[0];
      return int(*sh);
    }
    if (type=="int") {
      char ch[4];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
	ss.get(ch[2]);
	ss.get(ch[3]);
      } else {
	ss.get(ch[3]);
	ss.get(ch[2]);
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      int *sh = (int*)&ch[0];
      return int(*sh);
    }
    if (type=="uint") {
      char ch[4];
      if (!is_big_endian) {
	ss.get(ch[0]);
	ss.get(ch[1]);
	ss.get(ch[2]);
	ss.get(ch[3]);
      } else {
	ss.get(ch[3]);
	ss.get(ch[2]);
	ss.get(ch[1]);
	ss.get(ch[0]);
      }
      unsigned int *sh = (unsigned int*)&ch[0];
      return int(*sh);
    }
    std::cout << "Unknown type in .ply file" << std::endl;
    return 0;
  }
  std::basic_istream<char> &getline(std::stringstream &ss, std::string &line)
  {
    if (!is_binary) return std::getline(ss,line);
    return ss;     
  }
  class stringstream {
  public:
    stringstream(PlyLoader &ref, std::stringstream *str2, std::string s, bool is_binary) : ref(ref), is_binary(is_binary) {
      if (is_binary) {
	str = str2;
      } else {
	str = new std::stringstream(s);
      }
    }
    stringstream &get(float &f, std::string type) {
      if (is_binary) {
	f = ref.get_float(*str, type);
      } else {
	str->operator>>(f);
      }
      return *this;
    }
    stringstream &get(int &i, std::string type) {
      if (is_binary) {
	i = ref.get_int(*str,type);
      } else {
	str->operator>>(i);
      }
      return *this;
    }
    explicit operator bool() { return !str->fail(); }
    std::stringstream *str;
    PlyLoader &ref;
    bool is_binary;
  };
  void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  
  void Prepare()
  {
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string code(vec->begin(), vec->end());
      std::stringstream ss2(code);
      std::string line;
      std::string current_section="";
      while(std::getline(ss2,line)) {
	std::stringstream ss(line);
	std::string key;
	ss >> key;
	if (key=="ply") continue;
	if (key=="format") {
	  std::string format;
	  ss >> format;
	  if (format=="ascii") { is_binary=false; }
	  else if (format=="binary_little_endian") {
	    is_binary = true; is_big_endian=false;
	  }
	  else if (format=="binary_big_endian") {
	    is_binary = true; is_big_endian=true;
	  } else { std::cout << ".ply unknown format" << std::endl; return; }
	}
	if (key=="comment") continue;
	if (key=="element") {
	  std::string name;
	  int count;
	  ss >> name >> count;
	  sections.push_back(name);
	  sections_count.push_back(count);
	  current_section = name;
	  properties[current_section] = new std::vector<std::string>;
	  properties_type1[current_section] = new std::vector<std::string>;
	  properties_type2[current_section] = new std::vector<std::string>;
	  properties_is_list[current_section] = new std::vector<bool>;
	  floats[current_section] = new std::map<std::string,std::vector<float>*>;
	  ints[current_section] = new std::map<std::string,std::vector<int>*>;
	  int_lists[current_section] = new std::map<std::string,std::vector<std::vector<int>*>*>;
	  float_lists[current_section] = new std::map<std::string,std::vector<std::vector<float>*>*>;
	  continue;
	}
	if (key=="property") {
	  std::string type1="";
	  std::string type2="";
	  std::string name;
	  bool is_list = false;
	  ss >> type1;
	  if (type1=="list") {
	    ss >> type1 >> type2;
	    is_list = true;
	  } else {
	    if (type1=="char" || type1=="uchar" || type1=="short" || type1=="ushort" || type1=="int" || type1=="uint")
	      {
		type2="int";
	      } else {
	        if (type1=="float" || type1=="double") { type2="float"; }
	      }
	  }
	  ss >> name;
	  if (!is_list && type2=="float") {
	    (*floats[current_section])[name] = new std::vector<float>;
	  }
	  if (!is_list && type2=="int") {
	    (*ints[current_section])[name] = new std::vector<int>;
	  }
	  if (is_list && (type2=="float"||type2=="double")) {
	    (*float_lists[current_section])[name] = new std::vector<std::vector<float>*>;
	  } else {
	    (*int_lists[current_section])[name] = new std::vector<std::vector<int>*>;
	  }
	  
	  properties[current_section]->push_back(name);
	  properties_type1[current_section]->push_back(type1);
	  properties_type2[current_section]->push_back(type2);
	  properties_is_list[current_section]->push_back(is_list);
	}
	if (key=="end_header")
	  {
	    break;
	  }
      }
      int curr_sec=0;
      int current_line=0;
      int target_line=sections_count[curr_sec];
      if (sections.size()>0)
	current_section = sections[0];
      while(getline(ss2,line)) {
	stringstream ss(*this, &ss2,line,is_binary);
	std::vector<std::string>* prop_vec = properties[current_section];
	std::vector<std::string>* type1_vec = properties_type1[current_section];
	std::vector<std::string>* type2_vec = properties_type2[current_section];
	std::vector<bool>* is_list_vec = properties_is_list[current_section];
	int s = prop_vec->size();
	for(int i=0;i<s;i++) {
	  std::string prop_name = (*prop_vec)[i];
	  std::string type1 = (*type1_vec)[i];
	  std::string type2 = (*type2_vec)[i];
	  bool is_list = (*is_list_vec)[i];
	  if (!is_list) {
	    /* is_list=false */
	    if (type2=="float") {
	      float value = 0.0;
	      if (ss.get(value,type1)) {
		(*floats[current_section])[prop_name]->push_back(value);
	      } else {
		std::cout << ".ply file broken, not enough data in " << prop_name << std::endl;
	      }
	    }
	    if (type2=="int") {
	      int value = 0;
	      if (ss.get(value,type1)) {
		(*ints[current_section])[prop_name]->push_back(value);
	      } else {
		std::cout << ".ply file broken, not enough data in " << prop_name << std::endl;
	      }
	    }
	  } else
	    {
	      /* is_list=true */
	      int count = 0;
	      ss.get(count,type1);
	      //std::cout << "Got count=" << count << std::endl;
	      if (type2=="float"||type2=="double") {
		std::vector<float> *vec = new std::vector<float>;
		(*float_lists[current_section])[prop_name]->push_back(vec);
	      for(int i=0;i<count;i++) {
		if (type2=="float"||type2=="double") {
		  float value=0;
		  if (ss.get(value,type2))
		    vec->push_back(value);
		  else
		    {
		      std::cout << ".ply file broken/not enough data in a list" << type1 << " " << type2 << std::endl;
		    }
		} else
		  {
		    std::cout << "int lists are not supported in .ply loader" << std::endl;
		    return;
		  }

	      }
	      } else {
		std::vector<int> *vec = new std::vector<int>;
		(*int_lists[current_section])[prop_name]->push_back(vec);
		for(int i=0;i<count;i++) {
		  if (type2=="char"||type2=="uchar"||type2=="short"||type2=="ushort"||type2=="int"||type2=="uint") {
		    int value=0;
		    if (ss.get(value,type2))
		      vec->push_back(value);
		    else
		      {
			std::cout << ".ply file broken/not enough data in a list" << type1 << " " << type2<< std::endl;
		      }
		  } else
		    {
		      std::cout << "Float lists are not supported in .ply loader" << std::endl;
		      return;
		    }
		}
	      }
	    }
	}
	current_line++;
	if (current_line == target_line) {
	  curr_sec++;
	  current_line = 0;
	  if (curr_sec<sections_count.size()) {
	    target_line = sections_count[curr_sec];
	    current_section=sections[curr_sec];
	  }
	  else
	    break;
	}
      }
  }
      
public:
  GameApi::Env &e;
  std::string url, homepage;
  std::vector<std::string> sections;
  std::vector<int> sections_count;
  bool is_binary;
  bool is_big_endian;
  mutable std::map<std::string, std::vector<std::string>*> properties;
  mutable std::map<std::string, std::vector<std::string>*> properties_type1;
  mutable std::map<std::string, std::vector<std::string>*> properties_type2;
  mutable std::map<std::string, std::vector<bool>*> properties_is_list;
  mutable std::map<std::string,std::map<std::string,std::vector<float>* >*> floats;
  mutable std::map<std::string,std::map<std::string,std::vector<int>* >*> ints;
  mutable std::map<std::string, std::map<std::string,std::vector<std::vector<float>*>*>*> float_lists;
  mutable std::map<std::string, std::map<std::string,std::vector<std::vector<int>*>*>*> int_lists;
};

class PlyPTS : public PointsApiPoints
{
public:
  PlyPTS(GameApi::Env &e, std::string url, std::string homepage) : load(e,url,homepage) { }
  void Collect(CollectVisitor &vis) { load.Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { load.Prepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    std::map<std::string,std::vector<float>*> *mymap = load.floats["vertex"];
    if (!mymap) return 0;
    std::vector<float>* xvec = mymap->operator[]("x");
    if (!xvec) return 0;
    return xvec->size();
  }
  virtual Point Pos(int i) const
  {
    std::map<std::string,std::vector<float>*> *mymap = load.floats["vertex"];
    if (!mymap) return Point(0.0,0.0,0.0);
    std::vector<float>* xvec = mymap->operator[]("x");
    std::vector<float>* yvec = mymap->operator[]("y");
    std::vector<float>* zvec = mymap->operator[]("z");
    if (!xvec||!yvec||!zvec) return Point(0.0,0.0,0.0);
    float x = xvec->operator[](i);
    float y = yvec->operator[](i);
    float z = zvec->operator[](i);
    return Point(x,y,z);
  }
  virtual unsigned int Color(int i) const
  {
    std::map<std::string,std::vector<int>*> *mymap = load.ints["vertex"];
    if (!mymap) { /*std::cout << "mymap not found!" << std::endl;*/ return 0xff000000; }
    std::vector<int> *rvec = mymap->operator[]("red");
    std::vector<int> *gvec = mymap->operator[]("green");
    std::vector<int> *bvec = mymap->operator[]("blue");
    if (!rvec||!gvec||!bvec) { /*std::cout << "Colours not found!" << std::endl;*/ return 0xff000000; }
    int r = rvec->operator[](i);
    int g = gvec->operator[](i);
    int b = bvec->operator[](i);
    unsigned int color = 0xff000000 + (r<<16) + (g << 8) + b;
    return color;
  }

private:
  PlyLoader load;
};

extern std::string gameapi_homepageurl;

GameApi::PTS GameApi::PointsApi::ply_pts(std::string url)
{
  return add_points_api_points(e, new PlyPTS(e,url,gameapi_homepageurl));
}

class PlyFaceCollection : public FaceCollection
{
public:
  PlyFaceCollection(GameApi::Env &e, std::string url, std::string homepage) : load(e,url,homepage) { }
  void Collect(CollectVisitor &vis) { load.Collect(vis); }
  void HeavyPrepare() { }
  virtual void Prepare() { load.Prepare(); }
  virtual int NumFaces() const
  {
    std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) { std::cout << "numfaces:mymap" << std::endl; return 0; }
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *vec2 = mymap->operator[]("vertex_indices");
    if (!vec&&!vec2) { std::cout << "numfaces:vec" << std::endl; return 0; }
    if (vec)
      return vec->size();
    else
      return vec2->size();
  }
  virtual int NumPoints(int face) const
  {
    std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) { std::cout << "numpoints:mymap" << std::endl; return 0; }
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *vec2 = mymap->operator[]("vertex_indices");
    if (!vec&&!vec2) { std::cout << "numpoints:vec" << std::endl; return 0;}
    if (vec)
      return vec->operator[](face)->size();
    else return vec2->operator[](face)->size();
  }
  virtual Point FacePoint(int face, int point) const
  {
    std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) { std::cout << "No mymap" << std::endl; return Point(0.0,0.0,0.0); }
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *veca = mymap->operator[]("vertex_indices");
    if (!vec&&!veca) { std::cout << "no vec" << std::endl; return Point(0.0,0.0,0.0); }
    if (!(face>=0 && face<std::max(vec?vec->size():0,veca?veca->size():0))) { std::cout << "face index" << std::endl; return Point(0.0,0.0,0.0); }
    int vertex = 0;
    if (vec) vertex = vec->operator[](face)->operator[](point);
    if (veca) vertex = veca->operator[](face)->operator[](point);
    
    std::map<std::string,std::vector<float>* > *vmap = load.floats["vertex"];
    std::vector<float> *xvec = vmap->operator[]("x");
    std::vector<float> *yvec = vmap->operator[]("y");
    std::vector<float> *zvec = vmap->operator[]("z");
    if (!xvec||!yvec||!zvec) { std::cout << "xyzvec problem" << std::endl; return Point(0.0,0.0,0.0); }
    float x = xvec->operator[](vertex);
    float y = yvec->operator[](vertex);
    float z = zvec->operator[](vertex);
    std::cout << "Point:" << x << " " << y << " " << z << std::endl;
    return Point(x,y,z);
  }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) return Point(0.0,0.0,0.0);
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *veca = mymap->operator[]("vertex_indices");
    if (!vec&&!veca) return Point(0.0,0.0,0.0);
    
    if (!(face>=0 && face<std::max(vec?vec->size():0,veca?veca->size():0))) return Point(0.0,0.0,0.0); 
    int vertex = 0;
    if (vec) vertex = vec->operator[](face)->operator[](point);
    if (veca) vertex = veca->operator[](face)->operator[](point);
    std::map<std::string,std::vector<float>* > *vmap = load.floats["vertex"];
    std::vector<float> *nxvec = vmap->operator[]("nx");
    std::vector<float> *nyvec = vmap->operator[]("ny");
    std::vector<float> *nzvec = vmap->operator[]("nz");
    if (!nxvec||!nyvec||!nzvec) return Vector(0.0,0.0,0.0);
    float nx = nxvec->operator[](vertex);
    float ny = nyvec->operator[](vertex);
    float nz = nzvec->operator[](vertex);
    return Vector(nx,ny,nz);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
        std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) return 0xffffffff;
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *veca = mymap->operator[]("vertex_indices");
    if (!vec&&!veca) return 0xffffffff;
    if (!(face>=0 && face<std::max(vec?vec->size():0,veca?veca->size():0))) return 0xffffffff;
    int vertex = 0;
    if (vec) vertex=vec->operator[](face)->operator[](point);
    if (veca) vertex=veca->operator[](face)->operator[](point);
    std::map<std::string,std::vector<int>* > *vmap = load.ints["vertex"];
    std::vector<int> *redvec = vmap->operator[]("red");
    std::vector<int> *greenvec = vmap->operator[]("green");
    std::vector<int> *bluevec = vmap->operator[]("blue");
    if (!redvec||!greenvec||!bluevec) return 0xffffffff;
    int r = redvec->operator[](vertex);
    int g = greenvec->operator[](vertex);
    int b = bluevec->operator[](vertex);
    return 0xff000000 + (r<<16) + (g<<8) + b;
  }
  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const
  {

    std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
    if (!mymap) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *veca = mymap->operator[]("vertex_indices");
    if (!vec&&!veca) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    
    if (!(face>=0 && face<std::max(vec?vec->size():0,veca?veca->size():0))) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    int vertex = 0;
    if (vec) vertex = vec->operator[](face)->operator[](point);
    if (veca) vertex = veca->operator[](face)->operator[](point);
    std::map<std::string,std::vector<float>* > *vmap = load.floats["vertex"];
    std::vector<float> *txvec = vmap->operator[]("s");
    std::vector<float> *tyvec = vmap->operator[]("t");
    if (!txvec||!tyvec) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    float tx = txvec->operator[](vertex);
    float ty = tyvec->operator[](vertex);
    Point2d p;
    p.x = tx;
    p.y = ty;
    return p;

#if 0
    
        std::map<std::string,std::vector<std::vector<int>*>*>* mymap = load.int_lists["face"];
	if (!mymap) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    std::vector<std::vector<int>*> *vec = mymap->operator[]("vertex_index");
    std::vector<std::vector<int>*> *veca = mymap->operator[]("vertex_indices");
    if (!vec&&!veca) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    if (!(face>=0 && face<std::max(vec?vec->size():0,veca?veca->size():0))) { Point2d p; p.x=0.0; p.y=0.0; return p; } 
    
    int vertex = 0;
    if (vec) vertex=vec->operator[](face)->operator[](point);
    if (veca) vertex=veca->operator[](face)->operator[](point);
    
    std::map<std::string,std::vector<std::vector<float>*>*>* mymap2 = load.float_lists["face"];
    if (!mymap2) { Point2d p; p.x=0.0; p.y=0.0; return p; }

    std::vector<std::vector<float>*>* vec2 = mymap2->operator[]("texcoord");
    if (!vec2) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    if (!(face>=0 && face<vec2->size())) { Point2d p; p.x=0.0; p.y=0.0; return p; } 
    
    float tx = vec2->operator[](face)->operator[](point*2);
    float ty = vec2->operator[](face)->operator[](point*2+1);
    
    Point2d p;
    p.x = tx;
    p.y = ty;
    return p;
#endif
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
private:
  PlyLoader load;  
};

GameApi::P GameApi::PointsApi::ply_faces(std::string url)
{
  return add_polygon2(e, new PlyFaceCollection(e,url,gameapi_homepageurl),1);
}
