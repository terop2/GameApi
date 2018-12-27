#include <string>
#include "GameApi_h.hh"

extern int score;
extern int hidden_score;
extern int async_pending_count;
extern int async_pending_count_previous;
extern std::string gameapi_homepageurl;
extern std::string gameapi_seamless_url;
extern int async_is_done;
extern Low_SDL_Window *sdl_window;
void P_cb(void *data);

void combparse(std::string comb, std::vector<std::string> &vec)
{
  std::string s;
  int ss = comb.size();
  for(int i=0;i<ss;i++)
    {
      char c = comb[i];
      if (c==':')
	{
	  vec.push_back(s);
	  s="";
	}
      else
	{
	  s+=c;
	}
    }
  if (s!="")
    {
      vec.push_back(s);
    }
}


std::vector<std::string> parse_sep(std::string s, char sep)
{
  int ss = s.size();
  int pos = 0;
  std::vector<std::string> vec;
  for(int i=0;i<ss;i++)
    {
      if (s[i]==sep) {
	std::string beg = s.substr(pos, i-pos);
	vec.push_back(beg);
	pos = i+1;
      }
    }
  std::string beg = s.substr(pos,s.size()-pos);
  vec.push_back(beg);
  //std::cout << "Parse sep: ";
  //int s2 = vec.size();
  //for(int i=0;i<s2;i++) std::cout << vec[i] << "::";
  //std::cout << std::endl;
  return vec;
}


int find_str(std::string val, std::string repl)
{
  std::size_t pos = val.find(repl);
  if (pos==std::string::npos) return -1;
  return pos;
}

std::string replace_str(std::string val, std::string repl, std::string subst)
{
  std::string s = "";
  int p = 0;
  while(1) {
    int pos = find_str(val,repl);
    if (pos==-1) { s+=val.substr(p,val.size()-p); return s; }
    s+=val.substr(p,pos-p);
    s+=subst;
    val[pos]='X'; // remove %N
    p=pos+repl.size();
  }

  while(1) {
  int pos = find_str(val, repl);
  if (pos==-1) return val;
  val.replace(pos,pos+repl.size()-1,subst);
  }
  return "ERROR";
}


struct Envi_2 {
  GameApi::EveryApi *ev;
  GameApi::MN move;
  GameApi::ML mainloop;
  float pos_x=0.0, pos_y=0.0;
  float rot_y=0.0;
  float speed = 10.0;
  float rot_speed = 1.0*3.14159*2.0/360.0;
  float speed_x = 1.0;
  float speed_y = 1.0;
  GameApi::InteractionApi::Quake_data data;
  bool logo_shown = true;
  bool fpscounter = false;
  float start_time = 0.0f;
  GameApi::SH color_sh;
  GameApi::SH texture_sh;
  GameApi::SH arr_texture_sh;
  bool exit = false;
  float timeout = 100000.0f;
  int screen_width=800;
  int screen_height=600;
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
  virtual GameApi::ML mat2_inst2(GameApi::P p, GameApi::PTA pta) const=0;
  virtual GameApi::ML mat_inst_fade(GameApi::P p, GameApi::PTS pts, bool flip, float start_time, float end_time) const=0;
};

int frame_count=0;
struct LogoEnv
{
  GameApi::EveryApi *ev;
  GameApi::ML res;
  GameApi::SH color;
  GameApi::SH texture;
  GameApi::SH texture_2d;
  GameApi::SH arr;
};
LogoEnv *logo_env = 0;

std::vector<std::pair<std::string,int> > bitmap_prepare_cache_data;
int bitmap_find_data(std::string data) {
  int s = bitmap_prepare_cache_data.size();
  for(int i=0;i<s;i++)
    {
      std::pair<std::string,int> p = bitmap_prepare_cache_data[i];
      if (p.first==data) return p.second;
    }
  return -1;
}
