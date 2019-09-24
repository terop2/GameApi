
#include "GameApi_h.hh"
#include "GraphI.hh"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

extern std::string gameapi_homepageurl;


class MatrixMovement : public Movement
{
public:
  MatrixMovement(Movement *next, Matrix m) : next(next), m(m) { }
  virtual void event(MainLoopEvent &e) { next->event(e); }
  virtual void frame(MainLoopEnv &e) { next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { next->draw_event(e); }

  void set_matrix(Matrix mm) { m = mm; }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return next->get_whole_matrix(time, delta_time)*m;
  }
private:
  Movement *next;
  Matrix m;
};


class MaterialForward : public Material
{
public:
  GameApi::ML call(GameApi::P p) const
  {
    GameApi::ML ml;
    ml.id = mat(p.id);
    return ml;
  }
  GameApi::ML call_inst(GameApi::P p, GameApi::PTS pts)
  {
    GameApi::ML ml;
    ml.id = mat_inst(p.id,pts.id);
    return ml;
  }
  GameApi::ML call_inst_matrix(GameApi::P p, GameApi::MS ms)
  {
    GameApi::ML ml;
    ml.id = mat_inst_matrix(p.id,ms.id);
    return ml;
  }

  int mat(int p) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::ML ml = mat2(p2);
    return ml.id;
  }
  int mat_inst(int p, int pts) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat2_inst(p2,p3);
    return ml.id;
  }
  int mat_inst_matrix(int p, int ms) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::MS p3;
    p3.id = ms;
    GameApi::ML ml = mat2_inst_matrix(p2,p3);
    return ml.id;
  }

  int mat_inst2(int p, int pta) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTA p3;
    p3.id = pta;
    GameApi::ML ml = mat2_inst2(p2,p3);
    return ml.id;

  }
  int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    GameApi::P p2;
    p2.id = p;
    GameApi::PTS p3;
    p3.id = pts;
    GameApi::ML ml = mat_inst_fade(p2,p3, flip, start_time, end_time);
    return ml.id;

  }
  virtual GameApi::ML mat2(GameApi::P p) const=0;
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const=0;
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const=0;
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};

extern std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
int bitmap_find_data(std::string data);

class BitmapPrepareCache : public Bitmap<Color>
{
public:
  BitmapPrepareCache(GameApi::Env &e, std::string id, Bitmap<Color> *bm) : e(e), id(id), bm(bm) { }
  void Prepare()
  {
    if (bitmap_find_data(id)!=-1) {
      return;
    }
    bm->Prepare();
    GameApi::BM num = add_color_bitmap2(e, bm);
    bitmap_prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  Bitmap<Color> *get_bm() const
  {
    if (bm_cache) return bm_cache;
    int num = bitmap_find_data(id);
    if (num==-1) { const_cast<BitmapPrepareCache*>(this)->Prepare(); num=bitmap_find_data(id); }
    GameApi::BM bm2;
    bm2.id = num;
    BitmapHandle *handle = find_bitmap(e, bm2);
    Bitmap<Color> *bbm = find_color_bitmap(handle);
    bm_cache=bbm;
    return bbm;
  }
  virtual int SizeX() const { return get_bm()->SizeX(); }
  virtual int SizeY() const { return get_bm()->SizeY(); }
  virtual Color Map(int x, int y) const
  {
    return get_bm()->Map(x,y);
  }
private:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
  mutable Bitmap<Color> *bm_cache=0;
};


bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr);
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr);

bool FileExists(const std::string &abs_filename, void *ptr);
std::string ExpandFilePath(const std::string &str, void *ptr);
bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *ptr);
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char> &contents, void *ptr);


GameApi::Env *g_e = 0;

class LoadGltf
{
public:
  LoadGltf(GameApi::Env &e, std::string base_url, std::string url, std::string homepage, bool is_binary) : e(e), base_url(base_url), url(url), homepage(homepage), is_binary(is_binary) {
    g_e = &e;
    tinygltf::FsCallbacks fs = {
      &FileExists,
      &ExpandFilePath,
      &ReadWholeFile,
      &WriteWholeFile,
      (void*)this
    };
    tiny.SetFsCallbacks(fs);
    //tiny.SetImageLoader(&LoadImageData, this);
    //tiny.SetImageWriter(&WriteImageData, this);
  }
  void Prepare() {

#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "ASync not ready!" << std::endl; return; }
    std::string str(vec->begin(),vec->end());
    //bool b = false;
    std::string err;
    std::string warn;
    if (!is_binary) {
      tiny.LoadASCIIFromString(&model, &err, &warn, str.c_str(), str.size(), base_url, tinygltf::REQUIRE_ALL);
    } else {
      tiny.LoadBinaryFromMemory(&model, &err, &warn, &vec->operator[](0), vec->size(), base_url, tinygltf::REQUIRE_ALL); 
    }
    if (!warn.empty()) { std::cout << "WARN: " << warn << std::endl; }
    if (!err.empty()) { std::cout << "ERROR: " << err << std::endl; }
  }
public:
  GameApi::Env &e;
  std::string base_url;
  std::string url;
  std::string homepage;
  bool is_binary;
  tinygltf::TinyGLTF tiny;
  tinygltf::Model model;
};

void call_prepare(void *ptr) {
  LoadGltf *load = (LoadGltf*)ptr;
  load->Prepare();
}

struct PrepareCB
{
  void (*fptr)(void*);
  void *ptr;
};

extern std::vector<PrepareCB> g_prepare_callbacks;

bool FileExists(const std::string &abs_filename, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "FileExists " << abs_filename << std::endl;
  return true;
}
std::string ExpandFilePath(const std::string &str, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "ExpandFilePath " << str << std::endl;
  return str;
}

bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "ReadWholeFile " << filepath << std::endl;
  std::string url = filepath;
#ifndef EMSCRIPTEN
    g_e->async_load_url(url, gameapi_homepageurl);
#endif
    std::vector<unsigned char> *vec = g_e->get_loaded_async_url(url);
    if (!vec) return false;
    *out = *vec;
  return true;
}
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char> &contents, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "WriteWholeFile" << filepath << std::endl;
  return false;
}
bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "LoadImageData " << req_width << " " << req_height << " " << size << std::endl;
  return false;
}
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "WriteImageData " << *basepath << " " << *filename << std::endl;
  return false;
}

class GLTFImage : public Bitmap<Color>
{
public:
  GLTFImage(LoadGltf *load, int image_index) : load(load), image_index(image_index) { 
  }
  virtual void Prepare()
  {
    load->Prepare();
    img = 0;
    if (image_index>=0 && image_index<int(load->model.images.size()))
      img = &load->model.images[image_index];
  }

  virtual int SizeX() const { if (img) return img->width; return 0; }
  virtual int SizeY() const { if (img) return img->height; return 0; }
  virtual Color Map(int x, int y) const
  {
    if (!img) return Color(0x0);
    unsigned char *ptr = &img->image[0];
    int offset = (x*img->component + y*img->width*img->component)*(img->bits/8);
    if ((img->component==4 ||img->component==3)&& img->bits==8) {


      unsigned int val = *(unsigned int*)(ptr+offset);


	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b; 

	return Color(val);
    }
    if ((img->component==4 ||img->component==3)&& img->bits==16) {
      unsigned short *c0 = (unsigned short*)(ptr+offset);
      unsigned short r = c0[0];
      unsigned short g = c0[1];
      unsigned short b = c0[2];
      unsigned short a = c0[3];
      r>>=8;
      g>>=8;
      b>>=8;
      a>>=8;
      unsigned int aa = a;
      unsigned int rr = r;
      unsigned int gg = g;
      unsigned int bb = b;

	rr>>=16;
	gg>>=8;

	bb<<=16;
	gg<<=8;
	unsigned int val = aa+rr+gg+bb; 

      return Color(val);
    }
    std::cout << "GLTF Image format not regognized" << std::endl;
    return Color(0x0);
  }
private:
  LoadGltf *load;
  tinygltf::Image *img;
  int image_index;
};

class GLTFFaceCollection : public FaceCollection
{
public:
  GLTFFaceCollection( LoadGltf *load, int mesh_index, int prim_index) : load(load), mesh_index(mesh_index), prim_index(prim_index) {
  }
  virtual void Prepare() { 
    load->Prepare();

    
    if (mesh_index>=0 && mesh_index<int(load->model.meshes.size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else
      return;
    model = &load->model;
    
    mode = prim->mode;

    //int material = prim->material;
    //tinygltf::Material *mat = 0;
    //if (material!=-1)
    //  mat = &load->model.materials[material];


    // find indices
    indices_index = prim->indices;
    position_index = -1;
    normal_index = -1;
    texcoord_index = -1;
    color_index = -1;
    if (prim->attributes.find("POSITION") != prim->attributes.end())
      position_index = prim->attributes["POSITION"];
    if (prim->attributes.find("NORMAL") != prim->attributes.end())
      normal_index = prim->attributes["NORMAL"];
    if (prim->attributes.find("TEXCOORD_0") != prim->attributes.end())
      texcoord_index = prim->attributes["TEXCOORD_0"];
    if (prim->attributes.find("COLOR_0") != prim->attributes.end())
      color_index = prim->attributes["COLOR_0"];

    //if (mat) {
    //  int index = mat->pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
    //  int index_b = mat->pbrMetallicRoughness.baseColorTexture.texCoord;
    //  int index_n = mat->normalTexture.texCoord;
    //  int index_o = mat->occlusionTexture.texCoord;
    //  int index_e = mat->emissiveTexture.texCoord;
    //  if (index!=-1)
    //	texcoord_index = index_b;
    //}


    // find Accessors
    indices_acc = 0;
    if (indices_index!=-1)
      indices_acc = &model->accessors[indices_index];

    assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);
    //std::cout << "gltf component type: " << indices_acc->componentType << std::endl;
    //assert(indices_acc->componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);


    position_acc = 0;
    if (position_index!=-1)
      position_acc = &model->accessors[position_index];

    normal_acc = 0;
    if (normal_index!=-1)
      normal_acc = &model->accessors[normal_index];
    
    texcoord_acc = 0;
    if (texcoord_index!=-1)
      texcoord_acc = &model->accessors[texcoord_index];

    color_acc = 0;
    if (color_index!=-1)
      color_acc = &model->accessors[color_index];


    // find BufferViews
    indices_bv = 0;
    if (indices_acc) {
      int view = indices_acc->bufferView;
      if (view!=-1)
	indices_bv = &model->bufferViews[view];
    }

    position_bv = 0;
    if (position_acc) {
      int view = position_acc->bufferView;
      if (view!=-1)
	position_bv = &model->bufferViews[view];
    }

    normal_bv = 0;
    if (normal_acc) {
      int view = normal_acc->bufferView;
      if (view!=-1)
	normal_bv = &model->bufferViews[view];
    }

    texcoord_bv = 0;
    if (texcoord_acc) {
      int view = texcoord_acc->bufferView;
      if (view!=-1)
	texcoord_bv = &model->bufferViews[view];
    }

    color_bv = 0;
    if (color_acc) {
      int view = color_acc->bufferView;
      if (view!=-1)
	color_bv = &model->bufferViews[view];
    }

    // find buffers
    indices_buf = 0;
    if (indices_bv) {
      int buf = indices_bv->buffer;
      if (buf!=-1) {
	indices_buf = &model->buffers[buf];
      }
    }

    position_buf = 0;
    if (position_bv) {
      int buf = position_bv->buffer;
      if (buf!=-1) {
	position_buf = &model->buffers[buf];
      }
    }

    normal_buf = 0;
    if (normal_bv) {
      int buf = normal_bv->buffer;
      if (buf!=-1) {
	normal_buf = &model->buffers[buf];
      }
    }

    texcoord_buf = 0;
    if (texcoord_bv) {
      int buf = texcoord_bv->buffer;
      if (buf!=-1) {
	texcoord_buf = &model->buffers[buf];
      }
    }

    color_buf = 0;
    if (color_bv) {
      int buf = color_bv->buffer;
      if (buf!=-1) {
	color_buf = &model->buffers[buf];
      }
    }
    
  }
  virtual int NumFaces() const 
  {
    if (mode==TINYGLTF_MODE_TRIANGLES && indices_acc) {
      return indices_acc->count/3;
    }
    if (mode==TINYGLTF_MODE_TRIANGLE_STRIP && indices_acc) {
      return indices_acc->count-2;
    }
    if (mode==TINYGLTF_MODE_TRIANGLE_FAN) {
      return 1;
    }
    std::cout << "TINYGLTF mode wrong in NumFaces() " << mode << std::endl;
    return 0;
  }
  virtual int NumPoints(int face) const { 
    if (mode==TINYGLTF_MODE_TRIANGLE_FAN && indices_acc) {
      return indices_acc->count;
    }
    return 3;
  }
  int get_index(int face, int point) const
  {
	unsigned char *ptr = &indices_buf->data[0];
	//int size1 = indices_buf->data.size();
	unsigned char *ptr2 = ptr + indices_bv->byteOffset;
	//int size2 =indices_bv->byteLength;
	int stride = indices_bv->byteStride;

	switch(indices_acc->componentType) {
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	  if (stride==0) stride=sizeof(unsigned char);
	  break;
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	  if (stride==0) stride=sizeof(char);
	  break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	    if (stride==0) stride=sizeof(unsigned short); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	    if (stride==0) stride=sizeof(short); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_INT:
	    if (stride==0) stride=sizeof(int); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	    if (stride==0) stride=sizeof(unsigned int); // 3 = num of indices in a ttiangle
	    break;
	default:
	  std::cout << "componentType wrong: " << indices_acc->componentType << std::endl;
	  if (stride==0) stride=sizeof(short);
	  break;
	};


	int comp = face*3+point;
	unsigned char *ptr3 = ptr2 + indices_acc->byteOffset + comp*stride;
	//int size3 = indices_acc->count;
	int index = 0;
	switch(indices_acc->componentType)
	  {
	  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	    {
	      unsigned char *ptr4 = (unsigned char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	  case TINYGLTF_COMPONENT_TYPE_BYTE:
	    {
	      char *ptr4 = (char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  {
	  unsigned short *ptr4 = (unsigned short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  {
	  short *ptr4 = (short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_INT:
	  {
	  int *ptr4 = (int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  {
	  unsigned int *ptr4 = (unsigned int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	  };
	return index;
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (position_bv && position_acc && position_buf) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &position_buf->data[0];
	int stride2 = position_bv->byteStride;
	if (stride2==0) stride2 = 3*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + position_bv->byteOffset + index*stride2 + position_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return Point(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &position_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + position_bv->byteOffset;
	int stride = position_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + position_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return Point(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
      }
    }
    } else {
      return Point(0.0,0.0,0.0);
    }
    std::cout << "gltf FacePoint unknown mode" << std::endl;
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const {
    if (normal_bv && normal_acc && normal_buf) {

    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &normal_buf->data[0];
	int stride2 = normal_bv->byteStride;
	if (stride2==0) stride2 = 3*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + normal_bv->byteOffset + index*stride2 + normal_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return -Vector(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// TODO, check that this branch works
	unsigned char *pos_ptr = &normal_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + normal_bv->byteOffset;
	int stride = normal_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + normal_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return -Vector(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
      }
    }
    } else {
      return Vector(0.0,0.0,0.0);
    }
    std::cout << "gltf PointNormal unknown mode" << std::endl;
    return Vector(0.0,0.0,0.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const {
    if (color_bv && color_acc && color_buf) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &color_buf->data[0];
	int stride2 = color_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	unsigned int *pos_ptr2 = (unsigned int*)(pos_ptr + color_bv->byteOffset + index*stride2 + color_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return *pos_ptr2;
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &color_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + color_bv->byteOffset;
	int stride = color_bv->byteStride;
	if (stride==0) stride=4*sizeof(unsigned char);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + color_acc->byteOffset + comp*stride;
	unsigned int *pos_ptr4 = (unsigned int*)pos_ptr3; // 3 = num of components in (x,y,z)
	return *pos_ptr4;
      }
    }
    } else {
      return 0xffffffff;
    }

    std::cout << "gltf Color unknown mode" << std::endl;
    return 0xffffffff;

  }
  virtual Point2d TexCoord(int face, int point) const { 
    Point p = tex(face,point);
    //std::cout << "TexCoord: " << p.x << " " << p.y << std::endl;
    Point2d pp; pp.x=p.x; pp.y=p.y; return pp; 
  }
  virtual float TexCoord3(int face, int point) const { 
    Point p = tex(face,point);
    return p.z;
  }

  Point tex(int face, int point) const
  {
    if (texcoord_bv && texcoord_acc && texcoord_buf) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &texcoord_buf->data[0];
	int stride2 = texcoord_bv->byteStride;
	if (stride2==0) stride2 = 2*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + texcoord_bv->byteOffset + index*stride2 + texcoord_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return Point(pos_ptr2[0], pos_ptr2[1], 0.0 /*pos_ptr2[2]*/);
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &texcoord_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + texcoord_bv->byteOffset;
	int stride = texcoord_bv->byteStride;
	if (stride==0) stride=2*sizeof(float);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + texcoord_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return Point(pos_ptr4[0], pos_ptr4[1], 0.0 /*pos_ptr4[2]*/);
      }
    }
    } else {
      return Point(0.0,0.0,0.0);
    }
    std::cout << "gltf TexCoord unknown mode" << std::endl;
    return Point(0.0,0.0,0.0);
  }

private:
  tinygltf::Primitive *prim;
  tinygltf::Model *model;
  int mode;
  int indices_index;
  int position_index;
  int normal_index;
  int texcoord_index;
  int color_index;

  tinygltf::Accessor *indices_acc;
  tinygltf::Accessor *position_acc;
  tinygltf::Accessor *normal_acc;
  tinygltf::Accessor *texcoord_acc;
  tinygltf::Accessor *color_acc;

  tinygltf::BufferView *indices_bv;
  tinygltf::BufferView *position_bv;
  tinygltf::BufferView *normal_bv;
  tinygltf::BufferView *texcoord_bv;
  tinygltf::BufferView *color_bv;

  tinygltf::Buffer *indices_buf;
  tinygltf::Buffer *position_buf;
  tinygltf::Buffer *normal_buf;
  tinygltf::Buffer *texcoord_buf;
  tinygltf::Buffer *color_buf;

  LoadGltf *load;
  int mesh_index;
  int prim_index;
};

GameApi::BM GameApi::PolygonApi::gltf_load_bitmap( GameApi::EveryApi &ev, std::string base_url, std::string url, int image_index )
{
  if (image_index==-1) {
    return ev.bitmap_api.newbitmap(1,1, 0xffffffff);
  }
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  GLTFImage *img = new GLTFImage( load, image_index );

  Bitmap<Color> *img2 = new MemoizeBitmap(*img);

  std::stringstream ss;
  ss << image_index;

  Bitmap<Color> *bbm = new BitmapPrepareCache(e, url + ss.str(), img2);

  ::Bitmap<Color> *b = bbm;
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  //BM bm2 = ev.bitmap_api.flip_y(bm);
  //BM bm3 = ev.bitmap_api.flip_x(bm);
  return bm;

}
GameApi::BM gltf_load_bitmap2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int image_index)
{
  if (image_index==-1) {
    return ev.bitmap_api.newbitmap(1,1,0xffffffff);
  }

  GLTFImage *img = new GLTFImage( load, image_index );

  Bitmap<Color> *img2 = new MemoizeBitmap(*img);

  std::stringstream ss;
  ss << image_index;

  Bitmap<Color> *bbm = new BitmapPrepareCache(e, load->url + ss.str(), img2);

  ::Bitmap<Color> *b = bbm;
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  GameApi::BM bm = add_bitmap(e, handle2);
  //GameApi::BM bm2 = ev.bitmap_api.flip_y(bm);
  //GameApi::BM bm3 = ev.bitmap_api.flip_x(bm);
  return bm;
}
std::map<std::string, bool> g_gltf_cache;
GameApi::P gltf_load2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_index, int prim_index )
{
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  GameApi::P p = add_polygon2(e, faces,1);
  GameApi::P p2 = ev.polygon_api.file_cache(p, load->url, prim_index+mesh_index*50);
  std::stringstream ss;
  ss << load->url;
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }
  //GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
 return p2;
} 
GameApi::P GameApi::PolygonApi::gltf_load( GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  P p = add_polygon2(e, faces,1);
  GameApi::P p2 = ev.polygon_api.file_cache(p, load->url, prim_index+mesh_index*50);
  std::stringstream ss;
  ss << load->url;
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }

  GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
  return p3;
}

class GLTF_Material2 : public MaterialForward
{
public:
  GLTF_Material2(GameApi::Env &e, GameApi::EveryApi &ev, float mix, float roughness, float metallic, float basecolor_r, float basecolor_g, float basecolor_b, float basecolor_a, float occul_strength) : e(e), ev(ev), mix(mix), roughness(roughness), metallic(metallic), base_r(basecolor_r), base_g(basecolor_g), base_b(basecolor_b), base_a(basecolor_a), occul(occul_strength) { }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    std::vector<GameApi::BM> bm;

    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); // todo base color
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    std::vector<GameApi::BM> bm;

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); 
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    std::vector<GameApi::BM> bm;

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,p,ms,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); 
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst2(p.id,pta.id);
    std::cout << "ERROR gltf::mat2inst2" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    std::cout << "ERROR gltf::mat_inst_fade" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;

  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  float mix;
  float roughness, metallic, base_r, base_g, base_b, base_a, occul;
};



class GLTF_Material : public MaterialForward
{
public:
  GLTF_Material(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int material_id, float mix) : e(e), ev(ev),  load(load), material_id(material_id),mix(mix) { 
  }
  int num_textures() const {
    return 5; // (1=base color, 2=metallicroughness), 3=normal, 4=occulsion, 5=emissive
  }
  GameApi::BM texture(int i) const {
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    }
    switch(i) {
    case 0: return gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index);
    case 1: return gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index);
    case 2: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].normalTexture.index);
    case 3: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].occlusionTexture.index);
    case 4: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].emissiveTexture.index);
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
    switch(i) {
    case 0: return load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index!=-1;
    case 1: return load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index!=-1;
    case 2: return load->model.materials[material_id].normalTexture.index!=-1;
    case 3: return load->model.materials[material_id].occlusionTexture.index!=-1;
    case 4: return load->model.materials[material_id].emissiveTexture.index!=-1;
    default: return false;
    };
  }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_textures();
    for(int i=0;i<s;i++) bm.push_back(texture(i));
    //GameApi::ML I13;
    //I13.id = next->mat(p.id);
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0); // todo base color
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_textures();
    for(int i=0;i<s;i++) bm.push_back(texture(i));
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,bm);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_textures();
    for(int i=0;i<s;i++) bm.push_back(texture(i));
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,p,ms,bm);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst2(p.id,pta.id);
    std::cout << "ERROR gltf::mat2inst2" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    std::cout << "ERROR gltf::mat_inst_fade" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;

  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  LoadGltf *load;
  int material_id;
  float mix;
};


class GLTF_Material_env : public MaterialForward
{
public:
  GLTF_Material_env(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int material_id, float mix, GameApi::BM diffuse_env, GameApi::BM specular_env, GameApi::BM brfd) : e(e), ev(ev),  load(load), material_id(material_id),mix(mix), diffuse_env(diffuse_env), specular_env(specular_env), bfrd(brfd) { 
    // TODO, HOW TO CALL PREPARE?
    
  }
  int num_textures() const {
    return 8; // (1=base color, 2=metallicroughness), 3=normal, 4=occulsion, 5=emissive, 6=env_bm
  }
  GameApi::BM texture(int i) const {
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    }
    switch(i) {
    case 0: return gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index);
    case 1: return gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index);
    case 2: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].normalTexture.index);
    case 3: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].occlusionTexture.index);
    case 4: return gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].emissiveTexture.index);
    case 5: return diffuse_env;
    case 6: return specular_env;
    case 7: return bfrd;
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
    switch(i) {
    case 0: return load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index!=-1;
    case 1: return load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index!=-1;
    case 2: return load->model.materials[material_id].normalTexture.index!=-1;
    case 3: return load->model.materials[material_id].occlusionTexture.index!=-1;
    case 4: return load->model.materials[material_id].emissiveTexture.index!=-1;
    case 5: return true;
    case 6: return true;
    case 7: return true;
    default: return false;
    };
  }
  int type(int i) const
  {
    switch(i) { 
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return 0; // sampler2D
    case 5:
    case 6:
      return 1; // samplerCube
    case 7:
      return 0; // sampler2D
    default:
      return 0;
    };
  }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    std::vector<int> types;
    int s = num_textures();
    for(int i=0;i<s;i++) { bm.push_back(texture(i)); types.push_back(type(i)); }
    //GameApi::ML I13;
    //I13.id = next->mat(p.id);
    GameApi::P I10=p; 
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm,types);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0); // todo base color
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    std::vector<int> types;
    int s = num_textures();
    for(int i=0;i<s;i++) { bm.push_back(texture(i)); types.push_back(type(i)); }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,p,pts,bm,types);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    std::vector<int> types;
    int s = num_textures();
    for(int i=0;i<s;i++) { bm.push_back(texture(i)); types.push_back(type(i)); }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,p,ms,bm,types);
    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I19;

  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst2(p.id,pta.id);
    std::cout << "ERROR gltf::mat2inst2" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    //GameApi::ML I13;
    //I13.id = next->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
    std::cout << "ERROR gltf::mat_inst_fade" << std::endl;
    GameApi::ML ml;
    ml.id=-1;
    return ml;

  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  LoadGltf *load;
  int material_id;
  float mix;
  GameApi::BM diffuse_env, specular_env, bfrd;
};

GameApi::MT gltf_material2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int material_id, float mix )
  {
    Material *mat = new GLTF_Material(e,ev, load, material_id, mix);
  return add_material(e, mat);
  }
GameApi::MT GameApi::MaterialsApi::gltf_material3( GameApi::EveryApi &ev, float roughness, float metallic, float base_r, float base_g, float base_b, float base_a, float mix)
{
  return add_material(e, new GLTF_Material2(e,ev, mix, roughness, metallic, base_r, base_g, base_b, base_a,1.0));
}

GameApi::MT GameApi::MaterialsApi::gltf_material( EveryApi &ev, std::string base_url, std::string url, int material_id, float mix )
  {
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material(e,ev, load, material_id,mix);
  return add_material(e, mat);
} 

GameApi::MT GameApi::MaterialsApi::gltf_material_env( EveryApi &ev, std::string base_url, std::string url, int material_id, float mix, BM diffuse_env, BM specular_env, BM bfrd )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material_env(e,ev, load, material_id,mix, diffuse_env, specular_env, bfrd);
  return add_material(e, mat);

}
std::pair<float,Point> find_mesh_scale(FaceCollection *coll);


GameApi::ML scale_to_gltf_size(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::ML ml)
{
      FaceCollection *coll = find_facecoll(e,p);
      std::pair<float,Point> dim = find_mesh_scale(coll);

    GameApi::MN I4=ev.move_api.empty();
    GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
    GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
    GameApi::ML I7=ev.move_api.move_ml(ev,ml,I6,1,10.0);

    return I7;
}

GameApi::ML gltf_mesh2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id);

GameApi::ML gltf_node2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int node_id )
{
  int s2 = load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
    GameApi::P empty = ev.polygon_api.empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  tinygltf::Node *node = &load->model.nodes[node_id];

  // Load mesh
  int mesh_id = node->mesh;
  GameApi::ML mesh;
  mesh.id = -1;
  if (mesh_id!=-1) {
    mesh = gltf_mesh2( e, ev, load, mesh_id );
  }

  // todo cameras

  // recurse children
  int s = node->children.size();
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
    int child_id = node->children[i];
    if (child_id!=-1) {
      GameApi::ML ml = gltf_node2( e, ev, load, child_id );
      vec.push_back(ml);
    }
  }
  if (mesh.id != -1)
    vec.push_back( mesh );
  GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);

  GameApi::MN mv = ev.move_api.empty();
  if (int(node->translation.size())==3) {
    double m_x = node->translation[0];
    double m_y = node->translation[1];
    double m_z = node->translation[2];
    mv = ev.move_api.trans2(mv, m_x, m_y, m_z);
  }
  if (int(node->rotation.size())==4) {
    double r_x = node->rotation[0];
    double r_y = node->rotation[1];
    double r_z = node->rotation[2];
    double r_w = node->rotation[3];
    Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
    Matrix m = Quarternion::QuarToMatrix(q);
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(e, mv2);
  }
  if (int(node->scale.size())==3) {
    double s_x = node->scale[0];
    double s_y = node->scale[1];
    double s_z = node->scale[2];
    mv = ev.move_api.scale2(mv, s_x, s_y, s_z);
  }
  if (int(node->matrix.size())==16) {
    double *arr = &node->matrix[0];
    Matrix m;
    for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(e, mv2);    
  }
  GameApi::ML ret = ev.move_api.move_ml(ev, array, mv, 1, 10.0 );
  return ret;
}

GameApi::ML gltf_scene2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int scene_id )
{
  int s2 = load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
    GameApi::P empty = ev.polygon_api.empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  tinygltf::Scene *scene = &load->model.scenes[scene_id];
  int s = scene->nodes.size();
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_node2( e, ev, load, scene->nodes[i] );
    vec.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, vec);
}


GameApi::ML gltf_mesh2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id)
{
  if (mesh_id>=0 && mesh_id<int(load->model.meshes.size())) {
    int s = load->model.meshes[mesh_id].primitives.size();
    std::vector<GameApi::ML> mls;
    for(int i=0;i<s;i++) {
      GameApi::P p = gltf_load2(e, ev, load, mesh_id, i);
      int mat = load->model.meshes[mesh_id].primitives[i].material;
      GameApi::MT mat2 = gltf_material2(e, ev, load, mat, 1.0);
      GameApi::ML ml = ev.materials_api.bind(p,mat2);
      mls.push_back(ml);
    }
    GameApi::ML ml = ev.mainloop_api.array_ml(ev, mls);
    return ml;
  } else {
    GameApi::P empty = ev.polygon_api.empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
  }
}
GameApi::ML GameApi::MainLoopApi::gltf_mesh( GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_id )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);

  GameApi::ML ml = gltf_mesh2(e,ev,load, mesh_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
}
GameApi::ML GameApi::MainLoopApi::gltf_node( GameApi::EveryApi &ev, std::string base_url, std::string url, int node_id )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  GameApi::ML ml = gltf_node2(e,ev,load,node_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
  //return ml;
}

GameApi::ML GameApi::MainLoopApi::gltf_scene( GameApi::EveryApi &ev, std::string base_url, std::string url, int scene_id )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  GameApi::ML ml = gltf_scene2(e,ev,load,scene_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
  //return ml;
}


GameApi::ML gltf_mesh_all2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load )
{
  int s = load->model.meshes.size();
  std::vector<GameApi::ML> mls;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_mesh2( e, ev, load, i );
    mls.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, mls);
}
GameApi::ML GameApi::MainLoopApi::gltf_mesh_all( GameApi::EveryApi &ev, std::string base_url, std::string url )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);

  GameApi::ML ml = gltf_mesh_all2( e, ev, load );
  return scale_to_gltf_size(e,ev,mesh,ml);
}

