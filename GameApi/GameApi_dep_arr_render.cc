
#include "GameApi_h.hh"

#include "GameApi_low.hh"

int ArrayRender::NumVertices(FaceCollection &coll)
{
  return coll.NumFaces()*coll.NumPoints(0);
}
int ArrayRender::NumFaces(FaceCollection &coll)
{
  return coll.NumFaces();
}
std::pair<int,int> ArrayRender::Insert(FaceCollection &coll)
{
  if (used_vertex_count[0]==0)
    {
      quads = coll.NumPoints(0)==4;
    }
  int old_used_vertex_count = used_vertex_count[0];
  int old_face_count = used_face_count[0];
  int offset3 = used_vertex_count[0]*3;
  int offset4 = used_vertex_count[0]*4;
  int offset2 = used_vertex_count[0]*2;
  int size_faces = coll.NumFaces();
  int size_points = coll.NumPoints(0);
  int i = 0;
  for(int f=0;f<size_faces;f++)
    {
    index_array[used_face_count[0] + f] = used_vertex_count[0] + i;
    for(int p=0;p<size_points;p++)
      {
	Point pp = coll.FacePoint(f,p);
	//std::cout << "f: " << f << " p: " << p << " " << pp << std::endl;
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	Vector n = coll.PointNormal(f,p);
	normal_array[offset3 + i*3 + 0] = n.dx;
	normal_array[offset3 + i*3 + 1] = n.dy;
	normal_array[offset3 + i*3 + 2] = n.dz;
	unsigned int color = coll.Color(f,p);
	unsigned char *c = (unsigned char*)&color;
	color_array[offset4 + i*4] = c[0];
	color_array[offset4 + i*4 + 1] = c[1];
	color_array[offset4 + i*4 + 2] = c[2];
	color_array[offset4 + i*4 + 3] = c[3];
	Point2d texcoord = coll.TexCoord(f,p);
	tex_coord_array[offset2 + i*2 + 0] = texcoord.x;
	tex_coord_array[offset2 + i*2 + 1] = texcoord.y;

	i++;
      }
    }
  used_face_count += size_faces;
  used_vertex_count[0] += i;
  return std::make_pair(old_used_vertex_count, old_face_count);
}
void ArrayRender::UpdatePos(FaceCollection &coll, std::pair<int,int> p)
{
  int vertex_count = p.first;
  int face_count = p.second;
  int offset3 = vertex_count*3;
  int size_faces = coll.NumFaces();
  int size_points = coll.NumPoints(0);
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
    for(int p=0;p<size_points;p++)
      {
	Point pp = coll.FacePoint(f,p);
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	i++;
      }
    }
}
std::pair<int,int> ArrayRender::InsertMesh(Mesh &mesh, int frame)
{
  //std::cout << "InsertMesh start" << std::endl;
  if (used_vertex_count[frame]==0)
    {
      quads = mesh.NumPoints()==4;
    }
  else
    {
      if ((mesh.NumPoints() == 4)!=quads) std::cout << "ArrayRender::InsertMesh ERROR!" << std::endl;
    }
  int old_used_vertex_count = used_vertex_count[frame];
  int old_face_count = used_face_count[frame];
  int offset3 = used_vertex_count[frame]*3 + vertex_array_size*3*frame;
  //int offset4 = used_vertex_count[frame]*4 + vertex_array_size*4*frame;
  //int offset2 = used_vertex_count[frame]*2 + vertex_array_size*2*frame;;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = 0;
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "M: " << f << "/" << size_faces << std::endl;
	}

    index_array[used_face_count[frame] + f] = used_vertex_count[frame] + i;
    for(int p=0;p<size_points;p++)
      {
	Point pp = mesh.FacePoint(frame, 0, f,p);
	//std::cout << "f: " << f << " p: " << p << " " << pp << std::endl;
	vertex_array[offset3 + i*3 + 0] = pp.x;
	vertex_array[offset3 + i*3 + 1] = pp.y;
	vertex_array[offset3 + i*3 + 2] = pp.z;
	i++;
      }
    }
  used_face_count[frame] += size_faces;
  used_vertex_count[frame] += i;
  //std::cout << "InsertMesh end" << std::endl;
  return std::make_pair(old_used_vertex_count, old_face_count);
}

void ArrayRender::UpdateTexCoord(Mesh &mesh, MeshTexCoords &coord, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  //int offset3 = vertex_count*3 + vertex_array_size*3*frame;
  int offset2 = vertex_count*2 + vertex_array_size*2*frame;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "TX: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	Point2d texcoord = coord.TexCoord(frame, 0, f,p);
	//std::cout << "f: " << f << " p: " << p << " " << texcoord.x << "," << texcoord.y << std::endl;
	tex_coord_array[offset2 + i*2 + 0] = texcoord.x;
	tex_coord_array[offset2 + i*2 + 1] = texcoord.y;
	i++;
      }
    }
}
void ArrayRender::AllocTexture(int count)
{
  OpenglLowApi *ogl = g_low->ogl;
  int texcount = count;
  texture = new int[texcount];
  ogl->glGenTextures(texcount, (Low_GLuint*)&texture[0]);
#if 0
  GLenum e = ogl->glGetError();
  if (e!=Low_GL_NO_ERROR)
    {
      std::cout << "ArrayRender::AllocTexture error!" << e << std::endl;
    }
#endif
  texture_count = texcount;
  textures = new BufferRef*[count];
}
void ArrayRender::UpdateAllTextures(MeshTextures &tex)
{
  int count = tex.NumTextures();
  for(int i=0;i<count;i++)
    {
      UpdateTexture(tex, i);
    }
}

//#ifndef EMSCRIPTEN
#ifndef ARM
#ifdef THREADS
void *thread_func_bitmap(void* data);
void *thread_func_bitmap(void *data)
{
  ThreadInfo_bitmap *ti = (ThreadInfo_bitmap*)data;
  ti->buffer->Gen(ti->start_x, ti->end_x, ti->start_y, ti->end_y);
  return 0;
}

int ThreadedUpdateTexture::push_thread(BufferFromBitmap* bm, int start_x, int end_x, int start_y, int end_y)
  {
    //std::cout << "Starting thread" << std::endl;
    buffers.push_back(bm);
    ThreadInfo_bitmap *info = new ThreadInfo_bitmap;
    info->buffer = bm;
    info->start_x = start_x;
    info->start_y = start_y;
    info->end_x = end_x;
    info->end_y = end_y;
    ti.push_back(info);
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 3000000);
    pthread_create(&info->thread_id, &attr, &thread_func_bitmap, (void*)info);
    pthread_attr_destroy(&attr);
    return buffers.size()-1;
  }
void ThreadedUpdateTexture::join(int id)
  {
    void *res;
    pthread_join(ti[id]->thread_id, &res);
  }
ThreadedUpdateTexture::~ThreadedUpdateTexture() {
    int s = ti.size();
    for(int i=0;i<s;i++)
      {
	delete ti[i];
      }

  }

#endif
#endif


void ArrayRender::UpdateTexture(MeshTextures &tex, int num)
{
  OpenglLowApi *ogl = g_low->ogl;

  //std::cout << "UpdateTexture " << num << std::endl;
  ogl->glActiveTexture(Low_GL_TEXTURE0+num);
#if 0
  GLenum e = ogl->glGetError();
  if (e!=Low_GL_NO_ERROR)
    {
      std::cout << "ArrayRender::UpdateTexture1 error!" << e << std::endl;
    }
#endif
  tex.GenTexture(num);
  BufferRef ref = tex.TextureBuf(num);
  BitmapFromBuffer buf(ref);
  FlipColours flip(buf);
  BufferFromBitmap buf2(flip);
#ifndef THREADS
  buf2.Gen();
#else
  buf2.GenPrepare();

  int numthreads = 4;
  ThreadedUpdateTexture threads;
  int sx = flip.SizeX();
  int sy = flip.SizeY();
  int dsy = sy/numthreads + 1;
  std::vector<int> ids;
  for(int i=0;i<numthreads;i++)
    {
      int start_x = 0;
      int end_x = sx;
      int start_y = i*dsy;
      int end_y = (i+1)*dsy;
      if (start_y>sy) { start_y = sy; }
      if (end_y>sy) end_y = sy;
      
      if (end_y-start_y > 0) {
	//std::cout << "Thread:" << start_x << ".." << end_x << ", " << start_y << ".." << end_y << std::endl;
	ids.push_back(threads.push_thread(&buf2, start_x, end_x, start_y, end_y));
      }
      }
  int ss = ids.size();
  for(int i=0;i<ss;i++)
    {
      threads.join(ids[i]);
    }
#endif

  //buf2.Gen(); 
  BufferRef ref2 = buf2.Buffer(); 
  int sizex = ref2.width;
  int sizey = ref2.height;
  ogl->glBindTexture(Low_GL_TEXTURE_2D, texture[num]);
  //for(int i=0;i<sizex;i++)
  //  {
  //    std::cout << std::hex << ref2.buffer[i+5*ref2.ydelta] << std::dec << std::endl;
  //  }
  ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, sizex, sizey, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref2.buffer);
#if 0
  GLenum e2 = glGetError();
  if (e2!=GL_NO_ERROR)
    {
      std::cout << "ArrayRender::UpdateTexture2 error!" << e2 << std::endl;
    }
#endif
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_LINEAR);      
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_LINEAR);	
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
  ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);
  //ogl->glHint(Low_GL_PERSPECTIVE_CORRECTION_HINT, Low_GL_NICEST);
  
}
void ArrayRender::EnableTexture(int num)
{
  OpenglLowApi *ogl = g_low->ogl;
  //std::cout << "EnableTexture " << num << std::endl;
  ogl->glActiveTexture(Low_GL_TEXTURE0+num);
  ogl->glBindTexture(Low_GL_TEXTURE_2D, texture[num]);
  //ogl->glEnable(Low_GL_TEXTURE_2D);
}
void ArrayRender::DisableTexture()
{
  //ogl->glDisable(Low_GL_TEXTURE_2D);
}
void ArrayRender::UpdateColors(Mesh &mesh, MeshColors &color, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  //int offset3 = vertex_count*3 + vertex_array_size*3*frame;
  //int offset2 = vertex_count*2 + vertex_array_size*2*frame;
  int offset4 = vertex_count*4 + vertex_array_size*4*frame;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "C: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	unsigned int col = color.VertexColor(frame, 0, f,p);
	//std::cout << "UpdateColors: " << std::hex << col << std::endl;
	unsigned char *c = (unsigned char*)&col;
	color_array[offset4 + i*4] = c[0];
	color_array[offset4 + i*4 + 1] = c[1];
	color_array[offset4 + i*4 + 2] = c[2];
	color_array[offset4 + i*4 + 3] = c[3];
	i++;
      }
    }
}

std::vector<std::pair<int,int> > ArrayRender::InsertAll(Mesh &mesh, MeshNormals &normal, MeshTexCoords &coord, MeshColors &color)
{
  std::vector<std::pair<int,int> > vec;
  int meshsize = mesh.NumFrames();
  for(int i=0;i<meshsize;i++)
    {
      std::pair<int,int> p = InsertMesh(mesh,i);
      //std::cout << "Mesh: " << p.first << " " << p.second << std::endl;
      vec.push_back(p);
    }
  int normalsize = normal.NumFrames();
  for(int ii=0;ii<meshsize;ii++)
    {
      for(int k=0;k<normalsize;k++)
	{
	  UpdateNormals(mesh, normal, vec[ii], k);
	}
    }

  int texcoordsize = coord.NumFrames();
  for(int ie=0;ie<meshsize;ie++)
    {
      for(int k=0;k<texcoordsize;k++)
	{
	  UpdateTexCoord(mesh, coord, vec[ie], k);
	}
    }

  int colorsize = color.NumFrames();
  for(int id=0;id<meshsize;id++)
    {
      for(int k=0;k<colorsize;k++)
	{
	  UpdateColors(mesh, color, vec[id], k);
	}
    }
  return vec;
}

void ArrayRender::UpdateNormals(Mesh &mesh, MeshNormals &normal, std::pair<int,int> p, int frame)
{
  int vertex_count = p.first;
  int face_count = p.second;
  int offset3 = vertex_count*3;
  //int offset2 = vertex_count*2;
  int size_faces = mesh.NumFaces(frame);
  int size_points = mesh.NumPoints();
  int i = index_array[face_count];
  for(int f=0;f<size_faces;f++)
    {
      if (f%1000 == 0)
	{
	  //std::cout << "N: " << f << "/" << size_faces << std::endl;
	}
    for(int p=0;p<size_points;p++)
      {
	Vector norm = normal.PointNormal(frame, 0, f,p);
	normal_array[offset3 + i*3 + 0] = norm.dx;
	normal_array[offset3 + i*3 + 1] = norm.dy;
	normal_array[offset3 + i*3 + 2] = norm.dz;
	i++;
      }
    }
}



void ArrayRender::Alloc(int numfaces, int numvertices)
{
  std::cout << "ArrayRender::Alloc " << numfaces << " " << numvertices << std::endl;
  vertex_array = new float[3*numvertices];
  vertex_array_size = numvertices;
  used_vertex_count = new int[1];
  used_face_count = new int[1];
  used_vertex_count[0] = 0;
  used_face_count[0] = 0;
  normal_array = new float[3*numvertices];
  color_array = new unsigned char[4*numvertices];
  tex_coord_array = new float[2*numvertices];

  index_array = new int[numfaces];


  q_vertex_array=0;
  q_normal_array=0;
  q_color_array=0;
  q_tex_coord_array=0;

}
ArrayRender::~ArrayRender()
{
  OpenglLowApi *ogl = g_low->ogl;
  if (texture_count)
    {
      ogl->glDeleteTextures(texture_count, (const Low_GLuint*)&texture[0]);
    }
  delete [] q_vertex_array;
  delete [] q_normal_array;
  delete [] q_color_array;
  delete [] q_tex_coord_array;
  delete [] textures;
  delete [] vertex_array;
  delete [] normal_array;
  delete [] color_array;
  delete [] tex_coord_array;
  delete [] index_array;
  delete [] texture;
  delete [] used_vertex_count;
  delete [] used_face_count;
  
}
void ArrayRender::Alloc(int numfaces, int numvertices, int vertexframes, int normalframes, int colorframes, int texcoordframes)
{
  vertex_array = new float[3*numvertices*vertexframes];
  for(int i=0;i<3*numvertices*vertexframes;i++)
    {
      vertex_array[i]=0.0f;
    }
  vertex_array_size = numvertices;
  used_vertex_count = new int[vertexframes];
  used_face_count = new int[vertexframes];
  for(int i=0;i<vertexframes;i++)
    {
      used_vertex_count[i] = 0;
      used_face_count[i] = 0;
    }
  normal_array = new float[3*numvertices*normalframes*vertexframes];
  color_array = new unsigned char[4*numvertices*colorframes*vertexframes];
  tex_coord_array = new float[2*numvertices*texcoordframes*vertexframes];

  index_array = new int[numfaces*vertexframes];

  m_vertex_frames = vertexframes;
  m_normal_frames = normalframes;
  m_color_frames = colorframes;
  m_texcoord_frames = texcoordframes;
}

void ArrayRender::Prepare()
{
  if (quads)
    {
      q_num_vertices = vertex_array_size*6/4;
    }
  else
    {
      q_num_vertices = vertex_array_size;
    }
  q_vertex_array = new float[q_num_vertices*3];
  q_normal_array = new float[q_num_vertices*3];
  q_color_array = new unsigned char[q_num_vertices*4];
  q_tex_coord_array = new float[q_num_vertices*2];

  if (quads) {
    int count = 0;
    //std::cout << "Vertex Array Size2: " << vertex_array_size << std::endl;
    for(int i=0;i<vertex_array_size/(4*3);i++)
      {
	q_vertex_array[count+0*3+0] = vertex_array[i*4*3+0*3+0];
	q_vertex_array[count+0*3+1] = vertex_array[i*4*3+0*3+1];
	q_vertex_array[count+0*3+2] = vertex_array[i*4*3+0*3+2];
	q_vertex_array[count+1*3+0] = vertex_array[i*4*3+1*3+0];
	q_vertex_array[count+1*3+1] = vertex_array[i*4*3+1*3+1];
	q_vertex_array[count+1*3+2] = vertex_array[i*4*3+1*3+2];
	q_vertex_array[count+2*3+0] = vertex_array[i*4*3+2*3+0];
	q_vertex_array[count+2*3+1] = vertex_array[i*4*3+2*3+1];
	q_vertex_array[count+2*3+2] = vertex_array[i*4*3+2*3+2];
	q_vertex_array[count+3*3+0] = vertex_array[i*4*3+0*3+0];
	q_vertex_array[count+3*3+1] = vertex_array[i*4*3+0*3+1];
	q_vertex_array[count+3*3+2] = vertex_array[i*4*3+0*3+2];
	q_vertex_array[count+4*3+0] = vertex_array[i*4*3+2*3+0];
	q_vertex_array[count+4*3+1] = vertex_array[i*4*3+2*3+1];
	q_vertex_array[count+4*3+2] = vertex_array[i*4*3+2*3+2];
	q_vertex_array[count+5*3+0] = vertex_array[i*4*3+3*3+0];
	q_vertex_array[count+5*3+1] = vertex_array[i*4*3+3*3+1];
	q_vertex_array[count+5*3+2] = vertex_array[i*4*3+3*3+2];
	
	q_normal_array[count+0*3+0] = normal_array[i*4*3+0*3+0];
	q_normal_array[count+0*3+1] = normal_array[i*4*3+0*3+1];
	q_normal_array[count+0*3+2] = normal_array[i*4*3+0*3+2];
	q_normal_array[count+1*3+0] = normal_array[i*4*3+1*3+0];
	q_normal_array[count+1*3+1] = normal_array[i*4*3+1*3+1];
	q_normal_array[count+1*3+2] = normal_array[i*4*3+1*3+2];
	q_normal_array[count+2*3+0] = normal_array[i*4*3+2*3+0];
	q_normal_array[count+2*3+1] = normal_array[i*4*3+2*3+1];
	q_normal_array[count+2*3+2] = normal_array[i*4*3+2*3+2];
	q_normal_array[count+3*3+0] = normal_array[i*4*3+0*3+0];
	q_normal_array[count+3*3+1] = normal_array[i*4*3+0*3+1];
	q_normal_array[count+3*3+2] = normal_array[i*4*3+0*3+2];
	q_normal_array[count+4*3+0] = normal_array[i*4*3+2*3+0];
	q_normal_array[count+4*3+1] = normal_array[i*4*3+2*3+1];
	q_normal_array[count+4*3+2] = normal_array[i*4*3+2*3+2];
	q_normal_array[count+5*3+0] = normal_array[i*4*3+3*3+0];
	q_normal_array[count+5*3+1] = normal_array[i*4*3+3*3+1];
	q_normal_array[count+5*3+2] = normal_array[i*4*3+3*3+2];
	
	
	q_color_array[count+0*3+0] = color_array[i*4*3+0*3+0];
	q_color_array[count+0*3+1] = color_array[i*4*3+0*3+1];
	q_color_array[count+0*3+2] = color_array[i*4*3+0*3+2];
	q_color_array[count+1*3+0] = color_array[i*4*3+1*3+0];
	q_color_array[count+1*3+1] = color_array[i*4*3+1*3+1];
	q_color_array[count+1*3+2] = color_array[i*4*3+1*3+2];
	q_color_array[count+2*3+0] = color_array[i*4*3+2*3+0];
	q_color_array[count+2*3+1] = color_array[i*4*3+2*3+1];
	q_color_array[count+2*3+2] = color_array[i*4*3+2*3+2];
	q_color_array[count+3*3+0] = color_array[i*4*3+0*3+0];
	q_color_array[count+3*3+1] = color_array[i*4*3+0*3+1];
	q_color_array[count+3*3+2] = color_array[i*4*3+0*3+2];
	q_color_array[count+4*3+0] = color_array[i*4*3+2*3+0];
	q_color_array[count+4*3+1] = color_array[i*4*3+2*3+1];
	q_color_array[count+4*3+2] = color_array[i*4*3+2*3+2];
	q_color_array[count+5*3+0] = color_array[i*4*3+3*3+0];
	q_color_array[count+5*3+1] = color_array[i*4*3+3*3+1];
	q_color_array[count+5*3+2] = color_array[i*4*3+3*3+2];
	

	q_tex_coord_array[count+0*3+0] = tex_coord_array[i*4*3+0*3+0];
	q_tex_coord_array[count+0*3+1] = tex_coord_array[i*4*3+0*3+1];
	q_tex_coord_array[count+0*3+2] = tex_coord_array[i*4*3+0*3+2];
	q_tex_coord_array[count+1*3+0] = tex_coord_array[i*4*3+1*3+0];
	q_tex_coord_array[count+1*3+1] = tex_coord_array[i*4*3+1*3+1];
	q_tex_coord_array[count+1*3+2] = tex_coord_array[i*4*3+1*3+2];
	q_tex_coord_array[count+2*3+0] = tex_coord_array[i*4*3+2*3+0];
	q_tex_coord_array[count+2*3+1] = tex_coord_array[i*4*3+2*3+1];
	q_tex_coord_array[count+2*3+2] = tex_coord_array[i*4*3+2*3+2];
	q_tex_coord_array[count+3*3+0] = tex_coord_array[i*4*3+0*3+0];
	q_tex_coord_array[count+3*3+1] = tex_coord_array[i*4*3+0*3+1];
	q_tex_coord_array[count+3*3+2] = tex_coord_array[i*4*3+0*3+2];
	q_tex_coord_array[count+4*3+0] = tex_coord_array[i*4*3+2*3+0];
	q_tex_coord_array[count+4*3+1] = tex_coord_array[i*4*3+2*3+1];
	q_tex_coord_array[count+4*3+2] = tex_coord_array[i*4*3+2*3+2];
	q_tex_coord_array[count+5*3+0] = tex_coord_array[i*4*3+3*3+0];
	q_tex_coord_array[count+5*3+1] = tex_coord_array[i*4*3+3*3+1];
	q_tex_coord_array[count+5*3+2] = tex_coord_array[i*4*3+3*3+2];
	
	count += 6*3;
      }
  } else
    {
      //int count = 0;
    //std::cout << "Vertex Array Size: " << vertex_array_size << std::endl;
    for(int i=0;i<vertex_array_size;i++) {
      q_vertex_array[i*3+0] = vertex_array[i*3+0];
      q_vertex_array[i*3+1] = vertex_array[i*3+1];
      q_vertex_array[i*3+2] = vertex_array[i*3+2];
      q_normal_array[i*3+0] = normal_array[i*3+0];
      q_normal_array[i*3+1] = normal_array[i*3+1];
      q_normal_array[i*3+2] = normal_array[i*3+2];
      q_color_array[i*4+0] = color_array[i*4+0];
      q_color_array[i*4+1] = color_array[i*4+1];
      q_color_array[i*4+2] = color_array[i*4+2];
      q_color_array[i*4+3] = color_array[i*4+3];
      q_tex_coord_array[i*2+0] = tex_coord_array[i*2+0];
      q_tex_coord_array[i*2+1] = tex_coord_array[i*2+1];
    }
    }
  OpenglLowApi *ogl = g_low->ogl;

#ifdef VAO
  ogl->glGenVertexArrays(1,vao);
  ogl->glBindVertexArray(vao[0]);
#endif
  ogl->glGenBuffers(5, &buffer[0]);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[0]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_vertex_array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[1]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_normal_array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[2]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, q_num_vertices*sizeof(unsigned char)*4, q_color_array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[3]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*2, q_tex_coord_array, Low_GL_STATIC_DRAW);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[4]);
  ogl->glBufferData(Low_GL_ARRAY_BUFFER, q_num_vertices*sizeof(float)*3, q_vertex_array, Low_GL_STATIC_DRAW);

#ifdef VAO
  ogl->glEnableVertexAttribArray(0);
  ogl->glEnableVertexAttribArray(1);
  ogl->glEnableVertexAttribArray(2);
  ogl->glEnableVertexAttribArray(3);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[0]);
  ogl->glVertexAttribPointer(0,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[1]);
  ogl->glVertexAttribPointer(1,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[2]);
  ogl->glVertexAttribPointer(2,4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[3]);
  ogl->glVertexAttribPointer(3,2, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[4]);
  ogl->glVertexAttribPointer(4,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
#endif
  delete [] q_vertex_array; q_vertex_array=0;
  delete [] q_normal_array; q_normal_array=0;
  delete [] q_color_array; q_color_array=0;
  delete [] q_tex_coord_array; q_tex_coord_array=0;

}
void ArrayRender::Render(int vertexframe, int normalframe, int colorframe, int texcoordframe, int vertex_pos, int vertex_size)
{
  OpenglLowApi *ogl = g_low->ogl;
#ifdef VAO
  ogl->glBindVertexArray(vao[0]);
#else
  ogl->glEnableVertexAttribArray(0);
  ogl->glEnableVertexAttribArray(1);
  ogl->glEnableVertexAttribArray(2);
  ogl->glEnableVertexAttribArray(3);
  ogl->glEnableVertexAttribArray(4);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[0]);
  ogl->glVertexAttribPointer(0,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[1]);
  ogl->glVertexAttribPointer(1,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[2]);
  ogl->glVertexAttribPointer(2,4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[3]);
  ogl->glVertexAttribPointer(3,2, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[4]);
  ogl->glVertexAttribPointer(4,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
#endif
  ogl->glDrawArrays(Low_GL_TRIANGLES, 0, vertex_size*6/4);
#ifndef VAO
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(1);
  ogl->glDisableVertexAttribArray(2);
  ogl->glDisableVertexAttribArray(3);
  ogl->glDisableVertexAttribArray(4);
#endif

#if 0
  // enabling
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, Low_GL_FLOAT, 0, vertex_array+vertex_pos*3+vertexframe*vertex_array_size*3);

  if (normalframe != -1)
    {
      glEnableClientState(Low_GL_NORMAL_ARRAY);
      glNormalPointer(Low_GL_FLOAT, 0, normal_array+vertex_pos*3+normalframe*vertex_array_size*3);
    }
  if (colorframe != -1)
    {
      glEnableClientState(Low_GL_COLOR_ARRAY);
      glColorPointer(4, Low_GL_UNSIGNED_BYTE, 0, color_array+vertex_pos*4+colorframe*vertex_array_size*4);
    }
  if (texcoordframe != -1)
    {
      glEnableClientState(Low_GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, Low_GL_FLOAT, 0, tex_coord_array+vertex_pos*2+texcoordframe*vertex_array_size*2);
    }

  // Render
  if (quads)
    {
      glDrawArrays(Low_GL_QUADS, 0, vertex_size);
    }
  else
    {
      glDrawArrays(Low_GL_TRIANGLES, 0, vertex_size);
    }

  // disabling
  if (texcoordframe != -1)
    {
      glDisableClientState(Low_GL_TEXTURE_COORD_ARRAY);
    }
  if (colorframe != -1)
    {
      glDisableClientState(Low_GL_COLOR_ARRAY);
    }
  if (normalframe != -1)
    {
      glDisableClientState(Low_GL_NORMAL_ARRAY);
    }
  glDisableClientState(Low_GL_VERTEX_ARRAY);
#endif
}


void ArrayRender::Render(bool normal, bool color, bool texcoord, int vertex_pos, int vertex_size)
{
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glEnableVertexAttribArray(0);
  ogl->glEnableVertexAttribArray(1);
  ogl->glEnableVertexAttribArray(2);
  ogl->glEnableVertexAttribArray(3);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[0]);
  ogl->glVertexAttribPointer(0,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[1]);
  ogl->glVertexAttribPointer(1,3, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[2]);
  ogl->glVertexAttribPointer(2,4, Low_GL_UNSIGNED_BYTE, Low_GL_FALSE, 0,0);
  ogl->glBindBuffer(Low_GL_ARRAY_BUFFER, buffer[3]);
  ogl->glVertexAttribPointer(3,2, Low_GL_FLOAT, Low_GL_FALSE, 0,0);
  ogl->glDrawArrays(Low_GL_TRIANGLES, 0, q_num_vertices);
  ogl->glDisableVertexAttribArray(0);
  ogl->glDisableVertexAttribArray(1);
  ogl->glDisableVertexAttribArray(2);
  ogl->glDisableVertexAttribArray(3);

#if 0
  // enabling
  ogl->glEnableClientState(Low_GL_VERTEX_ARRAY);
  ogl->glVertexPointer(3, GL_FLOAT, 0, vertex_array+vertex_pos*3);

  if (normal)
    {
      ogl->glEnableClientState(Low_GL_NORMAL_ARRAY);
      ogl->glNormalPointer(Low_GL_FLOAT, 0, normal_array+vertex_pos*3);
    }
  if (color)
    {
      ogl->glEnableClientState(GL_COLOR_ARRAY);
      ogl->glColorPointer(4, Low_GL_UNSIGNED_BYTE, 0, color_array+vertex_pos*4);
    }
  if (texcoord)
    {
      ogl->glEnableClientState(Low_GL_TEXTURE_COORD_ARRAY);
      ogl->glTexCoordPointer(2, Low_GL_FLOAT, 0, tex_coord_array+vertex_pos*2);
    }

  // Render
  if (quads)
    {
      ogl->glDrawArrays(Low_GL_QUADS, 0, vertex_size);
    }
  else
    {
      ogl->glDrawArrays(Low_GL_TRIANGLES, 0, vertex_size);
    }

  // disabling
  if (texcoord)
    {
      ogl->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if (color)
    {
      ogl->glDisableClientState(GL_COLOR_ARRAY);
    }
  if (normal)
    {
      ogl->glDisableClientState(GL_NORMAL_ARRAY);
    }
  ogl->glDisableClientState(GL_VERTEX_ARRAY);
#endif
}
