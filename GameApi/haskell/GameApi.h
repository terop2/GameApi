
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
struct EX { int id; };
struct PH { int id; };
struct TS { int id; };
struct CT { int id; };
struct CC { int id; };
struct c_BitmapApi { void * bmApi; };
struct c_EveryApi  { void * everyApi; };
struct c_MainloopApi { void *mainLoopApi; };
struct c_Env { void *env; };

#ifdef __cplusplus
extern "C" {
#endif

  c_Env *createEnv();
  c_EveryApi *geteveryApi(c_Env *api);
  c_BitmapApi *getbitmapApi(c_EveryApi *api);
  int newbitmap(c_BitmapApi *env, int sx, int sy, unsigned int color);
  int loadbitmap(c_BitmapApi *env, const char *filename);
  c_MainloopApi *getmainloopapi(c_EveryApi *api);
  void init_window(c_MainloopApi *api, int wid,int hei);

#ifdef __cplusplus
}
#endif
