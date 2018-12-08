
#include <iostream>
#include <sstream>
#include <fstream>

std::string find_class(std::string content, std::string classname)
{
  std::stringstream ss(content);
  std::string line;
  bool take=false;
  std::string res;
  while(std::getline(ss,line)) {
    std::stringstream ss2(line);
    std::string classid;
    std::string classname2;
    ss2 >> classid >> classname2;
    if (classid=="class" && classname2==classname)
      {
	take=true;
      }
    if (take) { res+=line+"\n"; }
    if (classid=="};") { take=false; }
  }
  return res;
}

std::string find_funcname(std::string content, std::string funcname)
{
  std::stringstream ss(content);
  std::string line;
  std::string res;
  while(std::getline(ss,line)) {
    int s = line.size();
    int s2 = funcname.size();
    for(int i=0;i<s-s2;i++)
      {
	int j=0;
	for(;j<s2;j++)
	  {
	    if (line[i+j]!=funcname[j]) break;
	  }
	if (j==s2) {
	  res+= line + "\n";
	}
      }
  }
  return res;
}

std::string pairs[] = { 
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


int main(int argc, char *argv[])
{
  if (argc!=2 && argc!=3) { std::cout << "./a.out classname [funcname]" << std::endl; exit(0); }
  std::string classname = argv[1];
  std::string funcname;
  if (argc==3) funcname=argv[2];

  int s = sizeof(pairs)/sizeof(std::string);
  for(int i=0;i<s;i+=2)
    {
      std::string p1 = pairs[i];
      std::string p2 = pairs[i+1];
      if (classname==p2) classname=p1;
    }

  std::ifstream ss("..\\GameApi.hh");
  char c;
  std::string file;
  std::string line;
  while(std::getline(ss,line)) { file+=line+"\n"; }
  //std::cout << file << std::endl;
  std::string classspec = find_class(file, classname);
  if (argc==2)
    std::cout << classspec << std::endl;

  if (argc==3)
    {
      std::string func = find_funcname(classspec," " + funcname + "(");
      std::cout << func << std::endl;
    }
}
