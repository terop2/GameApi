#include "GameApi.hh"

using namespace GameApi;

int main(int argc, char *argv[])
{
  if (argc!=2) { std::cout << "Usage: " << argv[0] << " filename.obj" << std::endl; return 0; }
  std::string arg = argv[1];
  std::string argpart = arg.substr(arg.size()-4);
  std::cout << "SubPart:" << argpart;
  if (argpart!=".obj") {
    std::cout << "file extension must be .obj" << std::endl;
    return 0;
  }
  
  std::string out_filename = arg.substr(0,arg.size()-4);
  out_filename+=".ds";

  std::cout << "Writing " << out_filename << std::endl;

  Env e;
  EveryApi ev(e);

  P p = ev.polygon_api.load_model_all(arg, 3);
  DS ds = ev.polygon_api.p_ds_inv(p);
  ev.mainloop_api.save_ds(out_filename, ds);
}
