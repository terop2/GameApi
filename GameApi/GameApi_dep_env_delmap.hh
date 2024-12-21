#include "GameApi_h.hh"
#include "GraphI.hh"

class FetchInBlocks;

  struct VECENTRY {
    std::string first;
    const std::vector<unsigned char, GameApiAllocator<unsigned char> > * second;
  };
#ifdef EMSCRIPTEN
  struct VECENTRY2
  {
    std::string first;
    FetchInBlocks* second;
  };
#endif

extern del_map_interface &g_del_map;
