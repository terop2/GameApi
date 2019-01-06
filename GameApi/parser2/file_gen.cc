#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

std::string removed_names[] = {
  "polygon_api", "empty"
};

bool is_removed(std::string classname, std::string funcname)
{
  int s= sizeof(removed_names)/sizeof(std::string);
  for(int i=0;i<s;i+=2) {
    std::string c=removed_names[i];
    std::string f=removed_names[i+1];
    if (classname==c && funcname==f) return true;
  }
  return false;
}

std::string codegen(std::string classname, std::string funcname)
{
  if (is_removed(classname,funcname)) {
    std::cout << "Codegen(skipped): " << classname << "::" << funcname << std::endl;
    return "";
  }

  std::cout << "Codegen: " << classname << "::" << funcname << std::endl;
  FILE *f = popen((std::string("parser.exe ") + classname + " " + funcname).c_str() ,"r");
  std::string res;
  do {
    int c = fgetc(f);
    if (feof(f)) break;
    if (c!='\r')
      res+=c;
  } while(1);
  return res;
}

int find_ch(std::string s, char ch)
{
  int ss = s.size();
  for(int i=0;i<ss;i++) { if (s[i]==ch) return i; }
  return -1;
}

std::vector<std::pair<std::string,std::string> > parse_file(std::string contents)
{
  std::stringstream ss(contents);
  std::string line;
  std::vector<std::pair<std::string,std::string> > vec;
  while(std::getline(ss,line)) {
    int pos1 = find_ch(line,'.');
    std::string rest = line.substr(pos1+1);
    int pos2 = find_ch(rest,'.');
    std::string first = line.substr(pos1+1,pos2);
    std::string rest2 = rest.substr(pos2+1);
    int pos3 = find_ch(rest2,'(');
    std::string second = rest2.substr(0,pos3);
    vec.push_back(std::make_pair(first,second));
  }
  return vec;
}

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
void error(std::string reason);

std::string map_apiname(std::string apiname)
{
  int s = sizeof(apinames)/sizeof(std::string);
  for(int i=0;i<s;i+=2)
    {
      std::string p1 = apinames[i];
      std::string p2 = apinames[i+1];
      if (apiname==p2) return p1;
    }
  error("NOT FOUND " + apiname);
  return "";
}


std::string output_contents(std::vector<std::pair<std::string,std::string> > vec)
{
  int s = vec.size();
  std::string res;
  for(int i=0;i<s;i++)
    {
      res+=codegen(vec[i].first,vec[i].second)+"\n";
    }
  return res;
}

int find_str(std::string contents, std::string label)
{
  int s = contents.size();
  int s2 = label.size();
  for(int i=0;i<s-s2+1;i++)
    {
      int j = 0;
      for(;j<s2;j++)
	{
	  if (contents[i+j]!=label[j]) break;
	}
      if (j==s2) { return i; }
    }
  return -1;
}

int find_chars(std::string contents, int pos, std::string chars)
{
  int s = contents.size();
  for(int i=pos;i<s;i++)
    {
      bool found = false;
      int ss = chars.size();
      for(int j=0;j<ss;j++)
	{
	  if (contents[i]==chars[j]) found=true;
	}
      if (!found) return i; 
    }
  return s;
}

std::pair<std::vector<std::string>,std::vector<std::string> > find_adds(std::string contents)
{
  std::stringstream ss(contents);
  std::string line;
  std::vector<std::string> add_vec,find_vec;
  while(std::getline(ss,line)) {
    int pos = find_str(line,"add_");
    std::string add;
    std::string find;
    if (pos!=-1) {
      int pos_a = find_chars(line,pos,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
      add=line.substr(pos,pos_a-pos);
    }
    int pos2 = find_str(line,"find_");
    if (pos2!=-1)
      {
	int pos2_a = find_chars(line,pos2,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	find=line.substr(pos2,pos2_a-pos2);
      }
    if (add!="") { add_vec.push_back(add); }
    if (find!="") { find_vec.push_back(find); }
  }
  return std::make_pair(find_vec,add_vec);
}

void error(std::string reason) { std::cout << reason << std::endl; exit(0); }

bool find(std::string s, std::string s2)
{
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      int sj = s2.size();
      bool found = true;
      for(int j=0;j<sj;j++)
	{
	  if (s[i+j]!=s2[j]) found=false;
	}
      if (found) { return true; }
    }
  return false;
}


std::string network(std::string url)
{
#ifdef WINDOWS
  std::string cmd = "..\\curl\\curl.exe -s -N --url " + url;
#else
  std::string cmd = "curl -s -N --url " + url;
#endif
#ifdef __APPLE__
  FILE *f = popen(cmd.c_str(), "r");
#else
#ifdef LINUX
  FILE *f = popen(cmd.c_str(), "r");
#else
  FILE *f = popen(cmd.c_str(),"rb");
#endif
#endif
  if (!f) {std::cout << "popen failed!" << std::endl; }
  
  unsigned char c;
  std::vector<unsigned char> buffer;
  while(fread(&c,1,1,f)==1) { buffer.push_back(c); }
  bool is404 = find(std::string(buffer.begin(), buffer.end()), "404 Not Found");
  if (is404) error(std::string("404 not found: " + url));
  //std::cout << buffer.size() << std::endl;
  if (buffer.size()==0) { error(std::string("url not found: ") + url); }
  return std::string(buffer.begin(),buffer.end());
}

std::string replace_string(std::string s, std::string rep, std::string subst)
{
  int val = s.find(rep);
  s.replace(s.begin()+val, s.begin()+val+rep.size(),subst);

  return s;
}

std::string get_last_line(std::string lines)
{
  std::string s;
  std::stringstream ss(lines);
  while(std::getline(ss,s));
  return s;
}
std::string find_label(std::string line)
{
  int pos = find_ch(line,'=');
  return line.substr(4,pos-4);
}
std::string remove_chars(std::string contents, char ch)
{
  std::string res;
  int s = contents.size();
  for(int i=0;i<s;i++)
    {
      if (contents[i]!=ch)
	res+=contents[i];
    }
  return res;
}
bool is_in_list(std::vector<std::pair<std::string,std::string> > vec, std::string cl, std::string fn)
{
  if (cl=="@"&&fn=="@") return true;
  int s = vec.size();
  for(int i=0;i<s;i++) {
    //std::cout << "Compare: " << vec[i].first << "==" << cl << " & " << vec[i].second << "==" << fn << std::endl;
    if (vec[i].first==cl && vec[i].second==fn) return true;
  }
  return false;
}

std::string add_spaces(std::string s, int count, int count2)
{
  std::string res;
  count-=s.size();
  for(int i=0;i<count;i++) res+=' ';
  res+=s;
  count2-=s.size();
  for(int i=0;i<count2;i++) res+=' ';
  return res;
}

void usage() {
    std::cout << "./file_gen.exe --url url" << std::endl;
    std::cout << "./file_gen.exe --" << std::endl;
    exit(0);
}
int main(int argc, char *argv[])
{
  if (argc<2) usage();
  std::string arg = argv[1];
  std::string url = "";
  bool use_existing=true;
  if (arg=="--url") { use_existing=false; url=argv[2]; } 
  else {
    if (arg=="--") { use_existing=true; } 
    else {
      usage();
    }
  }
  std::string contents;
  if (!use_existing) {
    url = replace_string(url, "mesh.php", "mesh_pre.php");
    contents = network(url);
    std::ofstream ss("contents.txt");
    ss << contents;
    ss.close();
  } else {
    std::ifstream c("contents.txt");
    std::string s;
    while(std::getline(c,s)) { contents+=s + "\n"; }
    contents.pop_back();
  }
  //std::cout << contents << std::endl;
  std::vector<std::pair<std::string,std::string> > vec = parse_file(contents);

  std::ifstream sk("dependent.txt");
  std::string sc,sf;
  std::string p1b,p2b;
  std::string line;
  while(std::getline(sk,line)) { 
    std::stringstream ss(line);
    ss >> sc >> sf >> p1b >> p2b;
    std::cout << "...Checking dep: " << add_spaces(sc,15,0) << "::" << add_spaces(sf,0,15) << " ";
    if (is_in_list(vec,sc,sf)) {
      std::cout << " -> " << add_spaces(p1b,15,0) << "::" << add_spaces(p2b,0,15) << std::endl;
      vec.push_back(std::make_pair(p1b,p2b)); 
    } else {
      std::cout << "(skipped)" << std::endl;
    }
  }

  std::sort(vec.begin(),vec.end());
  std::vector<std::pair<std::string,std::string> >::iterator it = std::unique(vec.begin(),vec.end());
  vec.erase(it,vec.end());

  //int s = vec.size();
  //for(int i=0;i<s;i++)
  //  {
  //     std::cout << vec[i].first << " " << vec[i].second << std::endl;
  //  }
  std::string res;
  res+="#include \"Common.hh\"\n";
  res+="#include \"GameApi_h.hh\"\n";
  res+="#include \"GameApi.hh\"\n";
  res+="using namespace GameApi;\n";
  std::string res2 = output_contents(vec);
  std::ofstream ss("output.cpp");

  std::string last_line = get_last_line(contents);
  std::string label = find_label(last_line);
  std::cout << "Label: " << label << std::endl;

  std::string res3 = "int main() {\n";

  res3+="#include \"Common.cc\"\n";
  res3+=remove_chars(contents,'\r')+"\n";
  res3+="RUN r=" + label + ";\n";
  res3+="#include \"Common2.cc\"\n";
  res3+="}\n";
  if (!use_existing) { 
    std::ofstream ss("contents2.txt");
    ss << res3;
    ss.close();
  } else {
    res3="";
    std::ifstream ss("contents2.txt");
    std::string s;
    while(std::getline(ss,s)) { res3+=s + "\n"; }
  }
  std::string res4;
  res4 += "#include \"Common2.hh\"\n";

  std::string res5;
  res5 += "#include \"Common3.cc\"\n";

  ss << res << res2 << res3 << res4 << res5 << std::endl;
  ss.close();
}
// todo std::string => quotes
// todo hex numbers in colours to have 0x
// todo [P] => std::vector<P>
