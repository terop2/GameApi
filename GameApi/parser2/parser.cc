
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ios>

struct Class {
  std::string classname;
  std::string contents;
};

struct Function {
  std::string funcsignature;
  std::string contents;
  std::vector<std::string> link;
};

int find_ch(std::string s, char ch)
{
  int ss = s.size();
  for(int i=0;i<ss;i++) if (s[i]==ch) return i;
  return -1;
}

class Parser
{
public:
  Parser(std::string apiname) : apiname(apiname) { }
  std::string trim(std::string s) {
    int i1=0;
    while(s[i1]==' ') i1++;
    int i2=s.length()-1;
    while(s[i2]==' ') i2--;
    return s.substr(i1,i2-i1+1);
  }
  int find_one_of(std::string line, std::string chars)
  {
    int s = line.size();
    for(int i=0;i<s;i++)
      {
	int ss = chars.size();
	bool cont=false;
	for(int j=0;j<ss;j++) {
	  if (line[i]==chars[j]) { cont=true; continue; }
	}
	if (!cont) { return i; }
      }
    return s;
  }
  int find(std::string line, std::string str)
  {
    std::string::iterator i = std::search(line.begin(),line.end(),str.begin(), str.end());
    if (i != line.end()) {
      return i-line.begin();
    }
    return -1;
#if 0
    int s = line.size();
    for(int i=0;i<s;i++)
      {
	int ss = str.size();
	bool match=false;
	for(int j=0;j<std::min(ss,s-i);j++) {
	  if (j==0) match=true;
	  if (line[i+j]!=str[j]) match=false;
	}
	if (match==true) return i;
      }
    return -1;
#endif
  }
  void parse(std::string filename) {
    std::ifstream ss(filename.c_str(),std::ios_base::in|std::ios_base::binary);
    std::string line;
    int type=-1;
    std::vector<std::string> link;
    std::string data1,data2;
    std::string sign;
    std::string class_name;
    std::string prev_line;
    while(std::getline(ss,line,'\n')) {
      int ch = find(line, "class ");
      if (ch!=-1)
      {
	type=1;
	std::string label = ch+6<line.size()?line.substr(ch+6):"";
	std::string chars = " _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int pos = find_one_of(label, chars);
	std::string classname = label.substr(0,pos);
	classname = trim(classname);
	class_name = classname;
	//if (classname=="MainLoopBlocker_win32_and_emscripten")
	  {
	    //std::cout << "CLASSNAME: " << classname << std::endl;
	  }
	//std::cout << line << std::endl;
	count=0;
	data1="";
	if (find(prev_line,"template")!=-1) {
	  data1+=prev_line+"\n";
	}
	data1+=line+"\n";
      } 
      int ch2 = find(line, apiname);
      if (ch2!=-1 && apiname!="")
	{
	  int ch4 = find(line,"(");
	  if (ch4!=-1) {
	  type=2;
	  sign = line;
	  count=0;
	  //std::cout << "LL:" << line << std::endl;
	  link=std::vector<std::string>();
	data2="";
	  data2+=line+"\n";
	  }
	}
      int ch3 = find(line, "new ");
      if (ch3!=-1)
	{
	  std::string label = ch3+4<line.size()?line.substr(ch3+4):"";
	  std::string chars = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	  int pos = find_one_of(label, chars);
	  std::string classname = label.substr(0,pos);
	  link.push_back(classname);
	  //std::cout << "LINKCLASSNAME: " << classname << std::endl;
	}
      int ch4 = find(line, "{");
      if (ch4!=-1)
	{
	  count++;
	  //std::cout << "{" << count << std::endl;
	}
      if (count>=1) {
	//std::cout << "L:" << line << std::endl;
	data1+=line+"\n";
	data2+=line+"\n";
	}
      int ch5 = find(line, "}");
      if (ch5!=-1)
	{
	  //std::cout << count << "}" << std::endl;
	  count--;
	  //std::cout << "I:" << count << ":" << type << ":" << line << std::endl;
	  if (count==0) {
	    if (type==1)
	      {
		//std::cout << "Add class: " << class_name << " :: " << data1 << std::endl;
		Class cls;
		cls.classname = class_name;
		cls.contents = data1;
		classes.push_back(cls);
		sign="";
		data1="";
		type=-1;
		link=std::vector<std::string>();

	      }
	    if (type==2)
	      {
		//std::cout << "Add function: " << sign << " :: " << data2 << std::endl;
		Function f;
		f.funcsignature = sign;
		f.contents = data2;
		f.link = link;
		functions.push_back(f);
		sign="";
		data2="";
		link=std::vector<std::string>();
		type=-1;
	      }
	  }
	}
      prev_line=line;
    }

    

  }
  void print(std::string funcname)
  {
    int s = functions.size();
    for(int i=0;i<s;i++)
      {
	Function f = functions[i];
	int pos = find(f.funcsignature, "::" + funcname+"(");
	if (pos!=-1)
	  {
	    //std::cout << "Found: " << f.funcsignature << std::endl;
	    std::vector<std::string> link = f.link;
	    //std::cout << "LINK: "<< link << std::endl;
	    int ss = classes.size();
	    for(int j=0;j<ss;j++)
	      {
		int ss1 = link.size();
		for(int k=0;k<ss1;k++) {
		if (classes[j].classname==link[k])
		  {
		    std::cout << "#ifndef CLASS_" + classes[j].classname << std::endl;
		    std::cout << "#define CLASS_" + classes[j].classname << std::endl;
		    std::cout << classes[j].contents << std::endl;
		    std::cout << "#endif" << std::endl;
		  }
		}
	      }
	    //std::cout << f.funcsignature << std::endl;
	    std::cout << f.contents << std::endl;
	    //break;
	  }
      }
  }
  void save(std::string filename)
  {
    std::ofstream ss1(filename.c_str());
    int s = classes.size();
    for(int i=0;i<s;i++) {
      Class c = classes[i];
      ss1 << c.classname << "@" << c.contents << "£";
      //std::cout << c.classname << " " << c.contents << std::endl;
    }
    ss1.close();
  }
  void save2(std::string filename2) {
    std::ofstream ss2(filename2.c_str(),std::ios_base::app);
    int ss = functions.size();
    for(int j=0;j<ss;j++) {
      Function f = functions[j];
      ss2 << f.funcsignature << "@";
      int s = f.link.size();
      for(int i=0;i<s;i++)
	{
	  if (i!=0) ss2<< ":";
	  ss2 << f.link[i];
	}
      ss2 << "@" << f.contents << "£";
    }
    ss2.close();
  }
private:
  std::string apiname;
  int count=0;
  std::vector<Class> classes;
  std::vector<Function> functions;
};

std::string filenames[]= { 
  //"VectorTools.hh", "Buffer.hh",				      
  //"Intersect.hh", "Bitmap.hh", "Effect2.hh", "Shader.hh", "VolumeObjects.hh", "VolumeObjects.cc",
  //"Tree.hh", "Event.hh", "ObjectCreator.hh", "IntersectObject.hh",
  //"Widgets.hh", "Category.hh", "Plane.hh","Coordinates.hh", "Triangle.hh", "Game.hh",
  //"Editor.hh","Web.hh", "Functions.hh", "ShaderExpr.hh", "Font.hh", "FontEditor.hh",
  //"FreeType.hh", "FreeType2.hh", "VertexArray.hh", "StateChange.hh", "DistanceObject.hh", "GameRunner.hh",
  //"Graph.hh", "Effect.hh", 
  "GameApi.cc",
  "GameApi_an.cc","GameApi_bm.cc","GameApi_co.cc","GameApi_dr.cc","GameApi_ev.cc",
  "GameApi_ex.cc","GameApi_fbo.cc","GameApi_f.cc","GameApi_fn.cc",
  "GameApi_fnt.cc","GameApi_gr.cc","GameApi_gui.cc",
  "GameApi_lay.cc",		  
  "GameApi_li.cc","GameApi_main.cc","GameApi_mx.cc","GameApi_pc.cc",
  "GameApi_phy.cc","GameApi_pla.cc","GameApi_pl.cc","GameApi_pt.cc",
  "GameApi_pts.cc","GameApi_sh.cc","GameApi_shm.cc","GameApi_sm.cc",
  "GameApi_spa.cc","GameApi_sp.cc","GameApi_st.cc","GameApi_su.cc",
  "GameApi_tex.cc","GameApi_tr.cc","GameApi_trk.cc","GameApi_tx.cc", 
  "GameApi_vbo.cc","GameApi_ve.cc","GameApi_vo.cc","GameApi_wmod.cc",
  "GameApi_wv.cc","GameApi_vx.cc","GameApi_cut.cc","GameApi_in.cc",
  "GameApi_imp.cc","GameApi_plane.cc","GameApi_integrator.cc" };


std::string apinames[] = {
  "MainLoopApi","mainloop_api",
  "PointApi","point_api",
  "VectorApi","vector_api",
  "MatrixApi","matrix_api",
  "SpriteApi","sprite_api",
  "GridApi","grid_api",
  "BitmapApi","bitmap_api",
  "PolygonApi","polygon_api",
  "BoolBitmapApi","bool_bitmap_api",
  "FloatBitmapApi","float_bitmap_api",
  "ContinuousBitmapApi","cont_bitmap_api",
  "FontApi","font_api",
  "AnimApi","anim_api",
  "EventApi","event_api",
  "FunctionApi","function_api",
  "VolumeApi","volume_api",
  "FloatVolumeApi","float_volume_api",
  "ColorVolumeApi","color_volume_api",
  "DistanceFloatVolumeApi","dist_api",
  "VectorVolumeApi","vector_volume_api",
  "ShaderApi","shader_api",
  "StateChangeApi","state_change_api",
  "TextureApi","texture_api",
  "SeparateApi","separate_api",
  "WaveformApi","waveform_api",
  "ColorApi","color_api",
  "LinesApi","lines_api",
  "PlaneApi","plane_api",
  "PointsApi","points_api",
  "VoxelApi","voxel_api",
  "FrameBufferApi","fbo_api",
  "SampleCollectionApi","sample_api",
  "TrackerApi","tracker_api",
  "ShaderModuleApi","sh_api",
  "WModApi","mod_api",
  "PhysicsApi","physics_api",
  "TriStripApi","ts_api",
  "CutterApi","cutter_api",
  "BooleanOps","bool_api",
  "CollisionPlane","collision_api",
  "MovementNode","move_api",
  "ImplicitApi","implicit_api",
  "PickingApi","picking_api",
  "TreeApi","tree_api",
  "MaterialsApi","materials_api",
  "UberShaderApi","uber_api",
  "CurveApi","curve_api",
  "MatricesApi","matrices_api",
  "Skeletal","skeletal_api",
  "PolygonArrayApi","polygon_arr_api",
  "PolygonDistanceField","polygon_dist_api",
  "BlockerApi","blocker_api",
  "VertexAnimApi","vertex_anim_api",
  "NewPlaneApi","newplane_api",
  "SurfaceApi","surface_api",
  "LowFrameBufferApi", "low_frame_api"
};

std::string convertapiname(std::string apiname)
{
  int s=sizeof(apinames)/sizeof(apinames[0]);
  for(int i=0;i<s;i+=2)
    {
      if (apiname==apinames[i+1]) { return apinames[i]; }
    }
  return apiname;
}

int main(int argc, char *argv[])
{
  bool save=false;
  if (argc==2 && std::string(argv[1])==std::string("--save")) {
    int s=sizeof(apinames)/sizeof(apinames[0]);
    for(int i=0;i<s;i+=2)
      {
	Parser p(apinames[i]);

	int s = sizeof(filenames)/sizeof(filenames[0]);
	for(int j=0;j<s;j++)
	  {
	    std::string filename = filenames[j];
	    p.parse(std::string("../") + filename);
	  }
	if (i==0)
	  p.save("data.txt");
	p.save2("data2.txt");
      }
    exit(0);
  }
  if (argc!=3) { std::cout << argv[0] << " ApiName funcname" << std::endl; exit(0); }
  std::string apiname = convertapiname(argv[1]);
  std::string funcname = argv[2];

  Parser p(apiname);
  int s = sizeof(filenames)/sizeof(filenames[0]);
  for(int i=0;i<s;i++)
    {
      std::string filename = filenames[i];
      p.parse(std::string("../") + filename);
    }
  
  p.print(funcname);
}
