
#include "GameApi_h.hh"

#define TINYGLTF_IMPLEMENTATION


#include "tiny_gltf.h"

extern std::string gameapi_homepageurl;


bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr);
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr);

bool FileExists(const std::string &abs_filename, void *ptr);
std::string ExpandFilePath(const std::string &str, void *ptr);
bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *ptr);
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char> &contents, void *ptr);



class LoadGltf
{
public:
  LoadGltf(GameApi::Env &e, std::string base_url, std::string url, std::string homepage, bool is_binary) : e(e), base_url(base_url), url(url), homepage(homepage), is_binary(is_binary) {
    tinygltf::FsCallbacks fs = {
      &FileExists,
      &ExpandFilePath,
      &ReadWholeFile,
      &WriteWholeFile,
      (void*)this
    };
    tiny.SetFsCallbacks(fs);
    tiny.SetImageLoader(&LoadImageData, this);
    tiny.SetImageWriter(&WriteImageData, this);
  }
  void Prepare() {

#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    std::string str(vec->begin(),vec->end());
    bool b = false;
    std::string err;
    std::string warn;
    if (!is_binary) {
      b = tiny.LoadASCIIFromString(&model, &err, &warn, str.c_str(), str.size(), base_url, tinygltf::REQUIRE_ALL);
    } else {
      b = tiny.LoadBinaryFromMemory(&model, &err, &warn, &vec->operator[](0), vec->size(), base_url, tinygltf::REQUIRE_ALL); 
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

bool FileExists(const std::string &abs_filename, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "FileExists " << abs_filename << std::endl;
}
std::string ExpandFilePath(const std::string &str, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "ExpandFilePath " << str << std::endl;
  return data->base_url + "/" + str;
}
bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "ReadWholeFile " << filepath << std::endl;
}
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char> &contents, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "WriteWholeFile" << filepath << std::endl;
}
bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "LoadImageData " << req_width << " " << req_height << " " << size << std::endl;
}
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr)
{
  LoadGltf *data = (LoadGltf*)ptr;
  std::cout << "WriteImageData " << *basepath << " " << *filename << std::endl;
}

class GLTFImage : public Bitmap<Color>
{
public:
  GLTFImage(LoadGltf *load, tinygltf::Image *img) : load(load), img(img) { 
  }
  virtual int SizeX() const { return img->width; }
  virtual int SizeY() const { return img->height; }
  virtual Color Map(int x, int y) const
  {
    unsigned char *ptr = &img->image[0];
    int offset = (x*img->component + y*img->width*img->component)*(img->bits/8);
    if (img->component==4 && img->bits==8) {
      unsigned int c0 = *(unsigned int*)(ptr+offset);
      unsigned int cr = c0 & 0xff;
      unsigned int cg = c0 & 0xff00;
      unsigned int cb = c0 & 0xff0000;
      unsigned int ca = c0 & 0xff000000;
      cg>>=8;
      cb>>=16;
      ca>>=24;
      
      ca<<=24;
      cr<<=16;
      cg<<=8;
      cb<<=0;
      return Color(ca+cr+cg+cb);
    }
    if (img->component==4 && img->bits==16) {
      unsigned short *c0 = (unsigned short*)(ptr+offset);
      unsigned short r = c0[0];
      unsigned short g = c0[1];
      unsigned short b = c0[2];
      unsigned short a = c0[3];
      r>>=8;
      g>>=8;
      b>>=8;
      a>>=8;
      unsigned int rr = r;
      unsigned int gg = g;
      unsigned int bb = b;
      unsigned int aa = a;
      aa<<=24;
      rr<<=16;
      gg<<=8;
      return Color(rr+gg+bb+aa);
    }
    std::cout << "GLTF Image format not regognized" << std::endl;
    return Color(0x0);
  }
  virtual void Prepare()
  {
  }
private:
  LoadGltf *load;
  tinygltf::Image *img;
};

class GLTFFaceCollection : public FaceCollection
{
public:
  GLTFFaceCollection( LoadGltf *load, int mesh_index, int prim_index) : load(load), mesh_index(mesh_index), prim_index(prim_index) {
  }
  virtual void Prepare() { 
    load->Prepare();

    if (mesh_index>=0 && mesh_index<load->model.meshes.size() && prim_index>=0 && prim_index<load->model.meshes[mesh_index].primitives.size())
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else
      return;
    model = &load->model;
    
    mode = prim->mode;

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
	return Vector(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// TODO, check that this branch works
	unsigned char *pos_ptr = &normal_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + normal_bv->byteOffset;
	int stride = normal_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + normal_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return Vector(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
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
	if (stride2==0) stride2 = 3*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + texcoord_bv->byteOffset + index*stride2 + texcoord_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return Point(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &texcoord_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + texcoord_bv->byteOffset;
	int stride = texcoord_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + texcoord_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return Point(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
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

GameApi::P GameApi::PolygonApi::gltf_load( std::string base_url, std::string url, int mesh_index, int prim_index )
{
  bool is_binary=false;
  if (url.size()>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  return add_polygon2(e, faces,1);
}
