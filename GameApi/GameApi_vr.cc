/*
*/

#include "GameApi_h.hh"

#include <GL/glew.h>
#include <SDL_opengl.h>

#ifdef VIRTUAL_REALITY
#ifndef EMSCRIPTEN
#include "openvr/openvr_mingw.h"
#else
#include <emscripten/vr.h>
#endif


EXPORT GameApi::RUN GameApi::BlockerApi::vr_window(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration, bool invert, bool translate)
{
  ML I43 = ev.mainloop_api.setup_hmd_projection(ev,ml,false,10.1,60000.0,false);
  TXID I44 = ev.fbo_api.fbo_ml(ev,I43,1080,1200,false);
  ///
  ML I66 = ev.mainloop_api.setup_hmd_projection(ev,ml,true,10.1,60000.0, false);
  TXID I67 = ev.fbo_api.fbo_ml(ev,I66,1080,1200,false);
  ML I68 = ev.blocker_api.vr_submit_ml(ml, I44,I67,invert,translate);
  RUN I69 = ev.blocker_api.game_window2(ev,I68,logo,fpscounter,start_time,duration);
  return I69;
}
vr::IVRSystem *hmd = 0;
Matrix hmd_pose = Matrix::Identity();
#ifdef EMSCRIPTEN
VRDisplayHandle *current_display;
bool emscripten_vr_ready = false;
void vr_cb(void *arg)
{
  emscripten_vr_ready = true;
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
  if (hmd) {
    vr::TrackedDevicePose_t pose[ vr::k_unMaxTrackedDeviceCount ];
    vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL,0);
  }
#else
  if (!emscripten_vr_ready) {
    int val = emscripten_vr_init(&vr_cb, NULL);
    if (val==0) { std::cout << "Emscripten VR not found!" << std::endl;
    }
#endif
}
class SubmitML : public MainLoopItem
{
public:
  SubmitML(MainLoopItem *item, TextureID *left, TextureID *right, bool invert, bool translate) : item(item), left(left), right(right), invert(invert), translate(translate) {
    firsttime = true;
  }
  virtual void execute(MainLoopEnv &e)
  {
    if (firsttime) {
      check_vr_compositor_init();
      firsttime = false;
    }
    item->execute(e);
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
    if (emscripten_vr_ready) {
	emscripten_vr_submit_frame(current_display);
      
      //VRDisplayCapabilities cap;
      //int emscripen_vr_get_eye_parameters( display, &cap );
      
      VRFrameData d;
      int val = emscripten_vr_get_frame_data( current_display, &d);
      if (d.pose.poseFrags & VR_POSE_ORIENTATION) {
	VRQuarternion q = d.pose.orientation;
	Quarternion q2;
	q2.x = q.x; q2.y = q.y; q2.z = q.z; q2.w = q.w;
	hmd_pose = Quarternion::QuarToMatrix(q2);
      }
    }
#endif
  }
  Matrix ConvertMatrix( const vr::HmdMatrix34_t &pose)
  {
    Matrix m = { pose.m[0][0], pose.m[0][1], pose.m[0][2], 0.0 /*pose.m[0][3]*/,
		 pose.m[1][0], pose.m[1][1], pose.m[1][2], 0.0 /*pose.m[1][3]*/,
		 pose.m[2][0], pose.m[2][1], pose.m[2][2], 0.0 /*pose.m[2][3]*/,
	         0.0, 0.0,0.0, 1.0f, false };
    return m;
  }

  virtual void handle_event(MainLoopEvent &e)
  {
    left->handle_event(e);
    right->handle_event(e);
    item->handle_event(e);
  }
  virtual int shader_id() { return item->shader_id(); }
private:
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
};
EXPORT GameApi::ML GameApi::BlockerApi::vr_submit_ml(ML ml, TXID left, TXID right, bool invert, bool translate)
{
  MainLoopItem *item = find_main_loop(e, ml);
  TextureID *left_eye = find_txid(e, left);
  TextureID *right_eye = find_txid(e, right);
  return add_main_loop(e, new SubmitML(item, left_eye, right_eye, invert, translate));
}
EXPORT GameApi::RUN GameApi::BlockerApi::vr_submit(EveryApi &ev, TXID left, TXID right)
{

P I1=ev.polygon_api.vr_fullscreen_quad(ev,false);
MT I2=ev.materials_api.textureid(ev,left,1.0);
ML I3=ev.materials_api.bind(I1,I2);
P I4=ev.polygon_api.vr_fullscreen_quad(ev,true);
 MT I5=ev.materials_api.textureid(ev,right,1.0);
ML I6=ev.materials_api.bind(I4,I5);
ML I7=ev.mainloop_api.array_ml(std::vector<ML>{I3,I6});
RUN I8=ev.blocker_api.game_window2(ev,I7,false,false,0.0,100000.0);
 return I8;
}

class PoseMovement : public Movement
{
public:
  PoseMovement(Movement *next) : next(next) { }
  virtual void event(MainLoopEvent &e) { if (next) next->event(e); }
  virtual void frame(MainLoopEnv &e) { if (next) next->frame(e); }
  void set_matrix(Matrix m) { }
  Matrix get_whole_matrix(float time, float delta_time) const
  {
    return next->get_whole_matrix(time,delta_time)*hmd_pose;
  }
private:
  Movement *next;
  bool eye;
};
EXPORT GameApi::MN GameApi::MovementNode::pose(MN next)
{
  Movement *nxt = find_move(e, next);
  return add_move(e, new PoseMovement(nxt));
}

class HMDProjection : public MainLoopItem
{
public:
  HMDProjection(GameApi::Env &env, GameApi::EveryApi &ev, MainLoopItem *item, bool eye, float near, float far, bool translate) : env(env), ev(ev), item(item), eye(eye), m_fNearClip(near), m_fFarClip(far), translate(translate) {}


Matrix GetHMDMatrixPoseEye( bool eye )
{
#ifndef EMSCRIPTEN
  if (!hmd) { return Matrix::Identity(); }
  vr::Hmd_Eye nEye = eye ? vr::Eye_Left : vr::Eye_Right;
  vr::HmdMatrix34_t mat = hmd->GetEyeToHeadTransform( nEye );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0, false };

#else
  if (emscripen_vr_ready) {
    VRFrameData d;
    int val = emscripten_vr_get_frame_data(current_display, &d);
    if (!val) { std::cout << "vr_get_frame_data invalid handle" << std::endl;
      Matrix m;
      if (!eye) {
	for(int i=0;i<16;i++) m.matrix[i] = d.leftViewMatrix[i];
      } else {
	for(int i=0;i<16;i++) m.matrix[i] = d.rightViewMatrix[i];
      }
      m.is_identity = false;
    } else { m=Matrix::Identity(); }
#endif
  
  //std::cout << "Pose:" << std::endl;
  //for(int i=0;i<16;i++)
  //  std::cout << m.matrix[i] << ",";
  //std::cout << std::endl;
  //m = Matrix::Inverse(m);

  return m;
}

Matrix GetHMDMatrixProjectionEye( bool eye )
{
#ifndef EMSCRIPTEN
  if (!hmd) { return Matrix::Identity(); }
  vr::Hmd_Eye nEye = eye ? vr::Eye_Left : vr::Eye_Right;
  vr::HmdMatrix44_t mat = hmd->GetProjectionMatrix( nEye, m_fNearClip, m_fFarClip );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3], false };


#else
  VRFrameData d;
  int val = emscripten_vr_get_frame_data(current_display, &d);
  if (!val) { std::cout << "vr_get_frame_data invalid handle" << std::endl;
  Matrix m;
  if (!eye) {
    for(int i=0;i<16;i++) m.matrix[i] = d.leftProjectionMatrix[i];
  } else {
    for(int i=0;i<16;i++) m.matrix[i] = d.rightProjectionMatrix[i];
  }
  m.is_identity = false;
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
    sh_color.id = e.sh_color;
    sh_texture.id = e.sh_texture;
    sh_texture_2d.id = e.sh_texture_2d;
    sh_array_texture.id = e.sh_array_texture;
    vertex.id = e.us_vertex_shader;
    fragment.id = e.us_fragment_shader;

    Matrix proj_matrix; Matrix pos_mat; Matrix scene_translate = Matrix::Identity();
    proj_matrix=GetHMDMatrixProjectionEye( eye );
    pos_mat= GetHMDMatrixPoseEye( eye );
    Matrix proj_m = proj_matrix * scene_translate * pos_mat;
    GameApi::M proj = add_matrix2( env, proj_m );
    Matrix id_m = Matrix::Identity();
    GameApi::M id = add_matrix2( env, id_m );
    ev.shader_api.set_var(sh_color, "in_P", proj);
    ev.shader_api.set_var(sh_texture, "in_P", proj);
    ev.shader_api.set_var(sh_texture_2d, "in_P", proj);
    ev.shader_api.set_var(sh_array_texture, "in_P", proj);
    ev.shader_api.set_var(vertex, "in_P", proj);
    ev.shader_api.set_var(fragment, "in_P", proj);

    ev.shader_api.set_var(sh_color, "in_T", id);
    ev.shader_api.set_var(sh_texture, "in_T", id);
    ev.shader_api.set_var(sh_texture_2d, "in_T", id);
    ev.shader_api.set_var(sh_array_texture, "in_T", id);
    ev.shader_api.set_var(vertex, "in_T", id);
    ev.shader_api.set_var(fragment, "in_T", id);

    MainLoopEnv ee = e;
    ee.in_T = id_m;
    glEnable( GL_MULTISAMPLE );
    item->execute(ee);
    glDisable( GL_MULTISAMPLE );
    
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
  float m_fNearClip, m_fFarClip;
  bool translate;
};

EXPORT GameApi::ML GameApi::MainLoopApi::setup_hmd_projection(EveryApi &ev, ML ml, bool eye, float nnear, float nfar, bool translate)
{
  MainLoopItem *item = find_main_loop(e, ml);
  return add_main_loop(e, new HMDProjection(e,ev,item, eye, nnear,nfar, translate));
}
#ifdef EMSCRIPTEN
void splitter_iter2(void *arg);
struct Splitter_arg
{
  VRDisplayHandle* display;
  Splitter *spl;
};
void splitter_iter3(void *arg)
{
  Splitter_arg *a = (Splitter_arg*)arg;
  current_display = a->display;
  Splitter *spl = a->spl;
  splitter_iter2((void*)spl);
}

void vr_run2(Splitter *spl2)
{
      if (emscripten_vr_ready) {
      int s = emscripten_vr_count_displays();
      for(int i=0;i<s;i++) {
	VRDisplayHandle *display = new VRDisplayHandle;
	*display = emscripten_vr_get_display_handle(i);
	const char *name = emscripten_vr_get_display_name( *display );
	std::cout << "VR Display #" << i << ": " << name << std::endl;
	Splitter_arg *arg = new Splitter_arg;
	arg->display = display;
	arg->spl = spl2;
	emscripten_vr_set_display_render_loop_arg(display, &splitter_iter3, (void*)&arg);
      }
  emscripten_set_main_loop_arg(splitter_iter2, (void*)spl2, 0,1);
}
#endif
#endif
