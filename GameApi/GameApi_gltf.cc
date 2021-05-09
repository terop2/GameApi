
#include "GameApi_h.hh"
#include "GraphI.hh"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

extern std::string gameapi_homepageurl;

void confirm_texture_usage(GameApi::Env &e, GameApi::P p);

class LoadGltf;

GameApi::LI gltf_anim_skeleton2(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index);


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
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (bitmap_find_data(id)!=-1) {
      return;
    }
    GameApi::BM num = add_color_bitmap2(e, bm);
    bitmap_prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
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



class LoadGltf : public CollectInterface
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
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void Prepare() {
    if (prepare_done) return;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    std::vector<unsigned char> *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "LoadGLTF ASync not ready!" << std::endl; return; }
    std::vector<char> vec2(vec->begin(), vec->end());
    std::string str(vec->begin(),vec->end());

    //std::cout << str << std::endl;
    //bool b = false;
    std::string err;
    std::string warn;
    if (!is_binary) {
      std::cout << "File size: " << url  << "::" << str.size() << std::endl;
      int sz = str.size();
#ifdef EMSCRIPTEN
      sz--;
#endif
      tiny.LoadASCIIFromString(&model, &err, &warn, &vec2.operator[](0), sz, base_url, tinygltf::REQUIRE_ALL);
    } else {
      int sz = vec->size();
      std::cout << "File size: " << url  << "::" << sz << std::endl;
#ifdef EMSCRIPTEN
      sz--;
#endif
      tiny.LoadBinaryFromMemory(&model, &err, &warn, &vec->operator[](0), sz, base_url, tinygltf::REQUIRE_ALL); 
    }
    if (!warn.empty()) { std::cout << "WARN: " << warn << std::endl; }
    if (!err.empty()) { std::cout << "ERROR: " << err << std::endl; }
    prepare_done = true;
  }
public:
  GameApi::Env &e;
  std::string base_url;
  std::string url;
  std::string homepage;
  bool is_binary;
  tinygltf::TinyGLTF tiny;
  tinygltf::Model model;
  bool prepare_done = false;
};

struct KeyStruct
{
  std::string key;
  LoadGltf *obj;
};

std::vector<KeyStruct> g_gltf_instances;

LoadGltf *find_gltf_instance(GameApi::Env &e, std::string base_url, std::string url, std::string homepage, bool is_binary)
{
  std::string key = base_url + ":" + url;

  int s = g_gltf_instances.size();
  for(int i=0;i<s;i++) {
    if (g_gltf_instances[i].key == key) return g_gltf_instances[i].obj;
  }
  LoadGltf *obj = new LoadGltf(e,base_url,url,homepage,is_binary);
  //obj->Prepare();
  KeyStruct s2;
  s2.key = key;
  s2.obj = obj;
  g_gltf_instances.push_back(s2);
  return obj;
}


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
  //std::cout << "FileExists " << abs_filename << std::endl;
  return true;
}
std::string ExpandFilePath(const std::string &str, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "ExpandFilePath " << str << std::endl;
  return str;
}
bool is_in_registered(std::string url);
bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "ReadWholeFile " << filepath << std::endl;
  std::string url = filepath;
#ifndef EMSCRIPTEN
    g_e->async_load_url(url, gameapi_homepageurl);
#endif
    std::vector<unsigned char> *vec = g_e->get_loaded_async_url(url);
    if (!vec) { std::cout << "ReadWholeFile::async not ready:" << url << std::endl; std::cout << "Please use async_url() to register it to system" << std::endl; return false; } else {
      if (!is_in_registered(url))
	std::cout << "\nWarning: Please note that you might need to add async_url() for " << url << std::endl;
    }
    int sz = vec->size();
#ifdef EMSCRIPTEN
    sz--;
#endif
    *out = std::vector<unsigned char>(vec->begin(),vec->begin()+sz);
  return true;
}
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char> &contents, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "WriteWholeFile" << filepath << std::endl;
  return false;
}
bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "LoadImageData " << req_width << " " << req_height << " " << size << std::endl;
  return false;
}
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "WriteImageData " << *basepath << " " << *filename << std::endl;
  return false;
}

class GLTFImage : public Bitmap<Color>
{
public:
  GLTFImage(LoadGltf *load, int image_index) : load(load), image_index(image_index) { 
  }
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    img = 0;
    if (image_index>=0 && image_index<int(load->model.images.size()))
      img = &load->model.images[image_index];
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
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (mesh_index>=0 && mesh_index<int(load->model.meshes.size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else { std::cout << "Prim failed!" << std::endl;  return; }

    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
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
    joints_index = -1;
    weights_index = -1;
    if (prim->attributes.find("POSITION") != prim->attributes.end())
      position_index = prim->attributes["POSITION"];
    if (prim->attributes.find("NORMAL") != prim->attributes.end())
      normal_index = prim->attributes["NORMAL"];
    if (prim->attributes.find("TEXCOORD_0") != prim->attributes.end())
      texcoord_index = prim->attributes["TEXCOORD_0"];
    if (prim->attributes.find("COLOR_0") != prim->attributes.end())
      color_index = prim->attributes["COLOR_0"];
    if (prim->attributes.find("JOINTS_0") != prim->attributes.end())
      joints_index = prim->attributes["JOINTS_0"];
    if (prim->attributes.find("WEIGHTS_0") != prim->attributes.end())
      weights_index = prim->attributes["WEIGHTS_0"];

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

    if (indices_acc) {
      assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);
    }
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

    joints_acc = 0;
    if (joints_index!=-1)
      joints_acc = &model->accessors[joints_index];


    weights_acc = 0;
    if (weights_index!=-1)
      weights_acc = &model->accessors[weights_index];

    
    
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

    joints_bv = 0;
    if (joints_acc) {
      int view = joints_acc->bufferView;
      if (view!=-1)
	joints_bv = &model->bufferViews[view];
    }

    weights_bv = 0;
    if (weights_acc) {
      int view = weights_acc->bufferView;
      if (view!=-1)
	weights_bv = &model->bufferViews[view];
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

    joints_buf = 0;
    if (joints_bv) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &model->buffers[buf];
      }
    }

    weights_buf = 0;
    if (weights_bv) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &model->buffers[buf];
      }
    }

  }
  
  virtual void Prepare() { 
    //std::cout << "GLTFFaceCollection::Prepare()" << std::endl;
    load->Prepare();
    
    
    if (mesh_index>=0 && mesh_index<int(load->model.meshes.size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else { std::cout << "Prim failed!" << std::endl;  return; }

    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
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
    joints_index = -1;
    weights_index = -1;
    if (prim->attributes.find("POSITION") != prim->attributes.end())
      position_index = prim->attributes["POSITION"];
    if (prim->attributes.find("NORMAL") != prim->attributes.end())
      normal_index = prim->attributes["NORMAL"];
    if (prim->attributes.find("TEXCOORD_0") != prim->attributes.end())
      texcoord_index = prim->attributes["TEXCOORD_0"];
    if (prim->attributes.find("COLOR_0") != prim->attributes.end())
      color_index = prim->attributes["COLOR_0"];
    if (prim->attributes.find("JOINTS_0") != prim->attributes.end())
      joints_index = prim->attributes["JOINTS_0"];
    if (prim->attributes.find("WEIGHTS_0") != prim->attributes.end())
      weights_index = prim->attributes["WEIGHTS_0"];

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

    if (indices_acc) {
      assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);
    }
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

    joints_acc = 0;
    if (joints_index!=-1)
      joints_acc = &model->accessors[joints_index];


    weights_acc = 0;
    if (weights_index!=-1)
      weights_acc = &model->accessors[weights_index];

    
    
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

    joints_bv = 0;
    if (joints_acc) {
      int view = joints_acc->bufferView;
      if (view!=-1)
	joints_bv = &model->bufferViews[view];
    }

    weights_bv = 0;
    if (weights_acc) {
      int view = weights_acc->bufferView;
      if (view!=-1)
	weights_bv = &model->bufferViews[view];
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

    joints_buf = 0;
    if (joints_bv) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &model->buffers[buf];
      }
    }

    weights_buf = 0;
    if (weights_bv) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &model->buffers[buf];
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
    if (mode==TINYGLTF_MODE_TRIANGLES && position_acc) {
      return position_acc->count/3;
    }
    if (mode==TINYGLTF_MODE_TRIANGLE_STRIP && position_acc) {
      return position_acc->count-2;
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
  bool has_normal() const { return normal_bv && normal_acc&&normal_buf; }
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
  bool has_color() const { return true; /*color_bv && color_acc && color_buf*/; }
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
  virtual bool has_skeleton() const
  {
    return joints_bv && joints_acc && joints_buf && weights_bv && weights_acc && weights_buf;
  }
  virtual VEC4 Joints(int face, int point) const
  {

    if (joints_bv && joints_acc && joints_buf) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &joints_buf->data[0];
	int stride2 = joints_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	unsigned char *pos_ptr2 = (unsigned char*)(pos_ptr + joints_bv->byteOffset + index*stride2 + joints_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << std::hex << int(pos_ptr2[0]) << "," << int(pos_ptr2[1]) << "," << int(pos_ptr2[2]) << "," << int(pos_ptr2[3]) << std::endl;
	//vec4 res;
	//res[0] = pos_ptr2[0];
	//res[1] = pos_ptr2[1];
	//res[2] = pos_ptr2[2];
	//res[3] = pos_ptr2[3];
	//std::cout << "Attached end" << std::endl;
	VEC4 res;
	res.x = 0.5+int(((unsigned int)(pos_ptr2[0]))&0xff);
	res.y = 0.5+int(((unsigned int)(pos_ptr2[1]))&0xff);
	res.z = 0.5+int(((unsigned int)(pos_ptr2[2]))&0xff);
	res.w = 0.5+int(((unsigned int)(pos_ptr2[3]))&0xff);
	//std::cout << "Joints: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;

	return res;
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &joints_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + joints_bv->byteOffset;
	int stride = joints_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + joints_acc->byteOffset + comp*stride;
	unsigned char *pos_ptr4 = (unsigned char*)pos_ptr3; // 3 = num of components in (x,y,z)
	//std::cout << face << " " << point << "::" << int(pos_ptr4[0]) << std::endl;
	//res[0] = pos_ptr4[0];
	//res[1] = pos_ptr4[1];
	//res[2] = pos_ptr4[2];
	//res[3] = pos_ptr4[3];
	//return res;
	//std::cout << "Attached end" << std::endl;
	VEC4 res;
	res.x = 0.5+int(((unsigned int)(pos_ptr4[0]))&0xff);
	res.y = 0.5+int(((unsigned int)(pos_ptr4[1]))&0xff);
	res.z = 0.5+int(((unsigned int)(pos_ptr4[2]))&0xff);
	res.w = 0.5+int(((unsigned int)(pos_ptr4[3]))&0xff);
	//std::cout << "Joints2: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;
	return res;
	//return int(((unsigned int)(pos_ptr2[num]))&0xff);
      }
      }
    } else {
      //std::cout << "Attached end" << std::endl;
      VEC4 res;
      res.x = 0.0;
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
      //      return 0;
    }
    std::cout << "gltf attach unknown mode: "<< mode << std::endl;
      VEC4 res;
      res.x = 0.0;
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
      //    return 0;

    
  }
  virtual VEC4 Weights(int face, int point) const
  {

    if (weights_bv && weights_acc && weights_buf) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &weights_buf->data[0];
	int stride2 = weights_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + weights_bv->byteOffset + index*stride2 + weights_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << std::hex << int(pos_ptr2[0]) << "," << int(pos_ptr2[1]) << "," << int(pos_ptr2[2]) << "," << int(pos_ptr2[3]) << std::endl;
	//vec4 res;
	//res[0] = pos_ptr2[0];
	//res[1] = pos_ptr2[1];
	//res[2] = pos_ptr2[2];
	//res[3] = pos_ptr2[3];
	//std::cout << "Attached end" << std::endl;
	VEC4 res;
	res.x = pos_ptr2[0];
	res.y = pos_ptr2[1];
	res.z = pos_ptr2[2];
	res.w = pos_ptr2[3];
	//std::cout << "Weights: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;
	return res;
	//return pos_ptr2[num];
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &weights_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + weights_bv->byteOffset;
	//int stride = weights_bv->byteStride;
	//if (stride==0) stride=4*sizeof(char);
	//int comp = face*3+point;
	//unsigned char *pos_ptr3 = pos_ptr2 + weights_acc->byteOffset + comp*stride;
	//float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	//std::cout << face << " " << point << "::" << int(pos_ptr4[0]) << std::endl;
	//res[0] = pos_ptr4[0];
	//res[1] = pos_ptr4[1];
	//res[2] = pos_ptr4[2];
	//res[3] = pos_ptr4[3];
	//return res;
	//std::cout << "Attached end" << std::endl;
	VEC4 res;
	res.x = pos_ptr2[0];
	res.y = pos_ptr2[1];
	res.z = pos_ptr2[2];
	res.w = pos_ptr2[3];
	//std::cout << "Weights2: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;
	return res;
	//return pos_ptr4[num];
      }
      }
    } else {
      //std::cout << "Attached end" << std::endl;
      VEC4 res;
      res.x = 0.0;
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
    }
    std::cout << "gltf weights unknown mode: "<< mode << std::endl;
    VEC4 res;
    res.x = 0.0;
    res.y = 0.0;
    res.z = 0.0;
    res.w = 0.0;
    return res;
    
    
  }
  
  bool has_texcoord() const { return texcoord_bv&&texcoord_acc&&texcoord_buf; }
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
  int joints_index;
  int weights_index;

  tinygltf::Accessor *indices_acc;
  tinygltf::Accessor *position_acc;
  tinygltf::Accessor *normal_acc;
  tinygltf::Accessor *texcoord_acc;
  tinygltf::Accessor *color_acc;
  tinygltf::Accessor *joints_acc;
  tinygltf::Accessor *weights_acc;
  
  tinygltf::BufferView *indices_bv;
  tinygltf::BufferView *position_bv;
  tinygltf::BufferView *normal_bv;
  tinygltf::BufferView *texcoord_bv;
  tinygltf::BufferView *color_bv;
  tinygltf::BufferView *joints_bv;
  tinygltf::BufferView *weights_bv;
  
  tinygltf::Buffer *indices_buf;
  tinygltf::Buffer *position_buf;
  tinygltf::Buffer *normal_buf;
  tinygltf::Buffer *texcoord_buf;
  tinygltf::Buffer *color_buf;
  tinygltf::Buffer *joints_buf;
  tinygltf::Buffer *weights_buf;

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

  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //   new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  GLTFImage *img = new GLTFImage( load, image_index );

  //Bitmap<Color> *img2 = new MemoizeBitmap(*img);

  std::stringstream ss;
  ss << image_index;

  Bitmap<Color> *bbm = new BitmapPrepareCache(e, url + ss.str(), img);

  ::Bitmap<Color> *b = bbm;
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  //BM bm2 = ev.bitmap_api.flip_y(bm);
  //BM bm3 = ev.bitmap_api.flip_x(bm);
  return bm;

}

struct CacheItem
{
  std::string url;
  GameApi::BM bitmap;
};
std::vector<CacheItem> g_bitmap_cache;




GameApi::BM gltf_load_bitmap2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int image_index)
{
  if (image_index==-1) {
    return ev.bitmap_api.newbitmap(1,1,0xffffffff);
  }


  std::stringstream ss;
  ss << image_index;
  
  //int s = g_bitmap_cache.size();
  //for(int i=0;i<s;i++) {
  //  const CacheItem *item = &g_bitmap_cache[i];
  //  if (item->url == load->url + ss.str()) { return g_bitmap_cache[i].bitmap; }
  //}
  

  GLTFImage *img = new GLTFImage( load, image_index );

  //Bitmap<Color> *img2 = new MemoizeBitmap(*img);


  Bitmap<Color> *bbm = new BitmapPrepareCache(e, load->url + ss.str(), img);

  ::Bitmap<Color> *b = bbm;
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  GameApi::BM bm = add_bitmap(e, handle2);
  //GameApi::BM bm2 = ev.bitmap_api.flip_y(bm);
  //GameApi::BM bm3 = ev.bitmap_api.flip_x(bm);

  //CacheItem item;
  //item.url = load->url + ss.str();
  //item.bitmap = bm;
  //g_bitmap_cache.push_back(item);
  return bm;
}
std::map<std::string, bool> g_gltf_cache;
GameApi::P gltf_load2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_index, int prim_index )
{
  int c = get_current_block();
  set_current_block(-1);
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  GameApi::P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);
  confirm_texture_usage(e,p);
  GameApi::P p2 = ev.polygon_api.file_cache(p, load->url, prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << load->url;
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }
  //GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
  GameApi::P p3 = ev.polygon_api.flip_normals(p2);
 return p3;
}
GameApi::P resize_to_correct_size2(GameApi::Env &e, GameApi::P model, Matrix *mat);
extern Matrix g_last_resize;

GameApi::P GameApi::PolygonApi::gltf_load_nr( GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index)
{

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  int c = get_current_block();
  set_current_block(-1);
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);

  confirm_texture_usage(e,p);
  GameApi::P p2 = ev.polygon_api.file_cache(p, load->url, prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << load->url;
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }

  GameApi::P p3 = resize_to_correct_size2(e,p2,&g_last_resize);
  GameApi::P p4 = ev.polygon_api.flip_normals(p3);
  return p4;

}
GameApi::P GameApi::PolygonApi::gltf_load( GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index )
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  int c = get_current_block();
  set_current_block(-1);
  FaceCollection *faces = new GLTFFaceCollection( load, mesh_index, prim_index );
  P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);

  confirm_texture_usage(e,p);
  GameApi::P p2 = ev.polygon_api.file_cache(p, load->url, prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << load->url;
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }

  GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
  GameApi::P p4 = ev.polygon_api.flip_normals(p3);
  return p4;
}

class GLTF_Material2 : public MaterialForward
{
public:
  GLTF_Material2(GameApi::Env &e, GameApi::EveryApi &ev, float mix, float roughness, float metallic, float basecolor_r, float basecolor_g, float basecolor_b, float basecolor_a, float occul_strength) : e(e), ev(ev), mix(mix), roughness(roughness), metallic(metallic), base_r(basecolor_r), base_g(basecolor_g), base_b(basecolor_b), base_a(basecolor_a), occul(occul_strength) { }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    if (!coll->has_color())
      {
	p = ev.polygon_api.color(p,0xffffffff);
      }
    if (!coll->has_normal())
      {
	p = ev.polygon_api.recalculate_normals(p);
      }
      
    
    std::vector<GameApi::BM> bm;

    //GameApi::P I10=p; 
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); // todo base color
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    if (!coll->has_color())
      {
	p = ev.polygon_api.color(p,0xffffffff);
      }
    if (!coll->has_normal())
      {
	p = ev.polygon_api.recalculate_normals(p);
      }

    std::vector<GameApi::BM> bm;
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,I10,pts,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); 
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    if (!coll->has_color())
      {
	p = ev.polygon_api.color(p,0xffffffff);
      }
    if (!coll->has_normal())
      {
	p = ev.polygon_api.recalculate_normals(p);
      }

    std::vector<GameApi::BM> bm;
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,I10,ms,bm);
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0); 
    // GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

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
  int num_indexes() const {
    int s = 5;
    int count = 0;
    for(int i=0;i<s;i++) {
      if (has_texture(i)) count++;
    }
    return count;
  }
  int map_index(int j) const
  {
    int s = 5;
    int count = 0;
    int last_tex = 0;
    for(int i=0;i<s;i++) {
      if (has_texture(i)) last_tex=i;
      if (has_texture(i) && count==j)
	return i;
      if (has_texture(i)) count++;
    }
    return last_tex;
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
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      return false;
    }

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
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    //GameApi::ML I13;
    //I13.id = next->mat(p.id);
    //GameApi::P I10=p; 
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {
      tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
      tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
      I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0); // todo base color
    }
    // GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,I10,pts,bm);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {

    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,I10,ms,bm);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {

    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
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
  int num_indexes() const {
    int s = 8;
    int count = 0;
    for(int i=0;i<s;i++) {
      if (has_texture(i)) count++;
    }
    return count;
  }
  int map_index(int j) const
  {
    int s = 8;
    int count = 0;
    int last_tex = 0;
    for(int i=0;i<s;i++) {
      if (has_texture(i)) last_tex=i;
      if (has_texture(i) && count==j)
	return i;
      if (has_texture(i)) count++;
    }
    return last_tex;
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
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      return false;
    }

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
    int s = num_indexes();
    for(int i=0;i<s;i++) { 
      int j = map_index(i);
      bm.push_back(texture(j)); types.push_back(type(j)); 
    }
    //GameApi::ML I13;
    //I13.id = next->mat(p.id);
    //GameApi::P I10=p; 
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm,types);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {

    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0); // todo base color
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    std::vector<int> types;
    int s = num_indexes();
    for(int i=0;i<s;i++) { 
      int j = map_index(i);
      bm.push_back(texture(j)); types.push_back(type(j)); }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,I10,pts,bm,types);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {

    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    load->Prepare();
    std::vector<GameApi::BM> bm;
    std::vector<int> types;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j)); types.push_back(type(j)); }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,I10,ms,bm,types);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(load->model.materials.size())) {
      I18 = I17;
    } else {

    tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, r.baseColorFactor[0],r.baseColorFactor[1],r.baseColorFactor[2],r.baseColorFactor[3], o.strength, 1.0);
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

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
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
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
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material_env(e,ev, load, material_id,mix, diffuse_env, specular_env, bfrd);
  return add_material(e, mat);

}
std::pair<float,Point> find_mesh_scale(FaceCollection *coll);


class MatrixLineCollection : public LineCollection
{
public:
  MatrixLineCollection(Matrix m, LineCollection &coll) : m(m),coll(coll) { }
  void Collect(CollectVisitor &vis)
  {
    coll.Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { coll.Prepare(); }

  virtual int NumLines() const { return coll.NumLines(); }
  virtual Point LinePoint(int line, int point) const 
  {
    return coll.LinePoint(line,point)*m;
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; }  

private:
  Matrix m;
  LineCollection &coll;
};

class ScaleToGltf_li : public LineCollection
{
public:
  ScaleToGltf_li(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::LI li) : env(e), ev(ev), p(p), li(li) { }

  void Collect(CollectVisitor &vis)
  {
      FaceCollection *coll = find_facecoll(env,p);
      coll->Collect(vis);
      vis.register_obj(this);
  }
  void HeavyPrepare()
  {
      FaceCollection *coll = find_facecoll(env,p);
      std::pair<float,Point> dim = find_mesh_scale(coll);
      GameApi::MN I4=ev.move_api.mn_empty();
      GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
      GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
      Movement *move = find_move(env,I6);
      Matrix m = move->get_whole_matrix(0.0, 1.0);
      //g_last_resize=m;
      GameApi::M m2 = add_matrix2(env,m);
      res= ev.lines_api.li_matrix(li,m2);
      LineCollection *coll2 = find_line_array(env,li);
      coll2->Prepare();
  }
  void Prepare()
  {
      FaceCollection *coll = find_facecoll(env,p);
      coll->Prepare();
      std::pair<float,Point> dim = find_mesh_scale(coll);
      GameApi::MN I4=ev.move_api.mn_empty();
      GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
      GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
      Movement *move = find_move(env,I6);
      Matrix m = move->get_whole_matrix(0.0, 1.0);
      //g_last_resize=m;
      GameApi::M m2 = add_matrix2(env,m);
      res= ev.lines_api.li_matrix(li,m2);
      LineCollection *coll2 = find_line_array(env,li);
      coll2->Prepare();

  }
  virtual int NumLines() const
  {
    LineCollection *coll = find_line_array(env, res);
    return coll->NumLines();
  }
  virtual Point LinePoint(int line, int point) const
  {
    LineCollection *coll = find_line_array(env, res);
    return coll->LinePoint(line,point);
  }
  virtual unsigned int LineColor(int line, int point) const {

    LineCollection *coll = find_line_array(env, res);
    return coll->LineColor(line,point);
  }

  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::LI li;
  GameApi::LI res;
};



GameApi::LI scale_to_gltf_size_li(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::LI li)
{
  return add_line_array(e, new ScaleToGltf_li(e,ev,p,li));
}

class ScaleToGLTF_p : public FaceCollection
{
public:
  ScaleToGLTF_p(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::P p2) : env(e), ev(ev), p(p), p2(p2) { res.id=-1; }
  void Collect(CollectVisitor &vis)
  {
  FaceCollection *coll = find_facecoll(env,p);
  coll->Collect(vis);
  FaceCollection *coll2 = find_facecoll(env,p2);
  coll2->Collect(vis);
  vis.register_obj(this);
  }
  void HeavyPrepare()
  {
  FaceCollection *coll = find_facecoll(env,p);
    std::pair<float,Point> dim = find_mesh_scale(coll);
    GameApi::MN I4=ev.move_api.mn_empty();
    GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
    GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
    Movement *move = find_move(env,I6);
    Matrix m = move->get_whole_matrix(0.0, 1.0);
    //g_last_resize=m;
    GameApi::M m2 = add_matrix2(env,m);
    res = ev.polygon_api.matrix(p2,m2);
  
  }
  void Prepare()
  {
  FaceCollection *coll = find_facecoll(env,p);
  coll->Prepare();
  std::pair<float,Point> dim = find_mesh_scale(coll);
  GameApi::MN I4=ev.move_api.mn_empty();
  GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
  GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
  Movement *move = find_move(env,I6);
  Matrix m = move->get_whole_matrix(0.0, 1.0);
  //g_last_resize=m;
  GameApi::M m2 = add_matrix2(env,m);
  res = ev.polygon_api.matrix(p2,m2);
  FaceCollection *coll2 = find_facecoll(env,p2);
  coll2->Prepare();
  }

  virtual int NumFaces() const {
    if (res.id!=-1) {
      FaceCollection *coll = find_facecoll(env,res);
      return coll->NumFaces();
    } else return 0;
  }
  virtual int NumPoints(int face) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->NumPoints(face);
    }
    return 0;
  }
  virtual bool has_normal() const {
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return false;
    return coll->has_normal();
  }
  virtual bool has_attrib() const {
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return false;
    return coll->has_attrib();
  }
  virtual bool has_color() const {
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return false;
    return coll->has_color();
  }
  virtual bool has_texcoord() const {
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return false;
    return coll->has_texcoord();
  }
  virtual bool has_skeleton() const {
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return false;
    return coll->has_skeleton();
  }
  
  
  virtual Point FacePoint(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->FacePoint(face,point);
    } else return Point(0.0,0.0,0.0);
  }
  
  virtual Vector PointNormal(int face, int point) const
  {
    if (res.id!=-1) {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->PointNormal(face,point);
    } else return Vector(0.0,0.0,0.0);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (res.id!=-1) {
   FaceCollection *coll = find_facecoll(env,res);
   return coll->Attrib(face,point,id);
    } else return 0.0;
   }
  virtual int AttribI(int face, int point, int id) const
  {
    if (res.id!=-1) {
   FaceCollection *coll = find_facecoll(env,res);
   return coll->AttribI(face,point,id);
    } else return 0;
   }
  virtual unsigned int Color(int face, int point) const
  {
    if (res.id!=-1) {
   FaceCollection *coll = find_facecoll(env,res);
    return coll->Color(face,point);
    } else return 0xfffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (res.id!=-1) {
   FaceCollection *coll = find_facecoll(env,res);
    return coll->TexCoord(face,point);
    } else { Point2d p; p.x =0.0; p.y=0.0; return p; }
   }
  virtual float TexCoord3(int face, int point) const {
    if (res.id!=-1) {
      FaceCollection *coll = find_facecoll(env,res);
      return coll->TexCoord3(face,point);
    } else return 0.0;
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::P p2;
  GameApi::P res;
};

GameApi::P scale_to_gltf_size_p(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::P p2)
{
  return add_polygon2(e, new ScaleToGLTF_p(e,ev,p,p2),1);
}


class ScaleToGltf : public MainLoopItem
{
public:
  ScaleToGltf(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::ML ml) : env(e), ev(ev), p(p), ml(ml) {res.id=-1; }
  virtual void logoexecute() { }
  void Collect(CollectVisitor &vis)
  {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    MainLoopItem *item = find_main_loop(env,ml);
    item->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    FaceCollection *coll = find_facecoll(env,p);
    std::pair<float,Point> dim = find_mesh_scale(coll);
    
    GameApi::MN I4=ev.move_api.mn_empty();
    GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
    GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
    //Movement *mv = find_move(env,I6);
    //g_last_resize = mv->get_whole_matrix(0.0,0.01);
    GameApi::ML I7=ev.move_api.move_ml(ev,ml,I6,1,10.0);
    res = I7;

  }
  virtual void Prepare() {

    FaceCollection *coll = find_facecoll(env,p);
    coll->Prepare();
    std::pair<float,Point> dim = find_mesh_scale(coll);
    
    GameApi::MN I4=ev.move_api.mn_empty();
    GameApi::MN I5=ev.move_api.trans2(I4,dim.second.x,dim.second.y,dim.second.z);
    GameApi::MN I6=ev.move_api.scale2(I5,dim.first,dim.first,dim.first);
    //Movement *mv = find_move(env,I6);
    //g_last_resize = mv->get_whole_matrix(0.0,0.01);
    GameApi::ML I7=ev.move_api.move_ml(ev,ml,I6,1,10.0);
    res = I7;
    MainLoopItem *item = find_main_loop(env,ml);
    item->Prepare();

  }
  virtual void execute(MainLoopEnv &e)
  {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env, res);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env, res);
      item->handle_event(e);
    }
  }
  
  virtual std::vector<int> shader_id() {
    if (res.id!=-1) {

    MainLoopItem *item = find_main_loop(env, res);
    return item->shader_id();
    } else { return std::vector<int>(); }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::P p;
  GameApi::ML ml;

  GameApi::ML res;
};

GameApi::ML scale_to_gltf_size(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p, GameApi::ML ml)
{
  return add_main_loop(e, new ScaleToGltf(e,ev,p,ml));
}

struct TransformObject
{
  double scale_x, scale_y, scale_z;
  double rot_x, rot_y, rot_z, rot_w;
  double trans_x, trans_y, trans_z;
  Matrix m;
};
void print_transform(TransformObject o)
{
  std::cout << "Scale:" << o.scale_x << " " << o.scale_y << " " << o.scale_z << std::endl;
  std::cout << "Rot:" << o.rot_x << " " << o.rot_y << " " << o.rot_z << " " << o.rot_w << std::endl;
  std::cout << "Trans:" << o.trans_x << " " << o.trans_y << " " << o.trans_z << std::endl;
  std::cout << "Matrix: " << o.m << std::endl;
}

TransformObject gltf_node_default()
{
  TransformObject o;
  o.scale_x = 1.0;
  o.scale_y = 1.0;
  o.scale_z = 1.0;
  o.rot_x = 0.0;
  o.rot_y = 0.0;
  o.rot_z = 0.0;
  o.rot_w = 1.0;
  o.trans_x = 0.0;
  o.trans_y = 0.0;
  o.trans_z = 0.0;
  o.m = Matrix::Identity();
  return o;
}

void slerp(float *prev, float *next, float val, float *res);


TransformObject slerp_transform(TransformObject o, TransformObject o2, float val)
{
  TransformObject res;
  res.trans_x = val*o2.trans_x + (1.0-val)*o.trans_x;
  res.trans_y = val*o2.trans_y + (1.0-val)*o.trans_y;
  res.trans_z = val*o2.trans_z + (1.0-val)*o.trans_z;
  res.scale_x = val*o2.scale_x + (1.0-val)*o.scale_x;
  res.scale_y = val*o2.scale_y + (1.0-val)*o.scale_y;
  res.scale_z = val*o2.scale_z + (1.0-val)*o.scale_z;

  float prev[4];
  float next[4];
  float res2[4];
  prev[0]=o.rot_x;
  prev[1]=o.rot_y;
  prev[2]=o.rot_z;
  prev[3]=o.rot_w;

  next[0]=o2.rot_x;
  next[1]=o2.rot_y;
  next[2]=o2.rot_z;
  next[3]=o2.rot_w;
  slerp(prev,next,val,res2);
  res.rot_x = res2[0];
  res.rot_y = res2[1];
  res.rot_z = res2[2];
  res.rot_w = res2[3];

  for(int i=0;i<16;i++) res.m.matrix[i] = val*o2.m.matrix[i] + (1.0-val)*o.m.matrix[i];
  
  // FIXME (TO BE REMOVED)
  //res.rot_x = val*o2.rot_x + (1.0-val)*o.rot_x;
  //res.rot_y = val*o2.rot_y + (1.0-val)*o.rot_y;
  //res.rot_z = val*o2.rot_z + (1.0-val)*o.rot_z;
  //res.rot_w = val*o2.rot_w + (1.0-val)*o.rot_w;
  return res;
}

TransformObject gltf_node_transform_obj(tinygltf::Node *node)
{
  TransformObject o = gltf_node_default();
  if (int(node->scale.size())==3) {
    double s_x = node->scale[0];
    double s_y = node->scale[1];
    double s_z = node->scale[2];
    o.scale_x = s_x;
    o.scale_y = s_y;
    o.scale_z = s_z;
  }
  if (int(node->rotation.size())==4) {
    double r_x = node->rotation[0];
    double r_y = node->rotation[1];
    double r_z = node->rotation[2];
    double r_w = node->rotation[3];
    o.rot_x = r_x;
    o.rot_y = r_y;
    o.rot_z = r_z;
    o.rot_w = r_w;
  }

  if (int(node->translation.size())==3) {
    double m_x = node->translation[0];
    double m_y = node->translation[1];
    double m_z = node->translation[2];
    o.trans_x = m_x;
    o.trans_y = m_y;
    o.trans_z = m_z;
  }

  if (int(node->matrix.size())==16) {
    double *arr = &node->matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];
      o.m = m;
  }
  return o;
}

Matrix gltf_node_transform_obj_apply(GameApi::Env &e, GameApi::EveryApi &ev, Matrix root, const TransformObject &o)
{
  //GameApi::MN mv = ev.move_api.mn_empty();
  Matrix mv = Matrix::Identity();
  Quarternion q = { float(o.rot_x), float(o.rot_y), float(o.rot_z), float(o.rot_w) };
  Matrix m = Quarternion::QuarToMatrix(q);
  // Scale
  mv = mv * Matrix::Scale(o.scale_x, o.scale_y, o.scale_z);
  mv = mv * m;
  mv = mv * Matrix::Translate(o.trans_x, o.trans_y, o.trans_z);
  mv = mv * o.m;
  mv = mv * root;


  //std::cout << mv << std::endl;
  
  return mv;
}

GameApi::MN gltf_node_transform(GameApi::Env &e, GameApi::EveryApi &ev, tinygltf::Node *node, GameApi::MN root)
{
  GameApi::MN mv = root; //ev.move_api.mn_empty();


  /* TODO, WHY REMOVING THESE TRANSLATIONS BREAK THE MODEL */
  if (int(node->scale.size())==3) {
    double s_x = node->scale[0];
    double s_y = node->scale[1];
    double s_z = node->scale[2];
    mv = ev.move_api.scale2(mv, s_x, s_y, s_z);
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
  if (int(node->translation.size())==3) {
    double m_x = node->translation[0];
    double m_y = node->translation[1];
    double m_z = node->translation[2];
    mv = ev.move_api.trans2(mv, m_x, m_y, m_z);
  }
  
  if (int(node->matrix.size())==16) {
    double *arr = &node->matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];

      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(e, mv2);    
    }
  return mv;
}

GameApi::ML gltf_mesh2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id);

GameApi::ML gltf_mesh2_with_skeleton( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id, int skin_id);
GameApi::MT gltf_anim_material3(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int num_timeindexes, GameApi::MT next, std::string keys);


bool is_child_node(LoadGltf *load, int node_id, int node2)
{
  return true;
}


GameApi::ML gltf_node2( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load,  int node_id)
{
  //if (!load2) load2 = load;
  int s2 = load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  tinygltf::Node *node = &load->model.nodes[node_id];

  // Load mesh
  int ss = load->model.skins.size();
  bool done = false;
  GameApi::ML mesh;
  for(int i=0;i<ss;i++) {
    if (load->model.skins[i].skeleton != -1)
      {
	int mesh_id = node->mesh;
	mesh.id = -1;
	if (mesh_id != -1) {
	  mesh = gltf_mesh2_with_skeleton(e,ev,load, mesh_id, i);
	  done = true;
	}
	if (done)
	  break;
      }
    
  }
  if (!done) {
    int mesh_id = node->mesh;
    mesh.id = -1;
    if (mesh_id!=-1) {
      mesh = gltf_mesh2( e, ev, load, mesh_id );
    }
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
  GameApi::MN mv = ev.move_api.mn_empty();
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
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];

      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
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
    GameApi::P empty = ev.polygon_api.p_empty();
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

GameApi::ML gltf_mesh2_with_skeleton( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id, int skin_id)
{
  //g_last_resize=Matrix::Identity();
  if (mesh_id>=0 && mesh_id<int(load->model.meshes.size())) {
    int s = load->model.meshes[mesh_id].primitives.size();
    std::vector<GameApi::ML> mls;
    for(int i=0;i<s;i++) {
      GameApi::P p = gltf_load2(e, ev, load, mesh_id, i);
      int mat = load->model.meshes[mesh_id].primitives[i].material;
      GameApi::MT mat2 = gltf_material2(e, ev, load, mat, 1.0);
      GameApi::MT mat2_anim = gltf_anim_material3(e,ev, load, skin_id, 300, mat2, "cvbnmfghjklertyuiop");
      GameApi::ML ml = ev.materials_api.bind(p,mat2_anim);
      mls.push_back(ml);
    }
    GameApi::ML ml = ev.mainloop_api.array_ml(ev, mls);
    return ml;
  } else {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
  }
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
      Material *mat0 = find_material(e,mat2);
      GLTF_Material *mat3 = (GLTF_Material*)mat0;
      GameApi::BM bm = mat3->texture(0); // basecolor
      GameApi::MT mat4 = ev.materials_api.transparent_material(ev,bm, mat2);
      GameApi::ML ml = ev.materials_api.bind(p,mat4);
      mls.push_back(ml);
    }
    GameApi::ML ml = ev.mainloop_api.array_ml(ev, mls);
    return ml;
  } else {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
  }
}

class GltfMesh : public MainLoopItem
{
public:
  GltfMesh(GameApi::Env &env, GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_id)
    :env(env), ev(ev), base_url(base_url), url(url),mesh_id(mesh_id) { }


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(env,ev, load, 0,0);

  GameApi::ML ml = gltf_mesh2(env,ev,load, mesh_id);
  res = scale_to_gltf_size(env,ev,mesh,ml);
    
  }
  virtual void execute(MainLoopEnv &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      return item->shader_id();
    else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string base_url;
  std::string url;
  GameApi::ML res;
  int mesh_id;
};

GameApi::ML GameApi::MainLoopApi::gltf_mesh( GameApi::EveryApi &ev, std::string base_url, std::string url, int mesh_id )
{
#if 0
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);

  GameApi::ML ml = gltf_mesh2(e,ev,load, mesh_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
#endif
  return add_main_loop(e, new GltfMesh(e,ev,base_url,url,mesh_id));
}

class GltfNode : public MainLoopItem
{
public:
  GltfNode(GameApi::Env &env, GameApi::EveryApi &ev, std::string base_url, std::string url, int node_id)
    :env(env), ev(ev), base_url(base_url), url(url),node_id(node_id) { }


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(env,ev, load, 0,0);
  GameApi::ML ml = gltf_node2(env,ev,load,node_id);
  res = scale_to_gltf_size(env,ev,mesh,ml);

    
  }
  virtual void execute(MainLoopEnv &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      return item->shader_id();
    else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string base_url;
  std::string url;
  GameApi::ML res;
  int node_id;
};

GameApi::ML GameApi::MainLoopApi::gltf_node( GameApi::EveryApi &ev, std::string base_url, std::string url, int node_id )
{
#if 0
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  GameApi::ML ml = gltf_node2(e,ev,load,node_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
  //return ml;
#endif
  return add_main_loop(e,new GltfNode(e,ev,base_url,url,node_id));
}


class GltfScene : public MainLoopItem
{
public:
  GltfScene(GameApi::Env &env, GameApi::EveryApi &ev, std::string base_url, std::string url, int scene_id)
    :env(env), ev(ev), base_url(base_url), url(url),scene_id(scene_id) { }


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(env,ev, load, 0,0);
  GameApi::ML ml = gltf_scene2(env,ev,load,scene_id);
  res= scale_to_gltf_size(env,ev,mesh,ml);    
  }
  virtual void execute(MainLoopEnv &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      return item->shader_id();
    else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string base_url;
  std::string url;
  GameApi::ML res;
  int scene_id;
};

GameApi::ML GameApi::MainLoopApi::gltf_scene( GameApi::EveryApi &ev, std::string base_url, std::string url, int scene_id )
{
  #if 0
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  GameApi::ML ml = gltf_scene2(e,ev,load,scene_id);
  return scale_to_gltf_size(e,ev,mesh,ml);
#endif
  return add_main_loop(e, new GltfScene(e,ev,base_url,url,scene_id));
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
class GltfMeshAll : public MainLoopItem
{
public:
  GltfMeshAll(GameApi::Env &env, GameApi::EveryApi &ev, std::string base_url, std::string url)
    :env(env), ev(ev), base_url(base_url), url(url) { res.id = -1;}


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {

    bool is_binary=false;
    if (int(url.size())>3) {
      std::string sub = url.substr(url.size()-3);
      if (sub=="glb") is_binary=true;
    }
    LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
    //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
    load->Prepare();
    GameApi::P mesh = gltf_load2(env,ev, load, 0,0);
    
    GameApi::ML ml = gltf_mesh_all2( env, ev, load );
    res = scale_to_gltf_size(env,ev,mesh,ml);

    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env,res);
      if (item)
	item->Prepare();
    }
    
  }
  virtual void execute(MainLoopEnv &e) {
    if (res.id!=-1) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e) {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env,res);
      if (item)
	item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    if (res.id!=-1) {
    MainLoopItem *item = find_main_loop(env,res);
    if (item)
      return item->shader_id();
    else return std::vector<int>();
    } else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string base_url;
  std::string url;
  GameApi::ML res;
};
GameApi::ML GameApi::MainLoopApi::gltf_mesh_all( GameApi::EveryApi &ev, std::string base_url, std::string url )
{
  return add_main_loop(e, new GltfMeshAll(e,ev,base_url,url));
}

class GLTFSkeleton : public LineCollection
{
public:
  GLTFSkeleton(GameApi::Env &env, GameApi::EveryApi &ev, LoadGltf *load, int skin_num) : env(env), ev(ev), load(load), skin_num(skin_num) { }
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    tinygltf::Skin *skin = &load->model.skins[skin_num];
    int start_node = skin->skeleton;
    
    tinygltf::Node *node = &load->model.nodes[start_node];
    recurse_node(node, Matrix::Identity(), Point(-300.0,-300.0,-300.0), Point(300.0,300.0,300.0));
  }
		   
  void Prepare() {
    load->Prepare();
    tinygltf::Skin *skin = &load->model.skins[skin_num];
    int start_node = skin->skeleton;
    
    tinygltf::Node *node = &load->model.nodes[start_node];
    recurse_node(node, Matrix::Identity(), Point(-300.0,-300.0,-300.0), Point(300.0,300.0,300.0));
  }
  Point recurse_node(tinygltf::Node *node, Matrix mat, Point pos, Point pos_next)
  {
    //std::cout << "Node:" << node->name << std::endl;
    GameApi::MN mv0 = ev.move_api.mn_empty();
    GameApi::MN mv = mv0; //ev.move_api.matrix(mv0,add_matrix2(env,mat)); // TODO, why does this cause odd shape for the skeleton LI?

    if (int(node->matrix.size())==16) {
      double *arr = &node->matrix[0];
      Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];
      Movement *orig = find_move(env, mv);
      Movement *mv2 = new MatrixMovement(orig, m);
      mv = add_move(env, mv2);    
    }

    if (int(node->scale.size())==3) {
      double s_x = node->scale[0];
      double s_y = node->scale[1];
      double s_z = node->scale[2];
      mv = ev.move_api.scale2(mv, s_x, s_y, s_z);
      }

    if (int(node->rotation.size())==4) {
      double r_x = node->rotation[0];
      double r_y = node->rotation[1];
      double r_z = node->rotation[2];
      double r_w = node->rotation[3];
      Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
      Matrix m = Quarternion::QuarToMatrix(q);
      Movement *orig = find_move(env, mv);
      Movement *mv2 = new MatrixMovement(orig, m);
      mv = add_move(env, mv2);
      }
    
    if (int(node->translation.size())==3) {
      double m_x = node->translation[0];
      double m_y = node->translation[1];
      double m_z = node->translation[2];
      mv = ev.move_api.trans2(mv, m_x, m_y, m_z);
    }
   

    
    Movement *move = find_move(env,mv);
    Matrix m = move->get_whole_matrix(0.0, 1.0);
    Point pos2 = pos*m;
    Point pos2_next = pos_next*m;
    //if (node->mesh != -1) {
      start_pos.push_back(pos2);
      end_pos.push_back(pos2_next);
      //std::cout << "LINETO: " << pos2 << "->" << pos2_next << std::endl;
	  //}

    // recurse children
    int s = node->children.size();
    std::vector<GameApi::ML> vec;
    for(int i=0;i<s;i++) {
      int child_id = node->children[i];
      if (child_id!=-1) {
	tinygltf::Node *child_node = &load->model.nodes[child_id];

	
	
	/*
	tinygltf::Skin *skin = &load->model.skins[skin_num];
	int s2 = skin->joints.size();
	bool doit = false;
	for(int j=0;j<s2;j++) {
	  if (skin->joints[j]==child_id) { doit=true; break; }
	  }*/
	bool doit = true;
	if (doit) {
	  recurse_node( child_node, m, pos2, pos2_next );
	}
      }
    }
    return pos;
  }

  virtual int NumLines() const { return start_pos.size(); }
  virtual Point LinePoint(int line, int point) const
  {
    int sz = start_pos.size();
    int sz2 = end_pos.size();
    if (line>=0 && line<sz && line<sz2) {
      if (point==0) return start_pos[line];
      if (point==1) return end_pos[line];
    } else return Point(0.0,0.0,0.0);
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; } 
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  LoadGltf *load;
  int skin_num;
  int start_node;
  std::vector<Point> start_pos, end_pos;
};



GameApi::LI GameApi::MainLoopApi::gltf_skeleton(GameApi::EveryApi &ev, std::string base_url, std::string url, int skin_num)
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  LineCollection *coll = new GLTFSkeleton(e,ev, load, skin_num);
  GameApi::P p = gltf_load2(e,ev, load, 0,0);
  GameApi::LI li = add_line_array(e, coll); 
  return scale_to_gltf_size_li(e,ev,p,li);
}

				
GameApi::ARR GameApi::MainLoopApi::gltf_anim_skeleton(GameApi::EveryApi &ev, std::string base_url, std::string url, int skin_num, int animation, int /*channel*/, int num_keyframes)
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  GameApi::P p = gltf_load2(e,ev, load, 0,0);

  std::vector<GameApi::LI> vec;
  int s = num_keyframes;
  ArrayType *array = new ArrayType;
  array->type=0;
  for(int i=0;i<s-1;i++)
    {
      GameApi::LI li = gltf_anim_skeleton2(e,ev,load, skin_num, animation, i);
      GameApi::LI li2 = scale_to_gltf_size_li(e,ev,p,li);
      array->vec.push_back(li2.id);
    }
  return add_array(e, array);
}

class GLTFAnimation : public CollectInterface
{
public:
  GLTFAnimation(LoadGltf *load, int animation, int channel, int time_index) : load(load), animation(animation), channel(channel), time_index(time_index) { }
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    model = &load->model;

    int sz = model->animations.size();
    if (animation<0||animation>=sz) return;
    anim = &model->animations[animation];
    //std::cout << "Animation:" << anim->name << std::endl;
    chan = &anim->channels[channel];
    int target_node = chan->target_node;
    //std::cout << "Target node:" << target_node << std::endl;
    //std::cout << "Target node name:" << &model->nodes[chan->target_node] << std::endl;
    tinygltf::Node *n = &model->nodes[target_node];

    int skin = n->skin;
    tinygltf::Skin *s = &model->skins[skin];
    int inverseBindMatrices = s->inverseBindMatrices;

    int sz2 = model->accessors.size();
    if (inverseBindMatrices!=-1 && inverseBindMatrices>=0 && inverseBindMatrices<sz2) 
      bind_acc = &model->accessors[inverseBindMatrices];
    int sz3 = model->bufferViews.size();
    if (bind_acc && bind_acc->bufferView>=0 && bind_acc->bufferView<sz3)
      bind_buf = &model->bufferViews[bind_acc->bufferView];
    int sz4 = model->buffers.size();
    if (bind_buf && bind_buf->buffer>=0 && bind_buf->buffer<sz4)
      bind = &model->buffers[bind_buf->buffer];
    
    //std::cout << "Name:" << n->name << std::endl;
    sampler = chan->sampler;
    int sz5 = anim->samplers.size();
    if (sampler>=0 && sampler<sz5)
      samp = &anim->samplers[sampler];
    if (samp) {
      //std::cout << "AnimSampler:" << samp->interpolation << std::endl;
      int input_idx = samp->input;
      //std::cout << "input=" << input_idx << std::endl;
      int sz = model->accessors.size();
      if (input_idx>=0 && input_idx<sz)
	input_acc = &model->accessors[input_idx]; // keyframe times
      if (input_acc) {
	//std::cout << "input:" << input_acc->componentType << " " << input_acc->count << " " << input_acc->type << std::endl;
      }

      int output_idx = samp->output;
      //std::cout << "output=" << output_idx << std::endl;
      int sz6 = model->accessors.size();
      if (output_idx>=0 && output_idx<sz6)
	output_acc = &model->accessors[output_idx]; // property value changes
      if (output_acc) {
	//std::cout << "output:" << output_acc->componentType << " " << output_acc->count << " " << output_acc->type << " " << std::endl;
      }

    }
    if (input_acc) {
      input_buf = &model->bufferViews[input_acc->bufferView];
    }
    if (output_acc) {
      output_buf = &model->bufferViews[output_acc->bufferView];
    }
    if (input_buf)
      input = &model->buffers[input_buf->buffer];
    if (output_buf)
      output = &model->buffers[output_buf->buffer];

  }
  void Prepare() {
    load->Prepare();
    model = &load->model;

    int sz = model->animations.size();
    if (animation<0||animation>=sz) return;
    anim = &model->animations[animation];
    //std::cout << "Animation:" << anim->name << std::endl;
    chan = &anim->channels[channel];
    int target_node = chan->target_node;
    //std::cout << "Target node:" << target_node << std::endl;
    //std::cout << "Target node name:" << &model->nodes[chan->target_node] << std::endl;
    tinygltf::Node *n = &model->nodes[target_node];

    int skin = n->skin;
    tinygltf::Skin *s = &model->skins[skin];
    int inverseBindMatrices = s->inverseBindMatrices;
    int sz2 = model->accessors.size();
    if (inverseBindMatrices!=-1 && inverseBindMatrices>=0 && inverseBindMatrices<sz2) 
      bind_acc = &model->accessors[inverseBindMatrices];
    int sz3 = model->bufferViews.size();
    if (bind_acc && bind_acc->bufferView>=0 && bind_acc->bufferView<sz3)
      bind_buf = &model->bufferViews[bind_acc->bufferView];
    int sz4 = model->buffers.size();
    if (bind_buf && bind_buf->buffer>=0 && bind_buf->buffer<sz4)
      bind = &model->buffers[bind_buf->buffer];
    
    //std::cout << "Name:" << n->name << std::endl;
    sampler = chan->sampler;
    int sz5 = anim->samplers.size();
    if (sampler>=0 && sampler<sz5)
      samp = &anim->samplers[sampler];
    if (samp) {
      //std::cout << "AnimSampler:" << samp->interpolation << std::endl;
      int input_idx = samp->input;
      //std::cout << "input=" << input_idx << std::endl;
      int sz = model->accessors.size();
      if (input_idx>=0 && input_idx<sz)
	input_acc = &model->accessors[input_idx]; // keyframe times
      if (input_acc) {
	//std::cout << "input:" << input_acc->componentType << " " << input_acc->count << " " << input_acc->type << std::endl;
      }

      int output_idx = samp->output;
      //std::cout << "output=" << output_idx << std::endl;
      int sz2 = model->accessors.size();
      if (output_idx>=0 && output_idx<sz2)
	output_acc = &model->accessors[output_idx]; // property value changes
      if (output_acc) {
	//std::cout << "output:" << output_acc->componentType << " " << output_acc->count << " " << output_acc->type << " " << std::endl;
      }

    }
    if (input_acc) {
      input_buf = &model->bufferViews[input_acc->bufferView];
    }
    if (output_acc) {
      output_buf = &model->bufferViews[output_acc->bufferView];
    }
    if (input_buf)
      input = &model->buffers[input_buf->buffer];
    if (output_buf)
      output = &model->buffers[output_buf->buffer];
  }

  Matrix InverseBindMatrix(int joint) const
  {
    if (bind && bind_acc && bind_buf) { 
    unsigned char *dt = &bind->data[0];
    int offset = bind_buf->byteOffset;
    // int length = bind_buf->byteLength;
    unsigned char *dt2 = dt + offset;
    
    int offset2 = bind_acc->byteOffset;
    int stride = bind_acc->ByteStride(*bind_buf);
    unsigned char *dt3 = dt2 + offset2 + stride*joint;
    float *dt4 = (float*)dt3;
    Matrix res;
    for(int i=0;i<4;i++)
      for(int j=0;j<4;j++)
	res.matrix[i+j*4] = dt4[j+i*4];
    return res;
    } else return Matrix::Identity();
  }
  
  int target_node() const
  {
    if (!chan) return 0;
    int t = chan->target_node;
    return t;
  }
  
  float start_time() const
  {
    if (input && input_buf && input_acc) {
    
    unsigned char *dt = &input->data[0];
    int offset = input_buf->byteOffset;
    //int length = input_buf->byteLength;
    unsigned char *dt2 = dt + offset;
    int stride = input_buf->byteStride;

    int offset2 = input_acc->byteOffset;
    unsigned char *dt3 = dt2 + offset2;
    int count = input_acc->count;
    assert(input_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    assert(input_acc->type==TINYGLTF_TYPE_SCALAR);
    if (stride==0) { stride=sizeof(float); }
    //assert(stride==0);
    if (time_index>=0 && time_index<count) {
      float *arr = (float*) (dt3 + stride*time_index);
      return *arr;
    } else { return 0.0; }

    } else return 0.0;
    
  }
  float end_time() const
  {
    if (input && input_buf && input_acc) {
    unsigned char *dt = &input->data[0];
    int offset = input_buf->byteOffset;
    //int length = input_buf->byteLength;
    unsigned char *dt2 = dt + offset;
    int stride = input_buf->byteStride;

    int offset2 = input_acc->byteOffset;
    unsigned char *dt3 = dt2 + offset2;
    int count = input_acc->count;
    assert(input_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    //assert(input_acc->type==TINYGLTF_TYPE_SCALAR);
    if (stride==0 && input_acc->type==TINYGLTF_TYPE_SCALAR) { stride=sizeof(float); }
    if (stride==0 && input_acc->type==TINYGLTF_TYPE_VEC2) { stride=2*sizeof(float); }
    if (stride==0 && input_acc->type==TINYGLTF_TYPE_VEC3) { stride=3*sizeof(float); }
    if (stride==0 && input_acc->type==TINYGLTF_TYPE_VEC4) { stride=4*sizeof(float); }
    //assert(stride==0);
    if (time_index+1>=0 && time_index+1<count) {
      float *arr = (float*) (dt3 + stride*(time_index+1));
      return *arr;
    } else { return 0.0; }
    } else return 0.0;
  }

  virtual int Count() const
  {
    if (output_acc) {
      int count = output_acc->count;
      return count;
    } else return 0;
  }
  virtual float *Amount(int i) const {
    static float arr2[4];
    if (output && output_buf && output_acc) {
    unsigned char *dt = &output->data[0];
    int offset = output_buf->byteOffset;
    //int length = output_buf->byteLength;
    unsigned char *dt2 = dt + offset;
    
    int offset2 = output_acc->byteOffset;
    unsigned char *dt3 = dt2 + offset2;
    int stride = output_acc->ByteStride(*output_buf);
    //std::cout << "stride: " << stride << " " << output_acc->type << std::endl;
    if (stride==0 && output_acc->type==TINYGLTF_TYPE_SCALAR) stride = 1*sizeof(float);
    if (stride==0 && output_acc->type==TINYGLTF_TYPE_VEC2) stride = 2*sizeof(float);
    if (stride==0 && output_acc->type==TINYGLTF_TYPE_VEC3) stride = 3*sizeof(float);
    if (stride==0 && output_acc->type==TINYGLTF_TYPE_VEC4) stride = 4*sizeof(float);
    assert(output_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    // TODO: if componentType != float, then it can be converted to float using
    // equations from the gltf 2.0 specification chapter Animations.
    
    //assert(output_acc->type==TINYGLTF_TYPE_VEC3);
    float *arr = (float*) (dt3 + i*stride);
    return arr;
    } else { std::cout << "ERROR: output" << std::endl; return arr2; }
  }
  virtual int Type(int i) const
  {
    if (chan && output_acc) {
      std::string t = chan->target_path;
      if (t=="translation" && output_acc->type==TINYGLTF_TYPE_VEC3) return 0;
      if (t=="rotation" && output_acc->type==TINYGLTF_TYPE_VEC4) return 1;
      if (t=="scale" && output_acc->type==TINYGLTF_TYPE_VEC3) return 2;
      if (t=="weights") return 3;
      std::cout << "target_path: " << t << " ERROR :" << output_acc->type << std::endl;
      return -1;
    } else return -1;
  }
  
private:
  LoadGltf *load;
  tinygltf::Model *model=0;
  int animation;
  tinygltf::Animation *anim=0;
  int channel;
  tinygltf::AnimationChannel *chan=0;
  int sampler;
  tinygltf::AnimationSampler *samp=0;
  int time_index;
  tinygltf::Accessor *input_acc=0;
  tinygltf::Accessor *output_acc=0;
  tinygltf::Accessor *bind_acc=0;
  tinygltf::BufferView *input_buf=0;
  tinygltf::BufferView *output_buf=0;
  tinygltf::BufferView *bind_buf=0;
  tinygltf::Buffer *input=0;
  tinygltf::Buffer *output=0;
  tinygltf::Buffer *bind=0;
};

GameApi::ML gltf_anim3(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int animation, int target_node)
{
  int channel = -1;
  tinygltf::Animation *anim = &load->model.animations[animation];
  int s = anim->channels.size();
  GameApi::ML array; array.id = -1;
  
      tinygltf::Node *node = &load->model.nodes[target_node];
      //std::cout << "\nNode:" << node->name << std::endl;
      int mesh_id = node->mesh;
      GameApi::ML ml = gltf_mesh2(e,ev, load, mesh_id);
      if (channel==-1)
	{
	  std::vector<GameApi::ML> vec;
	  int s2 = node->children.size();
	  //std::vector<GameApi::ML> vec;
	  for(int i=0;i<s2;i++) {
	    int child_id = node->children[i];
	    if (child_id!=-1) {
	      GameApi::ML ml = gltf_anim3( e, ev, load, animation, child_id );
	      vec.push_back(ml);
	    }
	  }
	  
	  vec.push_back(ml);
	  GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);
	  return array;
	}

  for(int j=0;j<s;j++) {
    tinygltf::AnimationChannel *chan = &anim->channels[j];
    if (chan->target_node == target_node) {
      channel = j;

      
      GLTFAnimation *anim3 = new GLTFAnimation(load,animation, channel, 0);
      anim3->Prepare();
      int count = anim3->Count();

  std::vector<GameApi::ML> vec;
      if (j==0) {
  int s2 = node->children.size();
  //std::vector<GameApi::ML> vec;
  for(int i=0;i<s2;i++) {
    int child_id = node->children[i];
    if (child_id!=-1) {
      GameApi::ML ml = gltf_anim3( e, ev, load, animation, child_id );
      vec.push_back(ml);
    }
  }
      
  vec.push_back(ml);
      }
  if (array.id!=-1)
    vec.push_back(array);
  array = ev.mainloop_api.array_ml(ev, vec);
  
  for(int i=0;i<count-1;i++)
    {
      GameApi::MN current = ev.move_api.mn_empty();
      GameApi::MN current2 = ev.move_api.mn_empty();
      //int target_node = anim->target_node();
      //if (target_node!=node_idx) continue;

      GLTFAnimation *anim2 = new GLTFAnimation(load,animation, channel, i);
      anim2->Prepare();
      float start_time = anim2->start_time();
      float end_time = anim2->end_time();

      //std::cout << "start_time: " << start_time << std::endl;
      //std::cout << "end_time: " << end_time << std::endl;
      
      float *a = anim2->Amount(i);
      int type = anim2->Type(i);

      float *b = anim2->Amount(i+1);
      int b_type = anim2->Type(i+1);
      if (type==0) // translate
	{
	  //std::cout << "Trans: " << a[0] << " " << a[1] << " " << a[2] << std::endl;
	  current = ev.move_api.trans2(current, a[0], a[1],a[2]);
	}
      if (type==1) // rotate
	{
	  //std::cout << "Rot: " << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << std::endl;
	  Quarternion q;
	  q.x = a[0];
	  q.y = a[1];
	  q.z = a[2];
	  q.w = a[3];
	  Matrix m = Quarternion::QuarToMatrix(q);
	  GameApi::M m2 = add_matrix2(e, m);
	  current = ev.move_api.matrix(current, m2);
	  //current = ev.move_api.anim_
	}
      if (type==2) // scale
	{
	  //std::cout << "Scale: " << a[0] << " " << a[1] << " " << a[2] << std::endl;
	  current = ev.move_api.scale2(current, a[0],a[1],a[2]);
	}
      if (type==3) // weights
	{
	}



      if (b_type==0) // translate
	{
	  //std::cout << "b_Trans: " << b[0] << " " << b[1] << " " << b[2] << std::endl;
	  current2 = ev.move_api.trans2(current2, b[0], b[1],b[2]);
	}
      if (b_type==1) // rotate
	{
	  //std::cout << "b_Rot: " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << std::endl;
	  Quarternion q;
	  q.x = b[0];
	  q.y = b[1];
	  q.z = b[2];
	  q.w = b[3];
	  Matrix m = Quarternion::QuarToMatrix(q);
	  GameApi::M m2 = add_matrix2(e, m);
	  current2 = ev.move_api.matrix(current2, m2);


	  //Quarternion q;
	  //q.x = a[0];
	  //q.y = a[1];
	  //q.z = a[2];
	  //q.w = a[3];
	  //Matrix m = Quarternion::QuarToMatrix(q);
	  //current = ev.move_api.anim_
	}
      if (b_type==2) // scale
	{
	  //std::cout << "b_Scale: " << b[0] << " " << b[1] << " " << b[2] << std::endl;
	  current2 = ev.move_api.scale2(current2, b[0],b[1],b[2]);
	}
      if (b_type==3) // weights
	{
	}
      GameApi::MN res = ev.move_api.mn_interpolate2(current, current2, start_time, end_time);
      array = ev.move_api.move_ml(ev, array, res, 1, 10.0);
    }


  
    }
  }

  

  return array;
}

void slerp(float *prev, float *next, float val, float *res)
{
  float dot = prev[0]*next[0] + prev[1]*next[1] + prev[2]*next[2];
  if (dot<0.0) {
    next[0] = -next[0];
    next[1] = -next[1];
    next[2] = -next[2];
    next[3] = next[3]; // TODO, does this need to be -?
    dot = -dot;
  }
  if (dot>0.9995) {
    res[0] = prev[0] + val*(next[0]-prev[0]);
    res[1] = prev[1] + val*(next[1]-prev[1]);
    res[2] = prev[2] + val*(next[2]-prev[2]);
    res[3] = prev[3] + val*(next[3]-prev[3]);
    float val = sqrt(res[0]*res[0]+res[1]*res[1]+res[2]*res[2]+res[3]*res[3]);
    res[0]/=val;
    res[1]/=val;
    res[2]/=val;
    res[3]/=val;
    return;
  }
  float theta_0 = acos(dot);
  float theta = val*theta_0;
  float sin_theta = sin(theta);
  float sin_theta_0 = sin(theta_0);

  float scaleprev = cos(theta) - dot*sin_theta/sin_theta_0;
  float scalenext = sin_theta/sin_theta_0;
  res[0] = prev[0]*scaleprev + next[0]*scalenext;
  res[1] = prev[1]*scaleprev + next[1]*scalenext;
  res[2] = prev[2]*scaleprev + next[2]*scalenext;
  res[3] = prev[3]*scaleprev + next[3]*scalenext;
  return;
}

extern Matrix g_last_resize;

class GLTFJointMatrices : public MainLoopItem
{
public:
  GLTFJointMatrices(GameApi::Env &env, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index, MainLoopItem *next, bool has_anim) : env(env), ev(ev), load(load), skin_num(skin_num), animation(animation), time_index(time_index),next(next), has_anim(has_anim) { firsttime=true; max_joints = 63;}
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
    next->Prepare();
      jointmatrices_start.resize(max_joints);
      jointmatrices_end.resize(max_joints);
      bindmatrix.resize(max_joints);
      start_t.resize(max_joints);
      end_t.resize(max_joints);
      root_env.resize(max_joints);
      root_env_2.resize(max_joints);
      int s = max_joints;
      for(int i=0;i<s;i++) {
	//GameApi::MN mn = ev.move_api.mn_empty();
	root_env[i]= Matrix::Identity();
	root_env_2[i]=Matrix::Identity();
	jointmatrices_start[i]=gltf_node_default();
	jointmatrices_end[i]=gltf_node_default();
	bindmatrix[i]=Matrix::Identity();
	start_t[i] = 0.0;
	end_t[i] = 0.0;
      }
      firsttime = false; 
      // std::cout << "GLTFSkeletonAnim::Prepare() start" << std::endl;

      int sz = load->model.skins.size();
    if (skin_num>=0 && skin_num<sz) {
      tinygltf::Skin *skin = &load->model.skins[skin_num];
      int start_node = skin->skeleton;
      recurse_node(start_node, 0, Matrix::Identity(), Matrix::Identity(), 0 /*anim*/, time_index, -1);
     }
    }

  }
  void Prepare()
  {
    if (firsttime) {
    next->Prepare();
      jointmatrices_start.resize(max_joints);
      jointmatrices_end.resize(max_joints);
      bindmatrix.resize(max_joints);
      start_t.resize(max_joints);
      end_t.resize(max_joints);
      root_env.resize(max_joints);
      root_env_2.resize(max_joints);
      int s = max_joints;
      for(int i=0;i<s;i++) {
	//GameApi::MN mn = ev.move_api.mn_empty();
	root_env[i]= Matrix::Identity();
	root_env_2[i]=Matrix::Identity();
	jointmatrices_start[i]=gltf_node_default();
	jointmatrices_end[i]=gltf_node_default();
	bindmatrix[i]=Matrix::Identity();
	start_t[i] = 0.0;
	end_t[i] = 0.0;
      }
      firsttime = false; 
      // std::cout << "GLTFSkeletonAnim::Prepare() start" << std::endl;

    load->Prepare();
    int sz2 = load->model.skins.size();
    if (skin_num>=0 && skin_num<sz2) {
      tinygltf::Skin *skin = &load->model.skins[skin_num];
      int start_node = skin->skeleton;
      recurse_node(start_node, 0, Matrix::Identity(), Matrix::Identity(), 0 /*anim*/, time_index, -1);
     }
    }
  }

  virtual void execute(MainLoopEnv &e)
  {
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    return next->shader_id();
  }

  
  Matrix recurse_node(int node_id, tinygltf::Node * /*node*/, Matrix pos, Matrix pos2, GLTFAnimation * /*anim*/, int time_index, int /*channel*/)
  {
    int sz = load->model.animations.size();
    if (animation<0||animation>=sz) return Matrix::Identity();
    
    tinygltf::Node *node = &load->model.nodes[node_id];
    
    //std::cout << "Node: " << node->name << std::endl;

    TransformObject start_obj = gltf_node_transform_obj(node);
    TransformObject end_obj = start_obj;
    //TransformObject parent = start_obj;
    
    int channel = -1;
    tinygltf::Animation *anim3 = &load->model.animations[animation];
    int s5 = anim3->channels.size();
      int jj = -1;

	tinygltf::Skin *skin = &load->model.skins[skin_num];
	int s2 = skin->joints.size();
	//bool doit = false;
	for(int j=0;j<s2;j++) {
	  if (skin->joints[j]==node_id) { /*doit=true;*/ jj=j; break; }
	  }


	
    for(int i=s5-1;i>=0;i--) {
      tinygltf::AnimationChannel *chan = &anim3->channels[i];
      if (chan->target_node == node_id) {
	channel = i;



	
#if 1
    float *a = 0;
    float *a2 = 0;
    int type = -1;
    if (channel!=-1)
      {
      int ik = time_index;
      GLTFAnimation *anim = new GLTFAnimation(load, animation, channel, ik);
      anim->Prepare();
      if (jj!=-1) {
	start_t[jj] = 5.0*anim->start_time();
	end_t[jj] = 5.0*anim->end_time();
      }
      a = anim->Amount(ik);
      a2 = anim->Amount(ik+1);
      type = anim->Type(time_index);

      if (jj!=-1) {
	Matrix m2 = Matrix::Identity();
	if (anim) m2 = anim->InverseBindMatrix(jj);
	bindmatrix[jj] = m2;
      }

      
      }


    if (has_anim) {
    
      if (type==0 && a && a2) { // translation
	//std::cout << "trans:" << a[0] << "->" << a2[0] << " " << a[1] << "->" << a2[1] << " " << a[2] << "->" << a2[2] << std::endl;
	//mv2 = ev.move_api.trans2(mv2,a[0]*1,a[1]*1,a[2]*1);
	//mv = ev.move_api.trans2(mv, a2[0]*1, a2[1]*1, a2[2]*1);

	start_obj.trans_x =a[0];
	start_obj.trans_y =a[1];
	start_obj.trans_z =a[2];
	end_obj.trans_x =a2[0];
	end_obj.trans_y =a2[1];
	end_obj.trans_z =a2[2];
	
	//pos3+=Vector(a2[0]-a[0],a2[1]-a[1],a2[2]-a[2]);
      } else {

      }

#if 1
      if (type==1 && a && a2) { // rotation
	start_obj.rot_x=a[0];
	start_obj.rot_y=a[1];
	start_obj.rot_z=a[2];
	start_obj.rot_w=a[3];
	end_obj.rot_x=a2[0];
	end_obj.rot_y=a2[1];
	end_obj.rot_z=a2[2];
	end_obj.rot_w=a2[3];
	
#if 0
	Quarternion q,q2;
	q.x = float(a[0]);
	q.y = float(a[1]);
	q.z = float(a[2]);
	q.w = float(a[3]);
	q2.x = float(a2[0]);
	q2.y = float(a2[1]);
	q2.z = float(a2[2]);
	q2.w = float(a2[3]);
	Matrix m = Quarternion::QuarToMatrix(q);
	Matrix m2 = Quarternion::QuarToMatrix(q2);
	GameApi::M m_ = add_matrix2(env,m);
	mv2 = ev.move_api.matrix(mv2,m_);
	GameApi::M m2_ = add_matrix2(env, m2);
	mv = ev.move_api.matrix(mv, m2_);

#endif
	
	//std::cout << "Rot:" << a[0] << "->" << a2[0] << " " << a[1] << "->" << a2[1] << " " << a[2] << "->" << a2[2] << " " << a[3] << "->" << a2[3] << std::endl;
	//std::cout << "IRot:" << res2[0] << " " << res2[1] << " " << res2[2] << " " << res2[3] << std::endl;
      }

#endif
#if 1
      if (type==2 && a && a2) { // scale
	start_obj.scale_x=a[0];
	start_obj.scale_y=a[1];
	start_obj.scale_z=a[2];
	end_obj.scale_x=a2[0];
	end_obj.scale_y=a2[1];
	end_obj.scale_z=a2[2];
	//mv2 = ev.move_api.scale2(mv2,a[0],a[1],a[2]);
	//mv = ev.move_api.scale2(mv, a2[0],a2[1],a2[2]);
	//std::cout << "Sc:" << a[0] << "->" << a2[0] << " " << a[1] << "->" <<a2[1] << " " << a[2] << "->" << a2[2] << " " << std::endl;

      } 
  
      if (type==3) { // weights
      }
#endif

    
#endif
       } // if has_anim 
      } // if chan_target_node
    } // for i=s5
    

#if 0
    
#endif


    //GameApi::MN mv00 = ev.move_api.mn_empty();
    //GameApi::MN mv0 = ev.move_api.matrix(mv00, add_matrix2(env,pos));
    //GameApi::MN mv0_2 = ev.move_api.matrix(mv00, add_matrix2(env,pos2));
    Matrix m = gltf_node_transform_obj_apply(env,ev,pos,start_obj /*parent*/);//gltf
    Matrix m2 = gltf_node_transform_obj_apply(env,ev,pos2,end_obj /*parent*/);//gltf
    // Movement *move = find_move(env,mv);
    //Matrix m = move->get_whole_matrix(0.0, 1.0);
    //Movement *move_2 = find_move(env,mv_2);
    //Matrix m2 = move_2->get_whole_matrix(0.0, 1.0);

    //Point pos2 = pos*m;
    //if (node->mesh != -1) {


    //Point p0;
    //p0.x = -300.0; p0.y = -300.0; p0.z = -300.0;
    //Point p1;
    //p1.x = 300.0; p1.y = 300.0; p1.z = 300.0;
    //start_pos.push_back(p0*m3);
    //end_pos.push_back(p1*m);


    if (jj!=-1) { 
      //std::cout << "Set matrix " << jj << std::endl;
      root_env[jj] = pos;
      root_env_2[jj] = pos2;
      jointmatrices_start[jj] = start_obj;
      jointmatrices_end[jj] = end_obj;
    }
    //}

    // recurse children
    int s = node->children.size();
    std::vector<GameApi::ML> vec;
    for(int i=0;i<s;i++) {
      int child_id = node->children[i];
      if (child_id!=-1) {
	//tinygltf::Node *child_node = &load->model.nodes[child_id];

	
	
	bool doit = true;
	//int jj = 0;
	if (doit) {

	  recurse_node( child_id, 0 /*child_node*/, m,m2, 0 /*anim*/, time_index, channel );
	}
      }
    }
    //std::cout << "recurse end" << std::endl;
    //Movement *move2 = find_move(env,mv2);
    return Matrix::Identity(); //move2->get_whole_matrix(0.0,1.0);
    //return mv;
  }
public:
  std::vector<TransformObject> *start() { return &jointmatrices_start; }
  std::vector<TransformObject> *end() { return &jointmatrices_end; }
  std::vector<Matrix> *root() { return &root_env; }
  std::vector<Matrix> *root2() { return &root_env_2; }
  std::vector<Matrix> *bind() { return &bindmatrix; }
  const std::vector<float> *start_time() const { return &start_t; }
  const std::vector<float> *end_time() const { return &end_t; }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  LoadGltf *load;
  int skin_num;
  int animation;
  int time_index;
  MainLoopItem *next;
  int max_joints;
  std::vector<Matrix> root_env;
  std::vector<Matrix> root_env_2;
  std::vector<TransformObject> jointmatrices_start;
  std::vector<TransformObject> jointmatrices_end;
  std::vector<Matrix> bindmatrix;
  std::vector<float> start_t;
  std::vector<float> end_t;
  bool firsttime;
  bool has_anim;
};

GameApi::ML gltf_joint_matrices2(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index, GameApi::ML next, bool has_anim)
{
  MainLoopItem *item = find_main_loop(e,next);
  return add_main_loop(e, new GLTFJointMatrices(e,ev,load, skin_num, animation, time_index, item,has_anim));
}

GLTFJointMatrices *find_joint_matrices(GameApi::Env &e, GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  GLTFJointMatrices *item2 = (GLTFJointMatrices*)item;
  return item2;
}

class GLTFSkeletonAnim : public LineCollection
{
public:
  GLTFSkeletonAnim(GameApi::Env &env, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index) : env(env), ev(ev), load(load), skin_num(skin_num), animation(animation), time_index(time_index) { firsttime = true; }
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
      firsttime = false; 
    
    tinygltf::Skin *skin = &load->model.skins[skin_num];
    int start_node = skin->skeleton;
    recurse_node(start_node, 0, Matrix::Identity(), 0 /*anim*/, time_index, -1);
    }
    
  }
  void Prepare() {
    if (firsttime) {
      firsttime = false; 
      // std::cout << "GLTFSkeletonAnim::Prepare() start" << std::endl;

    load->Prepare();
    
    tinygltf::Skin *skin = &load->model.skins[skin_num];
    int start_node = skin->skeleton;

    //int channel = -1;
    //tinygltf::Animation *anim3 = &load->model.animations[animation];
    //int s = anim3->channels.size();
    //for(int i=0;i<s;i++) {
    //tinygltf::AnimationChannel *chan = &anim3->channels[i];
    //if (chan->target_node == start_node) {
    //  channel = i;
    //}
    // }
  
    //GLTFAnimation *anim=0;
  //if (channel!=-1) {
  //  anim = new GLTFAnimation(load, animation, channel, 0);
  //  anim->Prepare();
  //}
    



    recurse_node(start_node, 0, Matrix::Identity(), 0 /*anim*/, time_index, -1);
  //std::cout << "GLTFSkeletonAnim::Prepare() end" << std::endl;
    }
  }
  Matrix recurse_node(int node_id, tinygltf::Node * /*node*/, Matrix pos, GLTFAnimation * /*anim*/, int time_index, int /*channel*/)
  {
    tinygltf::Node *node = &load->model.nodes[node_id];
    
    //std::cout << "Node: " << node->name << std::endl;
    GameApi::MN mv00 = ev.move_api.mn_empty();
    GameApi::MN mv0 = ev.move_api.matrix(mv00, add_matrix2(env,pos));
    GameApi::MN mv = gltf_node_transform(env,ev,node,mv0); //ev.move_api.mn_empty();

    GameApi::MN mv2 = mv;



    int channel = -1;
    tinygltf::Animation *anim3 = &load->model.animations[animation];
    int s5 = anim3->channels.size();
    for(int i=s5-1;i>=0;i--) {
      tinygltf::AnimationChannel *chan = &anim3->channels[i];
      if (chan->target_node == node_id) {
	channel = i;

	tinygltf::Skin *skin = &load->model.skins[skin_num];
	int s2 = skin->joints.size();
	//bool doit = false;
	//int jj = -1;
	for(int j=0;j<s2;j++) {
	  if (skin->joints[j]==node_id) { /*doit=true;*/ /*jj=j;*/ break; }
	  }


	
#if 1
    float *a = 0;
    float *a2 = 0;
    int type = -1;
    if (channel!=-1)
      {
      int i = time_index;
      GLTFAnimation *anim = new GLTFAnimation(load, animation, channel, i);
      anim->Prepare();

      a = anim->Amount(i);
      a2 = anim->Amount(i+1);
      type = anim->Type(time_index);

#if 0
      Matrix m2 = Matrix::Identity();
      if (anim) m2 = anim->InverseBindMatrix(jj);
      mv = ev.move_api.matrix(mv, add_matrix2(env,m2));
      
      Matrix m3 = Matrix::Identity();
      if (anim) m3 = anim->InverseBindMatrix(jj);
      mv2 = ev.move_api.matrix(mv2, add_matrix2(env,m3));
#endif
      
      }



    
      if (type==0 && a && a2) { // translation
	//std::cout << "trans:" << a[0] << "->" << a2[0] << " " << a[1] << "->" << a2[1] << " " << a[2] << "->" << a2[2] << std::endl;
	mv2 = ev.move_api.trans2(mv2,a[0]*1,a[1]*1,a[2]*1);
	mv = ev.move_api.trans2(mv, a2[0]*1, a2[1]*1, a2[2]*1);
	//pos3+=Vector(a2[0]-a[0],a2[1]-a[1],a2[2]-a[2]);
      } else {

      }

#if 0
      if (type==1 && a && a2) { // rotation
	Quarternion q,q2;
	q.x = float(a[0]);
	q.y = float(a[1]);
	q.z = float(a[2]);
	q.w = float(a[3]);
	q2.x = float(a2[0]);
	q2.y = float(a2[1]);
	q2.z = float(a2[2]);
	q2.w = float(a2[3]);
	Matrix m = Quarternion::QuarToMatrix(q);
	Matrix m2 = Quarternion::QuarToMatrix(q2);
	GameApi::M m_ = add_matrix2(env,m);
	mv2 = ev.move_api.matrix(mv2,m_);
	GameApi::M m2_ = add_matrix2(env, m2);
	mv = ev.move_api.matrix(mv, m2_);
	//std::cout << "Rot:" << a[0] << "->" << a2[0] << " " << a[1] << "->" << a2[1] << " " << a[2] << "->" << a2[2] << " " << a[3] << "->" << a2[3] << std::endl;
	//std::cout << "IRot:" << res2[0] << " " << res2[1] << " " << res2[2] << " " << res2[3] << std::endl;
      }

#endif
#if 1
      if (type==2 && a && a2) { // scale
	mv2 = ev.move_api.scale2(mv2,a[0],a[1],a[2]);
	mv = ev.move_api.scale2(mv, a2[0],a2[1],a2[2]);
	//std::cout << "Sc:" << a[0] << "->" << a2[0] << " " << a[1] << "->" <<a2[1] << " " << a[2] << "->" << a2[2] << " " << std::endl;

      } 
  
      if (type==3) { // weights
      }
#endif

    
#endif

      }
    }


      Movement *move = find_move(env,mv);
    Matrix m = move->get_whole_matrix(0.0, 1.0);

    Movement *move3 = find_move(env,mv2);
    Matrix m3 = move3->get_whole_matrix(0.0, 1.0);

    //Point pos2 = pos*m;
    //if (node->mesh != -1) {


    Point p0;
    p0.x = -300.0; p0.y = -300.0; p0.z = -300.0;
    Point p1;
    p1.x = 300.0; p1.y = 300.0; p1.z = 300.0;
    start_pos.push_back(p0*m3);
    end_pos.push_back(p1*m);
    //}

    // recurse children
    int s = node->children.size();
    std::vector<GameApi::ML> vec;
    for(int i=0;i<s;i++) {
      int child_id = node->children[i];
      if (child_id!=-1) {
	//tinygltf::Node *child_node = &load->model.nodes[child_id];

	
	
	bool doit = true;
	//int jj = 0;
	if (doit) {

	  recurse_node( child_id, 0 /*child_node*/, m, 0 /*anim*/, time_index, channel );
	}
      }
    }
    //std::cout << "recurse end" << std::endl;
    Movement *move2 = find_move(env,mv2);
    return move2->get_whole_matrix(0.0,1.0);
    //return mv;
  }

  virtual int NumLines() const { return start_pos.size(); }
  virtual Point LinePoint(int line, int point) const
  {
    //       std::cout << "LinePoint start" << std::endl;

    if (line>=0 && line<start_pos.size() && line<end_pos.size()) {
      if (point==0) return start_pos[line];
      if (point==1) return end_pos[line];
    } else return Point(0.0,0.0,0.0);
    return Point(0.0,0.0,0.0);
  }
  virtual unsigned int LineColor(int line, int point) const { return 0xffffffff; } 
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  LoadGltf *load;
  int skin_num;
  int animation;
  //int channel;
  int time_index;
  int start_node;
  std::vector<Point> start_pos, end_pos;
  bool firsttime;
};

GameApi::LI gltf_anim_skeleton2(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index)
{
  GLTFSkeletonAnim *anim =  new GLTFSkeletonAnim(e, ev, load, skin_num, animation, time_index);
  return add_line_array(e, anim);
}


GameApi::ML gltf_scene3( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int scene_id, int animation )
{
  int s2 = load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  tinygltf::Scene *scene = &load->model.scenes[scene_id];
  int s = scene->nodes.size();
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
    //GameApi::ML ml = gltf_node2( e, ev, load, scene->nodes[i] );
    GameApi::ML ml = gltf_anim3(e,ev,load,animation, scene->nodes[i]);
    vec.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, vec);
}



GameApi::ML GameApi::MainLoopApi::gltf_scene_anim( GameApi::EveryApi &ev, std::string base_url, std::string url, int scene_id, int animation )
{

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  GameApi::ML ml = gltf_scene3(e,ev,load,scene_id,animation);
  return scale_to_gltf_size(e,ev,mesh,ml);

}

GameApi::ML GameApi::MainLoopApi::gltf_anim4( GameApi::EveryApi &ev, std::string base_url, std::string url, int animation, int channel )
{

  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  load->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, load, 0,0);
  std::vector<GameApi::ML> mls;
    tinygltf::Model *model = &load->model;
    tinygltf::Animation *anim2 = &model->animations[animation];

    
    GLTFAnimation *anim = new GLTFAnimation(load,animation, channel, 0);
      anim->Prepare();
      
      int target_node = anim->target_node();
      GameApi::ML ml = gltf_anim3(e,ev,load,animation, target_node);
    return scale_to_gltf_size(e,ev,mesh,ml);
}




class GLTF_Att : public Attach
{
public:
  GLTF_Att( LoadGltf *load, int mesh_index, int prim_index, int num ) : load(load), mesh_index(mesh_index), prim_index(prim_index), num(num) { }
  void Collect(CollectVisitor &vis)
  {
    load->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (mesh_index>=0 && mesh_index<int(load->model.meshes.size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else
      return;
    
    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
    model = &load->model;
    
    mode = prim->mode;

    indices_index = prim->indices;
    indices_acc = 0;
    if (indices_index!=-1)
      indices_acc = &model->accessors[indices_index];

    assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);


        indices_bv = 0;
    if (indices_acc) {
      int view = indices_acc->bufferView;
      if (view!=-1)
	indices_bv = &model->bufferViews[view];
    }

    indices_buf = 0;
    if (indices_bv) {
      int buf = indices_bv->buffer;
      if (buf!=-1) {
	indices_buf = &model->buffers[buf];
      }
    }

    
    
    joints_index = -1;
    if (prim->attributes.find("JOINTS_0") != prim->attributes.end())
      joints_index = prim->attributes["JOINTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    joints_acc = 0;
    if (joints_index!=-1)
      joints_acc = &model->accessors[joints_index];
    
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    joints_bv = 0;
    if (joints_acc) {
      int view = joints_acc->bufferView;
      if (view!=-1)
	joints_bv = &model->bufferViews[view];
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    joints_buf = 0;
    if (joints_bv) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &model->buffers[buf];
      }
    }





    weights_index = -1;
    if (prim->attributes.find("WEIGHTS_0") != prim->attributes.end())
      weights_index = prim->attributes["WEIGHTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    weights_acc = 0;
    if (weights_index!=-1)
      weights_acc = &model->accessors[weights_index];
    
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    weights_bv = 0;
    if (weights_acc) {
      int view = weights_acc->bufferView;
      if (view!=-1)
	weights_bv = &model->bufferViews[view];
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    weights_buf = 0;
    if (weights_bv) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &model->buffers[buf];
      }
    }


    
    //std::cout << "joints_buf: " << joints_buf << std::endl;
  }
  void Prepare()
  {
    
    load->Prepare();
    
    
    if (mesh_index>=0 && mesh_index<int(load->model.meshes.size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
      prim = &load->model.meshes[mesh_index].primitives[prim_index];
    else
      return;
    
    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
    model = &load->model;
    
    mode = prim->mode;

    indices_index = prim->indices;
    indices_acc = 0;
    if (indices_index!=-1)
      indices_acc = &model->accessors[indices_index];

    assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);


        indices_bv = 0;
    if (indices_acc) {
      int view = indices_acc->bufferView;
      if (view!=-1)
	indices_bv = &model->bufferViews[view];
    }

    indices_buf = 0;
    if (indices_bv) {
      int buf = indices_bv->buffer;
      if (buf!=-1) {
	indices_buf = &model->buffers[buf];
      }
    }

    
    
    joints_index = -1;
    if (prim->attributes.find("JOINTS_0") != prim->attributes.end())
      joints_index = prim->attributes["JOINTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    joints_acc = 0;
    if (joints_index!=-1)
      joints_acc = &model->accessors[joints_index];
    
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    joints_bv = 0;
    if (joints_acc) {
      int view = joints_acc->bufferView;
      if (view!=-1)
	joints_bv = &model->bufferViews[view];
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    joints_buf = 0;
    if (joints_bv) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &model->buffers[buf];
      }
    }





    weights_index = -1;
    if (prim->attributes.find("WEIGHTS_0") != prim->attributes.end())
      weights_index = prim->attributes["WEIGHTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    weights_acc = 0;
    if (weights_index!=-1)
      weights_acc = &model->accessors[weights_index];
    
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    weights_bv = 0;
    if (weights_acc) {
      int view = weights_acc->bufferView;
      if (view!=-1)
	weights_bv = &model->bufferViews[view];
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    weights_buf = 0;
    if (weights_bv) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &model->buffers[buf];
      }
    }


    
    //std::cout << "joints_buf: " << joints_buf << std::endl;
	
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

  virtual int Weights(int face, int point, int num) const
  {
    if (weights_bv && weights_acc && weights_buf) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &weights_buf->data[0];
	int stride2 = weights_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + weights_bv->byteOffset + index*stride2 + weights_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << std::hex << int(pos_ptr2[0]) << "," << int(pos_ptr2[1]) << "," << int(pos_ptr2[2]) << "," << int(pos_ptr2[3]) << std::endl;
	//vec4 res;
	//res[0] = pos_ptr2[0];
	//res[1] = pos_ptr2[1];
	//res[2] = pos_ptr2[2];
	//res[3] = pos_ptr2[3];
	//std::cout << "Attached end" << std::endl;
	return pos_ptr2[num];
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &weights_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + weights_bv->byteOffset;
	int stride = weights_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + weights_acc->byteOffset + comp*stride;
	float *pos_ptr4 = (float*)pos_ptr3; // 3 = num of components in (x,y,z)
	//std::cout << face << " " << point << "::" << int(pos_ptr4[0]) << std::endl;
	//res[0] = pos_ptr4[0];
	//res[1] = pos_ptr4[1];
	//res[2] = pos_ptr4[2];
	//res[3] = pos_ptr4[3];
	//return res;
	//std::cout << "Attached end" << std::endl;
	return pos_ptr4[num];
      }
      }
    } else {
      //std::cout << "Attached end" << std::endl;
      return 0;
    }
    std::cout << "gltf weights unknown mode: "<< mode << std::endl;
    return 0;


  }
  
  virtual int Attached(int face, int point) const
  {
    int s = 4;
    int chosen = -1;
    for(int i=0;i<s;i++) {
      //std::cout << "Weights:" << face << " " << point << ": "<< i << " " << Weights(face,point,i) << std::endl;
      if (Weights(face,point,i)>0.5) chosen=i;
    }

    //if (fabs(Weights(face,point,chosen)-1.0)>0.01) std::cout << "Weights:" << Weights(face,point,chosen) << std::endl;
    
    int num = chosen;

    if (num==-1) return -1;
    
    //std::cout << "Attached start" << std::endl;
    if (joints_bv && joints_acc && joints_buf) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf && indices_bv && indices_acc) {
      
	int index = get_index(face,point);

	unsigned char *pos_ptr = &joints_buf->data[0];
	int stride2 = joints_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	unsigned char *pos_ptr2 = (unsigned char*)(pos_ptr + joints_bv->byteOffset + index*stride2 + joints_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << std::hex << int(pos_ptr2[0]) << "," << int(pos_ptr2[1]) << "," << int(pos_ptr2[2]) << "," << int(pos_ptr2[3]) << std::endl;
	//vec4 res;
	//res[0] = pos_ptr2[0];
	//res[1] = pos_ptr2[1];
	//res[2] = pos_ptr2[2];
	//res[3] = pos_ptr2[3];
	//std::cout << "Attached end" << std::endl;
	return int(((unsigned int)(pos_ptr2[num]))&0xff);
      } else {
	// todo, check that this branch works
	unsigned char *pos_ptr = &joints_buf->data[0];
	unsigned char *pos_ptr2 = pos_ptr + joints_bv->byteOffset;
	int stride = joints_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	unsigned char *pos_ptr3 = pos_ptr2 + joints_acc->byteOffset + comp*stride;
	unsigned char *pos_ptr4 = (unsigned char*)pos_ptr3; // 3 = num of components in (x,y,z)
	//std::cout << face << " " << point << "::" << int(pos_ptr4[0]) << std::endl;
	//res[0] = pos_ptr4[0];
	//res[1] = pos_ptr4[1];
	//res[2] = pos_ptr4[2];
	//res[3] = pos_ptr4[3];
	//return res;
	//std::cout << "Attached end" << std::endl;
	return int(((unsigned int)(pos_ptr2[num]))&0xff);
      }
      }
    } else {
      //std::cout << "Attached end" << std::endl;
      return 0;
    }
    std::cout << "gltf attach unknown mode: "<< mode << std::endl;
    return 0;
  }

private:
  tinygltf::Primitive *prim = 0;
  tinygltf::Model *model =0 ;

  int mode;

  int num = 0;
  
  int indices_index;
  int joints_index;
  int weights_index;
  tinygltf::Accessor *indices_acc=0;
  tinygltf::Accessor *joints_acc=0;
  tinygltf::Accessor *weights_acc=0;
  
  tinygltf::BufferView *indices_bv=0;
  tinygltf::BufferView *joints_bv=0;
  tinygltf::BufferView *weights_bv=0;
  
  tinygltf::Buffer *indices_buf=0;
  tinygltf::Buffer *joints_buf=0;
  tinygltf::Buffer *weights_buf=0;
  
  LoadGltf *load=0;
  int mesh_index;
  int prim_index;
  
};

GameApi::ATT gltf_attach(GameApi::Env &e, LoadGltf *load, int mesh_index, int prim_index, int num)
{
  return add_attach(e, new GLTF_Att(load,mesh_index, prim_index, num));
}

GameApi::ARR gltf_split_faces(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_index, int prim_index, int max_attach, int num)
{
  GameApi::P p = gltf_load2(e,ev, load, mesh_index, prim_index);
  GameApi::P p2 = scale_to_gltf_size_p(e,ev, p, p);
  GameApi::ATT att = gltf_attach(e,load, mesh_index, prim_index, num);
  return ev.polygon_api.split_faces(p2, att, max_attach);
}

GameApi::ARR GameApi::PolygonApi::gltf_split_faces2(EveryApi &ev, std::string base_url, std::string url, int mesh_index, int prim_index, int max_attach)
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  LoadGltf *load = find_gltf_instance(e,base_url, url, gameapi_homepageurl, is_binary);
  return gltf_split_faces(e, ev,load,mesh_index, prim_index, max_attach, 0); // 0=index of joint to choose from 4 alternatives 0|1|2|3.
}



class GLTF_Animation_Material : public MaterialForward
{
public:
  GLTF_Animation_Material(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int num_timeindexes, Material *next, int key) : e(e), ev(ev), load(load), skin_num(skin_num), animation(animation), num_timeindexes(num_timeindexes), next(next), key(key) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    load->Prepare();
    GameApi::ML ml;
    ml.id = next->mat(p.id);
    std::vector<GameApi::ML> mls;
    GameApi::ML ml_orig = gltf_joint_matrices2(e,ev,load,skin_num,animation,0,ml,false);
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML ml1 = gltf_joint_matrices2(e,ev, load, skin_num, animation, i, ml, true);
      mls.push_back(ml1);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key);
    return ml2;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    load->Prepare();
    GameApi::ML ml;
    ml.id = next->mat_inst(p.id, pts.id);
    std::vector<GameApi::ML> mls;
    GameApi::ML ml_orig = gltf_joint_matrices2(e,ev,load,skin_num,animation,0,ml,false);
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML ml1 = gltf_joint_matrices2(e,ev, load, skin_num, animation, i, ml, true);
      mls.push_back(ml1);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig, mls,key);
    return ml2;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    load->Prepare();
    GameApi::ML ml;
    ml.id = next->mat_inst_matrix(p.id, ms.id);
    std::vector<GameApi::ML> mls;
    GameApi::ML ml_orig = gltf_joint_matrices2(e,ev,load,skin_num,animation,0,ml,false);
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML ml1 = gltf_joint_matrices2(e,ev, load, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key);
    return ml2;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    load->Prepare();
    GameApi::ML ml;
    ml.id = next->mat_inst2(p.id, pta.id);
    std::vector<GameApi::ML> mls;
    GameApi::ML ml_orig = gltf_joint_matrices2(e,ev,load,skin_num,animation,0,ml,false);
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML ml1 = gltf_joint_matrices2(e,ev, load, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key);
    return ml2;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    load->Prepare();
    GameApi::ML ml;
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    GameApi::ML ml_orig = gltf_joint_matrices2(e,ev,load,skin_num,animation,0,ml,false);
    std::vector<GameApi::ML> mls;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML ml1 = gltf_joint_matrices2(e,ev, load, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key);
    return ml2;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Material *next;
  LoadGltf *load;
  int skin_num;
  int animation;
  int num_timeindexes;
  int key;
};


class KeysMaterial : public MaterialForward
{
public:
  KeysMaterial(GameApi::EveryApi &ev, std::vector<Material*> vec, std::string keys) : ev(ev), vec(vec), keys(keys) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat(p.id);
      vec2.push_back(ml);
    }
    return ev.mainloop_api.key_ml(vec2, keys);
  }
  
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst(p.id, pts.id);
      vec2.push_back(ml);
    }
    return ev.mainloop_api.key_ml(vec2, keys);
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst_matrix(p.id, ms.id);
      vec2.push_back(ml);
    }
    return ev.mainloop_api.key_ml(vec2, keys);
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst2(p.id, pta.id);
      vec2.push_back(ml);
    }
    return ev.mainloop_api.key_ml(vec2, keys);
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
      vec2.push_back(ml);
    }
    return ev.mainloop_api.key_ml(vec2, keys);
  }
  
private:
  GameApi::EveryApi &ev;
  std::vector<Material*> vec;
  std::string keys;
};

GameApi::MT GameApi::MaterialsApi::m_keys(EveryApi &ev, std::vector<MT> vec, std::string keys)
{
  int s = vec.size();
  std::vector<Material*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_material(e,vec[i]));
    }
  return add_material(e,new KeysMaterial(ev, vec2, keys));
}


GameApi::MT gltf_anim_material4(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int num_timeindexes, GameApi::MT next, int key)
{
  Material *next_mat = find_material(e,next);
  Material *mat = new GLTF_Animation_Material(e,ev,load, skin_num, animation, num_timeindexes, next_mat, key);
  return add_material(e, mat);
}


GameApi::MT gltf_anim_material3(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int num_timeindexes, GameApi::MT next, std::string keys)
{
  int s = keys.size();
  std::vector<GameApi::MT> vec;
  for(int i=0;i<s;i++) {
    GameApi::MT mt = gltf_anim_material4(e,ev,load, skin_num, i, num_timeindexes, next, keys[i]);
    vec.push_back(mt);
  }
  return ev.materials_api.m_keys(ev,vec,keys);

}


GameApi::MT GameApi::MaterialsApi::gltf_anim_material2(GameApi::EveryApi &ev, std::string base_url, std::string url, int skin_num, int num_timeindexes, MT next, std::string keys)
{
  int s = keys.size();
  std::vector<GameApi::MT> vec;
  for(int i=0;i<s;i++) {
    GameApi::MT mt = gltf_anim_material(ev,base_url,url, skin_num, i, num_timeindexes, next, keys[i]);
    vec.push_back(mt);
  }
  return m_keys(ev,vec,keys);
}


GameApi::MT GameApi::MaterialsApi::gltf_anim_material(GameApi::EveryApi &ev, std::string base_url, std::string url, int skin_num, int animation, int num_timeindexes, MT next, int key)
{
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  Material *next_mat = find_material(e,next);
  Material *mat = new GLTF_Animation_Material(e,ev,load, skin_num, animation, num_timeindexes, next_mat, key);
  return add_material(e, mat);
}
extern Matrix g_last_resize;

char key_mapping(char ch, int type);

class GltfAnimShaderML : public MainLoopItem
{
public:
  GltfAnimShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *ml_orig, std::vector<MainLoopItem*> items, int key) : env(env), ev(ev), ml_orig(ml_orig),items(items), key(key) { firsttime=true; resize=Matrix::Identity(); }
  std::vector<int> shader_id() {
    return ml_orig->shader_id();
  }
  void handle_event(MainLoopEvent &e)
  {
    int ch = key_mapping(e.ch,e.type);
    
    if (ch==key &&e.type==0x300) {
      key_time = ev.mainloop_api.get_time()/1000.0; /*current_time;*/
    }
    
    items[0]->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    int s = items.size();
    for(int i=0;i<s;i++) {
      items[i]->Collect(vis);
    }
    ml_orig->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    resize = g_last_resize; // this communicates with resize_to_correct_size()
  }
  void Prepare() {
    int s = items.size();
    for(int i=0;i<s;i++) {
      items[i]->Prepare();
    }
    ml_orig->Prepare();
    resize = g_last_resize; // this communicates with resize_to_correct_size()
  }
  void logoexecute() {
    items[0]->logoexecute();
  }
  void execute(MainLoopEnv &e) {
    current_time = e.time;
    float time = e.time-key_time;

    MainLoopEnv ee = e;
    if (firsttime)
      {

#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_gltf_anim(vertex);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_gltf_anim(fragment);
    ee.us_fragment_shader = fragment.id;
#endif	
      }
    if (1)
      {
     std::vector<int> sh_ids = ml_orig->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    if (sh_id!=-1)
      {
	GLTFJointMatrices *joints_0 = (GLTFJointMatrices*)ml_orig;
	std::vector<TransformObject> *start_0 = joints_0->start();
	std::vector<Matrix> *r_0 = joints_0->root();
	std::vector<Matrix> *bind = joints_0->bind();	

	ev.shader_api.use(sh);
	std::vector<GameApi::M> vec;
	int sz = 1;
	for(int ii=0;ii<64;ii++)
	  {
	    current = -1;
	    const std::vector<float> *current_start_time=0, *current_end_time=0;
	    int sz = items.size();
	    for(int t=0;t<sz;t++)
	      {
		GLTFJointMatrices *joints = (GLTFJointMatrices*)items[t];
		const std::vector<float> *start_time = joints->start_time();
		const std::vector<float> *end_time = joints->end_time();
		
		int ssz = std::min(start_time->size(),end_time->size());
		if (ii<ssz)
		  if (time>=start_time->operator[](ii) && time<end_time->operator[](ii))
		    {
		      current=t;
		      current_start_time = start_time;
		      current_end_time = end_time;
		      break;
		    }
	      }

	    if (current==-1) { vec.push_back(add_matrix2(env,Matrix::Identity())); continue; }
	    
	    MainLoopItem *next = items[current];
	    GLTFJointMatrices *joints = (GLTFJointMatrices*)next;
	    std::vector<TransformObject> *start = joints->start();
	    std::vector<TransformObject> *end = joints->end();
	    std::vector<Matrix> *r = joints->root();
	    std::vector<Matrix> *r_2 = joints->root2();
	    sz = std::min(start_0->size(),std::min(start->size(),end->size()));

	    float time01 = (time-current_start_time->operator[](ii))/(current_end_time->operator[](ii)-current_start_time->operator[](ii));

	    if (std::isinf(time01)) time01 = 0.0;

	    TransformObject start_obj;
	    if (ii<sz)
	      start_obj= start->operator[](ii);
	    else start_obj = gltf_node_default();
	    TransformObject end_obj;
	    if (ii<sz)
	      end_obj = end->operator[](ii);
	    else end_obj = gltf_node_default();
	    
	    Matrix rr0;
	    if (ii<sz)
	      rr0 = r_0->operator[](ii);
	    else rr0=Matrix::Identity();


	    Matrix rr;
	    if (ii<sz)
	      rr = r->operator[](ii);
	    else rr=Matrix::Identity();

	    Matrix rr2;
	    if (ii<sz)
	      rr2 = r_2->operator[](ii);
	    else rr2=Matrix::Identity();

	    
	    
	    TransformObject m0t;
	    if (ii<sz)
	      m0t = start_0->operator[](ii);
	    else m0t = gltf_node_default();
	    
	    Matrix m0 = gltf_node_transform_obj_apply(env,ev,rr0,m0t);	    
	    Matrix m0i = Matrix::Inverse(m0);
	    	    
	    Matrix bindm;
	    if (i<sz)
	      bindm=bind->operator[](ii);
	    else bindm = Matrix::Identity();

	    TransformObject obj = slerp_transform(start_obj,end_obj,time01);

	    Matrix mr;
	    for(int j=0;j<16;j++) mr.matrix[j]=(time01)*rr2.matrix[j] + (1.0-time01)*rr.matrix[j];
	    
	    
	    Matrix mv = gltf_node_transform_obj_apply(env,ev,mr,obj);
	    Matrix m = mv;  
	    Matrix ri = Matrix::Inverse(resize);
	    vec.push_back(add_matrix2(env,ri *m0i*m*bindm *resize));
	  }
	ev.shader_api.set_var(sh, "jointMatrix", vec, 64);

      }
#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV);
	GameApi::M m1 = add_matrix2(env, e.in_T); 
	GameApi::M m3 = add_matrix2(env, e.in_P); 
	GameApi::M m2 = add_matrix2(env, e.in_N); 
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
    ml_orig->execute(ee);

      } else {
      // THIS IS ONLY FOR SITUATION WHEN MODEL DOESNT EXIST.
      MainLoopItem *next = items[0];
      std::vector<GameApi::M> mat;
      for(int i=0;i<64;i++)
	mat.push_back(add_matrix2(env,Matrix::Identity()));

      std::vector<int> sh_ids = next->shader_id();
      int s=sh_ids.size();
      for(int i=0;i<s;i++) {
	int sh_id = sh_ids[i];
	sh.id = sh_id;
	ev.shader_api.use(sh);
	
	ev.shader_api.set_var(sh, "jointMatrix", mat,64);
      }
    ml_orig->execute(ee);
    }
    if (firsttime) 	firsttime = false;
	
    ev.shader_api.unuse(sh);
    
    
  }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<MainLoopItem*> items;
  MainLoopItem *ml_orig;
  int key;
  int current = -1;
  float key_time = -999990.0;
  float current_time=0.0;
  bool firsttime;
  GameApi::SH sh;
  Matrix resize;
};
EXPORT GameApi::ML GameApi::PolygonApi::gltf_anim_shader(GameApi::EveryApi &ev, ML ml_orig, std::vector<GameApi::ML> mls, int key)
{
  int s = mls.size();
  std::vector<MainLoopItem*> items;
  for(int i=0;i<s;i++) {
    MainLoopItem *item = find_main_loop(e,mls[i]);
    items.push_back(item);
  }
  MainLoopItem *orig = find_main_loop(e,ml_orig);
  return add_main_loop(e, new GltfAnimShaderML(e,ev,orig,items, key));
}

void ASyncGltfCB(void *data);

extern std::vector<std::string> g_registered_urls;

class ASyncGltf : public MainLoopItem
{
public:
  ASyncGltf(GameApi::Env &env, MainLoopItem *next, std::string base_url, std::string url, std::string homepage) : env(env) ,next(next), base_url(base_url), url(url), homepage(homepage) {
    done = false;
    env.async_load_callback(url, &ASyncGltfCB, this);
  }
  void Prepare2()
  {
    //std::cout << "AsyncGltf::PREPARE2" << std::endl;
    if (!done) {
      done = true;
    
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      std::vector<unsigned char> *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "ASyncGltf::async not ready!" << std::endl; done=false; return; }
    std::string ss(vec->begin(),vec->end());
    std::stringstream s(ss);
    std::string line;
    while(std::getline(s,line)) {
      std::string uri;
      std::stringstream ss2(line);
      ss2>> uri;
      //std::cout << "\'" << uri << "\'" << std::endl;
      if (uri=="\"uri\":") {
	std::string url2;
	ss2 >> url2;
	if (base_url[base_url.size()-1]=='/')
	  url2 = base_url + url2.substr(1,url2.size()-2);
	else
	  url2 = base_url + "/" + url2.substr(1,url2.size()-2);
	//std::cout << "URL:" << url2 << std::endl;
	g_registered_urls.push_back(url2);
	env.async_load_url(url2,homepage);
      }
    }
    }
  }

  virtual void Collect(CollectVisitor &vis) { Prepare2(); next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { Prepare2(); next->Prepare(); }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() {
    return next->shader_id();
  }

  
private:
  GameApi::Env &env;
  MainLoopItem *next;
  std::string url;
  std::string base_url;
  std::string homepage;
  bool done;
};

GameApi::ML GameApi::MainLoopApi::async_gltf(ML ml, std::string base_url, std::string url)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new ASyncGltf(e,next, base_url, url, gameapi_homepageurl));
}

void ASyncGltfCB(void *data)
{
  ASyncGltf *ptr = (ASyncGltf*)data;
  ptr->Prepare2();
}
