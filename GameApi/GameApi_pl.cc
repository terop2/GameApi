
#include "GameApi_h.hh"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "GameApi_low.hh"
#include "GameApi_cmd.hh"
#include <atomic>
#include <iostream>
#include <queue>

#ifdef LINUX
#define USE_VIDEO 1
#endif

// NOT WORKING
//#ifdef EMSCRIPTEN
//#define USE_VIDEO 1
//#endif

bool GameApi::PolygonApi::ready_to_prepare(ML p)
{
  MainLoopItem *item = find_main_loop(e,p);
  return item->ReadyToPrepare();
}

GameApi::P resize_to_correct_size2(GameApi::Env &e, GameApi::P model, Matrix *mat);
extern Matrix g_last_resize;

template<class T>
void print(std::string label, T *ptr)
{
  int s = 4;
  std::cout << label << ":";
  for(int i=0;i<s;i++) std::cout << ptr[i];
  std::cout << std::endl;
}


#define NO_MV 1

IMPORT void InstallProgress(int num, std::string label, int max=15);
IMPORT void ProgressBar(int num, int val, int max, std::string label);
void SetupProgress(int num, int count);

int my_getline(LoadStream *stream, int index, std::string &line);

class ObjFileParser : public CollectInterface
{
public:
  ObjFileParser(LoadStream *file_data, int objcount, std::vector<std::string> material_names) : file_data(file_data), obj_num(objcount), material_names_external(material_names) {
    firsttime = true;
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
    if (firsttime) {
      //std::cout << "Loading: " << filename << " " << obj_num << std::endl;
      //check_invalidate();
      Load2();
      firsttime = false;
    }
  }

  void Load2() {

    //std::ifstream file(filename.c_str());
    //std::string s(file_data.begin(), file_data.end());

    //int total_line_count=0;
    //int s4 = file_data.size();
    //for(int i=0;i<s4;i++) { if (file_data[i]=='\n') total_line_count++; }

    //std::stringstream file(s);
    std::string line;
    int obj_count = 0;
    int vertex_count = 0;
    int vertex_count2 = 0;
    int face_count=0;
    int normal_count =0;
    int normal_count2 = 0;
    int color_count = 0;
    int color_count2 = 0;
    int tex_count = 0;
    int tex_count2 = 0;

    //int obj_base_v = 0;
    //int obj_base_t = 0;
    //int obj_base_n = 0;
    //int mat_base_v = 0;
    //int mat_base_t = 0;
    //int mat_base_n = 0;
    //int obj_face_count = 0;
    int mat_face_count = 0;
    //int obj_base_c = 0;
    int mtl_material = -1;
    std::string word;
    std::stringstream ss;
    int curr_line = 0;
    bool first = true;
    int curr_tex_index = 0;
    InstallProgress(193,"Parsing .obj file", 15);
    int index = 0;
    //bool last_was_f = false;
    //bool new_mat = false;
    while((index = my_getline(file_data, index, line))!=-1)
      {
	curr_line++;
	//if (total_line_count>0 && curr_line%(total_line_count/15)==0) {
	//  ProgressBar(193,curr_line*15/total_line_count,15,"Parsing .obj file");
	//}
	ss.str(line);
	ss.clear();
	//ss << line;
	ss>>word;
	if (word == "usemtl")
	  {
	    std::string material_name;
	    ss >> material_name;
	    material_names_internal.push_back(material_name);
	    mtl_material++;

	    
	    //obj_vertex_start.push_back(mat_base_v);
	    // obj_vertex_end.push_back(vertex_count2);
	    // obj_normal_start.push_back(mat_base_n);
	    // obj_normal_end.push_back(normal_count2);
	    //  obj_tex_start.push_back(mat_base_t);
	    //  obj_tex_end.push_back(tex_count2);
	    //  new_mat = true;
	      
	      if (!first) {
		obj_face_counts_start.push_back(mat_face_count);
		obj_face_counts_end.push_back(face_count);
	      }
	      
	      //mat_base_v = vertex_count2;
	      //mat_base_n = normal_count2;
	      //mat_base_t = tex_count2;
	    mat_face_count = face_count;

	    first = false;
	    
	  }
	if (word == "o")
	  {
	    
	    //std::cout << "o" << std::flush;
	    obj_count++;
	    //obj_base_v = vertex_count2;
	    //obj_base_n = normal_count2;
	    //obj_base_t = tex_count2;

	    //obj_face_count = face_counts.size();
	    //obj_base_c = color_count2;
	  }
	if (word == "v")
	  {
	    
	    //std::cout << "v" << std::flush;
	    vertex_count++;
	    //if (vertex_count % 1000==0) 
	    //  std::cout << "v" << std::flush;
	    float x,y,z;
	    ss >> x >> y >> z;
	    float cr = 1.0, cg = 1.0, cb = 1.0;
	    bool b1 = false;
	    if (ss >> cr) b1=true;
	    bool b2 = false;
	    if (ss >> cg) b2=true;
	    bool b3 = false;
	    if (ss >> cb) b3=true;
	    //std::cout << "Vertex:" << vertex_data.size() << " " << x << " " << y << " " << z << std::endl;
	    Point p(x,y,z);
	    vertex_data.push_back(p);
	    texcoord3_data.push_back(0.0);
	    if (b1&&b2&&b3) {
	    ::Color vc(cr,cg,cb,1.0f);
	    color_data.push_back(vc.clamp().Pixel());
	    }
	  } else if (word =="v") { vertex_count2++; }
	if (word == "vt")
	  {	

	    //std::cout << "vt" << std::flush;

	    tex_count++;
	    //if (tex_count %1000 == 0)
	    //  std::cout << "t" << std::flush;
	    //std::cout << "Texture:" << texcoord_data.size() << std::endl;
	    float tx,ty,tz;
	    ss >> tx >> ty >> tz;
	    Point2d p = { tx, ty };
	    texcoord_data.push_back(p);
	    if (int(texcoord3_data.size())>tex_count-1)
	      texcoord3_data[tex_count-1]=0.0; // TODO, oli 0.0
	  } else if (word=="vt") { tex_count2++; }
	if (word == "vn")
	  {

	    //std::cout << "vn" << std::flush;
	    normal_count++;
	    //if (normal_count %1000 == 0)
	    //  std::cout << "n" << std::flush;

	    //std::cout << "Normal:" << normal_data.size() << std::endl;

	    float nx, ny, nz;
	    ss >> nx >> ny >> nz;
	    Vector v(nx,ny,nz);
	    normal_data.push_back(v);
	  } else if (word=="vn") { normal_count2++; }
	if (word == "vc")
	  {

	    //std::cout << "vc" << std::flush;
	    color_count++;
	    //if (color_count %1000 == 0)
	    //  std::cout << "c" << std::flush;

	    //std::cout << "Normal:" << normal_data.size() << std::endl;

	    int nr, ng, nb, na;
	    ss >> nr >> ng >> nb >> na;
	    ::Color vc(nr,ng,nb,na);
	    color_data.push_back(vc.clamp().Pixel());
	  } else if (word=="vc") { color_count2++; }
	if (word == "f")
	  {
	    //std::cout << "f" << std::flush;
	    face_count++;
	    //if (face_count % 1000 == 0)
	    //  std::cout << "f" << std::flush;

	    int v_index, t_index, n_index;
	    int count = 0;
	    char c;
	    bool t_bool = texcoord_data.size()!=0;
	    bool n_bool = normal_data.size()!=0;

	    has_t = t_bool;
	    has_n = n_bool;
	    
	    //std::cout << "Face:" << face_counts.size() << std::endl;
	    if (t_bool && n_bool) {
	      while(ss>>v_index>> c >>t_index>> c >>n_index)
		{
		  //std::cout << "1" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    if (t_index<0) t_index = texcoord_data.size()+t_index+1;
	    if (n_index<0) n_index = normal_data.size()+n_index+1;
	    //v_index -= obj_base_v;
	    //t_index -= obj_base_t;
	    //n_index -= obj_base_n;
	    vertex_index.push_back(v_index -1);
	    texture_index.push_back(t_index-1);
	    if (int(texcoord3_data.size())>t_index-1)
	      texcoord3_data[t_index-1]=mtl_material+0.3;
	    normal_index.push_back(n_index-1);
		  //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		  count++;
		}
	    }
	    if (t_bool && !n_bool)
	      {
		while(ss>>v_index>> c >>t_index)
		  {
		    //std::cout << "2" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    if (t_index<0) t_index = texcoord_data.size()+t_index+1;
	    //if (n_index<0) n_index = -n_index;
	    //v_index -= obj_base_v;
	    //t_index -= obj_base_t;
	    vertex_index.push_back(v_index-1);
	    texture_index.push_back(t_index-1);
	    if (int(texcoord3_data.size())>t_index-1)
	      texcoord3_data[t_index-1]=mtl_material+0.3;
	    normal_index.push_back(0);
		    //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		    count++;
		  }
	      }
	    if (!t_bool && n_bool)
	      {

		while(ss>>v_index>> c>> c >>n_index)
		  {
		    //std::cout << "3" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;
	    //if (t_index<0) t_index = -t_index;
	    if (n_index<0) n_index = normal_data.size()+n_index+1;
	    //v_index -= obj_base_v;
	    //n_index -= obj_base_n;

	    vertex_index.push_back(v_index-1);
	    texture_index.push_back(v_index-1);
	    if (int(texcoord3_data.size())>v_index-1)
	      texcoord3_data[v_index-1]=mtl_material+0.3;
	    //if (int(texcoord3_data.size())>0)
	    //  texcoord3_data[0]=mtl_material+0.3;
	    normal_index.push_back(n_index-1);
		    //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		    count++;
		  }
	      }
	    if (!t_bool && !n_bool) {
	      while(ss>>v_index)
		{
		  //std::cout << "4" << std::flush;
	    if (v_index<0) v_index = vertex_data.size()+v_index+1;

	    //v_index -= obj_base_v;

	    //if (t_index<0) t_index = -t_index;
	    //if (n_index<0) n_index = -n_index;

	    vertex_index.push_back(v_index-1);
		  texture_index.push_back(v_index-1);
	    if (int(texcoord3_data.size())>v_index-1)
	      texcoord3_data[v_index-1]=mtl_material+0.3;
	    //  if (int(texcoord3_data.size())>0)
	    //	    texcoord3_data[0]=mtl_material+0.3;

		  normal_index.push_back(0);
		  //std::cout << "Index: " << v_index << " " << t_index << " " << n_index << std::endl;
		  count++;
		}
	    }

	    face_counts.push_back(count);
	  }
      }
    if (texcoord_data.size()==0) 
      {
	Point2d p;
	p.x = 0.0;
	p.y = 0.0;
	texcoord_data.push_back(p);
      }
    if (texcoord3_data.size()==0) 
      {
	texcoord3_data.push_back(0.0);
      }
    if (normal_data.size()==0)
      {
	Point p(0.0,0.0,0.0);
	normal_data.push_back(p);
      }
    if (vertex_data.size()==0)
      {
	Point p(0.0,0.0,0.0);
	vertex_data.push_back(p);
	vertex_data.push_back(p);
	vertex_data.push_back(p);
      }
    if (color_data.size()==0)
      {
	color_data.push_back(0xffffffff);
      }

    //obj_vertex_start.push_back(mat_base_v);
    //obj_vertex_end.push_back(vertex_count2);
    //obj_normal_start.push_back(mat_base_n);
    //obj_normal_end.push_back(normal_count2);
    //obj_tex_start.push_back(mat_base_t);
    //obj_tex_end.push_back(tex_count2);
    obj_face_counts_start.push_back(mat_face_count);
    obj_face_counts_end.push_back(face_count);


    int ss2 = material_names_internal.size();
    for(int j=0;j<ss2;j++) {
      std::string internal = material_names_internal[j];
    int s = material_names_external.size();
    for(int i=0;i<s;i++)
      {
	std::string name = material_names_external[i];
	//std::cout << internal << "==" << name << std::endl;
	if (internal==name) {
	  material_names_map[j] = i+1;
	}
      }
    }
    
    // remove file from memory
    //file_data.clear();
  }

public:
  LoadStream *file_data;
  std::vector<Point> vertex_data;
  std::vector<Point2d> texcoord_data;
  std::vector<float> texcoord3_data;
  std::vector<Vector> normal_data;
  std::vector<unsigned int> color_data;
  std::vector<int> vertex_index;
  std::vector<int> texture_index;
  std::vector<int> normal_index;
  std::vector<int> face_counts;
  int obj_num;
  mutable std::vector<int> counts;
  mutable bool firsttime;
  std::vector<std::string> material_names_external;
  std::vector<std::string> material_names_internal;  

  std::map<int,int> material_names_map;
  
  //std::vector<int> obj_vertex_start;
  //std::vector<int> obj_vertex_end;
  //std::vector<int> obj_normal_start;
  //std::vector<int> obj_normal_end;
  //std::vector<int> obj_tex_start;
  //std::vector<int> obj_tex_end;
  std::vector<int> obj_face_counts_start;
  std::vector<int> obj_face_counts_end;

  bool has_t;
  bool has_n;
};

class ObjFileFaceCollection : public FaceCollection
{
public:
  ObjFileFaceCollection(ObjFileParser &parser, int obj_num) : parser(parser), obj_num(obj_num) { }
  virtual std::string name() const { return "ObjFileFaceCollection"; }

  void Collect(CollectVisitor &vis)
  {
    parser.Collect(vis);
  }
  void HeavyPrepare() { } // not called because register_obj missing
  void Prepare() { parser.Prepare(); }
  
  int NumFaces() const {
    int s = std::min(parser.obj_face_counts_end.size(),parser.obj_face_counts_start.size());
    if (obj_num>=s) return 0;
    //std::cout << "Obj:" << parser.obj_face_counts_end[obj_num];
    //std::cout << " " << parser.obj_face_counts_start[obj_num] << std::endl;
    //std::cout << parser.obj_face_counts_end[obj_num]-parser.obj_face_counts_start[obj_num] << std::endl; 
    return parser.obj_face_counts_end[obj_num]-parser.obj_face_counts_start[obj_num];
    //return parser.face_counts.size()<=0 ? 1 : parser.face_counts.size();
  }
  int NumPoints(int face) const {
    int s = std::min(parser.obj_face_counts_end.size(),parser.obj_face_counts_start.size());
    if (obj_num>=s) return 1;
    if (parser.face_counts.size()<=0) return 3;
    return parser.face_counts[face + parser.obj_face_counts_start[obj_num]];
    //return parser.face_counts.size()<=0 ? 3 : parser.face_counts[face];
  }
  Point FacePoint(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)parser.vertex_index.size())
      {
	int index = parser.vertex_index[c];
	if (index>=0 && index<(int)parser.vertex_data.size()) {
	  //std::cout << "FacePoint:" << face << " " << point << ":" << parser.vertex_data[index] << std::endl;
	  return parser.vertex_data[index];
	}
      }
    std::cout << "FacePoint error" << std::endl;
    Point p(0.0,0.0,0.0);
    return p;
  }
  bool has_normal() const { return parser.normal_index.size()>2; }
  Vector PointNormal(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)parser.normal_index.size())
      {
	int index = parser.normal_index[c];
	if (index>=0 && index<(int)parser.normal_data.size())
	  {
	    return parser.normal_data[index];
	  }
      }
    Vector v(0.0,0.0,0.0);
    return v;
  }
  bool has_attrib() const { return false; }
  float Attrib(int face, int point, int id) const {
    return 0.0; 
  }
  int AttribI(int face, int point, int id) const {
    return 0; }
  bool has_color() const { return parser.color_data.size()>2; }
  unsigned int Color(int face, int point) const { 
    int c = Count(face,point);
    if (c>=0 && c<(int)parser.vertex_index.size())
      {
	int index = parser.vertex_index[c];
	if (index>=0 && index<(int)parser.color_data.size())
	  return parser.color_data[index];
      }
    unsigned int p = 0xffffffff;
    //Point p(0.0,0.0,0.0);
    return p;
  }
  bool has_texcoord() const { return parser.texcoord3_data.size()>0; }
  float TexCoord3(int face, int point) const
  {
    int c = Count(face,point);
    if (parser.has_t) {
    if (c>=0 && c<(int)parser.texture_index.size())
      {
	int index = parser.texture_index[c];
	//if (face==0) std::cout << "R1:" << index << std::endl;
	if (index>=0 && index<(int)parser.texcoord3_data.size())
	  {
	    float i = parser.texcoord3_data[index];
	    int ii = int(i);
	    //if (face==0) std::cout << "R2:" << ii << std::endl;

	    int val = parser.material_names_map[ii];
	    //if (face==0) std::cout << "R3:" << val << std::endl;

	    if (val!=0) {
	      return float(val-1)+0.5;
	    }
	    

	    return i;
	  } else
	  {
	    int index = parser.vertex_index[c];
	    //if (face==0) std::cout << "R0:" << index << " " << parser.texcoord3_data.size() << std::endl;
	if (index>=0 && index<(int)parser.texcoord3_data.size())
	  {
	    float i = parser.texcoord3_data[index];
	    int ii = int(i);
	    //if (face==0) std::cout << "R2:" << ii << std::endl;

	    int val = parser.material_names_map[ii];
	    //if (face==0) std::cout << "R3:" << val << std::endl;

	    if (val!=0) {
	      return float(val-1)+0.5;
	    }
	    

	    return i;
	   
	  }
	  }
      }
    } else
      {
	    int index = parser.vertex_index[c];
	    //if (face==0) std::cout << "R0:" << index << " " << parser.texcoord3_data.size() << std::endl;
	if (index>=0 && index<(int)parser.texcoord3_data.size())
	  {
	    float i = parser.texcoord3_data[index];
	    int ii = int(i);
	    //if (face==0) std::cout << "R2:" << ii << std::endl;

	    int val = parser.material_names_map[ii];
	    //if (face==0) std::cout << "R3:" << val << std::endl;

	    if (val!=0) {
	      return float(val-1)+0.5;
	    }
	    

	    return i;
	   
	  }
      }
    
    return 0.0;
  }
  Point2d TexCoord(int face, int point) const
  {
    int c = Count(face,point);
    if (c>=0 && c<(int)parser.texture_index.size())
      {
	int index = parser.texture_index[c];
	if (index>=0 && index<(int)parser.texcoord_data.size())
	  {
	    return parser.texcoord_data[index];
	  }
      }
    Point2d p = { 0.0, 0.0 };
    return p;
  }

  int Count(int face, int point) const {
    int s = parser.face_counts.size();
    if (counts.size()>0) { return counts[face+parser.obj_face_counts_start[obj_num]]+point; }
    //if (s>0) { return face_counts[0]*face+point; }
    int c = 0;
    counts.push_back(c);
    for(int i=0;i<s;i++)
      {
	c+=parser.face_counts[i];
	counts.push_back(c);
      }
    return counts[face+parser.obj_face_counts_start[obj_num]]+point;
  }

private:
  ObjFileParser &parser;
  int obj_num;
  mutable std::vector<int> counts;

};


EXPORT GameApi::PolygonApi::PolygonApi(GameApi::Env &e) : e(e)
{
  priv = (void*)new PolyPriv;  
} 

EXPORT GameApi::PolygonApi::~PolygonApi()
{
  delete(PolyPriv*)priv;
}

EXPORT GameApi::P GameApi::PolygonApi::from_polygon(P p, std::function<P (int face, 
									  PT p1, PT p2, PT p3, PT p4)> f)

{
  return from_polygon_1(p,f);
}
void GameApi::PolygonApi::print_data(P p)
{
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_data" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_data" << std::endl;  return; }
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      std::cout << "Face " << i << ": ";
      int ss = coll->NumPoints(i);
      for(int j=0;j<ss;j++)
	{
	  Point p = coll->FacePoint(i,j);
	  std::cout << p << " ";
	}
      std::cout << std::endl;
    }

}
void GameApi::PolygonApi::print_data2(P p)
{
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_data" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_data" << std::endl;  return; }
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      std::cout << "Face2 " << i << ": ";
      int ss = coll->NumPoints(i);
      for(int j=0;j<ss;j++)
	{
	  Point p = coll->EndFacePoint(i,j);
	  std::cout << p << " ";
	}
      std::cout << std::endl;
    }

}
void GameApi::PolygonApi::print_stat(P p)
{
  //FaceCollPolyHandle *handle = find_poly(e, p);
  //FaceCollection *coll = handle->coll;
  if (p.id == -1) { std::cout << "INVALID P OBJECT at print_stat" << std::endl; return; }
  FaceCollection *coll = find_facecoll(e, p);
  if (!coll) { std::cout << "INVALID FACECOLLECTION at print_stat" << std::endl;  return; }
  //int faces = coll->NumFaces();
  //int points = faces>0 ? coll->NumPoints(0) : 0;
  //std::cout << "Faces: " << faces << std::endl;
  //std::cout << "Points: " << points << std::endl;

}

GameApi::P GameApi::PolygonApi::from_polygon_1(P p, std::function<P (int face,
								     PT p1, PT p2, PT p3, PT p4)> f)
{
  FaceCollPolyHandle *handle = find_poly(e, p);
  FaceCollection *coll = handle->coll;
  int s = coll->NumFaces();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point p1 = coll->FacePoint(i, 0);
      Point p2 = coll->FacePoint(i, 1);
      Point p3 = coll->FacePoint(i, 2);
      Point p4 = coll->NumPoints(i)>3 ? coll->FacePoint(i, 3) : coll->FacePoint(i,2);
      PT pp1 = add_point(e, p1.x, p1.y, p1.z);
      PT pp2 = add_point(e, p2.x, p2.y, p2.z);
      PT pp3 = add_point(e, p3.x, p3.y, p3.z);
      PT pp4 = add_point(e, p4.x, p4.y, p4.z);
      P p = f(i, pp1, pp2, pp3, pp4);
      vec.push_back(p);
    }
  P pp = or_array(&vec[0], s);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_lines(LI li, std::function<P (int i, float sx, float sy, float sz, float ex, float ey, float ez, unsigned int scolor, unsigned int ecolor)> f)
{
  LineCollection *coll = find_line_array(e, li);
  int count = coll->NumLines();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point p1 = coll->LinePoint(i, 0);
      Point p2 = coll->LinePoint(i, 1);
      unsigned int c1 = coll->LineColor(i, 0);
      unsigned int c2 = coll->LineColor(i, 1);
      P item = f(i,p1.x,p1.y,p1.z, p2.x,p2.y,p2.z,c1,c2);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::from_points(PTS pts, std::function<P (int i, float x, float y, float z, unsigned int color)> f)
{
  PointsApiPoints *p = find_pointsapi_points(e, pts);
  int count = p->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<count;i++)
    {
      Point pos = p->Pos(i);
      unsigned int color = p->Color(i);
      P item = f(i, pos.x,pos.y,pos.z,color);
      vec.push_back(item);
    }
  P pp = or_array(&vec[0], count);
  return pp;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_voxel(std::function<P (unsigned int c)> f, VX voxel, float dx, float dy, float dz, int max_c)
{
  std::vector<P> pieces;
  for(int i=0;i<max_c;i++)
    {
      pieces.push_back(f(i));
    }


  Voxel<unsigned int> *vox = find_voxel(e, voxel);
  int sx = vox->SizeX();
  int sy = vox->SizeY();
  int sz = vox->SizeZ();
  std::vector<P> vec_x;
  vec_x.reserve(sx);
  for(int x=0;x<sx;x++)
    {
      std::vector<P> vec_y;
      vec_y.reserve(sy);
      for(int y=0;y<sy;y++)
	{
	  std::vector<P> vec_z;
	  vec_z.reserve(sz);
	  for(int z=0;z<sz;z++)
	    {
	      unsigned int c = vox->Map(x,y,z);
	      unsigned int i = c;
	      P p = pieces[i];
	      P p2 = translate_1(p, 0.0,0.0,z*dz);
	      vec_z.push_back(p2);
	    }
	  P p = or_array_1(&vec_z[0], sz);
	  P p2 = translate_1(p, 0.0,y*dy,0.0);
	  vec_y.push_back(p2);
	}
      P p = or_array_1(&vec_y[0], sy);
      P p2 = translate_1(p, x*dx,0.0,0.0);
      vec_x.push_back(p2);
    }
  P p = or_array_1(&vec_x[0],sx);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap2(EveryApi &ev, std::function<P (int c, PT tl, PT tr, PT bl, PT br)> f, BM int_bm, FB float_bm, float dx, float dz, float height)
{
  BitmapIntHandle *handle = static_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  Bitmap<int> *bm = handle->bm;
  Bitmap<float> *fbm = find_float_bitmap(e, float_bm)->bitmap;
  std::vector<P> vec;
  int sx = std::min(fbm->SizeX(),bm->SizeX());
  int sy = std::min(fbm->SizeY(),bm->SizeY());
  for(int y=0;y<sy-1;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx-1;x++)
	{
	  float height_1 = fbm->Map(x,y);
	  float height_2 = fbm->Map(x+1,y);
	  float height_3 = fbm->Map(x,y+1);
	  float height_4 = fbm->Map(x+1,y+1);

	  PT pos_1 = ev.point_api.point(0.0, height_1*height, 0.0);
	  PT pos_2 = ev.point_api.point(dx, height_2*height, 0.0);
	  PT pos_3 = ev.point_api.point(0.0, height_3*height, dz);
	  PT pos_4 = ev.point_api.point(dx, height_4*height, dz);

	  P p = f(bm->Map(x,y), pos_1, pos_2, pos_3, pos_4);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx-1);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy-1);
  return p;
}
int index_from_string(char c, std::string s)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      if (s[i]==c) { return i; }
    }
  return 0;
}

EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap(EveryApi &ev, std::vector<P> pieces, std::string filename, std::string chars, float dx, float dz, int ssx, int ssy)
{
  if (pieces.size()==0) return p_empty();
  char *array = new char[ssx*ssy];
  std::ifstream ss(filename.c_str());
  for(int i=0;i<ssx*ssy;i++)
    {
      ss >> array[i];
    }

  BM int_bm = ev.bitmap_api.newintbitmap(array, ssx,ssy, [&chars](char c) { return index_from_string(c,chars); });

  BitmapIntHandle *handle = static_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }

  Bitmap<int> *bm = handle->bm;
  std::vector<P> vec;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  int val = bm->Map(x,y);
	  int sk = pieces.size();
	  if (val<0) val=0;
	  if (val>=sk) val=0;
	  P p = pieces[val];
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;

}
EXPORT GameApi::P GameApi::PolygonApi::world_from_bitmap(std::function<P (int c)> f, BM int_bm, float dx, float dz, int max_c)
{
  BitmapIntHandle *handle = static_cast<BitmapIntHandle*>(find_bitmap(e, int_bm));
  if (!handle) { GameApi::P p1 = { 0 }; return p1; }
  std::vector<P> pieces;
  for(int i=0;i<max_c;i++)
    {
      pieces.push_back(f(i));
    }

  Bitmap<int> *bm = handle->bm;
  std::vector<P> vec;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  P p = pieces[bm->Map(x,y)];
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}

class ColorDistance3 : public ForwardFaceCollection
{
public:
  ColorDistance3(FaceCollection *faces, Point center, unsigned int color_center, unsigned int color_dist, float dist_center, float dist_dist) : ForwardFaceCollection(*faces), center(center), color_center(color_center), color_dist(color_dist), dist_center(dist_center), dist_dist(dist_dist) { }
  virtual std::string name() const { return "ColorDistance3"; }

  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    p-=center;
    float d = p.Dist();
    d-=dist_center;
    d/=dist_dist-dist_center;
    if (d<0.0) d=0.0;
    if (d>1.0) d=1.0;
    return Color::Interpolate(color_center, color_dist, d);
  }
private:
  Point center;
  unsigned int color_center, color_dist;
  float dist_center, dist_dist;
};

GameApi::P GameApi::PolygonApi::color_distance(P faces, float center_x, float center_y, float center_z, unsigned int color_center, unsigned int color_dist, float dist_center, float dist_dist)
{
  FaceCollection *face = find_facecoll(e, faces);
  Point pt(center_x, center_y, center_z);
  return add_polygon2(e, new ColorDistance3(face, pt, color_center, color_dist, dist_center, dist_dist),1);
}

#if 0
class DistFromLines : public SingleForwardFaceCollection
{
public:
  DistFromLines(LineCollection *coll, float d1, float d2, Point center) : coll(coll), d1(d1), d2(d2), center(center) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
  }
  void HeavyPrepare() { } // not called because register_obj missing
  void Prepare() { coll->Prepare(); }
  virtual int NumFaces() const { return coll->NumLines(); }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = coll->LinePoint(face, 0);
    Point p2 = coll->LinePoint(face, 1);
    
    Vector v1 = p1 - center;
    Vector v2 = p2 - center;
    float dist1 = v1.Dist();
    float dist2 = v2.Dist();
    v1/=dist1;
    v2/=dist2;
    Point pp1a = center + v1*(dist1+d1);
    Point pp1b = center + v1*(dist1+d2);
    Point pp2a = center + v2*(dist2+d1);
    Point pp2b = center + v2*(dist2+d2);
    if (point==0) return pp1a;
    if (point==1) return pp2a;
    if (point==2) return pp2b;
    if (point==3) return pp1b;
    return pp1a;
  }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    v/=v.Dist();
    store_face=face;
    store_res=v;
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p1 = { 0.0, 0.0 };
    Point2d p2 = { 1.0, 0.0 };
    Point2d p3 = { 1.0, 1.0 };
    Point2d p4 = { 0.0, 1.0 };
    if (point==0) return p1;
    if (point==1) return p2;
    if (point==2) return p3;
    if (point==3) return p4;
    return p1;
  }

private:
  LineCollection *coll;
  float d1,d2;
  Point center;
  mutable int store_face=-1;
  mutable Vector store_res;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::dist_from_lines(LI li, float d1, float d2, PT center)
{
#if 0
  Point *pt = find_point(e, center);
  LineCollection *lines = find_line_array(e, li);
  return add_polygon2(e, new DistFromLines(lines, d1, d2, *pt), 1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::triangle(PT p1, PT p2, PT p3)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  FaceCollection *coll = new TriElem(*pp1, *pp2, *pp3);
  return add_polygon(e, coll,1);
}
#if 0
class UnitFaceColl : public ForwardFaceCollection
{
public:
  UnitFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::unit_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
 Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1), 1);
#endif
}
#if 0
class UnitToFaceColl : public ForwardFaceCollection
{
public:
  UnitToFaceColl(FaceCollection *coll, Point pos, Vector u_x, Vector u_y, Vector u_z) : ForwardFaceCollection(*coll) { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p1 = UnitToCube(p, pos, u_x, u_y, u_z);
    return p1;
  }
private:
  Point pos;
  Point u_x;
  Point u_y;
  Point u_z;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::unit_to_cube(P orig, PT pos, V u_x, V u_y, V u_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  Point *pos_1 = find_point(e, pos);
  Vector *u_x_1 = find_vector(e, u_x);
  Vector *u_y_1 = find_vector(e, u_y);
  Vector *u_z_1 = find_vector(e, u_z);
  return add_polygon(e, new UnitToFaceColl(coll, *pos_1, *u_x_1, *u_y_1, *u_z_1),1);
#endif
}

#if 0
class UnitToFlexClass : public ForwardFaceCollection
{
public:
  UnitToFlexClass(FaceCollection *coll, Point bTL, Point bTR, Point bBL, Point bBR,
	     Point fTL, Point fTR, Point fBL, Point fBR) 
    : ForwardFaceCollection(*coll), bTL(bTL), bTR(bTR), bBL(bBL), bBR(bBR),
      fTL(fTL), fTR(fTR), fBL(fBL), fBR(fBR)
  { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face, point);
    Point pp = UnitToFlex(p, bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR);
    return pp;
  }
private:
  Point bTL, bTR, bBL, bBR, fTL, fTR, fBL, fBR;
};
#endif

EXPORT GameApi::P GameApi::PolygonApi::unit_to_flex(P orig, 
					     PT bTL, PT bTR, PT bBL, PT bBR,
					     PT fTL, PT fTR, PT fBL, PT fBR)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  Point *bTL1 = find_point(e, bTL);
  Point *bTR1 = find_point(e, bTR);
  Point *bBL1 = find_point(e, bBL);
  Point *bBR1 = find_point(e, bBR);

  Point *fTL1 = find_point(e, fTL);
  Point *fTR1 = find_point(e, fTR);
  Point *fBL1 = find_point(e, fBL);
  Point *fBR1 = find_point(e, fBR);
  return add_polygon(e, new UnitToFlexClass(coll, *bTL1, *bTR1, *bBL1, *bBR1,
				       *fTL1, *fTR1, *fBL1, *fBR1),1);
#endif
}


EXPORT GameApi::P GameApi::PolygonApi::p_empty()
{
  return add_polygon(e,new EmptyBoxableFaceCollection, 1);
}
LoadStream *load_from_vector(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec);

EXPORT GameApi::P GameApi::PolygonApi::load_model_all_no_cache(std::string filename,  int count, bool nr)
{
  std::ifstream data(filename.c_str());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
  char c;
  while(data.get(c)) vec2.push_back(c);
  LoadStream *stream = load_from_vector(vec2);
  return load_model_all_no_cache(stream,count,nr);
}
EXPORT GameApi::P GameApi::PolygonApi::load_model_all_no_cache(LoadStream *file_data, int count, bool nr)
{


  int s=count;
  std::vector<P> vec;
  LoadStream *file = file_data->Clone();
  file->Prepare();
  ObjFileParser *parser = new ObjFileParser(file, -1, std::vector<std::string>());
  for(int i=0;i<s;i++)
    {

      GameApi::P model = add_polygon2(e, new ObjFileFaceCollection(*parser, i),1);
      //GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(file, i, std::vector<std::string>()), 1);
      vec.push_back(model);
    }
  GameApi::P obj = or_array2(vec);
  if (!nr) {
    GameApi::P resize = resize_to_correct_size(obj);
    return resize;
  } else {
    GameApi::P resize = resize_to_correct_size2(e,obj,&g_last_resize);
    return resize;
  }

}

EXPORT GameApi::P GameApi::PolygonApi::load_model_all_no_cache_mtl(std::string filename, int count, std::vector<std::string> material_names, bool nr)
{
  std::ifstream data(filename.c_str());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
  char c;
  while(data.get(c)) vec2.push_back(c);
  LoadStream *stream = load_from_vector(vec2);
  return load_model_all_no_cache_mtl(stream,count,material_names,nr);
}
EXPORT GameApi::P GameApi::PolygonApi::load_model_all_no_cache_mtl(LoadStream *file_data, int count, std::vector<std::string> material_names, bool nr)
{
  int s=count;
  if (s>700) {
    // This is slower version, but works with less memory,
    // if count is bigger than 500, the slower version is used.
    P vec = p_empty();
    
    LoadStream *stream = file_data->Clone();
    stream->Prepare();
    ObjFileParser *parser = new ObjFileParser(stream, -1, material_names);
    for(int i=0;i<s;i++)
      {
	GameApi::P model = add_polygon2(e, new ObjFileFaceCollection(*parser, i),1);
	vec = or_elem(vec,model);
      }
    GameApi::P obj = vec; //or_array2(vec);
    if (!nr) {
      GameApi::P resize = resize_to_correct_size(obj);
      return resize;
    } else {
      GameApi::P resize = resize_to_correct_size2(e,obj,&g_last_resize);
      return resize;
    }
  } else {
    // This eats more memory, but works faster.
    std::vector<P> vec;
    LoadStream *stream = file_data->Clone();
    stream->Prepare();
    ObjFileParser *parser = new ObjFileParser(stream, -1, material_names);
    for(int i=0;i<s;i++)
      {
	GameApi::P model = add_polygon2(e, new ObjFileFaceCollection(*parser, i),1);	
	vec.push_back(model);
      }
    GameApi::P obj = or_array2(vec);
    if (!nr) {
      GameApi::P resize = resize_to_correct_size(obj);
      return resize;
    } else {      
      GameApi::P resize = resize_to_correct_size2(e,obj,&g_last_resize);
      return resize;
    }
  }
}

LoadStream *load_from_vector(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec);

EXPORT GameApi::P GameApi::PolygonApi::load_model_all(std::string filename, int count)
{
  std::ifstream data(filename.c_str());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
  char c;
  while(data.get(c)) vec2.push_back(c);

  LoadStream *stream = load_from_vector(vec2);
  ObjFileParser *parser = new ObjFileParser(stream, -1, std::vector<std::string>());

  int s=count;
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      int c = get_current_block();
      set_current_block(-1);
        GameApi::P model = add_polygon2(e, new ObjFileFaceCollection(*parser, i),1);
	//GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(stream, i, std::vector<std::string>()), 1);
	GameApi::P cache = model; //file_cache(model, filename,i);
      set_current_block(c);
      vec.push_back(cache);
    }
  GameApi::P obj = or_array2(vec);
  GameApi::P resize = resize_to_correct_size(obj);
  return resize;
}

std::vector<std::pair<std::string, int> > prepare_cache_data;

int find_data(std::string data) {
  int s = prepare_cache_data.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string,int> p = prepare_cache_data[i];
      if (p.first==data) return p.second;
    }
  return -1;
}

class PrepareCache : public FaceCollection
{
public:
  PrepareCache(GameApi::Env &e, std::string id, FaceCollection *coll) : e(e), id(id), coll(coll) {}
  virtual std::string name() const { return "PrepareCache"; }

  FaceCollection *get_coll2() const {
    if (coll_cache) return coll_cache;
    int num = find_data(id);
    if (num==-1) { /*std::cout << "get_coll2: textures probably not shown" << std::endl;*/ return 0; }
    GameApi::P p;
    p.id = num;
    FaceCollection *coll = find_facecoll(e, p);
    return coll;
  }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    
    if (find_data(id)!=-1)
      {

      std::string url = id;
      /*
  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url + " (cached)",15);
  }

  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url + " (cached)");
  }
      */
	//std::cout << "PrepareCache: SKIPPED!" << std::endl;
	return;
      }
    //coll->Prepare();
    int c = get_current_block();
    set_current_block(-2); // dont take ownership of this
    GameApi::P num = add_polygon2(e, coll,1);
    set_current_block(c);
    prepare_cache_data.push_back(std::make_pair(id,num.id));

    
  }
  void SetDoneCount(int i) { coll->SetDoneCount(i); }
  void PrepareDone() { coll->PrepareDone(); }
  void Prepare()
  {
    //std::cout << "PrepareCache: " << id << std::endl;
    if (find_data(id)!=-1)
      {

      std::string url = id;

      /*
      { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  InstallProgress(sum,url + " (cached)",15);
  }

  { // progressbar
  int s = url.size();
  int sum=0;
  for(int i=0;i<s;i++) sum+=int(url[i]);
  sum = sum % 1000;
  ProgressBar(sum,15,15,url + " (cached)");
  }
      */
	//std::cout << "PrepareCache: SKIPPED!" << std::endl;
	return;
      }
    coll->Prepare();
    int c = get_current_block();
    set_current_block(-2); // dont take ownership of this
    GameApi::P num = add_polygon2(e, coll,1);
    set_current_block(c);
    prepare_cache_data.push_back(std::make_pair(id,num.id));
  }
  FaceCollection *get_coll() const
  {
    if (coll_cache) return coll_cache;
    int num = find_data(id);
    if (num==-1) {const_cast<PrepareCache*>(this)->Prepare(); num=find_data(id); }
    GameApi::P p;
    p.id = num;
    FaceCollection *coll = find_facecoll(e, p);
    coll_cache = coll;
    return coll;
  }
  virtual bool has_normal() const { return get_coll()->has_normal(); }
  virtual bool has_attrib() const { return get_coll()->has_attrib(); }
  virtual bool has_color() const { return get_coll()->has_color(); }
  virtual bool has_texcoord() const { return get_coll()->has_texcoord(); }
  virtual bool has_skeleton() const { return get_coll()->has_skeleton(); }

  virtual int NumFaces() const 
  {
    return get_coll()->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    return get_coll()->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    return get_coll()->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return get_coll()->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return get_coll()->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return get_coll()->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return get_coll()->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return get_coll()->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const {
    return get_coll()->TexCoord3(face,point);
  }
  virtual int NumObjects() const { return get_coll()->NumObjects(); }
  virtual std::pair<int,int> GetObject(int o) const { return get_coll()->GetObject(o); }

  
  virtual int NumTextures() const { return get_coll()->NumTextures(); }
  virtual void GenTexture(int num) { get_coll()->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return get_coll()->TextureBuf(num); }
  virtual int FaceTexture(int face) const { return get_coll()->FaceTexture(face); }

private:
  GameApi::Env &e;
  std::string id;
  FaceCollection *coll;
  mutable FaceCollection *coll_cache=0;
};

LoadStream *load_from_url_stream(std::string url);
class NetworkedFaceCollection : public FaceCollection
{
public:
  NetworkedFaceCollection(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *empty, std::string url, std::string homepage, int count, bool nr) : e(e), ev(ev), url(url), homepage(homepage), count(count), empty(empty),nr(nr)
  {
    current = empty;
    filled = 0;
  }
  virtual std::string name() const { return "NetworkedFaceCollection"; }

  ~NetworkedFaceCollection() { delete stream; }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void Prepare()
  {
    if (current == empty) {

      if (url.size()>2 && url[url.size()-3]=='.' && url[url.size()-2]=='d' &&url[url.size()-1]=='s') {

	GameApi::P p = ev.polygon_api.p_ds_url(ev,url);
	FaceCollection *coll = find_facecoll(e,p);
	filled = coll;
	current = filled;
	current->Prepare();
      } else {
      
#ifdef HAS_POPEN
	delete stream;
      stream = load_from_url_stream(url);
#else
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif

    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }
    static int num=0;
    num++;
    //std::stringstream ss2;
    //ss2 << "p_url_file" << num << ".obj";
    //std::string filename = ss2.str();
    //std::fstream ss(filename.c_str(), std::ios_base::binary |std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.close();
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec(ptr->begin(),ptr->end());
    delete stream;
    stream = load_from_vector(vec);
#endif
    GameApi::P p = ev.polygon_api.load_model_all_no_cache(stream, count,nr);
    FaceCollection *coll = find_facecoll(e, p);
    filled = coll;
    current = filled;
    current->Prepare();
      }
    }
  }

  virtual bool has_normal() const { return current->has_normal(); }
  virtual bool has_attrib() const { return current->has_attrib(); }
  virtual bool has_color() const { return current->has_color(); }
  virtual bool has_texcoord() const { return current->has_texcoord(); }
  virtual bool has_skeleton() const { return current->has_skeleton(); }

  virtual int NumFaces() const { return current->NumFaces(); }
  virtual int NumPoints(int face) const { return current->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return current->FacePoint(face,point); }
  virtual Point EndFacePoint(int face, int point) const { return current->EndFacePoint(face, point); }

  virtual Vector PointNormal(int face, int point) const { return current->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return current->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return current->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return current->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return current->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return current->TexCoord3(face,point); }
  virtual int NumTextures() const { return current->NumTextures(); }
  virtual void GenTexture(int num) { current->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return current->TextureBuf(num); }
  virtual int FaceTexture(int face) const { return current->FaceTexture(face); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  int count;
  FaceCollection *empty;
  FaceCollection *filled;
  FaceCollection *current;
  LoadStream *stream=0;
  bool nr;
};

class NetworkedFaceCollectionMTL : public FaceCollection
{
public:
  NetworkedFaceCollectionMTL(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *empty, std::string url, std::string homepage, int count, std::vector<std::string> material_names, bool nr) : e(e), ev(ev), url(url), homepage(homepage), count(count), empty(empty), material_names(material_names),nr(nr)
  {
    current = empty;
    filled = 0;
  }
  virtual std::string name() const { return "NetworkedFaceCollectionMTL"; }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  void Prepare()
  {
    if (current == empty) {
      if (url.size()>2 && url[url.size()-3]=='.' && url[url.size()-2]=='d' &&url[url.size()-1]=='s') {

	GameApi::P p = ev.polygon_api.p_ds_url(ev,url);
	FaceCollection *coll = find_facecoll(e,p);
	filled = coll;
	current = filled;
	current->Prepare();
      } else {


      
#ifdef HAS_POPEN
	//std::cout << "load_from_url" << url << std::endl;
      LoadStream *stream = load_from_url_stream(url);
#else
      
      //std::cout << "A" << std::endl;
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    //std::cout << "B" << std::endl;

    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }
    static int num=0;
    num++;
    //std::stringstream ss2;
    //ss2 << "p_url_file" << num << ".obj";
    //std::string filename = ss2.str();
    //std::fstream ss(filename.c_str(), std::ios_base::binary |std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.close();
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec(ptr->begin(),ptr->end());
    LoadStream *stream = load_from_vector(vec);
#endif
    GameApi::P p = ev.polygon_api.load_model_all_no_cache_mtl(stream, count,material_names,nr);
    FaceCollection *coll = find_facecoll(e, p);
    filled = coll;
    current = filled;
    current->Prepare();
      }
    }
  }

  virtual bool has_normal() const { return current->has_normal(); }
  virtual bool has_attrib() const { return current->has_attrib(); }
  virtual bool has_color() const { return current->has_color(); }
  virtual bool has_texcoord() const { return current->has_texcoord(); }
  virtual bool has_skeleton() const { return current->has_skeleton(); }

  
  virtual int NumFaces() const { return current->NumFaces(); }
  virtual int NumPoints(int face) const { return current->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return current->FacePoint(face,point); }
  virtual Point EndFacePoint(int face, int point) const { return current->EndFacePoint(face, point); }

  virtual Vector PointNormal(int face, int point) const { return current->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return current->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return current->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return current->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return current->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return current->TexCoord3(face,point); }
  virtual int NumTextures() const { return current->NumTextures(); }
  virtual void GenTexture(int num) { current->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return current->TextureBuf(num); }
  virtual int FaceTexture(int face) const { return current->FaceTexture(face); }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  int count;
  FaceCollection *empty;
  FaceCollection *filled;
  FaceCollection *current;
  std::vector<std::string> material_names;
  bool nr;
};

void MTL_CB(void *data);
void MTL2_CB(void *data);
class NetworkedFaceCollectionMTL2;
struct CBData {
  NetworkedFaceCollectionMTL2 *t;
  int i;
  std::string url;
  std::string d_url;
  std::string bump_url;
};


extern int async_pending_count;
IMPORT extern std::string gameapi_homepageurl;

std::string convert_slashes(std::string s);

std::vector<GameApi::TXID> GameApi::PolygonApi::mtl_parse(EveryApi &ev, std::vector<unsigned char> mtlfilecontents, std::string url_prefix, int delta)
{
  std::vector<unsigned char> *ptr2 = &mtlfilecontents;
  static int num=0;
  num++;
  std::stringstream a_ss2;
    a_ss2 << "p_mtl_id_file" << num << ".mtl";
    std::string start="";
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\";
#endif
#ifndef ANDROID
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  if (val) { std::cout << "system returned: " << val << std::endl; }
  start+="/";
#endif
#endif
#ifdef ANDROID
  start="/data/local/tmp/";
#endif

  /*
    std::string a_filename = start+a_ss2.str();
    //std::cout << "Saving: " << a_filename << std::endl;
    std::fstream a_ss(a_filename.c_str(), std::ios_base::binary |std::ios_base::out);
    //int a_s = ptr2->size();
    //for(int a_i=0;a_i<a_s;a_i++) a_ss.put(ptr2->operator[](a_i));
    a_ss.write((const char *)&ptr2->operator[](0),ptr2->size());
    a_ss.close();
  */
    std::string s(&ptr2->operator[](0), &ptr2->operator[](0) + ptr2->size());
    std::vector<GameApi::MaterialDef> mat = ev.polygon_api.parse_mtl(s /*a_filename*/);
    GameApi::Env &env = ev.get_env();
    int b_s = mat.size();
    std::vector<TXID> vec;
    std::vector<std::string> vec2;
    std::vector<std::string> vec3; // d
    std::vector<std::string> vec4; // bump
    for(int b_i=0;b_i<b_s;b_i++)
      {
	std::string s = mat[b_i].map_Ka;
	if (s.size()==0) s=mat[b_i].map_Kd;

	std::string s2 = mat[b_i].map_d;
	std::string s3 = mat[b_i].map_bump;
	//static int ii = 0;
	//std::stringstream ss;
	//ss << ii; ii++;
	//std::cout << "mtl_parse: " << url_prefix << "/" << s << std::endl;
	std::string url = convert_slashes(url_prefix+"/"+s);
	std::string url2 = convert_slashes(url_prefix+"/"+s2);
	std::string url3 = convert_slashes(url_prefix+"/"+s3);
	vec2.push_back(url);
	vec3.push_back(url2); // d
	vec4.push_back(url3); // bump
	vec.push_back(ev.bitmap_api.dyn_fetch_bitmap(ev,url,300000,b_i-delta));
      }
    env.async_load_all_urls(vec2, gameapi_homepageurl);
    return vec;
}

int g_use_texid_material=0;

void MTL_d_CB(void *data);
void MTL_bump_CB(void *data);

struct ASyncCallback { void (*fptr)(void*); void *data; };
ASyncCallback *rem_async_cb(std::string url);

//extern std::map<std::string, std::vector<unsigned char>* > load_url_buffers_async;

std::string MB(long num);
int CalcUrlIndex(std::string url);

std::vector<std::string> mtl_urls;
int g_previous_texid_material = 0;
void stackTrace();
class NetworkedFaceCollectionMTL2 : public FaceCollection
{
public:
  ~NetworkedFaceCollectionMTL2() {
    int ds = del_vec.size();
    for(int i=0;i<ds;i++)
      delete del_vec[i];
    int s = buffer.size();
    for(int i=0;i<s;i++)
      {
  	BufferRef::FreeBuffer(buffer[i]);
      }
    int s2 = d_buffer.size();
    for(int i2=0;i2<s2;i2++)
      {
  	BufferRef::FreeBuffer(d_buffer[i2]);
      }
    int s3 = bump_buffer.size();
    for(int i3=0;i3<s3;i3++)
      {
  	BufferRef::FreeBuffer(bump_buffer[i3]);
      }
  }
  virtual std::string name() const { return "NetworkedFaceCollectionMTL2"; }

  void free_extra_mem()
  {
    //std::cout << "FREE_EXTRA_MEM" << std::endl;
    int s = buffer.size();
    for(int i=0;i<s;i++)
      {
  	BufferRef::FreeBuffer(buffer[i]);
      }
    int s2 = d_buffer.size();
    for(int i2=0;i2<s2;i2++)
      {
  	BufferRef::FreeBuffer(d_buffer[i2]);
      }
    int s3 = bump_buffer.size();
    for(int i3=0;i3<s3;i3++)
      {
  	BufferRef::FreeBuffer(bump_buffer[i3]);
      }
    
  }
  void SetDoneCount(int i) { done_count = i; }
  void PrepareDone() { /*std::cout << "PrepareDone" << std::endl; stackTrace();*/
    current->PrepareDone();
    done_count--;
    if (done_count<=0)
      free_extra_mem();
  }
  NetworkedFaceCollectionMTL2(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *empty, std::string obj_url, std::string homepage, int count, std::string mtl_url, std::string url_prefix, bool cached, bool load_d, bool load_bump, bool nr) : e(e), ev(ev), url(obj_url), homepage(homepage), mtl_url(mtl_url), url_prefix(url_prefix), count(count), empty(empty),load_d(load_d), load_bump(load_bump),nr(nr)
  {
    int id1 = CalcUrlIndex(mtl_url);
    SetupProgress(id1, 15);
    SetupProgress(-1, 15);
    
    bool cached2 = false;
    int s = mtl_urls.size();
    for(int i=0;i<s;i++) {
      if (mtl_urls[i]==mtl_url) cached2=true;
    }
    mtl_urls.push_back(mtl_url);
    current = empty;
    filled = 0;
    bool cached3 = (cached || cached2) && g_previous_texid_material == g_use_texid_material;
    //cached3 = false;
    g_previous_texid_material = g_use_texid_material;
    if (!cached3) {
    e.async_load_callback(mtl_url, &MTL2_CB, (void*)this);
#ifdef EMSCRIPTEN
    async_pending_plus("MTL2","");
    async_pending_count++;
    //std::cout << "NetworkedFaceCollectionMTL2 asyncpending++" << async_pending_count << std::endl;
#endif
    }
    done_mtl=false;
  }
  void PrepareMTL()
  {
    if (done_mtl) return;
    // std::cout << "PrepareMTL" << std::endl;
    GameApi::ASyncVec *ptr2 = e.get_loaded_async_url(mtl_url);
    if (!ptr2) {
      std::cout << "p_mtl .mtl async not ready yet, failing..." << std::endl;
      return;
    }
    static int num=0;
    num++;

    std::stringstream a_ss2;
    a_ss2 << "p_mtl_file" << num << ".mtl";
    std::string start="";
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\";
#endif
#ifndef ANDROID
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  if (val)
    std::cout << "system returned: " << val << std::endl;
  start+="/";
#endif
#endif
#ifdef ANDROID
  start="/data/local/tmp/";
#endif
  
  std::string a_filename = start+a_ss2.str();
  //std::cout << "Saving: " << a_filename << std::endl;
  /*
    std::fstream a_ss(a_filename.c_str(), std::ios_base::binary |std::ios_base::out);
    //int a_s = ptr2->size();
    //for(int a_i=0;a_i<a_s;a_i++) a_ss.put(ptr2->operator[](a_i));
    a_ss.write((const char*)&ptr2->operator[](0),ptr2->size());
    a_ss.close();
  */
    std::string s(&ptr2->operator[](0), &ptr2->operator[](0) + ptr2->size());
    delete ptr2;
    std::vector<GameApi::MaterialDef> mat = ev.polygon_api.parse_mtl(s /*a_filename*/);
    materials = mat;
    BufferRef ref; ref.buffer=0;
    BufferRef ref2; ref2.buffer=0;
    BufferRef ref3; ref3.buffer=0;
    int b_s = mat.size();
    std::vector<std::string> urls;
    std::vector<std::string> urls2;
    std::vector<std::string> urls3;
    for(int b_i=0;b_i<b_s;b_i++)
      {
	material_names.push_back(mat[b_i].material_name);
	CBData *dt = new CBData;
	del_vec.push_back(dt);
	dt->t = this;
	dt->i = b_i;
	std::string s;
	  //if (mat[b_i].map_Ka)
	s = mat[b_i].map_Ka;
	if (s.size()==0) s=mat[b_i].map_Kd;

	std::string s2;
	  //if (mat[b_i].map_d)
	s2= mat[b_i].map_d;
	std::string s3;
	  //if (mat[b_i].map_bump)
	s3= mat[b_i].map_bump;

	
	bool skip=false;
	dt->url = convert_slashes(url_prefix+"/"+s);
	if (dt->url.size()>0 && dt->url[dt->url.size()-1]=='/')
	  {
	    skip=true;
	    //std::cout << "skipping " << b_i << std::endl;
	  }
	unsigned int r = rand();
	std::stringstream ss;
	ss << r;
	dt->url+="?" + ss.str();
	std::string url2 = convert_slashes(url_prefix+"/"+s2);
	dt->d_url = url2;
	std::string url3 = convert_slashes(url_prefix+"/"+s3);
	dt->bump_url = url3;
	  buffer.push_back(ref);
	  d_buffer.push_back(ref2);
	  bump_buffer.push_back(ref3);
	if (s==""||skip) {
	  Prepare2_color(b_i, mat[b_i]);
	}

	//std::cout << "set_callback: " << dt->url << std::endl;
    if (!g_use_texid_material)
      {
	if (s!=""&&!skip) {
	flags.push_back(1);
	e.async_load_callback(dt->url, &MTL_CB, (void*)dt);
#ifdef EMSCRIPTEN
	//if (load_url_buffers_async[std::string("load_url.php?url=")+dt->url]==0) {
	//std::cout << "Loading url:" << dt->url << std::endl;
	e.async_load_url(dt->url, homepage);
	  //} else {
	  //  ASyncCallback *cb = rem_async_cb(std::string("load_url.php?url=")+dt->url);
	  // if (cb) {
	  //   (*cb->fptr)(cb->data);
	  //  }
	  //}
#else
	urls.push_back(dt->url);
#endif
	} else { flags.push_back(0); }
	
	if (load_d && s2!="") {
	d_flags.push_back(1);
	  e.async_load_callback(url2, &MTL_d_CB, (void*)dt);
#ifdef EMSCRIPTEN
	  //if (load_url_buffers_async[std::string("load_url.php?url=")+url2]==0) {

	  e.async_load_url(url2, homepage);
	  //} else {
	  //  ASyncCallback *cb = rem_async_cb(std::string("load_url.php?url=")+url2);
	  //    if (cb) {
	  //    (*cb->fptr)(cb->data);
	  //  }
	  //}
#endif
	} else { d_flags.push_back(0); }
	
	if (load_bump && s3!="") {
	bump_flags.push_back(1);
	  e.async_load_callback(url3, &MTL_bump_CB, (void*)dt);
#ifdef EMSCRIPTEN
	  //if (load_url_buffers_async[std::string("load_url.php?url=")+url3]==0) {

	  e.async_load_url(url3, homepage);
	  //} else {
	  //	  ASyncCallback *cb = rem_async_cb(std::string("load_url.php?url=")+url3);
	  // if (cb) {
	  //   (*cb->fptr)(cb->data);
	  //  }
	  //
	//e.async_load_url(url3, homepage);
#endif
	} else { bump_flags.push_back(0); }
      }
#ifndef EMSCRIPTEN
	//Prepare2(dt->url,b_i);
#endif
#ifdef EMSCRIPTEN
    if (!g_use_texid_material) {
      if (!skip) {
    	async_pending_count++;
	async_pending_plus("texid_material", dt->url);
      }
    }
    //std::cout << "NetworkedFaceCollectionMTL2(1) asyncpending++"<< async_pending_count << std::endl;
#endif

    if (!g_use_texid_material)
	if (load_d && s2!="") {
#ifdef EMSCRIPTEN
	  {
    async_pending_count++;
    async_pending_plus("load_d", dt->d_url);
	  }
    //std::cout << "NetworkedFaceCollectionMTL2(2) asyncpending++" << async_pending_count << std::endl;
#endif
	}
    if (!g_use_texid_material)
	if (load_bump && s3!="") {
#ifdef EMSCRIPTEN
    async_pending_count++;
    async_pending_plus("load_bump", dt->bump_url);
    //std::cout << "NetworkedFaceCollectionMTL2(3) asyncpending++" << async_pending_count << std::endl;
#endif
	}
      }
  
#ifndef EMSCRIPTEN
        e.async_load_all_urls(urls, homepage);

	//     int s = urls.size();
	//for(int i=0;i<s;i++)
	// {
	//	e.async_load_url(urls[i],homepage);
	//}
#endif
#ifdef EMSCRIPTEN
    async_pending_count--;
    async_pending_minus("MTL2","");
    //std::cout << "NetworkedFaceCollectionMTL2 asyncpending--" << async_pending_count << std::endl;
#endif
    done_mtl=true;
  }
  void Prepare2_color(int i, GameApi::MaterialDef &def)
  {
    //std::cout << "Prepare2_color i=" << i << std::endl;
    BufferRef img = BufferRef::NewBuffer(2,2);
      int sx = img.width;
      int sy = img.height;
      ::Color cc(def.Kd_x,def.Kd_y,def.Kd_z,1.0f);
      unsigned int color = cc.Pixel();
      for(int y=0;y<sy;y++)
	for(int x=0;x<sx;x++)
	  {
	    img.buffer[x+y*img.ydelta] = color; 
	  }
      BufferRef::FreeBuffer(buffer[i]);
      buffer[i] = img;
  }
  void Prepare2(std::string url, int i)
  {
    //std::cout << "MTL:Prepare2: " << url << " " << i << std::endl;
    GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
      if (!vec) {
	std::cout <<  "Async failing at: mtl prepare2: " << url << std::endl;
      }
      bool b = false;
      std::vector<unsigned char> vec2(vec->begin(), vec->end());
      delete vec;
      BufferRef img = LoadImageFromString(vec2,b);
      //std::cout << "NetworkedFaceCollectionMTL2(Prepare2)::" << img.width << "x" << img.height << "=" << MB(img.width*img.height*sizeof(unsigned int)) << std::endl;
      
      // flip texture in y-direction
      int sx = img.width;
      int sy = img.height;
      for(int y=0;y<sy/2;y++)
	for(int x=0;x<sx;x++)
	  {
	    std::swap(img.buffer[x+y*img.ydelta],img.buffer[x+(sy-y-1)*img.ydelta]);
	  }

      BufferRef::FreeBuffer(buffer[i]);
      buffer[i] = img;
      //std::cout << "p_mtl prepare2 " << url << " " << i << std::endl;

#ifdef EMSCRIPTEN
      if (flags[i]==1) { async_pending_minus("texid_material",url); async_pending_count--; flags[i]=0; } else { std::cout << "flags[i] failure " << i << " " << url << std::endl; }
      //std::cout << "NetworkedFaceCollectionMTL2(flags) asyncpending--" << async_pending_count << std::endl;
#endif

  }

  void PrepareD(std::string url, int i)
  {
    //std::cout << "MTL:PrepareD: " << url << " " << i << std::endl;
    GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
      if (!vec) {
	std::cout <<  "Async failing at: mtl prepareD: " << url << std::endl;
      }
      bool b = false;
      std::vector<unsigned char> vec2(vec->begin(), vec->end());
      delete vec;
      BufferRef img = LoadImageFromString(vec2,b);
      //std::cout << "NetworkedFaceCollectionMTL2(PrepareD)::" << img.width << "x" << img.height << "=" << MB(img.width*img.height*sizeof(unsigned int)) << std::endl;
      
      // flip texture in y-direction
      int sx = img.width;
      int sy = img.height;
      for(int y=0;y<sy/2;y++)
	for(int x=0;x<sx;x++)
	  {
	    std::swap(img.buffer[x+y*img.ydelta],img.buffer[x+(sy-y-1)*img.ydelta]);
	  }

      BufferRef::FreeBuffer(d_buffer[i]);
      d_buffer[i] = img;
      //std::cout << "p_mtl prepare2 " << url << " " << i << std::endl;

#ifdef EMSCRIPTEN
      if (d_flags[i]==1) { async_pending_minus("load_d",url); async_pending_count--; d_flags[i]=0; }
      //std::cout << "NetworkedFaceCollectionMTL2(d_flags) asyncpending--" << async_pending_count << std::endl;
#endif

  }

  void PrepareBump(std::string url, int i)
  {
    
    //std::cout << "MTL:PrepareBump: " << url << " " << i << std::endl;
    GameApi::ASyncVec *vec = e.get_loaded_async_url(url);
      if (!vec) {
	std::cout <<  "Async failing at: mtl prepareBump: " << url << std::endl;
      }

      bool b = false;
      std::vector<unsigned char> vec2(vec->begin(), vec->end());
      BufferRef img = LoadImageFromString(vec2,b);
      //std::cout << "NetworkedFaceCollectionMTL2(PrepareBump)::" << img.width << "x" << img.height << "=" << MB(img.width*img.height*sizeof(unsigned int)) << std::endl;

      // flip texture in y-direction
      int sx = img.width;
      int sy = img.height;
      for(int y=0;y<sy/2;y++)
	for(int x=0;x<sx;x++)
	  {
	    std::swap(img.buffer[x+y*img.ydelta],img.buffer[x+(sy-y-1)*img.ydelta]);
	  }

      BufferRef::FreeBuffer(bump_buffer[i]);
      bump_buffer[i] = img;
      //std::cout << "p_mtl prepare2 " << url << " " << i << std::endl;

#ifdef EMSCRIPTEN
      if (bump_flags[i]==1) { async_pending_minus("load_bump",url); async_pending_count--; bump_flags[i]=0; }
      //std::cout << "NetworkedFaceCollectionMTL2(bump_flags) asyncpending--" << async_pending_count << std::endl;

#endif

  }
  void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }
  
  void Prepare()
  {
    //std::cout << "MTL:Prepare()" << std::endl;
    if (current == empty) {
      if (url.size()>2 && url[url.size()-3]=='.' && url[url.size()-2]=='d' &&url[url.size()-1]=='s') {
#ifndef EMSCRIPTEN
	e.async_load_url(mtl_url, homepage);
	PrepareMTL();
#endif
  int c = get_current_block();
  set_current_block(-1);

	GameApi::P p = ev.polygon_api.p_ds_url(ev,url);
  set_current_block(c);
	
	FaceCollection *coll = find_facecoll(e,p);
	filled = coll;
	current = filled;
	current->Prepare();
      } else {

      
#ifdef HAS_POPEN
	delete stream;
	stream = load_from_url_stream(url);

#ifndef EMSCRIPTEN
    e.async_load_url(mtl_url, homepage);
      PrepareMTL();
#endif

#else
#ifndef EMSCRIPTEN
      //std::cout << "AA" << std::endl;
    e.async_load_url(url, homepage);
    //std::cout << "BB" << std::endl;
    e.async_load_url(mtl_url, homepage);
    //std::cout << "CC" << std::endl;
      PrepareMTL();
      //std::cout << "DD" << std::endl;
#endif

      GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_mtl .obj async not ready yet, failing..." << std::endl;
      return;
    }
    static int num=0;
    num++;
    std::stringstream ss2;
    ss2 << "p_mtl_file" << num << ".obj";
    std::string start="";
#ifdef WINDOWS
  std::string drive = getenv("systemdrive");
  std::string path = getenv("homepath");
  start=drive+path+"\\";
#endif
#ifdef LINUX
  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:"";
  std::string home = getenv("HOME");
  home+="/";
  if (dockerdir!="") home=dockerdir;
  start = home + ".gameapi_builder";
  std::string cmd = "mkdir -p " + start;
  int val = system(cmd.c_str());
  start+="/";
#endif

  std::string filename = start+ss2.str();
    //std::cout << "Saving: " << filename << std::endl;
    //std::fstream ss(filename.c_str(), std::ios_base::binary |std::ios_base::out);
    //int s = ptr->size();
    //for(int i=0;i<s;i++) ss.put(ptr->operator[](i));
    //ss.write(&ptr->operator[](0),ptr->size());
    //ss.close();
    std::vector<unsigned char, GameApiAllocator<unsigned char> > vec(ptr->begin(),ptr->end());
    delete stream;
    stream = load_from_vector(vec);
#endif
  int c = get_current_block();
  set_current_block(-1);
    
  GameApi::P p = ev.polygon_api.load_model_all_no_cache_mtl(stream, count,material_names,nr);
    set_current_block(c);
    FaceCollection *coll = find_facecoll(e, p);
    filled = coll;
    current = filled;
    current->Prepare();
      }
    }
  }

  virtual bool has_normal() const { return current->has_normal(); }
  virtual bool has_attrib() const { return current->has_attrib(); }
  virtual bool has_color() const { return current->has_color(); }
  virtual bool has_texcoord() const { return current->has_texcoord(); }
  virtual bool has_skeleton() const { return current->has_skeleton(); }

  
  virtual int NumFaces() const { return current->NumFaces(); }
  virtual int NumPoints(int face) const { return current->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return current->FacePoint(face,point); }
  virtual Point EndFacePoint(int face, int point) const { return current->EndFacePoint(face, point); }

  virtual Vector PointNormal(int face, int point) const { return current->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return current->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return current->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return current->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return current->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return current->TexCoord3(face,point); }

  virtual int NumObjects() const { return current->NumObjects(); }
  virtual std::pair<int,int> GetObject(int o) const { return current->GetObject(o); }

  
  virtual int NumTextures() const { return buffer.size(); }
  virtual void GenTexture(int num) {
    //int s = buffer.size();
    //if (num>=0 && num<s)
    //  if (buffer[num].buffer==0) std::cout << "ERROR, p_mtl texture not ready!" << std::endl;
  }
  virtual BufferRef TextureBuf(int num) const {
    int s = buffer.size();
    if (num>=0 && num < s)
      return buffer[num];
    BufferRef ref = BufferRef::NewBuffer(1,1);
    return ref;
  }
  virtual int FaceTexture(int face) const { return current->FaceTexture(face); }
 
public:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  std::string mtl_url;
  std::string url_prefix;
  int count;
  FaceCollection *empty;
  FaceCollection *filled;
  FaceCollection *current;
  bool async_taken;
  std::vector<BufferRef> buffer;
  std::vector<BufferRef> d_buffer;
  std::vector<BufferRef> bump_buffer;
  std::vector<std::string> material_names;
  std::vector<GameApi::MaterialDef> materials;
  std::vector<int> flags;
  std::vector<int> d_flags;
  std::vector<int> bump_flags;
  bool done_mtl;

  std::vector<std::string> d_url;
  std::vector<std::string> bump_url;
  std::vector<CBData*> del_vec;
  bool load_d, load_bump;
  LoadStream *stream = 0;
  int done_count=1;
  bool nr;
};

void MTL_CB(void *data)
{
  CBData *dt = (CBData*)data;
  dt->t->Prepare2(dt->url, dt->i);
}
void MTL_d_CB(void *data)
{
  CBData *dt = (CBData*)data;
  dt->t->PrepareD(dt->d_url,dt->i);
}
void MTL_bump_CB(void *data)
{
  CBData *dt = (CBData*)data;
  dt->t->PrepareBump(dt->bump_url,dt->i);
}


void MTL2_CB(void *data)
{
    NetworkedFaceCollectionMTL2 *dt = (NetworkedFaceCollectionMTL2*)data;
    dt->PrepareMTL();
}

GameApi::ARR GameApi::PolygonApi::comb_mat(GameApi::EveryApi &ev, std::vector<MT> vec1, std::vector<MT> vec2)
{

  ArrayType *array = new ArrayType;
  array->type=0;
  int s = std::min(vec1.size(),vec2.size());
  for(int i=0;i<s;i++) {
    array->vec.push_back(ev.materials_api.combine_materials(ev,vec1[i],vec2[i]).id);
  }
  
  return add_array(e,array);

}
GameApi::ARR GameApi::PolygonApi::p_mtl_materials(GameApi::EveryApi &ev, P p)
{
#if (FEATURE_GLTF==1)
  FaceCollection *coll = find_facecoll(e,p);
  coll->Prepare();
#ifndef EMSCRIPTEN
  PrepareCache *coll2 = dynamic_cast<PrepareCache*>(coll);
#else
  PrepareCache *coll2 = static_cast<PrepareCache*>(coll);
#endif

  FaceCollection *coll3 = coll2->get_coll2();
  


#ifndef EMSCRIPTEN
  NetworkedFaceCollectionMTL2 *coll4 = dynamic_cast<NetworkedFaceCollectionMTL2*>(coll3);
#else
  NetworkedFaceCollectionMTL2 *coll4 = static_cast<NetworkedFaceCollectionMTL2*>(coll3);
#endif
  if (!coll4) {
    //std::cout << "ERROR: dynamic_cast failed in p_mtl_d()" << std::endl;
    ArrayType *array = new ArrayType;
    array->type=0;
    return add_array(e,array);
  }
  std::vector<GameApi::MaterialDef> vec = coll4->materials;

  ArrayType *array = new ArrayType;
  array->type=0;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    GameApi::MaterialDef mat = vec[i];
    //std::cout << "Material: Ni=" << mat.Ni << ", Ns=" << mat.Ns << ", Kd=(" << mat.Kd_x << "," << mat.Kd_y << "," << mat.Kd_z << "), d="<< mat.d << std::endl;
    GameApi::MT m = ev.materials_api.gltf_material3(ev,mat.Ni,mat.Ns,mat.Kd_x,mat.Kd_y,mat.Kd_z,1.0, mat.d,1.0,1.0, 0.0, 0.0, -400.0);
    array->vec.push_back(m.id);
  }
  return add_array(e,array);
#endif
  
}

GameApi::ARR GameApi::PolygonApi::p_mtl2_materials(GameApi::EveryApi &ev, P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  coll->Prepare();
#ifndef EMSCRIPTEN
  PrepareCache *coll2 = dynamic_cast<PrepareCache*>(coll);
#else
  PrepareCache *coll2 = static_cast<PrepareCache*>(coll);
#endif

  FaceCollection *coll3 = coll2->get_coll2();
  


#ifndef EMSCRIPTEN
  NetworkedFaceCollectionMTL2 *coll4 = dynamic_cast<NetworkedFaceCollectionMTL2*>(coll3);
#else
  NetworkedFaceCollectionMTL2 *coll4 = static_cast<NetworkedFaceCollectionMTL2*>(coll3);
#endif
  if (!coll4) {
    //std::cout << "ERROR: dynamic_cast failed in p_mtl_d()" << std::endl;
    ArrayType *array = new ArrayType;
    array->type=0;
    return add_array(e,array);
  }
  std::vector<GameApi::MaterialDef> vec = coll4->materials;

  ArrayType *array = new ArrayType;
  array->type=0;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    GameApi::MaterialDef mat = vec[i];
    //GameApi::MT m = ev.materials_api.gltf_material3(ev,mat.Ni,mat.Ns,mat.Kd_x,mat.Kd_y,mat.Kd_z,1.0, mat.d);
    GameApi::MT m0 = ev.materials_api.m_def(ev);
    unsigned int ambient = 0xff000000 + (int(mat.Ka_x*255.0)<<16) + (int(mat.Ka_y*255.0)<<8) + int(mat.Ka_z*255.0);
    unsigned int diffuse = 0xff000000 + (int(mat.Kd_x*255.0)<<16) + (int(mat.Kd_y*255.0)<<8) + int(mat.Kd_z*255.0);
    unsigned int specular = 0xff000000 + (int(mat.Ks_x*255.0)<<16) + (int(mat.Ks_y*255.0)<<8) + int(mat.Ks_z*255.0);
    GameApi::MT m = ev.materials_api.phong2(ev, m0, 1.0,1.0,1.0,ambient, diffuse, specular, mat.Ns);
    array->vec.push_back(m.id);
  }
  return add_array(e,array);
  
}


GameApi::ARR GameApi::PolygonApi::p_mtl_d(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
#ifndef EMSCRIPTEN
  PrepareCache *coll2 = dynamic_cast<PrepareCache*>(coll);
#else
  PrepareCache *coll2 = static_cast<PrepareCache*>(coll);
#endif

  FaceCollection *coll3 = coll2->get_coll2();
  


#ifndef EMSCRIPTEN
  NetworkedFaceCollectionMTL2 *coll4 = dynamic_cast<NetworkedFaceCollectionMTL2*>(coll3);
#else
  NetworkedFaceCollectionMTL2 *coll4 = static_cast<NetworkedFaceCollectionMTL2*>(coll3);
#endif
  if (!coll4) {
    //std::cout << "ERROR: dynamic_cast failed in p_mtl_d()" << std::endl;
    ArrayType *array = new ArrayType;
    array->type=0;
    return add_array(e,array);
  }

  std::vector<BufferRef> d_buf = coll4->d_buffer;
  int s = d_buf.size();
  ArrayType *array = new ArrayType;
  array->type=0;
  for(int i=0;i<s;i++) {
    BitmapFromBuffer *bm2 = new BitmapFromBuffer(d_buf[i]);
    BitmapColorHandle *handle2 = new BitmapColorHandle;
    handle2->bm = bm2;
    BM bm = add_bitmap(e, handle2);
    array->vec.push_back(bm.id);
  }
  return add_array(e,array);
}

GameApi::ARR GameApi::PolygonApi::p_mtl_bump(P p)
{
  FaceCollection *coll = find_facecoll(e,p);


#ifndef EMSCRIPTEN
  PrepareCache *coll2 = dynamic_cast<PrepareCache*>(coll);
#else
  PrepareCache *coll2 = static_cast<PrepareCache*>(coll);
#endif

  FaceCollection *coll3 = coll2->get_coll2();
  


#ifndef EMSCRIPTEN
  NetworkedFaceCollectionMTL2 *coll4 = dynamic_cast<NetworkedFaceCollectionMTL2*>(coll3);
#else
  NetworkedFaceCollectionMTL2 *coll4 = static_cast<NetworkedFaceCollectionMTL2*>(coll3);
#endif

  if (!coll4) {
    //std::cout << "ERROR: dynamic_cast failed in p_mtl_d()" << std::endl;
    ArrayType *array = new ArrayType;
    array->type=0;
    return add_array(e,array);
  }
  std::vector<BufferRef> bump_buf = coll4->bump_buffer;
  int s = bump_buf.size();
  ArrayType *array = new ArrayType;
  array->type=0;
  for(int i=0;i<s;i++) {
    BitmapFromBuffer *bm2 = new BitmapFromBuffer(bump_buf[i]);
    BitmapColorHandle *handle2 = new BitmapColorHandle;
    handle2->bm = bm2;
    BM bm = add_bitmap(e, handle2);
    array->vec.push_back(bm.id);
  }
  return add_array(e,array);
}



class NetworkedFaceCollectionDS : public FaceCollection
{
public:
  NetworkedFaceCollectionDS(GameApi::Env &e, GameApi::EveryApi &ev, FaceCollection *empty, std::string url, std::string homepage) : e(e), ev(ev), url(url), homepage(homepage), empty(empty)
  {
    current = empty;
    filled = 0;
  }
  virtual std::string name() const { return "NetworkedFaceCollectionDS"; }

  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  void PrepareDone()
  {
    //std::cout << "DEL" << std::endl;
    // TODO HOW TO DELETE THESE?
    //if (ptr) {
    //  ptr->del();
    // delete ptr;
    //  ptr=0;
    //}
  }
  void Prepare()
  {
    if (current==empty) {
#ifndef EMSCRIPTEN
      e.async_load_url(url, homepage);
#endif
      ptr = e.get_loaded_async_url(url);
      if (!ptr) {
	std::cout << "p_ds_url async not ready yet, failing..." << std::endl;
	return;
      }
      //std::vector<unsigned char> vec2(ptr->begin(), ptr->end());
  int c = get_current_block();
  set_current_block(-1);
      GameApi::P p = ev.polygon_api.p_ds(ev,ptr->begin(),ptr->end());
      //delete ptr;
      set_current_block(c);
      FaceCollection *coll = find_facecoll(e,p);
      coll->Prepare();
      filled = coll;
      current = filled;
    }
  }

  virtual bool has_normal() const { return current->has_normal(); }
  virtual bool has_attrib() const { return current->has_attrib(); }
  virtual bool has_color() const { return current->has_color(); }
  virtual bool has_texcoord() const { return current->has_texcoord(); }
  virtual bool has_skeleton() const { return current->has_skeleton(); }

  virtual int NumFaces() const { return current->NumFaces(); }
  virtual int NumPoints(int face) const { return current->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const { return current->FacePoint(face,point); }
  virtual Point EndFacePoint(int face, int point) const { return current->EndFacePoint(face, point); }

  virtual Vector PointNormal(int face, int point) const { return current->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return current->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return current->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return current->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return current->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return current->TexCoord3(face,point); }
  virtual int NumObjects() const { return current->NumObjects(); }
  virtual std::pair<int,int> GetObject(int o) const { return current->GetObject(o); }
  
  virtual int NumTextures() const { return current->NumTextures(); }
  virtual void GenTexture(int num) { current->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const { return current->TextureBuf(num); }
  virtual int FaceTexture(int face) const { return current->FaceTexture(face); }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  FaceCollection *empty;
  FaceCollection *filled;
  FaceCollection *current;
  GameApi::ASyncVec *ptr=0;
};



IMPORT extern std::string gameapi_homepageurl;
EXPORT GameApi::P GameApi::PolygonApi::p_url(EveryApi &ev, std::string url, int count)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollection(e,ev, emp, url, gameapi_homepageurl, count,false),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll),1);
  set_current_block(c);
  return p2;
}
EXPORT GameApi::P GameApi::PolygonApi::p_url_nr(EveryApi &ev, std::string url, int count)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollection(e,ev, emp, url, gameapi_homepageurl, count,true),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll),1);
  set_current_block(c);
  return p2;
}
EXPORT GameApi::P GameApi::PolygonApi::p_url_mtl(EveryApi &ev, std::string url, int count, std::vector<std::string> material_names)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollectionMTL(e,ev, emp, url, gameapi_homepageurl, count,material_names,false),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll),1);
  set_current_block(c);
  return p2;
}
extern int g_script_hash;
EXPORT GameApi::P GameApi::PolygonApi::p_mtl(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  std::stringstream hash;
  hash << g_script_hash;
  std::string key = obj_url + mtl_url + prefix + hash.str();
  bool cached = find_data(key)!=-1;
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollectionMTL2(e,ev, emp, obj_url, gameapi_homepageurl, count,mtl_url,prefix,cached,false,false,false),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,key,coll),1);
  set_current_block(c);
  return p2; // PREPARECACHE DISABLED.
}
EXPORT GameApi::P GameApi::PolygonApi::p_mtl_nr(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  std::stringstream hash;
  hash << g_script_hash;
  std::string key = obj_url + mtl_url + prefix + hash.str();
  bool cached = find_data(key)!=-1;
  FaceCollection *emp = find_facecoll(e, p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollectionMTL2(e,ev, emp, obj_url, gameapi_homepageurl, count,mtl_url,prefix,cached,false,false,true),1); 
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,key,coll),1);
  set_current_block(c);
  return p2; // PREPARECACHE DISABLED.
}

EXPORT GameApi::P GameApi::PolygonApi::p_ds_url(EveryApi &ev, std::string url)
{
  int c = get_current_block();
  set_current_block(-1);
  P p = p_empty();
  FaceCollection *emp = find_facecoll(e,p);
  GameApi::P p1 = add_polygon2(e, new NetworkedFaceCollectionDS(e,ev,emp,url,gameapi_homepageurl),1);
  FaceCollection *coll = find_facecoll(e,p1);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll),1);
  set_current_block(c);

  return p2;
}

LoadStream *load_from_vector(std::vector<unsigned char, GameApiAllocator<unsigned char> > vec);

EXPORT GameApi::P GameApi::PolygonApi::load_model(std::string filename, int num)
{
  std::ifstream data(filename.c_str());
  std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
  char c;
  while(data.get(c)) vec2.push_back(c);

  LoadStream *stream = load_from_vector(vec2);
  
  int c2 = get_current_block();
  set_current_block(-1);

  ObjFileParser *parser = new ObjFileParser(stream, -1, std::vector<std::string>());
  
  GameApi::P model = add_polygon2(e, new ObjFileFaceCollection(*parser, num),1);
  
      //GameApi::P model = add_polygon2(e, new LoadObjModelFaceCollection(stream, num, std::vector<std::string>()), 1);
  GameApi::P cache = model; //file_cache(model, filename,num);
  GameApi::P resize = resize_to_correct_size(cache);
  set_current_block(c2);
  return resize;
} 
class SaveModel : public MainLoopItem
{
public:
  SaveModel(GameApi::Env &e, GameApi::PolygonApi &api, GameApi::P poly, std::string filename) : env(e), api(api), poly(poly), filename(filename)
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    FaceCollection *coll = find_facecoll(env,poly);
    coll->Collect(vis);
    vis.register_obj(this);    
  }
  void HeavyPrepare() { Prepare(); }
  void Prepare() {
    const char *dd = getenv("BUILDER_DOCKER_DIR");
    std::string dockerdir = dd?dd:"";
	std::string home = getenv("HOME");
	home+="/";
	if (dockerdir!="") home=dockerdir;
	std::string path = home + ".gameapi_builder/";
    
      std::cout << "Saving " << path+filename << std::endl;
      api.save_model(poly, path+filename);

      	std::ifstream ss((path+filename).c_str());
        char ch;
	std::vector<unsigned char> vec;
	while(ss.get(ch)) { vec.push_back(ch); }
	ss.close();
	int bar = env.add_to_download_bar(filename);
	int ii = env.download_index_mapping(bar);
	env.set_download_data(ii, vec);
	env.set_download_ready(ii);
  }
  void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      firsttime = false;
    }
  }
  std::vector<int> shader_id() { return std::vector<int>(); }

private:
  GameApi::Env &env;
  GameApi::PolygonApi &api;
  GameApi::P poly;
  std::string filename;
  bool firsttime;
};
EXPORT GameApi::ML GameApi::PolygonApi::save_model_ml(GameApi::P poly, std::string filename)
{
  return add_main_loop(e, new SaveModel(e,*this, poly, filename));
}
EXPORT void GameApi::PolygonApi::save_model(GameApi::P poly, std::string filename)
{
  FaceCollection *face = find_facecoll(e, poly);
  face->Prepare();
  SaveObjModelFaceCollection save(face);
  save.save(filename);
}
   
#if 0
EXPORT GameApi::P GameApi::PolygonApi::line(PT p1, PT p2)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p2);
  FaceCollection *coll = new TriElem(*pp1, *pp2, (*pp3)+Vector(1.0,1.0,1.0));
  return add_polygon(e, coll,1);
}
#endif
class TexCoordQuadFaceCollection : public ForwardFaceCollection
{
public:
  TexCoordQuadFaceCollection(FaceCollection &coll, TexCoordQuad q_, int sx, int sy) : ForwardFaceCollection(coll), q(q_) //,sx(sx),sy(sy) 
  {
    q.p1.x /= float(sx);
    q.p1.y /= float(sy);
    q.p2.x /= float(sx);
    q.p2.y /= float(sy);
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
  }
  virtual std::string name() const { return "TexCoordQuadFaceCollection"; }

  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const 
  { 
    //std::cout << "TexCoord" << face << " " << point << std::endl;
    //std::cout << "P1: " << q.p1 << " P2: " << q.p2 << std::endl;
    if (point==0) {
      return q.p1;
    }
    if (point==1) {
      Point2d p;
      p.x = q.p2.x;
      p.y = q.p1.y;
      return p;
    }
    if (point==2) {
      return q.p2;
    }
    if (point==3) {
      Point2d p;
      p.x = q.p1.x;
      p.y = q.p2.y;
      return p;
    }
    Point2d pp;
    pp.x = 0.0;
    pp.y = 0.0;
    return pp;
  }
private:
  TexCoordQuad q;
  //int sx,sy;
};

class TexCoordSpherical : public ForwardFaceCollection
{
public:
  TexCoordSpherical(Point center, FaceCollection *coll) : ForwardFaceCollection(*coll), center(center), coll(coll) { }
  bool has_texcoord() const { return true; }
  virtual std::string name() const { return "TexCoordSpherical"; }
  
  virtual Point2d TexCoord(int face, int point) const { 
    if (point<0||point>=ForwardFaceCollection::NumPoints(face)) { Point2d p; p.x=0.0; p.y=0.0; std::cout << "stub" << std::endl; return p; }
    Point p0 = ForwardFaceCollection::FacePoint(face,0);
      Point p1 = ForwardFaceCollection::FacePoint(face,1);
      Point p2 = ForwardFaceCollection::FacePoint(face,2);
      Point p3 = ForwardFaceCollection::FacePoint(face,3%ForwardFaceCollection::NumPoints(face));
      p0-=Vector(center);
      p1-=Vector(center);
      p2-=Vector(center);
      p3-=Vector(center);
      float r0 = sqrt(p0.x*p0.x+p0.y*p0.y+p0.z*p0.z);
      float r1 = sqrt(p1.x*p1.x+p1.y*p1.y+p1.z*p1.z);
      float r2 = sqrt(p2.x*p2.x+p2.y*p2.y+p2.z*p2.z);
      float r3 = sqrt(p3.x*p3.x+p3.y*p3.y+p3.z*p3.z);
      float alfa0 = acos(p0.z/r0);
      float beta0 = atan2(p0.y,p0.x);
      float alfa1 = acos(p1.z/r1);
      float beta1 = atan2(p1.y,p1.x);
      float alfa2 = acos(p2.z/r2);
      float beta2 = atan2(p2.y,p2.x);
      float alfa3 = acos(p3.z/r3);
      float beta3 = atan2(p3.y,p3.x);
      alfa0/=2.0*3.14159;
      beta0/=2.0*3.14159;
      alfa1/=2.0*3.14159;
      beta1/=2.0*3.14159;
      alfa2/=2.0*3.14159;
      beta2/=2.0*3.14159;
      alfa3/=2.0*3.14159;
      beta3/=2.0*3.14159;

      bool alfa0b = alfa0<0.7;
      bool beta0b = beta0<0.7;
      bool alfa1b = alfa0<0.7;
      bool beta1b = beta0<0.7;
      bool alfa2b = alfa0<0.7;
      bool beta2b = beta0<0.7;
      bool alfa3b = alfa0<0.7;
      bool beta3b = beta0<0.7;

      float alfa_arr[] = { alfa0, alfa1, alfa2, alfa3 };
      float beta_arr[] = { beta0, beta1, beta2, beta3 };
      bool alfa_b_arr[] = { alfa0b, alfa1b,alfa2b,alfa3b };
      bool beta_b_arr[] = { beta0b, beta1b,beta2b,beta3b };
      
      float alfa = alfa_arr[point];
      float beta = beta_arr[point];

      float alfa_b = alfa_arr[point] <0.3;
      float beta_b = beta_arr[point] <0.3;
      
      if (alfa_b != alfa0b) alfa+=1.0;
      else if (alfa_b != alfa1b) alfa+=1.0;
      else if (alfa_b != alfa2b) alfa+=1.0;
      else if (alfa_b != alfa3b) alfa+=1.0;

      if (beta_b != beta0b) beta+=1.0;
      else if (beta_b != beta1b) beta+=1.0;
      else if (beta_b != beta2b) beta+=1.0;
      else if (beta_b != beta3b) beta+=1.0;
      
      Point2d pp;
      pp.x = alfa;
      pp.y = beta;
      std::cout << "texcoord: " << pp.x << " " << pp.y << std::endl;
      return pp;
    }
private:
  Point center;
  FaceCollection *coll;
};

#if 0
class TexCoordCylindar : public ForwardFaceCollection
{
public:
  TexCoordCylindar(FaceCollection *coll, float start_y, float end_y) : ForwardFaceCollection(*coll), coll(coll), start_y(start_y), end_y(end_y) { }
  bool has_texcoord() const { return true; }

  virtual Point2d TexCoord(int face, int point) const { 
      Point p = ForwardFaceCollection::FacePoint(face,point);
      //float r = sqrt(p.x*p.x+p.z*p.z);
      float alfa = atan2(p.z, p.x);
      alfa /= 2.0*3.14159;
      float z = p.y - start_y;
      z/=(end_y-start_y);
      Point2d pp;
      pp.x = alfa;
      pp.y = z;
      return pp;
    }
private:
  FaceCollection *coll;
  float start_y, end_y;
};
#endif
class TexCoordManual : public ForwardFaceCollection
{
public:
  TexCoordManual(FaceCollection *coll,
		 float p1_x, float p1_y,
		 float p2_x, float p2_y,
		 float p3_x, float p3_y,
		 float p4_x, float p4_y) : ForwardFaceCollection(*coll),
					   p1_x(p1_x), p1_y(p1_y),
					   p2_x(p2_x), p2_y(p2_y),
					   p3_x(p3_x), p3_y(p3_y),
					   p4_x(p4_x), p4_y(p4_y) { }

  virtual std::string name() const { return "TexCoordManual"; }

  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  {
    switch(point) {
    case 0:
      {
      Point2d p1 = { p1_x, p1_y };
      return p1;
      }
    case 1:
      {
      Point2d p2 = { p2_x, p2_y };
      return p2;
      }
    case 2:
      {
      Point2d p3 = { p3_x, p3_y };
      return p3;
      }
    case 3:
      {
      Point2d p4 = { p4_x, p4_y };
      return p4;
      }
    };
    Point2d pp = { 0.0, 0.0 };
    return pp;
  }
private:
  float p1_x, p1_y;
  float p2_x, p2_y;
  float p3_x, p3_y;
  float p4_x, p4_y;
					   
};
class TexCoordPlane : public ForwardFaceCollection
{
public:
  TexCoordPlane(FaceCollection *coll, float start_x, float end_x, float start_y, float end_y) : ForwardFaceCollection(*coll), coll(coll), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
  virtual std::string name() const { return "TexCoordPlane"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    find_bounding_box();
  }
  void find_bounding_box()
  {

    a_start_x = 300000.0;
      a_start_y = 300000.0;
      a_start_z = 300000.0;
      a_end_x =  -300000.0;
      a_end_y =  -300000.0;
      a_end_z =  -300000.0; 

    int s = std::min(coll->NumFaces(),100);
    if (s<1) s=1;
    int step = coll->NumFaces()/s;
    int faces = coll->NumFaces();
    for(int i=0;i<faces;i+=step)
      {
	    Point p1 = coll->FacePoint(i,0);
	    Point p2 = coll->FacePoint(i,1);
	    Point p3 = coll->FacePoint(i,2);
	    Point p4 = coll->NumPoints(i)==4 ? coll->FacePoint(i,3) : p1;

	    handlepoint(p1);
	    handlepoint(p2);
	    handlepoint(p3);
	    handlepoint(p4);
      }
  }
  void handlepoint(Point p)
  {
    if (p.x<a_start_x) { a_start_x = p.x; }
    if (p.y<a_start_y) { a_start_y = p.y; }
    if (p.z<a_start_z) { a_start_z = p.z; }
    if (p.x>a_end_x) { a_end_x = p.x; }
    if (p.y>a_end_y) { a_end_y = p.y; }
    if (p.z>a_end_z) { a_end_z = p.z; }    
  }

  void Prepare()
  {
    coll->Prepare();
    HeavyPrepare();
  }
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    p.x-=a_start_x;
    p.x/=(a_end_x-a_start_x);
    p.x*=400.0;
    p.y-=a_start_y;
    p.y/=(a_end_y-a_start_y);
    p.y*=400.0;
    
    p.x-=start_x;
    p.x/=end_x-start_x;
    p.y-=start_y;
    p.y/=end_y-start_y;
    Point2d pp;
    pp.x = p.x;
    pp.y = p.y;
    return pp;
  }  
private:
  FaceCollection *coll;
  float start_x, end_x;
  float start_y, end_y;
  float a_start_x, a_end_x;
  float a_start_y, a_end_y;
  float a_start_z, a_end_z;
};

EXPORT GameApi::P GameApi::PolygonApi::texcoord_plane(P orig,
						      float start_x, float end_x,
						      float start_y, float end_y)
{
  FaceCollection *face = find_facecoll(e,orig);
  return add_polygon(e, new TexCoordPlane(face, start_x, end_x, start_y, end_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_manual(P orig, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float p4_x, float p4_y)
{
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon(e, new TexCoordManual(face, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_spherical(PT center, P orig)
{
  Point *center_1 = find_point(e, center);
  FaceCollection *face = find_facecoll(e, orig);
  
  return add_polygon2(e, new TexCoordSpherical(*center_1, face),1);
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_cylindar(P orig, float start_y, float end_y)
{
#if 0
  FaceCollection *face = find_facecoll(e, orig);
  return add_polygon(e, new TexCoordCylindar(face,start_y, end_y),1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, TX tx, int id)
{
  TextureApi t(e);
  Q q = t.get_tex_coord_1(tx, id);
  return sprite_bind_1(p, q, tx);
}
EXPORT GameApi::P GameApi::PolygonApi::sprite_bind(P p, Q q, TX tx)
{
  return sprite_bind_1(p,q,tx);
}
GameApi::P GameApi::PolygonApi::sprite_bind_1(P p, Q q, TX tx)
{
  TextureI *texture = find_texture(e, tx);
  int sx = texture->SizeX();
  int sy = texture->SizeY();
  FaceCollection *pp1 = find_facecoll(e, p);
  TexCoordQuad qq = find_tex_quad(e, q);
  return add_polygon(e, new TexCoordQuadFaceCollection(*pp1, qq, sx,sy),1);
}

EXPORT GameApi::P GameApi::PolygonApi::quad(PT p1, PT p2, PT p3, PT p4)
{
  Point *pp1 = find_point(e, p1);
  Point *pp2 = find_point(e, p2);
  Point *pp3 = find_point(e, p3);
  Point *pp4 = find_point(e, p4);
  FaceCollection *coll = new QuadElem(*pp1, *pp2, *pp3, *pp4);
  return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_x(float x,
		  float y1, float y2,
		  float z1, float z2)
{
  Point pp1 = Point(x,y1,z1);
  Point pp2 = Point(x,y2,z1);
  Point pp3 = Point(x,y2,z2);
  Point pp4 = Point(x,y1,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quad_y(float x1, float x2,
		  float y,
		  float z1, float z2)
{
  Point pp1 = Point(x1,y,z1);
  Point pp2 = Point(x2,y,z1);
  Point pp3 = Point(x2,y,z2);
  Point pp4 = Point(x1,y,z2);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::fullscreen_quad(EveryApi &ev)
{
  int sx = 800-40; //ev.mainloop_api.get_screen_sx();
  int sy = 600-40; //ev.mainloop_api.get_screen_sy();
  return quad_z(-sx,sx, -sy, sy, 0.0);
}
EXPORT GameApi::ML GameApi::PolygonApi::bg_image(EveryApi &ev, BM bm)
{
  GameApi::P I1=ev.polygon_api.fullscreen_quad(ev);
  GameApi::BM I2=bm;
  GameApi::BM I22=ev.bitmap_api.flip_y(I2);
  GameApi::MT I3=ev.materials_api.texture(ev,I22,1.0);
  GameApi::ML I4=ev.materials_api.bind(I1,I3);
#ifdef EMSCRIPTEN
  GameApi::MN I5=ev.move_api.mn_empty();
  GameApi::MN I6=ev.move_api.scale2(I5,1.5,1.5,1);
  GameApi::ML I7=ev.move_api.move_ml(ev,I4,I6,1,10.0);
  I4=I7;
#endif
  return I4;
}



class BitmapSizedQuad : public FaceCollection
{
public:
  BitmapSizedQuad(GameApi::Env &e, GameApi::PolygonApi *pl, Bitmap<::Color> *bm) : e(e), pl(pl), bm(bm) { }
  virtual std::string name() const { return "BitmapSizedQuad"; }

  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    sx/=2;
    sy/=2;
    p = pl->quad_z(-sx,sx,-sy,sy,0.0);
  }
  virtual void Prepare()
  {
    bm->Prepare();
    int sx = bm->SizeX();
    int sy = bm->SizeY();
    sx/=2;
    sy/=2;
    p = pl->quad_z(-sx,sx,-sy,sy,0.0);
  }

  virtual bool has_normal() const {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->has_normal();

  }
  virtual bool has_attrib() const {

    FaceCollection *coll = find_facecoll(e,p);
    return coll->has_attrib();

  }
  virtual bool has_color() const {

    FaceCollection *coll = find_facecoll(e,p);
    return coll->has_color();

  }
  virtual bool has_texcoord() const {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->has_texcoord();


  }
  virtual bool has_skeleton() const {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->has_skeleton();
  }

  virtual int NumFaces() const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const {
    FaceCollection *coll = find_facecoll(e,p);
    return coll->TexCoord3(face,point);
  }

private:
  GameApi::Env &e;
  GameApi::PolygonApi *pl;
  Bitmap<::Color> *bm;
  GameApi::P p;
};
EXPORT GameApi::P GameApi::PolygonApi::bitmapsized_quad(BM bm)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  return add_polygon2(e, new BitmapSizedQuad(e,this,b2),1);
}
EXPORT GameApi::P GameApi::PolygonApi::vr_fullscreen_quad(EveryApi &ev, bool is_right)
{
  //int sx = 800-40; //ev.mainloop_api.get_screen_sx();
  //int sy = 600-40; //ev.mainloop_api.get_screen_sy();
  int sx = ev.mainloop_api.get_screen_width();
  int sy = ev.mainloop_api.get_screen_height();
#ifdef EMSCRIPTEN
 if (!is_right)
   return quad_z(-sx,0, -sy/2.0, sy/2.0, 0.0);
  else
    return quad_z(0,sx,-sy/2.0,sy/2.0,0.0);
#else
  if (!is_right)
    return quad_z(-sx,0, -sy, sy, 0.0);
  else
    return quad_z(0,sx,-sy,sy,0.0);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::quad_z(float x1, float x2,
		  float y1, float y2,
		  float z)
{
  Point pp1 = Point(x1,y1,z);
  Point pp2 = Point(x2,y1,z);
  Point pp3 = Point(x2,y2,z);
  Point pp4 = Point(x1,y2,z);
  FaceCollection *coll = new QuadElem(pp1,pp2,pp3,pp4);
  return add_polygon(e,coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::rounded_cube(EveryApi &ev, float start_x, float end_x,
						    float start_y, float end_y,
						    float start_z, float end_z,
						    float r)
{
  P p110 = cube(start_x+r, end_x-r, start_y+r, end_y-r, start_z, end_z);
  P p101 = cube(start_x+r, end_x-r, start_y, end_y, start_z+r, end_z-r);
  P p011 = cube(start_x, end_x, start_y+r, end_y-r, start_z+r, end_z-r);

  PT cen111 = ev.point_api.point(start_x+r, start_y+r, start_z+r);
  P c111 = sphere(cen111, r, 60,30);

  PT cen211 = ev.point_api.point(end_x-r, start_y+r, start_z+r);
  P c211 = sphere(cen211, r, 60,30);

  PT cen121 = ev.point_api.point(start_x+r, end_y-r, start_z+r);
  P c121 = sphere(cen121, r, 60,30);

  PT cen112 = ev.point_api.point(start_x+r, start_y+r, end_z-r);
  P c112 = sphere(cen112, r, 60,30);

  PT cen122 = ev.point_api.point(start_x+r, end_y-r, end_z-r);
  P c122 = sphere(cen122, r, 60,30);

  PT cen221 = ev.point_api.point(end_x-r, end_y-r, start_z+r);
  P c221 = sphere(cen221, r, 60,30);

  PT cen212 = ev.point_api.point(end_x-r, start_y+r, end_z-r);
  P c212 = sphere(cen212, r, 60,30);

  PT cen222 = ev.point_api.point(end_x-r, end_y-r, end_z-r);
  P c222 = sphere(cen222, r, 60,30);



  P L111_211 = cone(30, cen111, cen211, r, r);
  P L111_121 = cone(30, cen111, cen121, r, r);
  P L111_112 = cone(30, cen111, cen112, r, r);

  P L222_221 = cone(30, cen222, cen221, r, r);
  P L222_122 = cone(30, cen222, cen122, r, r);
  P L222_212 = cone(30, cen222, cen212, r, r);

  P L211_221 = cone(30, cen211, cen221, r,r);
  P L211_212 = cone(30, cen211, cen212, r,r);
  P L121_221 = cone(30, cen121, cen221, r,r);
  P L121_122 = cone(30, cen121, cen122, r,r);
  P L112_212 = cone(30, cen112, cen212, r,r);
  P L112_122 = cone(30, cen112, cen122, r,r);
  

  P array[] = { p110,p101,p011, c111, c211, c121, c112, c122, c221, c212, c222,
		L111_211, L111_121, L111_112, L222_221, L222_122, L222_212,
		L211_221, L211_212, L121_221, L121_122, L112_212, L112_122
  };

  P arr = or_array(array,17+6);
  return arr;
}

EXPORT GameApi::P GameApi::PolygonApi::cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  Point p111(start_x, start_y, start_z);
  Point p112(start_x, start_y, end_z);
  Point p121(start_x, end_y, start_z);
  Point p122(start_x, end_y, end_z);
  Point p211(end_x, start_y, start_z);
  Point p212(end_x, start_y, end_z);
  Point p221(end_x, end_y, start_z);
  Point p222(end_x, end_y, end_z);
  
  FaceCollection *coll = new CubeElem(p111,p112,p121,p122,
				      p211,p212,p221,p222);
  return add_polygon2(e, coll,1);  
}

EXPORT GameApi::P GameApi::PolygonApi::cube(PT *p)
{
  Point *p111 = find_point(e,p[0]);
  Point *p112 = find_point(e,p[1]);
  Point *p121 = find_point(e,p[2]);
  Point *p122 = find_point(e,p[3]);
  Point *p211 = find_point(e,p[4]);
  Point *p212 = find_point(e,p[5]);
  Point *p221 = find_point(e,p[6]);
  Point *p222 = find_point(e,p[7]);
  
  FaceCollection *coll = new CubeElem(*p111,*p112,*p121,*p122,
				      *p211,*p212,*p221,*p222);
  return add_polygon(e, coll,1);  
}
EXPORT GameApi::P GameApi::PolygonApi::sphere(PT center, float radius, int numfaces1, int numfaces2)
{
    Point *p = find_point(e,center);
    FaceCollection *coll = new SphereElem(*p, radius, numfaces1, numfaces2);
    return add_polygon(e, coll,1);
}
EXPORT GameApi::P GameApi::PolygonApi::torus2(EveryApi &ev, int numfaces1, int numfaces2, PT center, float radius1, float radius2)
{
  V u_x = ev.vector_api.vector(1.0,0.0,0.0);
  V u_y = ev.vector_api.vector(0.0,1.0,0.0);
  V uu_x = ev.vector_api.vector(1.0,0.0,0.0);
  V uu_y = ev.vector_api.vector(0.0,0.0,1.0);
  return torus(numfaces1, numfaces2, center, u_x, u_y, radius1, uu_x, uu_y, radius2);
}
EXPORT GameApi::P GameApi::PolygonApi::torus(int numfaces1, int numfaces2, PT center, V u_x, V u_y, float radius1, V uu_x, V uu_y, float radius2)
{
  Point *cent = find_point(e, center);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);
  Vector *uu_x1 = find_vector(e, uu_x);
  Vector *uu_y1 = find_vector(e, uu_y);
  Point m_cent = cent?*cent:Point(0.0,0.0,0.0);
  Vector m_u_x1 = u_x1?*u_x1:Vector(1.0,0.0,0.0);
  Vector m_u_y1 = u_y1?*u_y1:Vector(0.0,1.0,0.0);
  Vector m_uu_x1 = uu_x1?*uu_x1:Vector(0.0,0.0,1.0);
  Vector m_uu_y1 = uu_y1?*uu_y1:Vector(0.0,-1.0,0.0);
  FaceCollection *coll = new TorusElem(numfaces1, numfaces2, m_cent, m_u_x1, m_u_y1, radius1, m_uu_x1, m_uu_y1, radius2);
  return add_polygon(e, coll, 1); 
}
EXPORT GameApi::P GameApi::PolygonApi::cone(int numfaces, PT p1, PT p2, float rad1, float rad2)
{
    Point *pp1 = find_point(e,p1);
    Point *pp2 = find_point(e,p2);
    FaceCollection *coll = new ConeElem(numfaces, *pp1, *pp2, rad1, rad2);
    return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::ring(float sx, float sy, float x, int steps)
{
#if 0
  std::pair<Point, Vector> array[] = 
    { 
      std::make_pair(Point(-sx,-sy,0.0), Vector(-1.0,-1.0,0.0)),
      std::make_pair(Point(sx,-sy,0.0), Vector(1.0, -1.0,0.0)),
      std::make_pair(Point(sx,sy,0.0), Vector(1.0, 1.0,0.0)),
      std::make_pair(Point(-sx,sy,0.0), Vector(-1.0, 1.0,0.0))
    };
  VectorArray<std::pair<Point,Vector> > *pvarray = new VectorArray<std::pair<Point,Vector> >(array, array+4);
  PointVectorCollectionConvert *pointvector = new PointVectorCollectionConvert(*pvarray);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(pvarray));
  env->deletes.push_back(std::shared_ptr<void>(pointvector));
  RingElem *ring = new RingElem(*pointvector, x, steps);
  return add_polygon(e, ring,1);
#endif
}

EXPORT GameApi::P GameApi::PolygonApi::shadow(P p, PT pos, V u_x, V u_y, V light_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *light = find_vector(e, light_vec);
  return add_polygon(e, new ShadowFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *light), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::reflection(P p, PT pos, V u_x, V u_y, V ref_vec)
{
  FaceCollection *pp = find_facecoll(e, p);
  Point *pos_1 = find_point(e, pos);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *ref = find_vector(e, ref_vec);
  return add_polygon(e, new ReflectFaceCollection(*pp, *pos_1, *uu_x, *uu_y, *ref), 1);
}


EXPORT GameApi::P GameApi::PolygonApi::or_elem(P p1, P p2)
{
  FaceCollection *pp1 = find_facecoll(e, p1);
  FaceCollection *pp2 = find_facecoll(e, p2);
  //OrElem<FaceCollection> *coll = new OrElem<FaceCollection>;
  //coll->push_back(pp1);
  //coll->push_back(pp2);
  //coll->update_faces_cache();
  FaceCollection *coll = new OrElem2(pp1,pp2);
  return add_polygon2(e, coll,1);
}
 

EXPORT GameApi::P GameApi::PolygonApi::texture(P orig, BM bm, int choose)
{
  FaceCollection *coll = find_facecoll(e, orig);
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle, choose);
  BufferFromBitmap *req = new BufferFromBitmap(*bitmap);
  SimpleTexCoords *coords = new SimpleTexCoords(*coll, 0);
  BoxableFaceCollection *coll2 = new TextureElem2(*coll, *req, *coords);
  return add_polygon(e, coll2, 1);
}

struct FaceRange
{
  int start_face;
  int end_face;
  int start_obj;
  int end_obj;
};

std::ostream &operator<<(std::ostream &o, VEC4 v)
{
  o << "(" << v.x << " " << v.y << " " << v.z << " " << v.w << ")";
}

class OrArrayNoMemory : public FaceCollection
{
public:
  OrArrayNoMemory(std::vector<FaceCollection*> vec) : vec(vec) { }
  virtual std::string name() const { return "OrArrayNoMemory"; }

  ~OrArrayNoMemory()
  {
    int s1 = m_p1.size(); for(int i=0;i<s1;i++)
			    delete [] m_p1[i];
    int s2 = m_p2.size(); for(int i=0;i<s2;i++)
			    delete [] m_p2[i];
    int s3 = m_p3.size(); for(int i=0;i<s3;i++)
			    delete [] m_p3[i];
    int s4 = m_p4.size(); for(int i=0;i<s4;i++)
			    delete [] m_p4[i];
    int s5 = m_p5.size(); for(int i=0;i<s5;i++)
			    delete [] m_p5[i];
    int s6 = m_p6.size(); for(int i=0;i<s6;i++)
			    delete [] m_p6[i];
    int s7 = m_p7.size(); for(int i=0;i<s7;i++)
			    delete [] m_p7[i];
    int s8 = m_p8.size(); for(int i=0;i<s8;i++)
			    delete [] m_p8[i];
    int s9 = m_p9.size(); for(int i=0;i<s9;i++)
			    delete [] m_p9[i];
    int s10 = m_p10.size(); for(int i=0;i<s10;i++)
			      delete [] m_p10[i];
  }
  BBOX GetBoundingBox(bool &success) const
  {
    int s = vec.size();
    BBOX bb;
    bb.start_x=30000.0;
    bb.end_x=-30000.0;
    bb.start_y=30000.0;
    bb.end_y=-30000.0;
    bb.start_z=30000.0;
    bb.end_z=-30000.0;
    success=false;
    bool success2 = true;
    for(int i=0;i<s;i++) {
      BBOX b = vec[i]->GetBoundingBox(success2);
      if (success2) {
	bb.start_x=std::min(bb.start_x,b.start_x);
	bb.start_y=std::min(bb.start_y,b.start_y);
	bb.start_z=std::min(bb.start_z,b.start_z);
	bb.end_x=std::max(bb.end_x,b.end_x);
	bb.end_y=std::max(bb.end_y,b.end_y);
	bb.end_z=std::max(bb.end_z,b.end_z);
	success=true;
      }
    }
    return bb;
  }

  virtual void Collect(CollectVisitor &vis) {
    //std::cout << "COLLECT" << std::endl;
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Collect(vis);
      }
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
    //std::cout << "HEAVYPREPARE" << std::endl;
    int s = vec.size();
    int start_face=0;
    int start_obj=0;
    ranges.clear();
    for(int i=0;i<s;i++)
      {
	int ss = vec[i]->NumFaces();
	int ss2 = vec[i]->NumObjects();
	//std::cout << "GOT: " << ss << " " << ss2 << std::endl;
	//print_objs();
	if (ss<0) { std::cout << "ss<0::" << ss << std::endl; ss=1; }
	if (ss2<0) { std::cout << "ss2<0::" << ss2 << std::endl; ss2=1; }
	if (ss>10000000) {
	  std::cout << "Warning: ss>1000000: ss=" << ss << std::endl;
	  ss=1;
	} // this is because NumFaces() gives trash if Prepare() has not been done yet.
	FaceRange r = { start_face, start_face+ss, start_obj, start_obj+ss2 };
	ranges.push_back(r);
	start_face+=ss;
	start_obj+=ss2;
      }
    firsttime=false;
  }
  //void print_objs() const {
  //  int s = vec.size();
  //  for(int i=0;i<s;i++) std::cout << typeid(*vec[i]).name() << std::endl;
  //}
  virtual void Prepare()
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i]->Prepare();
      }
    HeavyPrepare();
  }
  virtual int NumFaces() const
  {
    //std::cout << "NUMFACES" << std::endl;
    //print_objs();
    if (ranges.size()==0) const_cast<OrArrayNoMemory*>(this)->HeavyPrepare();
    if (ranges.size()==0) return 0;
    int res = ranges[ranges.size()-1].end_face;
    //std::cout << "NUMFACES END"<< res << std::endl;
    return res;
  }
  virtual int NumPoints(int face) const
  {
    //std::cout << "NUMPOINTS" << std::endl;
    if (ranges.size()==0) const_cast<OrArrayNoMemory*>(this)->HeavyPrepare();
    if (ranges.size()==0) return 0;
    std::pair<int,int> s = split(face);
    return vec[s.first]->NumPoints(s.second);
  }

  std::pair<int,int> split(int face) const
  {
    //std::cout << "SPLIT" << std::endl;
    if (ranges.size()==0) { const_cast<OrArrayNoMemory*>(this)->HeavyPrepare(); }
    int s = ranges.size();
    for(int i=0;i<s;i++)
      {
	const FaceRange &r = ranges[i];
	if (face>=r.start_face && face<r.end_face)
	  {
	    return std::make_pair(i,face-r.start_face);
	  }
      }
    return std::make_pair(0,0);
  }


  std::pair<int,int> split_obj(int o) const
  {
    //std::cout << "SPLITOBJ" << std::endl;
    if (ranges.size()==0) { const_cast<OrArrayNoMemory*>(this)->HeavyPrepare(); }
    int s = ranges.size();
    for(int i=0;i<s;i++)
      {
	const FaceRange &r = ranges[i];
	if (o>=r.start_obj && o<r.end_obj)
	  {
	    return std::make_pair(i,o-r.start_obj);
	  }
      }
    return std::make_pair(0,0);
  }

  
  
  virtual Point FacePoint(int face, int point) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->FacePoint(s.second,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->PointNormal(s.second,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->Attrib(s.second,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->AttribI(s.second,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->Color(s.second,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    std::pair<int,int> s = split(face);
    return vec[s.first]->TexCoord(s.second,point);
  }
  virtual float TexCoord3(int face, int point) const {

    std::pair<int,int> s = split(face);
    return vec[s.first]->TexCoord3(s.second,point);

  }
  virtual VEC4 Joints(int face, int point) const {

    std::pair<int,int> s = split(face);
    return vec[s.first]->Joints(s.second,point);
    
  }
  virtual VEC4 Weights(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->Weights(s.second,point);

  }


  virtual Point EndFacePoint(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndFacePoint(s.second,point);

  }
  virtual Vector EndPointNormal(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndPointNormal(s.second,point);
  }
  virtual float EndAttrib(int face, int point, int id) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndAttrib(s.second,point,id);
  }
  virtual int EndAttribI(int face, int point, int id) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndAttribI(s.second,point,id);
  }
  virtual unsigned int EndColor(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndColor(s.second,point);
  }
  virtual Point2d EndTexCoord(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndTexCoord(s.second,point);
  }
  virtual float EndTexCoord3(int face, int point) const {
    std::pair<int,int> s = split(face);
    return vec[s.first]->EndTexCoord3(s.second,point);
  }


  
  virtual int NumObjects() const {
    //  std::cout << "NUMOBJECTS" << std::endl;

    if (ranges.size()==0) const_cast<OrArrayNoMemory*>(this)->HeavyPrepare();
    if (ranges.size()==0) return 0;
    return ranges[ranges.size()-1].end_obj;
    
  }
  virtual std::pair<int,int> GetObject(int o) const {
    std::pair<int,int> s = split_obj(o);
    std::pair<int,int> oo = vec[s.first]->GetObject(s.second);
    oo.first+=ranges[s.first].start_face;
    oo.second+=ranges[s.first].start_face;
    return oo;
  }

  bool HasBatchMap() const {
    bool b = true;
    int s = vec.size();
    for(int i=0;i<s;i++) b&=vec[i]->HasBatchMap();
    return b;
  }

  unsigned char *combine_indices_char(unsigned char *p1, unsigned char *p2, int numfaces1, int numfaces2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static unsigned char *p=0;
    //delete [] m_p1;
    unsigned char *p;
    m_p1.push_back(p=new unsigned char[numfaces1*3+numfaces2*3]);
    std::copy(p1,p1+numfaces1*3,p);
    int i = 0;
    for(;i<numfaces2;i++)
      {
	p[(numfaces1+i)*3+0] = numvertices1 + p2[i*3+0];
	p[(numfaces1+i)*3+1] = numvertices1 + p2[i*3+1];
	p[(numfaces1+i)*3+2] = numvertices1 + p2[i*3+2];
      }
    return p;
  }
  unsigned short *combine_indices_short(unsigned short *p1, unsigned short *p2, int numfaces1, int numfaces2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static unsigned short *p=0;
    //delete [] m_p2;
    unsigned short *p;
    m_p2.push_back(p=new unsigned short[numfaces1*3+numfaces2*3]);
    std::copy(p1,p1+numfaces1*3,p);
    int i = 0;
    for(;i<numfaces2;i++)
      {
	p[(numfaces1+i)*3+0] = numvertices1 + p2[i*3+0];
	p[(numfaces1+i)*3+1] = numvertices1 + p2[i*3+1];
	p[(numfaces1+i)*3+2] = numvertices1 + p2[i*3+2];
	//p[(numfaces1+i)*3] = numvertices1 + p2[i];
      }
    return p;
  }
  unsigned int *combine_indices_int(unsigned int *p1, unsigned int *p2, int numfaces1, int numfaces2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static unsigned int *p=0;
    //delete [] m_p3;
    unsigned int *p;
    m_p3.push_back(p=new unsigned int[numfaces1*3+numfaces2*3]);
    std::copy(p1,p1+numfaces1*3,p);
    int i = 0;
    for(;i<numfaces2;i++)
      {
	p[(numfaces1+i)*3+0] = numvertices1 + p2[i*3+0];
	p[(numfaces1+i)*3+1] = numvertices1 + p2[i*3+1];
	p[(numfaces1+i)*3+2] = numvertices1 + p2[i*3+2];
	//p[(numfaces1+i)*3] = numvertices1 + p2[i];
      }
    return p;
  }
  
  Point *combine_pos(Point *p1, Point *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static Point *p=0;
    //delete [] m_p4;
    Point *p;
    m_p4.push_back(p=new Point[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;
  }
  Point *combine_pos2(Point *p1, Point *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static Point *p=0;
    //delete [] m_p5;
    Point *p;
    m_p5.push_back(p=new Point[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;
  }
  Vector *combine_normal(Vector *p1, Vector *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static Vector *p=0;
    //delete [] m_p6;
    Vector *p;
    m_p6.push_back(p=new Vector[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;
  }
  unsigned int *combine_color(unsigned int *p1, unsigned int *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static unsigned int *p=0;
    //delete [] m_p7;
    unsigned int *p;
    m_p7.push_back(p=new unsigned int[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;
  }
  Point *combine_texcoord(Point *p1, Point *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static Point *p=0;
    //delete [] m_p8;
    Point *p;
    m_p8.push_back(p=new Point[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;
  }
  VEC4 *combine_joints(VEC4 *p1, VEC4 *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static VEC4 *p=0;
    //delete [] m_p9;
    VEC4 *p;
    m_p9.push_back(p=new VEC4[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;    
  }
  VEC4 *combine_weights(VEC4 *p1, VEC4 *p2, int numvertices1, int numvertices2) const
  {
    if (!p1||!p2) return 0;
    //static VEC4 *p=0;
    //delete [] m_p10;
    VEC4 *p;
    m_p10.push_back(p=new VEC4[numvertices1+numvertices2]);
    std::copy(p1,p1+numvertices1,p);
    std::copy(p2,p2+numvertices2,p+numvertices1);
    return p;    
  }
  template<class T>
  void swap2( T**ptr, T*val) const
  {
    //delete [] (*ptr);
    (*ptr)=val;
  }

  FaceBufferRef BatchMap(int start_x, int end_x) const
  {
    if (vec.size()==0) { FaceBufferRef r; r.numfaces=0; return r; }
    std::vector<FaceBufferRef> vec2;
    int s = vec.size();
    int num = 0;
    for(int i=0;i<s;i++)
      {
	if (start_x>=0 && end_x>=0) {
	  FaceBufferRef r = vec[i]->BatchMap(start_x,end_x);
	  start_x-=r.numfaces;
	  end_x-=r.numfaces;
	  num+=r.numfaces;
	  vec2.push_back(r);
	} else if (start_x<0 && end_x>=0)
	  {
	  FaceBufferRef r = vec[i]->BatchMap(0,end_x);
	  start_x-=r.numfaces;
	  end_x-=r.numfaces;
	  num+=r.numfaces;
	  vec2.push_back(r);
	  }
      }
    static FaceBufferRef result;
    result = vec2[0];
    int s2 = vec2.size();
    for(int i=1;i<s2;i++)
      {
	swap2(&result.facepoint,combine_pos(result.facepoint,vec2[i].facepoint,result.numvertices,vec2[i].numvertices));
	swap2(&result.facepoint2,combine_pos2(result.facepoint2,vec2[i].facepoint2,result.numvertices,vec2[i].numvertices));
	swap2(&result.pointnormal, combine_normal(result.pointnormal,vec2[i].pointnormal,result.numvertices,vec2[i].numvertices));
	swap2(&result.color,combine_color(result.color,vec2[i].color,result.numvertices,vec2[i].numvertices));
	swap2(&result.texcoords, combine_texcoord(result.texcoords,vec2[i].texcoords,result.numvertices,vec2[i].numvertices));
	swap2(&result.joints,combine_joints(result.joints,vec2[i].joints,result.numvertices,vec2[i].numvertices));
	swap2(&result.weights,combine_weights(result.weights,vec2[i].weights,result.numvertices,vec2[i].numvertices));

	result.indices_int = combine_indices_int(result.indices_int, vec2[i].indices_int, result.numfaces, vec2[i].numfaces, result.numvertices, vec2[i].numvertices);
	result.indices_short = combine_indices_short(result.indices_short, vec2[i].indices_short, result.numfaces, vec2[i].numfaces, result.numvertices, vec2[i].numvertices);
	result.indices_char = combine_indices_char(result.indices_char, vec2[i].indices_char, result.numfaces, vec2[i].numfaces, result.numvertices, vec2[i].numvertices);

	result.numfaces += vec2[i].numfaces;
	result.numvertices += vec2[i].numvertices;
      }
    //print("POINTNORMAL", result.pointnormal);
    return result;
  }
  
private:
  std::vector<FaceCollection*> vec;
  std::vector<FaceRange> ranges;
  bool firsttime;
  mutable std::vector<unsigned char *> m_p1;
  mutable std::vector<unsigned short *> m_p2;
  mutable std::vector<unsigned int *> m_p3;
  mutable std::vector<Point *> m_p4;
  mutable std::vector<Point *> m_p5;
  mutable std::vector<Vector *> m_p6;
  mutable std::vector<unsigned int *> m_p7;
  mutable std::vector<Point *> m_p8;
  mutable std::vector<VEC4 *> m_p9;
  mutable std::vector<VEC4 *> m_p10;
};





EXPORT GameApi::P GameApi::PolygonApi::or_array3(std::vector<P> vec)
{
  if (vec.size()==0) return p_empty();
  if (vec.size()==1) return vec[0];
  if (vec.size()==2) return or_elem(vec[0],vec[1]);
  if (vec.size()>150) return or_array(&vec[0],vec.size());
  int s = vec.size();
  std::vector<FaceCollection*> vec2;
  for(int i=0;i<s;i++) {
    FaceCollection *coll = find_facecoll(e,vec[i]);
    if (coll)
      vec2.push_back(coll);
  }
  FaceCollection *res;
  GameApi::P p = add_polygon2(e, res=new OrArrayNoMemory(vec2),1);
  res->HeavyPrepare();
  return p;
}

EXPORT GameApi::P GameApi::PolygonApi::or_array2(std::vector<P> vec)
{
  return or_array3(vec);
  /*
  if (vec.size()>0)
    return or_array(&vec[0],vec.size());
  return p_empty();
  */
}
EXPORT GameApi::P GameApi::PolygonApi::or_array(P *p1, int size)
{
  return or_array_1(p1,size);
}
GameApi::P GameApi::PolygonApi::or_array_1(P *p1, int size)
{
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<size;i++)
    {
      FaceCollection *pp1 = find_facecoll(e, p1[i]);
      if (!pp1) { std::cout << "or_array, bad FaceCollection" << std::endl;  continue; }
      vec->push_back(pp1);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  OrElem<FaceCollection> *coll = new OrElem<FaceCollection>(vec->begin(), vec->end());
  coll->update_faces_cache();
  return add_polygon(e, coll,1);
}

class MixColorFaceColl : public ForwardFaceCollection
{
public:
  MixColorFaceColl(FaceCollection *c1, FaceCollection *c2, float val) : ForwardFaceCollection(*c1), c1(c1), c2(c2),val(val) { }
  virtual std::string name() const { return "MixColorFaceColl"; }

  void Collect(CollectVisitor &vis)
  {
    c1->Collect(vis);
    c2->Collect(vis);
  }
  void HeavyPrepare() { } // not called because register_obj missing
  void Prepare() { c1->Prepare(); c2->Prepare(); }
  unsigned int Color(int face, int point) const
  {
    unsigned int col1 = c1->Color(face,point);
    unsigned int col2 = c2->Color(face,point);
    return Color::Interpolate(col1, col2, val);
  }

private:
  FaceCollection *c1;
  FaceCollection *c2;
  float val;
};

class MaxColorFaceColl : public ForwardFaceCollection
{
public:
  MaxColorFaceColl(FaceCollection *c1, FaceCollection *c2) : ForwardFaceCollection(*c1), c1(c1), c2(c2) { }
  virtual std::string name() const { return "MaxColorFaceColl"; }
  void Collect(CollectVisitor &vis)
  {
    c1->Collect(vis);
    c2->Collect(vis);
  }
  void HeavyPrepare() { } // not called because register_obj missing
  void Prepare() { c1->Prepare(); c2->Prepare(); }
  unsigned int Color(int face, int point) const
  {
    unsigned int col1 = c1->Color(face,point);
    unsigned int col2 = c2->Color(face,point);
    return Color::max_color(col1, col2);
  }

private:
  FaceCollection *c1;
  FaceCollection *c2;
};
class MinColorFaceColl : public ForwardFaceCollection
{
public:
  MinColorFaceColl(FaceCollection *c1, FaceCollection *c2) : ForwardFaceCollection(*c1), c1(c1), c2(c2) { }
  virtual std::string name() const { return "MinColorFaceColl"; }
  void Collect(CollectVisitor &vis)
  {
    c1->Collect(vis);
    c2->Collect(vis);
  }
  void HeavyPrepare() { } // not called because register_obj missing

  void Prepare() { c1->Prepare(); c2->Prepare(); }
  unsigned int Color(int face, int point) const
  {
    unsigned int col1 = c1->Color(face,point);
    unsigned int col2 = c2->Color(face,point);
    return Color::min_color(col1, col2);
  }

private:
  FaceCollection *c1;
  FaceCollection *c2;
};


EXPORT GameApi::P GameApi::PolygonApi::mix_color(P orig, P orig2, float val)
{
  FaceCollection *c1 = find_facecoll(e, orig);
  FaceCollection *c2 = find_facecoll(e, orig2);
  return add_polygon(e, new MixColorFaceColl(c1,c2,val), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::min_color(P orig, P orig2)
{
  FaceCollection *c1 = find_facecoll(e, orig);
  FaceCollection *c2 = find_facecoll(e, orig2);
  return add_polygon(e, new MinColorFaceColl(c1,c2), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::max_color(P orig, P orig2)
{
  FaceCollection *c1 = find_facecoll(e, orig);
  FaceCollection *c2 = find_facecoll(e, orig2);
  return add_polygon(e, new MaxColorFaceColl(c1,c2), 1);
}
class Lambert : public ForwardFaceCollection
{
public:
  Lambert(FaceCollection *coll, unsigned int color, Vector light_dir, float pow, float intensity) : ForwardFaceCollection(*coll), color(color), light_dir(light_dir),pow(pow), intensity(intensity) 
  {
    light_dir /= light_dir.Dist();
  }
  virtual std::string name() const { return "Lambert"; }

  bool has_color() const { return true; }
  unsigned int Color(int face, int point) const
  {
    Vector n = ForwardFaceCollection::PointNormal(face,point);
    n /= n.Dist();
   
    //n /= n.Dist();
    float dot = Vector::DotProduct(n,light_dir);
    //if (dot<0.0) dot=0.0;
    //if (dot>1.0) dot=1.0;
    dot = std::pow(dot,pow);
    // dot = fabs(dot);
    dot*=intensity;
    if (dot<0.0) dot=0.0;
    //if (dot>1.0) dot=1.0;
    ::Color c(color);
    c.r*=dot;
    c.g*=dot;
    c.b*=dot;
    return c.clamp().Pixel();
  }
private:
  unsigned int color;
  Vector light_dir;
  float pow;
  float intensity;
};
EXPORT GameApi::P GameApi::PolygonApi::color_lambert(P orig, unsigned int color, float light_dir_x, float light_dir_y, float light_dir_z, float pow, float intensity)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *coll = new Lambert(c, color, Vector(light_dir_x, light_dir_y, light_dir_z), pow, intensity);
  return add_polygon(e, coll,1);
  
}
class ColorElem2 : public ForwardFaceCollection
{
public:
  ColorElem2(FaceCollection *coll, unsigned int col) : ForwardFaceCollection(*coll), col(col),coll(coll) { }
  virtual std::string name() const { return "ColorElem2"; }

  ~ColorElem2()
  {
    int s1 = m_p1.size(); for(int i=0;i<s1;i++)
			    delete [] m_p1[i];
  }

  bool has_color() const { return true; }
  unsigned int Color(int face, int point) const {return col; }


  virtual bool HasBatchMap() const { return coll->HasBatchMap(); }
  
  void change_color(unsigned int **pos, int numvertices) const
  {
    static unsigned int *pos2=0;
    //delete [] pos2;
    m_p1.push_back(pos2 = new unsigned int[numvertices]);
    for(int i=0;i<numvertices;i++)
      {
	unsigned int c = col; // NOTE, COLOURS IN WRONG ORDER HERE
	c<<=8;
	c|=((c&0xff000000)>>24);
	pos2[i] = c;
	//if (i<20)
	//std::cout << "FLIPNORMAL:" << pos2[i] << std::endl;
      }
    *pos = pos2;
  }

  
  virtual FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    FaceBufferRef r = coll->BatchMap(start_face,end_face);
    change_color(&r.color,r.numvertices);
    return r;
  }

private:
  unsigned int col;
  FaceCollection *coll;
  mutable std::vector<unsigned int *> m_p1;

};

EXPORT GameApi::P GameApi::PolygonApi::color(P next, unsigned int color)
{
  FaceCollection *c = find_facecoll(e, next);
  FaceCollection *coll = new ColorElem2(c, color);
  return add_polygon(e, coll,1);
}
class ColorGrayScale : public ForwardFaceCollection
{
public:
  ColorGrayScale(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual std::string name() const { return "ColorGrayScale"; }

  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int a = col&0xff000000;
    unsigned int r = col&0xff0000;
    unsigned int g = col&0xff00;
    unsigned int b = col&0xff;
    r>>=16;
    g>>=8;
    unsigned int c = (r+g+b)/3;
    unsigned int cc = (c<<16)+(c<<8)+c;
    return a+cc;
  }
};
class ColorAlpha : public ForwardFaceCollection
{
public:
  ColorAlpha(FaceCollection *coll, unsigned int alpha) : ForwardFaceCollection(*coll), alpha(alpha),coll(coll) { }
    virtual std::string name() const { return "ColorAlpha"; }

  ~ColorAlpha()
  {
    int s1 = m_p1.size(); for(int i=0;i<s1;i++)
			    delete [] m_p1[i];
  }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int c = ForwardFaceCollection::Color(face,point);
    c = c & 0x00ffffff;
    c = c | (alpha << 24);
    return c;
  }

  virtual bool HasBatchMap() const { return coll->HasBatchMap(); }
  
  void change_color(unsigned int **pos, int numvertices) const
  {
    static unsigned int *pos2=0;
    //delete [] pos2;
    m_p1.push_back(pos2 = new unsigned int[numvertices]);
    for(int i=0;i<numvertices;i++)
      {
	unsigned int c = (*pos)[i];
	c = c & 0xffffff00; // NOTE HERE COLOURS ARE WRONG ORDER
	c = c | (alpha&0xff);
	pos2[i] = c;
	//if (i<20)
	//std::cout << "FLIPNORMAL:" << pos2[i] << std::endl;
      }
    *pos = pos2;
  }

  
  virtual FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    FaceBufferRef r = coll->BatchMap(start_face,end_face);
    change_color(&r.color,r.numvertices);
    return r;
  }

private:
  unsigned int alpha;
  FaceCollection *coll;
  mutable std::vector<unsigned int *> m_p1;
};
class ColorFromNormals : public ForwardFaceCollection
{
public:
  ColorFromNormals(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual std::string name() const { return "ColorFromNormals"; }

  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    Vector v = ForwardFaceCollection::PointNormal(face,point);
    v/=v.Dist();
    if (std::isnan(v.dx)) v.dx=0.0;
    if (std::isnan(v.dy)) v.dy=0.0;
    if (std::isnan(v.dz)) v.dz=0.0;
    int r = v.dx*127.5+127.5;
    int g = v.dy*127.5+127.5;
    int b = v.dz*127.5+127.5;
    r<<=16;
    g<<=8;
    return 0xff000000+r+g+b;
  }
  
};
class ColorRangeFaceCollection : public ForwardFaceCollection
{
public: 
  ColorRangeFaceCollection(FaceCollection *coll, unsigned int source_upper, unsigned int source_lower, unsigned int upper_range, unsigned int lower_range) : ForwardFaceCollection(*coll), coll(coll), source_upper(source_upper), source_lower(source_lower), upper_range(upper_range), lower_range(lower_range) { }
  virtual std::string name() const { return "ColorRangeFaceCollection"; }

  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int col = ForwardFaceCollection::Color(face,point);
    unsigned int col2 = Color::RangeChange(col, source_upper, source_lower, upper_range, lower_range);
    return col2;
  }
private:
  FaceCollection *coll;
  unsigned int source_upper, source_lower;
  unsigned int upper_range, lower_range;
};
EXPORT GameApi::P GameApi::PolygonApi::color_range(P orig, unsigned int source_upper, unsigned int source_lower, unsigned int upper_range, unsigned int lower_range)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorRangeFaceCollection(c,source_upper, source_lower, upper_range,lower_range);
  return add_polygon2(e, c2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_grayscale(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorGrayScale(c);
  return add_polygon2(e, c2, 1);
}
class QuadsToTris2 : public ForwardFaceCollection
{
public:
  QuadsToTris2(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "QuadsToTris2"; }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { Iterate(); } // not called because register_obj missing

  void Prepare() {
    coll->Prepare();
    Iterate();
  }
  void Iterate()
  {
    int counter = 0;
    int s = coll->NumFaces();
    for(int f=0;f<s;f++)
      {
	int count = coll->NumPoints(f);
	if (count==3) { 
	  counts.push_back(3);
	  counts2.push_back(counter); counter+=3;
	  for(int i=0;i<3;i++) { 
	    vec.push_back(coll->FacePoint(f,i));
	    norm.push_back(coll->PointNormal(f,i));
	    color.push_back(coll->Color(f,i));
	    texcoord.push_back(coll->TexCoord(f,i));
	  }
					      
	}
	else if (count==4)
	  {
	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,1));
	    vec.push_back(coll->FacePoint(f,2));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,1));
	    norm.push_back(coll->PointNormal(f,2));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,1));
	    color.push_back(coll->Color(f,2));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,1));	    
	    texcoord.push_back(coll->TexCoord(f,2));

	    counts.push_back(3);
	    counts2.push_back(counter); counter+=3;
	    vec.push_back(coll->FacePoint(f,0));
	    vec.push_back(coll->FacePoint(f,2));
	    vec.push_back(coll->FacePoint(f,3));
	    norm.push_back(coll->PointNormal(f,0));
	    norm.push_back(coll->PointNormal(f,2));
	    norm.push_back(coll->PointNormal(f,3));
	    color.push_back(coll->Color(f,0));
	    color.push_back(coll->Color(f,2));
	    color.push_back(coll->Color(f,3));
	    texcoord.push_back(coll->TexCoord(f,0));
	    texcoord.push_back(coll->TexCoord(f,2));
	    texcoord.push_back(coll->TexCoord(f,3));

	  }
	else
	  {
	    int j = count;
	    int prev = j;
	    int prev2 = 1;
	    for(int i=2;i<count/2+1;i++,j--)
	      {
		counts.push_back(3);
		counts2.push_back(counter); counter+=3;

		vec.push_back(coll->FacePoint(f, prev%count));
		norm.push_back(coll->PointNormal(f,prev%count));
		color.push_back(coll->Color(f,prev%count));
		texcoord.push_back(coll->TexCoord(f,prev%count));

		vec.push_back(coll->FacePoint(f,prev2));
		norm.push_back(coll->PointNormal(f,prev2));
		color.push_back(coll->Color(f,prev2));
		texcoord.push_back(coll->TexCoord(f,prev2));

		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i+2));
		texcoord.push_back(coll->TexCoord(f,i));


		//i++; j--;
		//if (i>=count/2) break;

		counts.push_back(3);
		counts2.push_back(counter); counter+=3;

		vec.push_back(coll->FacePoint(f,i));
		norm.push_back(coll->PointNormal(f,i));
		color.push_back(coll->Color(f,i));
		texcoord.push_back(coll->TexCoord(f,i));


		vec.push_back(coll->FacePoint(f,j-1));
		norm.push_back(coll->PointNormal(f,j-1));
		color.push_back(coll->Color(f,j-1));
		texcoord.push_back(coll->TexCoord(f,j-1));


		vec.push_back(coll->FacePoint(f,j%count));
		norm.push_back(coll->PointNormal(f,j%count));
		color.push_back(coll->Color(f,j%count));
		texcoord.push_back(coll->TexCoord(f,j%count));


		prev--; prev2++;

	      }
	    

	  }
      }
  }
  virtual int NumFaces() const { return counts.size(); }
  virtual int NumPoints(int face) const { return counts[face]; }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[counts2[face]+point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return norm[counts2[face]+point];
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
        return color[counts2[face]+point];
  }
  virtual Point2d TexCoord(int face, int point) const
  {
       return texcoord[counts2[face]+point]; 
  }

  virtual bool has_normal() const { return coll->has_normal(); }
  virtual bool has_attrib() const { return coll->has_attrib(); }
  virtual bool has_color() const { return coll->has_color(); }
  virtual bool has_texcoord() const { return coll->has_texcoord(); }
  virtual bool has_skeleton() const { return coll->has_skeleton(); }

  
private:
  FaceCollection *coll;
  std::vector<int> counts;
  std::vector<int> counts2;
  std::vector<Point> vec;
  std::vector<Vector> norm;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
};
class FlipPolygonOrder : public ForwardFaceCollection
{
public:
  FlipPolygonOrder(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "FlipPolygonOrder"; }

  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(face,Order(point, NumPoints(face)));
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(face,Order(point, NumPoints(face)));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(face,Order(point, NumPoints(face)),id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(face,Order(point, NumPoints(face)),id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(face,Order(point, NumPoints(face)));
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(face,Order(point, NumPoints(face)));
  }

  int Order(int a, int count) const
  {
    return count-a-1;
  }
private:
  FaceCollection *coll;
};
EXPORT GameApi::P GameApi::PolygonApi::flip_polygon_order(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new FlipPolygonOrder(c);
  return add_polygon(e,c2,1);
}
EXPORT GameApi::P GameApi::PolygonApi::quads_to_triangles(P p)
{
  FaceCollection *c = find_facecoll(e, p);
  FaceCollection *c2 = new QuadsToTris2(c);
  return add_polygon(e,c2,1);
}
#if 0
class SkeletalAnim : public ForwardFaceCollection
{
public:
  SkeletalAnim(FaceCollection *coll, Point p0, Point p1, Point n0, Point n1)
    : ForwardFaceCollection(*coll), prop(p0, p1), prop2(n0, n1) { }

  Point FacePoint(int face, int point) const
  {
    Point pp = ForwardFaceCollection::FacePoint(face,point);
    float val = prop.LineCoords(pp);
    Point pp1 = prop.MiddlePoint(val);
    Point pp2 = prop2.MiddlePoint(val);
    Vector v = pp2-pp1;
    return pp + v;
  }
private:
  FaceCollection *coll;
  LineProperties prop;
  LineProperties prop2;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::skeletal_anim(P p, PT p_0, PT p_1,
						     PT n_0, PT n_1)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  Point *pp_0 = find_point(e,p_0);
  Point *pp_1 = find_point(e,p_1);
  Point *nn_0 = find_point(e,n_0);
  Point *nn_1 = find_point(e,n_1);
  FaceCollection *coll2 = new SkeletalAnim(coll, *pp_0, *pp_1, *nn_0, *nn_1);
  return add_polygon(e, coll2, 1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::color_from_normals(P orig)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorFromNormals(c);
  return add_polygon2(e, c2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_alpha(P orig, unsigned int alpha)
{
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *c2 = new ColorAlpha(c, alpha);
  return add_polygon2(e, c2, 1);
}
#if 0
class ColorCubeElem : public ForwardFaceCollection
{
public:
  ColorCubeElem(FaceCollection *next, Point p_0, Point p_x, Point p_y, Point p_z,
		unsigned int c_0, unsigned int c_x, unsigned int c_y, unsigned int c_z) : ForwardFaceCollection(*next), p_0(p_0), p_x(p_x), p_y(p_y), p_z(p_z), c_0(c_0), c_x(c_x), c_y(c_y), c_z(c_z) { }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const 
  {
    Point pos = ForwardFaceCollection::FacePoint(face,point);
    Coords c;
    c.center = p_0;
    c.u_x = p_x-p_0;
    c.u_y = p_y-p_0;
    c.u_z = p_z-p_0;
    Point pos2 = c.FindInternalCoords(pos);
    pos2.x /= c.u_x.Dist();
    pos2.y /= c.u_y.Dist();
    pos2.z /= c.u_z.Dist();

    // now [0..1]
    unsigned int cc_x_a = pos2.x*((c_x-c_0)&0xff000000);
    unsigned int cc_x_r = pos2.x*((c_x-c_0)&0xff0000);
    unsigned int cc_x_g = pos2.x*((c_x-c_0)&0xff00);
    unsigned int cc_x_b = pos2.x*((c_x-c_0)&0xff);
    unsigned int cc_y_a = pos2.y*((c_y-c_0)&0xff000000);
    unsigned int cc_y_r = pos2.y*((c_y-c_0)&0xff0000);
    unsigned int cc_y_g = pos2.y*((c_y-c_0)&0xff00);
    unsigned int cc_y_b = pos2.y*((c_y-c_0)&0xff);
    unsigned int cc_z_a = pos2.z*((c_z-c_0)&0xff000000);
    unsigned int cc_z_r = pos2.z*((c_z-c_0)&0xff0000);
    unsigned int cc_z_g = pos2.z*((c_z-c_0)&0xff00);
    unsigned int cc_z_b = pos2.z*((c_z-c_0)&0xff);
    
    cc_x_a &= 0xff000000;
    cc_y_a &= 0xff000000;
    cc_z_a &= 0xff000000;

    cc_x_r &= 0xff0000;
    cc_y_r &= 0xff0000;
    cc_z_r &= 0xff0000;

    cc_x_g &= 0xff00;
    cc_y_g &= 0xff00;
    cc_z_g &= 0xff00;

    cc_x_b &= 0xff;
    cc_y_b &= 0xff;
    cc_z_b &= 0xff;

    unsigned int cc_x = cc_x_a+cc_x_r+cc_x_g+cc_x_b;
    unsigned int cc_y = cc_y_a+cc_y_r+cc_y_g+cc_y_b;
    unsigned int cc_z = cc_z_a+cc_z_r+cc_z_g+cc_z_b;

    return c_0+cc_x+cc_y+cc_z;
  }

private:
  Point p_0, p_x, p_y, p_z;
  unsigned int c_0, c_x, c_y, c_z;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::color_cube(P next,
					   PT p_0,
					   PT p_x,
					   PT p_y,
					   PT p_z,
					   unsigned int c_0,
					   unsigned int c_x,
					   unsigned int c_y,
					   unsigned int c_z)
{
#if 0  
  Point *pp_0 = find_point(e,p_0);
  Point *pp_x = find_point(e,p_x);
  Point *pp_y = find_point(e,p_y);
  Point *pp_z = find_point(e,p_z);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorCubeElem(convert, *pp_0, *pp_x, *pp_y, *pp_z, c_0, c_x, c_y, c_z);
  return add_polygon2(e, coll,1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::color_faces(P next, 
					 unsigned int color_1, 
					 unsigned int color_2,
					 unsigned int color_3, 
					 unsigned int color_4)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, next);
  BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new ColorFaceElem(*convert, color_1,color_2,color_3,color_4);
  return add_polygon2(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::translate(P orig, float dx, float dy, float dz)
{
  return translate_1(orig, dx,dy,dz);
}
GameApi::P GameApi::PolygonApi::matrix(P orig, M mat)
{
  Matrix mat2 = find_matrix(e, mat);
  //::EnvImpl *env = ::EnvImpl::Environment(&e); 
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { return p_empty(); }
  FaceCollection *coll = new MatrixElem(*c, mat2);
  return add_polygon2(e, coll,1);

}
GameApi::P GameApi::PolygonApi::translate_1(P orig, float dx, float dy, float dz)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << orig.id << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::Translate(dx,dy,dz));
  return add_polygon2(e, coll,1);
}
 
EXPORT GameApi::P GameApi::PolygonApi::rotatex(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << orig.id << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::XRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatey(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::YRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotatez(P orig, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::ZRotation(angle));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::rotate(P orig, PT point, V axis, float angle)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  Point *pp = find_point(e, point);
  Vector *ax = find_vector(e, axis);
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem(*c, Matrix::RotateAroundAxisPoint(*pp, *ax, angle));
  return add_polygon(e, coll,1);
}


EXPORT GameApi::P GameApi::PolygonApi::scale(P orig, float sx, float sy, float sz)
{
  //::EnvImpl *env = ::EnvImpl::Environment(&e);  
  FaceCollection *c = find_facecoll(e, orig);
  FaceCollection *coll = new MatrixElem(*c, Matrix::Scale(sx,sy,sz));
  return add_polygon(e, coll,1);
}

EXPORT GameApi::P GameApi::PolygonApi::move(P orig, PT obj_0, V obj_x, V obj_y, V obj_z,
				     PT world_0, V world_x, V world_y, V world_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  Point *obj0 = find_point(e, obj_0);
  Vector *objx = find_vector(e, obj_x);
  Vector *objy = find_vector(e, obj_y);
  Vector *objz = find_vector(e, obj_z);

  Point *world0 = find_point(e, world_0);
  Vector *worldx = find_vector(e, world_x);
  Vector *worldy = find_vector(e, world_y);
  Vector *worldz = find_vector(e, world_z);
  
  Coords obj;
  obj.center = *obj0;
  obj.u_x = *objx;
  obj.u_y = *objy;
  obj.u_z = *objz;
  Coords world;
  world.center = *world0;
  world.u_x = *worldx;
  world.u_y = *worldy;
  world.u_z = *worldz;
  FaceCollection *coll2 = new CoordChangeFaceColl(coll, false, obj);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(coll2));  
  FaceCollection *coll3 = new CoordChangeFaceColl(coll2, true, world);
  return add_polygon(e, coll3, 1);
#endif
}

EXPORT GameApi::P GameApi::PolygonApi::splitquads(P orig, int x_count, int y_count)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *next = new SplitQuads(*coll, x_count, y_count);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(next));  
  //MemoizeFaces *next2 = new MemoizeFaces(*next);
  //next2->Reset();
  return add_polygon(e, next, 1);
}


class GameApiPointFunction2 : public Function<Point,Point>
{
public:
  GameApiPointFunction2(GameApi::Env &e, GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data), void *data) : e(e), fptr(fptr), data(data) { }
  Point Index(Point p) const
  {
    GameApi::EveryApi ev(e);
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = fptr(ev, pt, data);
    Point *pp = find_point(e, pt2);
    return *pp;
  }
  
private:
  GameApi::Env &e;
  GameApi::PT (*fptr)(GameApi::EveryApi &ev, GameApi::PT p, void *data);
  void *data;
};
class GameApiPointFunction : public Function<Point,Point>
{
public:
  GameApiPointFunction(GameApi::Env &e, GameApi::FunctionCb<GameApi::PT,GameApi::PT> *cb) : e(e), cb(cb){ }
  Point Index(Point p) const
  {
    GameApi::PT pt = add_point(e, p.x, p.y, p.z);
    GameApi::PT pt2 = cb->Map(pt);
    Point *pp = find_point(e, pt2);
    if (!pp) return Point(0.0,0.0,0.0);
    return *pp;
  }
private:
  GameApi::Env &e;
  GameApi::FunctionCb<GameApi::PT, GameApi::PT> *cb;
};
#if 0
void GameApi::PolygonApi::del_cb_later(GameApi::FunctionCb<PT,PT> *cb)
{
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(cb));
}
#endif

class ChangePoints2 : public ForwardFaceCollection
{
public:
  ChangePoints2(FaceCollection &coll, std::function<Point (Point, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  virtual std::string name() const { return "ChangePoints2"; }

  virtual Point FacePoint(int face, int point) const 
  {
    return f(ForwardFaceCollection::FacePoint(face,point),
		face, point);
  }
private:
  std::function<Point (Point, int,int)> f;
};
struct ChangePositions_data
{
  GameApi::Env *env;
  std::function<GameApi::PT (GameApi::PT p, int face, int point)> f;
};

Point ChangePositions_Func(Point p, int face,int point, void* data)
{
  ChangePositions_data *dt = (ChangePositions_data*)data;
  GameApi::PT pt = add_point(*dt->env, p.x, p.y, p.z);
  GameApi::PT pt2 = dt->f(pt, face, point);
  Point *pp = find_point(*dt->env, pt2);
  if (!pp) return Point(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_positions(P orig, std::function<PT (PT p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
  FaceCollection *coll = find_facecoll(e, orig);
  ChangePositions_data dt;
  dt.env = &e;
  dt.f = f;
  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  FaceCollection *coll2 = new ChangePoints2(*coll, std::bind(&ChangePositions_Func, _1,_2,_3,(void*)&dt));
  return add_polygon(e, coll2, 1);
#endif
  GameApi::P p;
  p.id = 0;
  return p;
}


#if 0
class ChangeNormal2 : public ForwardFaceCollection
{
public:
  ChangeNormal2(FaceCollection &coll, std::function<Vector (Vector, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const 
  {
    return f(ForwardFaceCollection::PointNormal(face,point),
		face, point);
  }
private:
  std::function<Vector (Vector, int,int)> f;
};
#endif
struct ChangeNormal_data
{
  GameApi::Env *env;
  std::function<GameApi::V (GameApi::V p, int face, int point)> f;
};

Vector ChangeNormals_Func(Vector p, int face,int point,void* data)
{
  ChangeNormal_data *dt = (ChangeNormal_data*)data;
  GameApi::V pt = add_vector(*dt->env, p.dx, p.dy, p.dz);
  GameApi::V pt2 = dt->f(pt, face, point);
  Vector *pp = find_vector(*dt->env, pt2);
  if (!pp) return Vector(0.0,0.0,0.0);
  return *pp;
}

EXPORT GameApi::P GameApi::PolygonApi::change_normals(P orig, std::function<V (V p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeNormal_data dt;
  dt.env = &e;
  dt.f = f;

  GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;

  FaceCollection *coll2 = new ChangeNormal2(*coll, std::bind(&ChangeNormals_Func, _1, _2, _3, &dt));
  return add_polygon(e, coll2, 1);
#endif
#endif
  GameApi::P p;
  p.id = 0;
  return p;
}


#if 0
class ChangeColor2 : public ForwardFaceCollection
{
public:
  ChangeColor2(FaceCollection &coll, std::function<unsigned int (unsigned int, int,int)> f)
    : ForwardFaceCollection(coll), f(f) { }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const 
  {
    return f(ForwardFaceCollection::Color(face,point),
		face, point);
  }
private:
  //GameApi::EveryApi &ev;
  std::function<unsigned int ( unsigned int, int,int)> f;
  //void *data;
};
#endif
struct ChangeColor_data
{
  GameApi::Env *env;
  std::function<unsigned int (unsigned int p, int face, int point)> f;
  //void *data;
};

unsigned int ChangeColor_Func(unsigned int p, int face,int point,void* data)
{
  ChangeColor_data *dt = (ChangeColor_data*)data;
  return dt->f(p,face,point);
}

EXPORT GameApi::P GameApi::PolygonApi::change_colors(P orig, std::function<unsigned int (unsigned int p, int face, int point)> f)
{
#ifndef EMSCRIPTEN
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  ChangeColor_data *dt = new ChangeColor_data;
  dt->env = &e;
  dt->f = f;
  //dt.data = data;

  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(dt));

  FaceCollection *coll2 = new ChangeColor2(*coll, std::bind(ChangeColor_Func, _1,_2,_3,(void*)dt));
  return add_polygon(e, coll2, 1);
#endif
#endif
  GameApi::P p;
  p.id = 0;
  return p;
}

#if 0
class ChangeTexture : public ForwardFaceCollection
{
public:
  ChangeTexture(FaceCollection &coll, std::function<int (int)> f, ::Bitmap< ::Color> **array, int size)
    : ForwardFaceCollection(coll), f(f), array(array) /*, size(size)*/ { temp = new BufferFromBitmap*[size]; }
  void GenTexture(int num) 
  {
    delete temp[num];
    temp[num] = new BufferFromBitmap(*(array[num]));
    temp[num]->Gen();
  }
  virtual BufferRef TextureBuf(int num) const 
  { 
    return temp[num]->Buffer();
  }
  virtual int FaceTexture(int face) const { return f(face); }
private:
  //GameApi::EveryApi &ev;
  std::function<int (int)> f;
  //void *data;
  Bitmap< ::Color> **array;
  //int size;
  mutable BufferFromBitmap **temp;
};
#endif

EXPORT GameApi::P GameApi::PolygonApi::change_texture(P orig, std::function<int (int face)> f, BM *array, int size)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  std::vector<Bitmap<Color>*> *vec = new std::vector<Bitmap<Color>*>;
  for(int i=0;i<size;i++)
    {
      BitmapHandle *handle = find_bitmap(e, array[i]);
      ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
      vec->push_back(bitmap);
    }
  //GameApi::EveryApi *ev = new GameApi::EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  ChangeTexture *tex = new ChangeTexture(*coll, f, &(*vec)[0], size);
  return add_polygon(e, tex, 1);
#endif
}



//EXPORT GameApi::P GameApi::PolygonApi::smooth_normals(P orig)
//{
//  FaceCollection *coll = find_facecoll(e, orig);
//  FaceCollection *coll2 = new SmoothNormals(*coll);
//  return add_polygon(e, coll2, 1);
//}
EXPORT GameApi::P GameApi::PolygonApi::recalculate_normals(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new RecalculateNormals(*coll);
  return add_polygon(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::average_normals(P orig, int sx, int sy)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  FaceCollection *coll2 = new AverageNormals(coll, sx,sy);
  return add_polygon(e, coll2, 1);
#endif
}

EXPORT GameApi::P GameApi::PolygonApi::memoize(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::memoize_all(P orig)
{
  FaceCollection *coll = find_facecoll(e, orig);
  MemoizeFaces *coll2 = new MemoizeFaces(*coll);
  coll2->Reset();
  coll2->MemoizeAll();
  return add_polygon2(e, coll2, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::circular_span(EveryApi &ev, LI li,
						     float delta_angle,
						     int num_steps)
{
  M m = ev.matrix_api.yrot(delta_angle);
  return span(li, m, num_steps);
}
#if 0
class Span : public SingleForwardFaceCollection
{
public:
  Span(LineCollection *lines, Matrix m, int num_steps) : lines(lines), m(m), num_steps(num_steps) 
  {
    Matrix mm = Matrix::Identity();
    arr.push_back(mm);
    for(int i=0;i<num_steps;i++)
      {
	mm = mm * m; 
	arr.push_back(mm);
      }
  }
  void Collect(CollectVisitor &vis)
  {
    lines->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { lines->Prepare(); }
  virtual int NumFaces() const { return lines->NumLines() * num_steps; }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int line = face / num_steps;
    int step = face - line*num_steps;
    Matrix m1 = arr[step];
    Point p = lines->LinePoint(line, 0);
    Point py = lines->LinePoint(line, 1);
    p = p * m1;
    py = py * m1;
    Point px = p*m;
    Point pxy = py*m;
    switch(point)
      {
      case 0: return p;
      case 1: return px;
      case 2: return pxy;
      case 3: return py;
      }
    return p;
  }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    v/=v.Dist();
    store_face=face;
    store_res=v;
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    int line = face / num_steps;
    //int step = face - line*num_steps;
    unsigned int p = lines->LineColor(line, 0);
    unsigned int py = lines->LineColor(line, 1);
    switch(point)
      {
      case 0: return p;
      case 1: return p;
      case 2: return py;
      case 3: return py;
      };
    return p;

  }
  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p00 = { 0.0, 0.0 };
    Point2d p10 = { 1.0, 0.0 };
    Point2d p11 = { 1.0, 1.0 };
    Point2d p01 = { 0.0, 1.0 };
    switch(point)
      {
      case 0: return p00;
      case 1: return p10;
      case 2: return p11;
      case 3: return p01;
      }
    return p00;
  }

private:
  LineCollection *lines;
  Matrix m;
  int num_steps;
  std::vector<Matrix> arr;
  mutable int store_face=-1;
  mutable Vector store_res;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::span(LI li,
					    M matrix,
					    int num_steps)
{
#if 0
  LineCollection *lines = find_line_array(e, li);
  Matrix m = find_matrix(e, matrix);
  FaceCollection *span = new Span(lines, m, num_steps);
  return add_polygon2(e, span, 1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::linear_span(EveryApi &ev, LI li,
						   float dx, float dy, float dz,
						   int num_steps)
{
  return span(li, ev.matrix_api.trans(dx,dy,dz), num_steps);
}


class HeightMap2 : public FaceCollection
{
public:
  HeightMap2(Bitmap<float> &bm, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : bm(bm), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z) { }
  virtual std::string name() const { return "HeightMap2"; }

  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual int NumFaces() const { return (bm.SizeX()-1)*(bm.SizeY()-1); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm.SizeX();
    int yy = face-(xx*bm.SizeX());
    if (point==1 || point==2) xx++;
    if (point==2 || point==3) yy++;
    Point p;
    p.x = start_x + xx*(end_x-start_x)/bm.SizeX();
    p.y = start_y + bm.Map(xx,yy)*(end_y-start_y);
    p.z = start_z + yy*(end_z-start_z)/bm.SizeY();
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face,0);
    Point p2 = FacePoint(face,1);
    Point p3 = FacePoint(face,2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    v/=v.Dist();
    store_face=face;
    store_res=v;

    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int xx = face/bm.SizeX();
    int yy = face-(xx*bm.SizeX());
    if (point==1 || point==2) xx++;
    if (point==2 || point==3) yy++;
    Point2d p;
    p.x = float(xx)/float(bm.SizeX());
    p.y = float(yy)/float(bm.SizeY());
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }

private:
  Bitmap<float> &bm;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  mutable int store_face=-1;
  mutable Vector store_res;
};

EXPORT GameApi::P GameApi::PolygonApi::heightmap2(FB bm, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  Bitmap<float> *bbm = find_float_bitmap(e, bm)->bitmap;
  return add_polygon2(e, new HeightMap2(*bbm, start_x, end_x, start_y,end_y,start_z, end_z),1);
}



EXPORT GameApi::P GameApi::PolygonApi::heightmap(FB bm,
					  std::function<P (float)> f, float dx, float dz)
{
  FloatBitmap *fb = find_float_bitmap(e, bm);
  int sx = fb->bitmap->SizeX();
  int sy = fb->bitmap->SizeY();
  std::vector<P> vec;
  for(int y=0;y<sy;y++)
    {
      std::vector<P> vec2;
      for(int x=0;x<sx;x++)
	{
	  float val = fb->bitmap->Map(x,y);
	  P p = f(val);
	  P p2 = translate_1(p, dx*x, 0.0, 0.0);
	  vec2.push_back(p2);
	}
      P p = or_array_1(&vec2[0], sx);
      P p2 = translate_1(p, 0.0, 0.0, dz*y);
      vec.push_back(p2);
    }
  P p = or_array_1(&vec[0], sy);
  return p;
}
EXPORT GameApi::P GameApi::PolygonApi::heightmap(BM bm,
					  HeightMapType t,
				       float min_x, float max_x,
				       float min_y, float max_y,
				       float min_z, float max_z)
{
#if 0
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *bitmap = find_color_bitmap(handle);
  HeightMap3DataImpl *data = new HeightMap3DataImpl(*bitmap);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(data));
  Point p(min_x, max_y, min_z);
  Vector u_x(max_x-min_x, 0.0, 0.0);
  Vector u_y(0.0, -(max_y-min_y),0.0);
  Vector u_z(0.0,0.0,max_z-min_z);

  HeightMap3 *heightmap = new HeightMap3(*data, 0.0, 1.0,
					 p, u_x, u_y, u_z);
  env->deletes.push_back(std::shared_ptr<void>(heightmap));
  MeshFaceCollection *coll = new MeshFaceCollection(*heightmap, 0);
  return add_polygon(e, coll, 1);
#endif
}

EXPORT GameApi::P GameApi::PolygonApi::anim_array(P *array, int size)
{
  int s = size;
  std::vector<FaceCollection*> *vec = new std::vector<FaceCollection*>;
  for(int i=0;i<s;i++)
    {
      P *ptr = array+i;
      FaceCollection *c = find_facecoll(e, *ptr);
      //if (!c) { std::cout << "anim_array: dynamic cast failed" << std::endl; exit(0); }
      vec->push_back(c);
    }
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(vec));
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec->begin(), vec->end());
  handle->collarrayowned = true;
  //handle->size = size;
  return add_polygon(e, handle);
}

EXPORT void GameApi::PolygonApi::render(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "Need to call prepare() before render() call(do not put prepare() to renderloop)" << std::endl; }
  state_bm->Render(choose); 
}

StateBitmaps * PrepareFaceCollPolyHandle(FaceCollPolyHandle *h2, int bbm_choose)
{
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
    
  FaceCollection * const *array2 = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      VectorArray<FaceCollection*> *ptr = static_cast<VectorArray<FaceCollection*>*>(h2->collarray);
      //std::cout << "Using collarray" << std::endl;
      array2 = ptr->get_whole_array();
      size = ptr->Size();
    }
  if (bbm_choose != -1)
    {
      array2 = &array2[bbm_choose];
      size = 1;
    }
  FaceCollection **array = const_cast<FaceCollection**>(array2);
  FaceArrayMesh *mesh = new FaceArrayMesh(array, size);
  FaceArrayMeshNormals *normal = new FaceArrayMeshNormals(array, size);
  FaceArrayMeshTexCoords *coord = new FaceArrayMeshTexCoords(array, size);
  FaceArrayMeshColors *color = new FaceArrayMeshColors(array, size);
  FaceArrayMeshTextures *textures = new FaceArrayMeshTextures(array, size, 0);

  StateBitmaps *state_bm = new StateBitmaps(mesh, normal, coord, color, textures, *textures);
  state_bm->Prepare();
  return state_bm;
}

EXPORT void GameApi::PolygonApi::prepare(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  StateBitmaps *state_bm = PrepareFaceCollPolyHandle(h2, bbm_choose);
  pp->states[p.id] = state_bm;
}

EXPORT void GameApi::PolygonApi::preparepoly(P p, int bbm_choose)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = new ArrayRender(g_low);
  pp->rend[p.id] = r;
  FaceCollPolyHandle *h2 = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
  
  FaceCollection **array = &h2->coll;
  int size = 1; //h2->size;
  if (!h2->coll && h2->collarray)
    {
      std::cout << "Using collarray" << std::endl;
      //array = h2->collarray;
      AllocToNativeArray(*h2->collarray, &array);
    }
  if (bbm_choose != -1)
    {
      array = &array[bbm_choose];
      size = 1;
    }
  FaceArrayMesh mesh(array, size);
  FaceArrayMeshNormals normal(array, size);
  FaceArrayMeshTexCoords coord(array, size);
  FaceArrayMeshColors color(array, size);

  MeshToTriangles trimesh(mesh);
  MeshNormalsToTriangles trinormal(normal, mesh.NumPoints()==4);
  MeshTexCoordsToTriangles tricoord(coord, mesh.NumPoints()==4);
  MeshColorsToTriangles tricolor(color, mesh.NumPoints()==4);

  //std::cout << "preparepoly:" <<  h2->size << std::endl;
  //std::cout << mesh.NumFaces(0) << " " << mesh.NumPoints() << std::endl;
  r->Alloc(trimesh.NumFaces(0), trimesh.NumFaces(0)*trimesh.NumPoints(), size, size, size, size);
  r->InsertAll(trimesh, trinormal, tricoord, tricolor);
#if 0
  int texcount = array[0]->NumTextures();
  if (texcount)
    {
      r->AllocTexture(texcount);
      MeshTextures *texture = 0;
      for(int i=0;i<texcount;i++)
	{
	  array[0]->GenTexture(i);
	  BufferRef buf = array[0]->TextureBuf(i);
	  BitmapFromBuffer *bitmap = new BitmapFromBuffer(buf);
	  if (texture)
	    {
	      texture = new MeshTexturesImpl(*bitmap, *texture);
	    }
	  else
	    {
	      texture = new MeshTexturesImpl(*bitmap);
	    }
	}
      r->UpdateTexture(*texture, 0);
    }
#endif
  //for(int i=0;i<h2->size;i++)
  //  {
  //    std::pair<int,int> p = r->InsertMesh(mesh, i);
  //  }
}
void GameApi::PolygonApi::renderpoly(P p, PT pos)
{
  Point *pp = find_point(e, pos);
  renderpoly(p, 0, pp->x, pp->y, pp->z);
}
void GameApi::PolygonApi::renderpoly(P p, float x, float y, float z)
{
  renderpoly(p, 0, x,y,z);
}
void GameApi::PolygonApi::renderpoly(P p, int choose, float x, float y, float z)
{
  PolyPriv *pp = (PolyPriv*)priv;
  ArrayRender *r = pp->rend[p.id];
  if (!r) { std::cout << "To use renderpoly() you should first call preparepoly(do not put it to frame loop)" << std::endl; return; }
  OpenglLowApi *ogl = g_low->ogl;
  ogl->glPushMatrix();
#ifndef EMSCRIPTEN
  ogl->glTranslatef(x,y,z);
#endif
  //std::cout << "renderpoly: " << r->used_vertex_count << std::endl;

  //PolyHandle *handle = find_poly(e,p);
  //FaceCollPolyHandle *h2 = dynamic_cast<FaceCollPolyHandle*>(handle);
#if 0
  if (r->TextureCount())
    {
      r->EnableTexture(0);
    }
#endif
  r->Render(choose, choose, choose, choose, 0, r->used_vertex_count[choose]);
#if 0
  if (r->TextureCount())
    {
      r->DisableTexture();
    }
#endif
 ogl->glPopMatrix();
}

#if 0
class CountsFaceCollection : public ForwardFaceCollection
{
public:
  CountsFaceCollection(int numfaces, FaceCollection *next) : ForwardFaceCollection(*next), numfaces(numfaces) { }
  int NumFaces() const { return numfaces; }
private:
  int numfaces;
};
#endif

#if 0
class CountFuncFaceCollection : public ForwardFaceCollection
{
public:
  CountFuncFaceCollection(FaceCollection *next, std::function<int (int face)> f) : ForwardFaceCollection(*next),  f(f) { }
  virtual int NumPoints(int face) const
  {
    return f(face);
  }
  
private:
  //GameApi::EveryApi &ev;
  std::function<int (int face)> f; 
  //void *data;
};
#endif

#if 0
class PointFaceCollection : public ForwardFaceCollection
{
public:
  PointFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual Point FacePoint(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};
#endif

class NormalFaceCollection : public ForwardFaceCollection
{
public:
  NormalFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::V (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual std::string name() const { return "NormalFaceCollection"; }

  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    GameApi::V pp = f(face, point);
    Vector *p = find_vector(e, pp);
    return *p;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::V (int face, int point)> f;
  //void *data;
};

#if 0
class ColorFaceCollection : public ForwardFaceCollection
{
public:
  ColorFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<unsigned int (int face, int point)> f) : ForwardFaceCollection(*next), /*e(e),*/  f(f) { }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int pp = f(face, point);
    return pp;
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<unsigned int (int face, int point)> f;
  void *data;
};
#endif

class TexFaceCollection : public ForwardFaceCollection
{
public:
  TexFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<GameApi::PT (int face, int point)> f) : ForwardFaceCollection(*next), e(e), f(f) { }
  virtual std::string name() const { return "TexFaceCollection"; }

  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const
  {
    GameApi::PT pp = f(face, point);
    Point *p = find_point(e, pp);
    Point2d px = { p->x, p->y };
    return px;
  }

private:
  GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<GameApi::PT (int face, int point)> f;
  //void *data;
};

#if 0
class AttribFaceCollection : public ForwardFaceCollection
{
public:
  AttribFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<float (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/ f(f), idx(idx) { }
  bool has_attrib() const { return true; }
  virtual float Attrib(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::Attrib(face,point,id);
  }

private:
  std::function<float (int face, int point, int id)> f;
  int idx;
};
class AttribIFaceCollection : public ForwardFaceCollection
{
public:
  AttribIFaceCollection(GameApi::Env &e, FaceCollection *next, std::function<int (int face, int point, int id)> f, int idx) : ForwardFaceCollection(*next), /*e(e),*/  f(f), idx(idx) { }

  virtual int AttribI(int face, int point, int id) const
  {
    if (idx == id) {
      float pp = f(face, point, id);
      return pp;
    }
    return ForwardFaceCollection::AttribI(face,point,id);
  }

private:
  //GameApi::Env &e;
  //GameApi::EveryApi &ev;
  std::function<int (int face, int point, int id)> f;
  int idx;
};
#endif



EXPORT GameApi::P GameApi::PolygonApi::counts(P p1, int numfaces)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  return add_polygon(e, new CountsFaceCollection(numfaces, poly),1);  
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::count_function(P p1, std::function<int (int face)> f)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new CountFuncFaceCollection(poly, f),1);  
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::point_function(P p1, std::function<PT (int face, int point)> f)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  EveryApi *ev = new EveryApi(e);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new PointFaceCollection(e, poly, f),1);  
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::normal_function(P p1, std::function<V (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new NormalFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::color_function(P p1, std::function<unsigned int (int face, int point)> f)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new ColorFaceCollection(e, poly,f),1);  
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::texcoord_function(P p1, std::function<PT (int face, int point)> f)
{
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new TexFaceCollection(e, poly, f),1);  
}
EXPORT GameApi::P GameApi::PolygonApi::attrib_function(P p1, std::function<float (int face, int point, int idx)> f, int idx)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribFaceCollection(e, poly, f, idx),1);  
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::attribi_function(P p1, std::function<int (int face, int point, int idx)> f, int idx)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p1);
  //EveryApi *ev = new EveryApi(e);
  //::EnvImpl *env = ::EnvImpl::Environment(&e);
  //env->deletes.push_back(std::shared_ptr<void>(ev));
  return add_polygon(e, new AttribIFaceCollection(e, poly, f, idx),1);  
#endif
}

EXPORT GameApi::P GameApi::PolygonApi::create_static_geometry(GameApi::P *array, int size)
{
#if 0
  if (size==0) { std::cout << "Empty array in remove_changing" << std::endl; }
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(find_facecoll(e,array[i]));
    }
  IsChangingFace *func = new IsChangingFace(vec);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(func));
  FilterFaces *coll2 = new FilterFaces(*(vec[0]), *func);
  return add_polygon(e, coll2, 1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::create_dynamic_geometry(GameApi::P *array, int size)
{
#if 0
  std::vector<FaceCollection*> vec;
  for(int i=0;i<size;i++)
    {
      FaceCollection *coll = find_facecoll(e,array[i]);
      vec.push_back(coll);
    }
  IsChangingFace *ch = new IsChangingFace(vec);
  for(int i=0;i<size;i++)
    {
      vec[i] = new FilterFaces(*(vec[i]), *ch);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = NULL;
  handle->collowned = false;
  handle->collarray = new VectorArray<FaceCollection*>(vec.begin(), vec.end());
  handle->collarrayowned = true;

  return add_polygon(e, handle);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::tri_vertex_array(float *v_array, int v_size,
						 float *n_array, int n_size,
						 unsigned int *c_array, int c_size,
						 float *tex_array, int tex_size,
						 float **attrib_array, int a_size1, int a_size2)
{
#if 0
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = new VertexArrayFaceCollection(v_array, v_size,
					       n_array, n_size,
					       c_array, c_size,
					       tex_array, tex_size,
					       attrib_array, a_size1, a_size2);
  handle->collowned = true;
  handle->collarray = NULL;
  handle->collarrayowned = false;
  return add_polygon(e, handle);
#endif
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_frames(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_frames" << std::endl; }

  FaceCollPolyHandle *h2 = find_poly(e,p);
  if (!h2->collarray) { return 1; }
  return h2->collarray->Size();
}

EXPORT int GameApi::PolygonApi::get_tri_vertex_array_rows(P p)
{
  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array_rows" << std::endl; }
  return state_bm->bitmap->SizeY();
}

EXPORT void GameApi::PolygonApi::get_tri_vertex_array(P p, int choose, int row,
					       int *v_size, float **v_array,
					       int *n_size, float **n_array,
					       int *c_size, unsigned int **c_array,
					       int *tex_size, float **tex_array,
					       int *attrib_size1, int *attrib_size2, float ***attrib_array)
{

  PolyPriv *pp = (PolyPriv*)priv;
  StateBitmaps *state_bm = pp->states[p.id];
  if (!state_bm) { std::cout << "need to call prepare() before get_tri_vertex_array" << std::endl; }
  StateRow &r = state_bm->bitmap->DynRow(row);
  int vertex_pos = 0;
  int vertex_size = r.rend->used_vertex_count[choose];
  int v_choose = choose;
  int n_choose = choose;
  int c_choose = choose;
  int t_choose = choose;
  ArrayRender *rend = state_bm->GetRender(row);
  //Matrix m = state_bm->GetMatrix(choose);
  int v_offset3 = vertex_pos*3+v_choose*rend->vertex_array_size*3;
  int n_offset3 = vertex_pos*3+n_choose*rend->vertex_array_size*3;
  int c_offset =  vertex_pos+c_choose*rend->vertex_array_size;
  int tex_offset2 = vertex_pos*2+t_choose*rend->vertex_array_size*2;
  *v_array = rend->vertex_array+v_offset3;
  *n_array = rend->normal_array+n_offset3;
  *c_array = (unsigned int*)rend->color_array+c_offset;
  *tex_array = rend->tex_coord_array+tex_offset2;
  
  *v_size = vertex_size*3;
  *n_size = vertex_size*3;
  *c_size = vertex_size;
  *tex_size = vertex_size*2;
}

#if 0
class TriStripFaceCollection : public SingleForwardFaceCollection
{
public:
  TriStripFaceCollection(std::vector<Point> vec) : vec(vec) { }
  virtual int NumFaces() const { return vec.size()-2; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    return vec[face+point];
  }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = -Vector::CrossProduct(p2-p1,p3-p1);
    v/=v.Dist();
    store_face=face;
    store_res=v;
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = { 0.0, 0.0 };
    return p;
  }
private:
  std::vector<Point> vec;
  mutable int store_face=-1;
  mutable Vector store_res;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::tri_strip(PT *array, int size)
{
#if 0
  std::vector<Point> vec;
  for(int i=0;i<size;i++)
    {
      vec.push_back(*find_point(e, array[i]));
    }
  return add_polygon2(e, new TriStripFaceCollection(vec),1);
#endif  
}
EXPORT GameApi::P GameApi::PolygonApi::polygon2(std::vector<PT> vec)
{
  return polygon(&vec[0], vec.size());
}
EXPORT GameApi::P GameApi::PolygonApi::polygon3(PTS points)
{
  PointsApiPoints *p = find_pointsapi_points(e, points);
  PolygonElem *coll = new PolygonElem;
  p->Prepare();
  int sz = p->NumPoints();
  for(int i=0;i<sz;i++)
    {
      Point pp = p->Pos(i);
      coll->push_back(pp);
    }
  if (sz!=0)
    coll->push_back(p->Pos(0));
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = coll;
  handle->collowned = false;
  handle->collarray = NULL; 
  handle->collarrayowned = false;

  return add_polygon(e, handle);

}

EXPORT GameApi::P GameApi::PolygonApi::polygon(PT *array, int size)
{
  PolygonElem *coll = new PolygonElem;
  int sz = size;
  for(int i=0;i<sz;i++)
    {
      PT p = array[i];
      Point *pp = find_point(e,p);
      coll->push_back(*pp);
    }
  FaceCollPolyHandle *handle = new FaceCollPolyHandle;
  handle->coll = coll;
  handle->collowned = false;
  handle->collarray = NULL; 
  handle->collarrayowned = false;

  return add_polygon(e, handle);
  
}

EXPORT void GameApi::PolygonApi::render_dynamic(GameApi::P p, int array_elem, bool textures)
{
  FaceCollPolyHandle *handle = find_poly(e,p);
  FaceCollection *coll = handle->collarray->Index(array_elem);
  if (textures)
    {
      RenderOpenGlTextures(*coll);
    }
  else
    {
      RenderOpenGl(*coll);
    }
}

#if 0
class ColorVoxelFaceCollection : public ForwardFaceCollection
{
public:
  ColorVoxelFaceCollection(FaceCollection &coll, Voxel<unsigned int> &c, Point p, Vector v_x, Vector v_y, Vector v_z) : ForwardFaceCollection(coll), c(c), pp(p) { cc.center = v_x; cc.u_x = v_x; cc.u_y = v_y; cc.u_z = v_z; }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const { 
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Point p2 = cc.FindInternalCoords(p);
    return c.Map(p2.x,p2.y,p2.z);
  }
private:
  Voxel<unsigned int> &c;
  mutable Coords cc;
  Point pp;
};
#endif

EXPORT GameApi::P GameApi::PolygonApi::color_voxel(P orig, VX colours, PT p, V u_x, V u_y, V u_z)
{
#if 0
  Point *pp = find_point(e, p);
  Vector *uu_x = find_vector(e, u_x);
  Vector *uu_y = find_vector(e, u_y);
  Vector *uu_z = find_vector(e, u_z);

  FaceCollection *coll = find_facecoll(e, orig);
  Voxel<unsigned int> *v = find_voxel(e, colours);
  return add_polygon(e, new ColorVoxelFaceCollection(*coll, *v, *pp, *uu_x, *uu_y, *uu_z), 1);
#endif
}
#if 0
class UpdateVA : public MainLoopItem
{
public:
  UpdateVA(GameApi::PolygonApi &api, GameApi::VA va, GameApi::P p, bool keep) : api(api), va(va), p(p), keep(keep) { }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  void Prepare() { }
  void execute(MainLoopEnv &e)
  {
    api.update_vertex_array(va,p,keep);
  }
  std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::PolygonApi &api;
  GameApi::VA va;
  GameApi::P p;
  bool keep;
};
#endif
EXPORT GameApi::ML GameApi::PolygonApi::update_vertex_array_ml(GameApi::VA va, GameApi::P p, bool keep)
{
#if 0
  return add_main_loop(e, new UpdateVA(*this, va, p, keep));
#endif
}
EXPORT void GameApi::PolygonApi::update_vertex_array_no_memory(GameApi::VA va, GameApi::P p)
{
  FaceCollection *faces = find_facecoll(e, p);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  VertexArraySet *set = find_vertex_array(e, va);
  set->clear_arrays();
  FaceCollectionVertexArray2 faces2(*faces, *set);
  faces2.copy(0,faces->NumFaces());
  rend->update(0);
}
EXPORT void GameApi::PolygonApi::delete_vertex_array(GameApi::VA va)
{
  GameApi::P p = p_empty();
  update_vertex_array(va,p);
}
void ProgressBar(int num, int val, int max, std::string label);

#ifdef THREADS
extern ThreadInfo volatile *ti_global;
pthread_cond_t *g_cond;
#endif
extern volatile int thread_counter;

extern std::atomic<bool> g_lock1;
extern std::atomic<bool> g_lock2;
extern std::atomic<bool> g_lock3;

extern long long g_copy_total;
extern long long g_copy_progress;



EXPORT void GameApi::PolygonApi::update_vertex_array(GameApi::VA va, GameApi::P p, bool keep)
{
  if (keep) {
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0); 
    if (!keep)
      s->free_memory();
    add_update_vertex_array(e, va, s, arr2);

  }


#ifdef THREADS


#ifndef BATCHING
  int num_threads = 8;
  FaceCollection *faces = find_facecoll(e, p);
  ThreadedPrepare prep(faces);
  int s = faces->NumFaces();
  if (s<100) { num_threads = 1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "Numthreads: " << num_threads << std::endl;
  for(int i=0;i<num_threads;i++)
    {
      int start_range = i*delta_s;
      int end_range = (i+1)*delta_s;
      if (end_range>s) end_range = s;
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread(start_range, end_range));
    }
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
    }
  VertexArraySet *set = prep.collect();
  RenderVertexArray *arr2 = new RenderVertexArray(g_low, *set);
  arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
    }
  add_update_vertex_array(e, va, set, arr2);
#else // BATCHING
  int num_threads = 8;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();

  ThreadedPrepare prep(faces);  
  int s = faces->NumFaces();   
  //std::cout << "NumFaces: " << s << std::endl;
  if (s<100) { num_threads=1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "NumThreads2: " << num_threads << std::endl;
  Counts ct = CalcCounts(faces, 0, faces->NumFaces());
  VertexArraySet *set = new VertexArraySet;
  RenderVertexArray *arr2 = new RenderVertexArray(g_low,*set);
  arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));
  pthread_mutex_t *mutex1 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *mutex3 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *gmutex = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_mutex_t *gmutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
  pthread_cond_t *g_cond = new pthread_cond_t(PTHREAD_COND_INITIALIZER);
  pthread_cond_t *g_cond2 = new pthread_cond_t(PTHREAD_COND_INITIALIZER);
  thread_counter = 0;
  g_lock1 = false;
  g_lock3 = true;
  g_lock2 = true;
  //pthread_mutex_lock(mutex3); // LOCK mutex3
  //pthread_mutex_lock(mutex2);
  for(int i=0;i<num_threads;i++)
    {  
      int start_range = i*delta_s; 
      int  end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; } 
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread2(start_range, end_range,arr2, mutex1, mutex2,mutex3,gmutex,gmutex2,g_cond,g_cond2));
    }
  int progress = 0;
  //InstallProgress(999,"gpu mem",15);
  long long prev = 0;
  while(1) {
    //std::cout << "lock3 wait" << std::endl;
#if 1
    pthread_mutex_lock(gmutex);
    while(g_lock3==true) {
      pthread_cond_wait(g_cond,gmutex);
      
#ifdef EMSCRIPTEN
      // emscripten_sleep(100);
#endif

      //if (g_copy_progress*15/g_copy_total!=prev)
      //	{
      //	  prev = g_copy_progress*15/g_copy_total;
      //	  ProgressBar(999,prev,15,"gpu mem");
	  
      //	}
	 
      
    }
    pthread_mutex_unlock(gmutex);
#endif
    //g_lock3.wait(false); // wait is available only in c++20
    g_lock3 = true;
    //std::cout << "Lock3 wait end" << std::endl;
    //pthread_mutex_lock(mutex3); // WAIT FOR mutex3 to open.
    progress++;

    // now ti_global is available
    ThreadInfo volatile *ti_global2 = ti_global;
    if (ti_global2) {
      //std::cout << "transfer" << std::endl;
    ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 0, ti_global2->ct2_offsets.tri_count*3, ti_global2->ct2_offsets.tri_count*3 + ti_global2->ct2_counts.tri_count*3); 
    ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 1, ti_global2->ct2_offsets.quad_count*6, ti_global2->ct2_offsets.quad_count*6 + ti_global2->ct2_counts.quad_count*6);
    ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 2, std::max(ti_global2->ct2_offsets.poly_count-1,0), std::max(ti_global2->ct2_offsets.poly_count-1,0) + (ti_global2->ct2_offsets.poly_count?ti_global2->ct2_counts.poly_count:ti_global2->ct2_counts.poly_count-1));
    }
    ti_global = 0;
    //std::cout << "lock 2 release" << std::endl;
    g_lock2 = false;
    pthread_cond_signal(g_cond2);
    //pthread_mutex_unlock(mutex2); // release other process
    if (thread_counter==num_threads) break;
  }
 
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
     }
  pthread_mutex_destroy(mutex1);
  pthread_mutex_destroy(mutex2);
  pthread_mutex_destroy(mutex3);
  pthread_mutex_destroy(gmutex);
  pthread_mutex_destroy(gmutex2);
  pthread_cond_destroy(g_cond);
  pthread_cond_destroy(g_cond2);

  delete mutex1;
  delete mutex2;
  delete mutex3;
    delete gmutex;
    delete gmutex2;
    delete g_cond;
    delete g_cond2;


  //VertexArraySet *set = new VertexArraySet;
  //RenderVertexArray *arr2 = new RenderVertexArray(*set);
  //arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
      //::EnvImpl *env = ::EnvImpl::Environment(&e);
      //env->temp_deletes.push_back(std::shared_ptr<void>( arr2 ) );
    }
  add_update_vertex_array(e, va, set, arr2);
  
#endif

#else
  FaceCollection *faces = find_facecoll(e, p);
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces());  
  RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  add_update_vertex_array(e, va, s, arr2);
#endif

}
EXPORT void GameApi::PolygonApi::explode(VA va, PT pos, float dist)
{
  VertexArraySet *s = find_vertex_array(e, va);
  Point *pt = find_point(e, pos);
  s->explode(0, *pt, dist);
}

EXPORT void GameApi::PolygonApi::create_vertex_array_hw(GameApi::VA va)
{
  //RenderVertexArray *rend = find_vertex_array_render(e, va);
  //VertexArraySet *set = find_vertex_array(e, va);
  //rend->prepare(0);
}

extern bool g_disable_polygons;


bool is_texture_usage_confirmed(const FaceCollection *p);
bool is_texture_usage_confirmed(VertexArraySet *set);



EXPORT GameApi::VA GameApi::PolygonApi::create_vertex_array(GameApi::P p, bool keep)
{

    FaceCollection *faces2 = find_facecoll(e, p);
    if (faces2->HasBatchMap()) {
      //std::cout << "BatchMap rendering" << std::endl;
    //std::cout << "IMPL#1:KEEP" << std::endl;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    //arr.copy(0,faces->NumFaces());


    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(s)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    s->has_normal = has_normal2;
    s->has_attrib = has_attrib2;
    s->has_color = has_color2;
    s->has_texcoord = has_texcoord2;
    s->has_skeleton = has_skeleton2;

    
    FaceBufferRef ref = faces->BatchMap(0,faces->NumFaces());

    int s2 = ref.numfaces;
    //std::cout << "BATCHMAP RENDER: " << s2 << std::endl;
    for(int i=0;i<s2;i++)
      {
	if (ref.indices_int)
	  {
	    if (ref.facepoint) {
	      s->push_poly_with_indices(0,3,ref.facepoint,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20)
	      //print("facepoint",ref.facepoint);
	    }
	    if (ref.facepoint2) {
	      s->push_poly2_with_indices(0,3,ref.facepoint2,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20)
	      //print("facepoint2",ref.facepoint2);
	    }
	    if (ref.pointnormal) {
	      s->push_normal_with_indices(0,3,ref.pointnormal,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20)
	      //print("pointnormal",ref.pointnormal);
	    }
	    if (ref.color) {
	      s->push_color_with_indices(0,3,ref.color,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20) {
	      //std::cout << std::hex;
	      //print("color",ref.color);
	      //std::cout << std::dec;
	      //}
	    }
	    
	    if (ref.texcoords) {
	      s->push_texcoord_with_indices(0,3,ref.texcoords,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20)
	      //print("texcoord",ref.texcoords);
	    }
	    if (ref.joints) {
	      s->push_joint_with_indices(0,3,ref.joints,ref.indices_int+i*3,ref.numvertices);
	      //if (i<20)
	      //print("joints",ref.joints);
	    }
	    if (ref.weights) {
	      s->push_weight_with_indices(0,3,ref.weights,ref.indices_int+i*3,ref.numvertices);	
	      //if (i<20)
	      //print("weigthts",ref.weights);
	    }
	  }
	else
	  {
	    if (ref.facepoint)
	      s->push_poly(0,3,ref.facepoint+i*3);
	    if (ref.facepoint2)
	      s->push_poly2(0,3,ref.facepoint2+i*3);
	    if (ref.pointnormal)
	      s->push_normal(0,3,ref.pointnormal+i*3);
	    if (ref.color)
	      s->push_color(0,3,ref.color+i*3);
	    if (ref.texcoords)
	      s->push_texcoord(0,3,ref.texcoords+i*3);
	    if (ref.joints)
	      s->push_joint(0,3,ref.joints+i*3);
	    if (ref.weights)
	      s->push_weight(0,3,ref.weights+i*3);
	  }
      }


	


    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0);
    if (!keep) {
      s->free_memory();
    }
    return add_vertex_array(e, s, arr2);
    }

    //   std::cout << "Normal rendering" << std::endl;
  
  /*
    FaceCollection *faces2 = find_facecoll(e, p);
    if (faces2->HasBatchMap())
      {
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();




    VertexArraySet *s = new VertexArraySet;


    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(s)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    s->has_normal = has_normal2;
    s->has_attrib = has_attrib2;
    s->has_color = has_color2;
    s->has_texcoord = has_texcoord2;
    s->has_skeleton = has_skeleton2;

    

    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    FaceBufferRef ref = faces->BatchMap(0,faces->NumFaces());
    //s->SetRef(ref);
    //arr.copy(0,faces->NumFaces());

    int s2 = faces->NumFaces();
    for(int i=0;i<s2;i++)
      {
    if (ref.facepoint && ref.indices_int)
      {
	s->push_poly_with_indices(0,3,ref.facepoint,ref.indices_int+i*3);
      }
    else if (ref.facepoint)
    s->push_poly(0,3,ref.facepoint+i*3);
    if (ref.facepoint2 && ref.indices_int)
      {
	s->push_poly2_with_indices(0,3,ref.facepoint2,ref.indices_int+i*3);
      }
    else if (ref.facepoint2)
    s->push_poly2(0,3,ref.facepoint2+i*3);
    if (ref.pointnormal)
    s->push_normal(0,3,ref.pointnormal+i*3);
    if (ref.color)
    s->push_color(0,3,ref.color+i*3);
    if (ref.texcoords)
    s->push_texcoord(0,3,ref.texcoords+i*3);
    if (ref.joints)
    s->push_joint(0,3,ref.joints+i*3);
    if (ref.weights)
    s->push_weight(0,3,ref.weights+i*3);
      }
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0);
    //arr2->prepare_indices(0,ref.indices_char,ref.indices_short,ref.indices_int, ref.numfaces);
    //arr2->tri_count = ref.numvertices;
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
}
  */
  
  if (keep) {
    //std::cout << "IMPL#1:KEEP" << std::endl;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0);
    if (!keep) {
      s->free_memory();
    }
    return add_vertex_array(e, s, arr2);
  }
  
#ifdef THREADS
#ifdef EMSCRIPTEN
#ifdef __EMSCRIPTEN_PTHREADS__
  if (1) {
#else
    if (1) {
#endif // END OF __EMSCRIPTEN_PTHREADS
#endif // END OF EMSCRIPTEN
    
#ifndef BATCHING // THREADS=true, EMSCRIPTEN=??, BATCHING=false
      //std::cout << "IMPL#2:BATCHING=false, THREADS=true" << std::endl;
    int num_threads = 8;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    //std::cout << "FaceColl: " << faces << " " << faces->NumFaces() << std::endl;
    ThreadedPrepare prep(faces);  
    int s = faces->NumFaces();   
    //std::cout << "NumFaces: " << s << std::endl;
    if (s<100) { num_threads=1; }
    int delta_s = s/num_threads+1;
    std::vector<int> vec;
    //std::cout << "NumThreads2: " << num_threads << std::endl;
    std::cout << "Threads, no batching: numthreads=" << num_threads << std::endl;
    for(int i=0;i<num_threads;i++)
      {  
	int start_range = i*delta_s; 
	int  end_range = (i+1)*delta_s;
	if (end_range>s) { end_range = s; } 
	if (i==num_threads-1) {end_range = s; }
	vec.push_back(prep.push_thread(start_range, end_range));
    }
    for(int i=0;i<num_threads;i++)
      {
	prep.join(vec[i]);
      }
    VertexArraySet *set = prep.collect();

    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(set)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    set->has_normal = has_normal2;
    set->has_attrib = has_attrib2;
    set->has_color = has_color2;
    set->has_texcoord = has_texcoord2;
    set->has_skeleton = has_skeleton2;

    

    RenderVertexArray *arr2 = new RenderVertexArray(g_low,*set);
    arr2->prepare(0);
    if (!keep)
      {
	set->free_memory();
	//::EnvImpl *env = ::EnvImpl::Environment(&e);
	//env->temp_deletes.push_back(std::shared_ptr<void>( arr2 ) );
      }
#else // BATCHING=true, EMSCRIPTEN=??, THREADS=true
    //std::cout << "IMPL#3:BATCHING=true, THREADS=true" << std::endl;
    int num_threads = 8;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    
    ThreadedPrepare prep(faces);  
    int s = faces->NumFaces();
    if (s==0) {
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0);
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);

    }
    
    //std::cout << "NumFaces: " << s << std::endl;
    if (s<100) { num_threads=1; }
    int delta_s = s/num_threads+1;
    std::vector<int> vec;
    //std::cout << "NumThreads2: " << num_threads << std::endl;
    Counts ct = CalcCounts(faces, 0, faces->NumFaces());
    VertexArraySet *set = new VertexArraySet;
    set->check_m_set(0);
    
    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(set)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    set->has_normal = has_normal2;
    set->has_attrib = has_attrib2;
    set->has_color = has_color2;
    set->has_texcoord = has_texcoord2;
    set->has_skeleton = has_skeleton2;
    


    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *set);
    arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));
    pthread_mutex_t *mutex1 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
    pthread_mutex_t *mutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
    pthread_mutex_t *mutex3 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
	pthread_mutex_t *gmutex = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
	pthread_mutex_t *gmutex2 = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
	pthread_cond_t *g_cond = new pthread_cond_t(PTHREAD_COND_INITIALIZER);
	pthread_cond_t *g_cond2 = new pthread_cond_t(PTHREAD_COND_INITIALIZER);

    thread_counter = 0;
    g_lock1 = false;
    g_lock2 = true;
    g_lock3 = true;
    //pthread_mutex_lock(mutex3); // LOCK mutex3
    //pthread_mutex_lock(mutex2);
    for(int i=0;i<num_threads;i++)
      {
	
	int start_range = i*delta_s; 
	int  end_range = (i+1)*delta_s;
	if (end_range>s) { end_range = s; }
	//std::cout << "THREAD#" << i << "::Range=(" << start_range << ".." << end_range << ")" << std::endl;
	if (i==num_threads-1) {end_range = s; }
	vec.push_back(prep.push_thread2(start_range, end_range,arr2, mutex1, mutex2,mutex3,gmutex,gmutex2,g_cond,g_cond2));
      }
    int progress = 0;
    //InstallProgress(1,"send to gpu mem",10);
    unsigned int time = g_low->sdl->SDL_GetTicks();
    bool error = false;
    while(1) {
      //std::cout << "wait 3" << std::endl;
#if 1
      pthread_mutex_lock(gmutex);
      while(g_lock3==true) {
	pthread_cond_wait(g_cond,gmutex);
	//if (g_low->sdl->SDL_GetTicks()-time > 10000.0) { std::cout << "create_vertex_array: BATCHING EXITING ON 10s TIMER" << std::endl; error=true; break; }
#ifdef EMSCRIPTEN
	///	emscripten_sleep(100);
#endif
      }
      pthread_mutex_unlock(gmutex);
#endif
      // g_lock3.wait(); // wait is available in c++20 only
      g_lock3 = true;
      //std::cout << "wait 3 end" << std::endl;
      //pthread_mutex_lock(mutex3); // WAIT FOR mutex3 to open.
      progress++;
      //ProgressBar(1,progress*10/num_threads,10,"send to gpu mem");
      
      // now ti_global is available
      ThreadInfo volatile *ti_global2 = ti_global;
      if (ti_global2 && !error) {
	//std::cout << "transfer"<< std::endl;

	//std::cout << "TRANSFER::ctcounts(" << ti_global2->ct2_counts.tri_count << " " << ti_global2->ct2_counts.quad_count << " " << ti_global2->ct2_counts.poly_count << ")" << std::endl;
	//std::cout << "TRANSFER::ctoffsets(" << ti_global2->ct2_offsets.tri_count << " " << ti_global2->ct2_offsets.quad_count << " " << ti_global2->ct2_offsets.poly_count << ")" << std::endl;

	
	ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 0, ti_global2->ct2_offsets.tri_count *3, ti_global2->ct2_offsets.tri_count *3 + ti_global2->ct2_counts.tri_count *3); 
      ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 1, ti_global2->ct2_offsets.quad_count *6, ti_global2->ct2_offsets.quad_count *6 + ti_global2->ct2_counts.quad_count *6);
      if (!g_disable_polygons)
      ti_global2->prep->transfer_to_gpu_mem(ti_global2->set, *ti_global2->r, 0, 2, std::max(ti_global2->ct2_offsets.poly_count-1,0), std::max(ti_global2->ct2_offsets.poly_count-1,0) + (ti_global2->ct2_offsets.poly_count?ti_global2->ct2_counts.poly_count:ti_global2->ct2_counts.poly_count-1));
      }
      ti_global = 0;
      //std::cout << "rel 2" << std::endl;
      g_lock2 = false;
      pthread_mutex_lock(gmutex2);
      pthread_cond_signal(g_cond2);
      pthread_mutex_unlock(gmutex2);
      //pthread_mutex_unlock(mutex2); // release other process
      if (thread_counter==num_threads||error) break;
    }
    
    for(int i=0;i<num_threads;i++)
      {
	if (error) break;
	prep.join(vec[i]);
      }
    pthread_mutex_destroy(mutex1);
    pthread_mutex_destroy(mutex2);
    pthread_mutex_destroy(mutex3);
  pthread_mutex_destroy(gmutex);
  pthread_mutex_destroy(gmutex2);
  pthread_cond_destroy(g_cond);
  pthread_cond_destroy(g_cond2);
    delete mutex1;
    delete mutex2;
    delete mutex3;
    delete gmutex;
    delete gmutex2;
    delete g_cond;
    delete g_cond2;
    //VertexArraySet *set = new VertexArraySet;
    //RenderVertexArray *arr2 = new RenderVertexArray(*set);
    //arr2->prepare(0);
    if (!keep)
      {
    	set->free_memory();
    }
    
    
#endif // BATCHING

    
#ifdef EMSCRIPTEN
    return add_vertex_array(e, set, arr2);
    //#ifdef __EMSCRIPTEN_PTHREADS__
  } else {
    //#endif
#ifndef BATCHING
      //std::cout << "IMPL#4:BATCHING=false, EMSCRIPTEN=true, THREADS=true" << std::endl;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    VertexArraySet *s = new VertexArraySet;
    FaceCollectionVertexArray2 arr(*faces, *s);
    arr.reserve(0);
    arr.copy(0,faces->NumFaces());  
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    arr2->prepare(0); 
    if (!keep)
     s->free_memory();
    return add_vertex_array(e, s, arr2);
#else // BATCHING
    // std::cout << "IMPL#5:BATCHING=true, EMSCRIPTEN=true, THREADS=true" << std::endl;

    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    int total_faces = faces->NumFaces();
    int batch_count = total_faces/100000;
    if (batch_count<1) batch_count=1;
    if (total_faces<100) batch_count=1;
    int batch_faces = faces->NumFaces()/batch_count+1;
    Counts ct = CalcCounts(faces, 0, faces->NumFaces());
    VertexArraySet *s = new VertexArraySet;
    s->check_m_set(0);

    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(s)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    s->has_normal = has_normal2;
    s->has_attrib = has_attrib2;
    s->has_color = has_color2;
    s->has_texcoord = has_texcoord2;
    s->has_skeleton = has_skeleton2;

    
    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
    std::cout << "Counts: " << ct.tri_count << " " <<  ct.quad_count << " " << ct.poly_count << std::endl;
    //if (ct.tri_count==0 && ct.quad_count==0 && ct.poly_count==0) return;
    arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));  // SIZES MUST BE KNOWN
    //InstallProgress(2,"batching");
    for(int i=0;i<batch_count;i++) {
      //ProgressBar(2,i,batch_count,"batching");
      int start = i*batch_faces;
      int end = (i+1)*batch_faces;
      if (start>total_faces) { start=total_faces; }
      if (end>total_faces) { end=total_faces; }
      std::cout << "BATCH:" << start << " " << end << "::" << total_faces << std::endl;
      Counts ct2_counts = CalcCounts(faces, start, end);
      Counts ct2_offsets = CalcOffsets(faces, start);
      FaceCollectionVertexArray2 arr(*faces, *s);

    std::cout << "ct2_Counts: " << ct2_counts.tri_count << " " <<  ct2_counts.quad_count << " " << ct2_counts.poly_count << std::endl;
    std::cout << "ct2_Offsets: " << ct2_offsets.tri_count << " " <<  ct2_offsets.quad_count << " " << ct2_offsets.poly_count << std::endl;

      
      //arr.reserve(0);
      arr.copy(start,end);
      if (ct2_counts.tri_count!=0)
	arr2->update_tri(0, 0, ct2_offsets.tri_count*3, ct2_offsets.tri_count*3 + ct2_counts.tri_count*3);
      if (ct2_counts.quad_count!=0)
	arr2->update_tri(0, 1, ct2_offsets.quad_count*6, ct2_offsets.quad_count*6 + ct2_counts.quad_count*6);
      if (ct2_counts.poly_count!=0 && !g_disable_polygons)
	arr2->update_tri(0, 2, std::max(ct2_offsets.poly_count-1,0), std::max(ct2_offsets.poly_count-1,0) + (ct2_offsets.poly_count?ct2_counts.poly_count:ct2_counts.poly_count-1));
      s->free_reserve(0);
    }
    //std::cout << "\n";
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
#endif // BATCHING
    //#ifdef __EMSCRIPTEN_PTHREADS__
#ifdef THREADS
      }
#else
  }
#endif
  //#endif
#else // EMSCRIPTEN
  return add_vertex_array(e, set, arr2);
#endif // end of EMSCRIPTEN

#else // THREADS
#ifndef BATCHING // BATCHING=false, THREADS=false
  //  std::cout << "IMPL#6:BATCHING=false, THREADS=false" << std::endl;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces());  
  RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  return add_vertex_array(e, s, arr2);

#else // BATCHING=true, THREADS=false
  // std::cout << "IMPL#7:BATCHING=true, THREADS=false" << std::endl;
    FaceCollection *faces = find_facecoll(e, p);
    faces->Prepare();
    int total_faces = faces->NumFaces();
    int batch_count = total_faces/100000+1;
    if (total_faces<100000) batch_count=1;
    if (total_faces<100) batch_count=1;
    if (batch_count==0) batch_count=1;

    
    int batch_faces = total_faces/batch_count+1;
    if (batch_faces>total_faces) batch_faces=total_faces;
    Counts ct = CalcCounts(faces, 0, total_faces);
    VertexArraySet *s = new VertexArraySet;
    s->check_m_set(0);

    FaceCollection &coll = *faces;
    bool has_normal2 = coll.has_normal();
    bool has_attrib2 = coll.has_attrib();
    bool has_color2 = coll.has_color();
    bool has_texcoord2 = coll.has_texcoord() && (is_texture_usage_confirmed(s)||is_texture_usage_confirmed(&coll));
    bool has_skeleton2 = coll.has_skeleton();
    
    s->has_normal = has_normal2;
    s->has_attrib = has_attrib2;
    s->has_color = has_color2;
    s->has_texcoord = has_texcoord2;
    s->has_skeleton = has_skeleton2;
    

    RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);

    arr2->prepare(0,true,ct.tri_count*3, ct.quad_count*6, std::max(ct.poly_count-1,0));  // SIZES MUST BE KNOWN
    for(int i=0;i<batch_count;i++) {
      int start = i*batch_faces;
      int end = (i+1)*batch_faces;
      if (start>total_faces) { start=total_faces; }
      if (end>total_faces) { end=total_faces; }
      if (i==batch_count-1) { end=total_faces; }
      Counts ct2_counts = CalcCounts(faces, start, end);
      Counts ct2_offsets = CalcOffsets(faces, start);
      FaceCollectionVertexArray2 arr(*faces, *s);

      arr.reserve_fixed2(0,ct2_counts.tri_count,ct2_counts.quad_count,ct2_counts.poly_count);
      arr.copy(start,end);
      if (ct2_counts.tri_count!=0)
	arr2->update_tri(0, 0, ct2_offsets.tri_count*3, ct2_offsets.tri_count*3 + ct2_counts.tri_count*3);

      if (ct2_counts.quad_count!=0)
	arr2->update_tri(0, 1, ct2_offsets.quad_count*6, ct2_offsets.quad_count*6 + ct2_counts.quad_count*6);
      if (ct2_counts.poly_count!=0 && !g_disable_polygons)
	arr2->update_tri(0, 2, std::max(ct2_offsets.poly_count-1,0), std::max(ct2_offsets.poly_count-1,0) + (ct2_offsets.poly_count?ct2_counts.poly_count:ct2_counts.poly_count-1));
      s->free_reserve(0);
    }
    //std::cout << "\n";
    if (!keep)
      s->free_memory();
    return add_vertex_array(e, s, arr2);
#endif // end of BATCHING
#endif // end of THREADS
}
  
EXPORT GameApi::VA GameApi::PolygonApi::create_vertex_array_attribs(GameApi::P p, bool keep, std::vector<int> attribs, std::vector<int> attribi)
{ 
#ifdef THREADS
  int num_threads = 8;
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
  //std::cout << "FaceColl: " << faces << " " << faces->NumFaces() << std::endl; 
  ThreadedPrepare prep(faces);  
  int s = faces->NumFaces();   
  //std::cout << "NumFaces: " << s << std::endl;
  if (s<100) { num_threads=1; }
  int delta_s = s/num_threads+1;
  std::vector<int> vec;
  //std::cout << "NumThreads2: " << num_threads << std::endl;
  for(int i=0;i<num_threads;i++)
    {  
      int start_range = i*delta_s; 
      int  end_range = (i+1)*delta_s;
      if (end_range>s) { end_range = s; } 
      if (i==num_threads-1) {end_range = s; }
      vec.push_back(prep.push_thread(start_range, end_range,attribs,attribi));
    }
  for(int i=0;i<num_threads;i++)
    {
      prep.join(vec[i]);
    }
  VertexArraySet *set = prep.collect();
  RenderVertexArray *arr2 = new RenderVertexArray(g_low, *set);
  arr2->prepare(0);
  if (!keep)
    {
      set->free_memory();
    }
  return add_vertex_array(e, set, arr2);
#else
  FaceCollection *faces = find_facecoll(e, p);
  faces->Prepare();
  VertexArraySet *s = new VertexArraySet;
  FaceCollectionVertexArray2 arr(*faces, *s);
  arr.reserve(0);
  arr.copy(0,faces->NumFaces(),attribs,attribi);  
  RenderVertexArray *arr2 = new RenderVertexArray(g_low, *s);
  arr2->prepare(0); 
  if (!keep)
    s->free_memory();
  return add_vertex_array(e, s, arr2);
#endif
}


#if 0
EXPORT int GameApi::PolygonApi::access_point_count(VA va, bool triangle)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      return s->tri_count(0);
    }
  else
    {
      return s->quad_count(0);
    }
}
EXPORT float *GameApi::PolygonApi::access_points(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point *p = s->tri_polys(0);
      return &((float*)p)[face*3*3+point*3];
    }
  else
    {
      const Point *p = s->quad_polys(0);
      return &((float*)p)[face*4*3+point*3];
    }
  return 0;
}
EXPORT float *GameApi::PolygonApi::access_color(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle) {
    const float *ptr = s->tri_color_polys(0);
    return &((float*)ptr)[face*3*4+point*4];
  }
  else
    {
    const float *ptr = s->quad_color_polys(0);
    return &((float*)ptr)[face*4*4+point*4];      
    }
}
EXPORT float *GameApi::PolygonApi::access_normals(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Vector *ptr = s->tri_normal_polys(0);
      return &((float*)ptr)[face*3*3+point*3];
    }
  else
    {
      const Vector *ptr = s->quad_normal_polys(0);
      return &((float*)ptr)[face*4*3+point*3];
    }
}
EXPORT float *GameApi::PolygonApi::access_texcoord(VA va, bool triangle, int face, int point)
{
  VertexArraySet *s = find_vertex_array(e, va);
  if (triangle)
    {
      const Point2d *ptr = s->tri_texcoord_polys(0);
      return &((float*)ptr)[face*3*2+point*2];
    }
  else
    {
      const Point2d *ptr = s->quad_texcoord_polys(0);
      return &((float*)ptr)[face*4*2+point*2];
    }
}
#endif
class ChooseTex : public ForwardFaceCollection
{
public:
  ChooseTex(FaceCollection *coll, int val) : ForwardFaceCollection(*coll),val(val) { }
  virtual std::string name() const { return "ChooseTex"; }
  bool has_texcoord() const { return true; }
  virtual float TexCoord3(int face, int point) const { 
    return float(val)+0.5f;
  }
private:
  int val;
};
#if 0
class DefaultTex : public ForwardFaceCollection
{
public:
  DefaultTex(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  bool has_texcoor() const { return true; }
  Point2d TexCoord(int face, int point) const
  {
    if (point==0) {
      Point2d p0 = { 0.0, 0.0 }; return p0; 
    }
    if (point==1) {
      Point2d p1 = { 1.0, 0.0 }; return p1; 
    }
    if (point==2) {
      Point2d p2 = { 1.0, 1.0 }; return p2; 
    }
    if (point==3) {
      Point2d p3 = { 0.0, 1.0 }; return p3; 
    }
    Point2d p4;
    return p4;
    
  }
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::texcoord_default(P orig)
{
#if 0
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new DefaultTex(coll), 1);
#endif
}
EXPORT GameApi::P GameApi::PolygonApi::choose_texture(P orig, int num)
{
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new ChooseTex(coll, num), 1);
}
EXPORT void GameApi::PolygonApi::update(VA va)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *s2 = find_vertex_array_render(e, va);
  s2->update(0);
}
class RenderVA : public MainLoopItem
{
public:
  RenderVA(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::VA va) : env(env), ev(ev), api(api), va(va)
  {
    shader.id = -1;
    firsttime = true;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  void Prepare() { }
  void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      ev.polygon_api.create_vertex_array_hw(va);
    }

    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//ev.shader_api.use(sh);
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
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::SH shader;
};

class RenderP : public MainLoopItem
{
public:
  RenderP(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p) : env(env), ev(ev), api(api), p(p)
  {
    shader.id = -1;
    firsttime = true;
    va.id=0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
	va = ev.polygon_api.create_vertex_array(p,false);
    }
  }

  void Prepare() {
    if (va.id==0) {
	va = ev.polygon_api.create_vertex_array(p,false);
    }
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	ev.polygon_api.create_vertex_array_hw(va);
      }

    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (firsttime && shader.id==-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
#if 1
    //ev.shader_api.use(sh);
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
	//ev.shader_api.set_var(shader, "time", e.time);

	sh = shader;
#endif
	}
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    ev.shader_api.set_var(sh, "in_POS", e.in_POS);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
};

std::string get_shader_path(GameApi::Env &e, GameApi::US us)
{
  std::string s;
  ShaderCall *call = find_uber(e,us);
  s+=call->cache_id();
  while(call->get_next())
    {
      call = call->get_next();
      s+=":";
      s+=call->cache_id();
    }
  return s;
}

struct PTexCacheItem
{
  GameApi::Env *e;
  GameApi::US vertex;
  GameApi::US fragment;
  std::string v_shader_functions;
  std::string f_shader_functions;
  GameApi::SH shader;
  friend bool operator==(const PTexCacheItem &i1, const PTexCacheItem &i2)
  {
    return get_shader_path(*i1.e,i1.vertex)==get_shader_path(*i2.e,i2.vertex) && get_shader_path(*i1.e,i1.fragment)==get_shader_path(*i2.e,i2.fragment) &&
      i1.v_shader_functions==i2.v_shader_functions &&
      i1.f_shader_functions==i2.f_shader_functions;
  }
};

std::vector<PTexCacheItem> ptex_cache;

GameApi::SH find_ptex_shader(const PTexCacheItem &ii)
{
  int s = ptex_cache.size();
  for(int i=0;i<s;i++)
    {
      if (ii==ptex_cache[i]) return ptex_cache[i].shader;
    }
  GameApi::SH sh;
  sh.id = -1;
  return sh;
}


class RenderPTex : public MainLoopItem
{
public:
  RenderPTex(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, std::vector<GameApi::BM> bm, std::vector<int> types, std::vector<std::string> id_labels) : env(env), ev(ev), api(api), p(p), bm(bm), types(types),id_labels(id_labels)
  {
    shader.id = -1;
    firsttime = true;
    va.id=0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
      FaceCollection *coll = find_facecoll(env,p);
      if (coll->NumFaces()==0) disabled=true;
      else disabled=false;
      //std::cout << "NumFaces0:" << coll->NumFaces() << std::endl;
    }
  }

  void Prepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p, false); // should be true
      FaceCollection *coll = find_facecoll(env,p);
      if (coll->NumFaces()==0) disabled=true;
      else disabled=false;
      //std::cout << "NumFaces1:" << coll->NumFaces() << std::endl;
    }
  }
  void execute(MainLoopEnv &e)
  {
    if (firsttime||(disabled&&find_facecoll(env,p)->NumFaces()>0))
      {
	if (va.id==0) {

	  va = ev.polygon_api.create_vertex_array(p, false); // should be true
	  FaceCollection *coll = find_facecoll(env,p);
	  if (coll->NumFaces()==0) disabled=true;
	  else disabled=false;
      //std::cout << "NumFaces2:" << coll->NumFaces() << std::endl;
	  std::cout << "Warning: RenderPTex Prepare() not called!" << std::endl;
	}
	ev.polygon_api.create_vertex_array_hw(va);
	std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev, bm, types,false,id_labels);
	va = ev.texture_api.bind_many(va, id, types);
      }
    if (disabled) {
      if (firsttime) { firsttime = false; }
      return;
    }

    GameApi::SH sh;
    GameApi::US u_v;
    GameApi::US u_f;
    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1) {

    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (firsttime)
      {
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif
    }
    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    
	    PTexCacheItem item;
	    item.e = &env;
	    item.vertex = vertex;
	    item.fragment = fragment;
	    item.v_shader_functions = e.v_shader_functions;
	    item.f_shader_functions = e.f_shader_functions;
	    //std::cout << get_shader_path(env,item.vertex) << " " << get_shader_path(env,item.fragment) << " " << item.v_shader_functions << " " << item.f_shader_functions << std::endl;
	    GameApi::SH sh2 = find_ptex_shader(item);
	    if (sh2.id==-1)
	      {
	    
	    //std::cout << "hep" << std::endl;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	    
	    item.shader = shader;
	    ptex_cache.push_back(item);
	    
	ev.mainloop_api.init_3d(shader);
	   } else shader=sh2;
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	ev.mainloop_api.init_3d(shader);
	  }
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//ev.shader_api.use(sh);
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
	api.sort_vertices(va,m);

      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
  std::vector<GameApi::BM> bm;
  std::vector<int> types;
  std::vector<std::string> id_labels;
  bool disabled=false;
};

class RenderPTex_id : public MainLoopItem
{
public:
  RenderPTex_id(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, std::vector<GameApi::TXID> *bm) : env(env), ev(ev), api(api), p(p), bm(bm)
  {
    shader.id = -1;
    firsttime = true;
    va.id=0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
    }
  }

  void Prepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p, false); // should be true
    }
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	if (va.id==0) {
	  va = ev.polygon_api.create_vertex_array(p, false); // should be true
	  std::cout << "Warning: RenderPTex_id Prepare() not called!" << std::endl;
	}
	ev.polygon_api.create_vertex_array_hw(va);
	std::vector<GameApi::TXID> id = *bm; //ev.texture_api.prepare_many(ev, bm);
	va = ev.texture_api.bind_many(va, id);

      }
    // dynamically change the texture ids.
    VertexArraySet *s = find_vertex_array(env, va);
    s->texture_many_ids = std::vector<int>();
    int sk = bm->size();
    for(int i=0;i<sk;i++)
      {
	GameApi::TXID id = bm->operator[](i);
	TextureID *iid = find_txid(env,id);
	int tex = iid->texture();
	s->texture_many_ids.push_back(tex);
      }

    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	//ev.shader_api.use(sh);
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
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
  std::vector<GameApi::TXID> *bm;
};


class RenderPTexCubemap : public MainLoopItem
{
public:
  RenderPTexCubemap(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, std::vector<GameApi::BM> bm) : env(env), ev(ev), api(api), p(p), bm(bm)
  {
    shader.id = -1;
    firsttime = true;
    va.id=0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
    }
  }

  void Prepare() {
    if (va.id==0){
      va = ev.polygon_api.create_vertex_array(p, false); // should be true
    }
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	if (va.id==0) {
	  va = ev.polygon_api.create_vertex_array(p, false); // should be true
	  std::cout << "Warning: RenderPTexCubemap Prepare() not called!" << std::endl;
	}

	ev.polygon_api.create_vertex_array_hw(va);
	GameApi::BM right,left,top,bottom,back,front;
	right.id = 0;
	left.id = 0;
	top.id = 0;
	bottom.id = 0;
	back.id = 0;
	front.id = 0;
	if (bm.size()>0) right = bm[0];
	if (bm.size()>1) left = bm[1];
	if (bm.size()>2) top = bm[2];
	if (bm.size()>3) bottom = bm[3];
	if (bm.size()>4) back = bm[4];
	if (bm.size()>5) front = bm[5];
	//std::cout << "bm array size=" << bm.size() << std::endl;
	int s = bm.size();
	for(int i=0;i<s;i++) std::cout << bm[i].id << std::endl;
	if (left.id==0) left=right;
	if (top.id==0) top=right;
	if (bottom.id==0) bottom=right;
	if (back.id==0) back=right;
	if (front.id==0) front=right;
	GameApi::TXID id = ev.texture_api.prepare_cubemap(ev, right,left,top,bottom,back,front);
	va = ev.texture_api.bind_cubemap(va, id);

      }


    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {

	//ev.shader_api.use(shader);
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
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
  std::vector<GameApi::BM> bm;
};


class RenderPTex2 : public MainLoopItem
{
public:
  RenderPTex2(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p) : env(env), ev(ev), api(api), p(p)
  {
    shader.id = -1;
    firsttime = true;
    va.id=0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
    }
  }
  void Prepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p, false); // should be true
    }
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	if (va.id==0) {
	  va = ev.polygon_api.create_vertex_array(p, false); // should be true
	  std::cout << "Warning: RenderPTex2 Prepare() not called!" << std::endl;
	}

	ev.polygon_api.create_vertex_array_hw(va);
	// This loop fetches the textures from P type
	std::vector<GameApi::BM> bm;
	int s = ev.polygon_api.p_num_textures(p);
	for(int i=0;i<s;i++) {
	  ev.polygon_api.p_gen_texture(p,i);
	  bm.push_back(ev.polygon_api.p_texture(p,i));
	}
	
	std::vector<GameApi::TXID> id = ev.texture_api.prepare_many(ev, bm);
	va = ev.texture_api.bind_many(va, id);
	
      }


    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0) {
	  std::cout << "RenderPTex2::V_Empty" << std::endl;
	  u_v = ev.uber_api.v_empty();
	}
	if (u_f.id == 0) {
	  std::cout << "RenderPTex2::F_Empty" << std::endl;
	  u_f = ev.uber_api.f_empty(false);
	}
      }

#if 1
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (firsttime)
	  {
	    if (e.us_vertex_shader==-1) {	
	      std::cout << "RenderPTex2::V_Texture" << std::endl;
	      u_v = ev.uber_api.v_texture(u_v);
	    }
	    if (e.us_fragment_shader==-1) {
	      std::cout << "RenderPTex2::F_Texture" << std::endl;
	      u_f = ev.uber_api.f_texture(u_f);
	    }
	    if (ev.polygon_api.is_array_texture(va))
	      {
		sh.id = e.sh_array_texture;
		if (firsttime)
		  {
		    if (e.us_vertex_shader==-1) {
		      std::cout << "RenderPTex2::V_Texture_Arr" << std::endl;
		      u_v = ev.uber_api.v_texture_arr(u_v);
		    }
		    if (e.us_fragment_shader==-1) {
		      std::cout << "RenderPTex2::F_Texture_ARr" << std::endl;
		      u_f = ev.uber_api.f_texture_arr(u_f);
		    }
		  }
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
	      std::cout << "RenderPTex2::V_Colour" << std::endl;
		u_v = ev.uber_api.v_colour(u_v);
	      std::cout << "RenderPTex2::V_Light" << std::endl;
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
	      std::cout << "RenderPTex2::F_Colour" << std::endl;
		u_f = ev.uber_api.f_colour(u_f);
	      std::cout << "RenderPTex2::F_Light" << std::endl;
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    // std::cout << "RenderPTex2::Shadder" << std::endl;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    //  std::cout << "RenderPTex2::Shadder" << std::endl;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
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
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array(va);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::SH shader;
};


class RenderDynP : public MainLoopItem
{
public:
  RenderDynP(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::PolygonApi &api, GameApi::P p, GameApi::DC dc) : env(env), ev(ev), api(api), p(p), dc(dc)
  {
    shader.id = -1;
    firsttime = true;
    va.id = 0;
  }
  std::vector<int> shader_id() { if (shader.id>=0) return std::vector<int>{shader.id}; return std::vector<int>(); }
  //  int shader_id() { return shader.id; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis) {
    FaceCollection *coll = find_facecoll(env,p);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
	ev.polygon_api.clone(va);
    }
  }
  void Prepare() {
    if (va.id==0) {
      va = ev.polygon_api.create_vertex_array(p,false); // should be true
	ev.polygon_api.clone(va);
    }
  }
  void execute(MainLoopEnv &e)
  { 
    if (firsttime)
      {
	ev.polygon_api.create_vertex_array_hw(va);
      }

    GameApi::SH sh;
    if (ev.polygon_api.is_texture(va))
      {
	sh.id = e.sh_texture;
	if (ev.polygon_api.is_array_texture(va))
	  {
	    sh.id = e.sh_array_texture;
	  }
      }
    else
      {
	sh.id = e.sh_color;
      }

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
	if (u_v.id == 0)
	  u_v = ev.uber_api.v_empty();
	if (u_f.id == 0)
	  u_f = ev.uber_api.f_empty(false);
      }

#if 1
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
		u_v = ev.uber_api.v_light(u_v);
	      }
	    if (e.us_fragment_shader==-1)
	      {
		u_f = ev.uber_api.f_colour(u_f);
		u_f = ev.uber_api.f_light(u_f);
	      }
	  }
      }
#endif

    if (shader.id==-1 && e.us_vertex_shader!=-1 && e.us_fragment_shader!=-1)
      {
	GameApi::US vertex;
	GameApi::US fragment;
	vertex.id = u_v.id; //e.us_vertex_shader;
	fragment.id = u_f.id; //e.us_fragment_shader;
	if (e.sfo_id==-1)
	  {
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
	  }
	else
	  {
	    GameApi::SFO sfo;
	    sfo.id = e.sfo_id;
	    shader = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions, false, sfo);
	  }
	ev.mainloop_api.init_3d(shader);
	ev.mainloop_api.alpha(true); 

      }

    if (shader.id!=-1)
      {
	ev.shader_api.use(sh);
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
      }
    if (firsttime) { firsttime = false; }
    ev.shader_api.use(sh);
    api.render_vertex_array_dyn(va,dc,e);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::PolygonApi &api;
  bool firsttime;
  GameApi::VA va;
  GameApi::P p;
  GameApi::DC dc;
  GameApi::SH shader;
};


class NoiseShaderML : public MainLoopItem
{
public:
  NoiseShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="snoise";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="snoise";
#endif
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_snoise(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_snoise(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    next->execute(ee);
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  //int shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};

#if 0
class CustomShaderML : public MainLoopItem
{
public:
  CustomShaderML(GameApi::EveryApi &ev, MainLoopItem *next, std::string v_shaderstring, std::string f_shaderstring, std::string v_funcname, std::string f_funcname) : ev(ev), next(next), v_shaderstring(v_shaderstring), f_shaderstring(f_shaderstring), v_funcname(v_funcname), f_funcname(f_funcname) 
  {
    firsttime = true;
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_custom(vertex, v_funcname);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_custom(fragment, f_funcname);
    ee.us_fragment_shader = a2f.id;
    }

    int s = ee.v_shader_funcnames.size();
    bool v_add=true;
    for(int i=0;i<s;i++)
      {
	if (ee.v_shader_funcnames[i] == v_funcname) v_add=false;
      }
    int s2 = ee.f_shader_funcnames.size();
    bool f_add=true;
    for(int i=0;i<s2;i++)
      {
	if (ee.f_shader_funcnames[i] == f_funcname) f_add=false;
      }
    if (v_add)
      {
      ee.v_shader_functions += v_shaderstring;
      ee.v_shader_funcnames.push_back(v_funcname);
      }
    if (f_add)
      {
      ee.f_shader_functions += f_shaderstring;
      ee.f_shader_funcnames.push_back(f_funcname);
      }

    std::vector<int> sh_ids = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    int sh2 = sh_ids.size();
    for(int i=0;i<sh2;i++) {
      int sh_id = sh_ids[i];
      if (sh_id!=-1)
	{
	  //GameApi::SH sh;
	  sh.id = sh_id;
	  ev.shader_api.use(sh);
	  ev.shader_api.set_var(sh, "time",float(e.time*10.0));
      }
    }

    next->execute(ee);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  std::string v_shaderstring, f_shaderstring;
  std::string v_funcname, f_funcname;
};
#endif

#if 0
class ColorMixShaderML : public MainLoopItem
{
public:
  ColorMixShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next), mix(mix) { firsttime=true; }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_mix(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_mix(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    std::vector<int> sh_ids = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix3", mix);
      }
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};
#endif  


class TextureShaderML : public MainLoopItem
{
public:
  TextureShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_texture(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_texture(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    std::vector<int> sh_ids = next->shader_id();
    //std::cout << "sh_id" << sh_id << std::endl;
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
      }
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};



class TextureManyShaderML : public MainLoopItem
{
public:
  TextureManyShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
    sh.id=-1;
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (sh.id==-1 && firsttime) {
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      // std::cout << "ManyTexture::V_Empty" << std::endl;
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    //std::cout << "ManyTexture::V_ManyTexture" << std::endl;
    GameApi::US a2 = ev.uber_api.v_manytexture(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      //std::cout << "ManyTexture::F_Empty" << std::endl;
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    //std::cout << "ManyTexture::F_ManyTexture" << std::endl;
    GameApi::US a2f = ev.uber_api.f_manytexture(fragment);
    ee.us_fragment_shader = a2f.id;
    }
    if (sh_ids.size()==0) sh_ids=next->shader_id();
    // std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
    //std::cout << "sh_id" << sh_id << std::endl;
      if (sh_id!=-1)
	{
	  //GameApi::SH sh;
	  sh.id = sh_id;
	  ev.shader_api.use(sh);
	  ev.shader_api.set_var(sh, "color_mix", mix);
	}
    }
      if (firsttime) firsttime = false;
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
  std::vector<int> sh_ids;
};

class GLTFShaderML : public MainLoopItem
{
public:
  GLTFShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix, float self_mult, float rest_mult, bool tex0, bool tex1, bool tex2, bool tex3, bool tex4, bool tex5, bool tex6, bool tex7,float roughnessfactor, float metallicfactor, float basecolorfactor0, float basecolorfactor1, float basecolorfactor2, float basecolorfactor3, float occul_strength, float emiss_factor, bool spec, Vector diff_factor, Vector spec_factor, float glossi_factor, bool unlit, float emis2_r, float emis2_g, float emis2_b, Vector light_dir2, std::string cache_id) : ev(ev), next(next),mix(mix), self_mult(self_mult), rest_mult(rest_mult), tex0(tex0), tex1(tex1), tex2(tex2), tex3(tex3), tex4(tex4), tex5(tex5), tex6(tex6), tex7(tex7), roughnessfactor(roughnessfactor), metallicfactor(metallicfactor), basecolorfactor0(basecolorfactor0), basecolorfactor1(basecolorfactor1), basecolorfactor2(basecolorfactor2), basecolorfactor3(basecolorfactor3), occul_strength(occul_strength), emiss_factor(emiss_factor),spec(spec),diff_factor(diff_factor), spec_factor(spec_factor), glossi_factor(glossi_factor), unlit(unlit),emis2(Point(emis2_r,emis2_g,emis2_b)),light_dir2(light_dir2), cache_id(cache_id) 
  {
    firsttime = true;
    sh.id=-1;
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv &ee = e;
    int oldvert = ee.us_vertex_shader;
    int oldfrag = ee.us_fragment_shader;
    if (sh.id==-1) {
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      // std::cout << "ManyTexture::V_Empty" << std::endl;
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    //std::cout << "ManyTexture::V_ManyTexture" << std::endl;
    GameApi::US a2 = ev.uber_api.v_gltf(vertex,cache_id);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      //std::cout << "ManyTexture::F_Empty" << std::endl;
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    //std::cout << "ManyTexture::F_ManyTexture" << std::endl;
    GameApi::US a2f = ev.uber_api.f_gltf(fragment, tex0, tex1, tex2, tex3, tex4,tex5,tex6,tex7,spec,unlit,cache_id);
    ee.us_fragment_shader = a2f.id;
    }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
    //std::cout << "sh_id" << sh_id << std::endl;
      if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix4", mix);
	ev.shader_api.set_var(sh, "color_mult_self", self_mult);
	ev.shader_api.set_var(sh, "color_mult_rest", rest_mult);
	ev.shader_api.set_var(sh, "u_RoughnessFactor", roughnessfactor);
	ev.shader_api.set_var(sh, "u_MetallicFactor", metallicfactor);
	ev.shader_api.set_var(sh, "u_BaseColorFactor", basecolorfactor0,basecolorfactor1,basecolorfactor2, basecolorfactor3);
	ev.shader_api.set_var(sh, "u_OcculsionStrength", occul_strength);
	ev.shader_api.set_var(sh, "u_EmissiveFactor", emiss_factor);
	ev.shader_api.set_var(sh, "u_NormalScale", 1.0f);
	ev.shader_api.set_var(sh, "u_DiffFactor", diff_factor.dx, diff_factor.dy, diff_factor.dz);
	ev.shader_api.set_var(sh, "u_SpecFactor", spec_factor.dx, spec_factor.dy, spec_factor.dz);
	ev.shader_api.set_var(sh, "u_GlossiFactor", glossi_factor);
	ev.shader_api.set_var(sh, "u_EmissiveFactor2", emis2.x,emis2.y,emis2.z);
	// remove texsamplers so that cubesampler would work
	ev.shader_api.set_var(sh, "light_dir2", light_dir2.dx, light_dir2.dy, light_dir2.dz);
	//std::cout << "SETTING LIGHT DIR:" << -light_dir2.dx << " " << -light_dir2.dy << " " << -light_dir2.dz << std::endl;
#if 1
	ev.shader_api.set_var(sh, "texsampler_cube[0]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[1]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[2]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[3]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[4]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[5]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[6]", 9);
	ev.shader_api.set_var(sh, "texsampler_cube[7]", 9);
	ev.shader_api.set_var(sh, "texsampler[0]", 10);
	ev.shader_api.set_var(sh, "texsampler[1]", 10);
	ev.shader_api.set_var(sh, "texsampler[2]", 10);
	ev.shader_api.set_var(sh, "texsampler[3]", 10);
	ev.shader_api.set_var(sh, "texsampler[4]", 10);
	ev.shader_api.set_var(sh, "texsampler[5]", 10);
	ev.shader_api.set_var(sh, "texsampler[6]", 10);
	ev.shader_api.set_var(sh, "texsampler[7]", 10);

#endif
	//std::cout << tex0 << " " << tex1 << " " << tex2<< " " << tex3<< " " << tex4<< " " << tex5 << std::endl;
	
	int count = 0;
	if (tex0) ev.shader_api.set_var(sh, "texsampler[0]", count);
	if (tex0) count++;
	if (tex1) ev.shader_api.set_var(sh, "texsampler[1]", count);
	if (tex1) count++;
	if (tex2) ev.shader_api.set_var(sh, "texsampler[2]", count);
	if (tex2) count++;
	if (tex3) ev.shader_api.set_var(sh, "texsampler[3]", count);
	if (tex3) count++;
	if (tex4) ev.shader_api.set_var(sh, "texsampler[4]", count);
	if (tex4) count++;
	if (tex5) ev.shader_api.set_var(sh, "texsampler_cube[5]", count);
	if (tex5) count++;
	if (tex6) ev.shader_api.set_var(sh, "texsampler_cube[6]", count);
	if (tex6) count++;
	if (tex7) ev.shader_api.set_var(sh, "texsampler[7]", count);
	//std::cout << roughnessfactor << " " << metallicfactor << " " << basecolorfactor0 << " " << basecolorfactor1 << " " << basecolorfactor2 << " " << basecolorfactor3 << " " << occul_strength << " " << emiss_factor << std::endl;
      }
    }
    if (firsttime) firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
    ee.us_vertex_shader = oldvert;
    ee.us_fragment_shader = oldfrag;
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
  float self_mult, rest_mult;
  bool tex0, tex1, tex2, tex3, tex4,tex5, tex6, tex7;
  float roughnessfactor, metallicfactor, basecolorfactor0,basecolorfactor1,basecolorfactor2,basecolorfactor3;
  float occul_strength;
  float emiss_factor;
  bool spec;
  Vector diff_factor, spec_factor;
  float glossi_factor;
  bool unlit;
  Point emis2;
  Vector light_dir2;
  std::string cache_id;
};


class TextureCubemapShaderML : public MainLoopItem
{
public:
  TextureCubemapShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix, float mix2) : ev(ev), next(next),mix(mix), mix2(mix2) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_cubemaptexture(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_cubemaptexture(fragment);
    ee.us_fragment_shader = a2f.id;
    }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
	ev.shader_api.set_var(sh, "color_mix2", mix2);
	//	ev.shader_api.set_var(sh, "cubesampler", 0);
      }
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
  float mix2;
};



class TextureArrShaderML : public MainLoopItem
{
public:
  TextureArrShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float mix) : ev(ev), next(next),mix(mix) 
  {
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime) {
      firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_texture_arr(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_texture_arr(fragment);
    ee.us_fragment_shader = a2f.id;
    }
    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);
	ev.shader_api.set_var(sh, "color_mix", mix);
      }
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float mix;
};


#if 0
class LightShaderML : public MainLoopItem
{
public:
  LightShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="light";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="light";
#endif
    if (firsttime) {
      firsttime = false; 
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_light(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_light(fragment);
    ee.us_fragment_shader = a2f.id;
    }
    next->execute(ee);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};
#endif

class ToonShaderML : public MainLoopItem
{
public:
  ToonShaderML(GameApi::EveryApi &ev, MainLoopItem *next) : ev(ev), next(next) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
#if 0
    if (ee.vertex_shader=="") { ee.vertex_shader+="colour"; }
    if (ee.vertex_shader!="") { ee.vertex_shader+=":"; }
    ee.vertex_shader+="toon";
    if (ee.fragment_shader=="") { ee.fragment_shader+="colour"; }
    if (ee.fragment_shader!="") { ee.fragment_shader+=":"; }
    ee.fragment_shader+="toon";
#endif
    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_toon(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_toon(fragment);
    ee.us_fragment_shader = a2f.id;
      }
    next->execute(ee);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
};

class FadeShaderML : public MainLoopItem
{
public:
  FadeShaderML(GameApi::EveryApi &ev, MainLoopItem *next, float start_time, float end_time, float start_time2, float end_time2) : ev(ev), next(next), start_time(start_time), end_time(end_time), start_time2(start_time2), end_time2(end_time2) 
  {
    //sh = ev.shader_api.get_normal_shader("comb", "comb", "", "colour:snoise:light", "colour:snoise:light");
    //ev.mainloop_api.init_3d(sh);
    //ev.mainloop_api.alpha(true); 
    firsttime = true;
    sh.id=0;
  }
 
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2 = ev.uber_api.v_fade(vertex);
    ee.us_vertex_shader = a2.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_fade(fragment);
    ee.us_fragment_shader = a2f.id;



      }
    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      sh.id = sh_ids[i];

    if (sh.id!=-1) {
	ev.shader_api.use(sh);
      ev.shader_api.set_var(sh, "time_begin_start", start_time);
      ev.shader_api.set_var(sh, "time_begin_end", end_time);
      ev.shader_api.set_var(sh, "time_end_start", start_time2);
      ev.shader_api.set_var(sh, "time_end_end", end_time2);
      ev.shader_api.set_var(sh, "time", e.time);
    }
    }
		    
    next->execute(ee);
  }
  std::vector<int> shader_id() { return next->shader_id(); }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float start_time, end_time;
  float start_time2, end_time2;
};


class SkeletalShader : public MainLoopItem
{
public:
  SkeletalShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, std::vector<SkeletalNode*> vec) : env(env), ev(ev), next(next), vec(vec) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_skeletal(vertex);
    //ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
      }

      float time = ee.time;
      std::vector<GameApi::M> vec1;
      std::vector<GameApi::PT> vec2;
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	//std::cout << time << std::endl;
	Matrix mm = vec[i]->mat(time);
	//std::cout << mm << std::endl;
	GameApi::M m = add_matrix2(env, mm);
	Point ppt = vec[i]->pos(time);
	//std::cout << "Point: " << ppt << std::endl;
	GameApi::PT pt = add_point(env, ppt.x, ppt.y, ppt.z); // TODO MEM LEAK ON POINTS
	vec1.push_back(m);
	vec2.push_back(pt); 
      }

    
    std::vector<int> sh_ids = next->shader_id();
    int s4 = sh_ids.size();
    for(int k=0;k<s4;k++) {
      int sh_id = sh_ids[k];
      //std::cout << "sh_id" << sh_id << std::endl;
      if (sh_id!=-1)
	{
	  //GameApi::SH sh;
	  sh.id = sh_id;
	  ev.shader_api.use(sh);
	  
	  
	}
#ifndef NO_MV
    
      GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
      GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
      GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
      GameApi::M m3 = add_matrix2(env, e.in_P);
      ev.shader_api.use(sh);
      ev.shader_api.set_var(sh, "in_MV", m);
      //ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));
      
      ev.shader_api.set_var(sh, "in_T", m1);
      ev.shader_api.set_var(sh, "in_P", m3);
      ev.shader_api.set_var(sh, "in_N", m2);
#endif
      ev.shader_api.set_var(sh, "time", time);

    ev.shader_api.set_var(sh, "bones", vec1,50);
    ev.shader_api.set_var(sh, "bone_pos", vec2);
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  std::vector<SkeletalNode*> vec;
};

class DistFieldMeshShader : public MainLoopItem
{
public:
  DistFieldMeshShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_dist_field_mesh(vertex, sfo);
    ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
      }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];

    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }

    
#ifndef NO_MV
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m3 = add_matrix2(env, e.in_P);
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    //ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_P", m3);
    ev.shader_api.set_var(sh, "in_N", m2);
#endif
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    }
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};


class MeshColorShader : public MainLoopItem
{
public:
  MeshColorShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() {next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_pass_position(vertex);
    ee.sfo_id = sfo.id;
    ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_mesh_color(fragment, sfo);
    ee.us_fragment_shader = a2f.id;
      }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
#ifndef NO_MV
  GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh,
    GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    //ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_P", m3);
    ev.shader_api.set_var(sh, "in_N", m2);
#endif
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    }
    
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};

#if 0
class SFOSandboxShader : public MainLoopItem
{
public:
  SFOSandboxShader(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, GameApi::SFO sfo) : env(env), ev(ev), next(next), sfo(sfo) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    //GameApi::US a2v = ev.uber_api.v_pass_position(vertex);
    ee.sfo_id = sfo.id;
    //ee.us_vertex_shader = a2v.id;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_sandbox(fragment, sfo);
    ee.us_fragment_shader = a2f.id;
      }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
#ifndef NO_MV
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    //ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_P", m3);
    ev.shader_api.set_var(sh, "in_N", m2);
#endif
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    }
    next->execute(ee);

  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  GameApi::SFO sfo;
};
#endif

class ChooseColorShaderML : public MainLoopItem
{
public:
  ChooseColorShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, unsigned int color, float mix_val) : env(env), ev(ev), next(next), color(color), mix_val(mix_val) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;

    if (firsttime)
      {
	firsttime = false;
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;


    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    GameApi::US a2f = ev.uber_api.f_choose_color(fragment);
    ee.us_fragment_shader = a2f.id;
      }

    std::vector<int> sh_ids = next->shader_id();
    int s = sh_ids.size();
    for(int i=0;i<s;i++) {
    //std::cout << "sh_id" << sh_id << std::endl;
      int sh_id = sh_ids[i];
      if (sh_id!=-1)
      {
	//GameApi::SH sh;
	sh.id = sh_id;
	ev.shader_api.use(sh);


      }
#ifndef NO_MV
    GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
    GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
    GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
    ev.shader_api.set_var(sh, "in_MV", m);
    //ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

    ev.shader_api.set_var(sh, "in_T", m1);
    ev.shader_api.set_var(sh, "in_P", m3);
    ev.shader_api.set_var(sh, "in_N", m2);
#endif
    float time = ee.time;
    ev.shader_api.set_var(sh, "time", time);
    Color cc(color);
    ev.shader_api.set_var(sh, "color_choice", cc.rf(), cc.gf(), cc.bf(), cc.af());
    ev.shader_api.set_var(sh, "mix_val", mix_val);

    }
    
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  unsigned int color;
  float mix_val;
};

#if 0
class SpotlightShaderML : public MainLoopItem
{
public:
  SpotlightShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, int light_color_id, Movement *change) : env(env), ev(ev), next(next), light_color_id(light_color_id), change(change) 
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
	GameApi::US vertex;
	vertex.id = ee.us_vertex_shader;
	if (vertex.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.v_empty();
	    GameApi::US a1 = ev.uber_api.v_colour(a0);
	    ee.us_vertex_shader = a1.id;
	  }
	vertex.id = ee.us_vertex_shader;
	GameApi::US a2v = ev.uber_api.v_point_light(vertex);
	ee.us_vertex_shader = a2v.id;

	GameApi::US fragment;
	fragment.id = ee.us_fragment_shader;
	if (fragment.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.f_empty(false);
	    GameApi::US a1 = ev.uber_api.f_colour(a0);
	    ee.us_fragment_shader = a1.id;
	  }
	fragment.id = ee.us_fragment_shader;
	//GameApi::US a2f = ev.uber_api.f_color_from_id(fragment, light_color_id);
	GameApi::US a3f = ev.uber_api.f_point_light(fragment);
	ee.us_fragment_shader = a3f.id;
      }
    std::vector<int> sh_ids = next->shader_id();
    int s=sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      if (sh_id != -1)
	{
	  sh.id = sh_id;
	}
      {
	ev.shader_api.use(sh);
	Point p(0.0,0.0,0.0);
	Matrix m = change->get_whole_matrix(e.time*10.0, e.delta_time);
	Point p2 = p * m;
	std::stringstream ss;
	ss << e.spotlight_id;
	ev.shader_api.set_var(sh, ("lightpos" + ss.str()).c_str(), p2.x,p2.y,p2.z);
    }
#ifndef NO_MV
    GameApi::M m = add_matrix2( env, e.in_MV );
    GameApi::M m1 = add_matrix2( env, e.in_T );
    GameApi::M m3 = add_matrix2( env, e.in_P );
    GameApi::M m2 = add_matrix2( env, e.in_N );
    ev.shader_api.set_var( sh, "in_MV", m);
    ev.shader_api.set_var( sh, "in_T", m1);
    ev.shader_api.set_var( sh, "in_P", m3);
    ev.shader_api.set_var( sh, "in_N", m2);
    ev.shader_api.set_var( sh, "time", e.time);
#endif
    }
    ee.spotlight_id = e.spotlight_id+1;
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int light_color_id;
  Movement *change;
  bool firsttime;
  GameApi::SH sh;
};
#endif

#if 0
class ColouredLightsShaderML : public MainLoopItem
{
public:
  ColouredLightsShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float scale, unsigned int color_1, unsigned int color_2, unsigned int color_3, unsigned int color_4, unsigned int color_5, unsigned int color_6, unsigned int color_7, unsigned int color_8, Point pos_1, Point pos_2, Point pos_3, Point pos_4, Point pos_5, Point pos_6, Point pos_7, Point pos_8, float dist_1, float dist_2, float dist_3, float dist_4, float dist_5, float dist_6, float dist_7, float dist_8)
  : env(env), ev(ev), next(next), scale(scale),
      colors({color_1,color_2,color_3,color_4, color_5, color_6, color_7, color_8}),
      positions({pos_1,pos_2, pos_3,pos_4,pos_5,pos_6,pos_7,pos_8}),
      distances({dist_1,dist_2,dist_3,dist_4,dist_5,dist_6,dist_7,dist_8})
  {
    firsttime = true;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    if (firsttime)
      {
	firsttime = false;
	GameApi::US vertex;
	vertex.id = ee.us_vertex_shader;
	if (vertex.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.v_empty();
	    GameApi::US a1 = ev.uber_api.v_colour(a0);
	    ee.us_vertex_shader = a1.id;
	  }
	vertex.id = ee.us_vertex_shader;
	GameApi::US a2v = ev.uber_api.v_coloured_lights(vertex);
	ee.us_vertex_shader = a2v.id;

	GameApi::US fragment;
	fragment.id = ee.us_fragment_shader;
	if (fragment.id == -1)
	  {
	    GameApi::US a0 = ev.uber_api.f_empty(false);
	    GameApi::US a1 = ev.uber_api.f_colour(a0);
	    ee.us_fragment_shader = a1.id;
	  }
	fragment.id = ee.us_fragment_shader;
	//GameApi::US a2f = ev.uber_api.f_color_from_id(fragment, light_color_id);
	GameApi::US a3f = ev.uber_api.f_coloured_lights(fragment);
	ee.us_fragment_shader = a3f.id;
      }
    std::vector<int> sh_ids = next->shader_id();
    int s=sh_ids.size();
    for(int i=0;i<s;i++) {
      int sh_id = sh_ids[i];
      sh.id = sh_id;
      if (sh_id != -1)
      {
	ev.shader_api.use(sh);
	for(int j=0;j<8;j++) {
	  std::stringstream ss1,ss2,ss3;
	  ss1 << "c_color[" << j << "]";
	  ss2 << "c_scale[" << j << "]";
	  ss3 << "c_positions[" << j << "]";
	  //std::cout << ss1.str() << "::" << ss2.str() << "::" << ss3.str() << std::endl;
	  Color c(colors[j]);

	  //char *ptr1 = new char[ss1.str().size()+1];
	  //std::copy(ss1.str().begin(),ss1.str().end(),ptr1);
	  
	  //std::cout << c.rf() << " " << c.gf() << " " << c.bf() << std::endl;
	  ev.shader_api.set_var(sh, ss1.str().c_str(), c.rf(),c.gf(),c.bf(),1.0);
	  ev.shader_api.set_var(sh, ss2.str().c_str(), float(scale/distances[j]));
	  ev.shader_api.set_var(sh, ss3.str().c_str(), positions[j].x, positions[j].y, positions[j].z,0.0);
	}
    }
#ifndef NO_MV
    GameApi::M m = add_matrix2( env, e.in_MV );
    GameApi::M m1 = add_matrix2( env, e.in_T );
    GameApi::M m3 = add_matrix2( env, e.in_P );
    GameApi::M m2 = add_matrix2( env, e.in_N );
    ev.shader_api.set_var( sh, "in_MV", m);
    ev.shader_api.set_var( sh, "in_T", m1);
    ev.shader_api.set_var( sh, "in_P", m3);
    ev.shader_api.set_var( sh, "in_N", m2);
    ev.shader_api.set_var( sh, "time", e.time);
#endif
    }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  float scale;
  std::vector<unsigned int> colors;
  std::vector<Point> positions;
  std::vector<float> distances;
  bool firsttime;
  GameApi::SH sh;
};
#endif

class ShaderParamML : public MainLoopItem
{
public:
  ShaderParamML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir,
		unsigned int level1, unsigned int level2, unsigned int level3, float spec_size, bool ambient, bool diffuse, bool specular) : env(env), ev(ev), next(next), light_dir(light_dir), level1(level1), level2(level2), level3(level3), spec_size(spec_size), ambient(ambient), diffuse(diffuse), specular(specular) 
  { 
    firsttime = true;
    initialized=false;
    sh.id = -1;
  }
  std::vector<int> shader_id() { if (sh.id != -1) return std::vector<int>{sh.id}; return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { initialized=true; } // not called
  void Prepare() { next->Prepare(); initialized=true; }
  void execute(MainLoopEnv &e)
  {
    if (!initialized) return;
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    GameApi::US a2v = ev.uber_api.v_ambient(vertex);
    GameApi::US a3v = ev.uber_api.v_diffuse(a2v);
    GameApi::US a4v = ev.uber_api.v_specular(a3v);

    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = a4v.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient)
       fragment = ev.uber_api.f_ambient(fragment);
    if (diffuse)
      fragment = ev.uber_api.f_diffuse(fragment);
    if (specular)
      fragment = ev.uber_api.f_specular(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
	ev.shader_api.set_var(sh, "specular_size", spec_size);
	ev.shader_api.set_var(sh, "level1_color",
			      ((level1&0xff0000)>>16)/255.0,
			      ((level1&0xff00)>>8)/255.0,
			      ((level1&0xff))/255.0,
			      ((level1&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level2_color",
			      ((level2&0xff0000)>>16)/255.0,
			  ((level2&0xff00)>>8)/255.0,
			      ((level2&0xff))/255.0,
			  ((level2&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level3_color",
			      ((level3&0xff0000)>>16)/255.0,
			      ((level3&0xff00)>>8)/255.0,
			      ((level3&0xff))/255.0,
			      ((level3&0xff000000)>>24)/255.0);

      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	//ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  unsigned int level1, level2, level3;
  GameApi::SH sh;
  bool firsttime;
  float spec_size;
  bool ambient, diffuse, specular;
  bool initialized;
};


class GIShaderML : public MainLoopItem
{
public:
  GIShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, PointsApiPoints *pts, float obj_size) : env(env), ev(ev), next(next), pts(pts), obj_size(obj_size) {
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); } 
  
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_gi(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_gi(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int k=0;k<s;k++) {
       int sh_id = sh_ids[k];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	int s = pts->NumPoints();
	ev.shader_api.set_var(sh, "num_pos", s);
	for(int i=0;i<s;i++) {
	  Point p = pts->Pos(i);
	  std::stringstream ss;
	  ss << "obj_pos[" << i << "]";
	  ev.shader_api.set_var(sh, ss.str().c_str(), p.x, p.y, p.z);	  
	}

	ev.shader_api.set_var(sh, "obj_size", obj_size);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  PointsApiPoints *pts;
  float obj_size;
  bool firsttime;
  GameApi::SH sh;
};

class ColourShaderML : public MainLoopItem
{
public:
  ColourShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float mix) : env(env), ev(ev), next(next), mix(mix) {
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_colour_with_mix(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_colour_with_mix(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "color_mix", mix);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  float mix;
  bool firsttime;
  GameApi::SH sh;
};


class WaterShaderML : public MainLoopItem
{
public:
  WaterShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, unsigned int color1, unsigned int color2, unsigned int color3, Point center, float wave_mult, float time_mult) : env(env), ev(ev), next(next), color1(color1), color2(color2), color3(color3), center(center), wave_mult(wave_mult), time_mult(time_mult) {
    firsttime = true;
    sh.id = -1;


  }

  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e) {
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
    vertex = ev.uber_api.v_water(vertex);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_water(fragment);
    ee.us_fragment_shader = fragment.id;
#endif	
      }
     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "water_color1",
			      ((color1&0xff0000)>>16)/255.0,
			      ((color1&0xff00)>>8)/255.0,
			      ((color1&0xff))/255.0,
			      ((color1&0xff000000)>>24)/255.0);				ev.shader_api.set_var(sh, "water_color2",
			      ((color2&0xff0000)>>16)/255.0,
			      ((color2&0xff00)>>8)/255.0,
			      ((color2&0xff))/255.0,
			      ((color2&0xff000000)>>24)/255.0);
		ev.shader_api.set_var(sh, "water_color3",
			      ((color3&0xff0000)>>16)/255.0,
			      ((color3&0xff00)>>8)/255.0,
			      ((color3&0xff))/255.0,
			      ((color3&0xff000000)>>24)/255.0);	
	ev.shader_api.set_var(sh, "water_center", center.x, center.y,center.z);
	ev.shader_api.set_var(sh, "wave_mult", wave_mult);
	ev.shader_api.set_var(sh, "wave_time", time_mult*e.time);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);

    
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  unsigned int color1, color2,color3;
  Point center;
  float wave_mult;
  float time_mult;
  GameApi::SH sh;
  bool firsttime;
};

class GlowEdgeShaderML : public MainLoopItem
{
public:
  GlowEdgeShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float white_level, float gray_level, float edge_pos) : env(env), ev(ev), next(next), white_level(white_level), gray_level(gray_level), edge_pos(edge_pos) {
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e) {
    MainLoopEnv ee = e;
    if (firsttime)
      {

#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_glowedge(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_glowedge(fragment);
    ee.us_fragment_shader = fragment.id;
#endif	
      }
     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "white_level",
			      white_level);
	ev.shader_api.set_var(sh, "gray_level",
			      gray_level);
	ev.shader_api.set_var(sh, "edge_pos", edge_pos);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);

    
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::SH sh;
  MainLoopItem *next;
  float white_level;
  float gray_level;
  float edge_pos;
  bool firsttime;
};

extern GameApi::M g_view_rot;

class NewShadowShaderML_1 : public MainLoopItem
{
public:
  NewShadowShaderML_1(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir, float scale) : env(env), ev(ev), next(next), light_dir(light_dir), scale(scale) { firsttime=true; }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    //buf = ev.texture_api.prepare_fb(fbm, texture_id);
    //txid = ev.texture_api.send_fb_to_gpu(buf, texture_id);
  } // not called
  void Prepare() {
    next->Prepare();
    HeavyPrepare();
  }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {
    Vector light_dir2 = light_dir;
  OpenglLowApi *ogl = g_low->ogl;

  //if (firsttime)
  //   {
  //	ogl->glGenFramebuffers(1, &fbo);
  //	
  //    }
    
    MainLoopEnv ee = e;
     if (firsttime)
      {
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1||vertex.id==0) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_newshadow_1(vertex);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    //std::cout << "FRAG:" << ee.us_fragment_shader << std::endl;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1||fragment.id==0) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      ee.us_fragment_shader = a0.id;
      //std::cout << "FRAG2:" << ee.us_fragment_shader << std::endl;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_newshadow_1(fragment);
    ee.us_fragment_shader = fragment.id;
    //std::cout << "FRAG3:" << ee.us_fragment_shader << std::endl;
    }
  
     if (sh_ids.size()==0) sh_ids = next->shader_id();
     if (firsttime ||(notupdated&&e.time-timestamp>0.1)|| e.time-timestamp>5.0) { timestamp = e.time; sh_ids = next->shader_id(); if (!firsttime) notupdated=false; }
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	//ev.shader_api.set_var(sh, "in_NewShadowLightPos", light_dir.dx, light_dir.dy, light_dir.dz);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

	Vector up = { 0.0, 1.0, 0.0 }; // TODO, is this -1 or 1
	if (fabs(light_dir2.dy)>0.95) up={1.0,0.0,0.0};
	Vector right = Vector::CrossProduct(light_dir2, up);
	Vector new_up = Vector::CrossProduct(right, light_dir2);
	new_up/=new_up.Dist();

	Point eye = Point(400.0*(light_dir2/light_dir2.Dist()));

	Vector dx = Vector::CrossProduct(light_dir2, up);
	dx/=dx.Dist();
	dx*=1024/2.0;
	dx*=scale;
	Vector dy = new_up;
	dy/=dy.Dist();
	dy*=1024/2.0;
	dy*=scale;

	if (sh.id!=-1) {
	ev.shader_api.set_var(sh, "in_dx", dx.dx,dx.dy,dx.dz);
	ev.shader_api.set_var(sh, "in_dy", dy.dx,dy.dy,dy.dz);
	ev.shader_api.set_var(sh, "in_pp", eye.x,eye.y,eye.z); // in world coords
	//std::cout << "SHADER1:" << std::endl;
	//std::cout << "in_dx=" << dx << std::endl;
	//std::cout << "in_dy=" << dy << std::endl;
	//std::cout << "in_pp=" << eye << std::endl;
	
	ev.shader_api.set_var(sh, "in_scale", scale);
	//ev.shader_api.set_var(sh, "in_txid", txid.id);
	}
	
	ee.in_P = Matrix::Ortho(-300.0,300.0, -300.0, 300.0, -8000.0, 8000.0);
	ee.in_T = Matrix::Translate(-eye.x,-eye.y,-eye.z) * Matrix::gluLookAt(eye, Point(0.0,0.0,0.0),  new_up);
	if (sh.id!=-1) {
	ev.shader_api.set_var(sh, "in_P", add_matrix2(env,ee.in_P));
	ev.shader_api.set_var(sh, "in_T", add_matrix2(env,ee.in_T));
	ev.shader_api.set_var(sh, "in_Mat", add_matrix2(env, Matrix::Translate(eye.x,eye.y,eye.z) * Matrix::gluLookAt(eye, Point(0.0,0.0,0.0),  new_up)));
	}
        //ee.in_MV = Matrix::Identity();
	//std::cout << "checking error999:" << ogl->glGetError() << std::endl;
	//std::cout << "TEXTURE ID:" << txid.id << std::endl;
  next->execute(ee);
	/*
        ogl->glDisable(Low_GL_MULTISAMPLE);
  ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
  ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, fbo);
  ogl->glFramebufferTexture2D( Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0, Low_GL_TEXTURE_2D, txid.id, 0 );
  //std::cout << "checking error1:" << ogl->glGetError() << std::endl;
	//ogl->glDrawBuffer(Low_GL_NONE);
  //ogl->glDisable(Low_GL_TEXTURE_2D);
    //std::cout << "checking error2:" << ogl->glGetError() << std::endl;
    ogl->glFramebufferTexture2D( Low_GL_FRAMEBUFFER, Low_GL_COLOR_ATTACHMENT0, Low_GL_TEXTURE_2D, 0, 0 );
    	ogl->glBindFramebuffer(Low_GL_FRAMEBUFFER, 0);
	//std::cout << "checking error3:" << ogl->glGetError() << std::endl;
    //ogl->glDrawBuffer(Low_GL_BACK);
        ogl->glEnable(Low_GL_MULTISAMPLE);
	*/

    ev.shader_api.unuse(sh);
  }
  //GameApi::TXID get_txid() const { return txid; }
  //int size_x() const { return buf.width; }
  //int size_y() const { return buf.height; }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  bool firsttime;
  std::vector<int> sh_ids;
  bool notupdated=true;
  GameApi::SH sh;
  float timestamp=0.0;
  Low_GLuint texture_id;
  BufferRefF buf;
  //GameApi::TXID txid;
  //GameApi::FB fbm;
  float scale;
  Low_GLuint fbo;
};


class NewShadowShaderML_2 : public MainLoopItem
{
public:
  NewShadowShaderML_2(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir, float dark_level, float light_level, float scale, bool is_phong, int texindex) : env(env), ev(ev), next(next), light_dir(light_dir), dark_level(dark_level), light_level(light_level),scale(scale),is_phong(is_phong),texindex(texindex) { firsttime=true; }
  std::vector<int> shader_id() { return next->shader_id(); }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    //txid = prev->get_txid();
  } // not called
  void Prepare() { next->Prepare(); HeavyPrepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {

    Vector light_dir2 = light_dir;
    
      OpenglLowApi *ogl = g_low->ogl;

    MainLoopEnv ee = e;
    //if (firsttime) {
    //  ev.polygon_api.create_vertex_array_hw(va);
    //}
     if (firsttime)
      {
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    //vertex = ev.uber_api.v_texture(vertex);
    vertex = ev.uber_api.v_newshadow_2(vertex);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    //fragment = ev.uber_api.f_texture(fragment);
    fragment = ev.uber_api.f_newshadow_2(fragment,is_phong);
    ee.us_fragment_shader = fragment.id;
    }
      
     if (sh_ids.size()==0) sh_ids = next->shader_id();
     if (firsttime ||(notupdated&&e.time-timestamp>0.1)|| e.time-timestamp>5.0) { timestamp = e.time; sh_ids = next->shader_id(); if (!firsttime) notupdated=false; }
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	//ev.shader_api.set_var(sh, "in_NewShadowLightPos", light_dir.dx, light_dir.dy, light_dir.dz);
	//std::cout << "Levels: " << dark_level << " " << light_level << std::endl;
	  ev.shader_api.set_var(sh, "in_darklevel", dark_level);
	  ev.shader_api.set_var(sh, "in_lightlevel", light_level);
	  ev.shader_api.set_var(sh, "texindex", texindex);


	Vector up = { 0.0, 1.0, 0.0 }; // TODO, is this -1 or 1
	if (fabs(light_dir2.dy)>0.95) up={1.0,0.0,0.0};
	Vector right = Vector::CrossProduct(light_dir2, up);
	Vector new_up = Vector::CrossProduct(right, light_dir2);
	new_up/=new_up.Dist();

	Point eye = Point(400.0*(light_dir2/light_dir2.Dist()));

	Vector dx = Vector::CrossProduct(light_dir2, up);
	dx/=dx.Dist();
	dx*=1024/2.0;
	dx*=scale;
	Vector dy = new_up;
	dy/=dy.Dist();
	dy*=1024/2.0;
	dy*=scale;

	
	ev.shader_api.set_var(sh, "in_dx", dx.dx,dx.dy,dx.dz);
	ev.shader_api.set_var(sh, "in_dy", dy.dx,dy.dy,dy.dz);
	ev.shader_api.set_var(sh, "in_pp", eye.x,eye.y,eye.z); // in world coords
	//std::cout << "SHADER2:" << std::endl;
	//std::cout << "in_dx=" << dx << std::endl;
	//std::cout << "in_dy=" << dy << std::endl;
	//std::cout << "in_pp=" << eye << std::endl;
	


	//ev.shader_api.set_var(sh, "in_txid", 0);
	ev.shader_api.set_var(sh, "in_scale", scale);
	ev.shader_api.set_var(sh, "in_Mat", add_matrix2(env, Matrix::Translate(eye.x,eye.y,eye.z) * Matrix::gluLookAt(eye, Point(0.0,0.0,0.0),  new_up)));
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

#ifndef EMSCRIPTEN
	//  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
	//ogl->glActiveTexture(Low_GL_TEXTURE0+0);
	
	//ogl->glBindTexture(Low_GL_TEXTURE_2D, txid.id);
	//ee.in_T = ee.in_T;// * Matrix::Translate(-1.0,1.0,-1.0);
	next->execute(ee);
    //ogl->glBindTexture(Low_GL_TEXTURE_2D,0);

  //ogl->glDisable(Low_GL_TEXTURE_2D);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  float dark_level;
  float light_level;
  bool firsttime;
  std::vector<int> sh_ids;
  bool notupdated=true;
  GameApi::SH sh;
  float timestamp=0.0;
  //GameApi::TXID txid;
  //NewShadowShaderML_1* prev;
  float scale;
  bool is_phong;
  int texindex;
};

class AdjustShaderML : public MainLoopItem
{
public:
  AdjustShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, unsigned int ad_color, float ad_dark, float ad_light) : env(env), ev(ev), next(next), ad_color(ad_color), ad_dark(ad_dark), ad_light(ad_light) {
    firsttime = true;
    sh.id = -1;

  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_adjust(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_adjust(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }
     if (sh_ids.size()==0) sh_ids = next->shader_id();
     if (firsttime ||(notupdated&&e.time-timestamp>0.1)|| e.time-timestamp>5.0) { timestamp = e.time; sh_ids = next->shader_id(); if (!firsttime) notupdated=false; }
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "ad_color",
			      ((ad_color&0xff0000)>>16)/255.0,
			      ((ad_color&0xff00)>>8)/255.0,
			      ((ad_color&0xff))/255.0,
			      ((ad_color&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "ad_dark", ad_dark);
	ev.shader_api.set_var(sh, "ad_light", ad_light);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  unsigned int ad_color;
  float ad_dark;
  float ad_light;
  bool firsttime;
  GameApi::SH sh;
  bool notupdated=true;
  std::vector<int> sh_ids;
  float timestamp=0.0;
};

class BlurredRenderML : public MainLoopItem
{
public:
  BlurredRenderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, int numsamples, float blur_radius) : env(env), ev(ev), next(next), numsamples(numsamples), blur_radius(blur_radius) {
    firsttime=true;
    sh.id=-1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_blurred_render(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_blurred_render(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }
     if (sh_ids.size()==0) sh_ids = next->shader_id();
     if (firsttime ||(notupdated&&e.time-timestamp>0.1)|| e.time-timestamp>5.0) { timestamp = e.time; sh_ids = next->shader_id(); if (!firsttime) notupdated=false; }
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "blur_num_samples", numsamples);
	ev.shader_api.set_var(sh, "blur_radius", blur_radius);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  int numsamples;
  float blur_radius;
  std::vector<int> sh_ids;
  float timestamp=0.0;
  bool notupdated=true;
};


class PhongShaderML : public MainLoopItem
{
public:
  PhongShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir, unsigned int ambient, unsigned int specular, unsigned int highlight, float pow, bool background_included) : env(env), ev(ev), next(next), light_dir(light_dir), ambient(ambient), specular(specular), highlight(highlight), pow(pow), background_included(background_included) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv &ee = e;
    int old_vert = ee.us_vertex_shader;
    int old_frag = ee.us_fragment_shader;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_phong(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient) {
      if (background_included) {
	fragment = ev.uber_api.f_phong(fragment);
      } else {
	fragment = ev.uber_api.f_phong2(fragment);
      }
    }
    ee.us_fragment_shader = fragment.id;
#endif
      }
     if (sh_ids.size()==0) sh_ids = next->shader_id();
     if (firsttime ||(notupdated&&e.time-timestamp>0.1)|| e.time-timestamp>5.0) { timestamp = e.time; sh_ids = next->shader_id(); if (!firsttime) notupdated=false; }
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "in_PhongLightPos", light_dir.dx, light_dir.dy, light_dir.dz);
	ev.shader_api.set_var(sh, "level1_color",
			      ((ambient&0xff0000)>>16)/255.0,
			      ((ambient&0xff00)>>8)/255.0,
			      ((ambient&0xff))/255.0,
			      ((ambient&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level2_color",
			      ((specular&0xff0000)>>16)/255.0,
			      ((specular&0xff00)>>8)/255.0,
			      ((specular&0xff))/255.0,
			      ((specular&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level3_color",
			      ((highlight&0xff0000)>>16)/255.0,
			  ((highlight&0xff00)>>8)/255.0,
			      ((highlight&0xff))/255.0,
			  ((highlight&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "hilight", pow);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ee.us_vertex_shader = old_vert;
    ee.us_fragment_shader = old_frag;
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  GameApi::SH sh;
  bool firsttime;
  unsigned int ambient;
  unsigned int specular;
  unsigned int highlight;
  float pow;
  bool background_included;
  std::vector<int> sh_ids;
  float timestamp=0.0;
  bool notupdated=true;
};


class VertexPhongShaderML : public MainLoopItem
{
public:
  VertexPhongShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir, unsigned int ambient, unsigned int highlight, float pow, float mix) : env(env), ev(ev), next(next), light_dir(light_dir), ambient(ambient), highlight(highlight), pow(pow), mix(mix) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void logoexecute() { next->logoexecute(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_vertexphong(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient)
      fragment = ev.uber_api.f_vertexphong(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }
     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

#if 0
	// to fix bug in phong and bump phong in linux nvidia drivers.
	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, 18000.0 /*light_dir.dz*/);
#else
	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
#endif
	ev.shader_api.set_var(sh, "level1_color",
			      ((ambient&0xff0000)>>16)/255.0,
			      ((ambient&0xff00)>>8)/255.0,
			      ((ambient&0xff))/255.0,
			      ((ambient&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level2_color",
			      ((highlight&0xff0000)>>16)/255.0,
			  ((highlight&0xff00)>>8)/255.0,
			      ((highlight&0xff))/255.0,
			  ((highlight&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "hilight", pow);
	ev.shader_api.set_var(sh, "color_mix", mix);

      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  GameApi::SH sh;
  bool firsttime;
  unsigned int ambient;
  unsigned int highlight;
  float pow;
  float mix;
};


#if 0
class EdgeShaderML : public MainLoopItem
{
public:
  EdgeShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float edge_width, unsigned int edge_color) : env(env), ev(ev), next(next), edge_width(edge_width), edge_color(edge_color)
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_edge(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_edge(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "edge_width", edge_width);
	ev.shader_api.set_var(sh, "edge_color",
			      ((edge_color&0xff0000)>>16)/255.0,
			      ((edge_color&0xff00)>>8)/255.0,
			      ((edge_color&0xff))/255.0,
			      ((edge_color&0xff000000)>>24)/255.0);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float edge_width;
  unsigned int edge_color;
};
#endif

class GlobeShaderML : public MainLoopItem
{
public:
  GlobeShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float globe_r) : env(env), ev(ev), next(next), globe_r(globe_r)
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_globe(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    //if (ambient)
    //  fragment = ev.uber_api.f_phong(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s=sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
       sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "globe_mult", globe_r);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	if (firsttime) 	firsttime = false;

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  GameApi::SH sh;
  bool firsttime;
  float globe_r;
};


/*
class AOShaderML : public MainLoopItem
{
public:
  AOShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float radius, int kernelsize, int noisesize) : env(env), ev(ev), next(next), radius(radius), kernelsize(kernelsize), noisesize(noisesize)
  { 
    firsttime = true;
    sh.id = -1;
  }
  int shader_id() { if (sh.id != -1) return sh.id; return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      //GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a0.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_ao(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a0.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient)
      fragment = ev.uber_api.f_ao(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

    int sh_id = next->shader_id();
    sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);


	ev.shader_api.set_var(sh, "radius", radius);
	ev.shader_api.set_var(sh, "kernelsize", kernelsize);
	ev.shader_api.set_var(sh, "noisesize", noisesize);

	int s2 = noisesize;
	for(int i=0;i<s2;i++)
	  {
	  float x = double(r.next())/r.maximum();
	  x*=2.0;
	  x-=1.0;
	  float y = double(r.next())/r.maximum();
	  y*=2.0;
	  y-=1.0;

	  Vector v = { x,y,0.0f };
	  v/=v.Dist();
	  ev.shader_api.set_var(sh, (std::string("noise[") + NumI(i) + "]").c_str(), v.dx,v.dy,v.dz);
	  }
	  

	int s = kernelsize;
	for(int i=0;i<s;i++) {
	  Point p;
	  Random r;
	  float x = double(r.next())/r.maximum();
	  x*=2.0;
	  x-=1.0;
	  float y = double(r.next())/r.maximum();
	  y*=2.0;
	  y-=1.0;
	  float z = double(r.next())/r.maximum();
	  p.x = x;
	  p.y = y;
	  p.z = z;
	  Vector v = p;
	  v/=v.Dist();
	  float r = double(r.next())/r.maximum();
	  v*=r;

	  float scale = float(i)/float(kernelsize);
	  float k = scale*scale;
	  float ip = (1.0-k)*0.1f + k*1.0f;
	  v*=ip;
	  
	  ev.shader_api.set_var(sh, (std::string("kernel[") + NumI(i) + "]").c_str(), v.dx,v.dy,v.dz);
	}
      }

	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);

    next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  float radius;
  int kernelsize;
  int noisesize;
};
*/

class BumpPhongShaderML : public MainLoopItem
{
public:
  BumpPhongShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Vector light_dir, unsigned int ambient, unsigned int highlight, float pow) : env(env), ev(ev), next(next), light_dir(light_dir), ambient(ambient), highlight(highlight), pow(pow) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_bump_phong(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    if (ambient)
      fragment = ev.uber_api.f_bump_phong(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }
  std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

#if 0
	// to fix bug in phong and bumpphong in nvidia gfx drivers in linux
	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, 18000.0 /*light_dir.dz*/);
#else
	ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
#endif
	ev.shader_api.set_var(sh, "level1_color",
			      ((ambient&0xff0000)>>16)/255.0,
			      ((ambient&0xff00)>>8)/255.0,
			      ((ambient&0xff))/255.0,
			      ((ambient&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "level2_color",
			      ((highlight&0xff0000)>>16)/255.0,
			  ((highlight&0xff00)>>8)/255.0,
			      ((highlight&0xff))/255.0,
			  ((highlight&0xff000000)>>24)/255.0);
	ev.shader_api.set_var(sh, "hilight", pow);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  GameApi::SH sh;
  bool firsttime;
  unsigned int ambient;
  unsigned int highlight;
  float pow;
};


class FogShaderML : public MainLoopItem
{
public:
  FogShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, float fog_dist, unsigned int dark_color, unsigned int light_color) : env(env), ev(ev), next(next), fog_dist(fog_dist), dark_color(dark_color), light_color(light_color) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_fog(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_fog(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++)
       {
	 int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "fog_dist", fog_dist);
	Color c(dark_color);
	Color c2(light_color);
	ev.shader_api.set_var(sh, "fog_dark", c.r/255.0,c.g/255.0,c.b/255.0,c.alpha/255.0);
	ev.shader_api.set_var(sh, "fog_light", c2.r/255.0,c2.g/255.0,c2.b/255.0,c2.alpha/255.0);
	//ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
	//ev.shader_api.set_var(sh, "level1_color",
	//			      ((ambient&0xff0000)>>16)/255.0,
	//			      ((ambient&0xff00)>>8)/255.0,
	//			      ((ambient&0xff))/255.0,
	//			      ((ambient&0xff000000)>>24)/255.0);
	//ev.shader_api.set_var(sh, "level2_color",
	//			      ((highlight&0xff0000)>>16)/255.0,
	//			  ((highlight&0xff00)>>8)/255.0,
	//			      ((highlight&0xff))/255.0,
	//			  ((highlight&0xff000000)>>24)/255.0);
	//ev.shader_api.set_var(sh, "hilight", pow);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
       }
	next->execute(ee);
    ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  Vector light_dir;
  GameApi::SH sh;
  bool firsttime;
  float fog_dist;
  unsigned int dark_color;
  unsigned int light_color;
};

#if 0
class ShadowShaderML : public MainLoopItem
{
public:
  ShadowShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, int tex_id, Point pos, unsigned int dark_color, float mix) : env(env), ev(ev), next(next), tex_id(tex_id), pos(pos), dark_color(dark_color), mix(mix) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (sh.id==-1)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      // std::cout << "Shadow::V_Empty" << std::endl;

      GameApi::US a0 = ev.uber_api.v_empty();
      //std::cout << "Shadow::V_Colour" << std::endl;
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    //std::cout << "Shadow::V_Shadow" << std::endl;
    vertex = ev.uber_api.v_shadow(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      //std::cout << "Shadow::F_Empty" << std::endl;
      GameApi::US a0 = ev.uber_api.f_empty(false);
      //std::cout << "Shadow::F_Colour" << std::endl;
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    //std::cout << "Shadow::F_Shadow" << std::endl;
    fragment = ev.uber_api.f_shadow(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
     sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);

	ev.shader_api.set_var(sh, "color_mix2", mix);
	ev.shader_api.set_var(sh, "shadow_tex", float(tex_id));
	ev.shader_api.set_var(sh, "shadow_center", pos.x, pos.y, pos.z); 
	Color c(dark_color);
	ev.shader_api.set_var(sh, "shadow_dark", c.r/255.0,c.g/255.0,c.b/255.0,c.alpha/255.0);
	//ev.shader_api.set_var(sh, "light_dir", light_dir.dx, light_dir.dy, light_dir.dz);
	//ev.shader_api.set_var(sh, "level1_color",
	//			      ((ambient&0xff0000)>>16)/255.0,
	//			      ((ambient&0xff00)>>8)/255.0,
	//			      ((ambient&0xff))/255.0,
	//			      ((ambient&0xff000000)>>24)/255.0);
	//ev.shader_api.set_var(sh, "level2_color",
	//			      ((highlight&0xff0000)>>16)/255.0,
	//			  ((highlight&0xff00)>>8)/255.0,
	//			      ((highlight&0xff))/255.0,
	//			  ((highlight&0xff000000)>>24)/255.0);
	//ev.shader_api.set_var(sh, "hilight", pow);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	if (sh.id!=-1) {
	  ev.shader_api.set_var(sh, "in_MV", m);
	  ev.shader_api.set_var(sh, "in_T", m1);
	  ev.shader_api.set_var(sh, "in_P", m3);
	  ev.shader_api.set_var(sh, "in_N", m2);
	  ev.shader_api.set_var(sh, "time", e.time);
	  ev.shader_api.set_var(sh, "in_POS", e.in_POS);
	  ev.shader_api.use(sh);
	}
#endif
     }
	next->execute(ee);
    if (sh.id!=-1) {
      ev.shader_api.unuse(sh);
    }
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  int tex_id;
  Point pos;
  unsigned int dark_color;
  float mix;
};

#endif

extern std::vector<float> dyn_points_global_x;
extern std::vector<float> dyn_points_global_y;
extern std::vector<float> dyn_points_global_z;

class DynLightsShaderML : public MainLoopItem
{
public:
  DynLightsShaderML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *next, Point light_pos, float dist, int dyn_point) : env(env), ev(ev), next(next), light_pos(light_pos), dist(dist), dyn_point(dyn_point) 
  { 
    firsttime = true;
    sh.id = -1;
  }
  std::vector<int> shader_id() { return next->shader_id(); 
  }
  void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { next->Prepare(); }
  void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
     if (firsttime)
      {
	firsttime = false;
#if 1
    GameApi::US vertex;
    vertex.id = ee.us_vertex_shader;
    if (vertex.id==-1) { 
      GameApi::US a0 = ev.uber_api.v_empty();
      GameApi::US a1 = ev.uber_api.v_colour(a0);
      ee.us_vertex_shader = a1.id;
    }
    vertex.id = ee.us_vertex_shader;
    vertex = ev.uber_api.v_dyn_lights(vertex);
    //GameApi::US a2 = ev.uber_api.v_passall(a4v);
    ee.us_vertex_shader = vertex.id;

    GameApi::US fragment;
    fragment.id = ee.us_fragment_shader;
    if (fragment.id==-1) { 
      GameApi::US a0 = ev.uber_api.f_empty(false);
      GameApi::US a1 = ev.uber_api.f_colour(a0);
      ee.us_fragment_shader = a1.id;
    }
    fragment.id = ee.us_fragment_shader;
    fragment = ev.uber_api.f_dyn_lights(fragment);
    ee.us_fragment_shader = fragment.id;
#endif
      }

     std::vector<int> sh_ids = next->shader_id();
     int s = sh_ids.size();
     for(int i=0;i<s;i++) {
       int sh_id = sh_ids[i];
       sh.id = sh_id;
    //std::cout << "sh_id" << sh_id << std::endl;
    if (sh_id!=-1)
      {
	//GameApi::SH sh;
	ev.shader_api.use(sh);
#if 1
	int s = dyn_points_global_x.size();
	if (dyn_point>=0 && dyn_point<s) {
	  light_pos = Point(dyn_points_global_x[dyn_point],dyn_points_global_y[dyn_point],dyn_points_global_z[dyn_point]);
	}
#endif
	ev.shader_api.set_var(sh, "dyn_light_pos", light_pos.x, light_pos.y, light_pos.z);
	ev.shader_api.set_var(sh, "dyn_light_dist", dist);
      }

#ifndef NO_MV
	GameApi::M m = add_matrix2( env, e.in_MV); //ev.shader_api.get_matrix_var(sh, "in_MV");
	GameApi::M m1 = add_matrix2(env, e.in_T); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m3 = add_matrix2(env, e.in_P); //ev.shader_api.get_matrix_var(sh, "in_T");
	GameApi::M m2 = add_matrix2(env, e.in_N); //ev.shader_api.get_matrix_var(sh, "in_N");
	ev.shader_api.set_var(sh, "in_MV", m);
	//ev.shader_api.set_var(sh, "in_iMV", ev.matrix_api.transpose(ev.matrix_api.inverse(m)));

	ev.shader_api.set_var(sh, "in_T", m1);
	ev.shader_api.set_var(sh, "in_P", m3);
	ev.shader_api.set_var(sh, "in_N", m2);
	ev.shader_api.set_var(sh, "time", e.time);
	ev.shader_api.set_var(sh, "in_POS", e.in_POS);
#endif
     }
	next->execute(ee);
    if (sh.id!=-1)
      ev.shader_api.unuse(sh);
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  GameApi::SH sh;
  bool firsttime;
  Point light_pos;
  float dist;
  int dyn_point;
};


EXPORT GameApi::ML GameApi::PolygonApi::noise_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NoiseShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::custom_shader(EveryApi &ev, ML mainloop, std::string v_shaderstring, std::string f_shaderstring, std::string v_funcname, std::string f_funcname)
{
  std::cout << "ERROR: custom_shader() is missing!" << std::endl;
  GameApi::ML ml; ml.id=-1; return ml;
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new CustomShaderML(ev, item,v_shaderstring,f_shaderstring,v_funcname,f_funcname));
#endif
}
EXPORT GameApi::ML GameApi::PolygonApi::spotlight_shader(EveryApi &ev, ML mainloop, int light_color_id, MN move)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  Movement *change = find_move(e, move);
  return add_main_loop(e, new SpotlightShaderML(e,ev, item, light_color_id, change));
#endif
}
EXPORT GameApi::ML GameApi::PolygonApi::coloured_lights_shader(EveryApi &ev, ML mainloop, float scale,
							       unsigned int color_1, unsigned int color_2, unsigned int color_3, unsigned int color_4, unsigned int color_5, unsigned int color_6, unsigned int color_7, unsigned int color_8,
							       PT pos_1, PT pos_2, PT pos_3, PT pos_4, PT pos_5, PT pos_6, PT pos_7, PT pos_8,
							       float dist_1, float dist_2, float dist_3, float dist_4, float dist_5, float dist_6, float dist_7, float dist_8)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  Point *p1 = find_point(e, pos_1);
  Point *p2 = find_point(e, pos_2);
  Point *p3 = find_point(e, pos_3);
  Point *p4 = find_point(e, pos_4);
  Point *p5 = find_point(e, pos_5);
  Point *p6 = find_point(e, pos_6);
  Point *p7 = find_point(e, pos_7);
  Point *p8 = find_point(e, pos_8);
  
  return add_main_loop(e, new ColouredLightsShaderML(e,ev, item, scale, color_1, color_2, color_3, color_4, color_5, color_6, color_7, color_8,
						     *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8,
						     dist_1, dist_2, dist_3, dist_4, dist_5, dist_6, dist_7, dist_8
						     ));
#endif
}

#if 0
EXPORT GameApi::ML GameApi::PolygonApi::ambient_shader(EveryApi &ev, ML mainloop, int ambient_color_id, float ambient_level)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new AmbientShaderML(ev, item, ambient_color_id, ambient_level));
}
#endif
EXPORT GameApi::ML GameApi::PolygonApi::choose_color_shader(EveryApi &ev, ML mainloop, unsigned int color, float mix_val)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ChooseColorShaderML(e, ev, item, color, mix_val));
}
EXPORT GameApi::ML GameApi::PolygonApi::texture_shader(EveryApi &ev, ML mainloop, float mix=0.5)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new TextureShaderML(ev, item, mix));
}
EXPORT GameApi::ML GameApi::PolygonApi::mixshader_shader(EveryApi &ev, ML mainloop, float mix)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ColorMixShaderML(ev, item, mix));
#endif
}
 EXPORT GameApi::ML GameApi::PolygonApi::texture_many_shader(EveryApi &ev, ML mainloop, float mix=0.5)
 {
   MainLoopItem *item = find_main_loop(e, mainloop);
   return add_main_loop(e, new TextureManyShaderML(ev, item, mix));
 }
EXPORT GameApi::ML GameApi::PolygonApi::gltf_shader(EveryApi &ev, ML mainloop, float mix, bool tex0, bool tex1, bool tex2, bool tex3, bool tex4, bool tex5, bool tex6, bool tex7, float roughness, float metallic, float basecolor0, float basecolor1, float basecolor2, float basecolor3, float occul, float emiss, bool spec, float diff_factor_r, float diff_factor_g, float diff_factor_b, float spec_factor_r, float spec_factor_g, float spec_factor_b, float glossi_factor, bool unlit, float emis2_r, float emis2_g, float emis2_b, float light_dir_x, float light_dir_y, float light_dir_z, std::string cache_id, float self_mult, float rest_mult)
 {
   MainLoopItem *item = find_main_loop(e, mainloop);
   return add_main_loop(e, new GLTFShaderML(ev, item, mix,self_mult,rest_mult,tex0,tex1,tex2,tex3,tex4, tex5, tex6,tex7,roughness, metallic, basecolor0,basecolor1,basecolor2, basecolor3, occul, emiss,spec, Vector(diff_factor_r,diff_factor_g, diff_factor_b), Vector(spec_factor_r,spec_factor_g,spec_factor_b), glossi_factor, unlit,emis2_r,emis2_g,emis2_b,Vector(light_dir_x, light_dir_y, light_dir_z),cache_id));
 }
 EXPORT GameApi::ML GameApi::PolygonApi::texture_cubemap_shader(EveryApi &ev, ML mainloop, float mix=0.5, float mix2=0.5)
 {
   MainLoopItem *item = find_main_loop(e, mainloop);
   return add_main_loop(e, new TextureCubemapShaderML(ev, item, mix,mix2));
 }
EXPORT GameApi::ML GameApi::PolygonApi::texture_arr_shader(EveryApi &ev, ML mainloop, float mix=0.5)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new TextureArrShaderML(ev, item, mix));
}

EXPORT GameApi::ML GameApi::PolygonApi::light_shader(EveryApi &ev, ML mainloop)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new LightShaderML(ev, item));
#endif
}
EXPORT GameApi::ML GameApi::PolygonApi::toon_shader(EveryApi &ev, ML mainloop)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ToonShaderML(ev, item));
}
EXPORT GameApi::ML GameApi::PolygonApi::fade_shader(EveryApi &ev, ML mainloop, float start_time, float end_time, float start_time2, float end_time2)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new FadeShaderML(ev, item, start_time, end_time, start_time2, end_time2));
}
 EXPORT GameApi::ML GameApi::PolygonApi::dist_field_mesh_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new DistFieldMeshShader(e,ev,item,sfo));
}
 EXPORT GameApi::ML GameApi::PolygonApi::mesh_color_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new MeshColorShader(e,ev,item,sfo));
}
 EXPORT GameApi::ML GameApi::PolygonApi::sfo_sandbox_shader(EveryApi &ev, ML mainloop, SFO sfo)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new SFOSandboxShader(e,ev,item,sfo));
#endif
}
EXPORT GameApi::ML GameApi::PolygonApi::skeletal_shader(EveryApi &ev, ML mainloop, std::vector<SA> vec)
{
  std::vector<SkeletalNode*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_skeletal(e,vec[i]));
    }
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new SkeletalShader(e,ev,item,vec2));
}
EXPORT GameApi::ML GameApi::PolygonApi::glowedge_shader(EveryApi &ev, ML mainloop, float white_level, float gray_level, float edge_pos)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new GlowEdgeShaderML(e, ev, item, white_level, gray_level, edge_pos));
}
EXPORT GameApi::ML GameApi::PolygonApi::water_shader(EveryApi &ev, ML mainloop, unsigned int color1, unsigned int color2, unsigned int color3, float center_x, float center_y, float center_z, float wave_mult, float time_mult)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new WaterShaderML(e, ev, item, color1,color2,color3,Point(center_x,center_y,center_z), wave_mult, time_mult));
}

EXPORT GameApi::ML GameApi::PolygonApi::newshadow_shader_1(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, float scale)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NewShadowShaderML_1(e,ev,item,Vector(light_dir_x,light_dir_y,light_dir_z),scale));
}
class NewShadowShaderML_2;
EXPORT GameApi::ML GameApi::PolygonApi::newshadow_shader_2_phong(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, float dark_level, float light_level, float scale)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NewShadowShaderML_2(e,ev,item,Vector(light_dir_x,light_dir_y,light_dir_z),dark_level,light_level,scale,true,0));
}
EXPORT GameApi::ML GameApi::PolygonApi::newshadow_shader_2_gltf(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, float dark_level, float light_level, float scale, int texindex)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new NewShadowShaderML_2(e,ev,item,Vector(light_dir_x,light_dir_y,light_dir_z),dark_level,light_level,scale,false,texindex));
}

EXPORT GameApi::ML GameApi::PolygonApi::blurred_render_shader(EveryApi &ev, ML mainloop, int numsamples, float blur_radius)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new BlurredRenderML(e, ev, item, numsamples, blur_radius));
  
}

EXPORT GameApi::ML GameApi::PolygonApi::phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int specular, unsigned int highlight, float pow)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new PhongShaderML(e, ev, item, Vector(light_dir_x, light_dir_y, -light_dir_z),ambient, specular, highlight,pow,false));
}

EXPORT GameApi::ML GameApi::PolygonApi::adjust_shader(EveryApi &ev, ML mainloop, unsigned int ad_color, float ad_dark, float ad_light)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new AdjustShaderML(e, ev, item, ad_color,ad_dark,ad_light));
}

EXPORT GameApi::ML GameApi::PolygonApi::phong_shader2(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int specular,  unsigned int highlight, float pow)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new PhongShaderML(e, ev, item, Vector(light_dir_x, light_dir_y, -light_dir_z),ambient, specular, highlight,pow,true));
}
EXPORT GameApi::ML GameApi::PolygonApi::vertex_phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow, float mix)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new VertexPhongShaderML(e, ev, item, Vector(light_dir_x, light_dir_y, -light_dir_z),ambient, highlight,pow,mix));
}
EXPORT GameApi::ML GameApi::PolygonApi::edge_shader(EveryApi &ev, ML mainloop, float edge_width, unsigned int edge_color)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new EdgeShaderML(e,ev,item,edge_width,edge_color));
#endif
}
EXPORT GameApi::ML GameApi::PolygonApi::globe_shader(EveryApi &ev, ML mainloop, float globe_r)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new GlobeShaderML(e, ev, item, globe_r));
}

//EXPORT GameApi::ML GameApi::PolygonApi::ao_shader(EveryApi &ev, ML mainloop, float radius, int kernelsize, int noisesize)
//{
//  MainLoopItem *item = find_main_loop(e, mainloop);
//  return add_main_loop(e, new AOShaderML(e,ev, item, radius, kernelsize, noisesize));
//}
EXPORT GameApi::ML GameApi::PolygonApi::gi_shader(EveryApi &ev, ML mainloop, PTS points, float obj_size)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  PointsApiPoints *pts = find_pointsapi_points(e,points);
  return add_main_loop(e, new GIShaderML(e, ev, item, pts, obj_size));

}
EXPORT GameApi::ML GameApi::PolygonApi::colour_shader(EveryApi &ev, ML mainloop, float mix)
 {
   MainLoopItem *item = find_main_loop(e, mainloop);
   return add_main_loop(e, new ColourShaderML(e,ev,item,mix));
 }
EXPORT GameApi::ML GameApi::PolygonApi::bump_phong_shader(EveryApi &ev, ML mainloop, float light_dir_x, float light_dir_y, float light_dir_z, unsigned int ambient, unsigned int highlight, float pow)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new BumpPhongShaderML(e, ev, item, Vector(light_dir_x, light_dir_y, light_dir_z),ambient, highlight,pow));
}
EXPORT GameApi::ML GameApi::PolygonApi::fog_shader(EveryApi &ev, ML mainloop, float fog_dist, unsigned int dark_color, unsigned int light_color)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new FogShaderML(e,ev,item, fog_dist, dark_color, light_color));
}
EXPORT GameApi::ML GameApi::PolygonApi::shadow_shader(EveryApi &ev, ML mainloop, int tex_num, float p_x, float p_y, float p_z, unsigned int dark_color, float mix)
{
#if 0
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new ShadowShaderML(e,ev,item,tex_num,Point(p_x,p_y,p_z), dark_color,mix));
#endif
}

EXPORT GameApi::ML GameApi::PolygonApi::dyn_lights_shader(EveryApi &ev, ML mainloop, float light_pos_x, float light_pos_y, float light_pos_z, float dist, int dyn_point)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  return add_main_loop(e, new DynLightsShaderML(e, ev, item, Point(light_pos_x, light_pos_y, light_pos_z),dist,dyn_point));
}


EXPORT GameApi::ML GameApi::PolygonApi::shading_shader(EveryApi &ev, ML mainloop, unsigned int level1, unsigned int level2, unsigned int level3, float spec_size, bool ambient, bool diffuse, bool specular)
{
  MainLoopItem *item = find_main_loop(e, mainloop);
  Vector light_dir(1.0,1.0,1.0);
  return add_main_loop(e, new ShaderParamML(e, ev, item, light_dir, level1, level2, level3, spec_size, ambient, diffuse, specular));
}
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml(EveryApi &ev, VA va)
{
  return add_main_loop(e, new RenderVA(e, ev, *this, va));
}
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2(EveryApi &ev, P p)
{
#ifdef HAS_INSTANCING
    GameApi::PTS pts = ev.points_api.single_pts();
    GameApi::ML ml = ev.materials_api.render_instanced_ml(ev,p,pts);
    return ml;
#else
     return add_main_loop(e, new RenderP(e, ev, *this, p));
#endif
}
void confirm_texture_usage(GameApi::Env &e, GameApi::P p);


EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2_texture(EveryApi &ev, P p, std::vector<BM> bm, std::vector<int> types, std::vector<std::string> id_labels)
 {
   confirm_texture_usage(e,p);
   return add_main_loop(e, new RenderPTex(e, ev, *this, p, bm,types,id_labels));
 }
EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2_texture_id(EveryApi &ev, P p, std::vector<TXID> *bm)
 {
   confirm_texture_usage(e,p);
   return add_main_loop(e, new RenderPTex_id(e, ev, *this, p, bm));
 }
 EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2_cubemap(EveryApi &ev, P p, std::vector<BM> bm)
 {
   confirm_texture_usage(e,p);
   return add_main_loop(e, new RenderPTexCubemap(e, ev, *this, p, bm));
 }
 EXPORT GameApi::ML GameApi::PolygonApi::render_vertex_array_ml2_texture2(EveryApi &ev, P p)
 {
   confirm_texture_usage(e,p);
   return add_main_loop(e, new RenderPTex2(e, ev, *this, p));
 }
EXPORT GameApi::ML GameApi::PolygonApi::render_dynamic_ml(EveryApi &ev, P p, DC dyn)
{
  return add_main_loop(e, new RenderDynP(e, ev, *this, p, dyn));
}
EXPORT void GameApi::PolygonApi::print_stat(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  s->print_stat(0);
}
EXPORT bool GameApi::PolygonApi::is_texture(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  return s->is_texture();
}
EXPORT bool GameApi::PolygonApi::is_array_texture(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  return s->is_array_texture();
}
void GameApi::PolygonApi::sort_vertices(VA va, M m)
{
  Matrix mm = find_matrix(e,m);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  rend->sort_blit(0,mm);
}
EXPORT void GameApi::PolygonApi::render_vertex_array(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  OpenglLowApi *ogl = g_low->ogl;

  
#ifdef VAO
  if (rend->tri_count>0)
    ogl->glBindVertexArray(rend->vao[0]);
  else
  if (rend->quad_count>0)
    ogl->glBindVertexArray(rend->vao[1]);
  else
  if (rend->poly_count>0)
    ogl->glBindVertexArray(rend->vao[2]);
#endif
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	int texid = s->texture_many_ids[i];
	if (texid>=SPECIAL_TEX_ID_CUBEMAP && texid<SPECIAL_TEX_ID_CUBEMAP_END)
	  {
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+i);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, texid-SPECIAL_TEX_ID_CUBEMAP);
      //ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
	  } else {

	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
	// ogl->glEnable(Low_GL_TEXTURE_2D);
	}
      }
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);
      //ogl->glDisable(Low_GL_TEXTURE_2D);
      //ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      //ogl->glEnable(Low_GL_TEXTURE_2D);
      rend->render(0);
      //ogl->glDisable(Low_GL_TEXTURE_2D);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID_CUBEMAP && s->texture_id<SPECIAL_TEX_ID_CUBEMAP_END)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, s->texture_id-SPECIAL_TEX_ID_CUBEMAP);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      //ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);
      //ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //ogl->glEnable(Low_GL_TEXTURE_2D);
      //arr.render(0);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      // ogl->glDisable(Low_GL_TEXTURE_2D);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      //ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
      rend->render(0);
      //ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
#ifdef VAO
  ogl->glBindVertexArray(0);
#endif
}
EXPORT void GameApi::PolygonApi::clone(VA va)
{
  VertexArraySet *s = find_vertex_array(e, va);
  s->clone(0,1);
}
EXPORT void GameApi::PolygonApi::render_vertex_array_dyn(VA va, DC dc, MainLoopEnv &ee)
{
  OpenglLowApi *ogl = g_low->ogl;

  DynamicChange *ddc = find_dyn_change(e, dc);
  VertexArraySet *s = find_vertex_array(e, va);
  s->apply_change(ddc, 0, 1, ee);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  rend->update(1);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(1);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID_CUBEMAP && s->texture_id<SPECIAL_TEX_ID_CUBEMAP_END)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, s->texture_id-SPECIAL_TEX_ID_CUBEMAP);
     
      
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);
      
      //g_low->ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

    }

  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);

      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render(1);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(1);
    }
}
EXPORT void GameApi::PolygonApi::prepare_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);
  rend->prepare_instanced(0, (Point*)arr->array, (Vector*)arr->normal, (unsigned int *)arr->color, arr->numpoints);
}

EXPORT void GameApi::PolygonApi::prepare_vertex_array_instanced_matrix(ShaderApi &shapi, VA va, MSA pta, SH sh)
{
  //VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  MatrixArray3 *arr = find_matrix_array3(e, pta);
  rend->prepare_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, arr->numpoints);
}

EXPORT void GameApi::PolygonApi::render_vertex_array_instanced(ShaderApi &shapi, VA va, PTA pta, SH sh, int hide_n)
{
  OpenglLowApi *ogl = g_low->ogl;
#if 0
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);

  
  int ss = arr->numpoints;
  for(int i=0;i<ss;i++)
    {
      float x = arr->array[i*3];
      float y = arr->array[i*3+1];
      float z = arr->array[i*3+2];
      shapi.use(sh);
      shapi.set_var(sh, "in_InstPos", x,y,z);

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else
  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID_CUBEMAP && s->texture_id<SPECIAL_TEX_ID_CUBEMAP_END)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, s->texture_id-SPECIAL_TEX_ID_CUBEMAP);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
    }
#else
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  PointArray3 *arr = find_point_array3(e, pta);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	int txid = s->texture_many_ids[i];
	if (txid>=SPECIAL_TEX_ID_CUBEMAP && txid<SPECIAL_TEX_ID_CUBEMAP_END)
	  {
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+i);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, txid-SPECIAL_TEX_ID_CUBEMAP);

	  } else {
	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
	}
      }
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced(0, (Point*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced(0, (Point*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array,(Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      //rend->render(0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);

      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array,(Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced(0, (Point*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color,show_num);
      //rend->render(0);
    }
#endif
}

EXPORT void GameApi::PolygonApi::render_vertex_array_instanced_matrix(ShaderApi &shapi, VA va, MSA pta, SH sh, int hide_n)
{
    OpenglLowApi *ogl = g_low->ogl;

#if 0
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  MatrixArray3 *arr = find_point_array3(e, pta);

  
  int ss = arr->numpoints;
  for(int i=0;i<ss;i++)
    {
      float x = arr->array[i*3];
      float y = arr->array[i*3+1];
      float z = arr->array[i*3+2];
      shapi.use(sh);
      shapi.set_var(sh, "in_InstPos", x,y,z);

  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
      }
      rend->render(0);

      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else
  if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID_CUBEMAP && s->texture_id<SPECIAL_TEX_ID_CUBEMAP_END)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_CUBE_MAP);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, s->texture_id-SPECIAL_TEX_ID_CUBEMAP);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_CUBE_MAP);

    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);

      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);
      rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      rend->render(0);
    }
    }
#else
  VertexArraySet *s = find_vertex_array(e, va);
  RenderVertexArray *rend = find_vertex_array_render(e, va);
  MatrixArray3 *arr = find_matrix_array3(e, pta);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  if (s->texture_many_ids.size()!=0) {
    //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
    int ss = s->texture_many_ids.size();
    for(int i=0;i<ss;i++)
      {
	int txid = s->texture_many_ids[i];
	if (txid>=SPECIAL_TEX_ID_CUBEMAP && txid<SPECIAL_TEX_ID_CUBEMAP_END)
	  {
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+i);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP, txid-SPECIAL_TEX_ID_CUBEMAP);

	  } else {
	ogl->glActiveTexture(Low_GL_TEXTURE0+i);
#ifndef EMSCRIPTEN
        ogl->glClientActiveTexture(Low_GL_TEXTURE0+i);
#endif
	ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_many_ids[i]);
	}
      }
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
      ogl->glBindTexture(Low_GL_TEXTURE_CUBE_MAP,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);

  } 
  else if (s->texture_id!=-1 && s->texture_id<SPECIAL_TEX_ID)
    {
      TextureEnable(*env->renders[s->texture_id], 0, true);
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);
      rend->render_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      TextureEnable(*env->renders[s->texture_id], 0, false);
    }
  else if (s->texture_id!=-1 && s->texture_id>=SPECIAL_TEX_ID && s->texture_id<SPECIAL_TEX_IDA)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D, s->texture_id-SPECIAL_TEX_ID);

      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      //rend->render(0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D,0);

      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D);
    }
  else if (s->texture_id!=-1)
    {
      //g_low->ogl->glEnable(Low_GL_TEXTURE_2D_ARRAY);
#ifndef EMSCRIPTEN
      ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
      ogl->glActiveTexture(Low_GL_TEXTURE0+0);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY, s->texture_id-SPECIAL_TEX_IDA);

      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      //g_low->ogl->glDisable(Low_GL_TEXTURE_2D_ARRAY);
      ogl->glBindTexture(Low_GL_TEXTURE_2D_ARRAY,0);
    }
  else
    {
      //RenderVertexArray arr(*s);
      //arr.render(0);
      int hide_num = 0;
      if (hide_n != -1) { hide_num = hide_n; }
      int show_num = arr->numpoints-hide_num;
      show_num = std::max(0,show_num);
      show_num = std::min(arr->numpoints, show_num);

      rend->render_instanced_matrix(0, (Matrix*)arr->array, (Vector*)arr->normal, (unsigned int*)arr->color, show_num);
      //rend->render(0);
    }
#endif
}


#if 0
class AnimFace : public ForwardFaceCollection
{
public:
  AnimFace(FaceCollection &coll, Vector v) : ForwardFaceCollection(coll), /*coll(coll),*/ v(v) { }
  Point EndFacePoint(int face, int point) const 
  { return ForwardFaceCollection::FacePoint(face, point) + v; 
  }

private:
  //FaceCollection &coll;
  Vector v;
};
#endif

EXPORT GameApi::P GameApi::PolygonApi::anim_target_vector(P p, V v)
{
#if 0
  FaceCollection *i = find_facecoll(e, p);
  Vector *vv = find_vector(e,v);
  FaceCollection *coll = new AnimFace(*i, *vv);
  return add_polygon(e, coll, 1);
#endif
}
#if 0
class AnimColl : public ForwardFaceCollection
{
public:
  AnimColl(FaceCollection *i1, FaceCollection *i2) : ForwardFaceCollection(*i1), i1(i1), i2(i2) { }
  virtual Point EndFacePoint(int face, int point) const { return i2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return i2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return i2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return i2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return i2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return i2->TexCoord(face,point); }
private:
  FaceCollection *i1;
  FaceCollection *i2;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::anim_endpoints(P p1, P p2)
{
#if 0
  FaceCollection *i1 = find_facecoll(e,p1);
  FaceCollection *i2 = find_facecoll(e,p2);
  FaceCollection *coll = new AnimColl(i1, i2);
  return add_polygon(e, coll, 1);
#endif
}
#if 0
class AnimInterpolate : public ForwardFaceCollection
{
public:
  AnimInterpolate(FaceCollection *coll, float val) : ForwardFaceCollection(*coll), coll(coll), val(val) { }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const { return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = coll->FacePoint(face, point);
    Point p2 = coll->EndFacePoint(face, point);
    return Point::Interpolate(p1, p2, val);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v1 = coll->PointNormal(face,point);
    Vector v2 = coll->EndPointNormal(face, point);
    return Vector(Point::Interpolate(Point(v1),Point(v2), val));
  }
  virtual float Attrib(int face, int point, int id) const
  {
    float val1 = coll->Attrib(face,point,id);
    float val2 = coll->EndAttrib(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    int val1 = coll->AttribI(face,point,id);
    int val2 = coll->EndAttribI(face,point,id);
    return val1*val+val2*(1.0-val);
  }
  virtual unsigned int Color(int face, int point) const
  {
    unsigned int c1 = coll->Color(face,point);
    unsigned int c2 = coll->EndColor(face,point);
    return Color::CubicInterpolate(c1,c2, val);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p1 = coll->TexCoord(face,point);
    Point2d p2 = coll->EndTexCoord(face,point);
    Point2d pos = { float(p1.x*val+p2.x*(1.0-val)), float(p1.y*val+p2.y*(1.0-val)) };
    return pos;
  }
private:
  FaceCollection *coll;
  float val;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::anim_interpolate(P p, float val)
{
#if 0
  FaceCollection *i = find_facecoll(e,p);
  FaceCollection *coll = new AnimInterpolate(i, val);
  return add_polygon(e, coll, 1);
#endif
}

#if 0
class AnimFaceScale : public ForwardFaceCollection
{
public:
  AnimFaceScale(FaceCollection &coll, Point p, float scale_x, float scale_y, float scale_z) : ForwardFaceCollection(coll), /*coll(coll),*/ p(p), scale_x(scale_x), scale_y(scale_y), scale_z(scale_z) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * Matrix::Translate(-p.x,-p.y,-p.z)
      * Matrix::Scale(scale_x, scale_y, scale_z)
      * Matrix::Translate(p.x,p.y,p.z);
  }

private:
  //FaceCollection &coll;
  Point p;
  float scale_x, scale_y, scale_z;
};
#endif

EXPORT GameApi::P GameApi::PolygonApi::anim_target_scale(P p, PT center, float scale_x, float scale_y, float scale_z)
{
#if 0
  FaceCollection *i = find_facecoll(e, p);
  Point *pp = find_point(e, center);
  FaceCollection *coll = new AnimFaceScale(*i, *pp, scale_x, scale_y, scale_z);
  return add_polygon(e, coll, 1);
#endif
}
/*
class VolumeObjectFromCutter : public VolumeObject
{
public:
  VolumeObjectFromCutter(Cutter *cut) : cut(cut) { }
  bool Inside(Point v) const
  {
    std::vector<Point> v=cut->cut(Point(-10000.0,0,0), v);
    return v.size()%2==1;
  }
private:
  Cutter *cut;
};

EXPORT GameApi::O GameApi::CutterApi::cutter_volume2(CT c)
{
  Cutter *ct = find_cutter(e,c);
  return add_volume(e, new VolumeObjectFromCutter(ct));
}
*/
class CutFaces : public ForwardFaceCollection
{
public:
  CutFaces(FaceCollection *i, VolumeObject *oo, Cutter *cut) : ForwardFaceCollection(*i), i(i), oo(oo), cut(cut) { cut_them(); compress(); }
  
  virtual std::string name() const { return "CutFaces"; }
  
  void cut_them()
  {
    int f = i->NumFaces();
    for(int ii=0;ii<f; ii++)
      {
	faces.push_back(std::vector<Point>());
	int p = i->NumPoints(ii);
	//std::cout << p << std::endl;
	std::vector<Point> &ref = faces[faces.size()-1];
	//std::cout << "start" << std::endl;
	for(int jj=0;jj<p;jj++)
	  {
	    int jjj1 = jj;
	    while(jjj1<0) jjj1+=p;
	    while(jjj1>=p) jjj1-=p;
	    int jjj2 = jjj1+1;
	    while(jjj2<0) jjj2+=p;
	    while(jjj2>=p) jjj2-=p;
	    Point p1 = i->FacePoint(ii, jjj1);
	    Point p2 = i->FacePoint(ii, jjj2);
	    bool b1 = oo->Inside(p1);
	    bool b2 = oo->Inside(p2);

	    if (jj==0) { ref.push_back(p1); }

	    if (!b1 && !b2) {
	      //std::cout << "!b1 && !b2" << std::endl;
	      //ref.push_back(p1);
		  ref.push_back(p2);	      
	    }
	    if (!b1 && b2) {
	      //std::cout << "!b1 && b2" << std::endl;
	      Point prev = p1;
	      Point curr = p2;
	      for(int h=0;h<p;h++)
		{
		  int jjj3 = jjj2+h;
		  while(jjj3<0) jjj3+=p;
		  while(jjj3>=p) jjj3-=p;

		  curr = i->FacePoint(ii, jjj3);
		  bool b3 = oo->Inside(curr);
		  if (!b3) break;
		  prev = curr;
		}
	      std::vector<Point> c0 = cut->cut(p1,p2);
	      std::vector<Point> c1 = cut->cut(prev,curr);
	      //ref.push_back(p1);
	      ref.push_back(c0[0]);
	      ref.push_back(c1[0]);	      
	      ref.push_back(curr);
	    }
#if 1
	    if (b1 && !b2) {
	      //std::cout << "b1 && !b2" << std::endl;
	      std::vector<Point> c0 = cut->cut(p1,p2);
	      //ref.push_back(c0[0]);
	      ref.push_back(p2);	      
	    }
#endif
	    //if (b1 && b2)
	    //  {
	    //	  ref.push_back(p1);
	    //	  ref.push_back(p2);	      
	    //  }
	    
	    

	  }
      }
  }
  void compress()
  {
    int f = faces.size();
    int count = 0;
    for(int ii=0;ii<f; ii++)
      {
	int p = faces[ii].size();
	if (p==0 ||p==1 ||p==2) continue;
	faces2.push_back(std::vector<Point>());
#if 1
	Point prev;
	for(int i=0;i<p;i++)
	  {
	    if ((prev-faces[ii][i]).Dist()>0.001)
	      faces2[count].push_back(faces[ii][i]);
	    prev = faces[ii][i];
	  }
#endif
	//			faces2[count].push_back(faces2[count][0]);
	count++;
      }
  }

  virtual int NumFaces() const { return faces2.size(); }
  virtual int NumPoints(int face) const { return faces2[face].size(); }
  virtual Point FacePoint(int face, int point) const
  {
    return faces2[face][point];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0, 0.0, 0.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }


private:
  FaceCollection *i;
  VolumeObject *oo;
  Cutter *cut;
  std::vector<std::vector<Point> > faces;
  std::vector<std::vector<Point> > faces2;
};


#if 0
class AnimFaceMatrix : public ForwardFaceCollection
{
public:
  AnimFaceMatrix(FaceCollection &coll, Matrix m) : ForwardFaceCollection(coll), coll(coll), m(m) { }
  Point EndFacePoint(int face, int point) const 
  { 
    return ForwardFaceCollection::FacePoint(face, point)
      * m;
  }

private:
  FaceCollection &coll;
  Matrix m;
};

EXPORT GameApi::P GameApi::PolygonApi::anim_target_matrix(P p, M matrix)
{
  FaceCollection *i = find_facecoll(e, p);
  Matrix *mm = find_matrix(e,matrix);
  FaceCollection *coll = new AnimFaceMatrix(*i, *mm);
  
  return add_polygon(e, coll, 1);
}

EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p1, PT p2, PT p3, PT p4)
{
}
EXPORT GameApi::P GameApi::PolygonApi::sprite(Q bm, PT p, float mul_x, float mul_y)
{
}
#endif


EXPORT GameApi::P GameApi::PolygonApi::cut_faces(P p, O o, CT cutter)
{
  FaceCollection *i = find_facecoll(e, p);
  VolumeObject *oo = find_volume(e, o);
  Cutter *cut = find_cutter(e, cutter);
  FaceCollection *coll = new CutFaces(i, oo, cut);
  return add_polygon(e, coll, 1);
}
EXPORT GameApi::P GameApi::PolygonApi::intersect(EveryApi &ev, P p1, P p2,
						 O o1, O o2,
						 CT cutter1, CT cutter2)
{
  O o1_n = ev.volume_api.not_op(o1);
  O o2_n = ev.volume_api.not_op(o2);

  P p1_cut = cut_faces(p1, o2_n, cutter2);
  TS ts_1 = ev.ts_api.from_poly(p1_cut);
  P p_1 = ev.ts_api.to_poly(ts_1);

  P p2_cut = cut_faces(p2, o1_n, cutter1);
  TS ts_2 = ev.ts_api.from_poly(p2_cut);
  P p_2_ = ev.ts_api.to_poly(ts_2);

  P or_1 = or_elem(p_1, p_2_);
  return or_1;

}
#if 0
class TriToQuad : public ForwardFaceCollection
{
public:
  TriToQuad(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (coll->NumPoints(face)==3)
      {
	if (point==3) { return coll->FacePoint(face,2)+Vector(1.0,0.0,0.0); }
      }
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (point==3) point=2;
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const {
    if (point==3) point=2;
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (point==3) point=2;
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (point==3) point=2;
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (point==3) point=2;
    return coll->TexCoord(face,point);
  }

private:
  FaceCollection *coll;
};
#endif
EXPORT GameApi::P GameApi::PolygonApi::tri_to_quad(P p)
{
#if 0
  FaceCollection *poly = find_facecoll(e, p);
  return add_polygon2(e, new TriToQuad(poly),1);
#endif
}

class ColorMapPoly : public SingleForwardFaceCollection
{
public:
  ColorMapPoly(Bitmap<::Color> *bm, Point pos, Vector u_x, Vector u_y) : bm(bm), pos(pos), u_x(u_x), u_y(u_y) { prepared = false; }
  virtual std::string name() const { return "ColorMapPoly"; }

  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { prepared=true; } // not called

  void Prepare() { bm->Prepare(); prepared = true; }
  virtual int NumFaces() const { if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare(); return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare(); return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    //p.x = xx*sx/bm->SizeX();
    //p.y = yy*sy/bm->SizeY();
    //p.z = z;
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
if (!prepared) const_cast<ColorMapPoly*>(this)->Prepare();
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }
private:
  Bitmap<::Color> *bm;
  Point pos;
  Vector u_x, u_y;
  bool prepared;
};
#if 0
class ColorMapPoly4 : public FaceCollection
{
public:
  ColorMapPoly4(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) { 
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist();
  }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }

    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    p+=fb->Map(xx*fb->SizeX()/bm->SizeX(),yy*fb->SizeY()/bm->SizeY())*u_z;

    return p;
  }
};
#endif
class ColorMapPoly2 : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm),fb(fb), pos(pos), u_x(u_x), u_y(u_y) { u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); }
  virtual std::string name() const { return "ColorMapPoly2"; }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
  }
  void HeavyPrepare() { } // not called

  void Prepare() { bm->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Point p = pos + xx*u_x/bm->SizeX() + yy*u_y/bm->SizeY();
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(),yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val)) { 
      float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
      float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
      float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
      float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
      if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	{
	  val = val0;
	}
      else
	val=0.0; 
    }
    p+=val*u_z;
    //p.x = xx*sx/bm->SizeX();
    //p.y = yy*sy/bm->SizeY();
    //p.z = z;
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }
private:
  Bitmap<::Color> *bm;
  Bitmap<float> *fb;
  Point pos;
  Vector u_x, u_y;
  Vector u_z;
};
class ColorMapPoly2_cyl : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2_cyl(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) {
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); 
  }
  virtual std::string name() const { return "ColorMapPoly2_cyl"; }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { bm->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    Matrix m = Matrix::YRotation(xx*3.14159*2.0/bm->SizeX());
    Vector uu_x = u_x*m;
    Vector uu_y = u_y;
    Vector uu_z = u_z*m;
    Point p = pos + yy*uu_y/bm->SizeY();
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(), yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val))
      {
	float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	  {
	    val = val0;
	  }
	else
	  val=0.0; 
      }
    p+=val*uu_x + val*uu_z;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }

private:
  Bitmap<::Color> *bm;
  Bitmap<float> *fb;
  Point pos;
  Vector u_x, u_y;
  Vector u_z;

};


class ColorMapPoly2_sph : public SingleForwardFaceCollection
{
public:
  ColorMapPoly2_sph(Bitmap<::Color> *bm, Bitmap<float> *fb, Point pos, Vector u_x, Vector u_y) : bm(bm), fb(fb), pos(pos), u_x(u_x), u_y(u_y) {
    u_z = Vector::CrossProduct(u_x,u_y); u_z/=u_z.Dist(); 
    u_z *= u_x.Dist();
  }
  virtual std::string name() const { return "ColorMapPoly2_sph"; }
  void Collect(CollectVisitor &vis)
  {
    bm->Collect(vis);
    fb->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { bm->Prepare(); fb->Prepare(); }
  virtual int NumFaces() const { return bm->SizeX()*bm->SizeY(); }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    int xx1 = xx;
    int yy1 = yy;
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }

    float angle1 = 3.14159*xx/bm->SizeX();
    float angle2 = 2.0*3.14159*yy/bm->SizeY();

    Vector uu;
    uu.dx = u_x.Dist() * sin(angle1)*cos(angle2);
    uu.dy = u_y.Dist() * sin(angle1)*sin(angle2);
    uu.dz = u_z.Dist() * cos(angle1);

    //Matrix m = Matrix::YRotation(xx*3.14159*2.0/bm->SizeX());
    //Vector uu_x = u_x*m;
    //Vector uu_y = u_y;
    //Vector uu_z = u_z*m;
    Point p = pos;
    float val = fb->Map(xx*fb->SizeX()/bm->SizeX(), yy*fb->SizeY()/bm->SizeY());
    if (std::isnan(val))
      {
	float val0 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val1 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),yy1*fb->SizeY()/bm->SizeY());
	float val2 = fb->Map(xx1*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	float val3 = fb->Map((xx1+1)*fb->SizeX()/bm->SizeX(),(yy1+1)*fb->SizeY()/bm->SizeY());
	if (std::isnan(val0) && std::isnan(val1) && std::isnan(val2) && std::isnan(val3))
	  {
	    val = val0;
	  }
	else
	  val=0.0; 
      }
    p+=val*uu;
    return p;
  }

  virtual Vector PointNormal(int face, int point) const
  {
    Vector v(0.0,0.0,-1.0);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int xx = face/bm->SizeY();
    int yy = face - xx*bm->SizeY();
    if (point==1 ||point==2) { xx++; }
    if (point==2 ||point==3) { yy++; }
    return bm->Map(xx,yy).Pixel();
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p = {0.0, 0.0}; return p; }

private:
  Bitmap<::Color> *bm;
  Bitmap<float> *fb;
  Point pos;
  Vector u_x, u_y;
  Vector u_z;

};

EXPORT GameApi::ML GameApi::PolygonApi::sprite_render(EveryApi &ev, BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  GameApi::P I1=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  GameApi::BM I2=bm; 
  GameApi::MT I3=ev.materials_api.texture(ev,I2,1.0);
  GameApi::ML I4=ev.materials_api.bind(I1,I3);
  return I4;
}
EXPORT GameApi::ML GameApi::PolygonApi::sprite_render_inst(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z)
{
  P I5=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  BM I6=bm;
  MT I7=ev.materials_api.texture(ev,I6,1.0);
  ML I8=ev.materials_api.bind_inst(I5,pts,I7);
  return I8;
}
EXPORT GameApi::ML GameApi::PolygonApi::sprite_render_fade(EveryApi &ev, BM bm, PTS pts, float start_x, float end_x, float start_y, float end_y, float z, bool flip, float start_time, float end_time)
{
  P I9=ev.polygon_api.quad_z(start_x,end_x,start_y,end_y,z);
  BM I10=bm; //ev.bitmap_api.chessboard(10,10,8,8,ffffffff,ff888888);
  MT I11=ev.materials_api.texture(ev,I10,1.0);
  ML I12=ev.materials_api.bind_inst_fade(I9,pts,I11,flip,start_time,end_time);
  return I12;
}
							  
EXPORT GameApi::P GameApi::PolygonApi::color_map(BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);

  Point pos(start_x, start_y, z);
  Vector u_x(end_x-start_x, 0.0, 0.0);
  Vector u_y(0.0, end_y-start_y, 0.0);
  return add_polygon2(e, new ColorMapPoly(bm1, pos, u_x, u_y), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map2(BM bm, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  return add_polygon2(e, new ColorMapPoly(bm1, *pos1, *u_x1, *u_y1), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2(bm1, fb, *pos1, *u_x1, *u_y1), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3_cyl(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2_cyl(bm1, fb, *pos1, *u_x1, *u_y1), 1);
}
EXPORT GameApi::P GameApi::PolygonApi::color_map3_sph(BM bm, FB fb1, PT pos, V u_x, V u_y)
{
  Point *pos1 = find_point(e, pos);
  Vector *u_x1 = find_vector(e, u_x);
  Vector *u_y1 = find_vector(e, u_y);

  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;
  return add_polygon2(e, new ColorMapPoly2_sph(bm1, fb, *pos1, *u_x1, *u_y1), 1);
}


EXPORT GameApi::P GameApi::PolygonApi::color_map3(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2(bm1, fb, pos, u_x, u_y), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3_cyl(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2_cyl(bm1, fb, pos, u_x, u_y), 1);
}

EXPORT GameApi::P GameApi::PolygonApi::color_map3_sph(BM bm, FB fb1, float sx, float sy, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  Bitmap<::Color> *bm1 = find_color_bitmap(handle);
  Bitmap<float> *fb = find_float_bitmap(e, fb1)->bitmap;

  Point pos(0.0, 0.0, z);
  Vector u_x(sx, 0.0, 0.0);
  Vector u_y(0.0, sy, 0.0);
  return add_polygon2(e, new ColorMapPoly2_sph(bm1, fb, pos, u_x, u_y), 1);
}


#if 0
EXPORT GameApi::P GameApi::PolygonApi::cube_map(EveryApi &ev, float start_x, float end_x,
						float start_y, float end_y,
						float start_z, float end_z,
						BM bm_front, BM bm_back,
						BM bm_left, BM bm_right,
						BM bm_top, BM bm_bottom)
{
  PT pos_1 = ev.point_api.point(start_x, start_y, start_z);
  V u_x1 = ev.vector_api.vector(end_x-start_x, 0.0, 0.0);
  V u_y1 = ev.vector_api.vector(0.0, end_y-start_y, 0.0);
  P cube1 = color_map2(bm_front, pos_1, u_x1, u_y1);

  


}
#endif

EXPORT GameApi::LI GameApi::LinesApi::alt(std::vector<LI> v, int index)
{
  if (v.size()==0) return li_empty();
  int s = v.size();
  if (index<0 ||index>=s) return li_empty();
  return v[index];
}


EXPORT GameApi::MT GameApi::MaterialsApi::mt_alt(EveryApi &ev, std::vector<MT> v, int index)
{
  if (v.size()==0) return mt_empty(ev);
  int s = v.size();
  if (index<0 ||index>=s) return mt_empty(ev);
  return v[index];
}

EXPORT GameApi::P GameApi::PolygonApi::alt(std::vector<P> v, int index)
{
  if (v.size()==0) return p_empty();
  int s = v.size();
  if (index<0 ||index>=s) return p_empty();
  return v[index];
}
class SubPolyChange : public FaceCollection
{
public:
  SubPolyChange(FaceCollection *coll1, FaceCollection *coll2, VolumeObject *vol)
    : coll1(coll1), coll2(coll2), vol(vol) 
  {
    int s = coll1->NumFaces();
    for(int i=0;i<s;i++)
      {
	if (side(true, i)) { choose.push_back(0); faces.push_back(i); }
      }
    int s2 = coll2->NumFaces();
    for(int i2=0;i2<s2;i2++)
      {
	if (side(false, i2)) { choose.push_back(1); faces.push_back(i2); }
      }
  }
  virtual std::string name() const { return "SubPolyChange"; }

  void Collect(CollectVisitor &vis)
  {
    coll1->Collect(vis);
    coll2->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() { coll1->Prepare(); coll2->Prepare(); }
  bool side(bool which, int face) const
  {
    FaceCollection *coll = 0;
    if (which)
      {
	coll = coll1;
      }
    else
      {
	coll = coll2;
      }
    int s = coll->NumPoints(face);
    Point p(0.0,0.0,0.0);
    for(int i=0;i<s;i++)
      {
	p+=Vector(coll->FacePoint(face,i));
      }
    Vector v = p;
    v/=float(s);
    Point pp = v;
    bool b = vol->Inside(pp);
    if (which) b = !b;
    return b;
  }

  virtual bool has_normal() const { return coll1->has_normal()||coll2->has_normal(); }
  virtual bool has_attrib() const { return coll1->has_attrib()||coll2->has_attrib(); }
  virtual bool has_color() const { return coll1->has_color()||coll2->has_color(); }
  virtual bool has_texcoord() const { return coll1->has_texcoord()||coll2->has_texcoord(); }
  virtual bool has_skeleton() const { return coll1->has_skeleton()||coll2->has_skeleton(); }

  
  virtual int NumFaces() const { return choose.size(); }
  virtual int NumPoints(int face) const {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->NumPoints(faces[face]);
  }
  virtual Point FacePoint(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->FacePoint(faces[face], point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->PointNormal(faces[face], point);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->Color(faces[face], point);

  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int val = choose[face];
    FaceCollection *coll = 0;
    if (val==0) coll=coll1;
    if (val==1) coll=coll2;
    return coll->TexCoord(faces[face], point);
  }
private:
  FaceCollection *coll1;
  FaceCollection *coll2;
  VolumeObject *vol;
  std::vector<int> choose;
  std::vector<int> faces;
};
EXPORT GameApi::P GameApi::PolygonApi::subpoly_change(P p, P p2, O choose)
{
  FaceCollection *pa = find_facecoll(e, p);
  FaceCollection *pa2 = find_facecoll(e, p2);
  VolumeObject *choose_a = find_volume(e, choose);

  return add_polygon2(e, new SubPolyChange(pa, pa2, choose_a),1);
}
class TexCoordSpherical2 : public ForwardFaceCollection
{
public:
  TexCoordSpherical2(GameApi::Env &e, GameApi::EveryApi &ev, Point center, float r, FaceCollection *coll) : ForwardFaceCollection(*coll), e(e), ev(ev), center(center), r(r), coll(coll) 
  {
    GameApi::PT center2 = ev.point_api.point(center.x, center.y, center.z);
    fd = ev.dist_api.fd_sphere(center2, r);
  }
  virtual std::string name() const { return "TexCoordSpherical2"; }

  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  {
    Point p0 = coll->FacePoint(face, 0);
    Point p1 = coll->FacePoint(face, 1);
    Point p2 = coll->FacePoint(face, 2);
    Point p3 = coll->FacePoint(face, 3);
    Vector n = coll->PointNormal(face,point);
    GameApi::PT pp0 = ev.point_api.point(p0.x, p0.y, p0.z);
    GameApi::PT pp1 = ev.point_api.point(p1.x, p1.y, p1.z);
    GameApi::PT pp2 = ev.point_api.point(p2.x, p2.y, p2.z);
    GameApi::PT pp3 = ev.point_api.point(p3.x, p3.y, p3.z);
    GameApi::V v2 = ev.vector_api.vector(n.dx, n.dy, n.dz);
    GameApi::PT ip0 = ev.dist_api.ray_shape_intersect(fd, pp0, v2);
    GameApi::PT ip1 = ev.dist_api.ray_shape_intersect(fd, pp1, v2);
    GameApi::PT ip2 = ev.dist_api.ray_shape_intersect(fd, pp2, v2);
    GameApi::PT ip3 = ev.dist_api.ray_shape_intersect(fd, pp3, v2);
    GameApi::PT sph0 = ev.point_api.spherical_coords(ip0);
    GameApi::PT sph1 = ev.point_api.spherical_coords(ip1);
    GameApi::PT sph2 = ev.point_api.spherical_coords(ip2);
    GameApi::PT sph3 = ev.point_api.spherical_coords(ip3);
    float alfa0 = ev.point_api.pt_x(sph0);
    float beta0 = ev.point_api.pt_y(sph0);
    float alfa1 = ev.point_api.pt_x(sph1);
    float beta1 = ev.point_api.pt_y(sph1);
    float alfa2 = ev.point_api.pt_x(sph2);
    float beta2 = ev.point_api.pt_y(sph2);
    float alfa3 = ev.point_api.pt_x(sph3);
    float beta3 = ev.point_api.pt_y(sph3);


      bool alfa0b = alfa0<0.5;
      bool beta0b = beta0<0.5;
      bool alfa1b = alfa0<0.5;
      bool beta1b = beta0<0.5;
      bool alfa2b = alfa0<0.5;
      bool beta2b = beta0<0.5;
      bool alfa3b = alfa0<0.5;
      bool beta3b = beta0<0.5;

      float alfa_arr[] = { alfa0, alfa1, alfa2, alfa3 };
      float beta_arr[] = { beta0, beta1, beta2, beta3 };
      bool alfa_b_arr[] = { alfa0b, alfa1b,alfa2b,alfa3b };
      bool beta_b_arr[] = { beta0b, beta1b,beta2b,beta3b };
      
      float alfa = alfa_arr[point];
      float beta = beta_arr[point];

      float alfa_b = alfa_arr[point] <0.5;
      float beta_b = beta_arr[point] <0.5;


      int alfa_0_count=0;
      int alfa_1_count=0;
      int beta_0_count=0;
      int beta_1_count=0;
      
      if (alfa0b==false) alfa_0_count++; else alfa_1_count++;
      if (alfa1b==false) alfa_0_count++; else alfa_1_count++;
      if (alfa2b==false) alfa_0_count++; else alfa_1_count++;
      if (alfa3b==false) alfa_0_count++; else alfa_1_count++;

      if (beta0b==false) beta_0_count++; else beta_1_count++;
      if (beta1b==false) beta_0_count++; else beta_1_count++;
      if (beta2b==false) beta_0_count++; else beta_1_count++;
      if (beta3b==false) beta_0_count++; else beta_1_count++;


      bool choose_alfa = alfa_0_count>alfa_1_count ? 0 : 1;
      bool choose_beta = beta_0_count>beta_1_count ? 0 : 1;
      
      if (choose_alfa && alfa_b != true) { alfa+=3.14159; }
      if (!choose_alfa && alfa_b != false) { alfa-=3.14159; }
      if (choose_beta && beta_b != true) { beta+=3.14159*2.0; }
      if (!choose_beta && beta_b != false) { beta-=3.14159*2.0; }
      
      
      

    Point2d pp; 
    pp.x = alfa/3.14159265;
    pp.y = beta/3.14159265/2.0;
    std::cout << "texcoord: " << pp.x << " " << pp.y << std::endl;
    return pp;
  }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  Point center;
  float r;
  FaceCollection *coll;
  GameApi::FD fd;
};
EXPORT GameApi::P GameApi::PolygonApi::texcoord_spherical2(EveryApi &ev,
							  PT center,
							  float r,
							  P orig)
{
  FaceCollection *pa = find_facecoll(e, orig);
  Point *pt = find_point(e, center);
  return add_polygon2(e, new TexCoordSpherical2(e, ev, *pt, r, pa), 1);

}
class ColorFromTexcoord : public ForwardFaceCollection
{
public:
  ColorFromTexcoord(FaceCollection *coll, unsigned int color_tl, unsigned int color_tr,
		    unsigned int color_bl, unsigned int color_br)
    : ForwardFaceCollection(*coll), color_tl(color_tl), color_tr(color_tr),
      color_bl(color_bl), color_br(color_br) 
  {
  }
  virtual std::string name() const { return "ColorFromTexcoord"; }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  bool has_color() const { return true; }
  unsigned int Color(int face, int point) const
  {
    Point2d pos = ForwardFaceCollection::TexCoord(face,point);
    unsigned int c1 = Color::Interpolate(color_tl, color_tr, pos.x);
    unsigned int c2 = Color::Interpolate(color_bl, color_br, pos.x);
    unsigned int c3 = Color::Interpolate(c1,c2, pos.y);
    //std::cout << "ColorFromTexCoord: " << pos.x << " " << pos.y << ":" << std::hex << c1 << " " << c2 << " " << c3 << std::dec << std::endl;
    return c3;
  }
private:
  unsigned int color_tl, color_tr;
  unsigned int color_bl, color_br;
};
EXPORT GameApi::P GameApi::PolygonApi::color_from_texcoord(P orig,
							   unsigned int color_tl, unsigned int color_tr,
							   unsigned int color_bl, unsigned int color_br)
{
  FaceCollection *coll = find_facecoll(e, orig);
  return add_polygon2(e, new ColorFromTexcoord(coll, color_tl, color_tr, color_bl, color_br), 1);  
}
class DeformFaceCollection : public ForwardFaceCollection
{
public:
  DeformFaceCollection(FaceCollection *obj, VolumeObject *bools, Vector v) : ForwardFaceCollection(*obj), obj(obj), bools(bools), v(v) {}
  virtual std::string name() const { return "DeformFaceCollection"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  Point FacePoint(int face, int point) const
  {
    Point p = obj->FacePoint(face,point);
    bool b = bools->Inside(p);
    if (b) { p+=v; }
    return p;
  }
private:
  FaceCollection *obj;
  VolumeObject *bools;
  Vector v;
};
EXPORT GameApi::P GameApi::PolygonApi::deform(P obj, O bools, float dx, float dy, float dz)
{
  FaceCollection *obj_ = find_facecoll(e,obj);
  VolumeObject *bools_ = find_volume(e,bools);
  return add_polygon2(e, new DeformFaceCollection(obj_, bools_, Vector(dx,dy,dz)),1);
}
EXPORT GameApi::P GameApi::PolygonApi::and_not_elem(EveryApi &ev, P p1, P p_not,
						    O o1, O o_not,
						    CT cutter1, CT cutter_not)
{
  O o1_n = o1; //ev.volume_api.not_op(o1);
  O onot_n = ev.volume_api.not_op(o_not);

  P p1_cut = cut_faces(p1, onot_n, cutter_not);
  TS ts_1 = ev.ts_api.from_poly(p1_cut);
  P p_1 = ev.ts_api.to_poly(ts_1);

  P p_not_cut = cut_faces(p_not, o1_n, cutter1);
  TS ts_not = ev.ts_api.from_poly(p_not_cut);
  P p_not_ = ev.ts_api.to_poly(ts_not);


  P or_1 = or_elem(p_1, p_not_);
  return or_1;
  //return or_1;
}
GameApi::BM GameApi::PolygonApi::renderpolytobitmap(EveryApi &ev, P p, SH sh, float x, float y, float z, int sx, int sy)
{
  OpenglLowApi *ogl = g_low->ogl;

  GameApi::PolygonObj obj(ev, p, sh);
  obj.set_pos(x,y,z);
  obj.prepare();
  //int screen_width = ev.mainloop_api.get_screen_width();
  //int screen_height = ev.mainloop_api.get_screen_height();
  //Point2d pos = { 0.0, 0.0 };
  //Vector2d sz = { float(sx), float(sy) };
  //glViewport(pos.x, screen_height-pos.y-sz.dy, sz.dx, sz.dy);
  //ev.shader_api.use(sh);
  //ev.mainloop_api.switch_to_3d(true, sh, screen_width, screen_height);
  ogl->glEnable(Low_GL_DEPTH_TEST);
 
  FBO fbo = ev.fbo_api.create_fbo(ev,sx,sy);
  ev.fbo_api.config_fbo(fbo);
  GameApi::FrameBufferApi::vp viewport = ev.fbo_api.bind_fbo(fbo);

  ogl->glEnable(Low_GL_DEPTH_TEST);
  ogl->glDisable(Low_GL_BLEND);
  //glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
  //		       GL_ONE, GL_ONE);
  ev.mainloop_api.clear_3d(0x00000000);
  obj.render();
  ogl->glEnable(Low_GL_BLEND);
  while(!ev.fbo_api.fbo_status(fbo));
  //BM bm = ev.mainloop_api.screenshot();
  ev.fbo_api.bind_screen(viewport);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ogl->glDisable(Low_GL_DEPTH_TEST);
  //ev.mainloop_api.switch_to_3d(false, sh, screen_width, screen_height);

  //return ev.fbo_api.fbo_to_bitmap(ev, fbo);
  TXID id = ev.fbo_api.tex_id(fbo);
  //glDisable(GL_DEPTH_TEST);
  //glViewport(0,0,screen_width, screen_height);
  BM bm = ev.texture_api.to_bitmap(ev,id);
  BM bm2 = ev.bitmap_api.color_range(bm, 0x90ffffff, 0x00000000, 0xffffffff, 0x00000000);
  return bm2; 
}
struct CacheItem
{
public:
  std::string filename;
  GameApi::P obj;  
  int filesize;
  int obj_count;
};
int filesize(std::string filename)
{
  //std::ifstream in(filename, std::ifstream::ate|std::ifstream::binary);
  //return in.tellg();
  int s = filename.size();
  int sum = 0;
  for(int i=0;i<s;i++) sum+=int(filename[i]);
  return sum;
}
bool invalidate(CacheItem *item, std::string filename, int obj_count)
{
  int size = filesize(filename);
  return item->filesize != size || item->obj_count != obj_count;
}

#if 0
class PrepareCut : public ForwardFaceCollection
{
public:
  PrepareCut(FaceCollection *coll) : ForwardFaceCollection(*coll),coll(coll) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { }
private:
  FaceCollection *coll;
};
#endif
GameApi::P GameApi::PolygonApi::prepare_cut(P p)
{
#if 0
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new PrepareCut(coll), 1);
#endif
}

std::string cache_id(std::string filename, int obj_count)
{
  std::stringstream ss;
  ss << filename << obj_count;
  return ss.str();
}

struct CacheItemDel
{
  std::vector<CacheItem*> vec;
  ~CacheItemDel()
  {
    int s = vec.size();
    for(int i=0;i<s;i++) delete vec[i];
  }
};
CacheItemDel cache_del;

GameApi::P GameApi::PolygonApi::file_cache(P model, std::string filename, int obj_count)
{
  static std::map<std::string, CacheItem*> cache_map;
  CacheItem *item = cache_map[cache_id(filename,obj_count)];
  if (item && !invalidate(item,filename,obj_count)) {
    //std::cout << "From Cache" << std::endl;
    return item->obj;
  }
  //std::cout << "From File" << std::endl;
  CacheItem *item2 = new CacheItem;
  cache_del.vec.push_back(item2);
  item2->filename = filename;
  P p2 = memoize(model);
  item2->obj = prepare_cut(p2);
  item2->filesize = filesize(filename);
  item2->obj_count = obj_count;
  cache_map[cache_id(filename,obj_count)]=item2;
  return p2;
}
Matrix g_last_resize = Matrix::Identity();


void resize_reset();
void calc_resize_model(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p);
GameApi::ML resize_model(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::ML ml);


bool g_resize_disabled=false;

GameApi::ML GameApi::MainLoopApi::disable_resize(GameApi::ML ml)
{
  g_resize_disabled=true;
  return ml;
}



class ResizeFaceCollection : public ForwardFaceCollection
{
public:
  ResizeFaceCollection(FaceCollection *coll, bool ext_flag) : ForwardFaceCollection(*coll), coll(coll), ext_flag(ext_flag)
  {
  }
  ResizeFaceCollection(FaceCollection *coll, Matrix *m) : ForwardFaceCollection(*coll), coll(coll), ext_flag(true),ext_mat(m) { }
  virtual std::string name() const { return "ResizeFaceCollection"; }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    //stackTrace();
    find_bounding_box();
    print_bounding_box();
    calc_center();
    calc_size();
    calc_matrix();
  }

  void Prepare()
  {
    //stackTrace();
    coll->Prepare();

    find_bounding_box();
    print_bounding_box();
    calc_center();
    calc_size();
    calc_matrix();
  }
  Point FacePoint(int face, int point) const
  {
    // METHOD DISABLED
    if (g_resize_disabled) {
      Point p = coll->FacePoint(face,point);
      if (ext_flag) return p; //*(*ext_mat);
      return p; //*m_m;
    } else {
      Point p = coll->FacePoint(face,point);
      if (ext_flag) return p*(*ext_mat);
      return p*m_m;
    }
  }
public:
  Matrix get_matrix() const {
    if (g_resize_disabled) {
    // METHOD DISABLED
    return Matrix::Identity();
    } else {
    if (ext_flag) { return *ext_mat; }
    return m_m;
    }
  }

private:
  void find_bounding_box()
  {

    start_x = 300000.0;
      start_y = 300000.0;
      start_z = 300000.0;
      end_x =  -300000.0;
      end_y =  -300000.0;
      end_z =  -300000.0; 

    int s = std::min(coll->NumFaces(),100);
    if (s<1) s=1;
    int step = coll->NumFaces()/s;
    int faces = coll->NumFaces();
    for(int i=0;i<faces;i+=step)
      {
	    Point p1 = coll->FacePoint(i,0);
	    Point p2 = coll->FacePoint(i,1);
	    Point p3 = coll->FacePoint(i,2);
	    Point p4 = coll->NumPoints(i)==4 ? coll->FacePoint(i,3) : p1;

	    handlepoint(p1);
	    handlepoint(p2);
	    handlepoint(p3);
	    handlepoint(p4);
      }
  }
  void handlepoint(Point p)
  {
    if (p.x<start_x) { start_x = p.x; }
    if (p.y<start_y) { start_y = p.y; }
    if (p.z<start_z) { start_z = p.z; }
    if (p.x>end_x) { end_x = p.x; }
    if (p.y>end_y) { end_y = p.y; }
    if (p.z>end_z) { end_z = p.z; }    
  }

  void print_bounding_box()
  {
    //std::cout << "Bounding box:" << start_x << " " << end_x << " " << start_y << " " << end_y << " " << start_z << " " << end_z << std::endl;
  }
  void calc_center()
  {
    center_x = start_x+(end_x-start_x)/2.0;
    center_y = start_y+(end_y-start_y)/2.0;
    center_z = start_z+(end_z-start_z)/2.0;
  }
  void calc_size()
  {
    size_x = fabs(end_x-start_x);
    size_y = fabs(end_y-start_y);
    size_z = fabs(end_z-start_z);
  }
  void calc_matrix()
  {
    Matrix m = Matrix::Translate(-center_x, -center_y, -center_z);
    
    float val = std::max(std::max(size_x, size_y), size_z);
    float val2 = 1.0/val;
    val2*= 400.0;
    m_scale = val2;
    Matrix m2 = Matrix::Scale(val2, val2, val2);
    Matrix mm = m * m2;
    m_m= mm;
    if (!ext_flag)
      g_last_resize = mm;
  }

public:
  FaceCollection *coll;
  float start_x, start_y, start_z;
  float end_x, end_y, end_z;

  float center_x, center_y, center_z;
  float size_x, size_y, size_z;
  Matrix m_m;

  float m_scale;
  bool ext_flag = false;
  Matrix *ext_mat=0;
};
std::pair<float,Point> find_mesh_scale(FaceCollection *coll)
{
  if (g_resize_disabled) {
    return std::make_pair(1.0,Point(0.0,0.0,0.0));
  } else {
  // METHOD DISABLED
  
  ResizeFaceCollection *coll2 = new ResizeFaceCollection(coll,true);
  coll2->Prepare();
  std::pair<float,Point> p = std::make_pair(coll2->m_scale, Point(-coll2->center_x, -coll2->center_y, -coll2->center_z));
  delete coll2;
  return p;
  }
}


GameApi::P resize_to_correct_size2(GameApi::Env &e, GameApi::P model, Matrix *mat)
{
  FaceCollection *coll = find_facecoll(e, model);
  return add_polygon2(e, new ResizeFaceCollection(coll,mat),1);
}
GameApi::P GameApi::PolygonApi::resize_to_correct_size(P model)
{
  FaceCollection *coll = find_facecoll(e, model);
  return add_polygon2(e, new ResizeFaceCollection(coll,false),1);
}


Matrix g_resize_mat;
bool g_resize_mat_done=false;
void resize_reset() { g_resize_mat=Matrix::Identity(); g_resize_mat_done=false; }
void calc_resize_model(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  ResizeFaceCollection *resize = new ResizeFaceCollection(coll,false);
  resize->Prepare();
  Matrix m;
  if (!g_resize_mat_done) {
    if (!g_resize_disabled)
      m = resize->get_matrix(); // METHOD DISABLED
    else
      m=Matrix::Identity();
    g_resize_mat = m;
    g_resize_mat_done = true;
  }
}

GameApi::ML resize_model(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML ml)
{
  assert(g_resize_mat_done);
  Matrix m = g_resize_disabled?Matrix::Identity():g_resize_mat; 
  GameApi::MN mn = ev.move_api.mn_empty();
  mn = ev.move_api.matrix(mn,add_matrix2(e,m));
  return ev.move_api.move_ml(ev,ml,mn,1,0.0);
}



class PersistentCachePoly : public FaceCollection
{
public:
  PersistentCachePoly(FaceCollection &c, std::string filename) : c(c), filename(filename),res(0) { }
  virtual std::string name() const { return "PersistentCachePoly"; }
  
  void Collect(CollectVisitor &vis)
  {
    c.Collect(vis);
  }
  void HeavyPrepare() { Prepare(); }
  virtual void Prepare()
  {
    if (!res) {
      std::ifstream ss(filename.c_str());
      ss.tie(nullptr);
      int count = 0;
      char c2;
      while(ss.get(c2)) { count++; }
      if (count==0)
	{
	  std::cout << "Saving " << filename << std::endl;
	  SaveObjModelFaceCollection save(&c);
	  save.save(filename);
	}
      std::cout << "Loading " << filename << std::endl;
      std::ifstream data(filename.c_str());
      std::vector<unsigned char, GameApiAllocator<unsigned char> > vec2;
      char c;
      while(data.get(c)) vec2.push_back(c);

      LoadStream *stream = load_from_vector(vec2);

      ObjFileParser *parser = new ObjFileParser(stream, -1, std::vector<std::string>());

      res = new ObjFileFaceCollection(*parser,0);
      //res = new LoadObjModelFaceCollection(stream,0, std::vector<std::string>());
      res->Prepare();
    }
  }

  virtual bool has_normal() const { if (!res) return false; return res->has_normal(); }
  virtual bool has_attrib() const { if (!res) return false; return res->has_attrib(); }
  virtual bool has_color() const { if (!res) return false; return res->has_color(); }
  virtual bool has_texcoord() const { if (!res) return false; return res->has_texcoord(); }
  virtual bool has_skeleton() const { if (!res) return false; return res->has_skeleton(); }

  
  virtual int NumFaces() const 
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) return c.NumFaces();
    return res->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.NumPoints(face); }
    return res->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.FacePoint(face,point); }
    return res->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.PointNormal(face,point); }
    return res->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.Attrib(face,point,id); }
    return res->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.AttribI(face,point,id); }
    return res->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.Color(face,point); }
    return res->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.TexCoord(face,point); }
    return res->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const 
  {
    if (!res) { std::cout << "PersistentCachePoly:FaceCollection Prepare not called!" << std::endl; }
    if (!res) { return c.TexCoord3(face,point); }
    return res->TexCoord3(face,point);
  }
private:
  FaceCollection &c;
  std::string filename;
  FaceCollection *res;
};
GameApi::P GameApi::PolygonApi::persistent_cache(P p, std::string filename)
{
  FaceCollection *coll = find_facecoll(e, p);
  GameApi::P p2 = add_polygon2(e, new PersistentCachePoly(*coll, filename),1);
  GameApi::P cache = p2; //file_cache(p2, filename, 0);
  return cache;
}

class BuildOffsets : public ForwardFaceCollection
{
public:
  BuildOffsets(FaceCollection *coll, std::vector<Point> vec) : ForwardFaceCollection(*coll), coll(coll), vec(vec) { }
  virtual std::string name() const { return "BuildOffsets"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  Point FacePoint(int face, int point) const
  {
    int part = ForwardFaceCollection::AttribI(face,point, AttrPart);
    int s = vec.size();
    if (part>=0 && part<s)
      {
	Point p = vec[part];
	Point p2 = ForwardFaceCollection::FacePoint(face,point);
	Vector v = p2-p;
	Point p3 = v;
	return p3;
      }
    Point p0;
    p0.x = 0.0;
    p0.y = 0.0;
    p0.z = 0.0;
    return p0;
  }
private:
  FaceCollection *coll;
  std::vector<Point> vec;
};

GameApi::P GameApi::PolygonApi::build_offsets(P p, std::vector<PT> points)
{
  FaceCollection *coll = find_facecoll(e, p);
  int s = points.size();
  std::vector<Point> vec;
  for(int i=0;i<s;i++)
    {
      vec.push_back(*find_point(e, points[i]));
    }
  return add_polygon2(e, new BuildOffsets(coll, vec),1);  
}


class SplitterFaceCollection : public FaceCollection
{
public:
  SplitterFaceCollection(FaceCollection &coll, int start, int end) : coll(coll), start(start), end(end) { }
  virtual std::string name() const { return "SplitterFaceCollection"; }

  void Collect(CollectVisitor &vis)
  {
    coll.Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { return coll.Prepare(); }
  virtual int NumFaces() const { return std::min(end-start,coll.NumFaces()-start); }
  virtual int NumPoints(int face) const
  {
    return coll.NumPoints(face+start);
  }

  virtual bool has_normal() const { return coll.has_normal(); }
  virtual bool has_attrib() const { return coll.has_attrib(); }
  virtual bool has_color() const { return coll.has_color(); }
  virtual bool has_texcoord() const { return coll.has_texcoord(); }
  virtual bool has_skeleton() const { return coll.has_skeleton(); }
  
  virtual Point FacePoint(int face, int point) const { return coll.FacePoint(face+start,point); }
  virtual Vector PointNormal(int face, int point) const { return coll.PointNormal(face+start, point); }
  virtual float Attrib(int face, int point, int id) const { return coll.Attrib(face+start, point, id); }
  virtual int AttribI(int face, int point, int id) const { return coll.AttribI(face+start, point, id); }
  virtual unsigned int Color(int face, int point) const { return coll.Color(face+start, point); }
  virtual Point2d TexCoord(int face, int point) const { return coll.TexCoord(face+start,point); }
  virtual float TexCoord3(int face, int point) const { return coll.TexCoord3(face+start,point); }

private:
  FaceCollection &coll;
  int start;
  int end;
};
class TexCoordFromNormal : public ForwardFaceCollection
{
public:
  TexCoordFromNormal(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "TexCoordFromNormal"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  { 
    // NOTE: this requires power-of-2 textures, or else it will fail because of
    // clamp_to_edge.
    Vector n = coll->PointNormal(face,point);
    int nxt = point+2;
    nxt = nxt % coll->NumPoints(face);
    Vector n2 = coll->PointNormal(face,nxt);
    n/=n.Dist();
    n2/=n2.Dist();
    float r = sqrt(n.dx*n.dx+n.dy*n.dy+n.dz*n.dz);
    float r2 = sqrt(n2.dx*n2.dx+n2.dy*n2.dy+n2.dz*n2.dz);
    float alfa = acos(n.dz/r);
    float alfa2 = acos(n2.dz/r2);
    float beta = atan2(n.dy,n.dx);
    float beta2 = atan2(n2.dy,n2.dx);
    alfa/=3.14159265;
    beta/=2.0*3.14159265;
    alfa2/=3.14159265;
    beta2/=2.0*3.14159265;
#if 1
    while (alfa<0.0) alfa+=1.0;
    while (beta<0.0) beta+=1.0;
    while (alfa>1.0) alfa-=1.0;
    while (beta>1.0) beta-=1.0;

    while (alfa2<0.0) alfa2+=1.0;
    while (beta2<0.0) beta2+=1.0;
    while (alfa2>1.0) alfa2-=1.0;
    while (beta2>1.0) beta2-=1.0;
#endif
    if (alfa-alfa2 > 0.5) { alfa-=1.0; }
    if (beta-beta2 > 0.5) { beta-=1.0; }
    if (alfa-alfa2 < -0.5) { alfa+=1.0; }
    if (beta-beta2 < -0.5) { beta+=1.0; }
    
    Point2d p;
    p.x = alfa;
    p.y = beta;
    return p;
  }
private:
  FaceCollection *coll;
};
GameApi::P GameApi::PolygonApi::texcoord_from_normal(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new TexCoordFromNormal(coll),1);
}
class TexCoordFromNormal2 : public ForwardFaceCollection
{
public:
  TexCoordFromNormal2(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "TexCoordFromNormal2"; }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  { 
    Vector n = coll->PointNormal(face,point);
    while (n.dx<0.0) n.dx+=1.0;
    while (n.dy<0.0) n.dy+=1.0;
    while (n.dx>1.0) n.dx-=1.0;
    while (n.dy>1.0) n.dy-=1.0;
    Point2d p;
    p.x = n.dx;
    p.y = n.dy;
    return p;
  }
  float TexCoord3(int face, int point) const
  {
    Vector n = coll->PointNormal(face,point);
    return n.dz;
  }
private:
  FaceCollection *coll;
};
GameApi::P GameApi::PolygonApi::texcoord_from_normal2(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new TexCoordFromNormal2(coll),1);
}
#if 0
class FixTexCoord : public ForwardFaceCollection
{
public:
  FixTexCoord(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) {}
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const
  {
    int prev = point - 1;
    int next = point + 1;
    if (coll->NumPoints(face)==4) { prev--; next++; }
    prev = prev % coll->NumPoints(face);
    next = next % coll->NumPoints(face);

    Point2d tx_prev = coll->TexCoord(face, prev);
    Point2d tx_curr = coll->TexCoord(face, point);
    //Point2d tx_next = coll->TexCoord(face, next);

    Vector2d v1 = tx_curr - tx_prev;
    //Vector2d v2 = tx_next - tx_curr;
     
    float dist1 = v1.Dist();
    //float dist2 = v2.Dist();
    if (dist1>0.5) {
      Point2d tx_test1 = tx_curr;
      tx_test1.x -= 1.0;
      Point2d tx_test2 = tx_curr;
      tx_test2.y -= 1.0;
      Point2d tx_test3 = tx_curr;
      tx_test3.x += 1.0;
      Point2d tx_test4 = tx_curr;
      tx_test4.y += 1.0;

      float d0 = (tx_curr - tx_prev).Dist();
      float d1 = (tx_test1 - tx_prev).Dist();
      float d2 = (tx_test2 - tx_prev).Dist();
      float d3 = (tx_test3 - tx_prev).Dist();
      float d4 = (tx_test4 - tx_prev).Dist();
      if (d0 < d1 && d0 < d2 && d0 < d3 && d0 < d4) { return tx_curr; }
      if (d1 < d2 && d1 < d3 && d1 < d4) { return tx_test1; }
      if (d2 < d1 && d2 < d3 && d2 < d4) { return tx_test2; }
      if (d3 < d1 && d3 < d2 && d3 < d4) { return tx_test3; }
      if (d4 < d1 && d4 < d2 && d4 < d3) { return tx_test4; }
    }
    return tx_curr;
  }
private:
  FaceCollection *coll;
};
#endif
GameApi::P GameApi::PolygonApi::fix_texcoord(P p)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new FixTexCoord(coll),1);
#endif
}

GameApi::P GameApi::PolygonApi::split_p(P p, int start_face, int end_face)
{
#if 1
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SplitterFaceCollection(*coll, start_face,end_face),1);
#endif
}

class LineToCone : public FaceCollection
{
public:
  LineToCone(GameApi::Env &e, GameApi::EveryApi &ev, LineCollection *coll2, float size, int numfaces) : e(e), ev(ev), lines(coll2),size(size),numfaces(numfaces) { coll=0; }
  virtual std::string name() const { return "LineToCone"; }
  void Collect(CollectVisitor &vis)
  {
    lines->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (!coll) {
    int s = lines->NumLines();
    //std::cout << "LINETOCONE: " << s << std::endl;
    std::vector<GameApi::P> vec;
    for(int i=0;i<s;i++)
      {
	Point p1 = lines->LinePoint(i, 0);
	Point p2 = lines->LinePoint(i, 1);
	//std::cout << "LINETOCONE: " << p1 << " " << p2 << std::endl;
	GameApi::PT pp1 = ev.point_api.point(p1.x,p1.y,p1.z);
	GameApi::PT pp2 = ev.point_api.point(p2.x,p2.y,p2.z);
	GameApi::P ct = ev.polygon_api.cone(numfaces, pp1, pp2, size, size);
	vec.push_back(ct);
    }
    GameApi::P res = ev.polygon_api.or_array2(vec);
    coll = find_facecoll(e,res);
    }
  }
  void Prepare() {
    lines->Prepare();
    if (!coll) {
      int s = lines->NumLines();
      //std::cout << "LINETOCONE: " << s << std::endl;
      std::vector<GameApi::P> vec;
      for(int i=0;i<s;i++)
	{
	  Point p1 = lines->LinePoint(i, 0);
	  Point p2 = lines->LinePoint(i, 1);
	  //std::cout << "LINETOCONE: " << p1 << " " << p2 << std::endl;
	  GameApi::PT pp1 = ev.point_api.point(p1.x,p1.y,p1.z);
	  GameApi::PT pp2 = ev.point_api.point(p2.x,p2.y,p2.z);
	GameApi::P ct = ev.polygon_api.cone(numfaces, pp1, pp2, size, size);
	vec.push_back(ct);
	}
      GameApi::P res = ev.polygon_api.or_array2(vec);
      coll = find_facecoll(e,res);
    }
  }
  virtual int NumFaces() const { if (!coll) return 0; return coll->NumFaces(); }
  virtual int NumPoints(int face) const { if (!coll) return 3; return coll->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    if (!coll) return Point(0.0,0.0,0.0);
    return coll->FacePoint(face,point);
  }

  virtual bool has_normal() const { if (!coll) return false; return coll->has_normal(); }
  virtual bool has_attrib() const { if (!coll) return false;return coll->has_attrib(); }
  virtual bool has_color() const { if (!coll) return false; return coll->has_color(); }
  virtual bool has_texcoord() const { if (!coll) return false; return coll->has_texcoord(); }
  virtual bool has_skeleton() const { if (!coll) return false; return coll->has_skeleton(); }

  virtual Vector PointNormal(int face, int point) const
  {
    if (!coll) return Vector(0.0,0.0,0.0);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (!coll) return 0.0;

    return coll->Attrib(face,point,id);
  }

  virtual int AttribI(int face, int point, int id) const
  {
    if (!coll) return 0;
    return coll->AttribI(face,point,id);
  }

  virtual unsigned int Color(int face, int point) const
  {
    if (!coll) return 0x00000000;
    return coll->Color(face,point);
  }

  virtual Point2d TexCoord(int face, int point) const
  {
    if (!coll) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    return coll->TexCoord(face,point);
  }

  virtual float TexCoord3(int face, int point) const
  {
    if (!coll) { return 0.0; }
    return coll->TexCoord3(face,point);
  }

private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  LineCollection *lines;
  FaceCollection *coll;
  float size;
  int numfaces;
};


GameApi::ML GameApi::PolygonApi::line_to_cone3(EveryApi &ev, LI li, float size, int numfaces, MT mt, unsigned int color)
{
  GameApi::MS ms = ev.matrices_api.from_lines_3d(li);
  GameApi::PT pp1 = ev.point_api.point(0.0,0.0,0.0);
  GameApi::PT pp2 = ev.point_api.point(300.0,0.0,0.0);
  GameApi::P p = ev.polygon_api.cone(numfaces,pp1,pp2,size,size);
  GameApi::P p2 = ev.polygon_api.color(p,color);
  GameApi::ML ml = ev.materials_api.bind_inst_matrix(p2,ms,mt);
  return ml;
}

GameApi::ML GameApi::PolygonApi::line_to_cone2(EveryApi &ev, LI li, float size, int numfaces, MT mt)
{
  GameApi::MS ms = ev.matrices_api.from_lines_3d(li);
  GameApi::PT pp1 = ev.point_api.point(0.0,0.0,0.0);
  GameApi::PT pp2 = ev.point_api.point(300.0,0.0,0.0);
  GameApi::P p = ev.polygon_api.cone(numfaces,pp1,pp2,size,size);
  GameApi::ML ml = ev.materials_api.bind_inst_matrix(p,ms,mt);
  return ml;
}

GameApi::P GameApi::PolygonApi::line_to_cone(EveryApi &ev, LI li, float size, int numfaces)
{
  LineCollection *lines = find_line_array(e, li);
  return add_polygon2(e, new LineToCone(e,ev,lines,size,numfaces),1);
}
GameApi::P GameApi::PolygonApi::static_instancing_matrix(EveryApi &ev, P obj, MS matrix_array)
{
  MatrixArray *arr = find_matrix_array(e, matrix_array);
  int s = arr->Size();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Matrix m = arr->Index(i);
      M m2 = add_matrix2(e, m);
      P trans = matrix(obj, m2);
      vec.push_back(trans);
    }
  return or_array2(vec);
}
EXPORT GameApi::LI GameApi::PolygonApi::li_static_instancing_matrix(EveryApi &ev, LI obj, MS matrix_array)
{
  MatrixArray *arr = find_matrix_array(e, matrix_array);
  int s = arr->Size();
  std::vector<LI> vec;
  for(int i=0;i<s;i++)
    {
      Matrix m = arr->Index(i);
      M m2 = add_matrix2(e, m);
      LI trans = ev.lines_api.li_matrix(obj, m2);
      vec.push_back(trans);
    }
  return ev.lines_api.li_or_array(vec);
}

GameApi::P GameApi::PolygonApi::static_instancing(EveryApi &ev, P obj, PTS pos)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, pos);
  obj2->Prepare();
  int s = obj2->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point pp = obj2->Pos(i);
      P trans = translate(obj, pp.x,pp.y,pp.z);
      vec.push_back(trans);
    }
  return or_array2(vec);
}
GameApi::P GameApi::PolygonApi::static_instancing_vertex_color(EveryApi &ev, P obj, PTS pos)
{
  PointsApiPoints *obj2 = find_pointsapi_points(e, pos);
  obj2->Prepare();
  int s = obj2->NumPoints();
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      Point pp = obj2->Pos(i);
      P trans = translate(obj, pp.x,pp.y,pp.z);
      unsigned int c = obj2->Color(i);
      P color = PolygonApi::color(trans,c);
      vec.push_back(color);
    }
  return or_array2(vec);
}


GameApi::P GameApi::PolygonApi::static_instancing_with_color(EveryApi &ev, P obj, BM bm, float start_x, float end_x, float start_y, float end_y, float z)
{
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  b2->Prepare();
  int sx = b2->SizeX();
  int sy = b2->SizeY();
  
  //int s = obj2->NumPoints();
  std::vector<P> vec;
  for(int y=0;y<sy;y++)
    for(int x=0;x<sx;x++)
    {
      Color c = b2->Map(x,y);
      float xx = float(x)/sx;
      float yy = float(y)/sy;
      xx*=(end_x-start_x);
      yy*=(end_y-start_y);
      xx+=start_x;
      yy+=start_y;
      Point pp(xx,yy,z);
      P trans = translate(obj, pp.x,pp.y,pp.z);
      P cc = color(trans, c.Pixel());
      vec.push_back(cc);
    }
  return or_array2(vec);  
}

GameApi::P GameApi::PolygonApi::flip_normals(P obj)
{
  FaceCollection *obj2 = find_facecoll(e, obj);
  return add_polygon2(e, new FlipNormals(*obj2),1);
}
GameApi::ARR GameApi::PolygonApi::poly_array(std::vector<P> vec)
{
  ArrayType *arr = new ArrayType;
  arr->type = 0;
  int s = vec.size();
  for(int i=0;i<s;i++) arr->vec.push_back(vec[i].id);
  return add_array(e, arr);
}
int GameApi::PolygonApi::poly_size(ARR arr)
{
  ArrayType *t = find_array(e, arr);
  return t->vec.size();
}
GameApi::P GameApi::PolygonApi::poly_index(ARR arr, int idx)
{
  ArrayType *t = find_array(e, arr);
  int s = t->vec.size();
  if (idx>=0 && idx<s)
    {
      GameApi::P p;
      p.id = t->vec[idx];
      return p;
    }
  std::cout << "Array index error!" << std::endl;
  GameApi::P p = p_empty();
  return p;
}

GameApi::ARR GameApi::PolygonApi::poly_execute(EveryApi &ev, ARR arr, std::string gameapi_script)
{
  ArrayType *t = find_array(e, arr);
  int s = t->vec.size();
  ArrayType *t2 = new ArrayType;
  t2->type = 0;
  for(int i=0;i<s;i++)
    {
      ExecuteEnv env;
      env.envmap["E0"] = 0;
      env.env.push_back( t->vec[i] );
      std::pair<int, std::string> p = execute_codegen(e, ev, gameapi_script, env);
      t2->vec.push_back(p.first);
    }
  return add_array(e, t2);
}

GameApi::P GameApi::PolygonApi::disc(EveryApi &ev, int numfaces, float center_x, float center_y, float center_z, float normal_x, float normal_y, float normal_z, float radius)
{
  PT center = ev.point_api.point(center_x, center_y, center_z);
  V normal = ev.vector_api.vector(normal_x, normal_y, normal_z);
  V tiny_normal = ev.vector_api.mul(normal, 0.001);
  PT center_2 = ev.point_api.move(center, tiny_normal);
  return ev.polygon_api.cone(numfaces, center, center_2, radius, 0.0001);
}

class SphNormals : public ForwardFaceCollection
{
public:
  SphNormals(FaceCollection *coll, Point pt) : ForwardFaceCollection(*coll), coll(coll), pt(pt) { }
  virtual std::string name() const { return "SphNormals"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    Vector v = p - pt;
    v/=v.Dist();
    return v;
  }
private:
  FaceCollection *coll;
  Point pt;
};

GameApi::P GameApi::PolygonApi::spherical_normals(P p, float p_x, float p_y, float p_z)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SphNormals(coll, Point(p_x,p_y,p_z)),1);
}

std::string NumI(int val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}
std::string Num(float val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

// 3d position: gl_FragCoord
std::string dither_shader_string_v =
	"vec4 dither(vec4 pos)\n"
	"{\n"
	"   ex_Position = in_Position;\n"
	"   return pos;\n"
	"}\n";
std::string dither_shader_string_f =
	"vec4 dither(vec4 rgb)\n"
	"{\n"
	"   float v = gl_FragDepth;\n"
	"   return vec4(v,v,v,1.0);\n"
	"}\n";				     				 
GameApi::ML GameApi::PolygonApi::dither_shader(EveryApi &ev, ML mainloop)
{
  return custom_shader(ev,mainloop,dither_shader_string_v,dither_shader_string_f,"dither","dither");
}

std::string ConvF(float val)
{
  std::stringstream ss;
  ss << val;
  std::string s = ss.str();
  int sk = s.size();
  bool val2 = false;
  for(int i=0;i<sk;i++)
    {
      if (s[i]=='.') { val2=true; break; }
    }
  if (!val2) { s+=".0"; }
  return s;
}

GameApi::ML GameApi::PolygonApi::wave_shader(EveryApi &ev, ML mainloop, float radius, float t_mult, float x_mult, float y_mult)
{

  std::string r = ConvF(radius);
  std::string t = ConvF(t_mult);
  std::string x = ConvF(x_mult);
  std::string y = ConvF(y_mult);
  
std::string wave_v =
	"vec4 wave2(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
  std::string wave_f =
    "vec4 wave2(vec4 rgb)\n"
    "{\n"
    "  float r = " + r + ";\n"
    "  float t = " + t + "*time + ex_TexCoord.x*" + x +" + ex_TexCoord.y*" + y + ";\n"
    "  vec2 t_mx = ex_TexCoord.xy + vec2(r*cos(t),r*sin(t));\n"
    "  t_mx.x = clamp(t_mx.x,0.0,1.0);\n"
    "  t_mx.y = clamp(t_mx.y,0.0,1.0);\n"
    "   vec4 tex_mx = texture2D(tex, t_mx);	\n"
    "   return tex_mx;\n"
    "}\n";


  return custom_shader(ev, mainloop, wave_v, wave_f, "wave2", "wave2");

}

GameApi::ML GameApi::PolygonApi::blur_shader(EveryApi &ev, ML mainloop, float x_val, float y_val)
{
  //  std::stringstream ss; ss<<val;
  std::string x_ss = ConvF(x_val);
  std::string y_ss = ConvF(y_val);


std::string blur_v =
	"vec4 blur2(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
#if 0
  std::string blur_f =
	"vec4 blur2(vec4 rgb)\n"
	"{\n"
    "  vec2 t_mx = ex_TexCoord.xy + vec2(-" + ss + ",0.0);\n"
    "  vec2 t_my = ex_TexCoord.xy + vec2(0.0,-" + ss + ");\n"
    "  vec2 t_px = ex_TexCoord.xy + vec2("+ss+ ",0.0);\n"
    "  vec2 t_py = ex_TexCoord.xy + vec2(0.0," + ss + ");\n"
	"\n"
	"   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
	"   vec4 tex_mx = texture2D(tex, t_mx);	\n"
	"   vec4 tex_my = texture2D(tex, t_my);	\n"
	"   vec4 tex_px = texture2D(tex, t_px);	\n"
	"   vec4 tex_py = texture2D(tex, t_py);\n"
	"   vec3 t1 = mix(tex_mx.rgb, tex_px.rgb, 0.5);\n"
	"   vec3 t2 = mix(tex_my.rgb, tex_py.rgb, 0.5);\n"
	"   vec3 t12 = mix(t1,t2,0.5);\n"
	"   vec3 t12t = mix(t12,tex2.rgb,0.2);\n"   	
        "   float a1 = tex2.a;\n"
        "   float a_mx = tex_mx.a;\n"
        "   float a_my = tex_my.a;\n"
        "   float a_px = tex_px.a;\n"
        "   float a_py = tex_py.a;\n"
        "   float a = max(max(max(max(a1,a_mx),a_my),a_px), a_py);\n"
	"   return vec4(t12t,a);\n"
	"}\n";
#endif

  std::string blur_f =
    "   const int stepCount = 2;\n"
    "vec4 blur2(vec4 rgb)\n"
    "{\n"
    "     float gWeights[stepCount];\n"
    "    float gOffsets[stepCount];\n"
    "    gWeights[0] = 0.44908;\n"
    "    gWeights[1] = 0.05092;\n"
    "    gOffsets[0] = 0.53805;\n"
    "    gOffsets[1] = 2.06278;\n"
    "   vec2 pixelOffset = vec2(" + x_ss + "/800.0," + y_ss + "/600.0); \n"
    "   vec3 colOut = vec3(0,0,0);\n"
    "   float alphaOut = 0.0;\n"
    "    for( int i=0; i < stepCount; i++) {\n"
    "      vec2 texCoordOffset = gOffsets[i] * pixelOffset;\n"
    "      vec4 tex1 = texture2D( tex, ex_TexCoord.xy + texCoordOffset );\n"
    "      vec4 tex2 = texture2D( tex, ex_TexCoord.xy - texCoordOffset );\n"
    "      float alpha = tex1.a + tex2.a;\n"
    "      vec3 col = tex1.xyz + \n"
    "                 tex2.xyz; \n"
    "      colOut += gWeights[i] * col; \n"
    "      alphaOut += gWeights[i] * alpha;\n"
    "     }\n"
    "     return vec4(colOut,alphaOut);\n"
    "}\n";    

  return custom_shader(ev, mainloop, blur_v, blur_f, "blur2", "blur2");
}

 static std::string shader_unique_id()
 {
   static int id = 0;
   id++;
   std::stringstream ss;
   ss << id;
   return ss.str();
 }

 class TextureManySBM : public ShaderBitmap
 {
 public:
   TextureManySBM() { m_id = shader_unique_id(); }
   virtual std::string bitmapname() const { return "sbm" + m_id; }
   virtual std::string bitmapbody() const { 
     return "vec4 sbm" + m_id + "(vec3 pos)\n"
       "{\n"
       "  if (pos.z<0.7)\n"
       "  return texture2D(texsampler[0],pos.xy);\n"
       "  if (pos.z<1.7)\n"
       "  return texture2D(texsampler[1],pos.xy);\n"
       "  if (pos.z<2.7)\n"
       "  return texture2D(texsampler[2],pos.xy);\n"
       "  if (pos.z<3.7)\n"
       "  return texture2D(texsampler[3],pos.xy);\n"
       "  if (pos.z<4.7)\n"
       "  return texture2D(texsampler[4],pos.xy);\n"
       "  if (pos.z<5.7)\n"
       "  return texture2D(texsampler[5],pos.xy);\n"
       "  if (pos.z<6.7)\n"
       "  return texture2D(texsampler[6],pos.xy);\n"
       "  if (pos.z<7.7)\n"
       "  return texture2D(texsampler[7],pos.xy);\n"
       "  if (pos.z<8.7)\n"
       "  return texture2D(texsampler[8],pos.xy);\n"
       "  if (pos.z<9.7)\n"
       "  return texture2D(texsampler[9],pos.xy);\n"
       "  if (pos.z<10.7)\n"
       "  return texture2D(texsampler[10],pos.xy);\n"
       "  return vec4(0.0,0.0,0.0,0.0);\n"
       "}";
   }
   virtual std::string bitmapbody_v_init() const { return ""; }
   virtual std::string bitmapbody_v_body() const { return "ex_TexCoord = in_TexCoord;\nreturn pos;"; }
   virtual std::string bitmapbody_f_init() const { return ""; /*uniform sampler2D texsampler[16];\n";*/ }
   virtual std::string bitmapbody_f_body() const { return ""; }
 private:
   std::string m_id;
 };


GameApi::SBM GameApi::PolygonApi::texture_sbm()
{
  return add_shader_bitmap(e, new TextureManySBM);
}

class BloomCutSbm : public ShaderBitmap
{
public:
  BloomCutSbm(ShaderBitmap *tex, float r, float g, float b) : tex(tex),r(r), g(g), b(b) { m_id = shader_unique_id(); }
  virtual std::string bitmapname() const { return "bloom_cut" + m_id; }
  virtual std::string bitmapbody() const
  {
    std::string rr = ConvF(r);
    std::string gg = ConvF(g);
    std::string bb = ConvF(b);

    return tex->bitmapbody() +
      "vec4 bloom_cut" + m_id + "(vec3 pos)\n"
      "{\n"
      "   vec4 tex2 = " + tex->bitmapname() + "(pos);\n"
      "   if (tex2.r < " + rr + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   if (tex2.g < " + gg + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   if (tex2.b < " + bb + ") tex2 = vec4(0.0,0.0,0.0,0.0);\n"
      "   return tex2;\n"
      "}\n";
  }
  virtual std::string bitmapbody_v_init() const
  {
    return tex->bitmapbody_v_init() + "";
  }
  virtual std::string bitmapbody_v_body() const
  {
    return tex->bitmapbody_v_body() + "";
  }
  virtual std::string bitmapbody_f_init() const
  {
    return tex->bitmapbody_f_init() + "";
  }
  virtual std::string bitmapbody_f_body() const
  {
    return tex->bitmapbody_f_body() + "";
  }

private:
  ShaderBitmap *tex;
  std::string m_id;
  float r,g,b;
};

 GameApi::SBM GameApi::PolygonApi::bloom_cut_sbm(SBM texture, float r, float g, float b)
{
  ShaderBitmap *tex = find_shader_bitmap(e, texture);
  return add_shader_bitmap(e, new BloomCutSbm(tex,r,g,b));
}

class BlurSbm : public ShaderBitmap
{
public:
  BlurSbm(ShaderBitmap *tex, float pixel_x, float pixel_y) : tex(tex), pixel_x(pixel_x), pixel_y(pixel_y) { m_id = shader_unique_id(); }
  virtual std::string bitmapname() const { return "blur_sbm" + m_id; }
  virtual std::string bitmapbody() const
  {
    std::string x_ss = ConvF(pixel_x);
    std::string y_ss = ConvF(pixel_y);

    return tex->bitmapbody() + 
    "vec4 blur_sbm" + m_id + "(vec3 pos)\n"
    "{\n"
    "     float gWeights[stepCount" + m_id + "];\n"
    "    float gOffsets[stepCount" + m_id + "];\n"
    "    gWeights[0] = 0.383103;\n"
    "    gWeights[1] = 0.241843;\n"
    "    gWeights[2] = 0.060626;\n"
    "    gWeights[3] = 0.00598;\n"
    "    gOffsets[0] = 0.53805;\n"
    "    gOffsets[1] = 2.06278;\n"
    "    gOffsets[2] = 4.06278;\n"
    "    gOffsets[3] = 8.06278;\n"
    "   vec2 pixelOffset = vec2(" + x_ss + "," + y_ss + "); \n"
    "   vec3 colOut = vec3(0,0,0);\n"
    "   float alphaOut = 0.0;\n"
    "    for( int i=0; i < stepCount" + m_id + "; i++) {\n"
    "      vec2 texCoordOffset = gOffsets[i] * pixelOffset;\n"
      "      vec4 tex1 = " + tex->bitmapname() + "( vec3(pos.xy + texCoordOffset,pos.z) );\n"
      "      vec4 tex2 = " + tex->bitmapname() + "( vec3(pos.xy - texCoordOffset,pos.z) );\n"
    "      float alpha = tex1.a + tex2.a;\n"
    "      vec3 col = tex1.xyz + \n"
    "                 tex2.xyz; \n"
    "      colOut += gWeights[i] * col; \n"
    "      alphaOut += gWeights[i] * alpha;\n"
    "     }\n"
    "     return vec4(colOut,alphaOut);\n"
    "}\n";    

  }
  virtual std::string bitmapbody_v_init() const
  {
    return tex->bitmapbody_v_init() + "";
  }
  virtual std::string bitmapbody_v_body() const
  {
    return tex->bitmapbody_v_body() + "";
  }
  virtual std::string bitmapbody_f_init() const
  {
    return tex->bitmapbody_f_init() + "   const int stepCount" + m_id + " = 4;\n";
  }
  virtual std::string bitmapbody_f_body() const
  {
    return tex->bitmapbody_f_body() + "";
  }

private:
  ShaderBitmap *tex;
  float pixel_x, pixel_y;
  std::string m_id;
};

GameApi::SBM GameApi::PolygonApi::blur_sbm(SBM texture, float pixel_x, float pixel_y)
{
  ShaderBitmap *tex = find_shader_bitmap(e, texture);
  return add_shader_bitmap(e, new BlurSbm(tex, pixel_x, pixel_y));
}
 
class CombineSbm : public ShaderBitmap
{
public:
  CombineSbm(ShaderBitmap *sbm1, ShaderBitmap *sbm2) : sbm1(sbm1), sbm2(sbm2) { m_id=shader_unique_id(); }
  virtual std::string bitmapname() const { return "combine" + m_id; }
  virtual std::string bitmapbody() const
  {
    return sbm1->bitmapbody() + sbm2->bitmapbody() +
      "vec4 combine" + m_id + "(vec3 pos)\n"
      "{\n"
      "  vec4 tex1 = " + sbm1->bitmapname() + "(pos);\n"
      "  vec4 tex2 = " + sbm2->bitmapname() + "(pos);\n"
      "  return clamp(tex1+tex2,vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));\n"
      "}\n"
      ;
  }
  virtual std::string bitmapbody_v_init() const
  {
    return sbm1->bitmapbody_v_init() + sbm2->bitmapbody_v_init();
  }
  virtual std::string bitmapbody_v_body() const
  {
    return sbm1->bitmapbody_v_body() + sbm2->bitmapbody_v_body();
  }
  virtual std::string bitmapbody_f_init() const
  {
    return sbm1->bitmapbody_f_init() + sbm2->bitmapbody_f_init();
  }
  virtual std::string bitmapbody_f_body() const
  {
    return sbm1->bitmapbody_f_body() + sbm2->bitmapbody_f_body();
  }
private:
  ShaderBitmap *sbm1, *sbm2;
  std::string m_id;
};

GameApi::SBM GameApi::PolygonApi::combine_sbm(SBM texture1, SBM texture2)
{
  ShaderBitmap *tex1 = find_shader_bitmap(e, texture1);
  ShaderBitmap *tex2 = find_shader_bitmap(e, texture2);
  return add_shader_bitmap(e, new CombineSbm(tex1, tex2));
}
 std::string funccall_to_string(ShaderModule *mod);


class SBMCircle : public ShaderBitmap
{
public:
  SBMCircle(float start_time, float end_time, Point start_pos, Point end_pos, float start_radius, float end_radius) : start_time(start_time), end_time(end_time), start_pos(start_pos), end_pos(end_pos), start_radius(start_radius), end_radius(end_radius) { m_id = shader_unique_id(); }
  
  virtual std::string bitmapname() const { return "Circle" + m_id; }
  virtual std::string bitmapbody() const
  {
    std::stringstream start_time_t; start_time_t << start_time;
    std::stringstream end_time_t; end_time_t << end_time;
    
    std::stringstream start_pos_x; start_pos_x << start_pos.x;
    std::stringstream start_pos_y; start_pos_y << start_pos.y;
    std::stringstream start_pos_z; start_pos_z << start_pos.z;

    std::stringstream end_pos_x; end_pos_x << end_pos.x;
    std::stringstream end_pos_y; end_pos_y << end_pos.y;
    std::stringstream end_pos_z; end_pos_z << end_pos.z;
    
    std::stringstream start_radius_t; start_radius_t << start_radius;
    std::stringstream end_radius_t; end_radius_t << end_radius;
    
    std::string s;
    s+="vec4 Circle" + m_id + "(vec3 pos)\n";
    s+="{\n";
    s+="    pos.x*=600.0;\n";
    s+="    pos.y*=600.0;\n";
    s+="    pos.x-=300.0;\n";
    s+="    pos.y-=300.0;\n";
    s+="    float time_pos = (time-" + start_time_t.str() +")/(" + end_time_t.str() + "-" + start_time_t.str() + ");\n";
    s+="    if (time_pos <0.0) return vec4(0.0,0.0,0.0,0.0);\n";
    s+="    if (time_pos >1.0) return vec4(0.0,0.0,0.0,0.0);\n";
    s+="    float pos_x = time_pos*" + end_pos_x.str() + "+(1.0-time_pos)*" + start_pos_x.str() + ";\n";
    s+="    float pos_y = time_pos*" + end_pos_y.str() + "+(1.0-time_pos)*" + start_pos_y.str() + ";\n";
    s+="    float pos_z = time_pos*" + end_pos_z.str() + "+(1.0-time_pos)*" + start_pos_z.str() + ";\n";
    s+="    float radius = time_pos*" + end_radius_t.str() + "+(1.0-time_pos)*" + start_radius_t.str() + ";\n";
    s+="    pos.x-=pos_x;\n";
    s+="    pos.y-=pos_y;\n";
    s+="    pos.z-=pos_z;\n";
    s+="    float dist = length(pos);\n";
    s+="    if (dist<radius) return vec4(1.0,1.0,1.0,1.0);\n";
    s+="    return vec4(0.0,0.0,0.0,0.0);\n";
    s+="}\n";
    return s;
  }
  virtual std::string bitmapbody_v_init() const { return ""; }
  virtual std::string bitmapbody_v_body() const { return "ex_TexCoord = in_TexCoord;\n return pos;"; }
  virtual std::string bitmapbody_f_init() const { return ""; }
  virtual std::string bitmapbody_f_body() const { return ""; }
private:
  std::string m_id;
  float start_time, end_time;
  Point start_pos, end_pos;
  float start_radius, end_radius;
};

GameApi::SBM GameApi::PolygonApi::sbm_circle(float start_time, float end_time, float start_pos_x, float start_pos_y, float end_pos_x, float end_pos_y, float start_radius, float end_radius)
{
  return add_shader_bitmap(e, new SBMCircle(start_time, end_time, Point(start_pos_x, start_pos_y, 0.0), Point(end_pos_x, end_pos_y, 0.0), start_radius, end_radius));
}


class SFOSBM : public ShaderBitmap
{
public:
  SFOSBM(ShaderModule *mod) : mod(mod) { m_id = shader_unique_id(); }
  virtual std::string bitmapname() const { return "SFOSBM" + m_id; }
  virtual std::string bitmapbody() const {
    std::string s = mod->Function();
    s+="\n";
    s+="vec4 SFOSBM" + m_id + "(vec3 pos)\n";
    s+="{\n";    
    s+="   vec3 p0 = vec3(pos.x*600.0-300.0,pos.y*600.0-300.0,-400.0);\n";
    s+="   vec3 p1 = vec3(pos.x*600.0-300.0,pos.y*600.0-300.0,-399.0);\n";
    s+="vec4 rgb = ";
    s+=funccall_to_string(mod);
    s+=";\n";
    s+="return rgb;\n";
    s+="}\n";
    return s;
  }
  virtual std::string bitmapbody_v_init() const { return ""; }
  virtual std::string bitmapbody_v_body() const { return "ex_TexCoord = in_TexCoord;\n return pos;"; }
  virtual std::string bitmapbody_f_init() const { return ""; }
  virtual std::string bitmapbody_f_body() const { return ""; }
 
private:
  ShaderModule *mod;
  std::string m_id;
};
 
GameApi::SBM GameApi::PolygonApi::sfo_sbm(EveryApi &ev, SFO sfo)
{
  GameApi::SFO sfo2 = ev.sh_api.render(sfo);
  ShaderModule *mod = find_shader_module(e, sfo2);
  return add_shader_bitmap(e, new SFOSBM(mod));
}
 
GameApi::ML GameApi::PolygonApi::sbm_texture(EveryApi &ev, ML mainloop, SBM bitmap)
{
  ShaderBitmap *sbm = find_shader_bitmap(e, bitmap);
  std::string v = sbm->bitmapbody_v_init() +
    "vec4 sbmtex(vec4 pos)\n"
    "{\n" + sbm->bitmapbody_v_body() + " }\n";
  std::string f = sbm->bitmapbody_f_init() +
    sbm->bitmapbody() +
    "vec4 sbmtex(vec4 rgb) {\n"
    "  " + sbm->bitmapbody_f_body() + " "
    "  vec4 col = " + sbm->bitmapname() + "(ex_TexCoord.xyz);\n"
    "  return col;\n"
    "}\n";
  return custom_shader(ev, mainloop, v, f, "sbmtex", "sbmtex");
}

GameApi::ML GameApi::PolygonApi::bloom1_shader(EveryApi &ev, ML mainloop, float r_val, float g_val, float b_val)
{
  std::string ss_r = ConvF(r_val);
  std::string ss_g = ConvF(g_val);
  std::string ss_b = ConvF(b_val);

std::string bloom1_v =
	"vec4 bloom1(vec4 pos)\n"
	"{\n"
	"  ex_TexCoord = in_TexCoord;\n"
	"  return pos;\n"
	"}\n";
  
  std::string bloom1_f =
    "vec4 bloom1(vec4 rgb)\n"
    "{\n"
    "   vec4 tex2 = texture2D(tex, ex_TexCoord.xy);\n"
    "   vec3 v = vec3(" + ss_r + "," + ss_g + "," + ss_b + ");\n"
    "   float val = dot(tex2.rgb, v);\n"
    "   if (val <= 1.0) rgb = vec4(0,0,0,0.0);\n"
    "   return rgb;\n"
    "}\n";


  return custom_shader(ev, mainloop, bloom1_v, bloom1_f, "bloom1", "bloom1");
}

#if 0
class LogCoordsFaceCollection : public ForwardFaceCollection
{
public:
  LogCoordsFaceCollection(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    SphericalPoint sp(Point(0.0,0.0,0.0));
    sp.FromPoint(p);
    sp.r = log(sp.r);
    Point p2 = sp.ToPoint();
    return p2;
  }
};
#endif

GameApi::P GameApi::PolygonApi::log_coords(P p)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new LogCoordsFaceCollection(coll),1);
#endif
}

GameApi::P GameApi::PolygonApi::log_coords2(P p, int x_count, int y_count, float sx, float sy, float sz)
{
  P p1 = splitquads(p, x_count,y_count);
  P p2 = log_coords(p1);
  P p3 = scale(p2,sx,sy,sz);
  return p3;
}

class SphericalWave : public ForwardFaceCollection
{
public:
  SphericalWave(FaceCollection *coll, float r1, float fr_1, float r2, float fr_2) : ForwardFaceCollection(*coll), r1(r1), r2(r2), fr_1(fr_1), fr_2(fr_2), coll(coll) { }
  virtual std::string name() const { return "SphericalWave"; }

  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  Point FacePoint(int face, int point) const
  {
    Point p = ForwardFaceCollection::FacePoint(face,point);
    SphericalPoint sp(Point(0.0,0.0,0.0));
    sp.FromPoint(p);
    sp.r = sp.r + r1*cos(fr_1*sp.alfa) + r2*cos(fr_2*sp.beta);
    Point p2 = sp.ToPoint();
    return p2;
  }
private:
  FaceCollection *coll;
  float r1,r2;
  float fr_1, fr_2;
};

GameApi::P GameApi::PolygonApi::spherical_wave(P p, float r1, float freq_1, float r2, float freq_2)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SphericalWave(coll, r1, freq_1, r2, freq_2), 1);
}

class MainLoopPosition : public MainLoopItem
{
public:
  MainLoopPosition(MainLoopItem *next) : next(next) { }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { } // not called
  void Prepare() {next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    OpenglLowApi *ogl = g_low->ogl;
    int viewport[4];
    ogl->glGetIntegerv(Low_GL_VIEWPORT, viewport);
    
    MainLoopEnv ee = e;
    ee.in_MV = e.in_MV * Matrix::Translate(viewport[0], viewport[1], 0.0);
    ee.env = e.env * Matrix::Translate(viewport[0], viewport[1], 0.0);
    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};
 
GameApi::ML GameApi::PolygonApi::position_based_on_screen(ML obj)
{
  MainLoopItem *next = find_main_loop(e, obj);
  return add_main_loop(e, new MainLoopPosition(next));
}

class PlaneMap2 : public SurfaceIn3d
{
public:
  PlaneMap2(Bitmap<float> &fb, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float start_values, float end_values) : fb(fb), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), start_values(start_values), end_values(end_values) {
  }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare() { } // not called

  virtual void Prepare() { fb.Prepare(); }
  virtual Point Index(float x, float y) const
  {
    float cx = x*fb.SizeX();
    float cy = y*fb.SizeY();
    float zval = fb.Map(int(cx),int(cy));
    zval-=start_values;
    zval/=(end_values-start_values);
    zval*=(end_z-start_z);
    zval+=start_z;

    float xval = start_x+x*(end_x-start_x);
    float yval = start_y+y*(end_y-start_y);
    Point p(xval,yval,zval);
    return p;
  }
  virtual Point2d Texture(float x, float y) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
  virtual float Attrib(float x, float y, int id) const { return 0.0; }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float x, float y) const
  {
    Vector v;
    v.dx = 0.0;
    v.dy = 0.0;
    v.dz = end_z-start_z;
    return v;
  }
  virtual unsigned int Color(float x, float y) const { return 0xffffffff; }
  virtual float XSize() const { return 1.0; }
  virtual float YSize() const { return 1.0; }
private:
  Bitmap<float> &fb;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  float start_values, end_values;
};

class SphereMap2 : public SurfaceIn3d
{
public:
  SphereMap2(float p_x, float p_y, float p_z, Bitmap<float> &fb, float start_radius, float end_radius, float start_values, float end_values) : p_x(p_x), p_y(p_y), p_z(p_z), fb(fb), start_radius(start_radius), end_radius(end_radius), start_values(start_values), end_values(end_values) { }
  void Collect(CollectVisitor &vis)
  {
    fb.Collect(vis);
  }
  void HeavyPrepare() { } // not called

  virtual void Prepare() { fb.Prepare(); }
  virtual Point Index(float x, float y) const
  {
    float cx = x*(fb.SizeX()-1.0);
    float cy = y*(fb.SizeY()-1.0);
    float r = fb.Map(int(cx),int(cy));
    r-=start_values;
    r/=(end_values-start_values);
    r*=(end_radius-start_radius);
    r+=start_radius;
    float angle1 = x*3.14159265;
    float angle2 = y*3.14159265*2.0;
    
    Point p;
    p.x = p_x + r*sin(angle1)*cos(angle2);
    p.y = p_y + r*sin(angle1)*sin(angle2);
    p.z = p_z + r*cos(angle1);
    return p;
  }
  virtual Point2d Texture(float x, float y) const
  {
    Point2d p;
    p.x = x;
    p.y = y;
    return p;
  }
  virtual float Attrib(float x, float y, int id) const { return 0.0; }
  virtual int AttribI(float x, float y, int id) const { return 0; }
  virtual Vector Normal(float x, float y) const
  {
    Point center(p_x,p_y,p_z);
    Point p = Index(x,y);
    Vector v = p - center;
    v/=v.Dist();
    return v;
  }
  virtual unsigned int Color(float x, float y) const { return 0xffffffff; }
  virtual float XSize() const { return 1.0; }
  virtual float YSize() const { return 1.0; }
private:
  float p_x,p_y,p_z;
  Bitmap<float> &fb;
  float start_radius, end_radius;
  float start_values, end_values;
};

GameApi::S GameApi::PolygonApi::s_spherical(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values)
{
  Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
  SurfaceIn3d *surf = new SphereMap2(c_x,c_y,c_z, *fb2, start_radius, end_radius, start_values, end_values);
  SurfaceImpl impl;
  impl.surf = surf;
  return add_surface(e, impl);
}

GameApi::P GameApi::PolygonApi::s_sample(S surf, int sx, int sy)
{
  SurfaceImpl *surf2 = find_surface(e, surf);
  SurfaceIn3d *surf3 = surf2->surf;
  return add_polygon2(e, new SampleSurfaceIn3d(*surf3, 0, sx,sy),1);
}

GameApi::P GameApi::PolygonApi::sphere_map(float c_x, float c_y, float c_z, FB fb, float start_radius, float end_radius, float start_values, float end_values, int sx, int sy)
{
  Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
  SurfaceIn3d *surf = new SphereMap2(c_x, c_y, c_z, *fb2, start_radius, end_radius, start_values, end_values);
  ::EnvImpl *env = ::EnvImpl::Environment(&e);
  env->deletes.push_back(std::shared_ptr<void>(surf));
  FaceCollection *coll = new SampleSurfaceIn3d(*surf, 0, sx,sy);
  return add_polygon2(e, coll, 1);
}

 GameApi::P GameApi::PolygonApi::plane_map(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float start_values, float end_values, FB fb, int sx, int sy)
 {
   Bitmap<float> *fb2 = find_float_bitmap(e, fb)->bitmap;
   SurfaceIn3d *surf = new PlaneMap2(*fb2, start_x, end_x, start_y, end_y, start_z, end_z, start_values, end_values);
   ::EnvImpl *env = ::EnvImpl::Environment(&e);
   env->deletes.push_back(std::shared_ptr<void>(surf));
   FaceCollection *coll = new SampleSurfaceIn3d(*surf, 0, sx,sy);
   return add_polygon2(e, coll, 1);
 }

class CurveToPoly : public FaceCollection
{
public:
  CurveToPoly(Curve<Point> *curve, float start_x, float end_x, float start_y, float end_y, float start_angle, float end_angle, int numinstances) : curve(curve), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_angle(start_angle), end_angle(end_angle), numinstances(numinstances) {}
  virtual std::string name() const { return "CurveToPoly"; }

  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { } // not called

  void Prepare() { }
  int NumFaces() const { return numinstances; }
  int NumPoints(int face) const { return 4; }
  Point FacePoint(int face, int point) const
  {
    float p = float(face)/numinstances;
    Point2d p11 = { start_x, start_y };
    Point2d p21 = { end_x, start_y };
    Point2d p12 = { start_x, end_y };
    Point2d p22 = { end_x, end_y };
    float angle = start_angle+p*(end_angle-start_angle);
    Point2d rot_p11 = { float(cos(angle)*p11.x) -float(sin(angle)*p11.y), float(sin(angle)*p11.x)+float(cos(angle)*p11.y) };
    Point2d rot_p12 = { float(cos(angle)*p12.x -sin(angle)*p12.y), float(sin(angle)*p12.x+cos(angle)*p12.y) };
    Point2d rot_p21 = { float(cos(angle)*p21.x -sin(angle)*p21.y), float(sin(angle)*p21.x+cos(angle)*p21.y) };
    Point2d rot_p22 = { float(cos(angle)*p22.x -sin(angle)*p22.y), float(sin(angle)*p22.x+cos(angle)*p22.y) };
    Point curve_pos = curve->Index(p*curve->Size());
    switch(point) {
    case 0: return curve_pos+Vector(rot_p11.x, rot_p11.y, 0.0);
    case 1: return curve_pos+Vector(rot_p12.x, rot_p12.y, 0.0);
    case 2: return curve_pos+Vector(rot_p22.x, rot_p22.y, 0.0);
    case 3: return curve_pos+Vector(rot_p21.x, rot_p21.y, 0.0);
    };
    return curve_pos;
  }
  Vector PointNormal(int face, int point) const { return Point(0.0,0.0,1.0); }
  float Attrib(int face, int point, int id) const { return 0.0; }
  int AttribI(int face, int point, int id) const { return 0; }
  unsigned int Color(int face, int point) const { return 0xffffffff; }
  Point2d TexCoord(int face, int point) const { Point2d p; p.x = 0.0; p.y = 0.0; return p; }
  float TexCoord3(int face, int point) const { return 0.0; }
private:
  Curve<Point> *curve;
  float start_x, end_x;
  float start_y, end_y;
  float start_angle, end_angle;
  int numinstances;
};

GameApi::P GameApi::PolygonApi::curve_to_poly(C c, float start_x, float end_x, float start_y, float end_y, float start_angle, float end_angle, int numinstances)
{
  Curve<Point> *curve = find_curve(e, c);
  return add_polygon2(e, new CurveToPoly(curve, start_x, end_x, start_y, end_y, start_angle, end_angle, numinstances),1);
}


struct DSVertexHeader
{
  int numfaces;
  int numobjects;
  int flags; 
  int reserved_2=-1;
  int reserved_3=-1;
  int reserved_4=-1;
  int reserved_5=-1;
};
struct PP
{
  int first;
  int second;
  int reserved_1=-1;
  int reserved_2=-1;
};

struct DSGLTFHeader
{
  char base_url[255];
  char url[255];
  int default_scene;
  int num_accessors;
  int num_animations;
  int num_buffers;
  int num_bufferviews;
  int num_materials;
  int num_meshes;
  int num_nodes;
  int num_textures;
  int num_images;
  int num_skins;
  int num_samplers;
  int num_cameras;
  int num_scenes;
  int num_lights;
};

class DSGLTF : public GLTFModelInterface
{
public:
  DSGLTF(DiskStore *ds) : ds(ds) { ready = false; }
  virtual std::string name() const { return "DSGLTF"; }
  void Collect(CollectVisitor &vis)
  {
    ds->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { ready=true; }
  void Prepare() { ds->Prepare(); ready=true; }

  std::string BaseUrl() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return std::string(head->base_url);    
  }
  std::string Url() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return std::string(head->url);    
  }
  unsigned char *get_header() const
  {
    if (!ready) return 0;
    int num = find_block(0); // header
    if (num<0) return 0;
    unsigned char *ptr = ds->Block(num);
    return ptr;
  }
  int find_block(int id) const
  {
    //if (id==block_cache_id) return block_cache;
    
    int s = ds->NumBlocks();
    //std::cout << "FINDBLOCK:" << id << " " << s << std::endl;
    static bool once=false;
    if (s==0&&!once) { std::cout << "DSFaceCollection failed for NumBlocks==0 which means that file is not valid" << std::endl;
      once=true;
      // emscripten_run_script("jsStackTrace()");
    }
    int block =  ds->BlockTypeInv(id);
    if (block != -1) return block;
    return -1;
  }

  
  int get_default_scene() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->default_scene;
  }
  //
  // Sizes
  // 
  int accessors_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_accessors;

  }
  int animations_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_animations;
  }
  int buffers_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_buffers;
  }
  int bufferviews_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_bufferviews;
  }
  int materials_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_materials;
  }
  int meshes_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_meshes;
  }
  int nodes_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_nodes;
  }
  int textures_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_textures;
  }
  int images_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_images;
  }
  int skins_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_skins;
  }
  int samplers_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_samplers;
  }
  int cameras_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
     DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_cameras;
 }
  int scenes_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_scenes;
  }
  int lights_size() const {
    unsigned char *ptr = get_header();
    if (!ptr) return 0;
    DSGLTFHeader *head = (DSGLTFHeader*)ptr;
    return head->num_lights;
  }

  unsigned char *get_ptr(int block) const
  {
    if (!ready) return 0;
    int num = find_block(block);
    if (num<0) return 0;
    unsigned char *ptr = ds->Block(num);
    return ptr;
  }



  //
  // Accessors
  //
  virtual const tinygltf::Accessor &get_accessor(int i) const {
    unsigned char *ptr = get_ptr(1);
    tinygltf::Accessor *array = (tinygltf::Accessor*)ptr;
    return array[i];
  }
  virtual const tinygltf::Animation &get_animation(int i) const {
    unsigned char *ptr = get_ptr(2);
    tinygltf::Animation *array = (tinygltf::Animation*)ptr;
    return array[i];
  }
  virtual const tinygltf::Buffer &get_buffer(int i) const {
    unsigned char *ptr = get_ptr(3);
    tinygltf::Buffer *array = (tinygltf::Buffer*)ptr;
    return array[i];
  }
  virtual const tinygltf::BufferView &get_bufferview(int i) const {
    unsigned char *ptr = get_ptr(4);
    tinygltf::BufferView *array = (tinygltf::BufferView*)ptr;
    return array[i];
  }
  virtual const tinygltf::Material &get_material(int i) const {
    unsigned char *ptr = get_ptr(5);
    tinygltf::Material *array = (tinygltf::Material*)ptr;
    return array[i];
  }
  virtual const tinygltf::Mesh &get_mesh(int i) const {
    unsigned char *ptr = get_ptr(6);
    tinygltf::Mesh *array = (tinygltf::Mesh*)ptr;
    return array[i];
  }
  virtual const tinygltf::Node &get_node(int i) const {
    unsigned char *ptr = get_ptr(7);
    tinygltf::Node *array = (tinygltf::Node*)ptr;
    return array[i];
  }
  virtual const tinygltf::Texture &get_texture(int i) const {
    unsigned char *ptr = get_ptr(8);
    tinygltf::Texture *array = (tinygltf::Texture*)ptr;
    return array[i];
  }
  virtual const tinygltf::Image &get_image(int i) const {
    unsigned char *ptr = get_ptr(9);
    tinygltf::Image *array = (tinygltf::Image*)ptr;
    return array[i];
  }
  virtual const tinygltf::Skin &get_skin(int i) const {
    unsigned char *ptr = get_ptr(10);
    tinygltf::Skin *array = (tinygltf::Skin*)ptr;
    return array[i];
  }
  virtual const tinygltf::Sampler &get_sampler(int i) const {
    unsigned char *ptr = get_ptr(11);
    tinygltf::Sampler *array = (tinygltf::Sampler*)ptr;
    return array[i];
  }
  virtual const tinygltf::Camera &get_camera(int i) const {
    unsigned char *ptr = get_ptr(12);
    tinygltf::Camera *array = (tinygltf::Camera*)ptr;
    return array[i];
  }
  virtual const tinygltf::Scene &get_scene(int i) const {
    unsigned char *ptr = get_ptr(13);
    tinygltf::Scene *array = (tinygltf::Scene*)ptr;
    return array[i];
  }
  virtual const tinygltf::Light &get_light(int i) const {  
    unsigned char *ptr = get_ptr(14);
    tinygltf::Light *array = (tinygltf::Light*)ptr;
    return array[i];
  }

  //
  // Second level accessors
  //
  int accessor_name_size(int a) const {
    // 15
    unsigned char *ptr = get_ptr(15);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  unsigned char accessor_name_char(int a, int ch) const {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_name_size(i);
    unsigned char *ptr = get_ptr(16);
    ptr+=pos;
    return ptr[ch];
  }
  int accessor_extensions_size(int a) const {
    unsigned char *ptr = get_ptr(17);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  int accessor_extensions_size2(int a) const {
    //unsigned char *ptr = get_ptr(18);
    //unsigned int *ptr2 = (unsigned int*)ptr;
    //return ptr2[a];
    return sizeof(tinygltf::Value);
  }
  std::string accessor_extensions_key(int a, int ext) const
  {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_extensions_size(i);
    unsigned char *ptr = get_ptr(18);
    ptr+=pos;
    return std::string(&ptr[ext],&ptr[ext+accessor_name_size(a)]);
  }
  tinygltf::Value accessor_extensions_value(int a, int ext) const
  {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_extensions_size2(i);
    unsigned char *ptr = get_ptr(19);
    ptr+=pos;
    tinygltf::Value val;
    std::copy(&ptr[ext],&ptr[ext+sizeof(tinygltf::Value)],(unsigned char *)&val);
    //fill_heap_for_tinygltfValue(val,third_level+0);
    return val;
  }
  int accessor_extras_size(int a) const {
    unsigned char *ptr = get_ptr(20);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  unsigned char accessor_extras_char(int a, int ch) const {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_extras_size(i);
    unsigned char *ptr = get_ptr(16);
    ptr+=pos;
    return ptr[ch];
  }
  int accessor_extensions2_size(int a) const {
    unsigned char *ptr = get_ptr(20);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  unsigned char accessor_externsions2_char(int a, int ch) const {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_extensions2_size(i);
    unsigned char *ptr = get_ptr(16);
    ptr+=pos;
    return ptr[ch];
  }
  int accessor_minvalues_size(int a) const {
    unsigned char *ptr = get_ptr(20);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  double accessor_minvalues_get(int a, int val) const
  {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_minvalues_size(i);
    unsigned char *ptr = get_ptr(16);
    ptr+=pos;
    double *ptr2 = (double*)ptr;
    return ptr2[val];
  }
  int accessor_maxvalues_size(int a) const
  {
    unsigned char *ptr = get_ptr(20);
    unsigned int *ptr2 = (unsigned int*)ptr;
    return ptr2[a];
  }
  double accessor_maxvalues_get(int a, int val) const
  {
    int s = a;
    int pos = 0;
    for(int i=0;i<s;i++)
      pos+=accessor_maxvalues_size(i);
    unsigned char *ptr = get_ptr(16);
    ptr+=pos;
    double *ptr2 = (double*)ptr;
    return ptr2[val];
  }
  //
  // Second level animations
  // 
  int animation_name_size(int a) const
  {
  }
  unsigned char animation_name_get(int a, int ch) const
  {
  }
  int animation_channel_size(int a) const
  {
  }
  tinygltf::AnimationChannel animation_channel_get(int a, int chan) const
  {
  }
  int animation_sampler_size(int a) const
  {
  }
  tinygltf::AnimationSampler animation_sampler_get(int a, int chan) const
  {
  }
  //
  // Second level buffer
  //
  int buffer_name_size(int b) const
  {
  }
  unsigned char buffer_name_get(int b, int ch) const
  {
  }
  int buffer_data_size(int b) const
  {
  }
  unsigned char buffer_data_get(int b, int ch) const
  {
  }
  int buffer_uri_size(int b) const
  {
  }
  unsigned char buffer_uri_get(int b, int ch) const
  {
  }
  //
  // BufferViews second level
  //
  int bufferviews_name_size(int bv) const
  {
  }
  unsigned char bufferviews_name_get(int bv, int ch) const
  {
  }
  //
  // Material 2nd level
  //
  int material_name_size(int bv) const
  {
  }
  unsigned char material_name_get(int bv, int ch) const
  {
  }
  double material_emissivefactor(int bv, int zero_to_two) const
  {
  }
  int material_alphamode_size(int bv) const
  {
  }
  unsigned char material_alphamode_get(int bv, int ch) const
  {
  }
  tinygltf::PbrMetallicRoughness material_pbr(int bv) const
  {
  }
  tinygltf::NormalTextureInfo material_normaltexture(int bv) const
  {
  }
  tinygltf::OcclusionTextureInfo material_occlusiontexture(int bv) const
  {
  }
  tinygltf::TextureInfo material_textureinfo(int bv) const
  {
  }
  //
  // Meshes 2nd level
  //
  int mesh_name_size(int mesh) const
  {
  }
  unsigned char mesh_name_get(int mesh, int ch) const
  {
  }
  int mesh_primitives_size(int mesh) const
  {
  }
  tinygltf::Primitive mesh_primitives_get(int mesh, int prim) const
  {
  }
  int mesh_weights_size(int mesh) const
  {
  }
  double mesh_weights_get(int mesh, int index) const
  {
  }
  //
  // Nodes 2nd level
  //
  int node_name_size(int n) const
  {
  }
  unsigned char node_name_get(int n, int ch) const
  {
  }
  int node_children_size(int n) const
  {
  }
  int node_children_get(int n, int child) const
  {
  }
  int node_rotation_size(int n) const
  {
  }
  double node_rotation_get(int n, int i) const
  {
  }
  int node_scale_size(int n) const
  {
  }
  double node_scale_get(int n, int i) const
  {
  }
  int node_translation_size(int n) const
  {
  }
  double node_translation_get(int n, int i) const
  {
  }
  int node_matrix_size(int n) const
  {
  }
  double node_matrix_get(int n, int i) const
  {
  }
  //
  // Textures, 2nd level
  //
  int texture_name_size(int t) const
  {
  }
  unsigned char texture_name_get(int i, int ch) const
  {
  }
  //
  // Images, 2nd level
  //
  int image_name_size(int i) const
  {
  }
  unsigned char image_name_get(int i, int ch) const
  {
  }
  int image_image_size(int i) const
  {
  }
  unsigned char image_image_get(int i, int ch) const
  {
  }
  int image_mimeType_size(int i) const
  {
  }
  unsigned char image_mimeType_get(int i, int ch) const
  {
  }
  int image_uri_size(int i) const
  {
  }
  unsigned char image_uri_get(int i, int ch) const
  {
  }
  //
  // Skins 2nd level
  //
  int skin_name_size(int i) const
  {
  }
  unsigned char skin_name_get(int i, int ch) const
  {
  }
  int skin_joints_size(int i) const
  {
  }
  int skin_joints_get(int i, int j) const
  {
  }
  //
  // Samplers, 2nd level
  //
  int sampler_name_size(int s) const
  {
  }
  unsigned char sampler_name_get(int s, int ch) const
  {
  }
  //
  // Cameras 2nd level
  //
  int camera_type_size(int c) const
  {
  }
  unsigned char camera_type_get(int c, int ch) const
  {
  }
  int camera_name_size(int c) const
  {
  }
  unsigned char camera_name_get(int c, int ch) const
  {
  }
  tinygltf::PerspectiveCamera camera_perspective(int c) const
  {
  }
  tinygltf::OrthographicCamera camera_ortho(int c) const
  {
  }
  //
  // Scenes 2nd level
  //
  int scenes_name_size(int s) const
  {
  }
  unsigned char scenes_name_get(int s, int ch) const
  {
  }
  int scenes_nodes_size(int s) const
  {
  }
  int scenes_nodes_get(int s, int node) const
  {
  }
  //
  // Lights 2nd level
  //
  int light_name_size(int l) const
  {
  }
  unsigned char light_name_get(int l, int ch) const
  {
  }
  int light_color_size(int l) const
  {
  }
  double light_color_get(int l, int component) const
  {
  }
  int light_type_size(int l) const
  {
  }
  unsigned char light_type_get(int l, int ch) const
  {
  }
  tinygltf::SpotLight light_spot(int i) const
  {
  }
  //
  // 3rd level
  //
  void fill_heap_for_tinygltfValue(tinygltf::Value &val, int block)
  {
  }
private:
  DiskStore *ds;
  bool ready;
  int third_level=-1; // TODO
};

class DSFaceCollection : public FaceCollection
{
public:
  DSFaceCollection(DiskStore *ds) :ds(ds) { ready=false; }
  virtual std::string name() const { return "DSFaceCollection"; }

  void Collect(CollectVisitor &vis)
  {
    ds->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() { ready=true; }
  void Prepare() { ds->Prepare(); ready=true; }
  int NumFaces() const {
    if (!ready) return 0;
    int num = find_block(6); // header
    if (num<0) return 0;
    unsigned char *ptr = ds->Block(num);
    DSVertexHeader *head = (DSVertexHeader*)ptr;
    return head->numfaces;
  }
  bool HasFlags() const { return GetFlags()!=-1; }
  int GetFlags() const {
    if (!ready) return -1;
    int num = find_block(6); // header
    if (num<0) return -1;
    unsigned char *ptr = ds->Block(num);
    DSVertexHeader *head = (DSVertexHeader*)ptr;
    return head->flags;
  }
  int NumPoints(int face) const
  {
    if (!ready) return 0;
    int num = find_block(7);
    if (num<0) return 0;
    unsigned char *ptr = ds->Block(num);
    int *array = (int*)ptr;
    return array[face];
  }
  Point FacePoint(int face, int point) const {
    if (!ready) return Point(0.0,0.0,0.0);
    int num = find_block(0);
    if (num<0) return Point(0.0,0.0,0.0);
    unsigned char *ptr = ds->Block(num);
    Point *array = (Point*)ptr;
    int pos = vertex_index(face,point);
    return array[pos];
  }
  bool has_normal() const {
    if (HasFlags() && (GetFlags() & DSDisableNormal)) return false; 
    return true;
  }
  Vector PointNormal(int face, int point) const
  {
    if (!ready) return Vector(0.0,0.0,0.0);
    if (HasFlags() && GetFlags()&DSDisableNormal)
      return Vector(0.0,0.0,0.0);
    int num = find_block(3);
    if (num<0) return Vector(0.0,0.0,0.0);
    unsigned char *ptr = ds->Block(num);
    Vector *array = (Vector*)ptr;
    int pos = vertex_index(face,point);
    return array[pos];
  }
  float Attrib(int face, int point, int id) const { return 0.0; }
  int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const {
    if (HasFlags() && (GetFlags() & DSDisableColor)) return false; 
    return true; }
  unsigned int Color(int face, int point) const
  {
    if (!ready) return 0xff000000;
    if (HasFlags() && GetFlags()&DSDisableColor)
      return 0xff000000;
    int num = find_block(1);
    if (num<0) return 0xff000000;
    unsigned char *ptr = ds->Block(num);
    unsigned int *array = (unsigned int*)ptr;
    int pos = vertex_index(face,point);
    return array[pos];
  }
  bool has_texcoord() const {
    if (HasFlags() && (GetFlags() & DSDisableTexCoord)) return false; 
    return true; }
  Point2d TexCoord(int face, int point) const
  {
    if (!ready) { Point2d p; p.x=0.0; p.y =0.0; return p; }
    if (HasFlags() && GetFlags()&DSDisableTexCoord) {
      Point2d p; p.x=0.0; p.y =0.0; return p;
    }
    int num = find_block(2);
    if (num<0) { Point2d p; p.x=0.0; p.y=0.0; return p; }
    unsigned char *ptr = ds->Block(num);
    Point2d *array = (Point2d*)ptr;
    int pos = vertex_index(face,point);
    return array[pos];
  }
  float TexCoord3(int face, int point) const
  {
    if (!ready) return 0.0;
    if (HasFlags() && GetFlags()&DSDisableTexCoord3) {
      return 0.0;
    }
    int num = find_block(9);
    if (num<0) return 0.0;
    unsigned char *ptr = ds->Block(num);
    float *array = (float*)ptr;
    int pos = vertex_index(face,point);
    return array[pos];
  }
  int NumObjects() const {
    if (!ready) return 0;
    if (HasFlags() && GetFlags()&DSDisableObjects) { return 0; }

    int num = find_block(6); // header
    if (num<0) return 0;
    unsigned char *ptr = ds->Block(num);
    DSVertexHeader *head = (DSVertexHeader*)ptr;
    //std::cout << "NUMOBJECTS:" << head->numobjects << std::endl;
    return head->numobjects;

  }
  std::pair<int,int> GetObject(int o) const {
    if (!ready) return std::make_pair(0,0);
    if (HasFlags() && GetFlags()&DSDisableObjects) { return std::make_pair(0,0); }
    int num = find_block(10);
    if (num<0) return std::make_pair(0,0);
    unsigned char *ptr = ds->Block(num);
    PP *array = (PP*)ptr;
    PP obj = array[o]; 
    //std::cout << "DSPAIR:" << obj.first << " " << obj.second << std::endl;
    return std::make_pair(obj.first,obj.second);
  }
  int find_block(int id) const
  {
    //if (id==block_cache_id) return block_cache;
    
    int s = ds->NumBlocks();
    //std::cout << "FINDBLOCK:" << id << " " << s << std::endl;
    static bool once=false;
    if (s==0&&!once) { std::cout << "DSFaceCollection failed for NumBlocks==0 which means that file is not valid" << std::endl;
      once=true;
      // emscripten_run_script("jsStackTrace()");
    }
    int block =  ds->BlockTypeInv(id);
    if (block != -1) return block;
#if 0
    /*
    for(int i=0;i<s;i++) {
      //std::cout << "FB:" << i << " " << id << " " << ds->BlockType(i) << std::endl;
      if (ds->BlockType(i)==id) { /*block_cache_id=id; block_cache=i;*/ return i;}
    }
  */
#endif
    static bool once2 = false;
    if (!once2) { std::cout << "DSFaceCollection: couldnt find block " << id << std::endl; once2=true; 
    for(int i=0;i<s;i++) { std::cout << "Block type: " << ds->BlockType(i) << std::endl; }
    }
    //block_cache_id=id;
    //block_cache=-1;
    return -1;
  }
  int vertex_index(int face, int point) const
  {
    int num = find_block(8);
    unsigned char *ptr = ds->Block(num);
    int *array = (int*)ptr;
    return array[face]+point;
  }
private:
  DiskStore *ds;
  bool ready;
  mutable int block_cache_id=-2;
  mutable int block_cache=-2;  
};

GameApi::P GameApi::PolygonApi::p_ds2(EveryApi &ev, DS ds)
{
  DiskStore *dds = find_disk_store(e, ds);  
  return add_polygon2(e, new DSFaceCollection(dds),1);
}

GameApi::P GameApi::PolygonApi::p_ds(EveryApi &ev, const unsigned char *buf, const unsigned char *end)
{
  DS ds = ev.mainloop_api.load_ds_from_mem(buf,end);
  DiskStore *dds = find_disk_store(e, ds);
  return add_polygon2(e, new DSFaceCollection(dds),1);
}


class DiskStoreCollection : public DiskStore
{
public:
  DiskStoreCollection(FaceCollection *coll, int flags=-1) : coll(coll),flags(flags) { if (flags==-1) flags=0; }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    int s = coll->NumFaces();
    header.numfaces=s;
    int s3 = coll->NumObjects();
    header.numobjects = s3;
    header.flags = flags;
    int accum = 0;
    InstallProgress(111,"Fetch points", 15);
    pointcounts.reserve(s);
    vertexindex.reserve(s);
    for(int i=0;i<s;i++) {
      if (s/15>0 && i%(s/15)==0) ProgressBar(111,i*15/s,15,"Fetch points"); 
      int ss = coll->NumPoints(i);
      pointcounts.push_back(ss);
      vertexindex.push_back(accum);
      accum +=ss;
    }
    InstallProgress(112,"Fetch vertex arrays", 15);
    vertex.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableNormal))
      normal.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableColor))
      color.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableTexCoord))
      texcoord.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableTexCoord3))
      texcoord3.reserve(s*coll->NumPoints(0));
    for(int i=0;i<s;i++)
      {
      if (s/15>0 && i%(s/15)==0) ProgressBar(112,i*15/s,15,"Fetch vertex arrays"); 
	int ss = coll->NumPoints(i);
	for(int j=0;j<ss;j++) {
	  Point p = coll->FacePoint(i,j);
	  Vector n = coll->PointNormal(i,j);
	  unsigned int c = coll->Color(i,j);
	  Point2d tx = coll->TexCoord(i,j);
	  float tx3 = coll->TexCoord3(i,j);
	  vertex.push_back(p);
	  if(!(flags&DSDisableNormal))	  
	    normal.push_back(n);
	  if(!(flags&DSDisableColor))	  
	    color.push_back(c);
	  if(!(flags&DSDisableTexCoord))
	    texcoord.push_back(tx);
	  if(!(flags&DSDisableTexCoord3))
	    texcoord3.push_back(tx3);
	}
      }
    if (!(flags&DSDisableObjects)) {
    int s2 = coll->NumObjects();
    InstallProgress(113,"Fetch objects", 15);
    for(int k=0;k<s2;k++)
      {
	if (s2/15>0 && k%(s2/15)==0) ProgressBar(113,k*15/s2,15,"Fetch objects"); 
	std::pair<int,int> p = coll->GetObject(k);
	PP p2;
	p2.first = p.first;
	p2.second = p.second;
	obj.push_back(p2);
      }
    }

  }

  void Prepare() { 
    coll->Prepare();
    int s = coll->NumFaces();
    header.numfaces=s;
    int s3 = coll->NumObjects();
    header.numobjects = s3;
    int accum = 0;
    InstallProgress(111,"Fetch points", 15);
    pointcounts.reserve(s);
    vertexindex.reserve(s);
    for(int i=0;i<s;i++) {
      if (s/15>0 && i%(s/15)==0) ProgressBar(111,i*15/s,15,"Fetch points"); 
      int ss = coll->NumPoints(i);
      pointcounts.push_back(ss);
      vertexindex.push_back(accum);
      accum +=ss;
    }
    InstallProgress(112,"Fetch vertex arrays", 15);
    if(!(flags&DSDisableNormal))
      normal.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableColor))
      color.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableTexCoord))
      texcoord.reserve(s*coll->NumPoints(0));
    if(!(flags&DSDisableTexCoord3))
      texcoord3.reserve(s*coll->NumPoints(0));
    for(int i=0;i<s;i++)
      {
      if (s/15>0 && i%(s/15)==0) ProgressBar(112,i*15/s,15,"Fetch vertex arrays"); 
	int ss = coll->NumPoints(i);
	for(int j=0;j<ss;j++) {
	  Point p = coll->FacePoint(i,j);
	  Vector n = coll->PointNormal(i,j);
	  unsigned int c = coll->Color(i,j);
	  Point2d tx = coll->TexCoord(i,j);
	  float tx3 = coll->TexCoord3(i,j);
	  vertex.push_back(p);
	  if(!(flags&DSDisableNormal))	  
	    normal.push_back(n);
	  if(!(flags&DSDisableColor))	  
	    color.push_back(c);
	  if(!(flags&DSDisableTexCoord))
	    texcoord.push_back(tx);
	  if(!(flags&DSDisableTexCoord3))
	    texcoord3.push_back(tx3);
	}
      }
    if (!(flags&DSDisableObjects)) {
    int s2 = coll->NumObjects();
    InstallProgress(113,"Fetch objects", 15);
    for(int k=0;k<s2;k++)
      {
	if (s2/15>0 && k%(s2/15)==0) ProgressBar(113,k*15/s2,15,"Fetch objects"); 
	std::pair<int,int> p = coll->GetObject(k);
	PP p2;
	p2.first = p.first;
	p2.second = p.second;
	obj.push_back(p2);
      }
    }
  }
  int Type() const { return 0; }
  int NumBlocks() const { return 9; }
  int BlockTypeInv(int id) const
  {
    switch(id) {
    case 6: return 0;
    case 7: return 1;
    case 8: return 2;
    case 0: return 3;
    case 3: return 4;
    case 1: return 5;
    case 2: return 6;
    case 9: return 7;
    case 10: return 8;
    case -1: return -1;
    }
    return -1;
  }
  int BlockType(int block) const {
    switch(block) {
    case 0: return 6; // vertexheader
    case 1: return 7; // vertexpointcounts
    case 2: return 8; // vertexindex
    case 3: return 0; // vertex
    case 4: return 3; // normal
    case 5: return 1; // color
    case 6: return 2; // texcoord
    case 7: return 9; // texcoord3
    case 8: return 10; // objects
    };
    return -1;
  }
  size_t BlockSizeInBytes(int block) const
  {
    switch(block) {
    case 0: return sizeof(header);
    case 1: return pointcounts.size()*sizeof(int);
    case 2: return vertexindex.size()*sizeof(int);
    case 3: return vertex.size()*sizeof(Point);
    case 4: if (!(flags&DSDisableNormal)) return normal.size()*sizeof(Vector); else return 0;
    case 5: if (!(flags&DSDisableColor)) return color.size()*sizeof(unsigned int); else return 0;
    case 6: if (!(flags&DSDisableTexCoord)) return texcoord.size()*sizeof(Point2d); else return 0;
    case 7: if (!(flags&DSDisableTexCoord3)) return texcoord3.size()*sizeof(float); else return 0;
    case 8: if (!(flags&DSDisableObjects)) return obj.size()*sizeof(PP); else return 0;
    };
    return -1;
  }
  unsigned char *Block(int block) const
  {
    switch(block) {
    case 0: return (unsigned char*)&header;
    case 1: return (unsigned char*)&pointcounts[0];
    case 2: return (unsigned char*)&vertexindex[0];
    case 3: return (unsigned char*)&vertex[0];
    case 4: return (unsigned char*)&normal[0];
    case 5: return (unsigned char*)&color[0];
    case 6: return (unsigned char*)&texcoord[0];
    case 7: return (unsigned char*)&texcoord3[0];
    case 8: return (unsigned char*)&obj[0];
    };
    return 0;
  }
  unsigned char *BlockWithOffset(int block, int offset, int size) const
  {
    switch(block) {
    case 0: return ((unsigned char*)&header)+offset;
    case 1: return ((unsigned char*)&pointcounts[0])+offset;
    case 2: return ((unsigned char*)&vertexindex[0])+offset;
    case 3: return ((unsigned char*)&vertex[0])+offset;
    case 4: return ((unsigned char*)&normal[0])+offset;
    case 5: return ((unsigned char*)&color[0])+offset;
    case 6: return ((unsigned char*)&texcoord[0])+offset;
    case 7: return ((unsigned char*)&texcoord3[0])+offset;
    case 8: return ((unsigned char*)&obj[0])+offset;
    };
    return 0;
  }
private:
  FaceCollection *coll;
  DSVertexHeader header;
  std::vector<int> pointcounts;
  std::vector<int> vertexindex;
  std::vector<Point> vertex;
  std::vector<Vector> normal;
  std::vector<unsigned int> color;
  std::vector<Point2d> texcoord;
  std::vector<float> texcoord3;
  std::vector<PP> obj;
  int flags;
};

GameApi::DS GameApi::PolygonApi::p_ds_inv(GameApi::P p, int flags)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_disk_store(e, new DiskStoreCollection(coll,flags));
}

class DiskStoreModel : public DiskStore
{
public:
  DiskStoreModel(GLTFModelInterface *tf, int flags=-1) : tf(tf), flags(flags) { if (flags==-1) flags=0; }
  void Collect(CollectVisitor &vis)
  {
    tf->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
  }
  virtual void Prepare() { }
  virtual int Type() const { return 4; }
  virtual int NumBlocks() const { return 15; }
  virtual int BlockType(int block) const { return 0; }
  virtual int BlockTypeInv(int block) const { return 0; }
  virtual size_t BlockSizeInBytes(int block) const
  {
    switch(block) {
      // header
    case 0: return sizeof(DSGLTFHeader);
      // tinygltf data structures
    case 1: return sizeof(tinygltf::Accessor)*tf->accessors_size();
    case 2: return sizeof(tinygltf::Animation)*tf->animations_size();
    case 3: return sizeof(tinygltf::Buffer)*tf->buffers_size();
    case 4: return sizeof(tinygltf::BufferView)*tf->bufferviews_size();
    case 5: return sizeof(tinygltf::Material)*tf->materials_size();
    case 6: return sizeof(tinygltf::Mesh)*tf->meshes_size();
    case 7: return sizeof(tinygltf::Node)*tf->nodes_size();
    case 8: return sizeof(tinygltf::Texture)*tf->textures_size();
    case 9: return sizeof(tinygltf::Image)*tf->images_size();
    case 10: return sizeof(tinygltf::Skin)*tf->skins_size();
    case 11: return sizeof(tinygltf::Sampler)*tf->samplers_size();
    case 12: return sizeof(tinygltf::Camera)*tf->cameras_size();
    case 13: return sizeof(tinygltf::Scene)*tf->scenes_size();
    case 14: return sizeof(tinygltf::Light)*tf->lights_size();
      // tinygltf 2nd level data structures
    case 15: return sizeof(int)*tf->accessors_size();
    case 16:
      {
	int s = tf->accessors_size();
	int sum=0;
	for(int i=0;i<s;i++)
	  sum+=tf->get_accessor(i).name.size();
	return sum;
      }
    };
  }
  void fill_header() const
  {
    std::string base_url = tf->BaseUrl();
    std::copy(base_url.begin(), base_url.end(),header.base_url);
    std::string url = tf->Url();
    std::copy(url.begin(),url.end(),header.url);
    
    header.num_accessors = tf->accessors_size();
    header.num_animations = tf->animations_size();
    header.num_buffers = tf->buffers_size();
    header.num_bufferviews = tf->bufferviews_size();
    header.num_materials = tf->materials_size();
    header.num_meshes = tf->meshes_size();
    header.num_nodes = tf->nodes_size();
    header.num_textures = tf->textures_size();
    header.num_images = tf->images_size();
    header.num_skins = tf->skins_size();
    header.num_samplers = tf->samplers_size();
    header.num_cameras = tf->cameras_size();
    header.num_scenes = tf->scenes_size();
    header.num_lights = tf->lights_size();
  }
  virtual unsigned char* Block(int block) const
  {
    switch(block)
      {
      case 0: fill_header(); return (unsigned char*)&header;
      case 1: return (unsigned char*)&tf->get_accessor(0);
      case 2: return (unsigned char*)&tf->get_animation(0);
      case 3: return (unsigned char*)&tf->get_buffer(0);
      case 4: return (unsigned char*)&tf->get_bufferview(0);
      case 5: return (unsigned char*)&tf->get_material(0);
      case 6: return (unsigned char*)&tf->get_mesh(0);
      case 7: return (unsigned char*)&tf->get_node(0);
      case 8: return (unsigned char*)&tf->get_texture(0);
      case 9: return (unsigned char*)&tf->get_image(0);
      case 10: return (unsigned char*)&tf->get_skin(0);
      case 11: return (unsigned char*)&tf->get_sampler(0);
      case 12: return (unsigned char*)&tf->get_camera(0);
      case 13: return (unsigned char*)&tf->get_scene(0);
      case 14: return (unsigned char*)&tf->get_light(0);
	// tinygltf 2nd level data structures
      };
  }
  virtual unsigned char* BlockWithOffset(int block, int offset, int size) const
  {
    return Block(block)+offset;
  }
private:
  GLTFModelInterface *tf;
  int flags;
  
  // data
  mutable DSGLTFHeader header;
  
};

GameApi::DS GameApi::PolygonApi::tf_ds_inv(GameApi::TF tf, int flags)
{
  GLTFModelInterface *ttf = find_gltf(e, tf);
  return add_disk_store(e, new DiskStoreModel(ttf,flags));
}


std::string str_tolower(std::string s)
{
  std::transform(s.begin(),s.end(),s.begin(), [](unsigned char c) { return std::tolower(c); } );
  return s;
}

 class MTLParser
 {
 public:
   MTLParser(std::string data) : data(data) { }
   void load_file() {
     int count = -1;
     std::stringstream ss(data);
     std::string line;
     GameApi::MaterialDef def2;

     while(std::getline(ss,line)) {
       std::stringstream ss(line);
       std::string id;
       ss >> id;
       if (str_tolower(id)=="newmtl") {
	 if (count!=-1) {
	   GameApi::MaterialDef def = def2;
	   filenames.push_back(def);
	 }
	 count++;
	 ss >> def2.material_name;
	 def2.Ns = std::numeric_limits<float>::quiet_NaN();
	 def2.Ni = std::numeric_limits<float>::quiet_NaN();
	 def2.d = std::numeric_limits<float>::quiet_NaN();
	 def2.Tr = std::numeric_limits<float>::quiet_NaN();
	 def2.illum = 0;
	 def2.Ka_x = def2.Ka_y = def2.Ka_z = 0.0;
	 def2.Kd_x = def2.Kd_y = def2.Kd_z = 0.0;
	 def2.Ks_x = def2.Ks_y = def2.Ks_z = 0.0;
	 def2.Ke_x = def2.Ke_y = def2.Ke_z = 0.0;
       }
       if (str_tolower(id)=="ns") { ss >> def2.Ns; }
       if (str_tolower(id)=="ni") { ss >> def2.Ni; }
       if (str_tolower(id)=="d") { ss >> def2.d; }
       if (str_tolower(id)=="tr") { ss >> def2.Tr; }
       if (str_tolower(id)=="illum") { ss >> def2.illum; }
       if (str_tolower(id)=="ka") { ss >> def2.Ka_x >> def2.Ka_y >> def2.Ka_z; }
       if (str_tolower(id)=="kd") { ss >> def2.Kd_x >> def2.Kd_y >> def2.Kd_z; }
       if (str_tolower(id)=="ks") { ss >> def2.Ks_x >> def2.Ks_y >> def2.Ks_z; }
       if (str_tolower(id)=="ke") { ss >> def2.Ke_x >> def2.Ke_y >> def2.Ke_z; }
       if (str_tolower(id)=="map_ka") { ss >> def2.map_Ka; }
       if (str_tolower(id)=="map_kd") { ss >> def2.map_Kd; }
       if (str_tolower(id)=="map_d") { ss >> def2.map_d; }
       if (str_tolower(id)=="map_bump") { ss >> def2.map_bump; }
       if (str_tolower(id)=="bump") { ss >> def2.bump; }
     }
     GameApi::MaterialDef def = def2;
     filenames.push_back(def);
   }
   std::vector<GameApi::MaterialDef> get() const { return filenames; }
 private:
   std::string data;
   std::vector<GameApi::MaterialDef> filenames;
   std::string material_name;
};

// note, this function doesnt go to gameapi builder
std::vector<GameApi::MaterialDef> GameApi::PolygonApi::parse_mtl(std::string data)
{
  MTLParser parser(data);
  parser.load_file();
  return parser.get();
}

 std::string convert_slashes(std::string s)
 {
   int ss = s.size();
   for(int i=0;i<ss;i++)
     {
       if (s[i]=='\\') s[i]='/';
     }
   return s;
 }

 std::string GameApi::PolygonApi::output_ml(std::string objfileurl, int count, std::string prefix, std::vector<GameApi::MaterialDef> filenames)
 {
   int s2 = filenames.size();
   std::string label = "std::vector<std::string>{";
   for(int i=0;i<s2;i++)
     {
       label+=filenames[i].material_name;
       if (i!=s2-1) label+=",";
     }
  label+="}";
  std::string res;
  std::stringstream ss;
  ss << count;
  
  res+="P I1=ev.polygon_api.p_url_mtl(ev," + objfileurl + "," + ss.str() + "," + label + ");\n";
  int s = filenames.size();
  std::string names;
  names="std::vector<BM>{";
  for(int i=0;i<s;i++)
   {
     std::stringstream ii;
     ii << (i*2)+2;
     std::stringstream ii2;
     ii2 << (i*2+1)+2;
     res+="BM I" + ii.str() + "=ev.bitmap_api.loadbitmapfromurl(" + convert_slashes(prefix + "/" + filenames[i].map_Ka) + ");\n";
     res+="BM I" + ii2.str() + "=ev.bitmap_api.scale_bitmap(ev,I" + ii.str() + ",256,256);\n";
     names+="I" + ii2.str();
     if (i!=s-1) names+=",";
   }
  names+="}";
  std::stringstream sk;
  sk << (s*2)+2;
  std::stringstream sk2;
  sk2 << (s*2)+3;
  res+="MT I" + sk.str() + "=ev.materials_api.texture_many(ev," + names + ",1.0);\n";
  res+="ML I" + sk2.str() + "=ev.materials_api.bind(I1,I" + sk.str() + ");\n";
  return res;
}

int GameApi::PolygonApi::p_num_textures(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  return coll->NumTextures();
}
// you're only allowed to call this from ML type execute() or prepare()
void GameApi::PolygonApi::p_gen_texture(P p, int i)
{
  FaceCollection *coll = find_facecoll(e, p);
  coll->GenTexture(i);
}
// this can only be called after p_gen_texture.
GameApi::BM GameApi::PolygonApi::p_texture(P p, int i)
{
  FaceCollection *coll = find_facecoll(e, p);
  BufferRef ref = coll->TextureBuf(i);
  if (ref.buffer==0) { ref = BufferRef::NewBuffer(1,1); }
  //std::cout << "p_texture" << (int)ref.buffer << " " << ref.width << " " << ref.height << std::endl; 
  ::Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  BitmapColorHandle *handle2 = new BitmapColorHandle; 
  handle2->bm = bm;
  BM bm2 = add_bitmap(e, handle2);
  return bm2;
}


 class FaceCollectionSplitter : public FaceCollection
 {
 public:
   FaceCollectionSplitter(FaceCollection *coll, int start_index, int end_index) : coll(coll), start_index(start_index), end_index(end_index) { firsttime = true; }
  virtual std::string name() const { return "FaceCollectionSplitter"; }

   void Collect(CollectVisitor &vis)
   {
     coll->Collect(vis);
     vis.register_obj(this);
   }
   void HeavyPrepare()
   {
     if (firsttime) {
       //coll->Prepare();
       int s = coll->NumFaces();
       for(int i=0;i<s;i++) {
	 float val = coll->TexCoord3(i,0);
	 int val2 = int(val);
	 if (val2>=start_index && val2<end_index) {
	   facenums.push_back(i);
	 }
       }
       firsttime = false;
     }
   }
   
   virtual void Prepare()
   {
     if (firsttime) {
       coll->Prepare();
       int s = coll->NumFaces();
       for(int i=0;i<s;i++) {
	 float val = coll->TexCoord3(i,0);
	 int val2 = int(val);
	 if (val2>=start_index && val2<end_index) {
	   facenums.push_back(i);
	 }
       }
       firsttime = false;
     }
   }
   virtual int NumFaces() const { return facenums.size(); }
   virtual int NumPoints(int face) const
   {
         int sz = facenums.size();

     if (face>=0 && face<sz)
       return coll->NumPoints(facenums[face]);
     else return 1;
   }

  virtual bool has_normal() const { if (!coll) return false; return coll->has_normal(); }
  virtual bool has_attrib() const { if (!coll) return false;return coll->has_attrib(); }
  virtual bool has_color() const { if (!coll) return false; return coll->has_color(); }
  virtual bool has_texcoord() const { if (!coll) return false; return coll->has_texcoord(); }
  virtual bool has_skeleton() const { if (!coll) return false; return coll->has_skeleton(); }

   
   
   virtual Point FacePoint(int face, int point) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
     return coll->FacePoint(facenums[face],point);
     else return Point(0.0,0.0,0.0);
   }
   virtual Vector PointNormal(int face, int point) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
     return coll->PointNormal(facenums[face],point);
     else return Vector(0.0,0.0,0.0);
   }
   virtual float Attrib(int face, int point, int id) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
       return coll->Attrib(facenums[face],point,id);
     else return 0.0;
   }
   virtual int AttribI(int face, int point, int id) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
       return coll->AttribI(facenums[face],point,id);
     else
       return 0;
   }
   virtual unsigned int Color(int face, int point) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
       return coll->Color(facenums[face],point);
     else return 0x0;
   }
   virtual Point2d TexCoord(int face, int point) const
   {
    int sz = facenums.size();
     if (face>=0 && face<sz)
     return coll->TexCoord(facenums[face],point);
     else { Point2d p; p.x=0.0; p.y=0.0; return p; }
   }
   virtual float TexCoord3(int face, int point) const { 
    int sz = facenums.size();
     if (face>=0 && face<sz)
     return coll->TexCoord3(facenums[face],point)-start_index;
     else return 0.0;
   }


  virtual int NumTextures() const { return end_index-start_index; }
   virtual void GenTexture(int num) { coll->GenTexture(start_index + num); }
  virtual BufferRef TextureBuf(int num) const {
    return coll->TextureBuf(start_index + num);
  }
  virtual int FaceTexture(int face) const {
    int sz = facenums.size();
    if (face>=0 && face<sz)

    return coll->FaceTexture(facenums[face]) -start_index;
    else return 0;
  }
 private:
   FaceCollection *coll;
   int start_index, end_index;
   std::vector<int> facenums;
   bool firsttime;
 };

 GameApi::P GameApi::PolygonApi::texture_splitter(P p, int start_index, int end_index)
 {
   FaceCollection *coll = find_facecoll(e, p);
   return add_polygon2(e, new FaceCollectionSplitter(coll, start_index, end_index),1);
 }

GameApi::MT GameApi::PolygonApi::material_index(GameApi::EveryApi &ev, std::vector<MT> vec, int index)
{
  if (vec.size()==0) return ev.materials_api.m_def(ev);
  int sz = vec.size();
  if (index>=0 && index<sz)
    return vec[index];
  else
    return ev.materials_api.m_def(ev);
}
GameApi::ARR GameApi::PolygonApi::material_arr(std::vector<MT> vec, int start_range, int end_range)
{
  ArrayType *array = new ArrayType;
  array->type = 5;
  start_range = std::max(0,start_range);
  int s = vec.size();
  end_range = std::min(s,end_range);
  for(int i=start_range;i<end_range;i++)
    {
      array->vec.push_back(vec[i].id);
    }
  return add_array(e,array);
}


class TextureSplitter2 : public FaceCollection
{
public:
  TextureSplitter2(FaceCollection *coll, int val, int num_slots, int current_slot) : coll(coll), val(val),num_slots(num_slots), current_slot(current_slot)
  {
    if (num_slots==0) { num_slots=1; current_slot=0; }
  }
  virtual std::string name() const { return "TextureSplitter2"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
      firsttime=false;
    if (objs.size()==0) {
      int s = coll->NumObjects();
      for(int i=0;i<s;i++) {
	std::pair<int,int> p = coll->GetObject(i);
	//std::cout << "P:" << p.first << " " << p.second << std::endl;
	if (p.second-p.first>0) {
	  float mat = coll->TexCoord3(p.first,0);
	  //std::cout << "TEXCOORD0:" << mat << "==" << val << std::endl;
	  if (int(mat)==val) {
	    //std::cout << "PAIR:" << p.first << " " << p.second << std::endl;
	    objs.push_back(p);
	  }
	}
      }
    }
    }
  }
  void PrepareDone() { coll->PrepareDone(); }
  virtual void Prepare() {
    coll->Prepare();
    if (firsttime) {
      firsttime=false;
    if (objs.size()==0) {
      int s = coll->NumObjects();
      for(int i=0;i<s;i++) {
	std::pair<int,int> p = coll->GetObject(i);
	//std::cout << "P:" << p.first << " " << p.second << std::endl;
	if (p.second-p.first>0) {
	  float mat = coll->TexCoord3(p.first,0);
	  //std::cout << "TEXCOORD0:" << mat << "==" << val << std::endl;
	  if (int(mat)==val) {
	    //std::cout << "PAIR:" << p.first << " " << p.second << std::endl;
	    objs.push_back(p);
	  }
	}
      }
    }
    }
  }
  


  
  virtual int NumFaces() const {
    int s = objs.size();
    int count = 0;
    for(int i=0;i<s;i++) {
      std::pair<int,int> p = objs[i];
      count+=p.second-p.first;
    }
    actual_faces=count/num_slots;
    if (current_slot==num_slots-1)
      return count/num_slots + (count - num_slots*(count/num_slots));
    return count/num_slots;
  }
  int face_to_face(int face) const
  {
    int s = objs.size();
    int count = 0;
    for(int i=0;i<s;i++) {
      std::pair<int,int> p = objs[i];
      if (face>=count && face<count+p.second-p.first)
	{
	  //std::cout << "Returning:" << p.first + face - count << std::endl;
	  return p.first + face - count;
	}
      count+=p.second-p.first;
    }
    return 0;
  }

  virtual bool has_normal() const { if (!coll) return false; return coll->has_normal(); }
  virtual bool has_attrib() const { if (!coll) return false;return coll->has_attrib(); }
  virtual bool has_color() const { if (!coll) return false; return coll->has_color(); }
  virtual bool has_texcoord() const { if (!coll) return false; return coll->has_texcoord(); }
  virtual bool has_skeleton() const { if (!coll) return false; return coll->has_skeleton(); }

  
  virtual int NumPoints(int face) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->NumPoints(face_to_face(face));
  }
  virtual Point FacePoint(int face, int point) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->FacePoint(face_to_face(face),point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->PointNormal(face_to_face(face),point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->Attrib(face_to_face(face),point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->AttribI(face_to_face(face),point,id);

  }
  virtual unsigned int Color(int face, int point) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->Color(face_to_face(face),point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (actual_faces==0) NumFaces();
    face+=current_slot*actual_faces;
    return coll->TexCoord(face_to_face(face),point);
  }
  virtual float TexCoord3(int face, int point) const {
    return 0.0; // texture count to 0
    //return coll->TexCoord3(face_to_face(face),point);
  }
  virtual int NumTextures() const { return 1; }
  virtual void GenTexture(int num) { coll->GenTexture(val); }
  virtual BufferRef TextureBuf(int num) const { return coll->TextureBuf(val); }
  virtual int FaceTexture(int face) const { return 0; }
private:
  FaceCollection *coll;
  int val;
  std::pair<int,int> p;
  std::vector<std::pair<int,int> > objs;
  int num_slots;
  int current_slot;
  mutable int actual_faces=0;
  bool firsttime = true;
};

GameApi::P GameApi::PolygonApi::texture_splitter2(P p, int val, int num_slots, int current_slot)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new TextureSplitter2(coll,val, num_slots, current_slot),1);
}

GameApi::ARR GameApi::PolygonApi::material_extractor_p(P p, int start_index, int end_index, int num_slots, int current_slot)
{
  FaceCollection *coll = find_facecoll(e,p);
  coll->SetDoneCount(end_index-start_index);
  
  ArrayType *array = new ArrayType;
  array->type = 3;
  for(int i=start_index;i<end_index;i++)
    {
      GameApi::P p2 = texture_splitter2(p,i, num_slots, current_slot);
      array->vec.push_back(p2.id);
    }
  
  return add_array(e,array);
}

#if 0
class P_MTL2_ML : public MainLoopItem
{
public:
  P_MTL2_ML(GameApi::Env &e, GameApi::P p) :e(e), p(p) { }
    void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  void HeavyPrepare() { Prepare(); }
  void Prepare()
  {
    FaceCollection *coll = find_facecoll(e,p);
    coll->Prepare();
  }
  void execute(MainLoopEnv &e) { }
  void handle_event(MainLoopEvent &e) { }
private:
  GameApi::Env &e;
  GameApi::P p;
};
#endif

GameApi::ML GameApi::PolygonApi::p_mtl2_prepare(P p)
{
#if 0
  return add_main_loop(e, new P_MTL2_ML(e,p));
#endif
}

GameApi::ARR GameApi::PolygonApi::p_mtl2(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count, int start_index, int end_index, float mix)
{
  GameApi::P p = p_mtl(ev,obj_url,mtl_url, prefix, count);
  GameApi::ARR parr = material_extractor_p(p, start_index, end_index,1,0);
  GameApi::ARR matarr = p_mtl2_materials(ev,p);
  GameApi::ARR matarr2 = material_extractor_mt(ev,p,mix, start_index, end_index);
  GameApi::ARR bmarr = material_extractor_bm(p, start_index, end_index);
  GameApi::ML ml = p_mtl2_prepare(p);
  ArrayType *t = new ArrayType;
  t->vec.push_back(p.id);
  t->vec.push_back(parr.id);
  t->vec.push_back(matarr.id);
  t->vec.push_back(matarr2.id);
  t->vec.push_back(bmarr.id);
  t->vec.push_back(ml.id);
  return add_array(e,t);
}
GameApi::ARR GameApi::PolygonApi::p_mtl2_nr(EveryApi &ev, std::string obj_url, std::string mtl_url, std::string prefix, int count, int start_index, int end_index, float mix)
{
  GameApi::P p = p_mtl_nr(ev,obj_url,mtl_url, prefix, count);
  GameApi::ARR parr = material_extractor_p(p, start_index, end_index,1,0);
  GameApi::ARR matarr = p_mtl2_materials(ev,p);
  GameApi::ARR matarr2 = material_extractor_mt(ev,p,mix, start_index, end_index);
  GameApi::ARR bmarr = material_extractor_bm(p, start_index, end_index);
  GameApi::ML ml = p_mtl2_prepare(p);
  ArrayType *t = new ArrayType;
  t->vec.push_back(p.id);
  t->vec.push_back(parr.id);
  t->vec.push_back(matarr.id);
  t->vec.push_back(matarr2.id);
  t->vec.push_back(bmarr.id);
  t->vec.push_back(ml.id);
  return add_array(e,t);
}


class MaterialChoose : public Material
{
public:
  MaterialChoose(std::vector<Material*> mats, FaceCollection* faces) : mats(mats), faces(faces) { }
  virtual void logoexecute() { }
  virtual int mat(int p) const
  {
    return choose_mat()->mat(p);
  }
  virtual int mat_inst(int p, int pts) const { return choose_mat()->mat_inst(p,pts); }
  virtual int mat_inst_matrix(int p, int ms) const { return choose_mat()->mat_inst_matrix(p,ms); }
  virtual int mat_inst2(int p, int pta) const { return choose_mat()->mat_inst2(p,pta); }
  virtual int mat_inst_fade(int p, int pts, bool flip, float start_time, float end_time) const
  {
    return choose_mat()->mat_inst_fade(p,pts,flip,start_time, end_time);
  }
   

  int choose() const
  {
    FaceCollection *coll = faces;
    coll->Prepare();
    float val = faces->TexCoord3(0,0);
    int choose = (int)val;
    std::cout << "Material Choosing: " << val << " " << choose << std::endl;
    return choose;
  }
  Material *choose_mat() const
  {
    int ch = choose();
    return mats[ch];
  }
private:
  std::vector<Material*> mats;
  FaceCollection* faces;
};

GameApi::ARR GameApi::PolygonApi::material_choose(std::vector<MT> mat, std::vector<P> p)
{
  std::vector<Material*> mats;
  int s2 = mat.size();
  for(int i=0;i<s2;i++)
    {
      Material *m = find_material(e,mat[i]);
      mats.push_back(m);
    }
  ArrayType *t = new ArrayType;
  int s = p.size();
  for(int i=0;i<s;i++) {
    GameApi::P pp = p[i];
    FaceCollection *coll = find_facecoll(e,pp);
    Material *mt = new MaterialChoose(mats,coll);
    GameApi::MT mt2 = add_material(e,mt);
    t->vec.push_back(mt2.id);
  }
  return add_array(e,t);
}


class ExtractorBitmap : public Bitmap<Color>
{
public:
  ExtractorBitmap(FaceCollection *coll, int i, int start_index) : coll(coll), i(i), start_index(start_index) { bm=0;  }
  virtual int SizeX() const { if (bm) return bm->SizeX(); else return 1; }
  virtual int SizeY() const { if (bm) return bm->SizeY(); else return 1; }
  virtual Color Map(int x, int y) const { if (bm) return bm->Map(x,y); else return Color(0x0); }
  void Collect(CollectVisitor &vis)
  {
    if (i==start_index) coll->Collect(vis);
  }
  void HeavyPrepare()
  {
    if (i>=coll->NumTextures()) return;
    BufferRef ref = coll->TextureBuf(i);
    refs.push_back(ref);
    BitmapFromBuffer *buf = new BitmapFromBuffer(ref);
    bms.push_back(buf);
    bm = buf;
  }
  virtual void Prepare()
  {
    if (i==start_index)
      coll->Prepare();
    if (i>=coll->NumTextures()) return;
    BufferRef ref = coll->TextureBuf(i);
    refs.push_back(ref);
    BitmapFromBuffer *buf = new BitmapFromBuffer(ref);
    bms.push_back(buf);
    bm = buf;
  }
  ~ExtractorBitmap() {
    int s = refs.size();
    for(int i=0;i<s;i++)
      BufferRef::FreeBuffer(refs[i]);
    int s2 = bms.size();
    for(int i=0;i<s2;i++) delete bms[i];
    delete bm;
  }
private:
  FaceCollection *coll;
  int i;
  int start_index;
  std::vector<BufferRef> refs;
  std::vector<Bitmap<Color>*> bms;
  Bitmap<Color> *bm;
};

GameApi::ARR GameApi::PolygonApi::material_extractor_bm(P p, int start_index, int end_index)
{
  FaceCollection *coll = find_facecoll(e,p);
  std::vector<GameApi::BM> bms;
  ArrayType *array = new ArrayType;
  array->type = 4;
  for(int i=start_index;i<end_index;i++) {
    Bitmap<Color> *bitmap = new ExtractorBitmap(coll, i, start_index);
    BitmapColorHandle *handle2 = new BitmapColorHandle;
    handle2->bm = bitmap;
    BM bm = add_bitmap(e, handle2);
    array->vec.push_back(bm.id);
  }
  return add_array(e,array);
}

GameApi::ARR GameApi::PolygonApi::material_extractor_mt(GameApi::EveryApi &ev, P p, float mix, int start_index, int end_index)
{
  ARR arr = material_extractor_bm(p,start_index, end_index);
  ARR arr2 = p_mtl_bump(p);
  ARR arr3 = p_mtl_d(p);
  ArrayType *array2 = find_array(e,arr);
  ArrayType *array3 = find_array(e,arr2);
  ArrayType *array4 = find_array(e,arr3);
  ArrayType *array = new ArrayType;
  array->type = 4;

  
  int s = array2->vec.size();
  for(int i=0;i<s;i++) {
    int val = array2->vec[i];
    int sz = array3->vec.size();
    int sz2 = array4->vec.size();
    int val2 = i>=0&&i<sz ? array3->vec[i] : -1;
    int val3 = i>=0&&i<sz2 ? array4->vec[i] : -1;
    GameApi::BM bm;
    bm.id = val;
    if ((val2==-1 || val2 == 0) && (val3==-1 || val3==0)) {
      // only textures
      GameApi::MT mt1 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm},mix);
      //GameApi::MT mt2 = ev.materials_api.transparent_material(ev,bm,mt1);
      array->vec.push_back(mt1.id);
    } else if (val3==-1 || val3==0) {
      // textures and bump
      GameApi::BM bm2;
      bm2.id = val2;
      GameApi::MT mt1 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm2},mix);
      GameApi::MT mt2 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm},1.0-mix);
      GameApi::MT or_mt = ev.materials_api.combine_materials(ev,mt1,mt2);
      //GameApi::MT mt3 = ev.materials_api.transparent_material(ev,bm2,or_mt);
      array->vec.push_back(or_mt.id);
    } else if (val2==-1 || val2==0) {
      // textures and d
      GameApi::BM bm2;
      bm2.id = val3;
      GameApi::MT mt1 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm2},mix);
      GameApi::MT mt2 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm},1.0-mix);
      GameApi::MT or_mt = ev.materials_api.combine_materials(ev,mt1,mt2);
      //GameApi::MT mt3 = ev.materials_api.transparent_material(ev,bm2,or_mt);
      array->vec.push_back(or_mt.id);
    }
    else {
      // textures, d and bump
      GameApi::BM bm2;
      bm2.id = val2;
      GameApi::BM bm3;
      bm3.id = val3;
      GameApi::MT mt1 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm3},mix);
      GameApi::MT mt2 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm2},1.0-mix);
      GameApi::MT mt3 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm},0.5);
      GameApi::MT or_mt = ev.materials_api.combine_materials(ev,mt1,mt2);
      GameApi::MT or_mt2 = ev.materials_api.combine_materials(ev,or_mt,mt3);
      //GameApi::MT mt4 = ev.materials_api.transparent_material(ev,bm3,or_mt2);
      array->vec.push_back(or_mt2.id);

      /*
      GameApi::BM bm2;
      bm2.id = val2;
      GameApi::FB fb = ev.float_bitmap_api.from_red(bm2);
      GameApi::MT mt2 = ev.materials_api.bump_phong(ev,-0.3,0.3,-1,0xffff8800,0xffffffff,10,fb,5);
      GameApi::MT mt1 = ev.materials_api.texture_many(ev,std::vector<GameApi::BM>{bm},mix);
      GameApi::MT or_mt = ev.materials_api.combine_materials(ev,mt1,mt2);
      array->vec.push_back(or_mt.id);*/
    }
  }
  return add_array(e,array);
}

class TextureStorage : public ForwardFaceCollection
{
public:
  TextureStorage(FaceCollection *coll, int texture_sx, int texture_sy) : ForwardFaceCollection(*coll), coll(coll), texture_sx(texture_sx), texture_sy(texture_sy)
  {
    if (texture_sx>1024 ||texture_sy>1024)
      {
	std::cout << "Warning: Textures of sizes >1024 might not work in emscripten: " << texture_sx << " " << texture_sy << std::endl;
      }
    buf = BufferRef::NewBuffer(texture_sx, texture_sy);
    std::cout << "TextureStorage::" << buf.width << "x" << buf.height << "=" << MB(buf.width*buf.height*sizeof(unsigned int)) << std::endl;

  }
  virtual std::string name() const { return "TextureStorage"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    num = coll->NumFaces();
    num = sqrt(num);
    sx = texture_sx/num;
    sy = texture_sy/num;
  }
  void Prepare()
  {
    coll->Prepare();
    num = coll->NumFaces();
    num = sqrt(num);
    sx = texture_sx/num;
    sy = texture_sy/num;
  }
  virtual float TexCoord3(int face, int point) const { 
    return 0.0;
  }
  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const
  {
    std::pair<int,int> p = split_face(face);
    int pos_x = p.first*sx;
    int pos_y = p.second*sy;
    if (point==1) pos_x+=sx-1;
    if (point==2) { pos_x+=sx-1; pos_y+=sy-1; }
    if (point==3) { pos_y+=sy-1; }
    Point2d pp;
    pp.x = float(pos_x)/float(texture_sx);
    pp.y = float(pos_y)/float(texture_sy);
    return pp;
  }
  virtual int Reverse_TexCoord_Face(int x, int y) const
  {
    int xx = x/sx;
    int yy = y/sy;
    int face = xx+yy*num;
    return face;
  }
#if 0
  virtual Point Reverse_TexCoord_Pos(int x, int y) const
  {
    // works only with quads
    int face = Reverse_TexCoord_Face(x,y);
    Point p1 = coll->FacePoint(face,0);
    Point p2 = coll->FacePoint(face,1);
    Point p3 = coll->FacePoint(face,2);
    Point p4 = coll->FacePoint(face,3);
    Vector vx = p2-p1;
    Vector vy = p4-p1;
    std::pair<float,float> p = Reverse_TexCoord_UV(x,y);
    float coord_x = p.first;
    float coord_y = p.second;
    return p1 + vx*coord_x + vy*coord_y;
  }
#endif
#if 0
  virtual Vector Reverse_TexCoord_Normal(int x, int y) const
  {
    int face = Reverse_TexCoord_Face(x,y);
    std::pair<float,float> p = Reverse_TexCoord_UV(x,y);
    float coord_x = p.first;
    float coord_y = p.second;
    Vector n0 = coll->FaceNormal(face,0);
    Vector nx = coll->FaceNormal(face,1);
    Vector nxy = coll->FaceNormal(face,2);
    Vector ny = coll->FaceNormal(face,3);
    
    return 

  }
#endif
  /* TODO
  virtual std::pair<float,float> Reverse_TexCoord_UV(int x, int y) const
  {
    
  }
  */
  std::pair<int,int> split_face(int face) const
  {
    int num = coll->NumFaces();
    int sqrtnum = sqrt(num);
    sqrtnum++;
    int xx = face/sqrtnum;
    int yy = face - xx*sqrtnum;
    return std::make_pair(xx,yy);
  }
  virtual int NumTextures() const { return 1; }
  virtual void GenTexture(int num) {  }
  virtual BufferRef TextureBuf(int num) const {
    return buf;
  }
  virtual int FaceTexture(int face) const {
    return 0;
  }
private:
  FaceCollection *coll;
  BufferRef buf;
  int texture_sx, texture_sy;
  int sx,sy;
  int num;
};

GameApi::P GameApi::PolygonApi::texture_storage(P p, int texture_sx, int texture_sy)
{
  FaceCollection *face = find_facecoll(e,p);
  return add_polygon2(e, new TextureStorage(face, texture_sx, texture_sy), 1);
}

class ReplaceTexture : public ForwardFaceCollection
{
public:
  ReplaceTexture(FaceCollection *coll, Bitmap<::Color> *bm, int num) : ForwardFaceCollection(*coll), coll(coll), bm(bm),num2(num) {}
  virtual std::string name() const { return "ReplaceTexture"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual int NumTextures() const { return coll->NumTextures(); }
  virtual void GenTexture(int num) {
    if (num==num2) {
    coll->GenTexture(num);
    ref = coll->TextureBuf(num);
    int sx = ref.width;
    int sy = ref.height;
    float ssx = float(bm->SizeX());
    float ssy = float(bm->SizeY());
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++) {
	float xx = float(x)/float(ref.width);
	float yy = float(y)/float(ref.height);
	xx *= ssx;
	yy *= ssy;
	ref.buffer[x+y*ref.ydelta] = bm->Map(xx,yy).Pixel();
      }
    } else {
      coll->GenTexture(num);
    }
  }
  virtual BufferRef TextureBuf(int num) const {
    if (num==num2)
      return ref;
    return coll->TextureBuf(num);
  }
  virtual int FaceTexture(int face) const {
    return coll->FaceTexture(face);
  }  
private:
  FaceCollection *coll;
  Bitmap<::Color> *bm;
  BufferRef ref;
  int num2;
};

GameApi::P GameApi::PolygonApi::replace_texture(P p, BM bm, int num)
{
  FaceCollection *coll = find_facecoll(e, p);
  BitmapHandle *bbm0 = find_bitmap(e,bm);
  Bitmap<Color> *bbm = find_color_bitmap(bbm0);
  return add_polygon2(e, new ReplaceTexture(coll,bbm, num),1);
}

GameApi::P GameApi::PolygonApi::repeat_xy_p(EveryApi &ev, P p, float start_x, float start_y, float dx, float dy, int sx, int sy)
{
  BM bm = ev.bitmap_api.newbitmap(sx,sy,0xffffffff);
  PTS pts = ev.points_api.pts_grid(bm,start_x,start_x+dx*sx,start_y,start_y+dy*sy,0.0);
  P p2 = ev.polygon_api.static_instancing(ev, p, pts);
  return p2;
}
GameApi::P GameApi::PolygonApi::repeat_xz_p(EveryApi &ev, P p, float start_x, float start_z, float dx, float dz, int sx, int sz)
{
  BM bm = ev.bitmap_api.newbitmap(sx,sz,0xffffffff);
  PTS pts = ev.points_api.pts_grid(bm,start_x,start_x+dx*sx,start_z,start_z+dz*sz,0.0);
  PTS pts2 = ev.points_api.rot_x(pts, 3.14159265/2.0);
  P p2 = ev.polygon_api.static_instancing(ev, p, pts2);
  return p2;
}

GameApi::P GameApi::PolygonApi::repeat_yz_p(EveryApi &ev, P p, float start_y, float start_z, float dy, float dz, int sy, int sz)
{
  BM bm = ev.bitmap_api.newbitmap(sy,sz,0xffffffff);
  PTS pts = ev.points_api.pts_grid(bm,start_y,start_y+dy*sy,start_z,start_z+dz*sz,0.0);
  PTS pts2 = ev.points_api.rot_y(pts, 3.14159265/2.0);
  P p2 = ev.polygon_api.static_instancing(ev, p, pts2);
  return p2;
}

class MeshResize : public ForwardFaceCollection
{
public:
  MeshResize(FaceCollection *coll, Point start, Point end) : ForwardFaceCollection(*coll), coll(coll), start(start), end(end) { }
  virtual std::string name() const { return "MeshResize"; }

  void Collect(CollectVisitor &vis)
  {
    ForwardFaceCollection::Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    min_x=99999.0;
    max_x=-99999.0;
    min_y=99999.0;
    max_y=-99999.0;
    min_z=99999.0;
    max_z=-99999.0;
    
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      int ps = coll->NumPoints(i);
      for(int j=0;j<ps;j++)
	{
	  Point p = coll->FacePoint(i,j);
	  if (p.x<min_x) min_x=p.x;
	  if (p.x>max_x) max_x=p.x;
	  if (p.y<min_y) min_y=p.y;
	  if (p.y>max_y) max_y=p.y;
	  if (p.z<min_z) min_z=p.z;
	  if (p.z>max_z) max_z=p.z;
	}
    }
  }
  void Prepare() {
    ForwardFaceCollection::Prepare();
    min_x=99999.0;
    max_x=-99999.0;
    min_y=99999.0;
    max_y=-99999.0;
    min_z=99999.0;
    max_z=-99999.0;

    
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      int ps = coll->NumPoints(i);
      for(int j=0;j<ps;j++)
	{
	  Point p = coll->FacePoint(i,j);
	  if (p.x<min_x) min_x=p.x;
	  if (p.x>max_x) max_x=p.x;
	  if (p.y<min_y) min_y=p.y;
	  if (p.y>max_y) max_y=p.y;
	  if (p.z<min_z) min_z=p.z;
	  if (p.z>max_z) max_z=p.z;
	}
    }
  }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    p.x-=min_x;
    p.x/=(max_x-min_x);

    p.y-=min_y;
    p.y/=(max_y-min_y);

    p.z-=min_z;
    p.z/=(max_z-min_z);

    // now at [0..1]
    
    p.x*=end.x-start.x;
    p.x+=start.x;

    p.y*=end.y-start.y;
    p.y+=start.y;

    p.z*=end.z-start.z;
    p.z+=start.z;

    return p;    
  }
  
private:
  FaceCollection *coll;
  Point start, end;
  float min_x=99999.0;
  float max_x=-99999.0;
  float min_y=99999.0;
  float max_y=-99999.0;
  float min_z=99999.0;
  float max_z=-99999.0;
};

GameApi::P GameApi::PolygonApi::mesh_resize(P p, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon(e, new MeshResize(coll, Point(start_x, start_y, start_z), Point(end_x, end_y, end_z)), 1);
}

class SmoothNormals2 : public ForwardFaceCollection
{
public:
  SmoothNormals2(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { firsttime = true; }
  struct Data { Data() : v{0.01,0.01,0.01}, count(0) { } Vector v; int count; };

  virtual std::string name() const { return "SmoothNormals2"; }

  
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
    int s = coll->NumFaces();
    //int count = 0;
    for(int i=0;i<s;i++) {
      int s2 = coll->NumPoints(i);
      for(int j=0;j<s2;j++) {
	Point p1 = coll->FacePoint(i,(j+1)%s2);
	Point p2 = coll->FacePoint(i,(j+2)%s2);
	Point p3 = coll->FacePoint(i,(j+3)%s2);
	//Point p4 = coll->FacePoint(i,(j+3)%s2);
	//float area = Vector::DotProduct(coll->PointNormal(i,j),Vector::CrossProduct(p1,p2) + Vector::CrossProduct(p2,p3) + Vector::CrossProduct(p3,p4))/2.0;
	float a1 = Vector::Angle(p2-p1,p3-p1);
	//float a2 = Vector::Angle(p3-p2,p1-p2);
	//float a3 = Vector::Angle(p1-p3,p2-p3);
	if (std::isnormal(a1)) {
	  Point p = coll->FacePoint(i,j);
	  Data &v = mymap[key(p)];
	  Vector n = coll->PointNormal(i,j);
	  v.v+=n*a1;
	//v.v+=a2*n;
	//v.v+=a3*n;
	  v.count++;
	}
      }
    }
    firsttime = false;
    }
  }
  virtual void Prepare() {
    if (firsttime) {
    coll->Prepare();

    int s = coll->NumFaces();
    //int count = 0;
    for(int i=0;i<s;i++) {
      int s2 = coll->NumPoints(i);
      for(int j=0;j<s2;j++) {
	Point p1 = coll->FacePoint(i,(j+1)%s2);
	Point p2 = coll->FacePoint(i,(j+2)%s2);
	Point p3 = coll->FacePoint(i,(j+3)%s2);
	//Point p4 = coll->FacePoint(i,(j+3)%s2);
	//float area = Vector::DotProduct(coll->PointNormal(i,j),Vector::CrossProduct(p1,p2) + Vector::CrossProduct(p2,p3) + Vector::CrossProduct(p3,p4))/2.0;
	float a1 = Vector::Angle(p2-p1,p3-p1);
	//float a2 = Vector::Angle(p3-p2,p1-p2);
	//float a3 = Vector::Angle(p1-p3,p2-p3);
	if (std::isnormal(a1)) {
	  Point p = coll->FacePoint(i,j);
	  Data &v = mymap[key(p)];
	  Vector n = coll->PointNormal(i,j);
	  v.v+=n*a1;
	//v.v+=a2*n;
	//v.v+=a3*n;
	  v.count++;
	}
      }
    }
    firsttime = false;
    }
  }
  std::tuple<int,int,int> key(Point p) const
  {
    return std::make_tuple(int(p.x*100.0),int(p.y*100.0),int(p.z*100.0));
  }
  bool has_normal() const { return coll->has_normal(); }
  virtual Vector PointNormal(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    Data v = mymap[key(p)];
    if (v.count==1) { return coll->PointNormal(face,point); }
    if (v.count==0) v.count++;
    Vector vv = v.v/float(v.count);
    float dist = vv.Dist();
    //if (dist<0.01) { vv=Vector(1.0,0.0,0.0); dist=1.0; }
    return (vv)/dist;
  }
private:
  FaceCollection *coll;
  mutable std::map<std::tuple<int,int,int>, Data> mymap;
  bool firsttime;
};

GameApi::P GameApi::PolygonApi::smooth_normals2(P p) {
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new SmoothNormals2(coll),1);
}



class AccelNode
{
public:
  std::vector<void*> vec;
};

class AccelStructure
{
public:
  virtual ~AccelStructure() { }
  virtual void clear()=0;
  virtual AccelNode *find_point(Point p) const=0;
  virtual AccelNode *find_next(AccelNode *current, Point start, Point target, float &tmin) const=0;
  virtual std::vector<AccelNode*> find_ray(Point p1, Point p2, float &tmin) const=0;
  virtual std::vector<AccelNode*> find_tri(Point p1, Point p2, Point p3) const=0;
  virtual std::vector<AccelNode*> find_cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) const=0;
  virtual std::vector<AccelNode*> find_quad(Point p1, Point p2, Point p3, Point p4) const=0;
  virtual void push_back(Point p, void *data)=0;
  virtual void push_back(AccelNode *n, void *data)=0;
};

struct AccelNodeSpec
{
  int x,y,z;
};

#if 0
class GridAccel : public AccelStructure
{
public:
  GridAccel(int sx, int sy, int sz,
	    float start_x, float end_x,
	    float start_y, float end_y,
	    float start_z, float end_z) : sx(sx), sy(sy), sz(sz),
					  start_x(start_x), end_x(end_x),
					  start_y(start_y), end_y(end_y),
					  start_z(start_z), end_z(end_z) 
  { 
    grid = new AccelNode[sx*sy*sz];
  }
  void clear()
  {
    delete [] grid;
    grid = new AccelNode[sx*sy*sz];
  }
  AccelNode *find_node(const AccelNodeSpec &spec) const
  {
    if (spec.z>=0&&spec.z<sz)
      if (spec.y>=0&&spec.y<sy)
	if (spec.x>=0&&spec.x<sx)
	  return &grid[spec.z*sx*sy +
		       spec.y*sx +
		       spec.x];
    return 0;
  }
  AccelNodeSpec find_spec(AccelNode *n) const
  {
    if (!n) { AccelNodeSpec s; s.x = 0; s.y=0; s.z=0; return s; }
    int num = n - grid;
    int zz = num / (sx*sy);
    int rest = num - zz*sx*sy;
    int yy = rest / sx;
    int rest2 = rest - yy*sx;
    int xx = rest2;
    AccelNodeSpec s;
    s.x = xx;
    s.y = yy;
    s.z = zz;
    return s;
  }
  std::pair<Point,Point> bounds(AccelNodeSpec spec) const
  {
    float pos_x = start_x+float(spec.x)/float(sx)*(end_x-start_x);
    float pos_y = start_y+float(spec.y)/float(sy)*(end_y-start_y);
    float pos_z = start_z+float(spec.z)/float(sz)*(end_z-start_z);
    float tend_x = pos_x + (end_x-start_x)/float(sx);
    float tend_y = pos_y + (end_y-start_y)/float(sy);
    float tend_z = pos_z + (end_z-start_z)/float(sz);
    return std::make_pair(Point(pos_x,pos_y,pos_z),Point(tend_x,tend_y,tend_z));
  }
  bool hit(Point ray_pos, Vector ray_dir, std::pair<Point,Point> bound, float &tmin, float &tmax) const
  {
    tmin = (bound.first.x - ray_pos.x)/ray_dir.dx;
    tmax = (bound.second.x - ray_pos.x)/ray_dir.dx;
    
    if (tmin > tmax) std::swap(tmin,tmax);
    
    float tymin = (bound.first.y - ray_pos.y) / ray_dir.dy;
    float tymax = (bound.second.y - ray_pos.y) / ray_dir.dy;

    if (tymin > tymax) std::swap(tymin,tymax);
    if ((tmin > tymax) ||(tymin > tmax)) return false;
    if (tymin > tmin)
      tmin = tymin;
    if (tymax < tmax)
      tmax = tymax;
    float tzmin = (bound.first.z - ray_pos.z) / ray_dir.dz;
    float tzmax = (bound.second.z - ray_pos.z) / ray_dir.dz;
    
    if (tzmin > tzmax) std::swap(tzmin,tzmax);
    
    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    if (tzmin > tmin) tmin=tzmin;
    if (tzmax < tmax) tmax = tzmax;
    return true;
  }
  virtual AccelNode *find_point(Point p) const
  {
    if (p.x<start_x) { return 0; }
    if (p.y<start_y) { return 0; }
    if (p.z<start_z) { return 0; }
    if (p.x>end_x) { return 0; }
    if (p.y>end_y) { return 0; }
    if (p.z>end_z) { return 0; }
				 
    p-=Vector(start_x, start_y, start_z);
    float dx = p.x / (end_x-start_x);
    float dy = p.y / (end_y-start_y);
    float dz = p.z / (end_z-start_z);
    // now [0..1]
    float ddx = dx * sx;
    float ddy = dy * sy;
    float ddz = dz * sz;
    // now [0..box]
    AccelNodeSpec s;
    s.x = int(ddx);
    s.y = int(ddy);
    s.z = int(ddz);
    AccelNode *n = find_node(s);
    return n;
  }
  Point2d edge_component( float start_x, float target_x, // x is special
			  float start_y, float target_y,
			  float start_z, float target_z, int x_spec) const
  {
    float s_x = start_x;
    float e_x = target_x;
    int x = 0;
    bool forward;
    if (s_x < e_x)
      {
	forward = true;
	x = x_spec + 1;
      }
    else
      {
	forward = false;
	x = x_spec; // why not -1?
      }
    float p_x = start_x + x*(end_x-start_x)/sx;
    float d_x = fabs(p_x-s_x);
    float dx = fabs(target_x-start_x);
    float dy = fabs(target_y-start_y);
    float dz = fabs(target_z-start_z);
    //if (dx<0.01) dx=1.0;
    //if (dy<0.01) dy=1.0;
    //if (dz<0.01) dz=1.0;
    float ky = dy*d_x/dx;
    float kz = dz*d_x/dx;
    if (!forward) { ky = -ky; kz = -kz; }
    Point2d p;
    p.x = ky;
    p.y = kz;
    return p;
  }

  Point point_at_edge_of_box(Point current, Point target, AccelNode *discard) const
  {
    // TODO RAY-BOX INTERSECTION
    AccelNodeSpec spec = find_spec(find_point(current));

    Point2d pyz = edge_component( current.x, target.x,
				 current.y, target.y,
				 current.z, target.z, spec.x);
    Point2d pxz = edge_component( current.y, target.y,
				 current.x, target.x,
				 current.z, target.z, spec.y);
    Point2d pxy = edge_component( current.z, target.z,
				 current.x, target.x,
				 current.y, target.y, spec.z);
    
    float x_delta = std::min(fabs(pxz.x),fabs(pxy.x));
    float y_delta = std::min(fabs(pyz.x),fabs(pxy.y));
    float z_delta = std::min(fabs(pyz.y),fabs(pxz.y));
    float x_mult = pyz.x < 0.0 ? -1.0 : 1.0;
    float y_mult = pxz.x < 0.0 ? -1.0 : 1.0;
    float z_mult = pxy.x < 0.0 ? -1.0 : 1.0;


    Vector v = { x_mult*x_delta, y_mult*y_delta, z_mult*z_delta };
    return current + v;
  }
  bool point_box_intersection(Point p, AccelNode *node) const
  {
    AccelNodeSpec spec = find_spec(node);
    p.x-=start_x;
    p.y-=start_y;
    p.z-=start_z;
    p.x/=end_x-start_x;
    p.y/=end_y-start_y;
    p.z/=end_z-start_z;
    p.x*=sx;
    p.y*=sy;
    p.z*=sz;
    int x = (int)p.x;
    int y = (int)p.y;
    int z = (int)p.z;
    return x==spec.x && y==spec.y && z==spec.z;
  }
  virtual AccelNode *find_next(AccelNode *current, Point start, Point target, float &tmin) const
  {
    if (point_box_intersection(target, current)) return 0; // we're at target

    AccelNodeSpec spec = find_spec(current);
    AccelNodeSpec spec_px = spec; spec_px.x++;
    AccelNodeSpec spec_mx = spec; spec_mx.x--;
    AccelNodeSpec spec_py = spec; spec_py.y++;
    AccelNodeSpec spec_my = spec; spec_my.y--;
    AccelNodeSpec spec_pz = spec; spec_pz.z++;
    AccelNodeSpec spec_mz = spec; spec_mz.z--;
    
    std::pair<Point,Point> b_px = bounds(spec_px);
    std::pair<Point,Point> b_mx = bounds(spec_mx);
    std::pair<Point,Point> b_py = bounds(spec_py);
    std::pair<Point,Point> b_my = bounds(spec_my);
    std::pair<Point,Point> b_pz = bounds(spec_pz);
    std::pair<Point,Point> b_mz = bounds(spec_mz);
    
    Vector dir = target-start;
    float tmin_px, tmax_px;
    float tmin_mx, tmax_mx;
    float tmin_py, tmax_py;
    float tmin_my, tmax_my;
    float tmin_pz, tmax_pz;
    float tmin_mz, tmax_mz;
    tmin = 99999999.0;
    if (hit(start,dir, b_px,tmin_px,tmax_px)) {  
      if (tmin_px<tmin && tmin_px>0.0001) { spec = spec_px; tmin=tmin_px; } 
      if (tmax_px<tmin && tmax_px>0.0001) { spec = spec_px; tmin=tmax_px; } 
    }
    if (hit(start,dir, b_mx,tmin_mx,tmax_mx)) {  
      if (tmin_mx<tmin && tmin_mx>0.0001) { spec = spec_mx; tmin=tmin_mx; } 
      if (tmax_mx<tmin && tmax_mx>0.0001) { spec = spec_mx; tmin=tmax_mx; } 
    }
    if (hit(start,dir, b_py,tmin_py,tmax_py)) {  
      if (tmin_py<tmin && tmin_py>0.0001) { spec = spec_py; tmin=tmin_py;} 
      if (tmax_py<tmin && tmax_py>0.0001) { spec = spec_py; tmin=tmax_py; } 
    }
    if (hit(start,dir, b_my,tmin_my,tmax_my)) { 
      if (tmin_my<tmin && tmin_my>0.0001) { spec = spec_my; tmin=tmin_my; } 
      if (tmax_my<tmin && tmax_my>0.0001) { spec = spec_my; tmin=tmax_my; } 
    }
    if (hit(start,dir, b_pz,tmin_pz,tmax_pz)) { 
      if (tmin_pz<tmin && tmin_pz>0.0001) { spec = spec_pz; tmin=tmin_pz;} 
      if (tmax_pz<tmin && tmax_pz>0.0001) { spec = spec_pz; tmin=tmax_pz; } 
    }
    if (hit(start,dir, b_mz,tmin_mz,tmax_mz)) { 
      if (tmin_mz<tmin && tmin_mz>0.0001) { spec = spec_mz; tmin=tmin_mz; } 
      if (tmax_mz<tmin && tmax_mz>0.0001) { spec = spec_mz; tmin=tmax_mz; } 
    }
    if (tmin>9999999.0) { return 0; }

    return find_node(spec);
    


 #if 0
    AccelNodeSpec spec = find_spec(current);

    std::cout << "Spec: " << spec.x << " " << spec.y << " " << spec.z << std::endl;

    Point2d pyz = edge_component( start.x, target.x,
				 start.y, target.y,
				 start.z, target.z, spec.x);
    Point2d pxz = edge_component( start.y, target.y,
				 start.x, target.x,
				 start.z, target.z, spec.y);
    Point2d pxy = edge_component( start.z, target.z,
				  start.x, target.x,
				  start.y, target.y, spec.z);

    std::cout << "Edge components(pyz,pxz,pxy): " << pyz << " " << pxz << " " << pxy << std::endl;

    bool b_x = fabs(pxz.x) < fabs(pxy.x);
    bool b_y = fabs(pyz.x) < fabs(pxy.y);
    bool b_z = fabs(pyz.y) < fabs(pxz.y);

    std::cout << "Bools: " << b_x << " " << b_y << " " << b_z << std::endl;

    int x_dir=0;
    int y_dir=0;
    int z_dir=0;
    // TODO: what happens if x_dir, y_dir or z_dir is 0?
    if (b_x) { /* choose pxz */ 
      if (pxz.x < 0.0) { 
	y_dir = -1;
      } else {
	y_dir = 1;
      }
    } else { 
      if (pxy.x < 0.0)
	{
	  z_dir = -1;
	}
      else
	{
	  z_dir = 1;
	}
      /* choose pxy */ 
    }
    if (b_y) {
      if (pyz.x < 0.0)
	{
	  x_dir = -1;
	}
      else
	{
	  x_dir = 1;
	}
      /* choose pyz */ 
    } else {
      if (pxy.y < 0.0)
	{
	  z_dir = -1;
	}
      else
	{
	  z_dir = 1;
	}
      /* choose pxy */ 
    }
    if (b_z) { 
      if (pyz.y < 0.0)
	{
	  x_dir = -1;
	}
      else
	{
	  x_dir = 1;
	}
      /* choose pyz */ 
    } else { 
      if (pxz.y < 0.0)
	{
	  y_dir = -1;
	}
      else
	{
	  y_dir = 1;
	}
      /* choose pxz */ 
    }
    std::cout << "Dirs: " << x_dir << " " << y_dir << " " << z_dir << std::endl;
    float x_delta = std::min(fabs(pxz.x),fabs(pxy.x));
    float y_delta = std::min(fabs(pyz.x),fabs(pxy.y));
    float z_delta = std::min(fabs(pyz.y),fabs(pxz.y));

    std::cout << "Deltas: " << x_delta << " " << y_delta << " " << z_delta << std::endl;

    if (x_delta < y_delta && x_delta < z_delta && x_dir!=0)
      {
	spec.x+=x_dir;
	std::cout << "MoveX" << x_dir << std::endl;
      } else
    if (y_delta < x_delta && y_delta < z_delta && y_dir!=0)
      {
	spec.y+=y_dir;
	std::cout << "MoveY" << y_dir << std::endl;
      } else
    if (z_delta < x_delta && z_delta < y_delta && z_dir!=0)
      {
	spec.z+=z_dir;
	std::cout << "MoveZ" << z_dir << std::endl;
      } 
    return find_node(spec);
#endif
  }
  virtual std::vector<AccelNode*> find_quad(Point p1, Point p2, Point p3, Point p4) const
  {
    int xxxmin = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
    int yyymin = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
    int zzzmin = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));

    int xxxmax = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
    int yyymax = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
    int zzzmax = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));

    return find_cube(xxxmin, xxxmax,
		     yyymin, yyymax,
		     zzzmin, zzzmax);
		     
  }
  virtual std::vector<AccelNode*> find_ray(Point p1, Point p2, float &tmin) const
  {
    std::vector<AccelNode*> vec;
    AccelNode *n = find_point(p1);
    vec.push_back(n);
    //std::cout << "find_ray_s: " << n << " " << p1 << " " << p2 << std::endl;
    float t = 0.0;
    while(n && !(std::isnan(p1.x)||std::isnan(p1.y)||std::isnan(p1.z)))
      {
    //std::cout << "find_ray: " << n << " " << p1 << " " << p2 << std::endl;
	float tmin;
	n = find_next(n, p1, p2,tmin);
	if (n)
	  {
            p1 = p1+tmin*Vector(p2-p1);
	    t+=tmin;
	    vec.push_back(n);
	  } 
      }
    tmin = t;
    //std::cout << "Exit ray: " << n << " " << p1 << std::endl;
    return vec;
  }
  virtual std::vector<AccelNode*> find_tri(Point p1, Point p2, Point p3) const
  {
    int xxxmin = std::min(std::min(p1.x,p2.x),p3.x);
    int yyymin = std::min(std::min(p1.y,p2.y),p3.y);
    int zzzmin = std::min(std::min(p1.z,p2.z),p3.z);

    int xxxmax = std::max(std::max(p1.x,p2.x),p3.x);
    int yyymax = std::max(std::max(p1.y,p2.y),p3.y);
    int zzzmax = std::max(std::max(p1.z,p2.z),p3.z);

    return find_cube(xxxmin, xxxmax,
		     yyymin, yyymax,
		     zzzmin, zzzmax);
  }
  virtual std::vector<AccelNode*> find_cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) const
  {
    AccelNode *n0 = find_point(Point(start_x, start_y, start_z));
    AccelNode *nx = find_point(Point(end_x, start_y, start_z));
    AccelNode *ny = find_point(Point(start_x, end_y, start_z));
    AccelNode *nz = find_point(Point(start_x, start_y, end_z));
    
    AccelNodeSpec s0 = find_spec(n0);
    AccelNodeSpec sx = find_spec(nx);
    AccelNodeSpec sy = find_spec(ny);
    AccelNodeSpec sz = find_spec(nz);


    int s_x = s0.x;
    int e_x = sx.x;
    int s_y = s0.y;
    int e_y = sy.y;
    int s_z = s0.z;
    int e_z = sz.z;
    if (e_x==s_x) e_x++;
    if (e_y==s_y) e_y++;
    if (e_z==s_z) e_z++;

    std::vector<AccelNode*> vec;
    for(int z=s_z;z<e_z;z++)
      for(int y=s_y;y<e_y;y++)
	for(int x=s_x;x<e_x;x++)
	  {
	    AccelNodeSpec s = { x,y,z };
	    vec.push_back(find_node(s));
	  }
    return vec;
  }
  virtual void push_back(Point p, void *data)
  {
    AccelNode *n = find_point(p);
    push_back(n, data);
  }
  virtual void push_back(AccelNode *n, void *data)
  {
    if (n)
      n->vec.push_back(data);
  }

private:
  int sx,sy,sz;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  AccelNode *grid;
};
#endif
std::pair<Point,Point> find_bounds(FaceCollection *coll)
{
  Point mymin, mymax;
  mymin = Point(10000000.0,1000000.0,100000.0);
  mymax = Point(-1000000.0,-100000.0,-100000.0);
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      int sp = coll->NumPoints(i);
      for(int j=0;j<sp;j++)
	{
	  Point p = coll->FacePoint(i,j);
	  if (p.x<mymin.x) mymin.x = p.x;
	  if (p.y<mymin.y) mymin.y = p.y;
	  if (p.z<mymin.z) mymin.z = p.z;

	  if (p.x>mymax.x) mymax.x = p.x;
	  if (p.y>mymax.y) mymax.y = p.y;
	  if (p.z>mymax.z) mymax.z = p.z;
	}
    }
  return std::make_pair(mymin,mymax);
}

struct Accel_P_ref
{
  FaceCollection *coll;
  int face;
};

// note, FaceCollection must be prepared before this
void bind_accel(FaceCollection *coll, AccelStructure *accel) {
  int s = coll->NumFaces();
  for(int i=0;i<s;i++)
    {
      Accel_P_ref *ref = new Accel_P_ref;
      ref->coll = coll;
      ref->face = i;
      int c = coll->NumPoints(i);
      std::vector<AccelNode*> vec;
      switch(c) {
      case 3:
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  vec = accel->find_tri(p1,p2,p3);
	  break;
	}
      case 4:
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  Point p4 = coll->FacePoint(i,3);
	  vec = accel->find_quad(p1,p2,p3,p4);
	  break;
	}
      };
      int s = vec.size();
      for(int i=0;i<s;i++)
	{
	  accel->push_back(vec[i], (void*)ref);
	}
    }

}

 class AreaCache
{
public:
  struct Area { int face; float area; };
  void push_area(int face, float area) { 
    if (std::isnan(area)) return;
    Area a; a.face = face; a.area=area; areas.push_back(a); }
  void calc_sum(int count)
  {
    sum = 0.0;
    int s = areas.size();
    for(int i=0;i<s;i++) {
      sum+=areas[i].area;
    }
    sum/=float(areas.size());
    p = float(1.0)/float(sum);
    int ss = areas.size();
    //float pos = 0.0;
    for(int i=0;i<ss;i++)
      {
	Area a = areas[i];
	//std::cout << "Area: " << a.face << " " << a.area << std::endl;
	float delta = p*a.area;
	//std::cout << "Delta:" << delta << std::endl;
	for(float pp=0.0;pp<delta;pp+=1.0) {
	  pick.push_back(a);
	}
      }
    //std::cout << "Pick size: " << pick.size() << std::endl;
    p = float(pick.size())/float(sum);
    m_count = count;
  }
  float range() const { return sum; }
  int choose(float val) const
  {
    int s = pick.size();
    int val2 = int(val*s/m_count);
    //std::cout << val2 << std::endl;
    if (val2>=0 && val2<s)
      {
	Area a = pick[val2];
	//std::cout << "Choose " << a.face << std::endl;
	return a.face;
      }
    //std::cout << "Choose none" << std::endl;
    return 0;
  }

private:
  std::vector<Area> areas;
  float m_count;
  float sum;
  float p;
  std::vector<Area> pick;
};

 class ShadowColor;
 struct ShadowCB
{
  ShadowColor *m_this;
  int current;
};

 void shadow_color_callback(void *ptr);
#if 0
 class ShadowColor : public ForwardFaceCollection
{
public:
  ShadowColor(FaceCollection *coll, int num, Vector light_dir) : ForwardFaceCollection(*coll), coll(coll),count(num), light_dir(light_dir), grid(0), cache(0) { 
    for(int i=0;i<split_count;i++) { 
      InstallProgress(4+i,"lighting",15);
    }
      
}
  virtual ~ShadowColor() { delete grid; delete cache; }

  void ASyncCallback(int q)
  {

    int numfaces = coll->NumFaces();
    int num = count;
    Random r;
    float p=15.0/float(num/split_count);
    for(int h=0;h<num/split_count;h++) {
      if (h%100==0)
	ProgressBar(4+q,p*h,p*num/split_count,"lighting");
      float xp = double(r.next())/r.maximum();
      float yp = double(r.next())/r.maximum();
      float zp = double(r.next())/r.maximum();
      xp*=2.0;
      yp*=2.0;
      xp-=1.0;
      yp-=1.0;
      zp*=float(30000.0);
      //std::cout << "Random: " << xp << " " << yp << " " << zp << std::endl;
      if (std::isnan(xp)) continue;
      if (std::isnan(yp)) continue;
      if (std::isnan(zp)) continue;
      
      int zpi = cache->choose(zp);
      if (zpi<0) zpi = 0;
      if (zpi>=numfaces) zpi = numfaces-1;
      //std::cout << "zpi: " << zpi << std::endl;

      int num = int(coll->TexCoord3(zpi,0));
      //std::cout << "num: " << num << std::endl;
      BufferRef ref = coll->TextureBuf(num);
      int sx = ref.width;
      int sy = ref.height;
      
      
      Point p1 = coll->FacePoint(zpi, 0);
      Point p2 = coll->FacePoint(zpi, 1);
      Point p3 = coll->FacePoint(zpi, 2);
      Point p4 = coll->FacePoint(zpi, 3);
      Point p = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(p1) + (1.0f+xp)*(1.0f-yp)*Vector(p2) + (1.0f+xp)*(1.0f+yp)*Vector(p3) + (1.0f-xp)*(1.0f+yp)*Vector(p4));

      Point2d t1 = coll->TexCoord(zpi, 0);
      Point2d t2 = coll->TexCoord(zpi, 1);
      Point2d t3 = coll->TexCoord(zpi, 2);
      Point2d t4 = coll->TexCoord(zpi, 3);
      //std::cout << "TexCoord: " << t1 << " " << t2 << " " << t3 << " " << t4 << std::endl;
      Point t = 1.0/4.0*((1.0f-xp)*(1.0f-yp)*Vector(t1.x,t1.y,0.0) + (1.0f+xp)*(1.0f-yp)*Vector(t2.x,t2.y,0.0) + (1.0f+xp)*(1.0f+yp)*Vector(t3.x,t3.y,0.0) + (1.0f-xp)*(1.0f+yp)*Vector(t4.x,t4.y,0.0));
      Point2d tt = { t.x,t.y };
      
      //std::cout << "Pos: " << p << " tex: " << t << std::endl;

      int x = int(tt.x*sx);
      int y = int(tt.y*sy);
      
      if (x<0 || x>=sx) continue;
      if (y<0 || y>=sy) continue;

      ref.buffer[x+y*ref.ydelta] = 0x80ffffff;

	  Point pos = p;
	  Point pos_end = pos+light_dir*2000.0;

	  //std::cout << pos << " " << pos_end << std::endl;
	  float tmin = 0.0;
	  std::vector<AccelNode*> ray_data = grid->find_ray(pos,pos_end, tmin);
	  int s = ray_data.size();
	  //std::cout << "ray_data:  " << s << std::endl;
	  bool exit = false;
	  for(int i=0;i<s;i++) {
	    AccelNode *n = ray_data[i];
	    //std::cout << "accelnode: " << n << std::endl;
	    if (!n) continue;
	    int ss = n->vec.size();
	    //std::cout << "count:" << ss << std::endl;
	    for(int j=0;j<ss;j++)
	      {
		void *ptr = n->vec[j];
		//std::cout << "void*: " << int(ptr) << std::endl;
		if (!ptr) continue;
		Accel_P_ref *ref2 = (Accel_P_ref*)ptr;
		FaceCollection *coll = ref2->coll;
		int face = ref2->face;
		Point p1 = coll->FacePoint(face,0);
		Point p2 = coll->FacePoint(face,1);
		Point p3 = coll->FacePoint(face,2);
		Point p4 = coll->FacePoint(face,3);
		
		//std::cout << "LineProperties:" << pos << " " << pos+light_dir << std::endl;
		LineProperties p(pos,pos+light_dir /* *20 */);
		float t;
		bool b = p.QuadIntersection(p1,p2,p3,p4,t);
		//std::cout << "quadintersect: " << b << std::endl;
		if (b) {
		  //std::cout << "intersect" << std::endl;
		  //Plane pl(p1,p2-p1, p3-p1);
		  //float val = pl.Dist(pos);
		  Point pos2 = pos + t*light_dir;
		  Vector v = pos2-pos;
		  float val = v.Dist();

		  val/=10.0;
		  //std::cout << "Dist: " << val << std::endl;
		  if (val>255.0) val=255.0;
		  //val = 255.0-val;
		  int val2 = int(val);
		  //unsigned int old = ref.buffer[x+y*ref.ydelta];
		  //unsigned int old2 = old&0xff;
		  //if (val2<old2) {
		    unsigned int rgb = 0x80000000 + (val2<<16) + (val2<<8); /*+ val2*/;
		    //std::cout << "rgb: " << x << " " << y << ":" << std::hex << int(rgb) << std::endl;
		    ref.buffer[x+y*ref.ydelta] = rgb;
		    //}
		  exit = true;
		  break;
		  }
	      }
	    if (exit==true) break;
	  }
    }
#ifdef EMSCRIPTEN
      async_pending_count--;
#endif

  }
  void Collect(CollectVisitor &vis)
  {
    ForwardFaceCollection::Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    std::pair<Point,Point> bounds = find_bounds(coll);
    int numfaces = coll->NumFaces();
    delete grid;
    grid = new GridAccel(18,18,18,
		   bounds.first.x-10.0,bounds.second.x+10.0,
		   bounds.first.y-10.0,bounds.second.y+10.0,
		   bounds.first.z-10.0,bounds.second.z+10.0);

    bind_accel(coll,grid);
    //std::cout << "Generating lights..." << std::endl;
  
    delete cache;
    cache = new AreaCache;
    int sj = numfaces;
    for(int w=0;w<sj;w++)
      {
	Point p1 = coll->FacePoint(w,0);
	Point p2 = coll->FacePoint(w,1);
	Point p3 = coll->FacePoint(w,2);
	Point p4 = coll->FacePoint(w,3);
	cache->push_area(w,AreaTools::QuadArea(p1,p2,p3,p4));
      }
    cache->calc_sum(30000);

#if 0
    for(int q=0;q<split_count;q++) {
      ShadowCB *cb = new ShadowCB;
      cb->m_this = this;
      cb->current = q;
      async_pending_count++;
      emscripten_async_call(&shadow_color_callback, cb, -100*q);
    }
#else
    for(int q=0;q<split_count;q++)
      {
      ShadowCB *cb = new ShadowCB;
      cb->m_this = this;
      cb->current = q;
      shadow_color_callback(cb);
      }
#endif

  }
  void Prepare() {
#ifdef EMSCRIPTEN
    std::cout << "Warning, you should save the bitmaps created with p_tex_light to a file, upload to a web server and load them to textures via bm_url (p_tex_light is so slow that it shouldn't be used in release builds)" << std::endl;
#endif
    ForwardFaceCollection::Prepare();

    std::pair<Point,Point> bounds = find_bounds(coll);
    int numfaces = coll->NumFaces();
    delete grid;
    grid = new GridAccel(18,18,18,
		   bounds.first.x-10.0,bounds.second.x+10.0,
		   bounds.first.y-10.0,bounds.second.y+10.0,
		   bounds.first.z-10.0,bounds.second.z+10.0);

    bind_accel(coll,grid);
    //std::cout << "Generating lights..." << std::endl;
  
    delete cache;
    cache = new AreaCache;
    int sj = numfaces;
    for(int w=0;w<sj;w++)
      {
	Point p1 = coll->FacePoint(w,0);
	Point p2 = coll->FacePoint(w,1);
	Point p3 = coll->FacePoint(w,2);
	Point p4 = coll->FacePoint(w,3);
	cache->push_area(w,AreaTools::QuadArea(p1,p2,p3,p4));
      }
    cache->calc_sum(30000);

#if 0
    for(int q=0;q<split_count;q++) {
      ShadowCB *cb = new ShadowCB;
      cb->m_this = this;
      cb->current = q;
      async_pending_count++;
      emscripten_async_call(&shadow_color_callback, cb, -100*q);
    }
#else
    for(int q=0;q<split_count;q++)
      {
      ShadowCB *cb = new ShadowCB;
      cb->m_this = this;
      cb->current = q;
      shadow_color_callback(cb);
      }
#endif
  }
private:
  int split_count=10;
  FaceCollection *coll;
  int count;
  Vector light_dir;
  GridAccel *grid;
  AreaCache *cache;
};
#endif

 void shadow_color_callback(void *ptr)
 {
   //ShadowCB *cb = (ShadowCB*)ptr;
   //int q = cb->current;
   //cb->m_this->ASyncCallback(q);
 }

GameApi::P GameApi::PolygonApi::light_transport(P p, int num, float light_dir_x, float light_dir_y, float light_dir_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  Vector light_dir(light_dir_x, light_dir_y, light_dir_z);
  return add_polygon2(e, new ShadowColor(coll, num, light_dir),1);
#endif
}

class TextureFromP : public Bitmap<Color>
{
public:
  TextureFromP(FaceCollection *coll, int num) : coll(coll), num(num) { firsttime = true; }
  virtual int SizeX() const { return ref.width; }
  virtual int SizeY() const { return ref.height; }
  virtual Color Map(int x, int y) const
  {
    int w = ref.width;
    int h = ref.height;
    if (x<0||x>=w) return Color(0);
    if (y<0||y>=h) return Color(0);
    unsigned int c = ref.buffer[x+y*ref.ydelta];
    return Color(c);
  }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
      firsttime = false;
      //coll->Prepare();
      coll->GenTexture(num);
      ref = coll->TextureBuf(num);
    }
  }
  virtual void Prepare()
  {
    if (firsttime) {
      firsttime = false;
      coll->Prepare();
      coll->GenTexture(num);
      ref = coll->TextureBuf(num);
    }
  }
private:
  FaceCollection *coll;
  int num;
  BufferRef ref;
  bool firsttime;
};

GameApi::BM GameApi::PolygonApi::texture_from_p(P p, int num)
{
  FaceCollection *coll = find_facecoll(e,p);

  Bitmap<Color> *bbm = new TextureFromP(coll,num);
  BitmapColorHandle *handle = new BitmapColorHandle;
  handle->bm = bbm;
  BM bm2 = add_bitmap(e, handle);

  return bm2;
}

class NormalToTexCoord : public ForwardFaceCollection
{
public:
  NormalToTexCoord(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "NormalToTexCoord"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  bool has_texcoord() const { return true; }
  Point2d TexCoord(int face, int point) const {
    Vector n = ForwardFaceCollection::PointNormal(face,point);
    Point2d p;
    p.x = -n.dx;
    p.y = -n.dy;
    return p;
  }
  float TexCoord3(int face, int point) const
  {
    Vector n = ForwardFaceCollection::PointNormal(face,point);
    return n.dz;
  }
private:
  FaceCollection *coll;
};
 
GameApi::P GameApi::PolygonApi::from_normal_to_texcoord(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new NormalToTexCoord(coll),1);
}

class MeshAnimFromMeshes : public MeshAnim
{
public:
  MeshAnimFromMeshes(std::vector<FaceCollection*> coll, float start_time, float time_step) : coll(coll), start_time_2(start_time), time_step(time_step) {}

  void Collect(CollectVisitor &vis)
  {
    int s = coll.size();
    for(int i=0;i<s;i++) coll[i]->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare()
  {
    int s = coll.size();
    for(int i=0;i<s;i++) coll[i]->Prepare();
  }
  virtual int NumFaces() const { return coll[0]->NumFaces(); }
  virtual int NumPoints(int face) const { return coll[0]->NumPoints(face); }
  virtual float StartTime() const { return start_time_2; }
  virtual float EndTime() const { return start_time_2+coll.size()*time_step; }
  virtual Point Vertex(int face,int point, float time) const {
    int n = Num(time);
    int ss = coll.size();
    if (ss==0) return Point(0.0,0.0,0.0);
    if (ss==1) { return coll[0]->FacePoint(face,point); }
    if (n<0) {n=0; }
    if (n>=ss-1) { return coll[ss-1]->FacePoint(face,point); }
    float p = Pos(time);
    Point p1 = coll[n]->FacePoint(face,point);
    Point p2 = coll[n+1]->FacePoint(face,point);
    Point pp = Point::Interpolate(p1,p2,p);
    return pp;
  }
  virtual unsigned int Color(int face,int point, float time) const
  {
    int n = Num(time);
    int ss = coll.size();
    if (ss==0) return 0;
    if (ss==1) { return coll[0]->Color(face,point); }
    if (n<0) {n=0; }
    if (n>=ss-1) { return coll[ss-1]->Color(face,point);}
    float p = Pos(time);
    unsigned int p1 = coll[n]->Color(face,point);
    unsigned int p2 = coll[n+1]->Color(face,point);
    unsigned int pp = Color::Interpolate(p1,p2,p);
    return pp;

  }
  virtual Vector Normal(int face, int point, float time) const
  {
    int n = Num(time);
    int ss = coll.size();
    if (ss==0) return Vector(0.0,0.0,0.0);
    if (ss==1) { return coll[0]->PointNormal(face,point); }
    if (n<0) { n=0; }
    if (n>=ss-1) { return coll[ss-1]->PointNormal(face,point); }
    float p = Pos(time);
    Vector p1 = coll[n]->PointNormal(face,point);
    Vector p2 = coll[n+1]->PointNormal(face,point);
    Point pp = Point::Interpolate(Point(p1),Point(p2),p);
    return Vector(pp);

  }
  virtual Point2d TexCoord(int face, int point, float time) const
  {
    int n = Num(time);
    int ss = coll.size();
    if (ss==0) { Point2d p; return p; }
    if (ss==1) { return coll[0]->TexCoord(face,point); }
    if (n<0) { n=0; }
    if (n>=ss-1) { return coll[ss-1]->TexCoord(face,point); }
    float p = Pos(time);
    Point2d p1 = coll[n]->TexCoord(face,point);
    Point2d p2 = coll[n+1]->TexCoord(face,point);
    Point pp = Point::Interpolate(Point(p1.x,p1.y,0.0),Point(p2.x,p2.y,0.0),p);
    Point2d pp2 = { pp.x,pp.y };
    return pp2;
  }
  virtual float TexCoord3(int face, int point, float time) const
  {
    int n = Num(time);
    int ss = coll.size();
    if (ss==0) { return 0.0; }
    if (ss==1) { return coll[0]->TexCoord3(face,point); }
    if (n<0) { n=0; }
    if (n>=ss-1) { return coll[ss-1]->TexCoord3(face,point); }
    float p = Pos(time);
    float p1 = coll[n]->TexCoord3(face,point);
    float p2 = coll[n+1]->TexCoord3(face,point);
    return (1.0-p)*p1+p*p2;
  }

  int Num(float time) const
  {
    float a = time-start_time_2;
    a/=time_step;
    return int(a);
  }
  float Pos(float time) const // returns [0.0 .. 1.0]
  {
    float a = time-start_time_2;
    a-=Num(time)*time_step;
    a/=time_step;
    return a;
  }
private:
  std::vector<FaceCollection*> coll;
  float start_time_2;
  float time_step;
};

GameApi::MA GameApi::PolygonApi::meshanim(std::vector<P> vec, float start_time, float end_time)
{
  start_time/=10.0;
  end_time/=10.0;
  int s = vec.size();
  std::vector<FaceCollection*> res;
  for(int i=0;i<s;i++)
    {
      FaceCollection *coll = find_facecoll(e,vec[i]);
      res.push_back(coll);
    }
  float time_step = (end_time-start_time)/float(s);
  return add_mesh_anim(e, new MeshAnimFromMeshes(res, start_time, time_step));
}

class MeshFromMeshAnim : public FaceCollection
{
public:
  MeshFromMeshAnim(MeshAnim *anim, float time) : anim(anim), time(time) { }
  virtual std::string name() const { return "MeshFromMeshAnim"; }

  void Collect(CollectVisitor &vis)
  {
    anim->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { anim->Prepare(); }
  virtual int NumFaces() const { return anim->NumFaces(); }
  virtual int NumPoints(int face) const { return anim->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return anim->Vertex(face,point,time);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return anim->Normal(face,point,time);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const { return 0;}
  virtual unsigned int Color(int face, int point) const
  {
    return anim->Color(face,point,time);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return anim->TexCoord(face,point,time);
  }
  virtual float TexCoord3(int face, int point) const { 
    return anim->TexCoord3(face,point,time);
  }

private:
  MeshAnim *anim;
  float time;
};
GameApi::P GameApi::PolygonApi::meshanim_mesh(MA ma, float time)
{
  MeshAnim *anim = find_mesh_anim(e,ma);
  return add_polygon2(e, new MeshFromMeshAnim(anim, time),1);
}

class MeshFromMeshAnim2 : public FaceCollection
{
public:
  MeshFromMeshAnim2(MeshAnim *anim, float time1, float time2) : anim(anim), time1(time1), time2(time2) { }
  virtual std::string name() const { return "MeshFromMeshAnim2"; }
  void Collect(CollectVisitor &vis)
  {
    anim->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { anim->Prepare(); }
  virtual int NumFaces() const { return anim->NumFaces(); }
  virtual int NumPoints(int face) const { return anim->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    return anim->Vertex(face,point,time1);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return anim->Normal(face,point,time1);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const { return 0;}
  virtual unsigned int Color(int face, int point) const
  {
    return anim->Color(face,point,time1);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return anim->TexCoord(face,point,time1);
  }
  virtual float TexCoord3(int face, int point) const { 
    return anim->TexCoord3(face,point,time1);
  }



  virtual Point EndFacePoint(int face, int point) const { return anim->Vertex(face,point,time2); }
  virtual Vector EndPointNormal(int face, int point) const { return anim->Normal(face,point,time2); }
  virtual float EndAttrib(int face, int point, int id) const { return 0.0; }
  virtual int EndAttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int EndColor(int face, int point) const { return anim->Color(face,point,time2); }
  virtual Point2d EndTexCoord(int face, int point) const { return anim->TexCoord(face,point,time2); }
  virtual float EndTexCoord3(int face, int point) const { return anim->TexCoord3(face,point,time2); }

  
private:
  MeshAnim *anim;
  float time1, time2;
};
GameApi::P GameApi::PolygonApi::meshanim_mesh2(MA ma, float time1, float time2)
{
  MeshAnim *anim = find_mesh_anim(e,ma);
  return add_polygon2(e, new MeshFromMeshAnim2(anim, time1,time2),1);
}

GameApi::ARR GameApi::PolygonApi::meshanim_anim_meshes(MA ma, float start_time, float delta_time, int count)
{
  int s = count;
  std::vector<P> vec;
  for(int i=0;i<s;i++)
    {
      GameApi::P p = meshanim_mesh2(ma, start_time+i*delta_time, start_time+(i+1)*delta_time);
      vec.push_back(p);
    }
  ArrayType *array = new ArrayType;
  array->type = 1;
  int s2 = count;
  for(int i=0;i<s2;i++)
    {
      array->vec.push_back(vec[i].id);
    }
  return add_array(e,array);
}

#if 0
class ChooseTime : public MainLoopItem
{
public:
  ChooseTime(MainLoopItem *next, std::vector<MainLoopItem*> render, float delta_time) : next(next), render(render), delta_time(delta_time) { firsttime=true;}
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    int s = render.size();
    for(int i=0;i<s;i++) render[i]->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() {  next->Prepare(); 
    int s = render.size();
    for(int i=0;i<s;i++) render[i]->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime)
      {
	int s = render.size();
	for(int i=0;i<s;i++)
	  {
	    render[i]->execute(e);
	  }
	firsttime = false;
      }
    
    int s = render.size();
    for(int i=0;i<s;i++)
      {
	MainLoopEnv ee = e;
	float t = e.time - i*delta_time;
	if (t>=0.0 && t < delta_time)
	  {
	  ee.in_POS = t/delta_time;
	  render[i]->execute(ee);
	  }
      }
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { return next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }

private:
  MainLoopItem *next;
  std::vector<MainLoopItem*> render;
  float delta_time;
  bool firsttime;
};
#endif
GameApi::ML GameApi::PolygonApi::choose_time(ML next, std::vector<ML> vec, float delta_time)
{
#if 0
  MainLoopItem *nxt = find_main_loop(e,next);
  int s = vec.size();
  std::vector<MainLoopItem*> v;
  for(int i=0;i<s;i++)
    {
      MainLoopItem *item = find_main_loop(e, vec[i]);
      v.push_back(item);
    }
  return add_main_loop(e, new ChooseTime(nxt, v, delta_time));
#endif
}


GameApi::ML GameApi::PolygonApi::anim(EveryApi &ev, ML next, MA anim, float start_time, float end_time, int count)
{
  start_time/=10.0;
  end_time/=10.0;

  std::vector<P> vec;
  int s = count;
  float delta_time = (end_time-start_time)/float(count);
  for(int i=0;i<s;i++)
    {
      vec.push_back(meshanim_mesh2(anim, start_time+i*delta_time, start_time+(i+1)*delta_time));
    }
  std::vector<ML> vec2;
  int s2 = vec.size();
  for(int i=0;i<s2;i++)
    {
      vec2.push_back(render_vertex_array_ml2(ev, vec[i]));
    }
  ML ml = choose_time(next, vec2, delta_time);
  return ml;
}

class AnimBind : public MainLoopItem
{
public:
  AnimBind(GameApi::PolygonApi *poly, GameApi::Env &e, GameApi::EveryApi &ev, GameApi::ML next, GameApi::MA anim, GameApi::MT material, float start_time, float end_time, int count) : poly(poly), env(e), ev(ev), next(next), anim(anim), material(material), start_time(start_time), end_time(end_time), count(count) { firsttime = true; }

  void Collect(CollectVisitor &vis)
  {
    MainLoopItem *item = find_main_loop(env,next);
    item->Collect(vis);
    vis.register_obj(this);
  }
  
  void HeavyPrepare()
  {
    if (firsttime)
      {
	firsttime=false;
  start_time/=10.0;
  end_time/=10.0;

  std::vector<GameApi::P> vec;
  int s = count;
  float delta_time = (end_time-start_time)/float(count);
  for(int i=0;i<s;i++)
    {
      vec.push_back(poly->meshanim_mesh2(anim, start_time+i*delta_time, start_time+(i+1)*delta_time));
    }
  std::vector<GameApi::ML> vec2;
  int s2 = vec.size();
  Material *mat = find_material(env, material);
  for(int i=0;i<s2;i++)
    {
      int ml_id = mat->mat(vec[i].id);
      GameApi::ML ml;
      ml.id = ml_id;
      vec2.push_back(ml /*render_vertex_array_ml2(ev, vec[i])*/);
    }
  ml = poly->choose_time(next, vec2, delta_time);
      }
  }
  void Prepare()
  {
    MainLoopItem *item = find_main_loop(env,next);
    item->Prepare();
    HeavyPrepare();
  }

  void handle_event(MainLoopEvent &e)
  {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->handle_event(e);
    }
  }
  void execute(MainLoopEnv &e)
  {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->execute(e);
    }
  }
  std::vector<int> shader_id() {
    if (ml.id!=-1) {
      MainLoopItem *item = find_main_loop(env,ml);
      item->shader_id();
    }
    return std::vector<int>(); }

private:
  GameApi::PolygonApi *poly;
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::ML next;
  GameApi::MA anim;
  GameApi::MT material;
  float start_time, end_time;
  int count;
  GameApi::ML ml = { -1 };
  bool firsttime;
};

GameApi::ML GameApi::PolygonApi::anim_bind(EveryApi &ev, ML next, MA anim, MT material, float start_time, float end_time, int count)
{
  return add_main_loop(e, new AnimBind(this,e,ev,next,anim,material,start_time,end_time,count));
}

class ShadowMap : public ForwardFaceCollection
{
public:
  ShadowMap(FaceCollection *next, Point pos,int sx, int sy) : ForwardFaceCollection(*next), next(next), pos(pos),sx(sx),sy(sy) {}
  virtual std::string name() const { return "ShadowMap"; }

  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual int NumFaces() const { return next->NumFaces(); }
  virtual int NumPoints(int face) const { return next->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = next->FacePoint(face,point);
    return calc(p);
  }
  Point calc(Point p) const 
  {
    p-=pos;
    float r = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    float alfa = acos(p.z/r);
    float beta = atan2(p.y,p.x);
    beta+=3.14159;
    alfa/=3.14159;
    beta/=3.14159*2.0;
    alfa*=2.0*512.0;
    beta*=2.0*512.0;
    alfa-=512.0;
    beta-=512.0;
    //beta-=512.0;
    return Point(alfa,beta,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return next->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return next->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return next->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return next->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return next->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const { return next->TexCoord3(face,point); }

private:
  FaceCollection *next;
  Point pos;
  int sx, sy;
};

GameApi::BM GameApi::PolygonApi::shadow_map(EveryApi &ev, P p, float p_x, float p_y, float p_z, int sx, int sy)
{
  FaceCollection *coll = find_facecoll(e, p);
  FaceCollection *coll2 = new ShadowMap(coll, Point(p_x,p_y,p_z), sx,sy);
  GameApi::P p2 = add_polygon2(e, coll2, 1);

  SH I2=ev.shader_api.shader_choice(ev,0);
  BM I3=ev.polygon_api.renderpolytobitmap(ev,p2,I2,0,0,0,sx,sy);
  return I3;
}

class ShadowMap2 : public Bitmap<bool>
{
public:
  ShadowMap2(FaceCollection *objs, float p_x, float p_y, float p_z, int sx, int sy, FaceCollection *quad) : objs(objs), p_x(p_x), p_y(p_y), p_z(p_z), sx(sx), sy(sy), quad(quad) 
  { 

  }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual bool Map(int x, int y) const
  {
    float xx = float(x)/float(sx);
    float yy = float(y)/float(sy);
    Vector d_x = d41*(1.0-xx)+d32*xx;
    Vector d_y = d21*(1.0-yy)+d34*yy;
    Point pp = q1+d_x*yy + d_y*xx;
    int s = objs->NumFaces();
    Point pp2(p_x,p_y,p_z);
    LineProperties lp(pp,pp2);
    bool res = false;
    for(int i=0;i<s;i++) {
      Point p0 = objs->FacePoint(i,0);
      Point p1 = objs->FacePoint(i,1);
      Point p2 = objs->FacePoint(i,2);
      Point p3 = objs->FacePoint(i,3);
      float t=0.0;
      if (lp.QuadIntersection(p0,p1,p2,p3,t)) { res=true; break; }
    }
    return res;
  }
  void Collect(CollectVisitor &vis)
  {
    objs->Collect(vis);
    quad->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    q1 = quad->FacePoint(0,0);
    q2 = quad->FacePoint(0,1);
    q3 = quad->FacePoint(0,2);
    q4 = quad->FacePoint(0,3);
    d21 = q2-q1;
    d34 = q3-q4;
    d32 = q3-q2;
    d41 = q4-q1;
  }
  virtual void Prepare()
  {
    objs->Prepare();
    quad->Prepare();
    q1 = quad->FacePoint(0,0);
    q2 = quad->FacePoint(0,1);
    q3 = quad->FacePoint(0,2);
    q4 = quad->FacePoint(0,3);
    d21 = q2-q1;
    d34 = q3-q4;
    d32 = q3-q2;
    d41 = q4-q1;
  }
private:
  FaceCollection *objs;
  float p_x, p_y, p_z;
  Point q1,q2,q3,q4;
  Vector d21,d34,d32,d41;
  int sx, sy;
  FaceCollection *quad;
};

 GameApi::BB GameApi::PolygonApi::shadow_map2(P p, float p_x, float p_y, float p_z, int sx, int sy, P quad)
 {
    FaceCollection *objs = find_facecoll(e, p);
    FaceCollection *quad2 = find_facecoll(e,quad);
    return add_bool_bitmap(e, new ShadowMap2(objs,p_x,p_y,p_z,sx,sy, quad2));
  }

class ShadowMap3 : public FaceCollection
{
public:
  ShadowMap3(FaceCollection *objs, Point pos, int sx, int sy, FaceCollection *quad) : objs(objs), pos(pos), sx(sx), sy(sy), quad(quad) { }

  virtual bool has_normal() const { if (!objs) return false; return objs->has_normal(); }
  virtual bool has_attrib() const { if (!objs) return false;return objs->has_attrib(); }
  virtual bool has_color() const { if (!objs) return false; return objs->has_color(); }
  virtual bool has_texcoord() const { if (!objs) return false; return objs->has_texcoord(); }
  virtual bool has_skeleton() const { if (!objs) return false; return objs->has_skeleton(); }

  void Collect(CollectVisitor &vis)
  {
    objs->Collect(vis);
    quad->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() {objs->Prepare(); quad->Prepare(); }
  virtual int NumFaces() const { return objs->NumFaces(); }
  virtual int NumPoints(int face) const { return objs->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = objs->FacePoint(face,point);

    Point q1 = quad->FacePoint(0,0);
    Point q2 = quad->FacePoint(0,1);
    Point q3 = quad->FacePoint(0,2);
    //Point q4 = quad->FacePoint(0,3);

    Point pp(pos.x,pos.y,pos.z);

    Plane pl(q1,q2-q1,q3-q1);
    Point2d res;
    Vector pp1 = pp-p;
    Vector pp2 = pp1/pp1.Dist();
    Vector pp3 = pp2*1000.0;
    Point pp4 = Point(pp3);
    bool b = pl.LineSegmentIntersection(p,pp4,res);
    if (b) {
      return Point(res.x*sx,res.y*sy,0.0);
    }

    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const { return objs->PointNormal(face,point); }
  virtual float Attrib(int face, int point, int id) const { return objs->Attrib(face,point,id); }
  virtual int AttribI(int face, int point, int id) const { return objs->AttribI(face,point,id); }
  virtual unsigned int Color(int face, int point) const { return objs->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return objs->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return objs->TexCoord3(face,point); }
  
private:
  FaceCollection *objs;
  Point pos;
  int sx,sy;
  FaceCollection *quad;
};

GameApi::BM GameApi::PolygonApi::shadow_map3(EveryApi &ev, P objs,float p_x, float p_y, float p_z, int sx, int sy, P quad)
{
  FaceCollection *f_objs = find_facecoll(e, objs);
  //FaceCollection *f_quad = find_facecoll(e, quad);
  FaceCollection *res = f_objs; //new ShadowMap3(f_objs, Point(p_x,p_y,p_z), sx,sy, f_quad);
  GameApi::P p = add_polygon2(e, res,1);
  GameApi::MN mn = ev.move_api.mn_empty();
  GameApi::FML poly = ev.low_frame_api.low_poly_draw("aa", p, mn);
  //GameApi::FBU fbu = ev.low_frame_api.low_framebuffer(poly, 4, sx,sy,0);
  //GameApi::BM bm = ev.mainloop_api.framebuffer_bitmap(fbu);
  GameApi::BM bm = ev.low_frame_api.low_frame_bitmap(poly, sx,sy);
  return bm;
}

#if 0
class AddMeshTexture : public ForwardFaceCollection
{
public:
  AddMeshTexture(FaceCollection *coll, Bitmap<::Color> *bm) : ForwardFaceCollection(*coll), coll(coll), bm(bm), bbm(*bm) { 
    buf = BufferRef::NewBuffer(1,1);
    firsttime = true;
  }
  
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    bm->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
#ifndef THREADS
  bbm.Gen();
#else
  bbm.GenPrepare();

  int numthreads = 8;
  ThreadedUpdateTexture threads;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
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
      
      if (end_y-start_y > 0)
	ids.push_back(threads.push_thread(&bbm, start_x, end_x, start_y, end_y));
    }
  //int ss = ids.size();
  //for(int i=0;i<ss;i++)
  //{
  threads.join(0 /*ids[i]*/);
      //}
#endif

  //  bbm.Gen();
    
    buf = bbm.Buffer();
    }
  }
  void Prepare() {
    coll->Prepare();
    bm->Prepare();

    if (firsttime) {
    
#ifndef THREADS
  bbm.Gen();
#else
  bbm.GenPrepare();

  int numthreads = 8;
  ThreadedUpdateTexture threads;
  int sx = bm->SizeX();
  int sy = bm->SizeY();
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
      
      if (end_y-start_y > 0)
	ids.push_back(threads.push_thread(&bbm, start_x, end_x, start_y, end_y));
    }
  //int ss = ids.size();
  //for(int i=0;i<ss;i++)
  // {
  threads.join(0 /*ids[i]*/);
      //  }
#endif

  //  bbm.Gen();
    
    buf = bbm.Buffer();
    }
  }
  virtual int NumTextures() const { return coll->NumTextures()+1; }
  virtual void GenTexture(int num) { 
  }
  virtual BufferRef TextureBuf(int num) const { if (num<coll->NumTextures()) return coll->TextureBuf(num);
    
    return buf;
  }
  virtual int FaceTexture(int face) const { return coll->FaceTexture(face); }

private:
  FaceCollection *coll;
  Bitmap<::Color> *bm;
  BufferRef buf;
  BufferFromBitmap bbm;
  bool firsttime;
};
#endif

GameApi::P GameApi::PolygonApi::texture_add(P p, BM bm)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  BitmapHandle *handle = find_bitmap(e, bm);
  ::Bitmap<Color> *b2 = find_color_bitmap(handle);
  return add_polygon2(e, new AddMeshTexture(coll, b2),1);
#endif
}


#if 0
class RenderMeshAnim : public MainLoopItem
{
public:
  RenderMeshAnim(MainLoopItem *next, MeshAnim *anim, float delta_time) : next(next), anim(anim), delta_time(delta_time) { firsttime = true; }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      int s = int((anim->EndTime()-anim->StartTime())/delta_time);
      for(int i=0;i<s;i++)
	{
	  vec.push_back(new MeshFromMeshAnim(anim,anim->StartTime()+i*delta_time));
	}
      firsttime = false;
    }
		    
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    next->handle_event(e);
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  bool firsttime;
  MainLoopItem *next;
  MeshAnim *anim;
  float delta_time;
  std::vector<FaceCollection*> vec;
};
GameApi::ML GameApi::PolygonApi::render_meshanim(MA ma, float delta_time)
{
}

#endif

 void convert_stl_to_binary(std::vector<unsigned char> *ptr)
{
  std::cout << "STL file in ascii, converting to binary" << std::endl;
  std::vector<unsigned char> res;
  for(int i=0;i<80;i++)
    {
      res.push_back(0);
    }
  res.push_back(0);
  res.push_back(0);
  res.push_back(0);
  res.push_back(0);
  std::string s(ptr->begin(),ptr->end());
  std::stringstream ss(s);
  std::string line;
  int count =0;
  int facetcount = 0;
  int vertexcount=0;
  int v_count=0;
  while(std::getline(ss,line)) {
    std::stringstream ss2(line);
    std::string id;
    ss2>>id;
    if (id=="facet")
      {
	facetcount++;
	count++;
	//if (count%1==0) {std::cout << "f"; }
	std::string n; 
	float nx,ny,nz;
	ss2>>n >> nx >> ny >> nz;
	unsigned char *ptr = (unsigned char*)&nx;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);

	ptr = (unsigned char*)&ny;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);

	ptr = (unsigned char*)&nz;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);
      }
    if (id=="vertex") {
      vertexcount++;
      v_count++;
      //if (v_count%1==0) {std::cout << "v"; }
	float vx,vy,vz;
	ss2 >> vx >> vy >> vz;
	unsigned char *ptr = (unsigned char*)&vx;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);

	ptr = (unsigned char*)&vy;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);

	ptr = (unsigned char*)&vz;
	res.push_back(ptr[0]);
	res.push_back(ptr[1]);
	res.push_back(ptr[2]);
	res.push_back(ptr[3]);
    }
    if (id=="endfacet") {
      if (vertexcount!=3 ||facetcount!=1) { std::cout << "STL(ascii) file format problem" << std::endl; }
      //if (v_count%1==0) {std::cout << "v"; }
	res.push_back(0);
	res.push_back(0);
	vertexcount=0;
	facetcount=0;
      
    }
  }
  unsigned char *ptr2 = (unsigned char*)&count;
  res[80]=ptr2[0];
  res[81]=ptr2[1];
  res[82]=ptr2[2];
  res[83]=ptr2[3];
  *ptr = res;
    
}

class STLFaceCollection : public FaceCollection
{
public:
  STLFaceCollection(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url),homepage(homepage) { m_ptr=0; }
  virtual std::string name() const { return "STLFaceCollection"; }

  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void Prepare() {
    if (!m_ptr) {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    std::vector<unsigned char> *ptr2 = new std::vector<unsigned char>(ptr->begin(),ptr->end());
    if (!ptr) { std::cout << "stlfacecollection async not ready!" << std::endl; return; }
    if (ptr->size()<5) { std::cout << "STLFaceCollection: async not found!" << std::endl; std::cout << "ptr->size()=" << ptr->size() << "<5" << std::endl; return; }

    int i=0;
    while(ptr->operator[](i)==' '||ptr->operator[](i)=='\n'||ptr->operator[](i)=='\r') i++;
    if (ptr->operator[](i+0)=='s' && ptr->operator[](i+1)=='o' && ptr->operator[](i+2)=='l' && ptr->operator[](i+3)=='i' && ptr->operator[](i+4)=='d')
      {
	int s = ptr->size();
	bool b = false;
	for(int i=0;i<s;i++) {
	  unsigned char ch = ptr->operator[](i);
	  if ((ch&0x80)==0x80) { b=true; break; }
	}
	if (!b)
	  convert_stl_to_binary(ptr2);
      }
    if (ptr->operator[](0)=='<' && ptr->operator[](1)=='!' && ptr->operator[](2)=='D' && ptr->operator[](3)=='O' && ptr->operator[](4)=='C' && ptr->operator[](5)=='T') {
      std::cout << "404 error at STLFaceCollcection / " << url << std::endl;
      return;
    }
    //std::vector<unsigned char> *ptr2 = new std::vector<unsigned char>(ptr->begin(),ptr->end());
    m_ptr = ptr2;
    }
  }
  virtual int NumFaces() const { 
    int i = get_int(80);
    if (i<0 || i>3000000) return 0;
    return i; }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    int pos = 80+4+((3*4*4)+2)*face;
    int pos1 = 4*3;
    int num = point*4*3;
    float v1_x = get_float(pos+pos1+num);
    float v1_y = get_float(pos+pos1+num+4);
    float v1_z = get_float(pos+pos1+num+8);
    return Point(v1_x, v1_y, v1_z);
  }
  bool has_normal() const { return true; }
  virtual Vector PointNormal(int face, int point) const
  {
    int pos = 80+4+((3*4*4)+2)*face;
    int pos0 = 0;
    float vx = get_float(pos+pos0+0);
    float vy = get_float(pos+pos0+4);
    float vz = get_float(pos+pos0+8);
    return Vector(vx,vy,vz);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return false; }
  virtual unsigned int Color(int face, int point) const { return 0xffffffff; }
  bool has_texcoord() const { return true; }
  virtual Point2d TexCoord(int face, int point) const { 
    Point2d p;
    Point pp = FacePoint(face,point);
    pp.x-=-300.0;
    pp.x/=300.0-(-300.0);
    pp.y-=-300.0;
    pp.y/=300.0-(-300.0);
    p.x=pp.x;
    p.y=pp.y;
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }

  int get_int(int index) const {
    if (!m_ptr) return 0;
    if (index<0) return 0;
    int s = m_ptr->size();
    if (index+3>=s) return 0;
    unsigned char c1 = m_ptr->operator[](index);
    unsigned char c2 = m_ptr->operator[](index+1);
    unsigned char c3 = m_ptr->operator[](index+2);
    unsigned char c4 = m_ptr->operator[](index+3);
    unsigned char arr[] = { c1,c2,c3,c4 };
    int *ptr = (int*)arr;
    int val = *ptr;
    return val;
  }
  float get_float(int index) const {
    if (!m_ptr) return 0.0;
    if (index<0) return 0.0;
    int s = m_ptr->size();
    if (index+3>=s) return 0.0;
    unsigned char c1 = m_ptr->operator[](index);
    unsigned char c2 = m_ptr->operator[](index+1);
    unsigned char c3 = m_ptr->operator[](index+2);
    unsigned char c4 = m_ptr->operator[](index+3);
    unsigned char arr[] = { c1,c2,c3,c4 };
    float *ptr = (float*)arr;
    float val = *ptr;
    return val;
  }
private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  std::vector<unsigned char> *m_ptr;
};

void GameApi::PolygonApi::preparedone(P p)
{
  FaceCollection *coll = find_facecoll(e, p);
  coll->PrepareDone();
}

GameApi::P GameApi::PolygonApi::stl_load(std::string url)
{
  int c = get_current_block();
  set_current_block(-1);

  GameApi::P p = add_polygon2(e, new STLFaceCollection(e,url,gameapi_homepageurl),1);
  print_data(p);
  FaceCollection *coll = find_facecoll(e,p);
  GameApi::P p2 = add_polygon2(e, new PrepareCache(e,url,coll), 1);
  GameApi::P p3 = resize_to_correct_size(p2);
  set_current_block(c);
  return p3;
}

float det3(float *arr);

class FixVertexOrder : public ForwardFaceCollection
{
public:
  FixVertexOrder(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "FixVertexOrder"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
  }
  void HeavyPrepare() {
  }
  virtual void Prepare() { coll->Prepare(); }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const { return coll->FacePoint(face,map_point(face,point)); }
  virtual Vector PointNormal(int face, int point) const { return coll->PointNormal(face,map_point(face,point)); }
  virtual float Attrib(int face, int point, int id) const { return coll->Attrib(face,map_point(face,point),id); }
  virtual int AttribI(int face, int point, int id) const { return coll->AttribI(face,map_point(face,point),id); }
  virtual unsigned int Color(int face, int point) const { return coll->Color(face,map_point(face,point)); }
  virtual Point2d TexCoord(int face, int point) const { return coll->TexCoord(face,map_point(face,point)); }
  virtual float TexCoord3(int face, int point) const { return coll->TexCoord3(face,map_point(face,point)); }

  int map_point(int face, int point) const
  {
    if (is_clockwise(face)) return point;
    return coll->NumPoints(face)-point-1;
  }

  bool is_clockwise(int face) const
  {
    int s = coll->NumPoints(face);
    Point p1 = coll->FacePoint(face,0);
    Point p2 = coll->FacePoint(face,1%s);
    Point p3 = coll->FacePoint(face,2%s);

    p1.z-=400.0;
    p2.z-=400.0;
    p3.z-=400.0;


    float arr[] = { p1.x,p1.y,p1.z,
		    p2.x,p2.y,p2.z,
		    p3.x,p3.y,p3.z };
    float val = det3(arr);

    return val>0.0;
  }
private:
  FaceCollection *coll;
};
GameApi::P GameApi::PolygonApi::fix_vertex_order(GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new FixVertexOrder(coll),1);
}

class FilterInvisible : public ForwardFaceCollection
{
public:
  FilterInvisible(FaceCollection *coll, float size) : ForwardFaceCollection(*coll), coll(coll), size(size) { firsttime = true; }
  virtual std::string name() const { return "FilterInvisible"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
    indices = std::vector<int>();
    int s = coll->NumFaces();
    for(int i=0;i<s;i++)
      {
	float dist=0.0;
	int k = coll->NumPoints(i);
	for(int j=0;j<k;j++) { dist=std::max(dist,(coll->FacePoint(i,j)-coll->FacePoint(i,(j+1)%k)).Dist() ); }
	if (dist>size) {
	  indices.push_back(i);
	}
      }
    firsttime = false;
    }
  }
  void Prepare()
  {
    if (firsttime) {
    coll->Prepare();
    indices = std::vector<int>();
    int s = coll->NumFaces();
    for(int i=0;i<s;i++)
      {
	float dist=0.0;
	int k = coll->NumPoints(i);
	for(int j=0;j<k;j++) { dist=std::max(dist,(coll->FacePoint(i,j)-coll->FacePoint(i,(j+1)%k)).Dist() ); }
	if (dist>size) {
	  indices.push_back(i);
	}
      }
    firsttime = false;
    }
  }
  virtual int NumFaces() const { return indices.size(); }
  virtual int NumPoints(int face) const { return coll->NumPoints(indices[face]); }
  virtual Point FacePoint(int face, int point) const
  { return coll->FacePoint(indices[face],point); }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(indices[face],point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(indices[face],point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(indices[face],point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(indices[face],point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(indices[face],point);
  }
  virtual float TexCoord3(int face, int point) const {
    return coll->TexCoord3(indices[face],point);
  }
private:
  FaceCollection *coll;
  std::vector<int> indices;
  float size;
  bool firsttime;
};

 GameApi::P GameApi::PolygonApi::filter_invisible(GameApi::P p, float size)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new FilterInvisible(coll,size),1);
}


struct DT { std::string name; int value; };
std::vector<DT> g_lod;
pthread_mutex_t *g_lod_mutex =0;

void lod_lock()
{
  if (!g_lod_mutex) { g_lod_mutex=new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER); }
  pthread_mutex_lock(g_lod_mutex);
}
void lod_unlock()
{
  pthread_mutex_unlock(g_lod_mutex);
}

 class LodChoose : public ForwardFaceCollection
{
public:
  LodChoose(std::vector<FaceCollection*> vec, std::string name) : ForwardFaceCollection(*vec[0]), vec(vec),name2(name) { }
  virtual std::string name() const { return "LodChoose"; }

  int find_val(std::string name) const
  {
    lod_lock();
    int s = g_lod.size();
    for(int i=0;i<s;i++) {
      if (g_lod[i].name==name2) {
	int val = g_lod[i].value;
	int sk = vec.size();
	lod_unlock();
	if (val>=0 && val<sk) { return val; }
	return -1;
      }
    }
    lod_unlock();
    return -1;
  }
  void Collect(CollectVisitor &vis)
  {
    int pos = find_val(name2);
    if (pos!=-1)
      vec[pos]->Collect(vis);
    
  }
  void HeavyPrepare()
  {
  }
  
  virtual void Prepare()
  {
    int pos = find_val(name2);
    if (pos!=-1)
      vec[pos]->Prepare();
  }
  virtual int NumFaces() const 
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->NumFaces();
    return 0;
  }
  virtual int NumPoints(int face) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->NumPoints(face);
    return 0;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->FacePoint(face,point);
    Point p{0.0,0.0,0.0};
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->PointNormal(face,point);
    Vector p{0.0,0.0,0.0};
    return p;

  }
  virtual float Attrib(int face, int point, int id) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->Attrib(face,point,id);
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->AttribI(face,point,id);
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->Color(face,point);
    return 0x00000000;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->TexCoord(face,point);
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual float TexCoord3(int face, int point) const { 
    int pos = find_val(name2);
    if (pos!=-1)
      return vec[pos]->TexCoord3(face,point);
    return 0.0;
  }
private:
  std::vector<FaceCollection*> vec;
  std::string name2;
};

 GameApi::P GameApi::PolygonApi::lod_choose(std::vector<P> vec, std::string name)
{
  std::vector<FaceCollection*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    vec2.push_back(find_facecoll(e,vec[i]));
  }
  return add_polygon2(e, new LodChoose(vec2, name), 1);
}

class LodSet : public ForwardFaceCollection
{
public:
  LodSet(FaceCollection *coll, std::string name, int val) : ForwardFaceCollection(*coll), coll(coll),name2(name), val(val) { }
  virtual std::string name() const { return "LodSet"; }

  void Collect(CollectVisitor &vis)
  {
    add_name(name2,val);
    coll->Collect(vis);
    rem_name(name2);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { 
    add_name(name2,val);
    coll->Prepare();
    rem_name(name2);
  }
  virtual int NumFaces() const 
  {
    add_name(name2,val);
    int v = coll->NumFaces();
    rem_name(name2);
    return v;
  }
  virtual int NumPoints(int face) const
  {
    add_name(name2,val);
    int v = coll->NumPoints(face);
    rem_name(name2);
    return v;
  }
  virtual Point FacePoint(int face, int point) const
  {
    add_name(name2,val);
    Point v = coll->FacePoint(face,point);
    rem_name(name2);
    return v;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    add_name(name2,val);
    Vector v = coll->PointNormal(face,point);
    rem_name(name2);
    return v;
  }
  virtual float Attrib(int face, int point, int id) const
  {
    add_name(name2,val);
    float v = coll->Attrib(face,point,id);
    rem_name(name2);
    return v;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    add_name(name2,val);
    int v = coll->AttribI(face,point,id);
    rem_name(name2);
    return v;
  }
  virtual unsigned int Color(int face, int point) const
  {
    add_name(name2,val);
    unsigned int v = coll->Color(face,point);
    rem_name(name2);
    return v;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    add_name(name2,val);
    Point2d v = coll->TexCoord(face,point);
    rem_name(name2);
    return v;
  }
  virtual float TexCoord3(int face, int point) const { 
    add_name(name2,val);
    float v = coll->TexCoord3(face,point);
    rem_name(name2);
    return v;
  }

  void add_name(std::string name, int val) const {
    lod_lock();
    DT dt;
    dt.name = name;
    dt.value = val;
    g_lod.push_back(dt);
    lod_unlock();
  }
  void rem_name(std::string name) const {
    lod_lock();
    int s = g_lod.size();
    for(int i=0;i<s;i++)
      {
	if (g_lod[i].name==name) {
	  g_lod.erase(g_lod.begin()+i);
	  lod_unlock();
	  return;
	}
      }
    lod_unlock();
  }

private:
  FaceCollection *coll;
  std::string name2;
  int val;
};
 GameApi::P GameApi::PolygonApi::lod_set(P p, std::string name, int val)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new LodSet(coll, name, val),1);
}

class LodSelect : public Fetcher<int>
{
public:
  LodSelect(float start_dist, float dist_step, int max_value) : start_dist(start_dist), dist_step(dist_step), max_value(max_value) {}
  virtual void event(MainLoopEvent &e) { }
  virtual void frame(MainLoopEnv &e) { 
    current_in_MV = e.in_MV;
  }
  virtual void draw_event(FrameLoopEvent &e) { }
  virtual void draw_frame(DrawLoopEnv &e) { }
  virtual void set(int t) { }
  virtual int get() const
  {
    Point p{0.0,0.0,0.0};
    Point p2 = p*current_in_MV;
    float z = p2.z;
    if (z<start_dist) { return 0; }
    if (z>start_dist+dist_step*max_value) { return max_value; }
    int val = (z-start_dist)/dist_step;
    return val;
  }
private:
  float start_dist;
  float dist_step;
  int max_value;
  Matrix current_in_MV;
};
 GameApi::IF GameApi::PolygonApi::lod_select(float start_dist, float dist_step, int max_value)
{
  return add_int_fetcher(e, new LodSelect(start_dist, dist_step, max_value));
}


class TexCoord_subarea : public ForwardFaceCollection
{
public:
  TexCoord_subarea(FaceCollection *coll, float start_x, float end_x, float start_y, float end_y) : ForwardFaceCollection(*coll), coll(coll), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y) { }
public:
  virtual std::string name() const { return "TexCoord_subarea"; }

  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p = coll->TexCoord(face,point);
    p.x *= (end_x-start_x);
    p.y *= (end_y-start_y);
    p.x += start_x;
    p.y += start_y;
    return p;
  }
private:
  FaceCollection *coll;
  float start_x, end_x;
  float start_y, end_y;
};

 GameApi::P GameApi::PolygonApi::texcoord_subarea(P p, float start_x, float end_x,
						 float start_y, float end_y)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new TexCoord_subarea(coll, start_x, end_x, start_y, end_y),1);
}

class SkeletalDataFromFaceCollection : public SkeletalData
{
public:
  SkeletalDataFromFaceCollection(FaceCollection *coll, SkeletalData *bones) : coll(coll), bones(bones) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { coll->Prepare(); }
  int NumBones() const { return bones->NumBones(); }
  Point Bone(int i, bool is_first) const { return bones->Bone(i,is_first); }
  float BoneDeltaAngle(int i) const { return bones->BoneDeltaAngle(i); }
  
  float dist_from_bone(Point p, int bone) const
  {
      Point p1 = Bone(bone,false);
      Point p2 = Bone(bone,true);
      LineProperties pp(p1,p2);
      return pp.Dist(p);
  }
  std::pair<int,int> split_pt(int pt) const
  {
    int s = coll->NumFaces();
    int count = 0;
    int oldcount = 0;
    for(int i=0;i<s;i++) {
      oldcount = count;
      count+=coll->NumPoints(i);
      if (count>=pt) { return std::make_pair(i,pt-oldcount); }
    }
    return std::make_pair(-1,-1);
  }

  int NumPoints() const {
    int s = coll->NumFaces();
    int count = 0;
    for(int i=0;i<s;i++) count+=coll->NumPoints(i);
    return count;
  }
  int PointAttachBone(int pt) const
  {
    std::pair<int,int> p = split_pt(pt);
    Point p2 = coll->FacePoint(p.first, p.second);
    int s = NumBones();
    int min_value = 0;
    float min_dist = 6666666.0;
    for(int i=0;i<s;i++)
      {
	float d = dist_from_bone(p2, i);
	if (d<min_dist) { min_dist = d; min_value=i; }
      }
    return min_value;
  }
  float DistFromBone(int pt) const
  {
    std::pair<int,int> p = split_pt(pt);
    Point p2 = coll->FacePoint(p.first, p.second);
    return dist_from_bone(p2,PointAttachBone(pt));
  }
  float PosAtBone(int pt) const
  {
    std::pair<int,int> p = split_pt(pt);
    Point p2 = coll->FacePoint(p.first, p.second);
    int pos = PointAttachBone(pt);
    Point pp1 = Bone(pos,false);
    Point pp2 = Bone(pos,true);
    LineProperties prop(pp1,pp2);
    return prop.LineCoords(p2);
  }
  float BoneAngle(int pt) const
  {
    return 0.0;
  }
private:
  FaceCollection *coll;
  SkeletalData *bones;
};


struct ProgressMatData
{
  int counter;
  int s;
  int ii;
};

bool g_execute_shows_logo=false;
void progress_mat(void *data)
{
  ProgressMatData *dt = (ProgressMatData*)data;
  if (dt->counter==0)
    InstallProgress(145+dt->ii, "bind_inst_many", dt->s);
  dt->counter++;
  ProgressBar(145+dt->ii,dt->counter,dt->s, "bind_inst_many");
  if (dt->counter==dt->s) {
    //g_execute_shows_logo=false;
  }
}


GameApi::ML GameApi::PolygonApi::m_bind_inst_many(EveryApi &ev, std::vector<P> vec, std::vector<MT> materials, PTS pts, int max_ticks)
{
  static int ii=0;
  ii++;
  int s = std::min(vec.size(),materials.size());
  ProgressMatData *counter = new ProgressMatData;
  counter->counter=0;
  counter->s = s;
  counter->ii = ii;
  std::vector<ML> vec2;
  //std::cout << "m_bind_inst_many count=" << s << std::endl;
  for(int i=0;i<s;i++) {
    MT mat = materials[i];
    MT mat2 = ev.materials_api.progressmaterial(mat,&progress_mat,(void*)counter);
    vec2.push_back(ev.materials_api.bind_inst(vec[i], pts, mat));
  }
  //GameApi::ML ml = ev.mainloop_api.filter_execute_array_ml(ev,vec2);
  GameApi::ML ml = ev.mainloop_api.array_ml(ev,vec2);
  return ml;
}


GameApi::ML GameApi::PolygonApi::m_bind_many(EveryApi &ev, std::vector<P> vec, std::vector<MT> materials, int max_ticks)
{
  static int ii=0;
  ii++;
  int s = std::min(vec.size(),materials.size());
  ProgressMatData *counter = new ProgressMatData;
  counter->counter=0;
  counter->s = s;
  counter->ii = ii;
  std::vector<ML> vec2;
  //std::cout << "m_bind_inst_many count=" << s << std::endl;
  for(int i=0;i<s;i++) {
    MT mat = materials[i];
    MT mat2 = ev.materials_api.progressmaterial(mat,&progress_mat,(void*)counter);
    vec2.push_back(ev.materials_api.bind(vec[i], mat));
  }
  //GameApi::ML ml = ev.mainloop_api.filter_execute_array_ml(ev,vec2);
  GameApi::ML ml = ev.mainloop_api.array_ml(ev,vec2);
  return ml;
}



 class SceneDesc : public MainLoopItem
 {
public:
   SceneDesc(GameApi::Env &env, GameApi::EveryApi &ev, std::string url, std::string homepage, int sx, int sz) : env(env), ev(ev), url(url), homepage(homepage),sx(sx),sz(sz) { firsttime = true; scene.id = 0; }
   void Collect(CollectVisitor &vis)
   {
     vis.register_obj(this);
   }
   void HeavyPrepare() { Prepare(); }
   void Prepare() {
     if (scene.id==0) {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
    if (!vec) { std::cout << "scenedesc async not ready!" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    std::stringstream ss(code);
    std::string url2;
    int pos_x=1;
    int pos_y=1;
    std::vector<GameApi::ML> vec2;
    while(ss >> pos_x >> pos_y >> url2) {
      GameApi::ML script = ev.mainloop_api.load_ML_script(ev, url2, "", "", "", "", "");
      GameApi::MN def = ev.move_api.mn_empty();
      GameApi::MN mn = ev.move_api.trans2(def, pos_x*sx, 0.0, pos_y*sz);
      GameApi::ML move = ev.move_api.move_ml(ev, script, mn, 1, 10.0);
      vec2.push_back(move);
    }
    scene = ev.mainloop_api.array_ml(ev,vec2);
     }
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      //Prepare();
      firsttime = false;
    }
    MainLoopItem *item = find_main_loop(env, scene);
    item->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    if (firsttime) {
      Prepare();
      firsttime = false;
    }
    MainLoopItem *item = find_main_loop(env, scene);
    item->handle_event(e);
  }
   virtual std::vector<int> shader_id() { 
    if (firsttime) {
      Prepare();
      firsttime = false;
    }
    MainLoopItem *item = find_main_loop(env, scene);
    return item->shader_id();
  }

  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::string url;
  std::string homepage;
  GameApi::ML scene;
  bool firsttime;
  int sx,sz;
};
 
GameApi::ML GameApi::PolygonApi::load_scene(GameApi::EveryApi &ev, std::string url, int sx, int sy)
{
  return add_main_loop(e, new SceneDesc(e,ev,url, gameapi_homepageurl, sx,sy));
}

class PTSTOVoxel : public VoxelArray
{
public:
  PTSTOVoxel(PointsApiPoints *points, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy, int sz) : points(points), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z), sx(sx),sy(sy),sz(sz) { }
  void Collect(CollectVisitor &vis)
  {
    points->Collect(vis);
  }
  void HeavyPrepare() { }
  void Prepare() { points->Prepare(); }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual int SizeZ() const { return sz; }
  virtual int Size() const { return points->NumPoints(); }
  virtual unsigned int Color(int i) const
  {
    return points->Color(i);
  }
  virtual int State(int i) const { return 0; }
  virtual VoxelArray::Pos Map(int i) const
  {
    Point p = points->Pos(i);
#if 0
    p.x-=sx*(-start_x);
    p.y-=sy*(-start_y);
    p.z-=sz*(-start_z);
    p.x/=sx;
    p.y/=sy;
    p.z/=sz;
#endif
    p.x-=start_x;
    p.y-=start_y;
    p.z-=start_z;
    p.x/=(end_x-start_x);
    p.y/=(end_y-start_y);
    p.z/=(end_z-start_z);
    p.x*=sx;
    p.y*=sy;
    p.z*=sz;

    VoxelArray::Pos pp;
    pp.x = (int)p.x;
    pp.y = (int)p.y;
    pp.z = (int)p.z;
    return pp;
  }
private:
  PointsApiPoints *points;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  int sx,sy,sz;
};

GameApi::AV GameApi::PolygonApi::pts_to_voxel(PTS pts, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, int sx, int sy, int sz)
{
  PointsApiPoints *points = find_pointsapi_points(e, pts);
  return add_voxel_array(e, new PTSTOVoxel(points, start_x, end_x, start_y, end_y, start_z, end_z, sx,sy,sz));
}

class VoxelArrayToPTS : public PointsApiPoints
{
public:
  VoxelArrayToPTS(VoxelArray *arr, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) : arr(arr), start_x(start_x), end_x(end_x), start_y(start_y), end_y(end_y), start_z(start_z), end_z(end_z) { }
  void Collect(CollectVisitor &vis)
  {
    arr->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    sx = arr->SizeX();
    sy = arr->SizeY();
    sz = arr->SizeZ(); 
  }
  virtual void Prepare() { 
    arr->Prepare(); 

    sx = arr->SizeX();
    sy = arr->SizeY();
    sz = arr->SizeZ(); 
  }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return arr->Size();
  }
  virtual Point Pos(int i) const
  {
    VoxelArray::Pos p = arr->Map(i);
    int x = p.x;
    int y = p.y;
    int z = p.z;
    float px = start_x+float(x)/float(sx)*(end_x-start_x);
    float py = start_y+float(y)/float(sy)*(end_y-start_y);
    float pz = start_z+float(z)/float(sz)*(end_z-start_z);
    return Point(px,py,pz);
  }
  virtual unsigned int Color(int i) const
  {
    unsigned int c = arr->Color(i);
    return c;
  }
private:
  VoxelArray *arr;
  int start_x, end_x;
  int start_y, end_y;
  int start_z, end_z;
  int sx,sy,sz;
};

GameApi::PTS GameApi::PolygonApi::voxelarray_to_pts(AV arr, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z)
{
  VoxelArray *arr2 = find_voxel_array(e, arr);
  return add_points_api_points(e, new VoxelArrayToPTS(arr2, start_x, end_x, start_y, end_y, start_z, end_z));
}

struct PosColor {
  VoxelArray::Pos pos;
  unsigned int color;
};
bool Compare_Voxel(const PosColor &c1, const PosColor &c2)
{
  if (c1.pos.x!=c2.pos.x) return c1.pos.x<c2.pos.x;
  if (c1.pos.y!=c2.pos.y) return c1.pos.y<c2.pos.y;
  return c1.pos.z<c2.pos.z;
}
bool Compare_Voxel_Eq(const PosColor &c1, const PosColor &c2)
{
  return c1.pos.x==c2.pos.x && c1.pos.y==c2.pos.y && c1.pos.z==c2.pos.z;
}
class AV_Unique : public VoxelArray
{
public:
  AV_Unique(VoxelArray *arr) : arr(arr) { }
  void Collect(CollectVisitor &vis)
  {
    arr->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    int s = arr->Size();
    for(int i=0;i<s;i++) {
      VoxelArray::Pos p = arr->Map(i);
      unsigned int c = arr->Color(i);
      PosColor cc;
      cc.pos = p;
      cc.color = c;
      vec.push_back(cc);
    }
    std::sort(vec.begin(), vec.end(), &Compare_Voxel);
    auto last = std::unique(vec.begin(), vec.end(), &Compare_Voxel_Eq);
    vec.erase(last, vec.end());

  }
  void Prepare() {
    arr->Prepare();

    int s = arr->Size();
    for(int i=0;i<s;i++) {
      VoxelArray::Pos p = arr->Map(i);
      unsigned int c = arr->Color(i);
      PosColor cc;
      cc.pos = p;
      cc.color = c;
      vec.push_back(cc);
    }
    std::sort(vec.begin(), vec.end(), &Compare_Voxel);
    auto last = std::unique(vec.begin(), vec.end(), &Compare_Voxel_Eq);
    vec.erase(last, vec.end());
  }
  virtual int SizeX() const { return arr->SizeX(); }
  virtual int SizeY() const { return arr->SizeY(); }
  virtual int SizeZ() const { return arr->SizeZ(); }
  virtual int Size() const
  {
    return vec.size();
  }
  
  virtual VoxelArray::Pos Map(int i) const
  {
    return vec[i].pos;
  }
  virtual unsigned int Color(int i) const { return vec[i].color; }
  virtual int State(int i) const { return 0; }

private:
  VoxelArray *arr;
  std::vector<PosColor> vec;
};

GameApi::AV GameApi::PolygonApi::av_unique(AV arr)
{
  VoxelArray *va = find_voxel_array(e, arr);
  return add_voxel_array(e, new AV_Unique(va));
}

class NormalDarkness : public ForwardFaceCollection
{
public:
  NormalDarkness(FaceCollection *coll, float dark) : ForwardFaceCollection(*coll), coll(coll),dark(dark) { }
  virtual std::string name() const { return "NormalDarkness"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual unsigned int Color(int face, int point) const
  {
    Vector n = coll->PointNormal(face,point);
    n/=n.Dist();
    float val;
    if (n.dy>0.0) {
      float val2 = dark*n.dy;
      if (val2>1.0) val2=1.0;
      if (val2<0.0) val2=0.0;
      val = 1.0 - val2;
    } else {
       val = 1.0;
    }
    ::Color c(coll->Color(face,point));
    c.r*=val;
    c.g*=val;
    c.b*=val;
    return c.Pixel();
  }

private:
  FaceCollection *coll;
  float dark;
};

GameApi::P GameApi::PolygonApi::normal_darkness(P p, float dark)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new NormalDarkness(coll, dark),1);
}


class GradientColor : public ForwardFaceCollection
{
public:
  GradientColor(FaceCollection *coll, Point pt, Vector v, unsigned int start_color, unsigned int end_color) : ForwardFaceCollection(*coll), coll(coll), pt(pt), v(v), start_color(start_color), end_color(end_color) { }
  virtual std::string name() const { return "GradientColor"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual unsigned int Color(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    p-=Vector(pt);
    float val = Vector::DotProduct(Vector(p), v);
    val /= v.Dist()*v.Dist();
    if (val<0.0) val=0.0;
    if (val>1.0) val=1.0;
    return Color::Interpolate(start_color, end_color, val);
  }
private:
  FaceCollection *coll;
  Point pt;
  Vector v;
  unsigned int start_color, end_color;
};

GameApi::P GameApi::PolygonApi::gradient_color(P p, float p_x, float p_y, float p_z, float v_x, float v_y, float v_z, unsigned int start_color, unsigned int end_color)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new GradientColor(coll, Point(p_x,p_y,p_z), Vector(v_x,v_y,v_z), start_color, end_color),1);
}


class LinesFaceCollection : public FaceCollection
{
public:
  LinesFaceCollection(LineCollection *li, float width) : li(li), width(width) { }
  void Collect(CollectVisitor &vis)
  {
    li->Collect(vis);
  }

  virtual std::string name() const { return "LinesFaceCollection"; }

  
  void HeavyPrepare() { }
  virtual void Prepare() { li->Prepare(); }
  virtual int NumFaces() const { return li->NumLines(); }
  virtual int NumPoints(int face) const 
  {
    return 4;
  }
  virtual Point FacePoint(int face, int point) const
  {
    Point p1 = li->LinePoint(face,0);
    Point p2 = li->LinePoint(face,1);
    Vector v=p2-p1;
    Vector v2=v;
    std::swap(v.dx,v.dy);
    v/=v.Dist();
    v*=width/2.0;
    v2/=v2.Dist();
    v2*=width/2.0;
    if (point==0) return p1-v2-v;
    if (point==1) return p1-v2+v;
    if (point==2) return p2+v2+v;
    if (point==3) return p2+v2-v;
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return Vector(0.0,0.0,-1.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return true; }
  virtual unsigned int Color(int face, int point) const
  {
    if (point==0||point==1) return li->LineColor(face,0);
    return li->LineColor(face,1);
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p; p.x=0.0; p.y=0.0; return p; }
  virtual float TexCoord3(int face, int point) const { return 0.0; }

private:
  LineCollection *li;
  float width;
};

GameApi::P GameApi::PolygonApi::li_polygon(LI li, float width)
{
  LineCollection *coll = find_line_array(e, li);
  return add_polygon2(e, new LinesFaceCollection(coll, width),1);
}

class MixMesh : public ForwardFaceCollection
{
public:
  MixMesh(FaceCollection *coll, PointsApiPoints *pts, float val) : ForwardFaceCollection(*coll), coll(coll), pts(pts), val(val) { }
  virtual std::string name() const { return "MixMesh"; }

  void Collect(CollectVisitor &vis) { }
  void HeavyPrepare() { }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    int s = pts->NumPoints();
    float dist = 3000000.0;
    Point p3(0.0,0.0,0.0);
    for(int i=0;i<s;i++) {
      Point p2 = pts->Pos(i);
      Vector v = p2-p;
      float dist2 = sqrt(v.dx*v.dx+v.dy*v.dy+v.dz*v.dz);
      if (dist2<dist) { p3 = p2; dist = dist2; }
    }
    return Point::Interpolate(p,p3,val);
  }  
private:
  FaceCollection *coll;
  PointsApiPoints *pts;
  float val;
};


GameApi::P GameApi::PolygonApi::mix_mesh(P p, PTS points, float val)
{
  FaceCollection *coll = find_facecoll(e, p);
  PointsApiPoints *pts = find_pointsapi_points(e, points);
  return add_polygon2(e, new MixMesh(coll, pts, val), 1);
}

struct Anim_Struct
{
  int key; // key event to start this sequence, or -1
  int state; // state or -1
  float start_time;
  float end_time;
  float repeat;
  GameApi::P pair;
  GameApi::MN move;
  GameApi::MT material;
  GameApi::MS inst;
  GameApi::IF var;
};

class MeshAnimCore : public MainLoopItem
{
public:
  MeshAnimCore(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<Anim_Struct> vec) : env(env), ev(ev), vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  
  virtual void Prepare()
  {
    int s = vec.size();
    int current_key = -1;
    int current_state = -1;
    for(int i=0;i<s;i++) {
      GameApi::P p = vec[i].pair;
      GameApi::FF val;
      if (vec[i].state == -1 && current_state==-1) {
       if (vec[i].key == -1 && current_key==-1) {
         val = ev.font_api.time_range_fetcher(vec[i].start_time, vec[i].end_time, -10000.0, 0.0, 1.0, -100000.0, vec[i].repeat);
       }
       else {
         if (vec[i].key!=-1)
           current_key = vec[i].key;
         val = ev.font_api.time_range_fetcher_key(current_key,vec[i].start_time, vec[i].end_time, -10000.0,0.0,1.0,-10000.0, vec[i].repeat);
       }
      } else { // state
       if (vec[i].state!=-1)
         current_state = vec[i].state;

       if (vec[i].key==-1 && current_key==-1) {
         val = ev.font_api.time_range_fetcher_state(current_state, vec[i].var, vec[i].start_time, vec[i].end_time, -10000.0,0.0,1.0,-10000.0, vec[i].repeat);
       } else {
         if (vec[i].key!=-1)
           current_key = vec[i].key;
         val = ev.font_api.time_range_fetcher_state_key(current_state, current_key, vec[i].var, vec[i].start_time, vec[i].end_time, -10000.0, 0.0, 1.0, -10000.0, vec[i].repeat);
       }
      }
      GameApi::MN mn = vec[i].move;
      GameApi::MT mat = vec[i].material;
      GameApi::MS inst = vec[i].inst;
      GameApi::ML ml = ev.polygon_api.mesh_anim_display_inst(ev,p,val,mn,mat,inst);
      mls.push_back(ml);
      MainLoopItem *item = find_main_loop(env,ml);
      item->Prepare();
    }
  }
  virtual void execute(MainLoopEnv &e) {
    int s = mls.size();
    for(int i=0;i<s;i++) {
      GameApi::ML ml = mls[i];
      MainLoopItem *item = find_main_loop(env,ml);      
      item->execute(e);
    }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = mls.size();
    for(int i=0;i<s;i++) {
      GameApi::ML ml = mls[i];
      MainLoopItem *item = find_main_loop(env,ml);      
      item->handle_event(e);
    }
  }
  virtual std::vector<int> shader_id() {
    std::vector<int> vec;
    int s = mls.size();
    for(int i=0;i<s;i++) {
      GameApi::ML ml = mls[i];
      MainLoopItem *item = find_main_loop(env,ml);
      std::vector<int> ids = item->shader_id();
      int ss = ids.size();
      for(int j=0;j<ss;j++) vec.push_back(ids[j]);
    }
    return vec;
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<Anim_Struct> vec;
  std::vector<GameApi::ML> mls;
};

GameApi::ML mesh_anim(GameApi::Env &e, GameApi::EveryApi &ev, std::vector<Anim_Struct> vec2)
{
  return add_main_loop(e, new MeshAnimCore(e,ev,vec2));
}

class MeshAnimFromUrl : public MainLoopItem
{
public:
  MeshAnimFromUrl(GameApi::Env &env,
                 GameApi::EveryApi &ev,
                 std::vector<GameApi::P> faces,
                 std::vector<GameApi::MN> move,
                 std::vector<GameApi::MT> materials,
                 std::vector<GameApi::MS> instantiates,
                 std::vector<GameApi::IF> states,
                 std::string url, std::string homepage)
    : env(env), ev(ev), faces(faces), move(move), materials(materials), instantiates(instantiates), states(states), url(url), homepage(homepage) {
    ml.id = -1;
    current_states.id = -1;
  }
  
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare() { Prepare(); }
  virtual void Prepare()
  {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
    if (!vec) { std::cout << "meshanimfromurl async not ready!" << std::endl; return; }
    std::string s3(vec->begin(), vec->end());
    std::stringstream ss(s3);
    int line_num = 0;
    std::string line;
    while(std::getline(ss,line)) {
      line_num++;
      std::stringstream ss2(line);
      std::string word;
      ss2 >> word;
      std::string tmp,tmp2,tmp3;
      if (word=="var") {
	ss2>>tmp;
	statesnames.push_back(tmp);
      }
      if (word=="mesh") {
	ss2>> tmp;
	facenames.push_back(tmp);
      }
      if (word=="move") {
	ss2 >> tmp;
	movenames.push_back(tmp);
      }
      if (word=="material") {
	ss2 >> tmp;
	materialnames.push_back(tmp);
      }
      if (word=="inst") {
	ss2 >> tmp;
	instnames.push_back(tmp);
      }
      if (word=="pair") {
	ss2 >> tmp;
	ss2 >> tmp2;
	ss2 >> tmp3;
	GameApi::P p1,p2;
	p1.id = -1;
	p2.id = -1;
	int s = std::min(facenames.size(),faces.size());
	for(int i=0;i<s;i++) {
	  if (tmp2 == facenames[i]) p1 = faces[i];
	  if (tmp3 == facenames[i]) p2 = faces[i];
	}
	if (p1.id==-1||p2.id==-1) std::cout << "Warning: pair " << tmp << " " << tmp2 << " " << tmp3 << " not found in line " << line_num << std::endl;
	pairnames.push_back(tmp);
	pairs.push_back(std::make_pair(p1,p2));
      }
      if (word=="state") {
	ss2 >> tmp;
	std::string statesname;
	ss2 >> statesname;
	int statenum;
	ss2 >> statenum;
	float repeattime;
	ss2 >> repeattime;
	current_states.id = -1;
	current_repeattime = repeattime;
	
	  int psa = std::min(statesnames.size(),states.size());
	  for(int i=0;i<psa;i++) {
	    std::string sname = statesnames[i];
	    if (statesname==sname) current_states = states[i];
	  }
	  state = statenum;
	
	current_time=0.0;
      }
      if (word=="key") {
	ss2 >> tmp;
	std::stringstream ss3(tmp);
	ss3 >> key;

	float repeattime;
	ss2 >> repeattime;
	current_repeattime = repeattime;
	current_time = 0.0;
      }
      if (word=="slot") {
	float duration = 10.0;

	GameApi::P pair;
	GameApi::MN mv;
	GameApi::MT mat;
	GameApi::MS inst;
	ss2 >> duration;
	std::string name;
	while(ss2 >> name) {
	  int ps = std::min(pairnames.size(),pairs.size());
	  for(int i=0;i<ps;i++) {
	    std::string pname = pairnames[i];
	    if (name == pname) pair = ev.polygon_api.mesh_elem(pairs[i].first,pairs[i].second);
	  }
	  int psa = std::min(facenames.size(),faces.size());
	  for(int i=0;i<psa;i++) {
	    std::string fname = facenames[i];
	    if (name==fname) pair = faces[i];
	  }
	  int ps2 = std::min(movenames.size(),move.size());
	  for(int i=0;i<ps2;i++) {
	    std::string mname = movenames[i];
	    if (name==mname) mv=move[i];
	  }
	  int ps3 = std::min(materialnames.size(),materials.size());
	  for(int i=0;i<ps3;i++) {
	    std::string matname = materialnames[i];
	    if (name==matname) mat=materials[i];
	  }
	  int ps4 = std::min(instnames.size(),instantiates.size());
	  for(int i=0;i<ps4;i++) {
	    std::string iname = instnames[i];
	    if (name==iname) inst=instantiates[i];
	  }
	}
	
	Anim_Struct str;
	str.key = key;
	str.state = state;
	str.start_time = current_time;
	str.end_time = current_time + duration;
	str.pair = pair;
	str.move = mv;
	str.material = mat;
	str.inst = inst;
	str.var = current_states;
	str.repeat = current_repeattime;
	current_time += duration;
	animations.push_back(str);
	key = -1;
      }
    }
    ml = mesh_anim(env,ev,animations);
    MainLoopItem *item = find_main_loop(env,ml);
    item->Prepare();
     }

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
    } else return std::vector<int>();
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<GameApi::P> faces;
  std::vector<GameApi::MN> move;
  std::vector<GameApi::MT> materials;
  std::vector<GameApi::MS> instantiates;
  std::vector<GameApi::IF> states;
  std::vector<std::pair<GameApi::P,GameApi::P> > pairs;
  std::vector<Anim_Struct> animations;
  std::vector<std::string> facenames;
  std::vector<std::string> movenames;
  std::vector<std::string> materialnames;
  std::vector<std::string> instnames;
  std::vector<std::string> pairnames;
  std::vector<std::string> statesnames;
  int key=-1;
  int state=-1;
  GameApi::IF current_states;
  float current_repeattime=1000000;
  float current_time = 0.0;
  std::string url, homepage;
  GameApi::ML ml;
};




GameApi::ML GameApi::PolygonApi::mesh_anim(GameApi::EveryApi &ev,
					   std::vector<GameApi::P> vec,
					   std::vector<GameApi::MN> move,
					   std::vector<GameApi::MT> materials,
					   std::vector<GameApi::MS> inst,
					   std::vector<GameApi::IF> states,
					   std::string url)
{

  return add_main_loop(e, new MeshAnimFromUrl(e, ev, vec, move, materials, inst, states,url, gameapi_homepageurl));

}

// note, needs to have equal amount of vertices. p_script is good way to generate these

class MeshElem : public FaceCollection
{
public:
  MeshElem(FaceCollection *face1, FaceCollection *face2) : face1(face1), face2(face2) { }
  virtual std::string name() const { return "MeshElem"; }

  void Collect(CollectVisitor &vis)
  {
    face1->Collect(vis);
    face2->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { face1->Prepare(); face2->Prepare(); }
  virtual int NumFaces() const { return std::min(face1->NumFaces(),face2->NumFaces()); }
  virtual int NumPoints(int face) const { return std::min(face1->NumPoints(face),face2->NumPoints(face)); }
  virtual Point FacePoint(int face, int point) const
  {
    return face1->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return face1->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return face1->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
    {
      return face1->AttribI(face,point,id);
    }
  virtual unsigned int Color(int face, int point) const
  {
    return face1->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return face1->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const {
    return face1->TexCoord3(face,point);
  }

  virtual Point EndFacePoint(int face, int point) const { return face2->FacePoint(face, point); }
  virtual Vector EndPointNormal(int face, int point) const { return face2->PointNormal(face,point); }
  virtual float EndAttrib(int face, int point, int id) const { return face2->Attrib(face, point, id); }
  virtual int EndAttribI(int face, int point, int id) const { return face2->AttribI(face,point,id); }
  virtual unsigned int EndColor(int face, int point) const { return face2->Color(face,point); }
  virtual Point2d EndTexCoord(int face, int point) const { return face2->TexCoord(face,point); }
  virtual float EndTexCoord3(int face, int point) const { return face2->TexCoord3(face,point); }

private:
  FaceCollection *face1;
  FaceCollection *face2;
};
GameApi::P GameApi::PolygonApi::mesh_elem(P start, P end)
{
  FaceCollection *face1 = find_facecoll(e,start);
  FaceCollection *face2 = find_facecoll(e,end);
  return add_polygon2(e, new MeshElem(face1,face2),1);
}

class MeshAnimDisplayInst : public MainLoopItem
{
public:
  MeshAnimDisplayInst(GameApi::Env &env,GameApi::EveryApi &ev, FaceCollection *coll,
		      Fetcher<float> *fetch,
		      Movement *move,
		      Material *mat,
		      MatrixArray *inst) : env(env), ev(ev), coll(coll), fetch(fetch), move(move), mat(mat), inst(inst) { mainloopitem = 0; }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (!prepare_done) {
      GameApi::P p = add_polygon2(env, coll, 1);
      GameApi::MS ms = add_matrix_array(env, inst);
      GameApi::ML ml;
      ml.id = mat->mat_inst_matrix(p.id, ms.id);
      mainloopitem = find_main_loop(env,ml);
      mainloopitem->Prepare();
      // std::cout << "Mainloopitem = " << mainloopitem << std::endl;
      prepare_done = true;
    }
  }
  virtual void Prepare()
  {
    if (!prepare_done) {
      coll->Prepare();
      GameApi::P p = add_polygon2(env, coll, 1);
      GameApi::MS ms = add_matrix_array(env, inst);
      GameApi::ML ml;
      ml.id = mat->mat_inst_matrix(p.id, ms.id);
      mainloopitem = find_main_loop(env,ml);
      mainloopitem->Prepare();
      // std::cout << "Mainloopitem = " << mainloopitem << std::endl;
      prepare_done = true;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    //ee.time = fmod(e.time, repeat_time/10.0);
    fetch->frame(ee);
    float mix_val = fetch->get(); // mix value to be used in animation
    //std::cout << "mix_val=" << mix_val << std::endl;
    // TODO, do we need *10.0 for delta_time?
    if (mix_val>=0.0f && mix_val<=1.0f) {
      if (move) 
      move->frame(ee);
      Matrix m = Matrix::Identity();
      if (move)
	m = move->get_whole_matrix(e.time*10.0,ev.mainloop_api.get_delta_time());
      //std::cout << m << std::endl;
      MainLoopEnv ee = e;
      ee.in_MV = e.in_MV * m;
      ee.in_POS = mix_val;
      //std::cout << mainloopitem << std::endl;
      if (mainloopitem) {
	std::vector<int> vec = mainloopitem->shader_id();
	int s = vec.size();
	for(int i=0;i<s;i++)
	  {
	    int sh_id = vec[i];
	    GameApi::SH sh;
	    sh.id = sh_id;
	    ev.shader_api.use(sh);
	    ev.shader_api.set_var(sh, "in_POS", mix_val);
	  }
	mainloopitem->execute(ee);
      }
    }
    
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    fetch->event(e);
    //float mix_val = fetch->get(); // mix value to be used in animation
    if (move)
      move->event(e);
    if (mainloopitem)
      mainloopitem->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    if (mainloopitem)
      return mainloopitem->shader_id();
    else return std::vector<int>();
  }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  FaceCollection *coll;
  Fetcher<float> *fetch;
  Movement *move;
  Material *mat;
  MatrixArray *inst;
  MainLoopItem *mainloopitem;
  bool prepare_done = false;
};

// use ff_range and ff_key_range with this.
GameApi::ML GameApi::PolygonApi::mesh_anim_display_inst(GameApi::EveryApi &ev, P mesh_elem, FF val, MN mn, MT mat,MS inst)
{
  FaceCollection *coll = find_facecoll(e, mesh_elem);
  Fetcher<float> *fetcher = find_float_fetcher(e, val);
  Movement *move = find_move(e, mn);
  Material *material = find_material(e, mat);
  MatrixArray *instantiate = find_matrix_array(e, inst);
  //Fetcher<int> *states2 = find_int_fetcher(e,states);
  return add_main_loop(e, new MeshAnimDisplayInst(e,ev, coll, fetcher, move, material, instantiate));
}
GameApi::ML GameApi::PolygonApi::mesh_anim_display_inst2(GameApi::EveryApi &ev, P mesh_elem, FF val, MN mn, MT mat, PTS inst)
{
  MS ms = ev.matrices_api.from_points(inst);
  return mesh_anim_display_inst(ev,mesh_elem,val,mn,mat,ms);
}


#if 0
class SplitQuads : public FaceCollection
{
public:
  SplitQuads(FaceCollection *coll, float normal_multiply) : coll(coll), normal_multiply(normal_multiply) { }
  virtual void Prepare() { coll->Prepare(); }
  virtual int NumFaces() const
  {
    return 4*coll->NumFaces();
  }
  virtual int NumPoints(int face) const=0;
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual float Attrib(int face, int point, int id) const=0;
  virtual int AttribI(int face, int point, int id) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual float TexCoord3(int face, int point) const { return 0.0; }


  int face(int f) const
  {
    return f/4;
  }
  int part(int f) const
  {
    return f%4;
  }
  int get_point(int part, int point) const
  {
    switch(part) {
    case 0:
      switch(point) {
      case 0: return 0;
      case 1: return 1;
      case 2: return 4;
      case 3: return 3;
      };
    case 1:
      switch(point) {
      case 0: return 1;
      case 1: return 2;
      case 2: return 5;
      case 3: return 4;
      };
    case 2:
      switch(point) {
      case 0: return 3;
      case 1: return 4;
      case 2: return 7;
      case 3: return 6;
      };
    case 3:
      switch(point) {
      case 0: return 4;
      case 1: return 5;
      case 2: return 8;
      case 3: return 7;
      };
    };
    return -1;
  }
  Point get_point(int face, int i) const
  {
    
    switch(i) {
    case 0:
      {
      Point p1 = coll->FacePoint(face,0);
      return p1;
      }
    case 1:
      {
      Point p1 = coll->FacePoint(face,0);
      Point p2 = coll->FacePoint(face,1);
      return Point::Interpolate(p1,p2,0.5);
      }
    case 2:
      {
      Point p2 = coll->FacePoint(face,1);
      return p2;
      }
    case 3: 
      {
      Point p1 = coll->FacePoint(face,0);
      Point p3 = coll->FacePoint(face,2);
      return Point::Interpolate(p1,p3,0.5);
      }
    case 4:
      {
      Point p1 = coll->FacePoint(face,0);
      Point p2 = coll->FacePoint(face,1);
      Point p3 = coll->FacePoint(face,2);
      Point p4 = coll->FacePoint(face,3);
      return Point::Interpolate(Point::Interpolate(p1,p2,0.5),Point::Interpolate(p4,p3,0.5),0.5);
      }
    case 5:
      {
	Point p2 = coll->FacePoint(face,1);
	Point p3 = coll->FacePoint(face,2);
	return Point::Interpolate(p2,p3,0.5);
      }
    case 6: 
      {
      Point p4 = coll->FacePoint(face,3);
      return p4;
      }
    case 7:
      {
      Point p3 = coll->FacePoint(face,2);
      Point p4 = coll->FacePoint(face,3);
      return Point::Interpolate(p3,p4,0.5);
      }
    case 8:
      {
	Point p3 = coll->FacePoint(face,2);
	return p3;
      }
    
    };
    return Point(0.0,0.0,0.0);
  }
  
private:
  FaceCollection *coll;
  float normal_multiply;
};
#endif


class Block : public FaceCollection
{
public:
  Block(FaceCollection *coll, float pos_x, float pos_z, int x, int z, float delta_x, float delta_z) : coll(coll), pos_x(pos_x), pos_z(pos_z), x(x),z(z), delta_x(delta_x),delta_z(delta_z) { }
  virtual std::string name() const { return "Block"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (face_index.size()==0) {
      int s = coll->NumFaces();
      float start_x = pos_x + x*delta_x;
      float start_z = pos_z + z*delta_z;
      float end_x = pos_x + (x+1)*delta_x;
      float end_z = pos_z + (z+1)*delta_z;
      for(int i=0;i<s;i++) {
	Point p = pos(i);
	if (p.x>=start_x && p.x<end_x)
	  if (p.z>=start_z && p.z<end_z)
	    face_index.push_back(i);
      }
    }
  }
  virtual void Prepare()
  {
    coll->Prepare();
    if (face_index.size()==0) {
      int s = coll->NumFaces();
      float start_x = pos_x + x*delta_x;
      float start_z = pos_z + z*delta_z;
      float end_x = pos_x + (x+1)*delta_x;
      float end_z = pos_z + (z+1)*delta_z;
      for(int i=0;i<s;i++) {
	Point p = pos(i);
	if (p.x>=start_x && p.x<end_x)
	  if (p.z>=start_z && p.z<end_z)
	    face_index.push_back(i);
      }
    }
  }
  Point pos(int i) const
  {
    Point p1 = coll->FacePoint(i,0);
    Point p2 = coll->FacePoint(i,1);
    Point p3 = coll->FacePoint(i,2);
    Point p4 = coll->FacePoint(i,3%coll->NumPoints(i));
    float xx = (p1.x+p2.x+p3.x+p4.x)/4.0;
    float yy = (p1.y+p2.y+p3.y+p4.y)/4.0;
    float zz = (p1.z+p2.z+p3.z+p4.z)/4.0;
    return Point(xx,yy,zz);
  }


  
  virtual int NumFaces() const { return face_index.size(); }
  virtual int NumPoints(int face) const {
    int s = face_index.size();
    if (face>=0 && face<s)
      return coll->NumPoints(face_index[face]);
    else return 1;
  }
  virtual Point FacePoint(int face, int point) const
  {
    int s = face_index.size();
    if (face>=0 && face<s)
      return coll->FacePoint(face_index[face],point);
    else return Point(0.0,0.0,0.0);
  }
   
  bool has_normal() const { return coll->has_normal(); }
  virtual Vector PointNormal(int face, int point) const
  {
    int s = face_index.size();
    if (face>=0 && face<s)
      return coll->PointNormal(face_index[face],point);
    else return Vector(0.0,0.0,1.0);
  }
  bool has_attrib() const { return coll->has_attrib(); }
  virtual float Attrib(int face, int point, int id) const
  {
    int s = face_index.size();
    if (face>=0 && face<s)
      return coll->Attrib(face_index[face],point,id);
    else return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  { 
    int s = face_index.size();
    if (face>=0 && face<s)
      return coll->AttribI(face_index[face],point,id);
    else return 0;
  }
  bool has_color() const { return coll->has_color(); }
  virtual unsigned int Color(int face, int point) const
  {
    int s = face_index.size();
    if (face>=0 && face<s)
    return coll->Color(face_index[face],point);
    else return 0x00;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    int s = face_index.size();
    if (face>=0 && face<s)
    return coll->TexCoord(face_index[face],point);
    else { Point2d p; p.x =0.0; p.y =0.0; return p; }
  }
  virtual float TexCoord3(int face, int point) const {
    int s = face_index.size();

    if (face>=0 && face<s)
      return coll->TexCoord3(face_index[face],point);
    else return 0.0;
  }
private:
  FaceCollection *coll;
  float pos_x, pos_z;
  int x,z;
  float delta_x, delta_z;
  std::vector<int> face_index;
};

GameApi::ARR GameApi::PolygonApi::block_divide(P p, float pos_x, float pos_z, int sx, int sz, float delta_x, float delta_z)
{
  FaceCollection *coll = find_facecoll(e,p);
  ArrayType *array = new ArrayType;
  array->type = 1;
  
  for(int j=0;j<sz;j++) {
    for(int i=0;i<sx;i++) {
      
      GameApi::P elem = add_polygon2(e,new Block(coll, pos_x,pos_z, i,j, delta_x, delta_z),1);
      array->vec.push_back(elem.id);
    }
  }
  return add_array(e,array);
}

GameApi::ARR GameApi::PolygonApi::block_render(GameApi::EveryApi &ev, std::vector<P> vec, MT mat)
{
  ArrayType *array = new ArrayType;
  array->type = 1;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    GameApi::P p = vec[i];
    GameApi::ML ml = ev.materials_api.bind(p,mat);
    array->vec.push_back(ml.id);
  }
  return add_array(e,array);
}

GameApi::ARR GameApi::PolygonApi::block_render2(GameApi::EveryApi &ev, std::vector<P> vec, std::vector<MT> mat)
{
  ArrayType *array = new ArrayType;
  array->type = 1;
  int s = std::min(vec.size(),mat.size());
  for(int i=0;i<s;i++) {
    GameApi::P p = vec[i];
    GameApi::ML ml = ev.materials_api.bind(p,mat[i]);
    array->vec.push_back(ml.id);
  }
  return add_array(e,array);

}


extern float quake_pos_x, quake_pos_y;
extern float quake_rot_y;

PointsApiPoints *g_pts;
MatrixArray *g_pts_matrix;
float g_pos1;
float g_pos2;
Matrix g_compare_in_MV;


bool ComparePTSObj2(int a, int b)
{
  float val1,val2;
  int num = g_pts->NumPoints();
  if (a==num-2) val1=g_pos1; 
  else if (a==num-1) val1 = g_pos2;
  else val1=g_pts->Pos(a).x;

  if (b==num-2) val2=g_pos1;
  if (b==num-1) val2=g_pos2;
  else val2=g_pts->Pos(b).x;
  if (std::fabs(val1-val2) < 1e-6f) return a<b;
  
  return val1<val2;
}
bool ComparePTSObj(int a, int b)
{
  float val1,val2;
  val1=g_pts->Pos(a).x;
  val2=g_pts->Pos(b).x;
  if (std::fabs(val1-val2) < 1e-6f) return a<b;
  return val1<val2;
}
bool ComparePTSObj_y(int a, int b)
{
  float val1,val2;
  
  val1=g_pts->Pos(a).z;

  val2=g_pts->Pos(b).z;
  if (std::fabs(val1-val2) < 1e-6f) return a<b;
  return val1<val2;
}
bool ComparePTSObj_y_matrix(int a, int b)
{
  float val1,val2;
  val1=g_pts_matrix->Index(a).matrix[4*2+3];
  val2=g_pts_matrix->Index(b).matrix[4*2+3];
  if (std::fabs(val1-val2) < 1e-6f) return a<b;
  return val1<val2;
}
class BlockPTS2;
class BlockPTS2_matrix;
BlockPTS2 *g_blockpts2;
BlockPTS2_matrix *g_blockpts2_matrix;

bool CompareWithCalc(int a, int b);
bool CompareWithCalc_matrix(int a, int b);


class BlockPTS : public PointsApiPoints
{
public:
  BlockPTS(PointsApiPoints *points, float d, int max_points) : points(points),d(d), max_points(max_points) { }

  void Collect(CollectVisitor &vis)
  {
    points->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int s = points->NumPoints();
    allpoints.clear();
    for(int i=0;i<s;i++)
      {
	allpoints.push_back(i);
      }
    g_pts = points;
    std::sort(allpoints.begin(),allpoints.end(),ComparePTSObj);
  }
  
  virtual void Prepare() { points->Prepare(); HeavyPrepare(); }
  virtual void HandleEvent(MainLoopEvent &event) { points->HandleEvent(event); }
  virtual bool Update(MainLoopEnv &e) {
    bool b = points->Update(e);
    //pos2=pos;
    pos.clear();
    int s = points->NumPoints();
    if (s<1) return true;
    if (allpoints.size()<1) return true;
    float start_x = -quake_pos_x-d;
    float end_x = -quake_pos_x+d;
    //allpoints.push_back(allpoints.size());
    //allpoints.push_back(allpoints.size());

    g_pos1 = start_x;
    g_pos2 = end_x;
    g_pts = points;
    
    //std::cout << "START_X:" << start_x << " END_X:" << end_x << std::endl;

    int result=-1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    
    while(left <= right)
      {
	int mid = (left+right)/2;
	if (points->Pos(allpoints[mid]).x < start_x && points->Pos(allpoints[mid+1]).x > start_x) { result=mid; break; }

	if (points->Pos(allpoints[mid]).x < start_x)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) result=(left+right)/2;
    }
    int start = result;

    result = -1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    while(left <= right)
      {
        int mid = (left+right)/2;
	if (points->Pos(allpoints[mid]).x < end_x && points->Pos(allpoints[mid+1]).x > end_x) { result=mid; break; }

	if (points->Pos(allpoints[mid]).x < end_x)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) result=(left+right)/2;
    }
    int end = result;
    
    
    //int start = ii-allpoints.begin();
    //int end = ii2-allpoints.begin();

    //std::cout << start << " " << end << std::endl;
    //std::cout << "POINT1:" << points->Pos(allpoints[start]).x << " POINT2:" << points->Pos(allpoints[end]).x << std::endl;

    
    //allpoints.erase(allpoints.begin()+allpoints.size()-1);
    //allpoints.erase(allpoints.begin()+allpoints.size()-1);
    


    /*
    int s2 = allpoints.size();
    for(int i=0;i<s2;i++)
      {
	std::cout << "Point#" << i << "=" << allpoints[i] << "::" << points->Pos(allpoints[i]) << std::endl;
      }
    */
    
    for(int i=start;i<end;i++)
      {
	if (enabled(allpoints[i]))
	  {
	    pos.push_back(allpoints[i]);
	  }
      }
    /*
    int s2 = std::min(pos2.size(),pos.size());
    bool changed = false;
    for(int i=0;i<s2;i++)
      {
	if (pos[i]!=pos2[i])
	  {
	    changed=true;
	  }
      }
    */
    //std::cout << "Update:" << prev << " " << pos.size() << std::endl;
    //if (prev==pos.size()&&!changed) return false;
    prev=pos.size();
    return true; }
  virtual int NumPoints() const { return max_points; }
  virtual Point Pos(int i) const { if (i>=pos.size()) return Point(-9999.0,-9999.0,-9999.0); return points->Pos(pos[i]); }
  virtual unsigned int Color(int i) const { if (i>=pos.size()) return 0xffffffff; return points->Color(pos[i]); }
  virtual Vector Normal(int i) const {if (i>=pos.size()) return Vector(1.0,0.0,0.0); return points->Normal(pos[i]); }

  bool enabled(int i) const
  {
    Point p = points->Pos(i);
    float cursor_pos_x = quake_pos_x;
    float cursor_pos_y = -quake_pos_y;

    float delta_x = p.x+cursor_pos_x;
    float delta_y = p.z-cursor_pos_y;
    float dist = delta_x*delta_x + delta_y*delta_y;
    //std::cout << "A:" << p.x << " " << p.z << std::endl;
    //std::cout << "B:" << cursor_pos_x << " " << cursor_pos_y << std::endl;
    if (dist<d*d)
      {
	return true;
      }
    return false;
  }
private:
  PointsApiPoints *points;
  float d;
  float sign;
  std::vector<int> allpoints;
  std::vector<int> pos;
  int prev=0;
  int max_points;
};


GameApi::PTS GameApi::PointsApi::block_pts(GameApi::PTS pts, float d, int max_points)
{
  PointsApiPoints *points = find_pointsapi_points(e,pts);
  return add_points_api_points(e,new BlockPTS(points,d,max_points));
}
extern Matrix g_quakeml2_matrix;
extern bool g_is_quakeml2;

class BlockPTS2 : public PointsApiPoints
{
public:
  BlockPTS2(GameApi::Env &env, PointsApiPoints *points, float start_x, float end_x, float start_y, float end_y, float ncd_z_start, float ncd_z_end,int max_points) : env(env), points(points),start_x2(start_x), end_x2(end_x), start_y2(start_y),end_y2(end_y), max_points(max_points), ncd_z_start2(ncd_z_start), ncd_z_end2(ncd_z_end) {
    
    if (start_x2>end_x2) std::swap(start_x2,end_x2);
    if (start_y2>end_y2) std::swap(start_y2,end_y2);
    if (ncd_z_start2>ncd_z_end2) std::swap(ncd_z_start2,ncd_z_end2);
    firsttime = true;
  }

  void Collect(CollectVisitor &vis)
  {
    points->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (firsttime) {
    int s = points->NumPoints();
    allpoints.clear();
    for(int i=0;i<s;i++)
      {
	allpoints.push_back(i);
      }
    g_pts = points;
    std::sort(allpoints.begin(),allpoints.end(),ComparePTSObj_y);
    firsttime = false;
    }
  }

  float calc_pos(int p) const
  {
#if 0
    int idx = allpoints[p];
    Point pp = calc_pos3(idx);
    return pp.z;
#endif
#if 1
    int idx = allpoints[p];
    Matrix p0 = Matrix::Identity(); 
    Point pt = points->Pos(idx); 
    Point world = pt * p0;
    world.x -=quake_pos_x;
    world.z -=quake_pos_y;
    Point view = world * in_MV;
    std::cout << "calc_pos:" << p << "::"<< view.z << std::endl;
    return view.z;
#endif
    
#if 0
    int pos = allpoints[p];
    Point pt = points->Pos(pos);
    pt.x-=quake_pos_x;
    pt.y=-60.0;
    pt.z-=quake_pos_y;
    Point p2 = pt*in_MV;
    return p2.z;
#endif
  }
  Point2d calc_pos2(int pos) const
  {
#if 1
    Matrix p0 = Matrix::Identity(); //points->Index(pos);
    Point local = points->Pos(pos); //(0.0f,0.0f,0.0f);
    Point world = local * p0;
    world.x -= quake_pos_x;
    world.z -= quake_pos_y;
    Point view = world * in_MV;
    Point2d res;
    res.x = view.x;
    res.y = view.z;
    return res;
#endif
#if 0
    Point p = calc_pos3(pos);
    Point2d res;
    res.x = p.x;
    res.y = p.z;
    return res;
#endif
    
#if 0
    Point p = points->Pos(pos);
    p.x-=quake_pos_x;
    p.y=-60.0;
    p.z-=quake_pos_y;
    Point p2 = p*in_MV;
    
    Point2d res;
    res.x = p2.x;
    res.y = p2.z;
    return res;
#endif
  }

  Point calc_pos3(int pos) const
  {
    Matrix p0 = Matrix::Identity(); //points->Index(pos);
    Point local = points->Pos(pos); //(0.0f,0.0f,0.0f);
    Point world = local * p0;
    world.x -= quake_pos_x;
    world.z -= quake_pos_y;
    Point ncd = world * in_MV * in_Proj;
    ncd.z-=1.0;
    ncd.z*=2.0;
    ncd.z-=1.0;
    return ncd;
  }

  
  virtual void Prepare() { points->Prepare(); HeavyPrepare(); }
  virtual void HandleEvent(MainLoopEvent &event) {
    points->HandleEvent(event);
    /*
    if (event.ch!=-1)
      {
	g_blockpts2 = this;
	std::sort(allpoints.begin(),allpoints.end(),CompareWithCalc);
      }
    */
  }
  virtual bool Update(MainLoopEnv &e) {
    in_MV = e.in_MV;
    in_Proj = e.in_P;
    g_compare_in_MV = in_MV;

    std::sort(allpoints.begin(), allpoints.end(),
	      [&](int a, int b){
		Point2d aa = calc_pos2(a);
		Point2d bb = calc_pos2(b);
		if (std::fabs(aa.y-bb.y) < 1e-6f) return a<b;
		return aa.y < bb.y;
	      });

    
    bool b = points->Update(e);
    pos.clear();
    int s = points->NumPoints();
    if (s<1) return true;
    if (allpoints.size()<1) return true;
    float start_y = start_y2;
    float end_y = end_y2;

#if 0
    
    std::cout << "start_y:" << start_y << std::endl;

    
    if (start_y>end_y) std::swap(start_y,end_y);
    int sy=allpoints.size();

    int result=-1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    while(left <= right)
      {
	int mid = (left+right)/2;
	if (calc_pos(mid) < start_y &&  calc_pos(mid+1) > start_y) { result=mid; break; }

    if (calc_pos(mid) < start_y)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) { std::cout << "search fail!" << std::endl; result=0; }
    }
    int start = result;
    if (start<0) start=0;
    if (start>allpoints.size()-1) start=allpoints.size()-1;
    
    result = -1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    while(left <= right)
      {
        int mid = (left+right)/2;
	if (calc_pos(mid) < end_y && calc_pos(mid+1) > end_y) { result=mid; break; }

	if (calc_pos(mid) < end_y)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) result=allpoints.size()-1; //(left+right)/2;
    }
    int end = result;
    if (end<0) end=0;
    if (end>allpoints.size()-1) end=allpoints.size()-1;

    
  if (start>end) std::swap(start,end);

  minimum=4000000.0;
  maximum=-4000000.0;

  //std::cout << "startend:" << start << " " << end << " " << calc_pos(start) << " " << calc_pos(end) << std::endl;

#endif
  
  int start = 0;
  int end = allpoints.size();

  
  for(int i=start;i<=end;i++)
      {
	if (enabled(allpoints[i]))
	  {
	    pos.push_back(allpoints[i]);
	  }
      }
  //std::cout << "Range:" << minimum << " " << maximum << std::endl;
    prev=pos.size();
    return true; }
  virtual int NumPoints() const { return max_points; }
  virtual Point Pos(int i) const { if (i>=pos.size()) return Point(-9999.0,-9999.0,-9999.0);
    Point p = points->Pos(pos[i]);
    return Point(p.x,p.y,-p.z); }
  virtual unsigned int Color(int i) const { if (i>=pos.size()) return 0xffffffff; return points->Color(pos[i]); }
  virtual Vector Normal(int i) const {if (i>=pos.size()) return Vector(1.0,0.0,0.0); return points->Normal(pos[i]); }

  bool enabled(int i) const
  {
#if 0
    Point pos = calc_pos3(i);
    if (/*pos.x>=-1.0 && pos.x<=1.0 &&*/
	/*pos.y>=-3.0 && pos.y<=3.0 &&*/
	pos.z>=ncd_z_start2 && pos.z<=ncd_z_end2) {
      return true;
    }
#endif
#if 1
    Point2d pos_y = calc_pos2(i);
    
    if (pos_y.y>=start_y2 && pos_y.y<=end_y2
	&&
	  pos_y.x>=start_x2 && pos_y.x<=end_x2)
      return true;
#endif
    return false;
  }
private:
  GameApi::Env &env;
  PointsApiPoints *points;
  float start_x2,end_x2;
  float start_y2,end_y2;
  float sign;
  std::vector<int> allpoints;
  std::vector<int> pos;
  int prev=0;
  int max_points;
  float ncd_z_start2,ncd_z_end2;
  Matrix in_MV;
  Matrix in_Proj;
  bool firsttime;
  mutable float minimum=4000000.0;
  mutable float maximum=-4000000.0;
};

bool CompareWithCalc(int a, int b)
{
  Point2d val1= g_blockpts2->calc_pos2(a);
  Point2d val2= g_blockpts2->calc_pos2(b);
  //if (std::fabs(val1.y-val2.y) < 1e-6f) return a<b;
  return val1.y<val2.y;  
}


class BlockPTS2_matrix : public MatrixArray
{
public:
  BlockPTS2_matrix(GameApi::Env &env, MatrixArray *points, float start_x, float end_x, float start_y, float end_y, int max_points, float ncd_z_start, float ncd_z_end) : env(env), points(points),start_x2(start_x), end_x2(end_x), start_y2(start_y),end_y2(end_y), max_points(max_points),ncd_z_start2(ncd_z_start), ncd_z_end2(ncd_z_end) {

    if (start_x2>end_x2) std::swap(start_x2,end_x2);
    if (start_y2>end_y2) std::swap(start_y2,end_y2);
    if (ncd_z_start2>ncd_z_end2) std::swap(ncd_z_start2,ncd_z_end2);
    firsttime = true;
  }

  void Collect(CollectVisitor &vis)
  {
    points->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (firsttime) {
    int s = points->Size();
    allpoints.clear();
    for(int i=0;i<s;i++)
      {
	allpoints.push_back(i);
      }
    g_pts_matrix = points;
    std::sort(allpoints.begin(),allpoints.end(),ComparePTSObj_y_matrix);
    firsttime = false;
    }
  }
  
  virtual void Prepare() { points->Prepare(); HeavyPrepare(); }
  virtual void HandleEvent(MainLoopEvent &event) {
    points->HandleEvent(event);
    /*    if (event.ch!=-1)
      {
	g_blockpts2_matrix = this;
	std::sort(allpoints.begin(),allpoints.end(),CompareWithCalc_matrix);
      }
    */
  }
  virtual bool Update(MainLoopEnv &e) {

    
    in_MV = e.in_MV;
    in_Proj = e.in_P;
    bool b = points->Update(e);
    std::sort(allpoints.begin(), allpoints.end(),
	      [&](int a, int b){
		Point2d aa = calc_pos2(a);
		Point2d bb = calc_pos2(b);
		if (std::fabs(aa.y-bb.y) < 1e-6f) return a<b;
		return aa.y < bb.y;
	      });


    //pos2=pos;
    pos.clear();
    int s = points->Size();
    if (s<1) return true;
    if (allpoints.size()<1) return true;
    float start_y = start_y2;
    float end_y = end_y2; 
    int start,end;
    
    if (start_y>end_y) std::swap(start_y,end_y);

    int result=-1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    
    while(left <= right)
      {
	int mid = (left+right)/2;
	if (calc_pos(mid) < start_y && calc_pos(mid+1) > start_y) { result=mid; break; }

	if (calc_pos(mid) < start_y)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) { std::cout << "search fail!" << std::endl; result=(left+right)/2; }
    }
    start = result;
    if (start<0) start=0;
    if (start>allpoints.size()-1) start=allpoints.size()-1;

    
    result = -1;
    {
    int left = 0;
    int right = allpoints.size()-1;

    while(left <= right)
      {
        int mid = (left+right)/2;
	if (calc_pos(mid) < end_y && calc_pos(mid+1) > end_y) { result=mid; break; }

	if (calc_pos(mid) < end_y)
	  {
	    left = mid + 1;
	  }
	else
	  {
	    right = mid - 1;
	  }
      }
    if (result==-1) result=(left+right)/2;
    }
    end = result;
    if (end<0) end=0;
    if (end>allpoints.size()-1) end=allpoints.size()-1;
    

    if (start>end) std::swap(start,end);


    
    for(int i=start;i<=end;i++)
      {
	if (enabled(allpoints[i]))
	  {
	    pos.push_back(allpoints[i]);
	  }
      }
    //std::cout << "Range:" << minimum << " " << maximum << std::endl;
    prev=pos.size();
    return true; }
  virtual int Size() const { return max_points; }
  virtual Matrix Index(int i) const {
    if (i>=pos.size()) { Matrix m=Matrix::Translate(-666666.0,-666666.0,-666666.0); return m; }
    Matrix m = points->Index(pos[i]);
    m.matrix[4*2+3]=-m.matrix[4*2+3];
    return m;
  }
  virtual unsigned int Color(int i) const { if (i>=pos.size()) return 0xffffffff; return points->Color(pos[i]); }
  virtual Vector Normal(int i) const {if (i>=pos.size()) return Vector(1.0,0.0,0.0); return points->Normal(pos[i]); }


  float calc_pos(int p) const
  {
#if 0
    int pos = allpoints[p];
    Matrix p0 = points->Index(pos);
    Point pt = Point(p0.get_translate());
    pt.x-=quake_pos_x;
    pt.y=-60.0;
    pt.z-=quake_pos_y;
    Point p2 = pt*in_MV; 
    return p2.z;
#endif
#if 1
    int idx = allpoints[p];
    Matrix p0 = points->Index(idx);
    Point pt = Point(0.0f,0.0f,0.0f);
    Point world = pt * p0;
    world.x -=quake_pos_x;
    world.z -=quake_pos_y;
    Point view = world * in_MV;
    std::cout <<"calc_pos:"<< view << std::endl;
    return view.z;
#endif
    /*    
    int idx = allpoints[p];
    Point pp = calc_pos3(idx);
    return pp.z;
    */
  }
  Point2d calc_pos2(int pos) const
  {
#if 0
    Matrix p0 = points->Index(pos);
    Point p = Point(p0.get_translate());
    p.x-=quake_pos_x;
    p.y=-60.0;
    p.z-=quake_pos_y;
    Point p2 = p*in_MV; 
    Point2d res;
    res.x = p2.x;
    res.y = p2.z;
    return res;
#endif
    Matrix p0 = points->Index(pos);
    Point local(0.0f,0.0f,0.0f);
    Point world = local * p0;
    world.x -= quake_pos_x;
    world.z -= quake_pos_y;
    Point view = world * in_MV;
    Point2d res;
    res.x = view.x;
    res.y = view.z;
    return res;
  }
  Point calc_pos3(int pos) const
  {
    Matrix p0 = points->Index(pos);
    Point local(0.0f,0.0f,0.0f);
    Point world = local * p0;
    world.x -= quake_pos_x;
    world.z -= quake_pos_y;
    Point ncd = world * in_MV * in_Proj;
    ncd.z-=1.0;
    ncd.z*=2.0;
    ncd.z-=1.0;
    return ncd;
  }
  
  
  bool enabled(int i) const
  {
#if 0
    Point pos = calc_pos3(i);
    if (pos.x>=-1.0 && pos.x<=1.0 &&
	/*pos.y>=-1.0 && pos.y<=1.0 &&*/
	pos.z>=ncd_z_start2 && pos.z<=ncd_z_end2)
      return true;
#endif
#if 1
    Point2d pos_y = calc_pos2(i);
    if (pos_y.y>=start_y2 && pos_y.y<=end_y2
	&&
	  pos_y.x>=start_x2 && pos_y.x<=end_x2)
      return true;
#endif
    return false;
  }
private:
  GameApi::Env &env;
  MatrixArray *points;
  float start_x2,end_x2;
  float start_y2,end_y2;
  float sign;
  std::vector<int> allpoints;
  std::vector<int> pos;
  int prev=0;
  int max_points;
  float ncd_z_start2,ncd_z_end2;
  Matrix in_MV;
  Matrix in_Proj;
  bool firsttime;
  mutable float minimum=4000000.0;
  mutable float maximum=-4000000.0;
};

bool CompareWithCalc_matrix(int a, int b)
{
  Point2d val1= g_blockpts2_matrix->calc_pos2(a);
  Point2d val2= g_blockpts2_matrix->calc_pos2(b);
  if (std::fabs(val1.y-val2.y) < 1e-6f) return a<b;
  return val1.y<val2.y;  
}


GameApi::PTS GameApi::PointsApi::block_pts_lod(GameApi::PTS pts, float start_x, float end_x, float start_y, float end_y, int max_points, float ncd_z_s, float ncd_z_e)
{
  PointsApiPoints *points = find_pointsapi_points(e,pts);
  return add_points_api_points(e,new BlockPTS2(e,points,start_x,end_x,start_y,end_y,max_points,ncd_z_s,ncd_z_e));
}

GameApi::MS GameApi::PointsApi::block_ms_lod(GameApi::MS ms, float start_x, float end_x, float start_y, float end_y, int max_points, float ncd_z_s, float ncd_z_e)
{
  MatrixArray *mat = find_matrix_array(e,ms);
  return add_matrix_array(e, new BlockPTS2_matrix(e,mat,start_x,end_x,start_y,end_y, max_points,ncd_z_s,ncd_z_e));
}


extern float quake_pos_x;
extern float quake_pos_y;
class BlockDraw : public MainLoopItem
{
public:
  BlockDraw(std::vector<MainLoopItem*> items, float pos_x, float pos_z, int sx, int sz, float delta_x, float delta_z, int view) : items(items), pos_x(pos_x), pos_z(pos_z), sx(sx),sz(sz),delta_x(delta_x), delta_z(delta_z), view(view) { }
  void Collect(CollectVisitor &vis)
  {
    int s = items.size();
    for(int i=0;i<s;i++) items[i]->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare()
  {
    int s = items.size();
    for(int i=0;i<s;i++) items[i]->Prepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    float cursor_pos_x = quake_pos_x;
    float cursor_pos_y = -quake_pos_y+400.0;

    cursor_pos_x = -cursor_pos_x;
    
    cursor_pos_x -= pos_x;
    cursor_pos_y -= pos_z;
    
    cursor_pos_x /= delta_x;
    cursor_pos_y /= delta_z;

    int p_x = int(cursor_pos_x);
    int p_z = int(cursor_pos_y);
    //std::cout << "P:" << p_x << " " <<p_z << std::endl;
    
    for(int j=p_z-view;j<p_z+view;j++)
      for(int i=p_x-view;i<p_x+view;i++)
	{
	  int ii = i+sx*j;
	  if (i>=0 && i<sx) {
	    if (j>=0 && j<sz) {
	      items[ii]->execute(e);
	    }
	  }
	}

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = items.size();
    for(int i=0;i<s;i++) items[i]->handle_event(e);
  }
  virtual std::vector<int> shader_id() {

    std::vector<int> vec;
    float cursor_pos_x = quake_pos_x;
    float cursor_pos_y = -quake_pos_y+400.0;

    cursor_pos_x = -cursor_pos_x;
    
    cursor_pos_x -= pos_x;
    cursor_pos_y -= pos_z;


    cursor_pos_x /= delta_x;
    cursor_pos_y /= delta_z;

    int p_x = int(cursor_pos_x);
    int p_z = int(cursor_pos_y);
    
    for(int j=p_z-view;j<p_z+view;j++)
      for(int i=p_x-view;i<p_x+view;i++)
	{
	  int ii = i+sx*j;
	  if (i>=0 && i<sx)
	    if (j>=0 && j<sz) {
	      std::vector<int> vec2 = items[ii]->shader_id();
	      int s = vec2.size();
	      for(int g=0;g<s;g++) vec.push_back(vec2[g]);
	    }
	}
    return vec;
   }
private:
  std::vector<MainLoopItem*> items;
  float pos_x, pos_z;
  int sx,sz;
  float delta_x,delta_z;
  int view;
};

GameApi::ML GameApi::PolygonApi::block_draw(std::vector<ML> vec, float pos_x, float pos_z, int sx, int sz, float delta_x, float delta_z, int view)
{
  std::vector<MainLoopItem*> items;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    items.push_back(find_main_loop(e,vec[i]));
  }
  return add_main_loop(e, new BlockDraw(items, pos_x, pos_z, sx,sz,delta_x,delta_z, view));
}



class SubstituteQuadWithMesh : public FaceCollection
{
public:
  SubstituteQuadWithMesh(FaceCollection *coll1, FaceCollection *coll2, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float normal) : coll1(coll1), coll2(coll2), start_x(start_x), end_x(end_x), start_y(start_y),end_y(end_y),start_z(start_z),end_z(end_z),normal(normal) { }
  virtual std::string name() const { return "SubstituteQuadWithMesh"; }

  void Collect(CollectVisitor &vis)
  {
    coll1->Collect(vis);
    coll2->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { coll1->Prepare(); coll2->Prepare(); }
  virtual int NumFaces() const
  {
    return coll1->NumFaces() * coll2->NumFaces();
  }

  
  virtual int NumPoints(int face) const
  {
    int f = face % coll2->NumFaces();
    return coll2->NumPoints(f);
  }
  virtual Point FacePoint(int face, int point) const
  {
    int f2 = face / coll2->NumFaces();
    int f = face % coll2->NumFaces();
    Point p = coll2->FacePoint(f,point);

    // start from cube [start_x..end_x,start_y..end_y, start_z..end_z]
    p.x-=start_x;
    p.x/=(end_x-start_x);
    p.y-=start_y;
    p.y/=(end_y-start_y);
    p.z-=start_z;
    p.z/=(end_z-start_z);
    // now it's cube [0..1,0..1,0..1]

    Point pp0 = coll1->FacePoint(f2,0);
    Point pp1 = coll1->FacePoint(f2,1);
    Point pp2 = coll1->FacePoint(f2,2);
    Point pp3 = coll1->FacePoint(f2,3);

    Vector n0 = coll1->PointNormal(f2,0);
    Vector n1 = coll1->PointNormal(f2,1);
    Vector n2 = coll1->PointNormal(f2,2);
    Vector n3 = coll1->PointNormal(f2,3);

    Point pk0 = pp0 + n0*p.z*normal;
    Point pk1 = pp1 + n1*p.z*normal;
    Point pk2 = pp2 + n2*p.z*normal;
    Point pk3 = pp3 + n3*p.z*normal;

    Vector dx_01 = pk1-pk0;
    Vector dx_32 = pk2-pk3;
    //Vector dy_03 = pk3-pk0;
    //Vector dy_12 = pk3-pk1;

    Point px0 = pk0 + dx_01*p.x;
    Point px1 = pk3 + dx_32*p.x;
    Vector vx = px1-px0;
    Point res = px0 + vx*p.y;
    return res;
  }
  virtual Vector PointNormal(int face, int point) const { Vector v{0.0,0.0,0.0}; return v; }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  bool has_color() const { return coll2->has_color(); }
  virtual unsigned int Color(int face, int point) const
  {
    int f = face % coll2->NumFaces();
    unsigned int p = coll2->Color(f,point);
    return p;
  }
  bool has_texcoord() const { return coll2->has_texcoord(); }
  virtual Point2d TexCoord(int face, int point) const
  {
    int f = face % coll2->NumFaces();
    Point2d p = coll2->TexCoord(f,point);
    return p;
  }
  virtual float TexCoord3(int face, int point) const {
    int f = face % coll2->NumFaces();
    float p = coll2->TexCoord3(f,point);
    return p;
  }

private:
  FaceCollection *coll1, *coll2;
  float start_x, end_x;
  float start_y, end_y;
  float start_z, end_z;
  float normal;
};


GameApi::P GameApi::PolygonApi::substitute(P p1, P p2, float start_x, float end_x, float start_y, float end_y, float start_z, float end_z, float normal)
{
  FaceCollection *coll1 = find_facecoll(e,p1);
  FaceCollection *coll2 = find_facecoll(e,p2);
  return add_polygon2(e, new SubstituteQuadWithMesh(coll1,coll2,start_x,end_x,start_y,end_y,start_z,end_z,normal),1);
}


class OptimizeMesh : public FaceCollection
{
public:
  OptimizeMesh(FaceCollection *coll, float max) : coll(coll),max(max) {}
   virtual std::string name() const { return "OptimizeMesh"; }
 
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int s = coll->NumFaces();
    vec.reserve(s);
    for(int i=0;i<s;i++) {
      if (!skip(i)) vec.push_back(i);
    }
  }
  virtual void Prepare() { coll->Prepare();

    int s = coll->NumFaces();
    vec.reserve(s);
    for(int i=0;i<s;i++) {
      if (!skip(i)) vec.push_back(i);
    }
    
  }

  virtual int NumObjects() const {
    return coll->NumObjects();
    }
  virtual std::pair<int,int> GetObject(int o) const {
    std::pair<int,int> p = coll->GetObject(o);
    int s = vec.size();
    int count1 = -1;
    int count2 = -1;
    for(int i=0;i<s;i++) {
      if (count1==-1 && vec[i]>=p.first) count1 = i;
      if (count2==-1 && vec[i]>=p.second) count2 = i;
    }
    return std::make_pair(count1,count2);
  }

  virtual int NumFaces() const
  {
    return vec.size();
  }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(vec[face]);

  }
  virtual Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(vec[face],point);
  }
  bool has_normal() const { return coll->has_normal(); }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(vec[face],point);

  } 
  bool has_attrib() const { return coll->has_attrib(); }

  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(vec[face],point,id);

  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(vec[face],point,id);

  } 
  bool has_color() const { return coll->has_color(); }
 virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(vec[face],point);
  }
  bool has_texcoord() const { return coll->has_texcoord(); }

  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(vec[face],point);
  }
  virtual float TexCoord3(int face, int point) const {
    
    return coll->TexCoord3(vec[face],point);    
  }


  bool skip(int face) const
  {
    Point p1 = coll->FacePoint(face,0);
    Point p2 = coll->FacePoint(face,1);
    Point p3 = coll->FacePoint(face,2);
    int num = coll->NumPoints(face);
    Point p4;
    if (num!=0)
      p4 = coll->FacePoint(face,3%num);
    else p4 = p1;
    
    bool x=false,y=false,z=false;
    
    float min_x = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
    float max_x = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
    if (max_x-min_x<max) x=true;

    float min_y = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
    float max_y = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
    if (max_y-min_y<max) y=true;

    float min_z = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));
    float max_z = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));
    if (max_z-min_z<max) z=true;
    return x&&y&&z;
  }
  
private:
  FaceCollection *coll;
  float max;
  std::vector<int> vec;
};
GameApi::P GameApi::PolygonApi::optimize_mesh(P p, float max)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e,new OptimizeMesh(coll,max),1);
}

#if 0

class RectanglePoly : public FaceCollection
{
public:
  RectanglePoly(RectangleArray &arr) : arr(arr) {}
  virtual void Prepare()=0;
  virtual int NumFaces() const = 0;
  virtual int NumPoints(int face) const=0;
  virtual Point FacePoint(int face, int point) const=0;
  virtual Vector PointNormal(int face, int point) const=0;
  virtual float Attrib(int face, int point, int id) const=0;
  virtual int AttribI(int face, int point, int id) const=0;
  virtual unsigned int Color(int face, int point) const=0;
  virtual Point2d TexCoord(int face, int point) const=0;
  virtual float TexCoord3(int face, int point) const { return 0.0; }

private:
  RectangleArray &arr;
};

struct RectCorner
{
  Point p;
  bool left_or_right;
  bool top_or_bottom;
  bool invert = false;
};

bool Compare_x(const RectCorner &x1, const RectCorner &x2)
{
  return x1.p.x<x2.p.x;
}
bool Compare_y(const RectCorner &y1, const RectCorner &y2)
{
  return y1.p.y<y2.p.y;
}

class RectangleBooleanOps : public RectangleArray
{
public:
  RectangleBooleanOps(RectangleArray &arr) : arr(arr) { }
  void Collect(CollectVisitor &vis)
  {
    arr.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    if (x_corners.size()==0 && y_corners.size()==0) {

    int s = arr.NumRects();
    for(int i=0;i<s;i++) {
      Rect r = arr.GetRect(i);
      RectCorner x_c_1;
      x_c_1.p = Point(r.x,r.y); // tl
      x_c_1.left_or_right = false;
      x_c_1.top_or_bottom = false;
      RectCorner x_c_2;
      x_c_2.p = Point(r.x+r.sx,r.y); // tr
      x_c_2.left_or_right = true;
      x_c_2.top_or_bottom = false;
      RectCorner x_c_3;
      x_c_3.p = Point(r.x,r.y+r.sy); // bl
      x_c_3.left_or_right = false;
      x_c_3.top_or_bottom = true;
      RectCorner x_c_4;
      x_c_4.p = Point(r.x+r.sx,r.y+r.sy); // br
      x_c_4.left_or_right = true;
      x_c_4.top_or_bottom = true;

      bool invert = arr.Type(i);
      if (invert) {
	x_c_1.invert = true;
	x_c_2.invert = true;
	x_c_3.invert = true;
	x_c_4.invert = true;
      }
      
      x_corners.push_back(x_c_1);
      x_corners.push_back(x_c_2);
      x_corners.push_back(x_c_3);
      x_corners.push_back(x_c_4);

      y_corners.push_back(x_c_1);
      y_corners.push_back(x_c_2);
      y_corners.push_back(x_c_3);
      y_corners.push_back(x_c_4);
    }
    std::sort(x_corners.begin(),x_corners.end(),&Compare_x);
    std::sort(y_corners.begin(),y_corners.end(),&Compare_y);
    }

  }
  virtual void Prepare()
  {
    arr.Prepare();
    if (x_corners.size()==0 && y_corners.size()==0) {

    int s = arr.NumRects();
    for(int i=0;i<s;i++) {
      Rect r = arr.GetRect(i);
      RectCorner x_c_1;
      x_c_1.p = Point(r.x,r.y); // tl
      x_c_1.left_or_right = false;
      x_c_1.top_or_bottom = false;
      RectCorner x_c_2;
      x_c_2.p = Point(r.x+r.sx,r.y); // tr
      x_c_2.left_or_right = true;
      x_c_2.top_or_bottom = false;
      RectCorner x_c_3;
      x_c_3.p = Point(r.x,r.y+r.sy); // bl
      x_c_3.left_or_right = false;
      x_c_3.top_or_bottom = true;
      RectCorner x_c_4;
      x_c_4.p = Point(r.x+r.sx,r.y+r.sy); // br
      x_c_4.left_or_right = true;
      x_c_4.top_or_bottom = true;

      bool invert = arr.Type(i);
      if (invert) {
	x_c_1.invert = true;
	x_c_2.invert = true;
	x_c_3.invert = true;
	x_c_4.invert = true;
      }
      
      x_corners.push_back(x_c_1);
      x_corners.push_back(x_c_2);
      x_corners.push_back(x_c_3);
      x_corners.push_back(x_c_4);

      y_corners.push_back(x_c_1);
      y_corners.push_back(x_c_2);
      y_corners.push_back(x_c_3);
      y_corners.push_back(x_c_4);
    }
    std::sort(x_corners.begin(),x_corners.end(),&Compare_x);
    std::sort(y_corners.begin(),y_corners.end(),&Compare_y);
    }
  }
  virtual int NumRects() const { return (x_corners.size()-1)*(y_corners.size()-1); }
  virtual Rect GetRect(int i) const
  {
    int sx = x_corners.size();
    
    int xx = i/sx;
    int yy = i-xx*sx;

    float start_x = x_corners[xx].p.x;
    float start_y = y_corners[yy].p.y;
    float end_x = x_corners[xx+1].p.x;
    float end_y = y_corners[yy+1].p.y;
    Rect r;
    r.x = start_x;
    r.y = start_y;
    r.sx = (end_x-start_x);
    r.sy = (end_y-start_y);

    
    
    return r;
  }
  virtual int Type(int i) const
  {
    
  }

private:
  RectangleArray &arr;
  std::vector<RectCorner> x_corners;
  std::vector<RectCorner> y_corners;

};
#endif

class ConvexHull : public FaceCollection
{
public:
  ConvexHull(PointsApiPoints *points) : points(points) { }
  virtual std::string name() const { return "ConvexHull"; }

  float signedVolume(Point a, Point b, Point c, Point d) const
  {
    Matrix m = { { a.x,a.y,a.z, 1.0,
		 b.x,b.y,b.z, 1.0,
		 c.x,c.y,c.z, 1.0,
		   d.x,d.y,d.z, 1.0}  };
    return Matrix::Determinant(m);
  }
  bool is_visible(Point a, Point b, Point c, Point p)  const
  {
    return signedVolume(a,b,c,p) > 0;
  }
  void calc_convex_hull()
  {
    H = std::vector<int>();
    faces = std::vector<std::vector<int> >();
    H.push_back(0);
    H.push_back(1);
    H.push_back(2);
    faces.push_back(std::vector<int>());
    faces[0].push_back(0);
    faces[0].push_back(1);
    faces[0].push_back(2);
    int n = points->NumPoints();
    std::cout << "ConvexHull::NumPoints n=" << n << std::endl;
    for(int i=3;i<n;i++) {
      Point pi = points->Pos(i);
      visible = std::vector<int>();
      int s = faces.size();
      std::cout << "ConvexHull s=" << s << std::endl;
      for(int j=0;j<s;j++) {
	int num = faces[j].size();
	std::cout << "NUM:" << num << std::endl;
	if (num==0) continue;
	float volume = 0.0;
	if (num==3) {
	  int i1 = faces[j][0];
	  int i2 = faces[j][1];
	  int i3 = faces[j][2];
	  Point p1 = points->Pos(H[i1]);
	  Point p2 = points->Pos(H[i2]);
	  Point p3 = points->Pos(H[i3]);
	  volume = signedVolume(p3,
				p2,
				p1,
				pi);
	  std::cout << "Volume: " << volume << "(" << i1 << ":" << p1 << "," << i2 << ":" << p2 << "," << i3 << ":" << p3 << "," << pi << std::endl;
	} else if (num==4) {
	  int i1 = faces[j][0];
	  int i2 = faces[j][1];
	  int i3 = faces[j][2];
	  //int i4 = faces[j][3];
	  Point p1 = points->Pos(H[i1]);
	  Point p2 = points->Pos(H[i2]);
	  Point p3 = points->Pos(H[i3]);
	  volume = signedVolume(p3,
				p2,
				p1,
				pi);
	  std::cout << "Volume: " << volume << "(" << i1 << ":" << p1 << "," << i2 << ":" << p2 << "," << i3 << ":" << p3 << "," << pi << std::endl;

	  /*+
	    signedVolume(points->Pos(H[i4]),
			 points->Pos(H[i3]),
			 points->Pos(H[i2]),
			 pi);*/
	}
	if (volume <= 0.0) // TODO, IS THIS CORRECT?
	  {
	    visible.push_back(j);
	  }
      }
      std::cout << "Visible size: " << visible.size() << std::endl;
      if (visible.size()==0)
	{
	  continue;
	}
      else
	{
	  int s = visible.size();
	  for(int k=0;k<s;k++) {
	    int s2 = faces[visible[k]].size();
	    for(int k2=0;k2<s2;k2++) {
	      int p1 = faces[visible[k]][k2];
	      int p2 = faces[visible[k]][(k2+1)%faces[visible[k]].size()];
	      H.push_back(H[p1]);
	      int ii1 = H.size()-1;
	      H.push_back(H[p2]);
	      int ii2 = H.size()-1;
	      H.push_back(i);
	      int ii3 = H.size()-1;
	      faces.push_back(std::vector<int>());
	      faces[faces.size()-1].push_back(ii1);
	      faces[faces.size()-1].push_back(ii2);
	      faces[faces.size()-1].push_back(ii3);
	    }
	  }
	  int s3 = visible.size();
	  for(int k3 = 0; k3<s3; k3++) {
	    faces[visible[k3]].clear();
	  }
	}
      
      
    }
  }
  void filter_empty() {
    faces_optimized = std::vector<std::vector<int> >();
    int s = faces.size();
    for(int i=0;i<s;i++) {
      if (faces[i].size()!=0) faces_optimized.push_back(faces[i]);
    }
  }
  void Collect(CollectVisitor &vis)
  {
    points->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (faces_optimized.size()==0) {
      //points->Prepare();
    calc_convex_hull();
    std::cout << "Convex_hull size: " << faces.size() << std::endl;
    filter_empty();
    std::cout << "Convex_hull size(optimized): " << faces_optimized.size() << std::endl;
    }
  }
  
  virtual void Prepare()
  {
    if (faces_optimized.size()==0) {
    points->Prepare();
    calc_convex_hull();
    std::cout << "Convex_hull size: " << faces.size() << std::endl;
    filter_empty();
    std::cout << "Convex_hull size(optimized): " << faces_optimized.size() << std::endl;
    }
  }
  virtual int NumFaces() const { return faces_optimized.size(); }
  virtual int NumPoints(int face) const
  {
    std::cout << "NumPoints:" << faces_optimized[face].size() << std::endl;
    return faces_optimized[face].size();
  }
  virtual Point FacePoint(int face, int point) const
  {
    std::cout << face << " " << point << "::" << faces_optimized[face][point] << "::" << H[faces_optimized[face][point]] << "::" << points->Pos(H[faces_optimized[face][point]]) << std::endl;
    return points->Pos(H[faces_optimized[face][point]]);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face, 0);
    Point p2 = FacePoint(face, 1);
    Point p3 = FacePoint(face, 2);
    Vector v = Vector::CrossProduct(p2-p1,p3-p1);
    v/=v.Dist();
    store_face=face;
    store_res=v;
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    return 0xffffffff;
    return points->Color(H[faces_optimized[face][point]]);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual float TexCoord3(int face, int point) const { return 0.0; }  
private:
  std::vector<int> H;
  std::vector<std::vector<int> > faces;
  std::vector<std::vector<int> > faces_optimized;
  std::vector<int> visible;
  std::vector<int> border_edge;
  Point pi;
  PointsApiPoints *points;
  mutable int store_face=-1;
  mutable Vector store_res;
};

GameApi::P GameApi::PolygonApi::convex_hull(PTS pts)
{
  PointsApiPoints *pts2 = find_pointsapi_points(e, pts);
  return add_polygon2(e, new ConvexHull(pts2),1);
}


#if 0
class Att : public Attach
{
public:
  Att(FaceCollection *coll, LineCollection *lines) : coll(coll), lines(lines) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    lines->Collect(vis);
  }
  void HeavyPrepare() { }
  
  void Prepare() {
    coll->Prepare();
    lines->Prepare();
  }
  int Attached(int face, int point) const
  {
    int s = lines->NumLines();
    Point p1a = coll->FacePoint(face,0);
    Point p2a = coll->FacePoint(face,1);
    Point p3a = coll->FacePoint(face,2);
    Point p4a;
    if (coll->NumPoints(face)==3) { Vector p3a_ = Vector(p1a+p2a+p3a); p3a_/=3; p4a = Point(p3a_); } else
    p4a = coll->FacePoint(face,3);
    Vector p2 = Vector(p1a+p2a+p3a+p4a);
    p2/=4.0;
    Point p = p2;
    
    float min_dist = 99999999.0;
    int min_item = 0;
    for(int i=0;i<s;i++) {
      //std::cout << "I=" << i << std::endl;
      Point p1 = lines->LinePoint(i, 0);
      Point p2 = lines->LinePoint(i, 1);
      LineProperties lp(p1,p2);

      Point mp = lp.MiddlePoint(0.5);
      float dd = (p2-p1).Dist()/2;
      if ((p-mp).Dist()>dd+lp.Dist(p)) continue;
      
#if 0      
      Vector v = p1 - p;
      float d2 = v.Dist();
      //std::cout << "D2: " << d2 << " " << min_dist << std::endl;
      if (d2<min_dist) { min_dist = d2; min_item=i; }
      Vector v_ = p2 - p;
      float d2_ = v_.Dist();
      //std::cout << "D2_: " << d2_ << " " << min_dist << std::endl;
      if (d2_<min_dist) { min_dist = d2_; min_item=i; }
#endif
      
      float d = lp.Dist(p);
      //std::cout << "D: " << d << " " << min_dist << std::endl;
      if (d<min_dist) { min_dist = d; min_item = i; }
    }
    return min_item;
  }
private:
  FaceCollection *coll;
  LineCollection *lines;
};
#endif

#if 0
class AttCache : public Attach
{
public:
  AttCache(Attach *next, FaceCollection *coll) : next(next), coll(coll) { }
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      std::vector<int> vec2;
      int sp = coll->NumPoints(i);
      for(int j=0;j<sp;j++) {
	vec2.push_back(next->Attached(i,j));
      }
      vec.push_back(vec2);
    }
  }
  virtual void Prepare() {
    next->Prepare();
    coll->Prepare();

    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      std::vector<int> vec2;
      int sp = coll->NumPoints(i);
      for(int j=0;j<sp;j++) {
	vec2.push_back(next->Attached(i,j));
      }
      vec.push_back(vec2);
    }
  }
  virtual int Attached(int face, int point) const
  {
    return vec[face][point];
  }
private:
  Attach *next;
  FaceCollection *coll;
  std::vector<std::vector<int> > vec;
};
#endif

GameApi::ATT GameApi::PolygonApi::find_attach2(P p, LI li)
{
#if 0
  FaceCollection *coll = find_facecoll(e,p);
  LineCollection *lines = find_line_array(e,li);

  return add_attach(e, new Att(coll,lines));
#endif
}
// NOTE, find_attach and attach_cache need to have the same P instance.
GameApi::ATT GameApi::PolygonApi::attach_cache(ATT a, P p)
{
#if 0
  Attach *att = find_attach(e, a);
  FaceCollection *coll = find_facecoll(e,p);
  return add_attach(e, new AttCache(att, coll));
#endif
}


#if 0
class SplitAttachFaces : public FaceCollection
{
public:
  SplitAttachFaces(FaceCollection *coll, Attach *att, int max_attach, int num, bool &prepared) : coll(coll), att(att), num(num), max_attach(max_attach),  prepared(prepared) { }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    att->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    prepared=true;
    
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      int a = att->Attached(i,0);
      if (a==num) { facemap.push_back(i); }
    }

  }
  
  virtual void Prepare() {
    if (!prepared) { coll->Prepare(); att->Prepare(); }
    prepared=true;
    
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      int a = att->Attached(i,0);
      if (a==num) { facemap.push_back(i); }
    }


  }
  virtual int NumFaces() const {
    int s = coll->NumFaces();
    int count=0;
    for(int i=0;i<s;i++) {
      int a = att->Attached(i,0);
      if (a==num) count++;
    }
    return count;
  }
  virtual int NumPoints(int face) const {
    if (facemap.size())
      return coll->NumPoints(facemap[face]);
    else return 0;
  }

  virtual bool has_normal() const { if (!coll||!facemap.size()) return false; return coll->has_normal(); }
  virtual bool has_attrib() const { if (!coll||!facemap.size()) return false;return coll->has_attrib(); }
  virtual bool has_color() const { if (!coll||!facemap.size()) return false; return coll->has_color(); }
  virtual bool has_texcoord() const { if (!coll||!facemap.size()) return false; return coll->has_texcoord(); }
  virtual bool has_skeleton() const { if (!coll||!facemap.size()) return false; return coll->has_skeleton(); }

  virtual Point FacePoint(int face, int point) const
  {
    if (facemap.size())
    return coll->FacePoint(facemap[face],point);
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (facemap.size())
    return coll->PointNormal(facemap[face], point);
    return Vector(0.0,0.0,0.0);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (facemap.size())    
    return coll->Attrib(facemap[face],point,id);
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (facemap.size())
    return coll->AttribI(facemap[face],point,id);
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  { 
    if (facemap.size())
      return coll->Color(facemap[face],point);
    return 0x000000;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (facemap.size())
    return coll->TexCoord(facemap[face],point);
    Point2d p;
    p.x = 0.0;
    p.y = 0.0;
    return p;
  }
  virtual float TexCoord3(int face, int point) const {
 
    if (facemap.size())
      return coll->TexCoord3(facemap[face],point);
    return 0.0;
  }
  
private:
  FaceCollection *coll;
  Attach *att;
  int num;
  int max_attach;
  std::vector<int> facemap;
  bool &prepared;
};
#endif
GameApi::P split_attach_faces(GameApi::Env &e, GameApi::P p, GameApi::ATT att, int max_attach, int num, bool &prepared)
{
#if 0
  FaceCollection *coll = find_facecoll(e, p);
  Attach *att2 = find_attach(e, att);
  return add_polygon2(e, new SplitAttachFaces(coll, att2, max_attach, num, prepared),1);
#endif
}

GameApi::ARR GameApi::PolygonApi::split_faces(P p, ATT att, int max_attach) // max_attach is number of lines in LI
{
  std::vector<GameApi::P> vec;
  int s = max_attach;
  ArrayType *array = new ArrayType;
  array->type =0;
  bool *g_prepared = new bool;
  *g_prepared = false;
  for(int i=0;i<s;i++)
    {
      GameApi::P pp = split_attach_faces(e,p, att, max_attach, i, *g_prepared);
      array->vec.push_back(pp.id);
    }
  GameApi::P pp2 = split_attach_faces(e,p,att,max_attach,-1,*g_prepared);
  array->vec.push_back(pp2.id);
  return add_array(e, array);
}

#if 0
class Pose2 : public FaceCollection
{
public:
  Pose2(GameApi::Env &env, GameApi::EveryApi &ev, MatrixArray *mat, std::vector<GameApi::P> vec, int i) : i(i), env(env), ev(ev), mat(mat), vec(vec) { }
  void Collect(CollectVisitor &vis)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	FaceCollection *coll = find_facecoll(env,vec[i]);
	coll->Collect(vis);
      }
    mat->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    
    GameApi::P p = vec[i];
    Matrix mm = mat->Index(i);
    GameApi::M m = add_matrix2(env,mm);
    GameApi::P p2 = ev.polygon_api.matrix44(p, m);
    res = p2;
    
  }
  void Prepare() {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	FaceCollection *coll = find_facecoll(env,vec[i]);
	coll->Prepare();
      }
    mat->Prepare();
    
    GameApi::P p = vec[i];
    Matrix mm = mat->Index(i);
    GameApi::M m = add_matrix2(env,mm);
    GameApi::P p2 = ev.polygon_api.matrix44(p, m);
    res = p2;
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

  
  virtual int NumFaces() const {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->NumFaces();
  }
  virtual int NumPoints(int face) const
  {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->NumPoints(face);
  }
  virtual Point FacePoint(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->FacePoint(face,point);
  }
  
  virtual Vector PointNormal(int face, int point) const
  {
    FaceCollection *coll = find_facecoll(env,res);
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
   FaceCollection *coll = find_facecoll(env,res);
   return coll->Attrib(face,point,id);
   }
  virtual int AttribI(int face, int point, int id) const
  {
   FaceCollection *coll = find_facecoll(env,res);
   return coll->AttribI(face,point,id);
   }
  virtual unsigned int Color(int face, int point) const
  {
   FaceCollection *coll = find_facecoll(env,res);
    return coll->Color(face,point);
 
  }
  virtual Point2d TexCoord(int face, int point) const
  {
   FaceCollection *coll = find_facecoll(env,res);
    return coll->TexCoord(face,point);
   }
  virtual float TexCoord3(int face, int point) const {
   FaceCollection *coll = find_facecoll(env,res);
    return coll->TexCoord3(face,point);
  }

  
private:
  int i;
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MatrixArray *mat;
  std::vector<GameApi::P> vec;

  GameApi::P res;
};
#endif

std::vector<GameApi::P> GameApi::PolygonApi::orig_pose2(EveryApi &ev, std::vector<P> vec, LI li, int li_size)
{
#if 0
  GameApi::MS ms_0 = ev.matrices_api.from_lines_3d(li);
  GameApi::MS ms_inv = ev.matrices_api.inverse_ms(ms_0);
  MatrixArray *mat = find_matrix_array(e, ms_inv);
    int s = vec.size();
    std::vector<P> res;
    for(int i=0;i<s;i++) {
      GameApi::P p = add_polygon2(e, new Pose2(e,ev,mat,vec,i),1);
      res.push_back(p);
    }
  return res;
#endif
}

std::vector<GameApi::P> GameApi::PolygonApi::orig_pose(EveryApi &ev, P p, LI li, int li_size)
{
  GameApi::MS ms_0 = ev.matrices_api.from_lines_3d(li);
  GameApi::MS ms_inv = ev.matrices_api.inverse_ms(ms_0);
  MatrixArray *mat = find_matrix_array(e, ms_inv);
  mat->Prepare();
  
  
  GameApi::ATT att = find_attach2(p, li);
  GameApi::ATT att2 = attach_cache(att,p);
  GameApi::ARR arr = split_faces(p, att2, li_size);
  ArrayType *arr2 = find_array(e, arr);
  int s3 = arr2->vec.size();
  std::vector<GameApi::P> split; 
  for(int i3=0;i3<s3;i3++) {
    GameApi::P p;
    p.id = arr2->vec[i3];
    split.push_back(p);
  }
  int s = split.size();
  std::vector<GameApi::P> res;
  for(int i=0;i<s;i++) {
    GameApi::P p = split[i];
    Matrix mm = i!=s-1?mat->Index(i):Matrix::Identity();
    GameApi::M m = add_matrix2(e,mm);
    GameApi::P p2 = matrix44(p, m);
    res.push_back(p2);
  }
  return res;
}
GameApi::MS ms_array(GameApi::Env &e, std::vector<Matrix> vec);


#if 0
class NewPose : public MatrixArray
{
public:
  NewPose(GameApi::Env &env, GameApi::EveryApi &ev, GameApi::LI li_orig, GameApi::LI li) : env(env), ev(ev), li_orig(li_orig), li(li) { }
  void Collect(CollectVisitor &vis)
  {
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    GameApi::MS ms = ev.matrices_api.from_lines_3d(li);
    MatrixArray *arr = find_matrix_array(env,ms);
    arr->Prepare();
    std::vector<Matrix> vec;
    for(int i=0;i<arr->Size();i++) { vec.push_back(arr->Index(i)); }
    vec.push_back(Matrix::Identity());
    res = ms_array(env,vec); //ev.matrices_api.mult_array(ms_inv, ms);

    MatrixArray *mat = find_matrix_array(env,res);
    mat->Prepare();
  }
  virtual void Prepare() {
    GameApi::MS ms = ev.matrices_api.from_lines_3d(li);
    MatrixArray *arr = find_matrix_array(env,ms);
    arr->Prepare();
    std::vector<Matrix> vec;
    for(int i=0;i<arr->Size();i++) { vec.push_back(arr->Index(i)); }
    vec.push_back(Matrix::Identity());
    res = ms_array(env,vec); //ev.matrices_api.mult_array(ms_inv, ms);

    MatrixArray *mat = find_matrix_array(env,res);
    mat->Prepare();
    
  }
  virtual void HandleEvent(MainLoopEvent &event) {
    MatrixArray *arr = find_matrix_array(env,res);
    arr->HandleEvent(event);
  }
  virtual bool Update(MainLoopEnv &e) {
    MatrixArray *arr = find_matrix_array(env,res);
    return arr->Update(e);


  }
  virtual int Size() const
  {
    MatrixArray *arr = find_matrix_array(env,res);
    return arr->Size();
  }
  virtual Matrix Index(int i) const
  {
    MatrixArray *arr = find_matrix_array(env,res);
    return arr->Index(i);

  }
  virtual unsigned int Color(int i) const {
    MatrixArray *arr = find_matrix_array(env,res);
    return arr->Color(i);
  }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  GameApi::LI li_orig;
  GameApi::LI li;

  GameApi::MS res;
};
#endif

GameApi::MS GameApi::PolygonApi::new_pose(EveryApi &ev, LI li_orig, LI li)
{
#if 0
  //GameApi::MS ms_0 = ev.matrices_api.from_lines_3d(li_orig);
  //GameApi::MS ms_inv = ev.matrices_api.inverse_ms(ms_0);
  return add_matrix_array(e, new NewPose(e,ev,li_orig,li));
#endif
}


GameApi::MS GameApi::PolygonApi::identity_pose(int li_size)
{
  std::vector<Matrix> vec;
  for(int i=0;i<li_size;i++) vec.push_back(Matrix::Identity());
  return ms_array(e,vec);
}

struct Rec
{
  int key;
  float duration;
  int start_index;
  int end_index;
};

class SkeletalAnim22 : public MainLoopItem
{
public:
  SkeletalAnim22(GameApi::Env &env, GameApi::EveryApi &ev, std::vector<GameApi::P> orig, std::vector<GameApi::MS> new_matrices, std::string url, std::string homepage, GameApi::P mesh, Material *mat) : env(env), ev(ev), orig(orig),  mesh(mesh), new_matrices(new_matrices), url(url), homepage(homepage),  mat(mat) { firsttime = true; }
  void Collect(CollectVisitor &vis)
  {
      int s4 = orig.size();
      for(int i=0;i<s4;i++) {
	FaceCollection *coll = find_facecoll(env, orig[i]);
	coll->Collect(vis);
      }
      FaceCollection *coll2 = find_facecoll(env, mesh);
      coll2->Collect(vis);


      int s = orig.size();
      for(int i=0;i<s;i++) {
	GameApi::ML ml = mat->mat(orig[i].id);
	orig2.push_back( ml );
	MainLoopItem *item = find_main_loop(env,ml);
	item->Collect(vis);
      }
      mesh2 = mat->mat(mesh.id);

      
    vis.register_obj(this);
  }
  void HeavyPrepare() {
    Prepare();
  }
  void Prepare() {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif
    GameApi::ASyncVec *vec = env.get_loaded_async_url(url);
    if (!vec) { std::cout << "async not ready!(SkeletalAnim22)" << std::endl; return; }
    std::string code(vec->begin(), vec->end());
    std::stringstream ss(code);
    std::string line;
    while(std::getline(ss,line)) {
      std::string word;
      std::stringstream ss2(line);
      ss2>>word;
      if (word=="key") {
	ss2 >> current_key;
      }
      if (word=="item") {
	Rec r;
	r.key = current_key;
	current_key = -1;
	ss2 >> r.duration;
	ss2 >> r.start_index;
	ss2 >> r.end_index;
	items.push_back(r);
      }

    }

  }

  void logoexecute() {
    mat->logoexecute();
  }
  virtual void execute(MainLoopEnv &e)
  {
    //std::cout << "SkeletalAnim44::execute() start" << std::endl;

    if (firsttime) {
    firsttime = false;
    }
    if (current_item!=-1) {
      int s = items.size();
      float t = ev.mainloop_api.get_time()-current_item_starttime;
      for(int i=current_item;i<s;i++) {
	if (items[i].key!=-1 && i!=current_item) { current_item = -1; break; }
	if (t>=0 && t<items[i].duration) {
	  float d = t/items[i].duration; // 0..1
	  int start = items[i].start_index;
	  int end = items[i].end_index;
	  int sz2 = new_matrices.size();
	  if (start<0 || start>=sz2)
	    {
	      std::cout << "START BROKEN!" << std::endl;
	      start =0;
	    }
	  int sz = new_matrices.size();
	  if (end<0 || end>=sz)
	    {
	      std::cout << "END BROKEN!" << std::endl;
	      end=0;
	    }
	  if (new_matrices.size()==0) {
	    std::cout << "Matrices_size==0" << std::endl;
	    return;
	  }
	  GameApi::MS start_ms = new_matrices[start];
	  GameApi::MS end_ms = new_matrices[end];
	  //std::cout << "POs:" << d << std::endl;
	  GameApi::MS middle_ms = ev.matrices_api.interpolate(start_ms, end_ms, d);
	  MatrixArray *arr = find_matrix_array(env, middle_ms);
	  arr->Prepare();
	  arr->Update(e);
	  int s3 = arr->Size();
	  int s4 = orig.size();
	  int s2 = std::min(s4,s3);
	  Matrix m2 = e.in_MV;
	  for(int i2=0;i2<s2;i2++) {
	    Matrix m = arr->Index(i2) * m2;
	    GameApi::ML ml = orig2[i2];
	    MainLoopItem *item = find_main_loop(env,ml);
	    MainLoopEnv ee = e;
	    ee.in_MV = m;
	    ee.env = m;
	    ee.has_inverse = true;
	    ee.in_iMV = Matrix::Transpose(Matrix::Inverse(m2)); // note, this is m2, not m.
	    ee.in_iMV.matrix[3]=0.0;
	    ee.in_iMV.matrix[4*1+3]=0.0;
	    ee.in_iMV.matrix[4*2+3]=0.0;
	    ee.in_iMV.matrix[4*3+0]=0.0;
	    ee.in_iMV.matrix[4*3+1]=0.0;
	    ee.in_iMV.matrix[4*3+2]=0.0;
	    ee.in_iMV.matrix[4*3+3]=1.0;
	    
	    //ee.in_iMV.set_translate(Vector(0.0,0.0,0.0));
	    item->execute(ee);
	  }
	  return;
	}
	t-=items[i].duration;
      }
    }
    MainLoopItem *item = find_main_loop(env,mesh2);
    item->execute(e);
    //std::cout << "SkeletalAnim44::execute() end" << std::endl;
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = items.size();
    for(int i=0;i<s;i++) {
      if (items[i].key==e.ch && e.type == 0x300 && e.ch!=-1 && e.ch!=key_pressed) {
	current_item = i;
	current_item_starttime = ev.mainloop_api.get_time();
	key_pressed = e.ch;
      }
      if (e.type==0x301 && e.ch==key_pressed) key_pressed = -1;
      
    }
  }
  virtual std::vector<int> shader_id() {
    int s = orig2.size();
    std::vector<int> res;
    for(int i=0;i<s;i++)
      {
	MainLoopItem *item = find_main_loop(env, orig2[i]);
	std::vector<int> sh = item->shader_id();
	int s2 = sh.size();
	for(int j=0;j<s2;j++) res.push_back(sh[j]);
      }
    MainLoopItem *item = find_main_loop(env,mesh2);
    std::vector<int> sh2 = item->shader_id();
	int s2 = sh2.size();
	for(int j=0;j<s2;j++) res.push_back(sh2[j]);
    return res;
  }
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  std::vector<GameApi::P> orig;
  std::vector<GameApi::ML> orig2;
  GameApi::P mesh;
  GameApi::ML mesh2;
  std::vector<GameApi::MS> new_matrices;
  std::string url;
  std::string homepage;
  int current_key;
  std::vector<Rec> items;
  int current_item = -1;
  float current_item_starttime = 0.0;
  bool firsttime;
  Material *mat;
  int key_pressed = -1;
  GameApi::MS inverse_mat;
};

class MatrixElem44 : public ForwardFaceCollection
{
public:
  MatrixElem44(FaceCollection &next, Matrix m) : ForwardFaceCollection(next), next(&next), m(m) {  }
  virtual std::string name() const { return "MatrixElem44"; }

  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual int NumFaces() const { return next->NumFaces(); }
  virtual int NumPoints(int face) const { return next->NumPoints(face); }
  virtual Point FacePoint(int face, int point) const
  {
    Point p = next->FacePoint(face,point);
    return p*m; 
  }
  virtual Point EndFacePoint(int face, int point) const
  {
    Point p = next->EndFacePoint(face,point);
    return p*m;  
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Vector v = next->PointNormal(face,point);
    return v;
  }
private:
  FaceCollection *next;
  Matrix m;
};
GameApi::P GameApi::PolygonApi::matrix44(P orig, M mat)
{
    Matrix mat2 = find_matrix(e, mat);
  //::EnvImpl *env = ::EnvImpl::Environment(&e); 
  FaceCollection *c = find_facecoll(e, orig);
  //BoxableFaceCollectionConvert *convert = new BoxableFaceCollectionConvert(*c);
  //env->deletes.push_back(std::shared_ptr<void>(convert));  
  if (!c) { std::cout << "dynamic cast failed" << std::endl; }
  FaceCollection *coll = new MatrixElem44(*c, mat2);
  return add_polygon2(e, coll,1);

}


GameApi::ML GameApi::PolygonApi::ske_anim2(EveryApi &ev, std::vector<P> mesh, LI orig_pose2_, int li_size, std::string url, std::vector<LI> new_poses, MT mat)
{
  Material *mat2 = find_material(e,mat);
  std::vector<GameApi::P> orig = orig_pose2(ev, mesh, orig_pose2_, li_size);
  std::vector<MS> new_matrices;
  int s = new_poses.size();
  int sz = mesh.size();
  if (s!=sz) { std::cout << "ske_anim2::[LI] size: " << new_poses.size() << " but meshes: " << mesh.size() << std::endl; }
  for(int i=0;i<s;i++) {
    new_matrices.push_back(new_pose(ev, orig_pose2_, new_poses[i]));
  }
  P mesh2 = ev.polygon_api.or_array2(mesh);
  return add_main_loop(e, new SkeletalAnim22(e, ev, orig, new_matrices, url, gameapi_homepageurl, mesh2,mat2));

}

GameApi::ML GameApi::PolygonApi::ske_anim(EveryApi &ev, P mesh, LI orig_pose2, int li_size, std::string url, std::vector<LI> new_poses, MT mat)
{
  Material *mat2 = find_material(e,mat);
  std::vector<GameApi::P> orig = orig_pose(ev, mesh, orig_pose2, li_size);
  std::vector<MS> new_matrices;
  int s = new_poses.size();
  for(int i=0;i<s;i++) {
    new_matrices.push_back(new_pose(ev, orig_pose2, new_poses[i]));
  }
  return add_main_loop(e, new SkeletalAnim22(e, ev, orig, new_matrices, url, gameapi_homepageurl, mesh,mat2));
}


struct RayRes
{
  int face;
  Point pos;
};

class RemoveFaces : public FaceCollection
{
public:
  RemoveFaces(FaceCollection *next) : coll(next) { firsttime=true; }
  virtual std::string name() const { return "RemoveFaces"; }

  virtual bool has_normal() const { return true; }
  virtual bool has_attrib() const { return false; }
  virtual bool has_color() const { return false; }
  virtual bool has_texcoord() const { return true; }
  virtual bool has_skeleton() const { return false; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    if (firsttime) {
      firsttime=false;
      //coll->Prepare();
      int s = coll->NumFaces();
      for(int i=0;i<s;i++)
	{
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,1);
	Point p3 = coll->FacePoint(i,2);
	Point p4 = coll->FacePoint(i,3);
	float min_x = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
	float max_x = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
	float min_y = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
	float max_y = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
	float min_z = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));
	float max_z = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));
	float volume = std::max(std::max(max_x-min_x,max_y-min_y),(max_z-min_z));
	if (volume > 30.0) 
	  vec.push_back(i);
	}
      std::cout << "NumFaces:" << vec.size() << std::endl;
    }

  }
  
  void Prepare() {
    if (firsttime) {
      firsttime=false;
      coll->Prepare();
      int s = coll->NumFaces();
      for(int i=0;i<s;i++)
	{
	Point p1 = coll->FacePoint(i,0);
	Point p2 = coll->FacePoint(i,1);
	Point p3 = coll->FacePoint(i,2);
	Point p4 = coll->FacePoint(i,3);
	float min_x = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
	float max_x = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
	float min_y = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
	float max_y = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
	float min_z = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));
	float max_z = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));
	float volume = std::max(std::max(max_x-min_x,max_y-min_y),(max_z-min_z));
	if (volume > 30.0) 
	  vec.push_back(i);
	}
      std::cout << "NumFaces:" << vec.size() << std::endl;
    }
  }
  virtual int NumFaces() const { return vec.size(); }
  virtual int NumPoints(int face) const
  {
    return coll->NumPoints(vec[face]);
  }

  virtual Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(vec[face],point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(vec[face],point);
  }
  virtual float Attrib(int face, int point, int id) const {
    return coll->Attrib(vec[face],point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(vec[face],point,id);
  }
    
  virtual unsigned int Color(int face, int point) const
  {
    return coll->Color(vec[face],point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(vec[face],point);
  }
  virtual float TexCoord3(int face, int point) const {
    return coll->TexCoord3(vec[face],point);
  }
  virtual int NumTextures() const { return coll->NumTextures(); }
  virtual void GenTexture(int num) { coll->GenTexture(num); }
  virtual BufferRef TextureBuf(int num) const {
    return coll->TextureBuf(num);
  }
  virtual int FaceTexture(int face) const { return coll->FaceTexture(face); }

private:
  bool firsttime;
  FaceCollection *coll;
  std::vector<int> vec;
};
GameApi::P GameApi::PolygonApi::remove_faces(GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new RemoveFaces(coll),1);
}

FaceCollection *g_faces;
Vector g_faces_vec;
float EvalFaces(int id)
{
  Point p1 = g_faces->FacePoint(id,0);
  Point p2 = g_faces->FacePoint(id,1);
  Point p3 = g_faces->FacePoint(id,2);
  Point p4 = g_faces->FacePoint(id,3);
  Point center1 = Vector(p1+Vector(p2)+Vector(p3)+Vector(p4))*(1.0/4.0);
  float dist = Vector::FindProjectionLength(Vector(center1),g_faces_vec);
  return dist;
}
bool CompareFaces(int id1, int id2)
{
  float dist = EvalFaces(id1);
  float dist2 = EvalFaces(id2);
  return dist<dist2;
}

#if 0
class SortFaces : public CollectInterface
{
public:
  SortFaces(FaceCollection *coll, Vector dir) : coll(coll), dir(dir) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    Prepare();
  }
  void Prepare()
  {
    coll->Prepare();
    g_faces_vec = dir;
    g_faces = coll;
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      vec.push_back(i);
    }
    std::sort(vec.begin(),vec.end(), CompareFaces);
  }
  int find_pos2(float target_pos, int start_pos, int end_pos) const
  {
  loop:
    //std::cout << "find_pos:" << start_pos << ".." << end_pos << std::endl;
    if (end_pos-start_pos<2) return start_pos;
    int pos = start_pos + (end_pos-start_pos)/2;
    int pos2 = vec[pos];
    float val = EvalFaces(pos2);
    if (val>target_pos) {
      //end_pos/=2;
      end_pos = pos;
      goto loop;
    } else if (val<target_pos) {      
      start_pos = pos;
      goto loop;
    }
    return start_pos;
  }
  int find_pos(Point p) const
  {
    float target_pos = Vector::FindProjectionLength(Vector(p),g_faces_vec);
    int start_pos = 0;
    int end_pos = vec.size();
    return find_pos2(target_pos, start_pos,end_pos);
  }
  
  
public:
  FaceCollection *coll;
  std::vector<int> vec;
  Vector dir;
};
#endif
#if 0
class FaceCollectionRays : public CollectInterface
{
public:
  FaceCollectionRays(FaceCollection *coll, Vector v) : coll(coll), v(v) { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    sort = new SortFaces(coll,v);
    sort->Prepare();
  }
  void Prepare()
  {
    coll->Prepare();
    //int s = coll->NumFaces();
    //std::cout << "NumFaces: " << vec.size() << std::endl;
    sort = new SortFaces(coll,v);
    sort->Prepare();
  }
  RayRes intersect(Point pos) const
  {
    LineProperties lp(pos,pos+v);
    //int s = coll->NumFaces();
    float min_r = 10000000.0;
    int res = -1;
    int start_pos = sort->find_pos(pos);
    int end_pos = sort->find_pos(pos+v/v.Dist()*30.0);
    //std::cout << "intersect: " << start_pos << " " << end_pos << std::endl;
    for(int ii=start_pos;ii<end_pos;ii++) {
      int i = sort->vec[ii];
      Point p1 = coll->FacePoint(i,0);
      Point p2 = coll->FacePoint(i,1);
      Point p3 = coll->FacePoint(i,2);
      Point p4 = coll->FacePoint(i,3);
      float r = 0.0;
      bool succ = lp.QuadIntersection(p1,p2,p3,p4, r);
      if (succ && r<min_r && r>0.0) { min_r=r; res=i; }
    }
    RayRes rr;
    rr.face = res;
    rr.pos = pos + min_r*v;
    return rr;
  }
  float dist_intersect(Point pos) const
  {
    RayRes r = intersect(pos);
    Vector vv = r.pos - pos;
    return vv.Dist();
  }
  float color_intersect(Point pos, float black_dist, float white_dist) const
  {
    float d=dist_intersect(pos);
    d-=black_dist;
    d/=(white_dist-black_dist);
    if (d<0.0) d=0.0;
    if (d>1.0) d=1.0;
    return d;
  }
private:
  FaceCollection *coll;
  std::vector<int> vec;
  Vector v;
  SortFaces *sort;
};
#endif

#if 0
class LightMapBitmap : public Bitmap<Color>
{
public:
  LightMapBitmap(int sx, int sy, FaceCollection *coll, FaceCollection *coll2, int face, Vector light_dir) : coll(coll), sx(sx),sy(sy), r(coll,light_dir), coll2(coll2), face(face), light_dir(light_dir) { }
  void setsize(int sx_, int sy_) { sx=sx_; sy=sy_; }
  virtual int SizeX() const { return sx; }
  virtual int SizeY() const { return sy; }
  virtual Color Map(int x, int y) const
  {
    Vector v_x = p2-p1;
    Vector v_y = p4-p1;
    float xx = float(x)/sx;
    float yy = float(y)/sy;
    Point pos = p1 + xx*v_x + yy*v_y;
    float c = r.color_intersect(pos, 0.0, 300.0);
    return Color(c,c,c,1.0f);
  }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    coll2->Collect(vis);
    r.Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    p1 = coll2->FacePoint(face,0);
    p2 = coll2->FacePoint(face,1);
    p3 = coll2->FacePoint(face,2);
    p4 = coll2->FacePoint(face,3);
    t1 = coll2->TexCoord(face,0);
    t2 = coll2->TexCoord(face,1);
    t3 = coll2->TexCoord(face,2);
    t4 = coll2->TexCoord(face,3);
    //float v_x = (p3.x-p1.x)/(t3.x-t1.x);
    //float p0_x = p1.x-t1.x*v_x;
    //float v_y = (p3.y-p1.y)/(t3.x-t1.x);
    //float p0_y = p1.y-t1.x*v_y;
    //float v_z = (p3.z-p1.z)/(t3.x-t1.x);
    //float p0_z = p1.z-t1.x*v_z;

    //float av_x = (p3.x-p1.x)/(t3.y-t1.y);
    //float ap0_x = p1.x-t1.y*av_x;
    //float av_y = (p3.y-p1.y)/(t3.y-t1.y);
    //float ap0_y = p1.y-t1.y*av_y;
    //float av_z = (p3.z-p1.z)/(t3.y-t1.y);
    //float ap0_z = p1.z-t1.y*av_z;

    //Point pp = Point(p0_x,p0_y,p0_z);
    //Vector v = Vector(v_x,v_y,v_z);
    //Vector av = Vector(av_x,av_y,av_z);
    //p1 = pp;
    //p2 = pp+v;
    //p3 = pp+v+av;
    //p4 = pp+av;

  }
  virtual void Prepare()
  {
    coll->Prepare();
    coll2->Prepare();
    r.Prepare();
    p1 = coll2->FacePoint(face,0);
    p2 = coll2->FacePoint(face,1);
    p3 = coll2->FacePoint(face,2);
    p4 = coll2->FacePoint(face,3);
    t1 = coll2->TexCoord(face,0);
    t2 = coll2->TexCoord(face,1);
    t3 = coll2->TexCoord(face,2);
    t4 = coll2->TexCoord(face,3);
    //float v_x = (p3.x-p1.x)/(t3.x-t1.x);
    //float p0_x = p1.x-t1.x*v_x;
    //float v_y = (p3.y-p1.y)/(t3.x-t1.x);
    //float p0_y = p1.y-t1.x*v_y;
    //float v_z = (p3.z-p1.z)/(t3.x-t1.x);
    //float p0_z = p1.z-t1.x*v_z;

    //float av_x = (p3.x-p1.x)/(t3.y-t1.y);
    //float ap0_x = p1.x-t1.y*av_x;
    //float av_y = (p3.y-p1.y)/(t3.y-t1.y);
    //float ap0_y = p1.y-t1.y*av_y;
    //float av_z = (p3.z-p1.z)/(t3.y-t1.y);
    //float ap0_z = p1.z-t1.y*av_z;

    //Point pp = Point(p0_x,p0_y,p0_z);
    //Vector v = Vector(v_x,v_y,v_z);
    //Vector av = Vector(av_x,av_y,av_z);
    //p1 = pp;
    //p2 = pp+v;
    //p3 = pp+v+av;
    //p4 = pp+av;
    
  }

private:
  FaceCollection *coll;
  int sx, sy;
  FaceCollectionRays r;
  FaceCollection *coll2;
  int face;
  Point p1,p2,p3,p4;
  Point2d t1,t2,t3,t4;
  Vector light_dir;
};
#endif
GameApi::BM GameApi::BitmapApi::lightmap_bitmap(int sx, int sy, P faces, P faces2, int face, float light_dir_x, float light_dir_y, float light_dir_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e,faces);
  FaceCollection *coll2 = find_facecoll(e, faces2);
  Bitmap<Color> *b = new LightMapBitmap(sx,sy,coll,coll2,face,Vector(light_dir_x, light_dir_y, light_dir_z));
  BitmapColorHandle *handle2 = new BitmapColorHandle;
  handle2->bm = b;
  BM bm = add_bitmap(e, handle2);
  return bm;
#endif
}

class CollectBitmap : public CollectInterface
{
public:
  CollectBitmap(FaceCollection *coll, std::vector<Bitmap<Color>*> vec, int tx3) : tx3(tx3), coll(coll), vec(vec) { }
  virtual ~CollectBitmap() { }
  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    int sx = vec[0]->SizeX();
    int sy = vec[0]->SizeY();
    ref = BufferRef::NewBuffer(sx,sy);
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	ref.buffer[x+y*ref.ydelta] = 0xffffffff;
    int s = vec.size();
    
    for(int i=0;i<s;i++) copy_tri(i);
  }
  void Prepare()
  {
    coll->Prepare();
    int s = vec.size();
    for(int i=0;i<s;i++) vec[i]->Prepare();

    int sx = vec[0]->SizeX();
    int sy = vec[0]->SizeY();
    ref = BufferRef::NewBuffer(sx,sy);
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	ref.buffer[x+y*ref.ydelta] = 0xffffffff;
    
    for(int i=0;i<s;i++) copy_tri(i);
    
  }
  void copy_tri(int face)
  {
    float tx3_ = coll->TexCoord3(face,0);
    if (tx3_ < float(tx3)) return;
    if (tx3_ > float(tx3+1)) return;

    std::cout << "copy_tri: " << face << "/" << coll->NumFaces() << std::endl;
    
    Point2d t1 = coll->TexCoord(face,0);
    Point2d t2 = coll->TexCoord(face,1);
    Point2d t3 = coll->TexCoord(face,2);
    Point2d t4 = coll->TexCoord(face,3);
    int sx = ref.width;
    int sy = ref.height;

    float min_x = std::min(std::min(t1.x,t2.x),std::min(t3.x,t4.x));
    float max_x = std::max(std::max(t1.x,t2.x),std::max(t3.x,t4.x));
    float min_y = std::min(std::min(t1.y,t2.y),std::min(t3.y,t4.y));
    float max_y = std::max(std::max(t1.y,t2.y),std::max(t3.y,t4.y));
    
    min_x*=sx;
    max_x*=sx;
    min_y*=sy;
    max_y*=sy;
    int min_xx = std::max(int(min_x),0);
    int max_xx = std::min(int(max_x),sx);
    int min_yy = std::max(int(min_y),0);
    int max_yy = std::min(int(max_y),sy);

    std::cout << "copy_tri: " << min_xx << ".." << max_xx << "x" << min_yy << ".." << max_yy << std::endl;
    
    
    for(int y=min_yy;y<max_yy;y++)
      for(int x=min_xx;x<max_xx;x++)
	{
	  Point2d pt = { float(x)/sx,float(y)/sy };
	  bool b1 = inside(pt, t1,t2,t3);
	  bool b2 = inside(pt, t1,t3,t4);
	  if (b1||b2)
	    {
	      ref.buffer[x+y*ref.ydelta] = vec[face]->Map(x,y).Pixel();
	    }
	}
    
  }
  float sign(Point2d p1, Point2d p2, Point2d p3)
  {
    return (p1.x-p3.x)*(p2.y-p3.y)-(p2.x-p3.x)*(p1.y-p3.y);
  }
  bool inside(Point2d pt, Point2d v1, Point2d v2, Point2d v3)
  {
    float d1 = sign(pt,v1,v2);
    float d2 = sign(pt,v2,v3);
    float d3 = sign(pt,v3,v1);
    bool has_neg = (d1<0) || (d2<0) || (d3<0);
    bool has_pos = (d1>0) || (d2>0) || (d3>0);
    return !(has_neg && has_pos);
  }
public:
  int tx3;
  BufferRef ref;
  FaceCollection *coll;
  std::vector<Bitmap<Color>*> vec;
};

#if 0
class SponzaFaceCollection : public ForwardFaceCollection
{
public:
  SponzaFaceCollection(FaceCollection *coll, float light_dir_x, float light_dir_y, float light_dir_z) : ForwardFaceCollection(*coll), coll(coll), light_dir_x(light_dir_x), light_dir_y(light_dir_y), light_dir_z(light_dir_z) { }
  ~SponzaFaceCollection() {
    int s = bms.size();
    for(int i=0;i<s;i++) delete bms[i];
  }
  void Collect(CollectVisitor &vis)
  {
    ForwardFaceCollection::Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    int s = coll->NumTextures();
    std::vector<Bitmap<::Color>*> vec;
    FaceCollection *coll2 = new RemoveFaces(coll);
    coll2->Prepare();
    int s2 = coll2->NumFaces();
    for(int j=0;j<s2;j++) {
      vec.push_back(new LightMapBitmap(1,1, coll2, coll2, j, Vector(light_dir_x, light_dir_y, light_dir_z)));
    }
    for(int i=0;i<s;i++) {
      coll->GenTexture(i);
      BufferRef ref = coll->TextureBuf(i);
      int sx = ref.width;
      int sy = ref.height;
      //vec= std::vector<Bitmap<::Color>*>();
      for(int j=0;j<s2;j++) {
	((LightMapBitmap*)vec[j])->setsize(sx,sy);
      }
      bms.push_back(new CollectBitmap(coll2, vec, i));
      std::cout << "Sponza: "<< i << "/" << s << std::endl;
      bms[i]->Prepare();
    }
  }
  void Prepare() {
    ForwardFaceCollection::Prepare();
    int s = coll->NumTextures();
    std::vector<Bitmap<::Color>*> vec;
    FaceCollection *coll2 = new RemoveFaces(coll);
    coll2->Prepare();
    int s2 = coll2->NumFaces();
    for(int j=0;j<s2;j++) {
      vec.push_back(new LightMapBitmap(1,1, coll2, coll2, j, Vector(light_dir_x, light_dir_y, light_dir_z)));
    }
    for(int i=0;i<s;i++) {
      coll->GenTexture(i);
      BufferRef ref = coll->TextureBuf(i);
      int sx = ref.width;
      int sy = ref.height;
      //vec= std::vector<Bitmap<::Color>*>();
      for(int j=0;j<s2;j++) {
	((LightMapBitmap*)vec[j])->setsize(sx,sy);
      }
      bms.push_back(new CollectBitmap(coll2, vec, i));
      std::cout << "Sponza: "<< i << "/" << s << std::endl;
      bms[i]->Prepare();
    }
    //for(int i=0;i<s;i++) {
    // }
  }
  virtual int NumTextures() const { return coll->NumTextures(); }
  virtual void GenTexture(int num) {
  }
  virtual BufferRef TextureBuf(int num) const { return bms[num]->ref; }
private:
  FaceCollection *coll;
  std::vector<CollectBitmap*> bms;
  float light_dir_x, light_dir_y, light_dir_z;
};
#endif
GameApi::P GameApi::PolygonApi::slow_calc_lights(GameApi::P p, float light_dir_x, float light_dir_y, float light_dir_z)
{
#if 0
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new SponzaFaceCollection(coll, light_dir_x, light_dir_y, light_dir_z),1);
#endif
}

class CombineTextures : public ForwardFaceCollection
{
public:
  CombineTextures(FaceCollection *coll1, FaceCollection *coll2) : ForwardFaceCollection(*coll1), coll1(coll1), coll2(coll2) { }
  virtual std::string name() const { return "CombineTextures"; }

  void Collect(CollectVisitor &vis)
  {
    coll1->Collect(vis);
    coll2->Collect(vis);
  }
  void HeavyPrepare() { }
  virtual void Prepare()
  {
    coll1->Prepare();
    coll2->Prepare();
  }
  virtual int NumTextures() const { return std::min(coll1->NumTextures(), coll2->NumTextures()); }
  virtual void GenTexture(int num) {
    coll1->GenTexture(num);
    coll2->GenTexture(num);
  }
  virtual BufferRef TextureBuf(int num) const {
    int sz = vec.size();
    if (sz<=num) vec.resize(num+1);
    BufferRef ref1 = coll1->TextureBuf(num);
    BufferRef ref2 = coll2->TextureBuf(num);
    int sx = std::min(ref1.width,ref2.width);
    int sy = std::min(ref1.height,ref2.height);
    vec[num] = BufferRef::NewBuffer(sx,sy);
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	vec[num].buffer[x+y*vec[num].ydelta] = combine(ref1.buffer[x+y*ref1.ydelta], ref2.buffer[x+y*ref2.ydelta]);
      
    return vec[num];
  }
  unsigned int combine(unsigned int c1, unsigned int c2) const
  {
    unsigned int c1a = c1&0xff000000;
    unsigned int c1r = c1&0xff0000;
    unsigned int c1g = c1&0xff00;
    unsigned int c1b = c1&0xff;

    unsigned int c2a = c2&0xff000000;
    unsigned int c2r = c2&0xff0000;
    unsigned int c2g = c2&0xff00;
    unsigned int c2b = c2&0xff;

    float c1aa = float(c1a>>24)/255.0;
    float c1rr = float(c1r>>16)/255.0;
    float c1gg = float(c1g>>8)/255.0;
    float c1bb = float(c1b)/255.0;


    float c2aa = float(c2a>>24)/255.0;
    float c2rr = float(c2r>>16)/255.0;
    float c2gg = float(c2g>>8)/255.0;
    float c2bb = float(c2b)/255.0;


    float maa = (c1aa+c2aa)/2.0;
    float mrr = c1rr*c2rr;
    float mgg = c1gg*c2gg;
    float mbb = c1bb*c2bb;

    maa *=255.0;
    mrr *=255.0;
    mgg *=255.0;
    mbb *=255.0;

    unsigned int ma = (unsigned int)(maa);
    unsigned int mr = (unsigned int)(mrr);
    unsigned int mg = (unsigned int)(mgg);
    unsigned int mb = (unsigned int)(mbb);
    
    ma<<=24;
    mr<<=16;
    mg<<=8;
    return ma+mr+mg+mb;
  }
private:
  FaceCollection *coll1, *coll2;
  mutable std::vector<BufferRef> vec;
};

GameApi::P GameApi::PolygonApi::combine_textures(GameApi::P p1, GameApi::P p2)
{
  FaceCollection *coll1 = find_facecoll(e,p1);
  FaceCollection *coll2 = find_facecoll(e,p2);
  return add_polygon2(e, new CombineTextures(coll1, coll2),1);
}

#if 0

class PolygonHeightMap : public Bitmap<Color>
{
public:
  PolygonHeightMap(FaceCollection *coll, int sx, int sy, Point start, Point end) : coll(coll) { }

  void Prepare() {
    int s = coll->NumFaces();
    for(int i=0;i<s;i++) {
      int count = coll->NumPoints(i);
      if (count==3)
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  Point2d pp1 = { p1.x, p1.z };
	  Point2d pp2 = { p2.x, p2.z };
	  Point2d pp3 = { p3.x, p3.z };
	  bms.push_back(new TriangleContinuousBitmap(sx,sy, pp1, pp2, pp3));
	}
      else if (count==4)
	{
	  Point p1 = coll->FacePoint(i,0);
	  Point p2 = coll->FacePoint(i,1);
	  Point p3 = coll->FacePoint(i,2);
	  Point p4 = coll->FacePoint(i,3);
	  Point2d pp1 = { p1.x, p1.z };
	  Point2d pp2 = { p2.x, p2.z };
	  Point2d pp3 = { p3.x, p3.z };
	  Point2d pp4 = { p4.x, p4.z };
	  bms.push_back(new QuadContinuousBitmap(sx,sy, pp1, pp2, pp3,pp4));
	}
    }
  }
private:
  FaceCollection *coll;
  std::vector<ContinuousBitmap<bool> *> bms;
};

GameApi::BM GameApi::PolygonApi::polygon_heightmap(GameApi::P p)
{
}
#endif

Matrix g_in_P;
bool g_in_P_used=false;

class PerspectiveMainLoop : public MainLoopItem
{
public:
  PerspectiveMainLoop(GameApi::EveryApi &ev, MainLoopItem *next, float mult, float front_plane, float end_plane) : ev(ev), next(next), mult(mult), front_plane(front_plane), end_plane(end_plane) { }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv &ee = e;
    Matrix old_inP = ee.in_P;
    Matrix m = Matrix::Perspective(mult, (double)800/(double)600, front_plane, end_plane); 
    ee.in_P = m;

    Matrix gp = g_in_P;
    g_in_P = m;
    g_in_P_used = true;

    
    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s11;
    s11.id = e.sh_texture_2d;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_P", add_matrix2(ev.get_env(),m));
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_P", add_matrix2(ev.get_env(),m));
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_P", add_matrix2(ev.get_env(),m));
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_P", add_matrix2(ev.get_env(),m)); 
    std::vector<int> v=next->shader_id();
    int s = v.size();
    for(int i=0;i<s;i++)
      {
	GameApi::SH s1;
	s1.id = v[i];
	ev.shader_api.use(s1);
	ev.shader_api.set_var(s1,"in_P", add_matrix2(ev.get_env(),m));
      }
   
    next->execute(ee);
    ee.in_P = old_inP;
    
    g_in_P_used = false;
    g_in_P = gp;
  }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
  virtual void destroy() { }

private:
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  float mult;
  float front_plane;
  float end_plane;
};

GameApi::ML GameApi::MainLoopApi::perspective(EveryApi &ev, ML next, float mult, float front_plane, float end_plane)
{
  MainLoopItem *item = find_main_loop(e,next);
  return add_main_loop(e, new PerspectiveMainLoop(ev,item,mult,front_plane, end_plane));
}


class MouseRollZoom : public MainLoopItem
{
public:
  MouseRollZoom(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next) : e2(e), ev(ev), next(next) { }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    ee.in_MV = mat * e.in_MV;
    ee.env = mat * e.env;

    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s11;
    s11.id = e.sh_texture_2d;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;


    GameApi::M mat2 = add_matrix2(e2,ee.in_MV);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);


    next->execute(ee);
    counter++;
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
    if (e.type==Low_SDL_MOUSEWHEEL && e.ch==1 /*&& counter>3*/)
      {
	zoom_pos--; if (zoom_pos<-1) zoom_pos=-1;
	calc_mat();
	counter=0;
      }
    if (e.type==Low_SDL_MOUSEWHEEL && e.ch==-1 /*&& counter>3*/)
      {
	zoom_pos++; if (zoom_pos>5) zoom_pos=5;
	calc_mat();
	counter=0;
      }
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
  virtual void destroy() { }
  void calc_mat()
  {
    float scale = 1.0;
    if (zoom_pos<0) { scale-=0.18*(-zoom_pos); }
    if (zoom_pos>0) { scale+=0.3*zoom_pos; }
    mat = Matrix::Scale(scale,scale,scale);
  }
private:
  GameApi::Env &e2;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int zoom_pos = 0;
  Matrix mat = Matrix::Identity();
  int counter=0;
};

class MouseRollZoom2 : public MainLoopItem
{
public:
  MouseRollZoom2(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next) : e2(e), ev(ev), next(next) { }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    ee.in_MV = e.in_MV * mat;
    ee.env = e.env * mat;

    GameApi::SH s1;
    s1.id = e.sh_texture;
    GameApi::SH s11;
    s11.id = e.sh_texture_2d;
    GameApi::SH s2;
    s2.id = e.sh_array_texture;
    GameApi::SH s3;
    s3.id = e.sh_color;


    GameApi::M mat2 = add_matrix2(e2,ee.in_MV);
    //GameApi::M mat2i = ev.matrix_api.transpose(ev.matrix_api.inverse(mat2));
    ev.shader_api.use(s1);
    ev.shader_api.set_var(s1, "in_MV", mat2);
    //ev.shader_api.set_var(s1, "in_iMV", mat2i);
    ev.shader_api.use(s11);
    ev.shader_api.set_var(s11, "in_MV", mat2);
    //ev.shader_api.set_var(s11, "in_iMV", mat2i);
    ev.shader_api.use(s2);
    ev.shader_api.set_var(s2, "in_MV", mat2);
    //ev.shader_api.set_var(s2, "in_iMV", mat2i);
    ev.shader_api.use(s3);
    ev.shader_api.set_var(s3, "in_MV", mat2);
    //ev.shader_api.set_var(s3, "in_iMV", mat2i);


    next->execute(ee);
    counter++;
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
    if (e.type==Low_SDL_MOUSEWHEEL && e.ch==1 /*&& counter>3*/)
      {
	zoom_pos--; if (zoom_pos<-13) zoom_pos=-13;
	calc_mat();
	counter=0;
      }
    if (e.type==Low_SDL_MOUSEWHEEL && e.ch==-1 /*&& counter>3*/)
      {
	zoom_pos++; if (zoom_pos>5) zoom_pos=5;
	calc_mat();
	counter=0;
      }
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
  virtual void destroy() { }
  void calc_mat()
  {
    float trans = 0.0;
    //if (zoom_pos<0) { scale-=0.18*(-zoom_pos); }
    //if (zoom_pos>0) { scale+=0.3*zoom_pos; }
    if (zoom_pos<0) { trans-=80.0*(-zoom_pos); }
    if (zoom_pos>0) { trans+=80.0*zoom_pos; }
    mat = Matrix::Translate(0.0,0.0,trans);
  }
private:
  GameApi::Env &e2;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  int zoom_pos = 0;
  Matrix mat = Matrix::Identity();
  int counter=0;
};


GameApi::ML GameApi::MainLoopApi::mouse_roll_zoom(EveryApi &ev, ML next)
{
  MainLoopItem *next2 = find_main_loop(e,next);
  return add_main_loop(e,new MouseRollZoom(e,ev,next2));
}
GameApi::ML GameApi::MainLoopApi::mouse_roll_zoom2(EveryApi &ev, ML next)
{
  MainLoopItem *next2 = find_main_loop(e,next);
  return add_main_loop(e,new MouseRollZoom2(e,ev,next2));
}

class RightMousePan : public MainLoopItem
{
public:
  RightMousePan(GameApi::Env &e, GameApi::EveryApi &ev, MainLoopItem *next) : e2(e), ev(ev), next(next) { firsttime = true; }
  virtual void logoexecute() { }
  virtual void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void execute(MainLoopEnv &e)
  {
    MainLoopEnv ee = e;
    ee.in_T = e.in_T * mat;

    next->execute(ee);
  }
  virtual void handle_event(MainLoopEvent &e) {
    next->handle_event(e);
    if (e.button==2 && oldbutton!=2) {
      old_x = e.cursor_pos.x;
      old_y = e.cursor_pos.y;
    }
    if (e.button==2)
      {
	float dx = e.cursor_pos.x - old_x;
	float dy = e.cursor_pos.y - old_y;

	pan_x += dx;
	pan_y += dy;
	if (pan_x<-600.0) pan_x=-600.0;
	if (pan_x>600.0) pan_x=600.0;
	if (pan_y<-600.0) pan_y=-600.0;
	if (pan_y>600.0) pan_y=600.0;
	calc_mat();
	old_x = e.cursor_pos.x;
	old_y = e.cursor_pos.y;
      }
    oldbutton = e.button;
  }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
  virtual void destroy() { }
  void calc_mat()
  {
    mat = Matrix::Translate(pan_x,-pan_y,0.0);
  }

private:
  GameApi::Env &e2;
  GameApi::EveryApi &ev;
  MainLoopItem *next;
  float pan_x = 0.0, pan_y = 0.0;
  Matrix mat = Matrix::Identity();
  bool firsttime;
  int oldbutton = 0;
  float old_x, old_y;
};

GameApi::ML GameApi::MainLoopApi::right_mouse_pan(EveryApi &ev, ML next)
{
  MainLoopItem *next2 = find_main_loop(e,next);
  return add_main_loop(e, new RightMousePan(e,ev,next2));
}

int g_global_face_count = 0;

class PublishFaceCount : public ForwardFaceCollection
{
public:
  PublishFaceCount(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual std::string name() const { return "PublishFaceCount"; }

  void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  void HeavyPrepare()
  {
    g_global_face_count = coll->NumFaces();
  }
  void Prepare() {
    coll->Prepare();
    g_global_face_count = coll->NumFaces();
  }
  bool HasBatchMap() const {
    return coll->HasBatchMap();
  }
  virtual FaceBufferRef BatchMap(int start_face, int end_face) const
  {
    FaceBufferRef r = coll->BatchMap(start_face,end_face);
    return r;
  }


  
private:
  FaceCollection *coll;
};

GameApi::P GameApi::PolygonApi::get_face_count(P p)
{ // publishes vertex count to js
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new PublishFaceCount(coll),1);
}


#if 0
class CombineAnim : public ForwardFaceCollection
{
public:
  CombineAnim(FaceCollection *coll1, FaceCollection *coll2) : ForwardFaceCollection(*coll1), coll2(coll2) { }
  virtual Point EndFacePoint(int face, int point) const { return coll2->FacePoint(face, point); }
private:
  FaceCollection *coll2;
  
};
#endif

extern bool g_use_vertices_only;

class AnimRender3 : public MainLoopItem
{
public:
  AnimRender3(GameApi::EveryApi &ev, TimeAnim *a, float delta) : ev(ev), a(a), delta(delta) { comb = 0; set = 0; render = 0; }
  void prepare_start(float time_1, float time_2)
  {
    start_time = time_1;
    end_time = time_2;

    start_coll = a->get_frame(start_time);
    end_coll = a->get_frame(end_time);

    delete comb;
    //comb = new CombineAnim(start_coll, end_coll);

    delete render;
    render = new RenderVertexArray(g_low, *set);

    delete arr;
    arr = new FaceCollectionVertexArray2(*comb, *set);
    last_prepare_time = start_time;
    set->clear_poly_and_poly2(0);
  }
  void prepare_frame(float time)
  {
    int s = comb->NumFaces();
    float d = time - last_prepare_time;
    float total = end_time-start_time;
    float start = last_prepare_time - start_time;
    float sz = d/total;
    if (start<0.0) start=0.0;
    if (start>1.0) start=1.0;
    if (sz<0.0) sz=0.0;
    if (start+sz>1.0) sz=1.0-start;
    start*=s;
    sz*=s;
    g_use_vertices_only = true;
    arr->copy(int(start), int(sz), std::vector<int>(), std::vector<int>());
    g_use_vertices_only = false;
    last_prepare_time = time;
  }
  void switch_to_next()
  {
    render_start_time = start_time;
    render_end_time = end_time;

    render->prepare(0, false, -1, -1, -1);
    
  }


  virtual void Collect(CollectVisitor &vis) {
    a->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    float s = a->start_time();
    float e = start_time + delta;
    prepare_start(s, e);
    delete set;
    set = new VertexArraySet;
  }
  virtual void Prepare() {
    a->Prepare();
    float s = a->start_time();
    float e = start_time + delta;
    prepare_start(s, e);
    delete set;
    set = new VertexArraySet;
  }
  virtual void execute(MainLoopEnv &e)
  {
    float time = e.time;
    prepare_frame(time);

    if (time>=start_time)
      {
	switch_to_next();
	prepare_start(end_time, end_time+delta);
      }
    float interpolate = (time-render_start_time)/(render_end_time-render_start_time);

    if (sh.id == -1) {

    GameApi::US u_v;
    GameApi::US u_f;
    u_v.id = 0;
    u_f.id = 0;
    if (e.us_vertex_shader!=-1)
      u_v.id = e.us_vertex_shader;
    if (e.us_fragment_shader!=-1)
      u_f.id = e.us_fragment_shader;
    if (e.us_vertex_shader==-1)
      {
	u_v = ev.uber_api.v_colour(u_v);
	u_v = ev.uber_api.v_light(u_v);
      }
    if (e.us_fragment_shader==-1)
      {
	u_f = ev.uber_api.f_colour(u_f);
	u_f = ev.uber_api.f_light(u_f);
      }    
    GameApi::US vertex;
    GameApi::US fragment;
    vertex.id = u_v.id; //e.us_vertex_shader;
    fragment.id = u_f.id; //e.us_fragment_shader;
    sh = ev.shader_api.get_normal_shader("comb", "comb", "", vertex, fragment,e.v_shader_functions, e.f_shader_functions);
    }

    ev.shader_api.use(sh);
    ev.shader_api.set_var(sh, "in_POS", interpolate);

    render->render(0);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
  

private:
  GameApi::EveryApi &ev;
  TimeAnim *a;
  Material *mat;
  float start_time, end_time;
  float delta;
  float last_prepare_time;
  float render_start_time, render_end_time;
  FaceCollection *start_coll, *end_coll;
  FaceCollection *comb;
  VertexArraySet *set;
  RenderVertexArray *render;
  FaceCollectionVertexArray2 *arr;
  GameApi::SH sh;
};

//GameApi::ML GameApi::PolygonApi::anim_render(AA a, float delta)
//{
//}


#if 0
class CombineAnim2 : public ForwardFaceCollection
{
public:
  CombineAnim2(FaceCollection *coll1, FaceCollection *coll2, float start_time, float end_time) : ForwardFaceCollection(*coll1), coll2(coll2), start_time(start_time), end_time(end_time) { }
  virtual Point EndFacePoint(int face, int point) const { return coll2->FacePoint(face,point); }
  virtual float StartTime() const { return start_time; }
  virtual float Duration() const { return end_time-start_time; }
  virtual void setup_time(float time) { }
private:
  FaceCollection *coll2;
  float start_time, end_time;
};
#endif

GameApi::P GameApi::PolygonApi::combine_anim(P p1, P p2, float start_time, float end_time)
{
#if 0
  FaceCollection *pp1 = find_facecoll(e,p1);
  FaceCollection *pp2 = find_facecoll(e,p2);
  return add_polygon2(e, new CombineAnim2(pp1,pp2,start_time,end_time),1);
#endif
}

GameApi::P GameApi::PolygonApi::sphere_anim(float c_x, float c_y, float c_z,
					    float c2_x, float c2_y, float c2_z,
					    float r, float r2, float start_time, float end_time, int numfaces1, int numfaces2)
{
  PT center = add_point(e, c_x, c_y, c_z);
  PT center2 = add_point(e, c2_x, c2_y, c2_z);
  P sph1 = sphere(center, r, numfaces1, numfaces2);
  P sph2 = sphere(center2, r2, numfaces1, numfaces2);
  return combine_anim(sph1, sph2, start_time, end_time);
}

GameApi::P GameApi::PolygonApi::cone_anim(float c_x, float c_y, float c_z,
					  float d_x, float d_y, float d_z,
					  float c2_x, float c2_y, float c2_z,
					  float d2_x, float d2_y, float d2_z,
					  float r1, float r2,
					  float r1_2, float r2_2, int numfaces, float start_time, float end_time)
{
  PT center = add_point(e, c_x, c_y, c_z);
  PT center_d = add_point(e, d_x, d_y, d_z);
  PT center2 = add_point(e, c2_x, c2_y, c2_z);
  PT center_d2 = add_point(e, d2_x, d2_y, d2_z);

  P sph1 = cone(numfaces, center, center_d, r1, r2);
  P sph2 = cone(numfaces, center2, center_d2, r1_2, r2_2);
  return combine_anim(sph1, sph2, start_time, end_time);
}

GameApi::P GameApi::PolygonApi::cube_anim(float start_x, float end_x,
					  float start_y, float end_y,
					  float start_z, float end_z,
					  float start_x2, float end_x2,
					  float start_y2, float end_y2,
					  float start_z2, float end_z2,
					  float start_time, float end_time)
{
  GameApi::P cube1 = cube(start_x, end_x,
			  start_y, end_y,
			  start_z, end_z);
  GameApi::P cube2 = cube(start_x2, end_x2,
			  start_y2, end_y2,
			  start_z2, end_z2);
  
  return combine_anim(cube1, cube2, start_time, end_time);
}
					  
//GameApi::ARR GameApi::PolygonApi::or_elem_anim(P p1, P p2, float time)
//{
//x}

class FaceCollectionCutter : public ForwardFaceCollection
{
public:
  FaceCollectionCutter(FaceCollection *coll, float start, float end) : ForwardFaceCollection(*coll), coll(coll), start(start), end(end) { }
  virtual std::string name() const { return "FaceCollectionCutter"; }

  virtual int NumFaces() const { return std::max(0,EndFace()-StartFace()); }
  virtual int NumPoints(int face) const
  {
    face+=StartFace();
    return coll->NumPoints(face);
  }

  virtual Point FacePoint(int face, int point) const
  {
    face+=StartFace();
    return coll->FacePoint(face,point);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    face+=StartFace();
    return coll->PointNormal(face,point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    face+=StartFace();
    return coll->Attrib(face,point,id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    face+=StartFace();
    return coll->AttribI(face,point,id);
  }
  virtual unsigned int Color(int face, int point) const
  {
    face+=StartFace();
    return coll->Color(face,point);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    face+=StartFace();
    return coll->TexCoord(face,point);
  }
  virtual float TexCoord3(int face, int point) const
  {
    face+=StartFace();
    return coll->TexCoord3(face,point);
  }
  virtual VEC4 Joints(int face, int point) const
  {
    face+=StartFace();
    return coll->Joints(face,point);
  }
  virtual VEC4 Weights(int face, int point) const
  {
    face+=StartFace();
    return coll->Weights(face,point);
  }

  int NumObjects() const { return coll->NumObjects(); }
  virtual std::pair<int,int> GetObject(int o) const {
    std::pair<int,int> p = coll->GetObject(o);
    if (p.first < StartFace()) p.first=StartFace();
    if (p.first > EndFace()) p.first=EndFace();
    if (p.second < StartFace()) p.second=StartFace();
    if (p.second > EndFace()) p.second=EndFace();
    p.first-=StartFace();
    p.second-=StartFace();
    return p;
  }
  
  
  int StartFace() const {
    if (start_face!=-1) return start_face;
    return start_face=int(float(coll->NumFaces())*start);
  }
  int EndFace() const {
    if (end_face!=-1) return end_face;
    return end_face=int(float(coll->NumFaces())*end);
  }


  
private:
  FaceCollection *coll;
  float start, end;
  mutable int start_face=-1;
  mutable int end_face=-1;
};
GameApi::P GameApi::PolygonApi::face_cutter(P p, float start, float end)
{
  FaceCollection *coll = find_facecoll(e, p);
  return add_polygon2(e, new FaceCollectionCutter(coll,start,end),1);
}


class AmbientOcculsion : public ForwardFaceCollection
{
public:
  AmbientOcculsion(FaceCollection *next) : ForwardFaceCollection(*next), next(next) { }
  virtual std::string name() const { return "AmbientOcculsion"; }

  struct Res
  {
    bool b;
    int face;
    int point;
  };
  Res find_match(int face, int point) const
  {
    Point px = next->FacePoint(face,point);
    int faces = next->NumFaces();
    for(int f=0;f<faces;f++) {
      if (f==face) continue;
      int points = next->NumPoints(f);
      for(int p=0;p<points;p++)
	{
	  Point p2 = next->FacePoint(f,p);
	  if (fabs(px.x-p2.x)<0.1 && fabs(px.y-p2.y)<0.1 && fabs(px.z-p2.z)<0.1) { Res r; r.b = true; r.face=f; r.point=p; return r; }
	}
    }
    Res r2;
    r2.b = false;
    return r2;
  }
  virtual unsigned int Color(int face, int point) const
  {
    Res r = find_match(face,point);
    if (r.b==true)
      {
	int face1 = face;
	int face2 = r.face;
	int point1 = point;
	int point2 = r.point;
	//Point p1 = next->FacePoint(face1,point1);
	Point p2 = next->FacePoint(face2,point2);
	//Point pp1 = next->FacePoint(face1,(point1-1)%next->NumPoints(face1));
	Point pp2 = next->FacePoint(face2,(point2+1)%next->NumPoints(face2));
	//Vector v1 = pp1-p1;
	Vector v2 = pp2-p2;

	Vector vv = next->PointNormal(face1,point1);
	v2/=v2.Dist();
	vv/=vv.Dist();
	float a = Vector::Angle(vv,v2);
	if (fabs(a)<3.14159/2.0) return 0xff000000;
	if (a>=3.14159*2.0-3.14159/2.0) return 0xff000000;
      }
    return 0xffffffff;
  }
private:
  FaceCollection *next;
};
GameApi::P GameApi::PolygonApi::ambient_occulsion_color(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e,new AmbientOcculsion(coll),1);
}

class XSplit : public ForwardFaceCollection
{
public:
  XSplit(FaceCollection *coll, float x, float x_0, float x_1) : ForwardFaceCollection(*coll), coll(coll), x(x), x_0(x_0), x_1(x_1) { }
  virtual std::string name() const { return "XSplit"; }

  Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    if (p.x<x) {
      p.x-=x;
      p.x+=x_0;
    } else
      {
	p.x-=x;
	p.x+=x_1;
      }
    return p;
  }
private:
  FaceCollection *coll;
  float x,x_0,x_1;
};

class YSplit : public ForwardFaceCollection
{
public:
  YSplit(FaceCollection *coll, float y, float y_0, float y_1) : ForwardFaceCollection(*coll), coll(coll), y(y), y_0(y_0), y_1(y_1) { }
  virtual std::string name() const { return "YSplit"; }

  Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    if (p.y<y) {
      p.y-=y;
      p.y+=y_0;
    } else
      {
	p.y-=y;
	p.y+=y_1;
      }
    return p;
  }
private:
  FaceCollection *coll;
  float y,y_0,y_1;
};

class ZSplit : public ForwardFaceCollection
{
public:
  ZSplit(FaceCollection *coll, float z, float z_0, float z_1) : ForwardFaceCollection(*coll), coll(coll), z(z), z_0(z_0), z_1(z_1) { }
  virtual std::string name() const { return "ZSplit"; }
  Point FacePoint(int face, int point) const
  {
    Point p = coll->FacePoint(face,point);
    if (p.z<z) {
      p.z-=z;
      p.z+=z_0;
    } else
      {
	p.z-=z;
	p.z+=z_1;
      }
    return p;
  }
private:
  FaceCollection *coll;
  float z,z_0,z_1;
};



GameApi::P GameApi::PolygonApi::x_split(P p, float x, float x_0, float x_1)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new XSplit(coll,x,x_0,x_1),1);
}
GameApi::P GameApi::PolygonApi::y_split(P p, float y, float y_0, float y_1)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new YSplit(coll,y,y_0,y_1),1);
}
GameApi::P GameApi::PolygonApi::z_split(P p, float z, float z_0, float z_1)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new ZSplit(coll,z,z_0,z_1),1);
}

class DisablePolygons : public MainLoopItem
{
public:
  DisablePolygons(MainLoopItem *next) : next(next) { }
  virtual void Collect(CollectVisitor &vis) {
    g_disable_polygons = true;
    next->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() { g_disable_polygons = false; }
  virtual void Prepare() {
    g_disable_polygons = true;
    next->Prepare();
    HeavyPrepare();
  }
  virtual void execute(MainLoopEnv &e)
  {
    g_disable_polygons = true;
    next->execute(e);
    g_disable_polygons = false;
  }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};

GameApi::ML GameApi::MainLoopApi::disable_polygons(GameApi::ML ml)
{
  MainLoopItem *next = find_main_loop(e,ml);
  return add_main_loop(e, new DisablePolygons(next));
}

class ExtractLargePolygons : public FaceCollection
{
public:
  ExtractLargePolygons(FaceCollection *coll, float size, bool reverse) : coll(coll),size(size),reverse(reverse) { }
  virtual std::string name() const { return "ExtractLargePolygons"; }

  void Collect(CollectVisitor &vis) { coll->Collect(vis); }
  void HeavyPrepare() { }
  void Prepare() { coll->Prepare(); }

  
  int NumFaces() const
  {
    if (vec2.size()>0) return numfaces=vec2[vec2.size()-1];
    const_cast<ExtractLargePolygons*>(this)->create_vec();
    if (vec2.size()>0)
      return numfaces=vec2[vec2.size()-1];
    return 0;
  }
    int NumPoints(int face) const
  {
    return coll->NumPoints(Mapping(face));
  }
  Point FacePoint(int face, int point) const
  {
    return coll->FacePoint(Mapping(face), point);
  }
  Point EndFacePoint(int face, int point) const
  {
    return coll->EndFacePoint(Mapping(face), point);
  }
  unsigned int Color(int face, int point) const
  {
    return coll->Color(Mapping(face), point);
  }
  Point2d TexCoord(int face, int point) const
  {
    return coll->TexCoord(Mapping(face), point);
  }
  float TexCoord3(int face, int point) const
  {
    return coll->TexCoord3(Mapping(face), point);
  }

  Vector PointNormal(int face, int point) const
  {
    return coll->PointNormal(Mapping(face), point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return coll->Attrib(Mapping(face), point, id);
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return coll->AttribI(Mapping(face),point,id);
  }
  virtual VEC4 Joints(int face, int point) const {
    return coll->Joints(Mapping(face),point);
  }
  virtual VEC4 Weights(int face, int point) const {
    return coll->Weights(Mapping(face),point);
  }
  int NumObjects() const { return coll->NumObjects(); }
  std::pair<int,int> GetObject(int o) const
  {
    if (vec.size()==0)
      const_cast<ExtractLargePolygons*>(this)->create_vec();
    std::pair<int,int> p = coll->GetObject(o);
    
    
    return std::make_pair(ReverseMapping(p.first),ReverseMapping(p.second));
  }

  virtual bool has_normal() const { return coll->has_normal(); }
  virtual bool has_attrib() const { return coll->has_attrib(); }
  virtual bool has_color() const { return coll->has_color(); }
  virtual bool has_texcoord() const { return coll->has_texcoord(); }
  virtual bool has_skeleton() const { return coll->has_skeleton(); }

  
  int Mapping(int ii) const
  {
    if (ii>numfaces) return 0;
    if (vec.size()>0) return vec[ii];
    const_cast<ExtractLargePolygons*>(this)->create_vec();
    return vec[ii];
  }

  int ReverseMapping(int i) const
  {
    return rev[i];
  }
  
  bool is_large(int face) const
  {
    int count = coll->NumPoints(face);
    if (count==3||count==4) {
      Point p1 = coll->FacePoint(face,0);
      Point p2 = coll->FacePoint(face,1);
      Point p3 = coll->FacePoint(face,2);
      Point p4 = coll->FacePoint(face,3%count);
      float x_min = std::min(std::min(p1.x,p2.x),std::min(p3.x,p4.x));
      float y_min = std::min(std::min(p1.y,p2.y),std::min(p3.y,p4.y));
      float z_min = std::min(std::min(p1.z,p2.z),std::min(p3.z,p4.z));
      float x_max = std::max(std::max(p1.x,p2.x),std::max(p3.x,p4.x));
      float y_max = std::max(std::max(p1.y,p2.y),std::max(p3.y,p4.y));
      float z_max = std::max(std::max(p1.z,p2.z),std::max(p3.z,p4.z));
      return (x_max-x_min)>size || (y_max-y_min)>size || (z_max-z_min)>size;
    } else return false;
  }

  void create_vec()
  {
    int count=0;
    int num = coll->NumFaces();
    //if (num>10000) num=10000;
    for(int i=0;i<num;i++)
      {
	rev.push_back(count);
	bool b = is_large(i);
	if (reverse) b=!b;
	if (b)
	  {
	    //if (count == ii) return i;
	    vec.push_back(i);
	    vec2.push_back(count);
	    count++;
	  }
      }
  }
private:
  FaceCollection *coll;
  float size;
  bool reverse;
  std::vector<int> vec;
  std::vector<int> vec2;
  std::vector<int> rev;
  mutable int numfaces=0;
};

GameApi::P GameApi::PolygonApi::extract_large_polygons(GameApi::P p, float minimum_size, bool reverse)
{
  FaceCollection *faces = find_facecoll(e,p);
  return add_polygon2(e, new ExtractLargePolygons(faces,minimum_size,reverse),1);
}

#if 0
class MaterialFaceCollection : public FaceCollection
{
public:
  MaterialFaceCollection(FaceCollection *base, FaceCollection *material) : base(base), material(material) { }
  virtual void Collect(CollectVisitor &vis) {
    base->Collect(vis);
    material->Collect(vis);
  }
  virtual void HeavyPrepare() { }
  virtual void Prepare()
  {
    base->Prepare();
    material->Prepare();
    HeavyPrepare();
  }

  virtual int NumFaces() const { return material->NumFaces(); }
  virtual int NumPoints(int face) const
  {
    return material->NumPoints(face);
  }
  
  virtual Point FacePoint(int face, int point) const
  {
    Point pos = material->FacePoint(face,point);
    Point2d txcoord = { pos.x, pos.z };
    float height = pos.y;

    txcoord.x /=300.0;
    txcoord.y /=300.0;
    height/=300.0;
    
    int s = base->NumFaces();
    for(int i=0;i<s;i++)
      {
	if (base->NumPoints(i)==3) {
	  Point2d tx0 = base->TexCoord(i,0);
	  Point2d tx1 = base->TexCoord(i,1);
	  Point2d tx2 = base->TexCoord(i,2);

	  bool b1 = PointIsInsideTriangle(tx0,tx1,tx2, txcoord);
	  if (!b1) continue;
	  Point bary = TriangleProperties::texture_to_barycentric(tx0,tx1,tx2,txcoord);
	  
	  TriangleProperties prop(base->FacePoint(i,0),base->FacePoint(i,1), base->FacePoint(i,2));
	  Point pt = prop.barycentric_to_point(bary);

	  TriangleProperties prop2(Point(base->PointNormal(i,0)),Point(base->PointNormal(i,1)), Point(base->PointNormal(i,2)));
	  Point normal0 = prop2.barycentric_to_point(bary);
	  
	  Vector normal = normal0;
	  pt += height*normal;
	  return pt;
	  
	} else if (base->NumPoints(i)==4) {
	  Point2d a_tx0 = base->TexCoord(i,0);
	  Point2d a_tx1 = base->TexCoord(i,1);
	  Point2d a_tx2 = base->TexCoord(i,2);


	  bool b1 = PointIsInsideTriangle(a_tx0,a_tx1,a_tx2, txcoord);
	  if (b1) {
	    Point bary = TriangleProperties::texture_to_barycentric(a_tx0,a_tx1,a_tx2,txcoord);
	  
	    TriangleProperties prop(base->FacePoint(i,0),base->FacePoint(i,1), base->FacePoint(i,2));
	    Point pt = prop.barycentric_to_point(bary);
	    
	    TriangleProperties prop2(Point(base->PointNormal(i,0)),Point(base->PointNormal(i,1)), Point(base->PointNormal(i,2)));
	    Point normal0 = prop2.barycentric_to_point(bary);
	    
	    Vector normal = normal0;
	    
	    pt += height*normal;
	    
	    return pt;
	  }

	  
	  Point2d b_tx1 = base->TexCoord(i,1);
	  Point2d b_tx2 = base->TexCoord(i,2);
	  Point2d b_tx3 = base->TexCoord(i,3);


	  bool b2 = PointIsInsideTriangle(b_tx1,b_tx2,b_tx3, txcoord);
	  if (b2) {
	    Point bary = TriangleProperties::texture_to_barycentric(b_tx1,b_tx2,b_tx3,txcoord);
	  
	    TriangleProperties prop(base->FacePoint(i,1),base->FacePoint(i,2), base->FacePoint(i,3));
	    Point pt = prop.barycentric_to_point(bary);
	    
	    TriangleProperties prop2(Point(base->PointNormal(i,1)),Point(base->PointNormal(i,2)), Point(base->PointNormal(i,3)));
	    Point normal0 = prop2.barycentric_to_point(bary);
	    
	    Vector normal = normal0;
	    
	    pt += height*normal;
	    
	    return pt;
	  } // if (b1)

	  
	} //numpoints==4
      } // for
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (store_face==face) return store_res;
    Point p1 = FacePoint(face,0);
    Point p2 = FacePoint(face,1);
    Point p3 = FacePoint(face,2);
    Vector v=-Vector::CrossProduct(p2-p1,p3-p1);
    store_face=face;
    store_res=v;
    return v;
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const { return material->Color(face,point); }
  virtual Point2d TexCoord(int face, int point) const { return material->TexCoord(face,point); }
  virtual float TexCoord3(int face, int point) const { return material->TexCoord3(face,point); }
  virtual VEC4 Joints(int face, int point) const { return material->Joints(face,point); }
  virtual VEC4 Weights(int face, int point) const { return material->Weights(face,point); }

private:
  FaceCollection *base;
  FaceCollection *material;
  mutable int store_face=-1;
  mutable Vector store_res;
};
#endif

GameApi::P GameApi::PolygonApi::material_face_collection(P base, P material)
{
#if 0
  FaceCollection *coll = find_facecoll(e,base);
  FaceCollection *coll2 = find_facecoll(e,material);
  return add_polygon2(e, new MaterialFaceCollection(coll,coll2),1);
#endif
}


/*
float calc_size(FaceCollection *coll, int obj)
{
  std::pair<int,int> p = coll->GetObject(obj);
  int start = p.first;
  int end = p.second;
  float size = 0;
  for(int f=start;f<end;f++) {
    int num = coll->NumPoints(f);
    if (num==0) num=1;
    Point p1 = coll->FacePoint(f,0);
    Point p2 = coll->FacePoint(f,1);
    Point p3 = coll->FacePoint(f,2);
    Point p4 = coll->FacePoint(f,3%num);
    Vector v1 = p2-p1;
    Vector v2 = p3-p2;
    Vector v3 = p4-p3;
    Vector v4 = p1-p4;
    float d1 = v1.Dist();
    float d2 = v2.Dist();
    float d3 = v3.Dist();
    float d4 = v4.Dist();
    float mm = std::max(std::max(d1,d2),std::max(d3,d4));
    if (mm>size) size=mm;
  }
  return size;
}

struct K
{
  FaceCollection *coll;
  int i;
  mutable float size_cache=-666.0;
};
bool operator<(const K &k1, const K &k2)
{
  float s1 = k1.size_cache>-600.0?k1.size_cache:calc_size(k1.coll,k1.i);
  float s2 = k2.size_cache>-600.0?k2.size_cache:calc_size(k2.coll,k2.i);
  k1.size_cache=s1;
  k2.size_cache=s2;
  return s1>s2;
}


class SortObjects : public ForwardFaceCollection
{
public:
  SortObjects(FaceCollection *coll) : ForwardFaceCollection(*coll), coll(coll) { }
  virtual void Collect(CollectVisitor &vis) {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
    int s = coll->NumObjects();
    for(int i=0;i<s;i++) {
      K k = { coll, i, -666.0 };
      vec.push_back(k);
    }
    std::sort(vec.begin(),vec.end());
  }
  virtual void Prepare()
  {
    coll->Prepare();
    HeavyPrepare();
  }
  int NumObjects() const { return vec.size(); }
  std::pair<int,int> GetObject(int o) const {
    int i = vec[o].i;
    std::pair<int,int> p = coll->GetObject(i);
    return p;
  }
   
private:
  FaceCollection *coll;
  std::vector<K> vec;
};

GameApi::ARR GameApi::PolygonApi::sort_objects_based_on_polygon_size(GameApi::P p, std::vector<P> p_vec, std::vector<MT> mat_arr1, std::vector<MT> mat_arr2, std::vector<BM> bmarr)
{
  SortObjects *objs = new SortObjects;
  GameApi::P p2 = add_polygon2(e, objs);

  ArrayType *array2 = new ArrayType;
  array2->type=0;
  array2->vec.push_back();
  GameApi::ARR arr = add_array(e,array2);
  
  ArrayType *array = new ArrayType;
  array->type=0;
  array->vec.push_back(p2.id);
  array->vec.push_back(arr.id);
  return add_array(e,array);
}
*/

#if 0
class PTSWorld : public IWorld
{
public:
  PTSWorld(PointsApiPoints *points, int type) : points(points), type(type) { }
  virtual int NumBlocks() const { return points->NumPoints(); }
  virtual int BlockType(int i) const { return type; }
  virtual Point BlockPos(int i) const { return points->Pos(i); }
  virtual Matrix BlockRot(int i) const { return Matrix::Identity(); }  
private:
  PointsApiPoints *points;
  int type;
};
#endif

GameApi::W GameApi::MainLoopApi::pts_world(PTS p, int type)
{
#if 0
  PointsApiPoints *points = find_pointsapi_points(e,p);
  return add_world(e, new PTSWorld(points,type));
#endif
}

#if 0
class SingleCache : public ICache
{
public:
  SingleCache(FaceCollection *coll, Material *mat) : coll(coll), mat(mat) { }
  int Num() const { return 1; }
  FaceCollection *GetFaces(int i) const { return coll; }
  Material *GetMaterial(int i) const { return mat; }
private:
  FaceCollection *coll;
  Material *mat;
};
#endif

GameApi::CX GameApi::MainLoopApi::cache_one(P p, MT mat2)
{
#if 0
  FaceCollection *coll = find_facecoll(e,p);
  Material *mat = find_material(e,mat2);
  return add_cache(e, new SingleCache(coll,mat));
#endif
}


#if 0
class ArrayCache : public ICache
{
public:
  ArrayCache(std::vector<ICache*> vec) : vec(vec) { }
  int Num() const {
    int count = 0;
    int s = vec.size();
    for(int i=0;i<s;i++) count+=vec[i]->Num();
    return count;
  }
  FaceCollection *GetFaces(int i) const {
    int count = 0;
    int s = vec.size();
    for(int ii=0;ii<s;ii++) {
      int c = vec[ii]->Num();
      if (i>=count && i<count+c) {
	return vec[ii]->GetFaces(i-count);
      }
      count+=c;
    }
    return 0;
  }
  Material *GetMaterial(int i) const
  {
    int count=0;
    int s = vec.size();
    for(int ii=0;ii<s;ii++) {
      int c = vec[ii]->Num();
      if (i>=count && i<count+c) {
	return vec[ii]->GetMaterial(i-count);
      }
      count+=c;
    }
    return 0;
  }
private:
  std::vector<ICache*> vec;
};
#endif
GameApi::CX GameApi::MainLoopApi::array_cache(std::vector<CX> vec)
{
#if 0
  std::vector<ICache*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    {
      ICache *cc = find_cache(e,vec[i]);
      vec2.push_back(cc);
    }
  return add_cache(e, new ArrayCache(vec2));
#endif
}

#if 0
class SubWorldCache : public ICache
{
public:
  SubWorldCache(ICache *cache, IWorld *world, int start_block, int end_block)
    : cache(cache), world(world), start_block(start_block), end_block(end_block) { }
  int Num() const { return end_block-start_block; }
  FaceCollection *GetFaces(int i) const
  {
    Matrix m = world->BlockRot(start_block+i);
    int type = world->BlockType(start_block+i);
    Point pos = world->BlockPos(start_block+i);
    FaceCollection *coll = cache->GetFaces(type);
    FaceCollection *coll2 = new MatrixElem(*coll,m*Matrix::Translate(pos.x,pos.y,pos.z));
    return coll2;
  }
  Material *GetMaterial(int i) const
  {
    int type = world->BlockType(start_block+i);
    Material *mat = cache->GetMaterial(type);
  }
private:
  ICache *cache;
  IWorld *world;
  int start_block;
  int end_block;
};
#endif
GameApi::CX GameApi::MainLoopApi::subworld(CX c, W w, int start_block, int end_block)
{
#if 0
  ICache *cc = find_cache(e,c);
  IWorld *ww = find_world(e,w);
  return add_cache(e, new SubWorldCache(cc,ww,start_block,end_block));
#endif
}

#if 0
class ArrayWorld : public IWorld
{
public:
  ArrayWorld(std::vector<IWorld*> w) : w(w) { }
  int NumBlocks() const
  {
    int s = w.size();
    int count = 0;
    for(int i=0;i<s;i++) count+=w[i]->NumBlocks();
    return count;
  }
  int BlockType(int i) const
  {
    int s = w.size();
    int count = 0;
    for(int ii=0;ii<s;ii++) {
      int c = w[ii]->NumBlocks();
      if (i>=count && i<count+c) {
	return w[ii]->BlockType(i-count);
      }
      count+=c;
    }
    return 0;
  }
  Point BlockPos(int i) const
  {
    int s = w.size();
    int count = 0;
    for(int ii=0;ii<s;ii++) {
      int c = w[ii]->NumBlocks();
      if (i>=count && i<count+c) {
	return w[ii]->BlockPos(i-count);
      }
      count+=c;
    }
    return Point(0.0,0.0,0.0);
  }
  Matrix BlockRot(int i) const
  {
    int s = w.size();
    int count = 0;
    for(int ii=0;ii<s;ii++) {
      int c = w[ii]->NumBlocks();
      if (i>=count && i<count+c) {
	return w[ii]->BlockRot(i-count);
      }
      count+=c;
    }
    return Matrix::Identity();
  }
private:
  std::vector<IWorld*> w;
};
#endif
GameApi::W GameApi::MainLoopApi::array_world(std::vector<W> vec)
{
#if 0
  std::vector<IWorld*> vec2;
  int s = vec.size();
  for(int i=0;i<s;i++)
    vec2.push_back(find_world(e,vec[i]));
  return add_world(e, new ArrayWorld(vec2));
#endif
}

class ArrayPlatform : public Platform
{
public:
  ArrayPlatform(std::vector<Platform*> vec) : vec(vec) { }
  bool Allow(float x, float z) const
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Platform *item = vec[i];
	bool b = item->Allow(x,z);
	if (b) return true;
      }
    return false;
  }
  float Height(float x, float z) const
  {
    int s = vec.size();
    float height = -1000000.0;
    for(int i=0;i<s;i++)
      {
	Platform *item = vec[i];
	bool b = item->Allow(x,z);
	if (b) {
	  float h = item->Height(x,z);
	  if (h>height) height=h;
	}
      }
    return height;
  }
  FaceCollection *Render() const
  {
    int s = vec.size();
    std::vector<FaceCollection*> vec2;
    for(int i=0;i<s;i++)
      {
	Platform *item = vec[i];
	FaceCollection *coll = item->Render();
	vec2.push_back(coll);
      }
    return new OrElem<FaceCollection>(vec2.begin(), vec2.end());
  }
private:
  std::vector<Platform*> vec;
};

class RectPlatform : public Platform
{
public:
  RectPlatform(float start_x, float end_x,
	       float start_y, float end_y,
	       float start_z, float end_z,
	       bool x_or_z,
	       bool start_or_end_higher) : m_start_x(start_x), m_end_x(end_x),
					   m_start_y(start_y), m_end_y(end_y),
					   m_start_z(start_z), m_end_z(end_z),
					   m_x_or_z(x_or_z),
					   m_start_or_end_higher(start_or_end_higher) {}
  virtual bool Allow(float x, float z) const
  {
    if (x>=m_start_x && x<m_end_x)
      if (z>=m_start_z && z<m_end_z)
	return true;
    return false;
  }
  virtual float Height(float x, float z) const
  {
    float delta_x = x-m_start_x;
    float delta_z = z-m_start_z;
    delta_x/=(m_end_x-m_start_x);
    delta_z/=(m_end_z-m_start_z);
    float height=0.0;
    if (m_x_or_z)
      { // z
	if (m_start_or_end_higher) { // end higher
	  height = (1.0-delta_z)*m_start_y + delta_z*m_end_y;
	}
	else
	  { // start higher
	  height = (1.0-delta_z)*m_end_y + delta_z*m_start_y;
	  }
      }
    else
      { // x
	if (m_start_or_end_higher) { // end higher
	  height = (1.0-delta_x)*m_start_y + delta_x*m_end_y;
	}
	else
	  { // start higher
	  height = (1.0-delta_x)*m_end_y + delta_x*m_start_y;
	  }
      }
    return height;
  }
  virtual FaceCollection *Render() const
  {
    Point pp1;
    Point pp2;
    Point pp3;
    Point pp4;
    pp1.x = m_start_x;
    pp1.z = m_start_z;
    pp2.x = m_end_x;
    pp2.z = m_start_z;
    pp3.x = m_end_x;
    pp3.z = m_end_z;
    pp4.x = m_start_x;
    pp4.z = m_end_z;

    if (m_x_or_z)
      {
	if (m_start_or_end_higher) {
	  pp1.y = m_start_y;
	  pp2.y = m_start_y;
	  pp3.y = m_end_y;
	  pp4.y = m_end_y;
	}
	else
	  {
	  pp1.y = m_end_y;
	  pp2.y = m_end_y;	    
	  pp3.y = m_start_y;
	  pp4.y = m_start_y;
	  }
      } else
      {
	if (m_start_or_end_higher) {
	  pp2.y = m_end_y;
	  pp1.y = m_start_y;
	  pp3.y = m_end_y;
	  pp4.y = m_start_y;
	}
	else
	  {
	  pp2.y = m_start_y;
	  pp1.y = m_end_y;
	  pp3.y = m_start_y;
	  pp4.y = m_end_y;
	  }
      }
    return new QuadElem(pp1,pp2,pp3,pp4);
  }
private:
  float m_start_x; float m_end_x;
  float m_start_y; float m_end_y;
  float m_start_z; float m_end_z;
  bool m_x_or_z;
  bool m_start_or_end_higher;
};

GameApi::PL GameApi::PolygonApi::array_pl(std::vector<PL> vec)
{
  int s = vec.size();
  std::vector<Platform*> vec2;
  for(int i=0;i<s;i++)
    {
      vec2.push_back(find_platform(e,vec[i]));
    }
  return add_platform(e, new ArrayPlatform(vec2));
}

GameApi::PL GameApi::PolygonApi::or_elem_pl(PL pl1, PL pl2)
{
  std::vector<PL> vec;
  vec.push_back(pl1);
  vec.push_back(pl2);
  return array_pl(vec);
}

GameApi::PL GameApi::PolygonApi::rect_pl(float start_x, float end_x,
				      float start_y, float end_y,
				      float start_z, float end_z,
				      bool x_or_z,
				      bool start_or_end_higher)
{
  return add_platform(e, new RectPlatform(start_x, end_x,
					  start_y, end_y,
					  start_z, end_z,
					  x_or_z,
					  start_or_end_higher));
}
				      
GameApi::P GameApi::PolygonApi::render_pl(PL pl)
{
  Platform *plat = find_platform(e,pl);
  FaceCollection *coll = plat->Render();
  return add_polygon2(e,coll,1);
}

template<class T>
Bitmap<T> *subbitmap_t(Bitmap<T> *bm, int x, int y, int width, int height);



// Here's needed functions for GI

#if 0

bool ray_obj_intersection(GameApi::P p, Point pos, Vector dir)
{
}

void GenerateSemiSphere(Vector dir, std::vector<Vector> &dir, int num)
{
}

GameApi::GI GameApi::PolygonApi::generate_gi(int num, P p)
{
}

GameApi::GA GameApi::PolygonApi::update_gi_percentage(GI gi, P scene)
{
}

void GameApi::PolygonApi::pass_to_shader(GI gi)
{
}
#endif

class ColourSpaceToI : public ColourSpaceI
{
public:
  ColourSpaceToI(ColourSpace &val, int sx, int sy, int sz) : val(val),sx(sx),sy(sy),sz(sz) { }
  virtual unsigned int Map(int x, int y, int z, float t) const
  {
    float xx = val.start_x()+(val.end_x()-val.start_x())*float(x)/float(sx);
    float yy = val.start_y()+(val.end_y()-val.start_y())*float(y)/float(sy);
    float zz = val.start_z()+(val.end_z()-val.start_z())*float(z)/float(sz);
    return val.Map(xx,yy,zz,t);
  }
  virtual int size_x() const { return sx; }
  virtual int size_y() const { return sy; }
  virtual int size_z() const { return sz; }
  virtual float start_t() const { return val.start_t(); }
  virtual float end_t() const { return val.end_t(); }
private:
  ColourSpace &val;
  int sx,sy,sz;
};

class ColorSpaceFaceCollection2 : public FaceCollection
{
public:
  ColorSpaceFaceCollection2(ColourSpaceI &colours, Point center, Vector u_x, Vector u_y, Vector u_z, float current_t) : colours(colours), center(center), u_x(u_x), u_y(u_y), u_z(u_z), current_t(current_t) { }

  struct FaceId {
    int dir;
    int x;
    int y;
    int z;
  };
  virtual std::string name() const { return "ColorSpaceFaceCollection2"; }

  FaceId split_face(int face) const
  {
    FaceId id;
    
    int sdir = 3;
    int sx = colours.size_x()-1;
    int sy = colours.size_y()-1;
    int sz = colours.size_z()-1;

    id.dir = face / (sx*sy*sz);
    int delta0 = face - id.dir*sx*sy*sz;

    int x = delta0 / (sy*sz);
    int delta1 = delta0 - x*sy*sz;

    int y = delta1 / sz;
    int delta2 = delta1 - y*sz;

    int z = delta2;
    
    
    id.x = x;
    id.y = y;
    id.z = z;
    return id;
  }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual int NumFaces() const
  {
    return 3*(colours.size_x()-1)*(colours.size_y()-1)*(colours.size_z()-1);
  }
  
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    FaceId id = split_face(face);
    Point plot = id.x*u_x + id.y*u_y + id.z*u_z;
    if (id.dir==0)
      {
	plot += u_z/2;
	plot -= u_x/2;
	if (point==0) return plot;
	plot+=u_x;
	if (point==1) return plot;
	plot+=u_y;
	if (point==2) return plot;
	plot-=u_x;
	if (point==3) return plot;
      }
    if (id.dir==1)
      {
	plot +=u_z/2;
	plot +=u_x/2;
	if (point==0) return plot;
	plot+=u_y;
	if (point==1) return plot;
	plot+=u_z;
	if (point==2) return plot;
	plot-=u_y;
	if (point==3) return plot;
      }
    if (id.dir==2)
      {
	plot -=u_z/2;
	plot -=u_y/2;
	if (point==0) return plot;
	plot+=u_y;
	if (point==1) return plot;
	plot-=u_x;
	if (point==2) return plot;
	plot-=u_y;
	if (point==3) return plot;
      }
  }
  virtual Vector PointNormal(int face, int point) const
  {
    FaceId id = split_face(face);
    if (id.dir==0) return Vector(0.0,0.0,-1.0);
    if (id.dir==1) return Vector(0.0,-1.0,0.0);
    if (id.dir==2) return Vector(-1.0,0.0,0.0);
    return Vector(0.0,0.0,1.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    FaceId id = split_face(face);
    int delta_x = 0;
    int delta_y = 0;
    if (point==1||point==2) delta_x=1;
    if (point==2||point==3) delta_y=1;
    if (id.dir==0) {
      return colours.Map(id.x+delta_x,id.y+delta_y,id.z,current_t);
    } else if (id.dir==1)
      {
      return colours.Map(id.x+delta_x,id.y,id.z+delta_y,current_t);
      } else if (id.dir==2)
      {
      return colours.Map(id.x,id.y+delta_x,id.z+delta_y,current_t);
      }
    return 0xffff00ff;
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p; return p; }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
private:
  ColourSpaceI &colours;
  Point center;
  Vector u_x, u_y, u_z;
  float current_t;
};


class ColorSpaceFaceCollection : public FaceCollection
{
public:
  ColorSpaceFaceCollection(ColourSpaceI &colours, Point center, Vector u_x, Vector u_y, Vector u_z, float current_t) : colours(colours),center(center), u_x(u_x),u_y(u_y),u_z(u_z),current_t(current_t) { }
  Point pos(int x, int y, int z) const { return center+(x-colours.size_x()/2)*u_x+(y-colours.size_y()/2)*u_y+(z-colours.size_z()/2)*u_z; }
  bool is_face_x(int x, int y, int z) const
  {
    unsigned int c0 = colours.Map(x,y,z,current_t);
    unsigned int c1 = colours.Map(x+1,y,z,current_t);
    //if (c0!=c1) std::cout << "YUP" << std::endl;
    return c0!=c1;
  }
  bool is_face_y(int x, int y, int z) const
  {
    unsigned int c0 = colours.Map(x,y,z,current_t);
    unsigned int c1 = colours.Map(x,y+1,z,current_t);
    //if (c0!=c1) std::cout << "YUP" << std::endl;
    return c0!=c1;
  }
  bool is_face_z(int x, int y, int z) const
  {
    unsigned int c0 = colours.Map(x,y,z,current_t);
    unsigned int c1 = colours.Map(x,y,z+1,current_t);
    //if (c0!=c1) std::cout << "YUP" << std::endl;
    return c0!=c1;
  }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual int NumFaces() const
  {
    return 3*(colours.size_x()-1)*(colours.size_y()-1)*(colours.size_z()-1);
  }
  struct FaceId {
    int dir;
    int x;
    int y;
    int z;
  };
  FaceId split_face(int face) const
  {
    FaceId id;
    
    int sdir = 3;
    int sx = colours.size_x()-1;
    int sy = colours.size_y()-1;
    int sz = colours.size_z()-1;

    id.dir = face / (sx*sy*sz);
    int delta0 = face - id.dir*sx*sy*sz;

    int x = delta0 / (sy*sz);
    int delta1 = delta0 - x*sy*sz;

    int y = delta1 / sz;
    int delta2 = delta1 - y*sz;

    int z = delta2;
    
    
    id.x = x;
    id.y = y;
    id.z = z;
    return id;
  }
  virtual int NumPoints(int face) const { return 4; }
  virtual Point FacePoint(int face, int point) const
  {
    FaceId id = split_face(face);
    //if (face % 2==0) return pos(id.x,id.y,id.z);
    int delta_x = 0;
    int delta_y = 0;
    if (point==1||point==2) delta_x=1;
    if (point==2||point==3) delta_y=1;
    if (id.dir==0) {
      if (!is_face_x(id.x,id.y,id.z)&& !is_face_y(id.x,id.y,id.z)) return pos(id.x,id.y,id.z);
      Point p = pos(id.x+delta_x,id.y+delta_y,id.z);
      return p;
    } else if (id.dir==1)
      {
	if (!is_face_x(id.x,id.y,id.z)&& !is_face_z(id.x,id.y,id.z)) return pos(id.x,id.y,id.z);
	Point p= pos(id.x+delta_x,id.y,id.z+delta_y);
	return p;
      } else if (id.dir==2)
      {
	if (!is_face_y(id.x,id.y,id.z)&& !is_face_z(id.x,id.y,id.z)) return pos(id.x,id.y,id.z);
	Point p = pos(id.x,id.y+delta_x,id.z+delta_y);
	return p;
      }
    Point p = pos(id.x,id.y,id.z);
    return p;
  }
  virtual Vector PointNormal(int face, int point) const
  {
    FaceId id = split_face(face);
    if (id.dir==0) return Vector(0.0,0.0,-1.0);
    if (id.dir==1) return Vector(0.0,-1.0,0.0);
    if (id.dir==2) return Vector(-1.0,0.0,0.0);
    return Vector(0.0,0.0,1.0);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    FaceId id = split_face(face);
    int delta_x = 0;
    int delta_y = 0;
    if (point==1||point==2) delta_x=1;
    if (point==2||point==3) delta_y=1;
    if (id.dir==0) {
      return colours.Map(id.x+delta_x,id.y+delta_y,id.z,current_t);
    } else if (id.dir==1)
      {
      return colours.Map(id.x+delta_x,id.y,id.z+delta_y,current_t);
      } else if (id.dir==2)
      {
      return colours.Map(id.x,id.y+delta_x,id.z+delta_y,current_t);
      }
    return 0xffff00ff;
  }
  virtual Point2d TexCoord(int face, int point) const { Point2d p; return p; }
  virtual float TexCoord3(int face, int point) const { return 0.0; }
  virtual VEC4 Joints(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
  virtual VEC4 Weights(int face, int point) const { VEC4 v; v.x = 0.0; v.y = 0.0; v.z = 0.0; v.w = 0.0; return v; }
private:
  ColourSpaceI &colours;
  Point center;
  Vector u_x;
  Vector u_y;
  Vector u_z;
  float current_t;
};


class ColourSpace_Function : public ColourSpace
{
public:
  ColourSpace_Function(std::function<unsigned int(float,float,float,float)> f, float s_x, float e_x, float s_y, float e_y, float s_z, float e_z, float s_t, float e_t) : f(f), s_x(s_x), e_x(e_x), s_y(s_y), e_y(e_y), s_z(s_z), e_z(e_z), s_t(s_t), e_t(e_t) { }
  virtual unsigned int Map(float x, float y, float z, float t) const
  {
    return f(x,y,z,y);
  }
  virtual float start_x() const { return s_x; }
  virtual float end_x() const { return e_x; }
  virtual float start_y() const { return s_y; }
  virtual float end_y() const { return e_y; }
  virtual float start_z() const { return s_z; }
  virtual float end_z() const { return e_z; }
  virtual float start_t() const { return s_t; }
  virtual float end_t() const { return e_t; }
private:
  std::function<unsigned int(float,float,float,float)> f;
  float s_x, e_x;
  float s_y, e_y;
  float s_z, e_z;
  float s_t, e_t;
};

unsigned int colourspace_sphere(float x, float y, float z, float t)
{
  float dist = x*x+y*y+z*z;
  unsigned int i =  x*x+y*y+z*z<t*t ? 0xffffffff : 0xff008800;
  //std::cout << x << " " << y << " " << z << " " << t << " " << std::hex << i << std::endl;
  i+=dist*300;
  return i;
}

GameApi::CS GameApi::PolygonApi::colourspace_sphere2()
{
  return colourspace_func(&colourspace_sphere,-1.0,1.0, -1.0, 1.0, -1.0, 1.0, 0.0, 600.0);
}

class ColourSpace_OrElem : public ColourSpace
{
public:
  ColourSpace_OrElem(ColourSpace &cs1, float delta_t, ColourSpace &cs2, float delta_t2) : cs1(cs1), cs2(cs2),delta_t(delta_t), delta_t2(delta_t2) { }
  virtual unsigned int Map(float x, float y, float z, float t) const
  {
    unsigned int c1 = cs1.Map(x,y,z,t);
    unsigned int c2 = cs2.Map(x,y,z,t);
    unsigned int alpha1 = (c1 &0xff000000)>>24;
    unsigned int alpha2 = (c2 &0xff000000)>>24;
    float alpha1f = alpha1/255.0;
    float alpha2f = alpha2/255.0;
    float sum = alpha1f+alpha2f;
    float delta1 = alpha1f/sum;
    float delta2 = alpha2f/sum;
    Color cc1(c1);
    Color cc2(c2);
    cc1*=delta1;
    cc2*=delta2;
    return (cc1+cc2).Pixel();
  }
  virtual float start_x() const { return std::max(cs1.start_x(),cs2.start_x()); }
  virtual float end_x() const { return std::min(cs1.end_x(),cs2.end_x()); }
  virtual float start_y() const { return std::max(cs1.start_y(),cs2.start_y()); }
  virtual float end_y() const { return std::min(cs1.end_y(),cs2.end_y()); }
  virtual float start_z() const { return std::max(cs1.start_z(),cs2.start_z()); }
  virtual float end_z() const { return std::min(cs1.end_z(),cs2.end_z()); }
  virtual float start_t() const { return std::max(delta_t+cs1.start_t(),delta_t2+cs2.start_t()); }
  virtual float end_t() const { return std::min(delta_t+cs1.end_t(),delta_t2+cs2.end_t()); }
private:
  ColourSpace &cs1;
  ColourSpace &cs2;
  float delta_t;
  float delta_t2;
};

GameApi::CS GameApi::PolygonApi::colourspace_or_elem(CS cs, float delta_t, CS cs2, float delta_t2)
{
  ColourSpace *cs1 = find_colourspace(e,cs);
  ColourSpace *cs2_ = find_colourspace(e,cs2);
  return add_colourspace(e,new ColourSpace_OrElem(*cs1,delta_t,*cs2_,delta_t2));
}

GameApi::CS GameApi::PolygonApi::colourspace_func(std::function<unsigned int (float,float,float,float)> f, float s_x, float e_x, float s_y, float e_y, float s_z, float e_z, float s_t, float e_t)
{
  return add_colourspace(e,new ColourSpace_Function(f,s_x,e_x,s_y,e_y,s_z,e_z,s_t,e_t));
}

GameApi::CSI GameApi::PolygonApi::colourspace_sample(CS i, int sx, int sy, int sz)
{
  ColourSpace *col = find_colourspace(e,i);
  return add_colourspaceI(e, new ColourSpaceToI(*col,sx,sy,sz));
}

GameApi::P GameApi::PolygonApi::colourspace_facecoll(CSI i, float t)
{
  ColourSpaceI *col = find_colourspaceI(e,i);
  return add_polygon2(e, new ColorSpaceFaceCollection2(*col,Point(0.0,0.0,0.0), Vector(3.5*4,0.0,0.0), Vector(0.0,3.5*4,0.0), Vector(0.0,0.0,3.5*4),t),1);
}

class FaceCollectionTo2 : public FaceCollection2
{
public:
  enum Fields {
    EFacePoint=0x1,
    EEndFacePoint=0x2,
    EPointNormal=0x4,
    EColor=0x8,
    ETexCoord=0x10,
    ETexCoord3=0x20,
    EJoint=0x40,
    EWeight=0x80
  };
  FaceCollectionTo2(FaceCollection *coll, Fields flags=Fields(EFacePoint+EEndFacePoint+EPointNormal+EColor+ETexCoord+ETexCoord3+EJoint+EWeight)) : coll(coll),flags(flags) { }
  virtual void Collect(CollectVisitor &vis) { return coll->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { coll->Prepare(); }
  virtual int NumFaces() const { return coll->NumFaces(); }
  virtual int NumPoints(int face) const { return coll->NumPoints(face); }
  virtual Vertex Face(int face, int point) const
  {
    Vertex v;
    if (flags&EFacePoint)
      v.facepoint = coll->FacePoint(face,point);
    if (flags&EEndFacePoint)
      v.end_facepoint = coll->EndFacePoint(face,point);
    if (flags&EPointNormal)
      v.pointnormal = coll->PointNormal(face,point);
    if (flags&EColor)
      v.color = coll->Color(face,point);
    if (flags&ETexCoord)
      v.texcoord = coll->TexCoord(face,point);
    if (flags&ETexCoord3)
      v.texcoord3 = coll->TexCoord3(face,point);
    if (flags&EJoint)
      v.joint = coll->Joints(face,point);
    if (flags&EWeight)
      v.weight = coll->Weights(face,point);
    return v;
  }

private:
  FaceCollection *coll;
  Fields flags;
};
/*
GameApi::P2 GameApi::PolygonApi::facecoll_to_2(GameApi::P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new FaceCollectionTo2(coll),1);
}
*/

class NoBatchMap : public ForwardFaceCollection
{
public:
  NoBatchMap(FaceCollection *coll) : ForwardFaceCollection(*coll) { }
  virtual std::string name() const { return "NoBatchMap"; }

  bool HasBatchMap() const { std::cout << "NoBatchMap: " << name() << std::endl; return false; }
};
GameApi::P GameApi::PolygonApi::no_batch_map(P p)
{
  FaceCollection *coll = find_facecoll(e,p);
  return add_polygon2(e, new NoBatchMap(coll),1);
}

GameApi::ML GameApi::PolygonApi::fade_pic(GameApi::EveryApi &ev, BM bm1, float start_time, float transition_time, float end_time, float end_transition_time,
					  BM bm2, float start2_time, float transition2_time, float end2_time, float end2_transition_time)
{
  P I1=ev.polygon_api.quad_z(-600,600,-600,600,0);
  MT I2=ev.materials_api.texture(ev,bm1,1.0);
  MT I3=ev.materials_api.fade(ev,I2,start_time,start_time+transition_time,end_time,end_time+end_transition_time);
  ML I4=ev.materials_api.bind(I1,I3);
  MT I5=ev.materials_api.texture(ev,bm2,1.0);
  MT I6=ev.materials_api.fade(ev,I5,start2_time,start2_time+transition2_time,end2_time,end2_time+end2_transition_time);
  ML I7=ev.materials_api.bind(I1,I6);
  ML I8=ev.mainloop_api.or_elem_ml(ev,I4,I7);
  return I8;
}

#ifdef USE_VIDEO
#if (ALL==1)||(VIDEOFILE_TXID_GENERIC==1)

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

void *writer(void*);

class VideoSource : public TextureID, public CollectInterface
{
public:
  VideoSource(GameApi::Env &e, std::string filename, int sx, int sy) : e(e), sx(sx),sy(sy), filename(filename) {
    ref=BufferRef::NewBuffer(sx,sy);    
    Random r;
    float val = double(r.next())/r.maximum();
    float val2 = val*10000.0;
    int val3 = (int)val2;

    std::stringstream ss;
    ss << val3;
    id = ss.str();
    
    //std::cout << "WARNING: Video support doesn't work in emscripten" << std::endl;
  }
  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    tasks_add(111,&writer,(void*)this);
    tasks_join(111);
    OpenglLowApi *ogl = g_low->ogl;
    ogl->glGenTextures(1,&tex);
  }
  virtual void Prepare() { HeavyPrepare(); }
  virtual void handle_event(MainLoopEvent &e) { }
  void Prepare2()
  {
    if (firsttime) {
      firsttime=false;
#ifndef EMSCRIPTEN
    e.async_load_url(filename, gameapi_homepageurl);
#endif
    GameApi::ASyncVec *ptr = e.get_loaded_async_url(filename);
    if (!ptr) { std::cout << "ASYNC NOT READY in VIDEO" << std::endl; return; }

    std::string home = getenv("HOME")?getenv("HOME"):".";

  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?"/root":home;

  std::string path = dd?dockerdir+"/.gameapi_builder/":home+"/.gameapi_builder/";
#ifdef EMSCRIPTEN
    home = ".";
    path = "/";
#endif
    std::ofstream ss((path + "video"+id+".mp4")
		     .c_str(),std::ios_base::out|std::ios_base::binary);
    std::string ss2(ptr->begin(),ptr->end());
    ss << ss2;
    ss.close();

    cap = cv::VideoCapture(path + "video"+id+".mp4");
    }
  }
  
  virtual void render(MainLoopEnv &e)
  {
    OpenglLowApi *ogl = g_low->ogl;
    cv::Mat frame;
    if (cap.grab())
      {
	cap.retrieve(frame);
      }
    else
      {
   std::string home = getenv("HOME");

  const char *dd = getenv("BUILDER_DOCKER_DIR");
  std::string dockerdir = dd?dd:home;

   
 	cap = cv::VideoCapture(dockerdir+"/.gameapi_builder/video"+id+".mp4");
	
	cap.grab();
	cap.retrieve(frame);
      }

    int channels = frame.channels();
    int nRows = frame.rows;
    int nCols = frame.cols;

    if (nRows==0) return;
    if (nCols==0) return;


    if (nRows!=sy||nCols!=sx)
      {
	BufferRef::FreeBuffer(ref);
	ref=BufferRef::NewBuffer(nCols,nRows);
	sx=nCols;
	sy=nRows;
      }
    
    //std::cout << "Channels: " << channels << std::endl;
    
    
    //if (frame.isContinuous())
    // {
    //	nCols *= nRows;
    //	nRows = 1;
    //}

    //std::cout << "sx=" << sx << " sy=" << sy << std::endl;
    //std::cout << "r=" << nRows << " c=" << nCols << std::endl;
    
    int delta_y = sy-nRows;
    int delta_x = sx-nCols;

    if (delta_y<0) delta_y=0;
    if (delta_x<0) delta_x=0;
    
    delta_x/=2;
    delta_y/=2;

    //std::cout << "delta_x=" << delta_x << " delta_y=" << delta_y<< std::endl;

    std::memset(ref.buffer, 0, sx*sy*sizeof(unsigned int));

    
    unsigned char *ptr = frame.data;
    for(int j=0;j<std::min(sy,nRows);j++) {
      unsigned char *rowP = frame.ptr<unsigned char>(j);
      for(int i=0;i<std::min(sx,nCols);i++)
	{
	  ref.buffer[delta_x+i+(j+delta_y)*ref.ydelta]=Color(rowP[i*channels+0],rowP[i*channels+1],rowP[i*channels+2],255).Pixel();
	}
    }
    for(int y=0;y<sy/2;y++)
      for(int x=0;x<sx;x++)
	{
	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+x+(ref.height-y-1)*ref.ydelta));
	}
    //for(int y=0;y<sy;y++)
    //  for(int x=0;x<sx/2;x++)
    //	{
    //	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+(ref.width-x-1)+y*ref.ydelta));
    //	}

    
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
    ogl->glBindTexture(Low_GL_TEXTURE_2D, tex);
    //std::cout << frame.cols << "x" << frame.rows << std::endl;
    ogl->glTexImage2D(Low_GL_TEXTURE_2D, 0, Low_GL_RGBA, ref.width, ref.height, 0, Low_GL_RGBA, Low_GL_UNSIGNED_BYTE, ref.buffer);
    ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MIN_FILTER,Low_GL_NEAREST);      
    ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_MAG_FILTER,Low_GL_NEAREST);
    ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_S, Low_GL_CLAMP_TO_EDGE);
    ogl->glTexParameteri(Low_GL_TEXTURE_2D,Low_GL_TEXTURE_WRAP_T, Low_GL_CLAMP_TO_EDGE);

    ogl->glBindTexture(Low_GL_TEXTURE_2D,0);
  }
  virtual int texture() const
      {
	OpenglLowApi *ogl = g_low->ogl;
#ifndef EMSCRIPTEN
  ogl->glClientActiveTexture(Low_GL_TEXTURE0+0);
#endif
  ogl->glActiveTexture(Low_GL_TEXTURE0+0);
	ogl->glBindTexture(Low_GL_TEXTURE_2D,tex);
	return tex;
      }
  virtual bool is_fbo() const { return false; }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }
private:
  GameApi::Env &e;
  int sx,sy;
  std::string filename;
  cv::VideoCapture cap;
  unsigned int tex;
  BufferRef ref;
  bool firsttime = true;
  std::string id;
};
void *writer(void* ptr)
{
  VideoSource *src = (VideoSource*)ptr;
  src->Prepare2();
  return 0;
}
#endif

IMPORT bool file_exists(std::string s);


#if (ALL==1)||(VIDEOFILE_TXID_GENERIC==1)

bool exists(const cv::String &path)
{
  std::string s = path;
  return file_exists(s);
}
#endif


GameApi::TXID GameApi::BitmapApi::video_source(std::string filename, int sx, int sy)
{
#if (ALL==1)||(VIDEOFILE_TXID_GENERIC==1)
  return add_txid(e,new VideoSource(e,filename,sx,sy));
#endif
}
#endif

class MutableFaceCollImpl : public MutableFaceCollection
{
public:
  MutableFaceCollImpl(FaceCollection &coll) : coll(coll) { }

  virtual std::string name() const { return "mutablefacecollimpl"; }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }

  
  virtual int NumVertices() const { return coll.NumFaces()*3; }
  virtual void Faces(std::vector<Point> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.FacePoint(i/3,i%3);
  }
  virtual void Normal(std::vector<Vector> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.PointNormal(i/3,i%3);
  }
  virtual void Color(std::vector<unsigned int> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.Color(i/3,i%3);
  }
  virtual void Tex(std::vector<Point2d> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.TexCoord(i/3,i%3);
  }
  virtual void Tex3(std::vector<float> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.TexCoord3(i/3,i%3);
  }
  virtual void Joints(std::vector<VEC4> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.Joints(i/3,i%3);
  }
  virtual void Weights(std::vector<VEC4> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      vec[i] = coll.Weights(i/3,i%3);
  }


  virtual int NumFaces() const { return coll.NumFaces()*3; }
  virtual void Indices(std::vector<int> &vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	vec[i] = i;
      }
  }
private:
  FaceCollection &coll;
};

class MutableImpl : public FaceCollection
{
public:
  MutableImpl(MutableFaceCollection &mut) : mut(mut) { }

  virtual std::string name() const { return "mutableimpl"; }
  virtual void Collect(CollectVisitor &vis) {
    vis.register_obj(this);
  }
  virtual void HeavyPrepare() {
    
    int vert = mut.NumVertices();
    int numfaces = mut.NumFaces();
    
    faces.resize(vert);
    normals.resize(vert);
    colors.resize(vert);
    textures.resize(vert);
    tex3.resize(vert);
    joints.resize(vert);
    weights.resize(vert);
    indices.resize(numfaces*3);
    
    mut.Faces(faces);
    mut.Normal(normals);
    mut.Color(colors);
    mut.Tex(textures);
    mut.Tex3(tex3);
    mut.Joints(joints);
    mut.Weights(weights);
    mut.Indices(indices);
    
  }
  virtual void Prepare() { HeavyPrepare(); }

  virtual Point FacePoint(int face, int point) const
  {
    return faces[indices[face*3+point]];
  }
  virtual Vector PointNormal(int face, int point) const
  {
    return normals[indices[face*3+point]];
  }
  virtual float Attrib(int face, int point, int id) const
  {
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    return colors[indices[face*3+point]];

  }
  virtual Point2d TexCoord(int face, int point) const
  {
    return textures[indices[face*3+point]];
  }
  virtual float TexCoord3(int face, int point) const {

    return tex3[indices[face*3+point]];

  }
  virtual VEC4 Joints(int face, int point) const {
    return joints[indices[face*3+point]];
  }
  virtual VEC4 Weights(int face, int point) const {
    return weights[indices[face*3+point]];
  }
private:
  MutableFaceCollection &mut;
  std::vector<Point> faces;
  std::vector<Vector> normals;
  std::vector<unsigned int> colors;
  std::vector<Point2d> textures;
  std::vector<float> tex3;
  std::vector<VEC4> joints;
  std::vector<VEC4> weights;

  std::vector<int> indices;
};

namespace Decimate
{

struct Vec3 {
    float x, y, z;

    float distance(const Vec3& other) const {
        return std::sqrt((x - other.x)*(x - other.x)
                       + (y - other.y)*(y - other.y)
                       + (z - other.z)*(z - other.z));
    }

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator/(float d) const { return {x / d, y / d, z / d}; }
};

struct Triangle {
    int v0, v1, v2;
};

struct Edge {
    int v1, v2;

    bool operator<(const Edge& other) const {
        return std::tie(v1, v2) < std::tie(other.v1, other.v2);
    }
};

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Triangle> triangles;
};

std::set<Edge> collectEdges(const std::vector<Triangle>& tris) {
    std::set<Edge> edges;
    for (auto& t : tris) {
        int v[3] = {t.v0, t.v1, t.v2};
        for (int i = 0; i < 3; ++i) {
            int a = std::min(v[i], v[(i+1)%3]);
            int b = std::max(v[i], v[(i+1)%3]);
            edges.insert({a, b});
        }
    }
    return edges;
}

void collapseEdge(Mesh& mesh, int v1, int v2) {
    // Collapse v2 into v1
    Vec3 newPos = (mesh.vertices[v1] + mesh.vertices[v2]) / 2.0f;
    mesh.vertices[v1] = newPos;

    for (auto& tri : mesh.triangles) {
        if (tri.v0 == v2) tri.v0 = v1;
        if (tri.v1 == v2) tri.v1 = v1;
        if (tri.v2 == v2) tri.v2 = v1;
    }

    // Remove degenerate triangles
    std::vector<Triangle> cleaned;
    for (auto& tri : mesh.triangles) {
        if (tri.v0 != tri.v1 && tri.v1 != tri.v2 && tri.v2 != tri.v0)
            cleaned.push_back(tri);
    }
    mesh.triangles = cleaned;
}

void decimate(Mesh& mesh, float target_ratio) {
    int target_faces = static_cast<int>(mesh.triangles.size() * target_ratio);

    while (mesh.triangles.size() > target_faces) {
        auto edges = collectEdges(mesh.triangles);
        float min_cost = std::numeric_limits<float>::max();
        Edge best_edge;

        for (auto& e : edges) {
            float cost = mesh.vertices[e.v1].distance(mesh.vertices[e.v2]);
            if (cost < min_cost) {
                min_cost = cost;
                best_edge = e;
            }
        }

        collapseEdge(mesh, best_edge.v1, best_edge.v2);
    }
}

}

Decimate::Mesh decimate_mesh_from_facecollection(FaceCollection *coll)
{
  Decimate::Mesh res;
  coll->Prepare();
  int s = coll->NumFaces();
  int vertex = 0;
  for(int i=0;i<s;i++)
    {
      Point p0 = coll->FacePoint(i,0);
      Point p1 = coll->FacePoint(i,1);
      Point p2 = coll->FacePoint(i,2);
      Decimate::Vec3 v0 = { p0.x,p0.y,p0.z };
      Decimate::Vec3 v1 = { p1.x,p1.y,p1.z };
      Decimate::Vec3 v2 = { p2.x,p2.y,p2.z };
      res.vertices.push_back(v0);
      res.vertices.push_back(v1);
      res.vertices.push_back(v2);
      Decimate::Triangle t;
      t.v0 = vertex;
      t.v1 = vertex+1;
      t.v2 = vertex+2;
      res.triangles.push_back(t);
      vertex+=3;
    }
  return res;
}


class DecimateFaceCollection : public FaceCollection
{
public:
  DecimateFaceCollection(FaceCollection *orig, Decimate::Mesh &mesh) : mesh(mesh) { }

  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  std::string name() const { return "DecimateFaceCollection"; }
  void Prepare() { }
  
  virtual int NumFaces() const { return mesh.triangles.size(); }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    Decimate::Vec3 pp = mesh.vertices[p];
    return Point(pp.x,pp.y,pp.z);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->PointNormal(face2,point2);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Color(face2,point2);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->TexCoord(face2,point2);
  }
  virtual float TexCoord3(int face, int point) const {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->TexCoord3(face2,point2);

  }
  virtual VEC4 Joints(int face, int point) const {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Joints(face2,point2);
  }
  virtual VEC4 Weights(int face, int point) const {
    Decimate::Triangle t = mesh.triangles[face];
    int p;
    switch(point)
      {
      case 0: p=t.v0; break;
      case 1: p=t.v1; break;
      case 2: p=t.v2; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Weights(face2,point2);
  }
private:
  FaceCollection *orig;
  Decimate::Mesh &mesh;
};

GameApi::P GameApi::PolygonApi::decimate(GameApi::P p, float val)
{
  FaceCollection *coll = find_facecoll(e,p);
  Decimate::Mesh m = decimate_mesh_from_facecollection(coll);
  Decimate::decimate(m, val);
  return add_polygon2(e,new DecimateFaceCollection(coll,m),1);
}


/*
int main() {
  Decimate::Mesh mesh;

    // Load or create your mesh here (example: a simple quad split into 2 triangles)
    mesh.vertices = {
        {0, 0, 0}, {1, 0, 0},
        {1, 1, 0}, {0, 1, 0}
    };
    mesh.triangles = {
        {0, 1, 2}, {0, 2, 3}
    };

    decimate(mesh, 0.5f); // reduce to 50% triangle count

    std::cout << "Reduced to " << mesh.triangles.size() << " triangles.\n";
}
*/


namespace Decimate2 {

struct Vec3 {
    double x, y, z;
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(double s) const { return {x / s, y / s, z / s}; }
    double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
};

Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

struct SymmetricMatrix {
    double m[10];
    SymmetricMatrix(double c = 0) { for (int i = 0; i < 10; ++i) m[i] = c; }
    SymmetricMatrix(double a, double b, double c, double d) {
        m[0]=a*a; m[1]=a*b; m[2]=a*c; m[3]=a*d;
        m[4]=b*b; m[5]=b*c; m[6]=b*d;
        m[7]=c*c; m[8]=c*d;
        m[9]=d*d;
    }
    double operator[](int i) const { return m[i]; }
    SymmetricMatrix operator+(const SymmetricMatrix& n) const {
        SymmetricMatrix r;
        for (int i = 0; i < 10; ++i) r.m[i] = m[i] + n.m[i];
        return r;
    }
    double vertexError(double x, double y, double z) const {
        return m[0]*x*x + 2*m[1]*x*y + 2*m[2]*x*z + 2*m[3]*x +
               m[4]*y*y + 2*m[5]*y*z + 2*m[6]*y +
               m[7]*z*z + 2*m[8]*z + m[9];
    }
};

struct Vertex {
    Vec3 p;
    SymmetricMatrix q;
    bool deleted = false;
};

struct Triangle {
    int v[3];
    bool deleted = false;
};

struct Edge {
    int v1, v2;
    double cost;
    Vec3 pos;
    bool operator>(const Edge& e) const { return cost > e.cost; }
};
struct Mesh
{
  std::vector<Vertex> vertices;
  std::vector<Triangle> faces;
};

  
bool readOBJ(const std::string& filename, std::vector<Vertex>& verts, std::vector<Triangle>& tris) {
    std::ifstream in(filename);
    if (!in) return false;

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            verts.push_back({v});
        } else if (tag == "f") {
            Triangle t;
            std::string f;
            for (int i = 0; i < 3; ++i) {
                ss >> f;
                t.v[i] = std::stoi(f) - 1;
            }
            tris.push_back(t);
        }
    }
    return true;
}

void writeOBJ(const std::string& filename, const std::vector<Vertex>& verts, const std::vector<Triangle>& tris) {
    std::ofstream out(filename);
    std::vector<int> remap(verts.size(), -1);
    int count = 1;

    for (size_t i = 0; i < verts.size(); ++i) {
        if (!verts[i].deleted) {
            out << "v " << verts[i].p.x << " " << verts[i].p.y << " " << verts[i].p.z << "\n";
            remap[i] = count++;
        }
    }

    for (const auto& t : tris) {
        if (!t.deleted)
            out << "f " << remap[t.v[0]] << " " << remap[t.v[1]] << " " << remap[t.v[2]] << "\n";
    }
}

SymmetricMatrix planeQuadric(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
    Vec3 n = cross(v1 - v0, v2 - v0);
    double len = std::sqrt(n.dot(n));
    if (len == 0) return SymmetricMatrix();
    n = n / len;
    double d = -n.dot(v0);
    return SymmetricMatrix(n.x, n.y, n.z, d);
}

void computeVertexQuadrics(std::vector<Vertex>& verts, const std::vector<Triangle>& tris) {
    for (const auto& t : tris) {
        SymmetricMatrix q = planeQuadric(verts[t.v[0]].p, verts[t.v[1]].p, verts[t.v[2]].p);
        for (int i = 0; i < 3; ++i)
            verts[t.v[i]].q = verts[t.v[i]].q + q;
    }
}

Vec3 midpoint(const Vec3& a, const Vec3& b) {
    return (a + b) / 2.0;
}

double computeEdgeCost(const Vertex& v1, const Vertex& v2, Vec3& opt) {
    SymmetricMatrix q = v1.q + v2.q;
    opt = midpoint(v1.p, v2.p);
    return q.vertexError(opt.x, opt.y, opt.z);
}

void decimate(std::vector<Vertex>& verts, std::vector<Triangle>& tris, float targetRatio) {
    computeVertexQuadrics(verts, tris);

    auto edgeSet = std::set<std::pair<int, int>>();
    for (const auto& t : tris) {
        for (int i = 0; i < 3; ++i) {
            int a = std::min(t.v[i], t.v[(i+1)%3]);
            int b = std::max(t.v[i], t.v[(i+1)%3]);
            edgeSet.insert({a, b});
        }
    }

    std::priority_queue<Edge, std::vector<Edge>, std::greater<>> queue;
    for (auto [a, b] : edgeSet) {
        Vec3 pos;
        double cost = computeEdgeCost(verts[a], verts[b], pos);
        queue.push({a, b, cost, pos});
    }

    int target = tris.size() * targetRatio;

    while (tris.size() > target && !queue.empty()) {
        Edge e = queue.top(); queue.pop();
        if (verts[e.v1].deleted || verts[e.v2].deleted) continue;

        verts[e.v1].p = e.pos;
        verts[e.v1].q = verts[e.v1].q + verts[e.v2].q;
        verts[e.v2].deleted = true;

        for (auto& t : tris) {
            if (t.deleted) continue;
            for (int i = 0; i < 3; ++i)
                if (t.v[i] == e.v2) t.v[i] = e.v1;

            if (t.v[0] == t.v[1] || t.v[1] == t.v[2] || t.v[2] == t.v[0])
                t.deleted = true;
        }
    }
}

}

Decimate2::Mesh decimate_mesh_from_facecollection2(FaceCollection *coll)
{
  Decimate2::Mesh res;
  coll->Prepare();
  int s = coll->NumFaces();
  int vertex = 0;
  for(int i=0;i<s;i++)
    {
      Point p0 = coll->FacePoint(i,0);
      Point p1 = coll->FacePoint(i,1);
      Point p2 = coll->FacePoint(i,2);
      Decimate2::Vec3 v0 = { p0.x,p0.y,p0.z };
      Decimate2::Vec3 v1 = { p1.x,p1.y,p1.z };
      Decimate2::Vec3 v2 = { p2.x,p2.y,p2.z };
      Decimate2::Vertex vv0 = { v0 };
      Decimate2::Vertex vv1 = { v1 };
      Decimate2::Vertex vv2 = { v2 };
      res.vertices.push_back(vv0);
      res.vertices.push_back(vv1);
      res.vertices.push_back(vv2);
      Decimate2::Triangle t;
      t.v[0] = vertex;
      t.v[1] = vertex+1;
      t.v[2] = vertex+2;
      res.faces.push_back(t);
      vertex+=3;
    }
  return res;
}

class DecimateFaceCollection2 : public FaceCollection
{
public:
  DecimateFaceCollection2(FaceCollection *orig, Decimate2::Mesh &mesh) : mesh(mesh) { }

  void Collect(CollectVisitor &vis)
  {
  }
  void HeavyPrepare() { }
  std::string name() const { return "DecimateFaceCollection2"; }
  void Prepare() { }
  
  virtual int NumFaces() const { return mesh.faces.size(); }
  virtual int NumPoints(int face) const { return 3; }
  virtual Point FacePoint(int face, int point) const
  {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    Decimate2::Vertex pp = mesh.vertices[p];
    return Point(pp.p.x,pp.p.y,pp.p.z);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->PointNormal(face2,point2);
  }
  virtual float Attrib(int face, int point, int id) const { return 0.0; }
  virtual int AttribI(int face, int point, int id) const { return 0; }
  virtual unsigned int Color(int face, int point) const
  {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Color(face2,point2);
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->TexCoord(face2,point2);
  }
  virtual float TexCoord3(int face, int point) const {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->TexCoord3(face2,point2);

  }
  virtual VEC4 Joints(int face, int point) const {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Joints(face2,point2);
  }
  virtual VEC4 Weights(int face, int point) const {
    Decimate2::Triangle t = mesh.faces[face];
    int p;
    switch(point)
      {
      case 0: p=t.v[0]; break;
      case 1: p=t.v[1]; break;
      case 2: p=t.v[2]; break;
      default: p=0; break;
      };
    int face2 = p/3;
    int point2 = p%3;
    return orig->Weights(face2,point2);
  }
private:
  FaceCollection *orig;
  Decimate2::Mesh &mesh;
};
GameApi::P GameApi::PolygonApi::decimate2(GameApi::P p, float val)
{
  FaceCollection *coll = find_facecoll(e,p);
  Decimate2::Mesh m = decimate_mesh_from_facecollection2(coll);
  Decimate2::decimate(m.vertices,m.faces, val);
  return add_polygon2(e,new DecimateFaceCollection2(coll,m),1);
}

struct PointPair2
{
  int face1;
  int point1;
  int face2;
  int point2;
};

std::vector<PointPair2> get_pairs(FaceCollection &coll)
{
  std::vector<PointPair2> vec;
  int s = coll.NumFaces();
  for(int i=0;i<s;i++)
    {
      int p = coll.NumPoints(i);
      for(int j=0;j<s;j++)
	{
	  Point point1 = coll.FacePoint(i,j);
	  for(int i2=i+1;i2<s;i2++)
	    {
	      //if (i2==i) continue;
	      int p2=coll.NumPoints(i2);
	      for(int j2=0;j2<p2;j2++)
		{
		  Point point2 = coll.FacePoint(i2,j2);
		  if (fabs(point2.x-point1.x)<0.000001 &&
		      fabs(point2.y-point1.y)<0.000001 &&
		      fabs(point2.z-point1.z)<0.000001)
		    {
		      PointPair2 px;
		      px.face1 = i;
		      px.point1 = j;
		      px.face2 = i2;
		      px.point2 = j2;
		      vec.push_back(px);
		    }
		}
	    }

	  
	}
    }
  return vec;
}


struct PointIndex
{
  int face;
  int point;
};
bool operator<(const PointIndex &p1, const PointIndex &p2)
{
  if (p1.face==p2.face)
    {
      return p1.point<p2.point;
    }
  return p1.face<p2.face;
}


class PointMap2
{
public:
  virtual PointIndex Map(const PointIndex &p) const=0;
};

class PointMapImpl : public PointMap2
{
public:
  PointMapImpl(std::vector<PointPair2> vec)
  {
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	PointIndex i1 = { vec[i].face1, vec[i].point1 };
	PointIndex i2 = { vec[i].face2, vec[i].point2 };
	mymap[i1]=i2;
	mymap[i2]=i1;
      }
  }
  PointIndex Map(const PointIndex &p) const { return mymap[p]; }
private:
  mutable std::map<PointIndex,PointIndex> mymap;
};

class StringByteStore : public ByteStore
{
public:
  StringByteStore(std::string s2) : s(s2.begin(),s2.end()) {}
  int Size() const { return s.size(); }
  unsigned char &Get(int i) const { return s[i]; }
private:
  mutable std::vector<unsigned char> s;
};

GameApi::BS GameApi::MainLoopApi::string_bytestore(std::string s)
{
  return add_bytestore(e,new StringByteStore(s));
}

class TimingByteStoreUser : public MainLoopItem
{
public:
  TimingByteStoreUser(ByteStore &store, int ch, std::string s, float time_delta) : store(store), ch(ch), s(s), time_delta(time_delta) { current_pos =0;}
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    static float prev_time = 0.0;
    float time = e.time;
    float delta = time - prev_time;
    int jump = int(delta/time_delta);
    current_pos+=jump;
    store.Get(ch) = s[current_pos%s.size()];
    prev_time+=jump*time_delta;
  }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }

private:
  ByteStore &store;
  int ch;
  std::string s;
  float time_delta;
  int current_pos;
};
GameApi::ML GameApi::MainLoopApi::bytestore_timing(BS store, int ch, std::string s, float time_delta)
{
  ByteStore *bs = find_bytestore(e,store);
  return add_main_loop(e, new TimingByteStoreUser(*bs,ch,s,time_delta));
}

class TimingByteStoreArray : public MainLoopItem
{
public:
  TimingByteStoreArray(GameApi::Env &e, GameApi::EveryApi &ev, GameApi::BS store, std::string url, std::string homepage, float delta_time) : e(e), ev(ev), store(store), url(url),homepage(homepage),delta_time(delta_time) { }
  virtual void Collect(CollectVisitor &vis) { vis.register_obj(this); }
  virtual void HeavyPrepare() {
#ifndef EMSCRIPTEN
    e.async_load_url(url, homepage);
#endif

    GameApi::ASyncVec *ptr = e.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }

    std::string s = std::string(ptr->begin(),ptr->end());
    std::stringstream ss(s);
    std::string line;
    int ch;
    std::string s2;
    while(std::getline(ss,line)) {
      std::stringstream ss2(line);
      ss2 >> ch >> s2;
      chs.push_back(ch);
      strings.push_back(s2);
    }
    int s3 = chs.size();
    for(int i=0;i<s3;i++)
      {
	mls.push_back(ev.mainloop_api.bytestore_timing(store,chs[i],strings[i],delta_time));
      }
  }
  virtual void Prepare() { }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &ee)
  {
    int s = mls.size();
    for(int i=0;i<s;i++) {
      GameApi::ML ml = mls[i];
      MainLoopItem *item = find_main_loop(e,ml);
      item->execute(ee);
    }
  }
  void handle_event(MainLoopEvent &) { }
private:
  GameApi::Env &e;
  GameApi::EveryApi &ev;
  GameApi::BS store;
  std::string url, homepage;
  std::vector<int> chs;
  std::vector<std::string> strings;
  std::vector<GameApi::ML> mls;
  float delta_time;
};

GameApi::ML GameApi::MainLoopApi::bytestore_array(GameApi::EveryApi &ev, GameApi::BS bs, std::string url, float delta_time)
{
  return add_main_loop(e,new TimingByteStoreArray(e,ev,bs,url,gameapi_homepageurl,delta_time));
}

class Bytestore_render : public MainLoopItem
{
public:
  Bytestore_render(GameApi::Env &e, GameApi::EveryApi &ev, ByteStore &store, std::vector<MainLoopItem*> mls, std::string url, std::string homepage, float delta_x, float delta_y) : env(e), ev(ev), store(store), mls(mls), url(url), homepage(homepage), delta_x(delta_x), delta_y(delta_y) { }
  virtual void Collect(CollectVisitor &vis) {
    int s = mls.size();
    for(int i=0;i<s;i++)
      mls[i]->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
#ifndef EMSCRIPTEN
    env.async_load_url(url, homepage);
#endif

    GameApi::ASyncVec *ptr = env.get_loaded_async_url(url);
    if (!ptr) {
      std::cout << "p_url async not ready yet, failing..." << std::endl;
      return;
    }

    std::string s = std::string(ptr->begin(),ptr->end());
    std::stringstream ss(s);
    std::string line;
    char ch;
    int index;
    
    while(std::getline(ss,line)) {
      std::stringstream ss2(line);
      ss2 >> ch >> index;
      MapItem ii;
      ii.ch = ch;
      ii.index = index;
      maps.push_back(ii);
    }

    
  }
  virtual void Prepare() {
    int s = mls.size();
    for(int i=0;i<s;i++)
      mls[i]->Prepare();
    HeavyPrepare();
  }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    int s2 = store.Size();
    int xx = 0;
    int yy = 0;
    for(int x=0;x<s2;x++)
      {
	unsigned char &ch = store.Get(x);
	if (ch=='\n')
	  {
	    xx=0;
	    yy++;
	    continue;
	  }
	else
	  {
	    int s3 = maps.size();
	    for(int i=0;i<s3;i++)
	      {
		MapItem &ch2 = maps[i];
		if (ch2.ch==ch)
		  {
		    MainLoopEnv ee = e;
		    ee.in_MV = e.in_MV * Matrix::Translate(xx*delta_x, 0.0, yy*delta_y);
		    mls[i]->execute(ee);
		    
		    
		  }
	      }
	  }
	xx++;
      }
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    int s = mls.size();
    for(int i=0;i<s;i++)
      mls[i]->handle_event(e);
  }
  virtual std::vector<int> shader_id() {
    int s = mls.size();
    std::vector<int> res;
    for(int i=0;i<s;i++) {
      std::vector<int> v=mls[i]->shader_id();
      int s2 = v.size();
      for(int j=0;j<s2;j++)
	res.push_back(v[j]);
    }
    return res;
  }

  struct MapItem {
    char ch;
    int index;
  };
  
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  ByteStore &store;
  std::vector<MainLoopItem*> mls;
  std::vector<MapItem> maps;
  std::string url;
  std::string homepage;
  float delta_x,delta_y;
};

GameApi::ML GameApi::MainLoopApi::bytestore_render(GameApi::EveryApi &ev, GameApi::BS bs, std::vector<GameApi::ML> ml, std::string url, float delta_x, float delta_y)
{
  ByteStore *bs2 = find_bytestore(e,bs);

  int s = ml.size();
  std::vector<MainLoopItem*> vec;
  for(int i=0;i<s;i++)
    vec.push_back(find_main_loop(e,ml[i]));
  
  return add_main_loop(e,new Bytestore_render(e,ev,*bs2,vec,url,gameapi_homepageurl,delta_x,delta_y));
}

class Bytestore_pts : public PointsApiPoints
{
public:
  Bytestore_pts(ByteStore &store, PointsApiPoints &pts, char disable_char, char enable_char, bool default_is_enable) : store(store), pts(pts), ch1(disable_char), ch2(enable_char),default_is_enable(default_is_enable) { }
  virtual void Collect(CollectVisitor &vis) { pts.Collect(vis); }
  virtual void HeavyPrepare()
  {
  }
  virtual void HandleEvent(MainLoopEvent &event) { }
  virtual bool Update(MainLoopEnv &e) { return false; }
  virtual int NumPoints() const
  {
    return pts.NumPoints();
  }
  virtual Point Pos(int i) const {
    if (i>=0 && i<store.Size()) {
      unsigned char ch = store.Get(i);
      if (ch==ch1) return Point(-6667,-6667,-6667);
      if (ch==ch2) return pts.Pos(i);
    }
    if (default_is_enable) return pts.Pos(i);
    return Point(-6667,-6667,-6667);
  }
  virtual unsigned int Color(int i) const { return pts.Color(i); }
  virtual Vector Normal(int i) const { return pts.Normal(i); }
private:
  ByteStore &store;
  PointsApiPoints &pts;
  char ch1;
  char ch2;
  bool default_is_enable;
};

GameApi::PTS GameApi::MainLoopApi::bytestore_pts(GameApi::BS bs, GameApi::PTS pts, char disable_char, char enable_char, bool default_is_enable)
{
  ByteStore *bs2 = find_bytestore(e,bs);
  PointsApiPoints *pts2 = find_pointsapi_points(e,pts);
  return add_points_api_points(e, new Bytestore_pts(*bs2,*pts2,disable_char,enable_char,default_is_enable));
}

std::vector<int> *g_idx_vec;
FaceCollection *g_idx_faces;

float triArea(Point a, Point b, Point c)
{
  Vector ab = b-a;
  Vector ac = c-a;
  return 0.5f * Vector::CrossProduct(ab,ac).Dist();
}

float quadArea(Point p1, Point p2, Point p3, Point p4)
{
  float area1 = triArea(p1,p2,p3);
  float area2 = triArea(p1,p3,p4);
  return area1+area2;
}
#if 0
bool CompareFaces(int i1, int i2)
{
  int idx1 = g_idx_vec[i1];
  int idx2 = g_idx_vec[i2];
  Point p1 = g_idx_faces->FacePoint(idx1,0);
  Point p2 = g_idx_faces->FacePoint(idx1,1);
  Point p3 = g_idx_faces->FacePoint(idx1,2);
  Point p4 = g_idx_faces->FacePoint(idx1,3%g_idx_faces->NumPoints(idx1));

  float area1 = quadArea(p1,p2,p3,p4);
  
  Point pp1 = g_idx_faces->FacePoint(idx2,0);
  Point pp2 = g_idx_faces->FacePoint(idx2,1);
  Point pp3 = g_idx_faces->FacePoint(idx2,2);
  Point pp4 = g_idx_faces->FacePoint(idx2,3%g_idx_faces->NumPoints(idx2));

  float area2 = quadArea(pp1,pp2,pp3,pp4);
  return area1<area2;
}
#endif

class DecimatePolygon : public FaceCollection
{
public:
  DecimatePolygon(FaceCollection *coll, float val) : coll(coll),val(val) { firsttime = true; }
  std::string name() const { return "DecimatePolygon"; }
  virtual void Collect(CollectVisitor &vis)
  {
    coll->Collect(vis);
    vis.register_obj(this);
  }
  virtual void HeavyPrepare()
  {
    if (firsttime) {
      int s = coll->NumFaces();
      indices.clear();
      for(int i=0;i<s;i++)
	{
	  indices.push_back(i);
	}
      g_idx_vec = &indices;
      g_idx_faces = coll;


    auto compareFaces = [this](int i1, int i2) {
            auto faceArea = [this](int idx) {
                int n = coll->NumPoints(idx);
                if (n < 3) return 0.0f; // degenerate

                // handle triangles directly
                if (n == 3) {
                    Point a = coll->FacePoint(idx,0);
                    Point b = coll->FacePoint(idx,1);
                    Point c = coll->FacePoint(idx,2);
                    return triArea(a,b,c);
                }

                // handle quads (or n-gons, just take first 4 verts)
                Point p1 = coll->FacePoint(idx,0);
                Point p2 = coll->FacePoint(idx,1);
                Point p3 = coll->FacePoint(idx,2);
                Point p4 = coll->FacePoint(idx,3 % n);
                return quadArea(p1,p2,p3,p4);
            };

            float area1 = faceArea(i1);
            float area2 = faceArea(i2);

            if (std::fabs(area1 - area2) < 1e-6f) {
                // tie-breaker by index to ensure strict weak ordering
                return i1 < i2;
            }
            return area1 > area2;
        };
     int keep = std::max(1,(int)std::round(s*val));
    if (keep<s) {
      std::nth_element(indices.begin(),indices.begin()+keep,indices.end(),compareFaces);
      indices.resize(keep);
    }
    //   std::sort(indices.begin(),indices.end(),compareFaces);
      firsttime = false;
    }
  }
  virtual void Prepare() { coll->Prepare(); HeavyPrepare(); }
  virtual int NumFaces() const { if (!firsttime) return indices.size(); return 0; }
  virtual int NumPoints(int face) const { if (!firsttime) return coll->NumPoints(indices[face]); return 0; }
  virtual Point FacePoint(int face, int point) const
  {
    if (!firsttime)
      return coll->FacePoint(indices[face],point);
    return Point(0.0,0.0,0.0);
  }
  virtual Vector PointNormal(int face, int point) const
  {
    if (!firsttime)
      return coll->PointNormal(indices[face],point);
  }
  virtual float Attrib(int face, int point, int id) const
  {
    if (!firsttime)
      return coll->Attrib(indices[face],point,id);
    return 0.0;
  }
  virtual int AttribI(int face, int point, int id) const
  {
    if (!firsttime)
      return coll->AttribI(indices[face],point,id);
    return 0;
  }
  virtual unsigned int Color(int face, int point) const
  {
    if (!firsttime)
      return coll->Color(indices[face],point);
    return 0x00000000;
  }
  virtual Point2d TexCoord(int face, int point) const
  {
    if (!firsttime)
    return coll->TexCoord(indices[face],point);
    Point2d p;
    p.x=0.0f;
    p.y=0.0f;
    return p;
  }
  virtual float TexCoord3(int face, int point) const {
    if (!firsttime)
      return coll->TexCoord3(indices[face],point);
    return 0.0f;
  }
  virtual VEC4 Joints(int face, int point) const {
    if (!firsttime)
      return coll->Joints(indices[face],point);
    VEC4 v;
    return v;
  }
  virtual VEC4 Weights(int face, int point) const {
    if (!firsttime)
      return coll->Weights(indices[face],point);
    VEC4 v;
    return v;
  }
private:
  FaceCollection *coll;
  std::vector<int> indices;
  bool firsttime;
  float val;
};
GameApi::P GameApi::PolygonApi::decimate3(GameApi::P faces, float val)
{
  FaceCollection *coll = find_facecoll(e,faces);
  return add_polygon2(e, new DecimatePolygon(coll,val),1);
}

struct DecimateVertex_float
{
  Point p1;
};
struct DecimateVertex_double
{
  double x,y,z;
};
struct DecimateIndex_int
{
  unsigned int i1,i2,i3;
};
struct DecimateIndex_short
{
  unsigned short i1,i2,i3;
};
struct DecimateIndex_byte
{
  unsigned char i1,i2,i3;
};



template<typename T>
struct StridedIterator {
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using iterator_category = std::random_access_iterator_tag;

  uint8_t* base;
    size_t stride;

    StridedIterator(uint8_t* base, size_t stride) : base(base), stride(stride) {}

    T& operator*() const {
        return *reinterpret_cast<T*>(base);
    }

    StridedIterator& operator++() {
        base += stride;
        return *this;
    }

    StridedIterator& operator--() {
        base -= stride;
        return *this;
    }

  
    StridedIterator operator+(ptrdiff_t n) const {
        return StridedIterator(base + n * stride, stride);
    }
  StridedIterator operator-(ptrdiff_t n) const {
    return StridedIterator(base - n*stride, stride);
  }

  
    ptrdiff_t operator-(const StridedIterator& other) const {
        return (base - other.base) / stride;
    }

    bool operator<(const StridedIterator& other) const {
        return base < other.base;
    }

    bool operator<=(const StridedIterator& other) const {
        return base <= other.base;
    }

    bool operator>=(const StridedIterator& other) const {
      return base >= other.base;
    }

  
  
    bool operator==(const StridedIterator& other) const {
        return base == other.base;
    }

    bool operator!=(const StridedIterator& other) const {
        return base != other.base;
    }
};


class DecimateTF : public ForwardGLTF
{
public:
  DecimateTF(GLTFModelInterface *next, float val) : ForwardGLTF(next), next(next), val(val) { firsttime = true; }
  std::string name() const { return "DecimateTF"; }
  
  void Collect(CollectVisitor &vis)
  {
    next->Collect(vis);
    vis.register_obj(this);
  }

  void do_one(int i, int j) const
  {
	  const tinygltf::Mesh &mesh = next->get_mesh(i);
	const std::vector<tinygltf::Primitive> &vec = mesh.primitives;
	      int indices_index = vec[j].indices;
	      //std::cout << "INDICES_INDEX:" << indices_index << std::endl;
	      //if (indices_index<=0 || indices_index>=next->accessors_size())
	      //	continue;
	      const tinygltf::Accessor &acc = next->get_accessor(indices_index);
	      const tinygltf::BufferView &bv = next->get_bufferview(acc.bufferView);
	      const tinygltf::Buffer &buf = next->get_buffer(bv.buffer);
	      const unsigned char *data = &buf.data[0];
	      const unsigned char *data1 = data + bv.byteOffset + acc.byteOffset;
	      const unsigned int *data2 = (unsigned int*)data1;
	      int s3 = std::min(acc.count,bv.byteStride!=0?bv.byteLength/bv.byteStride:66666666); //buf.data.size()/sz;
	      //std::cout << "s3=" << s3 << std::endl;
	      const std::map<std::string,int> &attrs = next->get_mesh(i).primitives[j].attributes;
	      if (attrs.find("POSITION")==attrs.end()) return;
	      
	      const tinygltf::Accessor &acc2 = next->get_accessor(attrs.find("POSITION")->second);
	      if (acc2.bufferView==-1) return;
	      const tinygltf::BufferView &bv2 = next->get_bufferview(acc2.bufferView);

	      int s4 = std::min(acc2.count,bv2.byteStride!=0?bv2.byteLength/bv2.byteStride:66666666); //buf.data.size()/sz;
	      if (bv2.buffer==-1) return;

	      const tinygltf::Buffer &buf2 = next->get_buffer(bv2.buffer);
	      const unsigned char *kdata = &buf2.data[0];
	      const unsigned char *kdata1 = kdata + bv2.byteOffset + acc2.byteOffset;

	      const DecimateVertex_float *kdata2 = (const DecimateVertex_float*)kdata1;

	      
	      auto compareFaces_int = [kdata1,kdata2,acc2,bv2](DecimateIndex_int i1, DecimateIndex_int i2) {
		//std::cout << "compareFaces:" << i1 << " and " << i2 << std::endl;
		auto faceArea = [kdata1,kdata2,acc2,bv2](DecimateIndex_int idx) {
		  //std::cout << "found:" << idx << std::endl;

		  if (idx.i1>=acc2.count) { std::cout << "i1=" << idx.i1 << " .. fail.." << std::hex << idx.i1 << std::dec << std::endl; return 0.0f; }
		  if (idx.i2>=acc2.count) { std::cout << "i2=" << idx.i2 << " .. fail.." << std::hex << idx.i2 << std::dec << std::endl; return 0.0f; }
		  if (idx.i3>=acc2.count) { std::cout << "i3=" << idx.i3 << " .. fail.." << std::hex << idx.i3 << std::dec << std::endl; return 0.0f; }

		  //std::cout << "Indices: " << idx.i1 << " " << idx.i2 << " " << idx.i3 << std::endl;
		  size_t stride = bv2.byteStride?bv2.byteStride:sizeof(DecimateVertex_float);
		  assert(acc2.componentType==TINYGLTF_COMPONENT_TYPE_FLOAT);
		  assert(acc2.type==TINYGLTF_TYPE_VEC3);
		  const unsigned char *kdata00 = kdata1 + idx.i1*stride;
		  const unsigned char *kdata01 = kdata1 + idx.i2*stride;
		  const unsigned char *kdata02 = kdata1 + idx.i3*stride;
		  
		  const DecimateVertex_float *pdata0 = (const DecimateVertex_float*)kdata00;
		  const DecimateVertex_float *pdata1 = (const DecimateVertex_float*)kdata01;
		  const DecimateVertex_float *pdata2 = (const DecimateVertex_float*)kdata02;
		  Point p1 = pdata0->p1;
		  Point p2 = pdata1->p1;
		  Point p3 = pdata2->p1;
		  //std::cout << p1 << " " << p2 << " " << p3 << std::endl;
		  return triArea(p1,p2,p3);
		};
		float area1 = faceArea(i1);
		float area2 = faceArea(i2);
		if (std::fabs(area1-area2) < 1e-6f) {
		  return i1.i1<i2.i1;
		}
		return area1 > area2;
	      };

	      auto compareFaces_short = [compareFaces_int,kdata1,kdata2,acc2,bv2](DecimateIndex_short i1, DecimateIndex_short i2) {
		DecimateIndex_int ii1,ii2;
		ii1.i1 = i1.i1;
		ii1.i2 = i1.i2;
		ii1.i3 = i1.i3;
		ii2.i1 = i2.i1;
		ii2.i2 = i2.i2;
		ii2.i3 = i2.i3;
		return compareFaces_int(ii1,ii2);
	      };
		auto compareFaces_byte = [compareFaces_int,kdata1,kdata2,acc2,bv2](DecimateIndex_byte i1, DecimateIndex_byte i2) {
		DecimateIndex_int ii1,ii2;
		ii1.i1 = i1.i1;
		ii1.i2 = i1.i2;
		ii1.i3 = i1.i3;
		ii2.i1 = i2.i1;
		ii2.i2 = i2.i2;
		ii2.i3 = i2.i3;
		return compareFaces_int(ii1,ii2);
		};

	      
	      int keep = std::max(1,(int)std::round(s3*val));
	      //std::cout << "KEEP:" << keep << " s=" << s3 << std::endl;
	      //std::cout << acc.type << " " << acc.componentType << std::endl;
	      if (acc.type==TINYGLTF_TYPE_SCALAR && (acc.componentType==TINYGLTF_COMPONENT_TYPE_INT||acc.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)) {
	      if (keep<s3) {
		int bufidx = new_buffer.size();
		int bufvidx = new_bufviews.size();
		int accidx = new_accessors.size();
	      
		new_buffer.push_back(tinygltf::Buffer());
		new_bufviews.push_back(tinygltf::BufferView());
		new_accessors.push_back(tinygltf::Accessor());
		
		tinygltf::Buffer &buf2a = new_buffer[new_buffer.size()-1];
		tinygltf::BufferView &bv2a = new_bufviews[new_bufviews.size()-1];
		tinygltf::Accessor &acc2a = new_accessors[new_accessors.size()-1];
	      
		buf2a = buf;
		bv2a = bv;
		acc2a =acc;
		
		acc2a.bufferView = start_bufview + bufvidx;
		bv2a.buffer = start_buffer + bufidx;
		//vec2[j].indices = start_accessors + accidx;
		indices_to_index[indices_index] = new_accessors.size()-1;

		unsigned char *udata = &buf2a.data[0];
		unsigned char *udata1 = udata + bv2a.byteOffset + acc2a.byteOffset;
		DecimateIndex_int *udata2 = (DecimateIndex_int*)udata1;
		//std::cout << "STRIDE: " << bv2a.byteStride << " " << sizeof(DecimateIndex_int) << std::endl;
		size_t stride = bv2a.byteStride?bv2a.byteStride*3:sizeof(DecimateIndex_int);
		auto begin = StridedIterator<DecimateIndex_int>((uint8_t*)udata2,stride);
		auto end = begin + keep/3;
		auto end2 = begin + s3/3;
		std::nth_element(begin,end,end2,compareFaces_int);
		bv2a.byteLength = std::min(bv2a.byteLength,keep*stride/3);
		
		acc2a.count = std::min(acc2a.count,size_t(keep));

	      }
	      } else
	      if (acc.type==TINYGLTF_TYPE_SCALAR && (acc.componentType==TINYGLTF_COMPONENT_TYPE_SHORT||acc.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)) {
	      if (keep<s3) {
		int bufidx = new_buffer.size();
		int bufvidx = new_bufviews.size();
		int accidx = new_accessors.size();
	      
		new_buffer.push_back(tinygltf::Buffer());
		new_bufviews.push_back(tinygltf::BufferView());
		new_accessors.push_back(tinygltf::Accessor());
		
		tinygltf::Buffer &buf2a = new_buffer[new_buffer.size()-1];
		tinygltf::BufferView &bv2a = new_bufviews[new_bufviews.size()-1];
		tinygltf::Accessor &acc2a = new_accessors[new_accessors.size()-1];
	      
		buf2a = buf;
		bv2a = bv;
		acc2a =acc;
		
		acc2a.bufferView = start_bufview + bufvidx;
		bv2a.buffer = start_buffer + bufidx;
		//vec2[j].indices = start_accessors + accidx;
		indices_to_index[indices_index] = new_accessors.size()-1;

		unsigned char *udata = &buf2a.data[0];
		unsigned char *udata1 = udata + bv2a.byteOffset + acc2a.byteOffset;
		DecimateIndex_short *udata2 = (DecimateIndex_short*)udata1;
		//std::cout << "STRIDE: " << bv2a.byteStride << " " << sizeof(DecimateIndex_short) << std::endl;
		size_t stride = bv2a.byteStride?bv2a.byteStride*3:sizeof(DecimateIndex_short);
		auto begin = StridedIterator<DecimateIndex_short>((uint8_t*)udata2,stride);
		auto end = begin + keep/3;
		auto end2 = begin + s3/3;
		std::nth_element(begin,end,end2,compareFaces_short);
		bv2a.byteLength = std::min(bv2a.byteLength,keep*stride/3);
		
		acc2a.count = std::min(acc2a.count,size_t(keep*3));

	      }
	      } else
			      if (acc.type==TINYGLTF_TYPE_SCALAR && (acc.componentType==TINYGLTF_COMPONENT_TYPE_BYTE||acc.componentType==TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)) {
	      if (keep<s3) {
		int bufidx = new_buffer.size();
		int bufvidx = new_bufviews.size();
		int accidx = new_accessors.size();
	      
		new_buffer.push_back(tinygltf::Buffer());
		new_bufviews.push_back(tinygltf::BufferView());
		new_accessors.push_back(tinygltf::Accessor());
		
		tinygltf::Buffer &buf2a = new_buffer[new_buffer.size()-1];
		tinygltf::BufferView &bv2a = new_bufviews[new_bufviews.size()-1];
		tinygltf::Accessor &acc2a = new_accessors[new_accessors.size()-1];
	      
		buf2a = buf;
		bv2a = bv;
		acc2a =acc;
		
		acc2a.bufferView = start_bufview + bufvidx;
		bv2a.buffer = start_buffer + bufidx;
		//vec2[j].indices = start_accessors + accidx;
		indices_to_index[indices_index] = new_accessors.size()-1;

		unsigned char *udata = &buf2a.data[0];
		unsigned char *udata1 = udata + bv2a.byteOffset + acc2a.byteOffset;
		DecimateIndex_byte *udata2 = (DecimateIndex_byte*)udata1;
		//std::cout << "STRIDE: " << bv2a.byteStride << " " << sizeof(DecimateIndex_byte) << std::endl;
		size_t stride = bv2a.byteStride?bv2a.byteStride*3:sizeof(DecimateIndex_byte);
		auto begin = StridedIterator<DecimateIndex_byte>((uint8_t*)udata2,stride);
		auto end = begin + keep/3;
		auto end2 = begin + s3/3;
		std::nth_element(begin,end,end2,compareFaces_byte);
		bv2a.byteLength = std::min(bv2a.byteLength,keep/3*stride);
		
		acc2a.count = std::min(acc2a.count,size_t(keep*3));

	      }
	      }
	      else { std::cout << "Wrong type or componentType:" << acc.type << " " << acc.componentType << std::endl; }
  
  }
  
  void HeavyPrepare()
  {
    if (firsttime) {
      start_meshes = next->meshes_size();
      start_accessors = next->accessors_size();
      start_bufview = next->bufferviews_size();
      start_buffer = next->buffers_size();
      int s = next->meshes_size();
      //std::cout << "Meshes: " << s << std::endl;
      for(int i=0;i<s;i++)
	{

	  if (m_enabled.size()<=i)
	    {
	      m_enabled.resize(i+1);
	    }
	  if (!m_enabled[i])
	    {
	      const tinygltf::Mesh &mesh = next->get_mesh(i);
	      const std::vector<tinygltf::Primitive> &vec = mesh.primitives;
	      int s2 = vec.size();
	      //std::cout << "Primitives2: " << s2 << std::endl;
	      for(int j=0;j<s2;j++)
		do_one(i,j);
	      m_enabled[i]=true;
	    }
	}
    }

    firsttime = false;
  }
  void Prepare()
  {
    next->Prepare();
    HeavyPrepare();
  }

  virtual int accessors_size() const { if (firsttime) return 0; return next->accessors_size()+new_accessors.size(); }
  virtual const tinygltf::Accessor &get_accessor(int i) const
  {
    if (i>=0 && i<next->accessors_size())
      return next->get_accessor(i);
    //std::cout << "get new acc:" << i << " " << start_accessors << " " << new_accessors.size() << std::endl;
    return new_accessors[i-start_accessors];
  }
  virtual int bufferviews_size() const { if (firsttime) return 0; return next->bufferviews_size() + new_bufviews.size(); }
  virtual const tinygltf::BufferView &get_bufferview(int i) const
  {
    if (i>=0 && i<next->bufferviews_size())
      return next->get_bufferview(i);
    return new_bufviews[i-start_bufview];
  }


  virtual int buffers_size() const { if (firsttime) return 0; return next->buffers_size() + new_buffer.size(); }
  virtual const tinygltf::Buffer &get_buffer(int i) const
  {
    if (i>=0 && i<next->buffers_size())
      return next->get_buffer(i);
    return new_buffer[i-start_buffer];
  }

  virtual int meshes_size() const { if (firsttime) return 0; return next->meshes_size(); }
  virtual const tinygltf::Mesh &get_mesh(int i) const
  {
    if (m_enabled.size()<=i)
      {
	m_enabled.resize(i+1);
      }
    if (!m_enabled[i])
      {
	const tinygltf::Mesh &mesh = next->get_mesh(i);
	const std::vector<tinygltf::Primitive> &vec = mesh.primitives;
	int s2 = vec.size();
	//std::cout << "Primitives2: " << s2 << std::endl;
	for(int j=0;j<s2;j++)
	  do_one(i,j);
	m_enabled[i]=true;
      }
    
    
    if (m_meshes.size()<=i)
      {
	m_meshes.resize(i+1);
      }
    
    m_meshes[i] = next->get_mesh(i);
    int s = m_meshes[i].primitives.size();
    for(int ii=0;ii<s;ii++)
      {
	//std::cout << "map " << m_mesh.primitives[i].indices << " to " << start_accessors + indices_to_index[m_mesh.primitives[i].indices] << std::endl;
	if (indices_to_index.find(m_meshes[i].primitives[ii].indices)!=indices_to_index.end())
	  m_meshes[i].primitives[ii].indices = start_accessors + indices_to_index[m_meshes[i].primitives[ii].indices];
      }

    return m_meshes[i];
  }
private:
  GLTFModelInterface *next;
  float val;
  bool firsttime = true;

  int start_meshes = -1;
  int start_accessors = -1;
  int start_bufview = -1;
  int start_buffer = -1;
  //std::vector<tinygltf::Mesh> new_mesh;
  //mutable tinygltf::Mesh m_mesh;
  mutable std::vector<tinygltf::Accessor> new_accessors;
  mutable std::vector<tinygltf::BufferView> new_bufviews;
  mutable std::vector<tinygltf::Buffer> new_buffer;

  mutable std::map<int,int> indices_to_index;
  mutable std::vector<tinygltf::Mesh> m_meshes;
  mutable std::vector<bool> m_enabled;
  };

GameApi::TF GameApi::PolygonApi::decimate_tf(GameApi::TF mesh, float val)
{
  GLTFModelInterface *i = find_gltf(e,mesh);
  return add_gltf(e, new DecimateTF(i,val));
}

class MoveFromMV : public MainLoopItem
{
public:
  MoveFromMV(MainLoopItem *next) : next(next) { }
  virtual void Collect(CollectVisitor &vis) { next->Collect(vis); }
  virtual void HeavyPrepare() { }
  virtual void Prepare() { next->Prepare(); HeavyPrepare(); }
  virtual void FirstFrame() { }
  virtual void execute(MainLoopEnv &e)
  {
    Matrix m = e.in_MV;
    Vector v = m.get_translate();
    quake_pos_x = v.dx;
    quake_pos_y = v.dz;
    quake_rot_y = 0.0;
    next->execute(e);
  }
  virtual void handle_event(MainLoopEvent &e) { next->handle_event(e); }
  virtual std::vector<int> shader_id() { return next->shader_id(); }
private:
  MainLoopItem *next;
};

GameApi::ML GameApi::MainLoopApi::get_movement_from_MV(GameApi::ML ml)
{
  MainLoopItem *mainloop = find_main_loop(e,ml);
  return add_main_loop(e, new MoveFromMV(mainloop));
}
