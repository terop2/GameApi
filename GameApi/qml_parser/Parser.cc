
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <fstream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#define USE_SDL
#define HAS_FILESYSTEM

#undef USE_WVGA
#define USE_VGA 
#undef USE_WQVGA
#undef USE_QVGA
#undef USE_TEST

#define Depth_2bit
#undef Depth_4bit
#undef Depth_8bit
#undef Depth_16bit

#ifdef USE_WVGA
#define tp_screen_width 800
#define tp_screen_height 480
#endif
#ifdef USE_VGA
#define tp_screen_width 640
#define tp_screen_height 480
#endif
#ifdef USE_WQVGA
#define tp_screen_width 480
#define tp_screen_height 272
#endif
#ifdef USE_QVGA
#define tp_screen_width 320
#define tp_screen_height 240
#endif
#ifdef USE_TEST
#define tp_screen_width 320
#define tp_screen_height 320
#endif

#ifdef USE_SDL
#define NO_SDL_GLEXT 
#include <SDL.h>
#include <cmath>
#endif

namespace GameApi
{
  struct Env { };
  struct EveryApi { };
};
struct Point { float x,y,z; };

int get_screen_width()
{
  return tp_screen_width;
}
int get_screen_height()
{
  return tp_screen_height;
}

std::string to_string(int val)
{
  std::stringstream ss;
  ss << val;
  return ss.str();
}

struct BufferRef
{
  unsigned int *buffer;
  unsigned int width;
  unsigned int height;
  unsigned int ydelta;
  //static BufferRef SubBuffer(BufferRef old, int x, int y, int width, int height)
;
  static BufferRef NewBuffer(int width, int height);
  static void FreeBuffer(BufferRef ref);
  //static void CopyBuffer(BufferRef source, BufferRef target, int x, int y, int rx, int ry, int rex, int rey);

  unsigned int *operator[](int y) { return buffer+y*ydelta; }
};

BufferRef BufferRef::NewBuffer(int width, int height)
{
  if (width<0) width=0;
  if (height<0) height=0;
  if (width==0) { width++; }
  if (height==0) {height++; }
  unsigned int *buffer = new unsigned int[width*height];
  if (!buffer)
    {
      std::cout << "BufferRef::NewBuffer failed" << std::endl;
    }
  BufferRef ref;
  ref.buffer = buffer;
  ref.width = width;
  ref.height = height;
  ref.ydelta = width;
  return ref;
}

void BufferRef::FreeBuffer(BufferRef ref)
{
  delete [] ref.buffer;
}
BufferRef LoadImageFromString(std::vector<unsigned char> buffer, bool &success)
{
  std::vector<unsigned char> mem;
  int s = buffer.size();
  for(int i=0;i<s;i++) mem.push_back(buffer[i]);

  int x=0,y=0;
  int comp=0;
  stbi_uc * ptr = stbi_load_from_memory(&mem[0], mem.size(), &x, &y, &comp, 4);
  //std::cout << "ImageSize: " << x << " " << y << " " << comp << std::endl;
  unsigned int *buf = new unsigned int[x*y];
  std::memcpy(buf, ptr, x*y*sizeof(unsigned int));
  free(ptr);

  BufferRef ref;
  ref.buffer = (unsigned int *)buf;
  ref.width = x;
  ref.height = y;
  ref.ydelta = x;

#if 1
  if (comp==3||comp==4)
  for(int yy=0;yy<y;yy++)
    for(int xx=0;xx<x;xx++)
      {
	unsigned int val = ref.buffer[xx+yy*ref.ydelta];
	unsigned int a = val &0xff000000;
	unsigned int r = val &0xff0000;
	unsigned int g = val &0x00ff00;
	unsigned int b = val &0x0000ff;
	r>>=16;
	g>>=8;

	b<<=16;
	g<<=8;
	val = a+r+g+b; 
	ref.buffer[xx+yy*ref.ydelta] = val;
      }
#endif
  success = true;
  return ref;
  

}



std::string qml_path;
std::string replace_path_separators(std::string s);

std::vector<unsigned char> load_file(std::string qml_relative_path, bool bin)
{
#ifdef HAS_FILESYSTEM
  std::string filename =(qml_path + "/" + qml_relative_path); 
  filename = replace_path_separators(filename);
  std::ios_base::openmode mode;// = std::ios_base::in;
  mode = std::ios_base::binary|std::ios_base::in;
  std::ifstream s(filename.c_str(),mode);
  char ch;
  std::string line;
  while(s.get(ch)) line+=ch;
  if (!bin) {
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end());
  }
  if (line.size()>0)
    std::cout << "load_file " << filename << std::endl;
  else
    std::cout << "load_file failed" << filename << std::endl;
  //std::string line;
  //std::getline(s,line,(char)s.eof());
  
  //std::cout << "load_file:" << line.size() << std::endl;
#else
#endif
  return std::vector<unsigned char>(line.begin(), line.end());
}
BufferRef decode_image_file(std::vector<unsigned char> vec)
{
  bool success = false;
  BufferRef ref = LoadImageFromString(vec, success);
  if (!success) std::cout << "Error in decode image file" << std::endl;
  return ref;
}

BufferRef scale_bitmap(BufferRef orig, int new_x, int new_y)
{
  if (!orig.buffer) return orig;
  BufferRef ref = BufferRef::NewBuffer(new_x, new_y);
  int sx=ref.width;
  int sy=ref.height;
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++) {
      float xx = x;
      float yy = y;
      xx/=ref.width;
      yy/=ref.height;
      xx*=orig.width;
      yy*=orig.height;
      int ax = int(xx);
      int ay = int(yy);
      unsigned int color = orig.buffer[ax+ay*orig.ydelta];
      ref.buffer[x+y*ref.ydelta] = color;
    }
  return ref;
}

std::string strip_spaces(std::string s)
{
  int start=0;
  while(s[start]==' '||s[start]=='\n'||s[start]=='\"') start++;
  int end=s.size()-1;
  while(s[end]==' '||s[end]=='\n'||s[end]=='\"') end--;
  return s.substr(start,end-start+1);

}
std::string replace_path_separators(std::string s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='/') s[i]='\\';
    }
  return s;
}

class Splitter
{
public:
  GameApi::Env *e;
  GameApi::EveryApi *ev;
public:
  virtual void Init()=0;
  virtual int Iter()=0;
  virtual void Destroy()=0;
  virtual Splitter* NextState(int code) { return this; }
  virtual void EnvTransfer(Splitter *next) { next->e = e; next->ev = ev; }
};



class QMLData
{
public:
  virtual void Prepare()=0;
  virtual QMLData* Parent() const=0;
  virtual std::string Type() const=0;
  virtual int NumImports() const=0;
  virtual std::string Import(int import_i) const=0;
  virtual int NumAttribs() const=0;
  virtual std::string AttrName(int attr_i) const=0;
  virtual std::string AttrValue(int attr_i) const=0;
  virtual int NumChildren() const=0;
  virtual std::string Name(int child_i) const=0;
  virtual std::string Type(int child_i) const=0;
  virtual QMLData *Children(int child_i) const=0;
};


void ClearDepthBuffer(float *arr, int width, int height)
{
  float val = std::numeric_limits<float>::max();
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	*arr++ = val;
      }
}

enum FrameBufferFormat
  {
    F_Mono1,
    F_Mono8,
    F_RGB565,
    F_RGB888,
    F_RGBA8888
  };
enum DrawBufferFormat
  {
    D_Mono1,
    D_RGBA8888
  };

struct FrameLoopEvent
{
  int type;
  int ch;
  Point cursor_pos;
  int button;
  bool pin[9];
  float time = 0.0;
};

class SourceBitmap
{
public:
  SourceBitmap(DrawBufferFormat fmt, int depth);
  SourceBitmap(const SourceBitmap &src);
  void operator=(const SourceBitmap &src);
  void set_data(void *data, int width, int height, int ydelta);
  void set_data_mono1(void *data, int width, int height, int ydelta);
  ~SourceBitmap();
public:
  void *m_data;
  DrawBufferFormat fmt;
  int m_width;
  int m_height;
  int m_ydelta;
  int m_depth;
  bool m_owned;
  bool m_owned2;
};


class SourceBitmap;

class FrameBuffer
{
public:
  virtual void Prepare()=0;
  virtual void handle_event(FrameLoopEvent &e)=0;
  virtual void frame()=0;
  virtual void *Buffer() const=0;
  virtual int Width() const=0;
  virtual int Height() const =0;
  virtual int Depth() const =0;
  virtual FrameBufferFormat format() const=0;
  virtual float *DepthBuffer() const=0;
  virtual void draw_sprite(SourceBitmap *bm, int x, int y) =0;
};


struct DrawLoopEnv
{
  DrawBufferFormat format;
  //void *drawbuffer;
  //int drawbuffer_width;
  //int drawbuffer_height;
  //int drawbuffer_depth;
  FrameBuffer *drawbuffer;

  float time = 0.0;
  float delta_time = 0.0;

  float *depth_buffer;
};



class FrameBufferLoop
{
public:
  virtual void Prepare()=0;
  virtual void handle_event(FrameLoopEvent &e)=0;
  virtual void frame(DrawLoopEnv &e)=0;

};

std::vector<std::string> split_from_char(std::string s, char c)
{
  int ss = s.size();
  std::vector<std::string> vec;
  int last_pos = 0;
  for(int i=0;i<ss;i++)
    {
      if (s[i]==c) {
	std::string part = s.substr(last_pos,i-last_pos);
	vec.push_back(part);
	last_pos = i+1;
      }
    }
  vec.push_back(s.substr(last_pos,ss-last_pos));
  return vec;
}

std::string focus_to_braces(std::string s)
{
  int ss = s.size();
  int left_pos = -1;
  int right_pos = -1;
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='{') left_pos = i;
      if (s[i]=='}') right_pos = i;
    }
  if (left_pos!=-1 && right_pos!=-1) {
    left_pos++;
    right_pos--;
    return s.substr(left_pos,right_pos-left_pos+1);
  }
  return s;
}

std::string remove_comment(std::string s)
{
  int ss = s.size();
  std::string res = s;
  for(int i=0;i<ss-1;i++)
    {
      if (s[i]=='/' && s[i+1]=='/')
	{
	  res = s.substr(0,i);
	}
    }
  return res;
}

std::pair<std::string,std::string> split_attr(std::string attr, bool &success);
struct PVec;
class QMLLoop;
int int_expr_parser(QMLLoop *loop, const PVec &vec, std::string value);


int constants(std::string id)
{
  if (id=="Qt.TopEdge") return 0;
  if (id=="Qt.BottomEdge") return 1;
  return -1;
}

struct PVec {
  std::vector<std::string> p1;
  std::vector<std::string> p2;
  mutable std::vector<int> is_used;
  void print(std::string label) {
    int s = p1.size();
    for(int i=0;i<s;i++)
      {
	std::cout << "Attr(" << label << ")";
	if (is_used[i]) std::cout << "*"; else std::cout << " ";
	std::cout << ": " << p1[i] << "=" << p2[i] << std::endl; 
      }
  }
  std::string get_val(std::string key) const
  {
    int s = p1.size();
    for(int i=0;i<s;i++) { 
      if (p1[i]==key) { 
	is_used[i]=true; 
	//std::cout << "Fetching " << key << ":==" << p2[i] << std::endl;

	return p2[i]; 
      }  
    }
    //std::cout << "Fetching " << key << ":== @" << std::endl;
    return "@";
  }
  int get_int_val(std::string key) const {
    std::string s = get_val(key);
    std::stringstream ss;
    int val;
    if (!ss >> val) val=-1;
    return val;
  }
  bool get_bool_val(std::string key) const {
    std::string s = get_val(key);
    std::stringstream ss;
    bool val = false;
    if (!ss >> val) val=false;
    return val;    
  }
  std::string get_id() const { return get_val("id"); }
  int get_y(QMLLoop *loop) const {
    std::string val = get_val("y");
    int v = int_expr_parser(loop,*this, val);
    return v;
  }
  int get_x(QMLLoop *loop) const {
    std::string val = get_val("x");
    int v = int_expr_parser(loop,*this, val);
    return v;
  }
  int get_width(QMLLoop *loop) const { 
    std::string val = get_val("width");
    int v = int_expr_parser(loop,*this,val);
    //std::cout << loop->Name() << ".width decided: " << v << std::endl;
    return v;
  }
  int get_height(QMLLoop *loop) const {
    std::string val = get_val("height");
    int v=int_expr_parser(loop,*this, val);
    //std::cout << loop->Name() << ".height decided: " << v << std::endl;
    return v;
  }
  int get_radius(QMLLoop *loop) const {
    std::string val = get_val("radius");
    int v=int_expr_parser(loop,*this, val);
    return v;
  }
  int get_enabled(QMLLoop *loop) const {
    std::string val = get_val("enabled");
    int v=int_expr_parser(loop,*this, val);
    return v;
  }
  bool is_visible(QMLLoop *loop) const { 
    std::string v = get_val("visible");
    return int_expr_parser(loop, *this, v)==1;
  }
  std::string get_source() const { return get_val("source"); }
  std::string get_image_source() const { return get_val("imageSource"); }
  int get_edge() const { 
    std::string s2 = ""; //get_val("property int edge");
    std::string s = get_val("edge");
    if (s=="Qt.TopEdge"||s2=="Qt.TopEdge") { return 0; }
    if (s=="Qt.BottomEdge"||s2=="Qt.BottomEdge") { return 1; }
    return -1;
  }
  std::string get_title() const { return get_val("title"); }
  int get_anchor_horizontal_center(QMLLoop *loop) const
  {
    std::string val = get_val("anchors.horizontalCenter");
    return int_expr_parser(loop,*this,val);
  }
  int get_anchor_top(QMLLoop *loop) const
  {
    std::string val= get_val("anchors.top");
    return int_expr_parser(loop,*this,val);
  }
  int get_anchor_bottom(QMLLoop *loop) const
  {
    std::string val=get_val("anchors.bottom");
    return int_expr_parser(loop,*this,val);
  }
  int get_onClicked(QMLLoop *loop) const
  {
    std::string val=get_val("onClicked");
    return int_expr_parser(loop,*this,val);    
  }
  std::pair<int,int> get_source_size() const
  {
    std::string val = get_val("sourceSize");
    if (val=="@") return std::make_pair(-1,-1);
    //std::cout << "sourcesize val:" << val << std::endl;
    val = focus_to_braces(val);
    val = strip_spaces(val);
    std::vector<std::string> split = split_from_char(val,';');
    //std::cout << "sourcesize:" << split.size() << std::endl;
    if (split.size()!=2) { std::cout << "get_source_size error #1" << std::endl; return std::make_pair(-1,-1); }
    bool success1=false, success2=false;
    std::pair<std::string,std::string> p = split_attr(split[0],success1);
    std::pair<std::string,std::string> p2 = split_attr(split[1],success2);
    if (!success1 ||!success2) { std::cout << "get_source_size error #2" << std::endl; return std::make_pair(-1,-1); }
    int w=-1,h=-1;
    p.first=strip_spaces(p.first);
    p2.first=strip_spaces(p2.first);
    p.second=strip_spaces(p.second);
    p2.second=strip_spaces(p2.second);
    //std::cout << p.first << "--" << p.second << std::endl;
    //std::cout << p2.first << "--" << p2.second << std::endl;
    if (p.first=="width") { std::stringstream ss(p.second); ss>> w; } else
    if (p2.first=="width") { std::stringstream ss(p2.second); ss>> w; } else
      {
	std::cout << "get_source_size error #3" << std::endl;
      }
    if (p.first=="height") { std::stringstream ss(p.second); ss>> h; } else 
    if (p2.first=="height") { std::stringstream ss(p2.second); ss>> h; } else
      {
	std::cout << "get_source_size error #4" << std::endl; 
	return std::make_pair(-1,-1);
      }
    return std::make_pair(w,h);
  }
};


class QMLLoop : public FrameBufferLoop
{
public:
  virtual QMLLoop *Parent() const=0;
  virtual std::string Name() const=0;
  virtual void PushChild(std::string name, QMLLoop *child)=0;
  virtual void PushAttr(std::string attr_name, std::string attr_value)
  {
      vec.p1.push_back(attr_name);
      vec.p2.push_back(attr_value);
      vec.is_used.push_back(0);
  }
  virtual std::string GetAttr(std::string attr_name) const
  {
    return vec.get_val(attr_name);
  }
  virtual PVec GetAttr2() const { return vec; }
  virtual int NumChildren() const=0;
  virtual QMLLoop *Children(int i) const=0;
  virtual std::string ChildName(int i) const=0;
  virtual std::pair<int,int> get_size() const=0;
  virtual std::pair<int,int> get_pos() const=0;
  virtual void SetRect(int x, int y, int width, int heigth)=0;
  virtual void CommitRect(int x, int y, int width, int heigth)=0;
  virtual void set_vec(PVec vec)=0;
  virtual std::string fetch_value(std::string key)=0;
protected:
  PVec vec;
};


bool is_import(std::string line) { 
  std::stringstream ss(line);
  std::string word;
  ss >> word;
  if (word=="import") return true;
  return false; 
}
bool is_component(std::string line) {
  if (line.size()>0) {
    char ch = line[line.size()-1];
    //std::cout << "CH:" << line << ";;" << ch<< std::endl;
    if (ch=='{') return true;
    return false;
  }
  return false;
}
bool is_component_end(std::string line) {
  if (line.size()>0) {
    char ch = line[line.size()-1];
    //std::cout << "CH2:" << line << ";;" << ch << std::endl;
    if (ch=='}') return true;
    return false;
  }
  return false;
}
int find_ch(std::string s, char ch)
{
  int ss = s.size();
  for(int i=0;i<ss;i++) {
    if (s[i]==ch) return i;
  }
  return -1;
}


std::pair<std::string,std::string> split_attr(std::string attr, bool &success)
{
  int ch = find_ch(attr, ':');
  int ch2 = find_ch(attr, '=');
  int ch3 = find_ch(attr, '{');
  if (ch3!=-1) {
    std::string first = attr.substr(0,ch3);
    std::string second = attr.substr(ch3,attr.size()-ch3+1);
    success=true;
    return std::make_pair(first,second);
  }
  if (ch==-1&&ch2==-1) {success=false; return std::make_pair("",""); }
  if (ch==-1) ch=ch2;
  std::string first = attr.substr(0,ch);
  std::string second = attr.substr(ch+1,attr.size()-ch-1);
  success = true;
  return std::make_pair(first,second);
}
bool is_attribute(std::string line)
{
  if (is_component(line)) return false;
  bool success = false;
  std::pair<std::string,std::string> p = split_attr(line, success);
  return success;
}
bool is_property(std::string line) { return false; }
bool is_pragma(std::string line) { return false; }
bool is_comment_start(std::string line)
{
  int s = line.size();
  int pos = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='/' && line[i+1]=='*') pos=i;
    }
  if (pos==-1) return false;
  int pos2 = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='*' && line[i+1]=='/') pos2=i;
    }
  if (pos2!=-1) return false;
  return true;
}

bool is_comment_end(std::string line)
{
  int s = line.size();
  int pos = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='*' && line[i+1]=='/') pos=i;
    }
  if (pos==-1) return false;
  return true;

}

void BufferRefToSourceBitmap(BufferRef &ref, SourceBitmap &target, DrawBufferFormat fmt)
{
  unsigned int *buffer = ref.buffer;
  unsigned int width = ref.width;
  unsigned int height = ref.height;
  unsigned int ydelta = ref.ydelta;
  switch(fmt) {
  case D_Mono1:
    //std::cout << "D_Mono1 not supported in BufferRefToSourceBitmap" << std::endl;
    target.set_data_mono1(buffer, width, height, ydelta);
    break;
  case D_RGBA8888:
    target.set_data(buffer, width, height, ydelta);
    break;
  default:
    std::cout << "Default not supported in BufferRefToSourceBitmap" << std::endl;
    break;
  };
}
SourceBitmap::SourceBitmap(DrawBufferFormat fmt, int depth) : m_data(0), fmt(fmt), m_width(1), m_height(1), m_ydelta(1), m_depth(depth),m_owned(false),m_owned2(false) 
  {
  }
SourceBitmap::SourceBitmap(const SourceBitmap &src)  
{ 
  if (src.m_owned && src.fmt==D_Mono1 && src.m_data) {
    unsigned char *buf = (unsigned char*)src.m_data;
    unsigned char *buf2 = new unsigned char[src.m_width*src.m_height/8];
    std::memcpy(buf2,buf,src.m_width*src.m_height/8);
    m_data = buf2;
  }
  if (src.m_owned2 && src.fmt==D_RGBA8888 && src.m_data) {
    unsigned int *buf = (unsigned int*)src.m_data;
    unsigned int *buf2 = new unsigned int[src.m_width*src.m_height];
    std::memcpy(buf2,buf,src.m_width*src.m_height*sizeof(unsigned int));
    m_data = buf2;
  }
  if (!src.m_owned && src.fmt==D_Mono1) { m_data = src.m_data; }
  if (!src.m_owned2 && src.fmt==D_RGBA8888) { m_data=src.m_data; }
  m_width = src.m_width;
  m_height = src.m_height;
  m_ydelta = src.m_ydelta;
  m_depth = src.m_depth;
  m_owned = src.m_owned;
  m_owned2 = src.m_owned2;
  fmt = src.fmt;
}
void SourceBitmap::operator=(const SourceBitmap &src)
{
  if (src.m_owned && src.fmt==D_Mono1 && src.m_data) {
    unsigned char *buf = (unsigned char*)src.m_data;
    unsigned char *buf2 = new unsigned char[src.m_width*src.m_height/8];
    std::memcpy(buf2,buf,src.m_width*src.m_height/8);
    m_data = buf2;
  }
  if (src.m_owned2 && src.fmt==D_RGBA8888 && src.m_data) {
    unsigned int *buf = (unsigned int*)src.m_data;
    unsigned int *buf2 = new unsigned int[src.m_width*src.m_height];
    std::memcpy(buf2,buf,src.m_width*src.m_height*sizeof(unsigned int));
    m_data = buf2;
  }
  if (!src.m_owned && src.fmt==D_Mono1) { m_data = src.m_data; }
  if (!src.m_owned2 && src.fmt==D_RGBA8888) { m_data=src.m_data; }
  m_width = src.m_width;
  m_height = src.m_height;
  m_ydelta = src.m_ydelta;
  m_depth = src.m_depth;
  m_owned = src.m_owned;
  m_owned2 = src.m_owned2;
  fmt = src.fmt;
}
  void SourceBitmap::set_data(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 1 " << std::endl;
    unsigned int *buf = new unsigned int[width*height];
    std::memset(buf, 0, width*height);
    m_data = buf;
    std::memcpy(buf, data, width*height*sizeof(unsigned int));
    m_width=width;
    m_height=height;
    m_ydelta=width;
    m_owned2 = true;
  }
  void SourceBitmap::set_data_mono1(void *data, int width, int height, int ydelta) {
    //std::cout << "set_data 2 " << std::endl;
    unsigned char *buf = new unsigned char[width*height/8];
    std::memset(buf, 0, width*height/8);
    m_data = buf;
    m_width=width;
    m_height=height;
    m_ydelta =width/8;
    for(int y=0;y<height;y++)
      for(int x=0;x<width;x++)
	{
	  unsigned int color = ((unsigned int*)data)[x+y*ydelta];
	  bool b = false;
	  //if ((color & 0xff000000)>0x80000000) b=true;
	  if ((color & 0x00ff0000)>0x00800000) b=true;
	  if ((color & 0x0000ff00)>0x00008000) b=true;
	  if ((color & 0x000000ff)>0x00000080) b=true;
	  if (b)
	    buf[x/8+y*m_ydelta] |= 1 << (7-(x%8));
	}
    m_owned = true;
  }
  SourceBitmap::~SourceBitmap() { 
    if (m_owned) delete[]((unsigned char*)m_data); 
    if (m_owned2) delete[]((unsigned int*)m_data);
  }



class QMLRoot : public QMLData
{
public:
  QMLRoot(std::string type) : type(type) { }
  virtual void Prepare() { }
  virtual QMLData* Parent() const { return 0; }
  virtual std::string Type() const { return "root"; }
  virtual int NumImports() const { return 0; }
  virtual std::string Import(int import_i) const { return ""; }
  virtual int NumAttribs() const { return 0; }
  virtual std::string AttrName(int attr_i) const { return ""; }
  virtual std::string AttrValue(int attr_i) const { return ""; }
  virtual int NumChildren() const { return m_child_map.size(); }
  virtual std::string Name(int child_i) const { return "root_" + type; }
  virtual std::string Type(int child_i) const { return type; }
  virtual QMLData *Children(int child_i) const { return m_child_map[child_i]; }
  virtual std::string ChildName(int child_i) const { return ""; }
public:
  std::vector<QMLData*> m_child_map;
  std::string type;
};

class QMLLoader : public QMLData
{
public:
  QMLLoader(std::string type) : parent(new QMLRoot(type)) { ((QMLRoot*)parent)->m_child_map.push_back(this); }
  QMLLoader(QMLData *parent) : parent(parent) { ((QMLLoader*)parent)->m_child_map.push_back(this); }
  void set_data(std::vector<unsigned char> vec) { data = vec; }
  virtual void Prepare()
  {
    m_imports=std::vector<std::string>();
    m_import_children = std::vector<QMLData*>();
    m_component_name = std::vector<std::string>();
    m_component_type = std::vector<std::string>();
    m_component_children = std::vector<QMLData*>();
    m_attributes = std::vector<std::pair<std::string,std::string> >();
    m_current_component=std::vector<std::string>();

    std::string s(data.begin(), data.end());
    std::stringstream ss(s);
    std::string line;
    int level=0;
    bool comment=false;
    while(std::getline(ss,line)) {
      //std::cout << "Line: " << line << std::endl;
      if (is_comment_end(line)) { comment=false; }
      if (is_comment_start(line)) { comment=true; }
      if (comment==true) continue;
      if (is_component(line)) { 
	//std::cout << "Level++" << std::endl; 
	level++; }
      if (is_component_end(line)) {
	//std::cout << "Level--" << std::endl;
	level--;
	if (level==0) {
	  //std::cout << "End Component" << std::endl;
	  std::string last = m_current_component.back();
	  m_current_component.pop_back();
	  std::vector<unsigned char> vec(last.begin(),last.end());
	  m_component_children.push_back(new QMLLoader(this));
	  QMLData *dt = m_component_children[m_component_children.size()-1];
	  QMLLoader *load = (QMLLoader*)dt;
	  load->set_data(vec);
	  //load->Prepare();
	}

      }

      if (m_current_component.size()>0)
	{
	  //std::cout << "AppendLine" << std::endl;
	  m_current_component[m_current_component.size()-1]+=line + "\n";
	  continue;
	}
      if (is_import(line)) { 
	//std::cout << "IMPORT" << std::endl;
	m_imports.push_back(line); }
      if (is_component(line)) { 
	//std::cout << "COMPONENT" << std::endl;
	std::string line2 = line.substr(0,line.size()-1-1);
	bool success = false;
	std::pair<std::string,std::string> p = split_attr(line2, success);
	if (success) {
	  m_component_name.push_back(p.first);
	  m_component_type.push_back(p.second);
	} else {
	  m_component_name.push_back("");
	  m_component_type.push_back(line2);
	}
	m_current_component.push_back("");
      }
      if (is_attribute(line)) {
	//std::cout << "ATTRIBUTE" << std::endl;
	bool success = false;
	std::pair<std::string,std::string> p = split_attr(line, success);
	//assert(success);
	m_attributes.push_back(std::make_pair(p.first,p.second));
      }
      if (is_property(line)) { /* TODO */ }
      if (is_pragma(line)) { /* TODO */ }
    }
  }
  virtual QMLData *Parent() const { return parent; }
  virtual std::string Type() const { 
    QMLData *p = Parent();
    QMLLoader *p2a = dynamic_cast<QMLLoader*>(p);
    QMLRoot *p2b = dynamic_cast<QMLRoot*>(p);
    if (p2a) {
      int s = p2a->m_child_map.size();
      for(int i=0;i<s;i++)
	{
	  QMLData *d = p2a->m_child_map[i];
	  if (d==this) { return p->Type(i); }
	}
    } else if (p2b) {
      int s = p2b->m_child_map.size();
      for(int i=0;i<s;i++)
	{
	  QMLData *d = p2b->m_child_map[i];
	  if (d==this) { return p->Type(i); }
	}
    } else { std::cout << "Unknown type in QMLLoader::Type()" << std::endl; }
    return "@";
  }
  virtual int NumImports() const { return m_imports.size(); }
  virtual std::string Import(int import_i) const
  {
    return strip_spaces(m_imports[import_i]);
  }
  //virtual QMLData *ImportChildren(int import_i) const
  //{
  //  return m_import_children[import_i];
  //}
  virtual int NumAttribs() const
  {
    return m_attributes.size();
  }
  virtual std::string AttrName(int attr_i) const
  {
    return strip_spaces(m_attributes[attr_i].first);
  }
  virtual std::string AttrValue(int attr_i) const
  {
    return strip_spaces(m_attributes[attr_i].second);
  }
  virtual int NumChildren() const
  {
    return m_component_children.size();
  }
  virtual std::string Name(int child_i) const { return strip_spaces(m_component_name[child_i]); }
  virtual std::string Type(int child_i) const { return strip_spaces(m_component_type[child_i]); }
  virtual QMLData *Children(int child_i) const
  {
    return m_component_children[child_i];
  }
  virtual std::string ChildName(int child_i) const { return m_component_name[child_i]; }

public:
  std::vector<QMLData*> m_child_map;
private:
  QMLData *parent;

  std::vector<unsigned char> data;
  std::vector<std::string> m_imports;
  std::vector<QMLData*> m_import_children;
  std::vector<std::string> m_component_name;
  std::vector<std::string> m_component_type;
  std::vector<QMLData*> m_component_children;
  std::vector<std::pair<std::string,std::string> > m_attributes;
  std::vector<std::string> m_current_component;
};

#if 0
class QMLModule : public QMLData
{
public:
  QMLModule(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url), homepage(homepage) {}
  void Prepare() {
    //std::cout << "qtPrepare" << std::endl;
#ifndef EMSCRIPTEN
    async_load_url(e,url,homepage);
#endif
    std::vector<unsigned char> *vec = get_loaded_async_url(e,url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    load.set_data(*vec);
    load.Prepare();
  }

  virtual int NumImports() const { return load.NumImports(); }
  virtual std::string Import(int import_i) const
  {
    return load.Import(import_i);
  }
  virtual QMLData *ImportChildren(int import_i) const
  {
    return load.ImportChildren(import_i);
  }
  virtual int NumAttribs() const
  {
    return load.NumAttribs();
  }
  virtual std::string AttrName(int attr_i) const
  {
    return load.AttrName(attr_i);
  }
  virtual std::string AttrValue(int attr_i) const
  {
    return load.AttrValue(attr_i);
  }
  virtual int NumChildren() const
  {
    return load.NumChildren();
  }
  virtual std::string Name(int child_i) const { return load.Name(child_i); }
  virtual std::string Type(int child_i) const { return load.Type(child_i); }
  virtual QMLData *Children(int child_i) const
  {
    return load.Children(child_i);
  }
  virtual std::string ChildName(int child_i) const { return load.ChildName(child_i); }
private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  QMLLoader load;
};
#endif

class PrintQMLData : public FrameBufferLoop
{
public:
  PrintQMLData(QMLData *mod) : mod(mod) {}
  void Prepare() {
    mod->Prepare();
    int s = mod->NumImports();
    for(int i=0;i<s;i++) {
      std::cout << mod->Import(i) << std::endl;
    }
    int s3 = mod->NumAttribs();
    for(int i=0;i<s3;i++) {
      std::cout << mod->AttrName(i) << "::" << mod->AttrValue(i) << std::endl;
    }
    int s4 = mod->NumChildren();
    for(int i=0;i<s4;i++)
      {
	std::cout << "Children: " << i << std::endl;
	std::cout << mod->Name(i) << ":" << mod->Type(i) << std::endl;
	QMLData *dt = mod->Children(i);
	PrintQMLData *dt2 = new PrintQMLData(dt);
	dt2->Prepare();
	std::cout << "EndChildren: " << i << std::endl;
      }
  }
  void handle_event(FrameLoopEvent &e) { }
  void frame(DrawLoopEnv &e) {}
private:
  QMLData *mod;
};
extern std::string gameapi_homepageurl;
#if 0
GameApi::FML GameApi::LowFrameBufferApi::qml_print(std::string url)
{
  QMLModule *mod = new QMLModule(e,url,gameapi_homepageurl);
  PrintQMLData *dt = new PrintQMLData(mod);
  return add_framemainloop(e, dt);
}
#endif


QMLLoop *create_qml_element(QMLLoop *parent, QMLData *root, QMLData *dt);

void recursive_prepare(QMLData *mod)
{
  mod->Prepare();
  int s = mod->NumChildren();
  for(int i=0;i<s;i++)
    recursive_prepare(mod->Children(i));
}


class QMLCreateNode : public QMLLoop
{
public:
  QMLCreateNode(QMLLoop *parent, QMLData *mod) : parent(parent), mod(mod) { }
  virtual QMLLoop *Parent() const { return 0; }
  virtual void PushChild(std::string name, QMLLoop *child) { }
  virtual std::string Name() const { return "CreateNode"; }
  void Prepare() {
    // these are QMLLoader's prepares
    recursive_prepare(mod);

    // these are QMLLoop's prepares (does it recursively)
    loop = create_qml_element(parent,mod,mod);
    CommitRect(x,y,w,h);
    loop->Prepare();
  }
  void frame(DrawLoopEnv &e)
  {
    loop->frame(e);
  }
  void handle_event(FrameLoopEvent &e)
  {
    loop->handle_event(e);
  }
  virtual std::pair<int,int> get_size() const
  {
    return loop->get_size();
    //return std::make_pair(w,h); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    return loop->get_pos();
    // return std::make_pair(x,y);
  }
  virtual void SetRect(int x_, int y_, int width, int height) { 
    std::cout << "createnode setrect" << std::endl;
    x=x_; y=y_; w=width; h=height;
  }
  virtual void CommitRect(int x, int y, int width, int height) { 
    std::cout << "createnode commitrect" << std::endl;
    loop->SetRect(x,y,w,h);
  }
  virtual int NumChildren() const { return 0; }
  virtual QMLLoop *Children(int i) const { return 0; }
  virtual std::string ChildName(int child_i) const { return ""; }
  virtual void set_vec(PVec vec) { loop->set_vec(vec); }
  virtual std::string fetch_value(std::string key) { return "@"; }
private:
  QMLLoop *parent;
  QMLData *mod;
  QMLLoop *loop;
  int x,y,w,h;
};

#if 0
GameApi::FML GameApi::LowFrameBufferApi::qml_create_node(std::string url)
{
  QMLModule *mod = new QMLModule(e,url,gameapi_homepageurl);
  return add_framemainloop(e,new QMLCreateNode(mod));
}
#endif


class UnknownElement : public QMLLoop
{
public:
  UnknownElement(std::string context, std::string type) : context(context), type(type) { }
  virtual QMLLoop *Parent() const { return 0; }
  virtual void PushChild(std::string name, QMLLoop *child) { }
  virtual std::string Name() const { return "Unknown"; }
  virtual void Prepare()
  {
    std::cout << "UnknownElement: " << context << "." << type << std::endl;
  }
  virtual void handle_event(FrameLoopEvent &e) { }
  virtual void frame(DrawLoopEnv &e) { }
  virtual void SetRect(int x, int y, int width, int heigth) { }
  virtual void CommitRect(int x, int y, int width, int heigth) { }
  virtual int NumChildren() const { return 0; }
  virtual QMLLoop *Children(int i) const { return 0; }
  virtual std::string ChildName(int child_i) const { return ""; }
  virtual void set_vec(PVec vec) { }
  virtual std::string fetch_value(std::string key) { return "@"; }

private:
  std::string context, type;
  PVec vec;
};
class UnknownWithChildren : public QMLLoop
{
public:
  UnknownWithChildren(QMLLoop *parent, std::string context, std::string type) : parent(parent), context(context), type(type) { }
  virtual QMLLoop *Parent() const { return parent; }
  virtual void PushChild(std::string name, QMLLoop *child) { 
    children.push_back(child);
    childnames.push_back(name);
  }

  virtual std::string Name() const { return "UnknownWithChildren(" + type + ")"; }
  virtual void Prepare()
  {
    //std::cout << "Unknown: " << context << "." << type << std::endl;
    CommitRect(m_x,m_y,m_width,m_height);
    PrepareChildren();
    std::cout << "Unknown:" << context << "." << type << std::endl;
    vec.print("UnknownWithChildren");
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->handle_event(e);

  }
  virtual void frame(DrawLoopEnv &e)
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->frame(e);

  }
  virtual std::pair<int,int> get_size() const
  {
    if (children.size()>0)
      return children[0]->get_size();
    return std::make_pair(m_width,m_height); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    if (children.size()>0)
      return children[0]->get_pos();
    return std::make_pair(m_x,m_y);
  }

  virtual void SetRect(int x, int y, int width, int height) { 
    std::cout << "unknown setrect at "<< type << " with " << children.size() << "childs." << std::endl;
    m_x=x; m_y=y; m_width=width; m_height = height;
  }
  virtual void CommitRect(int x, int y, int width, int height) { 
     int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->SetRect(x,y,width,height);
  }
  virtual int NumChildren() const { return children.size(); }
  virtual QMLLoop *Children(int i) const
  {
    return children[i];
  }
  virtual std::string ChildName(int child_i) const { return childnames[child_i]; }

  void PrepareChildren()
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->Prepare();
  }
  virtual void set_vec(PVec vec) { 
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->set_vec(vec); 
  }
  virtual std::string fetch_value(std::string key)
  {
    return "@";
  }

private:
  QMLLoop *parent;
  std::string context;
  std::string type;
  //PVec vec;
  std::vector<QMLLoop*> children;
  std::vector<std::string> childnames;
  int m_x,m_y,m_width,m_height;
};







class LowFrameBuffer : public FrameBuffer
{
public:
  LowFrameBuffer(FrameBufferLoop *loop, int format, int width, int height, int depth) : loop(loop),m_format(format), width(width), height(height), depth(depth) { firsttime = true;  }
  virtual void Prepare()
  {

    loop->Prepare();

    depth_buffer = new float[width*height];
    // 
    FrameBufferFormat fmt = (FrameBufferFormat)m_format;
    switch(fmt) {
    case FrameBufferFormat::F_Mono1:
      buffer = new unsigned char[width*height/8];
      size = width*height/8;
      break;
    case FrameBufferFormat::F_Mono8:
      buffer = new unsigned char[width*height];
      size = width*height;
      break;
    case FrameBufferFormat::F_RGB565:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGB888:
      buffer = new unsigned short[width*height];
      size=width*height*sizeof(unsigned short);
      break;
    case FrameBufferFormat::F_RGBA8888:
      buffer = new unsigned int[width*height];
      size=width*height*sizeof(unsigned int);
      break;
    }
    firsttime = true;
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
    auto p3 = std::chrono::system_clock::now();
    auto dur_in_seconds3 = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p3.time_since_epoch()));
    double val = dur_in_seconds3.count();  
    e.time = float(val-start_time_epoch);

    loop->handle_event(e);
  }
  virtual void frame()
  {
    // clear the buffer
    std::memset(buffer,0, size);
    ClearDepthBuffer(depth_buffer, width, height);
    if (firsttime) {
      auto p2 = std::chrono::system_clock::now();
      auto dur_in_seconds = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p2.time_since_epoch()));
      start_time = dur_in_seconds.count();
      //start_time/=1000.0;
      start_time_epoch = start_time;
      firsttime = false;
    }
    DrawLoopEnv e;
    e.format = D_RGBA8888;
    e.drawbuffer = this;
    e.depth_buffer = depth_buffer;
    //e.drawbuffer_width = width;
    //e.drawbuffer_height = height;
    //e.drawbuffer_depth = depth;
    auto p3 = std::chrono::system_clock::now();
    auto dur_in_seconds3 = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(p3.time_since_epoch()));
    double val = dur_in_seconds3.count();  
    //val/=1000.0;
    // std::cout << "FTime: " << val << " " << start_time_epoch << std::endl;
    
    //std::chrono::time_point p = std::chrono::high_resolution_clock::now();
    e.time = float(val-start_time_epoch);
    e.delta_time = float(val-start_time);
    start_time = val;
    loop->frame(e);

    // TODO submit to iot platform
  }
  virtual void *Buffer() const { return buffer; }
  virtual int Width() const { return width; }
  virtual int Height() const { return height; }
  virtual int Depth() const { return depth; }
  virtual FrameBufferFormat format() const { return (FrameBufferFormat)m_format; }
  virtual float *DepthBuffer() const { return depth_buffer; }

  virtual void draw_sprite(SourceBitmap *bm, int pos_x, int pos_y)
  {
    int sp_width=bm->m_width;
    int sp_height = bm->m_height;
    int sp_ydelta = bm->m_ydelta;
    void *buf = bm->m_data;
    switch(m_format) {
    case FrameBufferFormat::F_Mono1:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      int s_pos = (x/8+y*sp_ydelta);
	      int t_pos = ((pos_x+x)/8+(y+pos_y)*(width));
	      int s_bit = x&0x7;
	      int t_bit = (x+pos_x)&0x7;
	      unsigned char b = ((unsigned char*)buf)[s_pos];
	      b>>=7-s_bit;
	      b&=1;
	      if (b) {
		((unsigned char*)buffer)[t_pos]|=1<<(7-t_bit);
	      } else {
		((unsigned char*)buffer)[t_pos]&=~(1<<(7-t_bit));
	      }
	      //((unsigned char*)buffer)[(pos_x+x)/8+(y+pos_y)*width/8] = ((unsigned char*)buf)[x/8+y*sp_ydelta];

	    }
	}
	//std::cout << "draw_sprite, mono1->mono1 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono1 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_Mono8:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->mono8 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->mono8 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB565:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb565 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb565 not implemented" << std::endl;
	break;

      };
      break;
    case FrameBufferFormat::F_RGB888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:
	std::cout << "draw_sprite, mono1->rgb888 not implemented" << std::endl;
	break;

      case DrawBufferFormat::D_RGBA8888:
	std::cout << "draw_sprite, rgba8888->rgb888 not implemented" << std::endl;
	break;
      };
      break;
    case FrameBufferFormat::F_RGBA8888:
      switch(bm->fmt) {
      case DrawBufferFormat::D_Mono1:

	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	for(int y=start_y;y<h;y++)
	  for(int x=start_x;x<w;x++)
	    {
	      //std::cout << "(" << x << "," << y << ") " << buffer << " " << buf << std::endl;
	      int pos = (x/8+y*sp_ydelta);
	      int bit = x&0x7;
	      unsigned char b = ((unsigned char*)buf)[pos];
	      b>>=7-bit;
	      b&=1;

	      ((unsigned int*)buffer)[pos_x+x+(y+pos_y)*width] = b?0xffffffff:0xff000000;
	    }
	} 

	//std::cout << "draw_sprite, mono1->rgba8888 not implemented" << std::endl;
	break;
      case DrawBufferFormat::D_RGBA8888:
	{
	int start_x = 0;
	int start_y = 0;
	if (pos_x<0) start_x=-pos_x;
	if (pos_y<0) start_y=-pos_y;
	int w = std::min(sp_width, width-pos_x);
	int h = std::min(sp_height, height-pos_y);
	unsigned int *bufp = &((unsigned int*)buffer)[pos_x+start_x+(start_y+pos_y)*width];
	unsigned int *cp = &((unsigned int*)buf)[start_x+start_y*sp_ydelta];
	int delta_w = w-start_x;
	for(int y=start_y;y<h;y++) {
	  for(int x=start_x;x<w;x++)
	    {
	      unsigned int c = *cp;
	      
	      if (c>0x80000000) {
		*bufp = c;
	      }
	      cp++;
	      bufp++;
	    }
	  cp+=sp_ydelta-delta_w;
	  bufp+=width-delta_w;
	}
	}
	break;
      };
      break;
    }

  }
private:
  bool firsttime;
  FrameBufferLoop *loop;
  int m_format;
  void *buffer;
  int size;
  int width;
  int height;
  int depth;
  double start_time;
  double start_time_epoch;
  float *depth_buffer;
};
#ifdef USE_SDL

SDL_Window *sdl_framebuffer_window;
SDL_Surface *sdl_framebuffer;
SDL_Surface *init_sdl_surface_framebuffer(int scr_x, int scr_y)
{
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE|SDL_INIT_JOYSTICK);
  //std::cout << "SDL_Init: " << g_low->sdl->SDL_GetError() << std::endl;
  sdl_framebuffer_window = SDL_CreateWindow("Framebuffer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scr_x, scr_y, SDL_WINDOW_SHOWN);
  sdl_framebuffer = SDL_GetWindowSurface(sdl_framebuffer_window);
  if (!sdl_framebuffer) { std::cout << "sdl_framebuffer NULL" << std::endl; exit(0); }
  return sdl_framebuffer;
}

void CopyFrameToSurface(FrameBuffer *buf, SDL_Surface *surf)
{
  SDL_LockSurface(surf);
  int width = buf->Width();
  int height = buf->Height();
  //int depth = buf->Depth();
  void *buffer = buf->Buffer();
  FrameBufferFormat f = buf->format();
  for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
      {
	// GETPIXEL
	unsigned int val = 0xffff00ff;
#if 1
	switch(f) {
	case FrameBufferFormat::F_Mono1:
	  {
	  int byte = (y*width + x)/8;
	  int bit = (y*width+x)&0x7;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  v>>=7-bit;
	  v&=1;
	  val = v?0xffffffff:0xff000000;
	  //std::cout << "F_Mono1 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_Mono8:
	  {
	  int byte = y*width + x;
	  unsigned char v = *(((unsigned char*)buffer) + byte);
	  unsigned int vv = v;
	  vv <<= 8;
	  unsigned int vvv = v;
	  vvv <<= 16;
	  val = 0xff000000 + v + vv + vvv;
	  //std::cout << "F_Mono8 not supported\n" << std::endl;
	  break;
	  }
	case FrameBufferFormat::F_RGB565:
	  //std::cout << "F_RGB565 not supported\n" << std::endl;
	  {
	  unsigned short *ptr = ((unsigned short*)buffer) + y*width+x;
	  unsigned short v = *ptr;
	  unsigned int r = v&0x001f;
	  float rv = r/16.0;
	  rv*=255;
	  unsigned int g = v&(((4+2+1)+(8+4+2))*0x10);
	  g>>=5;
	  float gv = g/32.0;
	  gv*=255;
	  unsigned int b = v&(((8+4+2+1)+(8))*(1024));
	  b>>=11;
	  float bv = b/16.0;
	  bv*=255;
	  unsigned int rr = (unsigned int)rv;
	  unsigned int gg = (unsigned int)gv;
	  unsigned int bb = (unsigned int)bv;
	  rr<<=16;
	  gg<<=8;
	  val=0xff000000 + rr+gg+bb;
	  break;
	  }
	case FrameBufferFormat::F_RGB888:
	  //std::cout << "F_RGB888 not supported\n" << std::endl;
	  {
	  unsigned int *b = ((unsigned int*)buffer) + y*width+x;
	  val = *b;
	  val |= 0xff000000;
	  break;
	  }
	case FrameBufferFormat::F_RGBA8888:
	  {
	  unsigned int *b = ((unsigned int*) buffer) + y*width + x;
	  val = *b;
#ifdef EMSCRIPTEN
	  unsigned int a = val&0xff000000;
	  unsigned int r = val&0x00ff0000;
	  unsigned int g = val&0x0000ff00;
	  unsigned int bb = val&0x000000ff;
	  //a>>=24;
	  r>>=16;
	  g>>=8;
	  
	  bb<<=16;
	  g<<=8;
	  //r<<=8;
	  val=a+r+g+bb;
#endif

	  break;
	  }
	};
#endif
	
	// PUTPIXEL
	unsigned int *target_pixel = (unsigned int*)(((unsigned char*) surf->pixels) + y*surf->pitch + x*sizeof(*target_pixel));
	*target_pixel = val;
      }
  SDL_UnlockSurface(surf);

}


int async_pending_count=0;
bool async_is_done=false;

  struct Event
  {
    int type;
    int ch;
    float cursor_pos_x;
    float cursor_pos_y;
    int button;
    int mouse_wheel_y;
    bool joy0_button0;
    bool joy0_button1;
    bool joy0_button2;
    bool joy0_button3;

    bool joy1_button0;
    bool joy1_button1;
    bool joy1_button2;
    bool joy1_button3;
    bool last;
  };

Event get_event()
{
  SDL_Event event;
  Event e2;
  int last = SDL_PollEvent(&event);
  e2.last = last!=0;
  //MainLoopPriv *p = (MainLoopPriv*)priv;
  //e2.current_time = p->current_time;
  //e2.delta_time = p->delta_time;
  int x,y;
  int mouse_wheel_y = 0;
  int val = SDL_GetMouseState(&x, &y);
  e2.type = event.type;
  e2.ch = event.key.keysym.sym;
  if (event.type==256) { exit(0); }
  
#ifndef __APPLE__
  if (event.type==SDL_FINGERMOTION||event.type==SDL_FINGERDOWN||event.type==SDL_FINGERUP)
    {
      SDL_TouchFingerEvent *ptr = &event.tfinger;
      float xx = ptr->x * get_screen_width();
      float yy = ptr->y * get_screen_height();
      if (!std::isnormal(xx)) xx=0.0f;
      if (!std::isnormal(yy)) yy=0.0f;
      x = int(xx);
      y = int(yy); 
    }
#endif

  if (event.type==SDL_MOUSEWHEEL)
    {
      SDL_MouseWheelEvent *ptr = &event.wheel;
      mouse_wheel_y = ptr->y;
    }

    
  //int id = 0;
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //if (env->cursor_pos_point_id.id==-1)
    // {
      // GameApi::PT pt = add_point(e, x,y);
      //   id = pt.id;
      //  }
  //else
    //  {
      //   id = env->cursor_pos_point_id.id;
      //  }
  //env->pt[id].x = x;
  //env->pt[id].y = y;
  //e2.cursor_pos.id = id;
  e2.cursor_pos_x = x;
  e2.cursor_pos_y = y;
  e2.button = -1;

#ifdef __APPLE__
  SDL_Keymod mod = SDL_GetModState();
  if ((val & SDL_BUTTON(1))&&(mod & KMOD_CTRL)) { e2.button = 2; } else
#endif  	  
  if (val & SDL_BUTTON(1)) { e2.button = 0; }
  if (val & SDL_BUTTON(2)) { e2.button = 1; }
  if (val & SDL_BUTTON(3)) { e2.button = 2; }
  SDL_Joystick *joy1 = SDL_JoystickOpen(0);
  SDL_JoystickEventState(SDL_ENABLE);
  unsigned int but1 = SDL_JoystickGetButton(joy1, 0);  
  unsigned int but2 = SDL_JoystickGetButton(joy1, 1);
  unsigned int but3 = SDL_JoystickGetButton(joy1, 2);
  unsigned int but4 = SDL_JoystickGetButton(joy1, 3);
  //std::cout << e.type << " " << e.ch << std::endl;
  e2.joy0_button0 = but1==1;
  e2.joy0_button1 = but2==1;
  e2.joy0_button2 = but3==1;
  e2.joy0_button3 = but4==1;
  e2.mouse_wheel_y = mouse_wheel_y;

  SDL_Joystick *joy2 = SDL_JoystickOpen(1);
  unsigned int a_but1 = SDL_JoystickGetButton(joy2, 0);  
  unsigned int a_but2 = SDL_JoystickGetButton(joy2, 1);
  unsigned int a_but3 = SDL_JoystickGetButton(joy2, 2);
  unsigned int a_but4 = SDL_JoystickGetButton(joy2, 3);
  //std::cout << a_but1 << " " << a_but2 << " " << a_but3 << " " << a_but4 << std::endl;
  e2.joy1_button0 = a_but1==1;
  e2.joy1_button1 = a_but2==1;
  e2.joy1_button2 = a_but3==1;
  e2.joy1_button3 = a_but4==1;

  return e2;
}


class FBU_run : public Splitter
{
public:
  FBU_run(GameApi::Env &env, GameApi::EveryApi &ev, FrameBuffer *buf, int mode, int scr_x, int scr_y) : env(env), ev(ev), buf(buf), scr_x(scr_x), scr_y(scr_y) { exit=false; firsttime = true; }
  virtual void Init() {
    surf = init_sdl_surface_framebuffer(scr_x, scr_y);
  }
  virtual Splitter* NextState(int code) { return 0; }
  virtual int Iter() {
    //std::cout << "Iter: " << async_pending_count << std::endl;
    if (async_pending_count>0 && !async_is_done) {
      return -1;
    } 
    async_is_done = true;
    if (firsttime) {
      buf->Prepare();
      firsttime = false;
    }
    //std::cout << "FBU_run::Iter" << std::endl;
    // TODO clear the screen
    //clear_sdl_surface(surf,scr_x,scr_y,0xffffff00);
    // TODO event loop.
    // 
    buf->frame();
    CopyFrameToSurface(buf, surf);
    SDL_UpdateWindowSurface(sdl_framebuffer_window);

    Event e;
    while((e = get_event()).last==true)
      {
    	if (e.ch==27 && e.type==0x300) { exit=true; }

	FrameLoopEvent event;
	event.type = e.type;
	event.ch = e.ch;
	event.cursor_pos.x = e.cursor_pos_x;
	event.cursor_pos.y = e.cursor_pos_y;
	event.button = e.button;
	//event.pin = 
	//get_iot_event(e,&event.pin[0]);
	buf->handle_event(event);
     }
#ifndef EMSCRIPTEN
    // this tries to make emscripten port and win32 build run the same speed.
#ifndef LINUX
    SDL_Delay(16);
#endif
#endif
    if (exit) return 0;
    return -1;
  }
  virtual void Destroy()
  {
    SDL_FreeSurface(surf);
    SDL_DestroyWindow(sdl_framebuffer_window);
    sdl_framebuffer_window = 0;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  FrameBuffer *buf;
  SDL_Surface *surf;
  int scr_x, scr_y;
  bool exit;
  bool firsttime;
};
#endif

Splitter *splitter_current = 0;
void splitter_iter2(void *arg)
{
  if (!arg) { std::cout << "FAIL: Splitter_iter2 NULL" << std::endl; return; }
  Splitter *blk2 = (Splitter*)arg;
  int blocker_exit_code = blk2->Iter();
  if (blocker_exit_code!=-1) 
    {
      Splitter *next = blk2->NextState(blocker_exit_code);
      if (next) {
	blk2->EnvTransfer(next);
      }
      blk2->Destroy();
      if (next) {
	next->Init();
      }
#ifdef EMSCRIPTEN
      // TODO, VR ISSUES
      emscripten_set_main_loop_arg(splitter_iter2, (void*)next, 0,1);
#else
      splitter_current = next;
#endif
    }
}


#if 0
QQmlEngine *create_qml_engine()
{
  return new QQmlEngine;
}
QQmlComponent *create_qml_component(QQmlEngine *engine, std::string qml_file_name)
{
  QQmlComponent *comp = new QQmlComponent(engine, QUrl::fromLocalFile(qml_file_name));
  QQmlContext *context = NULL;
  comp->create(context);
  return ;
}
#endif
#ifdef USE_SDL
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow )
#else
int main()
#endif
{
  int example_num = 0;
  std::string filename;
  switch(example_num) {
  case 0: filename="wearable.qml"; qml_path="./qml"; break;
  case 1: filename="gallery.qml"; qml_path="./qml2"; break;
  }
 
  QMLLoader load("root");
  std::vector<unsigned char> vec = load_file(filename,false);
  load.set_data(vec);

  QMLCreateNode *node = new QMLCreateNode(0,&load);
#ifndef USE_SDL
  node->Prepare();
#endif
  LowFrameBuffer *framebuf = new LowFrameBuffer(node,4,tp_screen_width,tp_screen_height,0); 

#ifdef USE_SDL
  GameApi::Env e;
  GameApi::EveryApi ev;
  FBU_run *run = new FBU_run(e,ev,framebuf, 0, tp_screen_width,tp_screen_height);
  Splitter *run2 = (Splitter*)run;
  run2->e = &e;
  run2->ev = &ev;
  run2->Init();
  splitter_current = run2;
  while(splitter_current) {
    splitter_iter2((void*)splitter_current);
  }
#endif

}

class RefWrapper : public QMLLoop
{
public:
  RefWrapper(QMLLoop *loop, std::string context, std::string type) : loop(loop), context(context), type(type) { }
  virtual QMLLoop *Parent() const { return 0; }
  virtual void PushChild(std::string name, QMLLoop *child) { 
    loop = child;
  }
  virtual std::string Name() const { return "RefWrapper"; }
  virtual void Prepare() {
    CommitRect(x,y,width,height);
    loop->Prepare();
    loop->set_vec(vec);
    std::cout << "RefWrapper:" << context << "." << type << std::endl;
    vec.print("RefWrapper");
  }
  virtual void handle_event(FrameLoopEvent &e) { 
    loop->handle_event(e);
  }
  virtual void frame(DrawLoopEnv &e) { 
    loop->frame(e);
  }  
  virtual std::pair<int,int> get_size() const
  {
    return loop->get_size();
  }
  virtual std::pair<int,int> get_pos() const
  {
    return loop->get_pos();
  }

  virtual void SetRect(int x_, int y_, int width_, int height_) { 
    std::cout << "refwrapper setrect" << std::endl;
    x=x_; y=y_; width=width_; height=height_;
  }
  virtual void CommitRect(int x, int y, int width, int heigth) { 
    std::cout << "refwrapper commitrect" << std::endl;
    loop->SetRect(x,y,width,height);
  }
  virtual int NumChildren() const { return 1; }
  virtual QMLLoop *Children(int i) const { return loop; }
  virtual std::string ChildName(int i) const { return "ref"; }
  virtual void set_vec(PVec vec) { loop->set_vec(vec); }
  virtual std::string fetch_value(std::string key) { return loop->fetch_value(key);  }

private:
  QMLLoop *loop;
  std::string context, type;
  //PVec vec;
  int x,y,width,height;
};

class ApplicationWindow : public QMLLoop
{
public:
  ApplicationWindow() {}
  virtual QMLLoop *Parent() const { return 0; }
  virtual void PushChild(std::string name, QMLLoop *child) { 
    children.push_back(child);
    childnames.push_back(name);
  }

  virtual std::string Name() const { return "ApplicationWindow"; }
  virtual void Prepare() {
    w = vec.get_width(this);
    h = vec.get_height(this);
    v = vec.is_visible(this);
    title = vec.get_title();
    CommitRect(0,0,w,h);
    PrepareChildren(); 
    std::cout << "Application window:" << std::endl;
    vec.print("ApplicationWindow");
  }
  virtual void handle_event(FrameLoopEvent &e) { 
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->handle_event(e);
  }
  virtual void frame(DrawLoopEnv &e) { 
    if (vec.is_visible(this)) {
      int bg = child_by_name("background");
      if (bg!=-1) children[bg]->frame(e);
      int hd = child_by_name("header");
      if (hd!=-1) children[hd]->frame(e);
      int ft = child_by_name("footer");
      if (ft!=-1) children[ft]->frame(e);
    }
  }
  virtual int NumChildren() const { return children.size(); }
  virtual QMLLoop *Children(int i) const { return children[i]; }
  virtual std::string ChildName(int child_i) const { return childnames[child_i]; }
  virtual std::pair<int,int> get_size() const
  {
    return std::make_pair(w,h); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    return std::make_pair(0,0);
  }

  virtual void SetRect(int x, int y, int width, int height) {
    std::cout << "application window setrect" << std::endl;
    w=width;
    h=height;
  }
  virtual void CommitRect(int x, int y, int width, int heigth) { 
    std::cout << "application window commitrect" << std::endl;
    int bg = child_by_name("background");
    if (bg!=-1) children[bg]->SetRect(0,0,w,h);
    int hd = child_by_name("header");
    if (hd!=-1) children[hd]->SetRect(0,0,w,h /*header_size*/);
    int ft = child_by_name("footer");
    if (ft!=-1) children[ft]->SetRect(0,h-footer_size,w,h/*footer_size*/);
  }
  virtual void set_vec(PVec vec) { 
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->set_vec(vec);

  }
  virtual std::string fetch_value(std::string key) { return "@"; }

private:
  void PrepareChildren()
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->Prepare();
  }
  int child_by_name(std::string name) const
  {
    int s = childnames.size();
    for(int i=0;i<s;i++) {
      if (childnames[i]==name) return i;
    }
    return -1;
  }
private:
  //PVec vec;
  std::vector<QMLLoop*> children;
  std::vector<std::string> childnames;
  int w,h;
  bool v;
  std::string title;
  int header_size = 30;
  int footer_size=30;
};

class Rectangle2 : public QMLLoop
{
public:
  Rectangle2(QMLLoop *parent, DrawBufferFormat fmt) : parent(parent), fmt(fmt), src(fmt,0) { }
  virtual QMLLoop *Parent() const { return parent; }
  virtual void PushChild(std::string name, QMLLoop *child) { }
  virtual std::string Name() const { return "Rectangle"; }
  virtual void Prepare()
  {
    std::string filename = vec.get_source();
    if (filename.size()<2) { filename=vec2.get_image_source(); }

    //edge = vec.get_edge();
    //int edge2 = vec2.get_edge();
    //if (edge==-1) edge=edge2;
    //if (edge==-1) edge=0;
    //std::cout << "Rectangle decides edge=" << edge << std::endl;

    sourceSize = vec.get_source_size();
    if (sourceSize.first==-1||sourceSize.second==-1) sourceSize=vec2.get_source_size();

    anchor_horizontal_center=vec.get_anchor_horizontal_center(this);
    anchor_top = vec.get_anchor_top(this);
    anchor_bottom = vec.get_anchor_bottom(this);


    std::cout << "Rectagle: anchor_horiz_center is " << anchor_horizontal_center << std::endl;
    std::cout << "Rectagle: anchor_top is " << anchor_top << std::endl;
    std::cout << "Rectagle: anchor_bottom is " << anchor_bottom << std::endl;
    //std::cout << "Image Decided filename: " << filename << std::endl;
    //std::vector<unsigned char> data = load_file(filename,true);
    int sx = vec.get_width(this);
    int sy = vec.get_height(this);
    int radius = vec.get_radius(this);
    std::cout << "Rectangle size: " << sx << "," << sy << ".." << radius << std::endl;
    if (sx==-1) sx=40;
    if (sy==-1) sy=40;
    BufferRef ref = BufferRef::NewBuffer(sx,sy);
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++) {
	unsigned int color;
	if (x<radius && y<radius) { // topleft
	  int cx = radius;
	  int cy = radius;
	  int xx = x-cx;
	  int yy = y-cy;
	  int r = sqrt(xx*xx+yy*yy);
	  if (r>radius) color = 0x00000000; else
	  color = 0xffffffff;
	} else
	if (x>sx-radius && y<radius) { // topright
	  int cx = sx-radius;
	  int cy = radius;
	  int xx = x-cx;
	  int yy = y-cy;
	  int r = sqrt(xx*xx+yy*yy);
	  if (r>radius) color = 0x00000000; else
	    color = 0xffffffff;
	} else
	if (x<radius && y>sy-radius) { // borromleft
	  int cx = radius;
	  int cy = sy-radius;
	  int xx = x-cx;
	  int yy = y-cy;
	  int r = sqrt(xx*xx+yy*yy);
	  if (r>radius) color = 0x00000000; else
	  color = 0xffffffff;
	} else
	if (x>sx-radius && y>sy-radius) { // borromright
	  int cx = sx-radius;
	  int cy = sy-radius;
	  int xx = x-cx;
	  int yy = y-cy;
	  int r = sqrt(xx*xx+yy*yy);
	  if (r>radius) color = 0x00000000; else
	  color = 0xffffffff;
	} else
	  color = 0xffffffff;
	ref.buffer[x+y*ref.ydelta]=color; // TODO color
      }

    width = ref.width;
    height = ref.height;
    if (sourceSize.first!=-1 && sourceSize.second!=-1)
      {
	int sx = sourceSize.first;
	int sy = sourceSize.second;
	BufferRef ref2 = scale_bitmap(ref,sx,sy);
	BufferRef::FreeBuffer(ref);
	ref = ref2;
	width=sx;
	height=sy;
      }
    BufferRefToSourceBitmap(ref, src, fmt);
    BufferRef::FreeBuffer(ref);


    if (anchor_horizontal_center != -1) 
      {
	x = anchor_horizontal_center - width/2;
      }
    if (anchor_top != -1)
      {
	y = anchor_top;
      }
    if (anchor_bottom != -1)
      {
	y = anchor_bottom - height;
      }

    //x=0;
    //y=0;
    //if (edge==0) { y=0; } else
    //  if (edge==1) { y=tp_screen_height-height; }


    std::cout << "Rectangle:" << std::endl;
    vec.print("Rectangle");
    vec2.print("Rectangle2");
  }
  virtual void handle_event(FrameLoopEvent &e) 
  {
  }
  virtual void frame(DrawLoopEnv &e) 
  {
    //std::cout << "Rectangle frame " << x << " " << y << " " << width << " " << height << std::endl;

    e.drawbuffer->draw_sprite(&src, x, y );
  }
  virtual std::pair<int,int> get_size() const
  {
    return std::make_pair(width,height); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    return std::make_pair(x,y);
  }

  virtual void SetRect(int x_, int y_, int width_, int height_) { 
    std::cout << "rectangle setrect" << x_ << " " << y_ << " " << width_ << " " << height_ << std::endl;
    x=x_;
    y=y_;
    width=width_;
    height=height_;
  }
  virtual void CommitRect(int x, int y, int width, int heigth)
    {
    }
  virtual int NumChildren() const { return 0; }
  virtual QMLLoop *Children(int i) const { return 0; }
  virtual std::string ChildName(int i) const { return ""; }
  virtual void set_vec(PVec v)
  {
    int s = v.p1.size();
    for(int i=0;i<s;i++)
      {
	PushAttr(v.p1[i],v.p2[i]);
      }

    vec2 = v;
    Prepare();
    //std::cout << "Image(updating):" << std::endl;
    //v.print();
  }  
  virtual std::string fetch_value(std::string key) {
    if (key=="width") return to_string(width);
    if (key=="height") return to_string(height);
    if (key=="property int edge") return to_string(edge);
    //if (key=="edge") return to_string(edge);
    return "@";
  }
private:
  QMLLoop *parent;
  //PVec vec;
  PVec vec2;
  DrawBufferFormat fmt;
  SourceBitmap src;
  int x,y;
  int width,height;
  int edge;
  int anchor_horizontal_center;
  int anchor_top;
  int anchor_bottom;
  std::pair<int,int> sourceSize;
};


class AbstractButton : public QMLLoop
{
public:
  AbstractButton(QMLLoop *parent, DrawBufferFormat fmt) : fmt(fmt), src(fmt,0), parent(parent) { 
    x=0; 
    y=0;
    width=0;
    height=0;
  }
  virtual QMLLoop *Parent() const { return parent; }
  virtual void PushChild(std::string name, QMLLoop *child) { 
    m_child.push_back(child);
    childnames.push_back(name);
  }

  virtual std::string Name() const { return "AbstractButton"; }
  virtual void Prepare()
  {
    std::string filename = vec.get_source();
    if (filename.size()<2) { filename=vec2.get_image_source(); }

    edge = vec.get_edge();
    int edge2 = vec2.get_edge();
    if (edge==-1) edge=edge2;


    sourceSize = vec.get_source_size();
    if (sourceSize.first==-1||sourceSize.second==-1) sourceSize=vec2.get_source_size();

    //std::cout << "Image Decided filename: " << filename << std::endl;
    //std::vector<unsigned char> data = load_file(filename,true);
    int sx = vec.get_width(this);
    int sy = vec.get_height(this);
    std::cout << "AbstractButton size: " << sx << "," << sy << std::endl;

    
#if 1
    if (sx==-1) {
      std::pair<int,int> p = std::make_pair(-1,-1);
      int ci = child_by_name("contentItem");
      if (ci!=-1) p = m_child[ci]->get_size();
      sx=p.first;
    }
    if (sy==-1) {
      std::pair<int,int> p = std::make_pair(-1,-1);
      int ci = child_by_name("contentItem");
      if (ci!=-1) p = m_child[ci]->get_size();
      sy=p.second;
    }
#endif
    if (sx==-1) sx=1;
    if (sy==-1) sy=1;

    x=m_w/2-sx/2; 
    y=m_h/2-sy/2;
    if (edge==0) { y=0; } else
      if (edge==1) { 
	
	y=m_h -sy; 
      }

#if 0
    // does require "enabled" of stackview
    int xx = vec.get_x(this);
    int yy = vec.get_y(this);
    if (xx!=-1) { x=xx; }
    if (yy!=-1) { y=yy; }
#endif

    width = sx;
    height = sy;
    std::cout << "AbstractButton " << x << " " << y << " " << width << " " << height << std::endl;

    CommitRect(x,y,width,height);
    int s = m_child.size();
    for(int i=0;i<s;i++)
      {
	m_child[i]->Prepare();
      }

    std::cout << "AbstractButton size(defaulted to): " << sx << "," << sy << std::endl;

    BufferRef ref = BufferRef::NewBuffer(sx,sy);
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	ref.buffer[x+y*ref.ydelta]=0xffffffff; // TODO color

    if (sourceSize.first!=-1 && sourceSize.second!=-1)
      {
	int sx = sourceSize.first;
	int sy = sourceSize.second;
	BufferRef ref2 = scale_bitmap(ref,sx,sy);
	BufferRef::FreeBuffer(ref);
	ref = ref2;
	width=sx;
	height=sy;
      }
    //x=0; 
    //y=0;
    //if (edge==0) { y=0; } else
    //  if (edge==1) { y=tp_screen_height-height; }

    BufferRefToSourceBitmap(ref, src, fmt);
    BufferRef::FreeBuffer(ref);


    std::cout << "AbstractButton:" << std::endl;
    vec.print("AbstractButton");
    vec2.print("AbstractButton2");
  }
  virtual void handle_event(FrameLoopEvent &e) 
  {
    Point p = e.cursor_pos;
    if (e.button == 0 && p.x>x && p.x<x+width && p.y>y && p.y<y+height)
      { // clicked
	vec2.get_onClicked(this);
      }

    if (vec2.get_enabled(this)) {
      int s = m_child.size();
      for(int i=0;i<s;i++)
	{
	  m_child[i]->handle_event(e);
	}
    }
  }
  virtual void frame(DrawLoopEnv &e) 
  {
    if (vec2.get_enabled(this)) {

      int bg = child_by_name("background");
      if (bg!=-1) m_child[bg]->frame(e);
      
      //std::cout << "AbstractButton frame " << x << " " << y << " " << width << " " << height << std::endl;

      e.drawbuffer->draw_sprite(&src, x, y );
      
      int ci = child_by_name("contentItem");
      if (ci!=-1) m_child[ci]->frame(e);
    }
    //int s = m_child.size();
    //for(int i=0;i<s;i++)
    //  {
    //	m_child[i]->frame(e);
    //  }


  }
  virtual int NumChildren() const { return m_child.size(); }
  virtual QMLLoop *Children(int i) const { return m_child[i]; }
  virtual std::string ChildName(int child_i) const { return childnames[child_i]; }

  virtual void set_vec(PVec v)
  {
    int s = m_child.size();
    for(int i=0;i<s;i++)
      m_child[i]->set_vec(v); 

    vec2 = v;
    Prepare();

    //vec2 = v;
    //Prepare();
    //std::cout << "Image(updating):" << std::endl;
    //v.print();
  }  
  virtual std::string fetch_value(std::string key) {
    if (key=="width") return to_string(width);
    if (key=="height") return to_string(height);
    if (key=="top") return to_string(y);
    if (key=="bottom") return to_string(y+height);
    if (key=="left") return to_string(x);
    if (key=="right") return to_string(x+width);
    if (key=="horizontalCenter") return to_string(m_w/2);
    if (key=="verticalCenter") return to_string(m_h/2);
    if (key=="property int edge") {
      int edge2 = vec2.get_edge();
      return to_string(edge2);
    }
    if (key=="edge") return to_string(edge);
    return "@";
  }
  int child_by_name(std::string name) const
  {
    int s = childnames.size();
    for(int i=0;i<s;i++) {
      if (childnames[i]==name) return i;
    }
    return -1;
  }
  virtual std::pair<int,int> get_size() const
  {
    return std::make_pair(width,height); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    return std::make_pair(x,y);
  }

  virtual void SetRect(int x_, int y_, int width_, int height_) { 
    std::cout << "abstractbutton setrect " << x_ << " " << y_ << " " << width_ << " " << height_ << std::endl;
    x=x_;
    y=y_;
    width=width_;
    height=height_;
    m_w = width;
    m_h = height;
  }
  virtual void CommitRect(int x, int y, int width, int heigth) { 
    std::cout << "abstractbutton commitrect" << x << " " << y << " " << width << " " << height << std::endl;
    int bg = child_by_name("background");
    if (bg!=-1) m_child[bg]->SetRect(x,y,width,height);

    int ci = child_by_name("contentItem");
    if (ci!=-1) m_child[ci]->SetRect(x+border,y+border,width-border*2,height-border*2);

  }

private:
  QMLLoop *parent;
  //PVec vec;
  PVec vec2;
  DrawBufferFormat fmt;
  SourceBitmap src;
  int x,y;
  int width,height;
  int m_w,m_h;
  int edge;
  int border=0;
  std::pair<int,int> sourceSize;
  std::vector<QMLLoop*> m_child;
  std::vector<std::string> childnames;
};


class Image : public QMLLoop
{
public:
  Image(QMLLoop *parent, DrawBufferFormat fmt) : parent(parent), fmt(fmt),src(fmt,0) {
    width=1;
    height=1;
  }
  virtual void PushChild(std::string name, QMLLoop *child) { }
  virtual QMLLoop *Parent() const { return parent; }
  virtual std::string Name() const { return "Image"; }
  virtual void Prepare()
  {
    filename = vec.get_source();
    if (filename.size()<2) { filename=vec2.get_image_source(); }
    std::cout << "Image decides filename: " << filename << std::endl;

    //edge = vec.get_edge();
    //int edge2 = vec2.get_edge();
    //if (edge==-1) edge=edge2;
    //if (edge==-1) edge=0;
    //std::cout << "Image decides edge: " << edge << std::endl;

    sourceSize = vec.get_source_size();
    if (sourceSize.first==-1||sourceSize.second==-1) sourceSize=vec2.get_source_size();
    std::cout << "Image decides sourceSize: " << sourceSize.first << "," << sourceSize.second << std::endl;

    //std::cout << "Image Decided filename: " << filename << std::endl;
    std::vector<unsigned char> data = load_file(filename,true);
    BufferRef ref = decode_image_file(data);
    //width = ref.width;
    //height = ref.height;

    int sx = width;
    int sy = height;
    if (sourceSize.first!=-1) sx=sourceSize.first;
    if (sourceSize.second!=-1) sy=sourceSize.second;
    BufferRef ref2 = scale_bitmap(ref,sx,sy);
    BufferRef::FreeBuffer(ref);
    ref = ref2;
    width=sx;
    height=sy;
    std::cout << "Image decides (width/height): " << width << "," << height << std::endl;

    BufferRefToSourceBitmap(ref, src, fmt);
    BufferRef::FreeBuffer(ref);

    //x=0; y=0;
    //if (edge==0) { y=0; } else
    // if (edge==1) { y=tp_screen_height-height; }

    std::cout << "Image decides x,y: " << x << " " << y << std::endl;

    std::cout << "Image:" << std::endl;
    vec.print("Image");
    vec2.print("Image2");
  }
  virtual void handle_event(FrameLoopEvent &e) 
  {
  }
  virtual void frame(DrawLoopEnv &e) 
  {
    //std::cout << "Image frame " << filename << "::" << x << " " << y << " " << width << " " << height << std::endl;
    e.drawbuffer->draw_sprite(&src, x, y );
  }
  virtual std::pair<int,int> get_size() const
  {
    return std::make_pair(width,height); 
  }
  virtual std::pair<int,int> get_pos() const
  {
    return std::make_pair(x,y);
  }

  virtual void SetRect(int x_, int y_, int width_, int height_)
  {
    std::cout << "Image SetRect " << x_ << " " << y_ << " " << width_ << " " << height_ << std::endl;
    x=x_;
    y=y_;
    width=width_;
    height=height_;
  }
  virtual void CommitRect(int x_, int y_, int width_, int heigth_)
    {
    }
  virtual int NumChildren() const { return 0; }
  virtual QMLLoop *Children(int i) const { return 0; }
  virtual std::string ChildName(int i) const { return ""; }
  virtual void set_vec(PVec v)
  {
    std::cout << "Image set_vec" << std::endl;
    vec2 = v;
    Prepare();
    //std::cout << "Image(updating):" << std::endl;
    //v.print();
  }  
  virtual std::string fetch_value(std::string key)
  {
    if (key=="width") return to_string(width);
    if (key=="height") return to_string(height);
    if (key=="property int edge") return to_string(edge);
    //if (key=="edge") return to_string(edge);
    return "@";
  }
private:
  QMLLoop *parent;
  std::string filename;
  //PVec vec;
  PVec vec2;
  DrawBufferFormat fmt;
  SourceBitmap src;
  int x,y;
  int width,height;
  int edge;
  std::pair<int,int> sourceSize;
};

struct Rect {
  int x,y,w,h;
};

Rect remove_rect_from_rect(Rect orig, Rect cut, bool &success)
{
  bool success_x = false;

  // X-DIRECTION
  int start_x = orig.x;
  int end_x = orig.x+orig.w;
  int cut_start_x = cut.x;
  int cut_end_x = cut.x+cut.w;

  int new_start_x;
  int new_end_x;
  
  bool start_same_x = start_x==cut_start_x;
  bool end_same_x = end_x==cut_end_x;
  if (start_same_x && !end_same_x) { // leave right
    new_start_x = cut_end_x;
    new_end_x = end_x;
    success_x=true;
  }
  if (!start_same_x && end_same_x) { // leave left
    new_start_x = start_x;
    new_end_x = cut_start_x;
    success_x=true;
  }
  if (!start_same_x && !end_same_x) { // error, two parts
    new_start_x = start_x;
    new_end_x = end_x;

    success_x = true;
  }
  if (start_same_x && end_same_x) { // preserve x-direction
    new_start_x = start_x;
    new_end_x = end_x;
    success_x=true;
  }

  // Y-DIRECTION

  bool success_y = false;

  int start_y = orig.y;
  int end_y = orig.y+orig.h;
  int cut_start_y = cut.y;
  int cut_end_y = cut.y+cut.h;

  int new_start_y;
  int new_end_y;
  
  bool start_same_y = start_y==cut_start_y;
  bool end_same_y = end_y==cut_end_y;
  if (start_same_y && !end_same_y) { // leave right
    new_start_y = cut_end_y;
    new_end_y = end_y;
    success_y=true;
  }
  if (!start_same_y && end_same_y) { // leave left
    new_start_y = start_y;
    new_end_y = cut_start_y;
    success_y=true;
  }
  if (!start_same_y && !end_same_y) { // error, two parts, preserve y-dir
    new_start_y = start_y;
    new_end_y = end_y;

    success_y = true;
  }
  if (start_same_y && end_same_y) { // preserve x-direction
    new_start_y = start_y;
    new_end_y = end_y;
    success_y=true;
  }

  // combine results

  success=success_x && success_y;
  Rect r;
  r.x = new_start_x;
  r.y = new_start_y;
  r.w = new_end_x-new_start_x;
  r.h = new_end_y-new_start_y;
  return r;
}
Rect to_rect(int x, int y, int width, int height) {
  Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;
  return rect;
}

Rect to_rect(std::pair<int,int> tl, std::pair<int,int> sz)
{
  Rect rect;
  rect.x = tl.first;
  rect.y = tl.second;
  rect.w = sz.first;
  rect.h = sz.second;
  return rect;
}
std::ostream & operator<<(std::ostream &o, Rect r)
{
  o << "(" << r.x << "," << r.y << ": " << r.w << "," << r.h << ")"; 
  return o;
}

Rect anchors_fill_parent(QMLLoop *current, bool &res_success)
{
  QMLLoop *parent = current->Parent();
  std::cout << "anchors_fill parent name: " << parent->Name() << std::endl;
  std::cout << "anchors_fill current name: " << current->Name() << std::endl;

  Rect parentrect;
  std::pair<int,int> tl = parent->get_pos();
  std::pair<int,int> sz = parent->get_size();
  parentrect = to_rect(tl,sz);
  
  std::cout << "anchors_fill parent rect: "<< parentrect << std::endl;

  int s = parent->NumChildren();
  bool success = true;
  for(int i=0;i<s;i++) {
    QMLLoop *loop = parent->Children(i);
    std::cout << "anchors_fill loop name: " << loop->Name() << std::endl;
    std::cout << "anchors_fill compare " << loop << " " << current << std::endl;
    if (loop==current) continue;
    std::pair<int,int> c_tl = loop->get_pos();
    std::pair<int,int> c_sz = loop->get_size();
    Rect r2 = to_rect(c_tl, c_sz);
    bool success2 = true;
    parentrect = remove_rect_from_rect(parentrect, r2, success2);
    std::cout << "anchors_fill: " << r2 << " " << parentrect << std::endl;
    if (!success2) { res_success=false; return parentrect; }
  }
  res_success=true;
  std::cout << "anchors_fill result: " << parentrect << std::endl;
  return parentrect;
}


class StackView : public QMLLoop
{
public:
  StackView(QMLLoop *parent) : parent(parent) { }
  virtual QMLLoop *Parent() const { return parent; }
  virtual std::string Name() const { return "StackView"; }
  virtual void PushChild(std::string name, QMLLoop *child)
  {
    children.push_back(child);
    childnames.push_back(name);
  }
  virtual int NumChildren() const { return children.size(); }
  virtual QMLLoop *Children(int i) const { return children[i]; }
  virtual std::string ChildName(int i) const { return childnames[i]; }
  virtual std::pair<int,int> get_size() const
  {
    return std::make_pair(current_rect.w,current_rect.h);
  }
  virtual std::pair<int,int> get_pos() const
  {
    return std::make_pair(current_rect.x,current_rect.y);
  }
  virtual void SetRect(int x, int y, int width, int height)
  {
    current_rect = to_rect(x,y,width,height);
  }
  virtual void CommitRect(int x, int y, int width, int heigth)
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      {
	children[i]->SetRect(current_rect.x,current_rect.y,current_rect.w,current_rect.h);
      }
  }
  virtual void set_vec(PVec vec) { }
  virtual std::string fetch_value(std::string key)
  {
    if (key=="depth") return to_string(children.size());
    if (key=="pop(null)") {
      std::cout << "StackView::pop(null)" << std::endl;
      if (children.size()>0)
	{
	  children.erase(children.begin());
	  childnames.erase(childnames.begin());
	}
      //children.pop_front();
      //childnames.pop_front();
    }
    if (key=="pop()") {
      std::cout << "StackView::pop()" << std::endl;
      children.pop_back();
      childnames.pop_back();
    }
    return "@";
  }
  
  virtual void Prepare()
  {
    // get_initial rectangle
    QMLLoop *parent = Parent();
    std::pair<int,int> tl = parent->get_pos();
    std::pair<int,int> sz = parent->get_size();
    current_rect = to_rect(tl,sz);

    // anchors.fill
    std::string s = vec.get_val("anchors.fill");
    if (s=="parent") {
      bool success = false;
      Rect r = anchors_fill_parent(this, success);
      if (success) {
	current_rect = r;
      } else { 
	std::cout << "anchors.fill=parent failed to calculate rects" << std::endl;
      }
    }

    CommitRect(current_rect.x, current_rect.y, current_rect.w, current_rect.h);
    std::cout << "StackView child count=" << children.size() << std::endl;
    int s2 = children.size();
    for(int i=0;i<s2;i++)
      {
	children[i]->Prepare();
      }
    
  }
  virtual void handle_event(FrameLoopEvent &e) {
    if (children.size()>0) {
      children[0]->handle_event(e);
    }
  }
  virtual void frame(DrawLoopEnv &e)
  {
    if (children.size()>0) {
      children[0]->frame(e);
    }
  }
private:
  QMLLoop *parent;
  std::vector<QMLLoop*> children;
  std::vector<std::string> childnames;
  Rect current_rect;
};



std::string parse_import(std::string line)
{
  std::stringstream ss(line);
  std::string import_word;
  std::string path;
  ss >> import_word;
  ss >> path;
  return strip_spaces(path);
}

struct DBItem
{
  QMLLoop *loop;
  PVec vec;
  std::vector<QMLLoop*> child;
  std::vector<DBItem*> child_dbs;
};

std::vector<DBItem> g_db; 

void global_db(QMLLoop *loop, PVec vec, std::vector<QMLLoop*> child)
{
  int s = child.size();
  std::vector<DBItem*> db_items;
  for(int i=0;i<s;i++) {
    int sk = g_db.size();
    QMLLoop *item = child[i];
    for(int k=0;k<sk;k++) {
      if (item==g_db[k].loop) {
	db_items.push_back(&g_db[k]);
	break;
      }
    }
  }
  if (db_items.size()!=child.size()) {
    std::cout << "global_db:: not all childs found " << db_items.size() << "<" << child.size()  << std::endl;
  }
  DBItem i;
  i.loop = loop;
  i.vec = vec;
  i.child = child;
  i.child_dbs = db_items;
  g_db.push_back(i);

}

QMLLoop *find_by_id(QMLLoop *context, std::string id, std::string key="id", int depth=0)
{
  if (depth>3) return 0;
  if (context->GetAttr(key)==id) { return context; }
  QMLLoop *parent = context->Parent();
  if (!parent) {
    std::cout << "find_by_id failed on " << id << " at " << context->Name() << " because parent=0" << std::endl;
    return 0;
  }
  if (parent->GetAttr(key)==id) return parent;

  int s = parent->NumChildren();
  for(int i=0;i<s;i++)
    {
      QMLLoop *child = parent->Children(i);
      if (child->GetAttr(key)==id) return child;
    }
  return find_by_id(parent, id, key, depth+1);
  std::cout << "find_by_id failed on " << id << " at " << context->Name() << " via " << parent->Name() << " because children not found" << std::endl;
  return 0;


#if 0
  int s = g_db.size();
  // check if context is in children
  int found = -1;
  for(int i=0;i<s;i++)
    {
      DBItem *item = &g_db[i];
      int s2 = item->child.size();
      for(int k=0;k<s2;k++) {
	if (context==item->child[k]) {
	  found = i;
	}
      }
    }
  if (found==-1) {
    std::cout << "Searching not found " << id << std::endl;
    return 0;
  }



  std::cout << "Searching: " << id << ":::" << found << ":";
  DBItem *res = 0;
  DBItem *parent = &g_db[found];
  bool has_found = false;
  int sk = parent->child_dbs.size();
  for(int k=0;k<sk;k++) {
    DBItem *child_db = parent->child_dbs[k];
    std::cout << child_db->loop->Name() << ":" << std::endl;
    std::string m_id = child_db->vec.get_val("id");
    std::cout << m_id << ",";
    if (m_id == id) {
      std::cout << "Found!" << std::endl;
      has_found=true;
      res = child_db;
    }
  }
  if (!has_found) { 
    return 0;
    //if (!parent->loop) return 0;
    //return find_by_id(parent->loop, id); 
  }
  return res;
#endif
}

bool is_number(std::string s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]=='-') continue;
      if (s[i]>='0' && s[i]<='9') continue;
      return false;
    }
  return true;
}

int int_expr_parser(QMLLoop *loop, const PVec &vec, std::string value)
{
  //std::cout << "Parsing: " << value << std::endl;
  // remove spaces from beginning&end
  value = strip_spaces(value);
  
  // remove comments
  value = remove_comment(value);

  // check for error cases.
  if (value.size()>0 && value[0]=='@') {
    //std::cout << "Expr: @" << std::endl;
    return -1;
  }

  // constants
  int k = constants(value);
  if (k!=-1) {
    //std::cout << "Expr: constant " << value << ":=" << k << std::endl;
    return k;
  }

  if (value=="true") { return 1; }
  if (value=="false") { return 0; }

  // number
  if (is_number(value)) {
    std::stringstream ss(value);
    int val = -1;
    //std::cout << "Expr: number: " << value << std::endl;
    if (!(ss >> val)) {
      //std::cout << "Expr result: " << -1 << std::endl;
      return -1;
    }
    //std::cout << "Expr result: " << val << std::endl;
    return val;
  }


  
  // expr ? value1 : value2
  int pos_a = find_ch(value, '?');
  int pos_b = find_ch(value, ':');
  if (pos_a!=-1 && pos_b!=-1 && pos_a<pos_b) {
    std::string first = value.substr(0,pos_a-1);
    std::string second = value.substr(pos_a+1,pos_b-pos_a-1);
    std::string third = value.substr(pos_b+1,value.size()-pos_b-1);
    int val = int_expr_parser(loop,vec, first);
    if (val) {
      int val2 = int_expr_parser(loop,vec,second);
      //std::cout << "Expr: ?: " << value << std::endl;
      //std::cout << "Expr result: " << val2 << std::endl;
      return val2;
    } else {
      int val2 = int_expr_parser(loop,vec,third);
      //std::cout << "Expr: ?: " << value << std::endl;
      //std::cout << "Expr result: " << val2 << std::endl;
      return val2;
    }
  }
  
  int pos_h = find_ch(value,'>');
  if (pos_h!=-1)
    {
      std::string first = value.substr(0,pos_h-1);
      std::string second = value.substr(pos_h+1,value.size()-pos_h-1);
      int val = int_expr_parser(loop,vec, first);
      int val2 = int_expr_parser(loop,vec, second);
      if (val>val2) {
	return 1;
      }
      return 0;
    }
  // a === b
  int pos_k = find_ch(value,'=');
  if (pos_k!=-1 && pos_k+2<value.size() && value[pos_k+1]=='=' && value[pos_k+2]=='=')
    {
    std::string first = value.substr(0,pos_k-1);
    std::string second = value.substr(pos_k+3,value.size()-pos_k-3);      
    int val = int_expr_parser(loop,vec, first);
    int val2 = int_expr_parser(loop,vec, second);
    //std::cout << "Expr: ("<< first << ") === (" << second << ")"  << std::endl;

    if (val==val2) { 
      //std::cout << "Expr result: " << 1 << std::endl;

      return 1;
    }
    //std::cout << "Expr result: " << 0 << std::endl;
    return 0;
    }

  // addition
  int pos = find_ch(value,'+');
  if (pos!=-1) {
    std::string first = value.substr(0,pos-1);
    std::string second = value.substr(pos+1,value.size()-pos-1);
    //std::cout << "Expr: ("<< first << ") + (" << second << ")"  << std::endl;
    int val1 = int_expr_parser(loop,vec,first);
    int val2 = int_expr_parser(loop,vec,second);
    if (val1==-1) return -1;
    if (val2==-1) return -1;
    //std::cout << "Expr result: " << val1+val2 << std::endl;
    return val1+val2;
  }

  // minus
  int pos1 = find_ch(value,'-');
  if (pos1!=-1) {
    std::string first = value.substr(0,pos1-1);
    std::string second = value.substr(pos+1,value.size()-pos1-1);
    if (first.size()>0) {
    //std::cout << "Expr: ("<< first << ") - (" << second << ")"  << std::endl;
      int val1 = int_expr_parser(loop,vec,first);
      int val2 = int_expr_parser(loop,vec,second);
      if (val1==-1) return -1;
      if (val2==-1) return -1;
    //std::cout << "Expr result: " << val1-val2 << std::endl;

      return val1 - val2;
    }
  }


  // multiplication
  int pos2 = find_ch(value,'*');
  if (pos2!=-1) {
    std::string first = value.substr(0,pos2-1);
    std::string second = value.substr(pos2+1,value.size()-pos2-1);
    //std::cout << "Expr: ("<< first << ") * (" << second << ")"  << std::endl;
    int val1 = int_expr_parser(loop,vec,first);
    int val2 = int_expr_parser(loop,vec,second);
    if (val1==-1) {
      //std::cout << "Expr result: " << -1 << std::endl;

      return -1;
    }
    if (val2==-1) {
      //std::cout << "Expr result: " << -1 << std::endl;
      return -1;
    }
    //std::cout << "Expr result: " << val1*val2 << std::endl;
    return val1 * val2;
  }

  // division
  int pos3 = find_ch(value,'/');
  if (pos3!=-1) {
    std::string first = value.substr(0,pos3-1);
    std::string second = value.substr(pos3+1,value.size()-pos3-1);
    //std::cout << "Expr: ("<< first << ") / (" << second << ")"  << std::endl;
    int val1 = int_expr_parser(loop,vec,first);
    int val2 = int_expr_parser(loop,vec,second);
    if (val1==-1) {
      //std::cout << "Expr result: " << -1 << std::endl;
      return -1;
    }
    if (val2==-1) {
      //std::cout << "Expr result: " << -1 << std::endl;
      return -1;
    }
    //std::cout << "Expr result: " << val1/val2 << std::endl;
    return val1 / val2;
  }

  // id.attr
  int pos4 = find_ch(value,'.');
  if (pos4!=-1) {
    std::string first = value.substr(0,pos4);
    std::string second = value.substr(pos4+1,value.size()-pos4-1);

    //std::cout << "Expr: (" << first << ").(" << second << ")"<< std::endl;

    QMLLoop *loop2 = find_by_id(loop,first);
    if (loop2) {
      // ask the element if it has some values
      //QMLLoop *loop = db->loop;
      //std::cout << "id " << first << " found " << loop2->Name() << std::endl;

      std::string value = loop2->fetch_value(second);
      if (value!="@") { 
	//std::cout << "Expr: ." << second << " found: " << value << std::endl;
	int val = int_expr_parser(loop2,vec,value);
	//std::cout << "Expr result: " << val << std::endl;
	  return val;
      }

      // if not, try attributes
      PVec v = loop2->GetAttr2();
      std::string value2 = v.get_val(second);
      //std::cout << "Expr: attr value: " << value2 << std::endl;
      if (value2!="@") {
	int val = int_expr_parser(loop2,v,value2);
	//std::cout << "Expr result: " << val << std::endl;
	return val;
      }
      //std::cout << "Expr result: " << -1 << std::endl;
      return -1;
    } else {
      //std::cout << "Expr . failed" << std::endl;
    }
  }

  // attribute name
  std::string val = vec.get_val(value);
  //int val = vec.get_int_val(value);
  if (val!="@") { 
    //std::cout << "Expr: attrib name: " << value << "::" << val << std::endl;
    //std::cout << "Expr result: " << val << std::endl;
    return int_expr_parser(loop,vec,val); 
  }

  // ask component
  if (loop) {
    std::string val = loop->fetch_value(value);
    if (val!="@") {
      //std::cout << "Expr: component " << loop->Name() << " decides: " << val << std::endl;
      return int_expr_parser(loop, vec, val);
    }
  }

  //std::cout << "Expr(in " << loop->Name() << ") not parsed: " << value << std::endl;
  //std::cout << "Expr result: " << -1 << std::endl;
  return -1;

}

QMLLoop *create_qml_element(QMLLoop *parent, QMLData *root,QMLData *dt)
{
  QMLData *root2 = root;
  if (dt->NumImports()>0) root2=dt;

  //std::cout << "create_qml_element" << std::endl;
  std::string type = dt->Type();
  QMLLoop *current = 0;

  // find context and type2
  int s = type.size();
  int pos = -1;
  for(int i=0;i<s;i++) {
    if (type[i]=='.') pos=i;
  }
  std::string context;
  std::string type2=type;
  if (pos!=-1) {
    context = type.substr(0,pos);
    type2 = type.substr(pos+1,type.size()-1-1);
  }

  std::cout << "create_qml_element " << context << "." << type2 << "." << std::endl;
  // BUILTIN COMPONENTS
  if (type2=="ApplicationWindow") {
    current = new ApplicationWindow;
  }
  if (type2=="Image") {
    current = new Image(parent,D_RGBA8888);
  }
  if (type2=="Rectangle") {
    current = new Rectangle2(parent,D_RGBA8888);
   }
  if (type2=="AbstractButton") {
    current = new AbstractButton(parent,D_RGBA8888);
  }
  if (type2=="StackView") {
    current = new StackView(parent);
  }
  
  if (!current) {
    // LOADING QML FILES
    std::string qmlfilename = type2 + ".qml";
    int st = root2->NumImports();
    QMLLoader *load = 0;
    std::string qmlpath="";
    for(int i=0;i<st;i++)
      {
	qmlpath = root2->Import(i);
      //std::cout << "QMLPATH: " << qmlpath << std::endl;
	qmlpath = parse_import(qmlpath);
	//std::cout << "QMLPATH2: " << qmlpath << std::endl;
	std::vector<unsigned char> contents = load_file(qmlpath+"/"+qmlfilename, false);
	if (contents.size()>0) {
	  load = new QMLLoader(type2);
	  load->set_data(contents);
	  //load->Prepare();
	  break;
	}
      }
    if (load) {
      std::string old_path = qml_path;
      qml_path=qml_path+"/"+qmlpath;
      QMLLoop *loop = new QMLCreateNode(parent, load);
      //QMLLoop *loop = create_qml_element(root2,load);
      qml_path=old_path;
      QMLLoop *loop2 = new RefWrapper(loop, context, type2);
      current = loop2;
    } else { current=0; }
  }
  if (!current) { current = new UnknownWithChildren(parent,context,type2); }
  
  // collect attributes
  PVec p_vec;
  QMLData *c = dt;

  int sk = c->NumAttribs();
  for(int i=0;i<sk;i++)
    {
      std::string attr_name = c->AttrName(i);
      std::string attr_value = c->AttrValue(i);
      current->PushAttr(attr_name, attr_value);
    }
  // end collect attribs

  // create children
  std::vector<QMLLoop*> vec;
  int sl = c->NumChildren();
  for(int i=0;i<sl;i++)
    {
      QMLLoop *loop = create_qml_element(current, root2,c->Children(i));
      std::string name = c->Name(i);
      //vec.push_back(loop);
      //vec_names.push_back(name);
      current->PushChild(name,loop);
    }

  //std::vector<std::string> vec_names;
  //int s2 = c->NumChildren();
  //for(int i=0;i<s2;i++)
  //  {
  //  }
  

  //std::cout << "Type chosen: " << context << "." << type2 << std::endl;
  //if (type2=="NaviButton") {
  //  return new UnknownWithChildren(context,type2,p_vec,vec);
  //}
  //if (type2=="StackView") {
  //  return new UnknownWithChildren(context,type2,p_vec,vec);
  //}
  //QMLLoop *loop = new UnknownWithChildren(context,type2,p_vec,vec,vec_names);
  //global_db(loop, p_vec, vec);
  //return loop;
  return current;
}


