#ifndef __clang__
#define IMPORT 
#define EXPORT
#else
#ifdef GAMEAPI_COMPILATION
#define IMPORT __declspec(dllexport)
#define EXPORT __declspec(dllexport)
#else
#define IMPORT
#define EXPORT
#endif
#endif
#include <map>

#include "../Buffer.hh"
#include "../EffectI.hh"
#include "../GraphI.hh"

namespace GameApi {

  struct H { int id; };
  struct FBU { int id; };
  struct FML { int id; };
  struct CG { int id; };
  struct INP { int id; };
  struct IBM { int id; };
  struct DS { int id; };
  struct SBM { int id; };
  struct PN { int id; };
  struct DC { int id; };
  struct CBB { int id; };
  struct CFB { int id; };
  struct SI { int id; };
  struct PLP { int id; };
  struct PLL { int id; };
  struct PLF { int id; };
  struct PR { int id; };
  struct CMD { int id; };
  struct FI { int id; };
  struct SD { int id; };
  struct GI { int id; };
  struct FF { int id; };
  struct IF { int id; };
  struct PF { int id; };
  struct SF { int id; };
  struct ARR { int id; };
  struct PAR { int id; }; // P array
  struct CPP { int id; };
  struct PTT { int id; }; 
  struct KF { int id; };
  struct BLK { int id; };
  struct RUN { int id; };
  struct EV { int id; };
  struct AC { int id; };
  struct MX { int id; };
  struct Pa { int id; };
  struct Va { int id; };
  struct AS { int id; };
  struct MC { int id; };
  struct MS { int id; };
  struct US { int id; };
  struct MT { int id; };
  struct TL { int id; };
  //struct T { int id; };
  struct MN { int id; };
  struct CP { int id; };
  struct DR { int id; };
  struct OM { int id; };
  struct FO { int id; };
  struct WV { int id; }; // waveform
  struct BM { int id; }; // bitmap
  struct BMA { int id; }; // bitmap array
  struct VBM { int id; }; // vector bitmap
  struct BB { int id; }; // bool bitmap
  struct FB { int id; }; // float bitmap
  struct CBM { int id; }; // continuousbitmap
  struct SP { int id; }; // space
  struct SA { int id; }; // separate
  struct PT { int id; }; // point
  struct V { int id; }; // vector
  struct M { float mat[16]; }; // matrix
  struct LN { int id; }; // timeline
  struct RM { int id; }; // room
  struct IS { int id; }; // timed int sequence
  struct O { int id; }; // 3d object via volumes
  struct P { int id; }; // 3d object via polygons
  struct OO { int id; };
  struct PP { int id; };
  struct PA { int id; }; // path in 3d space
  struct LA { int id; }; // loop in 3d space
  struct TN { int id; }; // tree node
  struct SH { int id; }; // shader
  struct C { int id; }; // curve
  struct CO { int id; }; // color
  struct F { int id; }; // function or float
  struct FA { int id; }; // float array
  struct Ft { int id; }; // font
  struct VF { int id; }; // vectorfield
  struct PPT { int id; }; // physics point
  struct PSP { int id; }; // physics space
  struct MA { int id; };
  //struct M { int id; };
  struct S { int id; }; // surface
  struct Vb { int id; }; // vbo object
  //struct L { int id; }; // lighting
  struct T { int id; }; // time
  struct E { int id; }; // event
  struct L { int id; }; // link
  struct LAY { int id; }; // layout
  struct MP { int id; }; // mouse plane
  struct MV { int id; }; // movement
  struct LL { int id; }; // linkage
  struct ME { int id; }; // memory
  struct ST { int id; }; // state
  struct TY { int id; }; // type
  struct PS { int id; }; // powerset
  struct ID { int id; }; // id
  struct Str { int id; }; // string
  struct Fi { int id; }; // file
  struct Op { int id; }; // file operation
  struct D { int id; }; // directory
  struct Ht { int id; }; // html
  struct BF { int id; };
  struct VA { int id; }; // vertex array
  struct VAA { int id; }; // vertex array array
  struct VX { int id; }; // voxel
  struct PL { int id; }; // plane
  struct PLA { int id; }; // planearray
  struct TX { int id; }; // texture
  struct TXID { int id; }; // texture id
  struct TXA { int id; }; // texture array id
  struct TR { int id; }; // time range
  struct VV { int id; }; // time range vertex array
  struct Q { int id; }; // quad texture coordinates
  struct DO { int id; };
  struct PC { int id; };
  struct SV { int id; };
  struct FOA { int id; };
  struct COV { int id; };
  struct LI { int id; };
  struct LLA { int id; };
  struct FD { int id; };
  struct VO { int id; };
  struct PTS { int id; };
  struct PTA { int id; };
  struct RD { int id; };
  struct FBO { int id; };
  struct SM { int id; };
  struct TRK { int id; };
  struct WAV { int id; };
  struct TBUF { int id; };
  struct SFO { int id; };
  struct W { int id; };
  struct WM { int id; };
  struct FtA { int id; };
  struct ML { int id; };
  template<class T>
  struct A { int id; };
  struct EX { int id; };
  struct PH { int id; };
  struct TS { int id; };
  struct CT { int id; };
  struct CC { int id; };
  struct IM { int id; };
  //template<class T>
  //struct E { int id; };
  //struct A { int id; };


  class ShaderApi;
class Env
{
public:
  IMPORT Env();
  IMPORT void free_memory();
  IMPORT std::vector<int> store_counts();
  IMPORT void free_to_counts(std::vector<int> vec);
  IMPORT void free_temp_memory();
  IMPORT void async_load_url(std::string url, std::string homepage);
  IMPORT void async_load_callback(std::string url, void (*fptr)(void*), void *data);
  IMPORT std::vector<unsigned char> *get_loaded_async_url(std::string url);
  IMPORT ~Env();
  IMPORT static Env *Latest_Env();
private:
  Env(const Env &);
  void operator=(const Env &);
public:
  void *envimpl;
  friend struct EnvImpl;
};



} // namespace GameApi
