
int hhhh_gggg=1;

#include "GameApi_h.hh"
#include "GraphI.hh"
#include "VectorTools.hh"
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_ENABLE_DRACO 1
#define TINYGLTF_USE_CPP14 1
#include "tiny_gltf.h"
#include "Tasks.hh"

extern unsigned long g_glb_file_size;
extern unsigned long g_zip_file_size;
extern bool g_glb_animated;
extern int g_pthread_count;

template<class T>
void print(std::string label, T *ptr)
{
  int s = 4;
  std::cout << label << ":";
  for(int i=0;i<s;i++) std::cout << ptr[i];
  std::cout << std::endl;
}

class GLTF_Model_with_prepare;
class GLTF_Model_with_prepare_from_string;
std::vector<GLTF_Model_with_prepare*> g_model_del_items;
std::vector<GLTF_Model_with_prepare_from_string*> g_model_del_items_from_string;
std::vector<std::string> g_deleted_urls;

class LoadGltf;
class LoadGltf_from_string;
// not working because tinygltf doesn't allow it.
//#define CONCURRENT_IMAGE_DECODE 1

extern int async_pending_count;
class ThreadInfo_gltf_bitmap;


struct FETCHID
{
  int id;
  friend bool operator<(const FETCHID &id, const FETCHID &id2) { return id.id<id2.id; }
};
struct FILEID
{
  int id;
  friend bool operator<(const FILEID &id, const FILEID &id2) { return id.id<id2.id; }
};
struct IMAGEID
{
  int id;
  friend bool operator<(const IMAGEID &id, const IMAGEID &id2) { return id.id<id2.id; }
};

class GLTFImageDecoder;

class GLTFImageDecoder
{
public:
  GLTFImageDecoder(std::string url_prefix, LoadGltf *load) : url_prefix(url_prefix),load(load) { }
  GLTFImageDecoder(std::string url_prefix, LoadGltf_from_string *load) : url_prefix(url_prefix), load2(load) { }
  ~GLTFImageDecoder();
  std::vector<std::string> scan_gltf_file(std::vector<unsigned char, GameApiAllocator<unsigned char> > &vec);
  FETCHID fetch_file(GameApi::Env &e, std::string filename);
  std::string get_fetch_filename(FETCHID id);
  std::vector<FETCHID> fetch_ids(const std::vector<std::string> &filenames);
  void fetch_all_files(GameApi::Env &e, const std::vector<FETCHID> &ids);
  void set_fetch_callback(GameApi::Env &e, FETCHID id, void (*fptr)(void*), void *user_data);
  std::vector<unsigned char,GameApiAllocator<unsigned char> > *get_file(GameApi::Env &e, FETCHID id);
  FILEID add_file(std::vector<unsigned char, GameApiAllocator<unsigned char> > *vec, std::string filename);
  FILEID find_file(std::string filename);
  void start_decode_process(FETCHID fetch_id, FILEID id, int req_width, int req_height);
  //void decode_file(FILEID id);
  bool is_decoded(FILEID id);
  void set_decode_callback(FILEID id, void (*fptr)(void*), void *user_data);
  std::vector<unsigned char,GameApiAllocator<unsigned char> > *get_decoded_file(FILEID id);
  IMAGEID convert_to_image(std::vector<unsigned char> &vec);
  Bitmap<Color> *get_converted_image(IMAGEID id);
public:
  friend void *thread_func_gltf_bitmap(void *);
  LoadGltf *load;
  LoadGltf_from_string *load2;
  std::string url_prefix;
  std::map<FETCHID,std::string> filenames;
  std::map<FETCHID,std::vector<unsigned char, GameApiAllocator<unsigned char> > *> files;
  std::map<FETCHID,void (*)(void*)> fetch_cb;
  std::map<FILEID, std::string> filenames2;
  std::map<FILEID, std::vector<unsigned char, GameApiAllocator<unsigned char> > *> files2;
  std::map<FILEID, std::vector<unsigned char, GameApiAllocator<unsigned char> > *> decoded_files;
  std::map<FILEID, tinygltf::Image*> decoded_image;
  std::map<FILEID, void (*)(void*)> decode_cb;
  std::map<FILEID, void*> decode_user_data;
  std::map<IMAGEID,std::vector<unsigned char, GameApiAllocator<unsigned char> > *> image_data;
  std::map<IMAGEID,Bitmap<Color> *> images;
};


extern std::vector<const char *> g_urls;
extern std::string gameapi_homepageurl;

//bool feature_enable[255];

//void enable_features()
//{
  //std::cout << "Enable all features" << std::endl;
  //int s=255;
  //for(int i=0;i<s;i++) feature_enable[i]=true;
//}

static const float baseColorFactor = 1.0;

float baseColorChange(float val)
{
  //val/=(1.0/0.3);
  //val+=0.3;
  //std::cout << "BASECOLOR:" << val << std::endl;
  return val;
}


class GLTF_Model : public GLTFModelInterface
{
public:
  GLTF_Model(tinygltf::Model *model, std::string base_url, std::string url) : self(model), base_url(base_url), url(url) { }
  virtual std::string name() const { return "GLTF_Model"; }
  virtual void Prepare() { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }

  virtual std::string BaseUrl() const { return base_url; }
  virtual std::string Url() const { return url; }


  virtual int get_default_scene() const { return self->defaultScene; }
  
  virtual int accessors_size() const { return self->accessors.size(); }
  virtual const tinygltf::Accessor &get_accessor(int i) const {
    if (!self) { std::cout << "SELF FAIL(accessor)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->accessors.size())
	return self->accessors[i];
    std::cout << "ERROR: get_accessor out of bounds" << std::endl;
    exit(0);
  }

  
  virtual int animations_size() const { return self->animations.size(); }
  virtual const tinygltf::Animation &get_animation(int i) const {
    if (!self) { std::cout << "SELF FAIL(animation)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->animations.size())

    return self->animations[i];
    std::cout << "ERROR: get_animation out of bounds" << std::endl;
    exit(0);

  }

  virtual int buffers_size() const { return self->buffers.size(); }
  virtual const tinygltf::Buffer &get_buffer(int i) const {
    if (!self) { std::cout << "SELF FAIL(buffer)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->buffers.size())

    return self->buffers[i];

    std::cout << "ERROR: get_buffer out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int bufferviews_size() const { return self->bufferViews.size(); }
  virtual const tinygltf::BufferView &get_bufferview(int i) const {
    if (!self) { std::cout << "SELF FAIL(bufferview)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->bufferViews.size())

    return self->bufferViews[i];
    std::cout << "ERROR: get_bufferview out of bounds" << std::endl;
    exit(0);


  }
  
  virtual int materials_size() const { return self->materials.size(); }
  virtual const tinygltf::Material &get_material(int i) const {
    if (!self) { std::cout << "SELF FAIL(material)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->materials.size())
      
    return self->materials[i];

    std::cout << "ERROR: get_material out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int meshes_size() const { return self->meshes.size(); }
  virtual const tinygltf::Mesh &get_mesh(int i) const {
    if (!self) { std::cout << "SELF FAIL(mesh)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->meshes.size())
    return self->meshes[i];

    std::cout << "ERROR: get_mesh out of bounds" << std::endl;
    exit(0);

	}

  virtual int nodes_size() const { return self->nodes.size(); }
  virtual const tinygltf::Node &get_node(int i) const {
    if (!self) { std::cout << "SELF FAIL(node)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->nodes.size())

    return self->nodes[i];

    std::cout << "ERROR: get_node out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int textures_size() const { return self->textures.size(); }
  virtual const tinygltf::Texture &get_texture(int i) const {
    if (!self) { std::cout << "SELF FAIL(texture)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->textures.size())
    return self->textures[i];

    std::cout << "ERROR: get_texture out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int images_size() const { return self->images.size(); }
  virtual const tinygltf::Image &get_image(int i) const {
    if (!self) { std::cout << "SELF FAIL(image)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->images.size())

    return self->images[i];
    std::cout << "ERROR: get_image out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int skins_size() const { return self->skins.size(); }
  virtual const tinygltf::Skin &get_skin(int i) const {
    if (!self) { std::cout << "SELF FAIL(skin)" << url<< std::endl; exit(0); }

        if (i>=0&&i<self->skins.size())

    return self->skins[i];

    std::cout << "ERROR: get_skin out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int samplers_size() const { return self->samplers.size(); }
  virtual const tinygltf::Sampler &get_sampler(int i) const {
    if (!self) { std::cout << "SELF FAIL(sampler)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->samplers.size())

    return self->samplers[i];

    std::cout << "ERROR: get_sampler out of bounds" << std::endl;
    exit(0);

  }
  
  virtual int cameras_size() const { return self->cameras.size(); }
  virtual const tinygltf::Camera &get_camera(int i) const {
    if (!self) { std::cout << "SELF FAIL(camera)" << url<< std::endl; exit(0); }
    if (i>=0&&i<self->cameras.size())    
    return self->cameras[i];
    std::cout << "ERROR: get_camera out of bounds" << std::endl;
    exit(0);


  }

  virtual int scenes_size() const { return self->scenes.size(); }
  virtual const tinygltf::Scene &get_scene(int i) const {
    if (!self) { std::cout << "SELF FAIL(scene)" << url<< std::endl; exit(0); }

    if (i>=0&&i<self->scenes.size())
    return self->scenes[i];

    std::cout << "ERROR: get_scene out of bounds" << std::endl;
    exit(0);

  }

  virtual int lights_size() const { return self->lights.size(); }
  virtual const tinygltf::Light &get_light(int i) const {
    if (!self) { std::cout << "SELF FAIL(light)" << url << std::endl; exit(0); }

    if (i>=0&&i<self->lights.size())
    return self->lights[i];

    std::cout << "ERROR: get_light out of bounds" << std::endl;
    exit(0);

  }
  
public:
  tinygltf::Model *self;
public:
  std::string base_url, url;
};




void confirm_texture_usage(GameApi::Env &e, GameApi::P p);

class LoadGltf;

//GameApi::LI gltf_anim_skeleton2(GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int skin_num, int animation, int time_index);


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
  virtual bool IsDirectGltfImage() const { return bm->IsDirectGltfImage(); }
  virtual bool HasBatchMap() const { return bm->HasBatchMap(); }
  virtual BufferRef BatchMap(int start_x, int end_x, int start_y, int end_y) const { return get_bm()->BatchMap(start_x, end_x, start_y, end_y); }
public:
  GameApi::Env &e;
  std::string id;
  Bitmap<Color> *bm;
  mutable Bitmap<Color> *bm_cache=0;
};


bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr);
bool LoadImageData_from_string(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr);
bool WriteImageData(const std::string *basepath, const std::string *filename, tinygltf::Image *image, bool b, void *ptr);

bool FileExists(const std::string &abs_filename, void *ptr);
std::string ExpandFilePath(const std::string &str, void *ptr);
bool ReadWholeFile(std::vector<unsigned char,GameApiAllocator<unsigned char> > *out, std::string *err, const std::string &filepath, void *ptr);
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char,GameApiAllocator<unsigned char> > &contents, void *ptr);



struct ThreadInfo_gltf_bitmap
{
#ifdef THREADS
  pthread_t thread_id;
#endif
  tinygltf::Image *image;
  int req_width;
  int req_height;
  const unsigned char *bytes;
  int size;
  GLTFImageDecoder *decoder;
  int decoder_item;
  std::string url;
};

GameApi::Env *g_e = 0;

extern bool g_concurrent_download;

class LoadGltf;
struct LoadGltf2_data
{
  LoadGltf *obj;
  int id;
  FETCHID iid;
};
struct LoadGltf2_data_from_string
{
  LoadGltf_from_string *obj;
  int id;
  FETCHID iid;
};
void LoadGltf3_cb(void*);
void LoadGltf2_cb(void*);
void LoadGltf_cb(void *);
void LoadGltf_cb_from_string(void *);
void LoadGltf2_cb_from_string(void*);

int find_str(std::string val, std::string repl);

class LoadGltf : public CollectInterface
{
public:
  LoadGltf(GameApi::Env &e, std::string base_url, std::string url, std::string homepage, bool is_binary) : e(e), base_url(base_url), url(url), homepage(homepage), is_binary(is_binary) {
    g_e = &e;
    decoder = new GLTFImageDecoder(base_url,this);
    tinygltf::FsCallbacks fs = {
      &FileExists,
      &ExpandFilePath,
      &ReadWholeFile,
      &WriteWholeFile,
      (void*)this
    };
    tiny.SetFsCallbacks(fs);
    //#ifdef CONCURRENT_IMAGE_DECODE
#ifdef THREADS
    if (url.substr(url.size()-3,3)!="glb")
      tiny.SetImageLoader(&LoadImageData, this);
#endif
    //#endif
    //tiny.SetImageWriter(&WriteImageData, this);
    e.async_load_callback(url, &LoadGltf_cb, (void*)this);
  }
  ~LoadGltf()
  {
    g_deleted_urls.push_back(url);
    delete decoder;
    decoder=0;
    e.async_rem_callback(url);
    if (prepare_done) {
      
    }
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void PrePrePrepare(int i, FETCHID id)
  {
    if (!decoder) return;
    if (url.substr(url.size()-3,3)!="glb") {
    prepreprepare_done = true;
    std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec = decoder->get_file(e,id);
    if (!vec) return;
    //std::cout << "PrePrePrepare vecsize=" << vec->size() << std::endl;
    std::string filename = decoder->get_fetch_filename(id);
    //std::cout << "PrePrePrepare()" << filename << std::endl;
    FILEID iid = decoder->add_file(vec,filename);
    //#ifdef THREADS
    //delete vec;
    //#endif
    async_pending_count++;
    decoder->start_decode_process(id,iid,256,256);
    }
  }
  void PrePrepare() {
    if (!decoder) return;
    if (preprepare_done) return;
    preprepare_done = true;
    if (url.substr(url.size()-3,3)!="glb") {
      //std::cout << "PrePrepare()" << url << std::endl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);

#endif
    
    GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
    if (!vec) return;
    if (!vec->size()) return;
    std::vector<unsigned char,GameApiAllocator<unsigned char> > vec3(vec->begin(), vec->end());

    g_glb_file_size = g_glb_file_size > vec->size() ? g_glb_file_size : vec->size();



    
    std::vector<std::string> image_filenames = decoder->scan_gltf_file(vec3);

    //int ss = image_filenames.size();
    //std::cout << "filenames_count:" << ss << std::endl;
    //for(int i=0;i<ss;i++)
    //  {
    //std::cout << "IMAGE:" << image_filenames[i] << std::endl;
    // }
    
    
    std::vector<FETCHID> image_ids = decoder->fetch_ids(image_filenames);
    int ss = image_ids.size();
    for(int ii=0;ii<ss;ii++)
      {
	LoadGltf2_data *dt = new LoadGltf2_data;
	dt->obj = this;
	dt->id = ii;
	dt->iid = image_ids[ii];
	decoder->set_fetch_callback(e, image_ids[ii], &LoadGltf2_cb, (void*)dt); 
      }
    decoder->fetch_all_files(e,image_ids);

#ifndef EMSCRIPTEN
    std::map<FETCHID,std::string>::iterator it = decoder->filenames.begin();
    int ig = 0;
    for(;it!=decoder->filenames.end();it++)
      {
	std::pair<FETCHID,std::string> p = *it;
	PrePrePrepare(ig,p.first);
	ig++;
      }
#endif
    }
  }
  void Prepare() {
    if (prepare_done) return;
#ifndef EMSCRIPTEN
    PrePrepare();
#endif
    if (!preprepare_done)
      {
	PrePrepare();
      }
    //std::cout << "LoadGLTF: baseurl=" << base_url << std::endl;


 #ifdef THREADS
 #ifdef EMSCRIPTEN
    // wait for all threads to finish
    /*
    int ss = current_gltf_threads.size();
    for(int i=0;i<ss;i++)
      {
	void *res;
	pthread_join(current_gltf_threads[i]->thread_id,&res);
      }
    */
    tasks_join(3008);
    current_gltf_threads.clear();
 #endif
 #endif

    if (!prepreprepare_done) {
    std::map<FETCHID,std::string>::iterator it = decoder->filenames.begin();
    int ig = 0;
    for(;it!=decoder->filenames.end();it++)
      {
	std::pair<FETCHID,std::string> p = *it;
	PrePrePrepare(ig,p.first);
	ig++;
      }
    }
  

    
    
    //try {
      //std::cout << "LoadGLTF: url=" << url << std::endl;
    GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
    if (!vec) { std::cout << "LoadGLTF ASync not ready!" << std::endl; return; }
    int ssz = vec->end()-vec->begin();
    if (ssz<1) {
      std::cout << "LoadGLTF: ssz=" << ssz << std::endl;
      return;
    }
    std::vector<char> vec2(vec->begin(), vec->end());
    std::string str(vec->begin(),vec->end());


    bool is_animated = false;
    {
    int val = find_str(str,"\"animations\"");
    if (val!=-1)
      {
	is_animated=true;
	g_glb_animated=true;
      }
    }


    //std::cout << str << std::endl;
    //bool b = false;
    std::string err;
    std::string warn;
    if (!is_binary) {
      //std::cout << "File size: " << url  << "::" << str.size() << std::endl;
      int sz = str.size();

      if (g_concurrent_download) {
#ifdef EMSCRIPTEN
    int s = g_urls.size();
    bool has_space = true;
    for(int i=0;i<s;i++) {
      //std::cout << "Compare:" << g_urls[i] << "==" << url << std::endl;
      if (std::string(g_urls[i])==url) has_space=false;
    }
    if (has_space) {
      sz--;
    }
    if (sz<0) sz=0;
#endif
      }
    //std::cout << "ASCII: " << std::string(vec2.begin(),vec2.end()) << std::endl;
      tiny.LoadASCIIFromString(&model, &err, &warn, &vec2.operator[](0), sz, base_url, tinygltf::REQUIRE_ALL);
    } else {
      int sz = vec->size();
      //std::cout << "File size: " << url  << "::" << sz << std::endl;
      if (g_concurrent_download) {
#ifdef EMSCRIPTEN
    int s = g_urls.size();
    bool has_space = true;
    for(int i=0;i<s;i++) {
      //std::cout << "Compare:" << g_urls[i] << "==" << url << std::endl;
      if (std::string(g_urls[i])==url) has_space=false;
    }
    if (has_space)
      {
      sz--;
      }
    if (sz<0) sz=0;
#endif
      }
    char *ptr2 = &vec2.operator[](0);
    unsigned char *ptr3 = (unsigned char*)ptr2;
    //std::cout << "DATASIZE: " << vec2.size() << " " << sz << std::endl;
      tiny.LoadBinaryFromMemory(&model, &err, &warn, ptr3, sz, base_url, tinygltf::REQUIRE_ALL); 
    }
    if (!warn.empty()) { std::cout << "WARN: " << warn << std::endl; }
    if (!err.empty()) { std::cout << "ERROR: " << err << std::endl; }
    prepare_done = true;
    delete vec;
    //} catch(int a) { std::cout << "GltfLoad::Prepare() exception:" << url << std::endl; }
  }
  void set_urls(std::string burl, std::string url2) { base_url=burl; url=url2; }
public:
  GameApi::Env &e;
  std::string base_url;
  std::string url;
  std::string homepage;
  bool is_binary;
  tinygltf::TinyGLTF tiny;
  tinygltf::Model model;
  bool prepare_done = false;
  GLTFImageDecoder *decoder=0;
  bool preprepare_done = false;
  bool prepreprepare_done = false;
  std::vector<ThreadInfo_gltf_bitmap*> current_gltf_threads;
};

void LoadGltf_cb(void *ptr)
{
  LoadGltf *obj = (LoadGltf*)ptr;
  obj->PrePrepare();
}
void LoadGltf2_cb(void *ptr)
{
  LoadGltf2_data *dt = (LoadGltf2_data*)ptr;
  dt->obj->PrePrePrepare(dt->id, dt->iid);
}
class LoadGltf_from_string : public CollectInterface
{
public:
  LoadGltf_from_string(GameApi::Env &e, std::string data, std::string base_url,std::string url, std::string homepage, bool is_binary) : e(e), data(data), url(url), base_url(base_url), homepage(homepage), is_binary(is_binary) {
    g_e = &e;
    decoder = new GLTFImageDecoder(base_url,this);
    tinygltf::FsCallbacks fs = {
      &FileExists,
      &ExpandFilePath,
      &ReadWholeFile,
      &WriteWholeFile,
      (void*)this
    };
    tiny.SetFsCallbacks(fs);
    //#ifdef CONCURRENT_IMAGE_DECODE
#ifdef THREADS
    if (url.substr(url.size()-3,3)!="glb")
      tiny.SetImageLoader(&LoadImageData_from_string, this);
#endif
    //#endif
    //tiny.SetImageWriter(&WriteImageData, this);
    e.async_load_callback(url, &LoadGltf_cb_from_string, (void*)this);
  }
  ~LoadGltf_from_string()
  {
    g_deleted_urls.push_back(url);
    delete decoder;
    decoder=0;
    e.async_rem_callback(url);
    if (prepare_done) {
      
    }
  }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void PrePrePrepare(int i, FETCHID id)
  {
    if (!decoder) return;
    if (url.substr(url.size()-3,3)!="glb") {
    prepreprepare_done = true;
    std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec = decoder->get_file(e,id);
    if (!vec) return;
    //std::cout << "PrePrePrepare vecsize=" << vec->size() << std::endl;
    std::string filename = decoder->get_fetch_filename(id);
    //std::cout << "PrePrePrepare()" << filename << std::endl;
    FILEID iid = decoder->add_file(vec,filename);
    //#ifdef THREADS
    //delete vec;
    //#endif
    async_pending_count++;
    decoder->start_decode_process(id,iid,256,256);
    }
  }
  void PrePrepare() {
    if (!decoder) return;
    if (preprepare_done) return;
    preprepare_done = true;
    if (url.substr(url.size()-3,3)!="glb") {
      //std::cout << "PrePrepare()" << url << std::endl;
      //#ifndef EMSCRIPTEN
      //e.async_load_url(url, homepage);

      //#endif
    
    //GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
    //if (!vec) return;
    if (!data.size()) return;
    std::vector<unsigned char,GameApiAllocator<unsigned char> > vec3(data.begin(), data.end());

    g_glb_file_size = g_glb_file_size > data.size() ? g_glb_file_size : data.size();



    
    std::vector<std::string> image_filenames = decoder->scan_gltf_file(vec3);

    //int ss = image_filenames.size();
    //std::cout << "filenames_count:" << ss << std::endl;
    //for(int i=0;i<ss;i++)
    //  {
    //std::cout << "IMAGE:" << image_filenames[i] << std::endl;
    // }
    
    
    std::vector<FETCHID> image_ids = decoder->fetch_ids(image_filenames);
    int ss = image_ids.size();
    for(int ii=0;ii<ss;ii++)
      {
	LoadGltf2_data_from_string *dt = new LoadGltf2_data_from_string;
	dt->obj = this;
	dt->id = ii;
	dt->iid = image_ids[ii];
	decoder->set_fetch_callback(e, image_ids[ii], &LoadGltf2_cb_from_string, (void*)dt); 
      }
    decoder->fetch_all_files(e,image_ids);

#ifndef EMSCRIPTEN
    std::map<FETCHID,std::string>::iterator it = decoder->filenames.begin();
    int ig = 0;
    for(;it!=decoder->filenames.end();it++)
      {
	std::pair<FETCHID,std::string> p = *it;
	PrePrePrepare(ig,p.first);
	ig++;
      }
#endif
    }
  }
  void Prepare() {
    if (prepare_done) return;
#ifndef EMSCRIPTEN
    PrePrepare();
#endif
    if (!preprepare_done)
      {
	PrePrepare();
      }
    //std::cout << "LoadGLTF: baseurl=" << base_url << std::endl;


 #ifdef THREADS
 #ifdef EMSCRIPTEN
    // wait for all threads to finish
    /*
    int ss = current_gltf_threads.size();
    for(int i=0;i<ss;i++)
      {
	void *res;
	pthread_join(current_gltf_threads[i]->thread_id,&res);
      }
    */
    tasks_join(3008);
    current_gltf_threads.clear();
 #endif
 #endif

    if (!prepreprepare_done) {
    std::map<FETCHID,std::string>::iterator it = decoder->filenames.begin();
    int ig = 0;
    for(;it!=decoder->filenames.end();it++)
      {
	std::pair<FETCHID,std::string> p = *it;
	PrePrePrepare(ig,p.first);
	ig++;
      }
    }
  

    
    
    //try {
      //std::cout << "LoadGLTF: url=" << url << std::endl;
    //GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
    //if (!vec) { std::cout << "LoadGLTF ASync not ready!" << std::endl; return; }
    std::vector<unsigned char> *vec = new std::vector<unsigned char>(data.begin(), data.end());
    int ssz = vec->end()-vec->begin();
    if (ssz<1) {
      std::cout << "LoadGLTF: ssz=" << ssz << std::endl;
      return;
    }
    std::vector<char> vec2(vec->begin(), vec->end());
    std::string str(vec->begin(),vec->end());


    std::cout << "LOADGLTF:" << str.substr(0,50) << std::endl;

    
    bool is_animated = false;
    {
    int val = find_str(str,"\"animations\"");
    if (val!=-1)
      {
	is_animated=true;
	g_glb_animated=true;
      }
    }


    //std::cout << str << std::endl;
    //bool b = false;
    std::string err;
    std::string warn;
    if (!is_binary) {
      //std::cout << "File size: " << url  << "::" << str.size() << std::endl;
      int sz = str.size();

      if (g_concurrent_download) {
#ifdef EMSCRIPTEN
    int s = g_urls.size();
    bool has_space = true;
    for(int i=0;i<s;i++) {
      //std::cout << "Compare:" << g_urls[i] << "==" << url << std::endl;
      if (std::string(g_urls[i])==url) has_space=false;
    }
    if (has_space) {
      sz--;
    }
    if (sz<0) sz=0;
#endif
      }
    //std::cout << "ASCII: " << std::string(vec2.begin(),vec2.end()) << std::endl;
    std::cout << "LoadASCII" << std::endl;
      tiny.LoadASCIIFromString(&model, &err, &warn, &vec2.operator[](0), sz, base_url, tinygltf::REQUIRE_ALL);
    } else {
      int sz = vec->size();
      //std::cout << "File size: " << url  << "::" << sz << std::endl;
      if (g_concurrent_download) {
#ifdef EMSCRIPTEN
    int s = g_urls.size();
    bool has_space = true;
    for(int i=0;i<s;i++) {
      //std::cout << "Compare:" << g_urls[i] << "==" << url << std::endl;
      if (std::string(g_urls[i])==url) has_space=false;
    }
    if (has_space)
      {
      sz--;
      }
    if (sz<0) sz=0;
#endif
      }
    char *ptr2 = &vec2.operator[](0);
    unsigned char *ptr3 = (unsigned char*)ptr2;
    //std::cout << "DATASIZE: " << vec2.size() << " " << sz << std::endl;
    std::cout << "LoadBInary" << std::endl;
      tiny.LoadBinaryFromMemory(&model, &err, &warn, ptr3, sz, base_url, tinygltf::REQUIRE_ALL); 
    }
    if (!warn.empty()) { std::cout << "WARN: " << warn << std::endl; }
    if (!err.empty()) { std::cout << "ERROR: " << err << std::endl; }
    prepare_done = true;
    delete vec;
    //} catch(int a) { std::cout << "GltfLoad::Prepare() exception:" << url << std::endl; }
  }
  void set_urls(std::string burl, std::string url2) { base_url=burl; url=url2; }
public:
  GameApi::Env &e;
  std::string base_url;
  std::string url;
  std::string data;
  std::string homepage;
  bool is_binary;
  tinygltf::TinyGLTF tiny;
  tinygltf::Model model;
  bool prepare_done = false;
  GLTFImageDecoder *decoder=0;
  bool preprepare_done = false;
  bool prepreprepare_done = false;
  std::vector<ThreadInfo_gltf_bitmap*> current_gltf_threads;
};

void LoadGltf_cb_from_string(void *ptr)
{
  LoadGltf_from_string *obj = (LoadGltf_from_string*)ptr;
  obj->PrePrepare();
}
void LoadGltf2_cb_from_string(void *ptr)
{
  LoadGltf2_data_from_string *dt = (LoadGltf2_data_from_string*)ptr;
  dt->obj->PrePrePrepare(dt->id, dt->iid);
}


class GLTF_Model_with_prepare : public GLTF_Model
{
public:
  std::string name() const { return "GLTF_Model_with_prepare"; }
  GLTF_Model_with_prepare(LoadGltf *load, tinygltf::Model *model) : GLTF_Model(model,load->base_url, load->url), load(load), model(model) { firsttime=true; }
  virtual void Prepare() { if (firsttime&&load) { load->Prepare(); self=&load->model; model=&load->model; firsttime=false; } }
  virtual void Collect(CollectVisitor &vis) {  vis.register_obj(this); }
  virtual void HeavyPrepare() { if (firsttime&&load) { load->Prepare(); self=&load->model; model=&load->model; firsttime=false; } }
public:
  LoadGltf *load=0;
  tinygltf::Model *model=0;
  bool firsttime;
};


class GLTF_Model_with_prepare_from_string : public GLTF_Model
{
public:
  std::string name() const { return "GLTF_Model_with_prepare_from_string"; }
  GLTF_Model_with_prepare_from_string(LoadGltf_from_string *load, tinygltf::Model *model) : GLTF_Model(model,load->base_url, load->url), load(load), model(model) { firsttime=true; }
  virtual void Prepare() { if (firsttime&&load) { load->Prepare(); self=&load->model; model=&load->model; firsttime=false; } }
  virtual void Collect(CollectVisitor &vis) {  vis.register_obj(this); }
  virtual void HeavyPrepare() { if (firsttime&&load) { load->Prepare(); self=&load->model; model=&load->model; firsttime=false; } }
public:
  LoadGltf_from_string *load=0;
  tinygltf::Model *model=0;
  bool firsttime;
};



struct KeyStruct
{
  std::string key;
  LoadGltf *obj;
};

std::vector<KeyStruct> g_gltf_instances;
struct KeyStruct_from_string
{
  std::string key;
  LoadGltf_from_string *obj;
};

std::vector<KeyStruct_from_string> g_gltf_instances_from_string;

bool instance_deleter_installed=false;

int register_cache_deleter(void (*fptr)(void*), void*);


#ifdef EMSCRIPTEN
void del_instances(void*)
{
  int s3 = g_model_del_items.size();
  for(int ii=0;ii<s3;ii++)
    {
      GLTF_Model_with_prepare* item = g_model_del_items[ii];
      item->load = 0;
      item->model = 0;
      item->self = 0;
    }
  
  int s = g_gltf_instances.size();
  for(int i=0;i<s;i++)
    {
     KeyStruct &s = g_gltf_instances[i];
     //std::cout << "POINTER:" << (int)s.obj << std::endl;
     delete s.obj->decoder;
     s.obj->decoder = 0;
     s.obj->model.accessors.clear();
     s.obj->model.animations.clear();
     s.obj->model.buffers.clear();
     s.obj->model.bufferViews.clear();
     s.obj->model.materials.clear();
     s.obj->model.meshes.clear();
     s.obj->model.nodes.clear();
     s.obj->model.textures.clear();
     s.obj->model.images.clear();
     s.obj->model.skins.clear();
     s.obj->model.samplers.clear();
     s.obj->model.cameras.clear();
     s.obj->model.scenes.clear();
     s.obj->model.lights.clear();
     
    }
  g_gltf_instances.clear();
  //instance_deleter_installed=false;



  int s2 = g_model_del_items_from_string.size();
  for(int ii=0;ii<s2;ii++)
    {
      GLTF_Model_with_prepare_from_string* item = g_model_del_items_from_string[ii];
      item->load = 0;
      item->model = 0;
      item->self = 0;
    }
  
  int s4 = g_gltf_instances_from_string.size();
  for(int i=0;i<s4;i++)
    {
     KeyStruct_from_string &s = g_gltf_instances_from_string[i];
     //std::cout << "POINTER:" << (int)s.obj << std::endl;
     delete s.obj->decoder;
     s.obj->decoder = 0;
     s.obj->model.accessors.clear();
     s.obj->model.animations.clear();
     s.obj->model.buffers.clear();
     s.obj->model.bufferViews.clear();
     s.obj->model.materials.clear();
     s.obj->model.meshes.clear();
     s.obj->model.nodes.clear();
     s.obj->model.textures.clear();
     s.obj->model.images.clear();
     s.obj->model.skins.clear();
     s.obj->model.samplers.clear();
     s.obj->model.cameras.clear();
     s.obj->model.scenes.clear();
     s.obj->model.lights.clear();
     
    }
  g_gltf_instances_from_string.clear();
  //instance_deleter_installed=false;

}
#endif

LoadGltf *find_gltf_instance(GameApi::Env &e, std::string base_url, std::string url, std::string homepage, bool is_binary)
{
#ifdef EMSCRIPTEN
  if (instance_deleter_installed==false)
    {
      register_cache_deleter(&del_instances,(void*)0);
      instance_deleter_installed=true;
  }
#endif
  
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


LoadGltf_from_string *find_gltf_instance_from_string(GameApi::Env &e, std::string data, std::string base_url, std::string url, std::string homepage, bool is_binary)
{
#ifdef EMSCRIPTEN
  if (instance_deleter_installed==false)
    {
      register_cache_deleter(&del_instances,(void*)0);
      instance_deleter_installed=true;
  }
#endif
  
  std::string key = base_url + ":" + url;

  int s = g_gltf_instances_from_string.size();
  for(int i=0;i<s;i++) {
    if (g_gltf_instances_from_string[i].key == key) return g_gltf_instances_from_string[i].obj;
  }
  LoadGltf_from_string *obj = new LoadGltf_from_string(e,data,base_url,url,homepage,is_binary);
  //obj->Prepare();
  KeyStruct_from_string s2;
  s2.key = key;
  s2.obj = obj;
  g_gltf_instances_from_string.push_back(s2);
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

extern bool g_concurrent_download;

int g_requestedBytes;

bool ReadWholeFile(std::vector<unsigned char,GameApiAllocator<unsigned char> > *out, std::string *err, const std::string &filepath, void *ptr)
{
#ifdef THREADS
  if (filepath.substr(filepath.size()-3,3)=="glb"||
      filepath.substr(filepath.size()-3,3)=="bin"||
      filepath.substr(filepath.size()-4,4)=="gltf")
    {
#endif
      
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "ReadWholeFile " << filepath << std::endl;
  std::string url = filepath;
  //std::cout << "ReadWholeFile: " << url << std::endl;
  // remove starting / from file names.
  if (url.size()>0 && url[0]=='/') url=url.substr(1);
  // remove ending \" from the file names
  if (url.size()>0 && url[url.size()-1]=='\"') url=url.substr(0,url.size()-1);

#ifndef EMSCRIPTEN
    g_e->async_load_url(url, gameapi_homepageurl);
#endif
    GameApi::ASyncVec *vec = g_e->get_loaded_async_url(url);
    if (!vec) { std::cout << "ReadWholeFile::async not ready:" << url << std::endl; std::cout << "Please use async_url() to register it to system" << std::endl; return false; } else {
#ifndef EMSCRIPTEN
      if (!is_in_registered(url))
	std::cout << "\nWarning: Please note that you might need to add async_url() or async_gltf for " << url << std::endl;
#endif
    }
    int sz = vec->size();
    if (g_concurrent_download) {
#ifdef EMSCRIPTEN
    int s = g_urls.size();
    bool has_space = true;
    for(int i=0;i<s;i++) {
      //std::cout << "Compare:" << g_urls[i] << "==" << url << std::endl;
      if (std::string(g_urls[i])==url) has_space=false;
    }
    if (has_space)
      sz--;
#endif
    }
    *out = std::vector<unsigned char,GameApiAllocator<unsigned char> >(vec->begin(),vec->begin()+sz);
    delete vec;
    return true;
#ifdef THREADS
    } else
    {
      // png files are loaded via different route.
      *out = std::vector<unsigned char,GameApiAllocator<unsigned char> >();
      return true;
    }
#endif
} 
bool WriteWholeFile(std::string *err, const std::string &filepath, const std::vector<unsigned char,GameApiAllocator<unsigned char> > &contents, void *ptr)
{
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "WriteWholeFile" << filepath << std::endl;
  return false;
}

class GLTFImageDecoder;




void *thread_func_gltf_bitmap(void *data2)
{
  
  ThreadInfo_gltf_bitmap *bm = (ThreadInfo_gltf_bitmap*)data2;

  int image_idx = 0;
  tinygltf::Image *image = bm->image;

  //std::cout << "START_DECODING_IN_THREAD" << bm->url << std::endl;

  
  int req_width = bm->req_width;
  int req_height = bm->req_height;
  const unsigned char *bytes = bm->bytes;
  int size = bm->size;


  std::string res;
  std::string *err = &res;
  std::string *warn = &res;
  

  
  (void)warn;

  //tinygltf::LoadImageDataOption option;
  //if (user_data) {
  //  option = *reinterpret_cast<LoadImageDataOption *>(user_data);
  //}

  int w = 0, h = 0, comp = 0, req_comp = 0;

  unsigned char *data = nullptr;

  // preserve_channels true: Use channels stored in the image file.
  // false: force 32-bit textures for common Vulkan compatibility. It appears
  // that some GPU drivers do not support 24-bit images for Vulkan
  req_comp = 4; //option.preserve_channels ? 0 : 4;
  int bits = 8;
  int pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

  // It is possible that the image we want to load is a 16bit per channel image
  // We are going to attempt to load it as 16bit per channel, and if it worked,
  // set the image data accodingly. We are casting the returned pointer into
  // unsigned char, because we are representing "bytes". But we are updating
  // the Image metadata to signal that this image uses 2 bytes (16bits) per
  // channel:
  if (stbi_is_16_bit_from_memory(bytes, size)) {
    data = reinterpret_cast<unsigned char *>(
        stbi_load_16_from_memory(bytes, size, &w, &h, &comp, req_comp));
    if (data) {
      bits = 16;
      pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
    }
  }

  // at this point, if data is still NULL, it means that the image wasn't
  // 16bit per channel, we are going to load it as a normal 8bit per channel
  // mage as we used to do:
  // if image cannot be decoded, ignore parsing and keep it by its path
  // don't break in this case
  // FIXME we should only enter this function if the image is embedded. If
  // image->uri references
  // an image file, it should be left as it is. Image loading should not be
  // mandatory (to support other formats)
  if (!data) data = stbi_load_from_memory(bytes, size, &w, &h, &comp, req_comp);
  if (!data) {
    // NOTE: you can use `warn` instead of `err`
    if (err) {
      (*err) +=
          "Unknown image format. STB cannot decode image data for image[" +
          std::to_string(image_idx) + "] name = \"" + image->name + "\".\n";
    }
  FILEID id;
  id.id = bm->decoder_item;
  delete [] bm->bytes;
  delete bm->decoder->files2[id];
  bm->decoder->files2[id]=0;

  image->width = 1;
  image->height = 1;
  image->component = req_comp;
  image->bits = bits;
  image->pixel_type = pixel_type;
  image->image.resize(static_cast<uint64_t>(1 * 1 * req_comp) * size_t(bits / 8));

  
  std::cout << "ERROR1, size=" << size << std::endl;
  async_pending_count--;
    return 0;
  }

  if ((w < 1) || (h < 1)) {
    stbi_image_free(data);
    if (err) {
      (*err) += "Invalid image data for image[" + std::to_string(image_idx) +
                "] name = \"" + image->name + "\"\n";
    }
    std::cout << "ERROR2" << std::endl;
  FILEID id;
  id.id = bm->decoder_item;
  delete [] bm->bytes;
  delete bm->decoder->files2[id];
  bm->decoder->files2[id]=0;
  image->width = 1;
  image->height = 1;
  image->component = req_comp;
  image->bits = bits;
  image->pixel_type = pixel_type;
  image->image.resize(static_cast<uint64_t>(1 * 1 * req_comp) * size_t(bits / 8));


  async_pending_count--;
    return 0;
  }
  /*
  if (req_width > 0) {
    if (req_width != w) {
      stbi_image_free(data);
      if (err) {
        (*err) += "Image width mismatch for image[" +
                  std::to_string(image_idx) + "] name = \"" + image->name +
                  "\"\n";
      }
    std::cout << "ERROR3" << std::endl;
      return 0;
    }
  }
  */
  /*
  if (req_height > 0) {
    if (req_height != h) {
      stbi_image_free(data);
      if (err) {
        (*err) += "Image height mismatch. for image[" +
                  std::to_string(image_idx) + "] name = \"" + image->name +
                  "\"\n";
      }
    std::cout << "ERROR4" << std::endl;
      return 0;
    }
  }
  */
  if (req_comp != 0) {
    // loaded data has `req_comp` channels(components)
    comp = req_comp;
  }

  image->width = w;
  image->height = h;
  image->component = comp;
  image->bits = bits;
  image->pixel_type = pixel_type;
  //std::cout << "w:" << w << " h:" << h << " comp:" << comp << " bits:" << bits << std::endl;
  //std::cout << "image:" << (long)image << std::endl;
  image->image.resize(static_cast<uint64_t>(w * h * comp) * size_t(bits / 8));
  std::copy(data, data + w * h * comp * (bits / 8), image->image.begin());
  stbi_image_free(data);


  delete [] bm->bytes;


  //std::cout << "DECODING DONE" << bm->url << std::endl;
  
  FILEID id;
  id.id = bm->decoder_item;
  
  bm->decoder->decoded_image[id] = image;
  bm->decoder->decoded_files[id] = &image->image;
  if (bm->decoder->decode_cb[id])
    bm->decoder->decode_cb[id](bm->decoder->decode_user_data[id]);

#ifdef THREADS
#ifdef EMSCRIPTEN
  delete bm->decoder->files2[id];
  bm->decoder->files2[id]=0;
#endif
#endif
  
  async_pending_count--;
  
  return 0;


}

class GLTFImageDecoder;

//std::vector<ThreadInfo_gltf_bitmap*> current_gltf_threads;


void start_gltf_bitmap_thread(tinygltf::Image *image, int req_width, int req_height, const unsigned char *bytes, int size, GLTFImageDecoder *decoder, int decoder_item, std::string url)
{
#ifdef THREADS
  
  image->width = req_width;
  image->height = req_height;
  image->component = 3;
  image->bits = 8;
  image->pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
  
  ThreadInfo_gltf_bitmap *info = new ThreadInfo_gltf_bitmap;
  info->url = url;
  info->image = image;
  info->req_width = req_width;
  info->req_height = req_height;
  info->decoder = decoder;
  info->decoder_item = decoder_item;
  unsigned char *bytes2 = new unsigned char[size];
  std::copy(bytes, bytes+size,bytes2);
  info->bytes = bytes2;
  info->size = size;

  decoder->load->current_gltf_threads.push_back(info);
  
  //pthread_attr_t attr;
  //pthread_attr_init(&attr);
  //pthread_attr_setstacksize(&attr,300000);
  g_pthread_count++;
  //pthread_create(&info->thread_id, &attr, &thread_func_gltf_bitmap, (void*)info);
  tasks_add(3008,&thread_func_gltf_bitmap,(void*)info);
  
#endif
  
}

void gltf_join_threads(GLTFImageDecoder *decoder)
{
#ifdef THREADS
#ifdef EMSCRIPTEN
  //int ss = decoder->load->current_gltf_threads.size();
    /*
    for(int i=0;i<ss;i++)
      {
	void *res;
	pthread_join(decoder->load->current_gltf_threads[i]->thread_id,&res);
      }
    */
    tasks_join(3008);
    decoder->load->current_gltf_threads.clear();
#endif
#endif
}


bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr)
{
  LoadGltf *dt = (LoadGltf*)ptr;
  std::string url = dt->base_url;
  if (url.size()!=0&&url.substr(url.size()-1,1)!="/") url.push_back('/');
  url+=image->uri;
  //std::cout << "FIND_FILE:" << url << std::endl;
  FILEID id = dt->decoder->find_file(url);
  //if (!dt->decoder->is_decoded(id)) {
    //std::cout << "JOINING BITMAP THREADS!" << std::endl;
  //  gltf_join_threads(dt->decoder);
  // }
  //FILEID id2 = dt->decoder->find_file(url);
  if (dt->decoder->is_decoded(id)) {
    std::vector<unsigned char, GameApiAllocator<unsigned char> > *ptr2 = dt->decoder->get_decoded_file(id);
    //std::cout << "OUTPUT0:" << image->width << "x" << image->height << " " << image->component << " " << image->pixel_type << " " << image->bits << std::endl;
    image->image = *ptr2;
    image->width = dt->decoder->decoded_image[id]->width;
    image->height = dt->decoder->decoded_image[id]->height;
    image->component = dt->decoder->decoded_image[id]->component;
    image->pixel_type = dt->decoder->decoded_image[id]->pixel_type;
    image->bits = dt->decoder->decoded_image[id]->bits;
    //std::cout << "OUTPUT:" << image->width << "x" << image->height << " " << image->component << " " << image->pixel_type << " " << image->bits << std::endl;
  } else { std::cout << "FILE WAS NOT DECODED YET:" << dt->decoder->load->current_gltf_threads.size() << "::" << dt->base_url + image->uri << std::endl; }
  //#ifdef CONCURRENT_IMAGE_DECODE
  //start_gltf_bitmap_thread(image, req_width, req_height, bytes, size);
  //return true;
  //#endif
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "LoadImageData " << req_width << " " << req_height << " " << size << std::endl;
  //return false;
  return true;
}
bool LoadImageData_from_string(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *ptr)
{
  LoadGltf_from_string *dt = (LoadGltf_from_string*)ptr;
  std::string url = dt->base_url;
  if (url.size()!=0&&url.substr(url.size()-1,1)!="/") url.push_back('/');
  url+=image->uri;
  //std::cout << "FIND_FILE:" << url << std::endl;
  FILEID id = dt->decoder->find_file(url);
  //if (!dt->decoder->is_decoded(id)) {
    //std::cout << "JOINING BITMAP THREADS!" << std::endl;
  //  gltf_join_threads(dt->decoder);
  // }
  //FILEID id2 = dt->decoder->find_file(url);
  if (dt->decoder->is_decoded(id)) {
    std::vector<unsigned char, GameApiAllocator<unsigned char> > *ptr2 = dt->decoder->get_decoded_file(id);
    //std::cout << "OUTPUT0:" << image->width << "x" << image->height << " " << image->component << " " << image->pixel_type << " " << image->bits << std::endl;
    image->image = *ptr2;
    image->width = dt->decoder->decoded_image[id]->width;
    image->height = dt->decoder->decoded_image[id]->height;
    image->component = dt->decoder->decoded_image[id]->component;
    image->pixel_type = dt->decoder->decoded_image[id]->pixel_type;
    image->bits = dt->decoder->decoded_image[id]->bits;
    //std::cout << "OUTPUT:" << image->width << "x" << image->height << " " << image->component << " " << image->pixel_type << " " << image->bits << std::endl;
  } else { std::cout << "FILE WAS NOT DECODED YET:" << dt->decoder->load->current_gltf_threads.size() << "::" << dt->base_url + image->uri << std::endl; }
  //#ifdef CONCURRENT_IMAGE_DECODE
  //start_gltf_bitmap_thread(image, req_width, req_height, bytes, size);
  //return true;
  //#endif
  //LoadGltf *data = (LoadGltf*)ptr;
  //std::cout << "LoadImageData " << req_width << " " << req_height << " " << size << std::endl;
  //return false;
  return true;
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
  GLTFImage(GLTFModelInterface *interface, int image_index) : interface(interface), image_index(image_index) {
    img = 0;
  }
  void Collect(CollectVisitor &vis)
  {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (image_index>=0 && image_index<int(interface->images_size()))
      img = &interface->get_image(image_index);
  }

  
  virtual void Prepare()
  {
    interface->Prepare();
    if (image_index>=0 && image_index<int(interface->images_size()))
      img = &interface->get_image(image_index);
  }

  virtual int SizeX() const { if (img) return img->width; return 0; }
  virtual int SizeY() const { if (img) return img->height; return 0; }
  virtual Color Map(int x, int y) const
  {
    //if (!img) return Color(0x0);
    //if (x<0||x>=img->width) return Color(0x0);
    //if (y<0||y>=img->height) return Color(0x0);
    const unsigned char *ptr = &img->image[0];
    int offset = (x*img->component + y*img->width*img->component)*(img->bits/8);
    //if (img->component<0) { offset=(x+y*img->width)*(img->bits/8); img->component=4; }
    if ((img->component==4 ||img->component==3)&& img->bits==8) {


      unsigned int val = *(unsigned int*)(ptr+offset);


	unsigned int a = val &0xff000000;
	if (img->component==3) a=0xff000000;
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
	if (img->component==3) a=0xff;
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
    std::cout << "GLTF Image format not recognized" << std::endl;
    return Color(0x0);
  }
  virtual bool IsDirectGltfImage() const { return true; }
  virtual bool HasBatchMap() const {
    if (img->bits==8&&img->component==4)
      return true;
    return false;
  }
  virtual BufferRef BatchMap(int start_x, int end_x, int start_y, int end_y) const
  {
    return BufferRef::NewBufferWithoutAlloc(((unsigned int*)&img->image[0]) + start_x + start_y*img->width, end_x-start_x, end_y-start_y, img->width);
  }
public:
  GLTFModelInterface *interface;
  const tinygltf::Image *img;
  int image_index;
};

void bm_cb(void* ptr2);

class LoadBitmapFromUrl : public Bitmap<Color>
{
public:
  LoadBitmapFromUrl(GameApi::Env &env, std::string url, std::string homepage) : env(env), url(url),homepage(homepage) { cbm = 0;
    async_pending_count++;
    // id=register_cache_deleter(&del_bitmap_cache,(void*)this);
    env.async_load_callback(url,&bm_cb,this);
  }
  void unasync()
  {
    async_pending_count--;
  }
  ~LoadBitmapFromUrl()
  {
    //unregister_cache_deleter(id);
  }
  virtual int SizeX() const {
    if (!cbm) { return 100; }
    return cbm->SizeX(); }
  virtual int SizeY() const {
    if (!cbm) { return 100; } 
    return cbm->SizeY(); }
  virtual Color Map(int x, int y) const { 
    if (!cbm) { return Color(0xffffffff); }
    return cbm->Map(x,y); }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    Prepare();
  }
  void Prepare()
  {
    if (!cbm) {
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "async not ready!" << std::endl; return; }
      std::string s(vec->begin(), vec->end());
      std::vector<unsigned char> vec2(vec->begin(),vec->end());

      
      //std::cout << "VEC2 size=" << vec2.size() << std::endl;

      // sometimes we get failures if prepare is called too early, so this is good place to try to recover from it. Next prepare call will have correct data.
      if (vec2.size()<3) return; 
      
      bool b = false;
      img = LoadImageFromString(vec2, b);
      
    if (b==false) {
      img = BufferRef::NewBuffer(10,10);
      for(int x=0;x<10;x++)
	for(int y=0;y<10;y++)
	  {
	    img.buffer[x+y*img.ydelta] = ((x+y)&1)==1 ? 0xffffffff : 0xff000000;
	  }
      //std::cout << "ERROR: File not found: " << filename << std::endl;
    }
    cbm = new BitmapFromBuffer(img);
    }
  }
  void del_cache()
  {
    BufferRef::FreeBuffer(img);
    cbm=0;
  }
public:
  GameApi::Env &env;
  std::string url;
  std::string homepage;
  BufferRef img;
  Bitmap<Color> *cbm=0;
  bool load_finished = false;  
  int id;
};
void bm_cb(void* ptr2)
{
  LoadBitmapFromUrl* ptr = (LoadBitmapFromUrl*)ptr2;
  ptr->unasync();
}


EXPORT GameApi::BM GameApi::BitmapApi::loadbitmapfromurl(std::string url)
{
  int c = get_current_block();
  set_current_block(-1);
  Bitmap<Color> *bm = new LoadBitmapFromUrl(e,url,gameapi_homepageurl);
  Bitmap<Color> *bbm = new BitmapPrepareCache(e, url, bm);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bbm;
  BM bm2 = add_bitmap(e, handle);
  set_current_block(c);
  return bm2;
}


void BufferFromBitmap::Gen(int start_x, int end_x, int start_y, int end_y) const
{
#if 0
  if (t.IsDirectSTBIImage())
    { // FAST PATH FOR STB IMAGE
      //std::cout << "STBFASTPATH" << std::endl;
      FlipColours *bm1 = (FlipColours*)&t;
      BitmapPrepareCache *cache = (BitmapPrepareCache*)&bm1->c;
      LoadBitmapFromUrl *url = (LoadBitmapFromUrl*)cache->get_bm();
      BufferRef img = url->img;
      for(int y=start_y;y<end_y;y++) {
	int dd = y*buf.ydelta;
	int dd2= y*img.ydelta;
	for(int x=start_x;x<end_x;x++)
	  {
	    unsigned int val = img.buffer[dd2+x];
	      unsigned int a = val &0xff000000;
	      unsigned int r = val &0xff0000;
	      unsigned int g = val &0x00ff00;
	      unsigned int b = val &0x0000ff;
	      r>>=16;
	      g>>=8;
	      
	      b<<=16;
	      g<<=8;
	      val = a+r+g+b; 

    unsigned int val_a = val&0xff000000;
    unsigned int val_r = val&0x00ff0000;
    unsigned int val_g = val&0x0000ff00;
    unsigned int val_b = val&0x000000ff;
    val_a >>= 24;
    val_r >>= 16;
    val_g >>= 8;
    val_b >>= 0;

    // These are opengl RGBA format
    val_a <<= 24;
    val_b <<= 16;
    val_g <<= 8;
    val_r <<= 0;
    unsigned int v = val_r+val_g+val_b+val_a;

	    
	    buf.buffer[dd+x] = v;
	  }
      }
      return;
    }
#endif

#if 0
  if (t.IsDirectGltfImage())
    { // FAST PATH FOR GLTF IMAGE
      FlipColours *bm1 = (FlipColours*)&t;
      BitmapPrepareCache *cache = (BitmapPrepareCache*)&bm1->c;
      GLTFImage *img=(GLTFImage*)cache->get_bm();
      const tinygltf::Image *img2 = img->img;

      //std::cout << img2->component << " " << img2->bits << std::endl;

      //std::cout << img2->name << std::endl;
      //std::cout << img2->width << " " << img2->height << std::endl;
      //std::cout << img2->name << "::" << img2->width << " " << img2->height << " " << img2->component << " " << img2->bits << " " << img2->pixel_type << std::endl;
      if (img2 && (img2->component==4 ||img2->component==3)&& img2->bits==8) {

	//std::cout << "GLTFFASTPATH" << std::endl;
      
	for(int y=start_y;y<end_y;y++) {
	  int dd = y*buf.ydelta;
	  int offset2 = y*img2->width*img2->component * (img2->bits/8);
	  for(int x=start_x;x<end_x;x++)
	    {
	      int offset = (x*img2->component)*(img2->bits/8);

	      unsigned int val = *(unsigned int*)&img2->image[offset+offset2];
	      
	      unsigned int a = val &0xff000000;
	      unsigned int r = val &0xff0000;
	      unsigned int g = val &0x00ff00;
	      unsigned int b = val &0x0000ff;
	      r>>=16;
	      g>>=8;
	      
	      b<<=16;
	      g<<=8;
	      val = a+r+g+b; 
	      //val = Color(val).Pixel();


    unsigned int val_a = val&0xff000000;
    unsigned int val_r = val&0x00ff0000;
    unsigned int val_g = val&0x0000ff00;
    unsigned int val_b = val&0x000000ff;
    val_a >>= 24;
    val_r >>= 16;
    val_g >>= 8;
    val_b >>= 0;

    // These are opengl RGBA format
    val_a <<= 24;
    val_b <<= 16;
    val_g <<= 8;
    val_r <<= 0;
    unsigned int v = val_r+val_g+val_b+val_a;

	      
	      buf.buffer[dd+x] = v;
	    }
	}
	return;


      }
    
        if (img2 && (img2->component==4 ||img2->component==3)&& img2->bits==16) {
	  std::cout << "GLTFFASTPATH2" << std::endl;

	for(int y=start_y;y<end_y;y++) {
	  int dd = y*buf.ydelta;
	  int offset2 = y*img2->width*img2->component * (img2->bits/8);
	  for(int x=start_x;x<end_x;x++)
	    {
	      int offset = (x*img2->component)*(img2->bits/8);

	      unsigned short *c0 = (unsigned short*)&img2->image[offset+offset2];
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
	      unsigned int val2 = Color(val).Pixel();


	    unsigned int val_a = val2&0xff000000;
    unsigned int val_r = val2&0x00ff0000;
    unsigned int val_g = val2&0x0000ff00;
    unsigned int val_b = val2&0x000000ff;
    val_a >>= 24;
    val_r >>= 16;
    val_g >>= 8;
    val_b >>= 0;

    // These are opengl RGBA format
    val_a <<= 24;
    val_b <<= 16;
    val_g <<= 8;
    val_r <<= 0;
    unsigned int v = val_r+val_g+val_b+val_a;

	
	      buf.buffer[dd+x] = v;
	    
	  }
	}
	return;
	}

	//std::cout << "GLTFSLOWPATH" << std::endl;
  for(int y=start_y;y<end_y;y++) {
    int dd = y*buf.ydelta;
    for(int x=start_x;x<end_x;x++)
      {
	unsigned int color = t.Map(x,y).Pixel();
	buf.buffer[dd+x] = color;
      }
  }
	return;
    }
#endif
  
  //std::cout << "ORDISLOWPATH" << std::endl;
  int dd = start_y*buf.ydelta;
  for(int y=start_y;y<end_y;y++,dd+=buf.ydelta) {
    for(int x=start_x;x<end_x;x++)
      {
	unsigned int color = t.Map(x,y).Pixel();
	buf.buffer[dd+x] = color;
      }
  }
   
}


GameApi::BM gltf_load_bitmap2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int image_index);


#ifdef NO_PREPARE_RENDER

class GLTFVertexArrays : public VertexArrays
{
public:
  GLTFVertexArrays( GLTFModelInterface *interface, int mesh_id, int prim_id ) : interface(interface), mesh_id(mesh_id), prim_id(prim_id) { }
  virtual void Collect(CollectVisitor& vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { interface->Prepare(); }
  void Prepare() { HeavyPrepare(); }
  std::string enabled() const
  {
    std::string res;
    const char *l=0;
    for(int i=0;i<10;i++) {
      switch(i) {
      case 0: l="POSITION"; break;
      case 1: l="NORMAL"; break;
      case 2: l="TEXCOORD_0"; break;
      case 3: l="TEXCOORD_1"; break;
      case 4: l="COLOR_0"; break;
      case 5: l="COLOR_1"; break;
      case 6: l="JOINTS_0"; break;
      case 7: l="JOINTS_1"; break;
      case 8: l="WEIGHTS_0"; break;
      case 9: l="WEIGHTS_1"; break;
      };
      if (!(mesh_id>=0 && mesh_id<interface->meshes_size()))
	{
	  std::cout << "Mesh default to empty" << std::endl;
	  return "";
	}
      const tinygltf::Mesh &mesh = interface->get_mesh( mesh_id );
      tinygltf::Primitive &prim = const_cast<tinygltf::Primitive&>(mesh.primitives[ prim_id ]);
      int position_index = -1;
      if (prim.attributes.find(l) != prim.attributes.end())
	{
	  char ch=' ';
	  std::string st = "pntTcCjJw";
	  res+=st[i];
	}
    }
    return res;
  }
  virtual int Num(VertexArrayEnum i) const
  {
    const char *l;
    switch(i) {
    case 0: l="POSITION"; break;
    case 1: l="NORMAL"; break;
    case 2: l="TEXCOORD_0"; break;
    case 3: l="TEXCOORD_1"; break;
    case 4: l="COLOR_0"; break;
    case 5: l="COLOR_1"; break;
    case 6: l="JOINTS_0"; break;
    case 7: l="JOINTS_1"; break;
    case 8: l="WEIGHTS_0"; break;
    case 9: l="WEIGHTS_1"; break;
    };
    
      Data d = get_data(interface,mesh_id,prim_id,l);
      return d.count;
  }
  virtual int NumIndices() const
  {
      Data d = get_indices_data(interface,mesh_id,prim_id);
      return d.count;
  }
  struct Data {
    size_t offset;
    int componentType;
    int componentSize;
    size_t count;
    int type;
    size_t offset2;
    size_t length;
    size_t stride;
    int target;
    const unsigned char *buf;
  };
  static Data get_indices_data(GLTFModelInterface *interface, int mesh_id, int prim_id)
  {
      if (!(mesh_id>=0 && mesh_id<interface->meshes_size()))
	{
	  std::cout << "get_indices_data::Mesh default to empty"<< mesh_id << std::endl;
	  exit(0);
	}

    const tinygltf::Mesh &mesh = interface->get_mesh( mesh_id );
    tinygltf::Primitive &prim = const_cast<tinygltf::Primitive&>(mesh.primitives[ prim_id ]);
    int indices = prim.indices;

    const tinygltf::Accessor &acc = interface->get_accessor(indices);
    int bufferviewindex = acc.bufferView;
    size_t offset = acc.byteOffset;
    int componentType = acc.componentType;
    //std::cout << "INDICES accessor componenttype=" << print_component_type(componentType) << std::endl;
    size_t count = acc.count;
    int type = acc.type;

    const tinygltf::BufferView &bf = interface->get_bufferview(bufferviewindex);
    int bufferindex = bf.buffer;
    size_t offset2 = bf.byteOffset;
    size_t length = bf.byteLength;
    size_t stride = bf.byteStride;
    int target = bf.target;
    
    const tinygltf::Buffer &b = interface->get_buffer(bufferindex);
    const unsigned char *buf = &b.data[0];
	Data d;
	d.offset = offset;
	d.componentType = componentType;
	d.count = count;
	d.type=type;
	d.offset2 = offset2;
	d.length=length;
	d.stride=stride;
	d.target=target;
	d.buf = buf;	
	return d;

  }

  static std::string print_component_type(int val)
  {
    std::stringstream ss;
    ss << val;
    std::string num=ss.str();
    if (val==TINYGLTF_COMPONENT_TYPE_BYTE) return "BYTE" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) return "UNSIGNED BYTE" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_SHORT) return "SHORT" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) return "UNSIGNED SHORT" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_INT) return "INT" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) return "UNSIGNED INT" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_FLOAT) return "FLOAT" +num;
    if (val==TINYGLTF_COMPONENT_TYPE_DOUBLE) return "DOUBLE" +num;
    return "UNKNOWN" +num;
  }
  static Data get_data(GLTFModelInterface *interface, int mesh_id, int prim_id, const char *label )
  {
      if (!(mesh_id>=0 && mesh_id<interface->meshes_size()))
	{
	  std::cout << "get_indices_data::Mesh default to empty"<< mesh_id << std::endl;
	  exit(0);
	}
    const tinygltf::Mesh &mesh = interface->get_mesh( mesh_id );
    tinygltf::Primitive &prim = const_cast<tinygltf::Primitive&>(mesh.primitives[ prim_id ]);
    int position_index = -1;
    if (prim.attributes.find(label) != prim.attributes.end())
      position_index = prim.attributes[label];
    //std::cout << "Found " << position_index << std::endl;
    if (position_index!=-1)
      {
	const tinygltf::Accessor &acc = interface->get_accessor(position_index);
	int bufferviewindex = acc.bufferView;
	size_t offset = acc.byteOffset;
	int componentType = acc.componentType;
	size_t count = acc.count;
	int type = acc.type;


	//std::cout << bufferviewindex << " " << offset << " " << print_component_type(componentType) << " " << count <<" " << type << std::endl;
	  
	const tinygltf::BufferView &bf = interface->get_bufferview(bufferviewindex);
	int bufferindex = bf.buffer;
	size_t offset2 = bf.byteOffset;
	size_t length = bf.byteLength;
	size_t stride = bf.byteStride;
	int target = bf.target;

	//std::cout << bufferindex << " " << offset2 << " " << length << " " << stride << " " << target << std::endl;
	
	const tinygltf::Buffer &b = interface->get_buffer(bufferindex);
	const unsigned char *buf = &b.data[0];
	
	Data d;
	d.offset = offset;
	d.componentType = componentType;
	d.count = count;
	d.type=type;
	d.offset2 = offset2;
	d.length=length;
	d.stride=stride;
	d.target=target;
	d.buf = buf;
	return d;
      }
    std::cout << "MESH DOESNT HAVE POSITION DATA, exiting.." << std::endl;
    exit(0);
  }
  virtual size_t Stride(VertexArrayEnum i) const
  {
    const char *l = 0;
    switch(i) {
    case 0: l="POSITION"; break;
    case 1: l="NORMAL"; break;
    case 2: l="TEXCOORD_0"; break;
    case 3: l="TEXCOORD_1"; break;
    case 4: l="COLOR_0"; break;
    case 5: l="COLOR_1"; break;
    case 6: l="JOINTS_0"; break;
    case 7: l="JOINTS_1"; break;
    case 8: l="WEIGHTS_0"; break;
    case 9: l="WEIGHTS_1"; break;
    };
    if (l) {
      Data d = get_data(interface,mesh_id,prim_id,l);
      return d.stride;
    }
    return 0;
  }
  virtual unsigned char *Indices() const
  {
    Data d = get_indices_data(interface,mesh_id,prim_id);
    return (unsigned char*)(d.buf + d.offset + d.offset2);
  }
  virtual VA_ComponentType IndicesComponentType() const
  {
      Data d = get_indices_data(interface,mesh_id,prim_id);
      switch(d.componentType) {
      case TINYGLTF_COMPONENT_TYPE_BYTE: return VA_BYTE;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: return VA_UBYTE;
      case TINYGLTF_COMPONENT_TYPE_SHORT: return VA_SHORT;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return VA_USHORT;
      case TINYGLTF_COMPONENT_TYPE_INT: return VA_INT;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: return VA_UINT;
      case TINYGLTF_COMPONENT_TYPE_FLOAT: return VA_FLOAT;
      case TINYGLTF_COMPONENT_TYPE_DOUBLE: return VA_DOUBLE;
      };
    return VA_INVALID;
  }
  virtual int ComponentCount(VertexArrayEnum i) const
  {
        const char *l = 0;
    switch(i) {
    case 0: l="POSITION"; break;
    case 1: l="NORMAL"; break;
    case 2: l="TEXCOORD_0"; break;
    case 3: l="TEXCOORD_1"; break;
    case 4: l="COLOR_0"; break;
    case 5: l="COLOR_1"; break;
    case 6: l="JOINTS_0"; break;
    case 7: l="JOINTS_1"; break;
    case 8: l="WEIGHTS_0"; break;
    case 9: l="WEIGHTS_1"; break;
    };
    if (l) {
      Data d = get_data(interface,mesh_id,prim_id,l);
      switch(d.type) {
      case TINYGLTF_TYPE_VEC2: return 2;
      case TINYGLTF_TYPE_VEC3: return 3;
      case TINYGLTF_TYPE_VEC4: return 4;
      case TINYGLTF_TYPE_SCALAR: return 1;
      default:
	std::cout << "TINYGLTF_TYPE_ error" << std::endl;
      }
    }
  }
  virtual VA_ComponentType ComponentType(VertexArrayEnum i) const
  {
    const char *l = 0;
    switch(i) {
    case 0: l="POSITION"; break;
    case 1: l="NORMAL"; break;
    case 2: l="TEXCOORD_0"; break;
    case 3: l="TEXCOORD_1"; break;
    case 4: l="COLOR_0"; break;
    case 5: l="COLOR_1"; break;
    case 6: l="JOINTS_0"; break;
    case 7: l="JOINTS_1"; break;
    case 8: l="WEIGHTS_0"; break;
    case 9: l="WEIGHTS_1"; break;
    };
    if (l) {
      Data d = get_data(interface,mesh_id,prim_id,l);
      switch(d.componentType) {
      case TINYGLTF_COMPONENT_TYPE_BYTE: return VA_BYTE;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: return VA_UBYTE;
      case TINYGLTF_COMPONENT_TYPE_SHORT: return VA_SHORT;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return VA_USHORT;
      case TINYGLTF_COMPONENT_TYPE_INT: return VA_INT;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: return VA_UINT;
      case TINYGLTF_COMPONENT_TYPE_FLOAT: return VA_FLOAT;
      case TINYGLTF_COMPONENT_TYPE_DOUBLE: return VA_DOUBLE;
      };
    }
    return VA_INVALID;
  }
  virtual void *Attrib(VertexArrayEnum i) const {
    void *data = 0;
    switch(i) {
    case 0: data=Position(); break;
    case 1: data=Normal(); break;
    case 2: data=TexCoord_0(); break;
    case 3: data=TexCoord_1(); break;
    case 4: data=Color_0(); break;
    case 5: data=Color_1(); break;
    case 6: data=Joints_0(); break;
    case 7: data=Joints_1(); break;
    case 8: data=Weights_0(); break;
    case 9: data=Weights_1(); break;
    };
    return data;
  }
private:
  virtual Point *Position() const { // i=0
    Data d = get_data(interface, mesh_id,prim_id, "POSITION");
    return (Point*)(d.buf + d.offset + d.offset2);
  }
  virtual Vector *Normal() const {  // i=1
    Data d = get_data(interface, mesh_id,prim_id, "NORMAL");
    return (Vector*)(d.buf + d.offset + d.offset2);
  }
  virtual Vector *TexCoord_0() const { // i=2
    Data d = get_data(interface, mesh_id,prim_id, "TEXCOORD_0");
    return (Vector*)(d.buf + d.offset + d.offset2);
  }
  virtual Vector *TexCoord_1() const {  // i=3
    Data d = get_data(interface, mesh_id,prim_id, "TEXCOORD_1");
    return (Vector*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Color_0() const { // i=4
    Data d = get_data(interface, mesh_id,prim_id, "COLOR_0");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Color_1() const {   // i=5
    Data d = get_data(interface, mesh_id,prim_id, "COLOR_1");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Joints_0() const {    // i=6
    Data d = get_data(interface, mesh_id,prim_id, "JOINTS_0");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Joints_1() const {    // i=7
    Data d = get_data(interface, mesh_id,prim_id, "JOINTS_1");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Weights_0() const {   // i=8
    Data d = get_data(interface, mesh_id,prim_id, "WEIGHTS_0");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
  virtual VEC4 *Weights_1() const {   // i=9
    Data d = get_data(interface, mesh_id,prim_id, "WEIGHTS_1");
    return (VEC4*)(d.buf + d.offset + d.offset2);
  }
private:
  GLTFModelInterface *interface;
  int mesh_id;
  int prim_id;
};




class PrimRender : public MainLoopItem
{
public:
  PrimRender(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_index, int prim_index, std::vector<GameApi::BM> bm, std::vector<int> types, std::vector<std::string> id_labels) : env(env), ev(ev), gltf(interface,mesh_index,prim_index), dyn(&gltf), interface(interface), mesh_index(mesh_index), prim_index(prim_index),bms(bm),types(types),id_labels(id_labels) { firsttime = true; }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {

    
    
    std::string s = gltf.enabled();
    //std::cout << "ENabled: " << s << std::endl;
    int sz = s.size();
    for(int i=0;i<sz;i++)
      {
	char ch = s[i];
	int ii=0;
	switch(ch) {
	case 'p': ii=0; break;
	case 'n': ii=1; break;
	case 't': ii=2; break;
	case 'T': ii=3; break;
	case 'c': ii=4; break;
	case 'C': ii=5; break;
	case 'j': ii=6; break;
	case 'J': ii=7; break;
	case 'w': ii=8; break;
	case 'W': ii=9; break;
	};
	dyn.prepare(ii);
      }
    dyn.ready();
  }
  virtual void Prepare() { HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
    
    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0||u_v.id==-1)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0||u_f.id==-1)
	  u_f = ev.uber_api.f_empty(true);
      }
#if 0
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      u_v = ev.uber_api.v_texture(u_v);
	    if (e.us_fragment_shader==-1)
	      u_f = ev.uber_api.f_texture(u_f);
	  }
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	      if (firsttime)
	      {
		if (e.us_vertex_shader==-1)
		  u_v = ev.uber_api.v_texture_arr(u_v);
		if (e.us_fragment_shader==-1)
		  u_f = ev.uber_api.f_texture_arr(u_f);
	      }
	  }
      }
    else
      {
	sh.id = e.sh_color;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1)
	      {
		u_v = ev.uber_api.v_colour(u_v);
		//u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		//u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    //std::cout << "RenderInstanced::Execute" << std::endl;
    if (firsttime || shader.id==-1)
      {
	//std::cout << "RenderInstanced::SHADER" << std::endl;
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; 
	fragment.id = u_f.id; 
	GameApi::US vertex2 = vertex;
	shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex2, fragment,e.v_shader_functions, e.f_shader_functions,true);

	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }
    }


    ev.shader_api.use(shader);
    	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	GameApi::M m3 = add_matrix2(env, e.in_P);
	ev.shader_api.use(shader);
	ev.shader_api.set_var(shader, "in_MV", m);
	if (e.has_inverse) {
	  GameApi::M m4 = add_matrix2(env, e.in_iMV);	  
	  ev.shader_api.set_var(shader, "in_iMV", m4);
	} else
	  ev.shader_api.set_var(shader, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(shader, "in_T", m1);
	ev.shader_api.set_var(shader, "in_P", m3);
	ev.shader_api.set_var(shader, "in_N", m2);
	ev.shader_api.set_var(shader, "time", e.time);
	sh = shader;

    
    
    if (firsttime) {
      std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev,bms,types,true,id_labels);
      dyn.set_texture(id);
      firsttime=false;
    }
    dyn.render();
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
private:
  GameApi::Env &env;
  GameApi::SH shader;
  GameApi::SH sh;
  GameApi::EveryApi &ev;
  GLTFVertexArrays gltf;
  Dyn dyn;
  GLTFModelInterface *interface;
  int mesh_index;
  int prim_index;
  std::vector<GameApi::BM> bms;
  std::vector<int> types;
  std::vector<std::string> id_labels;
  bool firsttime;
};

GameApi::ML GameApi::MainLoopApi::prim_render(EveryApi &ev,TF tf, int mesh_index, int prim_index, std::vector<GameApi::BM> bm, std::vector<int> types, std::vector<std::string> id_labels)
{
  GLTFModelInterface *interface = find_gltf(e,tf);
  return add_main_loop(e, new PrimRender(e,ev,interface,mesh_index,prim_index,bm,types,id_labels));

}

GameApi::ML GameApi::MainLoopApi::mesh_render(EveryApi &ev, TF tf, int mesh_index, std::vector<BM> bm, std::vector<int> types, std::vector<std::string> id_labels)
{
  GLTFModelInterface *interface = find_gltf(e,tf);
  interface->Prepare();
      if (!(mesh_index>=0 && mesh_index<interface->meshes_size()))
	{
	  std::cout << "get_indices_data::Mesh default to empty"<< mesh_index << std::endl;
	  exit(0);
	}
  const tinygltf::Mesh &mesh = interface->get_mesh( mesh_index );
  int s = mesh.primitives.size();
  std::vector<GameApi::ML> mls;
  for(int i=0;i<s;i++)
    {
      GameApi::ML ml = prim_render(ev,tf,mesh_index,i,bm,types,id_labels);
      mls.push_back(ml);
    }
  return ev.mainloop_api.array_ml(ev,mls);
}


class GLTF_Material_noP : public MainLoopItem
{
public:
  GLTF_Material_noP(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int mesh_index, int prim_id, float mix) : env(env), ev(ev), tf(tf), mesh_index(mesh_index), prim_id(prim_id), mix(mix)
  {
  }
  void setup()
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
  interface->Prepare();
      if (!(mesh_index>=0 && mesh_index<interface->meshes_size()))
	{
	  std::cout << "get_indices_data::Mesh default to empty"<< mesh_index<< std::endl;
	  exit(0);
	}
    const tinygltf::Mesh &mesh = interface->get_mesh( mesh_index );
    tinygltf::Primitive &prim = const_cast<tinygltf::Primitive&>(mesh.primitives[ prim_id ]);
    material_id = prim.material;
    indices_id = prim.indices;
    mode = prim.mode;
  }

  bool get_unlit() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &unlit= (*m.extensions.find("KHR_materials_unlit")).second;
    return unlit.IsObject();
  }

  bool get_sheen() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &unlit= (*m.extensions.find("KHR_materials_sheen")).second;
    return unlit.IsObject();
  }

  int print_int(std::string label, int i) const { /*std::cout << label << ":" << i << std::endl;*/ return i; }
  float print_float(std::string label, float i) const { /*std::cout << label << ":" << i << stsd::endl;*/ return i; }
  Vector print_vector(std::string label, Vector i) const { /*std::cout << label << ":" << i.dx << " " << i.dy << " " << i.dz << std::endl;*/ return i; }
  
  
  Vector get_diffuse_factor() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_vector("diffuse_factor",Vector(1.0f,1.0f,1.0f));
    const tinygltf::Value &diffuse = specglossi.Get("diffuseFactor");
    if (!diffuse.IsArray()) return print_vector("diffuse_factor",Vector(1.0f,1.0f,1.0f));
    const tinygltf::Value &diff_r = diffuse.Get(0);
    const tinygltf::Value &diff_g = diffuse.Get(1);
    const tinygltf::Value &diff_b = diffuse.Get(2);
    Vector res;
    res.dx = diff_r.GetNumberAsDouble();
    res.dy = diff_g.GetNumberAsDouble();
    res.dz = diff_b.GetNumberAsDouble();
    return print_vector("diffuse_factor",res);
    //return diffuse.GetNumberAsDouble();
  }

  Vector get_specular_factor() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_vector("specular_factor",Vector(0.5f,0.5f,0.5f));
    const tinygltf::Value &diffuse = specglossi.Get("specularFactor");
    if (!diffuse.IsArray()) return print_vector("specular_factor",Vector(0.5f,0.5f,0.5f));
    const tinygltf::Value &diff_r = diffuse.Get(0);
    const tinygltf::Value &diff_g = diffuse.Get(1);
    const tinygltf::Value &diff_b = diffuse.Get(2);
    Vector res;
    res.dx = diff_r.GetNumberAsDouble();
    res.dy = diff_g.GetNumberAsDouble();
    res.dz = diff_b.GetNumberAsDouble();
    return print_vector("specular_factor",res);
  }

  float get_glossiness_factor() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_float("glossinessfactor",1.0f);
    const tinygltf::Value &diffuse = specglossi.Get("glossinessFactor");
    float glos = diffuse.GetNumberAsDouble();
    if (glos<0.04) glos=1.0f;
    return print_float("glossinessfactor",glos);
  }

  
  bool get_spec() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return false;
    return true;
  }

  int get_sheen_index() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_sheen")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &diffuse = specglossi.Get("sheenColorTexture");
    if (!diffuse.IsObject()) return -1;
    const tinygltf::Value &index = diffuse.Get("index");
    return print_int("sheen index",index.GetNumberAsInt());
  }
  
  int get_diffuse_index() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &diffuse = specglossi.Get("diffuseTexture");
    if (!diffuse.IsObject()) return -1;
    const tinygltf::Value &index = diffuse.Get("index");
    return print_int("diffuse_index",index.GetNumberAsInt());
  }
  int get_specglossi_index() const
  {
  GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &s = specglossi.Get("specularGlossinessTexture");
    if (!s.IsObject()) return -1;
    const tinygltf::Value &s2 = s.Get("index");
    return print_int("specglossi index",s2.GetNumberAsInt());
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
  GLTFModelInterface *interface = find_gltf(env,tf);
  //print_extension_map();
    if (material_id<0 || material_id>=int(interface->materials_size())||!has_texture(i)) {
      //std::cout << "Bad material: i=" << i << " material_id=" << material_id << " has_texture(i)=" << has_texture(i) <<std::endl; 
      return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    switch(i) {
    case 0: {
      int index = m.pbrMetallicRoughness.baseColorTexture.index;
      if (get_spec()) index=get_specglossi_index();
      if (get_sheen()) index=get_sheen_index();
      //std::cout << "IMG0=" << index << "(" << get_spec() << "," << get_sheen() << ")" << std::endl;
      return gltf_load_bitmap2(env,ev, interface, index);
    }
    case 1: {
      int index = m.pbrMetallicRoughness.metallicRoughnessTexture.index;
      if (get_spec()) index=get_diffuse_index();
      //std::cout << "IMG1=" << index << "(" << get_spec() << ")" << std::endl;
      return gltf_load_bitmap2(env,ev, interface, index);
    }
    case 2: return gltf_load_bitmap2(env,ev, interface, m.normalTexture.index);
    case 3: return gltf_load_bitmap2(env,ev, interface, m.occlusionTexture.index);
    case 4: return gltf_load_bitmap2(env,ev, interface, m.emissiveTexture.index);
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
  GLTFModelInterface *interface = find_gltf(env,tf);
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      return false;
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    //std::cout << "has_texture " << i << " materia_id=" << material_id << "<" << interface->materials_size() << " " << get_spec() << " " << get_specglossi_index() << " " << get_sheen() << " " << get_sheen_index() << " " << m.pbrMetallicRoughness.baseColorTexture.index <<std::endl;
    switch(i) {
    case 0: return m.pbrMetallicRoughness.baseColorTexture.index!=-1||(get_spec() && get_specglossi_index()!=-1)||(get_sheen() &&get_sheen_index()!=-1);
    case 1: return m.pbrMetallicRoughness.metallicRoughnessTexture.index!=-1||(get_spec()&&get_diffuse_index()!=-1);
    case 2: return m.normalTexture.index!=-1;
    case 3: return m.occlusionTexture.index!=-1;
    case 4: return m.emissiveTexture.index!=-1;
    default: return false;
    };
  }

  virtual void Collect(CollectVisitor &vis) {
    std::cout << "Collect" << std::endl;
  GLTFModelInterface *interface = find_gltf(env,tf);
    interface->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    setup();
    std::cout << "HeavyPrepare" << std::endl;
    GLTFModelInterface *interface = find_gltf(env,tf);
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }

    std::vector<std::string> id_labels;
    for(int i=0;i<s;i++)
      {
	std::stringstream ss;
	ss << interface->Url() << "_" << material_id << "_" << i << std::endl;
	id_labels.push_back(ss.str());
      }


    ml = ev.mainloop_api.prim_render(ev,tf,mesh_index,prim_id,bm,std::vector<int>(),id_labels);

      const tinygltf::Material &m = interface->get_material(material_id);
      std::vector<double> emis=m.emissiveFactor;
      Point emis2= { float(emis[0]),float(emis[1]),float(emis[2]) };
      const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
      const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
      ml=ev.polygon_api.gltf_shader(ev, ml, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),r.baseColorFactor[3], o.strength, 1.0,get_spec(),get_diffuse_factor().dx,get_diffuse_factor().dy,get_diffuse_factor().dz, get_specular_factor().dx,get_specular_factor().dy,get_specular_factor().dz, get_glossiness_factor(), get_unlit(),emis2.x,emis2.y,emis2.z); // todo base color
     

    
    MainLoopItem *item = find_main_loop(env,ml);
    item->Prepare();
  }
  virtual void Prepare() {
    //std::cout << "Prepare" << std::endl;
    HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    //std::cout << "Execute" << std::endl;
    if (ml.id!=-1) {
      //std::cout << "Execute(real)" << std::endl;

      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    //std::cout << "handleevent" << std::endl;
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    //std::cout << "shader_id" << std::endl;
    if (ml.id!=-1) {
    MainLoopItem *item = find_main_loop(env,ml);
    return item->shader_id();
    } else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int mesh_index;
  int material_id;
  int prim_id;
  int indices_id; // TODO
  int mode; // TODO
  float mix;
  GameApi::ML ml = { -1 };
};
GameApi::ML GameApi::MainLoopApi::gltf_material_nop(EveryApi &ev, TF tf, int mesh_index, int prim_id, float mix)
{
   GameApi::ML ml = add_main_loop(e, new GLTF_Material_noP(e,ev,tf,mesh_index, prim_id, mix));
   return ml;
}

void resize_reset();
void calc_resize_model(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p);
GameApi::ML resize_model(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::ML ml);




GameApi::ML GameApi::MainLoopApi::gltf_material_mesh(EveryApi &ev, TF tf, int mesh_index, float mix)
{

  GLTFModelInterface *interface = find_gltf(e,tf);
  interface->Prepare();
      if (!(mesh_index>=0 && mesh_index<interface->meshes_size()))
	{
	  std::cout << "get_indices_data::Mesh default to empty"<< mesh_index << std::endl;
	  exit(0);
	}
  const tinygltf::Mesh &mesh = interface->get_mesh( mesh_index );
  int s = mesh.primitives.size();
  std::vector<GameApi::ML> mls;
  for(int i=0;i<s;i++)
    {
      GameApi::ML ml = gltf_material_nop(ev,tf,mesh_index,i,mix);
      mls.push_back(ml);
    }
  return ev.mainloop_api.array_ml(ev,mls);
}




class GLTF_SKIN;

GLTF_SKIN *gltf_skin(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::TF tf, int skin_id, float mix);

class GLTF_Node_impl : public MainLoopItem
{
public:
  GLTF_Node_impl(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int node_index, float mix) : env(env), ev(ev), tf(tf), node_index(node_index)
  {
  }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare();

  virtual void Prepare() { HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env,res);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e) {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env,res);
      item->handle_event(e);
    }

  }
  virtual std::vector<int> shader_id() {
    if (res.id!=-1) {
      MainLoopItem *item = find_main_loop(env,res);
      item->shader_id();
    } else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int node_index;
  float mix;
  std::vector<GameApi::ML> children;
  GameApi::ML mesh = { -1 };
  GLTF_SKIN *skin=0;
  GameApi::ML skin_root = { -1 };
  GameApi::ML res = { -1 };
};

GameApi::ML GameApi::MainLoopApi::gltf_node2(EveryApi &ev, TF tf, int node_id, float mix)
{
  return add_main_loop(e, new GLTF_Node_impl(e,ev,tf,node_id,mix));
}

class GLTF_Scene : public MainLoopItem
{
public:
  GLTF_Scene(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int scene_id, float mix) : env(env), ev(ev), tf(tf), scene_id(scene_id), mix(mix) { }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare()
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    bool use_default=false;
    if (scene_id==-1) use_default=true;
    int s2 = interface->scenes_size(); //load->model.scenes.size();
    if (!(scene_id>=0 && scene_id<s2)) {
      use_default=true;
    }
    std::vector<GameApi::ML> mls;
    if (!use_default) {
      const tinygltf::Scene &scene = interface->get_scene( scene_id );
      std::vector<int> vec = scene.nodes;
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	  mls.push_back(ev.mainloop_api.gltf_node2(ev,tf,vec[i],mix));
	}
    } else {
      std::cout << "Scene defaulting to node 0" << std::endl;
      // default to node 0 
      mls.push_back(ev.mainloop_api.gltf_node2(ev,tf,0,mix));
    }
    ml = ev.mainloop_api.array_ml(ev,mls);
    MainLoopItem *item = find_main_loop(env,ml);
    item->Prepare();
  }
  virtual void Prepare() { HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (ml.id!=-1) {
    MainLoopItem *item = find_main_loop(env,ml);
    item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (ml.id!=-1) {
    MainLoopItem *item = find_main_loop(env,ml);
    item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
    MainLoopItem *item = find_main_loop(env,ml);
    return item->shader_id();
    }
    return std::vector<int>();
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int scene_id;
  float mix;
  GameApi::ML ml = { -1 };
};

GameApi::ML GameApi::MainLoopApi::gltf_scene2(EveryApi &ev, TF tf, int scene_id, float mix)
{
  return add_main_loop(e, new GLTF_Scene(e,ev,tf,scene_id,mix));
}

class GLTF_Buffer
{
public:
  GLTF_Buffer(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int buf) : env(env), ev(ev), tf(tf), buf(buf) { }
  ~GLTF_Buffer() { }
  const std::vector<unsigned char,GameApiAllocator<unsigned char> > &get_data() const {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Buffer &buffer = interface->get_buffer( buf );
    return buffer.data;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int buf;
};

class GLTF_BufferView
{
public:
  GLTF_BufferView(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int bv) : env(env), ev(ev), tf(tf), bv(bv) { }
  ~GLTF_BufferView() { delete m_buf; }
  const unsigned char *buf() const {
    gen();
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::BufferView &bufferview = interface->get_bufferview( bv );
    const std::vector<unsigned char, GameApiAllocator<unsigned char> > &data = m_buf->get_data();
    const unsigned char *ptr = &data[0];
    ptr+=bufferview.byteOffset;
    return ptr;
  }
  size_t stride() const {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::BufferView &bufferview = interface->get_bufferview( bv );
    return bufferview.byteStride;
  }
  size_t size() const {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::BufferView &bufferview = interface->get_bufferview( bv );
    return bufferview.byteLength;
  }
  int target() const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::BufferView &bufferview = interface->get_bufferview( bv );
    return bufferview.target;
  }
private:
  void gen() const {
    if (!m_buf) {
      GLTFModelInterface *interface = find_gltf(env,tf);
      const tinygltf::BufferView &bufferview = interface->get_bufferview( bv );
      int buf = bufferview.buffer;
      m_buf = new GLTF_Buffer(env,ev,tf,buf);
    }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int bv;
  mutable GLTF_Buffer *m_buf=0;
};

class GLTF_Accessor
{
public:
  GLTF_Accessor(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int acc) : env(env), ev(ev), tf(tf), acc(acc) { }
  ~GLTF_Accessor() { delete m_bv; }
  int type() const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    return accessor.type;
  }
  int size() const {
    size_t sz=4;
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    switch(accessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE: sz=1; break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: sz=1; break;
    case TINYGLTF_COMPONENT_TYPE_SHORT: sz=2; break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: sz=2; break;
    case TINYGLTF_COMPONENT_TYPE_INT: sz=4; break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: sz=4; break;
    case TINYGLTF_COMPONENT_TYPE_FLOAT: sz=4; break;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE: sz=8; break;
    }
    size_t sz2 = m_bv->size();
    assert(sz2/sz==accessor.count);
    return accessor.count;
  }
  void map(int index, char **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_BYTE);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  char *ptr = (char*)buf;
	  *output = ptr+index;
	} else {
	  char *ptr = (char*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf float accessor" << acc<< std::endl;
  }
  void map(int index, unsigned char **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  unsigned char *ptr = (unsigned char*)buf;
	  *output = ptr+index;
	} else {
	  unsigned char *ptr = (unsigned char*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf float accessor" << acc<< std::endl;

  }
  void map(int index, short **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_SHORT);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  short *ptr = (short*)buf;
	  *output = ptr+index;
	} else {
	  short *ptr = (short*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf float accessor" << acc<< std::endl;
  }
  void map(int index, unsigned short **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  unsigned short *ptr = (unsigned short*)buf;
	  *output = ptr+index;
	} else {
	  unsigned short *ptr = (unsigned short*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf unsigned short accessor" << acc<< std::endl;
  }
  void map(int index, int **output) const
  {
        GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_INT);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  int *ptr = (int*)buf;
	  *output = ptr+index;
	} else {
	  int *ptr = (int*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf int accessor" << acc<< std::endl;

  }
  void map(int index, unsigned int **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  unsigned int *ptr = (unsigned int*)buf;
	  *output = ptr+index;
	} else {
	  unsigned int *ptr = (unsigned int*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf unsigned int accessor" << acc<< std::endl;

  }
  void map(int index, float **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    int target = m_bv->target();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  float *ptr = (float*)buf;
	  *output = ptr+index;
	} else {
	  float *ptr = (float*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf float accessor" << acc<< std::endl;
  }
  void map(int index, double **output) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Accessor &accessor = interface->get_accessor( acc );
    
    gen();
    const unsigned char *buf = m_bv->buf();
    size_t stride = m_bv->stride();
    size_t sz = m_bv->size();
    buf += accessor.byteOffset;
    assert(accessor.componentType==TINYGLTF_COMPONENT_TYPE_DOUBLE);
    size_t count = accessor.count;
    if (index>=0 && index<count)
      {
	if (stride==0) {
	  double *ptr = (double*)buf;
	  *output = ptr+index;
	} else {
	  double *ptr = (double*)(buf + stride*index);
	  *output = ptr;
	}
      }
    else std::cout << "ERROR in gltf float accessor" << acc<< std::endl;
  }

  int target() const
  {
    gen();
    int t = m_bv->target();
    return t; 
  }
  void gen() const {
    if (!m_bv) {
      GLTFModelInterface *interface = find_gltf(env,tf);
      const tinygltf::Accessor &accessor = interface->get_accessor( acc );
      int bv = accessor.bufferView;
      m_bv = new GLTF_BufferView(env,ev,tf,bv);
    }
  }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int acc;
  mutable GLTF_BufferView *m_bv=0;
};

class GLTF_SKIN
{
public:
  GLTF_SKIN(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int skin_id, float mix) : env(env), ev(ev), tf(tf), skin_id(skin_id), mix(mix) { }
  ~GLTF_SKIN() { delete bind_acc; }
  Matrix inverse_bind(int joint) const {
    gen();
    float *ptr;
    bind_acc->map(joint,&ptr);
    Matrix res;
    for(int i=0;i<4;i++)
      for(int j=0;j<4;j++)
	res.matrix[i+j*4] = ptr[j+i*4];
    return res;
  }
  int root_node() const {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Skin &skin = interface->get_skin( skin_id );
    return skin.skeleton;
  }

  int joint_num(int node) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Skin &skin = interface->get_skin( skin_id );
    int s = skin.joints.size();
    for(int i=0;i<s;i++) if (skin.joints[i]==node) return i;
    return -1;
  }
  bool is_joint(int node) const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Skin &skin = interface->get_skin( skin_id );
    int s = skin.joints.size();
    for(int i=0;i<s;i++) if (skin.joints[i]==node) return true;
    return false;
  }
  void gen() const
  {
    if (!bind_acc)
      {
	GLTFModelInterface *interface = find_gltf(env,tf);
	const tinygltf::Skin &skin = interface->get_skin( skin_id );
	bind_acc=new GLTF_Accessor(env,ev,tf,skin.inverseBindMatrices);
      }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int skin_id;
  float mix;
  mutable GLTF_Accessor *bind_acc=0;
};
GLTF_SKIN *gltf_skin(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::TF tf, int skin_id, float mix)
{
  return new GLTF_SKIN(e,ev,tf,skin_id, mix);
}


  void GLTF_Node_impl::HeavyPrepare()
  {
    if (node_index==-1) return;
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Node *node; // = &interface->get_node( node_index );
    std::vector<GameApi::ML> mls;
    bool use_default=false;
    if (!(node_index>=0 && node_index<interface->nodes_size()))
      {
	std::cout << "Node defaulting to mesh 0" << std::endl;
	use_default=true;
      }
    if (!use_default) {
      node = &interface->get_node( node_index );
      const std::vector<int> &child = node->children;
    int s = child.size();
    for(int i=0;i<s;i++)
      {
	children.push_back(ev.mainloop_api.gltf_node2(ev,tf,child[i],mix));
      }
    if (node->mesh!=-1)
      mesh = ev.mainloop_api.gltf_material_mesh(ev,tf,node->mesh,mix);
    if (node->skin!=-1)
      skin = gltf_skin(env,ev,tf,node->skin,mix);
    if (skin) {
      int skin_root_id = skin->root_node();
      skin_root = ev.mainloop_api.gltf_node2(ev,tf,skin_root_id, mix);
    }
    mls = children;
    if (mesh.id!=-1)
      mls.push_back(mesh);
    } else
      {
	// default to mesh 0
	mesh = ev.mainloop_api.gltf_material_mesh(ev,tf,0,mix);
	mls.push_back(mesh);
      }

    GameApi::MN mv = ev.move_api.mn_empty();
    if (!use_default) {
  if (int(node->scale.size())==3) {
    double s_x = node->scale[0];
    double s_y = node->scale[1];
    double s_z = node->scale[2];
    mv = ev.move_api.scale2(mv, s_x, s_y, s_z);
    std::cout << "sc[" << s_x << "," << s_y << "," << s_z << "]";
  }
  if (int(node->rotation.size())==4) {
    double r_x = node->rotation[0];
    double r_y = node->rotation[1];
    double r_z = node->rotation[2];
    double r_w = node->rotation[3];
    //std::cout << "rot[" << r_x << "," << r_y << "," << r_z << "," << r_w << "]";
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
    //std::cout << "tr[" << m_x << "," << m_y << "," << m_z << "]";
  }
  //std::cout << node->matrix.size();
  if (int(node->matrix.size())==16) {
    const double *arr = &node->matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];
      //std::cout << "mat[]";

      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
    Movement *orig = find_move(env, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(env, mv2);    
  }

  // SKINNING

  if (skin && skin->is_joint(node_index)) {
    int joint = skin->joint_num(node_index);
    Matrix m = skin->inverse_bind(joint);
    
      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
    Movement *orig = find_move(env, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(env, mv2);    
  }
  // END OF SKINNING
    }

  GameApi::ML ml = ev.mainloop_api.array_ml(ev,mls);
  res = ev.move_api.move_ml(ev,ml,mv,1,10.0);
  MainLoopItem *item = find_main_loop(env,res);
  item->Prepare();
  }

class GLTF_Animation_Sampler
{
public:
  GLTF_Animation_Sampler(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int anim, int sampler) : env(env), ev(ev), tf(tf), m_anim(anim), m_sampler(sampler) {
    
  }
  ~GLTF_Animation_Sampler() { delete inp_acc; delete out_acc; }
  int count() const
  {
    int count = 0;
    int type = out_acc->type();
    switch(type)
      {
      case TINYGLTF_TYPE_SCALAR: count=1; break;
      case TINYGLTF_TYPE_VEC2: count=2; break;
      case TINYGLTF_TYPE_VEC3: count=3; break;
      case TINYGLTF_TYPE_VEC4: count=4; break;
      case TINYGLTF_TYPE_MAT2: count=4; break;
      case TINYGLTF_TYPE_MAT3: count=9; break;
      case TINYGLTF_TYPE_MAT4: count=16; break;
      };
    return count;
  }
  float *map(float in) const
  {
    gen();
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Animation &anims = interface->get_animation( m_anim );
    const tinygltf::AnimationSampler &sampler = anims.samplers[ m_sampler ];
    if (sampler.interpolation=="LINEAR")
      {
	int sz = std::min(inp_acc->size(),out_acc->size());
	for(int i=0;i<sz-1;i++)
	  {
	    float *ptr=0;
	    inp_acc->map(i,&ptr);
	    float val=*ptr;
	    inp_acc->map(i+1,&ptr);
	    float val2=*ptr;
	    float x = ((in-val)/(val2-val));
	    if (in>=val && in<val2) {
	      int type = out_acc->type();
	      int count = 0;
	      switch(type)
		{
		case TINYGLTF_TYPE_SCALAR: count=1; break;
		case TINYGLTF_TYPE_VEC2: count=2; break;
		case TINYGLTF_TYPE_VEC3: count=3; break;
		case TINYGLTF_TYPE_VEC4: count=4; break;
		case TINYGLTF_TYPE_MAT2: count=4; break;
		case TINYGLTF_TYPE_MAT3: count=9; break;
		case TINYGLTF_TYPE_MAT4: count=16; break;
		};
	      float *ptr=0;
	      out_acc->map(i,&ptr);
	      float *ptr2=0;
	      out_acc->map(i+1,&ptr2);
	      for(int i=0;i<count;i++) {
		float out =ptr[i];
		float out2 = ptr2[i];
		tmp[i]=(1.0-x)*out+x*out2;
	      }
	      return &tmp[0];
	    }
	  }
	return 0;
      }
    if (sampler.interpolation=="STEP")
      {
	int sz = std::min(inp_acc->size(),out_acc->size());
	for(int i=0;i<sz-1;i++)
	  {
	    float *ptr=0;
	    inp_acc->map(i,&ptr);
	    float val=*ptr;
	    inp_acc->map(i+1,&ptr);
	    float val2=*ptr;
	    if (in>=val && in<val2) {
	      float x = ((in-val)/(val2-val));
	      if (x<0.5) {
		float *ptr=0;
		out_acc->map(i,&ptr);
		return ptr;
	      } else {
		float *ptr=0;
		out_acc->map(i+1,&ptr);
		return ptr;
	      }
	    }
	  }
	return 0;
      }
    if (sampler.interpolation=="CUBICSPLINE")
      { // TODO
	std::cout << "NO cubicspline animation sampler implemented" << std::endl;
      }
    
  }
  void gen() const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Animation &anims = interface->get_animation( m_anim );
    const tinygltf::AnimationSampler &sampler = anims.samplers[ m_sampler ];
    int input = sampler.input;
    int output = sampler.output;
    if (!inp_acc) {
      inp_acc = new GLTF_Accessor(env,ev,tf,input);
    }
    if (!out_acc) {
      out_acc = new GLTF_Accessor(env,ev,tf,output);
    }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int m_anim;
  int m_sampler;
  mutable GLTF_Accessor *inp_acc=0;
  mutable GLTF_Accessor *out_acc=0;
  mutable float tmp[16];
};

class GLTF_AnimChannel
{
public:
  GLTF_AnimChannel(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int anim, int anim_chan) : env(env),ev(ev), tf(tf), anim(anim), anim_chan(anim_chan) { }
  std::string type() const { return m_target_path; }
  int node() const { return m_node; }
  void push_to_node(float time)
  {
    gen();
    float *val = m_samp->map(time);
    int node_id = m_node;
    if (m_target_path=="translation")
      {
	m_m_x=val[0];
	m_m_y=val[1];
	m_m_z=val[2];
      }
    if (m_target_path=="rotation")
      {
	m_r_x = val[0];
	m_r_y = val[1];
	m_r_z = val[2];
	m_r_w = val[3];
      }
    if (m_target_path=="scale")
      {
	//std::cout << "SCALE:" << val[0] << " " << val[1] << " " << val[2] << std::endl;
	m_s_x = val[0];
	m_s_y = val[1];
	m_s_z = val[2];
      }
    if (m_target_path=="weights")
      {
	m_w1=val[0];
	m_w2=val[1];
	m_w3=val[2];
	m_w4=val[3];
      }
  }
  void get_scale(double *s_x, double *s_y, double *s_z) { *s_x = m_s_x; *s_y=m_s_y; *s_z=m_s_z; }
  void get_rotation(double *r_x, double *r_y, double *r_z, double *r_w)
  {
    *r_x = m_r_x; *r_y=m_r_y; *r_z = m_r_z; *r_w=m_r_w;
  }
  void get_tranlation(double *m_x, double *m_y, double *m_z)
  {
    *m_x = m_m_x;
    *m_y = m_m_y;
    *m_z = m_m_z;
  }
  void get_weights(double *w1, double *w2, double *w3, double *w4)
  {
    *w1 = m_w1;
    *w2 = m_w2;
    *w3 = m_w3;
    *w4 = m_w4;
  }
  void gen() const
  {
    GLTFModelInterface *interface = find_gltf(env,tf);
    const tinygltf::Animation &anims = interface->get_animation( anim );
    const tinygltf::AnimationChannel &chan = anims.channels[ anim_chan ];

    int sampler = chan.sampler;
    m_node = chan.target_node;
    m_target_path = chan.target_path;
    if (!m_samp) {
      m_samp = new GLTF_Animation_Sampler(env,ev,tf,anim,sampler);
    }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::TF tf;
  int anim;
  int anim_chan;
  mutable GLTF_Animation_Sampler *m_samp;
  mutable int m_node;
  mutable std::string m_target_path;
  double m_s_x; // scale
  double m_s_y;
  double m_s_z;
  double m_r_x; // rotation
  double m_r_y;
  double m_r_z;
  double m_r_w;
  double m_m_x; // translation
  double m_m_y;
  double m_m_z;
  double m_w1,m_w2,m_w3,m_w4; // weights
};

GLTF_AnimChannel *anim_channel(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::TF tf, int anim, int anim_channel_id)
{
  return new GLTF_AnimChannel(env,ev,tf,anim, anim_channel_id);
}

#endif // NO_PREPARE_RENDER

unsigned long g_glb_file_size=0;
unsigned long g_zip_file_size=0;
bool g_glb_animated=false;


class GLTFFaceCollection : public FaceCollection
{
public:
  GLTFFaceCollection( GLTFModelInterface *interface, int mesh_index, int prim_index) : interface(interface), mesh_index(mesh_index), prim_index(prim_index) {
  }
  virtual std::string name() const { return "GLTFFaceCollection"; }
  ~GLTFFaceCollection()
  {
    int s1 = m_p1.size(); for(int i=0;i<s1;i++)
			    delete [] m_p1[i];
    int s2 = m_p2.size(); for(int i=0;i<s2;i++)
			    delete [] m_p2[i];
    int s3 = m_p3.size(); for(int i=0;i<s3;i++)
			    delete [] m_p3[i];
  }
  void Collect(CollectVisitor &vis)
  {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (mesh_index>=0 && mesh_index<int(interface->meshes_size()) && prim_index>=0 && prim_index<int(interface->get_mesh(mesh_index).primitives.size())) {
      const tinygltf::Mesh &m = interface->get_mesh(mesh_index);
      prim = m.primitives[prim_index];
    }
    else { std::cout << "Prim failed!" << std::endl; return; }

    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
    //model = interface;
    mode = prim.mode;

    //int material = prim->material;
    //tinygltf::Material *mat = 0;
    //if (material!=-1)
    //  mat = &load->model.materials[material];


    // find indices
    indices_index = prim.indices;
    position_index = -1;
    normal_index = -1;
    texcoord_index = -1;
    color_index = -1;
    joints_index = -1;
    weights_index = -1;
    if (prim.attributes.find("POSITION") != prim.attributes.end())
      position_index = prim.attributes["POSITION"];
    if (prim.attributes.find("NORMAL") != prim.attributes.end())
      normal_index = prim.attributes["NORMAL"];
    if (prim.attributes.find("TEXCOORD_0") != prim.attributes.end())
      texcoord_index = prim.attributes["TEXCOORD_0"];
    if (prim.attributes.find("COLOR_0") != prim.attributes.end())
      color_index = prim.attributes["COLOR_0"];
    if (prim.attributes.find("JOINTS_0") != prim.attributes.end())
      joints_index = prim.attributes["JOINTS_0"];
    if (prim.attributes.find("WEIGHTS_0") != prim.attributes.end())
      weights_index = prim.attributes["WEIGHTS_0"];

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
    //indices_acc = 0;
    indices_done = false;
    if (indices_index!=-1) {
      indices_acc = &interface->get_accessor(indices_index); //&model->accessors[indices_index];
      indices_done = true;
    }
      
    if (indices_done) {
      assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);
    }
    //std::cout << "gltf component type: " << indices_acc->componentType << std::endl;
    //assert(indices_acc->componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);


    //position_acc = 0;
    position_done = false;
    if (position_index!=-1) {
      position_acc = &interface->get_accessor(position_index); //&model->accessors[position_index];
      position_done = true;
    }
      
    //normal_acc = 0;
    normal_done = false;
    if (normal_index!=-1) {
      normal_acc = &interface->get_accessor(normal_index); //&model->accessors[normal_index];
      normal_done = true;
    }
      
    //texcoord_acc = 0;
    texcoord_done = false;
    if (texcoord_index!=-1) {
      texcoord_acc = &interface->get_accessor(texcoord_index); //&model->accessors[texcoord_index];
      texcoord_done = true;
    }
      
    //color_acc = 0;
    color_done = false; 
    if (color_index!=-1) {
      color_acc = &interface->get_accessor(color_index); //&model->accessors[color_index];
      color_done = true;
    }
      
    //joints_acc = 0;
    joints_done = false;
    if (joints_index!=-1) {
      joints_acc = &interface->get_accessor(joints_index); //&model->accessors[joints_index];
      joints_done = true;
    }

    //weights_acc = 0;
    weights_done = false;
    if (weights_index!=-1) {
      weights_acc = &interface->get_accessor(weights_index); //&model->accessors[weights_index];
      weights_done = true;
    }
      
    
    
    // find BufferViews
    //indices_bv = 0;
    indices_bv_done = false;
    if (indices_done) {
      int view = indices_acc->bufferView;
      if (view!=-1) {
	indices_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	indices_bv_done = true;
      }
    }

    //position_bv = 0;
    position_bv_done = false;
    if (position_done) {
      int view = position_acc->bufferView;
      if (view!=-1) {
	position_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	position_bv_done = true;
      }
    }

    //normal_bv = 0;
    normal_bv_done = false;
    if (normal_done) {
      int view = normal_acc->bufferView;
      if (view!=-1) {
	normal_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	normal_bv_done = true;
      }
    }

    //texcoord_bv = 0;
    texcoord_bv_done = false;
    if (texcoord_done) {
      int view = texcoord_acc->bufferView;
      if (view!=-1) {
	texcoord_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	texcoord_bv_done = true;
      }
    }

    //color_bv = 0;
    color_bv_done = false;
    if (color_done) {
      int view = color_acc->bufferView;
      if (view!=-1) {
	color_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	color_bv_done = true;
      }
    }

    //joints_bv = 0;
    joints_bv_done = false;
    if (joints_done) {
      int view = joints_acc->bufferView;
      if (view!=-1) {
	joints_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	joints_bv_done = true;
      }
    }

    //weights_bv = 0;
    weights_bv_done = false;
    if (weights_done) {
      int view = weights_acc->bufferView;
      if (view!=-1) {
	weights_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	weights_bv_done = true;
      }
    }
    
    // find buffers
    //indices_buf = 0;
    indices_buf_done = false;
    if (indices_bv_done) {
      int buf = indices_bv->buffer;
      if (buf!=-1) {
	indices_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	indices_buf_done = true;
      }
    }

    //position_buf = 0;
    position_buf_done = false;
    if (position_bv_done) {
      int buf = position_bv->buffer;
      if (buf!=-1) {
	position_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	position_buf_done = true;
      }
    }

    // normal_buf = 0;
    normal_buf_done = false;
    if (normal_bv_done) {
      int buf = normal_bv->buffer;
      if (buf!=-1) {
	normal_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	normal_buf_done = true;
      }
    }

    //texcoord_buf = 0;
    texcoord_buf_done = false;
    if (texcoord_bv_done) {
      int buf = texcoord_bv->buffer;
      if (buf!=-1) {
	texcoord_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	texcoord_buf_done = true;
      }
    }

    //color_buf = 0;
    color_buf_done = false;
    if (color_bv_done) {
      int buf = color_bv->buffer;
      if (buf!=-1) {
	color_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	color_buf_done = false;
      }
    }

    //joints_buf = 0;
    joints_buf_done = false;
    if (joints_bv_done) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	joints_buf_done = true;
      }
    }

    //weights_buf = 0;
    weights_buf_done = false;
    if (weights_bv_done) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	weights_buf_done = true;
      }
    }

  }
  
  virtual void Prepare() { 
    //std::cout << "GLTFFaceCollection::Prepare()" << std::endl;
    interface->Prepare();
    HeavyPrepare();
  }
  virtual int NumFaces() const 
  {
    int res=0;
    if (mode==TINYGLTF_MODE_TRIANGLES && indices_done) {
      //std::cout << "NumFaces(0):" << indices_acc->count/3 << std::endl;
      res=indices_acc->count/3;      
    }
    if (res==0 && mode==TINYGLTF_MODE_TRIANGLE_STRIP && indices_done) {
      //std::cout << "NumFaces(1):" << indices_acc->count-2 << std::endl;
      res=indices_acc->count-2;
    }
    if (res==0 && mode==TINYGLTF_MODE_TRIANGLE_FAN) {
      //std::cout << "NumFaces(3):" << 1 << std::endl;
      res= 1;
    }
    if (res==0 && mode==TINYGLTF_MODE_TRIANGLES && position_done) {
      //std::cout << "NumFaces(4):" << position_acc->count/3 << std::endl;
      res=position_acc->count/3;
    }
    if (res==0 && mode==TINYGLTF_MODE_TRIANGLE_STRIP && position_done) {
      //std::cout << "NumFaces(5):" << position_acc->count-2 << std::endl;
      res=position_acc->count-2;
    }
    if (res>20000000) { std::cout << "ERROR, numfaces too large" << res << std::endl; res=0; }
    if (res<0) { std::cout << "ERROR, numfaces too small" << res << std::endl; res=0; }
    //std::cout << "TINYGLTF mode wrong in NumFaces() " << mode << std::endl;
    //std::cout << "NumFaces(6):" << res << std::endl;
    return res;
  }
  virtual int NumPoints(int face) const { 
    if (mode==TINYGLTF_MODE_TRIANGLE_FAN && indices_done) {
      std::cout << "indices_acc->count==" << indices_acc->count << std::endl;
      return 3;
      //return indices_acc->count;
    }
    return 3;
  }
  int get_index(int face, int point) const
  {
	const unsigned char *ptr = &indices_buf->data[0];
	//int size1 = indices_buf->data.size();
	const unsigned char *ptr2 = ptr + indices_bv->byteOffset;
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
	const unsigned char *ptr3 = ptr2 + indices_acc->byteOffset + comp*stride;
	//int size3 = indices_acc->count;
	int index = 0;
	switch(indices_acc->componentType)
	  {
	  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	    {
	      const unsigned char *ptr4 = (const unsigned char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	  case TINYGLTF_COMPONENT_TYPE_BYTE:
	    {
	      const char *ptr4 = (const char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  {
	  const unsigned short *ptr4 = (const unsigned short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  {
	  const short *ptr4 = (const short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_INT:
	  {
	  const int *ptr4 = (const int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  {
	  const unsigned int *ptr4 = (const unsigned int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	  default:
	    std::cout << "indices_acc->componentType wrong" << indices_acc->componentType << std::endl;
	    return 0;
	  };
	return index;
  }
  BBOX GetBoundingBox(bool &success) const
  {
    if (!position_acc) { success=false; BBOX b; return b; }
    
   const std::vector<double> &m_min = position_acc->minValues;
   const std::vector<double> &m_max = position_acc->maxValues;

    if (m_min.size()!=3) { success=false;  BBOX b; return b; }
    if (m_max.size()!=3) { success=false;  BBOX b; return b; }

    success=true;
    BBOX bb;
    bb.start_x = m_min[0];
    bb.end_x = m_max[0];
    bb.start_y = m_min[1];
    bb.end_y = m_max[1];
    bb.start_z = m_min[2];
    bb.end_z = m_max[2];
    return bb;
  }
  virtual Point FacePoint(int face, int point) const
  {

    if (position_bv_done && position_done && position_buf_done) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &position_buf->data[0];
	int stride2 = position_bv->byteStride;
	if (stride2==0) stride2 = 3*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + position_bv->byteOffset + index*stride2 + position_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	switch(position_acc->componentType) {
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_INT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
	  return Point(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:
	  {
	  const double *pos_ptr4 = (const double*)pos_ptr2; // 3 = num of components in (x,y,z)
	  return Point(pos_ptr4[0],pos_ptr4[1],pos_ptr4[2]);
	  }
	default:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	};
	return Point(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// todo, check that this branch works
	const unsigned char *pos_ptr = &position_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + position_bv->byteOffset;
	int stride = position_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + position_acc->byteOffset + comp*stride;
	const float *pos_ptr4 = (const float*)pos_ptr3; // 3 = num of components in (x,y,z)
	switch(position_acc->componentType) {
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_INT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
	  return Point(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:
	  {
	  const double *pos_ptr4 = (const double*)pos_ptr3; // 3 = num of components in (x,y,z)
	  return Point(pos_ptr4[0],pos_ptr4[1],pos_ptr4[2]);
	  }
	default:
	  std::cout << "ERROR at position_acc->componentType" << position_acc->componentType << std::endl;
	  return Point(0.0,0.0,0.0);
	};
	return Point(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
      }
    }
    } else {
      std::cout << "GLTFFaceCollection: FacePoint:" << position_bv_done << " " << position_done << " " << position_buf_done << " " << mode << " " << TINYGLTF_MODE_TRIANGLES << " " << indices_buf_done << " " << indices_bv_done << " " << indices_done << std::endl;
    return Point(0.0,0.0,0.0);
    }
    std::cout << "gltf FacePoint unknown mode" << std::endl;
    return Point(0.0,0.0,0.0);
  }
  bool has_normal() const { return true; /*normal_bv_done && normal_done && normal_buf_done;*/ }
  virtual Vector PointNormal(int face, int point) const {
    if (normal_bv_done && normal_done && normal_buf_done) {

    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &normal_buf->data[0];
	int stride2 = normal_bv->byteStride;
	if (stride2==0) stride2 = 3*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + normal_bv->byteOffset + index*stride2 + normal_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return -Vector(pos_ptr2[0], pos_ptr2[1], pos_ptr2[2]);
      } else {
	// TODO, check that this branch works
	const unsigned char *pos_ptr = &normal_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + normal_bv->byteOffset;
	int stride = normal_bv->byteStride;
	if (stride==0) stride=3*sizeof(float);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + normal_acc->byteOffset + comp*stride;
	const float *pos_ptr4 = (const float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return -Vector(pos_ptr4[0], pos_ptr4[1], pos_ptr4[2]);
      }
    }
    } else {
      if (store_face==face) return store_res;
    
      Point p1 = FacePoint(face, 0);
      Point p2 = FacePoint(face, 1);
      Point p3 = FacePoint(face, 2);
      Vector v = Vector::CrossProduct(p2-p1,p3-p1);
      Vector res = v / v.Dist();
      store_face = face;
      store_res = res;
      return -res;
    
      
      //return Vector(0.0,0.0,0.0);
    }
    std::cout << "gltf PointNormal unknown mode" << std::endl;

      if (store_face==face) return store_res;
    
      Point p1 = FacePoint(face, 0);
      Point p2 = FacePoint(face, 1);
      Point p3 = FacePoint(face, 2);
      Vector v = Vector::CrossProduct(p2-p1,p3-p1);
      Vector res = v / v.Dist();
      store_face = face;
      store_res = res;
      return -res;

      // return Vector(0.0,0.0,0.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return true; /*color_bv && color_acc && color_buf*/; }
  virtual unsigned int Color(int face, int point) const {
    if (color_bv_done && color_done && color_buf_done) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &color_buf->data[0];
	int stride2 = color_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	unsigned int *pos_ptr2 = (unsigned int*)(pos_ptr + color_bv->byteOffset + index*stride2 + color_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return *pos_ptr2;
      } else {
	// todo, check that this branch works
	const unsigned char *pos_ptr = &color_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + color_bv->byteOffset;
	int stride = color_bv->byteStride;
	if (stride==0) stride=4*sizeof(unsigned char);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + color_acc->byteOffset + comp*stride;
	const unsigned int *pos_ptr4 = (const unsigned int*)pos_ptr3; // 3 = num of components in (x,y,z)
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
    return joints_bv_done && joints_done && joints_buf_done && weights_bv_done && weights_done && weights_buf_done;
  }
  virtual VEC4 Joints(int face, int point) const
  {

    if (joints_bv_done && joints_done && joints_buf_done) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &joints_buf->data[0];
	int stride2 = joints_bv->byteStride;
	if (stride2==0) {
	switch(joints_acc->componentType) {
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	  if (stride2==0) stride2=sizeof(unsigned char);
	  break;
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	  if (stride2==0) stride2=sizeof(char);
	  break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	    if (stride2==0) stride2=sizeof(unsigned short); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	    if (stride2==0) stride2=sizeof(short); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_INT:
	    if (stride2==0) stride2=sizeof(int); // 3 = num of indices in a ttiangle
	    break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	    if (stride2==0) stride2=sizeof(unsigned int); // 3 = num of indices in a ttiangle
	    break;
	default:
	  std::cout << "componentType wrong: " << indices_acc->componentType << std::endl;
	  if (stride2==0) stride2=sizeof(short);
	  break;
	};
	switch(joints_acc->type)
	  {
	  case TINYGLTF_TYPE_SCALAR: break;
	  case TINYGLTF_TYPE_VEC2: stride2*=2; break;
	  case TINYGLTF_TYPE_VEC3: stride2*=3; break;
	  case TINYGLTF_TYPE_VEC4: stride2*=4; break;
	  };
	}
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;
	unsigned char *ptr3 = (unsigned char*)(pos_ptr + joints_bv->byteOffset + index*stride2 + joints_acc->byteOffset); 
	switch(joints_acc->componentType)
	  {
	  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	    {
	      unsigned char *ptr4 = (unsigned char*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      break;
	    }
	  case TINYGLTF_COMPONENT_TYPE_BYTE:
	    {
	      char *ptr4 = (char*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      //	      index = *ptr4;
	      break;
	    }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  {
	  unsigned short *ptr4 = (unsigned short*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      //	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  {
	  short *ptr4 = (short*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      //	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_INT:
	  {
	  int *ptr4 = (int*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      //	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  {
	  unsigned int *ptr4 = (unsigned int*)ptr3;
	      index1 = ptr4[0];
	      index2 = ptr4[1];
	      index3 = ptr4[2];
	      index4 = ptr4[3];
	      //	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	};
	//if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)

	VEC4 res;
	res.x = 0.5+ int(((unsigned int)(index1))); // REMOVED &0XFF from these.
	res.y = 0.5+ int(((unsigned int)(index2)));
	res.z = 0.5+ int(((unsigned int)(index3)));
	res.w = 0.5+ int(((unsigned int)(index4)));
	//std::cout << "Joints: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;

	return res;
      } else {
	// todo, check that this branch works
	const unsigned char *pos_ptr = &joints_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + joints_bv->byteOffset;
	int stride = joints_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + joints_acc->byteOffset + comp*stride;
	const unsigned char *pos_ptr4 = (const unsigned char*)pos_ptr3; // 3 = num of components in (x,y,z)
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
      std::cout << "Attached end" << std::endl;
      VEC4 res;
      res.x = 0.5+int(mesh_index);
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
      //      return 0;
    }
    std::cout << "gltf attach unknown mode: "<< mode << std::endl;
      VEC4 res;
      res.x = 0.5+int(mesh_index);
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
      //    return 0;

    
  }
  virtual VEC4 Weights(int face, int point) const
  {

    if (weights_bv_done && weights_done && weights_buf_done) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &weights_buf->data[0];
	int stride2 = weights_bv->byteStride;
	if (stride2==0) { stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	switch(weights_acc->type)
	  {
	  case TINYGLTF_TYPE_SCALAR: break;
	  case TINYGLTF_TYPE_VEC2: stride2*=2; break;
	  case TINYGLTF_TYPE_VEC3: stride2*=3; break;
	  case TINYGLTF_TYPE_VEC4: stride2*=4; break;
	  };

	}
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
	const unsigned char *pos_ptr = &weights_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + weights_bv->byteOffset;

	int stride = weights_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + weights_acc->byteOffset + comp*stride;
	const float *pos_ptr4 = (const float*)pos_ptr3; // 3 = num of components in (x,y,z)
	//std::cout << face << " " << point << "::" << int(pos_ptr4[0]) << std::endl;
	//res[0] = pos_ptr4[0];
	//res[1] = pos_ptr4[1];
	//res[2] = pos_ptr4[2];
	//res[3] = pos_ptr4[3];
	//return res;
	//std::cout << "Attached end" << std::endl;
	VEC4 res;
	res.x = pos_ptr4[0];
	res.y = pos_ptr4[1];
	res.z = pos_ptr4[2];
	res.w = pos_ptr2[3];
	//std::cout << "Weights2: " << face << " " << point << "::" << res.x << ","<< res.y << "," << res.z << "," << res.w << std::endl;
	return res;
	//return pos_ptr4[num];
      }
      }
    } else {
      std::cout << "Attached end" << std::endl;
      VEC4 res;
      res.x = 1.0;
      res.y = 0.0;
      res.z = 0.0;
      res.w = 0.0;
      return res;
    }
    std::cout << "gltf weights unknown mode: "<< mode << std::endl;
    VEC4 res;
    res.x = 1.0;
    res.y = 0.0;
    res.z = 0.0;
    res.w = 0.0;
    return res;
    
    
  }
  
  bool has_texcoord() const { return texcoord_bv_done&&texcoord_done&&texcoord_buf_done; }
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
    if (texcoord_bv_done && texcoord_done && texcoord_buf_done) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &texcoord_buf->data[0];
	int stride2 = texcoord_bv->byteStride;
	if (stride2==0) stride2 = 2*sizeof(float); // 3 = num of components in (x,y,z)
	float *pos_ptr2 = (float*)(pos_ptr + texcoord_bv->byteOffset + index*stride2 + texcoord_acc->byteOffset); 
	//std::cout << face << " " << point << "::" << index << "::" << pos_ptr2[0] << "," << pos_ptr2[1] << "," << pos_ptr2[2] << std::endl;
	return Point(pos_ptr2[0], pos_ptr2[1], 0.0 /*pos_ptr2[2]*/);
      } else {
	// todo, check that this branch works
	const unsigned char *pos_ptr = &texcoord_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + texcoord_bv->byteOffset;
	int stride = texcoord_bv->byteStride;
	if (stride==0) stride=2*sizeof(float);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + texcoord_acc->byteOffset + comp*stride;
	const float *pos_ptr4 = (const float*)pos_ptr3; // 3 = num of components in (x,y,z)
	return Point(pos_ptr4[0], pos_ptr4[1], 0.0 /*pos_ptr4[2]*/);
      }
    }
    } else {
      return Point(0.0,0.0,0.0);
    }
    std::cout << "gltf TexCoord unknown mode" << std::endl;
    return Point(0.0,0.0,0.0);
  }

  bool HasBatchMap() const {
    /*
    int s = g_deleted_urls.size();
    for(int i=0;i<s;i++)
      {
	if (g_deleted_urls[i]==interface->Url())
	  {
	    return false;
	  }
      }
    */
    return false; /*mode==TINYGLTF_MODE_TRIANGLES && position_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT && g_glb_file_size<50000000 && g_zip_file_size<50000000 && g_glb_animated==false;*/ } // HERE CAN ENABLE THE GLTF OPTIMIZATION FOR VERTEX ARRAYS, IT IS NOT FULLY WORKING YET.
  FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    FaceBufferRef ref;
    ref.numfaces = indices_acc->count/3;
    ref.numvertices = position_acc->count/3;
    ref.indices_char = 0;
    ref.indices_short =0;
    ref.indices_int = 0;
    ref.facepoint=0;
    ref.facepoint2=0;
    ref.pointnormal=0;
    ref.color=0;
    ref.texcoords=0;
    ref.joints=0;
    ref.weights=0;
    
    if (indices_buf_done && indices_bv_done && indices_done)
    {
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

      
	const void *ptr = (unsigned char*)(&indices_buf->data[0]) + indices_bv->byteOffset + indices_acc->byteOffset + start_face*stride;

      
	switch(indices_acc->componentType) {
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	  ref.indices_char = (unsigned char*)ptr;
	  break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  ref.indices_short = (unsigned short*)ptr;
	    break;
	case TINYGLTF_COMPONENT_TYPE_INT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  ref.indices_int = (unsigned int*)ptr;
	    break;
	default:
	  std::cout << "componentType wrong: " << indices_acc->componentType << std::endl;
	  if (stride==0) stride=sizeof(short);
	  break;
	};
    }

    if (position_done)
    {
    int stride = position_bv->byteStride;
    if (stride==0) stride=3*sizeof(float);
    ref.facepoint = (Point*)((unsigned char*)(&position_buf->data[0]) + position_bv->byteOffset + position_acc->byteOffset + start_face*stride);
    }
    ref.facepoint2 = 0;

    if (normal_done)
    {
    int stride = normal_bv->byteStride;
    if (stride==0) stride=3*sizeof(float);
    int s = normal_acc->count;
    //std::cout << "NORMALS_LENGTH:" << s << std::endl;
    m_p1.push_back(ref.pointnormal = new Vector[s]);
    int byteOffset = normal_bv->byteOffset + normal_acc->byteOffset + start_face*stride;
    for(int i=0;i<s;i++)
      {
	// NOTE - is IMPORTANT HERE, IT CAUSES THE WHOLE LOOP
	ref.pointnormal[i] = -((Vector*)((unsigned char*)(&normal_buf->data[0]) + byteOffset))[i];
	//SHOULD BE OK std::cout << "NORMAL:" << i << "--" << ref.pointnormal[i] << std::endl;
      }
    
    //ref.pointnormal = (Vector*)((unsigned char*)(&normal_buf->data[0]) + normal_bv->byteOffset + normal_acc->byteOffset + start_face*stride);
    }

    if (color_done)
    {
      bool done = false;
    if (color_bv_done && color_done && color_buf_done) {
    if (mode==TINYGLTF_MODE_TRIANGLES) {
      done =true;
      
      int stride = color_bv->byteStride;
      if (stride==0) stride=4*sizeof(unsigned char);
      ref.color = (unsigned int*)((unsigned char*)(&color_buf->data[0]) + color_bv->byteOffset + color_acc->byteOffset + start_face*stride);
    }
    }
    if (!done)
      {
	int s = color_acc->count;
	m_p3.push_back(ref.color = new unsigned int[s]);
	std::memset(ref.color,0xff, sizeof(unsigned int)*s);
	
      }

    
    } else
      {
	int s = position_acc->count;
	m_p3.push_back(ref.color = new unsigned int[s]);
	std::memset(ref.color,0xff, sizeof(unsigned int)*s);

      }
    //std::cout << "GLTFFaceCollection::color=" << (int)ref.color << std::endl;

    if (texcoord_done)
    {
    int stride = texcoord_bv->byteStride;
    if (stride==0) stride=2*sizeof(float);
    int s = texcoord_acc->count;
    m_p2.push_back(ref.texcoords = new Point[s]);
    int byteOffset = texcoord_bv->byteOffset + texcoord_acc->byteOffset + start_face*stride;
    for(int i=0;i<s;i++)
      {
	ref.texcoords[i] = Point(((Point2d*)((unsigned char*)&texcoord_buf->data[0] + byteOffset))[i].x,
				   ((Point2d*)((unsigned char*)&texcoord_buf->data[0] + byteOffset))[i].y,
				   0.0);
      }
    //ref.texcoords = (Point*)((unsigned char*)(&texcoord_buf->data[0]) + texcoord_bv->byteOffset + texcoord_acc->byteOffset + start_face*stride);

    /*
    std::cout << "COLORGLTF:" << std::hex << ref.color[ref.indices_int[0]] << std::dec << std::endl;
    std::cout << std::hex << ref.color[ref.indices_int[1]] << std::dec<< std::endl;
    std::cout << std::hex << ref.color[ref.indices_int[2]] << std::dec << std::endl;

    std::cout << "TEXCOORDGLTF:" << ref.texcoords[ref.indices_int[0]] << std::endl;
    std::cout << ref.texcoords[ref.indices_int[1]] << std::endl;
    std::cout << ref.texcoords[ref.indices_int[2]] << std::endl;
    */
    /*

    std::cout << ref.pointnormal[ref.indices_int[0]] << std::endl;
    std::cout << ref.pointnormal[ref.indices_int[1]] << std::endl;
    std::cout << ref.pointnormal[ref.indices_int[2]] << std::endl;

    */
    
    /*
    std::cout << result.joints[result.indices_int[0]] << std::endl;
    std::cout << result.joints[result.indices_int[1]] << std::endl;
    std::cout << result.joints[result.indices_int[2]] << std::endl;

    std::cout << result.weights[result.indices_int[0]] << std::endl;
    std::cout << result.weights[result.indices_int[1]] << std::endl;
    std::cout << result.weights[result.indices_int[2]] << std::endl;
    */

    
    }

    if (joints_done)
    {
    int stride = joints_bv->byteStride;
    if (stride==0) stride=4*sizeof(unsigned char);
    ref.joints = (VEC4*)((&joints_buf->data[0]) + joints_bv->byteOffset + joints_acc->byteOffset + start_face*stride);
    }

    if (weights_done)
    {
    int stride = weights_bv->byteStride;
    if (stride==0) stride=4*sizeof(unsigned char);
    ref.weights = (VEC4*)((&weights_buf->data[0]) + weights_bv->byteOffset + weights_acc->byteOffset + start_face*stride);
    }

    return ref;
  }
  
private:
  tinygltf::Primitive prim;
  //tinygltf::Model *model;
  int mode;
  int indices_index;
  int position_index;
  int normal_index;
  int texcoord_index;
  int color_index;
  int joints_index;
  int weights_index;

  const tinygltf::Accessor *indices_acc;
  const tinygltf::Accessor *position_acc;
  const tinygltf::Accessor *normal_acc;
  const tinygltf::Accessor *texcoord_acc;
  const tinygltf::Accessor *color_acc;
  const tinygltf::Accessor *joints_acc;
  const tinygltf::Accessor *weights_acc;
  
  const tinygltf::BufferView *indices_bv;
  const tinygltf::BufferView *position_bv;
  const tinygltf::BufferView *normal_bv;
  const tinygltf::BufferView *texcoord_bv;
  const tinygltf::BufferView *color_bv;
  const tinygltf::BufferView *joints_bv;
  const tinygltf::BufferView *weights_bv;
  
  const tinygltf::Buffer *indices_buf;
  const tinygltf::Buffer *position_buf;
  const tinygltf::Buffer *normal_buf;
  const tinygltf::Buffer *texcoord_buf;
  const tinygltf::Buffer *color_buf;
  const tinygltf::Buffer *joints_buf;
  const tinygltf::Buffer *weights_buf;




  bool indices_done=false;
  bool position_done=false;
  bool normal_done=false;
  bool texcoord_done=false;
  bool color_done=false;
  bool joints_done=false;
  bool weights_done=false;
  
  bool indices_bv_done=false;
  bool position_bv_done=false;
  bool normal_bv_done=false;
  bool texcoord_bv_done=false;
  bool color_bv_done=false;
  bool joints_bv_done=false;
  bool weights_bv_done=false;
  
  bool indices_buf_done=false;
  bool position_buf_done=false;
  bool normal_buf_done=false;
  bool texcoord_buf_done=false;
  bool color_buf_done=false;
  bool joints_buf_done=false;
  bool weights_buf_done=false;

  
  GLTFModelInterface *interface;
  int mesh_index;
  int prim_index;

  mutable int store_face;
  mutable Vector store_res;
  mutable std::vector<Vector *> m_p1;
  mutable std::vector<Point*> m_p2;
  mutable std::vector<unsigned int*> m_p3;
};


GameApi::ML GameApi::MainLoopApi::gltf_material_nop_resize(EveryApi &ev, TF tf, int mesh_index, int prim_id, float mix)
{
  GLTFModelInterface *interface = find_gltf(e,tf);
   GameApi::ML ml = add_main_loop(e, new GLTF_Material_noP(e,ev,tf,mesh_index, prim_id, mix));
   resize_reset();
   GameApi::P p = add_polygon2(e, new GLTFFaceCollection(interface,mesh_index,prim_id),1);
   calc_resize_model(e,ev,p);
   return resize_model(e,ev,ml);
}



GameApi::BM GameApi::PolygonApi::gltf_load_bitmap( GameApi::EveryApi &ev, TF model0, int image_index )
{
  if (image_index==-1) {
    return ev.bitmap_api.newbitmap(1,1, 0xffffffff);
  }
  GLTFModelInterface *interface = find_gltf(e,model0);
  std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //   new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  //GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);
  GLTFImage *img = new GLTFImage( interface, image_index );
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




GameApi::BM gltf_load_bitmap2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int image_index)
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
  
  //GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);

  GLTFImage *img = new GLTFImage( interface, image_index );

  //Bitmap<Color> *img2 = new MemoizeBitmap(*img);


  Bitmap<Color> *bbm = new BitmapPrepareCache(e, interface->Url() + ss.str(), img);

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
GameApi::P gltf_load2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_index, int prim_index )
{
  int c = get_current_block();
  set_current_block(-1);
  //GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);
  GLTFModelInterface *model = interface;
  
  FaceCollection *faces = new GLTFFaceCollection( model, mesh_index, prim_index );
  GameApi::P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);
  confirm_texture_usage(e,p);
  GameApi::P pp;
  bool recalc_normals = !faces->has_normal();
  if (recalc_normals)
    pp = ev.polygon_api.recalculate_normals(p);
  else
    pp = p;
  GameApi::P p2 = pp; //ev.polygon_api.file_cache(pp, model->Url(), prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << model->Url();
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }
  //GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
  GameApi::P p3;
  if (!recalc_normals)
    p3 = ev.polygon_api.flip_normals(p2);
  else
    p3 = p2;
 return p3;
}
GameApi::P resize_to_correct_size2(GameApi::Env &e, GameApi::P model, Matrix *mat);
extern Matrix g_last_resize;

GameApi::P GameApi::PolygonApi::gltf_load_nr( GameApi::EveryApi &ev, GameApi::TF model0, int mesh_index, int prim_index)
{
  GLTFModelInterface *model = find_gltf(e,model0);
  std::string url = model->Url();
  
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  //LoadGltf *load = find_gltf_instance(e,model->BaseUrl(),model->Url(),gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  int c = get_current_block();
  set_current_block(-1);
  //GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);
  FaceCollection *faces = new GLTFFaceCollection( model, mesh_index, prim_index );
  P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);

  confirm_texture_usage(e,p);
  GameApi::P pp;
  bool recalc_normals = !faces->has_normal();
  if (recalc_normals)
    pp = ev.polygon_api.recalculate_normals(p);
  else
    pp = p;
  GameApi::P p2 = pp; //ev.polygon_api.file_cache(pp, model->Url(), prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << model->Url();
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }

  GameApi::P p3 = resize_to_correct_size2(e,p2,&g_last_resize);
  GameApi::P p4;
  if (!recalc_normals)
    p4 = ev.polygon_api.flip_normals(p3);
  else
    p4 = p3;
  //GameApi::P p4 = ev.polygon_api.flip_normals(p3);
  return p4;

}
GameApi::P GameApi::PolygonApi::gltf_load( GameApi::EveryApi &ev, GameApi::TF model0, int mesh_index, int prim_index )
{
  GLTFModelInterface *model = find_gltf(e,model0);
  std::string url = model->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  int c = get_current_block();
  set_current_block(-1);
  //GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);
  FaceCollection *faces = new GLTFFaceCollection( model, mesh_index, prim_index );
  P p = add_polygon2(e, faces,1);
  //p = ev.polygon_api.quads_to_triangles(p);

  confirm_texture_usage(e,p);
  GameApi::P pp;
  bool recalc_normals = !faces->has_normal();
  if (recalc_normals)
    pp = ev.polygon_api.recalculate_normals(p);
  else
    pp = p;
  GameApi::P p2 = pp; //ev.polygon_api.file_cache(p, model->Url(), prim_index+mesh_index*50);
  set_current_block(c);
  std::stringstream ss;
  ss << model->Url();
  ss << prim_index + mesh_index*50;
  bool b = g_gltf_cache[ss.str()];
  if (b) {
    // seems the caching messes up normals in the model? 
    //p2 = ev.polygon_api.flip_normals(p2);
  } else {
    g_gltf_cache[ss.str()] = true;
  }

  GameApi::P p3 = ev.polygon_api.resize_to_correct_size(p2);
  GameApi::P p4;
  if (!recalc_normals)
    p4 = ev.polygon_api.flip_normals(p3);
  else
    p4 = p3;
  //GameApi::P p4 = ev.polygon_api.flip_normals(p3);
  return p4;
}

class GLTF_Material2 : public MaterialForward
{
public:
  GLTF_Material2(GameApi::Env &e, GameApi::EveryApi &ev, float mix, float roughness, float metallic, float basecolor_r, float basecolor_g, float basecolor_b, float basecolor_a, float occul_strength, Vector light_dir) : e(e), ev(ev), mix(mix), roughness(roughness), metallic(metallic), base_r(basecolor_r), base_g(basecolor_g), base_b(basecolor_b), base_a(basecolor_a), occul(occul_strength), light_dir(light_dir) { }

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
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0,false,1.0,1.0,1.0, 1.0,1.0,1.0, 1.0,false,0.0,0.0,0.0, light_dir.dx,light_dir.dy,light_dir.dz); // todo base color
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
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0,false,1.0,1.0,1.0,1.0,1.0,1.0,1.0,false,0.0,0.0,0.0, light_dir.dx,light_dir.dy,light_dir.dz); 
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
    GameApi::ML I18=ev.polygon_api.gltf_shader(ev, I17, mix, false, false, false, false, false, false, false, false, roughness, metallic, base_r,base_g,base_b,base_a, occul, 1.0,false,1.0,1.0,1.0,1.0,1.0,1.0,1.0,false,0.0,0.0,0.0, light_dir.dx,light_dir.dy,light_dir.dz); 
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
  float emiss;
  Vector light_dir;
};


class BlendMainLoop : public MainLoopItem
{
public:
  BlendMainLoop(MainLoopItem *next, bool enable) : next(next),b(enable) { }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) {
    OpenglLowApi *ogl = g_low->ogl;
    if (b)
      ogl->glEnable(Low_GL_BLEND);
    else
      ogl->glDisable(Low_GL_BLEND);
    next->execute(e);
    ogl->glEnable(Low_GL_BLEND);
    
  }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
  bool b;
};
GameApi::ML blendmainloop(GameApi::Env &e, GameApi::ML ml, bool b)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e,new BlendMainLoop(item,b));
}

bool g_global_is_transparent=false;

class TransMainLoop : public MainLoopItem
{
public:
  TransMainLoop(MainLoopItem *next, bool enable) : next(next),is_transparent(enable) { }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void FirstFrame() { }

  virtual void execute(MainLoopEnv &e) {
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
  virtual bool IsTransparent() const { return is_transparent; }
private:
  MainLoopItem *next;
  bool b;
  bool is_transparent;
};
GameApi::ML transmainloop(GameApi::Env &e, GameApi::ML ml, bool b)
{
  MainLoopItem *item = find_main_loop(e,ml);
  return add_main_loop(e,new TransMainLoop(item,b));
}

class TransFaceCollection : public ForwardFaceCollection
{
public:
  TransFaceCollection(FaceCollection *next) : next(next), ForwardFaceCollection(*next) { }
  virtual std::string name() const { return "TransFaceCollection"; }

  virtual Point FacePoint(int face, int point) const { if (face==0) stackTrace(); return next->FacePoint(face,point); }
  virtual bool IsTransparent() const { std::cout << "TRUE" << std::endl; return true; }
private:
  FaceCollection *next;
};
GameApi::P transfaces(GameApi::Env &e, GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new TransFaceCollection(coll),1);
}


class GLTF_Material : public MaterialForward
{
public:
  GLTF_Material(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int material_id, float mix, Vector light_dir) : e(e), ev(ev), interface(interface), material_id(material_id),mix(mix), light_dir(light_dir) { 
  }
  bool IsTransparent() const
  {
    if (material_id>=0 && material_id<int(interface->materials_size())) {
      const tinygltf::Material &m = interface->get_material(material_id);
      const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
      if (r.baseColorFactor[3]<0.9)
	{
	  return true;
	}
    }
    return false;
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

  void print_extension_map() const {
#if 0
    const tinygltf::Material &m = interface->get_material(material_id);
    std::map<std::string, tinygltf::Value>::const_iterator i1 = m.extensions.begin();
    std::map<std::string, tinygltf::Value>::const_iterator i2 = m.extensions.end();
    for(std::map<std::string,tinygltf::Value>::const_iterator i = i1;i!=i2;i++) {
      std::pair<std::string,tinygltf::Value> p = *i;
      std::cout << p.first << " " << p.second.Type() << " " << p.second.GetNumberAsDouble() << p.second.IsBool() << " " << p.second.IsInt() << " " << p.second.IsNumber() << " " << p.second.IsReal() << " " << p.second.IsString() << " " << p.second.IsBinary() << " " << p.second.IsArray() << " " << p.second.IsObject() << std::endl;
    }
    
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;

    std::cout << "SPECGLOSSI:" << specglossi.IsBool() << " " << specglossi.IsInt() << " " << specglossi.IsNumber() << " " << specglossi.IsReal() << " " << specglossi.IsString() << " " << specglossi.IsBinary() << " " << specglossi.IsArray() << " " << specglossi.IsObject() << std::endl;
    std::vector<std::string> keys = specglossi.Keys();
    int s = keys.size();
    for(int i=0;i<s;i++) {
      std::cout << keys[i] << std::endl;
    }
#endif
  }

  bool get_unlit() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &unlit= (*m.extensions.find("KHR_materials_unlit")).second;
    return unlit.IsObject();
  }

  bool get_sheen() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &unlit= (*m.extensions.find("KHR_materials_sheen")).second;
    return unlit.IsObject();
  }

  
  
  Vector get_diffuse_factor() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_vector("diffuse_factor",Vector(1.0f,1.0f,1.0f));
    const tinygltf::Value &diffuse = specglossi.Get("diffuseFactor");
    if (!diffuse.IsArray()) return print_vector("diffuse_factor",Vector(1.0f,1.0f,1.0f));
    const tinygltf::Value &diff_r = diffuse.Get(0);
    const tinygltf::Value &diff_g = diffuse.Get(1);
    const tinygltf::Value &diff_b = diffuse.Get(2);
    Vector res;
    res.dx = diff_r.GetNumberAsDouble();
    res.dy = diff_g.GetNumberAsDouble();
    res.dz = diff_b.GetNumberAsDouble();
    return print_vector("diffuse_factor",res);
    //return diffuse.GetNumberAsDouble();
  }

  Vector get_specular_factor() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_vector("specular_factor",Vector(0.5f,0.5f,0.5f));
    const tinygltf::Value &diffuse = specglossi.Get("specularFactor");
    if (!diffuse.IsArray()) return print_vector("specular_factor",Vector(0.5f,0.5f,0.5f));
    const tinygltf::Value &diff_r = diffuse.Get(0);
    const tinygltf::Value &diff_g = diffuse.Get(1);
    const tinygltf::Value &diff_b = diffuse.Get(2);
    Vector res;
    res.dx = diff_r.GetNumberAsDouble();
    res.dy = diff_g.GetNumberAsDouble();
    res.dz = diff_b.GetNumberAsDouble();
    return print_vector("specular_factor",res);
  }

  float get_glossiness_factor() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return print_float("glossinessfactor",1.0f);
    const tinygltf::Value &diffuse = specglossi.Get("glossinessFactor");
    float glos = diffuse.GetNumberAsDouble();
    if (glos<0.04) glos=1.0f;
    return print_float("glossinessfactor",glos);
  }

  
  bool get_spec() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return false;
    return true;
  }
  int print_int(std::string label, int i) const { /*std::cout << label << ":" << i << std::endl;*/ return i; }
  float print_float(std::string label, float i) const { /*std::cout << label << ":" << i << std::endl;*/ return i; }
  Vector print_vector(std::string label, Vector i) const { /*std::cout << label << ":" << i.dx << " " << i.dy << " " << i.dz << std::endl;*/ return i; }

  int get_sheen_index() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_sheen")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &diffuse = specglossi.Get("sheenColorTexture");
    if (!diffuse.IsObject()) return -1;
    const tinygltf::Value &index = diffuse.Get("index");
    return print_int("sheen index",index.GetNumberAsInt());
  }
  
  int get_diffuse_index() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &diffuse = specglossi.Get("diffuseTexture");
    if (!diffuse.IsObject()) return -1;
    const tinygltf::Value &index = diffuse.Get("index");
    return print_int("diffuse_index",index.GetNumberAsInt());
  }
  int get_specglossi_index() const
  {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::Value &specglossi = (*m.extensions.find("KHR_materials_pbrSpecularGlossiness")).second;
    if (!specglossi.IsObject()) return -1;
    const tinygltf::Value &s = specglossi.Get("specularGlossinessTexture");
    if (!s.IsObject()) return -1;
    const tinygltf::Value &s2 = s.Get("index");
    return print_int("specglossi index",s2.GetNumberAsInt());
  }

  void specglossyprepare(GameApi::EveryApi &ev, GameApi::P p) const
  {
    confirm_texture_usage(ev.get_env(),p);
  }
#if 0
  GameApi::ML specglossyshader(GameApi::EveryApi &ev, GameApi::ML ml) const
  {
    //std::cout << "GLOSSY:" << get_diffuse_factor() << " " << get_glossiness_factor() << " " << get_specular_factor() << std::endl;
    GameApi::ML tex1 = ev.polygon_api.texture_many_shader(ev, ml, 0.5*get_glossiness_factor()+0.5*(1.0-get_diffuse_factor()));

    GameApi::ML tex2 = ev.polygon_api.phong_shader(ev,tex1, 0.0,0.0,-1.0,0xff000000,0xffffffff, (get_specular_factor())*30.0);
    return tex2;
  }
#endif  
  

  int num_textures() const {
    return 5; // (1=base color, 2=metallicroughness), 3=normal, 4=occulsion, 5=emissive
  }
  GameApi::BM texture(int i) const {
    print_extension_map();
    if (material_id<0 || material_id>=int(interface->materials_size())||!has_texture(i)) {
      //std::cout << "Bad material: i=" << i << " material_id=" << material_id << " has_texture(i)=" << has_texture(i) <<std::endl; 
      return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    switch(i) {
    case 0: {
      int index = m.pbrMetallicRoughness.baseColorTexture.index;
      if (get_spec()) index=get_specglossi_index();
      if (get_sheen()) index=get_sheen_index();
      //std::cout << "IMG0=" << index << "(" << get_spec() << "," << get_sheen() << ")" << std::endl;
      return gltf_load_bitmap2(e,ev, interface, index);
    }
    case 1: {
      int index = m.pbrMetallicRoughness.metallicRoughnessTexture.index;
      if (get_spec()) index=get_diffuse_index();
      //std::cout << "IMG1=" << index << "(" << get_spec() << ")" << std::endl;
      return gltf_load_bitmap2(e,ev, interface, index);
    }
    case 2: return gltf_load_bitmap2(e,ev, interface, m.normalTexture.index);
    case 3: return gltf_load_bitmap2(e,ev, interface, m.occlusionTexture.index);
    case 4: return gltf_load_bitmap2(e,ev, interface, m.emissiveTexture.index);
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      return false;
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    //std::cout << "has_texture " << i << " materia_id=" << material_id << "<" << interface->materials_size() << " " << get_spec() << " " << get_specglossi_index() << " " << get_sheen() << " " << get_sheen_index() << " " << m.pbrMetallicRoughness.baseColorTexture.index <<std::endl;
    switch(i) {
    case 0: return m.pbrMetallicRoughness.baseColorTexture.index!=-1||(get_spec() && get_specglossi_index()!=-1)||(get_sheen() &&get_sheen_index()!=-1);
    case 1: return m.pbrMetallicRoughness.metallicRoughnessTexture.index!=-1||(get_spec()&&get_diffuse_index()!=-1);
    case 2: return m.normalTexture.index!=-1;
    case 3: return m.occlusionTexture.index!=-1;
    case 4: return m.emissiveTexture.index!=-1;
    default: return false;
    };
  }

  virtual GameApi::ML mat2(GameApi::P p) const
  {



    interface->Prepare();
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
    confirm_texture_usage(ev.get_env(),p);

    //I10 = ev.polygon_api.fix_vertex_order(I10);
    //if (get_diffuse_index()!=-1) {
    //  specglossyprepare(ev,p);
    //}
    
    std::vector<std::string> id_labels;
    for(int i=0;i<s;i++)
      {
	std::stringstream ss;
	ss << interface->Url() << "_" << material_id << "_" << i << std::endl;
	id_labels.push_back(ss.str());
      }
    
    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm,std::vector<int>(),id_labels);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
      //if (get_diffuse_index()==-1) {
      const tinygltf::Material &m = interface->get_material(material_id);
      std::vector<double> emis=m.emissiveFactor;
      Point emis2= { float(emis[0]),float(emis[1]),float(emis[2]) };
      const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
      const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
      I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),r.baseColorFactor[3], o.strength, 1.0,get_spec(),get_diffuse_factor().dx,get_diffuse_factor().dy,get_diffuse_factor().dz, get_specular_factor().dx,get_specular_factor().dy,get_specular_factor().dz, get_glossiness_factor(), get_unlit(),emis2.x,emis2.y,emis2.z,light_dir.dx, light_dir.dy, light_dir.dz); // todo base color

      //} else {
      //	I18 = specglossyshader(ev,I17);
      //}
      // const tinygltf::Material &m = interface->get_material(material_id);


    if (m.alphaMode=="BLEND") {
      //OpenglLowApi *ogl = g_low->ogl;
      //ogl->glEnable(Low_GL_BLEND);
      //I18 = ev.mainloop_api.transparent(I18);
      I18 = blendmainloop(e,I18,true);
    } else {
      I18 = blendmainloop(e,I18,false);
    }

    if (m.doubleSided==true) {
      I18 = ev.mainloop_api.cullface(I18,false,true);
    } else {
      I18 = ev.mainloop_api.cullface(I18,true,true);
    }
    
    }
    // GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    interface->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }



    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    confirm_texture_usage(ev.get_env(),p);

    //I10 = ev.polygon_api.fix_vertex_order(I10);
    
    //if (get_diffuse_index()!=-1) {
    //  specglossyprepare(ev,p);
    //}
    std::vector<std::string> id_labels;
    for(int i=0;i<s;i++)
      {
	std::stringstream ss;
	ss << interface->Url() << "_" << material_id << "_" << i << std::endl;
	id_labels.push_back(ss.str());
      }
    
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,I10,pts,bm, std::vector<int>(),id_labels);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
      //if (get_diffuse_index()==-1) {
      const tinygltf::Material &m = interface->get_material(material_id);
      std::vector<double> emis=m.emissiveFactor;
      Point emis2= { float(emis[0]),float(emis[1]),float(emis[2]) };
    const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
    const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),r.baseColorFactor[3], o.strength, 1.0,get_spec(),get_diffuse_factor().dx,get_diffuse_factor().dy,get_diffuse_factor().dz, get_specular_factor().dx,get_specular_factor().dy,get_specular_factor().dz, get_glossiness_factor(), get_unlit(),emis2.x,emis2.y,emis2.z,light_dir.dx,light_dir.dy,light_dir.dz);
    //} else {
    //	I18 = specglossyshader(ev,I17);
    // }
    // const tinygltf::Material &m = interface->get_material(material_id);
    if (m.alphaMode=="BLEND") {
      //OpenglLowApi *ogl = g_low->ogl;
      //ogl->glEnable(Low_GL_BLEND);
      //I18 = ev.mainloop_api.transparent(I18);
      //      I18=ev.mainloop_api.blendfunc(I18,2,3);
      I18 = blendmainloop(e,I18,true);
    } else
      {
      I18 = blendmainloop(e,I18,false);
      }
    if (m.doubleSided==true) {
      I18 = ev.mainloop_api.cullface(I18,false, true);
    } else {
      I18 = ev.mainloop_api.cullface(I18,true, true);
    }

    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    interface->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }



    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    confirm_texture_usage(ev.get_env(),p);
    //I10 = ev.polygon_api.fix_vertex_order(I10);

    //if (get_diffuse_index()!=-1) {
    //  specglossyprepare(ev,p);
    //}

    
    std::vector<std::string> id_labels;
    for(int i=0;i<s;i++)
      {
	std::stringstream ss;
	ss << interface->Url() << "_" << material_id << "_" << i << std::endl;
	id_labels.push_back(ss.str());
      }
    
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,I10,ms,bm,std::vector<int>(),id_labels);
    GameApi::ML I18;
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
      // if (get_diffuse_index()==-1) {
      const tinygltf::Material &m = interface->get_material(material_id);
      std::vector<double> emis=m.emissiveFactor;
      Point emis2= { float(emis[0]),float(emis[1]),float(emis[2]) };
    const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
    const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4),false, false, false, r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),r.baseColorFactor[3], o.strength, 1.0,get_spec(),get_diffuse_factor().dx,get_diffuse_factor().dy,get_diffuse_factor().dz, get_specular_factor().dx,get_specular_factor().dy,get_specular_factor().dz, get_glossiness_factor(), get_unlit(),emis2.x,emis2.y,emis2.z,light_dir.dx,light_dir.dy,light_dir.dz);
    //} else {
    //	I18 = specglossyshader(ev,I17);
    // }
    //  const tinygltf::Material &m = interface->get_material(material_id);
    if (m.alphaMode=="BLEND") {
      //OpenglLowApi *ogl = g_low->ogl;
      //ogl->glEnable(Low_GL_BLEND);
      //I18 = ev.mainloop_api.transparent(I18);
      //      I18=ev.mainloop_api.blendfunc(I18,2,3);
      I18 = blendmainloop(e,I18,true);
    } else {
      I18 = blendmainloop(e,I18,false);
    }
    if (m.doubleSided==true) {
      I18 = ev.mainloop_api.cullface(I18,false, true);
    } else {
      I18 = ev.mainloop_api.cullface(I18,true, true);
    }
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
  GLTFModelInterface *interface;
  int material_id;
  float mix;
  Vector light_dir;
  bool is_transparent=false;
};

int get_num_textures(GLTF_Material *mat)
{
  return mat->num_textures();
}

class GLTF_Material_manual : public MaterialForward
{
public:
  
  GLTF_Material_manual(GameApi::Env &e, GameApi::EveryApi &ev, float mix,
		       GameApi::BM baseColor, GameApi::BM metalrough, GameApi::BM normaltexture, GameApi::BM occlusion, GameApi::BM emissive,
		       bool baseColor_b, bool metalrough_b, bool normaltexture_b, bool occlusion_b, bool emissive_b, float roughnessfactor, float metallicfactor, float baseColor_red, float baseColor_green, float baseColor_blue, float baseColor_alpha, float occulsionStrength, Vector light_dir)
    : e(e), ev(ev),  mix(mix), baseColor_bm(baseColor), metalrough_bm(metalrough), normaltexture_bm(normaltexture), occlusion_bm(occlusion), emissive_bm(emissive),baseColor_b(baseColor_b), metalrough_b(metalrough_b), normaltexture_b(normaltexture_b), occlusion_b(occlusion_b), emissive_b(emissive_b), roughnessfactor(roughnessfactor), metallicfactor(metallicfactor), baseColor_red(baseColor_red), baseColor_green(baseColor_green), baseColor_blue(baseColor_blue), baseColor_alpha(baseColor_alpha), occulsionStrength(occulsionStrength), light_dir(light_dir)

																													     
  { 
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
    //if (material_id<0 || material_id>=int(load->model.materials.size())) {
    //  return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    //}
    switch(i) {
    case 0: return baseColor_bm; /*gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index);*/
    case 1: return metalrough_bm; /*gltf_load_bitmap2(e,ev, load, load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index);*/
    case 2: return normaltexture_bm; /*gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].normalTexture.index);*/
    case 3: return occlusion_bm; /*gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].occlusionTexture.index);*/
    case 4: return emissive_bm; /*gltf_load_bitmap2(e,ev,load, load->model.materials[material_id].emissiveTexture.index);*/
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
    //if (material_id<0 || material_id>=int(load->model.materials.size())) {
    //  return false;
    //}

    switch(i) {
    case 0: return baseColor_b; //load->model.materials[material_id].pbrMetallicRoughness.baseColorTexture.index!=-1;
    case 1: return metalrough_b; //load->model.materials[material_id].pbrMetallicRoughness.metallicRoughnessTexture.index!=-1;
    case 2: return normaltexture_b; //load->model.materials[material_id].normalTexture.index!=-1;
    case 3: return occlusion_b; //load->model.materials[material_id].occlusionTexture.index!=-1;
    case 4: return emissive_b; //load->model.materials[material_id].emissiveTexture.index!=-1;
    default: return false;
    };
  }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    //load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    for(int i=0;i<s;i++)
      {
	ev.bitmap_api.prepare(bm[i]);
      }
    //GameApi::ML I13;
    //I13.id = next->mat(p.id);
    //GameApi::P I10=p; 
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.polygon_api.render_vertex_array_ml2_texture(ev,I10,bm);
    GameApi::ML I18;
    //if (material_id<0 || material_id>=int(load->model.materials.size())) {
    //  I18 = I17;
    //} else {
    //tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    //tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,roughnessfactor, metallicfactor, baseColor_red, baseColor_green,baseColor_blue,baseColor_alpha, occulsionStrength, 1.0,false,1.0,1.0,1.0,1.0,1.0,1.0,1.0,false,0.0,0.0,0.0, light_dir.dx,light_dir.dy,light_dir.dz); // todo base color
      //}
    // GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    //load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    for(int i=0;i<s;i++)
      {
	ev.bitmap_api.prepare(bm[i]);
      }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);

    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture(ev,I10,pts,bm);
    GameApi::ML I18;
    //if (material_id<0 || material_id>=int(load->model.materials.size())) {
      // I18 = I17;
      //} else {

      //tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
    //tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,roughnessfactor, metallicfactor, baseColor_red, baseColor_green,baseColor_blue,baseColor_alpha, occulsionStrength, 1.0,false,1.0,1.0,1.0,1.0,1.0,1.0,1.0,false,0.0,0.0,0.0, light_dir.dx,light_dir.dy,light_dir.dz); // todo base color
      //}
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    //load->Prepare();
    std::vector<GameApi::BM> bm;
    int s = num_indexes();
    for(int i=0;i<s;i++) {
      int j = map_index(i);
      bm.push_back(texture(j));
    }
    for(int i=0;i<s;i++)
      {
	ev.bitmap_api.prepare(bm[i]);
      }
    //GameApi::ML I13;
    //I13.id = next->mat_inst(p.id,pts.id);
    GameApi::P I10 = p; //ev.polygon_api.flip_normals(p);
    GameApi::ML I17=ev.materials_api.render_instanced_ml_texture_matrix(ev,I10,ms,bm);
    GameApi::ML I18;
    //if (material_id<0 || material_id>=int(load->model.materials.size())) {
    //  I18 = I17;
    //} else {

      //tinygltf::PbrMetallicRoughness &r = load->model.materials[material_id].pbrMetallicRoughness;
      //tinygltf::OcclusionTextureInfo &o = load->model.materials[material_id].occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), false,false, false,roughnessfactor, metallicfactor, baseColor_red, baseColor_green,baseColor_blue,baseColor_alpha, occulsionStrength, 1.0,false,1.0,1.0,1.0,1.0,1.0,1.0,1.0,false,0.0,0.0,0.0,light_dir.dx, light_dir.dy, light_dir.dz); // todo base color
      //}
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
  //LoadGltf *load;
  //int material_id;
  float mix;
  GameApi::BM baseColor_bm;
  GameApi::BM metalrough_bm;
  GameApi::BM normaltexture_bm;
  GameApi::BM occlusion_bm;
  GameApi::BM emissive_bm;
  bool baseColor_b, metalrough_b, normaltexture_b, occlusion_b, emissive_b;
  float roughnessfactor, metallicfactor, baseColor_red, baseColor_green, baseColor_blue, baseColor_alpha, occulsionStrength;
  Vector light_dir;
};


GameApi::MT gltf_material2_manual( GameApi::Env &e, GameApi::EveryApi &ev, float mix, GameApi::BM baseColor, GameApi::BM metalrough, GameApi::BM normaltexture, GameApi::BM occlusion, GameApi::BM emissive, bool baseColor_b, bool metalrough_b, bool normaltexture_b, bool occlusion_b, bool emissive_b, float roughnessfactor, float metallicfactor, float baseColor_red, float baseColor_green, float baseColor_blue, float baseColor_alpha, float occulsionStrength, Vector light_dir);


void MAT_CB(void *);

class GLTF_Material_from_file : public MaterialForward
{
public:
  GLTF_Material_from_file(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string homepage, Vector light_dir) : e(env), ev(ev), url(url), homepage(homepage), light_dir(light_dir) { mat.id = -1;

    env.async_load_callback(url, &MAT_CB, (void*)this);
  }
  void MaterialCallback()
  {
    if (mat.id==-1) mat=mat_from_file();
  }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    if (mat.id==-1) mat=mat_from_file();
    Material *mat2 = find_material(e,mat);
    return mat2->mat(p.id);
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    if (mat.id==-1) mat=mat_from_file();
    Material *mat2 = find_material(e,mat);
    return mat2->mat_inst(p.id,pts.id);
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    if (mat.id==-1) mat=mat_from_file();
    Material *mat2 = find_material(e,mat);
    return mat2->mat_inst_matrix(p.id,ms.id);
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    if (mat.id==-1) mat=mat_from_file();
    Material *mat2 = find_material(e,mat);
    return mat2->mat_inst2(p.id,pta.id);
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    if (mat.id==-1) mat=mat_from_file();
    Material *mat2 = find_material(e,mat);
    return mat2->mat_inst_fade(p.id,pts.id,flip,start_time,end_time);
  }

  
  std::string fix_url(std::string url, std::string name) const
  {
    int s = url.size();
    int pos = -1;
    for(int i=0;i<s;i++)
      {
	if (url[i]=='/'||url[i]=='\\')
	  {
	    pos = i;
	  }
      }
    if (pos==-1) return "";
    std::string start = url.substr(0,pos);
    return start + "/" + name;    
  }

  
  GameApi::MT mat_from_file() const
  {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    std::string s(ptr->begin(), ptr->end());
    std::stringstream ss(s);
    std::string line;
    while(std::getline(ss,line))
      {
	std::string key;
	std::stringstream ss2(line);
	ss2 >> key;
	//std::cout << "Parsing " << key << std::endl;
	if (key=="mix") { ss2 >> mix; }
	if (key=="baseColorTexture") {
	  std::string val;
	  ss2 >> val;
	  baseColor_b = val=="true";
	  ss2 >> baseColor_url;
	}
	if (key=="metalRoughTexture") {
	  std::string val;
	  ss2 >> val;
	  metalRough_b = val=="true";

	  ss2 >> metalRough_url; }
	if (key=="normalTexture") {
	  std::string val;
	  ss2 >> val;
	  normal_b = val=="true";

	  ss2 >> normal_url; }
	if (key=="occulsionTexture") {
	  std::string val;
	  ss2 >> val;
	  occul_b = val=="true";

	  ss2 >> occul_url; }
	if (key=="emissiveTexture") {
	  	  std::string val;
	  ss2 >> val;
	  emis_b = val=="true";

	  ss2 >> emis_url; }
	if (key=="roughnessfactor") { ss2 >> roughFactor; }
	if (key=="metallicfactor") { ss2 >> metalFactor; }
	if (key=="baseColor") { ss2 >> base_r >> base_g >> base_b >> base_a; }
	if (key=="occusionstrength") { ss2 >> occul_strength; }
      }
    
    baseColor_url = fix_url(url, baseColor_url);
    metalRough_url = fix_url(url, metalRough_url);
    normal_url = fix_url(url, normal_url);
    occul_url = fix_url(url, occul_url);
    emis_url = fix_url(url, emis_url);

#ifdef EMSCRIPTEN
    if (baseColor_b) e.async_load_url(baseColor_url, homepage);
    if (metalRough_b) e.async_load_url(metalRough_url, homepage);
    if (normal_b) e.async_load_url(normal_url, homepage);
    if (occul_b) e.async_load_url(occul_url, homepage);
    if (emis_b) e.async_load_url(emis_url, homepage);
#endif
    //std::cout << "Loading: " << baseColor_url << std::endl;
    //std::cout << "Loading: " << metalRough_url << std::endl;
    //std::cout << "Loading: " << normal_url << std::endl;
    //std::cout << "Loading: " << occul_url << std::endl;
    //std::cout << "Loading: " << emis_url << std::endl;
    
    
    GameApi::BM bm_base;
    if (baseColor_b) {
      bm_base = ev.bitmap_api.loadbitmapfromurl(baseColor_url);
    } else { bm_base = ev.bitmap_api.newbitmap(1,1,0x0); }

    GameApi::BM bm_metalrough;
    if (metalRough_b)
      {
	bm_metalrough = ev.bitmap_api.loadbitmapfromurl(metalRough_url);
      } else { bm_metalrough = ev.bitmap_api.newbitmap(1,1,0x0); }

    GameApi::BM bm_normal;
    if (normal_b)
      {
	bm_normal = ev.bitmap_api.loadbitmapfromurl(normal_url);
      } else { bm_normal = ev.bitmap_api.newbitmap(1,1,0x0); }

    GameApi::BM bm_occul;
    if (occul_b)
      {
	bm_occul = ev.bitmap_api.loadbitmapfromurl(occul_url);
      } else { bm_occul = ev.bitmap_api.newbitmap(1,1,0x0); }

    GameApi::BM bm_emis;
    if (emis_b)
      {
	bm_emis = ev.bitmap_api.loadbitmapfromurl(emis_url);
      } else { bm_emis = ev.bitmap_api.newbitmap(1,1,0x0); }
    

    return gltf_material2_manual(e, ev, mix, bm_base, bm_metalrough, bm_normal, bm_occul, bm_emis, baseColor_b, metalRough_b, normal_b, occul_b, emis_b, roughFactor, metalFactor, base_b, base_g, base_b, base_a, occul_strength,light_dir);
    
  }

  
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  mutable GameApi::MT mat;
  std::string url, homepage;
  mutable float mix = 1.0;
  mutable bool baseColor_b = false;
  mutable std::string baseColor_url;
  mutable bool metalRough_b = false;
  mutable std::string metalRough_url;
  mutable bool normal_b = false;
  mutable std::string normal_url;
  mutable bool occul_b = false;
  mutable std::string occul_url;
  mutable bool emis_b = false;
  mutable std::string emis_url;
  mutable float roughFactor = 0.5;
  mutable float metalFactor = 0.5;
  mutable float base_r=0.5, base_g=0.5, base_b=0.5, base_a=1.0;
  mutable float occul_strength=1.0;
  Vector light_dir;
};
void MAT_CB(void *dt)
{
  GLTF_Material_from_file* ptr = (GLTF_Material_from_file*)dt;
  ptr->MaterialCallback();
}



class GLTF_Material_env : public MaterialForward
{
public:
  GLTF_Material_env(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int material_id, float mix, GameApi::BM diffuse_env, GameApi::BM specular_env, GameApi::BM brfd) : e(e), ev(ev), interface(interface), material_id(material_id),mix(mix), diffuse_env(diffuse_env), specular_env(specular_env), bfrd(brfd) { 
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
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      return ev.bitmap_api.newbitmap(1,1,0xffffffff);
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    switch(i) {
    case 0: return gltf_load_bitmap2(e,ev, interface, m.pbrMetallicRoughness.baseColorTexture.index);
    case 1: return gltf_load_bitmap2(e,ev, interface, m.pbrMetallicRoughness.metallicRoughnessTexture.index);
    case 2: return gltf_load_bitmap2(e,ev,interface, m.normalTexture.index);
    case 3: return gltf_load_bitmap2(e,ev,interface, m.occlusionTexture.index);
    case 4: return gltf_load_bitmap2(e,ev,interface, m.emissiveTexture.index);
    case 5: return diffuse_env;
    case 6: return specular_env;
    case 7: return bfrd;
    default:
      std::cout << "ERROR: gltf_meterial::texture" << std::endl;
      GameApi::BM bm; bm.id=-1; return bm;
    };
  }
  bool has_texture(int i) const {
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      return false;
    }
    const tinygltf::Material &m = interface->get_material(material_id);
    switch(i) {
    case 0: return m.pbrMetallicRoughness.baseColorTexture.index!=-1;
    case 1: return m.pbrMetallicRoughness.metallicRoughnessTexture.index!=-1;
    case 2: return m.normalTexture.index!=-1;
    case 3: return m.occlusionTexture.index!=-1;
    case 4: return m.emissiveTexture.index!=-1;
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
    interface->Prepare();
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
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
    const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17, mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),1.0 /*r.baseColorFactor[3]*/, o.strength, 1.0,false); // todo base color
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;

  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    interface->Prepare();
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
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
    const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),1.0 /*r.baseColorFactor[3]*/, o.strength, 1.0,false);
    }
    //GameApi::ML I19=ev.mainloop_api.flip_scene_if_mobile(ev,I18);
    return I18;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    interface->Prepare();
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
    if (material_id<0 || material_id>=int(interface->materials_size())) {
      I18 = I17;
    } else {
    const tinygltf::Material &m = interface->get_material(material_id);
    const tinygltf::PbrMetallicRoughness &r = m.pbrMetallicRoughness;
    const tinygltf::OcclusionTextureInfo &o = m.occlusionTexture;
    I18=ev.polygon_api.gltf_shader(ev, I17,mix, has_texture(0), has_texture(1), has_texture(2), has_texture(3), has_texture(4), has_texture(5), has_texture(6), has_texture(7), r.roughnessFactor, r.metallicFactor, baseColorChange(r.baseColorFactor[0]*baseColorFactor),baseColorChange(r.baseColorFactor[1]*baseColorFactor),baseColorChange(r.baseColorFactor[2]*baseColorFactor),1.0 /*r.baseColorFactor[3]*/, o.strength, 1.0,false);
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
  //LoadGltf *load;
  GLTFModelInterface *interface;
  int material_id;
  float mix;
  GameApi::BM diffuse_env, specular_env, bfrd;
};

GameApi::MT gltf_material2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *model, int material_id, float mix, Vector light_dir )
  {
    Material *mat = new GLTF_Material(e,ev, model, material_id, mix, light_dir);
  return add_material(e, mat);
  }
GameApi::MT gltf_material2_manual( GameApi::Env &e, GameApi::EveryApi &ev, float mix, GameApi::BM baseColor, GameApi::BM metalrough, GameApi::BM normaltexture, GameApi::BM occlusion, GameApi::BM emissive, bool baseColor_b, bool metalrough_b, bool normaltexture_b, bool occlusion_b, bool emissive_b, float roughnessfactor, float metallicfactor, float baseColor_red, float baseColor_green, float baseColor_blue, float baseColor_alpha, float occulsionStrength, Vector light_dir)
{
  Material *mat = new GLTF_Material_manual(e,ev, mix, baseColor, metalrough, normaltexture, occlusion, emissive, baseColor_b, metalrough_b, normaltexture_b, occlusion_b, emissive_b, roughnessfactor, metallicfactor, baseColor_red, baseColor_green, baseColor_blue, baseColor_alpha, occulsionStrength, light_dir);
  return add_material(e, mat);
}
			   
GameApi::MT GameApi::MaterialsApi::gltf_material3( GameApi::EveryApi &ev, float roughness, float metallic, float base_r, float base_g, float base_b, float base_a, float mix, float light_dir_x, float light_dir_y, float light_dir_z)
{
  return add_material(e, new GLTF_Material2(e,ev, mix, roughness, metallic, base_r, base_g, base_b, base_a,1.0,Vector(light_dir_x,light_dir_y,light_dir_z)));
}

GameApi::MT GameApi::MaterialsApi::gltf_material_from_file( GameApi::EveryApi &ev, std::string url, float light_dir_x, float light_dir_y, float light_dir_z)
{
  return add_material(e, new GLTF_Material_from_file(e,ev, url, gameapi_homepageurl,Vector(light_dir_x,light_dir_y,light_dir_z)));
}

GameApi::MT GameApi::MaterialsApi::gltf_material( EveryApi &ev, TF model0, int material_id, float mix, float light_dir_x, float light_dir_y, float light_dir_z )
  {
    GLTFModelInterface *model = find_gltf(e,model0);
    std::string url = model->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material(e,ev, model, material_id,mix, Vector(light_dir_x, light_dir_y, light_dir_z));
  return add_material(e, mat);
} 

GameApi::MT GameApi::MaterialsApi::gltf_material_manual( EveryApi &ev, float mix , BM baseColor, BM metalrough, BM normaltexture, BM occlusion, BM emissive, bool baseColor_b, bool metalrough_b, bool normaltexture_b, bool occlusion_b, bool emissive_b, float roughnessfactor, float metallicfactor, float baseColor_red, float baseColor_green, float baseColor_blue, float baseColor_alpha, float occulsionstrength, float light_dir_x, float light_dir_y, float light_dir_z)
  {
    //bool is_binary=false;
    //if (int(url.size())>3) {
    //  std::string sub = url.substr(url.size()-3);
    // if (sub=="glb") is_binary=true;
    // }
    //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
    Material *mat = new GLTF_Material_manual(e,ev, mix, baseColor, metalrough, normaltexture, occlusion, emissive, baseColor_b, metalrough_b, normaltexture_b, occlusion_b, emissive_b, roughnessfactor, metallicfactor, baseColor_red, baseColor_green, baseColor_blue, baseColor_alpha, occulsionstrength, Vector(light_dir_x, light_dir_y, light_dir_z));
  return add_material(e, mat);
} 


GameApi::MT gltf_material_env2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int material_id, float mix, GameApi::BM diffuse_env, GameApi::BM specular_env, GameApi::BM bfrd)
{
  //bool is_binary=false;
  //if (int(url.size())>3) {
  //  std::string sub = url.substr(url.size()-3);
  //  if (sub=="glb") is_binary=true;
  //}
  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material_env(e,ev, interface, material_id,mix, diffuse_env, specular_env, bfrd);
  return add_material(e, mat);

}

GameApi::MT GameApi::MaterialsApi::gltf_material_env( EveryApi &ev, TF model0, int material_id, float mix, BM diffuse_env, BM specular_env, BM bfrd )
{
  GLTFModelInterface *model = find_gltf(e,model0);
  std::string url = model->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  Material *mat = new GLTF_Material_env(e,ev, model, material_id,mix, diffuse_env, specular_env, bfrd);
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
  virtual std::string name() const { return "ScaleToGLTF_p"; }

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

  virtual bool HasBatchMap() const
  {
    if (res.id==-1) { return false; }
    FaceCollection *coll = find_facecoll(env,res);
    return coll->HasBatchMap();
  }
  virtual FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    if (res.id==-1) { FaceBufferRef r; r.numfaces=0; return r; }
    FaceCollection *coll = find_facecoll(env,res);
    return coll->BatchMap(start_face,end_face);
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
    coll->Prepare();
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
  //GameApi::ML ml2 = ev.mainloop_api.print_stats(p);
  GameApi::ML ml3 = add_main_loop(e, new ScaleToGltf(e,ev,p,ml));
  return ml3;
}

struct TransformObject
{
  TransformObject() : scale_x(1.0), scale_y(1.0), scale_z(1.0),
		      rot_x(0.0), rot_y(0.0), rot_z(0.0), rot_w(0.0),
		      trans_x(0.0), trans_y(0.0), trans_z(0.0), m(Matrix::Identity()) { }
  TransformObject(const TransformObject&o) : scale_x(o.scale_x), scale_y(o.scale_y), scale_z(o.scale_z),
				       rot_x(o.rot_x), rot_y(o.rot_y), rot_z(o.rot_z), rot_w(o.rot_w),
				       trans_x(o.trans_x), trans_y(o.trans_y), trans_z(o.trans_z),
				       m(o.m) { }
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


float quar_dot(float *a0, float *a1)
{
  float res = 0.0;
  for(int i=0;i<4;i++) { res+=a0[i]*a1[i]; }
  return res;
}

float bound(float x)
{
  if (x<-1.0) x=-1.0f;
  if (x>1.0) x=1.0f;
  return x;
}

void lerp(float *v0, float *v1, float t, float *res);
void spherical_slerp(float *vk, float *vk1, float t, float *res)
{
  // This comes from gltf2.0 specification for LINEAR rotation interpolation

  float a0 = quar_dot(vk,vk1);
  float a1 = fabs(a0);
  float a2 = acos(bound(a1));
  float a = a2; //acos(fabs(quar_dot(vk,vk1)));
  if (fabs(a)<0.0001) { lerp(vk,vk1,t,res); return; }
  float s = quar_dot(vk,vk1)/fabs(quar_dot(vk,vk1));
  for(int i=0;i<4;i++) res[i] = sin(a*(1.0-t))/sin(a)*vk[i] + s*sin(a*t)/sin(a)*vk1[i];
}
void lerp(float *v0, float *v1, float t, float *res)
{
  res[0] = v1[0]*t+v0[0]*(1.0-t);
  res[1] = v1[1]*t+v0[1]*(1.0-t);
  res[2] = v1[2]*t+v0[2]*(1.0-t);
  res[3] = v1[3]*t+v0[3]*(1.0-t);
}

void quar_normalize(float *v2)
{
  float d = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]+v2[3]*v2[3]);
  v2[0]/=d;
  v2[1]/=d;
  v2[2]/=d;
  v2[3]/=d;
}
void quar_slerp(float *v0, float *v1, float alpha, float *res)
{
  float dot=quar_dot(v0,v1);
  const float DOT_TRESHOLD = 0.9995f;
  if (dot>DOT_TRESHOLD)
    {
      lerp(v0,v1,alpha,res);
      return;
    }
  if (dot<-1.0) dot=-1.0;
  if (dot>1.0) dot=1.0;
  float theta_0 = acosf(dot);
  float theta = theta_0*alpha;

  float v2[4];
  v2[0] = v1[0]-v0[0]*dot;
  v2[1] = v1[1]-v0[1]*dot;
  v2[2] = v1[2]-v0[2]*dot;
  v2[3] = v1[3]-v0[3]*dot;
  float d = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]+v2[3]*v2[3]);
  v2[0]/=d;
  v2[1]/=d;
  v2[2]/=d;
  v2[3]/=d;
  res[0] = v0[0]*cos(theta) + v2[0]*sin(theta);
  res[1] = v0[1]*cos(theta) + v2[1]*sin(theta);
  res[2] = v0[2]*cos(theta) + v2[2]*sin(theta);
  res[3] = v0[3]*cos(theta) + v2[3]*sin(theta);
}
void step_interpolate(float *a, float *b, float *res)
{
  res[0]=a[0];
  res[1]=a[1];
  res[2]=a[2];
  res[3]=a[3];
}
void fix_nan_inf(float *a, float *repl)
{
  if (std::isnan(a[0])) a[0]=repl[0];
  if (std::isnan(a[1])) a[1]=repl[1];
  if (std::isnan(a[2])) a[2]=repl[2];
  if (std::isnan(a[3])) a[3]=repl[3];
  if (std::isinf(a[0])) a[0]=repl[0];
  if (std::isinf(a[1])) a[1]=repl[1];
  if (std::isinf(a[2])) a[2]=repl[2];
  if (std::isinf(a[3])) a[3]=repl[3];
}

TransformObject slerp_transform(TransformObject o, TransformObject o2, float val)
{
  //std::cout << val << std::endl;
  //std::cout << "Input1" << std::endl;
  //print_transform(o);
  //std::cout << "Input2" << std::endl;
  //print_transform(o2);
  TransformObject res;
  res.trans_x = val*o2.trans_x + (1.0-val)*o.trans_x;
  res.trans_y = val*o2.trans_y + (1.0-val)*o.trans_y;
  res.trans_z = val*o2.trans_z + (1.0-val)*o.trans_z;
  res.scale_x = val*o2.scale_x + (1.0-val)*o.scale_x;
  res.scale_y = val*o2.scale_y + (1.0-val)*o.scale_y;
  res.scale_z = val*o2.scale_z + (1.0-val)*o.scale_z;


  //std::cout << "SCALE2:" << res.scale_x << " " << res.scale_y << " " << res.scale_z << std::endl;

  
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
  quar_normalize(prev);
  quar_normalize(next);
  spherical_slerp(prev,next,val,res2);
  //lerp(prev,next,val,res2);
  //step_interpolate(prev,next,res2);
  quar_normalize(res2);
  //fix_nan_inf(res2,next);
  res.rot_x = res2[0];
  res.rot_y = res2[1];
  res.rot_z = res2[2];
  res.rot_w = res2[3];
  
  for(int i=0;i<16;i++) res.m.matrix[i] = val*o2.m.matrix[i] + (1.0-val)*o.m.matrix[i];
  
  // FIXME (TO BE REMOVED)
  /*
  res.rot_x = val*o2.rot_x + (1.0-val)*o.rot_x;
  res.rot_y = val*o2.rot_y + (1.0-val)*o.rot_y;
  res.rot_z = val*o2.rot_z + (1.0-val)*o.rot_z;
  res.rot_w = val*o2.rot_w + (1.0-val)*o.rot_w;
  float d = sqrt(res.rot_x*res.rot_x+res.rot_y*res.rot_y+res.rot_z*res.rot_z+res.rot_w*res.rot_w);
  res.rot_x/=d;
  res.rot_y/=d;
  res.rot_z/=d;
  res.rot_w/=d;*/

  //std::cout << "output" << std::endl;
  //print_transform(res);
  return res;
}

TransformObject gltf_node_transform_obj(const tinygltf::Node *node)
{
  TransformObject o = gltf_node_default();
  if (int(node->scale.size())==3) {
    double s_x = node->scale[0];
    double s_y = node->scale[1];
    double s_z = node->scale[2];
    o.scale_x = s_x;
    o.scale_y = s_y;
    o.scale_z = s_z;
    //std::cout << "SCALE3:" << s_x << " " << s_y << " " << s_z << std::endl;

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
    const double *arr = &node->matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];
      o.m = m;
  }
  return o;
}

std::pair<Matrix,Matrix> gltf_node_transform_obj_apply(GameApi::Env &e, GameApi::EveryApi &ev, Matrix root, const TransformObject &o2)
{
  //std::cout << "obj_apply:" << std::endl;
  //print_transform(o);
  TransformObject o = o2;
  //GameApi::MN mv = ev.move_api.mn_empty();
  Matrix mv = Matrix::Identity();


  //std::cout << "quar:" << o.rot_x << " " << o.rot_y << " " << o.rot_z << " " << o.rot_w << std::endl;
  
  float d = sqrt(o.rot_x*o.rot_x+o.rot_y*o.rot_y+o.rot_z*o.rot_z +o.rot_w*o.rot_w);
  o.rot_x/=d;
  o.rot_y/=d;
  o.rot_z/=d;
  o.rot_w/=d;
  
  Quarternion q = { float(o.rot_x), float(o.rot_y), float(o.rot_z), float(o.rot_w) };
  Matrix m = Quarternion::QuarToMatrix(q);
  //std::cout << "objapply: " << m << std::endl;
  // Matrix mi = Matrix::Inverse(m);
  // Scale

  // gltf spec says the order must be scale, rotate, translate
  mv = mv * Matrix::Scale(o.scale_x, o.scale_y, o.scale_z);
  //std::cout << "Scale4: " << o.scale_x << " " << o.scale_y << " " << o.scale_z << std::endl;
  if (!Matrix::has_nan(m))
    mv = mv * m;
  //else std::cout << m << " (m) skipped in obj_apply" << std::endl;
  mv = mv * Matrix::Translate(o.trans_x, o.trans_y, o.trans_z);
  if (!Matrix::has_nan(o.m))
    mv = mv * o.m;
  //else std::cout << m << " (o.m) skipped in obj_apply" << std::endl;
  //mv = mv*root; // * root;


  //std::cout << mv << std::endl;
  
  return std::make_pair(mv,mv*root);
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
    //std::cout << "Scale5:" << s_x << " " << s_y << " " << s_z << std::endl;
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

GameApi::P gltf_mesh2_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys);
GameApi::ARR gltf_mesh2_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys,int (*fptr_mesh)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix) , float mix, Vector light_dir);

GameApi::P gltf_mesh2_with_skeleton_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys);

GameApi::ARR gltf_mesh2_with_skeleton_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys,int (*fptr)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix), float mix, Vector light_dir);


GameApi::ML gltf_mesh2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color);

GameApi::ML gltf_mesh2_with_skeleton( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color);
GameApi::MT gltf_anim_material3(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int num_timeindexes, GameApi::MT next, std::string keys, int mode,int inst);


bool is_child_node(GLTFModelInterface *interface, int node_id, int node2)
{
  return true;
}

Matrix fix_matrix(Matrix m);


GameApi::P gltf_node2_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,  int node_id, std::string keys)
{
  //if (!load2) load2 = load;
  int s2 = interface->nodes_size(); //load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    return empty;
    }
  // HERE WE HAVE NOT CALLED PREPARE?
  const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];

  // Load mesh
  int ss = interface->skins_size(); //load->model.skins.size();
  bool done = false;
  GameApi::P mesh;
  for(int i=0;i<ss;i++) {
    //if (interface->get_skin(i).skeleton != -1)
      {
	int mesh_id = node.mesh;
	mesh.id = -1;
	if (mesh_id != -1) {
	  mesh = gltf_mesh2_with_skeleton_p(e,ev,interface, mesh_id, i,keys);
	  done = true;
	}
	if (done)
	  break;
      }
    
  }
  if (!done) {
    int mesh_id = node.mesh;
    mesh.id = -1;
    if (mesh_id!=-1) {
      mesh = gltf_mesh2_p( e, ev, interface, mesh_id, 0, keys);
    }
  }
  // todo cameras

  // recurse children
  int s = node.children.size();
  std::vector<GameApi::P> vec;
  for(int i=0;i<s;i++) {
    int child_id = node.children[i];
    if (child_id!=-1) {
      //std::cout << "{";
      GameApi::P ml = gltf_node2_p( e, ev, interface, child_id,keys);
      vec.push_back(ml);
      //std::cout << "}";
    }
  }
  if (mesh.id != -1) {
    vec.push_back( mesh );
  }

  /*
 GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);
 GameApi::MN mv = ev.move_api.mn_empty();
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, o2.first);
    mv = add_move(e, mv2);    

 GameApi::MN mv3 = ev.move_api.mn_empty();
    Movement *orig2 = find_move(e, mv3);
    Movement *mv22 = new MatrixMovement(orig2, o2.second);
    mv3 = add_move(e, mv22);
  */
    
  
  
  
  GameApi::P array;
  // this is perf/memory optimization.
  if (vec.size()==1) array=vec[0]; else
  if (vec.size()==2) array=ev.polygon_api.or_elem(vec[0],vec[1]);
  else array = ev.polygon_api.or_array3(vec);
 
  if (int(node.scale.size())==3) {
    double s_x = node.scale[0];
    double s_y = node.scale[1];
    double s_z = node.scale[2];
    array = ev.polygon_api.scale(array, s_x,s_y,s_z);
    //std::cout << "Scale6:" << s_x << " " << s_y << " " << s_z << std::endl;
  }
  if (int(node.rotation.size())==4) {
    double r_x = node.rotation[0];
    double r_y = node.rotation[1];
    double r_z = node.rotation[2];
    double r_w = node.rotation[3];
    Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
    Matrix m = Quarternion::QuarToMatrix(q);
    GameApi::M mm = add_matrix2(e,m);
    array = ev.polygon_api.matrix(array,mm);
    }
  if (int(node.translation.size())==3) {
    double m_x = node.translation[0];
    double m_y = node.translation[1];
    double m_z = node.translation[2];
    array=ev.polygon_api.translate(array,m_x,m_y,m_z);
  }
  //std::cout << node->matrix.size();
  if (int(node.matrix.size())==16) {
    const double *arr = &node.matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];

      GameApi::M mm = add_matrix2(e,m);
      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
      array=ev.polygon_api.matrix(array,mm);
  }
  
  /*
  GameApi::ML ret2 = ev.move_api.move_ml(ev, mesh, mv3, 1, 10.0 );
  GameApi::ML ret_arr = ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{ret,ret2});
  */
  return array;
}


GameApi::ARR gltf_node2_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,  int node_id, std::string keys, int (*fptr_skeleton)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix),int (*fptr_mesh)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix), bool translate, float mix, Vector light_dir)
{
  //if (!load2) load2 = load;
  int s2 = interface->nodes_size(); //load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
      //std::cout << "Empty node" << std::endl;
      ArrayType *t = new ArrayType;
      t->type=0;
      t->vec=std::vector<int>();
      return add_array(e,t);
    }
  // HERE WE HAVE NOT CALLED PREPARE?
  const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];

  // Load mesh
  int ss = interface->skins_size(); //load->model.skins.size();
  bool done = false;
  GameApi::ARR mesh;
  for(int i=0;i<ss;i++) {
    //if (interface->get_skin(i).skeleton != -1)
      {
	int mesh_id = node.mesh;
	mesh.id = -1;
	if (mesh_id != -1) {
	  mesh = gltf_mesh2_with_skeleton_p_arr(e,ev,interface, mesh_id, i,keys,fptr_skeleton, mix, light_dir);
	  done = true;
	}
	if (done)
	  break;
      }
    
  }
  if (!done) {
    int mesh_id = node.mesh;
    mesh.id = -1;
    if (mesh_id!=-1) {
      mesh = gltf_mesh2_p_arr( e, ev, interface, mesh_id, 0, keys,fptr_mesh,mix, light_dir);
    }
  }
  // todo cameras

  ArrayType *res_t2 = new ArrayType;
  ArrayType *res_t = find_array(e,mesh);
  if (!res_t||mesh.id==-1) {
      res_t = new ArrayType;
      res_t->type=0;
      res_t->vec=std::vector<int>();
  }
  // recurse children
  int s = node.children.size();
  std::vector<GameApi::P> vec;
  for(int i=0;i<s;i++) {
    int child_id = node.children[i];
    if (child_id!=-1) {
      GameApi::ARR ml = gltf_node2_p_arr( e, ev, interface, child_id,keys,fptr_skeleton,fptr_mesh,translate,mix,light_dir);
      ArrayType *t = find_array(e,ml);
      int s3 = t->vec.size();
      for(int j=0;j<s3;j++) {
	res_t2->vec.push_back(t->vec[j]);
      }
    }
  }
  if (mesh.id != -1) {
    ArrayType *t = find_array(e, mesh);
    int s = res_t->vec.size();
    for(int i=0;i<s;i++) res_t2->vec.push_back(res_t->vec[i]);
    //if (translate)
    //  res_t->vec.push_back(mesh.id);
    //else
    //  res_t->vec.push_back(-1 );
  }

  int s4 = res_t2->vec.size();
  for(int i=0;i<s4;i++) {
    GameApi::P array;
    array.id = res_t2->vec[i];
    //std::cout << translate << " " << array.id << std::endl;
    if (translate) {
  if (int(node.scale.size())==3) {
    double s_x = node.scale[0];
    double s_y = node.scale[1];
    double s_z = node.scale[2];
    array = ev.polygon_api.scale(array, s_x,s_y,s_z);
    //std::cout << "Scale7:" << s_x << " " << s_y << " " << s_z << std::endl;
  }
  if (int(node.rotation.size())==4) {
    double r_x = node.rotation[0];
    double r_y = node.rotation[1];
    double r_z = node.rotation[2];
    double r_w = node.rotation[3];
    Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
    Matrix m = Quarternion::QuarToMatrix(q);
    GameApi::M mm = add_matrix2(e,m);
    array = ev.polygon_api.matrix(array,mm);
    }
  if (int(node.translation.size())==3) {
    double m_x = node.translation[0];
    double m_y = node.translation[1];
    double m_z = node.translation[2];
    array=ev.polygon_api.translate(array,m_x,m_y,m_z);
  }
  if (int(node.matrix.size())==16) {
    const double *arr = &node.matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];

      GameApi::M mm = add_matrix2(e,m);
      array=ev.polygon_api.matrix(array,mm);
  }
    }
  res_t2->vec[i]=array.id;
  }
  

  return add_array(e,res_t2);
}



GameApi::ML gltf_node2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,  int node_id, std::string keys, float mix, Matrix root, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color)
{
  //if (!load2) load2 = load;
  int s2 = interface->nodes_size(); //load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  // HERE WE HAVE NOT CALLED PREPARE?
  const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];

  // Load mesh
  int ss = interface->skins_size(); //load->model.skins.size();
  bool done = false;
  GameApi::ML mesh;
  for(int i=0;i<ss;i++) {
    //if (interface->get_skin(i).skeleton != -1)
      {
	int mesh_id = node.mesh;
	mesh.id = -1;
	if (mesh_id != -1) {
	  mesh = gltf_mesh2_with_skeleton(e,ev,interface, mesh_id, i,keys,mix,mode, light_dir, animation, border_width, border_color);
	  done = true;
	}
	if (done)
	  break;
      }
    
  }
  if (!done) {
    int mesh_id = node.mesh;
    mesh.id = -1;
    if (mesh_id!=-1) {
      mesh = gltf_mesh2( e, ev, interface, mesh_id, 0, keys,mix,mode, light_dir ,animation, border_width, border_color);
    }
  }
  // todo cameras

  TransformObject o = gltf_node_transform_obj(&node);
  std::pair<Matrix,Matrix> o2 = gltf_node_transform_obj_apply(e,ev, root,o);
  // recurse children
  int s = node.children.size();
  std::vector<GameApi::ML> vec;
  //std::cout << "CHILDREN:";
  //for(int i=0;i<s;i++) {
  //  int child_id = node.children[i];
  //  std::cout << child_id << ",";
  // }
  //std::cout << std::endl;
  for(int i=0;i<s;i++) {
    int child_id = node.children[i];
    if (child_id!=-1) {
      //std::cout << "{";
      GameApi::ML ml = gltf_node2( e, ev, interface, child_id,keys,mix,o2.second,mode, light_dir, animation, border_width, border_color );
      vec.push_back(ml);
      //std::cout << "}";
    }
  }
  if (mesh.id != -1) {
    vec.push_back( mesh );
    //std::cout << "MESH";
  }

  /*
 GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);
 GameApi::MN mv = ev.move_api.mn_empty();
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, o2.first);
    mv = add_move(e, mv2);    

 GameApi::MN mv3 = ev.move_api.mn_empty();
    Movement *orig2 = find_move(e, mv3);
    Movement *mv22 = new MatrixMovement(orig2, o2.second);
    mv3 = add_move(e, mv22);
  */
    
  
  
  
 GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);
 GameApi::MN mv = ev.move_api.mn_empty();
 
  if (int(node.scale.size())==3) {
    double s_x = node.scale[0];
    double s_y = node.scale[1];
    double s_z = node.scale[2];
    mv = ev.move_api.scale2(mv, s_x, s_y, s_z);
    //std::cout << "sc[" << s_x << "," << s_y << "," << s_z << "]";
    //std::cout << "Scale8:" << s_x << " " << s_y << " " << s_z << std::endl;
  }
  if (int(node.rotation.size())==4) {
    double r_x = node.rotation[0];
    double r_y = node.rotation[1];
    double r_z = node.rotation[2];
    double r_w = node.rotation[3];
    //std::cout << "rot[" << r_x << "," << r_y << "," << r_z << "," << r_w << "]";
    Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
    Matrix m = Quarternion::QuarToMatrix(q);
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(e, mv2);
    }
  if (int(node.translation.size())==3) {
    double m_x = node.translation[0];
    double m_y = node.translation[1];
    double m_z = node.translation[2];
    mv = ev.move_api.trans2(mv, m_x, m_y, m_z);
    //std::cout << "tr[" << m_x << "," << m_y << "," << m_z << "]";
  }
  //std::cout << node->matrix.size();
  if (int(node.matrix.size())==16) {
    const double *arr = &node.matrix[0];
    Matrix m;
      for(int i=0;i<4;i++)
      for(int j=0;j<4;j++) m.matrix[i*4+j] = (float)arr[j*4+i];
      //std::cout << "mat[]";

      // for(int i=0;i<16;i++) m.matrix[i] = (float)arr[i];
    Movement *orig = find_move(e, mv);
    Movement *mv2 = new MatrixMovement(orig, m);
    mv = add_move(e, mv2);    
  }
  
  GameApi::ML ret = ev.move_api.move_ml(ev, array, mv, 1, 10.0 );
  /*
  GameApi::ML ret2 = ev.move_api.move_ml(ev, mesh, mv3, 1, 10.0 );
  GameApi::ML ret_arr = ev.mainloop_api.array_ml(ev,std::vector<GameApi::ML>{ret,ret2});
  */
  return ret;
}

GameApi::P gltf_scene2_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, std::string keys )
{
  int s2 = interface->scenes_size(); //load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    return empty;
    }
  const tinygltf::Scene &scene = interface->get_scene(scene_id); //&load->model.scenes[scene_id];
  int s = scene.nodes.size();
  std::vector<GameApi::P> vec;
  for(int i=0;i<s;i++) {
    GameApi::P ml = gltf_node2_p( e, ev, interface, scene.nodes[i], keys);
    vec.push_back(ml);
  }
  return ev.polygon_api.or_array2( vec);
}


GameApi::ARR gltf_scene2_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, std::string keys,int (*fptr_skeleton)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix),int (*fptr_mesh)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix), bool translate, float mix,Vector light_dir )
{
  int s2 = interface->scenes_size(); //load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
      //std::cout << "Empty scene" << std::endl;
      ArrayType *array = new ArrayType;
      array->type=0;
      array->vec = std::vector<int>();
      return add_array(e,array);
    }

  const tinygltf::Scene &scene = interface->get_scene(scene_id); //&load->model.scenes[scene_id];
  int s = scene.nodes.size();
  std::vector<int> vec;
  for(int i=0;i<s;i++) {
    GameApi::ARR ml = gltf_node2_p_arr( e, ev, interface, scene.nodes[i], keys, fptr_skeleton, fptr_mesh,translate, mix, light_dir);
    ArrayType *array = find_array(e,ml);
    int s2 = array->vec.size();
    for(int j=0;j<s2;j++)
      {
	int id = array->vec[j];
	vec.push_back(id);
      }
  }
  ArrayType *array = new ArrayType;
  array->type=0;
  array->vec = vec;
  return add_array(e,array);
}


GameApi::ML gltf_scene2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color )
{
  int s2 = interface->scenes_size(); //load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  const tinygltf::Scene &scene = interface->get_scene(scene_id); //&load->model.scenes[scene_id];
  int s = scene.nodes.size();
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_node2( e, ev, interface, scene.nodes[i], keys,mix,Matrix::Identity(),mode,light_dir,animation, border_width, border_color );
    vec.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, vec);
}

GameApi::P gltf_mesh2_with_skeleton_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys)
{
  //g_last_resize=Matrix::Identity();
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<GameApi::P> mls;
    for(int i=0;i<s;i++) {
      GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      mls.push_back(p);
    }
    GameApi::P ml = ev.polygon_api.or_array3(mls);
    return ml;
  } else {
    GameApi::P empty = ev.polygon_api.p_empty();
    return empty;
  }
}

int arr_fetch_load(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix)
{
  GameApi::P p = gltf_load2(e,ev, interface, mesh_id, i);
  return p.id;
}

Vector g_light_dir = Vector(0.0,0.0,-400.0);

int arr_fetch_material(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix)
{
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
  int mat = m.primitives[i].material;
  GameApi::MT mat2 = gltf_material2(e, ev, interface, mat, mix, g_light_dir);
      Material *mat0 = find_material(e,mat2);
      GLTF_Material *mat3 = (GLTF_Material*)mat0;
      GameApi::BM bm = mat3->texture(0); // basecolor
      bool is_transparent = mat3->IsTransparent();
      GameApi::MT mat4 = ev.materials_api.transparent_material(ev,bm, mat2, is_transparent);
      return mat2.id; // TEST REMOVED TRANSPARENCY
  return mat4.id;
}

GameApi::ARR gltf_mesh2_with_skeleton_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, int (*fptr)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix), float mix, Vector light_dir)
{
  g_light_dir = light_dir;
  //g_last_resize=Matrix::Identity();
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<int> mls;
    for(int i=0;i<s;i++) {
      //GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      int id = fptr(e,ev,interface,mesh_id,i,mix);
      mls.push_back(id);
    }
    ArrayType *t = new ArrayType;
    t->type=0;
    t->vec = mls;
    return add_array(e,t);
  } else {
    ArrayType *t = new ArrayType;
    t->type=0;
    t->vec = std::vector<int>();
    return add_array(e,t);
  }
}


int gltf_mesh2_calc_max_timeindexes(GLTFModelInterface *interface, int animation, int skin_num);


std::vector<GLTFModelInterface*> g_interface_id;

int get_cache_id(GLTFModelInterface *interface)
{
  int s = g_interface_id.size();
  for(int i=0;i<s;i++) if (g_interface_id[i]==interface) return i;
  g_interface_id.push_back(interface);
  return g_interface_id.size()-1;
}



GameApi::ML gltf_mesh2_with_skeleton( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color)
{
  int cache_id = get_cache_id(interface);
  //std::cout << "MESH2: " << mesh_id << std::endl;
  //g_last_resize=Matrix::Identity();
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<GameApi::ML> mls;
    for(int i=0;i<s;i++) {
      //std::cout << "skeleton:" << mesh_id << " " << i << std::endl;
      GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      int mat = m.primitives[i].material;
      GameApi::MT mat2 = gltf_material2(e, ev, interface, mat, 1.0,light_dir);
      GameApi::MT mat3 = border_width>=0.5?ev.materials_api.toon_border(ev,mat2,border_width,border_color,true):mat2;

      GameApi::MT mat2_anim;
      if (interface->animations_size()!=0 && keys.size()>0) {
	//int ssz = interface->animations_size();
	int maxtimeindexes=0;
	//for(int iu=0;iu<ssz;iu++) {
	int iu = animation;
	  int mm = gltf_mesh2_calc_max_timeindexes(interface,iu,skin_id);
	  if (mm>maxtimeindexes) maxtimeindexes=mm;
	  //}
	mat2_anim= gltf_anim_material3(e,ev, interface, skin_id, maxtimeindexes, mat3, keys,mode,mesh_id+10*i+100*cache_id);
      } else
	{
	  mat2_anim = mat3;
	}
      Material *mat0 = find_material(e,mat2);
      GLTF_Material *mat33 = (GLTF_Material*)mat0;
      GameApi::BM bm = mat33->texture(0); // basecolor
      bool is_transparent = mat33->IsTransparent();
      GameApi::MT mat4 = ev.materials_api.transparent_material(ev,bm, mat2_anim,is_transparent);


      //GameApi::ML ml = ev.materials_api.bind(p,mat2_anim); // TEST, REMOVED TRANSPARENCY
      GameApi::ML ml = ev.materials_api.bind(p,mat4);

      GameApi::ML ml2=ev.mainloop_api.depthmask(ml,true);
      GameApi::ML ml3=ev.mainloop_api.depthfunc(ml2,3);
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

GameApi::P gltf_mesh2_p( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys )
{
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<GameApi::P> mls;
    for(int i=0;i<s;i++) {
      GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      mls.push_back(p);
    }
    GameApi::P ml = ev.polygon_api.or_array3( mls);
    return ml;
  } else {
    GameApi::P empty = ev.polygon_api.p_empty();
    return empty;
  }

}

GameApi::ARR gltf_mesh2_p_arr( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys,int (*fptr_mesh)(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface,int mesh_id, int i, float mix), float mix, Vector light_dir )
{
  g_light_dir = light_dir;
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<int> mls;
    for(int i=0;i<s;i++) {
      //GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      int id = fptr_mesh(e,ev,interface,mesh_id,i, mix);
      mls.push_back(id);
    }
    ArrayType *t = new ArrayType;
    t->type=0;
    t->vec = mls;
    //GameApi::P ml = ev.polygon_api.or_array3( mls);
    //return ml;
    return add_array(e,t);
  } else {
    ArrayType *t = new ArrayType;
    t->type=0;
    t->vec = std::vector<int>();
    return add_array(e,t);
  }

}



GameApi::ML gltf_mesh2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color)
{
  int cache_id = get_cache_id(interface);
  
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<GameApi::ML> mls;
    for(int i=0;i<s;i++) {
      // std::cout << "mesh2:" << mesh_id << " " << i << std::endl;
      GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      int mat = m.primitives[i].material;
      GameApi::MT mat2 = gltf_material2(e, ev, interface, mat, mix,light_dir);
      GameApi::MT mat3 = border_width>=0.5?ev.materials_api.toon_border(ev,mat2,border_width,border_color,true):mat2;

      GameApi::MT mat2_anim;
      if (interface->animations_size()!=0 && keys.size()>0) {
	int ssz = interface->animations_size();
	int maxtimeindexes = 0;
	//for(int iu=0;iu<ssz;iu++) {
	int iu = animation;
	  int mm = gltf_mesh2_calc_max_timeindexes(interface,iu,skin_id);
	  if (mm>maxtimeindexes) maxtimeindexes=mm;
	  //}
	mat2_anim = gltf_anim_material3(e,ev, interface, skin_id, maxtimeindexes, mat3, keys,mode,mesh_id+10*i+cache_id*100);
      } else { mat2_anim=mat3; }

      Material *mat0 = find_material(e,mat2);
      GLTF_Material *mat33 = (GLTF_Material*)mat0;
      GameApi::BM bm = mat33->texture(0); // basecolor
      bool is_transparent = mat33->IsTransparent();
      GameApi::MT mat4 = ev.materials_api.transparent_material(ev,bm, mat2_anim,is_transparent);
      //GameApi::ML ml = ev.materials_api.bind(p,mat2_anim); // TEST, REMOVED TRANSPARENCY

      GameApi::ML ml = ev.materials_api.bind(p,mat4);
      GameApi::ML ml2=ev.mainloop_api.depthmask(ml,true);
      GameApi::ML ml3=ev.mainloop_api.depthfunc(ml2,3);
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

GameApi::ML gltf_mesh2_env( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, GameApi::BM diffuse, GameApi::BM specular, GameApi::BM brfd, float mix)
{
  if (mesh_id>=0 && mesh_id<int(interface->meshes_size())) {
    const tinygltf::Mesh &m = interface->get_mesh(mesh_id);
    int s = m.primitives.size();
    std::vector<GameApi::ML> mls;
    for(int i=0;i<s;i++) {
      GameApi::P p = gltf_load2(e, ev, interface, mesh_id, i);
      int mat = m.primitives[i].material;
      GameApi::MT mat2 = gltf_material_env2(e, ev, interface, mat, 1.0, diffuse, specular, brfd);
      Material *mat0 = find_material(e,mat2);
      GLTF_Material *mat3 = (GLTF_Material*)mat0;
      GameApi::BM bm = mat3->texture(0); // basecolor
      bool is_transparent = mat3->IsTransparent();
      GameApi::MT mat4 = ev.materials_api.transparent_material(ev,bm, mat2,is_transparent);
      //GameApi::ML ml = ev.materials_api.bind(p,mat2); // TEST, REMOVED TRANSPARENCY
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
  GltfMesh(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_id, int skin_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color)
    :env(env), ev(ev), interface(interface),mesh_id(mesh_id),skin_id(skin_id), keys(keys),mix(mix),mode(mode),light_dir(light_dir),animation(animation),border_width(border_width), border_color(border_color) { res.id=-1; }


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  interface->Prepare();
  GameApi::P mesh = gltf_mesh2_p(env,ev,interface, mesh_id, skin_id, keys); //env,ev, interface, 0,0);

  GameApi::ML ml = gltf_mesh2(env,ev,interface, mesh_id, skin_id, keys,mix,mode,light_dir,animation, border_width, border_color);
  res = scale_to_gltf_size(env,ev,mesh,ml);
    
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  int mesh_id;
  int skin_id;
  std::string keys;
  float mix;
  int mode;
  Vector light_dir;
  int animation;
  float border_width;
  unsigned int border_color;
};

GameApi::ML GameApi::MainLoopApi::gltf_mesh( GameApi::EveryApi &ev, TF model0, int mesh_id, int skin_id, std::string keys, float mix,int mode, float light_dir_x, float light_dir_y, float light_dir_z, int animation, float border_width, unsigned int border_color )
{

  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfMesh(e,ev,interface,mesh_id, skin_id, keys,mix, mode,Vector(light_dir_x,light_dir_y,light_dir_z),animation, border_width, border_color));
}

class GltfNode : public MainLoopItem
{
public:
  GltfNode(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int node_id, std::string keys, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color)
    :env(env), ev(ev), interface(interface),node_id(node_id),keys(keys),mix(mix), mode(mode),light_dir(light_dir),animation(animation),border_width(border_width), border_color(border_color) { res.id=-1; }


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  // new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  interface->Prepare();
  GameApi::P mesh = gltf_node2_p(env,ev,interface,node_id,keys); //env,ev, interface, 0,0);
  GameApi::ML ml = gltf_node2(env,ev,interface,node_id,keys,mix,Matrix::Identity(), mode, light_dir, animation,border_width, border_color);
  res = scale_to_gltf_size(env,ev,mesh,ml);

    
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  int node_id;
  std::string keys;
  float mix;
  int mode;
  Vector light_dir;
  int animation;
  float border_width;
  unsigned int border_color;
};

GameApi::ML GameApi::MainLoopApi::gltf_node( GameApi::EveryApi &ev, TF model0, int node_id, std::string keys, float mix,int mode, float light_x, float light_y, float light_z, int animation, float border_width, unsigned int border_color )
{

  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e,new GltfNode(e,ev,interface,node_id,keys,mix,mode,Vector(light_x,light_y,light_z),animation,border_width,border_color));
}


class GltfScene : public MainLoopItem
{
public:
  GltfScene(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, std::string keys, float mix, int mode,Vector light_dir, int animation, float border_width, unsigned int border_color)
    :env(env), ev(ev), interface(interface),scene_id(scene_id),keys(keys),mix(mix),mode(mode),light_dir(light_dir),animation(animation),border_width(border_width), border_color(border_color) { res.id=-1;}


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  interface->Prepare();
  GameApi::P mesh = gltf_scene2_p(env,ev,interface,scene_id,keys); //env,ev, interface, 0,0);
  GameApi::ML ml = gltf_scene2(env,ev,interface,scene_id,keys,mix,mode,light_dir,animation, border_width, border_color);
  res= scale_to_gltf_size(env,ev,mesh,ml);    
  MainLoopItem *item = find_main_loop(env,res);
  item->Prepare();
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  int scene_id;
  std::string keys;
  float mix;
  int mode;
  Vector light_dir;
  int animation;
  float border_width;
  unsigned int border_color;
};

GameApi::ML GameApi::MainLoopApi::gltf_scene( GameApi::EveryApi &ev, TF model0, int scene_id, std::string keys, float mix, int mode, float light_dir_x, float light_dir_y, float light_dir_z, int animation, float border_width, unsigned int border_color )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfScene(e,ev,interface,scene_id,keys,mix,mode,Vector(light_dir_x, light_dir_y, light_dir_z),animation,border_width,border_color));
  //return ml;
}



GameApi::ML gltf_mesh_all2( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, float mix, int mode, Vector light_dir, int animation, float border_width, unsigned int border_color )
{
  int s = interface->meshes_size(); //load->model.meshes.size();
  std::vector<GameApi::ML> mls;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_mesh2( e, ev, interface, i, 0, "",mix,mode,light_dir,animation, border_width, border_color );
    mls.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, mls);
}

GameApi::ML gltf_mesh_all2_env( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, GameApi::BM diffuse, GameApi::BM specular, GameApi::BM brfd, float mix )
{
  int s = interface->meshes_size(); //load->model.meshes.size();
  std::vector<GameApi::ML> mls;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_mesh2_env( e, ev, interface, i, diffuse,specular,brfd,mix );
    mls.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, mls);
}

class GltfMeshAll : public MainLoopItem
{
public:
  GltfMeshAll(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, float mix, int mode, std::string keys, Vector light_dir, float border_width, unsigned int border_color)
    :env(env), ev(ev), interface(interface),mix(mix),mode(mode),keys(keys),light_dir(light_dir),border_width(border_width), border_color(border_color) { res.id = -1;}


  virtual void Collect(CollectVisitor &vis) {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  virtual bool ReadyToPrepare() const { return interface->ReadyToPrepare(); }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
    bool is_binary=false;
    if (int(url.size())>3) {
      std::string sub = url.substr(url.size()-3);
      if (sub=="glb") is_binary=true;
    }
    // LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
    //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
    interface->Prepare();
    int scene_id = interface->get_default_scene();
    GameApi::P mesh = gltf_scene2_p(env, ev, interface,scene_id,"");

    GameApi::ML ml = gltf_scene2( env, ev, interface,scene_id,keys,mix,mode,light_dir,0,border_width,border_color ); // 0 = take numtimeindexes from first animation
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  float mix;
  int mode;
  std::string keys;
  Vector light_dir;
  float border_width;
  unsigned int border_color;
};

class GltfMeshAllP : public FaceCollection
{
public:
  GltfMeshAllP(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface) : env(env), ev(ev), interface(interface) { res.id=-1;}
  virtual std::string name() const { return "GltfMeshAllP"; }
  virtual void Collect(CollectVisitor &vis) {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    std::string url = interface->Url();
    bool is_binary=false;
    if (int(url.size())>3) {
      std::string sub = url.substr(url.size()-3);
      if (sub=="glb") is_binary=true;
    }
    // LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
    //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
    //interface->Prepare();
    //GameApi::P mesh = gltf_load2(env,ev, interface, 0,0);

    int scene_id = interface->get_default_scene();
    GameApi::P p = gltf_scene2_p( env, ev, interface,scene_id,"");
    res = scale_to_gltf_size_p(env,ev,p,p);

    if (res.id!=-1) {
      FaceCollection *item = find_facecoll(env,res);
      if (item)
	item->Prepare();
    }    //    Prepare();
  }
  virtual void Prepare() {
    interface->Prepare();
    HeavyPrepare();
  }

  virtual int NumFaces() const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const {
      if (point==0&&cache_0&&cache_face_0==face) { return cache_res_0; }
      if (point==1&&cache_1&&cache_face_1==face) { return cache_res_1; }
      if (point==2&&cache_2&&cache_face_2==face) { return cache_res_2; }
      if (point==3&&cache_3&&cache_face_3==face) { return cache_res_3; }
    if (res.id==-1) return Point(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return Point(0.0,0.0,0.0);
    Point p = coll->FacePoint(face,point);
    if (point==0) { cache_res_0=p; cache_0=true; cache_face_0=face;}
    if (point==1) { cache_res_1=p; cache_1=true; cache_face_1=face;}
    if (point==2) { cache_res_2=p; cache_2=true; cache_face_2=face;}
    if (point==3) { cache_res_3=p; cache_3=true; cache_face_3=face;}
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (res.id==-1) return Vector(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return Vector(0.0,0.0,0.0);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (res.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0.0;
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (res.id==-1) return 0xffffffff;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0xffffffff;
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (res.id==-1) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    return coll->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const
  {
    if (res.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0.0;
    return coll->TexCoord3(face,point);
  }
  virtual VEC4 Joints(int face, int point) const {
    if (res.id==-1) { VEC4 v; return v; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { VEC4 v; return v; }
    return coll->Joints(face,point);
  }
  virtual VEC4 Weights(int face, int point) const {
    if (res.id==-1) { VEC4 v; return v; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { VEC4 v; return v; }
    return coll->Weights(face,point);
  }
  virtual int NumObjects() const {
    if (res.id==-1) return 0;

    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumObjects();
  }
  virtual std::pair<int,int> GetObject(int o) const {
    if (res.id==-1) { return std::make_pair(0,0); }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return std::make_pair(0,0); }
    return coll->GetObject(o);
  }
  virtual int NumTextures() const {
    if (res.id==-1) { return 0; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return 0; }
    return coll->NumTextures();
  }
  virtual void GenTexture(int num) {
    if (res.id==-1) { return; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return; }
    return coll->GenTexture(num);

  }
  virtual BufferRef TextureBuf(int num) const {
    if (res.id==-1) { BufferRef r; return r; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { BufferRef r; return r; }
    return coll->TextureBuf(num);
  }
  virtual int FaceTexture(int face) const {
    if (res.id==-1) { return 0; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return 0; }
    return coll->FaceTexture(face);
  }
  virtual bool HasBatchMap() const
  {
    if (res.id==-1) { return false; }
    FaceCollection *coll = find_facecoll(env,res);
    return coll->HasBatchMap();
  }
  virtual FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    if (res.id==-1) { FaceBufferRef r; r.numfaces=0; return r; }
    FaceCollection *coll = find_facecoll(env,res);
    return coll->BatchMap(start_face,end_face);
  }
  

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GLTFModelInterface *interface;
  GameApi::P res;
private:
  mutable int cache_face_0=-1;
  mutable int cache_face_1=-1;
  mutable int cache_face_2=-1;
  mutable int cache_face_3=-1;
  mutable Point cache_res_0; mutable bool cache_0=false;
  mutable Point cache_res_1; mutable bool cache_1=false;
  mutable Point cache_res_2; mutable bool cache_2=false;
  mutable Point cache_res_3; mutable bool cache_3=false;
  
  };
  
GameApi::P GameApi::MainLoopApi::gltf_mesh_all_p( GameApi::EveryApi &ev, TF model0)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_polygon2(e, new GltfMeshAllP(e,ev,interface),1);
}

class GltfMeshAllPArr : public FaceCollection
{
public:
  GltfMeshAllPArr(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int i, Vector light_dir) : env(env), ev(ev), interface(interface),i(i),light_dir(light_dir) { res.id=-1;}
  virtual std::string name() const { return "GltfMeshAllPArr"; }

  virtual void Collect(CollectVisitor &vis) {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    if (firsttime) {
      interface->Prepare();
    std::string url = interface->Url();
    bool is_binary=false;
    if (int(url.size())>3) {
      std::string sub = url.substr(url.size()-3);
      if (sub=="glb") is_binary=true;
    }
    if (g_cache.find(interface)==g_cache.end())
      {
	int c = get_current_block();
	set_current_block(-2);

	int scene_id = interface->get_default_scene();
	GameApi::ARR p = gltf_scene2_p_arr( env, ev, interface,scene_id,"", &arr_fetch_load, &arr_fetch_load,true, 1.0, light_dir);
	ArrayType *t = find_array(env,p);
	std::vector<GameApi::P> vv;
	int ss2 = t->vec.size();
	for(int i=0;i<ss2;i++) { GameApi::P p; p.id = t->vec[i]; if (p.id!=-1) vv.push_back(p); }
	GameApi::P p2 = ev.polygon_api.or_array3(vv);
	  //add_polygon2(env, new GltfMeshAllP(env, ev, interface ),1);
	g_cache[interface]=p;
	g_cache2[interface]=p2;
	set_current_block(c);
      }
    GameApi::ARR p = g_cache[interface];
    GameApi::P p2 = g_cache2[interface];
    //std::cout << "TT:" << p.id << " " << p2.id << std::endl;
    ArrayType *t = find_array(env,p);

    //int s = t->vec.size();
    //for(int k=0;k<s;k++) std::cout << t->vec[k] << ",";
    //std::cout << std::endl;

    
    //std::cout << "TT:" << i << " " << t->vec.size() << std::endl;
    if (i>=t->vec.size()) { /*std::cout << "PArr empty at "<< i << std::endl;*/ return; }
    GameApi::P p3;
    p3.id = t->vec[i];
    //std::cout << "CHOSEN: " <<i << " " << p3.id <<std::endl;
    res = scale_to_gltf_size_p(env,ev,p2,p3);
   
    
    if (res.id!=-1) {
      FaceCollection *item = find_facecoll(env,res);
      if (item)
	item->Prepare();

      //std::cout << "NUMFACES:" << item->NumFaces() << std::endl; 
      //if (item->NumFaces()<4) { res.id=-1; }
    }    //    Prepare();
    }
  }
  virtual void Prepare() {
    interface->Prepare();
    HeavyPrepare();
  }

  virtual int NumFaces() const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const {
      if (point==0&&cache_0&&cache_face_0==face) { return cache_res_0; }
      if (point==1&&cache_1&&cache_face_1==face) { return cache_res_1; }
      if (point==2&&cache_2&&cache_face_2==face) { return cache_res_2; }
      if (point==3&&cache_3&&cache_face_3==face) { return cache_res_3; }
    if (res.id==-1) return Point(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return Point(0.0,0.0,0.0);
    Point p = coll->FacePoint(face,point);
    if (point==0) { cache_res_0=p; cache_0=true; cache_face_0=face;}
    if (point==1) { cache_res_1=p; cache_1=true; cache_face_1=face;}
    if (point==2) { cache_res_2=p; cache_2=true; cache_face_2=face;}
    if (point==3) { cache_res_3=p; cache_3=true; cache_face_3=face;}
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (res.id==-1) return Vector(0.0,0.0,0.0);
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return Vector(0.0,0.0,0.0);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (res.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0.0;
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (res.id==-1) return 0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (res.id==-1) return 0xffffffff;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0xffffffff;
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (res.id==-1) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    return coll->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const
  {
    if (res.id==-1) return 0.0;
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0.0;
    return coll->TexCoord3(face,point);
  }
  virtual VEC4 Joints(int face, int point) const {
    if (res.id==-1) { VEC4 v; return v; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { VEC4 v; return v; }
    return coll->Joints(face,point);
  }
  virtual VEC4 Weights(int face, int point) const {
    if (res.id==-1) { VEC4 v; return v; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { VEC4 v; return v; }
    return coll->Weights(face,point);
  }
  virtual int NumObjects() const {
    if (res.id==-1) return 0;

    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) return 0;
    return coll->NumObjects();
  }
  virtual std::pair<int,int> GetObject(int o) const {
    if (res.id==-1) { return std::make_pair(0,0); }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return std::make_pair(0,0); }
    return coll->GetObject(o);
  }
  virtual int NumTextures() const {
    if (res.id==-1) { return 0; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return 0; }
    return coll->NumTextures();
  }
  virtual void GenTexture(int num) {
    if (res.id==-1) { return; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return; }
    return coll->GenTexture(num);

  }
  virtual BufferRef TextureBuf(int num) const {
    if (res.id==-1) { BufferRef r; return r; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { BufferRef r; return r; }
    return coll->TextureBuf(num);
  }
  virtual int FaceTexture(int face) const {
    if (res.id==-1) { return 0; }
    FaceCollection *coll = find_facecoll(env,res);
    if (!coll) { return 0; }
    return coll->FaceTexture(face);
  }



private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GLTFModelInterface *interface;
  GameApi::P res;
private:
  mutable int cache_face_0=-1;
  mutable int cache_face_1=-1;
  mutable int cache_face_2=-1;
  mutable int cache_face_3=-1;
  mutable Point cache_res_0; mutable bool cache_0=false;
  mutable Point cache_res_1; mutable bool cache_1=false;
  mutable Point cache_res_2; mutable bool cache_2=false;
  mutable Point cache_res_3; mutable bool cache_3=false;
  int i;
  static std::map<GLTFModelInterface*,GameApi::ARR> g_cache;
  static std::map<GLTFModelInterface*,GameApi::P> g_cache2;
  bool firsttime=true;
  Vector light_dir;
};
std::map<GLTFModelInterface*,GameApi::ARR> GltfMeshAllPArr::g_cache;
std::map<GLTFModelInterface*,GameApi::P> GltfMeshAllPArr::g_cache2;

GameApi::ARR GameApi::MainLoopApi::gltf_mesh_all_p_arr( GameApi::EveryApi &ev, TF model0, float light_dir_x, float light_dir_y, float light_dir_z)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  int s = 100;
  ArrayType *t = new ArrayType;
  t->type=0;
  for(int i=0;i<s;i++)
    {
      t->vec.push_back(add_polygon2(e,new GltfMeshAllPArr(e,ev,interface,i,Vector(light_dir_x,light_dir_y,light_dir_z)),1).id);
    }
  return add_array(e,t);
}

class GltfMeshAllMatArr : public MaterialForward
{
public:
  GltfMeshAllMatArr(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int i, float mix, Vector light_dir) : e(e), ev(ev), interface(interface), i(i),mix(mix),light_dir(light_dir) { m_mat=0; firsttime=true; }
  void create_mat() const
  {
    if (firsttime) {
    if (g_cache.find(interface)==g_cache.end()) {
	int c = get_current_block();
	set_current_block(-2);
      interface->Prepare();
      int scene_id = interface->get_default_scene();
      GameApi::ARR arr = gltf_scene2_p_arr(e,ev,interface,scene_id,"",&arr_fetch_material,&arr_fetch_material,false, mix,light_dir);
      g_cache[interface]=arr;
      set_current_block(c);
    }
    GameApi::ARR arr= g_cache[interface];
    ArrayType *t = find_array(e,arr);

    //int s = t->vec.size();
    //for(int i=0;i<s;i++) std::cout << t->vec[i] << ",";
    //std::cout << std::endl;

    
    if (i>=t->vec.size()) { /*std::cout << "MatArr empty at " << i << std::endl;*/  return; }
    GameApi::MT mt;
    if (t->vec[i]==-1) {
      std::cout << "ERROR" << std::endl;
      //mt.id = t->vec[i-1];
      m_mat = 0; //find_material(e,mt); 
    } else {
      mt.id = t->vec[i];
      m_mat = find_material(e,mt);
    }
    firsttime=false;
    }
  }

  virtual GameApi::ML mat2(GameApi::P p) const
  {
    create_mat();
    GameApi::ML ml = ev.mainloop_api.ml_empty();
    if (m_mat)
      ml.id=m_mat->mat(p.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    create_mat();
    GameApi::ML ml = ev.mainloop_api.ml_empty();
    if (m_mat)
    ml.id=m_mat->mat_inst(p.id,pts.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    create_mat();
    GameApi::ML ml = ev.mainloop_api.ml_empty();
    if (m_mat)
    ml.id = m_mat->mat_inst_matrix(p.id,ms.id);
    return ml;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    create_mat();
    GameApi::ML ml = ev.mainloop_api.ml_empty();
    if (m_mat)
    ml.id = m_mat->mat_inst2(p.id,pta.id);
    return ml;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const { }

  
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GLTFModelInterface *interface;
  int i;
  static std::map<GLTFModelInterface*,GameApi::ARR> g_cache;
  mutable Material *m_mat;
  float mix;
  mutable bool firsttime;
  Vector light_dir;
};
std::map<GLTFModelInterface*,GameApi::ARR> GltfMeshAllMatArr::g_cache;

GameApi::ARR GameApi::MainLoopApi::gltf_mesh_all_mt_arr( GameApi::EveryApi &ev, TF model0, float mix, float light_dir_x, float light_dir_y, float light_dir_z )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  int s = 200;
  ArrayType *t = new ArrayType;
  t->type=0;
  for(int i=0;i<s;i++)
    {
      t->vec.push_back(add_material(e,new GltfMeshAllMatArr(e,ev,interface,i,mix,Vector(light_dir_x,light_dir_y,light_dir_z))).id);
    }
  return add_array(e,t);
}

class EmptyMainLoop : public MainLoopItem
{
public:
    virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
};

GameApi::ML GameApi::MainLoopApi::ml_empty()
{
  return add_main_loop(e,new EmptyMainLoop);
}


GameApi::ML GameApi::MainLoopApi::gltf_mesh_all( GameApi::EveryApi &ev, TF model0, float mix, int mode, float light_dir_x, float light_dir_y, float light_dir_z , float border_width, unsigned int border_color)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfMeshAll(e,ev,interface,mix,mode,"",Vector(light_dir_x, light_dir_y, light_dir_z),border_width,border_color));
}
GameApi::ML GameApi::MainLoopApi::gltf_mesh_all_anim( GameApi::EveryApi &ev, TF model0, float mix, int mode, std::string keys, float light_dir_x, float light_dir_y, float light_dir_z, float border_width, unsigned int border_color)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfMeshAll(e,ev,interface,mix,mode,keys,Vector(light_dir_x,light_dir_y,light_dir_z),border_width,border_color));
}

class GltfMeshAllEnv : public MainLoopItem
{
public:
  GltfMeshAllEnv(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, GameApi::BM diffuse, GameApi::BM specular, GameApi::BM bfrd, float mix)
    :env(env), ev(ev), interface(interface), diffuse(diffuse),specular(specular),bfrd(bfrd),mix(mix) { res.id = -1;}


  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
    bool is_binary=false;
    if (int(url.size())>3) {
      std::string sub = url.substr(url.size()-3);
      if (sub=="glb") is_binary=true;
    }
    //LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
    //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
    interface->Prepare();
    GameApi::P mesh = gltf_scene2_p(env, ev, interface,0,"");
    
    GameApi::ML ml = gltf_mesh_all2_env( env, ev, interface,diffuse,specular,bfrd, mix );
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  GameApi::BM diffuse, specular, bfrd;
  float mix;
};
GameApi::ML GameApi::MainLoopApi::gltf_mesh_all_env( GameApi::EveryApi &ev, TF model0, BM diffuse,BM specular, BM bfrd, float mix )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfMeshAllEnv(e,ev,interface,diffuse,specular,bfrd,mix));
}

struct AnimData
{
  const tinygltf::Animation *anim;
  int current_anim=-1;
  const tinygltf::Accessor *input_acc;
  int current_input_acc=-1;
  const tinygltf::Accessor *output_acc;
  int current_output_acc=-1;
  const tinygltf::Accessor *bind_acc;
  int current_bind_acc=-1;
  const tinygltf::BufferView *input_buf;
  int current_input_buf=-1;
  const tinygltf::BufferView *output_buf;
  int current_output_buf=-1;
  const tinygltf::BufferView *bind_buf;
  int current_bind_buf=-1;
  const tinygltf::Buffer *input;
  int current_input=-1;
  const tinygltf::Buffer *output;
  int current_output=-1;
  const tinygltf::Buffer *bind;
  int current_bind=-1;
};


class GLTFAnimation : public CollectInterface
{
public:
  GLTFAnimation(AnimData *dt, GLTFModelInterface *interface, int animation, int channel, int time_index, int skin) : dt(dt), interface(interface), animation(animation), channel(channel), time_index(time_index), skin(skin) { }
  void Collect(CollectVisitor &vis)
  {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    //model = &load->model;

    int sz = interface->animations_size(); //model->animations.size();
    if (animation<0||animation>=sz) return;
    //if (dt->current_anim != animation)
      dt->anim = &interface->get_animation(animation); //&model->animations[animation];
      //dt->current_anim = animation;
    //std::cout << "Animation:" << anim->name << std::endl;
    chan = &dt->anim->channels[channel];
    int target_node = chan->target_node;
    //std::cout << "Target node:" << target_node << std::endl;
    //std::cout << "Target node name:" << &model->nodes[chan->target_node] << std::endl;
    const tinygltf::Node &n = interface->get_node(target_node); //model->nodes[target_node];

    //int skin = 0; //n->skin;
    int inverseBindMatrices=-1;
    if (skin>=0 && skin<interface->skins_size()) {
      const tinygltf::Skin &s = interface->get_skin(skin); //&model->skins[skin];
      inverseBindMatrices = s.inverseBindMatrices;
    }
    int sz2 = interface->accessors_size(); //model->accessors.size();
    bind_acc_done = false;
    if (inverseBindMatrices!=-1 && inverseBindMatrices>=0 && inverseBindMatrices<sz2) {
      if (dt->current_bind_acc != inverseBindMatrices) {
	dt->bind_acc = &interface->get_accessor(inverseBindMatrices); //&model->accessors[inverseBindMatrices];
	dt->current_bind_acc = inverseBindMatrices;
      }
      bind_acc_done = true;
    }
    int sz3 = interface->bufferviews_size(); //model->bufferViews.size();
    bind_buf_done = false;
    if (bind_acc_done && dt->bind_acc->bufferView>=0 && dt->bind_acc->bufferView<sz3) {
      if (dt->current_bind_buf != dt->bind_acc->bufferView)
      dt->bind_buf = &interface->get_bufferview(dt->bind_acc->bufferView); //&model->bufferViews[bind_acc->bufferView];
      dt->current_bind_buf=dt->bind_acc->bufferView;
      bind_buf_done = true;
    }
    int sz4 = interface->buffers_size(); //model->buffers.size();
    bind_done = false;
    if (bind_buf_done && dt->bind_buf->buffer>=0 && dt->bind_buf->buffer<sz4) {
      if (dt->current_bind != dt->bind_buf->buffer)
      dt->bind = &interface->get_buffer(dt->bind_buf->buffer); //&model->buffers[bind_buf->buffer];
      dt->current_bind = dt->bind_buf->buffer;
      bind_done = true;
    }
    //std::cout << "Name:" << n->name << std::endl;
    sampler = chan->sampler;
    int sz5 = dt->anim->samplers.size();
    if (sampler>=0 && sampler<sz5)
      samp = &dt->anim->samplers[sampler];
    if (samp) {
      //std::cout << "AnimSampler:" << samp->interpolation << std::endl;
      int input_idx = samp->input;
      //std::cout << "input=" << input_idx << std::endl;
      int sz = interface->accessors_size(); //model->accessors.size();
      input_acc_done = false;
      if (input_idx>=0 && input_idx<sz) {
	if (dt->current_input_acc != input_idx)
	dt->input_acc = &interface->get_accessor(input_idx); //&model->accessors[input_idx]; // keyframe times
	dt->current_input_acc = input_idx;
	input_acc_done = true;
      }
      if (input_acc_done) {
	//std::cout << "input:" << input_acc->componentType << " " << input_acc->count << " " << input_acc->type << std::endl;
      }

      int output_idx = samp->output;
      //std::cout << "output=" << output_idx << std::endl;
      int sz6 = interface->accessors_size(); //model->accessors.size();
      output_acc_done = false;
      if (output_idx>=0 && output_idx<sz6) {
	//if (dt->current_output_acc != output_idx)
	dt->output_acc = &interface->get_accessor(output_idx); //&model->accessors[output_idx]; // property value changes
	//dt->current_output_acc = output_idx;
	output_acc_done = true;
      }
      if (output_acc_done) {
	//std::cout << "output:" << output_acc->componentType << " " << output_acc->count << " " << output_acc->type << " " << std::endl;
      }

    }
    input_buf_done = false;
    if (input_acc_done) {
      if (dt->current_input_buf != dt->input_acc->bufferView)
      dt->input_buf = &interface->get_bufferview(dt->input_acc->bufferView); //&model->bufferViews[input_acc->bufferView];
      dt->current_input_buf = dt->input_acc->bufferView;
      input_buf_done = true;
    }
    output_buf_done = false;
    if (output_acc_done) {
      if (dt->current_output_buf != dt->output_acc->bufferView)
      dt->output_buf = &interface->get_bufferview(dt->output_acc->bufferView); //&model->bufferViews[output_acc->bufferView];
      dt->current_output_buf = dt->output_acc->bufferView;
      output_buf_done = true;
    }
    input_done = false;
    if (input_buf_done) {
      if (dt->current_input != dt->input_buf->buffer)
      dt->input = &interface->get_buffer(dt->input_buf->buffer); //&model->buffers[input_buf->buffer];
      dt->current_input = dt->input_buf->buffer;
      input_done = true;
    }
    output_done = false;
    if (output_buf_done) {
      if (dt->current_output != dt->output_buf->buffer)
      dt->output = &interface->get_buffer(dt->output_buf->buffer); //&model->buffers[output_buf->buffer];
      dt->current_output = dt->output_buf->buffer;
      output_done = true;
  }

  }
  void Prepare() {
    interface->Prepare();
    HeavyPrepare();
    //model = &load->model;
    /*
    int sz = interface->animations_size(); //model->animations.size();
    if (animation<0||animation>=sz) return;
    anim = interface->get_animation(animation); //&model->animations[animation];
    //std::cout << "Animation:" << anim->name << std::endl;
    chan = &anim.channels[channel];
    int target_node = chan->target_node;
    //std::cout << "Target node:" << target_node << std::endl;
    //std::cout << "Target node name:" << &model->nodes[chan->target_node] << std::endl;
    tinygltf::Node n = interface->get_node(target_node); //&model->nodes[target_node];

    //int skin = 0; //n->skin;
    tinygltf::Skin s;
    bool skins_done = false;
    if (skin<0 || skin>=interface->skins_size()) { } else {
      s = interface->get_skin(skin); //&model->skins[skin];
      skins_done = true;
    }
    int inverseBindMatrices = skins_done?s.inverseBindMatrices:-1;
    //std::cout << "INVERSEBINDMATRICES:" << inverseBindMatrices << std::endl;
    int sz2 = interface->accessors_size(); //model->accessors.size();
    if (inverseBindMatrices!=-1 && inverseBindMatrices>=0 && inverseBindMatrices<sz2) 
      bind_acc = interface->get_accessor(inverseBindMatrices); //&model->accessors[inverseBindMatrices];
    int sz3 = interface->bufferviews_size(); //model->bufferViews.size();
    if (bind_acc_done && bind_acc.bufferView>=0 && bind_acc.bufferView<sz3)
      bind_buf = interface->get_bufferview(bind_acc.bufferView); //&model->bufferViews[bind_acc->bufferView];
    int sz4 = interface->buffers_size(); //model->buffers.size();
    if (bind_buf_done && bind_buf.buffer>=0 && bind_buf.buffer<sz4)
      bind = interface->get_buffer(bind_buf.buffer); //&model->buffers[bind_buf->buffer];
    
    //std::cout << "Name:" << n->name << std::endl;
    sampler = chan->sampler;
    int sz5 = anim.samplers.size();
    if (sampler>=0 && sampler<sz5)
      samp = &anim.samplers[sampler];
    if (samp) {
      //std::cout << "AnimSampler:" << samp->interpolation << std::endl;
      int input_idx = samp->input;
      //std::cout << "input=" << input_idx << std::endl;
      int sz = interface->accessors_size(); //model->accessors.size();
      input_acc_done = false;
      if (input_idx>=0 && input_idx<sz) {
	input_acc = interface->get_accessor(input_idx); //&model->accessors[input_idx]; // keyframe times
	input_acc_done = true;
      }
      if (input_acc_done) {
	//std::cout << "input:" << input_acc->componentType << " " << input_acc->count << " " << input_acc->type << std::endl;
      }

      int output_idx = samp->output;
      //std::cout << "output=" << output_idx << std::endl;
      int sz2 = interface->accessors_size(); //model->accessors.size();
      output_acc_done = false;
      if (output_idx>=0 && output_idx<sz2) {
	output_acc = interface->get_accessor(output_idx); //&model->accessors[output_idx]; // property value changes
	output_acc_done = true;
      }
      if (output_acc_done) {
	//std::cout << "output:" << output_acc->componentType << " " << output_acc->count << " " << output_acc->type << " " << std::endl;
      }

    }
    input_buf_done = false;
    if (input_acc_done) {
      input_buf = interface->get_bufferview(input_acc.bufferView); //&model->bufferViews[input_acc->bufferView];
      input_buf_done = true;
    }
    output_buf_done = false;
    if (output_acc_done) {
      output_buf = interface->get_bufferview(output_acc.bufferView); //&model->bufferViews[output_acc->bufferView];
      output_buf_done = true;
    }
    input_done = false;
    if (input_buf_done) {
      input = interface->get_buffer(input_buf.buffer); //&model->buffers[input_buf->buffer];
      input_done = true;
    }
    output_done = false;
    if (output_buf_done) {
      output = interface->get_buffer(output_buf.buffer); //&model->buffers[output_buf->buffer];
      output_done = true;
    }
    */
  }

  std::string InterpolationMode() const {
    if (samp) {
      return samp->interpolation;
    }
    return "";
  }
  
  Matrix InverseBindMatrix(int joint) const
  {
    if (bind_done && bind_acc_done && bind_buf_done) { 
    const unsigned char *ddt = &dt->bind->data[0];
    int offset = dt->bind_buf->byteOffset;
    // int length = bind_buf->byteLength;
    const unsigned char *dt2 = ddt + offset;
    
    int offset2 = dt->bind_acc->byteOffset;
    int stride = dt->bind_acc->ByteStride(*dt->bind_buf);
    const unsigned char *dt3 = dt2 + offset2 + stride*joint;
    float *dt4 = (float*)dt3;
    Matrix res;
    for(int i=0;i<4;i++)
      for(int j=0;j<4;j++)
	res.matrix[i+j*4] = dt4[j+i*4];
    //std::cout << "Joint " << joint << "::" << res << std::endl;
    //std::cout << "Joint " << joint << "::" << res << std::endl;
    return res;
    } else {
      std::cout << std::endl << "Joint FAIL " << joint << std::endl;
      std::cout << "REASON: " << bind_done << " " << bind_acc_done << " " << bind_buf_done << std::endl;
      return Matrix::Identity();
    }
  }
  
  int target_node() const
  {
    if (!chan) return 0;
    int t = chan->target_node;
    return t;
  }
  
  float start_time() const
  {
    if (input_done && input_buf_done && input_acc_done) {
    
    const unsigned char *ddt = &dt->input->data[0];
    int offset = dt->input_buf->byteOffset;
    //int length = input_buf->byteLength;
    const unsigned char *dt2 = ddt + offset;
    int stride = dt->input_buf->byteStride;

    int offset2 = dt->input_acc->byteOffset;
    const unsigned char *dt3 = dt2 + offset2;
    int count = dt->input_acc->count;
    assert(dt->input_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    assert(dt->input_acc->type==TINYGLTF_TYPE_SCALAR);
    if (stride==0) { stride=sizeof(float); }
    //assert(stride==0);
    if (time_index>=0 && time_index-1<count) {
      float *arr = (float*) (dt3 + stride*(time_index));
      return *arr;
    } else {
      return *(float*)(dt3+stride*(count-1)); }

    } else return 0.0;
    
  }
  float end_time() const
  {
    if (input_done && input_buf_done && input_acc_done) {
    const unsigned char *ddt = &dt->input->data[0];
    int offset = dt->input_buf->byteOffset;
    //int length = input_buf->byteLength;
    const unsigned char *dt2 = ddt + offset;
    int stride = dt->input_buf->byteStride;

    int offset2 = dt->input_acc->byteOffset;
    const unsigned char *dt3 = dt2 + offset2;
    int count = dt->input_acc->count;
    assert(dt->input_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    //assert(input_acc->type==TINYGLTF_TYPE_SCALAR);
    if (stride==0 && dt->input_acc->type==TINYGLTF_TYPE_SCALAR) { stride=sizeof(float); }
    if (stride==0 && dt->input_acc->type==TINYGLTF_TYPE_VEC2) { stride=2*sizeof(float); }
    if (stride==0 && dt->input_acc->type==TINYGLTF_TYPE_VEC3) { stride=3*sizeof(float); }
    if (stride==0 && dt->input_acc->type==TINYGLTF_TYPE_VEC4) { stride=4*sizeof(float); }
    //assert(stride==0);
    if (time_index+1>=0 && time_index+1<count) {
	float *arr = (float*) (dt3 + stride*(time_index+1));
	return *arr; /* *arr */
    } else { /*      std::cout << "Returning end time2: " << *(float*)(dt3+stride*(count-1)) << std::endl;*/
      return *(float*)(dt3+stride*(count-1)); }
    } else {      /*std::cout << "Returning end time3: " << 0 << std::endl;*/ return 0.0; }
  }

  virtual int Count() const
  {
    if (output_acc_done) {
      int count = dt->output_acc->count;
      return count;
    } else return 0;
  }
  virtual float *Amount(int i) const {
    static float arr2[4];
    if (output_done && output_buf_done && output_acc_done) {
    const unsigned char *ddt = &dt->output->data[0];
    int offset = dt->output_buf->byteOffset;
    //int length = output_buf->byteLength;
    //std::cout << "AMOUNT(" << i << ")" << std::endl;
    //std::cout << "OFFSET:" << offset << std::endl;
    const unsigned char *dt2 = ddt + offset;
    
    int offset2 = dt->output_acc->byteOffset;
    //std::cout << "OFFSET2:" << offset2 << std::endl;
    const unsigned char *dt3 = dt2 + offset2;
    int stride = dt->output_acc->ByteStride(*dt->output_buf);
    //std::cout << "stride: " << stride << " " << output_acc->type << std::endl;
    //std::cout << "stride:" << stride << std::endl;
    if (stride==0 && dt->output_acc->type==TINYGLTF_TYPE_SCALAR) stride = 1*sizeof(float);
    if (stride==0 && dt->output_acc->type==TINYGLTF_TYPE_VEC2) stride = 2*sizeof(float);
    if (stride==0 && dt->output_acc->type==TINYGLTF_TYPE_VEC3) stride = 3*sizeof(float);
    if (stride==0 && dt->output_acc->type==TINYGLTF_TYPE_VEC4) stride = 4*sizeof(float);
    assert(dt->output_acc->componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
    // TODO: if componentType != float, then it can be converted to float using
    // equations from the gltf 2.0 specification chapter Animations.
    
    //assert(output_acc->type==TINYGLTF_TYPE_VEC3);
    int count = dt->output_acc->count;
    //std::cout << "COUNT:" << count << std::endl;

    if (i>=0 && i<count) {
      float *arr = (float*) (dt3 + i*stride);
      return arr;
    } else {
      float *arr = (float*) (dt3 + (count-1)*stride);
      return arr;
    }
    } else { std::cout << "ERROR: output" << std::endl; return arr2; }
  }
  virtual int Type(int i) const
  {
    if (chan && output_acc_done) {
      std::string t = chan->target_path;
      if (t=="translation" && dt->output_acc->type==TINYGLTF_TYPE_VEC3) return 0;
      if (t=="rotation" && dt->output_acc->type==TINYGLTF_TYPE_VEC4) return 1;
      if (t=="scale" && dt->output_acc->type==TINYGLTF_TYPE_VEC3) return 2;
      if (t=="weights") return 3;
      std::cout << "target_path: " << t << " ERROR :" << dt->output_acc->type << std::endl;
      return -1;
    } else return -1;
  }
  
private:
  AnimData *dt;
  GLTFModelInterface *interface;
  //tinygltf::Model *model=0;
  int animation;
  int channel;
  const tinygltf::AnimationChannel *chan=0;
  int sampler;
  const tinygltf::AnimationSampler *samp=0;
  int time_index;


  bool input_acc_done=false;
  bool output_acc_done = false;
  bool bind_acc_done=false;
  bool input_buf_done=false;
  bool output_buf_done=false;
  bool bind_buf_done=false;
  bool input_done=false;
  bool output_done=false;
  bool bind_done =false;
  
  int skin;
};


GameApi::ML gltf_anim3(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int animation, int target_node, float mix, int mode, Vector light_dir, float border_width, unsigned int border_color)
{
  int channel = -1;
  const tinygltf::Animation &anim = interface->get_animation(animation); //&load->model.animations[animation];
  int s = anim.channels.size();
  GameApi::ML array; array.id = -1;
  
  const tinygltf::Node &node = interface->get_node(target_node); //&load->model.nodes[target_node];
      //std::cout << "\nNode:" << node->name << std::endl;
      int mesh_id = node.mesh;
      GameApi::ML ml = gltf_mesh2(e,ev, interface, mesh_id, 0, "cvb",mix,mode,light_dir, animation, border_width, border_color);
      if (channel==-1)
	{
	  std::vector<GameApi::ML> vec;
	  int s2 = node.children.size();
	  //std::vector<GameApi::ML> vec;
	  for(int i=0;i<s2;i++) {
	    int child_id = node.children[i];
	    if (child_id!=-1) {
	      GameApi::ML ml = gltf_anim3( e, ev, interface, animation, child_id,mix,mode,light_dir,border_width, border_color );
	      vec.push_back(ml);
	    }
	  }
	  
	  vec.push_back(ml);
	  GameApi::ML array = ev.mainloop_api.array_ml(ev, vec);
	  return array;
	}

  for(int j=0;j<s;j++) {
    const tinygltf::AnimationChannel *chan = &anim.channels[j];
    if (chan->target_node == target_node) {
      channel = j;

      AnimData *dt = new AnimData;
      GLTFAnimation *anim3 = new GLTFAnimation(dt,interface,animation, channel, 0,0);
      anim3->Prepare();
      int count = anim3->Count();

  std::vector<GameApi::ML> vec;
      if (j==0) {
  int s2 = node.children.size();
  //std::vector<GameApi::ML> vec;
  for(int i=0;i<s2;i++) {
    int child_id = node.children[i];
    if (child_id!=-1) {
      GameApi::ML ml = gltf_anim3( e, ev, interface, animation, child_id,mix,mode,light_dir,border_width,border_color );
      vec.push_back(ml);
    }
  }
      
  vec.push_back(ml);
      }
  if (array.id!=-1)
    vec.push_back(array);
  array = ev.mainloop_api.array_ml(ev, vec);
  AnimData *ddt = new AnimData;
  for(int i=0;i<count-1;i++)
    {
      GameApi::MN current = ev.move_api.mn_empty();
      GameApi::MN current2 = ev.move_api.mn_empty();
      //int target_node = anim->target_node();
      //if (target_node!=node_idx) continue;

      GLTFAnimation *anim2 = new GLTFAnimation(ddt, interface,animation, channel, i,0);
      anim2->Prepare();
      float start_time = anim2->start_time();
      float end_time = anim2->end_time();

      //std::cout << "start_time: " << start_time << std::endl;
      //std::cout << "end_time: " << end_time << std::endl;
      
      float *a = anim2->Amount(i);
      int type = anim2->Type(i);

      float *b = anim2->Amount(i+1);
      int b_type = anim2->Type(i+1);
      delete anim2;
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
	  //std::cout << "Scale9:" << a[0] << " " << a[1] << " " << a[2] << std::endl;
	}
      if (type==3) // weights
	{
	}



      //Cif (feature_enable[3])
      if (b_type==0) // translate
	{
	  //std::cout << "b_Trans: " << b[0] << " " << b[1] << " " << b[2] << std::endl;
	  current2 = ev.move_api.trans2(current2, b[0], b[1],b[2]);
	}
      //Cif (feature_enable[4])
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
      //Cif (feature_enable[5])
      if (b_type==2) // scale
	{
	  //std::cout << "b_Scale: " << b[0] << " " << b[1] << " " << b[2] << std::endl;
	  current2 = ev.move_api.scale2(current2, b[0],b[1],b[2]);
	  //std::cout << "Scale10:" << b[0] << " " << b[1] << " " << b[2] << std::endl;
	}
      if (b_type==3) // weights
	{
	}
      GameApi::MN res = ev.move_api.mn_interpolate2(current, current2, start_time, end_time);
      array = ev.move_api.move_ml(ev, array, res, 1, 10.0);
    }

  delete dt;
  delete ddt;
    }
  }

  

  return array;
}

void slerp(float *prev, float *next, float val, float *res)
{
  float dot = prev[0]*next[0] + prev[1]*next[1] + prev[2]*next[2]+prev[3]*next[3];
    if (dot<0.0) {
    next[0] = next[0];
    next[1] = next[1];
    next[2] = next[2];
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

struct PrevNodes
{
  //std::map<int,TransformObject,std::less<int> > prev_nodes1;
  //std::map<int,TransformObject,std::less<int> > prev_nodes2;

  std::vector<std::pair<int,TransformObject> > prev_nodes1;
  std::vector<std::pair<int,TransformObject> > prev_nodes2;
  TransformObject find1(int ii) {
    int sz = prev_nodes1.size();
    for(int i=0;i<sz;i++)
      {
	if (prev_nodes1[i].first == ii) return prev_nodes1[i].second;
      }
    return TransformObject();
  }
  TransformObject find2(int ii) {
    int sz = prev_nodes2.size();
    for(int i=0;i<sz;i++)
      {
	if (prev_nodes2[i].first == ii) return prev_nodes2[i].second;
      }
    return TransformObject();
  }
};

std::vector<MainLoopItem*> g_joint_cache;
void add_joint_cache(MainLoopItem *item)
{
  g_joint_cache.push_back(item);
}
bool find_joint_cache(MainLoopItem *item)
{
  int s = g_joint_cache.size();
  for(int i=0;i<s;i++) {
    if (item==g_joint_cache[i]) return true;
  }
  return false;
}

class GLTFJointMatrices : public MainLoopItem
{
public:
  GLTFJointMatrices(GLTFJointMatrices *prev, GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int animation, int time_index, MainLoopItem *next, bool has_anim) : prev(prev), env(env), ev(ev), interface(interface), skin_num(skin_num), animation(animation), time_index(time_index),next(next), has_anim(has_anim) { firsttime=true; max_joints = 200; max_count=0; max_time=0.0; }
  ~GLTFJointMatrices() {
    int s = anims.size();
    for(int i=0;i<s;i++) delete anims[i];
  }
  void Collect(CollectVisitor &vis)
  {
    interface->Collect(vis);
    next->Collect(vis);
    vis.register_obj(this);
  }
  int calc_max_joints()
  {
    const tinygltf::Skin *skin=0;
    if (skin_num>=0 && skin_num<interface->skins_size()) {
      skin = &interface->get_skin(skin_num);
    }
    if (skin) {
      int s2 = skin->joints.size();
      // std::cout << "Max joints set to:" << s2 << std::endl;
      return s2;
    }
    return 0;
  }
  void HeavyPrepare()
  {
    if (firsttime) {
      max_joints = calc_max_joints();
      //next->Prepare();
      jointmatrices_start.resize(max_joints);
      jointmatrices_end.resize(max_joints);
      bindmatrix.resize(max_joints);
      start_t.resize(max_joints);
      end_t.resize(max_joints);
      root_env.resize(max_joints);
      root_env_2.resize(max_joints);
      node_ids.resize(max_joints);
      //local.resize(max_joints);
      //local_2.resize(max_joints);
      int s = max_joints;
      for(int i=0;i<s;i++) {
	//GameApi::MN mn = ev.move_api.mn_empty();
	root_env[i]= Matrix::Identity();
	root_env_2[i]=Matrix::Identity();
	//local[i]=Matrix::Identity();
	//local_2[i]=Matrix::Identity();
	jointmatrices_start[i]=gltf_node_default();
	jointmatrices_end[i]=gltf_node_default();
	bindmatrix[i]=Matrix::Identity();
	start_t[i] = 0.0;
	end_t[i] = 0.0;
	node_ids[i]=0;
      }
      firsttime = false; 
      // std::cout << "GLTFSkeletonAnim::Prepare() start" << std::endl;
      if (interface->skins_size()==0) {
	std::cout << "skins_size==0" << std::endl;
	int start_node = 0;
	PrevNodes *prev2 = new PrevNodes;
	AnimData *dt = new AnimData;
    fill_binds(dt);
	recurse_node(dt,*prev2, start_node, 0, Matrix::Identity(), Matrix::Identity(), 0 /*anim*/, time_index, -1);

	check_joints();
	delete dt;
	delete prev2;
      } else {
	int sz = interface->skins_size();
      if (skin_num>=0 && skin_num<sz) {
	const tinygltf::Skin &skin = interface->get_skin(skin_num); //&load->model.skins[skin_num];
	int start_node = skin.skeleton;
	if (start_node==-1) {
	  std::cout << "skin.skeleton==-1" << std::endl;
	  int defscene = interface->get_default_scene();
	  if (defscene==-1) { std::cout << "Default scene ==-1" << std::endl; start_node=0; } else { 
	    const tinygltf::Scene &scene = interface->get_scene(defscene);
	    if (scene.nodes.size()>0) {
	      start_node = scene.nodes[0];
	      if (scene.nodes.size()>1) std::cout << "ERROR: Possibly not all scene nodes rendered" << std::endl;
	      if (start_node==0) std::cout << "scene.nodes[0]==0" << std::endl;
	    }
	    else {
	      std::cout << "Default Scene nodes size=0" << std::endl;
	      start_node=0;
	    }
	  }
	}
	max_count=0;
	max_time = 0.0;
	PrevNodes *prev2 = new PrevNodes;
	AnimData *dt = new AnimData;
    fill_binds(dt);
	recurse_node(dt,*prev2, start_node, 0, Matrix::Identity(), Matrix::Identity(), 0 /*anim*/, time_index, -1);
	check_joints();
	delete dt;
	delete prev2;
      }
      }
    }

  }
  void Prepare()
  {
    if (firsttime) {
      max_joints=calc_max_joints();
      jointmatrices_start.resize(max_joints);
      jointmatrices_end.resize(max_joints);
      bindmatrix.resize(max_joints);
      start_t.resize(max_joints);
      end_t.resize(max_joints);
      root_env.resize(max_joints);
      root_env_2.resize(max_joints);
      node_ids.resize(max_joints);
      //local.resize(max_joints);
      //local_2.resize(max_joints);
      int s = max_joints;
      for(int i=0;i<s;i++) {
	//GameApi::MN mn = ev.move_api.mn_empty();
	root_env[i]= Matrix::Identity();
	root_env_2[i]=Matrix::Identity();
	//local[i]=Matrix::Identity();
	//local_2[i]=Matrix::Identity();
	jointmatrices_start[i]=gltf_node_default();
	jointmatrices_end[i]=gltf_node_default();
	bindmatrix[i]=Matrix::Identity();
	start_t[i] = 0.0;
	end_t[i] = 0.0;
	node_ids[i]=-1;
      }
      firsttime = false; 
      // std::cout << "GLTFSkeletonAnim::Prepare() start" << std::endl;

    interface->Prepare();
    int sz2 = interface->skins_size(); //load->model.skins.size();
    if (skin_num>=0 && skin_num<sz2) {
      const tinygltf::Skin &skin = interface->get_skin(skin_num); //&load->model.skins[skin_num];
      int start_node = skin.skeleton;
	if (start_node==-1) {
	  std::cout << "skin.skeleton==-1" << std::endl;
	  int defscene = interface->get_default_scene();
	  if (defscene==-1) { std::cout << "Default scene ==-1" << std::endl; start_node=0; } else { 
	    const tinygltf::Scene &scene = interface->get_scene(defscene);
	    if (scene.nodes.size()>0) {
	      start_node = scene.nodes[0];
	      if (scene.nodes.size()>1) std::cout << "ERROR: Possibly not all scene nodes rendered" << std::endl;
	      if (start_node==0) std::cout << "scene.nodes[0]==0" << std::endl;
	    }
	    else {
	      std::cout << "Default Scene nodes size=0" << std::endl;
	      start_node=0;
	    }
	  }
	}
      max_count =0;
      max_time = 0.0;
      PrevNodes *prev2 = new PrevNodes;
      AnimData *dt = new AnimData;
    fill_binds(dt);
      recurse_node(dt,*prev2,start_node, 0, Matrix::Identity(), Matrix::Identity(), 0 /*anim*/, time_index, -1);
	check_joints();
      delete prev2;
      delete dt;
     }
    if (!find_joint_cache(next)) {
      // skip prepare if we've seen this before.
      next->Prepare();
      add_joint_cache(next);
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

  void SetPreviousTimeIndex(PrevNodes &prev2,int node_id, TransformObject o1, TransformObject o2)
  {
    //std::cout << "timeindex:" << node_id << std::endl;
    //print_transform(o1);
    //print_transform(o2);
    //std::cout << (int)&prev2 << std::endl;
    prev2.prev_nodes1.push_back(std::make_pair(node_id,o1));
    //std::cout << "1" << std::endl;
    prev2.prev_nodes2.push_back(std::make_pair(node_id,o2));
    //std::cout << "3" << std::endl;
    //trans1 = o1; trans2 = o2;
  }
  void call_prev_time_index1(PrevNodes &prev2, GLTFJointMatrices *next, int node_id) {
    //tinygltf::Node *node = &load->model.nodes[node_id];
    bool done = false;
    for(int i=0;i<node_ids.size();i++) {
      //if (i<max_joints) {
      if(node_id==node_ids[i]) {
	next->SetPreviousTimeIndex(prev2,node_id, jointmatrices_start[i],jointmatrices_end[i]);
	done = true;
	}
      // }
    }
    //if (!done && prev) prev->call_prev_time_index1(next,node_id);
    if (!done && node_id>=0 && node_id<interface->nodes_size()) {
    AnimData *dt = new AnimData;
      const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];
      TransformObject start_obj = gltf_node_transform_obj(&node);
      TransformObject end_obj = start_obj;
      int sz = interface->animations_size(); //load->model.animations.size();
      if (animation<0||animation>=sz) return;
      
      const tinygltf::Animation &anim3 = interface->get_animation(animation); //&load->model.animations[animation];
    int s5 = anim3.channels.size();
      for(int i=s5-1;i>=0;i--) {
	int channel = i;
      const tinygltf::AnimationChannel *chan = &anim3.channels[i];
      std::string path = chan->target_path;
      if (chan->target_node == node_id) {

      int ik = time_index;
      GLTFAnimation *anim = new GLTFAnimation(dt, interface, animation, channel, ik, skin_num);
      //anims.push_back(anim);
      anim->Prepare();
      float *a = 0;
      float *a2 = 0;
      int type = -1;
      a = anim->Amount(ik); if (ik+1>=anim->Count()) a2=anim->Amount(0); else
	a2 = anim->Amount(ik+1);
      type = anim->Type(time_index);
      if (type==0 && a2 && path=="translation") { // translation
	start_obj.trans_x =a[0];
	start_obj.trans_y =a[1];
	start_obj.trans_z =a[2];
	end_obj.trans_x =a2[0];
	end_obj.trans_y =a2[1];
	end_obj.trans_z =a2[2];
	
      }

      if (type==1 && a2 && path=="rotation") { // rotation
	start_obj.rot_x=a[0];
	start_obj.rot_y=a[1];
	start_obj.rot_z=a[2];
	start_obj.rot_w=a[3];
	end_obj.rot_x=a2[0];
	end_obj.rot_y=a2[1];
	end_obj.rot_z=a2[2];
	end_obj.rot_w=a2[3];
	
	

      }

      if (type==2 && a2 && path=="scale") { // scale
	start_obj.scale_x=a[0];
	start_obj.scale_y=a[1];
	start_obj.scale_z=a[2];
	end_obj.scale_x=a2[0];
	end_obj.scale_y=a2[1];
	end_obj.scale_z=a2[2];
	//std::cout << "Scale11a:" << a[0] << " " << a[1] << " " << a[2] << std::endl;
	//std::cout << "Scale11b:" << a2[0] << " " << a2[1] << " " << a2[2] << std::endl;

      }
      if (type==3 && path=="weights") { std::cout << "Weights not implemented in anim2" << std::endl; }
      delete anim;
      }
      }
      next->SetPreviousTimeIndex(prev2,node_id, start_obj, end_obj);
      //next->SetPreviousTimeIndex(prev2,node_id,TransformObject(),TransformObject());
      delete dt;
    } else {
      // node_id bad.
      next->SetPreviousTimeIndex(prev2,node_id,TransformObject(),TransformObject());
    }
  }
  void call_prev_time_index2(PrevNodes &prev2,int node_id)
  {
    if (!prev) {
      const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];
      TransformObject start_obj = gltf_node_transform_obj(&node);
      TransformObject end_obj = start_obj;
#if 1
      int sz = interface->animations_size(); //load->model.animations.size();
      if (animation<0||animation>=sz) return;
      
      const tinygltf::Animation &anim3 = interface->get_animation(animation); //&load->model.animations[animation];
    int s5 = anim3.channels.size();
    AnimData *dt = new AnimData;
      for(int i=s5-1;i>=0;i--) {
	int channel = i;
      const tinygltf::AnimationChannel *chan = &anim3.channels[i];
      std::string path = chan->target_path;
      if (chan->target_node == node_id) {

      int ik = time_index;
      GLTFAnimation *anim = new GLTFAnimation(dt,interface, animation, channel, ik, skin_num);
      //      anims.push_back(anim);
      anim->Prepare();
      float *a = 0;
      float *a2 = 0;
      int type = -1;
      a = anim->Amount(0);
      a2 = anim->Amount(1);
      type = anim->Type(0);
      delete anim;

      if (type==0 && a2 && path=="translation") { // translation
	start_obj.trans_x =a[0];
	start_obj.trans_y =a[1];
	start_obj.trans_z =a[2];
	end_obj.trans_x =a2[0];
	end_obj.trans_y =a2[1];
	end_obj.trans_z =a2[2];
	
      }

      if (type==1 && a2 && path=="rotation") { // rotation
	start_obj.rot_x=a[0];
	start_obj.rot_y=a[1];
	start_obj.rot_z=a[2];
	start_obj.rot_w=a[3];
	end_obj.rot_x=a2[0];
	end_obj.rot_y=a2[1];
	end_obj.rot_z=a2[2];
	end_obj.rot_w=a2[3];
	
	

      }

      if (type==2 && a2 && path=="scale") { // scale
	start_obj.scale_x=a[0];
	start_obj.scale_y=a[1];
	start_obj.scale_z=a[2];
	end_obj.scale_x=a2[0];
	end_obj.scale_y=a2[1];
	end_obj.scale_z=a2[2];
	//std::cout << "Scale12a:" << a[0] << " " << a[1] << " " << a[2] << std::endl;
	//std::cout << "Scale12b:" << a2[0] << " " << a2[1] << " " << a2[2] << std::endl;
      }
      if (type==3 && path=="weights") {
	std::cout << "Weights not implemented in anim" << std::endl;
      }
      }
      }
#endif
      SetPreviousTimeIndex(prev2,node_id, start_obj, end_obj);
      //delete dt;
    } else {
      prev->call_prev_time_index1(prev2,this, node_id);
    }
  }
  void fill_binds(AnimData *dt)
  {
    if (animation>=0 && animation<interface->animations_size()) {
    const tinygltf::Animation *anim3;
      anim3 = &interface->get_animation(animation);
      const tinygltf::Skin *skin=0;
      if (skin_num>=0 && skin_num<interface->skins_size()) {
	skin = &interface->get_skin(skin_num);
      }
      int channel = 0;
      int ik = time_index;
      GLTFAnimation *anim = new GLTFAnimation(dt,interface, animation, channel, ik, skin_num);
      anim->Prepare();

      if (skin) { 
	int s2 = skin->joints.size();
	for(int i=0;i<s2;i++)
	  {
	Matrix m2 = Matrix::Identity();
	if (anim) m2 = anim->InverseBindMatrix(i);
	//std::cout << "Set Bind:" << i << " == " << m2 << std::endl;
	bindmatrix[i] = m2;
	    
	  }
      }
      delete anim;
    }
  }
  Matrix recurse_node(AnimData *dt, PrevNodes &prev2, int node_id, tinygltf::Node * /*node*/, Matrix pos, Matrix pos2, GLTFAnimation * /*anim*/, int time_index, int /*channel*/)
  {
    //std::cout << "Recurse_node: " << node_id << std::endl;
    if (node_id<0 || node_id>=interface->nodes_size()) { std::cout << "Invalid node_id in recurse_node(" << node_id << "/" << interface->nodes_size() << ")" << std::endl; return Matrix::Identity(); }
   

    call_prev_time_index2(prev2,node_id);
    int sz = interface->animations_size(); 
    if (animation<0||animation>=sz) { std::cout << "Invalid animation id in recurse_node (" << animation << "/" << sz << ")" << std::endl; return Matrix::Identity(); }

    //std::cout << "GETNODE:" << node_id << std::endl;
    const tinygltf::Node &node = interface->get_node(node_id); 

    TransformObject start_obj = prev2.find1(node_id); //prev_nodes1[node_id]; 
    TransformObject end_obj = prev2.find2(node_id); //prev_nodes2[node_id]; 

    int channel = -1;
    const tinygltf::Animation *anim3;
      anim3 = &interface->get_animation(animation);
    int s5 = anim3->channels.size();
      int jj = -1;
      const tinygltf::Skin *skin=0;
      if (skin_num>=0 && skin_num<interface->skins_size()) {
	skin = &interface->get_skin(skin_num);
      }
      if (skin) {
	int s2 = skin->joints.size();
	for(int j=0;j<s2;j++) {
	  //std::cout << "Compare:" << skin->joints[j] << "==" << node_id << std::endl;
	  if (jj!=-1 && skin->joints[j]==node_id)
	    {
	      std::cout << "Duplicate joint"<< node_id << " " << j << " " << jj  << std::endl;
	      jj=-1;
	      break;
	    }
	  if (skin->joints[j]==node_id) { /*doit=true;*/ jj=j; }
	  }
      }

      if (jj==-1)
	{
	  //std::cout << "node " << node_id << "not found in jj calc" << std::endl;
	}
      
      //std::cout << "Found jj:" << jj << std::endl;
      //if (jj==-1) {
      //	if (node_id<max_joints) {
      //	  jj=node_id;
      // 	}
      //}
      //max_time = 0.0;
	for(int i=0;i<s5;i++) {
      const tinygltf::AnimationChannel *chan = &anim3->channels[i];

      
      if (chan->target_node == node_id) {
	std::string path = chan->target_path;
	channel = i;

      int ik = time_index;
      GLTFAnimation *anim = new GLTFAnimation(dt,interface, animation, channel, ik, skin_num);
      anim->Prepare();
      //if (jj!=-1) {
      // 	Matrix m2 = Matrix::Identity();
      // 	if (anim) m2 = anim->InverseBindMatrix(jj);
      // 	bindmatrix[jj] = m2;
      //} 



	
#if 1
    float *a = 0;
    float *a2 = 0;
    int type = -1;
    if (channel!=-1)
      {
	
	if (anim->end_time()>max_time) { max_time = anim->end_time(); }
	//if (max_time<max_time2) max_time = max_time2;
      if (jj!=-1) {
	start_t[jj] = anim->start_time();
	end_t[jj] = anim->end_time();
      }

      a = anim->Amount(ik);
      if (ik+1>=anim->Count()) a2=anim->Amount(0); else
	a2 = anim->Amount(ik+1);
      type = anim->Type(time_index);


      delete anim;
      }

    if (has_anim) {
      if (type==0 && a && a2 && path=="translation") { // translation
	start_obj.trans_x =a[0];
	start_obj.trans_y =a[1];
	start_obj.trans_z =a[2];
	end_obj.trans_x =a2[0];
	end_obj.trans_y =a2[1];
	end_obj.trans_z =a2[2];
	//std::cout << "tr[" << a[0] << "," << a[1] << "," << a[2] << "->" << a2[0] << "," << a2[1] << "," << a2[2] << "]";
      }

      if (type==1 && a && a2 && path=="rotation") { // rotation
	start_obj.rot_x=a[0];
	start_obj.rot_y=a[1];
	start_obj.rot_z=a[2];
	start_obj.rot_w=a[3];
	end_obj.rot_x=a2[0];
	end_obj.rot_y=a2[1];
	end_obj.rot_z=a2[2];
	end_obj.rot_w=a2[3];
	
	//std::cout << "rot[" << a[0] << "," << a[1] << "," << a[2] << "," << a[3] << "->" << a2[0] << "," << a2[1] << "," << a2[2]<<"," << a2[3] << "]";
	

      }

      if (type==2 && a && a2 && path=="scale") { // scale
	start_obj.scale_x=a[0];
	start_obj.scale_y=a[1];
	start_obj.scale_z=a[2];
	end_obj.scale_x=a2[0];
	end_obj.scale_y=a2[1];
	end_obj.scale_z=a2[2];
	//std::cout << "Scale13a:" << a[0] << " " << a[1] << " " << a[2] << std::endl;
	//std::cout << "Scale13b:" << a2[0] << " " << a2[1] << " " << a2[2] << std::endl;
	//std::cout << "sc[" << a[0] << "," << a[1] << "," << a[2] << "->" << a2[0] << "," << a2[1] << "," << a2[2] << "]";
      } 
  
      if (type==3) { // weights
	std::cout << "WEIGHTS (not implemented)" << std::endl;
	std::cout << "WEIGHT(s): " <<a[0] << " " << a[1] << " " << a[2] << " "<< a[3] << std::endl;
	std::cout << "WEIGHT(t): " <<a2[0] << " " << a2[1] << " " << a2[2] << " "<< a2[3] << std::endl;
      }

    
#endif
    } // if has_anim 
      } // if chan_target_node
    } // for i=s5
    



    std::pair<Matrix,Matrix> m = gltf_node_transform_obj_apply(env,ev,pos,start_obj );
    std::pair<Matrix,Matrix> m2 = gltf_node_transform_obj_apply(env,ev,pos2,end_obj );

    
    if (jj!=-1 && jj>=0 && jj<max_joints) { 
      root_env[jj] = pos;
      root_env_2[jj] = pos2;
      jointmatrices_start[jj] = start_obj;
      jointmatrices_end[jj] = end_obj;
      node_ids[jj] = node_id;
    } 

    // recurse children
    int s = node.children.size();
    std::vector<GameApi::ML> vec;
    for(int i=0;i<s;i++) {
      int child_id = node.children[i];
      if (child_id!=-1) {

	
	
	bool doit = true;
	//int jj = 0;
	if (doit) {

	  recurse_node( dt,prev2, child_id, 0 , m.second /*m.first*/ /*pos*/, m2.second /*m2.first*/ /*pos2*/, 0 , time_index, channel );
	}
      }
    }
    return Matrix::Identity(); //move2->get_whole_matrix(0.0,1.0);
  }
  void check_joints()
  {
    int s = node_ids.size();
    for(int i=0;i<s;i++)
      {
	if (node_ids[i]==-1) { /*node_ids[i]=0;*/ std::cout << "INDEX #" << i << " is not filled in gltf anim" << std::endl; }
      }
  }
public:
  std::vector<TransformObject> *start() { return &jointmatrices_start; }
  std::vector<TransformObject> *end() { return &jointmatrices_end; }
  std::vector<Matrix> *root() { return &root_env; }
  std::vector<Matrix> *root2() { return &root_env_2; }
  std::vector<Matrix> *bind() { return &bindmatrix; }
  const std::vector<float> *start_time() const { return &start_t; }
  const std::vector<float> *end_time() const { return &end_t; }
  int get_max_count() const { return max_count; }
  float get_max_time() const { return max_time; }
private:
  GLTFJointMatrices *prev;
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  //LoadGltf *load;
  GLTFModelInterface *interface;
  int skin_num;
  int animation;
  int time_index;
  MainLoopItem *next;
  int max_joints;
  std::vector<Matrix> root_env;
  std::vector<Matrix> root_env_2;
  //std::vector<Matrix> local;
  //std::vector<Matrix> local_2;
  std::vector<TransformObject> jointmatrices_start;
  std::vector<TransformObject> jointmatrices_end;
  std::vector<int> node_ids;
  std::vector<Matrix> bindmatrix;
  std::vector<float> start_t;
  std::vector<float> end_t;
  bool firsttime;
  bool has_anim;
  std::vector<GLTFAnimation*> anims;
  float max_time;
  int max_count;
};

GameApi::ML gltf_joint_matrices2(GameApi::ML prev, GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int animation, int time_index, GameApi::ML next, bool has_anim)
{
  MainLoopItem *item = find_main_loop(e,next);
  GLTFJointMatrices *prev2 = 0;
  if (prev.id!=0) {
    MainLoopItem *prev1 = find_main_loop(e,prev);
    prev2 = (GLTFJointMatrices*)prev1;
  }
  return add_main_loop(e, new GLTFJointMatrices(prev2,e,ev,interface, skin_num, animation, time_index, item,has_anim));
}

GLTFJointMatrices *find_joint_matrices(GameApi::Env &e, GameApi::ML ml)
{
  MainLoopItem *item = find_main_loop(e, ml);
  GLTFJointMatrices *item2 = (GLTFJointMatrices*)item;
  return item2;
}

GameApi::ML gltf_scene3( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, int animation, std::string keys, float mix, int mode, Vector light_dir, float border_width, unsigned int border_color )
{
  int s2 = interface->scenes_size(); //load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
    GameApi::P empty = ev.polygon_api.p_empty();
    GameApi::ML ml = ev.polygon_api.render_vertex_array_ml2(ev,empty);
    return ml;
    }
  const tinygltf::Scene &scene = interface->get_scene(scene_id); //&load->model.scenes[scene_id];
  int s = scene.nodes.size();
  std::vector<GameApi::ML> vec;
  for(int i=0;i<s;i++) {
    GameApi::ML ml = gltf_node2( e, ev, interface, scene.nodes[i], keys,mix,Matrix::Identity(),mode,light_dir, animation, border_width, border_color );
    //GameApi::ML ml = gltf_anim3(e,ev,load,animation, scene->nodes[i]);
    vec.push_back(ml);
  }
  return ev.mainloop_api.array_ml(ev, vec);
}
/*
GameApi::ML gltf_mesh2_with_skeleton_and_anim( GameApi::Env &e, GameApi::EveryApi &ev, LoadGltf *load, int mesh_id, int skin_id)
{
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
*/
GameApi::P gltf_node4( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int node_id )
{
  int s2 = interface->nodes_size(); //load->model.nodes.size();
  if (!(node_id>=0 && node_id<s2))
    {
      return ev.polygon_api.p_empty();
    }
  const tinygltf::Node &node = interface->get_node(node_id); //&load->model.nodes[node_id];
  int mesh = node.mesh;
  int s = interface->get_mesh(mesh).primitives.size();
  std::vector<GameApi::P> vec;
  for(int i=0;i<s;i++) {
    GameApi::P rend = gltf_load2(e,ev,interface, mesh, i);
    vec.push_back(rend);
  }
  
    // handle children
    int ss = node.children.size();
    for(int j=0;j<ss;j++) {
      int child_id = node.children[j];
      if (child_id!=-1) {
	vec.push_back( gltf_node4(e,ev,interface,child_id) );
      }
    }
    GameApi::P res = ev.polygon_api.or_array2(vec);
    if (int(node.translation.size())==3) {
      double m_x = node.translation[0];
      double m_y = node.translation[1];
      double m_z = node.translation[2];
      res = ev.polygon_api.translate(res,m_x,m_y,m_z);
    }
    if (int(node.rotation.size())==4) {
      double r_x = node.rotation[0];
      double r_y = node.rotation[1];
      double r_z = node.rotation[2];
      double r_w = node.rotation[3];
      Quarternion q = { float(r_x), float(r_y), float(r_z), float(r_w) };
      Matrix m = Quarternion::QuarToMatrix(q);
      GameApi::M mm = add_matrix2(e,m);
      res = ev.polygon_api.matrix(res,mm);
    }
    if (int(node.scale.size())==3) {
      double s_x = node.scale[0];
      double s_y = node.scale[1];
      double s_z = node.scale[2];
      res = ev.polygon_api.scale(res,s_x,s_y,s_z);
      //std::cout << "Scale14:" << s_x << " " << s_y << " " << s_z << std::endl;
    }
    return res;
}

GameApi::P gltf_scene4( GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id )
{
  int s2 = interface->scenes_size(); //load->model.scenes.size();
  if (!(scene_id>=0 && scene_id<s2))
    {
      return ev.polygon_api.p_empty();
    }
  const tinygltf::Scene &scene = interface->get_scene(scene_id); //&load->model.scenes[scene_id];
  int s = scene.nodes.size();
  std::vector<GameApi::P> vec;
  for(int i=0;i<s;i++) {
    GameApi::P p = gltf_node4(e,ev,interface,i);
    vec.push_back(p);
  }
  return ev.polygon_api.or_array2(vec);
}

GameApi::P GameApi::MainLoopApi::gltf_scene_p( GameApi::EveryApi &ev, TF model0, int scene_id )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  interface->Prepare();
  return gltf_scene4(e,ev,interface,scene_id);
}

class GltfSceneAnim : public MainLoopItem
{
public:
  GltfSceneAnim(GameApi::Env &env, GameApi::EveryApi &ev, GLTFModelInterface *interface, int scene_id, int animation, std::string keys, float mix, int mode, Vector light_dir, float border_width, unsigned int border_color) : env(env), ev(ev), interface(interface), scene_id(scene_id), animation(animation), keys(keys),mix(mix),mode(mode),light_dir(light_dir),border_width(border_width), border_color(border_color) { res.id = -1; }

  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    Prepare();
  }
  virtual void Prepare() {
    std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(env,base_url,url,gameapi_homepageurl,is_binary);
  //  new LoadGltf(e, base_url, url, gameapi_homepageurl, is_binary);
  interface->Prepare();
  GameApi::P mesh = gltf_scene2_p(env,ev, interface, scene_id,"");
  GameApi::ML ml = gltf_scene3(env,ev,interface,scene_id,animation,keys,mix,mode,light_dir, border_width, border_color);
  res= scale_to_gltf_size(env,ev,mesh,ml);
  MainLoopItem *item = find_main_loop(env,res);
  item->Prepare();
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
  //std::string base_url;
  //std::string url;
  GLTFModelInterface *interface;
  GameApi::ML res;
  int scene_id;
  int animation;
  std::string keys;
  float mix;
  int mode;
  Vector light_dir;
  float border_width;
  unsigned int border_color;
};

GameApi::ML GameApi::MainLoopApi::gltf_scene_anim( GameApi::EveryApi &ev, TF model0, int scene_id, int animation, std::string keys, float mix,int mode, float light_dir_x, float light_dir_y, float light_dir_z, float border_width, unsigned int border_color )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  return add_main_loop(e, new GltfSceneAnim(e,ev,interface,scene_id,animation,keys,mix,mode,Vector(light_dir_x, light_dir_y, light_dir_z),border_width,border_color));

}

GameApi::ML GameApi::MainLoopApi::gltf_anim4( GameApi::EveryApi &ev, TF model0, int animation, int channel, float mix, int mode, float light_dir_x, float light_dir_y, float light_dir_z, float border_width, unsigned int border_color )
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  interface->Prepare();
  GameApi::P mesh = gltf_load2(e,ev, interface, 0,0);
  std::vector<GameApi::ML> mls;

  AnimData *dt = new AnimData;
  GLTFAnimation *anim = new GLTFAnimation(dt,interface,animation, channel, 0,0);
      anim->Prepare();
      
      int target_node = anim->target_node();
      delete anim;
      GameApi::ML ml = gltf_anim3(e,ev,interface,animation, target_node,mix,mode,Vector(light_dir_x,light_dir_y,light_dir_z),border_width, border_color);
    return scale_to_gltf_size(e,ev,mesh,ml);
}




class GLTF_Att : public Attach
{
public:
  GLTF_Att( GLTFModelInterface *interface, int mesh_index, int prim_index, int num ) : interface(interface), mesh_index(mesh_index), prim_index(prim_index), num(num) { }
  void Collect(CollectVisitor &vis)
  {
    interface->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (mesh_index>=0 && mesh_index<int(interface->meshes_size()) && prim_index>=0 && prim_index<int(interface->get_mesh(mesh_index).primitives.size()))
      prim = const_cast<tinygltf::Primitive*>(&interface->get_mesh(mesh_index).primitives[prim_index]);
    else
      return;
    
    //std::cout << "MESH: " << load->model.meshes[mesh_index].name << std::endl;
    
    //model = &load->model;
    
    mode = prim->mode;

    indices_index = prim->indices;
    //indices_acc = 0;
    indices_acc_done = false;
    if (indices_index!=-1) {
      indices_acc = &interface->get_accessor(indices_index); //&model->accessors[indices_index];
      indices_acc_done = true;
    }

    assert(indices_acc->type==TINYGLTF_TYPE_SCALAR);


    //indices_bv = 0;
    indices_bv_done = false;
    if (indices_acc_done) {
      int view = indices_acc->bufferView;
      if (view!=-1) {
	indices_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	indices_bv_done = true;
      }
    }

    //indices_buf = 0;
    indices_buf_done = false;
    if (indices_bv_done) {
      int buf = indices_bv->buffer;
      if (buf!=-1) {
	indices_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	indices_buf_done = true;
      }
    }

    
    
    joints_index = -1;
    if (prim->attributes.find("JOINTS_0") != prim->attributes.end())
      joints_index = prim->attributes["JOINTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    //joints_acc = 0;
    joints_acc_done = false;
    if (joints_index!=-1) {
      joints_acc = &interface->get_accessor(joints_index); //&model->accessors[joints_index];
      joints_acc_done = true;
    }
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    //joints_bv = 0;
    joints_bv_done = false;
    if (joints_acc_done) {
      int view = joints_acc->bufferView;
      if (view!=-1) {
	joints_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	joints_bv_done = true;
      }
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    //joints_buf = 0;
    joints_buf_done = false;
    if (joints_bv_done) {
      int buf = joints_bv->buffer;
      if (buf!=-1) {
	joints_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	joints_buf_done = true;
      }
    }





    weights_index = -1;
    if (prim->attributes.find("WEIGHTS_0") != prim->attributes.end())
      weights_index = prim->attributes["WEIGHTS_0"];

    //std::cout << "joints_index: " << joints_index << std::endl;
    
    //weights_acc = 0;
    weights_acc_done = false;
    if (weights_index!=-1) {
      weights_acc = &interface->get_accessor(weights_index); //&model->accessors[weights_index];
      weights_acc_done = true;
    }
    //std::cout << "joints_acc: " << joints_acc << std::endl;
    
    // weights_bv = 0;
    weights_bv_done = false;
    if (weights_acc_done) {
      int view = weights_acc->bufferView;
      if (view!=-1) {
	weights_bv = &interface->get_bufferview(view); //&model->bufferViews[view];
	weights_bv_done = true;
      }
    }
    
    //std::cout << "joints_bv: " << joints_bv << std::endl;


    //weights_buf = 0;
    weights_buf_done = false;
    if (weights_bv_done) {
      int buf = weights_bv->buffer;
      if (buf!=-1) {
	weights_buf = &interface->get_buffer(buf); //&model->buffers[buf];
	weights_buf_done = false;
      }
    }


    
    //std::cout << "joints_buf: " << joints_buf << std::endl;
  }
  void Prepare()
  {
    
    interface->Prepare();
    HeavyPrepare();
    /*    
    if (mesh_index>=0 && mesh_index<int(interface->meshes_size()) && prim_index>=0 && prim_index<int(load->model.meshes[mesh_index].primitives.size()))
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

    */
    
    //std::cout << "joints_buf: " << joints_buf << std::endl;
	
  }


  int get_index(int face, int point) const
  {
	const unsigned char *ptr = &indices_buf->data[0];
	//int size1 = indices_buf->data.size();
	const unsigned char *ptr2 = ptr + indices_bv->byteOffset;
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
	const unsigned char *ptr3 = ptr2 + indices_acc->byteOffset + comp*stride;
	//int size3 = indices_acc->count;
	int index = 0;
	switch(indices_acc->componentType)
	  {
	  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	    {
	      const unsigned char *ptr4 = (const unsigned char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	  case TINYGLTF_COMPONENT_TYPE_BYTE:
	    {
	      const char *ptr4 = (const char*)ptr3;
	      index = *ptr4;
	      break;
	    }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	  {
	  const unsigned short *ptr4 = (const unsigned short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	  {
	  const short *ptr4 = (const short*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_INT:
	  {
	  const int *ptr4 = (const int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	  {
	  const unsigned int *ptr4 = (const unsigned int*)ptr3;
	  index = *ptr4; // this is the index to the buffer
	  break;
	  }
	  };
	return index;
  }

  virtual int Weights(int face, int point, int num) const
  {
    if (weights_bv_done && weights_acc_done && weights_buf_done) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_acc_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &weights_buf->data[0];
	int stride2 = weights_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	const float *pos_ptr2 = (const float*)(pos_ptr + weights_bv->byteOffset + index*stride2 + weights_acc->byteOffset); 
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
	const unsigned char *pos_ptr = &weights_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + weights_bv->byteOffset;
	int stride = weights_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + weights_acc->byteOffset + comp*stride;
	const float *pos_ptr4 = (const float*)pos_ptr3; // 3 = num of components in (x,y,z)
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
    if (joints_bv_done && joints_acc_done && joints_buf_done) {
      if (mode==TINYGLTF_MODE_TRIANGLES) {
      if (indices_buf_done && indices_bv_done && indices_acc_done) {
      
	int index = get_index(face,point);

	const unsigned char *pos_ptr = &joints_buf->data[0];
	int stride2 = joints_bv->byteStride;
	if (stride2==0) stride2 = 4*sizeof(unsigned char); // 3 = num of components in (x,y,z)
	const unsigned char *pos_ptr2 = (const unsigned char*)(pos_ptr + joints_bv->byteOffset + index*stride2 + joints_acc->byteOffset); 
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
	const unsigned char *pos_ptr = &joints_buf->data[0];
	const unsigned char *pos_ptr2 = pos_ptr + joints_bv->byteOffset;
	int stride = joints_bv->byteStride;
	if (stride==0) stride=4*sizeof(char);
	int comp = face*3+point;
	const unsigned char *pos_ptr3 = pos_ptr2 + joints_acc->byteOffset + comp*stride;
	const unsigned char *pos_ptr4 = (const unsigned char*)pos_ptr3; // 3 = num of components in (x,y,z)
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
  //tinygltf::Model *model =0 ;

  int mode;

  int num = 0;
  
  int indices_index;
  int joints_index;
  int weights_index;
  const tinygltf::Accessor *indices_acc;
  const tinygltf::Accessor *joints_acc;
  const tinygltf::Accessor *weights_acc;
  
  const tinygltf::BufferView *indices_bv;
  const tinygltf::BufferView *joints_bv;
  const tinygltf::BufferView *weights_bv;
  
  const tinygltf::Buffer *indices_buf;
  const tinygltf::Buffer *joints_buf;
  const tinygltf::Buffer *weights_buf;


  bool indices_acc_done=false;
  bool joints_acc_done=false;
  bool weights_acc_done=false;
  
  bool indices_bv_done=false;
  bool joints_bv_done=false;
  bool weights_bv_done=false;
  
  bool indices_buf_done=false;
  bool joints_buf_done=false;
  bool weights_buf_done=false;

  
  GLTFModelInterface *interface;
  int mesh_index;
  int prim_index;
  
};

GameApi::ATT gltf_attach(GameApi::Env &e, GLTFModelInterface *interface, int mesh_index, int prim_index, int num)
{
  return add_attach(e, new GLTF_Att(interface,mesh_index, prim_index, num));
}

GameApi::ARR gltf_split_faces(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int mesh_index, int prim_index, int max_attach, int num)
{
  GameApi::P p = gltf_load2(e,ev, interface, mesh_index, prim_index);
  GameApi::P p2 = scale_to_gltf_size_p(e,ev, p, p);
  GameApi::ATT att = gltf_attach(e,interface, mesh_index, prim_index, num);
  return ev.polygon_api.split_faces(p2, att, max_attach);
}

GameApi::ARR GameApi::PolygonApi::gltf_split_faces2(EveryApi &ev, TF model0, int mesh_index, int prim_index, int max_attach)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }

  //LoadGltf *load = find_gltf_instance(e,base_url, url, gameapi_homepageurl, is_binary);
  return gltf_split_faces(e, ev,interface,mesh_index, prim_index, max_attach, 0); // 0=index of joint to choose from 4 alternatives 0|1|2|3.
}


struct MLS_cache_item
{
  int id;
  GameApi::ML orig;
  GameApi::ML ml;
  std::vector<GameApi::ML> vec;
};
std::vector<MLS_cache_item> g_mls_cache;
int g_mls_id=0;
void mls_add_to_cache(int id, GameApi::ML orig, GameApi::ML ml, std::vector<GameApi::ML> vec)
{
  //std::cout << "push to cache:" << id << std::endl;
  MLS_cache_item ii;
  ii.id = id;
  ii.orig = orig;
  ii.ml = ml;
  ii.vec = vec;
  g_mls_cache.push_back(ii);
}
std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> find_mls_cache_item(int id, bool &success)
{
  
  success=true;
  int s = g_mls_cache.size();
  for(int i=0;i<s;i++) { if (g_mls_cache[i].id==id) { /*std::cout << "found from cache" << id << std::endl;*/ return std::make_tuple(g_mls_cache[i].vec,g_mls_cache[i].orig,g_mls_cache[i].ml); } }
  success=false;
  //std::cout << "not found from cache" << id << std::endl;
  GameApi::ML ml; ml.id = -1;
  return std::make_tuple(std::vector<GameApi::ML>(),ml,ml);
}


class PointArrayLineCollection_tt : public LineCollection
{
public:
  PointArrayLineCollection_tt(std::vector<Point> vec) : vec(vec) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }

  void Prepare() { }
  int NumLines() const { return vec.size()-1; }
  Point LinePoint(int line, int point) const
  {
    int i = line*2 + point;
    return vec[i];
  }
  unsigned int LineColor(int line, int point) const
  {
    return 0xffffffff;
  }
private:
  std::vector<Point> vec;
};



class GLTF_Animation_Material : public MaterialForward
{
public:
  GLTF_Animation_Material(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int animation, int num_timeindexes, Material *next, int key, int mode, int inst) : e(e), ev(ev), interface(interface), skin_num(skin_num), animation(animation), num_timeindexes(num_timeindexes), next(next), key(key),mode(mode) {

    cache_key = animation + 10*skin_num + 100*inst;
    //std::cout << "CACHE_KEY: " << cache_key << std::endl;
  }

  void calc_num_timeindexes() const
  {
    int sz = interface->animations_size();
    if (!(animation>=0 && animation<sz)) return;
    const tinygltf::Animation &anim = interface->get_animation(animation);
    int s = anim.channels.size();
    int count=0;
    for(int i=0;i<s;i++)
      {
	AnimData dt;
	GLTFAnimation anim(&dt,interface,animation,i,0,skin_num);
	anim.Prepare();
	int val = anim.Count();
	//count+=val;
	if (val>count) count=val;
      }
    //std::cout << "num_timeindexes=" << count << std::endl;
    num_timeindexes=count;
  }

  GameApi::LI draw_pose( GameApi::Env &env, std::vector<GameApi::ML> mls ) const
  {
    MainLoopItem *item = find_main_loop( env, mls[0] );
    GLTFJointMatrices *mat = (GLTFJointMatrices*)item;
    const std::vector<Matrix> *joint = mat->root();
    const std::vector<Matrix> *joint2 = mat->bind();
    std::vector<Point> vec;
    for(size_t i=0;i<joint->size();i++)
      {
	const Matrix &m = (*joint).operator[](i);
	const Matrix &m2 = ((*joint2).operator[](i));
	Point p = m.get_translate();
	Point p2 = m2.get_translate();
	vec.push_back(p);
	vec.push_back(p2);
      }
    return add_line_array(e, new PointArrayLineCollection_tt(vec));
  }

  

  
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    interface->Prepare();
    calc_num_timeindexes();
    std::vector<GameApi::ML> mls;
    bool success=false;
    std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> mlsi = find_mls_cache_item(cache_key, success);
    GameApi::ML ml_orig;
    GameApi::ML ml;
    if (success) {
      mls = std::get<0>(mlsi);
      ml_orig = std::get<1>(mlsi);
      ml = std::get<2>(mlsi);
    }
    GameApi::ML ml_start;
    ml_start.id = 0;
    
    if (!success) {
    ml.id = next->mat(p.id);
    if (num_timeindexes==0) { return ml; }
    ml_orig = gltf_joint_matrices2(ml_start,e,ev,interface,skin_num,animation,0,ml,false);
    //std::cout << "NUM_TIMEINDEXES: " << num_timeindexes << std::endl;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML prev = i==0?ml_orig:mls[i-1];
      GameApi::ML ml1 = gltf_joint_matrices2(prev,e,ev, interface, skin_num, animation, i, ml, true);
      mls.push_back(ml1);
    }
    mls_add_to_cache(cache_key,ml_orig,ml,mls);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key,mode,num_timeindexes);
    //GameApi::LI li = draw_pose( e, mls ); // DEBUG
    //GameApi::ML ml3 = ev.lines_api.render_ml2(ev, li, 2.0); // DEBUG
    //return ev.mainloop_api.array_ml(ev,{ml2,ml3}); // DEBUG
    return ml2;
  }
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    interface->Prepare();
    calc_num_timeindexes();
    GameApi::ML ml;
    std::vector<GameApi::ML> mls;
    bool success=false;
    GameApi::ML ml_orig;
    std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> mlsi = find_mls_cache_item(cache_key, success);
    if (success) {
      mls=std::get<0>(mlsi);
      ml_orig=std::get<1>(mlsi);
      ml=std::get<2>(mlsi);
      }
    GameApi::ML ml_start;
    ml_start.id = 0;
    
    if (!success) {
    ml.id = next->mat_inst(p.id, pts.id);
    if (num_timeindexes==0) { return ml; }
    ml_orig  = gltf_joint_matrices2(ml_start,e,ev,interface,skin_num,animation,0,ml,false);
    //std::cout << "NUM_TIMEINDEXES: " << num_timeindexes << std::endl;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML prev = i==0?ml_orig:mls[i-1];
      GameApi::ML ml1 = gltf_joint_matrices2(prev,e,ev, interface, skin_num, animation, i, ml, true);
      mls.push_back(ml1);
    }
    mls_add_to_cache(cache_key,ml_orig,ml,mls);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig, mls,key,mode,num_timeindexes);
    return ml2;
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    interface->Prepare();
    calc_num_timeindexes();
    GameApi::ML ml;
    std::vector<GameApi::ML> mls;
    bool success=false;
    GameApi::ML ml_orig;
    std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> mlsi = find_mls_cache_item(cache_key, success);
    if (success) {
      mls = std::get<0>(mlsi);
      ml_orig = std::get<1>(mlsi);
      ml=std::get<2>(mlsi);
    }
    GameApi::ML ml_start;
    ml_start.id = 0;
    
    if (!success) {
      ml.id = next->mat_inst_matrix(p.id, ms.id);
      if (num_timeindexes==0) { return ml; }
      ml_orig = gltf_joint_matrices2(ml_start,e,ev,interface,skin_num,animation,0,ml,false);
      //std::cout << "NUM_TIMEINDEXES: " << num_timeindexes << std::endl;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML prev = i==0?ml_orig:mls[i-1];
      GameApi::ML ml1 = gltf_joint_matrices2(prev,e,ev, interface, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    mls_add_to_cache(cache_key,ml_orig,ml,mls);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key,mode,num_timeindexes);
    return ml2;
  }
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const
  {
    interface->Prepare();
    calc_num_timeindexes();
    GameApi::ML ml;
    std::vector<GameApi::ML> mls;
    bool success=false;
    GameApi::ML ml_orig;
    std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> mlsi = find_mls_cache_item(cache_key, success);
    if (success) { mls=std::get<0>(mlsi); ml_orig=std::get<1>(mlsi); ml=std::get<2>(mlsi); }
    GameApi::ML ml_start;
    ml_start.id = 0;
    if (!success) {
    ml.id = next->mat_inst2(p.id, pta.id);
    if (num_timeindexes==0) { return ml; }
    ml_orig = gltf_joint_matrices2(ml_start,e,ev,interface,skin_num,animation,0,ml,false);

    //  std::cout << "NUM_TIMEINDEXES: " << num_timeindexes << std::endl;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML prev = i==0?ml_orig:mls[i-1];
      GameApi::ML ml1 = gltf_joint_matrices2(prev,e,ev, interface, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    mls_add_to_cache(cache_key,ml_orig,ml,mls);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key,mode,num_timeindexes);
    return ml2;
  }
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const
  {
    interface->Prepare();
    calc_num_timeindexes();
    GameApi::ML ml;
    GameApi::ML ml_start;
    ml_start.id = 0;
    GameApi::ML ml_orig;
    std::vector<GameApi::ML> mls;
    bool success=false;
    std::tuple<std::vector<GameApi::ML>,GameApi::ML,GameApi::ML> mlsi = find_mls_cache_item(cache_key, success);
    if (success) {
      mls = std::get<0>(mlsi);
      ml_orig=std::get<1>(mlsi);
      ml=std::get<2>(mlsi);
    }
    if (!success) {
    ml.id = next->mat_inst_fade(p.id, pts.id, flip, start_time, end_time);
    if (num_timeindexes==0) { return ml; }
      ml_orig = gltf_joint_matrices2(ml_start,e,ev,interface,skin_num,animation,0,ml,false);
      //   std::cout << "NUM_TIMEINDEXES: " << num_timeindexes << std::endl;
    for(int i=0;i<num_timeindexes;i++) {
      GameApi::ML prev = i==0?ml_orig:mls[i-1];
      GameApi::ML ml1 = gltf_joint_matrices2(prev,e,ev, interface, skin_num, animation, i, ml,true);
      mls.push_back(ml1);
    }
    mls_add_to_cache(cache_key,ml_orig,ml,mls);
    }
    GameApi::ML ml2 = ev.polygon_api.gltf_anim_shader(ev,ml_orig,mls,key,mode,num_timeindexes);
    return ml2;
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Material *next;
  GLTFModelInterface *interface;
  int skin_num;
  int animation;
  mutable int num_timeindexes=0;
  int key;
  int mode;
  int cache_key;
};

char key_mapping(char ch, int type);

class CacheMLmat : public MainLoopItem
{
public:
  CacheMLmat(GameApi::Env &env, GameApi::P p, std::vector<Material*> vec, int i, std::string keys) : env(env), p(p), vec(vec),i(i),keys(keys) { firsttime=true;  firsttime2=true; ml.id=-1;}
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime2 && i==0) {
      ml.id = vec[i]->mat(p.id);
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
      
      firsttime = false;
      firsttime2=false;
    }
    if (ml.id!=-1) {
      //std::cout << "execute " << i << std::endl;
      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int ch = key_mapping(e.ch,e.type);
    //if (e.type==0x300) std::cout << "CH2: i=" << i << " -- " << ch << "==" << int(keys[i]) << " @@ " << std::hex << e.type << "==0x300" << std::endl;
    if (firsttime && (i<keys.size() && e.type==0x300 && ch==int(keys[i]))) {
      //std::cout << "prepare3 " << i << std::endl;
      ml.id = vec[i]->mat(p.id);
      firsttime = false;
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
    }
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
    
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      return item->shader_id();
    } else return std::vector<int>();
  }
private:
  GameApi::Env &env;
  std::vector<Material*> vec;
  bool firsttime;
  bool firsttime2;
  GameApi::ML ml;
  GameApi::P p;
  int i;
  std::string keys;
};
/*
std::vector<GameApi::ML> cache_creation_mat(GameApi::Env &env, GameApi::P p, std::vector<Material*> vec)
{
  int s = vec.size();
  std::vector<GameApi::ML> res;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = new CacheMLmat(env,p,vec,i);
      res.push_back(add_main_loop(env,item));
    }
  return res;
}
*/
class CacheMLmatinst : public MainLoopItem
{
public:
  CacheMLmatinst(GameApi::Env &env, GameApi::P p, GameApi::PTS pts, std::vector<Material*> vec, int i, std::string keys) : env(env), p(p), pts(pts), vec(vec),i(i), keys(keys) { firsttime=true; firsttime2=true; ml.id=-1;}
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime2 && i==0) {
      ml.id = vec[i]->mat(p.id);
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
      
      firsttime = false;
      firsttime2=false;
    }
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    char ch = key_mapping(e.ch,e.type);
    if (firsttime && i<keys.size() && e.type==0x300 && ch==keys[i]) {
      //std::cout << "prepare2 " << i << std::endl;
      ml.id = vec[i]->mat_inst(p.id,pts.id);
      firsttime = false;
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
    }
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
    
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      return item->shader_id();
    } else return std::vector<int>();
  }
private:
  GameApi::Env &env;
  std::vector<Material*> vec;
  bool firsttime;
  bool firsttime2;
  GameApi::ML ml;
  GameApi::P p;
  GameApi::PTS pts;
  int i;
  std::string keys;
};

class CacheMLmatinstmatrix : public MainLoopItem
{
public:
  CacheMLmatinstmatrix(GameApi::Env &env, GameApi::P p, GameApi::MS ms, std::vector<Material*> vec, int i, std::string keys) : env(env), p(p), ms(ms), vec(vec),i(i),keys(keys) { firsttime=true; firsttime2=true; ml.id=-1;}
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime2 && i==0) {
      ml.id = vec[i]->mat(p.id);
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
      
      firsttime = false;
      firsttime2=false;
    }
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    char ch = key_mapping(e.ch,e.type);
    if (firsttime && i<keys.size() && e.type==0x300 && ch==keys[i]) {
      //std::cout << "prepare " << i << std::endl;
      ml.id = vec[i]->mat_inst_matrix(p.id,ms.id);
      firsttime = false;
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
    }
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
    
  virtual std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      return item->shader_id();
    } else return std::vector<int>();
  }
private:
  GameApi::Env &env;
  std::vector<Material*> vec;
  bool firsttime;
  bool firsttime2;
  GameApi::ML ml;
  GameApi::P p;
  GameApi::MS ms;
  int i;
  std::string keys;
};


std::vector<GameApi::ML> cache_creation_mat(GameApi::Env &env, GameApi::P p, std::vector<Material*> vec, std::string keys)
{
  int s = vec.size();
  std::vector<GameApi::ML> res;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = new CacheMLmat(env,p,vec,i,keys);
      res.push_back(add_main_loop(env,item));
    }
  return res;
}

std::vector<GameApi::ML> cache_creation_matinst(GameApi::Env &env, GameApi::P p,GameApi::PTS pts, std::vector<Material*> vec, std::string keys)
{
  int s = vec.size();
  std::vector<GameApi::ML> res;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = new CacheMLmatinst(env,p,pts,vec,i,keys);
      res.push_back(add_main_loop(env,item));
    }
  return res;
}

std::vector<GameApi::ML> cache_creation_matinstmatrix(GameApi::Env &env, GameApi::P p, GameApi::MS ms, std::vector<Material*> vec, std::string keys)
{
  int s = vec.size();
  std::vector<GameApi::ML> res;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = new CacheMLmatinstmatrix(env,p,ms,vec,i,keys);
      res.push_back(add_main_loop(env,item));
    }
  return res;
}




class KeysMaterial : public MaterialForward
{
public:
  KeysMaterial(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<Material*> vec, std::string keys) : env(env), ev(ev), vec(vec), keys(keys) { }
  virtual GameApi::ML mat2(GameApi::P p) const
  {
    /*
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat(p.id);
      vec2.push_back(ml);
    }
    */
    return ev.mainloop_api.key_ml(cache_creation_mat(env,p,vec,keys), keys);
  }
  
  virtual GameApi::ML mat2_inst(GameApi::P p, GameApi::PTS pts) const
  {
    /*
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst(p.id, pts.id);
      vec2.push_back(ml);
      }*/
    return ev.mainloop_api.key_ml(cache_creation_matinst(env,p,pts,vec,keys), keys);
  }
  virtual GameApi::ML mat2_inst_matrix(GameApi::P p, GameApi::MS ms) const
  {
    /*
    int s = vec.size();
    std::vector<GameApi::ML> vec2;
    for(int i=0;i<s;i++) {
      GameApi::ML ml;
      ml.id = vec[i]->mat_inst_matrix(p.id, ms.id);
      vec2.push_back(ml);
      }*/
    return ev.mainloop_api.key_ml(cache_creation_matinstmatrix(env,p,ms,vec,keys), keys);
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
  GameApi::Env &env;
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
  return add_material(e,new KeysMaterial(e,ev, vec2, keys));
}


GameApi::MT gltf_anim_material4(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int animation, int num_timeindexes, GameApi::MT next, int key,int mode, int inst, int timeid)
{
  Material *next_mat = find_material(e,next);
  Material *mat = new GLTF_Animation_Material(e,ev,interface, skin_num, animation, num_timeindexes, next_mat, key,mode,inst);
  return add_material(e, mat);
}


GameApi::MT gltf_anim_material3(GameApi::Env &e, GameApi::EveryApi &ev, GLTFModelInterface *interface, int skin_num, int /*num_timeindexes*/, GameApi::MT next, std::string keys, int mode, int inst)
{
  int s = keys.size();
  std::vector<GameApi::MT> vec;
  for(int i=0;i<s;i++) {
    int mm = gltf_mesh2_calc_max_timeindexes(interface,i,skin_num);
    GameApi::MT mt = gltf_anim_material4(e,ev,interface, skin_num, i, mm /*num_timeindexes*/, next, keys[i],mode,inst,mm);
    vec.push_back(mt);
  }
  return ev.materials_api.m_keys(ev,vec,keys);

}


GameApi::MT GameApi::MaterialsApi::gltf_anim_material2(GameApi::EveryApi &ev, TF model0, int skin_num, int num_timeindexes, MT next, std::string keys, int mode)
{
  //LTFModelInterface *interface = find_gltf(e,model0);
  int s = keys.size();
  std::vector<GameApi::MT> vec;
  for(int i=0;i<s;i++) {
    GameApi::MT mt = gltf_anim_material(ev,model0, skin_num, i, num_timeindexes, next, keys[i],mode);
    vec.push_back(mt);
  }
  return m_keys(ev,vec,keys);
}


GameApi::MT GameApi::MaterialsApi::gltf_anim_material(GameApi::EveryApi &ev, TF model0, int skin_num, int animation, int num_timeindexes, MT next, int key, int mode)
{
  GLTFModelInterface *interface = find_gltf(e,model0);
  std::string url = interface->Url();
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  //LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  Material *next_mat = find_material(e,next);
  Material *mat = new GLTF_Animation_Material(e,ev,interface, skin_num, animation, num_timeindexes, next_mat, key,mode,animation+get_cache_id(interface));
  return add_material(e, mat);
}
extern Matrix g_last_resize;

char key_mapping(char ch, int type);

float fix_num(float x)
{
  if (std::isnan(x)) x=0.0;
  if (std::isinf(x)) x=0.0;
  return x;
}
float fix_num2(float x)
{
  if (std::isnan(x)) x=1.0;
  if (std::isinf(x)) x=1.0;
  return x;
}

float fix_rot(int index, float x)
{
  if (x>500.0||x<-500.0) { return 0.0; }
  x=fix_num(x);
  return x;
}
float fix_trans(int index,float x)
{
  if (x>5500.0||x<-5500.0) return 0.0;
  x=fix_num(x);
  return x;
}
float fix_scale(int index, float x)
{
  //if (x!=1.0)
  //  std::cout << "scale:" << x << std::endl;
  if (fabs(x)>300.0||fabs(x)<1.0/300.0) { if (x<0.0) { x=-1.0; } else  { x=1.0; } }
  //x=fabs(x);
  x=fix_num2(x);
  //if (x!=1.0)
  //  std::cout << "scale res: " << x << std::endl;
  return x;
}

float fix_zero(int index, float x)
{
  return 0.0;
}
float fix_one(int index, float x)
{
  x = fix_num2(x);
  if (x>1000.0||x<-1000.0) x=1.0;
  return x;
}

Matrix fix_matrix(Matrix m)
{
  m.matrix[0] = fix_scale(0,m.matrix[0]);
  m.matrix[5] = fix_scale(1+4,m.matrix[5]);
  m.matrix[10] = fix_scale(2+8,m.matrix[10]);
  m.matrix[15] = fix_one(3+8+4,m.matrix[15]);

  m.matrix[3] = fix_trans(3,m.matrix[3]);
  m.matrix[7] = fix_trans(3+4,m.matrix[7]);
  m.matrix[11] = fix_trans(3+8,m.matrix[11]);

  
  
  
  m.matrix[1] = fix_rot(1,m.matrix[1]);
  m.matrix[2] = fix_rot(2,m.matrix[2]);

  m.matrix[4] = fix_rot(0+4,m.matrix[4]);
  m.matrix[6] = fix_rot(2+4,m.matrix[6]);

  m.matrix[8] = fix_rot(0+8,m.matrix[8]);
  m.matrix[9] = fix_rot(1+8,m.matrix[9]);
  
  m.matrix[12] = fix_zero(0+8+4,m.matrix[12]);
  m.matrix[13] = fix_zero(1+8+4,m.matrix[13]);
  m.matrix[14] = fix_zero(2+8+4,m.matrix[14]);  return m;
  //std::cout << "START:"<<std::endl << m << std::endl;

  //std::cout << "END:"<<std::endl << m << std::endl;
  return m;
}

extern Matrix g_last_resize2;

int g_time_id;

class GltfAnimShaderML : public MainLoopItem
{
public:
  GltfAnimShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *ml_orig, std::vector<MainLoopItem*> items, int key, int mode, int id) : env(env), ev(ev), ml_orig(ml_orig),items(items), key(key),mode(mode),id(id) { firsttime=true; resize=Matrix::Identity(); keypressed = false;
    current_time=key_time=ev.mainloop_api.get_time()/1000.0;
    //keypressed =true;
}
  std::vector<int> shader_id() {
    return ml_orig->shader_id();
  }
  void handle_event(MainLoopEvent &e)
  {
    int ch = key_mapping(e.ch,e.type);

    /*
    if (ch>='0' && ch<='9' && e.type==0x300) {
      curr++;
      std::cout << "curr=" << curr << " count=" << count << std::endl;
      if (curr>=count) {
	feature_enable[ch-'0']=!feature_enable[ch-'0'];
	std::cout << "Feature_enable[" << ch-'0' << "]=" << feature_enable[ch-'0'] << std::endl;
	curr=0;
      }
    }
    */    
    if (ch==key &&e.type==0x300) {
      key_time = current_time/10.0; //ev.mainloop_api.get_time()/1000.0; /*current_time;*/
      keypressed = true;
    }
    if (items.size()>0)
      items[0]->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    int s = items.size();
    for(int i=0;i<s;i++) {
      if (items[i])
	items[i]->Collect(vis);
    }
    if (ml_orig)
      ml_orig->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    resize = g_last_resize; // this communicates with resize_to_correct_size()
    resizei = Matrix::Inverse(resize);
  }
  void Prepare() {
    int s = items.size();
    for(int i=0;i<s;i++) {
      if (items[i])
	items[i]->Prepare();
    }
    if (ml_orig)
      ml_orig->Prepare();
    resize = g_last_resize; // this communicates with resize_to_correct_size()
    resizei = Matrix::Inverse(resize);
  }
  void logoexecute() {
    items[0]->logoexecute();
  }
  struct TimeStore { int id; float time; };
  void execute(MainLoopEnv &e) {
    current_time = e.time;
    float time = e.time-key_time;
    static std::vector<TimeStore> timevec;
    int s = timevec.size();
    bool timedone = false;
    for(int i=0;i<s;i++)
      {
	TimeStore &tv = timevec[i];
	if (tv.id==id+300*g_time_id) {
	  time = tv.time;
	  timedone = true;
	}
      }
    if(keypressed && max_end_time>0.0001 && time>max_end_time) { key_time = ev.mainloop_api.get_time()/1000.0; time=e.time-key_time; timevec.clear(); }// repeat
    if (!timedone)
      {
	TimeStore t;
	t.id = id+300*g_time_id;
	t.time = time;
	timevec.push_back(t);
      }


    
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
	std::vector<GameApi::M> vec_bindm;
	std::vector<GameApi::M> vec_resize;
	std::vector<GameApi::M> vec_resizei;
	int sz = 1;
	int iisz = r_0->size();
	for(int ii=0;ii<iisz;ii++)
	  {
	    current = -1;
	    const std::vector<float> *current_start_time=0, *current_end_time=0;
	    int sz = items.size();
	    //GLTFJointMatrices *joints2 = (GLTFJointMatrices*)(items[sz-1]);		    //float finish_time = joints2->get_max_time();
		//std::cout << finish_time << std::endl;
	    for(int t=0;t<sz;t++)
	      {
		GLTFJointMatrices *joints = (GLTFJointMatrices*)(items[t]);
		const std::vector<float> *start_time = joints->start_time();
		const std::vector<float> *end_time = joints->end_time();

		int sr = end_time->size();
		for(int iu=0;iu<sr;iu++) {
		  float finish_time = end_time->operator[](iu);
		  if (finish_time>max_end_time) max_end_time = finish_time;
		}
		
		if (current==-1 && t==0) {
		  current_start_time=start_time;
		  current_end_time=end_time;
		}

		
		//int u = end_time->size();
		//if (ii<u && end_time->operator[](ii)>max_end_time) max_end_time=end_time->operator[](ii);

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
	    if (current==-1) {
	      static std::vector<float> *start_time2 = 0;
	      static std::vector<float> *end_time2 = 0;
	      delete start_time2;
	      delete end_time2;
	      start_time2 = new std::vector<float>;
	      end_time2 = new std::vector<float>;
	      int s = current_start_time->size();
	      for(int i=0;i<s;i++)
		start_time2->push_back(current_start_time->operator[](i)+max_end_time);
	      int s2 = current_end_time->size();
	      for(int i=0;i<s2;i++)
		end_time2->push_back(current_end_time->operator[](i)+max_end_time);
	      
	      current_start_time = start_time2;
	      current_end_time = end_time2;
	      current=0;
	    }

	    
	    MainLoopItem *next = items[current];
	    GLTFJointMatrices *joints = (GLTFJointMatrices*)next;
	    std::vector<TransformObject> *start = joints->start();
	    std::vector<TransformObject> *end = joints->end();
	    std::vector<Matrix> *r = joints->root();
	    std::vector<Matrix> *r_2 = joints->root2();
	    std::vector<Matrix> *j_bind = joints->bind();
	    //std::vector<Matrix> *l = joints->local_trans();
	    //std::vector<Matrix> *l_2 = joints->local_trans2();
	    //std::cout << "sizes:" << start_0->size() << " " << start->size() << " " << end->size() << std::endl;
	    sz = std::min(start_0->size(),std::min(start->size(),end->size()));

	    float time01 = (time-current_start_time->operator[](ii))/(current_end_time->operator[](ii)-current_start_time->operator[](ii));

	    if (std::isinf(time01)) time01 = 0.0;
	    if (time01<0.0) time01=0.0;
	    if (time01>1.0) time01=1.0;


	    int good_node = sz-1;
	    
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

	    
	    Matrix jb;
	    if (ii<sz)
	      jb = j_bind->operator[](ii);
	    else jb=Matrix::Identity();
	    
	    TransformObject m0t;
	    if (ii<sz)
	      m0t = start_0->operator[](ii);
	    else m0t = gltf_node_default();

	    Matrix rr_interpolate = rr;
	    rr_interpolate*=(1.0-time01);
	    Matrix rr_interpolate2=rr2;
	    rr_interpolate2*=time01;
	    Matrix rr_int = rr_interpolate + rr_interpolate2;

	    Matrix m0 = gltf_node_transform_obj_apply(env,ev,rr0,m0t).second;	    
	    Matrix m0i = Matrix::Inverse(m0);

	    Matrix bindm;
	    if (ii<sz)
	      bindm=bind->operator[](ii);
	    else { bindm = Matrix::Identity();
	    }
	    
	    TransformObject obj = slerp_transform(start_obj,end_obj,time01);

	    Matrix mr;
	    for(int j=0;j<16;j++) mr.matrix[j]=(time01)*rr2.matrix[j] + (1.0-time01)*rr.matrix[j];


	    Matrix mv = gltf_node_transform_obj_apply(env,ev,mr,obj).second;
	    Matrix m = mv; // TODO, THIS SHOULD BE MULTIPLIED BY BINDM
	    
	    Matrix ri = resizei; //Matrix::Inverse(resize);

	    if (Matrix::has_nan(ri)) { ri=Matrix::Identity();  }
	    if (Matrix::has_nan(m0i)) { m0i=Matrix::Identity(); }
	    if (Matrix::has_nan(m0)) { m0=Matrix::Identity(); }
	    if (Matrix::has_nan(bindm)) { bindm=Matrix::Identity(); }
	    if (Matrix::has_nan(m)) {  m=Matrix::Identity();  }
	    if (Matrix::has_nan(resize)) { resize=Matrix::Identity(); }
	    
	    if (mode==0)
	      {// m0i*m0
		vec.push_back(add_matrix2(env, ri*m0i*m0*bindm*m*resize));
	      //vec_bindm.push_back(add_matrix2(env, bindm));
	      //vec_resize.push_back(add_matrix2(env,resize));
	      //vec_resizei.push_back(add_matrix2(env,resizei));
	      }
	    /*
	    else if (mode==1)
	      vec.push_back(add_matrix2(env,   ri*bindm*m*m0i*inv_bindm*resize  ));
	    else if (mode==2)
	      vec.push_back(add_matrix2(env, ri*bindm*m0i*m*inv_bindm*resize));
	    else if (mode==3)
	      vec.push_back(add_matrix2(env, ri*bindm*m0*inv_m*inv_bindm*resize));
	    else if (mode==4)
	      vec.push_back(add_matrix2(env, ri*bindm*m*m0i*inv_jb*resize));
	    else if (mode==5)
	      vec.push_back(add_matrix2(env, ri*m0i*m0*bindm*m*inv_jb*resize));
	    */
	  }
	//vec.push_back(vec.operator[](vec.size()-1));
	ev.shader_api.set_var(sh, "jointMatrix", vec, std::min(vec.size(),size_t(200)));
	//ev.shader_api.set_var(sh, "bindMatrix", vec_bindm, std::min(vec_bindm.size(),size_t(200)));
	//ev.shader_api.set_var(sh, "resize", vec_resize, std::min(vec_resize.size(),size_t(200)));
	//ev.shader_api.set_var(sh, "resizei", vec_resizei, std::min(vec_resizei.size(),size_t(200)));
	{
	  static bool ftime=false;
	  if (!ftime)
	  if (vec.size()>size_t(200)) { std::cout << "number of joint matrices exceeds shader maximum array size of 200 in jointMatrix variable" << std::endl; ftime=true; }
	}
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
      for(int i=0;i<200;i++)
	mat.push_back(add_matrix2(env,Matrix::Identity()));

      std::vector<int> sh_ids = next->shader_id();
      int s=sh_ids.size();
      for(int i=0;i<s;i++) {
	int sh_id = sh_ids[i];
	sh.id = sh_id;
	ev.shader_api.use(sh);
	
	ev.shader_api.set_var(sh, "jointMatrix", mat, 200);
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
  Matrix resizei;
  static int count;
  static int curr;
  bool keypressed;
  float max_end_time=0.0;
  int mode;
  int id;
};
int GltfAnimShaderML::count=2;
int GltfAnimShaderML::curr=0;
EXPORT GameApi::ML GameApi::PolygonApi::gltf_anim_shader(GameApi::EveryApi &ev, ML ml_orig, std::vector<GameApi::ML> mls, int key,int mode, int timeid)
{
  int s = mls.size();
  std::vector<MainLoopItem*> items;
  for(int i=0;i<s;i++) {
    MainLoopItem *item = find_main_loop(e,mls[i]);
    items.push_back(item);
  }
  MainLoopItem *orig = find_main_loop(e,ml_orig);
  return add_main_loop(e, new GltfAnimShaderML(e,ev,orig,items, key,mode,timeid));
}

void ASyncGltfCB(void *data);

extern std::vector<std::string> g_registered_urls;

class ASyncGltf : public MainLoopItem
{
public:
  ASyncGltf(GameApi::Env &env, MainLoopItem *next, GLTFModelInterface *interface, std::string homepage) : env(env) ,next(next), interface(interface), homepage(homepage) {
    done = false;
    std::string url = interface->Url();
    if (url.substr(url.size()-4,4)!=".glb") {
      env.async_load_callback(interface->Url(), &ASyncGltfCB, this);
      }
  }
  //~ASyncGltf()
  //{
  //  if (interface)
  //    env.async_rem_callback(interface->Url());
  //}
  void Prepare2()
  {
    if (!interface) return;
    std::string url = interface->Url();

    if (interface)
      env.async_rem_callback(interface->Url());

    
    // this algo not needed in glb files since they have no urls to outside.
    if (url.substr(url.size()-4,4)==".glb") return;

    
    //std::cout << "AsyncGltf::PREPARE2" << std::endl;
    if (!done) {
      done = true;
    
#ifndef EMSCRIPTEN
      env.async_load_url(url, homepage);
#endif
      GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
      if (!vec) { std::cout << "ASyncGltf::async not ready!" << std::endl; done=false; return; }
    std::string ss(vec->begin(),vec->end());
    delete vec;
    std::stringstream s(ss);
    std::string line;
    std::string uri;
    while(std::getline(s,line)) {
      std::stringstream ss2(line);
      ss2>> uri;
      //std::cout << "\'" << uri << "\'" << std::endl;
      if (uri=="\"uri\":") {
	std::string url2;
	ss2 >> url2;
	std::string base_url = interface->BaseUrl();
	if (base_url[base_url.size()-1]=='/')
	  url2 = base_url + url2.substr(1,url2.size()-2);
	else if (base_url.size()>0)
	  url2 = base_url + "/" + url2.substr(1,url2.size()-2);
	else url2 = url2.substr(1,url2.size()-2);
	//std::cout << "URL:" << url2 << std::endl;
	if (url2.size()>0 && url2[url2.size()-1]=='\"') url2=url2.substr(0,url2.size()-1);

	g_registered_urls.push_back(url2);
#ifdef EMSCRIPTEN
	env.async_load_url(url2,homepage);
#endif
      }
    }
    }
#ifndef EMSCRIPTEN
    env.async_load_all_urls(g_registered_urls, gameapi_homepageurl);
#endif
    
  }

  virtual void Collect(CollectVisitor &vis) {  next->Collect(vis); vis.register_obj(this); }
  virtual void HeavyPrepare() { Prepare2(); }
  virtual void Prepare() { next->Prepare(); HeavyPrepare(); }
  virtual void execute(MainLoopEnv &e) { next->execute(e); }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() {
    return next->shader_id();
  }

  
private:
  GameApi::Env &env;
  MainLoopItem *next=0;
  //std::string url;
  //std::string base_url;
  GLTFModelInterface *interface=0;
  std::string homepage;
  bool done;
};

GameApi::ML GameApi::MainLoopApi::async_gltf(ML ml, TF tf)
{
  MainLoopItem *next = find_main_loop(e,ml);
  GLTFModelInterface *interface = find_gltf(e,tf);
  int c = get_current_block();
  set_current_block(-1);
  GameApi::ML ml2 = add_main_loop(e, new ASyncGltf(e,next, interface, gameapi_homepageurl));
  set_current_block(c);
  return ml2;
}

void ASyncGltfCB(void *data)
{
  ASyncGltf *ptr = (ASyncGltf*)data;
  ptr->Prepare2();
}


class GLTFEmpty : public GLTFModelInterface
{
public:
  std::string name() const { return "GLTFEmpty"; }
  virtual void Prepare() { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }

  virtual std::string BaseUrl() const { return ""; }
  virtual std::string Url() const { return ""; }

  
  virtual int accessors_size() const { return 0; }
  virtual const tinygltf::Accessor &get_accessor(int i) const { return a; }

  virtual int animations_size() const { return 0; }
  virtual const tinygltf::Animation &get_animation(int i) const { return a2; }

  virtual int buffers_size() const { return 0; }
  virtual const tinygltf::Buffer &get_buffer(int i) const {  return b; }

  virtual int bufferviews_size() const { return 0; }
  virtual const tinygltf::BufferView &get_bufferview(int i) const {  return v; }

  virtual int materials_size() const { return 0; }
  virtual const tinygltf::Material &get_material(int i) const {  return m; }

  virtual int meshes_size() const { return 0; }
  virtual const tinygltf::Mesh &get_mesh(int i) const { return m2; }

  virtual int nodes_size() const { return 0; }
  virtual const tinygltf::Node &get_node(int i) const {  return n; }

  virtual int textures_size() const { return 0; }
  virtual const tinygltf::Texture &get_texture(int i) const { return tx; }

  virtual int images_size() const { return 0; }
  virtual const tinygltf::Image &get_image(int i) const {  return i2; }

  virtual int skins_size() const { return 0; }
  virtual const tinygltf::Skin &get_skin(int i) const {  return s; }

  virtual int samples_size() const { return 0; }
  virtual const tinygltf::Sampler &get_sampler(int i) const {  return s2; }

  virtual int cameras_size() const { return 0; }
  virtual const tinygltf::Camera &get_camera(int i) const {  return c; }

  virtual int scenes_size() const { return 0; }
  virtual const tinygltf::Scene &get_scene(int i) const {  return s3; }

  virtual int lights_size() const { return 0; }
  virtual const tinygltf::Light &get_light(int i) const {  return l; }

private:
  tinygltf::Accessor a;
  tinygltf::Animation a2;
  tinygltf::Buffer b;
  tinygltf::BufferView v;
  tinygltf::Material m;
  tinygltf::Mesh m2;
  tinygltf::Node n;
  tinygltf::Texture tx;
  tinygltf::Image i2;
  tinygltf::Skin s;
  tinygltf::Sampler s2;
  tinygltf::Camera c;
  tinygltf::Scene s3;
  tinygltf::Light l;
};


void write_gltf(GLTFModelInterface *input, tinygltf::Model *model)
{
  model->accessors = std::vector<tinygltf::Accessor>();
  for(int i=0;i<input->accessors_size();i++)
    model->accessors.push_back(input->get_accessor(i));

  model->animations = std::vector<tinygltf::Animation>();
  for(int i=0;i<input->animations_size();i++)
    model->animations.push_back(input->get_animation(i));

  model->buffers = std::vector<tinygltf::Buffer>();
  for(int i=0;i<input->buffers_size();i++)
    model->buffers.push_back(input->get_buffer(i));

  model->bufferViews = std::vector<tinygltf::BufferView>();
  for(int i=0;i<input->bufferviews_size();i++)
    model->bufferViews.push_back(input->get_bufferview(i));

  model->materials = std::vector<tinygltf::Material>();
  for(int i=0;i<input->materials_size();i++)
    model->materials.push_back(input->get_material(i));

  model->meshes = std::vector<tinygltf::Mesh>();
  for(int i=0;i<input->meshes_size();i++)
    model->meshes.push_back(input->get_mesh(i));

  model->nodes = std::vector<tinygltf::Node>();
  for(int i=0;i<input->nodes_size();i++)
    model->nodes.push_back(input->get_node(i));

  model->textures = std::vector<tinygltf::Texture>();
  for(int i=0;i<input->textures_size();i++)
    model->textures.push_back(input->get_texture(i));

  model->images = std::vector<tinygltf::Image>();
  for(int i=0;i<input->images_size();i++)
    model->images.push_back(input->get_image(i));

  model->skins = std::vector<tinygltf::Skin>();
  for(int i=0;i<input->skins_size();i++)
    model->skins.push_back(input->get_skin(i));

  model->samplers = std::vector<tinygltf::Sampler>();
  for(int i=0;i<input->samplers_size();i++)
    model->samplers.push_back(input->get_sampler(i));

  model->cameras = std::vector<tinygltf::Camera>();
  for(int i=0;i<input->cameras_size();i++)
    model->cameras.push_back(input->get_camera(i));

  model->scenes = std::vector<tinygltf::Scene>();
  for(int i=0;i<input->scenes_size();i++)
    model->scenes.push_back(input->get_scene(i));

  model->lights = std::vector<tinygltf::Light>();
  for(int i=0;i<input->lights_size();i++)
    model->lights.push_back(input->get_light(i));
}
  
void save_gltf(GLTFModelInterface *input, std::string filename)
{
  bool is_binary=false;
  if (int(filename.size())>3) {
    std::string sub = filename.substr(filename.size()-3);
    if (sub=="glb") is_binary=true;
  }
  
  tinygltf::TinyGLTF tiny;
  tinygltf::Model model;
  write_gltf(input, &model);
  tiny.WriteGltfSceneToFile(&model, filename, true, true, false, is_binary);
}

class SaveGLTF : public MainLoopItem
{
public:
  SaveGLTF(GLTFModelInterface *input, std::string filename) : input(input), filename(filename) { }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() { Prepare(); }
  virtual void Prepare() {
    input->Prepare();
    save_gltf(input,filename);
  }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GLTFModelInterface *input;
  std::string filename;
};
  
GameApi::ML GameApi::MainLoopApi::save_gltf(TF tf, std::string filename)
{
  GLTFModelInterface *i = find_gltf(e,tf);
  return add_main_loop(e, new SaveGLTF(i, filename));
}


extern bool g_deploy_phase;

GameApi::TF GameApi::MainLoopApi::gltf_loadKK2(std::string url)
{
  return gltf_loadKK("",url);
}

GameApi::TF GameApi::MainLoopApi::gltf_loadKK(std::string base_url, std::string url)
{
  if (g_deploy_phase) base_url="./";
  else
    {
      int s = url.size();
      int pos = -1;
      for(int i=0;i<s;i++)
	{
	  if (url[i]=='/') { pos = i; }
	}
      if (pos!=-1) {
	base_url = url.substr(0,pos);
      }
    }
  
  bool is_binary=false;
  if (int(url.size())>3) {
    std::string sub = url.substr(url.size()-3);
    if (sub=="glb") is_binary=true;
  }
  
  LoadGltf *load = find_gltf_instance(e,base_url,url,gameapi_homepageurl,is_binary);
  GLTF_Model_with_prepare *model = new GLTF_Model_with_prepare(load, &load->model);
  g_model_del_items.push_back(model);
  GLTFModelInterface *i = (GLTFModelInterface*)model;
      int c = get_current_block();
      set_current_block(-1);
  GameApi::TF tf = add_gltf(e,i);
  set_current_block(c);
  return tf;
}


GameApi::TF LoadGLBFromString(GameApi::Env &e, std::string base_url, std::string url, std::string data)
{
  //std::ofstream ss("foo.err");
  //ss << data;
  //ss.close();


  std::cout << "BASE URL:" << base_url << std::endl;
  std::cout << "URL: " << url << std::endl;
  
  if (g_deploy_phase) base_url="./";
  else
    {
      int s = url.size();
      int pos = -1;
      for(int i=0;i<s;i++)
	{
	  if (url[i]=='/') { pos = i; }
	}
      if (pos!=-1) {
	base_url = url.substr(0,pos);
      }
    }
  
  bool is_binary=true;
  //if (int(url.size())>3) {
  //  std::string sub = url.substr(url.size()-3);
  //  if (sub=="glb") is_binary=true;
  //}
  
  LoadGltf_from_string *load = find_gltf_instance_from_string(e,data,base_url,url,gameapi_homepageurl,is_binary);
  GLTF_Model_with_prepare_from_string *model = new GLTF_Model_with_prepare_from_string(load, &load->model);
  g_model_del_items_from_string.push_back(model);
  GLTFModelInterface *i = (GLTFModelInterface*)model;
      int c = get_current_block();
      set_current_block(-1);
  GameApi::TF tf = add_gltf(e,i);
  set_current_block(c);
  return tf;
}

GameApi::ASyncVec *g_convert(std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec);

GLTFModelInterface *find_next(GLTFModelInterface *i);


void GameApi::MainLoopApi::save_glb_store(GameApi::EveryApi &ev, std::string output_filename, TF tf)
{
  //GameApi::DS ds = ev.polygon_api.tf_ds_inv(tf,0);
  //std::string write_buf = ev.mainloop_api.ds_to_string(ds);
  GLTFModelInterface *interface = find_gltf(e,tf);

  interface = find_next(interface);

  std::cout << "save_glb_store:" << interface->name() << std::endl;
  
#ifndef EMSCRIPTEN
  GLTF_Model_with_prepare *prepare = dynamic_cast<GLTF_Model_with_prepare*>(interface);
  if (!prepare) {
    GLTF_Model_with_prepare_from_string *prepare = dynamic_cast<GLTF_Model_with_prepare_from_string*>(interface);
    if (!prepare) { std::cout << "Using tf_ds_tf twice is not possible at " << prepare->name() << std::endl; }
    
    //prepare->Prepare();
  GLTF_Model *model = (GLTF_Model*)interface;
  LoadGltf_from_string *load = prepare->load;
  tinygltf::Model *self = model->self;
  std::stringstream ss;
  bool b = load->tiny.WriteGltfSceneToStream(self, ss, true, true);
  std::string write_buf = ss.str();
  std::vector<unsigned char,GameApiAllocator<unsigned char> > vec(write_buf.begin(),write_buf.end());
  e.store_file(output_filename,g_convert(&vec));
    
  return;

  }
#else
  GLTF_Model_with_prepare *prepare = (GLTF_Model_with_prepare*)interface;
#endif
  GLTF_Model *model = (GLTF_Model*)interface;
  //prepare->Prepare();
  LoadGltf *load = prepare->load;
  tinygltf::Model *self = model->self;
  std::stringstream ss;
  bool b = load->tiny.WriteGltfSceneToStream(self, ss, true, true);
  std::string write_buf = ss.str();
  std::vector<unsigned char,GameApiAllocator<unsigned char> > vec(write_buf.begin(),write_buf.end());
  e.store_file(output_filename,g_convert(&vec));
}


#include "zip_file.hpp"
#include "GameApi_dep_env_delmap.hh"


class GLTF_Model_with_prepare_sketchfab_zip;

struct ZipThreadData
{
#ifdef THREADS
  pthread_t thread_id;
#endif
  GLTF_Model_with_prepare_sketchfab_zip *obj;
  int i;
  mz_zip_archive *pZip;
};

void *thread_sketchfab_zip(void *data);

class GLTF_Model_with_prepare_sketchfab_zip : public GLTF_Model
{
public:
  std::string name() const { return "GLTF_Model_with_prepare_sketchfab_zip"; }
  GLTF_Model_with_prepare_sketchfab_zip(GameApi::Env &e, std::string zip_url, std::string homepage, LoadGltf *load, tinygltf::Model *model) : e(e), GLTF_Model(model, zip_url + "/", zip_url + "/scene.gltf"), zip_url(zip_url), homepage(homepage), load(load), model(model) { firsttime=true; }
  void Prepare() {
    if (firsttime) {
    UncompressZip();
    load->Prepare();
    model=&load->model;
    self=model;
    firsttime=false;
    }
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() {
    if (firsttime) {
    UncompressZip();
    load->Prepare();
    model=&load->model;
    self=model;
    firsttime=false;
    }
  }
  void UncompressZip()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(zip_url, homepage);
#endif
    GameApi::ASyncVec *vec = e.get_loaded_async_url(zip_url);
    if (!vec) { std::cout << "gltf_load_sketchfab_zip ASync not ready!" << std::endl; return; }
    std::vector<unsigned char> vec2(vec->begin(), vec->end());
    mz_ulong size = vec2.end()-vec2.begin();

    g_zip_file_size = g_zip_file_size > size ? g_zip_file_size : size;
    
    //std::cout << "Zip size: " << size << std::endl;
#if 1
    if (size>250000000) {
      std::cout << "Zip File too large! 250Mb is max. -> exiting..." << std::endl;
      return;
	}
#endif
    
    //mz_ulong size2 = 0;
    //unsigned char *ptr = new unsigned char[size2];
    //std::cout << "ZIP URL=" << zip_url << std::endl;
    //std::cout << "VECTOR SIZE=" << size << std::endl;

    
    mz_zip_archive pZip;
    std::memset(&pZip,0,sizeof(mz_zip_archive));

    mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

    //std::cout << "ZIP STATUS:" << b2 << std::endl;
    
    mz_uint num = mz_zip_reader_get_num_files(&pZip);
    //std::cout << "ZIp num=" << num << std::endl;
    std::vector<ZipThreadData*> thread_data;
    for(int i=0;i<num;i++)
      {

	ZipThreadData *info = new ZipThreadData;
	info->obj = this;
	info->i = i;
	info->pZip = &pZip;
#ifdef THREADS
	//pthread_attr_t attr;
	//pthread_attr_init(&attr);
	//pthread_attr_setstacksize(&attr,300000);
	g_pthread_count++;
	//pthread_create(&info->thread_id, &attr, &thread_sketchfab_zip, (void*)info);
	tasks_add(3009,&thread_sketchfab_zip,(void*)info);
	
	thread_data.push_back(info);
#else
	thread_sketchfab_zip((void*)info);
#endif
      }
#ifdef THREADS
    tasks_join(3009);
    /*
    for(int i2=0;i2<num;i2++)
      {
	ZipThreadData *info = thread_data[i2];
	void *res;
	pthread_join(thread_data[i2]->thread_id, &res);
      }
    */
#endif
    mz_zip_reader_end(&pZip);
    if (mainfilename!="")
      {
	std::string mainfile1 = "";
	int s = mainfilename.size();
	int pos=-1;
	for(int i=0;i<s;i++) {
	  if (mainfilename[i]=='/') pos=i;
	}
	if (pos != -1) {       
	  base_url = mainfilename.substr(0,pos+1);
	} else {
	  base_url = zip_url + "/";
	}
	url = mainfilename;
	load->set_urls(base_url,url);
      }
    
  }
public:
  GameApi::Env &e;
  std::string zip_url;
  std::string homepage;
  LoadGltf *load;
  tinygltf::Model *model;	       
  bool firsttime;
  std::string mainfilename;
};

void *thread_sketchfab_zip(void *data)
{
  //std::cout << "Thread sketchfab zip" << std::endl;
  ZipThreadData *dt = (ZipThreadData*)data;
  GLTF_Model_with_prepare_sketchfab_zip *obj = dt->obj;
  mz_zip_archive *pZip = dt->pZip;
  int i = dt->i;
  
	mz_bool is_dir = mz_zip_reader_is_file_a_directory(pZip, i);
	if (is_dir) {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(pZip, i, filename, 256);
	    //std::cout << "DIR:" << filename << std::endl;
	    delete[] filename;
	} else
	  {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(pZip, i, filename, 256);
	    if (strlen(filename)==0) { std::cout << "Skipping empty filename from .zip" << std::endl; delete [] filename; return 0; }
	    std::string url = "load_url.php?url=" + obj->zip_url + "/" + std::string(filename);
	    //std::cout << url.substr(url.size()-5) << "::" << url.substr(url.size()-4) << std::endl;
	    if (url.substr(url.size()-5)==".gltf" ||url.substr(url.size()-4)==".glb") obj->mainfilename = obj->zip_url + "/" + std::string(filename);
	    //std::cout << "Decompressing zip: " << filename << std::endl;

	    //if (g_del_map.load_url_buffers_async.find(url)!=g_del_map.load_url_buffers_async.end()) {
	    // delete [] filename;
	    //  continue;
	    //}
	    
	    size_t sz=0;
	    void *ptr = mz_zip_reader_extract_to_heap(pZip, i, &sz, 0);
	    if (sz<1) sz=1;
	    std::vector<unsigned char,GameApiAllocator<unsigned char> > *data = new std::vector<unsigned char,GameApiAllocator<unsigned char> >((char*)ptr,((char*)ptr)+sz);
	    free(ptr);
	    delete[] filename;
#ifdef EMSCRIPTEN
#if 0
	    data->push_back(0); // is this always needed?
#endif
#endif
	    // std::cout << url << "::" << data->size() << std::endl;
	    if (g_del_map.async_find(url)) {
	      g_del_map.del_async_url(url);
	      //delete g_del_map.load_url_buffers_async[url];
	    }
	    g_del_map.push_async_url(url,data);
	    //g_del_map.load_url_buffers_async[url] = data;
	  }

	return 0;
}


int find_indexhtml_string(std::string data, std::string str);

extern std::vector<const unsigned char*> g_content;
extern std::vector<const unsigned char*> g_content_end;
extern std::vector<const char*> g_urls;


class GameApi_Zip_loadassets : public MainLoopItem
{
public:
  GameApi_Zip_loadassets(GameApi::Env &e, std::string zip_url, std::string homepage) : e(e), zip_url(zip_url), homepage(homepage){ firsttime=true; }
  void Prepare() {
    if (firsttime) {
    UncompressZip();
    firsttime=false;
    }
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() {
    if (firsttime) {
    UncompressZip();
    firsttime=false;
    }
  }
  void execute(MainLoopEnv &e) { }
  void handle_event(MainLoopEvent &e) { }
  
  void UncompressZip()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(zip_url, homepage);
#endif
    GameApi::ASyncVec *vec = e.get_loaded_async_url(zip_url);
    if (!vec) { std::cout << "gltf_load_sketchfab_zip ASync not ready!" << std::endl; return; }
    std::vector<unsigned char> vec2(vec->begin(), vec->end());
    mz_ulong size = vec2.end()-vec2.begin();
    //mz_ulong size2 = 0;
    //unsigned char *ptr = new unsigned char[size2];
    //std::cout << "ZIP URL=" << zip_url << std::endl;
    //std::cout << "VECTOR SIZE=" << size << std::endl;

    
    mz_zip_archive pZip;
    std::memset(&pZip,0,sizeof(mz_zip_archive));

    mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

    //std::cout << "ZIP STATUS:" << b2 << std::endl;
    
    mz_uint num = mz_zip_reader_get_num_files(&pZip);
    //std::cout << "ZIp num=" << num << std::endl;
    for(int i=0;i<num;i++)
      {
	mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip, i);
	if (is_dir) {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    //std::cout << "DIR:" << filename << std::endl;
	    delete[] filename;
	} else
	  {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    if (strlen(filename)==0) { std::cout << "Skipping empty filename from .zip" << std::endl; delete [] filename; continue; }
	    std::string url = /*"load_url.php?url=" + zip_url + "/" +*/ std::string(filename);
	    //std::cout << url.substr(url.size()-5) << "::" << url.substr(url.size()-4) << std::endl;
	    if (url.substr(url.size()-5)==".gltf" ||url.substr(url.size()-4)==".glb") mainfilename = zip_url + "/" + std::string(filename);
	    //std::cout << "Decompressing zip: " << filename << std::endl;

	    //if (g_del_map.load_url_buffers_async.find(url)!=g_del_map.load_url_buffers_async.end()) {
	    // delete [] filename;
	    //  continue;
	    //}
	    
	    size_t sz=0;
	    void *ptr = mz_zip_reader_extract_to_heap(&pZip, i, &sz, 0);
	    std::vector<unsigned char,GameApiAllocator<unsigned char> > *data = new std::vector<unsigned char,GameApiAllocator<unsigned char> >((char*)ptr,((char*)ptr)+sz);
	    free(ptr);
	    delete[] filename;

	    // std::cout << url << "::" << data->size() << std::endl;
	    if (g_del_map.async_find(url)) {
	      g_del_map.del_async_url(url);
	      //delete g_del_map.load_url_buffers_async[url];
	    }
	    if (find_indexhtml_string(url,"engine/")==-1) {
	      //std::cout << "URL:" << url << std::endl;
	      //g_del_map.load_url_buffers_async[url] = data;
	      g_del_map.push_async_url(url,data);
	      g_content.push_back(&data->operator[](0));
	      g_content_end.push_back(&(*data->end()));
	      char *buf = new char[url.size()+1];
	      std::copy(url.begin(),url.end(),buf);
	      buf[url.size()]=0;
	      g_urls.push_back(buf);
	      
	    }
	  }
      }
    mz_zip_reader_end(&pZip);
    if (mainfilename!="")
      {
	/*
	std::string mainfile1 = "";
	int s = mainfilename.size();
	int pos=-1;
	for(int i=0;i<s;i++) {
	  if (mainfilename[i]=='/') pos=i;
	}
	*/
	/*
	if (pos != -1) {       
	  base_url = mainfilename.substr(0,pos+1);
	} else {
	  base_url = zip_url + "/";
	}
	url = mainfilename;
	*/
	//load->set_urls(base_url,url);
      }
    
  }
private:
  GameApi::Env &e;
  std::string zip_url;
  std::string homepage;
  //LoadGltf *load;
  //tinygltf::Model *model;	       
  bool firsttime;
  std::string mainfilename;
};

GameApi::ML GameApi::MainLoopApi::load_zip_assets(std::string zip_url)
{
  return add_main_loop(e,new GameApi_Zip_loadassets(e,zip_url, gameapi_homepageurl));
}

void empty_asset(void*) { }

class GameApi_Zip_loadassets2 : public Html
{
public:
  GameApi_Zip_loadassets2(GameApi::Env &e, std::string zip_url, std::string homepage) : e(e), zip_url(zip_url), m_homepage(homepage) { firsttime=true; }
  void Prepare() {
    if (firsttime) {
    UncompressZip();
    firsttime=false;
    }
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() {
    if (firsttime) {
    UncompressZip();
    firsttime=false;
    }
  }
  virtual std::string homepage() const { return m_homepage; }
  virtual std::string script_file() const { return ""; }
  virtual void SetCB(void(*fptr)(void*), void*data) { }
  
  void UncompressZip()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(zip_url, m_homepage);
#endif
    GameApi::ASyncVec *vec = e.get_loaded_async_url(zip_url);
    if (!vec) { std::cout << "gltf_load_sketchfab_zip ASync not ready!" << std::endl; return; }
    std::vector<unsigned char> vec2(vec->begin(), vec->end());
    mz_ulong size = vec2.end()-vec2.begin();
    //mz_ulong size2 = 0;
    //unsigned char *ptr = new unsigned char[size2];
    //std::cout << "ZIP URL=" << zip_url << std::endl;
    //std::cout << "VECTOR SIZE=" << size << std::endl;

    
    mz_zip_archive pZip;
    std::memset(&pZip,0,sizeof(mz_zip_archive));

    mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

    //std::cout << "ZIP STATUS:" << b2 << std::endl;
    
    mz_uint num = mz_zip_reader_get_num_files(&pZip);
    //std::cout << "ZIp num=" << num << std::endl;
    for(int i=0;i<num;i++)
      {
	mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip, i);
	if (is_dir) {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    //std::cout << "DIR:" << filename << std::endl;
	    delete[] filename;
	} else
	  {
	    char *filename = new char[256];
	    for(int j=0;j<256;j++)
	      filename[j]=0;
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    if (strlen(filename)==0) { std::cout << "Skipping empty filename from .zip" << std::endl; delete [] filename; continue; }
	    std::string url = /*"load_url.php?url=" + zip_url + "/" +*/ std::string(filename);
	    //std::cout << url.substr(url.size()-5) << "::" << url.substr(url.size()-4) << std::endl;
	    if (url.substr(url.size()-5)==".gltf" ||url.substr(url.size()-4)==".glb") mainfilename = zip_url + "/" + std::string(filename);
	    //std::cout << "Decompressing zip: " << filename << std::endl;

	    //if (g_del_map.load_url_buffers_async.find(url)!=g_del_map.load_url_buffers_async.end()) {
	    // delete [] filename;
	    //  continue;
	    //}
	    
	    size_t sz=0;
	    void *ptr = mz_zip_reader_extract_to_heap(&pZip, i, &sz, 0);
	    std::vector<unsigned char,GameApiAllocator<unsigned char> > *data = new std::vector<unsigned char, GameApiAllocator<unsigned char> >((char*)ptr,((char*)ptr)+sz);
	    free(ptr);
	    delete[] filename;

	    // std::cout << url << "::" << data->size() << std::endl;
	    if (g_del_map.async_find(url)) {
	      g_del_map.del_async_url(url);
	      //delete g_del_map.load_url_buffers_async[url];
	    }
	    if (find_indexhtml_string(url,"engine/")==-1) {
	      //std::cout << "URL:" << url << std::endl;
	      g_del_map.push_async_url(url,data);
		//g_del_map.load_url_buffers_async[url] = data;
	      g_content.push_back(&data->operator[](0));
	      g_content_end.push_back(&(*data->end()));
	      char *buf = new char[url.size()+1];
	      std::copy(url.begin(),url.end(),buf);
	      buf[url.size()]=0;
	      g_urls.push_back(buf);
	      
	    }
	  }
      }
    mz_zip_reader_end(&pZip);
    if (mainfilename!="")
      {
	/*
	std::string mainfile1 = "";
	int s = mainfilename.size();
	int pos=-1;
	for(int i=0;i<s;i++) {
	  if (mainfilename[i]=='/') pos=i;
	}
	*/
	/*
	if (pos != -1) {       
	  base_url = mainfilename.substr(0,pos+1);
	} else {
	  base_url = zip_url + "/";
	}
	url = mainfilename;
	*/
	//load->set_urls(base_url,url);
      }
    
  }
private:
  GameApi::Env &e;
  std::string zip_url;
  std::string m_homepage;
  //LoadGltf *load;
  //tinygltf::Model *model;	       
  bool firsttime;
  std::string mainfilename;
};


GameApi::HML GameApi::MainLoopApi::load_zip_assets2(std::string zip_url)
{
  return add_html(e,new GameApi_Zip_loadassets2(e,zip_url, gameapi_homepageurl));
}

/*
class GLB_Model_with_prepare_sketchfab_zip : public GLTF_Model
{
public:
  GLB_Model_with_prepare_sketchfab_zip(GameApi::Env &e, std::string zip_url, std::string homepage, LoadGltf *load, tinygltf::Model *model) : e(e), GLTF_Model(model, zip_url + "/", zip_url + "/scene.glb"), zip_url(zip_url), homepage(homepage), load(load), model(model) { }
  void Prepare() {
    UncompressZip();
    load->Prepare();
  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() {
    UncompressZip();
    load->Prepare();
  }
  void UncompressZip()
  {
#ifndef EMSCRIPTEN
    e.async_load_url(zip_url, homepage);
#endif
    GameApi::ASyncVec *vec = e.get_loaded_async_url(zip_url);
    if (!vec) { std::cout << "gltf_load_sketchfab_zip ASync not ready!" << std::endl; return; }
    std::vector<unsigned char> vec2(vec->begin(), vec->end());
    mz_ulong size = vec->end()-vec->begin();
    mz_ulong size2 = 0;
    unsigned char *ptr = new unsigned char[size2];

    mz_zip_archive pZip;
    std::memset(&pZip,0,sizeof(mz_zip_archive));

    mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

    mz_uint num = mz_zip_reader_get_num_files(&pZip);
    for(int i=0;i<num;i++)
      {
	mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip, i);
	if (is_dir) {
	    char *filename = new char[256];
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    delete[] filename;
	} else
	  {
	    char *filename = new char[256];
	    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
	    std::string url = "load_url.php?url=" + zip_url + "/" + std::string(filename);

	    size_t sz;
	    void *ptr = mz_zip_reader_extract_to_heap(&pZip, i, &sz, 0);
	    std::vector<unsigned char> *data = new std::vector<unsigned char>((char*)ptr,((char*)ptr)+sz);
	    free(ptr);
	    delete[] filename;
	    
	    g_del_map.load_url_buffers_async[url] = data;
	  }
	
      }
    
  }
private:
  GameApi::Env &e;
  std::string zip_url;
  std::string homepage;
  LoadGltf *load;
  tinygltf::Model *model;	       
};
*/

std::string fetch_more_data(std::string url);

std::string get_zip_license_file(std::string zipfilename)
{
  std::string zip = fetch_more_data(zipfilename);
  std::vector<unsigned char> vec2(zip.begin(),zip.end());
  mz_ulong size = zip.end()-zip.begin();
  mz_zip_archive pZip;
  std::memset(&pZip,0,sizeof(mz_zip_archive));

  mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

  mz_uint num = mz_zip_reader_get_num_files(&pZip);
  for(int i=0;i<num;i++) {
    mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip,i);
    if (is_dir) continue;
    char *filename = new char[256];
    *filename = 0;
    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
    if (strlen(filename)==0) continue;
    std::string filename2(filename);

    if (filename2=="license.txt"||filename2=="LICENSE.TXT") {
      size_t sz=0;
      void *ptr = mz_zip_reader_extract_to_heap(&pZip, i, &sz, 0);
      std::string res = std::string((char*)ptr,((char*)ptr)+sz);
      free(ptr);
      delete [] filename;
      return res;
    } 
    delete[] filename;
  }
  return "";
}

std::string get_zip_indexhtml_file(std::string zipfilename)
{
  std::string zip = fetch_more_data(zipfilename);
  std::vector<unsigned char> vec2(zip.begin(),zip.end());
  mz_ulong size = zip.end()-zip.begin();
  mz_zip_archive pZip;
  std::memset(&pZip,0,sizeof(mz_zip_archive));

  mz_bool b2 = mz_zip_reader_init_mem(&pZip, &vec2[0], size, 0);

  mz_uint num = mz_zip_reader_get_num_files(&pZip);
  for(int i=0;i<num;i++) {
    mz_bool is_dir = mz_zip_reader_is_file_a_directory(&pZip,i);
    if (is_dir) continue;
    char *filename = new char[256];
    *filename = 0;
    mz_uint err = mz_zip_reader_get_filename(&pZip, i, filename, 256);
    if (strlen(filename)==0) continue;
    std::string filename2(filename);

    if (filename2=="index.html") {
      size_t sz=0;
      void *ptr = mz_zip_reader_extract_to_heap(&pZip, i, &sz, 0);
      std::string res = std::string((char*)ptr,((char*)ptr)+sz);
      free(ptr);
      delete [] filename;
      return res;
    } 
    delete[] filename;
  }
  return "";
}

int find_indexhtml_string(std::string data, std::string str)
{
  int s = data.size();
  s-=str.size();
  int s2 = str.size();
  for(int i=0;i<s;i++) {
    bool found = true;
    for(int j=0;j<s2;j++)
      {
	if (data[i+j] != str[j]) { found = false; break; }
      }
    if (found) {
      return i+str.size();
    }
  }
  return -1;
}

std::string parse_zip_indexhtml(std::string data, bool b=false)
{
  int pos1 = find_indexhtml_string(data, "<pre id=\"gameapi_script\" style=\"display:none\">");
  if (pos1==-1) return "";
  std::string data2 = data.substr(pos1);
  std::string end="</pre>";
  if (b) end="RUN";
  int pos2 = find_indexhtml_string(data2, end);
  if (pos2==-1) return "";
  pos2-=end.size();
  return data.substr(pos1+1,pos2-1);
}

std::string replace_str(std::string val, std::string repl, std::string subst);

bool load_zip(GameApi::Env &e, GameApi::EveryApi &ev, std::string url_to_zip)
{
  std::string index = get_zip_indexhtml_file(url_to_zip);
  if (index=="") { return false; }

  std::string parse = parse_zip_indexhtml(index);
  if (parse=="") { return false; }
  	  parse = replace_str(parse, "&lt;", "<");
	  parse = replace_str(parse, "&gt;", ">");
	  parse = replace_str(parse, "&quot;", "\"");
	  parse = replace_str(parse, "&apos;", "\'");
	  parse = replace_str(parse, "&amp;", "&");
  GameApi::ExecuteEnv ee;
  std::pair<int,std::string> blk = GameApi::execute_codegen(e,ev, parse, ee);
  if (blk.second == "RUN") {
    GameApi::RUN r;
    r.id = blk.first;
    ev.blocker_api.run2(ev,r);
  } else if (blk.second=="OK")
    {
      GameApi::BLK b;
      b.id = blk.first;
      ev.blocker_api.run(b);
    } else
    {
      std::cout << "ERROR: internal error" << std::endl;
      return false;
    }
  return true;
}


class LoadZip2 : public Html
{
public:
  LoadZip2(GameApi::Env &env, GameApi::EveryApi &ev, std::string zip_url) : env(env), ev(ev), url_to_zip(zip_url),m_fptr(&empty_asset),m_data(0) { }
  virtual void SetCB(void(*fptr)(void*), void*data) { m_fptr=fptr; m_data=data; }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() { Prepare(); }
  virtual void Prepare() {
    if (firsttime) {
      firsttime=false;
      m_fptr(m_data);
    }
  }

  std::string homepage() const { return gameapi_homepageurl; }
  std::string script_file() const
  {
  std::string index = get_zip_indexhtml_file(url_to_zip);
  if (index=="") { return ""; }

  std::string parse = parse_zip_indexhtml(index,true);
  if (parse=="") { return ""; }
  	  parse = replace_str(parse, "&lt;", "<");
	  parse = replace_str(parse, "&gt;", ">");
	  parse = replace_str(parse, "&quot;", "\"");
	  parse = replace_str(parse, "&apos;", "\'");
	  parse = replace_str(parse, "&amp;", "&");
	  return parse;
  }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url_to_zip;
  void(*m_fptr)(void*);
  void* m_data;
  bool firsttime=true;
};
class CombineHtml : public Html
{
public:
  CombineHtml(Html *first, Html *second) : first(first), second(second) { }
  virtual void Collect(CollectVisitor &vis) { first->Collect(vis); second->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void SetCB(void(*fptr)(void*), void*data) { first->SetCB(fptr,data); second->SetCB(fptr,data); }
  virtual void Prepare() { first->Prepare(); second->Prepare(); }
  virtual std::string script_file() const { return second->script_file(); }
  virtual std::string homepage() const { return second->homepage(); }
private:
  Html *first;
  Html *second;
};


GameApi::HML GameApi::MainLoopApi::load_zip2(EveryApi &ev, std::string zip_url)
{
  GameApi::HML hml1 = load_zip_assets2(zip_url);
  GameApi::HML hml2 = add_html(e, new LoadZip2(e,ev,zip_url));
  Html *h1 = find_html(e,hml1);
  Html *h2 = find_html(e,hml2);
  GameApi::HML hml_comb = add_html(e, new CombineHtml(h1,h2));
  return hml_comb;
}

class LoadZip : public MainLoopItem
{
public:
  LoadZip(GameApi::Env &env, GameApi::EveryApi &ev, std::string zip_url) : env(env), ev(ev), zip_url(zip_url) { }
  virtual void Collect(CollectVisitor &vis) {vis.register_obj(this); }
  virtual void HeavyPrepare() { Prepare(); }
  virtual void Prepare() {
    bool b = load_zip(env, ev, zip_url);
  }
  virtual void execute(MainLoopEnv &e) { } 
  virtual void handle_event(MainLoopEvent &e) { }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string zip_url;
};
GameApi::ML GameApi::MainLoopApi::load_zip(EveryApi &ev, std::string zip_url)
{
  GameApi::ML ml1 = load_zip_assets(zip_url);
  GameApi::ML ml2 = add_main_loop(e, new LoadZip(e,ev,zip_url));
  return or_elem_ml(ev,ml1,ml2);
}



GameApi::TF GameApi::MainLoopApi::gltf_load_sketchfab_zip(std::string url_to_zip)
{
  bool is_binary=false;
  LoadGltf *load = find_gltf_instance(e,url_to_zip + "/",url_to_zip+"/scene.gltf",gameapi_homepageurl,is_binary);
  GLTF_Model_with_prepare_sketchfab_zip *model = new GLTF_Model_with_prepare_sketchfab_zip(e,url_to_zip, gameapi_homepageurl, load, &load->model);
  GLTFModelInterface *i = (GLTFModelInterface*)model;
  return add_gltf(e,i);
}
/*
GameApi::TF GameApi::MainLoopApi::glb_load_sketchfab_zip(std::string url_to_zip)
{
  bool is_binary=true;
  LoadGltf *load = find_gltf_instance(e,url_to_zip + "/",url_to_zip+"/scene.glb",gameapi_homepageurl,is_binary);
  GLTF_Model_with_prepare_sketchfab_zip *model = new GLTF_Model_with_prepare_sketchfab_zip(e,url_to_zip, gameapi_homepageurl, load, &load->model);
  GLTFModelInterface *i = (GLTFModelInterface*)model;
  return add_gltf(e,i);
}
*/

int gltf_mesh2_calc_max_timeindexes(GLTFModelInterface *interface, int animation, int skin_num)
  {

    int sz = interface->animations_size();
    if (!(animation>=0 && animation<sz)) return 0;
    const tinygltf::Animation &anim = interface->get_animation(animation);
    int s = anim.channels.size();
    int count=0;
    for(int i=0;i<s;i++)
      {
	AnimData dt;
	GLTFAnimation anim(&dt,interface,animation,i,0,skin_num);
	anim.Prepare();
	int val = anim.Count();
	if (val>count) count=val;
      }
    //std::cout << "max timeindexes:" << count << std::endl;
    return count;
}

//
// 2nd attempt at image decoding with multiple threads.
//


GLTFImageDecoder::~GLTFImageDecoder()
{
#ifndef WIN32
#ifndef LINUX
  std::map<FILEID,std::vector<unsigned char,GameApiAllocator<unsigned char> > *>::iterator i = files2.begin();
    for(;i!=files2.end();i++)
      {
	delete (*i).second;
      }
    std::map<FILEID,std::vector<unsigned char,GameApiAllocator<unsigned char> > *>::iterator i2 = decoded_files.begin();
    for(;i2!=decoded_files.end();i2++)
      {
	(*i2).second->clear();
      }
    std::map<FILEID,tinygltf::Image*>::iterator i3 = decoded_image.begin();
    for(;i3!=decoded_image.end();i3++)
      {
	delete (*i3).second;
      }
#endif
#endif
}


std::vector<std::string> GLTFImageDecoder::scan_gltf_file(std::vector<unsigned char,GameApiAllocator<unsigned char> > &vec)
{
  std::string s(vec.begin(),vec.end());
  std::stringstream ss(s);
  std::string line;
  std::string uri;
  std::vector<std::string> vec2;
  while(std::getline(ss,line)) {
    std::stringstream ss2(line);
    ss2 >> uri;
    //std::cout << "@" << uri << "@" << std::endl;
    if (uri=="\"uri\":") {
      	std::string url2;
	ss2 >> url2;
	std::string base_url = url_prefix;
	if (base_url[base_url.size()-1]=='/')
	  url2 = base_url + url2.substr(1,url2.size()-2);
	else if (base_url.size()>0)
	  url2 = base_url + "/" + url2.substr(1,url2.size()-2);
	else url2 = url2.substr(1,url2.size()-2);
	//std::cout << "URL:" << url2 << std::endl;
	if (url2.size()>0 && url2[url2.size()-1]=='\"') url2=url2.substr(0,url2.size()-1);

	//std::cout << "COMPARE:" << url2.substr(url2.size()-3,3) << std::endl;
	if (url2.substr(url2.size()-3,3)!="bin")
	  vec2.push_back(url2);
    }
  }
  //std::cout << "FOUND:" << vec2.size() << std::endl;
  return vec2;
}

FETCHID get_new_fetch_id()
{
  static int count=0;
  count++;
  FETCHID id;
  id.id = count;
  return id;
}
FILEID get_new_file_id()
{
  static int count=0;
  count++;
  FILEID id;
  id.id = count;
  return id;
}

FETCHID GLTFImageDecoder::fetch_file(GameApi::Env &e, std::string filename)
{
  e.async_load_url(filename, gameapi_homepageurl);
  FETCHID id = get_new_fetch_id();
  assert(filenames[id]=="");
  filenames[id] = filename;
  return id;
}
std::string GLTFImageDecoder::get_fetch_filename(FETCHID id)
{
  return filenames[id];
}


std::vector<FETCHID> GLTFImageDecoder::fetch_ids(const std::vector<std::string> &filenames_)
{
  int s = filenames_.size();
  std::vector<FETCHID> res;
  for(int i=0;i<s;i++)
    {
      std::string filename = filenames_[i];
      FETCHID id = get_new_fetch_id();
      assert(filenames[id]=="");
      filenames[id] = filename;
      res.push_back(id);
    }
  return res;
}

void GLTFImageDecoder::fetch_all_files(GameApi::Env &e, const std::vector<FETCHID> &ids)
{
  std::vector<std::string> filenames_;
  int s = ids.size();
  for(int i=0;i<s;i++)
    {
      filenames_.push_back(filenames[ids[i]]);
    }

  //std::cout << "FETCH:" << filenames_.size() << std::endl;
  e.async_load_all_urls(filenames_, gameapi_homepageurl);

      int sd = filenames_.size();
      for(int iu=0;iu<sd;iu++)
	{
	  e.async_load_url(filenames_[iu],gameapi_homepageurl);
	}


}
void GLTFImageDecoder::set_fetch_callback(GameApi::Env &e, FETCHID id, void (*fptr)(void*), void *user_data)
{
  std::string url = filenames[id];
  e.async_load_callback(url, fptr, user_data);
}

std::vector<unsigned char,GameApiAllocator<unsigned char> > *GLTFImageDecoder::get_file(GameApi::Env &e, FETCHID id)
{
  std::string url = filenames[id];
  //std::cout << "get_file:" << url << std::endl;
  GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
  //std::cout << "FILE SIZE:" << vec->size() << std::endl;
  return new std::vector<unsigned char,GameApiAllocator<unsigned char> >(vec->begin(),vec->end());
}
FILEID GLTFImageDecoder::find_file(std::string filename)
{
  std::map<FILEID,std::string>::iterator i=filenames2.begin();
  for(;i!=filenames2.end();i++)
    {
      std::pair<FILEID,std::string> p = *i;
      //std::cout << "@" << p.second << "@==@" << filename << "@" << std::endl;
      if (p.second == filename) return p.first;
    }
  FILEID id;
  id.id = -1;
  std::cout << "ERROR, GLTFImageDecoder::find_file()" << std::endl;
  return id;
}
FILEID GLTFImageDecoder::add_file(std::vector<unsigned char,GameApiAllocator<unsigned char> > *vec, std::string filename)
{
  //std::cout << "add_file:" << filename << std::endl;
  FILEID id = get_new_file_id();
  //std::cout << "add_file:" << id.id << "::" << filename << std::endl;
  assert(!files2[id]);
  files2[id] = vec; //new std::vector<unsigned char>(vec.begin(),vec.end());
  filenames2[id] = filename;
  return id;
}
void GLTFImageDecoder::start_decode_process(FETCHID fetch_id, FILEID id, int req_width, int req_height)
{
  //std::cout << "start_process:" << id.id << std::endl;
  tinygltf::Image *img = new tinygltf::Image;
#ifdef THREADS
#ifdef EMSCRIPTEN
  std::string filename = get_fetch_filename(fetch_id);
  start_gltf_bitmap_thread(img, req_width, req_height, &(files2[id]->operator[](0)), files2[id]->size(), this, id.id,filename);
#else
  // not-emscripten -> do syncronous version
  ThreadInfo_gltf_bitmap *info = new ThreadInfo_gltf_bitmap;
  info->image = img;
  info->req_width = req_width;
  info->req_height = req_height;
  info->decoder = this;
  info->decoder_item = id.id;
  info->bytes = &(files2[id]->operator[](0));
  info->size = files2[id]->size();
  thread_func_gltf_bitmap((void*)info);
#endif
#else
  ThreadInfo_gltf_bitmap *info = new ThreadInfo_gltf_bitmap;
  info->image = img;
  info->req_width = req_width;
  info->req_height = req_height;
  info->decoder = this;
  info->decoder_item = id.id;
  info->bytes = &(files2[id]->operator[](0));
  info->size = files2[id]->size();
  thread_func_gltf_bitmap((void*)info);
#endif
  
}
void GLTFImageDecoder::set_decode_callback(FILEID id, void (*fptr)(void*), void *user_data)
{
  decode_cb[id] = fptr;
  decode_user_data[id] = user_data;
}

//void GLTFImageDecoder::decode_file(FILEID id)
//{
//}
bool GLTFImageDecoder::is_decoded(FILEID id)
{
  return decoded_files[id]!=0;
}
std::vector<unsigned char, GameApiAllocator<unsigned char> > *GLTFImageDecoder::get_decoded_file(FILEID id)
{
  return decoded_files[id];
}
IMAGEID GLTFImageDecoder::convert_to_image(std::vector<unsigned char> &vec)
{
}
Bitmap<Color> *GLTFImageDecoder::get_converted_image(IMAGEID id)
{
}
					  
