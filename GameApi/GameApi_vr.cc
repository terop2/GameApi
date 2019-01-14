/*
*/

#include "GameApi_h.hh"

//#include <GL/glew.h>
//#include <SDL_opengl.h>
#include "GameApi_low.hh"

#ifndef ARM

#ifdef VIRTUAL_REALITY
#ifndef EMSCRIPTEN

#ifdef WINDOWS
#include "openvr/openvr_mingw.h"
#else
#ifdef LINUX
#include "openvr/openvr_mingw.h"
#else
#include "openvr/openvr.h"
#endif
#endif

#else
#include <emscripten.h>
#include <emscripten/vr.h>
#include <emscripten/html5.h>
#endif

bool g_use_vr = false;

EXPORT GameApi::RUN GameApi::BlockerApi::vr_window(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration, bool invert, bool translate)
{
  g_use_vr = true;

  int screen_w = ev.mainloop_api.get_screen_width();
  int screen_h = ev.mainloop_api.get_screen_height();

  int fbo_x = 1080;
  int fbo_y = 1200;
  fbo_x = screen_w;
  fbo_y = screen_h;


  screen_w-=100;
  screen_h-=100;
  screen_w-=10;
  screen_h-=10;


  // hmd=false
  ML I43 = ev.mainloop_api.setup_hmd_projection(ev,ml,false,false,10.1,60000.0,false);
  TXID I44 = ev.fbo_api.fbo_ml(ev,I43,fbo_x,fbo_y,false);
  ML I66 = ev.mainloop_api.setup_hmd_projection(ev,ml,true, false,10.1,60000.0, false);
  TXID I67 = ev.fbo_api.fbo_ml(ev,I66,fbo_x,fbo_y,false);

  ML I43a = ev.mainloop_api.setup_hmd_projection(ev,ml,false,false,10.1,60000.0,false);
  TXID I44a = ev.fbo_api.fbo_ml(ev,I43a,fbo_x,fbo_y,false);
  ML I66a = ev.mainloop_api.setup_hmd_projection(ev,ml,true,false,10.1,60000.0, false);
  TXID I67a = ev.fbo_api.fbo_ml(ev,I66a,fbo_x,fbo_y,false);
  ML res = ev.blocker_api.vr_submit_ml(ev,ml, I44,I67,invert,translate);
  ML I70a = ev.blocker_api.vr_submit(ev, I44a, I67a);
  ML res_I70a = ev.blocker_api.vr_submit_ml(ev,I70a, I44,I67,invert,translate);
  // hmd=false outputs: res, res_I170a

  //IF alt = ev.font_api.toggle_button_fetcher(screen_w,screen_w+100,screen_h,screen_h+100);
  // ML cho = ev.font_api.ml_chooser(std::vector<ML>{res,I70a},alt);

  // hmd=true
  ML I43_b = ev.mainloop_api.setup_hmd_projection(ev,ml,false,false,10.1,60000.0,false);
  TXID I44_b = ev.fbo_api.fbo_ml(ev,I43_b,fbo_x,fbo_y,false);
  ML I66_b = ev.mainloop_api.setup_hmd_projection(ev,ml,true, false,10.1,60000.0, false);
  TXID I67_b = ev.fbo_api.fbo_ml(ev,I66_b,fbo_x,fbo_y,false);

  //ML I43a_b = ev.mainloop_api.setup_hmd_projection(ev,ml,false,false,10.1,60000.0,false);
  TXID I44a_b = ev.fbo_api.fbo_ml(ev,ml/*I43a_b*/,fbo_x,fbo_y,false);
  // ML I66a_b = ev.mainloop_api.setup_hmd_projection(ev,ml,true,false,10.1,60000.0, false);
  TXID I67a_b = ev.fbo_api.fbo_ml(ev,ml/*I66a_b*/,fbo_x,fbo_y,false);
  ML res_b = ev.blocker_api.vr_submit_ml(ev,ml, I44_b,I67_b,invert,translate);
  ML I70a_b = ev.blocker_api.vr_submit(ev, I44a_b, I67a_b);
  ML res_I70a_b = ev.blocker_api.vr_submit_ml(ev,I70a_b, I44_b,I67_b,invert,translate);

  // hmd=true outputs res_b, res_I70a_b

  IF alt2 = ev.font_api.hmd_state_fetcher();
  ML cho2 = ev.font_api.ml_chooser(std::vector<ML>{res, res_b}, alt2);

  IF alt3 = ev.font_api.hmd_state_fetcher();
  ML cho3 = ev.font_api.ml_chooser(std::vector<ML>{res_I70a, res_I70a_b}, alt3);

  
  IF alt_b = ev.font_api.toggle_button_fetcher(screen_w,screen_w+100,screen_h,screen_h+100);
  IF alt_c = ev.font_api.hmd_request_presenting(alt_b);
  ML cho_b = ev.font_api.ml_chooser(std::vector<ML>{cho2,cho3},alt_c);
 
  

BB I1=ev.bool_bitmap_api.empty(100,100);
BB I2=ev.bool_bitmap_api.rectangle(I1,0,0,2,100);
BB I3=ev.bool_bitmap_api.rectangle(I2,0,0,100,2);
BB I4=ev.bool_bitmap_api.rectangle(I3,98,0,2,100);
BB I5=ev.bool_bitmap_api.rectangle(I4,0,98,100,2);
BB I6=ev.bool_bitmap_api.empty(100,100);
BB I7=ev.bool_bitmap_api.tri(I6,23,20,36,60,10,40);
BB I8=ev.bool_bitmap_api.empty(100,100);
BB I9=ev.bool_bitmap_api.tri(I8,23,20,49,40,36,60);
BB I10=ev.bool_bitmap_api.or_bitmap(I7,I9);
BB I11=ev.bool_bitmap_api.empty(100,100);
BB I12=ev.bool_bitmap_api.tri(I11,75,20,62,60,49,40);
BB I13=ev.bool_bitmap_api.empty(100,100);
BB I14=ev.bool_bitmap_api.tri(I13,75,20,88,40,61,60);
BB I15=ev.bool_bitmap_api.or_bitmap(I12,I14);
BB I16=ev.bool_bitmap_api.or_bitmap(I10,I15);
BB I17=ev.bool_bitmap_api.empty(100,100);
BB I18=ev.bool_bitmap_api.tri(I17,23,20,76,20,49,41);
BB I19=ev.bool_bitmap_api.or_bitmap(I16,I18);
BB I20=ev.bool_bitmap_api.sprite(I5,I19,0,10,1,1);
BM I21=ev.bool_bitmap_api.to_bitmap(I20,255,255,255,255,0,0,0,0);
ML I22=ev.sprite_api.vertex_array_render(ev,I21);
MN I23=ev.move_api.empty();
MN I24=ev.move_api.trans2(I23,screen_w,screen_h,0);
ML I25=ev.move_api.move_ml(ev,I22,I24,1,10.0);
ML I26=ev.sprite_api.turn_to_2d(ev,I25,0.0,0.0,800.0,600.0);

 ML arr = ev.mainloop_api.array_ml(std::vector<ML>{cho_b,I26});

  //#ifdef EMSCRIPTEN  
  //std::vector<ML> vec = std::vector<ML>{I70,res};
  // res = ev.mainloop_api.array_ml(vec);
  //#endif
  RUN I69 = ev.blocker_api.game_window2(ev,arr,logo,fpscounter,start_time,duration);

  return I69;
}
#ifndef EMSCRIPTEN
vr::IVRSystem *hmd = 0;
#endif
Matrix hmd_pose = Matrix::Identity();
Matrix hmd_left_view_matrix = Matrix::Identity();
Matrix hmd_right_view_matrix = Matrix::Identity();
bool vr_pose_not_active=false;
#ifdef EMSCRIPTEN
VRDisplayHandle current_display= 0;
bool vr_vr_ready = false;
void vr_cb(void *arg)
{
  std::cout << "OK: VR ready! (now it's possible to call other functions)" << std::endl;
  vr_vr_ready = true;
}
#endif
EXPORT void check_vr_compositor_init()
{
#ifndef EMSCRIPTEN
  if (!vr::VRCompositor()) {
    vr::EVRInitError eError = vr::VRInitError_None;
    hmd = vr::VR_Init( &eError, vr::VRApplication_Scene );
  }
  if (!vr::VRCompositor()) {
    std::cout << "ERROR: VR compositor initialization failed!" << std::endl;
  }
  if (hmd && vr::VRCompositor()) {
    vr::TrackedDevicePose_t pose[ vr::k_unMaxTrackedDeviceCount ];
    vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL,0);
  }
#else
  if (!vr_vr_ready) {
    int val = emscripten_vr_init(&vr_cb,NULL);
    if (val==0) { std::cout << "Emscripten VR not found!" << std::endl; }
  }
  
  //EmscriptenWebGLContextAttributes attr;
  //emscripten_webgl_init_context_attributes(&attr);
  //attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preferLowPowerToHighPerformance = attr.failIfMajorPerformanceCaveat = 0;
  //attr.preserveDrawingBuffer = 1;
  //attr.enableExtensionsByDefault = 1;
  //attr.premultipliedAlpha = 0;
  //attr.majorVersion = 1;
  //attr.minorVersion = 0;
  //EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(0, &attr);
  //emscripten_webgl_make_context_current(ctx);
  

#endif
}
#ifdef EMSCRIPTEN
VRFrameData g_d2;
#endif

class SubmitML : public MainLoopItem
{
public:
  SubmitML(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *item, TextureID *left, TextureID *right, bool invert, bool translate) :  env(env),ev(ev),item(item), left(left), right(right), invert(invert), translate(translate) {
    firsttime = true;
  }
  virtual void execute(MainLoopEnv &e)
  {
    //if (firsttime) {
    //  check_vr_compositor_init();
    //  firsttime = false;
    //}
    vr_pose_not_active=true;
    item->execute(e);
    vr_pose_not_active=false;
#ifndef EMSCRIPTEN
    if (hmd) {
    left->render(e);
    right->render(e);

    int texid_left = left->texture();
    int texid_right = right->texture();
    vr::Texture_t leftTexture = { (void*)(uintptr_t)texid_left, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftTexture);

    vr::Texture_t rightTexture = { (void*)(uintptr_t)texid_right, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightTexture);
    vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL,0);
    int s = vr::k_unMaxTrackedDeviceCount;
    for(int nDevice = 0; nDevice < s; ++nDevice) 
      {
	if (pose[nDevice].bPoseIsValid ) {
	  device_pose[nDevice] = ConvertMatrix( pose[nDevice].mDeviceToAbsoluteTracking );
	  
	}
      }
    if (pose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid ) {
      hmd_pose = device_pose[vr::k_unTrackedDeviceIndex_Hmd];
      if (invert) {
	hmd_pose = Matrix::Inverse(hmd_pose);
      }
      if (translate) {
	hmd_pose = Matrix::Translate(0.0,0.0,-1200.0) * hmd_pose;
      }
    }
    }
#else
    if (vr_vr_ready && current_display != 0 && current_display!=-1) {
      // This one gets poses, and is important or else the display doesnt 
      // render.
      if (!emscripten_vr_display_presenting(current_display)) {
	//std::cout << "FAIL: expected display is not presenting." << std::endl;
      }


      int val4 = emscripten_vr_get_frame_data( current_display, &g_d2);
      if (!val4) { std::cout << "FAIL: get_frame_data failed!" << std::endl; }

      ev.mainloop_api.clear_3d(0xff000000);
      left->render(e);
      right->render(e);
      static int ii=0;
      if (ii==0) { ii++;
	//std::cout << "vr submit_frame" << std::endl;
      }
      if (firsttime) {
	ml = ev.blocker_api.vr_submit(ev,add_txid(env,left),add_txid(env,right));
	firsttime = false;
      }
      MainLoopItem *item_n = find_main_loop(env, ml);
      item_n->execute(e);

      
      int val3 = emscripten_vr_submit_frame(current_display);
      if (!val3) { std::cout << "FAIL: submit_frame failed!" << std::endl; }


      
      //VRDisplayCapabilities cap;
      //int emscripen_vr_get_eye_parameters( display, &cap );
      
      VRFrameData d = g_d2;
      //int val = emscripten_vr_get_frame_data( current_display, &d);
      if ((d.pose.poseFlags & VR_POSE_ORIENTATION) != 0) {
	VRQuaternion q = d.pose.orientation;
	Quarternion q2;
	q2.x = q.x; q2.y = q.y; q2.z = q.z; q2.w = q.w;
	static int i = 0;
	if (i==0) { i++; 
	  //std::cout << "quarternion: " << q.x << " " << q.y << " " << q.z << " " << q.w << std::endl;
	}
	hmd_pose = Quarternion::QuarToMatrix(q2);
	if (invert) {
	  hmd_pose = Matrix::Inverse(hmd_pose);
	}
	if (translate) {
	  hmd_pose = Matrix::Translate(0.0,0.0,-1200.0) * hmd_pose;
	}

      }
    }
#endif
  }
#ifndef EMSCRIPTEN
  Matrix ConvertMatrix( const vr::HmdMatrix34_t &pose)
  {
    // 0.0 vaikuttaa sponzan 2 kerroksen pilareihin
    Matrix m = { pose.m[0][0], pose.m[0][1], pose.m[0][2], /*0.0*/ pose.m[0][3],
		 pose.m[1][0], pose.m[1][1], pose.m[1][2], /*0.0*/ pose.m[1][3],
		 pose.m[2][0], pose.m[2][1], pose.m[2][2], /*0.0*/ pose.m[2][3],
	         0.0, 0.0,0.0, 1.0f, false };
    for(int i=0;i<16;i++) if (std::isnan(m.matrix[i])) m.matrix[i]=0.0;
    return m;
  }
#endif
  virtual void handle_event(MainLoopEvent &e)
  {
    left->handle_event(e);
    right->handle_event(e);
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  bool firsttime;
  MainLoopItem *item;
  TextureID *left;
  TextureID *right;
#ifndef EMSCRIPTEN
  vr::TrackedDevicePose_t pose[ vr::k_unMaxTrackedDeviceCount ];
  Matrix device_pose[ vr::k_unMaxTrackedDeviceCount ];
#endif
  bool invert;
  bool translate;
  GameApi::ML ml;
};
EXPORT GameApi::ML GameApi::BlockerApi::vr_submit_ml(EveryApi &ev, ML ml, TXID left, TXID right, bool invert, bool translate)
{
  MainLoopItem *item = find_main_loop(e, ml);
  TextureID *left_eye = find_txid(e, left);
  TextureID *right_eye = find_txid(e, right);
  return add_main_loop(e, new SubmitML(e,ev,item, left_eye, right_eye, invert, translate));
}
EXPORT GameApi::ML GameApi::BlockerApi::vr_submit(EveryApi &ev, TXID left, TXID right)
{

P I1=ev.polygon_api.vr_fullscreen_quad(ev,false);
MT I2=ev.materials_api.textureid(ev,left,1.0);
ML I3=ev.materials_api.bind(I1,I2);
P I4=ev.polygon_api.vr_fullscreen_quad(ev,true);
 MT I5=ev.materials_api.textureid(ev,right,1.0);
ML I6=ev.materials_api.bind(I4,I5);
ML I7=ev.mainloop_api.array_ml(std::vector<ML>{I3,I6});
//---
#if 0
 MN I1a=ev.move_api.empty();
MN I2a=ev.move_api.trans2(I1a,0,0,-1200);
ML I3a=ev.move_api.move_ml(ev,I7,I2a,1,10.0);
//---
 IF alt2 = ev.font_api.hmd_state_fetcher();
 ML cho2 = ev.font_api.ml_chooser(std::vector<ML>{I7, I3a}, alt2);
#endif
 //ML cho2_2d = 
 ML cho3=ev.sprite_api.turn_to_2d(ev,I7,0.0,0.0,1080.0,1200.0); // 800,600
 return cho3;
 //RUN I8=ev.blocker_api.game_window2(ev,I7,false,false,0.0,100000.0);
 //return I8;
}

class PoseMovement : public Movement
{
public:
  PoseMovement(Movement *next, bool pose_in_screen) : next(next), pose_in_screen(pose_in_screen) { }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  virtual void draw_frame(DrawLoopEnv &e) { if (next) next->draw_frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { if (next) next->draw_event(e); }
  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    if (pose_in_screen) {
      return next->get_whole_matrix(time,delta_time)*hmd_pose;
    } else {
    if (vr_pose_not_active)
      return next->get_whole_matrix(time,delta_time);
    return next->get_whole_matrix(time,delta_time)*hmd_pose;
    }
  }
private:
  Movement *next;
  bool eye;
  bool pose_in_screen;
};
EXPORT GameApi::MN GameApi::MovementNode::pose(MN next, bool pose_in_screen)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new PoseMovement(nxt,pose_in_screen));
}

class HMDProjection : public MainLoopItem
{
public:
  HMDProjection(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *item, bool eye, bool standard, float nr, float fr, bool translate) : env(env), ev(ev), item(item), eye(eye), is_standard(standard), m_fNearClip(nr), m_fFarClip(fr),  translate(translate) {}


  Matrix GetHMDMatrixPoseEye( bool eye, bool is_std )
{
#ifndef EMSCRIPTEN
  if (!hmd || is_std) { return Matrix::Identity(); }
  vr::Hmd_Eye nEye = eye ? vr::Eye_Left : vr::Eye_Right;
  vr::HmdMatrix34_t mat = hmd->GetEyeToHeadTransform( nEye );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0, false };

#else
  Matrix m = Matrix::Identity();
  if ((vr_vr_ready && current_display != 0 && current_display!=-1) && !is_std) {
    VRFrameData d = g_d2;
    int val = 1; //emscripten_vr_get_frame_data(current_display, &d);
    if (!val) { std::cout << "vr_get_frame_data invalid handle" << std::endl; }
      if (!eye) {
	for(int j=0;j<4;j++)
	for(int i=0;i<4;i++) m.matrix[i+j*4] = d.leftViewMatrix[j+i*4];
      } else {
	for(int j=0;j<4;j++)
	for(int i=0;i<4;i++) m.matrix[i+j*4] = d.rightViewMatrix[j+i*4];
      }
      m.is_identity = false;
  }
#endif
  for(int i=0;i<16;i++) if (std::isnan(m.matrix[i])) m.matrix[i]=0.0;

  //std::cout << "Pose:" << std::endl;
  //for(int i=0;i<16;i++)
  //  std::cout << m.matrix[i] << ",";
  //std::cout << std::endl;
  //m = Matrix::Inverse(m);

  return m;
}

  //int which_display()
  //{
    // 0 = standard screen
    // 1 = double standard screen
    // 2 = left eye
    // 3 = right eye

  //}
  
  Matrix DefaultProjection(GameApi::EveryApi &ev)
  {
    GameApi::M m = ev.mainloop_api.in_P(ev,true);
    GameApi::M m2 = ev.matrix_api.scale(1.0,0.5,1.0);
    GameApi::M m3 = ev.matrix_api.mult(m2,m);
    Matrix mm = find_matrix(env, m3); // m3
    return mm;
  }
  Matrix DefaultSceneTranslate(GameApi::EveryApi &ev)
  {
    return Matrix::Identity();
    GameApi::M m = ev.matrix_api.trans(0.0,0.0,-500.0);
    Matrix mm = find_matrix(env,m);
    return mm;
  }

  Matrix GetHMDMatrixSceneTranslateEye( bool eye, bool is_std) {
#ifndef EMSCRIPTEN
  if (!hmd ||is_std) { return DefaultSceneTranslate(ev); }
  return Matrix::Identity();
#else
  if ((!vr_vr_ready ||current_display==NULL||current_display==-1) ||is_std) { return DefaultSceneTranslate(ev); }
  return Matrix::Identity();
#endif
  }
  Matrix GetHMDMatrixProjectionEye( bool eye, bool is_std )
{
#ifndef EMSCRIPTEN
  if (!hmd) { return DefaultProjection(ev); }
  vr::Hmd_Eye nEye = eye ? vr::Eye_Left : vr::Eye_Right;
  vr::HmdMatrix44_t mat = hmd->GetProjectionMatrix( nEye, m_fNearClip, m_fFarClip );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3], false };


  GameApi::M m2 = ev.matrix_api.scale(1.0,0.5,1.0);
  Matrix mm = find_matrix(env,m2);
  m = mm * m;

#else
  if (!vr_vr_ready ||current_display==NULL||current_display==-1) { return DefaultProjection(ev); }
    VRFrameData d = g_d2;
    //VRFrameData d;
    int val = 1; //emscripten_vr_get_frame_data(current_display, &d);
  if (!val) { std::cout << "FAIL: vr_get_frame_data invalid handle" << std::endl; }
  Matrix m;
  if (!eye) {
    for(int j=0;j<4;j++)
      for(int i=0;i<4;i++) m.matrix[i+j*4] = d.leftProjectionMatrix[j+i*4];
  } else {
    for(int j=0;j<4;j++)
      for(int i=0;i<4;i++) m.matrix[i+j*4] = d.rightProjectionMatrix[j+i*4];
  }
  m.is_identity = false;
  for(int i=0;i<16;i++) if (std::isnan(m.matrix[i])) m.matrix[i]=0.0;

  GameApi::M m2 = ev.matrix_api.scale(1.0,0.5,1.0);
  Matrix mm = find_matrix(env,m2);
  m = mm * m;

#endif

  
  //m.matrix[10]=0.0f;
  //m.matrix[11]=0.0f;
  //std::cout << "Proj:" << std::endl;
  //for(int i=0;i<16;i++)
  //  std::cout << m.matrix[i] << ",";
  //std::cout << std::endl;
  return m;
}

  virtual void execute(MainLoopEnv &e)
  {
    GameApi::SH sh_color, sh_texture, sh_texture_2d, sh_array_texture;
    GameApi::SH vertex, fragment;
    GameApi::SH sh_id;
    sh_color.id = e.sh_color;
    sh_texture.id = e.sh_texture;
    sh_texture_2d.id = e.sh_texture_2d;
    sh_array_texture.id = e.sh_array_texture;
    //sh_id.id = shader_id();
    vertex.id = e.us_vertex_shader;
    fragment.id = e.us_fragment_shader;

    Matrix proj_matrix; Matrix pos_mat; Matrix scene_translate = Matrix::Identity();
    proj_matrix=GetHMDMatrixProjectionEye( eye, is_standard );
    //std::cout << "Projection matrix: " << proj_matrix << std::endl;
    pos_mat= GetHMDMatrixPoseEye( eye, is_standard );
    Matrix proj_m = proj_matrix * scene_translate * pos_mat;
    GameApi::M proj = add_matrix2( env, proj_m );
    Matrix id_m = pos_mat; //GetHMDMatrixSceneTranslateEye(eye, is_standard);
    GameApi::M id = add_matrix2( env, id_m );
    //if (sh_id.id!=-1) {
    //  ev.shader_api.set_var(sh_id, "in_P", proj);
    //}
    ev.shader_api.set_var(sh_color, "in_P", proj);
    ev.shader_api.set_var(sh_texture, "in_P", proj);
    ev.shader_api.set_var(sh_texture_2d, "in_P", proj);
    ev.shader_api.set_var(sh_array_texture, "in_P", proj);
    ev.shader_api.set_var(vertex, "in_P", proj);
    ev.shader_api.set_var(fragment, "in_P", proj);

    //if (sh_id.id!=-1) {
    //  ev.shader_api.set_var(sh_id, "in_T", id);
    // }
    ev.shader_api.set_var(sh_color, "in_T", id);
    ev.shader_api.set_var(sh_texture, "in_T", id);
    ev.shader_api.set_var(sh_texture_2d, "in_T", id);
    ev.shader_api.set_var(sh_array_texture, "in_T", id);
    ev.shader_api.set_var(vertex, "in_T", id);
    ev.shader_api.set_var(fragment, "in_T", id);

    MainLoopEnv ee = e;
    Matrix old = e.in_T;
    ee.in_T = id_m;
    //ee.in_P = proj_m;
    g_low->ogl->glEnable( Low_GL_MULTISAMPLE );
    item->execute(ee);
    g_low->ogl->glDisable( Low_GL_MULTISAMPLE );

    GameApi::M old_m = add_matrix2(env, old);
    //if (sh_id.id != -1) {
    //  ev.shader_api.set_var(sh_id, "in_T", old_m);
    // }
    ev.shader_api.set_var(sh_color,"in_T", old_m);
    ev.shader_api.set_var(sh_texture,"in_T", old_m);
    ev.shader_api.set_var(sh_array_texture,"in_T", old_m);
    ev.shader_api.set_var(vertex,"in_T", old_m);
    ev.shader_api.set_var(fragment,"in_T", old_m);
    ev.shader_api.set_var(sh_color,"in_T", old_m);
  }
  virtual void handle_event(MainLoopEvent &e)
  {
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }

private:
  GameApi::Env &env;
  GameApi::EveryApi &ev;
  MainLoopItem *item;
  bool eye;
  bool is_standard;
  float m_fNearClip, m_fFarClip;
  bool translate;
};

EXPORT GameApi::ML GameApi::MainLoopApi::setup_hmd_projection(EveryApi &ev, ML ml, bool eye, bool is_standard, float nnear, float nfar, bool translate)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new HMDProjection(e,ev,item, eye, is_standard, nnear,nfar, translate));
}
#ifdef EMSCRIPTEN
void splitter_iter2(void *arg);
struct Splitter_arg
{
  VRDisplayHandle* display;
  Splitter *spl;
};

void choose_display()
{
}

int vr_activated=0;

extern "C" int activate_vr(int i)
{
    vr_activated=i;
    return 0;
}

void requestPresentCallback(void *arg) {
  Splitter_arg *a = (Splitter_arg*)arg;
  Splitter *spl = a->spl;
  if (emscripten_vr_display_presenting(current_display))
    {
      VREyeParameters left, right;
      emscripten_vr_get_eye_parameters(current_display, VREyeLeft, &left);
      emscripten_vr_get_eye_parameters(current_display, VREyeRight, &right);
      
      //emscripten_set_canvas_element_size("#canvas", left.renderWidth + right.renderWidth, left.renderHeight);
      static bool b = false;
      if (!b)
      if (!emscripten_vr_set_display_render_loop_arg(current_display, &splitter_iter2, (void*)spl))
	{
	  std::cout << "FAIL: set_display render loop failed (in request present)" << std::endl;
	} else {
	b=true;
	std::cout << "OK: set_display render loop success." << std::endl;
      }
    }

}
void onClick(void *data)
{
  if (emscripten_vr_display_presenting(current_display)) {
    emscripten_vr_exit_present(current_display);
    emscripten_vr_cancel_display_render_loop(current_display);
  } else {

  std::cout << "onclick trying request present!" << std::endl;
  std::cout << "TODO: canvas=NULL, should check if removing 2nd canvas from web page would fix it" << std::endl;
      VRLayerInit init = { NULL, VR_LAYER_DEFAULT_LEFT_BOUNDS, VR_LAYER_DEFAULT_RIGHT_BOUNDS };
    if (!emscripten_vr_request_present(current_display, &init, 1, requestPresentCallback, data)) {
      std::cout << "FAIL: request_present with default canvas failed." << std::endl;
      return;
    }
    std::cout << "OK: request_present succeeded on click!" << std::endl;
  }
}
int touchCallback(int eventType, const EmscriptenMouseEvent* e, void *data)
{
  if (!e || eventType!=EMSCRIPTEN_EVENT_TOUCHEND) return EM_FALSE;
  onClick(data);
  return EM_FALSE;
}
EM_BOOL clickCallback(int eventType, const EmscriptenMouseEvent* e, void *data)
{
  if (!e ||eventType!=EMSCRIPTEN_EVENT_CLICK) return EM_FALSE;
  long clientX = e->clientX;
  long clientY = e->clientY;
  onClick(data);
  return EM_FALSE;
}

int render_loop_called = 0;
void splitter_iter3(void *arg)
{
  Splitter_arg *a = (Splitter_arg*)arg;
  //current_display = a->display;
  Splitter *spl = a->spl;

  if (current_display==0) {
    //if (!emscripten_vr_ready()) { return; }
    if (!g_use_vr) { splitter_iter2((void*)spl); return; }
    if (!vr_vr_ready) { splitter_iter2((void*)spl); return; }
    
    int s = emscripten_vr_count_displays();
    if (s==0) { std::cout << "FAIL: No displays found" << std::endl; }
    std::cout << "OK: Found " << s << " displays" << std::endl;
    VRDisplayHandle *display = new VRDisplayHandle;
    for(int i=0;i<s;i++) {
      current_display = emscripten_vr_get_display_handle(i);
      const char *name = emscripten_vr_get_display_name( current_display );
      std::cout << "OK: Found display " << name << std::endl;
      VRDisplayCapabilities caps;
      if (!emscripten_vr_get_display_capabilities(current_display, &caps)) {
      	std::cout << "FAIL: Failed to get display capabilities" << std::endl;
      	continue;
      }
      if (!emscripten_vr_display_connected( current_display)) {
	std::cout << "FAIL: Display is not connected" << std::endl;
	continue;
      }
      
    }
    if (current_display==0) {
      std::cout << "FAIL: No display found!" << std::endl;
      current_display=-1;
      return;
    }


    emscripten_set_click_callback("#canvas", a, true, clickCallback);
    emscripten_set_touchend_callback("#canvas", a, true, (em_touch_callback_func)&touchCallback);
    
    VREyeParameters leftParam, rightParam;
    emscripten_vr_get_eye_parameters(current_display, VREyeLeft, &leftParam);
    emscripten_vr_get_eye_parameters(current_display, VREyeRight, &rightParam);
    //std::cout << "OK: Left eye: " << leftParam.offset.x << " " << leftParam.offset.y << " " << leftParam.offset.z << std::endl;
    //std::cout << "OK: Right eye: " << rightParam.offset.x << " " << rightParam.offset.y << " " << rightParam.offset.z << std::endl;

    //emscripten_vr_set_display_render_loop_arg(current_display, &splitter_iter2, (void*)spl);

  }

  render_loop_called++;
  if (render_loop_called==1 && current_display!=0 && current_display != -1) {
    VRLayerInit init = { NULL, VR_LAYER_DEFAULT_LEFT_BOUNDS, VR_LAYER_DEFAULT_RIGHT_BOUNDS };
    //if (!emscripten_vr_request_present(current_display, &init, 1, requestPresentCallback, NULL)) {
    //  std::cout << "FAIL: request_present with default canvas failed." << std::endl;
    //  return;
    // }
    // if (emscripten_vr_display_presenting(current_display)) {
    // std::cout << "FAIL: expected display is not presenting.\n" << std::endl;
    //return; }
  //VRFrameData data;
  //if (!emscripten_vr_get_frame_data(current_display, &data)) {
  //  std::cout << "FAIL: Could not get frame data. (first iteration)\n" << std::endl;
  //  return;
  //}
  //if (!emscripten_vr_submit_frame(current_display)) {
  //  std::cout << "FAIL: failed to submit frame to VR display " << current_display << "(first iteration)" << std::endl;
  //  return;
  // }
  return;
  }
  
  splitter_iter2((void*)spl);
}

void vr_run2(Splitter *spl2)
{
  //if (vr_vr_ready) {
    //} else {std::cout << "vr_vr_ready is not done early enoguh!" << std::endl;


    Splitter_arg *arg = new Splitter_arg;
    arg->display = NULL;
    arg->spl = spl2;
    emscripten_set_main_loop_arg(splitter_iter3, (void*)arg, 0,1);
}

#endif



#endif

class HMDRequestPresentingCallback : public Fetcher<int>
{
public:
  HMDRequestPresentingCallback(Fetcher<int> *fetcher) : fetcher(fetcher) { }

  virtual void event(MainLoopEvent &e) { fetcher->event(e); }
  virtual void frame(MainLoopEnv &e) { fetcher->frame(e); }
  virtual void draw_event(FrameLoopEvent &e) { fetcher->draw_event(e); }
  virtual void draw_frame(DrawLoopEnv &e) { fetcher->draw_frame(e); }
  virtual void set(int t) { fetcher->set(t); }
  virtual int get() const
  {
    int val = fetcher->get();
    if (val==1 && oldval==0) {
#ifdef EMSCRIPTEN      
      std::cout << "Trying request presenting" << std::endl;
      VRLayerInit init = { NULL, VR_LAYER_DEFAULT_LEFT_BOUNDS, VR_LAYER_DEFAULT_RIGHT_BOUNDS };

      if (!emscripten_vr_request_present(current_display, &init, 1, requestPresentCallback, NULL)) {
	std::cout << "FAIL: request_present with default canvas failed." << std::endl;
      } else {
      std::cout << "OK: Request presenting success" << std::endl;
      }
#endif
    }
    oldval = val;
    return val;
  }

private:
  mutable int oldval;
  Fetcher<int> *fetcher;
};

GameApi::IF GameApi::FontApi::hmd_request_presenting(IF i)
{
  Fetcher<int> *k = find_int_fetcher(e,i);
  return add_int_fetcher(e, new HMDRequestPresentingCallback(k));
}

class HMDStateIntFetcher : public Fetcher<int>
{
public:
  HMDStateIntFetcher() {}
  virtual void event(MainLoopEvent &e)
  {
  }
  virtual void frame(MainLoopEnv &e)
  {
  }
  virtual void draw_event(FrameLoopEvent &e)
  {
  }
  virtual void draw_frame(DrawLoopEnv &e)
  {
  }
  virtual void set(int t) { }
  virtual int get() const
  {
    // TODO: maybe this should use emscripten_vr_ready() && emscripten_vr_display_presenting(current_display) ?
#ifdef VIRTUAL_REALITY
#ifndef EMSCRIPTEN    
    if (!hmd) return 0; else return 1;
#else
    if (!vr_vr_ready  && current_display != 0 && current_display!=-1) return 0; else return 1;
#endif
#endif
    return 0;
  }
};

GameApi::IF GameApi::FontApi::hmd_state_fetcher()
{
  return add_int_fetcher(e, new HMDStateIntFetcher());
}
#endif
