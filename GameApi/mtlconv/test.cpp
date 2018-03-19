#include "GameApi.hh"
#include <sstream>
#include <fstream>

using namespace GameApi;

int main(int argc, char *argv[])
{
  if (!(argc==5)) { std::cout << "Usage: " << argv[0] << " filename.mtl http://tpgames.org/sponza.obj http://tpgames.org/ 10" << std::endl; return 0; }
  std::string arg = argv[1];
  std::string argpart = arg.substr(arg.size()-4);
  std::cout << "SubPart:" << argpart;
  if (argpart!=".mtl") {
    std::cout << "file extension must be .mtl" << std::endl;
    return 0;
  }
  std::string url = argv[2];
  std::string url2 = argv[3];
  int num = 10;
  std::string num2 = argv[4]; 
  std::stringstream ss2(num2); 
  ss2 >> num;
 
  
  std::string out_filename = arg.substr(0,arg.size()-4);
  out_filename+=".ml";

  std::cout << "Writing " << out_filename << std::endl;

  Env e;
  EveryApi ev(e);

  std::vector<GameApi::MaterialDef> v = ev.polygon_api.parse_mtl(arg);
  std::string ml = ev.polygon_api.output_ml(url, num, url2, v);
  std::ofstream ss(out_filename.c_str());
  ss << ml;
}
