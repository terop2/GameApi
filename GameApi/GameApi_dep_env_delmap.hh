#include "GameApi_h.hh"
#include "GraphI.hh"

class FetchInBlocks;

  struct VECENTRY {
    std::string first;
    const std::vector<unsigned char, GameApiAllocator<unsigned char> > * second;

    VECENTRY() : first(""), second(0) { }
    VECENTRY(const VECENTRY &a)
      : first(a.first),
	second(a.second) { }
    VECENTRY &operator=(const VECENTRY &a)
    {
      first = a.first;
      second = a.second;
      return *this;
    }

  };
#ifdef EMSCRIPTEN
  struct VECENTRY2
  {
    std::string first;
    FetchInBlocks* second;
  };
#endif

extern del_map_interface &g_del_map;



struct TASKENTRY
{
};
